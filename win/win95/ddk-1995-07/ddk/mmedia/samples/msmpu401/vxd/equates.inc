        page    60, 132

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
;   Module:   EQUATES.INC - equates/structure definitions
;
;   Version:  1.00
;******************************************************************************

;==============================================================================
;                              E Q U A T E S
;==============================================================================

BUG macro stuff
IF1
%out ----&stuff&
endif
endm

MSMPU401_CB_STRUCT struc

        mscb_hlhws              dd      ?               ; list handle to 

MSMPU401_CB_STRUCT ends

VM_HWSTATE_NODE struc

        hws_dwFlags             dd      ?
        hws_pMSMI               dd      ?               ; associated MSMI

VM_HWSTATE_NODE ends

HWCB_FLAG_ALREADYWARNEDMPU401   equ     00000001b

HARDWARE_INSTANCE_NODE  struc

        hwl_pMSMI               dd      ?               ; pointer to MSMI

HARDWARE_INSTANCE_NODE  ends

;
; Port offsets for MPU401
;

MPU401_DATA             equ     00h
MPU401_STATUS           equ     01h
MPU401_COMMAND          equ     01h

LAST_PORT_MPU401        equ     01h

MPU401_DSR              equ     80h          ; data set ready, 0 = ready
MPU401_DRR              equ     40h          ; data receive ready, 0 = ready

MPU401_CMD_UART_MODE    equ     3fh          ; put card in 'UART' mode
MPU401_CMD_RESET        equ     0ffh         ; reset command

;
; PIC register defintions
;

PIC_MASTER_OCW_REG      equ     020h
PIC_MASTER_MASK_REG     equ     021h
PIC_SLAVE_OCW_REG       equ     0A0h
PIC_SLAVE_MASK_REG      equ     0A1h

PIC_IRR_NEXT            equ     0Ah
PIC_ISR_NEXT            equ     0Bh
PIC_POLL_MODE           equ     0Ch

PIC_SPECIFIC_EOI        equ     60h
PIC_SPECIFIC_EOI_SLAVE  equ     62h

PIC_POLL_IRQ_FIRED      equ     80h
PIC_POLL_IRQ_MASK       equ     07h

;
; Get_pMSMI_From_XXX flags:
;

pMSMI_FromDevNode       equ     0000h
pMSMI_FromIRQ           equ     0001h
pMSMI_FromMPU401        equ     0002h

fpMSMI_FromDWord        equ     0000h
fpMSMI_FromWord         equ     0001h
