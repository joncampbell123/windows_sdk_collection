page	,132
title           Colour Memory StrBlt Routines
.286p

REALFIX 	equ	1

.xlist
include CMACROS.INC
include gdidefs.inc
include drvpal.inc
include 8514.inc
.list

externFP	BoardBltFar		;in BOARDBLT.ASM
externFP	SetScreenClipFar	;in ROUTINES.ASM
externA 	SMALLFONTLIMIT

sBegin          Data
externW     	FreeSpaceyOrg		;in Data.asm
externW     	FreeSpaceyExt		;in Data.asm
externB 	ShadowMemoryTrashed	;in SAVESCRN.ASM
externB 	PaletteFlags
externB 	BitsPixel		;in Data.asm
externB 	WriteEnable		;in Data.asm
externB 	BigCacheDisabled	;in textout.asm
externW 	FullScreenClip		;in data.asm
externW 	BigOldFaceNameLength	;in pstrblt.asm
sEnd            Data

sBegin	Code
assumes cs, Code

externW _cstods

cProc           StrBltDummy,<FAR>

include 	STRBLT.INC		;contains stack definitions

cBegin	<nogen>

;This routine exists so that we set up a stack frame which is correct for 
;our common StrBlt stack frame.  It's never called but allows us to make
;near calls to StrBlt routines.

cEnd	<nogen>

BltFunction label   word
	dw	FillRect4BitsPixel
	dw	FillRect8BitsPixel

cProc	DrawOpaqueRect, <FAR, PUBLIC>

cBegin	<nogen> 			;don't mess up the stack frame
	mov	cx, Options
	jcxz	DORDoneNode		;no opaquing rect??
	test	cl, 02h
	jnz	DORDrawOpaqueRect
DORDoneNode:
	jmp	short DORDone

DORDrawOpaqueRect:
	lds	si, lpDstDev
	assumes ds, nothing
	push	ss
	pop	es
	lea	di, MinimumClipRect
	sub	ax, ax
	stosw
	.errnz	left
	stosw
	.errnz	top-left-2
	mov	ax, [si].bmWidth
	stosw
	.errnz	right-top-2
	mov	ax, [si].bmHeight
	stosw
	.errnz	bottom-right-2
	sub	di, size RECT
	lds	si, lpOpaqueRect
	cCall	IntersectRect		;ES:DI-->Minimum clip rect
	jc	DORDoneNode		;if null intersect, exit now
	cmp	seg_lpClipRect, 0
	je	DORGetBltType
	lds	si, lpClipRect
	cCall	IntersectRect		;ES:DI-->Minimum clip rect
	jc	DORDoneNode
DORGetBltType:
	lds	si, lpDrawMode		;CL: background color out of draw mode
	mov	cl, [si][4]		;CL: byte ptr [si].bkColor
	mov	si, di			;SS:SI-->dest clip rect
	mov	ds, _cstods
	assumes ds, Data
	les	di, lpDstDev
	mov	al, [BitsPixel]
	cmp	al, es:[di].bmBitsPixel
	jne	DORDoneNode
	mov	ax, es:[di].bmSegmentIndex
	mov	SegmentIndex, ax
	mov	ax, es:[di].bmWidthBytes
	mov	WidthBytes, ax
	mov	bl, [WriteEnable]
	and	cl, bl			;strip off accelerator bytes if any
	mov	dl, bl
	not	dl
	mov	WritePlane, dl
	shr	bl, 3
	and	bl, 02h
	sub	bh, bh
	call	cs:BltFunction[bx]

DORDone:
	retf
cEnd	<nogen>

cProc	GetBitmapPointer, <NEAR, PUBLIC>
cBegin
	mov	bx, es:[di].bmScanSegment
	les	di,es:[di].bmBits
	mov	ax, MinimumClipRect.top
	cmp	ax, bx
	jl	short @f
	xor	dx,dx			;dx:ax / bx  Make sure dx is zero!
	div	bx			;ax = segment number
					;dx = desired scan in target segment.
	mov	bx,dx			;save remainder in bx.
	mul	SegmentIndex

	mov	dx,es
	add	dx,ax
	mov	es,dx			;es --> desired segment.
	mov	ax,bx
@@:	mul	WidthBytes
	add	di,ax			;es:di-->destination.
GBP_Exit:
cEnd

cProc	FillRect4BitsPixel, <NEAR, PUBLIC>

cBegin
	cCall	GetBitmapPointer
	push	word ptr ss:[0]		;save off temporarily.
	push	word ptr ss:[2]		;save off temporarily.
	mov	ax, MinimumClipRect.left
	shr	ax,1
	rcr	si,1			;put carry into cx.
	mov	ss:[0],ax		;Left offset in here.
	mov	dx,WidthBytes
	sub	dx,ax
	dec	dx
	mov	word ptr ss:[2],dx	;ss:[2] = (WidthBytes-left offset)-1
	rcl	si,1
	jnc	FR4ByteAlligned
	not	WritePlane
FR4ByteAlligned:
	add	di, ax			;ES:DI-->1st pixel to draw
	mov	al, cl			;put color index in low+high nibble
	shl	al, 4
	or	al, cl
	mov	ah, WritePlane		;phase mask in AH
	mov	cx, MinimumClipRect.bottom
	sub	cx, MinimumClipRect.top ;CX: inner loop count
	jcxz	FR4Done
	mov	si, MinimumClipRect.right
	sub	si, MinimumClipRect.left;SI: outer loop count
FR4Loop:
	xchg	cx, si
	jcxz	FR4Done
	push	cx
	mov	bx, di
	or	ah, ah
	js	FR4EvenStart
	mov	dh, al
	and	dh, ah
	not	ah
	mov	dl, es:[di]
	and	dl, ah
	not	ah
	or	dl, dh
	mov	es:[di], dl
	inc	di
	dec	cx
FR4EvenStart:
	shr	cx, 1
	jcxz	FR4DoRemainder
rep	stosb
FR4DoRemainder:
	jnc	FR4BottomOfLoop
	mov	dl, al
	and	dl, 0f0h
	mov	dh, es:[di]
	and	dh, 0fh
	or	dh, dl
	mov	es:[di], dh
FR4BottomOfLoop:
	pop	cx			;restore inner loop count
	xchg	cx, si
	add	bx, WidthBytes		;Is start pt. in the segment
	jc	FR4HugeInc		;no.
	add	bx, word ptr ss:[2]	;Is end of scan in the segment?
	jc	FR4HugeInc		;no.
	sub	bx, word ptr ss:[2]	;yes.
	mov	di, bx
	loop	FR4Loop
	jmp	short FR4Done

FR4HugeInc:
	dec	cx
	jcxz	FR4Done
	mov	di, es
	add	di, SegmentIndex
	mov	es, di
	mov	di,ss:[0]		;Make di left offset.
	jmp	short FR4Loop

FR4Done:
	pop	word ptr ss:[2]
	pop	word ptr ss:[0]
cEnd

cProc	FillRect8BitsPixel, <NEAR, PUBLIC>
cBegin
	cCall	GetBitmapPointer
	push	word ptr ss:[0]		;save off temporarily.
	push	word ptr ss:[2]		;save off temporarily.
	mov	ax,ss:[si].left		;save left offset for huge bitmap use.
	mov	ss:[0],ax		;Left offset in here.
	mov	dx,WidthBytes
	sub	dx,ax
	dec	dx
	mov	word ptr ss:[2],dx	;ss:[2] = (WidthBytes-left offset)-1
	add	di, ax
	mov	al, cl			;AL: color index to fill rect with
	mov	cx, ss:[si].bottom	;CX: outer loop count
	sub	cx, ss:[si].top
	jcxz	FR8Done
	mov	dx, ss:[si].right	;DX: inner loop count
	sub	dx, ss:[si].left
FR8Loop:
	xchg	cx, dx			;CX:inner loop count, DX: outer loop ct
	jcxz	FR8Done
	mov	si, cx
	mov	bx, di
rep	stosb
	mov	cx, si
	xchg	cx, dx			;CX: outer loop cnt, DX: inner loop cnt
	add	bx, WidthBytes		;Is start pt. in the segment
	jc	FR8HugeInc		;no.
	add	bx, word ptr ss:[2]	;Is end of scan in the segment?
	jc	FR8HugeInc		;no.
	sub	bx, word ptr ss:[2]	;yes.
	mov	di, bx
	loop	FR8Loop
	jmp	short FR8Done

FR8HugeInc:
	dec	cx
	jcxz	FR8Done
	mov	di, es
	add	di, SegmentIndex
	mov	es, di
	mov	di,ss:[0]		;Make di left offset.
	jmp	short FR8Loop

FR8Done:
	pop	word ptr ss:[2]
	pop	word ptr ss:[0]
cEnd

cProc	IntersectRect_far <FAR,PUBLIC>
cBegin	<nogen>
	call	IntersectRect
	ret
cEnd	<nogen>

cProc	IntersectRect, <NEAR, PUBLIC>

cBegin
	mov	ax, es:[di].left
	mov	bx, es:[di].right
	mov	cx, [si].left
	mov	dx, [si].right
	cCall	IntersectLines
	jc	IRDone
	mov	es:[di].left, cx
	mov	es:[di].right, dx

	mov	ax, es:[di].top
	mov	bx, es:[di].bottom
	mov	cx, [si].top
	mov	dx, [si].bottom
	cCall	IntersectLines
	jc	IRDone
	mov	es:[di].top, cx
	mov	es:[di].bottom, dx
IRDone:
cEnd

cProc	IntersectLines, <NEAR, PUBLIC>

cBegin
	cmp	ax, cx
	je	ILCheckHiLimit
	jl	ILCheckLoLimit
	cmp	ax, dx
	jge	ILErrorExit
	mov	cx, ax			;clip src rhs to dst rhs
ILCheckHiLimit:
	cmp	bx, dx
	jge	ILDone			;i.e., NC
	mov	dx, bx
	clc
	jmp	short ILDone

ILCheckLoLimit:
	cmp	bx, cx
	jle	ILErrorExit
	jmp	short ILCheckHiLimit

ILErrorExit:
	stc
ILDone:
cEnd

sEnd	Code

createSeg   _REAL, rCode, word, public, CODE
sBegin	    rCode
assumes     cs,rCode
assumes     ds,Data

externNP	RBoardStrBlt		;in STRBLT.ASM

cProc	RColourMemoryStrBlt, <NEAR, PUBLIC>

cBegin

;We take a very slow and complex strategy to perform this StrBlt.  This is
;because we hardly ever perform this routine (EXCEL drawing into a chart is
;the only place I've ever seen this thing needed).  The strategy that's used
;is the following:
;
;     1) If the BLT is to be transparent, call BoardBLT to BLT the destination
;        from main memory to the board's off screen workspace.
;
;     2) Play around with the clipping rectangle so that the string will be
;        clipped properly when BLTed to off-screen memory.
;
;     3) Call BoardStrBlt to BLT the string into invisible memory.
;
;     4) Call BoardBLT to read the string back into main memory.
;
;
;First, set up some common parameters for the two BoardBLTs:
	mov	SaveDS,ds
	cCall	DrawOpaqueRect		;need to draw opaque rect? Do it now!!
	and	Options,not 2		;Clear the Opaque Rect bit.
	mov	cx, Count		;CX: number of characters to process
	or	cx, cx			;We're done if there are no characters
	jnz	NeedToDoSomeWork	;to be drawn
	jmp	CMSExit
NeedToDoSomeWork:
	mov	BigCacheDisabled,1	;Turn off large font caching.
	mov	ShadowMemoryTrashed,0	;make sure save area is trashed
	mov	BigOldFaceNameLength,0	;make sure we trash the Big Font cache.
	mov	BoardDstDev,2000h	;this is so BoardBlt will BLT
					;onto the board
	les	di,lpFont		;get FONTINFO into ES:DI
	mov	bx,es:[di+22]		;get font height
	mov	FontHeight,bx		;save this for later

;-----------------------------------------------------------------------------
; Compute width of affect region of the bitmap.
;-----------------------------------------------------------------------------
	mov	ax,es:[di+27]		;get width of widest character
	inc	ax			;in the font
	inc	ax			;account for possible embolding
	mul	cx			;now AX has the worst-case
					;xExt of this BLT
	mov	xExt,ax 		;save this

public  CMSBAdjustCoordinates
CMSBAdjustCoordinates:
;-----------------------------------------------------------------------------
;It is unfortunately possible for StrBlts to have negative X & Y origins.
;Unfortunately, although our StrBlt code can handle this, BoardBLT cannot.
;Thus, we have to do input clipping on the origins.  We also must check to
;make sure our calculated extents do not exceed the size of the bitmap.  If
;they do, just use the ones from the BITMAP data structure:
;-----------------------------------------------------------------------------
	les	di,lpDstDev		;get the BITMAP data structure
	mov	bx,es:[di+2]		;get the size of the bitmap
	sub	bx,DstxOrg		;correct it for xOrg <> 0
	jle	CMSBACExit		;if negative or 0, there's
					;nothing to draw
	cmp	ax,bx			;do we run off the right side
					;of the bitmap?
	jb	CMSBAC1 		;nope, we're OK
	mov	xExt,bx 		;otherwise, replace our xExt

CMSBAC1:
	mov	ax,FontHeight
	mov	bx,es:[di+4]		;get the size of the bitmap
	sub	bx,DstyOrg		;correct it for yOrg <> 0
	jle	CMSBACExit		;if negative or 0, there's
					;nothing to draw
	cmp	ax,bx			;do we run off the bottom of
					;the bitmap?
	jb	CMSBAC2 		;nope, we're OK
	mov	FontHeight,bx		;otherwise, replace our yExt

CMSBAC2:
	xor	ax,ax			;AX will be our "input clip"
	mov	di,DstxOrg		;get our xOrg
	or	di,di			;is it negative?
	jns	CMSBAC3 		;nope, we're OK
	sub	ax,di			;now AX has the amount to clip
	xor	di,di			;and clip DI off to 0

CMSBAC3:
	sub	xExt,ax 		;subtract off amount clipped
	jle	CMSBACExit		;if negative, or zero there's
					;nothing to do
	mov	CMSDstxOrg,di
	xor	ax,ax			;AX will be our "input clip"
	mov	di,DstyOrg		;get our yOrg
	or	di,di			;is it negative?
	jns	CMSBAC4 		;nope, we're OK
	sub	ax,di			;now AX has the amount to clip
	xor	di,di			;and clip DI off to 0

CMSBAC4:
	sub	FontHeight,ax		;subtract off amount clipped
	jle	CMSBACExit		;if negative or zero, there's
					;nothing to do!
	mov	CMSDstyOrg,di
	jmp	short CMSBBLTDownDest	;continue

public  CMSBACExit
CMSBACExit:
	jmp	CMSExit 		;get on out

;-----------------------------------------------------------------------------
;Now, the coordinate (CMSDstxOrg,CMSDsstyOrg) is the upper left corner of
;the affected portion of the bitmap.  xExt is the width and FontHeight
;is the height of this region.  We must copy this region into offscreen
;memory, blt the string onto it, and then copy it back.  
;Unfortunately, it is possible that this region might be larger than 
;available offscreen memory.  In this case, we have to implement banding
;in x and y (what a pain!).
;-----------------------------------------------------------------------------
public  CMSBBLTDownDest
CMSBBLTDownDest:

;-----------------------------------------------------------------------------
;Get the y coordinate of the place in off screen memory that we are
;going to blt to. 
;-----------------------------------------------------------------------------
	mov	ax,FontHeight
	mov	TotalY,ax
	mov	dx,CMSDstyOrg
	mov	StartY,di

	mov	ax,1024			;1023 - (FontHeight-1)
	sub	ax,TotalY
	cmp	ax,Y_SIZE+64
	jge	short @f
	mov	ax,Y_SIZE+64
@@:	mov	StrColouryCoordinate,ax

public	yBandLoop
yBandLoop:
	mov	ax,TotalY
	cmp	ax,192
	jle	short @f
	mov	ax,192
@@:	mov	yBand,ax
	mov	ax,CMSDstxOrg
	mov	StartX,ax
	mov	bx,xExt
	mov	TotalX,bx

public	xBandLoop
xBandLoop:
	mov	ax,TotalX
	cmp	ax,X_SIZE
	jle	short @f
	mov	ax,X_SIZE
@@:	mov	xBand,ax


;-----------------------------------------------------------------------------
;Now call BoardBLT to get the destination into off-screen memory:
;-----------------------------------------------------------------------------
	lea	di,ss:BoardDstDev		;get the address as our
						;fake lpDstDev for the BLT
	mov	ax,StrColouryCoordinate 	;make FontHeight an extent
	and	PaletteFlags, (not NOMEMSTRBLT) ; signal `no color translation'
	arg	ss				;this is lpDstDev
	arg	di
	arg	0				;this is DstxOrg
	arg	StrColouryCoordinate		;this is DstyOrg
	arg	lpDstDev			;this is lpSrcDev
	arg	StartX				;this is SrcxOrg
	arg	StartY				;this is SrcyOrg
	arg	xBand				;this is xExt
	arg	yBand				;this is yExt
	arg	0cch				;this is Rop3 for SrcCopy
	arg	20h
	arg	0				;send down a dummy for lpPBrush
	arg	0
	arg	lpDrawMode			;this is lpDrawMode
	cCall	<far ptr BoardBltFar>		;get the destination onto the
                                                ;off-screen area of the board

;-----------------------------------------------------------------------------
;Now, we must take the original clipping rectangle passed
;to us and create a new clipping rectangle which will allow us to draw the
;correctly clipped string into invisible memory.  Since each band has
;been 'translated' to offscreen memory, we first compute this translation
;amount and then apply it to the clip rectangle.  We then intersect this
;new clip rectangle with a minimum clip rectangle that is the dimensions
;of the offscreen region.  We use the following formulae:
;	xDelta = StartX - 0
;	yDelta = StartY - StrColouryCoordinate
;
;	NewClip.left   = OriginalClip.left - xDelta
;	NewClip.right  = OriginalClip.right - xDelta
;	NewClip.top    = OriginalClip.top - yDelta
;	NewClip.bottom = OriginalClip.bottom - yDelta
;
;	BaseClip.left = 0;                    BaseClip.right  = X_SIZE
;	BaseClip.top  = StrColouryCoordinate;  BaseClip.bottom = 1023
;
;	IntersectRect(BaseClip,NewClip)	;Results in BaseClip.
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Compute x and y Deltas.
;-----------------------------------------------------------------------------
public CMSBComputeDeltas
CMSBComputeDeltas:
	mov	ax,StartX
	mov	xDelta,ax			
	mov	ax,StartY
	sub	ax,StrColouryCoordinate
	mov	yDelta,ax

;-----------------------------------------------------------------------------
; First, make a clip rect that is the dimensions of the off-screen area.
;-----------------------------------------------------------------------------
public CMSBMakeBaseRect
CMSBMakeBaseRect:
	push	ss
	pop	ds
	assumes	ds,nothing
	lea	si,BaseRect			;ds:si-->Base Rect.
	sub	ax,ax
	mov	[si],ax
	.errnz	left
	mov	ax,StrColouryCoordinate
	mov	[si+2],ax
	.errnz	top-left-2
	mov	ax,X_SIZE
	mov	[si+4],ax	
	.errnz	right-top-2
	mov	ax,1024
	mov	[si+6],ax
	.errnz	bottom-right-2

;-----------------------------------------------------------------------------
; If there is no clip rect, then force the new rect to be the clip rect.
;-----------------------------------------------------------------------------
public CMSBGetClipRect
CMSBGetClipRect:
	cmp	seg_lpClipRect,0	;any clipping rectangle?
	jne	short @f
	mov	seg_lpClipRect,ds
	mov	off_lpClipRect,si
@@:	les	di,lpClipRect		;es:di-->ClipRect
	mov	ax,es:[di]		;get the starting X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di],ax		;Set left
	mov	ax,es:[di+2]		;get the starting Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+2],ax		;Set top
	mov	ax,es:[di+4]		;get the ending X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di+4],ax		;Set right
	mov	ax,es:[di+6]		;get the ending Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+6],ax		;Set bottom
	cCall	IntersectRect_far	;ES:DI-->Modified Clip Rect

public CMSBGetOpaqueingRect
CMSBGetOpaqueingRect:
	cmp	seg_lpOpaqueRect,0
	je	CMSBDoStrBLT

	les	di,lpOpaqueRect		;es:di-->Opaque Rect
	mov	ax,es:[di]		;get the starting X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di],ax		;Set left
	mov	ax,es:[di+2]		;get the starting Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+2],ax		;Set top
	mov	ax,es:[di+4]		;get the ending X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di+4],ax		;Set right
	mov	ax,es:[di+6]		;get the ending Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+6],ax		;Set bottom
	cCall	IntersectRect_far	;ES:DI-->Modified Opaque Rect

public  CMSBDoStrBLT
CMSBDoStrBLT:
;-----------------------------------------------------------------------------
;Now we must adjust the the x,y coordinates of where we want to draw the
;string.  This is done as follows:
; DstxOrg -= xDelta
; DstyOrg -= yDelta
;-----------------------------------------------------------------------------
	mov	ds,SaveDS
	assumes	ds,Data
	mov	ax,DstxOrg
	push	ax
	sub	ax,xDelta
	mov	DstxOrg,ax

	mov	ax,DstyOrg
	push	ax
	sub	ax,yDelta
	mov	DstyOrg,ax

	push	xExt			;these may be destroyed by call
	push	FontHeight
	push	off_lpCharWidths

	cCall	RBoardStrBlt		;go do the StrBlt to invisible
	pop	off_lpCharWidths
	pop	FontHeight		;get back correct sizes for
	pop	xExt			;the bitmap readback

	pop	DstyOrg 		;get back original DstyOrg
	pop	DstxOrg 		;get back original DstxOrg

public	CMSBRestoreOpaqueRect
CMSBRestoreOpaqueRect:
	cmp	seg_lpOpaqueRect,0	;any lpOpaqueRect?
	je	CMSBRestoreClipRect	;nope, continue
	les	di,lpOpaqueRect		;reload the opaque rectangle
	pop	es:[di+6]		;get back the original rect
	pop	es:[di+4]
	pop	es:[di+2]
	pop	es:[di]

public	CMSBRestoreClipRect
CMSBRestoreClipRect:
	cmp	seg_lpClipRect,0	;any lpClipRect?
	je	CMSBReadBack		;nope, continue
	les	di,lpClipRect		;reload the clip rectangle
	pop	es:[di+6]		;get back the original rect
	pop	es:[di+4]
	pop	es:[di+2]
	pop	es:[di]


public  CMSBReadBack
CMSBReadBack:
;-----------------------------------------------------------------------------
;Lastly, call BoardBLT to read the string back into the main memory bitmap.
;-----------------------------------------------------------------------------
	mov	ds,SaveDS
	lea	di,ss:BoardDstDev		;make a pointer to the phony
						;board PDevice structure
	and	PaletteFlags, (not NOMEMSTRBLT) ; signal `no color translation'
	arg	lpDstDev
	arg	StartX
	arg	StartY
	arg	ss				;this is lpSrcDev
	arg	di
	arg	0				;this is SrcxOrg
	arg	StrColouryCoordinate		;this is SrcyOrg
	arg	xBand				;this is xExt
	arg	yBand				;this is yExt
	arg	0cch				;this is Rop2 code for SrcCopy
	arg	20h
	arg	0				;send down a dummy lpPBrush
	arg	0
	arg	lpDrawMode
	cCall	<far ptr BoardBltFar>

public	xBandLoopBottom
xBandLoopBottom:
	mov	ax,xBand
	sub	TotalX,ax
	jz	short yBandLoopBottom
	add	StartX,ax
	mov	ds,SaveDS
	jmp	xBandLoop

yBandLoopBottom:
	mov	ax,yBand
	sub	TotalY,ax
	jz	short CMSExit
	add	StartY,ax
	mov	ds,SaveDS
	jmp	yBandLoop
	
public  CMSExit
CMSExit:
	mov	ds,SaveDS
	mov	BigCacheDisabled,0	;Turn large font caching back on.
cEnd

sEnd	rCode

if REALFIX
createSeg   _PROTECT, pCode, word, public, CODE
sBegin	    pCode

assumes cs, pCode
assumes ds, Data
assumes es, nothing

externNP    PBoardStrBlt

cProc	PColourMemoryStrBlt, <NEAR, PUBLIC>
cBegin

;-----------------------------------------------------------------------------
;We take a very slow and complex strategy to perform this StrBlt.  This is
;because we hardly ever perform this routine (EXCEL drawing into a chart is
;the only place I've ever seen this thing needed).  The strategy that's used
;is the following:
;
;     1) If the BLT is to be transparent, call BoardBLT to BLT the destination
;        from main memory to the board's off screen workspace.
;
;     2) Play around with the clipping rectangle so that the string will be
;        clipped properly when BLTed to off-screen memory.
;
;     3) Call BoardStrBlt to BLT the string into invisible memory.
;
;     4) Call BoardBLT to read the string back into main memory.
;
;First, set up some common parameters for the two BoardBLTs:
;-----------------------------------------------------------------------------
	mov	SaveDS,ds
	cCall	DrawOpaqueRect		;need to draw opaque rect? Do it now!!
	and	Options, not 2		;Show Opaque rect has already been done.
	mov	cx, Count		;CX: number of characters to process
	or	cx, cx			;We're done if there are no characters
	jnz	PNeedToDoSomeWork	;to be drawn
	jmp	PCMSExit
PNeedToDoSomeWork:
	mov	BigCacheDisabled,1	;Turn off large font caching.
	mov	ShadowMemoryTrashed, 0	;invalidate save area and huge font cache.
	mov	BigOldFaceNameLength,0	;make sure we trash the Big Font cache.
	mov	BoardDstDev,2000h	;this is so BoardBlt will BLT
					;onto the board
	les	di,lpFont		;get FONTINFO into ES:DI
	mov	bx,es:[di+22]		;get font height
	mov	FontHeight,bx		;save this for later

;-----------------------------------------------------------------------------
; Compute width of affect region of the bitmap.
;-----------------------------------------------------------------------------
	mov	ax,es:[di+27]		;get width of widest character
	inc	ax			;in the font
	inc	ax			;account for possible embolding
	mul	cx			;now AX has the worst-case
					;xExt of this BLT
	mov	xExt,ax 		;save this

public	PCMSBAdjustCoordinates
PCMSBAdjustCoordinates:
;-----------------------------------------------------------------------------
;It is unfortunately possible for StrBlts to have negative X & Y origins.
;Unfortunately, although our StrBlt code can handle this, BoardBLT cannot.
;Thus, we have to do input clipping on the origins.  We also must check to
;make sure our calculated extents do not exceed the size of the bitmap.  If
;they do, just use the ones from the BITMAP data structure:
;-----------------------------------------------------------------------------
	les	di,lpDstDev		;get the BITMAP data structure
	mov	bx,es:[di+2]		;get the size of the bitmap
	sub	bx,DstxOrg		;correct it for xOrg <> 0
	jle	PCMSBACExit		;if negative or 0, there's
					;nothing to draw
	cmp	ax,bx			;do we run off the right side
					;of the bitmap?
	jb	PCMSBAC1		;nope, we're OK
	mov	xExt,bx 		;otherwise, replace our xExt

PCMSBAC1:
	mov	ax,FontHeight
	mov	bx,es:[di+4]		;get the size of the bitmap
	sub	bx,DstyOrg		;correct it for yOrg <> 0
	jle	PCMSBACExit		;if negative or 0, there's
					;nothing to draw
	cmp	ax,bx			;do we run off the bottom of
					;the bitmap?
	jb	PCMSBAC2		;nope, we're OK
	mov	FontHeight,bx		;otherwise, replace our yExt

PCMSBAC2:
	xor	ax,ax			;AX will be our "input clip"
	mov	di,DstxOrg		;get our xOrg
	or	di,di			;is it negative?
	jns	PCMSBAC3		;nope, we're OK
	sub	ax,di			;now AX has the amount to clip
	xor	di,di			;and clip DI off to 0

PCMSBAC3:
	sub	xExt,ax 		;subtract off amount clipped
	jle	PCMSBACExit		;if negative, or zero there's
	mov	CMSDstxOrg,di		;nothing to do
	xor	ax,ax			;AX will be our "input clip"
	mov	di,DstyOrg		;get our yOrg
	or	di,di			;is it negative?
	jns	PCMSBAC4		;nope, we're OK
	sub	ax,di			;now AX has the amount to clip
	xor	di,di			;and clip DI off to 0
PCMSBAC4:
	sub	FontHeight,ax		;subtract off amount clipped
	jle	PCMSBACExit		;if negative or zero, there's
	mov	CMSDstyOrg,di		;nothing to do!
	jmp	short PCMSBBLTDownDest	;continue

public	PCMSBACExit
PCMSBACExit:
	jmp	PCMSExit		;get on out

;-----------------------------------------------------------------------------
;Now, the coordinate (CMSDstxOrg,CMSDsstyOrg) is the upper left corner of
;the affected portion of the bitmap.  xExt is the width and FontHeight
;is the height of this region.  We must copy this region into offscreen
;memory, blt the string onto it, and then copy it back.  
;Unfortunately, it is possible that this region might be larger than 
;available offscreen memory.  In this case, we have to implement banding
;in x and y (what a pain!).
;-----------------------------------------------------------------------------
public	PCMSBBLTDownDest
PCMSBBLTDownDest:

;-----------------------------------------------------------------------------
;Get the y coordinate of the place in off screen memory that we are
;going to blt to. 
;-----------------------------------------------------------------------------
	mov	ax,FontHeight
	mov	TotalY,ax
	mov	dx,CMSDstyOrg
	mov	StartY,di

	mov	ax,1024			;1023 - (FontHeight-1)
	sub	ax,TotalY
	cmp	ax,Y_SIZE+64
	jge	short @f
	mov	ax,Y_SIZE+64
@@:	mov	StrColouryCoordinate,ax

public	PyBandLoop
PyBandLoop:
	mov	ax,TotalY
	cmp	ax,192
	jle	short @f
	mov	ax,192
@@:	mov	yBand,ax
	mov	ax,CMSDstxOrg
	mov	StartX,ax
	mov	bx,xExt
	mov	TotalX,bx

public	PxBandLoop
PxBandLoop:
	mov	ax,TotalX
	cmp	ax,X_SIZE
	jle	short @f
	mov	ax,X_SIZE
@@:	mov	xBand,ax

;-----------------------------------------------------------------------------
;Now call BoardBLT to get the destination into off-screen memory:
;-----------------------------------------------------------------------------
	lea	di,ss:BoardDstDev		;get the address as our
						;fake lpDstDev for the BLT
	mov	ax,StrColouryCoordinate 	;make FontHeight an extent
	and	PaletteFlags, (not NOMEMSTRBLT) ; signal `no color translation'

	arg	ss				;this is lpDstDev
	arg	di
	arg	0				;this is DstxOrg
	arg	StrColouryCoordinate		;this is DstyOrg
	arg	lpDstDev			;this is lpSrcDev

	arg	StartX				;this is SrcxOrg
	arg	StartY				;this is SrcyOrg
	arg	xBand				;this is xExt
	arg	yBand				;this is yExt

	arg	0cch				;this is Rop3 for SrcCopy
	arg	20h
	arg	0				;send down a dummy for lpPBrush
	arg	0
	arg	lpDrawMode			;this is lpDrawMode
	cCall	<far ptr BoardBltFar>		;get the destination onto the
                                                ;off-screen area of the board
;-----------------------------------------------------------------------------
;Now, we must take the original clipping rectangle passed
;to us and create a new clipping rectangle which will allow us to draw the
;correctly clipped string into invisible memory.  Since each band has
;been 'translated' to offscreen memory, we first compute this translation
;amount and then apply it to the clip rectangle.  We then intersect this
;new clip rectangle with a minimum clip rectangle that is the dimensions
;of the offscreen region.  We use the following formulae:
;	xDelta = StartX - 0
;	yDelta = StartY - StrColouryCoordinate
;
;	NewClip.left   = OriginalClip.left - xDelta
;	NewClip.right  = OriginalClip.right - xDelta
;	NewClip.top    = OriginalClip.top - yDelta
;	NewClip.bottom = OriginalClip.bottom - yDelta
;
;	BaseClip.left = 0;                    BaseClip.right  = X_SIZE
;	BaseClip.top  = StrColouryCoordinate;  BaseClip.bottom = 1023
;
;	IntersectRect(BaseClip,NewClip)	;Results in BaseClip.
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
; Compute x and y Deltas.
;-----------------------------------------------------------------------------
public PCMSBComputeDeltas
PCMSBComputeDeltas:
	mov	ax,StartX
	mov	xDelta,ax			
	mov	ax,StartY
	sub	ax,StrColouryCoordinate
	mov	yDelta,ax

;-----------------------------------------------------------------------------
; First, make a clip rect that is the dimensions of the off-screen area.
;-----------------------------------------------------------------------------
public PCMSBMakeBaseRect
PCMSBMakeBaseRect:
	push	ss
	pop	ds
	assumes	ds,nothing
	lea	si,BaseRect			;ds:si-->Base Rect.
	sub	ax,ax
	mov	[si],ax
	.errnz	left
	mov	ax,StrColouryCoordinate
	mov	[si+2],ax
	.errnz	top-left-2
	mov	ax,X_SIZE
	mov	[si+4],ax	
	.errnz	right-top-2
	mov	ax,1024
	mov	[si+6],ax
	.errnz	bottom-right-2

;-----------------------------------------------------------------------------
; If there is no clip rect, then force the new rect to be the clip rect.
;-----------------------------------------------------------------------------
public PCMSBGetClipRect
PCMSBGetClipRect:
	cmp	seg_lpClipRect,0	;any clipping rectangle?
	jne	short @f
	mov	seg_lpClipRect,ds
	mov	off_lpClipRect,si
@@:	les	di,lpClipRect		;es:di-->ClipRect
	mov	ax,es:[di]		;get the starting X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di],ax		;Set left
	mov	ax,es:[di+2]		;get the starting Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+2],ax		;Set top
	mov	ax,es:[di+4]		;get the ending X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di+4],ax		;Set right
	mov	ax,es:[di+6]		;get the ending Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+6],ax		;Set bottom
	cCall	IntersectRect_far	;ES:DI-->Modified Clip Rect

public PCMSBGetOpaqueingRect
PCMSBGetOpaqueingRect:
	cmp	seg_lpOpaqueRect,0
	je	PCMSBDoStrBLT

	les	di,lpOpaqueRect		;es:di-->Opaque Rect
	mov	ax,es:[di]		;get the starting X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di],ax		;Set left
	mov	ax,es:[di+2]		;get the starting Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+2],ax		;Set top
	mov	ax,es:[di+4]		;get the ending X
	push	ax			;save it.
	sub	ax,xDelta
	mov	es:[di+4],ax		;Set right
	mov	ax,es:[di+6]		;get the ending Y
	push	ax			;save it.
	sub	ax,yDelta
	mov	es:[di+6],ax		;Set bottom
	cCall	IntersectRect_far	;ES:DI-->Modified Opaque Rect

public	PCMSBDoStrBLT
PCMSBDoStrBLT:
;-----------------------------------------------------------------------------
;Now we must adjust the the x,y coordinates of where we want to draw the
;string.  This is done as follows:
; DstxOrg -= xDelta
; DstyOrg -= yDelta
;-----------------------------------------------------------------------------
	mov	ds,SaveDS
	assumes	ds,Data

	mov	ax,DstxOrg
	push	ax
	sub	ax,xDelta
	mov	DstxOrg,ax

	mov	ax,DstyOrg
	push	ax
	sub	ax,yDelta
	mov	DstyOrg,ax

	push	xExt			;these may be destroyed by call
	push	FontHeight
	push	off_lpCharWidths
	push	Count
	cCall	PBoardStrBlt		;go do the StrBlt to offscreen mem.
	pop	Count
	pop	off_lpCharWidths
	pop	FontHeight		;get back correct sizes for
	pop	xExt			;the bitmap readback

	pop	DstyOrg 		;get back original DstyOrg
	pop	DstxOrg 		;get back original DstxOrg

public	PCMSBRestoreOpaqueRect
PCMSBRestoreOpaqueRect:
	cmp	seg_lpOpaqueRect,0	;any lpOpaqueRect?
	je	PCMSBRestoreClipRect	;nope, continue
	les	di,lpOpaqueRect		;reload the opaque rectangle
	pop	es:[di+6]		;get back the original rect
	pop	es:[di+4]
	pop	es:[di+2]
	pop	es:[di]

public	PCMSBRestoreClipRect
PCMSBRestoreClipRect:
	cmp	seg_lpClipRect,0	;any lpClipRect?
	je	PCMSBReadBack		;nope, continue
	les	di,lpClipRect		;reload the clip rectangle
	pop	es:[di+6]		;get back the original rect
	pop	es:[di+4]
	pop	es:[di+2]
	pop	es:[di]

public	PCMSBReadBack
PCMSBReadBack:
;-----------------------------------------------------------------------------
;Lastly, call BoardBLT to read the string back into the main memory bitmap.
;-----------------------------------------------------------------------------
	mov	ds,SaveDS
	lea	di,ss:BoardDstDev		;make a pointer to the phony
						;board PDevice structure
	and	PaletteFlags, (not NOMEMSTRBLT) ; signal `no color translation'

	arg	lpDstDev
	arg	StartX
	arg	StartY
	arg	ss				;this is lpSrcDev
	arg	di
	arg	0				;this is SrcxOrg
	arg	StrColouryCoordinate		;this is SrcyOrg
	arg	xBand				;this is xExt
	arg	yBand				;this is yExt
	arg	0cch				;this is Rop2 code for SrcCopy
	arg	20h
	arg	0				;send down a dummy lpPBrush
	arg	0
	arg	lpDrawMode
	cCall	<far ptr BoardBltFar>

public	PxBandLoopBottom
PxBandLoopBottom:
	mov	ax,xBand
	sub	TotalX,ax
	jz	short PyBandLoopBottom
	add	StartX,ax
	mov	ds,SaveDS
	jmp	PxBandLoop

PyBandLoopBottom:
	mov	ax,yBand
	sub	TotalY,ax
	jz	short PCMSExit
	add	StartY,ax
	mov	ds,SaveDS
	jmp	PyBandLoop
	
public	PCMSExit
PCMSExit:
	mov	ds,SaveDS
	mov	BigCacheDisabled,0	;Turn large font caching back on.
cEnd
sEnd	    pCode
endif
end
