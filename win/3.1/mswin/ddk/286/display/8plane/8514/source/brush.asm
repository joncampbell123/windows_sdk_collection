page    ,132
title   Select Brush Routines
.286c

.xlist
include CMACROS.INC
include 8514.INC
incLogical = 1			    ;include logical definitions
include gdidefs.inc
include brush.inc
include macros.mac
.list

;DEBUG = 1

sBegin	Data
externW     FreeSpaceyOrg
externB     ForgroundMix
externB     BackgroundMix
sEnd	Data

subttl          Code Area
page +
sBegin          Code
assumes         cs,Code
assumes 	ds,Data

DispatchTable	label word
	dw	HBSolid 		;handle solid brush
	dw	HBSolid 		;handle hollow rush (ROP only)
	dw	HBPatterned		;handle monochrome patterned brush
	dw	HBExit			;invalid !
	dw	HBPatterned		;handle color patterned brush
	dw	HBDithered		;handle dithered brush

cProc           HandleBrush,<NEAR,PUBLIC>,<bx,cx>

;Entry:
;       AL contains desired MIX MODE

        parmD   lpPBrush
        parmD   lpDrawMode
        parmW   DstxOrg                 ;for polygons, this is "Count"
        parmW   DstyOrg                 ;for polygons, this is SEG_lpPoints
        parmW   xExt                    ;for polygons, this is OFF_lpPoints
	parmW	yExt
	parmW	PatXOrg
	parmW	PatYOrg

        localB  Function
	localB	BackgroundColour
	localW	XCnt
	localW	YCnt
	localW	FreeSpaceYStart
	localW	WorkSpaceXStart
	localV	RotatedPattern, 8

cBegin
	mov	cl,al			;save the passed function

public  HBWaitForQueue
HBWaitForQueue:         

;       First, check to see what type of brush we have.  The supported styles
;are:
;               0 -- A solid brush                                    
;               1 -- A hollow brush
;               2 -- A patterned brush (monochrome)
;               3 -- A hatched brush 
;		4 -- A patterned brush (colour)
;		5 -- A dithered brush
;
;Note: A hollow brush will only be passed to us if we are to perform a 
;NOT of the destination. Thus, the colour of the brush doesn't matter, since
;the NOT function doesn't pay any attention to it.

ifdef	DEBUG				;for debugging
.386					;use i386 32 bit registers
	mov	ax, DstxOrg
	mov	bx, DstyOrg
	shl	eax, 16 		;high word EAX: destination X origin
	shl	ebx, 16 		;high word EBX: destination Y origin
.286
endif

	les	di,lpPBrush		;get the brush data structure in ES:DI
	sub	bh, bh
	mov	bl, es:[di].bnStyle
	shl	bl, 1
	jmp	cs:[bx].DispatchTable

public	HBDithered
HBDithered:
	mov	Function, cl
	mov	ax, FreeSpaceyOrg
	mov	bx, 03f8h
	cmp	ax, bx
	jb	FreeSpaceOkay		; check whether SaveInUse is in use
	mov	ax, bx			; for now, take a gamble

;	 add	 FreeSpaceyOrg, 8
FreeSpaceOkay:
	mov	FreeSpaceYStart, ax
	mov	WorkSpaceXStart, bx
	push	ds			; keep ds=Data
	lds	si, lpPBrush		; get a local copy of the pattern bits
	assumes ds, nothing
	add	si, bnColorBits 	;chances are we have to rotate in X
					;   and Y now.
	mov	di, PatYOrg		;Do ratation in Y first
	mov	bx, PatXOrg
	sub	sp, 64			;create temporary storage space on the
	mov	ax, sp			; stack.
	push	bp			;Save BP
	mov	bp, ax			;SS:BP-->temp storage
	push	si			;keep initial pointer to bits
	mov	cx, 7
	mov	dx, cx			;DX: modulo 8 mask (07h)
	and	di, cx			;BX: pattern Y origin modulo 8
	and	bx, cx			;DI: pattern Y origin modulo 8
	inc	cx			;CX: need to copy 8 rows total
	.errnz	PDIMENSION-8		;assume an 8 by 8 pattern
	sub	cx, di			;do this many rows first
	push	di			;this will be the outer loop count the
	shl	di, 3			;  next time around
	add	si, di			;DS:SI-->first row to use
	sub	ah, ah			;this will an indicator when to stop
CopyPatternLoop_10:
	push	cx			;save outer loop count
	mov	cx, 8
CopyPatternLoop_20:
	mov	al, [bx][si]
	mov	[bp], al
	inc	bp
	inc	bx
	and	bx, dx
	loop	CopyPatternLoop_20
	pop	cx
	add	si, 8
	loop	CopyPatternLoop_10
	or	ax, ax			;do we need to keep going?
	js	CopyPatternDone
	dec	ah			;AH=0ffh, so we can terminate the loop
	pop	cx			;CX: outer loop count
	pop	si			;DS:SI-->beginning of pattern in PBrush
	jcxz	CopyPatternDone
	jmp	short CopyPatternLoop_10

CopyPatternDone:
	lea	si, [bp][-64]		;now SS:SI-->beginning of rotated pat.
	pop	bp			;restore base pointer

	MakeEmptyFIFO			;first write the dither pattern
	mov	ax, FreeSpaceYStart	; (Bitmap) into a 8 by 8 off-screen
	mov	dx, Srcy_PORT		; rectangle
	out	dx, ax
	mov	ax, WorkSpaceXStart
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	ax, 047h
	mov	dx, FUNCTION_0_PORT
	out	dx, ax
	mov	dx, FUNCTION_1_PORT
	out	dx, ax
	mov	ax, (PDIMENSION-1)
	mov	dx, RECT_WIDTH_PORT
	out	dx, ax
	mov	dx, RECT_HEIGHT_PORT
	out	dx, ax
	mov	ax, 0a080h
	mov	dx, MODE_PORT
	out	dx, ax
	mov	ax, 53b1h
	mov	dx, COMMAND_FLAG_PORT
	out	dx, ax
	mov	dx, PATTERN_DEFINE_PORT
	mov	cx, 32
	cld				;don't take ANY chances
rep	outs	dx, word ptr ss:[si]	;copy the rotated pattern to the board
	add	sp, 64			;free temporary storage space
	pop	ds
	assumes ds, Data

;At this point the 8 by 8 dither pattern bitmap is drawn in off-screen memory.
;Now this pattern needs to be copied (repeatedly) into the client rectangle.
;How many times that pattern needs to be copied into the client area is
;computed next.  The coordinates for the clipping rectangle need to be
;calculated as well.......

	CheckFIFOSpace	SEVEN_WORDS
	mov	dx, CLIP_PORT
	mov	ax, xExt
	mov	bx, ax			;BX: X extent
	add	ax, DstxOrg
	dec	ax
	or	ah, 40h
	out	dx, ax
	mov	ax, yExt
	mov	cx, ax			;CX: Y extent
	add	ax, DstyOrg
	dec	ax
	or	ah, 30h
	out	dx, ax

	mov	dx, (PDIMENSION-1)	;now compute the X and Y repeat counts
	.errnz	PDIMENSION-8		; as ceil(X (Y) extent/8)
	add	bx, dx
	add	cx, dx
	shr	bx, 3
	shr	cx, 3
	mov	XCnt, bx
	mov	YCnt, cx

;Now its time to copy the pattern into the client rectangle.  In the innermost
;loop we copy with increasing X, and in the outer loop we copy with increasing
;Y.

	mov	al, Function
	cmp	al, 0ffh
	je	AlternateSetFunction
	or	al, 60h
	mov	dx, FUNCTION_0_PORT
	out	dx, ax
	mov	dx, FUNCTION_1_PORT
	out	dx, ax

SetDimensions:
	mov	ax, (PDIMENSION-1)
	mov	dx, RECT_WIDTH_PORT
	out	dx, ax
	mov	dx, RECT_HEIGHT_PORT
	out	dx, ax
	mov	ax, 0a000h
	mov	dx, MODE_PORT
	out	dx, ax
	mov	bx, FreeSpaceYStart
	mov	cx, YCnt		; outer loop count in cx
	mov	di, DstyOrg		; keep current dst y in di
OuterCopyLoop:
	mov	si, DstxOrg		; keep current dst x in si
	push	cx			; save current outer loop count
	mov	cx, XCnt		; set inner loop count
InnerCopyLoop:
	CheckFIFOSpace	FIVE_WORDS
	mov	ax, bx
	mov	dx, Srcy_PORT
	out	dx, ax
	mov	ax, WorkSpaceXStart
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	ax, si
	mov	dx, Dstx_PORT
	out	dx, ax
	mov	ax, di
	mov	dx, Dsty_PORT
	out	dx, ax
	add	si, PDIMENSION
	mov	ax, 0c0b3h
	mov	dx, COMMAND_FLAG_PORT
	out	dx, ax
	loop	InnerCopyLoop
	pop	cx
	add	di, PDIMENSION
	loop	OuterCopyLoop

;We're done copying the dither pattern bitmap to the client rectangle.  Now
;we need to reset the screen clip region back to full screen size and then exit

;	sub	FreeSpaceyOrg, 8
	CheckFIFOSpace	TWO_WORDS
	mov	dx, CLIP_PORT
	mov	ax, 43ffh
	out	dx, ax			;reset right hand scissor
	mov	ah, 33h
	out	dx, ax			;reset bottom scissor
	jmp	HBExit

AlternateSetFunction:
	mov	al, ForgroundMix
	or	al, 60h
	mov	dx, FUNCTION_1_PORT
	out	dx, ax
	mov	al, BackgroundMix
	or	al, 60h
	mov	dx, FUNCTION_0_PORT
	out	dx, ax
	jmp	short SetDimensions

public  HBSolid
HBSolid:

;We have a solid brush.  Now set the colour:
;ES:DI-->PBrush

	MakeEmptyFIFO
	or	cl,cl			;is this a NOT operation?
	jz	HBSBSetFunc		;yes, skip setting the colour
	mov	al,es:[di].bnColor	;set the brush colour
	mov	dx,COLOUR_1_PORT
	out	dx,ax
	mov	dx,COLOUR_0_PORT
	out	dx,ax

public  HBSBSetFunc
HBSBSetFunc:

;Next, set the mix:

	cmp	cl,0ffh 		;are we to leave the function alone?
	je	HBSBSetMode		;yes, just go set the mode
	mov	al,cl			;set the "function"
	mov	dx,FUNCTION_1_PORT
	out	dx,ax
	mov	dx,FUNCTION_0_PORT
	out	dx,ax

public  HBSBSetMode
HBSBSetMode:

;Now, set the mode to "solid colour"

	mov	dx,MODE_PORT
	mov	ax,0a000h		;this is "normal mode"
	out	dx,ax

public  HBSBDrawRect
HBSBDrawRect:
	mov	ax,DstxOrg		;get left coordinate
	mov	dx,Srcx_PORT

	out	dx,ax
	mov	ax,DstyOrg		;get top coordinate
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	ax,xExt 		;get the width
	dec	ax			;decrement it (as per manual)
	mov	dx,RECT_WIDTH_PORT
	out	dx,ax
	CheckFIFOSpace	TWO_WORDS
	mov	ax,yExt 		;get the height
	dec	ax
	mov	dx,RECT_HEIGHT_PORT
	out	dx,ax
	mov	ax,40f3h		;this is command to draw rectangle
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax
	jmp	HBExit			;and we're done with our solid brush!

page
public	HBPatterned
HBPatterned:

;Entry:                                                  
;       ES:DI points at our PBrush.
;	CL has our function.
;       Check to see if our brush is in colour. If it is, go establish 
;the brush's true colour by comparing it with the DrawMode's background colour.

	CheckFIFOSpace	SEVEN_WORDS
	mov	dx,MODE_PORT
	mov	ax,0a040h		;set to "patterned fill" mode
	out	dx,ax

	cmp	es:[di].bnStyle, 4	;is it a colored patterned brush?
	jne	HBPBSetMono
	mov	ah,es:[di].bnFgColor	;get our foreground colour
	mov	al,es:[di].bnBgColor	;get the background colour that we

	jmp	short SetColors

public	HBPBSetMono
HBPBSetMono:
	mov	dx,ds			;save our Data segment
	lds	si,lpDrawMode		;get the DrawMode into DS:SI
	mov	ah,[si+4]		;get background colour (1s->bc color)
	mov	al,[si+8]		;get foreground colour (0s->fg color)
	mov	ds,dx			;restore saved Data segment
SetColors:

;Now AL contains the correct background colour and AH has the correct foreground
;colour.  Set the appropriate registers.  The bits that are 1 in the pattern
;will assume the foreground color and the bits that are 0 assume the background
;color.

	mov	BackgroundColour,al	;save the true background colour for
					;return value
	mov	dx,COLOUR_0_PORT	;send out the background colour
	out	dx,ax
	mov	dx,COLOUR_1_PORT	;send out the foreground colour
	mov	al,ah
	out	dx,ax
	cmp	cl, 0ffh
	je	HBPBSetupLoop		;yes, continue on

public  HBPBSetFunction
HBPBSetFunction:

;It turns out that we always want to draw opaquely if the function hasn't been
;preset.

	mov	al, cl			;Rop in AL
	mov	dx,FUNCTION_0_PORT	;set functions to "user-defined" pattern
	out	dx,ax
	or	al,20h
	mov	dx,FUNCTION_1_PORT
	out	dx,ax			;set the foreground function

public  HBPBSetupLoop
HBPBSetupLoop:

	mov	cx, PatXOrg		;we need to rotate the pattern bits
	sub	cx, DstxOrg		; in X By PatXOrg-DstxOrg
	and	cl, 07h
	lea	si, es:[di].bnPattern
;	 add	 di, bnPattern		 ;now ES:DI-->pattern bits
;	 mov	 si, di 		 ;now ES:SI-->pattern bits
	lea	di, RotatedPattern	;SS:DI-->where to store the rotated pat
	mov	bx, 4			;copy all eight bytes of pattern
HBPBRotatePatternXLoop:
	lods	word ptr es:[si]
	rol	al, cl
	rol	ah, cl
	mov	ss:[di], ax
	inc	di
	inc	di
	dec	bx
	jnz	HBPBRotatePatternXLoop
	sub	di, 8			;SS:DI-->X-rotated pattern

;Now set up some initial coordinates and extents etc.

	mov	dx,RECT_WIDTH_PORT	;set the X-extent
	mov	ax,xExt
	dec	ax			;according to manual
	out	dx,ax
	mov	dx,RECT_HEIGHT_PORT
	xor	ax,ax			;Y-extent is always 1 (decremented)
	out	dx,ax
	mov	si,DstyOrg		;save this since we need it in loop
	mov	cx,yExt 		;make CX our loop counter
	mov	bx, PatYOrg		;get the DstyOrg for
					;the pattern start calculation
	and	bx,07h			;and calculate the pattern start index
	push	bp
	mov	bp, DstxOrg

public  HBPBLoop
HBPBLoop:
	mov	bh,ss:[bx+di]		;get the pattern into BH
	CheckFIFOSpace	FIVE_WORDS	;trashes AX and DX
	mov	dx,PATTERN_0_PORT
	mov	ah,80h			;this is flag for pattern 0
	mov	al,bh			;get the high 4 bits of pattern
	shr	al,3			;get them in the right place
	out	dx,ax
	mov	ah,90h			;this is flag for pattern 1
	mov	al,bh			;get the low 4 bits of pattern
	shl	al,1			;get them into the right place
	out	dx,ax
	mov	dx,Srcx_PORT		;reset the starting X
	mov	ax, bp
	out	dx,ax
	mov	dx,Srcy_PORT		;set the Y-coordiante
	mov	ax,si			;get our next Y-coordinate
	out	dx,ax
	inc	si			;bump to next Y-coordinate
	mov	dx,COMMAND_FLAG_PORT	;send the command
	mov	ax,40f3h		;this is the correct one
	out	dx,ax
	inc	bx			;bump our pattern index
	and	bx, 07h 		;make that modulo 8
	loop	HBPBLoop		;loop for all lines
	pop	bp

HBExit:
	sub	ax, ax
	mov	al,BackgroundColour	;return the background colour in AL
cEnd


subttl          Convert a Device Format PBrush to a Microsoft Format
page +
cProc	ConvertBrush,<NEAR,PUBLIC>

cBegin

;This process converts a physical brush which is in device format 
;(see ROBJECT.ASM for details on its structure) to a Microsoft format, 
;compatible with BITBLT and OUTPUT.

;Entry:
;	AH: Write enable bitmask
;       DS:SI points to the device format PBrush.
;	ES:DI points to the Microsoft format PBrush to be filled.
;	BL: background color's mono bit in D0
;	BH: text color's mono bit in D0
;
;	Patterns were realized assuming white background and black text colors
;
;Exit:
;       AL & CX are destroyed.

	shr	bl, 1			;expand mono bit into BL
	sbb	bl, bl
	shr	bh, 1			;expand mono bit into BH
	sbb	bh, bh
	mov	cx,4			;we want to move 8 bytes of pattern
	mov	al, [si].bnStyle	;get the brush style into AL
	cmp	al, BS_HOLLOW		;is it a hollow brush?
	.errnz	BS_HOLLOW-1
	je	CBHollow		;yes.  Do hollow brush stuff
	cmp	al, 2			;is it a mono patterned brush?
	je	CBMonoPatterned
	cmp	al, 4			;is it a colored patterned brush?
	clc				;clear carry flag
	jne	CBNonPatterned		;no.  Skip these steps then.
	mov	bx, wptr [si].bnBgMono	;it is a colored patterned brush.  Has
	xchg	bl, bh
CBMonoPatterned:			; the same pre-proc. as mono pt. brush
	sub	dx, dx			;assume fg color == bk color, BX=XOR
	cmp	bl, bh			;mask.	Are they really equal?
	je	CBTrueSolidBrush	;yes! this will make it a solid brush

;At this point we're dealing with a true patterned brush or with a solid
;brush.  We know that the background color is different from the text color.
;In the "default case" BL is white (0ffh) and BH is black (0).	Create an XOR
;mask in BX that will be applied to the pattern when it is copied.

CBNonPatterned: 			;enter here with NC-->no-op mask
	sbb	bx, bx			;not default: BX=0ffffh, else BX=0
	mov	dl, [si].bnColor	;DL: actual brush color
	or	al,al			;is it solid? (AL: brush style)
	jz	CBSolid 		;yes, go do solid processing

public	CBPatterned
CBPatterned:
	add	si, bnPattern
	mov	word ptr es:[di+8],3	;set patterned style
	mov	byte ptr es:[di+10],10h ;indicate a monochrome brush
CBPatternLoop:
	lodsw				;fetch 2 bytes of the pattern
	xor	ax, bx			;apply XOR mask
	stosw				;store the pattern
	loop	CBPatternLoop		;do all 8 bytes of pattern
	jmp	short CBExit		;get out

CBHollow:
	cbw				;make style a word in AX
	mov	word ptr es:[di+8],ax	;set hollow style
	jmp	short CBExit		;and we're done

public	CBSolid
CBSolid:
	and	dl, ah			;strip off accelerator bits
	jz	CBTrueSolidBrush	;solid black?
	xor	dl, ah			;solid white?
	jnz	CBPatterned		;no.  Treat as a patterned brush
	xor	dl, ah			;restore color in DL
CBTrueSolidBrush:
	mov	wptr es:[di][8], 0
	.errnz	BS_SOLID
	mov	byte ptr es:[di+10],0b0h;set brush "accelerator" byte to
					;indicate monochrome, solid,"1s,0s only"
	neg	dl			;color either black or white
	sbb	ax, ax			;expand into AX
	xor	ax, bx			;apply XOR mask before storing
rep	stosw				;and put it into the destination
	.errnz	bnPattern-bnColor-1
	.errnz	bnBgColor-bnPattern-8

CBExit:
	clc				;NC to signal BitBlt to keep going
cEnd

sEnd	Code
end
