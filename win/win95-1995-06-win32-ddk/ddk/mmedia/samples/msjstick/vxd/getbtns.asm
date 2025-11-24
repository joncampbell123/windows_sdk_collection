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
;   History:	getbtns.asm
;   Content:	retrieve just the button state
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

VxD_PAGEABLE_CODE_SEG

public _jsGetButtons@4
_jsGetButtons@4 PROC

    ; get the button state

    mov	    edx, _dwJoyPort		; get whole port state
    in	    al, dx
    shr	    al,4			; shift 4 bits for J1 button state	
    mov	    edx, 15			; button bit mask
    cmp	    dword ptr [esp+4], 0	; id = joystick 1?   
    jz	    short @f			; yes, skip J2 setup
    shr	    al,2			; shift 6 bits for J2 button state
    mov	    edx, 3			; button bit mask
@@:
    and	    eax, edx			; off all other bits to make sure
    xor	    eax, edx			; invert sense (port gives reverse logic)
    mov	    _dwButtons, eax		; save it
    
    ret	    4

_jsGetButtons@4 ENDP

VxD_PAGEABLE_CODE_ENDS

end
