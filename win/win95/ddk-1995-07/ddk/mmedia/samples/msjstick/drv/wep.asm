;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
;
;   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
;
;   WEP.ASM
;
;   This module contains a do-nothing WEP() procedure for a DLL
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .286

        .xlist
        include cmacros.inc                   
        .list

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
;   code segment
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

sBegin CodeSeg

    assumes cs, CodeSeg

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @doc INTERNAL
;
; @asm WEP | This function is called when the DLL is unloaded.
;
; @parm WORD | UselessParm | This parameter has no meaning.
;
; @comm WARNING: This function is basically useless since you can't call any
;     kernel function that may cause the LoadModule() code to be reentered.
;
;     Following are all the rules to remember when dealing with WEP(). If you
;     don't follow these you can crash windows (in low memory etc....)
;
;     1. WEP() must be in the resident name table
;              (i.e. exported by name, not by ordinal)
;
;              EXPORTS WEP
;
;                -- or --
;
;              EXPORTS WEP     @1  RESIDENTNAME
;
;     2. WEP() must be in a FIXED code segment, not just non-DISCARDABLE
;
;     3. Everything WEP() *calls* must also be in a FIXED segment
;
;     4. WEP() can't call any linked DLLs
;
;     5. WEP() can't call any kernel module managment functions
;
;     6. The stack is small, so don't eat a lot of stack
;
;     7. a DLL must have a WEP or you will get a RIP in LoadModule()
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        assumes ds, nothing
        assumes es, nothing

cProc WEP <FAR, PUBLIC, PASCAL>, <>

        ParmW UselessParm

        cBegin nogen

        mov ax, 1
        retf 2

        cEnd nogen

sEnd CodeSeg

        end
