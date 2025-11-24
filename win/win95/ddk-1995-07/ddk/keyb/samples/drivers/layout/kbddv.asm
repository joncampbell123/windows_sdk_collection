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
;	kbddv.asm USA (Dvorak)
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

kbddvLayout	label byte
public kbddvLayout

	db	'DS'
	dd	00020409H			; Default LCID
	dw	0				; Version
	dd	dataOFFSET pkbddvStart		; Start of Data
	dW	DATASIZE				; Size of Data
	dd	0				; p(null)DibData
	dw	0				; DIBSIZE
	dd	0				; pUnicode
	dd	0				; nUnicode
LayoutData	label byte

; pkbddvDibData label word

; DIBSIZE EQU $ - pkbddvDibData

pkbddvStart label word
	dw		CAPSNORMAL		;
	dw		NSTATEKEYS		; NSTATE_KEYS
	dw		kbddv_NSTATES		; NSTATES
	dw		0			; NUM_DEAD
	dw		0			; NUM_LIG
	KBDOFFSET       pkbddvStates		; STATE_LIST
	KBDOFFSET       pkbddvToAscStates	; TOASC_STATES
	KBDOFFSET       pkbddvToAscStateTables	; STATETABLES
	KBDOFFSET       pkbddvToAscVkeyList	; VKEY_LISTS
	KBDOFFSET       pkbddvToAscVKeyListLens	; VKEY_LIST_LENS
	KBDOFFSET       pkbddvVKShiftStates	; VK_STATES
	KBDOFFSET       pkbddvScanToIdx		; SCANTOIDX
	KBDOFFSET       pkbddvVKeyToIdx		; VKEYTOIDX
	dw		SCANSIZE		; SCAN_SIZE
	KBDOFFSET       kbddvVKeyToAnsi		; VKEYTOANSI
	dw		0
	dw		0
	dw		0			; LIG_KEYS
	KBDOFFSET		pEngCapsBits		; CAPS

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls

;---------------------------------------------------------------------------

pkbddvStates	label byte
	db	VK_MENU,    080H
	db	VK_SHIFT,   080H
	db	VK_CONTROL, 080H

NSTATEKEYS equ ($ - pkbddvStates) shr 1

	db	0
pkbddvToAscStates	label byte
	db	1		; Alt
	db	2		; Shift
	db	3		; Alt+Shift
	db	4		; Ctrl
	db	6		; Ctrl+Shift

kbddv_NSTATES equ $-pkbddvToAscStates

pkbddvVKShiftStates label byte
	db	0		; unshifted
	db	3		; alt
	db	1		; shifted
	db	3		; alt+shift
	db	2		; Control
	db	3		; Control+Shift

pkbddvToAscStateTables label word
        KBDOFFSET       kbddvToAscNormal
        KBDOFFSET       kbddvToAscAlt
        KBDOFFSET       kbddvToAscShift
        KBDOFFSET       kbddvToAscAltShift
        KBDOFFSET       kbddvToAscCtrl
        KBDOFFSET       kbddvToAscCtrlShift

pkbddvToAscVKeyList label word
        KBDOFFSET       pkbddvVKeyToIdx
        KBDOFFSET       pkbddvVKeyToIdx
        KBDOFFSET       pkbddvVKeyToIdx
        KBDOFFSET       pkbddvVKeyToIdx
        KBDOFFSET       pkbddvVKeyCtrlToIdx
        KBDOFFSET       pkbddvVKeyCtrlShiftToIdx

pkbddvToAscVKeyListLens	label byte
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_CTRL_SIZE
	db VKEY_CTRLSHIFT_SIZE

pkbddvScanToIdx label byte
	db 41, 2, 3, 4, 5, 6, 7, 8
	db 9, 10, 11, 12, 13, 16, 17, 18
	db 19, 20, 21, 22, 23, 24, 25, 26
	db 27, 43, 30, 31, 32, 33, 34, 35
	db 36, 37, 38, 39, 40, 44, 45, 46
	db 47, 48, 49, 50, 51, 52, 53
	db 056H

SCANSIZE	equ	$ - pkbddvScanToIdx

;---------------------------------------------------------------------------

pkbddvVKeyToIdx label byte
;Virtual Keys
	db VK_OEM_3, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7
	db VK_8, VK_9, VK_0, VK_OEM_4, VK_OEM_6, VK_OEM_7, VK_OEM_COMMA
	db VK_OEM_PERIOD
	db VK_P, VK_Y, VK_F, VK_G, VK_C, VK_R, VK_L, VK_OEM_2
	db VK_OEM_PLUS, VK_OEM_5, VK_A, VK_O, VK_E, VK_U, VK_I, VK_D
	db VK_H, VK_T, VK_N, VK_S, VK_OEM_MINUS, VK_OEM_1, VK_Q, VK_J
	db VK_K, VK_X, VK_B, VK_M, VK_W, VK_V, VK_Z, VK_OEM_102

VKEY_LIST_SIZE	equ	$ - pkbddvVKeyToIdx

;---------------------------------------------------------------------------
kbddvToAscCtrl	label byte
	db 01BH, 01DH, 01CH, 01CH

pkbddvVkeyCtrlToIdx label byte
	db 0DBH, 0DDH, 0DCH, 0E2H

VKEY_CTRL_SIZE	equ	$ - pkbddvVkeyCtrlToIdx

;---------------------------------------------------------------------------

kbddvToAscCtrlShift label byte
	db 01EH, 01FH

pkbddvVkeyCtrlShiftToIdx label byte
	db 036H, 0BDH

VKEY_CTRLSHIFT_SIZE	equ	$ - pkbddvVkeyCtrlShiftToIdx

;---------------------------------------------------------------------------

kbddvToAscAlt		label byte
;Alt Key State

kbddvToAscNormal	label byte
;Unshifted Keys'
	db '`1234567'
	db '890[]'',.'
	db 'pyfgcrl/'
	db '=\aoeuid'
	db 'htns-;qj'
	db 'kxbmwvz'
	db '\'

;---------------------------------------------------------------------------

kbddvToAscAltShift	label byte
;Alt Shift State

kbddvToAscShift		label byte
;Shift State'
	db '~!@#$%^&'
	db '*(){}"<>'
	db 'PYFGCRL?'
	db '+|AOEUID'
	db 'HTNS_:QJ'
	db 'KXBMWVZ'
	db '|'

;---------------------------------------------------------------------------

kbddvVKeyToAnsi	label byte
;ANSI'
	db '`1234567'
	db '890[]'',.'
	db 'PYFGCRL/'
	db '=\AOEUID'
	db 'HTNS-;QJ'
	db 'KXBMWVZ'
	db '\'

;---------------------------------------------------------------------------


DATASIZE equ $-pkbddvStart



sEnd	DATA

;---------------------------------------------------------------------------


sBegin	CODE	; Beginning of code segment
assumes CS,CODE
assumes DS,DATA
sEnd	CODE

end
