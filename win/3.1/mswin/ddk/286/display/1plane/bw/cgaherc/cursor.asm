page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	CURSOR.ASM
;
; This file contains the pointer shape routines required to draw the
; pointer shape on the Hercules and IBM CGA graphics adapters.
;
; Created: 23-Feb-1987
; Author:  Walt Moore [waltm]
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
;
; Restrictions:
;
;   All routines herein assume protection either via cli/sti
;   or a semaphore at higher level code.
;	
;----------------------------------------------------------------------------;
; An Important Bug Fix:							     ;
;							   		     ;
;    The code which copies the area of the screen underneath a cursor to a   ;
;    local buffer accepted -ve coordinates for the top left corner of the    ;
;    area to be copied but didnot really bother about how to map the -ve     ;
;    nos. This didnot matter mostly because when the buffer would finally    ;
;    be drawn to the scree, it would clip the rectangle against the device   ;
;    coordinates (which always is +ve). The code also worked because of wrap ;
;    around at 0ffffh.							     ;
;									     ;
;    However for CGA, the available memory in the display segment is not till;
;    0ffffh and this caused hedious bugs - page not present faults while     ;
;    accessing and ROM cache over write during draw back.                    ;
;									     ;
;   The code for CGA now maps a -ve scan no to SCREEN_HEIGHT - |scan no|     ;
;   and takes care to update scree pointers form the last scan to the first  ;
;   scan. Like wise, if while copying from screen, the screen pointer is -ve ;
;   , this would be the contribution of a -ve left, +ve top coordinate. In   ;
;   this case the pointer in local buffer is nudged up by the amount of left ;
;   clip and screen pointer updated to zero.			             ;
;									     ;
;   The code now works very well.					     ;
;   -by- Amit Chatterjee [amitc]   Tue 11-Apr-1989 12:57:44                  ;
;----------------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include windefs.inc
	include cursor.inc		;Device specific constants
	include	macros.mac
	.list


	??_out	cursor


ifdef	HERCULES
	externA	HERCULES_DEFINED
endif

ifdef	IBM_CGA
	externA	IBM_CGA_DEFINED
endif

	externA ScreenSelector		;Segment of screen
	externA	EVEN_SCAN_INC
	externA	ODD_SCAN_INC		;increment to next interlaced scan
ifdef	HERCULES
	externA	INTERLACE_WRAP		;wrap back to opposite bank of scans
endif

CGA_OFFSET_LAST_SCAN       equ	03ef0h	; offset of last CGA scan
CGA_PAST_END_IN_EVEN_BANK  equ  01f40h	; offset past the end of even bank 

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

	public	x_cell			;Make all of these values available
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

x_cell			dw	0
y_cell			dw	0
cursor_left_clipped	dw	0	;will be true if cursor is clipped left


;	rotation maintains the number of bits the cursor masks have
;	been rotated.  This value is always between 0 and 7

rotation	db	0


save_area	db	MASK_LENGTH dup (?)
screen_buf	db	(((MAX_BUF_HEIGHT+1) and 0FFFEh) * BUF_WIDTH) dup (?)


;	old_x_cell and old_y_cell contain the (X,Y) on the
;	screen to which the contents of the save_area is mapped.
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
;	plus the overlap of the old and new cursor/icon.  Thus if
;	there is only 1 pixel difference in Y, only CUR_HEIGHT+1
;	scans must be processed.

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

	page
;--------------------------Public-Routine-------------------------------;
; move_cursors
;
;   Move AND and XOR cursor masks
;
;   The AND and XOR cursor masks are stored in the cursor work areas.
;
; Entry:
;	DS:SI --> AND mask
;	ES     =  Data segment
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DS,ES,BP
; Registers Destroyed:
;	AX,CX,DI,SI
; Calls:
;	none
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
;	erase_old_cursor
;	copy_buffer_to_save
;	rotate_masks
;	put_cursor_in_buffer
;	copy_buffer_to_screen
; History:
;	Tue 18-Aug-1987 14:36:59 -by-  Walt Moore [waltm]
;	Added test of the disabled flag.
;
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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

	mov	ax,ds
	mov	es,ax
	assumes es,Data

	cld				;This is interrupt code, do this!
	push	bp			;Must save this
	call	erase_old_cursor	;Erase old cursor and setup for new
	call	copy_buffer_to_save	;Save area under new cursor
	call	rotate_masks		;Rotate cursor masks into place
	call	put_cursor_in_buffer	;Generate new cursor in local buffer
	call	copy_buffer_to_screen	;Copy new cursor to screen (and
	pop	bp			;  possibly restore old cursor area)

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
;	copy_save_to_screen
; History:
;	Tue 18-Aug-1987 14:36:59 -by-  Walt Moore [waltm]
;	Added test of the disabled flag.
;
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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

	mov	ax,ds
	mov	es,ax
	assumes es,Data

	cld
	push	bp
	call	copy_save_to_screen
	pop	bp

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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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

	mov	cx,si			;save y coord for setup_copy_20
	call	compute_screen_pointer	;Compute address on screen
	call	setup_copy_20
	xchg	si,di
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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
	mov	si,DataOFFSET cur_and_mask
	mov	bx,DataOFFSET cur_xor_mask

put_cursor_in_buffer_10:
	mov	dx,SAVE_WIDTH

put_cursor_in_buffer_20:
	lodsw				;get AND mask
	and	ax,[di]			;AND it with the screen
	xor	ax,[bx]			;XOR with XOR mask
	stosw

	lodsw
	and	ax,[di]
	xor	ax,[bx+2]
	stosw

	lodsb
	and	al,[di]
	xor	al,[bx+4]
	mov	es:[di],al

	add	di,dx			;--> next buffer location
	add	bx,dx			;--> next XOR mask location
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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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

endif					;End of small rotate



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

endif				;End of SMALL_ROTATE



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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
	mov	bx,ds
	mov	es,bx

	assumes	es,Data

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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
	mov	bx,ds
	mov	es,bx

	assumes es,Data

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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


copy_buffer_to_screen proc near

	call	setup_copy
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
	mov	cursor_left_clipped,0	;assume no left clipping
	or	cx,cx			;Is the left side off the screen?
	jge	buf_to_screen_20	;  No,
	neg	cx			;  Yes, clip whatever is needed
	add	di,cx			;    by biasing the start of the
	add	si,cx			;    buffer and screen addresses
	mov	cursor_left_clipped,1	;cursor is clipped on left
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
;	add	ax,cx			;Bias the scanline increments
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
        neg     cx                      ;Compute excess to be clipped
        push    cx                      ;Save amount clipped

	push	dx			;save
	mov	dx,CGA_OFFSET_LAST_SCAN	;value to test boundary against
	cmp	cursor_left_clipped,1	;is cursor clipped ?
	jnz	@f			;no
	sub	dx,BUF_WIDTH - 1	;allow for left clip in boundary cond
@@:


ifdef   IBM_CGA
cga_buf_to_screen_60:


;----------------------------------------------------------------------------;
; we are treating CGA screen like a circular buffer in Y, so from the last   ;
; scan we need to go to scan no 0. Also because of x_cell being negative we  ;
; start a few bytes to the lft of desired scan, but this may at best be      ;
; BUF_WIDTH bytes.							     ;
;								             ;
; -Amit Chatterjee [am'itc] 04/11/89.					     ;
;----------------------------------------------------------------------------;

        add     si,vc_buf_1		;Update buffer start
	cmp	di,dx			;dx has the boundary condition
	jl	@f
	sub	di,CGA_PAST_END_IN_EVEN_BANK
@@:
        add     di,ax			;Update screen start   (scanline inc)
;bobgru -- don't think I need this
;	add     di,dx			;Update screen start   (width inc)
        xchg    ax,bx			;Toggle scanline increment
        loop    cga_buf_to_screen_60
	pop	dx			;restore
        pop     cx			;Get amount clipped
        jmp     short buf_to_screen_90	;Update scanline count
endif


ifdef   HERCULES
herc_buf_to_screen_60:
        add     si,vc_buf_1		;Update buffer start
        loop    herc_buf_to_screen_60
	pop	dx			;balance
        pop     cx			;Get amount clipped
        jmp     short buf_to_screen_90	;Update scanline count
endif


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
;		BX	 =  second scanline increment
;		DX	 =  width of move in bytes
;		BP	 =  # horizontal bytes clipped
;		vc_buf_2 =  # of scanlines to move
;		ES:DI	--> destination on screen
;		DS:SI	--> source in local buffer


buf_to_screen_100:
        mov     cx,ScreenSelector	;Set segment address of REGEN RAM
        mov     es,cx
	assumes	es,nothing
        xor     ch,ch			;Set msb of width
	sub	ax,dx			;adjust scan increment by width of move
	sub	bx,dx			;adjust scan increment by width of move
        mov     dh,bptr vc_buf_2	;Place # scanlines in here      ;112784

ifdef   IBM_CGA
cga_buf_to_screen_110:
        mov     cl,dl                   ;Set width of one scanline      ;112784
        rep     movsb                   ;Move this scan line to screen
        add     di,ax                   ;--> next scanline
        add     si,bp                   ;--> next source
        xchg    ax,bx                   ;Set next scanline increment
        dec     dh                      ;Any more scanlines to process? ;112784
        jnz     cga_buf_to_screen_110	;  Yes
        mov     ax,ds			;Leave es:=ds: for whomever follows
        mov     es,ax
        ret
endif


ifdef   HERCULES
herc_buf_to_screen_110:
        mov     cl,dl                   ;Set width of one scanline      ;112784
        rep     movsb                   ;Move this scan line to screen
        add     di,ax                   ;--> next scanline
        jns     herc_buf_to_screen_120	;  No wrap
        add     di,INTERLACE_WRAP       ;  Wrap

herc_buf_to_screen_120:
        add     si,bp                   ;--> next source
        dec     dh                      ;Any more scanlines to process? ;112784
        jnz     herc_buf_to_screen_110	;  Yes
        mov     ax,ds                   ;Leave es:=ds: for whomever follows
        mov     es,ax
        ret
endif



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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
;   take this into account and always be allocated according to:
;
;	    (MAX_BUF_HEIGHT + 1) and 1
;
; Entry:
;	screen_pointer	= offset of source in display memory
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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


copy_screen_to_buffer proc near

	push	ds

	mov	dx,x_buffer		;get left edge abiccsa
	rol	dx,1			;get sign bit to LSB
	and	dx,1			;dx = 1 for clipped left edge

        call    setup_copy              ;Prepare for the copy
        push    cx                      ;Save Y coordinate

;       Since the height of the data to be copied is rounded UP to the
;       next multiple of two, the space allocated for the buffer must
;       take this into account and always allocated according to:
;
;               (maxbufht + 1) AND 1


        mov     cx,buf_height

        mov     si,screen_pointer	;Set up source pointer
        mov     di,ScreenSelector
        mov     ds,di

	mov	di,DataBASE		;Set up destination pointer
	mov	es,di
        mov     di,OFFSET screen_buf


        assumes ds,nothing              ;ds is now trashed!
	assumes	es,Data



copy_cur_to_buf:                        ;Copy for a cursor


ifdef   IBM_CGA
        errn$   copy_cur_to_buf
cga_copy_cur_to_buf:

cga_copy_cur_to_buf_10:

;----------------------------------------------------------------------------;
; if screen pointer is negative, we have a left clip edge actually we are    ;
; expected to copy whatever here and clipping will be done later, so we just ;
; bump the buffer pointer by the amount the device pointer is below zero and ;
; then copy the remaining bytes in scan 0.				     ;
;									     ;
; -Amit Chatterjee  [amitc]   04/11/89.			                     ;
;----------------------------------------------------------------------------;

	push	cx			;save
	mov	cx,BUF_WIDTH		;width of buffer
	or	si,si			;is it to left of top scan left edge ?
	jns	@f			;no.
	add	cx,si
	neg	si			;no of bytes beyond left edge
	add	di,si			;take di past the insignificant bytes
	xor	si,si			;si starts here for valid bytes
@@:
	rep	movsb			;move the remaining bytes			;what
	sub	si,BUF_WIDTH		;get start of last copy

;----------------------------------------------------------------------------;
; we are treating CGA screen like a circular buffer in Y, so from the last   ;
; scan we need to go to scan no 0. Also because of x_cell being negative we  ;
; start a few bytes to the lft of desired scan, but this may at best be      ;
; BUF_WIDTH bytes.							     ;
;								             ;
; -Amit Chatterjee [amitc] 04/11/89.					     ;
;----------------------------------------------------------------------------;

	mov	cx,CGA_OFFSET_LAST_SCAN ;compare value against
	cmp	dx,1			;is left clipped ?
	jnz	@f			;no. 
	sub	cx,BUF_WIDTH - 1	;allow for left clipping
@@:
	cmp	si,cx			;test boundary condition
	jl	@f			;still not time to wrap over to start
	sub	si,CGA_PAST_END_IN_EVEN_BANK
@@:
        add     si,ax
	xchg	ax,bx			;swap even odd increments for next pass
	pop	cx			;get back loop count
        loop    cga_copy_cur_to_buf_10
        pop     cx
	pop	ds
        ret
endif	;IBM_CGA


ifdef   HERCULES
        errn$   copy_cur_to_buf
herc_copy_cur_to_buf:                   ;Copy for a cursor, hercules card
	sub	ax,BUF_WIDTH		;adjust scan inc for buffer width

        pop     dx                      ;Get back Y coordinate
        or      dx,dx                   ;Is Y off top of screen?
        jns     herc_copy_cur_to_buf_10 ;  No, all is fine
        add     cx,dx                   ;Skip this many lines
        jng     herc_copy_cur_to_buf_30 ;Nothing to read.

herc_copy_cur_to_buf_0:
	add     di,BUF_WIDTH
        inc     dx
        jnz     herc_copy_cur_to_buf_0

herc_copy_cur_to_buf_10:
        xor     bx,bx                   ;Zero -X adjustment
        mov     dx,x_buffer
        sar     dx,1
        jns     herc_copy_cur_to_buf_15 ;No adjustment needed
        sar     dx,1
        sar     dx,1
        sub     bx,dx                   ;bx = correction for -X coordinates

herc_copy_cur_to_buf_15:
        movsb
        movsb
        movsb
        movsb
        movsb
        movsb
        movsb
        movsb
        movsb
	errnz	BUF_WIDTH-9
        add     si,ax
        mov     dx,bx                   ;If correction + address negative,
        add     dx,si                   ;  then wrap
        jns     herc_copy_cur_to_buf_20
        add     si,INTERLACE_WRAP

herc_copy_cur_to_buf_20:
        loop    herc_copy_cur_to_buf_15

herc_copy_cur_to_buf_30:
	pop	ds
        ret
endif	;HERCULES


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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
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
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing

compute_screen_pointer	proc	near


ifdef   IBM_CGA
cga_compute_screen_pointer:

;----------------------------------------------------------------------------;
; If start Y is negative, roll it over to start at an approprite scan near   ;
; the bottom.								     ;
;									     ;
; -Amit Chatterjee [amitc]  04/11/89.					     ;
;----------------------------------------------------------------------------;

	or	si,si			;test for negative Y coordinate
	jns	@f			;y coordinate is poitive and OK.
	add	si,SCREEN_HEIGHT	;roll it over to bottom of screen
@@:
        xchg    ax,si                   ;Save x coordinate, get y

        sar     si,1                    ;Compute X offset
        sar     si,1
        sar     si,1

        shr     ax,1                    ;Odd scan line?
        jnc     cga_comp_scr_ptr_10        ;  No
        add     si,2000h                ;  Yes, add in interlace

cga_comp_scr_ptr_10:
	AddWidthTimesAxToSi		;macro defined in CURSOR.INC
        ret
endif	;IBM_CGA



ifdef   HERCULES
herc_compute_screen_pointer:
        xchg    ax,si                   ;Save x coordinate, get y

        sar     si,1                    ;Compute X offset
        sar     si,1
        sar     si,1

        sar     ax,1                    ;Check scan line
        js      herc_comp_scr_ptr_30         ;  Negative, exit now
        jnc     herc_comp_scr_ptr_10         ;Multiple of two, don't add 2000h
        add     si,2000h

herc_comp_scr_ptr_10:
        sar     ax,1
        jnc     herc_comp_scr_ptr_20         ;Multiple of four, don't add 4000h
        add     si,4000h

herc_comp_scr_ptr_20:
	AddWidthTimesAxToSi		;macro defined in CURSOR.INC

herc_comp_scr_ptr_30:
        ret
endif	;HERCULES

compute_screen_pointer	endp
	page

;--------------------------Private-Routine------------------------------;
; setup_copy - Set up a copy involving the screen
;
; A copy involving the screen is set up.  Two values are returned,
; both being increments to scanlines.
;
; Entry:
;	None
; Returns:
;	AX = inc to next scanline start
;	BX = inc to second scanline start
;	CX = [y_buffer]
; Error Returns:
;	No error return.
; Registers Preserved:
;	DX,DI,DS,ES
; Registers Destroyed:
;	AX,BX,CX,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

setup_copy	proc	near
        mov     cx,y_buffer
;       jmp     setup_copy_20
        errn$   setup_copy_20

setup_copy	endp
page

;--------------------------Private-Routine------------------------------;
; setup_copy_20 - Set up a copy involving the screen
;
; A copy involving the screen is set up.  Two values are returned,
; both being increments to scanlines.
;
; Entry:
;	CX = Y coordinate
; Returns:
;	AX = inc to next scanline start
;	BX = inc to second scanline start
; Error Returns:
;	No error return.
; Registers Preserved:
;	CX,DX,DI,DS,ES
; Registers Destroyed:
;	AX,BX,FLAGS
; Calls:
;	None
; History:
;	Mon 23-Feb-1987 12:47:30 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

setup_copy_20	proc	near


ifdef   IBM_CGA
        errn$   setup_copy_20
cga_setup_copy_20:
        mov     ax,EVEN_SCAN_INC         ;Assume start is even scan line
        mov     bx,ODD_SCAN_INC
        test    cl,1
        jz      csc1
        xchg    ax,bx
csc1:
        ret
endif	;IBM_CGA




ifdef   HERCULES
        errn$   setup_copy_20
herc_setup_copy_20:
        mov     ax,ODD_SCAN_INC         ;Increment is same regardless of scan
;       mov     bx,INTERLACE_WRAP       ;Unused for hercules.  Leave as garbage
        ret
endif	;HERCULES

setup_copy_20	endp

sEnd	Code


ifdef	PUBDEFS
	include cursor.pub
endif
	end

