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
;   Title:	VMPoll.Inc - Public services for VMPoll
;
;   Version:	3.10
;
;==============================================================================

Begin_Service_Table VMPoll

VMPoll_Service	VMPoll_Get_Version, LOCAL
VMPoll_Service	VMPoll_Enable_Disable, LOCAL
VMPoll_Service	VMPoll_Reset_Detection, LOCAL	; New for version 3.10

IFNDEF	WIN31COMPAT
VMPoll_Service	VMPoll_Check_Idle, LOCAL	; New for version 4.00
ENDIF

End_Service_Table VMPoll

;
;   Int 2Fh call-out API when system is idle
;	AX = 1607h
;	BX = VMPoll_Device_ID
;	CX = VMPoll_Call_Out_Sys_Idle
;
;   If TSR or device driver wants to "eat" the idle call, they should
;   set AX = 0 and *not* chain to other Int 2Fh hooks.	Otherwise, chain.
;
VMPoll_Call_Out_Sys_Idle    EQU     0		; CX = 0
