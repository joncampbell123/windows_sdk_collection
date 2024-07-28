;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	vcdsys.inc -
;
;   Version:	1.00
;
;   Date:	20-Oct-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   20-Oct-1989 RAP
;
;==============================================================================

;******************************************************************************
;******************************************************************************


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
