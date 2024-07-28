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
	INCLUDE OPTTEST.INC
	INCLUDE VDD.INC
	INCLUDE EGA.INC
	INCLUDE SHELL.INC
	INCLUDE VDDGRB.INC
	INCLUDE DEBUG.INC

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_State_Query:NEAR

	EXTRN	VDD_IO_SetTrap:NEAR

	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Do_Restore:NEAR
	EXTRN	VDD_Dont_Restore:NEAR
	EXTRN	VDD_Restore2:NEAR
	EXTRN	VDD_Cancel_Restore2:NEAR
	EXTRN	VDD_Cancel_Restore:NEAR
	EXTRN	VDD_Global_Restore:NEAR
	EXTRN	VDD_Save:NEAR
	EXTRN	VDD_Save2:NEAR
	EXTRN	VDD_Backgrnd_Event:NEAR

	EXTRN	VDD_Msg_NoSupMode:NEAR
	EXTRN	VDD_Msg_Exclusive:NEAR
	EXTRN	VDD_Msg_NoFGrnd:NEAR
	EXTRN	VDD_Msg_NoMainMem:NEAR

	EXTRN	VDD_Mem_CalcPage:NEAR
	EXTRN	VDD_Mem_Disable:NEAR
	EXTRN	VDD_Mem_Remap:NEAR
	EXTRN	VDD_Mem_MapNull:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_Mem_Physical2:NEAR
	EXTRN	VDD_Mem_ChkPag:NEAR
IFDEF	PEGA
	EXTRN	VDD_Map_PShdw:NEAR
	EXTRN	VDD_Alloc_PShdw:NEAR
	EXTRN	VDD_Dealloc_PShdw:NEAR
	EXTRN	VDD_Mem_QPShdw:NEAR
ENDIF
IFDEF	VGA
	EXTRN	VDD_SaveCtlr:NEAR
ENDIF

VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Msg_VM:DWORD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_SG0_JTab:DWORD
	EXTRN	VDD_SG2_JTab:DWORD
	EXTRN	VDD_PT_Buff:DWORD
	EXTRN	VDD_Win_Update_Time:DWORD

PUBLIC VDD_VMIdle_Chain
VDD_VMIdle_Chain    DD	?

PUBLIC VDD_SetTime_Chain
VDD_SetTime_Chain    DD  ?

VxD_DATA_ENDS

VxD_CODE_SEG

;******************************************************************************
;VDD_PFault
;
;DESCRIPTION:
;	VIDEO page faults occur in a number of cases.  For full screen VMs
;	they can occur when there is a mode change. After doing a few
;	checks to make sure that the mode change does not adversely
;	affect other VMs, the correct physical memory is remapped and
;	execution continues.  Full screen VMs can also page fault when
;	the application accesses more memory than has been set aside for
;	the VM, forcing the VM to run full screen, not background. For
;	windowed VMs, the same cases arise, but the action is a little
;	different.
;
;ENTRY: EAX = page number of faulting page
;
;EXIT:	PTE fixed or VM state modified so it won't run
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_PFault,PUBLIC

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	push	eax				; [ESP] = PTE number

	cmp	ebx, [Vid_VM_Handle]		; Q: Attached VM?
	je	VPF_Att 			;   Y: do memory remap

IFDEF	DEBUG
	pushfd
	cmp	[VDD_Msg_VM], 0 		; Message mode?
	jz	SHORT VPF_D000
	push	edx
	mov	edx,[VDD_Msg_VM]
Trace_Out "Video page fault in msg mode on VM #EDX, faulting VM #EBX, page #AX"
	pop	edx
VPF_D000:
	popfd
ENDIF


	VMMCall Test_Sys_VM_Handle		; Q: SYS VM touching mem
	je	VPF_MapNull			;   Y: It shouldn't

; Windowed or background VM page fault
VPF_RemapWindow:				;   N: Must be mode change
IFDEF	PEGA
	call	VDD_Mem_QPShdw			; Q: Paradise shadow page?
	jnc	VDD_Map_PShdw			;	Y: Go map it
ENDIF
	call	VDD_Mem_CalcPage		; ECX = page
	jc	VPF_MapNull			; jump if page not enabled
	cmp	ecx,8				; Q: Within 32k?
	jb	SHORT VPF_GoodPage		;   Y: Mode chg,remap the memory
						;   N: VM need 256k?
	TestMem [edi.VDD_Flags],fVDD_ModeSet	; Q: During mode set?
	jz	SHORT VPF_256			;   N: Need 256k memory
	jmp	VPF_MapNull			;   Y: Map null till mode set
VPF_GoodPage:
	cmp	ebx,[Vid_VM_Handle2]		; Q: 2nd EGA?
	jz	SHORT VPF_Remap0		;   Y: continue
	mov	[edi.VDD_MMode],-1		;   N: Force a remap

VPF_Remap0:
; This needs to take text mode into account and put less bits into PgAccMsk
%OUT Inefficient
	mov	eax, 1010101h
	shl	eax, cl
	or	[edi.VDD_PG.VPH_PgAccMsk], eax	; Indicate memory accessed

	call	VDD_Get_Mode
	call	VDD_Mem_Remap			; Remap the memory
	jc	SHORT VPF_NoRemap		;   if can't remap, suspend VM
VPF_Ok:
	mov	edx,[esp]
	shl	edx,12
	add	edx,[ebx.CB_High_Linear]
	shr	edx,12
	xor	eax,eax 			; EAX = 0
	VMMCall _CopyPageTable,<edx,1,<OFFSET32 VDD_PT_Buff>,eax>
	TestMem [VDD_PT_Buff],P_PRES		; Q: Did we map some mem here?
	jz	SHORT VPF_NoPage		;   Y: All done
	add	esp,4				; Remove PTE number from stack
	ret

; VM accessed memory between 32k and 64k, which means it must run 256k EGA
VPF_256:
	SetFlag [edi.VDD_Flags],fVDD_256	; Only run forground
	call	VDD_Msg_Exclusive
	VMMCall Test_Sys_VM_Handle		; Q: SYS VM?
	jz	SHORT VPF_MapNull		;   Y: Strange! Just map null
	jmp	SHORT VPF_NR_0

; Problem is either mode we don't understand or have not allocated memory
VPF_NoPage:
	mov	eax,[esp]
	call	VDD_Mem_CalcPage
	mov	eax,01010101h
	shl	eax,cl
	test	[edi.VDD_PG.VPH_PgAllMsk],eax	; Q: Page allocated?
	jnz	SHORT VPF_BadMode		;   Y: must be unsupported mode
	SetFlag [edi.VDD_EFlags],fVDE_NoMain
	call	VDD_Msg_NoMainMem
	jmp	SHORT VPF_Suspend

; Unsuccessful mem remap for unattached VM
VPF_NoRemap:
	VMMCall Test_Sys_VM_Handle		; Q: SYS VM
	jz	SHORT VPF_MapNull		;   Y: Strange! Just map null

	cmp	[VDD_Msg_VM], 0 		; Message mode?
	jnz	SHORT VPF_BadMode		;   Y: mode not supported

; Cannot run this VM in background with current forground application
	call	VDD_Msg_NoFGrnd

; Make sure VM does not run until later after setfocus or settype
VPF_NR_0:
	jmp	SHORT VPF_suspend

VPF_MapNull:
	pop	edx
Trace_Out "VDD: VM #EBX null page #DL"
	jmp	VDD_Mem_MapNull

; Was not able to figure out correct memory to map, just map null memory
VPF_BadMode:
	call	VDD_Msg_NoSupMode

VPF_suspend:
	pop	edx
	call	VDD_Mem_MapNull
; If message mode on this VM, cannot suspend
	cmp	[VDD_Msg_VM],ebx		; Q: Message mode?
	jz	SHORT VPF_Exit			;   Y: Don't suspend
	bts	[edi.VDD_Flags],fVDD_SuspEventBit ; Q: Suspension queued?
	jc	SHORT VPF_Exit			;   Y: all done
	push	ebx
	VMMcall Get_Crit_Section_Status 	; Q: Crit section owned ?
	pop	ebx
	mov	edx,ebx
	jnc	VPF_SuspendNow_Event		;   N: suspend now

; Need to suspend when critical section is unowned
	mov	esi,OFFSET32 VPF_SuspendNow_Event
	VMMCall Call_When_Not_Critical		;   Y: suspend later
VPF_Exit:
	ret


;**************
; We got page fault from attached VM, must be mode chg
VPF_Att:
IFDEF	VGA
	call	VDD_SaveCtlr			; Save state of this VM
ENDIF
IFDEF	PEGA
	call	VDD_Mem_QPShdw			; Q: Paradise shadow page?
	jnc	SHORT VPF_IsPShdwDisp 		;   Y: Go handle it
ENDIF
	mov	[edi.VDD_ModeEGA],-1
	call	VDD_Get_Mode			; Reinitialize mode
	mov	eax,[esp]
	call	VDD_Mem_CalcPage		; ECX = page
	jc	VPF_MapNull			; jump if page not enabled
	cmp	cl,8				; Q: Within 32k?
	jb	short VPF_NW1			;   Y: Try a mode change
						;   N: VM need 256k?
	TestMem [edi.VDD_Flags],fVDD_ModeSet	; Q: During mode set?
IFDEF	VGA
	jz	SHORT VPF_64k_user
	cmp	[edi.VDD_ModeEGA], 10h		;Q: extended VGA graphics mode?
	jbe	VPF_MapNull			;   N: Just map null memory
VPF_64k_user:
ELSE
	jnz	VPF_MapNull			;   Y: Just map null memory
ENDIF

; VM accessed memory between 32k and 64k, which means it must run 256k EGA
	VMMCall Test_Sys_VM_Handle		; Q: SYS VM
	jz	VPF_MapNull			;   Y: Strange! Just map null
	bts	[edi.VDD_Flags],fVDD_256Bit	; Only run with 256k
	jc	SHORT VPF_NW1			;   WEIRD (should not be set)
	call	VDD_Detach2			;   Detach any 2nd EGA guys

VPF_NW1:
	call	VDD_Mem_ChkPag			; Save dirty bits
	movzx	eax,[edi.VDD_ModeEGA]
	call	VDD_Mem_Physical		; Remap memory
	mov	edx,[esp]
	shl	edx,12
	add	edx,[ebx.CB_High_Linear]
	shr	edx,12
	xor	eax,eax 			; EAX = 0
	VMMCall _CopyPageTable,<edx,1,<OFFSET32 VDD_PT_Buff>,eax>
	TestMem [VDD_PT_Buff],P_PRES		; Q: Did we map some mem here?
	jz	VPF_MapNull			;   N: Just map null memory
	add	esp, 4
	ret

IFDEF	PEGA
VPF_IsPShdwDisp:
; map in the physical shadow ram for attached VMs
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdw
	mov	edx,0A0h
	VMMCall _PhysIntoV86,<edx,ebx,edx,1,0>
	add	esp,4
	ret
ENDIF
EndProc VDD_PFault

BeginProc VPF_SuspendNow_Event
	mov	edi,edx
	add	edi,[VDD_CB_Off]
	ClrFlag [edi.VDD_Flags], fVDD_SuspEvent ; Suspension dequeued
	bts	[edi.VDD_Flags],fVDD_WaitAttBit
IFDEF	DEBUG
	jnc	SHORT VPFSE_D0
Debug_Out "VDD: Already suspended VM: #EDX !?!?!"
VPFSE_D0:
ENDIF
	jc	SHORT VPFF_Exit
	SetFlag [edi.VDD_Flags],fVDD_WaitAtt	; VM is Suspended
	cmp	edx,[Vid_VM_Handle2]		; Q: Running in 2nd EGA?
	jnz	SHORT VPFF_Not2 		;   N: continue
	mov	ebx,edx
	call	VDD_Detach2			;   Y: Detach from 2nd EGA
VPFF_Not2:
Trace_Out "VDD: Suspending #EDX on VM #EBX"
	VMMcall Get_Sys_VM_Handle
	mov	eax, Low_Pri_Device_Boost
	mov	ecx, PEF_Wait_For_STI+PEF_Wait_Not_Crit+PEF_Always_Sched
	mov	esi, OFFSET32 VPF_Susp_Event
	VMMcall Call_Priority_VM_Event		; Set focus after VM suspended
	SetFlag [edi.VDD_Flags],fVDD_WaitAtt	; Suspended
	mov	ebx,edx
	VMMCall Suspend_VM
VPFF_Exit:
	ret
EndProc VPF_SuspendNow_Event

; Set focus away from suspended VM to system VM
;	and tell VMDOSAPP VM is suspended
BeginProc VPF_Susp_Event

	push	ebx
	mov	ebx,edx
	mov	ecx,VDA_Type_Chng		; Tell VMDOSAPP type has changed
	xor	eax,eax 			; No wParam
	xor	esi,esi 			; No Call back
	xor	edx,edx
	VxDCall SHELL_Event
	cmp	ebx, [VDD_Focus_VM]		; Q: This VM have focus?
	pop	ebx
	jne	SHORT VPFse_not_focus		;   N:
	mov	eax,Set_Device_Focus		;   Y: Setfocus to SYS VM
	xor	esi,esi 			;      No special flags
	xor	edx,edx 			;      Critical set device focus
	VMMCall System_Control
VPFse_not_focus:
	ret
EndProc VPF_Susp_Event



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
	TestMem [edi.VDD_Flags],fVDD_Win
	jz	SHORT VMI_00
	TestMem [edi.VDD_Flags],fVDD_ModeChange ; Q: In a mode change?
        jnz     SHORT VMI_00                    ;   Y: ignore  
	call	VDD_Clr_VM_Time_Out		;   N: Clear pending timeout
	call	VDD_Update_Window		; Do screen update if necessary
VMI_00:
	popad
	jmp	[VDD_VMIdle_Chain]
EndProc VDD_VMIdle

IF 0
;******************************************************************************
;VDD_Set_Time_Slice
;
;DESCRIPTION:
;	Validate that the proposed changes to forground/background status
;	bits is consistent with the mode and usage of the video hardware.
;	For example, reject (CF=1, do not chain) if the background bit
;	is set and this VM cannot run in the background.
;
;ENTRY: EAX = Flags (VMStat_Exclusive and/or VMStat_Background)
;	EBX = VM handle
;	ECX = Forground priority
;	EDX = Background priority
;
;EXIT:	IF CF=1, cancels Set_Time_Slice_Priority
;	    ELSE chain to next hooker of this service.
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Set_Time_Slice,SERVICE,High_Freq

	pushad
%OUT Set_Time_Slice checking...
	popad
	jmp	[VDD_SetTime_Chain]
EndProc VDD_Set_Time_Slice
ENDIF

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
;EXIT:
;
;USES:	Flags
;
;ASSUMES:
;
;==============================================================================
BeginProc VDD_VMTimeOut,PUBLIC,High_Freq
	mov	edi,edx 			; EDI = VM's VDD CB ptr
	ClrFlag [edi.VDD_Flags],fVDD_ModeChange
	mov	[edi.VDD_Time_Out_Handle],0
	TestMem [edi.VDD_Flags],fVDD_Win	; Q: Windowed VM?
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
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Update_Window,,High_Freq

	call	VDD_State_Query 		; Q: Video changed?
	jnc	SHORT no_upd
	inc	[edi.VDD_Update_Hit_Cnt]
	CallRet VDD_VMDA_Update 		;   Y: schedule update
no_upd:
	mov	[edi.VDD_Update_Hit_Cnt], 0
	Assumes_Fall_Through VDD_Set_VM_Time_Out
EndProc VDD_Update_Window


;==============================================================================
BeginProc VDD_Set_VM_Time_Out,PUBLIC,High_Freq
	cmp	[edi.VDD_Time_Out_Handle],0	; Q: Timeout active?
	jnz	SHORT VSVTO_Exit
	mov	eax, [VDD_Win_Update_Time]	; Display update time delay
	TestMem [edi.VDD_Flags],fVDD_ModeChange
        jz      SHORT VSVTO_00
	mov	eax,VDD_ModeChangeDelay
	jmp	short VSVTO_01

VSVTO_00:
	cmp	[edi.VDD_Update_Hit_Cnt], 3
	jb	short VSVTO_01
	shl	eax, 2				; increase timeout time 4 times
VSVTO_01:
	mov	edx,edi
	mov	esi,OFFSET32 VDD_VMTimeOut
	VMMCall Set_VM_Time_Out 		; Set display update timeout
	mov	[edi.VDD_Time_Out_Handle],esi	; Save timeout handle
VSVTO_Exit:
	ret
EndProc VDD_Set_VM_Time_Out

;==============================================================================
BeginProc VDD_Clr_VM_Time_Out,PUBLIC,High_Freq
	TestMem [edi.VDD_Flags],fVDD_ModeChange
        jnz     SHORT CVT_00
	xor	esi, esi
	xchg	esi, [edi.VDD_Time_Out_Handle]
	VMMCall Cancel_Time_Out 		; Cancel pending update timeout
CVT_00:
	ret
EndProc VDD_Clr_VM_Time_Out

;******************************************************************************
;VDD_Detach
;
;DESCRIPTION: Detach any currently attached or scheduled to be attached VM.
;	    NOTE: Assumes NO OUTSTANDING RESTORE EVENTS.  Cancel restore
;	    events before calling this routine.
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
	mov	ebx,[Vid_VM_Handle]
	test	ebx,ebx
	jz	SHORT VD_Ex			; Nothing currently attached

; If a restore event is pending, we need to cancel it in case the memory
;   allocation within VDD_Save causes the MMGR to want to grow the paging
;   file (we don't want to restore until the detach is complete).  The
;   "DO_RESTORE" at the end will put everything right again.
	call	VDD_Dont_Restore

	mov	edi,ebx
	add	edi,[VDD_CB_Off]

IFNDEF VGA8514
	VMMCall Test_Sys_VM_Handle
	jne	SHORT skip_bkgnd_notify

; Notify VM that it is going into background

%OUT add logic for suspended VM - cancel event, etc.
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Backgrnd_Event
	mov	eax,VDD_Pri_Device_Boost
	mov	ecx,0
	VMMCall Call_Priority_VM_Event		; Tell VM it is losing screen

skip_bkgnd_notify:
ENDIF

; Save the state of the attached VM
;   Note that the Vid_VM_Handle must not be cleared until the attached
;	VM's state is saved.
	call	VDD_Save			; Save attached VM's state
	mov	[Vid_VM_Handle],0

; Set up for background
	call	VDD_IO_SetTrap			; Set up IO trapping
	call	VDD_Mem_Disable 		; Force remap of memory

; Set up restore event for VM with focus
	call	VDD_Do_Restore
VD_Ex:
	pop	edi
	pop	ebx
	ret
EndProc VDD_Detach


;******************************************************************************
;VDD_Attach2
;
;DESCRIPTION:
;	Give a VM the 2nd EGA device.  This is called by VDD_Remap when
;	executing a complex EGA VM in the background.
;
;ENTRY: EBX = VM Handle of VM to attach
;	EDI = VDD CB ptr for VM
;
;EXIT:	CF = 1 if cannot run VM in background
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Attach2,PUBLIC

	TestMem [edi.VDD_Flags],fVDD_256	; Q: Need 256k video RAM?
	jnz	SHORT VA2_256			;   Y: Don't run this VM bckgnd
	push	edi
	mov	edi,[VDD_Focus_VM]
	add	edi,[VDD_CB_Off]
	TestMem [edi.VDD_Flags],fVDD_256	; Q: Attached VM a RAM hog?
	pop	edi
	jnz	SHORT VA2_WaitFocusChg		;   Y: Suspend for now
; Detach current 2nd EGA owner and attach new one
	call	VDD_Detach2			; Detach current owner
	mov	edx,edi
	call	VDD_Restore2			; Restore this VM 2nd EGA
	call	VDD_Mem_Physical2		; Map memory to 2nd EGA
	clc
	ret
VA2_256:
	call	VDD_Msg_Exclusive
	jmp	SHORT VA2_No2EGA
VA2_WaitFocusChg:
;;;IFDEF   DEBUG
;;;	push	ecx
;;;	mov	ecx,[VDD_Focus_VM]
;;;Debug_Out "VDD: Cannot attach 2nd VM #EBX because of attached VM #ECX"
;;; When VM video hog VM started, should have gotten exclusive message
;;;   so just return error with no message
;;;	pop	ecx
;;;ENDIF
VA2_No2EGA:
	stc
	ret
EndProc VDD_Attach2

;******************************************************************************
;VDD_Detach2	    Detach complex EGA mode VM from 2nd EGA
;
;DESCRIPTION: If a VM is currently running in the second half of the EGA,
;	its state is saved and it is detached.
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags, EAX
;
;==============================================================================
BeginProc VDD_Detach2,PUBLIC

	cmp	[Vid_VM_Handle2],0
	jz	SHORT VD2_Done			; Nothing currently attached
	push	ebx
	push	edi
	mov	ebx,[Vid_VM_Handle2]
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Get_Mode			; V_ModeEGA = current mode
	call	VDD_Save2			; Save current state
	SetFlag [edi.VDD_Flags],fVDD_Save	; Indicate state saved
	mov	[Vid_VM_Handle2],0		; Display is detached.
	call	VDD_Cancel_Restore2		; Cancel outstanding restores
	mov	[edi.VDD_Routine],OFFSET32 VDD_SG0_JTab ; Set up trap rtns
	call	VDD_Mem_Disable 		; Disable memory(force remap)
VD2_Exit:
	pop	edi
	pop	ebx
VD2_Done:
	clc
	ret
EndProc VDD_Detach2


;******************************************************************************
;VDD_VMDA_Update    Send update event to VMDOSAPP
;
;DESCRIPTION: If an update event is already queued, do nothing, else
;	call Shell_Event with update event for VMDOSAPP.
;
;ENTRY: EBX = VM handle for which grab occurred
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
	push	ebx
	VMMcall Get_Sys_VM_Handle
	mov	eax, Low_Pri_Device_Boost
	mov	ecx, PEF_Wait_For_STI+PEF_Wait_Not_Crit+PEF_Always_Sched
	mov	edx,edi 			; Pass VDD CB ptr to call back
	mov	esi, OFFSET32 VV_Update_Event
	VMMcall Call_Priority_VM_Event
	pop	ebx
VVU_Exit:
	ret
EndProc VDD_VMDA_Update

;******************************************************************************
;VV_Update_Event    Send update event to VMDOSAPP
;
;DESCRIPTION: Call Shell_Event with update event for VMDOSAPP.	This should
;	    only be called when critical section is unowned.
;
;ENTRY: EBX = SYS VM handle
;	EDX = VDD CB ptr for which grab occurred
;
;EXIT:	none
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VV_Update_Event

	mov	ebx,edx
	sub	ebx,[VDD_CB_Off]
	VMMCall Validate_VM_Handle
	jc	SHORT VVUE_Exit
	mov	esi,OFFSET32 Vid_Upd_CallBack	; Clear grab flag on completion
	mov	ecx,VDA_Display_Event		; Display event
	mov	eax,VDA_DE_DispChngMin		; subfunction - display change
                                                ;       no ctlr change
	VxDCall Shell_Event			; Queue up the event
        jc      SHORT Vid_Upd_CallBack
VVUE_Exit:
	ret
EndProc VV_Update_Event

;==============================================================================
BeginProc Vid_Upd_CallBack
	ClrFlag [edx.VDD_Flags], fVDD_Upd
	mov	edi,edx
	mov	ebx,edx
	sub	ebx,[VDD_CB_Off]
	VMMCall Validate_VM_Handle
	jnc	VDD_Set_VM_Time_Out		; schedule next timeout
	ret
EndProc Vid_Upd_CallBack

;******************************************************************************
;VDD_VMDA_Grab	    Send grab event to VMDOSAPP
;
;DESCRIPTION: If no Grab event is already queued, set up event to call
;	Shell_Event with grab event for VMDOSAPP.
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
	push	ebx
	VMMcall Get_Sys_VM_Handle
	mov	eax, Low_Pri_Device_Boost
	mov	ecx, PEF_Wait_For_STI+PEF_Wait_Not_Crit+PEF_Always_Sched
	mov	edx,edi 			; Pass VDD CB ptr to call back
	mov	esi, OFFSET32 VV_Grab_Event
	VMMcall Call_Priority_VM_Event
	pop	ebx
VVG_Exit:
	ret
EndProc VDD_VMDA_Grab

;******************************************************************************
;VV_Grab_Event	    Send grab event to VMDOSAPP
;
;DESCRIPTION: call Shell_Event with grab event for VMDOSAPP.
;
;ENTRY: EBX = VM handle for which grab occurred
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VV_Grab_Event,PUBLIC

	mov	ebx,edx
	sub	ebx,[VDD_CB_Off]
	VMMCall Validate_VM_Handle
	jc	SHORT VVGE_Exit
	mov	esi,OFFSET32 Vid_Grab_CallBack	; Clear grab flag on completion
	mov	ecx,VDA_Display_Event		; Display event
	mov	eax,VDA_DE_ScreenGrabMin+SE_WP_PrtScBoost ; Screen grab
	VxDCall Shell_Event			; Queue up the event
	jc	SHORT Vid_Grab_CallBack
VVGE_Exit:
	ret
EndProc VV_Grab_Event

;==============================================================================
BeginProc Vid_Grab_CallBack
	ClrFlag [edx.VDD_Flags], fVDD_Grab
%OUT redundant release of copy memory in case of errors?
	ret
EndProc Vid_Grab_CallBack


VxD_CODE_ENDS

	END
