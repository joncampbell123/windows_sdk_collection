.286
.xlist
include cmacros.inc
include dibdefs.inc
include gdidefs.inc
.list

public	OneFourEightToFour
public	BuildColorInfo_4
public	FourToOneFourEight

externFP    GetIndexOfRGBFar		;in colour.asm
externNP    StoreDIB			;in dibstuff.asm
externNP    ReadDIB			;in dibstuff.asm

sBegin	Data
externB Palette
externB ColorFlagTable
sEnd	Data

createSeg   _DIB, DIBSeg, word, public, CODE
sBegin	    DIBSeg

	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

externNP    DsEsSwap

;*********************************Public*Routine*******************************
;This routine will write a set of palette colors into the DIBs info block and
;generate a translate table to map the colors read to the colors that are
;defined in the DIBs palette.
;On entry:
;   ES:DI --> Destination device header (bmp)
;   DS:SI --> Color table in DIB's info block
;******************************************************************************

cProc	BuildColorInfo4, <NEAR, PUBLIC>, <ds, es, si, di, bx>
	parmD	lpbColorTable
cBegin
	call	DsEsSwap		;now DS:SI-->bmp, ES:DI-->color table
	cmp	cx, 1			;is it a 24 b/p DIB?
	jne	BuildColorInfo2 	;no.

	les	di, lpbColorTable	;ES:DI-->internal color table
	mov	ax, DataBASE
	mov	ds, ax
	assumes ds, Data
	mov	si, DataOFFSET Palette	;DS:SI-->8514's palette (system colors)
	mov	cx, 16
CopyPaletteLoop:
	movsw				;copy R and G
	movsb				;copy B
	stosb				;DWORD allign
	loop	CopyPaletteLoop 	;copy entire palette
	jmp	short BuildColorInfoEnd

BuildColorInfo2:
	cmp	cx, 2
	jne	BuildColorInfo16
	call	BuildColorTable2
	jmp	short BuildColorInfoEnd
BuildColorInfo16:
	cmp	cx, 16
	jne	BuildColorInfo256
	call	BuildColorTable16
	jmp	short BuildColorInfoEnd
BuildColorInfo256:
	cmp	cx, 256
	jne	BuildColorInfoEnd
	mov	cx, 16
	call	BuildColorTable16	;copy 16 system colors into palette
	mov	cx, 480 		;fill the remaining 240 slots in the
	sub	ax, ax			;palette with black
rep	stosw
BuildColorInfoEnd:
cEnd

BuildColorTable2    proc near
	sub	ax, ax			;first we write the color table into
	stosw				;the DIB info header
	stosw				;write black and flags
	dec	ax
	stosw				;write white
	stosb
	inc	ax
	stosb				;and a byte of flags
	les	di, lpbColorTable	;now ES:DI --> bColorTable
	cmp	[si].bmBitsPixel, 1	;is it a mono bitmap?
	je	DoMonoColorTable
	mov	ax, DataBASE		;now it's time to copy the mono
	mov	ds, ax			;bits stored in the ColorFlagTable
	assumes ds, Data		;into the local color Xlate table
	mov	si, DataOFFSET ColorFlagTable
	mov	bx, 0f0fh		;bit mask
	mov	cx, 8			;16 system colors--do 2 at once
BuildColorTable2Loop:
	lodsw				;get mono bit into bit 4 in AL and AH
	shl	ax, 3			;put mono bit into bit 7
	shl	al, 1			;expand mono bit into AL
	sbb	al, al
	shl	ah, 1			;expand mono bit into AH
	sbb	ah, ah
	and	ax, bx			;apply bit mask--white==0fh
	stosw				;store in color Xlate table
	loop	BuildColorTable2Loop
	ret
DoMonoColorTable:
	stosb				;AL was still 0
	inc	ax			;makes AL=1
	stosb				;and store it.	Maps 0->0 and 1->1
	ret
BuildColorTable2    endp

BuildColorTable16   proc near
	mov	dh, [si].bmBitsPixel	;keep # of bits/pixel of src bmp in DH
	mov	bx, off_lpbColorTable	;now ss:bx --> bColorTable
	sub	dl, dl			;DL is out index
	mov	si, DataOFFSET Palette
	mov	ax, DataBASE		;DS:SI-->system palette
	mov	ds, ax
	assumes ds, Data
BuildColorInfo16Loop:			;this loop produces the correct result
	lodsb				;ONLY for a 4 plane 8514.
	mov	ah, al			;AH: red
	lodsb				;AL: green
	movsb				;copy blue
	stosb				;store green
	mov	al, ah
	stosb				;store red
	sub	al, al
	stosb				;store flags (just a byte of 0)
	mov	ss:[bx], dl
	inc	bx
	inc	dx
	loop	BuildColorInfo16Loop
	cmp	dh, 1			;mono source bitmap?
	jne	BC16Done		;no, we're done
	mov	byte ptr ss:[bx][-15], 0fh	;yes, map 1s to white (0fh)
BC16Done:
	ret
BuildColorTable16   endp

cProc	DIBDummy, <NEAR>		;get frame definitions into this file
	include dibframe.inc
cBegin	<nogen>
cEnd	<nogen>

BuildColorInfo_4    proc near
	lea	bx, bColorTable
	push	ss
	push	bx
	cCall	BuildColorInfo4
	ret
BuildColorInfo_4    endp

;*****************************Public*Routine***********************************
;FourToOneFourEight is the translation routine that will convert a 4 bit
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

FourToOneFourEight  proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	lea	bx, bColorTable
	or	dx, dx
	jz	Remainder4148
	mov	ah, 0f0h		;ah=initial nibble mask
oLoop4148:
	push	dx			;save outer loop count
	push	cx			;save inner loop and shift count
	mov	dh, al			;restore what was in DH at bottom of
	sub	dl, dl			; outer loop.
iLoop4148:
	shl	dl, cl			;allign for next pixel
	mov	al, dh			;in case of lower nibble (saves 1 jump)
	or	ah, ah			;upper or lower nibble?
	jns	NibbleOkay		;lower nibble, go translate pixel
	lodsb				;upper nibble, fetch new byte
	mov	dh, al			;store lower nibble in dh
	shr	al, 4			;allign lower nibble
	and	dh, 0fh 		;get upper nibble ready for translation
NibbleOkay:
	not	ah			;toggle nibble mask
	xlatb	ss:[bx] 		;fetch logical color
	or	dl, al			;and add it to other pixels
	dec	ch			;update inner loop counter
	jnz	iLoop4148		;do inner loop
	mov	al, dl			;its time to save pixel in DIB
	stosb				;save at current ES:DI and update ptr
	mov	al, dh			;lower nibble in AL-might need it later
	pop	cx			;restore inner loop count
	pop	dx			;restore outer loop count
	dec	dx			;decrement loop counter
	jnz	oLoop4148		;loop if necessary
Remainder4148:
	pop	dx			;now do remainder (if any)
	or	dl, dl			;any remainder ?
	jz	Bypass4148		;no, just get out

	push	cx			;save inner loop count
	mov	ch, dl			;set inner loop count to remainder
	mov	al, dl			; compute the shift count necessary to
	mul	cl			; allign the last, incomplete byte such
	mov	dl, 8			; that there are no gaps in the bitmap
	sub	dl, al
	push	dx			;put that number on the stack
	mov	ah, 0f0h		;initial nibble mask
	sub	dl, dl
iLoop4148r:
	shl	dl, cl			;allign for next pixel
	mov	al, dh			;in case of lower nibble (saves 1 jump)
	or	ah, ah			;upper or lower nibble?
	jns	NibbleOkayr		;lower nibble, go translate pixel
	lodsb				;upper nibble, fetch new byte
	mov	dh, al			;store lower nibble in dh
	shr	al, 4			;allign lower nibble
	and	dh, 0fh 		;get upper nibble ready for translation
NibbleOkayr:
	not	ah			;toggle nibble mask
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	iLoop4148r
	mov	al, dl
	pop	cx			;get that shift count
	shl	al, cl			;align last byte
	stosb				;store it
	pop	cx			;restore cx
Bypass4148:
	call	DsEsSwap		;swap pointers back (for dibframe)
	ret
FourToOneFourEight  endp

;*****************************Public*Routine***********************************
;OneToFour is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixles to 4 bit per pixel 8514 color format.
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

OneFourEightToFour  proc near
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx			; keep remaining # of pixels on stack
	lea	bx, bColorTable
	mov	al, 0f0h		; initial nibble mask
	or	dx, dx
	jz	Remainder18
oLoop18:
	push	dx			; keep outer loop count on stack
	xchg	al, dh
	push	cx			; save inner loop count and shift count
	lodsb				; get first pixels
	mov	dl, al			; and keep them in a save place
iLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	or	dh, dh
	jns	SetLowerNibble18
	shl	al, 4
	stosb
	dec	di
	jmp	short iLoopBottom18
SetLowerNibble18:
	and	al, dh
	or	es:[di], al
	inc	di
iLoopBottom18:
	not	dh
	dec	ch
	jnz	iLoop18
	pop	cx
	xchg	al, dh
	pop	dx			; get oLoop counter
	dec	dx
	jnz	oLoop18
Remainder18:
	pop	dx			; get the # of remaining bits to do
	or	dl, dl			; dx=0 means no remaining bits to do
	jz	Bypass18		; so let's exit now

	mov	dh, al			;DH: current nibble mask
	mov	ch, dl			; now do the remaining bits in the
	lodsb				; the current line
	mov	dl, al
rLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	or	dh, dh
	jns	SetLowerNibble1r
	shl	al, 4
	stosb
	dec	di
	jmp	short iLoopBottom1r
SetLowerNibble1r:
	and	al, dh
	or	es:[di], al
	inc	di
iLoopBottom1r:
	not	dh
	dec	ch
	jnz	rLoop18
Bypass18:
	ret
OneFourEightToFour  endp


;*****************************Public*Routine***********************************
;FourToOneFourEight is the translation routine that will convert a 4 bit
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

cProc	Partial4To148, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bx
	lea	bx, bColorTable
	or	dx, dx
	jz	pRemainder4148
	mov	ah, 0f0h		;ah=initial nibble mask
poLoop4148:
	push	dx			;save outer loop count
	push	cx			;save inner loop and shift count
	mov	dh, al
	sub	dl, dl			;make dl=0
piLoop4148:
	shl	dl, cl			;allign for next pixel
	mov	al, dh			;in case of lower nibble (saves 1 jump)
	or	ah, ah			;upper or lower nibble?
	jns	pNibbleOkay		;lower nibble, go translate pixel
	lodsb				;upper nibble, fetch new byte
	mov	dh, al			;store lower nibble in dh
	shr	al, 4			;allign lower nibble
	and	dh, 0fh 		;get upper nibble ready for translation
pNibbleOkay:
	not	ah			;toggle nibble mask
	xlatb	ss:[bx] 		;fetch logical color
	or	dl, al			;and add it to other pixels
	dec	ch			;update inner loop counter
	jnz	piLoop4148		;do inner loop
	mov	al, dl			;its time to save pixel in DIB
	cCall	StoreDIB		;save at current ES:DI and update ptr
	mov	al, dh
	pop	cx			;restore inner loop count
	pop	dx			;restore outer loop count
	dec	dx			;decrement loop counter
	jnz	poLoop4148		;loop if necessary
pRemainder4148:
	pop	dx			;now do remainder (if any)
	or	dl, dl			;any remainder ?
	jz	pBypass4148		;no, just get out
	push	cx			;save inner loop count
	mov	ch, dl
	mov	al, dl			; compute the shift count necessary to
	mul	cl			; allign the last, incomplete byte such
	mov	dl, 8			; that there are no gaps in the bitmap
	sub	dl, al
	push	dx			;put that number on the stack
	mov	ah, 0f0h		;initial nibble mask
	sub	dl, dl
piLoop4148r:
	shl	dl, cl			;allign for next pixel
	mov	al, dh			;in case of lower nibble (saves 1 jump)
	or	ah, ah			;upper or lower nibble?
	jns	pNibbleOkayr		;lower nibble, go translate pixel
	lodsb				;upper nibble, fetch new byte
	mov	dh, al			;store lower nibble in dh
	shr	al, 4			;allign lower nibble
	and	dh, 0fh 		;get upper nibble ready for translation
pNibbleOkayr:
	not	ah			;toggle nibble mask
	xlatb	ss:[bx]
	or	dl, al
	dec	ch
	jnz	piLoop4148r
	mov	al, dl
	pop	cx			;get that shift count
	shl	al, cl
	cCall	StoreDIB
	pop	cx			;restore cx
pBypass4148:
	call	DsEsSwap		;swap pointers back (for dibframe)
cEnd

;*****************************Public*Routine***********************************
;4To24 is the translation routine used to convert a 4 bits per pixel Bmp to
;24 bit/pixel DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these pointers will be swapped
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine DOES check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	Partial4To24, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	push	bp
	lea	bp, bColorTable
	mov	cx, dx
	sub	bx, bx
pLoop424:
	lodsb				;fetch two pixels from source bmp
	mov	ah, al
	ror	ah, 4
	and	ax, 0f0fh		;high nibble in AH, low nibble in AL
	mov	dx, ax
	shl	ah, 2
	mov	bl, ah			;BX: index into palette
	xchg	si, bx
	mov	ax, [bp][si]
	cCall	StoreDIB
	mov	al, ah
	cCall	StoreDIB
	mov	al, [bp][si][2]
	cCall	StoreDIB
	xchg	si, bx
	dec	cx
	jcxz	pFourTo24Done
	shl	dl, 2
	mov	bl, dl
	xchg	si, bx
	mov	ax, [bp][si]
	cCall	StoreDIB
	mov	al, ah
	cCall	StoreDIB
	mov	al, [bp][si][2]
	cCall	StoreDIB
	xchg	si, bx
	loop	pLoop424
pFourTo24Done:
	pop	bp
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;OneToFour is the translation routine that will convert 1, 4, or 8 bit DIBmp
;pixles to 4 bit per pixel 8514 color format.
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

cProc	Partial148To4, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	push	bx			; keep remaining # of pixels on stack
	lea	bx, bColorTable
	mov	al, 0f0h		; initial nibble mask
	or	dx, dx
	jz	pRemainder18
poLoop18:
	push	dx			; keep outer loop count on stack
	xchg	al, dh
	push	cx			; save inner loop count and shift count
	cCall	ReadDIB 		; get first pixels
	mov	dl, al			; and keep them in a save place
piLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	or	dh, dh
	jns	pSetLowerNibble18
	shl	al, 4
	stosb
	dec	di
	jmp	short piLoopBottom18
pSetLowerNibble18:
	and	al, dh
	or	es:[di], al
	inc	di
piLoopBottom18:
	not	dh
	dec	ch
	jnz	piLoop18
	pop	cx
	xchg	al, dh
	pop	dx			; get oLoop counter
	dec	dx
	jnz	poLoop18
pRemainder18:
	pop	dx			; get the # of remaining bits to do
	or	dl, dl			; dx=0 means no remaining bits to do
	jz	pBypass18		; so let's exit now

	mov	dh, al			;DH: current nibble mask
	mov	ch, dl			; now do the remaining bits in the
	cCall	ReadDIB 		; the current line
	mov	dl, al
prLoop18:
	rol	dl, cl
	mov	al, dl
	and	al, ah			; one pixel at a time
	xlatb	ss:[bx] 		; find the color for it
	or	dh, dh
	jns	pSetLowerNibble1r
	shl	al, 4
	stosb
	dec	di
	jmp	short piLoopBottom1r
pSetLowerNibble1r:
	and	al, dh
	or	es:[di], al
	inc	di
piLoopBottom1r:
	not	dh
	dec	ch
	jnz	prLoop18
pBypass18:
cEnd

;*****************************Public*Routine***********************************
;24To4 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 4 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine DOES check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	Partial24To4, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	mov	cx, dx
	mov	dx, 0800h		;for rgb->index: 4 bits/pixel, no acc.
	mov	bh, 0f0h		; initial nibble mask in dl

pTwentyfourTo4Loop:			; at this point bh is available again
	push	cx
	mov	cx, dx
	cCall	ReadDIB 		; red in al
	mov	dl, al
	cCall	ReadDIB
	mov	ah, al
	cCall	ReadDIB
	xchg	dl, al			;al=red, ah=green, dl=blue, cl=0, ch=8
	save	<bx>
	call	GetIndexOfRGBFar	;dh=index of prev. RGB
	or	bh, bh
	jns	pStorePixel
	mov	bl, dh
	shl	bl, 4
	not	bh
	mov	dx, cx
	pop	cx
	loop	pTwentyFourTo4Loop
	jmp	short pTwentyFourToFourDone
pStorePixel:
	or	bl, dh
	not	bh
	mov	al, bl
	stosb
	mov	dx, cx
	pop	cx
	loop	pTwentyFourTo4Loop
pTwentyFourToFourDone:
cEnd

;*****************************Public*Routine***********************************
;4To24 is the translation routine used to convert a 4 bits per pixel Bmp to
;24 bit/pixel DIBmp format.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp	    these pointers will be swapped
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine does not check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	FourToTwentyFour, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	call	DsEsSwap
	mov	cx, dx
	lea	dx, bColorTable
Loop424:
	sub	bx, bx
	lodsb				;fetch two pixels from source bmp
	mov	ah, al
	ror	ah, 4
	and	ax, 0f0fh		;high nibble in AH, low nibble in AL
	shl	ah, 2
	mov	bl, ah			;BX: index into palette
	xchg	si, bx
	add	si, dx
	movs	word ptr es:[di], word ptr ss:[si]
	movs	byte ptr es:[di], byte ptr ss:[si]
	mov	si, bx
	dec	cx
	jcxz	FourTo24Done
	sub	bx, bx
	shl	al, 2
	mov	bl, al
	xchg	si, bx
	add	si, dx
	movs	word ptr es:[di], word ptr ss:[si]
	movs	byte ptr es:[di], byte ptr ss:[si]
	mov	si, bx
	loop	Loop424
FourTo24Done:
	call	DsEsSwap
cEnd

;*****************************Public*Routine***********************************
;24To4 is the translation routine used to convert a 24 bits per pixel DIBmp to
;the 4 bit/pixel format used for the 8514.
;On entry:  es:di -> PDevice bitmap
;	    ds:si -> DIBmp
;	    dx:   -> number of source pixel to convert-consistent w/ other xlate
;This routine DOES check for segment boundary crossings!
;On Exit:   si, di are updated, ax, cx have been altered
;******************************************************************************

cProc	TwentyFourToFour, <NEAR, PUBLIC>
cBegin
	assumes cs, DIBSeg
	assumes ds, nothing
	assumes es, nothing

	mov	cx, dx
	mov	dx, 0800h		;for rgb->index: 4 bits/pixel, no acc.
	mov	bh, 0f0h		; initial nibble mask in dl

TwentyfourTo4Loop:			; at this point bh is available again
	push	cx
	mov	cx, dx
	lodsw
	mov	dl, al
	lodsb
	xchg	dl, al			;al=red, ah=green, dl=blue, cl=0, ch=8
	save	<bx>
	cCall	GetIndexOfRGBFar	;dh=index of prev. RGB
	or	bh, bh
	jns	StorePixel
	mov	bl, dh
	shl	bl, 4
	not	bh
	mov	dx, cx
	pop	cx
	loop	TwentyFourTo4Loop
	jmp	short pTwentyFourToFourDone
StorePixel:
	and	dh, bh			;strip off acc. bits
	or	bl, dh
	not	bh
	mov	al, bl
	stosb
	mov	dx, cx
	pop	cx
	loop	TwentyFourTo4Loop
TwentyFourToFourDone:
cEnd

sEnd	DIBSeg
end
