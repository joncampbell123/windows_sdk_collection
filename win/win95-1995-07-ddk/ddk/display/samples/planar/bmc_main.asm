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
;----------------------------------------------------------------------------;
;		       DeviceIndependentBitmaps			             ;
;		       ------------------------                 	     ;
; This routine does the conversions from a device independent bitmap format  ;
; to a format adopted for EGA adapters and back.			     ;
;         Depending on one of the input parameters it call either,	     ;
;          . SetDeviceBitmapBits -  to set the bits in the internal format   ;
;          . GetDeviceBitmapBits -  to get the bits from the internal format ;
;									     ;
; General Description:							     ;
;									     ;
;	There are four standard bitmap formats.  All device drivers	     ;
;	are required to be able to translate between any of these	     ;
;	formats and their own internal formats. The standard formats	     ;
;	are as follows:							     ;
;									     ;
;			      Bitcount	      Planes			     ;
;			      --------	      ------			     ;
;				  1		 1			     ;
;				  4		 1			     ;
;				  8		 1			     ;
;				 24		 1			     ;
;									     ;
;	These formats are chosen because they are identical or similar	     ;
;	to all formats commonly used by raster devices.  Only single	     ;
;	plane formats are standard, but it is very easy to convert these     ;
;	to any multiple plane format used internally by a device.	     ;
;									     ;
;	For both the device independent format and the device specific format;
;       the origin of the bitmap is at the top left hand corner of the bitmap;
;       X axis extends to the right and Y towards the bottom, both moving to-;
;       -wards higher memory addresses.					     ;
;									     ;
;----------------------------------------------------------------------------;


	.xlist
	include	cmacros.inc
	include gdidefs.inc
	include	macros.inc
	.list

SD_COLOR	equ	01000000b	; surface is color
SET_CODE	equ	0		; setbitmapbits code
GET_CODE	equ	1		; getbitmapbitscode

;----------------------------------------------------------------------------;

createSeg _DIMAPS,DIMapSeg,word,public,CODE
sBegin	DIMapSeg
	assumes cs,DIMapSeg

	externNP	SetDeviceBitmapBits	; in ./bmc_eti.asm
	externNP	GetDeviceBitmapBits	; in ./bmc_ite.asm
	externFP	RLEBitBlt		; in ./rlebm.asm

	assumes ds,nothing
	assumes es,nothing

cProc	DeviceBitmapBits,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es>

	parmD   lp_dst_dev              ; -> pointer to own bitmap structure
	parmW	set_or_get		; 0 => set, 1 => get
	parmW	iStart			; start copy at this scan
	parmW   num_scans		; number of scans to copy
        parmD   lp_bits_ind             ; -> pointer to independent format
	parmD	lpbi			; -> Pointer to info block
        parmD   lpDrawMode              ; -> pointer to draw mode
        parmD   dwColorInfo             ; Extra stuff passed by GDI
	
	localB	fbsd			; own format mono or color
	localV	RLEClipRect,8		; clipping rectangle for RLE
cBegin
	cld
	lds	si,lp_dst_dev		; DS:SI points to internal BM struc
	assumes	ds,nothing

        les     di,lpbi                 ; ES:DI points to BITMAPINFOHEADER
	assumes	es,nothing

;----------------------------------------------------------------------------;
; if the DIB is RLE encoded or wants the RLE encoding back, call off to      ;
; RLEBitBlt.								     ;
;----------------------------------------------------------------------------;

	cmp	bptr es:[di].biCompression, 0
	jz	test_color		; not an RLE DIB

; create the clipping rectangle from the width and height of the dest surface
	

; for a SetDIB call the extent of the clipping rectangle are the width and
; height of the DIb where as for the GetDIB call the extent are the width
; and height of the surface

	cmp	set_or_get,GET_CODE	; is it a GetDIB call ?
	jnz	rle_extents_from_surface; get the extents from the surface

; get the extent from the DIB parameters

	mov	bx,wptr es:[di].biWidth ; the x extent
	mov	cx,wptr es:[di].biHeight; the y extent
	jmp	short rle_extents_in_bx_cx

rle_extents_from_surface:

	les	di,lp_dst_dev		; load the surface decriptor pointer
	mov	bx,es:[di].bmWidth	; the x extent
	mov	cx,es:[di].bmHeight	; the y extent

rle_extents_in_bx_cx:

	lea	si,RLEClipRect		
	smov	ds,ss			; ds:si point to area to hold rect
	mov	[si].left,0
	mov	[si].top,0		; top left is at DIB origin
	mov	[si].right,bx		; right of clip rect
	mov	[si].bottom,cx		; bottom of the clip rect
	farPtr	lp_clip_rect,ds,si	; long pointer to rect just created
	xor	ax,ax			; will need for param passing
	farPtr	lp_NULL,ax,ax

	arg	lp_dst_dev		; long pointer to pdevice
	arg	ax			; DstX = 0
	arg	ax			; DstY = 0
	arg	bx			; right of the rectangle is xExt
	arg	cx			; bottom of rect is also yExt
	arg	iStart			; tart scan wrt the complete RLE
	arg	num_scans		; no of scan in the RLE band
	arg	set_or_get		; get Rle or set RLE indicator
	arg	lp_clip_rect		; pass in the clipping rectangle
	arg	lp_NULL			; pass in a NULL for the drawmode
	arg	lpbi			; the DIB info block
        arg     lp_bits_ind             ; the DIB buffer area

	cCall	RLEBitBlt		; do the call
	jmp	short xfer_done		; ax has return code

;----------------------------------------------------------------------------;

format_error:
	jmp	error_detected		; relay jump

test_color:
	
; formats seem to match. So now we will call one of the two routines which 
; actually does the transfer. Also determine whether own map is mono or
; color.

	mov	al,SD_COLOR		; assume color map
	cmp	[si].bmPlanes,1		; is it one plane ?
	jnz	al_has_flag		; no, so it is color
	cmp	[si].bmBitsPixel,1	; 1 bit per pel
	jnz	al_has_flag		; no so it is color
	xor	al,al			; own flag is mono
al_has_flag:
	mov	fbsd,al			; set own color indicator

; the parameters for the call have been set, decide which routine to call.
; get a long pointer to the start of the destination bitmap

        les     di,lp_bits_ind          ; long pointer to user buffer
	assumes	es,nothing

	xor	ax,ax
        farPtr  <lp_bits>,es,di
	farPtr	<lp_own>,<word ptr [si].bmBits+2>,<word ptr [si].bmBits>
	farPtr	<iS>,ax,iStart		; convert to a DWORD
	farPtr  <iNum>,ax,num_scans	; convert to a DWORD

	cmp	set_or_get,SET_CODE	; set code is zero
        jnz     get_call                ; the other call

        mov     ax,es                   ; NULL pointer is invalid for a SET
        or      ax,di
        jz      format_error

	arg	lp_own			; pointer to own bitmap
	arg	iNum			; number of scans to copy
	arg	iS			; start scan num
        arg     lp_bits
	arg	lpbi
	arg 	fbsd

	cCall	SetDeviceBitmapBits
	jmp	short xfer_done		; return with status code in ax

get_call:
	cmp	set_or_get,GET_CODE	; make sure its a get call
	jnz	format_error		; error condition
	arg	lp_own			; pointer to own bitmap
	arg	iNum			; number of scans to copy
	arg	iS			; start scan num
        arg     lp_bits
	arg	lpbi
	arg	fbsd

	cCall	GetDeviceBitmapBits
	jmp	short xfer_done

error_detected:
	xor	ax,ax			; the error code
xfer_done:

cEnd

;----------------------------------------------------------------------------;
; A dummy procedure to handle the Create call for independent bitmaps	     ;
;----------------------------------------------------------------------------;

cProc	CreateBitmap,<PUBLIC,FAR>

; do not even bother about parameters

cBegin
	xor	ax,ax			; success code
cEnd


sEnd	DIMapSeg
END

