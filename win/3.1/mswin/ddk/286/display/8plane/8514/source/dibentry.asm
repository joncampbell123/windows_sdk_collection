;*******************************Module*Header**********************************
;this module contains the entry points for combined RLE and DIB calls.	The
;appropriate RLE or DIB routines (which are working just fine and I don't
;want to break what is working) are called with the parameters they expect.
;******************************************************************************

.286
.xlist
include cmacros.inc
include rlebitm.inc
include gdidefs.inc
include drvpal.inc
.list

sBegin	Data
externB PaletteFlags
sEnd	Data

createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	DIBSeg

assumes cs, DIBSeg
assumes ds, Data

externNP    RLEBitmap
externNP    DeviceBitmapBits
externNP    DIBToScreen

cProc	MemoryDIBBits, <FAR, PUBLIC, WIN, PASCAL>, <di>
	parmD	lpPdevice		; device dependent bmp
	parmW	wSetGet 		; 0 -> set, 1 -> get
	parmW	iStart			; first scan to get/set
	parmW	nNumScans		; the number of scans to process
	parmD	lpBits			; the DIBmp
	parmD	lpInfoBlock		; all the goodies about the DIBmp
	parmD	lpDrawMode		;draw mode (for RLE stuff)
	parmD	lpConversion		;how to do color mapping on palette dev

	localW	wDataSel
	localW	wRetVal
	localV	FakeClipRect, %(size RECT)
	localB	bLocalPaletteFlags
cBegin
	assumes ds, Data

	sub	ax, ax			;assume failure
	cmp	seg_lpPdevice, ax	;is there a PDevice
	je	MDIBExitNode_10 	;no.  Exit with error
	les	di, lpPdevice		;ES:DI-->PDevice
	cmp	es:[di].bmType, 2000h	;is it the screen?
MDIBExitNode_10:
	je	MDIBExitNode		;yes.  Exit with error
	mov	bx, es:[di].bmWidth	;get width and height of bmp
	mov	cx, es:[di].bmHeight
	mov	FakeClipRect.top, ax	;set clip region to bitmap dimensions
	mov	FakeClipRect.left, ax
	mov	FakeClipRect.right, bx
	mov	FakeClipRect.bottom, cx
	les	di, lpInfoBlock
	test	byte ptr es:[di].biStyle, 03h
	jz	DealWithDIBCall
	mov	dx, nNumScans		;save the # of scans to do locally
	mov	wRetVal, dx		;since RLEBitmap may change org. param.
	mov	wDataSel, ds		;save selector to data segment locally
	mov	dl, [PaletteFlags]	;save actual palette flags locally and
	mov	bLocalPaletteFlags, dl	; set no color Xlate in boardblt
	and	[PaletteFlags], NOT BITBLTACCELERATE
	lea	dx, FakeClipRect
	farPtr	lpFakeClip, ss, dx
	arg	lpPdevice		;PDevice
	arg	ax			;DestX
	arg	iStart			;DestY
	arg	bx			;ExtX
	arg	nNumScans		;ExtY
	arg	wSetGet 		;SetGet
	arg	lpFakeClip		;lpClipRect
	arg	lpDrawMode		;draw mode
	arg	lpInfoBlock		;RLE info
	arg	lpBits			;RLE Bits
	arg	lpConversion		;256 to 16 color mapping table
	cCall	RLEBitmap		;make RLE call
	cmp	wSetGet, 0		;is it a SetDIBits?
	je	MDIBRestoreFlags	;yes.  Always return # of scans
	or	ax, dx			;it was a get.	If size==0 return 0
	neg	ax			;set carry of AX!=0
	sbb	ax, ax			;extend carry into AX
	and	wRetVal, ax		;use as mask on return value (nop if NZ)
MDIBRestoreFlags:
	mov	ds, wDataSel
	assumes ds, Data
	mov	al, bLocalPaletteFlags	;now restore the actual palette flags
	mov	[PaletteFlags], al	; to use in a BitBlt
	mov	ax, wRetVal		;return the # of scans done
MDIBExitNode:
	jmp	short MDIBExit
DealWithDIBCall:
	arg	lpPdevice
	arg	wSetGet
	arg	iStart
	arg	nNumScans
	arg	lpBits
	arg	lpInfoBlock
	arg	lpConversion
	cCall	DeviceBitmapBits
MDIBExit:
cEnd

cProc	PhysicalDIBBits, <FAR, PUBLIC, WIN, PASCAL>, <si, di>
	parmD	lpPDevice
	parmW	wDestX
	parmW	wDestY
	parmW	iStart
	parmW	nNumScans
	parmD	lpClipRect
	parmD	lpDrawMode
	parmD	lpBits
	parmD	lpInfoBlock
	parmD	lpConversion

	localW	wSrcX
	localW	wRetVal
cBegin
	les	di, lpClipRect
	mov	ax, es:[di].left
	mov	bx, es:[di].right
	mov	cx, es:[di].bottom
	mov	si, cx
	mov	dx, es:[di].top
	sub	bx, ax			;BX=ExtX
	sub	cx, dx			;CX=ExtY
	cmp	seg_lpPDevice, 0
	je	PDIBExitNode
	les	di, lpPDevice
	test	es:[di].bmType, 2000h
	jz	PDIBExitNode
	les	di, lpInfoBlock
	test	byte ptr es:[di].biStyle, 03h
	jz	DealWithDIBCallAgain

	mov	dx, nNumScans		;save the # of scans to do locally
	mov	wRetVal, dx
	mov	bx, word ptr es:[di].biWidth[0]
	mov	cx, word ptr es:[di].biHeight[0]
	arg	lpPDevice
	arg	wDestX			;DestX
	arg	wDestY			;DestY
	arg	bx			;ExtX
	arg	cx			;ExtY
	arg	0			;wSetGet=0 -> Set RLE bits to screen
	arg	lpClipRect
	arg	lpDrawMode
	arg	lpInfoBlock
	arg	lpBits
	arg	lpConversion
	cCall	RLEBitmap
	mov	ax, wRetVal		;return the # of scans done
PDIBExitNode:
	jmp	short PDIBExit

DealWithDIBCallAgain:			;es:di --> lpInfoBlock
	push	ax			;AX: cliprect.left
	sub	ax, wDestX
	mov	wSrcX, ax
	pop	ax
	jge	PDIBDestXOkay
	mov	ax, wDestX
	mov	wSrcX, 0
PDIBDestXOkay:
	mov	si, iStart
	neg	si
	add	si, wDestY
	add	si, word ptr es:[di].biHeight
	cmp	si, dx			;is Start above top?
	jle	PDIBExit		;yes, zero clip rgn->exit now
	add	cx, dx			;cx=bottom
	sub	si, nNumScans		;si=DestY
	cmp	si, cx			;are we below the bottom?
	jge	PDIBExit		;yes, exit now

	cmp	si, dx			;clip to top
	jge	PDIBTopClipped
	push	dx
	sub	dx, si
	sub	nNumScans, dx
	add	si, dx			;now si=top
	pop	dx
PDIBTopClipped:
	push	si
	add	si, nNumScans
	cmp	si, cx
	jle	PDIBBottomClipped
	sub	si, cx
	sub	nNumScans, si
PDIBBottomClipped:
	pop	si

	mov	cx, wDestY
	add	cx, word ptr es:[di].biHeight
	sub	cx, si
	sub	cx, nNumScans

	arg	lpPDevice
	arg	ax			;DestX
	arg	si			;DestY
	arg	bx			;ExtX
	arg	nNumScans		;ExtY
	arg	wSrcX			;SrcX
	arg	cx			;SrcY
	arg	iStart
	arg	lpBits
	arg	lpInfoBlock
	arg	lpConversion
	cCall	DIBToScreen
PDIBExit:
cEnd

sEnd
end
