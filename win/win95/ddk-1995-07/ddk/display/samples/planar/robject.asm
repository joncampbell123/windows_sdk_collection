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

	page	,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	ROBJECT.ASM
;
; This module contains the code which realizes the objects required
; by GDI.
;
; Exported Functions:	RealizeObejct
;
; Public Functions:	sum_RGB_colors_alt
;
; Public Data:		none
;
; General Description:
;
;-----------------------------------------------------------------------;

	.286
MM_ALL		equ	0fh		;all bits of index on 
incLogical	= 1			;Include control for gdidefs.inc
incBrushStyle	= 1

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.inc
	include	mflags.inc
	.list


	??_out	robject
sBegin	Data
sEnd	Data


	
sBegin	Code
assumes cs,Code

	public	sum_RGB_colors_alt	;Alternate entry point
	public	sum_RGB_alt_far		;Alternate far entry point
	public	realize_solid
	public	realize_brush
	public	realize_pattern
	externNP dither 		;Brush dithering code
	externNP ColorDither
	externNP rgb_to_ipc		;Logical to physical color conv
	externNP RGB192Brush		;(?GAHIRES.ASM)
	externFP IndexToColor		;Gets h/w color triplet for an index
	externA  BW_THRESHOLD		;Where black becomes white
	externB PAccelTable			;in ?GA.ASM

        ifdef   EUCLIDEAN_COLOR_MATCH
        externNP SolidColorMatch
        endif   ;EUCLIDEAN_COLOR_MATCH

;	The following are the definitions for the hatch patterns.
;	They are defined by each individual driver base on its
;	resolution.


	externA < H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3>
	externA < H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7>
	externA < V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3>
	externA < V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7>
	externA <D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3>
	externA <D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7>
	externA <D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3>
	externA <D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7>
	externA <CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3>
	externA <CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7>
	externA <DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3>
	externA <DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7>
page
;--------------------------Exported-Routine-----------------------------;
; RealizeObject
;
;   RealizeObject - Logical to Physical Object Realization
;
;   RealizeObject performs the task of converting logical objects
;   into physical objects that this driver can manipulate to per-
;   form the various functions requested of it.
;
;   The size needed to realize an object will be returned if the
;   pointer to where the physical realization is to be stored is
;   NULL.
;
;   In some cases where the driver cannot realize the requested object,
;   a solid color pen must be realized which GDI will use when it
;   performs the nessacary simulations.  In other cases, punt.
;
; Entry:
;	None
; Returns:
;	AX = object size if ok
; Error Returns:
;	AX = 0	if error or object unrealizable
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,Data
assumes es,nothing


cProc	RealizeObject,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di>

	parmD	lp_device		;Pointer to Device structure
	parmW	style			;Style of realization
	parmD	lp_in_obj		;Pointer to input (logical) object
	parmD	lp_out_obj		;Pointer to output (physical) object
	parmD	lp_text_xform		;Pointer to a text transformation
					;  or (x,y) for brush realization
					;  Segment = y, Offset = x

	localB	brush_accel		;Temp brush accelerator
	localB	prev_dither		;Brush dither value
	localB	dither_flags		;for dithering
	localB	ExtraFlags		;for patterned and hatched brushes
	localW	BrushStyle
	localW	bw_sum			;Black & white sum for a brush
	localW	offset_to_phy_brush	;Offset to physical brush
	localW	IntensityPlane		;pattern for intensity plane

cBegin

	cld
	mov	ax,1			;Assume good exit
	mov	bx,style		;If delete object, nothing to do
	or	bx,bx			;  since we don't keep objects
	js	realize_exit		;This is a delete object
	lds	si,lp_in_obj		;--> logical object
	assumes ds,nothing

	dec	ax			;Assume error (AX = 0 as return code)
	dec	bx			;Determine style of realization.
	cmp	bx,OBJ_FONT-1		;Is it a legal object?
	jg	realize_exit		;  Not by our standards, return error
	shl	bx,1			;Compute index into dispatch table

	les	di,lp_out_obj		;If lp_out_obj is NULL, then return
	assumes es,nothing		;  size requirement?
	mov	cx,es
	or	cx,di
	jz	return_obj_size 	;They want the size

	push	bx			;Save object type
	call	cs:realize_dispatch[bx] ;Realize a physical object
	pop	bx			;Object's size will be return code
        jmp     realize_exit            ;and get out
        
return_obj_size:
	mov	ax,cs:realize_sizes[bx] ;Get size for the object

realize_exit:

cEnd
page
;--------------------------Private-Routine------------------------------;
; realize_pen
;
;   Realize Logical Pen Into a Physical Pen
;
;   The given logical pen is realized into a physical pen.
;
;   The pen will be realized regardless of the pen width or
;   style since GDI will need a pen to use for simulations.
;
;   If the width of the pen is >1, then a solid pen will be
;   realized regardless of the pen style.  GDI will use this
;   pen for simulating the wide line.
;
;   If the pen style isn't recognized, then a solid pen of
;   the given color will be realized (this is called punting).
;
; Entry:
;	DS:SI --> logical pen definition
;	ES:DI --> output object
; Returns:
;	AX non-zero to show success
; Error Returns:
;	No error return.
; Registers Preserved:
;	BP,DS,ES
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,FLAGS
; Calls:
;	sum_RGB_colors
;	convert_index
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_pen	proc near

	mov	cx,[si].lopnStyle	;If a NULL pen, then all that really
	cmp	cx,LS_NOLINE		;  needs to be stored is the pen
	je	realize_pen_20		;  type so it can be recognized
	mov	ax,LS_SOLID		;Assume solid pen will be required
	cmp	[si].lopnWidth,1	;If not a nominal width pen,
	ja	realize_pen_10		;  make a solid pen for simulations
	cmp	cx,MaxLineStyle 	;If the line style is unknown
	ja	realize_pen_10		;  make it a solid pen
	xchg	ax,cx			;Need to counter following XCHG

realize_pen_10:
	xchg	ax,cx			;Set pen type into CX
	lea	si,[si].lopnColor	;--> RGB color
;	call	convert_index		; convert the index into DH if the
	                                ; color is actually an index
;	jc	realize_pen_20		; it was an index


	call	sum_RGB_colors		;Sum up the color

realize_pen_20:
	stosw				;Save color of pen
	mov	ax,dx
	stosw
	mov	ax,cx			;Save style
	stosw
realize_just_a_return:
        or      al,1                    ;return success
	ret

	errnz	oem_pen_pcol		;Must be first field
	errnz	pcol_C0 		;Colors must be in this order
	errnz	pcol_C1-pcol_C0-1
	errnz	pcol_C2-pcol_C1-1
	errnz	pcol_C3-pcol_C2-1
	errnz	oem_pen_style-4 	;Style must be 4 bytes into phys pen

realize_pen	endp
page
;--------------------------Private-Routine------------------------------;
; realize_brush
;
;   Realize Logical Brush Into a Physical Brush
;
;   Four styles of logical brushes may be realized.  These are SOLID,
;   HOLLOW, HATCHED, and PATTERN.
;
;   A SOLID brush is defined with a logical RGB color definition.
;   This color is processed into one of 65 dithers.
;
;   A HATCHED brush is defined with a logical RGB color definition and
;   a hatching type.  The hatch type is mapped to one of the six hatched
;   styles that the driver supports.  All bits in the hatched brush which
;   are 1 are set to the hatch color passed in, and all bits which are
;   0 are set to the background color passed in.
;
;   A PATTERN brush is defined with an 8 X 8 pattern in the form of a
;   physical bitmap.  The bitmap may be monochromw or color.  More
;   discussion on converting is contained under the pattern brush code.
;
;   A HOLLOW brush is one which can never be seen.  The brush style is
;   maintained in the device brush structure so that a check can be
;   made and an abort executed if one is used.	No punting is needed
;   for hollow brushes.
;
;   Brushes will be aligned based at the (x,y) origin passed in via
;   the text transform.
;
; Entry:
;	DS:SI --> logical pen definition
;	ES:DI --> output object
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_brush	proc near

	xor	ax,ax			;Assume a solid brush will be needed
	errnz	BS_SOLID		;Must be 0, and first type of brush
	mov	bx,[si].lbStyle 	;Get the brush style.
	cmp	bx,MaxBrushStyle	;If an illegal brush, create a solid one
	ja	realize_brush_10	
	xchg	ax,bx			;Need brush style in AX

realize_brush_10:
	add	ax,ax			;Call routine which will do the
	xchg	ax,bx			;  actual realization
	call	cs:brush_realizations[bx]
	lds	di,lp_out_obj		;--> back to start of oem_brush_def
	assumes ds,nothing

	mov	[di].oem_brush_style,bx ;Save brush accelerator
	mov	[di].oem_brush_accel,dl ;Save brush accelerator


;	If this is a solid black or a solid white brush, then no
;	rotation is required.  Otherwise, rotate as needed.
;
;	Some acceleration can be gained by recognizing that the
;	color planes of the brush as solid and not rotating them

	and	dl,SOLID_BRUSH+GREY_SCALE+ONES_OR_ZEROS
	cmp	dl,SOLID_BRUSH+GREY_SCALE+ONES_OR_ZEROS
	je	realize_brush_30	;No rotations will be required

	mov	cx,off_lp_text_xform	;Get the X origin
	and	cx,111b 		;If at a byte boundary then
	jz	realize_brush_20	;  no X rotation is needed
	call	rotate_brush_x		;Perform X rotation

realize_brush_20:
	mov	bx,seg_lp_text_xform	;Get the Y origin
	and	bx,111b 		;If at a multiple of 8, then
	jz	realize_brush_30	;  no Y rotation is needed
	call	rotate_brush_y		;Perform Y rotation

realize_brush_30:
        mov     ax,8000h                ;Setup return (success, not solid)
        test    dl,SOLID_BRUSH          ;See if brush is solid
        jz      realize_brush_exit      ;If it isn't we're done
        or      al,GDI_SOLID_COLOR_BRUSH;Otherwise set the solid flag
realize_brush_exit:
	ret

realize_brush	endp

page
;--------------------------Public-Routine-------------------------------;
; sum_RGB_colors
; sum_RGB_colors_alt
;
;   Sum Given RGB Color Triplet
;
;   The given RGB color triplet is summed, and the result returned
;   to the caller.  Other useful information is also returned.
;
;   It is this routine which maps the colors to the bit planes
;   of the EGA.
;
;   Ordering of the color in a dword is such that when stored in
;   memory, red is the first byte, green is the second, and blue
;   is the third.  The high order 8 bits may be garbage when passed
;   in, and should be ignored.
;
;   when in a register:     xxxxxxxxBBBBBBBBGGGGGGGGRRRRRRRR
;
;   when in memory:	    db	    red,green,blue
;
;
; Entry:
;	DS:SI --> RGB triplet to sum	    (for sum_RGB_colors)
;	AL     =  Red	value of triplet    (for sum_RGB_colors_alt)
;	AH     =  Green value of triplet    (for sum_RGB_colors_alt)
;	DL     =  Blue	value of triplet    (for sum_RGB_colors_alt)
; Returns:
;	BX		= Sum of the triplet
;
; The EGA/VGA supports 2 bits for every color and so the physical color can 
; only be one of 4 values. The mapping chosen by experimentation happens to
; be:-
;		Physical Index            Color Vale(0-255)
;		--------------		  -----------------
;		     00				 0
;		     01				 128
;		     11				 255
; The intenstity bit,ie, the MS bit of the 4 bit index is common to all the
; 3 planes and an index 10 for all will be used to siginify a dark grey with
; color value being 32 for all 3 colors.
;
;	AL		= Physical red color byte (0,32,128,255)
;	AH		= Physical green color byte (0,32,128,255)
;	DL		= Physical blue color byte (0,32,128,255)
;	DH:C0		= red	bit
;	DH:C1		= green bit
;	DH:C2		= blue	bit
;	DH:C3		= intensity bit
;	DH:MONO_BIT	= 0 if BX < BWThreashold
;			= 1 if BX >= BWThreashold
;	DH:ONES_OR_ZERO = 1 if C0:C3 are all 1's or all 0's
;	DH:GREY_SCALE	= 0
;	DH_SOLID_BRUSH	= 0
; Error Returns:
;	None
; Registers Preserved:
;	CX,SI,DI,DS,ES
; Registers Destroyed:
;	CX,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing

sum_RGB_alt_far	proc far
	call 	sum_RGB_colors_alt
	ret
sum_RGB_alt_far	   endp

sum_RGB_colors	proc	near

	mov	ax,word ptr [si]	;AH = G, AL = R
	mov	dl,byte ptr [si][2]	;DL = B

sum_RGB_colors_alt proc near
        ifdef   EUCLIDEAN_COLOR_MATCH
        cCall   SolidColorMatch
        else
        push    ds
        cCall   rgb_to_ipc              ;sets DS to DGROUP
        pop     ds
        endif   ;EUCLIDEAN_COLOR_MATCH
sum_RGB_colors_exit:
	ret

sum_RGB_colors_alt endp
sum_RGB_colors	   endp
page

greys	label	word
	db	088h,022h		;dark grey
	db	0AAh,055h		;will never hit this
	db	0ddh,077h		;brighter grey

;--------------------------Private-Routine------------------------------;
; realize_solid
;
;   Realize Solid Style Brush
;
;   The given logical solid brush is realized.	Each color for
;   the brush (RGB) is dithered for the color bruhses.
;
;   The sum of all the colors is used to determine the dither
;   to use for monochrome portion of the brush.
;
; Entry:
;	DS:SI --> logical object    
;	ES:DI --> output object
; Returns:
;	DL = oem_brush_accel flags
; Error Returns:
;	None
; Registers Preserved:
;	BP,ES,DS,FLAGS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,FLAGS
; Calls:
;	dither
;	sum_RGB_colors_alt
;	convert_index
;-----------------------------------------------------------------------;


;----------------------------------------------------------------------------;

realize_solid	proc near
	
	mov	ax,wptr [si].lbColor	;AL = Red, AH = Green
	mov	dx,wptr [si].lbColor[2] ;DL = Blue, DH the 4th byte

	mov	bx, ax
	and	bl, al			;quick check for white brush
	and	bl, dl
	and	bl, bh
	inc	bl
if	MASMFLAGS and VGA
	jz	RSWhiteBrush_10 	;in case of VGA, jmp range too short
else
	jz	RSWhiteBrush
endif

	mov	bx, ax
	or	bl, dl			;quick check for black brush
	or	bl, ah
	jz	RSBlackBrush

ifdef	_NEW_COLOR_ADD

	sub	dh, dh			;DX: blue
	mov	bl, ah
	sub	ah, ah			;AX: red
	sub	bh, bh			;BX: green
	shl	ax, 1
	add	dx, ax			;DX: 2*R+1*B
	add	dx, bx
	shiftl	bx, 2
	add	dx, bx			;DX: 2*R+5*G+1*B
	add	dx, 4			;for proper rounding for /8
	shiftr	dx, 3			;divide by 8
else

;----------------------------------------------------------------------------;
; Do a weighted average of the color for a monochrome driver.		     ;
; the sum is (R+B)/4 + G/2						     ;
;----------------------------------------------------------------------------;
	add	dl,al			;R+B
	rcr	dl,1			;(R+B)/2
	add	dl,ah			;pitch in Green
	rcr	dl,1			;G/2 + (R+B)/4
	xor	dh,dh
endif
	mov	bw_sum,dx		;save it.  Restore RGB now
	mov	ax,wptr [si].lbColor	;AL = Red, AH = Green
	mov	dx,wptr [si].lbColor[2] ;DL = Blue, DH the 4th byte

if 	MASMFLAGS and VGA
	test	dh, 10h 		;is this the "special" grey for the
	jz	realize_solid_10	;tastlessly chosen default scrollbar
	mov	bl, 224 		;color? (224, 224, 224)
	cmp	al, bl
	jne	realize_solid_10
	cmp	ah, bl
	jne	realize_solid_10
	cmp	dl, bl
	jne	realize_solid_10
	mov	ax, 0aa55h		;make it a 50% dither of light grey
	mov	cx, 12			;(index=8) and white (index=f).
rep	stosw				;save R, G, B color planes
	mov	ax, -1
	mov	cl, 4
rep	stosw				;make I plane all ones.
	mov	brush_accel, 3fh	;claim the closest solid color is white
	jmp	short do_mono_brush	;go do the monochrome portion of brush

RSWhiteBrush_10:
	jmp	short RSWhiteBrush

realize_solid_10:
endif
;----------------------------------------------------------------------------;
; now call the color matcher routine to get the nearest color index and the  ;
; accelarator bits in DH, ignore the physical color values returned in AL,AH ;
; and DL.								     ;
;----------------------------------------------------------------------------;

	push	ax			
	push	dx			;save logical colors
	cCall	rgb_to_ipc		;map into index; makes DS=Data on ret.
	and	dh,not SOLID_BRUSH	;we will decide about this later
	mov	brush_accel,dh		;save the tentative accelarators
	pop	cx			;get back logical blue
	cmp	cl, dl			;is it the same as for nearest color?
	pop	bx			;get logical red and green
	jne	RSDitherBrush		;no. Dither that brush
	cmp	bx, ax			;is log. RG same as in nearest color?
	jne	RSDitherBrush		;no.  Dither that brush.

; Fill the brush with the solid color.
	or	brush_accel, SOLID_BRUSH
	mov	bx, 4			;We do 4 planes.
RSSetPlanesLoop:
	mov	cx, 4			;4 words per plane.
	shr	dh, 1			;Is this plane non-zero?
	sbb	ax, ax			;If yes, ax == FFFF else 0000.
rep	stosw				;Set (or clear) the plane.
	dec	bx			;Advance to next plane.
	jnz	RSSetPlanesLoop
	jmp	short do_mono_brush

RSBlackBrush:
	mov	bh, SOLID_BRUSH+GREY_SCALE+ONES_OR_ZEROS
	.errnz	SOLID_BRUSH+GREY_SCALE+ONES_OR_ZEROS-0e0h
RSWhiteBrush:
	mov	brush_accel, bh 	;save accelerator bits
	mov	cx, 20			;Fill the 4 color planes and the 1
rep	stosw				;monochrome plane with either all
	jmp	short set_up_return	;zeros or all ones.

RSDitherBrush:
	cCall	ColorDither

do_mono_brush:

; do the monochrome part of the brush

	xor	si,si			;clear GREY indicators
	mov	dx,bw_sum		;get color value for mono plane
	call	dither			;do the pattern
	or	si,si			;test for solid or bad dithers
	jz	set_up_return		;neither solid nor bad dither
	test	si,100b			;ignore solid color
	jnz	set_up_return

; we have a bad dither for the mono plane set it correct

	add	si,si			;index into DWORD
	and	si,7
	mov	ax,cs:greys[si][-2]	;get the better looking dither pattern
	sub	di,SIZE_PATTERN		;go to start of mono brush
	stosw
	stosw
	stosw
	stosw
	errnz	SIZE_PATTERN-8

set_up_return:
	mov	bx,BS_SOLID		;brush type solid
	mov	dl,brush_accel		;the accelarator bute
solid_brush_30:
	ret

realize_solid	endp



;----------------------------------------------------------------------------;
;			realize_solid_with_index			     ;
;                       ------------------------			     ;
; ES:DI --- points to the start of the physical brush structure		     ;
; AX    --- has an index value (last 4 bits significant)		     ;
;       								     ;
; This routine creates a 4 plane color brush pattern where every pel has the ;
; passed in index value. The monochrome part of the brush is going to ba all ;
; black if the index is 0 or else it is going to be white (for anyother index;
; value). The brush accelarator will be set, with the mono bit being 0 if    ;
; index is 0 else it will be 1.				                     ;
;								             ;
; This routine retuens BS_SOLID in BX and the accelarator flag in DL         ;
;----------------------------------------------------------------------------;

txrealize_solid_with_index proc	near

	mov	bx,ax		; working copy of the index
	mov	dl,al		; a copy of the index
	mov	cx,4		; there are 4 planes
index_for_a_plane:
	push	cx		; has the no of planes yet to cover
	mov	cx,4		; there are 4 words per plane
	shr	bx,1		; get the bit for the current plane into carry
	sbb	ax,ax		; set ax to 0 or ff depending on the bit
	rep	stosw		; fill up the bytes for a plane
	pop	cx		; get back the number of planes
	loop	index_for_a_plane

; now we will complete the mono chrome part of the map
	
	cmp	dl,1		; if zero set all bits to 0
	cmc			; carry = 1, iff dh != 0
	sbb	ax,ax		; set ax to 0 or 0ffh accordingly
	mov	cx,4		; 4 words for the mono part
	rep	stosw		; complete that part

; now the accelarator byte has to be created, low 3 bits already have the 
; index.

	and	dl,MM_ALL	; last 4 bits have index
	or	dl,ONES_OR_ZEROS; assume it is all zero
	cmp	dl,ONES_OR_ZEROS; was it so ?
	jz	set_solid_bit	; yes
	and 	dl,not ONES_OR_ZEROS ; reset that bit
	cmp	dl,07h		; all ones 
	jnz	set_solid_bit	; only solid brush bit to set on

; all the bits in index are one, so set on MONO_BIT and ONES_OR_ZEROS

	or	dl,MONO_BIT+ONES_OR_ZEROS

set_solid_bit:

; set a bit indicating a solid brush

	or	dl,SOLID_BRUSH
	mov	bx,BS_SOLID	; solid brush code

	ret
txrealize_solid_with_index endp
;----------------------------------------------------------------------------;

page
;--------------------------Private-Routine------------------------------;
; realize_pattern
;
;   Realize Pattern brush
;
;   The given bitmap is copied for use as a 8x8 bit pattern brush.
;   Any information beyond the first 8x8 bits is not required to be
;   maintained as part of the brush, even though GDI does allow
;   you to do this if desire.
;
;   If the bitmap is a monochrome bitmap, it is to be expanded up
;   into a black/white bitmap.	If the bitmap is a color bitmap,
;   then to compute the monochrome portion of the brush, the planes
;   will be ANDed together (this favors a white background).
;
;   This code doesn't correctly handle huge bitmaps as the source
;   for a pattern.
;
; Entry:
;	DS:SI --> logical object
;	ES:DI --> output object
; Returns:
;	DL = oem_brush_accel flags
; Error Returns:
;	None
; Registers Preserved:
;	BP,ES,FLAGS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_pattern proc near

	push	di			;save offset to PBrush
	mov	BrushStyle, BS_PATTERN	;assume colored pattered brush
	mov	brush_accel,0		;No accelerators set for the brush
	lds	si,[si].lbPattern	;Get pointer to the bitmap
	assumes ds,nothing

	mov	dx,[si].bmWidthBytes	;Get physical width of bitmap
	mov	bx,dx			;offset to next plane also

; DX will have the offset to the next scan line for the source. If it is
; a mono map so offset to next plane is also bmWidthBytes, for color maps
; it will be NUMBER_PLANES * DX, update DX to this value and save the update
; for mono maps in CX. Also the value calculated will be decremented by 1
; since MOVSB add 1 to SI

	mov	cx,dx			; save for mono
	add	dx,dx			; double dx
	add	dx,dx			; 4 times
	.errnz	NUMBER_PLANES - 4
	dec	dx			; to take care of auto inc in MOVSB
	dec	cx			; to take care of auto inc in MOVSB

	mov	ah,NUMBER_PLANES	;Setup color planes loop count
	cmp	[si].bmPlanes,ah	;Set 'Z' if color
	lds	si,[si].bmBits		;--> where the bits are
	assumes ds,nothing

	je	realize_pattern_10	;Handling color source
	xor	bx,bx			;Mono source, zero plane increment
	mov	dx,cx			; cx had offset to next mono scan
	inc	BrushStyle
	.errnz	BS_MONO_PATTERN-BS_PATTERN-1

realize_pattern_10:
	push	si			;Save start of plane
	mov	cx,SIZE_PATTERN 	;Set # bytes to move

realize_pattern_20:
	movsb				;Move one byte of pattern
	add	si,dx			;Skip rest of scanline
	loop	realize_pattern_20	;Until one plane has been moved
	pop	si			;Get back start of plane
	add	si,bx			;--> start of next plane

	sub	ah,1			;Done all the planes yet?
	ja	realize_pattern_10	;  No
	jb	realize_pattern_40	;  Just handled monochrome source


;	Handle the monochrome plane of the brush.  If the source is a
;	monochrome bitmap, then just copy it as is.  If the source is
;	a color bitmap, then pack the bits in the color planes into indices
;	and fetch their mono bit.  Those mono bits are accumulated to fill
;	a byte and are then saved in the monochrome portion of the PBrush.

	or	bx,bx			;Monochrome?
	jz	realize_pattern_10	;  Yes, just copy mono data again
	mov	cl,SIZE_PATTERN
	errnz	<SIZE_PATTERN AND 0FF00H>


;	Use the bits just copied into the brush area since we know
;	where they are and what the offset between planes is.

	lea	bx, PAccelTable
	mov	si, 8			;SI: outer loop counter

realize_pattern_30:
	push	si			;save outer loop counter
	mov	si, 8			;now SI is inner loop counter
	mov	cl, es:[di][-(1*SIZE_PATTERN)]	;Get C3
	mov	ch, es:[di][-(2*SIZE_PATTERN)]	; C2
	mov	dl, es:[di][-(3*SIZE_PATTERN)]	; C1
	mov	dh, es:[di][-(4*SIZE_PATTERN)]	; C0
realize_pattern_35:
	sub	al, al
	shl	cl, 1
	rcl	al, 1
	shl	ch, 1
	rcl	al, 1
	shl	dl, 1
	rcl	al, 1
	shl	dh, 1
	rcl	al, 1
	xlatb	cs:[bx]
	shiftl	al, 4			;mono bit into carry
	rcl	ah, 1			;accumulate bits in AH
	dec	si
	jnz	realize_pattern_35
	pop	si			;restore outer loop counter
	mov	al, ah			;put newly generated byte in AL
	stosb				;save it, increment DI
	dec	si			;update outer loop counter
	jnz	realize_pattern_30

realize_pattern_40:
	pop	di				;restore offset to start of PBr
	mov	bx,BrushStyle			;store the actual brush style
	mov	es:[di].oem_brush_fg, 0ffh	;no physical color index is
	mov	es:[di].oem_brush_bg, 0ffh	;ever equal 0ffh
	mov	dl,brush_accel			;No brush accelerators
	ret

realize_pattern endp
page
;--------------------------Private-Routine------------------------------;
; realize_hatch
;
;   Realize a Hatched Brush
;
;   The requested hatched brush is realized.  Two colors are invloved
;   for hatched brushes, the background color, and the hatch color.
;   If these two colors are the same, then the brush will be a
;   solid brush, and the solid brush code will be invoked
;
;   If not, then all 0 bits in the hatch pattern will be set to the
;   background color, and all 1 bits will be set to the foreground
;   color.  Note that hatched colors are solid colors; no dithering
;   takes place for their realization.
;
; Entry:
;	DS:SI --> logical object
;	ES:DI --> output object
; Returns:
;	DL = oem_brush_accel flags
; Error Returns:
;	None
; Registers Preserved:
;	BP,ES,DS,FLAGS
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,FLAGS
; Calls:
;	move_hatch
;       convert_index
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_hatch	proc near

	mov	cx,[si].lbHatch 	;Get hatch style
	cmp	cx,MaxHatchStyle	;Hatch style within range?
	jbe	realize_hatch_10	;  Yes
	jmp	realize_solid		;  No, create a solid brush

realize_hatch_10:
	lea	si,[si].lbBkColor	;Compute background color
;	call	convert_index		; convert index if there is one
;	jc	BkColor_index		; the color was an index
	call	sum_RGB_colors
BkColor_index:
	push	dx			;Save background rgb + mono color info

	sub	si,lbBkColor-lbColor	;Compute foreground color
;	call	convert_index		; convert index if there is one
;	jc	FgColor_index		; the color was an index
	call	sum_RGB_colors
FgColor_index:

	pop	ax			;Get background rgb + mono color info
	mov	dl,ah			;dh = forground, dl = background info

	mov	bx,cx			;Compute address of hatch pattern
	shl	bx,1
	shl	bx,1
	shl	bx,1
	lea	bx,hatched_brushes[bx]	;bx --> hatch pattern to use

	errnz	SIZE_PATTERN-8		;Pattern must be this size
	push	di
	call	move_hatch		;Move C0
	call	move_hatch		;Move C1
	call	move_hatch		;Move C2
	call	move_hatch		;Move C3
;	ror	dh,1			;Skip over C3 reserved bit
;	ror	dl,1
	call	move_hatch		;Move hatch brush
	pop	di
	add	di, oem_brush_trans
	mov	dx, 1			;fg: 0, bg: 1  get transparency mask
	call	move_hatch
	mov	bx,BS_HATCHED		;Set brush style
	xor	dl,dl			;No brush accelerator
	ret

	errnz	  C0_BIT-00000001b
	errnz	  C1_BIT-00000010b
	errnz	  C2_BIT-00000100b
	errnz	  C3_BIT-00001000b
	errnz	MONO_BIT-00010000b
	errnz	oem_brush_C0
	errnz	oem_brush_C1-oem_brush_C0-SIZE_PATTERN
	errnz	oem_brush_C2-oem_brush_C1-SIZE_PATTERN
	errnz	oem_brush_C3-oem_brush_C2-SIZE_PATTERN
	errnz	oem_brush_mono-oem_brush_C3-SIZE_PATTERN

realize_hatch	endp

;----------------------------------------------------------------------------;
; 			convert_index					     ;
; 	                -------------					     ;
; DS:SI points to a logical color structure. If the HIWORD is 0ffffh, then   ;
; the LOWORD has a color palette index (actually in low 3 bits,ignore rest)  ;
; However, if the HIWORD is not 0ffffh, then the LOW 3 bytes hold the        ;
; logical R,G and B values.						     ;
;									     ;
; If the color is indeed an index, this routine returns the index in DH with ;
; the accelarator bits set on.It also sets the carry in this case, else it   ;
; resets the carry.							     ;
;									     ;
; We also return the hardware color corresponding to the index in AL (red),  ;
; AH (green) and (DL) blue.						     ;
;----------------------------------------------------------------------------;

convert_index	proc	near

	mov	ax,[si][2]		; get the high word
	cmp	ax,0ffffh		; is it an index
	jnz	convert_index_ret	; no, nothing else to do.
	mov	dh,[si]			; index in lower 3 bits
	and 	dh,MM_ALL		; get only the index bits

; IndexToColor gets the hardware color corresponding to the index and
; also the accelarator bits. 


	call	IndexToColor	

; we can resturn after setting the carry on.

	stc				; set the carry to indicate it was 
	ret				; an index conversion
convert_index_ret:
	clc
	ret

convert_index	endp

;----------------------------------------------------------------------------;


page
;--------------------------Private-Routine------------------------------;
; move_hatch
;
;   Move Hatch Brush
;
;   The bits for one plane of a hatched brush are moved according
;   to the foreground and background colors passed in.	If the
;   colors are the same, then that color will be moved into the
;   brush.  If the colors are different, then either the pattern
;   or the inverse will be moved into the brush.   The pattern
;   will be moved in if the foreground color is 0, and the inverse
;   will be moved in if the foreground color is 1 (the brushes
;   were defined for a foreground color of 0 and a background
;   color of 1)
;
; Entry:
;	DH:D0  =  foreground color info (0/1)
;	DL:D0  =  background color info (0/1)
;	CS:BX --> hatch pattern to use
;	ES:DI --> next plane of destination brush
; Returns:
;	DH:D0  =  next foreground color info (0/1)
;	DL:D0  =  next background color info (0/1)
;	CS:BX --> hatch pattern to use
;	ES:DI --> next plane of destination brush
; Error Returns:
;	None
; Registers Preserved:
;	BP,DI,ES,DS,FLAGS
; Registers Destroyed:
;	AX,CX,SI,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


move_hatch	proc near

	mov	cx,SIZE_PATTERN/2	;Set up move for a single plane
	errnz	<SIZE_PATTERN and 1>	;Must be an even number of bytes

	ror	dh,1			;Place next foreground color into D7
	ror	dl,1			;Place next background color into D7
	sbb	si,si			;SI = FFFF if bit was set
	mov	al,dh
	xor	al,dl			;Set 'S' if different colors
	js	move_hatch_10		;Colors are different
	xchg	ax,si			;The colors are the same, so just
	rep	stosw			;  create a solid plane of the color
	ret

move_hatch_10:
	xchg	si,bx			;Need hatch pattern pointer in SI

move_hatch_20:
	lods	wptr cs:[si]		;Get next word of pattern
	xor	ax,bx			;Invert it if needed
	stosw				;Stuff it away
	loop	move_hatch_20
	lea	bx,[si].-SIZE_PATTERN	;Restore hatch pointer
	ret

move_hatch	endp
page
;--------------------------Private-Routine------------------------------;
; realize_hollow
;
;   Realize Hollow Brush
;
;   This is sleazy.  Hollow brushes are implemented by checking
;   the style flag, and ignoring the contents of the oem_brush_def
;   structure
;
; Entry:
;	DS:SI --> logical object
;	ES:DI --> temp brush area
; Returns:
;	DL = oem_brush_accel flags
; Error Returns:
;	None
; Registers Preserved:
;	BP,DI,ES,DS,FLAGS
; Registers Destroyed:
;	BX
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_hollow	proc near

	mov	bx,BS_HOLLOW
	mov	dl,SOLID_BRUSH+GREY_SCALE+ONES_OR_ZEROS

	ret

realize_hollow	endp
page
;--------------------------Private-Routine------------------------------;
; rotate_brush_x
;
;   Rotate Brush in X
;
;   The brush is rotated right by the given amount.  This is done
;   so that areas of the screen can be moved, and by realizing a
;   new brush with the correct (x,y), a pattern continued correctly
;   with a previous pattern.
;
; Entry:
;	DS:DI --> oem_brush_def
;	CL     =  alignment count (#ROR's required)
;	DL = oem_brush_accel SOLID_BRUSH flag bit
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	CH,DH,DI,DS,BP
; Registers Destroyed:
;	AL,SI,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


	errnz	SIZE_PATTERN-8
	errnz	oem_brush_C0
	errnz	oem_brush_C1-oem_brush_C0-SIZE_PATTERN
	errnz	oem_brush_C2-oem_brush_C1-SIZE_PATTERN
	errnz	oem_brush_C3-oem_brush_C2-SIZE_PATTERN
	errnz	oem_brush_mono-oem_brush_C3-SIZE_PATTERN
	errnz	oem_brush_style-oem_brush_mono-SIZE_PATTERN


rotate_brush_x	proc near

	mov	si,di			;DS:SI --> where to start rotating

; there are 5 planes 4 color planes and 1 mono plane

	mov	al,SIZE_PATTERN*5	;Assume all bytes must be rotated
	or	dl,dl			;Solid brush in the color planes?
	jns	rotate_brush_x_10	;  No, must rotate all bytes of brush
	errnz	SOLID_BRUSH-10000000b
	mov	al,SIZE_PATTERN 	;Just rotate mono plane
	lea	si,[si].oem_brush_mono

rotate_brush_x_10:
	ror	bptr [si],cl		;Rotate this byte of the brush
	inc	si
	dec	al			;Update loop count
	jnz	rotate_brush_x_10

	cmp	[di].oem_brush_style, BS_HATCHED
	jne	rotate_brush_x_30
	mov	al, SIZE_PATTERN	    ;in case of a hatched brush rotate
	lea	si, [di].oem_brush_trans    ;the transparency mask as well
rotate_brush_x_20:
	ror	bptr [si], cl
	inc	si
	dec	al
	jnz	rotate_brush_x_20
rotate_brush_x_30:
	ret				;All have been rotated

rotate_brush_x	endp
page
;--------------------------Private-Routine------------------------------;
; rotate_brush_y
;
;   Rotate Brush in Y
;
;   The brush is rotated in Y by the given amount.  This is done
;   so that areas of the screen can be moved, and by realizing a
;   new brush with the correct (x,y), a pattern continued correctly
;   with a previous pattern.
;
; Entry:
;	DS:DI --> oem_brush_def
;	BX = Y adjustment (ANDed with 111b)
;	CH = 0
;	DL = oem_brush_accel SOLID_BRUSH flag bit
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	DH,DI,DS,BP
; Registers Destroyed:
;	AX,SI,FLAGS
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


	errnz	SIZE_PATTERN-8
	errnz	oem_brush_C0
	errnz	oem_brush_C1-oem_brush_C0-SIZE_PATTERN
	errnz	oem_brush_C2-oem_brush_C1-SIZE_PATTERN
	errnz	oem_brush_C2-oem_brush_C1-SIZE_PATTERN
	errnz	oem_brush_C3-oem_brush_C2-SIZE_PATTERN
	errnz	oem_brush_mono-oem_brush_C3-SIZE_PATTERN
	errnz	oem_brush_style-oem_brush_mono-SIZE_PATTERN


rotate_brush_y	proc near

	or	dl,dl			;Set 'S' if Solid brush
	mov	si, di

; there are 5 planes now. 4 color planes and one mono plane.

	mov	dx,0507h		;DH = # planes, DL = wrap mask
	jns	rotate_brush_y_10	;Non-solid, rotate all bytes of brush
	errnz	SOLID_BRUSH-10000000b
	mov	dh,1			;Just rotate mono plane
	lea	di,[di].oem_brush_mono

rotate_brush_y_10:
	push	[di][6] 		;Save the bytes of this plane
	push	[di][4]
	push	[di][2]
	push	[di][0]
	mov	cl,SIZE_PATTERN/2

rotate_brush_y_20:
	pop	ax
	mov	[di][bx],al
	inc	bx
	and	bl,dl
	mov	[di][bx],ah
	inc	bx
	and	bl,dl
	loop	rotate_brush_y_20

	add	di,SIZE_PATTERN 	;--> next plane
	dec	dh			;Any more planes?
	js	rotate_brush_y_30	;after we rotated transparency mask
	jnz	rotate_brush_y_10	;  Yes
	add	di, oem_brush_trans-oem_brush_style
	cmp	[si].oem_brush_style, BS_HATCHED
	je	rotate_brush_y_10	;hatched brush?
rotate_brush_y_30:
	ret

rotate_brush_y	endp

page
;--------------------------Private-Routine------------------------------;
; realize_font
;
;   Realize a Font
;
;   Realize font is the routine that realizes a logical font into a
;   physical font.  It is possible to punt on this routine and have
;   GDI's font manager realize the desired font, or something close
;   to it, by returning the error code (ax = 0).  That's what I'm
;   going to do.
;
; Entry:
;	AX = 0
; Returns:
;	AX = 0
; Error Returns:
;	AX = 0
; Registers Preserved:
;	BP
; Registers Destroyed:
;	None
; Calls:
;	None
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


assumes ds,nothing
assumes es,nothing


realize_font equ realize_just_a_return
page
;	brush_realizations
;
;	brush_realizations contains the address of the function
;	that performs the realization of the desired type of brush.

brush_realizations label word

	dw	realize_solid
	dw	realize_hollow
	dw	realize_hatch
	dw	realize_pattern

	errnz	BS_SOLID
	errnz	BS_HOLLOW-BS_SOLID-1
	errnz	BS_HATCHED-BS_HOLLOW-1
	errnz	BS_PATTERN-BS_HATCHED-1





;	Predefined Hatched Brushes
;
;	The following brushes are the predefined hatched brushes that
;	this driver knows about.


hatched_brushes label	byte

	db	 H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3
	db	 H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7
	db	 V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3
	db	 V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7
	db	D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3
	db	D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7
	db	D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3
	db	D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7
	db	CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3
	db	CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7
	db	DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3
	db	DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7

	errnz	HS_HORIZONTAL
	errnz	HS_VERTICAL-HS_HORIZONTAL-1
	errnz	HS_FDIAGONAL-HS_VERTICAL-1
	errnz	HS_BDIAGONAL-HS_FDIAGONAL-1
	errnz	HS_CROSS-HS_BDIAGONAL-1
	errnz	HS_DIAGCROSS-HS_CROSS-1
	errnz	MaxHatchStyle-HS_DIAGCROSS






;	realize_dispatch
;
;	realize_dispatch contains the address of the procedure which
;	is invoked to realize a given object.

realize_dispatch label	word

	dw	realize_pen
	dw	realize_brush
	dw	realize_font

	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1




;	realize_sizes
;
;	realize_sizes contains the size of the memory required to realize
;	each of the objects.  These sizes will be returned to GDI when it
;	inquires the memory needed far an object.


realize_sizes	label	word

	dw	SIZE oem_pen_def
	dw	SIZE oem_brush_def
	dw	0

	errnz	OBJ_PEN-1
	errnz	OBJ_BRUSH-OBJ_PEN-1
	errnz	OBJ_FONT-OBJ_BRUSH-1

sEnd	Code
if	MASMFLAGS and PUBDEFS
	public	return_obj_size
	public	realize_exit
	public	realize_pen
	public	realize_pen_10
	public	realize_pen_20
	public	realize_brush
	public	realize_brush_10
	public	realize_brush_20
	public	realize_brush_30
	public	sum_RGB_colors
	public	sum_RGB_colors_exit
	public	realize_solid
	public	solid_brush_30
	public	greys
	public	realize_pattern
	public	realize_pattern_10
	public	realize_pattern_20
	public	realize_pattern_30
	public	realize_pattern_40
	public	realize_hatch
	public	realize_hatch_10
	public	move_hatch
	public	move_hatch_10
	public	move_hatch_20
	public	realize_hollow
	public	realize_just_a_return
	public	rotate_brush_x
	public	rotate_brush_x_10
	public	rotate_brush_y
	public	rotate_brush_y_10
	public	rotate_brush_y_20
	public	realize_font
	public	brush_realizations
	public	hatched_brushes
	public	realize_dispatch
	public	realize_sizes
endif
end

