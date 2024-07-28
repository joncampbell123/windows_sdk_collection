;
;	FILE:	cursor.asm
;	DATE:	1/9/91
;	AUTHOR: Jim Keller
;
;	This module is the cursor windows-cursor display driver interface.
;	It controls two other modules: hwcurs.asm and swcurs.asm the
;	hardware and software cursor modules. A hardware cursor is always
;	preferable, but depending upon the mode, resolution, board, etc.
;	sometimes only a software cursor can be displayed. Which cursor
;	type (HW or SW) to use is also dependent upon the cursor data, and
;	hence the decision is made every time SetCursor is called.

.286
FILE_CURSOR	EQU	1

include cmacros.inc
include macros.mac
include windefs.inc
include gdidefs.inc
include vgareg.inc
include vgautil.inc
include hwcurs.inc
include swcurs.inc
include cursor.inc


SAVE_SQREGS_COUNT	EQU	14
SAVE_GRREGS_COUNT	EQU	4
SQ_INDEX        EQU     2AH             ;storage offset in cursor_state array
GR_INDEX        EQU     2BH             ;storage offset in cursor_state array


curs_func	STRUC
	CURSFUNC_SET		DW	0
        CURSFUNC_MOVE           DW      0
	CURSFUNC_CHECK		DW	0
	CURSFUNC_OFF		DW	0
	CURSFUNC_EXCLUDE	DW	0
	CURSFUNC_UNEXCLUDE	DW	0
curs_func	ENDS

sBegin	Data

EXTRN	enabled_flag:WORD

PUBLIC  exclude_rect_left, exclude_rect_right
PUBLIC	exclude_rect_top, exclude_rect_bottom, exclude_global
PUBLIC	exclude_rect_valid, cursor_flags, cursor_funcs
PUBLIC	cursor_xcoord, cursor_ycoord
PUBLIC	cursor_xdraw, cursor_ydraw
PUBLIC  screen_busy

screen_busy		DB	1,0
cursor_xcoord		DW	0
cursor_ycoord		DW	0
cursor_xdraw		DW	0
cursor_ydraw		DW	0
cursor_flags		DW	0
exclude_global		DW	0
exclude_rect_valid      DW      0
exclude_rect_left	DW	0
exclude_rect_right	DW	0
exclude_rect_top	DW	0
exclude_rect_bottom	DW	0
cursor_data		DW	(SIZE cursorShape + 100H) DUP(?)
cursor_state_lock	DW	0
cursor_state            DB      30H DUP(0)
work_buf		DB	(256 + SIZE cursorShape) DUP(0)
cursor_funcs	DW	hwcursor_set, hwcursor_move, hwcursor_check
		DW	hwcursor_off, hwcursor_exclude, hwcursor_unexclude

hwcurs_funcs    LABEL   WORD
DW	hwcursor_set
DW      hwcursor_move
DW	hwcursor_check
DW	hwcursor_off
DW      hwcursor_exclude
DW	hwcursor_unexclude

swcurs_funcs	LABEL	WORD
DW	swcursor_set
DW	swcursor_move
DW	swcursor_check
DW	swcursor_off
DW	swcursor_exclude
DW	swcursor_unexclude

sEnd    Data


createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin  BlueMoonSeg
;       inquire_data contains information about mouse acceleration
;       which the window manager uses.
PUBLIC	inquire_data
inquire_data    CURSORINFO   <X_RATE,Y_RATE>
sEnd    BlueMoonSeg

sBegin	Code
	assumes cs,Code
	assumes ds,Data

;
;	SetCursor	- change cursor shape
;

	org	$+1			;The data segment value will
PUBLIC	_cstods 			;  be stuffed here and kept
_cstods label	word			;  current by the kernel
	org	$-1

cProc	SetCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es,ds>
	parmD	lp_cursor			;Far ptr to new cursor shape
cBegin
	cmp	enabled_flag,00H
	jne	SetCursor_enabled
SetCursor_locally:
	push	ds
	pop	es
	and	cursor_flags,NOT YES_CURSOR
        lds     si,lp_cursor
	mov	ax,ds
	or	ax,si
	je	SetCursor_Busy
	lea	di,cursor_data
	mov	cx,(SIZE cursorShape + 100H) / 2
	rep	movsw
	or	es:cursor_flags,LOCAL_LOAD + YES_CURSOR
	jmp	SetCursor_Busy

SetCursor_enabled:
	sub	cx,cx				;check for screen busy and
	xchg	screen_busy,cl			; prevent reentrance
	jcxz	SetCursor_Busy

	test	cursor_flags,NEW_COORDS
	jz	SetCursor_OldXY
	EnterCrit
	mov	cx,cursor_xcoord
	mov	bx,cursor_ycoord
	mov	cursor_xdraw,cx 		;these will be the coordinates
	mov	cursor_ydraw,bx 		; the cursor will be drawn at
	and     cursor_flags,not NEW_COORDS
	LeaveCrit b
SetCursor_OldXY:

	mov	exclude_global,1		;OFF flag for other modules
        and     cursor_flags,NOT YES_CURSOR
	call	cursor_funcs.CURSFUNC_OFF	;turn off current cursor
        les     si,lp_cursor                    ;NULL pointer -- no cursor
	mov	ax,es
        or      ax,si
        je      SetCursor_Exit
	mov	exclude_global,0
        or      cursor_flags,YES_CURSOR

	call	Set_Cursor_Type 		;Hardware or software cursor?
	call	cursor_funcs.CURSFUNC_SET	;pass cursor info to submodule

SetCursor_Exit:
	mov	screen_busy,1

SetCursor_Busy:

cEnd




;       MoveCursor
;
;	This routine moves the cursor hot spot to a designated x,y coord.


cProc	MoveCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es,ds>
	parmW	abs_x				;x coordinate of cursor
	parmW	abs_y				;y coordinate of cursor
cBegin
	EnterCrit				;need a coherent state here
	mov	ax,abs_x
	mov	cursor_xcoord,ax
        mov     ax,abs_y
	mov	cursor_ycoord,ax
	or	cursor_flags,NEW_COORDS
	LeaveCrit a

	test	cursor_flags,YES_CURSOR 	;if NULL cursor data, done
	jz	MoveCursor_Busy

	sub	cx,cx				;check for screen busy and
	xchg	screen_busy,cl			; prevent reentrance
	jcxz	MoveCursor_Busy

Move_cursor_busy_0:
	EnterCrit				;obtain a coherent state
	mov	cx,cursor_xcoord
	mov	bx,cursor_ycoord
	and	cursor_flags,NOT NEW_COORDS
	LeaveCrit a

	mov	cursor_xdraw,cx 		;these will be the coordinates
	mov	cursor_ydraw,bx 		; the cursor will be drawn at

	test	cursor_flags,LOCAL_LOAD
	jz	@F
	push	ds
	pop	es
	lea	si,cursor_data
	call	Set_Cursor_Type 		;Hardware or software cursor?
	call	cursor_funcs.CURSFUNC_SET	;pass cursor info to submodule
	and	cursor_flags,NOT LOCAL_LOAD

@@:
	call	cursor_funcs.CURSFUNC_MOVE
	test	cursor_flags,NEW_COORDS 	;if cursor moved since drawing
	jnz	Move_cursor_busy_0		; began, draw at new coords
	mov	screen_busy,1

MoveCursor_Busy:

cEnd



;	CheckCursor
;
;	This Routine is called every 1/4 of a second by the system. If
;	anything needs to happen to the cursor, this is your chance!!

cProc	CheckCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es,ds>
cBegin
	sub	cx,cx				;check for screen busy and
	xchg	screen_busy,cl			; prevent reentrance
	jcxz	CheckCursor_Busy
	test	cursor_flags,LOCAL_LOAD
	jz	@F
	push	ds
	pop	es
	lea	si,cursor_data
	call	Set_Cursor_Type 		;Hardware or software cursor?
	call	cursor_funcs.CURSFUNC_SET	;pass cursor info to submodule
	and	cursor_flags,NOT LOCAL_LOAD

@@:
        call    cursor_funcs.CURSFUNC_CHECK
	mov	screen_busy,1

CheckCursor_Busy:
cEnd



;	exclude
;
;	This routine dispatches to the hwcurs or swcurs routine. Those
;	routines will decide whether or not to perform the exclude.
;	Presumably, the hwcurs routine will not exclude the cursor and the
;	swcurs routine will.
;
;       PARMS:
;	cx	left
;	dx	top
;	si	right (inclusive)
;	di	bottom (inclusive)

PUBLIC	exclude_far
exclude_far	PROC	FAR

	call	exclude
	ret

exclude_far	ENDP


PUBLIC	exclude
exclude PROC	NEAR

	push	ds
	push	ax
	mov	ax,_cstods
	mov	ds,ax

	mov	screen_busy,0			;lock the screen
	mov	exclude_rect_valid,1		;these variables are used
	mov	exclude_rect_left,cx		; by interrupt time code
	mov	exclude_rect_right,si		; so they need to be set
	mov	exclude_rect_top,dx		; inside the screen lock
        mov     exclude_rect_bottom,di

        test    cursor_flags,YES_CURSOR
        jz      @F
	call	cursor_funcs.CURSFUNC_EXCLUDE

@@:	mov	screen_busy,1
	pop	ax
	pop	ds
	ret

exclude ENDP




;	unexclude
;
;	This routine dispatches to the hwcurs or swcurs routine.

PUBLIC	unexclude_far
unexclude_far	PROC	FAR

	call	unexclude
	ret

unexclude_far	ENDP


PUBLIC	unexclude
unexclude	PROC	NEAR

	push	ds
	push	ax
	push	dx
	mov	ax,_cstods
	mov	ds,ax

	mov	screen_busy,0			;lock the screen
        mov     exclude_rect_valid,0
        test    cursor_flags,YES_CURSOR
        jz      @F
	call	cursor_funcs.CURSFUNC_UNEXCLUDE
@@:	mov	screen_busy,1

	pop	dx
	pop	ax
	pop	ds
	ret

unexclude	ENDP




;       Set_Cursor_Type
;
;	This routine determines whether a hardware or software cursor will
;	be used. The determination works correctly on the following Video 7
;	boards: VRAM I, VRAMII, 1024I. (I think it works on VGA16 as well.)
;	The correct family of functions are loaded into the jump table
;	cursor_funcs.xxxx   The actual determination is quite simple:
;	32x32 HW cursors are supported by all boards in 16 color modes.
;	32x32 HW cursors are supported by all boards in a 256 color mode IF
;		bit 6 of Attribute register 10H is a 0; otherwise only
;		16x32 cursors are supported for the 256 color mode.
;
;	If a 32x32 HW cursor is supported, hardware cursor code will be used.
;       If it is determined that only a 16x32 cursor is supported in the
;	current mode, then a check will be made upon the cursor data to
;	see if the non-xparent data width" is 16 or less. If so, then the bits
;	in this "active data area" will be bit-doubled and a hardware cursor
;	can still be used. If the non-xparent data area is greater than 16
;	bits wide, then a software cursor will be used.
;
;	PARMS:
;	es:si	pointer to cursor structure (the structure, not just the data)
;	ds	DATA segment
;
;	RETURNS:
;	es:si	pointer to a cursor structure. If the cursor data had to be
;		bit-doubled to enable use of the hardware cursor, then a new
;		cursor structure was created with some fixups to the x hot
;		spot coordinate and ds:si will return pointing to this new
;		cursor structure.


PUBLIC	Set_Cursor_Type
Set_Cursor_Type 	PROC	NEAR

;	First see if we can use a hardware cursor!!

	lea	bx,hwcurs_funcs 		;assume a hardware cursor

	and	cursor_flags,NOT DOUBLE_PUMP
	mov	dx,VGAREG_STATUS1_R		;reset attribute toggle
	in	al,dx
	mov	dx,VGAREG_AT_ADDR		;We are in a 256 color mode
	mov	al,VGAREG_AT_MODE + 20H 	; 20H prevents screen blanking
	out	dx,al
	inc	dx
	in	al,dx				;If the double-pump memory
	and	al,40H				; bit is not set, then we
	je	Set_Cursor_Type_Found		; can use a hardware cursor
	or	cursor_flags,DOUBLE_PUMP

;       See if we are forced to use a software cursor

	call	Get_Cursor_Width_Mask

	lea	bx,swcurs_funcs 		;if ax != 0 then non-xparent
	or	ax,ax				; data is > 16 bits wide and a
	jne	Set_Cursor_Type_Found		; software cursor must be used
	cmp	dx,es:[si].csHotX		;if hot spot is in xparent part
	ja	Set_Cursor_Type_Found		; of data at left edge - hosed!


;	Hardware cursor is OK if we bit double and fixup the header info

	push	ds
	push	es
	pop	ds
	pop	es
	cld
	lea	di,work_buf			;mock up a cursor structure
	mov	ax,[si].csHotX			;Because of realignment and
	sub	ax,dx				; bit-doubling of active data,
	stosw
	add	si,2

	mov	cx,(SIZE cursorShape - 2) / 2	;copy in header information
	rep	movsw

	call	Bit_Double_Cursor
	lea	bx,hwcurs_funcs 		;hardware cursor usable
	mov	ax,es				;restore Data Segment
	mov	ds,ax
	lea	si,work_buf

Set_Cursor_Type_Found:
	mov	ax,[bx + 0]
	mov	[cursor_funcs].CURSFUNC_SET,ax
	mov	ax,[bx + 2]
	mov	[cursor_funcs].CURSFUNC_MOVE,ax
	mov	ax,[bx + 4]
	mov	[cursor_funcs].CURSFUNC_CHECK,ax
	mov	ax,[bx + 6]
	mov	[cursor_funcs].CURSFUNC_OFF,ax
	mov	ax,[bx + 8]
	mov	[cursor_funcs].CURSFUNC_EXCLUDE,ax
	mov	ax,[bx + 10]
	mov	[cursor_funcs].CURSFUNC_UNEXCLUDE,ax
	ret

Set_Cursor_Type         ENDP



;       Get_Cursor_Width_Mask
;
;	This routine forms a 32 bit mask that is defined to have a 1 in bit
;	position i if ANY row of the cursor has non-transparent data at bit
;	position i; otherwise bit i is a 0. The mask is left aligned by
;	shifting all 32 bits to the left until the high bit is a 1. This
;	left-shifted mask is returned in bx:ax and dx holds the left-alignment
;	shift count. (NOTE: if all the data is transparent -- certainly an
;	uninteresting cursor -- then bx:ax will of course = 0:0 and dx = 0.)
;
;	PARMS:
;	es:si	pointer to cursor structure (not just the data)
;
;	RETURNS:
;	bx:ax	left aligned mask
;       dx      left alignment shift count

PUBLIC	Get_Cursor_Width_Mask
Get_Cursor_Width_Mask	PROC	NEAR

	push	di
	mov	cx,CURSOR_HEIGHT		;number of rows of cursor data
	mov	dx,0FFFFH
	mov	di,dx
	sub	ax,ax
	mov	bx,ax
	add	si,SIZE cursorShape

@@:	and	di,es:[si]			;form a 32 bit mask which has
	and	dx,es:[si + 2]			; a 1 in a bit position i if
	or	ax,es:[si + 80H]		; ANY row of cursor data is
	or	bx,es:[si + 80H + 2]		; not transparent at that bit
	add	si,4				; position; otherwise bit
	loop	@B				; position i holds a 0
	sub	si,(80H + SIZE cursorShape)

	not	dx
	not	di
	mov	cx,CURSOR_WIDTH / 2		;32 bit mask is in bx:ax. If
	or	ax,di				; bx signed, non-xparent data
	or	bx,dx				; exists at cursor's left edge
	xchg	al,ah
	xchg	bl,bh

@@:	or	ax,ax				; otherwise left align the
	js	Get_Cursor_Width_Mask_Done	; non-xparent data keeping
	shl	bx,1				; track of how many xparent
	rcl	ax,1				; bits there were at left edge
	loop	@B

Get_Cursor_Width_Mask_Done:

	xchg	bx,ax
	mov	dx,CURSOR_WIDTH / 2		;cx should contain # of bits
	sub	dx,cx				; needed for left-alignment
	pop	di
	ret

Get_Cursor_Width_Mask	ENDP




;	Bit_Double_Cursor
;
;	This routine assumes that the the non-transparent cursor data is
;	at most 16 bits wide and it takes those 16 bits and bit-doubles
;	them out to 32 bits so that a hardware cursor can be implemented.
;	PARMS:
;	ds:si	pointer to cursor data (this is the data -- not the header)
;	es:di	place to store bit doubled cursor
;	dx	left-alignment shift count -- the amount to shift left one
;		row of the cursor data at ds:si so that the first non-
;		transparent bit position becomes the high bit.

PUBLIC	Bit_Double_Cursor
Bit_Double_Cursor	PROC	NEAR

        push    bp
	mov	bp,40H				;now bit-double -- grumble!
	cld

Bit_double_a_row:
	lodsw
	mov	bx,ax
	lodsw					;ax:bx has 32 bit cursor row
        xchg    al,ah
	xchg	bl,bh
	mov	cx,dx				;# to left-align active data
	or	cx,cx
	je	Bit_double_aligned

@@:	shl	ax,1				;when we are done aligning, the
	rcl	bx,1				; active data will all be in ax
	loop	@B				; since it is < 16 bits wide

Bit_double_aligned:
	push	dx
	mov	cx,16
@@:	rol	bx,1				;bit double ax into dx:bx
	rcl	dx,1
	rcl	ax,1
	ror	bx,1
	rol	bx,1
	rcl	dx,1
	rcl	ax,1
	loop	@B

	xchg	al,ah
	xchg	dl,dh
	stosw
	mov	ax,dx
	stosw
	pop	dx
	dec	bp
	jne	Bit_double_a_row
	pop	bp
	ret

Bit_Double_Cursor	ENDP



;	cursor_save_state
;
;	This routine saves the cursor state into a static data structure.
;	and sets up the registers for sequential chain 4 addressing. All
;	data-munging via the ALU is turned off.
;	This routine works in conjunction with cursor_restore state.
;	The equates below are important to both routines.
;	PARMS:
;	ds	Data segment

SAVE_SPLITBANK_OFFSET	EQU	11 * 2
SAVE_SQREGS_COUNT       EQU     14
SAVE_GRREGS_COUNT	EQU	4
SAVE_MISCREGS_COUNT	EQU	1
SAVE_LASTREG_OFFSET	EQU	(SAVE_SQREGS_COUNT + SAVE_GRREGS_COUNT + SAVE_MISCREGS_COUNT) * 2 - 2

PUBLIC  cursor_save_state
cursor_save_state	PROC	NEAR

	cmp	cursor_state_lock,0
	je	@F
	jmp	cursor_save_state_0

@@:	push	es
	mov	ax,ds
	mov	es,ax
	cld

	mov	dx,VGAREG_SQ_ADDR		;save the index
	in	al,dx
	lea	di,cursor_state
	mov	es:cursor_state.SQ_INDEX,al

	mov	al,VGAREG_SQ_MAP_MASK
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT (0FH)			;enable all planes for writing
	or	al,0FH
	out	dx,al

        dec     dx
	mov	al,VGAREG_SQ_MODE
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT 0CH			;turn on chain 4 mode
	or	al,(SEQ_CHAIN_4_MODE SHL 2)
	out	dx,al

	dec	dx
	mov	al,VGAREG_SQ_BACKLATCH_0
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb

	dec	dx
	mov	al,VGAREG_SQ_BACKLATCH_1
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb

        dec     dx
	mov	al,VGAREG_SQ_BACKLATCH_2
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb

        dec     dx
	mov	al,VGAREG_SQ_BACKLATCH_3
        stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb

	dec	dx
	mov	al,VGAREG_SQ_MASKED_WRITE_CONTROL
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT 1			;disable masked writes on vrams
	out	dx,al

	dec	dx
	mov	al,VGAREG_SQ_BANK_SELECT
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb

	dec	dx
	mov	al,VGAREG_SQ_EXTENDED_PAGE
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb

	dec	dx
	mov	al,VGAREG_SQ_COMPATIBILITY
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb
	or	al,20H				;enable sequential chain4 mode
	out	dx,al

	dec	dx
	mov	al,VGAREG_SQ_FOREBACK_MODE
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT 0EH			;turn off data-munging options
	out	dx,al

        dec     dx
	mov	al,VGAREG_SQ_SPLITBANK
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb
	or	al,80H				;enable access to splitbanks
	out	dx,al

	dec	dx
        mov     al,VGAREG_SQ_SRC_SPLITBANK
        stosb
        out     dx,al
        inc     dx
        in      al,dx
        stosb
	sub	al,al				;clear out src splitbank
        out     dx,al

        dec     dx
	mov	al,VGAREG_SQ_DST_SPLITBANK
	stosb
        out     dx,al
	inc	dx
	in	al,dx
	stosb
	sub	al,al				;clear out dst splitbank
	out	dx,al

	dec	dx				;this is the 14th seq reg saved
	mov	al,VGAREG_SQ_SPLITBANK		;if you want to save more
	out	dx,al				; adjust the equates at the
	inc	dx				; start of this routine
	in	al,dx
	and	al,7FH				;disable splitbank addressing
	out	dx,al


;       Now for the Graphics Controller regs

	mov	dx,VGAREG_GR_ADDR		;save the index
	in	al,dx
	mov	es:cursor_state.GR_INDEX,al

	mov	al,VGAREG_GR_ENABLE_SET_RESET
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT (0FH)			;disable set reset - all planes
	out	dx,al

	dec	dx
	mov	al,VGAREG_GR_FUNCTION
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT (1FH)			;disable ALU ROPS
	out	dx,al

	dec	dx
	mov	al,VGAREG_GR_MODE
	stosb
	out	dx,al
	inc	dx
	in	al,dx
	stosb
	and	al,NOT (03H)			;set write mode 0
	out	dx,al

	dec	dx				;this is the 14th gr reg saved
	mov	al,VGAREG_GR_BITMASK		;if you want to save more
	stosb					; adjust the equates at the
	out	dx,al				; start of this routine
	inc	dx
	in	al,dx
	stosb
	mov	al,0FFH 			;all bits enabled
	out	dx,al

	mov	dx,VGAREG_MISC_R
	in	al,dx
	stosb

	pop	es

cursor_save_state_0:
        mov     cursor_state_lock,1
	ret

cursor_save_state	ENDP



;	cursor_restore_state
;
;	This routine restores the VGA state saved by cursor_save_state.
;	PARMS:
;	ds	Data segment

PUBLIC	cursor_restore_state
cursor_restore_state	PROC	NEAR

	lea	si,cursor_state.SAVE_LASTREG_OFFSET
	std					;restore in reverse order

	mov	dx,VGAREG_MISC_W
	lodsw					;Yes lodsW -- so si := si - 2
        out     dx,al

	mov	dx,VGAREG_GR_ADDR
	mov	cx,SAVE_GRREGS_COUNT
@@:	lodsw
	out	dx,ax
	loop	@B
	mov	al,cursor_state.GR_INDEX	;restore Sequencer index
	out	dx,al

        mov     dx,VGAREG_SQ_ADDR
	mov	ax,word ptr cursor_state.SAVE_SPLITBANK_OFFSET
	or	ah,80H					;provide access to
	out	dx,ax					; splitbank regs
        mov     cx,SAVE_SQREGS_COUNT

@@:	lodsw
	out	dx,ax
	loop	@B
	mov	al,cursor_state.SQ_INDEX	;restore Graphics index
	out	dx,al
	cld

	mov	cursor_state_lock,0
        ret

cursor_restore_state	ENDP


;	cursor_kludge
;
;	This routine, at the time it was written was used only by the hardware
;	cursor to force a reload of the cursor data when there was a screen
;	switch from DOS.
;	PARMS:
;	ds	Data segment

PUBLIC	cursor_kludge
cursor_kludge	PROC	FAR

	lea	ax,hwcursor_set
	cmp	ax,[cursor_funcs]
	jne	cursor_kludge_done

	sub	cx,cx				;check for screen busy and
	xchg	screen_busy,cl			; prevent reentrance
	jcxz	cursor_kludge_done

	call	hwcursor_kludge 		;this forces a cursor reload
	mov	screen_busy,1

cursor_kludge_done:
	ret

cursor_kludge	ENDP

sEnd    Code

END

