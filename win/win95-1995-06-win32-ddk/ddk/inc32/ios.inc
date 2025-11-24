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

IOS_Device_ID	EQU	00010H
IOS_Major_Ver	EQU	3H
IOS_Minor_Ver	EQU	0AH

Begin_Service_Table IOS













IOS_Service IOS_Get_Version, LOCAL













IOS_Service IOS_BD_Register_Device














IOS_Service IOS_Find_Int13_Drive













IOS_Service IOS_Get_Device_List



















IOS_Service IOS_SendCommand













IOS_Service IOS_BD_Command_Complete











IOS_Service IOS_Synchronous_Command













IOS_Service IOS_Register













IOS_Service IOS_Requestor_Service


















IOS_Service IOS_Exclusive_Access













IOS_Service IOS_Send_Next_Command



















IOS_Service IOS_Set_Async_Time_Out













IOS_Service IOS_Signal_Semaphore_No_Switch

















IOS_Service IOSIdleStatus














IOS_Service	IOSMapIORSToI24













IOS_Service	IOSMapIORSToI21

ifdef INITLOG










IOS_Service	PrintLog

endif

End_Service_Table IOS

IOSEA_LOCK_THREAD	EQU	(1 SHL 8)
