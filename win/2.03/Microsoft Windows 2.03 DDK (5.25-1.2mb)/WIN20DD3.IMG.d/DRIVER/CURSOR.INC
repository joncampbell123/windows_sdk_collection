        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	CURSOR.INC
;
; This file contains definitions for the cursor requirements of
; the Hercules Monochrome Graphics Display Driver.
;
; Created: 23-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1984-1987 Microsoft Corporation
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

ifdef IBM_CGA
CUR_HEIGHT	equ	16			;Height of cursor/icon
MAX_BUF_HEIGHT	equ	32			;Number of scans in work buffer
SCREEN_WIDTH	equ	640			;Screen width in pixels
SCREEN_HEIGHT	equ	200			;Screen height in pixels
SCAN_BYTES	equ	80			;# bytes in one scanline
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

AddWidthTimesAxToSi	macro
        add     ax,ax                   ;*  2
        add     ax,ax                   ;*  4
        add     ax,ax                   ;*  8
        add     ax,ax                   ;* 16
        add     si,ax                   ;Add in * 16
        add     ax,ax                   ;* 32
        add     ax,ax                   ;* 64
        add     si,ax                   ;Add in * 64 for a total of 80
	errnz	SCAN_BYTES-80
endm         
endif


ifdef   HERCULES
CUR_HEIGHT	equ	32			;Height of cursor/icon
MAX_BUF_HEIGHT	equ	64			;Number of scans in work buffer
SCREEN_WIDTH	equ	720			;Screen width in pixels
SCREEN_HEIGHT	equ	348			;Screen height in pixels
SCAN_BYTES	equ	90			;# bytes in one scanline
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

AddWidthTimesAxToSi	macro
        add     ax,ax                   ;*  2
        add     si,ax                   ;Add in * 2
        add     ax,ax                   ;*  4
        add     ax,ax                   ;*  8
        add     si,ax                   ;Add in * 8
        add     ax,ax                   ;* 16
        add     si,ax                   ;Add in * 16
        add     ax,ax                   ;* 32
        add     ax,ax                   ;* 64
        add     si,ax                   ;Add in * 64 for a total of 90
	errnz	SCAN_BYTES-90
endm            
endif

