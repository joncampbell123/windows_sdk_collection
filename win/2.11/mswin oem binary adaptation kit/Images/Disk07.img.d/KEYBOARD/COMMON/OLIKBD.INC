;*** OLIKBBD.INC ***********************************************************
;                                                                          *
;   Copyright (C) 1984 by Olivetti					   *
;                                                                          *
;                                                                          *
;	Definitions for Olivetti keyboard mouse, for MS-Windows		   *
;	KEYBOARD.EXE -- include this before KEYBOARD.INC		   *
;                                                                          *
;***************************************************************************
;
;	History
;	11 nov 87	plb	Added 'IBMPS2ID'
;	 2 jul 87	plb	Removed Olivetti light definitions.
;				Revised KB... Olivetti keyboard type defs.
;	29 jun 87	plb	Changed bits in RTFlags -- deleted kbRT
if1
%out Olikbd.inc      Windows 2.00
endif

; for writing to Olivetti M24 keyboard output port, need to check status
;
kb_status	equ	64h	; status port -- bit 1: ok to write

BIOSDATASEG	equ	40h	; segment of ROM BIOS data

BIOSDATA STRUC
		DB	17H DUP (?)
kb_flag		db	?
kb_flag_1	db	?
		db	?
buffer_head	dw	?
buffer_tail	dw	?
kb_buffer	dw	16 dup (?)

BIOSDATA ENDS


;
; mouse interrupt vector, used for linkage between keyboard and mouse.
; drivers.
;
mouse_sys_vec	equ	33H	; or 51D

; ANSI accent codes

IFDEF GREEK
; Greece
acute		equ	"'"	; Normal ASCII acute
grave		equ	060h
circumflex	equ	05eh
umlaut		equ	0c0h	; solid block in font
tilde		equ	07Eh
ELSE
; Other countries
acute		equ	0b4h
grave		equ	060h
circumflex	equ	05eh
umlaut		equ	0a8h
tilde		equ	07Eh
ENDIF

; definitions of Olivetti shift positions (used in keyboard diagrams
; and dead-key table). Bits of values (after SHL operator) are shift,
; alt, control keys.  The values below are bits of a mask used to determine
; whether the current shift state matches (1 or more) shift states.
;
; Oli. Name:	Value:	  Description:

shift_1	equ	1 SHL 0	; NO shift	(00000001)
shift_2	equ	1 SHL 1	; SHIFT		(00000010)
shift_3	equ	1 SHL 3	; SHIFT-ALT	(00001000)
shift_4	equ	1 SHL 2	; ALT only	(00000100)
shift_5	equ	1 SHL 6	; CTRL-ALT	(01000000)
			; These aren't usually on Olivetti keyboard diagrams:
shift_6	equ	1 SHL 4	; CONTROL only
shift_7	equ	1 SHL 7	; SHIFT-CTRL-ALT (10000000)
shift_8	equ	1 SHL 5	; CTRL-SHIFT

; Olivetti keyboard identifiers in the OliType byte.  Valid only
; for Olivetti-protocol keyboards.  This byte is 0 for others.

KB83		equ	2	; M24 83-key keyboard
KB302		equ	4	; AT&T 6300 PLUS
KB102		equ	1	; M24 102-key keyboard
KB86		equ	10h	; M28 keyboard with SYSREQ key
KBM28		equ	10h	; .. same thing
KB102RT		equ	40h	; 101-102 extended keyboard (RONCO)(RT)
KBXT83		equ	42h	; XT 83 key keyboard on M28


; .. and special bit values in KeyType for the RT (101/102) keyboard.

kbShiftLock	equ	40h	; set if Shiftlock instead of caps lock
kbSLTable	equ	20h	; special caps lock table exists (Swiss Germ.)
kbAltGr		equ	08h	; AltGr = CTRL-ALT -- all but USA


; System ID byte or word values (AT F000:FFFE)

; Olivetti systems, ID = 0

M24ID		equ	0	; PC 1050 (M24), M24SP, AT&T 6300
M24ID_2		equ	41h

; Olivetti systems, ID = FF

M22ID		equ	0FFh	; 1020 (but don't worry about this).
				; 2nd byte probably not FF
M15ID		equ	0FFh
M15ID_2		equ	046h

; Olivetti systems, ID = FE

M19ID		equ	0FEh	; PC 1030
				; 2nd byte is NOT FF

; Olivetti systems, ID = FC

M28ID		equ	0FCh	; PC 1076
M28ID_2		equ	42H	; 1.10 2nd byte
M28ID_2_old	equ	0	; 1.08 2nd byte

ID1041		equ	0FCh
ID1041_2	equ	044h

ID1078		equ	0FCh
ID1078_2	equ	042h

ID1338		equ	0FCh
ID1338_2	equ	048h

ID6300P		equ	0FCh	; AT&T 6300 plus
ID6300P_2	equ	0	; same as IBM AT!

; Olivetti systems, ID = FB

ID1040		equ	0FBh	; 
ID1040_2	equ	43h

ID1336		equ	0FBh	;
ID1336_2	equ	47h

; IBM systems

IBMPCID		equ	0FFh	; IBM PC, and M22/1020
IBMPCID_2	equ	0FFh	; IBM PC, NOT M22

IBMXTID		equ	0FEh	; IBM PC XT, and M19/1030
IBMPCID_2	equ	0FFh	; IBM PC, NOT 1030

IBMATID		equ	0FCh	; IBM PC AT, and Oli. M28, etc.
IBMATID_2	equ	0	; IBM PC AT

IBMPS2ID	equ	0FAh	; IBM PS/2

; make sure CR follows ;


