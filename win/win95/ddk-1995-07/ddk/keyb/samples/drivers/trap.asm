;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

        page    ,132

;*** TRAP.ASM for Microsoft Windows 4.00  keyboard drivers ******************
;
;       This contains the hardware interrupt handler (keybd_int)
;       for the Windows keyboard driver.
;
;       NOTE: BIGSTK may or may not be defined here; see below.
;
;       NOTE: NOKIA removed at DavidW's request.
;
; 2/95  NOTE: A fundamental change was made to trap so a backup version was
;             also checked-in as trapold.asm (a-cliffe/Davidms). This fixes
;             the problem of "stuck-ALT or SHIFT or CTRL" conditions due to
;             loss of the first byte of a multibyte sequence (i.e. E0 or E1
;             on extended keys). These lost interrupts are now counted to make
;             future debugging easier.
;
;              - STI instruction placed after call to kbd_proc
;              - LastCursor treated as a semaphore so CLI/STI added to provide
;                for atomic operations on that flag
;              - A shift-DOWN is followed immediately by shift-UP if a nested
;                interrupt condition arises.
;
;       CAUTION: this fix for lost interrupts improves the situation, but may
;                not necessarily eliminate the problem altogether (although it
;                would now be a very rare event). This seems the better part of
;                wisdom in order to keep the changes to the driver too a minimum.
;                Proper handling of nested interrupts should be attempted at some
;                future release.
;
;***********************************************************
;       Conditional-assembly flags:
;
;       Only ONE of the following should be set in the command line!
;
;       ENHANCE handles 'RT' 101/102-key enhanced keyboard.
;
;       ENVOY   Handles Hewlett-Packard keyboards, including Envoy and
;               enhanced.
;
; ========================================================================
; Keyboard interrupt handler.
;
; Calls USER with....
;
; ENTRY: AL = Virtual Key Code, AH = 80 (up), 00 (down)
;	 BL = Scan Code
;	 BH = 0th bit is set if it is an enhanced key(Additional return etc.,).
;        SI = LOWORD of ExtraInfo for the message
;	 DI = HIWORD of ExtraInfo for the message
;
;
; ========================================================================
; display which option(s) we've chosen.


%out
%out    .... TRAP.ASM -- Windows keyboard INT 09H ....
%out    .... Keyboards handled besides XT ....


        %out .  Handles Enhanced, XT, AT keyboards

; ========================================================================

include keyboard.inc
include vkwin.inc
include vkoem.inc

;*;
;*; include file for LCID offsets
;*;
include lcid.inc

ifndef DosXMacro
    DosXMacro macro x, y

    endm
endif

; Allocate big stack when calling
; BIGSTK equ 0


SYSREQ  =       054h    ; PC-AT SYS REQ key .. not on M24 or ICO keyboard
                        ; (on Enhanced keyboard, is PrintScn/SysReq)
    ifdef SYSREQ
        %out .. Has SysReq handling
    else
        %out .. Does NOT have SysReq handling
    endif

; scan codes for special keys

cEsc            EQU      1      ;
cReturn         EQU     28

cCtrl           EQU     29
cLShift         EQU     42
cSlash          EQU     53      ; 35h
cRShift         EQU     54      ; 36h
cPrint          EQU     55      ; 37h IBM keyboard printscreen
cAlt            EQU     56      ; 38h
cCapsLock       EQU     58      ; 3ah
cF1             equ     59      ; 3bh F1 key on anything
cF7             equ     65      ; 41h F7 key on anything
cNumLock        EQU     69      ; 45h
cBreak          EQU     70      ; 46h
cUp             equ     72      ; 48h up key
cLeft           equ     75      ; 4bh left key
cRight          equ     77      ; 4dh right key
cDown           equ     80      ; 50h down key
cDelete         EQU     83

cExtended       EQU     96      ; E0h-80h - for RT extended codes - 13feb87
cExtended1      equ     97      ; E1h-80h - prefix for Pause key

cCommand        EQU     0F0H


;*****************************************************************************
;*****************************************************************************
;************************ data segment begins here ***************************
;*****************************************************************************
;*****************************************************************************

sBegin DATA

    externB fKeyType            ; flags mainly for RT keyboard.

;
; These things are updated at INIT or ENABLE time from the keyboard
; tables or from WIN.INI.
;
externB     PCType              ; identifies system type
externB     PCTypeHigh
externB     KeyType             ; keyboard type (usually == TableType)
externB     IsEri
externB     IsOli
externB     OliType
externB     NumLockFlag         ; Flag for change in BIOS NumLock flag.
externW ScanCodes
externW VirtKeys
externA KBD_SCANS
externA	SCAN_TO_VKEY_OFFS
externW	pCurrentLocale

; This is the translation for the numeric pad when NUMLOCK is set.
; This is the same for all keyboards, and is fixed in length.

        public  KeyNumTrans
KeyNumTrans     LABEL   BYTE
        DB      VK_NUMPAD7      ; numpad-7
        DB      VK_NUMPAD8      ; numpad-8
        DB      VK_NUMPAD9      ; numpad-9
        DB      VK_SUBTRACT     ; numpad-minus
        DB      VK_NUMPAD4      ; numpad-4
        DB      VK_NUMPAD5      ; numpad-5
        DB      VK_NUMPAD6      ; numpad-6
        DB      VK_ADD          ; numpad-plus
        DB      VK_NUMPAD1      ; numpad-1
        DB      VK_NUMPAD2      ; numpad-2
        DB      VK_NUMPAD3      ; numpad-3
        DB      VK_NUMPAD0      ; numpad-0
        DB      VK_DECIMAL      ; numpad-period

globalW keyNumBase <dataOffset KeyNumTrans>

extrn   AckByte:byte                    ; Value of acknowledge byte
EXTRN   CVWBreak:FAR                    ; Kernel routine for CVW handling
externB fSysReq                         ; Flag for enabling SysReq key

;*****************************************************************************
;******************************** Local data *********************************
;*****************************************************************************
;
; Address of keyboard event-proc
;
; the keyboard event_proc is called with the following parameters:
;
;       ah == 80h for uptrans, 00h for downtrans
;       al == virtual key
;       bh == 0 if no prefix byte, 1 if E0 prefix byte preceded this scancode.
;       bl == scan code

        even

globalD event_proc, 0                   ; Addr of enabled kbd event procedure

; (MS code has ShiftLockException table here)

fBreak      DB  0
fReEnter    DB  0       ; reentrancy flag

fAltGrDn    db  0       ; set if AltGr is down -- for preventing repeats.

LastCursor  db  0        ; last NUMERIC PAD cursor scancode read
                         ; with NumLock and SHIFT on.
NestedCount db  0        ; count nested interrupt occurances

Partial_faile0 db 0      ;count e0 failures.

Partial_faile1 db 0      ;count e1 failures.

GlobalB ShiftCollision 0 ; count these if they occur.

; Address of routine called to handle a scan code.
; For non-RT keyboards, this never changes -- it's always 'standard_proc'.
; For RT keyboards, this changes when a prefix code is seen

staticW kbd_proc,standard_proc

fExtend db      0               ; flag for above keys.

globalD bios_proc, 0
globalD nmi_vector, 0

sEnd DATA

;*****************************************************************************
;*****************************************************************************
;******************************* CODE SEGMENT ********************************
;*****************************************************************************
;*****************************************************************************
;
;       Some pointer variables in the CODE segment.
;       These are initialized in ENABLE, using
;       a DATA alias to the CODE segment, so that
;       they can be written in protect mode.
;

sBegin CODE                             ; CODE segment now.

    assumes     CS,CODE
    assumes     DS,DATA

;*****************************************************************************
;******************************* CVWHackCall *********************************
;*****************************************************************************

    hack_for_old_cvw        dw  offset CVWHackCall
                            dw  seg CVWHackCall

cProc   CVWHackCall, <FAR,PUBLIC>
cBegin <nogen>

    push    ds                          ; save DS
    push    ax                          ; ... AX
    mov     ax, _DATA                   ; reload DS
    mov     ds, ax
    assumes ds, data
    pop     ax                          ; restore AX
    push    word ptr [bios_proc][2]
    push    word ptr [bios_proc][0]     ; put address of bios proc on stack
    push    bp
    mov     bp, sp
    mov     ds, [bp+6]                  ; restore ds
    pop     bp
    retf    2                           ; "return" popping ds
                                        ; affecting a jump to bios
cEnd   <nogen>


;*****************************************************************************
;****************************** GetBIOSKeyProc *******************************
;*****************************************************************************

cProc   GetBIOSKeyProc, <FAR, PUBLIC, PUBLIC, LOADDS>
cBegin
    mov     ax, word ptr [bios_proc][0]
    mov     dx, word ptr [bios_proc][2]
cEnd

    assumes     DS,nothing


;*****************************************************************************
;************************ Kbd Hardware service routine ***********************
;*****************************************************************************
;
;       Usage of segment registers:
;
;       Currently:              In 2.10 and earlier drivers:
;
;       CS = CODE               (was DATA)
;       DS = DATA               (was BIOS data area at 40h)
;       ES = RAMBIOS (40h)      (wasn't used)
;
public keybd_int
keybd_int   PROC    FAR


        push    ds
        push    ax                      ; save registers we use
        push    es

        mov     ax, _DATA               ; What Cmacros does to 'DATA'
        mov     ds,ax                   ;  .. load DS
        mov     ax, RAMBIOS             ; BIOS RAM segment at 40h
        mov     es,ax                   ;  .. point ES to that!
    assumes DS, DATA
    assumes ES, RAMBIOS

        in      al,kb_data              ; get the scan code
        mov     ah,es:[kb_flag]         ; get current shift state..

        cmp     al,cCommand             ; is it a keyboard command?
        jb      kbiTestCursor           ; (note: cCommand is different
                                        ;  for Ericsson!)
        jmp     kbToBios                ; if so, let BIOS do it

kbiTestCursor:
        cmp     LastCursor, 0	; is flag (scancode) set for numpad
        jz      kbiTestUp       ; cursor?
        cmp     al, LastCursor  ; yes, is this the same scancode?
        jne     kbiTestCursorUp ; no, might be same key, different state
        jmp     kbi1            ; yes, so it's a MAKE of the same one
;
; Determine if the same scancode but different state - it might be a
; breaking key of the same scancode.
;
kbiTestCursorUp:                ; no... (was not same scancode)
        push    ax              ; it might be BREAK for same code
        and     al,7fh          ; remove key state (80H == break)
        cmp     LastCursor,al   ;
        pop     ax              ;
        je      kbiTestUp       ; if ==, it is a break of same scancode
;
;  What we have here is a reentrancy rarity: the interrupt belongs to a key
;  that is not the same as the last NUMPAD cursor key. By definition NUMLOCK
;  is down (ON) here, and so is shift, but we are faking right-shift-UP to
;  USER if numpad cursor keys are being used. There is logic in standard_proc
;  for numeric keypad cursor keys which sets LastCursor.
;
;  We simulate SHIFT-down to USER and let standard_proc decide on the key's
;  coming impact (including call to SetShiftState). If shift is breaking it will
;  generate a SHIFT-up message to USER to counter this message, because we will
;  clear LastCursor here and now. If a different key then we let standard_proc
;  potentially reconstitute the numpad processing logic. It appears safer to
;  assume the shift-down condition and let further processing counter this
;  assumption.
;
public KbiCollision
KbiCollision:
;
        push    ax               ; but it's not break, so we
        push    bx               ; save original key info and
        mov     ax, VK_SHIFT     ; prepare to fake SHIFT-down
        mov     bx,54
        mov     LastCursor, 0    ; clear flag (protected by CLI)
                                 ; do now in case USER does STI
        call    ds:[event_proc]  ; fake SHIFT DOWN message
                                 ; USER had better not enable interrupts.
;
;  ** Problem ** if the reentrancy flag (fReEnter) is set then standard_proc
;  is not called and the shift may get stuck down, so we conditionalize the
;  calling of USER. Note: we believe the first SHIFT-down message must be sent
;  regardless because numpad cursor keys use a faked shift-up condition. that
;  must be neutralized. Later, in standard_proc when the interrupt system in ON
;  there is more carefull attention paid to reentrancy.
;
        test    ds:[fReEnter], 0FFH  ; is the field crowded?
        je      kbiTestUp1           ; no, try to reach standard_proc
;
;  Windows preserves all registers. Assume
;
        inc     ShiftCollision         ; record this rare event
        mov     ax, VK_SHIFT+8000H     ; fake SHIFT UP
        mov     bx,54                  ;
        call    ds:[event_proc]
        call    SetShiftState          ; correct the key state

kbiTestUp1:
        pop     bx          ; restore original key info
        pop     ax          ; and atempt to send it

;-------------------------------------------------------------------

kbiTestUp:
        test    al,80h                  ; is it an up transition?
        jz      notkbi1                 ; .. if not, continue
        jmp     kbi1                    ; it's an UP transition, jump..
notkbi1:                                ; it's a DOWN transition..

        ; this will make BIOS clear fHold if it's set:

        test    byte ptr es:[kb_flag_1],fHold   ; in hold state?
        jnz     jkbToBios               ; if so, jump to ROM

        cmp     al,cCapsLock            ; is it CapsLock ?
        jnz     kbib                    ; no...

kbic:                               ; it's CapsLock --
        test    ah,fCtrl                ; is it Ctrl-NumLock or -CapsLock?
        jnz     jkbToBios               ; yes, jump to ROM
        jmp     kbi1                    ; no, go check hotkey
jkbToBios:
        jmp     kbToBios


; It's not capslock
; [AH] = BIOS shift state, [AL] = scan code,
; If it's Delete or Break, go check for Ctl-Alt


kbib:

        cmp     al,cDelete
        je      kbia
        cmp     al,cBreak
        je      kbia

; OLD Screen Print code here deleted 14 dec 88 .. see screen-capture code
; at KBI1:

; kbia: check for Control ALT something here
; Scan code is one of
;
;       cDelete         (Reset)
;       cBreak          (interrupt)
;       cPrint          (Screen Grab)
;
;       cCCPDel         - Vectra
;       cBackSlash      - Vectra
;       cMinus          - Vectra
;       cPlus           - Vectra
;
;
kbia:
        mov     ah,es:[kb_flag]         ; Get BIOS shift state (again)
        not     ah
        test    ah,fAlt+fCtrl           ; test for CTRL-ALT something...
        jnz     kbi1                    ; nope, go to hotkey check


; Control and Alt are BOTH down now.  We're going to BIOS
; if its cDelete..

        cmp     al,cDelete
        jne     kbi1

; It's control-alt-Delete ..
; Inform the kernel that Ctl+Alt+Del is happening so that he can inform
; any expanded memory card to reset. (Wed 21-Oct-1987 : bobgu)
; Not needed as of 5/21/93 DavidDS.  Handled by Win386 etc.
;       call    KbdRst

kbToBios:

        pop     es
        pop     ax

        push    word ptr [bios_proc][2]
        push    word ptr [bios_proc][0]     ; put address of bios proc on stack
        push    bp
        mov     bp, sp
        mov     ds, [bp+6]                  ; restore ds
        assumes DS,NOTHING
        pop     bp
        retf    2                           ; "return" popping ds

kbi1:

;
; Reset the keyboard controller and acknowledge the interrupt.
;

        assumes DS,DATA

kbi13:

        push    ax                      ; this little bit is for XT-like systems
        in      al,kb_ctl               ; reset interface chip (8255)
        mov     ah,al
        or      al,80h
        out     kb_ctl,al
        xchg    ah,al
        out     kb_ctl,al
        pop     ax                      ; just for delay..

        ;; ??? ;;                       ; for ATs and PS2s, may need
                                        ;  to enable keyboard here!

        push    ax                      ; this is for XT or AT-like systems
        mov     al,ds:AckByte           ; acknowledge interrupt
        out     ack_port,al
        pop     ax

        mov     ah,80h                  ; move the high order bit to the high
        and     ah,al                   ; order byte.
        xor     al,ah                   ; turn off bit if set

        push    bx                      ; stack = [AX,DS,ES,BX]

; Check for print-screen.
;
; For 3.0 Windows:
;       ALT-printscreen is grab of current window.
;       unshifted printscreen is grab of whole screen.
;       (shift-printscreen on XT, AT keyboards, however).
;
; If this is detected, event_proc is called with VK = VK_SNAPSHOT
; and BX = 0 for current window, and 1 for full screen grab.
;
; AL contains scan code (hi bit is 0), AH contains up/down flag.
;
        mov     bl,es:[kb_flag]         ; get current shift state..
        test    bl,fCtrl                ; CTRL down? if so,
        jnz     kbiNotPrint             ; This is no printscreen...!


        ; this little bit is a check for ALT-PRINT (= ALT-SysReq) on
        ; enhanced keyboard.  What a pain!  the PRINT key gives unprefixed
        ; SYSRQ scancode if ALT is down, and we want ALT-PRINT for current
        ; window grab!
        cmp     ds:[KeyType],4          ; Enhanced?
        jne     kbiNoAltPrint
        cmp     al,SYSREQ               ; SYSREQ scan code?
        jne     kbiNoAltPrint           ; .. nope.
        test    bl,fAlt                 ; ALT down? (must be, but be patient)
        jz      kbiNoAltPrint
        mov     al,VK_SNAPSHOT          ; this is really a screen grab
        mov     bx,0                    ;  0: current window
        jmp     short kbiPrintSend      ;

kbiNoAltPrint:
        cmp     al, cPrint              ; PrtScn scancode (multiply key)?
        jne     kbiNotPrint             ; (XT, AT, or Enhanced)

        cmp     ds:[KeyType],4          ; Enhanced keyboard?
        jne     kbiIsPrintKey           ; if not, skip the following;
                                        ; if so, don't need shift..
                                        ; and check/restore kbd_proc..
        cmp     ds:[kbd_proc], codeOFFSET prev_was_ext_proc
        jne     kbiNotPrint             ; MUST have E0 prefix for CTRL PRINT
        mov     ds:[kbd_proc], codeOFFSET standard_proc

        ; it is an enhanced-keyboard printscreen key, so..
        test    bl,fShift               ; is SHIFT down?
        jnz     kbiGoIgnore             ; if so, don't do it.
        mov     al,VK_SNAPSHOT
        mov     bx,1                    ; full-screen grab: bx == 1
        jmp     short kbiPrintSend

    kbiIsPrintKey:

        ; We have Printscreen scancode on non-enhanced keyboard.
        ; We look for SHIFT or ALT, and snap all or part of the screen.
        ; (unshifted PrintScreen is '*' character in this case)
kbiCheckPrint:
        test    bl,fShift+fAlt          ; is SHIFT or ALT down?
        jz      kbiNotPrint
kbiCheckIsAlt:
        mov     al,VK_SNAPSHOT          ; send special snapshot VK code.
        test    bl,fAlt                 ; is ALT down?
        mov     bx,1                    ; set BX, but don't change Z flag!!
        jz      kbiPrintSend            ; ALT-PRINT has SysRq scancode, so skip
        dec     bx                      ; if ALT, clear BX

kbiPrintSend:
        call    ds:[event_proc]         ; send VK_SNAPSHOT !!
kbiGoIgnore:
        jmp     ignorekey               ; .. no more processing

kbiNotPrint:

;
;  The (PC-AT) SYS REQ key is used to simulate an NMI.
;
;  This is done by clearing up the stack and doing a far jump to the NMI
;  interrupt vector location.  If the NMI interrupt points into the ROM,
;  we don't jump, since the ROM treats NMIs as parity errors.
;
;  When SymDeb returns from the NMI, it returns to wherever CS:IP was
;  when the keyboard interrupt happened!
;
; Depending on the system and keyboard, SysReq requires that ALT or
; both Control and ALT be down, when the scan code 54h is input.
;
; On the Olivetti M24, this is the scan code for the 00 key.
;
; On Enhanced (RT) and ICO (102-key Olivetti M24) keyboards, interpret
; Control-Alt-PrintScreen or Control-Alt-00 as Sys Req.
; We do a little arithmetic on the ifdef's here...
;
; Additional test, required if this is an Enhanced or ICO (2) keyboard:
; Must use CTRL ALT SCR-PRT on these keyboards, since Windows uses
; ALT SCR-PRT for screen grab.


        cmp     al,SYSREQ               ; SYSREQ key?
        jne     notsys

        ; This handles Enhanced but NOT ICO keyboards:
        ; For Enhanced keyboards, this is the printscreen key, so
        ; we change the scan code if Ctrl is not down.

        cmp     ds:[KeyType], 4         ; skip the following test,
        jne     sysNotEnh               ;  if it's XT or AT keyboard.
                                        ; it IS an Enhanced keyboard
        test    byte ptr es:[kb_flag], fCtrl    ; is Ctrl down?
        mov     al, cPrint              ; change scan code
        jz      notsys
sysNotEnh:


sysreqwait:
        or      ah,ah                   ; Only on key-down
        js      sysreqdown              ; sign set if key-up
igkey:
        jmp     ignorekey

sysreqdown:

        cmp     fSysReq, 0              ; EnableKBSysReq() sets/resets this:
        jz      igkey                   ; can we break to debugger?

        ; we now assume NMI points to SymDeb , so we simulate the NMI.
        ; But first, we turn control [and Alt?] off, in both the BIOS
        ; and Windows!

        and     byte ptr es:[kb_flag], not (fCtrl+fAlt) ; do BIOS

        mov     ax,VK_MENU+8000h        ; ALT off in Windows
        mov     bx,38h                  ; was bl .. make sure bh 0
        call    ds:[event_proc]
        mov     ax,VK_CONTROL+8000h     ; CONTROL off in Windows
        mov     bx,1dh
        call    ds:[event_proc]

        ; now determine if we are to call off to int 2 or kernel!

        test    fSysReq,02

        ; now we simulate the NMI
        ; The code pointer nmi_vector must be in the CODE segment, since
        ; we want to restore ALL the other registers to what they were
        ; when the INT 09 interrupt happened, when we emulate the NMI.
        ; The Enable() function sets up nmi_vector.

        pop     bx
        pop     es
        pop     ax
        jnz     @F

        push   word ptr nmi_vector[2]
        push   word ptr nmi_vector[0]  ; address of nmi_vector on stack
        push   bp
        mov    ds, [bp+6]              ; restore ds
        pop    bp
        retf   2                       ; "return" to nmi_vector & pop ds

@@:     pop     ds
        jmp     CVWBreak                ; go to KERNEL!!
notsys:

; We are about to enable interrupt, before doing that we have to protect
; against another interrupt happening before we are done with this one.

        cmp     ds:[fReEnter],0         ; are we alone?
        jz      kbiR                    ; yes, continue

        inc     NestedCount             ; no, count the event
        jmp     ignorekey               ; is he typing REALLY fast?

kbiR:   inc     ds:[fReEnter]
    ;
    ;   In order for this to work in various 386 Virtual environments
    ;   just setting this "fReEnter" exclusion flag is not the right thing
    ;   to do. A 386 virtual machine monitor may be Simulating keyboard
    ;   activity to us and just doing this exclude will cause us to miss
    ;   lots of keys that the monitor is trying to send us because it will
    ;   send us keys as soon as we EOI the keyboard and enable interrupts.
    ;
    ;   We fix this problem by masking off the keyboard interrupt at the
    ;   interrupt controller. This prevents the 386 monitor from sending us
    ;   more keys till we're ready to get them. This works fine in the non-386
    ;   environments as well. This method is prefered over disabling the
    ;   keyboard at the Keyboard Controller because it is more portable.
    ;   There seems to be a fair variation amoung clones in the keyboard
    ;   controller used, but the keyboard is always IRQ 1, and the interrupt
    ;   controller is always at the IBM port addresses, and is an 8259.
    ;
    ;
        push    ax
        in      al,21h                  ; Get IRQ mask register
        or      al,02h                  ; mask off kybd ints (IRQ 1)
        jmp     $+2                     ; I/O delay for AT type machines
        jmp     $+2
        out     21h,al                  ; set new mask
        pop     ax

public kbiCall
kbiCall:
        xor     bh,bh
;;;     sti       ; interrupts on for others... (do this later!!)
;
; it is important to do the STI after the call and not rely on the
; hardware to ensure that the interrupt system remains off for the
; next instruction as well (namely the call). Nested interrupts can
; conceivably destroy the kbd_proc link before the previous call
; gets to execute.
;
        call    ds:[kbd_proc]   ; standard_proc or prev_was_ext_proc

        mov     ds:[fReEnter],0         ; unlock keyboard
;
; Re-enable keyboard INTs at the interrupt controller
;
        cli                       ; Ints off again.
        in      al,21h            ; get IRQ mask register
        and     al,NOT 02h        ; turn on kybd ints again (IRQ 1)
        jmp     $+2               ; I/O delay for AT type machines
        jmp     $+2
        out     21h,al            ; restore correct mask
        jmp     $+2               ; I/O delay for AT type machines

int_exit:
        pop     bx
        pop     es
        pop     ax
        pop     ds
        sti                       ; add for '286
        iret

;-----------------------------------------------------------------
Public ignorekey
ignorekey:
        cmp     ds:[fReEnter],0  ; are we alone?
        je      int_exit         ; yes, then just leave

        cmp     al, cExtended    ; scancode = e0?
        jne     int_exit1
        inc     partial_faile0   ; yes, count it
        jmp     int_exit         ;

int_exit1:                ;
        cmp     al, cExtended1   ; scancode = e1?
        jne     int_exit2        ;
        inc     partial_faile1   ; yes, count it

int_exit2:
        jmp    int_exit ; easy to differentiate from normal exits

keybd_int   ENDP

;*****************************************************************************
;******************************* standard_proc *******************************
;*****************************************************************************
;
; Standard routine for translating scan code to virtual keycode
; This is the default and the only one if the keyboard is NOT RT-like.
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;               BH=0
;
        Public standard_proc

standard_proc proc near

        sti
        xor     bh,bh                   ; Clear Extended key flag
        cmp     al,cNumLock             ; (See ICO code below)
        jne     notNumLock              ; but if it's NumLock, we
        inc     bh                      ; set it.
notNumLock:
        mov     fExtend, bh

        call    SetShiftState

; The following block of code is for drivers handling Enhanced keyboard
; only!

        cmp     ds:[KeyType],4          ; Enhanced?
        jne     noPrevExt
                                        ; this IS an Enhanced keyboard.
        cmp     al,cExtended1           ; E1 prefix for Pause key?
        jne     CheckForE0
        mov     ds:[kbd_proc],codeOFFSET pause_proc     ; yes, handle specially
        jmp     short j_stdproc_end

CheckForE0:
        cmp     al,cExtended            ; is this Extended scan code (E0)?
        jne     noPrevExt               ; if so, next keyboard int handled
        mov     ds:[kbd_proc],codeOFFSET prev_was_ext_proc      ; by this..
j_stdproc_end:
        jmp     stdproc_end

        Public noPrevExt

noPrevExt:

; code to handle ShiftLock.
;
; This code simulates pressing CapsLock when either the ShiftLock
; or either Shift key is pressed, depending on the state of the fCaps
; flag in kb_flag.
;
; To the Windows high-level code, and to the ToAscii() routine, this
; looks just like CapsLock.

        test    ds:[fKeyType],kbShiftLock       ; ShiftLock for this KB?
        jz      noSpecial               ; no, normal Caps Lock instead.

        cmp     al,cCapsLock            ; yes -- is this Capslock key?
        jnz     noCaps
        or      ah,ah                   ; up or down?
        js      shiftLockUp

        test    byte ptr es:[kb_flag], fCaps    ; shiftlock set?
        jnz     shiftLockUp             ; if not,
        call    FakeCapsLock            ; fake CAPSLOCK key going down/up

shiftLockUp:                            ; CapsLock up .. do nothing
        jmp     stdproc_end

noCaps:                                 ; not Caps Lock key
        cmp     al,cLShift              ; is (non-extended) Left Shift?
        je      isShift
        cmp     al,cRShift
        jnz     noSpecial
isShift:                                ; it's shift key, clear Caps Lock
        or      ah,ah                   ; is it make?
        js      shiftUp                 ; skip if not

        test    byte ptr es:[kb_flag], fCaps    ; is shiftlock flag set?
        jz      shiftUp                 ; if so,
        call    FakeCapsLock            ; fake CAPSLOCK key going down/up

shiftUp:

;; End of special code for RT keyboard ...

noSpecial:


; If the 2 shift keys are down, when the first is released we need
; to keep Shift State down, so we'll ignore the first Shift break.
; Remember: we already called SetShiftState that may have reset
;           the Shift state we are interested in, Yeerk !
;
        test    byte ptr es:[kb_flag],fShift
        jz      cbd0                    ; no Shift down
        or      ah,ah                   ; is it break ?
        jns     cbd0                    ; no
        cmp     al,cLShift              ; LeftShift ?
        jz      nosp1                   ; yes, ignore
        cmp     al,cRShift              ; RightShift ?
        jnz     cbd0                    ; no, skip
nosp1:  jmp     stdproc_end             ; yes, ignore
cbd0:

;
; Translate scan code to virtual key
; First, is it control-numlock or control-break?
;
        test    byte ptr es:[kb_flag],fCtrl     ; is it Ctrl?
        jz      kbi2

;; test for control-numlock:
        cmp     al,cNumLock
        jnz     kbi1a

        ; Control + Numlock translated to VK_PAUSE ..
        mov     bx,ax
        mov     al,VK_PAUSE
        jmp     kbi4                    ; (no longer short)

kbi1a:                                  ; it's NOT NumLock

        cmp     al,cBreak               ; is it Break?
        jnz     kbi2

        mov     ds:[fBreak],0
        mov     bx,ax
        test    ah,80h                  ; only signal for up transitions!
        jz      @F
@@:
        mov     al,VK_CANCEL
tokbi4:
        jmp     kbi4

kbi2:

CheckScanForVKey:
        public CheckScanForVKey
        push    di				; set registers and flags
        push    cx
        push    es
	pushf
	cld					; !!!!!!!
        push    ds
        pop     es

        mov     bx,     pCurrentLocale		; check base lcid list
        mov     di,     [bx+SCANTOIDX]
        mov     cx,     [bx+SCAN_SIZE]
        repnz   scasb
        jz      kbi2b

        mov     di,     dataOFFSET ScanCodes	; didnt find in pCurrent list,
        mov     cx,     KBD_SCANS		; try base list.
        repnz   scasb
        jnz     kbi2a

	add	di, SCAN_TO_VKEY_OFFS		; found vkey in base list
        jmp     kbi2c
kbi2a:
        mov     bl,     al			; didnt find vkey anywhere!
        mov     al,     -1
        jmp     kbi2d
kbi2b:
        sub     di,     [bx+SCANTOIDX]		; move from table to table
        dec     di
        add     di,     [bx+VKEYTOIDX]
kbi2c:
        mov     bl,     al			; put back scan code
        mov     al,     [di]			; get vkey
kbi2d:
        xor     bh,     bh			; clear extended bit

	popf					; restore flags
        pop     es
        pop     cx
        pop     di
        cmp     al,     -1
        je      kbi4

Translated:

;
;================================================
; ====== Handle keys on numeric pad ============
;================================================
;
CheckNumPad:

        xor     bh,bh         ; is it on numeric keypad?
        cmp     bl,71
        jb      kbi4
        cmp     bl,83
        ja      kbi4          ; nope!

public isNumPad
isNumPad:
        ; yes...  This is the numeric pad.
        ; Here, if NumLock is set, we change the virtual keycodes to
        ; numeric VK_NUMPAD codes, so the keys will be translated
        ; as numbers etc.  But if a shift key is down, we handle
        ; these as cursor keys, but we need to make sure that these
        ; are seen as UNSHIFTED

        test    byte ptr es:[kb_flag],fNum      ; is num lock down?
        jz      kbi4                    ; no, do normal processing

        test    byte ptr es:[kb_flag],fShift ; either shift key down?
        jnz     kbi3                    ; yes, keep as cursor keys
                                        ; no, treat as numeric keys.

        ; Now we prepare to translate as a NUMERIC key

        mov     al,bh
        or      al,al
        jnz     kbi4

        push    si
        mov     si,KeyNumBase

        mov     al,ds:[bx+si-71]        ; new: movable table
        pop     si

        jmp     short kbi4

; The key is on the numeric pad, NumLock is set, but a shift key is down
; (bit(s) set in BIOS key state), so we are going to keep this as a cursor
; key.  To do this, we need to make sure that Windows' state vector entry
; for VK_SHIFT is OFF even though a shift key is actually down. To protect
; against nested interrupt reentrancy problems we hit the interrupt flag,
; because LastCursor is used as a binary semaphore in keybd_int().

kbi3:
        cli                             ; protect numpad semaphore
        cmp     bl, LastCursor          ; are we repeating?
        mov     LastCursor, bl          ; save the scan code anyway
        sti
        je      kbi3a                   ; if so, we just send the key
                                        ; as a flag and turn off VK_SHIFT.
;
;  A SHIFT-up message is sent only once for every numpad cursor key that is
;  different from the previous one (repeats of the same key do not result in
;  repeated SHIFT-up messages).
;
;  ** Problem ** If the shift key is released (by means of a nested interrupt)
;  then SHIFT up/down messages would be out of sequence - the UP message here
;  is benign, but the premature DOWN message that would result is catered for
;  within keybd_int(). So we ignore the problem here, however, LastCursor was
;  treated as an atomic binary semaphore above.
;
        push    ax
        push    bx
        mov     ax,VK_SHIFT+8000H
        mov     bl,54
        call    ds:[event_proc]    ; fake shift UP.

        pop     bx
        pop     ax                 ; restore original key now and let
        jmp     short kbi4         ; USER see that one also.

;--------------------------------------------------------------------
;
; This is the same cursor key --  turn VK_SHIFT back on and
; clear the flag if the cursor key is being released (break)

kbi3a:
        test    ah,80h     ; break?  If not, keep shift OFF
        jz      kbi4       ; and send actual cursor key now.
;
; Repeating key is breaking so follow the cursor key message with a make
; believe shift key-down message.
;
; ** Problem ** A potential reentrancy problem may occur here from a nested
; interrupt call to standard_proc. Prior to standard_proc being called from
; keybd_int() the potential loss of SHIFT-up was catered for, however, if
; that occurred first then we would have an extra SHIFT-down message here
; without an accompanying UP message. We can tell by checking shift is still
; down. The best solution entails a short hit on the interrupt system.
;
        cli
        mov     LastCursor, 0                ; end of cursor logic
        test    byte ptr es:[kb_flag],fShift ; either shift key down?
        sti
        je     kbi4    ; no, we got hit - throw away actual cursor key.
;
; We send this SHIFT-down message knowing that keybd_int() could never send
; another SHIFT-down message inadvertently (because LastCursor = 0).
;
        call    ds:[event_proc]     ; send actual cursor key now...
        mov     ax,VK_SHIFT         ; pretend shift key went down
        mov     bl,54
;
kbi4:
        public kbi4
;
;-----------------------------------------------------------
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
; bh = 0 (except for ICO extended keys, and NumLock)
;
; Windows preserves all registers
;
        xor     bh,bh                   ; clear and
        xchg    bh,fExtend              ; get extended key flag
        call    ds:[event_proc]

stdproc_end:

        ret

standard_proc endp


;*****************************************************************************
;******************************* FakeCapsLock ********************************
;*****************************************************************************

; For Shift Lock, fake Caps Lock key being depressed.
; This is called when the caps lock or shift key is pressed,
; and the value of fCaps flag at 40:17h is to be changed.

        Public FakeCapsLock

FakeCapsLock proc near

        push    ax                      ; fake CAPSLOCK key going ..
        push    bx
        mov     bx,cCapsLock            ; load capslock scan code.
        mov     ax,VK_CAPITAL           ; .. down and ..
        call    ds:[event_proc]
        mov     ax,VK_CAPITAL+8000H     ; .. up.
        call    ds:[event_proc]
        pop     bx
        pop     ax                      ; continue processing SHIFT key..

        ret

FakeCapsLock endp



;*****************************************************************************
;***************************** prev_was_ext_proc *****************************
;*****************************************************************************
;
; prev_was_ext_proc -- used when previous was extended prefix (E0)
;
; used only by RT keyboard
;
;       Input   AL= Scancode (without make/break bit)
;               AH= 00 if make, 80h if break
;               BH=0
;
;       Mainly undoes Shifting and Unshifting generated internally
;       by this (@#$) keyboard and uses the extended prefixes to
;       distingush between normal keys and new (RT) keys.
;

        Public prev_was_ext_proc

prev_was_ext_proc proc near

        mov     ds:[kbd_proc], codeOFFSET standard_proc
        sti     ; enable interrupts for others

        cmp     al,cLShift          ; is it extended Left Shift ?
        jz      LeavePrevProc        ; if yes, eat it
        cmp     al,cRShift          ; else is it extended Right Shift ?
        jz      LeavePrevProc        ; if yes, eat it

; Test if we receive PrintScreen. Remember the make code has been eaten
; before we get a chance to be called. The break code will release the
; temporarily LeftShift that was then set.
        cmp     al,cPrint
        jnz     prev10

LeavePrevProc:                          ; Add label 04dec87
        jmp     prevproc_end

prev10:
        push    bx
        call    SetShiftStateNoNumlock
        pop     bx
        mov     bl,al                   ; bl= scancode (all along proc)
;
; Divide key special case: if Slash is found a VK_DIVIDE is sent
;
        cmp     al,cSlash               ; is it extended Slash ?
        jnz     prev20                  ; if not, skip
        mov     al,VK_DIVIDE            ; else send VK_DIVIDE
        jmp     short prev40

; When ShiftLock is on, we don't want to be get our VK_code shifted (none
; of our code correspond to alphanumeric), so we have nothing to do.
; We would have one exception (VK_DIVIDE), but we already took care of it.

; Break case
;
prev20:
        cmp     al,cBreak               ; is it extended Break ?
        jnz     prev30                  ; if not, skip
        mov     ds:[fBreak],0
        test    ah,80h                  ; only signal for up transitions!
        jz      @F
@@:
        mov     al,VK_CANCEL            ; always do VK_CANCEL
        JMP     SHORT PREV40            ; 07 sep 89.. don't stuff below..

; Standard case, convert to our virtual key
;
prev30:
        push    di
        push    es
        push    cx
	pushf
	cld
        push    ds
        pop     es

        lea     di, ScanCodes
        mov     cx, KBD_SCANS
        mov     al, bl              ; get the scan code
        repnz   scasb
        jz      @F
        mov     al, -1
        jmp     prev35
@@:
	add	di, SCAN_TO_VKEY_OFFS
        mov     al, [di]
prev35:
	popf
        pop     cx
        pop     es
        pop     di

        errn$   prev40  ; causes phase error if MASM size error
;
prev40:
;
; If flag is set for AltGr (right hand ALT) handled as control-ALT,
; then insert a CONTROL key call to event_proc before the ALT key call,
; but only if the BIOS CONTROL key flag is false (got that?).
; (15 oct 87: add code to prevent repeats on AltGr key, which were putting
; strange things in Windows' input buffer.)
;
        cmp     bl,cAlt                 ; was this an ALT key?
        jne     prev60                  ; skip on if not..


        test    ds:[fKeyType],kbAltGr   ; does this KB do this?
        jz      prev60                  ; skip on if not..
        test    byte ptr es:[kb_flag],fCtrl     ; is the real control key down?
        jnz     prev60                  ; if so, don't bother..
                                        ; the following code prevents repeats
        or      ah,ah                   ; on AltGr key depressions.
        jns     FakeAltGrDown           ; up or down?
        mov     fAltGrDn,0              ; up, clear flag
        jmp     short DoFakeAltGr       ; go fake control-Alt release

FakeAltGrDown:
        cmp     fAltGrDn,0              ; down, is this a repeat?
        jnz     prevproc_end            ; if so, ignore it.
        mov     fAltGrDn,1              ; otherwise fake control-Alt depression

DoFakeAltGr:
        push    ax                      ; OK, let's fake a Control key
        push    bx                      ; (but not set kb_flag bit!!)
        mov     al,VK_CONTROL           ; [AH] tells whether fake ctrl key
        mov     bx,cCtrl                ;  is going up or down.. (bh = 0)
        call    ds:[event_proc]         ; we have control (key)
        pop     bx                      ; get ALT key parameters back,
        pop     ax                      ; and now tell Windows about that...
;
; Windows preserves all registers
prev60:

; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bh = extended key flag, bl = scan code
;
        or      bh,1                    ; this flag indicates extended key.
        call    ds:[event_proc]

prevproc_end:

        ret

prev_was_ext_proc endp


;*****************************************************************************
;******************************** pause_proc *********************************
;*****************************************************************************
; Handle Extended keyboard Pause key.
; This is called for the 2 bytes following an E1 prefix.
; the whole scan code sequence is
;
;       E1 1D 45  E1 9D C5
;
; The shift state bits are not changed.
;
; Input         AL = Scancode
;               AH = 00 if make, 80h if break
;               bh = 0
;***********************************************************

pause_proc proc near

        sti
        cmp     al, cCtrl               ; first byte = 1D (control)?
        je      pause_proc_end          ; yes, just pass this on

        mov     ds:[kbd_proc], codeOFFSET standard_proc
        cmp     al, 45h                 ; second byte?
        jne     pause_proc_end

        xor     bh, bh                  ; clear enhanced flag
        mov     bl, al                  ; scan code in BL
        mov     al, VK_PAUSE            ; it's VK_PAUSE
        call    ds:[event_proc]

pause_proc_end:
        ret

pause_proc endp


;*****************************************************************************
;******************************* SetShiftState *******************************
;*****************************************************************************
;
;  Keep accurate track of shift state byte at 40:17H
;  For Alt and Control, we set or reset bits in 40:18 also.
;  For Numlock, the state bit is in 40:18, and the toggle bit is
;  in 40:17h.
;
;       AL:     scan code
;       AH:     sign bit indicates up/down
;
;       Uses BL or BX as mask.
;
        Public SetShiftState            ; public only for debug

SetShiftState proc near

        cmp     al,cNumLock             ; check for numlock
        jz      ccNum1

SetShiftStateNoNumlock:
        mov     bx,fLshift              ; check shift, ctrl, alt bits
        cmp     al,cLshift
        jz      ccv4
        mov     bx,fRshift
        cmp     al,cRshift
        jz      ccv4
        mov     bx,fCtrlW
        cmp     al,cCtrl
        jz      ccv4
        mov     bx,fAltW
        cmp     al,cAlt
        jnz     ccv6

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ccv4:
        DosXMacro       ds, Mod0
        or      ah,ah
        jns     ccv5
        not     bx
        and     word ptr es:[kb_flag],bx
        ret
ccv5:   or      word ptr es:[kb_flag],bx
ccv6:
        ret

; It's NumLock up/down.  This is similar to above, but keeps state in
; 40:18H, and toggles bit in 40:18H, so it's a little more complex.
; [BL] =  fNum is the appropriate bit in BOTH bytes.
; If CTRL is down, we do nothing.

ccNum1:
        test    byte ptr es:[kb_flag], fCtrl    ; (was fShift+fAlt+fCtrl)
         jnz    ccv6                            ; must have Ctrl off!
        mov     bl,fNum                         ; bit for numlock
        or      ah,ah
        jns     ccNum2
        not     bl                              ; upstroke, so just
        and     byte ptr es:[kb_flag_1],bl      ; clear state
        ret
ccNum2:                                         ; downstroke: already down?
        test    byte ptr es:[kb_flag_1],bl
        jnz     ccNum3
        xor     byte ptr es:[kb_flag],bl        ; no, so toggle the toggle bit,
        inc     ds:[NumLockFlag]                ; and set flag for ToAscii()

ccNum3:
        or      byte ptr es:[kb_flag_1],bl      ; set state bit.
        ret

SetShiftState endp

        public endTRAP
endTRAP:

if2
%out    .... END TRAP.ASM ....
%out
endif
sEnd CODE

END
