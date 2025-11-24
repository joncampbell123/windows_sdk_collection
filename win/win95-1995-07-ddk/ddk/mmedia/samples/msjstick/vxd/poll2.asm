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
;   File:	poll2.asm
;   Content:	code to poll 2 axes on either joy1 or joy2
;
;---------------------------------------------------------------------------;

	.386p

;---------------------------------------------------------------------------;
;			     I N C L U D E S
;---------------------------------------------------------------------------;

	.xlist
	include vmm.inc
	include debug.inc
	include configmg.inc
	include mmdevldr.inc
	include vjoyd.inc
	include poll.inc
	.list

VxD_LOCKED_CODE_SEG

public _jsPoll2@4
_jsPoll2@4 PROC

    push    ebx

    ;
    ; get the button state, set up for polling:
    ; bl = shift to get to X bit
    ; bh = shift to get to Y bit
    ; ch = mask for X & Y
    ;
    mov	    edx, _dwJoyPort		; get whole port state
    in	    al, dx
    mov	    edx, [esp+8]		; id
    mov	    bl, _cXshift[edx]		; set up X shift
    mov	    bh, _cYshift[edx]		; set up Y shift
    mov	    ch, _cXYBits[edx]		; set up mask 
    mov	    cl, _cButtonShift[edx]	; shift for buttons
    shr	    al,cl			; shift to get button state	
    mov	    edx, 15			; base button bit mask
    sub	    cl,4			; get amount to shift button mask by
    shr	    edx,cl			; get button mask for joystick
    and	    eax, edx			; off all other bits to make sure
    xor	    eax, edx			; invert sense (port gives reverse logic)
    mov	    _dwButtons, eax		; save it
    
    ;
    ; We poll the analog ports that we are interested in twice, once to
    ; make sure that the monostable vibrators have stabilized since the
    ; last time they were polled and once to actually read the positions.
    ; The first poll is almost always a single pass (ie, things are stable)
    ; but this one pass helps ensure that the entire loop is in cache.
    ;

    pushfd
    cli

    mov	    _bPass1, 1			; loop once to make sure bits are stable
    
real_poll:
    xor	    eax,eax
    mov	    _dwXpos, eax		; initialize the X position counter
    mov	    _dwYpos, eax		; initialize the Y position counter
    
    cmp	    _bPass1, 1
    je	    short poll_loop		; only start timer on second poll
    mov	    edx, _dwJoyPort		; get port state
    out	    dx, al			; start the timers 
    jmp	    short poll_loop

; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.
	
    align   4
poll_loop:
    mov	    edx, _dwJoyPort		; get port state
    in	    al, dx			; get position info
    and	    al, ch			; only watch bits of interest
    jz	    short poll_done		; counters hit zero, exit

;
; increment the X counter
;
    mov	    cl,bl			; get # of bits to shift to get to X
    mov     edx,eax			; get port result
    shr	    edx,cl			; shift bit down to first position
    and	    edx,1			; ditch all other bits
    add	    _dwXpos,edx			; add to current count
    mov	    edx,_dwTimeOut		; timeout value
    cmp	    edx, _dwXpos		; did we time out?
    jae	    short do_y	 		; nope
    xor	    eax, eax			; yes, call joystick unplugged
    jmp	    short poll_exit
    
;
; increment the Y counter
;
do_y:
    mov	    cl,bh			; get # of bits to shift to get to Y
    mov     edx,eax			; get port result
    shr	    edx,cl			; shift bit down to first position
    and	    edx,1			; ditch all other bits
    add	    _dwYpos,edx			; add to current count
    mov	    edx,_dwTimeOut		; timeout value
    cmp	    edx, _dwYpos		; did we time out?
    jae	    short poll_loop 		; nope
    xor	    eax, eax			; yes, call joystick unplugged
    jmp	    short poll_exit

;
; done this polling pass
;
poll_done:				; end of poll loop
    dec	    _bPass1			; pass countdown
    jz	    real_poll			; keep going if 0 (2nd pass)
    mov	    eax,1			; everything worked OK

poll_exit:
    popfd				; restore flags (including interrupt)
    pop	    ebx
    ret	    4

_jsPoll2@4 ENDP

VxD_LOCKED_CODE_ENDS

end
