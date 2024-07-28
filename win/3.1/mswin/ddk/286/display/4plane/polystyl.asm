

;-----------------------------------------------------------------------------;
;		              POLYSTYL.ASM			              ;
;		              ------------			              ;
; This file has styled line drawing subroutines for EGA and BITMAPs. The      ;
; file is included in POLYLINE.ASM				              ;
;					                                      ;
;                           						      ;
;                       STYLED LINE DRAW ROUTINES                             ;
;                       -------------------------                             ;
;  There are 6 different line drawing styles and each of them can be assoc-   ;
;  -iated with a byte mask as shown below:				      ;
;						                              ;
;             STYLE NAME                    BYTE MASK			      ;
;	      ----------		    ---------			      ;
;	       PS_SOLID                (excluded from styling)                ;
;              PS_DASH                       11001100	                      ;
;	       PS_DOT		             10101010			      ;
;              PS_DASHDOT                    11100100			      ;
;              PS_DASHDOTDOT                 11101010			      ;
;	       PS_NULL			     00000000			      ;
;							                      ;
; each one or a zero in the mask actually represents a set of pixel on the    ;
; screen or the map. The no of pixels that each mask bit represents would     ;
; depend on the direction the line is being drawn.			      ;
;					                                      ;
; The logic adopted is as follows:				              ;
;             						                      ;
;	   . based on acpect ratio tec the following magic numbers have been  ;
;            arrived at:				                      ;
;	                  . 48 units alon X is equivalent to 38 units along Y ;
;                           which is equivalent to 61 units along the diagonal;
;								              ;
;                         . The bit from the mask that we select depends on   ;
;                           the direction and how long we have been using the ;
;                           previous bit in the mask. An initial style error  ;
;                           term 0d 122 (2 * 61) has been assumed.            ;
;								              ;
;                         . These figures ae so chosen that the do not exceed ;
;		            127 as the signed jumps may be used               ;
;									      ;
;		          . After processing one pixel, say in the X direction;
;                           48 is added to the style error. Once the style    ;
;                           error crosses 256, the next bit in the style is   ;
;                           selected and the high byte of the error term neg- ;
;                           -lected to bring the eroor below 256              ;
;	   								      ;
;          . The gaps in the style can either be transparent or opaque. If    ;
;            the gap is opaque we have to draw the gaps in a separate pass    ;
;            where we use inverse of the mask to decide which bits are to be  ;
;            set on and which are to be skipped.		              ;
;								              ;
;          . Routines for drawing are very similar to what had been used for  ;
;            solid lines                                                      ;
;							                      ;
;	   . Move routines are not affected by line styling and remain the    ;
;            same.						              ;
;									      ;
;-----------------------------------------------------------------------------;

comment ~
		  EGA STYLED LINE DRAWING ROUTINES       
		  -------------------------------- 

INPUTS:
         DS:DI    ----	  current byte in EGA memory
         BL       ----    rotating bit mask
	 SI	  ----    distance to next scan line
         DX       ----    address of the graphics controller data register
   bStyleError    ----    current value of style error term
    bStyleMask    ----    current position of rotating style mask
    wDistHoriz    ----    scale factor in HORIZONTAL direction (=48)
     wDistVert    ----    scale factot in VERTICAL direction (=38)
     wDistDiag    ----    scale factot in diagonal direction (=61)

RETURNS:
	 DS:DI, BL and wStyleError are updated.
         DX,SI     retains their values
         CX,AX,BH  destroyed

The draw routines in Positive_Y and Diagonal_1Q direction simply negates the
distance to the next scan line and calls the corresponding routines in the 
opposite direction.

end comment ~
;-----------------------------------------------------------------------------;

St_Negative_Y	proc	near

; load the relevant work values into the registers

	mov	dx,EGA_GRAPH+1  ; load graphics controller data reg address
	mov	al,bl		; the rotating mask
	out	dx,al		; set up tha BIT MASK register for vert draw   
	mov	bl,bStyleError	; current value of the error term
	mov	ah,bStyleMask	; get the current mask (MSB is the one to see)
	
; go through the loop
		
	dec	cx		; last pixel to be drawn separately
	jz	st_vert_last
st_vert_loop:
	or	ah,ah		; test for msb set
	jns	st_vert_ignore  ; if reset, the pixel is not to be drawn
	or	byte ptr [di],0ffh ; the bit is drawn
st_vert_ignore:
	add	di,si		; go on to the next scan line
	

; now test to see if the style mask is to be advanced or not 

	xor	bh,bh		; will hold the highbyte of error addition
	add	bx,wDistVert    ; add the scale factor
	xchg	cl,bh		; if BH=1, the mask is to be rotated
	rol	ah,cl		; rotate the mask if necessary
	mov	cl,bh		; get back count
	loop	st_vert_loop    ; bl has relevant part of error term

st_vert_last:

; now draw the last pixel if the mask says so

	or	ah,ah	
	jns	st_vert_return  ; donot draw the last pixel
	or	byte ptr [di],0ffh 
	

st_vert_return:
	
; now save the value for the style error and current position of the mask 

	mov	bStyleError,bl
	mov	bStyleMask,ah
	mov	bl,al		; get back the bit mask
	ret

St_Negative_Y	endp		
;-----------------------------------------------------------------------------;
St_Positive_X	proc	near

; load relevent values into the registers

        mov	dx,EGA_GRAPH+1	
	push	si		; will actually not need the value here
	mov	al,bl		; the rotating bit mask
	mov	bl,bStyleError	; current value of the error term
	mov	ah,bStyleMask   ; current position of the rotating style mask
	mov	si,wDistHoriz   ; load the vertical scale factor
	
; the draw loop

	dec	cx		; leave out the last pixel from the loop	
        jz	st_horiz_last

st_horiz_loop:
	
; draw the pixel if the mask says so

	or	ah,ah		; the msb of the mask is the one to inspect
	jns	st_horiz_ignore ; pixel not to be drawn
	out	dx,al		; set up the BIT MASK register
	or	byte ptr [di],0ffh ; set the pixel on

st_horiz_ignore:
	
	ror	al,1		; set up bitmask for next bit
	adc	di,0		; next byte if wrap aroud occurs

; advance the mask if necessary

	xor	bh,bh		; zero out high byte for add result
	add	bx,si		; add the horizontal scale factor to error
	xchg	cl,bh		; if BH is 1, the mask has to be rotated
	rol	ah,cl		; rotate the mask
	mov	cl,bh		; get back the count
	loop	st_horiz_loop	; process all the pixels

; draw the last pixel if the mask says so.

st_horiz_last:
	or	ah,ah		; test the mask
	jns	st_horiz_return ; not to be drawn
	out	dx,al		; set up the mask
	or	byte ptr [di],0ffh
	
st_horiz_return:
	
; save the current values of the error term and the style mask

	mov	bStyleError,bl
	mov	bStyleMask,ah
	mov	bl,al		; get back the pdated value of rotating bitmask
	pop	si		; restore saved value
	ret

St_Positive_X	endp
;-----------------------------------------------------------------------------;
St_Diagonal_4Q	proc	near 

; load the relevant values into registers

	mov	dx,EGA_GRAPH+1
	mov	al,bl		; the rotating bit mask
	mov	bl,bStyleError	; the style error value
	mov	ah,bStyleMask	; current position of rotating style mask

; the main draw loop is now being set up

        dec	cx		; leave out the last pixel from the loop
	jz	st_diag_last

st_diag_loop:
	or	ah,ah		; the MSB of the mask is to be inspected
	jns	st_diag_ignore  ; mask does not permit to draw
	out	dx,al		; set up the bit mask register
	or	byte ptr [di],0ffh
st_diag_ignore:
	ror	al,1		; select the next bit position
	adc	di,si		; the next scan line

; advance the style mask if necessary

	xor	bh,bh		; will hold the hybyte of err term addition
	add	bx,wDistDiag    ; scale factor along diagonal
	xchg	cl,bh		; if BH = 1, the style mask is to be rotated
	rol	ah,cl		; rotate mask if necessary
	mov	cl,bh		; restore loop count
	loop	st_diag_loop

; draw the last pixel if the mask says so.

st_diag_last:
	or	ah,ah		; test MSB to decide
	jns	st_diag_return  ; do not draw
	out	dx,al		; set up bit mask value
	or	byte ptr [di],0ffh

st_diag_return:
	
; update the values of the style error and the style mask

	mov	bStyleError,bl
	mov	bStyleMask,ah
	mov	bl,al		; updated value of rotating bit mask
	ret

St_Diagonal_4Q	endp
;-----------------------------------------------------------------------------;
	
comment ~
		  BITMAP STYLED LINE DRAWING ROUTINES       
		  ----------------------------------- 

INPUTS:
         DS:DI    ----	  current byte in EGA memory
         BL       ----    rotating bit mask
   bStyleError    ----    current value of style error term
    bStyleMask    ----    current position of rotating style mask
    wDistHoriz    ----    scale factor in HORIZONTAL direction (=48)
     wDistVert    ----    scale factot in VERTICAL direction (=38)
     wDistDiag    ----    scale factot in diagonal direction (=61)
      NextScan    ----    (add) offset to next scanline in direction of move

RETURNS:
	 DS:DI, BL and wStyleError are updated.
         SI     retains their values
         CX,BH  destroyed
         AX     number of scanlines left from bottom

The draw routines in Positive_Y and Diagonal_1Q direction simply negates the
distance to the next scan line and calls the corresponding routines in the 
opposite direction.

end comment ~
;---------------------------------------------------------------------------;
StBm_Negative_Y	proc	near

	mov	si,ScansLeftFromBot ; scans left from bottom
        mov	al,bl               ; get rotating bit mask
	mov	ah,al		    ; also into ah
	and	ax,wBitmapROP       ; get the mask into the AND,XOR msk 
	not	al		    ; invert the AND mask
        mov	dl,bStyleError      ; the current value of style error
        mov	bh,bStyleMask       ; current position of style mask
	dec	cx	            ; take out last pixel from loop
	jz	StBm_Y_last_pixel
StBm_Y_loop:
	or	bh,bh		    ; does mask permit bit to be set on
	jns	stbm_vert_ignore    ; no
	and	[di],al
	xor	[di],ah		    ; process one pixel

stbm_vert_ignore:

; advance the style mask
		
	xor	dh,dh		    ; will hold the hibyte of err add
	add	dx,wDistVert	    ; sacle factor in vertical dir  
	xchg	cl,dh		    ; if DH = 1, the mask has to shift
	rol	bh,cl		    ; rotate if necessary
	mov	cl,dh		    ; restore count

; now move to the next scan line

	add	di,cScanMove	  ; next scan
	dec	si		  ;decrement no of scans left from bot
	loopnz	StBm_Y_loop
	jnz 	StBm_Y_last_pixel ; do the last pel
	mov	si,ds		  ; get current segment	
	add	si,selHugeDelta   ; positive or negative offset
	mov	ds,si
	add	di,cbHugeScanDelta; bypass the filler bytes at end   
	mov	si,wScans         ; number of scans in full segment
	or	cx,cx		  ; still more to do ?
	jnz	StBm_Y_loop	  ; yes

; do the last pixel

StBm_Y_last_pixel:

	or	bh,bh		    ; can last pixel be set on
	jns	stbm_vert_return    ; no
	and	[di],al
	xor	[di],ah		    ; process the last pixel
stbm_vert_return:
	mov	bStyleError,dl	    ; save
	mov	bStyleMask,bh	    ; save style mask
	mov	ScansLeftFromBot,si ;save updated value
	ret

StBm_Negative_Y	endp
;-----------------------------------------------------------------------------;
StBm_Positive_X proc	near

; load relevent values into the registers

	mov	al,bl		; the rotating bit mask
	mov	ah,al		; get it into ah also for AND,XOR masking
	and	ax,wBitmapROP   ; do the AND,XOR masking for selected bit
	not	al		; adjust the AND mask to not affect other bits
	mov	dl,bStyleError	; current value of the error term
	mov	bh,bStyleMask   ; current position of the rotating style mask
	mov	si,wDistHoriz   ; load the vertical scale factor
	
; the draw loop

	dec	cx		; leave out the last pixel from the loop	
        jz	stbm_horiz_last

stbm_horiz_loop:
	
; draw the pixel if the mask says so

	or	bh,bh		; the msb of the mask is the one to inspect
	jns	stbm_horiz_ignore ; pixel not to be drawn
	and	[di],al	        ; use the AND mask
	xor	[di],ah	        ; use the XOR mask

stbm_horiz_ignore:
	
	ror	al,1		; set up bitmask for next bit
	ror	ah,1	
	ror	bl,1		; update for final return
	adc	di,0		; next byte if wrap aroud occurs

; advance the mask if necessary

	xor	dh,dh		; zero out high byte for add result
	add	dx,si		; add the horizontal scale factor to error
	xchg	cl,dh		; if BH is 1, the mask has to be rotated
	rol	bh,cl		; rotate the mask
	mov	cl,dh		; get back the count
	loop	stbm_horiz_loop	; process all the pixels

; draw the last pixel if the mask says so.

stbm_horiz_last:
	or	bh,bh		; test the mask
	jns	stbm_horiz_return ; not to be drawn
        and	[di],al		; use the AND mask
	xor	[di],ah		; use the XOR mask	
stbm_horiz_return:
	
; save the current values of the error term and the style mask

	mov	bStyleError,dl
	mov	bStyleMask,bh
	ret

StBm_Positive_X	endp
;-----------------------------------------------------------------------------;
StBm_Diagonal_4Q proc	near

        mov	si,ScansLeftFromBot; get the no of scan lines left
	mov	al,bl		; get rotating bit mask
	mov	ah,al
	and	ax,wBitmapROP   ; combine with the ROP masks
	not	al		; adjust the AND mask
	mov	dl,bStyleError  ; the current value of the style error
	mov	bh,bStyleMask   ; current value of the style mask
	dec	cx		; leave out the last pixel
	jz	StBm_diag_last_pixel
StBm_Diag_loop:
	or	bh,bh		; test mask MSB
	jns	stbm_diag_ignore  ; ignore this pel
	and	[di],al
	xor	[di],ah		; process 1 pixel
stbm_diag_ignore:

; advance the style mask if  necessary

	xor	dh,dh		; will hold the high byte of err add
	add	dx,wDistDiag    ; scale distance along diagonal
	xchg	cl,dh		; if DH=1, style mask has to rotate
	rol	bh,cl		; rotate the mask 
	mov	cl,dh		; restore count

; move to next scan

	ror	al,1
	ror	ah,1
	ror	bl,1		; the rotating bit mask updated
	adc	di,cScanMove
	dec	si		; one more scan done
	loopnz	StBm_Diag_loop	; continue
	jnz	StBm_diag_last_pixel

; next huge segment

	mov	si,ds
	add	si,selHugeDelta ; offset of required segment
	mov	ds,si
	add	di,cbHugeScanDelta; no of left over bytes
	mov	si,wScans	; new value of number of scans left
	or	cx,cx		; more to do ?
	jnz	StBm_Diag_loop	; yes

; do the last pixel

StBm_diag_last_pixel:
		
	or	bh,bh		; does mak permit last pixel
	jns	stbm_diag_return ; no
	and	[di],al
	xor	[di],ah
stbm_diag_return:
	mov	bStyleError,dl	; save the current value of the error
	mov	bStyleMask,bh	; save value of rotating style mask
	mov	ScansLeftFromBot,si; save the no of scans left
	ret

StBm_Diagonal_4Q endp
;----------------------------------------------------------------------------;
