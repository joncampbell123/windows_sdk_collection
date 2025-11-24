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
                
;****** ENABLE.ASM ********************************************************

include keyboard.inc
include vkoem.inc
include vkwin.inc
include lcid.inc                                        ;[damianf] new

include int31.inc

if1
%out
%out ...........
%out ENABLE.ASM - (Multilingual Chicago)
%out ...........
endif

if1
ifdef NEWNMI
%out .. NMI save in EnableKBSysReq()
else
%out .. NMI save only in Enable()
endif
endif

;***************************************************************************
; Some Olivetti M24x /AT&T 6300x keyboard definitions.
;***************************************************************************

; Double byte range values for the Far East.
; The values defined here are for the Rest Of the World.
; These values are for the inquireData (KBINFO) structure defined below.
; ('KeyInfo' in the Kernel, pState in USER)
;
BeginRange1     equ     255
EndRange1       equ     254
BeginRange2     equ     255
EndRange2       equ     254

OFFSEL          struc
off     dw      ?
sel     dw      ?
OFFSEL          ends

externFP	InquireEx
ExternFP        LoadTheWorld            ; for initialisation


;***************************************************************************
; DATA segment -- data declarations and local data
;***************************************************************************

sBegin  DATA

assumes DS,DATA

; DATA segment variables accessed in KbInit() below


; Data to specify keyboard type and system type, in the keyboard table
; module.  Some of these values are loaded with the table, others are
; computed in the INIT code in this module.

        extrn   TableType:byte          ; Table type loaded (1,2,3,4)

        extrn   KeyType:byte            ; Computed keyboard type (1,2,3,4,5,6)

        extrn   OliType:byte            ; NZ if Olivetti-protocol keyboard,
                                        ; identifies keyboard type. Also
                                        ; for AT&T.
                                        ; For PCType values, see OLIKBD.INC
        extrn   fKeyType:byte           ; if RT, copy of KeyType

        extrn   PCType:byte             ; needed for Nokia

        EXTRN   event_proc:DWORD        ; in TRAP.ASM (user's keyboard handler)

public fTables                          ; public for debug
fTables         db      0               ; Call has been made to NewTable

public fSysReq
fSysReq         db      0               ; Enables CTRL-ALT-SysReq if NZ

; Keyboard information block (copied to 'KeyInfo' in the kernel)
; this is a KBINFO data structure.. defined in KERNEL.INC, USER.INC, USER.H
; and WINDEFS.INC.
;
; As of 3.0, build 1.30, KBINFO includes the number of function keys
; As of 3.0, build 1.59, the number of bytes in the state block is
; fixed at 4 MAX, for compatibility with Excel 2.1c!!!
;
                PUBLIC  inquireData
                PUBLIC  iqdNumFunc
inquireData     LABEL   BYTE
                DB      BeginRange1
                DB      EndRange1
                DB      BeginRange2
                DB      EndRange2
                DW      4               ; #bytes of state info for ToAscii()
iqdNumFunc      label   word
                dw      10              ; number of function keys

; flag for OS/2 to allow/prevent screen switches
; The ScreenSwitchEnable function, called from the display driver,
; sets/resets this flag. A nonzero value indicates that switching
; is enabled.
;
;       extrn   fSwitchEnable:byte
;


kflags          db      0ffh    ; prev. value in [kb_flag]
                                ; For handling of Oli M24 keyboard lights
globalB old_enhanced, 0         ; save value at 40h:96h (enhanced kbd) here..
globalB enabled, 0              ; layouts/driver enabled yet?

ExternD bios_proc           ; these are in DS for ROM
ExternD nmi_vector

sEnd


;***************************************************************************
;***************************************************************************
;***************************************************************************
;       Resident code for this module
;***************************************************************************
;***************************************************************************
;***************************************************************************

externFP        NewTable                                ;in TABS
externFP        LoadOemAnsiXlat                         ;Tabs.asm [damianf]
externFP        GetKeyboardInfo                         ;Tabs.asm [damianf]



sBegin  CODE        ; Beginning of code segment
assumes CS,CODE
assumes DS,DATA

.386

; Light handler in ToAscii.asm

    extrn SetLightHardware: near

; keyboard interrupt trap in TRAP.ASM

;*****************************************************************************
;**************************** ScreenSwitchEnable *****************************
;*****************************************************************************
;
;   This function is called by the display driver to inform the keyboard
;   driver that the display driver is in a critical section and to ignore
;   all OS/2 screen switches until the display driver leaves its
;   critical section.  The fEnable parameter is set to 0 to disable
;   screen switches, and a NONZERO value to re-enable them.  At startup,
;   screen switches are enabled.

cProc	ScreenSwitchEnable,<PUBLIC,FAR>
;ParmW	fEnable

cBegin nogen

retf 2

cEnd nogen


;*****************************************************************************
;****************************** EnableKBSysReq *******************************
;*****************************************************************************
;
;   This function enables and shuttles off NMI interrupt simulation
;   (trap to debugger) when CTRL-ALT-SysReq is pressed.
;   CVWBreak overides int 2.
;   fSysParm    = 01    enable  int 2
;               = 02    disable int 2
;               = 04    enable  CVWBreak
;               = 08    disable CVWBreak
;
cProc   EnableKBSysReq,<PUBLIC,FAR, LOADDS>
ParmW   fSysParm

cBegin  EnableKBSysReq

        mov     ax, fSysParm            ; get WORD parameter

        test    al,01                   ; turn on int 2?
        jz      @F
        or      fSysReq,01              ; yes, turn it on!
@@:     test    al,02                   ; turn off int 2?
        jz      @F
        and     fSysReq,NOT 01          ; yes, turn it off!

@@:     test    al,04                   ; turn on CVWBreak?
        jz      @F
        or      fSysReq,02              ; yes, turn it on!
@@:     test    al,08                   ; turn off CVWBreak?
        jz      @F
        and     fSysReq,NOT 02          ; yes, turn it off!
@@:
        xor     ah,ah
        mov     al,fSysReq
ifdef NEWNMI
        push    ax                      ; save return value
        call    short GetNmi            ; save NMI
        pop     ax                      ; restore ..
endif

cEnd    EnableKBSysReq

; save NMI vector.  Called above in EnableKBSysReq() and in Enable().

ifdef NEWNMI
GetNmi proc near
        mov     ax,3502h
        int     21h
        mov     word ptr ds:[nmi_vector][0],bx
        mov     word ptr ds:[nmi_vector][2],es
        ret
GetNmi endp
endif



;*****************************************************************************
;********************************** Enable ***********************************
;*****************************************************************************
;
; Enable( eventProc ) - enable hardware keyboard interrupts, with the passed
; procedure address being the target of all keyboard events.
;
; lpKeyState is a long pointer to the Windows 256 byte keystate table
;

cProc   Enable,<LOADDS, PUBLIC,FAR>,<si,di>
        ParmD   eventProc
        ParmD   lpKeyState

cBegin  
        les     bx,eventProc
        mov     ax,es
        mov     WORD PTR [event_proc],bx
        mov     WORD PTR [event_proc+2],es

; Initialize shift-key bytes in keyboard state vector to correspond to
; BIOS's flags in 40:17h.
; ES:DI points to movable memory, so no allocs allowed up to the point
; InitKeyState() is called.

; We do this EVERY time Enable is called, since we may be coming back from
; a full-screen old app.

        les     di,lpKeyState
        call    InitKeyState

;
; All done if just reenabling a different event proc
;
        cmp     [enabled],0
        jnz     EnableExit

	call	LoadTheWorld			; load em up!

;
; Set lights properly
;
        call    SetLightHardware

    public EnableExit   ; for debug
EnableExit:

cEnd    

;*****************************************************************************
;******************************* InitKeyState ********************************
;*****************************************************************************
;
; The point of this routine is to make the shift, control, alt,
; numlock, capslock or shiftlock, and scrollock bits in KeyState
; correspond to the ROM BIOS's key state bits when Windows starts.
;
; We assume that Windows has first cleared its key state vector to
; all 0's.
;
; NOTE:  the sign bit of each byte in KeyState indicates up/down,
;  and bit 0 is toggled on each depression.
; The entries in KeyState have already been initialized to 0's by
; Windows.
;
; NOTE: When this routine is called, the keyboard type may NOT have been
; determined yet.
;
; This is called with ES:DI => key state vector

public InitKeyState 
InitKeyState proc near

        ; Get BIOS shift state byte directly from RAM (40H:17H) instead
        ; of via int 16H call.

        push    es                      ; remember, we need ES:DI
        ; access RAM directly
        mov     si,RAMBIOS              ; segment 40H or selector for it.
        mov     es,si
        mov     al,byte ptr es:[kb_flag] ; [AL] = BIOS keystate (40H:17H)
        pop     es

        ; the binary masks
        ; The shift (first 3) flags need their STATE (80H) bit initialized:
        ; so we put 80h into DL to load into the state vector.

	mov	dl,80h

        mov     ah,fShift               ; init. VK_SHIFT byte in KeyState
        mov     bx,VK_SHIFT             ; note -- THIS CLEARS BH!
        call    SetKeyState
        mov     ah,fCtrl                ; init. VK_CONTROL byte in KeyState
        mov     bl,VK_CONTROL
        call    SetKeyState
        mov     ah,fALt                 ; init. VK_MENU byte in KeyState
        mov     bl,VK_MENU              ; 
        call    SetKeyState             ;

        ; The CAPSLOCK, SCROLL and NUMLOCK flags need their TOGGLE
        ; bits initialized, so put 1 in DL to load into the state vector.
        mov     dl,1                    ; load TOGGLE bit
        mov     ah,fScroll
        mov     bl,VK_OEM_SCROLL
        call    SetKeyState
        mov     ah,fNum
        mov     bl,VK_NUMLOCK
        call    SetKeyState
        mov     ah,fCaps                ; BIOS CAPS LOCK/SHIFT LOCK bit?
        mov     bl,VK_CAPITAL

public SetKeyState, sks1

SetKeyState:

        test    al,ah
        jz      sks1

	mov	es:[di+bx],dl

        ret
sks1:
	mov	byte ptr es:[di+bx],0
        ret

InitKeyState endp


;*****************************************************************************
;********************************* Disable ***********************************
;*****************************************************************************
;
; Disable( eventProc ) - disable hardware keyboard interrupts, restoring
; the previous IBM BIOS keyboard interrupt handler.
;

cProc	Disable,<PUBLIC,FAR>,<si,di>

cBegin  Disable
        ;
        ; Do nothing if not enabled
        ;
        cmp     [enabled],0
        jz      Dis_Done

;
; Wild stuff !! If RT keyboard has been detected, we put back
; the Enhanced keyboard flag at 40:96h, whatever it was.
;

        cmp     [KeyType], 4                    ; is it RT keyboard ?
        jne     dis10                           ; ... if not, skip
                                                ; ... else is RT
        mov     bl, old_enhanced                ; get saved value for 40:96h
        push    ds
        mov     ax, RAMBIOS
        mov     ds, ax
        assumes ds,RAMBIOS
        mov     byte ptr[KB_type], bl           ; restore Enhanced flag
        pop     ds
        assumes ds,DATA
dis10:
        ;
        ; Restore the keyboard interrupt vector to point to previous value
        ;
        mov     ax, 2500h or vector
        push    ds
        lds     dx,[bios_proc]
        int     21h
        mov     ax,RAMBIOS
        mov     ds,ax
        assumes ds,RAMBIOS
        and     WORD PTR [kb_flag],0FCFFh       ; clear left ctrl+left alt
        pop     ds
        assumes ds,DATA

        mov     [enabled],0

Dis_done:
cEnd    Disable

.286p
sEnd CODE

;***************************************************************************
;       int GetKbdType(Which)
;
;       if Which == 0, returns keyboard type (1..6)
;
;       if Which == 1, returns keyboard subtype ("OliType")
;
;       if Which == 2, returns number of function keys
;
;       otherwise returns 0.
;
;***************************************************************************

createSeg _GETTYPE, GETTYPE, BYTE, PUBLIC, CODE
sBegin  GETTYPE
assumes CS,GETTYPE
assumes DS,DATA

.386
cProc GetKeyboardType,<LOADDS, PUBLIC,FAR>,<si,di>

    ParmB Which

cBegin GetKeyboardType

        xor     ax,ax                           ; clear return value
        cmp     Which,0
        jne     GetKbd1
        mov     al,KeyType                      ; type= from SYSTEM.INI
        jmp     short GetKbdExit
GetKbd1:
        cmp     Which,1
        jne     GetKbd2
        mov     al,OliType                      ; subtype= from SYSTEM.INI
        jmp     short GetKbdExit
GetKbd2:
        cmp     Which,2
        jne     GetKbdExit
        mov     ax,iqdNumFunc                   ; number of function keys

GetKbdExit:

cEnd GetKeyboardType

.286p
sEnd GETTYPE

if2
%out .. end ENABLE.ASM
%out
endif

END
