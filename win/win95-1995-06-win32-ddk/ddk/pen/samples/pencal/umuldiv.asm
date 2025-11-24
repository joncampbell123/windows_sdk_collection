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

;-----------------------------Module-Header-----------------------------;
; Module Name:	UMULDIV.ASM
;
; A safe UNSIGNED version of MulDiv
;
;-----------------------------------------------------------------------;

	.386

	title Unsigned MulDiv

_TEXT SEGMENT DWORD READONLY PUBLIC USE16 'CODE'

	OPTION  PROLOGUE:None
    	OPTION  EPILOGUE:None

;
; Multiply signed 16.16 by 16.16, returns 16.16
;

UMulDiv PROC NEAR PASCAL PUBLIC

    	mov	bx,sp			; stack pointer to an index register
    	mov     ax,WORD PTR ss:[bx+6]	; get first multiplicand
    	mul     WORD PTR ss:[bx+4]	; unsigned multiply by 2nd multiplier
    	mov     cx,ss:[bx+2]		; divisor == 0?
    	jcxz    dbze			; divide by zero error

    	div     WORD PTR ss:[bx+2]	; divisor in DX:AX ready for udivide
    	ret     6			

dbze:
	xor     ax,ax			; prevent a GPF by returning a 0 
    	ret     6			; for divide by zero error

UMulDiv ENDP

_TEXT ENDS

    END
