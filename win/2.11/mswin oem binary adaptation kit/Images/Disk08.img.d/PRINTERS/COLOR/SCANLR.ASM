page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	ScanLR - Scan left or right
%out	ScanLR


;	Define the portions of gdidefs.inc that will be needed by bitblt.

incDrawMode	= 1			;Include drawing mode definitions


.xlist
include cmacros.inc
include gdidefs.inc
include color.inc
.list



StepLeft	equ	00000010b	;Flag values for DirStyle
StepRight	equ	00000000b
FindColor	equ	00000001b
FindNotColor	equ	00000000b


wp		equ	word ptr
bptr		equ	byte ptr





sBegin	code
assumes cs,code


extrn	RotBitTbl:byte		;Table of bit masks
extrn	DrawModeTbl:word	;Rop2 drawing mode template table


page

;	dmScanLR - Scan left or right
;
;	Starting at the given pixel and proceeding in the choosen direction,
;	the pixels are examined for the given color until one is found that
;	matches (or doesn't match depending on the style).  The x value is
;	then returned for the pixel that matched (or didn't match).
;
;	DirStyle:
;	       Left = 2, Right = 0, FindColor = 1, FindNotColor = 0
;
;	The physical device may be a monochrome bitmap, or a
;	bitmap in our color format.
;
;	There will be no error checking to see if the bitmap is in our
;	color format.  if it isn't, then we'll just return some bogus
;	value.
;
;	Entry:	Per parameters
;
;	Exit:	ax = -1 if nothing found
;		ax = 8000h if clipped
;		ax = x location of sought pixel
;
;	Uses:	ax,bx,cx,dx,es,flags


cProc	dmScanLR,<FAR,PUBLIC>,<si,di>

	parmd	lpDevice
	parmw	x
	parmw	y
	parmd	Color
	parmb	DirStyle		;Actually a word, but access as a byte

	localb	Planes			;#Planes
	localw	WidthBits		;Actual width of scan in bits
	localw	NextPlane		;Index to next plane if color


cBegin	dmScanLR

	lds	di,lpDevice		;--> device
	mov	al,bmPlanes[di] 	;Get #planes device has
	mov	ah,bptr (Color)+Mono	;Get the color to look for



;	If the given bitmap is a monochrome bitmap, then the physical
;	color must be set according to the monobit in the color.  If
;	this is the display, it will be faked as stated above.


	mov	Planes,al		;Set #planes
	dec	al			;Is this a mono bitmap?
	jnz	Scan30			;  No, must be a color
	add	ax,ax			;Set color to white or black
	cwd
	mov	OFF_Color,dx		;Color is white or black
	errnz	MonoBit-01000000b




;	The scanning code doesn't scan outside the bounds of the surface.
;	The starting coordinate however must be clipped.


Scan30:
	mov	ax,Y			;Get starting Y coordinate
	cmp	ax,bmHeight[di] 	;Within the surface of the device?
	jae	Scan31			;  No, return error
	mov	cx,bmWidth[di]		;Save width in pixels
	mov	WidthBits,cx
	cmp	cx,X			;Within the surface of the device?
	jae	Scan32			;  Yes, (X,Y) is a valid coordinate


;	The coordinate is clipped.  Return the clipped error code.

Scan31:
	mov	ax,8000h		;Set error code
	jmp	Scan180 		;  and return it



;	The coordinate is valid.  If a huge bitmap, compute which segment
;	the scan is in.


Scan32:
	les	si,bmBits[di]		;--> start of the bitmap or the screen
	mov	bx,wp bmWidthPlanes[di] ;Set offset to next plane
	mov	cx,bmSegmentIndex[di]	;Is this a huge bitmap?
	jcxz	Scan35			;  No, not huge



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	cwd				;Y must be positve.  Set dx = 0
	mov	bx,bmScanSegment[di]	;Get # scans per segment

Scan33:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	Scan33			;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment
	mov	cx,es			;Update bits pointer
	add	cx,dx
	mov	es,cx

	cmp	Planes,3		;If huge color, map the Y coordinate
	jne	Scan35			;  Not color, Y is OK as is
	mov	bx,ax			;  Is huge color, Multiply Y by the
	add	ax,ax			;    number of planes
	add	ax,bx
	mov	bx,bmWidthBytes[di]	;Need offset to next plane



;	Currently:
;
;		ax = Y coordinate to use
;		bx = index to next plane (if color)
;		es:si --> start of first plane
;		ds:di --> PDevice

Scan35:
	mov	NextPlane,bx		;Save index to next plane incase color
	mov	di,bmWidthBytes[di]	;Get scan width before loosing pointer
	mul	di			;Multiply by width of bitmap or screen
	add	si,ax			;ds:si --> start of scanline
	mov	bx,X
	mov	cx,bx
	shr	cx,1			;Compute byte offset in scan
	shr	cx,1
	shr	cx,1
	add	si,cx			;ds:si --> byte with start pixel
	mov	ax,es			;Will be working off both ds: and es:
	mov	ds,ax



;	Set cx to be the byte count for searching left.  Must adjust it
;	to include the byte pixel is in.

	inc	cx			;Adjust for partial byte



;	Compute the mask for the first byte (the partial byte).  Since
;	the defaults being set up are for searching left, this can be done
;	by getting the rotating bitmask for the pixel and decrementing it,
;	then using the logical NOT of the mask.  The mask will be used
;	for masking the bits to test in the partial (first) byte.
;
;		Bitmask 	  Mask		NotMask
;
;		10000000	01111111	10000000
;		01000000	00111111	11000000
;		00100000	00011111	11100000
;		00010000	00001111	11110000
;		00001000	00000111	11111000
;		00000100	00000011	11111100
;		00000010	00000001	11111110
;		00000001	00000000	11111111


	and	bx,00000111B		;Get bit mask for bit
	mov	bl,cs:RotBitTbl[bx]	;Assume we're going left.
	dec	bl			;Create mask




;	The assumption has been made that the scan will be right to left.
;	If the scan is left to right, then the first byte mask and the
;	byte count must be adjusted.
;
;	Also set up the correct bias for getting back to the interesting
;	byte for the rep scasb instruction (di is always updated by one
;	byte to many).


	std				;Assume search left
	mov	dx,1			;(to counter post decrement)
	test	DirStyle,StepLeft
	jnz	Scan40			;It is left




;	Compute the first byte mask for the first byte for stepping right.
;
;		Current 	  SHL		  INC		  NOT
;
;		01111111	11111110	11111111	00000000
;		00111111	01111110	01111111	10000000
;		00011111	00111110	00111111	11000000
;		00001111	00011110	00011111	11100000
;		00000111	00001110	00001111	11110000
;		00000011	00000110	00000111	11111000
;		00000001	00000010	00000011	11111100
;		00000000	00000000	00000001	11111110


	cld				;Going right, fix up dir flag
	shl	bl,1			;Fix up first bit mask per above
	inc	bl
	not	bl


;	Compute the number of bytes from current position to end of scanline
;	and set adjustment to counter the rep's post increment

	sub	cx,di			;Fix up byte count
	neg	cx
	inc	cx
	neg	dx			;(to counter post increment)




;	Set the pixel count for the entire scan.  The scanning will actually
;	continue until the end of the scan as given in bmWidthBytes, and
;	the result clipped to bmWidth.

Scan40:
	not	bl			;Need the NOT of the first byte mask
	shl	di,1			;Set di = pixel count of entire scan
	shl	di,1			;  line (rounded up)
	shl	di,1
	cmp	Planes,3		;Color scan?
	jne	Scan100 		;  No, monochrome





;	For color, the three planes will be XORed with the color for that
;	plane, and the results of each XOR will be ORed together.  This
;	will result in all pixels of the given color being a 0, and all
;	pixels not of the color being 1.  Searching for color can be
;	handled with an XOR mask to selectively invert the result.
;
;
;	Currently:	ds:si --> bitmap or display
;			es:si --> bitmap or display
;			bl = first byte mask
;			cx = byte count
;			dx = direction bias
;			di = bits/scanline


Scan50:
	push	di			;Save bits/scanline
	mov	dx,bx			;bx will be needed
	mov	dh,bptr (Color)+Red	;Get search color
	mov	di,wp ((Color)+Green)
	mov	bx,NextPlane		;Set index to next plane

Scan60:
	mov	al,[bx][si]		;Get green plane's byte
	add	bx,bx
	mov	ah,[bx][si]		;Get blue plane's value
	xor	ax,di			;XOR the colors
	or	ah,al			;Combine them
	lodsb				;Get red plane's byte
	xor	al,dh			;XOR the color
	or	al,ah			;Combine them
	xor	ah,ah			;Assume searching for not color
	test	DirStyle,FindColor
	jz	Scan70			;Searching for not color
	dec	ah			;Searching for color, invert result

Scan70:
	xor	al,ah			;Adjust the color
	and	al,dl			;Mask out the bits that don't count
	jnz	Scan90			;  Hit.  Check it out
	dec	cx			;Any bytes left?
	jz	Scan90			;  No

	push	bp			;Set bp = offset to blue plane
	mov	bp,bx
	shr	bx,1			;Set offset to green plane
	mov	dl,ah			;Save final invert mask

Scan80:
	mov	al,[bx][si]		;Get green plane's byte
	mov	ah,ds:[bp][si]		;Get blue plane's value
	xor	ax,di			;XOR the colors
	or	ah,al			;Combine them
	lodsb				;Get red plane's byte
	xor	al,dh			;XOR the color
	or	al,ah			;Combine them
	xor	al,dl			;Invert if needed
	jnz	Scan85			;Hit
	loop	Scan80			;Miss, try next

Scan85:
	pop	bp			;Was not found
Scan90:
	pop	di
	jnz	short Scan130		;Found the byte
	jmp	short Scan190





;	The desired action of the scan is to be able to do a rep scasb
;	over the scanline until either the color is found or not found.
;	Once the stopping condition is found, it has to be possible to
;	determine which bit was the bit that stopped the scan.
;
;	Monochrome notes:
;
;	    The color will be used as an XOR mask.  If the result of
;	    the XOR is zero, then the byte did not contain any bits of
;	    importance, otherwise we made a hit and need to return the
;	    location of it.
;
;	    If searching for the color, the color must be complemented
;	    so that the XOR will set all bits not of the color to zero,
;	    and leave all bits of the color 1's.  If searching for NOT
;	    the color, then the color can be left as is so that all bits
;	    of the color will be set to zero.  The complement also gives
;	    the compare value for the scasb instruction.
;
;
;	Currently:	ds:si --> bitmap or display
;			es:si --> bitmap or display
;			bl = first byte mask
;			cx = byte count
;			dx = direction bias
;			di = bits/scanline


Scan100:
	mov	ax,OFF_Color		;Get search color
	test	DirStyle,FindColor	;Search for color or NOT color?
	jz	Scan110 		;Searching for NOT color
	not	ax			;Searching for color, so complement
	not	bh			;  the search color
	errnz	FindColor-00000001b



;	Check the first byte for a hit or miss.

Scan110:
	lodsb				;Get the first byte
	xor	al,ah			;Adjust the color
	and	al,bl			;Mask out the bits that don't count
	jnz	Scan130 		;  Hit.  Check it out

	mov	al,ah			;Otherwise restore register for scan
	dec	cx			;Any bytes left to check?
	jz	Scan190 		;  No, show not found

	xchg	si,di			;scasb uses es:di
	repe	scasb			;Try for a hit or miss
	jz	Scan190 		;Scaned off the end, it's a miss
	inc	cx			;Decremented one time too many
	xchg	si,di
	add	si,dx			;Adjust from post increment/decrement
	lodsb				;Get the byte which we hit on
	xor	al,ah			;Adjust to look for a set bit



;	Had a hit.  Find which pixel it was really in.
;
;	Currently:	cx = byte index pixel is in
;			di = #pixels in the scan line
;			al = byte hit was in

Scan130:
	shl	cx,1			;Convert byte index to pixel index
	shl	cx,1
	shl	cx,1
	test	DirStyle,Stepleft	;Scanning Right to left?
	jnz	Scan160 		;  yes

Scan140:
	sub	cx,di			;Compute index of first pixel in byte
	neg	cx			;  where hit occured
	dec	cx			;Prepare for loop

Scan150:
	inc	cx			;Show next pixel
	shl	al,1			;Was this the hit?
	jnc	Scan150 		;  No, try next
	cmp	cx,WidthBits		;Is final x value in range?
	jge	Scan190 		;  No, show not found
	jmp	short Scan170		;  Yes, return it

Scan160:
	dec	cx			;Show next pixel
	shr	ax,1			;Was this the hit?
	jnc	Scan160 		;  No, try next

Scan170:
	mov	ax,cx			;Return position to caller

Scan180:
	cld

cEnd	dmScanLR


Scan190:
	mov	ax,-1
	jmp	Scan180


	ifdef	debug
	public	Scan30
	public	Scan31
	public	Scan32
	public	Scan33
	public	Scan35
	public	Scan40
	public	Scan50
	public	Scan60
	public	Scan70
	public	Scan80
	public	Scan85
	public	Scan90
	public	Scan100
	public	Scan110
	public	Scan130
	public	Scan140
	public	Scan150
	public	Scan160
	public	Scan170
	public	Scan180
	public	Scan190
	endif

sEnd
end
