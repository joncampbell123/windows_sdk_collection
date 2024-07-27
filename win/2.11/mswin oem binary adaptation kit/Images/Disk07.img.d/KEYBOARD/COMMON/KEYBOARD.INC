
;*** KEYBOARD.INC **********************************************************
;                                                                          *
;   Copyright (C) 1983,1984 by Microsoft Inc.                              *
;                                                                          *
;***************************************************************************
;
;	History
;	15 jun 88	plb	Add OPTALL options list
;
;	Olivetti history
;	04-25-85	??	Microsoft
;	25 sep 85	plb	moved accent ANSI codes defs here.
;	27 sep 85	plb	Added tilde to accent definitions.
;	30 sep 85	plb	moved accent def's to OLIKBD.INC.
;	 3 oct 85	plb	Changed some kbinfo equates
;	31 oct 85	plb	Default numFunc = 10
;	19 feb 86	plb	\lib is now on D:
;	27 jun 87	plb	\lib is now left for command line def.
;				.. also for windefs.inc
;	30 jun 87	plb	Moved banner to top.
;				Defined eoiAT.
if1
%out  KEYBOARD.INC
endif
;***************************************************************************
;                                                                          *
;   Header file for IBM/PC keyboard device driver.  Contains all machine   *
;   specific constant and data structure definitions.                      *
;                                                                          *
;***************************************************************************
include	cmacros.inc
include	windefs.inc

ifdef OPTALL
ERICSSON equ 1	; enable EIS code and tables.
ENVOY equ 1	; enable support for Hewlett-Packard ENVOY keyboards.
; VKFUN equ 1	; enable VkKeyScan()  [ define this in MAKE command line ]
endif

ack_port equ	20h	    ; 8259 acknowledge port
eoi	 equ	20h	    ; 8259 end of interrupt
eoiAT	equ	61h		; end of interrupt for AT-like system

timer_port  equ	40h	    ; 8253 Counter Timer
kb_data	    equ	60h
kb_ctl	    equ	61h

; hardware interrupt vector number
;
vector	    equ	09h
;
; Symbolic equates for contents of KBINFO data structure
;
FarEastRange EQU 0	; No kana/kanji.
stateSize    EQU 8	; # of bytes in state block (4 words)
numFunc	     EQU 10	; ten, 12 or 18 function keys.. default to 10
hasBreak     EQU -1	; has up/down codes
intsPerSec   EQU 15	; repeat speed (excl. of mouse!)

