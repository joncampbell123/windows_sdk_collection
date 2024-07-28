	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: BITBLT.ASM
;
; BitBLT at level of device driver.
;
; Created: In Windows' distant past (c. 1983)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
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
;
; For a detailed explanation of the contortions BitBLT goes through
; to put your bits in place, see the file COMMENT.BLT.
;
;
; BitBLT consists of the following files:
;
;	BITBLT.ASM		procedure definition
;	CBLT.ASM		procedure to compile arbitrary BLT on stack
;
;	GENLOCAL.BLT		function parameters and generic locals
;	CLRLOCAL.BLT		color/monochrome-related locals
;	DEVLOCAL.BLT		device-related locals
;
;	GENCONST.BLT		generic constants
;	CLRCONST.BLT		color/monochrome constants
;	DEVCONST.BLT		constants used by device-dependent code
;
;	GENDATA.BLT		generic compiled code templates and data
;	CLRDATA.BLT		color/monochrome-dependent templates and data
;	DEVDATA.BLT		device-dependent code templates and data
;
;	ROPDEFS.BLT		constants relating to ROP definitions
;	ROPTABLE.BLT		table of ROP templates
;
;       8514.BLT                8514 specific BitBlt code
;	PDEVICE.BLT		PDevice processing
;	PATTERN.BLT		pattern preprocessing
;	COPYDEV.BLT		copy device data into local frame
;	COMPUTEY.BLT		compute y-related values
;
;	COMMENT.BLT		overview of history and design
;-----------------------------------------------------------------------;
.286c

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
;	(See MACROS.MAC).

?CHKSTK		 = 1
?CHKSTKPROC	macro
		endm


;	Define the portions of GDIDEFS.INC that will be needed by bitblt.

incLogical	= 1		;Include GDI logical object definitions
incDrawMode	= 1		;Include GDI DrawMode definitions

	.xlist
	include MACROS.MAC
	include CMACROS.INC
	include GDIDEFS.INC
	include DISPLAY.INC
	include 8514.inc	; definitions needed in 8514.blt
	.list

externA __AHIncr

sBegin	Data
externW     TempSelector
externB     WriteEnable
sEnd	Data

externFP    PrestoChangoSelector	; imported from KERNEL for protect mode

sBegin	Code
assumes cs,Code
assumes ds,Data
assumes es,nothing
                                                         

externFP	GlobalAlloc		;need these KERNEL functions in mono
externFP	GlobalFree		;bmp to color dev/bmp Blts.
externFP	GlobalLock
externFP	GlobalUnlock

externFP        CBLT                            ;in CBLT.ASM
externNP        BoardBlt                        ;in BOARDBLT.ASM
externNP	ConvertBrush			;in BRUSH.ASM


;	Following are the BitBLT include-files.  Some are commented out
;	because they contain address definitions and are included in
;	CBLT.ASM, but are listed here for completeness.  The remaining
;	files include those that make up the local variable frame, and 
;	those containing subroutines.  The frame-variable files are
;	included immediately after the cProc BITBLT declaration.  The
;	subroutines files are included near the end of this file.

	.xlist
        include GENCONST.BLT    ;EQUs
        include DEVCONST.BLT    ;structures
        include GENDATA.BLT     ;bitmask and phase tables
        include ROPDEFS.BLT     ;Raster operation definitions
        include ROPTABLE.BLT    ;Raster operation code templates
	.list

cProc	BITBLT,<FAR,PUBLIC>,<si,di>

        include GENLOCAL.BLT    ;arguments and generic local vars
        include CLRLOCAL.BLT    ;color/monochrome-related locals
        include DEVLOCAL.BLT    ;device-related locals
;
;       Define _cstods.  This location will contain our data segment value.
;We do this in a convenient way which does not take up any space in the code.
;since the first instruction in the WINDOWS preamble is:
;
;       MOV     AX,DataSegmentAddress

;We simply put a label on byte 1 of this instruction which just so happens to
;be the value of the Data segment!  Then, whenever a routine needs this value,
;he can access this label as a variable:
;
;       MOV     DS,cs:_cstods
;                     
;instead of saying:
;
;       MOV     AX,seg xxxxxx
;       MOV     DS,AX

        org     $+1
_cstods label   word
        org     $-1
        public  _cstods

cBegin

ife	???				;If no locals
	?CHKSTKPROC 0			;See if room
endif
	jnc	bitblt_stack_ok		;There was room for the frame
	jmp	bitblt_stack_ov 	;There was no room


	subttl	ROP Preprocessing
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	Get the encoded raster operation, and map the raster op if needed.
;
;	To map the ROPS 80h through FFh to 00h through 7Fh, take the
;	1's complement of the ROP, and invert the "negate needed" flag.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

bitblt_stack_ok:
	mov	al, [WriteEnable]
	mov	bWriteEnable, al
	mov	ax, TempSelector
	mov	LocalSelector, ax
        cld                             ;Let's make no assumptions about this!

parse_rop:
	xor	ax,ax			;Assume not 80h : FFh
	mov	hTempMonoBmp, ax	;handle initially 0
	mov	hTempMonoBits, ax
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
@@:
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
        jmp     bitblt_exit_fail

;	v_exit - Just a vector to Exit

v_exit:
	jmp	bitblt_exit

parse_end:
	call	pdevice_processing	;now BH: gl_the_flags; trashes DS
	mov	gl_the_flags, bh	;time to save those flags
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


;       The source origin is off the left hand edge of the bitmap surface.
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
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
; Check for device special cases.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
	call	HiresBLTProcessing	;go determine and do a device bitblt
	jc	v_exit			;if carry set, we did a device bitblt

	xor	bx, bx
	call	pattern_preprocessing	;trashes DS
	jc	v_exit


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


phase_proc_90:
	test	cl,F0_SRC_PRESENT	;Is a source needed?
	mov	cl,ch			;  (Want CX = +/- 1)
	jz	phase_proc_100		;  No, skip source set-up
	push	dx			;Save destination Y
	push	bp			;Mustn't trash frame pointer
	lea	bp,gl_src		;--> source data structure
	call	compute_y		;Process as needed
	pop	bp
	pop	dx			;Restore destination Y

phase_proc_100:
	push	bp			;Mustn't trash frame pointer
	mov	ax,dx			;Put destination Y in ax
	lea	bp,gl_dest 		;--> destination data structure
	call	compute_y
	pop	bp			;Restore frame pointer


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

	?CHKSTKPROC MAX_BLT_SIZE+20h	;See if room on stack			 ;See if room
	jnc	short cblt_alloc_stack_ok ;There was room
        jmp     short bitblt_exit_fail	;There was no room

cblt_alloc_stack_ok:
	add	sp,20h			;Take off the slop

	mov	di,sp
	mov	off_gl_blt_addr,di	;Save the address for later
	mov	ax,ss			;Set the segment for the BLT
	mov	es,ax

	mov	seg_gl_blt_addr,ax	;Save the address for later
	mov	ax,cs			;Set data seg to CS so we can access
	mov	ds,ax			;  code without overrides
	xor	cx,cx			;Clear out count register


	assumes ds,Code

	cCall	CBLT			;compile the BLT onto the stack

	mov	ax, LocalSelector	;Get a Selector
	cCall	PrestoChangoSelector, <ss, ax>
	mov	seg_gl_blt_addr, ax

	subttl	Invocation and Exit
	page

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;	If the debug flag has been set, save the size of the created BLT
;	so it may be returned to the caller.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

call_blt:

ifdef	DEBUG
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

ifdef	TEFTI
	timer_begin
endif
public	Here_we_are			; want a break point
Here_we_are:

	call	gl_blt_addr 		;Call the FAR process

ifdef	TEFTI
	timer_end
endif
	pop	bp

ifdef	DEBUG
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

public	bitblt_exit
bitblt_exit:
	mov	si, hTempMonoBits	;first free the temp mono bits if
	or	si, si			;there are any
	jz	BitBlt_good_exit
	mov	ds, cs:[_cstods]	;DS was temp mono bits, make it Data
	assumes ds, Data		;or GlobalUnlock will fail.
	cCall	GlobalUnlock, <si>
	cCall	GlobalFree, <si>

	mov	si, hTempMonoBmp
	or	si, si			;was there a temporary monochrome bmp?
	jz	BitBlt_good_exit	;no. exit now
	cCall	GlobalUnlock, <si>	;unlock the memory for the mono bmp
	cCall	GlobalFree, <si>	;free it

BitBlt_good_exit:
	mov	ax, 1			;Clear out error register (good exit)
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


bitblt_stack_ov:
cEnd


;-----------------------------------------------------------------------;
;	Subroutines.  These have been included with the aim of
;	segregating device dependent code from independent code,
;	while cleanly preserving the local variable frame.
;-----------------------------------------------------------------------;
        include 8514.BLT          ;8514 specific code
        include PDEVICE.BLT       ;PDevice processing
        include PATTERN.BLT       ;pattern preprocessing
	include COPYDEV.BLT	  ;copy_dev procedure
        include COMPUTEY.BLT      ;compute_y procedure

sEnd	Code
end
