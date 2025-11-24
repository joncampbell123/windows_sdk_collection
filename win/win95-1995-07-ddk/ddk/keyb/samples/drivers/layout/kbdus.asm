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

        page    ,132
;*****************************************************************************
;
; LCIDENG - English (US) table
;
; note, we use Ctrl-Shift here, but NOT for AltGr
;
;*****************************************************************************

VKFUN = 0

.xlist
include	cmacros.inc

include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list

if1
%out
%out   lcidEng.asm  (Microsoft)
endif

;*****************************************************************************

sBegin   DATA

Header	label byte
	public Header
	db	'DS'
	dd	00000409H
	dw	1
	dw	offset LayoutData, 0		;; dd address
	dw	offset LayoutEnd - offset LayoutData
	dd	0
	dw	0
	dd	0				; pUnicode
	dd	0				; nUnicode

;*****************************************************************************

LayoutData	label byte

EngLayout   label byte
	dw			CAPSNORMAL				; FLAGS
	dw			NSTATEKEYS				; NSTATE_KEYS 
	dw			ENG_NSTATES				; NSTATES 
	dw			0						; NUM_DEAD
	dw			0						; NUM_LIG
	KBDOFFSET	pEngStates				; STATE_LIST 
	KBDOFFSET	pEngToAscStates			; TOASC_STATES 
	KBDOFFSET	pEngToAscStateTables	; STATETABLES 
	KBDOFFSET	pEngToAscVKeyList		; VKEY_LISTS 
	KBDOFFSET	pEngToAscVKeyListLens	; VKEY_LIST_LENS
	KBDOFFSET	pEngVKShiftStates		; VK_STATES
	KBDOFFSET	pEngScanToIdx			; SCANTOIDX
	KBDOFFSET	pEngVKeyToIdx			; VKEYTOIDX
	dw			SCANSIZE				; SCAN_SIZE
	KBDOFFSET	EngVKeyToAnsi			; VKEYTOANSI
	dw		0							; DEAD_KEYS
	dw		0							; DEAD_KEYTRANS
	dw		0							; LIG_KEYS
	KBDOFFSET	pEngCapsBits			; CAPS

;*****************************************************************************

pEngStates  label byte
   ;
   ; this table gives the states used. The values are in order, so that 
   ; everywhere else bits can be used instead.
   ;
   ; DO NOT CHANGE THE ORDER
   ;
	db    VK_MENU,    080H  ; 0x12, down,    gives state = 1
	db    VK_SHIFT,   080H  ; 0x10, down,    gives state = 2
	db    VK_CONTROL, 080H  ;       down,    gives state = 8

NSTATEKEYS equ ($ - pEngStates) shr 1

	db	0		; nothing - dont declare!
pEngToAscStates   label byte
   ;
   ; note: this table MUST be preceded by a zero
   ; note: the first three MUST come first in this order
   ;
	db	1		; Alt       
	db	2		; Shift     
	db	3		; Alt+Shift 
	db	4		; control
	db	6		; contrl+shift (NOT AltGr)

ENG_NSTATES equ $-pEngToAscStates

pEngVKShiftStates label byte
	db	0		; unshifted
	db	0		; alt (normal - wont get here!)
	db	1		; shifted
	db	1		; alt+shift (wont get here1)
	db	2		; control
	db	4		; control+shift (?)

;*****************************************************************************
;
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte				;used with CAPSNORMAL

	db	0,0,0,0,0,0,0,0				; Controls & Numerals
	db	0FEh,0FFh,0FFh,07h,0,0,0,0 	; VK_A -> VK_Z & Numpad & Function Keys
	db	0,0,0,0,0,0,0,0				; Function Keys & OEM Keys
	db	0,0,0,0,0,0,0,0				; OEM Keys, SVK_A -> SVK_Z, OEM Controls

;*****************************************************************************

pEngToAscStateTables label word
   ;
   ; the FIRST table here is the no-shift table. repnz scasb leaves di 
   ; pointing to the address after the one found.
   ;
	KBDOFFSET	EngToAscNormal
	KBDOFFSET	EngToAscAlt
	KBDOFFSET	EngToAscShift
	KBDOFFSET	EngToAscAltShift
	KBDOFFSET	EngToAscControl
	KBDOFFSET	EngToAscControlShift

pEngToAscVKeyList label word
	KBDOFFSET	pEngVKeyToIdx
	KBDOFFSET	pEngVKeyToIdx
	KBDOFFSET	pEngVKeyToIdx
	KBDOFFSET	pEngVKeyToIdx
	KBDOFFSET	pEngCtrlVkKeyToIdx
	KBDOFFSET	pEngCtrlShiftVkKeyToIdx

pEngToAscVKeyListLens   label byte
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_LIST_SIZE
	db VKEY_CTRL_LIST_SIZE
	db VKEY_CTRLSHIFT_LIST_SIZE

;*****************************************************************************

pEngVKeyToIdx label byte
	;
	; vkey index into the state tables
	;
	db 0c0H, '1234567890', 0bdH, 0bbH
	db 'QWERTYUIOP', 0dbh, 0ddh, 0dch
	db 'ASDFGHJKL', 0bah, 0deh
	db 'ZXCVBNM', 0bch, 0beh, 0bfh
	db VK_OEM_102

VKEY_LIST_SIZE equ $-pEngVKeyToIdx

;*****************************************************************************

pEngScanToIdx label byte
	;
	; scan code to index into the state tables
	;
	db	029H, 002H, 003H, 004H, 005H, 006H
	db	007H, 008H, 009H, 00AH, 00BH, 00CH
	db	00DH, 010H, 011H, 012H, 013H, 014H
	db	015H, 016H, 017H, 018H, 019H, 01AH
	db	01BH, 02BH, 01EH, 01FH, 020H, 021H
	db	022H, 023H, 024H, 025H, 026H, 027H
	db	028H, 02CH, 02DH, 02EH, 02FH, 030H
	db	031H, 032H, 033H, 034H, 035H, 056H	

SCANSIZE   equ   $ - pEngScanToIdx

;*****************************************************************************

pEngCtrlVkKeyToIdx label byte
	;
	; companion vkeys for the control characters
	;
	db 0DBH, 0DCH, 0DDH, 0E2H

VKEY_CTRL_LIST_SIZE equ $-pEngCtrlVkKeyToIdx	;# of control characters

labelB   EngToAscControl
	;
	; control characters
	;
	db 01BH, 01CH, 01DH, 01CH

;*****************************************************************************

pEngCtrlShiftVkKeyToIdx label byte
	;
	; companion vkeys for the control+shift characters
	;
	db 036H, 0BDH

VKEY_CTRLSHIFT_LIST_SIZE equ $-pEngCtrlShiftVkKeyToIdx	;# of CTRL+SHIFT chars

labelB   EngToAscControlShift
	;
	; control shifted characters
	;
	db 01Eh, 01FH

;*****************************************************************************

labelB   EngToAscAlt
labelB   EngToAscNormal
	;
	; unshifted characters
	;
	db      '`1234567890-='
	db      'qwertyuiop[]\asdfghjkl;', 027H
	db      'zxcvbnm,./'
	db '\'

;*****************************************************************************

EngToAscAltShift  label byte
EngToAscShift     label byte
	;
	; shifted characters
	;
	db '~!@#$%^&*()_+'
	db 'QWERTYUIOP{}|ASDFGHJKL:"'
	db 'ZXCVBNM<>?'
	db '|'

;*****************************************************************************

EngVKeyToAnsi     label byte
	;
	; ANSI characters
	;
	db	060H, 031H, 032H, 033H, 034H, 035H
	db	036H, 037H, 038H, 039H, 030H, 02DH
	db	03DH, 051H, 057H, 045H, 052H, 054H
	db	059H, 055H, 049H, 04FH, 050H, 05BH
	db	05DH, 05CH, 041H, 053H, 044H, 046H
	db	047H, 048H, 04AH, 04BH, 04CH, 03BH
	db	027H, 05AH, 058H, 043H, 056H, 042H
	db	04EH, 04DH, 02CH, 02EH, 02FH, 05CH 

;*****************************************************************************

LayoutEnd	label byte

sEnd     DATA

;*****************************************************************************

sBegin   CODE        ; Beginning of code segment

assumes CS,CODE
assumes DS,DATA

sEnd     CODE

end

