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

;*** KEYBOARD.INC **********************************************************
;
if1
%out  KEYBOARD.INC 3.00
endif
;***************************************************************************
;                                                                          *
;   Header file for IBM/PC keyboard device driver.  Contains all machine   *
;   specific constant and data structure definitions.                      *
;                                                                          *
;***************************************************************************
.286
DOS5=1
.xlist
include	cmacros.inc
include	windefs.inc
include windows.inc
include lcid.inc
.list

ack_port equ	20h	; 8259 acknowledge port
eoi	 equ	20h	; 8259 end of interrupt (Specific)
eoiAT	equ	61h	; end of interrupt for AT-like system (for IRQ 1)

timer_port  equ	40h	; 8253 Counter Timer
kb_data	    equ	60h
kb_ctl	    equ	61h

; hardware interrupt vector number
;
vector	    equ	09h

;***************************************************************************
;
; for writing to keyboard output port (Olivetti M24, etc.), need to check status
;
;***************************************************************************

kb_status	equ	64h	; status port -- bit 1: ok to write

;***************************************************************************
; Definition of BIOS RAM data area
;***************************************************************************

RAMBIOS	SEGMENT	AT 40H

	org 17h
kb_flag label byte
	org 18h
kb_flag_1 label byte
	org 1ah
buffer_head label word
	org 1ch
buffer_tail label word
	org 1eh
kb_buffer label word

	org 49h
CrtMode label byte		; bios data area for screen mode

	org 72h
fReset	label byte		; bios area for reset flags

	org  96h
KB_type		label BYTE  ; contains RT keyboard identification

RAMBIOS	ENDS



;***************************************************************************
; bits in kb_flag and kb_flag_1
;***************************************************************************


; Flag bits in kb_flag

fCaps		equ	01000000b	; caps (or shift-) lock
fNum		equ	00100000b	; numlock
fScroll		equ	00010000b	; scroll lock
fAlt		equ	00001000b	; ALT key
fCtrl		equ	00000100b	; Control key
fLshift		equ	00000010b	; left shift key
fRshift		equ	00000001b	; right shift key
fShift		equ	fLshift+fRshift	; either or both shift keys

; Flag bits in the word at kb_flag (kb_flag and kb_flag_1)
; (need to set/reset BOTH for Alt and Ctrl keys)

fAltW		equ   1000001000b	; ALT key
fCtrlW		equ   0100000100b	; Control key

; Flag bits in kb_flag_1
fHold		equ	00001000b	; Pause


;***************************************************************************
; Olivetti/AT&T keyboard identifiers in the OliType byte.  Valid only
; for Olivetti-protocol keyboards.  This byte is 0 for others.
; This is used in the 3.00 driver only as a flag (in WIN.INI) for
; Olivetti/AT&T keyboard type.  Only the first 3 types are used
; on 8086 systems.  The KB302 has an extra LED.
;***************************************************************************

KB83		equ	2	; M24 83-key keyboard
KB302		equ	4	; AT&T 6300 PLUS (HAS EXTRA LED TO HANDLE)
KB102		equ	1	; M24 102-key keyboard (84-key XT)
				;
KB86		equ	10h	; M28 keyboard with SYSREQ key
KBM28		equ	10h	; .. same thing
KB102RT		equ	40h	; 101-102 extended keyboard (RONCO)(RT)
KBXT83		equ	42h	; XT 83 key keyboard on M28


;***************************************************************************
; .. and special bit values in KeyType for the RT (101/102) keyboard.
;***************************************************************************

kbShiftLock	equ	40h	; set if Shiftlock instead of caps lock
kbAltGr		equ	08h	; AltGr = CTRL-ALT -- all but USA


;***************************************************************************
; System ID byte or word values (AT F000:FFFE)
;***************************************************************************

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

IDM240		equ	0FBh	;
IDM240_2	equ	0B5h

; IBM systems

IBMPCID		equ	0FFh	; IBM PC, and M22/1020
IBMPCID_2	equ	0FFh	; IBM PC, NOT M22

IBMXTID		equ	0FEh	; IBM PC XT, and M19/1030
IBMPCID_2	equ	0FFh	; IBM PC, NOT 1030

IBMATID		equ	0FCh	; IBM PC AT, and Oli. M28, etc.
IBMATID_2	equ	0	; IBM PC AT

; PS/2 Model 30		FA	00
; PS/2 Model 35		FA	AF
; PS/2 Model 50		FC	04
; PS/2 Model 60		FC	05
; PS/2 Model 80		F8	00

IBMPS2ID	equ	0FAh	; IBM PS/2 mod 30, 35
				; mods 50, 60 have AT ID!
IBMPS280ID	equ	0F8h	; IBM PS/2 mod 80

;***************************************************************************
; macros for allowing the virtualization of I/O in protect mode Windows
;***************************************************************************

;	extrn	__VIRTUALIZE_IO :abs
	extrn	__ROMBIOS	:abs
