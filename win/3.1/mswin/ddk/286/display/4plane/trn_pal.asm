
;----------------------------------------------------------------------------;
; This file contains the code for the following few functions in the palette ;
; manager module:							     ;
;                  SetPaletteTranslate 	---  Sets the translate table        ;
;		   GetPaletteTranslate  ---  Gets the current translate table;
;                  TranslateBrush       ---  Tranlates indices in a brush    ;
;                  TranslateTextColor   ---  Translates color in DRAWMODE    ;
;                  TranslatePen         ---  Translates indices in a pen     ;
;                  UpdateColor          ---  Translate a rect on the screen  ;
;----------------------------------------------------------------------------;

			public	SetPaletteTranslate
			public	GetPaletteTranslate
			public	TranslateBrush
			public  TranslateTextColor
			public	TranslatePen
			public	UpdateColor



incDrawMode = 1				; include the drawmode definitions

	.xlist
	include	cmacros.inc
	include	macros.mac
	include gdidefs.inc
	include	display.inc
	include	ega.inc
	include egamem.inc
	.list

	externFP	exclude_far
	externFP	unexclude_far
	externFP	IndexToColor

sBegin	Data

; the following variables and values are defined  in .\egahires\egahires.asm
; and in .\vga\vga.asm for the ega and vga drivers respectively.

	externA	NUM_PALETTES 		
	externA BW_THRESHOLD 
	externA	SCREEN_W_BYTES
	externA	ScreenSelector
	externW	PaletteTranslationTable 
	externB	PaletteModified		
	externB TextColorXlated         
	externB device_local_brush      
	externB device_local_drawmode   
	externB device_local_pen	

sEnd	Data
createSeg _PALETTE,PaletteSeg,word,public,CODE
sBegin	PaletteSeg
        assumes cs,PaletteSeg
	assumes	ds,nothing
	assumes	es,nothing

;----------------------------------------------------------------------------;
;		   	   SetPaletteTranslate				     ;
;			   -------------------				     ;
;    Sets up the palette tranlation table in own data segment to be used     ;
;    by subsequent draw calls.                                               ;
;  									     ;
;    Parameters:					                     ;
;		 wNumEntries --	   Number of entries in the table. 	     ;
;                                  
;                lpTranlate  --    a long pointer to the translate table     ;
;			           a NULL initializes the table(in this case ;
;			           ignore the first parameter
;    Returns:  	 							     ;
;		 a zero in AX (success)					     ;
;		 Fills in the Translate table in own data segment            ;
;									     ;
;    History:								     ;
;	         Modified to suite 4 plane EGA/VGA drivers.		     ;
;		 -by- Amit Chatterjee [amitc]  Thu 01-Dec-1988  09:00:00     ;
;									     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 03_Nov-1988  13:50:15     ;
;----------------------------------------------------------------------------;

cProc	SetPaletteTranslate,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es>

	assumes	ds,Data
	assumes	es,nothing
	
	parmW	wEntries	; no of indexes in the table
	parmD	lpTranslate	; long pointer to translate table 

cBegin
	cld
	
;----------------------------------------------------------------------------;
; get a pointer to the translate table in our own data segment in ES:DI	     ;
;----------------------------------------------------------------------------;

	mov	di,ds		        ; get our own data segment
	mov	es,di
	assumes es,nothing

	mov	di,DataOFFSET PaletteTranslationTable

; ES:DI now points to the translation table to be used by the driver

	lds	si,lpTranslate	        ; get a pointer to the translation table

; DS:SI = 0 means that the translation table has to be initialised

	mov	cx,wEntries		; no of indices to copy
	mov	ax,ds
	or	ax,si		
	jz	initialize_translate_table

; GDI has passed in a valid translate table, simply copy it into own segment

	rep	movsw			; copy all the indices

; now set the palette modified flag 

	mov	ax,es
	mov	ds,ax			; get back own data segment
	assumes ds,Data

	mov	PaletteModified,0ffh	; palette has been modified
	jmp	short SetPaletteTranslate_Ret

initialize_translate_table:
	mov	cx,NUM_PALETTES		; initialize all the registers
	xor	ax,ax			; value for the first register
initialize_table_loop:
	stosw
	inc	ax			; value for the next register
	loop	initialize_table_loop

;----------------------------------------------------------------------------;
; after initialization we will reset the ModifiedPalette flag so that the    ;
; output routines will not do the 'on-the-fly' brush re-realization          ;
;----------------------------------------------------------------------------;

	mov	ax,es
	mov	ds,ax			; get back own data segment
	assumes	ds,Data

	mov	PaletteModified,0	; reset

SetPaletteTranslate_Ret:
	xor	ax,ax			; set the success code
cEnd

;----------------------------------------------------------------------------;
;		   	   GetPaletteTranslate				     ;
;			   -------------------				     ;
;    Returns the drivers palette translation table.                          ;
;  									     ;
;    Parameters:					                     ;
;                lpTranlate --     a long pointer to a buffer to store the   ;
;                                  translation table.                        ;
;    Returns:  	 							     ;
;		 a zero in AX (success)					     ;
;		 a 0FFH in AX if a NULL pointer was passed in		     ;
;		 Fills in the Translate table in the buffer provided         ;
;									     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 03_Nov-1988  14:42:50     ;
;----------------------------------------------------------------------------;

cProc	GetPaletteTranslate,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es>

	assumes	ds,Data
	assumes	es,nothing

	parmD	lpTranslate	; long pointer to translate table 

cBegin
	cld
	
	les	di,lpTranslate	; ES:DI points to GDI buffer
	mov	ax,es
	or	ax,di		; A NULL ptr is invalid
	jz	GetPaletteTranslate_Ret

; DS is already our own segment, set SI to point to the translation table

	mov	si,DataOFFSET PaletteTranslationTable

; now copy the indices

	mov	cx,NUM_PALETTES
	rep	movsw		; done
	mov	ax,1		; will be turned to zero below

GetPaletteTranslate_Ret:
	dec	ax		; set success/failure code

cEnd

;----------------------------------------------------------------------------;
;		   	   TranslateBrush				     ;
;			   --------------				     ;
;    Creates a new brush structure in local datasegment from the brush       ;
;    pattern passed in by any of the output routines and the current         ;
;    translate table.							     ;
;  									     ;
;    Parameters:					                     ;
;                lpBrush    --     a long pointer to an OEM brush structure  ;
;    Returns:  	 							     ;
;                DX:AX      --     a long pointer to the re-realized brush   ;
;									     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 03_Nov-1988  17:31:00     ;
;----------------------------------------------------------------------------;

cProc	TranslateBrush,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di,bx,cx>

	parmD	lpBrush		; pointer to the brush
cBegin
	mov	ax,ds
	mov	es,ax		; get our own segment in es
	assumes	es,Data

	mov	di,DataOFFSET device_local_brush

; es:di points to the brush area where we will put the translated bits

	xor	dx,dx		; resturn value will be NULL for error
	lds	si,lpBrush	; DS:SI points to the realized brush pattern
	assumes ds,nothing

	mov	cx,SIZE_PATTERN ; this is the number of rows to translate
	mov	bx,SIZE_PATTERN ; this is the offset to the other planes 
row_process_loop:
	
	mov	dl,[si][bx]	; get C1 plane
	add	si,bx
	mov	dh,[si][bx]	; get C2 plane
	shl	bx,1
	mov	ah,[si][bx]	; get C3 plane
	shr	bx,1		; restore bx
	sub	si,bx		; get back to original offset
	lodsb			; get C0 and si points to next row
	cCall	Convert_8_Bits  ; converts the 8 bits and stores them
	loop	row_process_loop; convert all the rows in the pattern

;----------------------------------------------------------------------------;
; we have converted the color part of the map, the mono chrome part of the   ;
; map and upto but not including the flags etc at the end of the brush will  ;
; be copied as is. We shall reset the solid brush indicator in the flag to 0 ;
;----------------------------------------------------------------------------;

	add	si,3 * SIZE_PATTERN
	add	di,3 * SIZE_PATTERN	; position at start of mono map
	mov	cx,SIZE oem_brush_def - 4 * (SIZE_PATTERN) - 3
	rep	movsb   		; rest of it transferred

; now DS:SI and ES:DI are pointing to the flag bytes, copy it by resetting
; the solid_brush indicator
	
	lodsw				; get the style
	or	ax,ax			; was it a solid brush
	jnz	donot_change_style	; we will force a solid style to pattern
	mov	ax,3			; force it to be pattern
donot_change_style:
	stosw

; we are forcing our brush to be a pattern brush even if it is solid,
; however in case of WHITENESS/BLACKNESS, PATBLT expects the color inthe
; accelarator flag, so lets get the value of pel1 and load it here, all other
; accelarator bits will be reset

	push	di			; save pointer to accelarator byte
	mov	di,DataOFFSET device_local_brush
	xor	ax,ax			; build up color here
	mov	ah,es:[di][3*SIZE_PATTERN] ; get the intensity plane
	shl	al,1			; get the first pel into carry
	adc	al,0			; accumulate into al
	mov	ah,es:[di][2*SIZE_PATTERN] ; get blue plane
	shl	ah,1			; shift of the first bit
	adc	al,al			; accumulate in al make room for green
	mov	ah,es:[di][SIZE_PATTERN]; get the green byte
	shl	ah,1			; get the first pel into carry
	adc	al,al			; accumulate in al make romm for red
	mov	ah,es:[di]		; get the red byte
	shl	ah,1			; shift out the first pel
	adc	al,al			; accumulate in al
	pop	di			; restore pointer to flag

; al has the color in case of solid brush, garbage otherwise

	stosb

; set up DX:AX to point to the structure in local memory

	mov	dx,es
	mov	ax,DataOFFSET device_local_brush

TranslateBrush_Ret:

cEnd

;----------------------------------------------------------------------------;
;		   	   Convert_8_Bits                                    ;
;			   --------------				     ;
;    This local table takes 8 bits worth of data and looks up the translate  ;
;    table, converts them and puts them into place in the brush that we are  ;
;    creating. 								     ;
;									     ;	
;    Parameters:					                     ;
;	         AL	    --     bits for plane 0			     ;
;                DL	    --     bits for plane 1                          ;
;                DH         --     bits for plane 2                          ;
;                AH         --     bits for plane 3
;                ES:DI      --     points to the row corresponding plane 0   ;
;    Returns:  	 							     ;
;	         ES:DI	    --     points to next row for plane 0, prvious   ;
;                                  row for all three planes will have the    ;
;                                  translated data.			     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Thu 03_Nov-1988  18:34:14     ;
;----------------------------------------------------------------------------;

cProc	Convert_8_Bits,<NEAR,PUBLIC>
	assumes	es,Data

cBegin	nogen

	push	cx
	push	bx		; save the registers which will be destroyed

	mov	cx,8		; we will convert 8 bits

bit_convert_loop:
	xor	bx,bx
	shl	ah,1		; get next bit for plane C3 into carry
	rcl	bx,1		; accumulate in bx
	shl	dh,1		; get the next bit for plane C2 into carry
	rcl	bx,1		; get it into bx
	shl	dl,1		; corresponding bit for plane C1
	rcl	bx,1		; accumulate it
	shl	al,1		; finally the corresponding bit for C0
	rcl	bx,1		; so BX now has the index value to translate
	shl	bx,1		; the translate table is a word table
	mov	bx,[bx][PaletteTranslationTable] ; get the new index into bl

; we assume index is only in bl and will ignore HIBYTE

	shr	bl,1		; get the bit for C0 into carry
	adc	al,0		; AL has a place at the LSB position, OR it
	shr	bl,1		; get the bit for plane C1
	adc	dl,0		; get it into the empty slot
	shr	bl,1		; get the bit for plane C2
	adc	dh,0		; get it into the empty slot
	shr	bl,1		; get the bit for plane C3
	adc	ah,0		; get it into the empty slot

; so we have basically taken a pel off the left edge of the registers, 
; converted it, and stuffed it at the right edge. By the time we have done
; this 8 times, the 4 register AL,DL,DH,AH will hold the converted value for
; planes C0,C1,C2 and C3 respectively for all 8 bits.

	loop   	bit_convert_loop 

; So now AL,DL,DH,AH do have the converted values and we will stuff them in

	mov	bx,SIZE_PATTERN
	mov	es:[di][bx],dl	; stuff in bits for plane C1
	add	di,bx
	mov	es:[di][bx],dh	; stuff in bits for plane C2
	shl	bx,1
	mov	es:[di][bx],ah	; stuff in bits for plane C3
	shr	bx,1		; restore bx
	sub	di,bx
	stosb			; stuff in bits for C0 and update DI 

	pop	bx
	pop	cx

	ret
cEnd	nogen
;----------------------------------------------------------------------------;
;		   	   TranslateTextColor				     ;
;			   ------------------				     ;
;    Here we translate the text foreground and textbackground colors which   ;
;    are stored in the drawmode structure . Also a flag will be set          ;
;    indicating that the foreground and background color in the drawmode     ;
;    have been translated. This flag will be reset when a set foreground or  ;
;    set background color call is made next.			             ;
;  									     ;
;    Parameters:					                     ;
;                lpDrawMode --     a long pointer to an drawmode structure   ;
;    Returns:  	 							     ;
;                translates the foreground and background colors in the      ;
;                structure and sets TextColorXlated flag on                  ;
;									     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Mon 07-Nov-1988  17:31:00     ;
;----------------------------------------------------------------------------;

cProc	TranslateTextColor,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di,bx,cx>
	assumes	ds,Data

	parmD	lpDrawMode	; pointer to the drawmode structure
cBegin
	mov	ax,ds
	mov	es,ax		; get our own segment in es
	assumes	es,nothing

	mov	di,DataOFFSET device_local_drawmode

	lds	si,lpDrawMode	; get the drawmode structure
	assumes	ds,nothing

; first make a complete local copy

	push	di
	push	si		; save the pointers
	mov	cx, SIZE DRAWMODE
	rep	movsb
	pop	si
	pop	di

; now translate the colors

	xor	bx,bx
	mov	bl,[si].bkColor.pcol_C3 

; now with the old index in BL we call the TranslateColor routine which 
; returns the translated index in low nibble of DH, the accelarator flags
; in the high nibble and the device specific colors (RED,GREEN & BLUE) in
; AL, AH and DL.

	cCall	TranslateColor

	mov	es:[di].bkColor.pcol_C0,al	 ; save the translated red
	mov     es:[di].bkColor.pcol_C1,ah  ; save the translated green
	mov     es:[di].bkColor.pcol_C2,dl  ; the translated blue
	mov	es:[di].bkColor.pcol_C3,dh  ; the accelarator flag

; now do a similar translation on the foreground color

	xor	bx,bx
	mov	bl,[si].TextColor.pcol_C3 

	cCall	TranslateColor

; now with the old index in BH we call the TranslateColor routine which 
; returns the translated index in low nibble of DH, the accelarator flags
; in the high nibble and the device specific colors (RED,GREEN & BLUE) in
; AL, AH and DL.

	mov	es:[di].TextColor.pcol_C0,al  ; save the translated red
	mov     es:[di].TextColor.pcol_C1,ah  ; save the translated green
	mov     es:[di].TextColor.pcol_C2,dl  ; the translated blue
	mov	es:[di].TextColor.pcol_C3,dh  ; the accelarator flag

; return local structure in DX:AX

	mov	ax,di
	mov	dx,es

cEnd

;----------------------------------------------------------------------------;
;		   	   TranslatePen					     ;
;			   ------------				             ;
;    The pen color has a structure similar to the text background and fore   ;
;    ground color and will be translated in a similar fashion.		     ;
;						                             ;
;    Parameters:					                     ;
;                lpPen		--  a pointer to a pen structure	     ;
;    Returns:  	 							     ;
;                translates the physical color in the pen		     ;
;									     ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Mon 07-Nov-1988  13:35:00     ;
;----------------------------------------------------------------------------;

cProc	TranslatePen,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di,bx,cx>

	parmD	lpPen		; a pointer to a pen color
cBegin
	mov	ax,ds
	mov	es,ax		; get our own segment in es
	assumes	es,Data

	mov	di,DataOFFSET device_local_pen

	lds	si,lpPen	; point to the pen structure
	mov	bl,[si].pcol_C3 ; get the byte which has the index in lonibble

; call a routine which does the translation as done for TextColors

	cCall	TranslateColor

; load the translated values

	mov	es:[di].pcol_C0,al	; the translated red
	mov	es:[di].pcol_C1,ah	; the translated green
	mov	es:[di].pcol_C2,dl	; the translated blue
	mov	es:[di].pcol_C3,dh	; the new index and the accelarator flags

; now copy the pen style
	
	mov	ax,[si].oem_pen_style	; load the original style
	mov	es:[di].oem_pen_style,ax; store it in local pen structure

; finally return a pointer to the local pen structure in DX:AX

	mov	dx,es
	mov	ax,di


cEnd

;----------------------------------------------------------------------------;
;		   	   TranslateColor                                    ;
;			   --------------				     ;
;    This local routine gets an old index in BL, looks up the translation    ;
;    table and gets the new index in LONIBBLE of DH. 			     ;
;    It the calls the IndexToColor routine to get the corresponding h/w      ;
;    colors into AL(red), AH(green) and DL(blue) and the accl flags in the   ;
;    high nibble of DH.						             ;It also puts the colors ;
;							                     ;
;    Parameters:					                     ;
;	         BL         --	   original index			     ;
;                ES         --     own data segment                          ;
;    Returns:  	 							     ;
;	         DH	    --     accelarator flags and new index	     ;
;                DL	    --	   Blue value (0,32,128 or 255)	             ;
;                AH         --     Green value (0,32,128 or 255)             ;
;                AL         --     Red value (0,32,128 or 255)               ;
;    History:								     ;
;		 Created.						     ;
;		 -by- Amit Chatterjee [amitc]  Mon 07-Nov-1988  13:49:30     ;
;----------------------------------------------------------------------------;

cProc	TranslateColor,<NEAR,PUBLIC>
	assumes	es,Data

cBegin	nogen

	and	bl,0fh		; last 3 bits have index
	shl	bx,1		; will be indexing into a word table
	mov	bx,[bx][PaletteTranslationTable]  ; get the translated index

; we assume that the index is in the LOBYTE and we ignore the high byte

	mov	dh,bl		; get the index into DH
	call	IndexToColor	; does all the necessary tablelook up

	ret

cEnd	nogen


;----------------------------------------------------------------------------;
;		         UpdateColor (lp_rect)			             ;
;			 ---------------------				     ;
;  									     ;
; This routine translates the pixel values in a rectangular region of the    ;
; EGA/VGA screen based on the current translate table maintained by the      ;
; driver.						                     ;
;									     ;
; ENTRY:								     ;
;         lp_rect	---     a long pointer to a RECT structure which has ;
;                               the coordinates of the rectangle to be trans-;
;                               lated.					     ;
; RETURNS:							             ;
;	  nothing.							     ;
;								             ;
; HISTORY:								     ;
;         Mon	Nov-21-1988	-by-  Amit Chatterjee [amitc] 08:26:14       ;
;         Created.					                     ;
;----------------------------------------------------------------------------;


cProc	UpdateColor,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di>

	parmD	lp_rect			; RECT specifying the update region
	
	localW	own_dseg		; save own data segment
	localW	NextScan		; offset to next scan line
	localB	FirstMask		; mask for 1st byte of row
	localB	LastMask		; mask for last byte of row
	localW	nRows			; no of rows
	localW	nBytes			; no of bytes

cBegin

	mov	own_dseg,ds		; save the driver data segment

;----------------------------------------------------------------------------;
; load the rectangle coordinates and exclude the cursor from that region     ;
;----------------------------------------------------------------------------;

ifdef	EXCLUSION

	lds	bx,lp_rect		; DS:BX points to the rectangle
	mov	cx,[bx].left	
	mov	dx,[bx].top
	mov	si,[bx].right
	mov	di,[bx].bottom		; load the rectangle coordinates
	call	exclude_far		; exclude the cursor

endif
      
;----------------------------------------------------------------------------;
; We shal program the EGA registers in READ MODE 1 and WRITE MODE 2, and use ;
; the color compare register. The method is as follows:			     ;
;								             ;
;            .  Each byte will be tested agains one pel value at a time by   ;
;               puting a pel value in the color-compare register and then    ;
;               using read mode 1.				             ;
;									     ;
;            .  Whenever a read in Mode 1 tells us that the pel value on the ;
;               screen has matched that in the color compare register, we    ;
;               write back the translated pel value which is from the CPU    ;
;									     ;
;            .  Actually, the result of the read in mode 1 will be used as a ;
;               bit mask value for the write in mode 2.			     ;
;									     ;
;	     .  The above operations will be repeated for each of the 16 pel ;
;               values that an EGA/VGA supports, we also maintain a running  ;
;               mask to keep track of the no of pels converted, when this    ;
;               goes 0 we can move on to the next byte.			     ;
;								             ;
;            .  Ega registers will be reprogrammed to be in their initial    ;
;               state after the translation has been done.		     ;
;----------------------------------------------------------------------------;

; set read mode 1 and write mode 2

	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_MODE
	mov	ah,00001010b
	out	dx,ax

; set the color don't care register to select all bits
	
	mov	dx,EGA_BASE + GRAF_ADDR
	mov	al,GRAF_CDC
	mov	ah,0fh
	out	dx,ax

; enable all planes for Set/Reset register.

	mov	dx,EGA_BASE + GRAF_ADDR	
	mov	al,GRAF_ENAB_SR		
	mov	ah,0ffh			; disable all planes for S/R
	out	dx,ax

;----------------------------------------------------------------------------;
; retrieve the RECT structure and do calculations to obtain the following    ;
; values:								     ;
;		   first byte to modify for the region (ES:DI)		     ;
;                  number of rows to modify.		     		     ;
;                  first byte mask		             		     ; 
;                  last byte mask                             		     ;
;                  number of bytes to touch - 1                              ;
;----------------------------------------------------------------------------;

	lds	si,lp_rect		; DS:SI points to the rect structure

; save the offset to the next scan line in a variable

	mov	bx,SCREEN_W_BYTES	; defines as an extern
	mov	NextScan,bx

; calculate the number of rows to deal with

	mov	bx,[si].bottom		
	sub	bx,[si].top		; get the vertical extent
	inc	bx			; bx has the number of rows to update
	mov	nRows,bx		; save it

; calculate the starting offset into DI and load ES

	mov	ax,ScreenSelector	; the screen segment
	mov	es,ax
	mov	ax,[si].top		; the starting row
	mul	NextScan
	mov	di,[si].left		; the starting X coordinate
	mov	cx,di			; save it in cx
	shiftr	di,3			; get the byte position
	add	di,ax			; ES:DI has starting byte position

; calculate the position for the staring bit

	and	cx,7			; get the no of bits in partial 1st byte
	mov	bl,80h			; bit positions start from the left
	ror	bl,1			; bl has the starting bit position

; get the no of pels to modify

	mov	cx,[si].right		; the right boundary
	sub	cx,[si].left		; the left boundary


; get the start and end masks and the no of bytes to touch
; cx has no of pels to move (1 less the no of pels to draw)

	
	mov	ax,cx			; save it
	shiftr	ax,3			; gives the no of whole bytes
	and	cx,7			

; cx has the no of bits in the last byte after moving ax  bytes from the
; starting bit position

	mov	bh,bl			; save the initial bit postion
	rol	bl,cl			; get the final bit position
	mov	cx,ax			; cx has no of bytes

; bh has the initial bit position, bl has the final -- convert them to masks

	mov	ax,bx			; get the bit positions
	xchg	ah,al			; al initial bit, ah final bit
	add	al,al
	dec	al			; al has the first byte mask
	mov	FirstMask,al		; save it
	neg	ah			; ah has the last byte mask
	mov	LastMask,ah		; save it

; test for byte wrap and increase no of bytes to touch

	cmp	bh,bl			; wrap past a byte ?
	adc	cx,0			; cx has no of bytes to touch - 1
	mov	nBytes,cx		; save it

;----------------------------------------------------------------------------;
; now do the actual translation using the method discussed above.	     ;
;----------------------------------------------------------------------------;

	mov	ds,own_dseg
	assumes	ds,Data

	mov	bx,DataOFFSET PaletteTranslationTable

	mov	cx,nRows		; the no of rows to convert
	mov	dx,EGA_BASE + GRAF_ADDR ; set it up for prog. GRX regs

pel_convert_loop:
	push	cx
	push	di			; save row start address
	mov	si,nBytes		; no of bytes to touch - 1
	mov	cl,FirstMask		; get the first mask
	xor	ch,ch			; will be set for last byte in row
	or	si,si			; is there just one partial byte ?
	jz	partial_byte

	xor	ah,ah			; start at pel value 0
byte_loop:

; get the translated value

	mov	al,ah			; get the pel into al
	shl	al,1			; index into words
	xlat				; get the translated byte
	cmp	al,ah			; identity translation
	jz	next_pel		; no translation for this pel
	push	ax			; save current pel value in ah
	push	bx			; save translate table address

; program the color compare and the set reset registers

	mov	bx,ax			; save translated value in bl
	mov	al,GRAF_COL_COMP	; the color compare register
	out	dx,ax			; gets actual pel value

	mov	al,GRAF_BIT_MASK
	out	dx,al			; set up address for bitmask
	inc	dx			; point to data register

; read the results of color compare
	
	mov	al,es:[di]		; 1 => color compares
	or	al,al			; test for no compare
	jz	no_translation

; set up the mask for the byte to the updated mask in cl
	
	mov	ah,al			; save the mask
	and	al,cl			; update with previous masks
	not	ah			; converted bits will not be converted
	and	cl,ah			; again, so update mask

; program the bit mask register

	out	dx,al			; the bitmask is set up

; write the translated value back

	mov	es:[di],bl		; translation done for onemore pel

no_translation:
	dec	dx			; go back to address register

; repeat the conversion for the current byte with other pels if mask is still
; not zero
	
	pop	bx			; get back translate table address
	pop	ax			; get back current pel value
next_pel:
	or	cl,cl			; is mask zeroised ?
	jz	move_to_next_byte	; mask bits are all zero
	inc	ah			; get the next pel
	cmp	ah,10h			; all pels converted
	jb	byte_loop		; no continue on same byte

move_to_next_byte:
	xor	ah,ah			; start at pel value 0
	inc	di			; get to next byte
	mov	cl,0ffh			; enable all mask bits
	dec	si			; one more byte completed
	jnz	byte_loop		; complete all the bytes

	inc	ch			; before last byte ch = 0ffh
	jz	row_completed		; we complete a row

; we have still to do the last byte.

partial_byte:
	and	cl,LastMask		; the lst byte mask
	mov	si,1			; just one byte to complete
	mov	ch,0ffh			; last byte indicator
	jmp	byte_loop		; convert one more byte

row_completed:

	pop	di			; get back the start address
	add	di,NextScan		; go to the next scan
	pop	cx			; get the no of rows left
	loop	pel_convert_loop	; convert the area for the pel value
	
;----------------------------------------------------------------------------;
; we have completed the translation, now reset the cursor and the EGA        ;
; programming.								     ;
;----------------------------------------------------------------------------;

blt_complete:

ifdef	EXCLUSION
	call	unexclude_far		; get back the cursor
endif

; disable all plane for Set/Reset

	mov	dx,EGA_BASE + GRAF_ADDR	; GRX select register
	mov	al,GRAF_ENAB_SR		
	xor	ah,ah			; disable all planes
	out	dx,ax

; enable all bits in bit mask register

	mov	ah,0ffh			; enable all bits
	mov	al,GRAF_BIT_MASK
	out	dx,ax

; set read mode zro and write mode 0

	mov	al,GRAF_MODE		; mode register
	mov	ah,M_DATA_READ + M_PROC_WRITE
	out	dx,ax

; set the map mask register in the sequencer to select all planes

	mov	dl,SEQ_ADDR
	mov	al,SEQ_MAP_MASK
	mov	ah,0fh			; enable all planes
	out	dx,ax

; return back from the routine

cEnd

;----------------------------------------------------------------------------;

sEnd	PaletteSeg

END

;----------------------------------------------------------------------------;





















