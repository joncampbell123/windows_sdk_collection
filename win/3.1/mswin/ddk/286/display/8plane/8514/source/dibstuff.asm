.xlist
include cmacros.inc
.list

public	    OneFourEightToEight8
public	    OneFourEightToOne8
public	    EightToOneFourEight8
public	    OneToOneFourEight8
public	    DsEsSwap

externA     __AHIncr		    ;huge increment; imported from kernel
if 0
UPPER_TWO	equ	0c0h
UPPER_THREE	equ	0e0h
RMASK		equ	007h
GMASK		equ	038h
BLMASK		equ	0c0h
endif
createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	    DIBSeg

	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

cProc	DIBDummy, <NEAR>
	include dibframe.inc
cBegin	<nogen>
cEnd	<nogen>

cProc	BuildColorTable_4, <NEAR, PUBLIC, WIN, PASCAL, NODATA>, <bx>
	parmD	lpPaletteColors

	localW	lMinLo
	localW	wIndex
	localB	lMinHi

cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	cmp	cx, 1			; if cx=1, don't translate anything
	je	BuildColorTableEndNode

OuterBuildLoop:
	push	cx
	lodsw
	mov	bx, ax
	lodsw				; logical colors are in
	mov	dl, al			; bl: red, bh: green, dl: blue
	mov	cx, 16			; 16=number of colors
	push	ds			; save current pointer to logical
	push	si			; colors
	push	di			; save index
	lds	si, lpPaletteColors
	xor	ax, ax
	dec	ax
	mov	lMinLo, ax		; initialize error term to some
	mov	lMinHi, al		; hideously large value (00ffffffh)

LetsBoogy:
	sub	dh, dh
	lodsb				; get logical red
	sub	al, dl			; subtract red we want
	ja	SquareRed		; if the result was negative change
	neg	al			; the sign of the difference
SquareRed:
	mul	al			; square differnce now
	mov	di, ax			; and save it in di
	lodsb				; now do the same thing with green
	sub	al, bh
	ja	SquareGreen
	neg	al
SquareGreen:
	mul	al
	add	di, ax
	adc	dh, dh
	lodsb				; now compute delta B squared
	sub	al, bl
	ja	SquareBlue
	neg	al
SquareBlue:
	mul	al
	add	di, ax
	adc	dh, 0
	or	di, di			; look for exact match
	jz	PossibleExactMatch
NotExactMatch:
	cmp	lMinHi, dh		; Compare current error term
	ja	SetNewlMin		; with minimal error found previously
	jb	MatchLoopBottom 	; and swap them if necessary
	cmp	lMinLo, di
	ja	SetNewlMin
MatchLoopBottom:
	loop	LetsBoogy
	jmp	short ExitCleanup
SetNewlMin:
	mov	wIndex, cx
	mov	lMinHi, dh
	mov	lMinLo, di
	loop	LetsBoogy
	jmp	short ExitCleanup
PossibleExactMatch:
	or	dh, dh
	jnz	NotExactMatch
	mov	bx, cx
	jmp	short EndRonHatesThis
BuildColorTableEndNode:
	jmp	short BuildColorTableEnd
ExitCleanup:
	mov	bx, wIndex
EndRonHatesThis:
	mov	ax, 16			; 16=number of colors
	sub	ax, bx

	pop	di
	pop	si
	pop	ds
	pop	cx

	stosb
	loop	OuterBuildLoop

BuildColorTableEnd:
cEnd


;*****************************Public*Routine***********************************
;OneToEight is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixels to 8 bit per pixel 8514 color format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    ah	  -> Pixelmask = nColor-1
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine does not check for segment boundary crossing!
;Upon exit: es:di -> next word in PDevice bitmap
;	    ds:si -> next word in DIBmp
;all other general use registers except cx are altered
;******************************************************************************

OneFourEightToEight8	proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx			; keep remaining # of pixels on stack
	lea	bx, bColorTable
	or	dx, dx
	jz	Remainder18
oLoop18:
	push	dx			; keep outer loop count on stack
	mov	dh, ch			; save inner loop count
	lodsb				; get first pixels
	mov	dl, al			; and keep them in a save place
iLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	stosb				; and store the translated value
	dec	ch
	jnz	iLoop18
	mov	ch, dh			; re-initialize iLoop counter
	pop	dx			; get oLoop counter
	dec	dx
	jnz	oLoop18
Remainder18:
	pop	dx			; get the # of remaining bits to do
	or	dl, dl			; dx=0 means no remaining bits to do
	jz	Bypass18		; so let's exit now
	mov	ch, dl			; now do the remaining bits in the
	lodsb				; the current line
	mov	dl, al
rLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	stosb				; and store the translated value
	dec	ch
	jnz	rLoop18
Bypass18:
	ret
OneFourEightToEight8	endp

;*****************************Public*Routine***********************************
;OneToOne is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixles to 1 bit per pixel 8514 `color' format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    ah	  -> Pixelmask = nColor-1
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine does not check for segment boundary crossing!
;Upon exit: es:di -> next word in PDevice bitmap
;	    ds:si -> next word in DIBmp
;all other general use registers except cx are altered
;******************************************************************************

OneFourEightToOne8  proc Near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx
	lea	bx, bColorTable
	or	dx, dx			;if outer loop count == 0 skip the
	jz	OFE1Remainder		;main loop and just do the remainder
OuterLoop:
	push	dx
	mov	al, 8
FetchLoop:
	push	ax
	lodsb				; get pixel(s) into al
	mov	dl, al
	push	cx			; preserve inner loop count
InnerLoop:
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	rcr	al, 1
	rcl	dh, 1
	dec	ch
	jnz	InnerLoop
	pop	cx			; restore inner loop count
	pop	ax
	sub	al, ch
	ja	FetchLoop
	mov	al, dh
	stosb				; store the pixel build last
	pop	dx			; get remaining outer loop count
	dec	dx
	jnz	OuterLoop
OFE1Remainder:
	pop	dx			; get remainder count
	or	dl, dl
	jz	Bypass11
	push	di			;save DI; we'll need it as a count var
	sub	di, di
	mov	al, dl
FetchLoopr:				; at most, build one more byte
	push	ax			; save for later compare
	lodsb				; get pixel(s) into al
	mov	dl, al
	push	cx			; preserve inner loop count
InnerLoopr:
	inc	di
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	rcr	al, 1
	rcl	dh, 1
	dec	ch
	jnz	InnerLoopr
	pop	cx
	pop	ax
	sub	al, ch
	jg	FetchLoopr

	mov	cx, 8
	sub	cx, di
	shl	dh, cl
	mov	al, dh

	pop	di
	stosb				; store the pixel build last
Bypass11:
	ret
OneFourEightToOne8  endp

;*****************************Public*Routine***********************************
;OneToOneFourEight converts an internal single plane 1 bit/pixel bitmap to
;device independent bitmap format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    These pointers will be swapped!
;	    ah	  -> Pixelmask = nColor-1--not needed for this routine
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8 -- # of bytes to write
;This routine does not check for segment boundary crossing!
;The remainder count in bl needs to be adjusted, as, depending on biBitsPixel,
;up to eight bytes are written to the destination bitmap.
;Reentering into this routine after a segment boundary crossing is particularly
;tricky in this routine.
;******************************************************************************

OneToOneFourEight8  proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	sub	ah, ah			;signal: need to fetch source byte
	lea	bx, bColorTable 	; yes, we need to convert the `color'
	or	dx, dx
	jz	Remainder1148
OuterLoop1148:
	push	dx			;save outer loop count
	push	cx			;save inner loop count
	mov	dh, al			;DH: tmp src pixel storeage
	sub	dl, dl			;DL: where to put dest pixel(s); must
InnerLoop1148:				; be init. to 0 for 8 b/p dest support
	or	ah, ah			;time to fetch a new source pixel?
	jnz	NoFetch1148		;no.
	lodsb				;fetch 8 new pixels from Src bmp
	mov	dh, al			;DH: temp source pixel storage
	mov	ah, 8			;AH: src pixels left
NoFetch1148:
	shl	dh, 1			; next byte to be written to the DIBmp
	rcl	al, 1			;get next pixel into LSB of AL
	and	al, 01h 		;look only at LSB
	xlatb	ss:[bx] 		;convert to desired color
	shl	dl, cl			;align destination
	or	dl, al			;add new pixel color index
	dec	ah			;signal: another src pixel done
	dec	ch			;update inner loop count
	jnz	InnerLoop1148
	mov	al, dl			;put new dest pixel(s) into AL
	stosb				; write the newly built byte into DIBmp
	mov	al, dh			;put remaining src pixels (if any) in AL
	pop	cx			; restore iLoop count and check if we
	pop	dx			; restore outer loop count
	dec	dx
	jnz	OuterLoop1148

Remainder1148:
	pop	dx			; get remainder count off the stack
	or	dl, dl
	jz	WeAreDone148to8

	push	cx			;save normal inner loop parameters
	mov	ch, dl			;CH: remainder->inner loop count
	mov	dh, al			;DH: remaining SRC pixels (if any)
InnerLoop1148r:
	or	ah, ah			;time to read another byte from source?
	jnz	NoFetch1148r		;no
	lodsb				;fetch 8 new pixels
	mov	dh, al			;save them in DH
	mov	ah, 8			;AH: src pixels left counter
NoFetch1148r:
	shl	dh, 1			;next pixel in carry
	rcl	al, 1			;next pixel in LSB of AL
	and	al, 01h 		;mask off other junk
	xlatb	ss:[bx] 		;expand to desired color
	shl	dl, cl			;aling dest.
	or	dl, al			;add new pixel to dest
	dec	ah			;another src pixel processed
	dec	ch			;update inner loop count
	jnz	InnerLoop1148r
	mov	al, dl
	pop	cx			; restore iLoop and shift count
	sub	ch, bRem		;CH: # of dest shifts by CL left to do
PutRemainderInPlaceLoop:		; to phase align dest byte
	shl	al, cl
	dec	ch
	jnz	PutRemainderInPlaceLoop

	stosb				; write the newly built byte into DIBmp
WeAreDone148to8:
	call	DsEsSwap
	ret
OneToOneFourEight8  endp

;*****************************Public*Routine***********************************
;EightToOneFourEight is the translation routine that will convert an 8 bit
;internal bitmap to the device independent bitmap format of either 1, 4, or 8
;bits per pixel.
;On entry:  es:di -> DIBitmapPDevice
;	    ds:si -> PDevice Bitmap
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine does not check for segment boundary crossings!
;Upon exit: es:di -> next word in DIBmp
;	    ds:si -> next word in PDevice bitmap
;all other general use registers except cx are altered
;******************************************************************************

EightToOneFourEight8	proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	lea	bx, bColorTable
	or	dx, dx
	jz	Remainder8148
oLoop8148:
	push	dx
	mov	dh, ch
	sub	dl, dl
iLoop8148:
	shl	dl, cl
	lodsb
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	iLoop8148
	mov	al, dl
	stosb
	mov	ch, dh
	pop	dx
	dec	dx
	jnz	oLoop8148
Remainder8148:
	pop	dx
	or	dl, dl
	jz	Bypass8148
	mov	dh, ch
	mov	ch, dl
	mov	al, 8			; compute the shift count necessary to
	sub	al, dl			; allign the last, incomplete byte such
	mul	cl			; that there are no gaps in the bitmap
	push	ax
	sub	dl, dl
iLoop8148r:
	shl	dl, cl
	lodsb
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	iLoop8148r
	mov	al, dl
	mov	ch, cl
	pop	cx
	shl	al, cl
	stosb
	mov	cl, ch
	mov	ch, dh
Bypass8148:
	call	DsEsSwap
	ret
EightToOneFourEight8	endp

;*****************************Public*Routine***********************************
;This routine swaps si with di and ds with es.	It is used to swap pointers
;when writing to a DIBmp and reading from PDevice.
;******************************************************************************

DsEsSwap    proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	ds
	push	es
	pop	ds
	pop	es
	push	si
	push	di
	pop	si
	pop	di
	ret
DsEsSwap    endp


;*****************************Public*Routine***********************************
;ReadDIB() will read a byte from DS:SI into AL, update SI, and, in case of a
;segment boundary crossing, update DS.
;StoreDIB() writes the byte in AL to ES:DI, updates DI and in case of a seg.
;bnd. crossing, updates ES.
;******************************************************************************

cProc	ReadDIB, <NEAR, PUBLIC>
cBegin
	lodsb
	or	si, si
	jz	RDUpdateSelector
	ret
RDUpdateSelector:
	mov	si, ds
	add	si, __AHIncr
	mov	ds, si
	sub	si, si
cEnd

cProc	StoreDIB, <NEAR, PUBLIC>
cBegin
	stosb
	or	di, di
	jz	SDUpdateSelector
	ret
SDUpdateSelector:
	mov	di, es
	add	di, __AHIncr
	mov	es, di
	sub	di, di
cEnd

;*****************************Public*Routine***********************************
;OneToEight is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixles to 8 bit per pixel 8514 color format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    ah	  -> Pixelmask = nColor-1
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine DOES check for segment boundary crossings!
;Upon exit: es:di -> next word in PDevice bitmap
;	    ds:si -> next word in DIBmp
;all other general use registers except cx are altered
;******************************************************************************

cProc	Partial148to8, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx			; keep remaining # of pixels on stack
	lea	bx, bColorTable
	or	dx, dx
	jz	pRemainder18
poLoop18:
	push	dx			; keep outer loop count on stack
	mov	dh, ch			; save inner loop count
	cCall	ReadDIB 		; get first pixels
	mov	dl, al			; and keep them in a save place
piLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	stosb				; and store the translated value
	dec	ch
	jnz	piLoop18
	mov	ch, dh			; re-initialize iLoop counter
	pop	dx			; get oLoop counter
	dec	dx
	jnz	poLoop18
pRemainder18:
	pop	dx			; get the # of remaining bits to do
	or	dl, dl			; dx=0 means no remaining bits to do
	jz	pBypass18		; so let's exit now
	mov	ch, dl			; now do the remaining bits in the
	cCall	ReadDIB 		; the current line
	mov	dl, al
prLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	stosb				; and store the translated value
	dec	ch
	jnz	prLoop18
pBypass18:
cEnd

;*****************************Public*Routine***********************************
;OneToOne is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixles to 1 bit per pixel 8514 `color' format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    ah	  -> Pixelmask = nColor-1
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine DOES check for segment boundary crossings!
;Upon exit: es:di -> next word in PDevice bitmap
;	    ds:si -> next word in DIBmp
;all other general use registers except cx are altered
;******************************************************************************

cProc	Partial148to1, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx
	lea	bx, bColorTable
	or	dx, dx			;if outer loop count == 0 skip the
	jz	pOFE1Remainder		;main loop and just do the remainder
pOuterLoop:
	push	dx
	mov	al, 8
pFetchLoop:
	push	ax
	cCall	ReadDIB 		; get pixel(s) into al
	mov	dl, al
	push	cx			; preserve inner loop count
pInnerLoop:
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	rcr	al, 1
	rcl	dh, 1
	dec	ch
	jnz	pInnerLoop
	pop	cx			; restore inner loop count
	pop	ax
	sub	al, ch
	ja	pFetchLoop
	mov	al, dh
	stosb				; store the pixel build last
	pop	dx			; get remaining outer loop count
	dec	dx
	jnz	pOuterLoop
pOFE1Remainder:
	pop	dx			; get remainder count
	or	dl, dl
	jz	pBypass11
	push	di			;save DI; we'll need it as a count var
	sub	di, di
	mov	al, dl
pFetchLoopr:				 ; at most, build one more byte
	push	ax			; save for later compare
	cCall	ReadDIB 		; get pixel(s) into al
	mov	dl, al
	push	cx			; preserve inner loop count
pInnerLoopr:
	inc	di
	rol	dl, cl
	mov	al, dl
	and	al, ah
	xlatb	ss:[bx]
	rcr	al, 1
	rcl	dh, 1
	dec	ch
	jnz	pInnerLoopr
	pop	cx
	pop	ax
	sub	al, ch
	jg	pFetchLoopr

	mov	cx, 8
	sub	cx, di
	shl	dh, cl
	mov	al, dh

	pop	di			;restore DI from above
	stosb				; store the pixel build last in bmp
pBypass11:
cEnd

;*****************************Public*Routine***********************************
;EightToOneFourEight is the translation routine that will convert an 8 bit
;internal bitmap to the device independent bitmap format of either 1, 4, or 8
;bits per pixel.
;On entry:  es:di -> DIBitmapPDevice
;	    ds:si -> PDevice Bitmap
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8
;This routine DOES check for segment boundary crossings!
;Upon exit: es:di -> next word in DIBmp
;	    ds:si -> next word in PDevice bitmap
;all other general use registers except cx are altered
;******************************************************************************

cProc	Partial8To148, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	lea	bx, bColorTable
	or	dx, dx
	jz	pRemainder8148
poLoop8148:
	push	dx
	mov	dh, ch
	sub	dl, dl
piLoop8148:
	shl	dl, cl
	lodsb
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	piLoop8148
	mov	al, dl
	cCall	StoreDIB
	mov	ch, dh
	pop	dx
	dec	dx
	jnz	poLoop8148
pRemainder8148:
	pop	dx
	or	dl, dl
	jz	pBypass8148

	mov	dh, ch
	mov	ch, dl
	mov	al, 8			; compute the shift count necessary to
	sub	al, dl			; allign the last, incomplete byte such
	mul	cl			; that there are no gaps in the bitmap
	push	ax
	sub	dl, dl
piLoop8148r:
	shl	dl, cl
	lodsb
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	piLoop8148r
	mov	al, dl
	mov	ch, cl
	pop	cx
	shl	al, cl
	cCall	StoreDIB
	mov	cl, ch
	mov	ch, dh
pBypass8148:
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;OneToOneFourEight converts an internal single plane 1 bit/pixel bitmap to
;device independent bitmap format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    These pointers will be swapped!
;	    ah	  -> Pixelmask = nColor-1--not needed for this routine
;	    bl	  -> rem = nWidth*biBitsPixel % 8
;	    cl	  -> shift count = biBitsPixel % 8
;	    ch	  -> iCount = 8 / biBitsPixel
;	    dx	  -> oCount = nWidth*biBitsPixel / 8 -- # of bytes to write
;This routine DOES check for segment boundary crossings!
;The remainder count in bl needs to be adjusted, as, depending on biBitsPixel,
;up to eight bytes are written to the destination bitmap.
;******************************************************************************

cProc	Partial1to148, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	sub	ah, ah			;signal: need to fetch source byte
	lea	bx, bColorTable 	; yes, we need to convert the `color'
	or	dx, dx
	jz	pRemainder1148
pOuterLoop1148:
	push	dx			;save outer loop count
	push	cx			;save inner loop count
	mov	dh, al			;DH: tmp src pixel storeage
	sub	dl, dl			;for 1-->8b/p DIB support, clear DL
pInnerLoop1148:
	or	ah, ah			;time to fetch a new source pixel?
	jnz	pNoFetch1148		;no.
	lodsb				;fetch 8 new pixels from Src bmp
	mov	dh, al			;DH: temp source pixel storage
	mov	ah, 8			;AH: src pixels left
pNoFetch1148:
	shl	dh, 1			; next byte to be written to the DIBmp
	rcl	al, 1			;get next pixel into LSB of AL
	and	al, 01h 		;look only at LSB
	xlatb	ss:[bx] 		;convert to desired color
	shl	dl, cl			;align destination
	or	dl, al			;add new pixel color index
	dec	ah			;signal: another src pixel done
	dec	ch			;update inner loop count
	jnz	pInnerLoop1148
	mov	al, dl			;put new dest pixel(s) into AL
	cCall	StoreDIB		; write the newly built byte into DIBmp
	mov	al, dh			;put remaining src pixels (if any) in AL
	pop	cx			; restore iLoop count and check if we
	pop	dx			; restore outer loop count
	dec	dx
	jnz	pOuterLoop1148

pRemainder1148:
	pop	dx			; get remainder count off the stack
	or	dl, dl
	jz	pWeAreDone148to8

	push	cx			;save normal inner loop parameters
	mov	ch, dl			;CH: remainder->inner loop count
	mov	dh, al			;DH: remaining SRC pixels (if any)
pInnerLoop1148r:
	or	ah, ah			;time to read another byte from source?
	jnz	pNoFetch1148r		;no
	lodsb				;fetch 8 new pixels
	mov	dh, al			;save them in DH
	mov	ah, 8			;AH: src pixels left counter
pNoFetch1148r:
	shl	dh, 1			;next pixel in carry
	rcl	al, 1			;next pixel in LSB of AL
	and	al, 01h 		;mask off other junk
	xlatb	ss:[bx] 		;expand to desired color
	shl	dl, cl			;aling dest.
	or	dl, al			;add new pixel to dest
	dec	ah			;another src pixel processed
	dec	ch			;update inner loop count
	jnz	pInnerLoop1148r
	mov	al, dl
	pop	cx			; restore iLoop and shift count
	sub	ch, bRem		;CH: # of dest shifts by CL left to do
pPutRemainderInPlaceLoop:		; to phase align dest byte
	shl	al, cl
	dec	ch
	jnz	pPutRemainderInPlaceLoop

	cCall	StoreDIB		; write the newly built byte into DIBmp
pWeAreDone148to8:
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;24To8 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 8 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine DOES check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	Partial24To8, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	mov	cx, dx
pTwentyfourTo8Loop:
	cCall	ReadDIB 		; red in al
	mov	dl, al
	cCall	ReadDIB 		; get green
	mov	ah, al
	cCall	ReadDIB 		; get blue
	xchg	al, dl
	push	cx
	push	es
	cCall	lpDeviceColorMatch, <dx, ax, lpConversionInfo>
	pop	es
	pop	cx
	stosb				; store in our bitmap
	loop	pTwentyfourTo8Loop
cEnd

;*****************************Public*Routine***********************************
;24To1 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 1 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    cx:   -> restart parameters
;	    dx:   -> number of pixels to convert--be consistent w/ other xlate
;This routine DOES check for segment boundary crossings!
;On Exit:   si, di are updated, ax, bx, cx, dx have been altered
;******************************************************************************

cProc	Partial24To1, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

pOuter:
	mov	cx, 8
	sub	bx, bx
pInner:
	push	cx
	push	bx
	cCall	ReadDIB 		; red in al
	mov	dl, al
	cCall	ReadDIB 		; get green
	mov	ah, al
	cCall	ReadDIB 		; get blue
	xchg	al, dl
	push	cx
	push	es
	cCall	lpDeviceColorMatch, <dx, ax, lpConversionInfo>
	pop	es
	pop	cx
	pop	bx
	shr	al, 1
	rcl	bl, 1
	pop	cx
	loop	pInner
	mov	al, bl
	stosb
	dec	dx
	jnz	pOuter
cEnd

;*****************************Public*Routine***********************************
;OneTo24 is used to convert a one bit per pixel internal bitmap to 24 bit/pixel
;DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these two pointers will be swapped
;	    dx:   -> oCount--number of pixels/scanline-an even multiple of 8
;	    cx:   -> Pertinent restart information
;This procedure DOES check for segment boundary crossings
;If the source pixel is zero, three bytes of 0 will be written into the DIBmp,
;if the source pixel is one, three bytes of 0ffh will be written instead.
;******************************************************************************

cProc	Partial1To24, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	mov	bx, dx
	and	bx, 07h
	shr	dx, 1
	shr	dx, 1
	shr	dx, 1
	mov	cx, dx

pOuterLoop124:
	push	cx
	mov	cx, 8
	lodsb				;read 8 pixels from bmp
	mov	dl, al			;put them into DL
pInnerLoop124:
	shl	dl, 1			;shift next pixel into carry CY if 1
	sbb	al, al
	cCall	StoreDIB
	cCall	StoreDIB
	cCall	StoreDIB
	loop	pInnerLoop124
	pop	cx
	loop	pOuterLoop124

	mov	cx, bx			; now get the remainder
	jcxz	pBypass124

	lodsb
	mov	dl, al
pInnerLoop124r:
	shl	dl, 1
	sbb	al, al
	cCall	StoreDIB
	cCall	StoreDIB
	cCall	StoreDIB
	loop	pInnerLoop124r
pBypass124:
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;8To24 is the translation routine used to convert a 8 bits per pixel Bmp to
;24 bit/pixel DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these pointers will be swapped
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine DOES not check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	Partial8To24, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	mov	bx, pdwIndexToRGB
	push	bp
	mov	cx, dx
	sub	ax, ax
pLoop824:
	lodsb
	xchg	si, bp
	mov	si, ax
	shl	si, 1
	shl	si, 1
	mov	al, ss:[bx][si]
	cCall	StoreDIB
	inc	si
	mov	al, ss:[bx][si]
	cCall	StoreDIB
	inc	si
	mov	al, ss:[bx][si]
	cCall	StoreDIB
	xchg	si, bp
	loop	pLoop824
	pop	bp
	cCall	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;8To24 is the translation routine used to convert a 8 bits per pixel Bmp to
;24 bit/pixel DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these pointers will be swapped
;	    ah:   -> entry point number-needed for segment boundary cossings
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine does not check for segment boundary crossing!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	EightTo248, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	mov	bx, pdwIndexToRGB
	push	bp
	mov	cx, dx
	sub	ax, ax
Loop824:
	lodsb
	xchg	si, bp
	mov	si, ax
	shl	si, 1
	shl	si, 1
	add	si, bx
	movs	word ptr es:[di], word ptr ss:[si]
	movs	byte ptr es:[di], byte ptr ss:[si]
	xchg	si, bp
	loop	Loop824
	pop	bp
	cCall	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;OneTo24 is used to convert a one bit per pixel internal bitmap to 24 bit/pixel
;DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these two pointers will be swapped
;	    dx:   -> oCount--number of pixels/scanline-an even multiple of 8
;	    bl:   -> bRem--number of remaining bits (pixels) in source byte
;	    ah:   -> Entry point for restart after segment boundary crossing
;	    cx:   -> Pertinent restart information
;This procedure does not check for segment boundary crossings
;If the source pixel is zero, three bytes of 0 will be written into the DIBmp,
;if the source pixel is one, three bytes of 0ffh will be written instead.
;******************************************************************************

cProc	OneTo248, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	mov	bx, dx
	and	bx, 07h
	shr	dx, 1
	shr	dx, 1
	shr	dx, 1
	mov	cx, dx

OuterLoop124:
	push	cx
	mov	cx, 8
	lodsb				;read 8 pixels from bmp
	mov	dl, al			;put them into DL
InnerLoop124:
	shl	dl, 1			;shift next pixel into carry CY if 1
	sbb	al, al
	stosb
	stosb
	stosb
	loop	InnerLoop124
	pop	cx
	loop	OuterLoop124

	mov	cx, bx			; now get the remainder
	jcxz	Bypass124

	lodsb
	mov	dl, al
InnerLoop124r:
	shl	dl, 1
	sbb	al, al
	stosb
	stosb
	stosb
	loop	InnerLoop124r
Bypass124:
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;24To8 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 8 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine does not check for segment boundary crossing!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	TwentyfourTo88, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	mov	cx, dx
TwentyfourTo8Loop:
	lodsb				; red in al
	mov	dl, al
	lodsb				; get green
	mov	ah, al
	lodsb				; get blue
	xchg	al, dl
	push	cx
	push	es
	cCall	lpDeviceColorMatch, <dx, ax, lpConversionInfo>
	pop	es
	pop	cx
	stosb				; store in our bitmap
	loop	TwentyfourTo8Loop
cEnd

;*****************************Public*Routine***********************************
;24To1 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 1 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    cx:   -> restart parameters
;	    dx:   -> number of pixels to convert--be consistent w/ other xlate
;This routine does not check for segment boundary crossing!
;On Exit:   si, di are updated, ax, bx, cx, dx have been altered
;******************************************************************************

cProc	TwentyfourTo18, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

Outer:
	mov	cx, 8
	sub	bx, bx
Inner:
	push	cx
	push	bx
	lodsb				; red in al
	mov	dl, al
	lodsb				; get green
	mov	ah, al
	lodsb				; get blue
	xchg	al, dl
	push	cx
	push	es
	cCall	lpDeviceColorMatch, <dx, ax, lpConversionInfo>
	pop	es
	pop	cx
	pop	bx
	shr	al, 1
	rcl	bl, 1
	pop	cx
	loop	Inner
	mov	al, bl
	stosb
	dec	dx
	jnz	Outer
cEnd

sEnd	DIBSeg
end
