        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	SCANLINE.ASM
;
;   This module contains the scanline sub-function of Output.
;
; Created: 22-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	do_scanlines
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;
; Restrictions:
;
;-----------------------------------------------------------------------;


;	This function will perform private stack checking.  In order for
;	private stack checking to occur, two symbols must be defined
;	prior to the inclusion of cmacros.inc.	?CHKSTK must be defined
;	if the cmacros are to perform stack checking on procedures with
;	local parameters.  ?CHKSTKPROC must be defined if private stack
;	checking will be used.
;
;	The actual macro body for ?CHKSTKPROC will be defined later.
;
;	Since the stack-checking code is designed to work quickly
;	as a near subroutine, and normally resides in segment Code,
;	a duplicate is included in this segment.  To reach this, the
;	the macro ?CHKSTKNAME is defined.




?CHKSTK = 1
?CHKSTKPROC	macro
		endm
?CHKSTKNAME	macro
	call	ScanlineSeg_check_stack
		endm


incLogical	= 1			;Include control for gdidefs.inc
incDrawMode	= 1			;Include control for gdidefs.inc
incOutput	= 1			;Include control for gdidefs.inc


	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
	.list

	??_out	scanline

;	Link time constants describing the size of the display

	externA ScreenSelector		;Selector to the screen
	externA SCREEN_W_BYTES		;Screen width in bytes
	externA	Y_SHIFT_COUNT


ifdef	EXCLUSION
	externFP exclude_far		;Exclude area from screen
	externFP unexclude_far		;Clear excluded area
endif



;	The cntrl_blk structure will contain the addresses of the
;	processor for the first and last byte, inner loop, and the
;	accelerator byte for skipping the operation.

cntrl_blk	struc
first_last_proc dw	?		;Adderss for first and last byte
inner_loop_proc dw	?		;Address for inner loop bytes
nop_flag	db	?		;D7 set if this is a nop
pattern 	db	?		;The pattern for the plane
cntrl_blk	ends

		errnz	<(size cntrl_blk) and 1>


;	The following equates are for the binary raster ops
;	which will be passed to this routine.  The values
;	shown are 0:15 since the rop is mapped upon entry
;	into this function.

ROP_DDX 	equ	0		;DDx
ROP_DPON	equ	1		;DPon
ROP_DPNA	equ	2		;DPna
ROP_PN		equ	3		;Pn
ROP_PDNA	equ	4		;PDna
ROP_DN		equ	5		;Dn
ROP_DPX 	equ	6		;DPx
ROP_DPAN	equ	7		;DPan
ROP_DPA 	equ	8		;DPa
ROP_DPXN	equ	9		;DPxn
ROP_D		equ	10		;D
ROP_DPNO	equ	11		;DPno
ROP_P		equ	12		;P
ROP_PDNO	equ	13		;PDno
ROP_DPO 	equ	14		;DPo
ROP_DDXN	equ	15		;DDxn


;	The following values will be set in another_tbl for
;	doing variuos accelerations on the raster op.
;
;	NO_OBJECT is the value used to indicate that the drawing
;	operation doesn't require a pen or brush.
;
;	NEG_PATTERN is used as a flag for special cased P and Pn
;	operations.  If this flag is set, then the pattern needs
;	to be negated before stored in EGA memory at current_brush.
;
;	ALT_NEG_PATTERN is used as a flag to indicated that the
;	pattern should be negated for word templates.

NEG_PATTERN	equ	10000000b
NO_OBJECT	equ	01000000b
ALT_NEG_PATTERN equ	00000001b


;	Define the flag to indicate cursor unexclusion required
;	before exiting.

CU_NONE		equ	00000000b
CU_EXCLUDE	equ	00000001b


;	The following are the values which will be stored
;	in some_flags.

INDEX_XPARENT	equ	00010000b	;Transparent operation


sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

sEnd	Data


createSeg _SCANLINE,ScanlineSeg,word,public,CODE
sBegin	ScanlineSeg
assumes cs,ScanlineSeg

	externNP ScanlineSeg_check_stack
	externW ScanlineSeg_interlace_adjust
page

;--------------------------Exported-Routine-----------------------------;
; do_scanlines
;
; Entry:
;	None
; Return:
;	AX = Non-zero to show success
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	comp_scan
;	get_fill_data
;	comp_interval
;	unexclude_far
;	various drawing functions set up by other code
; History:
;	Wed 04-Mar-1987 12:25:32 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


cProc	do_scanlines,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_dst_dev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lp_points		;--> to a set of points
	parmD	lp_phys_pen		;--> to physical pen
	parmD	lp_phys_brush		;--> to physical brush
	parmD	lp_draw_mode		;--> to a Drawing mode
	parmD	lp_clip_rect		;--> to a clipping rectange if <> 0

	localW	saved_y 		;Scan's Y coordinate
	localW	raster_op		;Rop2, mapped 0==>15
	localW	fetch_data		;Control parameters for interval calc
	localW	first_mask
	localW	last_mask
	localW	inner_loop_count
	localD	scan_start

	localV	processors,%(size cntrl_blk)
	localB	transparency_mask	;Mask of altered bits within a byte
	localB	some_flags		;Some flags
	localB	clean_up		;Flags for cleaning up at exit

cBegin

	cld				;We are forever doing this
	call	comp_scan		;Compute scanline, device data
	jc	do_scans_done		;comp_scan aborted for some reason
	call	get_fill_data		;Get data for filling
	jc	do_scans_done		;get_fill_data aborted for some reason
	mov	es,seg_scan_start	;This segment will stay unaltered
	assumes es,nothing

scan_next:
	call	comp_interval		;Get next interval to fill
	jc	do_scans_done		;No more points, exit

	cmp	processors.nop_flag,ROP_D
	je	do_scans_done 		;ROP is a nop, skip it

scan_do_first:
	push	di			;Have to save starting address
	mov	bl,processors.pattern
	mov	bh,bl
	mov	ax,first_mask		;If no first byte, skip it
	or	ax,ax
	jz	scan_do_inner
	mov	cx,1
	call	processors.first_last_proc

scan_do_inner:
	mov	cx,inner_loop_count	;If no inner loop, skip it
	jcxz	scan_do_last
	mov	al,transparency_mask
	mov	ah,al
	call	processors.inner_loop_proc

scan_do_last:
	mov	ax,last_mask		;If no inner loop, skip it
	or	ax,ax
	jz	scan_restore_pointer
	inc	cx
	call	processors.first_last_proc

scan_restore_pointer:
	pop	di
	jmp	scan_next

do_scans_done:
	mov	al,clean_up
	or	al,al
	jz	do_scans_exit		;No clean up needed

do_scans_clear_exclude:

ifdef	EXCLUSION			;If the registers were alterd, then
	call	unexclude_far		;  this must have been set up
endif

do_scans_exit:

cEnd
page
;	The following table is used to determine which drawing function
;	should be invoked for the inner loop bytes of the interval
;
;	The table is indexed as follows:
;
;		000 x rrrr
;
;		    |  |
;		    |	--------  raster op index
;		    |
;		     ----  transparent pixels if 1
;
;
;	The table can also be used to determine the drawing function
;	for the first and last bytes of the intervals by faking
;	transparent mode.
;
;	All entries are the offset from the start of the raster
;	op drawing routines, which allows the table to be stored
;	as bytes instaed of words, saving 32 bytes.


proc_addr_tbl	label byte

	db	opaque_ddx-rops_start	;Opaque entries are first
	db	opaque_dpon-rops_start
	db	opaque_dpna-rops_start
	db	opaque_pn-rops_start
	db	opaque_pdna-rops_start
	db	opaque_dn-rops_start
	db	opaque_dpx-rops_start
	db	opaque_dpan-rops_start
	db	opaque_dpa-rops_start
	db	opaque_dpxn-rops_start
	db	some_ret-rops_start
	db	opaque_dpno-rops_start
	db	opaque_p-rops_start
	db	opaque_pdno-rops_start
	db	opaque_dpo-rops_start
	db	opaque_ddxn-rops_start

	db	transp_ddx-rops_start		   ;Transparent entries are second
	db	transp_dpon-rops_start
	db	transp_dpna-rops_start
	db	transp_pn-rops_start
	db	transp_pdna-rops_start
	db	transp_dn-rops_start
	db	transp_dpx-rops_start
	db	transp_dpan-rops_start
	db	transp_dpa-rops_start
	db	transp_dpxn-rops_start
	db	some_ret-rops_start
	db	transp_dpno-rops_start
	db	transp_p-rops_start
	db	transp_pdno-rops_start
	db	transp_dpo-rops_start
	db	transp_ddxn-rops_start



;	The following table is used to map raster ops when the pattern
;	is all 1's or 0's.  All the rops can be mapped into one of
;	the following:
;
;		DDxn	DDx	Dn	D
;
;
;	based on the following results:
;
;
;	      Color	Result		      Color	Result
;
;	DDx	0	  0		DPa	0	  0
;		1	  0			1	 dest
;
;	DPon	0	~dest		DPxn	0	~dest
;		1	  0			1	 dest
;
;	DPna	0	 dest		D	0	 dest
;		1	  0			1	 dest
;
;	Pn	0	  1		DPno	0	  1
;		1	  0			1	 dest
;
;	PDna	0	  0		P	0	  0
;		1	~dest			1	  1
;
;	Dn	0	~dest		PDno	0	~dest
;		1	~dest			1	  1
;
;	DPx	0	 dest		DPo	0	 dest
;		1	~dest			1	  1
;
;	DPan	0	  1		DDxn	0	  1
;		1	~dest			1	  1
;
;
;
;	The table is indexed as follows:
;
;		000 rrrr c
;
;		     |	 |
;		     |	 -----	color 0 = 1s, 1 = 0's
;		     |
;		     ---------	raster op index

map_1s_0s	label	byte
	db	ROP_DDX 		;DDx	with all 0s
	db	ROP_DDX 		;DDx	with all 1s
	db	ROP_DN			;DPon	with all 0s
	db	ROP_DDX 		;DPon	with all 1s
	db	ROP_D			;DPna	with all 0s
	db	ROP_DDX 		;DPna	with all 1s
	db	ROP_DDXN		;Pn	with all 0s
	db	ROP_DDX 		;Pn	with all 1s
	db	ROP_DDX 		;PDna	with all 0s
	db	ROP_DN			;PDna	with all 1s
	db	ROP_DN			;Dn	with all 0s
	db	ROP_DN			;Dn	with all 1s
	db	ROP_D			;DPx	with all 0s
	db	ROP_DN			;DPx	with all 1s
	db	ROP_DDXN		;DPan	with all 0s
	db	ROP_DN			;DPan	with all 1s
	db	ROP_DDX 		;DPa	with all 0s
	db	ROP_D			;DPa	with all 1s
	db	ROP_DN			;DPxn	with all 0s
	db	ROP_D			;DPxn	with all 1s
	db	ROP_D			;D	with all 0s
	db	ROP_D			;D	with all 1s
	db	ROP_DDXN		;DPno	with all 0s
	db	ROP_D			;DPno	with all 1s
	db	ROP_DDX 		;P	with all 0s
	db	ROP_DDXN		;P	with all 1s
	db	ROP_DN			;PDno	with all 0s
	db	ROP_DDXN		;PDno	with all 1s
	db	ROP_D			;DPo	with all 0s
	db	ROP_DDXN		;DPo	with all 1s
	db	ROP_DDXN		;DDxn	with all 0s
	db	ROP_DDXN		;DDxn	with all 1s




;	another_tbl contains various flags for the different raster ops

another_tbl	label	byte

	db	NO_OBJECT	 					;DDx
	db	ALT_NEG_PATTERN 					;DPon
	db	0							;DPna
	db	NEG_PATTERN+ALT_NEG_PATTERN				;Pn
	db	0							;PDna
	db	NO_OBJECT						;Dn
	db	0							;DPx
	db	0							;DPan
	db	0							;DPa
	db	ALT_NEG_PATTERN						;DPxn
	db	NO_OBJECT	 					;D
	db	ALT_NEG_PATTERN						;DPno
	db	0							;P
	db	ALT_NEG_PATTERN 					;PDno
	db	0							;DPo
	db	NO_OBJECT+ALT_NEG_PATTERN 				;DDxn


page
;--------------------------Private-Routine------------------------------;
; comp_scan
;
;   The starting Y coordinate will be computed, and the device specific
;   parameters fetched and saved.  If this is the display, the area of
;   the screen where the drawing will occur will be excluded.
;
;   The Y of the scan will be saved incase it is required for a brush.
;   The points count will be decremented to account for the point containing
;   the starting Y.
;
; Entry:
;	DS = Data
; Returns:
;	'C' clear if successful
; Error Returns:
;	'C' set if to abort
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,FLAGS
; Calls:
;	exclude_far
; History:
;	Tue 18-Aug-1987 14:50:37 -by-  **** ***** [*****]
;	Added test of the disabled flag.
;
;	Wed 04-Mar-1987 17:19:12 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


comp_scan	proc near

	mov	clean_up,CU_NONE	;Show no clean-up needed
	stc				;Assume no points ('C' set on exit)
	mov	di,count		;If less than two points,
	dec	di			;  exit now.
	jle	comp_scan_exit_vect	;No points, exit now
	les	bx,lp_points		;--> first point
	assumes es,nothing

	lea	ax,[bx][size PTTYPE]	;Get offset of first interval
	mov	off_lp_points,ax	;  and save it
	mov	ax,es:[bx].ycoord	;Get scan's Y coordinate
	mov	saved_y,ax		;Save for pattern fetch code
	mov	dl,enabled_flag 	;Load this before trashing DS
	lds	si,lp_dst_dev		;--> output device
	assumes ds,nothing

	mov	cx,[si].bmType		;Is this a bitmap?
	jcxz	comp_scan_for_bitmap	;  Yes, go process for a bitmap
	or	dl,dl			;If the display has been disabled,
	stc				;  abort the operation
	jz	comp_scan_exit		;Disabled, abort


;	While we still have the points pointer sitting around,
;	exclude the area of the screen where we'll be drawing.
;
;	The intervals are sorted from left to right, so the
;	exclusion area will be the left X of the first interval
;	and the right X of the last interval.


ifdef	EXCLUSION
	xchg	dx,ax				;Set top into DX
	mov	cx,es:[bx][size PTTYPE].xcoord	;Set left
	shiftl	di,2
	mov	si,es:[bx][di].ycoord	;Set right
	errnz	<(SIZE PTTYPE) - 4>	;Must be four bytes long
	mov	di,dx			;Set bottom of exclude rectangle
	call	exclude_far 		;Exclude the scan from the screen
	mov	clean_up,CU_EXCLUDE
endif

	mov	ax,saved_y		;Compute destination scan's address
	mov	bx,ax
	and	bx,00000011b
	shl	bx,1
	mov	cl,Y_SHIFT_COUNT
	shr	ax,cl
	mov	cx,SCREEN_W_BYTES
	mul	cx
	add	ax,ScanlineSeg_interlace_adjust[bx]
	mov	off_scan_start,ax	;Save address of scan
	mov	seg_scan_start,ScreenSelector
	jmp	short comp_scan_save	;Exit with success


comp_scan_exit_vect:
	jmp	short comp_scan_exit	;I hate needing these stupid vectors



;	The device is a memory bitmap.	 If it is a huge bitmap,
;	special processing must be performed to compute the Y address.
;
;	currently:	AX     =  Y start
;			DS:SI --> device
;

comp_scan_for_bitmap:
	xor	dx,dx			;Set segment bias to 0
	mov	cx,[si].bmSegmentIndex	;Is this a huge bitmap?
	jcxz	cs_for_bitmap_40	;  No



;	This is a huge bitmap.	Compute which segment the Y coordinate
;	is in.	Assuming that no huge bitmap will be bigger than two
;	or three segments, iteratively computing the value would be
;	faster than a divide, especially if Y is in the first segment
;	(which would always be the case for a huge color bitmap that
;	didn't have planes >64K).


	mov	bx,[si].bmScanSegment	;Get # scans per segment

cs_for_bitmap_30:
	add	dx,cx			;Show in next segment
	sub	ax,bx			;See if in this segment
	jnc	cs_for_bitmap_30	;Not in current segment, try next
	add	ax,bx			;Restore correct Y
	sub	dx,cx			;Show correct segment


;	This is a memory DC.
;
;	Currently:
;		AX     =  Y coordinate
;		DX     =  Segment bias for huge bitmaps
;		DS:SI --> Physical device

cs_for_bitmap_40:
	mov	bx,[si].bmWidthBytes	;Get index to next plane

cs_for_bitmap_60:
	add	dx,wptr [si].bmBits[2]	;Compute segment of the bits
	mov	seg_scan_start,dx	;Save segment of the scan
	mul	bx			;Compute start of scan
	add	ax,wptr [si].bmBits[0]	;Add in any offset defined here

	mov	off_scan_start,ax	;Save address of scan

comp_scan_save:
	clc				;Show successful

comp_scan_exit:
	ret


comp_scan	endp
page
;--------------------------Private-Routine------------------------------;
; get_fill_data
;
;   Get All Data Required For Filling The Scan
;
;   The raster operation, pen or pattern, and transparent bits
;   are fetched and saved.  The addresses of the functions which
;   will do the actual drawing will be set.  Device initialization
;   will be performed as needed.  The inner loop base size (byte
;   or word) will be determined.
;
;   Brushes can have pixels in them which are the same as the
;   background color, and a mask must be computed for the bits
;   which are not background color.
;
;   Pens are solid objects, and therefore the only transparency
;   check for them is if the pen color is the same as the
;   background color.
;
; Entry:
; Returns:
;	'C' clear if successful
; Error Returns:
;	'C' set if to abort
; Registers Preserved:
;	ES,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,FLAGS
; Calls:
;	set_procs
; History:
;	Tue 03-Mar-1987 18:00:47 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


get_fill_data	proc near


;	Get the information out of the Raster Op Block on the raster op
;	to use, the background mode, and the background color.

;	Retrieve the binary raster operation (Rop2) and save it.
;	It must be mapped from 1:16 into 0:15.


	lds	si,lp_draw_mode 	;--> DrawMode Block
	assumes ds,nothing

	mov	transparency_mask,0FFh	;Show no bits to be altered
	mov	bx,[si].Rop2		;Get the drawing mode (raster op)
	dec	bx			;  make it zero based
	and	bx,000FH		;  (play it safe)
	cmp	bl,ROP_D		;Is the rop "D"?
	je	get_fill_do_nothing	;  Yes, nop
	mov	raster_op,bx		;  and save a copy


;	Fetch the background mode and combine it with the rop_accelerators.

	cmp	bptr [si].bkMode,OPAQUE ;Set 'C' if transparent
	sbb	bl,bl			;BL = FFh if transparent, 00 otherwise
	and	bl,INDEX_XPARENT shl 1
	errnz	TRANSPARENT-00000001b	;Must be these bits
	errnz	OPAQUE-00000010b
	errnz	INDEX_XPARENT-00010000b

	stc
	rcr	bl,1			;Move mono flag into some_flags
	mov	some_flags,bl		;Save the dispatch index for later




;	Question:  If the rop is one of the rops which doesn't
;	require a pattern or pen, and the mode is transparent,
;	is a mask of altered/unaltered bits still created?
;
;	I have to answer this yes since that is how it is currently
;	done.  Also, even though DDxn and DDx do not require a
;	pen or brush, if the ben or brush is null (hollow), then
;	the operation should be aborted.


	mov	ah,[si].bkColor.SPECIAL ;May need the background color


;	If lp_phys_brush is null, then the pen is to be used for
;	the drawing object.  If lp_phys_brush isn't null, then
;	the brush is to be used.


	mov	di,off_lp_phys_brush	;See if a brush was given
	mov	si,seg_lp_phys_brush
	mov	cx,si
	or	cx,di
	jnz	get_fill_brush		;Must use the brush



;	A pen is to be used.  If the pen type is NULL, then we're
;	all done.  Otherwise, load the pen color and make it look
;	as if we were processing a solid brush (pens are solid
;	colors, so we can do this).


get_fill_pen:
	lds	di,lp_phys_pen		;--> physical pen
	assumes ds,nothing

	cmp	[di].oem_pen_style,LS_NOLINE
	je	get_fill_do_nothing	;Null pen, exit

	mov	ch,bptr [di].oem_pen_pcol.SPECIAL

	mov	al,ch			;Save the brush accelerator
	shiftl	ch,4			;Map the mono bit into FF or 00
	sbb	ch,ch
	or	al,SOLID_BRUSH or ONES_OR_ZEROS ;Fake a solid brush
	jmp	short preprocess_dispatch


;	Either a null object was given, or there are no bits which
;	will be altered by the current color and raster op combination.

get_fill_do_nothing:
	stc				;Return 'C' to terminate scanline
	ret



;	A brush will be used for the filling.  If it is null, then
;	exit now since nothing will show.  Otherwise, process the
;	colors of the brush against the background color.
;
;	Currently:   SI:DI --> the physical brush
;			AH  =  background color
;			BH  =  0


get_fill_brush:
	mov	ds,si			;DS:DI --> brush
	assumes ds,nothing

	xchg	ax,si			;Must save AL
	mov	ax,[di].oem_brush_style ;Get brush style
	cmp	ax,MaxBrushStyle	;Legal?
	ja	get_fill_do_nothing	;  Outside range, return error
	cmp	ax,BS_HOLLOW		;Hollow?
	je	get_fill_do_nothing	;   Yes, return now.
	xchg	ax,si

	mov	al,[di].oem_brush_accel ;Get the solid brush accellerator
	mov	bl,bptr saved_y 	;The current Y scan dictates which
	and	bl,00000111b		;  scan of brush to use
;	xor	bh,bh
	mov	ch,[di].oem_brush_mono[bx] ;Get the bits for the brush



;	Dispatch based on opaque/transparent for preprocessing the
;	pattern.
;
;	The following will be passed to each routine:
;
;		AH = background color
;		AL = brush accelerator flags
;		CH = pattern
;
;	Since creating the transparent bits mask destroys the color
;	for a monochrome device, the color will be placed into DL
;	if this is a mono device.

preprocess_dispatch:
	mov	dl,ch			;save color in dl

	mov	bl,some_flags
	test	bl,INDEX_XPARENT
	jz	test_if_obj_required


;	Process the brush for transparent mode

	shiftl	ah,4			;Map the mono bit into FF or 00
	sbb	ah,ah
	xor	ch,ah			;Set like bits to 0


;	The transparency mask has been computed and is in CH.
;	'Z' is set if no pixels will show.
;
;	If all pixels show (transparency mask = FF), then the
;	operation can be treated the same as opaque
;
;	Currently:
;		AL = brush accelerator flags
;		CH = transparency mask
;		'Z' set if no pixels show


	jz	get_fill_do_nothing	;No pixels will be altered
	mov	transparency_mask,ch	;Save mask of bits to alter
	inc	ch			;'Z' if mask is FFh
	jnz	test_if_obj_required
	and	some_flags,not INDEX_XPARENT
	errnz	<HIGH INDEX_XPARENT>



;	Four of the Rops do not require an object (D, Dn, DDx, DDxn).
;	If the current rop is one of these, then we want to show
;	that the pattern is solid so that special case code can
;	kick in regardless of what pattern was given.


test_if_obj_required:
	mov	bx,raster_op
	mov	ah,another_tbl[bx]	;These flags will be useful
	test	ah,NO_OBJECT
	jz	dispatch_pattern	;Not one of the rops
	xor	dl,dl			;Show brush as black
	mov	al,SOLID_BRUSH+ONES_OR_ZEROS


;	Currently:
;
;		AL = brush accelerator flags
;		AH = another_tbl[bx]
;		BX = raster_op
;		DL = pattern

dispatch_pattern:
	ror	ah,1			;Set AH = 0FFh if to invert the pattern
	sbb	ah,ah

	mov	al,80h			;D7 = 1 to show nop
	lea	si,processors		;--> first control element

	mov	ch,dl			;store pattern for call
	call	set_procs
	mov	fetch_data,0F04h	;Set for word alignment
	shl	al,1			;Set 'C' if operation can be skipped
	ret

get_fill_data	endp
page
;--------------------------Private-Routine------------------------------;
; set_procs
;
;   The processor addresses for performing the raster operation
;   with the current pattern and transparent pixels (if any)
;   are set in the given structure
;
; Entry:
;	CH     =  pattern
;	AL:D7  =  NOP flag
;	AH     =  0FF if to invert pattern, 00 otherwise
;	SS:SI --> proc_blk structure where info is to be stored
; Returns:
;	AL     =  0 if function isn't ROP_D, else unaltered
;	AH     =  0FF if to invert pattern, 00 otherwise
; Error Returns:
;	None
; Registers Preserved:
;	CL,DX,DS,ES
; Registers Destroyed:
;	CH,BX,DI,FLAGS
; Calls:
;	None
; History:
;	Sun 08-Mar-1987 17:09:27 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


set_procs	proc	near

	xor	ch,ah			;Invert pattern if needed
	mov	ss:[si].pattern,ch
	xor	ch,ah
	mov	bx,raster_op		;Need raster op index
	inc	ch			;Colors of all 1s or 0s can be
	cmp	ch,1			;  special cased
	ja	set_procs_no_map	;Not 00 or FF
	sub	ch,1			;Set 'C' = color (1 or 0)
	rcl	bx,1
	mov	bl,map_1s_0s[bx]	;bl = mapped rop

set_procs_no_map:
	mov	ss:[si].nop_flag,bl	;Show ROP_D if it is
	cmp	bl,ROP_D		;If rop is ROP_D, done
	je	set_procs_exit
	mov	ch,some_flags		;Combine the transparent flag into
	and	ch,INDEX_XPARENT	;  the rop for indexing into the
	or	bl,ch			;  various tables
	xchg	ax,di			;Must save AH
	xor	ax,ax
	mov	al,proc_addr_tbl[bx]	;Compute rops address and save it
	add	ax,ScanlineSegOFFSET rops_start
	mov	ss:[si].inner_loop_proc,ax
	or	bl,INDEX_XPARENT	;Fake transparency for first/last byte
	mov	bl,proc_addr_tbl[bx]	;Save inner loop processer address
	add	bx,ScanlineSegOFFSET rops_start
	mov	ss:[si].first_last_proc,bx
	xchg	ax,di
	xor	al,al			;Clear D7 to show operation occured

set_procs_exit:
	ret

set_procs	endp
page
;	The following two bit mask tables are used for fetching
;	the first and last byte used-bits bitmask.
;
;	So I lied.  I no longer use the tables, I create the
;	masks on the fly.  I'll leave them here so you can
;	get the idea of what I'm trying to do later in the
;	code.

bit_mask_tbl_left label word
					;Masks for leftmost byte
;	dw	1111111111111111B
;	dw	0111111111111111b
;	dw	0011111111111111b
;	dw	0001111111111111b
;	dw	0000111111111111b
;	dw	0000011111111111b
;	dw	0000001111111111b
;	dw	0000000111111111b
;	dw	0000000011111111b
;	dw	0000000001111111b
;	dw	0000000000111111b
;	dw	0000000000011111b
;	dw	0000000000001111b
;	dw	0000000000000111b
;	dw	0000000000000011b
;	dw	0000000000000001b


bit_mask_tbl_right label word

;	dw	1000000000000000B	;Masks for rightmost byte
;	dw	1100000000000000B
;	dw	1110000000000000B
;	dw	1111000000000000B
;	dw	1111100000000000B
;	dw	1111110000000000B
;	dw	1111111000000000B
;	dw	1111111100000000B
;	dw	1111111110000000B
;	dw	1111111111000000B
;	dw	1111111111100000B
;	dw	1111111111110000B
;	dw	1111111111111000B
;	dw	1111111111111100B
;	dw	1111111111111110B
;	dw	1111111111111111B
page
;--------------------------Private-Routine------------------------------;
; comp_interval
;
;   The next interval to be filled will be computed.
;
;   A first mask and a last mask will be calculated, and possibly
;   combined into the inner loop count.  If no first byte exists,
;   the start address will be incremented by the size (byte/word)
;   to adjust for it.
;
; Entry:
;	None
; Returns:
;	'C' clear
;	DS:DI --> first point
; Error Returns:
;	'C' if no more intervals
; Registers Preserved:
;	ES,BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,FLAGS
; Calls:
;	None
; History:
;	Tue 10-Mar-1987 17:16:52 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


comp_interval_no_more:
	stc
	ret


comp_interval	proc near
	lds	si,lp_points		;--> the scanline pairs
	assumes ds,nothing

comp_interval_null:
	dec	count			;Any more pairs?
	jle	comp_interval_no_more	;  No, exit
	lodsw				;Get start & end coordinates
	mov	dx,ax
	lodsw
	errnz	xcoord
	errnz	ycoord-xcoord-2

	mov	bx,ax			;Compute extent of interval
	sub	bx,dx
	jle	comp_interval_null	;Null interval, skip it
	mov	off_lp_points,si	;Save new points pointer
	dec	bx			;Make interval inclusive



;	Compute the starting byte/word address of this interval.
;	fetch_data contains the shift count required for either
;	bytes or words.

	mov	cx,fetch_data		;Get mask/shift counts
	mov	di,dx			;Don't destroy starting x
	shr	di,cl			;
	mov	ds,seg_scan_start	;Set segment of first byte/word
	assumes ds,nothing




;	We now have to determine how many bits will be affected,
;	and how they are aligned within the bytes.
;
;	(left_x MOD word_size) will give us the starting pixel
;	within the left byte/word.  Adding the inclusive extent
;	of the interval to left_x MOD word_size) and taking the
;	result MOD word_size will give us the last pixel affected
;	in the last byte/word.	These pixel indexes (0:7 for bytes,
;	0:15 for words), can be used to create the first and last
;	altered bits mask.
;
;
;	To compute the number of bytes/words in the inner loop,
;	use the second calculation above
;
;		(left_x MOD word_size) + inclusive_extent
;
;	and divide it by the word size (8/16).	This gives you
;	the following:
;
;
;	    1)	If the result is 0, then only one destination
;		byte/word is being altered.  In this case, the
;		start & ending masks should be ANDed together,
;		the innerloop count set to zero, and last_mask
;		set to to all 0's (don't alter any bits).
;
;			|      x x x x x|		|
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 3,  extent-1 = 4
;			3+7 DIV 8 = 0, only altering one byte
;
;
;
;	    2)	If the result is 1, then only two bytes/words
;		will be altered.  In this case, the start and
;		ending masks are valid, and all that needs to
;		be done is set the innerloop count to 0.
;
;			|  x x x x x x x|x x x x x x x|
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 1,  extent-1 = 14
;			3+14 DIV 8 = 1.  There is a first and last
;			byte but no innerloop count
;
;
;
;	    3)	If the result is > 1, then some number of entire
;		bytes/words will be altered by the innerloop.  In
;		this case the number of innerloop bytes/words will
;		be the result - 1.
;
;			|	       x|x x x x x x x x|x
;			|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;			 0 1 2 3 4 5 6 7
;
;			start MOD 8 = 7,  extent-1 = 9
;			7+9  DIV 8 = 2.  There is a first and last
;			byte and an innerloop count of 1 (result - 1)




;	Compute the starting bit position on the left and the ending
;	bit position on the right


	and	dl,ch			;Compute bit index for left side
	xor	dh,dh
	add	bx,dx			;Compute bit index for right side
	mov	si,bx			;(save for inner loop count)
	and	bl,ch


;	Turn the bit index within the word (or byte) into a mask.
;	We are basically performing a table lookup into the two
;	tables bit_mask_tbl_right and bit_mask_tbl_left.  However,
;	by doing either arithmetic shifts or logical shifts, we
;	can create the masks and save a lot of table space.


	mov	ch,cl			;Save  byte/word shift count
	mov	cl,dl			;Compute left side altered bits mask
	mov	ax,0FFFFh
	mov	dx,ax			;Need this here later
	shr	ax,cl			;Compute right side altered bits mask
	mov	cl,bl
	mov	bx,8000h
	sar	bx,cl
	mov	cl,ch			;Restore byte/word shift count
	cmp	cl,3			;Must handle word/byte intervals
	jne	comp_word_interval	;  seperately from this point



;	This is for byte intervals.  The left side mask was not
;	created correctly.  It is off by one entire byte, or 8
;	bit positions.	The right side mask is also incorrect.
;	It must be placed into BL so the 8-bit partial byte code
;	can find it.

	add	di,off_scan_start	;DS:DI --> first byte
	mov	al,ah			;Set correct first byte mask
	xor	ah,ah
	xchg	bl,bh			;Place these in the correct order
	shr	si,cl			;Compute inner byte count
	jnz	comp_byte_interval_10	;loop count + 1 > 0, check it out


;	Only one byte will be affected.  Combine the first and
;	last byte masks, and set the loop count to 0.


	and	al,bl			;AL = left, BL = right, AH = 0
	xor	bx,bx			;Want the entire mask to be 0
	inc	si			;Fall through to set 0

comp_byte_interval_10:
	dec	si			;Dec inner loop count (might become 0)


;	If all pixels in the first byte are altered, combine the
;	first byte into the inner loop and clear the first byte
;	mask.  Ditto for the last byte

	cmp	al,dl			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	al,dl			;Set 'C' if not all pixels 1
	sbb	al,dl			;If no 'C', sub -1 (add 1), else sub 0

	cmp	bl,dl			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	bl,dl			;Set 'C' if not all pixels 1
	sbb	bl,dl			;If no 'C', sub -1 (add 1), else sub 0


;	Save the first and last byte masks and the loop count.
;	And the masks with the transparency mask now to save
;	a little time in the loop for multiple passes.
;
;	If the first byte/word mask is not zero before ANDing
;	in the transparency mask, and becomes zero after ANDing
;	in the transparency mask, then the starting address needs
;	to be updated by the size (byte/word)


comp_interval_save:
	mov	inner_loop_count,si	;Save the inner loop count
	dec	cx			;Map 3==>1, 4==>2 for increment size
	dec	cx
	neg	dx			;Set DX = 0001 (was FFFF)
	cmp	ax,dx			;'C' only if mask is all zeros
	sbb	dl,dl			;DX = 00FF if it was all zeros
	not	dl			;DX = 0000 if it was all zeros
	and	dx,cx			;Now place size or 0 into DX

	mov	cl,transparency_mask
	mov	ch,cl
	and	ax,cx
	and	bx,cx
	mov	first_mask,ax
	mov	last_mask,bx


;	If we just created a first byte/word mask of all zero's, then
;	we have to increment the starting address appropriately.


	cmp	ax,1			;Set 'C' if mask is zeros
	sbb	ax,ax			;AX = FF is mask was zeros
	and	ax,dx			;Only leave increment if mask is zeros
	add	di,ax			;Offset start if needed
	ret




;	Everything said for bytes will just be repeated for
;	words.


comp_word_interval:
	shl	di,1			;Turn word offset into byte offset
	add	di,off_scan_start	;DS:DI --> first word
	shr	si,cl			;Compute inner byte count
	jnz	comp_word_interval_10	;loop count + 1 > 0, check it out


;	Only one word will be affected.  Combine the first and
;	last word masks, and set the loop count to 0.

	and	ax,bx
	xor	bx,bx
	inc	si			;Fall through to set to 0

comp_word_interval_10:
	dec	si			;Dec innerloop count (might become 0)


;	Words fetched from memory come out with the high order and low
;	order bytes swapped.  This will be handled by swapping the two
;	halves of each mask.

	xchg	al,ah
	xchg	bl,bh


;	If all pixels in the first word are altered, combine the
;	first word into the inner loop and clear the first word
;	mask.  Ditto for the last word

	cmp	ax,dx			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	ax,dx			;Set 'C' if not all pixels 1
	sbb	ax,dx			;If no 'C', sub -1 (add 1), else sub 0

	cmp	bx,dx			;Set 'C' if not all pixels 1
	sbb	si,dx			;If no 'C', sub -1 (add 1), else sub 0
	cmp	bx,dx			;Set 'C' if not all pixels 1
	sbb	bx,dx			;If no 'C', sub -1 (add 1), else sub 0
	jmp	comp_interval_save	;Finish here

comp_interval	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpna
; transp_dpna
;
;   The following is the code for the DPna raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPna, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	dest
; //	 1	0	dest
; //	 1	1	 0
; //
; // Wherever both the transparency mask and the pattern are a 1,
; // the destination should be set to 0.
; //
; // This can be implemented by ANDing the pattern with the transparency
; // mask to get a 1 for each pixel which is to change.  This result is
; // then inverted to give a 0 wherever each pixel of the destination
; // should be set to 0.  This is then ANDed to the destination for the
; // desired result
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing

rops_start	proc	near		;Start of all the rops

opaque_dpna	proc	near
transp_dpna	proc	near

	and	ax,bx			;AX = 1 wherever dest will be 0

;	not	ax			;AX = 0 wherever dest will be 0
;
;transp_dpna_next:
;	and	wptr [di],ax		;Set pixels to 0 as needed
;	inc	di			;--> next destination word
;	inc	di
;	loop	transp_dpna_next	;Until all have been processed
;	ret

;	jmp	transp_ddx
	errn$	transp_ddx		;Finish here

transp_dpna	endp
opaque_dpna	endp
page
;--------------------------Private-Routine------------------------------;
; transp_ddx
;
;   The following is the code for the DDx raster operation.
;   It will be used for memory bitmaps.
;
;   It will only be used when transparency is involved.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DDx, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 0
; //	 1	0	dest
; //	 1	1	 0
; //
; // Wherever the transparency mask is a 1, the destination should be
; // set to 0.
; //
; // This can be implemented by inverting the transparency mask to give
; // a 0 wherever each pixel of the destination should be set to 0.
; // This is then ANDed to the destination for the desired result
; //
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


transp_ddx	proc	near

	not	ax			;AX = 0 wherever dest will be 0

transp_ddx_next:
	and	wptr [di],ax		;Set pixels to 0 as needed
	inc	di			;--> next destination word
	inc	di
	loop	transp_ddx_next 	;Until all have been processed
	ret

transp_ddx	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpa
; transp_dpa
;
;   The following is the code for the DPa raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPa, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 0
; //	 1	0	dest
; //	 1	1	dest
; //
; // Wherever the transparency mask is 1 and the pattern is 0,
; // the destination should be set to 0.
; //
; // This can be implemented by inverting the transparency mask then
; // ORing it with the pattern.  This will give a 0 wherever the
; // destination should be set to 0.  This is then ANDed to the
; // destination for the desired result
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpa	proc	near
transp_dpa	proc	near

	not	ax			;AX = 1 where pixel doesn't change
	or	ax,bx			;AX = 0 wherever the pixel becomes 0

transp_dpa_next:
	and	wptr [di],ax		;Set pixels to 0 as needed
	inc	di			;--> next destination word
	inc	di
	loop	transp_dpa_next 	;Until all have been processed
	ret

transp_dpa	endp
opaque_dpa	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpxn
; transp_dpxn
;
;   The following is the code for the DPxn raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPxn, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1      ~dest
; //	 1	0	dest
; //	 1	1	dest
; //
; // Wherever the transparency mask is 1 and the pattern is 0,
; // the destination should be inverted.
; //
; // The pattern will be inverted upon entry (actually before entry).
; // This will give the new truth table of:
; //
; //	Pat    Mask    Result
; //
; //	 1	0	dest
; //	 1	1      ~dest
; //	 0	0	dest
; //	 0	1	dest
; //
; // The new pattern and the transparency mask will be ANDed
; // together.	This will give a 1 wherever the destination
; // should be inverted.  This is then XORed to the destination.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpxn	proc	near
transp_dpxn	proc	near

;	not	bx			;Inverted at setup time

;	and	ax,bx			;AX = 1 wherever dest is inverted
;
;transp_dpxn_next:
;	xor	wptr [di],ax		;Invert pixels as needed
;	inc	di			;--> next destination word
;	inc	di
;	loop	transp_dpxn_next	;Until all have been processed
;	ret

;	jmp	transp_dpx		;Finish here
	errn$	opaque_dpx
	errn$	transp_dpx

transp_dpxn	endp
opaque_dpxn	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpx
; transp_dpx
;
;   The following is the code for the DPx raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPx, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	dest
; //	 1	0	dest
; //	 1	1      ~dest
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be inverted.
; //
; // This can be implemented by ANDing the pattern with the
; // transparency mask.  This will give a 1 wherever the
; // destination should be inverted.  This is then XORed to
; // the destination for the desired result
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpx	proc	near
transp_dpx	proc	near

	and	ax,bx			;AX = 1 wherever dest is inverted

;transp_dpx_next:
;	xor	wptr [di],ax		;Invert pixels as needed
;	inc	di			;--> next destination word
;	inc	di
;	loop	transp_dpx_next 	;Until all have been processed
;	ret

;	jmp	transp_dn
;	errn$	transp_dn		;Finish here
;	errn$	opaque_dn

transp_dpx	endp
opaque_dpx	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dn
; transp_dn
;
;   The following is the code for the Dn raster operation.
;   It will be used for memory bitmaps.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For Dn, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1      ~dest
; //	 1	0	dest
; //	 1	1      ~dest
; //
; // Wherever the transparency mask is 1, the destination
; // should be inverted.
; //
; // This can be implemented by XORing the transparency mask
; // with the destination.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dn	proc	near
transp_dn	proc	near

transp_dn_next:
	xor	wptr [di],ax		;Invert pixels as needed
	inc	di			;--> next destination word
	inc	di
	loop	transp_dn_next		;Until all have been processed
	ret

transp_dn	endp
opaque_dn	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpno
; transp_dpno
;
;   The following is the code for the DPno raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPno, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 1
; //	 1	0	dest
; //	 1	1	dest
; //
; // Wherever the transparency mask is 1 and the pattern is 0,
; // the destination should be set to 1.
; //
; // The pattern will be inverted upon entry (actually before entry).
; // This will give the new truth table of:
; //
; //	Pat    Mask    Result
; //
; //	 1	0	dest
; //	 1	1	 1
; //	 0	0	dest
; //	 0	1	dest
; //
; // The new pattern and the transparency mask will be ANDed
; // together.	This will give a 1 wherever the destination
; // should be set to 1.  This is then ORed to the destination.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpno	proc	near
transp_dpno	proc	near

;	not	bx			;Inverted at setup time
;
;	and	ax,bx			;AX = 1 wherever the pixel becomes 1
;
;transp_dpno_next:
;	or	wptr [di],ax		;Set pixels to 1 as needed
;	inc	di			;--> next destination word
;	inc	di
;	loop	transp_dpno_next	;Until all have been processed
;	ret

;	jmp	transp_dpo
	errn$	opaque_dpo		;Finish here
	errn$	transp_dpo

transp_dpno	endp
opaque_dpno	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpo
; transp_dpo
;
;   The following is the code for the DPo raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPo, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	dest
; //	 1	0	dest
; //	 1	1	 1
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be set to 1.
; //
; // This can be implemented by ANDing the pattern with the transparency
; // mask.  This will give a 1 wherever the destination should be set
; // to 1.  This is then ORed with the destination for the desired result.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpo	proc	near
transp_dpo	proc	near

	and	ax,bx			;AX = 1 wherever pixel becomes a 1

;transp_dpo_next:
;	or	wptr [di],ax		;Set pixels to 1 as needed
;	inc	di			;--> next destination word
;	inc	di
;	loop	transp_dpo_next 	;Until all have been processed
;	ret

;	jmp	transp_ddxn
	errn$	transp_ddxn		;Finish here

transp_dpo	endp
opaque_dpo	endp
page
;--------------------------Private-Routine------------------------------;
; transp_ddxn
;
;   The following is the code for the DDxn raster operation.
;   It will be used for memory bitmaps.
;
;   It will only be used when transparency is involved.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DDxn, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 1
; //	 1	0	dest
; //	 1	1	 1
; //
; // Wherever the transparency mask is a 1, the destination should be
; // set to 1.
; //
; // This can be implemented by ORing the transparency mask with the
; // destination.
; //
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


transp_ddxn	proc	near

transp_ddxn_next:
	or	wptr [di],ax		;Set pixels to 1 as needed
	inc	di			;--> next destination word
	inc	di
	loop	transp_ddxn_next	;Until all have been processed
	ret

transp_ddxn	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpon
; transp_dpon
;
;   The following is the code for the DPon raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPon, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1      ~dest
; //	 1	0	dest
; //	 1	1	 0
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be set to 0.  Wherever the transparency
; // mask is a 1 and the pattern a 0, the destination should be
; // inverted.
; //
; // The pattern will be inverted upon entry (actually before entry).
; // This will give the new truth table of:
; //
; //	Pat    Mask    Result
; //
; //	 1	0	dest
; //	 1	1      ~dest
; //	 0	0	dest
; //	 0	1	 0
; //
; //
; // Two working patterns will then be generated.  The first will be
; // new pattern ANDed with the transparency mask.  This will give a
; // 1 wherever the destination should be inverted.  It will be XORed
; // with the destination.
; //
; // The second working pattern will be the new pattern ORed with the
; // inverse of the transparency mask.	This will give a 0 wherever the
; // destination should become 0.  It will be ANDed with the destination.
; //
; // Since the pattern register will be destroyed, it must be reloaded
; // before exit.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpon	proc	near
transp_dpon	proc	near

;	not	bx			;Inverted at setup time
;
;	mov	dx,bx			;Need to use the work register
;	and	bx,ax			;XOR mask
;	not	ax
;	or	dx,ax			;AND mask
;
;transp_dpon_next:
;	mov	ax,wptr [di]		;Quicker to have dest into a register
;	xor	ax,bx			;Invert bits as needed
;	and	ax,dx			;Set bits to 0 as needed
;	stosw				;Store and update destination ptr
;	loop	transp_dpon_next	;Until all have been processed
;	mov	bl,processors.pattern
;	mov	bh,bl
;	ret

;	jmp	opaque_pdna
	errn$	opaque_pdna		;Will use this routine
	errn$	transp_pdna

transp_dpon	endp
opaque_dpon	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_pdna
; transp_pdna
;
;   The following is the code for the PDna raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For PDna, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 0
; //	 1	0	dest
; //	 1	1      ~dest
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be inverted.  Wherever the transparency
; // mask is a 1 and the pattern a 0, the destination should be
; // set to 0.
; //
; // Two working patterns will then be generated.  The first will be
; // pattern ANDed with the transparency mask.	This will give a
; // 1 wherever the destination should be inverted.  It will be XORed
; // with the destination.
; //
; // The second working pattern will be the pattern ORed with the
; // inverse of the transparency mask.	This will give a 0 wherever
; // the destination should become 0.  It will be ANDed with the
; // destination.
; //
; // Since the pattern register will be destroyed, it must be reloaded
; // before exit.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_pdna	proc	near
transp_pdna	proc	near

	mov	dx,bx			;Need to use the work register
	and	bx,ax			;XOR mask
	not	ax
	or	dx,ax			;AND mask

transp_pdna_next:
	mov	ax,wptr [di]		;Quicker to have dest into a register
	xor	ax,bx			;Invert bits as needed
	and	ax,dx			;Set bits to 0 as needed
	stosw				;Store and update destination ptr
	loop	transp_pdna_next	;Until all have been processed
	mov	bl,processors.pattern
	mov	bh,bl
	ret

transp_pdna	endp
opaque_pdna	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_pdno
; transp_pdno
;
;   The following is the code for the PDno raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For PDno, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1      ~dest
; //	 1	0	dest
; //	 1	1	 1
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be set to 1.  Wherever the transparency
; // mask is a 1 and the pattern a 0, the destination should be
; // inverted.
; //
; // The pattern will be inverted upon entry (actually before entry).
; // This will give the new truth table of:
; //
; //	Pat    Mask    Result
; //
; //	 1	0	dest
; //	 1	1      ~dest
; //	 0	0	dest
; //	 0	1	 1
; //
; //
; // Two working patterns will then be generated.  The first will be
; // pattern ANDed with the transparency mask.	This will give a
; // 1 wherever the destination should be inverted.  It will be XORed
; // with the destination.
; //
; // The second working pattern will be the pattern ORed with the
; // inverse of the transparency mask.	This will give a 0 wherever
; // the destination should become 0.  It will be ANDed with the
; // destination.
; //
; // Since the pattern register will be destroyed, it must be reloaded
; // before exit.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_pdno	proc	near
transp_pdno	proc	near

;	not	bx			;Inverted at setup time
;
;	mov	dx,bx			;Need to use the work register
;	and	bx,ax			;XOR mask
;	not	dx			;Map 0 ==> 1 for ORing
;	and	dx,ax			;OR mask
;
;transp_pdno_next:
;	mov	ax,wptr [di]		;Quicker to have dest into a register
;	xor	ax,bx			;Invert bits as needed
;	or	ax,dx			;Set bits to 1 as needed
;	stosw				;Store and update destination ptr
;	loop	transp_pdno_next	;Until all have been processed
;	mov	bl,processors.pattern
;	mov	bh,bl
;	ret

;	jmp	opaque_dpan
	errn$	opaque_dpan		;Will use this routine
	errn$	transp_dpan

transp_pdno	endp
opaque_pdno	endp
page
;--------------------------Private-Routine------------------------------;
; opaque_dpan
; transp_dpan
;
;   The following is the code for the DPan raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For DPan, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 1
; //	 1	0	dest
; //	 1	1      ~dest
; //
; // Wherever the transparency mask is 1 and the pattern is 1,
; // the destination should be inverted.  Wherever the transparency
; // mask is a 1 and the pattern a 0, the destination should be
; // set to 0.
; //
; // Two working patterns will then be generated.  The first will be
; // pattern ANDed with the transparency mask.	This will give a
; // 1 wherever the destination should be inverted.  It will be XORed
; // with the destination.
; //
; // The second working pattern will be the pattern inverted and then
; // ANDed with the transparency mask.	This will give a 1 wherever
; // the destination should become 1.  It will be ORed with the
; // destination.
; //
; // Since the pattern register will be destroyed, it must be reloaded
; // before exit.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_dpan	proc	near
transp_dpan	proc	near

	mov	dx,bx			;Need to use the work register
	and	bx,ax			;XOR mask
	not	dx			;Map 0 ==> 1 for ORing
	and	dx,ax			;OR mask

transp_dpan_next:
	mov	ax,wptr [di]		;Quicker to have dest into a register
	xor	ax,bx			;Invert bits as needed
	or	ax,dx			;Set bits to 1 as needed
	stosw				;Store and update destination ptr
	loop	transp_dpan_next	;Until all have been processed
	mov	bl,processors.pattern
	mov	bh,bl
	ret

transp_dpan	endp
opaque_dpan	endp
page
;--------------------------Private-Routine------------------------------;
; transp_pn
;
;   The following is the code for the Pn raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For Pn, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 1
; //	 1	0	dest
; //	 1	1	 0
; //
; // Wherever the transparency mask is 1, the destination should
; // be set to the inverse of the pattern.
; //
; // The pattern will be inverted upon entry (actually before entry).
; // This will give the new truth table of:
; //
; //	Pat    Mask    Result
; //
; //	 1	0	dest
; //	 1	1	 1
; //	 0	0	dest
; //	 0	1	 0
; //
; //
; // Did your mother ever tell you about the old double XOR trick
; // for setting some subset of the bits within a byte?  If not,
; // here it is:
; //
; //	dest = dest XOR pattern AND ~transparency_mask XOR pattern
; //
; // The transparency mask must be inverted to give a mask of 0's
; // where a bit should change.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


transp_pn proc	near

;	not	bx			;Inverted at setup time
;
;	not	ax			;Need inverse for unaltered bits
;	xchg	ax,dx			;Need to use the work register
;
;transp_pn_next:
;	mov	ax,wptr [di]		;Quicker to dest into a register
;	xor	ax,bx			;Invert bits which will not change
;	and	ax,dx			;Set new bits to 0
;	xor	ax,bx			;Invert unchanged again, set new bits
;	stosw				;Store and update destination ptr
;	loop	transp_pn_next		;Until all have been processed
;	ret

;	jmp	transp_p
	errn$	transp_p		;Will use this routine

transp_pn endp
page
;--------------------------Private-Routine------------------------------;
; transp_p
;
;   The following is the code for the P raster operation.
;
;   It will handle both opaque and transparent modes.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // For P, the truth table is:
; //
; //	Pat    Mask    Result
; //
; //	 0	0	dest
; //	 0	1	 1
; //	 1	0	dest
; //	 1	1	 0
; //
; // Wherever the transparency mask is 1, the destination should
; // be set to the pattern.
; //
; // Did your mother ever tell you about the old double XOR trick
; // for setting some subset of the bits within a byte?  If not,
; // here it is:
; //
; //	dest = dest XOR pattern AND ~transparency_mask XOR pattern
; //
; // The transparency mask must be inverted to give a mask of 0's
; // where a bit should change.
; //
; // The same code can be used for both transparent and opaque mode.
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


transp_p proc	near

	not	ax			;Need inverse for unaltered bits
	xchg	ax,dx			;Need to use the work register

transp_p_next:
	mov	ax,wptr [di]		;Quicker to dest into a register
	xor	ax,bx			;Invert bits which will not change
	and	ax,dx			;Set new bits to 0
	xor	ax,bx			;Invert unchanged again, set new bits
	stosw				;Store and update destination ptr
	loop	transp_p_next		;Until all have been processed
	ret

transp_p endp
page
;--------------------------Private-Routine------------------------------;
; opaque_ddxn
; opaque_ddx
; opaque_pn
; opaque_p
;
;   The following is the code for DDxn, DDx, Pn, and P raster
;   operations.  It will be used for memory bitmaps.
;
;   It will handle opaque modes for inner loops only.
;
; Entry:
;	DS:DI --> destination word
;	ES:DI --> destination word
;	   AX  =  mask of altered bits
;	   BX  =  pattern to use (color)
;	   CX  =  loop count
; Returns:
;	DS:DI --> next destination word
;	ES:DI --> next destination word
;	   CX  =  0
; Error Returns:
;	None
; Registers Preserved:
;	BX,DS,ES,BP,SI
; Registers Destroyed:
;	AX,DX,FLAGS
; Calls:
;	None
; History:
;	Thu 12-Mar-1987 16:02:04 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; // The pattern for the Pn raster operation will have been inverted
; // during setup.  For DDx and DDxn, the pattern has to be ignored
; // since these rops are used as accelerators for planes of solid
; // colors, and the correct color set into bx
;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,nothing
	assumes es,nothing


opaque_ddxn	proc	near

	mov	ax,0FFFFh		;Need 1's for storing
	jmpnext
	errn$	opaque_ddx

opaque_ddx	proc	near

	xor	ax,ax			;Need 0's for storing
	jmpnext
	errn$	opaque_p

opaque_pn	proc	near
opaque_p	proc	near

	mov	ax,bx			;Get pattern to store
	jmpnext stop

	rep	stosw			;I like simple raster operations!
some_ret:
	ret

opaque_p	endp
opaque_pn	endp
opaque_ddx	endp
opaque_ddxn	endp


rops_start	endp			;End of all the rops


sEnd	ScanlineSeg

ifdef	PUBDEFS
	include scanline.pub
endif

end
