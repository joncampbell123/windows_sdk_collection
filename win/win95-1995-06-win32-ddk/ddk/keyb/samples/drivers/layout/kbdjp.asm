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
; Kbdjp.asm JAPAN 101 KEYBOARD LAYOUT
;
;*****************************************************************************

VKFUN = 0

.xlist
include cmacros.inc

include ..\vkwin.inc
include ..\vkoem.inc
include ..\lcid.inc
.list

if1
%out
%out   kbdjp101.asm  (Microsoft)
endif


;*****************************************************************************

sBegin   DATA

Header  label byte
        public Header
        db      'DS'
        dd      00000411H
        dw      1
        dw      offset LayoutData, 0            ;; dd address
        dw      offset LayoutEnd - offset LayoutData
        dd      0
        dw      0
        dd      0                               ; pUnicode
        dd      0                               ; nUnicode
LayoutData      label byte

EngLayout   label byte
        dw              CAPSNORMAL              ; FLAGS
        dw              NSTATEKEYS              ; NSTATE_KEYS
        dw              ENG_NSTATES             ; NSTATES
        dw              0                       ; NUM_DEAD
        dw              0                       ; NUM_LIG
        KBDOFFSET       pEngStates              ; STATE_LIST
        KBDOFFSET       pEngToAscStates         ; TOASC_STATES
        KBDOFFSET       pEngToAscStateTables    ; STATETABLES
        KBDOFFSET       pEngToAscVKeyList       ; VKEY_LISTS
        KBDOFFSET       pEngToAscVKeyListLens   ; VKEY_LIST_LENS
        KBDOFFSET       pEngVKShiftStates       ; VK_STATES
        KBDOFFSET       pEngScanToIdx           ; SCANTOIDX
        KBDOFFSET       pEngVKeyToIdx           ; VKEYTOIDX
        dw              SCANSIZE                ; SCAN_SIZE
        KBDOFFSET       EngVKeyToAnsi           ; VKEYTOANSI
        dw              0                       ; DEAD_KEYS
        dw              0                       ; DEAD_KEYTRANS
        dw              0                       ; LIG_KEYS
        KBDOFFSET       pEngCapsBits            ; CAPS BIT MAP
        KBDOFFSET       KanaNormal              ; KANA_NORMAL

;---------------------------------------------------------------------------
; CapsLock Vkey Bit Enable Table - CAPSNORMAL flag is required
;
pEngCapsBits label byte                         ;used with CAPSNORMAL

        db      0,0,0,0,0,0,0,0                         ; Controls & Numerals
        db      0FEh,0FFh,0FFh,07h,0,0,0,0      ; VK_A -> VK_Z & Numpad & Function Keys
        db      0,0,0,0,0,0,0,0                         ; Function Keys & OEM Keys
        db      0,0,0,0,0,0,0,0                         ; OEM Keys, SVK_A -> SVK_Z, OEM Controls

;---------------------------------------------------------------------------

pEngStates  label byte
   ;
   ; this table gives the states used. The values are in order, so that
   ; everywhere else bits can be used instead.
   ;
   ; DO NOT CHANGE THE ORDER
   ;
      db    VK_MENU,    080H  ; 0x12, down,    gives state = 1
      db    VK_SHIFT,   080H  ; 0x10, down,    gives state = 2
      db    VK_CONTROL, 080H  ;       down,    gives state = 8, will fail

NSTATEKEYS equ ($ - pEngStates) shr 1

        db      0               ; nothing - dont declare!
pEngToAscStates   label byte
   ;
   ; note: this table MUST be preceded by a zero
   ; note: the first three MUST come first in this order
   ;
        db      1               ; Alt
        db      2               ; Shift
        db      3               ; Alt+Shift
        db      4               ; control
ENG_NSTATES equ $-pEngToAscStates

pEngVKShiftStates label byte
        db      0               ; unshifted
        db      3               ; alt (normal - wont get here!)
        db      1               ; shifted
        db      3               ; alt+shift (wont get here1)
        db      2               ; control

pEngToAscStateTables label word
   ;
   ; the FIRST table here is the no-shift table. repnz scasb leaves di
   ; pointing to the address after the one found.
   ;
        KBDOFFSET       EngToAscNormal
        KBDOFFSET       EngToAscAlt
        KBDOFFSET       EngToAscShift
        KBDOFFSET       EngToAscAltShift
        KBDOFFSET       EngToAscControl

pEngToAscVKeyList label word
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngVKeyToIdx
        KBDOFFSET       pEngCtrlVkKeyToIdx

pEngToAscVKeyListLens   label byte
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_LIST_SIZE
   db VKEY_CTRL_LIST_SIZE

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

pEngScanToIdx label byte
   ;
   ; scan code to index into the state tables
   ;
   db    029H                                            ; tilde
   db    002H, 003H, 004H, 005H, 006H, 007H, 008H, 009H  ; number row
   db    00aH, 00BH, 00Ch, 00Dh
   db    010H, 011h, 012h, 013h, 014h, 015h, 016h  ; qwerty ...
   db    017h, 018h, 019h, 01ah, 01bh, 02bh

   db    01eH, 01fh, 020h, 021h, 022h, 023h, 024h, 025h  ; asdf...
   db    026h, 027h, 028h
   db    02ch, 02dh, 02eh, 02fh, 030h, 031h, 032H, 033h  ; zxc ..
   db    034h, 035h

   db    56H
SCANSIZE   equ   $ - pEngScanToIdx

;*****************************************************************************

pEngCtrlVkKeyToIdx label byte
        db 0DBH, 0DDH
VKEY_CTRL_LIST_SIZE equ $-pEngCtrlVkKeyToIdx

labelB   EngToAscControl
        ;
        ; control characters
        ;
        db 01BH, 01DH

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

EngToAscAltShift  label byte
EngToAscShift     label byte
        ;
        ; shifted characters
        ;
        db '~!@#$%^&*()_+'
        db 'QWERTYUIOP{}|ASDFGHJKL:"'
        db 'ZXCVBNM<>?'
        db '|'

EngVKeyToAnsi     label byte
   db    '`1234567890-='
   db    'QWERTYUIOP[]\ASDFGHJKL;',39
   db    'ZXCVBNM,./'
   db    '\'

KanaNormal        label byte ; Kana_KeyCode
        db      0dbh, 0c7h, 0cch, 0b1h, 0b3h, 0b4h, 0b5h, 0d4h, 0d5h
        db      0d6h, 0dch, 0ceh, 0cdh
        db      0c0h, 0c3h, 0b2h, 0bdh, 0b6h, 0ddh, 0c5h, 0c6h, 0d7h
        db      0beh, 0deh, 0dfh, 0d1h
        db      0c1h, 0c4h, 0bch, 0cah, 0b7h, 0b8h, 0cfh, 0c9h, 0d8h
        db      0dah, 0b9h
        db      0c2h, 0bbh, 0bfh, 0cbh, 0bah, 0d0h, 0d3h, 0c8h, 0d9h
        db      0d2h, 0dbh

; This table must follow KanaNormal table and adjacent to each other.
KanaShift       label byte ; Kana_KeyCode

        db      0dbh, 0c7h, 0cch, 0a7h, 0a9h, 0aah, 0abh, 0ach, 0adh
        db      0aeh, 0a6h, 0b0h, 0cdh
        db      0c0h, 0c3h, 0a8h, 0bdh, 0b6h, 0ddh, 0c5h, 0c6h, 0d7h
        db      0beh, 0a2h, 0a3h, 0d1h
        db      0c1h, 0c4h, 0bch, 0cah, 0b7h, 0b8h, 0cfh, 0c9h, 0d8h
        db      0dah, 0b9h
        db      0afh, 0bbh, 0bfh, 0cbh, 0bah, 0d0h, 0d3h, 0a4h, 0a1h
        db      0a5h, 0dbh

LayoutEnd       label byte

sEnd     DATA

;*****************************************************************************

sBegin   CODE        ; Beginning of code segment

assumes CS,CODE
assumes DS,DATA

sEnd     CODE

end
