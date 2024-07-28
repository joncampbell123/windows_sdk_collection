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
;		   Wed 28-Sep-1988.		     Amit Chatterjee [amitc]  ;
;								              ;
;              .   Adapted the interleaved scan line format for small bitmaps ;
;                  also. We shall still maintain separate identities for small;
;                  and huge maps, but the offset to the next scan line and to ;
;                  the next plane will be the same for both the formats.      ;
;		   Fri 14-Sep-1988		     Amit Chatterjee [amitc]  ;
;									      ;
;		.  Adapted to the 8514 driver to perform solid lines into     ;
;		   memory bitmaps.					      ;
;		   Jan 1990			     Gnter Zieber [GunterZ]  ;
;							                      ;
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
;	The actual body of ?CHKSTKPROC is included in MACROS.MAC
;
;	Since the stack-checking code is designed to work quickly
;	as a near subroutine, and normally resides in segment Code, a
;	duplicate is included in the segment _LINES.  To reach this,
;	the macro ?CHKSTKNAME is defined.

.286p					;a safe assumption with the 8514

incLogical	= 1			;Include GDI Logical object definitions
incDrawMode	= 1			;Include GDI DrawMode definitions
incOutput	= 1			;Include GDI Output definitions

.xlist
	include lines.inc
	include 8514.inc
        include cmacros.inc
	include gdidefs.inc
	include macros.mac
	include	polyline.inc
.list

??_out polyline

ifdef PALETTES
	externB PaletteModified
	externFP TranslateBrush		;'on-the-fly' translation of brush
	externFP TranslatePen		;'on-the-fly' translation of pen
	externFP TranslateTextColor	;'on-the-fly' translation of textcol
endif
	
ifdef	EXCLUSION
	externFP    CursorExclude	; exclude area from screen.
	externFP    CursorUnExclude	; clear excluded area.
endif
	      
	

TYPE_IS_DEV	equ	10000000b	; indicates drawing device is screen
TYPE_IS_HUGE    equ     01000000b	; drawing device is huge bitmap
TYPE_IS_BITMAP	equ	00000000b
TYPE_IS_MONOBMP equ	00000001b
TYPE_IS_STYLED  equ     00000001b	; indicates lines are styled
OPAQUE          equ	2		; indicates OPAQUE background mode
HALF_FLIP	equ	0001h		; half flip done
XY_XCHG		equ	0002h		; X and Y interchanged
Y_FLIP		equ	0004h		; vertical flip done
VERTICAL_FLIP	equ	0100h		; HALF_FLIP XOR Y_FLIP
LEFT_XCHG	equ	0080h		; forced left to right
CLIP_TOBE_DONE	equ	0001h		; clipping done by driver
PS_SOLID	equ	0		; solid lines
PS_NULL		equ	5		; null lines


ifdef	_PLANE_8
createSeg   _DEIGHT, DynamicEight, word, public, CODE
sBegin	    DynamicEight

assumes cs, DynamicEight
assumes ds, nothing

externW     DrawModeTbl8
DrawModeTable	equ	DrawModeTbl8
endif

ifdef	_PLANE_4
createSeg   _DFOUR, DynamicFour, word, public, CODE
sBegin	    DynamicFour

assumes cs, DynamicFour
assumes ds, nothing

externW     DrawModeTbl4
DrawModeTable	equ	DrawModeTbl4
endif

; the following stores the address of the draw routines for solid EGA lines

;Draw routine addresses for solid lines into color bitmaps

DrawTable_solid_bmColor label	word
ifdef	_PLANE_8
			dw	offset Bm8_Positive_X
			dw	offset Bm8_Diagonal_4Q
			dw	offset Bm8_Negative_Y
			dw	offset Bm8_Diagonal_4Q
			dw	offset Bm8_Positive_X
			dw	offset Bm8_Diagonal_1Q
			dw	offset Bm8_Positive_Y
			dw	offset Bm8_Diagonal_1Q
endif
ifdef	_PLANE_4
			dw	offset Bm4_Positive_X
			dw	offset Bm4_Diagonal_4Q
			dw	offset Bm4_Negative_Y
			dw	offset Bm4_Diagonal_4Q
			dw	offset Bm4_Positive_X
			dw	offset Bm4_Diagonal_1Q
			dw	offset Bm4_Positive_Y
			dw	offset Bm4_Diagonal_1Q
endif

; Draw routine addresses for solid lines to monochrome BITMAPS

DrawTable_solid_bm      dw      offset Bm_Positive_X
			dw	offset Bm_Diagonal_4Q
			dw	offset Bm_Negative_Y
			dw	offset Bm_Diagonal_4Q
			dw	offset Bm_Positive_X
			dw	offset Bm_Diagonal_1Q
			dw      offset Bm_Positive_Y
			dw	offset Bm_Diagonal_1Q

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

ifdef	_PLANE_4
cProc	MemoryPolyline4, <FAR, PUBLIC>
endif

ifdef	_PLANE_8
cProc	MemoryPolyline8, <FAR, PUBLIC>
endif


	parmD	lp_dst_dev		;--> to the destination
	parmW	style			;Output operation
	parmW	count			;# of points
	parmD	lp_points		;--> to a set of points
	parmD	lp_phys_pen		;--> to physical pen
	parmD	lp_phys_brush		;--> to physical brush
	parmD	lp_draw_mode		;--> to a Drawing mode
	parmD	lp_clip_rect		;--> to a clipping rectange if <> 0


	localB	BackColor		; background color
	localB	bBitsPixel		; either 4 or 8
	localW	DrawModeIndex		; ROP code
        localB	PenANDFlag		; AND mask for EGA ROP codes
        localB  PenXORFlag		; XOR mask for EGA ROP codesP
        localB  DataROTFlag		; combine function for EGA ROP codes
        localB  SingleFlag		; no of passes required for EGA
	localB	TmpColor		; pen color
	localB	TmpMono 		; pen color's mono bit
	localB	DeviceFlags		; identifies type of device

; following params are necessary for BITMAPS

	localW	BitmapSegment  		; start segment for small/huge maps
	localW  BitmapOffset            ; offset of start of map
	localW	NextScan		; offset to next scan line in map
	localW	wScans			; no of scan lines in huge map segment
	localW	wRop2			;offset to function that implements ROP
	localB	NumPlanes		; no of color planes in map
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
	localB  ROPcolor		; temp value of pen color
	localW	ScansLeftFromBot	; # of scan lines from bottom
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
	localW	StyleAdvOrBypass        ; addr of style advance routine/null

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
	localB	cRotation		; no of bits to rotate on sidestep
	localW	cScans			; no of scans lines for pseudo line
	localW	cTempScans		; save for later passes
	localW	wDistDiag		; style error add for diag draw
	localW	ClipFlag		; clip to be done indicator
	localW	SideStep		; side step contribution to error
	localW	WorkErrTerm		; work variable for error term

; the following variables define the clipping rectangle
	
	localW	X1			; left margin
	localW	X2			; right margin
	localW	Y1			; bottom margin
	localW	Y2			; top margin

cBegin
if 0					;we don't have any stack checking
	jnc	stack_space_ok
	jmp	jump_error_exit		; exit with error on stack overflow
stack_space_ok:
endif

	cld				;don't take any chances
;	 mov	 al, BitsPixel		 ;this contains the number of planes
;	 mov	 bBitsPixel, al 	 ; that are installed (4 or 8)

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

	mov	ClipFlag,0		;initialize this flag (WORD)
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
	lds	si,lp_dst_dev		; DS:SI points to Pdevice structure
	assumes	ds,nothing

	mov	di,word ptr [si].bmBits+2
	mov	TempCurSeg,di		; store segment value
	mov	cx,[si].bmType		; get the device type
	jcxz	get_bitmap_info 	; calculate params for bitmaps
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

polyline_exclude_scan:			;exclude the cursor from the screen
	cCall	CursorExclude, <cx, dx, si, di>
endif
	jmp	short load_color_info   ; get parameters pertaining to color 


; following two jumps are included to make conditional jumps be within range

jump_exit_output:
	jmp	exit_polyline
jump_error_exit:
	jmp	error_exit


get_bitmap_info:

	mov	DeviceFlags,TYPE_IS_BITMAP  ; assume a small bitmap
;	 mov	 cl,[si].bmPlanes	     ; get the no of color planes
	mov	cl, [si].bmBitsPixel	    ;get # of bits per pixel
	mov	NumPlanes,cl		    ; save it
	mov	al, cl
	and	al, 01h
	or	bptr DeviceFlags[0], al
	.errnz	TYPE_IS_MONOBMP-1

;	 mov	 TempNumPass,cl 	     ; save a copy of it
;
;	 mov	 di,word ptr [si].bmWidthPlanes
;	 mov	 NextPlane,di		     ; no of bytes in one plane

	mov	di,word ptr [si].bmWidthBytes
	mov	NextScan,di		    ; # of bytes per scan line
	
	mov	di,word ptr [si].bmBits+2
	mov	BitmapSegment,di	    ; start segment of the bitmap
	mov	di,word ptr [si].bmBits
	mov	BitmapOffset,di		    ; offset of the start of the map

; anticipating small bitmaps we will initialize some of the variables

	mov	wScans,0ffffh		    ; assume 64k scan lines!
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
; of color planes.  The 8514 has packed pixel bitmaps so the offset to the
; next scan is simply bmWidthBytes, which is stored in NextScan

merge_point_for_small:

; now load the pen color and line style information

load_color_info: 

	lds	si,lp_phys_pen		    ; DS:SI points to the pen structure
	mov	cx, [si]		    ;get pen style and color into CL,CH
	mov	TmpColor, ch		    ;save pen color
	mov	al, [si][2]		    ;AL: pen color's mono bit
	mov	TmpMono, al		    ;save for later
	sub	ch, ch			    ;make CH zero now
	or	cx, cx			    ;can do only solid polylines
	jnz	jump_error_exit
	.errnz	LS_SOLID		    ;make sure assumption is valid
	mov	LineStyle,cl		    ; save style


; get information about the ROPs

	lds	si,lp_draw_mode		    ; DS:SI points to the drawmode block
	mov	bx,[si].Rop2		    ; get the ROP code
	dec	bx			    ; make it zero based
	and	bx,RASTER_OP_MASK	    ; only 4 bits are significant
	mov	DrawModeIndex,bx	    ; save a copy
	shl	bx, 1			    ;make it a word offset
	mov	ax, cs:[bx].DrawModeTable   ;get function to do color ROPs
	mov	wRop2, ax		    ;save it locally
if 0
	mov	si,cx			    ; the passed in line style
	shl	si,1			    ; 2 bytes of mask per style
	mov	ax,wptr cs:style_table[si]  ; get the style mask
	mov	wStyleMask,ax		    ; save both the mask
	mov	wStyleCounter,0		    ; initialize it

; we will normally work with the mask for forward progressing line

	mov	bStyleMask,al		    ; save it

; we shall bypass style advance code for non styled line

	mov	StyleAdvOrBypass,offset style_advance_bypass

	jcxz	polyline_setup_ega	    ; skip code for solid line styles

; initialization code for styled lines

	mov	StyleAdvOrBypass, offset style_advance
	or	DeviceFlags,TYPE_IS_STYLED  ; set indicator for styled lines
;	 lds	 si,lp_draw_mode	     ; point to the draw mode block
	mov	dx,[si].bkMode		    ; get the background mode
	mov	BackMode,dl		    ; save it
	mov	dl,bptr [si].bkColor	    ;this is just the bk color index
	mov	BackColor,dl		    ; save the background color

; get the ROP AND and XOR masks for the EGA 

polyline_setup_ega:
endif
	test	DeviceFlags,TYPE_IS_DEV	    ; test for the device type
	jz	init_params_for_bm	    ; skip code below for bitmaps
	int	3			    ;we should NEVER reach this point
if 0
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

	mov	TempNumPass,1		    ; assume single pass 

; for style passes also the EGA has to be reinitialized, assume one pass

	mov	style_pass_init, offset device_one_pass_init

	test	SingleFlag,SINGLE_OK	    ; is it really single pass ?
	jnz	yes_single_pass
	inc	TempNumPass		    ; two pass
	mov	ax,offset device_first_pass_of_two
	mov	style_pass_init,ax	    ; has to be called bef style pass  
	call	ax			    ; initialize EGA for first pass
	jmp	short setup_pass_prelude

yes_single_pass:
	call	device_one_pass_init        ; initialize EGA for one pass

setup_pass_prelude:
	          
; for two pass ROPs the prelude procedure to be called before each pass is that
; which initializes the EGA for the second pass, set up its address. for single
; pass ROPs this will not be required at all

	mov	pass_prelude,offset device_second_pass_of_two
	jmp	short start_draw           ; get into the draw routines
endif

; for bitmaps, before each pass over the map the AND,XOR flags has to be    
; calculated from the color bit corresponding to the plane in question. This
; is also to be done before the style passes. Set up the address of the routine
; which does this

init_params_for_bm:

	mov	ax,offset get_next_bitmap_ROP_mask
	mov	pass_prelude,ax
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
if 0
; restore the EGA back to the defaults

	mov	dx,EGA_BASE+GRAF_ADDR
	mov	ax,GRAF_ENAB_SR		   ; disable all planes for set/reset
	out	dx,ax

	mov	ax,DR_SET*256 + GRAF_DATA_ROT
	out	dx,ax			   ; set the combine fn to set/no rot
	
	mov	ax,MM_ALL  
	mov	dl,SEQ_DATA
	out	dx,al			   ; enable all planes
endif
device_exit:
	xor	ax,ax
	inc	ax
	jmp	short exit

error_exit: 
	xor	ax,ax
	errn$	exit
exit:
ifdef	EXCLUSION 
	call	CursorUnExclude 	   ; remove any exclusion area
endif
 
polyline_return:
cEnd	  

;-----------------------------------------------------------------------------;
;                        EGA_LINE_DISPATCH		                      ;
;                        -----------------			              ;
; 									      ;
; Draws the set of polylines after all parameters for the target device and   ;
; various masks have been calculated.					      ;
; 								              ;
; Inputs:							              ;
;                  CX    ---	first Y coordinate			      ;
; 		   DX    ---	first X coordinate		              ;
;               DS:SI    ---	points to table of subsequent poits           ;
;	    assumes that various masks and scanline next plane details        ;
;           for BITMAPS are already loaded.			              ;
;									      ;
;-----------------------------------------------------------------------------;

cProc	ega_line_dispatch, <NEAR>
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

; above routine will clip the line against the clipping rectangle and will set
; the clipping boundray to the inclusive interval [Xa,Xb]

	jmp	short region_clip_done

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
if 0					;don't support styled lines
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
	xchg	di,si			; get them back for the time being
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

; AX has f1, CX has f2,  total add factor z = f1 * (major-minor) + minor * Y
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
endif

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
	mov	Ca,ax			; save Ca

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
	mov	WorkErrTerm,dx		    ; es will be used as work err register

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
if 0					;don't support styled lines
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
endif
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
; we will set it to 1/2 of wScans for small bitmaps

	mov	ScansLeftFromBot,0ffffh/2
	test	DeviceFlags,TYPE_IS_DEV
	jnz	calculate_for_ega	; target is ega
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
	mov	ds,dx			; DS:AX-->1st scanline to draw at
	jmp	short calculate_effect_of_Xi

calculate_for_ega:
	int	3			;we should NEVER get to this point
if 0
; for EGA the ScanLeftFromBot var will hold the address of the GRAPHICS 
; controller DATA REGISTER

	mov	ScansLeftFromBot,EGA_GRAPH+1
	xor	dx,dx
	mov	NextPlane,dx		; for EGA next plane concept is invalid
	mov	dx,TempCurSeg		; for EGA segment value saved
	mov	ds,dx			; load ds
	mul	NextScan
endif
calculate_effect_of_Xi:
	cmp	NumPlanes, 1		;what kind of bitmap are we dealing
	je	do_mono_stuff		; with?  Monochrome?
ifdef	_PLANE_4
	cmp	NumPlanes, 4
	je	do_four_plane_stuff	;four planes?
endif
ifdef	_PLANE_8
	cmp	NumPlanes, 8
	je	do_eight_plane_stuff	;eight planes?
endif
	jmp	return_from_engine	;if none of the above something is wrong

do_mono_stuff:
	mov	cx,di			; get Xi into cx
	shiftr	di,3			; divide by 8 to get # bytes to move
	add	ax,di			; this is the byte offset
	mov	TempCurByte,ax		; save it
	mov	TempCurByteSty,ax	; save another copy for styled lines
	and	cx,7			; get the bit offset in the byte
	mov	bl,80h			; initial bit mask
	ror	bl,cl			; get the initial bit mask
	mov	TempCurBit,bl		; save it
	mov	bl, TmpMono
	shl	bl, 1			;put mono bit into CY
	sbb	bl, bl
	mov	TmpColor, bl
	jmp	short get_draw_routine
ifdef	_PLANE_4
do_four_plane_stuff:
	mov	bl, 0aah		;initial nibble mask (byte aligned)
	shiftr	di, 1			;compute the byte offset
	jnc	phase_byte_aligned
	not	bl
phase_byte_aligned:
	add	ax, di			;add to offset into bitmap
	mov	TempCurByte,ax		; save it
	mov	TempCurByteSty,ax	; save another copy for styled lines
	mov	TempCurBit, bl		;save nibble mask
	mov	bl, TmpColor		;Copy the color index from the low
	and	bl, 0fh 		;strip off accelerator bits
	mov	bh, bl			; nibble into the high nibble and
	shl	bl, 4			; save it in TmpColor
	or	bl, bh
	mov	TmpColor, bl
endif
ifdef	_PLANE_8
do_eight_plane_stuff:
	add	ax, di
	mov	TempCurByte,ax		; save it
	mov	TempCurByteSty,ax	; save another copy for styled lines
	mov	TempCurBit, -1		;enable all 8 bits
endif

; now calculate the address of the move and draw routines
; The draw and move tables have 8 addresses each. We have 4 draw table: tables
; for solid lines to EGA, styled line sto EGA, solid lines to MAPs and styled
; lines to maps. The addresses are stored such that the LOBYTE of XformFlags
; gives the index into the table. And the order of the table is such that the 
; LOWNIBBLE of DeviceFlags give us the table no to use (0 to 3)

get_draw_routine:
	xor	ax,ax
	mov	al,DeviceFlags
	and	al,0fh			; get the low nibble
;	 push	 ax			 ; save it
	shiftl	al,4			; * 16, each table of 16 bytes
	mov	si,ax			; si selects the table
	mov	bx,XformFlags		; get flags
	and	bx,0fh			; index is the last nibble
	shl	bx,1			; each entry 2 bytes
	mov	ax,[DrawTable_solid_bmColor][si][bx]
	.errnz	DrawTable_solid_bm-DrawTable_solid_bmColor-16
	mov	S1,ax			; the draw direction
;	 pop	 ax			 ; get back device flag
;	 shr	 al,1			 ; take out styl info
;	 .errnz  TYPE_IS_STYLED-1	 ;this is what we assume here
;	 shiftl  al,4			 ; multiply by 16
;	 mov	 si,ax
	mov	ax,[MoveTable_bm][bx]
	mov	S1_Move,ax		

; for BITMAPS calculate the masks for the ROP codes

	test	DeviceFlags,TYPE_IS_MONOBMP
	jz	save_vars
	test	DeviceFlags,TYPE_IS_DEV	; is it screen ?
	jnz	save_vars
	mov	al,TmpColor		; color of the pen
	mov	ROPcolor,al		; to be used for ROP mask calculations
	call	get_next_bitmap_ROP_mask

save_vars:

; save the variables wchich will be used for style draw pass
	mov	al,TmpColor
	mov	SaveColor,al		; save forground color
	mov	ax,cScans
	mov	cTempScans,ax		; save it
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
;	 call	 StyleAdvOrBypass	 ; advance style for styled lines
	
	dec	cScans			; one more scan done
	jz	do_the_last_scan

; get the intermediate run length and the new error term

	mov	cx,cm			; m
	mov	ax,WorkErrTerm		    ; the error term
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
	mov	WorkErrTerm,ax		    ; save it
	jmp	short draw_loop		

do_the_last_scan:
	mov	cx,Cb			; the last slice length
	call	S1			; draw the slice
if 0
	dec	NumPasses		; one more pass done
	jz	all_passes_done
	
; before the next pass we will either have to reinitialize the ega registers
; in case of draw to ega, or re genarate the AND,XOR flags in case of bitmaps
; The address of the relevant routine has been saved in pass_prelude

	call	pass_prelude

; if ax returns 0, then this pass need not be done as it is not going to 
; alter any bits

	or	ax,ax			; test return code
	jz	all_passes_done
	
; load the variables destroyed by the first pass

	mov	di,TempCurByte		; byte offset at start of prev pass
	add	di,NextPlane		; move onto next plane
	mov	TempCurByte,di		; save it
	mov	ax,wError		; the initial error term
	mov	WorkErrTerm,ax		    ; reset the work variable

	test	DeviceFlags,TYPE_IS_STYLED
	jz	do_another_pass		; do the next pass

; for styled lines reset the error term and the style mask

	mov	al,bTempStyleError
	mov	bStyleError,al
	mov	al,bTempStyleMask
	mov	bStyleMask,al		; style variables 
	jmp	short do_another_pass
endif
all_passes_done:
if 0
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
	mov	TempCurByte,di		; reload it into work var
	mov	ax,wError		; initial ereror term
	mov	WorkErrTerm,ax		    ; load it into work variable
	call	style_pass_init		; initialization before style pass
	mov	al,bTempStyleError	; error at start of draw
	mov	bStyleError,al		; reload it
	mov	al,bTempStyleMask	; mask at start of draw
	not	al			; complement of mask for gaps
	mov	bStyleMask,al		; load mask for gaps
	mov	bTempStyleMask,al	; next pass will get from here
	jmp	do_another_pass	        ; start afresh
endif
; reset the ega registers in case of draw to ega

reset_ega:
;	 test	 DeviceFlags,TYPE_IS_DEV ; is it EGA ?
;	 jz	 return_from_engine	 ; no
;	 call	 style_pass_init	 ; initialize for next pass
return_from_engine:
	ret

Draw_Engine	endp
;-----------------------------------------------------------------------------;
;		        STYLE_ADVANCE                   		      ;
;			-------------					      ;
;  Advances the style error term and the mask after the side step	      ;
;-----------------------------------------------------------------------------;
if 0
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

	mov	al,1		; to indicate that second EGA pass required 
ega_get_out:
	ret

device_second_pass_of_two endp
endif

;----------------------------------------------------------------------------;
;		     GET_NEXT_BITMAP_ROP_MASK                                ;
;		     ------------------------			             ;
; This routine is called once before every pass over the BITMAP memory       ;
;								             ;
comment ~

we will here load the AND and XOR masks for the next pass over the BITMAPPed
memory. For each pass we look into the least significant bit of ROPcolor to
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

; if ax is zero, there will be no need to draw the line

	ret


get_next_bitmap_ROP_mask endp
;									     ;
;----------------------------------------------------------------------------;

include clip.asm
;include plysolid.asm			;don't draw onto device surface
include polydraw.asm
;include polystyl.asm			;styled lines not supported on 8514

ifdef	_PLANE_8
sEnd	DynamicEight
endif
ifdef	_PLANE_4
sEnd	DynamicFour
endif

END
