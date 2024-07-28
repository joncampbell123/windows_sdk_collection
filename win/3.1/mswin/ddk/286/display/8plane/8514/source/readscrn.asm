page            ,132
title           Read Screen Routines For the IBM 8514
.286c


.xlist
include CMACROS.INC
include 8514.INC
include gdidefs.inc
include 8514port.inc
.list

READCOMMAND	equ FILL_X_RECT or PC_TRANS or YMAJAXIS or INCX or INCY or DRAWCMD
DATA_AVAILABLE	equ 1

sBegin		Data
;externW ProtectTable
externD ReadScreenColor
externB WriteEnable
sEnd            Data

subttl          Code Area
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data

externA     __AHIncr			;imported from kernel for protm support
externFP    AllocCSToDSAlias		;imported from kernel for protect mode
externNP    GetHugeBitmapParams 	;in BLKWRI.ASM

cProc	ReadScreen,<NEAR,PUBLIC>

        include BOARDBLT.INC            ;frame definitions

cBegin          <nogen>

;Set the Y-extent before we split into the various routines

	mov	ax, word ptr ReadScreenColor
	mov	dx, word ptr ReadScreenColor+2
	mov	off_ReadColor, ax	;in case we'll do a color blt
	mov	seg_ReadColor, dx

	CheckFIFOSpace	SEVEN_WORDS
	xor	ax,ax			;extent in board terms is always 1
	mov	dx,RECT_HEIGHT_PORT
	out	dx,ax

;Now, determine if we have a huge bitmap and call the procedure which sets
;up the variables and counters necesary to handle huge bitmap processing.

	mov	ax,DstyOrg		;initialise the starting Y
	push	ds			;save our DS for now
	lds	si,lpDstDev		;get the destination PDEVICE
	xor	bx,bx			;assume we have a small bitmap
	xor	dx,dx			;and init these return values
	mov	cx,[si].bmSegmentIndex	;get the segment-to-segment adder
	mov	wSegmentIndex, cx
	jcxz	RSSaveBitmapParams	;if 0, we have a small bitmap
	cCall	GetHugeBitmapParams	;otherwise, set up for a huge bitmap

public  RSSaveBitmapParams
RSSaveBitmapParams:

;Now, get the bitmap colour format (either 4 bits per pixel = colour
;or 1 bit per pixel = monochrome) and decide which output procedure to use:

	mov	StartingScanline,ax	;save starting scanline within segment
	mov	SegmentAdder,bx 	;save the adder to correct segment
	mov	ScanlinesLeftInSegment,cx
					;save nbr of scanlines left in segment
	mov	ScanlinesPerSegment,dx	;save nbr of scanlines per seg (or 0 if
					;a small bitmap)
	mov	dl,[si].bmBitsPixel	;get the colour format
	mov	cx,[si].bmWidthBytes	;get nbr of bytes per line
	mov	wBytesPerScanline, cx
	add	bx,[si+12]		;now BX has the segment which contains
					;our starting Y
	mov	di,[si+10]		;now DI points at starting bitmap offset
	mov	es,bx			;now ES:DI points at correct segment
	mov	bl,dl			;save the colour format from the
					;following multiply
	pop	ds			;restore data segment into DS
	mul	cx			;now AX contains offset of starting
					;line within our segment
	add	di,ax			;now ES:DI points at start of line
                                        ;containing our starting Y

;Now decide whether to do it colour or mono:

	rcr	bl,1			;get the 1's bit into carry
	jnc	RSCallRoutine		;if no carry, we're in colour
	cCall	ReadScreenMono
	jmp	short RSEnd

public  RSCallRoutine
RSCallRoutine:
	cCall	<dword ptr ReadColor>	;call the proper routine
RSEnd:
	ret
cEnd		<nogen>


;This macro does the main work: it fetches the next pixel from the temporary
;buffer (pointed to by DS:SI).	BH:WriteEnable, BL: BackgroundColour
;CX=number of pixels to process (must not be greater than 8).
;On Exit:  CH: the newly assembled pixels!

ProcessPixelStream  macro
local	MainLoop
MainLoop:
	in	ax, dx			;;fetch next pixel in AL
	and	al, bh			;;strip off accelerator bits
	sub	al, bl			;;if the current pixel is equal to the
	neg	al			;; background color set NC else CY
	rcl	ch, 1			;;accumulate pixel value in CH
	dec	cl			;;update loop counter
	jnz	MainLoop
	not	ch			;;now pixels==bkColor-->ones
	endm

KeepTable   label byte
	db	0
	db	1
	db	3
	db	7
	db	0fh
	db	1fh
	db	3fh
	db	7fh

cProc	ReadScreenMono, <NEAR, PUBLIC>

	assumes ds, Data
	assumes cs, Code
	assumes es, nothing

cBegin
	mov	bFirstByteShift, 0
	mov	ax, DstxOrg
	mov	dx, ax
	mov	si, ax
	shr	ax, 3			;compute # of bytes to skip at start
	mov	wByteOffset, ax 	; of scanline
	and	dx, 07h 		;compute which pixels to keep in
	jz	RSMStoreFirstPixels	; source bitmap and which pixels to
	mov	bx, 08			; overwrite
	sub	bl, dl
	mov	al, cs:[bx].KeepTable
	mov	bFirstByteKeep, al
	not	al
	mov	bFirstByteErase, al
	mov	dl, bl			;DX: pixels in first partial byte
	cmp	dx, xExt		;is it no more than the X extent?
	jbe	RSMStoreFirstPixels	;yes.  Everything is normal, keep going

	mov	bx, xExt		;BX: X extent
	add	si, bx
	mov	bFirstBytePixels, bl	;which is the # of pixels in 1st byte
	sub	dx, bx			;DX: # of pixels to keep in 1st byte
	xchg	dx, bx
	mov	bFirstByteShift, bl	;this is the 1st byte shift factor
	mov	al, cs:[bx].KeepTable	;AL: additional pixels to keep in DEST
	or	bFirstByteErase, al	;add to pixels to be saved already
	sub	ax, ax
	cwd
	jmp	short RSMContinuePreProcessing

RSMStoreFirstPixels:
	mov	bFirstBytePixels, dl	;save the # of pixels in partial byte

	mov	ax, xExt		;AX: X extent
	add	si, ax			;SI: X extent plus Dest. X
RSMContinuePreProcessing:
	sub	ax, dx			;AX: pixels in whole bytes (except last)
	mov	dx, ax			;save in DX
	shr	ax, 3			;compute number of whole bytes to store
	mov	wByteCount, ax
	and	dx, 07h 		;compute # of trailing partial byte
	jz	RSMStoreLastPixels	; pixels
	mov	bx, 08
	sub	bl, dl			;BL: # of trailing pixels in part. byte
	mov	al, cs:[bx].KeepTable	;AL: bitmask for pixels to keep in dst.
	mov	bLastByteKeep, al
	mov	bLastByteShift, bl
RSMStoreLastPixels:
	mov	bLastBytePixels, dl

	add	si, 7			;compute ending byte in scan
	shr	si, 3
	sub	cx, si			;subtract from bmWidthBytes (in CX)
	xchg	cx, si			;SI: offset to next scan

	CheckFIFOSpace	EIGHT_WORDS
	mov	al, [WriteEnable]	;AL: bitmask for enabled  bit planes
	mov	bh, al			;save in BH
	rol	al, 1			; now set all sorts of 8514 registers
	mov	dx, READ_ENABLE_PORT	; for the read screen operation
	out	dx, ax
	mov	ax, SrcxOrg
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	ax, SrcyOrg
	mov	dx, Srcy_PORT
	out	dx, ax
	mov	ax, 0a000h
	mov	dx, MODE_PORT
	out	dx, ax
	mov	ax, yExt		;AX: Y extent
	mov	cx, ax			;make this the outer (line) loop count
	dec	ax
	and	ax, 07ffh
	out	dx, ax
	mov	ax, xExt
	dec	ax
	mov	dx, RECT_WIDTH_PORT
	out	dx, ax
	mov	ax, READCOMMAND
	mov	dx, COMMAND_FLAG_PORT
	out	dx, ax
RSMCheckReadQueue:
	in	ax, dx			;after issuing the read command, make
	and	ah, DATA_AVAILABLE	; sure data are available before
	jz	RSMCheckReadQueue	; proceeding
	mov	dx, PATTERN_DEFINE_PORT
	mov	bl, BackgroundColour
	and	bl, bh			;strip off accelerator bits!

RSMLineLoop:
	push	cx
	add	di, wByteOffset
	mov	cl, bFirstBytePixels
	or	cl, cl
	jz	RSMCore
	ProcessPixelStream
	mov	al, es:[di]
	and	al, bFirstByteErase	;make room for pixels just read
	mov	cl, bFirstByteShift	;align pixels just read
	shl	ch, cl
	and	ch, bFirstByteKeep	;mask un-needed bits
	or	al, ch			;merge with destination
	stosb				;store whole byte

RSMCore:
	mov	cx, wByteCount
	jcxz	RSMProcessRemainder
RSMByteLoop:
	push	cx
	mov	cl, 8
	ProcessPixelStream
	mov	al, ch
	stosb
	pop	cx
	loop	RSMByteLoop

RSMProcessRemainder:
	mov	cl, bLastBytePixels
	or	cl, cl
	jz	RSMBottomOfLineLoop
	ProcessPixelStream
	mov	al, es:[di]
	and	al, bLastByteKeep
	mov	cl, bLastByteShift
	shl	ch, cl
	or	al, ch
	stosb
RSMBottomOfLineLoop:
	pop	cx			;restore outer loop counter
	add	di, si			;add offset to beginning of next scan
	jz	RSMUpdateSegment	;are exactly at the end of the segment?
	mov	ax, di
	add	ax, wBytesPerScanline	;is it time to move to the next segment
	jz	RSMCarryOn		;not yet
	jc	RSMUpdateSegment	;yes
RSMCarryOn:
	loop	RSMLineLoop		;Keep going
RSMDone:
cEnd

RSMUpdateSegment:
	dec	cx			;update loop counter
	jcxz	RSMDone 		;are we done?
	mov	di, es
	add	di, wSegmentIndex
	mov	es, di
	sub	di, di
	jmp	RSMLineLoop


sEnd            Code
end
