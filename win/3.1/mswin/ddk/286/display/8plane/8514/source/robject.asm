page            ,132
title           Translate Passed Logical Pens & Brushes into Physical Objects
.286c

.xlist
	include CMACROS.INC
	include 8514.inc
	include brush.inc	; new brush defintion
.list

DITHERSTYLE	    equ     5

externFP	GetPalette		;in TBINIT.ASM

sBegin	Data
externB TranslateTable
externB BitsPixel
externB WriteEnable
externB Palette 			;in PALETTE.DAT
externB PhysicalIndexTable		;defined in palette.dat
externB ColorFlagTable
externB SystemPaletteColors
sEnd

subttl          Realize Object Dispatch Code
page +
sBegin  Code
assumes cs,Code
assumes ds,Data

subttl  Various Tables for RealizeObject
page +                                      
public  RealizeObjectDispatchTable
RealizeObjectDispatchTable      label   word
        dw      RORealizePen
	dw	RORealizeBrush

BrushStyleTable label	word
	dw	ROSolidBrush		;solid brush
	dw	ROHollowBrush		;hollow brush
	dw	ROHatchedBrush		;hatched brush
	dw	ROPatternedBrush	;patterned brush

public  ObjectSizeTable
ObjectSizeTable                 label   word
	dw	3		;size required for a pen
	dw	size NewBrush	;size required for a brush structure (78 bytes)
        dw      0               ;no size required for fonts

public  HatchPatterns
HatchPatterns		label	byte
	db	0ffh, 7 dup(0)				;horizontal hatch ==
	db	8 dup(80h)				;vertical hatch ||
	db	80h, 40h, 20h, 10h, 8, 4, 2, 1		;diagonal hatch \\
	db	1, 2, 4, 8, 10h, 20h, 40h, 80h		;diagonal hatch //
	db	0ffh, 7 dup(80h)			;cross hatch ++
	db	81h, 42h, 24h, 18h, 18h, 24h, 42h, 81h	;diagonal X-hatch

RotationTable	label	byte
	db	0		;use this table to phase align the brush
	db	7		;pattern to the origin passed in from GDI
	db	6
	db	5
	db	4
	db	3
	db	2
	db	1

externW         _cstods                 ;in BITBLT.ASM
externNP	ColourTranslate 	;in COLOUR.ASM
externNP	Dither			;in DITHER.ASM
externNP	ColorDither		;in DITHERS.ASM
externNP	GetRGBOfIndex		;in COLORONF.ASM

;       The RealizeObject routine is one of the most important functions in the
;driver.  To "realize an object" simply means to convert a pen, brush, or font
;which is in device independent form into the most convenient form for the
;device driver to use.  The device independent format is documented in the 
;device drivers guide.  The device dependent objects are documented here:
;
;       Pens:
;               Byte 0 --  Style (same as in the device independent format)
;		Byte 1 --  Colour (a one byte colour index)
;		Byte 3 --  Mono bit in LSB
;
;	Brushes:
;		Structure definition in BRUSH.INC
;
;NOTE: Since this device has no hardware fonts, we return an error if 
;the realize font routine is called.

cProc	RealizeObject, <FAR, PUBLIC, PASCAL, WIN>, <si,di>

        parmD   lpDevice
        parmW   Style
        parmD   lpInObj
        parmD   lpOutObj
        parmD   lpPatternOrigin

	localB	<bTrueRed, bTrueGreen, bTrueBlue, bBitsPixel>
	localB	bConvertToMono
	localV	TempPatternBitmap, %(size NewBrush)
	localD	dTranslatedColor

cBegin
	cld
	mov	bx,Style
	or	bx,bx			;if sign flag set, then this is a request
					;for a "delete physical object"
	jns	RODispatch		;no sign, flag, continue
	jmp	ROGoodExit		;This driver keeps no objects, so
					;nothing needs to be done

public  RODispatch
RODispatch:
	mov	al, [BitsPixel]
	mov	bBitsPixel, al
	lds	si,lpInObj		;--> logical object
	dec	bx			;make it offset from 0
	cmp	bx,2			;Is it a pen or a brush?
	ja	ROError 		;nope, return an error
	shl	bx,1			;make it a word offset into tables
	cmp	seg_lpOutObj,0		;if the pointer to the output info is 0, they
					;want us to return the size needed for physical
					;objects
	je	ROReturnObjectSizes	;return the sizes
	jmp	cs:[RealizeObjectDispatchTable+bx]
					;go realize the object

public  ROReturnObjectSizes
ROReturnObjectSizes:
	mov	ax,cs:ObjectSizeTable[bx]
					;Get size for the object from the table
	jmp	ROExit			;and we're done!

ROError:
	xor	ax,ax			;return an error
	jmp	ROExit


subttl          Realize Pen Routines
page +                              
public  RORealizePen
RORealizePen:

;Entry:
;       DS:SI points to the passed logical pen (lpInObj).

	sub	cx, cx			;assume palette color (mono bit=0)
	lodsw				;get the pen style into AX
	cmp	al,5			;is it a null pen?
	je	RORPStorePenStyle	;yes, continue
	xor	ax,ax			;otherwise, make it a solid pen

RORPStorePenStyle:
	les	di,lpOutObj		;point ES:DI at our output information
	stosb				;get the style into OutObj
	mov	ax, [si+4]		;get RGB colour into DL:AX
	mov	dx, [si+6]
	or	dh, dh			; do we already have an index (dh=0ffh)
	js	ROPenIndexOkay		; yes, index is in al
	cCall	ColourTranslate 	;no, go translate the color into index
	mov	al,dh			;get the physical pen colour into AL
ROPenIndexOkay:
	stosb				;and save it in OutObj
	shr	cl, 1
	sbb	al, al			;AL: mono bit for color just saved
	stosb				;save it as well
	jmp	ROGoodExit		;we're done, get out


subttl          Realize Brush Routines
page +

;NOTE:   A HOLLOW brush is one which can never be seen.  The only time that this
;        type of brush can be used is if a BOARDBLT is doing a NOT operation.
;        Then, it will pass this type of brush (if that's what was passed to it)
;        and HandleBrush will go ahead and draw a NOT just like a solid brush.
;Entry:
;       DS:SI points to the passed logical brush.

public  RORealizeBrush
RORealizeBrush:
	lodsw			;now AL has the passed brush style
	cmp	al,3		;have we a patterned brush?
	ja	ROError 	;anything above patterned means we don't do it
	shl	ax, 1
	mov	bx, ax
	jmp	cs:[bx].BrushStyleTable

public	ROHollowBrush
ROHollowBrush:
	shr	ax, 1

page                                                                   
public  ROSolidBrush
ROSolidBrush:
	les	di,lpOutObj	;point ES:DI at our physical brush
	stosb			;get the style into OutObj (0 or 1)
	.errnz	bnStyle
	.errnz	bnColor-bnStyle-1
	.errnz	bnPattern-bnColor-1
	push	si		;save pointer to passed RGB colour
	lodsw			;now AX has the RED+GREEN RGB colour desired
	mov	dx,ax		;save the RED+GREEN in DX for now
	lodsw			;now AX has the BLUE RGB colour desired
	xchg	ax,dx		;now AX has the RED+GREEN and DL has the blue

	push	ax		;A solid brush has a monochrome component.
	push	dx		;It is created by taking the average of the
	push	di		;RGB that was passed in and creating a
	inc	di		;dither pattern based upon that average.

;We have a problem with realizing the mono portion of the brush if the 
;the brush color is a palette index.  What we're going to do is look up the
;color from the translated LUT and create a mono dither based upon this RGB.

	or	dh,dh		;Is this a palette index?
	jns	short @f	;No. proceed with the RGB in AX:DX
	push	ds
	mov	ds, cs:_cstods
	lea	bx,TranslateTable
	xor	ah,ah		;Yes. Lookup RGB from palette index.
	xlatb
	lea	si,dTranslatedColor
	farPtr	lpPalette,ss,si
	push	es
	cCall	GetPalette,<ax,1,lpPalette>
	mov	ax,word ptr dTranslatedColor
	mov	dx,word ptr dTranslatedColor+2
	pop	es
	pop	ds
@@:

ifdef	_NEW_COLOR_ADD
	mov	dh, ah		;DH: green
	sub	ah, ah		;AX: red
	shl	ax, 1		;multiply by 2
	mov	si, ax		;SI: color accumulator
	mov	al, dh
	sub	ah, ah		;AX: green
	add	si, ax
	shl	ax, 2		;multiply by 5
	add	si, ax		;accumulate with red
	sub	dh, dh
	add	si, dx		;add blue
	add	si, 4		;for rounding for /8
	shr	si, 3		;divide accumulated, weighted colors by 8
	mov	dx, si		;now DL: (2*R+5*G+1*B+4)/8
else
	sub	dh, dh		;the resulting 8 byte pattern is stored in
	add	dl, al		;bytes 2..9 (incl.) of the PBrush structure.
	adc	dh, dh		;see also BRUSH.INC for a complete definition
	add	dl, ah		;of the fields in the PBrush
	adc	dh, 0
	mov	ax, dx
	sub	dx, dx
	mov	bx, 3
	div	bx
	mov	dx, ax
endif
	sub	si, si
	cCall	Dither		;now ES:DI-->bnBgColor
	.errnz	bnBgColor-bnPattern-8
	.errnz	bnFgColor-bnBgColor-1
	mov	ax, 0ffh
	stosw			;make bgColor white, text color black
	sub	di, bnColorBits-bnPattern
	mov	word ptr es:[di].bnBgMono[-2], ax
	lea	si, [di+6]
	.errnz	bnColorBits-bnFgColor-1
	cCall	RotateMonoPattern	;rotate dither pattern in X and Y
	pop	di
	pop	dx
	pop	ax

	pop	si		;restore ptr to RGB in log brush
	mov	bTrueRed, al	; keep the requested colours just in case we
	mov	bTrueGreen, ah	; need to dither the solid brush
	mov	bTrueBlue, dl
	or	dh, dh		;is it RGB or index?
	js	ROIndexSolidBrush
	cCall	ColourTranslate ;go make this into a physical colour

	cmp	ax,[si] 	;was it an exact match on RED & GREEN?
	jne	RODitherBrush	;nope, better go dither
	cmp	dl,[si+2]	;was it an exact match on BLUE?
	je	ROTrueSolidBrush;yes, we need not dither

RODitherBrush:
	cCall	ColourDither	;go dither the brush
	lds	si, lpOutObj	;now rotate this dithered brush.  To do
	add	si, bnColorBits ; so make DS:SI point to the dither
	mov	dx, PDIMENSION	; pattern bits and jump to ROPBColour
	mov	bBitsPixel, 8	;Always invoke 8 b/p bit transfer code to
	mov	bConvertToMono, 0   ; rotate dither bmp.  Don't create mono
	jmp	ROPBColour	; pattern from color pattern

ROTrueSolidBrush:
	mov	al,dh
ROIndexSolidBrush:		;we have the color index in al already
	stosb			;and store the physical colour in the brush
	jmp	ROGoodExit	;we're done now

ROHatchPaletteColor:
	mov	dh, al		;copy color index into DH
	sub	cl, cl		;convert to black in mono DCs
	jmp	short ROSaveColorHatched

page
public	ROHatchedBrush
ROHatchedBrush:
	;Our Hatched PBrush structure will look like a coloured pattern brush.

	les	di, lpOutObj
	mov	al,4		;hatched style = colour patterned style = 4
	stosb			;save it in PBrush
	.errnz	bnStyle
	lodsw			;now AX has the RED+GREEN RGB colour desired
	mov	dx,ax		;save the RED+GREEN in DX for now
	lodsw			;now AX has the BLUE RGB colour desired
	xchg	ax,dx		;now AX has the RED+GREEN and DL has the blue
	or	dh, dh		;is it a palette manager definded color?
	js	ROHatchPaletteColor
	cCall	ColourTranslate ;no, go translate the RGB colour into physical
ROSaveColorHatched:		;colour (forground color)
	mov	es:[di].bnFgColor-1, dh ;save the hatch (text) colour
	shr	cl, 1
	sbb	al, al
	mov	es:[di].bnFgMono-1, al	;save monochrome bit for text color
	lodsw			;get the hatch style into AX
	shl	ax,3		;multiply by 8 bytes per hatch pattern
	mov	bx,ax		;save it temporarily in BX
	lodsw			;get the background colour into AL
	mov	es:[di].bnBgColor-1,al	;save it in our PBrush
	shr	ah, 1
	sbb	al, al
	mov	es:[di].bnBgMono-1, al	;save mono bit as well
	inc	di
	.errnz	bnPattern-bnColor-1
	lea	si, cs:[bx].HatchPatterns
	mov	cx, 4
rep	movs	word ptr es:[di], word ptr cs:[si]
	.errnz	bnBgColor-bnPattern-8
	lea	si, [di-2]
	sub	di, bnBgColor-bnPattern
	cCall	RotateMonoPattern
	jmp	ROGoodExit

page
public  ROPatternedBrush
ROPatternedBrush:
	mov	bConvertToMono, 1	;if col. pat. br., create mono pattern
	lds	si,[si+0]		;get the pointer to the brush BITMAP
					;data structure into DS:SI
	mov	dx, [si+6]		;get number of bytes per scanline in DX
	cmp	byte ptr [si+9],1	;is the brush in a monochrome format?
	lds	si,[si+10]		;(make DS:SI point at the bitmap bits
					; for this brush)
	jne	ROPBColour		;nope, go do a colour brush

public  ROPBMonochrome
ROPBMonochrome:

;At this point DS:SI points to the brush bits for our mono brush
;	       DX contains the number of bytes per scanline

	les	di, lpOutObj
	mov	al,2			;get mono brush style code into our
	stosb				;temporary PBrush data structure
	.errnz	bnStyle
	inc	di			;advance to pattern part to PBrush
	.errnz	bnColor-bnStyle-1
	.errnz	bnPattern-bnColor-1
	dec	dx			;lodsb instr. increments SI by 1
	mov	bx, dx			;BX: line to line offset
	.errnz	bnBgColor-bnPattern-8	;verify that assumption
	mov	cx, 4

ROPBMonoLoop:
	lodsb				;fetch first byte from scanline
	mov	ah, al			;save it in AH
	add	si, bx			;advance to next scanline
	lodsb				;fetch first byte from next scanline
	add	si, bx			;advance to next scanline
	xchg	al, ah			;swap pixels
	stosw				;store them
	loop	ROPBMonoLoop
	sub	ax, ax			;now store background color (0ffh) and
	dec	al			;text color (0h)
	stosw
	.errnz	bnFgColor-bnBgColor-1
	.errnz	bnColorBits-bnFgColor-1
	lea	si, [di-4]		;ES:SI-->last word of mono pattern
	sub	di, bnColorBits-bnPattern
	mov	word ptr es:[di].bnBgMono-2, ax
	cCall	RotateMonoPattern
	jmp	ROGoodExit

FetchFourBitsPerPixelBmp:

;at this point DS:SI --> bitmap bits, si is pushed on stack, es=ss,
;es:di --> TempPatternBitmap, DX=Number of bytes per scanline
;BX=AX= Y origin % 8, AX is pushed on stack, CX=OuterLoop1Counter

	mov	ax,dx
	imul	bl
	add	si,ax
	call	TransferLines4
	pop	cx
	pop	si
	jcxz	RotateYDone
	call	TransferLines4
	jmp	short RotateYDone

public  ROPBColour
ROPBColour:
	push	ss			;make ES==SS
	pop	es
	lea	di, TempPatternBitmap	;ES:DI-->TempPatternBitmap
	push	si			;DS:SI-->Bitmap bits, DX: bmWidthBytes
	mov	bx, seg_lpPatternOrigin ;get Y pattern origin
	and	bx, 07h 		;modulo 8
	mov	al, cs:[bx].RotationTable
	cbw
	mov	bx, ax			;BX: Y origin modulo 8
	mov	cx, PDIMENSION
	.errnz	PDIMENSION-8
	sub	cx, ax
	push	ax

	cmp	bBitsPixel, 8
	jne	FetchFourBitsPerPixelBmp

	mov	ax,dx
	imul	bl
	add	si,ax

	call	TransferLines8

	pop	cx
	pop	si
	jcxz	RotateYDone
	call	TransferLines8
RotateYDone:

	mov	bx, off_lpPatternOrigin ;BX: pattern origin X coordinate
	and	bx, 07h 		;BX: pattern X org. mod 8

	les	di, lpOutObj
	mov	al,5			;get colour brush style code into our
	stosb				;temporary PBrush data structure
	add	di, (bnColorBits-1)	;ES:DI-->color brush pattern
	lea	si, TempPatternBitmap	;SS:SI-->temp brush pattern
	mov	dx, 0807h		;DH: # of rows, DL: wrap mask

RotateBrushX10:
	push	ss:[si][6]
	push	ss:[si][4]
	push	ss:[si][2]
	push	ss:[si][0]
	mov	cx, PDIMENSION/2
RotateBrushX20:
	pop	ax
	mov	es:[di][bx], al
	inc	bx
	and	bl, dl
	mov	es:[di][bx], ah
	inc	bx
	and	bl, dl
	loop	RotateBrushX20
	add	si, PDIMENSION
	add	di, PDIMENSION
	dec	dh
	jnz	RotateBrushX10
	cmp	bConvertToMono, 0	;need to create mono pattern as well?
	je	ROGoodExit		;no.  We're done here

;Here we need to create a mono pattern bmp of the color pattern that was just
;copied and rotated in X and Y.  First create a color index LUT to perform
;the color to monochrome conversion.  Then read the array and index into this
;LUT to get the actual pixel value.

	sub	sp, 256 		;allocate translate table on stack
	push	ss
	pop	es
	mov	di, sp			;ES:DI-->color to mono Xlate table
	cCall	GenerateXlateTable
	mov	bx, sp			;SS:BX-->color to mono Xlate table
	les	di, lpOutObj		;ES:DI-->solid color index
	lea	si, [di].bnColorBits	;ES:SI-->color pattern pixels
	mov	ax, 0ffh
	mov	word ptr es:[di].bnBgColor, ax
	mov	word ptr es:[di].bnBgMono, ax
	inc	di
	.errnz	bnColor-1		;this is what is meant in [-1] above
	mov	al, 1			;any color !black or !white will do
	stosb				;ES:DI-->mono pattern
	mov	cx, 8			;byte (outer) loop count
MakeMonoPattern10:
	mov	dh, cl			;save outer loop count
	mov	cl, 4			;do two pixels at a time
MakeMonoPattern20:
	lods	word ptr es:[si]	;AX: two pixels of color pattern
	xlatb	ss:[bx] 		;convert 1st pixel from color to mono
	shr	al, 1			;and accumulate the result in DL
	rcl	dl, 1
	mov	al, ah			;do the same thing with the 2nd pixel
	xlatb	ss:[bx]
	shr	al, 1
	rcl	dl, 1
	loop	MakeMonoPattern20
	mov	al, dl
	stosb				;save mono pattern bits
	mov	cl, dh			;restore outer loop count
	loop	MakeMonoPattern10
	add	sp, 256 		;de-allocate Xlate table on stack
	errn$	ROGoodExit

ROGoodExit:
	mov	ax,1
ROExit:
cEnd

cProc	GenerateXlateTable, <NEAR, PUBLIC>
cBegin
	mov	ds, cs:_cstods
	assumes ds, Data
	mov	si, DataOFFSET ColorFlagTable
	mov	bl, [WriteEnable]
	sub	bh, bh
	inc	bx			;BX: 16 or 256
	mov	cl, [SystemPaletteColors]
	sub	ch, ch			;CX: 16 or 20
	sub	bx, cx			;BX: 0 or 236
	shr	cl, 2			;do 2 entries at once
	mov	dx, cx			;save # of entries div 4 in DX
UpperCopyLoop:
	lodsw
	shr	ax, 4
	stosw
	loop	UpperCopyLoop

	mov	cx, bx
	jcxz	SetupLowerCopyLoop

	sub	ax, ax			;palette manager defined colors are
	shr	cx, 1			; converted to black
rep	stosw

SetupLowerCopyLoop:
	mov	cx, dx
LowerCopyLoop:
	lodsw
	shr	ax, 4
	stosw
	loop	LowerCopyLoop
cEnd

cProc	RotateMonoPattern, <NEAR, PUBLIC>
cBegin					;ES:DI-->beginning of mono pattern
	std				;ES:SI-->last word of pattern
	mov	dx, 07h 		;DX: bitmask
	mov	bx, 4			;we move 8 bytes of pattern into PBrush
	mov	cx, off_lpPatternOrigin
	and	cx, dx			;CX: pattern X origin mod 7
RotatePatternXLoop:
	lods	word ptr es:[si]	;get two bytes of mono pattern
	ror	al, cl			;perform rotation in X
	ror	ah, cl
	push	ax			;save on stack
	dec	bx			;update loop counter
	jnz	RotatePatternXLoop
	cld				;auto increment again
	mov	bx, seg_lpPatternOrigin
	and	bx, dx
	mov	cl, 4
	.errnz	bnBgColor-bnPattern-8
RotatePatternYLoop:
	pop	ax
	mov	es:[di][bx], al
	inc	bx
	and	bl, dl
	mov	es:[di][bx], ah
	inc	bx
	and	bl, dl
	loop	RotatePatternYLoop
cEnd

TransferLines8	proc near
ROTransferLinesLoop8:
	push	cx
	push	si
	mov	cx, PDIMENSION
rep	movsb
	pop	si
	add	si, dx			;DX: bytes per scanline
	pop	cx
	loop	ROTransferLinesLoop8
	ret
TransferLines8	endp

TransferLines4	proc near
ROTransferLinesLoop4:
	push	cx
	push	si
	mov	cx, (PDIMENSION SHR 1)
ROTransferPixelLoop4:
	lodsb
	mov	ah, al
	shr	al, 4
	stosb
	mov	al, ah
	and	al, 0fh
	stosb
	loop	ROTransferPixelLoop4
	pop	si
	add	si, dx
	pop	cx
	loop	ROTransferLinesLoop4
	ret
TransferLines4	endp

cProc	ColourDither, <NEAR, PUBLIC>
cBegin
	dec	di			;ES:DI-->Output Object (PBrush)
	mov	al, DITHERSTYLE 	; store style and color information in
	stosb				; temporary brush on the stack
	.errnz	bnStyle
	mov	al, dh			;save closest pure color index
	stosb
	.errnz	bnColor-bnStyle-1
	add	di, (bnColorBits-bnPattern)
	sub	ax, ax			;ES:DI-->dither bmp bits
	mov	ds, cs:_cstods
	assumes ds, Data
	mov	al, bTrueRed
	push	ax
	mov	al, bTrueGreen
	push	ax
	mov	al, bTrueBlue
	push	ax
	cCall	ColorDither		;create the dither
cEnd

sEnd	Code
end
