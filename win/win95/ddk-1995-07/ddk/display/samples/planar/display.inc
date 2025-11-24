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

;-----------------------------Module-Header-----------------------------;
; Module Name:	DISPLAY.INC
;
; This file contains definitions of OEM defined objects used by
; the driver.
;
; Exported Functions:	none
;
; Public Functions:	none
;
; Public Data:		none
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;


EXCLUSION	 =	1		;Assemble cursor exclusion code
GEN_COLOR_BLT	 =	1		;Assemble color BitBLT code

NUMBER_PLANES	equ	4		;3 plane format
;OLD_MEM	 equ	 1		 ;Old planer format for small bitmaps
GEN_COLOR	equ	1		;Generate code for a color driver



;	The color structure is how this driver stores its physical
;	colors in memory.  The color consists of four bytes (dword),
;	one byte for each of three planes of color, and a fourth
;	byte which is a combination of one bit from each plane, plus
;	other special information.
;
;	C3Bit will have to be expanded as needed for the fourth plane.


phys_color	struc
pcol_C0 	db	?		;Plane C0
pcol_C1 	db	?		;Plane C1
pcol_C2 	db	?		;Plane C2
pcol_C3 	db	?		;Plane C3/Monochrome/Special information
phys_color	ends


;	Definitions for the pcol_C3 byte of the physical color
;
;	Some of these definitions have limitations as to when they
;	are valid.  They are as follows:
;
;	C0_BIT		color device, phys color, solid brushes if SOLID_COLOR
;	C1_BIT		color device, phys color, solid brushes if SOLID_COLOR
;	C2_BIT		color device, phys color, solid brushes if SOLID_COLOR
;	C3_BIT		color device, phys color, solid brushes if SOLID_COLOR
;	MONO_BIT	mono  device, phys color
;	ONES_OR_ZEROS	color device, phys color, solid brushes if SOLID_COLOR
;	GREY_SCALE	color device, dithered solid and hatched brushes
;	SOLID_BRUSH	color device, solid brush qualifier
;
;	There may be brushes where the accelerators could have been set,
;	but wasn't.  That's life.


SPECIAL 	equ	pcol_C3 	;Special information is here
C0_BIT		equ	00000001b	;  C0 color
C1_BIT		equ	00000010b	;  C1 color
C2_BIT		equ	00000100b	;  C2 color
C3_BIT		equ	00001000b	;  C3 color
MONO_BIT	equ	00010000b	;  Monochrome bit
ONES_OR_ZEROS	equ	00100000b	;  Color is really all 1's or all 0's
GREY_SCALE	equ	01000000b	;  Indicates a real grey scale brush
SOLID_BRUSH	equ	10000000b	;  Indicates a solid color brush

	errnz	<(size phys_color) - 4> ;Must be a double word

;       Definitions for return value from RealizeObject of a brush.  GDI wants
;       to know if a brush is solid in order to optimize Unrealizations (i.e.
;       GDI won't ask the driver to re-realize a solid brush.)  There are two
;       flags indicating if the brush is solid in mono & color DCs.

GDI_SOLID_COLOR_BRUSH   equ     1
GDI_SOLID_MONO_BRUSH    equ     2

;	The brush structure is OEM dependant, and can contain whatever
;	information that is needed to realize the given brush.
;
;	For this implementation, the brush will consist of an 8x8
;	pattern for each of the planes, and another 8x8 pattern
;	for monochrome devices and for monochrome to color conversion
;	(for something like a hatched brush, this would contain the
;	monochrome mask used to create the brush).
;
;	The style will also be stored in the brush and is used to
;	catch hollow brushes and exit early.
;
;	A flag specific to the EGA is also stored in the brush.  This
;	flag indicates that the brush is a solid brush and that the
;	color for each plane is a solid color (all 1's or all 0's).
;	Patterns which are solid in each plane can be handle as a
;	special case in Bitblt when the raster op is P or Pn.


SIZE_PATTERN	equ	8		;Size of an 8 by 8 pattern in bytes


oem_brush_def	struc
oem_brush_C0	db	SIZE_PATTERN dup (?)	;C0 plane
oem_brush_C1	db	SIZE_PATTERN dup (?)	;C1 plane
oem_brush_C2	db	SIZE_PATTERN dup (?)	;C2 plane
oem_brush_C3	db	SIZE_PATTERN dup (?)    ;C3 plane
oem_brush_mono	db	SIZE_PATTERN dup (?)	;Mono portion
oem_brush_style dw	0			;Style of the brush
oem_brush_accel db	0			;Accellerator for solids
oem_brush_fg	db	0			;current foreground (text) color
oem_brush_bg	db	0			;current background color
oem_brush_spare db	0			;just a fill byte for now
oem_brush_trans db	SIZE_PATTERN dup(0)	;transparency mask
oem_brush_def	ends				;  (same as "Special" above)

ifdef	incBrushStyle
if	incBrushStyle
BS_MONO_PATTERN     equ BS_PATTERN+1	    ;this are brush styles used by the
BS_COLOR_PATTERN    equ BS_PATTERN+2	    ;driver for PBrushes
MyMaxBrushStyle     equ BS_COLOR_PATTERN
endif
endif

;	The pen structure is OEM dependant, and can contain whatever
;	information that is needed to realize the given pen.  For this
;	implementation, this will be the style of the pen and the
;	physical color of the pen.
;
;	The style is used to catch pens of style NO_LINE and exit if
;	encountered.

oem_pen_def	struc
oem_pen_pcol	db	(size phys_color) dup (0);Physical color
oem_pen_style	dw	0			 ;Pen style
oem_pen_def	ends



;	The Internal Physical Device will look very similar to a bitmap,
;	with the exception that a couple of extra bytes will be tagged
;	onto the end to store the configuration of the screen and the
;	equipment flags
;
;	A flag which will also be used to inicatte that output is
;	enabled to the device.	This is required because it is possible
;	under Win386 to be asked to do output while the OEM layer is
;	disabled.


int_phys_device struc
		db	(size BITMAP) dup (?)
ipd_format	db	0		;Old screen format
ipd_equipment	db	0		;Old equipment switch values
ipd_enabled	db	0		;0 if output is disabled
int_phys_device ends
