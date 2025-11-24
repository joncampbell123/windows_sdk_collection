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
;	kbdsf.asm SWISS(FRENCH)
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


;---------------------------------------------------------------------------

kbdsfLayout	label byte
public kbdsfLayout

	db	'DS'
	dd	0100cH				; Default LCID
	dw	0				; Version
	dd	dataOFFSET pkbdsfStart		; Start of Data
	dW	DATASIZE				; Size of Data
	dd	0				; pDibData
	dw	0				; DIBSIZE
	dd	0				; pUnicode
	dd	0				; nUnicode
LayoutData	label byte

; pkbdsfDibData label word

; DIBSIZE EQU $ - pkbdsfDibData

pkbdsfStart label word
	dw		ALTGRUSED or CAPSNORMAL
	dw		NSTATEKEYS		; NSTATE_KEYS
	dw		kbdsf_NSTATES		; NSTATES
	dw		5			; NUM_DEAD
	dw		0			; NUM_LIG
        KBDOFFSET       pkbdsfStates		; STATE_LIST
        KBDOFFSET       pkbdsfToAscStates	; TOASC_STATES
        KBDOFFSET       pkbdsfToAscStateTables	; STATETABLES
        KBDOFFSET       pkbdsfToAscVkeyList	; VKEY_LISTS
        KBDOFFSET       pkbdsfToAscVKeyListLens	; VKEY_LIST_LENS
        KBDOFFSET       pkbdsfVKShiftStates	; VK_STATES
        KBDOFFSET       pkbdsfScanToIdx		; SCANTOIDX
        KBDOFFSET       pkbdsfVKeyToIdx		; VKEYTOIDX
	dw		SCANSIZE		; SCAN_SIZE
        KBDOFFSET       kbdsfVKeyToAnsi		; VKEYTOANSI
        KBDOFFSET       pkbdsfDeadKeyTable	; DEAD_KEYS
        KBDOFFSET       pkbdsfDeadTrans		; DEAD_KEYTRANS
	dw		0			; LIG_KEYS
	KBDOFFSET	pEngCapsBits			; CAPS

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls
;---------------------------------------------------------------------------

pkbdsfStates	label byte
	db	VK_MENU,    080H
	db	VK_SHIFT,   080H
	db	VK_CONTROL, 080H

NSTATEKEYS equ ($ - pkbdsfStates) shr 1

	db	0
pkbdsfToAscStates	label byte
	db	1		; Alt
	db	2		; Shift
	db	3		; Alt+Shift
	db	4		; Control
	db	6		; Control+Shift
	db	5		; AltGr
	db	7		; AltGr+Shift

kbdsf_NSTATES equ $-pkbdsfToAscStates

pkbdsfVKShiftStates label byte
	db	0		; unshifted
	db	0		; alt
	db	1		; shifted
	db	1		; alt+shift
	db	2		; control
	db	3		; control+shift
	db	6		; AltGr
	db	7		; AltGr+Shift

pkbdsfToAscStateTables label word
        KBDOFFSET       kbdsfToAscNormal
        KBDOFFSET       kbdsfToAscAlt
        KBDOFFSET       kbdsfToAscShift
        KBDOFFSET       kbdsfToAscAltShift
        KBDOFFSET       kbdsfToAscControl
        KBDOFFSET       kbdsfToAscCtrlShift
        KBDOFFSET       kbdsfToAscAltGr
        KBDOFFSET       kbdsfToAscAltGrShift

pkbdsfToAscVKeyList label word
        KBDOFFSET       pkbdsfVKeyToIdx
        KBDOFFSET       pkbdsfVKeyToIdx
        KBDOFFSET       pkbdsfVKeyToIdx
        KBDOFFSET       pkbdsfVKeyToIdx
        KBDOFFSET       pkbdsfControlVKeyToIdx
        KBDOFFSET       pkbdsfCtrlShiftVKeyToIdx
        KBDOFFSET       pkbdsfAltGrVKeyToIdx
        KBDOFFSET       pkbdsfAltGrShiftVKeyToIdx

pkbdsfToAscVKeyListLens	label byte
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_CTRL_LIST_SIZE
	db VKEY_CTRL_SHIFT_SIZE
	db VKEY_ALTGR_LIST_SIZE
	db VKEY_ALTGR_SHIFT_SIZE

;---------------------------------------------------------------------------
pkbdsfScanToIdx label byte
	db 41, 2, 3, 4, 5, 6, 7, 8
	db 9, 10, 11, 12, 13, 16, 17, 18
	db 19, 20, 21, 22, 23, 24, 25, 26
	db 27, 43, 30, 31, 32, 33, 34, 35
	db 36, 37, 38, 39, 40, 44, 45, 46
	db 47, 48, 49, 50, 51, 52, 53
	db 56H
SCANSIZE	equ	$ - pkbdsfScanToIdx

;---------------------------------------------------------------------------

pkbdsfVKeyToIdx label byte
;Virtual Keys
	db VK_OEM_2, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7
	db VK_8, VK_9, VK_0, VK_OEM_4, VK_OEM_6, VK_Q, VK_W, VK_E
	db VK_R, VK_T, VK_Z, VK_U, VK_I, VK_O, VK_P, VK_OEM_1
	db VK_OEM_3, VK_OEM_8, VK_A, VK_S, VK_D, VK_F, VK_G, VK_H
	db VK_J, VK_K, VK_L, VK_OEM_7, VK_OEM_5, VK_Y, VK_X, VK_C
	db VK_V, VK_B, VK_N, VK_M, VK_OEM_COMMA, VK_OEM_PERIOD, VK_OEM_MINUS
	db VK_OEM_102	
VKEY_LIST_SIZE	equ	$ - pkbdsfVKeyToIdx

;---------------------------------------------------------------------------

kbdsfToAscControl label byte
	db 01FH, 01DH, 01BH, 01CH, 01CH
pkbdsfControlVKeyToIdx label byte
	db 0BDH, 0C0H, 0BAH, 0DFH, 0E2H
VKEY_CTRL_LIST_SIZE	equ	$ - pkbdsfControlVKeyToIdx


kbdsfToAscCtrlShift label byte
	db 01EH
pkbdsfCtrlShiftVKeyToIdx label byte
	db 036H
VKEY_CTRL_SHIFT_SIZE	equ	$ - pkbdsfCtrlShiftVKeyToIdx

;---------------------------------------------------------------------------

kbdsfToAscAltGrShift label byte
	db 0B4H, 07EH
pkbdsfAltGrShiftVKeyToIdx label byte
	db 0DBH, 0DDH
VKEY_ALTGR_SHIFT_SIZE	equ	$ - pkbdsfAltGrShiftVKeyToIdx

;---------------------------------------------------------------------------

kbdsfToAscAltGr label byte

	db	0a6h
	db	'@'
	db	'#'
	db	0b0h
	db	0a7h
	db	0ach
	db	'|'
	db	0a2h
	db	'['
	db	']'
	db	'{'
	db	'}'
	db	tilde
	db	acute
	db	'\'

VKEY_ALTGR_LIST_SIZE equ $-kbdsfToAscAltGr

pkbdsfAltGrVKeyToIdx label byte

	db	VK_1      
	db	VK_2      
	db	VK_3      
	db	VK_4      
	db	VK_5      
	db	VK_6      
	db	VK_7      
	db	VK_8      
	db	VK_OEM_1  
	db	VK_OEM_3  
	db	VK_OEM_5  
	db	VK_OEM_8  
	db	VK_OEM_6
	db	VK_OEM_4
	db	VK_OEM_102

;---------------------------------------------------------------------------
kbdsfToAscAlt		label byte
;Alt Key State'

kbdsfToAscNormal	label byte
;Unshifted Keys'
	db 'ß1234567'
	db '890''^qwe'
	db 'rtzuiopË'
	db umlaut,'$asdfgh'
	db 'jklÈ‡yxc'
	db 'vbnm,.-'
	db '<'
;---------------------------------------------------------------------------
kbdsfToAscAltShift	label byte
;Alt Shift State'

kbdsfToAscShift		label byte
;Shift State'
	db '∞+"*Á%&/'
	db '()=?`QWE'
	db 'RTZUIOP¸'
	db '!£ASDFGH'
	db 'JKLˆ‰YXC'
	db 'VBNM;:_'
	db '>'
;---------------------------------------------------------------------------

kbdsfVKeyToAnsi	label byte
;ANSII
	db 'ß1234567'
	db '890''^QWE'
	db 'RTZUIOPË'
	db umlaut,'$ASDFGH'
	db 'JKLÈ‡YXC'
	db 'VBNM,.-'
	db '<'
;---------------------------------------------------------------------------

pkbdsfDeadKeyTable	label byte
	Db	GRAVE	   
	Db	ACUTE	   
	Db	CIRCUMFLEX 
	Db	UMLAUT	   
	Db	TILDE	   

pkbdsfDeadTrans	label byte
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

NDEADLIST	equ	(($-pkbdsfDeadTrans) shr 1)-1

pkbdsfDeadList	label byte
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
	db	060h

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
	db	0B4h

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
	db	'^' 

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
	db	'~' 

DATASIZE equ $-pkbdsfStart

;---------------------------------------------------------------------------


sEnd	DATA

;---------------------------------------------------------------------------


sBegin	CODE	; Beginning of code segment
assumes CS,CODE
assumes DS,DATA
sEnd	CODE

end
