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
;   Title:    MSOPL.INC - MICROSOFT OPL3/OPL3 Driver Common VxD Interface
;
;   Module:   MSOPL.INC - public equates/structure definitions
;
;   Version:  4.00
;******************************************************************************
;
;   NOTES:
;       This common interface is supported by VxDs wishing to use the
;       MSOPL.DRV FM synthesis driver.  This include file defines the
;       API interface and structures used by MSOPL.DRV.
;
;******************************************************************************

MSOPL_Device_ID                 equ 44dh    ; Device ID for MSOPL.VXD

MSOPL_Ver_Major                 equ 4       ; version 4.00 of this VxD
MSOPL_Ver_Minor                 equ 0

;
; PMode and RMode API indexes
;

MSOPL_API_Get_Version           equ 0100h   ; get version info of
                                            ; supporting VxD.

MSOPL_API_Get_Info              equ 0101h
MSOPL_API_GetInfoF_DevNode      equ 0001h   ; devnode specified in ECX
MSOPL_API_GetInfoF_Mask         equ 0001h   ; valid functions

MSOPL_API_Acquire               equ 0102h
MSOPL_API_Release               equ 0103h

PIPE_API_Open                   equ 0200h   ; pipeOpen()
PIPE_API_Close                  equ 0201h   ; pipeClose()

;
; Error returns for MSOPL_API_Acquire/Release (returned in Client_AX)
;

MSOPL_API_Err_Bad_Base_Port     equ 0001h
MSOPL_API_Err_Already_Owned     equ 0002h
MSOPL_API_Err_Not_Yours         equ 0003h
MSOPL_API_Err_State_Unknown     equ 8000h

;
; MSOPL information structure
;

MSOPLINFO struc

        msoi_dwSize             dd  ?   ; size of structure
        msoi_wHardwareOptions   dw  ?   ; hardware support
        msoi_wIOAddressSynth    dw  ?   ; base I/O
        msoi_wVersionVxD        dw  ?
        msoi_wFlags             dw  ?

        msoi_dn                 dd  ?   ; PnP DevNode
        msoi_dwSynthOwnerCur    dd  ?   ; VM handle of current OPL2/3 owner
        msoi_dwSynthOwnerLast   dd  ?   ; VM handle of previous OPL2/3 owner
        msoi_hOPLStubs          dd  0   ; trap stubs for OPL2/3

MSOPLINFO ends

MSOI_FLAG_DISABLEWARNING        equ     0001h   ; warnings disabled on contention

MSOPL_HWOPTIONSF_OPL3DETECTED   equ     0001h   ; OPL3 detected
