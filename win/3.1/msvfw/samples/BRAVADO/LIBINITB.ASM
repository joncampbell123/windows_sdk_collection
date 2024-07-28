        TITLE LIBINIT.ASM
        page 60,132

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; LIBINIT.ASM - 
;
;   General Description:
;      Library stub to do local init for a dynamic linked library.
;
;   Restrictions:
;      This must be the first object file in the LINK line.  This assures
;      that the reserved parameter block is at the *base* of DGROUP.
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

if1
%out link me first!!
endif

        PMODE = 1

        .xlist
        include cmacros.inc
        .list
        .286

?PLM=1  ; Pascal calling convention
?WIN=0  ; Windows prolog/epilog code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   segmentation
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ifndef SEGNAME
    SEGNAME equ <_TEXT>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   external functions
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        externFP LocalInit           ; in KERNEL
        externNP LibMain             ; C code to do DLL init

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   data segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin Data

        assumes ds, Data

; stuff needed to avoid the C runtime coming in, and init the Windows
; reserved parameter block at the base of DGROUP

        org 0               ; base of DATA segment!

        dd  0               ; so null pointers get 0

maxRsrvPtrs = 5
        dw  maxRsrvPtrs

usedRsrvPtrs = 0
labelDP <PUBLIC, rsrvptrs>

DefRsrvPtr  macro   name
        globalW     name, 0
        usedRsrvPtrs = usedRsrvPtrs + 1
endm

DefRsrvPtr  pLocalHeap          ; local heap pointer
DefRsrvPtr  pAtomTable          ; atom table pointer
DefRsrvPtr  pStackTop           ; top of stack
DefRsrvPtr  pStackMin           ; minimum value of SP
DefRsrvPtr  pStackBot           ; bottom of stack

if maxRsrvPtrs-usedRsrvPtrs
        dw maxRsrvPtrs-usedRsrvPtrs DUP (0)
endif

public  __acrtused
        __acrtused = 1

sEnd Data

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

        assumes cs, CodeSeg

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LibEntry  - Called when DLL is loaded.
;
; CX	- Size of heap.
;
; DI	- Module handle.
;
; DS	- Automatic data segment.
;
; ES:SI - Address of command line (not used).
;
; Returns AX is TRUE if the load is successful and FALSE otherwise.
;
; Registers preserved are SI,DI,DS,BP.  Registers destroyed are
;       AX,BX,CX,DX,ES,FLAGS.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc LibEntry <FAR, PUBLIC, NODATA>, <>
cBegin

;
;   push frame for LibMain (hModule, cbHeap, lpszCmdLine)
;
        push    di
        push    cx
        push    es
        push    si

;
; init the local heap (if one is declared in the .def file)
;
        jcxz    no_heap

        cCall   LocalInit, <0, 0, cx>

no_heap:
        cCall   LibMain
cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; WEP	This function is called when the DLL is unloaded.
;
; wUselessParm	- This parameter has no meaning.
;
; WARNING: This function is basically useless since you can't call any
;     kernel function that may cause the LoadModule() code to be reentered.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc WEP <FAR, PUBLIC, NODATA>, <>
;       ParmW   wUselessParm
cBegin nogen

        mov     ax, 1
        retf    2

cEnd nogen



sEnd CodeSeg

        end LibEntry
