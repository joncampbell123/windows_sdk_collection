	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	CURSOR.ASM
;
; This file contains the pointer shape routines required to draw the
; pointer shape on the EGA.
;
; Created: 23-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1983-1987 Microsoft Corporation
;
; Exported Functions:	none
;
; Public Functions:	move_cursors
;			draw_cursor
;			cursor_off
;
; Public Data:		x_cell
;			y_cell
;			cur_cursor
;			inquire_data
;			real_width
;			CUR_HEIGHT
;			CUR_ROUND_LEFT
;			CUR_ROUND_RIGHT
;			INIT_CURSOR_X
;			INIT_CURSOR_Y
;
;			EGAMem:current_brush
;			EGAMem:known_word
;			EGAMem:shadowed_mode
;			EGAMem:saved_latches
;			EGAMem:plane_index
;			EGAMem:enable_test
;			EGAMem:tonys_bar_n_grill
;
; General Description:
;
;   All display drivers must support a "cursor" for the pointing
;   device.  The cursor is a small graphics image which is allowed
;   to move around the screen independently of all other operations
;   to the screen, and is normally bound to the location of the
;   pointing device.  The cursor is non-destructive in nature, i.e.
;   the bits underneath the cursor image are not destroyed by its
;   presence.
;
;   A cursor consists of an AND mask and an XOR mask, which give
;   possible pixel colors of 0 (black), 1 (white), display, or
;   inverse display.
;
;		    AND XOR | DISPLAY
;		    ---------------------
;		     0	 0  |	  0
;		     0	 1  |	  1
;		     1	 0  |	Display
;		     1	 1  | Not Display
;
;   The cursor also has a "hot spot", which is the pixel of the
;   cursor image which is to be aligned with the actual pointing
;   device location.
;
;
;		  |	    For a cursor like this, the hot spot
;		  |	    would normally be the *, which would
;	       ---*---	    be aligned with the pointing device
;		  |	    position
;		  |
;
;   The cursor may be moved to any location on the screen, be
;   restricted to only a section of the screen, or be made invisible.
;   Part of the cursor image may be past the edge of the screen, and
;   in such a case only the visible part is displayed.
;
;
;
;   Logically, the cursor image isn't part of the physical display
;   surface.  When a drawing operation coincides with the cursor
;   image, the result is the same as if the cursor image wasn't
;   there.  In reality, if the cursor image is part of the display
;   surface it must be removed from memory before the drawing
;   operation occurs, and redrawn afterwards.
;
;   Exclusion of the cursor image is the responsibility of the
;   display driver.  Each output operation must decide whether
;   or not to remove the cursor from display memory, and, if yes,
;   to set a protection rectangle wherein the cursor must not be
;   displayed.  The cursor image drawing routine honors this
;   protection rectangle.
;
;
;
;   To reduce the amount of perceived flicker of the cursor,
;   a buffering scheme has been implemented where the cursor
;   update is performed off-screen.
;
;   To do this, a couple of buffers are maintained.  One buffer
;   contains the contents of the screen in an area around where
;   the cursor will go, and the other saves the contents of this
;   buffer where the actual cursor is to be drawn.  The region
;   of the screen where the cursor goes is read into the buffer,
;   the old cursor removed by copying the contents of the save
;   area over the old cursor, the area under the new cursor is
;   saved, and the new cursor written into the buffer.	The
;   buffer is then written back to the screen.	This has the
;   advantage that removal of the old cursor and writing of the
;   new happen at the same time on the screen.
;
;   Since the buffer is of a fixed size, it must be determined
;   if both the old and new cursors fit within. If they do not
;   both fit within the buffer, the old cursor is removed from
;   the screen by copying the save area directly to the screen.
;   The drawing of the cursor then proceeds normally, except
;   that there is no old cursor to remove from the buffer.
;
;   This code doesn't distinguish between cursors and icons,
;   They both are the same size, 32 x 32 pixels.
;
; Restrictions:
;
;   All routines herein assume protection either via cli/sti
;   or a semephore at higher level code.
;
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include windefs.inc
	include cursor.inc		;Device specific constats
	include ega.inc
	include macros.mac
	include egamemd.inc
	.list


	??_out	cursor


	externA ScreenSelector		;Segment of screen
	externNP enable_switching	;Allow screen group switching
	externNP disable_switching	;Disallow screen group switching


	public	move_cursors
	public	draw_cursor
	public	cursor_off

	page
;	The SMALL_ROTATE flag conditionally assembles optimum code for
;	rotating cursor masks one or two bit positions.  This however
;	costs approximately 90 bytes of code space
;
;		0 = do not assemble the small rotate code
;		1 = do	   assemble the small rotate code

SMALL_ROTATE	 equ	 1


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg

;	inquire_data contains information about mouse acceleration
;	which the window manager uses.

inquire_data	CURSORINFO   <X_RATE,Y_RATE>

sEnd	BlueMoonSeg



sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

	public	x_cell			;Make all of these value available
	public	y_cell			;  to the other cursor routines
	public	cur_cursor
	public	inquire_data
	public	real_width
	public	CUR_HEIGHT
	public	CUR_ROUND_LEFT
	public	CUR_ROUND_RIGHT
	public	INIT_CURSOR_X
	public	INIT_CURSOR_Y



;	cur_cursor contains the cursor data structure (less the
;	actual bits) for the current cursor shape.

cur_cursor	cursorShape <,,,,,>



;	old_valid contains a flag which is used to indicate
;	whether or not the contents of the cursor save area
;	contains valid data.

old_valid	db	0		;True if old cursor contains valid data
OLD_IS_INVALID	equ	0		;  No cursor in save area
OLD_IS_VALID	equ	1		;  Cursor in save area



;	(x_cell,y_cell) is the location of the cursor on the screen.
;	These locations are only updated whenever a cursor is drawn.

x_cell		dw	0
y_cell		dw	0



;	rotation maintains the number of bits the cursor masks have
;	been rotated.  This value is always between 0 and 7

rotation	db	0



;	old_x_cell and old_y_cell contain the (X,Y) on the
;	screen where the contents of the save_area maps to.
;	These cells are only valid if old_valid = OLD_IS_VALID

old_x_cell	dw	0
old_y_cell	dw	0



;	x_buffer and y_buffer contain the (X,Y) of the upper left
;	hand corner of the screen locations which were copied into
;	screen_buf.  screen_pointer contains the offset in display
;	memory of (x_buffer, y_buffer).


x_buffer	dw	0
y_buffer	dw	0
screen_pointer	dw	0



;	buf_height contains the number of scans of valid information
;	in screen_buf.	This is set to the height of a cursor/icon
;	plus the overlap of the old and new cursor/icon.  Thus is
;	there is only 1 pixel difference in Y, only CUR_HEIGHT+1
;	scans must be read processed.

buf_height	dw	MAX_BUF_HEIGHT



;	vc_buf_1 and vc_buf_2 are intermediate locations used
;	by copy_buffer_to_screen.  They contains the actual
;	number of bytes and scans which must be copied to the
;	screen (remember, we clip the cursor).

vc_buf_1	dw	0		;Bytes per line
vc_buf_2	dw	0		;Buffer height



;	real_width contains the width in bits of the currently
;	selected cursor/icon.  This is a holdover from the days
;	when cursors and icon were different widths.  It is used
;	by exclude_test for hit testing.

real_width	dw	CUR_ICON_WIDTH*8



;	The following are the masks which make up the cursor image.

cur_and_mask	db	MASK_LENGTH dup (?)
cur_xor_mask	db	MASK_LENGTH dup (?)

sEnd	Data


sBegin	Code
	assumes cs,Code
	externNP save_hw_regs
	externNP res_hw_regs
sEnd	Code

	page
sBegin	Code
	assumes cs,Code

;--------------------------Public-Routine-------------------------------;
; move_cursors
;
;   Move AND and XOR cursor masks
;
;   The AND and XOR cursor masks are stored in the cursor work areas.
;
; Entry:
;	DS:DI --> AND mask segment
;	ES     =  Data segment
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,ES,BP
; Registers Destroyed:
;	AX,CX,DI
; Calls:
;	none
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing		;DS is pointing to user data
	assumes es,Data 		;ES was set up by SetCursor


move_cursors	proc	near

	cld
	mov	di,DataOFFSET cur_and_mask
	mov	al,0ffh 		;Set implicit part of AND mask
	call	move_cursors_10 	    ;Move AND mask
	mov	di,DataOFFSET cur_xor_mask
	xor	al,al			;Set implicit part of XOR mask (0)
	mov	rotation,al		;Show mask isn't rotated

move_cursors_10:
	mov	cx,CUR_HEIGHT/2 	;Set height for move
	errnz	<CUR_HEIGHT and 1>	;  (Must be even)

move_cursors_20:
	movsw				;Move explicit part
	movsw				;Move explicit part
	stosb				;Move implicit part
	movsw
	movsw
	stosb
	errnz	SAVE_WIDTH-5

	loop	move_cursors_20
	ret

move_cursors	endp

	page
;--------------------------Public-Routine-------------------------------;
; draw_cursor
;
;   Draw a cursor based at x_cell, y_cell
;
;   The currently defined cursor/icon is drawn.  If the old
;   cursor/icon is currently on the screen, it is removed.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	enable_switching
;	disable_switching
;	save_hw_regs
;	res_hw_regs
;	erase_old_cursor
;	copy_buffer_to_save
;	rotate_masks
;	put_cursor_in_buffer
;	copy_buffer_to_screen
; History:
;	Sun 20-Sep-1987 19:41:38 -by-  [*****], *** ****** [******]
;	OS/2 3.x box compatibility support.  Don't allow screen group
;	switch while we have state saved for the EGA (we can only
;	save one state at a time).
;
;	Tue 18-Aug-1987 14:36:59 -by-  **** ***** [*****]
;	Added test of the disabled flag.
;
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing

draw_cursor	proc	near

	test	enabled_flag,0FFh	;Cannot output if display has
	jz	exit_draw_cursor	;  been disabled
	call	disable_switching
	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	save_hw_regs		;Save EGA registers


;	Set the EGA up for copying bytes on the EGA.  This will be
;	done using M_PROC_WRITE, M_DATA_READ, DR_SET, and setting
;	GRAF_BIT_MASK to 0.  This simulates Write Mode 1.  All of
;	the above except GRAF_BIT_MASK was set by save_hw_regs.
;
;	All planes were also enabled for writing by save_hw_regs.


	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out16	dx,ax			;Disable all bits

	cld				;This is interrupt code, do this!
	push	bp			;Must save this
	call	erase_old_cursor	;Erase old cursor and setup for new
	call	copy_buffer_to_save	;Save area under new cursor
	call	rotate_masks		;Rotate cursor masks into place
	call	put_cursor_in_buffer	;Generate new cursor in local buffer


;	Set the EGA up for copying bytes on the EGA.  This will be
;	done using M_PROC_WRITE, M_DATA_READ, DR_SET, and setting
;	GRAF_BIT_MASK to 0.  This simulates Write Mode 1.
;
;	Both GRAF_BIT_MASK and GRAF_DATA_ROT may have been altered
;	by the rotation code, so they must be set correctly before
;	we can copy the buffer to the screen.


	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out16	dx,ax			;Disable all bits
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out	dx,ax
	call	copy_buffer_to_screen	;Copy new cursor to screen (and
	pop	bp			;  possibly restore old cursor area)
	mov	ax,ScreenSelector	;res_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	res_hw_regs		;Restore user's state
	call	enable_switching

exit_draw_cursor:
	ret

draw_cursor	endp
	page

;--------------------------Public-Routine-------------------------------;
; cursor_off
;
;   Remove Cursor From Screen
;
;   The old cursor is removed from the screen if it currently
;   is on the screen.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	enable_switching
;	disable_switching
;	save_hw_regs
;	res_hw_regs
;	copy_save_to_screen
; History:
;	Sun 20-Sep-1987 19:41:38 -by-  [*****], *** ****** [******]
;	OS/2 3.x box compatibility support.  Don't allow screen group
;	switch while we have state saved for the EGA (we can only
;	save one state at a time).
;
;	Tue 18-Aug-1987 14:36:59 -by-  **** ***** [*****]
;	Added test of the disabled flag.
;
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


cursor_off	proc	near

	test	enabled_flag,0FFh	;Cannot output if display has
	jz	cursor_off_end		;  been disabled
	cmp	old_valid,OLD_IS_VALID	;Does old screen contain valid image?
	jne	cursor_off_end		;  No, nothing to do
	mov	old_valid,OLD_IS_INVALID;  Yes, show screen restored
	call	disable_switching

	mov	ax,ScreenSelector	;save_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	save_hw_regs		;Save EGA registers


;	Set the EGA up for copying bytes on the EGA.  This will be
;	done using M_PROC_WRITE, M_DATA_READ, DR_SET, and setting
;	GRAF_BIT_MASK to 0.  This simulates Write Mode 1.  All of
;	the above except GRAF_BIT_MASK was set by save_hw_regs.
;
;	All planes were also enabled for writing by save_hw_regs.


	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,GRAF_BIT_MASK
	out16	dx,ax			;Disable all bits

	cld
	push	bp
	call	copy_save_to_screen
	pop	bp

	mov	ax,ScreenSelector	;res_hw_regs requires this
	mov	es,ax
	assumes es,EGAMem

	call	res_hw_regs		;Restore user's state
	call	enable_switching

cursor_off_end:
	ret

cursor_off	endp
	page

;--------------------------Private-Routine------------------------------;
; copy_save_to_screen
;
;   The contents of the save area (which contains the bits saved
;   from underneath the cursor or icon) are placed on the screen
;   where they came from.
;
; Entry:
;	DS = Data Segment
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	compute_screen_pointer
;	buf_to_screen_10  (jumps to it)
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

copy_save_to_screen proc near

	mov	di,offset save_area	;--> bits to restore to screen
	mov	ax,old_x_cell		;Get screen coordinates of upper
	mov	si,old_y_cell		;  left bit
	push	si
	push	ax

	call	compute_screen_pointer	;Compute address on screen
	xchg	si,di
	mov	ax,SCAN_BYTES-SAVE_WIDTH;Get axis update values
	mov	cx,SAVE_WIDTH		;Get width of save area
	mov	bp,CUR_HEIGHT		;Set maximum to move (entire save buf)
	jmp	buf_to_screen_10

copy_save_to_screen endp
	page
;--------------------------Private-Routine------------------------------;
; put_cursor_in_buffer
;
;   The current cursor/icon is ANDed and XORed into the
;   current local buffer.
;
; Entry:
;	DS = Data Segment
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	map_xy
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


put_cursor_in_buffer proc near

	mov	ax,x_cell		;Get screen coordinate of upper left
	mov	si,y_cell		;  bit and compute its address in
	call	map_xy			;  the local buffer
;	mov	cx,CUR_HEIGHT		;(done by map_xy)
	mov	di,si			;--> local buffer
	mov	ax,ScreenSelector
	mov	es,ax
	assumes es,EGAMem


;	Set the EGA for AND mode.  Everything is set correctly except
;	GRAF_BIT_MASK and GRAF_DATA_ROT.


	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_AND shl 8 + GRAF_DATA_ROT
	out16	dx,ax

	mov	ax,0FF00h + GRAF_BIT_MASK
	out	dx,ax

	mov	bx,di			;Save local buffer pointer
	mov	si,DataOFFSET cur_and_mask
	call	put_cursor_in_buffer_10 ;Process AND mask

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_XOR shl 8 + GRAF_DATA_ROT
	out16	dx,ax

	mov	di,bx			;--> local buffer
	mov	si,DataOFFSET cur_xor_mask
	mov	cx,CUR_HEIGHT		;Process XOR mask


	assumes ds,Data
	assumes es,EGAMem

put_cursor_in_buffer_10:
	cld
	mov	dx,BUF_WIDTH-SAVE_WIDTH ;Set buffer scan increment

put_cursor_in_buffer_20:
	mov	al,es:[di]		;Load latches
	movsb				;Perform logic operation with screen
	mov	al,es:[di]
	movsb
	mov	al,es:[di]
	movsb
	mov	al,es:[di]
	movsb
	mov	al,es:[di]
	movsb
	add	di,dx			;--> next buffer location
	loop	put_cursor_in_buffer_20
	errnz	SAVE_WIDTH-5

a_return:
	ret

put_cursor_in_buffer endp
	page
;--------------------------Private-Routine------------------------------;
; rotate_masks
;
;   The cursor/icon masks are rotated to be aligned for the
;   new (x,y).	The rotate is performed as a single-bit shift
;   of the entire mask.
;
; Entry:
;	DS = Data
;	direction flag cleared
; Returns:
;	direction flag cleared
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


rotate_masks	proc near

	mov	ax,ds			;Set up ES for string instructions
	mov	es,ax
	assumes es,Data

	mov	bl,00000111B
	mov	al,bptr x_cell		;Get d0..d2 of new X coordinate
	and	al,bl
	and	bl,rotation		;Get d0..d2 of current rotation
	mov	rotation,al		;Save new rotation index
	sub	al,bl			;Compute delta rotate
	jz	a_return		;  Mask is already aligned
	jl	rot_cur_left		;  New < old, rotate left

	mov	bh,al			;Save rotate count
	mov	si,DataOFFSET cur_and_mask
	call	rot_right
	mov	al,bh
	mov	si,DataOFFSET cur_xor_mask


rot_right:
if	SMALL_ROTATE


;	Rotate the given mask right (al) bits.	If the rotate is more than
;	two bits, use the fast code.


	cmp	al,3			;Use the big rotate code?
	jnc	rot_right_big		;  Yes, it will be faster
	mov	bl,al			;Save rotate count

rot_right_10:
	mov	di,si			;Set pointers
	mov	cx,MASK_LENGTH/2	;Set # words to rotate
	errnz	<MASK_LENGTH and 1>	;Must be a multiple of 2 for words

rot_right_20:
	mov	ax,wptr [di]		;Rotate this word
	xchg	ah,al
	rcr	ax,1
	xchg	ah,al
	stosw				;Store new word and update pointer
	loop	rot_right_20


;	Now finish the rotate by setting the first bit in the first byte
;	to the bit shifted out of the last byte.  I believe (I know!)
;	that a couple of rotates will handle this quite nicely.


	mov	al,[si]
	rcl	al,1			;Remove unwanted bit, add in wanted bit
	ror	al,1			;Put wanted bit in D7 where it belongs
	mov	[si],al 		;Store new byte
	dec	bl			;More to rotate?
	jnz	rot_right_10		;  Yes, rotate them
	ret

endif	;SMALL_ROTATE



;	The big rotate rotates 'n' bits at a time.  Time will be saved
;	using this code for rotates of over two bits.


rot_right_big:
	mov	cl,al			;Set rotate count
	mov	di,si			;Set pointers
	mov	dx,MASK_LENGTH		;Set # bytes to rotate
	xor	bl,bl			;Zero initial previous bits

rot_right_big_10:
	xor	ax,ax			;Zero LSB for the shift
	mov	ah,bptr [di]		;Rotate this byte
	shr	ax,cl			;Rotate as needed
	or	ah,bl			;Get previous unused bits
	mov	bl,al			;Save new unused bits
	mov	al,ah			;Store new byte
	stosb				;  and update pointers
	dec	dx			;Loop until entire mask rotated
	jnz	rot_right_big_10


;	Now finish the rotate by setting the first byte's high
;	order bits to the bits shifted out of the last byte.


	or	bptr [si],bl
	ret




;	Rotate the given mask left (al) bits.  If the rotate is
;	more than two bits, use the fast code.


rot_cur_left:
	neg	al			;Mask shift count positive
	mov	bh,al			;Save rotate count
	mov	si,DataOFFSET cur_and_mask
	call	rot_left
	mov	al,bh
	mov	si,DataOFFSET cur_xor_mask


rot_left:
	std				;Decrement for rotating left

if	SMALL_ROTATE

	cmp	al,3			;Use the big rotate code?
	jnc	rot_left_big		;  Yes, it will be faster
	mov	bl,al			;Save rotate count

rot_left_10:
	mov	di,si			;Set pointers
	mov	cx,MASK_LENGTH		;Get # bytes in mask
	add	di,cx			;--> last word
	sub	di,2
	mov	dx,di			;Will need this later
	shr	cx,1			;Compute # words to move
	errnz	<MASK_LENGTH and 1>	;Must be a multiple of 2 for words

rot_left_20:
	mov	ax,wptr [di]		;Rotate this word
	xchg	ah,al
	rcl	ax,1
	xchg	ah,al
	stosw				;Store new word and update pointer
	loop	rot_left_20


;	Now finish the rotate by setting the last bit in the last byte
;	to the bit shifted out of the first byte.


	mov	di,dx			;Get pointer to last word
	mov	al,[di][1]		;  (want the last BYTE)
	rcr	al,1			;Remove unwanted bit, add in wanted bit
	rol	al,1			;Put wanted bit in D0 where it belongs
	mov	[di][1],al		;Store new byte
	dec	bl			;More to rotate?
	jnz	rot_left_10		;  Yes, rotate them
	cld				;Clear for other folks
	ret

endif	;SMALL_ROTATE



;	The big rotate rotates 'n' bits at a time.  Time will be saved
;	using this code for rotates of over two bits.


rot_left_big:
	mov	cl,al			;Set rotate count
	mov	dx,MASK_LENGTH		;Get # bytes in mask
	add	si,dx			;--> last word
	dec	si
	mov	di,si			;Will need this later
	xor	bl,bl			;Zero initial previous bits

rot_left_big_10:
	xor	ax,ax			;Zero MSB for the shift
	mov	al,bptr [di]		;Rotate this byte
	shl	ax,cl			;Rotate as needed
	or	al,bl			;Get previous unused bits
	mov	bl,ah			;Save new unused bits
	stosb				;Store byte and update pointer
	dec	dx			;Loop until entire mask rotated
	jnz	rot_left_big_10


;	Now finish the rotate by setting the last byte's low
;	order bits to the bits shifted out of the first byte.


	or	bptr [si],bl
	cld
	ret

rotate_masks	endp
	page
;--------------------------Private-Routine------------------------------;
; copy_buffer_to_save
;
;   The contents of the local buffer where the cursor or
;   icon will go is saved in the save area.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	map_xy
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


copy_buffer_to_save proc near

	mov	di,offset save_area	;--> where the old bits are to go
	mov	ax,x_cell		;Save the screen (x,y) of the
	mov	si,y_cell		;  upper left bit of the bits
	mov	old_x_cell,ax
	mov	old_y_cell,si
	mov	old_valid,OLD_IS_VALID
	call	map_xy
;	mov	cx,CUR_HEIGHT		;map_xy set cx = CUR_HEIGHT
	shr	cx,1
	errnz	<CUR_HEIGHT and 1>
	mov	dx,BUF_WIDTH-SAVE_WIDTH ;Set source skip count
	mov	bx,ds			;Save DS
	mov	ax,ScreenSelector
	mov	es,ax
	mov	ds,ax
	assumes ds,EGAMem
	assumes es,EGAMem

copy_buffer_to_save_10:
	movsb
	movsb
	movsb
	movsb
	movsb
	add	si,dx			;--> next source byte
	movsb
	movsb
	movsb
	movsb
	movsb
	add	si,dx
	errnz	SAVE_WIDTH-5

	loop	copy_buffer_to_save_10
	mov	ds,bx			;Restore data segment
	assumes ds,Data

	ret

copy_buffer_to_save endp
	page
;--------------------------Private-Routine------------------------------;
; copy_save_to_buf
;
;   The contents of the save area is copied into the local buffer,
;   removing the cursor.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	map_xy
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing

copy_save_to_buf proc near

	mov	di,offset save_area
	mov	ax,old_x_cell		;Get starting coordinate of the
	mov	si,old_y_cell		;  saved bits
	call	map_xy
;	mov	cx,CUR_HEIGHT		;map_xy set cx = CUR_HEIGHT
	shr	cx,1
	errnz	<CUR_HEIGHT and 1>
	mov	dx,BUF_WIDTH-SAVE_WIDTH ;Set source skip count
	xchg	si,di
	mov	bx,ds			;Save DS
	mov	ax,ScreenSelector
	mov	es,ax
	mov	ds,ax
	assumes ds,EGAMem
	assumes es,EGAMem

copy_save_to_buf_10:
	movsb
	movsb
	movsb
	movsb
	movsb
	add	di,dx
	movsb
	movsb
	movsb
	movsb
	movsb
	add	di,dx
	errnz	SAVE_WIDTH-5

	loop	copy_save_to_buf_10
	mov	ds,bx			;Restore data segment
	assumes ds,Data

	ret

copy_save_to_buf endp
	page
;--------------------------Private-Routine------------------------------;
; map_xy
;
;   Map X,Y into the local buffer
;
;   The given screen (x,y) coordinate is mapped to a pointer
;   into the local buffer.
;
; Entry:
;	AX = screen x coordinate
;	SI = screen y coordinate
;	DS = Data
; Returns:
;	CX = default cursor height
;	SI = pointer into the local buffer
; Error Returns:
;	No error return.
; Registers Preserved:
;	BX,DX,DI,BP,ES,DS
; Registers Destroyed:
;	AX,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


map_xy	proc	near

	sub	ax,x_buffer		;Compute byte difference of
	sar	ax,1			;  buffer and cursor
	sar	ax,1
	sar	ax,1

	sub	si,y_buffer
	mov	cx,si
	shiftl	si,3			;*8
	add	si,cx			;si = y*9
	errnz	BUF_WIDTH-9		;Must be 9 bytes wide

	add	si,ax			;Add byte offset
	add	si,offset screen_buf	;Point to screen buffer
	mov	cx,CUR_HEIGHT		;Set cursor height
	ret

map_xy	endp
	page
;--------------------------Private-Routine------------------------------;
; copy_buffer_to_screen
;
;   The contents of the local buffer is copied to the screen.
;   The contents are clipped to the screen as needed.
;   If the image is entirly off the screen, then no copy is
;   performed and the contents of the save area are invalidated.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


copy_buffer_to_screen proc near

	mov	ax,SCAN_INC		;Prepare for the copy
	mov	si,offset screen_buf	;--> source
	mov	di,screen_pointer	;--> destination
	mov	cx,BUF_WIDTH		;Get width of the buffer
	mov	bp,buf_height		;Set height of valid data in buffer
	push	y_buffer		;Set (x,y) of upper left hand corner
	push	x_buffer		;  of the buffer

buf_to_screen_10:
	mov	dx,cx			;Save width of buffer
	mov	vc_buf_1,cx
	mov	vc_buf_2,bp


;	Check the left hand side of the image to see if clipping must
;	be done.  If it must be done, then clip by adjusting the starting
;	address and the byte count.


	xor	bp,bp			;Zero clipped horizontal bytes
	pop	cx			;Get x-coordinate
	ashiftr cx,3			;/8 for byte address
	or	cx,cx			;Is the left side off the screen?
	jge	buf_to_screen_20	;  No,
	neg	cx			;  Yes, clip whatever is needed
	add	di,cx			;    by biasing the start of the
	add	si,cx			;    buffer and screen addresses
	jmp	short buf_to_screen_30	;    and the byte count


;	The left side of the buffer will fit onto the screen without being
;	clipped.  Now see if the right hand side will fit without being
;	clipped.  If it won't, then clip as needed.


buf_to_screen_20:
	add	cx,vc_buf_1		;Add in width of the cursor/icon
	sub	cx,(SCREEN_WIDTH+7)/8	;Clip the right side of the image?
	jle	buf_to_screen_40	;  No, it fits

buf_to_screen_30:
	mov	bp,cx			;Save amount clipped
	add	ax,cx			;Bias the scanline increments
	sub	dx,cx			;Sub clipped count from width
;	jle	buf_to_screen_130	;Nothing shows, exit and remove Y
	jg	buf_to_screen_40	;  from the stack
	jmp	buf_to_screen_130
	errn$	buf_to_screen_40


;	Check the top of the image to see if clipping must de done.
;	If it must be done, then clip by adjusting the starting
;	address and the scanline count.  Since the display is inter-
;	laced, the scanline increments must also be updated.


buf_to_screen_40:
	pop	cx			;Get y coordinate
	or	cx,cx			;Is it on the screen?
	jge	buf_to_screen_80	;  Yes, check bottom

buf_to_screen_50:
	neg	cx			;Compute excess to be clipped
	mov	bx,cx			;Save amount clipped

buf_to_screen_60:
	add	si,vc_buf_1		;Update buffer start
	add	di,ax			;Update screen start   (scanline inc)
	add	di,dx			;Update screen start   (width inc)
	loop	buf_to_screen_60
	mov	cx,bx			;Get amount clipped
	jmp	short buf_to_screen_90	;Update scanline count



;	Check the bottom of the image to see if clipping must de done.
;	If it must be done, then clip by adjusting the scan line count.


buf_to_screen_80:
	add	cx,vc_buf_2		;Add in height of data in buffer
	sub	cx,SCREEN_HEIGHT	;Does it fit on the screen?
	jle	buf_to_screen_100	;  Yes, great

buf_to_screen_90:
	sub	vc_buf_2,cx		;Clip vertical count
	jle	buf_to_screen_140	;  Nothing shows, exit


;	Now copy the clipped region of the buffer to the screen
;
;	Currently:
;		AX	 =  next scanline increment
;		DX	 =  width of move in bytes
;		vc_buf_2 =  # of scanlines to move
;		ES:DI	--> destination on screen
;		DS:SI	--> source in local buffer


buf_to_screen_100:
	mov	dh,bptr vc_buf_2	;Place # scanlines in here
	push	ds			;Save this guy
	mov	cx,ScreenSelector	;Set segment address of REGEN RAM
	mov	es,cx
	mov	ds,cx
	assumes ds,EGAMem
	assumes es,EGAMem

	xor	ch,ch			;Set msb of width

buf_to_screen_110:
	mov	cl,dl			;Set width of one scanline
	rep	movsb			;Move this scan line to screen
	add	di,ax			;--> next scanline
	add	si,bp			;--> next source
	dec	dh			;Any more scanlines to process?
	jnz	buf_to_screen_110	;  Yes
	pop	ds
	assumes ds,Data
	assumes es,nothing

	ret


	assumes ds,Data
	assumes es,nothing

buf_to_screen_130:
	pop	cx			;Remove Y from stack

buf_to_screen_140:
	mov	old_valid,OLD_IS_INVALID;Invalidate save area
	ret

copy_buffer_to_screen endp
	page
;--------------------------Private-Routine------------------------------;
; erase_old_cursor
;
;   The old cursor is erased from the screen, and the local buffer
;   updated as needed.	This may be performed in a couple of different
;   ways:
;
;	If a cursor/icon isn't drawn on the screen, the local buffer
;	is filled from the screen in preperation of the forthcoming
;	draw.
;
;	If a cursor/icon is drawn on the screen and the new and old
;	cursors/icons will not fit within the buffer, the cursor/icon
;	is removed from the screen, and then the local buffer is
;	filled from the screen in preperation of the forthcoming draw.
;
;	If a cursor/icon is drawn on the screen and the new and old
;	cursors/icons will fit within the buffer, the local buffer is
;	filled from the screen (based at the new (x,y)) in preperation
;	of the forthcoming draw.
;
; Entry:
;	DS = Data
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	compute_buffer_xy
;	copy_save_to_screen
;	copy_screen_to_buffer
;	copy_save_to_buf
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


erase_old_cursor proc near

	call	compute_buffer_xy	;See if old and new will fit in buffer
	xor	ax,ax
	mov	bx,DataOFFSET old_valid
	cmp	[bx],al 		;Is old image invalid?
	jz	erase_old_cursor_10	;  Yes, just read new area into buffer
	mov	[bx],al 		;Mark old saved cursor as invalid
	errnz	OLD_IS_INVALID		;(must be zero)
	or	bp,bp			;Will both cursors fit within buffer?
	jz	erase_old_cursor_20	;  Yes


;	Both cursors will not fit within the local buffer.  Restore the
;	area under the cursor, and refill the local buffer based on the
;	new cursor (x,y)


	call	copy_save_to_screen	;Restore what was under the cursor

erase_old_cursor_10:
	call	copy_screen_to_buffer	;Copy from screen into local buffer
	ret



;	Both cursors can be contained within the local buffer.	Reread
;	the information from the screen based on the new cursor (x,y)
;	and remove the old cursor from the local buffer.


erase_old_cursor_20:
	call	copy_screen_to_buffer	;Read screen into local buffer
	call	copy_save_to_buf	;Remove cursor
	ret

erase_old_cursor endp
	page
;--------------------------Private-Routine------------------------------;
; copy_screen_to_buffer
;
;   The contents of the given region of Regen RAM where the cursor/icon
;   is/will go are copied into the local buffer.
;
;   Since the height of the data to be copied is rounded UP to the
;   next multiple of two, the space allocated for the buffer must
;   take this into account and always allocated according to:
;
;	    (MAX_BUF_HEIGHT + 1) and 1
;
; Entry:
;	screen_pointer	= offset of source in EGAMem
;	buf_height	= # of scanlines to transfer
;	DS		= Data
; Returns:
;	None
; Error Returns:
;	No error return.
; Registers Preserved:
;	BP,DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


copy_screen_to_buffer proc near

	mov	ax,SCAN_INC		;Prepare for the copy
	mov	cx,buf_height		;Set # scans to copy (round up)
	inc	cx
	sar	cx,1
	mov	si,screen_pointer	;Set up source pointer
	mov	dx,ds			;Save data segment
	mov	di,ScreenSelector	;Set up destination and source segs
	mov	ds,di
	mov	es,di
	assumes ds,EGAMem
	assumes es,EGAMem

	mov	di,offset screen_buf

copy_screen_to_buffer_10:
	movsb				;Use bytes to prevent an exception
	movsb				;  on the 80286 for a word read at
	movsb				; ScreenSeg:FFFF
	movsb
	movsb
	movsb
	movsb
	movsb
	movsb
	add	si,ax
	movsb
	movsb
	movsb
	movsb
	movsb
	movsb
	movsb
	movsb
	movsb
	add	si,ax
	loop	copy_screen_to_buffer_10
	errnz	BUF_WIDTH-9		;Must be 9 bytes wide

	mov	ds,dx			;Restore trashed data segment
	assumes ds,Data

	ret

copy_screen_to_buffer endp
	page
;--------------------------Private-Routine------------------------------;
; compute_buffer_xy
;
;   The (x,y) coordinate of the bounding box that can contain both
;   the old and new cursors within the local buffer is computed.
;   The (x,y) computed will be the upper left hand corner of this box.
;
;   If no box exists, then the (x,y) of the new cursor location will
;   be used and the caller given a flag indicating that both cursors
;   did not fit.
;
;   The screen address of this (x,y) is also computed.
;
; Entry:
;	DS = Data
; Returns:
;	BP = 0 if both cursors fit within the buffer
;	BP <> 0 if both cursors did not fit (or no old cursor)
; Error Returns:
;	No error return.
; Registers Preserved:
;	DS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,BP,ES,FLAGS
; Calls:
;	compute_screen_pointer
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing

compute_buffer_xy proc near

	xor	bp,bp			;Show both cursors can fit in buffer
	cmp	old_valid,OLD_IS_INVALID;Is there a cursor on screen?
	je	comp_buf_xy_30		;  No, can read anything we want


;	Compute the leftmost x coordinate and the delta x of the cursors.
;	Then see if delta x is such that both cursors will fit within the
;	buffer.


	mov	ax,x_cell		;Compute the minimum of old and new X
	mov	bx,old_x_cell		;  and put it in bx
	cmp	ax,bx
	jge	comp_buf_xy_10		;Old is lesser
	xchg	ax,bx			;New is lesser

comp_buf_xy_10:
	and	bl,11111000B		;Convert lesser to a byte address
	sub	ax,bx			;Compute delta X
	cmp	ax,FARTHEST		;Will both cursors fit?
	jge	comp_buf_xy_30		;  No
	mov	x_buffer,bx		;Set X coordinate of left hand side


;	Compute the uppermost y coordinate and the delta y of the cursors.
;	Then see if delta y is such that both cursors will fit within the
;	buffer.


	mov	ax,y_cell		;Compute the minimum of old and new Y
	mov	bx,old_y_cell		;  and put it in BX
	mov	cx,ax			;  (save y_cell)
	sub	ax,bx			;Compute the delta
	jge	comp_buf_xy_20		;  Old is greater
	neg	ax			;  New is greater
	mov	bx,cx			;Set the minimum

comp_buf_xy_20:
	cmp	ax,MAX_BUF_HEIGHT-CUR_HEIGHT
	jle	comp_buf_xy_40		;Both cursors fit in the buffer



;	Either both cursors will not fit into the buffer or there
;	is no old cursor.  Set up the new (x,y) as the starting
;	coordinate and flag that both are not within the buffer.


comp_buf_xy_30:
	mov	ax,x_cell		;Store the new x,y of the buffer
	and	al,11111000b
	mov	x_buffer,ax
	mov	bx,y_cell
	xor	ax,ax			;Only minimum scan lines must be read
	not	bp			;Show both cursors did not fit

comp_buf_xy_40:
	mov	y_buffer,bx		;Set Y coordinate
	add	ax,CUR_HEIGHT		;Set # scanlines to read in
	mov	buf_height,ax		;Save buffer height
	mov	ax,x_buffer		;Compute screen address of the upper
	mov	si,bx			;  left corner of the buffer
	call	compute_screen_pointer
	mov	screen_pointer,si
	ret

compute_buffer_xy endp
	page
;--------------------------Private-Routine------------------------------;
; compute_screen_pointer
;
;   The screen address of point (ax,si) is computed.
;
; Entry:
;	AX = screen x coordinate
;	SI = screen y coordinate
; Returns:
;	SI = screen pointer
; Error Returns:
;	No error return.
; Registers Preserved:
;	BX,CX,DX,DI,DS,ES
; Registers Destroyed:
;	AX,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing

compute_screen_pointer proc near

	xchg	ax,si			;Save X coordinate, get Y
	ashiftr si,3			;Compute X offset
	shiftl	ax,4
	add	si,ax			;Add in * 16
	shiftl	ax,2
	add	si,ax			;Add in * 64 for a total of 80
	errnz	SCAN_BYTES-80		;Must be 80 bytes wide
	ret

compute_screen_pointer endp

sEnd	Code

ifdef	PUBDEFS
	include CURSOR.PUB
endif
	end
