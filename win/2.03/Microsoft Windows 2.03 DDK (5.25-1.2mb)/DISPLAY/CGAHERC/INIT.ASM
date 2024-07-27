	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: INIT.ASM
;
; CGA/Hercules initialization code.
; 
; Created:  June 1987
; Author: *** ******
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


createSeg _INIT,InitSeg,byte,public,CODE
sBegin	InitSeg
assumes cs,InitSeg
page

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
;	Thu 01-Oct-1987 -by- *** ******    [******]
;	Wrote it.
;-----------------------------------------------------------------------;

;----------------------------Pseudo-Code--------------------------------;
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc	dev_initialization,<NEAR,PUBLIC>

cBegin

	mov	ax,1			;no way to have error

cEnd

sEnd	InitSeg
end
