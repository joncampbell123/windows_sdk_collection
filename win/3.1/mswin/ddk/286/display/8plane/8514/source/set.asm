page	,132
title	Produce the IBM 8514 Cursor
.286c

.xlist
include CMACROS.INC
include WINDEFS.INC
include 8514.INC
.list

subttl	Data Area for Set Cursor Routines
page +
sBegin	Data
externW CURSOR_X_COORDINATE		;in MOVE.ASM
externW CURSOR_Y_COORDINATE		;in MOVE.ASM
externW UNDONE_X_COORDINATE		;in MOVE.ASM
externW UNDONE_Y_COORDINATE		;in MOVE.ASM
externB CURSOR_MOVED_FLAG		;in MOVE.ASM
externB DRAW_BUSY_FLAG			;in MOVE.ASM
externW FreeSpaceyOrg			;in IBMDRV.ASM
externW FreeSpaceyExt			;in IBMDRV.ASM
externB WriteEnable

globalB CURSOR_ENABLED_FLAG,0ffh,1
globalW CURSOR_X_SIZE,0,1
globalW CURSOR_Y_SIZE,0,1
globalW X_HOT_SPOT,0,1
globalW Y_HOT_SPOT,0,1

sEnd	Data

public	CURSOR_ON_VACATION
public	CURSOR_DISABLED
public	CURSOR_DRAW_BUSY

CURSOR_DISABLED     equ 1
CURSOR_DRAW_BUSY    equ 2
CURSOR_ON_VACATION  equ CURSOR_DISABLED or CURSOR_DRAW_BUSY


subttl  The SetCursor Code
page +
sBegin	Code
assumes cs,Code
assumes ds,Data


externNP        DRAW_MONO_BITMAP                ;in ROUTINES.ASM


cProc	SetCursor, <PUBLIC, FAR>, <si, di>

        parmD   lpCursorShape

cBegin
	EnterCrit			;disallow interrupts
	sub	ax, ax			;in case of lpCursorShape == 0
	cmp	ax, seg_lpCursorShape	;set the CURSOR_DISABLED flag
	cmc				;in addition to the
	sbb	al, al			;CUROSR_DRAW_BUSY flag.
	and	al, CURSOR_DISABLED
	add	al, CURSOR_DRAW_BUSY
	mov	DRAW_BUSY_FLAG, al	;tell MoveCursor that it can't
					;do any position updates
	LeaveCrit a			;reallow interrupts

SC_ERASE_CURSOR:    
	call	DISABLE_CURSOR		;go disable the cursor
	cmp	seg_lpCursorShape,0	;is this a null pointer?
	je	SC_END			;yep, we're done

SC_DRAW_A_CURSOR:
	push	ds				;save DS = Data
	mov	ax,ds				;make ES --> Data
	mov	es,ax				;
	lds	si,lpCursorShape		;DS:SI points at our cursor
	lodsw					;get the X-hotspot
	mov	es:X_HOT_SPOT,ax		;save the hot spot value
	lodsw					;and the Y-hotspot too!
	mov	es:Y_HOT_SPOT,ax		;
	lodsw					;get the size in X
	mov	es:CURSOR_X_SIZE,ax		;save it
	lodsw					;get the size in Y
	mov	es:CURSOR_Y_SIZE,ax		;save it
	mov	di,ax				;this is size of our cursor
	add	si,4				;point at the cursor bits
	mov	ax,CURSOR_AND_INVISIBLE_X	;pass the X-coordinate
	mov	bx,2				;foreground colour is white (1)
	mov	cx,1				;background colour is black (0)
	call	DRAW_MONO_BITMAP		;go draw the AND bitmap

;Upon return, DS:SI will point at the XOR cursor bitmap!

	mov	ax,CURSOR_XOR_INVISIBLE_X	;pass the X-coordinate
	mov	bx,1				;foreground colour is black (0)
	mov	cx,2				;background colour is white (1)
	call	DRAW_MONO_BITMAP		;go draw the XOR bitmap
	pop	ds				;restore saved DS (--> Data)

;Now we have both components in their invisible areas, enable the cursor...
	cmp	CURSOR_MOVED_FLAG,0ffh
	jne	short @f
	mov	ax,UNDONE_X_COORDINATE
	mov	CURSOR_X_COORDINATE,ax
	mov	ax,UNDONE_Y_COORDINATE
	mov	CURSOR_Y_COORDINATE,ax
	mov	CURSOR_MOVED_FLAG,0
@@:	call	ENABLE_CURSOR			;go reenable the cursor

;Now we must manage our memory.  We reserve a 64 by 64 block for the cursor.
;If this area is already reserved, don't goof with it.  If it isn't, we'd
;best allocate it to ourselves:

	cmp	FreeSpaceyOrg,Y_SIZE+64 	;have we allocated free space?
	jae	SC_END				;yep, we're OK
	mov	FreeSpaceyOrg,Y_SIZE+64 	;otherwise, allocate some space
	sub	FreeSpaceyExt,64

SC_END:                                          
	EnterCrit a				;disallow interrupts
	and	byte ptr DRAW_BUSY_FLAG, not CURSOR_DRAW_BUSY	;MoveCursor can
                                                ;now do positional updates
	LeaveCrit a				;reallow interrupts
	mov	ax,1				;return success code
cEnd     


subttl          General Cursor Enable/Disable Routines
page +
public  ENABLE_CURSOR
ENABLE_CURSOR                   proc    near

;We assume that DS points to our general data segment.

	cmp	byte ptr CURSOR_ENABLED_FLAG,0ffh
						;is cursor already enabled?
	je	EC_EXIT 			;yep! get out now!
	test	DRAW_BUSY_FLAG, CURSOR_DISABLED ;don't draw the cursor if its
	jnz	EC_EXIT 			;disabled!

ENC_1:
	xor	di,di				;tell 'em to enable the cursor
	cCall	BLOCK_MOVE_CURSOR_AREA		;go do the work
	jc	EC_EXIT 			;if no cursor size, get out
	mov	byte ptr CURSOR_ENABLED_FLAG,0ffh
                                                ;tell ourselves that cursor
                                                ;is enabled

public  EC_DRAW_CURSOR
EC_DRAW_CURSOR:

;       Since the 8514 colour scheme for foreground and background colours
;are different from MS-WINDOWS, we must use a NAND (NOT SOURCE AND DEST)
;ROP to put our AND cursor component out instead of an AND ROP.

	mov	bl,0eh				;set foreground & background
						;functions to NAND opaque
	mov	di,CURSOR_AND_INVISIBLE_X	;this is source coordinate
	call	EXPAND_CURSOR			;
	mov	bl,05h				;set foreground & background
						;functions to XOR opaque
	mov	di,CURSOR_XOR_INVISIBLE_X	;this is source coordinate
	call	EXPAND_CURSOR			;

EC_EXIT:                                                
	ret					;and return to caller

ENABLE_CURSOR                   endp


page
public  DISABLE_CURSOR
DISABLE_CURSOR	proc near
	cmp	byte ptr CURSOR_ENABLED_FLAG,0	;is cursor already disabled?
	je	DC_EXIT 			;yep! get out now!
	mov	di,1				;tell 'em to disable our cursor
	cCall	BLOCK_MOVE_CURSOR_AREA		;go do the work
	mov	byte ptr CURSOR_ENABLED_FLAG,0	;tell ourselves that cursor
						;is not enabled
DC_EXIT:                                         
	ret					;and return to caller

DISABLE_CURSOR                  endp


page +                                      
public  BLOCK_MOVE_CURSOR_AREA
BLOCK_MOVE_CURSOR_AREA          proc    near

;Entry:
;       DI contains mode (0 = enable cursor, 1 = disable cursor).
;       DS points to data segment.
;Exit:
;       CX will contain the decremented, clipped Y-size of the cursor.
;       Carry set if cursor should not be drawn.
;
;We'd best see if the cursor is going to violate line 768.  It is possible
;for the cursor to do this because our clip window is usually set to include
;all 1024 lines.  If line 768 is violated, we must cut the Y-extent down so
;that it won't destroy the font cache which we keep at line 768!

	mov	cx,CURSOR_Y_SIZE	;get the Y-extent
	mov	bx,CURSOR_Y_COORDINATE	;get the Y-coordinate
	sub	bx,Y_HOT_SPOT		;correct for the hot spot
	mov	dx,Y_SIZE-1		;this is last line we can
					;draw the cursor onto
	add	bx,cx			;get the sum of the two
	sub	dx,bx			;get -(clipped cursor size)
	jns	BMCA_GET_X_SIZE 	;if result is positive,
					;(or zero), go get the X-size
	add	cx,dx			;otherwise, get new cursor size
					;into CX
	jle	BMCA_LEAVE		;if new cursor size is 0 or
					;negative, just get out

public  BMCA_GET_X_SIZE
BMCA_GET_X_SIZE:                  
	mov	bx,CURSOR_X_SIZE	;get size of the cursor

;Since the board requires the extent to be decremented, we might as well
;do it now and test the sign bit to test for a zero extent cursor.

	dec	bx			;if no cursor size,
	js	BMCA_LEAVE		;get the heck out
	dec	cx			;same for the Y-size
	jmp	short BMCA_MOVE_CURSOR	;continue

public	BMCA_LEAVE
BMCA_LEAVE:
	stc				;set the error flag
	jmp	BMCA_EXIT		;get on out

public  BMCA_MOVE_CURSOR
BMCA_MOVE_CURSOR:                        

;Now, set up the plane enable appropriately:

	MakeEmptyFIFO
	mov	al, WriteEnable 	;set to read and write to all planes
	rol	al, 1
	mov	dx,READ_ENABLE_PORT
	out	dx,ax

public  BMCA_SET_FUNCTION
BMCA_SET_FUNCTION:

;Now set to function 'replace':

	mov	ax,67h			;set to replace, block move mode
	mov	dx,FUNCTION_0_PORT	;set the background
	out	dx,ax
	mov	dx,FUNCTION_1_PORT
	out	dx,ax

public  BMCA_SET_MODE
BMCA_SET_MODE:

;Now, set the mode to "block move pattern":

	mov	ax,0a000h
	mov	dx,MODE_PORT
	out	dx,ax

public  BMCA_SET_EXTENTS
BMCA_SET_EXTENTS:

;At this point:
;       BX has X-extent of cursor.
;       CX has Y-extent of cursor.
;
;Now set the X & Y extents of the cursor:

	mov	ax,bx			;get the X-extent
	mov	dx,RECT_WIDTH_PORT	;set the width of the character on board
	out	dx,ax
	mov	ax,cx			;get the Y-extent
	mov	dx,RECT_HEIGHT_PORT	;set it on the board
	out	dx,ax

;Now check the enable-disable flag to see what we're doing:

	push	cx			;save the clipped Y-size of cursor
	or	di,di			;are we saving the screen area?
	jz	BMCA_SAVE_SCREEN	;yes, go set up for saving the screen

public  BMCA_RESTORE_SCREEN
BMCA_RESTORE_SCREEN:
	mov	ax,CURSOR_SAVE_X	;this is SrcxOrg
	mov	bx,CURSOR_SAVE_Y	;this is SrcyOrg
	mov	cx,CURSOR_X_COORDINATE	;this is DstxOrg
	sub	cx,X_HOT_SPOT		;correct for HotSpot
	and	cx,7ffh 		;if CX was negative, make it into
					;an outrageous positive number
	mov	di,CURSOR_Y_COORDINATE	;this is DstyOrg
	sub	di,Y_HOT_SPOT		;correct for HotSpot
	and	di,7ffh 		;in DI was negative, make it into
                                        ;an outrageous positive number
	jmp	short BMCA_COMMON	;go do common processing

public  BMCA_SAVE_SCREEN
BMCA_SAVE_SCREEN:
	mov	ax,CURSOR_X_COORDINATE	;this is SrcxOrg
	sub	ax,X_HOT_SPOT		;correct for HotSpot
	and	ax,7ffh 		;if AX was negative, make it into
					;an outrageous positive number
	mov	bx,CURSOR_Y_COORDINATE	;this is SrcyOrg
	sub	bx,Y_HOT_SPOT		;correct for HotSpot
	and	bx,7ffh 		;if BX was negative, make it into
					;an outrageous positive number
	mov	cx,CURSOR_SAVE_X	;this is DstxOrg
	mov	di,CURSOR_SAVE_Y	;this is DstyOrg

public  BMCA_COMMON
BMCA_COMMON:
	mov	dx,Srcx_PORT		;set the SrcxOrg
	out	dx,ax
	mov	dx,Srcy_PORT		;set the SrcyOrg
	mov	ax,bx
	out	dx,ax
	CheckFIFOSpace	THREE_WORDS
	mov	dx,Dstx_PORT		;set the DstxOrg
	mov	ax,cx
	out	dx,ax
	mov	dx,Dsty_PORT		;set the DstyOrg
	mov	ax,di
	out	dx,ax

;Now we send the block move command:

	mov	ax,0c0b2h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax
	pop	cx			;restore saved clipped cursor Y-size
	clc				;clear the error flag

BMCA_EXIT:                    
	ret
BLOCK_MOVE_CURSOR_AREA          endp


page
public  EXPAND_CURSOR
EXPAND_CURSOR	proc	near

;Entry:   
;       BL must contain the function (always opaque).
;       CX must contain the clipped yExt (decremented) of the cursor.
;       DI must contain the starting X-coordinate of cursor component.
;
;Now, set the foreground and background colours:

	MakeEmptyFIFO
	xor	al,al			;set the foreground colour to black
	mov	dx,COLOUR_1_PORT
	out	dx,ax
	dec	al			;set the background colour to white
	mov	dx,COLOUR_0_PORT
	out	dx,ax

;Next, set up the plane enable appropriately:

	mov	al,02h			;our source data is on plane 7!
	mov	dx,READ_ENABLE_PORT
	out	dx,ax

public  EXC_SET_FUNCTION
EXC_SET_FUNCTION:

;Now set the appropriate "function":

	mov	al,bl			;get the passed function
	mov	dx,FUNCTION_0_PORT	;set the background
	out	dx,ax
	or	al,20h			;set the foreground
	mov	dx,FUNCTION_1_PORT
	out	dx,ax

public  EXC_SET_MODE
EXC_SET_MODE:

;Now, set the mode to "block move pattern":

	mov	ax,BLOCK_MOVE_MODE
	mov	dx,MODE_PORT
	out	dx,ax

;Now set the X & Y coordinates on the screen:

	mov	ax,CURSOR_X_COORDINATE	;get the X-origin of our destination
	sub	ax,X_HOT_SPOT		;subtract out the X-hot-spot
	and	ax,7ffh 		;if starting X is negative, this'll
					;make it into a clipped positive nbr
	mov	dx,Dstx_PORT
	out	dx,ax
	mov	ax,CURSOR_Y_COORDINATE	;get the Y-origin of our destination
	sub	ax,Y_HOT_SPOT		;subtract out the Y-hot-spot
	and	ax,7ffh 		;if starting Y is negative, this'll
					;make it into a clipped positive nbr
	mov	dx,Dsty_PORT
	out	dx,ax

;Set the width and height of the cursor:

	CheckFIFOSpace	FIVE_WORDS
	mov	ax,CURSOR_X_SIZE	;this is the width of cursor
	dec	ax			;as per manual
	mov	dx,RECT_WIDTH_PORT	;set the width of the cursor on board
	out	dx,ax
	mov	ax,cx			;this is corrected size of the cursor
	mov	dx,RECT_HEIGHT_PORT	;set it on the board
	out	dx,ax

;Now set up the source X & Y as the spot in invisible memory where we're BLTing
;from:

	mov	ax,di			;get passed in X-location
	mov	dx,Srcx_PORT		;get it onto board
	out	dx,ax
	mov	ax,CURSOR_SAVE_Y
	mov	dx,Srcy_PORT		;get it onto board
	out	dx,ax

;Now we send the block move command to put our cursor where we want it
;on the displayable screen.

	mov	ax,0c0b3h		;this is the appropriate command
	mov	dx,COMMAND_FLAG_PORT
	out	dx,ax

EXC_EXIT:
ret                                     ;and return to caller

EXPAND_CURSOR	endp


sEnd            Code
end
