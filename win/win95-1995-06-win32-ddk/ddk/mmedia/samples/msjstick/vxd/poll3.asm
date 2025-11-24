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
;   File:	poll3.asm
;   Content:    polls any 3 axes
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

public _jsPoll3@8
_jsPoll3@8 PROC

    push    ebx
    
;
; get the button state
;
    
    mov	    edx, _dwJoyPort		; port address
    in	    al, dx			; get button bits
    shr	    al, 4
    and	    eax, 15			; isolate button bits
    xor	    eax, 15			; invert sense (port gives reverse logic)
    mov	    _dwButtons, eax		; save it

;
; We poll the analog ports that we are interested in twice, once to
; make sure that the monostable vibrators have stabilized since the
; last time they were polled and once to actually read the positions.
; The first poll is almost always a single pass (ie, things are stable)
; but this one pass also helps ensure that the entire loop is in cache.
;

    pushfd
    cli

    mov	    _bPass1, 1			; loop once to make sure bits are stable
    
real_poll:
    xor	    eax,eax
    mov	    _dwXpos, eax		; initialize the X position counter
    mov	    _dwYpos, eax		; initialize the Y position counter
    mov	    _dwZpos, eax		; initialize the Z position counter
    
    mov	    ebx, dword ptr [esp+12]	; id
    
    cmp	    dword ptr [esp+16], 0	; is_r?
    jne	    is_r			; yes, go set up for R
    mov	    ah,_cZshift[ebx]		; no, then shift 3rd axis for Z
    jmp	    short @f
is_r:
    mov	    ah,_cRshift[ebx]		; shift 3rd axis for R
@@:
    
    cmp	    _bPass1, 1
    je	    short poll_loop		; only start timer on second poll
    mov	    edx, _dwJoyPort		; port address
    out	    dx, al			; start the timers
    jmp	    short poll_loop

;
; When reading joystick port, the bit for a pot starts at 1, and when
; the resistance value for a counter is greater than or equal to the
; pot's, the bit goes to zero, and stays there.
;
	
    align   4
poll_loop:
    mov	    edx, _dwJoyPort		; get port state
    in	    al, dx			; get position info
    and	    al, _c3Bits[ebx]		; only watch bits of interest
    jz	    poll_done 			; counters hit zero, done
    
;
; increment the X counter
;
    mov	    cl,_cXshift[ebx]		; get # of bits to shift to get to X
    mov     edx,eax			; get port result
    shr	    edx,cl			; shift bit down to first position
    and	    edx,1			; ditch all other bits
    mov	    ecx,_dwTimeOut		; timeout value
    add	    _dwXpos,edx			; add to current count
    cmp	    ecx, _dwXpos		; did we time out?
    jae	    short do_y	 		; nope
    xor	    eax, eax			; yes, call joystick unplugged
    jmp	    short poll_exit
    
;
; increment the Y counter
;
do_y:
    mov	    cl,_cYshift[ebx]		; get # of bits to shift to get to Y
    mov     edx,eax			; get port result
    shr	    edx,cl			; shift bit down to first position
    and	    edx,1			; ditch all other bits
    mov	    ecx,_dwTimeOut		; timeout value
    add	    _dwYpos,edx			; add to current count
    cmp	    ecx, _dwYpos		; did we time out?
    jae	    short do_z	 		; nope
    xor	    eax, eax			; yes, call joystick unplugged
    jmp	    short poll_exit
    
;
; increment the Z counter
;
do_z:
    mov	    cl,ah			; get # of bits to shift to get to Z/R
    mov     edx,eax			; get port result
    shr	    edx,cl			; shift bit down to first position
    and	    edx,1			; ditch all other bits
    mov	    ecx,_dwTimeOut		; timeout value
    add	    _dwZpos,edx			; add to current count
    cmp	    ecx, _dwZpos		; did we time out?
    jae	    short poll_loop		; nope
    xor	    eax, eax			; yes, call joystick unplugged
    jmp	    short poll_exit
    
;
; done this polling pass
;
poll_done:				; end of poll loop
    dec	    _bPass1			; no longer on pass 1
    jz	    real_poll			; keep going if 0 (2nd pass)
    mov	    eax,1			; everything worked OK

poll_exit:
    popfd				; restore flags (including interrupt)
    
    mov	    ecx,_dwZpos			; copy Z value into R position
    mov	    _dwRpos,ecx
    
    pop	    ebx
    
    ret	    8

_jsPoll3@8 ENDP

VxD_LOCKED_CODE_ENDS

end
