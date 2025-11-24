	TITLE MULDIV.ASM
	page 60,132

;-----------------------------------------------------------------------;
;	MULDIV.ASM							;
;									;
;	These are routines for fast 32 bit multiply and divide.		;
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


?WIN	= 0
?PLM	= 1
?NODATA = 0
PMODE   = 1

.xlist
include cmacros.inc
include windows.inc
.list


;---------------------------------------;
;	Definitions and Macros		;
;---------------------------------------;

EAXtoDXAX	macro
	shld	edx,eax,16	; move HIWORD(eax) to dx
	endm


; Manually perform "pop" dword register instruction to remove warning
POPD macro reg
	db	66h
	pop	reg
endm

;---------------------------------------;
;	Segment Declarations		;
;---------------------------------------;

ifndef SEGNAME
	SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

sBegin  CodeSeg
	.386
	assumes cs,CodeSeg
	assumes ds,nothing
	assumes es,nothing


;-----------------------------------------------------------------------;
;	muldiv32							;
;									;
;	Multiples two 32 bit values and then divides the result by a	;
;	third 32 bit value with full 64 bit presision.			;
;									;
;	ulResult = (ulNumber * ulNumerator) / ulDenominator		;
;									;
;	Entry:								;
;	dwNumber = number to multiply by nNumerator			;
;	dwNumerator = number to multiply by nNumber			;
;	dwDenominator = number to divide the multiplication result by.	;
;									;
;	Returns:							;
;	DX:AX = result of multiplication and division.			;
;									;
;	Error Returns:	none						;
;									;
;	Registers Preserved:  DS,ES,SI,DI				;
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

cProc   muldiv32,<NEAR,PUBLIC,NODATA,NONWIN>,<>
;	ParmD  ulNumber
;	ParmD  ulNumerator
;	ParmD  ulDenominator
cBegin  nogen
	.386

	pop	cx	; get return addr
	POPD	bx	; get ulDenominator	; pop ebx
	POPD	dx	; get ulNumerator	; pop edx
	POPD	ax	; get ulNumber		; pop eax

	imul	edx	; edx:eax = (ulNumber * ulNumerator)
	idiv	ebx	; eax	= (ulNumber * ulNumerator) / ulDenominator
	EAXtoDXAX	; covert eax to dx:ax for 16 bit programs

	push	cx	; retore return addr
	ret

cEnd	nogen

sEnd   CodeSeg

	end
