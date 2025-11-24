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
;   Title:    AVVXP500.INC - Auravision VXP500 VxD Interface
;
;   Module:   AVVXP500.INC - public equates/structure definitions
;
;   Version:  4.00
;******************************************************************************
;
;   NOTES:
;       This common interface is supported by VxDs wishing to use the
;       AVVXP500.DRV MIDI interface driver.  This include file defines the
;       API interface and structures used by AVCAPT.DRV.
;
;******************************************************************************

;
; The BELOW defined VxD ID is not valid!!! One must be _ASSIGNED_ to 
; AURAVISION by Microsoft.  If necessary email vxdid@microsoft.com
;

AVVXP500_Device_ID              equ 4321h   ; Device ID for AVVXP500.VXD

AVVXP500_Ver_Major              equ 4       ; version 4.00 of this VxD
AVVXP500_Ver_Minor              equ 0

;
; PMode and RMode API indexes
;

AVVXP500L_API_Get_Version       equ 0000h   ; get version info of
                                            ; supporting VxD.

AVVXP500_API_Get_Info           equ 0001h
AVVXP500_API_Acquire            equ 0002h
AVVXP500_API_Release            equ 0003h

;
; Error returns for AVVXP500_API_Acquire/Release (returned in Client_AX)
;

AVVXP500_API_Err_Bad_Base_Port   equ 0001h
AVVXP500_API_Err_Already_Owned   equ 0002h
AVVXP500_API_Err_Not_Yours       equ 0003h
AVVXP500_API_Err_State_Unknown   equ 8000h

;
; MSOPL information structure
;

AVVXP500INFO struc

        avdi_dwSize             dd  ?   ; size of structure
        avdi_wHardwareOptions   dw  ?   ; hardware support
        avdi_wIOAddressVxP500   dw  ?   ; base I/O
        avdi_bIRQ               db  ?   ; IRQ connected or -1
        avdi_bReserved          db  ?   ; filler
        avdi_wVersionVxD        dw  ?   ; VxD Version
        avdi_wFlags             dw  ?   ; flags
        avdi_wSelector          dw  ?   ; GDT selector pointing to frame buf
        avdi_dwMemBase          dd  ?   ; frame buffer segment

        avdi_dn                 dd  ?   ; PnP DevNode
        avdi_dwIRQHandle        dd  ?   ; VPICD IRQ handle
        avdi_dwVxP500OwnerCur   dd  ?   ; VM handle of current VxP500 owner
        avdi_dwVxP500OwnerLast  dd  ?   ; VM handle of previous VxP500 owner
        avdi_hVXP500Stubs	dd  ?   ;

AVVXP500INFO ends

AVDI_FLAG_DISABLEWARNING        equ 0001h   ; warnings disabled on contention
AVDI_FLAG_IRQWASUNMASKED        equ 8000h   ; irq being used was unmasked
