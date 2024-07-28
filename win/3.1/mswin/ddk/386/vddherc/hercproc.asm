       title   VDD - Virtual Display Device HERC version 0.00  6/2/87
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW, PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;
;DESCRIPTION:
;	This module provides most of routines called by VMM. The VDD_Control
;	procedure dispatches these VMM calls. VDD_VMCreate is called
;	when a VM is being created.  VDD_VMInit is called just before a VM
;	is run for the first time. VDD_VMSetFocus and is called when the
;	display focus is changed. Initialization is done through the
;	VDD_Sys_Critical_Init and VDD_Device_Init.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC

	Create_VDD_Service_Table EQU True

	INCLUDE VDD.INC
	INCLUDE HERC.INC
	INCLUDE SHELL.INC
	INCLUDE VDDGRB.INC
	INCLUDE DEBUG.INC

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VDD, 1, 0, VDD_Control, VDD_Device_ID, VDD_Init_Order,,\
		       VDD_PM_API

;******************************************************************************
;******************************************************************************

; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_PM_API:NEAR
	EXTRN	VDD_Begin_Msg_Mode:NEAR
	EXTRN	VDD_End_Msg_Mode:NEAR
	EXTRN	VDD_Msg_InitScreen:NEAR
	EXTRN	VDD_Device_Init:NEAR
	EXTRN	VDD_Sys_Critical_Init:NEAR

	EXTRN	VDD_State_VMCreate:NEAR
	EXTRN	VDD_State_Query:NEAR
	EXTRN	VDD_Get_Mode:NEAR

	EXTRN	VDD_Mem_VMCreate:NEAR
	EXTRN	VDD_Mem_VMInit:NEAR
	EXTRN	VDD_Mem_VMDestroy:NEAR

;	EXTRN	VDD_Font_Restore:NEAR
;	EXTRN	VDD_Font_Save:NEAR

	EXTRN	VDD_Restore:NEAR
	EXTRN	VDD_Cancel_Restore:NEAR
	EXTRN	VDD_Save:NEAR
	EXTRN	VDD_SaveMsgStt:NEAR

	EXTRN	VDD_Mem_Virtual:NEAR
	EXTRN	VDD_Mem_MapNull:NEAR
	EXTRN	VDD_Mem_Physical:NEAR

VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	EXTRN	VDD_PhysB0000:DWORD

;******************************************************************************
; Reserve space for the VDD's global data
;
PUBLIC	vgVDD
vgVDD		DB	SIZE vgVDD_Struc DUP (?)

PUBLIC VDD_VMIdle_Chain
VDD_VMIdle_Chain    DD	?

PUBLIC	VDD_Focus_VM
VDD_Focus_VM	DD  ?

PUBLIC	VDD_CB_Off
VDD_CB_Off	DD  ?

PUBLIC	VDD_Win_Update_Time
VDD_Win_Update_Time DD	50

%OUT allow INI switch to modify this default setting
def_Cntrl   db 3

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
	Control_Dispatch Set_Device_Focus, VDD_VMSetFocus
	Control_Dispatch Begin_Message_Mode,VDD_Begin_Msg_Mode
	Control_Dispatch End_Message_Mode,VDD_End_Msg_Mode
	Control_Dispatch VM_Suspend, VDD_Suspend
	Control_Dispatch VM_Resume, VDD_Resume
	Control_Dispatch Create_VM, <SHORT VDD_VMCreate>
	Control_Dispatch Destroy_VM, VDD_VMDestroy
	Control_Dispatch VM_Init, <SHORT VDD_VMInit>
	Control_Dispatch Sys_VM_Init, <SHORT VDD_VMInit>
	Control_Dispatch System_Exit, <SHORT VDD_VMInit>
	Control_Dispatch Device_Init, VDD_Device_Init
	Control_Dispatch Sys_Critical_Init, VDD_Sys_Critical_Init
IFDEF	DEBUG
	Control_Dispatch Debug_Query, VDD_DebugQuery
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

	mov	edi,ebx
	add	edi,[VDD_CB_Off]

	mov	[edi.VDD_Flags],0		; Initialize flags

	call	VDD_Mem_VMCreate		; Initialize VM's memory state
	call	VDD_State_VMCreate		; Init VM's controller state

	mov	[edi.VDD_Mode],-1		; Initialize mode
	mov	[edi.VDD_LastMode],-1		; Initialize VMDA mode
	call	VDD_Get_Mode			; EAX = current mode for VM

	clc					; return with no error
	ret
EndProc VDD_VMCreate

;******************************************************************************
;VDD_VMInit
;
;DESCRIPTION: Initialize VM's memory and device state by calling INT 10.
;
;ENTRY: EBX = VM Handle
;
;EXIT:	CF = 1 if error
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_VMInit

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Mem_VMInit			; Memory init
	jc	VVMI_Exit
;
; Set video mode via INT 10 which will set controller values via I/O traps
;
	Push_Client_State
	VMMcall Begin_Nest_Exec 		; Get ready for software ints
	mov	[ebp.Client_AX], 0007h
	mov	eax,10h
	VMMcall Exec_Int			; Set mode 7
	VMMcall End_Nest_Exec			; All done with software ints
	Pop_Client_State
; If first time (SYS VM) save mode 3 state for message mode
	VMMCall Test_Sys_VM_Handle
	jnz	SHORT VVMI_00
	call	VDD_SaveMsgStt			; Save for message mode
        bts     [vgVDD.Vid_Flags],fVid_MsgSttSavedBit ; indicate msg stt saved
VVMI_00:

        test    [edi.VDD_Flags],fVDD_Win        ; Q: Windowed?
        jz      SHORT VVMI_NotWin               ;   N:
        or      [edi.VDD_Flags],fVDD_ForcedUpd  ;   Y: Force window update
VVMI_NotWin:
	bt	[edi.VDD_EFlags],fVDE_NoMainBit ; Fail init if mem alloc failed

	mov	dx,pStatMono		; Status port
	in	al,dx
	mov	[edi.VDD_Stt.V_Stat],al ; Set status register
	mov	al, [def_Cntrl]
	mov	[edi.VDD_Stt.V_Cntrl], al

	mov	[edi.VDD_Pg.VPH_LLen], 80 * 2
	mov	[edi.VDD_Pg.VPH_Rows], 25
	mov	[edi.VDD_Pg.VPH_PgSz], 80 * 2 * 25

VVMI_Exit:
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
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cmp	ebx,[VDD_Focus_VM]		; Q: proper VM the focus?
	je	SHORT VSF_01			;   Y: Don't detach

	call	VDD_Detach			; detach from display
VSF_01:
	mov	[VDD_Focus_VM],ebx
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	test	[vgVDD.Vid_Flags],fVid_Msg	; Q: Message mode?
	jnz	VDD_Msg_InitScreen		;   Y: Go initialize
	call	VDD_Restore			;	physical display
VDD_SF_Ex:
	clc 
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
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: Is this attached VM?
	jnz	SHORT VD_1			;   N: all done
	mov	[vgVDD.Vid_VM_Handle],0 	;   Y: detach
VD_1:
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
	or	[edi.VDD_Flags],fVDD_Sus
	cmp	ebx,[VDD_Focus_VM]		; Q: This VM have focus
	jne	SHORT VSus_00			;   N: Proceed
	call	VDD_Cancel_Restore		;   Y: Cancel restore pending
VSus_00:
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: This VM own display?
	jne	SHORT VSus_01			;   N: Proceed
	call	VDD_Detach			;   Y: Detach
VSus_01:
IF2
%OUT unlock video memory
ENDIF
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
	btr	[edi.VDD_Flags], fVDD_SusBit
	jnc	SHORT VRes_00

        cmp     ebx,[VDD_Focus_VM]
        jnz     SHORT VR_SkipRest
        call    VDD_Restore
        jmp     SHORT VRes_00
VR_SkipRest:
	call	VDD_Mem_Virtual
VRes_00:
	clc
	ret
EndProc VDD_Resume


;******************************************************************************
;VDD_PFault	Do Not Handle page faults on video pages
;
;DESCRIPTION:
;	In the current design of the HERC VDD, page faults on video
;	memory indicate a system error.  They are never supposed to
;	happen.
;
;ENTRY: EBX = VM handle
;	EDX = page number
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
BeginProc VDD_PFault

Debug_Out "VDD page fault??"
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	jmp	VDD_Mem_MapNull
EndProc VDD_PFault

;******************************************************************************
;VDD_VMIdle	    Check for screen update while VM is idle
;
;DESCRIPTION:
;	Display changes are checked for after a VM has been executing for
;	a fixed period of time, or the VM has become idle(e.g. waiting for
;	keyboard input) in which case the VM will not be executing much at
;	all. This is the routine that gets called when the VM is idle.
;
;ENTRY: EBX = VM handle
;
;EXIT:	CF=0(don't force this VM to run again)
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_VMIdle,SERVICE,High_Freq

	pushad
	VMMCall Get_Cur_VM_Handle
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VM's VDD ptr
	test	[edi.VDD_Flags],fVDD_Win
	jz	SHORT VMI_00
	call	VDD_Clr_VM_Time_Out		; Clear pending timeout
	call	VDD_Update_Window		; Do screen update if necessary
VMI_00:
	popad
	jmp	[VDD_VMIdle_Chain]
EndProc VDD_VMIdle

;******************************************************************************
;VDD_VMTimeOut	    Check for screen update after VM has executed for awhile
;
;DESCRIPTION:
;	Display changes are checked for after a VM has been executing for
;	a fixed period of time, or the VM has become idle(e.g. waiting for
;	keyboard input) in which case the VM will not be executing much at
;	all. This is the routine that gets called when the VM has executed
;	for awhile
;
;ENTRY: EBX = VM handle
;	EDX = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_VMTimeOut,PUBLIC,High_Freq

	mov	edi,edx 			; EDI = VM's VDD CB ptr
	mov	[edi.VDD_Time_Out_Handle],0
	test	[edi.VDD_Flags],fVDD_Win	; Q: Windowed VM?
	jnz	SHORT VDD_Update_Window 	;   Y:Check for window update
	ret
EndProc VDD_VMTimeOut

;******************************************************************************
;VDD_Update_Window  Check for screen update
;
;DESCRIPTION:
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Update_Window,,High_Freq

	call	VDD_State_Query 		; Q: Video changed?
	jnc	SHORT VDD_Set_VM_Time_Out	;   N: resched timeout
	call	VDD_VMDA_Update 		;   Y: schedule update
	ret
EndProc VDD_Update_Window


;******************************************************************************
;VDD_Set_VM_Time_Out
;
;DESCRIPTION:
;	Start VM Time out, unless time out already active.
;
;ENTRY: EBX = VM handle
;	EDX = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Set_VM_Time_Out,PUBLIC,High_Freq
	test	[edi.VDD_Time_Out_Handle],-1	; Q: Timeout active?
	jnz	SHORT VSVTO_Exit
	mov	eax, [VDD_Win_Update_Time]	; Display update time delay
	mov	edx,edi
	mov	esi,OFFSET32 VDD_VMTimeOut
	VMMCall Set_VM_Time_Out 		; Set display update timeout
	mov	[edi.VDD_Time_Out_Handle],esi	; Save timeout handle
VSVTO_Exit:
	ret
EndProc VDD_Set_VM_Time_Out

;******************************************************************************
;VDD_Clr_VM_Time_Out
;
;DESCRIPTION:
;	Clear VM Time out if time out is active.
;
;ENTRY: EBX = VM handle
;	EDX = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Clr_VM_Time_Out,PUBLIC,High_Freq

	xor	esi, esi
	xchg	esi, [edi.VDD_Time_Out_Handle]
	VMMCall Cancel_Time_Out 		; Cancel pending update timeout
	ret
EndProc VDD_Clr_VM_Time_Out

;******************************************************************************
;VDD_Detach
;
;DESCRIPTION: Detach any currently attached or scheduled to be attached VM.
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags, EDX, ESI
;
;==============================================================================
BeginProc VDD_Detach,PUBLIC

	push	ebx
	push	edi
	mov	ebx,[VDD_Focus_VM]
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
;
; Save the state of the attached VM
;
VD_00:
	test	[vgVDD.Vid_VM_Handle],-1
	jz	SHORT VD_01			; Nothing currently attached
	mov	ebx,[vgVDD.Vid_VM_Handle]
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Save			; Save attached VM's state
	mov	[vgVDD.Vid_VM_Handle],0
VD_01:
	call	VDD_Cancel_Restore		; Cancel any pending restore
	call	VDD_Mem_Virtual 		; Force remap of memory
	pop	edi
	pop	ebx
	ret
EndProc VDD_Detach

;******************************************************************************
;VDD_VMDA_Update    Send update event to VMDOSAPP
;
;DESCRIPTION: If an update event is already queued, do nothing, else
;	call Shell_Event with update event for VMDOSAPP.
;
;ENTRY: EBX = VM handle for which screen change occurred
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_VMDA_Update,PUBLIC

	bts	[edi.VDD_Flags],fVDD_UpdBit	; Only one update allowed
	jc	SHORT VVU_Exit
	mov	esi,OFFSET32 Vid_Upd_CallBack	; Clear Upd flag on completion
	mov	edx,edi 			; Pass VDD CB ptr to call back
	mov	ecx,VDA_Display_Event		; Display event
	mov	eax,VDA_DE_DispChngMin		; Screen Chng subfunction
	VxDCall Shell_Event			; Queue up the event
	jc	SHORT Vid_Upd_CallBack		; Error: clear status, try again
VVU_Exit:
	ret
EndProc VDD_VMDA_Update

BeginProc Vid_Upd_CallBack
	and	[edx.VDD_Flags], NOT fVDD_Upd
	mov	edi,edx
	mov	ebx,edx
	sub	ebx,[VDD_CB_Off]
	jmp	VDD_Set_VM_Time_Out	; schedule next timeout
EndProc Vid_Upd_CallBack

;******************************************************************************
;VDD_VMDA_Grab	    Send grab event to VMDOSAPP
;
;DESCRIPTION: If no Grab event is already queued, call Shell_Event with grab
;	event for VMDOSAPP.
;
;ENTRY: EBX = VM handle for which grab occurred
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_VMDA_Grab,PUBLIC

	bts	[edi.VDD_Flags],fVDD_GrabBit	; Only one grab allowed
	jc	SHORT VVG_Exit
	mov	esi,OFFSET32 Vid_Grab_CallBack	; Clear grab flag on completion
	mov	edx,edi 			; Pass VDD CB ptr to call back
	mov	ecx,VDA_Display_Event		; Display event
	mov	eax,VDA_DE_ScreenGrabMin + SE_WP_PrtScBoost   ; Screen grab subfunction
	VxDCall Shell_Event			; Queue up the event
%OUT What if error from Shell_Event? Set up timeout and try later.
VVG_Exit:
	ret
EndProc VDD_VMDA_Grab

BeginProc Vid_Grab_CallBack
	and	[edx.VDD_Flags], NOT fVDD_Grab
	ret
EndProc Vid_Grab_CallBack


IFDEF	DEBUG
;******************************************************************************
;VDD_DebugQuery
;
;DESCRIPTION:
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
	VMMCall Get_Cur_VM_Handle
	call	Vid_DumpState
VDQ_00:
	VMMCall Get_Next_VM_Handle
	VMMCall Test_Cur_VM_Handle
	jz	SHORT VDQ_Ex
	call	Vid_DumpState
VDQ_Ex:
	popad
	ret

;*******
Vid_DumpState:
Trace_Out	"***********************************"
	cmp	ebx,[vgVDD.Vid_VM_Handle]
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
	mov	esi,[edi.VDD_Pg.VPH_MState.VDA_Mem_Addr]
Trace_Out	"Mem mode = #AL, alloc mask = #ECX, addr = #ESI"
	movzx	eax,[edi.VDD_Mode]
Trace_Out	"Controller mode = #AL"
Trace_Out	"Registers:"
Trace_Out	"CRTC:" +
	mov	al,[edi.VDD_Stt.V_Indx]
Trace_Out	"(#AL)" +
	mov	ecx,9
	lea	esi,[edi.VDD_Stt.V_HTotal]
	call	Vid_Reg_Dump
	mov	ecx,9
Trace_Out	" "
Trace_Out	"         " +
	call	Vid_Reg_Dump
Trace_Out	" "
	mov	al, [edi.VDD_Stt.V_xMode]
	mov	cl, [edi.VDD_Stt.V_Score]
	mov	dl, [edi.VDD_Stt.V_Strike]
Trace_Out	"         xMode(20) #al, Score(21) #cl, Strike(22) #dl"

	mov	al,[edi.VDD_Stt.V_Mode]
Trace_Out	"Mode:    #al"
	ret

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



