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
;   Title:    MSMPU401.386 - MICROSOFT MPU-401 386 Driver
;
;   Module:   VALIDATE.ASM - Hardware validation routines
;
;   Version:  4.00
;******************************************************************************
;
;   Functional Description:
;      MPU-401 validation routines.
;
;******************************************************************************

        .386p

;==============================================================================
;                             I N C L U D E S
;==============================================================================

        .xlist
        include vmm.inc
        include debug.inc
        include msmpu401.inc
        include equates.inc
        .list


;==============================================================================
;                             P N P   C O D E
;==============================================================================

VxD_PNP_CODE_SEG

;---------------------------------------------------------------------------;
;
;   Is_MPU401_Valid
;
;   DESCRIPTION:
;       Verifies the IO address in EDX.
;
;   ENTRY:
;       EDX = IO address to try.
;
;   EXIT:
;       if Carry clear
;           IO Address is valid
;       else Carry set
;           IO Address is not valid.
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc Is_MPU401_Valid
        
        enter   8, 0

fNotReady       equ     dword ptr [ebp - 4]     ; set when not ready
fResetNoACK     equ     dword ptr [ebp - 8]     ; set when no ACK received

        push    eax
        push    ecx
        push    edx

        Trace_Out "MSMPU401, Is_MPU401_Valid: #EDX"

        mov     fResetNoACK, False              ; clear ACK status

IMV_TryAgain:
        mov     fNotReady, False              
        mov     ecx, 64                         ; 64 bytes max??

IMV_Clean_FIFO:
        inc     edx                             ; move to status port
        in      al, dx                          ; and read
        dec     edx                             ; back to data
        test    al, MPU401_DSR                  ; data set ready?
        jnz     SHORT IMV_Check_Ready
        in      al, dx                          ; waste the data
        loop    IMV_Clean_FIFO

IMV_Check_Ready:
        mov     ecx, 6000h                      ; !!! hardcoded loop !!!

IMV_Wait_Ready:
        inc     edx                             ; point to status port
        in      al, dx                          ; read status
        dec     edx                             ; back to data port
        test    al, MPU401_DRR                  ; Q: can we write command?
        jz      SHORT IMV_Hardware_Ready        ;   Y: then do it!
        loop    IMV_Wait_Ready

        ;
        ; timed out - hardware might be in a bad state, so force it if
        ; we're trying to reset, if second time through, we're hosed.
        ;

        cmp     fNotReady, False
        jne     SHORT IMV_Failure
        mov     fNotReady, NOT False

        Trace_Out "MSMPU401: MPU-401 Forced Reset"

        inc     edx
        mov     al, MPU401_CMD_RESET            ; forced reset
        out     dx, al  
        dec     edx

        jmp     SHORT IMV_Check_Ready

IMV_Hardware_Ready:
       
        Trace_Out "MSMPU401: MPU-401 hardware ready, resetting"

        mov     al, MPU401_CMD_RESET            ; reset
        inc     edx
        out     dx, al  
        dec     edx

IMV_Loop_For_ACK:
        mov     ecx, 6000h                      ; !!! hardcoded loop !!!

IMV_Wait_ACK:
        inc     edx
        in      al, dx
        dec     edx
        test    al, MPU401_DSR
        jz      SHORT IMV_Read_Data
        loop    IMV_Wait_ACK

        ;
        ; We've failed to get an ACK, try one-more time.
        ;

        cmp     fResetNoACK, False
        jne     SHORT IMV_Failure
        mov     fResetNoACK, NOT False
        jmp     SHORT IMV_TryAgain

IMV_Read_Data:
        in      al, dx
        cmp     al, 0feh                        ; Q: Is this the ACK?
        jne     SHORT IMV_Failure               ;    N: Failure

IMV_OK:
        Trace_Out "Everything passed A-OK!"

        clc
        jmp     SHORT IMV_Exit


IMV_Failure:
        Trace_Out "MPU-401 test failed!"
        
        stc

        Assumes_Fall_Through IMV_Exit

IMV_Exit:
        pop     edx
        pop     ecx
        pop     eax

        leave
        ret

EndProc Is_MPU401_Valid

;---------------------------------------------------------------------------;
;
;   _MPU401_Command_Write
;
;   DESCRIPTION:
;       Writes a command to the MPU-401.
;
;   ENTRY:
;       
;
;   EXIT:
;
;   USES:
;       EAX, ECX, EDX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _MPU401_Command_Write, PUBLIC

        wBaseMPU401     equ     [ebp + 8]
        bData           equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        mov     ecx, 6000h
        movzx   edx, word ptr wBaseMPU401

        ;
        ; dummy read
        ;

        in      al, dx
        inc     edx

mcw_Wait:
        in      al, dx
        test    al, MPU401_DRR
        jz      SHORT mcw_Ready
        loop    mcw_Wait

        jmp     SHORT mcw_Exit

mcw_Ready:
        mov     al, byte ptr bData
        out     dx, al

mcw_Exit:
        pop     ebp
        ret

EndProc _MPU401_Command_Write

;---------------------------------------------------------------------------;
;
;   _MPU401_Generate_IRQ
;
;   DESCRIPTION:
;       Forces the MPU-401 to generate an interrupt.
;
;   ENTRY:
;       
;
;   EXIT:
;
;   USES:
;       Flags
;
;---------------------------------------------------------------------------;

BeginProc _MPU401_Generate_IRQ, PUBLIC

        wBaseMPU401     equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    eax
        push    ebx
        push    ecx


        cCall   _MPU401_Command_Write, <wBaseMPU401, MPU401_CMD_RESET>

        movzx   edx, word ptr wBaseMPU401
        inc     edx

        mov     ecx, 6000h

mgi_Wait:
        in      al, dx
        test    al, MPU401_DSR
        jz      SHORT mgi_Wait_For_PIC
        loop    mgi_Wait

mgi_Wait_For_PIC:
        mov     ecx, 2000h

mgi_Wait_A_Bit:
        in      al, dx
        loop    mgi_Wait_A_Bit

mgi_Exit:
        pop     ecx
        pop     ebx
        pop     eax

        pop     ebp
        ret

EndProc _MPU401_Generate_IRQ

;---------------------------------------------------------------------------;
;
;   _MPU401_Clear_IRQ
;
;   DESCRIPTION:
;       Clears interrupt status for MPU-401.
;
;   ENTRY:
;       
;
;   EXIT:
;
;   USES:
;       EAX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _MPU401_Clear_IRQ, PUBLIC

        wBaseMPU401     equ     [ebp + 8]

        push    ebp
        mov     ebp, esp

        push    eax
        push    ecx
        push    edx

        movzx   edx, word ptr wBaseMPU401
        inc     edx

        mov     ecx, 6000h

mci_Wait:
        in      al, dx
        test    al, MPU401_DSR
        jnz     SHORT mci_Exit
        dec     edx
        in      al, dx
        inc     edx
        loop    mci_Wait

mci_Exit:
        pop     edx
        pop     ecx
        pop     eax

        pop     ebp
        ret

EndProc _MPU401_Clear_IRQ

;---------------------------------------------------------------------------;
;
;   _Validate_IRQ
;
;   DESCRIPTION:
;       Finds the location of the IRQ setting for the MPU-401.
;
;   ENTRY:
;       
;
;   EXIT:
;       EAX is TRUE if IRQ is as specified or FALSE otherwise.
;
;   USES:
;       EAX, Flags
;
;---------------------------------------------------------------------------;

BeginProc _Validate_IRQ, PUBLIC

        wBaseMPU401     equ     [ebp + 8]
        wIRQ            equ     [ebp + 12]

        push    ebp
        mov     ebp, esp

        push    ebx                             ; save some regs.
        push    ecx
        push    edx

        ;-------------------------------------------------------
        ; Tell hardware to generate an IRQ and watch the
        ; IRR to see if it fired.
        ;-------------------------------------------------------

        movzx   eax, word ptr wIRQ
        xor     ecx, ecx
        bts     ecx, eax

        pushfd                                  ; save IF
        cli                                     ; Disable interrupts!

        in      al, PIC_SLAVE_MASK_REG          ; Get slave PIC mask
        mov     ah, al                          ; Shift it over
        in      al, PIC_MASTER_MASK_REG         ; Get master PIC mask
        push    eax                             ; Save mask for later

        or      eax, ecx                        ; Mask possible IRQs

        ;
        ; Write new mask to PIC.
        ;

        xchg    al, ah
        out     PIC_SLAVE_MASK_REG, al                  
        xchg    al, ah
        out     PIC_MASTER_MASK_REG, al

        ;
        ; Set IRQ to low... should be reflected in IRR.
        ;

        mov     al, PIC_IRR_NEXT
        out     PIC_SLAVE_OCW_REG, al
        IO_Delay
        out     PIC_MASTER_OCW_REG, al
        IO_Delay

        cCall   _MPU401_Clear_IRQ, <wBaseMPU401>

        in      al, PIC_SLAVE_OCW_REG
        mov     ah, al
        in      al, PIC_MASTER_OCW_REG

        Trace_Out "MSMPU401: _Validate_IRQ (low): PIC IRR: #AX, looking for: #CX"

        test    eax, ecx
        jnz     SHORT VI_IRQ_Failure

        ;
        ; Set IRQ to high... should be reflected in IRR.
        ;

        cCall   _MPU401_Generate_IRQ, <wBaseMPU401>

        ;
        ; Check the IRR registers to see if an interrupt fired.
        ;

        in      al, PIC_SLAVE_OCW_REG
        mov     ah, al
        in      al, PIC_MASTER_OCW_REG

        Trace_Out "MSMPU401: _Validate_IRQ (high): PIC IRR: #AX, looking for: #CX"

        test    eax, ecx
        jnz     SHORT VI_CleanUp

VI_IRQ_Failure:
        xor     eax, eax

VI_CleanUp:
        ;-------------------------------------------------------
        ; Clear the IRQ...
        ;-------------------------------------------------------

        cCall   _MPU401_Clear_IRQ, <wBaseMPU401>

        mov     edx, eax                        ; save return value

        ;
        ; Restore PIC to previous state
        ;

        pop     eax                             ; Get saved PIC mask
        xchg    al, ah                          
        out     PIC_SLAVE_MASK_REG, al          ; Write mask to PIC
        xchg    al, ah
        out     PIC_MASTER_MASK_REG, al

        mov     edx, eax                        ; get return value

        popfd                                   ; restore IF

        pop     edx
        pop     ecx                             ; Restore saved regs.
        pop     ebx

        pop     ebp

        ret

EndProc _Validate_IRQ

VxD_PNP_CODE_ENDS

end
