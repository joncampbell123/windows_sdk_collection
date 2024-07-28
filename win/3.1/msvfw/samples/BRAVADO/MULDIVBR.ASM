        TITLE MULDIV.ASM
        page 60,132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; MULDIV.ASM - Fast 32 bit multiplies
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

?WIN	= 0
?PLM	= 1
?NODATA = 0
PMODE   = 1

        .xlist
        include cmacros.inc
        include windows.inc
        .list

;       The following two equates are just used as shorthand
;       for the "word ptr" and "byte ptr" overrides.

wptr    equ     word ptr
bptr    equ     byte ptr

; The following structure should be used to access high and low
; words of a DWORD.  This means that "word ptr foo[2]" -> "foo.hi".

LONG    struc
lo      dw      ?
hi      dw      ?
LONG    ends

EAXtoDXAX   macro
        shld    edx,eax,16      ; move HIWORD(eax) to dx
        endm

DXAXtoEAX   macro
        ror     eax,16          ; xchg HIWORD(eax) and LOWORD(eax)
        shrd    eax,edx,16      ; move LOWORD(edx) to HIWORD(eax)
        endm

ifndef SEGNAME
    SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

sBegin  CodeSeg
        .386
        assumes cs,CodeSeg
	assumes ds,nothing
        assumes es,nothing

;---------------------------Public-Routine------------------------------;
; muldiv32
;
; multiples two 32 bit values and then divides the result by a third
; 32 bit value with full 64 bit presision
;
; ulResult = (ulNumber * ulNumerator) / ulDenominator
;
; Entry:
;       dwNumber = number to multiply by nNumerator
;       dwNumerator = number to multiply by nNumber
;       dwDenominator = number to divide the multiplication result by.
;   
; Returns:
;       DX:AX = result of multiplication and division.
; Error Returns:
;       none
; Registers Preserved:
;       DS,ES,SI,DI
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

cProc   muldiv32,<NEAR,PUBLIC,NODATA,NONWIN>,<>
;       ParmD  ulNumber
;       ParmD  ulNumerator
;       ParmD  ulDenominator
cBegin  nogen
        .386

        pop     cx      ; get return addr
        pop     ebx     ; get ulDenominator
        pop     edx     ; get ulNumerator
        pop     eax     ; get ulNumber

; unsigned version 
;        mul     edx     ; edx:eax = (ulNumber * ulNumerator)
;        div     ebx     ; eax     = (ulNumber * ulNumerator) / ulDenominator

; signed version 
        imul     edx     ; edx:eax = (ulNumber * ulNumerator)
        idiv     ebx     ; eax     = (ulNumber * ulNumerator) / ulDenominator
        EAXtoDXAX       ; covert eax to dx:ax for 16 bit programs

        push    cx      ; retore return addr
        ret

cEnd    nogen

sEnd   CodeSeg

       end
