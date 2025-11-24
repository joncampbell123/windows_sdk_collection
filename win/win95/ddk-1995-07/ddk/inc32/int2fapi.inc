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

W386_API_Int	EQU	2FH
W386_Int_Multiplex	EQU	16H
W386_Get_Version	EQU	00H
W386_Old_Get_VMID_API	EQU	02H
W386_Startup	EQU	05H
W386_Exit	EQU	06H
W386_Device_Broadcast	EQU	07H
W386_Startup_Complete	EQU	08H
W386_Begin_Exit	EQU	09H
W386_Windows_ID	EQU	0AH
W386_TSR_Identify	EQU	0BH
W386_ROM_Detect	EQU	0CH
W386_WDEB	EQU	0DH
W386_Logo	EQU	0EH
W386_INT10	EQU	0FH
W386_Get_Shell	EQU	11H
W386_Get_BIOS_Data	EQU	12H
W386_Get_SYSDAT_Path	EQU	13H
W386_Set_SYSDAT_Path	EQU	14H
SAVE32_ID	EQU	1615H
W386_Release_Time	EQU	80H
W386_Begin_Critical	EQU	81H
W386_End_Critical	EQU	82H
W386_Get_Cur_VMID	EQU	83H
W386_Get_Device_API	EQU	84H
W386_Switch_And_Call	EQU	85H
W386_Test_Int31_Avail	EQU	86H
W386_Get_PM_Switch_Addr	EQU	87H
W386_Get_LDT_Base_Sel	EQU	88H
W386_Win_Kernel_Idle	EQU	89H
W386_DPMI_Extension	EQU	8AH
W386_Set_Focus	EQU	8BH
W386_Restart_Cmd	EQU	8CH
W386_Get_Win32_API	EQU	8DH
W386_VM_Title	EQU	8EH
W386_VM_Close	EQU	8FH
W386_Return_RMD	EQU	90H

Win386_Startup_Info_Struc	STRUC
SIS_Version	DB	04h,01h
SIS_Next_Ptr	DD	?
SIS_Virt_Dev_File_Ptr	DD	0
SIS_Reference_Data	DD	?
SIS_Instance_Data_Ptr	DD	0
SIS_Opt_Instance_Data_Ptr	DD	0
SIS_Reclaim_Data_Ptr	DD	0
Win386_Startup_Info_Struc	ENDS

Instance_Item_Struc	STRUC
IIS_Ptr	DD	?
IIS_Size	DW	?
Instance_Item_Struc	ENDS

Reclaim_Item_Struc	STRUC
RIS_Seg	DW	?
RIS_Paras	DW	?
RIS_HookTable	DD	?
RIS_Flags	DW	?
Reclaim_Item_Struc	ENDS
RIS_RECLAIM	EQU	0001H
RIS_RESTORE	EQU	0002H
RIS_DOSARENA	EQU	0004H

Reclaim_Hook_Table	STRUC
RHT_Num_Hooks	DW	?
RHT_Low_Seg	DW	?
RHT_High_Seg	DW	?
RHT_Flags	DW	?
Reclaim_Hook_Table	ENDS
RHT_DISABLED	EQU	0001H
RHT_UNHOOKED	EQU	0002H

Reclaim_Hook_Entry	STRUC
RHE_Low_Off	DW	?
RHE_High_Off	DW	?
Reclaim_Hook_Entry	ENDS

Return_RMD_Struc	STRUC
RRS_RMD	DD	?
RRS_Next_Ptr	DD	?
Return_RMD_Struc	ENDS
Win_Idle_Mouse_Busy	EQU	1
Win_Idle_Mouse_Busy_Bit	EQU	0

TSR_Info_Struc	STRUC
TSR_Next	DD	?
TSR_PSP_Segment	DW	?
TSR_API_Ver_ID	DW	100h
TSR_Exec_Flags	DW	0
TSR_Exec_Cmd_Show	DW	0
TSR_Exec_Cmd	DD	0
TSR_Reserved	DB	4 DUP (0)
TSR_ID_Block	DD	0
TSR_Data_Block	DD	0
TSR_Info_Struc	ENDS
TSR_WINEXEC	EQU	1
TSR_LOADLIBRARY	EQU	2
TSR_OPENDRIVER	EQU	4
