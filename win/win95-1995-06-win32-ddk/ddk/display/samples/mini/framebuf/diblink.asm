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
        externW wEnabled
	externW BitBltDevProc
	externW TextOutDevProc
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386

DIBLINK ColorInfo,		DIB_ColorInfo
DIBLINK GetCharWidth,		DIB_GetCharWidth
DIBLINK SetCursor,		DIB_SetCursorExt,	        lpDriverPDevice
DIBLINK MoveCursor,		DIB_MoveCursorExt,	        lpDriverPDevice
DIBLINK Inquire,		DIB_Inquire
DIBLINK DibBlt,			DIB_DibBltExt,		        wPalettized
DIBLINK DibToDevice,		DIB_DibToDevice
DIBLINK CreateDIBitmap,		DIB_CreateDIBitmap
DIBLINK EnumDFonts,		DIB_EnumDFonts
DIBLINK EnumObj,		DIB_EnumObjExt,		        lpDriverPDevice
DIBLINK Output,			DIB_Output
DIBLINK SetPaletteTranslate,	DIB_SetPaletteTranslateExt,     lpDriverPDevice
DIBLINK GetPaletteTranslate,	DIB_GetPaletteTranslateExt,     lpDriverPDevice
DIBLINK GetPalette,		DIB_GetPaletteExt,	        lpDriverPDevice
DIBLINK UpdateColors,		DIB_UpdateColorsExt,	        lpDriverPDevice
DIBLINK Pixel,			DIB_Pixel
DIBLINK RealizeObject,		DIB_RealizeObjectExt,	        lpDriverPDevice
DIBLINK SelectBitmap,		DIB_SelectBitmap
DIBLINK BitmapBits,		DIB_BitmapBits
DIBLINK ScanLR,			DIB_ScanLR
DIBLINK DeviceMode,		DIB_DeviceMode
DIBLINK StretchBlt,		DIB_StretchBlt
DIBLINK StretchDIBits,		DIB_StretchDIBits
DIBLINK DeviceBitmap,		DIB_DeviceBitmap
DIBLINK SetAttribute,		DIB_SetAttribute
;;DIBLINK Strblt,		DIB_Strblt
;;DIBLINK ExtTextOut,		DIB_ExtTextOut
DIBLINK FastBorder,		DIB_FastBorder

;----------------------------------------------------------------------------
; Weird linkages
;----------------------------------------------------------------------------
externFP DIB_CheckCursorExt
public CheckCursor
CheckCursor:
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	cmp	wEnabled,0
	je	short @f
	pop	ecx
	push	lpDriverPDevice
	push	ecx
	jmp	DIB_CheckCursorExt
@@:	retf

externFP DIB_BitBlt
public BitBlt
BitBlt:
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	cmp	BitBltDevProc,0
	je	short @f
	jmp	[BitBltDevProc]
@@:	jmp	DIB_BitBlt


public Strblt
Strblt:
	pop	ecx			;Get caller's return address
	xor	eax,eax		
	push	eax			;push null for lp_dx
	push	eax			;push null for lp_opaque_rext
	push	ax			;push null for options
	push	ecx

externFP DIB_ExtTextOut
public ExtTextOut
ExtTextOut:
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	cmp	TextOutDevProc,0
	je	short @f
	jmp	[TextOutDevProc]
@@:	jmp	DIB_ExtTextOut


sEnd	Code
end
