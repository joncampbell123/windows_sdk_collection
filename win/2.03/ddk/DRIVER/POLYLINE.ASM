	page	,132
;----------------------------Module-Header------------------------------;
; Module Name: polyline.asm
;
; Brief Description: Polyline drawing device driver.
;
; Created: 3/3/87
; Author: **** ******	(******)
;
; Copyright (c) 1983 - 1987  Microsoft Corporation
;
; Given a set of points, draw a set of polylines connecting adjoining
; points.  Depending on whether we are writing to the EGA or a bitmap
; we initialize as necessary.  Solid and styled lines are handled.
; Small (<= 64k bytes) monochrome and color bitmaps and huge monochrome
; and color bitmaps are all supported.  A run length slice algorithm is
; used to determine the pixels used to draw each line.  The algorithm is
; explained later on.  
;
; The line drawing code is slightly different depending on whether
; we are drawing solid lines to the EGA, solid lines to a bitmap, or
; styled lines to either the EGA or a memory bitmap.  For the sake
; of speed, a different set of line drawing routines is called
; for each case.  Only the case we are using will be brought into memory.
; In almost every case, only one of these destinations at a time will 
; be used.  If the user wants to draw lines to the EGA and a bitmap at the
; same time, for example, then they will have both of these sets of 
; routines in memory at the same time, and are hosed on memory.
;
; There are sixteen raster operations (sets of logical operations) performed
; on the data written out.  When writing to the EGA there are four of
; these operations which take two passes of EGA memory.  In each of these
; cases the first passes sets the necessary bits in the necessary planes
; to ones or zeros explicitly.  The second pass then XORs the necessary
; bit in the necessary planes.  The other twelve raster operations can
; be done in one pass of EGA memory.  All raster operations are done in
; one pass of memory for bitmaps.  Depending on the raster operation and
; the color of the pen, it is easily determined whether we set bits to 
; zeros, set bits to ones, invert bits or do nothing.  Bitmaps are written
; to one plane at a time.  In the case of a mono bitmap, only one plane is
; written to.
;
; Styled lines are drawn in two passes.  The first pass draws the line color.
; The second pass draws the gap color.  If we are writing to the EGA and
; have one of the four raster operations which takes two passes of EGA memory,
; then a total of four passes will have to be made.  Two passes to draw
; the line color and two more to draw the gap color.  Styled lines are 
; drawn one pixel at a time.  there is no efficient way around this due
; to the rotating style error term which has to be updated with each bit
; output.  It, unfortunately, depends on the value of the bit, which makes
; outputting a byte at a time expensive to set up.
;
; !!! For the release of Driver 2.0 small bitmaps are stored on a plane !!!
; !!! by plane basis (ie if we have three color planes, then the segment !!!
; !!! is stored as the red color plane, green color plane and then the !!!
; !!! blue color plane).  For the release of Driver 3.0 this will need to !!!
; !!! be changed to match the format of huge bitmaps. !!!
;
; Huge bitmaps are stored on a scan line basis.  For example, if we have
; three color planes, the the bitmap is stored as a red scan line, a green
; scan line, a blue scan line, a red scan line, and so on.  Because of this
; format, there will most likely be some left over bytes at the end of the
; segment.  These will have to be accounted for when crossing segment
; boundaries.  
;
; All lines, except vertical, are drawn from left to right.
;-----------------------------------------------------------------------;


;	This function will perform private stack checking.  In order for
;	private stack checking to occur, two symbols must be defined
;	prior to the inclusion of cmacros.inc.  ?CHKSTK must be defined
;	if the cmacros are to perform stack checking on procedures with
;	local parameters.  ?CHKSTKPROC must be defined if private stack
;	checking will be used.
;
;	The actual macro body for ?CHKSTKPROC will be defined later.
;
;	Since the stack-checking code is designed to work quickly
;	as a near subroutine, and normally resides in segment Code, a
;	duplicate is included in the segment _LINES.  To reach this,
;	the macro ?CHKSTKNAME is defined.


?CHKSTK = 1
?CHKSTKPROC	macro
		endm

?CHKSTKNAME	macro
	call	LineSeg_check_stack
		endm


incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include display.inc
	include egamem.inc
	include ega.inc
	include macros.mac
	include polyline.inc   		; polyline equates file.
	.list

	??_out	polyline


	externA SCREEN_W_BYTES
	externA ScreenSelector
	externA	SCREEN_WIDTH
	externA	SCREEN_HEIGHT
	externA	HYPOTENUSE
	externA	Y_MAJOR_DIST
	externA	X_MAJOR_DIST
	externA	Y_MINOR_DIST
	externA	X_MINOR_DIST
	externA	MAX_STYLE_ERR

	externFP LineSeg_check_stack

ifdef	EXCLUSION
	externFP exclude_far   		; exclude area from screen.
	externFP unexclude_far 		; clear excluded area.
endif


sBegin	Data

	externB enabled_flag		;Non-zero if output allowed

sEnd	Data


createSeg _LINES,LineSeg,word,public,CODE
sBegin	LineSeg
assumes cs,LineSeg


; the following external routines are all used to draw solid lines to
; the EGA.  they are all located in PLYSOLID.ASM.  there are special
; case routines depending on the direction of the line to be drawn.

	externNP	draw_x_axial_segments
	externNP	draw_y_axial_segments
	externNP	draw_diag_x_major_segments
	externNP	draw_diag_y_major_segments
	externNP	draw_horizontal_line	
	externNP	draw_vertical_line
	externNP	draw_diagonal_line	


; the following external routines are all used to draw solid lines to a
; small bitmap. they are all located in POLYBITM.ASM.  there are special
; case routines depending on the direction of the line to be drawn.

	externNP	bitmap_draw_x_axial_segments
	externNP	bitmap_draw_y_axial_segments
	externNP	bitmap_draw_diag_x_major_segments
	externNP	bitmap_draw_diag_y_major_segments
	externNP	bitmap_draw_horizontal_line
	externNP	bitmap_draw_vertical_line
	externNP	bitmap_draw_diagonal_line
	externNP	bitmap_set_to_one	; routine to set bits to ones.
	externNP	bitmap_set_to_zero	; routine to set bits to zeros.
	externNP	bitmap_not_dest		; routine to invert bits.


; the following extermal routines are used for segment overflow checking of
; huge bitmaps.  the address of the proper routine is loaded into OverflowProc.

	externNP	dont_check_overflow	; simply returns.
	externNP	check_segment_overflow	; checks for and handles overflow.


; the following external routines are all used to draw styled lines to
; either the EGA or a small bitmap.  the proper outputting routine is 
; chosen depending on the destination.  these are all located in 
; POLYSTYL.ASM.  there are special case routines depending on the direction
; of the line to be drawn.

	externNP	styled_draw_x_axial_segments
	externNP	styled_draw_y_axial_segments
	externNP	styled_draw_diag_x_major_segments
	externNP	styled_draw_diag_y_major_segments
	externNP	styled_draw_horizontal_line
	externNP	styled_draw_vertical_line
	externNP	styled_draw_diagonal_line
	externNP	styled_line_pixel     	; output routine for EGA line.
	externNP	styled_gap_pixel 	; output routine for EGA gap.
	externNP	styled_bitmap_line_pixel ; output routine for bitmap line.
	externNP	styled_bitmap_gap_pixel	; output routine for bitmap gap.

	public	bit_offset_table
	public	LineSeg_rot_bit_tbl


;	The rotating bit table is used to fetch the initial mask to use
;	for the line code.  The mask is based on D2..D0 of the X coordinate.

LineSeg_rot_bit_tbl	label	byte
		db	10000000b
		db	01000000b
		db	00100000b
		db	00010000b
		db	00001000b
		db	00000100b
		db	00000010b
		db	00000001b



; bit_offset_table contains 64 bytes.  only eight of these bytes have any
; meaning: zero,one,two,four,eight,sixteen,thirty-two,and sixty-four.
; a rotating bitmask, with one bit set, is used to index into this
; table.  depending on which bit is set, the number of bits to rotate a 
; byte is returned.  due to the sparce nature of this table, and to save
; space, several other tables have been embedded into this table (at non-
; meaningful locations).

bit_offset_table   label   byte
	db	7			; zero
bit_offset_one:
	db	6			; one
bit_offset_two:
	db	5			; two
	db	1 dup (?)
bit_offset_four:
	db	4			; four
	db	3 dup (?)

bit_offset_eight:
	db	3			; eight
	db	1 dup (?)


; style_table contains style masks used for the different line styles
; while drawing styled lines.

style_table	label	 byte
	db	11111111B		;Solid line
	db	11100111B		;Dashed
	db	10101010B		;Dotted
	db	11100100B		;Dot-dash
	db	11101010B		;Dash-dot-dot
	db	00000000B		;No line

bit_offset_sixteen:
	db	2			; sixteen
	db	7 dup (?)


; ega_standard_routines contains addresses of routines used to draw
; "non-simple" solid lines on the EGA.
		  	
ega_standard_routines	label   word
	dw	draw_x_axial_segments
	dw	draw_y_axial_segments
	dw	draw_diag_x_major_segments
	dw	draw_diag_y_major_segments

bit_offset_thirty_two:
	db	1			; thirty-two
	db	7 dup (?)


; ega_simple_routines contains addresses of routines used to draw
; "simple" solid lines on the EGA.

ega_simple_routines	label	word
	dw	draw_horizontal_line
	dw	draw_vertical_line
	dw	draw_diagonal_line
	dw	draw_diagonal_line


; bitmap_standard_routines and bitmap_simple_routines contain addresses of 
; routines used to draw solid lines on a small bitmap.

bitmap_standard_routines       label   word
	dw	bitmap_draw_x_axial_segments
	dw	bitmap_draw_y_axial_segments
	dw	bitmap_draw_diag_x_major_segments
	dw	bitmap_draw_diag_y_major_segments


bitmap_simple_routines label   word
	dw	bitmap_draw_horizontal_line
	dw	bitmap_draw_vertical_line
	dw	bitmap_draw_diagonal_line
	dw	bitmap_draw_diagonal_line

bit_offset_sixty_four:
	db	0			; sixty-four


; this is the end of bit_offset_table.


; styled_standard_routines and styled_simple_routines contain addresses of
; routines used to draw styled lines to the ega or a small bitmap.

styled_standard_routines	label   word
	dw	styled_draw_x_axial_segments
	dw	styled_draw_y_axial_segments
	dw	styled_draw_diag_x_major_segments
	dw	styled_draw_diag_y_major_segments

styled_simple_routines	label   word
	dw	styled_draw_horizontal_line
	dw	styled_draw_vertical_line
	dw	styled_draw_diagonal_line
	dw	styled_draw_diagonal_line
		     	

; bitmap_procedure_table contains addresses of routines used for bitmap
; output operations.  depending on the raster operation and the color
; it is determined which of these routines should be used for the current
; color plane.  Its address is then loaded into BitmapProc, located above.

bitmap_procedure_table	 label	 word
	dw	bitmap_set_to_zero
	dw	bitmap_not_dest
	dw	BITMAP_DO_NOTHING
	dw	bitmap_set_to_one

	errnz	bit_offset_one-bit_offset_table-1
	errnz	bit_offset_two-bit_offset_table-2
	errnz	bit_offset_four-bit_offset_table-4
	errnz	bit_offset_eight-bit_offset_table-8
	errnz	bit_offset_sixteen-bit_offset_table-16
	errnz	bit_offset_thirty_two-bit_offset_table-32
	errnz	bit_offset_sixty_four-bit_offset_table-64


	.xlist
	include drawmod2.asm
	.list
page

;--------------------------Public-Routine-------------------------------;
; do_polylines(lp_dst_dev,style,count,lp_points,lp_phys_pen,lp_phys_brush,
;	       lp_draw_mode,lp_clip_rect)
;
; DWORD lp_dst_dev 			// pointer to destination.
; short	style				// output operation.
; short count				// number of points.
; DWORD lp_points			// pointer to set of points.
; DWORD lp_phys_pen			// pointer to physical pen.
; DWORD lp_phys_brush			// pointer to physical brush.
; DWORD	lp_draw_mode			// pointer to drawing mode.
; DWORD lp_clip_rect			// pointer to clipping rect if <> 0.
;
; do_polylines initializes things for the line drawing routines.  if
; the lines are being drawn to the EGA, then the EGA is initialized 
; as necessary and the exclusion area is handled.  if the lines are
; being written to a bitmap, information about the bitmap is loaded.
; necessary tables and pointers are set up depending on line style and
; destination device.  When all of the necessary initialization is
; complete, we jump to polyline_loop which does the DDA and the line
; drawing.
;
; Entry: per parameters.
;
; Returns: AX = 1 if polylines drawn.
;
; Error Returns: AX = 0 if polylines not drawn.
;
; Registers Destroyed: AX,BX,CX,DX,DS,flags.
;
; Registers Preserved: DI,SI.
;
; Calls: 
;	 polyline_set_ega_for_one_pass
;	 polyline_set_ega_for_two_pass
;	 exclude_far	(far version of exclude)
;	 unexclude_far	(far version of un_exclude)
;
; History:
;  Tue 18-Aug-1987 14:50:37 -by-	**** *****	    [*****]
; Added test of the disabled flag.
;
;  Thu 30-Apr-1987 13:20:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; do_polylines(lp_dst_dev,style,count,lp_points,lp_phys_pen,lp_phys_brush,
;	       lp_draw_mode,lp_clip_rect)
;
; DWORD lp_dst_dev 			// pointer to destination.
; short	style				// output operation.
; short count				// number of points.
; DWORD lp_points			// pointer to set of points.
; DWORD lp_phys_pen			// pointer to physical pen.
; DWORD lp_phys_brush			// pointer to physical brush.
; DWORD	lp_draw_mode			// pointer to drawing mode.
; DWORD lp_clip_rect			// pointer to clipping rect if <> 0.
;
; {
;    if (style is not a polyline)
;        return(0);			// return that line is not drawn.
;
;    // for speed considerations EGA specific code is separated from
;    // memory bitmap specific code.
;
;    if (destination is bitmap)
;        jump to get_bitmap_info;
;
;    // handle exclusion area on the screen.
;
;#ifdef EXCLUSION
;    assume exclusion area is entire screen;
;
;    if (passed a clipping rectangle)
;        set exclusion to passed clipping rectangle;
;
;    exclude_far();			// exclude scan from the screen.
;#endif
;
;    jump to load_color_info;
;
;    // get bitmap specific information,  this section is skipped over
;    // if destination is the EGA.
;
;get_bitmap_info:
;    assume monochrome bitmap;
;    get number of color planes;
;    if (number of planes > 1)
;        DeviceFlags = DEST_IS_BITMAP+DEST_IS_COLOR;
;
;    NumPlanes = number of planes;
;
;    // the following group of information is found from the current
;    // bitmap structure.
;
;    BitmapSegment = starting segment of bitmap;
;    NextPlane = index to the next plane;
;    NextScan = index to next scan line;
;    BitmapOffset = starting offset of bitmap;
;
;    // check to see if small or huge bitmap.  if it is huge, this is a
;    // good time to load huge bitmap information.
;
;    if (small bitmap)
;        jump to load_color_info;
;
;    // load huge bitmap information.
;
;    DeviceFlags |= DEST_IS_HUGE;
;
;    SegIndex = index to the next segment;
;    FillBytes = number of fill bytes at end of segment;
;    ScansSeg = scan lines per segment;
;
;    // huge bitmaps are stored in groups of scan lines.  For example, with
;    // three color planes, the bitmap is stored as a scan line of the first
;    // color plane, a scan line of the second color plane, a scan line of the
;    // third color plane, a scan line of the first color plane, and so on.
;    // therefore, when writing to a color plane, a move to the next scan line
;    // actually has to jump over three scan lines.
;
;    NextPlane = NextScan;		// reset index to next plane.
;    NextScan *= Numplanes;		// reset index to next scan line.
;
;    // load the pen and line style information.  this is done for both
;    // EGA and bitmaps.
;
;load_color_info:
;    TmpColor = current pen color;
;    get current line style;
;
;    if (line style > MaxLineStyle)
;        return(0);			// exit with error.
;
;    if (line style == 0)
;        return(1);			// do nothing.
;
;    // get raster operation from draw mode structure.
;
;    DrawModwIndex = raster operation;
;
;    index = current line style;
;    CurStyle = style mask indexed in style_table;
;
;    if (styled line)
;    {
;        // the same style information is needed for both EGA and bitmaps.
;        // get the background mode and store it as the high byte of
;        // StyleFlags and store CurByte as the low byte.  also, get the
;        // gap color and set the rotating style information.
;
;        BackMode = background mode;
;        BackColor.SPECIAL = background color;
;        high byte of StyleFlags = BackMode;
;        low byte of StyleFlags = CurStyle;
;        StyleLength = MAX_STYLE_ERR;
;    }
;
;    // load flag to see if raster op can be done in one pass of EGA memory.
;
;    SingleFlag = dm_flags + DrawModeIndex;
;
;    if (destination is bitmap)
;        jump to set_bitmap_table;	// skip EGA stuff.
;    
;    PenANDFlag = dm_pen_and + DrawModeIndex;
;    PenXORFlag = dm_pen_xor + DrawModeIndex;
;    DataROTFlag = dm_data_r + DrawModeIndex;
;
;    // depending on whether the raster op can be done in one pass of EGA
;    // memory or not, we set up the EGA differently.
;
;    if (raster op can be done in one pass)
;        polyline_set_ega_for_one_pass();
;    else
;        polyline_set_ega_for_two_pass();
;
;    // point to the appropriate set of tables.
;
;    if (solid line)
;    {
;	 DDAstandardProcTable = LineSegOFFSET ega_standard_routines;
;	 DDAsimpleProcTable = LineSegOFFSET ega_simple_routines;
;    }
;    else
;    {
;        StyledProc => styled_line_pixel;  // ega styled line output routine.
;	 DDAstandardProcTable = LineSegOFFSET styled_standard_routines;
;	 DDAsimpleProcTable = LineSegOFFSET styled_simple_routines;
;    }
;
;    jump to set_up_loop;		// skip over bitmap stuff.
;
;set_bitmap_table:
;    if (solid line)
;    {
;	 DDAstandardProcTable = LineSegOFFSET bitmap_standard_routines;
;	 DDAsimpleProcTable = LineSegOFFSET bitmap_simple_routines;
;    }
;    else
;    {
;        StyledProc => styled_bitmap_line_pixel; // bitmap line routine.
;	 DDAstandardProcTable = LineSegOFFSET styled_standard_routines;
;	 DDAsimpleProcTable = LineSegOFFSET styled_simple_routines;
;    }
;
;set_up_loop:
;    // assume destination is not huge bitmap.
;
;    OverflowProc = LineSegOFFSET dont_check_overflow;
;
;    get first polyline point;
;
;    point CurByte at that point;
;    if (destination is huge bitmap)
;    {
;        // we have a huge bitmap.  compute which segment the Y coordinate
;        // is in. set up SegmentNum to show which segment number this
;        // point is in. set SEG_CurByte to the segment which the point is in.
;
;        SegmentNum = current segment number;
;        SEG_CurByte = current segment;
;    }
;
;    index = x coordinate;
;    index &= PARTIAL_BYTE_MASK;	// get bit offset.
;    BL = rot_bit_tbl + index;		// get rotating bitmask in BL.
;
;    jump to polyline loop;
;
;exit_polyline:
;    if (destination is EGA)
;    {
;        restore EGA to default state;
;#ifdef EXCLUSION
;        unexclude_far();		// free up the exclusion area.
;#endif
;
;    return(1);				// indicate success.
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

cProc	do_polylines,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	include plylocal.inc

cBegin
	jc	jump_error_exit		; if stack overflow, exit w/error
	cld
	mov	al,enabled_flag 	;Load this before trashing DS
	lds	si,lp_dst_dev		; check to see what type of device we
	assumes ds,nothing

	mov	cx,[si].bmType		; have.  jump if we have a memory
	jcxz	get_bitmap_info 	; bitmap.
	or	al,al			;If the display has been disabled,
	jz	jump_error_exit 	;  abort the operation
	mov	DeviceFlags,DEST_IS_DEV	; indicate writing to screen.


; This is the EGA.  For speed considerations on the screen,
; perform the EGA specific calculations seperate from memory
; bitmaps.

; take care of the exclusion area on the screen.  this has to do with the
; mouse pointer, so we will not have to worry about this with a memory bitmap.

ifdef	EXCLUSION
	xor	cx,cx			;Assume entire screen
	mov	dx,cx	
	mov	di,SCREEN_HEIGHT-1
	mov	si,SCREEN_WIDTH-1

	lds	bx,lp_clip_rect 	;See if there is a clipping rectangle
	assumes ds,nothing
	mov	ax,ds
	or	ax,bx
	jz	polyline_exclude_scan 	;There was no clipping rectangle

	mov	cx,[bx].left		;Set exclusion to passed clipping
	mov	dx,[bx].top		;  rectangle.
	mov	si,[bx].right
	mov	di,[bx].bottom

polyline_exclude_scan:
	call	exclude_far 		;Exclude the scan from the screen
endif
	jmp	short load_color_info	; skip over bitmap stuff.


; the next two jumps are simply so conditional jumps following can be
; handled neatly.

jump_exit_output:
	jmp	exit_polyline

jump_error_exit:
	jmp	error_exit


; get bitmap specific information.  this section is skipped over if we
; are writing to the screen.
;
; when working with small bitmaps is assumed that the total bitmap, all
; planes included, will fit into one 64K segment.  it is also guaranteed that
; the entire bitmap will fit into the segment pointed to by bmBits.  so for
; small bitmaps we don't have to do any worrying about segment boundaries.
;
; when we have a color bit map, bmPlanes will = 03h or 04h, depending
; on whether the color is 3 or 4 planes deep.  therefore we can use
; what is returned in bmPlanes for the number of planes.

get_bitmap_info:
	mov	DeviceFlags,DEST_IS_BITMAP ; indicate mono bitmap.
	mov	cl,[si].bmPlanes 	; get color plane information.
	cmp	cl,ONLY_ONE_PLANE	; is it monochrome?
	je	set_number_of_planes

must_be_color_bitmap:
	mov	DeviceFlags,DEST_IS_BITMAP+DEST_IS_COLOR

set_number_of_planes:
	mov	NumPlanes,cl		; set the number of planes.

	mov	di,word ptr [si].bmBits+2
	mov	BitmapSegment,di	; save segment to memory bitmap.
	mov	seg_CurByte,di

	mov	di,word ptr [si].bmWidthPlanes
	mov	NextPlane,di		; save index to next plane.

	mov	di,word ptr [si].bmWidthBytes
	mov	NextScan,di		; save pointer to next scan line.

	mov	di,word ptr [si].bmBits
	mov	BitmapOffset,di		; save offset to memory bitmap.

	page
; check to see if we have a small or a huge bitmap.  if it is huge,
; then this is a good time to load huge bitmap information.  if it
; is small, skip this section.

	mov	cx,[si].bmSegmentIndex	; is it a huge bitmap?
	jcxz	load_color_info 	; jump if not.

	or	DeviceFlags,DEST_IS_HUGE ; indicate huge bitmap.

	mov	SegIndex,cx		; save index to next segment.
	mov	cx,[si].bmFillBytes
	mov	FillBytes,cx		; save # of unused bytes in segment.
	mov	cx,[si].bmScanSegment
	mov	ScansSeg,cx		; save scans per segment.

; huge bitmaps are stored in groups of scan lines.  For example, with
; three color planes, the bitmap is stored as a scan line of the first
; color plane, a scan line of the second color plane, a scan line of the
; third color plane, a scan line of the first color plane, and so on.
; therefore, when writing to a color plane, a move to the next scan line
; actually has to jump over three scan lines.

	mov	ax,NextScan		; the next plane starts with the
	mov	NextPlane,ax 		; next scan line.
	mov	al,NumPlanes		; get number of color planes.
	xor	ah,ah			; make it a word.
	mul	NextScan
	mov	NextScan,ax		; store new scan line index.

; load the pen color and line style information.  this is done for both
; bitmaps and the ega.

load_color_info:
	lds	si,lp_phys_pen		;Get current pen
	mov	ah,[si].oem_pen_pcol.SPECIAL ;Get the accelerators of the pen
	mov	CurPen.SPECIAL,ah

	mov	TmpColor,ah
	mov	cx,[si].oem_pen_style	;Get and save line style
	cmp	cl,MaxLineStyle 	;Legal line style?
	jg	jump_error_exit 	;  No, abort
	je	jump_exit_output	;  Null, exit
	errnz	MaxLineStyle-LS_NOLINE

; Get the information out of the Raster Op Block on the raster operation
; to use.

	lds	si,lp_draw_mode 	;--> DrawMode Block
	mov	bx,[si].Rop2		;Get the drawing mode (raster op)
	dec	bx			;  make it zero based
	and	bx,RASTER_OP_MASK	;  (play it safe)
	mov	DrawModeIndex,bx	;  and save a copy

	mov	si,cx			; from the line style passed in, get
	mov	al,cs:style_table[si]	; a style mask from the table, and
	mov	CurStyle,al		; save that in CurStyle.

	jcxz	polyline_set_up_ega	; skip over style stuff if solid line.

; if we get here then we have a styled line.  it does not matter if we are
; writing to a bitmap or the ega, the same information needs to be set up.
; get the background mode, OPAQUE or TRANSPARENT, and store it as the high
; byte of StyleFlags and store CurStyle as the low byte.  Also, get the
; backgound, or gap, color, and initialize the StyleLength value.

	lds	si,lp_draw_mode 	;--> DrawMode Block
	mov	dx,[si].bkMode		; get background mode.
	mov	BackMode,dl		; and save it.
	xor	dh,dh
	cmp	dl,OPAQUE
	jne	get_back_color
	mov	dh,OPAQUE

get_back_color:
	mov	dl,byte ptr [si].bkColor.SPECIAL
	mov	byte ptr BackColor.SPECIAL,dl

	mov	StyleLength,MAX_STYLE_ERR
	mov	dl,al
	mov	StyleFlags,dx		; set the style flags.
	mov	CaseFlags,0FFh		; initial value.

polyline_set_up_ega:
	mov	al,dm_flags[bx] 	
	mov	SingleFlag,al		

	test	DeviceFlags,DEST_IS_DEV	; if we are writing to a bitmap, we
	jz	set_bitmap_table	; can skip all of the ega setup.

	  			 	; store several raster operation flags.
					; these flags are only used with the
	mov	al,dm_pen_and[bx]	; ega, so the bitmap code skips over
	mov	PenANDFlag,al		; this.  by assigning these local
	mov	al,dm_pen_xor[bx]	; variables now, we free up a register
	mov	PenXORFlag,al		; for later use and do not have to
	mov	al,dm_data_r[bx]	; reload DrawModeIndex.
	mov	DataROTFlag,al

; depending on whether the raster operation coming in can be done in one
; pass of ega memory or not, we set up the ega differently.  calls are used
; here because identical operations have to be done later on.

	test	SingleFlag,SINGLE_OK
	jnz	one_pass_set_up

	page
; set up the ega for a two pass raster operation.

	call	polyline_set_ega_for_two_pass
	jmp	short set_solid_tables

; set up the ega for a one pass raster operation.

one_pass_set_up:
	call	polyline_set_ega_for_one_pass

; point the the appropriate set of jump tables, depending on whether we have
; solid or styled lines, and whether we are writing to a bitmap or the screen.

set_solid_tables:
	mov	ax,LineSegOFFSET ega_standard_routines
	mov	dx,LineSegOFFSET ega_simple_routines
	jcxz	set_the_tables

set_styled_standard_routiness:
	mov	ax,LineSegOFFSET styled_standard_routines
	mov	dx,LineSegOFFSET styled_simple_routines
	jmp	short set_the_tables

set_bitmap_table:
	mov	ax,LineSegOFFSET bitmap_standard_routines
	mov	dx,LineSegOFFSET bitmap_simple_routines
	jcxz	set_the_tables

	mov	ax,LineSegOFFSET styled_standard_routines
	mov	dx,LineSegOFFSET styled_simple_routines

set_the_tables:
	mov	DDAstandardProcTable,ax
	mov	DDAsimpleProcTable,dx

; set up OverflowProc assuming destination is not huge bitmap.

	mov	word ptr OverflowProc,LineSegOFFSET dont_check_overflow

;	set up polyline loop (dest byte calculation)

	lds	si,lp_points		;--> first point
	lodsw
	mov	dx,ax			;Save X coordinate
	errnz	xcoord
	lodsw
	mov	di,ax			;Save Y coordinate
	errnz	ycoord-xcoord-2

	test	DeviceFlags,DEST_IS_DEV
	jnz	set_up_ega_curbyte	; jump if writing to screen

	page
; set up the pointer to the current byte in the memory bitmap.

	push	dx			; save x coordinate.
	test	DeviceFlags,DEST_IS_HUGE
	jz	set_up_small_only	; skip next section if small bitmap.

; we have a huge bitmap.  compute which segment the Y coordinate is in.
; set up SegmentNum to show which segment number this point is in.
; set SEG_CurByte to the segment which the point is in.

	xor	dx,dx			; zero out segment bias.
	mov	SegmentNum,dx		; zero out segment counter.

top_segment_loop:
	add	dx,SegIndex		; show in next segment.
	inc	SegmentNum		
	sub	ax,ScansSeg		; see if it is this segment.
	jnc	top_segment_loop	; not in this segment, try next one.
	add	ax,ScansSeg		; restore proper values.
	dec	SegmentNum
	sub	dx,SegIndex

	add	dx,BitmapSegment
	mov	seg_CurByte,dx		; point to proper segment.

set_up_small_only:
	mul	NextScan
	add	ax,BitmapOffset		; point to start of current scanline.

	pop	dx			; restore x coordinate.

	xchg	cx,di			; save new y coordinate in CX.

	jmp	short finish_curbyte

; set up the pointer to the current byte in ega memory.

set_up_ega_curbyte:
	mov	cx,dx
	mov	dx,SCREEN_W_BYTES
	mov	NextScan,dx
	mul	dx
	mov	dx,cx
	mov	cx,di
	mov	seg_CurByte,ScreenSelector ;Set screen segment

finish_curbyte:
	mov	di,dx			;Compute offset within scanline
	shiftr	di,3
	add	di,ax			;(di) = offset of first byte
	mov	off_CurByte,di

get_rot_bitmask:
	mov	bx,dx			;Get rotating bit mask for the move
	and	bx,PARTIAL_BYTE_MASK	;  point and save it
	mov	bl,LineSeg_rot_bit_tbl[bx]

	mov	ax,count		;Set count of line segments
	jmp	short polyline_loop

exit_polyline:
	test	DeviceFlags,DEST_IS_DEV
	jz	bitmap_exit		; jump if bitmap.

; restore the EGA back to the defaults.

	mov	dx,EGA_BASE+GRAF_ADDR
	mov	ax,GRAF_ENAB_SR 	; disable all planes for set_reset.
	out16	dx,ax

	mov	ax,DR_SET*256 + GRAF_DATA_ROT
	out16	dx,ax			; set, no rotation.

	mov	ax,0FF00h+GRAF_BIT_MASK
	out16	dx,ax			; enable all bits.

	mov	ax,MM_ALL		; enable all planes.
	mov	dl,SEQ_DATA
	out	dx,al

device_exit:
	mov	ax,1			; show success.
	jmp	short exit

error_exit:
	xor	ax,ax			; show error.
	errn$	exit

exit:
ifdef	EXCLUSION
	call	unexclude_far		;Remove any exclusion area
endif

polyline_get_out:

cEnd

; bitmap_exit is separated out because the bitmap code needs to skip
; around all of the EGA setup and exclusion stuff when exiting, and it
; is faster for EGA and bitmap cases if we don't have to jump around.

bitmap_exit:
	mov	ax,1			; indicate success.
	jmp	short polyline_get_out
page

;-------------------------Private-Routine-------------------------------;
; polyline_loop
;
; polyline_loop contains the run length slice algorithm DDA.
; when this routine is entered, it is passed the coordinates of the first
; point.  it gets the next point and draws the line between them.  the
; first point of the line is drawn and the last point is not.  this is 
; so when raster operations such as XOR are done, the end points do not
; get drawn twice and cancel each other.  different line drawing routines
; are called depending on the destination device and the type of line
; being drawn.
;
; the DDA algorithm used here is based on Jack Bresenham's Run Length
; Algorithm for Incremental Lines.  NATO ASI Series.  Volume F17.
; Fundamental Algorithms for Computer Graphics.  Springer-Verlag
; Berlin Heidelberg  1985.
;
; ALGORITHM
;
; // Given two integer points (x1,y1) and (x2,y2), the following algorithm
; // will calculate run lengths or slices of constant direction movement to 
; // increment a rastered line from (x1,y1) to (x2,y2) under the constraint
; // that unit steps are restricted to those eight axial/diagonal moves in
; // which the x and/or y positions change only by 1, 0, or -1 per step.
;
; HALF OCTANT NORMALIZATION
;
; // Normalize the directed line segment to a standard first partial octant,
; // zero origin form.
;
; delta_x = x2 - x1;
; delta_y = y2 - y1;
; delta_major = max(delta_x, delta_y);
; delta_minor = min(delta_x, delta_y);
; del_b = min(delta_minor, (delta_major - delta_minor));
;
; // Determine true display incremental directions (m11,m12) and (m21,m22) 
; // which correspond to full first octant diagonal and axial unit steps,
; // then re-order as appropriate for pseudo-axial, pseudo-diagonal partial
; // octant movement pairs (s11,s12) and (s21,s22).
;
; m21 = (delta_x >= 0) ? 1 : -1;
; m22 = (delta_y >= 0) ? 1 : -1;
; m11 = (abs(delta_x) >= abs(delta_y)) ? m21 : 0;
; m12 = (abs(delta_x) >= abs(delta_y)) ? 0 : m22;
;
; if (delta_major >= 2 * delta_minor)
; {
;	s11 = m11;
;	s12 = m12;
;	s21 = m21;
;	s22 = m22;
; }
; else
; {
;	s11 = m21;
;	s12 = m22;
;	s21 = m11;
;	s22 = m12;
; }
;
; // Provisionally specifiy HLast to accomodate the degenerate cases of 
; // movement solely in an axial or diagonal direction.
;
; HLast = delta_major;
;
; if (del_b > 0)
; {
;	// PARAMETERS
;
;	// Calculate parameters for the repetitive run length generaton loop.
;
;	BitCount = delta_major / del_b;	// bits per intemediate line segment.
;
;	r = delta_major % del_b;
;
;	m = BitCount / 2;
;
;	n = even(q) ? r : (r + del_b);
; 
;	if ((delta_y >= 0) || (n != 0))
;		HFirst = m;		// bits per first line segment.
;	else
;		HFirst = m -1;		// bits per first line segment.
;
;	if ((delta_y < 0) || (n != 0))
;		HLast = m;		// bits per last line segment.
;	else
;		HLast = m - 1;		// bits per last line segment.
;
;	DDAcount = del_b;		// number of intermediate line segments.
;
;	// Bias the initial decision variable to generate retractable lines.
;	
;	if (delta_y >= 0)
;		ErrTerm = n + 2*r - 2*del_b;
;	else
;		ErrTerm = n + 2*r - 2*del_b - 1;
;
;	// INITIAL RUN LENGTH
;
;	// Output initial run length pair.  First run of length HFirst is
;	// in the direction (s11,s12).  The single diagonal step of length
;	// one is in direction (s21,s22).
;
;	for (i = 0; i < HFirst; i++)
;	{
;		x1 += s11;
;		y1 += s12;
;	}
;
;	x1 += s21;
;	x2 += s22;
;
;	// INTERMEDIATE RUN LENGTH LOOP
;
;	// select appropriate run and output successive intermediate run
;	// length pairs associated with movement directions given by
;	// (s11,s12) and (s21,s22).
;
;	while (-- count)
;	{
;		if (ErrTerm >= 0)
;		{
;			hi = BitCount;	// the ith intermediate segment.
;			ErrTerm += 2*r - 2*del_b;
;		}
;		else
;		{
;			hi = BitCount - 1;
;			ErrTerm += 2*r;
;		}
;
;		for (j = 0; j < hi; j++)
;		{
;			x1 += s12;
;			x2 += s12;
;		}
;
;		x1 += s21;
;		y1 += s22;
;	}
; }
;
; // TERMINATION
;
; // output final single run length in direction (s11,s12).
;
; for (i = 0; i < HLast; i++)
; {
;	x1 += s11;
;	x2 += s12;
; }
;
; // END OF ALGORITHM.
;
;
; Entry: 
;	AX = number of points to process + 1.
;	BL = rotating bit mask.
;	CX = first Y coordinate.
;	DX = first X coordinate.
;	DI = offset of current byte.
;	DS:SI points to next point.
;
; Returns: None.
;
; Error Returns: None.
;
; Registers Destroyed: AX,BX,CX,DX,DI,SI,DS,flags.
;
; Registers Preserved: None.
;
; Calls: 
;	 ega_line_dispatch_routine
;	 bitmap_line_dispatch_routine
;	 polyline_set_ega_for_one_pass
;	 polyline_set_ega_for_two_pass
;
; History:
;  Thu 30-Apr-1987 13:20:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Major re-write.
;  Tue 28-Oct-1986 16:05:04	-by-    **** ********	    [*****]
; Created.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; polyline_loop
;
; {
;    number of points--;
;    if (number of points == 0)
;        jump to exit_polyline;		// no more lines to do.
;
;    // set some variables.
;
;    RotBitMask = BL;
;    OFF_CurByte = DI;
;    TmpCurByte = DI;
;
;    get the next point;
;
;    if (destination is huge bitmap)
;    {
;        // compute which 64K block of memory the end point is in.  if 
;        // both points are in the same segment, then we don't need to
;        // check for segment overflow, otherwise we do.
;
;        compute segment end point is in;
;        if (ending segment == starting segment)
;            OverflowProc => dont_check_overflow;
;        else
;            OverflowProc => check_segment_overflow;
;
;        // we have a huge bitmap.  compute which segment the Y coordinate
;        // is in. set up SegmentNum to show which segment number this
;        // point is in. 
;
;        SegmentNum = number of current segment.
;    }
;
;    CaseFlags = 0;
;
;    // depending on whether delta x is positive or negative, we will
;    // set the stepping left or right bit.
; 
;    if (delta x >= 0)
;        jump to x2_ge_x1;		// moving right.
;
;    delta x *= -1;			// make it positive.
;    CaseFlags |= STEPLEFT;		// indicate moving left.
;
;    // the following bunch of ugliness is to set up CurByte when we are
;    // drawing a left moving line.  we will be drawing all lines moving
;    // right.  so when we draw a left moving line, we have to jump CurByte
;    // over to the left side of the line, then draw the line, and then
;    // reset CurByte one last time.
;
;    reset OFF_CurByte to left endpoint of line;
;
;    if (destination is huge bitmap)
;    {
;        // we have a huge bitmap.  compute which segment the Y coordinate
;        // is in. set up SegmentNum to show which segment number this
;        // point is in. set SEG_CurByte to the segment which the point is in.
;
;        SegmentNum = number of current segment;
;        SEG_CurByte = current segment;
;        TmpSegment = current segment; 	// segment reset value.
;    }
;
;    set new RotBitMask;
;    TmpBitMask = RotBitMask;		// save a copy.
;
;    swap Y1 and Y2;			// since we are drawing line backwards.
;
;    // depending on whether delta y is positive or negative, set the 
;    // stepping up or down bit.
;
;x2_ge_x1:
;    AddVertStep = NextScan;		// set up scan line jumping values.
;    SubVertStep = - NextScan;
;
;    if (delta y >= 0)
;        jump to y2_ge_y1;		// moving down.
;
;    delta y *= -1;			// make it positive.
;    CaseFlags |= Y_MAJOR;		// indicate moving up.
;
;    AddVertStep = -NextScan;		// reverse scan line jumping values.
;    SubVertStep = NextScan;
;
;    // depending on the relative values of delta x and delta y we decide
;    // whether the line is x or y major and set the bit accordingly.
;
;y2_ge_y1:
;    if (delta x < delta y)
;        CaseFlags |= Y_MAJOR;		// indicate y major.
;
;    // if delta_major >= 2*delta_minor then the line is axial, else
;    // it is diagonal, and we set the bit accordingly.
;
;    if (delta_major < 2 * delta_minor)
;        CaseFlags |= DIAGONAL;		// indicate diagonal line.
;
;    // now all of the CaseFlags are set to point us to the correct case
;    // if the procedure table.  we now want to check the value of del_b
;    // to see if we have a simple case.  if del_b = 0 then we have a 
;    // simple case.
;    // del_b = min(delta_minor, (delta_major - delta_minor)).
;
;    del_b = min(delta_minor, (delta_major - delta_minor));
;
;    if (del_b == 0)
;    {
;        // we have a simple case.  by simple it is meant that the line is
;        // horizontal, vertical or on a diagonal.  these cases can be 
;        // handled much faster than other cases, so they are broken out
;        // separately.  call the proper routine, depending on whether we 
;        // are writing to a bitmap or the EGA.  by setting up for the call
;        // slightly differently from the standard line case, we are able to
;        // use the same line drawing procedure.  in a simple case, delta_major
;        // is equal to the total number of pixels of the entire line.
;
;        HFirst = delta_major;		// total # of pixels to draw.
;
;        // point to the proper set of routines.
;
;        DDAcurrentProcTable = DDAsimpleProcTable;
;
;        if (writing to the EGA)
;        {
;            ega_line_dispatch_routine();	// go draw the line.
;            jump to polyline_get_return;
;        }
;        else
;        {
;            bitmap_line_dispatch_routine;	// go draw the line.
;            jump to ignore_two_pass_setup;
;        }
;    }
;
;    // if we get here, we do not have one of the simple cases.  the next
;    // thing to do is to calculate the parameters for the repetitive
;    // run length generation loop.
;
;    BitCount = delta_major / del_b; 		// bits per intermediate segment.
;    
;    if (BitCount is even)
;        n = r (remainder from above division);
;    else
;	 n = r + del_b;
;
;    if (delta y < 0 && n == 0)
;        HFirst = m - 1;		// decrement first segment length.
;    else
;        HFirst = m;
;
;    if (delta y >= 0 && n == 0)	
;        HLast = m - 1;			// decrement last segment length.
;    else
;        HLast = m;
;
;    DDAcount = del_b;			// set number of segments.
;    DDAtmpcount = del_b;		// save a copy.
;
;    // now calculate the proper error term.
;
;    if (delta y >= 0)
;        ErrTerm = n + 2*r - 2*del_b;
;    else
;        ErrTerm = n + 2*r - 2*del_b - 1;
;
;    ErrAdj = 2*r;
;    ErrReset = 2*r - 2*del_b;
;
;    DDAcurentProcTable = DDAstandardProcTable;
;
;    if (destination is bitmap)
;    {
;        bitmap_line_dispatch_routine();
;        jump to ignore_two_pass_setup;
;    }
;    else
;        ega_line_dispatch_routine();
;
;polyline_get_return:
;    if (raster op was done in one pass)
;        jump to ignore_two_pass_setup;
;
;    // only do the following if finishing up a line with a raster op which
;    // took two passes of EGA memory, since the EGA would be all screwed
;    // up to start the next line.
;
;    polyline_set_ega_for_two_pass();
;
;ignore_two_pass_setup:
;    if (moving left)
;    {
;        DI = TmpCurByte;		// reset current byte pointer.
;        BL = TmpBitMask;		// reset bitmask.
;    }
;
;    get next point to be processed;
;
;    jump to polyline_loop;		// process the next line.
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

polyline_loop	proc	near

	dec	ax			; any lines to draw?
	jle	exit_polyline		; no, then exit.

	push	ax    	 		; save # of points left to process
	push	ds			; save point pointer
	push	si
	mov	RotBitMask,bl		; save RotBitMask
	mov	off_CurByte,di		; save current byte offset
	mov	TmpCurByte,di		; save reset value.

	mov	ax,[si].xcoord		; get new X coordinate
	mov	si,[si].ycoord		; get new Y coordinate

	test	DeviceFlags,DEST_IS_HUGE
	jz	set_up_stepping_flags	; skip next section if not huge bitmap.

; compute which 64K block of memory the ending point is in.  if both points
; are in the same segment, then we don't need to check for segment overflow.
; otherwise we do.

	push	ax
	push	dx

	mov	ax,si			; get Y2.
	xor	dx,dx
	div	ScansSeg		; divide by #scans per segment.

	mov	word ptr OverflowProc,LineSegOFFSET dont_check_overflow
	cmp	ax,SegmentNum		; is it in the same segment?
	je	skip_segment_stuff	; jump if so.
	mov	word ptr OverflowProc,LineSegOFFSET check_segment_overflow

; we have a huge bitmap.  compute which segment the Y coordinate is in.
; set up SegmentNum to show which segment number this point is in.

set_up_segments:
	xor	dx,dx			; zero out segment bias.
	mov	SegmentNum,dx		; zero out segment counter.
	mov	ax,si			; get y coordinate.

right_top_segment_loop:
	add	dx,SegIndex		; show in next segment.
	inc	SegmentNum		
	sub	ax,ScansSeg		; see if it is this segment.
	jnc	right_top_segment_loop	; not in this segment, try next one.
	add	ax,ScansSeg		; restore proper values.
	dec	SegmentNum
	sub	dx,SegIndex

skip_segment_stuff:
	pop	dx
	pop	ax

set_up_stepping_flags:
	xor	di,di			; zero stepping flags

; It will take less time to refetch the (x,y) pair that was just
; fetched at the end of the loop than to push them onto the stack
; and then pop them at the end of the loop.

; AX = x2
; CX = y1
; DX = x1
; SI = y2

	cmp	CurStyle,0FFh		
	je	set_for_solid

; assume styled line is not moving left and set up style parameters as such.

	mov	XMajorDist,X_MAJOR_DIST
	mov	YMajorDist,Y_MAJOR_DIST
	mov	XMinorDist,X_MINOR_DIST
	mov	YMinorDist,Y_MINOR_DIST
	mov	Hypot,HYPOTENUSE

; depending on whether delta x is positive or negative, we will then
; set the stepping left or right bits.

set_for_solid:
	mov	bx,ax			; save X2 in BX.
	sub	ax,dx			; AX = delta_x
;	jge	x2_ge_x1
	jl	gots_to_be_left
	jmp	x2_ge_x1

gots_to_be_left:
	neg	ax			; AX = abs(delta_x)
	or	di,STEP_LEFT		; vs stepRight

	cmp	CurStyle,0FFh		; if styled line, jump over the
	jne	x2_ge_x1		; line resetting stuff.

; the following bunch of ugliness is to set up CurByte when we are drawing
; a left moving line.  we will be drawing all lines moving right.  so when
; we draw a left moving line, we have to jump CurByte over to the left side
; of the line, then draw the line, and then reset CurByte one last time.

continue_left_case:
	push	ax			; save delta_x.
	xchg	dx,bx			; save X2 in DX.
	test	DeviceFlags,DEST_IS_DEV
	jnz	reset_ega_curbyte	; jump if writing to screen

; set up the pointer to the current byte in the memory bitmap.

 	push	dx			; save x coordinate.
	mov	ax,si			; get y coordinate.
	test	DeviceFlags,DEST_IS_HUGE
	jz	set_left_small_only	; skip next section if small bitmap.

; we have a huge bitmap.  compute which segment the Y coordinate is in.
; set up SegmentNum to show which segment number this point is in.
; set SEG_CurByte to the segment which the point is in.

	xor	dx,dx			; zero out segment bias.
	mov	SegmentNum,dx		; zero out segment counter.

left_top_segment_loop:
	add	dx,SegIndex		; show in next segment.
	inc	SegmentNum		; increment segment counter.
	sub	ax,ScansSeg		; see if it is this segment.
	jnc	left_top_segment_loop	; not in this segment, try next one.
	add	ax,ScansSeg		; restore proper values.
	dec	SegmentNum
	sub	dx,SegIndex

	add	dx,BitmapSegment
	mov	seg_CurByte,dx		; point to proper segment.
	mov	TmpSegment,dx		; save reset value.

set_left_small_only:
	mul	NextScan
	add	ax,BitmapOffset		; point to start of current scanline.

	pop	dx			; restore x coordinate.

	jmp	short finish_reset_curbyte

	page
; set up the pointer to the current byte in ega memory.

reset_ega_curbyte:
	mov	bx,dx
	mov	ax,SCREEN_W_BYTES
	mul	si
	mov	dx,bx

finish_reset_curbyte:
	mov	bx,dx			; keep x coordinate in BX.
	shiftr	dx,3
	add	dx,ax			;(di) = offset of first byte
	pop	ax			; restore delta_x.
	mov	off_CurByte,dx
	mov	TmpCurByte,dx		; save reset value.

reget_rot_bitmask:
	and	bx,00000111B		; get rotating bitmask for the move
	mov	bl,LineSeg_rot_bit_tbl[bx]     ; point and save it.
	mov	RotBitMask,bl
	mov	TmpBitMask,bl
	xchg	cx,si			; swap Y1 and Y2.

; depending on whether delta y is positive or negative, we will then
; set the stepping up or down bits.

x2_ge_x1:
	mov	bx,NextScan
	mov	AddVertStep,bx		; set up the stepping constants.
	neg	bx
	mov	SubVertStep,bx
	sub	si,cx			; SI = delta_y
	jge	y2_ge_y1
	neg	si			; SI = abs(delta_y)
	or	di,STEP_UP		; vs stepDown
	mov	AddVertStep,bx		; set up the stepping constants.
	neg	bx
	mov	SubVertStep,bx

	page
; we want --
; AX = delta_major (major axis delta = max(abs(delta_x), abs(delta_y)))
; SI = delta_minor (minor axis delta = min(abs(delta_x), abs(delta_y)))

; depending on the relative values of delta x and delta y we decide
; whether the line is x or y major and set the bit accordingly.

y2_ge_y1:
	cmp	ax,si			; abs(delta_x) > abs(delta_y) ?
	jge	x_major
	xchg	ax,si
	or	di,Y_MAJOR		; vs Xmajor

; if delta_major >= 2*delta_minor then the line is axial, else it is diagonal,
; and we set the bit accordingly.

x_major:
	mov	bx,si
	shl	bx,1			; BX = 2 * delta_minor
	cmp	ax,bx			; delta_major >= 2*delta_minor ?
	jge	da_ge_2db
	or	di,DIAGONAL		; vs Axial

; we now have all of the bits set to point us to the right case in the
; jump table. we now want to check the value of del_b to see if we have
; a special case.  if del_b <= 0 then we have a special (simple) case.

; del_b = min(delta_minor, (delta_major - delta_minor))
; del_b >= 0.

da_ge_2db:
	mov	CaseFlags,di		; save case flags.
	mov	bx,ax
	sub	bx,si			; BX = del_b = delta_major - delta_minor
	cmp	bx,si
	jl	da_lt_db
	mov	bx,si			; BX = del_b = delta_minor

; AX = delta_major
; BX = del_b.

da_lt_db:
	or	bx,bx			; del_b > 0 ?
	jg	del_b_gt_zero		; jump if so.

	page
; we have a simple case.  by simple it is meant that the line is horizontal,
; vertical or on a diagonal.  these cases can be handled much faster than
; other cases, so they are broken out separately.  call the proper routine,
; depending whether we are writing to a bitmap or the EGA.  by setting up
; for the call slightly different from the standard line case, we are able 
; to use the same line drawing procedure.  in a simple case, delta_major is
; eqaul to the total number of pixels of the entire line to be drawn.

we_have_simple_case:
	xchg	cx,ax			; put delta_major into CX.
	mov	HFirst,cx		; HFirst = total pixels to draw.
	mov	ax,DDAsimpleProcTable	; make sure we call "simple" line
	mov	DDAcurrentProcTable,ax	; routines.

	test	DeviceFlags,DEST_IS_DEV
	jz	do_simple_bitmap_call	; jump if writing to memory bitmap.

	call	ega_line_dispatch_routine    ; draw ega simple case line.

	jmp	polyline_get_return

do_simple_bitmap_call:
	call	bitmap_line_dispatch_routine ; draw bitmap simple case line.

	jmp	ignore_two_pass_setup
	
					; AX = delta_major
					; BX = del_b

; if we get to this point then we do not have one of the simple cases.
; the next thing we have to do is to calculate the parameters for the
; repetitive run length generation loop.

del_b_gt_zero:
	cwd				; DX:AX = delta_major
	div	bx			; AX = BitCount, DX = r
	mov	BitCount,ax		; save bits per line segment count.
	mov	si,dx			; SI = n = r (if BitCount is even)

	mov	cx,ax
	shr	cx,1			; CX = BitCount/2.
	jnc	bitcount_is_even	; jump if BitCount is even.

	add	si,bx			; SI = n = r + del_b (if BitCount is odd)

bitcount_is_even:
	mov	HLast,cx		; HLast = m = BitCount/2.
	or	si,si			; check if n == 0.
	jnz	end_retrace_adjust	; jump if n != 0.
	test	di,STEP_UP		; check if delta_y < 0.
	jnz	stepping_up		; jump if so.
	dec	cx			; if not, CX = m - 1.
	jmp	short end_retrace_adjust

stepping_up:
	dec	HLast			; HLast = m -1.

					; AX = BitCount.
					; BX = del_b.
					; CX = HFirst.
					; DX = r.
					; SI = n.

end_retrace_adjust:
	mov	HFirst,cx		; set length of first segment.
	mov	DDAcount,bx		; set the count to del_b.
	mov	DDAtmpcount,bx		; save value for second pass.

; we now calculate the proper error term for the particular case we have.

	shl	dx,1			; DX = 2*r.
	shl	bx,1			; BX = 2*del_b.
	add	si,dx			; assume delta_y < 0.
	sub	si,bx			; ErrTerm = n + 2*r - 2*del_b.
	test	di,STEP_UP		; test delta_y.
	jnz	going_up		; jump if delta_y >= 0.
	dec	si			; ErrTerm = n + 2*r - 2*del_b - 1

going_up:
	mov	ErrAdj,dx		; ErrAdj = 2*r
	sub	dx,bx
	mov	ErrReset,dx		; ErrReset = 2*r - 2*del_b

; test to see if we are writing to bitmap or screen.

	mov	dx,DDAstandardProcTable
	mov	DDAcurrentProcTable,dx

	test	DeviceFlags,DEST_IS_DEV
	jnz	go_do_standard_ega

	call	bitmap_line_dispatch_routine
	jmp	short ignore_two_pass_setup

go_do_standard_ega:
	call	ega_line_dispatch_routine

polyline_get_return:
	test	SingleFlag,SINGLE_OK
	jnz	ignore_two_pass_setup

; we only have to do the following if we are finishing up a line with
; one of the raster ops which cannot be done with one pass of ega memory,
; since the ega would be all screwed up to start the next line.

	call	polyline_set_ega_for_two_pass

ignore_two_pass_setup:
	cmp	CurStyle,0FFh		; skip if styled line.
	jne	dont_reset_curbyte

	test	CaseFlags,STEP_LEFT
	jz	dont_reset_curbyte	; skip next section if moving right.

ignore_style_reset:
	mov	di,TmpCurByte		; reset DI when moving left.
	mov	bl,TmpBitMask		; reset bitmask.

dont_reset_curbyte:
	pop	si			; get back polyline pointer
	pop	ds

	cld
	lodsw				; get next X coordinate.
	mov	dx,ax
	lodsw				; get next Y coordinate.
	mov	cx,ax
	errnz	xcoord
	errnz	ycoord-2

	or	bl,bl			; if RotBitMask = 0, then recompute
	jnz	polyline_loop_back_to_top
	call	update_curbyte

polyline_loop_back_to_top:
	pop	ax			;Get back # of line segments left
	jmp	polyline_loop		;See about another line

polyline_loop	endp
page

;-------------------------Private-Routine-------------------------------;
; polyline_set_ega_for_one_pass
;
; polyline_set_ega_for_one_pass set up the ega to handle one of the twelve
; raster operations which can be done in one pass of EGA memory.
;
; Entry: None.
;
; Returns: None.
;
; Error Returns: None.
;
; Registers Destroyed: DL,flags.
;
; Registers Preserved: AX,DH.
;
; Calls: None
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; polyline_set_ega_for_one_pass
;
; {
;    // this routine sets up the EGA to handle one of the twelve
;    // raster operations which can be done in one pass of EGA memory.
;
;    // set up the set/reset register.
;
;    TmpColor &= PenANDFlag;
;    TmpColor ^= PenXORFlag;
;    set set/reset register to TmpColor;
;
;    enable all planes for set/reset;
;
;    set data rotate register to DataROTFlag;
;
;    // the bitmask register must be the last register written to the 
;    // graphics controller so that its address is still in the
;    // address register.
;
;    enable all bits in bitmask register;
;
;    enable all planes for writing in the sequencer;
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

polyline_set_ega_for_one_pass	proc	near

	push	ax
	mov	ah,TmpColor
	and	ah,PenANDFlag
	xor	ah,PenXORFlag
	mov	al,GRAF_SET_RESET	;Write to set/reset register
	mov	dx,EGA_BASE + GRAF_ADDR
	out16	dx,ax

	mov	ah,MM_ALL		; enable all planes for set_reset.
	mov	al,GRAF_ENAB_SR
	out16	dx,ax

	mov	ah,DataROTFlag	      ;Set DataRotate register value
	mov	al,GRAF_DATA_ROT
	out16	dx,ax

; The bitmask register must be the last register written to the
; Graphics Controller so that it's address is still in the
; address register.

set_bit_mask:
	mov	ax,0FF00h+GRAF_BIT_MASK
	out16	dx,ax

	mov	al,MM_ALL		; enable all planes for writing.
	mov	dl,SEQ_DATA
	out	dx,al
	pop	ax
	ret
polyline_set_ega_for_one_pass	endp
page

;-------------------------Private-Routine-------------------------------;
; polyline_set_ega_for_two_pass
;
; polyline_set_ega_for_two_pass sets up the EGA for the four raster
; operations which take two passes of EGA memory.  these raster operations
; are all handled in a similar fashion: on the first pass we set the 
; necessary bits of the necessary planes to ones or zeros explicitly.
; the second pass we invert the destination bits for the remaining planes.
;
; Entry: None.
;
; Returns: None.
;
; Error Returns: None.
;
; Registers Destroyed: DL,flags.
;
; Registers Preserved: AX,DH.
;
; Calls: None
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; polyline_set_ega_for_two_pass
;
; {
;    // this routine sets up the EGA for the four raster operations which
;    // take two passes of EGA memory.  these raster operations are all
;    // handled in a similar fashion: on the first pass we set the necessary
;    // bits of the necessary planes to ones or zeros explicitly. the second
;    // pass we invert the destination bits for the remaining planes.
;
;    set set/reset register to PenANDFlag;
;
;    set TmpColor to the current pen or background color;
;    TmpColor ^= PenXORFlag;   		// invert the color if necessary.
;    TmpColor &= MM_ALL;
;
;    // enable only the planes that need it.
;
;    set enable set/reset register to TmpColor;
;
;    set data rotate register to DataROTFlag;
;
;    // the bitmask register must be the last register written to the 
;    // graphics controller so that its address is still in the
;    // address register.
;
;    enable all bits in bit mask register;
;
;    reset TmpColor to current pen or background color;
;    TmpColor ^= PenXORFlag;		// invert color if necessary.
;    
;    // enable only necessary planes for writing.
;
;    set sequencer data register to TmpColor;
;
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

polyline_set_ega_for_two_pass	proc	near

	push	ax
	mov	ah,PenANDFlag
	mov	al,GRAF_SET_RESET	;Write to set/reset register
	mov	dx,EGA_BASE + GRAF_ADDR
	out16	dx,ax

	mov	ah,TmpColor		; get the color.
	xor	ah,PenXORFlag		; invert color if necessary.
	and	ah,MM_ALL		; enable only the planes that need it.
	mov	al,GRAF_ENAB_SR
	out16	dx,ax

	mov	ah,DataROTFlag	        ;Set DataRotate register value
	mov	al,GRAF_DATA_ROT
	out16	dx,ax

; The bitmask register must be the last register written to the
; Graphics Controller so that it's address is still in the
; address register.

	mov	ax,0FF00h+GRAF_BIT_MASK
	out16	dx,ax

	mov	al,TmpColor		; get the color.
	xor	al,PenXORFlag		; invert color if necessary.
	and	al,MM_ALL		; enable only necessary planes for writing.
	mov	dl,SEQ_DATA
	out	dx,al
	pop	ax
	ret
polyline_set_ega_for_two_pass	endp
page

;-------------------------Private-Routine-------------------------------;
; ega_line_dispatch_routine
;
; ega_line_dispatch_routine handles both "simple" and "standard" lines
; while writing to the screen.  by simple it is meant that the line is
; horizontal, vertical or on a diagonal.  these cases can be handle much 
; faster than the standard cases, so they are broken out separately.  
; there are up to four passes set up.  two passes are possible for the
; four raster operations which cannot be done in one pass of EGA memory.
; the other twelve raster ops can be done in one pass.  styled lines are
; handled with 2 passes, one to draw the lines and one to draw the gaps.
;
; DDAcurrentProcTable points to one of four tables, ega_standard_routines,
; ega_simple_routines, styled_standard_routines or styled_simple_routines, depending on
; line style and whether it is simple or not.
;
; Entry: 
;	 if "standard" line to be drawn:
;
;		 AX = BitCount (bits per line segment).
;		 CX = HFirst (bits in first segment).
;		 SI = ErrTerm.
;		 DI = CaseFlags.
;
;	 else if "simple" line:
;
;		 CX = HFirst = total bits in the line.
;		 DI = CaseFlags.
;
; Returns: 
;	BL = rotating bitmask.
;	DS:DI = pointer to current byte in EGA memory.
;
; Error Returns: None.
;
; Registers Destroyed: AX,CX,DX,SI,flags.
;
; Registers Preserved: None.
;
; Calls: draw_x_axial_segments
;	 draw_y_axial_segments
;	 draw_diag_x_major_segments
;	 draw_diag_y_major_segments
;	 styled_draw_x_axial_segments
; 	 styled_draw_y_axial_segments
;	 styled_draw_diag_x_major_segments
;	 styled_draw_diag_y_major_segments
;	 polyline_set_ega_for_one_pass
;	 polyline_set_ega_for_two_pass
;
; History:
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; ega_line_dispatch_routine
;
; {
;standard_top_of_loop:
;    DX = EGA_BASE + GRAF_DATA;		// set up output port.
;    get index into DDAcurrentProcTable from CaseFlags;
;    call indexed line drawing routine;
;
;    if (raster op can be done in one pass)
;        jump to check for style;   	// don't do the second pass.
;
;    // if we get here we have one of the four raster operations which cannot
;    // be done in one pass of ega memory.  the first pass will have set the
;    // necessary bits in the necessary planes to either 1's or 0's explicitly.
;    // this pass will not the destination for the necessary bits in the
;    // necessary planes.  the ega has to be set up to not the destination,
;    // before the line is drawn.
;
;    TmpColor = current pen or background color;
;    TmpColor ^= PenXORFlag;		// invert color if necessary.
;    TmpColor = ! TmpColor;		// get the other planes.
;  
;    // enable the planes not enabled in first pass.
;
;    set sequencer data register to TmpColor;
;
;    DDAcount = DDAtmpcount;		// get bak intermediate counter.
;
;    set EGA to XOR mode;
;
;    enable all bits in bit mask register;
;
;    // reset style variables.
;
;    StyleLength = MAX_STYLE_ERR;
;    StyleFlags = StyleReset;
;
;    DX = EGA_BASE + GRAF_DATA;
;    CX = HFirst;
;    get index into DDAcurrentProcTable from CaseFlags;
;    call indexed line drawing routine.
;
;check_for_style:
;    if (solid line)
;        jump to ega_get_out;		// skip styled line stuff.
;
;    if (transparent mode)
;        jump to ega_get_out;		// skip gap drawing pass.
;
;    if (second pass already done)
;        jump to reset_the _pen_color;	// reset TmpColor to pen color.
;
;    // set up for second pass to draw gaps in styled line.
;
;    DDAcount = DDAtmpcount;  		// get back intermediate counter.
;    set TmpColor to background color;
;    set second pass flag;
;    StyleLength = MAX_STYLE_ERR;
;    StyleFlags = StyleReset;
;
;    StyledProc => styled_gap_pixel;	// point to gap output routine.
;    CX = HFirst;
;
;    if (raster op can be done in one pass)
;        polyline_set_ega_for_one_pass();
;
;    else
;        polyline_set_ega_for_two_pass();
;
;    jump to standard_top_of_loop;
;
;reset_the_pen_color:
;    // by the time we get here, we have made a pass or two to draw the
;    // line part of the styled line, then we set TmpColor to the background
;    // color and drew the gaps in the line.  we now reset TmpColor for when
;    // we draw the next line.
;
;    reset TmpColor to the pen color;	
;    StyledProc => styled_line_pixel;	// point to line output routine.
;
;    StyleLength = MAX_STYLE_ERR;
;    StyleFlags = StyleReset; 		// reset style information.
;    
;    if (raster op takes two passes)
;        polyline_set_ega_for_one_pass; // reset the EGA for one pass.
;
;ega_get_out:
;    return();
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

ega_line_dispatch_routine	proc	near

	mov	word ptr StyledProc,LineSegOFFSET styled_line_pixel

standard_top_of_loop:
	push	si			; save error term.
	mov	dx,StyleLength		; save style values for future passes.
	mov	TmpStyleLength,dx
	mov	dx,StyleFlags
	mov	TmpStyleFlags,dx
	mov	dx,EGA_BASE+GRAF_DATA
	and	di,CASE_TABLE_MASK	; mod 4.
	shl	di,1			; make word pointer.
	add	di,DDAcurrentProcTable
	call	cs:[di] 		; call the proper routine.
	pop	si			; restore error term.

	test	SingleFlag,SINGLE_OK	; is this one of the four ROPS?
	jnz	check_for_standard_style ; jump if not.

; if we get here we have one of the four raster operations which cannot
; be done in one pass of ega memory.  the first pass will have set the
; necessary bits in the necessary planes to either 1's or 0's explicitly.
; this pass will not the destination for the necessary bits in the
; necessary planes.  the ega has to be set up to not the destination,
; before the line is drawn.

do_second_pass:
	mov	al,TmpColor		; get the color.
	xor	al,PenXORFlag		; invert color if necessary.
	not	al			; enable the other planes.
	and	al,MM_ALL
	jz	check_for_standard_style ; no bits to XOR.

	mov	dx,DDAtmpcount		; get back intermediate counter.
	mov	DDAcount,dx

	mov	dx,EGA_BASE+SEQ_DATA
	out	dx,al

	mov	dl,GRAF_ADDR		; set to xor mode.
	mov	ax,DR_XOR * 256 + GRAF_DATA_ROT
	out16	dx,ax

	mov	ax,0FF00h+GRAF_BIT_MASK ; set up bitmask register.
	out16	dx,ax

	mov	di,TmpStyleLength	; reset style information.
	mov	StyleLength,di
	mov	di,TmpStyleFlags
	mov	StyleFlags,di

	push	si			; save error term.
	mov	dl,GRAF_DATA
	mov	cx,HFirst		
	mov	di,CaseFlags
	and	di,CASE_TABLE_MASK	; mod 4.
	shl	di,1			; make word pointer.
	add	di,DDAcurrentProcTable
	call	cs:[di] 		; call the proper routine.
	pop	si			; restore error term.

check_for_standard_style:
	cmp	CurStyle,0FFh
	je	standard_ega_get_out	; skip styled stuff if solid line.

	cmp	BackMode,OPAQUE 	; if the background mode is transparent,
	jne	standard_ega_get_out	; then do not make another pass to fill it in.

	test	SingleFlag,SECOND_PASS	; if the second pass has already been
	jnz	reset_the_pen_color	; made, then reset TmpColor to pen color.

; set up for second pass to draw the gaps of a styled line.

	mov	ax,DDAtmpcount
	mov	DDAcount,ax
	mov	al,BackColor.SPECIAL
	mov	TmpColor,al		; set TmpColor to background color.
	or	SingleFlag,SECOND_PASS	; indicate we are making second pass.
	mov	di,TmpStyleLength	; reset style information.
	mov	StyleLength,di
	mov	di,TmpStyleFlags
	mov	StyleFlags,di
	mov	di,CaseFlags
	mov	word ptr StyledProc,LineSegOFFSET styled_gap_pixel

ega_reset_hfirst:
	mov	cx,HFirst

	test	SingleFlag,SINGLE_OK
	jz	standard_styled_two_pass

; if we make this call, then we have a styled line using one of the twelve
; raster operations which can be done with one pass of ega memory.

	call	polyline_set_ega_for_one_pass
	jmp	standard_top_of_loop

; if we make this call, then we have a styled line using one of the four
; raster operations which take two passes of ega memory.

standard_styled_two_pass:
	call	polyline_set_ega_for_two_pass
	jmp	standard_top_of_loop

; by the time we get here, we have made a pass or two to draw the line part
; of the styled line, then we set TmpColor to the background color and drew
; the gaps in the line.  we now reset TmpColor for when we draw the next line.

reset_the_pen_color:
	mov	cl,CurPen.SPECIAL
	mov	TmpColor,cl		; reset TmpColor to pen color.

	and	SingleFlag,not SECOND_PASS

	test	SingleFlag,SINGLE_OK
	jz	standard_ega_get_out

; set up the ega for raster op which can be done in one pass of ega memory.

	call	polyline_set_ega_for_one_pass

standard_ega_get_out:
	ret

ega_line_dispatch_routine	endp
page

;-------------------------Private-Routine-------------------------------;
; bitmap_line_dispatch_routine
;
; bitmap_line_dispatch_routine handles both "simple" and "standard" lines
; while writing to a bitmap.  by simple it is meant that the line is
; horizontal, vertical or on a diagonal.  these cases can be handle much 
; faster than the standard cases, so they are broken out separately.  
; there are up to four passes set up.  one pass is made for each color 
; plane when drawing lines to a bitmap.  therfore, if writing to a monocrhome
; bitmap, only one pass is made.  styled lines are handled with 2 passes,
; one to draw the lines and one to draw the gaps.
;
; DDAcurrentProcTable points to one of four tables, bitmap_standard_routines,
; bitmap_simple_routines, styled_standard_routines or styled_simple_routines,
; depending on line style and whether it is simple or not.
;
; Entry: 
;	 if "standard" line to be drawn:
;
;		 AX = BitCount (bits in each line segment).
;		 CX = HFirst (bits in first line segment).
;		 SI = ErrTerm.
;		 DI = CaseFlags.
;
;	 else if "simple" line:
;
;		 CX = HFirst = total pixels in the line.
;		 DI = CaseFlags.
;
; Returns: 
;	BL = rotating bitmask.
;	DS:DI = pointer to current byte in EGA memory.
;
; Error Returns: None.
;
; Registers Destroyed: AX,CX,DX,SI,flags.
;
; Registers Preserved: None.
;
; Calls: bitmap_draw_x_axial_segments
;	 bitmap_draw_y_axial_segments
;	 bitmap_draw_diag_x_major_segments
;	 bitmap_draw_diag_y_major_segments
;	 styled_draw_x_axial_segments
; 	 styled_draw_y_axial_segments
;	 styled_draw_diag_x_major_segments
;	 styled_draw_diag_y_major_segments
;	 polyline_set_ega_for_one_pass
;	 polyline_set_ega_for_two_pass
;
; History:
;  Thu 30-Apr-1987 13:20:00	-by-	**** ******	    [******]
; Added huge bitmap handling.
;  Wed 08-Apr-1987 10:32:33	-by-	**** ******	    [******]
; Modified to draw all lines moving right.
;  Mon 23-Feb-1987 12:56:41	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;

;---------------------------Pseudo-Code---------------------------------;
; bitmap_line_dispatch_routine
; {
;bitmap_top_loop:
;    load the color into AH;
;
;    // with the bitmaps we can break the operation down into four categories.
;    // set some bits to zero, set some bits to one, xor some bits or do nothing.
;    // which operation to be used is determined by the raster operation, and
;    // whether or not the current bits in the current color plane are set or not.
;    // BitmapProc points to the correct procedure to call. for the do nothing
;    // case BitmapProc == 0, and the pass of the plane is skipped.
;
;    while (number of planes--)
;    {
;    // the color we will be drawing is loaded into AH.  by rotating AH with
;    // each plane we output, we determine whether that color plane is used
;    // in the current color we are drawing with.  for example, if the color
;    // is red, the the first color plane is used, and all others are not.
;    // depending on whether the current color plane is set or not, we will
;    // index the following procedure table differently.
;
;        rotate AH right 1 bit;		// see if color plane is set.
;
;        if (color plane is set)
;            index = DrawModeIndex / 4;
;        else
;            index = DrawModeIndex mod 4;
;
;        // set up proper bitmap output routine.
;
;	 BitmapProc = contents of LineSegOFFSET bitmap_procedure_table + index;
;
;        if (BitmapProc	== 0)
;            jump to bottom_of_loop;	// do nothing this time.
;
;        CX = HFirst;
;        get index into DDAcurrentProcTable from CaseFlags;
;        call indexed line drawing procedure;
;
;bottom_of_loop:
;        // set up for next color plane.
;
;        OFF_CurByte += NextPlane;	// point to next plane.
;        DDAcount = DDAtmpcount;	// get back intermediate counter.
;        StyleLength = MAX_STYLE_ERR;	// reset style information.
;        StyleFlags = StyleReset;
;    }
;
;    if (solid line)
;        jump to done_bitmap;		// skip style line stuff.
;
;    if (transparent mode)
;        jump to done_bitmap;		// dont fill in the gaps.
;
;    if (second pass has already been made)
;        jump to reset_color_of_pen;
;
;    // set up for second pass to draw gaps in styled line.
;
;    set TmpColor to background color;
;    set second pass flag;
;    StyleLength = MAX_STYLE_ERR;	// reset style information.
;    StyleFlags = StyleReset;
;    Off_CurByte = TmpCurByte;		// point back to start of line.
;
;    // point to gap drawing output routine.
;
;    StyledProc => styled_bitmap_gap_pixel;
;
;    jump to bitmap_top_loop		// go draw the gaps.
;
;    // by the time we get here, we have made a pass to draw the line part
;    // of the styled line, then we set TmpColor to the background color
;    // and drew the gaps in the line.  we now reset TmpColor for when we
;    // draw the next line.
;
;reset_color_of_pen:
;    reset TmpColor to pen color;
;
;    // point to line drawing output routine.
;
;    StyledProc => styled_bitmap_line_pixel;
;
;    StyleLength = MAX_STYLE_ERR;	// reset style information.
;    StyleFlags = StyleReset;
;    clear second pass flag;
;
;done_bitmap:
;    Seg_CurByte = DS;			// set ending segment.
;    if (moving left)
;        SEG_CurByte = TmpSegment;	// reset segment.
;
;    // if writing to a color bitmap, the destination pointer will currently
;    // be pointing into the last color plane.  reset the pointer to point
;    // into the first color plane for the next pass.
;
;    DI -= (NumPlanes - 1) * NextPlane;
;
;    return();
; }
;-----------------------------------------------------------------------;

bitmap_line_dispatch_routine	proc	near

	mov	word ptr StyledProc,LineSegOFFSET styled_bitmap_line_pixel

	mov	dx,StyleLength		; save style values for future passes.
	mov	TmpStyleLength,dx
	mov	dx,StyleFlags
	mov	TmpStyleFlags,dx

standard_bitmap_top_loop:
	xor	ch,ch
	mov	cl,NumPlanes		; set up for loop.
	mov	ah,TmpColor		; get the color.

; with the bitmaps we can break the operation down into four categories.
; set some bits to zero, set some bits to one, xor some bits or do nothing.
; which operation to be used is determined by the raster operation, and
; whether or not the current bits in the current color plane are set or not.
; BitmapProc points to the correct procedure to call. for the do nothing
; case BitmapProc == 0, and the pass of the plane is skipped.

top_of_standard_loop:
	mov	di,DrawModeIndex	; get the raster op.

; the color we will be drawing is loaded into AH.  by rotating AH with
; each plane we output, we determine whether that color plane is used
; in the current color we are drawing with.  for example, if the color
; is red, the the first color plane is used, and all others are not.
; depending on whether the current color plane is set or not, we will
; index the following procedure table differently.

	ror	ah,1			; see if the color plane is set.
	jc	standard_plane_is_set	; jump if so.

	and	di,CASE_TABLE_MASK	; mod 4.
	jmp	short set_the_standard_proc

standard_plane_is_set:
	shiftr	di,2			; divide by 4.  CANNOT be combined 
					; with following shl.
set_the_standard_proc:
	shl	di,1			; make a word ptr.

	mov	bx,LineSegOFFSET bitmap_procedure_table
	mov	bx,cs:[bx][di]		; add index into table.
	mov	word ptr BitmapProc,bx	; set up the procedure to call.

	or	bx,bx
	jz	bottom_of_standard_loop ; do nothing

	push	ax			; save color plane info + bitmask.
	push	cx			; save loop counter.
	push	si			; save error term.
	mov	dx,TmpStyleLength	; reset style information.
	mov	StyleLength,dx
	mov	dx,TmpStyleFlags
	mov	StyleFlags,dx
	mov	cx,HFirst
	mov	di,CaseFlags
	and	di,CASE_TABLE_MASK	; mod 4.
	shl	di,1			; make word pointer.
	add	di,DDAcurrentProcTable
	call	cs:[di] 		; call the appropriate procedure.
	pop	si			; restore error term.
	pop	cx			; restore loop counter.
	pop	ax			; restore color plane info + bitmask.

bottom_of_standard_loop:
	mov	dx,NextPlane		; point to next plane.
	add	off_CurByte,dx
	mov	dx,DDAtmpcount		; reset DDAcount
	mov	DDAcount,dx
	loop	top_of_standard_loop

	cmp	CurStyle,0FFh
	je	done_standard_bitmap	; skip styled stuff if solid line.

	cmp	BackMode,OPAQUE 	; if the background mode is transparent,
	jne	done_standard_bitmap	; then do not make another pass to fill it in.

	test	SingleFlag,SECOND_PASS	; if the second pass has already been
	jnz	reset_color_of_pen	; made, then reset TmpColor to pen color.

; set up for second pass to draw the gaps of a styled line.

	mov	dl,BackColor.SPECIAL
	mov	TmpColor,dl		; set TmpColor to background color.
	or	SingleFlag,SECOND_PASS	; indicate we are making second pass.
	mov	dx,TmpCurByte
	mov	off_CurByte,dx		; point back at start of line.
	mov	word ptr StyledProc,LineSegOFFSET styled_bitmap_gap_pixel

standard_next_plane:
	jmp	standard_bitmap_top_loop

; by the time we get here, we have made a pass to draw the line part
; of the styled line, then we set TmpColor to the background color and drew
; the gaps in the line.  we now reset TmpColor for when we draw the next line.

reset_color_of_pen:
	mov	al,CurPen.SPECIAL
	mov	TmpColor,al		; reset TmpColor to pen color.

	and	SingleFlag,not SECOND_PASS

; if writing to a color bitmap, the destination pointer will currently be
; pointing into the last color plane.  reset the pointer to point into the
; first color plane for the next pass.

done_standard_bitmap:
	test	DeviceFlags,DEST_IS_HUGE
	jz	dest_aint_huge		; skip next section if not huge bitmap.

	mov	dx,ds
	mov	seg_CurByte,dx		; update current segment.

dest_aint_huge:
	xor	ah,ah			; DI = DI - (NumPlanes-1)*NextPlane.
	mov	al,NumPlanes		
	dec	ax
	mul	NextPlane
	sub	di,ax

	ret

bitmap_line_dispatch_routine	endp


;--------------------------Public-Routine-------------------------------;
; update_curbyte
;
; Update CurByte to point to a given x,y coordinate in a bitmap.
;
; Entry: 
;	AX = Y coordinate.
;	DX = X coordinate.
;
; Returns: 
;	CurByte => current destination byte.
;	BL = rotating bit mask
;	DI = current destination byte offset
;
; Error Returns: None.
;
; Registers Destroyed: None.
;
; Registers Preserved: AX,DX
;
; Calls:
;	None.
;
; History:
;  Mon 18-May-1987 03:11:00	-by-	**** ******	    [******]
; Wrote it.
;-----------------------------------------------------------------------;


	assumes	ds,Data
	assumes	es,nothing

update_curbyte	proc	near

	push	ax
	push	dx

; set up the pointer to the current byte in the memory bitmap.

	push	dx			; save x coordinate.
	test	DeviceFlags,DEST_IS_HUGE
	jz	uc_set_up_small_only	; skip next section if small bitmap.

; we have a huge bitmap.  compute which segment the Y coordinate is in.
; set up SegmentNum to show which segment number this point is in.
; set SEG_CurByte to the segment which the point is in.

	xor	dx,dx			; zero out segment bias.
	mov	SegmentNum,dx		; zero out segment counter.

uc_top_segment_loop:
	add	dx,SegIndex		; show in next segment.
	inc	SegmentNum		
	sub	ax,ScansSeg		; see if it is this segment.
	jnc	uc_top_segment_loop	; not in this segment, try next one.
	add	ax,ScansSeg		; restore proper values.
	dec	SegmentNum
	sub	dx,SegIndex

	add	dx,BitmapSegment
	mov	seg_CurByte,dx		; point to proper segment.

uc_set_up_small_only:
	mul	NextScan
	add	ax,BitmapOffset		; point to start of current scanline.

	pop	dx			; restore x coordinate.

	mov	di,dx			;Compute offset within scanline
	shiftr	di,3
	add	di,ax			;(DI) = offset of first byte
	mov	off_CurByte,di

	mov	bx,dx
	and	bx,PARTIAL_BYTE_MASK
	mov	bl,LineSeg_rot_bit_tbl[bx]

	pop	dx
	pop	ax

	ret

update_curbyte	endp

sEnd	LineSeg

ifdef	PUBDEFS
	include polyline.pub
endif

	end

