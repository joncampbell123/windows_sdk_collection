;*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
;*  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
;*  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
;*  A PARTICULAR PURPOSE.
;*
;*  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.

        .286

        .xlist
        include cmacros.inc
        .list

?PLM=1  ; Pascal calling convention
?WIN=1  ; Windows prolog/epilog code

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
        externFP FatalAppExit        ; in KERNEL
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
; @doc INTERNAL
;
; @asm LibEntry | Called when DLL is loaded.
;
; @reg  CX | Size of heap.
;
; @reg  DI | Module handle.
;
; @reg  DS | Automatic data segment.
;
; @reg  ES:SI | Address of command line (not used).
;
; @rdesc AX is TRUE if the load is successful and FALSE otherwise.
;
; @comm Registers preserved are SI,DI,DS,BP.  Registers destroyed are
;       AX,BX,CX,DX,ES,FLAGS.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, Data
        assumes es, nothing

cProc LibEntry <FAR, PUBLIC, NODATA>, <>

        cBegin

ifdef DEBUG

        ; if this module is not linked first the reserved parameter block
        ; will not be initialized correctly - check for this

        lea ax, pLocalHeap
        cmp ax, 6
        je RsrvPtrsOk

RsrvPtrsHosed:
        int 3

        lea ax, RsrvPtrsMsg
        cCall FatalAppExit, <0, cs, ax>
        jmp RsrvPtrsOk

RsrvPtrsMsg:
        db 'RsrvPtrs hosed!', 0

RsrvPtrsOk:
endif

        ; push frame for LibMain (hModule, cbHeap, lpszCmdLine)

        push di
        push cx
        push es
        push si

        ; init the local heap (if one is declared in the .def file)

        jcxz no_heap

        cCall LocalInit, <0, 0, cx>

no_heap:
        cCall LibMain

        cEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm WEP | This function is called when the DLL is unloaded.
;
; @parm WORD | UselessParm | This parameter has no meaning.
;
; @comm WARNING: This function is basically useless since you can't call any
;     kernel function that may cause the LoadModule() code to be reentered.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc WEP <FAR, PUBLIC, NODATA>, <>

        ParmW UselessParm

        cBegin
        cEnd

sEnd CodeSeg

        end LibEntry
