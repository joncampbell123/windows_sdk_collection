page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1983-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

TITLE	StrBlt - String Block Transfer
%out	Strblt


wp	equ	word ptr
bptr	equ	byte ptr


?CHKSTK = 1				;Must be in here
?CHKSTKPROC macro lvs
extrn dmCheckStack:near
mov	ax, lvs
call	dmCheckStack
endm


;	Define the portions of gdidefs.inc that will be needed

incFont 	= 1			;Include font definitions
incDrawMode	= 1			;Include drawing mode definitions


	.xlist
	include cmacros.inc
	include gdidefs.inc
	include color.inc
	.list





sBegin	code
assumes cs,code


page

;	dmStrBlt(lpDestBitmap,x,y,lpClipRect,lpString,
;	       count,lpFont,lpDrawMode,lpXform)

cProc	dmStrBlt,<FAR,PUBLIC>,<si,di>

	parmd	lpDestBitmap
	parmw	x
	parmw	y
	parmd	lpclipRect
	parmd	lpString
	parmw	count
	parmd	lpFont
	parmd	lpDrawMode	; includes background mode and bkColor
	parmd	lpXform

	localw	LeadFrag	; number of bits needed in first byte out
	localw	charCount	; decremented while moving string to stack (see note above)
	localw	charstart	; char string moved to stak, starts here

	localw	SStart		; start address of dest byte on screen
	localw	SInc		; Increment to move to next line on screen

	localw	GetChar 	; routine to get next char for Pro or nonPro fonts
	localw	justify 	; routine to justify for Pro or nonPro fonts
	localw	FlipRoutine	; used to toggle between normal get char
				;   and set inter char space routine

	localw	yStart		; address of first line to be drawn
	localw	yInc		; increment to line of font = fiWidthBytes
	localw	ystop		; when yStart = yStop, we done!

				; used only for opaque bkColor
	localw	BBoxLeft	; Left coordinates of char bounding box
	localw	BBoxRight	; Right side of bounding box
	localw	BBoxTop 	; Top of char bounding box
	localw	BBoxBottom	; Bottom of char bounding box

	localw	ScanCount	; Height of bounding box

	localw	CharWidth	; fonts of width 8 are special cased

	localw	BrkXtra 	; number of pixels to add at every break
	localw	BrkErr		; running error term for break remainder
	localw	BrkRem		; number of pixels to be spread across line
	localw	BrkCount	; number of breaks in which to spread remainder
	localw	ChrXtra 	; number of pixels to add at each char

	localw	WriteBits	;Address of routine that stuffs the bits
	localw	NextPlane	;Index to next plane of the scan line
	localw	NextScan	;Index to next scan line for opaque code
	locald	BackColor	;BackGround color
	locald	ForeColor	;Foreground (TextColor)

	localw	YUpdate 	;Y update routine
	localw	SegmentIndex	;Index to next segment if huge
	localw	Overflow	;Address range where segment overflow occurs
	localw	FillBytes	;Number of unused bytes in a segment
	locald	lpBits		;Pointer to the bits



cBegin	dmStrBlt

	if	???			;Do test only if locals
	jc	NoRoom
	endif
	mov	ax,count		;Get absolute value of
	cwd				;  string length
	xor	ax,dx
	shl	ax,1			;4 bytes per character is the
	shl	ax,1			;  cost in terms of stack space
	add	ax,20h			;Plus some other stuff
	mov	cx,ax			;Save # bytes requesting
	call	dmCheckStack		;See if room
	jnc	Room			;Room

NoRoom:
	jmp	StackOV 		;No room, abort

Room:
	add	sp,cx			;Don't need the space yet
	mov	charstart,sp		;will eventually store chars on stack
	lds	si,lpclipRect
	les	di,lpFont


;	set all the constant values

;
;	yinc	= fontwidthbytes	    ; set font line loop variables
;	if no clip  then
;	    ystart  = lpFontbits
;	    ystop   = lpFontbits + fontHeight*yinc
;	    build clip rectangle on stack = dest bitmap size
;	else
;	    k = max(y, clprect.orgY) - y
;	    if k => fontHeight then exit ;string won't show
;	    ystart  = lpFontbits + k*yinc
;	    k = min(y+fontHeight, clprect.orgY+.extY) - y
;	    if k <= 0 then exit  ;string won't show
;	    ystop   = lpFontbits + k*yinc
;	endif

	xor	dx,dx
	mov	ax,count
	or	ax,ax
	jz	jmpexit 		;if no characters do nothing

	jg	SaveCharCount	    ; INTERFACE CHANGE FIX  3/18/85
	neg	ax		    ; negative char count means return width
	xor	bx,bx		    ;	 and update the BrkErr
	mov	OFF_lpclipRect,bx   ; open up the clip so all chars are counted
				    ;
SaveCharCount:			    ; INTERFACE CHANGE FIX  3/18/85

	mov	charCount,ax
	mov	cx,es:dfWidthBytes[di]
	mov	yInc,cx
	mov	ax,es:dfPixWidth[di]
	mov	CharWidth,ax
	or	si,si
	jz	Noclip

	mov	ax,top[si]
	mov	dx,ax			;Save top for exclusion code
	sub	ax,y
	mov	bx,ax
	jge	maxinBX
	sub	dx,bx			;Move top of clip down screen some
	xor	bx,bx

maxinBX:				;bx = #of scan lines to clip off top
	mov	BBoxTop,dx		;Save top of exclusion area
	mov	BBoxBottom,dx		;Also save as bottom of exclusion area

	mov	dx,es:dfPixHeight[di]
	cmp	bx,dx
	jge	jmpexit 		;string won't show
	mov	ax,bottom[si]
	sub	ax,y
	cmp	ax,dx
	jl	mininAX
	mov	ax,dx

mininAX:				;ax = number of last clipped scan line
	or	ax,ax
	jle	jmpexit 		;string won't show
	cmp	ax,bx			;test for 0 extent clip rect
	jle	jmpexit
	jmp	short setystartstop

jmpexit:
	jmp	exit

NoClip:
	mov	bx,Y			;Set Y as exclusion area
	mov	BBoxTop,bx		;Save top of exclusion area
	mov	BBoxBottom,bx		;Also save as bottom of exclusion area
	xor	bx,bx
	mov	ax,es:dfPixHeight[di]

setystartstop:
	sub	ax,bx			;Remember total scan count for BBox
	mov	ScanCount,ax
	add	BBoxBottom,ax		;Set bottom of exclusion area
	add	ax,bx

	mul	cx
	mov	dx,wp es:dfBitsPointer[di]
	add	ax,dx

	dec	ax			;bug fix: see below

	mov	yStop,ax
	mov	ax,bx
	mov	si,bx			;remember cliped y for screen set
	mov	bx,dx
	mul	cx
	add	ax,bx

;	bug fix: character offset 0 is used to flag break chars or 8 wide fonts
;	thus all char offsets are bumped up by one to make them non 0.	The
;	difference is made up here.

	dec	ax

	mov	yStart,ax
	mov	ax,si


;	if font is proportional then  ; set address to proper routine
;	    GetChar = PorpGetChar
;	else
;	    GetChar = NonpropGetChar
;	endif

	lds	si,lpDrawMode		;see if we must deal with inter
	mov	BrkErr,0		;  character spacing or justification

	cmp	es:dfPixWidth[di],0	;width 0 implies proportional font
	je	PropChars

	mov	dx,codeOFFSET NPnoJustify
	mov	justify,dx
	cmp	TBreakExtra[si],0
	je	setNP
	mov	dx,codeOFFSET NPJustify
	mov	justify,dx
	mov	dx,BreakExtra[si]
	mov	BrkXtra,dx
	mov	dx,BreakRem[si]
	mov	BrkRem,dx
	mov	dx,BreakErr[si]
	mov	BrkErr,dx
	mov	dx,BreakCount[si]
	mov	BrkCount,dx
	xor	dx,dx			;lie about char width so that 8 wide
	mov	CharWidth,dx		;  special case will not be used

setNP:
	mov	dx,CharExtra[si]
	mov	si,codeOFFSET NonpropGetChar
	or	dx,dx
	je	GotGetChar
	mov	si,codeOFFSET SpaceNonpropGetChar
	mov	ChrXtra,dx
	xor	dx,dx			;lie about char width so that 8 wide
	mov	CharWidth,dx		;  special case will not be used
	jmp	short GotGetChar


PropChars:
	mov	dx,codeOFFSET PnoJustify
	mov	justify,dx
	cmp	TBreakExtra[si],0
	je	setP
	mov	dx,codeOFFSET PJustify
	mov	justify,dx
	mov	dx,BreakExtra[si]
	mov	BrkXtra,dx
	mov	dx,BreakRem[si]
	mov	BrkRem,dx
	mov	dx,BreakErr[si]
	mov	BrkErr,dx
	mov	dx,BreakCount[si]
	mov	BrkCount,dx
	xor	dx,dx			;lie about char width so that 8 wide
	mov	CharWidth,dx		;  special case will not be used

setP:
	mov	dx,CharExtra[si]
	mov	si,codeOFFSET PropGetChar
	or	dx,dx
	je	GotGetChar
	mov	si,codeOFFSET SpacePropGetChar
	mov	ChrXtra,dx
	xor	dx,dx			;lie about char width so that 8 wide
	mov	CharWidth,dx		;  special case will not be used

GotGetChar:
	mov	GetChar,si
	xor	si,codeOFFSET InterCharSpace
	mov	FlipRoutine,si


;	linestart   = y*bitmap.widthbytes
;	SInc	    = bitmap.widthbytes


;	The extents of what will fit within the clipping rectangle have
;	been computed.	Compute the starting Y coordinate.  If this is
;	a huge bitmap, then the correct segment will have to be computed.
;	If this is a color bitmap, Y will have to be multiplied by the
;	number of planes.


YComp:
	add	ax,Y			;Compute starting Y coordinate
	mov	YUpdate,codeOFFSET xyz	;Assume normal Y updates (do nothing)
	xor	dx,dx			;Sum segments in dx
	lds	si,lpDestBitmap 	;--> the device
	mov	cx,bmSegmentIndex[si]	;Is this a huge bitmap?
	jcxz	YComp30 		;  No
	mov	bx,bmScanSegment[si]	;Get # scans per segment



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide.


YComp10:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	YComp10 		;Not in current segment, try next
	mov	di,ax			;Make a copy of Y
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment
	xchg	cx,dx			;Leave segment bias in cx


;	The starting Y has been computed.  If the ending Y will be
;	in another segment, then set up the Y update routine to be
;	the huge bitmap update.


	add	di,ScanCount		;Will stay negative or 0 if it fits
	dec	di
	js	YComp20 		;It fits within a segment


;	The huge bitmap will not fit within a segment.	Special Y
;	update code will have to be used.

	mov	YUpdate,codeOFFSET UHuge;Huge Y update required
	mov	SegmentIndex,dx 	;Save it incase it's needed
	mov	dx,bmFillBytes[si]
	mov	FillBytes,dx
	mov	bx,bmWidthBytes[si]
	add	dx,bx
	mov	Overflow,dx


;	OK.  Huge bitmap processing has been computed.	If this is
;	a color bitmap, then the Y must be multiplied by the number
;	of planes.


YComp20:
	cmp	wp bmPlanes[si],0103h	;Is this a color memory bitmap?
	jne	YComp30 		;  No
	errnz	bmBitsPixel-bmPlanes-1

	add	bx,bx			;The opaque code works from blue to
	add	Overflow,bx		;  red, so adjust overflow for this

	mov	bx,ax			;Compute Y to use for address calc
	add	ax,ax
	add	ax,bx



;	Y has been adjusted if needed.	Now the final address can finally
;	be computed.


YComp30:
	add	cx,wp bmBits+2[si]	;Compute segment of the bits
	mov	SEG_lpBits,cx		;Save segment of the bits
	mov	cx,wp bmBits[si]	;Save offset  of the bits
	mov	OFF_lpBits,cx

	mov	dx,bmWidthBytes[si]	;Get width of a scan
	mov	SInc,dx 		;Save width
	mul	dx			;linestart = y*widthbytes



;	move string to stack  and preprocess it. inner loop expects
;	    push offset to first byte of char
;	    mov  cl,amt needed to left justify first byte
;	    mov  ch,width of char - cl
;	    push  cx
;
;	    care must be taken to modify the offset & width of the first and
;	    last char in the string to have them fit the clip rectangle
;
;	    sloc    = x - ClipRect.rOriginX	; screen location relative to
;	    hiclp   = ClipRect.rExtentX 	;    the clip rectangle
;	    if	sloc < 0 then
;		sStart = linestart + cliprect.rOriginX/8
;	    else
;		sStart	= linestart + x/8
;		hiclp	= hiclp - sloc		    ; set coordinates relative
;		sloc	= 0			    ;	 to sStart
;	    endif
;	    if	sloc => hiclip	then exit	; won't show
;
;	lookforfirst:
;	    call    getchar	    ; will get offset and width of next char
;				    ; will also add width to sloc
;	    jle     lookforfirst
;
;	    offset  = offset + width - sloc	;partial clip first visable char
;	    width   = sloc
;	    sloc    = sloc - hiclp		;set up to clip other end
;
;	lookforlast:
;	    jge     cliplast
;	    push    offset/8	    ; byte offset to stack
;	    mov     cl,offset and 7
;	    mov     ch,width - ch
;	    push    cx
;	    call    getchar
;	    jmp     lookforlast
;
;	cliplast:
;	    width   = width - sloc
;	    jle     gotlast	    ; entire last char was cliped
;	    push    offset/8	    ; byte offset to stack
;	    mov     cl,offset and 7
;	    mov     ch,width - cl
;	    push    cx
;
;	gotlast:
;	    push    dummy
;	    push    -1		    ; flag that there are no more chars
;
;
;	Currently:	(ax) = linestart


	lds	si,lpClipRect
	mov	bx,x			;sloc
	or	si,si
	jz	againNoClip
	mov	cx,left[si]		;needed for if
	mov	BBoxLeft,cx		;assume char box starts at clip region
	mov	di,bx			;needed for else
	mov	dx,right[si]		;hiclp
	mov	BBoxRight,dx		;assume char box ends at clip region
	sub	dx,cx
	sub	bx,cx
	jl	bbb

	mov	BBoxLeft,di		;fits inside clip box on right side
	mov	cx,di
	sub	dx,bx
	xor	bx,bx

bbb:
	mov	di,cx			;calculate leading fragment
	and	di,7
	sub	di,8
	neg	di
	mov	LeadFrag,di
	shr	cx,1
	shr	cx,1
	shr	cx,1
	add	ax,cx
	mov	sStart,ax
	cmp	bx,dx			;bx = sloc, dx = hiclp
	jge	jexit

	lds	si,lpString
	les	di,lpFont

lookforfirst:
	call	getchar 		;will get offset=ax and width=cx of
	jle	lookforfirst		;  next char.  also add width to sloc

	or	ax,ax			;if first char is break we must insure
	je	firstIsBreak		;   that offset stays 0

	add	ax,cx			;offset  = offset + width - sloc
	sub	ax,bx
	mov	cx,bx			;width	 = sloc
	sub	bx,dx			;sloc	 = sloc - hiclp

lookforlast:
	jge	cliplast
	mov	ch,cl
	mov	cl,al
	shr	ax,1
	shr	ax,1
	shr	ax,1
	push	ax
	and	cl,7
;	add	ch,cl							;061785
;	sub	ch,8							;061785
	push	cx
	call	getchar
	jmp	lookforlast

firstIsBreak:
	add	ax,cx			;offset  = offset + width - sloc
	sub	ax,bx
	and	ax,7			; mash the byte offset to 0
	mov	cx,bx			;width	 = sloc
	sub	bx,dx			;sloc	 = sloc - hiclp
	jmp	lookforlast


;	Since no clipping box starts at char coordinate, set other end to x
;	because when we run out of chars the total width of the char string
;	will be added to BBox right.

againNoClip:
	mov	BBoxLeft,bx
	mov	BBoxRight,bx

	mov	cx,bx							;080785
	mov	di,cx			;calculate leading fragment
	and	di,7
	sub	di,8
	neg	di
	mov	LeadFrag,di
	shl	bx,1			;Convert x to byte offset
	shl	bx,1
	shl	bx,1
	add	ax,bx			;add to line start to get Sstart
	mov	sStart,ax
	lds	si,lpString
	les	di,lpFont
	xor	bx,bx

ANC1:
	call	getchar
	mov	ch,cl
	mov	cl,al
	shr	ax,1
	shr	ax,1
	shr	ax,1
	push	ax
	and	cl,7
;	add	ch,cl							;061785
;	sub	ch,8							;061785
	push	cx
	jmp	ANC1


jexit:
	jmp exit

JmpDoneBBox1:
	jmp	DoneBBox

SpecialCase:				;do 8 wide seperately
	pop	dx			;put end of char flag just before
	pop	si			;  last char
	xor	ax,ax
	push	ax			;char offset 0 flags next to last
	dec	ax
	push	ax
	push	si
	push	dx
	jmp	short markend

cliplast:
	sub	cx,bx			;width	 = width - sloc
	jle	gotlast 		;entire last char was cliped
	mov	ch,cl
	mov	cl,al
	shr	ax,1
	shr	ax,1
	shr	ax,1
	push	ax
	and	cl,7
;	add	ch,cl							;061785
;	sub	ch,8							;061785
	push	cx

gotlast:
	cmp	sp,charstart		;did we put characters on the stack?
	je	jexit			;if not there are none to print
	cmp	CharWidth,8		;special case the 8 wide fonts
	je	SpecialCase

markend:
	xor	ax,ax
	push	ax
	dec	ax
	push	ax			;flag that there are no more chars

;	if	?CHKSTK1		;If stack checking enabled,	;100985
;	xor	ax,ax			;  see if we pushed too much	;100985
;	cCall	chkstk							;100985
;	endif								;100985



; INTERFACE CHANGE FIX	3/18/85;
;
;	If the count is negative, then return the extents

	mov	cx,count		;Negative count is text extent call
	or	cx,cx
	jg	CheckExclude		;Not negative, must output string
	mov	cx,BrkErr		;If we used break err, it will be non 0
	jcxz	SomeLabel		;  in which case we should return the
	lds	si,lpDrawMode
	mov	BreakErr[si],ax 	;  final value so it can be used
					;  for the next run
SomeLabel:
	mov	ax,BBoxRight		;Compute the extents to return
	sub	ax,BBoxLeft
	mov	dx,BBoxBottom
	sub	dx,BBoxTop
	jmp	exit

; INTERFACE CHANGE FIX	3/18/85 	;End of fix



;	The background mode isn't opaque.  Go dump out the characters


NotOpaque:
	mov	WriteBits,dx		;Set output routine's address
	jmp	DoneBBox		;No bounding box needed






CheckExclude:
	lds	si,lpDestBitmap 	;--> physical device




;	The device is a memory bitmap.
;
;	Set up the address of the output routine which will handle
;	the actual setting of the bits in memory, and the opaquing
;	of the background, and even set up the background/foreground
;	colors.

	mov	ax,wp bmWidthPlanes[si] ;Save plane increment incase small color
	mov	cx,bmSegmentIndex[si]	;Load huge flag incase color bitmap
	cmp	wp bmPlanes[si],0103h
	errnz	bmBitsPixel-bmPlanes-1
	lds	si,lpDrawMode		;--> the colors
	je	ColorBitmap		;Color memory bitmap in our format



;	The bitmap is monochrome.  Set the colors according to the
;	mono bit in the background and foreground colors.
;
;	Also set up the address of the output routine which will handle
;	the actual setting of the bits in memory, and the opaquing
;	of the background.


	mov	al,TextColor.Mono[si]	;Munge text (foreground) color
	shl	al,1
	cbw
	mov	bptr (ForeColor),ah
	errnz	MonoBit-01000000b
	mov	dx,CodeOFFSET WriteMonoBits

	cmp	bkMode[si],TRANSPARENT	;Transparent mode set?
	je	NotOpaque		;Yes, don't need to process background
	mov	al,BkColor.Mono[si]	;Munge background color
	shl	al,1
	cbw
	mov	bptr (BackColor),ah
	errnz	MonoBit-01000000b
	mov	cx,CodeOFFSET MonoOpaque;Set routine to handle opaquing bbox
	jmp	short DoOpaque



;	The bitmap is color.  Copy the foreground and background colors
;	Also set up the address of the output routine which will handle
;	the actual setting of the bits in memory, and the opaquing
;	of the background.


ColorBitmap:
	mov	dx,SInc 		;Will need this (it's bmWidthBytes)
	jcxz	ColorBitmap10		;Not huge


;	Huge color bitmap.  SInc should be 3*bmWidthBytes.  NextPlane
;	should be bmWidthBytes.  NextScan should be 6*bmWidthBytes.

	mov	ax,dx
	add	dx,dx
	add	dx,ax
	mov	SInc,dx 		;SInc is 3*bmWidthBytes

ColorBitmap10:
	mov	NextPlane,ax		;Set index to next plane
	mov	bx,ax			;Compute nextscan for the opaque
	add	ax,ax			;  code.  It is 3*NextPlanes+SInc
	add	ax,bx			;  for small bitmaps, or 6*bmWidthBytes
	add	ax,dx			;  for huge color bitamps
	mov	NextScan,ax


	mov	ax,wp TextColor[si]
	mov	wp (ForeColor),ax
	mov	al,TextColor.Blue[si]
	mov	bptr (ForeColor)+Blue,al
	mov	dx,CodeOFFSET WriteColorBits

	cmp	bkMode[si],TRANSPARENT	;Transparent mode set?
	je	NotOpaque		;Yes, don't need to process background
	mov	ax,wp bkColor[si]
	mov	wp (BackColor),ax
	mov	al,bptr bkColor.Blue[si]
	mov	bptr (BackColor)+Blue,al
	errnz	Red
	errnz	Green-Red-1
	errnz	Blue-Green-1

	mov	cx,CodeOFFSET ColorOpaque
;	jmp	short DoOpaque
	errn$	DoOpaque





;	The character bounding box must be blted out.  Compute the first
;	and last byte masks, and the innerloop count.  Then invoke the
;	passed routine for the bounding box
;
;	Currently:	cx = bounding box output routine
;			dx = bits output routine

DoOpaque:
	mov	WriteBits,dx		;Save output routine's address
	push	cx			;Save routine to do bounding box
	mov	cx,BBoxLeft		;Convert dimensions to byte and offset
	mov	ax,cx			;Compute byte offset of left side
	shr	ax,1
	shr	ax,1
	shr	ax,1
	mov	dl,0ffh 		;Compute altered bits mask
	and	cx,7
	shr	dl,cl

	mov	cx,BBoxRight		;Compute byte offset of right side
	dec	cx			;note: rect from 0 to 1 sets 1 pixel,
	mov	bx,cx			;    not 2, so we make adjustment here
	shr	bx,1
	shr	bx,1
	shr	bx,1
	mov	dh,0ffh 		;Compute altered bits mask
	and	cx,7
	sub	cx,7
	neg	cx
	shl	dh,cl

	inc	ax			;Adjustment for first byte
	sub	bx,ax			;Number of bytes affected
	jnl	GotFirstLastMasks	;At least two bytes
	and	dl,dh			;Head and tail in same byte


GotFirstLastMasks:
	mov	BBoxLeft,bx		;Save for multiple passes through loop
	les	di,lpBits
	add	di,sStart		;es:si --> first scan


;	The bounding box has been computed, so blt it out.
;
;	Currently:	es:di	 --> first byte of destination
;			BBoxLeft  =  Innerloop count
;			bx	  =  Innerloop count
;			    -	     indicates only first byte
;			    0	     indicates first and last bytes
;			    +	     indicates first, middle, and last bytes
;			dl	  =  first byte mask
;			dh	  =  last byte mask if BBoxLeft >= 0
;			ScanCount =  # of scanlines


xyz	proc	near			;Need this to be a near return

	ret				;To the given Opaque routine

xyz	endp




;	MonoOpaque - Monochrome Bitmap Opaque Routine
;
;	The bounding box of the text string is painted with the
;	background color in the given monochrome memory bitmap.
;
;	Entry:	es:di	 --> first byte of destination
;		BBoxLeft  =  Innerloop count
;		bx	  =  Innerloop count
;		    -	     indicates only first byte
;		    0	     indicates first and last bytes
;		    +	     indicates first, middle, and last bytes
;		dl	  =  first byte mask
;		dh	  =  last byte mask if BBoxLeft >= 0
;		ScanCount =  # of scanlines
;
;	Exit:	to DoneBBox
;
;	Uses:	All

MonoOpaque:
	mov	ah,bptr (BackColor)	;Get color to set
	mov	si,SInc 		;Compute increment to next scan
	sub	si,bx			;Take off inner loop count (or -1)
;	sub	si,2			;Take off first and last byte
;	inc	si			;No increment after last byte
	dec	si
	mov	bx,ScanCount		;Set height of the BBox

MonoStartScan:
	mov	al,es:[di]		;Process first byte
	xor	al,ah
	and	al,dl
	xor	es:[di],al
	inc	di			;--> next byte

	mov	cx,BBoxLeft		;Get innerloop count
	or	cx,cx			;Any bytes left?
	jl	MonoNextScan		;No inner bytes or end byte
	mov	al,ah			;Set color
	rep	stosb			;Set middle bytes (or none if zero)

	mov	al,es:[di]		;Process last byte
	xor	al,ah
	and	al,dh
	xor	es:[di],al

MonoNextScan:
	add	di,si			;--> next scanline
	call	YUpdate 		;YUpdate --> UHuge or xyz(ret)
	dec	bx			;All done?
	jz	DoneBBox		;  Yes
	call	YUpdate 		;YUpdate --> UHuge or xyz(ret)
	jmp	MonoStartScan







;	ColorOpaque - Color Bitmap Opaque Routine
;
;	The bounding box of the text string is painted with the
;	background color in the given color memory bitmap.
;
;	Entry:	es:di	 --> first byte of destination
;		BBoxLeft  =  Innerloop count
;		bx	  =  Innerloop count
;		    -	     indicates only first byte
;		    0	     indicates first and last bytes
;		    +	     indicates first, middle, and last bytes
;		dl	  =  first byte mask
;		dh	  =  last byte mask if BBoxLeft >= 0
;		ScanCount =  # of scanlines
;
;	Exit:	to DoneBBox
;
;	Uses:	All

ColorOpaque:
	mov	ax,NextPlane		;--> Blue plane
	add	ax,ax
	add	di,ax

ColorStartScan:
	mov	si,2			;Set plane loop count

ColorStartPlane:
	mov	bx,di			;Save start of plane
	mov	ah,bptr (BackColor)[si]
	mov	al,es:[di]		;Process first byte
	xor	al,ah
	and	al,dl
	xor	es:[di],al
	inc	di			;--> next byte

	mov	cx,BBoxLeft		;Get innerloop count
	or	cx,cx			;Any bytes left?
	jl	ColorNextPlane		;No inner bytes or end byte
	mov	al,ah			;Set color
	rep	stosb			;Set middle bytes (or none if zero)

	mov	al,es:[di]		;Process last byte
	xor	al,ah
	and	al,dh
	xor	es:[di],al

ColorNextPlane:
	mov	di,bx			;Restore plane pointer
	sub	di,NextPlane		;--> next plane (Blue==>Green==>Red)
	dec	si			;Done all three planes?
	jns	ColorStartPlane 	;  No
	add	di,NextScan		;--> to blue plane, next scan
	dec	ScanCount		;Done all scans?
	jz	DoneBBox		;  Yes, all done
	call	YUpdate 		;YUpdate --> UHuge or xyz(ret)
	jmp	ColorStartScan		;Do next scan line



;	Any bounding box has been output.  Now do the actual outputting
;	of the characters.


DoneBBox:
	lds	si,lpFont		;--> font bits
	lds	si,dfBitsPointer[si]
	les	di,lpBits
	add	di,sStart		;add screen offset to display base
	mov	sStart,di		;shift to absolute coordinates
	jmp	nextline		;go do it!



;	inner loop uses these variables and register assignments
;	 ah=outbyte, dh=numneeded  (number of bits needed to complete outbyte)
;	 al=inbyte, cl=inin (number of bits in in byte)
;	 dl = mask for storage =0ffh or 0
;	    get_next_byte (sets inbyte, numleft, & bumps pointer, including
;		resetting pointer the next char in string)
;		uses  ch=bitsleft (number of bits left to be fetched to inchar)
;

dumpout:
	add	cl,dh			;finish shift
	shl	ax,cl
	mov	cl,dh
	call	WriteBits		;Write the bits
	mov	dh,8			;numneeded

	neg	cl
	jz	nextbyte

shiftout:
	sub	dh,cl			;determine number of bits to shift
	jle	dumpout
	shl	ax,cl

nextbyte:
;	sub	ch,8			;bitsleft			;061785
;	jge	fatchar 						;061785
;	add	ch,8							;061785
;	jle	newchar 						;061785
	cmp	ch,8							;061785
	ja	fatchar 						;061785
	or	ch,ch							;061785
	je	newchar 						;061785


lastbyteofchar:
	xor	al,al			;assume a break char
	or	si,si			;  until proven otherwise
	je	aa1
	lodsb

aa1:
	mov	cl,ch			;inin, bitsleft in char
	xor	ch,ch			;none left
	jmp	shiftout

fatchar:
	xor	al,al			;assume a break char
	or	si,si			;until proven otherwise
	je	aa2
	lodsb

aa2:
	mov	cl,8			;inin
	sub	ch,cl							;061785
	jmp	shiftout

newchar:
	dec	bx
	dec	bx
	mov	si,ss:[bx]		;character offset in font
	dec	bx
	dec	bx
	mov	cx,ss:[bx]		;get inin and numleft
	or	cl,cl			;inin neg means no more chars
	js	stringout
	xor	al,al			;assume a break char
	or	si,si			;until proven otherwise
	je	aa3
	add	si,yStart		;convert to address
	lodsb

aa3:
	shl	al,cl			;left justify it in inbyte
	sub	cl,8
	neg	cl			;set inin

;	or	ch,ch							;061785
;	jge	shiftout						;061785
;	add	cl,ch							;061785
	sub	ch,cl							;061785
	jnc	shiftout						;061785
	add	cl,ch							;061785
	xor	ch,ch							;061785
	jmp	shiftout

stringout:
	add	si,yStart		;convert to address
	mov	cl,dh			;numneeded to shift count
	shl	ah,cl
	call	WriteBits		;Write fragment

	mov	si,yStart		;move font offsets
	add	si,yInc
	cmp	si,yStop		;have we done all the lines
	jnb	AnotherJmpExit
	mov	yStart,si

	mov	di,SInc 		;Move screen offset
	add	di,SStart
	call	YUpdate 		;YUpdate --> UHuge or xyz(ret)
	mov	SStart,di

nextline:
	mov	bx,charstart		;reset character pointer

;	start with byteout blank, and num needed to be all bits

	xor	ax,ax			;clear byteout
	mov	dh,bptr (LeadFrag)	;num needed
	cmp	CharWidth,8		;special case the 8 wide fonts
	je	First8
	jmp	newchar

AnotherJmpExit:
	jmp	exit

First8:
	xor	cx,cx
	mov	cl,dh
	dec	bx
	dec	bx
	mov	si,ss:[bx]
	or	si,si
	je	Last8
	add	si,yStart
	mov	cx,ss:-2[bx]
	lodsb
	xchg	ah,al
	shl	ah,cl
	sub	cl,8
	neg	cl
	cmp	dh,cl
	jg	numNeededGTinin

	xchg	cl,dh
	rol	ax,cl
	sub	cl,dh
	jne	out8
	mov	cl,8			;num needed always 8 for warp8
	jmp	short warp8		;if allignment is 0 go at warp speed

numNeededGTinin:
	rol	ax,cl
	sub	cl,dh
	jmp	short new8char


warploop:
	add	si,yStart
	lodsb

warp8:
	xchg	al,ah
	call	WriteBits
	xchg	al,ah
	sub	bx,4
	mov	si,ss:[bx]
	or	si,si
	jne	warploop
	jmp	short last8

loop8:
	add	si,yStart
	mov	ah,[si]
	rol	ax,cl
	sub	cl,8

out8:
	neg	cl
	xchg	al,ah
	call	WriteBits
	xchg	al,ah
	rol	ax,cl
	sub	cl,8

new8char:
	neg	cl
	sub	bx,4
	mov	si,ss:[bx]
	or	si,si
	jne	loop8
;	jmp	last8

last8:
	dec	bx
	dec	bx
	mov	ah,al
	mov	dh,cl
	jmp	newchar


exit:
	mov	sp,charstart		;pop off string from stack

StackOV:				;Stack overflow exit		;100985

cEnd	dmStrBlt




subrou	proc	near
;	getchar 	; will get offset=ax and width=cx of next char
;			; and will add cx to bx as the last thing


NPdud:
	mov	al,es:dfDefaultChar[di]
	jmp	short GotNPChar

SpaceNonPropGetChar:
	mov	cx,FlipRoutine		;switch over to interchar space routine
	xor	GetChar,cx

NonPropGetChar:
	lodsb
	dec	charCount
	jl	NPout			;no characters in string, get out
	cmp	al,es:dfLastChar[di]
	ja	NPdud
	sub	al,es:dfFirstChar[di]
	jb	NPdud

GotNPChar:
	mov	cx,es:dfPixWidth[di]
	cmp	al,es:dfBreakChar[di]
	je	just			;will be NPJustify or NPnoJustify

NPnoJustify:
	mul	cl			;non-proportional fonts
					;  offset = char*width
	add	ax,8			;increment byte address to avoid 0
	add	bx,cx			;add width to sloc
	ret

NPout:					;out of characters
	add	BBoxRight,bx		;since ran out of chars adjust bound
					;  box from clip rectangle
	pop	ax			;remove return from stack
	jmp	gotlast 		;push marker onto stack

just:
	jmp	justify


Pjustify:
	xor	ah,ah
	shl	ax,1			;convert to word count
	push	bx			;get some room
	mov	bx,ax
	mov	ax,es:dfCharOffset[bx+di]
	mov	cx,es:dfCharOffset+2[bx+di]
	sub	cx,ax			;compute width from two adjacent offsets
	pop	bx
	jz	Pload			;Ignore 0 width characters



;	we have hit a break char so must put in space.	We run a small
;	Breshenham algorithm to spread the space.

NPJustify:
	add	cx,BrkXtra		;increase the width by Break Extra
	mov	ax,BrkErr		;pick up running error term
	sub	ax,BrkRem		;see if ready to throw one of the
	jg	j1			;  remaining bits into this one

	add	ax,BrkCount		;reset running error
	inc	cx

j1:
	mov	BrkErr,ax
	xor	ax,ax			;set index to 0 to flag a break
	add	bx,cx			;add width to sloc
	ret				;return from GetChar




Pdud:					;here if char was a dud
	mov	al,es:dfDefaultChar[di]
	jmp	short GotPChar

SpacePropGetChar:
	mov	cx,FlipRoutine		;switch over to interchar space routine
	xor	GetChar,cx

PropGetChar:				;proportional font

Pload:
	lodsb
	dec	charCount
	jl	Pout			;no characters in string, get out
	cmp	al,es:dfLastChar[di]
	ja	Pdud			;use dud for characters not in font
	sub	al,es:dfFirstChar[di]
	jb	Pdud			;use dud for characters in the font

GotPChar:
	cmp	al,es:dfBreakChar[di]
	je	just			;will be either PJustify or PnoJustify

PnoJustify:
	xor	ah,ah
	shl	ax,1			;convert to word count
	push	bx			;get some room
	mov	bx,ax
	mov	ax,es:dfCharOffset[bx+di]
	mov	cx,es:dfCharOffset+2[bx+di]
	sub	cx,ax			;comp width from two adjacent offsets
	add	ax,8			;increment byte address to avoid 0
	pop	bx
	jz	Pload			;Ignore 0 width characters
	add	bx,cx			;add width to sloc
	ret

Pout:					;no chars left
	add	BBoxRight,bx		;since ran out of chars adjust bound
					;  box from clip rectangle
	pop	ax			;eliminate return
	jmp	gotlast 		;push marker to stack




InterCharSpace: 			;puts in a break record to give the
					;  desired intercharacter spacing
	mov	cx,FlipRoutine		;switch back to the true get char
	xor	GetChar,cx		;  routine

	mov	cx,ChrXtra
	xor	ax,ax			;offset 0 indicates break record
	add	bx,cx			;add width to sloc
	ret				;return from GetChar
page

;	WriteMonoBits - Write Font Bits To Monochrome Bitmap
;
;	A monochrome write of the given bits will be performed
;	to the given monochrome bitmap.
;
;	Entry:	ah     =  bits to write
;		es:di --> where bits go
;
;	Exit:	es:di --> next byte
;
;	Uses:	dl

WriteMonoBits:
	mov	dl,es:[di]		;Process first byte
	xor	dl,bptr (ForeColor)
	and	dl,ah
	xor	es:[di],dl
	inc	di			;--> next byte
	ret





;	WriteColorBits - Write Font Bits To Color Bitmap
;
;	A Color write of the given bits will be performed
;	to the given color bitmap.
;
;	Entry:	ah     =  bits to write
;		es:di --> where bits go
;
;	Exit:	es:di --> next byte
;
;	Uses:	dx

WriteColorBits:
	push	bx
	mov	bx,NextPlane		;Index to next plane
	mov	dl,es:[di]		;Process red byte
	xor	dl,bptr (ForeColor).Red
	and	dl,ah
	xor	es:[di],dl

	mov	dl,es:[bx][di]		;Process green byte
	xor	dl,bptr (ForeColor).Green
	and	dl,ah
	xor	es:[bx][di],dl

	add	bx,bx
	mov	dl,es:[bx][di]		;Process blue byte
	xor	dl,bptr (ForeColor)+Blue
	and	dl,ah
	xor	es:[bx][di],dl
	inc	di			;--> next byte
	pop	bx
	ret
page

;	UHuge - Update in Y for Huge Bitmaps.
;
;	If the new address just computed is processed against the
;	segment bounds of a huge bitmap.  If overflow occured, the
;	new selector and offset is computed.  The check to see if
;	any more scans exist must have been performed prior to calling
;	this routine.
;
;	Entry:	es:di = Y address, first plane of scan
;
;	Exit:	es:di = Y address, first plane of scan, updated if needed
;
;	Uses:	es,di,cx,flags

UHuge:
	mov	cx,di			;Get current offset
	add	cx,FillBytes		;Add in fill bytes to force over 0
	cmp	cx,Overflow		;Did overflow occur?
	jnc	UHuge10 		;  No, no overflow
	mov	di,cx			;Set new offset
	mov	cx,es			;Compute new segment
	add	cx,SegmentIndex
	mov	es,cx

UHuge10:
	ret

subrou	endp
page
	ifdef	debug
	public	SaveCharCount
	public	maxinBX
	public	mininAX
	public	jmpexit
	public	NoClip
	public	setystartstop
	public	setNP
	public	PropChars
	public	setP
	public	GotGetChar
	public	YComp
	public	YComp10
	public	YComp20
	public	YComp30
	public	bbb
	public	lookforfirst
	public	lookforlast
	public	firstIsBreak
	public	againNoClip
	public	ANC1
	public	jexit
	public	JmpDoneBBox1
	public	SpecialCase
	public	cliplast
	public	gotlast
	public	markend
	public	SomeLabel
	public	CheckExclude
	public	NotOpaque
	public	ColorBitmap
	public	ColorBitmap10
	public	DoOpaque
	public	GotFirstLastMasks
	public	MonoOpaque
	public	MonoStartScan
	public	MonoNextScan
	public	ColorOpaque
	public	ColorStartScan
	public	ColorStartPlane
	public	ColorNextPlane
	public	DoneBBox
	public	dumpout
	public	shiftout
	public	nextbyte
	public	lastbyteofchar
	public	aa1
	public	fatchar
	public	aa2
	public	newchar
	public	aa3
	public	stringout
	public	nextline
	public	AnotherJmpExit
	public	First8
	public	numNeededGTinin
	public	warploop
	public	warp8
	public	loop8
	public	out8
	public	new8char
	public	last8
	public	exit
	public	StackOV
	public	NPdud
	public	SpaceNonPropGetChar
	public	NonPropGetChar
	public	GotNPChar
	public	NPnoJustify
	public	NPout
	public	just
	public	Pjustify
	public	NPJustify
	public	j1
	public	Pdud
	public	SpacePropGetChar
	public	PropGetChar
	public	Pload
	public	GotPChar
	public	PnoJustify
	public	Pout
	public	InterCharSpace
	public	WriteMonoBits
	public	WriteColorBits
	public	UHuge
	public	UHuge10
	endif
sEnd	code
end
