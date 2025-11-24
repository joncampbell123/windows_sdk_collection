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
; OAK.ASM 
;----------------------------------------------------------------------------
	.xlist
	include macros.inc
	DOS5=1
	include cmacros.inc
	include	device.inc
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
; Is_Oak
; Exit:
;  ax:dx == OAK if detected.
;----------------------------------------------------------------------------
cProc	Is_Oak,<NEAR,PUBLIC,PASCAL,NODATA>,<si>
cBegin
        xor     si,si           ;assume failure
	xor	cx,cx		;initial nesting level is zero.
	cli			;Turn off interrupts so mouse can't affect us.

	mov	dx,3deh
	PushReg			;save 3de(?)

	mov	al,9
	out	dx,al
	PushReg			;save 3de(9)

	inc	dx
	xor	al,al
	out	dx,al

; Put something else out on the bus to avoid bus noise.

	dec	dx
	mov	al,9
	out	dx,al
	inc	dx
	IO_Delay

	in	al,dx
	or	al,al
	jnz	short ISO_Not
	mov	al,55h
	out	dx,al

	dec	dx
	mov	al,9
	out	dx,al
	inc	dx
	IO_Delay

	in	al,dx
	cmp	al,55h
	jne	short ISO_Not

PLABEL ISO_Success
	mov	si,OAK		;Show success.

PLABEL ISO_Not
@@:	PopReg
	jnz	@b
	sti
	mov	ax,si
	xor	dx,dx
cEnd

sEnd	InitSeg
end
