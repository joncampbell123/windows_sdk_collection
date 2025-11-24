    page    , 132
;---------------------------------------------------------------------------;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   File:	poll1.asm
;   Content:	code to poll only one axis of one joystick
;
;---------------------------------------------------------------------------;

	.386p

	.xlist
	include vmm.inc
	include debug.inc
	include configmg.inc
	include mmdevldr.inc
	include vjoyd.inc
	include poll.inc
	.list
	
JOY_AXIS_X	equ	0
JOY_AXIS_Y	equ	1
JOY_AXIS_Z	equ	2
JOY_AXIS_R	equ	3

VxD_LOCKED_CODE_SEG

public _jsPoll1@8
_jsPoll1@8 PROC

    push    ebx
    
    ; get the button state

    mov	    edx, _dwJoyPort		; get whole port state
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
    mov	    _dwXpos, eax		; initialize the X position counter
    
    mov	    ecx, dword ptr[esp+12]	; id
    mov	    eax, dword ptr[esp+16]	; axis
    cmp	    eax, JOY_AXIS_X
    jne	    short try_y
    mov	    bl,_cXbit[ecx]
    jmp	    short poll_timers
try_y:
    cmp	    eax, JOY_AXIS_Y
    jne	    short try_z
    mov	    bl,_cYbit[ecx]
    jmp	    short poll_timers
try_z:
    cmp	    eax, JOY_AXIS_Z
    jne	    short try_r
    mov	    bl,_cZbit[ecx]
    cmp	    bl,0			; is there a bit to test?
    jne	    short poll_timers		; yes
    mov	    bl, JOY2_Y_MASK		; no, set default value
    jmp	    short poll_timers
try_r:
    cmp	    eax, JOY_AXIS_R
    jne	    short bad_axis
    mov	    bl,_cRbit[ecx]
    cmp	    bl,0			; is there a bit to test?
    jne	    short poll_timers		; yes
    mov	    bl, JOY2_X_MASK		; no, set default value
    jmp	    short poll_timers
bad_axis:
    xor	    eax,eax
    jmp	    short poll_exit
    
poll_timers:
    cmp	    _bPass1, 1
    je	    short poll_loop		; only start timer on second (real) poll
    xor	    eax, eax
    mov	    edx, _dwJoyPort		; get port state
    out	    dx, al			; start the timers (output a zero byte)
    jmp	    short poll_loop

; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.
	
    align   4
poll_loop:
    mov	    edx, _dwJoyPort		; get port state
    in	    al, dx			; get position info
    and	    al, bl			; only watch bits of interest
    jz	    short poll_done		; counters hit zero?
    inc	    _dwXpos
    mov	    eax,_dwXpos			; get value
    cmp	    _dwTimeout,eax		; overflow?
    jae	    short poll_loop		; nope
    mov	    eax,0			; yes, done
    jmp	    short poll_exit
    
poll_done:				; end of poll loop
    dec	    _bPass1			; no longer on pass 1
    jz	    real_poll			; keep going if 0 (2nd pass)
    mov	    eax,1			; everything worked OK

poll_exit:
    popfd				; restore flags (including interrupt)
    pop	    ebx
    ret	    8

_jsPoll1@8 ENDP

VxD_LOCKED_CODE_ENDS

end
