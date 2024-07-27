page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1985-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

TITLE	RObject - Realize Object
%out	RObject



;	Define the portions of gdidefs.inc that will be needed

incLogical	= 1


	.xlist
	include cmacros.inc
	include gdidefs.inc
	include color.inc
	.list

sBegin	code
assumes cs,code


	public	SumRGBColorsAlt 	;Alternate entry point

	extrn	Dither:near		;Dithering code

	extrn	BWThreshold:abs

	extrn	HHatchBr_0:abs
	extrn	HHatchBr_1:abs
	extrn	HHatchBr_2:abs
	extrn	HHatchBr_3:abs
	extrn	HHatchBr_4:abs
	extrn	HHatchBr_5:abs
	extrn	HHatchBr_6:abs
	extrn	HHatchBr_7:abs

	extrn	VHatchBr_0:abs
	extrn	VHatchBr_1:abs
	extrn	VHatchBr_2:abs
	extrn	VHatchBr_3:abs
	extrn	VHatchBr_4:abs
	extrn	VHatchBr_5:abs
	extrn	VHatchBr_6:abs
	extrn	VHatchBr_7:abs

	extrn	D1HatchBr_0:abs
	extrn	D1HatchBr_1:abs
	extrn	D1HatchBr_2:abs
	extrn	D1HatchBr_3:abs
	extrn	D1HatchBr_4:abs
	extrn	D1HatchBr_5:abs
	extrn	D1HatchBr_6:abs
	extrn	D1HatchBr_7:abs

	extrn	D2HatchBr_0:abs
	extrn	D2HatchBr_1:abs
	extrn	D2HatchBr_2:abs
	extrn	D2HatchBr_3:abs
	extrn	D2HatchBr_4:abs
	extrn	D2HatchBr_5:abs
	extrn	D2HatchBr_6:abs
	extrn	D2HatchBr_7:abs

	extrn	CrHatchBr_0:abs
	extrn	CrHatchBr_1:abs
	extrn	CrHatchBr_2:abs
	extrn	CrHatchBr_3:abs
	extrn	CrHatchBr_4:abs
	extrn	CrHatchBr_5:abs
	extrn	CrHatchBr_6:abs
	extrn	CrHatchBr_7:abs

	extrn	DCHatchBr_0:abs
	extrn	DCHatchBr_1:abs
	extrn	DCHatchBr_2:abs
	extrn	DCHatchBr_3:abs
	extrn	DCHatchBr_4:abs
	extrn	DCHatchBr_5:abs
	extrn	DCHatchBr_6:abs
	extrn	DCHatchBr_7:abs
page

;	dmRealizeObject - Logical to Physical Object Realization
;
;	dmRealizeObject performs the task of converting logical objects
;	into physical objects that this driver can manipulate to per-
;	form the various functions requested of it.
;
;	The size needed to realize an object will be returned if the
;	pointer to where the physical realization is to be stored is
;	NULL.
;
;	In some cases where the driver cannot realize the requested object,
;	a solid color pen must be realized which GDI will use when it
;	performs the nessacary simulations.  In other cases, punt.
;
;	Entry:	Per parameters below
;
;	Exit:	(ax) = 0 if error
;
;	Uses:	ax,bx,cx,dx,es,flags

cProc	dmRealizeObject,<FAR,PUBLIC>,<si,di>

	parmD	lpDevice		;Pointer to Device structure
	parmW	Style			;Style of realization
	parmD	lpInObj 		;Pointer to input (logical) object
	parmD	lpOutObj		;Pointer to output (physical) object
	parmD	lpTextXForm		;Pointer to a text transformation
					;  or (x,y) for brush realization
					;  Segment = y
					;  Offset = x

	localw	SavePtr
	locald	SaveColor
	localv	Tempbrush,%(SIZE OEMBrushDef)	;Temp storage for brushes



cBegin	dmRealizeObject

	cld				;All code following assumes this
	mov	bx,Style		;Is this delete object?
	or	bx,bx			;  (style negative)
	jns	Realize1		;  No, this is a realize or inquire size
	jmp	GoodExit		;This driver keeps no objects, so
					;  nothing needs to be done
Realize1:
	lds	si,lpInObj		;--> logical object
	dec	bx			;Determine style of realization.
	cmp	bx,OBJ_FONT-1		;Is it a legal object?
	jg	ErrAbort		;  Not by our standards, abort
	shl	bx,1			;Compute index into dispatch table
	mov	ax,OFF_lpOutObj 	;Do they want the size requirement?
	or	ax,SEG_lpOutObj
	jz	Realize2		;  Yes
	jmp	cs:RTable[bx]		;  No, realize an object

	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1

Realize2:
	mov	ax,cs:QTable[bx]	   ;Get size for the object
	jmp	Exit

	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1

ErrAbort:
	jmp	ErrorExit
page

;	RealizePen - Realize Logical Pen Into a Physical Pen
;
;	The given logical pen is realized into a physical pen.
;
;	The pen will be realized regardless of the pen width or
;	style since GDI will need a pen to use for simulations.
;
;	If the width of the pen is >1, then a solid pen will be
;	realized regardless of the pen style.  GDI will use this
;	pen for simulating the wide line.
;
;	If the pen style isn't recognized, then a solid pen of
;	the given color will be realized (this is called punting).
;
;
;	Entry:	ds:si --> logical object
;
;	Exit:	None
;
;	Uses:	All

RealizePen:
	mov	cx,lopnStyle[si]	;Get pen style
	cmp	cx,LS_NOLINE		;Is this a null pen?
	je	RealizePen3		;  Yes, realize regardless of width
	cmp	lopnWidth[si],2 	;Is this a wide line?
	jb	RealizePen2		;  No. We can realize the pen.

RealizePen1:
	mov	cx,LS_SOLID		;Realize solid pen for simulations

RealizePen2:
	cmp	cx,MaxLineStyle 	;Is the line style known?
	jg	RealizePen1		;  No, make it a solid pen

RealizePen3:
	lea	si,lopnColor[si]	;--> RGB color
	call	SumRGBColors		;Sum up the color
	les	di,lpOutObj		;--> where pen color is to be saved

	stosw				;Save color
	errnz	OEMPen
	errnz	Red
	errnz	Green-1
	mov	ax,dx
	stosw
	errnz	Blue-2
	errnz	Mono-3

	mov	ax,cx			;Save style
	stosw
	errnz	<OEMPenStyle-(SIZE PhysColor)>
	jmp	GoodExit
page

;	RealizeBrush - Realize Logical Brush Into a Physical Brush
;
;	Four styles of logical brushes may be realized.  These are SOLID,
;	HOLLOW, HATCHED, and PATTERN.
;
;	A SOLID brush is defined with a logical RGB color definition.
;	This color is processed into one of 65 dithers.
;
;	A HATCHED brush is defined with a logical RGB color definition and
;	a hatching type.  The hatch type is mapped to one of the six hatched
;	styles that the driver supports.  All bits in the hatched brush which
;	are 1 are set to the hatch color passed in, and all bits which are
;	0 are set to the background color passed in.
;
;	A PATTERN brush is defined with an 8 X 8 pattern in the form of a
;	physical bitmap.  The bitmap may be monochromw or color.  More
;	discussion on converting is contained under the hatched brush code.
;
;	A HOLLOW brush is one which can never be seen.	The brush style is
;	maintained in the device brush structure so that a check can be
;	made and an abort executed if one is used.  No punting is needed
;	for hollow brushes.
;
;
;	Brushes will be aligned based at the (x,y) origin passed in.
;
;
;	Entry:	ds:si --> logical object
;
;	Exit:	brush set
;
;	Uses:	All

RealizeBrush:
	mov	di,ss			;Set ss:di --> temp brush
	mov	es,di
	lea	di,TempBrush

	mov	bx,lbStyle[si]		;Get the brush style.
	cmp	bx,BS_PATTERN		;Legal brush ?
	jle	realizeBrush1		;  Yes
	mov	bx,BS_SOLID		;  No, realize a solid brush

RealizeBrush1:
	add	bx,bx			;Set offset into process index table
	jmp	cs:BSTable[bx]		;Jump to appropriate brush realization

	errnz	BS_SOLID
	errnz	BS_HOLLOW-BS_SOLID-1
	errnz	BS_HATCHED-BS_HOLLOW-1
	errnz	BS_PATTERN-BS_HATCHED-1
page

;	RHollowBrush - Realize Hollow Brush
;
;	This is sleazy.  Hollow brushes are implemented by checking
;	the style flag, and ignoring the contents of the OEMBrush
;	field.
;
;	Entry:	ds:si --> logical object
;		es:di --> temp brush area
;
;	Exit:	None
;
;	Uses:	All

RHollowBrush:
	lea	di,OEMBrushStyle[di]	;--> where style goes
	mov	bx,BS_HOLLOW
	jmp	Rotatebrush
page

;	RSolidBrush - Realize SOLID Style Brush
;
;	The given logical solid brush is realized.  Each color for
;	the brush (RGB) is dithered for the color bruhses.
;	The sum of all the colors is used to determine the dither
;	to use for monochrome portion of the brush.
;
;	Entry:	ds:si --> logical object
;		es:di --> temp brush area
;
;	Exit:	None
;
;	Uses:	All

RSolidBrush:
	lea	si,lbColor[si]		;--> RGB values
	xor	cx,cx			;Save individual intensities
	mov	cl,Blue[si]
	push	cx
	mov	cl,Green[si]
	push	cx
	mov	cl,Red[si]
;	push	cx			;SumRGBColors saves cx
	call	SumRGBColors		;Compute black/white sum
	mov	OFF_SaveColor,bx	;Save B/W sum for later

	lea	di,TempBrush.RedPlane	;--> where brush will go
	mov	si,ss
	mov	es,si
	xor	si,si			;Clear "grey" flag

	mov	dx,cx			;Dither the red
	call	Dither
	pop	dx
	call	Dither			;Dither the green
	pop	dx
	call	Dither			;Dither the blue


;	Both the dithering alogrithms may generate an undesirable grey
;	(lores for 808080, hires for 404040 and C0C0C0), so these are
;	special cased to a known grey that will be uniform across the
;	devices, and is known to have a satisfactory apperance.

	or	si,si			;Was this one of the greys?
	jz	RSolidBrush20		;  No
	cmp	si,00010101b		;Dark grey in all three planes?
	je	RSolidBrush10		;  Yes
	cmp	si,00101010b		;Grey in all three planes
	je	RSolidBrush10
	cmp	si,00111111b		;Light grey in all three planes?
	jne	RSolidBrush20		;  No

RSolidBrush10:
	add	si,si			;Fill with desired grey
	and	si,00000111b
	mov	ax,cs:Greys-2[si]
	mov	cx,SizePattern*3
	sub	di,cx
	shr	cx,1
	rep	stosw

RSolidBrush20:
	mov	ax,OFF_SaveColor	;Get monochrome color
	cwd
	mov	si,3
	div	si
	mov	dx,ax
	xor	si,si
	call	Dither


;	Both the dithering alogrithms may generate an undesirable grey
;	(lores for 808080, hires for 404040 and C0C0C0), so these are
;	special cased to a known grey that will be uniform across the
;	devices, and is known to have a satisfactory apperance.

	or	si,si			;Was this one of the greys?
	jz	RSolidBrush30		;  No
	add	si,si			;  Yes, fill with real grey
	mov	ax,cs:Greys-2[si]
	sub	di,SizePattern
	stosw
	stosw
	stosw
	stosw

RSolidBrush30:
;	lea	di,TempBrush.OEMBrushStyle
	errnz	OEMBrushStyle-MonoPlane-SizePattern
	mov	bx,BS_SOLID		;Set type of brush
	jmp	RotateBrush		;Rotate and store the temp brush



greys	label	word
	db	088h,022h		;Dark grey dither
	db	0AAh,055h		;Grey dither
	db	0DDh,077h		;Light grey dither

page

;	RPatternBrush - Realize PATTERN brush
;
;	The given bitmap is copied for use as a 8x8 bit pattern brush.
;	Any information beyond the first 8x8 bits is not required to be
;	maintained as part of the brush, even though GDI does allow
;	you to do this if desire.
;
;	If the bitmap is a monochrome bitmap, it is to be expanded up
;	into a black/white bitmap.  If the bitmap is a color bitmap,
;	then to compute the monochrome portion of the brush, the three
;	planes will be ANDed together (this favors a white background).
;
;
;	Entry:	ds:si --> logical object
;		es:di --> temp brush area
;
;	Exit:	None
;
;	Uses:	All

RPatternBrush:
	lds	si,lbpattern[si]	;Get pointer to the bitmap
	mov	dx,bmWidthBytes[si]	;Get physical width of bitmap
	dec	dx			;MOVSB automatically increments
	mov	bx,word ptr bmWidthPlanes[si] ;Get width of one plane
	mov	ah,3			;Setup color planes loop count
	cmp	bmPlanes[si],3		;Set 'Z' if color
	lds	si,bmBits[si]		;--> where the bits are
	je	RPatBrush1		;Handling color source
	xor	bx,bx			;Mono source, zero plane increment

RPatBrush1:
	mov	SavePtr,si		;Save start of plane
	mov	cx,SizePattern		;Set # bytes to move

RPatBrush2:
	movsb				;Move one byte of pattern
	add	si,dx			;Skip rest of scanline
	loop	RPatBrush2		;Until one plane has been moved

	mov	si,SavePtr		;Get back start of plane
	add	si,bx			;--> start of next plane

;	dec	ah,1			;(need to test "above" and "below")
	sub	ah,1			;Done all three planes yet?
	ja	RPatBrush1		;  No
	jb	rPatBrush4		;  Just handled monochrome source


;	Handle the monochrome plane of the brush.  If the source is a
;	monochrome bitmap, then just copy it as is.  If the source is
;	a color bitmap, then AND all the colors together to get the
;	color of the destination (favors white background).

	or	bx,bx			;Monochrome?
	jz	RPatBrush1		;  Yes, just copy mono data again
	mov	cl,SizePattern
	errnz	<SizePattern AND 0FF00H>

RPatBrush3:
	mov	al,byte ptr es:-(1*SizePattern)[di]	;Get blue
	and	al,byte ptr es:-(2*SizePattern)[di]	;AND with green
	and	al,byte ptr es:-(3*SizePattern)[di]	;AND with red
	stosb						;Store mono byte
	loop	RPatBrush3


RPatBrush4:
	mov	bx,BS_PATTERN
	jmp	short RotateBrush
page

;	RHatchBrush - Realizes HATCHED Brush
;
;	The requested hatched brush is returned.  Two colors are invloved
;	for hatched brushes, the background color, and the hatch color.
;
;	If these two colors are the same, then the brush will be a
;	solid brush.
;
;	If not, then all 0 bits in the hatch pattern will be set to the
;	background color, and all 1 bits will be set to the foreground
;	color.	Note that hatched colors are solid colors; no dithering
;	takes place for their realization.
;
;
;	Entry:	ds:si --> logical object
;		es:di --> temp brush area
;
;	Exit:	None
;
;	Uses:	All

RHatchBrush:
	mov	cx,lbHatch[si]		;Get hatch style
	cmp	cx,MaxHatchStyle	;Hatch style within range?
	jng	RHatchBrush1		;  Yes
	jmp	RSolidBrush		;  No, create a solid brush

RHatchBrush1:
	lea	si,lbBkColor[si]	;Compute background color
	call	SumRGBColors
	push	dx			;Save background rgb + mono color info
	lea	si,(lbColor-lbBkColor)[si] ;Compute the foreground color
	call	SumRGBColors
	pop	ax			;Get background rgb + mono color info
	mov	dl,ah			;dh = forground, dl = background info

	mov	bx,cx			;Compute address of hatch pattern
	shl	bx,1
	shl	bx,1
	shl	bx,1
	lea	bx,HHatch[bx]		;bx --> hatch pattern to use
	errnz	SizePattern-8
	errnz	VHatch-HHatch-SizePattern
	errnz	D1Hatch-VHatch-SizePattern
	errnz	D2Hatch-D1Hatch-SizePattern
	errnz	CrHatch-D2Hatch-SizePattern
	errnz	DCHatch-CrHatch-SizePattern
	errnz	DCHatch-CrHatch-SizePattern

	call	MoveHatch		;Move red
	call	MoveHatch		;Move green
	call	MoveHatch		;Move blue
	mov	cl,3			;Move mono
	ror	dh,cl
	ror	dl,cl
	call	MoveHatch
	errnz	RedBit-00000001b
	errnz	GreenBit-00000010b
	errnz	BlueBit-00000100b
	errnz	MonoBit-01000000b

	mov	bx,BS_HATCHED
	jmp	short RotateBrush
page

;	MoveHatch - Move Hatch Brush
;
;	The bits for one plane of a hatched brush are moved according
;	to the foreground and background colors passed in.  If the
;	colors are the same, then that color will be moved into the
;	brush.	If colors are different, then either the pattern or
;	the inverse will be moved into the brush.   The pattern will
;	be moved in if the foreground color is 0, and the inverse
;	will be moved in if the foreground color is 1 (the brushes
;	were defined for a foreground color of 0 and a background
;	color of 1)
;
;	This code takes advantage of the fact that the three planes
;	can be considered three independant planes with no relation
;	to one another.
;
;
;	Entry:	(dh:d0) =  foreground color info
;		(dl:d0) =  background color info
;		cs:bx --> hatch pattern to use
;		es:di --> next plane of destination brush
;
;	Exit:	(dh:d0) = next foreground color info
;		(dl:d0) = next background color infog
;		cs:bx --> hatch pattern to use
;		es:di --> next plane of destination brush
;
;	Uses:	al,ah,cx,si

MoveHatch	proc	near

	mov	si,bx			;Assume colors are different
	mov	cx,SizePattern		;Set up move for a single plane
	ror	dh,1			;Place next foreground color into D7
	ror	dl,1			;Place next background color into D7
	mov	al,dh			;Get foreground color (1 or 0)
	cbw				;Set foreground xor mask (00 or FF)
	xor	al,dl			;Set 'S' if different colors
	js	MoveHatch1		;Colors are different
	mov	si,codeOFFSET BlackBr	;Colors are same, make a solid plane

Movehatch1:
	lods	byte ptr cs:[si]	;Get next byte of pattern
	xor	al,ah			;Invert it if needed
	stosb				;Stuff it away
	loop	MoveHatch1
	ret

MoveHatch	endp
page

;	RotateBrush - Rotate Brush To Specified Alignment
;
;	The brush just created will be aligned with the (x,y) passed
;	in.  This is done so that areas of the screen can be moved,
;	and by realizing a new brush with the correct (x,y), a pattern
;	continued correctly with a previous pattern.
;
;	Entry:	es:di --> OEMBrushStyle
;		(bx) = brush style
;
;	Exit:	None
;
;	Uses:	All

	errnz	OEMBrushStyle-(4*SizePattern)
	errnz	SizePattern-8

RotateBrush:
	lea	si,-OEMBrushStyle[di]	;[ss:si] --> to first byte of brush
	les	di,lpOutObj		;[es:di] --> destination of object
	mov	dx,00000111b		;This mask will be used a couple times
	mov	cx,OFF_lpTextXForm	;Get X coordinate for alignment
	and	cl,dl			;Mask X coordinate to 0-7
	mov	es:OEMBrushStyle[di],bx ;Save brush style
	mov	bx,SEG_lpTextXForm	;Get Y coordinate for alignment
	mov	ah,4			;Set # planes to process

RotateBrush1:
	mov	ch,SizePattern		;Set loop count

RotateBrush2:
	lods	byte ptr ss:[si]	;Get next byte of brush
	ror	al,cl			;Rotate it to the origin passed
	and	bx,dx			;Mask Y alignment
	mov	es:[bx][di],al		;Store X and Y aligned bits
	inc	bx			;Update destination index
	dec	ch			;Update loop count
	jnz	RotateBrush2		;Negative indicates all processed
	add	di,SizePattern		;--> next plane
	dec	ah			;Update outer loop count
	jnz	RotateBrush1




;	Post process the brush to see if the brush is a physical solid
;	color or a real grey (same value for each byte of a scan).
;	This will allow acceleration of solid patterns and grey scales.

	lea	si,-OEMBrushStyle[si]	;--> back to start of temp brush
	mov	ax,ss
	mov	ds,ax
	push	di
	xor	bx,bx			;Will collect colors in these registers
	xor	dx,dx
	xor	di,di
	mov	cx,GreyScale*256+SizePattern

RotateBrush3:
	xor	ax,ax
	mov	al,BluePlane[si]	;Collect the blue plane
	add	bx,ax
	mov	al,GreenPlane[si]	;Collect the green plane
	add	dx,ax
	lodsb				;Collect the red plane
	add	di,ax

	mov	ah,NOT GreyScale	;Check for a grey scale
	cmp	al,GreenPlane-1[si]
	jne	RotateBrush4
	cmp	al,BluePlane-1[si]
	jne	RotateBrush4
	not	ah			;Is a grey scale

RotateBrush4:
	and	ch,ah
	dec	cl
	jnz	RotateBrush3

	mov	ax,0FFh*SizePattern
	or	bx,bx			;Blue plane solid?
	jz	RotateBrush5		;  Yes
	cmp	bx,ax
	jne	RotateBrush8		;  No

RotateBrush5:
	or	dx,dx			;Green plane solid?
	jz	RotateBrush6		;  Yes
	cmp	dx,ax
	jne	RotateBrush8		;  No

RotateBrush6:
	or	di,di			;Green plane solid?
	jz	RotateBrush7		;  Yes
	cmp	di,ax
	jne	RotateBrush8		;  No

RotateBrush7:
	and	di,RedBit*256		;Get red plane's color
	or	cx,di
	and	dh,GreenBit
	or	ch,dh
	and	bh,BlueBit
	or	ch,bh
	or	ch,BrushIsSolid 	;Show solid

RotateBrush8:
	pop	di			;Get back pointer to brush
	mov	es:byte ptr (OEMBrushAccel-OEMBrushStyle)[di],ch

;	jmp	GoodExit
	errn$	GoodExit





;	GoodExit - Indicate Successful

GoodExit:
	mov	al,1
	jmp	short Exit
page

; NO OP
RealizeFont:
	errn$	ErrorExit		;Just return an error




;	ErrorExit - Exit with error
;
;	An error is flagged to the caller.
;
;	Entry:	None
;
;	Exit:	(ax) = 0 to show error
;
;	Uses:	Ax,flags

ErrorExit:
	xor	ax,ax



Exit:

cEnd	dmRealizeObject
page

;	SumRGBColors - Sum Given RGB Color Triplet
;
;	The given RGB color triplet is summed, and the result returned
;	to the caller.	Other useful information is also returned.
;
;	Ordering of the color in a dword is such that when stored in
;	memory, red is the first byte, green is the second, and blue
;	is the third.  The high order 8 bits may be garbage when passed
;	in, and should be ignored.
;
;	when in a register:	xxxxxxxxBBBBBBBBGGGGGGGGRRRRRRRR
;
;	when in memory: 	db	red,green,blue
;
;	Entry:	ds:si --> RGB triplet to sum
;
;	Exit:	(bx) = Sum of the triplet
;		(al) = 0FFh if red   intensity (al) > 127
;		(al) = 000h if red   intensity (al) < 128
;		(ah) = 0FFh if green intensity (ah) > 127
;		(ah) = 000h if green intensity (ah) < 128
;		(dl) = 0FFh if blue  intensity (dl) > 127
;		(dl) = 000h if blue  intensity (dl) < 128
;		(dh:RedBit)   = red   intensity msb
;		(dh:GreenBit) = green intensity msb
;		(dh:BlueBit)  = blue  intensity msb
;		(dh:MonoBit)  = 0 if bx < BWThreashold
;		(dh:MonoBit)  = 0 if bx >= BWThreashold
;		ds:si --> RGB triplet to sum
;
;	Uses:	ax,bx,dx,flags


SumRGBColors	proc	near
	mov	ax,word ptr [si]	;ah = G, al = R
	mov	dl,byte ptr 2[si]	;dl = B

SumRGBColorsAlt:
	push	cx			;Don't drstroy cx
	xor	dh,dh			;Turn R, G, and B into bytes
	xor	bx,bx
	xor	cx,cx
	xchg	ah,cl
	add	bx,ax			;Sum the colors for the mono bit
	add	bx,dx
	add	bx,cx

	mov	ah,cl			;Restore green value
	mov	cl,8			;Compute individual R,G,B values
	sar	dl,cl			;Compute blue
	rcl	dh,1			;Move blue into mono byte
	sar	ah,cl			;Compute green
	rcl	dh,1			;Move green into mono byte
	sar	al,cl			;Compute red
	rcl	dh,1			;Move red into mono byte
	cmp	bx,BWThreshold		;White (for monochrome bitmaps)?
	jl	SumRGBColors2		;  No, its black
	or	dh,MonoBit		;  Show as white

	errnz	RedBit-00000001b
	errnz	GreenBit-00000010b
	errnz	BlueBit-00000100b
	errnz	MonoBit-01000000b

SumRGBColors2:
	pop	cx
	ret

SumRGBColors	endp
page

;	BSTable - Brush Style Realization Table
;
;	The following table contains the offset of the function
;	that performs the realization of the desired type of brush.

BSTable dw	RSolidBrush
	dw	RHollowBrush		;Can't realize hollow brushes
	dw	RHatchBrush
	dw	RPatternBrush

	errnz	BS_SOLID
	errnz	BS_HOLLOW-BS_SOLID-1
	errnz	BS_HATCHED-BS_HOLLOW-1
	errnz	BS_PATTERN-BS_HATCHED-1




BlackBr db	0,0,0,0,0,0,0,0 	;Used for solid plane of a hatched brush




;	Predefined Hatched Brushes
;
;	The following brushes are the predefined hatched brushes that
;	this driver knows about.

HHatch	db	HHatchBr_0
	db	HHatchBr_1
	db	HHatchBr_2
	db	HHatchBr_3
	db	HHatchBr_4
	db	HHatchBr_5
	db	HHatchBr_6
	db	HHatchBr_7

VHatch	db	VHatchBr_0
	db	VHatchBr_1
	db	VHatchBr_2
	db	VHatchBr_3
	db	VHatchBr_4
	db	VHatchBr_5
	db	VHatchBr_6
	db	VHatchBr_7


D1Hatch db	D1HatchBr_0
	db	D1HatchBr_1
	db	D1HatchBr_2
	db	D1HatchBr_3
	db	D1HatchBr_4
	db	D1HatchBr_5
	db	D1HatchBr_6
	db	D1HatchBr_7

D2Hatch db	D2HatchBr_0
	db	D2HatchBr_1
	db	D2HatchBr_2
	db	D2HatchBr_3
	db	D2HatchBr_4
	db	D2HatchBr_5
	db	D2HatchBr_6
	db	D2HatchBr_7

CrHatch db	CrHatchBr_0
	db	CrHatchBr_1
	db	CrHatchBr_2
	db	CrHatchBr_3
	db	CrHatchBr_4
	db	CrHatchBr_5
	db	CrHatchBr_6
	db	CrHatchBr_7

DCHatch db	DCHatchBr_0
	db	DCHatchBr_1
	db	DCHatchBr_2
	db	DCHatchBr_3
	db	DCHatchBr_4
	db	DCHatchBr_5
	db	DCHatchBr_6
	db	DCHatchBr_7



;	RTable - Realization Style Table
;
;	The offset to the routine to perform the required realize
;	function (pen, font, brush) is contained in this
;	table.

RTable	dw	RealizePen
	dw	RealizeBrush
	dw	RealizeFont

	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1





;	QTable contains the size required to realize each of the
;	objects, or the size to realize a pen for those objects
;	that the driver cannot handle( i.e. wide lines).


QTable	dw	SIZE OEMPenDef
	dw	SIZE OEMBrushDef
	dw	0


	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1



ifdef	debug
public Realize1
public Realize2
public ErrAbort
public RealizePen
public RealizePen1
public RealizePen2
public RealizePen3
public RealizeBrush
public RealizeBrush1
public RHollowBrush
public RSolidBrush
public RSolidBrush10
public RSolidBrush20
public RSolidBrush30
public RPatternBrush
public RPatBrush1
public RPatBrush2
public RPatBrush3
public RPatBrush4
public RHatchBrush
public RHatchBrush1
public Movehatch1
public RotateBrush
public RotateBrush1
public RotateBrush2
public RotateBrush3
public RotateBrush4
public RotateBrush5
public RotateBrush6
public RotateBrush7
public RotateBrush8
public GoodExit
public RealizeFont
public ErrorExit
public SumRGBColorsAlt
public SumRGBColors2
endif


sEnd	code
end
