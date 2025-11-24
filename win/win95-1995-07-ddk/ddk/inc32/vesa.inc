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

VbeInfoBlock		struc
	VbeSignature		db	4 dup(?)
	VbeVersion		dw	?
	OemStringPtr		dd	?
	VESACaps		db	4 dup(?)
	VideoModePtr		dd	?
	VESATotalMemory 	dw	?
	OemSoftwareRev		dw	?
	OemVendorNamePtr	dd	?
	OemProductNamePtr	dd	?
	OemProductRevPtr	dd	?
	VESAReserved		db	222 dup(?)
	VESAOemData		db	256 dup(?)
VbeInfoBlock		ends
VbeV1Scratch			equ	OemSoftwareRev	;VESA 1 scratch area
.errnz	SIZE VbeInfoBlock - 512
;
;
VESAModeInfoBlock	struc
	VModeAttributes 	dw	?
	VWinAAttributes 	db	?
	VWinBAttributes 	db	?
	VWinGranularity 	dw	?
	VWinSize		dw	?
	VWinASegment		dw	?
	VWinBSegment		dw	?
	VWinFuncPtr		dd	?
	VBytesPerScanLine	dw	?
	VXResolution		dw	?
	VYResolution		dw	?
	VXCharSize		db	?
	VYCharSize		db	?
	VNbrOfPlanes		db	?
	VBitsPerPixel		db	?
	VNbrOfBanks		db	?
	VMemoryModel		db	?
	VBankSize		db	?
	VNbrOfImagePages	db	?
	VReserved		db	?
	VRedMaskSize		db	?
	VRedFieldPosition	db	?
	VGreenMaskSize		db	?
	VGreenFieldPosition	db	?
	VBlueMaskSize		db	?
	VBlueFieldPosition	db	?
	VReservedMaskSize	db	?
	VReservedFieldPosition	db	?
	VDirectColorModeInfo	db	?
	VPhysBasePtr		dd	?
	VOffScreenMemOffset	dd	?
	VOffScreenMemSize	dw	?
	VFiller 		db	206 dup(?)
VESAModeInfoBlock	ends
.errnz	SIZE VESAModeInfoBlock - 256
