	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: INIT.ASM
;
; CGA/Hercules initialization code.
; 
; Created:  June 1987
; Author: Bob Grudem
;
; Copyright (c) 1985, 1986, 1987  Microsoft Corporation
;
; This module handles any device initialization beyond that done in
; physical_enable.
;-----------------------------------------------------------------------;

	title	CGA/Hercules Initialization Code

incDevice = 1				;allow assembly of needed constants

	.xlist
	include cmacros.inc
	include gdidefs.inc
	.list

	??_out	init

	externA		ScreenSelector	; the selector for display memory
	externFP      	AllocCSToDSAlias; get a data seg alias for CS
	externFP	FreeSelector	; Frees the selector

	

sBegin	Data

	externW ssb_mask	;Mask for save screen bitmap bit

sEnd	Data


createSeg _INIT,InitSeg,byte,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
page

	externW	physical_device

;--------------------------Public-Routine-------------------------------;
; dev_initialization - device specific initialization
;
; Any device specific initialization is performed.
;
; Entry:
;	None
; Returns:
;	AX = 1
; Registers Preserved:
;	SI,DI,BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	int 10h
; History:
;	Thu 01-Oct-1987 -by- Bob Grudem    [bobgru]
;	Wrote it.
;-----------------------------------------------------------------------;

;----------------------------Pseudo-Code--------------------------------;
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc	dev_initialization,<NEAR,PUBLIC>

cBegin
;----------------------------------------------------------------------------;
; at this point we will fill in the  video memory segment addresses into the ;
; pdevice data steucture.					             ;
;----------------------------------------------------------------------------;
	push	es			; save
	cCall	AllocCSToDSAlias,<cs>	; get a data segment alias for cs
	mov	es,ax			; es has this code segment
	assumes	es,InitSeg
	
	mov	ax,ScreenSelector	; get the address of memory
	mov	word ptr es:[physical_device.bmType],ax
	mov	word ptr es:[physical_device.bmBits+2],ax


	mov	ax,es			; get the selector
	xor	bx,bx			; for es
	mov	es,bx			; have it in es till pop
	cCall	FreeSelector,<ax>	; free it
	pop	es			; restore es

;----------------------------------------------------------------------------;

	mov	ax,1			;no way to have error

cEnd

sEnd	InitSeg
end
