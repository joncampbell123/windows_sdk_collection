;----------------------------------------------------------------------------;
; This file contains the code for the following two functions in the palette ;
; manager module:							     ;
;		   SetPaletteEntries    ---  Sets Palette Register           ;
;                  GetPaletteEntries    ---  Gets the Palette Register values;
;----------------------------------------------------------------------------;

			public	SetPaletteEntries
			public	GetPaletteEntries


;----------------------------------------------------------------------------;
;		   	   SetPaletteEntries				     ;
;			   -----------------				     ;
;    Programs the EGA hardware palette registers to refer to a specific set  ;
;    of colors.								     ;
;  									     ;
;    Parameters:					                     ;
;                wIndex	    --     Starting Palette Register number          ;
;                wCount     --     Count of the number of registers          ;
;                lpColors   --     a long pointer to a table which has the   ;
;                                  following structure:			     ;
;			           STRUC				     ;
;				        RGB   ColorAsked 		     ;
;				        RGB   ColorGiven                     ;
;				   END STRUC   				     ;
;				   with ColorAsked filled in		     ;
;							                     ;
;    Returns:  	 							     ;
;		 Number of Registers set				     ;
;		 Fills in the ColorGiven triplets in color table	     ;
;									     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 27-Oct-1988  11:14:30     ;
;----------------------------------------------------------------------------;


RGB	STRUC
Red	db	0		; Red Value
Green	db	0		; Green Value
Blue	db	0		; Blue Value
Flag    db	0
RGB	ENDS


ColorTable	STRUC
ColorAsked	db	size RGB dup (?)	; Requested Color
; ColorGiven	db	size RGB dup (?)        ; Actual colors
ColorTable	ENDS

incDrawMode = 1				; include the drawmode definitions

	.xlist
	include	cmacros.inc
	include	macros.mac
	include gdidefs.inc
	include	display.inc
	.list

sBegin	Data
	externA	NUM_PALETTES 		; in .\egahires.asm
	externA BW_THRESHOLD	        ; in .\egahires.asm
	externW	PaletteTranslationTable ; in .\egahires.asm
	externB	PaletteModified		; in .\egahires.asm
	externB TextColorXlated         ; in .\egahires.asm
	externB device_local_brush      ; in .\egahires.asm
	externB device_local_drawmode   ; in .\egahires.asm
	externB device_local_pen	; in .\egahires.asm

sEnd	Data
createSeg _PALETTE,PaletteSeg,word,public,CODE
sBegin	PaletteSeg
        assumes cs,PaletteSeg
	assumes	ds,nothing
	assumes	es,nothing

cProc	SetPaletteEntries,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmW	wIndex		; index of the first register
	parmW	wCount		; no of registers to program
	parmD	lpColorTable	; pointer to an array of color table entries

	localW	RegCopied	; no of registers copied

cBegin
	cCall	do_validations	; do validation on the passed in parameters

;----------------------------------------------------------------------------;
; the above call sets the carry flag in case the parameters are not correct. ;
; if parameters are valid, the registers CX and BX will have the following   ;
; values:								     ;
;		BX	---     start palette register number		     ;
;               CX      ---     number of register to set		     ;
; also, DS:SI & ES:DI   ---     will point to the passed in color structure  ;
;			        SI pointing to asked colors and DI to given  ;
;                               colors.                                      ;
;----------------------------------------------------------------------------;
	jnc	set_params_ok	; all parameters ok
	jmp	short SetPaletteEntries_Ret

;----------------------------------------------------------------------------;
; The last 6 bits of the palette registers are important and have a format   ;
;          rgbRGB   ----   r,g,b are for higher intensity		     ;
; 								             ;
; Thus as every color has 2 bits in the palette, we will take the MS 2 bits  ;
; from the logical color bytes for each color.				     ;
;----------------------------------------------------------------------------;



set_params_ok:

;----------------------------------------------------------------------------;
; At this point,	                                                     ;
;                BX      ---    has the start register number		     ;
;		 CX      ---    count of registers to program		     ;
;                DS:SI   ---    points to the first asked color		     ;
;                ES:DI   ---    points to the place for the first given color;
;----------------------------------------------------------------------------;

	push	cx		; save count of registers to program
	push	bx		; save register number

	mov	bh,7		; set the intensity bits on for intensities

; convert Red to get 2 bits for the palette register and a byte to return

	mov	al,[si].Red	; get the red color (0-255)
	and	al,0c0h		; retain the 2 MS bits only
;	mov	es:[di].Red,al	; return the realized color
	shl	al,1		; get the intensity bit into carry
	rcl	bh,1		; get the intensity bit in
	shl	al,1		; get the color bit into the carry
	shl	cl,1		; get the color bit in

; convert Green to get 2 bits for the palette register and a byte to return

	mov	al,[si].Green	; get the red color (0-255)
	and	al,0c0h		; retain MS 2 bits only
;	mov	es:[di].Green,al; return the realized color
	shl	al,1		; get the intensity bit into carry
	rcl	bh,1		; get the intensity bit in
	shl	al,1		; get the color bit into the carry
	shl	cl,1		; get the color bit in

; convert Blue to get 2 bits for the palette register and a byte to return

	mov	al,[si].Blue	; get the red color (0-255)
	and	al,0c0h		; retain only the MS 2 bits
;	mov	es:[di].Blue,al	; return the realized color
	shl	al,1		; get the intensity bit into carry
	rcl	bh,1		; get the intensity bit in
	shl	al,1		; get the color bit into the carry
	shl	cl,1		; get the color bit in

; convert the intensity and the color bits
	
	shl	bh,1
	shl	bh,1
	shl	bh,1		; get the intensity bits in place
	and	bh,038h		; retain only the intensity bits
	and	cl,07h		; only the color bits
	or	bh,cl		; combine the intensity and color

; so now bh has the value to program for register bl, program it

	mov	ax,1000h	; set palette register call

; AL = 0 = function code (set palette)
; BL =     Palette register number (0 through 15)
; BH =     value for the register

	int	10h		; palette register gets programmed

; update si and di to point to their respective next entries

	add	si,size RGB
	add	di,size RGB
	
; set up values for the next iteration

	pop	bx		; get back current register number
	inc	bx  		; go to the next register
	pop	cx		; count of registers to program
	loop	set_params_ok	; program all the requested registers

; all the registers have been programmed. Return the no of registers copied
; to the caller

SetPaletteEntries_Ret:
	mov	ax,RegCopied

cEnd
;----------------------------------------------------------------------------;

cProc	do_validations,<NEAR,PUBLIC>

cBegin

;----------------------------------------------------------------------------;
;  do some local validations. VGA has a set of 16 palette registers so the   ;
;  range of registers specified should fit in the range [0..15].	     ;
;----------------------------------------------------------------------------;

	mov	RegCopied,0		; no of registers copied
	mov	bx,wIndex		; load the index and count into registers
	mov	cx,wCount
		   
	cmp	bx,NUM_PALETTES - 1
	ja	param_error		; it cant be > than the last register num
	mov	ax,bx			; we don't want the deatroy bx
	add	ax,wCount
	dec	ax			; ax has the no of the last reg to program
	cmp	ax,NUM_PALETTES - 1
	jbe	range_ok		; register range is ok
	mov	cx,NUM_PALETTES - 1
	sub	cx,wIndex
	add	cx,1			; the number tha actually will be copied
range_ok:

	lds	si,lpColorTable 	; DS:SI points to the first asked color
	mov	ax,ds
	or	ax,si			; test for a valid pointer
	jz	param_error		; can't work wih a NULL pointer
	les	di,lpColorTable
	add	di, size RGB		; ES:DI points to the place for GIVEN color
	mov	RegCopied,cx		; set the return value
	clc
	jmp	short do_validations_ret
param_error:
	stc				; indicate error
do_validations_ret:

cEnd

;----------------------------------------------------------------------------;
;		   	   GetPaletteEntries				     ;
;			   -----------------				     ;
;    Gets the current value of a set of Palette Registers.      	     ;
;							                     ;
;    Parameters:					                     ;
;                wIndex	    --     Starting Palette Register number          ;
;                wCount     --     Count of the number of registers          ;
;                lpColors   --     a long pointer to a table which has the   ;
;                                  following structure:			     ;
;			           STRUC				     ;
;				        RGB   ColorAsked 		     ;
;				        RGB   ColorGiven                     ;
;				   END STRUC   				     ;
;				   GDI could fill in the ColorAsked values   ;
;									     ;
;    Returns:  	 							     ;
;		 Number of Registers set				     ;
;		 fills in the ColorGiven triplets			     ;							     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 27-Oct-1988  14:26:20     ;
;----------------------------------------------------------------------------;

cProc	GetPaletteEntries,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmW	wIndex			; index of the first register
	parmW	wCount			; no of registers to program
	parmD	lpColorTable		; pointer to an array of color table entries

	localW	RegCopied               ; no of registers copied

; we will allocate an area in the stack to store the color values 

	localV	ColorBuffer,<NUM_PALETTES +1>

cBegin
	cCall	do_validations	        ; validate the passed in parameters
	jnc	get_params_ok	        ; parameters look to be all valid
	jmp	short GetPaletteEntries_Ret

get_params_ok:
	push	es		        ; save the segment of the color table
	lea	dx,ColorBuffer	
	mov	ax,ss
	mov	es,ax		        ; ES:DX points to a buffer to read in DAC regs

; read in the value for the 16 palette registers and the overscan register

	mov	ax,1009h	        ; int 10 code to read palette and overscan
	int	10h		        ; registers read into local buffer

; now transfer the actual colors the callers buffer

	pop	es		        ; ES:DI points to user buffer
	add	di,size RGB	        ; ES:DI poits to first ColorGiven field
	mov	ax,ss
	mov	ds,ax	
	lea	si,ColorBuffer	        ; DS:SI poits to the colors read in

; for EGA physical color is 2 bits and the palette format is rgbRGB, the
; color returned will extract the intensity and color bits for each color 
; and return it as
;			rR000000, gG000000, bB000000

; position SI to point to the first palette register of interest

	add	si,bx			; the palettes are stored in a byte table

color_xfer_loop:
      	lodsb				; get an index
	shl	al,1
	shl	al,1			; take away the dummy 2 MS bits
	shl	al,1			; shift out r
	rcl	bl,1			; get red intensity
	shl	al,1			; shift out g
	rcl	bh,1			; get green intensity
	shl	al,1			; shift out b
	rcl	ah,1			; get blue intensity
	shl	al,1			; shift out R
	rcl	bl,1			; get red color
	shl	al,1			; shift out G
	rcl	bh,1			; get green color
	shl	al,1			; shift out B
	rcl	ah,1			; get blue color

; BL,BH,AH  has the 2 color bits in the LS position, mask and get them into
; the MS position and then store them in the return array

	and	bl,03h			; last 2 bits have color
	rol	bl,1
	rol	bl,1			; get them into the 2 MS positions
	mov	es:[di].Red,bl		; save it

	and	bh,03h			; last 2 bits have color
	rol	bh,1
	rol	bh,1			; get them into the 2 MS positions
	mov	es:[di].Green,bh	; save it

	and	ah,03h			; last 2 bits have color
	rol	ah,1
	rol	ah,1			; get them into the 2 MS positions
	mov	es:[di].Blue,ah		; save it

	add	di,size RGB	        ; go to the next ColorGiven field
	loop	color_xfer_loop	        ; transfer all the colors

; we have filled in the values of all the requested registers, now return

GetPaletteEntries_Ret:
	mov	ax,RegCopied	        ; no of registers copied

cEnd

sEnd	PaletteSeg

END

;----------------------------------------------------------------------------;
