page    ,132
title           Move the IBM 8514 Cursor to Requested Location


.xlist
include         CMACROS.INC
include         WINDEFS.INC
include         8514.INC
.list


page +
sBegin	Data
globalW CURSOR_X_COORDINATE,0,1
globalW CURSOR_Y_COORDINATE,0,1
globalW UNDONE_X_COORDINATE,0,1
globalW UNDONE_Y_COORDINATE,0,1
globalB CURSOR_MOVED_FLAG,0,1
globalB DRAW_BUSY_FLAG,0,1
sEnd	Data


page +
sBegin	Code
assumes cs,Code
assumes ds,Data


externNP    DISABLE_CURSOR		;in SET.ASM
externNP    ENABLE_CURSOR		;in SET.ASM

externA     CURSOR_ON_VACATION		;in SET.ASM
externA     CURSOR_DRAW_BUSY		;in SET.ASM

page +

cProc	MoveCursor, <PUBLIC, FAR>, <si, di>

        Parmw   absX
        Parmw   absY

cBegin
	EnterCrit			;disable interrupts
	mov	cl, CURSOR_DRAW_BUSY	;set draw busy and get current
	xchg	cl,DRAW_BUSY_FLAG	;status into CL
	mov	ax,absX 		;get X-coordinate
	mov	bx,absY 		;get Y-coordinate
	LeaveCrit d			;reenable the interrupts
	or	ax,ax			;exceeding low X-limit?
	jg	MC_CHECK_HIGH_X_LIMIT	;nope, go on
	xor	ax,ax			;fix cursor position to 0

MC_CHECK_HIGH_X_LIMIT:
	cmp	ax,X_SIZE		;exceeding high X-limit?
	jb	MC_CHECK_LOW_Y_LIMIT	;nope, go on
	mov	ax,X_SIZE-1		;fix cursor position

MC_CHECK_LOW_Y_LIMIT:
	or	bx,bx			;exceeding low Y-limit?
	jg	MC_CHECK_HIGH_Y_LIMIT	;nope, go on
	mov	bx,1			;fix cursor position

MC_CHECK_HIGH_Y_LIMIT:
	cmp	bx,Y_SIZE		;exceeding high Y-limit?
	jb	MC_MOVE_CURSOR		;nope, go do the cursor
	mov	bx,Y_SIZE-1		;fix cursor position

MC_MOVE_CURSOR:                  
	test	cl, CURSOR_ON_VACATION	;is board busy with something?
	jnz	DONT_MOVE_CURSOR	;yep, don't you dare do anything
	push	cx
	push	ax			;save the registers that we need
	push	bx
	call	DISABLE_CURSOR		;go get rid of old cursor
	pop	bx			;restore saved coordinates
	pop	ax
	mov	CURSOR_X_COORDINATE,ax	;save this location
	mov	CURSOR_Y_COORDINATE,bx
	call	ENABLE_CURSOR		;and turn on the new!
	pop	cx			;restore saved DRAW_BUSY_FLAG
	jmp	short MOVE_CURSOR_EXIT	;and get out

DONT_MOVE_CURSOR:
	mov	byte ptr CURSOR_MOVED_FLAG,0ffh
					;indicate that the cusor was moved
	mov	UNDONE_X_COORDINATE,ax	;save this location
	mov	UNDONE_Y_COORDINATE,bx

MOVE_CURSOR_EXIT:                                                       
	EnterCrit			;disallow interrupts
	xchg	cl,DRAW_BUSY_FLAG	;restore the original state of
					;the draw busy flag
	LeaveCrit a			;reallow interrupts
cEnd

sEnd    code
end
