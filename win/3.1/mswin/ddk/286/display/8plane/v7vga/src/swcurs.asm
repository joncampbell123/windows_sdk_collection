;
;	FILE:	swcurs.asm
;	DATE:	1/9/91
;	AUTHOR: Jim Keller
;
;	This file handles the software cursor on the VRAM I, VRAM II,
;	and 1024I boards. (I think it also works on the VGA16.)
;
;       NOTE: swcursor_unexclude is LAZY which means that the cursor will
;	be brought back on (if it is excluded) by CheckCursor calling
;	swcursor_check at intervals of approximately 1/4 second. This
;	prevents the cursor from being redrawn on an unexclude and then
;	immediately reexcluded by another blt call.

.286
FILE_SWCURS	EQU	1

include cmacros.inc
include macros.mac
include windefs.inc
include gdidefs.inc
include display.inc
include vgareg.inc
include vgautil.inc
include cursor.inc
include swcurs.inc

externNP        enable_switching       ;Allow screen group switching
externNP	disable_switching      ;Disallow screen group switching

CURSOR_WIDTH	EQU	20H
CURSOR_HEIGHT	EQU	20H


CURSOR_UP	EQU	00000001B
SAVED_STATE	EQU	00000010B

sBegin  Data
EXTRN   ScreenSelector:word
save_screen_x		DW	0
save_screen_y		DW	0
save_screen_width	DW	CURSOR_WIDTH
save_screen_height	DW	CURSOR_HEIGHT
save_screen_xclip	DW	CURSOR_WIDTH
save_screen_yclip	DW	CURSOR_HEIGHT
save_screen_address     DW      0
save_screen_bank	DW	0
save_screen_buffer	DW	1024 DUP(0)

swcursor_local	DW		0
swcursor_flags	DW		0
swcursor	cursorShape	<>
swcursor_data	DW		2048 DUP (0)
sEnd    Data


sBegin	Code
	assumes cs,Code
	assumes ds,Data


;	swcursor_set
;
;	This routine should be called whenever new cursor data is supplied.
;	The current cursor will be taken down and the new cursor put up at
;	the same place.
;	PARMS:
;       ds      Data segment
;	es:si	ptr to cursorShape structure

PUBLIC	swcursor_set
swcursor_set	PROC	NEAR

	cld
        push    ds
	push	es
	pop	ds
	pop	es
	mov	cx,(SIZE cursorShape) / 2	;copy in header information
	lea	di,swcursor
	rep	movsw
	mov	dx,80H				;# bytes in AND mask

swcursor_expand_loop:
	mov	ah,[si + 80H]
	lodsb
	mov	bx,ax

	mov	cx,8
@@:	shl	bl,1
	sbb	al,al
	shl	bh,1
	sbb	ah,ah
	stosw
	loop	@B

	dec	dx
	jne	swcursor_expand_loop
        push    es
        pop     ds
        call    swcursor_move
        ret

swcursor_set	ENDP



;	swcursor_move
;
;	This routine moves the cursor to the coordinates:
;	cursor_xdraw, cursor_ydraw (maintained by cursor.asm).
;	PARMS:
;	ds	Data segment
;       cursor_xdraw, cursor_ydraw

PUBLIC	swcursor_move, swcursor_unexclude
swcursor_move	PROC	NEAR

	call	swcursor_sequence

swcursor_unexclude:
	ret

swcursor_move	ENDP




;	swcursor_exclude
;
;	This routine excludes the cursor from the exclude rectangle
;	(maintained by cursor.asm). This routine is needed for speed. The
;	swcursor_sequence routine could just be called and everything
;	would work; but swcursor_sequence will pull the cursor down if
;	it is not currently excluded, then check to see if the current
;	rectangle will cause an exclusion and, if not, put the cursor
;	back up. This may result in the cursor coming down and going back
;	up unnecssarily. This routine avoids that.
;	PARMS:
;	ds	Data segment

PUBLIC	swcursor_exclude, swcursor_check
swcursor_exclude	PROC	NEAR

swcursor_check:

	test	swcursor_flags,CURSOR_UP
	jz	ssex0
	call	swcursor_exclude_test
	jc	ssex1
	ret

ssex0:	call	swcursor_exclude_test
	jc	@F
ssex1:	call	swcursor_sequence

@@:     ret

swcursor_exclude	ENDP




;	swcursor_sequence
;
;	This routine sequences sequences taking the cursor down and then
;	putting it back up again.
;	PARMS:
;	ds	Data segment
;       cursor_xdraw, cursor_ydraw

PUBLIC	swcursor_sequence, swcursor_off
swcursor_sequence	PROC	NEAR

swcursor_off:

	test	swcursor_flags,CURSOR_UP
	jz	swcursor_seq_notup
		call	cursor_save_state
		or	swcursor_flags,SAVED_STATE
		call	swcursor_restore_screen
		and	swcursor_flags,NOT CURSOR_UP

swcursor_seq_notup:
        mov     ax,cursor_xdraw                 ;remember the coordinates
        mov     save_screen_x,ax                ; where the cursor is drawn.
        mov     ax,cursor_ydraw                 ; these will be used now to
        mov     save_screen_y,ax                ; put up the cursor and later

	call	swcursor_exclude_test
	jc	swcursor_seq_done
		call	cursor_save_state
		or	swcursor_flags,SAVED_STATE
                call    swcursor_save_screen
		call	swcursor_draw
		or	swcursor_flags,CURSOR_UP

swcursor_seq_done:
	test	swcursor_flags,SAVED_STATE
	jz	@F
	call	cursor_restore_state
	and	swcursor_flags,NOT SAVED_STATE

@@:	ret

swcursor_sequence	ENDP



;       swcursor_exclude_test
;
;	This routine does a hit test to see if any part of the cursor
;	overlaps the exclude rectangle.
;	PARMS:
;	ds	Data segment
;	exclude_rect_left, exclude_rect_right, exclude_rect_top
;	exclude_rect_bottom, exclude_rect_valid
;
;	RETURNS:
;	carry	CLEAR no exclusion needed
;		SET   exclusion needed

PUBLIC	swcursor_exclude_test
swcursor_exclude_test	PROC	NEAR

	cmp	exclude_global,1
	je	exclude_needed

	cmp	exclude_rect_valid,0		;if rect is not valid, then
	je	no_exclude			; return with carry clear

	mov	ax,save_screen_x		;if left edge of cursor >
	sub	ax,swcursor.csHotX		; right edge of exclude_rect
	cmp	ax,exclude_rect_right		; no exclusion needed
	jg	no_exclude

	add	ax,20H				;if right edge of cursor + 1 <=
	cmp	ax,exclude_rect_left		; left edge of exclude rect
	jle	no_exclude			; no exclusion needed

exclude_check_y:
	mov	ax,save_screen_y		;if top edge of cursor >
	sub	ax,swcursor.csHotY		; bottom edge of exclude_rect
	cmp	ax,exclude_rect_bottom		; no y overlap
	jg	no_exclude

	add	ax,20H				;if bottom edge of cursor + 1 >
	cmp	ax,exclude_rect_top		; top edge of exclude rect
	jle	no_exclude

exclude_needed:
	stc
	ret

no_exclude:
	clc
	ret

swcursor_exclude_test	ENDP




;	swcursor_save_screen
;
;	This routine copies a rectangular area of the screen into a static
;	local buffer. The rectangular area cannot be greater than 32x32 = 1024
;	total bytes.
;	PARMS:
;	ds	Data segment

PUBLIC	swcursor_save_screen
swcursor_save_screen	PROC	NEAR

	push	ds
	push	ds

	call	swcursor_setaddr
	mov	si,save_screen_address
        pop     es
	lea	di,save_screen_buffer
	mov	dx,save_screen_bank
	mov	bx,save_screen_height
        mov     cx,save_screen_width
        mov     ax,ScreenSelector
        mov     ds,ax

swcursor_save_screen_loop:
	push	si
	push	cx
	shr	cx,1
	rep	movsw
	rcl	cx,1
	rep	movsb
	pop	cx
	pop	si
	add	si,MEMORY_WIDTH
	jnc	@F
	inc	dx
	call	far_set_both_pages

@@:	dec	bx
	jne	swcursor_save_screen_loop
	pop	ds
	ret

swcursor_save_screen	ENDP



;	swcursor_restore_screen
;
;	This routine copies a portion of the save screen buffer back onto
;	the screen.
;	PARMS:
;	ds	Data segment

PUBLIC	swcursor_restore_screen
swcursor_restore_screen PROC	NEAR

	mov	dx,save_screen_bank
	call	far_set_both_pages

	lea	si,save_screen_buffer
	mov	ax,ScreenSelector
	mov	es,ax
	mov	di,save_screen_address
        mov     bx,save_screen_height

swcursor_restore_screen_loop:
	push	di
	mov	cx,save_screen_width
        shr     cx,1
	rep	movsw
	rcl	cx,1
	rep	movsb
	pop	di
	add	di,MEMORY_WIDTH
	jnc	@F
	inc	dx
	call	far_set_both_pages

@@:	dec	bx
	jne	swcursor_restore_screen_loop
	ret

swcursor_restore_screen ENDP



;	swcursor_draw
;
;	This routine draws the cursor onto the screen at the x,y coordinates
;	cursor_xdraw, cursor_ydraw.
;	PARMS:
;	ds	Data segment
;	cursor_xdraw, cursor_ydraw

PUBLIC	swcursor_draw
swcursor_draw	PROC	NEAR

	cld
	mov	dx,save_screen_bank
	call	far_set_both_pages

	lea	si,swcursor_data
	mov	ax,save_screen_yclip		;clip top of cursor data
	shl	ax,5
	add	ax,save_screen_xclip		;clip right of cursor data
	shl	ax,1
	add	si,ax
	mov	ax,ScreenSelector
	mov	es,ax
	mov	di,save_screen_address
        mov     bx,save_screen_height

swcursor_draw_loop:
        push    di
	push	si
	mov	cx,save_screen_width

@@:	lodsw						;ah:al = XOR:AND mask
	and	al,es:[di]
	xor	al,ah
	stosb
	loop	@B

	pop	si					;goto next row of
	add	si,40H					; cursor data

        pop     di
	add	di,MEMORY_WIDTH
	jnc	@F
	inc	dx
	call	far_set_both_pages

@@:	dec	bx
	jne	swcursor_draw_loop
        ret

swcursor_draw	ENDP




;       swcursor_setaddr
;
;	This routine returns the screen address of the upper left corner of
;	the upper left corner of the DISPLAYED cursor. It also sets the src
;	and dst bank and the page correctly. Since the cursor needs to be
;	clipped at all the screen edges, this routine also returns the
;	displayed width and height of the cursor.
;	PARMS:
;	ds	Data segment
;	cursor_xdraw, cursor_ydraw
;
;	RETURNS:
;	save_screen_xclip	# of pixels clipped off LEFT edge of cursor
;	save_screen_yclip	# of pixels clipped off TOP edge of cursor
;	save_screen_address	address of upper left corner of cursor
;	save_screen_width	width in pixels of displayed cursor
;	save_screen_height	height in pixels of displayed cursor

PUBLIC	swcursor_setaddr
swcursor_setaddr	PROC	NEAR

	mov	si,20H
	mov	save_screen_xclip,0
	mov	cx,save_screen_x
	sub	cx,swcursor.csHotX
	jns	swcursor_clip_right
	sub	save_screen_xclip,cx
	add	si,cx
	sub	cx,cx

swcursor_clip_right:
	mov	ax,VScreen_Width
        sub     ax,20H
	cmp	ax,cx
	jns	swcursor_clip_top
	add	si,ax
	sub	si,cx

swcursor_clip_top:
	mov	di,20H
	mov	dx,save_screen_y
	mov	save_screen_yclip,0
	sub	dx,swcursor.csHotY
	jns	swcursor_clip_bottom
	sub	save_screen_yclip,dx
	add	di,dx
	sub	dx,dx

swcursor_clip_bottom:
	mov	ax,VScreen_Height
	sub	ax,20H
	cmp	ax,dx
	jns	@F
	add	di,ax
	sub	di,dx

@@:	mov	ax,MEMORY_WIDTH
	mul	dx
	add	ax,cx
	mov	save_screen_address,ax
	mov	save_screen_bank,dx
	call	far_set_both_pages
	mov	save_screen_width,si
	mov	save_screen_height,di
	ret

swcursor_setaddr	ENDP

sEnd    Code

END


