page            ,132
title           Enumerate the 8514's Pens and Brushes
.286c


.xlist
include CMACROS.INC
incLogical = 1		;include logical pen/brush objects in gdidefs.inc
include gdidefs.inc
include 8514.INC
.list

externA DefaultLength	;palette.dat
externA ExtraPalLength	;palette.dat

DoubleWord  struc
    lo	dw  ?
    hi	dw  ?
DoubleWord  ends

LogicalColor	struc
    lcRed   db	0
    lcGreen db	0
    lcBlue  db	0
    lcSpare db	0
LogicalColor	ends


subttl          Data Area
page +
sBegin		Data

externB Palette 			;in PALETTE.DAT, DATA.ASM, INIT.ASM
externB BitsPixel			;in Data.asm

sEnd            Data

subttl          Code Area
page +
createSeg	_ENUM,EnumSeg,word public,CODE
sBegin		EnumSeg
assumes 	cs,EnumSeg
assumes 	ds,Data

cProc	GetSystemColors, <NEAR, PUBLIC>
cBegin
	mov	cx, 16
	mov	si, DataOFFSET Palette
	add	si, DefaultLength-3
	cmp	[BitsPixel], 8
	jne	GSCDone
	add	cx, 4
	add	si, ExtraPalLength
GSCDone:
cEnd

cProc	EnumObj, <FAR, PUBLIC>, <si, di>

        parmD   lpDstDev
        parmW   Style
        parmD   lpCallbackFunc
        parmD   lpClientData

        localD  lpLogObj
	localV	LogicalObject,12
	localW	wStyle
	localW	wHatch

cBegin
	lea	cx,ss:LogicalObject	;set up pointer to LogObj we return
	mov	off_lpLogObj,cx 	;common to pen and brush enumeration
	mov	seg_lpLogObj,ss
	mov	ax,1			;assume it's a bad function
	mov	bx,Style		;get the enumeration type
	dec	bx			;is Style=1=Pen?
	jz	EnumeratePen		;yes, go enumerate the pen
	dec	bx			;is Style=2=Brush?
	jz	EnumerateBrush		;yes, go enumerate the brush
	jmp	EOExit			;if not, it's a bad Style


public  EnumeratePen
EnumeratePen:

;We need to enumerate all of our solid pens.  Programs such as EXCEL assume
;that the first 8 pens returned are the EGA colours.  Thus, we return the
;RGBs of the 8 default EGA colours first.  (assuming the old 3 plane driver)

	cCall	GetSystemColors 	;CX=number of system colors
					;DS:SI-->last palette entry (white)
public	EnumeratePensLoop
EnumeratePensLoop:
	sub	ax, ax
	mov	LogicalObject.lopnStyle, ax
	.errnz	LS_SOLID
	inc	ax
	mov	LogicalObject.lopnWidth.lo, ax	;return pen width=1
	mov	LogicalObject.lopnWidth.hi, ax
	dec	ax
	mov	LogicalObject.lopnColor.lcSpare, al

	lodsw					    ;get the RED + GREEN in DI
	mov	LogicalObject.lopnColor.lo, ax	    ;get in the RED+GREEN
	lodsb					    ;get the BLUE in AL
	sub	si, 6				    ;DS:SI-->next lower color
	mov	LogicalObject.lopnColor.lcBlue,al   ;get in the BLUE

;Now, our LOGPEN is ready to be sent back to the caller:

	push	cx
	push	si
	push	ds
	arg	lpLogObj
	arg	lpClientData
	cCall	lpCallbackFunc		;go call the "caller"
	pop	ds
	pop	si
	pop	cx
	or	ax,ax			;has caller ordered us to stop
					;enumerating?
	jz	EOExit			;yes, we're done enumerating
	loop	EnumeratePensLoop
	jmp	short EOExit		;get out of here!


subttl          Enumerate Solid Brushes
page +         
public  EnumerateBrush
EnumerateBrush:

;We need to enumerate all of our solid brushes.  Programs such as EXCEL assume
;that the first 8 brushes returned are the EGA colours.  Thus, we return the
;RGBs of the 8 default EGA colours first.  (assuming the old 3 plane driver)

;We must also enumerate our hatched brushes.  We do not enumerate all of the
;possible combinations of forground and background colors, just the forground
;colors for each hatch type.


;First Enumerate all Solid brushes
	cCall	GetSystemColors
	sub	ax, ax				;Set Style and hatch to zero.
	mov	wStyle,ax
	mov	wHatch,ax
	cCall	EnumerateBrushesLoop		;Go enumerate all solid colors.
	jc	EOExit				;'C' set if client aborted enumeration.

;Next, enumerate hatched brushes
	mov	wHatch,5			;start with diagonal X-hatch brush (5).
	mov	wStyle,2			;Style is hatched (2).
@@:	cCall	GetSystemColors
	cCall	EnumerateBrushesLoop		;Go enumerate all solid colors.
	jc	EOExit				;'C' set if client aborted enumeration.
	dec	wHatch
	jge	@b
EOExit:
cEnd                                                             

cProc	EnumerateBrushesLoop, <NEAR, PUBLIC>
cBegin
	mov	ax,wStyle
	mov	LogicalObject.lbStyle, ax
	mov	ax,wHatch
	mov	LogicalObject.lbHatch, ax
	.errnz	BS_SOLID
	xor	ax,ax
	mov	LogicalObject.lbColor.lcSpare, al
	mov	LogicalObject.lbBkColor.lo, ax
	mov	LogicalObject.lbBkColor.hi, ax

	lodsw					;get the RED + GREEN in AX
	mov	LogicalObject.lbColor.lo, ax	;get in the RED+GREEN
	lodsb					;get the BLUE in AL
	sub	si, 6
	mov	LogicalObject.lbColor.lcBlue,al ;get in the BLUE

;Now, our LOGBRUSH is ready to be sent back to the caller:

	push	cx				;save the world
	push	ds
	push	es
	arg	lpLogObj
	arg	lpClientData
	cCall	lpCallbackFunc			;go call the "caller"
	or	ax,ax				;has caller ordered us to stop
						;enumerating?
	pop	es				;(restore saved registers)
	pop	ds
	pop	cx
	jz	short @f			;yes, we're done enumerating
	loop	EnumerateBrushesLoop
	clc
	jmp	EBL_Exit
@@:
	stc
EBL_Exit:
cEnd


cProc	EnumDFonts,<FAR,PUBLIC>

        parmD   lpDstDev
        parmD   lpFaceName
        parmD   lpCallbackFunc
        parmD   lpClientData

cBegin

;This device has no hardware fonts.  We therefore return a code of AX = 1:

	mov	ax,1
cEnd

sEnd	EnumSeg
end
