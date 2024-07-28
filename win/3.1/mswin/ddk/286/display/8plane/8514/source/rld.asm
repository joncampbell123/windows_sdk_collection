;*******************************Module*Header**********************************
;RLE (run length encoded bitmaps) stuff.
;RLEs have the following format: |count|index|...
;Where count is the length of the run and index is (are) the pixel value(s) that
;is (are) count times to be put into the output stream, i.e., on the screen.
;This is done on a per scanline basis.	There are special escapes to indicate
;the end of a line, the end of the RLE encoded bitmap and some special modes
;explained below.  Here is a summary of all escapes:
;Count=0:	Next byte
;		    0		End of Line escape
;		    1		End of Bitmap escape
;		    2		Delta encoding.  Next 2 bytes are unsigned
;				offsets off the current position.  Resume at
;				new position
;		    3+		Absolute Run Mode.  This number is a count of
;				pixels that are to be used face value rather
;				than being interpreted as count|index.
;
;There are two RLE formats: an 8 bit/pixel and a 4 bit/pixel.  In the 8 b/p
;format one color index occupies one byte.  In the 4 b/p format the byte
;following the count contains 2 color indices.	The high nibble contains pixel
;one and the low nibble pixel 2.  When outputting a run encoded in 4 b/p
;format, the output routine needs to alternate between the 2 pixels when
;writing to the output stream, starting with the pixel stored in the high
;nibble.  A total of count pixels are to be written.  If the count is one, the
;2nd pixel is a `don't care'.
;
;
;All code in this module will work in 4 plane as well as in 8 plane mode of
;the 8514 display driver.  The source and/or destination DC can be either a
;memory of a screen DC (a total of 4 combinations).  At the API level, however,
;only MemDC to MemDC and MemDC (source) to Screen DC (destination) transfers
;are enabled.
;
;[History]
;   Summer '89:     8 b/p code written and debugged by GunterZ [Gunter Zieber]
;   Oct 19 '89:     4 b/p code finished and debugged (works like a champ!)
;		    by GunterZ
;   March 16 '90:   completely re-wrote RLE decoding to fix bugs and greatly
;		    enhance performance --by-- GunterZ
;******************************************************************************

.286
.xlist
include cmacros.inc
include 8514.inc
incDrawMode equ 1
include gdidefs.inc
include rlebitm.inc
include savescrn.inc
.list

HI_SRCCOPY	    equ     00cch
LO_SRCCOPY	    equ     0020h
stack_top	    equ     000Ah

LongPtr struc
    off dw  ?
    sel dw  ?
LongPtr ends

DoubleWord  struc
    lo	dw  ?
    hi	dw  ?
DoubleWord  ends

sBegin	Data
externW VisibleScreenClip
externW FullScreenClip
externB WriteEnable
externB Rop2TranslateTable
externB TranslateTable
externB Palette

externW FreeSpaceyOrg		;in IBMDRV.ASM
externW FreeSpaceyExt		;in IBMDRV.ASM
externW SaveSpaceyOrg		;in SAVESCRN.ASM
externB ShadowMemoryTrashed	;in SAVESCRN.ASM
externB SaveInUse		;in SAVESCRN.ASM
externW SaveSpaceyExt
sEnd	Data

externA     __AHIncr
externFP    SetScreenClipFar		;in routines.asm
externFP    CursorExclude		; ditto
externFP    CursorUnExclude		; ditto
externFP    BoardBltFar 		;in boardblt.asm

createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	DIBSeg

	assumes cs, DIBSeg
	assumes ds, Data
	assumes es, nothing

externNP    BuildColorInfo4
externNP    BuildColorTable_4

ColorTableFunction  label   word
	dw	SetColorTable8
	dw	SetColorTable4
	dw	GetColorTable8
	dw	GetColorTable4

RLEFunction label   word
	dw	DecodeMemory
	dw	EncodeMemory
	dw	PhysicalRLE
	dw	GetPhysicalRLE

DumpToBmpTable	label word
	dw	DumpToBmp1
	dw	DumpToBmp4
	dw	DumpToBmp8

DecodeFunction	label	word
	dw	DecodeRLE8
	dw	DecodeRLE4

EncodeFunction	label	word
	dw	EncodeRLE8
	dw	EncodeRLE4

;********************************Public*Routine********************************
;RLEBitmap() is the entry point to all RLE functions that are supported in the
;8514 display driver.
;
;Entry:
;	ES:DI-->lpRLEInfo
;
;Exit:
;on GET DX:AX	size of RLE bitmap needed to store RLE bits
;******************************************************************************

cProc	RLEBitmap, <NEAR, PUBLIC, WIN, PASCAL>, <si, di>

	parmD	lpPDevice
	parmW	DestX
	parmW	DestY
	parmW	ExtX
	parmW	ExtY
	parmW	wSetGet
	parmD	lpClipRect
	parmD	lpDrawMode
	parmD	lpRLEInfo
	parmD	lpRLEBits
	parmD	lpConversionInfo

	localB	bBitMask
	localB	bCurrentCount
	localW	oCount
	localW	wRem
	localW	wHeight
	localW	wScreenCurrentX
	localW	wScreenCurrentY
	localW	wFreeSpaceYOrg
	localW	wDestX
	localW	wDestY
	localW	wFakePDevice
	localW	pfnStorePixel1
	localW	pfnStorePixel2
	localW	wRLEtype

	localB	bFirstMask
	localB	bLastMask
	localB	bLastShift
	localW	wStackTop
	localW	pfnDumpBuffer
	localW	wBytesScan
	localW	wScansSeg
	localW	wFillBytes
	localW	wScansLeft
	localW	wLastY
	localW	wSegmentIndex
	localD	lpBmpBits

	localD	dwRLESize
	localD	lpPrevDest
	localD	lpFakePDevice
	localD	dwRop3
	localD	lpTempDestPtr
	localV	LocalClipRect, %(size RECT)
	localV	bColorTable, 256

cBegin
	assumes ds, Data

	cld				;want to count up
	mov	ax, word ptr es:[di].biStyle
	and	ax, 02h 		;4 b/p (D1=1) or 8 b/p (D0=1) RLE?
	mov	wRLEtype, ax		;store what we found out
	mov	al, WriteEnable
	mov	bBitMask, al
	sub	bx, bx
	mov	dwRLESize.lo, bx
	mov	dwRLESize.hi, bx
	cmp	bx, wSetGet		;select the appropriate Color table
	sbb	bl, bl			;function
	and	bl, 04h 		;D2=1 --> Get RLE bits
	inc	al
	shr	al, 3			;D1=1 --> Four plane device
	or	bl, al
	mov	ax, cs:[bx].ColorTableFunction
	push	ds			;save DS around the color table call
	call	ax			;go execute the color table function
	pop	ds
	les	di, lpPDevice		;get pointer to physical device
	mov	ax, es:[di]		;memory or screen?
	shr	ah, 2			;D3=1 --> Screen Device
	or	bl, ah
	shr	bl, 1
	and	bl, 0feh		;D1->Get RLE, D2->Screen device
	mov	ax, cs:[bx].RLEFunction
	call	ax
	cmp	wSetGet, 1		;always store the length of the RLE on
	jne	RLEEnd			;a `get' bits
	mov	ax, dwRLESize.lo
	mov	dx, dwRLESize.hi
	lds	si, lpRLEInfo
	mov	[si].biSizeImage.lo, ax
	mov	[si].biSizeImage.hi, dx
RLEEnd:
cEnd

;******************************Private*Routine*********************************
;ES:DI --> lpPDevice (memory bitmap).  lpDrawMode and lpClipRect are ignored.
;DS = Data  DestX, DestY, ExtX, ExtY are wrt PDevice (bitmap)
;First a part (or all) of the source bitmap is blt'ed into off-screen memory.
;Then the Encode-off_the-screen routine is called to do the actual encoding.
;******************************************************************************

cProc	EncodeMemory, <NEAR, PUBLIC>
cBegin
	call	SaveParameters
	cmp	ax, X_SIZE		;ensure X extent is within device's
	ja	EMErrorExit		;capabilities
	add	ax, cx			;ensure bmp passed is consistent with
	cmp	ax, es:[di].bmWidth	;other parameters passed
	ja	EMErrorExit
	mov	ax, bx
	add	ax, DestY
	cmp	ax, es:[di].bmHeight
	jna	EMBitmapOkay
EMErrorExit:
	jmp	EMExit
EMBitmapOkay:
	mov	ax, wFreeSpaceYOrg	;set DestY for RLE encodeing proc
	mov	DestY, ax
	mov	DestX, 0		; ditto for DestX
	sub	dx, dx
	mov	cx, [FreeSpaceyExt]	;anticipate no need for banding
	mov	wRem, cx		;and initailize banding variables
	mov	oCount, dx		;accordingly
	cmp	cx, bx			;do we need banding?
	jnb	EMNoBandingNeededNode	;no! We're fine
	test	[SaveInUse], SAVEBITMAPUSED
	jz	EMSaveNotUsed		;no space available from save screen
	mov	[ShadowMemoryTrashed], 0
	mov	[SaveInUse], SAVEBITMAPCANCELED
	mov	ax, [SaveSpaceyOrg]	;set flags that off-screen memory has
	mov	[FreeSpaceyOrg], ax	;been used for something else, de-
	mov	wFreeSpaceYOrg, ax	;allocate save-screen memory
	mov	DestY, ax		;update DestY accordingly
	mov	ax, [SaveSpaceyExt]	;allocate memory from save screen.
	add	[FreeSpaceyExt], ax
	add	cx, ax			;again, anticipate no need for banding
	mov	wRem, cx
	cmp	cx, bx			;do we still need banding?
EMNoBandingNeededNode:
	jnb	EMNoBandingNeeded	;no, now we're fine

EMSaveNotUsed:
	mov	ax, bx
	div	cx			;AX=banding loop count, DX=rem lines
	mov	oCount, ax		;AX>=1
	mov	wRem, dx

	mov	ax, [FreeSpaceyExt]	;this is the Y extent we want to use
	mov	ExtY, ax		;for the banding procedure
	mov	wHeight, ax
	mov	ax, off_lpRLEBits
	mov	off_lpTempDestPtr, ax
	mov	ax, seg_lpRLEBits
	mov	seg_lpTempDestPtr, ax

EMBandingLoop:
	call	BandBitBlt		;Blt band into off-screen video memory
	mov	ax, off_lpTempDestPtr	;establish pointer to destination
	mov	off_lpRLEBits, ax	;(lpRLEBits is reloaded by PhysRLE8
	mov	ax, seg_lpTempDestPtr	;each time its called.	Need to save it
	mov	seg_lpRLEBits, ax	;between calls)
	push	ds
	call	GetPhysicalRLE		;run length encode off the off-scr mem
	pop	ds
	mov	ax, wHeight
	mov	ExtY, ax
	cmp	lpRLEBits.sel, 0
	je	EMUpdateLoop
	sub	di, 2			;over-write End Of Bitmap escape in RLE
	jnc	EMUpdateTempDestPtr	;if necessary, go back to prev. segment
	mov	ax, es
	sub	ax, __AHIncr
	mov	es, ax
EMUpdateTempDestPtr:
	mov	off_lpTempDestPtr, di	;store last position in RLE bmp for
	mov	seg_lpTempDestPtr, es	;next band
EMUpdateLoop:
	dec	oCount			;update loop variable
	jnz	EMBandingLoop

	cmp	lpRLEBits.sel, 0
	je	EMPtrOkay
	mov	ax, off_lpTempDestPtr
	mov	off_lpRLEBits, ax
	mov	ax, seg_lpTempDestPtr
	mov	seg_lpRLEBits, ax
EMPtrOkay:
	mov	cx, wRem
	jcxz	EMExit			;make sure remainder is non-zero.
	mov	ExtY, cx

EMNoBandingNeeded:
	call	BandBitBlt
	call	GetPhysicalRLE
EMExit:
cEnd

;saves passed parameters in local stack.  Makes a fake PDevice (screen) struct.
;On Exit: AX=ExtX, BX=ExtY, CX=DestX, DX=DestY+ExtY

SaveParameters	proc near
	mov	seg_dwRop3, HI_SRCCOPY	;make Rop3=SRCCOPY
	mov	off_dwRop3, LO_SRCCOPY
	mov	wFakePDevice, 2000h	;make a fake PDevice `structure'
	lea	ax, wFakePDevice
	mov	off_lpFakePDevice, ax	;now make a pointer to out fake PDevice
	mov	seg_lpFakePDevice, ss	;on the stack
	mov	ax, [FreeSpaceyOrg]	;save free space origin
	mov	wFreeSpaceYOrg, ax
	mov	cx, DestX		;store passed parameters in a save
	mov	wDestX, cx		;place
	mov	dx, DestY
	mov	ax, ExtX
	mov	bx, ExtY
	add	dx, bx			;make wDestY point to the lower left
	mov	wDestY, dx		;hand corner of src bmp
	ret
SaveParameters	endp

BandBitBlt  proc near
	MakeEmptyFIFO
	mov	bx, ExtY		;make wDestY point to u.l. hand corner
	sub	wDestY, bx		;of next band to blt
	sub	ax, ax
	arg	lpFakePDevice		;lpDestDev
	arg	ax			;DestXOrg
	arg	wFreeSpaceYOrg		;DestYOrg
	arg	lpPDevice		;lpSrcDev (our bitmap)
	arg	wDestX			;SrcX
	arg	wDestY			;SrcY
	arg	ExtX			;ExtX
	arg	bx			;ExtY
	arg	dwRop3			;teritary ROP
	arg	ax			;lpPBrush
	arg	ax
	arg	lpDrawMode		;lpDrawMode
	cCall	BoardBltFar		;call BoardBlt far entry point
	ret
BandBitBlt  endp

;On entry: ES:DI --> lpPDevice (Bitmap) DS=Data

cProc	DecodeMemory, <NEAR, PUBLIC>
cBegin
	mov	LocalClipRect.top, 0
	mov	ax, DestX		;first verify that the passed dest.
	add	ax, ExtX		;and extents lie completely inside the
	mov	cx, es:[di].bmWidth	;the passed in bitmap.	If they don't,
	cmp	ax, cx			;abort the decoding process
	ja	DMExit
	mov	ax, DestY
	add	ax, ExtY
	mov	bx, es:[di].bmHeight
	cmp	ax, bx
	ja	DMExit
	mov	LocalClipRect.bottom, bx
	mov	ax, sp			;now see if there is enough room on the
	sub	ax, cx			; stack to hold one scanline of pixels
	cmp	ax, ss:stack_top	;If there is not enough room exit now.
	jb	DMExit
	mov	wLastY, -1		;causes correct Sel:Off comp. later
	mov	ax, es:[di].bmWidthBytes;copy the # of bytes per scanline to
	mov	wBytesScan, ax		;local variable frame
	mov	ax, es:[di].bmScanSegment   ;copy the # of scans per segment
	mov	wScansSeg, ax		;assuming a huge bitmap
	mov	ax, es:[di].bmFillBytes
	mov	wFillBytes, ax
	mov	ax, es:[di].bmSegmentIndex
	mov	wSegmentIndex, ax
	neg	ax			;set carry if huge bmp
	jc	DMHugeBmp
	mov	wScansSeg, -1
DMHugeBmp:
	mov	ax, es:[di].bmBits.off
	mov	lpBmpBits.off, ax
	mov	ax, es:[di].bmBits.sel
	mov	lpBmpBits.sel, ax
	sub	bx, bx			;determine which function to use to
	mov	bl, es:[di].bmBitsPixel ;copy pixels to dest bmp. 0-->mono
	shr	bx, 1			;2-->color (4 plane), 4-->color (8 pl.)
	mov	ax, cs:[bx].DumpToBmpTable
	mov	pfnDumpBuffer, ax
	call	InitLocalXY
	lds	si, lpRLEBits		;DS:SI --> RLE bitmap
	mov	bx, wRLEtype
	mov	ax, cs:[bx].DecodeFunction
	call	ax			;go and do the work
DMExit:
cEnd

cProc	SetColorTable8, <NEAR, PUBLIC>
cBegin
	mov	cx, 16
	test	wRLEtype, 02h
	jnz	SCTCountOkay
	mov	cx, 256
SCTCountOkay:
	lds	si, lpRLEInfo		;DS:SI-->array of indices (words) that
	add	si, [si]		;map logical colors to physical colors
	push	ss
	pop	es
	lea	di, bColorTable
SCTLoop:
	lodsw
	stosb
	loop	SCTLoop
cEnd

cProc	GetColorTable8, <NEAR, PUBLIC>
cBegin
	test	wRLEtype, 02h		;4 b/p RLE?
	jz	GCT8Exit		;no. don't need to do anything.
	push	ss
	pop	es
	lds	si, lpConversionInfo
	lea	di, bColorTable
	mov	cx, 128
	rep	movsw
GCT8Exit:
cEnd

ColorToMonoMap	label	byte
	db	0, 0, 0, 0, 0, 0, 0, 1
	db	0, 0, 1, 1, 0, 1, 1, 1

cProc	SetColorTable4, <NEAR, PUBLIC>
cBegin
	assumes ds, Data
	mov	ax, ds
	lds	si, lpRLEInfo		;match RLE's colors to system colors
	mov	cx, 16			;lie to other routines
	test	wRLEtype, 02h
	jnz	SCT4CountOkay
	mov	cx, 256
SCT4CountOkay:
	add	si, [si]		;move ptr to RLE bitmap color table
	push	ss
	pop	es
	lea	di, bColorTable
	lea	dx, Palette		;DX:AX-->Palette of system colors
	push	cx			;save CX
	cCall	BuildColorTable_4, <ax, dx>
	pop	cx			;restore CX
	les	di, lpPDevice
	cmp	word ptr es:[di], 0	;bitmap or screen?
	jnz	SCT4Done		;screen.  We're done
	cmp	es:[di].bmBitsPixel, 1	;mono bitmap?
	jne	SCT4Done		;no.  We're done
	lea	si, bColorTable 	;SS:SI-->color table
	push	bx			;don't trash BX
	mov	bx, DIBSegOFFSET ColorToMonoMap
SCT4ColorToMonoLoop:
	lods	byte ptr ss:[si]
	xlatb	cs:[bx]
	mov	ss:[si][-1], al
	loop	SCT4ColorToMonoLoop
	pop	bx
SCT4Done:
cEnd

cProc	GetColorTable4, <NEAR, PUBLIC>
cBegin
	lds	si, lpRLEInfo
	mov	cx, 16			;for now; should be read out of rleinfo
	add	si, [si]		;move ptr to RLE bitmap color table
	lea	ax, bColorTable 	;get offset to local color table
	cCall	BuildColorInfo4, <ss, ax>
cEnd

;******************************Private*Routine*********************************
;here we issue the `read rectangle' command to the 8514.  ;;We'll read one line
;at a time.  Don't need a function to read planes
;******************************************************************************

Set8514ReadScreenCommand    proc near
	MakeEmptyFIFO
	mov	dx, Srcx_PORT
	mov	ax, wScreenCurrentX
	out	dx, ax
	mov	dx, Srcy_PORT
	mov	ax, wScreenCurrentY
	out	dx, ax
	mov	dx, RECT_WIDTH_PORT
	mov	ax, ExtX
	dec	ax
	out	dx, ax
	mov	dx, MODE_PORT
	mov	ax, 0a080h
	out	dx, ax
	mov	ax, ExtY		;read the entire rectangle at once
	dec	ax
	out	dx, ax
	.errnz	RECT_HEIGHT_PORT-MODE_PORT
	mov	dx, COMMAND_FLAG_PORT
	mov	ax, 4130h
	out	dx, ax
SRSCWaitForData:
	in	ax, dx			;make sure date are in the queue before
	and	ah, 01h 		;moving on
	jz	SRSCWaitForData
	mov	si, PATTERN_DEFINE_PORT
	ret
Set8514ReadScreenCommand    endp

cProc	ReadPixel, <NEAR, PUBLIC>
cBegin
	xchg	dx, si
	in	ax, dx			;read next pixel from board
	xchg	dx, si
	and	al, bBitMask		;needed for 4 plane support
cEnd

public	StorePixel
StorePixel  proc near
	add	dwRLESize.lo, 1 	;update size of RLE
	adc	dwRLESize.hi, 0
	stosb				;store that pixel
	or	di, di			;need new selector?
	jz	SPNewSegment
	ret
SPNewSegment:
	mov	di, es			;advance to new selector
	add	di, __AHIncr
	mov	es, di
	sub	di, di
	ret
StorePixel  endp

AccumulateLength    proc near
	add	dwRLESize.lo, 1
	adc	dwRLESize.hi, 0
	ret
AccumulateLength    endp

FakeInsertAbsCount  proc near
	cmp	wRLEtype, 2		;is it RLE4 type?
	je	FIACWordAligned 	;yes, byte count is okay as is!
	test	cl, 1			;make sure we stay word aligned after
	jz	FIACWordAligned 	;finishing an absolute run
	call	AccumulateLength
FIACWordAligned:
	ret
FakeInsertAbsCount  endp

public	InsertAbsCount
InsertAbsCount	proc near
	push	di			;put absolute run count into its place
	push	es
	les	di, lpPrevDest		;get offset to put count
	mov	es:[di], cl		;and store count
	pop	es
	pop	di			;restore dest ptr to next run
	cmp	wRLEtype, 2		;is it RLE4 type?
	je	IACWordAligned		;yes, don't even touch the dest ptr.
	test	cl, 1			;at this point we need to check that
	jz	IACWordAligned		; the absolute run we just finished
	call	StorePixel		; will end up being word aligned
IACWordAligned:
	ret
InsertAbsCount	endp

public	DoNormalEOLStuff
DoNormalEOLStuff    proc near
	sub	bx, bx			;reset current X extent
	sub	dh, dh			;dh: end of bitmap flag
	dec	ExtY
	jz	DNESEndOfBitmap
	ret
DNESEndOfBitmap:
	dec	dh			;signal end of bitmap
	ret
DoNormalEOLStuff    endp

public	StoreCountAndIndex
StoreCountAndIndex  proc near
	mov	al, cl
	call	pfnStorePixel1		;store run length
	mov	al, dl
	call	pfnStorePixel1		;store color index
	ret
StoreCountAndIndex  endp

;******************************Private*Routine*********************************
;On entry: es:di lpPDevice == Screen (will not be needed any more)
;No clipping rectangle is used for getting a RLE.  ROP2 is ignored
;******************************************************************************

cProc	GetPhysicalRLE, <NEAR, PUBLIC>
cBegin
	mov	ax, DestX		;exclude the cursor from the region
	mov	bx, DestY		;we are about to convert to a RLE
	push	ax
	push	bx
	add	ax, ExtX
	add	bx, ExtY
	push	ax
	push	bx
	cCall	CursorExclude		;cursor excluded now

	call	InitLocalXY		;initialize position variables
	mov	ax, DestY		;use top and right of clip rect to
	mov	LocalClipRect.top, ax	;indicate the end of a scanline
	mov	ax, DestX
	add	ax, ExtX
	mov	LocalClipRect.right, ax

	call	Set8514ReadScreenCommand;Get the 8514 ready to go

;now we'that re done preprocessing, encode the bitmap

	lea	ax, AccumulateLength
	lea	bx, FakeInsertAbsCount
	cmp	lpRLEBits.sel, 0
	je	SaveFixup
	lea	ax, StorePixel
	lea	bx, InsertAbsCount
	les	di, lpRLEBits		;establish ptr to dest bmp
SaveFixup:
	mov	pfnStorePixel1, ax
	mov	pfnStorePixel2, bx

	mov	bx, wRLEtype
	mov	ax, cs:[bx].EncodeFunction
	call	ax
	cCall	CursorUnExclude
cEnd

cProc	EncodeRLE8, <NEAR, PUBLIC>
cBegin
	sub	bx, bx			;relative X position; initially = 0
	sub	cx, cx			;accumulate run length in cx

public	GPMainLoop
GPMainLoop:
	call	ReadPixel		;get first index in new run
	mov	dl, al			;save it for later comparison in DL
	inc	bx			;update current X extent
	inc	cx			;update run length (now CX=1)
	cmp	bx, ExtX		;end of scanline?
	jae	GPEndOfLine		;yes, go finish current scanline

public	GPSubsequentPixel
GPSubsequentPixel:
	call	ReadPixel		;get subsequent pixel
	cmp	al, dl			;are both pixels of same color?
	jne	GPAbsolute		;no, may have to enter absolute mode
public	GPNextRun
GPNextRun:
	inc	cx			;update run length
	inc	bx			;update X extent
	cmp	bx, ExtX		;be sure to test for end of scan BEFORE
	jae	GPEndOfLine		; testing end of run
	cmp	cl, 0ffh		;cl >= 2 at this point if not GPNextRun
	je	GPRunComplete
	jmp	short GPSubsequentPixel ;do next pixel

GPExitNode:
	jmp	GPDone

;At this point we decide whether to enter absolute mode or not.  If the pixel
;count in the current run is not equal to 1, just finish the current run and
;start a new one.  If the pixel count is equal to 1, make sure that there are
;at least 3 pixels left to be encoded in the current scanline.	If this is
;the case, enter absolute run mode, else finish the current scanline using
;run(s) of length one and/or two.  BX, the number of pixels fetched in the
;current scanline, is one behind.

public	GPAbsolute
GPAbsolute:
	cmp	cl, 1			;only one pixel of this color?
	jne	GPStoreCurrentRun	;Y->enter abs mode N->complete run first
	push	bx			;in order to enter absolute mode we
	inc	bx			;need to make sure that there are at
	inc	bx			;three more pixels in the current
	cmp	bx, ExtX		;scanline.  If not, finish the line
	pop	bx			;using runs of length 1 and 2
	jbe	GPEnterAbsolute
GPStoreCurrentRun:
	push	ax			;save latest color value
	call	StoreCountAndIndex	;finish last run.  Trashes AL
	sub	cx, cx			;cx=0
	pop	ax			;restore current pixel
	mov	dl, al			;save current pixel
	jmp	short GPNextRun 	;next run: update X, and run length

public	GPEndOfLine
GPEndOfLine:
	call	DoNormalEOLStuff	;check end of bmp, reset BX
	call	StoreCountAndIndex
GPEOL:
	sub	cx, cx
	sub	dl, dl
	call	StoreCountAndIndex	;store EOL escape, init CX for next run
	or	dh, dh			;end of bitmap?
	jz	GPMainLoop		;nope, keep going

public	GPEndOfBitmap
GPEndOfBitmap:
	call	pfnStorePixel1		;write end of bitmap escape, AL=0 here
	inc	ax
	call	pfnStorePixel1
	jmp	short GPDone		;and exit! We're done

public	GPRunComplete
GPRunComplete:
	call	StoreCountAndIndex	;pixel value in dl, rep-count in cl
	sub	cx, cx			;CX=0 reset run length
	jmp	short GPMainLoop	;back into main loop for next run

public	GPEnterAbsolute
GPEnterAbsolute:			;cl=1, al=2nd pixel, dl=first pixel
	mov	dh, al			;save 2nd pixel in DH
	sub	al, al			;al=0: 1st part of Abs mode escape
	call	pfnStorePixel1		;can always store that
	mov	lpPrevDest.off, di	;this is where we need to put the count
	mov	lpPrevDest.sel, es
	call	pfnStorePixel1		;now: ES:DI --> absolute indices
	mov	al, dh			;AL=2nd pixel
	mov	dh, dl			;store first absolute pixel now (in DH)
	dec	cx			;prepare for loop (cx=0 now); we're one

public	GPAAbsoluteModeLoop
GPAAbsoluteModeLoop:			;pixel behind in this loop
	xchg	al, dh			;put most recent pixel in DH
	call	pfnStorePixel1		;store second most recent pixel (in AL)
	inc	bx			;update current X position
	inc	cx			;update abs. pixel count
	cmp	bx, ExtX		;EOL?
	jae	GPAEndOfLine		;we've hit the end of the scanline
	cmp	cl, 0feh		;is this run full?
	je	GPARunComplete		;yes, go finish it up
	call	ReadPixel		;get next pixel into AL
	cmp	al, dh			;are these two pixels of diff. color?
	jne	GPAAbsoluteModeLoop	;yes, continue absolute mode
	cmp	cl, 2			;an absolute run is at least length=3
	jbe	GPAAbsoluteModeLoop

	call	pfnStorePixel2		;nope, exit abs mode al=dh; store count
	mov	cl, 2			;already have two pixels of that color
	mov	dl, dh			;be consistent with normal run mode
	inc	bx			;update X position
	cmp	bx, ExtX		;are we at the end of a line?
	jae	GPEndOfLine		;do EOL stuff, back into GPMainLoop
	jmp	GPSubsequentPixel	;enter normal run mode

public	GPARunComplete
GPARunComplete:
	mov	al, dh			;make sure we store the last pixel too
	call	pfnStorePixel1
	inc	cx			;CL=0ffh now if !EOS
	call	pfnStorePixel2
	sub	cx, cx
GPMainLoopNode:
	jmp	GPMainLoop		;now it's time to start a new run

public	GPAEndOfLine
GPAEndOfLine:
	mov	dl, dh			;save current pixel value in DL
	call	DoNormalEOLStuff	;update line count; BX=0, trashes DH
	mov	al, dl			;store last pixel in abs run
	call	pfnStorePixel1
	inc	cx			;update counter
	call	pfnStorePixel2		;insert abs run length in last run
	jmp	short GPEOL

public	GPDone
GPDone:
cEnd

;******************************************************************************
;******************************************************************************
;The following routines deal with decoding a RLE bitmap
;******************************************************************************
;******************************************************************************

ReadRLE proc near
	lodsb
	or	si, si
	jz	RRUpdateSegment
	ret
RRUpdateSegment:
	mov	si, ds
	add	si, __AHIncr
	mov	ds, si
	sub	si, si
	ret
ReadRLE endp

cProc	VerifyDestAndCnt, <NEAR, PUBLIC>
cBegin
	jcxz	VCDExit 		;first verify dest coordinates and cnt
	mov	si, wScreenCurrentY	;SI: current Y position
	cmp	si, LocalClipRect.top
	jl	VCDExit
	cmp	si, LocalClipRect.bottom
	jge	VCDExit
	clc
	ret
VCDExit:
	stc
cEnd

cProc	BmpComputeY, <NEAR, PUBLIC>
cBegin
	inc	si			;SI: previous scanline
	cmp	si, wLastY
	jne	BCYCheckIfDeltaX

	dec	wLastY
	sub	di, wBytesScan		;ES:DI-->start of dest scanline
	ret

BCYCheckIfDeltaX:
	dec	si			;SI: current Y position
	mov	ax, wScansLeft		;if the two are equal then ES has just
	cmp	ax, wScansSeg		; been bumped down to the prev. sel.
	je	BCYComputeTheHardWay	; and needs to be re-computed
	inc	wScansLeft		;assume we are--make up for prev. dec
	cmp	si, wLastY		;are we still in the same scanline?
	je	BCYDone 		;yes.  No need to recompute ES:DI
BCYComputeTheHardWay:
	mov	wLastY, si
	sub	dx, dx			;for divide
	mov	ax, si
	div	wScansSeg		;AX: # of segments to advance
	mov	si, dx			;SI: # of scanlines to advance
	inc	dx			;DX: # of scans left in segment
	mov	wScansLeft, dx		;save # of scans left locally for later
	mov	di, wSegmentIndex
	mul	di			;AX: selector offset from top of bmp
	add	ax, lpBmpBits.sel
	mov	es, ax
	mov	ax, si
	mul	wBytesScan
	mov	di, ax
	add	di, lpBmpBits.off	;ES:DI-->beginning of dest scanline
BCYDone:
cEnd

cProc	PreviousSegment, <NEAR, PUBLIC>
cBegin
	mov	ax, wScansSeg		;first reset the scans left counter
	mov	wScansLeft, ax
	mov	ax, es			;now bump ES one selector down
	sub	ax, wSegmentIndex
	mov	es, ax
	sub	di, di			;finally reset DI and skip over the
	sub	di, wFillBytes		; fill bytes
cEnd

cProc	DumpToBmp8, <NEAR, PUBLIC>, <si>
cBegin
	cCall	VerifyDestAndCnt	;first verify dest coordinates and cnt
	jc	DB8Exit
	cCall	BmpComputeY		;makes ES:DI-->start of dest scanline
	mov	dx, di			;save DI in DX
	add	di, wScreenCurrentX	;ES:DI-->where to put pixels
	mov	si, wStackTop
	dec	si
	dec	si			;SS:SI-->accumulated pixels
	shr	cx, 1			;sets carry if CX was odd
	jcxz	DB8DoRemainder
DB8PixelTransferLoop:
	std				;want to decrement SI
	lods	word ptr ss:[si]	;fetch 2 source pixels
	cld				;want to increment DI
	stosw				;save the two pixels
	loop	DB8PixelTransferLoop

DB8DoRemainder:
	jnc	DB8TransferDone
	lods	word ptr ss:[si]	;AL: last pixel to store
	stosb				;put it into the bitmap

DB8TransferDone:
	mov	di, dx			;restore initial DI
	dec	wScansLeft		;one more scan done in this segment
	jnz	DB8Exit 		;it wasn't the last scan

	cCall	PreviousSegment
DB8Exit:
cEnd

cProc	DumpToBmp4, <NEAR, PUBLIC>, <si>
cBegin
	cCall	VerifyDestAndCnt
	jc	DB4Exit
	cCall	BmpComputeY
	mov	si, wStackTop
	dec	si
	dec	si			;SS:SI-->accumulated pixels on stack
	push	di			;save offset to start of scanline
	mov	ax, wScreenCurrentX
	shr	ax, 1
	jnc	DB4EvenStart
	add	di, ax			;ES:DI-->1st byte of destination
	std				;decrement SI
	lods	word ptr ss:[si]	;AL: 1st pixel, AH: 2nd pixel
	mov	dl, es:[di]		;load existing pixels
	and	dl, 0f0h		;get rid of 2nd pixel in dest
	or	al, dl			;merge the two pixels
	cld				;increment DI
	stosb				;store the new pixel
	dec	cx			;account for the pixel just saved
	jcxz	DB4Done 		;if no pixels left, exit
	shr	cx, 1			;do 2 pixels at a time in main loop
	rcl	dh, 1			;if CX was odd, set D0 bit in DH
	jcxz	DB4OddDoRemainder

DB4OddPixelSaveLoop:
	shl	ah, 4			;this is the 1st pixel in next byte
	mov	dl, ah
	std				;decrement SI
	lods	word ptr ss:[si]	;AL: 1st pixel, AH: 2nd pixel
	or	al, dl			;merge the two pixels
	cld				;increment DI
	stosb				;store the new pixel
	loop	DB4OddPixelSaveLoop

DB4OddDoRemainder:
	test	dh, 01h 		;was count odd?
	jz	DB4Done 		;no.  We're done
	shl	ah, 4
	mov	al, es:[di]
	and	al, 0fh 		;keep 2nd pixel of destination
	or	al, ah
	stosb
	jmp	short DB4Done

DB4EvenStart:
	add	di, ax			;ES:DI-->1st byte of destination
	shr	cx, 1			;do 2 pixels at a time in main loop
	rcl	dh, 1			;if CX was odd, set D0 bit in DH
	jcxz	DB4EvenDoRemainder

DB4EvenPixelSaveLoop:
	std				;decrement SI
	lods	word ptr ss:[si]	;AL: 1st pixel, AH: 2nd pixel
	shl	al, 4			;put 1st pixel in high nibble
	or	al, ah			;pack both pixels into one byte
	cld				;increment DI
	stosb				;save the two pixels
	loop	DB4EvenPixelSaveLoop

DB4EvenDoRemainder:
	test	dh, 01h 		;was count odd?
	jz	DB4Done 		;no.  We're done
	std				;decrement SI
	lods	word ptr ss:[si]	;AL: 1st pixel, AH: 2nd pixel
	shl	al, 4			;put 1st pixel in high nibble
	mov	ah, es:[di]		;AH: current 2 dest. pixels
	and	ah, 0fh 		;keep the 2nd pixel in dest
	or	al, ah
	stosb

DB4Done:
	pop	di
	cld
	dec	wScansLeft		;one more scan done in this segment
	jnz	DB4Exit 		;it wasn't the last scan
	cCall	PreviousSegment
DB4Exit:
cEnd

MonoPhaseTable	label	byte
	db	80h
	db	40h
	db	20h
	db	10h
	db	08h
	db	04h
	db	02h
	db	01h

MonoMaskTable	label	byte
	db	0
	db	080h
	db	0c0h
	db	0e0h
	db	0f0h
	db	0f8h
	db	0fch
	db	0feh

MonoShiftTable	label	byte
	db	0
	db	7
	db	6
	db	5
	db	4
	db	3
	db	2
	db	1

cProc	DumpToBmp1, <NEAR, PUBLIC>, <si>
cBegin
	cCall	VerifyDestAndCnt	;make sure we're not clipped in Y
	jc	DB1Exit_10
	cCall	BmpComputeY		;get ES:DI-->start of current scanline
	push	di			;save offset to start of scanline
	mov	ax, wScreenCurrentX	;now compute byte offset and phase for
	mov	si, ax			; current X position, as well as first
	shr	ax, 3			; and last byte masks and shift count
	add	di, ax			;ES:DI-->1st byte of destination
	mov	ax, si
	and	si, 07h
	mov	dl, cs:[si].MonoPhaseTable
	mov	dh, cs:[si].MonoMaskTable
	mov	bFirstMask, dh
	add	ax, cx
	and	ax, 7
	mov	si, ax
	mov	al, cs:[si].MonoMaskTable
	not	al
	mov	bLastMask, al
	mov	al, cs:[si].MonoShiftTable
	mov	bLastShift, al
	mov	si, wStackTop
	dec	si
	dec	si			;SS:SI-->accumulated pixels
	sub	dh, dh			;accumulate pixels in DH
	std				;decrement SI
	cmp	dl, 080h		;are we already phase aligned?
	je	DB1MainLoop		;yes.  no need to mess with 1st byte

DB1FirstByteProc:
	lods	word ptr ss:[si]	;AX: first two pixels
	shr	al, 1			;put color in carry
	rcl	dh, 1			;accumulate pixels in DH
	ror	dl, 1			;update current phase
	jc	DB1SaveFirstByte1
	dec	cx			;update pixel count
	jcxz	DB1SpecialLastPixelProc
	shr	ah, 1			;color of 2nd pixel in carry
	rcl	dh, 1			;accumulate in DH
	ror	dl, 1			;update phase
	jc	DB1SaveFirstByte2
	loop	DB1FirstByteProc

DB1SpecialLastPixelProc:
	mov	al, es:[di]
	mov	ah, al
	and	al, bFirstMask
	and	ah, bLastMask
	mov	cl, bLastShift
	shl	dh, cl
	or	al, ah
	or	al, dh
	stosb
	jmp	short DB1Exit

DB1Exit_10:
	jmp	short DB1Done

DB1SaveFirstByte1:
	mov	al, es:[di]
	and	al, bFirstMask
	or	al, dh
	jmp	short DB1FirstPixelEntry

DB1SaveFirstByte2:
	mov	al, es:[di]
	and	al, bFirstMask
	or	al, dh
	jmp	short DB1SecondPixelEntry

DB1MainLoop:
	lods	word ptr ss:[si]	;next two pixels in AL and AH
	shr	al, 1			;put color in carry
	rcl	dh, 1			;accumulate pixels in DH
	ror	dl, 1			;update current phase
	jnc	DB1SecondPixel
	mov	al, dh
DB1FirstPixelEntry:
	cld				;increment DI
	stosb				;save pixels in AL
	std				;decrement SI
DB1SecondPixel:
	dec	cx			;update pixel count
	jcxz	DB1LastPixelProc	;was this the last pixel?
	shr	ah, 1
	rcl	dh, 1
	ror	dl, 1
	jc	DB1SavePixels
	loop	DB1MainLoop

DB1LastPixelProc:
	cmp	dl, 080h		;are there any pixels to save?
	je	DB1Exit 		;no.
	mov	ah, es:[di]
	and	ah, bLastMask
	mov	cl, bLastShift
	jcxz	DB1IgnoreLastPixels
	shl	dh, cl
	or	dh, ah			;merge with pixels already there
DB1IgnoreLastPixels:
	mov	al, dh
	stosb
	jmp	short DB1Exit

DB1SavePixels:
	mov	al, dh			;copy Pixels into AL
DB1SecondPixelEntry:
	cld				;increment DI
	stosb				;save pixels in AL
	std				;decrement SI
	loop	DB1MainLoop		;if CX becomes 0 now, no partial byte
DB1Exit:
	pop	di			;restore offset of start of scanline
	dec	wScansLeft		;account for one more scanline done
	jnz	DB1Done 		;if zero, bump down to prev selector
	cCall	PreviousSegment
DB1Done:
	cld				;count UP by default
cEnd

cProc	DumpToScreen, <NEAR, PUBLIC>, <si>
cBegin
	cCall	VerifyDestAndCnt	;first verify dest coordinates and cnt
	jc	DSExit			;if CY, nothing to draw
	CheckFIFOSpace	FIVE_WORDS	;don't take chances with that FIFO
	mov	dx, Srcx_PORT
	mov	ax, wScreenCurrentX
	out	dx, ax
	mov	dx, Srcy_PORT
	mov	ax, si
	out	dx, ax
	mov	dx, RECT_WIDTH_PORT
	mov	ax, cx
	dec	ax
	out	dx, ax
	mov	dx, COMMAND_FLAG_PORT
	mov	ax, 53b1h
	out	dx, ax
	mov	dx, PATTERN_DEFINE_PORT
	std				;count DOWN in mem str operation
	mov	si, wStackTop		;SS:SI-->pixels to send to board
	dec	si
	dec	si
	shr	cx, 1
	adc	cx, 0
rep	outs	dx, word ptr ss:[si]
	cld				;count UP again
DSExit:
cEnd

InitLocalXY proc near
	mov	ax, DestY		;get the position where we'll start to
	add	ax, ExtY		;draw. Start at bottom left hand corner
	dec	ax
	mov	wScreenCurrentY, ax	;we'll change that variable a lot
	mov	ax, DestX		;kept current for delta encoding
	mov	wScreenCurrentX, ax
	ret
InitLocalXY endp

;******************************Private*Routine*********************************
;This routine decodes an 8 bit/pixel RLE.  Translated pixels are accumulated
;on the stack.	The save routine specified in pfnDumpBuffer copies the
;pixels to the screen or to a bitmap.
;On Entry:  DS:SI-->RLEBits
;
;Calls:     pfnDumpBuffer to save accumulated pixels
;******************************************************************************

cProc	DecodeRLE8, <NEAR, PUBLIC>
cBegin
	lea	bx, bColorTable 	;now ss:bx --> DIB translate table
	mov	wStackTop, sp		;save current SP
DecodeRLEMainLoop:
	sub	cx, cx			;accumulate length of scan in CX
DRLEDataLoop:
	sub	ah, ah
	cCall	ReadRLE 		;AL: count or escape (0)
	or	al, al			;is it an escape?
	jz	DRLEMaybeEscape 	;yes, escape or absolute run

	mov	dh, al			;DH: run length
	cCall	ReadRLE 		;AL: color index
	xlatb	ss:[bx] 		;AL: index to use
	xchg	al, dh			;AX: count, DH: new color index
	test	cl, 1			;is there a leftover pixel in DL?
	jz	DRLEEvenCountStart	;no. we're all set to go
	push	dx			;save prev. pixel and new pixel
	inc	cx			;add this pixel to total count
	dec	ax			;mark this pixel as already saved
	jz	DRLEDataLoop		;if no pixel left to do, re-enter loop

DRLEEvenCountStart:
	mov	dl, dh			;copy pixel index into DL
	add	cx, ax			;update total pixel count
	shr	ax, 1			;determine # of WORDS to save
	jz	DRLEDataLoop		;nothing left to do here
	xchg	ax, cx
DRLESaveRunDataLoop:
	push	dx			;save pixels on stack
	loop	DRLESaveRunDataLoop
	xchg	ax, cx
	jmp	short DRLEDataLoop

DRLEMaybeEscape:
	cCall	ReadRLE 		;AL: type of escape
	cmp	al, 3			;is it an absolute run?
	jb	DRLEEscapeCode		;no.

	mov	bCurrentCount, al	;save current run length
	test	cl, 1			;leftover pixel in DL?
	jz	DRLEEvenAbsStart	;no.  We're ready to go
	mov	dh, al			;save count in DH
	cCall	ReadRLE 		;AL: next pixel
	xlatb	ss:[bx] 		;convert it
	xchg	al, dh			;AX: count, DH: new pixel
	push	dx			;save new and remaining prev. pixel
	inc	cx			;update total pixel count
	dec	ax			;update run length (left to do count)
DRLEEvenAbsStart:
	add	cx, ax			;update total pixel count
	mov	ah, al			;AH: loop count
DRLESaveAbsDataLoop:
	cCall	ReadRLE 		;AL: new pixel
	xlatb	ss:[bx] 		;convert it
	mov	dl, al
	dec	ah
	jz	DRLESaveAbsDataDone
	cCall	ReadRLE
	xlatb	ss:[bx]
	mov	dh, al
	push	dx
	dec	ah
	jnz	DRLESaveAbsDataLoop

DRLESaveAbsDataDone:
	test	bCurrentCount, 1	;are we WORD aligned?
	jz	DRLEDataLoop		;yes
	cCall	ReadRLE 		;force WORD alignment--read dummy byte
	jmp	short DRLEDataLoop

DRLEEscapeCode:
	jcxz	DRLEDoEscape		;count==0 --> no need to save anything
	test	cl, 1			;is there one more pixel to be pushed?
	jz	DRLEPixelsCollected
	push	dx			;save last pixel
DRLEPixelsCollected:
	push	ax			;save escape type
	push	cx			;save accumulated pixel count
	cCall	pfnDumpBuffer		;dump accumulated pixels to screen/bmp
	pop	cx
	pop	ax
	add	wScreenCurrentX, cx	;update current X position
	mov	sp, wStackTop		;restore stack ptr

DRLEDoEscape:
	or	al, al			;end of line?
	jz	DRLEEndOfLine
	cmp	al, 1			;end of bitmap?
	je	DRLEDone

	call	ReadRLE 		;get delta X (unsigned)
	add	wScreenCurrentX, ax	;update our X position
	call	ReadRLE 		;get delta Y (unsigned)
	sub	wScreenCurrentY, ax	;update Y. We're upside down because
	jmp	DecodeRLEMainLoop	;of @#$%! PM

DRLEEndOfLine:
	mov	ax, DestX		;kept current for delta encoding
	mov	wScreenCurrentX, ax
	dec	wScreenCurrentY 	;Update Y position as well!
	jmp	DecodeRLEMainLoop
DRLEDone:
cEnd


;******************************Private*Routine*********************************
;This routine decodes an 4 bit/pixel RLE.  Translated pixels are accumulated
;on the stack.	The save routine specified in pfnDumpBuffer copies the
;pixels to the screen or to a bitmap.
;On Entry:  DS:SI-->RLEBits
;
;Calls:     pfnDumpBuffer to save accumulated pixels
;******************************************************************************

cProc	DecodeRLE4, <NEAR, PUBLIC>
cBegin
	lea	bx, bColorTable 	;now ss:bx --> DIB translate table
	mov	wStackTop, sp		;save current SP
DecodeRLE4MainLoop:
	sub	cx, cx			;accumulate length of scan in CX
DRLE4DataLoop:
	sub	ah, ah
	cCall	ReadRLE 		;AL: count or escape (0)
	or	al, al			;is it an escape?
	jz	DRLE4MaybeEscape	;yes, escape or absolute run

	mov	dh, al			;DH: run length
	cCall	ReadRLE 		;AL: color indices
	mov	ah, al			;separate indices into AL and AH
	shr	ah, 4			;AH: first pixel
	and	al, 0fh 		;AL: second pixel
	xlatb	ss:[bx]
	xchg	al, ah
	xlatb	ss:[bx] 		;AL: first pixel, AH: second pixel
	test	cl, 1			;is there a leftover pixel in DL?
	jz	DRLE4EvenCountStart	;no. we're all set to go
	xchg	al, dh			;AL: count, DH: first pixel
	push	dx			;save prev. pixel and new pixel
	mov	dl, ah			;DL: second pixel
	sub	ah, ah			;clear AH
	inc	cx			;add this pixel to total count
	dec	ax			;mark this pixel as already saved
	jz	DRLE4DataLoop		;if no pixel left to do, re-enter loop
	jmp	short DRLE4SetupForLoop

DRLE4EvenCountStart:
	xchg	ax, dx			;DL 1st pixel, DH 2nd pixel
	mov	al, ah
	sub	ah, ah			;AX: run length

DRLE4SetupForLoop:
	add	cx, ax			;update total pixel count
	shr	ax, 1			;determine # of WORDS to save
	jz	DRLE4DataLoop		;nothing left to do here
	xchg	ax, cx
DRLE4SaveRunDataLoop:
	push	dx			;save pixels on stack
	loop	DRLE4SaveRunDataLoop
	xchg	ax, cx
	jmp	short DRLE4DataLoop

DRLE4MaybeEscape:
	cCall	ReadRLE 		;AL: type of escape
	cmp	al, 3			;is it an absolute run?
	jb	DRLE4EscapeCode 	;no.

	mov	bCurrentCount, al	;save current run length
	test	cl, 1			;leftover pixel in DL?
	jz	DRLE4EvenAbsStart	;no.  We're ready to go
	add	cx, ax			;update total pixel count
	mov	ah, al			;AH: current loop counter
	cCall	ReadRLE 		;AL: next pixel
	mov	dh, al			;separate indices into AL and DH
	shr	al, 4			;AL: first pixel
	and	dh, 0fh 		;DH: second pixel
	xlatb	ss:[bx]
	xchg	al, dh
	xlatb	ss:[bx] 		;DH: first pixel, AL: second pixel
	push	dx			;save new and remaining prev. pixel
	mov	dl, al			;now: most recent pixel in DL
	dec	ah			;update run length (left to do count)
	shr	ah, 1			;process pixels in sets of 2
DRLE4OddAbsLoop:
	cCall	ReadRLE 		;AL: next pixel
	mov	dh, al			;separate indices into AL and DH
	shr	al, 4			;AL: first pixel
	and	dh, 0fh 		;DH: second pixel
	xlatb	ss:[bx]
	xchg	al, dh
	xlatb	ss:[bx] 		;DH: first pixel, AL: second pixel
	push	dx			;save new and remaining prev. pixel
	mov	dl, al			;now: most recent pixel in DL
	dec	ah			;update run length (left to do count)
	jnz	DRLE4OddAbsLoop
	jmp	short DRLE4SaveAbsDataDone

DRLE4EvenAbsStart:			;CX is even at this point
	add	cx, ax			;update total pixel count
	shr	al, 1			;divide run length by 2, round up if odd
	adc	al, 0
	mov	ah, al			;AH: loop count
DRLE4EvenAbsLoop:
	cCall	ReadRLE 		;AL: new pixel
	mov	dh, al
	and	al, 0fh 		;AL: 2nd index
	xlatb	ss:[bx] 		;convert it
	shr	dh, 4			;DH: 1st index
	xchg	al, dh
	xlatb	ss:[bx] 		;AL: 1st pixel, DH: 2nd pixel
	mov	dl, al			;DH: 1st pixel
	push	dx
	dec	ah
	jnz	DRLE4EvenAbsLoop

	test	cl, 1			;if CX is now odd, remove last pixel
	jz	DRLE4SaveAbsDataDone	; from stack back into DL
	pop	dx

DRLE4SaveAbsDataDone:
	mov	al, bCurrentCount	;are we WORD aligned?
	shr	al, 1
	adc	al, 0
	test	al, 1
	jz	DRLE4DataLoop_10	;yes
	cCall	ReadRLE 		;force WORD alignment--read dummy byte
DRLE4DataLoop_10:
	jmp	DRLE4DataLoop

DRLE4EscapeCode:
	jcxz	DRLE4DoEscape		;count==0 --> no need to save anything
	test	cl, 1			;is there one more pixel to be pushed?
	jz	DRLE4PixelsCollected
	push	dx			;save last pixel
DRLE4PixelsCollected:
	push	ax			;save escape type
	push	cx			;save accumulated pixel count
	cCall	pfnDumpBuffer		;dump accumulated pixels to screen/bmp
	pop	cx
	pop	ax
	add	wScreenCurrentX, cx	;update current X position
	mov	sp, wStackTop		;restore stack ptr

DRLE4DoEscape:
	or	al, al			;end of line?
	jz	DRLE4EndOfLine
	cmp	al, 1			;end of bitmap?
	je	DRLE4Done

	call	ReadRLE 		;get delta X (unsigned)
	add	wScreenCurrentX, ax	;update our X position
	call	ReadRLE 		;get delta Y (unsigned)
	sub	wScreenCurrentY, ax	;update Y. We're upside down because
	jmp	DecodeRLE4MainLoop	;of @#$%! PM

DRLE4EndOfLine:
	mov	ax, DestX		;kept current for delta encoding
	mov	wScreenCurrentX, ax
	dec	wScreenCurrentY 	;Update Y position as well!
	jmp	DecodeRLE4MainLoop

DRLE4Done:
cEnd

;******************************Private*Routine*********************************
;PhysicalRLE is called when we need to write to the screen.  This routine does
; - Cursor exclusion
; - Setting/restoring the clipping rectangle
; - Set proper mode and rasterOP for 8514
; - Calls DecodeRLE to do the actual work
;On entry: DS= Data
;******************************************************************************
public	PhysicalRLE
PhysicalRLE proc near
	call	InitLocalXY
	cmp	seg_lpClipRect, 0	;we want to use the hardware scissors
	jz	NoClipRect		;of the 8514 to do the clipping for us
	mov	di, ds			;save DS=Data in DI
	lds	si, lpClipRect		;if we didn't get a clip region, set
	assumes ds, nothing		;default to full visible screen
CallClipProc:
	push	di			;save Data selector
	push	ss
	pop	es			;es=ss
	lea	di, LocalClipRect	;want to save clip rgn locally
	push	si			;save lpClipRect in DS:SI
	push	ds			;exclude the cursor from our region of
	lodsw				;interest
	push	ax
	stosw
	lodsw
	push	ax
	stosw
	lodsw
	push	ax
	stosw
	lodsw
	push	ax
	stosw
	cCall	CursorExclude		;go exclude the cursor
	assumes ds, Data		;DS=Data after last call
	pop	ds			;re-establish DS:SI --> lpClipRect
	pop	si
	call	SetScreenClipFar	;go set hardware scissors
	pop	es			;make es=data
	assumes es, Data
	jc	PhysicalRLEEnd		;if CY -> zero visible region -> exit
	CheckFIFOSpace FOUR_WORDS	;we'll need that much room in FIFO
	mov	al, 47h 		;set to "copy host data"
	mov	dx, FUNCTION_0_PORT	;now send the ROP to the board
	out	dx, ax			;this is the rop we want to use for
	mov	dx, FUNCTION_1_PORT	;the board operations
	out	dx, ax
	mov	ax, 0a080h		;set mode to normal mode of operation
	mov	dx, MODE_PORT		;for system to board transfers
	out	dx, ax
	sub	ax, ax			;make height=1 -- we're writing one
	out	dx, ax			;scanline at a time
	.errnz	RECT_HEIGHT_PORT-MODE_PORT
	mov	ax, DIBSegOFFSET DumpToScreen
	mov	pfnDumpBuffer, ax	;unload pixels to the screen
	lds	si, lpRLEBits		;DS:SI --> RLE bitmap
	mov	bx, wRLEtype
	mov	ax, cs:[bx].DecodeFunction
	call	ax			;go and do the work

	mov	ax, seg FullScreenClip	;before we exit, set hardware clip
	mov	ds, ax			;region to full-screen
	assumes ds, Data
	lea	si, FullScreenClip
PhysicalRLEEnd:
	call	SetScreenClipFar
	cCall	CursorUnExclude
	ret
NoClipRect:
	assumes ds, Data
	lea	si, FullScreenClip
	jmp	short CallClipProc
PhysicalRLE endp

;******************************************************************************
;******************************************************************************
;This are the routines needed to do 4 bit per pixel RLE encoding.
;
;******************************************************************************
;******************************************************************************

cProc	StoreCountAndIndex4, <NEAR, PUBLIC>
cBegin
	mov	al, cl
	call	pfnStorePixel1		;store run length
	mov	al, ch
	test	cl, 1			;are pixels in CH already alligned
	jnz	SCIPixelsAlligned
	ror	al, 4			;transpose pixels order
SCIPixelsAlligned:
	call	pfnStorePixel1		;store both color indices
cEnd

cProc	ReadPixel4, <NEAR, PUBLIC>, <bx>
cBegin
	xchg	si, dx			;get port address out of SI into DX
	in	ax, dx			;read one pixel
	xchg	si, dx			;restore registers SI and DX
	lea	bx, bColorTable
	and	al, bBitMask		;need this to work in 4 plane mode
	xlatb	ss:[bx] 		;GDI provides 256->16 color xlate table
cEnd

;******************************Private*Routine*********************************
;This is the 4 bit/pixel RLE encoding routine.
;On entry: es:di lpPDevice == Screen (will not be needed any more)
;No clipping rectangle is used for getting a RLE.  ROP2 is ignored
;******************************************************************************

cProc	EncodeRLE4, <NEAR, PUBLIC>
cBegin
	sub	bx, bx			;relative X position; initially = 0
	sub	cx, cx			;accumulate run length in cx

public	GP4MainLoop
GP4MainLoop:				;here we start a new run (CL=0)
	call	ReadPixel4		;get first index in new run
GPFetch2ndPixel:
	mov	ch, al			;store new pixel in low nibble of CH
	inc	bx			;update current X extent
	inc	cx			;update run length (now CL=1)
	cmp	bx, ExtX		;end of scanline?
	jae	GP4EndOfLine		;yes, go finish current scanline
	call	ReadPixel4		;get second pixel (-> high nibble)
	shl	al, 4			;put the second pixel in the high
	or	ch, al			;nibble of CH
	inc	bx			;update current X extent
	inc	cx			;update run length (now CL=2)
	cmp	bx, ExtX		;end of scanline?
	jae	GP4EndOfLine		;yes, go finish current scanline

public	GP4SubsequentPixel
GP4SubsequentPixel:
	call	ReadPixel4		;get subsequent pixel (low nibble AL)
	mov	dl, ch			;now: AL Nth pixel
	and	dl, 0fh 		;DL N-2nd pixel
	cmp	al, dl			;are both pixels of same color?
	jne	GP4Absolute		;no, may have to enter absolute mode

	ror	ch, 4			;transpose order of pixels
	inc	cx			;update run length
	inc	bx			;update X extent
	cmp	bx, ExtX
	jae	GP4EndOfLine		;be sure to test EOL BEFORE end of run
	cmp	cl, 0ffh		;CL >= 2 at this point if not GPNextRun
	jb	GP4SubsequentPixel	;do the next pixel

public	GP4RunComplete
GP4RunComplete:
	call	StoreCountAndIndex4	;pixel value in CH, rep-count in CL
	sub	cx, cx			;CL=0 reset run length
	jmp	short GP4MainLoop	;back into main loop for next run

public	GP4EndOfLine
GP4EndOfLine:
	cmp	cl, 1
	jne	GP4PixelsLinedUp
	ror	ch, 4
GP4PixelsLinedUp:
	call	StoreCountAndIndex4	;store this run's count and pixels
GP4EOL:
	call	DoNormalEOLStuff	;check end of bmp, reset BX
	sub	cx, cx			;reset run length
	call	StoreCountAndIndex4	;store EOL escape, init CX for next run
	or	dh, dh			;end of bitmap?
	jz	GP4MainLoop		;nope, keep going

public	GP4EndOfBitmap
GP4EndOfBitmap:
	call	pfnStorePixel1		;write end of bitmap escape, AL=0 here
	inc	ax
	call	pfnStorePixel1
	jmp	GP4Done 		;and exit! We're done

public	GP4Absolute
GP4Absolute:
	cmp	cl, 2			;only two pixels of this color?
	je	GP4EnterAbsolute	;then enter absolute mode

public	GP4StoreCurrentRun
GP4StoreCurrentRun:			;AL: newest pixel (low nibble)
	push	ax			;save latest color value
	cCall	StoreCountAndIndex4	;finish last run
	sub	cx, cx			;reset run length CL=0
	pop	ax			;restore current pixel
	jmp	short GPFetch2ndPixel

public	GPA4EndOfLineNode
GPA4EndOfLineNode:
	jmp	short GP4EndOfLine

public	GP4EnterAbsolute		;CL better be 2 at this point
GP4EnterAbsolute:			;CL=2, AL=3rd pixel, CH=Pixels 2, 1
	mov	dh, al			;save 3rd pixel in DH
	sub	al, al			;AL=0: 1st part of Abs mode escape
	call	pfnStorePixel1		;can always store that
	mov	lpPrevDest.off, di	;this is where we need to put the count
	mov	lpPrevDest.sel, es	;dummy store where abs count will go
	call	pfnStorePixel1		;now: ES:DI --> absolute indices

	mov	al, ch			;we can store first 2 pixels now
	ror	al, 4			;transpose order of pixels
	cCall	pfnStorePixel1		;store the first two pixels
	shl	al, 4
	or	al, dh			;AL: high: 2nd pixel, low: 3rd pixel
	mov	ch, al			;CH: high: 2nd pixel, low: 3rd pixel

public	GPA4AbsoluteModeLoop
GPA4AbsoluteModeLoop:			;count one pixel behind in this loop
	inc	cx			;update loop count CL >= 3 now
	inc	bx			;update X position
	cmp	bx, ExtX		;end of scanline?
	jae	GPA4EndOfLine		;then go finish this puppy off.
	cmp	cl, 0ffh		;have we hit the end of this run?
	je	GPA4RunComplete 	;yes, store what we have left
	cCall	ReadPixel4		;next pixel in al
	mov	ah, ch
	shr	ah, 4
	cmp	al, ah			;is the new pixel equal to the 2nd to
	je	GPAEndAbsolute		;last one? If so, exit abs. mode
GPA4ResumeAbsolute:
	mov	ah, al			;save latest pixel in AH
	test	cl, 1			;is it time to store the pixels?
	jnz	GPANoStore		;CL odd? --> yes, keep going
	mov	al, ch			;put the 2 pixels into AL
	cCall	pfnStorePixel1		;go store the pixels
GPANoStore:
	shl	ch, 4			;update pixel register
	or	ch, ah			;put latest pixel into CH
	jmp	short GPA4AbsoluteModeLoop

public	GPAEndAbsolute
GPAEndAbsolute:

;At his point: AL: new pixel==high nibble of CH.  This means the high nibble
;has the latest pixel and the low nibble the 2nd to latest pixel.  This
;format is directly compatible to what is expected at GPSubsequent pixel.
;CL, the absolute run length count, is up to date at this point.  So is BX,
;the current X position.  But because of the need to back patch, both are by two
;ahead of what is already stored as an absolute run.  If CL has to be greater
;than 4 to guaranty an absolute run of length 3 or greater.  Since the pixels
;are compared before anything is written into the output data stream, it is
;never necessary to modify the destination pointer.

	cmp	cl, 5			;is back-patched run at least length 3?
	jb	GPA4ResumeAbsolute	;no, keep going in abs mode loop
	sub	cl, 2			;back patch loop count
	cCall	pfnStorePixel2		;insert modified absolute run count
	inc	cx			;check whether the number of pixels
	shr	cl, 1			;stored needs to be word alligned
	mov	al, cl
	inc	ax
	and	al, 0feh
	cmp	al, cl
	je	GPA4WordAlligned
	cCall	pfnStorePixel1
GPA4WordAlligned:
	mov	cl, 3
	inc	bx			;update X position
	cmp	bx, ExtX		;are we at the end of a line?
	jae	GPA4EndOfLineNode	;do EOL stuff, back into GPMainLoop
	jmp	GP4SubsequentPixel	;enter normal run mode

public	GPA4EndOfLine
GPA4EndOfLine:

;If CL, the absolute run length, is even at this point, all but the last one
;or two pixels have been
;stored already.  If it is odd, the last pixel needs to be stored.  Lastly,
;the run length (in bytes) needs to be word alligned.

	cCall	pfnStorePixel2		;insert absolute count
	mov	al, ch
	test	cl, 1			;have to store last pixel.  count odd?
	jz	GPA4PixelStored 	;no.  Skip pixel allignment
	shl	al, 4			;only one pixel to store
GPA4PixelStored:
	cCall	pfnStorePixel1		;put it into the RLE bmp
	inc	cx			;now word allign the absolute run's
	shr	cl, 1			;length in bytes
	mov	al, cl
	inc	ax
	and	al, 0feh
	cmp	al, cl			;are we already word alligned?
	je	GP4EOLNode		;yes, we're don with this run
	cCall	pfnStorePixel1		;write a dummy pixel
GP4EOLNode:
	jmp	GP4EOL


public	GPA4RunComplete
GPA4RunComplete:

;0FFh pixels have been read from the source.  7Fh bytes have been written so
;far, and the last pixel is yet to be written, thus making the total number
;of bytes stored 80h, which is even.

	cCall	pfnStorePixel2		;insert absolute count
	mov	al, ch
	shl	al, 4
	cCall	pfnStorePixel1		;store last pixel
	sub	cx, cx
GP4MainLoopNode:
	jmp	GP4MainLoop		;now it's time to start a new run

public	GP4Done
GP4Done:
cEnd

sEnd	DIBSeg
end
