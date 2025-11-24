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
; V7.ASM 
;----------------------------------------------------------------------------
	.xlist
	include macros.inc
	DOS5=1
	include cmacros.inc
	include device.inc
	.list

;----------------------------------------------------------------------------
; E Q U A T E S
;----------------------------------------------------------------------------

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg

;----------------------------------------------------------------------------
; Is_V7
; Exit:
;  ax:dx == !0 (true) if detected.
;----------------------------------------------------------------------------
cProc	Is_V7,<NEAR,PUBLIC,PASCAL,NODATA>,<si>
cBegin
;
; check for any V7 board
;
	xor	bx,bx		; clear it out
        mov     ax,6f00h
	int	10h
        xor     si,si           ;assume failure
	cmp	bx,'V7'         ; any of the products?
	jnz	SHORT ISV7_Not	; nope...
IF 0
;
; check the chip version #
;
	mov	cx,0ffffh
	mov	ax,06f07h	; get the # from the bios
	int	10h
        xor     si,si           ;assume failure
	xor	dx,dx		; Assume no flags passed in
	or	cx,cx		; zero?
	jne	SHORT ISV7_Not
	cmp	bh,70h		; V7VGA chip?
	jl	SHORT ISV7_Not	; nope...  must be in range 70h-7fh
ENDIF
;
; if we get here it is a video seven board with all the trimmings

PLABEL ISV7_Success
	mov	si,V7		;Show success.

PLABEL ISV7_Not
	mov	ax,si
	xor	dx,dx
cEnd

sEnd	InitSeg
end
