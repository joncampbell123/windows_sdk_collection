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
;		. EGA mono can be treated as a small bitmap at A000h	      ;
;	          and hence in this routine we will basically consider drawing;
;		  to bitmaps.						      ;
;								 	      ;
;		. There are 2 sets of draw routines depending on the device & ;
;                 the type of the line. These are:			      ;
;                             . Solid Lines to BITMAPS (huge and small)       ;
;                             . Styled Lines to BITMAPS (huge and small)      ;
;                 Routines are made separate as the method used to process    ;
;                 the pixels are different.                                   ;
;				                                              ;
;	        . There is one set of MOVE routines, for the  BITMAPs.	      ;
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
;  History:				                                      ;
;	        .  Originally developed              by Tony Pisculli [tonyp] ;
;                  Tue 28-Oct-1986.                                           ;
;				 				              ;
;               .  Major re-write                    by Kent Settle  [kentse] ;
;		   Mon 23-Feb-1987.			                      ;
;           							              ;
;	        .  All lines to be drawn from left   by Kent Settle  [kentse] ;
;                  Wed 08-Apr-1987.			                      ;
;               						              ;
;               .  Huge BITMAP handling              by Kent Settle  [kentse] ;
;                  Thu 30-Apr-1987.			                      ;
;             							              ;
;               .  Rewrite from scratch for the Presentation Manager.         ;
;		   Sat 07-May-1988.                  Chuck Whitmer  [chuckwh] ;
;								              ;
;		.  Total re-write to clean up the existing code and include   ;
;                  optimizations/improvements done for the PresentationManager;
;                  Lots of explanations thrown in to help the reader.         ;
;                  Wed 14-Sep-1988.		     Amit Chatterjee  [amitc] ;
;   									      ;
;            	.  Included the code for clipping to rectangles. The method   ;
;		   of drawing styled lines now use the L-infinity metric as   ;
;		   opposed to the Manhatten metric that was used eralier.     ;
;		   (a documentation the algorithms used in this routine is to ;
;		   be found in line.doc					      ;
;									      ;
;		.  Bug fix -- bug #3066.  Removed multiply by local variable  ;
;		   NumPlanes.  It was never initialized and should have been  ;
;		   the value "1" which pretty much makes the multiply	      ;
;		   unecessary.						      ;
;		   Wed  2-Jan-1991.		     Ray Patrick [raypat]     ;
;							                      ;
; -last modified by-  Ray Patrick [raypat]   Wed Jan 2, 1991 18:05            ;
;-----------------------------------------------------------------------------;
comment ~

 As the EGA monochrome memory can be treated as a BITMAP as A000h, we need not 
 reprogram the EGA registers, as the default programming allows us to write on it

end comment ~
                     
;	The function below will perform private stack checking. In order for
;       the checking to occur, two symbols must be defined prior to the inclu- 
;       -sion of CMACROS.INC. ?CHKSTK must be defined if the CMACROS are to
;       perform stack checking on procedures with local parameters. ?CHKSTKPROC
;       must be defined if private stack checking will be used.
;								
;	The actual body of ?CHKSTKPROC is included in MACROS.MAC
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

.xlist
	include lines.inc
	include ega.inc
        include cmacros.inc
	include egamem.inc
	include gdidefs.inc
	include macros.mac
	include polyline.inc
	include	display.inc
.list

??_out polyline

	externA SCREEN_W_BYTES
	externA ScreenSelector
	externA	SCREEN_WIDTH
	externA	SCREEN_HEIGHT

	externFP LineSeg_check_stack

ifdef	EXCLUSION
	externFP exclude_far   		; exclude area from screen.
	externFP unexclude_far 		; clear excluded area.
endif
	      
	


sBegin		Data

TYPE_IS_HUGE    equ     01000000b	; drawing device is huge bitmap
TYPE_IS_BITMAP  equ     00000000b	; drawing device is small bitmap
TYPE_IS_STYLED  equ     00000001b	; indicates lines are styled
wDistHoriz	equ	40h		; Horiz. scale factor for style mask
wDistVert	equ	55h		; Vert. scale factor for style mask
OPAQUE          equ	2		; indicates OPAQUE background mode
HALF_FLIP	equ	0001h		; half flip done
XY_XCHG		equ	0002h		; X and Y interchanged
Y_FLIP		equ	0004h		; vertical flip done
VERTICAL_FLIP	equ	0100h		; HALF_FLIP XOR Y_FLIP
LEFT_XCHG	equ	0080h		; forced left to right
CLIP_TOBE_DONE	equ	0001h		; clipping done by driver
PS_SOLID	equ	0		; solid lines
PS_NULL		equ	5		; null lines

externB		enabled_flag		; non zero if output to screen allowed

sEnd		Data

createSeg	_LINES,LineSeg,word,public,CODE
sBegin		LineSeg

assumes		cs,LineSeg

; Draw routine addresses for solid lines to BITMAPS

DrawTable_solid_bm      dw      offset Bm_Positive_X
			dw	offset Bm_Diagonal_4Q
			dw	offset Bm_Negative_Y
			dw	offset Bm_Diagonal_4Q
			dw	offset Bm_Positive_X
			dw	offset Bm_Diagonal_1Q
			dw      offset Bm_Positive_Y
			dw	offset Bm_Diagonal_1Q

; Draw routine addresses for styled lines to BITMAPS 

DrawTable_styled_bm     dw	offset StBm_Positive_X
			dw	offset StBm_Diagonal_4Q
			dw	offset StBm_Negative_Y
			dw	offset StBm_Diagonal_4Q
			dw	offset StBm_Positive_X
			dw	offset StBm_Diagonal_1Q
			dw      offset StBm_Positive_Y
			dw	offset StBm_Diagonal_1Q

; Move routine addresses for BITMAPS

MoveTable_bm		dw	offset Bm_Move_Diagonal_4Q
			dw	offset Bm_Move_Positive_X
			dw	offset Bm_Move_Diagonal_4Q
			dw	offset Bm_Move_Negative_Y
			dw	offset Bm_Move_Diagonal_1Q
			dw	offset Bm_Move_Positive_X
			dw	offset Bm_Move_Diagonal_1Q
			dw	offset Bm_Move_Positive_Y

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

cProc	do_polylines,<FAR,PUBLIC,WIN,PASCAL>,<si,di>


	parmD	lp_dst_dev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lp_points		;--> to a set of points
	parmD	lp_phys_pen		;--> to physical pen
	parmD	lp_phys_brush		;--> to physical brush
	parmD	lp_draw_mode		;--> to a Drawing mode
	parmD	lp_clip_rect		;--> to a clipping rectange if <> 0


	localB	BackColor		; background color
	localW	DrawModeIndex		; ROP code
        localB  TmpColor		; pen color
	localB	DeviceFlags		; identifies type of device

; following params are necessary for BITMAPS

	localW	BitmapSegment  		; start segment for small/huge maps
	localW  BitmapOffset            ; offset of start of map
	localW	NextScan		; offset to next scan line in map
	localW	wScans			; no of scan lines in huge map segment
	localW	NextPlane		; offset to next color plane	
	localW	FillBytes		; filler bytes at end of huge segment
	localW	NextSegOff		; offset to next hugemap segment

; following variables are used for styled lines

        localW  StyleFlags		; HIWORD has background mode,LO -- mask
        localB  BackMode		; back ground mode

; following variables are used during the draw routines

	localW	S1	      		; stores principle dir draw
        localW  S1_Move			; routine which handles S1 move
        localW	TempCurSeg              ; a copy of CurSeg
        localW  TempCurByte             ; a copy of CurByte
        localW  TempCurByteSty          ; one more saved for style pass
        localB  TempCurBit              ; a copy of rotating bit mask
	localW	ScansLeftFromBot	; # of scan lines from bottom
	localB  ROPcolor		; temp value of pen color
        localW  wBitmapROP              ; XOR,AND mask for bitmaps
        localB	StylePass		; style gap pass indicator
        localB  bStyleError             ; style error term
        localB  bTempStyleError         ; saves the error term
        localB  bStyleMask              ; rotating style mask
        localB  bTempStyleMask          ; saves the style mask
	localW	StyleAdvOrBypass        ; addr of style advance routine/null
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
	localW	WorkErrTerm		; to keep the working
	localB	NeedExclusion		; scans being excluded

; the following variables define the clipping rectangle
	
	localW	X1			; left margin
	localW	X2			; right margin
	localW	Y1			; bottom margin
	localW	Y2			; top margin

cBegin

	jc	jump_error_exit		; exit with error on stack overflow
	cld
	mov	NeedExclusion,0
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

	mov	cx,[si].bmType		; get the device type
	jcxz	get_bitmap_info 	; calculate params for bitmaps
	or	al,al			; is the screen enabled
	jz	jump_error_exit		; no, exit with error
	

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
	mov	NeedExclusion,1		; have to unexclude later
	call	exclude_far		; exclude the scan from the screen
endif

	lds	si,lp_dst_dev		; DS:SI points to device structure
	assumes	ds,nothing

	jmp	short get_bitmap_info   ; get parameters pertaining to bitmaps


; following two jumps are included to make conditional jumps be within range

jump_exit_output:
	jmp	exit_polyline
jump_error_exit:
	jmp	error_exit


get_bitmap_info:

	mov	DeviceFlags,TYPE_IS_BITMAP  ; assume a small bitmap

; for monochrome BITMAPS, the no of color planes is always 1. This information
; will be used implicitly

	mov	di,word ptr [si].bmWidthPlanes
	mov	NextPlane,di		    ; no of bytes in one plane
	
	mov	di,word ptr [si].bmWidthBytes
	mov	NextScan,di		    ; # of bytes per scan line
	
	mov	di,word ptr [si].bmBits+2
	mov	BitmapSegment,di	    ; start segment of the bitmap
	mov	di,word ptr [si].bmBits
	mov	BitmapOffset,di		    ; offset of the start of the map

	page
	mov	wScans,0ffffh               ; make it 64k for small bitmaps
	mov	NextSegOff,0		    ; initialize for small maps
	mov	FillBytes,0		    ; initialize for smal maps

; check to see if we have a small or huge bitmap. If it is huge then additional
; information has to be calculated,for small bitmaps all the parameters have
; been loaded

	mov	cx,[si].bmSegmentIndex      ; is it a huge bitmap
	jcxz	load_color_info		    ; no, skip the code below

; cx has the segment offset to the next huge bitmap segment

	mov	NextSegOff,cx		    ; save the segment offset
	or	DeviceFlags,TYPE_IS_HUGE    ; set the indicator for huge maps
		
	mov	cx,[si].bmFillBytes   	    ; no of filler bytes at the end
	mov	FillBytes,cx
	mov	cx,[si].bmScanSegment	    
	mov	wScans,cx		    ; no of scan lines per segment

; now load the pen color and line style information

load_color_info: 

	lds	si,lp_phys_pen		    ; DS:SI points to the pen structure
	mov	ah,[si].oem_pen_pcol.SPECIAL
	shiftr	ah,4			    ; get the mono color into lsb
	mov	TmpColor,ah		    ; save the pen color
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

; we shall bypass style advance code for non styled line

	mov	StyleAdvOrBypass,offset style_advance_bypass

	jcxz	start_draw                  ; skip code for solid line styles

; initialization code for styled lines

	mov	StyleAdvOrBypass, offset style_advance
	or	DeviceFlags,TYPE_IS_STYLED  ; set indicator for styled lines
	lds	si,lp_draw_mode		    ; point to the draw mode block
	mov	dx,[si].bkMode		    ; get the background mode
	mov	BackMode,dl		    ; save it
	mov	dl,byte ptr [si].bkColor.SPECIAL
	mov	BackColor,dl		    ; save the background color

start_draw:

	lds	si,lp_points		   ; DS:SI points to point array
	lodsw
	mov	dx,ax			   ; first X coordinate
	lodsw
	mov	cx,ax			   ; first Y coordinate
	push	es		           ; es will be trashed
	cCall	ega_line_dispatch	   ; draws the set of poly lines
	pop	es		           ; restore es

exit_polyline:
	mov	ax,1			   ; success code for bitmaps
	jmp	short exit

error_exit: 
	xor	ax,ax
	errn$	exit
exit:
ifdef	EXCLUSION 
	cmp	NeedExclusion,1
	jnz	no_unexclude		  ; scans were not excluded
	call	unexclude_far		   ; remove any exclusion area
no_unexclude:
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
	lodsw				; get Xf
	mov	di,ax
	lodsw				; get Yf
	push	si			; save table pointer
	mov	si,ax			; (DI:SI) has end point
	mov	cx,dx
	xchg	cx,bx                   ; (BX:CX) has start point
	push	si
	push	di			; save end points
	push	ds			; save table segment
	call	Draw_Line		; draw the line
	pop	ds
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

Draw_Line_Ret:
	ret				; one more line drawn

Draw_Line	endp
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

non_horizontal:
	
;------------------------------------------------------------------------------;
; compute the various slope quantities					       ;
;									       ;
; m      = floor(s)							       ;	       ;
; wFracS = frac(s)  (fraction in units of 1/dy)                                ;
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
;------------------------------------------------------------------------------;

	shr	dx,1			; get frac in units of 1/dy
	add	dx,wFracS 										
	sub	dx,cdy			; subtract 1 in units of 1/dy
	mov	wError,dx		; the first error term
	mov	WorkErrTerm,dx          ; use as a working copy

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
	sub	ax,Xa		; no of pels to set on
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
;               . it rotates the current bit position by 1 bit (or does not)   ;
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
	mov	cScanMove,si		; offset to move to next(prev) scan
	neg	ch			; setting up mask
	mov	cl,ch			; make it a word
	and	si,cx			

; si will be forced to zero if no scan move during step is necessary

	mov	cScanStep,si		; offset used in step routines

; now we will calculate the offset in the display memory of the first point

; first we translate the clipped start point wrt the actual start

	add	bx,StartX				
	add	ax,StartY
	mov	di,bx			; load Xi into di
	xor	dx,dx			; will hold SEGMENT value for MAPS
	
; we will set the no of scans left from bot to a large value for small maps
; large value chosen is 1/2 of wScans for small maps

	mov	ScansLeftFromBot,0ffffh/2
	test	DeviceFlags,TYPE_IS_HUGE
	jz	calculate_for_small	; small bit maps

; calculate address for huge maps here

get_segment_loop:
	sub	ax,wScans		; subtract no of scans/seg from Y
	jc	positioned_in_segment
	add	dx,NextSegOff		; move onto the next segment
	jmp	short get_segment_loop	; continue till postioned in req seg
positioned_in_segment:
	neg	ax			; ax has no of scan lines from bot
	mov	ScansLeftFromBot,ax	; save it
	neg	ax
	add	ax,wScans		; ax now has scan pos from top of seg

; at this point the segment offset has been calculated and we can do the 
; code below for both small and huge maps

calculate_for_small:
	push	dx			; has offset to correct segment
	mul	NextScan		; mutiply relative Y by bytes/scan
	pop	dx			; get back segment value
	add	ax,BitmapOffset		; start offset of map
	add	dx,BitmapSegment	; start segment for map
	mov	TempCurSeg,dx		; save start segment value
	mov	ds,dx			; load ds
	mov	cx,di			; get Xi into cx
	shiftr	di,3			; divide by 8 to get # bytes to move
	add	ax,di			; this is the byte offset
	mov	TempCurByte,ax		; save it
	mov	TempCurByteSty,ax	; save another copy for styled lines
	and	cx,7			; get the bit offset in the byte
	mov	bl,80h			; initial bit mask
	ror	bl,cl			; get the initial bit mask
	mov	TempCurBit,bl		; save it

; now calculate the address of the move and draw routines
; The draw and move tables have 8 addresses each. We have 2 draw table: tables
; solid lines to MAPs and styled lines to maps. The addresses are stored such
; that the LONIBBLE of XformFlags gives the index into the table. And the order
; of the table is such that the LONIBBLE of DeviceFlags give us the table no to 
; use (0 or 1)

	xor	ax,ax
	mov	al,DeviceFlags
	and	al,0fh			; get the low nibble
	shiftl	al,4			; * 16, each table of 16 bytes
	mov	si,ax			; si selects the table
	mov	bx,XformFlags		; get flags
	and	bx,0fh			; index is the last nibble
	shl	bx,1			; each entry 2 bytes
	mov	ax,[DrawTable_solid_bm][si][bx]
	mov	S1,ax			; the draw direction
	mov	ax,[MoveTable_bm][bx]
	mov	S1_Move,ax		

; for BITMAPS calculate the masks for the ROP codes

	mov	al,TmpColor		; color of the pen
	mov	ROPcolor,al		; to be used for ROP mask calculations
	call	get_next_bitmap_ROP_mask

save_vars:

; save the variables wchich will be used for style draw pass
	mov	al,TmpColor
	mov	SaveColor,al		; save forground color
	mov	ax,cScans
	mov	cTempScans,ax		; save it
	mov	al,bStyleError		; error at start
	mov	bTempStyleError,al	; save it
	mov	al,bStyleMask		; mask at start of line
	mov	bTempStyleMask,al	; save it
	mov	StylePass,0		; reset style pass var

	mov	si,cScanMove		; offset to move to next scan
	mov	di,TempCurByte		; current byte offset
do_another_pass:
	mov	ax,TempCurSeg		; get the start segment
	mov	ds,ax
	mov	dx,ScansLeftFromBot
	mov	bl,TempCurBit		; bl has current bit mask
	mov	ax,cTempScans		; the initial value of cScans
	mov	cScans,ax		; reload it
	
	or	ax,ax			; is cScans = 0
	jz	do_the_last_scan	; one slice line

; draw the first slice

	mov	cx,Ca			; first slice length
draw_loop:
	call	S1			; draw the slice
	call	S1_Move			; step to the next slice
	call	StyleAdvOrBypass	; advance style for styled lines
	
	dec	cScans			; one more scan done
	jz	do_the_last_scan

; get the intermediate run length and the new error term

	mov	cx,cm			; m
	mov	ax,WorkErrTerm 		; the error term
	or	ax,ax
	js	last_err_negative
	inc	cx			; one more pel to draw

;------------------------------------------------------------------------------;
; note that we had had,							       ;
; fn+1' = fn' + frac(s) - 1        if fn' positive                             ;
;       = fn' + frac(s)            if fn' negative                             ;
;									       ;
; the fractions are all in units of 1/dy, so 1 in the above equation will be dy;
;------------------------------------------------------------------------------;

	sub	ax,cdy			; add '1' in units of 1/dy
last_err_negative:
	add	ax,wFracS		; this is the new error
	mov	WorkErrTerm,ax 		; save it
	jmp	short draw_loop		

do_the_last_scan:
	mov	cx,Cb			; the last slice length
	call	S1			; draw the slice

; for styled lines and opaque background modes we need another pass

	test	DeviceFlags,TYPE_IS_STYLED
	jz	return_from_engine	; we are done
	cmp	BackMode,OPAQUE		; opaque background ?
	jne	return_from_engine	; do not have to draw gaps
	test	StylePass,1		; is it the first style pass ?
	jz	style_draw_pass		; yes

; we have completed the style pass also

	xor	StylePass,1		; reset indicator
	mov	al,SaveColor		; saved value of foreground color
	mov	TmpColor,al		; reset it
	jmp	short return_from_engine		

; we begin the style draw pass for the gaps

style_draw_pass:
	xor	StylePass,1		; set indicator
	mov	al,BackColor		; background color
	mov	TmpColor,al		
	mov	ROPcolor,al		; to be used for BITMAP masks
	mov	di,TempCurByteSty	; initial byte offset
	mov	TempCurByte,di		; reload it into work var
	mov	ax,wError		; initial ereror term
	mov	WorkErrTerm,ax 		; load it into work variable
	call	get_next_bitmap_ROP_mask  ; get the AND,XOR flags
	mov	al,bTempStyleError	; error at start of draw
	mov	bStyleError,al		; reload it
	mov	al,bTempStyleMask	; mask at start of draw
	not	al			; complement of mask for gaps
	mov	bStyleMask,al		; load mask for gaps
	mov	bTempStyleMask,al	; next pass will get from here
	jmp	do_another_pass	        ; start afresh

return_from_engine:
	ret

Draw_Engine	endp
;-----------------------------------------------------------------------------;
;		        STYLE_ADVANCE                   		      ;
;			-------------					      ;
;  Advances the style error term and the mask after the side step	      ;
;-----------------------------------------------------------------------------;

style_advance	proc	near

	mov	cl,bStyleError		; the current error term
	xor	ch,ch
	add	cx,SideStep		; add the step error term
	mov	bStyleError,cl		; update
	mov	cl,ch			; rotate if ch nonzero
	rol	bStyleMask,cl		; rotate mask
style_advance_bypass:
	ret

style_advance	endp
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

	mov	bx,DrawModeIndex	; has the zero based ROP code
	ror	ROPcolor,1		; rotate the color
	jnc	color_bit_is_0		; the color for this plane is 0
	shiftr	bx,2			; IDIV by 4
color_bit_is_0:
	and	bx,3			; MOD 4
	add	bx,bx		        ; double up for WORD retrieval
	mov	ax,[BitmapMaskTable][bx]
	mov	wBitmapROP,ax		; save it
	ret

get_next_bitmap_ROP_mask endp
;									     ;
;----------------------------------------------------------------------------;

include clip.asm
include polybitm.asm
include polystyl.asm		


sEnd	LineSeg
 	
        END	
