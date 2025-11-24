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

	page	,132
;*****************************************************************************
;
;	kbdfr.asm : FRENCH
;
;*****************************************************************************

VKFUN = 0
.xlist
include cmacros.inc
include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list
sBegin	DATA

sm_two		equ	0b2h 
sm_three	equ	0b3h
lu_acute	equ	0f9h
uu_acute	equ	0d9h
micron		equ	0b5h
eng_pound	equ	0a3h

;---------------------------------------------------------------------------

kbdfrLayout	label byte
public kbdfrLayout

	db	'DS'
	dd	0000040cH		; Default LCID
	dw	0				; Version
	dd	dataOFFSET pkbdfrStart		; Start of Data
	dW	DATASIZE				; Size of Data
	dd	0				; pDibData
	dw	0				; DIBSIZE
	dd	0				; pUnicode
	dd	0				; nUnicode
LayoutData	label byte

; pkbdfrDibData label word

; DIBSIZE EQU $ - pkbdfrDibData

pkbdfrStart label word
	dw		ALTGRUSED or SHIFTLOCKUSED or DEADCOMBOS ;
	dw		NSTATEKEYS		; NSTATE_KEYS
	dw		kbdfr_NSTATES		; NSTATES
	dw		4			; NUM_DEAD
	dw		0			; NUM_LIG
	KBDOFFSET       pkbdfrStates		; STATE_LIST
	KBDOFFSET       pkbdfrToAscStates	; TOASC_STATES
	KBDOFFSET       pkbdfrToAscStateTables	; STATETABLES
	KBDOFFSET       pkbdfrToAscVkeyList	; VKEY_LISTS
	KBDOFFSET       pkbdfrToAscVKeyListLens	; VKEY_LIST_LENS
	KBDOFFSET       pkbdfrVKShiftStates	; VK_STATES
	KBDOFFSET       pkbdfrScanToIdx		; SCANTOIDX
	KBDOFFSET       pkbdfrVKeyToIdx		; VKEYTOIDX
	dw		SCANSIZE		; SCAN_SIZE
	KBDOFFSET       kbdfrVKeyToAnsi		; VKEYTOANSI
	KBDOFFSET       pkbdfrDeadKeyTable	; DEAD_KEYS
	KBDOFFSET       pkbdfrDeadTrans		; DEAD_KEYTRANS
	dw		0			; LIG_KEYS
	KBDOFFSET		pEngCapsBits			; CAPS

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls

;---------------------------------------------------------------------------

pkbdfrStates	label byte
	db	VK_MENU,    080H
	db	VK_SHIFT,   080H
	db	VK_CONTROL, 080H

NSTATEKEYS equ ($ - pkbdfrStates) shr 1

	db	0
pkbdfrToAscStates	label byte
	db	1		; Alt
	db	2		; Shift
	db	3		; Alt+Shift
	db	5		; AltGr
	db	4		; control
	db	6		; controlShift

kbdfr_NSTATES equ $-pkbdfrToAscStates

pkbdfrVKShiftStates label byte
	db	0		; unshifted
	db	0		; alt
	db	1		; shifted
	db	1		; alt+shift
	db	6		; AltGr
	db	2		; control
	db	3		; control+shift

pkbdfrToAscStateTables label word
        KBDOFFSET       kbdfrToAscNormal
        KBDOFFSET       kbdfrToAscAlt
        KBDOFFSET       kbdfrToAscShift
        KBDOFFSET       kbdfrToAscAltShift
	KBDOFFSET	kbdfrToAscAltGr
	KBDOFFSET       kbdfrToAscControl
	KBDOFFSET       kbdfrToAscControlShift

pkbdfrToAscVKeyList label word
        KBDOFFSET       pkbdfrVKeyToIdx
        KBDOFFSET       pkbdfrVKeyToIdx
        KBDOFFSET       pkbdfrVKeyToIdx
        KBDOFFSET       pkbdfrVKeyToIdx
        KBDOFFSET       pkbdfrAltGrVKeyToIdx
	KBDOFFSET       pkbdfrCtrlVKeyToIdx
	KBDOFFSET       pkbdfrCtrlShiftVKeyToIdx

pkbdfrToAscVKeyListLens	label byte
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_ALTGRLIST_SIZE
	db VKEY_CTRL_LIST_SIZE
	db VKEY_CTRLSHIFT_LIST_SIZE

pkbdfrScanToIdx label byte
	db 41, 2, 3, 4, 5, 6, 7, 8
	db 9, 10, 11, 12, 13, 16, 17, 18
	db 19, 20, 21, 22, 23, 24, 25, 26
	db 27, 43, 30, 31, 32, 33, 34, 35
	db 36, 37, 38, 39, 40, 44, 45, 46
	db 47, 48, 49, 50, 51, 52, 53
	db 56H
SCANSIZE	equ	$ - pkbdfrScanToIdx

;---------------------------------------------------------------------------

pkbdfrVKeyToIdx label byte
;Virtual Keys
	db VK_OEM_7, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7
	db VK_8, VK_9, VK_0, VK_OEM_4, VK_OEM_PLUS, VK_A, VK_Z, VK_E
	db VK_R, VK_T, VK_Y, VK_U, VK_I, VK_O, VK_P, VK_OEM_6
	db VK_OEM_1, VK_OEM_5, VK_Q, VK_S, VK_D, VK_F, VK_G, VK_H
	db VK_J, VK_K, VK_L, VK_M, VK_OEM_3, VK_W, VK_X, VK_C
	db VK_V, VK_B, VK_N, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_8
	db VK_OEM_102	
VKEY_LIST_SIZE	equ	$ - pkbdfrVKeyToIdx

;---------------------------------------------------------------------------

kbdfrToAscControl label byte
	db 01BH, 01CH, 01DH, 01CH

pkbdfrCtrlVKeyToIdx label byte
	db 0DDH, 0DCH, 0BAH, 0E2H
VKEY_CTRL_LIST_SIZE equ $ - pkbdfrCtrlVKeyToIdx

;---------------------------------------------------------------------------

kbdfrToAscControlShift label byte
	db 01BH, 01FH, 01CH, 01EH

pkbdfrCtrlShiftVKeyToIdx label byte
	db 035H, 036H, 038H, 039H
VKEY_CTRLSHIFT_LIST_SIZE equ $ - pkbdfrCtrlShiftVKeyToIdx

;---------------------------------------------------------------------------

kbdfrToAscAltGr	label	byte
	db	07eh, 023h, 07bh, 05bh, 07ch, 060h, 05ch, circumflex
	db	040h, 05dh, 07dh, 0a4h

pkbdfrAltGrVKeyToIdx	label byte
	db	'234567890',0dbH, 0bbh, 0bah

VKEY_ALTGRLIST_SIZE	equ $-pkbdfrAltGrVKeyToIdx

;---------------------------------------------------------------------------

kbdfrToAscAlt		label byte
kbdfrToAscNormal	label byte
;Unshifted Keys'
	db 0B2H, '&й"''(-и_за)='
	db 'azertyuiop',circumflex, '$*'
	db 'qsdfghjklm', lu_acute
	db 'wxcvbn,;:!'
	db '<'

;---------------------------------------------------------------------------

kbdfrToAscAltShift	label byte
kbdfrToAscShift		label byte
;Shift State'
	db 0,'1234567890', ring, '+'
	db 'AZERTYUIOP', umlaut, eng_pound, micron
	db 'QSDFGHJKLM%'
	db 'WXCVBN?./§'
	db '>'

;---------------------------------------------------------------------------

kbdfrVKeyToAnsi	label byte
; Ansi Chars
	db sm_two, '1234567890)='
	db 'AZERTYUIOP',circumflex
	db '$*QSDFGHJKLM', uu_acute
	db 'WXCVBN,;:!'
	db '<'

;---------------------------------------------------------------------------
; DEADCOMBOS means that the dead key table is dwords with each entry as 
; follows: char | vkey | shiftstate | 0 |.
;
pkbdfrDeadKeyTable DEADKEYSTATE {circumflex, 0DDH, 0, 0},
                                {umlaut,     0DDH, 2, 0},
                                {tilde,      VK_2, 5, 0},
                                {grave,      VK_7, 5, 0}

pkbdfrDeadTrans	label byte
	dw	NDEADLIST
	db	circumflex,	97
	db	circumflex,	101
	db	circumflex,	117
	db	circumflex,	105
	db	circumflex,	111
	db	circumflex,	65
	db	circumflex,	69
	db	circumflex,	85
	db	circumflex,	73
	db	circumflex,	79
	db	circumflex,	' '

	db	umlaut,		65
	db	umlaut,		69
	db	umlaut,		85
	db	umlaut,		73
	db	umlaut,		79
	db	umlaut,		97
	db	umlaut,		101
	db	umlaut,		121
	db	umlaut,		117
	db	umlaut,		105
	db	umlaut,		111
	db	umlaut,		' '

	db	grave,		'a'
	db	grave,		'e'
	db	grave,		'i'
	db	grave,		'o'
	db	grave,		'u'
	db	grave,		'A'
	db	grave,		'E'
	db	grave,		'I'
	db	grave,		'O'
	db	grave,		'U'
	db	grave,		' '

	db	tilde,		'a'
	db	tilde,		'A'
	db	tilde,		'n'
	db	tilde,		'N'
	db	tilde,		'o'
	db	tilde,		'O'
	db	tilde,		' '

NDEADLIST	equ	(($-pkbdfrDeadTrans) shr 1)-1

pkbdfrDeadList	label byte
;circumflex
	db	226
	db	234
	db	251
	db	238
	db	244
	db	194
	db	202
	db	219
	db	206
	db	212
	db circumflex
;umlaut
	db	196
	db	203
	db	220
	db	207
	db	214
	db	228
	db	235
	db	255
	db	252
	db	239
	db	246
	db	umlaut

;grave
	db	0e0h, 0e8h, 0ech, 0f2h, 0f9h
	db	0c0h, 0c8h, 0cch, 0d2h, 0d9h, grave
;tilde
	db	0e3h, 0c3h, 0f1h, 0d1h, 0f5h, 0d5h, tilde

;---------------------------------------------------------------------------

DATASIZE equ $-pkbdfrStart

sEnd	DATA

;---------------------------------------------------------------------------


sBegin	CODE	; Beginning of code segment
assumes CS,CODE
assumes DS,DATA
sEnd	CODE

end
