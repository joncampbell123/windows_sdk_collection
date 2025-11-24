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
;   Title:    MSMPU401.INC - MICROSOFT MPU401 Driver Common VxD Interface
;
;   Module:   MSMPU401.INC - public equates/structure definitions
;
;   Version:  4.00
;******************************************************************************
;
;   NOTES:
;       This common interface is supported by VxDs wishing to use the
;       MSMPU401.DRV MIDI interface driver.  This include file defines the
;       API interface and structures used by MSMPU401.DRV.
;
;******************************************************************************

MSMPU401_Device_ID              equ 44ch    ; Device ID for MSMPU401.386

MSMPU401_Ver_Major              equ 4       ; version 4.00 of this VxD
MSMPU401_Ver_Minor              equ 0

;
; PMode and RMode API indexes
;

PIPE_API_Open                   equ 0200h   ; pipeOpen()
PIPE_API_Close                  equ 0201h   ; pipeClose()

MSMPU401L_API_Get_Version       equ 0300h   ; get version info of
                                            ; supporting VxD.

MSMPU401_API_Get_Info           equ 0301h
MSMPU401_API_GetInfoF_DevNode   equ 0001h   ; devnode specified in ECX
MSMPU401_API_GetInfoF_Mask      equ 0001h   ; valid functions

MSMPU401_API_Acquire            equ 0302h
MSMPU401_API_Release            equ 0303h

;
; Error returns for MSMPU401_API_Acquire/Release (returned in Client_AX)
;

MSMPU401_API_Err_Bad_Base_Port   equ 0001h
MSMPU401_API_Err_Already_Owned   equ 0002h
MSMPU401_API_Err_Not_Yours       equ 0003h
MSMPU401_API_Err_State_Unknown   equ 8000h

;
; MSOPL information structure
;

MSMPU401INFO struc

        msmi_dwSize             dd  ?   ; size of structure
        msmi_wHardwareOptions   dw  ?   ; hardware support
        msmi_wIOAddressMPU401   dw  ?   ; base I/O
        msmi_bIRQ               db  ?   ; IRQ connected or -1
        msmi_bReserved          db  ?
        msmi_wVersionVxD        dw  ?
        msmi_wFlags             dw  ?
        msmi_wReserved          dw  ?

        msmi_dn                 dd  ?   ; PnP DevNode
        msmi_dwIRQHandle        dd  ?   ; VPICD IRQ handle
        msmi_dwMPU401OwnerCur   dd  ?   ; VM handle of current MPU401 owner
        msmi_dwMPU401OwnerLast  dd  ?   ; VM handle of previous MPU401 owner
        msmi_hMPU401Stubs       dd  0   ; trap stubs for MPU-401

MSMPU401INFO ends

MSMPU401_HWOPTIONSF_IRQSHARED   equ     0001    ; IRQ is shared, controlling
                                                ; driver will open pipe to
                                                ; grab ISR entry

MSMI_FLAG_DISABLEWARNING        equ 0001h   ; warnings disabled on contention
MSMI_FLAG_IRQWASUNMASKED        equ 8000h   ; irq being used was unmasked
