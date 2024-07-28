;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988-1990
;
;   Title:	VMPoll.Inc - Public services for VMPoll
;
;   Version:	3.10
;
;   Date:	30-Nov-1988
;
;   Author:	ARR
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   30-Nov-1988 ARR Original
;   18-Jun-1991 RAL Added Reset_Detection service
;
;==============================================================================

Begin_Service_Table VMPoll

VMPoll_Service	VMPoll_Get_Version, LOCAL
VMPoll_Service	VMPoll_Enable_Disable, LOCAL
VMPoll_Service	VMPoll_Reset_Detection, LOCAL	; New for version 3.10

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
