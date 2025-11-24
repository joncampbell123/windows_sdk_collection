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
;   Title:    MSOPL.386 - MICROSOFT MPU-401 386 Driver
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

MSOPL_CB_STRUCT struc

        mscb_hlhws              dd      ?               ; list handle to 

MSOPL_CB_STRUCT ends

VM_HWSTATE_NODE struc

        hws_dwFlags             dd      ?
        hws_pMSOI               dd      ?               ; associated MSOI

VM_HWSTATE_NODE ends

HWCB_FLAG_ALREADYWARNEDOPL      equ     00000001b

HARDWARE_INSTANCE_NODE  struc

        hwl_pMSOI               dd      ?               ; pointer to MSOI

HARDWARE_INSTANCE_NODE  ends

;
; Port offsets for OPL2/3
;

BASE_OPL                equ     0388h
OPL_0                   equ     00h
OPL_1                   equ     01h
OPL_2                   equ     02h
OPL_3                   equ     03h

LAST_PORT_OPL3          equ     OPL_3
LAST_PORT_OPL2          equ     OPL_1

;
; FM registers
;

AD_LSI                  equ     0001h
AD_LSI2                 equ     0101h
AD_TIMER1               equ     0002h
AD_TIMER2               equ     0003h
AD_MASK                 equ     0004h
AD_CONNECTION           equ     0104h
AD_NEW                  equ     0105h
AD_NTS                  equ     0008h
AD_MULT                 equ     0020h
AD_MULT2                equ     0120h
AD_LEVEL                equ     0040h
AD_LEVEL2               equ     0140h
AD_AD                   equ     0060h
AD_AD2                  equ     0160h
AD_SR                   equ     0080h
AD_SR2                  equ     0180h
AD_FNUMBER              equ     00a0h
AD_FNUMBER2             equ     01a0h
AD_BLOCK                equ     00b0h
AD_BLOCK2               equ     01b0h
AD_DRUM                 equ     00bdh
AD_FEEDBACK             equ     00c0h
AD_FEEDBACK2            equ     01c0h
AD_WAVE                 equ     00e0h
AD_WAVE2                equ     01e0h

;
; Get_pMSOI_From_XXX flags:
;

pMSOI_FromDevNode       equ     0000h
pMSOI_FromSynth         equ     0001h

fpMSOI_FromDWord        equ     0000h
fpMSOI_FromWord         equ     0001h
