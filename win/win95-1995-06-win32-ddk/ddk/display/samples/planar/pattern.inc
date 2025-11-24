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

	.xlist
;----------------------------Module-Header------------------------------;
; Module Name: PATTERN.BLT
;
; Subroutine for pattern preprocessing.
;
; Labels used but not defined here:
;	None
;
; This file is part of a set that makes up the Windows BitBLT function
; at driver-level.
;-----------------------------------------------------------------------;
	.list


	.xlist
if	MASMFLAGS and PUBDEFS
	public	pattern_preprocessing
ifdef	GEN_COLOR_BLT
	public	pattern_preproc_color
	public	pattern_preproc_mono
endif
	public	pattern_preproc_end
	public	pattern_preproc_error
endif
	.list

;----------------------------Private-Routine----------------------------;
; pattern_preprocessing
;
; If a pattern is needed, make sure that it isn't a hollow
; brush. If it is a hollow brush, then return an error.
;
; The type of brush to use will be set, and the brush pointer
; updated to point to the mono bits if the mono brush will be
; used.  The type of brush used will match the destination device.
;
; If the destination is mono and the source is color, then a mono
; brush fetch will be used, with the color brush munged in advance
; according to the background/foreground colors passed:
;
;     All brush pixels which match the background color should be set
;     to white (1).  All other brush pixels should be set to black (0).
;
;     If the physical color is stored as all 1's or 0's for each
;     plane, then by XORing the physical color for a plane with the
;     corresponding byte in the brush, and ORing the results, this
;     will give 0's where the color matched, and  1's where the colors
;     didn't match.  Inverting this result will then give 1's where
;     the brush matched the background color and 0's where it did not.
;
; If both the source and destination are color, or the source is mono
; and the destination color, then the color portion of the brush will
; be used.
;
; If both the source and destination are mono, then the monochrome
; portion of the brush will be used.
;
; Entry:
;	BH = gl_the_flags
; Returns:
;	Carry flag clear if no error
; Error Returns:
;	Carry flag set if error (null lpPBrush, or hollow brush)
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,flags
; Calls:
;	None
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assume	ds:nothing
	assume	es:nothing

pattern_preprocessing	proc	near

	mov	gl_the_flags,bh		;Save flag values
	test	bh,F0_PAT_PRESENT	;Pattern required?
	jz	pattern_preproc_end	;  No, skip pattern check

	lds	si,lpPBrush		;--> physical brush
	mov	ax,ds
	or	ax,si
	jz	pattern_preproc_error	;Null pointer, error

	les	di,lpDrawMode		;to get background/text color
	mov	ax, oem_brush_style[si] ;AX: brush style
	cmp	ax, BS_HOLLOW
	je	pattern_preproc_error	;Hollow brush.	Abort with an error
	cmp	ax, BS_MONO_PATTERN	;monochrome patterned brush?
	je	PPFillColorPortion	;yes.  Go generate color part of brush
	mov	dl,oem_brush_accel[si]	;Save EGA brush accelerator
	mov	cl_brush_accel,dl	; in local variable frame

ifdef	GEN_COLOR_BLT
	shl	bh,1			;What type of pattern fetch?
	js	pattern_preproc_end	;Color pattern Fetch.  OK as is
	jnc	pattern_preproc_mono	;Not going color ==> mono, OK as is
	errnz	F0_COLOR_PAT-01000000b
	errnz	F0_GAG_CHOKE-10000000b


;	This is a color ==> mono BLT.  The color brush must be processed
;	against the background and foreground colors as stated above,
;	giving a pseudo monochrome brush.
;
;	This new brush will be stored on the frame and the brush pointer
;	biased to point to it.


	mov	dx,wptr es:bkColor.pcol_C0[di]	; DL = red, DH = green
	.errnz	pcol_C1 - pcol_C0 - 1
if NUMBER_PLANES eq 3
	mov	bl,bptr es:bkColor.pcol_C2[di]	; BL = blue = either FF or 00
else
	mov	bx,wptr es:bkColor.pcol_C2[di]	; BL = blue = either FF or 00
	.errnz	SPECIAL - pcol_C2 - 1
	and	bh,C3_BIT
	neg	bh
	sbb	bh,bh		; BH = intensity plane = either FF or 00
endif
	mov	ax,ss
	mov	es,ax
	lea	di,cl_a_brush		;ES:DI --> temp brush area
	mov	cx,SIZE_PATTERN		;Set loop count

pattern_preproc_color:
	lodsb				;Get red plane
	mov	ah,oem_brush_C1-1[si]	;Get green plane
	xor	ax,dx			;Set matching bits to 0
if NUMBER_PLANES eq 3
	or	ah,al
	mov	al,oem_brush_C2-1[si]	;Process blue plane
	xor	al,bl
else
	push	di			;save
	mov	di,ax
	mov	al,oem_brush_C2-1[si]	;Process blue plane
	mov	ah,oem_brush_C3-1[si]	;Process intensity plane
	xor	ax,bx
	or	ax,di
	pop	di			;restore
endif
	or	al,ah			;Combine red and green
	not	al			;Do final inversion
	stosb				;  and store the byte
	loop	pattern_preproc_color

	mov	seg_lpPBrush,es 	;Set segment of brush
	lea	si,-oem_brush_style[di]	;Set up for offsetting mono brush

pattern_preproc_mono:
endif	;GEN_COLOR_BLT
	add	si,oem_brush_mono	;--> mono portion of the brush
	mov	off_lpPBrush,si

pattern_preproc_end:
	clc
	ret

pattern_preproc_error:
	stc
	ret

PPFillColorPortion:
	push	bx			;save the flags in BX
	mov	al, bh			;AL: gl_the_flags
	mov	bh, es:[di].bkColor.SPECIAL
	mov	ah, es:[di].TextColor.SPECIAL
	test	al, F0_COLOR_PAT	;is destination in color?
	jnz	PPDestIsColor		;yes, use bk/text color as is
	shiftl	bh, 4			;dest is monochrome. use intensity bit
	sbb	bh, bh			; to determine whether the dest is
	shiftl	ah, 4			; black or white.
	sbb	ah, ah
	and	bh, 3fh
	and	ah, 3fh
	or	ah, 20h 		;these colors are ZEROS_OR_ONES now.
	or	bh, 20h

PPDestIsColor:
	cmp	bh, [si].oem_brush_bg	;is the color portion already updated?
	jne	PPDoColorPortion	;nope, we still need to do that
	cmp	ah, [si].oem_brush_fg	;does the text color match?
	je	PPDoColorPortion_10	;yes, we don't need to do anything here
PPDoColorPortion:
	mov	[si].oem_brush_bg, bh	;store the current bk/text colors in
	mov	[si].oem_brush_fg, ah	; PBrush
	xchg	di, si			;make DS:DI-->PBrush
	call	BrushMonoToColor	;create the appropriate color portion
	xchg	di, si			;DS:SI-->PBrush, ES:DI-->DrawMode
PPDoColorPortion_10:
	mov	al,oem_brush_accel[si]	;Save EGA brush accelerator
	mov	cl_brush_accel,al	; in local variable frame
	pop	bx			;restore those flags
	clc				;clear carry
	ret				;we're done converting this brush!

pattern_preprocessing	endp

;*********************************Public*Routine*******************************
;BrushMonoToColor is used to convert the monochrome portion of a patterned
;brush (style BS_MONO_PATTERN) into the color portion using the current
;foreground and text colors as passed in in the DrawMode.
;On entry:
;DS:DI-->PBrush
;BH: background color
;AH: foreground (text) color
;Destoys AX, BX, CX, DX
;******************************************************************************

BrushMonoToColor    proc near
	and	[di].oem_brush_accel, not GREY_SCALE
	test	ah, ONES_OR_ZEROS	;if both background and text color
	jz	BMC_10			; have the ONES_OR_ZEROS bit set, we
	test	bh, ONES_OR_ZEROS	; can set the GREY_SCALE flag in the
	jz	BMC_10			; physical brush.
	or	[di].oem_brush_accel, GREY_SCALE
BMC_10:
	push	bp			;we'll need BP as an index register
	sub	bp, bp
	mov	al, 8			;AL: outer loop counter
mono_to_color_outer_loop:
	mov	bl, ds:[di].oem_brush_mono[bp]
	push	ax
	mov	al, 8			;AL is also our inner loop counter
mono_to_color_inner_loop:
	push	ax
	mov	al, bh			;assume we need the background color
	shl	bl, 1			;foreground or background color?
	jc	get_fill_got_color
	mov	al, ah			;nope, need text (foreground) color
get_fill_got_color:
	shr	al, 1			; plane 0 in CY
	rcl	cl, 1			;and put it into CL
	shr	al, 1			;same for plane 1
	rcl	ch, 1
	shr	al, 1
	rcl	dl, 1
	shr	al, 1
	rcl	dh, 1
	pop	ax			;restore inner loop count in AL
	dec	al
	jnz	mono_to_color_inner_loop
	mov	ds:[di].oem_brush_C0[bp], cl
	mov	ds:[di].oem_brush_C1[bp], ch
	mov	ds:[di].oem_brush_C2[bp], dl
	mov	ds:[di].oem_brush_C3[bp], dh
	inc	bp			;update index into each plane
	pop	ax			;restore outer loop count AL
	dec	al			;update outer loop count
	jnz	mono_to_color_outer_loop
	pop	bp			;restore base pointer to local vars.
	ret
BrushMonoToColor    endp

public	BrushMonoToColorFar
BrushMonoToColorFar proc far		;this routine is also called from
	call	BrushMonoToColor	;do_scanlines
	ret
BrushMonoToColorFar endp
