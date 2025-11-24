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
; XGA.INC
;----------------------------------------------------------------------------

;
;Following are the POS device ID equates for XGA/1 (original) and XGA/2
;(the later one which did non-interlaced modes):
;
XGA1_ID 	equ	8fdbh
XGA2_ID 	equ	8fdah
AGX_ID		equ	8fd9h

;
;Following is the structure of the XGA co-processor memory mapped registers:
;
XGABLTerRegs		struc
	PageDirBaseAddr 	dd	?
	CurrentVirtualAddr	dd	?
	Reserved_1		dd	?
	StateALen		db	?
	StateBLen		db	?
	Reserved_2		dw	?
	Reserved_3		db	?
	CoprocessorControl	db	?
	PixmapIndex		db	?
	Reserved_4		db	?
	PixmapBasePointer	dd	?
	PixmapWidth		dw	?
	PixmapHeight		dw	?
	PixmapFormat		db	?
	Reserved_5		db	3 dup(?)
	BresenhamErrorTerm	dw	?
	Reserved_6		dw	?
	BresenhamK1		dw	?
	Reserved_7		dw	?
	BresenhamK2		dw	?
	Reserved_8		dw	?
	DirectionsSteps 	dd	?
	Reserved_9		db	24 dup(?)
	FgdMix			db	?
	BgdMix			db	?
	DestColorCompCond	db	?
	Reserved_10		db	?
	DestColorCompareValue	dd	?
	PixelBitMask		dd	?
	CarryChainMask		dd	?
	ForegroundColorReg	dd	?
	BackgroundColorReg	dd	?
	OperationDimension1	dw	?
	OperationDimension2	dw	?
	Reserved_11		dd	2 dup(?)
	MaskMapOriginXOffset	dw	?
	MaskMapOriginYOffset	dw	?
	SourceMapX		dw	?
	SourceMapY		dw	?
	PatternMapX		dw	?
	PatternMapY		dw	?
	DestMapX		dw	?
	DestMapY		dw	?
	PixelOperation		dd	?
XGABLTerRegs		ends

;
;The following general purpose macro makes sure that the XGA hardware isn't
;busy:
;
MakeHardwareNotBusy	macro	SegReg, OffReg
local	HardwareStillBusy
HardwareStillBusy:
	test	SegReg:[OffReg].CoprocessorControl,80h	;;busy bit still set?
	jnz	HardwareStillBusy			;;yes, go try again
endm
