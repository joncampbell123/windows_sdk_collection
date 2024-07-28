.286
.xlist
include cmacros.inc
include gdidefs.inc
include 8514.inc
ifdef _PMODE
include fontseg.inc
endif
ifdef _RMODE
include rlfntseg.inc
endif
.list

externFP    TranslateDrawMode		;in TBINIT.ASM
externFP    CursorExclude		;in ROUTINES.ASM
externFP    CursorUnExclude		;in ROUTINES.ASM
externFP    SetScreenClipFar		;in ROUTINES.ASM

sBegin	Data
externW FontInfoTable			;in GETFONT.ASM
externW SystemFontInfoTable		;in GETFONT.ASM
externW FullScreenClip			;in DATA.ASM
externB OldFaceName			;defined in TEXTOUT.ASM
externB BigOldFaceName			;this one and all below as well
externW OldFaceNameLength
externW BigOldFaceNameLength
externW OldFontHdr
externW BigOldFontHdr
externB SystemFontLoaded
externW SystemFontPointSize
externB ShadowMemoryTrashed
externW BigFontInfoTable
externB BigCacheDisabled
sEnd	Data

externA     SMALLFONTLIMIT

ifdef	_PMODE
createSeg   _PROTECT, pCode, word, public, CODE
sBegin	    pCode

assumes cs, pCode
assumes ds, Data
assumes es, nothing

externNP    PStrMem			;in PSTRMEM.ASM
externNP    PLoadFont			;in PGETFONT.ASM
externNP    PDrawFont			;in PGETFONT.ASM
externNP    PColourMemoryStrBlt 	;in STRCOLOR.ASM

cProc	ExtTextOutP, <FAR,PUBLIC,WIN,PASCAL>, <si, di>
	include STRBLT.INC		;frame definitions
cBegin	nogen
.386
	push	esi			;need to save 32 bit pointers as they
	push	edi			;may get changed in LoadFont
.286
endif
ifdef	_RMODE
createSeg   _REAL, rCode, word, public, CODE
sBegin	    rCode

assumes cs, rCode
assumes ds, Data
assumes es, nothing

externNP    RStrMem			;in PSTRMEM.ASM
externNP    RLoadFont			;in PGETFONT.ASM
externNP    RDrawFont			;in PGETFONT.ASM
externNP    RColourMemoryStrBlt 	;in STRCOLOR.ASM

cProc	ExtTextOutR, <FAR,PUBLIC,WIN,PASCAL>, <si, di>
	include STRBLT.INC		;frame definitions
cBegin	nogen

endif

;First see what action we should take:

	cld				;set direction forward
	mov	cx,Count		;get character count into CX
	jcxz	SBTestForNoAction	;if Count is zero, see if it's a NOP
	or	cx,cx			;is character count greater than 0?
	jns	SBTestForMemoryDraw	;if count is negative, return length

SBCallStrMem:

;If we've reached this point, we are either drawing to a main memory monochrome
;bitmap or asking for the extents of the string (but not drawing it).  Since
;the EGA Monochrome StrBLT already accomplishes these tasks very nicely, all
;we need to do is call this code which we've incorporated into this driver.

	arg	lpDstDev
	arg	DstxOrg
	arg	DstyOrg
	arg	lpClipRect
	arg	lpString
	arg	Count
	arg	lpFont
	arg	lpDrawMode
	arg	lpTextForm
	arg	lpCharWidths
	arg	lpOpaqueRect
	arg	Options
ifdef	_PMODE
	cCall	PStrMem
endif
ifdef	_RMODE
	cCall	RStrMem
endif
	jmp	short SBExit		;returned data is in DX:AX

SBTestForNoAction:
	test	Options,02h		;do we have to draw the opaquing rect?
	jz	SBGoodExit		;nope, there's nothing to do!

SBTestForMemoryDraw:
	les	di,lpDstDev		;get the destination PDEVICE
	mov	al,es:[di+1]		;get the device type
	or	al,al			;are we BLTing to main memory?
	jnz	SBBoardIsDest		;nope, go BLT to the board
	cmp	byte ptr es:[di+9],1	;is it a mono memory StrBlt?
	je	SBCallStrMem		;yes, go call the monochrome code
ifdef	_PMODE
	cCall	PColourMemoryStrBlt	;oops, we have to do it in colour
endif
ifdef	_RMODE
	cCall	RColourMemoryStrBlt
endif
	jmp	short SBGoodExit	;get out now

SBBoardIsDest:

	cCall	<far ptr TranslateDrawMode>, <lpDrawMode>
	mov	seg_lpDrawMode, dx
	mov	off_lpDrawMode, ax
ifdef	_PMODE
	cCall	PBoardStrBlt		;go do the StrBlt to the Board
endif
ifdef	_RMODE
	cCall	RBoardStrBlt
endif

SBGoodExit:
	sub	ax, ax
	cwd				;DX:AX=0 means we were successful

SBExit:
ifdef	_PMODE
.386
	pop	edi
	pop	esi
.286
endif
cEnd

ifdef	_PMODE
cProc	PBoardStrBlt, <NEAR, PUBLIC>
endif
ifdef	_RMODE
cProc	RBoardStrBlt, <NEAR, PUBLIC>
endif
cBegin
;First, go intersect and validate our two clipping rectangles:
	mov	bMegaFont,0
	mov	al, SystemFontLoaded	;put the flag that indicates the
	mov	SystemFontPresent, al	;presence of the system font into local
					;stack frame
	cCall	IntersectClip		;go do the intersection
	jnc	BSBValidateCoordinates	;if error-free, go check the coordinates

BSBLeave:
	jmp	BSBExit 		;there's nothing to draw, get out now

BSBValidateCoordinates:
	test	Options, 02
	jz	BSBUseDestXY
	cmp	MinimumClipRect.left, X_SIZE
	jge	BSBLeave
	cmp	MinimumClipRect.top, X_SIZE ;include off-screen area
	jge	BSBLeave
	jmp	short BSBGetFont
BSBUseDestXY:
	cmp	DstxOrg,X_SIZE		;is the string totally off screen?
	jge	BSBLeave		;yes, forget it
	cmp	DstyOrg,1024		;is the string totally off screen?
	jge	BSBLeave		;yes, forget it
	cmp	DstyOrg,-1024
	jl	BSBLeave

BSBGetFont:

;Now, get some parameters from the FontInfo data structure that we'll need:

	push	ds			;save our DS (= Data)
	mov	ax,ds
	mov	es,ax			;make ES --> Data
	assumes ds,nothing
	assumes es,Data
	lds	si,lpFont		;get pointer to FONTINFO in DS:SI
	mov	di,[si+2]		;get the point size
	mov	PointSize,di
	mov	al,[si+1]		;get the system font flag
	cmp	al,'S'			;is this the system font?
	jne	BSBSaveFontParams	;nope, continue
	cmp	di,es:SystemFontPointSize
					;is this REALLY the system font?
	je	BSBSaveFontParams	;yes! continue
	xor	al,al			;otherwise, this is a stretched font

BSBSaveFontParams:
	cmp	SystemFontPresent, 0	;label the first font we encounter as
	jnz	SystemFontIsPresent	;the system font
	mov	byte ptr [si+1], 'S'	;label this font as the system font
	mov	es:SystemFontPointSize, di  ;set the system font point size
	mov	al, 'S' 		;and set the system font flag
SystemFontIsPresent:
	mov	SystemFontFlag,al	;save the flag locally
	mov	dl, al			;save for a little later
	mov	al, [si][31]		;get the default character
	mov	DefaultChar, al
	mov	ax,[si+22]		;get the font height
	mov	FontHeight,ax
	mov	bx, ax			;save it in BX for now
	mov	al,[si+32]		;get the BreakChar
	mov	BreakChar,al
	mov	al,[si+29]		;get the first character in the set
	mov	FirstChar,al
	mov	al,[si+30]		;get the last character in the set
	mov	LastChar,al

	cmp	dl, 'S' 		;is it the system font?
	je	BSBCursorExclude	;yes.  No need to fetch any cache stuff
	cmp	bx, SMALLFONTLIMIT	;is is a "small" font
	jbe	BSBSmallFont		;yes.  Fetch small font cache data

BSBBigFont:
	mov	ax,192			;bx = fontheight, ax = big font limit.
	sub	ax,bx			;Carry set if ax > 192
	adc	bMegaFont,0		;bMegaFont = 1 if ax > 192.
;--------------------------------------------------------------------------
;Under certain conditions, the Big Font Cache in off screen memory
;must be disabled.  For example, when drawing text into a color bitmap,
;the color bitmap is copied into the memory that normally would contain
;the big font cache, and the text is then rendered into it.  
;If the Big Font Cache were not disabled in this case, the font might 
;be cached on top of the color bitmap which is a definite
;no no.  Therefore, we must disable the font cache, and copy the string
;directly from the font segment, instead of the cache (like we do
;for really large fonts).
;--------------------------------------------------------------------------
	mov	al,es:[BigCacheDisabled];1 = disabled, 0 = enabled.
	or	bMegaFont,al		;If bMegaFont is != 0 then don't 
	jnz	BSBCursorExclude	; mess with the glyph cache.

	mov	ax, DataOFFSET BigFontInfoTable
	mov	bx, DataOFFSET BigOldFontHdr
	mov	cx, DataOFFSET BigOldFaceName
	mov	dx, DataOFFSET BigOldFaceNameLength
	jmp	short BSBSaveFixups

BSBSmallFont:
	mov	ax, DataOFFSET FontInfoTable
	mov	bx, DataOFFSET OldFontHdr
	mov	cx, DataOFFSET OldFaceName
	mov	dx, DataOFFSET OldFaceNameLength

BSBSaveFixups:
	mov	pwFontInfoTable, ax	;save font cache data locally now.
	mov	pwOldFontHdr, bx
	mov	pwOldFaceName, cx
	mov	pwOldFaceNameLength, dx
	mov	bNewFont, 0		;assume cache resident font

	mov	si,[si+39]		;DS:SI points at facename text
	sub	ah, ah			;we need to find the length of the
	sub	cx, cx			;facename string in order to avoid
BSBGetNameLength:			;protect mode violations when it is
	lodsb				;compared to the previous face name
	inc	cx			;or copied into the drivers Data segm.
	cmp	al, ah
	jne	BSBGetNameLength
	mov	wNewFaceNameLength, cx

BSBCursorExclude:

;It's now time to exclude the cursor.  Since we may have to draw a opaquing
;rectangle which is disjoint from the clipping rectangle, we'd best use our
;MinimumClipRect that we set up earlier as the cursor exclusion area under
;the conditions of the Options flags.

	lds	si,lpClipRect		;assume we want to use lpClipRect
	test	Options,04h		;do we want to use the intersected
					;clipping rectangle?
	jz	BSBCE1			;nope, go exclude the cursor
	lea	si,MinimumClipRect	;otherwise, get a long pointer to
	mov	ax,ss			;MinimumClipRect
	mov	ds,ax

BSBCE1:                
	push	ds			;save pointer to clipping rectangle
	push	si
	lodsw				;get starting X
	push	ax
	lodsw				;get starting Y
	push	ax
	lodsw				;get ending X
	push	ax
	lodsw				;get ending Y
	push	ax
	cCall	CursorExclude		;go exclude the cursor

BSBGetDrawMode:
	lds	si,lpDrawMode		;get pointer to the DrawMode
	mov	ax,[si+22]		;get the CharExtra
	mov	CharExtra,ax
	mov	ax,[si+12]		;get the proportional spacing flag
	mov	PropSpaceFlag,ax
	or	ax,ax			;any proportional spacing?
	jz	BSBGDMGetColours	;nope, skip these gets
	mov	ax,[si+14]		;get the BreakExtra
	mov	BreakExtra,ax
	mov	ax,[si+16]		;get the BreakErr
	mov	BreakErr,ax
	mov	ax,[si+18]		;get the BreakRem
	mov	BreakRem,ax
	mov	ax,[si+20]		;get the BreakCount
	mov	BreakCount,ax

BSBGDMGetColours:
	mov	al,[si+4]		;get the background colour
	mov	BackgroundColour,al
	mov	al,[si+8]		;get the foreground colour
	mov	ForegroundColour,al
	mov	al,[si+2]		;get the opaque flag
	mov	OpaqueFlag,al

BSBDrawOpaqueRect:
	mov	ax, CharExtra		;first test for proportional spacing
	or	ax, PropSpaceFlag
	or	ax, seg_lpCharWidths	;or user defined spacing
	jnz	BSBNonStandardStrBlt	;if so, use existing code for opaque
	cmp	Count, 0		; rect handling.  Same in case of Count
	je	BSBNonStandardStrBlt	; equal 0
	test	Options, 02h		;is there an opaquing rect to draw?
	jz	BSBCheckForNewFont	;nope, continue on
	mov	OpaqueFlag, 02h 	;force opaque mode--draw opaque rect as
	jmp	short BSBCheckForNewFont   ; the string is being put out

BSBNonStandardStrBlt:
	test	Options,02h		;do we want to draw the opaqueing rect?
	jz	BSBCheckForNewFont	;nope, continue on
	cCall	DrawOpaqueingRect
	cmp	Count,0 		;is there no string to BLT?
	jne	BSBCheckForNewFont	;there's a string, continue
	add	sp,6			;correct the stack for pushed parameters
	jmp	BSBUnExcludeCursor	;there's no string, just leave now

BSBCheckForNewFont:
	cmp	bMegaFont,0		;Is this a BigFont AND/OR the cache is disabled?
	jnz	BSBSetClippingRectangle ;Yup!  Don't bother with the cache.
	cmp	SystemFontFlag, 'S'	;is it the system font?
	jne	BSBCheckCache		;no.  Look if this font is in the cache
	cmp	byte ptr es:SystemFontLoaded, 0
					;is the system font already loaded?
	jne	BSBSetClippingRectangle ;yes, don't reload it!
	jmp	short BSBLoadNewFont	;no, go call LoadFont

BSBCheckCache:
	lds	si,lpFont		;get pointer to FontInfo into DS:SI
	assumes	ds,FontSeg
	mov	cx,wNewFaceNameLength	;CX: facename length
	mov	bx,pwOldFaceNameLength
	cmp	cx,es:[bx]
	jne	BSBDifferentFont	;Different font.
	mov	si,word ptr fsFace	;Get address of facename string.
	mov	di,pwOldFaceName	;ES:DI-->previous facename
repe	cmpsb				;compare the facenames
	jnz	BSBDifferentFont
	lea	si,fsPoints
	mov	cx,16			;32 bytes.
	mov	di,pwOldFontHdr		;ES:DI-->previous font hdr.
repe	cmpsw				;compare the font headers.
	jz	BSBLoadNewFont		;same font. get new characters if needed

BSBDifferentFont:
	dec	bNewFont		;signal: need to load a new font

BSBUpdateFacenameAndHdr:
	mov	di, pwOldFaceName	;ES:DI-->previous facename
	mov	si, word ptr fsFace	;DS:SI-->New facename
	mov	cx,wNewFaceNameLength	;CX: facename length
	mov	es:[bx],cx		;Update New facename length.
rep	movsb				;save the new facename

	lea	si,fsPoints
	mov	cx,16			;32 bytes.
	mov	di,pwOldFontHdr		;ES:DI-->previous font hdr.
rep	movsw				;save the new Header

BSBLoadNewFont:
ifdef	_PMODE
	cCall	PLoadFont		;go load a new font
endif
ifdef	_RMODE
	cCall	RLoadFont
endif

BSBSetClippingRectangle:
	pop	si			;restore saved pointer to clip rectangle
	pop	ds
	cmp	si,DataOFFSET FullScreenClip
					;do we really need to set the clip?
	je	BSBSetReadPlane 	;nope, skip the set of the clip
	cCall	SetScreenClipFar	;go set the clip

BSBSetReadPlane:
	mov	si, DataOFFSET SystemFontInfoTable
	cmp	SystemFontFlag, 'S'
	je	BSBSetColour
	mov	si, pwFontInfoTable

BSBSetColour:
	CheckFIFOSpace	SEVEN_WORDS
	mov	al,BackgroundColour
	mov	dx,COLOUR_0_PORT	;set the background colour
	out	dx,ax
	mov	al,ForegroundColour	;set the foreground colour
	mov	dx,COLOUR_1_PORT
	out	dx,ax
	pop	ds			;make DS = Data
	assumes ds,Data

BSBSetFunction:

;Now decide whether our draw is opaque or transparent and set the appropriate
;"function":

	mov	al,07h			;assume we're in opaque mode
	cmp	OpaqueFlag,02h		;are we opaque?
	je	BSBSF1			;yep, continue
	mov	al,03h			;otherwise, set background to "leave
					;alone"
BSBSF1:
	mov	dx,FUNCTION_0_PORT
	out	dx,ax
	mov	al,27h			;foreground is ALWAYS replace mode
	mov	dx,FUNCTION_1_PORT
	out	dx,ax

BSBSetupForLoop:

;Now, set the mode to "block move pattern":

	mov	ax,BLOCK_MOVE_MODE
	mov	dx,MODE_PORT
	out	dx,ax

;Set the height of the character:

	mov	ax,FontHeight		;get the font height into AX
	dec	ax			;decrement it according to documentation
	.errnz	RECT_HEIGHT_PORT-MODE_PORT
	out	dx,ax			;set it on the board

;Next, get and save the starting X coordinate of our destination:

	mov	ax,DstxOrg
	mov	LocDstxOrg,ax		;save it

;Now it's time to do the actual drawing of the string.  We look up the width
;and location of the character in our local character location table set up
;by LoadFont:

	mov	cx,Count		;get number of characters to write
	assumes es,nothing
	les	di,lpString		;make ES:DI our string pointer
	cmp	CharExtra,0		;is there proportional spacing?
	jne	BSBProportional 	;yes, better do it the hard way
	cmp	PropSpaceFlag,0 	;is there proportional spacing?
	jne	BSBProportional 	;yes, go do it the hard way

BSBCheckForCharWidth:
	cmp	seg_lpCharWidths,0	;any CharWidth table?
	je	BSBStandardBLT		;nope, just do it normally
ifdef	_PMODE				;do the CharWidth table look-ups
	mov	dx,pCodeOFFSET PCharWidthStrBlt
endif
ifdef	_RMODE
	mov	dx,rCodeOFFSET CharWidthStrBlt
endif
	jmp	short BSBPCallRoutine

BSBProportional:
ifdef	_PMODE				;assume there's no CharWidth table
	mov	dx,pCodeOFFSET ProportionalStrBlt
endif
ifdef	_RMODE
	mov	dx,rCodeOFFSET ProportionalStrBlt
endif

BSBPCallRoutine:
	cCall	dx			;call the proper routine
	jmp	BSBResetClip		;and get out

BSBBadCharacter:
	mov	al, DefaultChar 	;in case of an invalid char code, use
	jmp	short BSBGoodCharacter	;default char and continue.

BSBStandardBLT:
	test	bMegaFont,1		;If this is a MegaFont, then
	jz	short @f		;load ds:si --> Font.
	lds	si,lpFont
	add	si, ((offset fsCharOffset)-(offset fsType))
@@:
	mov	bl,FirstChar		;get these into registers for faster
	mov	bh,LastChar		;access in loop
	sub	bh, bl			;un-bias last char by first char
	sub	ch, ch			;CH: current read plane cache

BSBLoop:
	test	bMegaFont,1		;Is the font too big to fit in the cache?
	jz	short @f
	push	si
	call	DrawMegaGlyph
	add	LocDstxOrg,ax		;update the DstxOrg
	jmp	BSBEndLoop
@@:

;Now set the X & Y coordinates on the screen:

	CheckFIFOSpace	SEVEN_WORDS
	mov	ax,LocDstxOrg		;get the X-origin of our destination
	cmp	ax, X_SIZE		;is it off the screen?
	jge	BSBOpaqueRectCheck	;yes, we're done now.
	xor	dx,dx			;dx = 0 means can't rely on 8514 to clip.
	cmp	ax,-X_SIZE
	jl	short @f		;Too negative to be handled by the 8514 clip h/w.
	and	ax,7ffh 		;in case of neg. starting X make sure
					;upper 5 bits of dest X are 0
	mov	dx,Dstx_PORT		;such output is clipped of by the
	out	dx,ax			;8514's scissors
	mov	ax,DstyOrg		;get the Y-origin of our destination
	mov	dx,Dsty_PORT
	out	dx,ax
@@:

;Now, get the character that we want to BLT and determine its place in
;invisible memory:  (DS:SI --> a valid font info table, ES:DI --> lpString)

	push	si			;save offset of FontInfoTable
	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next code point to draw
	sub	al, bl			;un-bias by first char
	cmp	al,bh			;is it a nonesense code point?
	ja	BSBBadCharacter 	;yes! Use default character
BSBGoodCharacter:
	shl	ax, 1
	add	si,ax			;point at entry for our code point in
	shl	ax, 1			;the 6 byte per code point table
	add	si,ax			;now SI points to entry for this
					;code point
;Set the width of the character:

	lodsw				;get the width of the character in AX
	mov	cl, ah			;save read plane in CL
	and	ax, 07ffh
	jz	BSBEndLoop
	add	LocDstxOrg,ax		;update the DstxOrg
	dec	ax			;decrement it by 1
	or	dx,dx			;if dx is still zero, we are clipped.
	jz	BSBEndLoop
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
	test	cl, ch			;are we in the same plane?
	jnz	BSBLoadXY		;no.  No need to set it again
	mov	al, cl			;put plane information into AL
	and	al, 0f0h		;mask off any width info
	mov	ch, al			;save current read plane in CH
	shr	al, 3			;adjust read plane to what 8514 expects
	mov	dx, READ_ENABLE_PORT	;and set it to the board
	out	dx, ax

;Now set up the source X & Y as the spot in invisible memory where we're BLTing
;from:

BSBLoadXY:
	lodsw				;get source X-coordinate of character
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	lodsw				;get source Y-coordinate of character
	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;Now we send the block move command to put our character where we want it
;on the displayable screen.

	mov	ax,0c0b3h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

BSBEndLoop:
	pop	si			;restore offset of FONTINFO start
	inc	di			;bump to the next character in string
	dec	Count
	jz	short BSBOpaqueRectCheck
	jmp	BSBLoop

BSBOpaqueRectCheck:
	test	Options, 02h
	jz	BSBResetClip
	cCall	OpaqueRectRoutine

BSBResetClip:

;Lastly, we must reset the clip rectangle to full screen for our other routines:

	CheckFIFOSpace	FOUR_WORDS
	mov	dx, CLIP_PORT
	sub	ax, ax
	mov	ah, 10h
	out	dx, ax			;reset clip top
	mov	ah, 20h
	out	dx, ax			;reset clip left
	mov	ax, (X_SIZE-1)+4000h
	out	dx, ax			;reset clip right
	mov	ax, (X_SIZE-1)+3000h
	out	dx, ax			;reset clip bottom incl. off-screen

BSBUnExcludeCursor:
	cCall	CursorUnExclude 	;go free the cursor
BSBExit:
cEnd


cProc	OpaqueRectRoutine, <NEAR>
cBegin
	MakeEmptyFIFO			;assume we need to draw 1+ rect
	mov	dx, MODE_PORT
	mov	ax,0a000h		;this is "normal mode"
	out	dx,ax
	mov	dx,FUNCTION_1_PORT
	mov	al,07h			;set to opaque replace using background
	out	dx,ax			;colour
	lea	si, MinimumClipRect	;SS:SI-->actual opaquing rectangle
	mov	bx, ss:[si].bottom	;put the rect height into BX
	sub	bx, ss:[si].top
	mov	ax, DstxOrg		;check if there is a leading rect on
	sub	ax, ss:[si].left	; left hand side to be drawn
	js	NoLeftRect
	jz	NoLeftRect
	mov	cx, ss:[si].left
	mov	di, ss:[si].top
	cCall	DrawThatRect
NoLeftRect:
	mov	ax, ss:[si].right	;check if there is a trailing rect on
	sub	ax, LocDstxOrg		; right hand side to be drawn
	js	NoRightRect
	jz	NoRightRect
	mov	cx, LocDstxOrg
	mov	di, ss:[si].top
	cCall	DrawThatRect
NoRightRect:
	mov	bx, DstyOrg		;check if there is a rect above the
	sub	bx, ss:[si].top 	; string to be drawn
	js	NoTopRect
	jz	NoTopRect
	mov	ax, ss:[si].right
	mov	cx, ss:[si].left
	sub	ax, cx
	mov	di, ss:[si].top
	cCall	DrawThatRect
NoTopRect:
	mov	di, DstyOrg
	add	di, FontHeight		;DI: dest Y org if visible rect to draw
	mov	bx, ss:[si].bottom	;check if there is a rect below the
	sub	bx, di			;char string to be drawn
	js	NoBottomRect
	jz	NoBottomRect
	mov	ax, ss:[si].right
	mov	cx, ss:[si].left
	sub	ax, cx
	cCall	DrawThatRect
NoBottomRect:
cEnd

cProc	DrawThatRect, <NEAR>
cBegin
	mov	dx, RECT_WIDTH_PORT	;there are at least 2 slots left in
	dec	ax			;FIFO at this point
	out	dx, ax			;set rect width
	mov	dx, RECT_HEIGHT_PORT	;and height
	mov	ax, bx
	dec	ax
	out	dx, ax
	CheckFIFOSpace	FIVE_WORDS	;make enough room for another iteration
	mov	dx, Srcx_PORT		;now set Destination X
	mov	ax, cx
	and	ax, 07ffh
	out	dx, ax
	mov	dx, Srcy_PORT		;set Destination Y
	mov	ax, di
	and	ax, 07ffh
	out	dx, ax
	mov	dx, COMMAND_FLAG_PORT	;set command to
	mov	ax, 80b3h		;draw a "Fast Rectangle"
	out	dx, ax
cEnd

cProc	ProportionalStrBlt, <NEAR>
cBegin                    
	assumes ds,Data
	assumes es,nothing
	test	bMegaFont,1		;If this is a MegaFont, then
	jz	short @f		;load ds:si --> Font.
	lds	si,lpFont
	add	si, ((offset fsCharOffset)-(offset fsType))
@@:
	cmp	OpaqueFlag,02h		;do we have to draw opaque background?
	jne	PSBSetupForLoop 	;nope, continue
	cCall	ProportionalOpaqueBackground

PSBSetupForLoop:
	mov	bl,FirstChar		;get these into registers for faster
	mov	bh,LastChar		;access in loop
	sub	bh, bl			;un-bias last character by 1st char

PSBLoop:
	test	bMegaFont,1		;Is the font too big to fit in the cache?
	jz	short PSBNotMegaGlyph
	push	si
	call	DrawMegaGlyph
	mov	si,CharExtra		;Add on any CharExtra to the running xOrigin.
	add	LocDstxOrg,si		
	or	ax,ax			;Don't worry about zero width chars.
	jz	short @f
	mov	si,ax			;put width into si.
	add	LocDstxOrg,si		;Add glyph width onto xOrigin.
	mov	al,es:[di]		;get the mega code point in string
	sub	al,bl			;subtract off first code point in set
	cmp	al,BreakChar		;Was this a Mega Break Character?
	jne	short @f		;Nope, LocDstXOrg is correct.
	cmp	PropSpaceFlag,0 	;is there any special BreakChar
					;handling?
	je 	short @f		;Nope.
	sub	LocDstxOrg,si		;subtract off adjustment and jump
	mov	ax,si			;get width back into ax.
	jmp	PSBMegaBreakCharEntersHere ;into dda code for non-mega break characters.
@@:
	jmp	PSBEndLoop		;we're done with this Mega glyph.

PSBExitNode:
	jmp	PSBExit

PSBNotMegaGlyph:

;Now set the X & Y coordinates on the screen:

	CheckFIFOSpace	SEVEN_WORDS
	mov	ax,LocDstxOrg		;get the X-origin of our destination
	cmp	ax, X_SIZE		;is it off the screen?
	jge	PSBExitNode		;yes, we're done now.
	mov	OffScreen,0		;Assume 8514 can't handle the clip.
	cmp	ax,-X_SIZE
	jl	short @f		;Too negative to be handled by the
					;8514 clip. h/w.
	mov	OffScreen,1		;8514 h/w can handle the clip.
	and	ax,7ffh 		;change negative X-coordinates into
					;outrageous positive X-coordinates
	mov	dx,Dstx_PORT
	out	dx,ax
	mov	ax,DstyOrg		;get the Y-origin of our destination
	mov	dx,Dsty_PORT
	out	dx,ax
@@:

;Add on any CharExtra to the running xOrigin:

	mov	dx,CharExtra
	add	LocDstxOrg,dx

;Now, get the character that we want to BLT and determine its place in
;invisible memory:

	push	si			;save offset of our font info table
	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next code point in string
	sub	al,bl			;subtract off first code point in set
	cmp	al, bh			;bad character?
	ja	PSBBadCharacter 	;yes, get default
PSBGoodCharacter:
	mov	dl,al			;save code point for BreakChar compare
	shl	ax, 1
	add	si,ax			;make DS:SI point to entry within
	shl	ax, 1			;6 byte per code point table
	add	si,ax

;Get the width of the character:

	lodsw				;get the width of the character into AX
	mov	dh, ah
	and	ax, 07ffh		;don't handle zero width characters
	jz	PSBEndLoop
	push	dx
	cmp	dl,BreakChar		;is it the break character?
	jne	PSBNotBreakChar 	;nope, continue

PSBBreakCharHandling:

;AX contains the width of the BreakChar.

	cmp	PropSpaceFlag,0 	;is there any special BreakChar
					;handling?
	je	PSBNotBreakChar 	;nope, continue on normally
	pop	dx			;adjust stack for exit or end of loop
PSBMegaBreakCharEntersHere:
	add	ax,BreakExtra		;get proper width
	add	LocDstxOrg,ax		;fix up position of next character
	mov	dx,BreakRem		;update the BreakErr
	or	dx,dx		
	jl	short @f
	sub	BreakErr,dx
	jge	PSBBCHExit		;if BreakErr is positive, we're done
	inc	LocDstxOrg		;it's negative or zero, adjust the DDA
	mov	dx,BreakCount
	add	BreakErr,dx	
	jmp	short PSBBCHExit
@@:
	sub	BreakErr,dx
	jl	short PSBBCHExit
	dec	LocDstxOrg		;it's negative or zero, adjust the DDA
	mov	dx,BreakCount
	sub	BreakErr,dx	

PSBBCHExit:
	jmp	short PSBEndLoop	;we're done with the break character

PSBBadCharacter:
	mov	al, DefaultChar
	jmp	short PSBGoodCharacter

PSBLoopNode:
	jmp	PSBLoop

PSBNotBreakChar:

;AX contains the width of the code point.

	add	LocDstxOrg,ax		;update the DstxOrg
	cmp 	OffScreen,0		;Are we clipped so far that 8514 can't handle it?
	je	PSBEndLoop		;Yes, don't draw anything.
	dec	ax			;decrement the width by 1
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
	pop	ax			;get back read plane information
	mov	al, ah
	shr	al, 3
	mov	dx, READ_ENABLE_PORT
	out	dx, ax

;Now set up the source X & Y as the spot in invisible memory where we're BLTing
;from:

	lodsw				;get source X-coordinate of code point
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	lodsw				;get source Y-coordinate of our
					;character
	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;Now we send the block move command to put our character where we want it
;on the displayable screen.

	mov	ax,0c0b3h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

PSBEndLoop:
	pop	si			;restore offset of FONTINFO start
PSBEndLoop2:
	inc	di			;bump to the next character in string
	loop	PSBLoopNode

PSBExit:
cEnd

ifdef	_PMODE
cProc	PCharWidthStrBlt, <NEAR, PUBLIC>

cBegin                    
	assumes ds,Data
	assumes es,nothing

;The purpose of this ExtTextOut routine is to put up the characters in 
;X-locations that are explicitly determined by a table of values (one value
;corresponding to each character in the string).  The characters may overlap
;depending on the values in this table.  Text justification (ie: the values
;in DrawMode) is never used when the CharWidth table is used.

	cmp	OpaqueFlag,02h		;do we have to draw opaque background?
	jne	PCWSBSetupForLoop	;nope, continue
	cCall	CharWidthOpaqueBackground
	jmp	short PCWSBSetupForLoop

PCWSBBadCharacter:
	mov	al, DefaultChar
	jmp	short PCWSBGoodCharacter

PCWSBSetupForLoop:
	test	bMegaFont,1		;If this is a MegaFont, then
	jz	short @f		;load ds:si --> Font.
	lds	si,lpFont
	add	si, ((offset fsCharOffset)-(offset fsType))
@@:	mov	bl,FirstChar		;get these values into registers
	mov	bh,LastChar		;for faster access in loop
	sub	bh, bl
.386
	push	fs
	mov	fs, seg_lpCharWidths
.286

public	PCWSBLoop
PCWSBLoop:

	test	bMegaFont,1		;Is the font too big to fit in the cache?
	jz	short @f
	push	si
	call	DrawMegaGlyph
	jmp	PCWSBGetCharOffset
@@:

;Now set the X & Y coordinates on the screen:
	
	CheckFIFOSpace	SEVEN_WORDS
	mov	ax,LocDstxOrg		;get the X-origin of our destination
	cmp	ax,X_SIZE
	jge	PCWSBCharClipped	;Char is not visible.
	cmp	ax,-X_SIZE
	jl	PCWSBCharClipped	;Char is not visible.
	and	ax,7ffh 		;change negative X-coordinates into
	mov	dx,Dstx_PORT		;outrageous positive X-coordinates.
	out	dx,ax
	mov	ax,DstyOrg		;get the Y-origin of our destination
	mov	dx,Dsty_PORT
	out	dx,ax

;Now, get the character that we want to BLT and determine its place in
;invisible memory:

	push	si			;save offset of FontInfoTable
	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next character to draw
	sub	al,bl			;subtract off first character in set
	cmp	al,bh			;nonesense character?
	ja	PCWSBBadCharacter	;yes, use default
PCWSBGoodCharacter:
	shl	ax, 1
	add	si,ax			;make DS:SI point to entry for this
	shl	ax, 1			;code point within the 5 byte per
	add	si,ax			;code point table

;Get the width of the character:

	lodsw				;get the width of the character in AX
	mov	dl, ah			;save read plane in dl
	and	ax, 07ffh
	jz	PCWSBEndLoop
	dec	ax			;decrement it by 1
	push	dx
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
	pop	ax
	shr	al, 3			;adjust read plane
	mov	dx, READ_ENABLE_PORT	;and set it to the board
	out	dx, ax

;Now set up the source X & Y as the spot in invisible memory where we're BLTing
;from:

	lodsw				;get source X-coordinate of our
					;character
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	lodsw				;get source Y-coordinate of our
					;character
	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;Now we send the block move command to put our character where we want it
;on the displayable screen.

	mov	ax,0c0b3h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

public	PCWSBGetCharOffset
PCWSBGetCharOffset:
	mov	si, off_lpCharWidths
.386
	lods	word ptr fs:[si]	;get the next CharWidth
.286
	mov	off_lpCharWidths,si	;(save the pointer to next CharWidth)
	cwd				;extend sign into DX
	not	dx			;invert it
	and	ax, dx			;if AX<0, make it 0, else leave as is
	add	LocDstxOrg,ax		;now we're updated for the next char

PCWSBEndLoop:
	pop	si			;restore offset of FONTINFO start
	inc	di			;bump to the next character in string
	loop	PCWSBLoop
.386
	pop	fs
.286
PCWSBExit:
cEnd

PCWSBCharClipped:
	push	si
	jmp	PCWSBGetCharOffset

endif
ifdef	_RMODE
cProc	CharWidthStrBlt, <NEAR, PUBLIC>

cBegin                    
	assumes ds,Data
	assumes es,nothing

;The purpose of this ExtTextOut routine is to put up the characters in 
;X-locations that are explicitly determined by a table of values (one value
;corresponding to each character in the string).  The characters may overlap
;depending on the values in this table.  Text justification (ie: the values
;in DrawMode) is never used when the CharWidth table is used.

	cmp	OpaqueFlag,02h		;do we have to draw opaque background?
	jne	CWSBSetupForLoop	;nope, continue
	cCall	CharWidthOpaqueBackground
	jmp	short CWSBSetupForLoop

CWSBBadCharacter:
	mov	al, DefaultChar
	jmp	short CWSBGoodCharacter

CWSBSetupForLoop:
	test	bMegaFont,1		;If this is a MegaFont, then
	jz	short @f		;load ds:si --> Font.
	lds	si,lpFont
	add	si, ((offset fsCharOffset)-(offset fsType))
@@:	mov	bl,FirstChar		;get these values into registers
	mov	bh,LastChar		;for faster access in loop
	sub	bh, bl

public	CWSBLoop
CWSBLoop:

	test	bMegaFont,1		;Is the font too big to fit in the cache?
	jz	short @f
	push	si
	call	DrawMegaGlyph
	jmp	CWSBGetCharOffset
@@:

;Now set the X & Y coordinates on the screen:

	CheckFIFOSpace	SEVEN_WORDS
	mov	ax,LocDstxOrg		;get the X-origin of our destination
	cmp	ax,X_SIZE
	jge	CWSBCharClipped		;Char is not visible.
	cmp	ax,-X_SIZE
	jl	CWSBCharClipped		;Char is not visible.
	and	ax,7ffh 		;change negative X-coordinates into
	mov	dx,Dstx_PORT		;outrageous positive X-coordinates
	out	dx,ax
	mov	ax,DstyOrg		;get the Y-origin of our destination
	mov	dx,Dsty_PORT
	out	dx,ax

;Now, get the character that we want to BLT and determine its place in
;invisible memory:

	push	si			;save offset of FontInfoTable
	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next character to draw
	sub	al,bl			;subtract off first character in set
	cmp	al,bh			;nonesense character?
	ja	CWSBBadCharacter	;yes, use default
CWSBGoodCharacter:
	shl	ax, 1
	add	si,ax			;make DS:SI point to entry for this
	shl	ax, 1			;code point within the 5 byte per
	add	si,ax			;code point table

;Get the width of the character:

	lodsw				;get the width of the character in AX
	mov	dl, ah			;save read plane in dl
	and	ax, 07ffh
	jz	CWSBEndLoop
	dec	ax			;decrement it by 1
	push	dx
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
	pop	ax
	shr	al, 3			;adjust read plane
	mov	dx, READ_ENABLE_PORT	;and set it to the board
	out	dx, ax

;Now set up the source X & Y as the spot in invisible memory where we're BLTing
;from:

	lodsw				;get source X-coordinate of our
					;character
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	lodsw				;get source Y-coordinate of our
					;character
	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;Now we send the block move command to put our character where we want it
;on the displayable screen.

	mov	ax,0c0b3h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

public	CWSBGetCharOffset
CWSBGetCharOffset:
	push	ds			;save our DS (pointer to FONTINFO)
	assumes ds,nothing
	lds	si,lpCharWidths 	;get a pointer to the CharWidths table
	lodsw				;get the next CharWidth
	mov	off_lpCharWidths,si	;(save the pointer to next CharWidth)
	cwd				;if the character width is negative
	not	dx			;make it zero, else leave it alone
	and	ax, dx
	add	LocDstxOrg,ax		;now we're updated for the next char
	pop	ds			;restore pointer to FONTINFO
	assumes ds,Data

CWSBEndLoop:
	pop	si			;restore offset of FONTINFO start
	inc	di			;bump to the next character in string
	loop	CWSBLoop

CWSBExit:
cEnd

CWSBCharClipped:
	push	si
	jmp	CWSBGetCharOffset
endif

cProc	ProportionalOpaqueBackground, <NEAR>, <cx, di, si>

cBegin

;First, perform the ENTIRE DDA to determine the string's length:

;Entry:
;       CX contains the nbr of characters in the string.
;       ES:DI points at the string.                     
;       DS:SI points at the font information table in the Data segment.
;                           - or - 
;         For Mega Fonts, DS:SI point to the width table of the font.
;

;First, calculate how many pixels are needed to compensate for CharExtra

	mov	ax,CharExtra		;get nbr of pixels per CharExtra
	mul	cx			;now AX has nbr of pixels for CharExtra
	mov	bx,ax			;BX contains the running xExt of string
	jmp	short POBLoop

POBBadCharacter:
	mov	al, DefaultChar
	jmp	short POBGoodCharacter

POBLoop:

;Now, get the next character and get its width:

	push	si			;save offset of our font info table
	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next code point in string
	cmp	al,LastChar		;nonesense code point?
	ja	POBBadCharacter 	;yes, use default
	sub	al,FirstChar		;subtract off first code point in set
	jb	POBBadCharacter 	;nonesense code point, get default
POBGoodCharacter:
	mov	dl,al			;save code point for BreakChar compare
	test	bMegaFont,1
	jz	short @f
ifdef _RMODE
.286
	shl	ax,2			;ax*4
endif
ifdef _PMODE
.386
	shl	ax,1			;ax*2
	push	bx
	mov	bx,ax
	shl	ax,1			;ax*4
	add	ax,bx			;dx*6
	pop	bx
endif

	add	si,ax
	lodsw				;Get width.
	jmp	PPOB_HaveWidth
@@:	
	shl	ax, 1
	add	si,ax			;make DS:SI point to entry within
	shl	ax, 1			;6 byte per code point table
	add	si,ax

;Get the width of the character:

	lodsw				;get the width of the character into AX
	and	ax, 03ffh		;mask out write plane information

PPOB_HaveWidth:
	cmp	dl,BreakChar		;is it the break character?
	jne	POBSaveWidth		;nope, continue

PPOBBreakCharHandling:

;AX contains the width of the BreakChar.

	cmp	PropSpaceFlag,0 	;is there any special BreakChar
					;handling?
	je	POBSaveWidth		;nope, continue on normally
	add	ax,BreakExtra		;get proper width

POBSaveWidth:

;AX contains the width of the code point.

	add	bx,ax			;update the running xExt

POBEndLoop:
	pop	si			;restore offset of FONTINFO start
	inc	di			;bump to the next character in string
	loop	POBLoop
	cCall	DrawBackgroundRect	;go draw the background rectangle
cEnd

cProc	CharWidthOpaqueBackground, <NEAR>, <ds, si, cx, dx>
cBegin

;First, perform the ENTIRE DDA to determine the string's length:
;Entry:
;       CX contains the nbr of characters in the string.

	assumes ds,nothing
	lds	si,lpCharWidths 	;get a pointer to the CharWidths table
	xor	bx,bx			;BX contains the running xExt of string

CWOBLoop:
	lodsw				;get the next CharWidth
	cwd				;if the char width is negative make it
	not	dx			;zero, else leave as is
	and	ax, dx
	add	bx,ax			;add it onto the running xExt
	loop	CWOBLoop
	cCall	DrawBackgroundRect	;go draw the background rectangle
cEnd

cProc	DrawBackgroundRect, <NEAR>
;Entry:
;       BX contains the xExt of the rectangle to draw.

cBegin
	assumes ds,Data
	MakeEmptyFIFO
	mov	dx,FUNCTION_1_PORT
	mov	al,07h			;set to opaque replace using background
	out	dx,ax			;colour

;Now, set the mode to "solid colour"

	mov	dx,MODE_PORT
	mov	ax,0a000h		;this is "normal mode"
	out	dx,ax
	mov	ax,DstxOrg
	mov	cx,X_SIZE
	cmp	ax,cx
	jg	short DBR_Exit
	neg	cx			;cx = -X_SIZE
	sub	cx,ax			;cx = -XSIZE - DstxOrg
	jg	short @f		;if >0, then peg DstxOrg to -X_SIZE
	xor 	cx,cx			;otherwise, don't mess with DstxOrg
@@:	add	ax,cx			;Adjust DstxOrg
	mov	si,ax			;si == adjusted DstxOrg
	mov	dx,Srcx_PORT		;set the starting X
	out	dx,ax
	mov	ax,DstyOrg
	mov	dx,Srcy_PORT		;set the starting Y
	out	dx,ax
	mov	ax,bx			;get the xExt of the string
	sub	ax,cx			;Subtract off any adjustment.
	mov	cx,ax
	add	cx,si			;cx = end of rectangle.
	cmp	cx,0			;off of the left of the screen?
	jl	DBR_Exit		;yes. No need to draw it.
	sub	cx,X_SIZE		;Compute how much over the right it is.
	jg	short @f		;If >0, then peg to right side.
	xor	cx,cx			;otherwise, don't mess with the width.
@@:	sub	ax,cx			;adjust the width.
	dec	ax
	mov	dx,RECT_WIDTH_PORT	;set the width
	out	dx,ax
	mov	ax,40f3h		;this is command to draw rectangle
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

;Now, we must reset the FUNCTION 1 and MODE ports to their previous values:

	mov	dx,FUNCTION_1_PORT
	mov	al,27h			;set to opaque replace using foreground
	out	dx,ax			;colour

;Now, reset the mode to "block move pattern"

	mov	dx,MODE_PORT
	mov	ax,BLOCK_MOVE_MODE
	out	dx,ax
DBR_Exit:
cEnd

cProc	IntersectClip, <NEAR>, <ds>

cBegin

;This process creates a new clipping rectangle which is the intersection 
;of the rectangles pointed to by lpClipRect and lpOpaqueRect.  If lpOpaqueRect
;is null or the Options flag is 0, we simply use the clipping rectangle 
;pointed to by lpClipRect.  We also validate the clipping rectangle. If the
;rectangle is 0 by 0 or is invalid, we return a carry flag.
;Exit:
;       MinimumClipRect contains the intersected clipping/opaquing rectangle.
;	All registers except DS are assumed destroyed.
;First, see if lpClipRect exists.  If it doesn't, set it to point at 
;FullScreenClip:

	assumes ds,nothing
	assumes es,nothing
	cmp	seg_lpClipRect,0	;is lpClipRect null?
	jne	ICGetClipRect		;no, continue
	mov	seg_lpClipRect,ds	;otherwise, point it at FullScreenClip
	mov	off_lpClipRect,DataOFFSET FullScreenClip

ICGetClipRect:
	lds	si,lpClipRect		;assume there's no opaquing rectangle
	lodsw				;get starting X in DX
	mov	dx,ax
	lodsw				;get starting Y in CX
	mov	cx,ax
	lodsw				;get ending X in BX
	mov	bx,ax
	lodsw				;get ending Y in AX

;Now see if there's an opaquing rectangle to intersect with:

	cmp	seg_lpOpaqueRect,0	;is there an opaquing rectangle?
	je	ICValidateIntersection	;nope, just use the clipping rectangle
	test	Options,02h		;does caller want us to use the
					;opaquing rectangle for anything?
	jnz	ICIntersectOpaqueRect	;yes, go do the intersection
	test	Options,04h
	jz	ICValidateIntersection	;nope, just use the clipping rectangle

ICIntersectOpaqueRect:

;We have an opaquing rectangle.  Intersect it with the clipping rectangle:

	les	di,lpOpaqueRect 	;get the ExtTextOut rectangle
	cmp	dx,es:[di+0]		;compare the starting X's
	jge	IC1			;minimize starting X's
	mov	dx,es:[di+0]

IC1:
	cmp	cx,es:[di+2]		;compare the starting Y's
	jge	IC2			;minimize starting Y's
	mov	cx,es:[di+2]

IC2:
	cmp	bx,es:[di+4]		;compare the ending X's
	jle	IC3			;minimize ending X's
	mov	bx,es:[di+4]

IC3:
	cmp	ax,es:[di+6]		;compare the ending Y's
	jle	ICValidateIntersection	;minimize ending Y's
	mov	ax,es:[di+6]

ICValidateIntersection:
	cmp	bx,dx			;is ending X <= starting X?
	jle	ICNoDraw		;yes, there's nothing to draw
	cmp	ax,cx			;in ending Y <= starting Y?
	jle	ICNoDraw		;yes, there's nothing to draw

ICStoreNewClipRect:

;Now make a new clip rectangle:

	mov	di,ss			;make ES --> Stack
	mov	es,di
	lea	di,ss:MinimumClipRect
	xchg	ax,dx			;get starting X
	stosw
	mov	ax,cx			;get starting Y
	stosw
	mov	ax,bx			;get ending X
	stosw
	mov	ax,dx			;get ending Y
	stosw
	clc				;clear the error flag
	jmp	short ICExit		;and get out

ICNoDraw:
	stc				;set the error flag

ICExit:
cEnd

cProc	DrawOpaqueingRect, <NEAR>

cBegin
	assumes ds,nothing
	MakeEmptyFIFO
	mov	al,BackgroundColour	;set the foreground colour to the
	mov	dx,COLOUR_0_PORT	;passed background colour
	out	dx,ax
	mov	dx,FUNCTION_1_PORT
	mov	al,07h			;set to opaque replace using background
	out	dx,ax			;colour

;Now, set the mode to "solid colour"

	mov	dx,MODE_PORT
	mov	ax,0a000h		;this is "normal mode"
	out	dx,ax
	lea	si,MinimumClipRect	;get the pointer to the rectangle that					      ;we're to draw
	mov	dx,ss
	mov	ds,dx
	lodsw				;get starting X
	mov	dx,Srcx_PORT		;set the starting X
	out	dx,ax
	lodsw				;get starting Y
	mov	di,ax			;save it
	mov	dx,Srcy_PORT		;set the starting Y
	out	dx,ax
	lodsw				;get ending X
	dec	ax
	sub	ax,[si-6]		;get (X-extent - 1)
	mov	dx,RECT_WIDTH_PORT	;set the width
	out	dx,ax
	lodsw				;get ending Y
	dec	ax
	sub	ax,di			;get (Y-extent -1)
	mov	dx,RECT_HEIGHT_PORT	;set the height
	out	dx,ax
	mov	ax,40f3h		;this is command to draw rectangle
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax
cEnd

;---------------------------------------------------------------------------
;MegaFont support.
;August 7, 1991 -by- [raypat]
;entry:
; LocDstxOrg: X coordinate of glyph
; DstyOrg   : Y coordinate of glyph
; es:di	    : lpString
; ds:si	    : lpFont -- CharOffset array
; bl	    : 1st character in font.
; bh	    : last character in font.
; preserves:
;  cx,di,bx
;---------------------------------------------------------------------------
DMG_CharClipped:
	ret

DMG_BadCharacter:
	mov	al, DefaultChar 	;in case of an invalid char code, use
	jmp	short DMG_GoodCharacter	;default char and continue.

ifdef	_PMODE
public	pDrawMegaGlyph
pDrawMegaGlyph:
endif
ifdef	_RMODE
public	rDrawMegaGlyph
rDrawMegaGlyph:
endif

DrawMegaGlyph	proc	near
	mov	ax,LocDstxOrg		;get the X-origin of our destination
	cmp	ax,X_SIZE
	jge	DMG_CharClipped		;Char is not visible.
	cmp	ax,-X_SIZE
	jl	DMG_CharClipped		;Char is not visible.
	and	ax,7ffh 		;change negative X-coordinates into
	mov	wDestX,ax		;huge positve coordinates.
	mov	ax,DstyOrg		;get the Y-origin of our destination
	mov	wDestY,ax

	CheckFIFOSpace	TWO_WORDS		
	xor	ax,ax			;1 scan line at a time.
	mov	dx,RECT_HEIGHT_PORT
	out	dx,ax			;ax=0a080h
	mov	ax,USER_DEFINED_PATTERN_MODE
	out	dx,ax

;Now, get the character that we want to BLT and determine its place 
;on the screen:

	xor	ax,ax			;make sure AH is clear
	mov	al,es:[di]		;get next character to draw
	sub	al,bl			;subtract off first character in set
	cmp	al,bh			;nonsense character?
	ja	DMG_BadCharacter	;yes, use default
DMG_GoodCharacter:
	xor	ah,ah			;Compute offset to width and 
	shl	ax,1			;pointer to glyph bits.
	mov	dx,ax
	shl	ax,1			;ax*4
	add	dx,ax			;dx*6
	push	cx
	push	di
	push	bx

ifdef _PMODE
.386
	add	si,dx
	lodsw				;Get width.
	mov	cx,ax			;cx = width.
	jcxz	DMG_Exit		;Width is zero, nothing to draw.
	mov	esi,[si]		;esi = --> Glyph Pattern
endif
ifdef _RMODE
.286
	add	si,ax
	lodsw				;Get width.
	mov	cx,ax			;cx = width.
	jcxz	DMG_Exit		;Width is zero, nothing to draw.
	mov	si,[si]			;si = --> Glyph Pattern
endif
	push	ax
	dec	ax	
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
ifdef _RMODE
	call	RDrawFont
endif
ifdef _PMODE
	call	PDrawFont
endif
	pop	ax			;return width of glyph in ax.
DMG_Exit:
	pop	bx
	pop	di
	pop	cx
	ret
DrawMegaGlyph	endp

ifdef	_PMODE
sEnd	pCode
endif
ifdef	_RMODE
sEnd	rCode
endif
end
