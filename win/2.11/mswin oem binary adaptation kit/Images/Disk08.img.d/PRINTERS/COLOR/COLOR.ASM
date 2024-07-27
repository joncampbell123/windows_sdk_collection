page	,132
;***************************************************************************
;									   *
;		Copyright (C) 1984-1986 by Microsoft Inc.		   *
;									   *
;***************************************************************************

title	Color Mode Specific Code and Routines
%out	Color


;	History:
;
;	090185	New hoirzontal and vertical sizes.  Required recomputing
;		mapmode parameters.



;	This file contains the constants, code templates, and tables
;	specific to the color mode.
;



;	Define the portions of gdidefs.inc that will be needed

incDevice = 1

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include color.inc
	.list




sBegin	code
assumes cs,code

	public	BWThreshold		;Black/white threshold
	public	RotBitTbl		;Rotating bitmask table

	public	BrushColors		;Start of brush RGB colors

	public	HHatchBr_0		;Horizontal Hatched brush colors
	public	HHatchBr_1
	public	HHatchBr_2
	public	HHatchBr_3
	public	HHatchBr_4
	public	HHatchBr_5
	public	HHatchBr_6
	public	HHatchBr_7

	public	VHatchBr_0		;Vertical Hatched brush colors
	public	VHatchBr_1
	public	VHatchBr_2
	public	VHatchBr_3
	public	VHatchBr_4
	public	VHatchBr_5
	public	VHatchBr_6
	public	VHatchBr_7

	public	D1HatchBr_0		;\ diagonal brush colors
	public	D1HatchBr_1
	public	D1HatchBr_2
	public	D1HatchBr_3
	public	D1HatchBr_4
	public	D1HatchBr_5
	public	D1HatchBr_6
	public	D1HatchBr_7

	public	D2HatchBr_0		;/ diagonal hatched brush colors
	public	D2HatchBr_1
	public	D2HatchBr_2
	public	D2HatchBr_3
	public	D2HatchBr_4
	public	D2HatchBr_5
	public	D2HatchBr_6
	public	D2HatchBr_7

	public	CrHatchBr_0		;+ hatched brush colors
	public	CrHatchBr_1
	public	CrHatchBr_2
	public	CrHatchBr_3
	public	CrHatchBr_4
	public	CrHatchBr_5
	public	CrHatchBr_6
	public	CrHatchBr_7

	public	DCHatchBr_0		;X hatched brush colors
	public	DCHatchBr_1
	public	DCHatchBr_2
	public	DCHatchBr_3
	public	DCHatchBr_4
	public	DCHatchBr_5
	public	DCHatchBr_6
	public	DCHatchBr_7

	public	ColorTable



;	The Logical colors that will be returned for enumerate object
;	for the brushes that this driver handles.


BrushColors label word

	dw	00000H			;Black brush color
	dw	04040H			;DkGrey brush color
	dw	08080H			;Grey brush color
	dw	0C0C0H			;LtGrey brush color
	dw	0FFFFH			;White brush color


;	The hatched brush patterns are defined for a foreground
;	color of 0 and a background color of 1


HHatchBr_0	equ	0FFh AND NOT 00000000b	;Horizontal Hatched brush colors
HHatchBr_1	equ	0FFh AND NOT 00000000b
HHatchBr_2	equ	0FFh AND NOT 00000000b
HHatchBr_3	equ	0FFh AND NOT 00000000b
HHatchBr_4	equ	0FFh AND NOT 11111111b
HHatchBr_5	equ	0FFh AND NOT 00000000b
HHatchBr_6	equ	0FFh AND NOT 00000000b
HHatchBr_7	equ	0FFh AND NOT 00000000b

VHatchBr_0	equ	0FFh AND NOT 00001000b	;Vertical Hatched brush colors
VHatchBr_1	equ	0FFh AND NOT 00001000b
VHatchBr_2	equ	0FFh AND NOT 00001000b
VHatchBr_3	equ	0FFh AND NOT 00001000b
VHatchBr_4	equ	0FFh AND NOT 00001000b
VHatchBr_5	equ	0FFh AND NOT 00001000b
VHatchBr_6	equ	0FFh AND NOT 00001000b
VHatchBr_7	equ	0FFh AND NOT 00001000b

D1HatchBr_0	equ	0FFh AND NOT 10000000b	;\ diagonal brush colors
D1HatchBr_1	equ	0FFh AND NOT 01000000b
D1HatchBr_2	equ	0FFh AND NOT 00100000b
D1HatchBr_3	equ	0FFh AND NOT 00010000b
D1HatchBr_4	equ	0FFh AND NOT 00001000b
D1HatchBr_5	equ	0FFh AND NOT 00000100b
D1HatchBr_6	equ	0FFh AND NOT 00000010b
D1HatchBr_7	equ	0FFh AND NOT 00000001b

D2HatchBr_0	equ	0FFh AND NOT 00000001b	;/ diagonal hatched brush colors
D2HatchBr_1	equ	0FFh AND NOT 00000010b
D2HatchBr_2	equ	0FFh AND NOT 00000100b
D2HatchBr_3	equ	0FFh AND NOT 00001000b
D2HatchBr_4	equ	0FFh AND NOT 00010000b
D2HatchBr_5	equ	0FFh AND NOT 00100000b
D2HatchBr_6	equ	0FFh AND NOT 01000000b
D2HatchBr_7	equ	0FFh AND NOT 10000000b

CrHatchBr_0	equ	0FFh AND NOT 00001000b	;+ hatched brush colors
CrHatchBr_1	equ	0FFh AND NOT 00001000b
CrHatchBr_2	equ	0FFh AND NOT 00001000b
CrHatchBr_3	equ	0FFh AND NOT 00001000b
CrHatchBr_4	equ	0FFh AND NOT 11111111b
CrHatchBr_5	equ	0FFh AND NOT 00001000b
CrHatchBr_6	equ	0FFh AND NOT 00001000b
CrHatchBr_7	equ	0FFh AND NOT 00001000b

DCHatchBr_0	equ	0FFh AND NOT 10000001b	;X hatched brush colors
DCHatchBr_1	equ	0FFh AND NOT 01000010b
DCHatchBr_2	equ	0FFh AND NOT 00100100b
DCHatchBr_3	equ	0FFh AND NOT 00011000b
DCHatchBr_4	equ	0FFh AND NOT 00011000b
DCHatchBr_5	equ	0FFh AND NOT 00100100b
DCHatchBr_6	equ	0FFh AND NOT 01000010b
DCHatchBr_7	equ	0FFh AND NOT 10000001b




;	The black/white threshold is used to determine the split
;	between black and white when summing an RGB Triplet


BWThreshold	equ	(3*0FFH)/2




;	The rotating bit table is used to fetch the initial mask to use
;	for the line code.  The mask is based on D2..D0 of the X coordinate.


RotBitTbl	label	byte
		db	10000000B
		db	01000000B
		db	00100000B
		db	00010000B
		db	00001000B
		db	00000100B
		db	00000010B
		db	00000001B







;	Color Table contains the color table
;	definitions.  The color table is used for the GetColorTable
;	Escape function and for pen and brush enumeration.  


ColorTable	label	dword

;		dd	xxbbggrr
		dd	00000000h	;Black
		dd	000000FFh	;Red
		dd	0000FF00h	;Green
		dd	0000FFFFh	;Yellow
		dd	00FF0000h	;Blue
		dd	00FF00FFh	;Magenta
		dd	00FFFF00h	;Cyan
		dd	00FFFFFFh	;White

sEnd	code
end
