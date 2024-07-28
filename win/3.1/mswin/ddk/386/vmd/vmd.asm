PAGE 58,132
;******************************************************************************
TITLE VMD - Virtual Mouse Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1988
;
;   Title:	VMD.ASM - Virtual Mouse Device
;
;   Version:	3.00
;
;   Date:	17-Nov-1988
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   17-Nov-1988 RAL Rewrite
;   09-Mar-1989 RAL Real mode stub won't load if no Int 33h driver installed
;   01-May-1989 RAL Broke apart VInt33.Asm and VMD.Asm -- VMD always loads
;   02-May-1989 RAL Convert IRQ 2 to IRQ 9
;   27-Feb-1991 RAL Fix for InPort running in data interrupt mode on IRQ 2
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE VPICD.Inc
	INCLUDE VCD.Inc
	.LIST

	Create_VMD_Service_Table EQU True

	INCLUDE VMD.Inc

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VMD, 3, 0, VMD_Control, VMD_Device_ID, VMD_Init_Order, VMD_API_Proc, VMD_API_Proc

;
;   Types returned in CH from int 33h call 36
;
VMD_Type_Undefined	EQU	0
VMD_Type_Bus		EQU	1
VMD_Type_Serial 	EQU	2
VMD_Type_InPort 	EQU	3
VMD_Type_PS2		EQU	4
VMD_Type_HP		EQU	5



;******************************************************************************
;			  L O C A L   D A T A
;******************************************************************************

VxD_LOCKED_DATA_SEG

	PUBLIC	VMD_Owner

VMD_IRQ_Handle	dd	?
VMD_Owner	dd	?
VMD_Virt_IRQ	db	False
VMD_Mouse_Type	db	0
VMD_IRQ_Number	db	-1
VMD_COM_Port	db	0

VMD_IRQ_Desc VPICD_IRQ_Descriptor <?,VPICD_Opt_Read_Hw_IRR, \
				   OFFSET32 VMD_Hw_INT,,OFFSET32 VMD_EOI>

VxD_LOCKED_DATA_ENDS




;******************************************************************************
;		   I N I T I A L I Z A T I O N	 C O D E
;******************************************************************************

VxD_ICODE_SEG

	EXTRN	Int33_Critical_Init:NEAR
	EXTRN	Int33_Init:NEAR
	EXTRN	Get_Mouse_Instance:NEAR
	EXTRN	Int33_VM_Init:NEAR

;******************************************************************************
;
;   VMD_Device_Init
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

BeginProc VMD_Device_Init

	mov	[VMD_Owner], ebx		; System VM owns mouse if any
	CallRet Int33_Init			; Init Int 33h driver if any

EndProc VMD_Device_Init

VxD_ICODE_ENDS



;******************************************************************************

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   VMD_Control
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

BeginProc VMD_Control

	Control_Dispatch Sys_Critical_Init, Int33_Critical_Init
	Control_Dispatch Device_Init, VMD_Device_Init
	Control_Dispatch Init_Complete, Get_Mouse_Instance
	Control_Dispatch Set_Device_Focus, <SHORT VMD_Set_Focus>
	Control_Dispatch Begin_Message_Mode, Int33_Begin_Message_Mode
	Control_Dispatch End_Message_Mode, Int33_End_Message_Mode
	Control_Dispatch VM_Init, Int33_VM_Init
	clc
	ret

EndProc VMD_Control

VxD_LOCKED_CODE_ENDS



VxD_CODE_SEG

	EXTRN	Int33_Create_VM:NEAR
	EXTRN	Int33_Update_State:NEAR
	EXTRN	Int33_Begin_Message_Mode:NEAR
	EXTRN	Int33_End_Message_Mode:NEAR
	EXTRN	Int33_API:NEAR

;******************************************************************************
;
;   VMD_Set_Focus
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;	EAX, EBX, ECX, Flags
;
;==============================================================================

BeginProc VMD_Set_Focus

	test	edx, edx			; Q: Critial set-focus?
	jz	SHORT VMD_SF_Focus_Change	;    Y: Change mouse too!
	cmp	edx, VMD_Device_ID		;    N: Q: Mouse?
	jne	SHORT VMD_SF_Exit		;	   N: Done!

VMD_SF_Focus_Change:
	xchg	ebx, [VMD_Owner]		; Get old/set new owner

	cmp	[VMD_IRQ_Number], -1		; Q: Mouse virtualized yet?
	je	SHORT VMD_SF_Exit		;    N: Nothing to do

	call	Int33_Update_State		; Update old owner
	push	ebx
	mov	ebx, [VMD_Owner]
	call	Int33_Update_State		; Update new owner
	pop	ebx

	movzx	esi, [VMD_COM_Port]
	test	esi, esi
	jnz	SHORT VMD_SF_COM_Mouse

	cmp	[VMD_Virt_IRQ], True
	jne	SHORT VMD_SF_Exit

	mov	eax, [VMD_IRQ_Handle]
;
;   For InPort mice we will always shove an interrupt into the new owner VM
;   to force it to wake the mouse back up if the previous owner was not
;   properly reading the inport data.
;
	cmp	[VMD_Mouse_Type], VMD_Type_Inport
	je	SHORT VMD_SF_Always_Set_Req

	VxDcall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_Virt_Req	; Q: Requested for old owner?
	jz	SHORT VMD_SF_Exit		;    N: Done!

VMD_SF_Always_Set_Req:
	VxDcall VPICD_Clear_Int_Request 	;    Y: Clear old owner's
	mov	ebx, [VMD_Owner]		;	And set new
	VxDcall VPICD_Set_Int_Request
	jmp	SHORT VMD_SF_Exit

VMD_SF_COM_Mouse:
	mov	eax, Set_Device_Focus
	mov	edx, VCD_Device_ID
	mov	ebx, [VMD_Owner]
	VMMcall System_Control

VMD_SF_Exit:
	clc
	ret

EndProc VMD_Set_Focus


;******************************************************************************
;			       S E R V I C E S
;******************************************************************************


BeginDoc
;******************************************************************************
;
;   VMD_Get_Version
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;	AH = Major version number
;	AL = Minor version number
;	Carry flag clear
;
;   USES:
;	EAX, Flags
;
;==============================================================================
EndDoc

BeginProc VMD_Get_Version, Service

	mov	eax, 300h
	clc
	ret

EndProc VMD_Get_Version



BeginDoc
;******************************************************************************
;
;   VMD_Set_Mouse_Type
;
;   DESCRIPTION:
;
;   ENTRY:
;	If EAX >= 0 then
;	    EAX = IRQ number
;	else (if negative)
;	    Abs(EAX) = Interrupt vector
;	ECX = Mouse type (see VMD.INC for mouse definitions)
;	If high bit of CL set, then DL = COM port (1 based)
;
;   EXIT:
;	If carry clear then
;	    Mouse virtualized
;	else
;	    Could not virtualize mouse
;	    EAX = Error code
;		  00 = Mouse already virtualized
;		  01 = Could not virtualize interrupt
;
;   USES:
;	EAX, Flags
;
;==============================================================================
EndDoc

BeginProc VMD_Set_Mouse_Type, Service

	cmp	[VMD_IRQ_Number], -1
	jne	VMD_SMT_2nd_Init

	pushad
	mov	[VMD_Mouse_Type], cl

	test	eax, eax
	jns	SHORT VMD_SMT_Have_IRQ
	neg	eax
	VxDcall VPICD_Convert_Int_To_IRQ
	jc	VMD_SMT_Cant_Virt_Int
VMD_SMT_Have_IRQ:
	cmp	al, 2
	jne	SHORT VHD_SMT_Valid_IRQ
	mov	al, 9
VHD_SMT_Valid_IRQ:
	mov	[VMD_IRQ_Number], al

	cmp	[VMD_Mouse_Type], VMD_Type_PS2
	je	DEBFAR VMD_SMT_Success

	cmp	[VMD_Mouse_Type], VMD_Type_Serial_w_port
	jne	short @F
	mov	[VMD_Mouse_Type], VMD_Type_Serial
	mov	al, dl				; al = COM device #
	jmp	short smt_call_vcd

@@:
	cmp	[VMD_Mouse_Type], VMD_Type_Serial
	jne	SHORT VMD_SMT_Virt_IRQ

;
;   Note:  This code assumes that COM1 is on IRQ4 and COM2 is on IRQ3.
;   It is not possible to have a serial mouse on COM3 or COM4 with this
;   code.
;
	sub	al, 5				; AL = -1 or -2
	neg	al				; al = COM device # (1 or 2)

smt_call_vcd:
	push	eax
	VxDcall VCD_Get_Version
	test	eax, eax
	pop	eax
	jz	SHORT VMD_SMT_Virt_IRQ

	mov	[VMD_COM_Port], al
	movzx	eax, al 			; EAX = # of COM mouse is on
	xor	edx, edx			; Port is globally owned
	VxDcall VCD_Set_Port_Global
IFDEF DEBUG
	jnc	SHORT VMD_SMT_Success
	Debug_Out 'VMD failed to set port #al global thru VCD'
ENDIF
	jmp	SHORT VMD_SMT_Success


VMD_SMT_Virt_IRQ:
	movzx	edi, [VMD_IRQ_Number]
	mov	[VMD_IRQ_Desc.VID_IRQ_Number], di
	mov	edi, OFFSET32 VMD_IRQ_Desc
	VxDcall VPICD_Virtualize_IRQ
	jc	SHORT VMD_SMT_Cant_Virt_Int
	mov	[VMD_IRQ_Handle], eax
	mov	[VMD_Virt_IRQ], True

VMD_SMT_Success:
	popad
	clc
	ret


;
;   Could not virtualize interrupt.  Return error.
;
VMD_SMT_Cant_Virt_Int:
	Debug_Out "VMD ERROR:  Unable to virtualize IRQ."
	mov	[VMD_IRQ_Number], -1
	popad
	xor	eax, eax
	inc	eax
	stc
	ret

;
;   Mouse already virtualized.	Return error for second init call.
;
VMD_SMT_2nd_Init:
	xor	eax, eax
	stc
	ret

EndProc VMD_Set_Mouse_Type


BeginDoc
;******************************************************************************
;
;   VMD_Get_Mouse_Owner
;
;   DESCRIPTION:
;
;   ENTRY:
;	None
;
;   EXIT:
;	EBX = Handle of VM with mouse focus
;
;   USES:
;	EBX, Flags
;
;==============================================================================
EndDoc

BeginProc VMD_Get_Mouse_Owner, Service

	mov	ebx, [VMD_Owner]
	ret

EndProc VMD_Get_Mouse_Owner



;******************************************************************************
;******************************************************************************


BeginDoc
;******************************************************************************
;
;   VMD_API_Proc
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
EndDoc

BeginProc VMD_API_Proc

	movzx	eax, [ebp.Client_AX]

	test	eax, eax
	jnz	SHORT VMD_API_Test_DOS_Mouse_Int
	mov	[ebp.Client_AX], 300h
VMD_API_Worked:
	and	[ebp.Client_Flags], NOT CF_Mask
	ret

;
;   Check for DOS mouse interface calls.  These calls all have AH = 0 and
;   are handled by Int33.Asm
;
VMD_API_Test_DOS_Mouse_Int:
	test	ah, ah
	jnz	SHORT VMD_API_Test_Set
	CallRet Int33_API

;
;   Check for setting the mouse driver version
;
VMD_API_Test_Set:
	cmp	ax, 100h
	jne	SHORT VMD_API_Invalid
	movzx	eax, [ebp.Client_BH]
	neg	eax
	movzx	ecx, [ebp.Client_BL]
	mov	dl, [ebp.Client_CL]
	VxDcall VMD_Set_Mouse_Type
	jnc	SHORT VMD_API_Worked


VMD_API_Invalid:
	or	[ebp.Client_Flags], CF_Mask
	ret

EndProc VMD_API_Proc

VxD_CODE_ENDS


VxD_LOCKED_CODE_SEG
;******************************************************************************
;	   H A R D W A R E   I N T E R R U P T	 R O U T I N E S
;******************************************************************************

;******************************************************************************
;
;   VMD_Hw_Int
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

BeginProc VMD_Hw_Int, High_Freq

	mov	ebx, [VMD_Owner]		; EBX = Current mouse owner
	VxDjmp	VPICD_Set_Int_Request

EndProc VMD_Hw_Int


;******************************************************************************
;
;   VMD_EOI
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

BeginProc VMD_EOI, High_Freq

	VxDCall VPICD_Phys_EOI
	VxDjmp	VPICD_Clear_Int_Request

EndProc VMD_EOI

VxD_LOCKED_CODE_ENDS

	END
