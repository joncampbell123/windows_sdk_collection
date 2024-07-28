page            ,132
title           Routines for Setting States on the IBM 8514
.286c


.xlist
include         CMACROS.INC
include         WINDEFS.INC
include         8514.INC
.list


subttl  Static & External Data Definitions -- DATA SEGMENT
page +
sBegin          Data
externB         DRAW_BUSY_FLAG                  ;in MOVE.ASM
externW         CURSOR_X_COORDINATE             ;in MOVE.ASM
externW         CURSOR_Y_COORDINATE             ;in MOVE.ASM
externW         CURSOR_X_SIZE                   ;in SET.ASM
externW         CURSOR_Y_SIZE                   ;in SET.ASM
externW         UNDONE_X_COORDINATE             ;in MOVE.ASM
externW         UNDONE_Y_COORDINATE             ;in MOVE.ASM
externW         X_HOT_SPOT                      ;in SET.ASM
externW         Y_HOT_SPOT                      ;in SET.ASM
externB         CURSOR_MOVED_FLAG               ;in MOVE.ASM

globalB 	CURSOR_EXCLUDE_FLAG,0ffh,1	;FF HEX = CURSOR IS OFF!
externB 	WriteEnable

sEnd		Data

externA CURSOR_DRAW_BUSY


subttl  External Declarations -- Code Segment
page +
sBegin          Code
assumes         cs,Code
assumes         ds,Data


externNP        ENABLE_CURSOR                   ;in SET.ASM
externNP        DISABLE_CURSOR                  ;in SET.ASM
externW         _cstods                         ;in BITBLT.ASM

public  SetScreenClipFar
SetScreenClipFar    proc    far
	call	SetScreenClip			;call SET_STRING_CLIP
	ret					;and do a far return to
                                                ;OutputSeg
SetScreenClipFar    endp


subttl          Set Clipping Rectangle
page +
cProc		SetScreenClip,<NEAR,PUBLIC>, <si, di>

;Entry:
;	DS:SI points to clipping rectangle
;Exit:
;       Carry flag set if we are to abort BLT

cBegin                                   

;Note that Windows's definition of the RECT data structure causes the clip
;rectangle passed in to have its ending X & Y coordinates 1 greater than
;they're actually supposed to be.
;
;First, do some verification on the coordinates passed in:

	lodsw				;get starting X into DI
	mov	di,ax			;
	lodsw				;get starting Y into CX
	mov	cx,ax			;
	lodsw				;get ending X into BX
	dec	ax			;(decrement ending coordinates)
	mov	bx,ax			;
	lodsw				;get ending Y into SI
	dec	ax			;(decrement ending coordinates)
	mov	si,ax			;
	cmp	di,bx			;is starting X >= ending X?
	jg	SSCDontDraw		;yes, don't draw anything!
	cmp	cx,si			;is starting Y >= ending Y?
	jg	SSCDontDraw		;yes, don't draw anything!

SSCSetClipOnBoard:
;
;At this point:
;       DI contains starting X.
;       CX contains starting Y.
;       BX contains (corrected) ending X.
;       SI contains (corrected) ending Y.

	CheckFIFOSpace	FOUR_WORDS
	mov	dx,CLIP_PORT		;set up to do the clip
	mov	ax,di			;get the starting X
	or	ax,2000h		;set the X-minimum bit
	out	dx,ax			;set the starting X
	mov	ax,cx			;get the starting Y
	or	ax,1000h		;set the Y-minimum bit
	out	dx,ax			;set the starting Y
	mov	ax,bx			;get the ending X
	or	ax,4000h		;set the X-maximum bit
	out	dx,ax			;set the ending X
	mov	ax,si			;get the ending Y
	or	ax,3000h		;set the Y-maximum bit
	out	dx,ax			;set the ending Y
	clc				;clear the carry flag
	jmp	short SSCExit		;we're done!

public  SSCDontDraw
SSCDontDraw:
	stc				;set the carry flag

public  SSCExit
SSCExit:
cEnd


subttl  Cursor Exclusion Routines
page +
cProc	CursorExclude,<FAR,PUBLIC,NODATA>
        parmW   LowX
        parmW   LowY
        parmW   HighX
        parmW   HighY
cBegin

;If LowX = FFFFH, then don't exclude the cursor.  Just don't allow it to move.
;If LowX = FFFEH, then unconditionally exclude the cursor.
;We assume that the data segment is pointed to by DS.

	pusha

;Firstly, tell MoveCursor that he is not to do any more updates to the cursor
;position until the entire draw operation is done!

	mov	ds,cs:_cstods			;establish pointer to DS
	cmp	byte ptr CURSOR_EXCLUDE_FLAG,0ffh
						;is cursor already excluded?
	je	CE_DONE 			;yep, don't do it again!
;	 EnterCrit
	or	[DRAW_BUSY_FLAG], CURSOR_DRAW_BUSY
;	 LeaveCrit a				 ;don't let cursor move
	cmp	LowX,0ffffh			;are we just shutting off the
                                                ;cursor and not excluding it?
	je	CE_NO_EXCLUDE			;yes, go do this "exclusion"
	cmp	LowX,0fffeh			;do we want to unconditionally
						;exclude the cursor?
	je	CE_EXCLUDE_CURSOR		;yes, go do it

;Next, get the last coordinates that the cursor was at when we told MoveCursor
;to shut off.

	mov	cx,CURSOR_X_COORDINATE		;get cursor position
	sub	cx,X_HOT_SPOT			;subtract out the hot spot
	mov	dx,CURSOR_Y_COORDINATE
	sub	dx,Y_HOT_SPOT

;Now find out if the cursor is in the exclude rectangle.

	mov	ax,LowX 			;get the low X coordinate
	mov	bx,CURSOR_X_SIZE		;get width (= height) of cursor
	sub	ax,bx				;add this width onto rectangle
	cmp	cx,ax				;cursor needs exclusion?
	jl	CE_NO_EXCLUDE			;nope, get out
	mov	ax,LowY 			;get Y-origin into AX
	sub	ax,bx				;add height onto rectangle
	cmp	dx,ax				;cursor needs exclusion?
	jl	CE_NO_EXCLUDE			;nope, get out
	cmp	cx,HighX			;cursor needs exclusion?
	jg	CE_NO_EXCLUDE			;nope, get out
	cmp	dx,HighY			;cursor needs exclusion?
	jg	CE_NO_EXCLUDE			;nope, get out

;We've determined that the cursor must be taken off the screen during this
;draw operation (and be put back AFTER the draw is complete).  Set the
;CURSOR_EXCLUDE_FLAG and turn off the cursor.

CE_EXCLUDE_CURSOR:
	mov	byte ptr CURSOR_EXCLUDE_FLAG,0ffh
						;tell 'em cursor's excluded
	call	DISABLE_CURSOR			;and get rid of the cursor
	jmp	short CE_DONE			;and go finish up

;The cursor does not have to be excluded.  Clear the CURSOR_EXCLUDE_FLAG
;but do not remove the cursor from the screen.

CE_NO_EXCLUDE:
	mov	byte ptr CURSOR_EXCLUDE_FLAG,0	;tell 'em cursor's not excluded

CE_DONE:                                                    
	popa
cEnd


page
cProc	CursorUnExclude,<FAR,PUBLIC,NODATA>
cBegin                                         
	pusha					;save the world
	mov	ds,cs:_cstods			;establish pointer to DS
	cmp	byte ptr CURSOR_MOVED_FLAG,0ffh ;has the cursor moved since
						;we excluded it?
	jne	CU_ENABLE_CURSOR		;nope, see if it was excluded
	call	DISABLE_CURSOR			;go disable the cursor from its
						;old position (if needed)
	mov	ax,UNDONE_X_COORDINATE		;update to new position in X
	mov	CURSOR_X_COORDINATE,ax
	mov	ax,UNDONE_Y_COORDINATE		;update to new position in Y
	mov	CURSOR_Y_COORDINATE,ax
	mov	byte ptr CURSOR_MOVED_FLAG,0	;reset the cursor moved flag
	jmp	short CU_EC_1			;go reenable it in the new place

CU_ENABLE_CURSOR:
	cmp	byte ptr CURSOR_EXCLUDE_FLAG,0ffh
						;was cursor excluded during
						;this draw operation?
	jne	CU_ALLOW_CURSOR 		;nope, simply allow it to move

CU_EC_1:
	call	ENABLE_CURSOR			;go reenable the cursor at
                                                ;the position where it was
                                                ;at when we did the CURSOR
                                                ;EXCLUDE

CU_ALLOW_CURSOR:
;	 EnterCrit				 ;don't allow interrupts
	and	DRAW_BUSY_FLAG, 0fdh		;allow cursor to move again

; We assume here that CURSOR_DRAW_BUSY is equal to 2

	mov	byte ptr CURSOR_EXCLUDE_FLAG,0	;reset our flag
;	 LeaveCrit				 ;allow interrupts again
	popa					;restore saved registers
cEnd
;

subttl  Mono Bitmap Definition Routines
page +          
public  DRAW_MONO_BITMAP
DRAW_MONO_BITMAP                proc    near

;Entry:
;       AX contains the X-coordinate of where to put our bitmap.
;       BX contains foreground function.
;       CX contains background function.              
;       DI contains the extent of our square bitmap.
;	DS:SI points at our bitmap bits.
;	ES Data

	push	ax			;save our bitmap X-coordinate
	MakeEmptyFIFO
	pop	ax			;restore saved X-coordinate
	mov	dx,Srcx_PORT		;set the X-coordinate
	out	dx,ax
	mov	ax,Y_SIZE		;also set up the Y-coordinate as first
					;line of invisible memory
	mov	dx,Srcy_PORT
	out	dx,ax
	mov	ax,di			;this is height and width of our bitmap
	dec	ax			;(decremented by 1 as per manual)
	mov	dx,RECT_HEIGHT_PORT	;set it on the board
	out	dx,ax
	mov	dx,RECT_WIDTH_PORT	;set it on the board
	out	dx,ax

;Now, set the mode to "user defined pattern":

	mov	ax,USER_DEFINED_PATTERN_MODE
	mov	dx,MODE_PORT
	out	dx,ax

;Next, set the FUNCTION 0 & 1 registers

	mov	ax,cx			;set appropriate function
	mov	dx,FUNCTION_0_PORT	;background
	out	dx,ax
	mov	ax,bx
	mov	dx,FUNCTION_1_PORT	;and foreground
	out	dx,ax

;Next, set the write-enable register:

	mov	ax,01h			;we save our bitmaps in plane 0
	mov	dx,WRITE_ENABLE_PORT
	out	dx,ax

;Now we send the command to define a user-supplied pattern to the board.  Then
;we'll proceed to extract the bitmap's pattern from the bitmap in DS:SI.

	CheckFIFOSpace	ONE_WORD
	mov	ax,41b1h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

public  DMC_DRAW_BITMAP
DMC_DRAW_BITMAP:
	mov	cx,di			;get the extent of the square bitmap
	mov	bx,cx			;copy into BX for calculating bytes-per
					;line
	shr	bx,3			;now BX has nbr of bytes per line
	mov	dx,PATTERN_DEFINE_PORT	;set up DX for pattern definition

public  DMC_LINE_LOOP
DMC_LINE_LOOP:

;Next, set up a loop to do all the lines in the character

	push	cx			;save the line loop counter
	mov	cx,bx			;get nbr of bytes per line

public  DMC_BYTE_LOOP
DMC_BYTE_LOOP:
	push	cx			;save our byte loop counter
	lodsb				;get next byte from source
	mov	ah,al			;into AH
	mov	cx,8			;there are 8 bits per byte

public  DMC_BL_BIT_LOOP
DMC_BL_BIT_LOOP:
	xor	al,al			;AL will be our pattern definer
	rcl	ah,1			;get next bit into carry
	sbb	al,0			;set AL to either 0 or FF depending on
					;state of carry flag after rotate
	out	dx,ax			;put it out
	loop	DMC_BL_BIT_LOOP 	;loop 8 times
	pop	cx			;restore saved full byte loop counter
	loop	DMC_BYTE_LOOP

;We're now done processing one scanline of this bitmap.  Loop to do the
;next scanline:

	pop	cx			;restore saved line-loop counter
	loop	DMC_LINE_LOOP		;and go do the next line
	CheckFIFOSpace	ONE_WORD
	mov	al, es:[WriteEnable]	;now reset write enable to all planes
	mov	dx,WRITE_ENABLE_PORT
	out	dx,ax
	ret				;and we're done drawing

DRAW_MONO_BITMAP    endp

sEnd	Code
end
