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
;----------------------------Module-Header------------------------------;
; Module Name: BITBLT.ASM
;
; BitBLT at level of device driver.
;
; This is the main module of those comprising the source to BitBLT
; (Bit BLock Transfer) for Microsoft Windows display drivers. It
; defines the procedure, and performs general preprocessing for all BLT
; requests.
;
; BitBLT  transfers a rectangle of bits from source to destination,
; doing some useful operations on the way, namely:
;
; o	clipping the source rectangle to fit within the
; 	source device dimensions;
;
; o	excluding the cursor;
;
; o	performing a user-specified raster operation, out of
; 	a vast array of choices, which takes the form
;
; 	D = f(S,D,P)
;
; 	where S = source bit, D = destination bit, P = pattern
; 	bit, and  f  is a sequence of logical operations (AND, OR,
;	XOR, NOT) on S, D, and P;
;		
; o	recognizing common special cases for accelerated processing.
;
;
; For a detailed explanation of the contortions BitBLT goes through
; to put your bits in place, see the file COMMENT.INC.
;
;
; BitBLT consists of the following files:
;
;	BITBLT.ASM		procedure definition
;	CBLT.ASM		procedure to compile arbitrary BLT on stack
;
;	GENLOCAL.INC		function parameters and generic locals
;	CLRLOCAL.INC		color/monochrome-related locals
;	DEVLOCAL.INC		device-related locals
;
;	GENCONST.INC		generic constants
;	CLRCONST.INC		color/monochrome constants
;	DEVCONST.INC		constants used by device-dependent code
;
;	GENDATA.INC		generic compiled code templates and data
;	CLRDATA.INC		color/monochrome-dependent templates and data
;	DEVDATA.INC		device-dependent code templates and data
;
;	ROPDEFS.INC		constants relating to ROP definitions
;	ROPTABLE.INC		table of ROP templates
;
;	PDEVICE.INC		PDevice processing
;	PATTERN.INC		pattern preprocessing
;	COPYDEV.INC		copy device data into local frame
;	COMPUTEY.INC		compute y-related values
;
;	EXIT.INC		device-specific cleanup before exiting
;	SPECIAL.INC		special case code
;
;	COMMENT.INC		overview of history and design
;-----------------------------------------------------------------------;

THIS_IS_DOS_3_STUFF = 1		; remove this line for WinThorn

	title	BitBLT
	%out	BitBlt


;	This function will perform private stack checking.  In order for
;	private stack checking to occur, two symbols must be defined
;	prior to the inclusion of CMACROS.INC.	?CHKSTK must be defined
;	if the cmacros are to perform stack checking on procedures with
;	local parameters.  ?CHKSTKPROC must be defined if private stack
;	checking will be used.
;
;	The actual macro body for ?CHKSTKPROC will be defined later.
;	(See MACROS.INC).

?CHKSTK		 = 1
?CHKSTKPROC	macro
		endm
		public	process_stack_code
ifdef	THIS_IS_DOS_3_STUFF
else
	.286p
endif


;	Define the portions of GDIDEFS.INC that will be needed by bitblt.

incLogical	= 1		;Include GDI logical object definitions
incDrawMode	= 1		;Include GDI DrawMode definitions
incBrushStyle	= 1

	.xlist
	include MACROS.INC
	include CMACROS.INC
	include GDIDEFS.INC
	include DISPLAY.INC
	include	MFLAGS.INC
	include CURSOR.INC
	include DEBUG.INC

ifdef	TEFTI
if1
	%out TEFTI timing code is present. Remove before RELEASE!!
endif	
	include TEFTI.INC
endif
	.list


	externA SCREEN_W_BYTES		;Screen width in bytes

ifdef	PALETTES
	externB PaletteModified         ;Set when palette is modified
	externFP TranslateBrush		;translates the brush
	externFP TranslateTextColor     ;translates text colors
endif

ifdef	THIS_IS_DOS_3_STUFF
	externA ScreenSelector		;Segment of Regen RAM
endif
	externNP	CBLT		;(must be NP, even though defined
					; as FAR -- see CBLT.ASM)

	externFP	AllocSelector	; allocate a new selector
	externFP	PrestoChangeoSelector ; CS <--> DS conversion
	externFP	FreeSelector 	; free an allocated selector

	externFP	AllocDSToCSAlias; alocates a CS alias for a Data seg
ifdef	EXCLUSION			;If cursor exclusion
	externNP	exclude		;Exclude area from screen
	externNP	unexclude	;Restore excluded area to screen
endif

ifdef	THIS_IS_DOS_3_STUFF
sBegin	Data

	externB enabled_flag		;Non-zero if output allowed
	externW	ScratchSel		; the free selector

ifdef   PALETTES
	externB	PaletteModified		;0ffh if palette modified
endif

ifdef 	TEFTI
	externD	BitBlt_time
	externD	BitBlt_count
endif

ifdef _BANK
	externNP SetBanks
	externNP SetReadBank
	externNP SetWriteBank
	externNP NextReadBank
	externNP NextWriteBank
	externNP PreviousReadBank
	externNP PreviousWriteBank
endif

sEnd	Data
endif


sBegin	Code
assumes cs,Code
assumes ds,Data
assumes es,nothing


;	Following are the BitBLT include-files.  Some are commented out
;	because they contain address definitions are are included in
;	CBLT.ASM, but are listed here for completeness.  The remaining
;	files include those that make up the local variable frame, and 
;	those containing subroutines.  The frame-variable files are
;	included immediately after the cProc BITBLT declaration.  The
;	subroutines files are included near the end of this file.

	.xlist
	include GENCONST.INC	;EQUs
	include	CLRCONST.INC	;EQUs
	include	DEVCONST.INC	;EQUs
	include	GENDATA.INC	;bitmask and phase tables
	include	CLRDATA.INC	;Color/mono specific templates,data
;	include DEVDATA.INC	;Driver specific templates,data
	include ROPDEFS.INC	;Raster operation definitions
	include	ROPTABLE.INC	;Raster operation code templates
	.list




cProc	BITBLT,<FAR,PUBLIC>,<si,di>

	.xlist
	include	GENLOCAL.INC	;arguments and generic local vars
	include	CLRLOCAL.INC	;color/monochrome-related locals
	include	DEVLOCAL.INC	;device-related locals
	.list

cBegin

ifdef	TEFTI
	push	ds
	push	ax
	push	dx
	timer_begin
	pop	dx
	pop	ax
endif


ife	???				;If no locals
	?CHKSTKPROC 0			;See if room
endif
	jnc	bitblt_stack_ok		;There was room for the frame
	jmp	bitblt_stack_ov 	;There was no room


bitblt_stack_ok:
	mov	al,enabled_flag 	;Save enabled_flag while we still
	mov	local_enable_flag,al	;  have DS pointing to Data
	mov	ax,ScratchSel		; get the free selector
	mov	WorkSelector,ax		; save it

;----------------------------------------------------------------------------;
; if the palette manager is supported, do the on-the-fly translation now     ;
;----------------------------------------------------------------------------;

ifdef	PALETTES
	cmp	PaletteModified,0ffh	; was the palette modified
	jnz	no_translation
	push	ds			; save own segment
	lds	si,lpDestDev		; the destination device
	lodsw				; get the type of the device
	pop	ds			; get back own segment
	or	ax,ax			; test for physical display
	jz	no_translation		; no translation for a mem output
	arg	lpPBrush
	cCall	TranslateBrush
	mov	seg_lpPBrush,dx
	mov	off_lpPBrush,ax
	push	ds			; save local segment
	lds	si,lpSrcDev		; get the pointer to source desc
	mov	al,byte ptr [si].bmPlanes ; get the number of planes
	pop	ds
	cmp	al,1			; monochrome source ?
	jz	no_translation		; donot translate the colors

	arg	lpDrawMode
	cCall	TranslateTextColor
	mov	seg_lpDrawMode,dx
	mov	off_lpDrawMode,ax
no_translation:

endif
;----------------------------------------------------------------------------;

	subttl	ROP Preprocessing
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Get the encoded raster operation, and map the raster op if needed.
;
;	To map the ROPS 80h through FFh to 00h through 7Fh, take the
;	1's complement of the ROP, and invert the "negate needed" flag.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	cld				;Let's make no assumptions about this!

;----------------------------------------------------------------------------;

	xor	ax,ax			;Assume not 80h : FFh
	mov	bx,seg_Rop
	or	bh,bh			;Outside the legal range?
	jnz	complain		;  Yes, ignore it
	or	bl,bl			;Is this in the second half (80-FF)?
	jns	parse_10 		;  No, rop index is correct
	not	bl			;  Yes, want the inverse
	mov	ah,HIGH NEGATE_NEEDED	;Want to invert the not flag
	errnz	<LOW NEGATE_NEEDED>

parse_10:
	add	bx,bx			;Turn into a word index
	xor	ax,roptable[bx] 	;Get ROP, maybe toggle negate flag
	mov	gl_operands,ax		;Save the encoded raster operation

	mov	bl,ah			;Set gl_the_flags for source and pattern
	and	bl,HIGH (SOURCE_PRESENT+PATTERN_PRESENT)
	ror	bl,1

	errnz	 <SOURCE_PRESENT - 0010000000000000b>
	errnz	<PATTERN_PRESENT - 0100000000000000b>
	errnz	 <F0_SRC_PRESENT - 00010000b>
	errnz	 <F0_PAT_PRESENT - 00100000b>
	
	jmp	short parse_end


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	complain - complain that something is wrong
;
;	An error is returned to the caller without BLTing anything.
;
;	Entry:	None
;
;	Exit:	AX = 0 (error flag)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

complain:
	xor	ax,ax			;Set the error code
	jmp	bitblt_exit_fail

;	v_exit - Just a vector to Exit

v_exit:
	jmp	bitblt_exit

parse_end:


	call	pdevice_processing
	jc	complain
	test	BH,F0_SRC_IS_DEV+F0_DEST_IS_DEV
	jz	dev_isnt_involved	;If the device is involved in
	test	local_enable_flag,0FFh	;  the blt, then it must be
	jz	complain		;  enabled

dev_isnt_involved:
	call	pattern_preprocessing
	jc	complain


	subttl	Input Clipping
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	GDI doesn't do input clipping.  The source device must be clipped
;	to the device limits, otherwise an exception could occur while in
;	protected mode.
;
;	The destination X and Y, and the extents have been clipped by GDI
;	and are positive numbers (0-7FFFh).  The source X and Y could be
;	negative.  The clipping code will have to check constantly for
;	negative values.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

input_clipping:
input_clip_x:
	mov	si,xExt 		;X extent will be used a lot
	mov	di,yExt 		;Y extent will be used a lot
	test	gl_the_flags,F0_SRC_PRESENT;Is there a source?
	jz	input_clip_chk_null_blt	;No source, no input clipping needed

	mov	ax,SrcxOrg		;Will need source X org
	mov	bx,gl_src.width_bits	;Maximum allowable is width_bits-1
	or	ax,ax			;Any left edge overhang?
	jns	input_clip_rgt_edge	;  No, left edge is on the surface


;	The source origin is off the left hand edge of the device surface.
;	Move both the source and destination origins right by the amount
;	of the overhang and also remove the overhang from the extent.
;
;	There is no need to check for the destination being moved off the
;	right hand edge of the device's surface since the extent would go
;	zero or negative were that to happen.


	add	si,ax			;Subtract overhang from X extent
	js	v_exit			;Wasn't enough, nothing to BLT
	sub	DestxOrg,ax		;Move destination left
	xor	ax,ax			;Set new source X origin
	mov	SrcxOrg,ax


;	The left hand edge has been clipped.  Now clip the right hand
;	edge.  Since both the extent and the source origin must be
;	positive numbers now, any sign change from adding them together
;	can be ignored if the comparison to bmWidth is made as an
;	unsigned compare (maximum result of the add would be 7FFFh+7FFFh,
;	which doesn't wrap past zero).


input_clip_rgt_edge:
	add	ax,si			;Compute right edge + 1
	sub	ax,bx			;Compute right edge overhang
	jbe	input_clip_save_xext	;No overhang
	sub	si,ax			;Subtract overhang from X extent
	js	v_exit			;Wasn't enough, nothing to BLT

input_clip_save_xext:
	mov	xExt,si 		;Save new X extent


;	Now clip the Y coordinates.  The procedure is the same and all
;	the above about positive and negative numbers still holds true.


input_clip_y:
	mov	ax,SrcyOrg		;Will need source Y org
	mov	bx,gl_src.height	;Maximum allowable is height-1
	or	ax,ax			;Any top edge overhang?
	jns	input_clip_btm_edge	;  No, top is on the surface


;	The source origin is off the top edge of the device surface.
;	Move both the source and destination origins down by the amount
;	of the overhang, and also remove the overhang from the extent.
;
;	There is no need to check for the destination being moved off
;	the bottom of the device's surface since the extent would go
;	zero or negative were that to happen.


	add	di,ax			;Subtract overhang from Y extent
	js	v_exit			;Wasn't enough, nothing to BLT
	sub	DestyOrg,ax		;Move destination down
	xor	ax,ax			;Set new source Y origin
	mov	SrcyOrg,ax


;	The top edge has been clipped.	Now clip the bottom edge. Since
;	both the extent and the source origin must be positive numbers
;	now, any sign change from adding them together can be ignored if
;	the comparison to bmWidth is made as an unsigned compare (maximum
;	result of the add would be 7FFFh+7FFFh, which doesn't wrap thru 0).


input_clip_btm_edge:
	add	ax,di			;Compute bottom edge + 1
	sub	ax,bx			;Compute bottom edge overhang
	jbe	input_clip_save_yext		;No overhang
	sub	di,ax			;Subtract overhang from Y extent
	js	v_exit			;Wasn't enough, nothing to BLT

input_clip_save_yext:
	mov	yExt,di 		;Save new Y extent

input_clip_chk_null_blt:
	or	si,si
	jz	v_exit			;X extent is 0
	or	di,di
	jz	v_exit			;Y extent is 0

input_clip_end:


	subttl	Cursor Exclusion
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Cursor Exclusion
;
;	If either device or both devices are for the display, then
;	the cursor must be excluded.  If both devices are the display,
;	then a union of both rectangles must be performed to determine
;	the exclusion area.
;
;	Currently:
;		SI = X extent
;		DI = Y extent
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
cursor_exclusion:

ifdef	EXCLUSION
	mov	al,gl_the_flags
	and	al,F0_SRC_IS_DEV+F0_DEST_IS_DEV	;Are both memory bitmaps?
	jz	cursor_exclusion_end	;  Yes, no exclusion needed

	dec	si			;Make the extents inclusive of the
	dec	di			;  last point

	mov	cx,DestxOrg		;Assume only a destination on the
	mov	dx,DestyOrg		;  display
	test	al,F0_SRC_IS_DEV	;Is the source a memory bitmap?
	jz	cursor_exclusion_no_union;  Yes, go set right and bottom
	test	al,F0_DEST_IS_DEV	;  (set 'Z' if dest is memory)
	xchg	ax,cx			;  No, prepare for the union
	mov	bx,dx

	mov	cx,SrcxOrg		;Set source org
	mov	dx,SrcyOrg
	jz	cursor_exclusion_no_union;Dest is memory. Set right and bottom

;	The union of the two rectangles must be performed.  The top left
;	corner will be the smallest x and smallest y.  The bottom right
;	corner will be the largest x and the largest y added into the
;	extents

cursor_exclusion_not_ssb:
	cmp	cx,ax			;Get smallest x
	jle	cursor_exclusion_y	;CX is smallest
	xchg	ax,cx			;AX is smallest

cursor_exclusion_y:
	cmp	dx,bx			;Get smallest y
	jle	cursor_exclusion_union	;DX is smallest
	xchg	dx,bx			;BX is smallest

cursor_exclusion_union:
	add	si,ax			;Set right
	add	di,bx			;Set bottom
	jmp	short cursor_exclusion_do_it	;Go do exclusion

cursor_exclusion_no_union:
	add	si,cx			;Set right
	add	di,dx			;Set bottom

cursor_exclusion_do_it:
	call	exclude 		;Exclude the area from the screen

endif	;EXCLUSION

cursor_exclusion_end:


	subttl	Phase Processing (X)
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Now the real work comes along:  In which direction will the
;	copy be done?  Refer to the 10 possible types of overlap that
;	can occur (10 cases, 4 resulting types of action required).
;
;	If there is no source bitmap involved in this particular BLT,
;	then the path followed must allow for this.  This is done by
;	setting both the destination and source parameters equal.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

phase_processing:
phase_processing_x:
	mov	dx,xExt 		;Get X extent
	dec	dx			;Make X extent inclusive

	mov	bx,DestxOrg		;Get destination X origin
	mov	di,bx
	and	bx,00000111b		;Get offset of destination within byte
					;   and set up BX for a base register!


;	If there is no source, then just use the pointer to the destination
;	bitmap and load the same parameters, which will cause the "equality"
;	path to be followed in the set-up code.  This path is the favored
;	path for the case of no source bitmap.


	mov	ax,di			;Assume no source needed
	test	gl_the_flags,F0_SRC_PRESENT;Is a source needed?
	jz	phase_proc_10		;  No, just use destination parameters
	mov	ax,SrcxOrg		;  Yes, get source origin X
	mov	gl_first_fetch,FF_TWO_INIT_FETCHES
					;  Assume two initial fetches (if no
					;    source, then it will be set = 1
					;    later)
phase_proc_10:
	mov	si,ax
	and	ax,00000111b		;Get offset of source within byte

	cmp	si,di			;Which direction will we be moving?
	jl	phase_proc_30		;Move from right to left


;	The starting X of the source rectangle is >= the starting X of
;	the destination rectangle, therefore we will be moving bytes
;	starting from the left and stepping right.
;
;	Alternatively, this is the path taken if there is no source
;	bitmap for the current BLT.
;
;	Rectangle cases: 3,4,5,6,8

	sub	al,bl			;Compute horiz. phase  (source-dest)
	mov	gl_step_direction,STEPRIGHT ;Set direction of move
	mov	ah,cs:[bx].bitmask_tbl1	;Get starting byte mask
	ja	phase_proc_two_fetches	;Scan line case 2, everything is
					;  already set for this case.



;	Scan line cases 1 and 3:
;
;	The correct first byte fetch needs to be set for the beginning
;	of the outer loop, and the phase must be made into a positive
;	number.
;
;	This is the path that will be followed if there is no source bitmap
;	for the current BLT.

	mov	gl_first_fetch,FF_ONE_INIT_FETCH;Set one initial fetch
	jmp	short pp_only_one_init_fetch
;-----------------------------------------------------------------------;
; If we get all the bits we need in the first fetch then a second
; (unnecessary) fetch could cause a GP Fault.  So let's examine this:
; The number of bits from (SI mod 8) to the end of the byte is the number
; of available bits we get on the first fetch.	This is (8 - (SI mod 8)).
; If this is greater than or equal to xExt then we have all the bits we
; need and we better not do the second fetch (even though the phase
; relationship may suggest we need it).
;
; Conclusion: If (8 - (SI mod 8)) >= xExt then DO NOT make second fetch.
;-----------------------------------------------------------------------;

phase_proc_two_fetches:

	mov	cx,8
	sub	cl,bl
	sub	cl,al

; We can save a couple cycles here since xExt - 1 is already in DX.
; The condition CX >= xExt is the same as CX > DX.

	cmp	cx,dx			;CX = (SI mod 8), DX = (xExt - 1)
	jle	pp_second_fetch_really_needed


;-----------------------------------------------------------------------;
; We are here BECAUSE the xExt is so small that we can get all the bits
; on the scanline with a single lodsb (no byte boundary is crossed) AND
; the phase relationship indicates that a second initial fetch is needed.
;
; We will override it and only do one fetch.  However, if we simply
; fail to do the second fetch then the phase code will screw us.
; It will be expecting the bits to get fetched in the first fetch, saved
; after the rotate, and mixed in in the second fetch's phase code.
; So after the first fetch the bits have been saved in BH, and ANDed out
; of the src data in AL.
;
; The solution is to set a flag here that tells the phase generation code
; not to generate the usual masking part of the phase code.
;
; Short Bitblt Cases:			(8 bits or less)
;
;	1) neither crosses byte boundary.
;
;	   a) phase requires second initial fetch
;
;	      Kill the phase masking.  It will screw us.  There will
;	      be just one lodsb and one stosb and the first byte mask
;	      will protect the dest bits that should not get hit.
;
;	   b) phase requires only one initial fetch
;
;	      Phase masking is irrelevant.  Removing it would
;	      be an optimiztation.
;
;	2) dest crosses byte boundary, but src does not
;
;	   a) phase requires second initial fetch
;
;	      impossible situation:  the way we determine that a 2nd fetch
;	      is necessary is if the first fetch does not get enough needed
;	      bits to satisfy the first dest byte.  Here the first fetch
;	      gets ALL the bits and the first dest byte needs less than
;	      ALL because it crosses a byte boundary.
;
;	   b) phase requires only one initial fetch
;
;	      Intervention would be bad.  None is necessary since the 2nd
;	      initial fetch will not be done.  If we do intervene we will
;	      cause trouble:  Killing the masking will prevent the
;	      "saved bits" from being saved.  The first byte masking
;	      can kill off these bits in AL and they will never
;	      make it to the second stosb.
;
;	3) src crosses byte boundary  (dest may or may not)
;	   (this is known to be untrue at this point)
;
;	   There are bits we need in the second fetch, so a second
;	   initial fetch can not cause a GP fault.  Therefore do
;	   everything the same as we would have before.
;
;
; Conclusion:  Intervention to kill the phase masking is
;	       necessary iff
;		  [src does not cross byte boundary] AND
;		  dest does not cross byte boundary  AND
;		  [phase requires second initial fetch].
;	       and bad if
;		  dest crosses byte boundary, but [src does not]
;
; Statements in [] are known to be true at this point.
;
; Solution:
;
; If we always kill the phase-masking when neither crosses a byte
; boundary and never kill it otherwise then everyone will be happy
; (regardless of other conditions like whether phase requests a 2nd
; initial fetch).
;-----------------------------------------------------------------------;

	mov	gl_first_fetch,FF_ONLY_1_SRC_BYTE
	.errnz	FF_ONE_INIT_FETCH

pp_second_fetch_really_needed:
pp_only_one_init_fetch:
	mov	ch,ah

;-----------------------------------------------------------------------;


;	We now have the correct phase and the correct first character fetch
;	routine set.  Save the phase and ...
;
;	currently:   AL = phase
;		     BL = dest start mod 8
;		     CH = first byte mask
;		     DX = inclusive X bit count
;		     SI = source X start (if there is a source)
;		     DI = destination X start
;

phase_proc_20:
	add	al,8			;Phase must be positive
	and	al,00000111b



;	To calculate the last byte mask, the inclusive count can be
;	added to the start X MOD 8 value, and the result taken MOD 8.
;	This is attractive since this is what is needed later for
;	calculating the inclusive byte count, so save the result
;	of the addition for later.

	add	bx,dx			;Add inclusive extent to dest MOD 8
	mov	dx,bx			;Save for innerloop count !!!
	and	bx,00000111b		;Set up bx for a base reg
	mov	cl,cs:[bx].bitmask_tbl2	;Get last byte mask

;-----------------------------------------------------------------------;
; To avoid GP faults must never do an extra fetch we don't need.
; When we're ready for the last fetch there may already be enough bits
; saved from the previous fetch (which we plan to combine with the bits
; in the fetch we are about to do).  If so then we'd better not do this
; last fetch (it could cause a GP fault).
;
; The number of bits we have left from the previous byte is (8 - AL)
; AL is the phase.  (1 + BL) is the number of bits we actually need
; to write to the final destination byte.
;
; So if  (8 - AL) >= (1 + BL)  then DO NOT do the last fetch.  This
; simplifies:  if  (BL + AL) <= 7  then DO NOT do the last fetch.
;-----------------------------------------------------------------------;

	add	bl,al
	cmp	bl,7
	jg	phase_proc_last_fetch_needed
	or	gl_first_fetch,FF_NO_LAST_FETCH
phase_proc_last_fetch_needed:

	mov	bl,al			;Compute offset into phase mask table
	add	bx,bx
	mov	bx,cs:[bx].phase_tbl1	;Get the phase mask


;	Currently:
;		AL = phase
;		BX = phase mask
;		CL = last byte mask
;		CH = first byte mask
;		DX = inclusive bit count + dest start MOD 8
;		SI = source X start (if there is a source)
;		DI = destination starting X

	jmp	short phase_proc_50	;Finish here

;	The starting X of the source rectangle is < the X of the destination
;	rectangle, therefore we will be moving bytes starting from the right
;	and stepping left.
;
;	This code should never be reached if there is no source bitmap
;	for the current BLT.
;
;	Rectangle cases: 1,2,7

phase_proc_30:
	mov	gl_step_direction,ah	;Set direction of move
	errnz	STEPLEFT
	mov	cl,cs:[bx].bitmask_tbl1	;Get last byte mask
	push	bx
	add	ax,dx			;Find end of the source


;	To calculate the first byte mask, the inclusive count is
;	added to the start MOD 8 value, and the result taken MOD 8.
;	This is attractive since this is what is needed later for
;	calculating the inclusive byte count, so save the result
;	of the addition for later.

	add	bx,dx			;Find end of the destination
	add	di,dx			;Will need to update dest start address
	add	si,dx			;  and source's too
	mov	dx,bx			;Save inclusive bit count + start MOD 8
	and	ax,00000111b		;Get source offset within byte
	and	bx,00000111b		;Get dest   offset within byte
	mov	ch,cs:[bx].bitmask_tbl2	;Get start byte mask
	cmp	al,bl			;Compute horiz. phase  (source-dest)
	jb	pp_double_fetch		;Scan line case 5, everything is
					;  already set for this case.

;	Scan line cases 4 and 6:
;
;	The correct first byte fetch needs to be set for the beginning
;	of the outer loop

	mov	gl_first_fetch,FF_ONE_INIT_FETCH;Set initial fetch routine
	jmp	short pp_one_initial_fetch
;-----------------------------------------------------------------------;
; If only-one-fetch is already set, then the following is a NOP.
; It doesn't seem worth the effort to check and jmp around.
;
; If we get all the bits we need in the first fetch then a second
; (unnecessary) fetch could cause a GP Fault.  So let's examine this:
;
; (DX + SI) points to the first pel (remember we're stepping left).
; So the number of needed bits we get in the first fetch is
; ((DX + SI + 1) mod 8).  This is currently equal to AX.
; If AX >= xExt then we'd better not do two init fetches.
;-----------------------------------------------------------------------;

pp_double_fetch:
	dec	xExt
	cmp	ax,xExt
	jl	pp_double_fetch_really_needed
	mov	gl_first_fetch,FF_ONLY_1_SRC_BYTE
	.errnz	FF_ONE_INIT_FETCH
pp_double_fetch_really_needed:
	inc	xExt

pp_one_initial_fetch:
	sub	al,bl			;Compute horiz. phase  (source-dest)
	add	al,8			;Ensure phase positive
	and	al,00000111b

;-----------------------------------------------------------------------;
; To avoid GP faults must never do an extra fetch we don't need.
; The last byte fetch is unnecessary if Phase is greater than or equal
; to 8 - BL.  Phase is the number of bits we still have from the previous
; fetch. 8 - BL is the number of bits we actually need to write to the
; final destination byte.  So if AL - (8 - BL) >= 0  skip the last fetch.
;-----------------------------------------------------------------------;

	pop	bx
	add	bl,al
	sub	bl,8
	jl	pp_need_last_fetch
	or	gl_first_fetch,FF_NO_LAST_FETCH
pp_need_last_fetch:
phase_proc_40:
;-----------------------------------------------------------------------;

;	We now have the correct phase and the correct first character fetch
;	routine set.  Generate the phase mask and save it.
;
;	currently:   AL = phase
;		     CH = first byte mask
;		     CL = last byte mask
;		     DX = inclusive bit count + start MOD 8

	mov	ah,cl			;Save last mask
	mov	cl,al			;Create the phase mask
	mov	bx,00FFh		;  by shifting this
	shl	bx,cl			;  according to the phase
	mov	cl,ah			;Restore last mask
;	jmp	phase_proc_50		;Go compute # of bytes to BLT
	errn$	phase_proc_50




; The different processing for the different X directions has been
; completed, and the processing which is the same regardless of
; the X direction is about to begin.
;
; The phase mask, the first/last byte masks, the X byte offsets,
; and the number of innerloop bytes must be calculated.
;
;
; Nasty stuff coming up here!  We now have to determine how
; many bits will be BLTed and how they are aligned within the bytes.
; This is how it's done (or how I'm going to do it):
;
; The number of bits (inclusive number that is) is added to the
; start MOD 8 value ( the left side of the rectangle, minimum X
; value), then the result is divided by 8. Then:
;
;
;    1)	If the result is 0, then only one destination byte is being
;	BLTed.	In this case, the start & ending masks will be ANDed
;	together, the innerloop count (# of full bytes to BLT) will
;	be zeroed, and the gl_last_mask set to all 0's (don't alter any
;	bits in last byte which will be the byte following the first
;	(and only) byte).
;
;		|      x x x x x|		|
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 3,  extent-1 = 4
;		3+7 DIV 8 = 0, only altering one byte
;
;
;
;    2)	If the result is 1, then only two bytes will be BLTed.
;	In this case, the start and ending masks are valid, and
;	all that needs to be done is set the innerloop count to 0.
;	(it is true that the last byte could have all bits affected
;	the same as if the innerloop count was set to 1 and the
;	last byte mask was set to 0, but I don't think there would be
;	much time saved special casing this).
;
;		|  x x x x x x x|x x x x x x x|
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 1,  extent-1 = 14
;		3+14 DIV 8 = 1.  There is a first and last
;		byte but no innerloop count
;
;
;
;    3)	If the result is >1, then there is some number of entire
;	bytes to be BLted by the innerloop.  In this case the
;	number of innerloop bytes will be the result - 1.
;
;		|	       x|x x x x x x x x|x
;		|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;		 0 1 2 3 4 5 6 7
;
;		start MOD 8 = 7,  extent-1 = 9
;		7+9  DIV 8 = 2.  There is a first and last
;		byte and an innerloop count of 1 (result - 1)
;
;	Currently:	AL = horizontal phase
;			BX = horizontal phase mask
;			CH = first byte mask
;			CL = last byte mask
;			DX = left side X MOD 8 + inclusive X count
;			SI = source start X
;			DI = dest   start X


phase_proc_50:
	mov	gl_phase_h,al		;Save horizontal phase
	mov	gl_mask_p,bx		;Save phase mask
	shr	dx,3			;/8 to get full byte count
	jnz	phase_proc_60		;Result is >0, check it out


;	There will only be one byte affected.  Therefore the two byte masks
;	must be combined, the last byte mask cleared, and the innerloop
;	count set to zero.

	or	gl_first_fetch,FF_ONLY_1_DEST_BYTE
	and	ch,cl			;Combine the two masks
	xor	cl,cl			;Clear out the last byte mask
	inc	dx			;Now just fall through to set
	errn$	phase_proc_60		;  the innerloop count to 0!


phase_proc_60:
	dec	dx			;Dec count (might become 0 just like
	mov	gl_inner_loop_count,dx 	;  we want), and save it
	mov	bl,ch
	mov	ch,cl			;Compute last byte mask
	not	cl			;  and save it
	mov	gl_last_mask,cx
	mov	bh,bl			;Compute start byte mask
	not	bl			;  and save it
	mov	gl_start_mask,bx



;	There may or may not be a source bitmap for the following address
;	computation.  If there is no source, then the vertical setup code
;	will be entered with both the source and destination Y's set to the
;	destination Y and the address calculation skipped.  If there is a
;	source, then the address calculation will be performed and the
;	vertical setup code entered with both the source and destination Y's.

phase_processing_y:
	shiftr	di,3			;Compute byte offset of destination
					;  and add to current destination
					;  offset
	add	wptr gl_dest.lp_bits[0],di

	mov	dx,DestyOrg		;Get destination Y origin
	mov	ax,dx			;Assume no source
	mov	cl,gl_the_flags
	test	cl,F0_SRC_PRESENT	;Is a source needed?
	jz	phase_proc_70		;  No, skip source set-up

	shiftr	si,3			;Compute byte offset of source
					;  and add to current source offset
	add	wptr gl_src.lp_bits[0],si
	mov	ax,SrcyOrg		;Get source Y origin



	subttl	Phase Processing (Y)
	page

;	The horizontal parameters have been calculated.  Now the vertical
;	parameters must be calculated.
;
;	Currently:
;		DX = destination Y origin
;		AX = source Y origin (destination origin if no source)
;		CL = gl_the_flags

phase_proc_70:
	mov	bx,yExt 		;Get the Y extent of the BLT
	dec	bx			;Make it inclusive



;	The BLT will be Y+ if the top of the source is below or equal
;	to the top of the destination (cases: 1,4,5,7,8).  The BLT
;	will be Y- if the top of the source is above the top of the
;	destination (cases: 2,3,6)
;
;
;		  !...................!
;		  !D		      !
;	      ____!		..x   !
;	     |S   !		  :   !     Start at top of S walking down
;	     |	  !		      !
;	     |	  !...................!
;	     |			  :
;	     |____________________:
;
;
;	      __________________
;	     |S 		|
;	     |	  .....................     Start at bottom of S walking up
;	     |	  !D		      !
;	     |	  !		:     !
;	     |____!	      ..x     !
;		  !		      !
;		  !....................


	mov	ch,INCREASE		;Set Y direction for top to bottom
	cmp	ax,dx			;Which direction do we move?
	jge	phase_proc_80		;Step down screen (cases: 1,4,5,7,8)


;	Direction will be from bottom of the screen up (Y-)
;
;	This code will not be executed if there is no source since
;	both Y's were set to the destination Y.


	add	dx,bx			;Find bottom scan line index for
	add	ax,bx			;  destination and source
	mov	ch,DECREASE		;Set pattern increment

phase_proc_80:
	mov	gl_pat_row,dl		;Set pattern row and increment
	mov	gl_direction,ch
	sar	ch,1			;Map FF==>FF, 01==>00
	errnz	DECREASE-0FFFFh
	errnz	INCREASE-00001h



;	The Y direction has been computed.  Compute the rest of the
;	Y parameters.  These include the actual starting address,
;	the scan line and plane increment values, and whether or not
;	the extents will cross a 64K boundary.
;
;	Currently:
;		DX = Y of starting destination scan
;		AX = Y of starting source scan
;		CH = BLT direction
;		       00 = increasing BLT, Y+
;		       FF = decreasing BLT, Y-
;		CL = gl_the_flags
;		BX = inclusive Y extent

ifdef	_BANK
	push	ss:[0]
	mov	ss:[0],0ffffh
endif

phase_proc_90:
	test	cl,F0_SRC_PRESENT	;Is a source needed?
	mov	cl,ch			;  (Want CX = +/- 1)
	jz	phase_proc_100		;  No, skip source set-up
	push	dx			;Save destination Y
	push	bp			;Mustn't trash frame pointer
	lea	bp,gl_src		;--> source data structure
	call	compute_y		;Process as needed
ifdef _BANK
	test	dev_flags[bp],IS_DEVICE	; is it the physical display
	jz	short @f
	mov	ss:[0],dl
@@:
endif
	pop	bp
	pop	dx			;Restore destination Y

phase_proc_100:
	push	bp			;Mustn't trash frame pointer
	mov	ax,dx			;Put destination Y in ax
	lea	bp,gl_dest 		;--> destination data structure
	call	compute_y
ifdef _BANK
	test	dev_flags[bp],IS_DEVICE	; is it the physical display
	jz	short @f
	mov	ss:[1],dl
@@:
endif
	pop	bp			;Restore frame pointer

ifdef _BANK
	mov	dx,ss:[0]
	cmp	dx,0ffffh
	je	short @f
	xor	dl,dh
	js	short SetBoth
	xor	dl,dh
	mov	al,dl
	push	dx
	call	SetReadBank
	pop	dx
	mov	al,dh
	call	SetWriteBank
	jmp	short @f
SetBoth:
	xor	dl,dh
	and	dl,dh
	mov	al,dl
	call	SetBanks
@@:
	pop	ss:[0]
endif
	call	check_device_special_cases
	jc	bitblt_exit		;C ==> BLT done w/special case

	subttl	Memory allocation for BLT compilation
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Allow room for the BLT code.  The maximum that can be generated
;	is defined by the variable MAX_BLT_SIZE.  This variable must be
;	an even number.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

	assumes cs,Code
	assumes ds,nothing
	assumes es,nothing


cblt_allocate:
	?CHKSTKPROC MAX_BLT_SIZE+20h	;See if room on stack	
	jc	bitblt_stack_too_small  ;There was no room

cblt_alloc_stack_ok:
	add	sp,20h			;Take off the slop

	mov	di,sp
	mov	off_gl_blt_addr,di	;Save the address for later
	mov	ax,ss			;Set the segment for the BLT
	mov	es,ax

ifdef	THIS_IS_DOS_3_STUFF
else
	assumes ss,InstanceData
	mov	ax,proc_cs_alias
	assumes ss,nothing
endif

;----------------------------------------------------------------------------;
; we will now get a free selector and convert it into a code segment alias of;
; the stack selector and later free it.				             ;
;----------------------------------------------------------------------------;

	mov	ax,WorkSelector		; get the free selector

	push	es			; save stack selector

; convert it to a code segment copy of SS

	cCall	PrestoChangeoSelector,<ss,ax>
	mov	seg_gl_blt_addr,ax	;Save the address for later
	pop	es			; get back stack selector

; we will now execute off this new code selector from the stack


	assumes ds,Code

	cCall	CBLT			;compile the BLT onto the stack


	subttl	Invocation and Exit
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	If the debug flag has been set, save the size of the created BLT
;	so it may be returned to the caller.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

call_blt:

if	MASMFLAGS and DEBUG
	sub	di,off_gl_blt_addr	;Compute the length
	push	di			;  and save it
endif


;	The BLT has been created on the stack.	Set up the initial registers,
;	set the direction flag as needed, and execute the BLT.


	test	gl_the_flags,F0_SRC_PRESENT ;Is there a source?
	jz	call_blt_get_dest_bits	;  No, don't load its pointer
	lds	si,gl_src.lp_bits	;--> source device's first byte

call_blt_get_dest_bits:
	les	di,gl_dest.lp_bits	;--> destination device's first byte
	mov	cx,yExt 		;Get count of lines to BLT
	cld				;Assume this is the direction
	cmp	gl_step_direction,STEPRIGHT ;Stepping to the right?
	jz	call_blt_do_it		;  Yes
	std

call_blt_do_it:
	push	bp			;MUST SAVE THIS

process_stack_code:
	call	gl_blt_addr 		;Call the FAR process
	pop	bp

if	MASMFLAGS and DEBUG
	pop	bx			;Get length of created BLT code
endif
	add	sp,MAX_BLT_SIZE		;Return BLT space

;	jmp	bitblt_exit		;Hey, we're done!
	errn$	bitblt_exit


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	exit - leave BitBLT
;
;	Well, the BLT has been processed.  Restore the stack to its
;	original status, restore the saved user registers, show no
;	error, and return to the caller.
;
;	Entry:	None
;
;	Exit:	AX = 1
;
;	Uses:	All
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

bitblt_exit:
	call	clean_up_before_exit

	mov	ax,1			;Clear out error register (good exit)
;	jmp	bitblt_exit_fail
	errn$	bitblt_exit_fail


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	bitblt_exit_fail - exit because of failure
;
;	The BLT is exited.
;
;	Entry:	AX = error code (0 if error)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

bitblt_exit_fail:
	cld				;Leave direction cleared
ifdef	EXCLUSION			     
	call	unexclude		;Remove any exclusion area
endif

bitblt_stack_ov:

ifdef TEFTI
	pop	ds
	assumes ds,Data
	push	ax
	push	dx
	timer_end
	add	word ptr BitBlt_time,ax
	adc	word ptr BitBlt_time+2,dx
	add	word ptr BitBlt_count,1
	adc	word ptr BitBlt_count+2,0
	pop	dx
	pop	ax
endif

cEnd

bitblt_stack_too_small:
	DebugErr DBF_ERROR,"No room on stack for Blt code."
	jmp	bitblt_exit_fail	;There was no room


;-----------------------------------------------------------------------;
;	Subroutines.  These have been included with the aim of
;	segregating device dependent code from independent code,
;	while cleanly preserving the local variable frame.
;-----------------------------------------------------------------------;

	include	PDEVICE.INC	;PDevice processing
	include	PATTERN.INC	;pattern preprocessing
	include	COPYDEV.INC	;copy_dev procedure
	include	COMPUTEY.INC	;compute_y procedure
	include	SPECIAL.INC	;non-compiled BLT subroutines
	include	EXIT.INC	;device-specific cleanup before exit
sEnd	Code

if 	MASMFLAGS and PUBDEFS
	public	bitblt_stack_ok
	public	parse_10
	public	complain
	public	v_exit
	public	parse_end
	public	input_clipping
	public	input_clip_x
	public	input_clip_rgt_edge
	public	input_clip_save_xext
	public	input_clip_y
	public	input_clip_btm_edge
	public	input_clip_save_yext
	public	input_clip_chk_null_blt
	public	input_clip_end
	public	cursor_exclusion
ifdef	EXCLUSION
	public	cursor_exclusion_y
	public	cursor_exclusion_union
	public	cursor_exclusion_no_union
	public	cursor_exclusion_do_it
endif	;EXCLUSION
	public	cursor_exclusion_end
	public	phase_processing
	public	phase_processing_x
	public	phase_proc_10
	public	phase_proc_20
	public	phase_proc_30
	public	phase_proc_40
	public	phase_proc_50
	public	phase_proc_60
	public	phase_processing_y
	public	phase_proc_70
	public	phase_proc_80
	public	phase_proc_90
	public	phase_proc_100
	public	cblt_allocate
	public	cblt_alloc_stack_ok
	public	call_blt
	public	call_blt_get_dest_bits
	public	call_blt_do_it
	public	bitblt_exit
	public	bitblt_exit_fail
	public	bitblt_stack_ov
	public  compute_y
endif
end
