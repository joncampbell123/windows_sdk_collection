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
; Exported Functions:	none
;
; Public Functions:	dev_to_background
;			dev_to_foreground
;			dev_to_save_regs
;			dev_to_res_regs
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
	include macros.inc
	include	mflags.inc
	include	ega.inc
	include	egamem.inc
	.list

	??_out	3xswitch

	public		dev_to_foreground
	public		dev_to_background
	public		dev_to_save_regs
	public		dev_to_res_regs

sBegin	Data
	externW wGraphicsMode
sEnd	Data

	externA		ScreenSelector
	externNP	save_hw_regs
	externNP	res_hw_regs
	externFP	init_hw_regs
	externFP	SetMode
	externFP	SetMode
	externFP	SetColors

sBegin	Code
assumes cs,Code
	page

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
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


dev_to_foreground	proc	near
	mov	ax,wGraphicsMode
	or	al,80h
	call	SetMode
	call	SetColors

DTF_InitHWRegs:

;	mov	dx,EGA_BASE + SEQ_ADDR	;the sequence address register must
;	mov	al,SEQ_MAP_MASK		;be pointing to the map mask register
;	out	dx,al

	mov	ax,ScreenSelector	;res_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	init_hw_regs

;
;Now, erase the screen to black:
;
	call	ClearVGAScreen_Far	;make sure screen is black
	ret

dev_to_foreground	endp

 
;----------------------------------------------------------------------------;
; procedure to restore th device registers.				     ;
;----------------------------------------------------------------------------;


dev_to_res_regs	proc	near
	call	SetColors

;	mov	dx,EGA_BASE + SEQ_ADDR	;the sequence address register must
;	mov	al,SEQ_MAP_MASK		;be pointing to the map mask register
;	out	dx,al

	mov	ax,ScreenSelector	;res_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	res_hw_regs


;	Invalidate any save we'd done to the upper page of EGA memory,
;	because we can't know if it was preserved.

	or	shadow_mem_status,SHADOW_TRASHED

	ret

dev_to_res_regs	endp

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

;	call	save_hw_regs

	ret

dev_to_background	endp

;----------------------------------------------------------------------------;
; procedure called when device is asked to restore its registers             ;
;----------------------------------------------------------------------------;

dev_to_save_regs	proc	near

	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	save_hw_regs

	ret

dev_to_save_regs	endp

sEnd	Code
;
;
subttl		Clear the VGA Screen To Black
page +
createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg
;
;
cProc		ClearVGAScreen_Far,<FAR,PUBLIC>
;
cBegin		<nogen>
.386
;
;We clear the screen to black upon returning from a DOS box:
;
	assumes ds,Data
	cld				;set direction forward
	mov	dx,3ceh 		;DX --> GCR index register
	in	al,dx			;get current value of GCR index register
	mov	bl,al			;save it in BL
	mov	al,5			;set to GCR Mode register
	out	dx,al			;
	inc	dx			;
	in	al,dx			;get current value of GCR mode register
	mov	bh,al			;and save it in BH
	mov	al,2			;set to write mode 2
	out	dx,al			;
	ror	ebx,16			;save GCR data in high word of EBX
;
;Now, enable all four planes for writing:
;
	mov	dl,0c4h 		;
	in	al,dx			;get current value of Seq index register
	mov	bl,al			;save it in BL
	mov	al,2			;set to Seq plane mask register
	out	dx,al			;
	inc	dx			;
	in	al,dx			;get current value of Seq mask register
	mov	bh,al			;and save it in BH
	mov	al,0fh			;set to "all planes on"
	out	dx,al			;
;
;Now, write out 64K of zeros to clear the VGA memory:
;
	xor	eax,eax 		;this is value to write
	mov	ecx,(64*1024)/4 	;this is number of DWORDS to write
	mov	di,ScreenSelector	;make ES:EDI --> start of VRAM
	mov	es,di			;
	xor	edi,edi 		;make sure EDI == 0
	rep	stos dword ptr es:[edi] ;clear the memory
;
;Now restore the Sequencer registers back to the way they were:
;
	mov	al,bh			;restore Seq map mask register
	out	dx,al			;
	dec	dx			;
	mov	al,bl			;restore Seq index register
	out	dx,al			;
;
;And lastly, restore the GCR registers:
;
	ror	ebx,16			;regain access to saved GCR stuff
	mov	dl,0cfh 		;DX --> GCR data register
	mov	al,bh			;restore GCR mode register
	out	dx,al			;
	dec	dx			;DX --> GCR index register
	mov	al,bl			;restore GCR index register
	out	dx,al			;
;
CVSFExit:
	retf
.286c
cEnd		<nogen>
;
;
sEnd		InitSeg

end
