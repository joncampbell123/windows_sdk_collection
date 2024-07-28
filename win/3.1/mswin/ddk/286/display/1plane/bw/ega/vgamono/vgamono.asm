
        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	vgamono.ASM
;
;   This module contains functions and definitions specific to
;   the vgamono Display Driver.
;
; Created: 22-Feb-1987
; Author:  Walt Moore [waltm]
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
;		COLOR_FORMAT
;		SCREEN_W_BYTES
;		SCREEN_WIDTH
;		SCREEN_HEIGHT			ScreenSelector
;		COLOR_TBL_SIZE
;
;		HYPOTENUSE
;		Y_MAJOR_DIST
;		X_MAJOR_DIST
;		Y_MINOR_DIST
;		X_MINOR_DIST
;		MAX_STYLE_ERR
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
STOP_IO_TRAP	equ 4000h		; stop io trapping
START_IO_TRAP	equ 4007h		; re-start io trapping


incDevice = 1				;Include control for gdidefs.inc

	.xlist
	include cmacros.inc
	include gdidefs.inc
	include	egamem.inc
	include display.inc
	include macros.mac
	include	cursor.inc
	.list


;------------------------------ Macro ----------------------------------;
;	cdw - conditional dw
;
;	cdw is used to conditionally define a word to one of the
;	given values.
;
;	usage:
;		cdw	v1,v2
;
;	v1 will be used if LARGE_SCREEN_SIZE if defined.
;	v1 will be used otherwise.
;-----------------------------------------------------------------------;

cdw	macro	v1,v2
ifdef	LARGE_SCREEN_SIZE
	dw	v1
else
	dw	v2
endif
	endm

	externA	ScreenSelector
	externA	WinFlags

	public	PHYS_DEVICE_SIZE	;Number of bytes in physical device
	public	BW_THRESHOLD		;Black/white threshold
	public	COLOR_FORMAT		;Color format
	public	SCREEN_W_BYTES		;Screen width in bytes
	public	SCREEN_WIDTH		;Screen width in pixels
	public	SCREEN_HEIGHT		;Screen height in scans
	public	COLOR_TBL_SIZE		;Number of entries in color table
	public	physical_enable 	;Enable routine
	public	physical_disable	;Disable

	public	physical_device 	;Physical device descriptor
	public	info_table_base 	;GDIInfo table base address

	public	BlueMoonSeg_color_table


	public	HYPOTENUSE
	public	Y_MAJOR_DIST
	public	X_MAJOR_DIST
	public	Y_MINOR_DIST
	public	X_MINOR_DIST
	public	MAX_STYLE_ERR

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

		externFP	AllocSelector


;-----------------------------------------------------------------------;
;	The hatched brush pattern definitions
;-----------------------------------------------------------------------;

H_HATCH_BR_0	equ	00000000b	;Horizontal Hatched brush
H_HATCH_BR_1	equ	00000000b
H_HATCH_BR_2	equ	00000000b
H_HATCH_BR_3	equ	00000000b
H_HATCH_BR_4	equ	11111111b
H_HATCH_BR_5	equ	00000000b
H_HATCH_BR_6	equ	00000000b
H_HATCH_BR_7	equ	00000000b

V_HATCH_BR_0	equ	00001000b	;Vertical Hatched brush
V_HATCH_BR_1	equ	00001000b
V_HATCH_BR_2	equ	00001000b
V_HATCH_BR_3	equ	00001000b
V_HATCH_BR_4	equ	00001000b
V_HATCH_BR_5	equ	00001000b
V_HATCH_BR_6	equ	00001000b
V_HATCH_BR_7	equ	00001000b

D1_HATCH_BR_0	equ	10000000b	;\ diagonal brush
D1_HATCH_BR_1	equ	01000000b
D1_HATCH_BR_2	equ	00100000b
D1_HATCH_BR_3	equ	00010000b
D1_HATCH_BR_4	equ	00001000b
D1_HATCH_BR_5	equ	00000100b
D1_HATCH_BR_6	equ	00000010b
D1_HATCH_BR_7	equ	00000001b

D2_HATCH_BR_0	equ	00000001b	;/ diagonal hatched brush
D2_HATCH_BR_1	equ	00000010b
D2_HATCH_BR_2	equ	00000100b
D2_HATCH_BR_3	equ	00001000b
D2_HATCH_BR_4	equ	00010000b
D2_HATCH_BR_5	equ	00100000b
D2_HATCH_BR_6	equ	01000000b
D2_HATCH_BR_7	equ	10000000b

CR_HATCH_BR_0	equ	00001000b	;+ hatched brush
CR_HATCH_BR_1	equ	00001000b
CR_HATCH_BR_2	equ	00001000b
CR_HATCH_BR_3	equ	00001000b
CR_HATCH_BR_4	equ	11111111b
CR_HATCH_BR_5	equ	00001000b
CR_HATCH_BR_6	equ	00001000b
CR_HATCH_BR_7	equ	00001000b

DC_HATCH_BR_0	equ	10000001b	;X hatched brush
DC_HATCH_BR_1	equ	01000010b
DC_HATCH_BR_2	equ	00100100b
DC_HATCH_BR_3	equ	00011000b
DC_HATCH_BR_4	equ	00011000b
DC_HATCH_BR_5	equ	00100100b
DC_HATCH_BR_6	equ	01000010b
DC_HATCH_BR_7	equ	10000001b



;-----------------------------------------------------------------------;
;	Line style definitions for the EGA Card
;
;	Since the style update code in the line DDA checks for a sign,
;	the values chosen for distances, HYPOTENUSE, and MAX_STYLE_ERR
;	must not be bigger than 127+min(X_MAJOR_DIST,Y_MAJOR_DIST).  If
;	this condition is met, then the sign bit will always be cleared
;	on the first subtraction after every add-back.
;-----------------------------------------------------------------------;


HYPOTENUSE	=	51		;Distance moving X and Y
Y_MAJOR_DIST	=	36		;Distance moving Y only
X_MAJOR_DIST	=	36		;Distance moving X only
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

globalW ScratchSel,0			;have a scratch selector
globalW ssb_mask,0FFFFh 		;Mask for save screen bitmap bit
globalB enabled_flag,0			;Display is enabled if non-zero
globalW	is_protected,WinFlags		;LSB set in protected mode
globalB bEquipmentFlag, 0		;BYTE at 40:10 in ROM BIOS save area

sEnd	Data
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin	InitSeg
assumes cs,InitSeg


SCREEN_W_BYTES	equ	SCAN_BYTES*1	;"*1" to get in public symbol table
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



;	The following constants keep the parameter list to BITMAP within
;	view on an editing display 80 chars wide.

SCRSEL		equ	ScreenSelector
P		equ	 COLOR_FORMAT AND 000FFh	;# color planes
B		equ	(COLOR_FORMAT AND 0FF00h) SHR 8	;# bits per pixel
H		equ	SCREEN_HEIGHT			;new display height
W		equ	SCREEN_WIDTH			;display width, pels
WB		equ	SCREEN_W_BYTES			;display width, bytes

physical_device BITMAP <SCRSEL,W,H,WB,P,B,0A0000000H,0,0,0,0,0,0,0>



;-----------------------------------------------------------------------;
;	The GDIInfo data Structure.  The specifics of the EGA
;	mode are passed to GDI via this structure.
;-----------------------------------------------------------------------;


info_table_base label byte

	dw	30Ah			;Version = 3.10
	errnz	dpVersion

	dw	DT_RASDISPLAY		;Device classification
	errnz	dpTechnology-dpVersion-2

	cdw	240,208 		;Horizontal size in millimeters
	errnz	dpHorzSize-dpTechnology-2

	cdw	180,156 		;Vertical size in millimeters
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

	dw	CP_RECTANGLE 		;Clipping capabilities
	errnz	dpClip-dpText-2

					;BitBlt capabilities
	dw	RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT+RC_DI_BITMAP+RC_DIBTODEV
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
	cdw	15,2080 		;  HorzSize * 10
	cdw	15,1560 		;  VertSize * 10
	cdw	4,640			;  HorizRes
	cdw	-4,-480 		;  -VertRes


	errnz	dpMHiWin-dpMLoWin-8	;Metric  Hi res WinX,WinY,VptX,VptY
	cdw	150,20800		;  HorzSize * 100
	cdw	150,15600		;  VertSize * 100
	cdw	4,640			;  HorizRes
	cdw	-4,-480 		;  -VertRes


	errnz	dpELoWin-dpMHiWin-8	;English Lo res WinX,WinY,VptX,VptY
	cdw	375,325 		;  HorzSize * 1000
	cdw	375,325 		;  VertSize * 1000
	cdw	254,254 		;  HorizRes * 254
	cdw	-254,-254		;  -VertRes * 254


	errnz	dpEHiWin-dpELoWin-8	;English Hi res WinX,WinY,VptX,VptY
	cdw	3750,1625		;  HorzSize * 10000
	cdw	3750,1625		;  VertSize * 10000
	cdw	254,127 		;  HorizRes * 254
	cdw	-254,-127		;  -VertRes * 254


	errnz	dpTwpWin-dpEHiWin-8	;Twips		WinX,WinY,VptX,VptY
	cdw	5400,2340		;  HorzSize * 14400
	cdw	5400,2340		;  VertSize * 14400
	cdw	254,127 		;  HorizRes * 254
	cdw	-254,-127		;  -VertRes * 254


	dw	96			;Logical Pixels/inch in X
	errnz	dpLogPixelsX-dpTwpWin-8

	dw	96			;Logical Pixels/inch in Y
	errnz	dpLogPixelsY-dpLogPixelsX-2

	dw	DC_IgnoreDFNP		;dpDCManage
	errnz	dpDCManage-dpLogPixelsY-2

	dw	0			;Reserved fields
	dw	0
	dw	0
	dw	0
	dw	0

; start of entries in version 3.0 of this structure


	dw	0 			; number of palette entries
	errnz	dpNumPalReg-dpDCManage-12

        dw      0                       ; number of reserved
        errnz   dpPalReserved-dpNumPalReg-2

	dw	0			; DAC resolution for RGB
	errnz	dpColorRes-dpPalReserved-2


	errnz	<(offset $)-(offset info_table_base)-(size GDIINFO)>

page

;---------------------------Public-Routine------------------------------;
; physical_enable
;
;   640x480 b/w graphics mode is enabled.
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
;	Tue 18-Aug-1987 18:09:00 -by-  Walt Moore [waltm]
;	Added enabled_flag
;
;	Thu 26-Feb-1987 13:45:58 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;


	assumes ds,Data
	assumes es,nothing


physical_enable proc near

;----------------------------------------------------------------------------;
; allocate the scratch selector here				             ;
;----------------------------------------------------------------------------;
	push	es
	xor	ax,ax
	cCall	AllocSelector,<ax>	; get a free selector
	pop	es
	mov	ScratchSel,ax		; save it

;-----------------------------------------------------------------------------;

	mov	bEquipmentFlag,0	;initialize.
	mov	ax,0F00h		;Save old mode
	int	10h
	stosb

	mov	ax,0011h		;Set hi res b/w graphics
	int	10h

	mov	ax,0F00h		;See if it was set
	int	10h
	cmp	al,11h
	je	mode_set_successful	;we have set it into mode 11.

; We could not get into mode 11h directly. We will now try to do a mode change 
; call after reseting the byte at 40:10h to imply color display.

	push	es
	mov	ax,40h			;BIOS data segment.
	mov	es,ax
	mov	al,es:[10h]		;get the equipment byte
	mov	bEquipmentFlag, al	;that the subsequent int 10h suceeds
	and	al, 0efh
	mov	es:[10h], al		;setup for color
	pop	es			;restore.

; try to get into mode 11H again.

	mov	ax,0011h		;Set hi res b/w graphics
	int	10h

	mov	ax,0F00h		;See if it was set
	int	10h
	cmp	al,11h
	je	mode_set_successful	;we have set it into mode 11.

; we have failed yet again. Reset the equipment byte and fail this call.

	push	es			;save
	mov	ax,40h			;BIOS data segment.
	mov	es,ax			
	mov	al,bEquipmentFlag	;get the flag
	mov	es:[10h],al		;reset
	pop	es			;restore.
	xor	ax,ax			;fail the call.
	jmp	short phys_enable_20	;get out.

mode_set_successful:

	mov	enabled_flag,0FFh	;Show enabled

	mov	ax,cs			;--> palette table
	mov	es,ax
	assumes es,nothing
	mov	dx,InitSegOFFSET palette
	mov	ax,1002h
	int	10h			;Set the palette up

;----------------------------------------------------------------------------;
; at this point notify kernel that driver is cable of doing a save/restore   ;
; of its state registers and kernel should stop I/O trapping.		     ;
; Do this only if we are in protected mode.				     ;
;----------------------------------------------------------------------------;
	
	test	is_protected,1  	;will be set if in protected mode
	jz	phys_enable_ok		;not in protectde mode
	mov	ax,STOP_IO_TRAP		
	int	2fh		

;----------------------------------------------------------------------------;

phys_enable_ok:
	mov	ax,1

phys_enable_20:

	ret

physical_enable endp
page

;---------------------------Public-Routine------------------------------;
; physical_disable
;
;   640x480 graphics mode is exited.  The previous mode of the
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
;	init_hw_regs
; History:
;	Tue 18-Aug-1987 18:09:00 -by-  Walt Moore [waltm]
;	Added enabled_flag
;
;	Thu 26-Feb-1987 13:45:58 -by-  Walt Moore [waltm]
;	Created.
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,Data

physical_disable proc near

; if the equipment byte has been saved, reset it first.

	cmp	bEquipmentFlag,0	;has the equipment byte been saved ?
	jz	reset_mode		;no.
	push	ds
	mov	ax,40h
	mov	ds,ax			;BIOS data segment
	mov	al,bEquipmentFlag	;get the saved value
	mov	ds:[10h],al		;reset it
	pop	ds			;restore.

reset_mode:

	mov	al,[si].ipd_format
	xor	ah,ah
	mov	enabled_flag,ah 	;Show disabled
	int	10h

;----------------------------------------------------------------------------;
; at this point as the kernel to do the io trapping again, provided we are in;
; protected mode.							     ;
;----------------------------------------------------------------------------;
	
	test	is_protected,1		;will be set if we are in prot mode
	jz	phys_disable_ret	;we are in real mode
	mov	ax,START_IO_TRAP
	int	2fh			;start i/o trapping
phys_disable_ret:
;----------------------------------------------------------------------------;

	mov	al,1
	ret

physical_disable endp
;-----------------------------------------------------------------------;
;	Palette contains the palette values for the EGA card
;	to give the desired colors.  The planes have been
;	changed from the normal EGA plane definition to be
;	C0 = red, C1 = green, C2 = blue.  The intensity bit
;	will be set for all the colors.	For bw display set all odd
;       registers to white and even to black
;-----------------------------------------------------------------------;

palette label	byte

	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	00h			;Black
	db	3Fh			;White
	db	0			;Overscan will be black


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
end


