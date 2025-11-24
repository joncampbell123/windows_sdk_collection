;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;-----------------------------------------------------------------------------;
;		          PLYSOLID.ASM		                              ;
;                         ------------			                      ;
; This file contains the draw for EGA solid lines and the EGA move routines   ;
; The file is included in POLYLINE.ASM			                      ;
;					                                      ;
comment ~


Breshenham's algorithm requires moves in one of the eight basic directions,
which are:

        Positive_X              : towards increasing X value, horizontally
        Negative_X              : horizontally towards decreasing X value
        Positive_Y              : vertically towards top of display memory
        Negative_Y              : vertically towards end of display memory
        Diagonal_1Q		: diagonally in first quadrant
	Diagonal_2Q		: disgonally in second quadrant
        Diagonal_3Q		: diagonnaly in third quadrant
	Diagonal_4Q		: diagonnaly in fourth quadrant

We modify the end coordinates to ensure that lines never move to the left. This
manipulation allows us to dispense of with three of the 8 directions that    
involve movement along negative X direction.

Furthermore, the routine for Positive_Y will exactly be similar to the one for
Negative_Y, if we negate the BYTES_PER_LINE parameter and hence move towards
lower EGA DISPLAY MEMORY address with the same ADD instruction. This is also
true for Diagonal_1Q routine, which with the same manipulation can jmp off
to the Diagonal_4Q routine. The offset to the next scan line will actually be 
neagated before the draw routines, so we can dispense of with Positive_Y and
Diagonal_1Q. This is also true for the move routines.


The routines share a common set of parameters and return values

Inputs:

	DS:DI			: points to current byte in display memory
	BL			: bit position in current byte
	CX			: no of bits to set on in the specific dir.    
	DX			: address of the GRAPHICS CONTROLLER DATA reg.
	

Outputs:
	
	DS:DI			: points to the current byte after drawing line
	BL			: bit position after line drawing
	DX,SI			: unchanged
	AX,CX,BH		: destroyed

Assumptions on EGA register values:

	. WRITE MODE 0   has been programmed
        . SET/RESET registers disabled
	. COMBINE function set to REPLACE, SHIFT function to NULL
	. GRAPHICS CONTROLLER ADDRESS REGISTER contains index of BIT MASK
	  REGISTER
	. PALLETE REGISTER 0 and 15 set to the desired background and
          fore ground colours
        . SEQUENCER bit map mak register enables write to all planes

The code for each direction has carefully been planned out to save on cycles
(at the expense of bytes) 

end comment ~
;					                                      ;
;-----------------------------------------------------------------------------;


comment ~

Negative_Y:

                 1. A small loop in the routine is used to draw 8 bits at a 
                    time. However the loop is opened up to output each bit
                    separately and the code for each bit takes 4 BYTES.

                 2. The last bit is always set on after the loop, so CX
                    is decremented by 1 to start with.
                  
                 3. CX is then divided by 8 to get a quotient and a remainder
                    The quotient would actually give the number of times the
                    SET_8_PIXELS loop is to be traversed. The Remainder gives
                    the last few bits (<8) to set on.
                  
                 4. To avoid having to write code to set on the last few bits
                    the following trick was done:

                        CX in step 3 is decremented by 1 before the divide
                        and the remainder incremented after the divide, this
                        ensures that the remainder will never be 0 but may
                        be 8.

                        Remember the SET_8_PIXEL loop has 8 different points
                        ,each of 4 bytes, to set on each of the 8 bits.

                        The entry point into the loop is modified so that the
                        first pass actually draws the partial bits. To do this
                        simply multiply the remainder by 4 (4 bytes for each
                        bit to be set on) and enter at that offset from the 
                        end of the loop.

                        Since the loop is also being used to draw the partial
                        bits, the quotient of the division is incremented by
                        one to account for the extra pass.
                                
end comment ~ 

Negative_Y	proc	near

	mov	bh,0ffh		; will be used to OR with the FILTER
	mov	al,bl		; save the rotating FILTER

; The GRAPHICS CONTROLLER ADDRESS REGISTER has the index of the BIT MASK 
; REGISTER, and DX points to the DATA REGISTER. So to set up FILTER simply
; output desired bit FILTER pattern

	out	dx,al		; set up FILTER for the selected bit
	dec	cx		; CX now has number of lines to move
	jnz	@f		; intermediate steps too

	or	[di],bh		; set on the last pixel
	ret

@@:

	cmp	cx,8		; less than or eq 8 pels ?
	ja	Y_multiple_scans;no.

;
; multiply AX by 4 and subtract from the offset of the end of the loop to 
; obtain the entry point into the loop for the first pass
;

	shiftl	cx,2
	neg	cx
	add	cx,offset lt_8_negative_end_of_loop
	jmp	cx		; jump into the calculated entry point

	even			; allign on word boundary

lt_8_negative_set_8_pixels:

	or	[di],bh		; set a bit on
	add	di,si		; next scan line

; the above code takes 4 bytes

	.errnz ($ - lt_8_negative_set_8_pixels) - 4

	or	[di],bh		; similar code for next 7 bits follow
	add	di,si

 	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si
	
	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

lt_8_negative_end_of_loop:

	or	[di],bh		; set on the last pixel
	ret

Y_multiple_scans:

        dec	cx		; decremented to ensure remainder != 0

	mov	ax,cx
        shiftr	cx,3		; no of complete loop traversals
	inc	cx		; account for the partial traversal
	and	ax,7		; get remainder
	inc	ax		; takes care of the DEC done above
;
; multiply AX by 4 and subtract from the offset of the end of the loop to 
; obtain the entry point into the loop for the first pass
;

	shiftl	ax,2
	neg	ax
	add	ax,offset negative_end_of_loop
	jmp	ax		; jump into the calculated entry point

	even			; allign on word boundary

negative_set_8_pixels:
	or	[di],bh		; set a bit on
	add	di,si		; next scan line

; the above code takes 4 bytes

	.errnz ($ - negative_set_8_pixels) - 4

	or	[di],bh		; similar code for next 7 bits follow
	add	di,si

 	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si
	
	or	[di],bh
	add	di,si

	or	[di],bh
	add	di,si

negative_end_of_loop:
	loop	negative_set_8_pixels

negative_vertical_last_pixel:
	or	[di],bh		; set on the last pixel
	ret

Negative_Y	endp

;----------------------------------------------------------------------------; 
comment ~

Positive_X

If CX has the no of bits to set on, dec CX to get the bits to move in the 
specified direction.

Divide CX by 8 to get a quotient and a remainder. The quotient gives the
number of bytes to move and the strting bit position shifted in the specified
direction by the remainder gives the ending bit position.

All bits between the starting bit position and the ending bit position have
to be set on. Hence,

        if bh has the starting bit position and bl the ending bit

        MASK FOR FIRST BYTE = 2 * BL - 1  (set all bits to the right and incl-
                                           uding the starting bit to 1)
        MASK FOR LAST BYTE  = neg BH      (sets all bits to the left of and
                                           including the original bit to 1)

        MASK FOR ALL INTERVENING BYTES = 0ffh

Incase the starting and ending bits fall on the same byte, AND the initial
and final FILTERs.      

Note, that if the position in a byte of the final bit is to the left of
the position of the initial byte, the quotient above has to be incremented
by 1. The quotient after this update gives one less the number of bytes
affected. 

end comment ~

Positive_X	proc	near

	dec	cx		; CX has number of bits to move
	cmp	cx,7		; if <= 7 the atbest 2 bytes will be done
	ja	X_multiple_bytes; more than 2 bytes to touch
	mov	bh,bl		; save the starting bit position in bh
	ror	bl,cl		; bl now has the position of end bit
	mov	ax,bx
	xchg	al,ah		; al initial bit, ah final bit
	add	al,al
	dec	al		; FILTER for start byte will have all bits 1 to
				; the left and including the start bit
	neg	ah		; 2's complement ensures that the FILTER for the
				; last byte has 1's to the left of and         
				; including the last bit
	cmp	bh,bl		; adjust the count incase of byte wrap
	jc	X_two_bytes	; intermediate bytes present
	and	al,ah		; incase the start and end bits in same byte
	out	dx,al		; set up FILTER
	or	byte ptr [di],0ffh         
	ret
	
X_two_bytes:

; put out the first byte
	
	out	dx,al		; outputs the starting FILTER to BITMASKREGISTER
	mov	al,0ffh		; or with this
	or	[di],al   	; sets bits on depending on value of FILTER
	inc	di		; point to next byte
	and	al,ah		; incase the start and end bits in same byte
	out	dx,al		; set up FILTER
	or	byte ptr [di],0ffh                  
	
;
; es:di still pts to the current byte and bl still has the current pel in it
;
	ret

X_multiple_bytes:

	mov	ax,cx
	shiftr	ax,3		; divide by 8 to get the no of whole bytes in 
			        ; the extent of the line
	and	cx,7		; this is the no of bits in the last byte
				; after moving ax bytes from the starting bit
	mov	bh,bl		; save the starting bit position in bh
	ror	bl,cl		; bl now has the position of end bit
	mov	cx,ax		; save no of bytes in line  
	mov	ax,bx
	xchg	al,ah		; al initial bit, ah final bit
	add	al,al
	dec	al		; FILTER for start byte will have all bits 1 to
				; the left and including the start bit
	neg	ah		; 2's complement ensures that the FILTER for the
				; last byte has 1's to the left of and         
				; including the last bit
	cmp	bh,bl		; adjust the count incase of byte wrap
	adc	cx,0		; CX has no of bytes to touch - 1.
	jnz	@f		; intermediate bytes present
	and	al,ah		; incase the start and end bits in same byte
	out	dx,al		; set up FILTER
	or	byte ptr [di],0ffh         
	ret
	
@@:

; put out the first byte
	
	out	dx,al		; outputs the starting FILTER to BITMASKREGISTER
	mov	al,0ffh		; or with this
	or	[di],al   	; sets bits on depending on value of FILTER
	inc	di		; point to next byte
	dec	cx		; one byte done
	jz	positive_just_do_last	; the end FILTER
	out	dx,al		; for intervening byte FILTER hasall bitsenabled
positive_output_loop:
	or	[di],al  	; make all intervening bytes one
	inc	di		; next byte 
	loop	positive_output_loop ; completely fill up the intervening bytes
	
positive_just_do_last:
	and	al,ah		; incase the start and end bits in same byte
	out	dx,al		; set up FILTER
	or	byte ptr [di],0ffh                  
	
;
; es:di still pts to the current byte and bl still has the current pel in it
;

	ret

Positive_X	endp

;-----------------------------------------------------------------------------;
comment ~
Diagonal_4Q

A simple loop outputs the bits, each time going a bit to the right and a
bit down. The current bit position is used as a FILTER to set up the bits.
	     
end comment ~

Diagonal_4Q	proc	near

	test	cx,7
	jz	special_diagonal
	mov	al,bl		; get current bit position
	
; remember that the GRAPHICS CONTROLLER ADDRESS REGISTER still has the index
; of the BIT MASK REGISTER, so we can output its value into the DATA REGISTER	

        mov	bh,0ffh		; will be used to AND with the FILTER
	dec	cx
	jz	four_last_pixel   ; only one pixel to set on

four_diagonal_loop: 
	out	dx,al		; set up the FILTER as the current bit position
	or	[di],bh		; set current bit on
	ror	al,1		; next bit postion is 1 bit right
	adc	di,si		; and 1 bit down
	loop	four_diagonal_loop
four_last_pixel:
	out	dx,al		; set up the bit FILTER register
	or	[di],bh		; set it on
	mov	bl,al		; return with the last bit position in bl
	ret

public special_diagonal
special_diagonal:

	push	bp
	shiftr	cx,3		; no of bytes to touch
	mov	bh,cl		; save
	mov	cx,7		; 7 positions
	mov	bp,si
	shiftl	bp,3		;8 rows distant
	inc	bp

spec_diag_loop:

	push	cx
	push	di
	mov	cl,bh		; get the no of rows
	mov	al,bl		; get the mask
	out	dx,al		; set up the FILTER as the current bit position
@@:
	or	bptr [di],0ffh	; set current bit on
	add	di,bp		; 8 rows above 
	loop	@b		; set all rows bit same bit position
	pop	di
	ror	al,1		; next bit postion is 1 bit right
	adc	di,si		; and 1 bit down/up
	mov	bl,al		; get the mask
	pop	cx
	loop	spec_diag_loop
	mov	cl,bh		; get the no of rows
	mov	al,bl		; get the mask
	out	dx,al		; set up the FILTER as the current bit position
@@:
	or	bptr [di],0ffh	; set current bit on
	add	di,bp		; 8 rows above 
	loop	@b		; set all rows bit same bit position
	sub	di,bp		;get back to last
	pop	bp
   	ret


Diagonal_4Q	endp


