        page    60, 132

;******************************************************************************
        title   VALIDATE.ASM - Detection routines
;******************************************************************************
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
;******************************************************************************
;
;   Title:    MSOPL.386 - MICROSOFT OPL2/OPL3 386 Driver
;
;   Module:   VALIDATE.ASM - Hardware validation routines
;
;   Version:  4.00
;******************************************************************************
;
;   Functional Description:
;      OPL2/OPL3 validation routines.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include msopl.inc
        include equates.inc
        .list

EXTRN OPL_RegWrite:NEAR

;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;------------------------------------------------------------------------------
;
;   Is_Synth_Valid
;
;   DESCRIPTION:
;       Determines if the Synth is present.
;
;   ENTRY:
;       EDX = suspected IO Address of Synth
;
;   EXIT:
;       if Carry clear
;           Synth has been detected, MSOPL_HWOPTIONSF_OPL3DETECTED in EAX
;           if OPL3 return value detected.
;       else Carry set
;           Synth is not present
;
;   USES:
;       EAX, Flags
;
;------------------------------------------------------------------------------

BeginProc Is_Synth_Valid, PUBLIC

        Trace_Out "MSSNDSYS: Synth validation: #EDX"

        push    ebx
        push    ecx

        pushfd

        cli                                     ; NO INTERRUPTS HERE!!!

        mov     ebx, AD_MASK                    ; mask both timers
        mov     al, 60h
        call    OPL_RegWrite
        mov     ebx, AD_MASK                    ; reset timers
        mov     al, 80h
        call    OPL_RegWrite
        mov     ebx, AD_TIMER1                  ; set timer value
        mov     al, 80h
        call    OPL_RegWrite
        mov     ebx, AD_MASK                    ; start timer
        mov     al, 01h
        call    OPL_RegWrite

        mov     cx, 4000h

IOV_TimerLoop:
        in      al, dx
        test    al, 40h
        jnz     SHORT IOV_TimerFired
        loop    IOV_TimerLoop

IOV_TimerFired:
        push    eax
        mov     ebx, AD_MASK                    ; mask both timers
        mov     al, 60h
        call    OPL_RegWrite
        mov     ebx, AD_MASK                    ; reset timers
        mov     al, 80h
        call    OPL_RegWrite
        pop     eax

        popfd

        test    al, 40h                         ; Q: Did timer 1 fire?
        jz      SHORT IOV_Failure               ;    N: Not found

        ;
        ; NOTE:  This information comes from Yamaha.  On the OPL3,
        ;        data bits 1 and 2 on the status register should be
        ;        low... if not it's probably an OPL2.
        ;

        test    al, 06h
        jnz     SHORT IOV_NotOPL3
        mov     eax, MSOPL_HWOPTIONSF_OPL3DETECTED
        jmp     SHORT IOV_Success
        
IOV_NotOPL3:
        xor     eax, eax

IOV_Success:
        clc
        jmp     SHORT IOV_Exit

IOV_Failure:
        stc
        
IOV_Exit:
        pop     ecx
        pop     ebx
        ret

EndProc Is_Synth_Valid

VxD_PNP_CODE_ENDS

end
