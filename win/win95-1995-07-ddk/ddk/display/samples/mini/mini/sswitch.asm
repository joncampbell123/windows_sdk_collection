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

;----------------------------------------------------------------------------
; SSWITCH.ASM
;----------------------------------------------------------------------------
        .xlist
        include cmacros.inc
	include	dibeng.inc
        include macros.inc
        .list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------
STOP_IO_TRAP      equ   4000h           ;Stop io trapping
ENTER_CRIT_REG    equ   4003h           ;Enter Critical Section notification.
EXIT_CRIT_REG     equ   4004h           ;Exit Critical Section notification.
START_IO_TRAP     equ   4007h           ;Restart io trapping
SCREEN_SWITCH_OUT equ   4001h           ;Moving 3xBox to background
SCREEN_SWITCH_IN  equ   4002h           ;Moving 3xBox to foreground
REPAINT_EXPORT_INDEX    equ     275
PREVENT_SWITCH    equ   10000000b       ;Don't allow switch (DOS 3.x, 4.x)
INT_2F_HOOKED     equ   00000001b       ;Have hooked int 2Fh
FLAGS_ON_STACK    equ   4               ;iret --> offset(0), seg(2), flags(4)
CARRY_FLAG        equ   00000001b
DEVICECOLORMATCH_EXPORT_INDEX   equ     449

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
        externFP GetModuleHandle
        externFP GetProcAddress
        externFP AllocCSToDSAlias       ;get a DS alias for CS
        externFP FreeSelector           ;frees up selectors
        externW  wPalettized
	externW  ScreenSelector
        externFP ResetHiResMode

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin  Data
externD	lpDriverPDevice
globalW DeviceFlags,0
SwitchTable     label   word
                dw      DevToBackground
                dw      DevToForeground
SwitchControl   db      0               ;Switch control flags
;----------------------------------------------------------------------------
; Repaint variables.
;----------------------------------------------------------------------------
bRepaintDisable db      0h              ;ok to call user to repaint
RepaintPending  db      0               ;repaint is pending.
public RepaintAddr
RepaintAddr     dd      0
szUSER          db      'USER',0

public GetColorAddr
GetColorAddr   dd      0
szGDI          db      'GDI',0
sEnd    Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin  Code
assumes cs,Code
        .386
PrevInt2Fh	dd	0		;Previous int 2Fh vector
externFP	ResetHiResMode		;in VGA.ASM

;----------------------------------------------------------------------------
; UserRepaintDisable
;
; USER calls this function to tell the display driver whether it expects
; the repaint call to be postponed or not.
;
; If RepaintDisable is TRUE then the display driver should not send
; repaint requests to USER immediately but wait till it is enabled again.
; USER will enable repaint by seting RepaintDisable to FALSE.
;----------------------------------------------------------------------------
cProc   UserRepaintDisable,<FAR,PASCAL,PUBLIC>
        parmB   disable                 ;TRUE to Disable/FALSE to enable
cBegin
        assumes ds,Data
	mov	al,disable		;get the value
        mov     bRepaintDisable,al      ;save it
        or      al,al                   ;being enabled again
	jnz	short @f		;no.
	cmp	RepaintPending,0ffh	;pending repaint ?
        jnz     short @f                ;no.
        call    DTF_RepaintScreen       ;call repaint
        mov     RepaintPending,0        ;done with pending call
@@:
cEnd

;----------------------------------------------------------------------------
; ScreenSwitchHook   INTERRUPT HANDLER
;
; Watches calls to the INT 2Fh multiplex interrupt chain, and traps calls
; to the driver to save or restore the state of the display hardware
; before a context switch.
;
; If a save/restore call is recognized, then it will be dispatched
; to the device dependent handler unless PREVENT_SWITCH is
; set in SwitchControl.
;
; Entry:
;       AH = multiplex number
;       AL = function code
; Returns:
;       'C' set if screen switch cannot occur (DOS's < 10.0)
;       'C' clear otherwise
;----------------------------------------------------------------------------
        assumes ds,nothing
        assumes es,nothing

PPROC ScreenSwitchHook far
        cmp     ax,SCREEN_SWITCH_IN     ;Switching to the FOREGROUND?
        je      SSH_HandleIt            ;yes.
        cmp     ax,SCREEN_SWITCH_OUT    ;Switching to the BACKGROUND?
        je      SSH_HandleIt            ;yes.
        jmp     PrevInt2Fh              ;Not ours, pass it along.

SSH_HandleIt:
        push    bp                      ;set up frame for altering flags
        mov     bp,sp
        push    ds
        pushad
        mov     bx,_DATA
        mov     ds,bx
        assumes ds,Data
        mov     ah,SwitchControl
        add     ah,ah
        jc      SSH_ExitError
        errnz   PREVENT_SWITCH-10000000b
        and     ax,00000010b            ;Use D1 of function to index into
        mov     bx,ax                   ;  the two word dispatch table
        errnz   SCREEN_SWITCH_OUT-4001h
        errnz   SCREEN_SWITCH_IN-4002h
        call    SwitchTable[bx]
        popad
        pop     ds
        assumes es,nothing
        assumes ds,nothing
;----------------------------------------------------------------------------
; Note that BP is still on the stack (hence FLAGS_ON_STACK+2).
;----------------------------------------------------------------------------
        and     wptr [bp][FLAGS_ON_STACK][2],not CARRY_FLAG ;Show success.
        pop     bp
        iret

SSH_ExitError:
        popad
        pop     ds                              ;This is not a good time
        assumes ds,nothing                      ;to do a screen switch.
        or      wptr [bp][FLAGS_ON_STACK][2],CARRY_FLAG  ;Set the carry flag
        pop     bp                              ;and hopefully, the VDD will
        iret                                    ;try again later.
ScreenSwitchHook        endp

;----------------------------------------------------------------------------
; DevToForeground
; Performs any action necessary to restore the state of the display
; hardware prior to becoming the active context.
;----------------------------------------------------------------------------
PPROC DevToForeground   near
        assumes ds,Data
        assumes es,nothing
	assumes	gs,nothing
	assumes	fs,nothing
PLABEL DTF_EnableDriver
;
;Some applications such as AAPLAYER depend on the INT 2FH call to
;not only repaint the screen, but also reset the mode from low resolution
;back into the Windows HiRes mode.  Normally, we'd already have been
;switched back to HiRes mode by the time we've reached here.  However,
;if the app hasn't switched back in the normally accepted way,
;we'll still be in a standard (13H or below) VGA mode:
;
	les	si,lpDriverPDevice
	test	es:[si].deFlags,BUSY	;Is the driver already in the Foreground?
	jz	DTF_RepaintScreen	;yes, repaint only.
        call    ResetHiResMode

PLABEL DTF_RepaintScreen
        mov     ax,wptr RepaintAddr[0]  ;We expect to always have the
        or      ax,wptr RepaintAddr[2]  ;  address, but let's be sure
        jz      short DTF_Exit          ;Nope.
        cmp     bRepaintDisable,0       ;Is repaint enabled ?
        je      short @f                ;yes.
        mov     RepaintPending,0ffh     ;no. repaint pending
        ret
@@:     call    RepaintAddr             ;Force repaint of all windows
PLABEL DTF_Exit
        ret
DevToForeground endp

;----------------------------------------------------------------------------
; DevToBackground
; Performs any action necessary to save the state of the display
; hardware prior to becoming an inactive context.
;----------------------------------------------------------------------------
PPROC DevToBackground   near
        assumes ds,Data
        assumes es,nothing
	les	si,lpDriverPDevice
lock	or	es:[si].deFlags,BUSY
        ret
DevToBackground endp

;----------------------------------------------------------------------------
; DisableSwitching
; This function is called whenever we need to prevent a screen switch
; from occuring.
;----------------------------------------------------------------------------
PPROC DisableSwitching near
        mov     ax,ENTER_CRIT_REG
        int     2fh
        or      SwitchControl,PREVENT_SWITCH
        ret
DisableSwitching endp

;----------------------------------------------------------------------------
; EnableSwitching
; This function is called whenever we can allow a screen group switch.
;----------------------------------------------------------------------------
PPROC EnableSwitching near
        assumes ds,Data
        assumes es,nothing
        mov     ax,EXIT_CRIT_REG
        int     2fh
        and     SwitchControl,not PREVENT_SWITCH
        ret
EnableSwitching endp
sEnd    Code
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;----------------------------------------------------------------------------
; HookInt2Fh
;
; Installs a link in the 2Fh multiplex interrupt chain to watch for
; calls to the driver to save or restore the state of the display
; hardware before a context switch.
;
; This function is called whenever the driver recieves an enable call.
;----------------------------------------------------------------------------
PPROC HookInt2Fh        near
        assumes ds,Data
        assumes es,nothing
        push    bx
        push    cx
        push    dx
        push    ds
        push    es
        mov     ax,wptr [RepaintAddr][0]        ;Do we already have User's
        or      ax,wptr [RepaintAddr][2]        ;repaint address?
        jnz     HI2F_HookTheThang               ;Yes. Go hook the interrupt.
        mov     ax,DataOFFSET szUSER    ;No. Get User's module
        farPtr  module_name,ds,ax               ;handle.
        cCall   GetModuleHandle,<module_name>
        xchg    ax,bx
        mov     ax,REPAINT_EXPORT_INDEX
        cwd
        farPtr  func_number,dx,ax
        cCall   GetProcAddress,<bx,func_number> ;Get the address of User's
        mov     wptr [RepaintAddr][0],ax        ;repaint routine and save
        mov     wptr [RepaintAddr][2],dx        ;away for later use.

        mov     ax,DataOFFSET szGDI     ;Now lets get the address
        farPtr  module_name,ds,ax               ;of GDI's color matching
        cCall   GetModuleHandle,<module_name>   ;routine.
        xchg    ax,bx
        mov     ax,DEVICECOLORMATCH_EXPORT_INDEX
        cwd
        farPtr  func_number,dx,ax
        cCall   GetProcAddress,<bx,func_number>
        mov     wptr [GetColorAddr][0],ax       ;Save it away too. This
        mov     wptr [GetColorAddr][2],dx       ;is called by the DIB code.

HI2F_HookTheThang:
        cli                             ;Disable interrupts while hooking
        or      SwitchControl,INT_2F_HOOKED ;this puppy....
        xor     ax,ax
        push    ds                      ;save or Data segment
        mov     ax,CodeBASE             ;get the CS selector
        cCall   AllocCSToDSAlias,<ax>   ;get a data segment alias
        mov     ds,ax                   ;Now we can write into our code segment.
        push    ax                      ;save for later FreeSelector call
        assumes ds,Code                 ;do this to save address in CS
        mov     ax,3500h+2Fh            ;get the vector
        int     21h                     ;Ask DOS for the old vector.
        mov     wptr PrevInt2Fh[0],bx   ;Save off the previous vector.
        mov     wptr PrevInt2Fh[2],es
        mov     dx,CodeOFFSET ScreenSwitchHook ;This will be the new vector.
        mov     ax,CodeBASE
        mov     ds,ax
        assumes ds,nothing
        mov     ax,2500h+2Fh
        int     21h                     ;Ask DOS to install the new vector.
        cCall   FreeSelector            ;selector is on the stack
        pop     ds                      ;get back own data segment
        assumes ds,Data
        sti                             ;Turn interrupts back on.
HI2F_Done:
        pop     es
        assumes es,nothing
        pop     ds
        assumes ds,nothing
        pop     dx
        pop     cx
        pop     bx
        ret
HookInt2Fh      endp

;----------------------------------------------------------------------------
; RestoreInt2Fh
;
; If we installed ourselves into int 2Fh, we'll restore the previous
; vector.
; This function is called whenever the driver receives a disable call.
;----------------------------------------------------------------------------
PPROC RestoreInt2Fh near
        assumes es,nothing
        mov     ax,DGROUP
        mov     es,ax
        assumes es,Data                 ;es = Data
        test    SwitchControl,INT_2F_HOOKED
        jz      RI2F_Done
        cli
        and     SwitchControl,not INT_2F_HOOKED
        push    es
        push    ds
        mov     ax,CodeBASE             ; get the code address
        cCall   AllocCSToDSAlias,<ax>   ; get a data alias out of it
        mov     es,ax
        push    ax                      ; save for FreeSelector call
        assumes es,Code
        push    dx                      ; save
        lds     dx,PrevInt2Fh           ; get the saved vector
        mov     ax,252fh                ; Call DOS to set vector 2Fh.
        int     21h
        pop     dx                      ; restore
        sti
        cCall   FreeSelector            ; selector is on the stack
        pop     ds
        assumes ds,nothing
        pop     es
        assumes es,Data
RI2F_Done:
        ret
RestoreInt2Fh endp
sEnd    InitCode
end
