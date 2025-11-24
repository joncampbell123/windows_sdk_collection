;***************************************************************************
;                                                                          *
;   Copyright (C) 1983,1984 by Microsoft Inc.                              *
;                                                                          *
;***************************************************************************

;***************************************************************************
;                                                                          *
;   Header file for IBM/PC keyboard device driver.  Contains all machine   *
;   specific constant and data structure definitions.                      *
;                                                                          *
;***************************************************************************
.xlist
include cmacros.inc
.list
include vkwin.inc
include vkoem.inc

;       Macros for disabling and restoring hardware interrupt enable flag
;
;       The LeaveCrit macro has been updated for the mask problem on
;       the 80286 processor.


EnterCrit   MACRO
        pushf
        cli
ENDM

LeaveCrit   MACRO
        POPFF
ENDM

POPFF   MACRO
        local   a
        jmp     $+3
a       label   near
        iret
        push    cs
        call    a
ENDM

; move table macro

movetbl MACRO count,src,dest
	mov   cx,count
	mov   si,initOFFSET src
	mov   di,dataOFFSET dest
	rep   movsb
	ENDM

; extended codes definition
;
circumflex  equ 05Eh    ; circumflex accent (^)
grave       equ 060h    ; grave accent
point_inv   equ 0A1h    ; inverted point (!)
b_pound     equ 0A3h    ; british pound
sun         equ 0A4h    ; sun ! (nobody knows the exact name)
section     equ 0A7h    ; section symbol
umlaut      equ 0A8h    ; umlaut
sca         equ 0AAh    ; a underlined superscript
lnot        equ 0ACh    ; logical not
degree      equ 0B0h    ; degree symbol
sc2         equ 0B2h    ; 2 superscript
acute       equ 0B4h    ; acute
mu          equ 0B5h    ; greek mu
middot      equ 0B7h    ; middle dot
sco         equ 0BAh    ; o undelined superscript
question_inv equ 0BFh   ; inverted question mark (?)
a_umlaut_uc equ 0C4h    ; A umlaut (upper case)
a_ring_uc   equ 0C5h    ; a ring (upper case)
ae_uc       equ 0C6h    ; ae (upper case)
c_cedilla_uc equ 0C7h   ; c cedilla (upper case)
n_tilde_uc  equ 0D1h    ; n tilde (upper case)
o_umlaut_uc equ 0D6h    ; O umlaut (upper case)
o_slash_uc  equ 0D8h    ; o slashed (upper case)
u_umlaut_uc equ 0DCh    ; U umlaut (upper case)
sharp_s     equ 0DFh    ; german sharp s (beta)
a_grave     equ 0E0h
a_umlaut_lc equ 0E4h    ; a umlaut (lower case)
a_ring_lc   equ 0E5h    ; a ring (lower case)
ae_lc       equ 0E6h    ; ae (lower case)
c_cedilla   equ 0E7h
c_cedilla_lc equ 0E7h   ; c cedilla (lower case)
e_grave     equ 0E8h
e_acute     equ 0E9h
i_grave     equ 0ECh
n_tilde_lc  equ 0F1h    ; n tilde (lower case)
o_grave     equ 0F2h
o_umlaut_lc equ 0F6h    ; o umlaut (lower case)
o_slash_lc  equ 0F8h    ; o slashed (lower case)
u_grave     equ 0F9h
u_umlaut_lc equ 0FCh    ; u umlaut (lower case)


ROMBIOS SEGMENT AT 0F000h
	org  0FFFEh
PC_type label BYTE  ; contains computer identification
ROMBIOS ENDS


RAMBIOS SEGMENT AT 00040h
	org  0096h
KB_type label BYTE  ; contains keyboard type
RAMBIOS ENDS





KBINFO          STRUC
kbRanges        DB      4 dup (0)  ; Far East ranges for KANJI
kbStateSize     DW      0       ; #bytes of state info maintained by TOASCII
kbNumFuncKeys   DW      0       ; How many function keys are on the keyboard
kbHasBreak      DW      0       ; true => keyboard supplies make and break
kbRate          DW      0       ; maximum rate of keyboard input events
KBINFO          ENDS

ack_port equ    20h         ; 8259 acknowledge port
eoi      equ    20h         ; 8259 end of interrupt

timer_port  equ 40h         ; 8253 Counter Timer
kb_data     equ 60h
kb_ctl      equ 61h

; hardware interrupt vector number
;
vector      equ 09h
;
; Symbolic equates for contents of KBINFO data structure
;
FarEastRange EQU 0
stateSize    EQU 8
numFunc      EQU 10             ; ten function keys or 12 for RT keyboard
hasBreak     EQU -1
intsPerSec   EQU 10
