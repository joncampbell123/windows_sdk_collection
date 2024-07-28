PAGE 58,132
;******************************************************************************
TITLE NETDEBUG.ASM -- Network debugging code for VNETBIOS.386
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	NETDEBUG.ASM -- Network debugging code for VNETBIOS.386
;
;   Version:	1.00
;
;   Date:	27-Sep-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   27-Sep-1989 RAL Original
;
;==============================================================================

	.386p


;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE NBLocal.Inc
	.LIST


;******************************************************************************
;				E Q U A T E S
;******************************************************************************

IFDEF DEBUG

VxD_DATA_SEG
	EXTRN	HCB_Segment:WORD
	EXTRN	VN_CB_Offset:DWORD

Command_Name_Table LABEL BYTE
	db	10h, "Call                   "
	db	11h, "Listen                 "
	db	12h, "Hang up                "
	db	14h, "Send                   "
	db	15h, "Receive                "
	db	16h, "Receive any            "
	db	17h, "Chain send             "
	db	20h, "Send datagram          "
	db	21h, "Receive datagram       "
	db	22h, "Send broadcast datagram"
	db	23h, "Receive broadcast dgram"
	db	30h, "Add name               "
	db	31h, "Delete name            "
	db	32h, "Reset                  "
	db	33h, "Adapter status         "
	db	34h, "Session status         "
	db	35h, "Cancel                 "
	db	36h, "Add group name         "
	db	70h, "Unlink                 "
	db	7Fh, "Install check          "
	db	00h, "UNKNOWN COMMAND        "

Name_Len = 23

VxD_DATA_ENDS

Dump_If_Set MACRO Flag
	LOCAL	Dont_Dump
	test	eax, HF_&Flag
	jz	SHORT Dont_Dump
	Trace_Out "&Flag ", No_EOL
Dont_Dump:
	ENDM

VxD_CODE_SEG

;******************************************************************************
;
;   VNETBIOS_Dump_Debug
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Dump_Debug

	mov	edi, [VN_CB_Offset]
	VMMcall Get_Cur_VM_Handle

VN_DD_VM_Loop:
	mov	ecx, [ebx.edi.VN_CB_Hook_List]
	test	ecx, ecx
	jnz	SHORT VN_DD_VM_Hook_Loop
	Trace_Out " "
	Trace_Out "VM #EBX has no hook control blocks allocated"
	jmp	VN_DD_Next_VM

VN_DD_VM_Hook_Loop:
	Trace_Out " "
	Trace_Out "HBC #ECX for virtual machine #EBX"
	Trace_Out " "
	mov	eax, [ecx.HCB_Next]
	Trace_Out "        Next = #EAX"

	mov	eax, [ecx.HCB_Flags]
	VMMcall Debug_Convert_Hex_Binary
	Trace_Out "       Flags = #EAX ", No_EOL
	mov	eax, [ecx.HCB_Flags]
	Dump_If_Set Wait_For_IRET
	Dump_If_Set Wait_For_POST
	Dump_If_Set Wait_For_Sim_POST
	Dump_If_Set POST_Crit
	Dump_If_Set From_PM
	Dump_If_Set NCB_Active
	Trace_Out " "

	mov	eax, [ecx.HCB_Real_NCB]
	Trace_Out "    Real_NCB = ", No_EOL
	call	Dump_Seg_Off

	mov	eax, [ecx.HCB_NCB_Lin_Addr]
	Trace_Out "NCB_Lin_Addr = #EAX"

	mov	eax, [ecx.HCB_Buf1_Handle]
	Trace_Out " Buf1_Handle = #EAX"
	mov	eax, [ecx.HCB_Buf2_Handle]
	Trace_Out " Buf2_Handle = #EAX"

	movzx	eax, [ecx.NCB_Command]
	Trace_Out "     Command = #AL -- ", No_EOL
	btr	eax, 7
	jnc	SHORT Not_Async
	Trace_Out "No Wait ", No_EOL
Not_Async:
	mov	esi, OFFSET32 Command_Name_Table
Find_Name_Loop:
	cmp	BYTE PTR [esi], 0
	je	SHORT Print_Cmd_Name
	cmp	al, BYTE PTR [esi]
	je	SHORT Print_Cmd_Name
	add	esi, Name_Len+1
	jmp	Find_Name_Loop
Print_Cmd_Name:
	inc	esi
	mov	eax, Name_Len
	call	Print_String

	mov	al, [ecx.NCB_RetCode]
	Trace_Out "     RetCode = #AL"

	mov	al, [ecx.NCB_LSN]
	Trace_Out "         LSN = #AL"

	mov	al, [ecx.NCB_Num]
	Trace_Out "         Num = #AL"

	mov	eax, [ecx.NCB_Buffer_Ptr]
	Trace_Out "  Buffer_Ptr = ", No_EOL
	call	Dump_Seg_Off

	mov	ax, [ecx.NCB_Length]
	Trace_Out "      Length = #AX"

	movzx	eax, [ecx.NCB_Command]
	and	al, 1111111b
	cmp	al, 17h
	jne	SHORT Not_Chain_Send
	Trace_Out "  2nd_Buffer = ", No_EOL
	mov	eax, DWORD PTR [ecx.NCB_CallName]
	call	Dump_Seg_Off
	jmp	SHORT Dump_RTO

Not_Chain_Send:
	Trace_Out "    CallName = ", No_EOL
	lea	esi, [ecx.NCB_CallName]
	mov	eax, 16
	call	Print_String

	Trace_Out "        Name = ", No_EOL
	lea	esi, [ecx.NCB_Name]
	mov	eax, 16
	call	Print_String

Dump_RTO:
	mov	al, [ecx.NCB_RTO]
	Trace_Out "         RTO = #AL"
	mov	al, [ecx.NCB_STO]
	Trace_Out "         STO = #AL"

	mov	eax, [ecx.NCB_Post_Ptr]
	Trace_Out "   POST_Addr = ", No_EOL
	call	Dump_Seg_Off

	mov	al, [ecx.NCB_LanA_Num]
	Trace_Out "    LanA_Num = #AL"
	mov	al, [ecx.NCB_Cmd_Cplt]
	Trace_Out "    Cmd_Cplt = #AL"

	Trace_Out "    Reserved = ", No_EOL
	pushad
	lea	esi, [ecx.NCB_Reserved]
	mov	ecx, 14
Res_Dump_Loop:
	cld
	lodsb
	Trace_Out "#AL ", No_EOL
	loopd	Res_Dump_Loop
	popad
	Trace_Out " "


	VMMcall In_Debug_Chr
	jz	SHORT VN_DD_Exit

	mov	ecx, [ecx.HCB_Next]
	test	ecx, ecx
	jnz	VN_DD_VM_Hook_Loop

VN_DD_Next_VM:
	VMMcall Get_Next_VM_Handle
	VMMcall Test_Cur_VM_Handle
	jne	VN_DD_VM_Loop

VN_DD_Exit:
	ret

EndProc VNETBIOS_Dump_Debug


;******************************************************************************
;
;   Dump_Seg_Off
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Segment:Offset to print
;
;   EXIT:
;	EAX trashed
;
;   USES:
;
;==============================================================================

BeginProc Dump_Seg_Off

	push	ecx
	mov	ecx, eax
	shr	eax, 16
	Trace_Out "#AX:#CX"
	pop	ecx
	ret

EndProc Dump_Seg_Off


;******************************************************************************
;
;   Print_String
;
;   DESCRIPTION:
;
;   ENTRY:
;	ESI -> String to print
;	EAX = Number of characters to print
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Print_String

	pushad

	mov	ecx, eax
PS_Loop:
	cld
	lodsb
	VMMcall Out_Debug_Chr
	loopd	PS_Loop

	popad

	Trace_Out " "
	ret

EndProc Print_String



VxD_CODE_ENDS

ENDIF

	END
