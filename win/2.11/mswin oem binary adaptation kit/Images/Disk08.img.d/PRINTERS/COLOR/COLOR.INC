;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************



;	Physical representations of various GDI objects in the
;	color drivers.


;	The color structure is how this driver stores its physical
;	colors in memory.  The color consists of four bytes (dword),
;	one byte for each of the three planes of color, and a fourth
;	byte which is a combination of one bit from each plane, plus
;	one bit used for monochrome bitmaps (black/white).

PhysColor	struc
  Red		db	?		;Red
  Green 	db	?		;Green
  Blue		db	?		;Blue
  Mono		db	?		;Monochrome and special info
PhysColor	ends


RedBit		equ	00000001b	;Red   bit for mono byte
GreenBit	equ	00000010b	;Green bit for mono byte
BlueBit 	equ	00000100b	;Blue  bit for mono byte
MonoBit 	equ	01000000b	;Monochrome bit


	errnz	<(SIZE PhysColor) - 4>	;Must be a double word




;	The brush structure is OEM dependant, and can contain whatever
;	information that is needed to realize the given brush.
;
;	For this implementation, the brush will consist of an 8x8
;	pattern for each of the three planes, and a fourth 8x8
;	pattern intended for monochrome to color conversion (for
;	something like a hatched brush, this would contain the
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


SizePattern	equ	8		;Size of an 8 by 8 pattern in bytes
BrushIsSolid	equ	10000000b	;Indicates a solid color brush
GreyScale	equ	01000000b	;Indicates a real grey scale brush


OEMBrushDef	struc
  RedPlane	db	SizePattern dup (?)	;Red   plane
  GreenPlane	db	SizePattern dup (?)	;Green plane
  BluePlane	db	SizePattern dup (?)	;Blue  plane
  MonoPlane	db	SizePattern dup (?)	;Mono portion
  OEMBrushStyle dw	0			;Style of the brush
  OEMBrushAccel db	0			;Accellerator for solids

OEMBrushDef	ends



;	The pen structure is OEM dependant, and can contain whatever
;	information that is needed to realize the given pen.  For this
;	implementation, this will the style of the pen and the physical
;	color of the pen.
;
;	The style is used to catch pens of style NO_LINE and exit if
;	encountered.

OEMPenDef	struc
  OEMPen	db	(SIZE PhysColor) dup (0);Physical color
  OEMPenStyle	dw	0			;Pen style
OEMPenDef	ends
