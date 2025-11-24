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

;;; VKOEM.INC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

if1
%out .	VKOEM.INC
endif
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This group is mainly used by Nokia.
; Some ICO keyboards will generate VK_OEM_F17 and VK_OEM_F18
; VK_OEM_F&x = 80h, 81h, 82h, 83h, 84h, 85h, 86h, 87h

	IRP	x,<17,18,19,20,21,22,23,24>
VK_OEM_F&x	= 80H + (x - 17)
	ENDM

; 88h..8Fh unassigned

; VK_OEM_NUMBER	= 090H		; NumLock: now VK_NUMLOCK, devined in VKWIN.INC
VK_OEM_SCROLL	= 091H		; ScrollLock

; 92h..B9h unassigned

VK_OEM_1	= 0BAH		; ';:' for US
VK_OEM_PLUS	= 0BBH		; '+' any country
VK_OEM_COMMA	= 0BCH		; ',' any country
VK_OEM_MINUS	= 0BDH		; '-' any country
VK_OEM_PERIOD	= 0BEH		; '.' any country
VK_OEM_2	= 0BFH		; '/?' for US
VK_OEM_3	= 0C0H		; '`~' for US

;
; C1h..DAh newly assigned for "Extended Chars" ie. Characters > 127
; on the codepage
;
;	[damianf] Multilingual Chicago
;

SVK_A		= 0C1H		;
SVK_B		= 0C2H		; Used to represent
SVK_C		= 0C3H		; Characters > 127 in the
SVK_D		= 0C4H		; Codepage.
SVK_E		= 0C5H		; Allows us to mix
SVK_F		= 0C6H		; keyboard Layouts
SVK_G		= 0C7H		; without conflict
SVK_H		= 0C8H		;
SVK_I		= 0C9H		;
SVK_J		= 0CAH		; Used mainly by Greek
SVK_K		= 0CBH		; and Russian Keyboard
SVK_L		= 0CCH		; layouts
SVK_M		= 0CDH		;
SVK_N		= 0CEH		;
SVK_O		= 0CFH		;
SVK_P		= 0D0H		;
SVK_Q		= 0D1H		;
SVK_R		= 0D2H		;
SVK_S		= 0D3H		;
SVK_T		= 0D4H		;
SVK_U		= 0D5H 		;
SVK_V		= 0D6H		;
SVK_W		= 0D7H		;
SVK_X		= 0D8H		;
SVK_Y		= 0D9H		;
SVK_Z		= 0DAH		;

VK_OEM_4	= 0DBH		; '[{' for US
VK_OEM_5	= 0DCH		; '\|' for US
VK_OEM_6	= 0DDH		; ']}' for US
VK_OEM_7	= 0DEH		; ''"' for US
VK_OEM_8	= 0DFH

; codes various extended or enhanced keyboards
VK_F17		= 0E0H		; F17 key on ICO, win 2.xx
VK_F18		= 0E1H		; F18 key on ICO, win 2.xx

VK_OEM_102	= 0E2H		; "<>" or "\|" on RT 102-key kbd.

VK_ICO_HELP	= 0E3H		; Help key on ICO
VK_ICO_00	= 0E4H		; 00 key on ICO

; E5h unassigned

VK_ICO_CLEAR	= 0E6H

; E7h .. E8h unassigned

;	Nokia/Ericsson definitions

VK_ERICSSON_BASE = 0E8H

VK_OEM_RESET	= VK_ERICSSON_BASE + 1	; e9
VK_OEM_JUMP	= VK_ERICSSON_BASE + 2	; ea
VK_OEM_PA1	= VK_ERICSSON_BASE + 3	; eb
VK_OEM_PA2	= VK_ERICSSON_BASE + 4	; ec
VK_OEM_PA3	= VK_ERICSSON_BASE + 5	; ed
VK_OEM_WSCTRL	= VK_ERICSSON_BASE + 6	; ee
VK_OEM_CUSEL	= VK_ERICSSON_BASE + 7	; ef
VK_OEM_ATTN	= VK_ERICSSON_BASE + 8	; f0
VK_OEM_FINNISH= VK_ERICSSON_BASE + 9	; f1
VK_OEM_COPY	= VK_ERICSSON_BASE + 10	; f2
VK_OEM_AUTO	= VK_ERICSSON_BASE + 11	; f3
VK_OEM_ENLW	= VK_ERICSSON_BASE + 12	; f4
VK_OEM_BACKTAB= VK_ERICSSON_BASE + 13	; f5


; F6h..FEh unassigned.

