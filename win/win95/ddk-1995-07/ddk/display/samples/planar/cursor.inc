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

; MASMFLAGS and SVGA means 800x600 16 color vga

; Tecmar VGA-AD			TSENG		0x16

; Genoa 6400			GENOA 		0x29	41 dec
; STB EM			TSENG4		0x29
; Orchid Pro Designer II	TSENG4		0x29
; Genoa 5400			TSENG		0x29
; Genoa 5200			TSENG		0x29
; Orchid Designer		TSENG		0x29
; Sigma VGA/H			TSENG		0x29
; STB VGA/EM			TSENG		0x29

; ATI VGA Wonder		ATI		0x54	84 dec

; AST				PARADISE	0x58    88 dec
; AST VGA Plus			PARADISE	0x58
; Compaq			PARADISE	0x58
; Dell				PARADISE	0x58
; Paradise VGA Pro		PARADISE	0x58

; ATI Prism Elite		TRIDENT 	0x5b	91 dec
; Imtec				TRIDENT 	0x5b
; Maxxon			TRIDENT 	0x5b
; Logix				TRIDENT 	0x5b
; Trident 8900			ZYMOS 		0x5b

; Vega V7			CIRRUS		0x62 0x6f05	98 dec
; Northgate Pro 		VIDEO7		0x62 0x6f05
; Video 7 V7 VRAM		VIDEO7		0x62 0x6f05
; Video 7 Fastwrite		VIDEO7		0x62 0x6f05
; Video 7 Fastwrite AD		VIDEO7		0x62 0x6f05
; Video 7 VRAM			VIDEO7		0x62 0x6f05

; Hi Res 512			ZYMOS 		0x6A	106 dec
; Ahead B Chip			AHEADB		0x6A

; Everev 678			TRIDENT 	0x70	112 dec
; Everex EV-673			EVEREX		0x70
; Cardinal			C_T 		0x70
; C&T MK82452			C_T		0x70



; Module Name:	CURSOR.INC
;
; This file contains definitions for the cursor requirements of
; the EGA/VGA Display Drivers.
;
;-----------------------------------------------------------------------;

CUR_HEIGHT	equ	32		;Height of cursor/icon
MAX_BUF_HEIGHT	equ	64		;Number of scans in work buffer

if MASMFLAGS and SVGA
SCREEN_WIDTH	equ	800		;Screen width in pixels
else
ifdef _BANK
SCREEN_WIDTH	equ	1024		;Screen width in pixels
else
SCREEN_WIDTH	equ	640		;Screen width in pixels
endif
endif
SCAN_BYTES	equ	SCREEN_WIDTH/8	;# bytes in one scanline

if MASMFLAGS and EGA
  ifdef OLIVETTI
SCREEN_HEIGHT	equ	400		;Screen height in pixels
  else
SCREEN_HEIGHT	equ	350		;Screen height in pixels
  endif
else
  if MASMFLAGS and SVGA
SCREEN_HEIGHT	equ	600		;Screen height in pixels
  else
    ifdef _BANK
SCREEN_HEIGHT	equ	768		;Screen height in pixels
    else
SCREEN_HEIGHT	equ	480		;Screen height in pixels
    endif
  endif
endif

CUR_ICON_WIDTH	equ	4			;Width in bytes of cursor/icon
SAVE_WIDTH	equ	CUR_ICON_WIDTH+1	;Width of save area
BUF_WIDTH	equ	SAVE_WIDTH+CUR_ICON_WIDTH  ;Width of work buffer
SCAN_INC	equ	SCAN_BYTES-BUF_WIDTH	;Index to next scan line
MASK_LENGTH	equ	SAVE_WIDTH*CUR_HEIGHT	;#bytes in AND/XOR mask
FARTHEST	equ	(CUR_ICON_WIDTH+1)*8	;How far apart in X old and
						;  new cursor may be for
						;  both to fit in work buffer
X_RATE		equ	1			;X mickeys/pixel rate
Y_RATE		equ	2			;Y mickeys/pixel rate
INIT_CURSOR_X	equ	SCREEN_WIDTH/2		;Initial cursor x
INIT_CURSOR_Y	equ	SCREEN_HEIGHT/2 	;Initial cursor x
CUR_ROUND_LEFT	equ	0FFF0h			;Round down to a word
CUR_ROUND_RIGHT equ	0000Fh			;Round up   to a word
