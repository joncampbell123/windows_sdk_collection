;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

;******************************************************************************
;
;   Title:	vcdsys.inc
;
;==============================================================================

;***********
;  BIOS defined memory locations that point to base IO addrs of com ports
;
VCD_Comm_0_Base_Addr equ 400h	; Addr of port addr of comm 0 (COM1)
VCD_Comm_1_Base_Addr equ 402h	; Addr of port addr of comm 1 (COM2)
VCD_Comm_2_Base_Addr equ 404h	; Addr of port addr of comm 2 (COM3)
VCD_Comm_3_Base_Addr equ 406h	; Addr of port addr of comm 3 (COM4)

;
; flag bits for flag byte in COMM.DRV structure pointed to by VCD_COMDEB_Flag
;
fCOM_ignore_ints    equ 00000001b
bCOM_ignore_ints    equ        0
