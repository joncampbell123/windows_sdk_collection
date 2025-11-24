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
; CHIPS.ASM 
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
; E X T E R N S
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin Data
sEnd

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;----------------------------------------------------------------------------
; Is_Chips
; Exit:
;  ax:dx == !0 (true) if detected.
;----------------------------------------------------------------------------
cProc	Is_Chips,<NEAR,PUBLIC,PASCAL,NODATA>,<si,di>
cBegin
	cli			;Turn off interrupts so mouse can't affect us.
	xor	bx,bx		;assume failure.
	mov	dx,3d6h
	in	al,dx
	cmp	al,0FFh
	je	ISC_Done
	mov	al,0
	out	dx,al
	in	ax,dx
        cmp     al,0
        jne     ISC_Done
	cmp	ah,0
	je	ISC_Done
	cmp	ah,0FFh
	je	ISC_Done
	mov	bx,CHIPS	;Show success.
	and	ah,0F8h
	cmp	ah,10h
	jne	ISC_Done
	mov	bx,CHIPS452

PLABEL ISC_Done
	sti
	mov	ax,bx
	xor	dx,dx
cEnd

sEnd	InitSeg
end
