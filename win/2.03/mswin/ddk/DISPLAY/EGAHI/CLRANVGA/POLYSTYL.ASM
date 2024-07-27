	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: polystyl.asm
;
; Brief Description: Polyline styled line drawing device driver.
;
; Created: 3/30/87
; Author: **** ******	(******)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
; This module contains all of the routines called by POLYLINE.ASM to draw
; styled polylines in memory bitmaps or the EGA.  The routines are basically 
; broken into six different cases.  Lines are categorized as x major,
; y major or diagonal.  They are also broken down into simple and 
; non-simple, or standard, cases; where simple means horizontal, vertical
; or on a diagonal.  These simple cases can be drawn much faster than
; the standard cases, and are therefore broken out from the rest.
;
; There are similar routines in PLYSOLID.ASM for solid lines to the EGA,
; and POLYBITM.ASM for solid lines to memory bitmaps.  While these 
; routines are all quite similar, they are separated purely for speed
; considerations.  POLYLINE.ASM is the dispatching module for all of 
; these routines, and the run length slice algorithm DDA, which all of
; these routines are based on, is explained in that module.
;
; When drawing style lines in opaque mode, two passes are made.  The first
; pass draws the line part of the polyline, skipping the gaps.  The second
; pass draws the gaps.  If the line is drawn in transparent mode, the 
; second pass is skipped.
;
; At the end of this module, there are four output routines.  
; styled_line_pixel draws the line portions of styled lines to the EGA.
; styled_gap_pixel draws the gap portions of the styled lines to the EGA.
; styled_bitmap_line_pixel and styled_bitmap_gap_pixel act similarly with
; bitmaps.  As needed, the address of one of these four routines is loaded
; into StyledProc.  Then every time a pixel is drawn, StyledProc is
; called to do the output.
;
; !!! For the release of Driver 2.0 small bitmaps are stored on a plane !!!
; !!! by plane basis (ie if we have three color planes, then the segment !!!
; !!! is stored as the red color plane, green color plane and then the !!!
; !!! blue color plane).  For the release of Driver 3.0 this will need to !!!
; !!! be changed to match the format of huge bitmaps. !!!
;
; Huge bitmaps are stored on a scan line basis.  For example, if we have
; three color planes, the the bitmap is stored as a red scan line, a green
; scan line, a blue scan line, a red scan line, and so on.  Because of this
; format, there will most likely be some left over bytes at the end of the
; segment.  These will have to be accounted for when crossing segment
; boundaries.  
;-----------------------------------------------------------------------;
	title	Polyline styled line drawer

incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include egamem.inc
	include ega.inc
	include macros.mac
	include polyline.mac
	include polyline.inc
	.list

	??_out	polystyl

	externA	MAX_STYLE_ERR


createSeg _LINES,LineSeg,word,public,CODE
sBegin	LineSeg
assumes cs,LineSeg

	public	styled_draw_horizontal_line
	public	styled_draw_vertical_line
	public	styled_draw_diagonal_line
	public	styled_draw_first_x_axial_segment
	public	styled_draw_last_x_axial_segment
	public	styled_draw_last_y_axial_segment
	public	styled_draw_last_diagonal_segment
	public	styled_draw_x_axial_segments
	public	styled_draw_y_axial_segments
	public	styled_draw_diag_x_major_segments
	public	styled_draw_diag_y_major_segments
	public	styled_line_pixel
	public	styled_gap_pixel
	public	styled_bitmap_line_pixel
	public	styled_bitmap_gap_pixel
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_horizontal_line
;
; this routine is called to draw the completely horizontal lines very quickly.
; this routine is only called in the case of a horizontal line.  the location
; of the current destination byte is loaded into DS:DI and we fall through
; to styled_draw_first_x_axial_segment.
;
; the reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  
;
; Entry:
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;
; Return:
;	AL = bit offset.
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: none.
;
; History:
;  Mon 20_apr_1987 13:56:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_horizontal_line
; {
;    // this routine is called to draw horizontal lines.
;
;    DS = SEG_CurByte;			// DS:DI => current destination byte.
;    DI = OFF_CurByte;
;    BL = RotBitMask;
;
;    if (moving left)
;    {
;        rotate bit mask right one bit;
;        if (done with byte)
;            move to next byte;
;        adjust StyleLength;
;    }
;
;    fall through to styled_draw_first_x_axial_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing
	assumes ss,nothing

cProc	styled_draw_horizontal_line,<FAR,PUBLIC,WIN,PASCAL>

	.xlist
	include plylocal.inc
	.list

cBegin	<nogen>

	lds	di,CurByte		; DS:DI => current destination byte.
	assumes	ds,nothing
	mov	bl,RotBitMask

;	jmp	styled_draw_first_x_axial_segment
	errn$	styled_draw_first_x_axial_segment

cEnd	<nogen>
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_first_x_axial_segment
;
; styled_draw_first_x_axial_segment draws a single x axial line segment in a
; bitmap of EGA.  this routine is called by x_axial_cases macro to draw the
; first segment of a non-horizontal x axial line.  this routine is fallen
; into from styled_draw_horizontal_line when drawing a horizontal line.
;
; the reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  
;
; Entry:
;	BL = RotBitMask.
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;	DS:DI = pointer to current EGA memory byte.
;
; Return:
;	AL = bit offset.
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: none.
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_first_x_axial_segment
; {
;    // since this is an x axial case, we will be setting multiple bits
;    // per byte.  the rotating bit mask acts as an index into 
;    // bit_offset_table, which tells how many places to shift the bits.
;
;    index = RotBitMask / 2;
;    BitOffset = bit_offset_table[index];
;
;    // do the actual work.
;
;    fall through to styled_draw_last_x_axial_segment;
; }
;-----------------------------------------------------------------------;

styled_draw_first_x_axial_segment	proc	near

; since this is an X-major axial case, we will be setting multiple bits,
; instead of just using the rotating bit mask as given.  CX tells us the
; number of bits we will be setting, and the rotating bit mask gives us
; an index into r_bit_off_tbl which gives us the number of places to shift
; the bits.

	mov	ax,8			; initial value.

top_first_seg_loop:
	dec	ax
	shr	bl,1
	jnc	top_first_seg_loop

	mov	bl,al			; assume right moving case.
	test	CaseFlags,STEP_LEFT
	jz	done_first		; jump if moving right.

	mov	bl,7 			; transform for left moving case.
	sub	bl,al

done_first:
;	jmp	styled_draw_last_x_axial_segment
	errn$	styled_draw_last_x_axial_segment ; falls through to x_axial_sub_final

styled_draw_first_x_axial_segment	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_last_x_axial_segment
;
; styled_draw_last_x_axial_segment draws a single x_axial line segment in a
; bitmap or EGA.  this routine is jumped to from x_axial_cases macro to draw
; the last segment of a non-horizontal x axial line.  this routine is fallen
; into from styled_draw_first_x_axial_segment when drawing a horizontal
; line or the first segment of a non-horizontal line.
;
; the reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  completely horizontal lines are  
; handled by this procedure.  they are handled by styled_draw_first_x_axial_segment, which
; falls through to here.
;
; Entry:
;	BL = number of places to shift byte.
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;	DS:DI = pointer to current EGA memory byte.
;
; Return:
;	AL = bit offset.
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_last_x_axial_segment
; {
;    // calculate number of bits needed and see if we fit into one byte.
;
;    if ((BitCount+BitOffset) < 8)
;    {
;        // set up to output one byte.
;
;        shift BitCount bits into AL;
;        shift them into position by BitOffset;
;        BL = BitOffset + BitCount;
;        jump to output_final_byte;	// go output the byte.
;    }
;
;    // output multiple bytes.  the first and last bytes will be partial
;    // bytes.  any intermediate bytes will have all bits set.
;
;    // set up for first byte.
;
;    AL = 0xFF;
;    AL >>= BitOffset;
;    output byte to bitmap memory;
;    DI++;				// increment destination counter.
;
;    // output whole bytes.
;
;    while (number of whole bytes--)
;    {
;        output 0xFF to bitmap memory;
;        DI++;				// increment destination counter.
;    }
;
;    // set up to output the final byte.
;
;    AX = 0xFF00;
;    AX >>= # bits remaining.		// shift bits into AL.
;
;output_final_byte:
;    output byte to bitmap memory;
;    increment DI if done with byte;
;
;    get rotating bitmask in BL;
;					// return with BL = rotating bitmask,
;    return();				// DS:DI => current destination byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_last_x_axial_segment  proc near

	xor	bh,bh			; make a word.
	xchg	bx,cx

	test	CaseFlags,STEP_LEFT	; are we moving left or right?
	jnz	x_style_moving_left	; jump if moving left.

; draw the line moving left to right.

	mov	al,080h
	shr	al,cl			; shift that many bits.
	xchg	bx,cx
	add	bx,cx
	jcxz	s_no_final_byte

	push	bx
	mov	bx,XMajorDist

s_output_loop:
	call	word ptr StyledProc	; output the byte.
	sub	StyleLength,bx		; adjust style length.
	ror	al,1			; rotate bitmask.
	adc	di,0			; move to next byte if necessary.
	loop	s_output_loop		; go do next pixel.
	jmp	short s_get_outa_here	; skip over left moving stuff.

; draw the line moving right to left.

x_style_moving_left:
	mov	al,01h			; shift bitmask into position.
	shl	al,cl
	xchg	bx,cx
	add	bx,cx
	jcxz	s_no_final_byte

	push	bx
	mov	bx,XMajorDist

x_style_left_loop:
	call	word ptr StyledProc	; output the byte.
	sub	StyleLength,bx		; adjust style length.
	rol	al,1			; rotate bitmask.
	sbb	di,0			; move to next byte if necessary.
	loop	x_style_left_loop	; go do next pixel.

s_get_outa_here:
	pop	bx

s_no_final_byte:
	and	bx,7
	xchg	al,bl 			; AL = bitoffset, BL = rotbitmask.

	ret

styled_draw_last_x_axial_segment  endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_vertical_line
;
; this routine is called to draw the completely vertical lines very quickly.
; this routine is only called in the case of a vertical line.  the location
; of the current destination byte is loaded into DS:DI and we fall through
; to styled_draw_last_y_axial_segment.
;
; the reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.   
;
; Entry:
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;	AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc
;
; History:
;  Mon 20-Apr-1987 14:01:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_vertical_line
; {
;    // this routine is called to draw vertical lines.
;
;    DS = SEG_CurByte;			// DS:DI => current destination byte.
;    DI = OFF_CurByte;
;    AL = RotBitMask;			// get rotating bitmask.
;
;    fall through to bitmap_draw_last_y_axial_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_vertical_line	proc	near

	lds	di,CurByte		; DS:DI => current destination byte.
	assumes	ds,nothing
	mov	al,RotBitMask		; rotating bit mask.
;	jmp	styled_draw_last_y_axial_segment
	errn$	styled_draw_last_y_axial_segment

styled_draw_vertical_line	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_last_y_axial_segment
;
; styled_draw_last_y_axial_segment draws a single y_axial line segment in
; a bitmap or EGA.  this routine is jumped to from y_axial_cases macro to draw
; the last segment of a non-vertical y axial line.  this routine is fallen
; into from styled_draw_vertical_line when drawing a vertical line.
;
; the reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.   
;
; Entry:
;	AL = rotating bit mask.
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;	DS:DI = pointer to current EGA memory byte.
;	AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc
;
; History:
;  Thu 30-Apr-1987 11:18:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_last_y_axial_segment
; {
;    // this routine outputs BitCount vertical bits.  therefore, the 
;    // rotating bit mask is contant for all bytes output.
;
;    while (BitCount--)
;    {
;        output byte to bitmap memory;
;        DI += AddVertStep;		// jump to next scan line.
;        check for segment overflow;
;    }
;
;    					// return with BL = rotating bitmask.
;    return();				// DS:DI => current destination byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_last_y_axial_segment	  proc near

; this routine outputs CX vertical bits.  therefore, the rotating bit	    
; mask is constant for all bytes output.  

	jcxz	styled_y_axial_end_of_final
	mov	si,AddVertStep

	mov	bx,YMajorDist

styled_y_axial_final_loop:
	call	word ptr StyledProc 	; output the byte.
	add	di,si			; jump to next scan line.
	call	word ptr OverflowProc	; check for segment overflow.
	sub	StyleLength,bx		; adjust style length.
	loop	styled_y_axial_final_loop

styled_y_axial_end_of_final:
	mov	bl,al			; return BL = RotBitMask
	ret

styled_draw_last_y_axial_segment	  endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_diagonal_line
;
; this routine is called to draw the completely diagonal lines very quickly.
; this routine is only called in the case of a diagonal line.  the location
; of the current distination byte is loaded into DS:DI and we fall through
; to styled_draw_last_diagonal_segment.
;
; the reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  this number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  by diagonal bits it is meant that both
; x and y coordinates are incremented or decremented, as necessary, at the
; same time.
;
; Entry:
;	 CX = number of bits to output.
;	 DX = EGA_BASE + GRAF_DATA.
;	 AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc.
;
; History:
;  Thu 30-Apr-1987 11:18:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Mon 20-Apr_1987 14:07:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_diagonal_line
; {
;    // this routine is called to draw diagonal lines.
;
;    DS = SEG_CurByte;			// DS:DI => current destination byte.
;    DI = OFF_CurByte;
;    AL = RotBitMask;			// get rotating bitmask.
;
;    if (moving left)
;    {
;        rotate bit mask right one bit;
;        if (done with byte)
;            move to next byte;
;        jump to next scan line;
;        check for segment overflow;
;        adjust StyleLength;
;    }
;
;    fall through to styled_draw_last_diagonal_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_diagonal_line	proc	near

	lds	di,CurByte		; DS:DI => current destination byte.
	assumes	ds,nothing
	mov	al,RotBitMask		; get rotating bit mask.

styled_diag_go_right:
;	jmp	styled_draw_last_diagonal_segment
	errn$	styled_draw_last_diagonal_segment

styled_draw_diagonal_line	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_last_diagonal_segment
;
; styled_draw_last_diagonal_segment draws a single diagonal line segment in
; a bitmap or EGA.  this routine is jumped to from diagonal_cases macro to
; draw the last segment of a not completely diagonal line.  this routine is
; fallen into from styled_draw_diagonal_line when drawing a diagonal line.
;
; the reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  this number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  by diagonal bits it is meant that both
; x and y coordinates are incremented or decremented, as necessary, at the
; same time.
;
; Entry:
;	AL = rotating bit mask.
;	CX = number of bits to output.
;	DX = EGA_BASE + GRAF_DATA.
;	DS:DI = pointer to current EGA memory byte.
;	AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current EGA memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc.
;
; History:
;  Thu 30-Apr-1987 11:18:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_last_diagonal_segment
; {
;    // this routine draws BitCount diagonal bits.  since we are drawing
;    // a diagonal segment, x and y coordinates will change with each
;    // bit drawn.
;
;    while (BitCount--)
;    {
;        output byte to bitmap memory;
;        rotate bit mask;
;        increment DI if done with byte;
;        DI += AddVertStep;		// jump to next scan line.
;        check for segment overflow;
;        adjust StyleLength;
;    }
;
;    BL = rotating bit mask;
;					// return with BL = rotating bitmask,
;    return();				// DS:DI => current destination byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_last_diagonal_segment	proc	near

	jcxz	styled_diagonal_end_loop ; jump if no bits to do.
	mov	bx,Hypot

	test	CaseFlags,STEP_LEFT	; are we moving left or right?
	jnz	diag_gone_left

; draw line moving right.

	mov	si,AddVertStep	     	

styled_diagonal_right_loop:
	call	word ptr StyledProc	; output the byte.
	ror	al,1			; rotate bit mask.
	adc	di,si			; update destination pointer.
	call	word ptr OverflowProc	; check for segment overflow.
	sub	StyleLength,bx		; update style length.
	loop	styled_diagonal_right_loop
	jmp	short styled_diagonal_end_loop

; draw line moving left.
	
diag_gone_left:
	mov	si,SubVertStep

styled_diagonal_left_loop:
	call	word ptr StyledProc	; output the byte.
	rol	al,1			; rotate bit mask.
	sbb	di,si			; update destination pointer.
	call	word ptr OverflowProc	; check for segment overflow.
	sub	StyleLength,bx		; update style length.
	loop	styled_diagonal_left_loop

styled_diagonal_end_loop:
	mov	bl,al			; return BL = RotBitMask
	ret

styled_draw_last_diagonal_segment	   endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_x_axial_segments
;
; styled_draw_x_axial_segments is called to draw non-horizontal x axial 
; lines.  this routine is called from POLYLINE.ASM.  the code for this
; routine resides in the x_axial_cases macro in POLYLINE.MAC, where a
; detailed explanation is given.
;
; the reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  completely horizontal lines are not 
; handled by this procedure.  they are handled faster by styled_draw_first_x_axial_segment.
;
; Entry:
;	CX = hFirst (number of bits in first line segment).
;	DX = EGA_BASE + GRAF_DATA.
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current EGA memory location.
;	AddVertStep = bytes to next scan line.
;	BitCount = number of bits per segment.
;
; Returns:
;	CX = hLast (number of bits in last line segment).
;	DS:DI = pointer to current destination byte (CurByte).
;	AL = rotating bit mask.
;
; Error Returns: None.
;
; Registers Destroyed: BX,SI,flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc.
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_x_axial_segments
; {
;    // x_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_x_axial_segments	proc	near

; the x_axial_cases macro contains the line drawing code for this case.
; the 0,1 means EGA, styled line; but the styled line overrules EGA and
; writes to a bitmap if that is the case.

	x_axial_cases  0,1		; go draw the line.

styled_draw_x_axial_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_y_axial_segments
;
; styled_draw_y_axial_segments is called to draw non-vertical y axial 
; lines.  this routine is called from POLYLINE.ASM.  the code for this
; routine resides in the y_axial_cases macro in POLYLINE.MAC, where a
; detailed explanation is given.
;
; the reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  this number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  completely vertical lines are not 
; handled by this procedure.  they are handled faster by styled_draw_last_y_axial_segment.
;
; Entry:
;	CX = hFirst (number of bits in first line segment).
;	DX = EGA_BASE + GRAF_DATA.
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current EGA memory location.
;	AddVertStep = bytes to next scan line.
;	BitCount = number of bits per segment.
;
; Returns:
;	CX = hLast (number of bits in last line segment).
;	DS:DI = pointer to current destination byte (CurByte).
;	AL = rotating bit mask.
;
; Error Returns: None.
;
; Registers Destroyed: BX,SI,flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc.
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_y_axial_segments
; {
;    // y_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_y_axial_segments	proc	near

; the y_axial_cases_macro contains the line drawing code for this case.
; the 0,1 means EGA, styled line; but the styled line overrules the EGA
; and draws to a bitmap if that is the case.

	y_axial_cases  0,1		; go draw the line.

styled_draw_y_axial_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_diag_x_major_segments
;
; styled_draw_diag_x_major_segments is called to draw diagonal x major 
; lines.  this routine is called from POLYLINE.ASM.  the code for this
; routine resides in the diagonal_cases macro in POLYLINE.MAC, where a
; detailed explanation is given.
;
; the reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  this number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  completely diagonal lines are not 
; handled by this procedure.  they are handled faster by styled_draw_last_diagonal_segment.
; by diagonal bits it is meant that both x and y coordinates are incremented
; or decremented, as necessary, at the same time.
;
; Entry:
;	CX = hFirst (number of bits in first line segment).
;	DX = EGA_BASE + GRAF_DATA.
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current EGA memory location.
;	AddVertStep = bytes to next scan line.
;	BitCount = number of bits per segment.
;
; Returns:
;	CX = HLast (number of bits in last line segment).
;	DS:DI = pointer to current destination byte (CurByte).
;	AL = rotating bit mask.
;
; Error Returns: None.
;
; Registers Destroyed: BX,SI,flags.
;
; Registers Preserved: DX.
;
; Calls: StyledProc.
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_diag_x_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_diag_x_major_segments	proc	near

; the diagonal_cases macro contains the line drawing code for this case.
; the 0,0,1 means x major, EGA, styled lines; but the styled lines overrides
; the EGA and draws in bitmaps if that is the case.

	diagonal_cases	   0,0,1 	; go draw the line.

styled_draw_diag_x_major_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_draw_diag_y_major_segments
;
; styled_draw_diag_y_major_segments is called to draw diagonal y major 
; lines.  this routine is called from POLYLINE.ASM.  the code for this
; routine resides in the diagonal_cases macro in POLYLINE.MAC, where a
; detailed explanation is given.
;
; the reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  this number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  completely diagonal lines are not 
; handled by this procedure.  they are handled faster by styled_draw_last_diagonal_segment.
; by diagonal bits it is meant that both x and y coordinates are incremented
; or decremented, as necessary, at the same time.
;
; Entry:
;	CX = hFirst (number of bits in first line segment).
;	DX = EGA_BASE + GRAF_DATA.
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current EGA memory location.
;	AddVertStep = bytes to next scan line.
;	BitCount = number of bits per segment.
;
; Returns:
;	CX = HLast (number of bits in last line segment).
;	DS:DI = pointer to current destination byte (CurByte).
;	AL = rotating bit mask.
;
; Error Returns: None.
;
; Registers Destroyed: BX,SI,flags.
;
; Registers Preserved: None.
;
; Calls: StyledProc.
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_draw_diag_y_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_draw_diag_y_major_segments	proc	near

; the diagonal_cases macro contains the line drawing code for this case.
; the 1,0,1 means y major, EGA, styled lines; but the styled lines overrides
; the EGA and draws in bitmaps if that is the case.

	diagonal_cases	   1,0,1 	; go draw the line.

styled_draw_diag_y_major_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; styled_line_pixel
;
; styled line output routine which draws the line part of a styled line,
; while skipping over the gaps in the line.  the address of this routine
; is loaded into StyledProc for the first pass of a styled line, then
; every time a pixel is drawn, this routine is called to do it.	 this
; routine draws styled lines to the EGA.
;
; Entry:
;	AL = rotating bit mask.
;	DX = EGA_BASE + GRAF_DATA
;	DS:DI = current EGA memory location.
;
; Returns: none.
;
; Error Returns: none.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX, BX, CX, DX, DS, DI.
;
; Calls: none.
;
; History:
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_line_pixel
; {
;    // rotate style flag if necessary.
;
;    if (StyleLength < 0)
;    {
;        style flag <<= 1;		// rotate style flag.
;        StyleLength += MAX_STYLE_ERR;	// reset style length.
;    }
;
;    if (high bit of style flag is set)
;        output byte to EGA;		// set the line color.
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_line_pixel   proc    near

	push	bx
	push	cx

	mov	bx,StyleLength
	mov	cx,StyleFlags
	or	bx,bx			; if StyleLength >= 0 then
	jge	line_dont_advance_style ; no need to rotate style.

	rol	cl,1			; rotate the style.
	add	bx,MAX_STYLE_ERR	; reset length count.
	mov	StyleFlags,cx		; save changed style mask.

line_dont_advance_style:
	mov	StyleLength,bx		; save length.
	or	cl,cl			; is highest bit set?
	jge	leave_line		; do line color if so.

	out	dx,al
	or	byte ptr [di],0FFh	; set the byte.

leave_line:
	pop	cx
	pop	bx
	ret

styled_line_pixel   endp
page

;--------------------------Public-Routine-------------------------------;
; styled_gap_pixel
;
; styled line output routine which draws the gap part of a styled line,
; while skipping over the line parts in the line.  the address of this routine
; is loaded into StyledProc for the secon pass of a styled line, then
; every time a pixel is drawn, this routine is called to do it.	 this
; routine draws styled lines to the EGA.
;
; Entry:
;	AL = rotating bit mask.
;	DX = EGA_BASE + GRAF_DATA
;	DS:DI = current EGA memory location.
;
; Returns: none.
;
; Error Returns: none.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX, BX, CX, DX, DS, DI.
;
; Calls: none.
;
; History:
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_gap_pixel
; {
;    // rotate style flag if necessary.
;
;    if (StyleLength < 0)
;    {
;        style flag <<= 1;		// rotate style flag.
;        StyleLength += MAX_STYLE_ERR;	// reset style length.
;    }
;
;    if (high bit of style flag is not set)
;        output byte to EGA;		// set the gap color
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_gap_pixel    proc    near

	push	bx
	push	cx

	mov	bx,StyleLength
	mov	cx,StyleFlags
	or	bx,bx
	jge	gap_dont_advance_style	; no need to rotate style.

	rol	cl,1			; rotate the style.
	add	bx,MAX_STYLE_ERR	; reset length count.
	mov	StyleFlags,cx		; save changed style mask.

gap_dont_advance_style:
	mov	StyleLength,bx		; save length.
	or	cl,cl
	js	leave_gap

	out	dx,al
	or	byte ptr [di],0FFh	; set the byte.

leave_gap:
	pop	cx
	pop	bx
	ret

styled_gap_pixel    endp
page

;--------------------------Public-Routine-------------------------------;
; styled_bitmap_line_pixel
;
; styled line output routine which draws the line part of a styled line,
; while skipping over the gaps in the line.  the address of this routine
; is loaded into StyledProc for the first pass of a styled line, then
; every time a pixel is drawn, this routine is called to do it.  this 
; routine draws styled lines to bitmaps.
;
; Entry:
;	AL = rotating bit mask.
;	DS:DI = current EGA memory location.
;
; Returns: none.
;
; Error Returns: none.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX, BX, CX, DS, DI.
;
; Calls: BitmapProc.
;
; History:
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_bitmap_line_pixel
; {
;    // rotate style flag if necessary.
;
;    if (StyleLength < 0)
;    {
;        style flag <<= 1;		// rotate style flag.
;        StyleLength += MAX_STYLE_ERR;	// reset style length.
;    }
;
;    if (high bit of style flag is set)
;        output byte to EGA;		// set the line color.
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_bitmap_line_pixel   proc    near

	push	bx
	push	cx

	mov	bx,StyleLength
	mov	cx,StyleFlags
	or	bx,bx
	jge	sb_line_dont_advance_style ; no need to rotate style.

	rol	cl,1			; rotate the style.
	add	bx,MAX_STYLE_ERR	; reset length count.
	mov	StyleFlags,cx		; save changed style mask.

sb_line_dont_advance_style:
	mov	StyleLength,bx		; save length.
	or	cl,cl			; is highest bit set?
	jns	sb_leave_line		   ; do gap color if so.

	call	word ptr BitmapProc

sb_leave_line:
	pop	cx
	pop	bx
	ret

styled_bitmap_line_pixel   endp
page

;--------------------------Public-Routine-------------------------------;
; styled_bitmap_gap_pixel
;
; styled line output routine which draws the gap part of a styled line,
; while skipping over the line parts in the line.  the address of this routine
; is loaded into StyledProc for the secon pass of a styled line, then
; every time a pixel is drawn, this routine is called to do it.	 this
; routine draws styled lines to a bitmap.
;
; Entry:
;	AL = rotating bit mask.
;	DS:DI = current EGA memory location.
;
; Returns: none.
;
; Error Returns: none.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX, BX, CX, DS, DI.
;
; Calls: BitmapProc.
;
; History:
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; styled_bitmap_gap_pixel
; {
;    // rotate style flag if necessary.
;
;    if (StyleLength < 0)
;    {
;        style flag <<= 1;		// rotate style flag.
;        StyleLength += MAX_STYLE_ERR;	// reset style length.
;    }
;
;    if (high bit of style flag is not set)
;        output byte to EGA;	       	// set the gap color.
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

styled_bitmap_gap_pixel    proc    near

	push	bx
	push	cx

	mov	bx,StyleLength
	mov	cx,StyleFlags
	or	bx,bx
	jge	sb_gap_dont_advance_style  ; no need to rotate style.

	rol	cl,1			; rotate the style.
	add	bx,MAX_STYLE_ERR	; reset length count.
	mov	StyleFlags,cx		; save changed style mask.

sb_gap_dont_advance_style:
	mov	StyleLength,bx		; save length.
	or	cl,cl
	js	sb_leave_gap

	call	word ptr BitmapProc

sb_leave_gap:
	pop	cx
	pop	bx
	ret

styled_bitmap_gap_pixel    endp


sEnd	LineSeg

ifdef	PUBDEFS
	include polystyl.pub
endif

end
