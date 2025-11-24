;-----------------------------------------------------------------------;
;	VCAP.INC							;
;									;
;	Assembly language equates and macros.				;
;									;
;	For the AuraVision video capture driver AVCAPT.DRV.		;
;									;
;-----------------------------------------------------------------------;
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************


TRUE		equ	1
FALSE		equ	0

PIC_EOI_MASTER	equ	20h		; where to send master EOI
PIC_EOI_SLAVE	equ	0A0h		; where to send slave EOI

PIC_IMR_MASTER	equ	21h
PIC_IMR_SLAVE	equ	0A1h


; Common structures for accessing lo/hi words of DWORD

LONG	struc
lo	dw	?
hi	dw	?
LONG	ends

LOHI	struc
lob	db	?
hib	db	?
LOHI	ends

FARPOINTER	struc
off	dw	?
sel	dw	?
FARPOINTER	ends


;---------------------------------Macro---------------------------------;
;	EnterCrit							;
;									;
;	Saves the current state of the interrupt flag on the stack then	;
;	disables interrupts.						;
;									;
;	Registers Destroyed: BX, FLAGS					;
;-----------------------------------------------------------------------;

EnterCrit macro
	local   no_cli
	pushf
	pushf
	pop	bx
	test	bh,2		; if interupts are already off, dont blow
	jz	no_cli		; ... ~300 clocks doing the cli
	cli
no_cli:
endm


;---------------------------------Macro---------------------------------;
;	LeaveCrit							;
;									;
;	Restore the interrupt state saved by EnterCrit.			;
;									;
;	Registers Destroyed:  BX, FLAGS					;
;-----------------------------------------------------------------------;

LeaveCrit macro reg
	local   no_sti
	pop	bx
	test	bh, 2
	jz	no_sti
	sti
no_sti:
endm


;-----------------------------------------------------------------------;
;	Debug macros							;
;-----------------------------------------------------------------------;

ifdef DEBUG
	extrn OutputDebugString:far	
	extrn _wDebugLevel:word	; initc.c
endif


D1 macro text
	DOUT 1, < ",13,10,"Capture: &text&>
	endm

D2 macro text
	DOUT 2, < &text&>
	endm

D3 macro text
	DOUT 3, < &text&>
	endm

D4 macro text
	DOUT 4, < &text&>
	endm

DOUT macro level, text
	local   string_buffer
	local   wrong_level

ifdef DEBUG

_DATA segment
string_buffer label byte
	db	"&text&",0
_DATA ends

	cmp	[_wDebugLevel], level
	jl	wrong_level
	push	DataBASE
	push	DataOFFSET string_buffer
	call	OutputDebugString
wrong_level:
endif
	endm


;-----------------------------------------------------------------------;
;	Assert macros							;
;									;
;	AssertF byte	-- fail iff byte==0				;
;	AssertT byte	-- fail iff byte!=0				;
;-----------------------------------------------------------------------;

AssertF	macro exp
	local   assert_ok
ifdef DEBUG
	push	ax
	
	mov	al,exp
	or	al,al
	jnz	assert_ok

	D1	<AssertF fail (&exp&)>
	int	3

assert_ok:
	pop	ax
endif
	endm


AssertT	macro exp
	local   assert_ok
ifdef DEBUG
	push	ax
	
	mov	al,exp
	or	al,al
	jz	assert_ok

	D1	<AssertT fail (&exp&)>
	int	3

assert_ok:
	pop	ax
endif
	endm


;-----------------------------------------------------------------------;
;	Reminder macro							;
;-----------------------------------------------------------------------;

BUG macro stuff
if1
%out ----&stuff&
endif
endm
