page            ,132
title           OUTPUT routines for the IBM 8514
.286c


.xlist
incOutput	equ	0ffh		;for GDIDEFS inclusion
include         CMACROS.INC
include 	GDIDEFS.INC
include 	drvpal.inc

subttl          Data Area
page +
sBegin	Data
externW FullScreenClip		;in IBMDRV.ASM
;externB BitsPixel
externD MemoryScanline
externD MemoryPolyline
externB PaletteFlags
sEnd	Data

externFP    TranslateDrawMode
externFP    TranslatePen
externFP    TranslateBrush
;externFP    MemoryScanline8
;externFP    MemoryScanline4

subttl          Code Segment Definitions
page +
sBegin          Code
assumes cs,Code
externFP	CursorUnExclude 	;in ROUTINES.ASM
externFP	SetScreenClipFar	;in POLYGON.ASM
sEnd            Code

subttl          Definitions -- Output Segment
page +
createSeg       _OUTPUT,OutputSeg,word,public,CODE
sBegin          OutputSeg
assumes         cs,OutputSeg
assumes         ds,Data

.xlist
include         8514.INC
.list

subttl          External Definitions
page +
externNP	Scanlines		;in SCANLINE.ASM uses lpPBrush
externNP	Polylines		;in POLYLINE.ASM
;externNP	 Polygon		 ;in POLYGON.ASM
externNP	Rectangle		;in RECTNGLE.ASM

subttl          Output Entry
page +
cProc   Output,<FAR,PUBLIC>,<si,di>


include         OUTPUT.INC                      ;contains stack definitions


cBegin
subttl          Determine Destination Device Type
page +

;First get the device type (board or main memory).

	cld				;set direction forward
;	 mov	 al, [BitsPixel]	 ;save number of bits per pixel in
;	 mov	 BandFlags, al		 ;local stack variable
	push	ds			;save our DS
	lds	si,lpDstDev		;get destination PDEVICE struct.
	assumes ds, nothing
	lodsw				;get device type into AX
	mov	bl,[si+7]		;get colour format into BL
	or	ax,ax			;drawing to main memory?
	pop	ds			;(restore saved DS)
	assumes ds, Data
	jnz	OutputToBoard		;nope, do it to the board


subttl		Memory Output Routines
page +
public	MemoryOutput
MemoryOutput:
	mov	ax,0ffffh		;assume we don't support his
					;style
	cmp	Style,OS_SCANLINES	;does he want scanlines?
	jne	MemOutPolyline		;nope, try for polylines
;	 cmp	 BandFlags, 8
;	 jne	 DoMemoryScanline4
;	 cCall	 MemoryScanline8	 ;go do the main memory scanlines
;	 jmp	 short MemOutGoodExit	 ;and we're done!
;DoMemoryScanline4:
;	 cCall	 MemoryScanline4
	cCall	<dword ptr MemoryScanline>
	jmp	short MemOutGoodExit

public	MemOutPolyline
MemOutPolyline:
	cmp	Style,OS_POLYLINE	;does he want polylines?
	jne	OutputQuickExit 	;nope, we don't support anything else
	arg	lpDstDev		;--> to the destination
	arg	Style			;Output operation
	arg	Count			;# of points
	arg	lpPoints		;--> to a set of points
	arg	lpPPen			;--> to physical pen
	arg	lpPBrush		;--> to physical brush
	arg	lpDrawMode		;--> to a Drawing mode
	arg	lpClipRect		;--> to a clipping rectangle if <> 0
	cCall	<dword ptr MemoryPolyline>	;go do the polyline

MemOutGoodExit:
	mov	ax,1			;return success code

OutputQuickExit:
	jmp	OutputEnd		;we're done now


subttl  Output to the IBM 8514
page +          
public  OutputToBoard
OutputToBoard:

	test	[PaletteFlags], BITBLTACCELERATE
	jz	OTBBypassXlate
	push	bx			; all the colors involved are
	push	dx			; translated only before drawing to the
	push	ds			; board
	cCall	TranslatePen, <lpPPen>
	mov	seg_lpPPen, dx
	mov	off_lpPPen, ax
	cCall	TranslateDrawMode, <lpDrawMode>
	mov	seg_lpDrawMode, dx
	mov	off_lpDrawMode, ax
	cCall	TranslateBrush, <lpPBrush>
	mov	seg_lpPBrush, dx
	mov	off_lpPBrush, ax
	pop	ds
	pop	dx
	pop	bx
OTBBypassXlate:
	mov	ax,0ffffh		;assume requested style is
					;unsupported
	mov	di,Style		;get type of figure to draw

public	OTBScanlines
OTBScanlines:
	cmp	di,OS_SCANLINES 	;does he want scanlines?
	jne	OTBPolys		;nope, go check for polylines
					;and polygons
	cCall	Scanlines		;go do scanlines
	jmp	short OTBGoodExit	;and get out

public	OTBPolys
OTBPolys:
	mov	BandFlags,0		;initialize the flags
	cmp	di,OS_POLYLINE		;does he want a polyline?
	je	OTBPolyline		;yes, just go draw it
	cmp	di,OS_POLYGON		;does he want a polygon?
	jne	OTBRectangle		;nope, go try for rectangle
;if 0
public	OTBPolygon
OTBPolygon:
	cmp	seg_lpPBrush,0		;is there any brush?
	je	OTBPolyline		;nope, just draw the outline
	les	si,lpPBrush		;get the brush into ES:DI
	cmp	byte ptr es:[si],1	;is it a hollow brush?
	je	OTBPolyline		;yes, just go do the border

;We have determined that we're doing a filled polygonal figure.  Call
;Polygon to draw the interior

;	 cCall	 Polygon		 ;go draw the interior
	or	ax,ax			;any error?
	jnz	OutputEnd		;yes, return failure to GDI
	mov	BandFlags,0ffh		;tell polyline that we're only
    jmp short OTBExit			;drawing an outline of an
					;already filled polygon
;endif
public	OTBPolyline
OTBPolyline:
	cmp	seg_lpPPen,0		;was a pen passed?
	je	OTBGoodExit		;nope, just leave
	cCall	Polylines		;go do polyline or polygon
					;outline
	jmp	short OTBGoodExit	;and get out

public	OTBRectangle
OTBRectangle:
;	 jmp	 short OTBExit		 ;disable rectangle functions for now
	cmp	di,OS_RECTANGLE 	;does he want a rectangle?
	jne	OTBExit 		;nope, fail and leave
	cCall	Rectangle		;go do a rectangle

public	OTBGoodExit
OTBGoodExit:
	mov	si,DataOFFSET FullScreenClip	;set clipping rect back to
						;full screen
	call	SetScreenClipFar
	cCall	CursorUnExclude 		;go free up cursor exclusion
	mov	ax,1			;return success code

public  OTBExit
OTBExit:       

;At this point, AX has either 1 (success code) or FFFFH if style was not
;supported.

OutputEnd:
cEnd

sEnd    OutputSeg
end
