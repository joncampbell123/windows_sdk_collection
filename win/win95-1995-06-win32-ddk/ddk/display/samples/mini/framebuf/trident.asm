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
; TRIDENT.ASM 
;----------------------------------------------------------------------------
	.xlist
	include macros.inc
	DOS5=1
	include cmacros.inc
	include device.inc
	.list

;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externA __C000h 	;kernel selector for physical c000:0
	externA __E000h 	;kernel selector for physical e000:0

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin Data
TVGA_Sig	DB	"TRIDENT MICROSYSTEMS"
TVGA_Sig_Len	EQU	$-TVGA_Sig
sEnd

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;----------------------------------------------------------------------------
; Is_Trident
; Exit:
;  ax:dx == TRIDENT if detected.
;----------------------------------------------------------------------------
cProc	Is_Trident,<NEAR,PUBLIC,PASCAL,NODATA>,<si,di>
cBegin
	xor	ax,ax			;assume failure.

;----------------------------------------------------------------------------
; Try to find "TRIDENT MICROSYSTEMS" from ROM BIOS.
; Search C000h. If not found, try E000h, some motherboard makers
; put our BIOS there.
; - Trident, Henry Zeng
;----------------------------------------------------------------------------
	mov	bx,__C000h
	mov	es,bx

PLABEL IST_SearchROM
	xor	di,di
	mov	cx,128

PLABEL IST_SearchString
	lea	si,TVGA_Sig
	push	di
	push	cx
	mov	cx, TVGA_Sig_Len
	repe	cmpsb  			; ? Trident
	pop	cx
	pop	di
	jz	short IST_Success	; Gotcha!
	inc	di
	dec	cx
	jnz	IST_SearchString

	mov	bx,es
	cmp	bx,__E000h
	je	IST_Not
	mov	bx,__E000h
	mov	es,bx
	jmp	IST_SearchROM

PLABEL IST_Success
	mov	ax,TRIDENT		;Show success.

PLABEL IST_Not
	xor	dx,dx
cEnd

sEnd	InitSeg
end
