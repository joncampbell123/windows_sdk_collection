page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1983-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	GDI Output Routines
%out	Output



;	History:
;
;	100985 W.M.	Change cBegin and cEnd and the compiling
;			code to handle the private stack checking
;			code which will return with 'C' set if no
;			room for the frame.


?CHKSTK = 1
?CHKSTKPROC macro lvs
extrn dmCheckStack:near
mov	ax, lvs
call	dmCheckStack
endm
incLogical	= 1
incDrawmode	= 1
incOutput	= 1

.xlist
include cmacros.inc
include gdidefs.inc
include color.inc
.list


comment %
page

This output module supports the following required GDI OUTPUT functions:

	ScanLines

ScanLines must work on the physical device and
on memory bitmaps (both monochrome and in the device's color format).



ScanLines:
	Scanline is implemented using most of the same code as that used for
	polylines.  Scanline doesn't recognize any line style.

	Instead of getting a series of points to draw between, the scanline
	routine gets a series of x coordinate pairs (the Y coordinate
	is implied to be the same for all x's).  The area between these points
	is filled in with the given pattern or pen according to the given
	rasterop (very simular to BITBLT).  Like lines, the interval to be
	filed is exclusive of the end points.
	%
page

DestIsColor	equ	00000001b	;Device is color
HugeDest	equ	01000000b	;Device is a huge bitmap


MaxCodeSize	=	150		;Max size of the created code
page


sBegin	code
assumes cs,code
assumes ds,nothing
assumes es,nothing
assumes ss,nothing


;	The drawing mode table contains the starting address of the code
;	template for each drawing mode.
;
;	The drawing mode length table contains the length of the code
;	for each drawing mode.	The length is needed for computing how
;	many bytes of code is to be moved into the created line drawing
;	code.


	extrn	DrawModeTBL:word	;Table with the drawing modes
	extrn	DrawModeLen:byte	;Length of each drawing mode



;	Others

	extrn	bitmaskTbl1:byte
	extrn	bitmaskTbl2:byte



page

wp		equ	word ptr
bptr		equ	byte ptr
FirstPlane	equ	00100001b	;Plane indicator





page

cProc	dmOutput,<FAR,PUBLIC>,<si,di>

	parmd	lpDstDev		;--> to the destination
	parmw	style			;Output operation
	parmw	count			;# of points
	parmd	lpPoints		;--> to a set of points
	parmd	lpPPen			;--> to physical pen
	parmd	lpPBrush		;--> to physical brush
	parmd	lpDrawMode		;--> to a Drawing mode
	parmd	lpClipRect		;--> to a clipping rectange if <> 0


	locald	EntryAddr		;Entry point into created code
	locald	CurByte 		;Address of current point
	localw	DrawModeIndex		;Drawing Mode index
	localb	TheFlags		;Some flags
	localb	ScanMask		;Background color mask for scanline
	localb	BackMode		;BackGround mode
	localb	BrushAccel		;Brush accelerator for scanline
	localw	NextPlane		;Index to next plane (0 if display)
	localw	PrevSeg 		;Prev segment count for huge bitmaps
	localv	BackColor,4		;Background color
	localv	CurPen,4		;Current Pen
	localv	Linecode,%MaxCodeSize	;Created DDA goes here



cBegin
	ife	???			;If no locals			;100985
	xor	ax,ax			;  check anyway 		;100985
	call	dmCheckStack						;100985
	endif								;100985
	jnc	Room							;100985
	jmp	StackOV 		;No room, abort 		;100985
Room:									;100985
	cld


;	Get the raster op to use and the background color for
;	later use.


	lds	si,lpDrawMode		;--> DrawMode Block
	mov	ax,wp bkColor[si]	;Save background color
	mov	wp BackColor.Red,ax
	mov	ax,wp bkColor.Blue[si]
	mov	wp BackColor.Blue,ax
	mov	ax,bkMode[si]		;Save background mode
	mov	BackMode,al
	mov	bx,ROP2[si]		;Get the drawing mode (raster op)
	dec	bx			;  make it zero based
	and	bx,000FH		;  (play it safe)
	mov	DrawModeIndex,bx	;  and save a copy



;	To save a little space, the starting Y coordinate will be computed
;	before the call is distributed.   The device specific parameters
;	will also be fetched and saved.
;
;	If the device is a huge bitmap, special processing must be
;	performed to compute the Y address.


	lds	si,lpPoints		;--> first point
	mov	ax,ycoord[si]		;Get first Y coordinate
	lds	si,lpDstDev		;--> physical device

	mov	cx,wp bmPlanes[si]	;Check device for validity
	errnz	bmBitsPixel-bmPlanes-1
	cmp	cx,0101h		;Mono chrome?
	je	Output10		;  Yes
	or	bh,DestIsColor SHL 1	;Show device is color
	cmp	cx,0103h		;Our color format?
	je	Output10		;  Yes
	jmp	errExit 		;  No, abort with an error

Output10:
	shr	bh,1

	xor	dx,dx			;Set segment bias to 0
	mov	PrevSeg,dx		;Zero previous segment index
	mov	di,wp bmWidthPlanes[si] ;Set index to next plane (assume small)
	mov	cx,bmSegmentIndex[si]	;Is this a huge bitmap?
	jcxz	Output40		;  No
	or	bh,HugeDest		;  Yes


;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	di,bmScanSegment[si]	;Get # scans per segment

Output20:
	add	dx,cx			;Show in next segment
	inc	PrevSeg 		;Count which segment it's in
	sub	ax,di			;See if in this segment
	jnc	Output20		;Not in current segment, try next
	add	ax,di			;Restore correct Y
	sub	dx,cx			;Show correct segment
	dec	PrevSeg
	mov	di,bmWidthBytes[si]	;Set offset to next plane
	mov	cx,di
	test	bh,DestIsColor		;Is this a color device?
	jz	Output30		;  Mono device, Y is correct
	mov	cx,ax			;  Color, Y is really *3
	add	ax,ax
	add	ax,cx

Output30:

Output40:
	mov	TheFlags,bh		;Save device flags
	mov	NextPlane,di		;Save index to next plane of a scan
	add	dx,wp bmBits+2[si]	;Compute segment of the bits
	mov	SEG_CurByte,dx		;Save segment of bitmap/screen

	mov	dx,bmWidthBytes[si]	;Save width of a scan line
	test	bh,HugeDest		;If huge dest, this isn't the
	jnz	Output50		;  index to the next scan

Output50:
	mul	dx			;Compute address of the scan line
	add	ax,wp bmBits[si]
	mov	OFF_CurByte,ax		;Save offset of the scanline

	mov	ax,style		;Distribute this call based on the style
	cmp	ax,OS_ScanLines 	;  Is this a scanline ?
	jnz	errExit 		;    No, return error
	jmp	scanline		;    Yes, enter scanline code




;	ExitOutput - Exit Output Routine
;
;	The OUTPUT routine is exited.  A valid exit is indicated by
;	returning non-zero to the user.
;
;	Entry:	None
;
;	Exit:	al = 1
;
;	Uses:	All

ExitOutput:
	mov	ax,1
	jmp	short Exit




;	errExit
;
;	An error is returned to the user
;
;	Entry:	None
;
;	Exit:	ax = 0	(error indicator)
;
;	Uses:	All

errExit:
	xor	ax,ax			;Show error
	errn$	Exit




;	Exit - Exit Output Routine
;
;	The OUTPUT routine is exited.  
;
;	Entry:	(ax) = return code
;
;	Exit:	(ax) = return code
;
;	Uses:	All


Exit:

StackOV:								;100985

cEnd


page

;	NormalScan - Normal Scanline Code
;
;	The normal scanline code is used whenever the EGA scanline code
;	cannot be used (memory bitmaps, non-solid brushes, the 5 raster
;	operations which cannot be performed in one pass).
;
;	This code must be copied to the stack and patched as needed for
;	whichever drawing mode is being used.
;
;
;	Entry:	es:di --> destination
;		ds:di --> destination
;		si     =  d15:8 = last byte mask    d7:0 = 0
;		dx     =
;		cx     =  innerloop count
;		bh     =  pen (brush)
;		bl     =  innerloop mask
;		ah     =  first byte mask
;		al     =


NormalScan	proc	far

NormalScan10:
	mov	dh,ah			;Save first byte mask
	mov	dl,1			;Set internal loop count

NormalScan20:
	mov	ah,[di] 		;Get destination
	mov	al,bh			;Get pen

;	<rop>				;Rop goes in here

NormalScan30:
	xor	al,ah			;Only alter bits as needed
	and	al,dh
	xor	al,ah
	stosb
	or	dl,dl			;Exit time?
	jnz	NormalScan40		;  No
	ret				;  Yes, back to caller

NormalScan40:
	mov	dx,si			;Set last byte mask
	jcxz	NormalScan20		;  (fixup needed)

NormalScan50:
	mov	ah,[di] 		;Get destination
	mov	al,bh			;Get pen

;	<rop>				;Rop goes in here


NormalScanJCXZ	equ	NormalScan50-$-1



NormalScan60:
	xor	al,ah			;Only alter bits as needed
	and	al,bl
	xor	al,ah
	stosb
	loop	NormalScan50		;  (fixup needed)
NormalScan70:
	jmp	NormalScan40		;Do last byte  (fixup needed)


NormalScan	endp



;	All the fixups in the scan code only need to have the size of
;	the drawing mode code added or subtracted off of the jmp's
;	displacement.


NormalScan1Len	equ	NormalScan30-NormalScan10
NormalScan2Len	equ	NormalScan60-NormalScan30
NormalScan3Len	equ	$-NormalScan60
NormalScanLOOP	equ	NormalScan70-$-1
NormalScanJMP	equ	NormalScan70-$+1






	subttl	Scanline Output Routine
	page

;	The scanline routine is used for fast filling of intervals on
;	a single scan line (polygon filling).  Scanline is similar to
;	BITBLT in its function, and is used instead of BITBLT to fill
;	arbitrary shapes.
;
;	Scanline fills arbitrary portions of a single scanline according
;	to a given rasterop and pattern (the pattern is used just lke a
;	pen).  It is given a series of inclusive X coordinate pairs, and
;	fills in the region between the two points.  Transparent mode must
;	be handled by scanline.
;
;
;	Currently:
;		BackColor     = background color
;		BackMode      = background mode
;		DrawModeIndex = Raster Operation Index
;		TheFlags      = Type flags for the device or bitmap
;		bh	      = TheFlags
;		CurByte       = Start of the scanline
;
;		bh	      = TheFlags


ScanLine:

;	Get the brush or pen to use.  Pens are solid objects, and
;	therefore the only transparency check for them is if the
;	pen color is the same as the background color (this check
;	must be made with respect to b/w and color).  Brushes can
;	have pixels in them which are the same as the background
;	color, and a mask must be computed to get those which are
;	background color.

	mov	di,OFF_lpPBrush 	;See if a brush was given
	mov	ax,SEG_lpPBrush
	mov	cx,ax
	or	cx,di
	mov	cl,8			;  (will be used later)
	jnz	ScanLine70		;There is a brush



;	A pen is to be used.  Get it and check it for validity.
;	Then make the pen look like a brush so some code can be shared.

ScanLine40:
	lds	di,lpPPen		;--> physical pen
	cmp	OEMPenStyle[di],LS_NOLINE
	je	ScanLine60		;Null pen, exit
	mov	ax,wp OEMPen.Red[di]	;Get the colors of the pen
	mov	dx,wp OEMPen.Blue[di]
	mov	ch,dh			;Show solid color for brush accelerator
	or	ch,BrushIsSolid
	shl	dh,1			;Convert mono bit into 8 bit color
	sar	dh,cl
	errnz	MonoBit-01000000b
	jmp	short ScanLine80	;Continue here



ScanLine50:
	jmp	errExit 		;Error occured

ScanLine60:
	jmp	exit			;Done




;	A brush will be used.  Check to see if it's valid. If it is,
;	save it in the current pen.  Also save the solid accellerator
;	for the brush.


ScanLine70:
	lds	si,lpPoints		;--> the scanline pairs
	mov	bx,ycoord[si]		;Get the correct scanline
	and	bx,00000111b		;  for the brush
	mov	ds,ax			;es:di --> brush
	mov	ax,OEMBrushStyle[di]	;Get brush style
	cmp	ax,MaxBrushStyle	;Legal?
	ja	ScanLine50		;  Outside range, abort
	cmp	ax,BS_HOLLOW		;Hollow?
	je	ScanLine60		;   Yes, return now.

	mov	ch,OEMBrushAccel[di]	;Get the solid brush accellerator
	mov	al,RedPlane[bx][di]	;Get the bits for the brush
	mov	ah,GreenPlane[bx][di]	;  and save them
	mov	dl,BluePlane[bx][di]
	mov	dh,MonoPlane[bx][di]

ScanLine80:
	mov	wp CurPen.Red,ax	;Save pen color and brush accelerator
	mov	wp CurPen.Blue,dx
	mov	BrushAccel,ch



;	If the background mode is transparent, then the brush must be
;	processed to give a mask of bits to alter.  By XORing each
;	byte of the brush with the background color, and ORing the
;	result of each XOR together, a mask can be created where
;	there is a zero for each pixel that matches the background
;	color, and a one for each pixel that doesn't.  This mask
;	can then be used to mask off unaltered bits in the destination
;	(same as using a rotating bit mask for lines).

	mov	al,0FFh 		;Assume no mask needed
	cmp	BackMode,opaque 	;Opaque background mode?
	je	ScanLine100		;  Yes, don't need the mask
	test	TheFlags,DestIsColor	;Color destination?
	jz	ScanLine90		;  No, monochrome
	mov	al,CurPen.Red		;Get back red since it was trashed
	xor	al,BackColor.Red
	xor	ah,BackColor.Green
	xor	dl,BackColor.Blue
	or	al,ah
	or	al,dl
	jz	ScanLine60		;The brush is all background, exit
	jmp	short ScanLine100



;	The destination is monochrome.	Compute the mask for it, and if
;	it is zero, exit (there is nothing to do).

ScanLine90:
	mov	al,BackColor.Mono	;Convert mono background to full
	shl	al,1			;  8 bit color
	sar	al,cl
	xor	al,dh
	jz	ScanLine60		;No bits will be changed
	errnz	MonoBit-01000000b

ScanLine100:
	mov	ScanMask,al		;Save the background mask




;	The background mask has been computed for the current brush.
;	Set up the scanline code for the given rasterop and device.
;	If the destination is the device, the brush (pen) is solid,
;	and the raster op is one of the 11 single pass raster ops,
;	then the static code can be used, else the code will have to
;	be compiled to the stack.


	mov	bx,DrawModeIndex	;This will be needed

;	Copy the scanline template to the stack, and fix it up as
;	needed.

	lea	di,LineCode		;Get offset where code goes
	mov	cx,ss			;Set up es: for string instructions
	mov	es,cx			;  which will be going to the stack
	mov	OFF_EntryAddr,di	;Set entry address for entry
	mov	SEG_EntryAddr,cx	;Set segment address for entry
	mov	cx,cs			;Code comes from the code segment
	mov	ds,cx

	mov	cx,NormalScan1Len	;Move first part of scan code
	mov	si,codeOFFSET NormalScan10
	rep	movsb

	mov	cl,DrawModeLen[bx]	;Move drawing mode code
	mov	ax,cx			;Save for fixups
	add	bx,bx
	mov	si,DrawModeTbl[bx]	;Get offset of drawing mode code
	rep	movsb			;Move drawing logic

	mov	cx,NormalScan2Len	;Move second part of scan code
	mov	si,codeOFFSET NormalScan30
	rep	movsb
	sub	es:NormalScanJCXZ[di],al;Fixup jcxz instruction

	mov	cx,ax			;Move drawing mode code
	mov	si,DrawModeTbl[bx]	;Get offset of drawing mode code
	rep	movsb

	mov	cx,NormalScan3Len	;Move third part of scan code
	mov	si,codeOFFSET NormalScan60
	rep	movsb
	sub	es:NormalScanLOOP[di],al;Fixup loop instruction
	sub	es:NormalScanJMP[di],al ;Fixup jmp instruction





;	The code has been compiled (if needed), now start looping through
;	the scan pairs and do what we came here for, outputting intervals.

	mov	ax,count		;Get the count of scanline pairs
	lds	si,lpPoints		;--> the scanline pairs
	add	si,SIZE PTTYPE

ScanLine210:
	dec	ax			;Any more lines to draw?
	jg	ScanLine220		;  Yes
	jmp	ExitOutput		;  No, exit with all lines drawn

ScanLine220:
	push	ax			;Save count
	lodsw				;Get start coordinate
	mov	bx,ax
	lodsw				;Get end coordinate
	push	ds			;Save points pointer
	push	si

	mov	dx,ax			;Compute extent of interval
	sub	dx,bx
	jnz	ScanLine230		;There is something to the interval
	jmp	ScanLine280		;Null interval, skip it



;	All the magic that is about to occur was copied from the bitblt
;	code.  Setup400's comment should explain it all.  Take a look
;	there for what I hope is a good explanation.

ScanLine230:
	dec	dx
	mov	di,bx			;Compute starting coordinate
	shr	di,1
	shr	di,1
	shr	di,1
	add	di,OFF_CurByte
	mov	ds,SEG_CurByte		;Set segment of bitmap/screen
	mov	es,SEG_CurByte		;Set segment of bitmap/screen

	and	bx,00000111b		;Get first byte mask
	mov	ch,[bx].bitMaskTbl1
	add	bx,dx			;Compute last byte mask
	mov	dx,bx
	and	bx,00000111b
	mov	cl,[bx].bitMaskTbl2	;Get last byte mask

	shr	dx,1			;Compute innerloop byte count
	shr	dx,1
	shr	dx,1
	jnz	ScanLine240		;InnerLoopCount+1 >0, check it out


;	Only one byte will be affected.  Combine the first and last byte
;	masks, and set the loop count to 0.

	and	ch,cl
	xor	cl,cl
	inc	dx			;Fall through to set to 0


ScanLine240:
	dec	dx			;Dec innerloop count (might become 0)
	mov	bl,ScanMask		;Set scan mask
	and	ch,bl			;Create real first/last byte masks
	and	cl,bl


;	The scanline routine will expect the following:
;
;		es:di --> destination
;		ds:di --> destination
;		si     =  d15:8 = last byte mask
;		dx     =
;		cx     =  innerloop count
;		bh     =  pen (brush)
;		bl     =  innerloop mask
;		ah     =  first byte mask
;		al     =


	mov	ah,ch			;Set first byte mask
	mov	ch,cl			;Set last byte mask
	xor	cl,cl
	mov	si,cx
	mov	bh,CurPen.Mono		;Set color (assume single pass)
	mov	cx,dx			;Set innerloop count

	mov	al,FirstPlane		;Initialize plane indicator
	test	TheFlags,DestIsColor	;One pass or three?
	jnz	ScanLine260		;Three
	call	EntryAddr		;Do the scan
	jmp	short ScanLine280



;	Three passes must be made over the bitmap/screen


ScanLine260:
	push	di			;Save bits bointer
	push	si			;Save last byte mask
	push	ax			;Save first byte mask
	push	bx			;Save pen color and innerloop mask
	push	cx			;Save innerloop count

	push	ax			;Will use this for I/O

	shr	al,1
	and	ax,00000011b
	xchg	si,ax			;Get next brush/pen to use
	mov	bh,CurPen[si]
	mov	si,ax
	pop	ax

	call	EntryAddr		;Do the segment
	pop	cx			;Get innerloop count
	pop	bx			;Get pen color and innerloop mask
	pop	ax			;Get first byte mask, plane indicator
	pop	si			;Get last byte mask
	pop	di			;Get bits pointer
	add	di,NextPlane		;--> next plane if memory bitmap
	rol	al,1			;Any more planes?
	jnc	ScanLine260		;More planes

ScanLine280:
	pop	si			;Get back points pointer
	pop	ds
	pop	ax			;Get back points count
	jmp	ScanLine210
page
	ifdef	debug

	public	Room
	public	Output10
	public	Output20
	public	Output30
	public	Output40
	public	ExitOutput
	public	NormalScan10
	public	NormalScan20
	public	NormalScan30
	public	NormalScan40
	public	NormalScan50
	public	NormalScan60
	public	NormalScan70
	public	ScanLine
	public	ScanLine40
	public	ScanLine50
	public	ScanLine60
	public	ScanLine70
	public	ScanLine80
	public	ScanLine90
	public	ScanLine100
	public	ScanLine210
	public	ScanLine220
	public	ScanLine230
	public	ScanLine240
	public	ScanLine260
	public	ScanLine280
	endif


sEnd	code
end
