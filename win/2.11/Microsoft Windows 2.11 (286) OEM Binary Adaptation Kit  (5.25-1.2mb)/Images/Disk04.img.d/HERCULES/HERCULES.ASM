        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	HERCULES.ASM
;
;   This module contains functions and definitions specific to
;   the Hercules Monochrome Display Driver.
;
; Created: 22-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1984-1987 Microsoft Corporation
;
; Exported Functions:	none
;
; Public Functions:	physical_enable
;			physical_disable
;
; Public Data:
;		PHYS_DEVICE_SIZE		info_table_base
;		BW_THRESHOLD			physical_device
;		COLOR_FORMAT			interlace_adjust
;		SCREEN_W_BYTES			rot_bit_tbl
;		SCREEN_WIDTH			color_table
;		SCREEN_HEIGHT			ScreenSelector
;		COLOR_TBL_SIZE			BlueMoonSeg_colo_table
;		Y_SHIFT_COUNT			PixelSeg_interlace_adjust
;		EVEN_SCAN_INC			LineSeg_interlace_adjust
;		ODD_SCAN_INC			ScanlineSeg_interlace_adjust
;		TOGGLE_EVEN_ODD 		BlueMoonSeg_interlace_adjust
;		INTERLACE_WRAP			BlueMoonSeg_color_table
;		SCAN_INC_PLUS_WRAP
;
;		HYPOTENUSE
;		Y_MAJOR_DIST
;		X_MAJOR_DIST
;		Y_MINOR_DIST
;		X_MINOR_DIST
;		MAX_STYLE_ERR
;
;		HERCULES_DEFINED
;
;		 H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3
;		 H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7
;		 V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3
;		 V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7
;		D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3
;		D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7
;		D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3
;		D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7
;		CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3
;		CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7
;		DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3
;		DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;


incDevice = 1				;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include	cursor.inc
	include gdidefs.inc
	include display.inc
	include macros.mac
	.list


	public	HERCULES_DEFINED

	public	PHYS_DEVICE_SIZE	;Number of bytes in physical device
	public	BW_THRESHOLD		;Black/white threshold
	public	COLOR_FORMAT		;Color format
	public	SCREEN_W_BYTES		;Screen width in bytes
	public	SCREEN_WIDTH		;Screen width in pixels
	public	SCREEN_HEIGHT		;Screen height in scans
	public	COLOR_TBL_SIZE		;Number of entries in color table
	public	ScreenSelector		;Segment of Regen RAM

	public	physical_enable 	;Enable routine
	public	physical_disable	;Disable

	public	physical_device 	;Physical device descriptor
	public	info_table_base 	;GDIInfo table base address
	public	interlace_adjust
	public	interlace_adjust
	public	BlueMoonSeg_interlace_adjust
	public	LineSeg_interlace_adjust
	public	ScanlineSeg_interlace_adjust
	public	PixelSeg_interlace_adjust

	public	Y_SHIFT_COUNT
	public	INTERLACE_WRAP
	public	SCAN_INC_PLUS_WRAP
	public	ODD_SCAN_INC
	public	EVEN_SCAN_INC
	public	TOGGLE_EVEN_ODD

	public	HYPOTENUSE
	public	Y_MAJOR_DIST
	public	X_MAJOR_DIST
	public	Y_MINOR_DIST
	public	X_MINOR_DIST
	public	MAX_STYLE_ERR

	public	BlueMoonSeg_color_table ;Table of physical colors

	public	 H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3
	public	 H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7
	public	 V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3
	public	 V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7
	public	D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3
	public	D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7
	public	D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3
	public	D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7
	public	CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3
	public	CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7
	public	DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3
	public	DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7




;	The following label provides some error checking at link time
;	as to whether a module was compiled for the CGA or for the
;	Hercules card.  This label enables extern statements which
;	the linker will catch in case of incorrect target adapters.

HERCULES_DEFINED = 1



;-----------------------------------------------------------------------;
;	The hatched brush pattern definitions
;-----------------------------------------------------------------------;

H_HATCH_BR_0	equ	00000000b	;Horizontal Hatched brush colors
H_HATCH_BR_1	equ	00000000b
H_HATCH_BR_2	equ	00000000b
H_HATCH_BR_3	equ	00000000b
H_HATCH_BR_4	equ	11111111b
H_HATCH_BR_5	equ	00000000b
H_HATCH_BR_6	equ	00000000b
H_HATCH_BR_7	equ	00000000b

V_HATCH_BR_0	equ	00001000b	;Vertical Hatched brush colors
V_HATCH_BR_1	equ	00001000b
V_HATCH_BR_2	equ	00001000b
V_HATCH_BR_3	equ	00001000b
V_HATCH_BR_4	equ	00001000b
V_HATCH_BR_5	equ	00001000b
V_HATCH_BR_6	equ	00001000b
V_HATCH_BR_7	equ	00001000b

D1_HATCH_BR_0	equ	10000000b	;\ diagonal brush colors
D1_HATCH_BR_1	equ	01000000b
D1_HATCH_BR_2	equ	00100000b
D1_HATCH_BR_3	equ	00010000b
D1_HATCH_BR_4	equ	00001000b
D1_HATCH_BR_5	equ	00000100b
D1_HATCH_BR_6	equ	00000010b
D1_HATCH_BR_7	equ	00000001b

D2_HATCH_BR_0	equ	00000001b	;/ diagonal hatched brush colors
D2_HATCH_BR_1	equ	00000010b
D2_HATCH_BR_2	equ	00000100b
D2_HATCH_BR_3	equ	00001000b
D2_HATCH_BR_4	equ	00010000b
D2_HATCH_BR_5	equ	00100000b
D2_HATCH_BR_6	equ	01000000b
D2_HATCH_BR_7	equ	10000000b

CR_HATCH_BR_0	equ	00001000b	;+ hatched brush colors
CR_HATCH_BR_1	equ	00001000b
CR_HATCH_BR_2	equ	00001000b
CR_HATCH_BR_3	equ	00001000b
CR_HATCH_BR_4	equ	11111111b
CR_HATCH_BR_5	equ	00001000b
CR_HATCH_BR_6	equ	00001000b
CR_HATCH_BR_7	equ	00001000b

DC_HATCH_BR_0	equ	10000001b	;X hatched brush colors
DC_HATCH_BR_1	equ	01000010b
DC_HATCH_BR_2	equ	00100100b
DC_HATCH_BR_3	equ	00011000b
DC_HATCH_BR_4	equ	00011000b
DC_HATCH_BR_5	equ	00100100b
DC_HATCH_BR_6	equ	01000010b
DC_HATCH_BR_7	equ	10000001b


;-----------------------------------------------------------------------;
;	Line style definitions
;
;	Since the style update code in the line DDA checks for a sign,
;	the values chosen for distances, HYPOTENUSE, and MAX_STYLE_ERR
;	must not be bigger than 127+min(X_MAJOR_DIST,Y_MAJOR_DIST).  If
;	this condition is met, then the sign bit will always be cleared
;	on the first subtraction after every add-back.
;-----------------------------------------------------------------------;


HYPOTENUSE	=	19		;Distance moving X and Y
Y_MAJOR_DIST	=	16		;Distance moving Y only
X_MAJOR_DIST	=	11		;Distance moving X only
Y_MINOR_DIST	=	HYPOTENUSE-X_MAJOR_DIST
X_MINOR_DIST	=	HYPOTENUSE-Y_MAJOR_DIST
MAX_STYLE_ERR	=	HYPOTENUSE*2	;Max error before updating
					;  rotating bit mask



;-----------------------------------------------------------------------;
;	The black/white threshold is used to determine the split
;	between black and white when summing an RGB Triplet
;-----------------------------------------------------------------------;


BW_THRESHOLD	equ	(3*0FFh)/2
page

sBegin	Data

globalW ssb_mask,<not RC_SAVEBITMAP>	;Mask for save screen bitmap bit
globalB enabled_flag,0			;Display is enabled if non-zero

sEnd	Data
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg


SCREEN_W_BYTES	equ	SCAN_BYTES*1	;Screen width in bytes
COLOR_FORMAT	equ	0101h


;-----------------------------------------------------------------------;
;	PhysDeviceSize is the number of bytes that the enable routine
;	is to copy into the passed PDevice block before calling the
;	physical_enable routine.  For this driver, the length is the
;	size of the bitmap structure.
;-----------------------------------------------------------------------;

PHYS_DEVICE_SIZE equ	size BITMAP



;-----------------------------------------------------------------------;
;	Allocate the physical device block for the EGA Card.
;	For this driver, physical devices will be in the same format
;	as a normal bitmap descriptor.	This is very convienient since
;	it simplifies the structures that the code must work with.
;
;	The bmWidthPlanes field will be set to zero to simplify some
;	of the three plane code.  By setting it to zero, it can be
;	added to a memory bitmap pointer without changing the pointer.
;	This allows the code to add this in regardless of the type of
;	the device.
;
;	The actual physical block will have some extra bytes stuffed on
;	the end (IntPhysDevice structure), but only the following is static
;-----------------------------------------------------------------------;

ScreenSelector	equ	0B800H


;	The following constants keep the parameter list to BITMAP within
;	view on an editing display 80 chars wide.

SCRSEL		equ	ScreenSelector
P		equ	 COLOR_FORMAT AND 000FFh	;# color planes
B		equ	(COLOR_FORMAT AND 0FF00h) SHR 8	;# bits per pixel
H		equ	SCREEN_HEIGHT			;new display height
W		equ	SCREEN_WIDTH			;display width, pels
WB		equ	SCREEN_W_BYTES			;display width, bytes

physical_device BITMAP <SCRSEL,W,H,WB,P,B,0B8000000H,0,0,0,0,0,0,0>




;	The GDIInfo data Structure.  The specifics of the Hercules
;	card are passed to GDI via this structure.

info_table_base label byte


	dw	201h			;Version = 0201h for now
	errnz	dpVersion

	dw	DT_RASDISPLAY		;Device classification
	errnz	dpTechnology-dpVersion-2

	dw	225			;Horizontal size in millimeters
	errnz	dpHorzSize-dpTechnology-2

	dw	145			;Vertical size in millimeters
	errnz	dpVertSize-dpHorzSize-2

	dw	SCREEN_WIDTH		;Horizontal width in pixels
	errnz	dpHorzRes-dpVertSize-2

	dw	SCREEN_HEIGHT		;Vertical width in pixels
	errnz	dpVertRes-dpHorzRes-2

	dw	1			;Number of bits per pixel
	errnz	dpBitsPixel-dpVertRes-2

	dw	1			;Number of planes
	errnz	dpPlanes-dpBitsPixel-2

	dw	65+6+6			;Number of brushes the device has
	errnz	dpNumBrushes-dpPlanes-2 ;  (Show lots of brushes)

	dw	10			;Number of pens the device has
	errnz	dpNumPens-dpNumBrushes-2;  (2 colors * 5 styles)

	dw	0			;Reserved

	dw	0			;Number of fonts the device has
	errnz	dpNumFonts-dpNumPens-4

	dw	2			;Number of colors in color table
	errnz	dpNumColors-dpNumFonts-2

	dw	size int_phys_device	;Size required for device descriptor
	errnz	dpDEVICEsize-dpNumColors-2

	dw	CC_NONE 		;Curves capabilities
	errnz	dpCurves-dpDEVICEsize-2

	dw	LC_POLYLINE+LC_STYLED	;Line capabilities
	errnz	dpLines-dpCurves-2

	dw	PC_SCANLINE		;Polygonal capabilities
	errnz	dpPolygonals-dpLines-2

	dw	TC_CP_STROKE+TC_RA_ABLE ;Text capabilities
	errnz	dpText-dpPolygonals-2

	dw	CP_NONE 		;Clipping capabilities
	errnz	dpClip-dpText-2

					;BitBlt capabilities
	dw	RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT
	errnz	dpRaster-dpClip-2

	dw	X_MAJOR_DIST		;Distance moving X only
	errnz	dpAspectX-dpRaster-2

	dw	Y_MAJOR_DIST		;Distance moving Y only
	errnz	dpAspectY-dpAspectX-2

	dw	HYPOTENUSE		;Distance moving X and Y
	errnz	dpAspectXY-dpAspectY-2

	dw	MAX_STYLE_ERR		;Length of segment for line styles
	errnz	dpStyleLen-dpAspectXY-2


	errnz	dpMLoWin-dpStyleLen-2	;Metric  Lo res WinX,WinY,VptX,VptY
	dw	2250			;  HorzSize * 10
	dw	1450			;  VertSize * 10
	dw	720			;  HorizRes
	dw	-348			;  -VertRes


	errnz	dpMHiWin-dpMLoWin-8	;Metric  Hi res WinX,WinY,VptX,VptY
	dw	22500			;  HorzSize * 100
	dw	14500			;  VertSize * 100
	dw	720			;  HorizRes
	dw	-348			;  -VertRes


	errnz	dpELoWin-dpMHiWin-8	;English Lo res WinX,WinY,VptX,VptY
	dw	1000			;  HorzSize * 1000 scaled(/225)
	dw	725			;  VertSize * 1000 scaled(/200)
	dw	813			;  HorizRes * 254  scaled(/225)
	dw	-442			;  -VertRes * 254  scaled(/200)


	errnz	dpEHiWin-dpELoWin-8	;English Hi res WinX,WinY,VptX,VptY
	dw	10000			;  HorzSize * 10000 scaled(/225)
	dw	3625			;  VertSize * 10000 scaled(/400)
	dw	813			;  HorizRes * 254   scaled(/225)
	dw	-221			;  -VertRes * 254   scaled(/400)


	errnz	dpTwpWin-dpEHiWin-8	;Twips		WinX,WinY,VptX,VptY
	dw	3240			;  HorzSize * 14400 scaled(/1000)
	dw	5220			;  VertSize * 14400 scaled(/400)
	dw	183			;  HorizRes * 254   scaled(/1000)
	dw	-221			;  -VertRes * 254   scaled(/400)


	dw	96			;Logical Pixels/inch in X
	errnz	dpLogPixelsX-dpTwpWin-8

	dw	72			;Logical Pixels/inch in Y
	errnz	dpLogPixelsY-dpLogPixelsX-2

	dw	DC_IgnoreDFNP		;dpDCManage
	errnz	dpDCManage-dpLogPixelsY-2

	dw	0			;Reserved fields
	dw	0
	dw	0
	dw	0
	dw	0


	errnz	<(offset $)-(offset info_table_base)-(size GDIINFO)>
page

;	Hercules 6845 CRTC parameters for high-res graphics

herc_graph_table	label	byte

	db	35h
	db	2Dh
	db	2Eh
	db	07h
	db	5Bh
	db	02h
	db	57h
	db	57h
	db	02h
	db	03h
	db	00h
	db	00h


;	Hercules 6845 CRTC parameters for Text Mode

herc_text_table	label	byte

	db	61h
	db	50h
	db	52h
	db	0Fh
	db	19h
	db	06h
	db	19h
	db	19h
	db	02h
	db	0Dh
	db	0Bh
	db	0Ch


;	Constants and macro for changing the screen mode of the IBM/PC display

SET_MODE	equ	0
READ_MODE	equ	15
TEXT_MODE	equ	3
GRAPHICS_MODE	equ	6

video_io	macro	func,arg
	mov	ah,func
	ifnb	<arg>
		mov	al,arg
	endif
	int	10h
endm
page

;---------------------------Public-Routine------------------------------;
; physical_enable
;
;   Hercules 720x348 graphics mode is enabled.
;
; Entry:
;	ES:DI --> ipd_format in our pDevice
;	DS: = Data
; Returns:
;	AX = non-zero to show success
; Error Returns:
;	AX = 0
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;	INT 10h
; History:
;	Tue 18-Aug-1987 18:09:00 -by-  **** ***** [*****]
;	Added enabled_flag
;
;	Thu 26-Feb-1987 13:45:58 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

physical_enable proc near

	video_io	READ_MODE	;save old format
	stosb

	mov	dx,3BFh			;enable graphics and allow
	mov	al,00000011b		;  page two
	out	dx,al

	mov	al,82h			;select graphics, page two
	mov	si,InitSegOFFSET herc_graph_table
	mov	bx,5555h		;fill word
	mov	cx,4000h		;fill 348 scans
	call	set_mode		;set graphics mode
	mov	enabled_flag,BL 	;Show enabled

	mov	ax,1
	ret

physical_enable endp
page

;---------------------------Public-Routine------------------------------;
; physical_disable
;
;   Hercules 720x348 graphics mode is exited.  The text mode of the
;   adapter is restored.
;
; Entry:
;	DS:SI --> int_phys_device
;	ES:    =  Data
; Returns:
;	AX = non-zero to show success
; Error Returns:
;	None
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;	INT 10h
; History:
;	Tue 18-Aug-1987 18:09:00 -by-  **** ***** [*****]
;	Added enabled_flag
;
;	Thu 26-Feb-1987 13:45:58 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,Data

physical_disable proc near

	mov	al,[si].ipd_format

	push	ax			;save the old format
	mov	al,20h			;reprogram the 6845
	mov	si,InitSegOFFSET herc_text_table
	mov	bx,720h			;fill word
	mov	cx,2000h		;size of a text screen
	mov	enabled_flag,cl 	;Show disabled
	call	set_mode
	pop	ax

	video_io	SET_MODE	;restore original mode

	mov	ax,1
	ret

physical_disable endp
page

;---------------------------Private-Routine-----------------------------;
; set_mode
;
; Set Hercules graphics card mode
;
; The Hercules graphics card CRTC parameters will be set for
; graphics.
;
;
; Entry:
;	SI --> parameter table
;	CX  =  number of words to be cleared
;	BX  =  blank value
;	AL  =  mode byte
;	DS  =  CS
;	ES:DI --> ipd_format in our pDevice
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	AX,BX,CX,DS
; Registers Destroyed:
;	DX,SI,DI,ES,FLAGS
; Calls:
;	INT 10h
; History:
;	Thu 26-Feb-1987 13:45:58 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	public	set_mode		;Public for debugging

set_mode	proc	near

	push	ax
	push	bx
	push	cx
	mov	dx,3B8h
	out	dx,al			;change mode with screen off
	mov	dx,3B4h
	mov	cx,12			;12 parameters to be output
	xor	ah,ah
parms:
	mov	al,ah
	out	dx,al
	inc	dx
	lods	herc_text_table
	out	dx,al
	inc	ah
	dec	dx
	loop	parms

	pop	cx
	mov	ax,ScreenSelector
	mov	es,ax
	cld
	xor	di,di
	pop	ax
	rep	stosw			;clear the buffer

	mov	dx,3B8h			;control register
	pop	ax
	or	al,8
	out	dx,al			;turn screen on

	ret

set_mode	endp

sEnd	InitSeg
page


COLOR_TBL_SIZE	equ	2		;8 entries in the table

createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin	BlueMoonSeg

BlueMoonSeg_color_table label	dword

;		dd	xxbbggrr
		dd	00000000h	;Black
		dd	00FFFFFFh	;White

sEnd	BlueMoonSeg
page

;-----------------------------------------------------------------------;
;	Allocate and define the Interlace Adjust Table and Y Shift Count.
;
;	The Interlace Adjust table is used for determining the interlace
;	value based on the low order two bits of the Y address.  Since the
;	IBM Color card is only interlaced based on the low order bit, the
;	first two words will be duplicated in the second two words.
;
;	The Y Shift Count is used in the same operation.  It indicates
;	how much to shift the Y coordinate right to adjust for the
;	interlace stuff.
;-----------------------------------------------------------------------;

Y_SHIFT_COUNT	equ	2		;divide by four
INTERLACE_WRAP	equ	8000h+SCREEN_W_BYTES


;	The odd row increment is the value used when incrementing from
;	an odd scan line to an even scan line, with an increasing Y.
;
;	The even row increment is the value used when incrementing from
;	an even scan line to an odd scan line, with an increasing Y.
;
;	The even-odd toggle value is the XOR mask used to toggle the
;	odd row increment to the next even row for an increasing Y.

EVEN_SCAN_INC		equ	02000h
ODD_SCAN_INC		equ	02000h
TOGGLE_EVEN_ODD		equ	00000h


;	The next constant is needed by fixed-pitch ExtTextOut.

SCAN_INC_PLUS_WRAP	equ	EVEN_SCAN_INC + INTERLACE_WRAP


	.lall

int_adj macro	segname,logname,isdefined
ifdif <isdefined>,<defined>
createSeg segname,logname,word,public,CODE
endif
sBegin	logname
ifidn <logname>,<Code>
interlace_adjust label	word
else
logname&_interlace_adjust label  word
endif
	dw	0			;Even row
	dw	2000h			;Odd row
	dw	4000h			;Even row
	dw	6000h			;Odd row
sEnd	logname
	endm

	int_adj _TEXT,Code,<defined>
	int_adj _BLUEMOON,BlueMoonSeg,<defined>
	int_adj _LINES,LineSeg,<not defined>
	int_adj _SCANLINE,ScanlineSeg,<not defined>
	int_adj _PIXEL,PixelSeg,<not defined>

end
