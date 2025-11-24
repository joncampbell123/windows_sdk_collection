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

Begin_Service_Table BlockDev

BlockDev_Service    BlockDev_Get_Version, LOCAL
BlockDev_Service    BlockDev_Register_Device, LOCAL
BlockDev_Service    BlockDev_Find_Int13_Drive, LOCAL
BlockDev_Service    BlockDev_Get_Device_List, LOCAL
BlockDev_Service    BlockDev_Send_Command, LOCAL
BlockDev_Service    BlockDev_Command_Complete, LOCAL
BlockDev_Service    BlockDev_Synchronous_Command, LOCAL

End_Service_Table   BlockDev

BD_Priv_Data_Size	EQU	30H
BD_Major_Ver	EQU	3
BD_Minor_Ver	EQU	0AH

BlockDev_Device_Descriptor	STRUC
BDD_Next	DD	?
BDD_Major_Version	DB	BD_Major_Ver
BDD_Minor_Version	DB	BD_Minor_Ver
BDD_Device_Type	DB	?
BDD_Int_13h_Number	DB	?
BDD_Flags	DD	?
BDD_Name_Ptr	DD	?
BDD_Max_Sector	DD	2 DUP (?)
BDD_Sector_Size	DD	?
BDD_Num_Heads	DD	?
BDD_Num_Cylinders	DD	?
BDD_Num_Sec_Per_Track	DD	?
BDD_Sync_Cmd_Proc	DD	?
BDD_Command_Proc	DD	?
BDD_Hw_Int_Proc	DD	0
BDD_Reserved_BlockDev	DB	BD_Priv_Data_Size  DUP (?)
BlockDev_Device_Descriptor	ENDS
BDF_Int13_Drive	EQU	01H
BDF_Int13_Drive_Bit	EQU	0
BDF_Writeable	EQU	02H
BDF_Writeable_Bit	EQU	1
BDF_Removable	EQU	04H
BDF_Removable_Bit	EQU	2
BDF_Remote	EQU	08H
BDF_Remote_Bit	EQU	3
BDF_Serial_Cmd	EQU	10H
BDF_Serial_Cmd_Bit	EQU	4
BDF_Cache	EQU	20H
BDF_Cache_Bit	EQU	5
BDF_Bypass_Queue	EQU	800H
BDF_Bypass_Queue_Bit	EQU	11
BDF_VERSION_002	EQU	0400H
BDF_VERSION_002_BIT	EQU	10
BDT_360K_5_Inch_Floppy	EQU	00H
BDT_1200K_5_Inch_Floppy	EQU	01H
BDT_720K_3_Inch_Floppy	EQU	02H
BDT_Single_Dens_8_Inch	EQU	03H
BDT_Double_Dens_8_Inch	EQU	04H
BDT_Fixed_Disk	EQU	05H
BDT_Tape_Drive	EQU	06H
BDT_Other	EQU	07H

BlockDev_Command_Block	STRUC
BD_CB_Next	DD	?
BD_CB_Command	DW	?
BD_CB_Cmd_Status	DW	?
BD_CB_Flags	DD	?
BD_CB_Cmd_Cplt_Proc	DD	?
BD_CB_Sector	DD	2 DUP (?)
BD_CB_Count	DD	?
BD_CB_Buffer_Ptr	DD	?
BD_CB_Reserved_Client	DD	?
BD_CB_Reserved_BlockDev	DD	?
BD_CB_Reserved_FastDisk	DD	?
BD_CB_Spare	DD	5  DUP (?)
BD_CB_Req_Req_Handle	DD	?
BD_CB_Req_Vol_Handle	DD	?
BD_CD_SGD_Lin_Phys	DD	?
BD_CB_Num_SGDs	DB	?
BD_CB_Vol_Designtr	DB	?
BD_CB_reserved_1	DW	?
BlockDev_Command_Block	ENDS

BlockDev_Scatter_Gather	STRUC
BD_SG_Count	DD	?
BD_SG_Buffer_Ptr	DD	?
BlockDev_Scatter_Gather	ENDS
BDC_Read	EQU	0
BDC_Write	EQU	1
BDC_Verify	EQU	2
BDC_Cancel	EQU	3
BDC_Lock_Volume	EQU	09H
BDC_Unlock_Volume	EQU	0aH
BDS_Success	EQU	0
BDS_Success_With_Retries	EQU	1
BDS_Success_With_ECC	EQU	2
BDS_First_Error_Code	EQU	10H
BDS_Invalid_Sector_Number	EQU	10H
BDS_Canceled	EQU	11H
BDS_Cmd_In_Progress	EQU	12H
BDS_Invalid_Cmd_Ptr	EQU	13H
BDS_Media_Error	EQU	14H
BDS_Device_Error	EQU	15H
BDS_Invalid_Command	EQU	16H
BDS_No_Device	EQU	17H
BDS_No_Media	EQU	18H
BDS_Uncertain_Media	EQU	19H
BDS_Unrec_Error	EQU	1aH
BDS_Hw_Failure	EQU	1bH
BDS_Unformatted_Media	EQU	1cH
BDS_Memory_Error	EQU	1dH
BDS_Time_Out	EQU	1eH
BDS_Write_Protect	EQU	1fH
BDS_Not_Ready	EQU	20H
BDS_Busy	EQU	21H
BDCF_High_Priority	EQU	01H
BDCF_High_Priority_Bit	EQU	0
BDCF_Scatter_Gather	EQU	02H
BDCF_Scatter_Gather_Bit	EQU	1
BDCF_Dont_Cache	EQU	04H
BDCF_Dont_Cache_Bit	EQU	2
BDCF_Bypass_Voltrk_Bit	EQU	3
BDCF_Bypass_Voltrk	EQU	(1 SHL BDCF_Bypass_Voltrk_Bit)
BDCF_SWAPPER_IO	EQU	0020H
BDCF_Double_Buffer	EQU	0040H
BDCF_Double_Buffer_Bit	EQU	6
BDCF_Bypass_Queue_Bit	EQU	11
BDCF_Bypass_Queue	EQU	(1 SHL BDCF_Bypass_Queue_Bit)
BDCF_Logical_Start_Sector_Bit	EQU	19
BDCF_Logical_Start_Sector	EQU	(1 SHL BDCF_Logical_Start_Sector_Bit)
BD_SC_Get_Version	EQU	0000H
BD_SC_Err_Invalid_Cmd	EQU	0001H
BlockDev_API_Hw_Detect_Start	EQU	0001H
BlockDev_API_Hw_Detect_End	EQU	0002H
