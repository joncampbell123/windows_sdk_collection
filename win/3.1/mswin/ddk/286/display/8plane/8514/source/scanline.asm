page            ,132
title           Scanlines Support for the IBM 8514
.286c

.xlist
include 	CMACROS.INC
include 	8514.INC
include 	brush.inc
.list

subttl          Data Area
page +
sBegin          Data
externB 	Rop2TranslateTable	;in INIT.ASM
externB WriteEnable
sEnd            Data

sBegin          Code
externFP	CursorExclude		;in ROUTINES.ASM
sEnd            Code

subttl          Code Area
page +


createSeg       _OUTPUT,OutputSeg,word,public,CODE
sBegin		OutputSeg

assumes         cs,OutputSeg
assumes         ds,Data

externNP	DrawInterior			; in Rectngle.asm


subttl          Code for Scanline Support
page +
cProc           OutputDummy,<FAR>

include         OUTPUT.INC                      ;contains stack definitions

cBegin          <nogen>     

;This routine exists so that we set up a stack frame which is correct for 
;our common Output stack frame.  It's never called but allows us to make
;near calls to Output routines.

cEnd            <nogen>

page +
cProc           Scanlines,<NEAR,PUBLIC>,<ds>

cBegin

;First, set the cursor exclusion.  We want to use the Y-coordinate
;as our cursor exclusion area.

	mov	al, [WriteEnable]
	mov	BandFlags, al		;save bit mask in BandFlags
	mov	ax,ds			;make ES = Data
	mov	es,ax
	assumes es, Data
	lds	si,lpPoints		;DS:SI has address of scanline
	assumes ds, nothing		;point array
	mov	di,[si+2]		;get Y-coordinate into DI
	mov	yCoordinate,di		;save it locally
	arg	0			;this is low X
	arg	di			;this is Y
	arg	X_SIZE			;this is high X
	arg	di			;this is Y
	cCall	CursorExclude		;go exclude the cursor
	cCall	HandleDrawMode		;this will leave DS:SI-->lpDrawMode
	jc	SCLeave

;Now, get the brush:

	cmp	seg_lpPBrush, 0 	;is there a brush
	je	SCGetPen		;nope, get the pen
	lds	si,lpPBrush		;get the PBRUSH into DS:SI

public  SCGetPattern
SCGetPattern:

;DS:SI points to brush structure

	lodsw				;get brush style in AL
	.errnz	bnStyle 		;get solid brush colour in AH
	.errnz	bnColor-bnStyle-1
	cmp	al, 4			;hatched brush?
	je	SCOpaqueFlagSet 	;yes, leave opaque flag as it is
	mov	OpaqueFlag, 2		;no, make object opaque
SCOpaqueFlagSet:
	cmp	al, 5			; check if it's a dithered brush
	je	SCGetDitheredBrush
	dec	al			;hollow brush?
	jz	SCGetPen		;yes, go back and check for pen
	jns	SCGetPatternedBrush	;nope, continue

;We have a solid brush, AL has the solid brush pattern (= FFH from the 
;decrement instruction) and AH has the colour.

public	SCGetSolidBrush
SCGetSolidBrush:

	mov	bh,ah			;get solid brush colour in BH
	jmp	short SCCommon

public	SCGetDitheredBrush
SCGetDitheredBrush:			; here we want to call rectangle
	mov	ax, yCoordinate
	mov	Y1, ax
	inc	ax			;make rect height=1, ie, Y2=Y1+1
	mov	Y2, ax
	lds	si, lpPoints
	add	si, 4
	mov	cx, Count
	dec	cx			; now cx is our loop counter
	jcxz	SCLeave 		; don't loop with cx=0
	mov	dx, es
SCDrawDitheredBrushLoop:
	lodsw
	mov	X1, ax
	lodsw
	mov	X2, ax
	push	cx
	push	dx
	push	ds
	push	si
	mov	ds, dx			; make ds=Data
	assumes ds, Data
	call	DrawInterior
	pop	si
	pop	ds
	pop	dx
	pop	cx
	loop	SCDrawDitheredBrushLoop

SCLeave:
	jmp	SCEnd			;don't do anything!

public  SCGetPen
SCGetPen:
	cmp	seg_lpPPen,0		;is there any pen?
	je	SCLeave 		;nope, this is a NOP call
	lds	si,lpPPen		;get the pen into DS:SI
	lodsw				;get the pen style in AL
					;get the pen colour in BH
	mov	bh,ah			;forground color in BH
	cmp	al,5			;is this a null pen?
	mov	al,0ffh 		;(get pattern for a solid pen)
	jne	SCCommon		;nope, assume it's solid
	jmp	SCEnd			;don't draw with a null pen

public  SCGetPatternedBrush
SCGetPatternedBrush:

;DS:SI points to bnPattern

	mov	bx, yCoordinate
	and	bx, 07h 		;get pattern index for our line
	dec	al			;set the zero flag if we have
					;a mono brush
	mov	al, [si][bx]		;get the pattern into AL
	mov	bh, BackgroundColour	;from draw mode structure
	mov	bl, ForegroundColour
	jz	SCCommon		;if zero flag set (from above),
					;it's a mono brush
;We have a colour brush.  Use the foreground/background color that came with
;the brush.

	mov	bx, word ptr [si].bnBgColor-2

public  SCCommon
SCCommon:

;At this point:
;       AL contains the pattern to set.
;       BH contains the foreground colour.
;       BL contains the background colour.

	mov	cl,al			;save the pattern to set
	MakeEmptyFIFO
	mov	dx,MODE_PORT
	mov	ax,0a040h		;set to "patterned fill" mode
	out	dx,ax
	mov	ah,80h			;this is flag for pattern 0
	mov	al,cl			;get the high 4 bits of pattern
	shr	al,3			;get them in the right place
	out	dx,ax
	mov	ah,90h			;this is flag for pattern 1
	mov	al,cl			;get the low 4 bits of pattern
	shl	al,1			;get them into the right place
	out	dx,ax

;Now set up our Y-parameters which will always be constant:

	xor	ax,ax			;Y-extent is always 1
	out	dx,ax

	mov	al, bh
	mov	dx,COLOUR_1_PORT	;set the foreground colour
	out	dx,ax
	mov	al, bl
	mov	dx,COLOUR_0_PORT	;set the background colour
	out	dx,ax

;If in transparent background mode, set Function 0 mix register to "leave
;alone"
	cmp	OpaqueFlag, 02h 	;opaque draw?
	je	SCOpaqueDraw		;yes
	mov	al, 3			;AL: "leave alone" bk mix modef
	mov	dx, FUNCTION_0_PORT
	out	dx, ax
SCOpaqueDraw:

;Set up a loop to draw "Count" number of scanlines:

	mov	cx,Count		;make CX our loop counter
	dec	cx			;correct for point containing
					;only our Y-coordinate
	lds	si,lpPoints		;DS:SI points at our point array
	add	si,4			;make DS:SI point at the 1st X coord.

public  SCLoop
SCLoop:      
	CheckFIFOSpace	FOUR_WORDS
	lodsw				;get the starting X
	mov	dx,Srcx_PORT		;and set it on board
	out	dx,ax
	mov	bx,ax			;put it in BX for extent
					;calculation
	lodsw				;get the ending X
	sub	ax,bx			;make it an extent
	dec	ax			;don't draw last point
	jl	SCLoopEnd		;if extent becomes negative,
					;don't draw
	mov	dx,RECT_WIDTH_PORT	;and set it on board
	out	dx,ax
	mov	dx,Srcy_PORT		;set the Y-coordinate
	mov	ax,yCoordinate
	out	dx,ax
	mov	dx,COMMAND_FLAG_PORT	;send the command
	mov	ax,40f3h		;this is the correct one
	out	dx,ax

public  SCLoopEnd
SCLoopEnd:
	loop	SCLoop			;and continue till done

public	SCEnd
SCEnd:                        
cEnd

cProc	HandleDrawMode, <NEAR, PUBLIC>, <ax>
cBegin	nogen
	assumes es, Data
	lds	si,lpDrawMode		;get the drawmode into DS:SI
	assumes ds, nothing
	CheckFIFOSpace	TWO_WORDS	;trashes AX and DX
	mov	ax, [si]		;get ROP->AX (can't incl gdidefs)
	or	ax, ax			;now make sure that it is within
	jz	InvalidRop2		; [1..16].  Otherwise, exit with an
	cmp	ax, 16			; error.
	ja	InvalidRop2
	dec	ax			;make it offset from 0
	mov	bx,DataOFFSET Rop2TranslateTable
	xlat	es:Rop2TranslateTable	;now AL has the proper function
	mov	bl, [si][2]		;get the opaque flag
	mov	bh, BandFlags		;BH: Write enable mask
	mov	cl, [si][8]		;get foreground colour
	mov	ch, [si][4]		;get background colour
	mov	OpaqueFlag, bl		;save opaque flag locally
	mov	ah,al			;assume an opaque draw

;At this point:
;       AL has our foreground function.
;       AH has our background function.

	or	al,20h			;use Color1 reg. for foreground color
	mov	dx,FUNCTION_1_PORT	;and set foreground function
	out	dx,ax
	mov	al,ah
	mov	dx,FUNCTION_0_PORT	;set background function
	out	dx,ax
	mov	al, cl
	and	al, bh			;strip off accelerator bits
	mov	ForegroundColour, al
	mov	al, ch
	and	al, bh
	mov	BackgroundColour, al
	clc
	ret
InvalidRop2:
	stc
	ret
cEnd	nogen

sEnd            OutputSeg
end
