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
;   Title:    AVVXP500.386 - AURAVISION VxP500 386 Driver
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

AVVXP500_CB_STRUCT struc

        mscb_hlhws              dd      ?               ; list handle to 

AVVXP500_CB_STRUCT ends

VM_HWSTATE_NODE struc

        hws_dwFlags             dd      ?
        hws_pMSMI               dd      ?               ; associated MSMI

VM_HWSTATE_NODE ends

HWCB_FLAG_ALREADYWARNEDVXP500   equ     00000001b

HARDWARE_INSTANCE_NODE  struc

        hwl_pMSMI               dd      ?               ; pointer to MSMI

HARDWARE_INSTANCE_NODE  ends

;
; Port offsets for VXP500
;

VXP500_INDEX             equ     00h
VXP500_DATA		 equ     01h

LAST_PORT_VXP500        equ     01h

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
pMSMI_FromVXP500        equ     0002h

fpMSMI_FromDWord        equ     0000h
fpMSMI_FromWord         equ     0001h
