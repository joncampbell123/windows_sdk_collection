page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1985-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	GDI Color BitBLT
%out	BitBlt


?CHKSTK = 1				;Must be in here
?CHKSTKPROC macro lvs
extrn dmCheckStack:near
mov	ax, lvs
call	dmCheckStack
endm


;	Define the portions of gdidefs.inc that will be needed by bitblt.

incLogical	=	1
incDrawMode	=	1

.xlist
include cmacros.inc
include gdidefs.inc
include color.inc
.list


wp	equ	word ptr
bptr	equ	byte ptr


include bitblteq.inc			;Lots of equates, start of code segment
page

;	The DEV structure contains all the information taken from the
;	PDevices passed in.  PDevices are copied to the frame to reduce
;	the number of long pointer loads required.  Having the data
;	contained in the structure allows MOVSW to be used when copying
;	the data.
;
;	WidthBits	The number of pixels wide the device is.
;
;	Height		The number of scans high the device is.
;
;	WidthB		The width of a scan in bytes.
;
;	lpBits		The pointer to the actual bits of the device.
;			It will be adjusted as necessary to point to the
;			first byte to be modified by the BLT operation.
;
;	PlaneW		Width of one plane of data.  Only used if the
;			device is a small color bitmap.
;
;	SegIndex	Index to get to the next segment of the bitmap.
;			Only defined if the bitmap is a huge bitmap.
;
;	ScansSeg	Number of scan lines per 64K segment.  Only
;			defined if the bitmap is a huge bitmap.
;
;	FillBytes	Number of unused bytes per 64K segment.  Only
;			defined if the bitmap is a huge bitmap.
;
;	DevFlags	Device Specific Flags
;			SpansSeg   - BLT will span 64K segment of the device
;			ColorUp    - Generate color scan line update
;			IsColor    - Device is a color device
;
;	CompTest	JC or JNC opcode, used in the huge bitmap scan line
;			update code.  This opcode is based on whether the
;			BLT is Y+, or Y-.
;
;	CompValue	Range of addresses to compare the offset against
;			to determine if overflow occured.  CompTest is the
;			conditional jump to use for no overflow after doing
;			a compare with the offset register and this value.
;
;	NextScan	Bias to get to the next (previous) scan line.


DEV		struc

  WidthBits	dw	?		;Width in bits
  Height	dw	?		;Height in scans
  WidthB	dw	?		;Width in bytes
  lpBits	dd	?		;Pointer to the bits
  PlaneW	dw	?		;Increment to next plane
  SegIndex	dw	?		;Index to next segment if huge bitmap
  ScansSeg	dw	?		;Scans per segment if huge
  FillBytes	dw	?		;Filler bytes per segment if huge
  DevFlags	db	?		;Device flags as given above
  CompTest	db	?		;jc or jnc opcode
  CompValue	dw	?		;Huge bitmap overflow range
  NextScan	dw	?		;Index to next scan

DEV		ends


IsColor 	equ	00000001b	;Device is color
ColorUp 	equ	00000010b	;Color scan line update
SpansSeg	equ	10000000b	;BLT spans a segment boundary

OFF_lpBits	equ	wp lpBits	;Offset  portion of lpBits
SEG_lpBits	equ	wp lpBits+2	;Segment portion of lpBits



page

cProc	dmBITBLT,<FAR,PUBLIC>,<si,di>

	parmd	lpDestDev		;--> to destination bitmap descriptor
	parmw	DestxOrg		;Destination origin - x coordinate
	parmw	DestyOrg		;Destination origin - y coordinate
	parmd	lpSrcDev		;--> to source bitmap descriptor
	parmw	SrcxOrg 		;Source origin - x coordinate
	parmw	SrcyOrg 		;Source origin - y coordinate
	parmw	xExt			;x extent of the BLT
	parmw	yExt			;x extent of the BLT
	parmd	Rop			;Raster operation descriptor
	parmd	lpPBrush		;--> to a physical brush (pattern)
	parmd	lpDrawMode		;--> to a drawmode


	localv	Src,%(SIZE DEV) 	;Source device data
	localb	phaseH			;Horizontal phase (rotate count)
	localb	PatRow			;Current row for patterns [0..7]
	localb	direction		;Increment/decrement flag
	localb	Firstfetch		;Number of first fetches needed
	localb	stepDirection		;Direction of move (left right)
	localb	BrushAccel		;Brush accelerator
	localb	TheFlags		;Lots of flags
	localb	MooreFlags		;More flags
	localb	nLogOps 		;# logic operators in expression

	localw	startMask		;Mask for first dest byte
	localw	lastMask		;Mask for last	dest byte
	localw	maskP			;Horizontal phase mask
	localw	innerLoopCnt		;# of entire bytes to BLT in innerloop

	localw	operands		;The operand string to use for decoding
	localw	startFL 		;Start of fetch/logic operation
	localw	endFL			;End   of fetch/logic operation
	localw	endFLS			;End   of fetch/logic/store operation
	localw	AddrBrushIndex		;Address of brush index in code


	locald	BLTaddr 		;BLT offset address

	localv	Dest,%(SIZE DEV)	;Destination device data
	localv	nOps,4			;# of each operand used
	localv	ABrush,SizePattern	;Munged color ==> mono brush,



;	ABrush is overloaded.  If going color ==> mono, it will
;	contain a brush processed against the given background
;	and foreground colors.	If going mono ==> color, it will
;	contain the AND and XOR masks for converting each plane.
;
;	2*NumberPlanes <= SizePattern must hold true to overload
;	the ABrush like this.

	errnz	SizePattern-8		;Must be at least 6 bytes worth



cBegin
	ife	???			;If no locals
	xor	ax,ax			;  check anyway
	call	dmCheckStack
	endif
	jnc	BitBlt000		;There was room for the frame
	jmp	StackOV 		;There was no room

BitBlt000:



subttl	ROP Preprocessing
page

;	Parse the raster op to find out how many operands will be needed
;	and how many operators there are.  Trailing NOTs that cancel will
;	be discarded.
;
;	If this is a source copy, then the parsing will be skipped since
;	the result of the parsing is quit simple and the speed increase
;	is well worth it.
;
;	This could be performed via table lookup on the high order word
;	of the raster op at a cost of approximately 512 bytes, and a
;	savings of lots of clocks.


	cld				;Let's make no assumptions about this!
	mov	cx,0100H		;Initialize operand counters for a
	mov	wp nOps.OpSpec,cx	;  source copy
	errnz	OpSpec			;OpSpec must be an index of 0
	errnz	OpSrc-1 		;OpSrc must be an index of 1

	xor	ax,ax
	mov	wp nOps.OpDest,ax	;Zero rest of the counters
	errnz	OpDest-2		;OpDest must be an index of 2
	errnz	OpPat-3 		;OpPat must be an index of 3


	mov	ax,OFF_Rop		;Get raster op
	cmp	ax,sourceCopy		;Is this a source copy?
	jz	Setup100		;  Yes, skip parsing

	mov	nOps.OpSrc,cl		;Zero source count
	mov	dx,logOp1		;Load initial test mask
	xchg	ch,cl			;(ch)=#trailing NOTs,(cl)=#operands



;	The ROP isn't for a source copy.  The number of operands needed
;	is the number of binary operators + 1.	Loop through the ROP
;	counting the number of binary operators.  Also count the number
;	of trailing NOTs for reduction.
;
;	For the loop:
;
;		ch = Number of trailing NOTs	(initially 0)
;		cl = Number of binary operators (initially 1)


Parse10:
	inc	ch			;Assume a trailing not
	test	ax,dx			;Is this a NOT?
	jz	Parse20 		;  Yes, no extra operand needed
	inc	cl			;  No, will need an operand for it
	xor	ch,ch			;    Also clear trailing NOT count
	errnz	LogNOT			;NOT must be zero

Parse20:
	shl	dx,1			;Rotate to next mask
	shl	dx,1			;If there was no carry,
	jnc	Parse10 		;  then more to test

	errnz	LogOp1-0000000011000000B;These must also hold true
	errnz	LogOp2-0000001100000000B
	errnz	LogOp3-0000110000000000B
	errnz	LogOp4-0011000000000000B
	errnz	LogOp5-1100000000000000B



;	All the binary operators have been counted.  Now remove
;	trailing NOTs that cancel.  Subtracting the number of NOTs
;	which cancel from the total number of operators possible
;	will give the number of operations that must be performed.
;	This number will be used by the code that generates the
;	ROP logic.
;
;	Currently:
;
;		(cl) = the number of operands needed
;		(ch) = number of trailing NOTs


	mov	dl,5			;Assume 5 logical operators
	test	ax,LogPar		;Need an extra NOT?
	jz	Parse30 		;  No
	inc	ch			;  Yes, show another trailing NOT
	inc	dl			;    and another logical operator
Parse30:
	and	ch,NOT 1		;Remove trailing NOTs that cancel
	sub	dl,ch			;(dh) = # Logical operations to perform
	mov	nLogOps,dl		;Save number of logical operators



;	The number of operands required and the number of operators
;	have been counted.  Rotate the parse string so that the
;	first operand used will be in D1:D0.
;
;	The amount to rotate is the number of operands, plus any
;	bias specified in the ROP (epsOff).  If the parse string is
;	complex, then this number must also be adjusted for both the
;	"push" and "pop" encoded into the string.
;
;
;	for:  PDSPnoaxn , ROP = 0085 1E05
;
;
;	    1E05 = 00 01 11 10 00 0 001 01
;		   |  |  |  |  |  |  |	|
;		   |  |  |  |  |  |  |	|___ bias start by 1
;		   |  |  |  |  |  |  |______ use string 1
;		   |  |  |  |  |  |_________ parity - no trailing NOT
;		   |  |  |  |  |____________ Logic operation #1 is a NOT
;		   |  |  |  |_______________ Logic operation #2 is a OR
;		   |  |  |__________________ Logic operation #3 is a AND
;		   |  |_____________________ Logic operation #4 is a XOR
;		   |________________________ Logic operation #5 is a NOT
;
;
;
;	    String #1 is defined as:	  SPDSPDSP
;
;	     4 - 3 binary operators require 4 operands
;	     1 - bias by 1 as specified in ROP
;	    ---
;	     5 - total alignment needed
;
;
;	    SPDSPDSP  rotated left 5 times gives  DSPSPDSP
;						      ||||_ 1st operand
;						      |||__ 2nd operand
;						      ||___ 3rd operand
;						      |____ 4th operand
;	Currently:
;
;		(cl) = # operands needed so far (source, destination, pattern)
;		(dl) = # logical operators (ANDs, ORs, XORs, NOTs)


	mov	bx,ax			;Compute index of parse string
	and	bx,epsIndx
	shr	bx,1
	errnz	epsIndx-11100B
	and	al,epsOff		;Get offset into string
	errnz	epsOff-11B

	cmp	bl,cmplxParseStr	;If a complex parse string
	jc	Parse50 		;  (isn't complex)
	add	cl,2			;  two more operands will be needed
					;    (a push and a pop)
Parse50:
	mov	dl,cl			;Save number of operands
	add	cl,al			;Compute first operand's index
	add	cl,cl			;(cl) = index to first used operand

	mov	bx,[bx].parseStrings	;Get the correct parse string
	rol	bx,cl			;Rotate operands into initial place
	mov	operands,bx		;Save initial parse string



;	The first used operand of the parse string is now in D1:D0
;	the next used operand in D2:D3 and so on.
;
;	Now count the number of times each type of operand is used.
;
;	Currently:
;
;		bx = new parse string
;		dl = number of operands needed


Parse60:
	mov	si,bx			;Set operand counter index
	and	si,11B
	ror	bx,1			;Rotate in next operand
	ror	bx,1
	inc	bptr (nOps[si]) 	;Increment next operand counter
	dec	dl			;More counters to increment?
	jnz	Parse60 		;  Yes



;	The number of times each type of operand is used has been
;	computed.  This is important to know since we don't want to
;	check for a pattern or source field if we don't have one.
;
;	Also, we now know if there are an even number of push/pops
;	that are performed.  If there are an odd number of push/pops
;	then we can abort early and do nothing, guaranteeing that
;	there won't be anything left on the stack for those
;	implementors that must actually perform pushes/pops.


	test	nOps.opSpec,1		;Odd number of push/pops?
	jz	Setup100		;  No
;	jnz	complain		;  Yes
	errn$	complain





;	complain - complain that something is wrong
;
;	An error is returned to the caller without BLTing anything.
;
;	Entry:	None
;
;	Exit:	(ax) = 0 (error flag)
;
;	Uses:	None

complain:
	xor	ax,ax			;Set the error code
	jmp	exit_fail



subttl	PDevice Processing
page

;	Check the required bitmaps for validity, get their parameters
;	and store the information locally.
;
;	If a brush (pattern) is required, get the address of the bits
;	making up the pattern.
;
;	If an invalid bitmap is encountered, abort.


Setup100:
	mov	ax,ss			;Set es to frame segment
	mov	es,ax
	xor	bx,bx			;bh =TheFlags
	cmp	nOps.opSrc,bl		;Is the source used?
	je	Setup110		;  No, skip source validation
	mov	bh,1			;Show source present
	errnz	SrcPresent-00010000b

	lds	si,lpSrcDev		;Get pointer to source
	mov	ax,ds			;Null pointer?
	or	ax,si
	jz	complain		;Null pointer, error, abort
	lea	di,Src			;--> where parameters will go
	call	CopyDev 		;Get all the data



;	Decode the destination parameters.  All BLTs must have a destination.
;	The pattern fetch code will be based on the color format of the
;	destination.  If the destination is mono, then a mono fetch will be
;	performed.  If the destination is color, then a color fetch will be
;	performed.

Setup110:
	lds	si,lpDestDev		;Get pointer to destination
	lea	di,Dest 		;--> where parameters will go
	call	CopyDev 		;Get all the data
	test	bh,DestIsColor		;Show color pattern needed if
	jz	Setup120		;  destination is color
	or	bh,ColorPat



;	Check for color conversion.  If so, then set GagChoke.
;	Color conversion will exist if the source and destination are of
;	different color formats.


Setup120:
	test	bh,SrcPresent		;Is there a source?
	jz	Setup140		;  No, cannot be converting.
	mov	al,bh
	and	al,SrcIsColor+DestisColor
	jz	Setup130		;Both are monochrome
	xor	al,SrcIsColor+DestisColor
	jz	Setup130		;Both are color
	or	bh,GagChoke		;Mono ==> color or color ==> mono


;	Setup the scan line update flag in the source device structure.
;	The source will use a monochrome style update if it is the display,
;	it is monochrome, or it is color and the destination device is
;	monochrome.

Setup130:
	mov	al,bh			;Set 'Z' if to use color update
	and	al,SrcIsColor+DestIsColor
	xor	al,SrcIsColor+DestIsColor
	jnz	Setup140		;Use the mono update
	or	Src.DevFlags,ColorUp	;Show color scan update


;	Setup the scan line update flag in the destination device
;	structure.  The destination will use a monochrome update
;	if it is monochrome or the display.  It will use a color
;	update if it is a color bitmap.

Setup140:
	mov	al,bh			;Set 'Z' if to use color destination
	and	al,DestIsColor;  update code
	xor	al,DestIsColor
	jnz	Setup150		;Mono update
	or	Dest.DevFlags,ColorUp	;Show color scan update



subttl	Pattern Preprocessing
page

;	If a pattern is needed, make sure that it isn't a hollow
;	brush.	If it is a hollow brush, then return an error.
;
;	The type of brush to use will be set, and the brush pointer
;	updated to point to the mono bits if the mono brush will be
;	used.  The type of brush used will match the destination device.
;
;	If the destination is mono and the source is color, then a mono
;	brush fetch will be used, with the color brush munged in advance
;	according to the background/foreground colors passed:
;
;	    All brush pixels which match the background color should be set
;	    to white (1).  All other brush pixels should be set to black (0).
;
;	    If the physical color is stored as all 1's or 0's for each
;	    plane, then by XORing the physical color for a plane with the
;	    corresponding byte in the brush, and ORing the results, this
;	    will give 0's where the color matched, and  1's where the colors
;	    didn't match.  Inverting this result will then give 1's where
;	    the brush matched the background color and 0's where it did not.
;
;	If both the source and destination are color, or the source is mono
;	and the destination color, then the color portion of the brush will
;	be used.
;
;	If both the source and destination are mono, then the monochrome
;	portion of the brush will be used.


Setup150:
	mov	TheFlags,bh		;Save flag values
	cmp	nOps.opPat,0		;Is a pattern needed?
	je	Setup200		;  No, skip pattern check
	or	TheFlags,PatPresent	;  Yes, show a pattern is present
	lds	si,lpPBrush		;--> physical brush
	mov	ax,ds
	or	ax,si
	jz	vComplain		;Null pointer, error
	cmp	OEMBrushStyle[si],BS_HOLLOW
	je	vComplain		;Hollow brush.	Abort with an error
	mov	al,OEMBrushAccel[si]	;Save EGA brush accelerator
	mov	BrushAccel,al

	shl	bh,1			;What type of pattern fetch?
	js	Setup200		;Color pattern Fetch.  OK as is
	jnc	Setup170		;Not going color ==> mono, OK as is
	errnz	ColorPat-01000000b
	errnz	GagChoke-10000000b


;	This is a color ==> mono BLT.  The color brush must be processed
;	against the background and foreground colors as stated above,
;	giving a pseudo monochrome brush.
;
;	This new brush will be stored on the frame and the brush pointer
;	biased to point to it.


	les	di,lpDrawMode		;Get background color
	mov	bx,wp es:bkColor[di]
	mov	dx,wp es:bkColor[di]+2
	mov	ax,ss
	mov	es,ax
	lea	di,ABrush		;es:di --> temp brush area
	mov	cx,SizePattern		;Set loop count

Setup160:
	lodsb				;Get red plane
	mov	ah,GreenPlane-1[si]	;Get green plane
	xor	ax,dx			;Set matching bits to 0
	or	ah,al
	mov	al,BluePlane-1[si]	;Process blue plane
	xor	al,bl
	or	al,ah			;Combine red and green
	not	al			;Do final inversion
	stosb				;  and store the byte
	loop	Setup160
	mov	SEG_lpPBrush,es 	;Set segment of brush
	lea	si,-OEMBrushStyle[di]	;Set up for offsetting mono brush

Setup170:
	add	si,MonoPlane		;--> mono portion of the brush
	mov	OFF_lpPBrush,si
	jmp	short Setup200




;	vComplain - Just a vector to Complain

vComplain:
	jmp	complain


;	vExit - Just a vector to Exit

vExit:
	jmp	Exit



subttl	Input Clipping
page

;	GDI doesn't do input clipping.  The source device must be clipped
;	to the device limits, else an exception could occur while in
;	protected mode.
;
;	The destination X and Y, and the extents have been clipped by GDI
;	and are positive numbers (0-7FFFh).  The source X and Y could be
;	negative.  The clipping code will have to constantly check for
;	negative values.


Setup200:
	mov	si,xExt 		;X extent will be used a lot
	mov	di,yExt 		;Y extent will be used a lot
	test	TheFlags,SrcPresent	;Is there a source?
	jz	Setup230		;No source, no input clipping needed

	mov	ax,SrcxOrg		;Will need source X org
	mov	bx,Src.WidthBits	;Maximum allowable is WidthBits-1
	or	ax,ax			;Any left edge overhang?
	jns	Setup205		;  No, left edge is on the surface


;	The source origin is off the left hand edge of the device surface.
;	Move both the source and destination origins right by the amount
;	of the overhang and also remove the overhang from the extent.
;
;	There is no need to check for the destination being moved off the
;	right hand edge of the device's surface since the extent would go
;	zero or negative if that were to happen.


	add	si,ax			;Subtract overhang from X extent
	js	vExit			;Wasn't enough, nothing to BLT
	sub	DestxOrg,ax		;Move destination left
	xor	ax,ax			;Set new source X origin
	mov	SrcxOrg,ax


;	The left hand edge has been clipped.  Now clip the right hand
;	edge.  Since both the extent and the source origin must be
;	positive numbers now, any sign change from adding them together
;	can be ignored if the comparison to bmWidth is made as an
;	unsigned compare (maximum result of the add would be 7FFFh+7FFFh,
;	which doesn't wrap past zero).


Setup205:
	add	ax,si			;Compute right edge + 1
	sub	ax,bx			;Compute right edge overhang
	jbe	Setup210		;No overhang
	sub	si,ax			;Subtract overhang from X extent
	js	vExit			;Wasn't enough, nothing to BLT

Setup210:
	mov	xExt,si 		;Save new X extent


;	Now clip the Y coordinates.  The procedure is the same and all
;	the above about positive and negative numbers still holds true.


Setup215:
	mov	ax,SrcyOrg		;Will need source Y org
	mov	bx,Src.Height		;Maximum allowable is Height-1
	or	ax,ax			;Any top edge overhang?
	jns	Setup220		;  No, top is on the surface


;	The source origin is off the top edge of the device surface.
;	Move both the source and destination origins down by the amount
;	of the overhang, and also remove the overhang from the extent.
;
;	There is no need to check for the destination being moved off
;	the bottom of the device's surface since the extent would go
;	zero or negative if that were to happen.


	add	di,ax			;Subtract overhang from Y extent
	js	vExit			;Wasn't enough, nothing to BLT
	sub	DestyOrg,ax		;Move destination down
	xor	ax,ax			;Set new source Y origin
	mov	SrcyOrg,ax


;	The top edge has been clipped.	Now clip the bottom edge. Since
;	both the extent and the source origin must be positive numbers
;	now, any sign change from adding them together can be ignored if
;	the comparison to bmWidth is made as an unsigned compare (maximum
;	result of the add would be 7FFFh+7FFFh, which doesn't wrap thru 0).


Setup220:
	add	ax,di			;Compute bottom edge + 1
	sub	ax,bx			;Compute bottom edge overhang
	jbe	Setup225		;No overhang
	sub	di,ax			;Subtract overhang from Y extent
	js	vExit			;Wasn't enough, nothing to BLT

Setup225:
	mov	yExt,di 		;Save new Y extent

Setup230:
	or	si,si
	jz	vExit			;X extent is 0
	or	di,di
	jz	vExit			;Y extent is 0



page

;	Currently:
;		si = X extent
;		di = Y extent


	dec	si			;Make the extents inclusive of the
	dec	di			;  last point

subttl	Phase Processing (X)
page

;	Now the real work comes along:	In which direction will the
;	copy be done?  Refer to the 10 possible types of overlap that
;	can occur (10 cases, 4 resulting types of action required).
;
;	If there is no source bitmap involved in this particular BLT,
;	then the path followed must allow for this.  This is done by
;	setting both the destination and source parameters equal.


	mov	dx,xExt 		;Get X extent
	dec	dx			;Make X extent inclusive

	mov	bx,DestxOrg		;Get destination X origin
	mov	di,bx
	and	bx,00000111B		;Get offset of destination within byte
					;   and set up BX for a base register!


;	If there is no source, then just use the pointer to the destination
;	bitmap and load the same parameters, which will cause the "equality"
;	path to be followed in the set-up code.  This path is the favored
;	path for the case of no source bitmap.


	mov	ax,di			;Assume no source needed
	cmp	nOps.opSrc,bh		;Is a source needed?
	je	Setup310		;  No, just use destination parameters
	mov	ax,SrcxOrg		;  Yes, get source origin X
	mov	firstFetch,2		;  Assume two initial fetches (if no
					;    source, then it will be set = 1
					;    later)
Setup310:
	mov	si,ax
	and	ax,00000111B		;Get offset of source within byte

	cmp	si,di			;Which direction will we be moving?
	jl	Setup340		;Move from right to left




;	The starting X of the source rectangle is >= the starting X of
;	the destination rectangle, therefore we will be moving bytes
;	starting from the left and stepping right.
;
;	Alternatively, this is the path taken if there is no source
;	bitmap for the current BLT.
;
;	Rectangle cases: 3,4,5,6,8

Setup320:
	sub	al,bl			;Compute horiz. phase  (source-dest)
	mov	stepDirection,stepright ;Set direction of move
	mov	ch,cs:[bx].bitmaskTbl1	;Get starting byte mask
	ja	Setup330		;Scan line case 2, everything is
					;  already set for this case.



;	Scan line cases 1 and 3:
;
;	The correct first byte fetch needs to be set for the beginning
;	of the outer loop, and the phase must be made into a positive
;	number.
;
;	This is the path that will be followed if there is no source bitmap
;	for the current BLT.

	mov	firstFetch,1		;Set one initial fetch




;	We now have the correct phase and the correct first character fetch
;	routine set.  Save the phase and ...
;
;	currently:   al = phase
;		     bl = dest start mod 8
;		     ch = first byte mask
;		     dx = inclusive X bit count
;		     si = source X start (if there is a source)
;		     di = destination X start
;

Setup330:
	add	al,8			;Phase must be positive
	and	al,00000111B



;	To calculate the last byte mask, the inclusive count can be
;	added to the start X MOD 8 value, and the result taken MOD 8.
;	This is attractive since this is what is needed later for
;	calculating the inclusive byte count, so save the result
;	of the addition for later.

	add	bx,dx			;Add inclusive extent to dest MOD 8
	mov	dx,bx			;Save for innerloop count !!!
	and	bx,00000111B		;Set up bx for a base reg
	mov	cl,cs:[bx].bitmaskTBL2	;Get last byte mask

	mov	bl,al			;Compute offset into phase mask table
	add	bx,bx
	mov	bx,cs:[bx].phaseTBL1	;Get the phase mask


;	Currently:
;		al = phase
;		bx = phase mask
;		cl = last byte mask
;		ch = first byte mask
;		dx = inclusive bit count + dest start MOD 8
;		si = source X start (if there is a source)
;		di = destination starting X

	jmp	short Setup400		;Finish here






;	The starting X of the source rectangle is < the X of the destination
;	rectangle, therefore we will be moving bytes starting from the right
;	and stepping left.
;
;	This code should never be reached if there is no source bitmap
;	for the current BLT.
;
;	Rectangle cases: 1,2,7

Setup340:
	mov	stepDirection,ah	;Set direction of move
	errnz	stepleft
	mov	cl,cs:[bx].bitmaskTbl1	;Get last byte mask
	add	ax,dx			;Find end of the source


;	To calculate the first byte mask, the inclusive count is
;	added to the start MOD 8 value, and the result taken MOD 8.
;	This is attractive since this is what is needed later for
;	calculating the inclusive byte count, so save the result
;	of the addition for later.

	add	bx,dx			;Find end of the destination
	add	di,dx			;Will need to update dest start address
	add	si,dx			;  and source's too
	mov	dx,bx			;Save inclusive bit count + start MOD 8
	and	ax,00000111B		;Get source offset within byte
	and	bx,00000111B		;Get dest   offset within byte
	mov	ch,cs:[bx].bitmaskTbl2	;Get start byte mask
	sub	al,bl			;Compute horiz. phase  (source-dest)
	jb	Setup350		;Scan line case 5, everything is
					;  already set for this case.



;	Scan line cases 4 and 6:
;
;	The correct first byte fetch needs to be set for the beginning
;	of the outer loop

	mov	firstFetch,1		;Set initial fetch routine


Setup350:
	add	al,8			;Ensure phase positive
	and	al,00000111B




;	We now have the correct phase and the correct first character fetch
;	routine set.  Generate the phase mask and save it.
;
;	currently:   al = phase
;		     ch = first byte mask
;		     cl = last byte mask
;		     dx = inclusive bit count + start MOD 8

	mov	ah,cl			;Save last mask
	mov	cl,al			;Create the phase mask
	mov	bx,00FFH		;  by shifting this
	shl	bx,cl			;  according to the phase
	mov	cl,ah			;Restore last mask
;	jmp	Setup400		;Go compute # of bytes to BLT
	errn$	Setup400




;	The different processing for the different X directions has been
;	completed, and the processing which is the same regardless of
;	the X direction is about to begin.
;
;	The phase mask, the first/last byte masks, the X byte offsets,
;	and the number of innerloop bytes must be calculated.
;
;
;	Nasty stuff coming up here!  We now have to determine how
;	many bits will be BLTed and how they are aligned within the bytes.
;	This is how it's done (or how I'm going to do it):
;
;	The number of bits (inclusive number that is) is added to the
;	start MOD 8 value ( the left side of the rectangle, minimum X
;	value), then the result is divided by 8. Then:
;
;
;	    1)	If the result is 0, then only one destination byte is being
;		BLTed.	In this case, the start & ending masks will be ANDed
;		together, the innerloop count (# of full bytes to BLT) will
;		be zeroed, and the lastMask set to all 0's (don't alter any
;		bits in last byte which will be the byte following the first
;		(and only) byte).
;
;			|      x x x x x|		|
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 3,  extent-1 = 4
;			3+7 DIV 8 = 0, only altering one byte
;
;
;
;	    2)	If the result is 1, then only two bytes will be BLTed.
;		In this case, the start and ending masks are valid, and
;		all that needs to be done is set the innerloop count to 0.
;		(it is true that the last byte could have all bits affected
;		the same as if the innerloop count was set to 1 and the
;		last byte mask was set to 0, but I don't think there would be
;		much time saved special casing this).
;
;			|  x x x x x x x|x x x x x x x|
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 1,  extent-1 = 14
;			3+14 DIV 8 = 1.  There is a first and last
;			byte but no innerloop count
;
;
;
;	    3)	If the result is >1, then there is some number of entire
;		bytes to be BLted by the innerloop.  In this case the
;		number of innerloop bytes will be the result - 1.
;
;			|	       x|x x x x x x x x|x
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 7,  extent-1 = 9
;			7+9  DIV 8 = 2.  There is a first and last
;			byte and an innerloop count of 1 (result - 1)
;
;	Currently:	al = horizontal phase
;			bx = horizontal phase mask
;			ch = first byte mask
;			cl = last byte mask
;			dx = left side X MOD 8 + inclusive X count
;			si = source start X
;			di = dest   start X


Setup400:
	mov	phaseH,al		;Save horizontal phase
	mov	maskP,bx		;Save phase mask
	shr	dx,1			;/8 to get full byte count
	shr	dx,1
	shr	dx,1
	jnz	Setup410		;Result is >0, check it out


;	There will only be one byte affected.  Therefore the two byte masks
;	must be combined, the last byte mask cleared, and the innerloop
;	count set to zero.

	and	ch,cl			;Combine the two masks
	xor	cl,cl			;Clear out the last byte mask
	inc	dx			;Now just fall through to set
	errn$	Setup410		;  the innerloop count to 0!


Setup410:
	dec	dx			;Dec count (might become 0 just like
	mov	innerLoopCnt,dx 	;  we want), and save it
	mov	bl,ch
	mov	ch,cl			;Compute last byte mask
	not	cl			;  and save it
	mov	lastmask,cx
	mov	bh,bl			;Compute start byte mask
	not	bl			;  and save it
	mov	startMask,bx



;	There may or may not be a source bitmap for the following address
;	computation.  If there is no source, then the vertical setup code
;	will be entered with both the source and destination Y's set to the
;	destination Y and the address calculation skipped.  If there is a
;	source, then the address calculation will be performed and the
;	vertical setup code entered with both the source and destination Y's.

	shr	di,1			;Compute byte offset of destination
	shr	di,1			;  and add to current destination
	shr	di,1			;  offset
	add	Dest.OFF_lpBits,di

	mov	dx,DestyOrg		;Get destination Y origin
	mov	ax,dx			;Assume no source
	mov	cl,TheFlags
	test	cl,SrcPresent		;Is a source needed?
	jz	Setup500		;  No, skip source set-up

	shr	si,1			;Compute byte offset of source
	shr	si,1			;  and add to current source offset
	shr	si,1
	add	Src.OFF_lpBits,si
	mov	ax,SrcyOrg		;Get source Y origin



subttl	Phase Processing (Y)
page

;	The horizontal parameters have been calculated.  Now the vertical
;	parameters must be calculated.
;
;	Currently:
;		dx = destination Y origin
;		ax = source Y origin (destination origin if no source)
;		cl = TheFlags

Setup500:
	mov	bx,yExt 		;Get the Y extent of the BLT
	dec	bx			;Make it inclusive



;	The BLT will be Y+ if the top of the source is below or equal
;	to the top of the destination (cases: 1,4,5,7,8).  The BLT
;	will be Y- if the top of the source is above the top of the
;	destination (cases: 2,3,6)
;
;
;		  !...................!
;		  !D		      !
;	      ____!		..x   !
;	     |S   !		  :   !     Start at top of S walking down
;	     |	  !		      !
;	     |	  !...................!
;	     |			  :
;	     |____________________:
;
;
;	      __________________
;	     |S 		|
;	     |	  .....................     Start at bottom of S walking up
;	     |	  !D		      !
;	     |	  !		:     !
;	     |____!	      ..x     !
;		  !		      !
;		  !....................


	mov	ch,increase		;Set Y direction for top to bottom
	cmp	ax,dx			;Which direction do we move?
	jge	Setup520		;Step down screen (cases: 1,4,5,7,8)


;	Direction will be from bottom of the screen up (Y-)
;
;	This code will not be executed if there is no source since
;	both Y's were set to the destination Y.


	add	dx,bx			;Find bottom scan line index for
	add	ax,bx			;  destination and source
	mov	ch,decrease		;Set pattern increment

Setup520:
	mov	patRow,dl		;Set pattern row and increment
	mov	direction,ch
	sar	ch,1			;Map FF==>FF, 01==>00
	errnz	decrease-0FFFFh
	errnz	increase-00001h



;	The Y direction has been computed.  Compute the rest of the
;	Y parameters.  These include the actual starting address,
;	the scan line and plane increment values, and whether or not
;	the extents will cross a 64K boundary.
;
;	Currently:
;		dx = Y of starting destination scan
;		ax = Y of starting source scan
;		ch = BLT direction
;		       00 = increasing BLT, Y+
;		       FF = decreasing BLT, Y-
;		cl = TheFlags
;		bx = inclusive Y extent


Setup540:
	test	cl,SrcPresent		;Is a source needed?
	mov	cl,ch			;  (Want cx = +/- 1)
	jz	Setup560		;  No, skip source set-up
	push	dx			;Save destination Y
	push	bp			;Mustn't trash frame pointer
	lea	bp,Src			;--> source data structure
	call	ComputeY		;Process as needed
	pop	bp
	pop	dx			;Restore destination Y

Setup560:
	push	bp			;Mustn't trash frame pointer
	mov	ax,dx			;Put destination Y in ax
	lea	bp,Dest 		;--> destination data structure
	call	ComputeY
	pop	bp			;Restore frame pointer




subttl	Compile - Special Cases
page

;	The parameters needed for the BLT (phase alignment, directions
;	of movement, ...) have been computed and saved.  These parameters
;	will now be interpreted and a BLT created on the stack.

CBLT:
	mov	dh,TheFlags		;Will need these in a bit
	mov	al,bptr (Rop)		;Get the raster op
	test	al,epsIndx		;Can this be special cased?
	jnz	CBLT2000		;  No
	errnz	<HIGH epsIndx>
	errnz	SpecParseStrIndx	;The special case index must be 0
	mov	di,Dest.NextScan	;Special case code expects this
	test	al,epsOff		;Is this a source copy?
	jz	CBLT2000		;  Yes, go check it out
	errnz	<sourceCopy AND 11B>	;Offset for source copy must be 0
	errnz	epsOff-11B		;epsOff must be these bits


;	We should have one of the following fill operations:
;		P	- Pattern
;		Pn	- NOT pattern
;		DDx	- 0 fill
;		DDxn	- 1 fill

	mov	bl,BrushAccel		;Set this for EGASolidPat
	shl	al,1			;Is it 0 or 1 fill?
	jns	CBLT1000		;  No, must be a pattern fill
	mov	bl,BrushIsSolid+RedBit+GreenBit+BlueBit
	mov	BrushAccel,BrushIsSolid ;(no brush given for DDx or DDxn)

CBLT1000:
	or	bl,bl			;Brush accelerator set?
	js	CBLT1010		;  Yes, can special case it
	errnz	BrushIsSolid-10000000b
	test	bl,GreyScale		;Grey scale?
	jz	CBLT2000		;  No, cannot special case it
	mov	ds,SEG_lpPBrush 	;Set brush segment
	shl	al,1			;Invert the grey if needed
	cbw
	not	ah
	mov	bl,ah
	jmp	short CBLT2000

CBLT1010:
	shl	al,1			;Invert the color if needed
	cbw
	not	ah
	xor	bl,ah
	and	bl,RedBit+GreenBit+BlueBit

	errnz	   patCopy-00100001B
	errnz	NOTpatCopy-00000001B
	errnz	 FillBlack-01000010B
	errnz	 FillWhite-01100010B



subttl	Compile - Allocate
page

;	Allow room for the BLT code.  The maximum that can be generated
;	is defined by the variable maxBLTsize.	This variable must be
;	an even number.

	assumes cs,code
	assumes ds,nothing
	assumes es,nothing
	assumes ss,nothing


CBLT2000:
	mov	ax,maxBLTsize+20h	;See if room on stack
	call	dmCheckStack
	jnc	CBLT2001		;There was room
	jmp	exit_fail		;There was no room

CBLT2001:
	add	sp,20h			;Take off the slop

	mov	di,sp
	mov	OFF_BLTaddr,di		;Save the address for later
	mov	ax,ss			;Set the segment for the BLT
	mov	es,ax
	mov	SEG_BLTaddr,ax		;Save the address for later
	mov	ax,cs			;Set data seg to CS so we can access
	mov	ds,ax			;  code without overrides
	xor	cx,cx			;Clear out count register



subttl	Compile - Outer Loop, Plane Selection
page

;	Create the outerloop code.  The first part of this code will save
;	the scan line count register, destination pointer, and the source
;	pointer (if there is a source).  If the destination device is color
;	and the display is involved in the blt, then the color plane selection
;	logic must be added in.  If the destination is monochrome, then no
;	plane logic is needed.	 Two color memory bitmaps will not cause the
;	plane selection logic to be copied.
;
;
;	The generated code should look like:
;
;		push	cx		;Save scan line count
;		push	di		;Save destination pointer
;	<	push	si	>	;Save source pointer
;	<	push	bx	>	;Save plane index
;	<	plane selection >	;Select plane


	mov	bl,TheFlags
	mov	ax,I_pushCX_pushDI	;Save scan line count, destination ptr
	stosw
	test	bl,SrcPresent		;Is a source needed?
	jz	CBLT2020		;  No
	mov	al,I_pushSI		;  Yes, save source pointer
	stosb

CBLT2020:
	test	bl,DestIsColor		;Is the destination color?
	jz	CBLT2040		;  No
	mov	al,I_pushBX		;Save plane index
	stosb


subttl	Compile - Pattern Fetch
page

;	Set up any pattern fetch code that might be needed.
;	The pattern code has many fixups, so it isn't taken from a
;	template.  It is just stuffed as it is created.  The address
;	of the increment for the brush is saved for the plane looping
;	logic if the destination is a color device.
;
;	Entry:	None
;
;	Exit:	dh = pattern
;
;	Uses:	ax,bx,cx,dh,flags
;
;	The pattern fetch code will look like:
;
;	For monochrome brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;ds:bx --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,cs:[1234h]	;Get brush index
;	    add     al,direction	;Add displacement to next byte (+1/-1)
;	    and     al,00000111B	;Keep it in range
;	    mov     cs:[1234h],al	;Store displacement to next byte
;	    mov     ds,cx		;Restore ds
;
;
;	For color brushes:
;
;	    mov     ax,1234h		;Load segment of the brush
;	    mov     bx,1234h		;Load offset of the brush
;	    mov     cx,ds		;Save DS
;	    mov     ds,ax		;ds:bx --> brush
;	    mov     dh,7[bx]		;Get next brush byte
;	    mov     al,cs:[1234h]	;Get brush index
;	    add     al,SIZE Pattern	;Add displacement to next plane's bits
;	    and     al,00011111B	;Keep it within the brush
;	    mov     cs:[1234h],al	;Store displacement to next plane's bits
;	    mov     ds,cx		;Restore ds


CBLT2040:
	test	bl,PatPresent		;Is a pattern needed?
	jz	CBLT3000		;  No, skip pattern code

	mov	al,I_movAXwordI 	;mov ax,SEG_lpPBrush
	stosb
	mov	ax,SEG_lpPBrush
	stosw
	mov	al,I_movBXwordI 	;mov bx,OFF_lpPBrush
	stosb
	mov	ax,OFF_lpPBrush
	stosw
	mov	ax,I_movCX_DS		;mov cx,ds
	stosw
	mov	ax,I_movDS_AX		;mov ds,ax
	stosw
	mov	ax,I_movDH_BX_Disp8	;mov dh,PatRow[bx]
	stosw
	mov	dx,di			;Save address of the brush index
	mov	al,PatRow		;Set initial pattern row
	mov	bh,00000111b		;Set brush index mask
	and	al,bh			;Make sure it's legal at start
	stosb
	mov	ax,I_csOverride+(I_movAL_Mem*256)
	stosw				;mov al,cs:[xxxx]
	mov	ax,dx
	stosw
	mov	al,I_addALByteI
	mov	ah,direction		;Set brush index
	errnz	increase-1		;Must be a 1
	errnz	decrease+1		;Must be a -1
	test	bl,ColorPat		;If color pattern required
	jz	CBLT2060		;  (is not color)
	mov	ah,SizePattern		;  set increment to next plane
	mov	AddrBrushIndex,dx	;  save address of brush index
	mov	bh,00011111b		;  set brush index mask

CBLT2060:
	stosw
	mov	ah,bh			;and al,BrushIndexMask
	mov	al,I_andALbyteI
	stosw
	mov	ax,I_csOverride+(I_movMem_AL*256)
	stosw				;mov cs:[xxxx],al
	mov	ax,dx
	stosw
	mov	ax,I_movDS_CX		;mov ds,cx
	stosw



subttl	Compile - Initial Byte Fetch
page

;	Create the initial byte code.  This may consist of one or two
;	initial fetches (if there is a source), followed by the required
;	logic action.  The code should look something like:
;
;	BLTouterloop:
;	<	mov	bp,maskP    >	;Load phase mask for entire loop
;	<	xor	bh,bh	    >	;Clear previous unused bits
;
;	;	Perform first byte fetch
;
;	<	lodsb		    >	;Get source byte
;	<	color<==>mono munge >	;Color <==> mono conversion
;	<	phase alignment     >	;Align bits as needed
;
;	;	If an optional second fetch is needed, perform one
;
;	<	lodsb		    >	;Get source byte
;	<	color to mono munge >	;Color to mono munging
;	<	phase alignment     >	;Align bits as needed
;
;		logical action		;Perform logical action required
;
;		mov	ah,es:[di]	;Get destination
;		and	ax,cx		;Saved unaltered bits
;		or	al,ah		;  and mask in altered bits
;		stosb			;Save the result
;
;
;	The starting address of the first fetch/logical combination will be
;	saved so that the code can be copied later instead of recreating it
;	(if there are two fecthes, the first fetch will not be copied)
;
;	The length of the code up to the masking for altered/unaltered bits
;	will be saved so the code can be copied into the inner loop.


CBLT3000:
	xor	dx,dx
	or	dh,phaseH		;Is the phase 0? (also get the phase)
	jz	CBLT3020		;  Yes, so no phase alignment needed
	mov	al,I_movBPwordI 	;Set up the phase mask
	stosb
	mov	ax,maskP		;Place the mask into the instruction
	stosw
	mov	ax,I_xorBH_BH		;Clear previous unused bits
	stosw

CBLT3020:
	mov	StartFL,di		;Save starting address of action
	or	dl,nOps.OpSrc		;Is there a source?
	jnz	CBLT3040		;  Yes, generate fetch code
	jmp	CBLT4000		;  No, don't generate fetch code



;	Generate the required sequence of instructions for a fetch
;	sequence.  Only the minimum code required is generated.
;
;	The code generated will look something like the following:
;
;	BLTfetch:
;	<	lodsb		      > ;Get the next byte
;	<	color munging	      > ;Mono <==> color munging
;
;
;	;	If the phase alignment isn't zero, then generate the minimum
;	;	phase alignment needed.  RORs or ROLs will be generated,
;	;	depending on the fastest sequence.  If the phase alignment
;	;	is zero, than no phase alignment code will be generated.
;
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	ror	al,1	      > ;Rotate as needed
;	<	mov	ah,al	      > ;Mask used, unused bits
;	<	and	ax,bp	      > ;(bp) = phase mask
;	<	or	al,bh	      > ;Mask in old unused bits
;	<	mov	bh,ah	      > ;Save new unused bits
;
;
;	The nice thing about the above is it is possible for the fetch to
;	degenerate into a simple LODSB instruction.
;
;	If this was a iAPX80286 implementation, if would be faster to
;	make three or four rotates into a "ror al,n" instruction.
;
;	Currently:	bl = TheFlags


CBLT3040:
	shl	bl,1			;Color conversion?
	jc	CBLT3060		;  Yes, gag and choke on it
	jmp	CBLT3120		;  No, we were lucky this time
	errnz	GagChoke-10000000b

CBLT3060:
	mov	MooreFlags,0		;Assume REP cannot be used
	lds	si,lpDrawMode		;--> background color
	lea	si,bkColor[si]		;  (lea preserves the flags)
	js	CBLT3100		;Mono ==> color
	errnz	ColorPat-01000000b



subttl	Compile - Initial Byte Fetch, Color ==> Mono
page

;	Generate the code to go from color to mono.  Color to mono
;	should map all colors that are background to 1's (white), and
;	all colors which aren't background to 0's (black).  If the source
;	is the display, then the color compare register will be used.
;	If the source is a memory bitmap, each byte of the plane will be
;	XORed with the color from that plane, with the results all ORed
;	together.  The final result will then be complemented, giving
;	the desired result.
;
;	The generated code for bitmaps should look something like:
;
;	    mov     al,PlaneW[si]	;Get green byte of source
;	    mov     ah,2*PlaneW[si]	;Get blue  byte of source
;	    xor     ax,Green+(Blue*256) ;XOR with plane's color
;	    or	    ah,al		;OR the result
;	    lodsb			;Get red source
;	    xor     al,RedColor 	;XOR with red color
;	    or	    al,ah		;OR with previous result
;	    not     al			;NOT to give 1's where background
;
;
;	    where PlaneW is defined to be:
;
;		a)  bmWidthPlanes for bitmaps <64K
;		b)  bmWidthBytes  for bitmaps >64K


;	The source is a memory bitmap.	Generate the code to compute
;	the result of the three planes:
;
;	    mov     al,PlaneW[si]	;Get green byte of source
;	    mov     ah,2*PlaneW[si]	;Get blue  byte of source
;	    xor     ax,Green+(Blue*256) ;XOR with plane's color
;	    or	    ah,al		;OR the result
;	    lodsb			;Get red source
;	    xor     al,RedColor 	;XOR with red color
;	    or	    al,ah		;OR with previous result
;	    not     al			;NOT to give 1's where background
;


	mov	ax,I_movAL_SI_Disp16
	stosw
	mov	ax,Src.PlaneW
	stosw
	mov	bx,ax
	add	bx,bx
	mov	ax,I_movAH_SI_Disp16
	stosw
	mov	ax,bx
	stosw
	mov	al,I_xorAXwordI
	stosb
	mov	ax,wp green[si]
	stosw
	errnz	Blue-Green-1
	mov	ax,I_orAH_AL
	stosw
	mov	ax,I_lodsb+(I_xorALbyteI*256)
	stosw
	movsb
	errnz	red
	mov	ax,I_orAL_AH
	stosw
	mov	ax,I_notAL
	stosw
	jmp	short CBLT3140		;Go create logic code



subttl	Compile - Initial Byte Fetch, Mono ==> Color
page

;	The conversion is mono to color.  Generate the code to
;	do the conversion, and generate the table which will
;	have the conversion values in it.
;
;	When going from mono to color, 1 bits are considered to be
;	the background color, and 0 bits are considered to be the
;	foreground color.
;
;	For each plane:
;
;	  If the foreground=background=1, then 1 can be used in
;	  place of the source.
;
;	  If the foreground=background=0, then 0 can be used in
;	  place of the source.
;
;	  If the foreground=0 and background=1, then the source
;	  can be used as is.
;
;	  If the foreground=1 and background=0, then the source
;	  must be complemented before using.
;
;	  Looks like a boolean function to me.
;
;	An AND mask and an XOR mask will be computed for each plane,
;	based on the above.  The source will then be processed against
;	the table.  The generated code should look like
;
;		lodsb
;		and	al,ss:[xxxx]
;		xor	al,ss:[xxxx+1]
;
;
;	The table for munging the colors as stated above should look like:
;
;	     BackGnd   ForeGnd	  Result    AND  XOR
;		1	  1	    1	     00   FF
;		0	  0	    0	     00   00
;		1	  0	    S	     FF   00
;		0	  1	not S	     FF   FF
;
;	From this, it can be seen that the XOR mask is the same as the
;	foreground color.  The AND mask is the XOR of the foreground
;	and the background color.  Not too hard to compute
;
;
;	It can also be seen that if the background color is white and the
;	foreground (text) color is black, then the conversion needn't be
;	generated (it just gives the source).  This is advantagous since
;	it will allow phased aligned source copies to use REP MOVSW.
;
;
;	Currently:	ds:si --> bkColor in lpDrawMode


	errnz	TextColor-bkColor-4	;Must be continguous

CBLT3100:
	mov	ah,Mono[si]		;Get background color	(white)
	xor	ah,MonoBit+RedBit+GreenBit+BlueBit
	or	ah,Mono+4[si]		;Get foreground color	(black)
	jnz	CBLT3110		;Not black
	mov	MooreFlags,RepOK+NoMunge;Show reps as ok, no color munge table
	jmp	short CBLT3120		;Normal fetch required



;	No way around it.  The color conversion table and code
;	must be generated.

CBLT3110:
	mov	ah,4[si]		;Get foreground Red color
	lodsb				;Get background Red color
	xor	al,ah
	mov	wp (red*2).ABrush,ax
	mov	ah,4[si]		;Get foreground Green color
	lodsb				;Get background Green color
	xor	al,ah
	mov	wp (Green*2).ABrush,ax
	mov	ah,4[si]		;Get foreground Blue color
	lodsb				;Get background Blue color
	xor	al,ah
	mov	wp (Blue*2).ABrush,ax



;	Generate the code for munging the color as stated above.

	mov	ax,I_lodsb+(I_SSoverride*256)
	stosw				;lodsb
	mov	ax,I_andAL_Mem		;and al,ss:[xxxx]
	stosw
	lea	ax,ABrush		;  Set address of color munge
	stosw
	mov	bx,ax			;  Save address
	mov	al,I_SSoverride 	;cs:
	stosb
	mov	ax,I_xorAL_Mem		;xor al,[xxxx]
	stosw
	lea	ax,1[bx]		;  Set address of XOR mask
	stosw
	jmp	short CBLT3140



subttl	Compile - Phase Alignment
page

;	Just need to generate the normal fetch sequence (lodsb)

CBLT3120:
	mov	al,I_lodsb		;Generate source fetch
	stosb


;	Generate any phase alignment

CBLT3140:
	mov	ax,cs			;Just incase it was trashed by
	mov	ds,ax			;  color conversion code
	xor	cx,cx			;Might have garbage in it
	or	dh,dh			;Any phase alignment?
	jz	CBLT3180		;  No, so skip alignment
	mov	cl,dh			;Get horizontal phase for rotating
	mov	ax,I_rolAL_1		;Assume rotate left n times
	cmp	cl,5			;4 or less rotates?
	jc	CBLT3160		;  Yes
	neg	cl			;  No, compute ROR count
	add	cl,8
	mov	ah,HIGH I_rorAL_1
	errnz	<(LOW I_rolAL_1)-(LOW I_rorAL_1)>

CBLT3160:
	rep	stosw			;Stuff the phase alignment rotates
					;  then the phase alignment code
	mov	si,codeOFFSET phaseAlign
	mov	cl,(phaseAlignLen SHR 1)
	rep	movsw
	if	phaseAlignLen AND 1
	movsb
	endif

CBLT3180:
	dec	firstFetch		;Generate another fetch?
	jz	CBLT4000		;  No

;	A second fetch needs to be stuffed.  Copy the one just created.
	mov	si,di			;Get start of fetch logic
	xchg	si,StartFL		;Set new start, get old
	mov	cx,di			;Compute how long fetch is
	sub	cx,si			;  and move the bytes
	mov	ax,es
	mov	ds,ax
	rep	movsb



subttl	Compile - ROP Generation
page

;	Create the logic action code
;
;	The given ROP will be converted into the actual code that
;	performs the ROP.




srcInAL 	equ	00000001B	;Source field is in AL		(0)
DestInAH	equ	00000010B	;Destination field is in AH	(1)
PushPopFlag	equ	00000100B	;Next push/pop is a pop 	(1)



CBLT4000:
	xor	dh,dh			;Set initial flags for this code


;	If the ROP happens to be a source copy, then the entire ROP
;	generation can be skipped since the source is already in AL
;	and no operations will be performed on it.


	mov	cx,OFF_Rop		;Get the raster op
	cmp	cx,sourceCopy		;If the raster op is SourceCopy,
	jnz	CBLT4020		;  (isn't SourceCopy)
	jmp	CBLT4260		;  skip the code generation



;	The ROP isn't a source copy.  A little preprocessing will be done.
;
;	For the source:
;
;	    If more than one copy of the source is needed, the source
;	    in AL will be saved in DL.
;
;	    If only one copy of the source is needed, and it isn't
;	    the first operand, the source will be saved in DL.
;
;	    If only one copy of the source is needed and it is the
;	    first operand, it is already in AL from the fetch logic,
;	    so nothing need be done.
;
;
;	For the destination:
;
;	    If only one copy of the destination is needed, it will
;	    be fetched from memory when required.
;
;	    If more than one copy of the destination is required,
;	    it will be loaded into AH and used from there.



CBLT4020:
	shr	cx,1			;Raster op must be pre-rotated
	shr	cx,1			;  twice for entry into create loop
	mov	si,operands		;Get the operand string
	errnz	LogOp1-00C0H		;  (this is assumed)
	mov	dl,nLogOps		;Get number of operators

	cmp	nOps.OpSrc,1		;How many sources are needed?
	jc	CBLT4060		;  None needed
	jne	CBLT4040		;  At least two needed
	mov	ax,si			;  Only one, is it the first operand?
	and	al,11B
	cmp	al,OpSrc
	jz	CBLT4060		;Source is first operand

CBLT4040:
	mov	ax,I_movDL_AL		;Save source in a register
	stosw

CBLT4060:
	cmp	nOps.OpDest,2		;Destination used more than once?
	jc	CBLT4080		;  No, leave it in memory
	mov	al,I_esOverride 	;  Yes, load destination into a reg
	stosb
	mov	ax,I_movAH_dest
	stosw
	or	dh,DestInAH		;Show that the destination is in AH



;	This is the start of the actual ROP generator.	This code is
;	entered either for the very first operand or after a "push".
;	The next operand will be loaded into AL.
;
;	If this is the first time, then it is possible for the first
;	operand to be a "PUSH" if an invalid raster op was given.  If
;	this is the case and real PUSHES are being performed, then abort
;	since the raster op isn't one of the published ones and there is
;	no guarantee that a POP will be performed.
;
;	If we are here because a "PUSH" was just performed, then the next
;	operand cannot be a POP since none of the strings were generated
;	with sequential push/pops.
;
;	Currently:
;
;		si = parse string
;		     D1:D0 = next operand
;		cx = ROP
;		     D3:D2 = next operator
;		dl = # operators left
;		dh = flags: srcInAL, DestInAH, PushPopFlag


CBLT4080:
	mov	ax,si			;Mask next operand
	and	ax,11B
;	jnz	CBLT4100		;Is not a push, it's ok
;	jmp	CBLT6280		;is a push, ABORT

CBLT4100:
	dec	ax			;Is operand a source?
	jnz	CBLT4120		;  No, must be destination or pattern
	test	dh,SrcInAL		;  Yes, is source in AL now?
	jz	CBLT4240		;    Yes, do nothing
	and	dh,NOT SrcInAL		;    No, show source is in AL
	mov	ax,I_movAL_DL		;      and move source into AL
	jmp	short CBLT4220		;

CBLT4120:
	dec	ax			;Is operand the destination?
	mov	ax,I_movAL_DH		;  (assume operand is pattern)
	jnz	CBLT4200		;This is P, load pattern into AL
	mov	ax,I_movAL_AH		;Assume destination is in AH
	test	dh,DestInAH		;Is destination in AH?
	jnz	CBLT4200		;  Yes, load it into AL
	mov	al,I_esOverride 	;  No, load destination from memory
	stosb
	mov	ax,I_movAL_dest
	jmp	short CBLT4200



;	This is the loop portion of the ROP generation.  The current
;	operand is in AL.  Get the next operator and apply it to AL
;	if unary (NOT) or against AL and the next operand if binary
;	(AND, OR, XOR).
;
;	If the next operand is a push or pop, then extra work will
;	be required.


CBLT4140:
	shr	cx,1			;Get next operator
	shr	cx,1
	mov	bx,cx
	and	bx,000CH
	mov	ax,I_notAL		;Assume operator is "NOT"
	jz	CBLT4200		;Operator is a NOT

	ror	si,1			;Binary operator.  Get next operand
	ror	si,1
	mov	ax,si
	and	ax,11B			;Mask operand and test for pop/push
	jnz	CBLT4160		;It is source, pattern, or destination


;	The operator is binary and a push or pop is required.
;
;	OpTbl was layed out so that pops could use the normal operand
;	code.  OpSpec is 00b, and the binary operators ending with
;	00B are applied against the pushed value".
;
;	If this is a push, more work needs to be done.	The code to
;	push the current result will be generated.  The next operand
;	will be set, and the current binary operator restored.	The
;	initialize code will then be entered to load AL with a new
;	value.


	xor	dh,PushPopFlag		;Toggle the push/pop flag
	test	dh,PushPopFlag		;Is this a pop?
	jz	CBLT4160		;  Yes
	mov	ax,I_movBL_AL		;  No, perform a "PUSH"
	stosw
	shl	cx,1			;Restore this operator
	shl	cx,1
	ror	si,1			;Rotate in next operand
	ror	si,1


;	Bias the operator count since loading another value into
;	AL via CBLT4080 will decrement this counter (if this wasn't
;	done, then the count would be one less than it should be,
;	thus dropping the very last operator).


	inc	dl			;Bias operator count
	jmp	CBLT4080		;Get new operand



;	Normal binary operation.  If the destination is the current
;	operand and it isn't in AH, generate the code to do the
;	operation with es:[di], else generate the code to do the
;	operation with AH.


CBLT4160:
	or	bx,ax			;Generate operation table index/2
	cmp	ax,OpDest		;Is operand the destination?
	jnz	CBLT4180		;  No, table pointer is valid
	test	dh,DestInAH		;  Yes, Is it in a register (AL)
	jnz	CBLT4180		;    Yes, table pointer is valid
	mov	al,I_esOverride 	;    No, work from memory
	stosb
	shr	bx,1			;Make pointer point to memory operation
	shr	bx,1

CBLT4180:
	add	bx,bx			;Table entries are words
	mov	ax,cs:[bx].OpTbl	;Get the correct instruction from table

CBLT4200:
	or	dh,SrcInAL		;Show source is no longer in AL

CBLT4220:
	stosw				;Save the current instruction

CBLT4240:
	dec	dl			;More operators?
	jnl	CBLT4140		;  Yes, continue processing

CBLT4260:
	mov	endFL,di		;Save end of fetch/logic operation



subttl	Compile - Mask And Save
page

;	Generate code to mask and save the result.  If the destination
;	isn't in a register, it will be loaded from es:[di] first.  The
;	mask operation will then be performed, and the result stored.


	test	dh,DestInAH		;Is destination in AH?
	jnz	CBLT4280		;  Yes, don't load it into AH
	mov	al,I_esOverride 	;  No, load it into AH
	stosb
	mov	ax,I_movAH_dest
	stosw

CBLT4280:
	mov	ax,cs
	mov	ds,ax
	mov	si,codeOFFSET maskedStore ;Move rest of masked store template
	movsw
	movsw
	movsw
	errnz	maskedStoreLen-6	;Must be six bytes long
	mov	ax,startMask		;Stuff start mask into
	xchg	ah,al			;  the template
	mov	es:maskedStoreMask[di],ax
	mov	endFLS,di		;Save end of fetch/logic/store operation



subttl	Compile - Inner Loop Generation
page

;	Now for the hard stuff; The inner loop (said with a "gasp!").
;
;	If there is no innerloop, then no code will be generated
;	(now that's fast!).

CBLT5000:
	mov	ax,es			;Set ds: to es: since code will be
	mov	ds,ax			;  copied from/to the stack
	mov	dx,innerloopCnt 	;Get the loop count
	or	dx,dx			;If the count is null
;	jz	CBLT6000
	jz	CBLT5140		;  don't generate any code.



;	We have something for a loop count.  If this just happens to be
;	a source copy (S) with a phase of zero, then the innerloop degenerates
;	to a repeated MOVSB instruction.  This little special case is
;	worth checking for and handling!
;
;	Also, if this is one of the special cases {P, Pn, DDx, DDxn}, then it
;	will also be special cased since these are all pattern fills (pattern,
;	not pattern, 0, 1).
;
;	The same code can be shared for these routines, with the exception
;	that patterns use a STOSx instruction instead of a MOVSx instruction
;	and need a value loaded in AX
;
;
;	So we lied a little.  If a color conversion is going on, then the
;	REP MOVSB might not be usable.	If the RepOK flag has been set, then
;	we can use it.	The RepOK flag will be set for a mono ==> color
;	conversion where the background color is white and the foreground
;	color is black, or for a color ==> mono conversion with the screen
;	as the source (the color compare register will be used).
;
;	For the special cases {P, Pn, DDx, DDxn}, color conversion is
;	not possible, so ignore it for them.


	mov	bl,bptr (Rop)		;Get the raster op
	test	bl,epsIndx		;Can this be special cased?
	jnz	CBLT5500		;  No
	errnz	<HIGH epsIndx>
	errnz	SpecParseStrIndx	;The special case index must be 0

	test	bl,epsOff		;Is this a source copy
	jz	CBLT5040		;  Yes
	errnz	<sourceCopy AND 11B>	;Offset for source copy must be 0



;	We should have one of the following fill operations:
;
;		P	- Pattern
;		Pn	- NOT pattern
;		DDx	- 0 fill
;		DDxn	- 1 fill


	mov	ax,I_movAL_0FFH 	;Assume this is a 0 or 1 fill
	test	bl,01H			;Is it 0 or 1 fill?
	jz     CBLT5020 		;  Yes, initialize AX with 0FFH
	mov	ax,I_movAL_DH		;  No,	initialize AX with pattern

	errnz	   patCopy-0000000000100001B
	errnz	NOTpatCopy-0000000000000001B
	errnz	 FillBlack-0000000001000010B
	errnz	 FillWhite-0000000001100010B

CBLT5020:
	stosw
	mov	ax,I_movAH_AL
	stosw
	mov	si,I_stosb		;Set up for repeated code processor
	test	bl,LogPar		;If Pn or 0, then complement pattern
	jnz	CBLT5060		;  Is just P or 1
	errnz	<HIGH LogPar>
	mov	ax,I_notAX		;  Is Pn or 0, complement AX
	stosw
	jmp	short CBLT5060

	errnz	   patCopy-00100001B
	errnz	NOTpatCopy-00000001B
	errnz	 FillBlack-01000010B
	errnz	 FillWhite-01100010B




;	This is a source copy.	The phase must be zero for a source copy
;	to be condensed into a REP MOVSx.  Also, for a color conversion,
;	RepOK must be set.

CBLT5040:
	test	phaseH,0FFH		;Is horizontal phase zero?
	jnz	CBLT5500		;  No, can't condense source copy
	mov	si,I_movsb		;Set register for moving bytes
	test	TheFlags,GagChoke	;Color conversion?
	jz	CBLT5060		;  No, rep is OK to use
	test	MooreFlags,RepOK	;  Yes, can we rep it?
	jz	CBLT5500		;    No, do it the hard way



;	This is a source copy or pattern fill.	Process an odd byte with
;	a MOVSB or STOSB, then process the rest of the bytes with a REP
;	MOVSW or a REP STOSW.  If the REP isn't needed, leave it out.
;
;	Don't get caught on this like I did!  If the direction of the
;	BLT is from right to left (decrementing addresses), then both
;	the source and destination pointers must be decremented by one
;	so that the next two bytes are processed, not the next byte and
;	the byte just processed.  Also, after all words have been processed,
;	the source and destination pointers must be incremented by one to
;	point to the last byte (since the last MOVSW or STOSW would have
;	decremented both pointers by 2).
;
;	If the target machine is an 8086, then it would be well worth the
;	extra logic to align the fields on word boundaries before the MOVSxs
;	if at all possible.
;
;	The generated code should look something like:
;
;	WARP8:				     ;This code for moving left to right
;		movsb			     ;Process an odd byte
;		ld	cx,innerLoopCnt/2    ;Set word count
;		rep			     ;If a count, then repeat is needed
;		movsw			     ;Move words until done
;
;
;	WARP8:				     ;This code for moving left to right
;		movsb			     ;Process an odd byte
;		dec	si		     ;adjust pointer for moving words
;		dec	di
;		ld	cx,innerLoopCnt/2    ;Set word count
;		rep			     ;If a count, then repeat is needed
;		movsw			     ;Move words until done
;		inc	si		     ;adjust since words were moved
;		inc	di
;
;
;	Of course, if any part of the above routine isn't needed, it isn't
;	generated (i.e. the generated code might just be a single MOVSB)

CBLT5060:
	shr	dx,1			;Byte count / 2 for words
	jnc	CBLT5080		;  No odd byte to move
	mov	ax,si			;  Odd byte, move it
	stosb
CBLT5080:
	jz	CBLT5140		;No more bytes to move
	xor	bx,bx			;Flag as stepping from left to right
	cmp	bl,stepDirection	;Moving from the right to the left?
	errnz	stepleft		;  (left direction must be zero)
	jnz	CBLT5100		;  No
	mov	ax,I_decSI_decDI	;  Yes, decrement both pointers
	stosw
	mov	bx,I_incSI_incDI	;Set up to increment the pointers later

CBLT5100:
	cmp	dx,1			;Move one word or many words?
	jz	CBLT5120		;  Only one word
	mov	al,I_movCXwordI 	;  Many words, load count
	mov	ah,dl
	stosw
	mov	al,dh			;Set MSB of count
	mov	ah,I_rep		;  and a repeat instruction
	stosw

CBLT5120:
	mov	ax,si			;Set the word instruction
	inc	ax
	stosb
	errnz	I_movsw-I_movsb-1	;The word form of the instruction
	errnz	I_stosw-I_stosb-1	;  must be the byte form + 1

	or	bx,bx			;Need to increment the pointers?
	jz	CBLT5140		;  No
	mov	ax,bx			;  Yes, increment both pointers
	stosw

CBLT5140:
	jmp	short CBLT6000		;Done setting up the innerloop
page

;	There is some count for the innerloop of the BLT.  Generate the
;	required BLT. Two or four copies of the BLT will be placed on the
;	stack.	 This allows the LOOP instruction at the end to be distributed
;	over two or four bytes instead of 1, saving 11 or 12 clocks for each
;	byte (for 4).  Multiply 12 clocks by ~ 16K and you save a lot of
;	clocks!
;
;	If there are less than four (two) bytes to be BLTed, then no looping
;	instructions will be generated.  If there are more than four (two)
;	bytes, then there is the possibility of an initial jump instruction
;	to enter the loop to handle the modulo n result of the loop count.
;
;	The innerloop code will look something like:
;
;
;	<	mov	cx,loopcount/n> ;load count if >n innerloop bytes
;	<	jmp	short ???     > ;If a first jump is needed, do one
;
;	BLTloop:
;		replicate initial byte BLT code up to n times
;
;	<	loop	BLTloop >	;Loop until all bytes processed


CBLT5500:
	mov	bx,endFL		;Compute size of the fetch code
	sub	bx,startFL
	inc	bx			;A stosb will be appended
	mov	si,4			;Assume replication 4 times
	mov	cl,2			;  (shift count two bits left)
	cmp	bx,32			;Small enough for 4 times?
	jc	CBLT5520		;  Yes, replicate 4 times
	shr	si,1			;  No,	replicate 2 times
	dec	cx

CBLT5520:
	cmp	dx,si			;Generate a loop?
	jle	CBLT5540		;  No, just copy code
	mov	al,I_movCXwordI
	stosb				;mov cx,loopcount/n
	mov	ax,dx			;Compute loop count
	shr	ax,cl
	stosw
	shl	ax,cl			;See if loopcount MOD n is 0
	sub	ax,dx
	jz	CBLT5540		;Zero, no odd count to handle


;	There is an odd portion of bytes to be processed.  Increment
;	the loop counter for the odd pass through the loop and then
;	compute the displacement for entering the loop.
;
;	To compute the displacement, subtract the number of odd bytes
;	from the modulus being used  (i.e. 4-3=1).  This gives the
;	number of bytes to skip over the first time through the loop.
;
;	Multiply this by the number of bytes for a logic sequence,
;	and the result will be the displacement for the jump.


	inc	wp es:-2[di]		;Not zero, adjust for partial loop
	add	ax,si			;Compute where to enter the loop at
	mul	bl
	mov	cx,ax
	mov	al,I_JMPnear		;Stuff jump instruction
	stosb
	mov	ax,cx			;Stuff displacement for jump
	stosw



;	Currently:	dx = loop count
;			si = loop modulus
;			bx = size of one logic operation
;			di --> next location in the loop

CBLT5540:
	mov	cx,bx			;Set move count
	mov	bx,dx			;Set maximum for move
	cmp	bx,si			;Is the max > what's left?
	jle	CBLT5560		;  No, just use what's left
	mov	bx,si			;  Yes, copy the max

CBLT5560:
	sub	dx,si			;If dx > 0, then loop logic needed
	mov	si,startFL		;--> fetch code to copy
	mov	ax,cx			;Save a copy of fetch length
	rep	movsb			;Move fetch code and stuff stosb
	mov	si,di			;--> new source (and top of loop)
	sub	si,ax
	mov	bptr es:-1[di],I_stosb
	dec	bl			;One copy has been made
	mul	bl			;Compute # bytes left to move
	mov	cx,ax			;Set move count
	rep	movsb			;Move the fetches
	sub	si,ax			;Restore pointer to start of loop


;	The innermost BLT code has been created and needs the looping
;	logic added to it.  If there is any looping to be done, then
;	generate the loop code.  The code within the innerloop may be
;	greater than 126 bytes, so a LOOP instruction may not be used
;	in this case.

CBLT5580:
	or	dx,dx			;Need a loop?
	jle	CBLT6000		;  No, don't generate one

	mov	ax,si			;Compute offset of loop
	sub	ax,di
	cmp	ax,-125 		;Can this be a short label?
	jc	CBLT5600		;  No, must make it a near jmp

	sub	al,2			;Bias offset by length of LOOP inst.
	mov	ah,al
	mov	al,I_LOOP
	stosw				;Set the loop instruction
	jmp	short CBLT6000		;Go process the last byte code


CBLT5600:
	mov	si,codeOFFSET jmpCXnz	;Move in the dec CX jnz code
	movs	wp es:[di],wp cs:[si]
	movs	wp es:[di],wp cs:[si]
	errnz	jmpCXnzLen-4		;Must be four bytes long
	sub	ax,6			;Adjust jump bias
	stosw				;  and store it into jump



subttl	Compile - Last Byte Processing
page

;	All the innerloop stuff has been processed.  Now generate the code for
;	the final byte if there is one.  This code is almost identical to the
;	code for the first byte except there will only be one fetch (if a
;	fetch is needed at all).
;
;	The code generated will look something like:
;
;	<	fetch		>	;Get source byte
;	<	align		>	;Align source if needed
;		action			;Perform desired action
;		mask and store

CBLT6000:
	mov	dx,lastMask		;Get last byte mask
	or	dh,dh			;Is there a last byte to be processed?
	jz	CBLT6100		;  No.

	mov	cx,endFLS		;Get end of fetch/logic/store operation
	mov	si,startFL		;Get start of fetch/logic sequence
	sub	cx,si			;Compute length of the code
	rep	movsb			;Copy the fetch/action/store code
	xchg	dh,dl
	mov	maskedStoreMask[di],dx	;Stuff last byte mask into the code




subttl	Compile - Looping Logic
page

;	Looping logic.
;
;	The looping logic must handle monochrome bitmaps, color bitmaps,
;	huge bitmaps, the device, the presence or absence of a source
;	or pattern, and mono <==> color interactions.
;
;	The type of looping logic is always based on the destination.
;
;
;	Plane Update Facts:
;
;	1)  If the destination device is color, then there will be
;	    logic for plane selection.	Plane selection is performed
;	    at the start of the loop for the display.  Plane selection
;	    for bitmaps is performed at the end of the loop in anticipation
;	    of the next plane.
;
;
;	    The following applies when the destination is color:
;
;
;	    a)	The destination update consists of:
;
;		1)  If the destination is the display, the next plane will
;		    be selected by the plane selection code at the start
;		    of the scan line loop.
;
;		2)  If not the display, then the PDevice must a bitmap.
;		    The next plane will be selected by updating the
;		    destination offset by the PlaneW value.
;
;
;	    b)	If GagChoke isn't specified, then there may be a source.
;		If there is a source, it must be color, and the update
;		consists of:
;
;		1)  If the source is the display, the next plane will be
;		    selected by the plane selection code at the start of
;		    the loop.
;
;		2)  If not the display, then the PDevice must a bitmap.
;		    The next plane will be selected by updating the
;		    destination offset by the PlaneW value.
;
;
;	    c)	If GagChoke is specified, then the source must be a
;		monochrome bitmap which is undergoing mono to color
;		conversion.  The AND & XOR mask table which is used
;		for the conversion will have to be updated, unless
;		the NoMunge flag is set indicating that the color
;		conversion really wasn't needed.
;
;		The source's pointer will not be updated.  It will
;		remain pointing to the same scan of the source until
;		all planes of the destination have been processed.
;
;
;	    d)	In all cases, the plane mask rotation code will be
;		generated.  If the plane indicator doesn't overflow,
;		then start at the top of the scan line loop for the
;		next plane.
;
;		If the plane indicator overflows, then:
;
;		    1)	If there is a pattern present, it's a color
;			pattern fetch.	The index of which scan of
;			the brush to use will have to be updated.
;
;		    2)	Enter the scan line update routine
;
;
;	2)	If the destination is monochrome, then there will be no
;		plane selection logic.
;
;		If GagChoke is specified, then color ==> mono conversion
;		is taking place.  Any plane selection logic is internal
;		to the ROP byte fetch code.  Any color brush was pre-
;		processed into a monochrome brush, so no brush updating
;		need be done



subttl	Looping Logic - Plane Selection
page

;	Get saved parameters off of the stack.
;
;	<	pop	bx	      > ;Get plane indicator
;	<	pop	si	      > ;Get source pointer
;		pop	di		;Get destination pointer
;		pop	cx		;Get loop count


CBLT6100:
	mov	ax,cs			;Reset ds: back to cs:
	mov	ds,ax
	mov	bh,TheFlags		;These flags will be used a lot
	test	bh,DestIsColor		;Is the destination color?
	jz	CBLT6120		;  No
	mov	al,I_popBX		;Restore plane index
	stosb

CBLT6120:
	test	bh,SrcPresent		;Is a source needed?
	jz	CBLT6140		;  No
	mov	al,I_popSI		;  Yes, get source pointer
	stosb

CBLT6140:
	mov	ax,I_popDI_popCX	;Get destination pointer
	stosw				;Get loop count
	test	bh,DestIsColor		;Color scanline update?
	jnz	CBLT6160		;  Yes
	jmp	CBLT6300		;  No, just do the mono scanline update




;	The scanline update is for color.  Generate the logic to update
;	a brush, perform plane selection, process mono ==> color conversion,
;	and test for plane overflow.


CBLT6160:
	or	bh,bh			;Color conversion?
	jns	CBLT6180		;  No
	errnz	GagChoke-10000000b



;	The source is monochrome.  Handle mono ==> color conversion.
;	The AND & XOR mask table will need to be rotated for the next
;	pass over the source.
;
;	The source scanline pointer will not be updated until all planes
;	have been processed for the current scan.
;
;	If NoMunge has been specified, then the color conversion table
;	and the color conversion code was not generated, and no update
;	code will be needed.
;
;
;		lea	bp,ABrush
;		mov	ax,4[bp]
;		xchg	ax,2[bp]
;		xchg	ax,[bp]
;		mov	4[bp],ax


	test	MooreFlags,NoMunge	;Is there really a conversion table?
	jnz	short CBLT6200		;  No, so skip the code

	mov	al,I_movBPwordI 	;lea bp,ABrush
	stosb
	lea	ax,ABrush		;Get address of table
	stosw
	lea	si,RotANDXOR		;--> rotate code
	mov	cx,LenRotANDXOR/2
	rep	movsw
	if	LenRotANDXOR AND 1
	movsb
	endif
	jmp	short CBLT6200





;	If there is a source, it must be color.  If it is a memory
;	bitmap, then the next plane must be selected, else it is
;	the display and the next plane will be selected through
;	the hardware registers.
;
;	<	add	si,PlaneW>


CBLT6180:
	test	bh,SrcPresent		;Is there really a source?
	jz	CBLT6200		;No source.
	mov	ax,I_addSIwordI 	;  No, generate plane update
	stosw				;Add si,PlaneW
	mov	ax,Src.PlaneW
	stosw




;	If the destination isn't the device, then it must be a color
;	memory bitamp, and it's pointer will have to be updated by
;	bmWidthPlanes.	If it is the display, then the next plane
;	will be selected through the hardware registers.
;
;	<	add	di,PlaneW>

CBLT6200:
	mov	ax,I_addDIwordI 	;  No, update bitmap to the next plane
	stosw
	mov	ax,Dest.PlaneW
	stosw





;	The source and destination pointers have been updated.
;	Now generate the plane looping logic.
;
;	<	shl	bl,1	      > ;Select next plane
;	<	jc	$+5	      > ;  No, reset to first
;	<	jmp	StartOfLoop   > ;  Yes, go process next
;	<	mov	bl,Plane1     > ;Reset plane indicator
;
;	or
;
;	<	shl	bl,1	      > ;Select next plane
;	<	jnc	StartOfLoop   > ;  Yes, go process next
;	<	mov	bl,Plane1     > ;Reset plane indicator


	mov	ax,I_shlBL_1		;Stuff plane lloping logic
	stosw

	mov	dx,OFF_BLTaddr		;Compute relative offset of
	sub	dx,di			;  start of loop
	cmp	dx,-125 		;Can this be a short label?
	jc	CBLT6240		;  No, must make it a near jmp
	sub	dl,2			;Bias offset by length of jz inst.
	mov	ah,dl
	mov	al,I_JNC
	stosw				;jnc StartOfLoop
	jmp	short CBLT6260

CBLT6240:
	mov	ax,I_jcp5h		;jc $+5
	stosw
	mov	al,I_JMPnear		;jmp near
	stosb
	sub	dx,5			;Adjust jump bias
	mov	ax,dx
	stosw				;Store jmp displacement

CBLT6260:
	mov	ax,(Plane1*256)+I_movBLbyteI
	stosw



subttl	Looping Logic - Color Brush Update
page

;	The plane update logic has been copied.  If a pattern was
;	involved for a color BLT, then the pattern index will need
;	to be updated to the next scanline.
;
;	This will involve subtracting off 3*SizePattern (MonoPlane),
;	and adding in the increment.  The result must be masked with
;	00000111b to select the correct source.  Note that the update
;	can be done with an add instruction and a mask operation.
;
;	inc   index+MonoPlane	inc-MonoPlane	result	 AND 07h
;
;	 1	 0+24 = 24	  1-24 = -23	   1	     1
;	 1	 7+24 = 31	  1-24 = -23	   8	     0
;	-1	 0+24 = 24	 -1-24 = -25	  FF	     7
;	-1	 7+24 = 31	 -1-24 = -25	   6	     6
;
;	<	mov	al,cs:[1234]  > ;Get brush index
;	<	add	al,n	      > ;Add displacement to next byte
;	<	and	al,00000111b  > ;Keep it in range
;	<	mov	cs:[1234],al  > ;Store displacement to next byte


CBLT6280:
	test	bh,PatPresent		;Is a pattern involved?
	jz	CBLT6300		;  No

	mov	ax,I_csOverride+(I_movAL_Mem*256)
	stosw				;mov al,cs:[xxxx]
	mov	dx,AddrBrushIndex
	mov	ax,dx
	stosw
	mov	al,I_addALByteI
	mov	ah,direction		;add al,bais
	sub	ah,MonoPlane		;Anybody ever fly one of these things?
	errnz	increase-1		;Must be a 1
	errnz	decrease+1		;Must be a -1
	stosw
	mov	ax,0700h+I_andALbyteI	;and al,00000111b
	stosw
	mov	ax,I_csOverride+(I_movMem_AL*256)
	stosw				;mov cs:[xxxx],al
	mov	ax,dx
	stosw



subttl	Looping Logic - Scan Line Update
page

;	Any plane selection stuff has been done.  Now generate the
;	next scanline code.  The next scan line code must handle
;	monochrome bitmaps, color bitmaps, huge bitmaps, the device,
;	the presence or absence of a source, and mono <==> color
;	interactions.
;
;	<	add si,Src.NextScan   > ;Normal source scan line update
;	<	Huge Bitmap Update    > ;>64K source update code
;		add di,Dest.NextScan	;Normal destination scan line update
;	<	Huge Bitmap Update    > ;>64K destination update code
;
;
;	All updates will at least consist of the add IndexReg,PlaneW.


CBLT6300:
	mov	ch,direction		;Load this for YUpdate code
	test	bh,SrcPresent		;Is there a source?
	jz	CBLT6340		;  No, skip source processing
	mov	dx,I_addSIwordI 	;add si,increment
	mov	bx,((HIGH I_movSI_AX)*256)+(HIGH I_leaAX_SI_Disp16)
	mov	cl,HIGH I_movAX_DS
	push	bp
	lea	bp,Src
	call	YUpdate 		;Generate the Y scan line update code
	pop	bp			;Restore frame pointer

CBLT6340:
	mov	dx,I_addDIwordI 	;add reg,increment
	mov	bx,((HIGH I_movDI_AX)*256)+(HIGH I_leaAX_DI_Disp16)
	mov	cl,HIGH I_movAX_ES
	push	bp
	lea	bp,Dest 		;--> destination data
	call	YUpdate 		;Generate the Y scan line update code
	pop	bp			;Restore frame pointer





;	Compile the scan line loop.  The code simply jumps to the start
;	of the outer loop if more scans exist to be processed.


CBLT6380:
	mov	ax,OFF_BLTaddr		;Compute relative offset of
	sub	ax,di			;  start of loop
	cmp	ax,-125 		;Can this be a short label?
	jc	CBLT6400		;  No, must make it a near jmp
	sub	al,2			;Bias offset by length of LOOP inst.
	mov	ah,al
	mov	al,I_LOOP
	stosw				;Set the loop instruction
	jmp	short CBLT6420

CBLT6400:
	mov	si,codeOFFSET jmpCXnz	;Move in the dec CX jnz code
	movsw
	movsw
	errnz	jmpCXnzLen-4		;Must be four bytes long
	sub	ax,6			;Adjust jump bias
	stosw				;  and store it into jump

CBLT6420:
	mov	al,I_retFAR		;Stuff the far return instruction
	stosb



subttl	Invocation and Exit
page

;	If the debug flag has been set, save the size of the created BLT
;	so it may be returned to the caller.

CBLT7000:
	ifdef	debug
	sub	di,OFF_BLTaddr		;Compute the length
	push	di			;  and save it
	endif



;	The BLT has been created on the stack.	Set up the initial registers,
;	set the direction flag as needed, and execute the BLT.

	test	TheFlags,SrcPresent	;Is there a source?
	jz	CBLT7020		;  No, don't load its pointer
	lds	si,Src.lpBits		;--> source device's first byte

CBLT7020:
	les	di,Dest.lpBits		;--> destination device's first byte

	mov	cx,yExt 		;Get count of lines to BLT
	cld				;Assume this is the direction
	cmp	stepDirection,stepright ;Stepping to the right?
	jz	CBLT7040		;  Yes
	std
CBLT7040:
	mov	bl,Plane1		;Set initial plane select logic
	push	bp			;MUST SAVE THIS
	call	BLTaddr 		;Call the FAR process
	pop	bp


CBLT7060:
	ifdef	debug
	pop	bx			;Get length of created BLT code
	endif

	add	sp,maxBLTsize		;Return BLT space
;	jmp	exit			;Hey, we're done!
	errn$	exit





;	EXIT- Exit BitBLT
;
;	Well, the BLT has been processed.  Restore the stack to its
;	original status, restore the saved user registers, show no
;	error, and return to the caller.
;
;	Entry:	None
;
;	Exit:	ax = 1
;
;	Uses:	All

exit:
	mov	ax,1			;Clear out error register (good exit)
;	jmp	exit_fail
	errn$	exit_fail




;	exit_fail - exit because of failure
;
;	The BLT is exited because of an error occured.	Restore the stack
;	to its original status, restore the saved user registers, and return
;	to the caller.
;
;	Entry:	ax = error code (0)
;
;	Exit:	(ax) = error code (0)
;
;	Uses:	All

exit_fail:
	cld				;Leave direction cleared

StackOV:								;100985


cEnd




subttl	CopyDev - Copy Device Parameters
page

assume	ds:nothing,es:nothing,ss:nothing

;	CopyDevice - Copy Device Information to Frame
;
;	Entry:	ds:si --> device
;		es:di --> frame DEV structure
;		bh     =  TheFlags, accumulated so far
;
;	Exit:	bh     =  TheFlags, accumulated so far
;
;	Uses:	ax,dx,si,di,flags


CopyDev:
	lodsw				;Get type
	errnz	bmType			;Must be the 1st word
	shl	bh,1			;Move in type

	movsw				;Width in bits
	errnz	bmWidth-bmType-2
	errnz	WidthBits

	movsw				;Height in scans
	errnz	bmHeight-bmWidth-2
	errnz	Height-WidthBits-2

	movsw				;Width in bytes
	errnz	bmWidthBytes-bmHeight-2
	errnz	WidthB-Height-2

	lodsw				;Get Planes/pixels
	cmp	ax,0101H		;Monochrome?
	je	CopyDev20		;  Yes	('C' clear)
	cmp	ax,0103h		;Our color?
	jne	CopyDev80		;  No, complain about color format
	stc				;  (show color)

CopyDev20:
	rcl	bh,1			;Rotate in color flag
	errnz	SrcIsColor-00000100b
	errnz	DestIsColor-00000001b
	errnz	bmPlanes-bmWidthBytes-2
	errnz	bmBitsPixel-bmPlanes-1

	movsw				;lpBits
	movsw
	errnz	bmBits-bmBitsPixel-1
	errnz	lpBits-WidthB-2

	movsw				;Width of a plane if small color bitmap
	errnz	bmWidthPlanes-bmBits-4
	errnz	PlaneW-lpBits-4

	add	si,6			;-->segment index
	movsw				;Segment index if huge bitmap
	errnz	bmSegmentIndex-bmWidthPlanes-8
	errnz	SegIndex-PlaneW-2

	movsw				;Number of scans in a segment
	errnz	bmScanSegment-bmSegmentIndex-2
	errnz	ScansSeg-SegIndex-2

	movsw				;Number of unused bytes if huge bitmap
	errnz	bmFillBytes-bmScanSegment-2
	errnz	FillBytes-ScansSeg-2

	mov	al,bh			;Set IsColor flag in the Device Flags
	and	al,IsColor
	stosb
	errnz	DevFlags-FillBytes-2
	errnz	IsColor-DestIsColor	;Must be same bits
	ret



;	The device has the wrong color format.	Remove the return address
;	and go straight to the error exit.

CopyDev80:
	pop	ax			;Remove return address
	jmp	exit_fail		;  and return the error code




subttl	Y Computations
page

;	ComputeY - Compute Y Releated Parameters
;
;	The parameters related to the Y coordinate and BLT direction
;	are computed.  The parameters include:
;
;	    a)	Index to next scan line
;
;	    b)	Index to next plane
;
;	    c)	Starting Y address calculation
;
;	    d)	Huge bitmap update parameters
;
;		1)  overflow conditional jump opcode
;
;		2)  overflow address range
;
;		3)  bmFillBytes correct value
;
;		4)  bmSegmentIndex correct value
;
;
;	Entry:	bp --> DEV structure to use (src or dest)
;		ax  =  Y coordinate
;		cx  =  BLT direction
;		       0000 = Y+
;		       FFFF = Y-
;		bx  =  inclusive Y extent
;
;	Exit:	cx  =  BLT direction
;		bx  =  inclusive count
;
;	Uses:	ax,dx,si,di,flags


ComputeY:
	mov	si,SegIndex[bp] 	;Is this a huge bitmap?
	or	si,si
	jne	ComputeY100		;  Yes, lots of special processing



;	This is a small bitmap.  Compute the Y address
;	and update lpBits.  Then compute and save the increment to the
;	next scan line.  This increment will be:
;
;
;	    If a monochrome bitmap, regardless of color conversion:
;
;		+/- bmWidthBytes
;
;	    If a color bitmap, and it's the source for a color==>mono BLT:
;
;		+/- bmWidthBytes
;
;	    For a color bitmap, either mono==>color as the destination,
;	    or color==>color:
;
;		- 3*bmWidthPlanes +/- bmWidthBytes
;
;
;	These increments take into account any additions to the offset
;	made in the plane selection logic.



	mov	si,WidthB[bp]		;Need bmWidthBytes a couple of times
	mul	si			;Compute Y address
	add	OFF_lpBits[bp],ax	;  and add it into lpBits
	xor	si,cx			; 1's complement if Y-
	sub	si,cx			; 2's complement if Y-


;	si is now +/- bmWidthBytes.  This is the correct increment
;	to the next scan line unless the device is a color bitmap
;	involved in a color to color BLT or is the destination in
;	a mono==>color BLT.


ComputeY20:
	test	DevFlags[bp],ColorUp	;Use color scan line update?
	jz	ComputeY40		;  No, use mono update

	mov	ax,PlaneW[bp]		;Compute 3*bmWidthBytes
	sub	si,ax			;  and subtract this from the
	add	ax,ax			;  next scan line index
	sub	si,ax			;  (this counters the plane updates)

ComputeY40:
	mov	NextScan[bp],si 	;Set index to next scan line
	ret				;All done with device, small bitmaps



subttl	Y Computations For Huge Bitmaps
page

;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in, and update the lpBits pointer to that segment.  This will
;	be done iteratively since it is expected that only two or three
;	segments will ever be involved, so it will be faster in most
;	cases.



ComputeY100:
;	mov	si,SegIndex[bp] 	;Get segment index
	mov	di,ScansSeg[bp] 	;Get number of scans per segment
	xor	dx,dx			;Sum segments here

ComputeY120:
	add	dx,si			;Compute bias to next segment selector
	sub	ax,di			;See if Y is within this 64K
	jae	ComputeY120		;Y isn't in the 64K, try next
	sub	dx,si			;Added one index too many
	add	SEG_lpBits[bp],dx	;Update selector



;	The selector has been updated if needed.  Now see if the BLT
;	will span a segment boundary.  If it will span a segment boundary,
;	then set the SpansSeg flag.  Optimizations can be made if the BLT
;	doesn't cross a segment boundary.
;
;	ax is currently the Y offset within a segment minus the number of
;	scans in a segment, so it must be negative.
;
;	Currently:
;		ax = offset within segment - # scans per segment.
;		di - # scans per segment
;		cx = BLT direction
;		bx = inclusive Y extent


ComputeY140:
	mov	si,ax			;Need a copy of Y
	jcxz	ComputeY160		;This is a Y+ BLT


;	Y- BLT
;
;	si is the Y offset within the segment minus the number of scans
;	in the segment (it is negative).  Add in the number of scans,
;	then subtract the inclusive extent.  If the result stays positive,
;	then the result fits within the segment, else it spans more than
;	one segment.
;
;	Also take the 2's complement of both the bmSegmentIndex and the
;	bmFillBytes values.  This will allow the Y update code to always
;	generate adds.	Also save the conditional jump opcode for the
;	scan line update generation.


	mov	CompTest[bp],LOW I_jcp12h
	neg	FillBytes[bp]		;Set 2's complement for >64K
	neg	SegIndex[bp]		;  huge update generation
	add	si,di			;Get Y offset within segment
	sub	si,bx			;Subtract inclusive Y extent
	jns	ComputeY200		;BLT fits within one segment
	jmp	short ComputeY180	;BLT spans a segment boundary




;	Y+ BLT
;
;	si is the Y offset within the segment minus the number of scans
;	in the segment (it is negative).  Add in the inclusive extent.
;	If the result is still negative, then the BLT is contained
;	within the segment, else it spans more than one segment.
;
;	Also save the conditional jump opcode for the scan line update
;	generation.


ComputeY160:
	mov	CompTest[bp],LOW I_jncp12h
	add	si,bx			;Is last scan line in anohter segment?
	js	ComputeY200		;  No, BLT will fit in the segment

ComputeY180:
	or	DevFlags[bp],SpansSeg	;Show source spans a segment



;	The correct segment and the SpansSeg flag have been computed.
;	Compute the Y address and update lpBits.


ComputeY200:
	add	ax,di			;Get Y offset within segment
	mov	si,WidthB[bp]		;Need bmWidthBytes a couple of times
	mul	si			;Compute Y address
	add	OFF_lpBits[bp],ax	;  and add it into lpBits



;	Compute and save the increment to the next scan line.  This
;	increment will be:
;
;	    If a monochrome bitmap, regardless of color conversion:
;
;		+/- bmWidthBytes
;
;	    If a color bitmap, and it's the source for a color==>mono BLT:
;
;		+/- bmWidthBytes*3
;
;	    For a color bitmap, either mono==>color as the destination,
;	    or color==>color:
;
;		if Y+ blt, then 0
;		if Y-, then -6*bmWidthBytes
;
;
;	These increments take into account any additions to the offset
;	made in the plane selection logic.
;
;	In all cases, set PlaneW to be bmWidthBytes.
;
;
;	Also compute the addess range used to test for overflow.
;	This value will be (bmFillBytes+bmWidthBytes) for Y+ BLTS.
;	For Y- BLTs, it will be -(planes*bmWidthBytes).


	mov	PlaneW[bp],si		;Plane width will always be scan width
	mov	dx,si			;Set dx = Y+ range check value
	add	dx,FillBytes[bp]	;  of bmFillBytes+bmWidthBytes
	mov	di,si			;Set di = Y- mono range check
	neg	di
	test	DevFlags[bp],IsColor	;Is this a mono device?
	jz	ComputeY220		;  Yes, just conditionally complement


;	Have a color device. The increment will depend on color
;	conversion or not, using the values given above.
;
;	Also have to correct the offset portion of the pointer to account
;	for this being a color bitmap.


	add	ax,ax			;Compute 2 * offset within plane
	add	OFF_lpBits[bp],ax	;  and add it into lpBits
	add	si,si			;Compute 3*bmWidthBytes
	sub	si,di			;
	mov	di,si			;Set Y- range check
	neg	di
	test	DevFlags[bp],ColorUp	;Color ==> mono update in use?
	jz	ComputeY220		;  Yes, just conditionally complement


;	The BLT is color to color.  Want either 0 or -6*bmWidthBytes.

	xor	ax,ax			;Assume Y+ BLT
	xchg	ax,si			;Huge color Y+ increment is 0
	jcxz	ComputeY240		;This is a Y+ BLT
	xchg	ax,si
	add	si,si			;Y-, need -6*bmWidthBytes.

ComputeY220:
	xor	si,cx			;Conditionally negate increment
	sub	si,cx

ComputeY240:
	mov	NextScan[bp],si 	;Save update to next scan
	jcxz	ComputeY260		;Y+, dx has range check value
	mov	dx,di

ComputeY260:
	mov	CompValue[bp],dx	;Save range check value
	ret				;All done




subttl	Scan Line Update Generation
page


;	YUpdate - Generate Y Update Code
;
;	The Y update code is generated as follows:
;
;	For the display, small bitmaps, and huge bitmaps where the BLT
;	doesn't span a segment bounday, all that need be done is add
;	NextScan to the offset portion of the bits pointer.  NextScan
;	is a 2's complement if the BLT is Y-, so an addition can always
;	be done.
;
;	    < add   si,NextScan >
;	      add   di,NextScan
;
;
;	For huge bitmaps where the BLT spans a segment boundary, the
;	above update must be performed, and the overflow/undeflow
;	detected.  This isn't too hard to detect.
;
;	For any huge bitmap, there can be a maximum of Planes*bmWidthBytes-1
;	unused bytes in a 64K segment.	The minimum is 0.  The scan line
;	update always updates to the first plane of the next (previous) scan.
;
;
;	When the BLT is Y+, if the new offset is anywhere within the
;	unused bytes of a segment, or in the first scan of a segment,
;	then overflow must have occured:
;
;	      -bmFillBytes <= offset < Planes*bmWidthBytes
;
;	Since the update is always made to the first plane of a scan,
;	Planes in the above equation can be thrown out.  Also, if
;	bmFillBytes is added to both sides of the equation:
;
;		0 <= offset < bmWidthBytes+bmFillBytes	(unsigned compare)
;
;	will be true if overflow occurs.  The Y+ overflow check will
;	look like:
;
;
;	    lea ax,bmFillBytes[si]		;Adjust for fill bytes now
;	    cmp ax,bmWidthBytes+bmFillBytes	;Overflow occur?
;	    jnc NoOverflow			;  No
;	    cmp cx,2				;Any more scans?
;	    jnc NoOverflow			;  No, don't update selector
;	    add si,bmFillBytes			;Step over fill bytes
;	    mov ax,ds				;Compute new selector
;	    add ax,bmSegmentIndex
;	    mov ds,ax
;
;	  NoOverflow:
;
;
;
;	For Y- BLTs, the test is almost the same.  The equation becomes
;
;	   -(Planes*bmWidthBytes) > offset	(unsigned compare)
;
;	then underflow occurs.	Planes in the above equation cannot be
;	thrown out.  The Y- underflow check will look like:
;
;	    mov ax,si
;	    cmp ax,-(Planes*bmWidthBytes)	;Overflow occur?
;	    jc	NoOverflow			;  No
;	    cmp cx,2				;Any more scans?
;	    jnc NoOverflow			;  No, don't update selector
;	    add si,bmFillBytes			;Step over fill bytes
;	    mov ax,ds				;Compute new selector
;	    add ax,bmSegmentIndex
;	    mov ds,ax
;
;	bmFillBytes and bmSegment index will be the 2's complement by
;	now if the BLT is Y-.
;
;
;
;	Entry:	ss:bp --> source or destination data
;		ss:di --> where to generate the code
;		dx     =  update register (add si,wordI & mov ax,si)
;		bl     =  lea register (si or di)
;		bh     =  mov si,ax   or   mov di,ax register
;		cl     =  segment register (ds or es)
;		ch     =  Direction
;
;	Exit:	ss:bp --> source or destination data
;		ss:di --> where to generate the code
;		bl     =  lea register (si or di)
;		bh     =  mov si,ax   or   mov di,ax register
;		cl     =  segment register (ds or es)
;		ch     =  Direction
;
;	Uses:	ax,di,flags


YUpdate:

;	Stuff the scan line increment for the source or destination
;
;	<   add     si,1234h	>	;Update source
;	<   add     di,9ABCh	>	;Update destination


	mov	ax,NextScan[bp] 	;Get the increment
	or	ax,ax			;If zero, don't generate the code
	jz	YUpdate10
	xchg	ax,dx			;Set opcode
	stosw
	xchg	ax,dx			;Set increment
	stosw

YUpdate10:
	test	DevFlags[bp],SpansSeg	;Does the BLT span a segment?
	jnz	YUpdate20		;  Yes, lots of work
	ret				;  No, all done



;	The BLT spans a segment.  The code to detect when the segment is
;	crossed must be generated, as given above.


YUpdate20:
	mov	ah,dh			;Set register for MOV
	errnz	<(HIGH I_addSIwordI) - (HIGH I_movAX_SI)>
	errnz	<(HIGH I_addDIwordI) - (HIGH I_movAX_DI)>

	mov	al,LOW I_movAX_SI	;Assume Y- BLT
	errnz	<(LOW I_addSIwordI) - (LOW I_addDIwordI)>

	cmp	ch,decrease		;Y- BLT?
	je	YUpdate30		;  Yes

	mov	ah,bl			;lea reg, bmFillBytes
	mov	al,LOW I_leaAX_SI_Disp16
	errnz	<(LOW I_leaAX_SI_Disp16) - (LOW I_leaAX_DI_Disp16)>

	stosw
	mov	ax,FillBytes[bp]

YUpdate30:
	stosw

	mov	al,I_cmpAXwordI
	stosb
	mov	ax,CompValue[bp]
	stosw

	mov	al,CompTest[bp]
	mov	ah,HIGH I_jcp12h
	stosw
	errnz	<(HIGH I_jcp12h) - (HIGH I_jncp12h)>

	mov	ax,I_cmpCX_2
	stosw

	mov	ax,2+((LOW I_jcp0Dh)*256)
	stosw

	mov	al,(HIGH I_jcp0Dh)
	stosb
	errnz	<(LOW I_movSI_AX)-(LOW I_movDI_AX)>

	xchg	ax,dx			;Get add si, or add di,
	stosw
	mov	ax,FillBytes[bp]
	stosw

	mov	al,LOW I_movAX_DS
	mov	ah,cl
	stosw
	errnz	<(LOW I_movAX_DS)-(LOW I_movAX_ES)>

	mov	al,I_addAXwordI
	stosb

	mov	ax,SegIndex[bp]
	stosw

	mov	al,LOW I_movDS_AX	;mov SegmentReg,ax
	mov	ah,cl
	stosw
	errnz	<(LOW I_movDS_AX)-(LOW I_movES_AX)>
	errnz	<(HIGH I_movDS_AX)-(HIGH I_movAX_DS)>
	errnz	<(HIGH I_movES_AX)-(HIGH I_movAX_ES)>

YUpdate40:
	ret
page
	ifdef	debug
	public	jmpCXnz
	public	phaseAlign
	public	maskedStore
	public	RotANDXOR
	public	BitBlt000
	public	Parse10
	public	Parse20
	public	Parse30
	public	Parse50
	public	Parse60
	public	complain
	public	Setup100
	public	Setup110
	public	Setup120
	public	Setup130
	public	Setup140
	public	Setup150
	public	Setup160
	public	Setup170
	public	vComplain
	public	vExit
	public	Setup200
	public	Setup205
	public	Setup210
	public	Setup215
	public	Setup220
	public	Setup225
	public	Setup230
	public	Setup310
	public	Setup320
	public	Setup330
	public	Setup340
	public	Setup350
	public	Setup400
	public	Setup410
	public	Setup500
	public	Setup520
	public	Setup540
	public	Setup560
	public	CBLT
	public	CBLT1000
	public	CBLT1010
	public	CBLT1015
	public	CBLT2000
	public	CBLT2001
	public	CBLT2020
	public	CBLT2040
	public	CBLT2060
	public	CBLT3000
	public	CBLT3020
	public	CBLT3040
	public	CBLT3060
	public	CBLT3100
	public	CBLT3110
	public	CBLT3120
	public	CBLT3140
	public	CBLT3160
	public	CBLT3180
	public	CBLT4000
	public	CBLT4020
	public	CBLT4040
	public	CBLT4060
	public	CBLT4080
	public	CBLT4100
	public	CBLT4120
	public	CBLT4140
	public	CBLT4160
	public	CBLT4180
	public	CBLT4200
	public	CBLT4220
	public	CBLT4240
	public	CBLT4260
	public	CBLT4280
	public	CBLT5000
	public	CBLT5020
	public	CBLT5040
	public	CBLT5060
	public	CBLT5080
	public	CBLT5100
	public	CBLT5120
	public	CBLT5140
	public	CBLT5500
	public	CBLT5520
	public	CBLT5540
	public	CBLT5560
	public	CBLT5580
	public	CBLT5600
	public	CBLT6000
	public	CBLT6100
	public	CBLT6120
	public	CBLT6140
	public	CBLT6160
	public	CBLT6180
	public	CBLT6200
	public	CBLT6240
	public	CBLT6260
	public	CBLT6280
	public	CBLT6300
	public	CBLT6340
	public	CBLT6380
	public	CBLT6400
	public	CBLT6420
	public	CBLT7000
	public	CBLT7020
	public	CBLT7040
	public	CBLT7060
	public	exit_fail
	public	CopyDev
	public	CopyDev20
	public	CopyDev80
	public	ComputeY
	public	ComputeY20
	public	ComputeY40
	public	ComputeY100
	public	ComputeY120
	public	ComputeY140
	public	ComputeY160
	public	ComputeY180
	public	ComputeY200
	public	ComputeY220
	public	ComputeY240
	public	ComputeY260
	public	YUpdate
	public	YUpdate20
	public	YUpdate30
	public	YUpdate40
	endif

sEnd	code
end
