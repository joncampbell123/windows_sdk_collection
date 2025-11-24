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
; STRBLT.ASM
;----------------------------------------------------------------------------
	.286c
	.xlist
	DOS5 = 1			;so we don't get INC BP in <cBegin>
	include cmacros.inc
	include	dibeng.inc
	include	macros.inc
	include xga.inc
	incFont = 1
	include	gdidefs.inc
	.list
;----------------------------------------------------------------------------
; E X T E R N S
;----------------------------------------------------------------------------
	externFP	DIB_ExtTextOut
	externFP	DIB_ExtTextOutExt

;----------------------------------------------------------------------------
; D A T A
;----------------------------------------------------------------------------
sBegin	Data
	externD OffScreenStartAddr		;in VGA.ASM
	externD XGARegs 			;in VGA.ASM
	externW wBpp				;in VGA.ASM
	externW wScreenWidth			;in VGA.ASM
	externW wChipId 			;in VGA.ASM
	externW MemAccessReg			;in VGA.ASM
	externB MemAccessColorValue		;in VGA.ASM
	externB WindowsEnabledFlag		;in VGA.ASM
	externB AGXChipID			;in VGA.ASM
sEnd	Data

;----------------------------------------------------------------------------
; C O D E
;----------------------------------------------------------------------------
sBegin	Code
	assumes cs,Code

cProc	Strblt,<FAR,PUBLIC,PASCAL,WIN>
	parmD	lpDevice		;Destination device
	parmW	x			;Left origin of string
	parmW	y			;Top  origin of string
	parmD	lpClipRect		;Clipping rectangle
	parmD	lpString		;The string itself
	parmW	count			;Number of characters in the string
	parmD	lpFont  		;Font to use
	parmD	lpDrawMode		;Drawmode structure to use
	parmD	lpXform 		;Current text transform
cBegin	<nogen>
	pop	cx			;Save caller's return address
	pop	bx
	xor	ax,ax
	push	ax			;Push null for lp_dx
	push	ax
	push	ax			;Push null for lp_opaque_rect
	push	ax
	push	ax			;Push null for options
	push	bx			;Restore return address
	push	cx
cEnd	<nogen>
	errn$	ExtTextOut

cProc	ExtTextOut,<FAR,PUBLIC,PASCAL,NODATA>
	parmD	lpDevice		;Destination device
	parmW	x			;Left origin of string
	parmW	y			;Top  origin of string
	parmD	lpClipRect		;Clipping rectangle
	parmD	lpString		;The string itself
	parmW	count			;Number of characters in the string
	parmD	lpFont  		;Font to use
	parmD	lpDrawMode		;Drawmode structure to use
	parmD	lpXform 		;Current text transform
	parmD	lpDx			;Widths for the characters
	parmD	lpOpaqueRect		;Opaquing rectangle
	parmW	etoOptions		;ExtTextOut options
cBegin <nogen>
.386
	mov	ax,DGROUP
	mov	es,ax
	assumes	ds,nothing
	assumes	es,Data
	assumes	fs,nothing
	assumes	gs,nothing

	mov	bx,sp
	cmp	word ptr ss:[bx][26],0	;is this an extent call ?
	jl	ETO_CallExtTextOut	;let the DIB Engine handle it.

	test	word ptr ss:[bx][4], ETO_LEVEL_MODE	; Special TO?
	jnz	ETO_CallExtTextOut	; Yes, give to de

	lfs	bx,ss:[bx][40]		;ds:bx-->lpDevice
	test	fs:[bx].deFlags,VRAM	;is this the screen?
	jz	ETO_CallExtTextOut	;no, let the DIB Engine handle it.
        test    fs:[bx].deFlags,BUSY    ;is the screen BUSY?
        jnz     ETO_CallExtTextOut      ;yes, let the DIB Engine handle it.
	cmp	WindowsEnabledFlag,0	;is Windows currently screen owner?
	jne	ETO_CallExtTextOut	;nope. Let the DIB Engine fail.
	cmp	wBpp,8			;bpp = 8?
	je	ETO_SetupForDTB 	;yes, we can handle it in the BLTer
	cmp	wChipId,AGX_ID		;running on the IIT AGX?
	jne	ETO_CallExtTextOut	;nope, let the DIB Engine handle it.
	cmp	wBpp,16 		;running on the AGX at 16 BPP?
	jne	ETO_CallExtTextOut	;nope, let the DIB Engine handle it.
	cmp	AGXChipID,15h		;running on the AGX 15 or above?
	jb	ETO_CallExtTextOut	;nope, on the AGX 14, no hardware BLT
					;at 16 BPP
	cmp	wScreenWidth,800	;running 800x600x16 on the AGX?
	je	ETO_CallExtTextOut	;nope, BLTer not supported at 800x600x16
;
PLABEL ETO_SetupForDTB
	pop	ebx			;Get caller's return address
	xor	eax,eax
	mov	ax,cs
	shl	eax,16
	mov	ecx,eax
	mov	ax,CodeOFFSET DrawTextBitmap
	mov	cx,CodeOFFSET DrawORect
	push	eax
	push	ecx
	push	ebx
	jmp	DIB_ExtTextOutExt
cEnd   <nogen>

PLABEL ETO_CallExtTextOut
	jmp	DIB_ExtTextOut

.286c
cEnd	<nogen>
;
;
subttl		XGA Device Dependent StrBLT Expansion Routine
page +
;---------------------------------------------------------------------------
; DrawTextBitmap - draw the mono bitmap rectangle to given coordinates
; and color. This mono Bitmap is the realized text string. Clips to given
; clip rectangle.
;---------------------------------------------------------------------------
cProc	DrawTextBitmap,<FAR,PUBLIC,PASCAL,NODATA>
;
	parmD	lpDevice		;Destination device
	parmD	lpMonoBuffer
	parmW	Flags
	parmD	BgColor
	parmD	FgColor
	parmW	DstxOrg 		;Left origin of string bitmap
	parmW	DstyOrg 		;Top  origin of string bitmap
	parmW	WidthBytes		;width of string bitmap
	parmW	Height			;height of string bitmap
	parmD	lpClipRect		;Clipping rectangle
;
cBegin
.386
	mov	ax,DGROUP
	mov	gs,ax
	assumes ds,nothing
	assumes	es,nothing
	assumes	fs,nothing
	assumes gs,Data
	push	esi			;save 32 bit versions of these
	push	edi			;
;
;First, let's copy the mono buffer to the off-screen memory area.  There's
;always an even number of doublewords in the buffer.
;
	cld				;set direction forward
;
;If we're using a software cursor, we need to set the BusyBit so that the
;cursor doesn't try to asynchronously draw while we're fooling with the
;BLTer:
;
	lfs	di,lpDevice		;FS:DI --> DIB Engine PDevice structure
	or	fs:[di].deFlags,BUSY	;
;
;Set MemAccessReg (21x9) to the appropriate value for transfering a
;monochrome, Intel format bitmap:
;
	mov	dx,MemAccessReg 	;
	mov	al,08h			;set monochrome, Motorola ordering
	out	dx,al			;
	lfs	di,XGARegs		;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	push	di			;save FS:DI --> XGA registers
	les	edi,fword ptr OffScreenStartAddr
	xor	esi,esi 		;make sure top word of ESI is 0
	lds	si,lpMonoBuffer 	;DS:ESI --> buffer containing string
	movzx	eax,WidthBytes		;get total size of bitmap to copy
	cmp	wChipId,AGX_ID		;running on an IIT AGX?
	jne	DTBCopyXGAMonoImage	;nope, go do it the XGA way!
;
public	DTBCopyAGXMonoImage
DTBCopyAGXMonoImage:
;
;The AGX controller only accepts power of 2 bitmap width, so we need to
;round the bitmap width to the nearest power of 2.
;
	dec	eax
	xor	ecx, ecx
	bsr	ecx, eax
	inc	cx
	inc	eax
	mov	ebx, 1
	shl	bx, cl
	push	bx			;save BX = power of 2 width of bitmap
	sub	ebx, eax
	movzx	edx, Height
;
DTBLoadMonoLoop:
	movzx	ecx, WidthBytes
	shr	ecx, 2
	rep	movs	dword ptr es:[edi], dword ptr [esi]
	add	edi, ebx
	dec	dx
	jnz	DTBLoadMonoLoop
	pop	WidthBytes		;make "power of 2 width" our WidthBytes
	jmp	DTBMonoImageCommon
;
public	DTBCopyXGAMonoImage
DTBCopyXGAMonoImage:
	movzx	edx,Height		;
	mul	edx			;EAX has total nbr of bytes to copy
	mov	ecx,eax 		;ECX has total nbr of bytes to copy
	shr	ecx,2			;ECX has total nbr of dwords to copy
	rep	movs dword ptr es:[edi],dword ptr [esi]
;
DTBMonoImageCommon:
	pop	di			;restore saved FS:DI --> XGA registers
;
;Reset the MemAccessReg (21x9) back to colour format:
;
	mov	dx,MemAccessReg 	;
	mov	al,MemAccessColorValue	;
	out	dx,al			;
;
public	DTBCoordinateCalcs
DTBCoordinateCalcs:
;
;First, define the dimensions of the source (1 BPP pattern) pixmap:
;
	mov	fs:[di].PixmapIndex,3	;make sure we're pointing at index 3
	mov	ax,WidthBytes		;just WidthBytes * 8
	shl	ax,3			;
	dec	ax			;
	mov	fs:[di].PixmapWidth,ax	;set PixMap's X-dimension to XGA
	mov	ax,Height		;
	dec	ax			;
	mov	fs:[di].PixmapHeight,ax ;set PixMap's Y-dimension to XGA
;
;The starting destination X-coordinate is simply the left of the clip rect:
;
	lds	si,lpClipRect		;
	mov	ax,[si] 		;get left side of clip rect
	mov	fs:[di].DestMapX,ax	;set DstxOrg to XGA
;
;The starting source X-coordinate is left-clip - DstxOrg:
;
	sub	ax,DstxOrg		;
	mov	fs:[di].PatternMapX,ax	;set SrcxOrg to XGA
;
;The OperationDimension1 is right-clip - left-clip:
;
	mov	ax,[si+4]		;get right side of clip rect
	sub	ax,[si] 		;now AX has xExt of string
	dec	ax			;for the XGA
	mov	fs:[di].OperationDimension1,ax
;
;The starting source Y-coordinate is 0:
;
	mov	fs:[di].PatternMapY,0	;set SrcyOrg to 0 on XGA
;
;The starting destination Y-coordinate is the top-clip:
;
	mov	ax,[si+2]		;get top of clip rectangle
	mov	fs:[di].DestMapY,ax	;set DstyOrg to XGA
;
;The OperationDimension2 is Height:
;
	mov	cx,Height		;
	dec	cx			;for the XGA
	mov	fs:[di].OperationDimension2,cx
;
public	DTBSetColours
DTBSetColours:
	mov	eax,BgColor		;
	mov	fs:[di].BackgroundColorReg,eax
	mov	eax,FgColor		;
	mov	fs:[di].ForegroundColorReg,eax
	mov	al,3			;assume REPLACE is mix mode
	mov	fs:[di].FgdMix,al	;foreground mix is always REPLACE
	test	Flags,01h		;BLTing transparently?
	jz	@F			;nope, set background mix to REPLACE
	mov	al,5			;yes, set background to LEAVE ALONE
@@:	mov	fs:[di].BgdMix,al	;set the desired background mix
;
public	DTBSendCommand
DTBSendCommand:
;
;Now, send the command, using the PixMap 3 as the string pattern and
;PixMap 1 (the visible screen) as our destination:
;
	mov	fs:[di].PixelOperation,08113000h
;
;If we're using a software cursor, we need to restore the deFlags that we
;saved at the beginning of this routine.  This will un-set the BUSY bit that
;we set to prevent the cursor from drawing:
;
	lfs	di,lpDevice		;FS:DI --> DIB Engine PDevice structure
	and	fs:[di].deFlags,NOT BUSY;and restore them to the PDevice
;
DTBExit:
	pop	edi			;restore saved registers
	pop	esi			;
	mov	ax,1
.286c
cEnd
;
;
subttl		XGA Device Dependent Opaque Rectangle Drawing Routine
page +
cProc	DrawORect,<FAR,PUBLIC,PASCAL,NODATA>,<di>
;
	parmD	lpDevice		;Destination device
	parmD	BgColor
	parmW	DstxOrg 		;Left origin of orect
	parmW	DstyOrg 		;Top  origin of orect
	parmW	xExt			;width of orect
	parmW	yExt			;height of orect
;
cBegin
.386
	mov	ax,DGROUP
	mov	ds,ax
	assumes ds,Data
	assumes	es,nothing
	assumes	fs,nothing
	assumes gs,nothing
;
;If we're using a software cursor, we need to set the BusyBit so that the
;cursor doesn't try to asynchronously draw while we're fooling with the
;BLTer:
;
	lfs	di,lpDevice		;FS:DI --> DIB Engine PDevice structure
	or	fs:[di].deFlags,BUSY	;
	lfs	di,XGARegs		;FS:DI --> XGA Coprocessor registers
	MakeHardwareNotBusy	fs,di
	mov	fs:[di].FgdMix,3	;set MixMode to REPLACE
	mov	fs:[di].BgdMix,3	;
;
;Set the colour onto the XGA:
;
	mov	eax,BgColor		;
	mov	fs:[di].ForegroundColorReg,eax
;
;Set the coordinates:
;
	mov	ax,DstxOrg		;get left coordinate
	mov	fs:[di].DestMapX,ax	;
	mov	ax,DstyOrg		;get top coordinate
	mov	fs:[di].DestMapY,ax	;
;
;And the extents:
;
	mov	ax,xExt 		;get the width
        dec     ax                      ;decrement it (as per manual)
	mov	fs:[di].OperationDimension1,ax
					;set the X-extent
	mov	ax,yExt 		;get the height
        dec     ax                      ;
	mov	fs:[di].OperationDimension2,ax
					;set the Y-extent
;
;Send the solid coloured rectangle command out:
;
	mov	fs:[di].PixelOperation,08118000h
;
;If we're using a software cursor, we need to restore the deFlags that we
;saved at the beginning of this routine.  This will un-set the BUSY bit that
;we set to prevent the cursor from drawing:
;
	lfs	di,lpDevice		;FS:DI --> DIB Engine PDevice structure
	and	fs:[di].deFlags,NOT BUSY;and restore them to the PDevice
;
DORExit:
	mov	ax,1
.286c
cEnd
;
;
sEnd	Code
	end
