	title	Polyline solid line drawing
	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: plysolid.asm
;
; Brief Description: Polyline solid line drawing device driver.
;
; Created: 3/30/87
; Author: **** ******	(******)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
; This module contains all of the routines called by POLYLINE.ASM
; to draw solid polylines in EGA memory.  The routines are basically 
; broken into six different cases.  Lines are categorized as x major,
; y major or diagonal.  They are also broken down into simple and 
; non-simple, or standard, cases; where simple means horizontal, vertical
; or on a diagonal.  These simple cases can be drawn much faster than
; the standard cases, and are therefore broken out from the rest.
;
; There are similar routines in POLYBITM.ASM for solid lines to bitmaps,
; and POLYSTYL.ASM for styled lines to EGA or bitmaps.  While these 
; routines are all quite similar, they are separated purely for speed
; considerations.  POLYLINE.ASM is the dispatching module for all of 
; these routines, and the run length slice algorithm DDA, which all of
; these routines are based on, is explained in that module.
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions
	include gdidefs.inc
	include display.inc
	include egamem.inc
	include ega.inc
	include macros.mac
	include polyline.inc
	include polyline.mac
	.list

??_out	plysolid

createSeg _LINES,LineSeg,word,public,CODE
sBegin	LineSeg
assumes cs,LineSeg
assumes ds,Data
assumes es,nothing
assumes ss,nothing

	externB  LineSeg_rot_bit_tbl	;Table of rotating bit masks
	externB  bit_offset_table	;Bit offset translation table.

	public	draw_horizontal_line
	public	draw_vertical_line
	public	draw_diagonal_line
	public	draw_first_x_axial_segment
	public	draw_last_x_axial_segment
	public	draw_last_y_axial_segment
	public	draw_last_diagonal_segment
	public	draw_x_axial_segments
	public	draw_y_axial_segments
	public	draw_diag_x_major_segments
	public	draw_diag_y_major_segments
page

;--------------------------Public-Routine-------------------------------;
; draw_horizontal_line
;
; this routine is called to draw the completely horizontal lines very quickly.
; this routine is only called in the case of a horizontal line.  the location
; of the current destination byte is loaded into DS:DI and we fall through
; to draw_first_x_axial_segment.
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
;  Mon 20-Apr-1987 12:00:00	-by-    **** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; draw_horizontal_line
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
;    }
;
;    fall through to styled_draw_first_x_axial_segment;
; }
;-----------------------------------------------------------------------;

cProc	draw_horizontal_line,<FAR,PUBLIC,WIN,PASCAL>

	.xlist
	include plylocal.inc
	.list

cBegin nogen

	lds	di,CurByte		; DS:DI => current destination byte.
	mov	bl,RotBitMask

	test	CaseFlags,STEP_LEFT	; if this is a left moving line, then
	jz	horizontal_going_right	; we dont want to draw first point of line.

	ror	bl,1			; rotate bit mask as if point was done.
	adc	di,0			; move to next byte if done with 
					; current byte.
horizontal_going_right:
;	jmp	draw_first_x_axial_segment
	errn$	draw_first_x_axial_segment

cEnd nogen
page

;--------------------------Public-Routine-------------------------------;
; draw_first_x_axial_segment
;
; draw_first_x_axial_segment draws a single x axial line segment in EGA
; memory.  this routine is called by x_axial_cases macro to draw the
; first segment of a non-horizontal x axial line.  this routine is fallen
; into from draw_horizontal_line when drawing a horizontal line.
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
; Registers Destroyed: AX, CX, SI, flags.
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
; draw_first_x_axial_segment
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
;    fall through to draw_last_x_axial_segment;
; }
;-----------------------------------------------------------------------;

draw_first_x_axial_segment	proc	near

; since this is an X-major axial case, we will be setting multiple bits,
; instead of just using the rotating bit mask as given.  CX tells us the
; number of bits we will be setting, and the rotating bit mask gives us
; an index into bit_offset_table which gives us the number of places to shift
; the bits.

	xor	bh,bh			; zero out BH.
	shr	bl,1			; zero base index into table.
	mov	bl,bit_offset_table[bx]	; BL = number of places to shift bits.

;	jmp	draw_last_x_axial_segment
	errn$	draw_last_x_axial_segment   ; falls through to draw_last_x_axial_segment

draw_first_x_axial_segment	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_last_x_axial_segment
;
; draw_last_x_axial_segment draws a single x_axial line segment in EGA
; memory.  this routine is jumped to from x_axial_cases macro to draw
; the last segment of a non-horizontal x axial line.  this routine is fallen
; into from draw_first_x_axial_segment when drawing a horizontal
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
; draw_last_x_axial_segment
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
;    output byte to EGA memory;
;    DI++;				// increment destination counter.
;
;    // output whole bytes.
;
;    while (number of whole bytes--)
;    {
;        output 0xFF to memory;
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

draw_last_x_axial_segment  proc near
; calculate the number of bits needed and see if we fit into a byte.

	mov	si,cx			; get the number of bits to set.
	xor	bh,bh
	add	si,bx			; add the number of places to shift.
	cmp	si,8			; does it fit into one byte?
	jge	crosses_byte_boundary	; jump if more than one byte.

	mov	ax,0FF00h
	shr	ax,cl			; get CL bits together in AL.
	xchg	bl,cl
	shr	al,cl			; shift them into position.
	add	bl,cl			; BL = # bits set + # bits shifted.
	jmp	output_last_final_byte	; go output the byte.

; by the time we set the necessary number of bits and then shift them, they
; will cross over a byte boundary.  so we have to take care of that case here.

; we will have to output multiple bytes since we cross byte boundaries.  with the
; first byte, we can start with all of the bits set and then shift out the
; appropriate number of bits.

crosses_byte_boundary:
	mov	al,0FFh
	xchg	bx,cx			; get the number of bits to shift in CL.
	shr	al,cl			; shift that many bits.
					; AL equals first bit mask for output.
	xchg	bx,cx			; put BX and CX back the way they were.

	out	dx,al			; set up bitmask.
	or	byte ptr [di],0FFh
	inc	di

; since we are working with X-major axial lines, we should check to see if
; we have a long horizontal segment, ie more than two bytes.  if so, we can
; output bytes at a time without having to worry about the individual bits.

	sub	bl,8
	neg	bl
	xor	bh,bh
	sub	cx,bx			; CX = bits left to deal with.
	mov	bx,cx
	and	bl,7			; BL = bits left to shift in.
	shiftr	cx,3			; CX = number of bytes where all of the
	jle	no_bytes_to_be_done	; bits will be set. ie a long horizontal
					; line.
	mov	al,0FFh
	out	dx,al			; set up bit mask for all bits in byte.

; remember CX holds the number of bytes which hold consecutive horizontal bits
; over the entire byte.

output_loop:
	or	byte ptr [di],0FFh
	inc	di
	loop	output_loop

no_bytes_to_be_done:
	mov	cl,bl			; CL = # of remaining bits in line.
	mov	ax,0FF00h
	shr	ax,cl			; shift bits into AL.

; output the last byte of this line segment.  the bit mask is defined from
; whichever route got us here.

output_last_final_byte:
	out	dx,al			; set up bit mask register.
	or	byte ptr [di],0FFh	; move byte into memory.

	shr	al,1			; point to next byte if done
	adc	di,0			; with the current one.

	and	bx,7
	mov	al,bl			; RETURN AL = bit offset.
	mov	bl,LineSeg_rot_bit_tbl[bx] ; RETURN BL = rotating bit mask
	ret

draw_last_x_axial_segment  endp
page

;--------------------------Public-Routine-------------------------------;
; draw_vertical_line
;
; this routine is called to draw the completely vertical lines very quickly.
; this routine is only called in the case of a vertical line.  the location
; of the current destination byte is loaded into DS:DI and we fall through
; to draw_last_y_axial_segment.
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
; Calls: none.
;
; History:
;  Mon 20-Apr-1987 12:00:00	-by-    **** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; draw_vertical_line
; {
;    // this routine is called to draw vertical lines.
;
;    DS = SEG_CurByte;			// DS:DI => current destination byte.
;    DI = OFF_CurByte;
;    AL = RotBitMask;			// get rotating bitmask.
;
;    fall through to draw_last_y_axial_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_vertical_line	proc	near

	lds	di,CurByte   		; DS:DI => current EGA memory byte.
	mov	al,RotBitMask		; get rotating bit mask.
;	jmp	draw_last_y_axial_segment
	errn$	draw_last_y_axial_segment

draw_vertical_line	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_last_y_axial_segment
;
; draw_last_y_axial_segment draws a single y_axial line segment in
; EGA memory.  this routine is jumped to from y_axial_cases macro to draw
; the last segment of a non-vertical y axial line.  this routine is fallen
; into from draw_vertical_line when drawing a vertical line.
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
; draw_last_y_axial_segment
; {
;    // this routine outputs BitCount vertical bits.  therefore, the 
;    // rotating bit mask is contant for all bytes output.
;
;    while (BitCount--)
;    {
;        output byte to EGA memory;
;        DI += AddVertStep;		// jump to next scan line.
;    }
;
;    					// return with BL = rotating bitmask.
;    return();				// DS:DI => current destination byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_last_y_axial_segment	   proc near

; this routine outputs CX vertical bits.  therefore, the rotating bit	    
; mask is constant for all bytes output.  the following section sets up
; for a loop which outputs eight vertical bytes.  this loop partially 
; unrolled to save time, but not unrolled too far as to waste many bytes.

	jcxz	y_axial_end_of_final
	mov	si,AddVertStep
	out	dx,al

	mov	bx,cx			; BX = BitCount
	shiftr	cx,3			; CX = number of times through loop
	inc	cx			; adjust for partial loop
	and	bx,7			; BX = remainder

; BX = the number of bits to set for the partial loop.  in the loop
; below it takes 5 bytes to output each pixel, so BX is multiplied by 5.

	push	cx
	mov	cx,bx
	shiftl	bx,2			; 5 bytes per vertical grouping
	add	bx,cx
	pop	cx
	neg	bx
	add	bx,LineSegOFFSET y_axial_end_of_final_loop
	jmp	bx

	even				; align on a word boundary.
y_axial_final_loop:
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
	or	byte ptr [di],0FFh 	; output the byte.
	add	di,si			; jump to next scan line.
y_axial_end_of_final_loop:
	loop	y_axial_final_loop
y_axial_end_of_final:
	mov	bl,al			; return BL = RotBitMask
	ret
draw_last_y_axial_segment	   endp
page

;--------------------------Public-Routine-------------------------------;
; draw_diagonal_line
;
; this routine is called to draw the completely diagonal lines very quickly.
; this routine is only called in the case of a diagonal line.  the location
; of the current distination byte is loaded into DS:DI and we fall through
; to draw_last_diagonal_segment.
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
; Calls: none.
;
; History:
;  Mon 20-Apr-1987 12:00:00	-by-    **** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; draw_diagonal_line
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
;    }
;
;    fall through to draw_last_diagonal_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_diagonal_line	proc	near

	lds	di,CurByte     		; DS:DI => current EGA memory byte.
	mov	al,RotBitMask		; get the bit mask.

	test	CaseFlags,STEP_LEFT
	jz	diagonal_going_right

	ror	al,1			; next byte if done with this one.
	adc	di,AddVertStep		; jump to next scan line.

diagonal_going_right:
;	jmp	draw_last_diagonal_segment
	errn$	draw_last_diagonal_segment

draw_diagonal_line	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_last_diagonal_segment
;
; draw_last_diagonal_segment draws a single diagonal line segment in
; EGA memory.  this routine is jumped to from diagonal_cases macro to
; draw the last segment of a not completely diagonal line.  this routine is
; fallen into from draw_diagonal_line when drawing a diagonal line.
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
; draw_last_diagonal_segment
; {
;    // this routine draws BitCount diagonal bits.  since we are drawing
;    // a diagonal segment, x and y coordinates will change with each
;    // bit drawn.
;
;    while (BitCount--)
;    {
;        output byte to EGA memory;
;        rotate bit mask;
;        increment DI if done with byte;
;        DI += AddVertStep;		// jump to next scan line.
;    }
;
;    BL = rotating bit mask;
;					// return with BL = rotating bitmask,
;    return();				// DS:DI => current destination byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_last_diagonal_segment	    proc near

	mov	si,AddVertStep
	jcxz	diagonal_end_loop  	; jump if no bytes to output.

diagonal_final_loop:
	out	dx,al		       	; set up bitmask register.
	or	byte ptr [di],0FFh	; output byte to EGA memory.
	ror	al,1			; rotate the bit mask.
	adc	di,si			; next scan line and next
					; byte if done with current one.
	loop	diagonal_final_loop	; next pixel.

diagonal_end_loop:
	mov	bl,al			; return BL = RotBitMask
	ret

draw_last_diagonal_segment	    endp
page

;--------------------------Public-Routine-------------------------------;
; draw_x_axial_segments
;
; draw_x_axial_segments is called to draw non-horizontal x axial 
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
; draw_x_axial_segments
; {
;    // x_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_x_axial_segments	proc	near

; the x_axial_cases macro contains the line drawing code for this case.
; the 0,0 means EGA, solid line.

	x_axial_cases  0,0		; go draw the line.

draw_x_axial_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_y_axial_segments
;
; draw_y_axial_segments is called to draw non-vertical y axial 
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
; draw_y_axial_segments
; {
;    // y_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_y_axial_segments	proc	near

; the y_axial_cases macro contains the line drawing code for this case.
; the 0,0 means EGA, solid line.

	y_axial_cases  0,0		; go draw the line.

draw_y_axial_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_diag_x_major_segments
;
; draw_diag_x_major_segments is called to draw diagonal x major 
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
; draw_diag_x_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_diag_x_major_segments	proc	near

; the diagonal_cases macro contains the line drawing code for this case.
; the 0,0,0 means x major, EGA, solid lines.

	diagonal_cases	0,0,0		; go draw the line.

draw_diag_x_major_segments	endp
page

;--------------------------Public-Routine-------------------------------;
; draw_diag_y_major_segments
;
; draw_diag_y_major_segments is called to draw diagonal y major 
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
; draw_diag_y_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

draw_diag_y_major_segments	proc	near

; the diagonal_cases macro contains the line drawing code for this case.
; the 1,0,0 means y major, EGA, solid line.

	diagonal_cases	1,0,0		; go draw the line.

draw_diag_y_major_segments	endp

sEnd	LineSeg

ifdef	PUBDEFS
	include plysolid.pub
endif

end
