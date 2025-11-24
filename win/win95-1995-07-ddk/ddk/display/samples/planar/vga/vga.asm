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

        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:  VGA.ASM
;
;   This module contains functions and definitions specific to
;   the VGA Display Driver.
;
; Exported Functions:   none
;
; Public Functions:     physical_enable
;                       physical_disable
;
; Public Data:
;               PHYS_DEVICE_SIZE                info_table_base
;               BW_THRESHOLD                    physical_device
;               COLOR_FORMAT                    rot_bit_tbl
;               SCREEN_W_BYTES                  color_table
;               SCREEN_WIDTH                    Code_palette
;               SCREEN_HEIGHT
;               SSB_START_OFFSET
;               SSB_END_OFFSET
;               SSB_SIZE_IN_BYTES
;               COLOR_TBL_SIZE
;               COLOR_DONT_CARE
;               ScreenSelector
;
;               HYPOTENUSE
;               Y_MAJOR_DIST
;               X_MAJOR_DIST
;               Y_MINOR_DIST
;               X_MINOR_DIST
;               MAX_STYLE_ERR
;
;                H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3
;                H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7
;                V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3
;                V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7
;               D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3
;               D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7
;               D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3
;               D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7
;               CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3
;               CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7
;               DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3
;               DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7
;
; General Description:
;
; Restrictions:
;
;-----------------------------------------------------------------------;
STOP_IO_TRAP    equ 4000h               ; stop io trapping
START_IO_TRAP   equ 4007h               ; re-start io trapping

incDevice = 1                           ;Include control for gdidefs.inc
incDrawMode = 1                         ;Include DRAWMODE structure
WM3_DETECT=1
        .xlist
        include cmacros.inc
        include gdidefs.inc
        include mflags.inc
        include ega.inc
        include egamem.inc
        include display.inc
        include macros.inc
        include cursor.inc
	include minivdd.inc
	.list

        externA ScreenSelector          ; an import from the kernel
        externA __WinFlags              ; an import from kernel
        externFP AllocSelector          ; create a new selector
        externFP FreeSelector           ; free the selector
        externFP AllocCSToDSAlias       ; CS -> DS (new)
        externFP GetPrivateProfileInt   ; Kernel!GetProfileInt
        externFP WritePrivateProfileString ; Kernel!WritePrivateProfileString

;------------------------------ Macro ----------------------------------;
;       cdw - conditional dw
;
;       cdw is used to conditionally define a word to one of the
;       given values.
;
;       usage:
;               cdw     v1,v2
;
;       v1 will be used if LARGE_SCREEN_SIZE if defined.
;       v1 will be used otherwise.
;-----------------------------------------------------------------------;

cdw     macro   v1,v2
ifdef   LARGE_SCREEN_SIZE
        dw      v1
else
        dw      v2
endif
        endm

        externFP init_hw_regs           ;Initialize ega state code
        externFP SetMode
        externFP SetColors

        public  PHYS_DEVICE_SIZE        ;Number of bytes in physical device
        public  BW_THRESHOLD            ;Black/white threshold
        public  COLOR_FORMAT            ;Color format
        public  SCREEN_W_BYTES          ;Screen width in bytes
        public  SCREEN_WIDTH            ;Screen width in pixels
        public  SCREEN_HEIGHT           ;Screen height in scans
        public  COLOR_TBL_SIZE          ;Number of entries in color table
        public  COLOR_DONT_CARE         ;Value for color don't care register
        public  ScratchSel
ifdef PALETTES
        public  NUM_PALETTES            ;Number of palette registers
        public  PaletteSupported        ;whether palette manager supported
endif
        public  physical_enable         ;Enable routine
        public  physical_disable        ;Disable

        public  physical_device         ;Physical device descriptor
        public  info_table_base         ;GDIInfo table base address

        public  SSB_START_OFFSET
        public  SSB_SIZE_IN_BYTES
        public  SSB_END_OFFSET

        public  HYPOTENUSE
        public  Y_MAJOR_DIST
        public  X_MAJOR_DIST
        public  Y_MINOR_DIST
        public  X_MINOR_DIST
        public  MAX_STYLE_ERR

        public  PixelSeg_color_table    ;Table of physical colors
        public  BlueMoonSeg_color_table ;Table of physical colors
        public  DIMapSeg_color_table    ;Table of physical colors (reversed)

        public   H_HATCH_BR_0, H_HATCH_BR_1, H_HATCH_BR_2, H_HATCH_BR_3
        public   H_HATCH_BR_4, H_HATCH_BR_5, H_HATCH_BR_6, H_HATCH_BR_7
        public   V_HATCH_BR_0, V_HATCH_BR_1, V_HATCH_BR_2, V_HATCH_BR_3
        public   V_HATCH_BR_4, V_HATCH_BR_5, V_HATCH_BR_6, V_HATCH_BR_7
        public  D1_HATCH_BR_0,D1_HATCH_BR_1,D1_HATCH_BR_2,D1_HATCH_BR_3
        public  D1_HATCH_BR_4,D1_HATCH_BR_5,D1_HATCH_BR_6,D1_HATCH_BR_7
        public  D2_HATCH_BR_0,D2_HATCH_BR_1,D2_HATCH_BR_2,D2_HATCH_BR_3
        public  D2_HATCH_BR_4,D2_HATCH_BR_5,D2_HATCH_BR_6,D2_HATCH_BR_7
        public  CR_HATCH_BR_0,CR_HATCH_BR_1,CR_HATCH_BR_2,CR_HATCH_BR_3
        public  CR_HATCH_BR_4,CR_HATCH_BR_5,CR_HATCH_BR_6,CR_HATCH_BR_7
        public  DC_HATCH_BR_0,DC_HATCH_BR_1,DC_HATCH_BR_2,DC_HATCH_BR_3
        public  DC_HATCH_BR_4,DC_HATCH_BR_5,DC_HATCH_BR_6,DC_HATCH_BR_7


;-----------------------------------------------------------------------;
;       The hatched brush pattern definitions
;-----------------------------------------------------------------------;

H_HATCH_BR_0    equ     00000000b       ;Horizontal Hatched brush
H_HATCH_BR_1    equ     00000000b
H_HATCH_BR_2    equ     00000000b
H_HATCH_BR_3    equ     00000000b
H_HATCH_BR_4    equ     11111111b
H_HATCH_BR_5    equ     00000000b
H_HATCH_BR_6    equ     00000000b
H_HATCH_BR_7    equ     00000000b

V_HATCH_BR_0    equ     00001000b       ;Vertical Hatched brush
V_HATCH_BR_1    equ     00001000b
V_HATCH_BR_2    equ     00001000b
V_HATCH_BR_3    equ     00001000b
V_HATCH_BR_4    equ     00001000b
V_HATCH_BR_5    equ     00001000b
V_HATCH_BR_6    equ     00001000b
V_HATCH_BR_7    equ     00001000b

D1_HATCH_BR_0   equ     10000000b       ;\ diagonal brush
D1_HATCH_BR_1   equ     01000000b
D1_HATCH_BR_2   equ     00100000b
D1_HATCH_BR_3   equ     00010000b
D1_HATCH_BR_4   equ     00001000b
D1_HATCH_BR_5   equ     00000100b
D1_HATCH_BR_6   equ     00000010b
D1_HATCH_BR_7   equ     00000001b

D2_HATCH_BR_0   equ     00000001b       ;/ diagonal hatched brush
D2_HATCH_BR_1   equ     00000010b
D2_HATCH_BR_2   equ     00000100b
D2_HATCH_BR_3   equ     00001000b
D2_HATCH_BR_4   equ     00010000b
D2_HATCH_BR_5   equ     00100000b
D2_HATCH_BR_6   equ     01000000b
D2_HATCH_BR_7   equ     10000000b

CR_HATCH_BR_0   equ     00001000b       ;+ hatched brush
CR_HATCH_BR_1   equ     00001000b
CR_HATCH_BR_2   equ     00001000b
CR_HATCH_BR_3   equ     00001000b
CR_HATCH_BR_4   equ     11111111b
CR_HATCH_BR_5   equ     00001000b
CR_HATCH_BR_6   equ     00001000b
CR_HATCH_BR_7   equ     00001000b

DC_HATCH_BR_0   equ     10000001b       ;X hatched brush
DC_HATCH_BR_1   equ     01000010b
DC_HATCH_BR_2   equ     00100100b
DC_HATCH_BR_3   equ     00011000b
DC_HATCH_BR_4   equ     00011000b
DC_HATCH_BR_5   equ     00100100b
DC_HATCH_BR_6   equ     01000010b
DC_HATCH_BR_7   equ     10000001b



;-----------------------------------------------------------------------;
;       Line style definitions for the EGA Card
;
;       Since the style update code in the line DDA checks for a sign,
;       the values chosen for distances, HYPOTENUSE, and MAX_STYLE_ERR
;       must not be bigger than 127+min(X_MAJOR_DIST,Y_MAJOR_DIST).  If
;       this condition is met, then the sign bit will always be cleared
;       on the first subtraction after every add-back.
;-----------------------------------------------------------------------;


HYPOTENUSE      =       51              ;Distance moving X and Y
Y_MAJOR_DIST    =       36              ;Distance moving Y only
X_MAJOR_DIST    =       36              ;Distance moving X only
Y_MINOR_DIST    =       HYPOTENUSE-X_MAJOR_DIST
X_MINOR_DIST    =       HYPOTENUSE-Y_MAJOR_DIST
MAX_STYLE_ERR   =       HYPOTENUSE*2    ;Max error before updating
                                        ;  rotating bit mask



;-----------------------------------------------------------------------;
;       The black/white threshold is used to determine the split
;       between black and white when summing an RGB Triplet
;-----------------------------------------------------------------------;


BW_THRESHOLD    equ     (3*0FFh)/2
page

sBegin  Data
globalD VDDEntryPoint,0

globalB do_int_30,0                     ;int 30 call necessary or not
globalB Line43,0                        ;43 line mode
globalD LineAddr,0
globalW ScratchSel,0                    ;have a scratch selector
globalW is_protected,__WinFlags         ;LSB=1 iff protected mode
WF_ENHANCED     =    0020h
globalW ssb_mask,0FFFFh                 ;Mask for save screen bitmap bit
globalB enabled_flag,0                  ;Display is enabled if non-zero

ifdef PALETTES

NUM_PALETTES    equ     16              ; number of palettes
PaletteSupported equ    1               ; palette manager not supported
globalB device_local_brush ,0,<SIZE oem_brush_def>; for translate palette
globalW PaletteTranslationTable,0, NUM_PALETTES   ; the tranlate table
globalB PaletteModified,0                         ; table tampered ?
globalB TextColorXlated,0                         ; text colors translated
globalB device_local_drawmode,0,<SIZE DRAWMODE>   ; local drawmode structure
globalB device_local_pen,0,<SIZE oem_pen_def>     ; local pen definitions
endif

globalW wGraphicsMode,0                 ;Contains the Int 10h BIOS mode
                                        ;we use to get into graphics mode.
globalB bEquipmentFlag, 0               ;BYTE at 40:10 in ROM BIOS save area
if WM3_DETECT
globalB bWm3Capable,0                   ;h/w supports Write Mode 3? 2=yes, 1=no.
endif
sEnd    Data
page

createSeg _INIT,InitSeg,word,public,CODE
sBegin  InitSeg
assumes cs,InitSeg


SCREEN_W_BYTES  equ     SCAN_BYTES*1    ;("*1" to get to public symbol table)
COLOR_FORMAT    equ     (0100h + NUMBER_PLANES)
COLOR_DONT_CARE equ     0f00h + GRAF_CDC;Value for color don't care register

;-----------------------------------------------------------------------;
;       PhysDeviceSize is the number of bytes that the enable routine
;       is to copy into the passed PDevice block before calling the
;       physical_enable routine.  For this driver, the length is the
;       size of the bitmap structure.
;-----------------------------------------------------------------------;

PHYS_DEVICE_SIZE equ    size BITMAP



;-----------------------------------------------------------------------;
;       Allocate the physical device block for the EGA Card.
;       For this driver, physical devices will be in the same format
;       as a normal bitmap descriptor.  This is very convienient since
;       it simplifies the structures that the code must work with.
;
;       The bmWidthPlanes field will be set to zero to simplify some
;       of the three plane code.  By setting it to zero, it can be
;       added to a memory bitmap pointer without changing the pointer.
;       This allows the code to add this in regardless of the type of
;       the device.
;
;       The actual physical block will have some extra bytes stuffed on
;       the end (IntPhysDevice structure), but only the following is static
;-----------------------------------------------------------------------;
;The following constants keep the parameter list to BITMAP within
;view on an editing display 80 chars wide.

P               equ      COLOR_FORMAT AND 000FFh        ;# color planes
B               equ     (COLOR_FORMAT AND 0FF00h) SHR 8 ;# bits per pixel

physical_device:
dw      ScreenSelector  ;bmType
dw      SCREEN_WIDTH    ;bmWidth
dw      SCREEN_HEIGHT   ;bmHeight
dw      SCREEN_W_BYTES  ;bmWidthBytes
db      P               ;bmPlanes
db      B               ;bmBitsPixel
dw      0               ;bmBits
dw      ScreenSelector  ;bmBits
dd      0               ;bmWidthPlanes
dd      0               ;bmlpPDevice
dw      0               ;bmSegmentIndex
dw      0               ;bmScanSegment
dw      0               ;bmFillBytes
dw      0               ;
dw      0               ;

;-----------------------------------------------------------------------;
;       The GDIInfo data Structure.  The specifics of the EGA
;       mode are passed to GDI via this structure.
;-----------------------------------------------------------------------;
        public  drivers_TC_caps
        public  drivers_RC_caps

info_table_base label byte

        dw      400h                    ;version number = 4.00
        errnz   dpVersion

        dw      DT_RASDISPLAY           ;Device classification
        errnz   dpTechnology-dpVersion-2

        cdw     240,208                 ;Horizontal size in millimeters
        errnz   dpHorzSize-dpTechnology-2

        cdw     180,156                 ;Vertical size in millimeters
        errnz   dpVertSize-dpHorzSize-2

        dw      SCREEN_WIDTH            ;Horizontal width in pixels
        errnz   dpHorzRes-dpVertSize-2

        dw      SCREEN_HEIGHT           ;Vertical width in pixels
        errnz   dpVertRes-dpHorzRes-2

        dw      1                       ;Number of bits per pixel
        errnz   dpBitsPixel-dpVertRes-2

        dw      4                       ;Number of planes
        errnz   dpPlanes-dpBitsPixel-2

        dw      -1                      ;Number of brushes the device has
        errnz   dpNumBrushes-dpPlanes-2 ;  (Show lots of brushes)

        dw      16*5                    ;Number of pens the device has
        errnz   dpNumPens-dpNumBrushes-2;  (16 colors * 5 styles)

        dw      0                       ;Reserved

        dw      0                       ;Number of fonts the device has
        errnz   dpNumFonts-dpNumPens-4

        dw      16                      ;Number of colors in color table
        errnz   dpNumColors-dpNumFonts-2

        dw      size int_phys_device    ;Size required for device descriptor
        errnz   dpDEVICEsize-dpNumColors-2

        dw      CC_NONE                 ;Curves capabilities
        errnz   dpCurves-dpDEVICEsize-2

        dw      LC_POLYLINE+LC_STYLED   ;Line capabilities
        errnz   dpLines-dpCurves-2

        dw      PC_SCANLINE             ;Polygonal capabilities
        errnz   dpPolygonals-dpLines-2

drivers_TC_caps label   word
ifdef ONLY286
        dw      TC_CP_STROKE+TC_RA_ABLE              ;Text capabilities
else
        dw      TC_CP_STROKE+TC_RA_ABLE+TC_EA_DOUBLE ;Text capabilities
endif
        errnz   dpText-dpPolygonals-2

        dw      CP_RECTANGLE            ;Clipping capabilities
        errnz   dpClip-dpText-2

drivers_RC_caps label word

ifdef ONLY286

ifdef PALETTES
                                        ;BitBlt capabilities
        dw      RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT+RC_SAVEBITMAP+RC_DI_BITMAP+RC_DIBTODEV+RC_PALETTE+RC_OP_DX_OUTPUT
else
        dw      RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT+RC_SAVEBITMAP+RC_DI_BITMAP+RC_DIBTODEV+RC_OP_DX_OUTPUT
endif

else

ifdef PALETTES
                                        ;BitBlt capabilities
        dw      RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT+RC_SAVEBITMAP+RC_DI_BITMAP+RC_DIBTODEV+RC_PALETTE+RC_BIGFONT+RC_OP_DX_OUTPUT
else
        dw      RC_BITBLT+RC_BITMAP64+RC_GDI20_OUTPUT+RC_SAVEBITMAP+RC_DI_BITMAP+RC_DIBTODEV+RC_BIGFONT+RC_OP_DX_OUTPUT
endif

endif
        errnz   dpRaster-dpClip-2

        dw      X_MAJOR_DIST            ;Distance moving X only
        errnz   dpAspectX-dpRaster-2

        dw      Y_MAJOR_DIST            ;Distance moving Y only
        errnz   dpAspectY-dpAspectX-2

        dw      HYPOTENUSE              ;Distance moving X and Y
        errnz   dpAspectXY-dpAspectY-2

        dw      MAX_STYLE_ERR           ;Length of segment for line styles
        errnz   dpStyleLen-dpAspectXY-2


        errnz   dpMLoWin-dpStyleLen-2   ;Metric  Lo res WinX,WinY,VptX,VptY
        cdw     15,2080                 ;  HorzSize * 10
        cdw     15,1560                 ;  VertSize * 10
        cdw     4,SCREEN_WIDTH                  ;  HorizRes
        cdw     -4,-SCREEN_HEIGHT               ;  -VertRes


        errnz   dpMHiWin-dpMLoWin-8     ;Metric  Hi res WinX,WinY,VptX,VptY
        cdw     150,20800               ;  HorzSize * 100
        cdw     150,15600               ;  VertSize * 100
        cdw     4,SCREEN_WIDTH                  ;  HorizRes
        cdw     -4,-SCREEN_HEIGHT               ;  -VertRes


        errnz   dpELoWin-dpMHiWin-8     ;English Lo res WinX,WinY,VptX,VptY
        cdw     375,325                 ;  HorzSize * 1000
        cdw     375,325                 ;  VertSize * 1000
if MASMFLAGS and SVGA
        cdw     254,318                 ;  HorizRes * 254
        cdw     -254,-318               ;  -VertRes * 254
else
        cdw     254,254                 ;  HorizRes * 254
        cdw     -254,-254               ;  -VertRes * 254
endif

        errnz   dpEHiWin-dpELoWin-8     ;English Hi res WinX,WinY,VptX,VptY
        cdw     3750,1625               ;  HorzSize * 10000
        cdw     3750,1625               ;  VertSize * 10000
if MASMFLAGS and SVGA
        cdw     254,159                 ;  HorizRes * 254
        cdw     -254,-159               ;  -VertRes * 254
else
        cdw     254,127                 ;  HorizRes * 254
        cdw     -254,-127               ;  -VertRes * 254
endif

        errnz   dpTwpWin-dpEHiWin-8     ;Twips          WinX,WinY,VptX,VptY
        cdw     5400,2340               ;  HorzSize * 14400
        cdw     5400,2340               ;  VertSize * 14400
if MASMFLAGS and SVGA
        cdw     254,159                 ;  HorizRes * 254
        cdw     -254,-159               ;  -VertRes * 254
else
        cdw     254,127                 ;  HorizRes * 254
        cdw     -254,-127               ;  -VertRes * 254
endif

        dw      96                      ;Logical Pixels/inch in X
        errnz   dpLogPixelsX-dpTwpWin-8

        dw      96                      ;Logical Pixels/inch in Y
        errnz   dpLogPixelsY-dpLogPixelsX-2

        dw      DC_IgnoreDFNP           ;dpDCManage
        errnz   dpDCManage-dpLogPixelsY-2

        dw      C1_GLYPH_INDEX+C1_SLOW_CARD ;CAPS1, Reserved fields
        dw      0
        dw      0
        dw      0
        dw      0

        dw      0
        dw      0
        dw      0

        errnz   <(offset $)-(offset info_table_base)-(size GDIINFO)>

ifdef PALETTES
; start of entries in version 3.0 of this structure

        dw      16                      ; number of palette entries
        errnz   dpNumPalReg-dpDCManage-12

        dw      18                      ; DAC resolution for RGB
        errnz   dpColorRes-dpNumPalReg-2

endif

page

;---------------------------Public-Routine------------------------------;
; physical_enable
;
;   VGA 640x480 graphics mode is enabled.  The VGA's Color-Don't-Care
;   register and palettes are set for an 8-color mode of operation.
;   The EGA state restoration code is initialized.
;
; Entry:
;       ES:DI --> ipd_format in our pDevice
;       DS:    =  Data
; Returns:
;       AX = non-zero to show success
; Error Returns:
;       AX = 0
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;       INT 10h
;       init_hw_regs
;-----------------------------------------------------------------------;
        assumes ds,Data
        assumes es,nothing

if MASMFLAGS and SVGA
szDisplay           db      "display",0         ;section name
szVgamode           db      "svgamode",0        ;key name
szSystem            db      "system.ini",0      ;file name
endif

physical_enable proc near

;----------------------------------------------------------------------------;
; allocate the scratch selector here                                         ;
;----------------------------------------------------------------------------;

        push    es
        xor     ax,ax
        cCall   AllocSelector,<ax>      ; get a free selector
        mov     ScratchSel,ax           ; save it

        pop     es
;-----------------------------------------------------------------------------;


        mov     al,3                    ;we will save old mode as mode 3
        stosb

; get the number of lines being displayed currently

        push    bp
        push    es
        push    ds
        pop     es
        lea     bp,LineAddr
        mov     ax,1130h
        mov     bx,0
        int     10h
        xor     al,al
        inc     dl
        cmp     dl,25
        jz      Line25
        mov     al,dl
Line25:
        mov     Line43,al
        mov     ax, 40h                 ;now get the equipment flag and if
        mov     es, ax                  ;a monochrome adaptor is currently
        mov     al, es:[10h]            ;used, select the color monitor so
        mov     bEquipmentFlag, al      ;that the subsequent int 10h suceeds
        and     al, 0efh
        mov     es:[10h], al
        pop     es
        pop     bp

if MASMFLAGS and SVGA
;-----------------------------------------------------------------------------;
;   Look in SYSTEM.INI to see if the user wants to override the BIOS mode
;   number
;
;       [display]
;           svgamode = <mode number -- decimal>
;-----------------------------------------------------------------------------;
        mov     ax,wGraphicsMode        ;Avoid calling GetPrivateProfileInt
        or      ax,ax                   ;if we have determined the mode before.
        jnz     short @f
        mov     ax,offset cs:[szDisplay]
        mov     bx,offset cs:[szVgamode]
        mov     cx,offset cs:[szSystem]
        cCall   GetPrivateProfileInt,<cs,ax,cs,bx,0,cs,cx>
@@:     push    ax                      ;ax contains mode from system.ini
        mov     si,offset wModeTable    ;si = top of mode table.
        or      ax,ax                   ;if ax is zero, then a mode number was
        jz      short SVGA_Next         ;not found in system.ini, drop into our loop.
@@:     mov     bl,(800/8)-1            ;setup bl,cx for call to SetAndValidate.
        mov     cx,600-1
        push    ax                      ;save the mode number across the call.
        push    si                      ;save the table index across the call.
        call    SetAndValidateMode      ;Go set the mode and make sure its right.
        pop     si
        pop     ax
        jc      SVGA_Success            ;Carry will be set if this mode is good.
SVGA_Next:
        mov     ax,cs:[si]              ;Get next mode number from table.
        add     si,4                    ;Advance table index.
        or      ax,ax                   ;If this is the sentinel then we are done.
        jnz     @b
        pop     ax
        xor     ax,ax
        jmp     phys_enable_20
SVGA_Success:
	mov	wGraphicsMode,ax	;Save for subsequent calls to Enable.
;
;We now need to register ourselves with the MiniVDD (if we're running with
;it).  This call won't do anything bad if we don't happen to be running
;with the MiniVDD.  We just have to tell it that we can't do background
;4 plane graphics mode DOS boxes:
;
.386
;
;First, get the doubleword address that we call to communicate with the
;VDD and the "pseudo VM-Handle" that we need for these calls to the VDD:
;
	mov	ax,1684h		;Int 2FH: GET DEVICE API ENTRY POINT
	mov	bx,0ah          	;this is VxD ID for VDD
	int	2fh			;get the API Entry Point
	mov	word ptr VDDEntryPoint,di
	mov	word ptr VDDEntryPoint+2,es
	mov	ax,1683h		;Int 2FH: GET OUR PSEUDO-VM HANDLE
	int	2fh			;returns OurVMHandle in BX
	mov	eax,VDD_DRIVER_REGISTER ;this is function code for VDD PM API
	movzx	ebx,bx			;VDD PM API needs OurVMHandle
	xor	di,di			;tell 'em there's no callback to do
	mov	es,di			;to set us back into Windows mode
	mov	edx,-1			;tell VDD NOT to attempt to virtualize
	call	dword ptr VDDEntryPoint
.286c

	pop	bx			;Is the mode that worked the same as
	cmp	wGraphicsMode,bx	;the mode in system.ini?
        je      short phys_enable_5     ;If yes, nothing else to be done. Otherwise,
                                        ;write this mode number into system.ini
        mov     ax,offset cs:[szDisplay] ;Section name
        mov     bx,offset cs:[szVgamode] ;Keyname
        mov     cx,offset cs:[szSystem]  ;File name
        mov     si,cs:[si-2]             ;Get pointer to decimal string.
        cCall   WritePrivateProfileString,<cs,ax,cs,bx,cs,si,cs,cx>
else
;
;Since the standard VGA driver can use off-screen memory for the
;SaveScreenBitmap area, and the VDD also uses off-screen memory to do
;virtualization of VGA 4 plane graphics apps in a window, we must register
;the address of this driver's shadow_mem_status flag by making a call to
;VDD.386's VDD_REGISTER_SSB_FLAGS routine.
;
.386
;
;First, get the doubleword address that we call to communicate with the
;VDD and the "pseudo VM-Handle" that we need for these calls to the VDD:
;
	mov	ax,1684h		;Int 2FH: GET DEVICE API ENTRY POINT
	mov	bx,0ah			;this is VxD ID for VDD
	int	2fh			;get the API Entry Point
	mov	word ptr VDDEntryPoint,di
	mov	word ptr VDDEntryPoint+2,es
	mov	ax,1683h		;Int 2FH: GET OUR PSEUDO-VM HANDLE
	int	2fh			;returns OurVMHandle in BX
	mov	eax,VDD_REGISTER_SSB_FLAGS
					;this is function code for VDD PM API
	movzx	ebx,bx			;VDD PM API needs OurVMHandle
	mov	si,DataOFFSET shadow_mem_status
					;DS:SI --> shadow_mem_status variable
	call	dword ptr VDDEntryPoint
.286c

        mov     ax,0012h                ; VGA 640x480 mode number
        mov     wGraphicsMode,ax
        call    SetMode
        cmp     al,12h                  ;Do the modes match?
	mov	ax,0
        jne     phys_enable_20          ;Mode wasn't set up.
endif
phys_enable_5:
        call    SetColors
        mov     enabled_flag,0FFh       ;Show enabled

        mov     do_int_30,1             ;int 30 call to be done before init
        mov     ax,ScreenSelector       ;init_hw_regs will require this
        mov     es,ax
        assumes es,nothing
        call    init_hw_regs
        mov     do_int_30,0             ;no more int 30 calls necessary

        assumes es,EGAMem

;       Check for extra EGA memory.  If present, then one bitmap at a
;       time can be sent there to speed up saving and restoring.

        xor     cl,cl                   ;Assume we don't have 256K

        test    ssb_mask,RC_SAVEBITMAP
        jz      phys_enable_15          ;Can't use speed-up
        mov     cl,SHADOW_EXISTS        ;We've got it

phys_enable_15:
        mov     shadow_mem_status,cl
	mov	ax,STOP_IO_TRAP
        int     2fh
phys_enable_ok:
if WM3_DETECT
        call    DetectWm3               ;Detect write mode 3.
        mov     bWm3Capable,al          ;save return value in global.
endif
        mov     ax,1                    ;success code

;----------------------------------------------------------------------------;

phys_enable_20:
        ret

physical_enable endp

if WM3_DETECT
;----------------------------------------------------------------------------;
;DetectWm3
;  Determines if this VGA adapter supports write mode 3.
;Exit:
;  ax  == 1 if Write Mode 3 is supported.
;      == 2 if Write Mode 3 is NOT supported.
;Preserves all registers.
;----------------------------------------------------------------------------;
DetectWm3       proc    near
        mov     ax,ScreenSelector
        mov     es,ax
        assumes es,EGAMem
        lea     di,tonys_bar_n_grill    ;we need a video memory location to
                                        ;do this test.
        mov     bx,010ah                ;bl=fg color, bh=bg color
        mov     dx,3ceh                 ;graphics controller
;----------------------------------------------------------------------------
; 1st we put the adapter into write mode 0 and write blue to the screen byte.
;----------------------------------------------------------------------------
        mov     ax,0005h        ;wm 0 in graf mode register
        out     dx,ax
        xor     al,al           ;al = set/reset register (0) = bg color
        mov     ah,bh           ;ah = bg color
        out     dx,ax
        mov     ax,0ff01h       ;enable s/r register = (01) = ffh
        out     dx,ax
        mov     ax,0FF08h       ;Set bitmask reg. (08) = FFh.
        out     dx,ax
        mov     es:[di],ah      ;write ffh to video ram.
;----------------------------------------------------------------------------
; Next, we put the adapter into write mode 3 and write green vertical bars
; to the byte.
;----------------------------------------------------------------------------
        mov     ax,0305h        ;wm3 in graf mode register
        out     dx,ax
        xor     al,al           ;al = set/reset register (0) = fg
        mov     ah,bl           ;ah = fg color
        out     dx,ax
        mov     ax,0ff01h       ;enable s/r register = (01) = ffh
        out     dx,ax
        mov     ax,0FF08h       ;Set bitmask reg. (08) = FFh.
        out     dx,ax
        mov     al,11110000b    ;write walking 1's to video memory
        xchg    al,es:[di]
        mov     si,2            ;assume success.
        mov     ax,0004         ;al=READ Map select register, ah=plane 0

        out     dx,ax           ;select plane 0.
        mov     cl,es:[di]
        cmp     cl,0fh
        jne     DW3_Nope

        inc     ah
        out     dx,ax           ;select plane 1
        mov     cl,es:[di]
        cmp     cl,0f0h
        jne     DW3_Nope

        inc     ah
        out     dx,ax           ;select plane 2
        mov     cl,es:[di]
        cmp     cl,00h
        jne     DW3_Nope

        inc     ah
        out     dx,ax           ;select plane 3
        mov     cl,es:[di]
        cmp     cl,0f0h
        je      DW3_Yep

;----------------------------------------------------------------------------
; Set VGA registers back to their default state.
;----------------------------------------------------------------------------
DW3_Nope:
        dec     si
DW3_Yep:
        mov     ax,0
        out     dx,ax           ;Set/Reset register (0) = 0
        inc     ax
        out     dx,ax           ;Enable Set/Reset register (1) = 0
        mov     ax,4
        out     dx,ax           ;Read Map Select (4) = 0
        inc     ax
        out     dx,ax           ;Read/Write Mode register (5) = 0
        mov     ax,0FF08H
        out     dx,ax           ;Bit Mask Register (8) = 0ffh
        mov     ax,si
        ret

DetectWm3       endp
endif
page

;---------------------------Public-Routine------------------------------;
; physical_disable
;
;   VGA 640x480 graphics mode is exited.  The previous mode of the
;   adapter is restored.
;
; Entry:
;       DS:SI --> int_phys_device
;       ES:    =  Data
; Returns:
;       AX = non-zero to show success
; Error Returns:
;       None
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,ES,DS,FLAGS
; Calls:
;       INT 10h
;       init_hw_regs
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,Data

physical_disable proc near
;----------------------------------------------------------------------------;
; disable the selector here                                                  ;
;----------------------------------------------------------------------------;

        push    es                      ; save
        mov     ax,ScratchSel           ; get the scratch selector
        cCall   FreeSelector,<ax>       ; free it
        pop     es                      ;restore ES

;----------------------------------------------------------------------------;

; if we have 43 line mode we must set the 350 scan line mode.

        push    ds
        mov     ax, 40h                 ;restore the original equipment flag
        mov     ds, ax                  ;value in ROM BIOS save area
        mov     al, bEquipmentFlag
        mov     ds:[10h], al
        pop     ds

        xor     ah,ah
        mov     enabled_flag,ah         ;Show disabled

        and     al, 30h
        cmp     al, 30h                 ;were we in 80x25 bw text mode?
        jne     restore_color_text      ;no

        mov     al, 7                   ;switch back into 80 by 25 line
        int     10h                     ;bw text mode.
        jmp     short LineOk

restore_color_text:
        cmp     Line43,43               ;43 line mode ?
        jnz     @f                      ;no.
        mov     ax,1201h                ;350 scan line mode
        mov     bl,30h                  ;set scan lines
        int     10h
@@:
        mov     al,[si].ipd_format
        sub     ah, ah
        int     10h

; restore 43 line mode if it existed at start

        cmp     Line43,0
        jz      LineOk
        mov     ax,1112h
        mov     bl,0
        int     10h
        mov     ax,0100h
        mov     cx,0607h
        int     10h
LineOk:
        mov     ax,START_IO_TRAP
        int     2fh                     ;start i/o trapping
phys_disable_ret:
;----------------------------------------------------------------------------;

        mov     al,1
        ret

physical_disable endp
page

;-----------------------------------------------------------------------;
;       Palette contains the palette values for the EGA card
;       to give the desired colors.  The planes have been
;       changed from the normal EGA plane definition to be
;       C0 = red, C1 = green, C2 = blue.  The intensity bit
;       will be set for all the colors.
;-----------------------------------------------------------------------;

palette label   byte

        db      00h                     ;Black
        db      0ch                     ;dark Red
        db      0ah                     ;dark Green
        db      0eh                     ;mustard
        db      01h                     ;dark Blue
        db      15h                     ;purple
        db      23h                     ;turquoise
        db      07h                     ;gray
        db      0fh                     ;blend of blue
        db      24h                     ;Red
        db      12h                     ;Green
        db      36h                     ;Yellow
        db      09h                     ;Blue
        db      2Dh                     ;Magenta
        db      1Bh                     ;Cyan
        db      3Fh                     ;White
        db      0                       ;Overscan will be black

if MASMFLAGS and SVGA
ModeEntry       struc
        wModeNumber     dw      ?
        pModeString     dw      ?
ModeEntry       ends

wModeTable      label   word
ModeEntry <6f62h,sz6f62h>       ;V7, VEGA, Tatung
ModeEntry <0029h,sz0029h>       ;Orchid, Genoa, Tseng ET4000, STB, Sigma, Allstar Peacock, VEGA
ModeEntry <0058h,sz0058h>       ;Paradise,AT&T VDC600, AST Plus, Compaq, Dell, HP D1180A
ModeEntry <005Bh,sz005Bh>       ;Tridentm Logix, ATI Prism Elite, Maxxon, SEFCO TVGA, Imtec
ModeEntry <0054h,sz0054h>       ;ATI VGA Wonder
ModeEntry <0064h,sz0064h>       ;Cirrus Logic
ModeEntry <0070h,sz0070h>       ;Everex, Cardinal, C&T
ModeEntry <0052h,sz0052h>       ;Oak Technologies VGA-16, AT&T VDC600
ModeEntry <0016h,sz0016h>       ;Tecmar VGA/AD
ModeEntry <006Ah,sz006Ah>       ;VESA, Ahead B, Genoa 6400, Zymos Poach, ATI VGA Wonder
        dw      0

sz6f62h: db     "28514",0
sz0029h: db     "41",0
sz0058h: db     "88",0
sz0054h: db     "84",0
sz005Bh: db     "91",0
sz0064h: db     "100",0
sz0070h: db     "112",0
sz0052h: db     "82",0
sz0016h: db     "22",0
sz006Ah: db     "106",0


;----------------------------------------------------------------------------;
; Entry:
;   ax = Desired mode number.
;   bl = desired byte width-1 of screen.
;   cx = desired height-1
; Exit:
;   Carry if mode is correct.
;----------------------------------------------------------------------------;
SetAndValidateMode      proc    near
        call    SetMode
        cmp     al,ah                   ;Do the modes match?
        jnz     VM_Error                ;No. Get out.

;----------------------------------------------------------------------------;
;We are in the right mode, so, now check if the video hardware is
;setup correctly.
;----------------------------------------------------------------------------;
        mov     dx,3cch                 ;Read misc. output reg. of the
        in      al,dx                   ;CRTC.
        test    al,1                    ;Is this a mono mode?
        jz      VM_Error                ;yes. Exit with carry clear.
;----------------------------------------------------------------------------;
; Check vertical height.
; First we assemble the vertical height count from the VGA registers, then
; we compare it against cx which contains our desired height.
;----------------------------------------------------------------------------;
        mov     dx,3d4h                 ;Get the index reg. of the CRTC.
        in      al,dx
        mov     bh,al                   ;Save current state in bh.
        mov     al,7
        out     dx,al                   ;Point to overflow register.
        inc     dx
        in      al,dx
;----------------------------------------------------------------------------;
; Extract bits from al and put into bits 8 and 9 of ax.
;----------------------------------------------------------------------------;
        and     al,01000010b            ;Bits  2 and 6 are msb's of height.
        xor     ah,ah                   ;ax = 00000000 0#0000#0
        shl     ax,2                    ;ax = 0000000# 0000#000
        rcl     al,5                    ;ax = 0000000# 00000000 ;carry may be set.
        rcl     ah,1                    ;ax = 000000## 00000000 ;
;----------------------------------------------------------------------------;
; Get lsb of height into al.
;----------------------------------------------------------------------------;
        dec     dx
        mov     al,12h                  ;Point to vertical display end reg.
        out     dx,al
        inc     dx                      ;point to its read register.
        in      al,dx                   ;al = lsb of number of scans.
        cmp     ax,cx                   ;Is the actual height = desired height.
        je      short @f                ;Yes...move on to next test.
        shl     ax,1                    ;No...check if h/w is reporting half
        inc     ax                      ;the height.  If so, accept because
        cmp     ax,cx                   ;its no doubt doubling.
        je      short @f
        jmp     short VM_Error
;----------------------------------------------------------------------------;
; Check horizontal width.
;----------------------------------------------------------------------------;
@@:     dec     dx
        mov     al,1                    ;point to horizontal end register.
        out     dx,al
        inc     dx                      ;point to its data.
        in      al,dx
        cmp     al,bl                   ;Check if screen is 100 bytes across.
        je      short @f                ;It is, move on to next test.
        shl     al,1                    ;Well, maybe the width is halved...
        inc     al
        cmp     al,bl                   ;Now do they match?
        je      short @f                ;Yes, move on to next test.
        jmp     short VM_Error          ;Otherwise, fail.
@@:     dec     dx
        mov     al,bh                   ;Restore saved state of CRTC register.
        out     dx,al
;----------------------------------------------------------------------------;
; Check for 16 color mode.
;----------------------------------------------------------------------------;
        mov     dx,3ceh                 ;Point to the graphics controller.
        in      al,dx
        mov     ah,al                   ;Save current state.
        mov     al,5
        out     dx,al                   ;Point to the mode register.
        inc     dx                      ;point to its data.
        in      al,dx                   ;read its data.
        test    al,1000000b             ;Check shift 256 bit.
        jnz     VM_Error                ;If set, we are in 256 color mode. This is bad.
        dec     dx
        mov     al,ah
        out     dx,al                   ;Restore graphics controller state.
        stc
        ret
VM_Error:
        clc
        ret
SetAndValidateMode      endp
endif

sEnd    InitSeg
page

;-----------------------------------------------------------------------;
;       Color Table contains the color table definition.  The color
;       table is used for the GetColorTable Escape function and for
;       pen and brush enumeration.
;
;       This table must match the palette register values issued to
;       the EGA palette registers to get the colors in the color table.
;
;       The table is also used to take the color index which is
;       created for a GetPixel and turn it into a color which
;       sum_RGB_colors_alt can deal with.
;-----------------------------------------------------------------------;


COLOR_TBL_SIZE  equ     16              ;16 entries in the table


createSeg _PIXEL,PixelSeg,word,public,CODE
sBegin  PixelSeg

PixelSeg_color_table    label   dword

;               dd      xxbbggrr
                dd      00000000h       ;Black
                dd      00000080h       ;Dark Red
                dd      00008000h       ;Dark Green
                dd      00008080h       ;Mustard
                dd      00800000h       ;Dark Blue
                dd      00800080h       ;Purple
                dd      00808000h       ;Turquoise
                dd      00808080h       ;dark Gray
                dd      00c0c0c0h       ;light gray
                dd      000000ffh       ;Red
                dd      0000ff00h       ;Green
                dd      0000ffffh       ;Yellow
                dd      00ff0000h       ;Blue
                dd      00ff00ffh       ;Magenta
                dd      00FFFF00h       ;Cyan
                dd      00ffffffh       ;White

sEnd    PixelSeg
page

createSeg _BLUEMOON,BlueMoonSeg,word,public,CODE
sBegin  BlueMoonSeg

BlueMoonSeg_color_table label   dword

;               dd      xxbbggrr
                dd      00000000h       ;Black
                dd      00000080h       ;Dark Red
                dd      00008000h       ;Dark Green
                dd      00008080h       ;Mustard
                dd      00800000h       ;Dark Blue
                dd      00800080h       ;Purple
                dd      00808000h       ;Turquoise
                dd      00808080h       ;dark Gray
                dd      00c0c0c0h       ;light gray
                dd      000000ffh       ;Red
                dd      0000ff00h       ;Green
                dd      0000ffffh       ;Yellow
                dd      00ff0000h       ;Blue
                dd      00ff00ffh       ;Magenta
                dd      00FFFF00h       ;Cyan
                dd      00ffffffh       ;White

sEnd    BlueMoonSeg

createSeg  _DIMAPS,DIMapSeg,word,public,code
sBegin  DIMapSeg
;--------------------------------------------------------------------------
;This color table is copied into the DIB on a GetDIBitmap call.  The color
;entries in a DIB (RGBQUAD structure) are reversed from the colortables
;maintained in this driver.  Therefore, any changes to the colortables
;above should be made to the following table as well but in a reversed
;order, i.e., xxbbggrr => xxrrggbb.
;--------------------------------------------------------------------------
DIMapSeg_color_table label      dword

if NUMBER_PLANES eq 4

;               dd      xxrrggbb
                dd      00000000h               ;Black
                dd      00800000h               ;Dark Red
                dd      00008000h               ;Dark Green
                dd      00808000h               ;Mustard
                dd      00000080h               ;Dark Blue
                dd      00800080h               ;Purple
                dd      00008080h               ;Turquoise
                dd      00808080h               ;dark grey
                dd      00c0c0c0h               ;light grey
                dd      00ff0000h               ;Red
                dd      0000ff00h               ;Green
                dd      00ffff00h               ;Yellow
                dd      000000ffh               ;Blue
                dd      00ff00ffh               ;Magenta
                dd      0000ffffh               ;Cyan
                dd      00ffffffh               ;White

else

                dd      00000000h               ;Black
                dd      00ff0000h               ;Red
                dd      0000ff00h               ;Green
                dd      00ffff00h               ;Yellow
                dd      000000ffh               ;Blue
                dd      00ff00ffh               ;Magenta
                dd      0000ffffh               ;Cyan
                dd      00ffffffh               ;White

endif
sEnd    DIMapSeg

sBegin  Code
assumes cs,Code
;-----------------------------------------------------------------------;
;       The routine SaveScreenBitmap (in SSB.ASM) saves and restores a
;       between visible and offscreen memory (ssb_device).
;       This offscreen region starts at A000:A000 and extends to
;       A000:FFFF.  This is 24576 bytes of memory which is less than
;       the 38400 bytes of visible screen memory (64%).
;
;       this no longer uses bitblt so the old ssb_device is no longer
;       needed.  these constants are used in SSB.ASM
;-----------------------------------------------------------------------;


; this is the offset into the frame buffer where the save area
; starts.  it includes the CUR_HEIGHT for the mouse drawing code

SSB_START_OFFSET  equ    (SCREEN_HEIGHT + CUR_HEIGHT) * SCAN_BYTES
SSB_END_OFFSET    equ    0FFFFh
SSB_SIZE_IN_BYTES equ    SSB_END_OFFSET - SSB_START_OFFSET + 1



public  PMatchTable1
public  PMatchTable2
public  PMatchTable3
public  PIndexTable1
public  PIndexTable2
public  PIndexTable3
public  PAccelTable
public  PColorTable
public  NUMBER_CL1_COLORS
public  NUMBER_CL2_COLORS
public  NUMBER_CL3_COLORS

NUMBER_CL1_COLORS       equ     3
NUMBER_CL2_COLORS       equ     3
NUMBER_CL3_COLORS       equ     4

PMatchTable1    label   byte
PMatchTable2    label   byte
        db      0
        db      80h
        db      0ffh

PMatchTable3    label   byte
        db      0
        db      80h
        db      0c0h
        db      0ffh

PIndexTable1    label   byte
        db      0
        db      1
        db      9

PIndexTable2    label   byte
        db      0
        db      3
        db      0bh

PIndexTable3    label   byte
        db      0
        db      7
        db      8
        db      0fh

PAccelTable label   byte
        db      20h                     ;black
        db      00h
        db      00h
        db      00h
        db      00h
        db      00h
        db      00h
        db      00h
        db      10h                     ;light grey
        db      00h                     ;red
        db      10h                     ;green
        db      10h                     ;yellow
        db      00h                     ;blue
        db      10h                     ;magenta
        db      10h                     ;cyan
        db      30h                     ;white

PColorTable label   byte
        db      0, 0, 0
        db      80h, 0, 0
        db      0, 80h, 0
        db      80h, 80h, 0
        db      0, 0, 80h
        db      80h, 0, 80h
        db      0, 80h, 80h
        db      80h, 80h, 80h
        db      0c0h, 0c0h, 0c0h
        db      0ffh, 0, 0
        db      0, 0ffh, 0
        db      0ffh, 0ffh, 0
        db      0, 0, 0ffh
        db      0ffh, 0, 0ffh
        db      0, 0ffh, 0ffh
        db      0ffh, 0ffh, 0ffh

;----------------------------------------------------------------------------;
; define two hack routines for EGA color tranalation. The first one is a     ;
; NOP for VGA, and the next one builds the color planes of a solid brush with;
; index 8.                                                                   ;
; (these operate differently under EGA.                                      ;
;----------------------------------------------------------------------------;

        public  EGAindex8to7
        public  RGB192Brush

EGAindex8to7    proc near

        ret

EGAindex8to7    endp

RGB192Brush     proc near

        xor     ax,ax                   ;value for the 3 color planes
        mov     cx,12                   ;12 words for 3 planes
        rep     stosw                   ;fill in the color planes
        dec     ax                      ;intensity is all 0's
        mov     cx,4                    ;4 word here
        rep     stosw                   ;do the intensity plane
        ret

RGB192Brush     endp
;----------------------------------------------------------------------------;

sEnd    Code
end

