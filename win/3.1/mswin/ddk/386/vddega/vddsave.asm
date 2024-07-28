       TITLE   VDD - Virtual Display Device for EGA   version 3.0  1/89
;******************************************************************************
;
;VDDSAVE - Virtual Display Device VM Save/Restore
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986-1991
;
;
;DESCRIPTION:
;	This module uses the VDD control block structure to save and restore
;	a VM to/from the physical video.  This includes the video controller as
;	well as the video RAM.	Note that restoring the 2nd half EGA VM
;	means restoring a subset of the controller registers and offsetting
;	the video memory(into the "2nd half").
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC
	INCLUDE EGA.INC
	INCLUDE SHELL.INC
	INCLUDE VDD.INC
	INCLUDE VDDGRB.INC
IFDEF	VGA
	INCLUDE VGA.INC
ENDIF
	INCLUDE DEBUG.INC

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_Msg_NoCopyMem:NEAR
	EXTRN	VDD_Msg_NoMainMem:NEAR
        EXTRN   VDD_Msg_CannotGrab:NEAR
	EXTRN	VDD_VMDA_Grab:NEAR
	EXTRN	VDD_IO_SetTrap:NEAR
	EXTRN	VDD_Mem_Disable:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_Mem_Physical2:NEAR
	EXTRN	VDD_Mem_Chg:NEAR
	EXTRN	VDD_Mem_Null:NEAR
	EXTRN	VDD_Mem_AMain:NEAR
	EXTRN	VDD_Mem_ACopy:NEAR
	EXTRN	VDD_Mem_DCopy:NEAR
	EXTRN	VDD_Mem_Test_256k:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_Scrn_Size:NEAR
	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Mem_UpdOff:NEAR
	EXTRN	VDD_Mem_MakeCopy:NEAR
        EXTRN   VDD_SetCopyMask:NEAR
IFDEF	VGA
	EXTRN	VDD_SaveCtlr:NEAR
	EXTRN	VDD_Rest_DAC:NEAR
IFDEF	V7VGA
	EXTRN	V7_RestCTLR:NEAR
	EXTRN	V7_RestCRTC:NEAR
ENDIF
IFDEF	ATIVGA
	EXTRN	ATi_RestCTLR:NEAR
	EXTRN	ATi_RestCRTC:NEAR
ENDIF
ENDIF

VxD_CODE_ENDS

;******************************************************************************
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Msg_VM:DWORD
	EXTRN	VDD_Scrn_Attr:BYTE
	EXTRN	VDD_SGx_JTab:DWORD
	EXTRN	VDD_SG2_JTab:DWORD

IFDEF	DEBUG
Latches_VM dd  0
ENDIF

PUBLIC VDD_Msg_Pseudo_VM, VDD_Msg_Pseudo_CB
VDD_Msg_Pseudo_VM   dd OFFSET32 VDD_Msg_Pseudo_CB
VDD_Msg_Pseudo_CB   db SIZE CB_EGA_Struc DUP (?)

FontLen 	    EQU 1024*8		 ; 8k font
VDD_MsgFont	    DB	FontLen DUP (?)

PUBLIC	VDD_Rest_Event_Handle, VDD_Rest2_Event_Handle
VDD_Rest_Event_Handle DD  ?
VDD_Rest2_Event_Handle DD  ?

; Save/restore addresses initialized by Sys_Critical_Init
PUBLIC	VDD_PhysA0000, VDD_Latch_Addr
VDD_PhysA0000	    DD	?		; Address of physical mem at A0000
VDD_Latch_Addr	    DD	?		; Address to save/restore latches

PUBLIC	VDD_Phys2EGA, VDD_2EGA_Start
VDD_Phys2EGA	    DD	?		; Address of physical mem at 2EGA_Start
VDD_2EGA_Start	    DD	?		; Start of "2nd EGA"


IFDEF	EGA
;******************************************************************************
; The order of this is defined by EGAMEMD.INC in WINDOWS 2.0 source code
;   it occupies the last 16 bytes of each HIRES plane in the EGA
;   It is used by the EGASTATE.ASM save and restore code. The memory is
;   restored when SYS VM is detached from and attached to the display.
;	Brush		    8 bytes
;	Tonys_Bar_n_Grill   not initialized(byte)
;	shadow_mem_status   6 = shadow mem exists and can be used
;	known_word	    =00FFh(word)
;	shadowed_mode	    =0(mode register always 0 when leave VM1)(byte)
;	plane_index	    not initialized(byte)
;	enable_test	    =0 in planes 0 and 1, FFh in planes 2 and 3(byte)
;	saved_latches	    not initialized(byte)
PUBLIC	VDD_Sys_VRAMAddr,
VDD_Sys_VRAMAddr    DD	?		; Address of SYS VM's special VRAM
saved_bit_mask	DB	?		; Temp during save_hw_regs

;PUBLIC  VDD_SYS_VRAM
;VDD_SYS_VRAM	 DB	 0,0,0,0,0,0,0,0	     ; plane 0 brush
;		 DB	 0FFh,6,0FFh,0,0,0,0,0	     ; plane 0 state
;		 DB	 0,0,0,0,0,0,0,0	     ; plane 1 brush
;		 DB	 0FFh,6,0FFh,0,0,1,0,0	     ; plane 1 state
;		 DB	 0,0,0,0,0,0,0,0	     ; plane 2 brush
;		 DB	 0FFh,6,0FFh,0,0,2,0FFh,0    ; plane 2 state
;		 DB	 0,0,0,0,0,0,0,0	     ; plane 3 brush
;		 DB	 000h,6,0FFh,0,0,3,0FFh,0    ; plane 3 state
;.ERRNZ  ($-VDD_SYS_VRAM)-(4*SYS_VRAM_Len)
ENDIF


; Mask for 1st half video memory restore
; Mode B has Mode 3 mask to support BIOS call 1C01/1C02H
VDD_RestMsk	label	dword
	dd	000FFFFFFh		; mode 0
	dd	000FFFFFFh		; mode 1
	dd	000FFFFFFh		; mode 2
	dd	000FFFFFFh		; mode 3
	dd	00000FFFFh		; mode 4
	dd	00000FFFFh		; mode 5
	dd	00000FFFFh		; mode 6
	dd	000FFFFFFh		; mode 7
	dd	000000000h		; mode 8, not supported
	dd	000000000h		; mode 9, not supported
	dd	000000000h		; mode A, not supported
	dd	000FFFFFFh		; mode B, not supported
	dd	000000000h		; mode C, not supported
	dd	0FFFFFFFFh		; mode D
	dd	0FFFFFFFFh		; mode E
	dd	000FF00FFh		; mode F
	dd	0FFFFFFFFh		; mode 10
	dd	0000000FFh		; mode 11
	dd	0FFFFFFFFh		; mode 12
	dd	0000000FFh		; mode 13


; Mask for 2nd half video memory restore
VDD_Rest2Msk	label	dword
	dd	000000000h		; mode 0
	dd	000000000h		; mode 1
	dd	000000000h		; mode 2
	dd	000000000h		; mode 3
	dd	000000000h		; mode 4
	dd	000000000h		; mode 5
	dd	000000000h		; mode 6
	dd	000000000h		; mode 7
	dd	000000000h		; mode 8
	dd	000000000h		; mode 9
	dd	000000000h		; mode A
	dd	000000000h		; mode B
	dd	000000000h		; mode C
	dd	0FFFFFFFFh		; mode D
	dd	0FFFFFFFFh		; mode E
	dd	000FF00FFh		; mode F
	dd	0FFFFFFFFh		; mode 10
	dd	000000003h		; mode 11
	dd	0FFFFFFFFh		; mode 12
	dd	0000000FFh		; mode 13

Public VDD_Grb_MskTab
; Mask for memory to save for a grab
VDD_Grb_MskTab	LABEL DWORD
	DD	000000001h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000001h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	00000000Fh		; mode 4, 320x200x2 graphics
	DD	00000000Fh		; mode 5, 320x200x2 graphics
	DD	00000000Fh		; mode 6, 640x200x1 graphics
	DD	00000000Fh		; mode 7, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000000000h		; mode B not supported
	DD	000000000h		; mode C not supported
	DD	003030303h		; mode D, 320x200 graphics, 4 32k planes
	DD	00F0F0F0Fh		; mode E, 640x200 graphics, 4 32k planes
	DD	0007F007Fh		; mode F, 640*350 graphics, 2 28k planes
	DD	07F7F7F7Fh		; mode 10,640*350 graphics, 4 28k planes
IFDEF	VGA
	DD	0000000FFh		; mode 11, 640x480 graphics, 1 38k pln
	DD	0FFFFFFFFh		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln
ENDIF


VxD_DATA_ENDS

VxD_CODE_SEG
;******************************************************************************
;VDD_Save		Save display state
;
;DESCRIPTION:
;	Saves memory and current controller state when detaching VM from
;	physical display adapter.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Save,PUBLIC

Assert_VDD_ptrs ebx,edi

; Save video state
	call	VDD_Scrn_Off			; Turn off screen
	call	VDD_SR_Setup			; Get set for save
	call	VDD_Get_Mode			; VDD_ModeEGA = mode
	VMMCall Test_Sys_VM_Handle		; Q: SYS VM?
	jz	SHORT VS_Ex			;   Y: Don't save memory
	mov	esi,[VDD_PhysA0000]
	stc
	call	VDD_SRAM			;   N: Save VM's memory
VS_Ex:
	ret
EndProc VDD_Save

;******************************************************************************
;VDD_Backgrnd_Event
;
;DESCRIPTION:
;	Informs VM that it is not running with the display
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Backgrnd_Event

	pushad
	mov	edi,edx
	cmp	ebx,[Vid_VM_Handle]		; Q: VM still attached?
	jnz	SHORT VEB_Detached		;   N: Set up before INT 2F

	btr	[Vid_Flags],fVid_CritBit	; Q: In VDD critical section?
	jnc	SHORT VEB_Normal		;   N: Normal INT 2F
	mov	eax, -Critical_Section_Boost
	VMMCall Adjust_Exec_Priority		;   Y: End Critical section

VEB_Detached:
	call	VDD_Mem_Chg
	call	VDD_Mem_Null			; Null mem for bogus INT 2F
	call	VEB_CallApp
	call	VDD_Mem_Disable 		; Force remap later
	popad
	ret

VEB_Normal:
	call	VEB_CallApp
	popad
	ret

VEB_CallApp:
	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], 4001h
	mov	eax,2Fh
	VMMcall Exec_Int			; VM detaches from display
	VMMcall End_Nest_Exec
	Pop_Client_State
	ret
EndProc VDD_Backgrnd_Event


;******************************************************************************
;VDD_Restore		Restore the display state
;
;DESCRIPTION:
;	This routine sets up an event to restore the video state for the
;	focus VM.
;
;ENTRY: EBX = focus VM handle
;	EDI = focus VM VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Restore,PUBLIC

Assert_VDD_ptrs ebx,edi
IFDEF	DEBUG
	cmp	ebx, [VDD_Focus_VM]
	je	SHORT VR_D00
Debug_Out "VDD:Restore called with vm #ebx which isn't the focus!!"
VR_D00:
ENDIF

	btr	[edi.VDD_Flags],fVDD_WaitAttBit
	jnc	SHORT VSF_03
	VMMCall Resume_VM
IFDEF	DEBUG
	jnc	SHORT VSF_D0
Debug_Out "ERROR: VDD:Cannot resume VM with focus!!"
VSF_D0:
ENDIF
VSF_03:
	cmp	[VDD_Rest_Event_Handle],0	; Q: Restore pending?
	jnz	SHORT VR_Exit			;   Y: All done
	push	edx
	push	esi
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Restore_Event
	VMMCall Schedule_VM_Event
	mov	[VDD_Rest_Event_Handle],esi
	pop	esi
	pop	edx
VR_Exit:
	ret
EndProc VDD_Restore


;******************************************************************************
;VDD_Do_Restore
;
;DESCRIPTION:
;	This routine cancels the DON'T RESTORE request.
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Do_Restore,PUBLIC
	mov	[VDD_Rest_Event_Handle],0	; Disable restore events
	jmp	VDD_Global_Restore
EndProc VDD_Do_Restore

;******************************************************************************
;VDD_Dont_Restore
;
;DESCRIPTION:
;	This routine cancels any restore event for the specified VM and prevents
;	further restore events.
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Dont_Restore,PUBLIC
	pushad
	mov	ebx,[VDD_Focus_VM]
	call	VDD_Cancel_Restore
	or	eax,-1
	mov	[VDD_Rest_Event_Handle],eax	; Disable restore events
	popad
	ret
EndProc VDD_Dont_Restore

;******************************************************************************
;VDD_Cancel_Restore
;
;DESCRIPTION:
;	This routine cancels any restore event for the specified VM.
;
;ENTRY: EBX = VM handle of current or previous focus VM, which might have
;	      a restore event scheduled.
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Cancel_Restore,PUBLIC
	push	esi
	xor	esi,esi
	xchg	esi,[VDD_Rest_Event_Handle]
	inc	esi
	jz	SHORT VCR_No_Event
	dec	esi
	jz	SHORT VCR_No_Event
	VMMCall Cancel_VM_Event 		; Cancel restore of attached VM
VCR_No_Event:
	pop	esi
	ret
EndProc VDD_Cancel_Restore

;******************************************************************************
;VDD_Restore_Event	Restore the display state
;
;DESCRIPTION:
;	This routine is a VM event procedure. It is called for a VM that
;	needs to be restored. By examining the VDD state, it will decide to
;	restore the memory or the controller state.
;	If the current VM is the one to be restored, this routine can be
;	called directly, short circuiting the event mechanism.
;
;ENTRY: EBX = current VM handle
;	EDX = current VM VDD ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Restore_Event

	mov	[VDD_Rest_Event_Handle],0
Assert_VDD_ptrs ebx,edx
	mov	edi,edx
	cmp	[VDD_Msg_VM], 0 		; Q: Message mode?
	jnz	VRE_Exit			;   Y: Do nothing
	cmp	ebx,[Vid_VM_HandleRun]		; Q: Already restored state?
	jz	VRE_Exit			;   Y: Do nothing
	cmp	ebx,[Vid_VM_Handle]		; Q: Just the controller state?
	jz	VRE_04				;   Y: Do ctlr state restore
IFDEF	DEBUG
	cmp	[Vid_VM_Handle],0
	jz	SHORT VRE_D01
Debug_Out   "VDD: Restore when another VM attached to display!!"
VRE_D01:
ENDIF
	mov	[Vid_VM_Handle],ebx		; Set new handle
	call	VDD_SR_Setup			; Set up for restore
	call	VDD_Scrn_Off			; Turn off the CRT
	call	VDD_IO_SetTrap			; Set up I/O trapping
	call	VDD_Get_Mode			; AL = mode
	call	VDD_Mem_Physical		; Remap physical memory
	call	VDD_RestCRTC			; Restore display (CRTC)
	mov	esi,[VDD_PhysA0000]		; EDI = start of VRAM
	movzx	eax,[edi.VDD_ModeEGA]
	VMMCall Test_Sys_VM_Handle		;Q: SYS VM?
	jnz	SHORT VRE_00			;   N: Restore the memory
IFNDEF	VGA8514
	call	VDD_Clear_RAM			;   Y: Just clear the memory
ENDIF
	jmp	SHORT VRE_02
VRE_00:
IF2
%OUT	SPEED: only restore visible mem and disable rest
ENDIF
	xor	edx,edx 			; assume only first 32k
	TestMem [edi.VDD_Flags],fVDD_256	; Q: Special 256k VM?
	jz	SHORT VRE_01			;   N: Restore <4*32k
	mov	edx,VDD_Rest2Msk[eax*4] 	;   Y: EDX = extended pages mask
VRE_01:
	push	ebp
	mov	ebp,VDD_RestMsk[eax*4]		; Get mask for first 32k pages
	call	VDD_Restore_RAM
	pop	ebp

VRE_02:
	ClrFlag [Vid_Flags],fVid_DOff		; Cancel global display off
	call	VDD_RestCtlr			; Restore mem access, indices
	mov	[edi.VDD_DirtyPages],0		; Clear pages mod'd
	mov	[edi.VDD_DirtyPages2],0
	call	VDD_Global_Restore		; If necessary, set 2nd EGA

IFNDEF VGA8514
	VMMCall Test_Sys_VM_Handle
	jne	SHORT skip_bkgnd_notify2

	xor	eax,eax 			; No priority boost
						; EBX = VM handle
	mov	ecx, PEF_Wait_For_STI+PEF_Wait_Not_Crit
	mov	edx,ebx 			; EDX = VM handle
	mov	esi,OFFSET32 VDD_FGrnd_Notify
	VMMCall Call_Priority_VM_Event

skip_bkgnd_notify2:
ENDIF
	ClrFlag [edi.VDD_Flags], fVDD_NTSave	; Reinitialize saved flag
VRE_Exit:
	ret

VRE_04:
; Restore just the registers modified by background VMs
	call	VDD_SR_Setup			; Set up for restore
	call	VDD_RestCtlr			; Restore mem access, indices

IFDEF	EGA
	TestMem [edi.VDD_Flags],fVDD_NoTrap
	jz	SHORT VRE_Exit

	VMMCall Test_Sys_VM_Handle
IFDEF	DEBUG
	je	SHORT VRE_D00
	Debug_Out "VDD: no trap not sys VM not supported"
	jmp	SHORT VRE_Exit
VRE_D00:
ENDIF
	jne	SHORT VRE_Exit

	btr	[edi.VDD_Flags],fVDD_NTSaveBit	; Q: Did save?
IFDEF	DEBUG
	jc	SHORT VRE_D02
Debug_Out "VDD:No Save of Register state!?"
VRE_D02:
ENDIF
	jnc	SHORT VRE_05			;   N: WEIRD!!
	btr	[edi.VDD_EFlags],fVDE_CritBit
	jc	SHORT VRE_05
	call	VDD_Res_HW_Regs
	jmp	VRE_Exit

; Error: need to simulate background/forground so will reinit
VRE_05:
	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], 4001h
	mov	eax,2Fh
	VMMcall Exec_Int			; VM detaches from display

; do a 2nd restore, in case the 4001h handler decided to change registers,
; because 4002h is documented as having the register state in the last known
; state
	call	VDD_SR_Setup			; Set up for restore
	call	VDD_RestCtlr			; Restore mem access, indices

	mov	[ebp.Client_AX], 4002h
	mov	eax,2Fh
	VMMcall Exec_Int			; VM reinitializes display
	VMMcall End_Nest_Exec

	Pop_Client_State
ENDIF
	jmp	VRE_Exit
;
; Tell app no longer in background
BeginProc VDD_FGrnd_Notify
	mov	ebx,edx
	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], 4002h
	mov	eax,2Fh
	VMMcall Exec_Int			; VM reinitializes display
	VMMcall End_Nest_Exec
	Pop_Client_State
	ret
EndProc VDD_FGrnd_Notify



EndProc VDD_Restore_Event

;******************************************************************************
;VDD_Save2		 Save 2nd EGA display state
;
;DESCRIPTION:
;	Saves the state of the 2nd EGA.
;
;ENTRY: EDI = VDD CB ptr of VM running in 2nd EGA
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Save2, PUBLIC

Assert_VDD_ptrs ebx,edi
; For notrap case, tell VM to save registers
	pushad
	mov	ebx,edi
	sub	ebx,[VDD_CB_Off]		; EBX = VM handle
	call	VDD_SR_Setup			; Get set for save
	call	VDD_Get_Mode			; VDD_ModeEGA = mode
	mov	esi,[VDD_Phys2EGA]		; ESI = start of physical VRAM
	stc
	call	VDD_SRAM			; EGA mode save
VS2_Exit:
	popad
	ret
EndProc VDD_Save2


;******************************************************************************
;VDD_Restore2		Restore the 2nd EGA display state
;
;DESCRIPTION:
;	This routine sets up an event to restore the video state for the
;	2nd EGA VM
;
;ENTRY: EBX = VM handle to be attached to 2nd EGA
;	EDI = VM VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Restore2,PUBLIC

	cmp	[VDD_Rest2_Event_Handle],0
	jnz	SHORT VR2_Exit
	push	edx
	push	esi
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Restore2_Event
	VMMCall Schedule_VM_Event
	mov	[VDD_Rest2_Event_Handle],esi
	pop	esi
	pop	edx
VR2_Exit:
	ret
EndProc VDD_Restore2

;******************************************************************************
;VDD_Cancel_Restore2
;
;DESCRIPTION:
;	This routine cancels the restore event for 2nd EGA VM.
;
;ENTRY: EBX = VM handle that was attached to 2nd EGA
;	EDI = VM VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Cancel_Restore2,PUBLIC
	push	esi
	xor	esi,esi
	xchg	esi,[VDD_Rest2_Event_Handle]
	VMMCall Cancel_VM_Event 		; Cancel restore of attached VM
	pop	esi
	ret
EndProc VDD_Cancel_Restore2

;******************************************************************************
;VDD_Restore2_Event
;
;DESCRIPTION:
;	This routine is a VM event procedure. It is called for a VM that needs
;	to be restored to the 2nd EGA. By examining the VDD state, it will
;	decide to restore the memory, controller state or part of the
;	controller state. If the current VM is the one to be restored,
;	this routine can be called directly, short circuiting the event
;	mechanism.
;
;ENTRY: EBX = current VM handle
;	EDX = current VM VDD ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Restore2_Event,PUBLIC

	mov	[VDD_Rest2_Event_Handle],0
	mov	edi,edx
IFDEF	DEBUG
	sub	edx,[VDD_CB_Off]
	cmp	edx,ebx
	jz	SHORT VER2_D000
Debug_Out   "VDD: Restore2 to not current VM!?!?!?!"
VER2_D000:
ENDIF
	cmp	ebx,[Vid_VM_HandleRun]		; Q: Already restored state?
	jz	DEBFAR VER2_Exit		;   Y: Do nothing
	cmp	ebx,[Vid_VM_Handle2]		; Q: Just the controller state?
	jz	SHORT VER2_01			;   Y: Do ctlr state restore
IFDEF	DEBUG
	cmp	[Vid_VM_Handle2],0
	jz	SHORT VER2_D01
Debug_Out   "VDD: Restore2 when another VM attached to 2nd EGA!!"
VER2_D01:
ENDIF
	call	VDD_SR_Setup			; Set up for restore
	call	VDD_IO_SetTrap			; Set up I/O trapping
	call	VDD_Get_Mode			; AL = mode
	movzx	eax,[edi.VDD_ModeEGA]
IF2
%OUT	SPEED: Disable mem and restore pages as page faults occur
ENDIF
	xor	edx,edx 			; Get mask for extended pages
	push	ebp
	mov	ebp,VDD_RestMsk[eax*4]		; Get mask for first 32k pages
	mov	esi,[VDD_Phys2EGA]		; ESI = start of physical VRAM
	call	VDD_Restore_RAM 		; Restore 2nd EGA RAM
	pop	ebp
	mov	[Vid_VM_Handle2],ebx
	mov	[edi.VDD_DirtyPages],0		; Clear pages mod'd
	mov	[edi.VDD_DirtyPages2],0
	jmp	SHORT VER2_02			; Skip SaveCtlr2(done above)
VER2_01:
; Restore just the registers modified by background VMs
	call	VDD_SR_Setup			; Set up for restore
VER2_02:
	call	VDD_Mem_Physical2		; Remap physical memory
	jmp	VDD_RestCtlr			; Restore mem access, indices
VER2_Exit:
	ret
EndProc VDD_Restore2_Event

;******************************************************************************
;VDD_Grab
;
;DESCRIPTION:
;	Copy the physical screen to pages indentified by VDD_CPg
;	and copy ctrlr state in VDD_Stt to VDD_SttCpy
;
;ENTRY: EBX = VM Handle of current VM
;
;EXIT:	If error (memory allocation), does nothing.
;	Physical VRAM saved in VRAM copy, controller state in state copy
;	Event set up for VMDOSAPP
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_Grab,PUBLIC

	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cmp	ebx,[Vid_VM_Handle]    ; Q: Attached VM?
	jnz	VG_WindGrab	       ;   N: Windowed grab
; Full screen grab
	call	VDD_Get_Mode
IFDEF VGA
        cmp     eax,013h               ; Q: Mode 13 grab?
        je      SHORT VG_GrabNotSup    ;   Y: not allowed
        cmp     eax,012h
ELSE
        cmp     eax,010h
ENDIF
        jne     SHORT VG_ModeOK
        test    [edi.VDD_Stt.G_Misc],02h ; Q: Odd planes chained to even planes?
        jnz     SHORT VG_GrabNotSup      ;   Y: strange mode 12 - no grabs
        
VG_ModeOK:
IFDEF	VGAMONO
	cmp	eax,07
	je	SHORT VG_FSText
ENDIF
        cmp     eax,03
        ja      SHORT VG_FSGrfx

VG_FSText:
	call	VDD_SaveRegs			; Read all untrapped regs
        call    VDD_GrabTextMode
	jc	SHORT VG_Exit
	call	VDD_State_Update		; save ctrlr state in copy
	call	VDD_VMDA_Grab			; Tell VMDA about grab
	bt	DWORD PTR [edi.VDD_Stt.A_Indx],fVAI_ScOnBit
	cmc					; Q: attached VM CRT on?
	jmp	VDD_RestIndx			; Restore indices
; Graphics mode grab
VG_FSGrfx:
	call	VDD_SR_Setup			; Set up for save
	call	VDD_Get_Mode                    ; Need to set AL and CB.VDD_ModeEGA
                                                ; trashed by previous call
	lea	esi,[edi.VDD_CPg]
	mov	[esi.VPH_Mode],al		; Save for VMDOSAPP
IFDEF	PEGA
	mov	edx,[edi.VDD_Cur_CRTC]
ELSE
	lea	edx,[edi.VDD_Stt.CRTC]
ENDIF
	call	VDD_Scrn_Size			; Calculate screen size
	clc
	mov	esi,[VDD_PhysA0000]
	call	VDD_SRAM			; EGA/VGA mode grab
	jc	SHORT VG_NoGrab
	call	VDD_State_Update		; save ctrlr state in copy
        call    VDD_Mem_UpdOff
	call	VDD_RestCtlr			; Restore running VM's state
	jmp	VDD_VMDA_Grab			; Tell VMDA about grab
VG_NoGrab:
	call	VDD_RestCtlr			; Restore running VM's state
VG_Exit:
	ret

VG_GrabNotSup:
        call    VDD_Msg_CannotGrab              ; on grab supported in this mode
        ret


; Grab of non-attached VM, copy memory and video state
VG_WindGrab:
Debug_Out "Windowed GRAB - should not get this - GRAB IGNORED!!"
	ret
EndProc VDD_Grab

;******************************************************************************
;
; VDD_GrabTextMode - called for text mode FS VM grabs only
;
; ENTRY: EAX = video mode
;	 EBX = Vm Handle
;        EDI = CB ptr
;
; EXIT: CF = 1 if grab not successful (out of memory)
;	Setup VDD_CPg, copy diplayed pages into VDD_CPg
; 
; USES: ECX, EDX, ESI, EAX
;
BeginProc VDD_GrabTextMode

	lea	esi,[edi.VDD_Pg]
        call    VDD_Mem_UpdOff
	movzx	eax,[esi.VPH_Mode]
	mov	[edi.VDD_CPg.VPH_Mode],al

	xor	edx,edx
	call	VDD_SetCopyMask 		; ECX = Mask for copy pages
	mov	[edi.VDD_CPg.VPH_PgAccMsk],ecx	
	mov	[edi.VDD_CPg.VPH_Pg2AccMsk],edx
IFDEF	DEBUG
;Trace_Out   "VDD: Full Screen VM, Mode = #AX, Alloc mask = #ECX"
ENDIF
        push    ecx
        push    edx
	call	VDD_Mem_ACopy			; Allocate copy mem
        pop     edx
        pop     ecx
	jc	SHORT VGT_NoGrab		; No memory, exit
	call	VDD_Scrn_Off
        mov     eax,ecx
	call	VDD_CopyTextPages
;
; [esi.VPHMode] may be trashed by previous two calls - restore it.
;
	call	VDD_Get_Mode
	mov	[edi.VDD_CPg.VPH_Mode],al		; Save for VMDOSAPP
	ClrFlag [Vid_Flags], fVid_DOff		; Cancel global display off
	clc
        ret
VGT_NoGrab:
	stc
        ret
EndProc VDD_GrabTextMode

;******************************************************************************
;VDD_CopyTextPages - Used for Full Screen VM grabs,text mode only
;
;DESCRIPTION:
;           Copy displayed pages to copy memory.
;	    Copies from physical text mem to VDD_CPg pages according to mask
;           in EAX
;
;ENTRY: EAX = Pages to copy
;       EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_CopyTextPages

	lea	esi,[edi.VDD_CPg]		; ESI = VPH for copy pages

; Copy page at a time according to page mask in EAX
	push	ebx
	mov	ebx,edi 			; EBX = VDD CB ptr
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
        cld
VCTP_01:
	shr	eax,1
	jnc	SHORT VCTP_03

	push	esi
	push	edi

; ESI = address of text mode memory
	shl	esi,12
	add	esi,[VDD_PhysA0000]
IFDEF	VGAMONO
	add	esi,10000H			; Assume text memory at B0000h
	cmp	[ebx.VDD_ModeEGA],7		; Q: Mono mode?
	je	SHORT VCTP_02			;   Y: memory at B0000h
	add	esi,8000H			;   N: memory at B8000h
VCTP_02:
ELSE
	add	esi,18000H			; Text memory at B8000h
ENDIF
; EDI = address of place to save memory
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_PgMap][edi]
IFDEF DEBUG
	cmp	edi, 0FFh
	jne	short VCTP_D01
	Debug_Out "VDD_CopyTextPages - invalid page index"
VCTP_D01:
ENDIF
	shl	edi,12
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]

	mov	ecx,1000h/4
	rep movsd				; Copy one page of memory
	pop	edi
	pop	esi
VCTP_03:
	inc	esi
	inc	edi
	or	al,al				; Q: More pages to copy?
	jnz	VCTP_01 			;   Y: go do it

	mov	edi,ebx
	pop	ebx
	ret
EndProc VDD_CopyTextPages

;******************************************************************************
;VDD_SR_Setup
;
;DESCRIPTION: Save any controller state not trapped for VM running in EGA,
;	save latches and set up registers for EGA memory access if running
;	in modes with memory addressed at A0000h or no changes if memory
;	accessed at B8000h. This is called by Save and Restore routines
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	none
;
;USES:	none
;
;==============================================================================
BeginProc VDD_SR_Setup

Assert_VDD_ptrs ebx,edi, Pseudo_OK
	cmp	[Vid_VM_HandleRun],0		; Q: Running VM to save?
	jz	VDD_Pre_SR			;   N: Do Save/Restore setup
	push	ebx
	push	edi
	mov	ebx,[Vid_VM_HandleRun]
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	cmp	ebx,[Vid_VM_Handle2]		; Q: 2nd EGA app running?

	jz	SHORT VSS_00			;   Y: already have all regs
	cmp	ebx, [VDD_Msg_Pseudo_VM]	; Q: msg mode VM?
	jz	SHORT VSS_00			;   Y: already have all regs
	call	VDD_SaveRegs			;   N: Read all untrapped regs
	jc	SHORT VSS_01			; If latches saved, only setup
VSS_00:
	call	VDD_Pre_SR			; Do Save/Restore setup
	call	VDD_Save_Latches		; Save latches
	pop	edi
	pop	ebx
	ret
VSS_01:
	pop	edi
	pop	ebx
	jmp	VDD_Pre_SR			; Do Save/Restore setup
EndProc VDD_SR_Setup


;******************************************************************************
;VDD_SRAM
;
;DESCRIPTION:
;	Save physical Video RAM in main memory.
;	In odd/even plane modes, the odd planes will not be allocated since
;	the save/restore algorithm will save/recreate plane 1 from plane 0.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;	ESI = Physical start address to save from
;	CF = 1 indicates copy to VDD_Pg(Physical VM detach)
;	CF = 0 indicates copy to VDD_CPg(Physical VM Grab, mode > 6)
;
;EXIT:	CF = 1 indicates unable to save the RAM(out of memory)
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_SRAM

Assert_VDD_ptrs ebx,edi
	push	eax
	push	ecx
	push	edx
	push	esi
	jnc	VSRAM_Grab
	call	VDD_Mem_Chg			; Q: Pages changed?
	mov	ecx,eax 			; ECX = changed memory mask
	pushfd
	call	VDD_Mem_Test_256k		; EDX = 2nd 128k pages mask
VSRAM_S0:
	popfd
	jnz	SHORT VSRAM_DoSave		;   Y: do save
	test	edx,edx 			; Q: pages changed?
	jz	SHORT VSRAM_Exit		;   N: No save
VSRAM_DoSave:					;   Y: set up for RAM save
	push	ebx
	push	esi
	lea	esi,[edi.VDD_Pg]
	movzx	eax,[edi.VDD_ModeEGA]
	push	edx
	push	ecx
IFDEF	VGAMONO
	cmp	al,0Fh				; Q: Mode F save?
	je	SHORT VSRAM_Pln02		;   Y: Alloc 2 planes
	cmp	al,07h				; Q: Mode 7 save?
	je	SHORT VSRAM_Pln02		;   Y: Alloc 2 planes
ENDIF
	cmp	al,3				; Q: Text mode save?
	jbe	SHORT VSRAM_Pln02		;   Y: Alloc 2 planes

	cmp	al,0Bh  			; Q: Text mode save?
	je	SHORT VSRAM_Pln02		;   Y: Alloc 2 planes

	cmp	al,6				; Q: Mode 4,5 or 6?
	ja	SHORT VSRAM_NotOddEven		;   N: Alloc all accessed
	and	ecx,0000000FFh			;   Y: Alloc only plane 0
VSRAM_Pln02:
	and	ecx,000FF00FFh			; Alloc plane 0 and 2
	xor	edx,edx 			; Don't save 2nd 32k
VSRAM_NotOddEven:
	or	[esi.VPH_PgAccMsk],ecx		; Indicate memory accessed
	or	[esi.VPH_Pg2AccMsk],edx
IFDEF	DEBUG
;	mov	edx,[esi.VPH_PgAllMsk]
;Trace_Out   "VDD:Save RAM, Mode = #AX, Dirty = #ECX, Alloc'd = #EDX"
ENDIF
	movzx	eax,[edi.VDD_ModeEGA]
	call	VDD_Mem_AMain			; Q: Allocate main mem?
	jnc	SHORT VSRAM_SaveMain		;   Y: go save it
	call	VDD_Msg_NoMainMem		;   N: Tell user save incomp.

; [esp] = flags
; [esp+4] = pages to save mask
; [esp+8] = pages to save mask
; [esp+12] = Save physical address
; [esp+16] = VM handle
VSRAM_SaveMain:
	mov	eax,[edi.VDD_Pg.VPH_PgAllMsk]
	not	eax
	and	[edi.VDD_DirtyPages],eax	; Clear mask of mod'd dirty
	mov	eax,[edi.VDD_Pg.VPH_Pg2AllMsk]	;	pages we will save
	not	eax
	and	[edi.VDD_DirtyPages2],eax
	mov	ebx,esi 			; EBX = VPH struc to save into
	pop	ecx				; ECX,EDX = save mask
	pop	edx
	pop	esi				; ESI = start of physical VRAM
VSRAM_Save:
;;;Debug_Out "VDD: Save #ECX,#EDX at #ESI into #EBX"
	call	VDD_Save_RAM
	pop	ebx
VSRAM_Exit:
	clc
VSRAM_Quit:
	pop	esi
	pop	edx
	pop	ecx
	pop	eax
	ret					; Exit, all done saving RAM

; Here on out of memory for allocation of main memory for video save
VSRAM_NoMain:
	SetFlag [edi.VDD_EFlags],fVDE_NoMain	; Missing main memory
	cmp	[edi.VDD_CPg.VPH_PgCnt],0	; Q: Any copy mem allocated?
	jz	SHORT VSRAM_SaveMain		;   N: No recovery possible
	call	VDD_Mem_DCopy			; Delete all of the copy mem
	bts	[edi.VDD_EFlags],fVDE_NoCopyBit ; Missing copy memory
	jc	SHORT VSRAM_NM0 		; User already told
	call	VDD_Msg_NoCopyMem		; Tell user missing copy memory
VSRAM_NM0:
	movzx	eax,[edi.VDD_ModeEGA]
	call	VDD_Mem_AMain			; Q: Allocate main mem success?
	jnc	SHORT VSRAM_SaveMain		;   Y: Go save the memory
	call	VDD_Msg_NoMainMem		;   N: Tell the user and then
	jmp	VSRAM_SaveMain			;	Go save the memory

; Set up RAM for grab
VSRAM_Grab:
	mov	ecx,VDD_Grb_MskTab[eax*4]	
	xor	edx,edx
IFDEF	VGA
	cmp	al, 11h
	jb	short vga_msk_done
	mov	edx,03
	je	short vga_msk_done
	mov	edx, 03030303h
vga_msk_done:
ENDIF
	mov	[edi.VDD_CPg.VPH_PgAccMsk],ecx	; Alloc just enough
	mov	[edi.VDD_CPg.VPH_Pg2AccMsk],edx
IFDEF	DEBUG
;Trace_Out   "VDD:Grab RAM, Mode = #AX, Alloc mask = #ECX"
ENDIF
	push	ebx
	push	esi
	push	edx
	push	ecx
	call	VDD_Mem_ACopy			; Allocate copy mem
	jc	SHORT VSRAM_NoGrab		; No memory, exit
	call	VDD_Scrn_Off			; Turn off the CRT
	ClrFlag [Vid_Flags],fVid_DOff		; Video on when restore ctlr
	movzx	eax,WORD PTR [edi.VDD_Stt.C_AddrH]
	xchg	ah,al				; ECX = display start w/in plane
	pop	ecx				; ECX,EDX = save mask
	pop	edx
	pop	esi				; ESI = start of physical VRAM
	add	esi,eax 			; ESI = display start address
	lea	ebx,[edi.VDD_CPg]		; EBX = where to save it
	cmp	[edi.VDD_ModeEGA],6		; Q: Odd/Even mode?
	jae	VSRAM_Save			;   N: Go do save
	mov	ch,cl				;   Y: Save even plane also
	jmp	VSRAM_Save
; Not enough grab memory, return error
VSRAM_NoGrab:
	add	esp,16
	stc
	jmp	VSRAM_Quit
EndProc VDD_SRAM

;******************************************************************************
;VDD_Save_RAM	    Save video RAM
;
;DESCRIPTION:
;	In odd/even plane modes, the odd planes will not be allocated since
;	the save/restore algorithm will save/recreate plane 1 from plane 0.
;
;ENTRY: EBX = VPH structure pointer
;	EDI = VDD control block data ptr
;	ESI = Video RAM address
;	ECX,EDX = bit map of pages to save
;	DS, ES = valid segment selectors
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Save_RAM
;Trace_Out "Save_Ram #EDI"
	push	ebp
	push	edi
	mov	ebp,ecx
	push	esi				; [esp] = VRAM address
	mov	al,[edi.VDD_ModeEGA]
	mov	[ebx.VPH_Mode],al		; Mem will be this mode
	mov	eax,ebp
	and	ebp,[ebx.VPH_PgAllMsk]		; Only save pages that exist
	and	edx,[ebx.VPH_Pg2AllMsk]

IFDEF	VGAMONO
	cmp	[ebx.VPH_Mode],7		; Q: odd/even mode?
	je	SHORT srer00			;   Y: Save odd plane also
ENDIF
	cmp	[ebx.VPH_Mode],0Bh		; Q: odd/even mode?
	je	SHORT srer00			;   Y: Save odd plane also
	cmp	[ebx.VPH_Mode],6		; Q: odd/even mode?
	jae	SHORT srer0			;   N: continue with mask
srer00:
	xor	edx,edx
	and	eax,0FF00h			;   Y: Save odd plane also
	shr	eax,8
	and	eax,[ebx.VPH_PgAllMsk]		; Save existing plane 0 pgs.
	shl	eax,8
	or	ebp,eax 			;   but set save mask to both

IFDEF TLVGA
; Reading font ram for text modes?
;   Because of a bug in the Tseng Labs chip set, we must go into graphics mode
;   in order to read the FONT ram. 
	cmp	[ebx.VPH_Mode],7		; Q: text mode?
        je      SHORT PrepareFontRead           ;   Y: Prepare for Font RAM read
	cmp	[ebx.VPH_Mode],3		; Q: text mode?
        ja      SHORT NoFontRead                ;   N: No Font RAM to read
PrepareFontRead:
        push    eax
        push    edx
        mov     dx,pGrpIndx
        mov	al,06h
	out	dx,al
	IO_Delay
	inc	dl
        mov     al,05h                  ; enable graphics mode and 64K at A000
	out	dx,al
	IO_Delay
        pop     edx
        pop     eax
NoFontRead:

ENDIF
        
srer0:
						; [esp+4] = VRAM address
	push	edx				; [esp] = extended mask
IFDEF	VGA
	cmp	[ebx.VPH_Mode], 13h
	jne	SHORT srer0a
	mov	edx, pSeqIndx
	mov	al, 4
	out	dx, al
	IO_Delay
	inc	dl
	in	al, dx
	IO_Delay
	or	al, fSeq4Chain4
	out	dx, al
	IO_Delay
srer0a:
ENDIF
	mov	edx,pGrpIndx
	xor	eax,eax 			; Start with plane 0
	cld

srer1:
	;
	; select next plane
	;
	mov	al,4
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,ah
	out	dx,al
	IO_Delay
	dec	dl

	xor	al,al				; Zeroeth page in plane
	mov	esi,[esp+4]			; ESI = VRAM address
srer2:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT srer5			;   N: go do next
	cmp	ah,1				; Q: Plane one?
	jz	srerOddEven			;   Y: Check for odd/even mode
srer3:
	push	eax
	xor	al,al
	shr	eax,5				; EAX = 8*plane number
	add	al, BYTE PTR [esp]		; Add in page within plane
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT srer_dbg0
Debug_Out   "Save to unallocated extended memory page"
srer_dbg0:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	mov	edi,eax
	add	edi,[ebx.VPH_MState.VDA_Mem_Addr] ; EDI = destination
	mov	ecx,1000h			; ECX = count of bytes
	rep movsb				; Move the memory
	sub	esi,1000h
	pop	eax
srer5:
	add	esi,1000h			; ESI = next page address
srer6:
	inc	al
	cmp	al,8				; Q: done with all pages?
	jb	SHORT srer2			;   N: go do next
; Now do extended memory, ESI = start of 32k boundary
	xchg	ebp,[esp]
	or	ebp,ebp 			; Q: any extended mem to save?
	jz	SHORT srer9			;   N: skip
	xor	al,al				; 2nd 32k, same plane
srer7:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT srer8			;   N: go do next
	push	eax
	xor	al,al
	shr	eax,5				; EAX = 8*plane number
	add	al, BYTE PTR [esp]		; add in page within plane
	mov	al,[ebx.VPH_MState.VDA_Mem_Pg2Map][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT srer_dbg1
Debug_Out   "Save to unallocated extended memory page"
srer_dbg1:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	mov	edi,eax
	add	edi,[ebx.VPH_MState.VDA_Mem_Addr] ; EDI = destination
	mov	ecx,1000h			; ECX = count of bytes
	rep movsb				; Move the memory
	sub	esi,1000h
	pop	eax
srer8:
	add	esi,1000h			; EDI = next page address
	inc	al
	cmp	al,8				; Q: done with all pages?
	jnz	SHORT srer7			;   N: go do next
srer9:
	xchg	ebp,[esp]
	inc	ah
	cmp	ah,4				; Q: Last plane?
	jnz	srer1				;   N: next

IFDEF	VGA
	cmp	[ebx.VPH_Mode], 13h
	jne	SHORT srer9a
	mov	edx, pSeqIndx
	mov	al, 4
	out	dx, al
	IO_Delay
	inc	dl
	in	al, dx
	IO_Delay
	and	al, NOT fSeq4Chain4
	out	dx, al
	IO_Delay
srer9a:
ENDIF
	add	esp,4
	pop	esi
	pop	edi
	pop	ebp
	ret
; Merge planes 0 and one(odd/even mode)
srerOddEven:
IFDEF	VGAMONO
	cmp	[ebx.VPH_Mode],7		; Q: odd/even mode?
	je	SHORT srer10				;   Y: save odd bytes in plane 0
ENDIF
	cmp	[ebx.VPH_Mode],0Bh		; Q: odd/even mode?
	je	SHORT srer10				;   Y: save odd bytes in plane 0
	cmp	[ebx.VPH_Mode],6		; Q: odd/even mode?
	jae	srer3				;   N: continue with normal save
srer10: 					;   Y: save odd bytes in plane 0
	push	eax
	xor	ah,ah
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT srer_dbg3
Debug_Out   "Save to unallocated memory page, text plane 1"
srer_dbg3:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	xchg	edi,eax
	add	edi,[ebx.VPH_MState.VDA_Mem_Addr] ; EDI = destination
	mov	ecx,1000h/2
srerOE_Lp:
	inc	edi
	movsb					; move an odd byte
	inc	esi
	loopd	srerOE_Lp
	pop	eax
	jmp	srer6
EndProc VDD_Save_RAM

;******************************************************************************
;VDD_Clear_RAM	  Clear EGA video RAM
;
;DESCRIPTION:
;	Clears the physical VRAM. The application has indicated that it
;	can save/restore its entire state. We clear the RAM for esthetics.
;	Assume that VM displays the first page of memory (A0000-A7FFF) and
;	in some modes overflowing to the second page
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;	ESI = address of physical video RAM
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
;==============================================================================
BeginProc VDD_Clear_RAM, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	edi
	mov	edx,pSeqIndx
	mov	al,2
	out	dx,al
	inc	dl
	IO_Delay
	mov	al,0Fh				; Write to all planes
	out	dx,al
	IO_Delay
	mov	ecx,8000h			; Clear 32k
	mov	edi,esi
; Note that we assume that VM is not displaying 2nd 32k!!
IFDEF	VGA
	call	VDD_Get_Mode
	cmp	al, 10h 			;Q: hi-res VGA graphics mode?
	jb	short do_clear			;   N:
	mov	ecx, 9600h			;   Y: clear for mode 12,13
	cmp	al, 13h 			;Q: 256 color VGA mode?
	jb	short do_clear			;   N:
	add	ecx, 0FA00h			;   Y: clear for mode 13
;	TestMem [Vid_Flags], fVid_450		;Q: 450 line Windows driver?
;	jz	short do_clear			;   Y:
;	mov	ecx, 9000h			;   N: clear another page
do_clear:
ENDIF
	xor	eax,eax
        cld
	rep stosb				; Bytes so DMA can function
	pop	edi
	ret
EndProc VDD_Clear_RAM

;******************************************************************************
;VDD_Restore_RAM    Restore EGA video RAM
;
;DESCRIPTION:
;	Restores physical VRAM from VDD_Pg. Note that we don't care
;	that this sets the access bit since it is at a different PTE than
;	the one associated with the VM
;	In odd/even plane modes, the odd planes will not be allocated since
;	the save/restore algorithm will save/recreate plane 1 from plane 0.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;	ESI = address of physical video RAM
;	EBP, EDX = mask for pages of RAM to restore
;
;EXIT:
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI, EBP
;
;==============================================================================
BeginProc VDD_Restore_RAM, PUBLIC

Assert_VDD_ptrs ebx,edi
;Trace_Out "Restore_Ram #EDI"
	push	ebx				; Save VM handle
	push	edi				; Save VM's VDD CB ptr
	push	esi				; Save VRAM address
	lea	ebx,[edi.VDD_Pg]		; EBX = VPH struc ptr
	mov	eax,ebp
	and	ebp,[ebx.VPH_PgAllMsk]
	and	edx,[ebx.VPH_Pg2AllMsk]
IFDEF	VGAMONO
	cmp	[ebx.VPH_Mode],7		; Q: odd/even mode?
	je	SHORT vrer000			;   Y: Restore odd plane also
ENDIF
	cmp	[ebx.VPH_Mode],0Bh		; Q: Mode 0B?
	je	SHORT vrer000			;   Y: treat like text mode
        
	cmp	[ebx.VPH_Mode],6		; Q: odd/even mode?
	jae	SHORT vrer00			;   N: continue with mask
vrer000:
	xor	edx,edx
	and	eax,0FF00h			;   Y: Restore odd plane also
	shr	eax,8
	and	eax,[ebx.VPH_PgAllMsk]		; Save existing plane 0 pgs.
	shl	eax,8
	or	ebp,eax
vrer00:
	push	edx				; Save extended RAM mask
; [ESP+4] = VRAM start address
; [ESP] = page mask for 2nd 32k pages
	mov	edx,pSeqIndx

	mov	ah,1				; AH = plane mask
IFDEF VGA
	mov	al, [edi.VDD_ModeEGA]
	cmp	al, 11h
	jb	SHORT restore_by_plane
	je	SHORT restore_4_planes
	cmp	al, 13h
	jne	SHORT restore_by_plane

	mov	al, 4
	out	dx, al
	IO_Delay
	inc	dl
	in	al, dx
	or	al, fSeq4Chain4
	IO_Delay
	out	dx, al
	IO_Delay
	dec	dl

restore_4_planes:
	mov	ah, 0Fh 			; if mode 11 or 13 then restore
						; the 1 saved plane into all
						; 4 planes
restore_by_plane:
ENDIF

	cld
vrer0:
	;
	; select next plane
	;
	mov	al,2
	out	dx,al
	inc	dl
	IO_Delay
	mov	al,ah
	out	dx,al
	IO_Delay
	dec	dl
	xor	al,al				; Zeroeth page in plane
	mov	edi,[esp+4]			; EDI = addr of physical mem
IFDEF	VGAMONO
	cmp	[ebx.VPH_Mode],7		; Q: Odd/even restore?
	je	vrerOddEven			;   Y: copy plane 0 to plane 1
ENDIF
	cmp	[ebx.VPH_Mode],0Bh		; Q: Mode 0B?
	je	vrerOddEven		        ;   Y: treat like text mode
	cmp	[ebx.VPH_Mode],6		; Q: Odd/even restore?
	jb	vrerOddEven			;   Y: copy plane 0 to plane 1
vrer1:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT vrer3			;   N: go do next
	xchg	ah,al				; AH=page, AL=plane mask
	push	eax
	bsf	eax,[esp]			; Convert map mask to plane
	shl	eax,3				; EAX = 8*plane number
	add	al, BYTE PTR [esp+1]		; add in page within plane
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT vrer_dbg1
Debug_Out   "Restore from unallocated memory page"
vrer_dbg1:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	mov	esi,eax
	add	esi,[ebx.VPH_MState.VDA_Mem_Addr] ; EDI = destination
	mov	ecx,1000h			; ECX = count of bytes
	rep movsb				; Move the mem(byte reps to EGA!)
	sub	edi,1000h
	pop	eax
	xchg	ah,al
vrer3:
	add	edi,1000h			; EDI = next page address
	inc	al
	cmp	al,8				; Q: done with all pages?
	jb	vrer1				;   N: go do next
vrer4:
	xchg	ebp,[esp]
	or	ebp,ebp 			; Q: Any extended pages?
	jz	SHORT vrer7			;   N: done this plane
; Now do the extended pages
	xor	al,al				; 2nd 32k, same plane
vrer5:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT vrer6			;   N: go do next
	xchg	ah,al				; AH=page, AL=plane mask
	push	eax
	bsf	eax,[esp]			; Convert map mask to plane
	shl	eax,3				; EAX = 8*plane number
	add	al, BYTE PTR [esp+1]		; add in page within plane
	mov	al,[ebx.VPH_MState.VDA_Mem_Pg2Map][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT vrer_dbg2
Debug_Out   "Restore from unallocated extended memory page"
vrer_dbg2:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	mov	esi,eax
	add	esi,[ebx.VPH_MState.VDA_Mem_Addr] ; ESI = destination
	mov	ecx,1000h			; ECX = count of bytes
	rep movsb				; Move the mem(byte reps to EGA!)
	sub	edi,1000h
	pop	eax
	xchg	ah,al				; AH=page, AL=plane mask
vrer6:
	add	edi,1000h			; EDI = next page address
	inc	al
	cmp	al,8				; Q: done with all pages?
	jnz	vrer5				;   N: go do next
vrer7:
	xchg	ebp,[esp]
	shl	ah,1
	cmp	ah,10h				; last plane?
	jb	vrer0				; next
IFDEF VGA
	cmp	[edi.VDD_ModeEGA], 13h
	jne	SHORT vrer8

	mov	edx,pSeqIndx
	mov	al, 4
	out	dx, al
	IO_Delay
	inc	dl
	in	al, dx
	and	al, NOT fSeq4Chain4
	IO_Delay
	out	dx, al
	IO_Delay
vrer8:
ENDIF
	add	esp,4
	pop	esi
	pop	edi
	pop	ebx
	ret

; Map plane 0 to plane 1
vrerOddEven:
	test	ah,010b 			; Q: Plane 1?
	jz	vrer1				;   N: Proceed normally
vrerOE1:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT vrerOE3			;   N: go do next
	push	eax
	xor	ah,ah				; Always page 0 memory
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT vrer_dbg3
Debug_Out   "Restore from unallocated memory page (text page 1)"
vrer_dbg3:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	xchg	esi,eax
	add	esi,[ebx.VPH_MState.VDA_Mem_Addr] ; ESI = source
	inc	esi
	mov	ecx,0FFFh			; ECX = count of bytes
	rep movsb				; Move mem(byte reps to EGA!)
	sub	edi,0FFFh
	pop	eax
vrerOE3:
	add	edi,1000h			; EDI = next page address
	inc	al
	cmp	al,8				; Q: done with all pages?
	jnz	SHORT vrerOE1			;   N: go do next
	jmp	vrer4				; Proceed
EndProc VDD_Restore_RAM


;******************************************************************************
;VDD_MsgRestFontAndClear
;
;DESCRIPTION:
;	This is used in message mode to restore the font and clear the memory
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_MsgRestFontAndClear

	push	edi
	mov	edx,pSeqIndx
	cld
	mov	al,2
	out	dx,al
	inc	dl
	IO_Delay
	mov	al,4
	out	dx,al
	mov	esi,OFFSET32 VDD_MsgFont
	mov	edi,[VDD_PhysA0000]
	mov	ecx,FontLen/4
	rep	movsd

; Initialize memory to blanks with backgnd of [VDD_Scrn_Attr]
	mov	al,1
	out	dx,al
	mov	edi,[VDD_PhysA0000]
	mov	al, ' '
	mov	ecx,25*80*2
	rep stosb

	mov	al,10b
	out	dx,al
	mov	edi,[VDD_PhysA0000]
	mov	al, [VDD_Scrn_Attr]
	mov	ecx,25*80*2
	rep stosb

	pop	edi
	ret
EndProc VDD_MsgRestFontAndClear


;******************************************************************************
;VDD_MsgSaveFont   Save EGA font RAM
;
;DESCRIPTION:
;	This is used to save the font for message mode
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_MsgSaveFont

	push	edi
	mov	edx,pGrpIndx

IFDEF TLVGA
;   Because of a bug in the Tseng Labs chip set, we must go into graphics mode
;   in order to read the FONT ram. 
        mov     al,6
        out     dx,al
	IO_Delay
	inc	dl
        mov     al,05h                  ; enable graphics mode and 64K at A000
        out     dx,al
	IO_Delay
        dec     dl
ENDIF
	cld
	mov	al,4
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,2
	out	dx,al
	mov	edi,OFFSET32 VDD_MsgFont
	mov	esi,[VDD_PhysA0000]		; EDI = start of VRAM
	mov	ecx,FontLen/4
	rep	movsd
	pop	edi
	ret
EndProc VDD_MsgSaveFont


;******************************************************************************
;VDD_RestCRTC Restore Display (CRTC) state
;
;DESCRIPTION:
;	Restore physical CRTC controller.  For paradise, there are four CRTC
;	states to restore based on all combinations of scrambled and
;	locked states.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
;==============================================================================
BeginProc VDD_RestCRTC

Assert_VDD_ptrs ebx,edi,Pseudo_OK
	Queue_Out "VDD_RestCRTC #ebx"
	push	ebp
	push	esi
	lea	esi, [edi.VDD_Stt]

IFDEF	PEGA

	TestMem [esi.V_Flag_PEGA],fVPa_PReg
	jz	SHORT VDD_NoPReg
; Paradise register
	mov	edx,pPStt
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pPReg AND 0FFh)
	mov	al,[esi.VC_PReg]
	out	dx,al				; Program Paradise reg
	IO_Delay
VDD_NoPReg:
	TestMem [esi.V_Flag_PEGA],fVPa_PMisc
	jz	SHORT VDD_RE_0
; Special paradise Misc register
	mov	edx,pPStt
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pMiscEGA AND 0FFh)
	mov	al,[esi.VC_PMisc]
	out	dx,al				; Program Paradise misc output
	IO_Delay
VDD_RE_0:
ENDIF

IFDEF	VGA8514
	VMMCall Test_Sys_VM_Handle
	jne	short TurnOnVGA

TurnOn8514:
    ;
    ;Now, set the adapter into Hi-Res mode, setting some states as we go along:
    ;
	mov	ax,7				;set misc I/O back to VGA
	mov	dx,4ae8h
	out	dx,ax
	call	VDD_Rest_DAC
        pop     esi
	pop	ebp
        ret
        
TurnOnVGA:
    ;
    ;We return to VGA mode and leave gracefully:
    ;
	mov	ax,6				;set misc I/O back to VGA
	mov	dx,4ae8h			;
	out	dx,ax				;
ENDIF	;VGA8514

IFDEF	VGA
;*******
; Enable access
	mov	edx,pVGAEna
IFDEF	TLVGA
; Tseng labs has video subsystem enable on two diff ports depending
;	on value in feature control.  Also has another bit to worry about.

	mov	ah,fVGAEna
	TestMem [Vid_Flags],fVid_TLVGA
	jz	SHORT VR_NotTLEnaRd
	mov	edx,pFeatRead
	in	al,dx
	and	al,3
	cmp	al,3
	mov	edx,pVGAEna
	jnz	SHORT VR_NotTLEnaRd
	or	ah,8
	mov	edx,46E8h			; Other VGA Enable port
VR_NotTLEnaRd:
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,ah				; Make sure enable is true
ELSE
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,fVGAEna			; Make sure enable is true
ENDIF
	out	dx,al
	IO_Delay

ENDIF

; Sequencer and Miscellaneous output first
	mov	edx,pSeqIndx
	cld
	xor	eax,eax
	out	dx,al
	IO_Delay
	mov	al,01				; Reset sequencer
	inc	dl
	out	dx,al
	IO_Delay
	inc	ah
	dec	dl
	push	esi
	lea	esi,[esi.S_ClMode]
	mov	ecx,S_MMode-S_ClMode+1
	mov	ebp,5				; Program ClMode and ChMap
	call	VDD_OutC_Ctrlr			;   skip Mask and MMode
	pop	esi
	mov	dl,(pMiscEGA AND 0FFh)
	mov	al,[esi.V_Misc]
	out	dx,al				; Program misc output
	IO_Delay
	xor	eax,eax
	mov	dl,(pSeqIndx AND 0FFh)
	out	dx,al
	IO_Delay
	mov	al,[esi.S_Rst]
	inc	dl
	out	dx,al				; Enable sequencer
	IO_Delay


; CRT controller
	Set3DxPorts				; Set 3Dx addressing
IFDEF	PEGA
	TestMem [esi.V_Flag_PEGA],fVPa_PNew
	jz	SHORT VDD_NotPNew
; Paradise New register(affects some CRTC timing and locking)
	mov	eax,xC_PNew
	mov	dl,(pIndx6845Colr and 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,[esi.VC_PNew]
	out	dx,al
	IO_Delay
VDD_NotPNew:
	or	ebp,-1				; All regs unscr, unlokd
	TestMem [esi.V_Flag_PEGA],<fVPa_PScrm+fVPa_PCLok> ; Q: special CRTC?
	jz	SHORT VDD_NoUnU 		;   N: skip
	PEGACUnlock				; Unlock and
	PEGACUnscram				;   and unscramble CRTC
	mov	ebp,DWORD PTR [esi.VC_PCUnUMsk] ; output regs written only
VDD_NoUnU:
ELSE
	or	ebp,-1				; All regs unscr, unlocked
ENDIF

;Need to call this function here if on a V7, so the extended clock
;select bits are set, ie we get the right clock!
IFDEF   V7VGA
        call    V7_RestCRTC
ENDIF
IFDEF	ATIVGA
EXTRN	ATi_RestCRTC:NEAR
	call	ATi_RestCRTC
ENDIF

	cld
	push	esi
.ERRE	(C_HTotal EQ 0) AND (CRTC EQ 0)

        mov	ecx,C_LnCmp-C_HTotal+1
	mov	dl,(pIndx6845Colr AND 0FFh)

IFDEF	VGA
; enable access CRTC
	mov	al, 11h
	out	dx, al
	IO_Delay
	inc	edx
	in	al, dx
	IO_Delay
	and	al, 7Fh
	out	dx, al
	IO_Delay
	dec	edx
IFDEF CTVGA
	mov	al,17h
	out	dx,al
	inc	edx
	in	al,dx
	or	al,10h
	out	dx,al				; Enable extended read mode
	dec	edx
	mov	al,xC_CT400
	out	dx,al
	inc	edx
	in	al,dx
	or	al,80h				; Read CRTC cursor posn regs
	out	dx,al
	dec	edx
ENDIF
ENDIF	; VGA

	xor	eax,eax
	call	VDD_OutC_Ctrlr			; Program CRTC cond'l on EBP
	pop	esi

IFDEF	PEGA
	TestMem [esi.V_Flag_PEGA],<fVPa_PScrm+fVPa_PCLok> ; Q: special CRTC?
	jz	VDD_Attr			;   N: go do Attr ctrlr now
	cmp	DWORD PTR [esi.VC_PCUnLMsk],0	; Q: Any unscrmbld, locked?
	jz	SHORT VDD_NtUnL 		;   N: skip
	PEGACUnscram				; Unscramble
	PEGACLock				; Lock
	cld
	mov	ebp,DWORD PTR [esi.VC_PCUnLMsk]
	and	ebp,0FFFF33FFh			; Don't reprogram cursor
	push	esi
	lea	esi,[esi.VC_PCUnL]
	mov	ecx,C_LnCmp-C_HTotal+1
	mov	dl,(pIndx6845Colr AND 0FFh)
	xor	eax,eax
	call	VDD_OutC_Ctrlr			; Program CRTC cond'l on EBP
	pop	esi
VDD_NtUnL:
	cmp	DWORD PTR [esi.VC_PCScUMsk],0	; Q: Any scrambled, unlkd?
	jz	SHORT VDD_NtScU 		;   N: skip
	PEGACScram				; Turn on scrambler
	PEGACUnlock				; Unlock
	cld
	mov	ebp,DWORD PTR [esi.VC_PCScUMsk]
	and	ebp,0FFFF33FFh			; Don't reprogram cursor
	push	esi
	lea	esi,[esi.VC_PCScU]
	mov	ecx,C_LnCmp-C_HTotal+1
	mov	dl,(pIndx6845Colr AND 0FFh)
	xor	eax,eax
	call	VDD_OutC_Ctrlr			; Program CRTC cond'l on EBP
	pop	esi
VDD_NtScU:
	cmp	DWORD PTR [esi.VC_PCScLMsk],0	; Q: Any scrambled, lokd?
	jz	SHORT VDD_NtScL 		;   N: skip
	PEGACScram				; Turn on scrambler
	PEGACLock				; Lock
	cld
	mov	ebp,DWORD PTR [esi.VC_PCScLMsk]
	and	ebp,0FFFF33FFh			; Don't reprogram cursor
	push	esi
	lea	esi,[esi.VC_PCScL]
	mov	ecx,C_LnCmp-C_HTotal+1
	mov	dl,(pIndx6845Colr AND 0FFh)
	xor	eax,eax
	call	VDD_OutC_Ctrlr			; Program CRTC cond'l on EBP
	pop	esi
VDD_NtScL:

; Restore the scrambler, if written, to current value
VDD_NotPCLocked:
	TestMem [esi.V_Flag_PEGA],fVPa_PScrm	; Q: Scrambler written?
	jz	SHORT VDD_TLock 		;   N: skip
	PEGACUnscram
	TestMem [esi.V_Flag_PEGA],fVPa_PCScrmd	; Q: Currently enabled?
	jz	SHORT VDD_TLock 		;   N: leave it disabled
	PEGACScram
VDD_TLock:
; Restore the CRTC lock, if written, to current value
	TestMem [esi.V_Flag_PEGA],fVPa_PCLok	; Q: Lock reg written?
	jz	SHORT VDD_Attr			;   N: skip
	PEGACUnlock				; Unlock
	TestMem [esi.V_Flag_PEGA],fVPa_PCLokd	; Q: Currently locked?
	jz	SHORT VDD_Attr			;   N: leave it unlocked
	PEGACLock
VDD_Attr:
ENDIF

IFDEF	VGA
;*******
; Attribute controller regs 10,12,13
	mov	edx,pAttrEGA
	mov	al,A_Mode-A_Pal
	mov	ah,[esi.A_Mode]
	call	Attr_Out
	mov	al,A_PlEna-A_Pal
	mov	ah,[esi.A_PlEna]
	call	Attr_Out
	mov	al,A_HPan-A_Pal
	mov	ah,[esi.A_HPan]
	call	Attr_Out

;*******
; Attribute controller regs 14,0-F, 11
IFNDEF CTVGA
	mov	al,A_Color-A_Pal
	mov	ah,[esi.A_Color]
	call	Attr_Out
ENDIF

	mov	dl,(pSeqIndx AND 0FFh)
	mov	al,1
	out	dx,al
	mov	al,[esi.S_ClMode]
	inc	edx
	or	al,20h				; Turn screen off
	out	dx,al

	mov	dl,(pAttrEGA AND 0FFh)
	push	esi
	lea	esi,[esi.A_Pal]
	mov	ecx,A_Mode-A_Pal
	xor	ah,ah
        cld
VVRStt_0:
	lodsb					; Get a value
	xchg	ah,al
	call	Attr_Out			; Program it
	xchg	ah,al
	inc	ah				; next register index
	loopd	VVRStt_0			; Program next attribute
	pop	esi

	mov	al,A_OvScn-A_Pal
	mov	ah,[esi.A_OvScn]
	call	Attr_Out

	call	VDD_Rest_DAC
	mov	dl,(pSeqIndx AND 0FFh)

	mov	al,1
	out	dx,al
	mov	al,[esi.S_ClMode]
	inc	edx
	out	dx,al				; Restore clocking mode

ELSE
; Attribute controller
IFDEF	PEGA
	TestMem [esi.V_Flag_PEGA],fVPa_PALok	; Q: attr lock written?
	jz	SHORT VDD_AnotLokd		;   N: leave alone
	PEGAAUnlock
VDD_AnotLokd:
ENDIF
	push	esi
	lea	esi,[esi.A_Pal]
	mov	ecx,A_HPan-A_Pal+1
	mov	dl,(pStatColr AND 0FFh)
	in	al,dx				; Reset Attr indx/reg toggle
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	xor	eax,eax
        cld
VDD_RE_Attr:					; Program attribute ctrlr
	mov	al,ah
	out	dx,al
	IO_Delay
	lodsb
	out	dx,al
	IO_Delay
	inc	ah
	loop	VDD_RE_Attr
	pop	esi
IFDEF	PEGA
	TestMem [esi.V_Flag_PEGA],fVPa_PALok	; Q: attr lock written?
	jz	SHORT VDD_ERSex 		;   N: leave alone
	TestMem [esi.V_Flag_PEGA],fVPa_PALokd	; Q: Attr locked?
	jz	SHORT VDD_ERSEx 		;   N: leave unlocked
	PEGAALock				;   Y: lock it
VDD_ERSEx:
	TestMem [esi.V_Flag_PEGA],fVPa_PNew	; Q: New PEGA REG written?
	jz	SHORT VDD_NtPNew		;   N: leave alone
	mov	eax,xC_PNew
	mov	dl,(pIndx6845Colr and 0FFh)
	out	dx,al
	IO_Delay
	mov	dl,(pPStt AND 0FFh)
	in	al,dx
	IO_Delay
	in	al,dx
	IO_Delay
	mov	dl,(pData6845Colr and 0FFh)
	mov	al,[esi.VC_PNew]
	out	dx,al				;   will get accepted
	IO_Delay
VDD_NtPNew:
ENDIF

ENDIF	;VGA

IFDEF	ATIEGA
	TestMem [Vid_Flags],fVid_ATI800
	jz	SHORT Rest_ATI_Not
	mov	edx,pATI_Indx
	xor	eax,eax
	mov	al,[edi.VDD_Stt.ATI_Regs]	 ; Reg 0
	call	Out_ATI_800
	mov	ah,1
	mov	al,[edi.VDD_Stt.ATI_Regs.1]	 ; Reg 1
	and	al,0F7h
	call	Out_ATI_800
	mov	ah,2
	mov	al,[edi.VDD_Stt.ATI_Regs.2]	 ; Reg 2
	and	al,01Fh
	call	Out_ATI_800
	mov	ah,3
	mov	al,[edi.VDD_Stt.ATI_Regs.3]	 ; Reg 3
	call	Out_ATI_800
	mov	eax,0Bh 			; Reg 0Bh - 1Eh
	push	esi
	lea	esi,[edi.VDD_Stt.ATI_Regs][eax]
ATI_Rest_Loop:
	mov	ecx,eax
	xchg	ah,al
	lodsb
	call	Out_ATI_800
	mov	eax,ecx
	inc	eax
	cmp	al,01fh
	jb	ATI_Rest_Loop
	pop	esi
Rest_ATI_Not:
ENDIF

	Rest3?xPorts [esi.V_Misc]		; Restore 3Dx/3Bx addressing

	pop	esi
	pop	ebp
	ret

IFDEF	ATIEGA
; Output to ATI 800 register, EDX = index port, AH = index, AL = value
;   USES: EAX, FLAGS
OUT_ATI_800:
	xchg	ah,al
	out	dx,al				; Output index
	IO_Delay
	inc	edx
	inc	edx
	in	al,dx				; Enable access
	IO_Delay
	xchg	ah,al
	out	dx,al				; Output data
	dec	edx
	dec	edx
	ret
ENDIF

;Program palette only when CRTC not accessing it(during VRTC)
Attr_Out:
	push	eax
	push	ecx
	push	edx
	xor	ecx,ecx
	mov	edx,pStatColr
Attr_O0:
	in	al,dx				; Reset address toggle
	IO_Delay
	test	al,8
	loopdz	SHORT Attr_O0			; Only write during VRTC
	pop	edx
	pop	ecx
	pop	eax
	out	dx,al
	IO_Delay
	xchg	al,ah
	out	dx,al
	IO_Delay
	xchg	al,ah
	ret


EndProc VDD_RestCRTC


;******************************************************************************
;VDD_OutC_Ctrlr    Output values to a set of video controller regisers
;
;DESCRIPTION:
;	Outputs to video registers in sequence conditionally on bits in EBP
;
;ENTRY: AH  = controller index
;	ECX = number of registers
;	EDX = controller port
;	EBP = controller port index bit mask
;	ESI = controller registers values ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, ESI
;
;==============================================================================
BeginProc VDD_OutC_Ctrlr, PUBLIC

        cld
VDD_PSc0:
	lodsb
	shr	ebp,1
	jnc	SHORT VDD_PSc1
	xchg	ah,al
	out	dx,al				; Set index register
	IO_Delay
	xchg	ah,al
	inc	dl
	out	dx,al				; Set data register
	dec	dl
VDD_PSc1:
	inc	ah
	loopd	VDD_PSc0
	ret
EndProc VDD_OutC_Ctrlr

;******************************************************************************
;VDD_Scrn_Off		    Turn off video display for EGA
;
;DESCRIPTION:
;	turns off CRT
;
;ENTRY: EBX = VM_Handle
;
;EXIT:	none
;
;USES:	Flags, EAX
;
;==============================================================================
BeginProc VDD_Scrn_Off, PUBLIC
IFDEF	VGA
	push	ebx
	push	edx
	push	edi
IFDEF	VGAMONO
	mov	edx,pMiscVGAIn
	in	al,dx
	mov	dl,(pStatColr AND 0FFh) 	; Assume color port addr(3Dx)
	test	al,1
	jnz	SHORT VSO_0
	mov	dl,(pStatMono AND 0FFh)
VSO_0:
ELSE
	mov	edx,pStatColr			; Assume color port addr(3Dx)
ENDIF
	in	al,dx				; Set attribute reg to index
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	cmp	[Vid_VM_HandleRun],0
	jz	SHORT VSO_01
	mov	ebx,[Vid_VM_HandleRun]
IFDEF DEBUG
	cmp	ebx, [VDD_Msg_Pseudo_VM]
	je	SHORT VSO_D00
Assert_VM_Handle ebx
VSO_D00:
ENDIF
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	bts	[Vid_Flags],fVid_DOffBit	; Q: Globally turned off?
	jc	SHORT VSO_01			;   Y: don't read the index
	in	al,dx				; Get current index
	bts	[edi.VDD_Flags],fVDD_AXRdBit	; Video saved?
	jc	SHORT VSO_01			; Already saved, don't save
	and	[edi.VDD_Stt.A_Indx],fVAI_Indx
	and	al,NOT fVAI_Indx
	or	[edi.VDD_Stt.A_Indx],al 	; Save it, preserving fVAI_Indx
VSO_01:
	xor	al,al
	out	dx,al				; Turn off the video
	IO_Delay
	pop	edi
	pop	edx
	pop	ebx
	ret
ELSE

	push	edx
	SetFlag [Vid_Flags],fVid_DOff	  ; Video off
	mov	edx,pStatColr			; Assume color port addr(3Dx)
	in	al,dx				; Set attribute write to index
	IO_Delay
	mov	dl,(pAttrEGA AND 0FFh)
	xor	al,al
	out	dx,al
	IO_Delay
	pop	edx
	ret
ENDIF
EndProc VDD_Scrn_Off


;******************************************************************************
;VDD_RestIndx	       Restore video index registers
;
;DESCRIPTION:
;	Restores video index registers, optionally leaving video off.  Leaves
;	video off when CF=1 and when CF=0 and restoring SYS VM index registers.
;
;ENTRY: EBX = VM_Handle, EDI = VM's VDD CB ptr
;	CF=1	indicates leave video off, CF = 0 indicates turn video on
;
;EXIT:
;
;USES:	Flags, EAX
;
;==============================================================================
BeginProc VDD_RestIndx, PUBLIC

Assert_VDD_ptrs ebx,edi, Pseudo_ok
;Trace_Out "RestIndx #EDI"
	push	edx
	pushfd
IFDEF VGA
	ClrFlag [edi.VDD_Flags], fVDD_AXRd	; indicate index reg restore
ENDIF
IFDEF	VGAMONO
	mov	edx,pMiscVGAIn
	in	al,dx
	mov	edx,pIndx6845Colr
	test	al,1
	jnz	SHORT VRI_0
	mov	dl,(pIndx6845Mono AND 0FFh)
VRI_0:
ELSE
	mov	edx,pIndx6845Colr
ENDIF

	mov	al,[edi.VDD_Stt.C_Indx]
	out	dx,al				; Update CRTC index
	IO_Delay
	mov	dl,(pGrpIndx AND 0FFh)
	mov	al,[edi.VDD_Stt.G_Indx]
	out	dx,al				; Update Graphics ctrlrs index
	IO_Delay

	mov	dl,(pStatColr AND 0FFh) 	; Assume color port addr(3Dx)
IFDEF	VGAMONO
; ZF = value tested for above (color or B/W)
	jnz	SHORT VRI_1
	mov	dl,(pStatMono AND 0FFh) 	; Assume color port addr(3Dx)
VRI_1:
ENDIF
	in	al,dx				; Reset Attr index/reg toggle
	IO_Delay
	mov	al,[edi.VDD_Stt.A_Indx]
	and	al,mAIndx			; assume leave video off
	popfd					; Q: Leave video off?
	push	edx
	mov	dl,(pAttrEGA AND 0FFh)
	jc	SHORT Dspy_Off			;   Y: leave video off
	TestMem [Vid_Flags],fVid_DOff		; Q: Globally turned off?
	jnz	SHORT Dspy_Off			;   Y: leave video off
	or	al,fVAI_ScOn			;   Y: leave video on
Dspy_Off:
	out	dx,al
	IO_Delay
	pop	edx
	test	[edi.VDD_Stt.A_Indx],fVAI_Indx	; Q: ctrlr expecting data?
	jz	SHORT Dspy_On_1 		;   Y: Leave ctrlr rdy for data
	in	al,dx				;   N: leave ctrlr rdy for index
	IO_Delay
Dspy_On_1:
	mov	dl,(pSeqIndx AND 0FFh)
	mov	al,[edi.VDD_Stt.S_Indx]
	out	dx,al				; Update Sequencer index reg.
	IO_Delay

IFDEF	VGA
;*******
; Restore VGA enable bit
	mov	edx,pVGAEna
IFDEF	TLVGA
; Tseng labs has video subsystem enable on two diff ports depending
;	on value in feature control.  Also has another bit to worry about.

	TestMem [Vid_Flags],fVid_TLVGA
	jz	SHORT VRI_NotTLEnaRd
	mov	al, [edi.VDD_Stt.V_Feat]
	and	al,3
	cmp	al,3
	mov	edx,pVGAEna
	jnz	SHORT VRI_NotTLEnaRd
	mov	edx,46E8h			; Other VGA Enable port
VRI_NotTLEnaRd:
ENDIF
	mov	al,[edi.VDD_STT.V_VGAEna]
	out	dx,al
ENDIF

	pop	edx
	ret

EndProc VDD_RestIndx


;******************************************************************************
;VDD_Global_Restore
;
;DESCRIPTION:
;	This routine will set up VM events to restore the VM state. It is
;	called when VDD modifies the video registers.  Note that when VDD
;	modifies the video registers, the Vid_VM_HandleRun is cleared.	It
;	will disable the 2nd EGA VM's memory, so that if the VM accesses the
;	memory, then VDD_PFault can make sure that the memory access registers
;	are reprogrammed correctly.
;
;ENTRY: none
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Global_Restore,PUBLIC
;Trace_Out "Global Restore"
	pushad

	cmp	[VDD_Msg_VM], 0 		;Q: message mode?
	jne	SHORT VGR_01			;   Y: No restore necessary

	mov	ebx,[VDD_Focus_VM]
	cmp	ebx,[Vid_VM_HandleRun]		; Q: Focus VM in registers?
	jz	SHORT VGR_01			;   Y: No restore necessary
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Restore			;   N: Set up event to restore
VGR_01:
	mov	ebx,[Vid_VM_Handle2]
	or	ebx, ebx			; Q: 2nd EGA running?
	jz	SHORT VGR_02			;   N: All done
						;   Y: restore regs later
; We need to restore 2nd EGA VM to registers before any memory access is done

	cmp	ebx,[Vid_VM_HandleRun]		; Q: 2nd EGA VM in registers?
	jz	SHORT VGR_02			;   Y: All done
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Mem_Chg			; Remember pages modified
	call	VDD_Mem_Disable 		; Force trap w/mem access
VGR_02:
	popad
	ret
EndProc VDD_Global_Restore

;******************************************************************************
;VDD_Pre_SR		      Set up EGA for video RAM Save/Restore
;
;DESCRIPTION:
;	Programs registers that affect CPU addressing, ready for saving or
;	restoring the video memory.
;
;ENTRY: [Vid_VM_Handle] = VM with CRT
;	EDI = VDD CB ptr for VM we are saving or restoring
;
;EXIT:	none
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_Pre_SR
IFDEF	PEGA
%OUT what if VM has PEGA scratch RAM mapped?
ENDIF

IFDEF DEBUG
	push	ebx
	mov	ebx, edi
	sub	ebx, [VDD_CB_Off]
Assert_VDD_ptrs ebx,edi,Pseudo_OK
	pop	ebx
ENDIF

	push	ebx
	mov	ebx,edi
	cmp	[Vid_VM_Handle],0		; Q: Attached VM exist?
	jz	SHORT VPSR_00			;   N: Use running VM bits
	mov	ebx,[Vid_VM_Handle]
	add	ebx,[VDD_CB_Off]		;   Y: EBX = VDD ptr attached VM
VPSR_00:
;Trace_Out "Pre_SR #EBX"

	btr	[Vid_Flags], fVid_MsgRBit	; out of msg mode
	jc	SHORT VPSR_restore
	xor	eax, eax
	xchg	eax, [Vid_VM_HandleRun] 	; No VM owns regs now
	or	eax, eax
	jz	SHORT VPSR_01
VPSR_restore:
	call	VDD_Global_Restore
VPSR_01:
	lea	ebx,[ebx.VDD_Stt]

IFDEF	VGA
;*******
; Enable access to VGA
	mov	edx,pVGAEna
IFDEF	TLVGA
; Tseng labs has video subsystem enable on two diff ports depending
;	on value in feature control.  Also has another bit to worry about.

	mov	ah,fVGAEna
	TestMem [Vid_Flags],fVid_TLVGA
	jz	SHORT VPS_NotTLEnaRd
	mov	edx,pFeatRead
	in	al,dx
	and	al,3
	cmp	al,3
	mov	edx,pVGAEna
	jnz	SHORT VPS_NotTLEnaRd
	or	ah,8
	mov	edx,46E8h			; Other VGA Enable port
VPS_NotTLEnaRd:
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,ah				; Make sure enable is true
ELSE
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,fVGAEna			; Make sure enable is true
ENDIF
	out	dx,al
	IO_Delay

ENDIF

; Set up Sequencer for sequential addressing at A000 for 64k
	mov	edx,pSeqIndx
	mov	al,04h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[ebx.S_MMode]
	and	al,1
	or	al,06h
	out	dx,al
	IO_Delay

; set enable Set/Reset to disable(write CPU data)
	mov	edx,pGrpIndx
	mov	al,01h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,00h
	out	dx,al
	IO_Delay
	dec	dl

; set raster OP to write data unmodified
	mov	al,03h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,00h
	out	dx,al
	IO_Delay
	dec	dl

; set read mode 0, write mode 0
	mov	al,05h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[ebx.G_Mode]
IFDEF VGA
	and	al,60h				; Preserve CRTC Shift bit
						;   and Shift 256 bit
ELSE
	and	al,20h				; Preserve CRTC Shift bit
ENDIF
	out	dx,al
	IO_Delay
	dec	dl

; set map to start at A0000H
	mov	al,06h
	out	dx,al
	IO_Delay
	inc	dl

	mov	al,[ebx.G_Misc]
	and	al,1				; Preserve grap/char mode bit
	or	al,04h
;	mov	al,05h                          ; for bug in Tseng Lab VGA 
	out	dx,al
	IO_Delay
	dec	dl

; set bit mask to write all CPU data bits
	mov	al,08h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,0FFh
	out	dx,al
	IO_Delay
	dec	dl

IFDEF	PVGA
	TestMem [Vid_Flags],fVid_PVGA		; Q: Unlocked ever?
	jz	SHORT VPSR_PVGA9
	mov	eax,G_PVGA5-G_SetRst		;   Y: restore paradise regs
	out	dx,al
	IO_Delay
	inc	edx
	mov	al, 5				; Unlock access
	out	dx,al
	IO_Delay
	dec	edx
VPSR_PVGA1:

	mov	al,G_PVGA0A-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	xor	al,al				; Zero address offset A
	out	dx,al
	IO_Delay
	dec	edx
VPSR_PVGA2:

	mov	al,G_PVGA0B-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	xor	al,al				; Zero address offset B
	out	dx,al
	IO_Delay
	dec	edx
VPSR_PVGA3:

	mov	al,G_PVGA1-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[ebx.G_PVGA1]		; Clear weird mem access
	and	al,07h				; Leave hardware config alone
	out	dx,al
	IO_Delay
	dec	edx
VPSR_PVGA4:

	mov	al,G_PVGA5-G_SetRst		; Restore access lock/unlock
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[ebx.G_PVGA5]
	out	dx,al
	IO_Delay
	dec	edx

VPSR_PVGA9:
ENDIF	;PVGA

IFDEF	V7VGA
EXTRN	V7_ExtRegs_PreSR:NEAR
	call	V7_ExtRegs_PreSR
ENDIF
IFDEF	ATIVGA
EXTRN	ATi_ExtRegs_PreSR:NEAR
	call	ATi_ExtRegs_PreSR
ENDIF
	pop	ebx

	ret
EndProc VDD_Pre_SR

;******************************************************************************
;VDD_SaveMsgStt
;
;DESCRIPTION:
;	Updates VDD_MsgStt with current VM state and VDD_MsgFont with FONT RAM.
;
;ENTRY: EBX = Handle of VM being saved/restored
;	EDI = VDD ptr of VM being saved/restored
;
;EXIT:	none
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_SaveMsgStt, PUBLIC
	pushad
	call	VDD_Scrn_Off			; Turn off the CRT
	call	VDD_SR_Setup			; Set up calls VDD_SaveRegs

	push	edi
IFDEF VGA
	lea	esi,[edi.VDD_DAC]
	lea	edi, [VDD_Msg_Pseudo_CB.VDD_DAC]
	mov	ecx, (VDD_DAC_Pad2+2-VDD_DAC) / 4
.ERRNZ (VDD_DAC_Pad2+2-VDD_DAC) MOD 4
        cld
	rep movsd				; Save DAC regs
	mov	edi, [esp]			; restore edi value
ENDIF
	lea	esi,[edi.VDD_Stt]
	lea	edi, [VDD_Msg_Pseudo_CB.VDD_Stt]
	mov	ecx, (SIZE EGA_State_struc) / 4
.ERRNZ (SIZE EGA_State_struc) MOD 4
	rep movsd				; Save adapter regs
	pop	edi
	call	VDD_MsgSaveFont 		; Save the font

	ClrFlag [Vid_Flags], fVid_DOff		; Cancel global display off
	call	VDD_RestCtlr			; Restore mem access, indices
	popad
	ret
EndProc VDD_SaveMsgStt

;******************************************************************************
;VDD_RestMsgStt
;
;DESCRIPTION:
;	Restores VDD_MsgStt to video adapter
;
;ENTRY: EBX = Handle of VM being switched to message mode
;	EDI = VDD ptr of VM being switched to message mode
;
;EXIT:	none
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_RestMsgStt, PUBLIC

	SetFlag [Vid_Flags],fVid_MsgA		; Set message mode Init
	call	VDD_Scrn_Off			; Turn off the CRT
	call	VDD_SR_Setup			; Set up for restore
	call	VDD_RestCRTC			; Set display for msg mode
	call	VDD_MsgRestFontAndClear 	; Restore the font and clear mem
	call	VDD_Global_Restore		; If necessary, set 2nd EGA
	ClrFlag [Vid_Flags], fVid_DOff		; Cancel global display off
	call	VDD_RestCtlr			; Restore mem access, indices

; When in message state, use pseudo VM handle

	mov	eax, [VDD_Msg_Pseudo_VM]
	mov	[Vid_VM_Handle], eax

	ret
EndProc VDD_RestMsgStt


;******************************************************************************
;VDD_Save_Latches	      Set up EGA for video RAM save
;
;DESCRIPTION:
;	Writes latches at VDD_Latch_Addr then does reads them from each
;	of the four planes and saves the values. Assumes that
;	VDD_Pre_SR has been called to set up registers.
;
;ENTRY: [Vid_VM_Handle] = VM with CRT
;	EDI = VDD ptr of VM to save latches to (most recently running VM)
;
;EXIT:
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_Save_Latches

	test	[edi.VDD_Stt.G_Misc],8		; Q: Graphics mode, not CGA?
	jz	SHORT VSL_01			;   Y: Save latches
VSL_00:
	ret					;   N: Skip save latches
VSL_01:
	push	ebx
	push	ecx
	push	esi
	push	edi
	mov	ebx,[Vid_VM_Handle]
	add	ebx,[VDD_CB_Off]		; EBX = VDD ptr attached VM
	lea	ebx,[ebx.VDD_Stt]
	cmp	[Vid_VM_Handle],0		; Q: Attached VM exist?
	jnz	SHORT VSL_02			;   Y: attached or msg VM bits
	lea	ebx,[edi.VDD_Stt]		;   N: Use running VM bits
VSL_02:
;;;	jmp	SHORT VSL_032
;;;	VMMCall Begin_Critical_Section
VSL_032:
;Trace_Out "SaveLatches #EBX"
IFDEF	DEBUG
	mov	[Latches_VM],ebx
ENDIF

; set write to all planes
	mov	edx,pSeqIndx
	mov	al,02h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,0Fh				; Write mask for all planes
	out	dx,al
	IO_Delay
	dec	dl

; set write mode 1
	mov	edx,pGrpIndx
	mov	al,05h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[ebx.G_Mode]
IFDEF VGA
	and	al,60h				; Preserve CRTC Shift bit
						;   and Shift 256 bit
ELSE
	and	al,20h				; Preserve CRTC Shift bit
ENDIF
	or	al,1				; Set write mode 1
	out	dx,al
	IO_Delay
	dec	dl
	mov	esi,[VDD_Latch_Addr]		; ESI = place to save latches
	mov	[esi],al			; Write the latches

; set read mode 0, write mode 0
	mov	al,05h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[ebx.G_Mode]
IFDEF VGA
	and	al,60h				; Preserve CRTC Shift bit
						;   and Shift 256 bit
ELSE
	and	al,20h				; Preserve CRTC Shift bit
ENDIF
	out	dx,al
	IO_Delay
	dec	dl

; read the latches
	mov	al,4
	out	dx,al
	IO_Delay
	inc	dl
	xor	ecx,ecx 			; Start with plane 0
VSL_0:
; select next plane
	mov	al,cl
	out	dx,al
	IO_Delay

; Read and save the latch
	mov	al,[esi]

	mov	[edi.VDD_Latches][ecx],al
	inc	ecx
	cmp	cl,3
	jbe	VSL_0

VSL_Skip_Latches:
; Restore plane select
;	mov	al,[ebx.G_MSlct]
;	out	dx,al
;	IO_Delay
;	dec	dl
;;;	jmp	SHORT VSL_033
;;;	VMMCall End_Critical_Section
VSL_033:
IFDEF	DEBUG
	cmp	[Latches_VM],ebx
	jz	SHORT VSL_Exit
Debug_Out "Reentered save latches"
VSL_Exit:
ENDIF
	pop	edi
	pop	esi
	pop	ecx
	pop	ebx
	ret
EndProc VDD_Save_Latches

;******************************************************************************
;VDD_Restore_Latches	       Restore EGA memory latches
;
;DESCRIPTION:
;	Writes saved values into each of the four planes at VDD_Latch_Addr and
;	then does read to put them into the physical latches. Assumes that
;	VDD_Pre_SR has been called to set up registers.
;
;ENTRY: EDI = VDD CB ptr of VM being restored to EGA
;
;EXIT:	none
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_Restore_Latches

Assert_VDD_ptrs ebx,edi
;Trace_Out "Restore_Latches #EDI"
IFDEF	DEBUG
	mov	[Latches_VM],ebx
ENDIF
	test	[edi.VDD_Stt.G_Misc],8		; Q: Graphics mode, not CGA?
	jnz	SHORT VRL_Skip_Latches		;   N: Skip restore latches

;;;	jmp	SHORT VRL_032
;;;	VMMCall Begin_Critical_Section
VRL_032:

	push	esi
	push	ecx
	mov	esi,[VDD_Latch_Addr]		; ESI = place to save latches

; Write all four latches
	mov	edx,pSeqIndx
	mov	al,2
	out	dx,al
	IO_Delay
	inc	dl
	xor	ecx,ecx 			; Plane 0 index
	mov	ah,1				; Plane 0 write mask
VRL_00:
	mov	al,ah
	out	dx,al				; select next plane for write
	IO_Delay

	mov	al,[edi.VDD_Latches][ecx]
	mov	[esi],al			; write this plane's latch value

	inc	cl
	shl	ah,1
	cmp	cl,3
	jbe	VRL_00

; Read into latches
	mov	al,[esi]			; Read data into latches

;;;	jmp	SHORT VRL_033
;;;	VMMCall End_Critical_Section
VRL_033:
	pop	ecx
	pop	esi
VRL_Skip_Latches:
IFDEF	DEBUG
	cmp	[Latches_VM],ebx
	jz	SHORT VRL_Exit
Debug_Out "Reentered restore latches"
VRL_Exit:
ENDIF
	ret
EndProc VDD_Restore_Latches


;******************************************************************************
;VDD_RestCtlr	      Restore EGA controller registers
;
;DESCRIPTION:
;	Restores the state of the VM with a couple of bits in the
;	controllers taken from the attached VM's ctrlr state.
;
;ENTRY: EBX = VM_Handle of VM to restore to EGA
;	EDI = VDD CB ptr of VM to restore to EGA
;
;EXIT:	[Vid_VM_HandleRun] = EBX
;
;USES:	Flags, EAX, ECX, EDX
;
;CALLS:
;
;==============================================================================
BeginProc VDD_RestCtlr
Assert_VDD_ptrs ebx,edi, Pseudo_OK

	mov	eax, [Vid_VM_Handle2]
	cmp	ebx, eax
	je	SHORT VRC_restoring_2nd
	or	eax, eax
	jz	SHORT VRC_000
	add	eax, [VDD_CB_Off]
	mov	[eax.VDD_Routine],OFFSET32 VDD_SGx_JTab ; Set up trap rtns
	jmp	SHORT VRC_000
VRC_restoring_2nd:
	mov	[edi.VDD_Routine],OFFSET32 VDD_SG2_JTab ; Set up trap rtns

VRC_000:
	push	esi

	cmp	ebx, [VDD_Msg_Pseudo_VM]	;Q: restoring msg mode state?
	jne	SHORT VRC_not_msg
	mov	[Vid_VM_HandleRun], 0		;   Y: clear HandleRun
	SetFlag [Vid_Flags], fVid_MsgR		;      set msg mode state bit
	jmp	SHORT VRC_msg
VRC_not_msg:
	mov	[Vid_VM_HandleRun],ebx		;   N: Save handle of VM on EGA
	ClrFlag [Vid_Flags], fVid_MsgR		;      clear msg mode state bit
VRC_msg:

	mov	esi,[Vid_VM_Handle]		; ESI = VM attached to scrn
	or	esi,esi
	jnz	SHORT VRC_00
	mov	esi,ebx
VRC_00:
	add	esi,[VDD_CB_Off]

IFDEF	VGA
	mov	edx,pVGAEna
IFDEF	TLVGA
; Tseng labs has video subsystem enable on two diff ports depending
;	on value in feature control.  Also has another bit to worry about.

	mov	ah,fVGAEna
	TestMem [Vid_Flags],fVid_TLVGA
	jz	SHORT VRC_NotTLEnaRd
	mov	edx,pFeatRead
	in	al,dx
	and	al,3
	cmp	al,3
	mov	edx,pVGAEna
	jnz	SHORT VRC_NotTLEnaRd
	or	ah,8
	mov	edx,46E8h			; Other VGA Enable port
VRC_NotTLEnaRd:
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,ah				; Make sure enable is true
ELSE
	in	al,dx
	IO_Delay
	mov	[edi.VDD_STT.V_VGAEna],al
	or	al,fVGAEna			; Make sure enable is true
ENDIF
	out	dx,al
	IO_Delay

ENDIF

	cmp	ebx, [VDD_Msg_Pseudo_VM]	;Q: restore msg mode state?
	je	SHORT VRC_01			;   Y: don't use latches

	call	VDD_Restore_Latches		; Restore the memory latches
VRC_01:
	push	edi
	lea	edi,[edi.VDD_Stt]
	lea	esi,[esi.VDD_Stt]

	mov	edx,pSeqIndx
;*******
; Restore Sequencer write mask
	mov	al,02h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.S_Mask]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; Restore Sequencer memory mode
	mov	al,04h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.S_MMode]
	and	al,0FEh 			; strip CRT font select bit
	mov	ah,[esi.S_MMode]
	and	ah,1
	or	al,ah				; Add bit from focus VM
	out	dx,al
	IO_Delay

;*******
; pass through Set/Reset
;
	mov	edx,pGrpIndx
	mov	al,0
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_SetRst]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; pass through enable Set/Reset
;
	mov	al,1
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_SREna]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; pass through Color Compare
;
	mov	al,2
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_CComp]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; pass through Raster OP function
;
	mov	al,3
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_Func]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; pass through Read Map Select
;
	mov	al,4
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_MSlct]
	out	dx,al
	IO_Delay
	dec	dl

;*******
; Preserve Mode Odd/Even bits bit from attached VM
;
	mov	al,5
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_Mode]
	mov	ah,[esi.G_Mode]
IFDEF VGA
	and	al,NOT 60h			; strip shift bits
	and	ah,60h				; Preserve CRTC Shift bit
						;   and Shift 256 bit
ELSE
	and	al,NOT 20h			; strip shift bit
	and	ah,20h				; Preserve CRTC Shift bit
ENDIF
	or	al,ah
	out	dx,al
	IO_Delay
	dec	dl

;*******
; Preserve Misc. Text/Graphics bit from attached VM
;
	mov	al,6
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_Misc]
	and	al,0FEh 			; strip odd/even bits bit
	mov	ah,[esi.G_Misc]
	and	ah,01h				; Add in bit from focus VM
	or	al,ah
	out	dx,al
	IO_Delay
	dec	dl

;*******
; pass through Color Don't Care
;
	mov	al,7
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_DCare]
	out	dx,al
	IO_Delay
	dec	dl


;*******
; pass through bit mask
;
	mov	al,08h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_Mask]
	out	dx,al
	IO_Delay
	dec	dl

IFDEF	PVGA
;*******
; If Paradise VGA, restore extended registers
;
	TestMem [Vid_Flags],fVid_PVGA		; Q: Paradise VGA?
	jz	SHORT VRC_PVGA9
	mov	eax,G_PVGA5-G_SetRst		;   Y: restore paradise regs
	out	dx,al
	IO_Delay
	inc	edx
	mov	al, 5				; Unlock access
	out	dx,al
	IO_Delay
	dec	edx
VRC_PVGA1:

	mov	al,G_PVGA0A-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[edi.G_PVGA0A]		; Restore Addr0A
	out	dx,al
	IO_Delay
	dec	edx
VRC_PVGA2:

	mov	al,G_PVGA0B-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[edi.G_PVGA0B]		; Restore Addr0B
	out	dx,al
	IO_Delay
	dec	edx
VRC_PVGA3:

	mov	al,G_PVGA1-G_SetRst
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[edi.G_PVGA1]		; Restore memory configuration
	out	dx,al
	IO_Delay
	dec	edx
VRC_PVGA4:

	mov	al,G_PVGA5-G_SetRst		; Restore access lock/unlock
	out	dx,al
	IO_Delay
	inc	edx
	mov	al,[edi.G_PVGA5]
	out	dx,al
	dec	edx

VRC_PVGA9:
ENDIF	;PVGA

IFDEF	PEGA
;*******
; pass through bit mask
;
	mov	al,08h
	out	dx,al
	IO_Delay
	inc	dl
	mov	al,[edi.G_Mask]
	out	dx,al
	IO_Delay
	dec	dl
ENDIF

	pop	edi
IFDEF	V7VGA
	call	V7_RestCTLR
ENDIF
IFDEF	ATIVGA
EXTRN	ATi_RestCTLR:NEAR
	call	ATi_RestCTLR
ENDIF

;*******
; Restore indices
	bt	DWORD PTR [esi.A_Indx],fVAI_ScOnBit
	cmc					; Q: attached VM CRT on?
	pop	esi
	jmp	VDD_RestIndx			; Restore indices
EndProc VDD_RestCtlr

;******************************************************************************
;VDD_SaveRegs_Event
;
;DESCRIPTION:
;	This routine is called to update any registers in the controller
;	data structure that are not correct for the VM running in the
;	EGA or VGA.
;
;ENTRY: EBX = running VM handle
;	EDX = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI
;
;==============================================================================
BeginProc VDD_SaveRegs_Event,PUBLIC

	mov	edi,edx

EndProc VDD_SaveRegs_Event

;******************************************************************************
;VDD_SaveRegs
;
;DESCRIPTION:
;	This routine is called to update any registers in the controller
;	data structure that are not correct for the VM running in the
;	EGA or VGA.
;
;ENTRY: EBX = running VM handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 0 indicates latches not saved
;	CF = 1 indicates latches saved
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_SaveRegs,PUBLIC

	cmp	ebx,[Vid_VM_Handle]		; Q: running VM attached?
IFDEF	VGA
	jnz	SHORT VSR_Exit			;   N: skip reg save
	call	VDD_SaveCtlr			;Read VGA registers
ENDIF
IFDEF	EGA
	jnz	VSR_Exit			;   N: skip save (regs trapped)
	TestMem [edi.VDD_Flags],fVDD_NoTrap	; Q: Focus VM save own regs?
	jz	SHORT VSR_01			;   N: Go save the registers
	bts	[edi.VDD_Flags],fVDD_NTSaveBit	; Q: Already did save?
	jc	VSR_Exit			;   Y: Don't save again
	TestMem [Vid_Flags],fVid_Crit		; Q: In video critical section
	jnz	SHORT VSR_Err			;   Y: can't do save!!
	VMMCall Test_Sys_VM_Handle		; Q: System VM?
	jz	VDD_Save_HW_Regs		;   Y: Special save algorithm
	VMMCall Test_Cur_VM_Handle
	jz	SHORT VSR_00
	ClrFlag [edi.VDD_Flags], fVDD_NTSave
	mov	esi,OFFSET32 VDD_SaveRegs_Event
	mov	eax,VDD_Pri_Device_Boost+1	; force attach VM to schedule
	mov	ecx,0
	mov	edx,edi
	VMMCall Call_Priority_VM_Event		; Save in attached VM context
	TestMem [edi.VDD_Flags],fVDD_NTSave	; Q: Already did save?
	jnz	SHORT VSR_Exit			;   Y: Good, all done
                                                ;   N: Darn!
VSR_Err:
Debug_Out "VDD: SaveRegs error: Cannot save regs now"
	SetFlag [edi.VDD_EFlags],fVDE_Crit
	jmp	SHORT VSR_Exit

VSR_00:
	stc					; Saved latches
	ret
VSR_01:
	TestMem [edi.VDD_Flags],fVDD_ModeSet	; Q: Doing mode set?
	jnz	SHORT VSR_Exit			;   Y: trapping all ports
IFDEF	ATIEGA
	call	Save_ATI_Regs
ENDIF
	TestMem [edi.VDD_PIF],fVidNoTrpTxt	; Q: No trapping enabled?
	jz	SHORT VSR_Exit			;   N: No more regs to read
	call	VDD_Get_Mode
	cmp	[edi.VDD_ModeEGA],3		; Q: Text mode?
	ja	SHORT VSR_Exit			;   N: No registers to read
; Special no trap text mode, must read cursor registers
IFDEF PEGA
	mov	eax,[edi.VDD_Cur_CRTC]
        add     eax,C_AddrH
        push    eax
ENDIF
	mov	edx,3d4h
	mov	ecx,C_CAddrL-C_AddrH+1
	lea	esi,[edi.VDD_Stt.C_AddrH]
	xchg	esi,edi
	mov	ah, C_AddrH-C_HTotal
        cld
VSR_02:
	mov	al,ah
	out	dx,al
	IO_Delay
	inc	edx
	in	al,dx

IFDEF PEGA
        xchg    edi,[esp]
        stosb
        xchg    edi,[esp]
ENDIF
	stosb
	dec	edx
	inc	ah
	loopd	VSR_02
IFDEF PEGA
        add     esp,4
ENDIF
	xchg	esi,edi
ENDIF	; EGA
VSR_Exit:
	clc					; Did not save latches
	ret

IFDEF	ATIEGA
; ATI register save/restore registers 7 through 1Fh
Save_ATI_Regs:
	TestMem [Vid_Flags],fVid_ATI800
	jz	SHORT Save_ATI_Ret
	mov	edx,pATI_Indx
	in	al,dx
	mov	[edi.VDD_Stt.ATI_Indx],al	; Save index
	xor	eax,eax
	call	In_ATI_800
	mov	[edi.VDD_Stt.ATI_Regs],al	 ; Reg 0
	mov	al,1
	call	In_ATI_800
	mov	[edi.VDD_Stt.ATI_Regs.1],al	 ; Reg 1
	mov	al,2
	call	In_ATI_800
	mov	[edi.VDD_Stt.ATI_Regs.2],al	 ; Reg 2
	mov	al,0Ah
	call	In_ATI_800
	mov	[edi.VDD_Stt.ATI_Regs.3],al	 ; Reg 3
	mov	eax,0Bh 			; Reg 0Bh - 1Eh
ATI_Save_Loop:
	mov	ecx,eax
	call	In_ATI_800
	mov	[edi.VDD_Stt.ATI_Regs][ecx],al
	mov	eax,ecx
	inc	eax
	cmp	al,01fh
	jb	ATI_Save_Loop
	mov	al,[edi.VDD_Stt.ATI_Indx]
	out	dx,al				; Restore index
Save_ATI_Ret:
	ret


In_ATI_800:
	out	dx,al				; Output index
	IO_Delay
	inc	edx
	inc	edx
	in	al,dx				; Enable access
	IO_Delay
	dec	edx
	dec	edx
	ret
ENDIF
EndProc VDD_SaveRegs

IFDEF	EGA
;******************************************************************************
;VDD_Save_HW_Regs	Save Hardware Video Registers
;
;DESCRIPTION:
;   THIS IS TAKEN FROM EGASTATE.ASM.  It must be identical to the code there.
;   It is used to asynchronously save the EGA registers that are not identical
;   to the values that existed when the INT 2F, function 4000h call was made.
;
; This routine is called by the VDD_SaveRegs code whenever
; the state of the EGA registers must be saved.  The contents of the
; following registers are saved:
;
;       Map Mask
;       Enable Set/Reset
;       Data Rotate
;       Read Map Select
;       Bit Mask
;	Processor Latches - not modified
;
;   Additional registers not saved/restore by the EGASTATE.ASM routines
;	but that need to be saved:
%out what about these add'l graphics controller regs.
; //	    Graphics Controller:    Set/Reset
; //				    Color Compare
; //				    Color Don't Care
;
; The VDD_Restore routine must call RES_HW_REGS to restore
; the registers and prepare the internal work areas for the next call
; to this routine.
;
;ENTRY: EDI = VDD CB ptr
;
;EXIT:	CF = 0 (latches not saved)
;	GRAF_MODE	= M_DATA_READ+M_PROC_WRITE	(EGA register)
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;	GRAF_BIT_MASK	= all bits enabled		(EGA register)
;	GRAF_DATA_ROT	= DR_SET			(EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
;
;USED:	EAX,ECX,EDX,FLAGS
;	GRAF_ADDR					(EGA register)
;	SEQ_ADDR					(EGA register)
;
; History:
;	Tue 06-Jan-1987 12:42:56
;	Initial version
;	27-Mar-1989 12:16:09
;	Modified from EGAState.asm - MDW
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; //
; //	New EGA Shadow Specification
; //
; //	The hypothesis is that there is a method wherein the state
; //	of the EGA registers used by the Color EGA Drivers can be
; //	determined at interrupt time, with minimum limitations on
; //	the display driver.
; //
; //	The registers used by the current color drivers are:
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Set/Reset
; //				    Enable Set/Reset
; //				    Color Compare
; //				    Data Rotate
; //				    Read Map Select
; //				    Mode
; //				    Color Don't Care
; //				    Bit Mask
; //
; //
; //	Of these registers, the following are currently shadowed for
; //	the interrupt cursor drawing code:
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Enable Set/Reset
; //				    Data Rotate
; //				    Mode
; //				    Bit Mask
; //
; //
; //
; //	By following the prescribed algorithm, the following registers
; //	could be saved and restored at interrupt time:
; //
; //	    Sequencer:		    Map Mask
; //
; //	    Graphics Controller:    Enable Set/Reset
; //				    Data Rotate
; //				    Read Map Select
; //				    Bit Mask
; //				    Set/Reset
; //				    Color Compare
; //	    Processor Latches
; //
; //
; //	The limitations proposed are:
; //
; //	    1)	The driver must indicate if it is in Data Read mode or
; //		Color Compare Read mode.  The driver must indicate the
; //		write mode in use.  These values are part of the same
; //		register, so only one register need be shadowed.
; //
; //
; //	The process:
; //
; //	    1)	Place the EGA into Write Mode 1.  In this mode, the
; //		contents of the EGA's latches are written to memory.
; //		Since the Read Mode is part of the same byte, it
; //		will be set to Data Read mode, which will be required
; //		by the state detection code.
; //
; //
; //	    2)	Determine the state of the Map Mask Register.
; //		This is accomplished by:
; //
; //		A)  Write to two locations in EGA memory where the
; //		    bytes are known to be different in the same
; //		    plane.  These locations will be maintained by
; //		    the cursor code and EGA initialization code in
; //		    this state.
; //
; //		B)  If the bytes are the same for a given plane, then
; //		    that plane was enabled for writing.  If the bytes
; //		    are different for a given plane, then that plane
; //		    was not enabled for writing.
; //
; //		The Map Mask Register may now be altered.
; //
; //
; //	    3)	Save the EGA Processor Latches.  This is accomplished
; //		by:
; //
; //		    A)	Enabling all planes for writing.
; //
; //		    B)	Writing to a predefined location in video RAM
; //			in Write Mode 1.
; //
; //		This location (in all four planes) now contains the
; //		contents of the Processor Latches at the time of the
; //		interrupt.
; //
; //		The Processor Latches may now be altered.
; //
; //
; //	    4)	Determined the Read Map Select Register.  Since the
; //		EGA has already been placed into Data Read mode,
; //		simply read from a predefined location.  Each plane
; //		of this location has the plane index value in it.
; //		The value returned will be the Read Map Select index.
; //
; //		The Read Map Select Register may now be altered.
; //
; //
; //	    5)	Determine the state of the Bit Mask Register and the
; //		Data Rotate Register.  Since the EGA driver doesn't
; //		use the Rotate Count, only the boolean function of
; //		the Data Rotate register need be determined.  This is
; //		accomplished by the following:
; //
; //		    A)	Enable Write Mode 2.  In Write Mode 2, the
; //			Enable Set/Reset Register and the Rotate
; //			Count are ignored.  Any host data is taken to
; //			be a color, with 8 bits of D0 being written
; //			to C0, and so forth for the other planes.
; //
; //		    B)	Perform two writes (XCHGs) to EGA memory in
; //			a location where the planes are defined as
; //			follows:
; //
; //				C3 C2 C1
; //
; //				FF FF 00
; //
; //			The first write consists of writing 00 to
; //			plane C1.  The second write consists of
; //			writing FF to planes C2 and C3.
; //
; //		    C)	By interpreting the results, the value of the
; //			Bit Mask Register and the Data Rotate Register
; //			can be determined:
; //
; //				C3 C2 C1
; //
; //				FF FF 00     Bit Mask is 0
; //				FF 00 FF     Mode is SET
; //				FF FF FF     Mode is OR
; //				FF 00 00     Mode is AND
; //				00 FF FF     Mode is XOR
; //
; //			Anywhere that the result is the original value,
; //			the Bit Mask Register was 0.  If all bits show
; //			the original value, then the Bit Mask Register
; //			was 0 in all bits, and the test must be rerun
; //			with the Bit Mask enabled for at least one bit
; //			(this will be the case when copying from screen
; //			to screen).
; //
; //			Note that C0 never participates in the decision.
; //
; //			Any '1' bit in C1 indicates a bit which changed.
; //			Any '0' bit in C2 or C3 indicates a bit which
; //			changed.
; //
; //
; //	    6)	Determine the state of the Enable Set/Reset Register.
; //		This is done by the following:
; //
; //		    A)	Enable all bits of the Bit Mask Register.
; //
; //		    B)	Enable writing to all planes.
; //
; //		    C)	Set Write Mode 0.  Enable Set/Reset is
; //			recognized in Write Mode 0.
; //
; //		    D)	Set Data Rotate Function Select to SET.
; //
; //		    E)	Write 55h to a location.
; //
; //		    F)	Any plane of the location where the result
; //			isn't 55h was enabled for Set/Reset.
; //
; //	The state of the registers has now been determined.
;
; NULL save_hw_regs()
; {
;   perform steps 1 thru 6;
;   GRAF_ENAB_SR = all planes disabled;
;   return();
; }
;==============================================================================

;	EGA Register Definitions.
;
;	The following definitions are the EGA registers and values
;	used by this driver.  All other registers are set up when
;	the EGA is placed into graphics mode and never altered
;	afterwards.
;
;	All unspecified bits in the following registers must be 0.


EGA_BASE	equ	300h		;Base address of the EGA (3xx)



;	Sequencer Registers Used

SEQ_ADDR	equ	0C4h		;Sequencer Address Register
SEQ_DATA	equ	0C5h		;Sequencer Data    Register

SEQ_MAP_MASK	equ	02h		;Write Plane Enable Mask
MM_C0		equ	00000001b	;  C0 plane enable
MM_C1		equ	00000010b	;  C1 plane enable
MM_C2		equ	00000100b	;  C2 plane enable
MM_C3		equ	00001000b	;  C3 plane enable
MM_ALL		equ	00001111b	;  All planes



;	Graphics Controller Registers Used

GRAF_ADDR	equ	0CEh		;Graphics Controller Address Register
GRAF_DATA	equ	0CFh		;Graphics Controller Data    Register

GRAF_SET_RESET	equ	00h		;  Set/Reset Plane Color
GRAF_ENAB_SR	equ	01h		;  Set/Reset Enable
GRAF_COL_COMP	equ	02h		;  Color Compare Register

GRAF_DATA_ROT	equ	03h		;  Data Rotate Register
DR_ROT_CNT	equ	00000111b	;    Data Rotate Count
DR_SET		equ	00000000b	;    Data Unmodified
DR_AND		equ	00001000b	;    Data ANDed with latches
DR_OR		equ	00010000b	;    Data ORed	with latches
DR_XOR		equ	00011000b	;    Data XORed with latches

GRAF_READ_MAP	equ	04h		;  Read Map Select Register
RM_C0		equ	00000000b	;    Read C0 plane
RM_C1		equ	00000001b	;    Read C1 plane
RM_C2		equ	00000010b	;    Read C2 plane
RM_C3		equ	00000011b	;    Read C3 plane

GRAF_MODE	equ	05h		;  Mode Register
M_PROC_WRITE	equ	00000000b	;    Write processor data rotated
M_LATCH_WRITE	equ	00000001b	;    Write latched data
M_COLOR_WRITE	equ	00000010b	;    Write processor data as color
M_AND_WRITE	equ	00000011b	;    Write (procdata AND bitmask)
M_DATA_READ	equ	00000000b	;    Read selected plane
M_COLOR_READ	equ	00001000b	;    Read color compare

GRAF_CDC	equ	07h		;  Color Don't Care Register
GRAF_BIT_MASK	equ	08h		;  Bit Mask Register


;	The func_select table is used to translate the results of step
;	5C into the actual value for the Data Rotate register.	The
;	algorithm which scans the planes to determine the boolean
;	operation of the Data Rotate register returns 0 wherever a
;	changed occured in the table listed for step 5C.
;
;	Since C0 is never involved in the result, it is masked off
;	via a SHR instruction.


func_select     label   byte
        db      DR_SET                  ;000 - illegal
        db      DR_SET                  ;001 - illegal
        db      DR_XOR                  ;010 - XOR
        db      DR_SET                  ;011 - illegal
        db      DR_SET                  ;100 - SET
        db      DR_AND                  ;101 - AND
        db      DR_OR                   ;110 - OR
        db      DR_SET                  ;111 - illegal

OUT16	MACRO
	out	dx,al
	inc	edx
	IO_Delay
	xchg	ah,al
	out	dx,al
	IO_Delay
	dec	edx
	xchg	ah,al
	ENDM


BeginProc VDD_Save_HW_Regs

	push	ebx
	push	esi
	mov	[Vid_VM_HandleRun],0		; No VM in registers after this
	call	VDD_Global_Restore
	mov	esi,[VDD_SYS_VRAMAddr]		; ESI = start of SYS VM Mem

;	Perform steps 1 and 2A for determining the value of the Map
;	Mask register.	The actual value of the Map Mask cannot be
;	computed until after the processor latches have been saved.
;	This step also sets Data Read mode which is required by the
;	Read Map detection code.

        mov     dx,EGA_BASE+GRAF_ADDR
	mov	ax,M_LATCH_WRITE shl 8 + GRAF_MODE
	out16	dx,ax
	mov	[esi.known_word],ax


;	Perform step 3 to save the current contents of the processor
;	latches.  Note that the Sequencer Address Register always has
;	the address of the Map Mask register in it, so the output
;	operation can be a single 8-bit I/O operation.

        mov     dl,SEQ_DATA
        mov     al,MM_ALL
        out     dx,al
	IO_Delay
	mov	[esi.saved_latches],al


;	Perform step 4 to determine and save the contents of the
;	Read Map register.

	mov	al,[esi.plane_index]
	mov	[edi.VDD_Stt.G_MSlct],al

;	The result of the Bit Mask test will be ANDed into SAVED_BIT_MASK.
;	This allows the test loop to be repeated when the Bit Mask is all
;	zeros and still return the correct result (FF AND 00 AND ?? = 00).

	mov	[edi.VDD_Stt.G_Mask],0FFh


;	Perform steps 5A and 5B to set up for determining the value of
;	the Data Rotate register.

        mov     dl,GRAF_DATA            ;Graphics Controller still has
        mov     al,M_COLOR_WRITE        ;  address of Mode register
        out     dx,al
	IO_Delay

save_hw_regs_retry:
        mov     dl,SEQ_DATA
        mov     ax,MM_C0+MM_C2
        out     dx,al
	IO_Delay
	xchg	ah,[esi.enable_test]
.ERRNZ	(MM_C0+MM_C2) AND 0FF00h      ;AH must be zero
        mov     ax,0FF00h+MM_C1+MM_C3
        out     dx,al
	IO_Delay
	xchg	ah,[esi.enable_test]

;	Perform step 5C to determine the boolean function set in the
;	Data Rotate register, and the value of the Bit Mask register.
;	If no bits are enabled in the Bit Mask register, then the
;	operation must be repeated with at least one bit of the Bit
;	Mask enabled.
;
;       Step 2B will also be performed within the same read loop.
;
;
;       The loop will use the following registers:
;
;               AX      work
;               BL      XOR test mask, Function Select mask
;               BH      Read Map Select
;               CH      accumulates Map Mask
;               CL      accumulates Bit Mask
;
;	The XOR test mask is XORed with the contents of the ENABLE_TEST
;	byte for the given plane.  The result of the XOR will give a 1
;	for any bit which changed (and thus was enabled in the bitmask)
;
;	The function select mask is used to accumulate planes where a
;	change took place so that Function Select can be determined via
;	a table lookup.  A 0 will be returned for each plane where a
;	change occured.


        mov     dl,GRAF_ADDR            ;Set Graphics Controller Addr Reg
        mov     al,GRAF_READ_MAP        ;  to Read Map Select Register
	out	dx,al
	IO_Delay
	inc	dx
	mov	ebx,RM_C3 shl 8 + 0C0h	;BH = Read Map index and loop counter
                                        ;BL = the XOR mask
        xor     cx,cx                   ;Accumulates Map Mask and Bit Mask
.ERRNZ	(RM_C3+1)-4			;Execute loop for all four planes

save_hw_next_plane:
        mov     al,bh                   ;Set Read Map Select
        out     dx,al
	IO_Delay
	mov	ax,[esi.known_word]	;Determine Map Mask for this plane
	xor	al,ah			;  Set 'C' if identical, indicating
	cmp	al,1			;    this plane was enabled
	rcl	ch,1			;  Save Map Enable for this plane
        mov     al,bl                   ;Get XOR test mask into AH
        cbw
	xor	ah,[esi.enable_test]	;AH = 1 wherever bit changed
        or      cl,ah                   ;Accumulate Bit Mask
        cmp     ah,1                    ;Want 'C' if no bits changed
        adc     bl,bl                   ;Accum function, set next XOR mask
        dec     bh                      ;Set next Read Map index
	jns	save_hw_next_plane	;More planes to process
	and	[edi.VDD_Stt.G_Mask],cl ;Save Bit Mask register value

;       Enable all bits of the Bit Mask in preparation of step 6.
;       This will also have to be done for retrying the boolean
;       function if the Bit Mask was 0, so this handles both cases.


	dec	dx			;--> Graphics Controller Addr reg
        mov     ax,0FF00h+GRAF_BIT_MASK ;Enable all bits for alteration
	out16	dx,ax

        or      cl,cl                   ;Retry if no bits enabled in Bit Mask
        jz      save_hw_regs_retry      ;  No bits, retry

;
;       Finish steps 2B and 5C.  To finish step 2B, save the computed
;	Map Mask value.  To finish step 5C, take the index computed for
;	the boolean function and perform a table lookup to determine
;	the actual value of the Data Rotate register.


	mov	[edi.VDD_Stt.S_Mask],ch ;Finish 2B - Save Map Mask
	xor	bh,bh			;Finish 5C
	shr	bx,1			;  D0 is always 0, so the table
	mov	al,func_select[ebx]	;    can be 8 bytes instead of 16
	and	[edi.VDD_Stt.G_Func],al ;  Save Data Rotate/Function


;       Perform steps 6B through 6E to prepare for determining
;       Enable Set/Reset. Step 6A has already been performed.


        mov     dl,SEQ_DATA
        mov     al,MM_ALL
        out     dx,al
	IO_Delay

        mov     dl,GRAF_ADDR
	mov	ax,M_PROC_WRITE shl 8 + GRAF_MODE
	out16	dx,ax
	mov	bx,5500h		;BH = pattern, BL = Set/Reset accum

        mov     al,GRAF_DATA_ROT
;       mov     ah,DR_SET
.ERRNZ	DR_SET-M_PROC_WRITE		;Must be the same
	out16	dx,ax

	mov	[esi.enable_test],bh


;       Perform step 6F.  Any plane which didn't have the 55h written
;       in the location ENABLE_TEST is enabled for Set/Reset

	inc	eax			;Set the Graphics Controller Address
        out     dx,al                   ;  Register to point to the Read Map
	IO_Delay
.ERRNZ	GRAF_READ_MAP-GRAF_DATA_ROT-1

	inc	edx			;--> Graphics Controller Data register
;       mov     ax,RM_C3                ;AH accumulates Enable mask
	dec	eax			;AL is Read Map and loop counter
.ERRNZ	GRAF_READ_MAP-RM_C3-1

save_hw_regs_sr_loop:
        out     dx,al                   ;Set read plane
	IO_Delay
	mov	cl,[esi.enable_test]	;Set 'C' if byte isn't 55h
        xor     cl,bh
	jz	SHORT SHWRSL0
	stc
SHWRSL0:
        adc     ah,ah                   ;Propagate 'C' as Enable bit
        dec     al                      ;Set next plane
        jns     save_hw_regs_sr_loop    ;More planes to test
	mov	[edi.VDD_Stt.G_SREna],ah;Save Enable Set/Reset

;***************
;***************
; New code: save Set/Reset register
;	Any plane which is all 0's must have 0 in the set/reset reg
;	Any plane which is all 1's must have 1 in the set/reset reg
	dec	edx			;--> Graphics Controller Addr register
	mov	ax,0F00h+GRAF_ENAB_SR
	out16	dx,ax			; Enable S/R for all planes
	mov	[esi.enable_test],al	;Put S/R reg value in all planes

	mov	ax,GRAF_READ_MAP	;AH=0 (accumulates S/R mask)
	out	dx,al			; Register to point to the Read Map
	IO_Delay

	inc	edx			;--> Graphics Controller Data register
;	mov	ax,RM_C3
	dec	eax			;AL is Read Map and loop counter
.ERRNZ	GRAF_READ_MAP-RM_C3-1

save_hw_sr_loop:
        out     dx,al                   ;Set read plane
	IO_Delay
	cmp	al,[esi.enable_test]	;Set 'C' if byte is all ones
        adc     ah,ah                   ;Propagate 'C' as Enable bit
        dec     al                      ;Set next plane
	jns	save_hw_sr_loop 	;More planes to test
	mov	[edi.VDD_Stt.G_SetRst],ah ;Save Set/Reset
;***************
;***************


	dec	edx			;--> Graphics Controller Addr register
	mov	al,GRAF_ENAB_SR
	out	dx,al
	IO_Delay
	inc	edx
	xor	al,al			;Disable any set/reset
	out	dx,al
	IO_Delay


	mov	al,[esi.saved_latches]	;Reload latches
	clc				;Save latches on return
	pop	esi
	pop	ebx
IF 0
Trace_Out "Sav_HW #EDI, " +
	mov	al,[edi.VDD_Stt.S_Mask]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_Mask]
Trace_Out "#AL "
	mov	al,[edi.VDD_Stt.G_SREna]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_SetRst]
Trace_Out "#AL "
	and	al,[edi.VDD_Stt.G_Func]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_MSlct]
Trace_Out "#AL "
ENDIF
        ret
EndProc VDD_Save_HW_Regs

;-----------------------------Public-Routine----------------------------;
; VDD_Res_HW_Regs
;
; Restore Hardware Video Registers
;
; This routine is called Restore routine to restore the SYS_VRAM area.
;
;       Map Mask
;       Enable Set/Reset
;       Data Rotate
;       Read Map Select
;       Bit Mask
;       Mode
;       Processor Latches
;
;ENTRY: GRAF_MODE	= M_DATA_READ+M_PROC_WRITE	(EGA register)
;       GRAF_ENAB_SR    = all planes disabled           (EGA register)
;       SEQ_ADDR        = SEQ_MAP_MASK                  (EGA register)
;       SEQ_MAP_MASK    = all planes enabled            (EGA register)
;
;EXIT:	GRAF_ADDR	= GRAF_BIT_MASK 		(EGA register)
;
;USES:	AX,DX,FLAGS
;
;HISTORY:
;	Tue 06-Jan-1987 12:42:56
;	Initial version
;	27-Mar-1989 12:16:09
;	Modified from EGAState.asm - MDW
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; NULL res_hw_regs()
; {
;   GRAF_DATA_ROT	= DR_SET;	    // must use DR_SET mode
;   GRAF_BIT_MASK	= 0xFF; 	    // must alter all bits
;   set_test_locs;			    // init locations for next time
;   SEQ_MAP_MASK	= saved_map_mask;   // restore saved registers
;   GRAF_READ_MAP	= saved_read_map;
;   GRAF_DATA_ROT	= saved_data_rot;
;   GRAF_ENAB_SR	= saved_enab_sr;
;   GRAF_MODE		= shadowed_mode[?]; // just use current plane
;   GRAF_BIT_MASK	= saved_bit_mask;   // must be last written
;   return();				    //	 to GRAF_ADDR
; }
;-----------------------------------------------------------------------;

BeginProc VDD_Res_HW_Regs

	push	ebx
	push	esi
	mov	esi,[VDD_SYS_VRAMAddr]		; ESI = start of SYS VM Mem

        mov     dx,EGA_BASE + GRAF_ADDR
	mov	ax,DR_SET shl 8 + GRAF_DATA_ROT
	out16	dx,ax
        mov     ax,0FF00h+GRAF_BIT_MASK
	out16	dx,ax

;set_test_locs
        mov     bx,00FFh                ;Set 00FF into all planes
	mov	[esi.known_word],bx	;  of a known word

;***************
;The following is not in the display driver code, but is done here
;to implement the 2nd EGA handling. We take the 2nd half of the memory
;away from the windows driver when another VM is running in the memory
;and return it when the VM is no longer running in the memory

	mov	al,[esi.shadow_mem_status]
	cmp	[Vid_VM_Handle2],0		; Q: 2nd half EGA in use?
	jz	SHORT VRHWR_ShadowExists	;   N: Let Windows have 2nd EGA
	btr	eax, shadow_exists_Bit		;   Y: Reset shadow exists
	jnc	SHORT VRHWR_NoChg		;	If already clear, done
	or	al, shadow_trashed
	jmp	SHORT VRHWR_SaveStat		;	Else save changes
VRHWR_ShadowExists:
	bts	eax, shadow_exists_Bit		;   Y: Set shadow exists
	jc	SHORT VRHWR_NoChg		;	If already set, done
	and	al,NOT shadow_trashed+shadow_in_use
VRHWR_SaveStat:
	mov	[esi.shadow_mem_status],al
VRHWR_NoChg:

;***************

	mov	dl,SEQ_DATA		;Set 00 into C0 and C1
	mov	al,MM_C0+MM_C1
        out     dx,al
	IO_Delay
	mov	[esi.enable_test],bh
        mov     al,MM_C2+MM_C3          ;Set FF into C2 and C3
        out     dx,al
	IO_Delay
	mov	[esi.enable_test],bl
;end set_test_locs

	mov	al,[edi.VDD_Stt.S_Mask]
	out	dx,al
	IO_Delay
        mov     dl,GRAF_ADDR
	mov	ah,[edi.VDD_Stt.G_MSlct]
	mov	al,GRAF_READ_MAP
	out16	dx,ax
	mov	ah,[edi.VDD_Stt.G_Func]
	mov	al,GRAF_DATA_ROT
	out16	dx,ax
	mov	ah,[edi.VDD_Stt.G_SREna]
	mov	al,GRAF_ENAB_SR
	out16	dx,ax
	mov	ah,[edi.VDD_Stt.G_SetRst]
	mov	al,GRAF_SET_RESET
	out16	dx,ax
        mov     al,GRAF_MODE
	mov	ah,[esi.shadowed_mode]	;Get user's read and write mode
	out16	dx,ax
	mov	ah,[edi.VDD_Stt.G_Mask]
	mov	al,GRAF_BIT_MASK
	out16	dx,ax
	mov	al,[esi.saved_latches]
	pop	esi
IF 0
Trace_Out "Res_HW #EDI, " +
	mov	al,[edi.VDD_Stt.S_Mask]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_Mask]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_SREna]
Trace_Out "#AL " +
	and	al,[edi.VDD_Stt.G_Func]
Trace_Out "#AL " +
	mov	al,[edi.VDD_Stt.G_MSlct]
Trace_Out "#AL "
ENDIF
	pop	ebx
        ret

EndProc  VDD_Res_HW_Regs

ENDIF	; EGA


VxD_CODE_ENDS

	END


