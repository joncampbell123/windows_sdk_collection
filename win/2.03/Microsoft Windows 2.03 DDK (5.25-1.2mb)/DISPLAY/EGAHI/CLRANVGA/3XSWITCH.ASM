	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	3XSWITCH.ASM
;
;   This module contains the functions:
;
;	dev_to_background
;	dev_to_foreground
;
;   These functions support screen switching in and out of the OS/2
;   compatibility box.
;
; Created: 16-Sep-1987
; Author:  *** ****** [******]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	none
;
; Public Functions:	dev_to_background
;			dev_to_foreground
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;

	.xlist
	include cmacros.inc
	include macros.mac
	include	ega.inc
	include	egamem.inc
	.list

	??_out	3xswitch

	public		dev_to_foreground
	public		dev_to_background

	externA		ScreenSelector
	externNP	save_hw_regs
	externNP	res_hw_regs


sBegin	Code
assumes cs,Code
	page

	externB		Code_palette

;---------------------------Public-Routine-----------------------------;
; dev_to_foreground
;
; Performs any action necessary to restore the state of the display
; hardware prior to becoming the active context.
;
; Entry:
;	nothing
; Returns:
;	Any bits saved by SaveScreenBitmap in the upper page of EGA
;	memory have been invalidated by changing flags in
;	shadow_mem_status.
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
; History:
;	Wed 16-Sep-1987 20:17:08 -by-  *** ****** [******]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


dev_to_foreground	proc	near


	mov	ax,cs			;reset the palette.
	mov	es,ax
	assumes	es,nothing
	mov	dx,CodeOFFSET Code_palette
	mov	ax,1002h
	int	10h

	mov	dx,EGA_BASE + SEQ_ADDR	;the sequence address register must
	mov	al,SEQ_MAP_MASK		;be pointing to the map mask register
	out	dx,al

	mov	ax,ScreenSelector	;res_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	res_hw_regs


;	Invalidate any save we'd done to the upper page of EGA memory,
;	because we can't know if it was preserved.

	or	shadow_mem_status,SHADOW_TRASHED

	ret

dev_to_foreground	endp

	page

;---------------------------Public-Routine-----------------------------;
; dev_to_background
;
; Performs any action necessary to save the state of the display
; hardware prior to becoming an inactive context.
;
; Entry:
;	nothing
; Returns:
;	nothing
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	none
; History:
;	Wed 16-Sep-1987 20:17:08 -by-  *** ****** [******]
;	Wrote it.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


dev_to_background	proc	near

	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	save_hw_regs

	ret

dev_to_background	endp

sEnd	Code
	end
