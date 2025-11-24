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

;**	WORD_S - size defined method of accessing the bytes in a word
;**	DWORD_S - size defined method of accessing the words in a dword
;**	QWORD_S - size defined method of accessing the dwords in a qword
;

;			....oditsz.a.p.c
F_OVERFLOW	EQU	0000100000000000B
F_DIRECTION	EQU	0000010000000000B
F_INTERRUPT	EQU	0000001000000000B
F_TRACE 	EQU	0000000100000000B
F_SIGN		EQU	0000000010000000B
F_ZERO		EQU	0000000001000000B
F_AUX		EQU	0000000000010000B
F_PARITY	EQU	0000000000000100B
F_CARRY 	EQU	0000000000000001B

word_s		struc
lobyte		db	?
hibyte		db	?
word_s		ends

dword_s		struc
loword		dw	?
hiword		dw	?
dword_s		ends

;* Far Pointer Structure

farp		struc
woff		dw	?		; word offset
wseg		dw	?		; word segment
farp		ends

;* 32 bit Far Pointer Structure (FWORD)

farp32		struc
offst32		dd	?		; dword offset
segmt32		dw	?		; word segment
farp32		ends

;* IRET Structures

IrStk		struc
  IrFff		dw	?		; offset
  IrSeg		dw	?		; segment
  IrFlag	dw	?		; flags
IrStk		ends


qword_s	struc
	lodword		DD	?
	hidword		DD	?
qword_s	ends

pushad_s	struct
	pushedi		DD	?
	pushesi		DD	?
	pushebp		DD	?
	pushesp		DD	?
	pushebx		DD	?
	pushedx		DD	?
	pushecx		DD	?
	pusheax		DD	?
pushad_s	ends

dtr_s	struc
	dtr_limit	dw	0		; limit of gdt/idt register
	dtr_baselow	dw	0		; bits 0-15 of base address
	dtr_basehigh	db	0		; bits 16-23 of base address
	dtr_baseext	db	0		; bits 24-31 of base address
dtr_s	ends

dtr_base		equ	(dword ptr dtr_baselow)

gate_s	struc
	gate_offset	dw	0		; gate offset
	gate_selector	dw	0		; gate selector
	gate_wordcount	db	0		; word count for call gate
	gate_access	db	0		; access byte
	gate_extoffset	dw	0		; extended target offset
gate_s	ends
