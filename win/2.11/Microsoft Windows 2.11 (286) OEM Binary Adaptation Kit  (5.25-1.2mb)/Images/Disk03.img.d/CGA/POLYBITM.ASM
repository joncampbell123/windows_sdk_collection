	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: polybitm.asm
;
; Brief Description: Polyline bitmap line drawing device driver.
;
; Created: 3/30/87
; Author: **** ******	(******)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
; This module contains all of the routines called by POLYLINE.ASM
; to draw solid polylines in memory bitmaps.  The routines are basically 
; broken into six different cases.  Lines are categorized as x major,
; y major or diagonal.  They are also broken down into simple and 
; non-simple, or standard, cases; where simple means horizontal, vertical
; or on a diagonal.  These simple cases can be drawn much faster than
; the standard cases.
;
; There are similar routines in 
; POLYSTYL.ASM for styled lines. While these 
; routines are all quite similar, they are separated for speed
; considerations.  POLYLINE.ASM is the dispatching module for all of 
; these routines. It also contains the run length slice algorithm DDA,
; on which all of these routines are based.
;
; Near the end of this module, there are three output routines:
; bitmap_set_to_zero sets masked bits in the destination byte to zeros;
; bitmap_set_to_one sets masked bits in the destination byte to ones;
; bitmap_not_dest inverts the masked bits in the destination byte.  The
; address of one of these routines (or zero, in the case where we want
; to do nothing) is loaded into BitmapProc.  Then when drawing lines in
; a bitmap, BitmapProc is called to output each byte to the bitmap.
;
; At the end of this module, there are two routines: check_segment_overflow
; and dont_check_overflow.  The address of check_segment_overflow is loaded
; into OverflowProc if the destination is a huge bitmap and the line being
; drawn is in multiple segments; otherwise, dont_check_overflow is loaded
; into OverflowProc.  The first routine checks and corrects for segment
; overflow.  The second routine simply does a near return.  Such a 
; complicated routine as this might seem strange, but it allows us to use
; the same code for huge and small bitmaps, with minimal speed change.
;
;-----------------------------------------------------------------------;

incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions


.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
	include polyline.mac
	include polyline.inc
.list

ifdef	HERCULES
	externA	HERCULES_DEFINED
endif
ifdef	IBM_CGA
	externA	IBM_CGA_DEFINED
endif


	??_out	polybitm

createSeg _LINES,LineSeg,word,public,CODE
sBegin	LineSeg
assumes cs,LineSeg

	externB  LineSeg_rot_bit_tbl	; table of rotating bit masks.
	externB  bit_offset_table	; bit offset translation table.

	public	bitmap_draw_horizontal_line
	public	bitmap_draw_vertical_line
	public	bitmap_draw_diagonal_line
	public	bitmap_draw_first_x_axial_segment
	public	bitmap_draw_last_x_axial_segment
	public	bitmap_draw_last_y_axial_segment
	public	bitmap_draw_last_diagonal_segment
	public	bitmap_draw_x_axial_segments
	public	bitmap_draw_y_axial_segments
	public	bitmap_draw_diag_x_major_segments
	public	bitmap_draw_diag_y_major_segments
	public	bitmap_set_to_zero
	public	bitmap_set_to_one
	public	bitmap_not_dest
	public	check_segment_overflow
	public	dont_check_overflow


;--------------------------Public-Routine-------------------------------;
; bitmap_draw_horizontal_line
;
; This routine is called to draw the completely horizontal lines very
; quickly. It is only called in the case of a horizontal line.  The
; location of the current bitmap memory byte is loaded into DS:DI and
; we fall through to bitmap_draw_first_x_axial_segment.
;
; The reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  
;
; Entry:
; 	CX = number of bits to output (BitCount).
;
; Return:
;	AL = bit offset.
;  	BL = rotating bit mask.
;  	DS:DI = updated pointer to current bitmap memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: none.
;
; Calls: none.
;
; History:
;  Mon 20-Apr-1987 12:39:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_draw_horizontal_line
; {
;    // this routine is called to draw horizontal lines.
;
;    DS = seg_CurByte;			// DS:DI => current bitmap byte.
;    DI = off_CurByte;
;    BL = RotBitMask;
;
;    if (moving left)
;    {
;        rotate bit mask right one bit;
;        if (done with byte)
;            move to next byte;
;    }
;
;    fall through to bitmap_draw_first_x_axial_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc	bitmap_draw_horizontal_line,<FAR,PUBLIC,WIN,PASCAL>

	include plylocal.inc

cBegin nogen
								
	lds	di,CurByte		; DS:DI => current bitmap byte.
	mov	bl,RotBitMask
	test	CaseFlags,STEP_LEFT	; if this is a left moving line, then
	jz	bitmap_horiz_go_right	; we dont want to draw first point of line.

	ror	bl,1			; rotate bit mask as if point was done.
	adc	di,0			; move to next byte if done with 
					; current byte.
bitmap_horiz_go_right:
;	jmp	bitmap_draw_first_x_axial_segment
	errn$	bitmap_draw_first_x_axial_segment

cEnd nogen
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_first_x_axial_segment
;
; This subroutine draws a single x axial line segment in a bitmap.  This
; routine is called by x_axial_cases macro to draw the first segment of
; a non-horizontal x axial line.  This routine is fallen into from
; bitmap_draw_horizontal_line when drawing a horizontal line.
;
; The reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster. 
;
; Entry:
;	BL = RotBitMask.
;	CX = number of bits to output (BitCount).
;	DS:DI = pointer to current bitmap memory byte.
;
; Return:
;	AL = bit offset.
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current bitmap memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: CX, SI, flags.
;
; Registers Preserved: none.
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
; bitmap_draw_first_x_axial_segment
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
;    fall through to bitmap_draw_last_x_axial_segment;
; }
;-----------------------------------------------------------------------;

bitmap_draw_first_x_axial_segment	proc	near

;	Since this is an X-major axial case, we will be setting multiple
;	bits, instead of just using the rotating bit mask as given.  CX
;	tells us the number of bits we will be setting, and the rotating
;	bit mask gives us an index into bit_offset_table which gives us
;	the number of places to shift the bits.

	xor	bh,bh			; zero out BH.
	shr	bl,1			; zero base index into table.
	mov	bl,bit_offset_table[bx]	; BL = number of places to shift bits.

;	jmp	bitmap_draw_last_x_axial_segment
	errn$	bitmap_draw_last_x_axial_segment ; falls through to x_axial_sub_final

bitmap_draw_first_x_axial_segment	endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_last_x_axial_segment
;
; This subroutine draws a single x_axial line segment in a bitmap. This
; routine is jumped to from x_axial_cases macro to draw the last segment
; of a non-horizontal x axial line.  This routine is fallen into from
; bitmap_draw_first_x_axial_segment when drawing a horizontal line or
; the first segment of a non-horizontal line.
;
; The reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.    
;
; Entry:
;	BL = number of places to shift byte (BitOffset).
;	CX = number of bits to output (BitCount).
;	DS:DI = pointer to current bitmap memory byte.
;
; Return:
;	AL = bit offset.
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current bitmap memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: CX, SI, flags.
;
; Registers Preserved: none.
;
; Calls: BitmapProc
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
; bitmap_draw_last_x_axial_segment
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
;    return();				// and DS:DI => current bitmap byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_last_x_axial_segment  proc near

;	Calculate the number of bits needed and see if we fit into a byte.

	mov	si,cx			; get the number of bits to set.
	xor	bh,bh
	add	si,bx			; add the number of places to shift.
	cmp	si,8			; does it fit into one byte?
	jge	bm_crosses_byte_boundary ; jump if more than one byte.

	mov	ax,0FF00h
	shr	ax,cl			; get CL bits together in AL.
	xchg	bl,cl
	shr	al,cl			; shift them into position.
	add	bl,cl			; BL = # bits set + # bits shifted.
	jmp	bm_output_last_final_byte	; go output the byte.

;	By the time we set the necessary number of bits and then shift
;	them, they will cross over a byte boundary.  So we have to take
;	care of that case here.

;	We will have to output multiple bytes since we cross byte boundaries.
;	With the first byte, we can start with all of the bits set and then
;	shift out the appropriate number of bits.

bm_crosses_byte_boundary:
	mov	al,0FFh
	xchg	bx,cx			; get the number of bits to shift in CL.
	shr	al,cl			; shift that many bits.
					; AL equals first bit mask for output.
	xchg	bx,cx			; put BX and CX back the way they were.

	call	word ptr BitmapProc
	inc	di

;	Since we are working with X-major axial lines, we should check
;	to see if we have a long horizontal segment, i.e. more than two
;	bytes.  If so, we can output bytes at a time without having to
;	worry about the individual bits.

	sub	bl,8
	neg	bl
	xor	bh,bh
	sub	cx,bx			; CX = bits left to deal with.
	mov	bx,cx
	and	bl,7			; BL = bits left to shift in.
	shiftr	cx,3			; CX = number of bytes where all of the
	jle	bm_no_bytes_to_be_done	   ; bits will be set. ie a long horizontal
					; line.
	mov	al,0FFh

;	Remember CX holds the number of bytes which hold consecutive
;	horizontal bits over the entire byte.

bm_output_loop:
	call	word ptr BitmapProc
	inc	di
	loop	bm_output_loop

bm_no_bytes_to_be_done:
	mov	cl,bl			; CL = # of remaining bits in line.
	mov	ax,0FF00h
	shr	ax,cl			; shift bits into AL.

;	Output the last byte of this line segment. The bit mask is
;	defined from whichever route got us here.

bm_output_last_final_byte:
	call	word ptr BitmapProc	; move byte into memory.

	shr	al,1			; point to next byte if done
	adc	di,0			; with the current one.

	and	bx,7
	mov	al,bl			; RETURN AL = bit offset.
	mov	bl,LineSeg_rot_bit_tbl[bx] ; RETURN BL = rotating bit mask
	ret

bitmap_draw_last_x_axial_segment  endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_vertical_line
;
; This routine is called to draw the completely vertical lines very quickly.
; It is only called in the case of a vertical line.  The location of the
; current bitmap memory byte is loaded into DS:DI and we fall through
; to bitmap_draw_last_y_axial_segment.
;
; The reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.   
;
; Entry: 
;	CX = number of bits to output.
;	AddVertStep = distance to next scanline.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current bitmap memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, flags.
;
; Registers Preserved: none.
;
; Calls: BitmapProc
;
; History:
;  Mon 20-Apr-1987 12:39:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_draw_vertical_line
; {
;    // this routine is called to draw vertical lines.
;
;    DS = seg_CurByte;			// DS:DI => current bitmap byte.
;    DI = off_CurByte;
;    AL = RotBitMask;			// get rotating bitmask.
;
;    fall through to bitmap_draw_last_y_axial_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_vertical_line	proc	near

	lds	di,CurByte		; DS:DI => current bitmap byte.
	mov	al,RotBitMask		; get rotating bit mask.
;	jmp	bitmap_draw_last_y_axial_segment
	errn$	bitmap_draw_last_y_axial_segment

bitmap_draw_vertical_line	endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_last_y_axial_segment
;
; This subroutine draws a single y_axial line segment in a bitmap. This
; routine is jumped to from y_axial_cases macro to draw the last segment
; of a non-vertical y axial line.  This routine is fallen into from
; bitmap_draw_vertical_line when drawing a vertical line.
;
; The reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.   
;
; Entry:
;	AL = rotating bit mask (RotBitMask).
;	CX = number of bits to output (BitCount).
;	DS:DI = pointer to current bitmap memory byte (CurByte).
;	AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask.
;	DS:DI = updated pointer to current bitmap memory byte.
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: none.
;
; Calls: BitmapProc
;
; History:
;  Wed 29-Apr-1987 11:04:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_draw_last_y_axial_segment
; {
;    // this routine outputs BitCount vertical bits.  therefore, the 
;    // rotating bit mask is constant for all bytes output.
;
;    while (BitCount--)
;    {
;        output byte to bitmap memory;
;        DI += AddVertStep;		// jump to next scan line.
;        check for segment overflow;
;    }
;
;    					// return with BL = rotating bitmask.
;    return();				// and DS:DI => current bitmap byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_last_y_axial_segment	proc near

;	This routine outputs CX vertical bits.  Therefore, the rotating
;	bit mask is constant for all bytes output.  The following section
;	sets up for a loop which outputs eight vertical bytes.  This loop
;	partially unrolled to save time, but not unrolled too far as to
;	waste many bytes.

;	jcxz	bitmap_y_axial_end_of_final ; be safe.
	or	cx,cx
	jnz	bitmap_y_axial_not_end_of_final
	jmp	bitmap_y_axial_end_of_final

bitmap_y_axial_not_end_of_final:
	mov	si,AddVertStep

	mov	bx,cx			; BX = BitCount
	shiftr	cx,3			; CX = number of times through loop
	inc	cx			; adjust for partial loop
	and	bx,7			; BX = remainder

;	BX = the number of bits to set for the partial loop.  In the loop 
;	below it takes 8 bytes to output each bit, so BX is multiplied by 8.

ifdef	HERCULES
	mov	dx,bx
	shiftl	bx,4			; 13 bytes per vertical grouping
	sub	bx,dx
	sub	bx,dx
	sub	bx,dx
endif
ifdef	IBM_CGA
	mov	dx,bx			; 11 bytes per vertical grouping
	shl	dx,1
	add	bx,dx
	shiftl	dx,2
	add	bx,dx
endif
	neg	bx
	add	bx,offset cs:bitmap_y_axial_end_of_final_loop
	jmp	bx

	even				; align on a word boundary.
bitmap_y_axial_loop:
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_1
	add	di,NextScanXor
bitmap_y_axial_1:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif

	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_2
	add	di,NextScanXor
bitmap_y_axial_2:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_3
	add	di,NextScanXor
bitmap_y_axial_3:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_4
	add	di,NextScanXor
bitmap_y_axial_4:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_5
	add	di,NextScanXor
bitmap_y_axial_5:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_6
	add	di,NextScanXor
bitmap_y_axial_6:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_7
	add	di,NextScanXor
bitmap_y_axial_7:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
	call	word ptr BitmapProc	; output the byte.
	add	di,si			; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_y_axial_8
	add	di,NextScanXor
bitmap_y_axial_8:
endif
ifdef	IBM_CGA
	xor	si,NextScanXor
endif
	call	word ptr OverflowProc	; check for overflow.
bitmap_y_axial_end_of_final_loop:
	loop	bitmap_y_axial_loop

bitmap_y_axial_end_of_final:
	mov	bl,al			; return BL = RotBitMask
	ret

bitmap_draw_last_y_axial_segment	endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_diagonal_line
;
; This routine is called to draw the completely diagonal lines very quickly.
; It is only called in the case of a diagonal line.  The location of the
; current bitmap memory byte is loaded into DS:DI and we fall through to
; bitmap_draw_last_diagonal_segment.
;
; The reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  This number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  by diagonal bits it is meant that both
; x and y coordinates are incremented or decremented, as necessary, at the
; same time.
;
; Entry: 
;	CX = number of bits to output (BitCount).
;
; Return:
;	BL = rotating bit mask (RotBitMask).
;	DS:DI = updated pointer to current bitmap memory byte (CurByte).
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: none.
;
; Calls: BitmapProc.
;
; History:
;  Wed 29-Apr-1987 11:04:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Mon 20-Apr-1987 12:39:00	-by-	**** ******	    [******]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_draw_diagonal_line
; {
;    // this routine is called to draw diagonal lines.
;
;    DS = seg_CurByte;			// DS:DI => current bitmap byte.
;    DI = off_CurByte;
;    AL = RotBitMask;			// get rotating bitmask.
;
;    if (moving left)
;    {
;        rotate bit mask right one bit;
;        if (done with byte)
;            move to next byte;
;        jump to next scan line;
;        check for segment overflow;
;    }
;
;    fall through to bitmap_draw_last_diagonal_segment;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_diagonal_line	proc	near

	lds	di,CurByte		; DS:DI => current bitmap byte.
	mov	al,RotBitMask		; get the bitmask.

	test	CaseFlags,STEP_LEFT
	jz	bitmap_diag_go_right

	ror	al,1			; next byte if done with this one.
	adc	di,AddVertStep		; jump to next scan line.
ifdef	HERCULES
	jns	bitmap_draw_diag_no_wrap
	add	di,NextScanXor
bitmap_draw_diag_no_wrap:
endif
ifdef	IBM_CGA
	mov	bx,NextScanXor
	xor	AddVertStep,bx
endif
	call	word ptr OverflowProc	; check for segment overflow.

bitmap_diag_go_right:
; 	jmp	bitmap_draw_last_diagonal_segment
	errn$	bitmap_draw_last_diagonal_segment

bitmap_draw_diagonal_line	endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_last_diagonal_segment
;
; This subroutine draws a single diagonal line segment in a bitmap.  This
; routine is jumped to from diagonal_cases macro to draw the last segment
; of a not completely diagonal line.  This routine is fallen into from
; bitmap_draw_diagonal_line when drawing a diagonal line.
;
; The reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  This number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  By diagonal bits it is meant that both
; x and y coordinates are incremented or decremented, as necessary, at the
; same time.
;
; Entry: 
;	AL = rotating bit mask (RotBitMask).
;	CX = number of bits to output (BitCount).
;	DS:DI = pointer to current bitmap memory byte (CurByte).
;	AddVertStep = distance to next scan line.
;
; Return:
;	BL = rotating bit mask (RotBitMask).
;	DS:DI = updated pointer to current bitmap memory byte (CurByte).
;
; Error Returns: none.
;
; Registers Destroyed: AX, CX, SI, flags.
;
; Registers Preserved: none.
;
; Calls: BitmapProc.
;
; History:
;  Wed 29-Apr-1987 11:04:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_draw_last_diagonal_segment
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
;    }
;
;    BL = rotating bit mask;
;					// return with BL = rotating bitmask,
;    return();				// and DS:DI => current bitmap byte.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_last_diagonal_segment	   proc near

	mov	si,AddVertStep
	jcxz	bitmap_diagonal_end_loop ; jump if no bits to set.

bitmap_diagonal_loop:
	call	word ptr BitmapProc	; output the byte.
	ror	al,1			; rotate bit mask.
	adc	di,si			; update destination pointer.
ifdef	HERCULES
	jns	bitmap_draw_last_diag_no_wrap
	add	di,NextScanXor
bitmap_draw_last_diag_no_wrap:
endif
ifdef	IBM_CGA
	mov	dx,NextScanXor
	xor	si,dx
endif
	call	word ptr OverflowProc	; check for segment overflow.
	loop	bitmap_diagonal_loop

bitmap_diagonal_end_loop:
	xchg	bl,al			; return BL = RotBitMask
	ret

bitmap_draw_last_diagonal_segment	   endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_x_axial_segments
;
; This subroutine is called to draw non-horizontal x axial lines.  This
; routine is called from POLYLINE.ASM.  The code for this routine resides
; in the x_axial_cases macro in POLYLINE.MAC, where a detailed explanation
; is given.
;
; The reason for breaking the x axial cases out separately is that with
; this algorithm a set number of consecutive horizontal bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.   
;
; Entry:
;	CX = HFirst (number of bits in first line segment).
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current bitmap memory location.
;	AddVertStep = bytes to next scan line.
;	BitCount = number of bits per segment.
;
; Returns:
;	CX = HLast (number of bits in last line segment).
;	DS:DI = pointer to current destination byte (CurByte).
;	AL = rotating bit mask (RotBitMask).
;
; Error Returns: None.
;
; Registers Destroyed: BX,SI,flags.
;
; Registers Preserved: None.
;
; Calls: BitmapProc.
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
; bitmap_draw_x_axial_segments
; {
;    // x_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_x_axial_segments	  proc near

;	The x_axial_cases macro contains the line drawing code for
;	this case.  The 0,0 means solid line, small bitmap.

	x_axial_cases  0,0		; go draw the line.

bitmap_draw_x_axial_segments	  endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_y_axial_segments
;
; This subroutine is called to draw non-vertical y axial lines.  This
; routine is called from POLYLINE.ASM.  The code for this routine resides
; in the y_axial_cases macro in POLYLINE.MAC, where a detailed explanation
; is given.
;
; The reason for breaking the y axial cases out separately is that with
; this algorithm a set number of consecutive vertical bits can be set at
; once.  This number (BitCount) is calculated before this proc is used, so
; the output process is made faster.  Completely vertical lines are not 
; handled by this procedure.  They are handled faster by
; bitmap_draw_last_y_axial_segment.
;
; Entry:
;	CX = HFirst (number of bits in first line segment).
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current bitmap memory location.
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
; Calls: BitmapProc.
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
; bitmap_draw_y_axial_segments
; {
;    // y_axial_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_y_axial_segments	  proc near

;	The y_axial_cases macro contains the line drawing code for
;	this case. The 0,0 means solid line, small bitmap.

	y_axial_cases	 0,0		; go draw the line.

bitmap_draw_y_axial_segments	  endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_diag_x_major_segments
;
; This subroutine is called to draw diagonal x major lines.  This routine
; is called from POLYLINE.ASM.  The code for this routine resides in the
; diagonal_cases macro in POLYLINE.MAC, where a detailed explanation is
; given.
;
; The reason for breaking the diagonal cases out separately is that with
; this algorithm a set number of consecutive diagonal bits can be set at
; once.  This number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  By diagonal bits it is meant that both
; x and y coordinates are incremented or decremented, as necessary, at
; the same time.
;
; Entry:
;	CX = HFirst (number of bits in first line segment).
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current bitmap memory location.
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
; Calls: BitmapProc.
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
; bitmap_draw_diag_x_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_diag_x_major_segments    proc near

;	The diagonal_cases macro contains the line drawing code for
;	this case. The 0,0,0 means x-major, solid line,	small bitmap.

	diagonal_cases	   0,0,0	; go draw the line.

bitmap_draw_diag_x_major_segments    endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_draw_diag_y_major_segments
;
; This subroutine is called to draw diagonal y major lines.  This routine
; is called from POLYLINE.ASM.  The code for this routine resides in the
; diagonal_cases macro in POLYLINE.MAC, where a detailed explanation is
; given.
;
; The reason for breaking the y axial cases out separately is that with
; This algorithm a set number of consecutive diagonal bits can be set at
; once.  This number (BitCount) is calculated before this proc is called, so
; the output process is made faster.  By diagonal bits it is meant that
; both x and y coordinates are incremented or decremented, as necessary,
; at the same time.
;
; Entry:
;	CX = HFirst (number of bits in first line segment).
;	SI = ErrTerm.
;	DDAcount = number of segments in polyline to be drawn.
;	RotBitMask = rotating bit mask.
;	CurByte = pointer to current bitmap memory location.
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
; Calls: BitmapProc.
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
; bitmap_draw_diag_y_major_segments
; {
;    // diagonal_cases macro contains the line drawing code for this case.
;
;    go draw the line;
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_draw_diag_y_major_segments    proc near

;	The diagonal_cases macro contains the line drawing code for
;	this case. The 1,0,0 means y-major, solid line,	small bitmap.

	diagonal_cases	   1,0,0	; go draw the line.

bitmap_draw_diag_y_major_segments    endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_set_to_zero
;
; Output byte such that any bits set in the bitmask will get set to zero.
;
; Entry:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Returns:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Error Returns: None.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX,DS,DI
;
; Calls: None
;
; History:
;  Thu 05-Mar-1987 12:39:41	-by-	**** ******	    [******]
; wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_set_to_zero
; {
;    // set masked bits to zero in destination byte.
;
;    destination byte &= (! bitmask);	// zero out appropriate bits.
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_set_to_zero  proc    near

	not	al
	and	[di],al 		; zero out the appropriate bits.
	not	al
	ret

bitmap_set_to_zero  endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_set_to_one
;
; Output byte such that any bits set in the bitmask will get set to one.
;
; Entry:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Returns:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Error Returns: None.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX,DS,DI
;
; Calls: None
;
; History:
;  Thu 05-Mar-1987 12:39:41	-by-	**** ******	    [******]
; wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_set_to_one
; {
;    // set masked bits to one.
;
;    destination byte |= bitmask;	; set appropriate bits to ones.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_set_to_one   proc    near

	or	[di],al 		; set appropriate bits to ones.
	ret

bitmap_set_to_one   endp
page

;--------------------------Public-Routine-------------------------------;
; bitmap_not_dest
;
; Output byte such that any bits set in the bitmask will get inverted.
;
; Entry:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Returns:
;	AL = bitmask
;	DS:DI => CurByte.
;
; Error Returns: None.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX,DS,DI
;
; Calls: None
;
; History:
;  Thu 05-Mar-1987 12:39:41	-by-	**** ******	    [******]
; wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_not_dest
; {
;    // invert the masked bits in the destination byte.
;
;    destination byte ^= bitmask;	// invert masked bits.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

bitmap_not_dest     proc    near

	xor	[di],al 		; zero out the appropriate bits.
	ret

bitmap_not_dest     endp
page

;--------------------------Public-Routine-------------------------------;
; check_segment_overflow
;
; This routine checks and corrects for segment overflow when drawing a line
; out to a huge memory bitmap.  Whenever a line cannot be drawn in one 
; segment the address of this routine is loaded into OverflowProc.
;
; This routine does not worry about overflow in the X direction, because
; bitmaps are stored by consecutive scan lines, making X overflow 
; impossible.  This routine is called every time a move is made in the Y
; direction while writing to a huge bitmap (assuming the line is drawn 
; in multiple memory segments).  If a bitmap boundary was crossed then the
; segment and offset are updated appropriately; otherwise, nothing is done.
;
; Entry:
;	DS:DI => current memory byte.
;
; Returns:
;	DS:DI => corrected current memory byte.
;
; Error Returns: None.
;
; Registers Destroyed: flags.
;
; Registers Preserved: AX,DX
;
; Calls: None
;
; History:
;  Wed 29-Apr-1987 16:39:00	-by-	**** ******	    [******]
; wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; check_segment_overflow
; {
;    //  this routine checks and corrects for segment overflow when drawing
;    // a line out to a huge memory bitmap.  whenever a line cannot be drawn
;    // in one segment the address of this routine is loaded into
;    // OverflowProc.
;
;    // this routine does not worry about overflow in the X direction,
;    // because bitmaps are stored by consecutive scan lines, making X
;    // overflow impossible.  this routine is called every time a move
;    // is made in the Y direction while writing to a huge bitmap (assuming
;    // the line is drawn in multiple memory segments).  if a bitmap boundary
;    // was crossed then the segment and offset are updated appropriately.
;    // otherwise nothing is done.
;
;    convert Y coordinate relative to current segment;
;
;    if (line is moving down)
;    {
;        // if the line is moving down, the last scan line in the segment
;        // is ScansSeg - 1.  the check is set up so when the next scan line
;        // is accessed, its number (segment relative) will be zero.  (ie
;        // it is the first scan line of the next segment.
;
;        if (Y == 0)
;        {
;            DI += FillBytes;		// skip over fill bytes at end of seg.
;            DS += SegIndex;		// jump to the next segment.
;        }
;    }
;
;    else // line is moving up.
;    {
;        if (Y == ScansSeg - 1)
;        {
;            DI -= FillBytes;		// skip over fill bytes at end of seg.
;            DS -= SegIndex;		// jump back one segment.
;        }
;    }
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

check_segment_overflow	proc	near

	push	ax
	push	dx

	test	CaseFlags,STEP_UP	; see if line moving up or down.
	jnz	switch_segment_moving_up


;	Calculate the Y coordinate.  Then calculate it relative to the
;	current segment.  This part of the routine is called only after
;	having moved down one scan line.  Therefore, the relative Y
;	cannot be zero for the current segment.  However, when we hit
;	the fill bytes, or the start of the next segment, the relative
;	Y is calculated to be zero, and that is when we know to move to
;	the next segment.

	xor	dx,dx			; set up for divide.
	mov	ax,di			; AX = destination byte.
	div	NextScan		; get the Y coordinate.
	xor	dx,dx
	div	ScansSeg		; DX = Y relative to its own segment.

	or	dx,dx			; we only want borderline cases.
	jnz	end_of_check
	add	di,FillBytes		; skip over fill bytes.
	mov	dx,ds			; get current segment
	add	dx,SegIndex		; update segment.
	mov	ds,dx
	assumes	ds,nothing
	jmp	short end_of_check


;	Calculate the Y coordinate.  Then calculate it relative to the
;	current segment.  This part of the routine is called only after
;	having moved up one scan line.  Therefore, the relative Y cannot
;	be ScansSeg for the current segment.  When stepping up, and we
;	hit the fill bytes, the relative Y is still calculated to be zero.
;	In order to "get to" the last scan line of the previous segment
;	we need to jump over the fill bytes.

switch_segment_moving_up:
	xor	dx,dx			; set up for divide.
	sub	di,FillBytes
	mov	ax,di			; AX = destination byte.
	div	NextScan		; get the Y coordinate.
	add	di,FillBytes
	xor	dx,dx
	div	ScansSeg		; DX = Y relative to its own segment.

	mov	ax,ScansSeg
	dec	ax
	cmp	dx,ax
	jne	end_of_check		; the only case we care about.

	sub	di,FillBytes		; skip over fill bytes.
	mov	dx,ds			; get current segment.
	sub	dx,SegIndex		; update segment.
	mov	ds,dx
	assumes	ds,nothing

end_of_check:
	pop	dx
	pop	ax

	ret

check_segment_overflow	endp
page

;--------------------------Public-Routine-------------------------------;
; dont_check_overflow
;
; This routine simply does a near return.  When the destination is a small
; bitmap, or when it is a huge bitmap, but the entire line can be drawn in
; one segment, then there is no need to check for segment overflow.  In the
; above cases, the address of this routine is loaded into OverflowProc.
;
; Entry:
;	None.
;
; Returns:
;	None.
;
; Error Returns: None.
;
; Registers Destroyed: None.
;
; Registers Preserved: None.
;
; Calls: None
;
; History:
;  Wed 29-Apr-1987 16:26:00	-by-	**** ******	    [******]
; wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; dont_check_overflow
; {
;    // this routine simply does a near return.  when the destination is a
;    // small bitmap, or when it is a huge bitmap, but the entire line can be
;    // drawn in one segment, then there is no need to check for segment 
;    // overflow.  in the above cases, the address of this routine is loaded
;    // into OverflowProc.
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

dont_check_overflow	proc	near

	ret				; do nothing.

dont_check_overflow	endp


sEnd	LineSeg

ifdef	PUBDEFS
	include polybitm.pub
endif

	end
