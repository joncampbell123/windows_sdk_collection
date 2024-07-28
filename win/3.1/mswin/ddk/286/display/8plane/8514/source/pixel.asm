        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	PIXEL.ASM
;
; This module contains the Set/Get Pixel routine.
;
; Created: 22-Feb-1987
; Author:  Walt Moore [waltm]
; Modified for Hires drivers by Fred Einstein [fredei]  4-Nov-1987
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	Pixel
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
;   Pixel is used to either set a pixel to a given color with the
;   current binary raster operation, or to return the color of the
;   the pixel at the given location.
;
; Restrictions:
;
;-----------------------------------------------------------------------;
.286c

	.xlist
	include cmacros.inc
        .list

	??_out	Pixel

subttl          Data Segment Definitions
page +
sBegin          data
externD 	PixelAddress
sEnd            data

subttl          Pixel Code Segment Definitions
page +
createSeg _OUTPUT,OutputSeg,word,public,CODE
sBegin  OutputSeg
assumes cs,OutputSeg

;--------------------------Exported-Routine-----------------------------;
; Pixel
;
;   Set or Get a Given Pixel
;
;   The given pixel is set to the given color or the given pixel's
;   physical color is returned.
;
;   The physical device may be the screen or a monochrome bitmap.
;
;   If lp_draw_mode is NULL, then the physical color of the pixel is
;   returned.  If lp_draw_mode isn't NULL, then the pixel will be set
;   to the physical color passed in, combined with the pixel already
;   at that location according to the raster-op in lp_draw_mode.  Pixel
;   doesn't pay attention to the background mode.
;
;   No clipping of the input value is required.  GDI clips the
;   coordinate before it is passed in, for both Set and Get.
;
; Entry:
; Returns:
;	DX:AX = physical color if get pixel
;	DX:AX = positive if no error and set was OK.
; Error Returns:
;	DX:AX = 8000:0000H if error occured
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	exclude_far
;	unexclude_far
; History:
;	Sat 31-Oct-1987 00:21:06 -by-  Walt Moore [waltm]
;	Added clipping of the (X,Y)
;
;	Tue 18-Aug-1987 14:50:37 -by-  Walt Moore [waltm]
;	Added test of the disabled flag.
;
;	Tue 03-Mar-1987 20:42:07 -by-  Kent Settle [kentse]
;	Moved a mov instruction in EGA ROP handling code.
;
;	Sun 22-Feb-1987 16:29:09 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

public	rot_bit_tbl
rot_bit_tbl     label   byte
		db	10000000b	;Table to map bit index into
		db	01000000b	;  a bit mask
		db	00100000b
		db	00010000b
		db	00001000b
		db	00000100b
		db	00000010b
		db	00000001b


	assumes ds,Data
	assumes es,nothing


cProc	Pixel,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

        parmD   lpDevice                ;Pointer to device
        parmW   x                       ;X coordinate of pixel
        parmW   y                       ;Y coordinate of pixel
        parmD   PhysColor               ;Physical color to set pixel to
        parmD   lpDrawMode              ;Drawing mode to use, or null if Get

	localW	Rop2Index
        localB  ColourFormat            ;from the PDEVICE structure

cBegin
	jmp	dword ptr [PixelAddress];far jump to the proper Pixel routine
cEnd	<nogen>

sEnd    OutputSeg
end
