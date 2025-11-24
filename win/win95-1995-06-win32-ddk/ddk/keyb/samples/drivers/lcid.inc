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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; File:  lcid.inc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

CAPSNORMAL              equ     2               ; dont change this!
ALTGRUSED               equ     8               ; dont change this! kbAltGr
SHIFTLOCKUSED           equ     40H             ; dont change this! kbShiftLock
DEADCOMBOS              equ     20H

INQEX_KBDINFO           equ     1
INQEX_NLCID             equ     2
INQEX_LPLCID            equ     3
INQEX_KEYBOARDTYPE      equ     4
INQEX_KEYBOARDSUBTYPE   equ     5
INQEX_NFUNCKEYS         equ     6
INQEX_SETBASELOCALE     equ     7
INQEX_SETSYSLOCALE      equ     8

;; standard diacritics
;; -------------------
;; These equates imply that diacritic marks used as deadkeys are required to
;; be implemented in the same position across all code pages. However, these 
;; may be replaced by other characters if the diacritics are not used.
;;
umlaut          equ     0a8h	; available on US intl KBD
acute           equ     0b4h	; available on US intl KBD
grave           equ     060h	; '`'
circumflex      equ     05eH    ; '^' preferred over 088H
tilde           equ     07eH    ; '~' preferred over 089H
cedilla         equ     0b8h	; 
ring            equ     0B0H    ; (176) degree sign: icelandic deadkey
beta_g          equ     223     ; german char, looks like a beta
hash            equ     023h    ; # sign
middledot       equ     0B7h	; middle dot

hachek			equ		0A1h	; inverted circumflex (concave)
breve			equ		0A2h	; shallow hachek (concave) shape
ogonek			equ		0B2h	; reversed cidella shape
dblacute		equ		0BDh	; double acute
overdot			equ		0FFh	; overdot diacritic
apostrophe		equ		027h	; used as a hook diacritic
dblquote		equ		022h	; used as a double hook diacritic
overcircle		equ		ring	; same as ring diacritic

KBDOFFSET macro n
        dw ((dataOFFSET n) - (dataOFFSET LayoutData))
        endm

;; Header Structure equates
;; ------------------------
;;;;DEFAULT_CHARSET   equ
LCID_FLAGS        equ   0
NSTATE_KEYS       equ   2
NSTATES           equ   4
NUM_DEAD          equ   6
NUM_LIG           equ   8
STATE_LIST        equ   10
TOASC_STATES      equ   12
STATETABLES       equ   14
VKEY_LISTS        equ   16
VKEY_LIST_LENS    equ   18
VK_STATES         equ   20
SCANTOIDX         equ   22
VKEYTOIDX         equ   24
SCAN_SIZE         equ   26    ; this should be the same as the CapsLock table
VKEYTOANSI        equ   28
DEAD_KEYS         equ   30
DEAD_KEYTRANS     equ   32
LIG_KEYS          equ   34
CAPSTABLE         equ   36
;; for Kana Key use only.
ifdef JAPAN
KANA_NORMAL       equ   38
endif

; Keyboard Manager structure
LCIDIB  struc
        LocaleID  dd    0
        pKeyData  dw    0
;**;    selData   dw    0
LCIDIB  ends

sizeofLCIDIB      equ     size LCIDIB

DeadKeyState  struc
	dksChar db 0
	dksVkey db 0
	dksState db 0
	dksPudding db 0
DeadKeyState ends

KBDHEADER struc
        kbdhMagic       dw      0
        kbdhLocale      dd      0
        kbdhVersion     dw      0
        kbdhLayout      dd      0
        kbdhSize        dw      0
        kbdhIcon        dd      0
        kbdhIconSize    dw      0
        kbdhUnicode     dd      0
        kbdhUniSize     dd      0
KBDHEADER ends

sizeofKBDHEADER equ (size KBDHEADER)

LCIDBUFFERS       equ     16
USLCID            equ     00000409h
US_KEYBOARD_TABLE equ     1000


LMHtoP macro r1,r2              ;; Local Movable Handle to pointer

;;
;; example: LMHtoP   bx will put the pointer of the handle in bx
;;
;; CAVEAT: DONT CALL ANY KERNEL/GDI/ANYONE ELSE FUNCTION WHILST THE POINTER
;; IS DEREFERNECED THUS.
;;
ifnb <r2>
        mov     r1,[r2]
else
        mov     r1,[r1]
endif
endm

LMHtoPES macro r1,r2             ;; Local Movable Handle to ptr, deref vi ES
ifnb <r2>
        mov     r1,es:[r2]
else
        mov     r1,es:[r1]
endif
endm

LMPsize macro r1,r2             ;; Local Movable pointer size
        mov     r1,-4[r2]
endm

dwptr   equ     dword ptr
wptr    equ     word ptr
bptr    equ     byte ptr

;***** Registration Database Support *************
; return codes from Registration functions
ERROR_SUCCESS                   EQU     0
ERROR_BADDB                     EQU     1
ERROR_REGKEY_NOTFOUND           EQU     2
ERROR_REGKEY_INVALIDKEY         EQU     2
ERROR_CANNOT_OPEN_REGFILE       EQU     3
ERROR_REGFILE_READERROR         EQU     4
ERROR_REGFILE_WRITEERROR        EQU     5
ERROR_REG_BUFFER_OVERFLOW       EQU     6
ERROR_INSUFFICIENT_MEMORY       EQU     6
ERROR_INVALID_PARAMETER         EQU     7
ERROR_REGKEY_CANNOT_DELETE      EQU     8
ERROR_VALUE_NOT_FOUND           EQU     9
REG_SZ                          EQU     0001H
REG_BINARY                      EQU     0003H

OFFSEL  struc
off     dw      ?
sel     dw      ?
OFFSEL  ends

