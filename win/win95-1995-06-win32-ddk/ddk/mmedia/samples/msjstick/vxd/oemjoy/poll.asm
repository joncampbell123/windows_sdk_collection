;---------------------------------------------------------------------------;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   File:	pollrtn.asm
;   Content:	code to poll only one axis of one joystick
;
;---------------------------------------------------------------------------;

	.386p

	.xlist
	include vmm.inc
	include debug.inc
	include configmg.inc
	.list
	
JOY1_X_MASK   equ <0001h>   	    ; mask for joystick 1 X port bits
JOY1_Y_MASK   equ <0002h>   	    ; mask for joystick 1 Y port bits
JOY2_X_MASK   equ <0004h>           ; mask for joystick 2 X port bits
JOY2_Y_MASK   equ <0008h>           ; mask for joystick 2 Y port bits
JOYPORT       equ 0201H             ; port address

extern _dwPos:DWORD
extern _dwButtons:DWORD
extern _dwTimeOut:DWORD
extern _bPass1:DWORD

VxD_LOCKED_CODE_SEG

public _pollIt@8
_pollIt@8 PROC

    push    ebx
    
    ; get the button state

    mov	    edx, JOYPORT		; get whole port state
    in	    al, dx
    shr	    al, 4
    and	    eax, 15			; off all other bits to make sure
    xor	    eax, 15			; invert sense (port gives reverse logic)
    mov	    _dwButtons, eax		; save it
    
    ; We poll the analog ports that we are interested in twice, once to
    ; make sure that the monostable vibrators have stabilized since the
    ; last time they were polled and once to actually read the positions.
    ; The first poll is almost always a single pass (ie, things are stable)
    ; but this one pass helps ensure that the entire loop is in cache.

    pushfd
    cli

    mov	    _bPass1, 1			; loop once to make sure bits are stable
    jmp	    short real_poll
    align   4
real_poll:
    xor	    eax,eax
    mov	    _dwPos, eax			; initialize the position counter
    cmp	    dword ptr [esp+12],0	; id = joystick 1?
    jne	    short set_2			; no, joystick 2
    cmp	    dword ptr [esp+16], 1	; do_y = y axis?
    je	    short set_1_y		; yes, go set
    mov	    bl, JOY1_X_MASK		; poll joystick 1, x axis
    jmp	    short poll_timers
set_1_y:
    mov	    bl, JOY1_Y_MASK		; poll joystick 1, y axis
    jmp	    short poll_timers
set_2:
    cmp	    dword ptr [esp+16],1	; do_y = y axis?
    je	    short set_2_y		; yes, go set
    mov	    bl, JOY2_X_MASK		; poll joystick 2, x axis
    jmp	    short poll_timers
set_2_y:
    mov	    bl, JOY2_Y_MASK		; poll joystick 2, y axis
    jmp	    short poll_timers
    
poll_timers:
    cmp	    _bPass1, 1
    je	    short poll_loop		; only start timer on second (real) poll
    xor	    eax, eax
    mov	    edx, JOYPORT		; get port state
    out	    dx, al			; start the timers (output a zero byte)
    jmp	    short poll_loop

; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.
	
    align   4
poll_loop:
    mov	    edx, JOYPORT	    ; get port state
    in	    al, dx		    ; get position info
    and	    al, bl		    ; only watch bits of interest
    jz	    short poll_done	    ; counters hit zero?
    inc	    _dwPos
    mov	    eax,_dwPos		    ; get value
    cmp	    _dwTimeout,eax	    ; overflow?
    jae	    short poll_loop	    ; nope
    mov	    eax,0		    ; yes, done
    jmp	    short poll_exit
    
poll_done:		      ; end of poll loop
    mov	    eax,1	      ; everything worked OK
    dec	    _bPass1	      ; no longer on pass 1
    jnz	    short poll_exit   ; stop if this was the second (real) poll 
    jmp	    short real_poll   ; otherwise go back and do the real one

poll_exit:
    popfd		      ; restore flags (including interrupt)
    pop	    ebx
    ret	    8

_pollIt@8 ENDP

VxD_LOCKED_CODE_ENDS

end
