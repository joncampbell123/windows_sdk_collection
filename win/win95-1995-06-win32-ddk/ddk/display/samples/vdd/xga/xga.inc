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

;
;Include file for MiniVDD definitions:
;

;
;Following are the POS device ID equates for XGA/1 (original) and XGA/2
;(the later one which did non-interlaced modes):
;
XGA1_ID 	equ	8fdbh
XGA2_ID 	equ	8fdah
AGX_ID		equ	8fd9h
;
;Equates for DAC virtualization:
;
RED_CYCLE	equ	0
GREEN_CYCLE	equ	1
BLUE_CYCLE	equ	2
DONE_WITH_CYCLE equ	3

;
;The PerVMData structure.  This structure represents the our reserved space
;in the so-called CB data structure.  The CB data structure is created for
;each VM started in the system.  We can therefore, save states that we need
;to remember on a per-VM basis.  This data will vary for every mini-VDD.
;The XGA chipset requires a few states for VESA support only.
;
PerVMData	struc
	CurrentMode	dw	?		;current mode that VM is in
	DACWriteIndex	db	?
	DACReadIndex	db	?
	DACWriteStatus	db	?
	DACReadStatus	db	?
	AGXIndex	db	?
	XGAFiller	db	?		;reclaim as needed
PerVMData	ends
