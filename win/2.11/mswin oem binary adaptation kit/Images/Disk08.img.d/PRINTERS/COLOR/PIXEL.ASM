page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	Pixel - Set/Get Pixel
%out	Pixel


?CHKSTK = 1				;Must be in here
extrn dmCheckStack:near
?CHKSTKPROC macro lvs
mov	ax, lvs
call	dmCheckStack
endm



;	Define the portions of gdidefs.inc that will be needed

incDrawMode	= 1


.xlist
include cmacros.inc
include gdidefs.inc
include color.inc
.list




RGBOp	equ	00000001b		;Handle all three planes
MonoOp	equ	00000100b		;Handle only monochrome
EndOp	equ	00001000b		;End of operation


wp	equ	word ptr
bptr	equ	byte ptr



sBegin	code
assumes cs,code
page

extrn	RotBitTbl:byte			;Table of bit masks
extrn	DrawModeTbl:word		;Rop2 drawing mode template table

extrn	SumRGBColorsAlt:near		;Sum RGB colors into a physical color

page

;	dmPixel - Set or Get a Given Pixel
;
;	The given pixel is set to the given color or the given pixel's
;	physical color is returned.
;
;	The physical device may be a monochrome bitmap, or a
;	bitmap in our color format.
;
;	There will be no error checking to see if the bitmap is in our
;	color format.  If it isn't, then we'll just return some bogus
;	value or set some bogus bit.
;
;	If lpDrawMode is NULL, then the physical color of the pixel is
;	returned.  If lpDrawMode isn't NULL, then the pixel will be set
;	to the physical color passed in, combined with the pixel already
;	at that location according to the raster-op in lpDrawMode.  dmPixel
;	doesn't pay attention to the background mode in lpDrawMode.
;
;	No clipping of the input value is required.  GDI clips the
;	coordinate before it is passed in, for both Set and Get.
;
;
;	Entry:	Per parameters
;
;	Exit:	dx:ax = 8000:0000H if error occured
;		dx:ax = physical color if get pixel
;		dx:ax = positive if no error and set was OK.
;
;
;	Uses:	ax,bx,cx,dx,es,flags


cProc	dmPixel,<FAR,PUBLIC>,<si,di>

	parmd	lpDevice		;Pointer to device
	parmw	X			;X coordinate of pixel
	parmw	Y			;Y coordinate of pixel
	parmd	PColor			;Physical color to set pixel to
	parmd	lpDrawMode		;Drawing mode to use or null if Get


cBegin	dmPixel

	ife	???			;If no locals
	xor	ax,ax			;  check anyway
	call	dmCheckStack
	endif
	jnc	Pixel10 		;Room
	jmp	StackOV 		;No room, abort

Pixel10:
	lds	si,lpDevice		;--> physical device
	mov	di,wp bmWidthPlanes[si] ;Set index to next plane (must be even)



;	If the device is a huge bitmap, special processing must be
;	performed to compute the Y address.


	mov	ax,Y			;Need Y coordinate a few times
	mov	bx,ax			;Need a copy for color test
	xor	dx,dx			;Set segment bias to 0
	mov	cx,bmSegmentIndex[si]	;Is this a huge bitmap?
	jcxz	Pixel40 		;  No



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	bx,bmScanSegment[si]	;Get # scans per segment

Pixel30:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	Pixel30 		;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment
	mov	bx,ax			;Compute huge color Y coordinate
	add	ax,ax
	add	ax,bx
	mov	di,bmWidthBytes[si]	;Set offset to next plane (must be even)



;	This is a memory DC.  If this is a monochrome memory DC, set up
;	the inner loop so that it will terminate after one time through
;	and set the color to be equal to the mono bit in the physical
;	color.	If it is color, set up the inner loop for three times,
;	same as for the display.
;
;	Currently:
;		ax     =  Y coordinate for a color bitmap
;		bx     =  Y coordinate for a monochrome bitmap
;		dx     =  Segment bias for huge bitmaps
;		di     =  Offset to next plane
;		ds:si --> PDevice


Pixel40:
	mov	cl,RGBOp		;Assume color loop
	cmp	wp bmPlanes[si],0103h
	je	Pixel50 		;It is our color format
	errnz	bmBitsPixel-bmPlanes-1

	mov	al,bptr (PColor)+Mono	;Set the color to black/white value
	rol	al,1
	cbw
	mov	bptr (PColor),ah
	errnz	MonoBit-01000000b
	mov	cl,MonoOp		;Set loop mask for mono
	mov	ax,bx			;Set ax = Y coordinate for mono bitmap

Pixel50:
	mov	bx,bmWidthBytes[si]	;Get width of one scan line
	add	dx,wp bmBits+2[si]	;Compute segment of the bits
	mov	si,wp bmBits[si]	;Get offset of the bits
	mov	ds,dx			;Set ds:si --> to the bits



;	Compute which byte the pixel is in.  Currently:
;
;		ax = Y coordinate to use
;		bx = width of one scan line
;		di = Index to next scan line
;		cl = looping flag
;		ds:si --> the bitmap or display


Pixel60:
	mul	bx			;Multiply Y by width of a scan
	add	si,ax			;ds:si --> start of scanline byte is in
	mov	ax,X			;Get X coordinate
	mov	bx,ax
	shr	ax,1			;Compute byte offset from start of scan
	shr	ax,1
	shr	ax,1
	add	si,ax			;ds:si --> byte of pixel

	and	bx,00000111B		;Get bit mask for bit
	mov	ch,RotBitTbl[bx]

	mov	dx,di			;Need plane width here
	mov	di,OFF_lpDrawMode	;If a drawmode was given
	mov	bx,SEG_lpDrawMode	;  then set the pixel, else
	mov	ax,bx			;  return it's physical color
	or	ax,di
	jnz	Pixel100		;Given, operation is set pixel
	jmp	Pixel200		;Not given, return pixel
page

;	The operation to be performed is a set.  Currently:
;
;		ch    =   bit mask
;		cl    =   loop mask
;		dx    =   Index to next plane
;		ds:si --> byte bit is to be set in
;		bx:di --> physical drawing mode


Pixel100:
	mov	es,bx			;es:di --> drawmode
	mov	bx,es:Rop2[di]		;Get drawing mode to use
	dec	bx			;Make it zero based
	and	bx,000Fh		;Keep it valid

	lea	di,PColor		;ss:di --> physical color for plane
	shl	bx,1			;Get address of drawmode routine
	push	bp			;Just a little short on registers
	mov	bp,DrawModeTbl[bx]	;Save ROP address here

Pixel140:
	mov	al,ss:[di]		;Get color to set pixel to
	mov	bh,al			;  (drawing modes expect this)
	mov	ah,[si] 		;Get the destination byte
	call	bp			;Do the rasterop
	xor	al,ah			;Isolate the bit that is to change
	and	al,ch			;  and then set its value in the
	xor	al,ah			;  destination byte
	mov	[si],al
	inc	di			;--> next plane's color
	add	si,dx			;--> byte in next plane
	rol	cl,1			;Select next plane
	test	cl,EndOp		;Done with all planes?
	jz	Pixel140		;  Not yet
	pop	bp

Pixel150:
	xor	ax,ax			;Set dx:ax = 0:0
	cwd
	jmp	short Pixel300		;Return 0:0 to show success
page

;	The operation to be performed is get pixel.  The color of the
;	pixel will be returned.  The color of the pixel will be composed
;	from all three planes if a color bitmap and the result "AND"ed
;	to produce the mono bit in the physical color (this is the same
;	as bitblt when blting from color to monochrome).
;
;	If this is a monochrome bitmap, then the color will simply be
;	black or white.
;
;	Currently:
;
;		ch    =   bit mask
;		cl    =   loop mask
;		dx    =   index to next plane
;		ds:si --> byte bit is to be set in

Pixel200:
	cmp	cl,MonoOp		;Is this for a mono bitmap?
	jne	Pixel210		;  No, its for color
	xor	dx,dx			;Assume pixel is black
	xor	ax,ax
	test	[si],ch 		;Is pixel black?
	jz	Pixel300		;  It is, return color in ax:bx
	dec	ax			;  No, set ax:bx for white
	mov	dx,(RedBit+GreenBit+BlueBit+MonoBit)*256+0FFh
	jmp	short Pixel300


Pixel210:
	mov	bl,bh			;Propagate red
	mov	bh,al			;Propagate green

	xor	ax,ax			;Assume bit is 0
	test	[si],ch 		;Is bit 0?
	jz	Pixel240		;Bit was a 0
	dec	ax			;Bit was a 1, make this color white

Pixel240:
	add	si,dx			;--> next plane
	rol	cl,1			;Select next plane
	test	cl,EndOp		;Done with all planes?
	jz	Pixel210		;  Not yet
;	jmp	short Pixel290		;Compute monochrome color
	errn$	Pixel290
page

;	A logical color has been summed into the following registers:
;		bl = red value
;		bh = green value
;		al = blue value
;
;	Compute the mono byte for the physical color and return it

Pixel290:
	mov	dx,ax			;Set things for SumRGBColorsAlt
	mov	ax,bx
	call	SumRGBColorsAlt


Pixel300:

StackOV:

cEnd	dmPixel

	ifdef	debug
	public	Pixel10
	public	Pixel30
	public	Pixel40
	public	Pixel50
	public	Pixel60
	public	Pixel100
	public	Pixel140
	public	Pixel150
	public	Pixel200
	public	Pixel210
	public	Pixel240
	public	Pixel290
	public	Pixel300
	endif

sEnd	code
end
