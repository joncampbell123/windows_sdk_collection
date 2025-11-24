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

VTD_API_GET_VERSION	EQU	0000H
VTD_API_GET_TIME	EQU	0100H
VTD_API_GET_TIME_IN_CLOCK_TICKS	EQU	0
VTD_API_GET_TIME_IN_MS	EQU	1
VTD_API_GET_VM_EXEC_TIME	EQU	2
ifndef Not_VxD
Begin_Service_Table VTD, VxD
VTD_Service	 VTD_Get_Version, LOCAL
VTD_Service	 VTD_Update_System_Clock, LOCAL
VTD_Service	 VTD_Get_Interrupt_Period, LOCAL
VTD_Service	 VTD_Begin_Min_Int_Period, LOCAL
VTD_Service	 VTD_End_Min_Int_Period, LOCAL
VTD_Service	 VTD_Disable_Trapping, LOCAL
VTD_Service	 VTD_Enable_Trapping, LOCAL
VTD_Service	 VTD_Get_Real_Time, LOCAL
VTD_Service	 VTD_Get_Date_And_Time, LOCAL
VTD_Service	 VTD_Adjust_VM_Count, LOCAL
VTD_Service	 VTD_Delay, LOCAL
End_Service_Table VTD, VxD
endif
