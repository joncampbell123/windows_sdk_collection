PAGE 58,132
;******************************************************************************
TITLE WDCTRL.ASM -- Block Device VxD for Western Digital Controlers
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1990
;
;   Title:	WDCTRL.ASM -- Block Device VxD for Western Digital Controlers
;
;   Version:	3.10
;
;   Date:	20-Dec-1991
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   19-Dec-1991 RAL Decided to add some debug code to this puppy
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE OptTest.Inc
	INCLUDE BlockDev.Inc
	INCLUDE Int13.Inc
	INCLUDE VPICD.Inc
	INCLUDE WDLocal.Inc
	.LIST

IFDEF DEBUG

VxD_DATA_SEG

EXTRN  WD_Drive_80h_BDD:DWORD
EXTRN  WD_Drive_81h_BDD:DWORD
EXTRN  WD_Current_Command:DWORD
EXTRN  WD_Current_BDD:DWORD
EXTRN  WD_Pending_Command:DWORD
EXTRN  WD_Pending_BDD:DWORD
EXTRN  WD_Cur_Xfer_Ptr:DWORD
EXTRN  WD_Remaining_Sec_Count:DWORD
EXTRN  WD_Cur_Region_Count:DWORD
EXTRN  WD_Cur_Region_Ptr:DWORD
EXTRN  WD_Next_Start_Sector:DWORD
EXTRN  WD_Rem_Sec_This_Xfer:DWORD
EXTRN  WD_Accum_Status:BYTE
EXTRN  WD_Num_Retries:BYTE

EXTRN  WD_Total_Retries:DWORD
EXTRN  WD_Total_Failed_Reads:DWORD
EXTRN  WD_Total_Failed_Writes:DWORD

VxD_DATA_ENDS

;******************************************************************************

VxD_CODE_SEG

WD_Dump MACRO Field_Name
	mov	eax, [WD_&Field_Name]
	Trace_Out "&Field_Name = #EAX"
	ENDM

;******************************************************************************
;
;   WDCtrl_Debug_Query
;
;   DESCRIPTION:
;	This procedure displays debug information about
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc WDCtrl_Debug_Query

	Trace_Out "=============================================================================="
	Trace_Out "===  DEBUG STATUS INFORMATON FOR WDCTRL (FASTDISK) VIRTUAL DEVICE DRIVER   ==="
	Trace_Out "=============================================================================="
	Trace_Out " "
	mov	ecx, [WD_Current_Command]
	test	ecx, ecx
	jz	WD_DQ_No_Cmd_In_Progress
	mov	edi, [WD_Current_BDD]
	Trace_Out "Command #ECX in progress to BDD #EDI -- ", No_EOL
	cmp	[ecx.BD_CB_Command], BDC_Read
	je	WD_DQ_Read_In_Progress
	Trace_Out "Write to", No_EOL
	jmp	WD_DQ_Finish_IO_Stat
WD_DQ_Read_In_Progress:
	Trace_Out "Read from", No_EOL

WD_DQ_Finish_IO_Stat:
	mov	al, [edi.BDD_Int_13h_Number]
	Trace_Out " drive #AL"

	WD_Dump <Cur_Xfer_Ptr	    >
	WD_Dump <Remaining_Sec_Count>
	WD_Dump <Cur_Region_Count   >
	WD_Dump <Cur_Region_Ptr     >
	WD_Dump <Next_Start_Sector  >
	WD_Dump <Rem_Sec_This_Xfer  >
	mov	al, [WD_Accum_Status]
	Trace_Out "Accum_Status        = #AL"
	mov	al, [WD_Num_Retries]
	Trace_Out "Num_Retries         = #AL"

	mov	ecx, [WD_Pending_Command]
	jecxz	WD_DQ_Show_Global_Info
	mov	edi, [WD_Pending_BDD]
	Trace_Out "Queued command #ECX for BDD #EDI"
	jmp	WD_DQ_Show_Global_Info

WD_DQ_No_Cmd_In_Progress:
	Trace_Out "No command in progress"
WD_DQ_Show_Global_Info:
	Trace_Out " "
	mov	eax, [WD_Total_Retries]
	mov	ebx, [WD_Total_Failed_Reads]
	mov	ecx, [WD_Total_Failed_Writes]
	Trace_Out "#EAX total retries"
	Trace_Out "#EBX reads failed"
	Trace_Out "#ECX writes failed"

	clc
	ret

EndProc WDCtrl_Debug_Query


VxD_CODE_ENDS

ENDIF

	END
