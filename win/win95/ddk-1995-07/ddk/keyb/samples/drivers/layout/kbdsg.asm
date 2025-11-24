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
;	kbdsg.asm SWISS(GERMAN)
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

eng_pound	equ	0a3h
lu_umlaut	equ	0fch
uu_umlaut	equ	0dch
lo_umlaut	equ	0f6h
uo_umlaut	equ	0d6h
la_umlaut	equ	0e4h
ua_umlaut	equ	0c4h

le_grave	equ	0e8h
ue_grave	equ	0c8h
le_acute	equ	0e9h
ue_acute	equ	0c9h
la_grave	equ	0e0h
ua_grave	equ	0c0h
paragraph	equ	0a7h

;---------------------------------------------------------------------------

kbdsgLayout	label byte
public kbdsgLayout

	db	'DS'
	dd	00000807H				; Default LCID
	dw	0						; Version
	dd	dataOFFSET pkbdsgStart	; Start of Data
	dW	DATASIZE				; Size of Data
	dd	0						; pDibData
	dw	0						; DIBSIZE
	dd	0						; pUnicode
	dd	0						; nUnicode

LayoutData	label byte

pkbdsgStart label word
	dw		ALTGRUSED 
	dw		NSTATEKEYS						; NSTATE_KEYS
	dw		kbdsg_NSTATES					; NSTATES
	dw		5								; NUM_DEAD
	dw		0								; NUM_LIG
	KBDOFFSET       pkbdsgStates			; STATE_LIST
	KBDOFFSET       pkbdsgToAscStates		; TOASC_STATES
	KBDOFFSET       pkbdsgToAscStateTables	; STATETABLES
	KBDOFFSET       pkbdsgToAscVkeyList		; VKEY_LISTS
	KBDOFFSET       pkbdsgToAscVKeyListLens	; VKEY_LIST_LENS
	KBDOFFSET       pkbdsgVKShiftStates		; VK_STATES
	KBDOFFSET       pkbdsgScanToIdx			; SCANTOIDX
	KBDOFFSET       pkbdsgVKeyToIdx			; VKEYTOIDX
	dw		SCANSIZE						; SCAN_SIZE
	KBDOFFSET       kbdsgVKeyToAnsi			; VKEYTOANSI
	KBDOFFSET       pkbdsgDeadKeyTable		; DEAD_KEYS
	KBDOFFSET       pkbdsgDeadTrans			; DEAD_KEYTRANS
	dw		0								; LIG_KEYS
	KBDOFFSET	pEngCapsBits				; CAPS

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required (not used here)
;
pEngCapsBits label byte		;used with CAPSNORMAL only (not applicable here)

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls
;---------------------------------------------------------------------------

pkbdsgStates	label byte
	db	VK_MENU,    080H
	db	VK_SHIFT,   080H
	db	VK_CONTROL, 080H
	db	VK_CAPITAL, 001H

NSTATEKEYS equ ($ - pkbdsgStates) shr 1

	db	0		; Normal
pkbdsgToAscStates	label byte
	db	1		; Alt
	db	2		; Shift
	db	3		; Alt+Shift
	db	4		; Control
	db	6		; Control+Shift
	db	5		; AltGr
	db	7		; AltGr+Shift
	db	8		; CapsLock
	db	9		; CapsLock + Alt
	db	10		; CapsLock + Shift	
	db	11		; CapsLock + Alt + Shift
	db	12		; CapsLock + Control
	db	14		; CapsLock + Control + Shift
	db	13		; CapsLock + AltGr
	db	15		; CapsLock + AltGr + Shift

kbdsg_NSTATES equ $-pkbdsgToAscStates

pkbdsgVKShiftStates label byte
	db	0		; unshifted
	db	0		; alt
	db	1		; shifted
	db	1		; alt+shift
	db	2		; Control
	db	3		; Control + Shift
	db	6		; AltGr
	db	7		; AltGr+Shift
	db	4		; CapsLock
	db	4		; CapsLock + Alt
	db	5		; CapsLock + Shift	
	db	5		; CapsLock + Alt + Shift
	db	2		; CapsLock + Control
	db	3		; CapsLock + Control + Shift
	db	6		; CapsLock + AltGr
	db	7		; CapsLock + AltGr + Shift

pkbdsgToAscStateTables label word
	KBDOFFSET       kbdsgToAscNormal
	KBDOFFSET       kbdsgToAscAlt
	KBDOFFSET       kbdsgToAscShift
	KBDOFFSET       kbdsgToAscAltShift
	KBDOFFSET       kbdsgToAscCtrl
	KBDOFFSET       kbdsgToAscCtrlShift
	KBDOFFSET       kbdsgToAscAltGr
	KBDOFFSET       kbdsgToAscAltGrShift
	KBDOFFSET       kbdsgToAscCapsLock
	KBDOFFSET       kbdsgToAscCapsLockAlt
	KBDOFFSET       kbdsgToAscCapsLockShift
	KBDOFFSET       kbdsgToAscCapsLockAltShift
	KBDOFFSET       kbdsgToAscCapsCtrl
	KBDOFFSET       kbdsgToAscCapsCtrlShift
	KBDOFFSET       kbdsgToAscCapsAltGr
	KBDOFFSET       kbdsgToAscCapsAltGrShift

pkbdsgToAscVKeyList label word
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgCtrlVKeyToIdx
	KBDOFFSET       pkbdsgCtrlShiftVKeyToIdx
	KBDOFFSET       pkbdsgAltGrVKeyToIdx
	KBDOFFSET       pkbdsgAltGrShiftVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgVKeyToIdx
	KBDOFFSET       pkbdsgCtrlVKeyToIdx
	KBDOFFSET       pkbdsgCtrlShiftVKeyToIdx
	KBDOFFSET       pkbdsgAltGrVKeyToIdx
	KBDOFFSET       pkbdsgAltGrShiftVKeyToIdx

pkbdsgToAscVKeyListLens	label byte
	db VKEY_LIST_SIZE_NORMAL
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db CTRL_VKEY_LIST_SIZE
	db CTRLSHIFT_VKEY_SIZE
	db ALTGR_VKEY_LIST_SIZE
	db ALTGRSHIFT_VKEY_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db CTRL_VKEY_LIST_SIZE
	db CTRLSHIFT_VKEY_SIZE
	db ALTGR_VKEY_LIST_SIZE
	db ALTGRSHIFT_VKEY_SIZE

;---------------------------------------------------------------------------
pkbdsgScanToIdx label byte
	db 41, 2, 3, 4, 5, 6, 7, 8
	db 9, 10, 11, 12, 13, 16, 17, 18
	db 19, 20, 21, 22, 23, 24, 25, 26
	db 27, 43, 30, 31, 32, 33, 34, 35
	db 36, 37, 38, 39, 40, 44, 45, 46
	db 47, 48, 49, 50, 51, 52, 53
	db 56H

SCANSIZE	equ	$ - pkbdsgScanToIdx
;
; NUMPAD keys
;
	db 053H		;NUMPAD DECIMAL

;---------------------------------------------------------------------------

pkbdsgVKeyToIdx label byte
;Virtual Keys
	db VK_OEM_2, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7
	db VK_8, VK_9, VK_0, VK_OEM_4, VK_OEM_6, VK_Q, VK_W, VK_E
	db VK_R, VK_T, VK_Z, VK_U, VK_I, VK_O, VK_P, VK_OEM_1
	db VK_OEM_3, VK_OEM_8, VK_A, VK_S, VK_D, VK_F, VK_G, VK_H
	db VK_J, VK_K, VK_L, VK_OEM_7, VK_OEM_5, VK_Y, VK_X, VK_C
	db VK_V, VK_B, VK_N, VK_M, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_MINUS
	db VK_OEM_102
		
VKEY_LIST_SIZE	equ	$ - pkbdsgVKeyToIdx
;
;  NUMPAD keys
;
	db 06EH		;NUMPAD DECIMAL

VKEY_LIST_SIZE_NORMAL	equ	$ - pkbdsgVKeyToIdx

;---------------------------------------------------------------------------

kbdsgToAscCapsCtrl label byte
kbdsgToAscCtrl	 label byte
	db	001BH, 01FH, 01CH, 01DH, 01CH

pkbdsgCtrlVKeyToIdx label byte
	db	0BAH, 0BDH, 0DFH, 0C0H, 0E2H

CTRL_VKEY_LIST_SIZE equ $-pkbdsgCtrlVKeyToIdx

;---------------------------------------------------------------------------

kbdsgToAscCapsCtrlShift label byte
kbdsgToAscCtrlShift	 label byte
	db	001EH

pkbdsgCtrlShiftVKeyToIdx label byte
	db	036H

CTRLSHIFT_VKEY_SIZE equ $-pkbdsgCtrlShiftVKeyToIdx

;---------------------------------------------------------------------------

pkbdsgAltGrVKeyToIdx label byte
	db VK_1, VK_2, VK_3, VK_4, VK_5, VK_6      
	db VK_7, VK_8, VK_OEM_1, VK_OEM_3, VK_OEM_5, VK_OEM_8  
	db VK_OEM_102, VK_OEM_4, VK_OEM_6

kbdsgToAscCapsAltGr label byte
kbdsgToAscAltGr	 label byte
	db	0a6h, '@#', ring, paragraph, 0ach, '|'
	db	0a2h, '[]{}\'
	db acute, tilde

ALTGR_VKEY_LIST_SIZE equ $-kbdsgToAscAltGr

;---------------------------------------------------------------------------
pkbdsgAltGrShiftVKeyToIdx label byte
	db	0DBH, 0DDH

kbdsgToAscCapsAltGrShift label byte
kbdsgToAscAltGrShift	 label byte
	db	0B4H, 07EH

ALTGRSHIFT_VKEY_SIZE equ $-kbdsgToAscAltGrShift

;---------------------------------------------------------------------------
kbdsgToAscAlt		label byte
;Alt Key State'

kbdsgToAscNormal	label byte
;Unshifted Keys'
	db paragraph, '1234567890''^'
	db 'qwertzuiop', lu_umlaut, umlaut, '$'
	db 'asdfghjkl', lo_umlaut, la_umlaut
	db 'yxcvbnm,.-'
	db '<'
	db 02EH
;---------------------------------------------------------------------------

kbdsgToAscAltShift	label byte
;Alt Shift State'
;
kbdsgToAscShift		label byte
;Shift State'
	db ring, '+"*ç%&/()=?`'
	db 'QWERTZUIOP', le_grave, '!', eng_pound
	db 'ASDFGHJKL', le_acute, la_grave
	db 'YXCVBNM;:_'
	db '>'
	db 08H

;---------------------------------------------------------------------------
kbdsgToAscCapsLockShift		label byte
kbdsgToAscCapsLockAltShift	label byte
;CapsLock Shift
	db ring, '+"*ç%&/()=?`'
	db 'qwertzuiop', ue_grave, '!', eng_pound
	db 'asdfghjkl', ue_acute, ua_grave
	db 'yxcvbnm;:_'
	db '>'
	db 08H

;---------------------------------------------------------------------------
kbdsgToAscCapsLock		label byte
kbdsgToAscCapsLockAlt	label byte
;CapsLock characters

kbdsgVKeyToAnsi	label byte
;ANSII characters
	db paragraph, '1234567890''^'
	db 'QWERTZUIOP', uu_umlaut, umlaut, '$'
	db 'ASDFGHJKL', uo_umlaut, ua_umlaut
	db 'YXCVBNM,.-'
	db '<'
	db 2EH
;---------------------------------------------------------------------------

pkbdsgDeadKeyTable	label byte
	Db	GRAVE	  
	Db	ACUTE	  
	Db	CIRCUMFLEX
	Db	UMLAUT	  
	Db	TILDE	  

pkbdsgDeadTrans	label byte
	dw	NDEADLIST

	db	grave,	'a'
	db	grave,	'e'
	db	grave,	'i'
	db	grave,	'o'
	db	grave,	'u'
	db	grave,	'A'
	db	grave,	'E'
	db	grave,	'I'
	db	grave,	'O'
	db	grave,	'U'
	db	grave,	' '

	db	acute,	'a'
	db	acute,	'e'
	db	acute,	'i'
	db	acute,	'o'
	db	acute,	'u'
	db	acute,	'y'
	db	acute,	'A'
	db	acute,	'E'
	db	acute,	'I'
	db	acute,	'O'
	db	acute,	'U'
	db	acute,	'Y'
	db	acute,	' '

	db	circumflex,'a'
	db	circumflex,'e'
	db	circumflex,'i'
	db	circumflex,'o'
	db	circumflex,'u'
	db	circumflex,'A'
	db	circumflex,'E'
	db	circumflex,'I'
	db	circumflex,'O'
	db	circumflex,'U'
	db	circumflex,' '

	db	umlaut,	'a'
	db	umlaut,	'e'
	db	umlaut,	'i'
	db	umlaut,	'o'
	db	umlaut,	'u'
	db	umlaut,	'y'
	db	umlaut,	'A'
	db	umlaut,	'E'
	db	umlaut,	'I'
	db	umlaut,	'O'
	db	umlaut,	'U'
	db	umlaut,	' '

	db	tilde,	'a'
	db	tilde,	'o'
	db	tilde,	'n'
	db	tilde,	'A'
	db	tilde,	'O'
	db	tilde,	'N'
	db	tilde,	' '

NDEADLIST	equ	(($-pkbdsgDeadTrans) shr 1)-1

pkbdsgDeadList	label byte

	db	0E0h
	db	0E8h
	db	0ECh
	db	0F2h
	db	0F9h
	db	0C0h
	db	0C8h
	db	0CCh
	db	0D2h
	db	0D9h
	db	grave

	db	0E1h
	db	0E9h
	db	0EDh
	db	0F3h
	db	0FAh
	db	0FDh
	db	0C1h
	db	0C9h
	db	0CDh
	db	0D3h
	db	0DAh
	db	0DDh
	db	acute

	db	0E2h
	db	0EAh
	db	0EEh
	db	0F4h
	db	0FBh
	db	0C2h
	db	0CAh
	db	0CEh
	db	0D4h
	db	0DBh
	db	circumflex

	db	0E4h
	db	0EBh
	db	0EFh
	db	0F6h
	db	0FCh
	db	0FFh
	db	0C4h
	db	0CBh
	db	0CFh
	db	0D6h
	db	0DCh
	db	umlaut

	db	0e3h
	db	0f5h
	db	0F1h
	db	0c3h
	db	0d5h
	db	0D1h
	db	tilde 

;---------------------------------------------------------------------------

DATASIZE equ $-pkbdsgStart

sEnd	DATA

;---------------------------------------------------------------------------

sBegin	CODE	; Beginning of code segment
assumes CS,CODE
assumes DS,DATA
sEnd	CODE

end
