	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	BUILDSTR.ASM
;
;   This module contains the strblt functions which build
;   up data on the stack for the actual output routines
;
; Revised: 29-Jan-1990  Ray Patrick [raypat]
;   Bug fix #1096.  Fixed negative text justification problem.
;   After label wcc_neg_dda, 'add' should have been a 
;   'sub' instruction.
;
; Create   17-Apr-1987
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1987 Microsoft Corporation
;
; Exported Functions:	!!!
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;
.286c

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
        include strmem.inc
	include rlfntseg.inc
	.list


	externNP	comp_byte_interval
	externNP	worst_case_ext
	externNP	output_o_rect
	externNP	preset_pro_text
	externA 	stack_top	;Stack probe location


createSeg   _REAL, rCode, word, public, CODE
sBegin	rCode
assumes cs,rCode
assumes ss,StrStuff
page

;---------------------------Public-Routine------------------------------;
; build_string
;
;   build_string builds up data on the stack consisting of character
;   offsets and widths, then invokes the routine which processes
;   this data.
;
; Entry:
;	stack frame as per strblt
; Returns:
;	none
; Error Returns:
;	none
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
; History:
;	Wed 06-May-1987 16:51:42 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing
assumes ss,StrStuff


define_frame build_string		;Define strblt's frame

cBegin	<nogen>
cEnd	<nogen>


real_build_string proc near

	mov	ax,ss:stack_top 	;Compute minimum allowable SP
	add	ax,STACK_SLOP
	mov	min_stack,ax

	mov	ax,text_bbox.left	;Set up text bbox to be a null interval
	mov	text_bbox.right,ax	;  We'll grow it as we progress
	mov	current_lhs,ax		;Need to init this for interval calc

	mov	al,accel		;Save the actual state of the
	mov	bl,al			;  IS_OPAQUE flag since the
	and	al,IS_OPAQUE		;  worst case code may alter
	mov	wc_flags,al		;  it
	mov	ah,excel		;Only set text visible once a string
	and	ah,not TEXT_VISIBLE	; is actually displayed
	mov	excel,ah

	mov	cx,rCodeOFFSET non_justified_text
	test	bl,WEIRD_SPACING
	jz	build_have_proc
	mov	cx,rCodeOFFSET justified_text
	test	bl,NEG_SPACING+HAVE_WIDTH_VECT
	jz	build_have_proc
	test	bl,IS_OPAQUE		;If transparent mode, then no
	jz	build_worst_ok		;  special work is required
	call	prepare_for_overlap	;Massive preprocessing required

build_worst_ok:
	mov	cx,rCodeOFFSET worst_case
	mov	bl,accel
	mov	ah,excel

build_have_proc:
	mov	build_proc,cx		;Save build routine address
	call	preset_pro_text 	;Set frame/ss: locations as needed

build_restart:
	mov	clear_stack,sp		;Used to clean the stack on exit
	mov	ax,sp			;Save buffer start pointer
	dec	ax
	dec	ax
	mov	buffer,ax

;-----------------------------------------------------------------------;
;
;	The routines which push text onto the stack expect the
;	following registers to be set on entry:
;
;		DS:SI --> current character in the string
;		ES:    =  font segment
;		AL     =  excel flags
;		AH     =  accel flags
;		CX     =  number of characters in the string
;		DI     =  current X position (and therefore starting x)
;
;-----------------------------------------------------------------------;
public	SetupBuildString
SetupBuildString:
	lds	si,lp_string
	assumes ds,nothing

	mov	es,seg_lp_font
	assumes es,FontSeg

	mov	al,excel
	mov	ah,accel
	mov	cx,count
	mov	di,x
	jmp	build_proc

build_ret_addr: 			;build routines return here
	mov	count,cx		;Save count
	mov	x,ax			;Save next char's start
	mov	off_lp_string,si	;Save next character
	call	pad_right_hand_side	;Fill to byte boundary if possible
	mov	current_rhs,di		;Save rhs
	mov	bx,di			;Compute the interval data
	mov	dx,current_lhs
	call	comp_byte_interval
	jc	build_all_done		;If nothing shows, we're done
	mov	left_clip_mask,al
	mov	right_clip_mask,ah
	mov	inner_byte_count,si
	mov	scan_start,di
	call	process_stack_data	;Display the string
	mov	sp,clear_stack
	or	excel,TEXT_VISIBLE	;Show something was displayed


;-----------------------------------------------------------------------;
;
;	If there is an opaquing rectangle, we must update the text
;	bounding box so that it won't overwrite the text we just
;	displayed when the rectangle is output.  IN transparent mode,
;	OPAQUE_RECT would have been cleared after the rectangle was
;	drawn and before we were called.
;
;-----------------------------------------------------------------------;

	test	excel,OPAQUE_RECT
	jz	build_no_o_rect
	mov	ax,current_lhs
	mov	bx,text_bbox.left
	min_ax	bx
	mov	text_bbox.left,ax
	mov	ax,current_rhs
	mov	bx,text_bbox.right
	max_ax	bx
	mov	text_bbox.right,ax

build_no_o_rect:
	mov	cx,count		;If no more characters
	jcxz	build_restore_opaque	;  go home, have dinner, sleep


;-----------------------------------------------------------------------;
;
;	Prepare for the next string.  If in opaque mode, the
;	CLIPPED_LEFT flag will have to be set so that we don't
;	try padding the lhs.  If in transparent mode, because
;	of stepping backwards, we might actually be clipped
;	anyway, so we'll have to test for thsi also.
;
;	FIRST_IN_PREV must be cleared.	It will be set if the
;	clipping code determines it needs to be.
;
;-----------------------------------------------------------------------;

	mov	bl,excel		;Will be changing flags in here
	and	bl,not (CLIPPED_LEFT+FIRST_IN_PREV)
	mov	di,x
	mov	ax,di			;Assume start will be current_lhs
	test	accel,IS_OPAQUE 	;When opaque, must clip on
	jnz	build_clip_next_time	;  a restart
	cmp	di,clip.left
	jge	build_no_clip_next_time

build_clip_next_time:
	or	bl,CLIPPED_LEFT 	;Clipping is required
	mov	ax,clip.left		;  and clip.left for clipping check
	max_ax	di			;  (but only if start X > old clip
	mov	clip.left,ax		;   lhs)

build_no_clip_next_time:
	mov	excel,bl
	mov	current_lhs,ax		;Need to update lhs for interval calc
	jmp	build_restart		;Try next part of the string

build_all_done:
	mov	sp,clear_stack

build_restore_opaque:
	mov	al,wc_flags
	test	al,WC_SET_LR		;If we stepped backwards, we'll
	jz	build_really_done	;  have to restore the real lhs
	mov	bx,wc_opaque_lhs	;  and rhs incase we have an
	mov	text_bbox.left,bx	;  opaque rectangle
	mov	bx,wc_opaque_rhs
	mov	text_bbox.right,bx

build_really_done:
	and	al,IS_OPAQUE		;Restore IS_OPAQUE incase worst_case
	or	accel,al		;  code cleared it, else the opaque
	ret				;  rectangle may overwrite our text

real_build_string endp

page
;---------------------------Public-Routine------------------------------;
;
; non_justified_text
;
;   This is the simple case for proportional text.  No justification,
;   no width vector.  Just run the string.  If we run out of stack
;   space, then that portion of the string which fits will be displayed,
;   and we'll restart again after that.
;
;   spcl - simple, proportional, clip lhs
;   sfcl - simple, fixed pitch,  clip lhs
;   sc	 - simple, clip rhs
;
; Entry:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	AL     =  excel flags
;	AH     =  accel flags
;	CX     =  number of characters in the string
;	DI     =  current X position (and therefore starting x)
;	stack frame as per strblt
; Returns:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	CX     =  number of characters left in string
;	DI     =  string rhs
;	AX     =  next character's X
; Error Returns:
;	none
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;	None
; History:
;	Tue 05-May-1987 18:27:29 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,FontSeg
assumes ss,StrStuff


non_justified_text proc near

	test	al,CLIPPED_LEFT
	jz	sc_no_left_clipping	;No clipping needed on lhs
	mov	dx,clip.left		;Characters become visible here
	test	ah,FIXED_PITCH
	jz	spcl_next_char		;Proportional font


;-----------------------------------------------------------------------;
;
;	Fixed pitch, no justification, left hand clipping
;
;-----------------------------------------------------------------------;

	mov	bx,lfd.font_width	;Fixed pitch font.

sfcl_next_char:
	add	di,bx			;Does this character become visible?
	cmp	dx,di			;DX is clip.left
	jl	sfcl_current_is_visible ;This char is visible
	inc	si
	loop	sfcl_next_char		;See if next character

sfcl_no_chars_visible:
	jmp	build_ret_addr		;Return to caller

sfcl_current_is_visible:
	sub	di,bx			;Restore staring address of character
					;  and just fall into the proportional
					;  code which will handle everything

;-----------------------------------------------------------------------;
;
;	Proportional, no justification, left hand clipping
;
;-----------------------------------------------------------------------;

spcl_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	jbe	spcl_good_character
	mov	al,lfd.default_char	;Character was out of range

spcl_good_character:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	add	di,ax			;0 width chars won't change x position
	cmp	dx,di			;DX is clip.left
	jl	spcl_current_is_visible ;This char is visible

spcl_see_if_next:
	loop	spcl_next_char		;See if next character
	jmp	build_ret_addr		;Return to caller

spcl_current_is_visible:
	sub	di,ax			;Restore starting x of character
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping


;-----------------------------------------------------------------------;
;
;	Instead of incrementing the current position by 8 and
;	having to recover the real current position, we just
;	slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;

	sub	dx,di			;Compute bits until we're visible
	je	spcl_save_first 	;Starts on clip edge
	sub	dx,8			;Is current byte visible?
	jl	spcl_have_vis_start	;  Yes

spcl_step_clipped_char:
	sub	ax,8			;Shorten the width of the character
	add	di,8			;Move current position right
	add	bx,lfd.font_height	;Move to next column of character
	sub	dx,8			;Is current byte visible?
	jge	spcl_step_clipped_char	;  No


;-----------------------------------------------------------------------;
;
;	If the lhs of the clip region and the starting X of the
;	character are in different bytes, then the FIRST_IN_PREV
;	flag must be set.  Only a clipped character can set this
;	flag.
;
;-----------------------------------------------------------------------;

spcl_have_vis_start:
	mov	dx,clip.left
	xor	dx,di
	and	dx,not 7
	jz	spcl_save_first 	;In same byte
	or	excel,FIRST_IN_PREV


;-----------------------------------------------------------------------;
;
;	We have the start of the first character which is visible
;	Determine which loop (clipped/non-clipped) will process it.
;	We let the routine we're about to call push the character
;	since it will handle both right clipping (if needed) and
;	fat characters.
;
;-----------------------------------------------------------------------;

spcl_save_first:
	jmp	short scc_clip_enters_here


;-----------------------------------------------------------------------;
;
;	There was no left hand clipping.  Whenever this is the case,
;	we want to try and pad the lhs out to a byte boundary so that
;	full byte code can be used.
;
;-----------------------------------------------------------------------;

sc_no_left_clipping:
	call	pad_left_hand_side	;Might be able to pad lhs
	jmp	short scc_next_char


;-----------------------------------------------------------------------;
;
;	scc - simple case, rhs clipping.
;
;	This loop is used when it is possible for the character
;	to be clipped on the rhs.  lhs clipping has already
;	been performed.  There is no justification.
;
;	Currently:
;		DS:SI --> current character in the string
;		ES:    =  font segment
;		DI     =  current X position
;		CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;


scc_bad_char:
	mov	al,lfd.default_char	;Character was out of range,
	jmp	short scc_good_char

scc_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	ja	scc_bad_char

scc_good_char:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2			;Index into width/offset table
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	or	ax,ax			;If width is 0, ignore character
	jz	scc_see_if_next
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping

scc_clip_enters_here:
	mov	dx,di			;Compute phase
	and	dl,7
	add	di,ax			;DI = next char's X position
	cmp	di,clip.right
	jge	scc_char_is_clipped	;Clipped (or first pixel of next is)

scc_been_clipped:
	mov	dh,dl			;Need phase in DH
	cmp	ax,8			;If character is less than 8 bits
	jbe	scc_width_ok		;  wide, push it's data
	mov	dl,8			;Need 8 for size in DL

scc_still_wide:
	push	dx			;Push data showing phase,
	push	bx			;  character is 8 wide, then
	sub	ax,8			;  create another character
	add	bx,lfd.font_height	;  of the remaining width
	cmp	ax,8
	ja	scc_still_wide

scc_width_ok:
	mov	ah,dh
	push	ax			;Save width and phase
	push	bx			;Save offset to bits
	cmp	sp,min_stack		;Stack compare must be unsigned
	jl	scc_restart		;Not enough stack for another character

scc_see_if_next:
	loop	scc_next_char		;Until all characters pushed
	mov	ax,di			;Next character starts here
	jmp	build_ret_addr


;-----------------------------------------------------------------------;
;
;	This character is either clipped, or it's last pixel is
;	the last pixel which will fit within the clipping rhs.
;	Adjust it's width so it fits, set the remaining character
;	count to 1 so the loop will terminate, and reenter the
;	code where we came from.
;
;-----------------------------------------------------------------------;

scc_char_is_clipped:
	mov	cx,clip.right		;Compute number of pixels
	sub	di,cx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,cx			;Set new rhs
	mov	cx,1			;Show this as last character
	jmp	scc_been_clipped	;Finish here


;-----------------------------------------------------------------------;
;
;	These is no more space on the stack to build characters.
;	If this was the last character, then don't bother with the
;	restart.
;
;-----------------------------------------------------------------------;

scc_restart:
	dec	cx			;Adjust count for char just pushed
	mov	ax,di			;Next character starts here
	jmp	build_ret_addr

non_justified_text endp

page
;---------------------------Public-Routine------------------------------;
;
; justified_text
;
;   This is the justification case for text, when positive character
;   extra and/or a positive DDA are present.  If we run out of stack
;   space, then that portion of the string which fits will be displayed,
;   and we'll restart again after that.
;
;   jc	- justify clipped
;   jcl - justify clip left
;
; Entry:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	AL     =  excel flags
;	AH     =  accel flags
;	CX     =  number of characters in the string
;	DI     =  current X position (and therefore starting x)
;	stack frame as per strblt
; Returns:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	CX     =  number of characters left in string
;	DI     =  string rhs
;	AX     =  next character's X
; Error Returns:
;	none
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
; History:
;	Tue 05-May-1987 18:27:29 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,FontSeg
assumes ss,StrStuff


justified_text	proc	near

	test	excel,CLIPPED_LEFT
	jnz	jcl_next_char		;Clipping needed
	call	pad_left_hand_side	;Might be able to pad lhs
	jmp	jc_next_char


jcl_bad_character:
	mov	al,lfd.default_char	;Character was out of range
	jmp	short jcl_good_character


;-----------------------------------------------------------------------;
;
;	This is the code which runs the DDA to intersperse pixels
;	into the string
;
;	Compute the amount of white space that will be introduced by
;	this character.  This will be the sum of any character extra,
;	any break extra (if a break character), and dda interspersed
;	pixels (if a break character)
;
;-----------------------------------------------------------------------;

jcl_have_break_char:
	mov	bl,accel
	and	bl,DDA_NEEDED+HAVE_BRK_EXTRA
	jz	jcl_have_tot_extra	;Must have only been char extra
	add	dx,brk_extra		;Extra every break (0 if none)
	test	bl,DDA_NEEDED
	jz	jcl_have_tot_extra
	mov	bx,brk_err		;The dda is required for this char
	sub	bx,brk_rem		;  Run it and add in an extra pixel
	jg	jcl_dont_distribute	;  if needed.
	add	bx,brk_count		;Add one pixel for the dda
	inc	dx

jcl_dont_distribute:
	mov	brk_err,bx		;Save rem for next time
	jmp	short jcl_have_tot_extra


;-----------------------------------------------------------------------;
;
;	This is the code which computes the number of DDA interspersed
;	pixels to be added to the string
;
;	If all the extra pixels will fit on the end of this character,
;	just adjust it's width, otherwise a null character should be
;	created for the extra.
;
;-----------------------------------------------------------------------;

jcl_extra_pixels:
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	neg	ax
	and	ax,7			;AX = # extra pixels which will fit
	jz	jcl_have_entire_width	;None will fit
	cmp	ax,dx
	jl	jcl_have_what_fits	;Some extra pixels will not fit
	mov	ax,dx			;All pixels will fit, make DX = 0

jcl_have_what_fits:
	sub	dx,ax			;DX = extra for the dummy character

jcl_have_entire_width:
	add	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	jmp	short jcl_have_width


;-----------------------------------------------------------------------;
;
;	This is the start of the real loop for left hand clipping.
;
;-----------------------------------------------------------------------;

jcl_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	ja	jcl_bad_character

jcl_good_character:
	mov	dx,char_xtra		;Base amount of extra pixels needed
	cmp	al,lfd.break_char
	je	jcl_have_break_char	;Go compute dda added pixels

jcl_have_tot_extra:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2			;BX = index into offset/width table
	or	dx,dx
	jnz	jcl_extra_pixels	;Extra pixels required
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]

jcl_have_width:
	add	di,ax			;DI = next chars starting X
	cmp	clip.left,di
	jl	jcl_current_is_visible	;This char is visible
	add	di,dx
	cmp	clip.left,di
	jl	jcl_dummy_is_visible	;Dummy is first visible character

jcl_see_if_next:
	loop	jcl_next_char		;See if next character
	jmp	build_ret_addr		;Return to caller


;-----------------------------------------------------------------------;
;
;	The dummy character is the first character which became
;	visible.  Just set the starting X to clip.left, and shorten
;	the width of the dummy character appropriately.
;
;-----------------------------------------------------------------------;

jcl_dummy_is_visible:
	mov	dx,di
	mov	di,clip.left		;Starting X is clip.left
	sub	dx,di			;DX is # pixels in dummy
	xor	ax,ax			;Show no real character
	jmp	short jcl_all_done


;-----------------------------------------------------------------------;
;
;	We just encountered the first character which will be visible
;	Clip it on the lhs as needed.
;
;-----------------------------------------------------------------------;

jcl_current_is_visible:
	sub	di,ax			;Restore starting x of character
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping


;-----------------------------------------------------------------------;
;
;	Instead of incrementing the current position by 8 and
;	having to recover the real current position, we just
;	slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;

	push	dx			;Save extra pixels
	mov	dx,clip.left
	sub	dx,di			;Compute bits until we're visible
	je	jcl_save_first		;Starts on clip edge
	sub	dx,8			;Is current byte visible?
	jl	jcl_have_vis_start	;  Yes

jcl_step_clipped_char:
	sub	ax,8			;Shorten the width of the character
	add	di,8			;Move current position right
	add	bx,lfd.font_height	;Move to next column of character
	sub	dx,8			;Is current byte visible?
	jge	jcl_step_clipped_char	;  No

;-----------------------------------------------------------------------;
;
;	If the lhs of the clip region and the starting X of the
;	character are in different bytes, then the FIRST_IN_PREV
;	flag must be set.  Only a clipped character can set this
;	flag.
;
;-----------------------------------------------------------------------;

jcl_have_vis_start:
	mov	dx,clip.left
	xor	dx,di
	and	dx,not 7
	jz	jcl_save_first		;In same byte
	or	excel,FIRST_IN_PREV

;-----------------------------------------------------------------------;
;
;	We have the start of the first character which is visible
;	We let the routine we're about to call push the character
;	since it will handle both right clipping (if needed) and
;	fat characters.
;
;-----------------------------------------------------------------------;


jcl_save_first:
	pop	dx			;Restore extra pixels

jcl_all_done:
	jmp	short jc_clip_enters_here



;-----------------------------------------------------------------------;
;
;	jc - justified with clipping
;
;	This loop is used for justified text.  It will perform
;	rhs clipping.  lhs clipping has already been performed.
;
;	Currently:
;		DS:SI --> current character in the string
;		ES:    =  font segment
;		DI     =  current X position
;		CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;


jc_bad_char:
	mov	al,lfd.default_char	;Character was out of range,
	jmp	short jc_good_character


;-----------------------------------------------------------------------;
;
;	This is the code which runs the DDA to intersperse pixels
;	into the string
;
;	Compute the amount of white space that will be introduced by
;	this character.  This will be the sum of any character extra,
;	any break extra (if a break character), and dda interspersed
;	pixels (if a break character)
;
;-----------------------------------------------------------------------;

jc_have_break_char:
	mov	bl,accel
	and	bl,DDA_NEEDED+HAVE_BRK_EXTRA
	jz	jc_have_tot_extra	;Must have only been char extra
	add	dx,brk_extra		;Extra every break (0 if none)
	test	bl,DDA_NEEDED
	jz	jc_have_tot_extra
	mov	bx,brk_err		;The dda is required for this char
	sub	bx,brk_rem		;  Run it and add in an extra pixel
	jg	jc_dont_distribute	;  if needed.
	add	bx,brk_count		;Add one pixel for the dda
	inc	dx

jc_dont_distribute:
	mov	brk_err,bx		;Save rem for next time
	jmp	short jc_have_tot_extra


;-----------------------------------------------------------------------;
;
;	If all the extra pixels will fit on the end of this character,
;	just adjust it's width, otherwise a null character should be
;	created for the extra.
;
;-----------------------------------------------------------------------;

jc_extra_pixels:
	neg	ax
	and	ax,7			;AX = # extra pixels which will fit
	jz	jc_have_entire_width	;None will fit
	cmp	ax,dx
	jl	jc_have_what_fits	;Some extra pixels will not fit
	mov	ax,dx			;All pixels will fit, make DX = 0

jc_have_what_fits:
	sub	dx,ax			;DX = extra for the dummy character

jc_have_entire_width:
	add	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	jmp	short jc_have_width


;-----------------------------------------------------------------------;
;
;	This is the start of the real loop
;
;-----------------------------------------------------------------------;

jc_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	ja	jc_bad_char

jc_good_character:
	mov	dx,char_xtra		;Base amount of extra pixels needed
	cmp	al,lfd.break_char
	je	jc_have_break_char	;Go compute dda added pixels

jc_have_tot_extra:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2			;BX = index into offset/width table
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	or	dx,dx
	jnz	jc_extra_pixels 	;Extra pixels required

jc_have_width:
	or	ax,ax
	jz	jc_check_dummy		;If width is 0, might still have dummy
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping

jc_clip_enters_here:
	mov	num_null_pixels,dx	;Save # null pixels
	mov	dx,di			;Compute phase
	and	dl,7
	add	di,ax			;DI = next char's X position
	cmp	di,clip.right
	jge	jc_char_is_clipped	;Clipped (or first pixel of next is)

jc_been_clipped:
	mov	dh,dl			;Need phase in DH
	mov	dl,8			;Need 8 for size in DL if fat
	cmp	ax,8			;If character is less than 8 bits
	jbe	jc_width_ok		;  wide, push it's data

jc_still_wide:
	push	dx			;Push data showing phase,
	push	bx			;  character is 8 wide, then
	sub	ax,8			;  create another character
	add	bx,lfd.font_height	;  of the remaining width
	cmp	ax,8
	ja	jc_still_wide

jc_width_ok:
	mov	ah,dh
	push	ax			;Save width and phase
	push	bx			;Save offset to bits
	mov	dx,num_null_pixels

jc_check_dummy:
	or	dx,dx
	jz	jc_see_if_next		;No pixels for justification
	xchg	ax,dx			;Set ax = number of pixels to fill
	mov	bx,null_char_offset
	mov	dx,di			;Compute phase
	and	dl,7
	add	di,ax			;DI = next char's X position
	cmp	di,clip.right
	jge	jc_dummy_is_clipped	;Clipped (or first pixel of next is)

jc_dummys_been_clipped:
	mov	dh,dl			;Need phase in DH
	mov	dl,8			;Need 8 for size in DL if fat
	cmp	ax,8			;If dummy is less than 8 bits
	jbe	jc_dummy_width_ok	;  wide, push it's data

jc_dummy_still_wide:
	push	dx			;Push data showing phase,
	push	bx			;  character is 8 wide, then
	sub	ax,8			;  create another character
	cmp	ax,8
	ja	jc_dummy_still_wide

jc_dummy_width_ok:
	mov	ah,dh
	push	ax			;Save width and phase
	push	bx			;Save offset to bits

jc_see_if_next:
	cmp	sp,min_stack		;Stack compare must be unsigned
	jl	jc_restart		;Not enough stack for another character
	dec	cx
	jle	jc_all_done
	jmp	jc_next_char		;Until all characters pushed

jc_all_done:
	mov	ax,di			;Next character starts here
	jmp	build_ret_addr


;-----------------------------------------------------------------------;
;
;	This character is either clipped, or it's last pixel is
;	the last pixel which will fit within the clipping rhs.
;	Adjust it's width so it fits, set the remaining character
;	count to 1 so the loop will terminate, and reenter the
;	code where we came from.
;
;	Might as well set num_null_pixels to zero to skip that code.
;
;-----------------------------------------------------------------------;

jc_char_is_clipped:
	mov	cx,clip.right		;Compute number of pixels
	sub	di,cx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,cx			;Set new rhs
	xor	cx,cx			;Dont't want any extra pixels
	mov	num_null_pixels,cx
	inc	cx			;Show this as last character
	jmp	jc_been_clipped 	;Finish here


;-----------------------------------------------------------------------;
;
;	The dummy is either clipped, or it's last pixel is
;	the last pixel which will fit within the clipping rhs.
;	Adjust it's width so it fits, set the remaining character
;	count to 1 so the loop will terminate, and reenter the
;	code where we came from.
;
;-----------------------------------------------------------------------;

jc_dummy_is_clipped:
	mov	cx,clip.right		;Compute number of pixels
	sub	di,cx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,cx			;Set new rhs
	mov	cx,1			;Show this as last character
	jmp	jc_dummys_been_clipped	;Finish here


;-----------------------------------------------------------------------;
;
;	These is no more space on the stack to build characters.
;	If this was the last character, then don't bother with the
;	restart.
;
;-----------------------------------------------------------------------;

jc_restart:
	dec	cx			;Adjust count for char just pushed
	mov	ax,di			;Next character starts here
	jmp	build_ret_addr

justified_text	endp

page
;---------------------------Public-Routine------------------------------;
;
; worst_case
;
;   This is the worst case text code, when there is some combination
;   of the width vector, negative character extra, and negative dda.
;   If we step backwards or we run out of stack space, then that
;   whatever has been built up on the stack will be displayed, and
;   we'll restart again after that.
;
;   wcc - worse case clipped
;   wccl - worse case clip left
;
; Entry:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	AL     =  excel flags
;	AH     =  accel flags
;	CX     =  number of characters in the string
;	DI     =  current X position (and therefore starting x)
;	stack frame as per strblt
; Returns:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	CX     =  number of characters left in string
;	DI     =  string rhs
;	AX     =  next character's X
; Error Returns:
;	none
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
; History:
;	Tue 05-May-1987 18:27:29 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,FontSeg
assumes ss,StrStuff

worst_case	proc near

	test	excel,CLIPPED_LEFT
	jnz	wccl_next_char		;Clipping needed
	call	pad_left_hand_side	;Might be able to pad lhs
	jmp	wcc_next_char


;-----------------------------------------------------------------------;
;
;	Set current character to the default character
;
;-----------------------------------------------------------------------;

wccl_bad_character:
	mov	al,lfd.default_char
	jmp	short wccl_good_character


;-----------------------------------------------------------------------;
;
;	This runs the DDA to intersperse pixels into the string
;
;	Compute the adjustment to the character's width.  This will
;	be the sum of any character extra, any break extra (if a
;	break character), and dda interspersed pixels (if a break
;	character)
;
;	The dda must be capable of handling positive and negative
;	justification.	Character extra may be negative.
;
;-----------------------------------------------------------------------;

wccl_have_break_char:
	mov	bl,accel
	and	bl,DDA_NEEDED+HAVE_BRK_EXTRA
	jz	wccl_have_tot_extra	;Must have only been char extra
	add	dx,brk_extra		;Extra every break (0 if none)
	test	bl,DDA_NEEDED
	jz	wccl_have_tot_extra

	mov	bx,brk_rem		;If the dda is stepping left instead
	or	bx,bx			;  of stepping right, then brk_rem
	jl	wccl_neg_dda		;  will be negative
	sub	brk_err,bx		;Run dda and add in an extra pixel
	jg	wccl_have_tot_extra	;  if needed.
	mov	bx,brk_count
	add	brk_err,bx
	inc	dx			;Add one pixel for the dda
	jmp	short wccl_have_tot_extra

wccl_neg_dda:
	add	brk_err,bx		;Run dda and subtract an extra pixel
	jl	wccl_have_tot_extra	 ;  if needed.
	mov	bx,brk_count
	sub	brk_err,bx		;Subtract one pixel for the dda
	dec	dx
	jmp	short wccl_have_tot_extra



;-----------------------------------------------------------------------;
;
;	This is the start of the real loop for left hand clipping.
;
;-----------------------------------------------------------------------;

wccl_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	ja	wccl_bad_character

wccl_good_character:
	mov	dx,char_xtra		;Base amount of extra pixels needed
	cmp	al,lfd.break_char
	je	wccl_have_break_char	;Go compute dda added pixels

wccl_have_tot_extra:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2			;BX = index into offset/width table
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	push	bx			;Must save character index
	test	accel,HAVE_WIDTH_VECT
	jz	wccl_have_width

wccl_get_user_width:
	les	bx,lp_dx
	assumes es,nothing
	add	dx,wptr es:[bx] 	;DX is delta to next char's start
	inc	bx
	inc	bx
	mov	off_lp_dx,bx
	mov	es,seg_lp_font
	assumes es,FontSeg
	sub	dx,ax			;Compute adjustment to width


;-----------------------------------------------------------------------;
;
;	DX contains any adjustment to the next characters starting
;	position.  If this number is negative, then we'll be stepping
;	over the previous character.
;
;	We don't allow a character to start before the previous
;	character.  We'll have to enforce this at this point by
;	making sure that any negative adjustment is less than or
;	equal to the character width.
;
;-----------------------------------------------------------------------;

wccl_have_width:
	or	dx,dx
	jge	wccl_adjustment_ok
	add	dx,ax			;Don't allow the backwards step
	sbb	bx,bx			;  to go beyond the start of this
	and	dx,bx			;  character
	sub	dx,ax

wccl_adjustment_ok:
	pop	bx			;Restore char index
	add	ax,di			;AX = rhs of character
	cmp	clip.left,ax
	jl	wccl_current_is_visible ;Some part of char is visible


;-----------------------------------------------------------------------;
;
;	If the adjustment for the character width is greater than the
;	actual width of the character, then it is possible that the
;	dummy pixels could become visible.  If the adjustment for the
;	character width is less than the actual character width, then
;	the dummy pixel (negative dummy pixels?) cannot become visible.
;
;-----------------------------------------------------------------------;

	add	ax,dx			;Next char starts at AX
	mov	di,clip.left
	cmp	di,ax
	jl	wccl_dummy_is_visible	;Some part of dummy char became visible
	xchg	di,ax			;Set start of next character

wccl_see_if_next:
	loop	wccl_next_char		;See if next character
	jmp	build_ret_addr		;Return to caller



;-----------------------------------------------------------------------;
;
;	The dummy character is the first character which became
;	visible.  Just set the starting X to clip.left, and shorten
;	the width of the dummy character appropriately.
;
;-----------------------------------------------------------------------;

wccl_dummy_is_visible:
	xchg	ax,dx			;Set DX = # pixels in dummy
	sub	dx,di
	xor	ax,ax			;Show no real character
	jmp	short wccl_all_done


;-----------------------------------------------------------------------;
;
;	So here we are, we have a character which will become visible,
;	and possibly have some adjustment to the character.
;
;	Our registers currently contain:
;
;		AX = rhs of character
;		BX = index into offset/width table
;		CX = # of characters left in the string
;		DX = # of extra pixels (zero or positive)
;		DI = starting X offset
;		ES = FontSeg
;		DS:SI --> string
;
;-----------------------------------------------------------------------;


wccl_current_is_visible:
	sub	ax,di			;Restore width of the character
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping


;-----------------------------------------------------------------------;
;
;	Instead of incrementing the current position by 8 and
;	having to recover the real current position, we just
;	slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;

	push	dx			;Save extra pixels to be added
	mov	dx,clip.left
	sub	dx,di			;Compute bits until we're visible
	je	wccl_save_first 	;Starts on clip edge
	sub	dx,8			;Is current byte visible?
	jl	wccl_have_vis_start	;  Yes

wccl_step_clipped_char:
	sub	ax,8			;Shorten the width of the character
	add	di,8			;Move current position right
	add	bx,lfd.font_height	;Move to next column of character
	sub	dx,8			;Is current byte visible?
	jge	wccl_step_clipped_char	;  No


;-----------------------------------------------------------------------;
;
;	If the lhs of the clip region and the starting X of the
;	character are in different bytes, then the FIRST_IN_PREV
;	flag must be set.  Only a clipped character can set this
;	flag.
;
;-----------------------------------------------------------------------;

wccl_have_vis_start:
	mov	dx,clip.left
	xor	dx,di
	and	dx,not 7
	jz	wccl_save_first 	;In same byte
	or	excel,FIRST_IN_PREV


;-----------------------------------------------------------------------;
;
;	We have the start of the first character which is visible.
;	We let the routine we're about to call push the character
;	since it will handle both right clipping (if needed) and
;	fat characters.
;
;-----------------------------------------------------------------------;

wccl_save_first:
	pop	dx			;Restore extra pixels

wccl_all_done:
	jmp	wcc_clip_enters_here


;-----------------------------------------------------------------------;
;
;	wcc - worse case, with rhs clipping
;
;	Currently:
;		DS:SI --> current character in the string
;		ES:    =  font segment
;		DI     =  current X position
;		CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;


wcc_bad_char:
	mov	al,lfd.default_char	;Character was out of range,
	jmp	short wcc_good_character


;-----------------------------------------------------------------------;
;
;	This is the code which runs the DDA to intersperse pixels
;	into the string
;
;	Compute the adjustment to the character's width.  This will
;	be the sum of any character extra, any break extra (if a
;	break character), and dda interspersed pixels (if a break
;	character)
;
;	The dda must be capable of handling positive and negative
;	justification.	Character extra may be negative.
;
;-----------------------------------------------------------------------;

wcc_have_break_char:
	mov	bl,accel
	and	bl,DDA_NEEDED+HAVE_BRK_EXTRA
	jz	wcc_have_tot_extra	;Must have only been char extra
	add	dx,brk_extra		;Extra every break (0 if none)
	test	bl,DDA_NEEDED
	jz	wcc_have_tot_extra

	mov	bx,brk_rem		;If the dda is stepping left instead
	or	bx,bx			;  of stepping right, then brk_rem
	jl	wcc_neg_dda		;  will be negative
	sub	brk_err,bx		;Run dda and add in an extra pixel
	jg	wcc_have_tot_extra	;  if needed.
	mov	bx,brk_count
	add	brk_err,bx
	inc	dx			;Add one pixel for the dda
	jmp	short wcc_have_tot_extra

wcc_neg_dda:
	sub	brk_err,bx		;Run dda and subtract an extra pixel
	jg	wcc_have_tot_extra	;  if needed.
	mov	bx,brk_count
	add	brk_err,bx
	dec	dx			;Subtract one pixel for the dda
	jmp	short wcc_have_tot_extra


;-----------------------------------------------------------------------;
;
;	This is the start of the real loop for right hand clipping.
;
;-----------------------------------------------------------------------;

wcc_next_char:
	lodsb
	sub	al,lfd.first_char
	cmp	al,lfd.last_char
	ja	wcc_bad_char

wcc_good_character:
	mov	dx,char_xtra		;Base amount of extra pixels needed
	cmp	al,lfd.break_char
	je	wcc_have_break_char	;Go compute dda added pixels

wcc_have_tot_extra:
	xor	ah,ah
	xchg	ax,bx
	shiftl	bx,2			;BX = index into offset/width table
	mov	ax,wptr fsCharOffset[bx][PROP_WIDTH]
	mov	bx,wptr fsCharOffset[bx][PROP_OFFSET]
	add	bx,amt_clipped_on_top	;Adjust pointer for any clipping
	push	bx
	test	accel,HAVE_WIDTH_VECT
	jz	wcc_have_width

wcc_get_users_width:
	les	bx,lp_dx
	assumes es,nothing
	add	dx,wptr es:[bx] 	;DX is delta to next char's start
	inc	bx
	inc	bx
	mov	off_lp_dx,bx
	mov	es,seg_lp_font
	assumes es,FontSeg
	sub	dx,ax			;Compute adjustment to width


;-----------------------------------------------------------------------;
;
;	DX contains any adjustment to the next characters starting
;	position.  If this number is negative, then we'll be stepping
;	over the previous character.
;
;	We don't allow a character to start before the previous
;	character.  We'll have to enforce this at this point by
;	making sure that any negative adjustment is less than or
;	equal to the character width.
;
;-----------------------------------------------------------------------;

wcc_have_width:
	or	dx,dx
	jge	wcc_adj_is_ok
	add	dx,ax			;Don't allow the backwards step
	sbb	bx,bx			;  to go beyond the start of this
	and	dx,bx			;  character
	sub	dx,ax

wcc_adj_is_ok:
	pop	bx			;Restore bits offset

wcc_clip_enters_here:
	or	ax,ax			;If character width is 0,
	jz	wcc_check_dummy 	;  might still have dummy char
	or	dx,dx			;Any adjustment to the width?
	jle	wcc_have_adj_width	;No extra pixels to add
	push	bx
	mov	bx,ax			;  into the empty space of the
	neg	ax			;  character
	and	ax,7			;AX = # extra pixels which will fit
	jz	wcc_have_entire_width	;None will fit
	cmp	ax,dx
	jl	wcc_have_what_fits	;Some extra pixels will not fit
	mov	ax,dx			;All pixels will fit, make DX = 0

wcc_have_what_fits:
	sub	dx,ax			;DX = extra for the dummy character

wcc_have_entire_width:
	add	ax,bx			;Set number of pixels to use in char
	pop	bx

wcc_have_adj_width:
	mov	num_null_pixels,dx	;Save number of dummy pixels
	mov	dx,di			;Compute phase
	and	dl,7
	add	di,ax			;DI = next char's X position
	cmp	di,clip.right
	jge	wcc_char_is_clipped	;Clipped (or first pixel of next is)

wcc_been_clipped:
	mov	dh,dl			;Need phase in DH
	mov	dl,8			;Need 8 for size in DL if fat
	cmp	ax,8			;If character is less than 8 bits
	jbe	wcc_width_ok		;  wide, push it's data

wcc_still_wide:
	push	dx			;Push data showing phase,
	push	bx			;  character is 8 wide, then
	sub	ax,8			;  create another character
	add	bx,lfd.font_height	;  of the remaining width
	cmp	ax,8
	ja	wcc_still_wide

wcc_width_ok:
	mov	ah,dh
	push	ax			;Save width and phase
	push	bx			;Save offset to bits
	mov	dx,num_null_pixels

wcc_check_dummy:
	xchg	ax,dx			;Just incase we go backwards
	or	ax,ax
	jz	wcc_see_if_next 	;No pixels for justification
	jl	wcc_going_back
	mov	bx,null_char_offset
	mov	dx,di			;Compute phase
	and	dl,7
	add	di,ax			;DI = next char's X position
	cmp	di,clip.right
	jge	wcc_dummy_is_clipped	;Clipped (or first pixel of next is)

wcc_dummys_been_clipped:
	mov	dh,dl			;Need phase in DH
	mov	dl,8			;Need 8 for size in DL if fat
	cmp	ax,8			;If dummy is less than 8 bits
	jbe	wcc_dummy_width_ok	;  wide, push it's data

wcc_dummy_still_wide:
	push	dx			;Push data showing phase,
	push	bx			;  character is 8 wide, then
	sub	ax,8			;  create another character
	cmp	ax,8
	ja	wcc_dummy_still_wide

wcc_dummy_width_ok:
	mov	ah,dh
	push	ax			;Save width and phase
	push	bx			;Save offset to bits

wcc_see_if_next:
	cmp	sp,min_stack		;Stack compare must be unsigned
	jb	wcc_restart		;Not enough stack for another character
	dec	cx
	jle	wcc_all_done
	jmp	wcc_next_char		;Until all characters pushed

wcc_all_done:
	mov	ax,di			;Next character starts here
	jmp	build_ret_addr



;-----------------------------------------------------------------------;
;
;	The dummy is either clipped, or it's last pixel is
;	the last pixel which will fit within the clipping rhs.
;	Adjust it's width so it fits, set the remaining character
;	count to 1 so the loop will terminate, and reenter the
;	code where we came from.
;
;	If the width adjustment was negative, we would never have
;	reached this code, so ignore any restart.
;
;-----------------------------------------------------------------------;

wcc_dummy_is_clipped:
	mov	cx,clip.right		;Compute number of pixels
	sub	di,cx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,cx			;Set new rhs
	mov	cx,1			;Show this as last character
	jmp	wcc_dummys_been_clipped ;Finish here



;-----------------------------------------------------------------------;
;
;	This character is either clipped, or it's last pixel is
;	the last pixel which will fit within the clipping rhs.
;
;	If there is a  negative adjustment to the width of the
;	character, it is possible that the next character could
;	be partially visible.  We'll have set up for a restart
;	if this is the case.
;
;	If this is the last character of the string, then there
;	is no problem
;
;	If no negative adjustment, adjust it's width so it fits,
;	set the remaining character count to 1 so the loop will
;	terminate, and reenter the code where we came from.
;
;-----------------------------------------------------------------------;

wcc_char_is_clipped:

	push	dx			;If num_null_pixels < 0, then
	mov	dx,num_null_pixels	;  a restart might be possible
	or	dx,dx
	jl	wcc_might_need_restart	;Might need a restart

wcc_clipped_no_restart:
	mov	cx,clip.right		;Compute number of pixels
	sub	di,cx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,cx			;Set new rhs
	xor	cx,cx			;Don't want any extra pixels
	mov	num_null_pixels,cx
	inc	cx			;Show this as last character
	pop	dx
	jmp	wcc_been_clipped	;Finish here


;-----------------------------------------------------------------------;
;
;	Might be looking a restart in the face.  Compute where the
;	next character would start, and if it is to the left of
;	clip.right, then a restart is needed.
;
;-----------------------------------------------------------------------;

wcc_might_need_restart:
	cmp	cx,1			;If last character
	jle	wcc_clipped_no_restart	;  then no restart needed
	add	dx,di			;Compute next starting x
	sub	dx,clip.right
	jge	wcc_clipped_no_restart	;Rest of string is clipped


;-----------------------------------------------------------------------;
;
;	Will have to force a restart for the next character.  We
;	can do this by computing the number of pixels between where
;	where the next character starts and clip.right.  This negative
;	number will be stuffed into num_null_pixels, so when we reenter
;	the main loop, we'll force a restart after pushing the character
;	data
;
;-----------------------------------------------------------------------;

	mov	num_null_pixels,dx
	mov	dx,clip.right		;Compute number of pixels
	sub	di,dx			;  which have to be clipped
	sub	ax,di			;Set new character width
	mov	di,dx			;Set new rhs
	pop	dx
	jmp	wcc_been_clipped	;Finish here


;-----------------------------------------------------------------------;
;
;	I have the unfortunate task of informing you that the next
;	character will start somewhere in the middle of the current
;	character.  If this is the last character of the string,
;	then nothing need be done.  If this isn't the last character,
;	then a restart will be needed.
;
;-----------------------------------------------------------------------;

wcc_going_back:
	add	ax,di			;Set position of next character
	dec	cx			;Adjust count for char just pushed
	jmp	build_ret_addr



;-----------------------------------------------------------------------;
;
;	Out of stack space.  Set up for a restart.
;
;-----------------------------------------------------------------------;

wcc_restart:
	mov	ax,di
	dec	cx			;Adjust count for char just pushed
	jmp	build_ret_addr

worst_case	endp

page
;--------------------------Private-Routine------------------------------;
; pad_left_hand_side
;
;   This routine is called when the text string isn't clipped on the
;   left side.	It attempts to pad the character with 0's if at all
;   possible.
;
;   If we can pad to the left of the first character with 0's, then
;   we can use the full byte code, which is many times faster than
;   the partial byte code.
;
;   If we do pad, we must update both current_lhs and the starting
;   X coordinate which will be used by the main loop code.
;
; Entry:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	CX     =  number of characters in the string
;	DI     =  current X position (and therefore starting x)
; Returns:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	CX     =  number of characters in the string
;	DI     =  current X position
; Error Returns:
;	None
; Registers Preserved:
;	CX,SI,DI,DS,ES,FLAGS
; Registers Destroyed:
;	AX,BX,DX
; Calls:
;	None
; History:
;	Thu 16-Apr-1987 23:37:27 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,FontSeg
assumes ss,StrStuff


pad_left_hand_side	proc near

	mov	ax,di			;Get starting x coordinate
	and	ax,7			;Address MOD 8 is amount to pad
	jz	plhs_all_done		;At byte boundary, don't need padding


;	If we're in transparent mode, we can always create a dummy
;	character of 0's for the left side to avoid any left clipping

	test	accel,IS_OPAQUE 	;If in transparent mode, we can
	jz	plhs_add_padding	;  always add the padding


;	In opaque mode.  If there is an opaquing rectangle, try and pad
;	the left side up to the byte boundary. If we cannot make it to
;	the byte boundary, go as far as we can so that the opaquing code
;	can skip the byte.
;
;	If there isn't an opaque rectangle, we cannot do any padding.

	mov	bl,excel
	not	bl
	test	bl,OPAQUE_RECT+BOUNDED_IN_Y
	jnz	plhs_all_done		;Cannot add any padding
	mov	bx,di
	sub	bx,o_rect.left
	jle	plhs_all_done		;Cannot add any.  Darn
	min_ax	bx			;Set AX = number of bits to pad


;	Some number of 0's can be added to the left of the character.
;	Add them, then move the lhs left by that amount.

plhs_add_padding:
	mov	dx,di			;DI = start x = text_bbox.left
	sub	dx,ax			;Set new lhs of text bounding box
	mov	current_lhs,dx
	mov	ah,dl			;Set phase (x coordinate) of char
	and	ah,7
	pop	dx
	push	ax			;Width and phase of null character
	push	null_char_offset	;Offset in font of null character
	jmp	dx

plhs_all_done:
	ret

pad_left_hand_side endp

page
;--------------------------Private-Routine------------------------------;
; pad_right_hand_side
;
;   This routine is called once the text string has been pushed onto
;   the stack.	It pads the character on the right up to a byte boundary
;   if possible.
;
;   We always pad out to a byte boundary with 0 since it makes the
;   stopping condition in the driving loop simpler since it never
;   has to check to see if it has any more bytes of data left, it
;   knows it does.  It just checks after each destination column
;   has been output to see if anything else exists.
;
;   The clipping mask will be computed to the last pixel we can
;   alter.  In transparent mode, this will always be to the byte
;   boundary.  In opaque mode, it depends on the opaquing rectangle.
;
; Entry:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	DI     =  X position where next char would have gone
; Returns:
;	DS:SI --> current character in the string
;	ES:    =  font segment
;	DI     =  rhs of the string, padded to boundary if possible
; Error Returns:
;	'C' set if no interval to output
; Registers Preserved:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Registers Destroyed:
;	AX,BX,DX
; Calls:
;	comp_byte_interval
; History:
;	Thu 16-Apr-1987 23:37:27 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,FontSeg
assumes ss,StrStuff


pad_right_hand_side	proc near

	mov	ax,di			;If next char would start at bit 0,
	and	al,7			;  then no padding is required.
	jz	prhs_have_rhs		;No padding needed
	mov	ah,al			;Save phase
	neg	al			;Set width needed
	and	al,7
	pop	bx
	push	ax			;Width and phase of null character
	push	null_char_offset	;Offset in font of null character
	push	bx
	xor	ah,ah			;Only want width of the nulls in AX
	test	accel,IS_OPAQUE 	;If in transparent mode
	jz	prhs_new_rhs		;  move text_bbox to byte boundary
	mov	bl,excel
	not	bl
	test	bl,OPAQUE_RECT+BOUNDED_IN_Y
	jnz	prhs_have_rhs		;Cannot alter actual rhs
	mov	bx,o_rect.right 	;Compute distance from where I am to
	sub	bx,di			;  where opaque rectangle ends
	jle	prhs_have_rhs		;Opaque rect is left of text end
	min_ax	bx			;Compute amount to move rhs

prhs_new_rhs:
	add	di,ax			;Slide rhs right

prhs_have_rhs:
	ret

pad_right_hand_side	endp

page
;--------------------------Private-Routine------------------------------;
;
; process_stack_data
;
; The non-overlapped data which has been accumulated on the stack is
; output using the supplied dispatch tables.
;
; Entry:
; Returns:
;	None
; Error Returns:
;	none
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI
; Registers Preserved:
;	DS,ES,BP
; Calls:
;	Lots
; History:
;	Tue 05-May-1987 18:27:29 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing
assumes ss,StrStuff

process_stack_data proc near

	mov	ds,wptr lfd.lp_font_bits[2]
	assumes	ds,nothing

	les	di,lp_surface
	assumes	es,nothing

	mov	ax,text_bbox.top	;load y coord
	mul	next_scan
	add	di,ax			;
	add	di,scan_start		;di complete


;	It is possible that the first character requires clipping.
;	If the clip mask is non-zero, then this is the case.


	mov	bx,non_clipped_table	;Table if not clipped
	mov	al,left_clip_mask
	or	al,al
	mov	ss_clip_mask,al
	jz	psd_not_clipped 	;No partial first byte
	inc	inner_byte_count	;One extra time through the loop
	mov	bx,clipped_table

psd_not_clipped:
	mov	al,16			;Looking to cross two byte boundaries
	test	excel,FIRST_IN_PREV	;Does first char span a boundary?
	jnz	psd_collect_chars	;Need to cross two
	mov	al,8			;Only 1 byte boundary (BL = 8)


;	This is the start of the real loop where we zip through
;	all the data we pushed onto the stack.


psd_collect_chars:
	xor	si,si			;Dispatch table index
	mov	dx,bp			;Save frame pointer
	mov	bp,buffer		;--> next character's data
	mov	cx,wptr [bp].fd_width	;CH = X position, CL = width
	errnz	fd_phase-fd_width-1

	add	ch,cl			;CH = next char's start bit position
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-1 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-2 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-3 * size frame_data]
	cmp	ch,al
	jb	psd_unlikely_cases
	ja	psd_have_more_than_enough
;	je	psd_have_exact_fit

psd_have_exact_fit:
	mov	cx,si			;# of source character involved
	shl	si,1			;Compute index into dispatch table
	mov	bx,wptr cs:[bx][si]	;Get address of drawing function
	shl	si,1
	errnz	<(size frame_data) - 4>
	neg	si			;Going backwards
	lea	ax,[bp].[-(size frame_data)][si]
	xchg	bp,dx			;Restore frame pointer
	mov	buffer,ax		;Save new buffer pointer


;	Call the procedure with the following register variables
;
;	    DS:     =  Font bits segment
;	    ES:DI  --> destination
;	    DX	    =  pointer to frame data
;	    CX	    =  # of source characters
;	    AX	    =  Visible height

	mov	ax,clipped_font_height
	call	bx			;Invoke drawing routine
	mov	al,8			;First doesn't cross byte boundary
	mov	bx,non_clipped_table	;No clipping required
	dec	inner_byte_count
	jg	psd_collect_chars
	jmp	short psd_see_about_last

psd_have_more_than_enough:
	mov	cx,si			;# of source character involved
	shl	si,1			;Compute index into dispatch table
	mov	bx,wptr cs:[bx][si]	;Get address of drawing function
	shl	si,1
	errnz	<(size frame_data) - 4>
	neg	si			;Going backwards
	lea	ax,[bp][si]
	xchg	bp,dx			;Restore frame pointer
	mov	buffer,ax		;Save new buffer pointer


;	Call the procedure with the following register variables
;
;	    DS:     =  Font bits segment
;	    ES:DI  --> destination
;	    DX	    =  pointer to frame data
;	    CX	    =  # of source characters
;	    AX	    =  Visible height

	mov	ax,clipped_font_height
	call	bx			;Invoke drawing routine
	mov	al,16			;First crosses byte boundary
	mov	bx,non_clipped_table	;No clipping required
	dec	inner_byte_count
	jg	psd_collect_chars
	jmp	short psd_see_about_last


psd_unlikely_cases:
	inc	si
	add	ch,[bp].fd_width[-4 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-5 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-6 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	je	psd_have_exact_fit

	inc	si
	add	ch,[bp].fd_width[-7 * size frame_data]
	cmp	ch,al
	ja	psd_have_more_than_enough
	jmp	psd_have_exact_fit	;It had to have fit!


psd_see_about_last:
;	mov	al,either 8 or 16	;Was done on the way here
	xor	cx,cx			;Zero last byte clipped mask
	xchg	cl,right_clip_mask	;  and get it
	jcxz	psd_exit		;No last byte to deal with
	mov	ss_clip_mask,cl 	;have last byte, set clip mask
	mov	bx,clipped_table	;Must use the clipped table
	jmp	psd_collect_chars

psd_exit:
	ret

process_stack_data endp
page
;---------------------------Public-Routine------------------------------;
;
; prepare_for_overlap
;
;   Possible negative justification and/or width vector.  If
;   opaque mode, then compute the extents of the string so that
;   the bounding box can be output if we step backwards.  If we
;   will step backwards, opaque the area where the string will
;   go and set transparent mode for the actual text output routine.
;
; Entry:
;	bl = accel
; Returns:
;	None
; Error Returns:
;	to build_all_done if nothing will show
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES
; Registers Preserved:
;	None
; Calls:
;	worst_case_ext
;	output_o_rect
; History:
;	Wed 06-May-1987 21:33:15 -by-  Walt Moore [waltm]
; wrote it
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing
assumes ss,StrStuff

prepare_for_overlap proc near


	push	brk_err 		;Must not destroy the justification
	push	brk_count		;  DDA parameters while we find out
	push	brk_rem 		;  how long the string is and if
	push	off_lp_dx		;  we stepped backwards
	mov	cx,count		;  we stepped backwards
	call	worst_case_ext
	pop	off_lp_dx
	pop	brk_rem
	pop	brk_count
	pop	brk_err
	test	wc_flags,STEPPED_BACK
	jz	pfo_exit		;Did not step backwards


;-----------------------------------------------------------------------;
;
;	Will be stepping backwards.  Opaque the area where the string
;	will go.  This area will have to be clipped.
;
;-----------------------------------------------------------------------;

	mov	ax,clip.right
	mov	cx,x			;CX = lhs
	add	bx,cx			;BX = rhs
	jo	pfo_have_rhs		;Use clip.right for right side
	min_ax	bx

pfo_have_rhs:
	xchg	ax,bx			;Need rhs in BX
	mov	ax,clip.left
	max_ax	cx
	cmp	bx,ax
	stc				;JGE is SF = OF, doesn't use carry!
	jle	pfo_exit_nothing_shows	;Null or negative interval


;-----------------------------------------------------------------------;
;
;	The interval will show.  Dummy this up as a call to the
;	opaque rectangle code to output the bounding box.  Since
;	TEXT_VISIBLE was clear bu build_string, the opaque code
;	will not perform an intersection of text_bbox and o_rect.
;
;-----------------------------------------------------------------------;

	mov	wc_opaque_lhs,ax	;Save lhs/rhs incase we have an
	mov	wc_opaque_rhs,bx	;  opaquing rectangle
	or	wc_flags,WC_SET_LR	;Set left/right into text bbox
	push	o_rect.left		;Save real o_rect bbox
	push	o_rect.right
	push	o_rect.top
	push	o_rect.bottom
	mov	cx,text_bbox.top
	mov	dx,text_bbox.bottom
	mov	o_rect.left,ax		;Set text bbox as area to opaque
	mov	o_rect.right,bx
	mov	o_rect.top,cx
	mov	o_rect.bottom,dx
	mov	bl,excel
	call	output_o_rect
	pop	o_rect.bottom
	pop	o_rect.top
	pop	o_rect.right
	pop	o_rect.left

	and	accel,not IS_OPAQUE	;Will output text in transparent mode

pfo_exit:
	ret

pfo_exit_nothing_shows:
	pop	ax
	jmp	build_restore_opaque

prepare_for_overlap endp


ifdef	PUBDEFS
	include BUILDSTR.PUB
endif


sEnd	rCode
end
