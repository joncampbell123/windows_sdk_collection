;***************************************************************************
;                                                                          *
;   Copyright (C) 1983,1984 by Microsoft Inc.                              *
;                                                                          *
;***************************************************************************

        TITLE   KEYBOARD - common code for IBM/PC keyboard driver

include keyboard.inc
;
; Double byte range values for the Far East.
; The values defined here are for the Rest Of the World.
;
BeginRange1     equ     255
EndRange1       equ     254
BeginRange2     equ     255
EndRange2       equ     254

SYSREQ  =       054h    ; PC-AT SYS REQ key

; scan codes for special keys

ShiftState  EQU 417H    ; rom bios data area of shift states
CrtMode     EQU 449H    ; rom bios data area for screen mode
KbFlag1     EQU 418H    ; rom bios data area for hold state
cCtrl       EQU 29
cLShift     EQU 42
cSlash      EQU 53
cRShift     EQU 54
cAlt        EQU 56
cCapsLock   EQU 58
cNumLock    EQU 69
cScrollLock EQU 70
cExtended   EQU 96
cCommand    EQU 0F0H

cDelete     EQU 83
cPrint      EQU 55
cBreak      EQU 70
cEsc	    EQU 01

fCaps   equ     01000000b
fNum    equ     00100000b
fScroll equ     00010000b
fAlt    equ     00001000b
fCtrl   equ     00000100b
fLshift equ     00000010b
fRshift equ     00000001b
fShift  equ     fLshift+fRshift
fHold   equ     00001000b


sBegin  DATA
;
; Define in a country specific module
;
externB <KeyNumTrans,keyTranslationTable>
externB <fAltGr>
EXTRN   KeyTransTblLen:ABS
EXTRN	KbdRst:FAR
;
; Keyboard information block
;
                PUBLIC  inquireData
inquireData     LABEL   BYTE
        DB      BeginRange1
        DB      EndRange1
        DB      BeginRange2
        DB      EndRange2

        DW      statesize        ; #bytes of state info
        DW      numFunc          ; How many Function keys are on keyboard
        DW      hasBreak         ; true => keyboard supplies make and break
        DW      intsPerSec       ; maximum rate of keyboard input events

; Where to save far procedure addresses
;
globalD  bios_proc,0   ; Saved address of BIOS hardware keyboard interrupt handler
globalD  bios1b_proc,0 ; Saved address of BIOS INT 1B.
staticD  LightsAddr,0  ; address of int 16h at boot time

staticD  event_proc,0  ; Address of enabled keyboard event procedure
staticD  nmi_vector,0  ; address of nmi vector

globalB  fKbRt,0       ; 1 if RT keyboard

ifdef DOSHIFTLOCK
globalB  fShiftLock,0  ; 1 if ShiftLock is on

; next table is not country dependent
; it contains the key scancode that will not be affected by ShiftLock on
; in the range from 2 to 35h.

; All prefixed scancodes will not be affected by ShiftLock.

labelB  ShiftLockException
	DB	0eh	; backspace
	DB	0fh	; tab
	DB	1ch	; Enter
	DB	1dh	; Ctrl
	DB	2ah	; Shift (left)
	DB	0	; end of table
endif
 
staticW  enabled,0
staticB  fBreak,0
staticB  fReEnter,0		; reentrancy flag
staticB  fOS2Box,0	; OS/2 Compatibility box
staticB  fIgnoreKey,0	; Set to ignore the keys used for screen switch
staticB  fIgnoreEsc,0	; Count of up keys to ignore
staticB  fSwitchEnable,1 ; Flag to prevent/allow screen switches

; Where is defined next proc to apply to incoming scan code.
; In case of a RT keyboard, the proc value will change to deal with long
; sequences.

staticW  kbd_proc,standard_proc


;-----------------------------------------------------
;
;--- keyboard hardware service -----------------------
;
;-----------------------------------------------------
;
cProc   keybd_int, <PUBLIC,FAR>
cBegin  nogen
assumes CS,DATA
        push    ax              ; save registers we use
        push    ds

        xor     ax,ax
        mov     ds,ax

        in      al,kb_data      ; get the scan code
        cmp     al,cCommand     ; is it a keyboard command?
        jae     kbi0
        test    al,80H                          ; is it an up transition?
        jnz     kbi1                            ; yes, process
        test    byte ptr ds:[KbFlag1],fHold     ; in hold state?
        jnz     kbi0                            ; yes, jump to ROM
;
; When running Windows it can happen we'll be interrupted by a Popup
; in text mode that will read keyboard (Business Network). This very
; weird case is detected by finding us in text mode that is quite
; uncommom when this keyboard driver is activated.

; However the OEM keyboard test (TKEYBD.EXE) is running in text mode,
; so this detection has to be removed when using this test program.
; 
ifndef  TEST
        cmp     byte ptr ds:[CrtMode],4         ; are we in text mode?
        jb      kbi0                            ; yes, jump to ROM
endif
        mov     ah,ds:[ShiftState]      ; get current shift states
        cmp     al,cNumLock             ; is it NumLock?
        jz      kbic
        cmp     al,cCapsLock            ; is it CapsLock ?
        jnz     kbib                    ; no...
kbic:
        test    ah,fCtrl                ; is it Ctrl-NumLock or -CapsLock?
        jnz     kbi0                    ; yes, jump to ROM
;       jmp     short kbi1              ; (commented out for space saving)

kbib:   cmp     al,cDelete		; Detect CTRL-ALT Delete or Break
        jz      kbia
        cmp     al,cBreak
        jz      kbia

; Print Screen detection. On a standard keyboard it's Shift '*' (multiply),
; On the new RT wonder it's a extended '*'. We just set temporarily
; LeftShift to let the BIOS think this is a normal PrintScreen.
;
        cmp     al,cPrint		; is it Multiply ? (Print key)
        jnz     kbi1			; if not, skip
	cmp	cs:[fKbRt],0		; else is it Ronco ?
	jnz	kbibb			; if yes, skip
        test    ah,fShift		; else is Shift down ?
        jnz     kbi0			; if yes, skip to ROM
	jmp	short kbia		; if not, skip

; If PrintScreen was preceeded by extended code, kbd_proc contains now
; prev_was_ext_proc and Shift doesn't need to be down.

kbibb:	cmp	cs:[kbd_proc], dataOFFSET prev_was_ext_proc
	jnz	kbia
	or	ds:byte ptr[ShiftState],fLshift   ; force Left Shift
	mov	cs:[kbd_proc], dataOFFSET standard_proc
	jmp	short kbi0

kbia:   mov     ah,ds:[ShiftState]
        not     ah
        test    ah,fAlt+fCtrl
        jnz     kbi1

	cmp	al,cDelete	; is it Ctl+Alt+Del ?
	jnz	kbi0		; no - just pass to BIOS
; Inform the kernel that Ctl+Alt+Del is happening so that he can inform
; any expanded memory card to reset. (Wed 21-Oct-1987 : bobgu)
	call	KbdRst

kbi0:
	pop	ds
        pop     ax
        jmp     cs:[bios_proc]  ; let ROM handle CTRL-ALT del, break, print

; Read keyboard status and reset the keyboard

kbi1:
; For OS/2 hot key support, we need to pass the Ctl and Esc key transitions
; on to the OS/2 interrupt handler.  This will allow Ctl+Esc to take us
; to the switcher.  Since we also need to use these keys, we must simulate
; an interrupt to the previous interrupt vector so that we get control
; again when they finish.

; There are two possibilities for Ctl+Esc key sequence:
;	1.     Ctl(down), Esc(down), Ctl(up), Esc(up)
;	1.     Ctl(down), Esc(down), Esc(up), Ctl(up)
;
; The only key that causes Windows grief is the Esc key.  We must ignore
; the Esc key up and down if is being used to get us to the switcher.  In
; the second case, it is easy to detect the Ctl key being down and set
; the fIgnoreKey flag for both the down and up transitions.  In the first
; case, however, we don't know if the Esc(up) was used with the Ctl key
; or not.  For this case, we set a flag (fIgnoreEsc) when we see the down
; of the Esc.  We then ignore any Esc(up) keys.  But to play it safe, we
; also need to clear fIgnoreEsc if we see any down transitions, which
; means that the switcher probably ate the Esc(up).

; Late update.... We could get switched TO by a Alt+Esc key sequence (This
; rotates through the screen groups).  However the STUPID person who wrote
; the OS/2 keyboard driver assumed that BOTH the down and the up of the
; Alt key are seen by their driver.  However, if windows is switched to we
; sucked up the break of the Alt key.  This resulted in their driver thinking
; the Alt key was down when it wasn't.  This also prevent us from switching
; out of the box since it thought that Alt+Esc was still happening.  So, we
; must pass all Alt key transitions through their driver to make them happy.
; (Tue 20-Oct-1987 : bobgu)

	cmp	cs:[fOS2Box],0	    ; In the Box?
	jz	kbi13		    ; Nope...

; Alt key up or down?  If so, pass it through the OS/2 driver
	cmp	al,cAlt 	    ; Alt key down ?
	jz	kbi11		    ; yes, pass it through
	cmp	al,cAlt+80h	    ; Alt key up ?
	jz	kbi11		    ; yes, pass it through

	mov	cs:[fIgnoreKey],0   ; initial state
	cmp	cs:[fIgnoreEsc],0   ; ignoring Esc up?
	jz	kbi14		    ; nope..
	mov	cs:[fIgnoreEsc],0   ; clear the flag
	cmp	al,cEsc+80h	    ; up of the Esc key?
	jnz	kbi14		    ; no - Esc was lost - don't ignore key
	inc	cs:[fIgnoreKey]     ; set flag to ignore key
	jmp	kbi11		    ; and pass it to OS/2

kbi14:

; Only interested in the Escape key down if the Control key is also down.

	mov	ah,ds:[ShiftState]
	test	ah,fCtrl	    ; Control key down?
	jz	kbi10		    ; no - check for control key
	cmp	al,cEsc 	    ; Escape key down?
	jnz	kbi10		    ; nope

; Clt+Esc - we really are doing a screen switch.  Set the flag so that
; we ignore this key (but correctly set our key state...)

	inc	cs:[fIgnoreKey]     ; set the flag to ignore
	inc	cs:[fIgnoreEsc]     ; set the "ignore Esc key" flag
	jmp	kbi11		    ; and pass off to OS/2

kbi10:
	cmp	al,cCtrl	    ; Control Key down?
	jz	kbi11		    ; yes

; If fSwitchEnable is zero, the display driver is in a critical section
; and we cannot switch now.  Don't pass any up-clicks to the OS/2 keyboard
; handler.
	cmp	cs:[fIgnoreEsc],0   ; About to switch ?
	jz	kbi111		    ; no - pass them anyway
	cmp	fSwitchEnable,0     ; prevent screen switch?
	jz	kbi13		    ; yes - do normal stuff

kbi111:
	cmp	al,cCtrl+80h	    ; Control Key Up?
	jz	kbi11		    ; yes
	cmp	al,cEsc+80h	    ; Esc key up
	jnz	kbi13		    ; no - do normal stuff

kbi11:
	pop	ds		    ; restore DS
	push	ds		    ; DS & AX still on stack...
	push	ax		    ; save current AX
	pushf			    ; Simulate an INT to the previous
	push	cs		    ; interrupt vector.
	lea	ax,kbi12
	push	ax
	jmp	cs:[bios_proc]
kbi12:
        xor     ax,ax
        mov     ds,ax
	pop	ax		    ; get back our AX

kbi13:
        push    ax
        in      al,kb_ctl
        mov     ah,al
        or      al,80h
        out     kb_ctl,al
        xchg    ah,al
        out     kb_ctl,al
	
        mov     al,eoi                  ; acknowledge the int
        out     ack_port,al
        pop     ax

        mov     ah,80h                  ; move the high order bit to the high
        and     ah,al                   ; order byte.
        xor     al,ah                   ; turn off bit if set

ifdef   SYSREQ
;
;  The SYS REQ key is used to simulate an NMI.
;  This is done by clearing up the stack and doing a far jump to the NMI
;  interrupt vector location.  If the NMI interrupt points into the ROM,
;  we don't jump, since the ROM treats NMIs as parity errors.
;
;  On the RT keyboard SYS REQ is generated by hitting Alt-PrintScreen,
;  badly this combination also has a special meaning in Windows (transfert
;  screen contents to the clipboard). To survive we'll keep this 2nd
;  behaviour and the NMI feature will be reached by hitting Crt-Alt-PrintScreen.
;
        cmp     al,SYSREQ           ; SYSREQ key?
        jne     notsys
	cmp	cs:[fKbRt], 0 	    ; RT keyboard ?
	jz	sys10		    ; if not, skip
	test	ds:byte ptr [ShiftState], fCtrl ; else, is Ctrl down ?
	mov	al, cPrint			; new scan code (only used if not)
	jz	notsys				; if not, skip (code = Print)
sys10:
        or      ah,ah               ; wait for key-up
        jns     ignorekey

        mov     ax,word ptr ds:[2*4+2]
        mov     word ptr cs:[nmi_vector+2],ax
        cmp     ax,0F000H         	; see if it points to ROM
        je      ignorekey        	; otherwise great

        mov     ax,word ptr ds:[2*4]
        mov     word ptr cs:[nmi_vector],ax
        pop     ds
        pop     ax
        jmp     cs:[nmi_vector]

notsys:
endif				; End of SYSREQ stuff

; We are about to enable interrupt, before doing that we have to protect
; against ourself coming before being done.

	cmp	cs:[fReEnter],0	; are we alone ?
	jz	kbiR		; if yes, skip
	jmp	short ignorekey	; else, abort

kbiR:	inc	cs:[fReEnter]	; lock ourself
    ;
    ; NOTE: In order for this to work in various 386 Virtual environments
    ;	just setting this "fReEnter" exclusion flag is not the right thing
    ;	to do. A 386 virtual machine monitor may be Simulating keyboard
    ;	activity to us and just doing this exclude will cause us to miss
    ;	lots of keys that the monitor is trying to send us because it will
    ;	send us keys as soon as we EOI the keyboard and enable interrupts.
    ;
    ;	We fix this problem by masking off the keyboard interrupt at the
    ;	interrupt controller. This prevents the 386 monitor from sending us
    ;	more keys till we're ready to get them. This works fine in the non-386
    ;	environments as well. This method is prefered over disabling the
    ;	keyboard at the Keyboard Controller because it is more portable.
    ;	There seems to be a fair variation amoung clones in the keyboard
    ;	controller used, but the keyboard is always IRQ 1, and the interrupt
    ;	controller is always at the IBM port addresses, and is an 8259.
    ;
	push	ax
	in	al,21h		; Get IRQ mask register
	or	al,02h		; mask off kybd ints (IRQ 1)
	jmp	$+2		; I/O delay for AT type machines
	jmp	$+2
	out	21h,al		; Set new mask
	pop	ax

	sti

	push	bx
	xor	bh,bh
	call	cs:[kbd_proc]
        pop     bx

	mov	cs:[fReEnter],0	; unlock keyboard
    ;
    ; Re-enable keyboard INTs at the interrupt controller
    ;
	cli			; Ints off again NOTE: don't need to turn
				;   them back on because we are going
				;   to do an IRET after we finish this.
				; NOTE: We don't need to worry about AX as we
				;   are about to restore it after we finish this.
	in	al,21h		; Get IRQ mask register
	and	al,NOT 02h	; turn on kybd ints again (IRQ 1)
	jmp	$+2		; I/O delay for AT type machines
	jmp	$+2
	out	21h,al		; Restore correct mask
	jmp	$+2		; I/O delay for AT type machines

ignorekey:
        pop     ds
	pop     ax
        iret
cEnd    nogen



;--------------------------------------------------------------------
;
;----- standard_proc - Standard main keyboard proc -----------------
;
;  This is the default kbd_proc and the only one used if keyboard is
;  not a RT-like.
;
;  Input: AL= Scan code (without break/make information)
;	  AH= 00 (if Make) or 80h (if Break)
;	  BH= 0.
;
;--------------------------------------------------------------------

standard_proc:
        call    SetShiftState

; If used key as OS/2 screen switch, don't do anything except set the
; shift state.
	cmp	cs:[fIgnoreKey],0
	jz	noIgnore
	jmp	stdproc_end
noIgnore:
	mov	bl,al		    ; bl= scancode (all along proc)

	cmp     cs:[fKbRt],0        ; is RT keyboard?
	jz      noSpecial           ; skip if not

; RT-Keyboard only !!!!!!!!!!

	cmp     al,cExtended        ; is it Extended scan code ?
	jnz     noPrevExt           ; skip if not
	mov	cs:[kbd_proc], dataOFFSET prev_was_ext_proc
	jmp     stdproc_end         ; and exit
noPrevExt:

ifdef   DOSHIFTLOCK

; In some european countries, ShiftLock replaces CapsLock, this special
; key only generate a dummy virtual key to set lights. Any pure Shift_Break
; event (not the ones generated internally by the RT keyboard in some
; cases) releases this state.

	cmp     al,cCapsLock        ; Get CapsLock ? (means here ShiftLock)
	jnz	noCaps              ; skip if not
	mov     cs:[fShiftLock],al  ; else store new state
	mov	al,-1		    ; send a dummy VK to set lights
	jmp     kbi4
noCaps:
	cmp     al,cLShift          ; is NON extended LeftShift ?
	jz      isShift
	cmp     al,cRshift          ; or NON extended RightShift ?
	jnz     noShift             ; skip if not
isShift:
	or      ah,ah               ; is it make?
	js      noSpecial           ; skip if not
	mov     cs:[fShiftLock],0   ; else reset ShiftLock
	jmp     short noSpecial
noShift:

; When ShiftLock is on, it just affects the alphanumeric keys, so we'll
; detect them, and if yes we'll shift temporarily before sending our
; VK_code.

	cmp     cs:[fShiftLock],0   ; is ShiftLock on ?
	jz      noSpecial           ; if not, skip

; All alphanumeric have scancodes between 2 and 35h, but there are some
; non alphanumerics inside, and so we need an exception table.

	cmp	al,2
	jb	noSpecial
	cmp	al,35h
	ja	noSpecial

	push	bx
	push	ax
	mov	bx, dataOFFSET ShiftLockException - 1
exceploop:
	inc	bx
	mov	ah,cs:[bx]
	or	ah,ah		    ; end of table ?
	jz	excepnotfound	    ; if yes, skip (ah =0)
	cmp	ah,al
	jnz	exceploop
	or	ah,ah		    ; ah != 0
excepnotfound:
	pop	ax
	pop	bx

; NZ if it is exception
; ZR if alphanumerical (don't need to test Ctrl-Break)

	jnz	noSpecial
				    ; bl is in range (2-35) and bh = 0
	mov	al,cs:[bx+KeyTranslationTable]
	jmp	kbi35

endif             ; end of DOSHIFTLOCK
;
; If the 2 shift keys are down, when the 1rst is released we need
; to keep Shift State down, so we'll ignore the first Shift break.
; Remember: we already called SetShiftState that may have reset
;           the Shift state we are interested in, Yeerk ! 
;
noSpecial:
	test	byte ptr ds:[ShiftState],fShift
	jz	cbd0		    ; no Shift down
	or	ah,ah		    ; is it break ?
	jns	cbd0		    ; no
	cmp	al,cLShift	    ; LeftShift ?
	jz	nosp1		    ; yes, ignore
	cmp	al,cRShift	    ; RightShift ?
	jnz	cbd0		    ; no, skip
nosp1:	jmp	stdproc_end	    ; yes, ignore
;
; Ctrl-Break detection
;
cbd0:
        test    byte ptr ds:[ShiftState],fCtrl  ; is it Ctrl?
        jz      kbi2
        cmp     al,cBreak           ; is it Break?
        jnz     kbi2
        mov     cs:[fBreak],0
        call    CtlBrkCase          ; Test for CtlBreak
        mov     al,VK_CANCEL
        cmp     cs:[fBreak],0
        jnz     kbi4
        jmp     short stdproc_end   ; ignore if somebody is trapping us
;
;  Translate scan code to virtual key
;
kbi2:
        mov     al,-1
        cmp     bl,keyTransTblLen		; test if scancode in table
        jae     kbi4				; if not, skip (VK = -1)
        mov     al,cs:[bx+KeyTranslationTable]  ; get the translation

        cmp     bl,71  ; is it on numeric keypad?
        jb      kbi4
        cmp     bl,83
        ja      kbi4

        test    byte ptr ds:[ShiftState],fNum   ; is num lock down?

; We are on the numeric keypad but Numlock is not down
; Virtual keys are Direction keys and Shifting them will mean
; extension. We use the standard Xlation table (KeyTranslationTable).

        jz      kbi4                            ; no, do normal processing
        test    byte ptr ds:[ShiftState],fLshift+fRshift        ; shift keys down?
        jnz     kbi3                                            ; yes, weird stuff

; NumLock is down but not the Shift keys, in this case we get the numbers,
; as the Xlation table doesn't contain the VK_numpad we have to use a
; secondary table (KeyNumTrans).

	mov     al,cs:[bx+KeyNumTrans-71]
        jmp     short kbi4

; NumLock is down and so are LeftShift or RightShift, in this case we
; want the direction keys (not extension). So we use the Direction virtual
; keys and before we undo temporarily Shift.
 
kbi3:   push    ax
        push    bx
        mov     ax,VK_SHIFT+8000H
        mov     bl,54		; scancode of RightShift
        call    cs:[event_proc]
        pop     bx
        pop     ax
        call    cs:[event_proc] ; send actual key...
        mov     ax,VK_SHIFT     ; pretend shift key went down
        mov     bl,54

ifdef	DOSHIFTLOCK
	jmp	short kbi4
;
; ShiftLock is down, and the scancode represents an alphanumeric key,
; in this case we shift temporarily before sending the scancode.
;
kbi35:
	push    ax
        push    bx
        mov     ax,VK_SHIFT	; pretend shift key goes down
        mov     bl,54		; scancode of LeftShift
        call    cs:[event_proc]
        pop     bx
        pop     ax
        call    cs:[event_proc] ; send actual key...
        mov     ax,VK_SHIFT+8000H ; pretend shift key goes up
        mov     bl,54
endif	; of DOSHIFTLOCK

	errn$	kbi4
;
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
;
; Windows preserves all registers
;
kbi4:   call    cs:[event_proc]
stdproc_end:
	ret


;---------------------------------------------------------------------
;
;----- prev_was_ext_proc - used when previous was extended prefix (E0) 
;
;  Used only by RT keyboard.

;  Input: AL= Scan code (without break/make information)
;	  AH= 00 (if Make) or 80h (if Break)
;	  BH= 0.
;
;      Mainly undoes Shifting and Unshifting generated internally by
;      this (@#?!) keyboard and use the extended prefixes to distinguish
;      between normal keys and new keys.
;
;---------------------------------------------------------------------
;
prev_was_ext_proc:
	mov	cs:[kbd_proc], dataOFFSET standard_proc
	cmp     al,cLShift          ; is it extended Left Shift ?
	jz	prevproc_end        ; if yes, eat it
	cmp	al,cRShift	    ; else is it extended Right Shift ?
	jz	prevproc_end        ; if yes, eat it

; Test if we receive PrintScreen. Remember the make code has been eaten
; before we get a chance to be called. The break code will release the
; temporarily LeftShift that was then set.

	cmp	al,cPrint
	jnz	prev10
	xor	ds:byte ptr[ShiftState],fLshift
	jmp	short prevproc_end

prev10:
        call    SetShiftState
	mov	bl,al		    ; bl= scancode (all along proc)

; Divide key special case: if Slash is found a VK_DIVIDE is sent
;
	cmp	al,cSlash	    ; is it extended Slash ?
	jnz	prev20		    ; if not, skip
	mov	al,VK_DIVIDE	    ; else send VK_DIVIDE
	jmp     short prev40

; When ShiftLock is on, we don't want to be get our VK_code shifted (none
; of our code correspond to alphanumeric), so we have nothing to do.
; We would have one exception (VK_DIVIDE), but we already took care of it.

; In some countries on the "enhanced" keyboard the new Right Alt
; acts like Ctrl-Alt, thus providing the extra characters
; (like |,\,{,...) that are not directly accessible. That does not
; affect other combinations like Ctrl-Alt-Del or Alt-SysRq).
;
prev20:
	cmp	cs:[fAltGr],0       ; special behaviour activated ?
	jz	prev25		    ; if not,skip
	cmp	al,cAlt             ; getting the Alt scancode means
	jnz	prev25              ; the right Alt (extended only)

; AltGr was detected, transforming it into Ctrl-Alt
;
	mov	al,VK_CONTROL
	mov	bl,cCtrl
	call	cs:[event_proc]     ; this guy must save ah
	mov	al,VK_MENU
	mov	bl,cAlt
	jmp	short prev40

; Break case
;
prev25:
	cmp	al,cBreak	    ; is it extended Break ?
	jnz	prev30		    ; if not, skip
        mov     cs:[fBreak],0
        call    CtlBrkCase          ; Test for CtlBreak
        mov     al,VK_CANCEL
        cmp     cs:[fBreak],0
        jnz     prev40
        jmp     short prevproc_end  ; ignore if somebody is trapping us

; Standard case, convert to our virtual key
;
prev30:
        mov     al,-1
        cmp     bl,keyTransTblLen		; test if scancode in table
        jae     prev40				; if not, skip (VK= -1)
        mov     al,cs:[bx+KeyTranslationTable]  ; else, get the translation
	errn$	prev40
;
; Call windows with ah == 80h for uptrans, 00h for downtrans
; al = virtual key, bl = scan code
;
; Windows preserves all registers
;
prev40:
	call    cs:[event_proc]
prevproc_end: 
	ret



;------------------------------------
;
; ----- CtlBrkCase - Break routine
;
;
;-------------------------------------
;
cProc   CtlBrkCase,NEAR
cBegin  nogen
; we are now control-breaking

        push    bx              ; save registers we use
        push    cx
        push    dx
        push    si
        push    di
        push    es
        push    bp

        INT     1bh

        pop     bp
        pop     es
        pop     di
        pop     si
        pop     dx
        pop     cx
        pop     bx
        ret
cEnd    nogen


;----------------------------------
;
;---- INT1BHandler ----------------
;
;----------------------------------

cProc   INT1BHandler, <PUBLIC,FAR>
cBegin  nogen
        inc     cs:[fBreak]
        iret
cEnd    nogen


;---------------------------------------------------
;
;---- SetShiftState --------------------------------
;
;  Keep accurate track of shift state byte at 40:17H
;
;  input: AL= Scancode
;	  AH= make/break (00/80h)
;  destroys: BL, flags
;
;---------------------------------------------------

SetShiftState:
        mov     bl,fLshift
        cmp     al,cLshift
        jz      ccv4
        mov     bl,fRshift
        cmp     al,cRshift
        jz      ccv4
        mov     bl,fCtrl
        cmp     al,cCtrl
        jz      ccv4
        mov     bl,fAlt
        cmp     al,cAlt
        jnz     ccv6
ccv4:   or      ah,ah
        jns     ccv5
        not     bl
        and     byte ptr ds:[ShiftState],bl
        ret
ccv5:   or      byte ptr ds:[ShiftState],bl
ccv6:   ret

sEnd    DATA


sBegin  CODE        ; Beginning of code segment
assumes CS,CODE
assumes DS,DATA

;---------------------------------------------------------------------
;
;---- ScreenSwitchEnable( fEnable ) ----------------------------------
;
;   This function is called by the display driver to inform the keyboard
;   driver that the display driver is in a critical section and to ignore
;   all OS/2 screen switches until the display driver leaves its
;   critical section.  The fEnable parameter is set to FALSE to disable
;   screen switches, and TRUE to re-enable them.  By default, screen
;   switches are enabled.
;---------------------------------------------------------------------
;
cProc	ScreenSwitchEnable,<PUBLIC,FAR>
ParmW	fEnable

cBegin	ScreenSwitchEnable
	xor	ah,ah		    ; assume disable
	cmp	fEnable,0
	jz	disab
	inc	ah		    ; enable switches
disab:
	mov	fSwitchEnable,ah    ; set the flag
cEnd	ScreenSwitchEnable


;--------------------------------------------------------------------
;
;---- Inquire( pKBINFO ) --------------------------------------------
;     Copies information about the keyboard hardware into the area
;     pointer to by the long pointer argument.  Returns a count of
;     the number of bytes copied.
;
;--------------------------------------------------------------------
;
cProc   Inquire,<PUBLIC,FAR>,<si,di>
Parmd   pKBINFO

cBegin  Inquire
        les     di,pKBINFO              ; Get far pointer of destination area
        mov     si,dataOFFSET inquireData
	cmp	[fKbRt],0               ; Test if Rt keyboard
	jz	noRt                    ; skip if not
	mov     [si].kbNumFuncKeys,numFunc+2 ; Increment by 2 if RT keyboard
noRt:
        mov     ax,SIZE KBINFO          ; Get number of bytes to move
        mov     cx,ax
        rep     movsb                   ; Move the bytes
        sub     ax,cx                   ; Return actual byte count moved
cEnd    Inquire


;--------------------------------------------------------------------
;
;---- Disable( eventProc ) ------------------------------------------
;     Disable hardware keyboard interrupts, restoring the previous
;     IBM BIOS keyboard interrupt handler.
;
;--------------------------------------------------------------------
;
cProc   Disable,<PUBLIC,FAR>,<si,di>

cBegin  Disable
; Do nothing if not enabled
        cmp     [enabled],0
        je      disdone

; Wild stuff !! If RT keyboard has been detected, we need to put back
; the type bit in RAM BIOS.

	cmp	[fKbRt],0	; is it RT keyboard ?
	jz	dis10		; if not, skip
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
   assumes ds,RAMBIOS
	or	[KB_type],10h	; magic
	pop	ds
   assumes ds,DATA

; Restore the keyboard interrupt vector to point to previous value
dis10:
        mov     ah,25h
        mov     al,vector
        push    ds
        lds     dx,[bios_proc]
        int     21h
        pop     ds

        mov     ah,25h
        mov     al,1bh
        push    ds
        lds     dx,[bios1b_proc]
        int     21h
        pop     ds

        mov     [enabled],0
disdone:
cEnd    Disable



;--------------------------------------------------------------------
;
;---- Enable( eventProc, lpKeyState ) -------------------------------
;     Enable hardware keyboard interrupts, with the passed procedure
;     address being the target of all keyboard events. lpKeyState is
;     a long pointer to the Windows 256 byte keystate table.
;
;--------------------------------------------------------------------
;
cProc   Enable,<PUBLIC,FAR>,<si,di>
    ParmD   eventProc
    ParmD   lpKeyState
cBegin  Enable
; Save away passed address of event procedure
        les     bx,eventProc
        mov     ax,es
        mov     WORD PTR [event_proc],bx
        mov     WORD PTR [event_proc+2],es

; All done if just reenabling a different event proc
        cmp     [enabled],0
        jne     enadone

; Wild stuff !! If RT keyboard has been detected, we'll let think the
; BIOS we have a standard keyboard.

	cmp	[fKbRt],0	; is it RT keyboard ?
	jz	ena10		; if not, skip
	push	ds
	mov	ax,RAMBIOS
	mov	ds,ax
   assumes ds,RAMBIOS
	xor	[KB_type],10h	; magic
	pop	ds
   assumes ds,DATA

; Save away current keyboard interrupt vector value
ena10:
        mov     ah,35h
        mov     al,vector
        int     21h
        mov     WORD PTR [bios_proc],bx
        mov     WORD PTR [bios_proc+2],es

; Save away current keyboard call vector value
        mov     ax,3516H
        int     21h

        mov     WORD PTR [LightsAddr],bx
        mov     WORD PTR [LightsAddr+2],es

; Setup keyboard interrupt vector to point to our interrupt routine
        mov     ah,25h
        mov     al,vector
        mov     dx,dataOFFSET keybd_int
        int     21h

; Initialize keyboard state.
; ES:DI points to movable memory, so no allocs allowed.
        les     di,lpKeyState
        call    InitKeyState

; Get the INT 1A vector, save away.
        mov     ah, 35h         ; Get Vector command
        mov     al, 1bh         ; Interrupt vector
        int     21h
        mov     word ptr [bios1b_proc],bx
        mov     word ptr [bios1b_proc + 2],es

; Set a new INT 1B vector
        mov     ah, 25h         ; Set Vector command
        mov     al, 1bh         ; Interrupt vector
        mov     dx,dataOFFSET INT1BHandler
        int     21h

	mov     [enabled],1

enadone:
cEnd    Enable



;----------------------------------------------------
;
;---- InitKeyState ----------------------------------
;     Initialize Windows key status (Shift, Ctrl, Alt,
;     CapsLock or ShiftLock) according to the present
;     values stored in the ROM BIOS.
;     Called by Enable().
;
;----------------------------------------------------
;
InitKeyState    proc    near
        mov     ah,2            ; get shift state
        int     16H

        mov     dl,10000000b
        mov     ah,00000011b
        mov     bx,VK_SHIFT     ; note bh=0
        call    SetKeyState
        mov     ah,00000100b
        mov     bl,VK_CONTROL
        call    SetKeyState
        mov     ah,00001000b
        mov     bl,VK_MENU
        call    SetKeyState

        mov     dl,1
        mov     ah,00010000b
        mov     bl,VK_OEM_SCROLL
        call    SetKeyState
        mov     ah,00100000b
        mov     bl,VK_OEM_NUMBER
        call    SetKeyState
        mov     ah,01000000b
        mov     bl,VK_CAPITAL
ifdef DOSHIFTLOCK
	cmp     [fKbRt],0            ; is RT keyboard ?
	jz	SetKeyState          ; skip if not
        mov     dl,10000000b         
        mov     bl,VK_SHIFT
	call    SetKeyState          ; CapsLock on means really Shift on
	jz      sks1                 ; done if was off
	mov     [fShiftLock],1       ; else set our own flag
	ret                          ; done
endif ; of DOSHIFTLOCK
SetKeyState:
        test    al,ah
        jz      sks1
sks0:   mov     es:[di+bx],dl
sks1:   ret
InitKeyState    endp


; This routine set the keyboard leds
; Dos version specific

cProc	DoLights,<PUBLIC,NEAR>
	parmD	lpKeyState
cBegin	DoLights
        xor     bx,bx
        mov     al,fCaps
ifdef DOSHIFTLOCK
;
; if Shift Lock is used it replaces CapsLock
;
	cmp	[fKbRt],0          ; is Ronco keyboard
	jz	dol2               ; skip if not (just set plain Capslock)
	mov     es,bx              ; ES = 0 (remember bx=0)
	cmp     [fShiftLock],0     ; is ShiftLock on ?
	jz      dol1               ; skip if not
	or      es:[ShiftState],al ; if yes set CapsLock light
	jmp     short dol3
dol1:
	not     al                 ; reset CapsLock light
        and     es:[ShiftState],al
	jmp     short dol3
dol2:
endif  ; of DOSHIFTLOCK

        mov     bl,VK_CAPITAL
        call    ss1
dol3:
        mov     al,fScroll
        mov     bl,VK_OEM_SCROLL
        call    ss1
        mov     al,fNum
        mov     bl,VK_OEM_NUMBER
        call    ss1
        mov     ah,1
        pushf
        call    [LightsAddr]         ; go set the lights
cEnd	DoLights


ShiftState  EQU 417H    ; rom bios data area of shift states

ss1:    les     si,lpKeyState
        test    byte ptr es:[si+bx],1
        mov     si,0
        mov     es,si
        jz      ss2
        or      es:[ShiftState],al
        ret
ss2:    not     al
        and     es:[ShiftState],al
        ret

sEnd    CODE


;
; The INIT segment is discardable, as it can be executed only once no
; special care has been taken inside when calling internal subroutines.
; 
createSeg _INIT, INIT, BYTE, PUBLIC, CODE
sBegin INIT
assumes CS,INIT
assumes DS,DATA

;----------------------------------------------------------
;
;---- SetKbdType ------------------------------------------
;     Detect Keyboard type (standard or enhanced (RT-like))
;     fKbRt is set accordingly (preset to 0 and set to 1
;     if the enhanced keyboard is found.
;     on output AX= 0 to indicate success.
;
;----------------------------------------------------------

cProc	SetKbdType,<PUBLIC,NEAR>
cBegin	nogen
	push    ds
	mov     ax,RAMBIOS
	mov     ds,ax
assumes DS,RAMBIOS
	test    byte ptr [KB_type],10h ; is RT keyboard flag set ?
	pop	ds
assumes DS,DATA
	jz      Set1            ; skip if not
;	mov	ah,91h
;	int     16h             ; now test if Bios support this keyboard
;	test    ah,80h          ; if it does ah will be decremented at least
;				; by 12h, else ah will stay unchanged.
;	jnz     Set1            ; skip if not
	mov     [fKbRt],1       ; Say we have a RT keyboard.
Set1:

; This is a convenient place to test to see if we are running in the
; OS/2 compatibility box.  If the DOS version # is 10 or greater then
; it is OS/2.  Set a flag so we test for Ctl+Esc to do screen switches.

	mov	ah,30h
	int	21h
	cmp	al,0Ah
	jl	notOS2
	inc	fOS2Box
notOS2:
	xor	ax,ax		; AX= 0 for success
	ret
cEnd	nogen


;----------------------------------------------------------
;
;---- IsAt -----------------------------------------------
;     Detect Computer type (AT or not)
;     on output ZR if AT-like, NZ otherwise
;
;----------------------------------------------------------

cProc	IsAt,<PUBLIC,NEAR>
cBegin	nogen
	push	ds
	mov     ax,ROMBIOS
	mov     ds,ax
assumes DS,ROMBIOS
	cmp     byte ptr [PC_type],0FCh ; is AT id ?
	pop	ds
	ret
cEnd	nogen

sEnd    INIT
end
