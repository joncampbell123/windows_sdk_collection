;;; VKOEM.INC ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   Copyright (C) 1989,1990 by Microsoft Corp.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	History
;
; 3.00 Windows:
;
;	15 jul 89	peterbe		Added comments on F17, F18
;	09 jun 89	peterbe		Deleted VK_OEM_NUMBER
;	08 sep 88	peterbe		Removed Ifdef NOKIA .. always valid now.
;					Removed VK_OEM_ALT.  Rearranged in 
;					numeric order.
;	14 aug 88	peterbe		Changed 'ERICSSON' to 'NOKIA'

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

; C1h..DAh unassigned

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
VK_OEM_FINNISH	= VK_ERICSSON_BASE + 9	; f1
VK_OEM_COPY	= VK_ERICSSON_BASE + 10	; f2
VK_OEM_AUTO	= VK_ERICSSON_BASE + 11	; f3
VK_OEM_ENLW	= VK_ERICSSON_BASE + 12	; f4
VK_OEM_BACKTAB	= VK_ERICSSON_BASE + 13	; f5


; F6h..FEh unassigned.

