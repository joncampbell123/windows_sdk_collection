page            ,132
title           Define the Font on the IBM 8514
.286c

.xlist
include CMACROS.INC
include 8514.INC
.list

subttl          Data Area
page +
sBegin          Data
externB WriteEnable
externB ShadowMemoryTrashed		;in SAVESCRN.ASM
externB SaveInUse			;ditto
externB SystemFontLoaded		;in STRBLT.ASM
externW FreeSpaceyOrg
externW FreeSpaceyExt
externW SystemFontyExt
externB FontInfoTable
externB SystemFontInfoTable
externB SystemFontEndPlane
externB FontFreeSpaceYOrg
externW FontFreeSpaceXOrg
externW BigFontXOrg
externB BigFontLRU
externB BigFontWritePlane
externW BigFontInfoTable
externB BigCacheDisabled

FontCurrentX	    dw	0
FontCurrentY	    db	0
FontCurrentPlane    db	0

;The FontInfoTable is a local pointer into our fonts that we cache on the
;board.  The first byte of each 5 byte entry is the width in pixels of the
;font.  The second word length entry is the X-coordinate of the start of
;this character.  The third word lengthed entry is the Y-coordinate of the
;start of this character.  We have a special one for the system font since
;it's kept resident at all times.

sEnd	Data

public	SMALLFONTLIMIT

STARTFONT	    equ     64
CURSORSPACE	    equ     128
SMALLFONTLIMIT	    equ     32

externA FontInfoTableLength

subttl          Code Area

ifdef	_PMODE
include fontseg.inc
createSeg   _PROTECT, pCode, word, public, CODE
sBegin	pCode
assumes cs, pCode
assumes ds, nothing
assumes es, Data
endif
ifdef	_RMODE
include rlfntseg.inc
createSeg   _REAL, rCode, word, public, CODE
sBegin	rCode
assumes cs, rCode
assumes ds, nothing
assumes es, Data
endif

page +
cProc		StrBltDummy, <FAR>


include         STRBLT.INC                      ;contains stack definitions


cBegin          <nogen>     

;This routine exists so that we set up a stack frame which is correct for 
;our common StrBlt stack frame.  It's never called but allows us to make
;near calls to StrBlt routines.

cEnd		<nogen>

ifdef	_PMODE		;enable 386 instructions in protect mode only
.386

cProc	PLoadFont, <NEAR, PUBLIC>
endif
ifdef	_RMODE
cProc	RLoadFont, <NEAR, PUBLIC>
endif

;Entry:
;       ES points to the data segment
;Exit:
;       Font has been defined to the 8514.

cBegin

;First, initialise the parameters that will stay constant throughout the draw
;such as font height, starting Y coordinate, mode, etc:

	CheckFIFOSpace	SIX_WORDS
	mov	ax,FontHeight		;get saved font height
	mov	cx,ax			;save it for later
	sub	ax, ax			;do one scanline at a time
	mov	dx,RECT_HEIGHT_PORT	;set it on the board
	out	dx,ax

;Now, set the mode to "user defined pattern":

	mov	ax,USER_DEFINED_PATTERN_MODE
	.errnz	MODE_PORT-RECT_HEIGHT_PORT
	out	dx,ax			;AX=0a080h

;Next, set the FUNCTION 0 & 1 registers:

	mov	ax,1			;set background to black
	mov	dx,FUNCTION_0_PORT
	out	dx,ax
	inc	ax			;set foreground to white
	mov	dx,FUNCTION_1_PORT
	out	dx,ax


;If the font height is greater than a certain amount, we run the risk of
;trashing off-screen memory totally by defining this font.  Therefore, we
;test for this condition and do a define of only the code points used in the
;string.  Although this makes for a horribly slow StrBLT, at least we won't
;crash the board!

	mov	ax,cx			;get back the font height
	cmp	ax, SMALLFONTLIMIT	;is font too big? (1/2 size of cursor)
	jbe	short LFNormalFont	;nope, continue

LFHugeFont:
	mov	di, DataOFFSET BigFontInfoTable ;this is the index table to use
	test	es:[ShadowMemoryTrashed], 02h	;is off-screen cache still okay
	jz	short LFLoadNewBigFont		;no. start from scratch
	cmp	bNewFont, 0			;do we need to load a new font?
	jz	short LFFontStillLoaded 	;no. use what's still left

LFLoadNewBigFont:
	mov	es:[ShadowMemoryTrashed], 02h	;signal: big font in off-scr

;flag that bitmaps in off-screen memory will be destroyed
;First, clear the font info table so we can determine if we've already 
;defined a code point:
;DI has the address of the FontInfoTable.
;AX has the FontHeight.

	mov	dx,di			;save address of FontInfoTable
	mov	cx,FontInfoTableLength
	mov	bx,ax			;save the font height for later
	xor	ax,ax
rep	stosw

	mov	di, DataOFFSET BigFontLRU
	mov	cx, 2			;clear LRU stack
rep	stosw

	mov	es:[BigFontXOrg], ax	;reset X origin

	inc	ax			;initialize LRU Stack
	mov	byte ptr es:[BigFontLRU], al
	mov	es:[BigFontWritePlane], al
	mov	di,dx			;get back offset of FontInfoTable

LFFontStillLoaded:

ifdef	_PMODE
	cCall	PLoadHugeFont		 ;load up the huge font (368 p mode)
endif
ifdef	_RMODE
	cCall	RLoadHugeFont		 ;load up the huge font ((2)86 r+p mode)
endif

;Now we must calculate the amount of space that this font has taken up in
;off screen memory.  This is Y_SIZE+64 (where we started defining this font)
;plus the height of the font:

	mov	ax, FontHeight
	jmp	LFManageFreeSpace	;go get new starting line of free memory

LFMegaFont:
	stc				;set carry == couldn't load font
	jmp	LFExit

LFNormalFont:
;Since we're going to use invisible memory to store our fonts, we want to 
;initialise the "free space" variables to the total off-screen
;memory size - the size of one row of fonts.  As we allocate more and more
;rows, we'll subtract off the size of them from the free space.  The
;size of all rows of the font in off-screen memory never exceeds 64 pixels,
;which is the cursor height.

;Now we have to set up a running loop so we can move to a new row of fonts
;when we run out of space on our first row.  We also initialise the starting
;X-coordinate to 64.  This is because we store our saved cursor at coordinates
;0 thru 64.  If we get to a row of fonts that starts above Y-coordinate
;(768 + 64), we'll start that row at column 0.

	cmp	es:[SystemFontLoaded], 0
	jnz	short LFSystemFontPresent
	and	byte ptr es:[ShadowMemoryTrashed], 0feh
	mov	yCoordinate,Y_SIZE	;init our starting Y-coordinate
	mov	es:[SystemFontLoaded], 0ffh
	mov	ax, 1			;load the system font into plane 0
	mov	di, DataOFFSET SystemFontInfoTable
	mov	bx, STARTFONT+CURSORSPACE   ;this is the starting X-coordinate
	mov	dx,WRITE_ENABLE_PORT	;set the plane to write to onto the
	out	dx,ax			;board
	mov	WritePlane, al

	xor	cx,cx			;make sure CH is blank
	mov	cl,LastChar		;get nbr of characters in the set
	sub	cl,FirstChar
	inc	cx
	lds	si,lpFont		;make DS:SI point at FONTINFO
	add	si, ((offset fsCharOffset)-(offset fsType))
					;now DS:SI points at our CharOffset
LFCodePointLoop:
	push	cx			;save our loop counter
ifdef	_PMODE
	cCall	PLoadCharacter
endif
ifdef	_RMODE
	cCall	RLoadCharacter
endif
	pop	cx
	loop	LFCodePointLoop 	;go draw the next code point

	mov	ax,es			;make DS --> Data
	mov	ds,ax
	assumes ds,Data
	mov	al, WritePlane		    ;now save plane and postion where
	mov	[SystemFontEndPlane], al    ;we left so that non-system fonts
if 0
;----------------------------------------------------------------------------
;NOTE:  The following code fixes a bug that will cause extended characters
;in the system font to be trashed when a dos box is open and used extensively.
;While this is not a big problem for English speaking markets, many European 
;markets are more seriously affected because they often use these extended
;characters.  
;This bug fix will not make it into the retail version of Windows 3.1.  
;Anyone building this driver from the  DDK will by default, build a 
;binary comparable version of the retail windows 3.1 8514 driver. However,
;this bug fix should be included in 3rd party drivers, and future revisions 
;of Microsoft's driver.
;----------------------------------------------------------------------------
	mov	bx,STARTFONT+CURSORSPACE
	dec	al
	jz	short @f
	mov	bx,STARTFONT
@@:
endif
	mov	[FontFreeSpaceXOrg], bx     ;will be loaded flush to the
	mov	ax, yCoordinate 	    ;system font in off-screen memory
	sub	ax, Y_SIZE
if 0
;----------------------------------------------------------------------------
;This code is part of the extended character trashing bug referenced above.
;----------------------------------------------------------------------------
	add	ax,FontHeight
endif
	mov	[FontFreeSpaceYOrg], al
	mov	[FreeSpaceyExt], 0c0h	    ;reset the off-screen memory save
	mov	[FreeSpaceyOrg], 0340h	    ;area
	mov	[ShadowMemoryTrashed], 0    ;indicate that there is nothing
	mov	[SaveInUse], 0		    ;in off-screen memory any more
	clc				    ;clear carry==successful operation

LFExitNode:
	jmp	LFExit

LFSystemFontPresent:
	mov	di, DataOFFSET FontInfoTable
	cmp	bNewFont, 0
	je	short LFCheckCache
	push	di			;save CX and DI and clear current
	push	cx			;content of the font info table, i.e
	mov	cx, FontInfoTableLength ;flush the cache
	sub	ax, ax
rep	stosw
	pop	cx			;restore font height in CX
	pop	di			;restore offset to font info table
	mov	al, es:[SystemFontEndPlane]
	mov	es:[FontCurrentPlane], al
	mov	al, es:[FontFreeSpaceYOrg]
	mov	es:[FontCurrentY], al
	mov	ax, es:[FontFreeSpaceXOrg]
	mov	es:[FontCurrentX], ax

LFCheckCache:
	mov	ax, Y_SIZE
	mov	si, es:[FontCurrentX]	;SI: the starting X-coordinate
	mov	dl, es:[FontCurrentPlane]
	mov	bl, es:[FontCurrentY]
	mov	bh, bl
	add	bh, cl			;add on height of new font (in CX)
	cmp	bh, STARTFONT		;will it still fit?
	jbe	short NewFontFits	;yes
	sub	bx, bx			;no, go to next plane
	shl	dl, 1
	mov	si, STARTFONT		;and reset the X coordinate as well
NewFontFits:
	add	al, bl
	mov	yCoordinate, ax
	mov	al, dl			;get where to load other small font
	mov	bx, si			;BX: initial X position
	mov	dx,WRITE_ENABLE_PORT	;set the plane to write to onto the
	out	dx,ax			;board
	mov	WritePlane, al

	mov	cx,Count		;get the number of characters in the
	jcxz	LFExitNode		;string as a loop counter

	mov	al, LastChar		;save last char on stack
	push	ax
	sub	al, FirstChar
	mov	LastChar, al
ifdef	_PMODE
	push	fs			
	lfs	si, lpString		;FS:SI-->pointer to string
	mov	ds, seg_lpFont
	assumes fs, nothing

public	PLFLoop
PLFLoop:
	push	cx			;save our loop counter
	push	di
	lods	byte ptr fs:[si]	;get next character in string
	sub	al,FirstChar		;subtract off 1st character in set
	cmp	al, LastChar		;is it a valid charater??
	ja	short PLFBadChar	;no! Go substitute default char.
PLFGoodChar:	
	xor	ah,ah			;make it a word in AX
	shl	ax, 1
	mov	dx,ax			;for multiply by 6
	shl	ax, 1
	add	dx,ax			;BX: 6*(unbiased char index)
	add	di, dx			;ES:DI-->current font info cell
	mov	cx, es:[di]		;get width of char out of our table
	jcxz	PLFCheckIfDefined
	jmp	short PLFEndLoop

PLFBadChar:	
	mov	al, DefaultChar 	;in case of an invalid char code, use
	jmp	short PLFGoodChar	;default char and continue.

PLFCheckIfDefined:
	push	si			;save offset to string
	mov	si, off_lpFont		;get pointer to FONTINFO
	add	si, ((offset fsCharOffset)-(offset fsType))
					;point at the CharOffset table
	add	si, dx			;DS:SI-->char-offset cell for this char
	cCall	PLoadCharacter		;load char image into off-screen memory
	pop	si			;restore pointer to string

PLFEndLoop:
	pop	di			;restore offset to base of font info
	pop	cx			;restore saved string loop counter
	loop	PLFLoop 		;go draw the next character
	pop	fs			;restore fs to keep KERNEL in the mood
endif
ifdef	_RMODE
	lds	si, lpString		;FS:SI-->pointer to string
	assumes ds, nothing

public	LFLoop
LFLoop:
	push	cx			;save our loop counter
	push	di
	lodsb				;get next character in string
	sub	al,FirstChar		;subtract off 1st character in set
	cmp	al, LastChar		;is it a valid charater??
	ja	short LFBadChar		;no! Go substitute default char.
LFGoodChar:	
	xor	ah,ah			;make it a word in AX
	shl	ax, 1
	mov	dx,ax			;for multiply by 6
	shl	ax, 1			;AX: 4*(unbiased char index)
	add	dx,ax			;DX: 6*(unbiased char index)
	add	di, dx			;ES:DI-->current font info cell
	mov	cx, es:[di]		;get width of char out of our table
	jcxz	LFCheckIfDefined
	jmp	short LFEndLoop

LFBadChar:	
	mov	al, DefaultChar 	;in case of an invalid char code, use
	jmp	short LFGoodChar	;default char and continue.

LFCheckIfDefined:
	push	ds
	push	si			;save offset to string
	lds	si, lpFont		;DS:SI-->FONTINFO
	add	si, ((offset fsCharOffset)-(offset fsType))
					;point at the CharOffset table
	add	si, ax			;DS:SI-->char-offset cell for this char
	cCall	RLoadCharacter		;load char image into off-screen memory
	pop	si			;restore pointer to string
	pop	ds

LFEndLoop:
	pop	di			;restore offset to base of font info
	pop	cx			;restore saved string loop counter
	loop	LFLoop			;go draw the next character
endif
	mov	es:[FontCurrentX], bx	;time to save updated X, Y and plane
	mov	al, WritePlane		;positions
	mov	es:[FontCurrentPlane], al
	mov	ax, yCoordinate
	sub	ax, Y_SIZE
	mov	es:[FontCurrentY], al
	pop	ax			;need to restore LastChar stack var.
	mov	LastChar, al
	clc				;clear carry==success
	jmp	short LFExit

LFManageFreeSpace:

	mov	dx, (256-STARTFONT)	;reset the amount of free space to it's
					;full amount
	sub	dx,ax			;update the amount of space available
	mov	es:[FreeSpaceyExt],dx	;in the free area
	add	ax,Y_SIZE+STARTFONT	;update the new Y-origin of invisible
					;free space
	mov	es:[FreeSpaceyOrg],ax	;this is the new value
	clc

LFExit:
	rcl	bl, 1			;save carry in LSB of bl
	CheckFIFOSpace	ONE_WORD
	mov	al, es:[WriteEnable]	;set write enable back to all planes
	mov	dx,WRITE_ENABLE_PORT
	out	dx,ax
	rcr	bl, 1			;restore carry flag
cEnd

cProc	UpdateCoordinateAndPlane, <NEAR>
cBegin
	mov	ax, FontHeight		;check whether another row of
	mov	bx, ax			;characters fits next to the cursor
	shl	ax, 1			;without sticking out over it
	add	ax, yCoordinate
	cmp	ax, (Y_SIZE+STARTFONT)
	ja	short AdvanceToNextPlane
	add	yCoordinate, bx
	test	WritePlane, 01h 	;still writing to plane 0?
	jz	short InitializeXCoordinate ;no, do normal initialization
	mov	bx, STARTFONT+CURSORSPACE   ;need special initialization for
	jmp	short GetXCoordianteReady   ;plane 0

AdvanceToNextPlane:
	mov	yCoordinate, Y_SIZE	;reset the Y coordinate
	shl	WritePlane, 1		;update write plane
	CheckFIFOSpace	THREE_WORDS
	mov	al, WritePlane		;and set new plane to the board
	mov	dx, WRITE_ENABLE_PORT
	out	dx, ax
	dec	di			;add the change of write plane in
	dec	di			;plane/width field as well
	mov	ah, al
	shl	ah, 4
	sub	al, al
	add	ax, cx
	stosw
InitializeXCoordinate:
	mov	bx, STARTFONT
GetXCoordianteReady:
	add	bx, cx
cEnd

ifdef	_PMODE
cProc	PLoadCharacter, <NEAR, PUBLIC>
endif
ifdef	_RMODE
cProc	RLoadCharacter, <NEAR, PUBLIC>
endif
cBegin
	assumes es, Data

	CheckFIFOSpace	TWO_WORDS
	lodsw				;get the width of next code point in AX
	mov	cx,ax			;save the width of code point in CX
	mov	dh, WritePlane		;high nibble of width indicates the
	shl	dh, 4			;plane in which the characters are
	or	ah, dh			;going to be written
	stosw				;store it in the FontInfo table
	jcxz	LFBypassSetWidth	;zero width-->special case
	mov	ax, cx
	dec	ax			;decrement it by 1
	mov	dx,RECT_WIDTH_PORT	;set width of code point on board
	out	dx,ax
LFBypassSetWidth:

	add	bx,cx			;add on the width of this code point to
	cmp	bx, X_SIZE		;get the X-coordinate for the next one
	jb	short LFXSizeOkay	;and update the plane if neccessary
	call	UpdateCoordinateAndPlane

LFXSizeOkay:
	mov	ax, bx
	sub	ax, cx			;AX: source X origin of next character
	mov	wDestX, ax		;save it for DrawFont routine
	stosw				;and save it in FontInfo

	push	bx			;save our new X-coordinate
ifdef	_PMODE
	lodsd				;get offset into FONTINFO segment of
					;our code point's definition
	push	si			;save offset of next CharOffset entry
	mov	esi, eax		;get offset of code point's bits into SI
endif
ifdef	_RMODE
	lodsw				;get offset into FONTINFO segment of
					;our code point's definition
	push	si			;save offset of next CharOffset entry
	mov	si, ax			;get offset of code point's bits into SI
endif
	mov	ax,yCoordinate		;for every code point, we have to reset
	mov	wDestY, ax
	stosw				;set the Y-coordinate into our table
;Now we send the command to define a user-supplied pattern to the board.  Then
;we'll proceed to extract the font's pattern from the FONTINFO data supplied.

;At this point,
;       CX has the width of our code point.
;	DS:ESI points to the start of our character's bits.

	jcxz	LFBypassDrawFont	;can't draw zero width character
ifdef	_PMODE
	call	PDrawFont		;go define the character
endif
ifdef	_RMODE
	call	RDrawFont		;go define the character
endif
LFBypassDrawFont:
	pop	si			;restore offset of next CharInfo entry
	pop	bx			;restore our running X-coordinate
cEnd

ifdef	_PMODE
cProc		PDrawFont, <NEAR, PUBLIC>, <di>
endif
ifdef	_RMODE
cProc		RDrawFont, <NEAR, PUBLIC>, <di>
endif
cBegin

;Entry:   
;	DS:(E)SI points to the start of the character definition.
;	CX contains width of the character.

	mov	bx, wDestX
	and	bx, 3			;BL: nibble phase alignment factor
	mov	ax, cx
	add	ax, 7
	and	ax, 0fff8h		;AX=(ceil(X extent/8))*8
	mov	dx, ax			;save in DX
	shr	ax, 3			;AX=ceil(X extent/8)
	mov	wByteCount, ax		;this is the # of bytes to do
	sub	dx, bx			;subtract nibble phase
	cmp	dx, cx			;if nibble phase aligned X extent GE
	sbb	bh, bh			;X extent ==>no rem. (partial nibble)
	mov	bRemainderNeeded, bh	;BH: remainder needed flag

	mov	cl, bl			;now compute the AND mask to select the
	sub	bh, bh			;left over bits from the previous byte
	stc				;of data
	rcr	bh, cl
	dec	bh
	not	bh
ifdef	_PMODE
	sub	edi, edi		;clear high word of EDI as well
endif
	mov	cx, FontHeight		;initialize outer loop counter
	mov	di, cx			;(E)DI: offset from one byte to next
	dec	di			;lodsb advances ESI by one

DFLineLoop:
	push	cx			;save # of lines to do--outer loop ctr
ifdef	_PMODE
	push	esi			;save offset to start of current scan
endif
ifdef	_RMODE
	push	si			;save offset to start of current scan
endif
	CheckFIFOSpace	FOUR_WORDS	;make enough room for next iteration
	mov	ax, wDestX		;set coordinates for this scanline
	mov	dx, Srcx_PORT
	out	dx, ax
	mov	ax, wDestY
	mov	dx, Srcy_PORT
	out	dx, ax
	inc	wDestY			;advance to next scanline
	mov	dx, COMMAND_FLAG_PORT
	mov	ax, 43b3h		;write using pel accrual feature
	out	dx, ax
	mov	dx, PATTERN_DEFINE_PORT

	mov	cx, wByteCount		;CH should always be zero
	jcxz	DFBottomOfLoop		;make sure we don't crash
	xchg	cx, bx			;CL: LoopShiftFactor, CH: AND mask
	sub	bh, bh			;make sure no trash is in BH

DFPixelLoop:				;BL: line loop counter
ifdef	_PMODE
	lods	byte ptr [esi]		;fetch 8 new pixels from DS:ESI
	add	esi, edi		;advanve to next source byte
endif
ifdef	_RMODE
	lodsb				;fetch 8 new pixels from DS:SI
	add	si, di			;advanve to next source byte
endif
	mov	ah, al			;copy into AH
	shr	ax, cl			;nibble align to destination
	or	ah, bh			;transfer left-over pels
	mov	bh, al			;copy left-over pixels to be
	and	bh, ch			;mask off pixels to be displayed now
	mov	al, ah			;AX: pixels to be displayed now
	shr	ah, 3			;align upper nibble to what 8514 wants
	shl	al, 1			;align lower nibble
	out	dx, ax			;send all pixels to the board
	dec	bl			;update # of bytes left to do
	jnz	DFPixelLoop

	cmp	bRemainderNeeded, 0
	je	short DFBottomOfLoop

	mov	ah, bh			;need to put out remaining pixels
	shr	ah, 3			;AH: remaining pixels (1..3)
	out	dx, ax			;AL: don't care

DFBottomOfLoop:
	mov	bx, cx			;BX: shift factor and mask.
ifdef	_PMODE
	pop	esi			;restore ptr to start of last scan
	inc	esi			;advance to start of next scan
endif
ifdef	_RMODE
	pop	si			;restore ptr to start of last scan
	inc	si			;advance to start of next scan
endif
	pop	cx			;restore outer loop counter
	loop	DFLineLoop
cEnd

ifdef	_PMODE
cProc	PLoadHugeFont, <NEAR, PUBLIC>, <ds>
endif
ifdef	_RMODE
cProc	RLoadHugeFont, <NEAR, PUBLIC>, <ds>
endif
cBegin

;A huge font is loaded one StrBlt at a time.  Since we know that a StrBLT 
;can at maximum be 1024 pixels long, we need only assure ourselves that we
;have 1024 pixels available in X.  Since the cursor takes up 64 pixels by
;64 pixels, by starting our defintion at Y-coordinate 64 in off-screen 
;memory, we can assure ourselves plenty of room.

;DI contains the offset of our local FontInfoTable.

	mov	cx,Count		;get the number of characters in the
	jcxz	LHFExitNode		;string as a loop counter

	mov	al, LastChar
	push	ax
	sub	al, FirstChar
	mov	LastChar, al

	mov	ax, es:[BigFontXOrg]
	mov	LocDstxOrg, ax		;init the starting X
	mov	al, es:[BigFontWritePlane]  ;get the proper write plane
	mov	WritePlane, al
	mov	dx, WRITE_ENABLE_PORT
	out	dx, ax

	lds	si,lpString		;get pointer to string

LHFLoop:
	push	cx			;save our loop counter
	lodsb				;get next code point to define
	sub	al,FirstChar		;subtract off 1st character in set
	cmp	al,LastChar		;is it a valid charater??
	ja	short LHFBadChar	;no! Go substitute default char.
LHFGoodChar:	
	xor	ah,ah			;make it a word in AX
	shl	ax, 1
	mov	bx,ax			;for multiply by 6
	shl	ax, 1			;AX: 4*unbiased char index
	add	bx,ax			;BX: 6*unbiased char index
	mov	cx, es:[bx+di]		;get width of char out of our table
	jcxz	LHFCheckIfDefined	;char not yet loaded into cache
	shr	ch, 4
	cmp	es:[BigFontLRU], ch	;is this char in most recently used
	je	short LHFEndLoop	;plane?  Yes, we're done
	mov	dl, ch			;no.  Need to update LRU stack
	call	UpdateLRUStack		;modifies DL only
	jmp	short LHFEndLoop

LHFBadChar:	
	mov	al, DefaultChar 	;in case of an invalid char code, use
	jmp	short LHFGoodChar	;default char and continue.


LHFExitNode:
	jmp	short LHFExit

LHFCheckIfDefined:
	push	ds			;save pointer to string
	push	si
	lds	si,lpFont		;get pointer to FONTINFO
	add	si, ((offset fsCharOffset)-(offset fsType))
ifdef	_PMODE				;point at the CharOffset table
	add	si,bx			;now DS:SI points to information for
endif					;this code point
ifdef	_RMODE
	add	si, ax			;DS:SI-->char offset cell for this char
endif
	lodsw				;get the width of our character
	mov	cx,ax			;save the width of our character
	jcxz	LHFEndLoopAndPop	;in some fonts the width may be zero

	call	ManageCache		;checks write plane and width,
					;maintains LRU Stack
	mov	es:[bx+di], ax		;and put it into our table
	CheckFIFOSpace	THREE_WORDS	;this will trash DX and AX
	mov	ax, cx
	dec	ax
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax

	mov	ax,LocDstxOrg		;set the X-coordinate on the board
	mov	es:[bx+di+2],ax 	;save the X-coordinate in our table
	mov	wDestX, ax
	add	ax, cx
	mov	LocDstxOrg, ax		;add on the width of this character
	mov	ax,Y_SIZE+64		;set the Y-coordinate to draw at
	mov	wDestY, ax
	mov	es:[bx+di+4],ax 	;save the Y-coordinate in our table

;Now we send the command to define a user-supplied pattern to the board.  Then
;we'll proceed to extract the font's pattern from the FONTINFO data supplied.

;At this point,
;       CX has the width of our character
ifdef	_PMODE
	mov	esi,dword ptr [si]	;get offset of character def into SI
	call	PDrawFont		;go define the character
endif
ifdef	_RMODE
	mov	si, [si]		;get offset of character def into SI
	call	RDrawFont		;go define the character
endif
LHFEndLoopAndPop:
	pop	si			;restore pointer to string
	pop	ds

LHFEndLoop:
	pop	cx			;restore saved string loop counter
	loop	LHFLoop 		;go draw the next character

	mov	ax, LocDstxOrg
	mov	es:[BigFontXOrg], ax
	mov	al, WritePlane
	mov	es:[BigFontWritePlane], al
	pop	ax			;need to restore LastChar stack var.
	mov	LastChar, al
LHFExit:
cEnd

cProc	ManageCache, <NEAR>		;ax=cx=Width of character
cBegin	nogen
	mov	dx, LocDstxOrg
	add	dx, cx
	cmp	dx, X_SIZE
	jae	short MCFindNewPlane
	mov	dl, WritePlane
	cmp	byte ptr es:[BigFontLRU], dl
	jne	short MCUpdateLRUStackNode
MCNormalExit:
	shl	dl, 4
	or	ah, dl
	ret
MCFindNewPlane:
	push	ax
	push	di
	sub	di, di
	mov	LocDstxOrg, di
	mov	al, 01h 		;has plane 0 not been used yet?
	cmp	byte ptr es:[di+BigFontLRU], 0
	je	short MCFreePlaneFound	;no, DL is set to enable plane 0
	shl	al, 1			;check if plane 1 has been used yet
	inc	di
	cmp	byte ptr es:[di+BigFontLRU], 0
	je	short MCFreePlaneFound	;no, DL is set to enable plane 1 now
	shl	al, 1
	inc	di
	cmp	byte ptr es:[di+BigFontLRU], 0
	je	short MCFreePlaneFound
	shl	al, 1
	inc	di
	cmp	byte ptr es:[di+BigFontLRU], 0
	je	short MCFreePlaneFound
	mov	al, es:[BigFontLRU+3]	;make the least recently used plane
	mov	dx, WRITE_ENABLE_PORT	;the current plane
	out	dx, ax
	mov	dl, al
	xchg	al, es:[BigFontLRU]	;put new plane on top of the stack
	xchg	al, es:[BigFontLRU+1]	;and put prev TOS into 2nd place
	xchg	al, es:[BigFontLRU+2]	;ditto for 3rd place
	xchg	al, es:[BigFontLRU+3]	;and 4th place
	mov	WritePlane, dl
	shl	dl, 4
	sub	ax, ax
	mov	di, DataOFFSET BigFontInfoTable
	push	cx
	mov	cx, 256 		;BigFontTable has 256 entries
MCPurgePlaneLoop:
	mov	dh, es:[di+1]
	and	dh, 0f0h
	cmp	dh, dl
	je	short MCFreeCharacter
	add	di, 6
	loop	MCPurgePlaneLoop
	jmp	short MCPurgePlaneDone
MCUpdateLRUStackNode:
	jmp	short MCUpdateLRUStack
MCFreeCharacter:
	stosw
	add	di, 4
	loop	MCPurgePlaneLoop
MCPurgePlaneDone:
	pop	cx
	pop	di
	pop	ax
	or	ah, dl
	ret
MCFreePlaneFound:
	mov	es:[di+BigFontLRU], al	;set the plane in LRU stack
	mov	dl, al
	call	UpdateLRUStack
	mov	dx, WRITE_ENABLE_PORT
	out	dx, ax
	mov	dl, al
	mov	WritePlane, al
	pop	di
	pop	ax
	jmp	MCNormalExit
MCUpdateLRUStack:
	call	UpdateLRUStack
	jmp	MCNormalExit
cEnd	nogen

UpdateLRUStack	proc near
	cmp	es:[BigFontLRU+1], dl
	je	short USFixStack1
	cmp	es:[BigFontLRU+2], dl
	je	short USFixStack2
	cmp	es:[BigFontLRU+3], dl
	jne	short USErrorExit
	xchg	es:[BigFontLRU], dl
	xchg	es:[BigFontLRU+1], dl
	xchg	es:[BigFontLRU+2], dl
	xchg	es:[BigFontLRU+3], dl
	ret
USFixStack1:
	xchg	es:[BigFontLRU], dl
	xchg	es:[BigFontLRU+1], dl
	ret
USFixStack2:
	xchg	es:[BigFontLRU], dl
	xchg	es:[BigFontLRU+1], dl
	xchg	es:[BigFontLRU+2], dl
	ret
USErrorExit:
	stc
	ret
UpdateLRUStack	endp

ifdef	_PMODE
sEnd	pCode
endif
ifdef	_RMODE
sEnd	rCode
endif
end
