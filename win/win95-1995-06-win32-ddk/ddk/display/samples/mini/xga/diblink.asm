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
; DIBLINK.ASM 
;----------------------------------------------------------------------------
;----------------------------------------------------------------------------
        .xlist
DOS5 = 1
        include cmacros.inc
        .list
;----------------------------------------------------------------------------
; MACROS
;----------------------------------------------------------------------------
DIBLINK macro name,target,extra
externFP target
public name
name:
	assumes	ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes	gs,nothing
ifnb <extra>
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	pop	ecx
	push	extra
	push	ecx
endif
	jmp	target
	endm

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD	lpDriverPDevice
        externW wPalettized
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

DIBLINK ColorInfo,		DIB_ColorInfo
DIBLINK GetCharWidth,		DIB_GetCharWidth
DIBLINK DibBlt,			DIB_DibBltExt,		        wPalettized
DIBLINK DibToDevice,		DIB_DibToDevice
DIBLINK CreateDIBitmap,		DIB_CreateDIBitmap
DIBLINK EnumDFonts,		DIB_EnumDFonts
DIBLINK EnumObj,		DIB_EnumObjExt,		        lpDriverPDevice
DIBLINK SetPaletteTranslate,	DIB_SetPaletteTranslateExt,     lpDriverPDevice
DIBLINK GetPaletteTranslate,	DIB_GetPaletteTranslateExt,     lpDriverPDevice
DIBLINK GetPalette,		DIB_GetPaletteExt,	        lpDriverPDevice
DIBLINK UpdateColors,		DIB_UpdateColorsExt,	        lpDriverPDevice
DIBLINK Pixel,			DIB_Pixel
DIBLINK RealizeObject,		DIB_RealizeObjectExt,	        lpDriverPDevice
DIBLINK SelectBitmap,		DIB_SelectBitmap
DIBLINK BitmapBits,		DIB_BitmapBits
DIBLINK ScanLR,			DIB_ScanLR
DIBLINK StretchBlt,		DIB_StretchBlt
DIBLINK StretchDIBits,		DIB_StretchDIBits
DIBLINK DeviceMode,		DIB_DeviceMode
DIBLINK DeviceBitmap,		DIB_DeviceBitmap
DIBLINK SetAttribute,		DIB_SetAttribute
DIBLINK Output, 		DIB_Output
DIBLINK FastBorder, 		DIB_FastBorder


sEnd	Code
end
