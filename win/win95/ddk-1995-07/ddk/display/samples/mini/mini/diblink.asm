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
        include dibeng.inc
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
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code
	.386
;;DIBLINK BitBlt,		DIB_BitBlt
DIBLINK ColorInfo,		DIB_ColorInfo
DIBLINK Control,		DIB_Control
;;DIBLINK  Disable,		DIB_Disable
;;DIBLINK  Enable,		DIB_Enable
DIBLINK EnumDFonts,		DIB_EnumDFonts
DIBLINK EnumObj,		DIB_EnumObjExt,		        lpDriverPDevice
DIBLINK Output,			DIB_Output
DIBLINK Pixel,			DIB_Pixel
DIBLINK RealizeObject,		DIB_RealizeObjectExt,	        lpDriverPDevice
DIBLINK Strblt,			DIB_Strblt
DIBLINK ScanLR,			DIB_ScanLR
DIBLINK DeviceMode,		DIB_DeviceMode
DIBLINK ExtTextOut,		DIB_ExtTextOut
DIBLINK GetCharWidth,		DIB_GetCharWidth
DIBLINK DeviceBitmap,		DIB_DeviceBitmap
DIBLINK FastBorder,		DIB_FastBorder
DIBLINK SetAttribute,		DIB_SetAttribute
DIBLINK DibBlt,			DIB_DibBltExt,		        wPalettized
DIBLINK CreateDIBitmap,		DIB_CreateDIBitmap
DIBLINK DibToDevice,		DIB_DibToDevice
;;DIBLINK SetPalette		DIB_SetPaletteExt,	        lpDriverPDevice
DIBLINK GetPalette,		DIB_GetPaletteExt,	        lpDriverPDevice
DIBLINK SetPaletteTranslate,	DIB_SetPaletteTranslateExt,     lpDriverPDevice
DIBLINK GetPaletteTranslate,	DIB_GetPaletteTranslateExt,     lpDriverPDevice
DIBLINK UpdateColors,		DIB_UpdateColorsExt,	        lpDriverPDevice
DIBLINK StretchBlt,		DIB_StretchBlt
DIBLINK StretchDIBits,		DIB_StretchDIBits
DIBLINK SelectBitmap,		DIB_SelectBitmap
DIBLINK BitmapBits,		DIB_BitmapBits
;;DIBLINK  ReEnable,		DIB_ReEnable
DIBLINK Inquire,		DIB_Inquire
DIBLINK SetCursor,		DIB_SetCursorExt,	        lpDriverPDevice
DIBLINK MoveCursor,		DIB_MoveCursorExt,	        lpDriverPDevice
;;DIBLINK CheckCursor,		DIB_CheckCursorExt,	        lpDriverPDevice
;;DIBLINK BeginAccess,		DIB_BeginAccess
;;DIBLINK EndAccess,		DIB_EndAccess

;----------------------------------------------------------------------------
; Weird linkages
;----------------------------------------------------------------------------
externFP DIB_CheckCursorExt
public CheckCursor
CheckCursor:
	mov	ax,DGROUP
	mov	es,ax
        assumes es,Data
        cmp     wEnabled,0
        je      short @f
	pop	ecx
	push	lpDriverPDevice
	push	ecx
        jmp     DIB_CheckCursorExt
@@:     retf

;------------------------------------------------------------------------
;------------------------------------------------------------------------
externFP DIB_BitBlt
public BitBlt
public BitBltHW

BB_JumpToDibEngineX:
        mov     bx,sp
        mov     eax,ss:[bx][32]         ;edx = lpDestDev
        cmp     eax,ss:[bx][24]         ;does lpSrcDev==lpDestDev?
        je      BitBltHW                ;yes, let us handle screen<==>screen
BB_JumpToDibEngine:
        jmp     DIB_BitBlt              ;Let DIB Engine handle it
BitBlt:
        mov     bx,sp                   ;ss:[bx]-->stack
	lfs	bx,ss:[bx][32]		;fs:bx-->lpDestDev
	mov	ax,fs:[bx].deFlags	;need many times
	test	ax,VRAM			;is destination screen ?
        jz      BB_JumpToDibEngine      ;no. Don't draw. Let DIB Engine handle it
        test    ax,BUSY                 ;Is the screen busy
        jnz     BB_JumpToDibEngine      ;yes, let DIB Engine handle it
	test	ax,PALETTE_XLAT		; do we need background palette translation ?
        jnz     BB_JumpToDibEngineX     ;yes, let DIB Engine handle it (mabey)
        errn$   BitBltHW                ;no, let us give it a try
BitBltHW:
	mov	ax,DGROUP
	mov	es,ax
	assumes	es,Data
	cmp	BitBltDevProc,0
        je      BB_JumpToDibEngine
	jmp	[BitBltDevProc]

sEnd	Code
end
