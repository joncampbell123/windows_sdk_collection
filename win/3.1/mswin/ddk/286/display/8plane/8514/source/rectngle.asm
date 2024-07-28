page            ,132
title           Rectangle Support for the IBM 8514 Series
.286c


.xlist
include         CMACROS.INC
include 	8514.INC
.list

CLIPPED_LEFT	equ 1
CLIPPED_TOP	equ 2
CLIPPED_RIGHT	equ 4
CLIPPED_BOTTOM	equ 8

CLIPPED_LOW	equ 2
CLIPPED_HIGH	equ 8

public	ForgroundMix
public	BackgroundMix


subttl          Data Area
page +
sBegin          Data
externB 	Rop2TranslateTable	;in INIT.ASM
ForgroundMix	db  ?
BackgroundMix	db  ?
externW     FullScreenClip
sEnd            Data


sBegin          Code
assumes cs,Code
externNP	HandleBrush		;in BRUSH.ASM
externFP	CursorExclude		;in ROUTINES.ASM
externFP	SetScreenClipFar	;in POLYGON.ASM

public  HandleBrushFar
HandleBrushFar	proc	far
	pop	bx			;get and save return address
	pop	cx
	call	HandleBrush		;call HandleBrush without
					;without screwing up the stack
	push	cx			;put the return address back
	push	bx			;so return will work
	ret				;and do a far return to
					;OutputSeg
HandleBrushFar	endp

sEnd            Code


subttl          Code Area
page +         
createSeg       _OUTPUT,OutputSeg,word,public,CODE
sBegin          OutputSeg
assumes         cs,OutputSeg
assumes         ds,Data                                       


externNP	Polylines		;in POLYLINE.ASM

subttl          Code for Rectangle Support
page +
cProc           OutputDummy,<FAR>

include 	OUTPUT.INC		;contains stack definitions

cBegin          <nogen>     

;This routine exists so that we set up a stack frame which is correct for 
;our common Output stack frame.  It's never called but allows us to make
;near calls to Output routines.

cEnd            <nogen>


page +
cProc           Rectangle,<NEAR,PUBLIC>

cBegin

;The first thing we want to do is draw the rectangle's interior with our brush
;using the already set ROP2. If no brush exists, we simply skip the fill 
;and draw the outline with the passed pen.
;
;First, set the cursor exclusion.  We want to use the clipping rectangle
;as our cursor exclusion area.

	sub	ax, ax			;assume there is no pen
	cmp	seg_lpPPen, 0
	jz	RTNoPen
	les	di, lpPPen
	cmp	es:[di], al		;is it a solid pen?
	jne	RTNoPen 		;no.  Treat it as if there was no pen
	inc	ax
RTNoPen:
	mov	PenPattern, ax		;this is our pen present/not pres. flag
	push	ds			;save our DS (= Data)
	push	ss
	pop	es			;always create a local clipping rect.
	lea	di, RectPoints
	cmp	seg_lpClipRect,0	;any clipping rectangle passed?
	jne	RTUseClipRect		;yes, go use it
	push	0fffeh			;otherwise, unconditionally
	sub	sp,6			;exclude the cursor
	cCall	CursorExclude
	sub	ax, ax			;if there is no clipping rectangle,
	mov	X1, ax			;make the clip region the whole screen
	stosw
	mov	Y1, ax
	stosw
	mov	ax, X_SIZE
	mov	X2, ax
	stosw
	mov	ax, Y_SIZE
	mov	Y2, ax
	stosw
	jmp	short RTGetCoordinates	;and continue

RTLeave:                                         
	pop	ds			;restore saved DS
	jmp	RTExit

public  RTUseClipRect
RTUseClipRect:
	lds	si,lpClipRect		;get pointer to rectangle
	lodsw				;get starting X
	mov	dx,ax
	mov	X1, ax
	stosw
	lodsw				;get starting Y
	mov	cx,ax
	mov	Y1, ax
	stosw
	lodsw				;get ending X
	mov	bx,ax
	mov	X2, ax
	stosw
	lodsw				;get ending Y
	mov	Y2, ax
	stosw
	arg	dx			;send these to CursorExclude
	arg	cx
	arg	bx
	arg	ax
	cCall	CursorExclude		;go exclude the cursor
;	 lds	 si,lpClipRect		 ;reestablish clipping rect
;	 call	 SetScreenClipFar	 ;go set the clipping rect to
					;lpClipRect
;	 jc	 RTLeave		 ;if error returned, unexclude
					;cursor and leave now

public  RTGetCoordinates
RTGetCoordinates:
	lds	si,lpPoints		;get point array into DS:SI
	sub	di, di			;di is our clipping flag accumulator
	mov	bx, X1			;get lowest X into bx
	mov	cx, X2			;get highest X into cx
	cmp	cx, bx			;and make sure the limits are valid
	jb	RTLeave
	call	SetLimits		;fetches low and high Operand and clips
	jc	RTLeave 		;if carry set, don't draw anything
	mov	X1, dx
	mov	X2, ax
	shr	di, 1			;postion flags properly
	mov	bx, Y1			;get lower limit for Y
	mov	cx, Y2			;get upper limit for Y
	cmp	cx, bx
	jb	RTLeave
	call	SetLimits
	jc	RTLeave
	mov	Y1, dx
	mov	Y2, ax

public  RTCheckBrush
RTCheckBrush:
	cmp	seg_lpPBrush, 0
	je	RTDrawOutline
	lds	si,lpPBrush		;get the brush into ES:SI
	lodsb				;get brush style (if there's
					;one there!)
;	 mov	 bl,al
;	 mov	 ax,ds			 ;is there a brush?
;	 or	 ax,si
	pop	ds			;(restore saved DS)
;	 jz	 RTDrawOutline		 ;nope, skip the interior fill
					;and just go draw the outline

public  RTDrawInterior
RTDrawInterior:
	dec	al			;is the brush hollow?
	jz	RTDrawOutline		;yes, go draw the outline
	push	di
	call	DrawInterior
	pop	di

subttl          Rectangle Outline Draw Routine
page +       
public  RTDrawOutline
RTDrawOutline:
;	 cmp	 seg_lpPPen,0		 ;any pen?
	cmp	PenPattern, 0
	je	RTExit			;nope, get out
	mov	BandFlags,0ffh		;tell polyline that we've
					;already excluded the cursor
					;and set the clip
	push	ds			;save our data segment selector
	push	ss
	pop	ds
	lea	si, RectPoints		;use the hardware scissors
	call	SetScreenClipFar	;call clip routine
	pop	ds			;restore data selctor
	mov	dx,X1
	mov	cx,Y1
	mov	bx,X2
;	 dec	 bx			 ;ending coordinates in RECT
					;data structure must be dec'd
	mov	ax,Y2
;	 dec	 ax			 ;ending coordinates in RECT
					;must be decremented
	sub	dx, PenPattern
	sub	cx, PenPattern
	shr	di, 1			;if a line is to be clipped away, just
	sbb	dx, 0			;place its coordinates outside the
	shr	di, 1			;clip region
	sbb	cx, 0
	shr	di, 1
	adc	bx, 0
	shr	di, 1
	adc	ax, 0

;We will call polylines to draw our rectangle for us.  We need to make a new
;"local" point array so that we can do a closed figure.

	mov	Count,5 		;put in our phony count
	mov	di,ss			;make ES = SS
	mov	es,di
	lea	di,ss:RectPoints	;now ES:DI points at our new
					;point array
	mov	off_lpPoints,di 	;set this pointer in
	mov	seg_lpPoints,ss
	xchg	ax,dx			;get starting X
	stosw
	xchg	ax,cx			;get starting Y
	stosw				;(end of point 1)
	xchg	ax,bx			;get ending X
	stosw
	xchg	ax,bx			;get starting Y
	stosw				;(end of point 2)
	xchg	ax,bx			;get ending X
	stosw
	xchg	ax,dx			;get ending Y
	stosw				;(end of point 3)
	xchg	ax,cx			;get starting X
	stosw
	xchg	ax,cx			;get ending Y
	stosw				;(end of point 4)
	mov	ax,cx			;get starting X
	stosw
	mov	ax,bx			;get starting Y
	stosw				;(whew! end of point 5)
	cCall	Polylines		;go do the outline
	lea	si, FullScreenClip
	call	SetScreenClipFar

RTExit:
cEnd

public	DrawInterior
DrawInterior	proc near
	CheckFIFOSpace	TWO_WORDS
	les	di,lpDrawMode		;get the drawmode into ES:DI
	mov	al,es:[di+0]		;get the ROP into AL
	dec	ax			;make it offset from 0
	mov	bx,DataOFFSET Rop2TranslateTable
	xlat	Rop2TranslateTable	;now AL has the proper function
	mov	ah,al			;assume an opaque draw
	mov	bl,es:[di+2]		;get the opaque flag
	mov	OpaqueFlag,bl		;save it in case someone needs
					;it
	rcr	bl,1			;get the 1's bit into carry
	jnc	RTSetRop2		;if no carry, we're opaque
	mov	ah,03h			;if transparent, set background
					;function to "leave alone"

public  RTSetRop2
RTSetRop2:

;At this point:
;       AL has our foreground function.
;       AH has our background function.

	mov	ForgroundMix, al
	or	al,20h			;it's a user defined pattern
	mov	dx,FUNCTION_1_PORT	;set foreground function
	out	dx,ax
	mov	al,ah
	mov	BackgroundMix, al
	mov	dx,FUNCTION_0_PORT	;set background function
	out	dx,ax

public  RTCallHandleBrush
RTCallHandleBrush:
	mov	bx,X2
	mov	dx, X1
	sub	bx, dx			;get the X-extent
;	 dec	 bx
	jle	short @f 		;if zero or less, get out
	mov	si,Y2
	mov	cx, Y1
	sub	si, cx			;get the Y-extent
;	 dec	 si
	jle	short @f 		;if zero or less, get out
	mov	al,0ffh 		;tell 'em not to reset the function
	arg	lpPBrush
	arg	es			;this is lpDrawMode in ES:DI
	arg	di
	arg	dx			;this is DstxOrg
	arg	cx			;this is DstyOrg
	arg	bx			;this is xExt
	arg	si			;this is yExt
	arg	dx			;pattern X origin
	arg	cx			;pattern Y origin
	cCall	HandleBrushFar		;go do the work
@@:	ret

DrawInterior	endp

SetLimits   proc near
	lodsw				;get the starting X into AX
;	 inc	 ax			 ;add pen width (always 1)
	add	ax, PenPattern
	or	ax, ax
	js	RT1Negative
RT1Low:
	cmp	ax, bx
	jnb	RT1LowOkay
	mov	ax, bx
	or	di, CLIPPED_LOW
RT1LowOkay:
	cmp	ax, cx
	ja	RT2Negative		;error exit if low X to right of clip
;	 mov	 ax, cx
;	 or	 di, CLIPPED_HIGH
;RT1HighOkay:
	mov	dx, ax
	mov	ax, [si+2]		;get ending coordinate
	dec	ax			;subtract pen width at bottom right
	or	ax, ax
	js	RT2Negative
	cmp	ax, bx			;if ending X is below the lower
	jb	RT2Negative		;limit for X, then exit
	cmp	ax, cx
	jna	RT2HighOkay
	mov	ax, cx
	or	di, CLIPPED_HIGH
RT2HighOkay:
	clc
	ret
RT1Negative:
	sub	ax, ax
	or	di, CLIPPED_LOW
	jmp	short RT1Low
RT2Negative:
	stc				;ending X must be positive to draw
	ret
SetLimits   endp


sEnd            OutputSeg
end
