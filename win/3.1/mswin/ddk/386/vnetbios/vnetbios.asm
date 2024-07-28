PAGE 58,132
;******************************************************************************
TITLE VNETBIOS.ASM - Virtual Network BIOS Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988
;
;   Title:	VNETBIOS - Virtual Network BIOS Device
;
;   Version:	3.00
;
;   Date:	25-Apr-1988
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   25-Apr-1988 RAL Complete rewrite of network (used to be VND)
;   27-Oct-1988 RAL Updated to use new Get_Crit_Section_Status
;   06-Nov-1988 RAL Redesigned to use remap instead of buffering
;   09-Mar-1989 RAL Real mode stub to not load if no redirector
;   05-Apr-1989 RAL This sucker actually works!
;   07-Apr-1989 RAL Added Int 2Fh API to get extended NETBIOS table
;   04-May-1989 RAL Works with 32-bit Build_Int_Stack_Frame
;   24-May-1989 RAL Test for NETBIOS instead of redir when loading
;   12-Jun-1989 RAL Fixed horrible page fault of doom bug
;   01-Sep-1989 RAL Make POSTS go to the RIGHT VM!!!
;   08-Oct-1989 RAL Fixed horrible InDOS assumption bug
;   29-Oct-1989 RAL Finished exit cancel code and documentation
;   04-Dec-1989 RAL Fixed critical section bug for non-hooked NCBs
;
;==============================================================================
;
;   This virtual device serves two purposes:  It buffers asynchronous
;   network requests and translates netbios calls for protected mode apps
;
;------------------------------------------------------------------------------

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.LIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE DOSMGR.Inc
	INCLUDE NBLocal.Inc
	INCLUDE Int2FAPI.Inc
	INCLUDE V86MMGR.Inc
	INCLUDE SYSINFO.Inc
	INCLUDE VDMAD.InC
	INCLUDE SHELL.Inc
	INCLUDE OptTest.Inc
	.XLIST

	Create_VNetBIOS_Service_Table EQU True

	INCLUDE VNETBIOS.Inc



;------------------------------------------------------------------------------


	PUBLIC	VN_CB_Offset

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VNETBIOS, 3, 0, VNETBIOS_Control, VNETBIOS_Device_ID, \
		       VNETBIOS_Init_Order

;******************************************************************************
;				E Q U A T E S
;******************************************************************************


VxD_ICODE_SEG
	EXTRN	NetBuff_Init_Complete:NEAR
VxD_ICODE_ENDS


VxD_CODE_SEG
	EXTRN	NetBuff_Map_Buffer:NEAR
	EXTRN	NetBuff_Release_Buffer:NEAR
	EXTRN	NetBuff_Alloc_HCB:NEAR
	EXTRN	NetBuff_Free_HCB:NEAR
IFDEF DEBUG
	EXTRN	VNETBIOS_Dump_Debug:NEAR
ENDIF
VxD_CODE_ENDS

VxD_IDATA_SEG
	EXTRN	DMA_Net_String:BYTE
	EXTRN	Min_Net_Buf_Str:BYTE
	EXTRN	Time_Out_Str:BYTE
	EXTRN	Map_Non_Buff_Str:BYTE
	EXTRN	Redir_Time_Out_String:BYTE
	EXTRN	Poll_Sync_Cmds_String:BYTE
VxD_IDATA_ENDS

VxD_DATA_SEG
	EXTRN	HCB_Segment:WORD
	EXTRN	VNETBIOS_Cant_Nuke_VM:BYTE
	EXTRN	VNETBIOS_Stupid_User:BYTE
	EXTRN	VNETBIOS_Big_Huge_Error:BYTE
	EXTRN	VNETBIOS_Min_Buff_Size:WORD
	EXTRN	VNETBIOS_Insuff_Buff_Msg:BYTE
	EXTRN	VNETBIOS_Hang_Err_Msg:BYTE
VxD_DATA_ENDS


;******************************************************************************


VxD_DATA_SEG

VN_CB_Offset		dd	?
VN_Free_Hook_List	dd	0
VN_Post_Hook_CSIP	dd	?
VN_Exit_Msg		dd	?

VN_Orig_PM_2A_Seg	dd	0
VN_Orig_PM_2A_Off	dd	0

VN_Crit_Time_Out	dd	5000
VN_Time_Out_Handle	dd	?
VN_Crit_VM_Handle	dd	?
VN_Crit_Count		dd	0

;
;   WARNING:  NEVER CHANGE THE VALUES OF THESE EQUATES.  They have been
;	      documented and are used by ISVs to specify their NetBIOS
;	      interface via an Int 2Fh call-out.
;
VN_Unknown		EQU	00h
VN_No_Map		EQU	04h
VN_Map_In		EQU	08h
VN_Map_Out		EQU	0Ch
VN_Map_In_Out		EQU	10h
VN_Chain_Send		EQU	14h
VN_Cancel		EQU	18h
VN_Buffer_In		EQU	1Ch
VN_Buffer_Out		EQU	20h
VN_Buffer_In_Out	EQU	24h
; WARNING:  Any command types added after Buffer_In_Out may require changes
;	    to the code that checks for conversion of sync commands to async
;	    commands.

;
;   Table of pointers to mapping procedures.
;
VN_Map_Proc_Table LABEL DWORD
	dd	OFFSET32 VNETBIOS_Unknown
	dd	OFFSET32 VNETBIOS_No_Map
	dd	OFFSET32 VNETBIOS_Map_In
	dd	OFFSET32 VNETBIOS_Map_Out
	dd	OFFSET32 VNETBIOS_Map_In_Out
	dd	OFFSET32 VNETBIOS_Chain_Send
	dd	OFFSET32 VNETBIOS_Cancel
	dd	OFFSET32 VNETBIOS_Buffer_In
	dd	OFFSET32 VNETBIOS_Buffer_Out
	dd	OFFSET32 VNETBIOS_Buffer_In_Out

VN_API_Def_Table LABEL BYTE
	db	VN_Unknown			; 00h - Probably 802.2 command
	db	VN_Unknown			; 01h - Probably 802.2 command
	db	VN_Unknown			; 02h - Probably 802.2 command
	db	VN_Unknown			; 03h - Probably 802.2 command
	db	VN_Unknown			; 04h - Probably 802.2 command
	db	VN_Unknown			; 05h - Probably 802.2 command
	db	VN_Unknown			; 06h - Probably 802.2 command
	db	VN_Unknown			; 07h - Probably 802.2 command
	db	VN_Unknown			; 08h - Probably 802.2 command
	db	VN_Unknown			; 09h - Probably 802.2 command
	db	VN_Unknown			; 0Ah - Probably 802.2 command
	db	VN_Unknown			; 0Bh - Probably 802.2 command
	db	VN_Unknown			; 0Ch - Probably 802.2 command
	db	VN_Unknown			; 0Dh - Probably 802.2 command
	db	VN_Unknown			; 0Eh - Probably 802.2 command
	db	VN_Unknown			; 0Fh - Probably 802.2 command
	db	VN_No_Map			; 10h - Call
	db	VN_No_Map			; 11h - Listen
	db	VN_No_Map			; 12h - Hang up
	db	VN_Unknown			; 13h -
	db	VN_Buffer_In			; 14h - Send
	db	VN_Buffer_Out			; 15h - Receive
	db	VN_Buffer_Out			; 16h - Receive any
	db	VN_Chain_Send			; 17h - Chain send
	db	VN_Unknown			; 18h -
	db	VN_Unknown			; 19h -
	db	VN_Unknown			; 1Ah -
	db	VN_Unknown			; 1Bh -
	db	VN_Unknown			; 1Ch -
	db	VN_Unknown			; 1Dh -
	db	VN_Unknown			; 1Eh -
	db	VN_Unknown			; 1Fh -
	db	VN_Buffer_In			; 20h - Send datagram
	db	VN_Buffer_Out			; 21h - Receive datagram
	db	VN_Buffer_In			; 22h - Send broadcast datagram
	db	VN_Buffer_Out			; 23h - Receive broadcast dgram
	db	VN_Unknown			; 24h -
	db	VN_Unknown			; 25h -
	db	VN_Unknown			; 26h -
	db	VN_Unknown			; 27h -
	db	VN_Unknown			; 28h -
	db	VN_Unknown			; 29h -
	db	VN_Unknown			; 2Ah -
	db	VN_Unknown			; 2Bh -
	db	VN_Unknown			; 2Ch -
	db	VN_Unknown			; 2Dh -
	db	VN_Unknown			; 2Eh -
	db	VN_Unknown			; 2Fh -
	db	VN_No_Map			; 30h - Add name
	db	VN_No_Map			; 31h - Delete name
	db	VN_No_Map			; 32h - Reset
	db	VN_Map_Out			; 33h - Adapter status
	db	VN_Map_Out			; 34h - Session status
	db	VN_Cancel			; 35h - Cancel
	db	VN_No_Map			; 36h - Add group name
	db	VN_Unknown			; 37h -
	db	VN_Unknown			; 38h -
	db	VN_Unknown			; 39h -
	db	VN_Unknown			; 3Ah -
	db	VN_Unknown			; 3Bh -
	db	VN_Unknown			; 3Ch -
	db	VN_Unknown			; 3Dh -
	db	VN_Unknown			; 3Eh -
	db	VN_Unknown			; 3Fh -
	db	VN_Unknown			; 40h -
	db	VN_Unknown			; 41h -
	db	VN_Unknown			; 42h -
	db	VN_Unknown			; 43h -
	db	VN_Unknown			; 44h -
	db	VN_Unknown			; 45h -
	db	VN_Unknown			; 46h -
	db	VN_Unknown			; 47h -
	db	VN_Unknown			; 48h -
	db	VN_Unknown			; 49h -
	db	VN_Unknown			; 4Ah -
	db	VN_Unknown			; 4Bh -
	db	VN_Unknown			; 4Ch -
	db	VN_Unknown			; 4Dh -
	db	VN_Unknown			; 4Eh -
	db	VN_Unknown			; 4Fh -
	db	VN_Unknown			; 50h -
	db	VN_Unknown			; 51h -
	db	VN_Unknown			; 52h -
	db	VN_Unknown			; 53h -
	db	VN_Unknown			; 54h -
	db	VN_Unknown			; 55h -
	db	VN_Unknown			; 56h -
	db	VN_Unknown			; 57h -
	db	VN_Unknown			; 58h -
	db	VN_Unknown			; 59h -
	db	VN_Unknown			; 5Ah -
	db	VN_Unknown			; 5Bh -
	db	VN_Unknown			; 5Ch -
	db	VN_Unknown			; 5Dh -
	db	VN_Unknown			; 5Eh -
	db	VN_Unknown			; 5Fh -
	db	VN_Unknown			; 60h -
	db	VN_Unknown			; 61h -
	db	VN_Unknown			; 62h -
	db	VN_Unknown			; 63h -
	db	VN_Unknown			; 64h -
	db	VN_Unknown			; 65h -
	db	VN_Unknown			; 66h -
	db	VN_Unknown			; 67h -
	db	VN_Unknown			; 68h -
	db	VN_Unknown			; 69h -
	db	VN_Unknown			; 6Ah -
	db	VN_Unknown			; 6Bh -
	db	VN_Unknown			; 6Ch -
	db	VN_Unknown			; 6Dh -
	db	VN_Unknown			; 6Eh -
	db	VN_Unknown			; 6Fh -
	db	VN_No_Map			; 70h - Unlink
	db	VN_Unknown			; 71h -
	db	VN_No_Map			; 72h - Ungerman Bass Register
	db	VN_Buffer_In			; 73h - Ungerman Bass SendNmc
	db	VN_No_Map			; 74h - Ungerman Bass Callniu
	db	VN_No_Map			; 75h - Ungerman Bass Calladdr
	db	VN_No_Map			; 76h - Ungerman Bass Listenaddr
	db	VN_Buffer_In			; 77h - Ungerman Bass SendPkt
	db	VN_Buffer_Out			; 78h - Ungerman Bass RcvPkt
	db	VN_Buffer_In			; 79h - Ungerman Bass SendAttn
	db	VN_Buffer_Out			; 7Ah - Ungerman Bass RcvAttn
	db	VN_Buffer_Out			; 7Bh - Ungerman Bass Listenniu
	db	VN_Buffer_Out			; 7Ch - Ungerman Bass RcvRaw
	db	VN_Buffer_In			; 7Dh - Ungerman Bass SendNmc2
	db	VN_Unknown			; 7Eh -
	db	VN_No_Map			; 7Fh - Install check

;
;   This variable is only used if we can not satisfy a network request because
;   we are out of buffer space.  It is used to inform the user of a minimum
;   required network buffer configuration.  Once the dialogue box is displayed
;   once this variable will be set to 0 so that we will not constantly anoy
;   the user with the same message.
;
VNETBIOS_Req_Buff   db	    ?			; # buffer pages requeste


;
;   If this is true then net requests that can't be buffered will be mapped
;
VN_Crit_If_No_Buff  db	    False

;
;   Time-out value for redirector transactions.
;
VNETBIOS_Redir_Time_Out db  0

;
;   If this is true then all commands will be converted to NoWait commands
;   by VNETBIOS.
;
VNETBIOS_Convert_Wait_Cmds db NOT False

VxD_DATA_ENDS


;******************************************************************************
;		    I N I T I A L I Z A T I O N   C O D E
;******************************************************************************



VxD_ICODE_SEG


;******************************************************************************
;
;   VNETBIOS_Sys_Critical_Init
;
;   DESCRIPTION:
;	The real mode portion of this device has already determined that
;	NetBIOS is present on this machine.  This code allocates a DMA
;	buffer on Micro Channel machines (32K default) and hooks the
;	protected mode Int 2Ah and 5Ch vectors.  Note that it will NEVER
;	chain to any other Int 5Ch protected mode hook since this code
;	expects to translate all NetBIOS calls to V86 mode.  Therefore, if
;	other devices want to hook the 5Ch vector, they should do so after
;	Sys_Critical_Init (Device_Init is a good time).
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	Carry clear (can not fail)
;
;   USES:
;	EAX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VNETBIOS_Sys_Critical_Init

	VMMcall Get_Machine_Info

	xor	eax, eax			; Assume we want 0 K DMA buffer

	TestReg ebx, GMIF_MCA			; Q: Micro channel?
	jz	SHORT VN_SCI_Test_Ini_File	; If not micro channel skip
	mov	eax, 32 			; else assume 32K DMA buffer

VN_SCI_Test_Ini_File:
	xor	esi, esi
	mov	edi, OFFSET32 DMA_Net_String
	VMMcall Get_Profile_Decimal_int

	add	eax, 3				; Round up to next highest page
	shr	eax, 2				; Convert to 4K pages
	jz	SHORT VH_SCI_Hook_Ints
	xor	ecx, ecx
	VxDCall VDMAD_Reserve_Buffer_Space

;
;   Hook Ints 2Ah and 5Ch in the protected mode interrupt VECTORS.  Note that
;   we will chain on Int 2Ah but if Int 5Ch was ever hooked, the previous
;   hook will be ignored.
;
VH_SCI_Hook_Ints:
	mov	eax, 2Ah
	VMMcall Get_PM_Int_Vector
	mov	[VN_Orig_PM_2A_Seg], ecx
	mov	[VN_Orig_PM_2A_Off], edx
	mov	esi, OFFSET32 VNETBIOS_PM_Int_2Ah
	VMMcall Allocate_PM_Call_Back
	jc	short VNSCI_Fail
	mov	ecx, eax
	movzx	edx, cx
	shr	ecx, 16
	mov	eax, 2Ah
	VMMcall Set_PM_Int_Vector

IFDEF DEBUG
	mov	eax, 5Ch
	VMMcall Get_PM_Int_Vector
	jz	SHORT VN_PM5C_OK
	Debug_Out "WARNING:  VNETBIOS about to hook Int 5Ch which is already hooked -- WONT CHAIN!"
VN_PM5C_OK:
ENDIF
	mov	esi, OFFSET32 VNETBIOS_PM_Int_5Ch
	VMMcall Allocate_PM_Call_Back
	jc	short VNSCI_Fail
	mov	ecx, eax
	movzx	edx, cx
	shr	ecx, 16
	mov	eax, 5Ch
	VMMcall Set_PM_Int_Vector
;
;   Done!  Return with carry clear to indicate success.
;
	clc
	ret

VNSCI_Fail:
	debug_out "Could not allocate PM call backs VNETBIOS_Sys_Critical_Init"
	VMMcall Fatal_Memory_Error

EndProc VNETBIOS_Sys_Critical_Init


;******************************************************************************
;
;   VNETBIOS_Device_Init
;
;   DESCRIPTION:
;	This procedure allocates a control block region for storing per-VM
;	data and queries the real NetBIOS handler for a table of extended
;	NetBIOS commands (see header for details).  It also reserves mapping
;	buffer space from the
;
;   ENTRY:
;	EBX = Current VM Handle
;
;   EXIT:
;	Carry clear to indicate success
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Device_Init

;
;   Get the redirector time-out value from System.Ini
;
	xor	eax, eax
	xor	esi, esi
	mov	edi, OFFSET32 Redir_Time_Out_String
	VMMcall Get_Profile_Decimal_Int
	cmp	eax, 120
	jbe	SHORT VN_DI_Reasonable_Time
	Debug_Out "Silly value #EAX specified for network time-out.  Setting to 2 minutes"
	mov	eax, 120
VN_DI_Reasonable_Time:
	add	al, al
	mov	[VNETBIOS_Redir_Time_Out], al

;
;   Get setting for automatic conversion of NoWait commands
;
	mov	al, [VNETBIOS_Convert_Wait_Cmds]
	xor	esi, esi
	mov	edi, OFFSET32 Poll_Sync_Cmds_String
	VMMcall Get_Profile_Boolean
	mov	[VNETBIOS_Convert_Wait_Cmds], al

	Assert_Cur_VM_Handle ebx

;
;   Allocate a structure in each VM control block.
;
	VMMCall _Allocate_Device_CB_Area, <<SIZE VNetBIOS_CB_Struc>, 0>
	test	eax, eax
	jnz	SHORT VN_DI_CB_OK
	Debug_Out "VNETBIOS ERROR:  Could not alloc control block data area space"
	VMMcall Fatal_Memory_Error

VN_DI_CB_OK:
	mov	[VN_CB_Offset], eax

;
;   Request information about network extensions
;
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec		; Only 8086 mode software!
	Assert_Client_Ptr ebp
	mov	[ebp.Client_AX], (W386_Int_Multiplex SHL 8h) + W386_Device_Broadcast
	mov	[ebp.Client_BX], VNETBIOS_Device_ID
	xor	eax, eax
	mov	[ebp.Client_CX], ax
	mov	[ebp.Client_ES], ax
	mov	[ebp.Client_DI], ax
	mov	eax, W386_API_Int
	VMMcall Exec_Int
	mov	ax, [ebp.Client_ES]
	or	ax, [ebp.Client_DI]
	jz	SHORT VNETBIOS_I_Table_Done

	Client_Ptr_Flat esi, es, di
	mov	edi, OFFSET32 VN_API_Def_Table
	mov	ecx, 128/4
	cld
	rep movsd

VNETBIOS_I_Table_Done:
	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	eax, 3*4			; Assume minimum of 3 buffers
	xor	esi, esi			; Windows/386 section
	mov	edi, OFFSET32 Min_Net_Buf_Str	; Look for this string
	VMMcall Get_Profile_Decimal_Int 	; EAX = Minimum network buffs
	add	eax, 3				; Round up to next 4K
	shr	eax, 2				; And convert to pages
	cmp	eax,000000FFh
	jbe	short RngOk1
	debug_out "NetHeapSize in pages #EAX is HUGE, sizing down to 0FFh VNETBIOS_Device_Init"
	mov	eax,000000FFh
RngOk1:
	push	eax

	VMMCall _GetLastV86Page

	inc	eax
	mov	edx,eax

	VMMCall _GetFirstV86Page

	sub	edx,eax 			; Size conventional in pages
	sub	edx,32				; Leave at least 128k
	jnc	short ChkRng2
	debug_out "Lastv86Page + 1 - FirstV86Page is < 128k VNETBIOS_Device_Init"
	mov	edx,3				; Back to default
ChkRng2:
	pop	eax
	cmp	eax,edx
	jbe	short RngOk2
	debug_out "NetHeapSize in pages #EAX is > #EDX bound, setting to bound VNETBIOS_Device_Init"
	mov	eax,edx
RngOk2:
	mov	[VNETBIOS_Req_Buff], al 	; Save this so in case we
						; can't satisfy a request we
						; can tell the user a reasonable
						; number to set

	mov	bx, 1				; 1 private buffer (for HCBs)
	mov	bh, al				; BH = Min shared buffers
	mov	cl, 10h 			; but prefer 10h pages
	xor	eax, eax			; No copy buffer needed
	VxDcall V86MMGR_Set_Mapping_Info	; Request the mapping area


;
;   Get critical section hang time-out value
;
	mov	eax, 5000			; Default to 5 seconds
	mov	ecx, 3				; 3 decimals
	xor	esi, esi			; From [386ENH] section
	mov	edi, OFFSET32 Time_Out_Str	; Find this string
	VMMcall Get_Profile_Fixed_Point 	; Get fixed point integer
	mov	[VN_Crit_Time_Out], eax

;
;   Get flag that allows user to force NetBIOS request that can't be buffered
;   to be forced into a critical section instead of failed.
;
	xor	eax, eax			; Default to false
	xor	esi, esi
	mov	edi, OFFSET32 Map_Non_Buff_Str
	VMMcall Get_Profile_Boolean
	mov	[VN_Crit_If_No_Buff], al

;
;   Return with carry clear to indicate success
;
	clc
	ret

EndProc VNETBIOS_Device_Init


;******************************************************************************
;
;   VNETBIOS_Init_Complete
;
;   DESCRIPTION:
;	We have to delay until this point before calling the V86MMGR to
;	allocate mapping regions.  This procedure first calls NetBuff to
;	intialize the free pool of Hook Control Blocks.  If that is successful
;	then we hook the V86 mode 2Ah and 5Ch interrupt chanins and allocate
;	a V86 call-back address to use for hooking POST call-backs.
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	If carry flag clear then
;	    Successful initialization
;	else
;	    VNETBIOS device could not initialize and we will not receive
;	    any more system control messages.
;
;   USES:
;	All registers and flags
;
;==============================================================================

BeginProc VNETBIOS_Init_Complete

;
;   Allocate the Hook Control Blocks first.
;   NOTE:  This call trashes all regs.
;
	call	NetBuff_Init_Complete		; Q: Everything OK?
IFDEF DEBUG
	jnc	short VNICD10
	debug_out "NetBuff_Init_Complete failed VNETBIOS_Init_Complete"
VNICD10:
ENDIF
	jc	SHORT VN_IC_Exit		;    N: Abort load!

;
;   Hook software interrupts 2Ah and 5Ch to watch network activity
;
	mov	eax, 2Ah
	mov	esi, OFFSET32 VNETBIOS_Int_2Ah
	VMMcall Hook_V86_Int_Chain

	mov	eax, 5Ch
	mov	esi, OFFSET32 VNETBIOS_Hook_NCB
	VMMcall Hook_V86_Int_Chain

;
;   Allocate a call-back CS:IP to hook NCB POST routines
;
	mov	esi, OFFSET32 VNETBIOS_Post_Hook
	VMMcall Allocate_V86_Call_Back
IFDEF DEBUG
	jnc	short VNICD20
	debug_out "Could not allocate V86 call back for post VNETBIOS_Init_Complete"
VNICD20:
ENDIF
	jc	SHORT VN_IC_Exit
	mov	[VN_Post_Hook_CSIP], eax

	clc					; Carry clear = Success!

VN_IC_Exit:
	ret


EndProc VNETBIOS_Init_Complete


VxD_ICODE_ENDS

;==============================================================================

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   VNETBIOS_Get_Version
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;	AH = Major version number
;	AL = Minor version number
;	EBX = Flags
;	      Bit 0 = 1 if protected mode netbios calls allowed
;
;   USES:
;	EAX, EBX, Flags
;
;==============================================================================

BeginProc VNETBIOS_Get_Version, Service

	mov	eax, 300h
	mov	ebx, 1
	clc
	ret

EndProc VNETBIOS_Get_Version



;******************************************************************************
;
;   VNETBIOS_Control
;
;   DESCRIPTION:
;	This is the main control procedure for the VNETBIOS device.
;
;   ENTRY:
;	EAX = System control message
;	Other registers may contain parameters.  See DDK for details of each
;	system control call.
;
;   EXIT:
;	Standard system control exit (usually, carry set indicates error)
;
;   USES:
;	See individual procedures for details.
;
;==============================================================================

BeginProc VNETBIOS_Control

	Control_Dispatch Sys_Critical_Init, VNETBIOS_Sys_Critical_Init
	Control_Dispatch Device_Init, VNETBIOS_Device_Init
	Control_Dispatch Init_Complete, VNETBIOS_Init_Complete
	Control_Dispatch VM_Terminate, <SHORT VNETBIOS_VM_Terminate>
	Control_Dispatch Sys_VM_Terminate, <SHORT VNETBIOS_Sys_VM_Terminate>
	Control_Dispatch VM_Not_Executeable, <SHORT VNETBIOS_VM_Not_Exec>
	Control_Dispatch Query_Destroy, VNETBIOS_Query_Destroy
	Control_Dispatch Close_VM_Notify, <SHORT VNETBIOS_Close_VM>
IFDEF DEBUG
	Control_Dispatch Debug_Query, VNETBIOS_Dump_Debug
ENDIF
	clc
	ret

EndProc VNETBIOS_Control

VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   VNETBIOS_VM_Terminate
;   VNETBIOS_Sys_VM_Terminate
;   VNETBIOS_VM_Not_Exec
;
;   DESCRIPTION:
;	This procedure attempts to cancel any network requests that are pending
;	for the dead VM.  It will be called from the VM_Terminate, Sys_VM_-
;	Terminate, and VM_Not_Executeable control calls.  Note that when a
;	VM crashes or is terminated by the user we will be forced to cancel
;	pending network requests in a VM other than the one in which they
;	were submitted since we will never get a "VM Terminate" control call.
;
;   ENTRY:
;	EBX = Handle of VM being terminated/destroyed
;
;   EXIT:
;	Carry clear
;
;   USES:
;	All registers and flags
;
;==============================================================================

BeginProc VNETBIOS_VM_Terminate

VNETBIOS_Close_VM LABEL NEAR
VNETBIOS_VM_Not_Exec LABEL NEAR
VNETBIOS_Sys_VM_Terminate LABEL NEAR

;
;   Set the "VM Dead" flag so that any future network activity such as POST
;   call-backs received for this VM will be ignored.
;
	mov	edi, [VN_CB_Offset]
	SetFlag [ebx+edi.VN_CB_Flags], VNF_VM_Dead

;
;   Test for any pending network requests.
;
	cmp	[ebx+edi.VN_CB_Hook_List], 0	; Q: Anything happening?
	je	VNETBIOS_VT_Quick_Exit		;    N: Good!

;
;   Set up initial warning message.  This message assumes that we will be
;   able to cancel any NCBs that are pending.  If we can not then we will
;   display a more forceful message.
;
	mov	[VN_Exit_Msg], OFFSET32 VNETBIOS_Stupid_User

;
;   Get a HCB to use to cancel this VM's pending requests.
;
	call	NetBuff_Alloc_HCB		; Q: Any free HCBs?
	jc	VNETBIOS_VT_Cant_Cancel 	;    N: Error!
						;    Y: EAX -> HCB we can use
;
;   Prepare to cancel the network requests by entering nested V86 execution.
;
	Push_Client_State
	VMMcall Begin_Nest_V86_Exec

;
;   Loop to cancel every pending request.
;
VNETBIOS_VT_Cancel_More:
	mov	ecx, [ebx+edi.VN_CB_Hook_List]	; ECX -> HCB to cancel
IFDEF DEBUG
	test	ecx, ecx
	jz	VNETBIOS_VT_Exit
ELSE
	jecxz	VNETBIOS_VT_Exit
ENDIF
	bts	[ecx.HCB_Flags], HF_Canceled_Bit; Q: Already canceled?
	jc	DEBFAR VNETBIOS_VT_Free_It	;    Y: Just free the HCB

	SetFlag [ecx.HCB_Flags], HF_Canceling	; HCB is busy and won't be freed

;
;   Set the client ES:BX to point to our HCB (in EAX) and point the buffer
;   pointer in our HCB to point to the HCB to cancel.
;
	push	edi
	movzx	edi, [HCB_Segment]
	mov	[eax.NCB_Buffer_Seg], di
	mov	[ebp.Client_ES], di
	shl	edi, 4
	push	ecx
	sub	ecx, edi
	mov	[eax.NCB_Buffer_Off], cx
	mov	ecx, eax
	sub	ecx, edi
	mov	[ebp.Client_BX], cx
	pop	ecx
	push	ecx
	mov	cl, [ecx.NCB_Lana_Num]
	mov	[eax.NCB_Lana_Num], cl
	pop	ecx
	pop	edi
	mov	[eax.NCB_Command], Cancel
;
;   Reflect an Int 2Ah or 5Ch to cancel the request.
;
	push	eax
	mov	eax, 5Ch
	TestMem [ecx.HCB_Flags], HF_From_Int2A
	jz	SHORT VNETBIOS_VT_Cancel_It
	mov	eax, 2Ah
	mov	[ebp.Client_AH], 1
VNETBIOS_VT_Cancel_It:
	VMMcall Exec_Int
	pop	eax

;
;   If the return code is 0 or 24h then we are happy campers.  Otherwise
;   we were unable to cancel the net request and we'll put up a VERY nasty
;   message for the user.
;
	cmp	[ebp.Client_AL], RC_Good
	je	SHORT VNETBIOS_VT_Free_It
	cmp	[ebp.Client_AL], RC_Pending
	je	SHORT VNETBIOS_VT_Free_It

	mov	[VN_Exit_Msg], OFFSET32 VNETBIOS_Big_Huge_Error
	Debug_Out "ERROR:  Could not cancel HCB for dead VM #EBX"

;
;   The HCB in ECX is now dead.  Free the HCB and unmap any buffers.
;
VNETBIOS_VT_Free_It:
	xchg	eax, ecx			; EAX = HCB to free
	call	VNETBIOS_Free_HCB		; Free HCB and buffer space
	mov	eax, ecx			; EAX = HCB used to cancel

	jmp	VNETBIOS_VT_Cancel_More 	; Cancel the rest

;
;   Done!  All HCBs have been freed.  Exit nested execution, restore the
;   client register state, and free the HCB we used to cancel.
;
VNETBIOS_VT_Exit:
	VMMcall End_Nest_Exec
	Pop_Client_State
	call	NetBuff_Free_HCB		; Free HCB in EAX

;
;   Tell the user that he/she has done something very stupid.
;
VNETBIOS_VT_Warn_User:
	mov	eax, MB_OK OR MB_ICONEXCLAMATION OR MB_SYSTEMMODAL OR MB_ASAP
	mov	ecx, [VN_Exit_Msg]
	xor	edi, edi
	xor	esi, esi
	VxDcall Shell_Sysmodal_Message

VNETBIOS_VT_Quick_Exit:
	clc
	ret

;
;   Wow!  Danger!  Absolutely NO free HCBs.  Just punt and warn the user.
;   This code frees HCBs without canceling them.
;
VNETBIOS_VT_Cant_Cancel:
	Debug_Out "ERROR:  Unable to cancel outstanding HCBs for VM #EBX"
	mov	[VN_Exit_Msg], OFFSET32 VNETBIOS_Big_Huge_Error
VNETBIOS_VT_Free_Loop:
	mov	eax, [ebx+edi.VN_CB_Hook_List]
	test	eax, eax
	jz	SHORT VNETBIOS_VT_Warn_User
	call	VNETBIOS_Free_HCB
	jmp	VNETBIOS_VT_Free_Loop

EndProc VNETBIOS_VM_Terminate


;******************************************************************************
;
;   VNETBIOS_Query_Destroy
;
;   DESCRIPTION:
;	This procedure is called when the user chooses to close a VM using
;	the Windows Close option (not exiting normally).  If there are any
;	HCB's in use by this VM then the destroy will be failed and a dialogue
;	box will be displayed informing the user that they should exit the
;	VM normally.
;
;   ENTRY:
;	EBX = Handle of VM that is about to be nuked
;
;   EXIT:
;	If carry clear then
;	    OK to nuke the VM
;	else
;	    Can't close VM now -- Dialogue box will be displayed
;
;   USES:
;	EAX, ECX, EDI, ESI, Flags
;
;==============================================================================

BeginProc VNETBIOS_Query_Destroy

	mov	ecx, [VN_CB_Offset]
	cmp	[ebx+ecx.VN_CB_Hook_List], 0	; Q: Any requests?
	je	SHORT VN_QD_OK_To_Nuke		;    N: OK to quit this VM
						;    Y: Tell user "NO!"
	mov	eax, MB_OK OR MB_ICONEXCLAMATION OR MB_SYSTEMMODAL
	mov	ecx, OFFSET32 VNETBIOS_Cant_Nuke_VM
	xor	edi, edi			; Standard caption
	xor	esi, esi			; No call back

	VxDcall SHELL_Message

	stc
	ret


VN_QD_OK_To_Nuke:
	clc
	ret

EndProc VNETBIOS_Query_Destroy


;******************************************************************************
;		       I N T E R R U P T   H O O K S
;******************************************************************************

;******************************************************************************
;
;   VNETBIOS_PM_Int_2Ah
;   VNETBIOS_PM_Int_5Ch
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

BeginProc VNETBIOS_PM_Int_2Ah

	mov	eax, 2Ah			; Interrrupt to reflect
	cmp	[ebp.Client_AH], 1		; Q: Special REDIR call
	je	SHORT VNETBIOS_Eat_PM_Int	;    Y: Hook it now
	cmp	[ebp.Client_AH], 4		; Q: Net BIOS command?
	je	SHORT VNETBIOS_Eat_PM_Int	;    Y: Pretend it's a 5Ch

	mov	ecx, [VN_Orig_PM_2A_Seg]
	mov	edx, [VN_Orig_PM_2A_Off]
	mov	eax, ecx
	or	eax, edx
	jz	SHORT VN_PI2A_Reflect_To_V86
	VMMjmp	Simulate_Far_Jmp

VN_PI2A_Reflect_To_V86:
	VMMcall Simulate_Iret
	VMMcall Begin_Nest_V86_Exec
	mov	eax, 2Ah
	VMMcall Exec_Int
	VMMjmp	End_Nest_Exec

EndProc VNETBIOS_PM_Int_2Ah

;==============================================================================

BeginProc VNETBIOS_PM_Int_5Ch

	mov	eax, 5Ch
VNETBIOS_Eat_PM_Int:
	VMMcall Simulate_Iret
	jmp	HNCB_Hook_Now

EndProc VNETBIOS_PM_Int_5Ch


;******************************************************************************
;
;   VNETBIOS_Int_2Ah
;
;   DESCRIPTION:
;	Since network commands can be executed through Int 2Ah, AH = 1, this
;	procedure will call the Int 5Ch code whenever a NETBIOS command is
;	issued through Int 2Ah.  Note that this makes it necessary for the
;	Int 5Ch layer to test every NCB to see if it has alreay been "hooked".
;
;   ENTRY:
;	EAX = Interrupt #
;	EBX = Current VM handle
;
;   EXIT:
;	Carry always set (reflect to next handler in Int 2Ah chain)
;
;   USES:
;	Same as VNETBIOS_Hook_NCB
;
;==============================================================================

BeginProc VNETBIOS_Int_2Ah, High_Freq

	cmp	[ebp.Client_AH], 1		; Q: Special REDIR call
	je	SHORT VNETBIOS_Hook_NCB 	;    Y: Hook it now
	cmp	[ebp.Client_AH], 4		; Q: Net BIOS command?
	je	SHORT VNETBIOS_Hook_NCB 	;    Y: Pretend it's a 5Ch
	stc					;    N: Just reflect interrupt
	ret

EndProc VNETBIOS_Int_2Ah




;******************************************************************************
;
;   VNETBIOS_Hook_NCB
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Interrupt #
;	EBX = Current VM handle
;	EBP -> Client register structure
;	Client_ES:BX -> NCB
;
;   EXIT:
;	CARRY FLAG IS ALWAYS CLEAR EVEN IF CALL FAILS
;	This procedure is used to hook the V86 Int 5Ch chain and it must return
;	with carry clear to indicate that the interrupt has been eaten.
;
;   USES:
;	All registers and flags.
;
;==============================================================================

BeginProc VNETBIOS_Hook_NCB, High_Freq

	Assert_Cur_VM_Handle ebx

	mov	si, [HCB_Segment]
	cmp	[ebp.Client_ES], si
	jne	SHORT HNCB_Not_Hooked
	cmp	[ebp.Client_BX], 1000h
	jae	SHORT HNCB_Not_Hooked

;
;   This interrupt has already been hooked by VNETBIOS.  Just reflect the int.
;
	stc
	ret


HNCB_Not_Hooked:
;
;   Point ESI at the NCB
;
	movzx	esi, [ebp.Client_ES]
	shl	esi, 4
	movzx	ecx, [ebp.Client_BX]
	add	esi, ecx

;
;   Commands 00h-0Fh are reserved by IBM for the 802.2 layer.  These are
;   not NetBIOS commands!  The only thing we want to do is to enter a
;   critical section around these Int 5Cs.  However, if the net translation
;   table entry for any of these commands is anything other than Unknown
;   then we will continue to map them as we would any other NCB.
;
	movzx	ecx, [esi.NCB_Command]		; ECX = Command byte of NCB
	and	ecx, NOT NoWait 		; Reset high bit
	cmp	ecx, 0Fh			; Q: Is this an 802.2 command?
	ja	SHORT HNCB_Test_Global		;    N: Map it normally
	cmp	[VN_API_Def_Table][ecx], VN_Unknown; Y: Q: Unknown command?
	je	SHORT HNCB_Dont_Hook		;	   Y: Don't hook it

;
;   See if this request is comming from "global" memory.  If it is then
;   don't hook the interrupt.
;
HNCB_Test_Global:
	push	eax
	VMMcall _TestGlobalV86Mem, <esi, <SIZE NCB_Struc>, 0>
	test	eax, eax
	pop	eax
	jz	HNCB_Hook_Now

;
;   The NCB resides in global V86 memory.  If we are in DOS then we will
;   make the potentially dangerous assumption that this call is from the
;   redirector.  This is important since the redir does HUGE receives
;   that we do not want to hook. Hooking these DOS transfers is to be
;   avoided at all cost because of the performance impact that it would
;   have. Accessing NET drives through DOS would become a lot slower.
;   Also we would have to get our hands on a large pile of hook buffer
;   because DOS transfers often range up very close to 64k. It is nice
;   to be able to avoid having to allocate this much hook buffer space
;   because of its impact on the low memory footprint of WIN386 for many
;   machines.
;
;   This assumption is dangerous because the BUFFER address in the NCB is
;   most probably LOCAL. This generally doesn't cause a problem for
;   reasonable network transports that don't wake up and do transfer
;   related stuff at TIMER interrupt time. The reason for this is that
;   non-TIMER GLOBAL interrupts are only simulated into the VM which
;   owns the critical section. The TIMER is generally the one GLOBAL
;   interrupt that doesn't work this way (it can't, because some
;   apps, and SYSTEM ROMs break if you do this). Thus if the NET
;   transport needs this data buffer to be accessable at TIMER
;   interrupt time, it needs to go into a critical section in its
;   timer interrupt handler so it will BLOCK if it gets called in
;   the wrong VM (one other than the one that owns the critical
;   section). See also TimerCriticalSection SYSTEM.INI variable.
;
	VxDcall DOSMGR_Get_DOS_Crit_Status	; Q: Is this from DOS?
	jnz	SHORT HNCB_Fix_Stupid_Timeout	;    Y: No need to hook it

;
;   We're not in DOS so we need to make sure the buffer is in global memory.
;
	movzx	edi, [esi.NCB_Command]		; EDI = Command
	and	edi, NOT NoWait 		; Turn off No Wait bit
	cmp	VN_API_Def_Table[edi],VN_No_Map ; Q: Does this cmd have a buff?
	je	SHORT HNCB_Dont_Hook		;    N: Don't hook it
	cmp	VN_API_Def_Table[edi],VN_Unknown; Q: Do we know about this one?
	jne	SHORT HNCB_Test_Buff_Addr	;    Y: Test buffer address
						;    N: Make our best guess!
	cmp	[esi.NCB_Buffer_Ptr], 0 	; Q: Is the buffer addr 0?
	je	SHORT HNCB_Dont_Hook		;    Y: ASSUME it has no buffer
	cmp	[esi.NCB_Length], 0		; Q: Is the length field 0?
	je	SHORT HNCB_Dont_Hook		;    Y: PROBABLY no buffer
						;    N: Assume it has a buffer
HNCB_Test_Buff_Addr:
	push	eax
	movzx	eax, [esi.NCB_Buffer_Seg]
	shl	eax, 4
	movzx	ecx, [esi.NCB_Buffer_Off]
	add	eax, ecx			; EAX = Linear addr of buffer
	movzx	ecx, [esi.NCB_Length]		; ECX = Size of buffer
	VMMcall _TestGlobalV86Mem, <eax, ecx, 0>; Q: Is this global memory?
	test	eax, eax
	pop	eax
	jz	SHORT HNCB_Hook_Now		;    N: Must hook it


;
;   Don't need to hook this interrupt BUT if the VNETBIOS has not entered
;   the critical section for this VM then we need to go into a crit section
;   now.
;
HNCB_Dont_Hook:
	Assert_Cur_VM_Handle ebx
	mov	edi, [VN_CB_Offset]		; Set EDI to control blk offset
	bts	[ebx+edi.VN_CB_Flags], VNF_In_Crit_Bit
	jc	SHORT HNCB_In_Crit_Section
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	xor	eax, eax
	mov	esi, OFFSET32 VNETBIOS_End_Critical
	VMMcall Call_When_VM_Returns
HNCB_In_Crit_Section:
	stc
	ret


;
;   Slime hack to force timeouts for the DOS redirector NCBs.
;   This is because the DOS redirector sets 0 timeouts (none) and this
;   causes the horrible beep of death and doom.
;
HNCB_Fix_Stupid_Timeout:
	cmp	[esi.NCB_RTO], 0		; Q: No time out?
	jne	HNCB_Dont_Hook			;    N: Done!
	mov	al, [VNETBIOS_Redir_Time_Out]	;    Y: Change it to this
	mov	[esi.NCB_RTO], al
	jmp	HNCB_Dont_Hook


;
;   2ND ENTRY POINT!  Jumped to from PM interrupt hooks!
;   At this point, EAX = Interrupt number
;   We must hook this network control block.
;
HNCB_Hook_Now:
	mov	edx, eax			; Save interrupt # in edx
	mov	edi, [VN_CB_Offset]		; Set EDI to control blk offset

	mov	cx, [ebp.Client_ES]
	shl	ecx, 16
	mov	cx, [ebp.Client_BX]		; ECX = NCB's CS:IP

;
;   Debug sanity check.  Scan all the Hook Control Blocks to make sure that
;   we don't have an ACTIVE HCB that points to the same NCB.
;
IFDEF DEBUG
	pushad
	mov	eax, [ebx+edi.VN_CB_Hook_List]
HNCB_Search_Loop:
	test	eax, eax
	jz	SHORT HNCB_Alloc_New
	cmp	[eax.HCB_Real_NCB], ecx
	je	SHORT HNCB_Test_Active
HNCB_Test_Next:
	mov	eax, [eax.HCB_Next]
	jmp	SHORT HNCB_Search_Loop

HNCB_Test_Active:
	TestMem [eax.HCB_Flags], HF_NCB_Active
	jz	SHORT HNCB_Test_Next
	Debug_Out "ERROR:  Hook control block #EAX active for NCB #ECX which is being re-used"
	jmp	SHORT HNCB_Test_Next

HNCB_Alloc_New:
	popad
ENDIF

;
;   ESI -> Network control block
;
	Client_Ptr_Flat esi, ES, BX

;
;   Make sure it's OK to do HOOKED network activity on this VM.  If the
;   VM is exiting then fail all requests.
;
	TestMem [ebx+edi.VN_CB_Flags], VNF_VM_Dead
	jnz	HNCB_Failed

;
;   It's OK to do this network request.  Try to allocate a Hook Control Block.
;
	call	NetBuff_Alloc_HCB		; Q: Did we get a HCB?
	jc	HNCB_Failed			;    N: Error

;
;   Initialize lots 'o fields in the Hook Control Block
;
	mov	[eax.HCB_Real_NCB], ecx 	; Save SEG:OFF of real NCB
	xor	ecx, ecx
	mov	[eax.HCB_Buf1_Handle], ecx	; No buffers
	mov	[eax.HCB_Buf2_Handle], ecx
	mov	[eax.HCB_VM_Handle], ebx	; This VM owns HCB
	mov	[eax.HCB_Flags], HF_Wait_For_IRET OR HF_NCB_Active
	mov	[eax.HCB_NCB_Lin_Addr], esi	; Save lin addr
	cmp	edx, 2Ah
	jne	SHORT HNCB_Test_Exec_Mode
	SetFlag [eax.HCB_Flags], HF_From_Int2A
HNCB_Test_Exec_Mode:
	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec
	jz	SHORT HNCB_Add_To_List
	SetFlag [eax.HCB_Flags], HF_From_PM

;
;   Add the HCB to the VM's list.
;
HNCB_Add_To_List:
	mov	ecx, eax			; ECX -> Hook Ctrl Block
	xchg	ecx, [ebx+edi.VN_CB_Hook_List]	; Set new head/Get old head
	mov	[eax.HCB_Next], ecx		; Link HCB into list

;
;   Copy the NCB into the HCB
;
	mov	edi, eax
	cld
	mov	ecx, SIZE NCB_Struc / 4
	rep movsd

;
;   Force Wait commands to be NoWait
;
	xor	esi, esi			; Indicate cmd not changed

	movzx	ecx, [eax.NCB_Command]
	test	cl, NoWait
	jnz	SHORT HNCB_Is_NoWait

	cmp	[VNETBIOS_Convert_Wait_Cmds], False
	je	SHORT HNCB_Map_Call

;
;   Make sure this command can be made async.  Check for data buffer.  All
;   commands that are BUFFER as opposed to MAP must, by definintion be
;   convertable to async commands.
;
	mov	cl, VN_API_Def_Table[ecx]
	cmp	cl, VN_Chain_Send
	je	SHORT HNCB_Convert_This_One
	cmp	cl, VN_Buffer_In
	jb	SHORT HNCB_Map_Call

HNCB_Convert_This_One:
	or	[eax.NCB_Command], NoWait	; Convert to NoWait command
	mov	esi, [eax.HCB_NCB_Lin_Addr]	; ESI -> NCB of converted cmd

;
;   All commands at this point are NoWait so hook the POST procedure
;
IFDEF DEBUG
	jmp	SHORT HNCB_Valid_Post
HNCB_Is_NoWait:
	TestMem [ebp.Client_EFlags], VM_Mask
	jnz	SHORT HNCB_Valid_Post
	cmp	WORD PTR [eax+2+NCB_Post_Ptr], 0
	je	SHORT HNCB_Valid_Post
	push	ecx
	movzx	ecx, WORD PTR [eax+2+NCB_Post_Ptr]
	lar	ecx, ecx
	pop	ecx
	jz	SHORT HNCB_Valid_Post
	Debug_Out "ERROR:  Invalid POST selector for HCB #EAX"
HNCB_Valid_Post:
ELSE
HNCB_Is_NoWait:
ENDIF

	SetFlag [eax.HCB_Flags], <HF_Wait_For_POST OR HF_Wait_For_Sim_POST>
	mov	ecx, [VN_Post_Hook_CSIP]
	mov	[eax.NCB_POST_Ptr], ecx

;
;   Okie Dokie, do the call now.  At this point EDX = # of int to reflect
;   EBX = Cur_VM_Handle, EAX -> HCB.  ESI = 0 if the command was *not* converted
;   from a wait command to a NoWait command.  If it was converted then ESI
;   points to the linear address of the original NCB.
;
HNCB_Map_Call:
	movzx	edi, [eax.NCB_Command]
	and	edi, NOT NoWait
	movzx	edi, VN_API_Def_Table[edi]
	push	esi
	call	VN_Map_Proc_Table[edi]
	pop	ecx				    ; Q: Did we convert this NCB
	jecxz	SHORT HNCB_Dont_Wait_For_Complete   ;	 N: Good!  Done!
						    ;	 Y: Poll til stat change
	cmp	[ecx.NCB_Cmd_Cplt], RC_Pending	; Q: Is command complete?
	jne	SHORT HNCB_Its_Done		;    Y: Done, set status
						;    N: Wait for it
	Push_Client_State
	VMMcall Begin_Nest_Exec
	VMMcall Enable_VM_Ints

;
;   This loop will allow events to be processed in the current VM and will
;   poll the status of the NCB until the command has completed.  Each time
;   through the loop it will release the time slice so that other VMs can
;   run.
;
HNCB_Service_Events_Loop:
	VMMcall Resume_Exec

	cmp	[ecx.NCB_Cmd_Cplt], RC_Pending
	jne	SHORT HNCB_Get_Out_Of_Nest_Exec

	VMMcall Release_Time_Slice
	jmp	HNCB_Service_Events_Loop

HNCB_Get_Out_Of_Nest_Exec:
	VMMcall End_Nest_Exec
	Pop_Client_State

HNCB_Its_Done:
	mov	al, [ecx.NCB_RetCode]
	mov	[ebp.Client_AL], al

HNCB_Dont_Wait_For_Complete:
HNCB_Abort:
	clc
	ret

;
;   Fail this call.  Either we could not allocate a HCB or the VM is about
;   to exit and we can't allow any more net requests.  It is assumed at this
;   point the ESI -> real NCB.
;
HNCB_Failed:
	mov	al, RC_Max_Cmd			; "Too many commands" error
	mov	[esi.NCB_RetCode], al
	mov	[esi.NCB_Cmd_Cplt], al
	mov	[ebp.Client_AL], al
	jmp	HNCB_Abort

EndProc VNETBIOS_Hook_NCB


;******************************************************************************
;
;   VNETBIOS_End_Critical
;
;   DESCRIPTION:
;	This procedure is called when a VM returns from a NetBIOS call that
;	was not hooked.  It exits the critical section and resets the
;	flag in the control block that indicates that we are in a network
;	critical section.
;
;   ENTRY:
;	EBX = Current VM handle
;
;   EXIT:
;	None
;
;   USES:
;	EBX
;
;==============================================================================

BeginProc VNETBIOS_End_Critical

	add	ebx, [VN_CB_Offset]		; EBX -> VNETBIOS crtl blk
	ClrFlag [ebx.VN_CB_Flags], VNF_In_Crit
	VMMjmp	End_Critical_Section		; Reset flag and end crit

EndProc VNETBIOS_End_Critical


;******************************************************************************
;
;   VNETBIOS_No_Map
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX -> HCB
;	EBX = Cur_VM_Handle
;	EDX = # of interupt to simulate
;	EBP -> Client register structure
;
;   EXIT:
;
;   USES:
;	ESI, EDI, Flags
;
;==============================================================================

BeginProc VNETBIOS_No_Map

IFDEF DEBUG
	Assert_Cur_VM_Handle ebx
	Assert_Client_Ptr    ebp
	cmp	edx, 5Ch
	je	SHORT VN_NM_Int_OK
	cmp	edx, 2Ah
	je	SHORT VN_NM_Int_OK
	Debug_Out "STRANGE!  VNETBIOS_No_Map called with EDX=#EDX (should be 5Ch or 2Ah)"
VN_NM_Int_OK:
ENDIF

	mov	edi, eax

	VMMcall Begin_Nest_V86_Exec

	push	ecx
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	pop	ecx

	push	DWORD PTR [ebp.Client_ES]
	push	[ebp.Client_EBX]

	movzx	esi, [HCB_Segment]
	mov	[ebp.Client_ES], si
	shl	esi, 4
	sub	eax, esi
	mov	[ebp.Client_BX], ax

	mov	eax, edx
	VMMcall Exec_Int

	mov	eax, edi

	pop	[ebp.Client_EBX]
	pop	DWORD PTR [ebp.Client_ES]

	VMMcall End_Critical_Section
	VMMcall End_Nest_Exec

	ClrFlag [eax.HCB_Flags], HF_Wait_For_IRET
	cmp	[ebp.Client_AL], 0
	je	SHORT VNETBIOS_NM_Update

	ClrFlag [eax.HCB_Flags], <(HF_Wait_For_Sim_POST OR HF_Wait_For_POST)>
	call	VNETBIOS_Clear_Post_Crit

VNETBIOS_NM_Update:
	CallRet Update_NCB

EndProc VNETBIOS_No_Map


;******************************************************************************
;
;   VNETBIOS_Unknown
;
;   DESCRIPTION:
;	This is a bad place to end up.	If we see a NetBIOS call that we
;	don't know how to translate, we come here.
;
;   ENTRY:
;	EAX -> HCB
;	EBX = Cur_VM_Handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================


BeginProc VNETBIOS_Unknown

IFDEF DEBUG
	pushad
	mov	cl, [eax.NCB_Command]
	Debug_Out "Warning:  Unknown network command #CL.  Hook Ctrl Blk = #EAX"
	popad
ENDIF
	cmp	[eax.NCB_Buffer_Ptr], 0 	; Q: Is the buffer addr 0?
	je	VNETBIOS_No_Map 		;    Y: ASSUME it has no buffer
	cmp	[esi.NCB_Length], 0		; Q: Is the length field 0?
	je	VNETBIOS_No_Map 		;    Y: PROBABLY no buffer
	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec
	jz	DEBFAR VNETBIOS_Buffer_In_Out
	lar	cx, [eax.NCB_Buffer_Seg]	; Q: Valid selector
	jz	DEBFAR VNETBIOS_Buffer_In_Out	;    Y: ASSUME it has a buffer
	Debug_Out "WARNING:  Buffer pointer in unknown command is non-zero with INVALID SELECTOR in buffer pointer"
	jmp	VNETBIOS_No_Map

EndProc VNETBIOS_Unknown


;******************************************************************************
;
;   VNETBIOS_Map_In_Out
;
;   DESCRIPTION:
;	This procedure is used for NCB APIs that have buffers but that
;	will return quickly enough that they do not need to be mapped into
;	global memory.	However, if the Int 5Ch came from a protected mode
;	application then we must map the buffers.
;
;   ENTRY:
;	EAX -> HCB
;	EBX = Cur_VM_Handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Map_In_Out

VNETBIOS_Map_In 	LABEL NEAR
VNETBIOS_Map_Out	LABEL NEAR

	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec
	jnz	SHORT VNETBIOS_Must_Buffer
	TestMem [eax.NCB_Command], NoWait
	jz	VNETBIOS_No_Map
	SetFlag [eax.HCB_Flags], HF_POST_Crit
	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	inc	[VN_Crit_Count] 		; In crit section again
	cmp	[VN_Crit_Count], 1		; Q: First crit call?
	ja	VNETBIOS_No_Map 		;    N: Ignore this one
						;    Y: Schedule time-out
	mov	[VN_Crit_VM_Handle], ebx
	mov	esi, [VN_CB_Offset]
	TestMem [ebx+esi.VN_CB_Flags], VNF_Timed_Out
	jnz	VNETBIOS_No_Map
	push	eax
	mov	eax, [VN_Crit_Time_Out]
	test	eax, eax
	jz	SHORT VNETBIOS_MIO_Timeout_Set
	mov	esi, OFFSET32 VNETBIOS_Hang_Error
	VMMcall Set_Global_Time_Out
	mov	[VN_Time_Out_Handle], esi
VNETBIOS_MIO_Timeout_Set:
	pop	eax
	jmp	VNETBIOS_No_Map

EndProc VNETBIOS_Map_In_Out


;******************************************************************************
;
;   VNETBIOS_Buffer_In_Out
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX -> HCB
;	EBX = Cur_VM_Handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Buffer_In_Out

VNETBIOS_Buffer_In	LABEL NEAR
VNETBIOS_Buffer_Out	LABEL NEAR

	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec
	jnz	SHORT VNETBIOS_Must_Buffer
	TestMem [eax.NCB_Command], NoWait
	jz	VNETBIOS_No_Map

VNETBIOS_Must_Buffer:
	movzx	ecx, [eax.NCB_Length]
	mov	esi, [eax.NCB_Buffer_Ptr]
	call	NetBuff_Map_Buffer
	jc	SHORT VNETBIOS_Buff_Error
	mov	[eax.NCB_Buffer_Ptr], edi
	mov	[eax.HCB_Buf1_Handle], esi

	CallRet VNETBIOS_No_Map

;
;   WARNING:  CAN BE JUMPED TO BY VNETBIOS_CHAIN_SEND!
;   Can't allocate a buffer.  If the user has specified that large requests
;   should be MAPPED then do so.  Otherwise, fail the command.
;
VNETBIOS_Buff_Error:
	cmp	[VN_Crit_If_No_Buff], False	; Q: User want this mapped?
	je	SHORT VNETBIOS_Fail_Command	;    N: Fail the command
	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec ; Y: Q: In PMode?
	jnz	SHORT VNETBIOS_Fail_Command	;	   Y: FAIL IT ANYWAY!
						;	   N: Map this one
	xor	esi, esi			; Must release buf2 in case this
	xchg	esi, [eax.HCB_Buf2_Handle]	; failed in 2nd part of chain
	call	NetBuff_Release_Buffer		; send command
	mov	esi, [eax.HCB_NCB_Lin_Addr]
	mov	esi, [esi.NCB_Buffer2_Ptr]	; Restore this hunk of the NCB
	mov	[eax.NCB_Buffer2_Ptr], esi	; in case it was a chain send
	jmp	VNETBIOS_Map_In_Out		; Map the command

;
;   Could not map the buffer in extended memory.  Return "too many commands"
;   and free the HCB.  We do this buy placing the error return condition in
;   the HCB, resetting all the busy flags and then calling Update_NCB.	Note
;   that any map handles that are valid will be freed by Update_NCB
;
;   At this point, ECX = Size of buffer request that failed.
;
VNETBIOS_Fail_Command:
	mov	[eax.NCB_RetCode], RC_Max_Cmd
	mov	[eax.NCB_Cmd_Cplt], RC_Max_Cmd
	mov	[ebp.Client_AL], RC_Max_Cmd

	ClrFlag [eax.HCB_Flags], <(HF_Wait_For_IRET OR HF_Wait_For_Sim_POST OR HF_Wait_For_POST)>
	call	VNETBIOS_Clear_Post_Crit

VNETBIOS_Buff_Err_Free_HCB:
	push	ecx
	call	Update_NCB
	pop	ecx

;
;   Tell the user that something bad has happened
;
	movzx	eax, [VNETBIOS_Req_Buff]
	test	eax, eax
	jz	SHORT VNETBIOS_Buff_Err_Exit

	lea	edi, [VNETBIOS_Min_Buff_Size]	; Set buffer size field of msg
	cld					; Clear dir flag for stosbs

	mov	[VNETBIOS_Req_Buff], 0
	add	ecx, 0FFFh			; Round up to next page
	shr	ecx, 12 			; and convert to pages
	add	eax, ecx
	shl	eax, 2				; Convert to K
	mov	esi, eax			; Save for later
	xor	edx, edx
	mov	ecx, 100
	idiv	ecx
	test	al, al				; Q: Is high digit 0?
	jz	SHORT VNETBIOS_Try_2nd_Char
	add	al, "0"
	stosb

VNETBIOS_Try_2nd_Char:
	mov	eax, edx
	aam					; AX = BCD buffer size
	add	eax, ("0" * 100h) + "0" 	; Convert to ASCII
	cmp	esi, 10 			; Q: Is # at least 10
	jb	SHORT VNETBIOS_Put_Digit	;    N: Set single digit
	xchg	ah, al
	stosb
	mov	al, ah

VNETBIOS_Put_Digit:
	stosb					; Slap the low digit into buff

	Assert_VM_Handle ebx

	mov	eax, MB_OK OR MB_ICONEXCLAMATION OR MB_SYSTEMMODAL
	mov	ecx, OFFSET32 VNETBIOS_Insuff_Buff_Msg
	xor	edi, edi			; Standard caption
	xor	esi, esi			; No call back

	VxDcall SHELL_Message

VNETBIOS_Buff_Err_Exit:
	ret

EndProc VNETBIOS_Buffer_In_Out



;******************************************************************************
;
;   VNETBIOS_Chain_Send
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

BeginProc VNETBIOS_Chain_Send

	TestMem [ebx.CB_VM_Status], VMStat_PM_Exec
	jnz	SHORT VN_CS_Must_Buffer
	TestMem [eax.NCB_Command], NoWait
	jz	VNETBIOS_No_Map

VN_CS_Must_Buffer:
	movzx	ecx, [eax.NCB_Length2]
	mov	esi, [eax.NCB_Buffer2_Ptr]
	call	NetBuff_Map_Buffer
	jc	VNETBIOS_Buff_Error
	mov	[eax.NCB_Buffer2_Ptr], edi
	mov	[eax.HCB_Buf2_Handle], esi

	jmp	VNETBIOS_Must_Buffer


EndProc VNETBIOS_Chain_Send



;******************************************************************************
;
;   VNETBIOS_Cancel
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

BeginProc VNETBIOS_Cancel

	mov	edi, [eax.NCB_Buffer_Ptr]

	mov	ecx, [VN_CB_Offset]
	mov	ecx, [ecx+ebx.VN_CB_Hook_List]
VN_C_Search_Loop:
	jecxz	SHORT VN_C_Not_Found
	cmp	[ecx.HCB_Real_NCB], edi
	je	SHORT VN_C_Found_Hook
	mov	ecx, [ecx.HCB_Next]
	jmp	VN_C_Search_Loop

VN_C_Found_Hook:
	TestMem [ecx.HCB_Flags], HF_Wait_For_Sim_Post
	jz	SHORT VN_C_Really_Cancel
	TestMem [ecx.HCB_Flags], HF_Wait_For_Post
	jnz	SHORT VN_C_Really_Cancel
	SetFlag [ecx.HCB_Flags], HF_Canceled
	ClrFlag [ecx.HCB_Flags], HF_NCB_Active
;
;   Now update the current NCB (the one doing the cancel)
;
	mov	[eax.NCB_RetCode], 0
	mov	[eax.NCB_Cmd_Cplt], 0
	mov	[ebp.Client_AL], 0
	ClrFlag [eax.HCB_Flags], <(HF_Wait_For_POST OR HF_Wait_For_Sim_POST)>
	call	VNETBIOS_Clear_Post_Crit
VH_C_Quick_Exit:
	ret


VN_C_Not_Found:
	Trace_Out "Can't find HCB for canceled NCB (Usually NOT an error)"
	jmp	VNETBIOS_No_Map


VN_C_Really_Cancel:
	movzx	esi, [HCB_Segment]
	mov	[eax.NCB_Buffer_Seg], si
	shl	esi, 4
	push	ecx
	sub	ecx, esi
	mov	[eax.NCB_Buffer_Off], cx
	pop	ecx

	SetFlag [ecx.HCB_Flags], HF_Canceling
	call	VNETBIOS_No_Map
	cmp	[ebp.Client_AL], 0
	jne	SHORT VN_C_Exit
	ClrFlag [ecx.HCB_Flags], <(HF_Canceling OR HF_Wait_For_Post OR HF_Wait_For_Sim_Post)>
	xchg	eax, ecx
	call	VNETBIOS_Clear_Post_Crit
	xchg	eax, ecx

VN_C_Free_Canceled_HCB:
	TestMem [ecx.HCB_Flags], HF_Busy_Mask
	jnz	SHORT VN_C_Exit
	xchg	eax, ecx
	call	VNETBIOS_Free_HCB
	xchg	eax, ecx

VN_C_Exit:
	ret

EndProc VNETBIOS_Cancel


;******************************************************************************
;
;   VNETBIOS_Post_Hook
;
;   DESCRIPTION:
;	This procedure is called for every POST to a HCB.  Note that it
;	DOES NOT update the NCB at this point even if the NCB does not have
;	a POST call-back.  This is so that we can leave the NCB unlocked.
;	If the NCB memory is not present then we must not touch it until the
;	priority VM event is processed.
;
;   ENTRY:
;	EBX = Current VM handle
;	EBP -> Client register structure
;	Client_ES:BX -> Hook Control Block
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Post_Hook

	VMMcall Simulate_Iret

	movzx	edx, [HCB_Segment]
IFDEF DEBUG
	cmp	dx, [ebp.Client_ES]
	je	SHORT VN_PH_Segment_OK
	Debug_Out "ERROR:  Invalid segment on POST!"
VN_PH_Segment_OK:
ENDIF
	shl	edx, 4
	movzx	esi, [ebp.Client_BX]
	add	edx, esi			; EAX -> Hook control block

	ClrFlag [edx.HCB_Flags], HF_Wait_For_POST

	mov	ebx, [edx.HCB_VM_Handle]	; We'll call this guy
	mov	edi, [VN_CB_Offset]		; as long has he's not dead
	TestMem [ebx+edi.VN_CB_Flags], VNF_VM_Dead
	jnz	SHORT VN_PH_Exit

;
;   Tell the time-slicer to run this VM again
;
	VMMcall Wake_Up_VM

	VMMcall Test_Cur_VM_Handle
	je	SHORT VNETBIOS_Post_Event

	mov	esi, OFFSET32 VNETBIOS_POST_Event
	mov	ecx, PEF_Wait_For_STI
	mov	eax, High_Pri_Device_Boost
	VMMjmp	Call_Priority_VM_Event

VN_PH_Exit:
	ret

EndProc VNETBIOS_Post_Hook


;******************************************************************************
;
;   VNETBIOS_POST_Event
;
;   DESCRIPTION:
;	Note that this procedure resets the HF_NCB_Active flag AFTER updating
;	the NCB.  Although Update_NCB may free the hook control block, it
;	is OK to reset the flag since the HCB would not have yet been removed
;	from the free list.
;
;   ENTRY:
;	EBX = Current VM Handle
;	EDX -> Hook Control Block
;	EBP -> Client register structure
;
;   EXIT:
;
;   USES:
;	EAX, EBX, ECX, EDX, ESI, EDI, Flags
;
;==============================================================================

BeginProc VNETBIOS_POST_Event

	VMMcall Wake_Up_VM			; Just to make sure we didn't
						; get put to sleep
	mov	edi, [VN_CB_Offset]
	TestMem [ebx+edi.VN_CB_Flags], VNF_VM_Dead
	jnz	VN_PE_Exit

	mov	eax, edx

	ClrFlag [eax.HCB_Flags], HF_Wait_For_Sim_Post
	call	VNETBIOS_Clear_Post_Crit

	TestMem [eax.HCB_Flags], HF_Canceled
	jnz	VN_PE_Canceled

	call	VNETBIOS_Update_Lin_Addr

	mov	ecx, [eax.HCB_NCB_Lin_Addr]
	TestMem [ecx.NCB_Command], NoWait	; Q: Was original cmd NoWait?
	jz	VN_PE_Dont_Sim_Post		;    N: Don't simulate POST
	mov	ecx, [ecx.NCB_Post_Ptr]
	test	ecx, ecx
	jz	VN_PE_Dont_Sim_Post

	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMcall Begin_Critical_Section
	Push_Client_State

	TestMem [eax.HCB_Flags], HF_From_PM
	jnz	SHORT VN_PE_Set_To_PMode
	VMMcall Begin_Nest_V86_Exec
	jmp	SHORT VN_PE_Exec_Nested
VN_PE_Set_To_PMode:
	VMMcall Begin_Nest_Exec

VN_PE_Exec_Nested:
	mov	esi, [eax.HCB_NCB_Lin_Addr]
	mov	esi, [esi.NCB_POST_Ptr]

	mov	edx, [eax.HCB_Real_NCB]
	mov	[ebp.Client_BX], dx
	shr	edx, 16
	mov	[ebp.Client_ES], dx
	mov	dl, [eax.NCB_RetCode]
	mov	[ebp.Client_AL], dl

	call	Update_NCB			; May FREE HCB but it is OK
	ClrFlag [eax.HCB_Flags], HF_NCB_Active	; to reset flag since HCB free

	mov	ecx, esi
	movzx	edx, cx 			; EDX = Offset to call
	shr	ecx, 10h			; CX = Segment to call

IFDEF DEBUG
	TestMem [ebp.Client_EFlags], VM_Mask
	jnz	SHORT VN_PE_No_Sel_Check
	push	ecx
	lar	ecx, ecx
	pop	ecx
	jz	SHORT VN_PE_No_Sel_Check
	Debug_Out "ERROR:  Invalid selector #CX in post call-back.  Will GP fault"
VN_PE_No_Sel_Check:
ENDIF


	VMMcall Build_Int_Stack_Frame

;
;   Disgusting, horrible slime hack for LANMAN bug.  The direction flag
;   must be clear when calling the post procedure or it will DIE.
;
	ClrFlag [ebp.Client_Flags], DF_Mask
	VMMcall Resume_Exec

	VMMcall End_Nest_Exec
	Pop_Client_State

	VMMjmp	End_Critical_Section

;
;   No need to simulate a POST for the NCB.
;
VN_PE_Dont_Sim_Post:
	call	Update_NCB
	ClrFlag [eax.HCB_Flags], HF_NCB_Active
VN_PE_Exit:
	ret

;
;   This HCB was canceled.  If it's not busy (waiting for IRET) then free it.
;   DO NOT UPDATE THE NCB since the cancel came AFTER the real POST but BEFORE
;   we posted to it (so we won't post to it).
;
VN_PE_Canceled:
	TestMem [eax.HCB_Flags], HF_Busy_Mask
	jnz	SHORT VN_PE_Exit
	CallRet VNETBIOS_Free_HCB

EndProc VNETBIOS_POST_Event


;******************************************************************************
;
;   VNETBIOS_Clear_Post_Crit
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

BeginProc VNETBIOS_Clear_Post_Crit

	btr	[eax.HCB_Flags], HF_Post_Crit_Bit
	jnc	SHORT VN_CPC_Exit

	dec	[VN_Crit_Count]
	jnz	SHORT VN_CPC_End_Crit
	push	esi
	xor	esi, esi
	xchg	esi, [VN_Time_Out_Handle]
	VMMcall Cancel_Time_Out
	pop	esi

VN_CPC_End_Crit:
	VMMcall End_Critical_Section

VN_CPC_Exit:
	ret

EndProc VNETBIOS_Clear_Post_Crit


;******************************************************************************
;
;   VNETBIOS_Hang_Error
;
;   DESCRIPTION:
;	This procedure is called when a time-out occurs in a VM that is in
;	a critical section.  We will only display this error once per VM.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VNETBIOS_Hang_Error

	Trace_Out "VNETBIOS WARNING:  Time-out for critical section ownership"

	mov	[VN_Time_Out_Handle], 0

;
;   If the VM causing the hang is exclusive and the focus VM and the only
;   VM scheduled then we won't print the message.
;
	VMMcall Get_Time_Slice_Info
	cmp	eax, 1
	ja	SHORT VNETBIOS_HE_Print_Msg
	cmp	ebx, [VN_Crit_VM_Handle]
	jne	SHORT VNETBIOS_HE_Print_Msg
	TestMem [ebx.CB_VM_Status], VMStat_Exclusive
	jnz	SHORT VNETBIOS_HE_Exit

VNETBIOS_HE_Print_Msg:
	mov	ebx, [VN_Crit_VM_Handle]
	mov	eax, MB_YESNO OR MB_ICONEXCLAMATION OR MB_SYSTEMMODAL
	mov	ecx, OFFSET32 VNETBIOS_Hang_Err_Msg
	xor	edi, edi			; Standard caption
	xor	esi, esi			; No call back

	VxDcall SHELL_Sysmodal_Message

	cmp	eax, IDYES
	jne	SHORT VNETBIOS_HE_Force_Focus
	xor	ecx, ecx
	mov	[VN_Crit_VM_Handle], ecx
	mov	[VN_Crit_Count], ecx
	VMMcall Nuke_VM
	jmp	SHORT VNETBIOS_HE_Exit

VNETBIOS_HE_Force_Focus:
	mov	esi, 00000001H			; Special "Must stay there" flag
	xor	edi, edi			; No "VM causing trouble" parm
	xor	edx, edx
	mov	eax, Set_Device_Focus
	VMMcall System_Control
	add	ebx, [VN_CB_Offset]
	SetFlag [ebx.VN_CB_Flags], VNF_Timed_Out

VNETBIOS_HE_Exit:
	ret

EndProc VNETBIOS_Hang_Error


;******************************************************************************
;
;   Update_NCB
;
;   DESCRIPTION:
;	If the HCB is still active (HF_NCB_Active bit is set) then this procedure
;	will copy the contents of the HCB to it's NCB.  If the HCB is no longer
;	busy (even if the HF_NCB_Active bit is set), the HCB will be freed.
;
;   ENTRY:
;	EAX -> HCB
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc Update_NCB

	pushad

	TestMem [eax.HCB_Flags], HF_NCB_Active
	jz	SHORT UNCB_Copied

	call	VNETBIOS_Update_Lin_Addr

	mov	edi, [eax.HCB_NCB_Lin_Addr]
	mov	esi, eax
	cld
	inc	esi			; Skip command
	inc	edi
	movsb				; Copy ret code, LSN, Num
	movsw
	add	esi, 4			; Skip the buffer pointer
	add	edi, 4
	movsw				; Copy the buffer length
	mov	bl, [eax.NCB_Command]
	and	ebx, 7Fh
	cmp	ebx, ChainSend
	je	SHORT UNCB_Chain_Send
	mov	ecx, 34 / 2		; Move CallName, Name, RTO, STO (words)
	jmp	SHORT UNCB_Move_Middle

UNCB_Chain_Send:
	add	esi, 16 		; Skip CallName
	add	edi, 16
	mov	ecx, 18 / 2		; Move Name, RTO, STO (words)

UNCB_Move_Middle:
	rep movsw
	add	esi, 4			; Skip the POST address
	add	edi, 4
	mov	ecx, 16 / 4
	rep movsd			; Copy LanA_Num, Cmd_Cplt, Reserved

UNCB_Copied:
	TestMem [eax.HCB_Flags], HF_Busy_Mask
	jnz	SHORT UNCB_Exit

;
;   This network control block is done!  Free our HCB and release any
;   buffer space we have allocated.
;
	call	VNETBIOS_Free_HCB

UNCB_Exit:
	popad
	ret


EndProc Update_NCB


;******************************************************************************
;
;   VNETBIOS_Free_HCB
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX -> Hook Control Block
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VNETBIOS_Free_HCB

	push	ebx
	push	esi

	mov	esi, [eax.HCB_Buf1_Handle]
	call	NetBuff_Release_Buffer
	mov	esi, [eax.HCB_Buf2_Handle]
	call	NetBuff_Release_Buffer

	mov	ebx, [eax.HCB_VM_Handle]
	add	ebx, [VN_CB_Offset]
	lea	ebx, [ebx+VN_CB_Hook_List-HCB_Next]
FHCB_Loop:
	cmp	[ebx.HCB_Next], eax
	je	SHORT FHCB_Found_HCB
	mov	ebx, [ebx.HCB_Next]
	test	ebx, ebx
	jnz	FHCB_Loop
	Debug_Out "VNETBIOS INTERNAL STATE ERROR"
	jmp	SHORT FHCB_Exit

FHCB_Found_HCB:
	mov	esi, [eax.HCB_Next]
	mov	[ebx.HCB_Next], esi

	call	NetBuff_Free_HCB

FHCB_Exit:
	pop	esi
	pop	ebx
	ret

EndProc VNETBIOS_Free_HCB


;******************************************************************************
;
;   VNETBIOS_Update_Lin_Addr
;
;   DESCRIPTION:
;	This procedure is called by the Update_NCB and Post_Event procedures
;	to make sure that a protected mode buffer has not moved in linear
;	memory.  If it has, a warning will be generated in debug mode and
;	the address will be reset to it's new linear address.
;
;   ENTRY:
;	EAX -> Hook Control Block
;
;   EXIT:
;	[eax.HCB_NCB_Lin_Addr] updated
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VNETBIOS_Update_Lin_Addr, High_Freq

	TestMem [eax.HCB_Flags], HF_From_PM
	jz	SHORT VN_ULA_Quick_Exit

	push	ebx
	push	ecx
	push	edx

	movzx	edx, WORD PTR [eax.HCB_Real_NCB+2]
	VMMcall Get_Cur_VM_Handle
	xchg	eax, ebx			; EAX = VM handle, EBX -> HCB
	VMMcall _SelectorMapFlat, <eax, edx, 0>

	movzx	edx, WORD PTR [ebx.HCB_Real_NCB]
	add	edx, eax

	mov	eax, ebx			; Restore original EAX

IFDEF DEBUG
	cmp	[eax.HCB_NCB_Lin_Addr], edx
	je	SHORT VN_ULA_Did_Not_Move
	Debug_Out "ERROR:  NCB pointed to by HCB #EAX changed linear address to #EDX"
VN_ULA_Did_Not_Move:
ENDIF
	mov	[eax.HCB_NCB_Lin_Addr], edx

	pop	edx
	pop	ecx
	pop	ebx

VN_ULA_Quick_Exit:
	ret

EndProc VNETBIOS_Update_Lin_Addr




VxD_CODE_ENDS


;******************************************************************************
;	  R E A L   M O D E   I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_REAL_INIT_SEG

;
;   Network Control Block for testing if NetBIOS is installed
;
Test_NCB    NCB_Struc <7Fh>

;******************************************************************************
;
;   VNETBIOS_Real_Init
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

BeginProc VNETBIOS_Real_Init

;
;   If another netbios is loaded then don't load -- Just abort our load
;
	test	bx, Duplicate_From_INT2F OR Duplicate_Device_ID
	jnz	SHORT VNETBIOS_RI_Abort_Load

;
;   Make sure the network is here by this logic:
;	If Int 5C vector = 0 then
;	    Abort load
;	else
;	    If NetBIOS command 7Fh returns "Invalid command" error (3) then
;		Load us
;	    else
;		Abort load
;
	mov	ax, 355Ch
	int	21h
	mov	ax, es
	or	ax, bx
	jz	SHORT VNETBIOS_RI_Abort_Load

	push	cs
	pop	es
	mov	bx, OFFSET Test_NCB
	xor	ax, ax
	inc	ax				; Make sure not valid return
	int	5Ch
	cmp	al, RC_Invalid
	je	SHORT VNETBIOS_RI_Load
	cmp	al, RC_Good
	je	SHORT VNETBIOS_RI_Load
	cmp	al, RC_Invalid_LANA
	je	SHORT VNETBIOS_RI_Load
	cmp	al, 40h 			; Everything above 40h is OK!
	jb	SHORT VNETBIOS_RI_Abort_Load

VNETBIOS_RI_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Device_Load_Ok
	ret

VNETBIOS_RI_Abort_Load:
	xor	bx, bx
	xor	si, si
	mov	ax, Abort_Device_Load + No_Fail_Message
	ret

EndProc VNETBIOS_Real_Init


VxD_REAL_INIT_ENDS

;------------------------------------------------------------------------------

	END	VNETBIOS_Real_Init
