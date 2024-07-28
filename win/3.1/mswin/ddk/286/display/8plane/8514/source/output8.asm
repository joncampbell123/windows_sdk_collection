.286
.xlist
include cmacros.inc
include gdidefs.inc
include brush.inc
.list

BITSPIXEL	equ	8

sBegin	Data
externB Palette
sEnd	Data

createSeg   _DEIGHT, DynamicEight, word, public, CODE
sBegin	    DynamicEight

assumes cs, DynamicEight
assumes ds, nothing

externB rot_bit_tbl8			;in PIXEL8.ASM

;       Output And Pixel Drawing Mode Logic Sequences
;
;       The logical templates for the requested function follow.  The
;       code will be copied into the created DDA and executed there.
;
;       The code will also be called from the PIXEL routine to perform
;       the nessacary drawing mode for that function.
;
;       The logical sequences assume the following on entry:
;
;       Entry:  al = current pen
;               ah = destination



DDx:                                    ;D = 0
	xor	al,al			;Rop2=1
DDxEnd:
        ret


DPna:                                   ;D = D AND (NOT P)
	not	al			;Rop2=3
        and     al,ah
DPnaEnd:
        ret



Pn:                                     ;D = NOT P
	not	al			;Rop2=4
PnEnd:
        ret



PDna:                                   ;D = (NOT D) AND P
	not	ah			;Rop2=5
        and     al,ah
        not     ah
PDnaEnd:
        ret



D:                                      ;D = D   (I hope nobody ever uses this)
	mov	al,ah			;Rop2=11
DEnd:
        ret



Dn:                                     ;D = NOT D
	mov	al,ah			;Rop2=6
        not     al
DnEnd:
        ret



DPx:                                    ;D = D XOR P
	xor	al,ah			;Rop2=7
DPxEnd:
        ret



DPxn:                                   ;D = NOT (D XOR P)
	xor	al,ah			;Rop2=10
        not     al
DPxnEnd:
        ret



DPa:                                    ;D = D AND P
	and	al,ah			;Rop2=9
DPaEnd:
        ret



DPan:                                   ;D = NOT (D AND P)
	and	al,ah			;Rop2=8
        not     al
DPanEnd:
        ret



DPno:                                   ;D = (NOT P) OR D
	not	al			;Rop2=12
        or      al,ah
DPnoEnd:
        ret



PDno:                                   ;D = (NOT D) OR P
	not	ah			;Rop2=14
        or      al,ah
        not     ah
PDnoEnd:
        ret



DPo:                                    ;D = D OR P
	or	al,ah			;Rop2=15
DPoEnd:
        ret



DPon:                                   ;D = NOT (D OR P)
	or	al,ah			;Rop2=2
        not     al
DPonEnd:
        ret



DDxn:                                   ;D = 1
	mov	al,0FFH 		;Rop2=16
DDxnEnd:
        ret



P:                                      ;D = P action routine
PEnd:					;Rop2=13
        ret



;       The drawing mode table contains the starting address of the code
;	template for each drawing mode.
public	DrawModeTbl8
DrawModeTbl8	label	word

        dw      DDx                     ;D = 0
        dw      DPon                    ;D = NOT (D OR P)
        dw      DPna                    ;D = D AND (NOT P)
        dw      Pn                      ;D = NOT P
        dw      PDna                    ;D = (NOT D) AND P
        dw      Dn                      ;D = NOT D
        dw      DPx                     ;D = D XOR P
        dw      DPan                    ;D = NOT (D AND P)
        dw      DPa                     ;D = D AND P
        dw      DPxn                    ;D = NOT (D XOR P)
        dw      D                       ;D = D
        dw      DPno                    ;D = (NOT P) OR D
        dw      P                       ;D = P
        dw      PDno                    ;D = (NOT D) OR P
        dw      DPo                     ;D = D OR P
        dw      DDxn                    ;D = 1


if 0

;       The drawing mode length table contains the length of the code
;       for each drawing mode.  The length is needed for computing how
;       many bytes of code is to be moved into the created line drawing
;       code.

DrawModeLen8	label	byte

        db      DDxEnd  - DDx           ;D = 0
        db      DPonEnd - DPon          ;D = NOT (D OR P)
        db      DPnaEnd - DPna          ;D = D AND (NOT P)
        db      PnEnd   - Pn            ;D = NOT P
        db      PDnaEnd - PDna          ;D = (NOT D) AND P
        db      DnEnd   - Dn            ;D = NOT D
        db      DPxEnd  - DPx           ;D = D XOR P
        db      DPanEnd - DPan          ;D = NOT (D AND P)
        db      DPaEnd  - DPa           ;D = D AND P
        db      DPxnEnd - DPxn          ;D = NOT (D XOR P)
        db      DEnd    - D             ;D = D
        db      DPnoEnd - DPno          ;D = (NOT P) OR D
        db      PEnd    - P             ;D = P
        db      PDnoEnd - PDno          ;D = (NOT D) OR P
        db      DPoEnd  - DPo           ;D = D OR P
	db	DDxnEnd - DDxn		;D = 1
endif

cProc	OutputStackDummy, <FAR> 	;make a stack frame compatible with
	include output.inc		;output
cBegin	<nogen>
cEnd	<nogen>

GetBitmapParams proc near
;At this point:
;       ES:DI points at the BITMAP data structure.

	mov	ax,yCoordinate		;get back the yCoordinate
	xor	bx,bx			;BX must be zero in case we have
					;a standard bitmap
	mov	cx,es:[di].bmSegmentIndex   ;are we a > 64K bitmap?
	jcxz	MScStandardBitmap	;yes, just go do regular calc

;At this point, AX has our Y-coordinate.  We need to calculate in which segment
;of the bitmap this scanline lies.

	mov	dx,es:[di].bmScanSegment    ;Get # scans per segment
MScHugeBitmapLoop:
	add	bx,cx			;add segment-to-segment increment
	sub	ax,dx			;if AX becomes negative, we
					;have reached the proper 64K
	jns	MScHugeBitmapLoop	;Not in current segment, try next
	add	ax,dx			;AX will now have Y-coordinate
					;within our segment
	sub	bx,cx			;and BX will have adder to
					;correct segment
MScStandardBitmap:
	mul	word ptr es:[di].bmWidthBytes	;now AX has adder to start of
						;our scanline in memory
	add	bx, word ptr es:[di].bmBits[2]	;get correct segment
	mov	di, word ptr es:[di].bmBits[0]	;get offset of bitmap start
	add	di,ax			;now DI points to start of
					;our line in memory
	mov	es,bx			;now ES has the correct segment
	ret
GetBitmapParams endp

InnerWriteLoop	proc near
MScInnerWriteLoop:
	mov	ah,es:[di]		;get next byte from dest
	lodsb				;get next pixel from dither pattern
	call	Rop2
	stosb
	loop	MScInnerWriteLoop
	ret
InnerWriteLoop	endp


;on exit: OpaqueFlag and Rop2 are set to valid values
;modifies ax, bx, si

cProc	GetDrawModeParams8, <NEAR, PUBLIC>
cBegin	nogen
	mov	cx, bx			;save Y coordinate in CX
	lds	si, lpDrawMode
	lodsw				;get ROP2 into AX
	or	ax, ax			;now verify that the ROP is within
	jz	InvalidRop2		; [1..16]
	cmp	ax, 16
	ja	InvalidRop2
	mov	bx, ax
	dec	bx			;make it offset from 0
	shl	bx,1			;and make it a word offset
					;into DRAWMODE table
;	add	bx,DynamicEightOFFSET DrawModeTbl8  ;add on base of table
	mov	ax,cs:[bx].DrawModeTbl8 ;now AX has offset of raster
					;op drawing code
	mov	Rop2,ax 		;and save it
	lodsw				;get opaque flag
	mov	OpaqueFlag,al		;save it
	lodsw				;AL: bkColor, AH: bkMono
	mov	BackgroundColour, al	;save it
	shr	ah, 1
	sbb	al, al
	mov	BackgroundMono, al
	inc	si			;advance si to text color
	inc	si
	lodsw				;AL: TextColor
	mov	ForegroundColour, al	;save it too
	shr	ah, 1
	sbb	al, al
	mov	ForegroundMono, al
	mov	bx, cx			;BX: Y coordinate
	clc
	ret
InvalidRop2:
	stc
	ret
cEnd	nogen

cProc	MemoryScanline8,<FAR, PUBLIC>

cBegin	<nogen>
	les	di,lpDstDev		;get pointer to bitmap
	mov	al,es:[di].bmBitsPixel	;get the colour format
	mov	ColourFormat,al 	;save it
	lds	si,lpPoints		;DS:SI points at our point array
	inc	si			;make SI point at Y-coordinate
	inc	si
	lodsw				;get the Y-coordinate
	or	ax, ax			;is the Y coordinate negative?
	js	MScLeave2		;then get out at once!
	mov	cx, Count		;CX: count
	dec	cx			;correct for point containing Y corrd.
	jcxz	MScLeave2		;is it zero? If so. leave
	mov	Count, cx		;save corrected count
	mov	off_lpPoints,si 	;make lpPoints point at the
					;first X-coordinate
	mov	yCoordinate,ax		;save the Y-coordinate
	mov	bx,ax			;and get it as a pattern index
	cCall	GetDrawModeParams8	;get Rop2, fg/bg colors, opaque flag
	jc	MScLeave2		;exit in case of an invalid Rop2
	cmp	seg_lpPBrush,0		;any brush?
	je	MScGetPen		;nope, try for a pen
	lds	si,lpPBrush		;get the brush into DS:SI
	mov	ax, word ptr [si].bnStyle   ;get brush type in AL
	.errnz	bnStyle 		;get brush colour (if solid) in AH
	.errnz	bnColor-bnStyle-1
	cmp	al, 4			;hatched brush?
	je	MScOpaqueFlagSet	;yes, leave opaque flag as it is
	mov	OpaqueFlag, 2		;no, make object opaque
MScOpaqueFlagSet:
	or	al,al			;is brush type solid?
	jz	MScSolidBrushNode	;yes, continue
	cmp	al, 5			;dithered brush?
	je	MScDitheredBrush	;indeed.
	cmp	al,1			;is brush hollow?
	je	MScGetPen		;yes, try for a pen

MScPatternedBrush:

;If AL==4: we're dealing with a hatched brush, which is in full color and there
;will be no need to change the colors any further.
;If AL==2: we're dealing with a monochrome patterned brush. This means we need
;to pull the current background/text color out of the draw mode we were passed
;with this call.
;Note: only hatched brushes (style==4) can be transparent!
;Our brush is patterned. we must check for a colour brush and do any corrections
;necesary (see comment in HandleBrush and above).  We must also retrieve the
;pattern for this scanline:

	and	bx,07h			;get the index for the pattern
	mov	dl,al			;get brush style into DL
	mov	al,[bx+si].bnPattern	;get the pattern into AL
	mov	BrushStyle,al		;and save it for later

	mov	ax, word ptr [si].bnBgColor	;assume hatched brush style
	mov	cx, word ptr [si].bnBgMono
	or	dl, dl			;is it a solid brush in disguise?
	jz	MScSaveColourNode	;yes.  Colors are okay as is
	xchg	al, ah			;assume colored patterned brush
	xchg	cl, ch
	cmp	dl, 2			;mono patterned brush?
	jne	MScSaveColourNode	;no.  use colors as is
	mov	al, BackgroundColour	;as provided in the draw mode
	mov	ah, ForegroundColour
	mov	cl, BackgroundMono
	mov	ch, ForegroundMono
MScSaveColourNode:
	jmp	MScSaveColour

MScGetPen:
	cmp	seg_lpPPen,0		;any pen?
	je	MScLeave2		;nope, get out
	lds	si,lpPPen		;get the pen
	lodsw				;get pen type in AL, colour in AH
	mov	cl, [si]		;get mono bit into cl
	shr	cl, 1
	sbb	cx, cx
	mov	OpaqueFlag, 2		;we only support solid pens-always opq.
	or	al,al			;is it a solid pen?
	jz	MScTrueSolidBrush	;yes, treat it as a solid brush

MScLeave2:
	jmp	MScExit 		;nope, get out

MScSolidBrushNode:
	jmp	short MScSolidBrush

MScMakePatternedBrush:			;pretend that this is not a dithered
	mov	al, 0			;but a solid brush to get fg/bg colors
	jmp	short MScPatternedBrush ;right

MScDitheredBrush:

;At this point DS:SI points to the beginning of the PBrush structure

	cmp	ColourFormat, BITSPIXEL ;make sure we're dealing with a color
	jne	MScMakePatternedBrush	;bitmap.  If not, do solid brush stuff

	call	GetBitmapParams 	;es:di -> Bitmap, on ret. es:di -> bits
	add	si, bnColorBits 	;now si points to the dither pattern
	mov	ax, yCoordinate
	and	ax, 07h
	shl	ax, 3
	add	si, ax			;now si points to the row we need
	lea	bx, RectPoints
	mov	cx, 4			;a local copy of the dither pattern
MScCopyPatternLoop:			;will be copied into the local stack
	lodsw				;frame
	mov	ss:[bx], ax
	inc	bx
	inc	bx
	loop	MScCopyPatternLoop

	lds	si, lpPoints
	mov	bx, Rop2		;SS:BX-->Rop2 routine
	mov	cx, Count
	mov	dx, 07h
	push	bp
	lea	bp, RectPoints		;SS:BP-->pattern to draw with

;At this point: DS:SI-->array of points, ES:DI-->destination bitmap

MScDitherDrawLoop:
	push	cx			;save # of line segments to draw
	push	di
	lodsw				;get starting X
	mov	cx, ax
	lodsw				;get ending X
	push	si			;save SI for later
	mov	si, cx
	add	di, si			;ES:DI-->where to start drawing
	and	si, dx			;SI: starting X modulo 8
	sub	cx, ax
	neg	cx			;CX: number of pixels to process
	jcxz	MScEndLoop		;don't get screwed
MScPixelProcessLoop:
	mov	ah, es:[di]		;get destination pixel
	mov	al, [bp][si]		;get pattern pixel
	call	bx			;call Rop2
	inc	si			;update source pointer
	and	si, dx			;but only modulo 8
	stosb				;save processed pixel
	loop	MScPixelProcessLoop
MScEndLoop:
	pop	si			;now DS:SI-->points
	pop	di
	pop	cx			;CX: outer loop counter
	loop	MScDitherDrawLoop

	pop	bp			;now we can address local variables

MScLeave:
	jmp	short MScExit		;nope, get out

MScSolidBrush:

;At this point:
;   BX: Y coordinate
;   DS:SI --> PBrush structure
;   ES:DI --> bitmap structure.
;
;Many times we'll be passed a solid brush with a colour which is not black or
;white.  We must therefore dither some of our solid brushes to get the proper
;effect when drawing into a main memory bitmap.

	cmp	ColourFormat,BITSPIXEL	;is destination in colour?
	je	MScTrueSolidBrush	;yes, no need to dither

;At this point we are dealing with a solid brush.  This brush is to be copied
;into a monochrome DC (bmp).  At this point, it is time to possible to treat
;it just like a patterned brush as a monochrome dither pattern was created at
;realize object time such that it looks just like a monochrome patterned
;brush!

	jmp	MScPatternedBrush

MScTrueSolidBrush:
	not	al			;make the brush pattern solid
	mov	BrushStyle,al		;save the pattern
	mov	al, ah			;copy foreground color into AL now

MScSaveColour:

;At this point:
;AL: background color to use for pixels that are=1
;AH: foreground color to use for pixels that are=0
;ES:DI-->PDevice (bitmap)

	cmp	ColourFormat, BITSPIXEL
	je	MScSaveColourValue
	mov	ax, cx
MScSaveColourValue:
	xchg	al, ah			;in this routine: 1->fg, 0->bg color
	mov	ForegroundColour,ah	;save the foreground colour
	mov	BackgroundColour, al

MScSetUpLoop:
	call	GetBitmapParams
	lds	si,lpPoints		;DS:SI points at our first
	mov	cx,Count		;make CX a loop counter
;	 dec	 cx			 ;but decrement to correct for
					;point containing Y-coordinate
	jcxz	MScExit 		;if no scanlines, get out

;Now get the point array and find place in target bitmap to start drawing:

MScScanlineLoop:

;At this point:
;       DS:SI points at next X-coordinate to do.
;       ES:DI points to start of scanline in bitmap.
;       CX contains nbr of scanlines left to do.

	push	cx			;save our loop counter
	push	di			;save offset of scanline start
	mov	bl,BrushStyle		;get the brush pattern into BL
	mov	bh,OpaqueFlag
	dec	bh			;BH will be 0 if transparent
					;	    1 if opaque
	lodsw				;get starting X-coordinate

MScSLRotateBrush:
	mov	cl,al			;get bit number in brush pattern
	and	cl,07h			;now we have brush rotate factor
					;in CL
	rol	bl,cl			;now the brush is rotated into
					;it's starting position
	mov	dx,ax			;save starting X in DX
	lodsw				;get ending X-coordinate

MScSLGetxExt:
	sub	ax,dx			;now AX has the X-extent (we
					;don't draw the last coordinate)
	mov	cx,ax			;make X-extent a loop counter
	jcxz	MScSLEnd		;if no X-extent, skip this
					;scanline
	cmp	ColourFormat,8		;are we in colour?
	je	MScSLColour		;yes, go do it
	cCall	MonoMemoryScanline	;otherwise, go do it monochrome
	jmp	short MScSLEnd		;and go prepare for the next
					;scanline
MScSLColour:
	add	di,dx			;now ES:DI points to starting X

MScByteLoop:
	mov	ah,es:[di]		;get next byte from dest
	mov	al,ForegroundColour	;assume brush is solid

MScBLGetPattern:
	or	bl,bl			;set the sign bit
	js	MScDoRop		;it was foreground, continue

;We have a background bit set in the brush.  Decide whether to do it based on
;the state of the OpaqueFlag (in BH):

	mov	al,BackgroundColour	;assume we're opaque
	or	bh,bh			;are we opaque?
	jnz	MScDoRop		;yes, go and paint the
					;background
	inc	di			;otherwise, leave the background
					;alone
	jmp	short MScBLEnd		;and go do the next pixel

MScDoRop:
	call	Rop2			;go do the drawmode
	stosb				;and send down completed byte

MScBLEnd:
	rol	bl,1			;set up for next pixel in brush
	loop	MScByteLoop		;and loop till done

MScSLEnd:
	pop	di			;restore pointer to start of
					;scanline in destination
	pop	cx			;restore scanline loop counter
	loop	MScScanlineLoop 	;and loop for all scanlines

MScExit:
	ret
cEnd	<nogen>

cProc		MonoMemoryScanline, <NEAR>

cBegin	<nogen>

;Entry:
;       BL has the brush pattern pre-rotated to our starting X-coordinate.
;       BH has 0 if we're transparent, 1 if we're opaque.
;       CX has the nbr of pixels to do in this scanline. 
;       DX has the starting X-coordinate for this scanline.
;       DS:SI points at the next scanline's starting X-coordinate.
;       ES:DI points to start of our scanline in main memory.

	mov	ax,dx			;for MOD 8 operations
	shr	dx,3			;get byte offset of start of
					;scanline in main memory
	add	di,dx			;now ES:DI points to byte
					;containing starting X
	and	al,07h			;get the bit offset of
					;starting X within the byte
	mov	dx,bx			;get pattern and opaque flag into DX
	mov	bx,DynamicEightOFFSET rot_bit_tbl8 ;now DS:BX points to mask
	xlat	cs:rot_bit_tbl8 	;now AL has the correct bitmask
					;for the starting X-coordinate
	mov	RunningBitMask,al	;save this

MMSByteLoop:

;We want to get the brush bit extended into an entire byte.  We do this
;by getting the brush bitmask (which has the next brush pixel to do in its
;sign bit (bit 7)), and sign extending it into a byte using the CBW instruction.

	mov	al,dl			;get the next brush pattern
	cbw				;sign extend it into AH
	mov	al,ah			;now AL has the proper brush
	or	al,al			;is next pixel foreground?
	mov	al,ForegroundColour	;(assume it's foreground)
	jnz	MMSDoRop		;yes, go set the pixel

;We have a background bit set in the brush.  Decide whether to do it based on
;the state of the OpaqueFlag (in DH):

	mov	al,BackgroundColour	;assume we're opaque
	or	dh,dh			;are we opaque?
	jz	MMSBLNextPixel		;nope, skip painting this pixel

MMSDoRop:
	mov	ah,es:[di]		;get the next byte to be operated on
	call	Rop2			;go do the drawmode

;Now, we must mask the bit in question based upon the RunningBitMask:

	mov	ah,RunningBitMask	;get the bit mask
	and	al,ah			;isolate the proper bit in AL
	jz	MMSBLTurnOffPixel	;if isolated bit is 0, go force
					;it off in the destination
	or	es:[di],al		;turn on the bit
	jmp	short MMSBLNextPixel	;and continue

MMSBLTurnOffPixel:
	not	ah			;turn on all bits except the one
					;that we want to turn off
	and	es:[di],ah		;turn off the bit

MMSBLNextPixel:
	rol	dl,1			;set up for next pixel in brush
	shr	RunningBitMask,1	;have we reached the end of our byte?
	jnc	MMSBLEnd		;nope, continue
	inc	di			;bump to the next byte
	mov	RunningBitMask,80h	;and reset to first bit in byte

MMSBLEnd:
	loop	MMSByteLoop		;and loop till done
	ret
cEnd	<nogen>

sEnd	DynamicEight
end
