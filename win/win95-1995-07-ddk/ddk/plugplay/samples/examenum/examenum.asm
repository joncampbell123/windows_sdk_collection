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

PAGE 58,132
;******************************************************************************
TITLE EXAMENUM.ASM - Windows/386 Example Enumerator (DLVxD)
;******************************************************************************
;
; Title:  EXAMENUM.ASM - Windows/386 Example Enumerator (DLVxD)
;
;==============================================================================

    .386p

;******************************************************************************
;               I N C L U D E S
;******************************************************************************

    .xlist
    include vmm.inc
    include debug.inc
    include configmg.inc
    .list


;============================================================================
;       V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================

DECLARE_VIRTUAL_DEVICE  EXAMENUM,PNPDRVS_Major_Ver,PNPDRVS_Minor_Ver,\
            EXAMENUM_Control,UNDEFINED_DEVICE_ID,UNDEFINED_INIT_ORDER

VXD_LOCKED_CODE_SEG

;===========================================================================
;
;   PROCEDURE: EXAMENUM_Control
;
;   DESCRIPTION:
;   Device control procedure for the EXAMENUM DLVxD
;
;   ENTRY:
;   EAX = Control call ID
;
;   EXIT:
;   If carry clear then
;       Successful
;   else
;       Control call failed
;
;   USES:
;   EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;============================================================================

BeginProc EXAMENUM_Control
    Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, EXAMENUM_Init, sCall
    Control_Dispatch SYS_DYNAMIC_DEVICE_EXIT, EXAMENUM_Exit, sCall
    Control_Dispatch PNP_NEW_DEVNODE, EXAMENUM_NewDevNode, sCall, <ebx, edx>
    clc
    ret
EndProc EXAMENUM_Control

VXD_LOCKED_CODE_ENDS
    end
