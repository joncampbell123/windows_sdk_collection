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
; WONDER.ASM 
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
	externA __C000h 	;kernel selector for physical c000:0

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin Data
ATI_Sig 	DB  " 761295520"    ; ATI signature at in ROM at offset 30
ATI_Sig_Len	EQU $-ATI_Sig
sEnd


;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;----------------------------------------------------------------------------
; Is_Wonder
; Exit:
;  ax:dx == !0 (true) if detected.
;----------------------------------------------------------------------------
cProc	Is_Wonder,<NEAR,PUBLIC,PASCAL,NODATA>,<si,di>
cBegin
	cli			;Turn off interrupts so mouse can't affect us.

	push	bp
	mov	ax,1203h
	mov	bx,5506h
	mov	bp,0ffffh
	int	10h
	xor	bx,bx		;Assume failure.
	cmp	bp,0ffffh
	pop	bp
	je	short ISW_Not
	mov	si,OFFSET ATI_Sig
	mov	di,__C000h
	mov	es,di
	mov	di,30h
	mov	cx, ATI_Sig_Len
	rep cmpsb
	jnz	SHORT ISW_Not

; Further checks for version of ATI VGA

	cmp	es:byte ptr [40h],'3'	;VGAWONDER product code ?
	jne	SHORT ISW_Not


PLABEL ISW_Success
	mov	bx,ATI		;Show success.

PLABEL ISW_Not
	sti
	mov	ax,bx
	xor	dx,dx
cEnd

sEnd	InitSeg
end
