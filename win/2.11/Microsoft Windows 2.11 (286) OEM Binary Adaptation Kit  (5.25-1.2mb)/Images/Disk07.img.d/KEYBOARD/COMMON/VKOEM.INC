;	History
;	 4-30-85	??	Microsoft version
;	23 sep 85	plb	Added VK_ codes for ICO keyboard
;	 3 oct 85	plb	Changed VK_ICO_* codes a little
;	14 apr 86	plb	Added VK_OEM_ALT, renumbered VK_OEM_8
;	21 apr 86	plb	Renumbered VK_OEM_ALT
;	 3 mar 87	plb	Added VK_OEM_SHIFTLOCK.
;	 4 mar 87	plb	made VK_OEM_SHIFTLOCK same as VK_CAPITAL
;	 5 mar 87	plb	commented VK_ICO_DIVIDE, VK_ICO_MULTIPLY out
;				Added VK_OEM_102 for 102nd key <> on RT kbd.
;	12 oct 87	AP	Added Ericsson 9140 keys
if1
%out Olivetti VKOEM.INC   21 apr 86
  ifdef	ERICSSON
%out .	with Ericsson support 87-10-12
  endif
endif
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

VK_OEM_NUMBER	= 090H		; NumLock
VK_OEM_SCROLL	= 091H		; ScrollLock

VK_OEM_1	= 0BAH		; ';:' for US
VK_OEM_PLUS	= 0BBH		; '+' any country
VK_OEM_COMMA	= 0BCH		; ',' any country
VK_OEM_MINUS	= 0BDH		; '-' any country
VK_OEM_PERIOD	= 0BEH		; '.' any country
VK_OEM_2	= 0BFH		; '/?' for US
VK_OEM_3	= 0C0H		; '`~' for US

VK_OEM_4	= 0DBH		; '[{' for US
VK_OEM_5	= 0DCH		; '\|' for US
VK_OEM_6	= 0DDH		; ']}' for US
VK_OEM_7	= 0DEH		; ''"' for US
VK_OEM_8	= 0DFH

; Additional Olivetti keycodes for extended keyboard
VK_F17		= 0E0H		; F17 key
VK_F18		= 0E1H		; F18 key

VK_OEM_102	= 0E2H		; "<>" or "\|" on RT 102-key kbd.

VK_ICO_HELP	= 0E3H
VK_ICO_00	= 0E4H
; VK_ICO_DIVIDE	= 0E5H
VK_ICO_CLEAR	= 0E6H

; 

; This is a fake keycode for the ALT key.

VK_OEM_ALT	= 092h	; 0E7H renumbered

; This is the keycode for the fake SHIFT LOCK key

VK_OEM_SHIFTLOCK	= 14H	; = VK_CAPITAL

ifdef ERICSSON

;	ERICSSON definitions

VK_ERICSSON_BASE = 0E8H

VK_OEM_RESET	= VK_ERICSSON_BASE + 1
VK_OEM_JUMP	= VK_ERICSSON_BASE + 2
VK_OEM_PA1	= VK_ERICSSON_BASE + 3
VK_OEM_PA2	= VK_ERICSSON_BASE + 4
VK_OEM_PA3	= VK_ERICSSON_BASE + 5
VK_OEM_WSCTRL	= VK_ERICSSON_BASE + 6
VK_OEM_CUSEL	= VK_ERICSSON_BASE + 7
VK_OEM_ATTN	= VK_ERICSSON_BASE + 8
VK_OEM_FINNISH	= VK_ERICSSON_BASE + 9
VK_OEM_COPY	= VK_ERICSSON_BASE + 10
VK_OEM_AUTO	= VK_ERICSSON_BASE + 11
VK_OEM_ENLW	= VK_ERICSSON_BASE + 12
VK_OEM_BACKTAB	= VK_ERICSSON_BASE + 13

	IRP	x,<17,18,19,20,21,22,23,24>
VK_OEM_F&x	= 80H + (x - 17)
	ENDM

endif
