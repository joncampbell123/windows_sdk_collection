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
;                    			                                      ;
;                           DO_POLYLINES                                      ;
;                           ------------			              ;
;  									      ;
;  Given a set of points, draws a set of polylines connecting adjoining points;
;  This routine handles output to EGA as well as to BITMAPS (small or huge)   ;
;								              ;
;  Breshenham's Run Length Slice algorithm is used to draw the lines. Though  ;
;  the algorithm is not derived here, the steps necessary in implementation   ;
;  are explained later.					                      ;
;							                      ;
;  The way the algorithm works can be summarised below:			      ;
;									      ;
;               . To draw any line we have to set on a set of  pixels which   ;
;                 fall on the line or are closest to it.In cases where a line ;
;                 passes exactly between two pixels, we resolve the tie by    ;
;                 setting on the lower pixel			              ;
;								              ;
;               . As a result of the above approximation, a line will actually;
;                 be drawn as a series of steps.			      ;
;								              ;
;               . At any particular pixel position, the next pixel to set on  ;
;                 depends on the direction of progress and the slope of the   ;
;                 line, but because of the discrete movements, it can be the  ;
;                 next pixel in one of the following 8 directions:            ;
;							                      ;
;		              . Positive X / Negative X			      ;
;                             . Positive Y / Negative Y		              ;
;                             . Along one of the 4 diagonals in 1st, 2nd, 3rd ;
;                               or 4th quadrants.		              ;
;							                      ;
;               . Depending upon the slope of the line we will actually have  ;
;                 a series of pixels set on in one of the 8 directions, then  ;
;                 a move of 1 pixel in a second direction (one of above 8)    ;
;                 followed by another run of pixels in the first direction    ;
;                 , a move 1 pixel in the second direction and so on ...      ;
;							                      ;
;		. A line thus will have an AXIAL direction, along which most  ;
;                 of the pixels are set on and a DIAGONAL (to the axial) dir  ;
;                 which separates one run of pixel on the AXIAL direction     ;
;                 from the other				              ;
;								              ;
;               . The AXIAL and DIAGONAL directions can be one of the above   ;
;                 8 directions, the exact directions can be calculated from   ;
;                 slope of the line and the direction of progress.            ;
;									      ;
;               . To facilitate the calculation of the run lengths along the  ;
;                 AXIAL direction, we can rotate the line till the AXIAL dir  ;
;                 becomes the POSITIVE X axis and the DIAGONAL direction beco-;
;                 -mes the diagonal in the first quadrant.                    ;
;						                              ;
;               . The steps to do the above rotation are:		      ;
;						                              ;
;		              . Translate to origin. Let the other end point  ;
;                               be at (Xf,Yf)				      ;
;	                      . Get the line into the first quadrant. So the  ;
;                               end point will now be (|Xf|,|Yf|)             ;
;                             . Get it below the slope=1 line. That is, inter-;
;                               -change the abscissa and ordinate of the end  ;
;                               point to ensure that the abscissa is >= the   ;
;                               ordinate. Let end point now be at (X,Y)       ;
;                             . If the line is above the slope=1/2 line, get  ;
;                               it below by making the end point to be (X,X-Y);
;               							      ;
;               . The line thus constructed will be in the same relative posi-;
;                 -tion wrt the +X axis and the 1st. quadrant diagonal, as the;
;                 original line was wrt it's AXIAL and DIAGONAL directions.   ;
;			                                                      ;
;               . If the pseudo line runs from (0,0) to (X,Y), we have to cal-;
;                 -culate the number of pixels to set on, on each of the hori-;
;                 -zontal scan lines at y=0,1,2,...,Y 			      ;
;                							      ;
;               . Consider scan line at y=i 0<i<Y.                            ;
;                 If the line intersects the scan line at y=i-1/2 at x, then  ;
;                 the first pixel to set on, on the ith scan line is at       ;
;                 floor(x)+1 and the last is at floor(x + s) where s is the   ;
;                 inverse slope.			                      ;
;							                      ;
;               . The run length on the ith scan line is thus:		      ;
;                             floor(x+s) - floor(x)		              ;
;								              ;
;               . The idea behind the algorithm is to calculate these run len-;
;                 -gths and does so by using the run length on the previous   ;
;                 line and an error term.			              ;
;					                                      ;
;               . We finally get a series of run lengths, H0 on i=0, scan line;
;                 HY on the last scan line, and Hi on the intermediate ones.  ;
;						                              ;
;		. The pseudo line was just used to calculate the run lengths. ;
;                 While drawing the line, we start at the original starting   ;
;                 point (not at origin) the first run of H0 is in the actual  ;
;                 AXIAL direction, a move of 1 pixel in the DIAGONAL dir,and  ;
;                 so on, till the last run of HY pixel on the AXIAL direction ;
;                 takes us to the actual end point of the line.               ;
;							                      ;
;									      ;
;  Certain manipulations are made with the end points to make the code simpler;
;  These are:							              ;
;									      ;
;               . All lines will be drawn towards positive X axis, to do this ;
;                 we will swap the initial and final coordinates if necessary ;
;                 This will elliminate three directions of movement which inv-;
;                 -olve the negative X axis, leaving us with 5 directions.    ;
;    						                              ;
;               . The code for drawing/moving along the diagonal or vertical  ;
;                 directions require the distance in bytes between one scan   ;
;                 line and the next. If we call the routine which draws in a  ;
;                 direction involving the positive Y axis, with the negative  ;
;                 of the scan line offset, it would actually draw in a the    ;
;                 corresponding direction involving negative Y                ;
;				                                              ;
;               . With the above two manipulations, we need to implement rout-;
;		  -ines to draw in 3 basic directions, Positive X, Positive Y ;
;                 and the diagonal in the first quadrant.		      ;
;							                      ;
;               . Routines to move 1 pixel in the DIAGONAL direction are made ;
;                 separate as we will not set on any pixel during these moves.;
;                 We will thus have 3 move routines too, as the DIAGONAL dire-;
;                 -ction of the line could agagin be one of the 5 basic ones. ;
;								              ;
;		. There are 4 sets of draw routines depending on the device & ;
;                 the type of the line. These are:			      ;
;		              . Solid Lines to EGA		              ;
;                             . Styled Lines to EGA                           ;
;                             . Solid Lines to BITMAPS (huge and small)       ;
;                             . Styled Lines to BITMAPS (huge and small)      ;
;                 Routines are made separate as the organization of memory and;
;                 the method used to process the pixels are different.        ;
;				                                              ;
;	        . There are two sets of MOVE routines, one for EGA and the ot-;
;                 -her for BITMAPS.		                              ;
;					                                      ;
;	        . Styled lines are drawn in two passes, the second pass is to ;
;                 draw the gaps for OPAQUE background modes.                  ;
;					                                      ;
;               . Windows support 16 Raster Operations for combining the pen  ;
;                 color and the color of the destination, these are implement-;
;                 -ed and are explained below.			              ;
;									      ;
;  For lines which are originally below the X axis, the metric of rounding    ;
;  down to resolve ties for the pseudo line will actually cause the tie cases ;
;  to be rounded to the pixel above the original line. Like wise for lines    ;
;  originally to the left of the slope=1 line, tie cases will be rounded to   ;
;  pixel on the right instead of the one on the left. This is inconsistent    ;
;							                      ;
;  To take care of this the metric for the above flipped psudo-lines should   ;
;  be to round up (that is take the floor value and not the ceiling). But we  ;
;  can continue with the original metric if we shift the pseudo line in these ;
;  cases by a minute amount to the left, so as to bias the ties infavour of   ;
;  required pixel but small enough so as to not affect the non-tie cases.     ;
;  The amount of shift can be shown to be (1/2Y) where the Y is the ordinate  ;
;  of the pseudo lines end point.			                      ;
;									      ;
;-----------------------------------------------------------------------------;



comment ~

                       Programing the EGA REGISTERS:
	               -----------------------------

 12 of the 16 ROPs can be completed in one pass. Let us represent the existing
 pixel value of a piexl to be operated on by D and the value of the pen as P.
 The ROPs which invert D and then combines it with P (or inverted P) cannot be
 completed in one pass. 

	                   EGA ONE PASS ROPs
                           -----------------

 12 of the ROP codes actually combine D with P or !P or a constant 0 or 1.These
 can all be done in one pass. The combination function can be one of four:
                             AND,XOR,OR or SET
 one of the operands of these function is D which is the value already existing
 to get the other operand it is enough that we mask P with PenANDFlag and then
 XOR it with PenXORFlag. These two flags are chosen appropriately so that it   
 gives us one of the following four values:
              
                             P   ----  ie, PenANDFlag = 0fh, PenXORFlag = 00h
                            !P   ----                 = 0fh             = 0fh
                          0000   ----                 = 00h             = 00h
                          1111   ----                 = 00h             = 0fh

  Thus for one pass ROPs, the EGA is programmed as follows:

                      . SET BIT MASK REGISTER to enable required bit
                      . SET ENABLE SET/RESET REGISTER to enable all S/R bits
                      . SET S/R register to P combined with AND & XOR as above
                      . SET DATA ROTATE REGISTER to proper combine function

  Note that with the above setting, the CPU value in a write does not affect\
  the final pixel value (as the S/R register value combines with D)

                          EGA TWO PASS ROPs
                          -----------------

  4 of the ROPs actually operate as follows:

                      . some of the bits in the pixel value are set to either
                        0 or 1. In some cases these are the bits where the 
                        Pen color has a 0, and in other cases these bits in
                        the Pen color have a 1.
                      . For the remaining bits, the existing color is inverted

  Conviniently the two passes over the EGA that are required will do precisely
  the above steps one after the other. We now require to enable some of the
  planes depending on the color value and set the S/R register to 0 or 1, 
  depending upon the value that is to be set. In the second pass the remaining
  planes are enabled and the S/R register is set to 1 and combine function
  set to XOR (to invert the existing bit values in the enabled planes). Note
  in the first pass the combine function is always set to SET.

         First Pass:
                      . XOR P with PenXORFlag use this to write to the
                        ENABLE S/R REGISTER and SEQUENCER MAP MASK REGISTER
                        (the PenXORFlag is chosen appropriately to ensure that
                         the planes that are enabled now are the ones that
                         have to be set to 0 or 1)
                      . Set the S/R REGISTER to value of PenANDFlag , this will
                        be 0 or 1 depending on whether bits are to be set on
                        or off.
                      . DATA ROTATE REGISTER is set to DataROTFlag (which is 
                        always = SET in the first pass)
                      . The drawing routines enable required pixels

         Second Pass:
                      . P is XORed with PenXORFlag and the result is then 
                        Inverted to select the planes that were disabled in
                        the first pass.
                        The above value programs the SEQUENCER MAP MASK REG 
                      . DATA ROTATE REGISTER is set to XOR combine mode
                      . Note that the ENABLE S/R resgieter is not touched,
                        but it has the complement of the value in the SEQ
                        MAP MASK REGISTER and hence the S/R register actually
                        does not participate in the second pass, but the CPU
                        used to write will be 0ffh and hence with the XOR    
                        function this will invert the required bits.

        Note that in the second pass the ENABLE S/R register is not touched
        but the SEQUENCER MAP MASK register is inverted this effectively 
        disables the S/R register and in the second pass the value CPU
        data register will be used to alter the pixel values. 

  Actually the address of the BIT MASK REGISTER will be left in the GRAPHICS
  controller ADDRESS register, and the the draw routines will mask the required
  bits.

end comment ~
                     
;	The function below will perform private stack checking. In order for
;       the checking to occur, two symbols must be defined prior to the inclu- 
;       -sion of CMACROS.INC. ?CHKSTK must be defined if the CMACROS are to
;       perform stack checking on procedures with local parameters. ?CHKSTKPROC
;       must be defined if private stack checking will be used.
;								
;	The actual body of ?CHKSTKPROC is included in MACROS.INC
;
;	Since the stack-checking code is designed to work quickly
;	as a near subroutine, and normally resides in segment Code, a
;	duplicate is included in the segment _LINES.  To reach this,
;	the macro ?CHKSTKNAME is defined.

?win = 1
?plm = 1

?CHKSTK = 1
?CHKSTKPROC	macro
		endm

?CHKSTKNAME	macro
	call	LineSeg_check_stack
		endm


incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions

FIREWALLS = 0

.xlist
	include lines.inc
	include	mflags.inc
	include ega.inc
        include cmacros.inc
	include egamem.inc
	include gdidefs.inc
	include macros.inc
	include	polyline.inc
	include	display.inc
	include njmp.inc
.list

??_out polyline
	
	public	Clip_Line
	public	ega_line_dispatch
	public	Draw_Line
	public	mask_needed
	public	calculate_mask
	public	Draw_Engine
	public	find_out_dda
	public	tests_done
	public	do_another_pass
	public	do_the_last_scan
	public	all_passes_done
	public	style_draw_pass
	public	Positive_X
	public	Negative_Y
	public	Diagonal_4Q
	public	St_Positive_X
	public	St_Negative_Y
	public	St_Diagonal_4Q
	public	Bm_Positive_X
	public	Bm_Negative_Y
	public	Bm_Diagonal_4Q
	public	StBm_Positive_X
	public	StBm_Negative_Y
	public	StBm_Diagonal_4Q


	externA SCREEN_W_BYTES
	externA ScreenSelector
	externA	SCREEN_WIDTH
	externA	SCREEN_HEIGHT
ifdef PALETTES
	externB PaletteModified
endif

	externFP LineSeg_check_stack

ifdef PALETTES
	externFP TranslateBrush		;'on-the-fly' translation of brush
	externFP TranslatePen		;'on-the-fly' translation of pen
	externFP TranslateTextColor	;'on-the-fly' translation of textcol
endif
	
ifdef	EXCLUSION
	externFP exclude_far   		; exclude area from screen.
	externFP unexclude_far 		; clear excluded area.
endif
	      
	


sBegin		Data

SINGLE_OK	equ	00000001b	; indicates single pass ROPs
SECOND_PASS	equ	00000010b	; indicates double pass ROPs
TYPE_IS_DEV     equ     10000000b	; indicates drawing device is screen
TYPE_IS_HUGE    equ     01000000b	; drawing device is huge bitmap
TYPE_IS_BITMAP  equ     00000010b	; drawing device is small bitmap
TYPE_IS_STYLED  equ     00000001b	; indicates lines are styled
EGA_GRAPH	equ	3ceh		; address of EGA GRAPHICS CONTROLLER
BYTES_PER_LINE  equ     SCREEN_W_BYTES  ; bytes per scan line
wDistHoriz	equ	40h		; Horiz. scale factor for style mask
wDistVert	equ	55h		; Vert. scale factor for style mask
OPAQUE          equ	2		; indicates OPAQUE background mode
LEFT_XCHG	equ	0001h		; forced left to right
XY_XCHG		equ	0002h		; X and Y interchanged
HALF_FLIP	equ	0004h		; half flip done
Y_FLIP		equ	0008h		; vertical flip done
VERTICAL_FLIP	equ	0100h		; HALF_FLIP XOR Y_FLIP
CLIP_TOBE_DONE	equ	0001h		; clipping done by driver
PS_SOLID	equ	0		; solid lines
PS_NULL		equ	5		; null lines

externB		enabled_flag		; non zero if output to screen allowed

sEnd		Data

createSeg	_LINES,LineSeg,word,public,CODE
sBegin		LineSeg

assumes		cs,LineSeg



	      
; the following stores the address of the draw routines for solid EGA lines

DrawTable_solid_ega     dw	offset Positive_X
			dw	offset Negative_Y
			dw	offset Diagonal_4Q
			dw	offset Diagonal_4Q

; Draw routine addresses for styled lines to EGA


DrawTable_styled_ega    dw	offset St_Positive_X
			dw	offset St_Negative_Y
			dw	offset St_Diagonal_4Q
			dw	offset St_Diagonal_4Q


; Draw routine addresses for solid lines to BITMAPS

DrawTable_solid_bm      dw      offset Bm_Positive_X
			dw	offset Bm_Negative_Y
			dw	offset Bm_Diagonal_4Q
			dw	offset Bm_Diagonal_4Q

; Draw routine addresses for styled lines to BITMAPS 

DrawTable_styled_bm     dw	offset StBm_Positive_X
			dw	offset StBm_Negative_Y
			dw	offset StBm_Diagonal_4Q
			dw	offset StBm_Diagonal_4Q

; AND/XOR mask definitions for ROP codes on BITMAPS

BitmapMaskTable		dw	000ffh	 	; set to zero
		        dw	0ff00h		; invert destination
			dw	00000h		; leave alone
			dw	0ffffh		; set to one

; Mask values for STYLED lines

style_table 	label	byte
	    	db	11111111b,11111111b	; solid line
	    	db	11100111b,11100111b	; dashed
	    	db	10101010b,01010101b	; dotted
	    	db	11100100b,00100111b	; dot_dash
	    	db	11101010b,01010111b	; dash_dot_dot

.xlist
include drawmod2.inc    ; includes the AND/XOR flags and combine functions
		        ; as well as the no of passes required for EGA ROPs
.list


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
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing

cProc	do_polylines,<FAR,PUBLIC,WIN,PASCAL>,<es,si,di>


	parmD	lp_dst_dev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lp_points		;--> to a set of points
	parmD	lp_phys_pen		;--> to physical pen
	parmD	lp_phys_brush		;--> to physical brush
	parmD	lp_draw_mode		;--> to a Drawing mode
	parmD	lp_clip_rect		;--> to a clipping rectange if <> 0


	localW	iWhichDDA		; index for the DDA to use
	localW	selHugeDelta		; next seg for huge bitmaps
	localW	cbHugeScanDelta		; offset on spanning a huge segment
	localB	BackColor		; background color
	localW	DrawModeIndex		; ROP code
        localB	PenANDFlag		; AND mask for EGA ROP codes
        localB  PenXORFlag		; XOR mask for EGA ROP codesP
        localB  DataROTFlag		; combine function for EGA ROP codes
        localB  SingleFlag		; no of passes required for EGA
        localB  TmpColor		; pen color
	localB	DeviceFlags		; identifies type of device

; following params are necessary for BITMAPS

	localW	BitmapSegment  		; start segment for small/huge maps
	localW  BitmapOffset            ; offset of start of map
	localW	NextScan		; offset to next scan line in map
	localW	wScans			; no of scan lines in huge map segment
	localB	NumPlanes		; no of color planes in map
	localW	NextPlane		; offset to next color plane	
	localW	FillBytes		; filler bytes at end of huge segment
	localW	NextSegOff		; offset to next hugemap segment

; following variables are used for styled lines

        localW  StyleFlags		; HIWORD has background mode,LO -- mask
        localB  BackMode		; back ground mode

; following variables are used during the draw routines

	localW	StartDDAAddr		; adderess of the start DDA
	localW	S1	      		; stores principle dir draw
        localW  S1_Move			; routine which handles S1 move
        localD  SurfaceStart            ; surface startt pointer
	localD	SurfaceAddress		; pointer to start of draw
        localW  TempCurByte             ; a copy of CurByte
        localW  TempCurByteSty          ; one more saved for style pass
        localB  TempCurBit              ; a copy of rotating bit mask
	localW	ScansLeftFromBot	; # of scan lines from bottom
	localW	wTmpScan		; a copy of above
	localB  ROPcolor		; temp value of pen color
        localW  wBitmapROP              ; XOR,AND mask for bitmaps
	localB	NumPasses		; # of passes
        localB  TempNumPass             ; saved value of # of passes
	localW	pass_prelude		; address pf prelude proc
	localW  style_pass_init         ; ega init tobe called before
				        ; style pass
        localB	StylePass		; style gap pass indicator
        localB  bStyleError             ; style error term
        localB  bTempStyleError         ; saves the error term
        localB  bStyleMask              ; rotating style mask
        localB  bTempStyleMask          ; saves the style mask
        localB  SaveColor               ; saves the pen color

	localW	StartX			; starting point abscissa
	localW	StartY			; starting point ordinate
	localW	Xa			; start clip point X
	localW	Ya			; start clip point Y
	localW	Xb			; end clip point X
	localW	Yb			; end clip point Y
	localW	f1			; style error first multiplier
	localW	f2			; style error second multiplier
	localW	wStyleCounter		; saved style counter
	localW	wStyleMask		; forward and reverse style masks
	localW	wFracS			; keeps frac(s)
	localW	cm			; keeps m = floor(s)
	localW	wError			; error term
	localW	Ca			; first slice length
	localW	Cb			; last slice length
	localW	XformFlags		; records transformations done
	localW	cdy			; stores pseudo line minor coord
	localW	cScanMove		; offset to next scan line
	localW	cScanStep		; sidestep error add factor
	localB	LineStyle		; value of line style
	localW	cScans			; no of scans lines for pseudo line
	localW	cTempScans		; save for later passes
	localW	wDistDiag		; style error add for diag draw
	localW	ClipFlag		; clip to be done indicator
	localW	SideStep		; side step contribution to error
	localB	cRotation		; no of bits to rotate on sidestep
	localW	WorkErrTerm		; work variable for error term

; the following variables define the clipping rectangle
	
	localW	X1			; left margin
	localW	X2			; right margin
	localW	Y1			; bottom margin
	localW	Y2			; top margin

cBegin

	jnc	stack_space_ok
	jmp	jump_error_exit		; exit with error on stack overflow
stack_space_ok:
	cld

;----------------------------------------------------------------------------;
; if palette manager is supported do the on-the-fly object translations now  ;
;----------------------------------------------------------------------------;

ifdef PALETTES
	cmp	PaletteModified,0ffh	; was the palette modified ?
	jnz	no_translation_needed	; no
	push	ds			; save own ds
	lds	si,lp_dst_dev		; the device information block
	lodsw				; get the device type
	pop	ds			; get back own segment
	or	ax,ax			; is the target physical device ?
	jz	no_translation_needed	; no!, so no translation is needed

	arg	lp_phys_brush
	cCall	TranslateBrush		;translate the brush
	mov	seg_lp_phys_brush,dx
	mov	off_lp_phys_brush,ax	;load the local brush pointer

	arg	lp_phys_pen
	cCall	TranslatePen		;translate the pen
	mov	seg_lp_phys_pen,dx
	mov	off_lp_phys_pen,ax	;load the local pen pointer

	arg	lp_draw_mode
	cCall	TranslateTextColor	;translate foreground/background cols
	mov	seg_lp_draw_mode,dx
	mov	off_lp_draw_mode,ax	;load the local pen pointer
no_translation_needed:

endif

;----------------------------------------------------------------------------;

	mov	cl,enabled_flag		; whether screen is enabled or not
	mov	ClipFlag,0
	lds	bx,lp_clip_rect		; DS:BX points to clip rectangle
	assumes	ds,nothing
	mov	ax,ds
	or	ax,bx			; NULL pointer means no clipping
	jz	do_no_clipping
	mov	ax,[bx].left
	mov	X1,ax			; the left margin
	mov	ax,[bx].top
	mov	Y1,ax			; for us this is bottom margin
	mov	ax,[bx].right		
	mov	X2,ax			; the right margin
	mov	ax,[bx].bottom
	mov	Y2,ax			; for us this is the top margin
	mov	ClipFlag,1		; turn on clipping
do_no_clipping:
	mov	al,cl			; get the enabled flag into al
	lds	si,lp_dst_dev		; DS:SI points to device structure
	assumes	ds,nothing
	mov	di,word ptr [si].bmBits+2
	mov	seg_SurfaceStart,di	; save slector
	mov	cx,[si].bmType		; get the device type
	mov	wScans,0		; scans per seg = 0
;	mov	NextSegOff,0		; reset
;	mov	FillBytes,0		; reset
	mov	off_SurfaceStart,0	; load the start pointer
	jcxz	get_bitmap_info 	; calculate params for bitmaps
	or	al,al			; is the screen enabled
	jz	relay_error_exit		; no, exit with error
	mov	DeviceFlags,TYPE_IS_DEV ; lines to be drawn on EGA
	

; take care of the exclusion area on the screen. This has to do with the mouse
; pointer, so we will not wory about this when we have a memory bitmap

ifdef	EXCLUSION
	xor	cx,cx			; assume entire screen
	mov	dx,cx
	mov	di,SCREEN_HEIGHT-1
	mov	si,SCREEN_WIDTH-1

	test	ClipFlag,CLIP_TOBE_DONE ; non zero clipping rectangle ?
	jz	polyline_exclude_scan   ; no clipping to be done

	mov	cx,X1			; set exclusion to passed rectangle
	mov	dx,Y1			; for exclude scan this is top
	mov	si,X2			; right margin
	mov	di,Y2			; for exclude scan this is bottom

polyline_exclude_scan:
	call	exclude_far		; exclude the scan from the screen
endif

	jmp	load_color_info


relay_error_exit:

	jmp	error_exit

get_bitmap_info:

	mov	DeviceFlags,TYPE_IS_BITMAP  ; assume a small bitmap
	mov	cl,[si].bmPlanes	    ; get the no of color planes
	mov	NumPlanes,cl		    ; save it
	mov	TempNumPass,cl		    ; save a copy of it
	
	mov	di,word ptr [si].bmWidthPlanes
	mov	NextPlane,di		    ; no of bytes in one plane
	
	mov	di,word ptr [si].bmWidthBytes
	mov	NextScan,di		    ; # of bytes per scan line
	
	mov	di,word ptr [si].bmBits+2
	mov	seg_SurfaceStart,di	    ;save it
	mov	di,word ptr [si].bmBits
	mov	off_SurfaceStart,di	    ;offset of the start of the map

; anticipating small bitmaps we will initialize some of the variables

	mov	wScans,0		    ; scans per seg = 0
	mov	NextSegOff,0		    ; reset
	mov	FillBytes,0		    ; reset
	page

;------------------------------------------------------------------------------;
; Small and Huge BITMAPs now have the same interleaved format for scan lines . ;
; This means that the offsets to the next scan line and to the next plane will ;
; be identical for them. However, since the small maps are < 64k, the segment  ;
; calculations will not be necessary, the control for the small maps will      ;
; bypass the segment calculation code and merge in with the code for huge maps ;
;------------------------------------------------------------------------------;

	mov	cx,[si].bmSegmentIndex      ; is it a huge bitmap
	jcxz	merge_point_for_small       ; no, skip the segment calcs

; cx has the segment offset to the next huge bitmap segment

	mov	NextSegOff,cx		    ; save the segment offset
	or	DeviceFlags,TYPE_IS_HUGE    ; set the indicator for huge maps
		
	mov	cx,[si].bmFillBytes   	    ; no of filler bytes at the end
	mov	FillBytes,cx
	mov	cx,[si].bmScanSegment	    
	mov	wScans,cx		    ; no of scan lines per segment
	

; bit maps are stored in groups of scan lines, therefor to move to the
; next scan lines we actually have to jump over n lines where n is the no
; of color planes

merge_point_for_small:

	mov	ax,NextScan		    ; the next plane starts with the--
	mov	NextPlane,ax		    ; -- next scan line
	mov	al,NumPlanes		    ; no of color planes	       
	xor	ah,ah 			    ; reset for multiplication
	mul 	NextScan		    ; to get to the next scan line
	mov	NextScan,ax

; now load the pen color and line style information

	lds	si,lp_phys_pen		    ; DS:SI points to the pen structure
	mov	ah,[si].oem_pen_pcol.SPECIAL

; in case of mono surface, get the mono bit into the LSB

	cmp	NumPlanes,1		    ; monochrome surface ?
	jnz	@f			    ; no.
	shiftr	ah,4			    ;get mono into lsb
	.errnz  MONO_BIT - 00010000b
@@:

	mov	TmpColor,ah		    ;save the color
	jmp	load_style_info



; following two jumps are included to make conditional jumps be within range

jump_exit_output:
	jmp	exit_polyline
jump_error_exit:
	jmp	error_exit

public load_color_info
load_color_info: 

	lds	si,lp_phys_pen		    ; DS:SI points to the pen structure
	mov	ah,[si].oem_pen_pcol.SPECIAL
	mov	TmpColor,ah		    ; save the pen color

load_style_info:

	mov	cx,[si].oem_pen_style	    ; get the requested style
	mov	LineStyle,cl		    ; save style
	cmp	cl,MaxLineStyle	            ; test for validity
	jg	jump_error_exit		    ; illegal style
	je	jump_exit_output            ; null lines, not to be drawn


; get information about the ROPs

	lds	si,lp_draw_mode		    ; DS:SI points to the drawmode block
	mov	bx,[si].Rop2		    ; get the ROP code
	dec	bx			    ; make it zero based
	and	bx,RASTER_OP_MASK	    ; only 4 bits are significant
	mov	DrawModeIndex,bx	    ; save a copy
	mov	si,cx   		    ; the passed in line style
	shl	si,1			    ; 2 bytes of mask per style
	mov	ax,word ptr cs:style_table[si]	    ; get the style mask
	mov	wStyleMask,ax		    ; save both the mask
	mov	wStyleCounter,0		    ; initialize it

; we will normally work with the mask for forward progressing line

	mov	bStyleMask,al		    ; save it
	jcxz	polyline_setup_ega	    ; skip code for solid line styles

; initialization code for styled lines

	or	DeviceFlags,TYPE_IS_STYLED  ; set indicator for styled lines
	lds	si,lp_draw_mode		    ; point to the draw mode block
	mov	dx,[si].bkMode		    ; get the background mode
	mov	BackMode,dl		    ; save it
	mov	dl,byte ptr [si].bkColor.SPECIAL
	mov	BackColor,dl		    ; save the background color

; get the ROP AND and XOR masks for the EGA 

polyline_setup_ega:
	
	test	DeviceFlags,TYPE_IS_DEV	    ; test for the device type
	jz	init_params_for_bm          ; skip code below for bitmaps
	
	mov	NextScan,BYTES_PER_LINE	    ; load offset to next scan line
	mov	al,dm_flags[bx]		    ; get the no of passes of EGA req
	mov	SingleFlag,al		    ; save it
	mov	al,dm_pen_and[bx]	    ; get the ROP AND mask
	mov	PenANDFlag,al		    ; save it
	mov	al,dm_pen_xor[bx]	    ; get the XOR mask
	mov	PenXORFlag,al		    ; save it
	mov	al,dm_data_r[bx]	    ; get the combine function
	mov	DataROTFlag,al		    ; save it

; set up the ega for single pass ROPs or for the first pass of two pass ROPs

	mov	cl,2			    ; assume 2 passes
	mov	ax,LineSegOFFSET device_first_pass_of_two
	mov	bx,LineSegOFFSET device_first_pass_of_two_ret
	test	SingleFlag,SINGLE_OK	    ; is it really single pass ?
	jz	better_do_double	    ; have to do two passes
	call	device_one_pass_init	    ; initialize for one pass
	mov	cl,1			    ; no of passes = 1
	mov	ax,LineSegOFFSET device_one_pass
	mov	bx,LineSegOFFSET device_one_pass_init

better_do_double:

	mov	TempNumPass,cl		    ; no of passes
	mov	pass_prelude,ax		    ; do before every pass
	mov	style_pass_init,bx	    ; for style passes.
	jmp	short start_draw           ; get into the draw routines

; for bitmaps, before each pass over the map the AND,XOR flags has to be    
; calculated from the color bit corresponding to the plane in question. This
; is also to be done before the style passes. Set up the address of the routine
; which does this

init_params_for_bm:

	mov	ax,offset get_next_bitmap_ROP_mask
	mov	pass_prelude,ax
	mov	ax,offset get_next_bm_ret  ; only a return 
	mov	style_pass_init,ax

start_draw:

	lds	si,lp_points		   ; DS:SI points to point array
	mov	dx,[si]			   ; first X coordinate
	mov	cx,[si+2]		   ; first Y coordinate
	add	si,4			   ; take it past the first node
	cCall	ega_line_dispatch	   ; draws the set of poly lines

exit_polyline:
	test	DeviceFlags,TYPE_IS_DEV    ; check the device type
	jnz	reset_EGA_registers	   ; EGA reinit necessary
	mov	ax,1			   ; success code for bitmaps
	jmp	short polyline_return

reset_EGA_registers:

; restore the EGA back to the defaults

	mov	dx,EGA_BASE+GRAF_ADDR
	mov	ax,GRAF_ENAB_SR		   ; disable all planes for set/reset
	out	dx,ax

	mov	ax,DR_SET*256 + GRAF_DATA_ROT
	out	dx,ax			   ; set the combine fn to set/no rot

	mov	ax,0ff00h + GRAF_BIT_MASK  ; set the bit mask
	out	dx,ax			   ; all bits enabled
	
	mov	ax,MM_ALL  
	mov	dl,SEQ_DATA
	out	dx,al			   ; enable all planes

device_exit:
	xor	ax,ax
	inc	ax
	jmp	short exit

error_exit: 
	xor	ax,ax
	errn$	exit
exit:
ifdef	EXCLUSION 
	call	unexclude_far		   ; remove any exclusion area
endif
 
polyline_return:
cEnd	  

;-----------------------------------------------------------------------------;
;                        EGA_LINE_DISPATCH		                      ;
;                        -----------------			              ;
; 									      ;
; Draws the set of polylines after all parameters for the target device and   ;
; varios masks have been calculated.					      ;
; 								              ;
; Inputs:							              ;
;                  CX    ---	first Y coordinate			      ;
; 		   DX    ---	first X coordinate		              ;
;               DS:SI    ---	points to table of subsequent poits           ;
;	    assumes that various masks and scanline next plane details        ;
;           for BITMAPS are already loaded.			              ;
;									      ;
;-----------------------------------------------------------------------------;

cProc ega_line_dispatch,<PUBLIC,NEAR>
cBegin
	mov	ax,count		; no of points
	dec	ax			; no of lines will be one less

; if a single point is to be drawn, we can go back as either the first or the
; last point of a line is never drawn, and if the line is a point, it implies,
; that the point will not be drawn

	jz	return_back		; point not ro be drawn

	mov	bx,cx			; BX gets Yi
	mov	cx,ax			; cx gets count of lines
dispatch_loop:
	push	cx			; save count
	mov	di,[si]			; get Xf
	add	si,2			; take it past the x
	lodsw				; get Yf
	push	si			; save table pointer
	mov	si,ax			; (DI:SI) has end point
	mov	cx,dx
	xchg	cx,bx                   ; (BX:CX) has start point
	push	si
	push	di			; save end points
	push	ds			; save point table segment
	call	Draw_Line		; draw the line
	pop	ds			; restore points segment
	pop	dx			; old Xf = new Xi
	pop	bx			; old Yf = new Yi
	pop	si			; get back table pointer
	pop	cx			; get back count of line
	loop	dispatch_loop		; draw all the lines
return_back:

cEnd
;------------------------------------------------------------------------------;
;			 DRAW_LINE					       ;
;			 ---------					       ;
;									       ;
;  This routine takes two end points of a line and does all the transformations;
;  necessary to get the pseudo line in the first one-fourth quadrant. It also  ;
;  clips the line against the clipping rectangle and for styled lines computes ;
;  the style mask and the style error term to start with, and the value of     ;
;  these terms at the end of the line.					       ;
;									       ;
;  Once all the transformations have been done, it calls the DRAW_ENGINE to    ;
;  actually draw the line. 						       ;
;									       ;
;------------------------------------------------------------------------------;

Draw_Line	proc	near


;---------------------------------------------------------------------------;
; We have now obtained the two end points:			 	    ;
;					                                    ;
;		(BX,CX)  ---  X and Y coordinates of 1st end point          ;
;               (DI,SI)  ---  X and Y cordinates of the other end point	    ;
;---------------------------------------------------------------------------;

; handle clipping separately

	mov	XformFlags,0		; initialize the transformation flags	
	test	ClipFlag,CLIP_TOBE_DONE
	jz	no_clipping		; we shall not do clipping
	call	Clip_Line		; clip the line

; above routine will clip the line against the clipping rectangle and wiill set
; the clipping boundray to the inclusive interval [Xa,Xb]

	jmp	region_clip_done

no_clipping:

; make sure that the line goes from left to right, exchange the end points if 
; not
	cmp	bx,di			; compare the two abscissas
	jle	is_left_to_right	; direction is OK
	xchg	bx,di
	xchg	cx,si		        ; exchange the coordinates
	or	XformFlags,LEFT_XCHG    ; line will be drawn from other end

is_left_to_right:

; we will save the starting coordinates to be used later for start point
; address in memory calculation
	
	mov	StartX,bx
	mov	StartY,cx

; translate the coordinates so that the first point is at origin

	sub	di,bx			; translate the end point
	sub	si,cx			
	jge	line_above_horizontal	; line already in 1st quadrant
	neg	si			; bring the end point into 1st quadrant
	or	XformFlags,Y_FLIP+VERTICAL_FLIP

line_above_horizontal:

; now get the line below the 45 degree line.

	cmp	si,di
	jbe	line_below_45		; we are already X major
	xchg	di,si			; exchange the end points
	or	XformFlags,XY_XCHG	; record the transformation

line_below_45:

; as we here we are not clipping the line, the clipping interval will be set
; to the two end of the line.

	mov	Xa,0    		; left hand clip margin
	mov	Xb,di	        	; right hand clip margin

region_clip_done:

; if the first pel is not to be drawn we will actually restrict the clipping
; interval if it has not already excluded the first pel.
; we skip the first pel only when the line moves from end to start but for the
; actual direction of the line, the last pel is clipped.

	test	XformFlags,LEFT_XCHG
	jz	clip_last_pel		; clip off the last pel
	cmp	Xa,1		        ; is the first pel already clipped ?
	jae	end_pel_processed	; yes
	mov	Xa,1		        ; clip the first pel
	jmp	short end_pel_processed
clip_last_pel:
	cmp	Xb,di		        ; is the last pel already clipped?
	jb	end_pel_processed	; yes, so no more to clip
	dec	di               	; take out last pel from line
	mov	Xb,di
	inc	di
	jnz	end_pel_processed	; watch out for zero pel line
	
; we want to continue with zero length lines still as the start style for the
; next line has to be decided next.

	mov	Xa,1
	mov	Xb,0		        ; Xa>Xb would tell us about 0 length
end_pel_processed:

;----------------------------------------------------------------------------;
;		COMPUTE THE STYLE MASK/ERR AT BOTH ENDS OF LINE              ;
;----------------------------------------------------------------------------;

	cmp	LineStyle,PS_SOLID	; is the line a solid line
	jz	bypass_mask		; ignore for solid lines
	cmp	LineStyle,PS_NULL	; invisible line
	jz	bypass_mask		; ignore for NULL lines too
	jmp	short mask_needed	; calculate masks
bypass_mask:
	jmp	do_halfflip		; do hal flip if required

mask_needed:
; if X and Y have been interchanged, get them back for the comparisions

	test	XformFlags,XY_XCHG	; where they interchanged ?
	jz	not_xchged		; no
	xchg	di,si			; get them back for the timebeing
not_xchged:

; now di has X and si has Y
; determine the stylistically major direction of the line

	mov	ax,wDistHoriz		; horizontal style err add factor
	mov 	bx,wDistVert		; vertical style error add factor
	mul	di			; di has X
	xchg	ax,bx			; get Y into BX, save AX
	mov	cx,dx			; save all 32 bits of mul
	mul	si			; si has Y

; now DX:AX has Y*fy and CX:BX has X*fx, compare them

	cmp	dx,cx			; compare HIWORD
	jb	styled_as_X_major
	ja	styled_as_Y_major
	cmp	ax,bx			; HIWORD tied, compare LOWORD
	ja	styled_as_Y_major
styled_as_X_major:
	mov	ax,wDistHoriz		; fx
	mov	f1,ax			; f1=f2=fd=ax
	mov	f2,ax
	mov	wDistDiag,ax
	mov	SideStep,ax
	mov	cx,ax			; ax has f1, cx has f2
	jmp	short calculate_mask
styled_as_Y_major:
	mov	ax,wDistVert
	mov	f1,ax
	mov	cx,ax			; f1=f2=fd=sidestep = fy 
	mov	f2,ax
	mov	wDistDiag,ax
	mov	SideStep,ax		; assume normally Y major case

; if the line is normaly X major, then side step will be 0 else fy

	cmp	di,si			; compare X with Y
	jb	calculate_mask
 	mov	SideStep,0      	; actually X major line.

calculate_mask:

; AX has f1, CX has f2,  totall add factor z = f1 * (major-minor) + minor * Y
; we will have to get back major into DI
	
	test	XformFlags,XY_XCHG	; were the ends shuffled ?
	jz	major_in_di		; no
	xchg	di,si			; major in di, minor in si
major_in_di:
	mov	bx,di
	sub	bx,si			; BX gets X - Y
	mul	bx			; f1*(X-Y)
	xchg	ax,cx			; save result, get f2
	mul	si			; f2*Y
	add	ax,cx			; get the total err add needed 
	mov	bx,wStyleCounter	; the state and error at start
	add	ax,bx
	mov	wStyleCounter,ax	; the state and error at end

; if we are going to draw the line from the far end, the reverse of the mask
; and the state from the end of the line is to be used

	mov	dx,wStyleMask		; dh has the reverse mask
	test	XformFlags,LEFT_XCHG	; are we going reverse
	jz	not_reverse		; no
	mov	bx,ax			; style counter at end
	not	bx			; we are going reverse from end
	mov	dl,dh			; use the reverse of the mask
not_reverse:
	mov	bStyleError,bl		; the error term to use
	mov	cl,bh			; the state
	and	cl,7			; we had not done mod 8 earlier
	rol	dl,cl			; rotate the default state
	mov	bStyleMask,dl		; the new style maska

; now we will do the half flip, if necessary, for styled lines.
; the code below is done for non styled lines too. For them f1 and f2 make
; no sense, but executing the code on f1 and f2 is not going to harm us

do_halfflip:
	mov	ax,si			; minor
	add	ax,ax			; 2*minor
	cmp	ax,di			; 2*minor versus major
	jbe	halfflip_not_req	; line already in 1st one 4th quad
	sub	si,di
	neg	si			; new minor = major-minor
	xor	XformFlags,HALF_FLIP + VERTICAL_FLIP
halfflip_not_req:

; now the equation for Z was for the original coordinates of the line, but
; now we have flipped the line. Thus although the major remains the same
; the minor has become major-minor. If we continue to use the same equation
; with the new major and minor values, then interms of the original values
; we will have:     z = minor*f1 + (major-minor)*f2, which will be wrong unless
; we interchange f1 and f2.

; Thus we will now interchange f1 and f2 and continue to use the original 
; equation for z even for the flipped line. fd and sidestep were calculated
; for the original line and they should not be affected.

	mov	ax,f1
	xchg	f2,ax			; f2 gets f1 and ax gets f2
	mov	f1,ax

; at this point all the transformations have been done and we are ready to
; draw the line.

	mov	ax,Xa
	cmp	ax,Xb	        	; test for valid clip interval
	ja	Draw_Line_Ret		; line is totally clipped
	mov	cdy,si	
	call	Draw_Engine		; draw the line

public Draw_Line_Ret
Draw_Line_Ret:
	ret				; one more line drawn

Draw_Line	endp
;----------------------------------------------------------------------------;
; Define a DDA running macro.  We use eight specialized loops for            ;
; speed.  Trying to make it just one leads to too many tests and jumps.      ;
;----------------------------------------------------------------------------;



; define the various keywords

m?sdda_rotate macro
?sdda_rotate = 1
endm

m?sdda_horiz macro
?sdda_horiz = 1
endm

m?sdda_vert macro
?sdda_vert = 1
endm

m?sdda_diag macro
?sdda_diag = 1
endm

; define the macro

run_the_sdda	macro	mylist
local	top_of_run,end_of_run


draw_horiz	macro	
	local	__x1,__x2


	dec	cx		; CX has number of bits to move
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
	jz	__x2		; intermediate bytes present

; put out the first byte
	
	out	dx,al		; outputs the starting FILTER to BITMASKREGISTER
	mov	al,0ffh		; or with this
	or	[di],al   	; sets bits on depending on value of FILTER
	inc	di		; point to next byte
	dec	cx		; one byte done
	jz	__x2		; the end FILTER
	out	dx,al		; for intervening byte FILTER hasall bitsenabled
__x1:
	or	[di],al  	; make all intervening bytes one
	inc	di		; next byte 
	loop	__x1		; completely fill up the intervening bytes
	
__x2:
	and	al,ah		; incase the start and end bits in same byte
	out	dx,al		; set up FILTER
	or	byte ptr [di],0ffh                  
    
;
; es:di still pts to the current byte and bl still has the current pel in it
;
	endm

draw_vert macro	
	local	__y1,__y2,__y3,__y4


	mov	bh,0ffh		; will be used to OR with the FILTER
	mov	al,bl		; save the rotating FILTER

; The GRAPHICS CONTROLLER ADDRESS REGISTER has the index of the BIT MASK 
; REGISTER, and DX points to the DATA REGISTER. So to set up FILTER simply
; output desired bit FILTER pattern

	out	dx,al		; set up FILTER for the selected bit
	dec	cx		; CX now has number of lines to move
	jz	__y4		; intermediate steps too

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
	add	ax,offset __y3
	jmp	ax		; jump into the calculated entry point

	even			; allign on word boundary

__y2:

	or	[di],bh		; set a bit on
	add	di,si		; next scan line

; the above code takes 4 bytes

	.errnz ($ - __y2) - 4

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

__y3:
	loop	__y2

__y4:

	or	[di],bh		; set on the last pixel

	endm

draw_diag	macro  
	local	__d1,__d2


	mov	al,bl		; get current bit position
	
; remember that the GRAPHICS CONTROLLER ADDRESS REGISTER still has the index
; of the BIT MASK REGISTER, so we can output its value into the DATA REGISTER	

	dec	cx
	jz	__d2		; only one pixel to set on

__d1:
	out	dx,al		; set up the FILTER as the current bit position
	or	bptr [di],0ffh	; set current bit on
	ror	al,1		; next bit postion is 1 bit right
	adc	di,si		; and 1 bit down
	loop	__d1
__d2:
	out	dx,al		; set up the bit FILTER register
	or	bptr [di],0ffh	; set it on
	mov	bl,al		; return with the last bit position in bl

	endm

;; determine what arguments we are given

?sdda_rotate = 0
?sdda_horiz = 0
?sdda_vert = 0
?sdda_diag = 0

	irp	x,<mylist>
ifdef m?sdda_&&x
	m?sdda_&&x
else
	??error2  <run_the_sdda - unknown keyword x>
endif
	endm

	cmp	cScans,0
	jz	end_of_run
	mov	cx,Ca

top_of_run:

	irp	x,<mylist>
ifdef draw_&&x
	draw_&&x 
endif
	endm

;; handle normal sidestep and rotate

if ?sdda_rotate
	ror	bl,1
	adc	di,cScanStep
else
	add	di,cScanStep
endif ; ?sdda_rotate

	dec	cScans
	jz	end_of_run

; determine the count for intermediate scans

	mov	cx,cm
	mov	ax,WorkErrTerm
	or	ax,ax
	js	@F
	inc	cx

;------------------------------------------------------------------------------;
; note that we had had,							       ;
; fn+1' = fn' + frac(s) - 1        if fn' positive                             ;
;       = fn' + frac(s)            if fn' negative                             ;
;									       ;
; the fractions are all in units of 1/dy, so 1 in the above equation will be dy;
;------------------------------------------------------------------------------;

	sub	ax,cdy			;; wError -= 1
@@:	add	ax,wFracS		;; wError += frac(s)
	mov	WorkErrTerm,ax

	jmp	short top_of_run

end_of_run:

	mov	cx,Cb
	irp	x,<mylist>
ifdef draw_&&x
	draw_&&x 
endif
	endm
;	jmp	style_pass_init
	ret				;;registers will be initialized on exit

	endm
;----------------------------------------------------------------------------;
; The table of DDA routines.						     ;
;----------------------------------------------------------------------------;

public sdda_table

sdda_table	equ	this word
	dw	sdda_X___
	dw	sdda__Y__
	dw	sdda___D_
	dw	sdda___D_
	dw	sdda_X__R
	dw	sdda__Y_R
	dw	sdda___DR
	dw	sdda___DR

public	sdda_X___
public	sdda_X__R
public	sdda__Y__
public	sdda__Y_R
public	sdda___D_
public	sdda___DR

sdda_X___:	run_the_sdda <horiz>
sdda_X__R:	run_the_sdda <horiz,rotate>
sdda__Y__:	run_the_sdda <vert>
sdda__Y_R:	run_the_sdda <vert,rotate>
sdda___D_:	run_the_sdda <diag>
sdda___DR:	run_the_sdda <diag,rotate>

;----------------------------------------------------------------------------;
; define the various keywords

m?dda_huge macro
?dda_huge = 1
endm

m?dda_style macro
?dda_style = 1
endm

m?dda_rotate macro
?dda_rotate = 1
endm

m?dda_short macro
?dda_short = 1
endm

; define the macro

run_the_dda	macro	mylist
	local	top_of_run

top_of_run:

;; determine what arguments we are given

?dda_huge = 0
?dda_style = 0
?dda_short = 0
?dda_rotate = 0
	irp	x,<mylist>
ifdef m?dda_&&x
	m?dda_&&x
else
	??error2  <run_the_dda - unknown keyword x>
endif
	endm

;; handle the huge sidestep
;; WARNING: only set HUGE if cbSideStepAddressDelta is non-zero!

if ?dda_huge

	dec	ScansLeftFromBot	;; no of scans left in segment
	jnz	@F
	mov	ax,wScans		;; reset wScan
	mov	ScansLeftFromBot,ax	;; 
	mov	ax,ds			;; advance the segment
	add	ax,selHugeDelta		;; advance to next segment
	mov	ds,ax
	assumes ds,nothing
	add	di,cbHugeScanDelta	;; wrap the surface address
@@:
endif ; ?dda_huge

;; handle normal sidestep and rotate

if ?dda_rotate
	ror	bl,1
	adc	di,cScanStep
else
	add	di,cScanStep
endif ; ?dda_rotate

; handle styling

if ?dda_style
	mov	cl,bStyleError		; the current error term
	xor	ch,ch
	add	cx,SideStep		; add the step error term
	mov	bStyleError,cl		; update
	mov	cl,ch			; rotate if ch nonzero
	and	cl,7
	rol	bStyleMask,cl		; rotate mask
endif ; ?dda_style

; go for the next scan

	dec	cScans
if ?dda_short
	jz	do_the_last_scan
else
	njz	do_the_last_scan
endif

; determine the count for intermediate scans

	mov	cx,cm
	mov	ax,WorkErrTerm
	or	ax,ax
	js	@F
	inc	cx

;------------------------------------------------------------------------------;
; note that we had had,							       ;
; fn+1' = fn' + frac(s) - 1        if fn' positive                             ;
;       = fn' + frac(s)            if fn' negative                             ;
;									       ;
; the fractions are all in units of 1/dy, so 1 in the above equation will be dy;
;------------------------------------------------------------------------------;

	sub	ax,cdy			;; wError -= 1
@@:	add	ax,wFracS		;; wError += frac(s)
	mov	WorkErrTerm,ax

; loop back to the top, while calling the RunRoutine

	push	LineSegOFFSET top_of_run
	jmp	S1			; CX = number of pixels to draw
	endm
;----------------------------------------------------------------------------;
; The table of DDA routines.						     ;
;----------------------------------------------------------------------------;

WDDA_ROTATE	equ	0002h
WDDA_STYLE	equ	0004h
WDDA_HUGE	equ	0008h

	.errnz	WDDA_ROTATE - 2
	.errnz	WDDA_STYLE  - 4
	.errnz	WDDA_HUGE   - 8

public dda_table

dda_table	equ	this word
	dw	dda____
	dw	dda___R
	dw	dda__S_
	dw	dda__SR
	dw	dda_H__
	dw	dda_H_R
	dw	dda_HS_
	dw	dda_HSR			     

;------------------------------------------------------------------------------;
;			   DRAW_ENGINE					       ;
;			   -----------					       ;
; This routine implements BRESHENHAM'S RUN LENGTH SLICE ALGORITHM	       ;
; 									       ;
; The description of the algorithm can be obtained from the file LINE.DOC      ;
;							 		       ;
; The pseudo line runs from (0,0) to (DI,SI)				       ;
;									       ;
;------------------------------------------------------------------------------;

Draw_Engine	proc	near

	mov	iWhichDDA,0		;initialize

	or	si,si			; is the line horizontal ?
	jnz	non_horizontal		; no

; we have got a horizontal line on scan line on the X axis

	mov	Ya,si			; the only scan line
	mov	cScans,si		; no more lines to scan
	mov	ax,Xb		
	sub	ax,Xa    		; will give no of points to set on
	inc	ax			; include both end points
	mov	Cb,ax			; give last scan length slice
	jmp	find_out_dda		; decide direction of draw/move

public	non_horizontal
non_horizontal:
	
;------------------------------------------------------------------------------;
; compute the various slope quantities					       ;
;									       ;
; m      = floor(s)							       ;	       ;
; wFracS = frac(s)  (fraction in units of 1/dy)                                ;
;									       ;
; Set a bit in iWhichDDA						       ;
;------------------------------------------------------------------------------;

	mov	ax,di			; has dx
	xor	dx,dx			; reset for division
	div	si			; AX has quotient = floor(s)
	mov	cm,ax			; save m
	mov	wFracS,dx		; DX has remainder
;------------------------------------------------------------------------------;
; compute the starting scan no:						       ;
;									       ;
; Divide [2dy.xa - 2dy.f0 + dx - 1] by 2dx				       ;
; ya = quotient of the division, rm the remainder			       ;
;									       ;
; remember 2dx may overflow into 17 bits				       ;
;------------------------------------------------------------------------------;

	add	si,si			; 2dy
	mov	ax,Xa
	mul	si			; 2dy.xa
	mov	bx,di			; dx
	dec	bx			; dx - 1
	xor	cx,cx

; 2dyf0 will be -1 if the LSB of the HIBYTE of XformFlags is 1

	cmp	byte ptr XformFlags[1],1
	.errnz	(VERTICAL_FLIP - 100h)

	cmc				; carry will be one if 2dyf0 = -1
	adc	ax,bx		
	adc	dx,cx			; DX:AX has dividend
	
	shr	dx,1
	rcr	ax,1			; we have divided by 2
	adc	cx,cx			; store carry out

; as we have divided by 2 by shifting, the final carry out will contribute 
; to rm

	div	di			; divide result by dx
	mov	Ya,ax			; quotient gives start scan ordinate

; DX has the remainder of division by dx, where the dividend happened to be
; the quotient after the divide by 2 above. Therefore rm that we need is actually
; now,
;		rm = 2.DX + CX

;------------------------------------------------------------------------------;
; compute the first run							       ;
;									       ;
; Ca = floor (s - (rm+1)/2dy) + 1					       ;
;									       ;
; remember SI has 2dy							       ;
;									       ;
; s - (rm+1)/2dy  =  (2dx - rm - 1)/2dy  = (2(dx-DX) - (CX+1))/2dy             ;
;------------------------------------------------------------------------------;

	mov	ax,di			; get dx
	sub	ax,dx			; dx - DX
	xor	dx,dx			; zeroise for addition overflow
	add	ax,ax
	adc	dx,dx			; DX:AX has 2(dx-DX)
	inc	cx
	sub	ax,cx
	sbb	dx,0			; DX:AX has dividend
	div	si			; divide by 2dy
	inc	ax				
	mov	Ca,ax			; sace Ca

;------------------------------------------------------------------------------;
; compute the initial error term					       ;									       
;									       ;
;  f1' = frac(s -(rm+1)/2dy) + frac(s) - 1				       ;
;									       ;
;  DX has currently the first frac in units of 1/2dy, frac(s) was calculated in;
;  terms of 1/dy.  As just the sign of f1' is used in decision, we shall get   ;
;  first term also in units of 1/dy                                            ;
;									       ;
;------------------------------------------------------------------------------;

	shr	dx,1			; get frac in units of 1/dy
	add	dx,wFracS 										
	sub	dx,cdy			; subtract 1 in units of 1/dy

	mov	wError,dx		; the first error term
	mov	WorkErrTerm,dx		; es will be used as work err register

;------------------------------------------------------------------------------;
; compute the last scanline ordinate:						       ;
;									       ;
; Divide [2dy.xb - 2dy.f0 + dx - 1] by 2dx				       ;
; yb = quotient of the division, rm' the remainder			       ;
;									       ;
; remember 2dx may overflow into 17 bits				       ;
;------------------------------------------------------------------------------;

	mov	ax,Xb
	mul	si			; 2dy.xb
	mov	bx,di			; dx
	dec	bx			; dx - 1
	xor	cx,cx

; 2dyf0 will be -1 if the LSB of the HIBYTE of XformFlags is 1

	cmp	byte ptr XformFlags[1],1
	.errnz	(VERTICAL_FLIP - 100h)

	cmc				; carry will be one if 2dyf0 = -1
	adc	ax,bx		
	adc	dx,cx			; DX:AX has dividend
	
	shr	dx,1
	rcr	ax,1			; we have divided by 2
	adc	cx,cx			; store carry out

; as we have divided by 2 by shifting, the final carry out will contribute 
; to rm'

	div	di			; divide result by dx
	mov	Yb,ax			; quotient gives start scan ordinate

; DX has the remainder of division by dx, where the dividend happened to be
; the quotient after the divide by 2 above. Therefore rm that we need is actually
; now,
;		rm' = 2.DX + CX

	sub	ax,Ya			; will give the no of scan lines
	mov	cScans,ax

; if Ya = Yb then we are clipped to be horizontal

	jnz	still_not_horizontal
	
; we have got a horizontal line on scan line Ya
	
	mov	ax,Xb
	sub	ax,Xa			; no of pels to set on
	inc	ax			; include both end points
	mov	Cb,ax			; save slice length
	jmp	short find_out_dda	; decide on draw/move directions	
still_not_horizontal:
	
;------------------------------------------------------------------------------;
;compute the last slice length:						       ;
;									       ;
; Cb = ceil(rm' + 1)/2dy						       ;
;									       ;
; note rm' = 2DX + CX							       ;
;------------------------------------------------------------------------------;

	mov	ax,dx
	xor	dx,dx			; reset for add overflow
	add	ax,ax			
	adc	dx,dx			; DX:AX has 2DX
	inc	cx			; include the 1 to be added
	add	ax,cx
	adc	dx,0			; DX:AX has rm' + 1
	div	si			; divide by 2.dy
	neg	dx			; if there was rem, carry will be set
	adc	ax,0			; gives the ceiling value
	mov	Cb,ax			; save last slice length

find_out_dda:

;------------------------------------------------------------------------------;
;we will first have to advance the style mask for clipping:		       ;
;									       ;
;The portion of the line which is being skipped corresponds to (0,0) to (xa,ya);
;we will use the formula  z = (major-minor)*f1 + minor*f2 to decide by how much;
;the style mask and the error term has to be advanced.			       ;
;									       ;
;------------------------------------------------------------------------------;

	cmp	LineStyle,PS_SOLID	; skip for solid lines
	jz	no_style_adjust
	cmp	LineStyle,PS_NULL	; skip for null lines too
	jz	no_style_adjust	
	or	iWhichDDA,WDDA_STYLE	;styled lines

; now the portion of the line from (0,0) to (xa,ya) is not going to be drawn 
; so the style mask has to be advanced. We will use the formula:
;		z = f1*(major-minor) + f2*minor
; f1 and f2, we have ensured, have the correct values depending on the actual
; direction of progress of the line.

	mov	ax,Xa			; the major coordinate
	mov	bx,Ya			; the minor coordinate
	sub	ax,bx
	mul	f1			; f1*(major-minor)
	xchg	ax,bx			; get the minor coordinate
	mul	f2
	add	ax,bx			; total add factor
	add	bStyleError,al		; update the error term
	adc	ah,0			; ah has the style mask state
	mov	cl,ah
	and	cl,7			; sate can be from 0 to 7
	rol	bStyleMask,cl		; get the mask into position
no_style_adjust:

;------------------------------------------------------------------------------;
; the move routine for the side step has  two functions:		       ;
;								               ;
;               . it rotates the current bit position by 1 bit (oe does not)   ;
;               . moves the byte position to the next scan (or does not)       ;
;									       ;
; it may do one or both of the above steps depending on the direction of the   ;
; move.  We will use two flags cRotate and cStep. cRotate (either 0 or 1) tell ;
; us by how much we rotate the current bit and cStep (either 0,NextScan or     ;
; -NextScan) tell us by how much the current byte has to be stepped. CL will be;
; the temporary storage for rotation flag and CH will tell us whether we need  ;
; step to next scan or not. We start with CL = 1 and CH = 1, ie, we need to do ;
; rotation and step in positive Y direction. We will test the transformations  ;
; we have undergone to get to the correct decision.                            ;
;									       ;
; Note, to calculate the address in memory of the first point, we need to step ;
; back (xa,ya) through the reverse of the transformations and translate them   ;
; wrt the start point of the line. We do all these below:		       ;
;------------------------------------------------------------------------------; 

	mov	si,NextScan		; distance to the next scan line
	mov	bx,Xa
	mov	ax,Ya			; (Xa,Ya) the clipped pseudo start pt
	mov	cx,101h

; so our default decision is that we are moving towards higher memory, ie Y 
; value increases, and that the step direction is really diagonal. This makes
; us assume that we are in the first fourth quadrant.

; test whether the half_flip was done or not

	mov	di,XformFlags
	test	di,HALF_FLIP
	jz	test_XY_XCHG		; it was not done

; for HALF FLIP, the actual value of the ordinate of the end point can be 
; obtained by subtracting the transformed ordinate from the abscissa. The 
; actual and the transformed abscissa are the same. I a line has been half-
; -flipped, the step direction is horizontal so, CH should be set to false

	sub	ax,bx
	neg	ax			; the ordinate before transformation
	xor	ch,ch			; reset step scan flag for side step

; test for reflection about 45 degree line

test_XY_XCHG:
	test	di,XY_XCHG
	jz	test_Y_FLIP		; reflection was not done
	xchg	ax,bx			; actual end point coordinates
	xchg	ch,cl			; rotation and step interchanged

; test for reflection about the X axis

test_Y_FLIP:
	test	di,Y_FLIP
	jz	tests_done
	neg	ax			; actual Y is -Y
	neg	si			; will be going up in display mem

tests_done:
	mov	cRotation,cl		; no of bits to ratate by (0 or 1)
	add	cl,cl
	or	bptr iWhichDDA,cl	; mark rotation or not
	.errnz	WDDA_ROTATE - 2

	mov	cScanMove,si		; offset to move to next(prev) scan
	neg	ch			; setting up mask
	sbb	cx,cx			; make it a word
	and	si,cx			

; si will be forced to zero if no scan move during step is necessary

	mov	cScanStep,si		; offset used in step routines

; now we will calculate the offset in the display memory of the first point

; first we translate the clipped start point wrt the actual start

	add	bx,StartX				
	add	ax,StartY
	mov	dx,ax			; dx has the scan line no

	mov	cx,seg_SurfaceStart	; start of surface
	mov	seg_SurfaceAddress,cx	; assume same

; handle adjustment for huge surfaces.

	mov	cx,wScans		; get the no of scans per segment
	jcxz	not_huge_surface	; it is a small bm or device
	or	si,si			; is there a scan move during step ?
	jz	@f			; no, treat as not huge
	or	iWhichDDA,WDDA_HUGE	; it is a huge surface
@@:		
	mov	si,seg_SurfaceStart	; get the start of the segment
	cmp	dx,cx			; does start in first segment ?
	jb	no_huge_adjust_needed	; yes.
	mov	ax,dx			; get the start scan number
	xor	dx,dx			; dx:ax has start scab num
	div	cx			; dx has the remainder
	push	dx			; save
	mov	dx,NextSegOff		; offset to nexy segment
	mul	dx			; result will fit in a word
	pop	dx			; restore the remainder
	add	si,ax			; start segment

no_huge_adjust_needed:

	mov	seg_SurfaceAddress,si	; save the start segment

; decide what the huge adjustments are

	sub	cx,dx			; no of remaining scans in segment
	mov	ax,FillBytes		; filler bytes at end
	mov	si,NextSegOff		; offset to next segment
	test	di,Y_FLIP		; going in opposite direction ?
	jz	@f			; no.
	mov	cx,dx			; no of scans left from top
	inc	cx			
	neg	ax			; offset to the start of last scan
	neg	si			; will have to go up
@@:
	mov	selHugeDelta,si		; segment offset
	mov	cbHugeScanDelta,ax	; scan offset

not_huge_surface:

	mov	ds,seg_SurfaceAddress	; satrt sel
	mov	ScansLeftFromBot,cx	; no of scans left
	mov	wTmpScan,cx		; save it here to

; compute offset and bit mask

	mov	ax,NextScan		; width of a scan
	mul	dx			; have the offset to start of scan
	mov	cx,bx			; get the start X
	shiftr	bx,3			; get no of whole bytes
	add	ax,bx			; offset to start of byte
	add	ax,off_SurfaceStart	; start of surface
	mov	off_SurfaceAddress,ax	; save as start of line
	mov	TempCurByteSty,ax	; save it
	and	cl,7			; get the bit number
	mov	al,80h			; start at left most bit
	ror	al,cl			; position for the mask
	mov	TempCurBit,al		; save start bit mask


; now calculate the address of the move and draw routines
; The draw and move tables have 4 addresses each. We have 4 draw table: tables
; for solid lines to EGA, styled line sto EGA, solid lines to MAPs and styled
; lines to maps. 
		

	test	SingleFlag,SINGLE_OK
	jz	@f
	mov	al,DeviceFlags
	and	al,TYPE_IS_DEV + TYPE_IS_STYLED
	cmp	al,TYPE_IS_DEV
	jnz	@f

	mov	ax, LineSegOFFSET sdda_table
	mov	si,iWhichDDA
	and	si,3			;get the rotate flag bit only
	shiftl	si,2			;4 words below is the rotate table
	add	ax,si			;start of proper group
	mov	si,di			;get the xform flag
	and	si,110b			;mask of all but two bits
	add	si,ax			;points to proper table
	lods	wptr cs:[si]		;get the address
	mov	di,off_SurfaceAddress	; load the pointer
	mov	dx,EGA_BASE+GRAF_DATA	
	mov	bl,TempCurBit		; bl has current bit mask
	mov	si,cScanMove		; load offset to next/prev scan
	jmp	ax			;draw and return

@@:

	mov	si,di			; get the xform flags
	and	si,110b			; mask off all but two bits
	.errnz  HALF_FLIP + XY_XCHG - 110b
	xor	ax,ax
	mov	al,DeviceFlags
	and	al,0fh			; get the low nibble
	shiftl	al,3			; * 8, each table of 8 bytes
	add	ax,LineSegOFFSET DrawTable_solid_ega
	add	si,ax			; points to table to use
	lods	word ptr cs:[si]	; get the routine address
	mov	S1,ax			; save the draw routine address
	mov	si,iWhichDDA		; get the DDA index
	mov	ax,dda_table[si]	; the start routine address
	mov	StartDDAAddr,ax		; save the address


; save the variables wchich will be used for style draw pass

	mov	al,TmpColor
	mov	SaveColor,al		; save forground color
	mov	al,TempNumPass		; get the number of passes
	mov	NumPasses,al		; save
	mov	StylePass,0		; reset style pass var
	mov	si,cScanMove		; load offset to next/prev scan

do_another_pass:

	mov	di,off_SurfaceAddress	; load the pointer
	call	pass_prelude		; do the pass
	or	ax,ax			; more passes to do ?
	jz	do_next_pass
	mov	bl,TempCurBit		; bl has current bit mask
	cmp	cScans,0		;single slice line ?
	jz	do_the_last_scan	;yes

; draw the first slice

	mov	cx,Ca			; first slice length 
	push	StartDDAAddr		; the start routine address
	jmp	S1			; start the process

; our favorite DDA routines.

dda____:	run_the_dda <short>
dda___R:	run_the_dda <short,rotate>

;----------------------------------------------------------------------------;
; finish up the work horse routines.					     ;
;----------------------------------------------------------------------------;

do_the_last_scan:
	mov	cx,Cb			; the last slice length
	call	S1			; draw the slice

do_next_pass:

	dec	NumPasses		; one more pass done
	jz	all_passes_done
	jmp	do_another_pass		; do the next pass
	
all_passes_done:

	test	DeviceFlags,TYPE_IS_STYLED
	jz	reset_ega		; we are done
	cmp	BackMode,OPAQUE		; opaque background ?
	jne	reset_ega		; do not have to draw gaps
	test	StylePass,1		; is it the first style pass ?
	jz	style_draw_pass		; yes

; we have completed the style pass also

	xor	StylePass,1		; reset indicator
	mov	al,SaveColor		; saved value of foreground color
	mov	TmpColor,al		; reset it
	jmp	short reset_ega		

; we begin the style draw pass for the gaps

style_draw_pass:

	xor	StylePass,1		; set indicator
	mov	al,BackColor		; background color
	mov	TmpColor,al		
	mov	ROPcolor,al		; to be used for BITMAP masks
	mov	al,TempNumPass
	mov	NumPasses,al		; reaload number of passes
	mov	di,TempCurByteSty	; initial byte offset
	mov	off_SurfaceAddress,di 	; reload it into work var
	mov	ax,wError		; initial ereror term
	mov	WorkErrTerm,ax	        ; load it into work variable
	mov	ax,cTempScans		;restore no of slices
	mov	cScans,ax
	mov	dx,wTmpScan		;restore scans left from bottom
	mov	ScansLeftFromBot,dx
	call	style_pass_init		; initialization before style pass
	mov	al,bTempStyleError	; error at start of draw
	mov	bStyleError,al		; reload it
	mov	al,bTempStyleMask	; mask at start of draw
	not	al			; complement of mask for gaps
	mov	bStyleMask,al		; load mask for gaps
	mov	bTempStyleMask,al	; next pass will get from here
	jmp	do_another_pass	        ; start afresh

; reset the ega registers in case of draw to ega

reset_ega:
	test	DeviceFlags,TYPE_IS_DEV	; is it EGA ?
	jz	return_from_engine	; no
	call	style_pass_init		; initialize for next pass
return_from_engine:
	ret

Draw_Engine	endp
;----------------------------------------------------------------------------;
; more DDA routines.							     ;
;----------------------------------------------------------------------------;

dda__S_:	run_the_dda	<style>
dda__SR:	run_the_dda	<style,rotate>
dda_H__:	run_the_dda	<huge>
dda_H_R:	run_the_dda	<huge,rotate>
dda_HS_:	run_the_dda	<huge,style>
dda_HSR:	run_the_dda	<huge,style,rotate>

;-----------------------------------------------------------------------------;
;	             DEVICE_ONE_PASS_INIT				      ;
;	             --------------------				      ;
;  Initializes the EGA registers for one pass ROP codes.		      ;
;							                      ;
;  On entry:						                      ;
;              PenANDFlag  ----  has the ROP code AND mask		      ;
;              PenXORFlag  ----  has the ROP code XOR mask		      ;
;             DataROTFlag  ----  has the EGA combine function		      ;
;                TmpColor  ----  value of the pen color			      ;
;						                              ;
comment ~

	The steps are:

         . TmpColor &= PenANDFlag
         . TmpColor ^= PenXORFlag

TmpColor now has P, !P, ZERO or ONES depending on the particular ROP code

         . enable all palnes for SET/RESET
         . set DATA ROTATE REGISTER to DataROTFlag       
         . enable all planes in the SEQUENCER MAP MASK REGISTER
         . enable all bits in the GRAPHICS CONTROLLER BIT MASK REGISTER and 
	   leave its address in the GRAPHICS CONTROLLER ADDRESS REGISTER

end comment ~
;                                 				             ;
;----------------------------------------------------------------------------;

device_one_pass_init proc	near

	mov	ah,TmpColor	; the pen color
	and	ah,PenANDFlag
	xor	ah,PenXORFlag
	mov	al,GRAF_SET_RESET ; address of the set reset register
	mov	dx,EGA_BASE + GRAF_ADDR
	out	dx,ax		; set up S/R REGISTER
	mov	ah,MM_ALL	; lst 4 bits on
	mov	al,GRAF_ENAB_SR ; enable s/r regsiter address
	out	dx,ax		; enable all the S/R REGISTER bits
	mov	ah,DataROTFlag	; hold the combine function
	mov	al,GRAF_DATA_ROT
	out	dx,ax
	mov	ax,0ff00h + GRAF_BIT_MASK ; enable all pixels
	out	dx,ax

; GRAPHICS CONTROLLER ADDRESS REGISTER will retain address of BIT MASK REGISTER

	mov	dl,SEQ_DATA	; SEQUENCER DATA REGISTER
	mov	al,MM_ALL	; enable all planes
	out	dx,al
	ret

device_one_pass_init endp

device_one_pass  proc near

	mov	dx,cScans
	mov	cTempScans,dx		; save it
	mov	dl,bStyleError		; error at start
	mov	bTempStyleError,dl	; save it
	mov	dl,bStyleMask		; mask at start of line
	mov	bTempStyleMask,dl	; save it
	mov	dx,EGA_BASE+GRAF_DATA	
	mov	ax,1
	ret

device_one_pass  endp
;-----------------------------------------------------------------------------;
;		     DEVICE_FIRST_PASS_OF_TWO			              ;
;		     ------------------------				      ;
; Programs the EGA registers for the first pass of a two pass ROP code.       ;
;							                      ;
comment ~

	Steps involved are:

		. TmpColor ^= PenXORFlag
		. set the ENABLE S/R register and the SEQUENCER MAP MASK
                  register to the value of TmpColor
	        . set the S/R register to the value of PenANDFlag
		. set the DATAT ROTATE register to the value of DataROTFlag
                  which has to be SET in this case
		. set all bits in the BIT MASK REGISTER on and leave its
                  address in the GRAPHICS CONTROLLER ADDRESS register
end comment ~
;							                     ;
;----------------------------------------------------------------------------;

device_first_pass_of_two proc	near

; make copies of the variables that the first pass would destroy

	mov	ax,cScans
	mov	cTempScans,ax		; save it
	mov	al,bStyleError		; error at start
	mov	bTempStyleError,al	; save it
	mov	al,bStyleMask		; mask at start of line
	mov	bTempStyleMask,al	; save it

; set up the EGA

	mov	ah,PenANDFlag
	mov	al,GRAF_SET_RESET
	mov	dx,EGA_BASE + GRAF_ADDR
	out	dx,ax		; S/R register programmed with PenANDFlag
	
	mov	ah,TmpColor
	xor	ah,PenXORFlag
	mov	al,GRAF_ENAB_SR	
	out	dx,ax		; ENABLE S/R register enabled for req. planes
	
	push	ax		; save al
	mov	ah,DataROTFlag  ; has to be = SET in this case
	mov	al,GRAF_DATA_ROT
	out	dx,ax		; the combine function set to REPLACE(ie., set)
	
	mov	ax,0ff00h + GRAF_BIT_MASK
	out	dx,ax		; enable all bits

; GRAPHICS CONTROLLER ADDRESS register will retain the address of BIT MASK reg

	pop	ax		; get back the XORed color
	xchg	al,ah		; get the mask into al             
	mov	dl,SEQ_DATA	; SEQUENCER DATA REGISTER
	out	dx,al		; enable required  planes
	mov	dl,GRAF_DATA	; load DX properly
	mov	pass_prelude,LineSegOFFSET device_second_pass_of_two
	mov	ax,1

device_first_pass_of_two_ret:

	ret

device_first_pass_of_two endp

;-----------------------------------------------------------------------------;
;		     DEVICE_SECOND_PASS_OF_TWO			              ;
;		     -------------------------				      ;
; Programs the EGA registers for the second pass of a two pass ROP code.      ;
;							                      ;
comment ~
	Steps involved are

		. TmpColor ^= PenXORFlag
		. TmpColor != TmpColor
		. set SEQUENCER MAP MASK REGISTER to the above value

        This enables the planes which were disabled in the first pass

		. set the DATA ROTATE register to XOR combine mode
		. enable all bits in the BIT MASK REGISTER and leave its addr
                  in the GRAPHICS CONTROLLER address register.

end comment ~
;						                             ;
;----------------------------------------------------------------------------;

device_second_pass_of_two proc	near

; restore variables destroyed by the first pass

	mov	ax,cTempScans		;restore no of slices
	mov	cScans,ax
	mov	ax,wError		;restore initial error
	mov	WorkErrTerm,ax
	mov	al,bTempStyleError	;restore style error
	mov	bStyleError,al
	mov	al,bTempStyleMask	;restore style mask
	mov	bStyleMask,al

; setup EGA for pass two

	xor	ah,ah		; will be part of return code
	mov	al,TmpColor
	xor	al,PenXORFlag
	not	al
	and	al,MM_ALL
	jz	ega_get_out	; no bits to XOR so second pass not required
	mov	dx,EGA_BASE + SEQ_DATA
	out	dx,al		; required planes enabled

	mov	dl,GRAF_ADDR
	mov	ax,DR_XOR * 256 + GRAF_DATA_ROT 
	out	dx,ax		; set combine function to XOR

	mov	ax,0ff00h + GRAF_BIT_MASK
	out	dx,ax		; enable all bits

; GRAPHICS CONTROLLER ADDRESS REGISTER retains address of BIT MASK REGISTER

	mov	dl,GRAF_DATA	; load DX properly
	mov	al,1		; to indicate that second EGA pass required 
ega_get_out:

	mov	pass_prelude,LineSegOFFSET device_first_pass_of_two
	ret

device_second_pass_of_two endp

;----------------------------------------------------------------------------;
;		     GET_NEXT_BITMAP_ROP_MASK                                ;
;		     ------------------------			             ;
; This routine is called once before every pass over the BITMAP memory       ;
;								             ;
comment ~

we will here load the AND and XOR masks for the next pass over the BITMAPPed
memory. For each pass we look into the leas significant bit of ROPcolor to
decide how DrawModeIndex is to be manipulated to retrive the mask from the
BitmapMaskTable

end comment ~
; 							                     ;
;----------------------------------------------------------------------------;

get_next_bitmap_ROP_mask proc	near


; save any variables we are going to destroy

	mov	al,NumPlanes		; get the no of planes
	cmp	al,1			; only one plane ?
	jnz	gnbrm_not_mono		; no.
	
; for mono surfaces, load the color into ROPColor

	mov	al,TmpColor		; get the color
	mov	ROPcolor,al		; save it
	jmp	short bm_save_vars	; save variables (incase styled lines)

gnbrm_not_mono:

	cmp	NumPasses,al		; first of the passes ?
	jz	bm_save_vars		; yes, save varibales

; restore saved variables

	mov	ax,cTempScans		;restore no of slices
	mov	cScans,ax
	mov	ax,wError		;restore initial error
	mov	WorkErrTerm,ax
	mov	al,bTempStyleError	;restore style error
	mov	bStyleError,al
	mov	al,bTempStyleMask	;restore style mask
	mov	bStyleMask,al
	mov	dx,wTmpScan		;restore scans left from bottom
	mov	ScansLeftFromBot,dx
	jmp	short bm_next_plane	;advance plane

bm_save_vars:

	mov	ax,cScans
	mov	cTempScans,ax		;store no of slices
	mov	ax,wError		;restore initial error
	mov	WorkErrTerm,ax
	mov	al,bStyleError
	mov	bTempStyleError,al	;store style error
	mov	al,bStyleMask
	mov	bTempStyleMask,al	;store style mask
	mov	dx,ScansLeftFromBot
	mov	wTmpScan,dx		;store scans left from bottom
	mov	al,TmpColor		; color of the pen
	mov	ROPcolor,al		; to be used for ROP mask calculations

bm_next_plane:

; advance start address to next plane. This will not have any effect till the
; next pass

	mov	ax,NextPlane		;offset to next plane
	add	off_SurfaceAddress,ax	;advance it

; load starting selector if this is a huge bitmap

	cmp	wScans,0		;is it huge ?
	jz	not_huge		;no.
	mov	ds,seg_SurfaceAddress	;load ds
	assumes	ds,nothing

not_huge:

	mov	bx,DrawModeIndex	; has the zero based ROP code
	ror	ROPcolor,1		; rotate the color
	jnc	color_bit_is_0		; the color for this plane is 0
	shiftr	bx,2			; IDIV by 4
color_bit_is_0:
	and	bx,3			; MOD 4
	add	bx,bx		        ; double up for WORD retrieval
	mov	ax,[BitmapMaskTable][bx]
	mov	wBitmapROP,ax		; save it

; if ax is zero, there will be no need to draw the line

get_next_bm_ret:

	ret


get_next_bitmap_ROP_mask endp
;									     ;
;----------------------------------------------------------------------------;

include clip.inc
include	plysolid.inc
include polybitm.inc
include polystyl.inc		


sEnd	LineSeg
 	
        END	
