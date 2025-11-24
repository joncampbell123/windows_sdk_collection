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
TITLE SAMPLE.ASM - Windows/386 Configuration Manager DLVxD Sample
;******************************************************************************
;
;   Title:	SAMPLE.ASM - Windows/386 Config Manager DLVxD Sample
;
;   Version:	1.00
;
;   Date:	21-Mar-1993
;
;==============================================================================

	.386p

;******************************************************************************
;				I N C L U D E S
;******************************************************************************

	.xlist
	include vmm.inc
	include debug.inc
	include configmg.inc
	.list

;============================================================================
;		V I R T U A L   D E V I C E   D E C L A R A T I O N
;============================================================================

DECLARE_VIRTUAL_DEVICE	SAMPLE,PNPDRVS_Major_Ver,PNPDRVS_Minor_Ver,\
			SAMPLE_Control,,UNDEFINED_INIT_ORDER


VXD_LOCKED_CODE_SEG

;===========================================================================
;
;   PROCEDURE: SAMPLE_Control
;
;   DESCRIPTION:
;	Device control procedure for the SAMPLE DLVxD
;
;   ENTRY:
;	EAX = Control call ID
;
;   EXIT:
;	If carry clear then
;	    Successful
;	else
;	    Control call failed
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;============================================================================

BeginProc SAMPLE_Control

	Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, SAMPLE_Init, sCall, <0>
	Control_Dispatch DEVICE_INIT, SAMPLE_Init, sCall, <<OFFSET32 SAMPLE_DDB>>
	Control_Dispatch PNP_NEW_DEVNODE, SAMPLE_NewDevNode, sCall, <ebx, edx>
	Control_Dispatch PNP_NEW_DEVNODE, SAMPLE_NewDevNode, sCall, <ebx, edx>
	clc
	ret
EndProc	SAMPLE_Control

VXD_LOCKED_CODE_ENDS

	end
