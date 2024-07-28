page            ,132
title           Polylines Support for the IBM 8514
.286c

.xlist
include         CMACROS.INC
include 	8514.INC
include 	gdidefs.inc
.list

sBegin          Data
externB 	Rop2TranslateTable
externB 	WriteEnable
sEnd            Data

sBegin          Code
assumes         cs,Code
externFP	CursorExclude		;in ROUTINES.ASM
externFP	SetScreenClipFar	;in OUTPUT.ASM
sEnd            Code


subttl          Code Area
page +
createSeg       _OUTPUT,OutputSeg,word,public,CODE
sBegin          OutputSeg
assumes         cs,OutputSeg
assumes         ds,Data

externW 	DrawModeTbl		;in DRAWMODE.ASM
externB 	rot_bit_tbl		;in PIXEL.ASM


subttl          Data For Polyline Support
page +


subttl          Code for Polyline Support
page +
cProc           OutputDummy,<FAR>

include 	OUTPUT.INC		;contains stack definitions

cBegin          <nogen>     

;This routine exists so that we set up a stack frame which is correct for 
;our common Output stack frame.  It's never called but allows us to make
;near calls to Output routines.

cEnd            <nogen>


page +
cProc           Polylines,<NEAR,PUBLIC>,<ds>

cBegin

;First, set the cursor exclusion.  We want to use the clipping rectangle
;as our cursor exclusion area.  If there is no clip rectangle, then
;unconditionaly remove the cursor from the screen while the drawing is going on.
;Then, set the scissor clip.

	mov	ax,ds			;make ES = DS
	mov	es,ax
	assumes es, Data
	cmp	BandFlags,0ffh		;is this the outline of a
					;filled polygon or rectangle?
	je	PLGetPen		;yes, no need to set the clip
	cmp	seg_lpClipRect,0	;any clipping rectangle?
	jne	PLGetCursorExcludeArea	;nope, get rid of the cursor
	push	0fffeh			;set unconditional exclusion
	sub	sp,6			;correct stack for 3 parameters
	cCall	CursorExclude
	jmp	short PLGetPen		;go get the pen
public  PLGetCursorExcludeArea
PLGetCursorExcludeArea:
	lds	si,lpClipRect		;get clip rectangle into ES:DI
	lodsw
	mov	bx,ax			;get starting X
	lodsw
	mov	cx,ax			;get starting Y
	lodsw
	mov	dx,ax			;get ending X
	lodsw				;get ending Y
	arg	bx			;send these to CursorExclude
	arg	cx
	arg	dx
	arg	ax
	cCall	CursorExclude		;go exclude the cursor
	lds	si,lpClipRect		;reestablish clip rectangle
	call	SetScreenClipFar	;go set the clipping rect to
					;lpClipRect
	jnc	PLGetPen		;if valid clip rect, continue

PLLeave:
	jmp	PLEnd			;otherwise, unexclude cursor
					;and leave now
public  PLGetPen
PLGetPen:

;Next, get and set our pen (style will always be solid since the 8514 doesn't
;know how to do styled lines).

	lds	si,lpPPen		;get the pen into DS:SI
	lodsw				;now AH has the pen colour
	cmp	al,5			;is this a null pen?
	je	PLLeave 		;yes, get out now!
	mov	cx,ax			;save pen in CX
public  PLSetRop2
PLSetRop2:

;Now, set the foreground colour and pattern:

CheckFIFOSpace  THREE_WORDS
	lds	si,lpDrawMode		;get the drawmode into ES:DI
	lodsw				;get the ROP into AX
	dec	ax			;make it offset from 0
	mov	bx,DataOFFSET Rop2TranslateTable
	xlat	es:Rop2TranslateTable	;now AL has the proper function

;At this point:
;       AL has our foreground function.

	or	al,20h			;look at foreground colour
	mov	dx,FUNCTION_1_PORT	;set foreground function
	out	dx,ax
	mov	al,ch			;get colour into AX
	and	al, es:[WriteEnable]	; use only 16 colors
	mov	dx,COLOUR_1_PORT
	out	dx,ax			;set foreground colour

;Now set up the mode:

	mov	dx,MODE_PORT
	mov	ax,0a000h		;set to "no pattern" mode
	out	dx,ax
public  PLSetupLoop
PLSetupLoop:

;OK, now it's time to set up our polyline loop:

	mov	cx,Count		;make CX a point loop counter
	dec	cx			;but correct for extra point
					;(since points are connected)
;	jle	PLEnd			;if no points, get out now
	jg	short @f		;if no points, get out now
	jmp	PLEnd
@@:	lds	si,lpPoints		;DS:SI points at our point array

public  PLLoop
PLLoop:

;The 8514 line command requires a coded direction flag.  We set ourselves up
;with a default command in BX and then adjust it as needed by the direction
;and major axis of the line:

	CheckFIFOSpace	SEVEN_WORDS
	push	cx			;save our loop counter
	xor	cx,cx			;this is either 0 or -1
	mov	bx,2017h		;get default cmd into BX
	lodsw				;get starting X
	mov	X1,ax
	lodsw				;get starting Y
	mov	Y1,ax
	mov	ax,[si]			;get ending X
	mov	X2,ax
	mov	ax,[si+2]		;get ending Y
	mov	Y2,ax
	

	push	cx
	push	bx
	push	si
	cCall	ClipLinePoints
	pop	si
	pop	bx
	pop	cx
	jc	PLEndLoop

	mov	ax,X1
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	mov	di,ax			;save it in DI

;	lodsw				;get starting Y
	mov	ax,Y1

;	cmp	di,1536 		;is starting X too big?
;	jae	PLEndLoop		;yes, skip this line
;	cmp	ax,1536 		;is starting Y too big?
;	jae	PLEndLoop		;yes, skip this line

	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;	sub	di,[si] 		;now DI has X-length
	sub	di,X2	 		;now DI has X-length

	jns	PLL_1			;if not negative, go on
	or	bl,20h			;if negative, change direction
	neg	di			;negate the X-length
	dec	cx			;make CX = -1
PLL_1:                                           
;	sub	ax,[si+2]		;subtract off ending Y
	sub	ax,Y2			;subtract off ending Y

	jns	PLL_2			;if not negative, go on
	or	bl,80h			;if negative, change direction
	neg	ax			;negate the Y-length
PLL_2:

;At this point, 
;               DI has the X-length
;               AX has the Y-length
;               BX has the command to this point
;               CX contains either 0 or -1

	cmp	di,ax			;see which axis is major
	ja	PLL_3			;if X is major, go on
	or	bl,40h			;set Y-major flag in command
	xchg	ax,di			;and exchange the lengths
PLL_3:
	shl	ax,1			;send out minor axis to K1
	mov	dx,K1_PORT
	out	dx,ax

;Now calculate the error term:

	sub	ax,di			;subtract minor from major axis
	xchg	ax,cx			;get error term
	add	ax,cx
	mov	dx,ERROR_TERM_PORT	;and put it out thru port
	out	dx,ax
	xchg	ax,cx			;get back AX
	sub	ax,di			;subtract off major extent
	mov	dx,K2_PORT		;and send it out as K2
	out	dx,ax
	mov	dx,RECT_WIDTH_PORT	;send out X-extent
	mov	ax,di
	out	dx,ax
	mov	ax,bx			;get completed command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax			;and send it on out
PLEndLoop:
	pop	cx			;restore saved loop counter
	loop	PLLoop			;loop for all lines
PLEnd:
cEnd

;------------------------------------------------------------------------------
;Entry:
;  x1,y1,x2,y2
;Returns:
;  Carry set: if line is rejected.
;    otherwise: x1,y1,x2,y2: clipped accordingly.
;------------------------------------------------------------------------------
cProc	ClipLinePoints, <NEAR, PUBLIC>	;will trash SI and DI
cBegin	nogen				;it is assumed that most points will
CLP_Loop:
	mov	si,x1
	mov	di,y1
	call	OutCode			
	mov	cx,ax			;cx = outcode of x1,y1
	mov	si,x2
	mov	di,y2
	call	OutCode			;ax = outcode of x2,y2
	mov	bx,ax			;bx = outcode of x2,y2

;Check for trivial accept.
	or	ax,cx
	jz	CLP_AcceptLine

;Check for trivial reject.
	test	bx,cx			;is the line completely outside the
	jnz	CLP_RejectLine		; clip rect?  If NZ, then reject line.

;Well, we have a line that must be clipped.

;The following code assumes that (x2,y2) is inside the clip region and
;(x1,y1) is outside of the clip region.  To insure this, all we need to
;do is check (x1,y1)'s outcode.  If it is zero, then we swap points and
;outcode.  This is a valid approach because we have already done
;done trivial acceptance and rejection, so we are sure that one point
;is within the clip region and one point outside of the clip region.
	or	cx,cx
	jnz	short @f
	xchg	cx,bx
	mov	ax,x1
	xchg	ax,x2
	mov	x1,ax
	mov	ax,y1
	xchg	ax,y2
	mov	y1,ax
@@:
	mov	ax,x2
	sub	ax,x1			;ax = (x2-x1)
	mov	bx,y2
	sub	bx,y1			;bx = (y2-y1)

	rcr	cx,1
	jc	CLP_DivideAtLeft
	rcr	cx,1
	jc	CLP_DivideAtRight
	rcr	cx,1
	jc	CLP_DivideAtBottom
;	rcr	cx,1
;	jc	CLP_DivideAtTop

CLP_DivideAtTop:			;ax=(x2-x1)
	xor	dx,dx			;cx=ymin	
@@:
	mov	cx,dx
	xchg	cx,y1			;cx=y1, y1=ymin
	sub	cx,dx			;cx=(y1-ymin)
	imul	cx			;ax=(x2-x1)*(y1-ymin)
	neg	bx			;bx =(y1-y2)
	idiv	bx			;ax=(x2-x1)*(y1-ymin)/(y1-y2)
	add	x1,ax			;ax=x1+(x2-x1)*(y1-ymin)/(y1-y2)
	jmp	CLP_Loop

CLP_DivideAtBottom:			;ax=(x2-x1)
	mov	dx,Y_SIZE		;cx=ymax
	jmp	@b
if 0
	mov	cx,dx
	xchg	cx,y1			;cx=y1, y1=ymax
	sub	cx,dx			;cx=(y1-ymax)
	imul	cx			;ax=(x2-x1)*(y1-ymax)
	neg	bx			;bx =(y1-y2)
	idiv	bx			;ax=(x2-x1)*(y1-ymax)/(y1-y2)
	add	x1,ax			;ax=x1+(x2-x1)*(y1-ymax)/(y1-y2)
	jmp	CLP_Loop
endif


CLP_DivideAtLeft:
	xor	dx,dx			;cx=xmin	
@@:
	xchg	ax,bx			;ax=(y2-y1)
	mov	cx,dx
	xchg	cx,x1			;cx=x1, x1=xmin
	sub	dx,cx			;dx=(xmin-x1)
	imul	dx			;ax=(y2-y1)*(xmin-x1)
	idiv	bx			;ax=(y2-y1)*(xmin-x1)/(x2-x1)
	add	y1,ax			;y1=y1+(y2-y1)*(xmin-x1)/(x2-x1)
	jmp	CLP_Loop

CLP_DivideAtRight:
	mov	dx,X_SIZE		;cx=xmax
	jmp	@b
if 0	
	xchg	ax,bx			;ax=(y2-y1)
	mov	cx,dx
	xchg	cx,x1			;cx=x1, x1=xmax
	sub	dx,cx			;dx=(xmax-x1)
	imul	dx			;ax=(y2-y1)*(xmax-x1)
	idiv	bx			;ax=(y2-y1)*(xmax-x1)/(x2-x1)
	add	y1,ax			;y1=y1+(y2-y1)*(xmax-x1)/(x2-x1)
	jmp	CLP_Loop
endif


CLP_AcceptLine:
	clc
	ret

CLP_RejectLine:
	stc
	ret
cEnd	nogen



;------------------------------------------------------------------------------
;Entry:
;  si,di: x,y
;Returns:
;  ax = outcode
;Destroys:
;  ax,bx,dx
;------------------------------------------------------------------------------
OutCode	proc	near
	xor	ax,ax
;y-ymin
	mov	bx,di		
;	sub	bx,0
	shl	bx,1
	rcl	ax,1

;ymax-y
	mov	dx,Y_SIZE
	sub	dx,di
	shl	dx,1
	rcl	ax,1

;xmax-x
	mov	dx,X_SIZE
	sub	dx,si
	shl	dx,1
	rcl	ax,1

;x-xmin
	mov	bx,si		
;	sub	bx,0
	shl	bx,1
	rcl	ax,1

	ret

OutCode	endp
	

sEnd            OutputSeg
end
