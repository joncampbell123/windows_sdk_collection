;******************************Module*Header***********************************
;Module name:	DIBTOSCR.ASM
;Created by:	GunterZ (Gunter Zieber)
;History:	12-14-88    Created
;		12-17-88    1, 4, 8 Bit/Pixel DIB support fully functional
;
; Copyright (c) 1985-1988 Microsoft Corporation
;
;Procedures in this module implement transfer of a device independent bitmap
;direcly to the screen.  DIBToScreen is the exported entry point to the driver.
;******************************************************************************

.xlist
include     cmacros.inc
include     gditype.inc
include     gdibitm.inc
include     8514port.inc
.list
.286

COLORTABLESIZE	    equ     256
WORKSPACESIZE	    equ     256
READCOMMAND	    equ     FILL_X_RECT or PC_TRANS or YMAJAXIS or INCX or DRAWCMD; or BIT16
WRITECOMMAND	    equ     READCOMMAND or BYTE_SWAP or WRITCMD
FORGROUNDMIX	    equ     PTRANS_ACTIVE or SRC
BACKGROUNDMIX	    equ     PTRANS_ACTIVE OR SRC
DEFAULTMIX	    equ     F_CLR_ACTIVE or SRC
NEWSTACK	    equ     1000h
ODD		    equ     1

ParameterBlock	struc
    oCount	dw	?
    wStartSkip	dw	?
    bStartRem	db	?
    bRem	db	?
ParameterBlock	ends

externFP    CursorExclude		; in ROUTINES.ASM
externFP    CursorUnExclude		; in ROUTINES.ASM
externFP    GetIndexOfRGBFar
externFP    GetModuleHandle		;in KERNEL
externFP    GetProcAddress		;also in KERNEL

sBegin	Data
externW Palette
externB WriteEnable
externb szGDI
sEnd

createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	    DIBSeg

	assumes cs, DIBSeg
	assumes ds, Data		; the driver's data segment is never
	assumes es, nothing		; needed in any of the routines here.

extrn	    __AHIncr:ABS		; imported from kernel for prot. mode
externNP    BuildColorTable_4

ColorTableFumble    label   word
	dw	BuildColorTable4	;can't call directly; need to pass pal.
	dw	BuildColorTable8

TwentyFourBitDIB    label   word
	dw	Transfer24BitDIB4
	dw	Transfer24BitDIB8

;******************************Public*Routine**********************************
;All arguments passed to this routine must be valid and consistent (in
;particular SrcY and nStart) since it does not perform any consistency checks
;of the arguments.  The ColorTable field in the InfoBlock is assumed to be a
;logical index to physical color translate table in which each entry is one
;WORD.
;restores sp, bp, si, di, all other registers change.
;******************************************************************************

cProc	DIBToScreen, <NEAR, PUBLIC, WIN, PASCAL>
	parmD	lpPDevice		; physical device
	parmW	DestX			; Destination X (on screen)
	parmW	DestY			; Destination Y (on screen)
	parmW	DExtX			; Extent in X direction
	parmW	DExtY			; Extent in Y direction
	parmW	SrcX			; Source X (in DIBitmap)
	parmW	SrcY			; Source Y (in DIBitmap)
	parmW	nStart			; the current line lpBits points to
	parmD	lpBits			; long pointer to DIBitmap Bits
	parmD	lpInfoBlock		; long pointer to DIBitmap info Block
	parmD	lpConversionInfo	;stuff to convert 24 b/p DIBs->indices

	localB	bMask
	localB	bShiftCount
	localB	iCount
	localB	bType24 		; zero if 24 bit bmp involved; DIBclrs.
	localB	oRed
	localB	oGreen
	localW	wDIBytesScanline
	localW	wColors
	localW	wTransferFunction
	localW	wColorTableFunction
	localW	wSegCrossProc
	localW	wTmpSrc
	localW	wSaveStack
	localW	wTopOfBuffer
	localW	wBottomOfBuffer
	localW	wReturnValue
	localW	wOldSegmentPixelCount
	localD	lpPalette
	localD	lpDeviceColorMatch
	localV	Current, %(size ParameterBlock)
	localV	SaveParam, %(size ParameterBlock)
	localV	bColorTable, COLORTABLESIZE
	localV	bWorkSpace, WORKSPACESIZE
cBegin
	call	Start8514
	lea	ax, Palette
	mov	seg_lpPalette, ds
	mov	off_lpPalette, ax
	sub	bx, bx
	mov	al, WriteEnable
	shl	al, 1
	adc	bx, bx
	shl	bx, 1
	mov	ax, cs:[bx].ColorTableFumble
	mov	wColorTableFunction, ax
	mov	wTransferFunction, bx
	cmp	DExtY, 0
	jne	DIBExtentOkay
DIBErrorNode:
	jmp	DIBError
DIBExtentOkay:
	push	ss			; make es=ss
	pop	es
	lds	si, lpInfoBlock
	mov	bx, 1
	cmp	[si].biPlanes, bx
	jne	DIBErrorNode
	mov	cx, [si].biBitCount
	mov	ax, word ptr [si].biWidth;compute DWORD alligned # of bytes per
	mul	cx			; scanline. Now dx:ax #of bits/scanline
	add	ax, 01fh		; allign to DWORD now
	adc	dx, 0
	and	ax, 0ffe0h
	shr	dx, 1
	rcr	ax, 1
	shr	dx, 1
	rcr	ax, 1
	shr	dx, 1
	rcr	ax, 1
	mov	wDIBytesScanline, ax	; save for later use
	cmp	cx, bx
	je	CountOkay
	cmp	cx, 4
	je	CountOkay
	cmp	cx, 8
	je	CountOkay
	cmp	cx, 24
	jne	DIBError
	sub	cx, cx
CountOkay:
	mov	bType24, cl		; a Flag; if 0 24 Bit/Pixel DIB
	shl	bx, cl			; either 1, 4, 8, or 0
	mov	wColors, bx		; 1<<cl=# of colors in colortable
	dec	bx
	mov	bMask, bl		; bMask has ones for each bit in pixel
	or	cx, cx
	jnz	Not24BitDIB

	mov	ax, DataBASE
	mov	bx, DataOFFSET szGDI	;name string must be in fixed segment
	cCall	GetModuleHandle, <ax, bx>
	sub	dx, dx
	mov	bx, 449 		;ord. # of DeviceColorMatch in GDI
	cCall	GetProcAddress, <ax, dx, bx>
	mov	seg_lpDeviceColorMatch, dx
	mov	off_lpDeviceColorMatch, ax

	mov	ax, DExtX		; 24 Bits/pixel DIB specific stuff here
;	 shl	 ax, 1			 ; oCount = 3*DExtX
;	 add	 ax, DExtX
	mov	Current.oCount, ax	;oCount=X extent
	mov	ax, SrcX		; wStartSkip = 3*SrcX
	shl	ax, 1
	add	ax, SrcX
	mov	Current.wStartSkip, ax	;StartSkip=byte offset to actual pixels
	sub	al, al			;in current line
	mov	Current.bRem, al	; bRem=0
	mov	Current.bStartRem, al	; bStartRem=0
	mov	bx, wTransferFunction
	mov	ax, cs:[bx].TwentyFourBitDIB
;	 lea	 ax, Transfer24BitDIB	 ; select proper transfer routines
	lea	bx, SegmentBoundaryProc24
	jmp	TransferSelected
DIBError:
	xor	ax, ax
	jmp	DIBEnd
Not24BitDIB:				; pre-compute params for transfer funct
	add	si, word ptr [si].biSize; we need to build a Color Table in
	mov	cx, wColors		; case of 1, 4, or 8 Bit/pixel DIBs
	lea	di, bColorTable
	cCall	wColorTableFunction
	mov	al, bType24		; al=#of bits/pixel
	sub	ah, ah
	mov	cx, ax			; cx=#of bits/pixel
	mov	al, 8
	div	cl			; iCount=8/BitsPixel
	mov	iCount, al		; compute inner loop count
	mov	bl, al			; compute #of bytes to skip
	sub	bh, bh			; StartSkip = SrcX/iCount
	mov	ax, SrcX		; and Start Rem = SrcX%iCount
	sub	dx, dx
	div	bx
	mov	Current.wStartSkip, ax
	mov	Current.bStartRem, dl
	mov	al, cl			; compute remainder allignment shift
	sub	ah, ah			; if 8 BitsPixel(BP) -> bRem always 0
AllignmentLoop: 			; if 4 BP -> bRem 0 or 1
	shr	al, 1			; if 1 BP -> bRem 0, 1, ..., 7
	jc	AllignmentLoopEnd
	inc	ah
	jmp	short AllignmentLoop
AllignmentLoopEnd:
	mov	al, ah			; keep remainder allignment shift cnt
	push	ax			; on stack until use
	mov	ax, cx			; shift count=BitsPixel%8
	and	al, 07h
	mov	bShiftCount, al 	; compute pixel to pixel shift count
	mov	ax, cx
	mul	DExtX
	mov	bx, ax
	and	al, 07h 		; rem=DExtX*BitsPixel%8
	pop	cx			; get remainder allignment shift count
	shr	al, cl
	mov	Current.bRem, al	; compute outer loop remainder
	shr	dx, 1
	rcr	bx, 1
	shr	dx, 1
	rcr	bx, 1
	shr	dx, 1
	rcr	bx, 1			; oCount=DExtX*BitsPixel/8
 	mov	al,iCount
 	mov	ah,al
 	dec	ah
 	sub	al,Current.bStartRem	; adjust outer count and remainder to
 	and	al,ah			; al = # of partial pels in 1st byte.
 	mov	ah,Current.bRem		; take pixels in partial bytes at the
 	sub	ah,al			; start into account as well
 	cmp	bx,0
 	je	short OuterCountOkay
	dec	bx
	add	ah, iCount
OuterCountOkay:
	mov	Current.bRem, ah	; save adjusted remainder
	mov	Current.oCount, bx	; save adjusted outer count
	lea	ax, Transfer148BitDIB	; select proper transfer routines
	lea	bx, SegmentBoundaryProc148
TransferSelected:
	mov	wTransferFunction, ax
	mov	wSegCrossProc, bx
	call	Setup8514Rect		; set dimensions, issue write command
	lds	ax, lpBits		; setup source pointer
	lea	bx, bColorTable 	; ss:bx->translate table
	mov	wTmpSrc, ax

	mov	ax, SrcY		; offset initial pointer by SrcY
	sub	ax, nStart		; minus nStart lines into bitmap
	mul	wDIBytesScanline	; DX=# of segments to advance
	add	wTmpSrc, ax		; AX=offset to pixles
	mov	ax, dx
	mov	cx, __AHIncr
	mul	cx
	mov	dx, ds
	add	dx, ax
	mov	ds, dx
	mov	dx, PIX_TRANS_REG	;write directly to the board
	mov	al, bMask		;put pixel mask into DI (hi byte=0ffh)
	mov	ah, 0ffh
	mov	di, ax
if 0
	mov	ax, sp			; allocate a temporary buffer for
	mov	wSaveStack, ax		; results on stack.  First, check that
	mov	cx, NEWSTACK		; there is enough room on the stack,
	cmp	ax, cx			; then set sp to its new value.  If
	jna	AlternateBuffer 	; there is not enough room on stack,
	mov	sp, cx			; exit.  Finally, set di to bottom of
	mov	di, cx			; tmp buffer. Assume es=ss
	mov	wBottomOfBuffer, cx
	test	ax, ODD 		; make buffer size an even number
	jz	SpaceOkay
	dec	ax
SpaceOkay:
	mov	wTopOfBuffer, ax	; keep for buffer-full check
	jmp	short TransferLoop
AlternateBuffer:			; if there is not enough room on the
	lea	ax, bWorkSpace		; stack, use the small buffer set
	mov	wBottomOfBuffer, ax	; aside on stack frame and hope it all
	mov	di, ax			; will work.
	add	ax, WORKSPACESIZE
	mov	wTopOfBuffer, ax
endif
TransferLoop:
	mov	ax, wTmpSrc
	mov	si, ax			; anticipate no seg. bound. crossing
	add	ax, wDIBytesScanline
	mov	wTmpSrc, ax
	jnc	NoSegmentCrossing
	cCall	wSegCrossProc		; do proper seg. bnd. crossing
	jmp	short BottomOfTransferLoop

NoSegmentCrossing:
	cCall	wTransferFunction	; now do first half of this line
BottomOfTransferLoop:
	dec	DExtY
	jnz	TransferLoop
;	 inc	 di			 ; if di odd, (di>>1)->(di>>1)+1, and
;	 call	 DumpBuffer		 ; if even (di>>1)->(di>>1) (no change)
;	 mov	 sp, wSaveStack 	 ; restore stack pointer-free tmp buffer
	mov	ax, wReturnValue	; our return value

DIBEnd:
	push	ax
	call	Stop8514
	pop	ax
cEnd

;******************************Private*Routine*********************************
;This routine is called to handle segment boundary crossings in 24
;bits/pixel DIBs.  Three different scenarious are possible:
; o Segment boundary is crossed in the section to be skipped over (its size is
;   specified by SrcX). In this case the segment is updated and the transfer
;   function is called with modified wStartSkip.
; o Segment boundary is crossed in the middle of the line segment to be
;   displayed.	In this case the transfer function is called with modified
;   oCount and bRem, the segment is updated, and the transfer function is
;   called a second time with wStartSkip=0 and modified oCount, bRem, and
;   bStartRem.
; o Segment boundary is crossed past the line segment to be displayed. In this
;   case the transfer function is called with normal parameters and the
;   segment is updated afterwards.
;On entry: ax -> number of bytes in next segment (updated wTmpSrc)
;******************************************************************************

SegmentBoundaryProc24	proc near
	mov	cx, ax
	mov	ax, wDIBytesScanline
	sub	ax, cx			; ax number of bytes left in old seg.
	jz	CallAndUpdateSegment24
	mov	cx, ax
	call	SaveParameters
	sub	ax, Current.wStartSkip	; NC->ax=#of bytes left to be displayed
	jc	NonZeroStart24		; CY->wStartSkip>#of bytes in old seg.
	mov	wOldSegmentPixelCount, ax
	mov	dx, ax
	mov	ax, DExtX		;make X ext into a byte extent
	shl	ax, 1
	add	ax, DExtX
	cmp	dx, ax			;DExtX <= # of pixels left to do?
	jnb	CallAndUpdateSegment24	;yes, transfer them and update segment
	mov	ax, dx			;ax=# of bytes left to do
	sub	dx, dx			;prepare of division
	mov	cx, 3
	div	cx			; ax:# of pixles, dx:remaining bytes
	mov	Current.oCount, ax	; in old segment. put these values in
	mov	Current.bRem, dl	;oCount and bRem
	cCall	wTransferFunction	;do rest of old (current) segment
	mov	ax, DExtX		;now commpute how many pixles are left
	shl	ax, 1			;to blt out.
	add	ax, DExtX
	sub	ax, wOldSegmentPixelCount
	mov	dx, ax			;dx=ax=# of bytes in new segment
	mov	cx, 3
	mov	al, Current.bRem	;if any partial pixels were started
	or	al, al
	jz	bRemOkay
	mov	ah, cl			;save cl
	sub	cl, al			;now cl=1 or 2
	sub	dl, cl			;dx=dx-# of bytes left in partial pixel
	sbb	dh, ch			;ch=0
	mov	al, cl
	mov	cl, ah			;restore cl
bRemOkay:
	mov	Current.bStartRem, al	;store # of bytes in partial pixel
	mov	ax, dx			;ax=adjusted # of bytes in new segment
	sub	dx, dx			;prepare for division
	mov	Current.wStartSkip, dx	;no start skip at beginning of new seg.
	mov	Current.bRem, dl	;and no remainder either
	div	cx			;ax=number of pixels in new segment
	mov	Current.oCount, ax	; dx better be zero at this point
UpdateSegmentAndCall24:
	call	NextSegmentDS		;update selector
	cCall	wTransferFunction	;do remaining pixles in scanline
	call	RestoreParameters	;restore standard parameters
	jmp	short EndSegBndcr24	;we're done
NonZeroStart24:
	sub	Current.wStartSkip, cx
	jmp	short UpdateSegmentAndCall24
CallAndUpdateSegment24:
	cCall	wTransferFunction
	cmp	DExtY,1
	je	short @f
	call	NextSegmentDS
EndSegBndcr24:
@@:	ret
SegmentBoundaryProc24	endp

;******************************Private*Routine*********************************
;This routine is called to handle segment boundary crossings in 1, 4, or 8
;bits/pixel DIBs.  There are three different scenarious possible:
; o Segment boundary is crossed in the section to be skipped over (its size is
;   specified by SrcX). In this case the segment is updated and the transfer
;   function is called with modified wStartSkip.
; o Segment boundary is crossed in the middle of the line segment to be
;   displayed.	In this case the transfer function is called with modified
;   oCount and bRem=0, the segment is updated, and the transfer function is
;   called a second time with wStartSkip=bStartRem=0 and modified oCount and
;   bRem.
; o Segment boundary is crossed past the line segment to be displayed. In this
;   case the transfer function is called with normal parameters and the
;   segment is updated afterwards.
;On entry: ax -> number of bytes in next segment (updated wTmpSrc)
;******************************************************************************

SegmentBoundaryProc148	proc near
	mov	cx, ax			; updated tmp source in cx
	mov	ax, wDIBytesScanline
	sub	ax, cx			; ax=#of bytes left to do in old segm.
	jz	CallAndUpdateSegment
	mov	cx, ax			; keep # of bytes in old segment in cx
	call	SaveParameters
	push	bx
	mov	bl, iCount
	sub	bh, bh
	mul	bx			; dx:ax #of pixels in old segment
	sub	ax, SrcX		; dx better be zero
	pop	bx
	jc	NonZeroStart
	mov	wOldSegmentPixelCount, ax
	cmp	ax, DExtX
	jnb	CallAndUpdateSegment
	sub	dx, dx			; now it's time to convert the pixel
	mov	cl, iCount		;count in ax into a byte count so that
	sub	ch, ch			;ax=oCount
	div	cx
	cmp	Current.bStartRem, ch	; at this point prev. Start up params
	jz	NoPartialBytes		; are still valid. dh better be zero
	dec	ax
NoPartialBytes:
	mov	Current.oCount, ax
	mov	Current.bRem, dl	; dh better be zero
	mov	dx, PIX_TRANS_REG	;write directly to the board
	cCall	wTransferFunction	; now do first half of this line
	mov	ax, DExtX
	sub	ax, wOldSegmentPixelCount
	mov	cl, iCount
	sub	ch, ch
	sub	dx, dx
	div	cx
	mov	Current.oCount, ax
	xchg	Current.bRem, dl
	mov	al,dl
	mov	Current.wStartSkip, 0
	mov	Current.bStartRem, al
UpdateSegmentAndCall:
	call	NextSegmentDS
	mov	dx, PIX_TRANS_REG	;write directly to the board
	cCall	wTransferFunction
	call	RestoreParameters
	jmp	short EndSegBndCr148
NonZeroStart:				; cx: Bytes left to do in old segment
	sub	Current.wStartSkip, cx
	jmp	short UpdateSegmentAndCall
CallAndUpdateSegment:
	mov	dx, PIX_TRANS_REG	;write directly to the board
	cCall	wTransferFunction
	cmp	DExtY,1
	je	short @f
	call	NextSegmentDS
EndSegBndCr148:
@@:	ret
SegmentBoundaryProc148	endp

;******************************Private*Routine*********************************
;moves ds to the next segment (or selector in protected mode) and makes si 0
;modifies ax
;******************************************************************************

NextSegmentDS	proc near
	mov	ax, ds			; get next segment with a little help
	add	ax, __AHIncr		; from kernel
	mov	ds, ax
	sub	si, si
	ret
NextSegmentDS	endp

;******************************Private*Routine*********************************
;In order to use the selected transfer function even in the case of segment
;boundary crossings in the source bitmap, certain parameters used by the
;transfer function must be modified and later restored.  All the parameters
;that might change are grouped together so that they can be copied easily to a
;temporary buffer.
;does not alter any register.
;******************************************************************************

SaveParameters	proc near
	push	es
	push	ds
	push	di
	push	si
	push	cx
	push	ax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	lea	di, SaveParam
	lea	si, Current
	mov	cx, size ParameterBlock
rep	movsb
	pop	ax
	pop	cx
	pop	si
	pop	di
	pop	ds
	pop	es
	ret
SaveParameters	endp

;******************************Private*Routine*********************************
;The complement of SaveParameters.  Does not alter any register.
;******************************************************************************

RestoreParameters   proc near
	push	es
	push	ds
	push	di
	push	si
	push	cx
	push	ax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	lea	si, SaveParam
	lea	di, Current
	mov	cx, size ParameterBlock
rep	movsb
	pop	ax
	pop	cx
	pop	si
	pop	di
	pop	ds
	pop	es
	ret
RestoreParameters   endp

;******************************Private*Routine*********************************
;Just a stub for now.
;******************************************************************************

Transfer24BitDIB8   proc near
	add	si, Current.wStartSkip	;make pointer to actual pixels
	mov	cx, Current.oCount	;get outer loop count
	mov	al, Current.bStartRem	;do we need to finish a partially
	or	al, al			;started pixel?
	jz	Transfer24BitLoop_8	;no, just do the real thing
	inc	cx			;need to loop once more than anticipated
	push	cx			;start the outer loop here
	mov	dl, oRed		;always get old red
	cmp	al, 2			;is this all we need to restore?
	je	EntryGreen24_8		;yes, enter the main loop
	mov	ah, oGreen		;need to get old Green
	jmp	short EntryBlue24_8	;enter main loop

Transfer24BitLoop_8:
	push	cx
	lodsb
	mov	dl, al
EntryGreen24_8:
	lodsb
	mov	ah, al
EntryBlue24_8:
	lodsb
	xchg	al, dl			;now: al=red, ah=green, dl=blue
	cCall	lpDeviceColorMatch, <dx, ax, lpConversionInfo>
	mov	dx, PIX_TRANS_REG
	out	dx, ax
	pop	cx
	loop	Transfer24BitLoop_8

	mov	dl, Current.bRem	;take care of remainder
	or	dl, dl			;do we need to start a partial pixel?
	jz	Transfer24BitDIBEnd_8	;no, get out now
	lodsb
	mov	oRed, al
	cmp	dl, 1
	jz	Transfer24BitDIBEnd_8
	lodsb
	mov	oGreen, al
Transfer24BitDIBEnd_8:
	ret
Transfer24BitDIB8   endp

Transfer24BitDIB4   proc near
	add	si, Current.wStartSkip	;make pointer to actual pixels
	mov	cx, Current.oCount	;get outer loop count
	mov	al, Current.bStartRem	;do we need to finish a partially
	or	al, al			;started pixel?
	jz	Transfer24BitLoop	;no, just do the real thing
	inc	cx			;need to loop once more than anticipated
	push	cx			;start the outer loop here
	mov	cx, 0800h
	mov	dl, oRed		;always get old red
	cmp	al, 2			;is this all we need to restore?
	je	EntryGreen24		;yes, enter the main loop
	mov	ah, oGreen		;need to get old Green
	jmp	short EntryBlue24	;enter main loop

Transfer24BitLoop:
	push	cx
	lodsb
	mov	dl, al
EntryGreen24:
	lodsb
	mov	ah, al
EntryBlue24:
	lodsb
	xchg	al, dl			;now: al=red, ah=green, dl=blue
	mov	cx, 0800h		;for GetIndexOfRGB: no acc. bits, 4 BP
	call	GetIndexOfRGBFar	;color index in dh now
	mov	al, dh			;put color index into al
	mov	dx, PIX_TRANS_REG
	out	dx, ax
	pop	cx
	loop	Transfer24BitLoop

	mov	dl, Current.bRem	;take care of remainder
	or	dl, dl			;do we need to start a partial pixel?
	jz	Transfer24BitDIBEnd	;no, get out now
	lodsb
	mov	oRed, al
	cmp	dl, 1
	jz	Transfer24BitDIBEnd
	lodsb
	mov	oGreen, al
Transfer24BitDIBEnd:
	ret
Transfer24BitDIB4   endp

;******************************Private*Routine*********************************
;This routine is used to copy transfer pixels from a device independent bitmap
;to the screen.  Pixels are read from the source buffer, tranlated into
;physical indices, and copied into a temporary buffer on the stack.  If that
;tmp buffer is full `DumpBuffer' is called to copy the tmp buffer to the screen
;On Entry:  ds:si   DIBitmapBits
;	    es:di   tmp buffer on stack
;modifies ax, cx, dx, si, di.
;It is the responsibility of the calling routine to check for segment boundary
;crossings.  The callers stack frame is assumed.
;******************************************************************************

Transfer148BitDIB   proc near
	mov	ax, Current.wStartSkip
	add	si, ax
	mov	al, Current.bStartRem
	or	al, al
	jz	StartByteAlligned
	mov	ah, al
	lodsb
	mov	cl, bShiftCount
StartSkipShiftLoop:
	rol	al, cl			; in this byte
	dec	ah
	jnz	StartSkipShiftLoop
	mov	ch, iCount
	sub	ch, Current.bStartRem
	mov	ah, al
StartLoop:
	rol	ah, cl
	mov	al, ah
	and	ax, di
	xlatb	ss:[bx]
	out	dx, ax
	dec	ch
	jnz	StartLoop
StartByteAlligned:
	mov	cx, Current.oCount
	jcxz	oLoopEnd		; if cx=0 skip outer loop entirely
oLoop148:
	push	cx			;this is the main part if the transfer
	mov	cl, bShiftCount 	;process
	mov	ch, iCount
	lodsb
	mov	ah, al
iLoop148:
	rol	ah, cl
	mov	al, ah
	and	ax, di
	xlatb	ss:[bx]
	out	dx, ax
	dec	ch
	jnz	iLoop148
	pop	cx
	loop	oLoop148
oLoopEnd:
	mov	ch, Current.bRem
	or	ch, ch
	jz	BypassRem148
	mov	cl, bShiftCount
	lodsb
	mov	ah, al
rLoop148:
	rol	ah, cl
	mov	al, ah
	and	ax, di
	xlatb	ss:[bx]
	out	dx, ax
	dec	ch
	jnz	rLoop148
BypassRem148:
	ret

Transfer148BitDIB   endp

if 0
;******************************Private*Routine*********************************
;copies the content of a tmp buffer (es:di) to the screen and resets pointer
;to bottom of tmp buffer.
;Does not alter any registers.
;******************************************************************************

DumpBuffer  proc near
	push	ax
	push	cx
	push	dx
	mov	cx, di
	mov	ax, wBottomOfBuffer
	sub	cx, ax
	shr	cx, 1
	jcxz	DumpBufferEnd		; just a precaution
	push	ds
	push	si
	mov	si, ax
	push	ss
	pop	ds
	mov	dx, PIX_TRANS_REG
rep	outsw
	pop	si
	pop	ds
DumpBufferEnd:
	mov	di, wBottomOfBuffer
	pop	dx
	pop	cx
	pop	ax
	ret
DumpBuffer  endp
endif

;******************************Private*Routine*********************************
;This routine sets the 8514's X, Y, DX, and DY registers to the dimensions of
;the rectangle to be drawn and issues the actual draw command.	Also, sets
;8514's mode, and mix registers.  Checks that FIFO has enough room to accept
;incomming data
;Modifies ax, dx
;******************************************************************************

cProc	Setup8514Rect, <NEAR, PUBLIC>
cBegin
	mov	dx, STATUS_REG
WaitQueueEmpty: 			; before we start writing to the 8514
	in	ax, dx			; we want to make sure that the input
	and	ah, 02h 		; queue is empty, so that we don't need
	jnz	WaitQueueEmpty		; to check each time we write something
	mov	dx, MISC_REG		;to set mode and height of rectangle
	mov	ax, 0a080h		;set to host supplied data
	out	dx, ax
	mov	dx, FRGD_MIX_REG	; set the forground mix register to
	mov	ax, 47h 		; source-copy with host supplied data
	out	dx, ax
	mov	dx, BKGD_MIX_REG	; do same with background mix register
	out	dx, ax
	mov	dx, CUR_X_POS		;set X origin
	mov	ax, DestX
	out	dx, ax
	mov	dx, CUR_Y_POS		;set Y origin
	mov	ax, DestY		;note: we start drawing at the bottom
	add	ax, DExtY		; of the rectangle
	dec	ax
	out	dx, ax
	mov	dx, MAJ_AXIS_PCNT	;set X extent
	mov	ax, DExtX
	dec	ax
	out	dx, ax
	mov	dx, MISC_REG		;set Y extent
	mov	ax, DExtY
	mov	wReturnValue, ax
	dec	ax
	out	dx, ax
	mov	dx, COMMAND_REG
	.errnz	COMMAND_REG-STATUS_REG
CheckForRoom:
	in	ax, dx			;make sure there is enough room for 1st
	test	al, 40h 		;data item on top of write command
	jnz	CheckForRoom
	mov	ax, WRITECOMMAND	;issue proper write command
	out	dx, ax
cEnd

;******************************Private*Routine*********************************
;This routine writes to a bunch of the 8514's registers to set up the
;environment needed to draw to the screen.  This routine also makes sure that
;all pending commands are executed before anything is altered.
;Modifies ax, bx, dx.
;******************************************************************************

cProc	Start8514, <NEAR, PUBLIC>
cBegin
	mov	ax, DestX		; exclude cursor from area we'll blt to
	mov	bx, DestY
	push	ax
	push	bx
	add	ax, DExtX
	add	bx, DExtY
	push	ax
	push	bx
	cCall	<far ptr CursorExclude>
cEnd

;******************************Private*Routine*********************************
;This routine puts the 8514 back into a `defined state' after we're done
;drawing our rectangle.
;******************************************************************************

Stop8514    proc near
;	 mov	 dx, FRGD_MIX_REG	 ; set the forground mix register to
;	 mov	 ax, DEFAULTMIX 	 ; default mix
;	 out	 dx, ax
;	 mov	 dx, BKGD_MIX_REG	 ; do the same with background mix
;	 out	 dx, ax 		 ; register
	cCall	<far ptr CursorUnExclude>
	ret
Stop8514    endp

BuildColorTable8    proc near
CopyIndexLoop:				; The color table that is passed
	lodsw				; is an array of physical color indices
	stosb				; Each index is passed as a word, but
	loop	CopyIndexLoop		; only the lower byte is used here
	ret
BuildColorTable8    endp

BuildColorTable4    proc near
	cCall	BuildColorTable_4, <lpPalette>
	ret
BuildColorTable4    endp


sEnd	DIBSeg
end
