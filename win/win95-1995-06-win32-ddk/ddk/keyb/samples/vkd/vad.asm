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

PAGE    56,132
;******************************************************************************
TITLE	PS/2 Virtual Auxiliary Device
;******************************************************************************
;
;   Title:      Virtual Auxiliary Device
;
;   Version:	2.01
;
;==============================================================================
;
;   The PS/2 mouse is virtualized at the BIOS level rather than the
;   hardware level.  While the I/O (actually only input) and interrupts
;   from the mouse are virtualized, the mouse will NEVER be reprogrammed
;   and the BIOS will NEVER be called in protected mode.  The following is
;   a basic outline of VAD:
;
;	In REAL MODE intialization the mouse is initilaized and the BIOS is
;	    set up to call a piece of common code in VMCOMMON.ASM
;	In PROTECTED MOVE initialization the common code is patched to be
;	    ~~~~~~~~
;	All BIOS Aux calls are intercepted and virtualized.  The Aux device
;	    is NEVER reprogrammed and many calls are simply ignored.
;	Mouse interrupts are intercepted and all mouse data is queued.
;	    Interrupts are simulated into the current pointing device owner.
;	When the BIOS calls TS_VAD_Far_Call the Virtual Aux Device determines
;	    wether the current VM has enabled the mouse and installed a
;	    far call.  If it has then the VMs far call routine is executed.
;
;******************************************************************************

	.386p


;******************************************************************************
;                             I N C L U D E S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE VPICD.Inc
	INCLUDE VKD.Inc
	INCLUDE VKDSys.Inc
	INCLUDE Debug.Inc
	INCLUDE VMD.Inc
	INCLUDE OPTTEST.Inc
	.LIST


VxD_RARE_CODE_SEG

VAD_Starts_Here LABEL BYTE

;
; We are putting lockable code in the RARE code segment.  (Go figure.)
; We need to disable might-block checking in this really gross way
; because vmm.h doesn't realize that the RARE code will not be pageable.
; So every procedure inside the lockable region which might be called
; at a time where paging is disallowed (e.g., hardware interrupt handlers)
; must be marked NO_PROLOG.

VxD_RARE_CODE_ENDS

;******************************************************************************
;                            E X T E R N A L S
;******************************************************************************

VxD_DATA_SEG
	EXTRN	VKD_irq_Handle:DWORD
VxD_DATA_ENDS

TRUE	EQU	VMM_TRUE

VK_VD_Queue_Out	MACRO S, V1, V2

	ENDM

;******************************************************************************
;				  E Q U A T E S
;******************************************************************************

VAD_Enabled_Bit     equ     00100000b		; Returned by status call
VAD_Max_Function    equ     7			; Maximum valid Aux BIOS fctn
VAD_Int_Time_Out    equ     100 		; 1/10 second time-out
VAD_Q_Length	    equ     32	      		; MUST BE A POWER OF 2!!!
VAD_Q_Mask	    equ     VAD_Q_Length - 1	; Mask for circular queue
						; ASSUMES LENGTH IS POWER OF 2


; Verify that queue length is a power of 2
SHIFT_VAL=0
REPT 8
	IF (VAD_Q_Length SHR SHIFT_VAL) AND 1
		.ERRNZ (VAD_Q_Length SHR SHIFT_VAL) AND 11111110b
	ENDIF
	SHIFT_VAL = SHIFT_VAL+1
ENDM




;******************************************************************************
;		     I N I T I A L I Z A T I O N   D A T A
;******************************************************************************

VxD_IDATA_SEG

VAD_IRQ_Desc VPICD_IRQ_Descriptor <0Ch,VPICD_OPT_CAN_SHARE,OFFSET32 VAD_INT, \
				   ,,, OFFSET32 VAD_IRET, VAD_Int_Time_Out>

VxD_IDATA_ENDS


;******************************************************************************
;			       L O C A L   D A T A
;******************************************************************************



VAD_CB_Struc STRUC
VAD_Far_Call	dd	?   ; CS:IP of VMs aux device routine
VAD_PMode_Addr	db	?   ; If TRUE then CS:IP is protected mode call-back
VAD_Enabled	db	?   ; If TRUE then aux is enabled
VAD_BallPt_On	db	?   ; If TRUE then ballpoint ID should be returned
VAD_CB_Struc ENDS

VxD_LOCKED_DATA_SEG

VAD_Next_I15_CS     dd	?
VAD_Next_I15_EIP    dd	?


VAD_Focus	dd	?

VAD_Queue       dw      VAD_Q_Length dup (?)
VAD_Q_Head      dw      0
VAD_Q_Tail      dw      0
VAD_Q_Count     dw      0


VAD_IRQ_Handle	dd	?
VAD_Requested	db	FALSE
VAD_IRET_Pending    db	FALSE
VAD_VM_Requesting   dd	?

VAD_Ballpoint_ID    dw	0

VxD_LOCKED_DATA_ENDS


VxD_DATA_SEG


VAD_CB_Offset	dd	?


VAD_Last_Status db	0

		PUBLIC	VAD_Exists

VAD_Exists	db	False
VAD_Initialized db	False

VAD_Internal_Int_15 db	False
VAD_Exiting	db	False

VAD_Toss_Count	db	0

VAD_Packet_Count    db  0

VxD_DATA_ENDS


VxD_CODE_SEG
	EXTRN VKD_Enable_Keyboard:NEAR
	EXTRN VKD_Disable_Keyboard:NEAR
VxD_CODE_ENDS

PAGE
;******************************************************************************
;		 V x D	 I N I T I A L I Z A T I O N   C O D E
;******************************************************************************

VxD_ICODE_SEG

VM_Int MACRO Int_Number

	mov	eax, Int_Number
	VMMcall Exec_Int
	bt	[ebp.Client_Flags], CF_Bit

	ENDM

;******************************************************************************
;
;   VAD_Device_Init
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM 1's handle
;
;   EXIT:
;       Carry clear if no error
;
;   USES:
;	Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_Device_Init, PUBLIC

	pushad

	VxDCall	VMD_Get_Version
	cmp	eax,0400h
	jae	VAD_Init_Done

	mov	[VAD_Focus], ebx

	mov	eax,OFFSET32 VAD_Starts_Here	; first byte
	mov	ecx,OFFSET32 VAD_Ends_Here-1	; last byte
	shr	eax,12				; first page
	shr	ecx,12				; Last page
	sub	ecx,eax				; #pages-1
	inc	ecx
	VMMCall	_LinPageLock,<eax,ecx,0>

	call	VKD_Disable_Keyboard

	Push_Client_State
	VMMcall Begin_Nest_Exec 		; Get ready for software ints
;
;   NOTE: VAD_Exists is alredy set to FALSE
;
	VM_Int	11h
	test	[ebp.Client_AL], 00000100b	; Q: Pointing device present?
	jz	VAD_SI_Test_Done		;    N: Done
						;    Y: Try to reset it
	mov	[ebp.Client_AX], 0C205h 	; Poiniting device init
	mov	[ebp.Client_BH], 3		; 3 byte packets
	VM_Int	15h
	jc	VAD_SI_Test_Done
	mov	[ebp.Client_AX], 0C201h 	; Reset pointing device
	VM_Int	15h
	jc	VAD_SI_Test_Done

;------------------------------------------------------------------------------
;
;   Determine if this is a ballpoint mouse.  If so, leave it in ballpoint
;   mode.
;
	mov	[ebp.Client_AX], 0C202h
	mov	[ebp.Client_BH], 0
	VM_Int	15h				; Set to 10hz
	jc	VAD_SI_Test_Done

	mov	[ebp.Client_AX], 0C204h 	; get mouse id
	VM_Int	15h
	jc	VAD_SI_Test_Done

	mov	ax,[ebp.Client_BX]
	mov	[VAD_BallPoint_ID],ax

;------------------------------------------------------------------------------

	mov	[ebp.Client_AX], 0C203h 	; Set resolution
	mov	[ebp.Client_BH], 3		; 8 counts per mm
	VM_Int	15h
	jc	VAD_SI_Test_Done
	mov	esi, OFFSET32 VAD_BIOS_Far_Call
	VMMcall Allocate_V86_Call_Back
	mov	[ebp.Client_AX], 0C207h 	; Far call initialization
	mov	[ebp.Client_BX], ax
	shr	eax, 10h
	mov	[ebp.Client_ES], ax
	VM_Int	15h
	jc	SHORT VAD_SI_Test_Done
	mov	[ebp.Client_AX], 0C206h 	; Extended commands
	mov	[ebp.Client_BH], 1		; Set scaling 1:1
	VM_Int	15h
	jc	SHORT VAD_SI_Test_Done
	mov	[ebp.Client_AX], 0C202h 	; Set sample rate
	mov	[ebp.Client_BH], 2		; 40 reports per second
	VM_Int	15h
	jc	SHORT VAD_SI_Test_Done

	mov	[ebp.Client_AX], 0C200h 	; Enable/Disable pointing device
	mov	[ebp.Client_BH], 1		; Enable
	VM_Int	15h
	jc	SHORT VAD_SI_Test_Done

	mov	[VAD_Exists], True	; Wow!	Initialization done!

VAD_SI_Test_Done:
	VMMcall End_Nest_Exec
	Pop_Client_State

	call	VKD_Enable_Keyboard

;
;   Hook the V86 interrupt chain EVEN IF THERE IS NO MOUSE ATTACHED!
;
	mov	eax, 15h
	mov	esi, OFFSET32 VAD_Int_15_Emulation
	VMMcall Hook_V86_Int_Chain

;
;   Now do things that are only important for machines that have VADs
;
	cmp	[VAD_Exists], True		; Q: Anything to initialize?
	jne	SHORT VAD_Init_Done		;    N: Done

	mov	edi, OFFSET32 VAD_IRQ_Desc      ; EDI -> IRQ descriptor
	VxDCall VPICD_Virtualize_IRQ		; Q: Can we virt IRQ?
	jc	SHORT VAD_Init_Err		;    N: Report init error
	mov	[VAD_IRQ_Handle], eax		;    Y: Save the IRQ handle

;
;   Hook the PM interrupt
;
	mov	eax, 15h
	VMMcall Get_PM_Int_Vector
	mov	[VAD_Next_I15_CS], ecx
	mov	[VAD_Next_I15_EIP], edx

	mov	esi, OFFSET32 VAD_PM_Int_15_Emulation
	VMMcall Allocate_PM_Call_Back

	movzx	edx, ax
	mov	ecx, eax
	shr	ecx, 16
	mov	eax, 15h
	VMMcall Set_PM_Int_Vector

;
;   Allocate a hunk 'o the control block for VAD to use
;
	VMMCall _Allocate_Device_CB_Area, <<SIZE VAD_CB_Struc>, 0>
	mov	[VAD_CB_Offset], eax

;
;   Initialize the system VM's state
;
	call	VAD_Create_VM

;
;   If the Virtual Mouse Device exists then inform it about the PS/2 type mouse
;
	VxDcall VMD_Get_Version 		; Get device version if exists
	test	eax, eax			; Q: Does it even exist?
	jz	SHORT VAD_Init_Done		;    N: Done
						;    Y: Set the appropriate type
	mov	ecx, VMD_Type_PS2
	mov	eax, 0Ch
	VxDcall VMD_Set_Mouse_Type

VAD_Init_Done:
	mov	[VAD_Initialized], True
        popad
	clc
        ret

VAD_Init_Err:
	Debug_Out "ERROR:  Could not virtualize PS/2 Aux device"
	popad
	stc
	ret

EndProc VAD_Device_Init

VxD_ICODE_ENDS


;******************************************************************************
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   VAD_Create_VM
;
;   DESCRIPTION:
;	Initialized the state of VM being created.
;
;   ENTRY:
;	EBX = Handle of VM being created
;
;   EXIT:
;	If carry clear then no error
;
;   USES:
;	Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_Create_VM, PUBLIC, VMCREATE

	cmp	[VAD_Exists], True		; Q: PS/2 Mouse?
	jne	SHORT VAD_I_Exit		;    N: Done

	push	ebx
	add	ebx, [VAD_CB_Offset]
	mov	[ebx.VAD_Far_Call], 0
	mov	[ebx.VAD_Enabled], FALSE
	pop	ebx
VAD_I_Exit:
	clc
        ret

EndProc VAD_Create_VM


;******************************************************************************
;
;   VAD_Destroy_VM
;
;   DESCRIPTION:
;	If an interrupt was requested for
;
;   ENTRY:
;       EBX = Handle of VM being destroyed
;
;   EXIT:
;       Carry set if error.
;
;   USES:
;       Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_Destroy_VM, PUBLIC, VMDESTROY

	cmp	[VAD_Exists], True		; Q: PS/2 Mouse?
	jne	SHORT VAD_D_Exit		;    N: Done

	cmp	[VAD_Requested], TRUE		; Q: Is an aux int requested?
	jne	SHORT VAD_D_Exit		;    N: Nothing to do

	cmp	ebx, [VAD_VM_Requesting]	; Q: Is it requested for dead VM
	jne	SHORT VAD_D_Exit		;    N: Alright

	push	eax
	mov	eax, [VAD_IRQ_Handle]
	VxDCall VPICD_Clear_Int_Request 	; Clear the reqest for dead VM
	mov	[VAD_Requested], FALSE		; Reset internal state
	mov	[VAD_IRET_Pending], FALSE	; Reset internal state
	push	ebx
	VMMcall Get_Sys_VM_Handle
	mov	[VAD_Focus], ebx
	pop	ebx
	call	VAD_Request_Int 		; And request for new owner
	pop	eax
	Debug_Out "PS/2 Aux interrupt WAS requested for destroyed VM"

VAD_D_Exit:
	clc
	ret

EndProc VAD_Destroy_VM


;******************************************************************************
;
;   VAD_System_Exit
;
;   DESCRIPTION:
;	Patches the AUX transer space jump back to a far return so that
;	if we get a mouse call in real mode before we have had a chance to
;	disable the mouse we will not choke and die.
;
;   ENTRY:
;	EBX = System VM handle
;
;   USES:
;	EAX, Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_System_Exit, PUBLIC, SYSEXIT

	Assert_VM_Handle ebx

	cmp	[VAD_Exists], True		; Q: PS/2 Mouse?
	jne	VAD_Exit_Done			;    N: Done

	mov	[VAD_Exiting], TRUE

	mov	[VAD_Q_Count], 0		; Clear all data from queue
	mov	eax, [VAD_IRQ_Handle]
	VxDcall VPICD_Clear_Int_Request

	mov	[VAD_Internal_Int_15], TRUE

	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], 0C201h 	; Reset pointing device function
	VM_Int	15h

	mov	eax,[VAD_CB_OFFSET]		; local data
	add	eax,ebx
	cmp	[eax.VAD_PMode_Addr],TRUE	; pmode ?
	jz	short VAD_Exit_NoEnable		; NO
	mov	eax,[eax.VAD_Far_Call]		; get far call address.
	or	eax,eax
	jz	short VAD_Exit_NoEnable
	mov	[ebp.Client_BX],ax
	shr	eax,10h
	mov	[ebp.Client_ES],ax
	mov	[ebp.Client_AX], 0C207h		; set handler
	VM_Int	15h
	mov	[ebp.Client_AX], 0C200h		; enable pointing device
	mov	[ebp.Client_BX], 0100h		; code to do it
	VM_Int	15h

VAD_Exit_NoEnable:

	VMMcall End_Nest_Exec
	Pop_Client_State

	mov	[VAD_Internal_Int_15], FALSE

VAD_Exit_Done:
	ret

EndProc VAD_System_Exit


;******************************************************************************
;
;   VAD_Set_Focus
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = handle of VM to set focus to
;	EDX = ID of device to set focus (0 for critical set focus)
;
;   EXIT:
;
;   USES:
;	Flags
;
;==============================================================================

BeginProc VAD_Set_Focus, PUBLIC, RARE, NO_PROLOG	; Dynamically locked

	push	eax
	push	ebx
	push	ecx

	test	edx, edx
	jz	SHORT VAD_SF_Focus_Change
	cmp	edx, VMD_Device_ID
	jne	SHORT VAD_SF_Exit
VAD_SF_Focus_Change:
	xchg	[VAD_Focus], ebx
	mov	eax, [VAD_IRQ_Handle]
	VxDcall VPICD_Get_Complete_Status
	test	ecx, VPICD_Stat_Virt_Dev_Req	; Q: Requested for old owner?
	jz	SHORT VAD_SF_Exit		;    N: Done!
	VxDcall VPICD_Clear_Int_Request 	;    Y: Clear old owner's

	mov	ebx, [VAD_Focus]		;	And set new
	VxDcall VPICD_Set_Int_Request
	mov	[VAD_VM_Requesting], ebx

	VK_VD_Queue_Out "Transfering AUX interrupt to VM #EBX"
	
VAD_SF_Exit:
	pop	ecx
	pop	ebx
	pop	eax
	ret

EndProc VAD_Set_Focus



;******************************************************************************
;
;   VAD_PM_Int_15_Emulation
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

BeginProc VAD_PM_Int_15_Emulation, RARE

	mov	eax, [ebp.Client_EAX]		; EAX = Client's EAX
	cmp	ah, 0C2h			; Q: PS/2 aux device call?
	je	SHORT VAD_PM_I15_Its_Ours	;    Y: Emulate it
	mov	ecx, [VAD_Next_I15_CS]
	mov	edx, [VAD_Next_I15_EIP]
	VMMjmp	Simulate_Far_Jmp

VAD_PM_I15_Its_Ours:
	VMMcall Simulate_Iret
	jmp	SHORT VAD_I15_Aux_Call

EndProc VAD_PM_Int_15_Emulation


;******************************************************************************
;
;   VAD_Int_15_Emulation
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = Interrupt # (15h)
;	EBX = VM handle
;
;   EXIT:
;
;   USES:
;	All registers and flags
;
;------------------------------------------------------------------------------


VxD_DATA_SEG
VAD_I15_Jump_Tab LABEL DWORD
	dd	OFFSET32 VAD_I15_Enable_Disable
	dd	OFFSET32 VAD_I15_Reset
	dd	OFFSET32 VAD_I15_Set_Sample_Rate
	dd	OFFSET32 VAD_I15_Set_Resolution
	dd	OFFSET32 VAD_I15_Read_Device_Type
	dd	OFFSET32 VAD_I15_Interface_Init
	dd	OFFSET32 VAD_I15_Extended_Cmds
	dd	OFFSET32 VAD_I15_Far_Call_Init
VxD_DATA_ENDS



BeginProc VAD_Int_15_Emulation, High_Freq, RARE

	mov	ax, [ebp.Client_AX]		; EAX = Client's EAX
	cmp	ah, 0C2h			; Q: PS/2 aux device call?
	je	SHORT VAD_I15_Aux_Call		;    Y: Emulate it
VAD_I15_Reflect:				;    N: Pass int to next handler
	stc
	ret

;
;   Pointing device Int 15h call
;
VAD_I15_Aux_Call:
	cmp	[VAD_Internal_Int_15], True	; Q: Is this from VAD?
	je	VAD_I15_Reflect 		;    Y: Let the real BIOS handle
						;    N: Emulate it
	mov	esi, [VAD_CB_Offset]		; ESI = Offset to local data
	add	esi, ebx			; ESI -> Local data
	movzx	eax, al 			; EAX = Subfunction #
	cmp	eax, VAD_Max_Function		; Q: Valid BIOS function?
	ja	SHORT VAD_I15_Invalid_Function	;    N: Return error
						;    Y: Jump to emulation code
	cmp	[VAD_Exists], True
	jne	SHORT VAD_I15_No_Mouse_Attached

	jmp	VAD_I15_Jump_Tab[eax*4]


;
;   Invalid function error
;
VAD_I15_Invalid_Function:
	or	[ebp.Client_Flags], CF_Mask
	mov	[ebp.Client_AH], 1
	clc					; Don't reflect this int
	ret

;
;   Invalid input error
;
VAD_I15_Invalid_Input:
	or	[ebp.Client_Flags], CF_Mask
	mov	[ebp.Client_AH], 2
	clc					; Don't reflect this int
	ret

;
;   No far call installed error
;
VAD_I15_No_Far_Call:
	or	[ebp.Client_Flags], CF_Mask
	mov	[ebp.Client_AH], 5
	clc					; Don't reflect this int
	ret

;
;   Return with no error
;
VAD_I15_No_Error:
	and	[ebp.Client_Flags], NOT CF_Mask
	mov	[ebp.Client_AH], 0
	clc					; Don't reflect this int
	ret

;
;   There is no AUX device attached to this computer.  Return the same
;   error codes that the IBM BIOS does.  These are:
;	Function    Error
;	   00h	     03h
;	   01h	     04h
;	   02h	     03h
;	   03h	     03h
;	   04h	     04h
;	   05h	     04h <--IBM CLEARS CARRY BUT I REFUSE TO BE THAT STUPID!
;	   06h	     04h
;	   07h	     NO ERROR!
;
;   In reality, the enable mouse call will return error 05h if there is no
;   far call installed but I don't really care.  Enough other calls should
;   have failed by the time they try that to prevent them from even trying
;   to enable the mouse.
;
;   At this point we know that the function number (in EAX) is valid.
;

VxD_DATA_SEG
VAD_Error_Table LABEL BYTE
	db	03h	; Function 00h
	db	04h	; Function 01h
	db	03h	; Function 02h
	db	03h	; Function 03h
	db	04h	; Function 04h
	db	04h	; Function 05h
	db	04h	; Function 06h
VxD_DATA_ENDS


VAD_I15_No_Mouse_Attached:
	cmp	eax, 7				; Q: Install far call?
	je	VAD_I15_No_Error		;    Y: Return success.
						;    N: Return error
	mov	al, VAD_Error_Table[eax]	; Suck out error code
	mov	[ebp.Client_AH], al		; Slap it in the client's AH
	or	[ebp.Client_Flags], CF_Mask	; And return with carry set
	clc					; Eat this Int 15h
	ret


;==============================================================================
;
;   Enable/Disable pointing device (AL = 0)
;
;   Clients BH = 0 -- Disable pointing device
;   Clients BH = 1 -- Enable pointing device
;
;   NOTE: The keyboard controler state is not changed and should not be
;	  changed since this may later be output to the physical controller.
;	  If the controler is virtualized we must make sure that the
;	  physical controler is NEVER set to disable the mouse.
;
;------------------------------------------------------------------------------

VAD_I15_Enable_Disable:
	cmp	[ebp.Client_BH], 1		; Q: Enable or disable?
	ja	VAD_I15_Invalid_Input		;    Neither -- Error
	jb	SHORT VAD_I15_Disable		;    Disable -- Jump
						;    Enable
	cmp	[esi.VAD_Far_Call], 0		; Q: Has VM installed driver?
	je	VAD_I15_No_Far_Call		;    N: BOZO - Won't enable
	mov	[esi.VAD_Enabled], TRUE 	;    Y: Enable aux
	jmp	VAD_I15_No_Error		;	And return with no error

VAD_I15_Disable:
	mov	[esi.VAD_Enabled], FALSE	; Disable aux
	jmp	VAD_I15_No_Error		; And return with no error


;==============================================================================
;
;   Reset pointing device (AL = 1)
;   Initialize pointing devicd interface (AL = 5)
;
;   These functions are essentially identical except that the interface
;   initialization does not return the device ID in BH.
;
;------------------------------------------------------------------------------

VAD_I15_Reset:
	mov	[ebp.Client_BH], 0		; Device ID = 0
	mov	[esi.VAD_BallPt_On], FALSE	; Not in ballpoint mode
VAD_I15_Interface_Init:
	mov	[esi.VAD_Far_Call], 0		; No far call intalled
	mov	[esi.VAD_Enabled], FALSE	; Aux is disabled
	jmp	VAD_I15_No_Error		; Everything is Okie Dokie


;==============================================================================
;
;   Set pointing device sample rate (AL = 2)
;
;   This call does not actually set the sample rate, but it "remembers" if
;   a VM ever sets the
;
;------------------------------------------------------------------------------

VAD_I15_Set_Sample_Rate:
	cmp	[ebp.Client_BH], 0
	jne	VAD_I15_No_Error
	mov	[esi.VAD_BallPt_On], TRUE
	jmp	VAD_I15_No_Error


;==============================================================================
;
;   Set pointing device resolution (AL = 3)
;
;   We ignore this calls!!!
;
;------------------------------------------------------------------------------

VAD_I15_Set_Resolution EQU VAD_I15_No_Error


;==============================================================================
;
;   Read pointing device type (AL = 4)
;
;   A very stupid call that we emulate perfectly.  Returns BH = 0
;
;------------------------------------------------------------------------------

VAD_I15_Read_Device_Type:
	mov	[ebp.Client_BH], 0		; Device ID = 0
	cmp	[esi.VAD_BallPt_On], TRUE	; Q: Is ballpoint mode enabled?
	jne	VAD_I15_No_Error		;    N: Device ID is 0
	mov	ax,[VAD_BallPoint_ID]		;    Y: Return BallPoint ID
	mov	[ebp.Client_BX],ax		;	in BX
	jmp	VAD_I15_No_Error		; No error


;==============================================================================
;------------------------------------------------------------------------------

VAD_I15_Extended_Cmds:
	mov	cl, [ebp.Client_BH]
	cmp	cl, 2				; Q: Valid subfunction?
	ja	VAD_I15_Invalid_Input		;    N: Return error
	je	VAD_I15_No_Error		; Any resolution for non-debug
	test	cl, cl				; Q: Return status function?
	jnz	VAD_I15_No_Error		;    N: 1:1 resolution OK
						;    Y: Return status to VM
	cmp	[esi.VAD_Enabled], TRUE 	; Q: Aux enabled?
	jne	SHORT VAD_I15_EC_Disabled	;    N: Don't set bit
	or	cl, VAD_Enabled_Bit		;    Y: Set enabled status
VAD_I15_EC_Disabled:
	mov	dl, [VAD_Last_Status]		; DL = Last status returned
	add	dl, 10b 			; Tricky -- See header
	and	dl, 101b			; for description
	or	cl, dl
	mov	[ebp.Client_BL], cl
	mov	[ebp.Client_CL], 03h		; 8 counts per mm
	mov	[ebp.Client_DL], 28h		; 40 reports per second
	jmp	VAD_I15_No_Error


;==============================================================================
;
;
;
;------------------------------------------------------------------------------

VAD_I15_Far_Call_Init:
	mov	cx, [ebp.Client_ES]
	shl	ecx, 10h
	mov	cx, [ebp.Client_BX]
	mov	[esi.VAD_Far_Call], ecx
	mov	[esi.VAD_PMode_Addr], False	; Assume address is V86 mode
	test	[ebx.CB_VM_Status], VMStat_PM_Exec
	jz	VAD_I15_No_Error
	mov	[esi.VAD_PMode_Addr], True	; Assume address is V86 mode
	jmp	VAD_I15_No_Error

EndProc VAD_Int_15_Emulation

;******************************************************************************
;
;   VAD_Read_Command
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM handle
;
;   EXIT:
;	If carry set then simulated BYTE INPUT value returned in AL
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc VAD_Read_Command, High_Freq, PUBLIC

        cmp     [VAD_Exists], True              ; Q: Virtualizing Aux?
	je	SHORT VAD_RC_Exists		;    Y: Check it out
	cmp	[VAD_Initialized], True 	; Q: VAD initialized yet?
	jne	SHORT VAD_RC_Before_Init	;    N: Perform read
	clc					;    Y: Can't be from aux
	ret

VAD_RC_Exists:
        push    ecx                             ; Save this puppy
	pushfd
	cli
        call    VAD_Test_Data_Avail             ; Q: Looking for Mr. Aux?
	jnc	SHORT VAD_RC_No_Data		;    N: Ok - Keyboard then
	mov	al, BYTE PTR VAD_Queue+1+[ecx*2]
	mov	ah, al
	and	ah, NOT 10b			; clear input buffer full bit
	mov	BYTE PTR VAD_Queue+1+[ecx*2], ah;    Y: AL = Cmd byte

VAD_RC_Ret_Data:
	popfd
        pop     ecx                             ;       Restore ECX
        stc                                     ;       Tell the VKD
        ret

VAD_RC_Before_Init:				; Allow read if not init'd yet
	push	ecx				;   (all exits pop this)
	pushfd
	cli
	in	al, pstat_Kybd			; Read keyboard status
	test	al, fKBS_DAV			; Q: Data available?
	jz	SHORT VAD_RC_No_Data		;    N:
	test	al, fKBS_AuxData		; Q: Aux data?
	jnz	SHORT VAD_RC_Ret_Data		;    Y: pass it back

VAD_RC_No_Data:
	popfd
        pop     ecx                             ; Restore ECX
        clc                                     ; Not aux 
        ret

EndProc VAD_Read_Command


;******************************************************************************
;
;   VAD_Write_Command
;
;   DESCRIPTION:    Check command in AL for an AUX command.
;
;   ENTRY:	    AL command byte written to 64h
;
;   EXIT:	    Z flag set if command was for AUX
;
;   USES:	    Flags
;
;==============================================================================

BeginProc VAD_Write_Command, High_Freq, PUBLIC

	cmp	al, KBD_Ctl_DisAux
	je	short ignore_Aux_cmd
	cmp	al, KBD_Ctl_EnAux
	je	short ignore_Aux_cmd
	cmp	al, KBD_Ctl_TstAux
	je	short ignore_Aux_cmd
	cmp	al, KBD_Ctl_WrAOBf
	je	short ignore_Aux_cmd
	cmp	al, KBD_Ctl_WrAux

ignore_Aux_cmd:
	ret

EndProc VAD_Write_Command


;******************************************************************************
;
;   VAD_Read_Data
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM handle
;
;   EXIT:
;	If carry set then simulated BYTE INPUT value returned in AL
;
;   USES:
;
;------------------------------------------------------------------------------


BeginProc VAD_Read_Data, High_Freq, PUBLIC

        cmp     [VAD_Exists], True              ; Q: Virtualizing Aux?
	je	SHORT VAD_RD_Exists		;    Y: Check it out
	cmp	[VAD_Initialized], True 	; Q: VAD initialized yet?
	jne	SHORT VAD_RD_Before_Init	;    N: Perform read
	clc					;    Y: Can't be from aux
        ret

VAD_RD_Exists:
	push	ecx				; Save this puppy
	pushfd
	cli
        call    VAD_Test_Data_Avail             ; Q: Looking for Mr. Aux?
	jnc	SHORT VAD_RD_No_Data		;    N: Ok - Keyboard then
	mov	al, BYTE PTR VAD_Queue[ecx*2]	;    Y: AL = Data byte
	dec	[VAD_Q_Count]			; One less byte in queue
	inc	cx				; Increment tail pointer
	and	cx, VAD_Q_Mask			; Circular queue
	mov	[VAD_Q_Tail], cx		; Save the tail pointer
	push	eax				; Save it on stack
	mov	eax, [VAD_IRQ_Handle]		; EAX = IRQ handle for Aux
	VK_VD_Queue_Out "VAD clearing int request"
	VxDCall VPICD_Clear_Int_Request 	; Clear the interrupt request
	mov	[VAD_Requested], FALSE
	pop	eax				; Restore data in AL

VAD_RD_Ret_Data:
	popfd
        pop     ecx                             ; Restore ECX
        stc                                     ; Tell the VKD
        ret

VAD_RD_Before_Init:				; Allow read if not init'd yet
	push	ecx				;   (all exits pop this)
	pushfd
	cli
	in	al, pstat_Kybd			; Read keyboard status
	test	al, fKBS_DAV			; Q: Data available?
	jz	SHORT VAD_RD_No_Data		;    N:
	test	al, fKBS_AuxData		; Q: Aux data?
	jz	SHORT VAD_RD_No_Data		;    N:
	in	al, pdata_Kybd			; AL = Data
	jmp	SHORT VAD_RD_Ret_Data

VAD_RD_No_Data:
	popfd
        pop     ecx                             ; Restore ECX
        clc                                     ; Not aux 
        ret

EndProc VAD_Read_Data


;******************************************************************************
;
;   VAD_Test_Data_Avail
;
;   DESCRIPTION:
;
;   ENTRY:
;	EBX = VM handle
;
;   EXIT:
;
;   USES:
;	ECX, Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_Test_Data_Avail, High_Freq, RARE, NO_PROLOG

	Assert_Ints_Disabled

	cmp	ebx, [VAD_VM_Requesting]
	jne	SHORT VAD_TDA_Quick_Exit
        cmp     [VAD_Q_Count], 0
        ja      SHORT VAD_TDA_Data_In_Queue
VAD_TDA_Quick_Exit:
        clc
        ret

VAD_TDA_Data_In_Queue:
        push    eax

        mov     eax, [VAD_IRQ_Handle]
	VxDCall VPICD_Get_Status
	TestReg ecx, VPICD_Stat_In_Service
	jz	SHORT VAD_TDA_No_Data
	mov	eax, [VKD_IRQ_Handle]
	VxDCall VPICD_Get_Status
	TestReg ecx, VPICD_Stat_In_Service
        jnz     SHORT VAD_TDA_No_Data
	movzx	ecx, [VAD_Q_Tail]

	pop	eax
        stc
        ret

VAD_TDA_No_Data:
        pop     eax
        clc
        ret

EndProc VAD_Test_Data_Avail


VxD_CODE_ENDS


VxD_LOCKED_CODE_SEG



;******************************************************************************
;
;   VAD_Request_Int
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = VAD_IRQ_Handle
;
;   EXIT:
;
;   USES:
;	EBX, Flags
;
;------------------------------------------------------------------------------

BeginProc VAD_Request_Int, High_Freq, RARE, NO_PROLOG

	cmp	[VAD_Q_Count], 0		; Q: Any data in queue?
	je	SHORT VAD_Dont_Request		;    N: Easy enough!

IFDEF DEBUG
	push	eax
	movzx	eax, [VAD_Q_Count]
	VK_VD_Queue_Out "VAD_Request_Int : #eax left in queue"
	pop	eax
ENDIF
	mov	bl, [VAD_IRET_Pending]		; Get iret_pending flag
	or	bl, [VAD_Requested]		; Is I_P | V_R ?
	jnz	SHORT VAD_Dont_Request		;    Y: Don't do it again
	not	bl				;    N: do it
	mov	[VAD_Requested],bl		; vad is requested 
	mov	[VAD_IRET_Pending],bl		; iret is pending
	mov	ebx, [VAD_Focus]		;    N: Request for mouse owner
	mov	[VAD_VM_Requesting], ebx
	VK_VD_Queue_Out "VAD request int for VM #EBX"
	VxDCall VPICD_Set_Int_Request

VAD_Dont_Request:
	ret

EndProc VAD_Request_Int



;******************************************************************************
;
;   VAD_Int
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = IRQ Handle
;	EBX = VM Handle
;
;   EXIT:
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc VAD_INT, High_Freq, RARE, NO_PROLOG	; Dynamically locked

IFDEF DEBUG
	VK_VD_Queue_Out "VAD_INT : Interrupt"
ENDIF


	Assert_Ints_Disabled

	in	al, pstat_Kybd			; Read keyboard status
	test	al, fKBS_DAV			; Q: Data available?
	jz	VAD_INT_Not_Aux 		;    N: Chain to next handler
	test	al, fKBS_AuxData		; Q: Aux data?
	jz	VAD_INT_Not_Aux 		;    N: Not ours!
	mov	ah, al				; AH = Status
	in	al, pdata_Kybd			; AL = Data

	cmp	[VAD_Exiting], TRUE		; Q: Exiting?
	je	DEBFAR VAD_INT_Eat_Data 	;    Y: Consume this data

	inc	[VAD_Packet_Count]		; Add 1 byte to this packet
	cmp	[VAD_Packet_Count], 3 		; Q: New packet? (3 bytes)
	jb	SHORT VAD_P_Check_Toss		;    N: Continue

	mov	[VAD_Packet_Count], 0		;    Y: reset counter
	cmp	[VAD_Q_Count], (VAD_Q_Length-2)	; Q: Any room left in queue
						;    (for an entire packet)
	jb	SHORT VAD_P_Enqueue		;    Y: Enqueue this byte
	VK_VD_Queue_Out "VAD - Throwing away:  1"
	mov	[VAD_Toss_Count], 2		;    N: throw it and next 2
	jmp	SHORT VAD_Int_Was_Ours		;	bytes away

VAD_P_Check_Toss:
	cmp	[VAD_Toss_Count], 0		; Q: Should we toss rest of pkt?
	jne	SHORT VAD_P_Toss_More_Data	;    Y: Do it

VAD_P_Enqueue:
	push	ebx
	inc	[VAD_Q_Count]
	movzx	ebx, [VAD_Q_Head]
	mov	VAD_Queue[ebx*2], ax
	inc	bx
	and	bx, VAD_Q_Mask			; Circular queue
	mov	[VAD_Q_Head], bx		; Save the head pointer
	pop	ebx
	jmp	SHORT VAD_Int_Was_Ours


VAD_P_Toss_More_Data:
	dec	[VAD_Toss_Count]

IFDEF DEBUG
	cmp	[VAD_Toss_Count],0
	je	SHORT VAD_P_Toss_Byte3
	VK_VD_Queue_Out "VAD - Throwing away:   2"
	jmp	SHORT VAD_P_Toss_EndDeb
VAD_P_Toss_Byte3:
	VK_VD_Queue_Out "VAD - Throwing away:    3"
VAD_P_Toss_EndDeb:
ENDIF

VAD_Int_Was_Ours:
	mov	eax, [VAD_IRQ_Handle]
	VK_VD_Queue_Out "VAD PHYS EOI"
	VxDcall VPICD_Phys_EOI
	call	VAD_Request_Int
	clc
	ret

;
;   Data was not from the Aux port.  Reflext to next handler.
;
VAD_Int_Not_Aux:
	stc
	ret


;
;   We're exiting -- Eat all data
;
VAD_INT_Eat_Data:
	mov	eax, [VAD_IRQ_Handle]
	VxDcall VPICD_Phys_EOI
	clc
	ret

EndProc VAD_INT


;******************************************************************************
;
;   VAD_IRET
;
;   DESCRIPTION:
;
;   ENTRY:
;	EAX = IRQ Handle
;	EBX = VM Handle
;
;   EXIT:
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc VAD_IRET, High_Freq, RARE, NO_PROLOG	; Dynamically locked

IFDEF DEBUG
	jnc	SHORT VAD_IRET_Did_Iret
	VK_VD_Queue_Out "VAD_IRET : timed-out"
	jmp	SHORT VAD_IRET_Continue
VAD_IRET_Did_Iret:
	VK_VD_Queue_Out "VAD_IRET : IRET"
VAD_IRET_Continue:
ENDIF

	Assert_Ints_Disabled

	mov	[VAD_IRET_Pending], FALSE	; Not waiting for an IRET
	CallRet VAD_Request_Int 		; Request any pending ints

EndProc VAD_IRET


VxD_LOCKED_CODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   VAD_BIOS_Far_Call
;
;   DESCRIPTION:
;	This is "Called" by the IBM int 15h bios code.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;------------------------------------------------------------------------------

BeginProc VAD_BIOS_Far_Call, High_Freq, RARE

	Client_Ptr_Flat esi, SS, SP
	mov	edi, esi			; Want to preserve ESI
	add	di, 0Ah 			; EDI -> Status byte
	mov	al, BYTE PTR [edi]		; Get the aux status
	mov	[VAD_Last_Status], al		; And save it in global data
	cmp	ebx, [VAD_Focus]		; Q: Does VM own the mouse?
	jne	SHORT VAD_BFC_Simulate_Far_Ret	;    N: Strange! - Eat it
	add	ebx, [VAD_CB_Offset]		; EBX -> VAD per VM data
	cmp	[ebx.VAD_Enabled], TRUE 	; Q: Does VM have Aux enabled?
	jne	SHORT VAD_BFC_Simulate_Far_Ret	;    N: Eat this call
						;    Y: Call VM's driver
	cmp	[ebx.VAD_PMode_Addr], True	; Q: Is this from protected mode
	je	SHORT VAD_BFC_PMode		;    Y: Do special things
	mov	ecx, [ebx.VAD_Far_Call]
	movzx	edx, cx 			; EDX = Offset
	shr	ecx, 10h			; CX = Segment
	VMMjmp	Simulate_Far_Jmp		; Replace VMs CS:IP

VAD_BFC_PMode:
	Push_Client_State
	VMMcall Begin_Nest_Exec

	add	si, 0Ah
	mov	ecx, 4
	push	[ebp.Client_ESP]
VAD_BFC_Copy_Stack:
	mov	ax, WORD PTR [esi]
	VMMcall Simulate_Push
	sub	si, 2
	loopd	VAD_BFC_Copy_Stack

	mov	ecx, [ebx.VAD_Far_Call]
	movzx	edx, cx
	shr	ecx, 10h
	VMMcall Simulate_Far_Call
	VMMcall Resume_Exec
	pop	[ebp.Client_ESP]

	VMMcall End_Nest_Exec
	Pop_Client_State

VAD_BFC_Simulate_Far_Ret:
	VMMcall Simulate_Far_Ret
	ret

EndProc VAD_BIOS_Far_Call

VxD_CODE_ENDS

VxD_RARE_Code_SEG

VAD_Ends_Here LABEL BYTE

VxD_RARE_Code_ENDS

        END
