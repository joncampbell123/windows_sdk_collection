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

MULTIMEDIA_OEM_ID	EQU	0440H
VTDAPI_DEVICE_ID	EQU	MULTIMEDIA_OEM_ID+2
VTDAPI_Get_Version	EQU	0
VTDAPI_Begin_Min_Int_Period	EQU	1
VTDAPI_End_Min_Int_Period	EQU	2
VTDAPI_Get_Int_Period	EQU	3
VTDAPI_Get_Sys_Time	EQU	4
VTDAPI_Timer_Start	EQU	5
VTDAPI_Timer_Stop	EQU	6
VTDAPI_Start_User_Timer	EQU	7
VTDAPI_Stop_User_Timer	EQU	8
VTDAPI_Get_System_Time_Selector	EQU	9
VTDAPI_Cleanup_Timers	EQU	10

VTDAPI_Get_Version_Parameters	STRUC
VTDAPI_Version	DD	?
VTDAPI_Min_Period	DD	?
VTDAPI_Max_Period	DD	?
VTDAPI_Get_Version_Parameters	ENDS

VTDAPI_Timer_Parameters	STRUC
VTDAPI_Timer_Period	DW	?
VTDAPI_Timer_Resolution	DW	?
VTDAPI_Timer_IPCS	DD	?
VTDAPI_Timer_Inst	DD	?
VTDAPI_Timer_Flags	DW	?
VTDAPI_Timer_Parameters	ENDS
ifndef TIME_ONESHOT
TIME_ONESHOT	EQU	0000H
TIME_PERIODIC	EQU	0001H
endif
ifndef TIMERR_BASE
TIMERR_BASE	EQU	96
TIMERR_NOERROR	EQU	0
TIMERR_NOCANDO	EQU	(TIMERR_BASE+1)
endif
