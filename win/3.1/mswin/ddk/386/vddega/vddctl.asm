	TITLE	VDD - Virtual Display Device for EGA/VGA  vers 3.0a
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1991
;
;DESCRIPTION:
;	This module provides the VDD_Control routines called by Device_Control.
;	VDD_Control dispatches the Device_Control calls. VDD_VMCreate is called
;	when a VM is being created.  VDD_VMInit is called just before a VM
;	is run for the first time. VDD_VMSetFocus and is called when the
;	display focus is changed. Initialization is done through the
;	VDD_Sys_Critical_Init and VDD_Device_Init.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC

	Create_VDD_Service_Table EQU True

	INCLUDE VDD.INC
	INCLUDE EGA.INC
	INCLUDE PAGESWAP.INC
	INCLUDE DEBUG.INC


;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VDD, 1, 0, VDD_Control, VDD_Device_ID, \
		       VDD_Init_Order,,VDD_PM_API

;******************************************************************************
; EXTRN routines
;
VxD_ICODE_SEG
	EXTRN	VDD_Sys_Critical_Init:NEAR
	EXTRN	VDD_Device_Init:NEAR
VxD_ICODE_ENDS

VxD_CODE_SEG
	EXTRN	VDD_PM_API:NEAR
	EXTRN	VDD_Begin_Msg_Mode:NEAR
	EXTRN	VDD_End_Msg_Mode:NEAR
	EXTRN	VDD_SaveMsgStt:NEAR

	EXTRN	VDD_Detach:NEAR
	EXTRN	VDD_Detach2:NEAR
	EXTRN	VDD_Clr_VM_Time_Out:NEAR

	EXTRN	VDD_State_VMCreate:NEAR
	EXTRN	VDD_State_Query:NEAR
	EXTRN	VDD_Mem_VMCreate:NEAR
	EXTRN	VDD_Mem_VMInit:NEAR
	EXTRN	VDD_Mem_VMDestroy:NEAR

	EXTRN	VDD_IO_SetTrap:NEAR

	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Restore:NEAR
	EXTRN	VDD_Cancel_Restore:NEAR
	EXTRN	VDD_Restore2:NEAR
	EXTRN	VDD_Cancel_Restore2:NEAR
	EXTRN	VDD_Save:NEAR
	EXTRN	VDD_Save2:NEAR
	EXTRN	VDD_Backgrnd_Event:NEAR
	EXTRN	VDD_Scrn_Off:NEAR

	EXTRN	VDD_Mem_Chg:NEAR
IFNDEF VGA
	EXTRN	VDD_SetEGAMemSize:NEAR
ENDIF
	EXTRN	VDD_Mem_Disable:NEAR

VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Msg_VM:DWORD
	EXTRN	VDD_I10_OldVector:DWORD
        EXTRN   VDD_Initial_Text_Rows:BYTE
IFDEF DEBUG
	EXTRN	VDD_Msg_Pseudo_VM:DWORD
ENDIF

PUBLIC	VDD_Focus_VM
VDD_Focus_VM	DD  ?

VxD_DATA_ENDS

VxD_CODE_SEG
;******************************************************************************
;VDD_Control
;
;DESCRIPTION:
;
;ENTRY: EAX = control code
;	EBX = VM handle
;	EBP = Client ptr
;
;EXIT:	control code specific return of carry flag indicate aborted operation
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
Begin_Control_Dispatch VDD

	Control_Dispatch Set_Device_Focus,  VDD_VMSetFocus
	Control_Dispatch Begin_Message_Mode,VDD_Begin_Msg_Mode
	Control_Dispatch End_Message_Mode,  VDD_End_Msg_Mode
	Control_Dispatch VM_Suspend,	    VDD_Suspend
	Control_Dispatch VM_Resume,	    VDD_Resume
	Control_Dispatch Create_VM,	    VDD_VMCreate
	Control_Dispatch Destroy_VM,	    VDD_VMDestroy
IFDEF	ReserveROMC6C7
	Control_Dispatch VM_Critical_Init,  VDD_VM_Critical_Init
ENDIF
	Control_Dispatch VM_Init,	    VDD_VMInit
	Control_Dispatch Sys_VM_Init,	    VDD_VMInit
	Control_Dispatch System_Exit,	    VDD_System_Exit
	Control_Dispatch Device_Init,	    VDD_Device_Init
	Control_Dispatch Sys_Critical_Init, VDD_Sys_Critical_Init
	Control_Dispatch Sys_Critical_Exit, VDD_Sys_Critical_Exit
IFDEF	DEBUG
	Control_Dispatch Debug_Query,	    VDD_DebugQuery
ENDIF

End_Control_Dispatch VDD



;******************************************************************************
;VDD_VMCreate	    Initialize VDD data for newly created VM.
;
;DESCRIPTION:
;	Init data structures.
;
;ENTRY: EBX = VM Handle
;
;EXIT:	CF = 1 if Error
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_VMCreate, PUBLIC

	Queue_Out "VDD_VMCreate #ebx"
	mov	edi,ebx
	add	edi,[VDD_CB_Off]

	mov	[edi.VDD_Flags],fVDD_IOT	; We are trapping I/O

;*******
;Initialize VM's video memory state to nothing allocated
	call	VDD_Mem_VMCreate
	jc	SHORT VMC_Ex			; If error, quit

;*******
;Initialize VM's controller state
	call	VDD_State_VMCreate

	or	al, -1
	mov	[edi.VDD_ModeEGA],al		; Initialize mode
	mov	[edi.VDD_LastMode],al		; Initialize VMDA mode
	call	VDD_Get_Mode			; EAX = current mode for VM

	clc					; return with no error
VMC_Ex:
	ret
EndProc VDD_VMCreate

;******************************************************************************
;VDD_VMInit
;
;DESCRIPTION: Initialize VM's memory, enable/disable I/O port trapping and
;	      initialize device state by calling INT 10.
;
;ENTRY: EBX = VM Handle
;
;EXIT:	CF = 1 if error
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_VMInit

	Queue_Out "VDD_VMInit #ebx"
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Mem_VMInit		   ; Memory init
	jc	SHORT VMI_Exit

IFNDEF VGA
        call    VDD_SetEGAMemSize          ; set video mem size in BIOS area
ENDIF
        call    VDD_SetupInitialVideoState ; Do int 10s and set up initial state
VMI_Exit:
	ret
EndProc VDD_VMInit


;******************************************************************************
;VDD_VMSetFocus
;
;DESCRIPTION:
;	Is called when switching the display focus.
;
;ENTRY: EBX = VM Handle of VM to switch display focus to
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_VMSetFocus,PUBLIC

	test	edx, edx			; Q: Critical set focus call?
	je	SHORT VSF_00			;    Y: Do it
	cmp	edx, VDD_Device_ID		;    N: Q: VDD set focus?
	jne	SHORT VDD_SF_Ex 		;	   N: Nothing to do
VSF_00:

	mov	esi, ebx
	xchg	esi, [VDD_Focus_VM]		; set new focus VM

	push	ebx
	mov	ebx, esi			;   N: Cancel pending restore
	call	VDD_Cancel_Restore		;      for old focus VM (esi)
	pop	ebx

	cmp	[VDD_Msg_VM], 0 		; Q: Message mode?
	jnz	SHORT VDD_SF_Ex 		;   Y: Don't do anything more

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cmp	ebx,esi 			; Q: VM already has focus?
	je	SHORT VSF_02			;   Y: Don't detach

	TestMem [edi.VDD_Flags],fVDD_256	;Q: new focus a 256K VM?
	jnz	SHORT VSF_01			;   Y:
	mov	eax,[VDD_CB_Off]
	TestMem [eax+esi.VDD_Flags],fVDD_256	;Q: old focus a 256K VM?
	jz	SHORT VSF_01			;   N:

	call	VDD_Resume_Bkgnd_VMs

VSF_01:
	call	VDD_Detach			; detach from display
	call	VDD_Detach2			; Detach from 2nd EGA
VSF_02:
	call	VDD_Restore			; physical display
VDD_SF_Ex:
	clc					; CLEAR CARRY IF JUMP!!!!
	ret
EndProc VDD_VMSetFocus


;******************************************************************************
;VDD_VMDestroy
;
;DESCRIPTION:
;	Deallocate the pages used for video ram.
;
;ENTRY: EBX = VM Handle of VM that is being destroyed
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_VMDestroy,PUBLIC

	mov	edi,ebx
	add	edi,[VDD_CB_Off]

;Deallocate VM's video pages
	call	VDD_Clr_VM_Time_Out		; Clear VM timeout
	call	VDD_Mem_VMDestroy		; Delete memory allocated

;Force detach
	cmp	ebx,[Vid_VM_Handle]		; Q: Is this attached VM?
	jnz	SHORT VD_1			;   N: all done
	mov	[Vid_VM_Handle],0		;   Y: detach
VD_1:
	cmp	ebx,[Vid_VM_HandleRun]		; Q: Is this running VM?
	jnz	SHORT VD_2			;   N: all done
	mov	[Vid_VM_HandleRun],0		;   Y: detach
VD_2:
	cmp	ebx,[Vid_VM_Handle2]		; Q: Is this attached to 2nd EGA?
	jnz	SHORT VD_3			;   N: proceed
	mov	[Vid_VM_Handle2],0		;   Y: detach
VD_3:
	cmp	ebx,[VDD_Focus_VM]		; Q: Is this focus VM?
	jnz	SHORT VD_4			;   N: all done
	push	ebx
	VMMCall Get_Sys_VM_Handle
	mov	[VDD_Focus_VM],ebx		;   Y: WEIRD! Attach SYS VM
	pop	ebx
VD_4:
	clc
	ret
EndProc VDD_VMDestroy

;******************************************************************************
;VDD_Suspend
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle of VM that is being suspended
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Suspend

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cmp	ebx, [VDD_Focus_VM]		; Q: This VM have focus
	jne	SHORT VSus_00			;   N: Proceed
	call	VDD_Cancel_Restore		;   Y: Cancel restore pending
	jmp	short VSus_01

VSus_00:
	cmp	ebx, [Vid_VM_Handle]		; Q: This VM own display?
	jne	SHORT VSus_01			;   N: Proceed
%OUT VDD_Suspend: can this happen?  detach Vid_VM_Handle != VMM_Focus_VM
	Debug_Out "VDD_Suspend: why are we getting here?"
	call	VDD_Detach			;   Y: Detach

VSus_01:

;
; unlock video memory
;
	mov	eax, [edi.VDD_Pg.VPH_hMem]
	or	eax, eax
	jz	SHORT VSus_No_main
	Trace_Out 'VDD: Unlocking #eax for #ebx'
	movzx	ecx, [edi.VDD_Pg.VPH_PgCnt]
	VMMCall _PageUnLock, <eax, ecx, 0, 0>
VSus_No_main:
	mov	eax, [edi.VDD_CPg.VPH_hMem]
	or	eax, eax
	jz	SHORT VSus_No_copy
	Trace_Out 'VDD: Unlocking #eax for #ebx'
	movzx	ecx, [edi.VDD_CPg.VPH_PgCnt]
	VMMCall _PageUnLock, <eax, ecx, 0, 0>
VSus_No_copy:
        call    VDD_Mem_Chg
	call	VDD_Mem_Disable 		; disable null mapped pages
	clc
	ret
EndProc VDD_Suspend

;******************************************************************************
;VDD_Resume
;
;DESCRIPTION:
;
;ENTRY: EBX = VM handle of VM to resume
;
;EXIT:	CF = 1 if cannot resume
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Resume

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
;
; lock video memory
;
	mov	eax, [edi.VDD_Pg.VPH_hMem]
	or	eax, eax
	jz	SHORT VRes_No_main
	Trace_Out 'VDD: Locking #eax for #ebx'
	movzx	ecx, [edi.VDD_Pg.VPH_PgCnt]
	VMMCall _PageLock,<eax, ecx, 0, 0>
	or	eax, eax			;Q: lock succeed?
	jz	SHORT VRes_Lk_failed		;   N:
VRes_No_main:
	mov	eax, [edi.VDD_CPg.VPH_hMem]
	or	eax, eax
	jz	SHORT VRes_No_copy
	Trace_Out 'VDD: Locking #eax for #ebx'
	movzx	ecx, [edi.VDD_CPg.VPH_PgCnt]
	VMMCall _PageLock,<eax, ecx, 0, 0>
	or	eax, eax			;Q: lock succeed?
	jz	SHORT VRes_Lk_failed		;   N:
VRes_No_copy:
	clc
	ret

VRes_Lk_failed:
	stc
	ret

EndProc VDD_Resume

;******************************************************************************
;VDD_System_Exit
;
;DESCRIPTION: Initialize VM's memory, enable/disable I/O port trapping and
;	      initialize device state by calling INT 10.
;
;ENTRY: EBX = VM Handle
;
;EXIT:	CF = 1 if error
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_System_Exit

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Mem_VMInit		   ; Memory init
	jc	SHORT VSE_Exit

IFDEF VGA8514
    ;
    ; On a Fatal exit we have to init the display to VGA mode 
    ; so that the user can see the message written to the VGA screen
    ;
	mov	ax,6				;set misc I/O back to VGA
	mov	dx,4ae8h			
	out	dx,ax				
ENDIF	

        call    VDD_SetupInitialVideoState ; Do int 10s and set up initial state
VSE_Exit:
	ret

EndProc VDD_System_Exit

;******************************************************************************
;VDD_SetupInitialVideoState
;
;DESCRIPTION: Setup up the initial video state for the VM
;             by Enable/disable I/O port trapping and using INT 10s
;
;ENTRY: EBX = VM Handle
;       EDI = CB ptr
;EXIT:	CF = 1 if error
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================

BeginProc VDD_SetupInitialVideoState,PUBLIC

	call	VDD_IO_SetTrap

;*******
; Special code to detect display adapters and set appropriate flags
	VMMCall Test_Sys_VM_Handle		; Only detect special features
	jnz	SHORT Not_SYS_VMInit		;   during SYS VM init.

        push    eax
        push    ebx
        push    ecx
        VxDcall PageSwap_Get_Version
        test    eax,eax                         ; Q: Is PageSwap installed?
        pop     ecx
        pop     ebx
        pop     eax
        jz      SHORT VSIV_NoPageSwap
	SetFlag [Vid_Flags],fVid_PageSwapInst	; PageSwap device installed
VSIV_NoPageSwap:

Not_SYS_VMInit:

;*******
; Set video mode via INT 10 which will set controller values via I/O traps
	Push_Client_State
	VMMcall Begin_Nest_Exec 		; Get ready for software ints

        mov     dl,[VDD_Initial_Text_Rows]
        or      dl,dl                           ; Q: Value specified in .INI?
        jns     SHORT VSIV_GotRows              ;   Y: Use that value

	mov	[ebp.Client_AX], 1130h          ;   N: Get # of rows from BIOS
	mov	[ebp.Client_BH], 3
	mov	[ebp.Client_DL], 0		; In case func 1130 not implem.
	mov	eax,10h
	VMMcall Exec_Int			; Query video
	mov	dl,[ebp.Client_DL]		; DL = rows on screen

VSIV_GotRows:

IFDEF V7VGA
	TestMem [Vid_Flags],fVid_V7VGA
	jz	SHORT VSIV_Pure0
	or	byte ptr ds:[487h],10h		; "reset" pure mode
VSIV_Pure0:
ENDIF

	mov	[ebp.Client_AX], 0003h
	mov	eax,10h
	VMMcall Exec_Int			; Set mode 3

IFDEF V7VGA
	TestMem [Vid_Flags],fVid_V7VGA
	jz	SHORT VSIV_Pure1
	and	byte ptr ds:[00487h],NOT 10h	; "set" us into pure mode
VSIV_Pure1:
ENDIF

; If first time (SYS VM) save mode 3 state for message mode
	VMMCall Test_Sys_VM_Handle
	jnz	SHORT VSIV_00

IFDEF V7VGA
	TestMem [Vid_Flags],fVid_V7VGA
	jz	SHORT VSIV_Pure2
	or	byte ptr ds:[487h],10h		; "reset" pure mode
VSIV_Pure2:
ENDIF

	SetFlag [Vid_Flags], fVid_SysVMI	; flag successful initialization
	call	VDD_SaveMsgStt			; Save for message mode
VSIV_00:
IFDEF EGA
	cmp	dl,42				; Q: 25 line mode?
ELSE
	cmp	dl,49				; Q: 25 line mode?
ENDIF
	jb	SHORT VEN_Not43 		;   Y: All done
	mov	[ebp.Client_AX], 1112h		;   N: Set "43" line mode- 50 lines on VGA
	mov	[ebp.Client_BL], 0
	mov	eax,10h
	VMMcall Exec_Int			; Select 8x8 alpha font, block 0
	mov	[ebp.Client_AX], 0100h
	mov	[ebp.Client_CX], 0407h
	mov	eax,10h
	VMMcall Exec_Int			; Set cursor to scan lines 4-7

VEN_Not43:
	VMMcall End_Nest_Exec			; All done with software ints
	Pop_Client_State
	TestMem [edi.VDD_Flags],fVDD_Win	; Q: Windowed?
        jz      SHORT VSIV_NotWin               ;   N:
	SetFlag [edi.VDD_Flags],fVDD_ForcedUpd	;   Y: Force window update
VSIV_NotWin:
	bt	[edi.VDD_EFlags],fVDE_NoMainBit ; Fail init if mem alloc failed
VSIV_Exit:
	ret

EndProc VDD_SetupInitialVideoState

;******************************************************************************
;
;   VDD_Sys_Critical_Exit
;
;   DESCRIPTION:    Restore original INT 10 handler
;
;   ENTRY:	    nothing
;
;   EXIT:	    nothing
;
;   USES:	    EAX, ECX, EDX, Flags
;
;==============================================================================
BeginProc VDD_Sys_Critical_Exit

	mov	ecx, [VDD_I10_OldVector]
	jecxz	SHORT VSCE_Return
	movzx	edx, cx
	shr	ecx, 16
	mov	eax, 10h
	VMMCall Set_V86_Int_Vector
VSCE_Return:
	clc
	ret

EndProc VDD_Sys_Critical_Exit


IFDEF	ReserveROMC6C7
;******************************************************************************
;
;   VDD_VM_Critical_Init
;
;   DESCRIPTION:
;	Finish setting up VM for execution
;
;   ENTRY:
;	EBX = VM's handle
;
;   EXIT:
;	CF = 0
;
;   USES:
;	Nothing
;
;==============================================================================

BeginProc VDD_VM_Critical_Init, PUBLIC
	pushad
	TestMem [Vid_Flags], fVid_RsrvC6C7
	jz	SHORT VVCI_Exit
	mov	edx,0C6h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,2,0>	; Map Phys=Linear from C6 to C7

VVCI_Exit:
	popad
	clc
	ret
EndProc VDD_VM_Critical_Init
ENDIF


;******************************************************************************
;
;   VDD_Resume_Bkgnd_VMs
;
;   DESCRIPTION:    Unsuspend VM's that were suspended because current
;		    focus was 256K pig
;
;   ENTRY:	    EBX = current VM
;
;   EXIT:
;
;   USES:	    EAX, Flags
;
;==============================================================================

BeginProc VDD_Resume_Bkgnd_VMs, PUBLIC

	push	ebx
	mov	eax,[VDD_CB_Off]
	VMMCall Get_Sys_VM_Handle
VRB_next_VM:
	VMMCall Get_Next_VM_Handle
	VMMcall Test_Sys_VM_Handle		;Q: back to Sys VM?
	je	SHORT VRB_exit			;   Y: done
	TestMem [ebx.CB_VM_Status], VMStat_Suspended	;Q: VM suspended?
	jz	VRB_next_VM				;   N: skip it
	TestMem [eax+ebx.VDD_Flags],fVDD_WaitAtt;Q: VM suspended by VDD?
	jz	VRB_next_VM			;   N: skip it
	Trace_Out 'VDD resuming #ebx'
	VMMCall No_Fail_Resume_VM		;   Y: unsuspend it now
IFDEF DEBUG
	jnc	SHORT VRB_D00
	Trace_Out '  resume failed!'
VRB_D00:
ENDIF
	jc	VRB_next_VM			; resume failed
	ClrFlag [eax+ebx.VDD_Flags], fVDD_WaitAtt
	jmp	VRB_next_VM

VRB_exit:
	pop	ebx
	ret

EndProc VDD_Resume_Bkgnd_VMs


IFDEF	DEBUG
;******************************************************************************
;VDD_DebugQuery
;
;DESCRIPTION:
;	Dump appropriate VDD state information for each VM.
;
;ENTRY: none
;
;EXIT:	none
;
;USES: Flags
;
;==============================================================================
BeginProc VDD_DebugQuery

	pushad
Trace_Out   "VDD video state dump for all Virtual Machines:"

	mov	eax, [VDD_CB_Off]
	Trace_Out "VDD_CB_Off        = #eax"
	mov	eax, [VDD_Focus_VM]
	Trace_Out "VDD_Focus_VM      = #eax"
	mov	eax, [VDD_Msg_VM]
	Trace_Out "VDD_Msg_VM        = #eax"
	mov	eax, [Vid_VM_Handle]
	Trace_Out "Vid_VM_Handle     = #eax"
	mov	eax, [Vid_VM_HandleRun]
	Trace_Out "Vid_VM_HandleRun  = #eax"
	mov	eax, [Vid_VM_Handle2]
	Trace_Out "Vid_VM_Handle2    = #eax"
	mov	eax, [VDD_Msg_Pseudo_VM]
	Trace_Out "VDD_Msg_Pseudo_VM = #eax"
	VMMcall In_Debug_Chr
	jz	short VDQ_Ex

	VMMCall Get_Cur_VM_Handle
VDQ_00:
	call	Vid_DumpState
	VMMCall Get_Next_VM_Handle
	VMMCall Test_Cur_VM_Handle
	jz	SHORT VDQ_Ex
	VMMcall In_Debug_Chr
	jnz	VDQ_00
VDQ_Ex:
	popad
	ret

;*******
Vid_DumpState:
Trace_Out	"***********************************"
	cmp	ebx,[Vid_VM_Handle]
	jz	SHORT VDQ_0
Trace_Out	"VM #EBX does not have display focus"
	jmp	SHORT VDQ_1
VDQ_0:
Trace_Out	"VM #EBX has display focus"
VDQ_1:
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	movzx	eax,[edi.VDD_Pg.VPH_Mode]
	mov	ecx,[edi.VDD_Pg.VPH_PgAllMsk]
	mov	edx,[edi.VDD_Pg.VPH_Pg2AllMsk]
	mov	esi,[edi.VDD_Pg.VPH_MState.VDA_Mem_Addr]
Trace_Out	"Mem mode = #AL, alloc mask = #EDX #ECX, addr = #ESI"
	movzx	eax,[edi.VDD_ModeEGA]
Trace_Out	"Controller mode = #AL"
Trace_Out	"Registers:"
Trace_Out	"CRTC:" +

IFDEF	PEGA
	mov	al,[edi.VDD_Stt.C_Indx]
Trace_Out	"(#AL)"

;******* Dump unscrambled, unlocked regs
	mov	ecx,DWORD PTR [edi.VDD_Stt.VC_PCUnUMsk]
Trace_Out	" #CX:   " +
	mov	ecx,(SIZE CRTC_Struc)/2
	lea	esi,[edi.VDD_Stt.CRTC]
	call	Vid_Reg_Dump
	mov	ecx,(SIZE CRTC_Struc)/2
Trace_Out	" "
Trace_Out	"         " +
	call	Vid_Reg_Dump
Trace_Out	" "

;******* Dump unscrambled, locked regs, if any
	mov	ecx,DWORD PTR [edi.VDD_Stt.VC_PCUnLMsk]
	or	ecx,ecx 		    ; Q: Any locked CRTC values?
	jz	SHORT VDQ_ScU		    ;	N: skip
Trace_Out	" #CX:   " +
	mov	ecx,(SIZE CRTC_Struc)/2
	lea	esi,[edi.VDD_Stt.VC_PCUnL]
	call	Vid_Reg_Dump
	mov	ecx,(SIZE CRTC_Struc)/2
Trace_Out	" "
Trace_Out	" Lok     " +
	call	Vid_Reg_Dump
Trace_Out	" "

;******* Dump scrambled, unlocked regs, if any
VDQ_ScU:
	mov	ecx,DWORD PTR [edi.VDD_Stt.VC_PCScUMsk]
	or	ecx,ecx 		    ; Q: Any scrm'd, unlocked values?
	jz	SHORT VDQ_ScL		    ;	N: skip
Trace_Out	" #CX:   " +
	mov	ecx,(SIZE CRTC_Struc)/2
	lea	esi,[edi.VDD_Stt.VC_PCScU]
	call	Vid_Reg_Dump
	mov	ecx,(SIZE CRTC_Struc)/2
Trace_Out	" "
Trace_Out	" Scrm    " +
	call	Vid_Reg_Dump
Trace_Out	" "

;******* Dump scrambled, locked regs, if any
VDQ_ScL:
	mov	ecx,DWORD PTR [edi.VDD_Stt.VC_PCScLMsk]
	or	ecx,ecx 		    ; Q: Any scrm'd, unlocked values?
	jz	SHORT VDQ_CRTC_Done	    ;	N: skip
Trace_Out	" #CX:   " +
	mov	ecx,(SIZE CRTC_Struc)/2
	lea	esi,[edi.VDD_Stt.VC_PCScL]
	call	Vid_Reg_Dump
	mov	ecx,(SIZE CRTC_Struc)/2
Trace_Out	" "
Trace_Out	" Scrm,Lok" +
	call	Vid_Reg_Dump
Trace_Out	" "
VDQ_CRTC_Done:

ELSE
	mov	al,[edi.VDD_Stt.C_Indx]
Trace_Out	"(#AL)" +
	mov	ecx,(SIZE CRTC_Struc)/2
	lea	esi,[edi.VDD_Stt.CRTC]
	call	Vid_Reg_Dump
	mov	ecx,(SIZE CRTC_Struc)/2
Trace_Out	" "
Trace_Out	"         " +
	call	Vid_Reg_Dump
Trace_Out	" "
ENDIF

Trace_Out	"Attr:" +
	mov	al,[edi.VDD_Stt.A_Indx]
Trace_Out	"(#AL)" +
	lea	esi,[edi.VDD_Stt.A_Pal]
	mov	ecx,16
	call	Vid_Reg_Dump
Trace_Out	" "
Trace_Out	"         " +
	mov	ecx,(S_Rst-A_Pal)-16
	call	Vid_Reg_Dump
Trace_Out	" "
Trace_Out	"Grp: " +
	mov	al,[edi.VDD_Stt.G_Indx]
Trace_Out	"(#AL)" +
	mov	ecx,A_Indx-G_SetRst
	lea	esi,[edi.VDD_Stt.G_SetRst]
	call	Vid_Reg_Dump
Trace_Out	" "
Trace_Out	"Seq: " +
	mov	al,[edi.VDD_Stt.S_Indx]
Trace_Out	"(#AL)" +
	mov	ecx,G_SetRst-S_Rst
	lea	esi,[edi.VDD_Stt.S_Rst]
	call	Vid_Reg_Dump
Trace_Out	" "
IFDEF V7VGA
	TestMem [Vid_Flags],<fVid_V7VGA+fVid_CLVGA>
	jz	SHORT VRD_Exit
Trace_Out	"V7_extnd:" +
	mov	ecx,8
	lea	esi,[edi.VDD_Stt.VDD_V7ER]
V7_R_Out:
	push	ecx
	mov	ecx,10h
	call	Vid_Reg_Dump
Trace_Out	" "
Trace_Out	"         " +
	pop	ecx
	loopd	V7_R_Out
VRD_Exit:
ENDIF

	ret

; Dump CX regs at ESI
Vid_Reg_Dump:
	xor	edx,edx
        cld
VRD_Lp:
	lodsb
Trace_Out " #AL" +
	inc	edx
	loop	VRD_lp
	ret
EndProc VDD_DebugQuery
ENDIF


VxD_CODE_ENDS

	END
