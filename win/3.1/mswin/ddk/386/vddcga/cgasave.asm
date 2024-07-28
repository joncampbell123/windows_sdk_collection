       title   VDD - Virtual Display Device for CGA version 3.00
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;
;DESCRIPTION:
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE CGA.INC
	INCLUDE DEBUG.INC
	INCLUDE VDD.INC

;******************************************************************************

;******************************************************************************
;
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_EMode_Table:WORD
	EXTRN	VDD_Msg_VM:DWORD

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
	DD	0000000FFh		; mode 7, IDC only


PUBLIC VDD_MsgStt	
VDD_MsgStt	    DB SIZE CGA_State_struc DUP (?)

PUBLIC	VDD_Rest_Event_Handle
VDD_Rest_Event_Handle DD  ?

; Save/restore addresses initialized by Sys_Critical_Init
PUBLIC	VDD_PhysB8000
VDD_PhysB8000	    DD	?		; Address of physical mem at B8000

VxD_DATA_ENDS

VxD_CODE_SEG
	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Mem_Chg:NEAR
	EXTRN	VDD_Mem_Null:NEAR
	EXTRN	VDD_IO_SetTrap:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_Scrn_Size:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_VMDA_Grab:NEAR
	EXTRN	VDD_Mem_ACopy:NEAR
	EXTRN	VDD_Mem_UpdOff:NEAR
	EXTRN	VDD_SetCopyMask:NEAR

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

        VMMCall Test_Sys_VM_Handle
        jnz     SHORT VS_00             
        call    VDD_Mem_Null            ; Map null memory for Sys VM
; Tell Sys VM switching to background
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Backgrnd_Event
	mov	eax,VDD_Pri_Device_Boost
	mov	ecx,0
	VMMCall Call_Priority_VM_Event		
VS_00:        
; Save video state
	call	VDD_Scrn_Off			; Turn off screen
	call	VDD_SaveRegs                    ; save CRTC regs
	call	VDD_Get_Mode			
	mov	[edi.VDD_Pg.VPH_Mode],al       ; update mode for copy pages
        stc
	call	VDD_SRAM			
	ret
EndProc VDD_Save

BeginProc VDD_Backgrnd_Event

	pushad
	mov	edi,edx
	test	[edi.VDD_Flags],fVDD_Sus
	jnz	SHORT VEB_Susp
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: VM still attached?
	jnz	SHORT VEB_Detached		;   N: Set up before INT 2F
VEB_Normal:
	call	VEB_CallApp
	popad
	ret
VEB_Susp:
%OUT Set flag to test if focus changes while suspended
	popad
	ret
VEB_Detached:
	test	[VDD_Focus_VM],-1
	jz	SHORT VEBD_00

	push	ebx
	push	edi
	mov	ebx,[VDD_Focus_VM]
	mov	edi,ebx
	add	edi,[VDD_CB_Off]
	call	VDD_Restore 		; Make sure focus restored
	pop	edi
	pop	ebx

VEBD_00:
	call	VDD_Mem_Chg
	call	VDD_RestCtlr		; Restore ctrlr state for INT 2F
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

	test	[VDD_Rest_Event_Handle],-1	; Q: Restore pending?
	jnz	SHORT VR_Exit			;   Y: All done
	test	[vgVDD.Vid_Flags],fVid_Msg	; Q: Message mode?
	jnz	SHORT VR_Exit			;   Y: Do nothing
	push	edx
	push	esi
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Restore_Event
	VMMCall Schedule_VM_Event
Queue_Out "Rst_event=#EAX",ESI
	mov	[VDD_Rest_Event_Handle],esi
	pop	esi
	pop	edx
VR_Exit:
	ret
EndProc VDD_Restore


;******************************************************************************
;VDD_Cancel_Restore
;
;DESCRIPTION:
;	This routine cancels the restore event for the focus VM.
;
;ENTRY: EBX = VM handle that was attached to 2nd EGA
;	EDI = VM VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Cancel_Restore,PUBLIC
IFDEF	DEBUG
	cmp	ebx,[VDD_Focus_VM]
	je	SHORT VCR_D00
Debug_Out "Cancel restore on wrong VM handle"
VCR_D00:
ENDIF
	push	esi
        test    [VDD_Rest_Event_Handle],-1
        jz      SHORT VCR_NoEventPending

        xor     esi,esi
        xchg    esi,[VDD_Rest_Event_Handle]
Queue_Out "Cancel=#EAX",ESI
	VMMCall Cancel_VM_Event 		; Cancel restore of attached VM
VCR_NoEventPending:
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

	mov	edi,edx
	mov	[VDD_Rest_Event_Handle],0
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: Just controller state?
	jz	VRE_04  			;   Y: 

IFDEF	DEBUG
	test	[vgVDD.Vid_VM_Handle],-1
	jz	SHORT VRE_D01
;Debug_Out   "VDD: Restore when another VM attached to display!!"
VRE_D01:
ENDIF
	mov	[vgVDD.Vid_VM_Handle],ebx	; Set new handle
	call	VDD_Scrn_Off			; Turn off the CRT
	call	VDD_IO_SetTrap			; Set up I/O trapping
	call	VDD_Get_Mode			; AL = mode
	call	VDD_Mem_Physical		; Remap physical memory
	call	VDD_RestCRTC			; Restore display (CRTC)
	mov	esi,[VDD_PhysB8000]		; EDI = start of VRAM
        call    VDD_Get_Mode                    ; EAX = Mode
	VMMCall Test_Sys_VM_Handle		;Q: SYS VM?
	jnz	SHORT VRE_00			;   N: Restore the memory
	call	VDD_Clear_RAM			;   Y: Just clear the memory
	jmp	SHORT VRE_02
VRE_00:
	call	VDD_Restore_RAM

VRE_02:
	and	[vgVDD.Vid_Flags],NOT fVid_DOff ; Cancel global display off
	call	VDD_RestCtlr			; Restore mem access, indices
	mov	[edi.VDD_DirtyPages],0		; Clear pages mod'd

        VMMCall Test_Sys_VM_Handle
        jnz     SHORT VRE_SkipINT2f
; Tell app no longer in background

	Push_Client_State
	VMMcall Begin_Nest_Exec
	mov	[ebp.Client_AX], 4002h
	mov	eax,2Fh
	VMMcall Exec_Int			; VM reinitializes display
	VMMcall End_Nest_Exec
	Pop_Client_State
VRE_SkipINT2f:
	ret
VRE_04:
; Restore just the registers modified by background VMs
	call	VDD_RestCtlr			; Restore mem access, indices
        ret
EndProc VDD_Restore_Event

IF 0
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
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: Attached VM?
	jnz	SHORT VG_WindGrab		;   N: Windowed grab
IFDEF	DEBUG
	VMMCall Test_Cur_VM_Handle
	jz	SHORT VG_D00
Debug_Out "Not grabbing current VM!!???!"
VG_D00:
ENDIF
	call	VDD_Get_Mode
	lea	esi,[edi.VDD_CPg]
	mov	[esi.VPH_Mode],al		; Save for VMDOSAPP
	lea	edx,[edi.VDD_Stt]
	call	VDD_Scrn_Size			; Calculate screen size
	movzx	eax,[esi.VPH_Mode]
        call	VDD_SRAM			
	jc	SHORT VG_NoGrab
	call	VDD_State_Update		; save ctrlr state in copy
	call	VDD_RestCtlr			; Restore running VM's state
	jmp	VDD_VMDA_Grab			; Tell VMDA about grab
VG_NoGrab:
	call	VDD_RestCtlr			; Restore running VM's state
	ret
VG_WindGrab:
IF2
%OUT	Do anything more for a Window grab?
ENDIF
	call	VDD_Get_Mode
	lea	esi,[edi.VDD_CPg]
	mov	[esi.VPH_Mode],al		; Save for VMDOSAPP
	jmp	VDD_VMDA_Grab			; Tell VMDA about grab
EndProc VDD_Grab

ENDIF

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
	cmp	ebx,[vgVDD.Vid_VM_Handle]    ; Q: Attached VM?
	jnz	SHORT VG_WindGrab	       ;   N: Windowed grab
	call	VDD_Get_Mode
        cmp     eax,03
        ja      SHORT VG_FSGrfx

VG_FSText:
        call    VDD_GrabTextMode
	jc	SHORT VG_Exit
	call	VDD_State_Update		; save ctrlr state in copy
	call	VDD_VMDA_Grab			; Tell VMDA about grab
        ret
; Graphics mode grab
VG_FSGrfx:
	call	VDD_Get_Mode
	lea	esi,[edi.VDD_CPg]
	mov	[esi.VPH_Mode],al		; Save for VMDOSAPP
	lea	edx,[edi.VDD_Stt]
	call	VDD_Scrn_Size			; Calculate screen size
	movzx	eax,[esi.VPH_Mode]
	clc
        call	VDD_SRAM			
	jc	SHORT VG_NoGrab
	call	VDD_State_Update		; save ctrlr state in copy
	call	VDD_RestCtlr			; Restore running VM's state
	jmp	VDD_VMDA_Grab			; Tell VMDA about grab
VG_NoGrab:
	call	VDD_RestCtlr			; Restore running VM's state
VG_Exit:
	ret
; Grab of non-attached VM, copy memory and video state
VG_WindGrab:
Debug_Out "Windowed GRAB - should not get this - GRAB IGNORED!!"
	ret
EndProc Vdd_Grab

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

	lea	edx,[edi.VDD_Stt]
	lea	esi,[edi.VDD_Pg]
        call    VDD_Mem_UpdOff
	movzx	eax,[esi.VPH_Mode]
	mov	[edi.VDD_CPg.VPH_Mode],al

	xor	edx,edx
	call	VDD_SetCopyMask 		; ECX = Mask for copy pages
	mov	[edi.VDD_CPg.VPH_PgAccMsk],ecx	
IFDEF	DEBUG
;Trace_Out   "VDD: Full Screen VM, Mode = #AX, Alloc mask = #ECX"
ENDIF
        push    ecx
        push    edx
	call	VDD_Mem_ACopy			; Allocate copy mem
        pop     edx
        pop     ecx
	jc	SHORT VGT_NoGrab		; No memory, exit
;	call	VDD_Scrn_Off
        mov     eax,ecx
	call	VDD_CopyTextPages
;
; [esi.VPHMode] may be trashed by previous two calls - restore it.
;
	call	VDD_Get_Mode
	mov	[edi.VDD_CPg.VPH_Mode],al		; Save for VMDOSAPP
;	and	[Vid_Flags],NOT fVid_DOff	; Cancel global display off
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

	lea	edx,[edi.VDD_Stt]		; EDX = CRTC ptr
	lea	esi,[edi.VDD_CPg]		; ESI = VPH for copy pages

; Copy page at a time according to page mask in EAX
	push	ebx
	mov	ebx,edi 			; EBX = VDD CB ptr
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
VCTP_01:
	shr	eax,1
	jnc	SHORT VCTP_03

	push	esi
	push	edi

; ESI = address of text mode memory
	shl	esi,12
	add	esi,[VDD_PhysB8000]
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
        cld
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
;VDD_SRAM
;
;DESCRIPTION:
;	Save physical Video RAM in main memory.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;	EAX = video mode
;
;EXIT:	
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

	jnc	SHORT VSRAM_Grab
	call	VDD_Mem_Chg			; Q: Pages changed?
	jz	SHORT VSRAM_Exit		;   N: No save
	mov	ecx,eax 			; ECX = changed memory mask
	push	ebx
	mov	eax,[edi.VDD_Pg.VPH_PgAllMsk]
	not	eax
	and	[edi.VDD_DirtyPages],eax	; Clear mask of mod'd dirty
						;	pages we will save
	lea	ebx,[edi.VDD_Pg]		; EBX = VPH struc to save into
	mov	esi,[VDD_PhysB8000]
	call	VDD_Save_RAM
	pop	ebx
        jmp     SHORT VSRAM_Exit
; Set up RAM for grab
VSRAM_Grab:
	mov	ecx,VDD_Grb_MskTab[eax*4]	
	mov	[edi.VDD_CPg.VPH_PgAccMsk],ecx	; Alloc just enough
IFDEF	DEBUG
;Trace_Out   "VDD:Grab RAM, Mode = #AX, Alloc mask = #ECX"
ENDIF
	push	ebx
	push	ecx
	call	VDD_Mem_ACopy			; Allocate copy mem
	jc	SHORT VSRAM_NoGrab		; No memory, exit
	pop	ecx				; ECX,EDX = save mask
	lea	ebx,[edi.VDD_CPg]		; EBX = where to save it
	mov	ch,cl				;   Y: Save even plane also
	jmp	SHORT VSRAM_Save
; Not enough grab memory, return error
VSRAM_NoGrab:
	add	esp,8
	stc
	jmp	SHORT VSRAM_Quit

VSRAM_Save:
;;;Debug_Out "VDD: Save #ECX,#EDX at #ESI into #EBX"
	mov	esi,[VDD_PhysB8000]
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

EndProc VDD_SRAM

;******************************************************************************
;VDD_Save_RAM	    Save video RAM
;
;ENTRY: EBX = VPH structure pointer
;	EDI = VDD control block data ptr
;	ESI = Video RAM address
;	ECX = bit map of pages to save
;	DS, ES = valid segment selectors
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
;==============================================================================
BeginProc VDD_Save_RAM

	push	ebp
	push	esi
	push	edi
	mov	ebp,ecx
        call    VDD_Get_Mode                    ; EAX = Mode
	mov	[ebx.VPH_Mode],al		; Mem will be this mode
	and	ebp,[ebx.VPH_PgAllMsk]		; Only save pages that exist
	cld
	xor	eax,eax 			; Start with page 0
srer2:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT srer5			;   N: go do next
srer3:
	push	eax
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT srer_dbg0
Debug_Out   "Save to unallocated memory page"
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
	pop	edi
	pop	esi
	pop	ebp
	ret
EndProc VDD_Save_RAM

;******************************************************************************
;VDD_Clear_RAM	  Clear EGA video RAM
;
;DESCRIPTION:
;	Clears the physical VRAM. The application has indicated that it
;	can save/restore its entire state. We clear the RAM for esthetics.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD control block data ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
;==============================================================================
BeginProc VDD_Clear_RAM, PUBLIC

	push	edi
	mov	ecx,(CGA_Pages)*4096		; bytes to clear for CGA
	cmp	vgVDD.Vid_Type,Vid_Type_ATT	; Q: Is it ATT/Olivetti?
	jz	SHORT VCCR_400Line		;   Y: clear 32k
	test	[edi.VDD_Stt.I_EMSel],fEMSelEMod ; Q: IDC in extended mode?
	jz	SHORT VCCR_Ok			;   N: bytes correct
VCCR_400Line:
	mov	ecx,(IDC_Pages)*4096		; Clear 32k
VCCR_Ok:
	mov	edi,[VDD_PhysB8000]
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
;
;EXIT:
;
;USES:	Flags, EAX, ECX, EDX, ESI, EDI, EBP
;
;==============================================================================
BeginProc VDD_Restore_RAM, PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ebx
	push	ebp
	push	esi
	push	edi
	lea	ebx,[edi.VDD_Pg]		; EBX = VPH struc ptr
	mov	ebp,[ebx.VPH_PgAllMsk]
	test	[edi.VDD_Stt.I_EMSel],fEMSelEMod ; Q: IDC in extended mode?
        jnz     SHORT vrer0                     ;  Y: copy 32K
        and     ebp,0Fh                         ;  N: copy only 16K
vrer0:
	cld
	xor	eax,eax 			; Start page 0
	mov	edi,esi 			; EDI = addr of physical mem
vrer1:
	shr	ebp,1				; Q: This page modified?
	jnc	SHORT vrer3			;   N: go do next
        push    eax
	mov	al,[ebx.VPH_MState.VDA_Mem_PgMap][eax]
						; EAX = page number desired
IFDEF	DEBUG
	test	al,al
	jns	SHORT vrer_dbg1
Debug_Out   "Restore from unallocated memory page"
vrer_dbg1:
ENDIF
	shl	eax,12				; EAX = linear offset to page
	add	eax,[ebx.VPH_MState.VDA_Mem_Addr]
	mov	esi,eax 			; ESI = source
	mov	ecx,1000h			; ECX = count of bytes
	rep movsb				; byte reps! - lets DMA work
	sub	edi,1000h
	pop	eax
vrer3:
	add	edi,1000h			; EDI = next page address
	inc	al
	cmp	al,8				; Q: done with all pages?
	jb	vrer1				;   N: go do next
	pop	edi
	pop	esi
	pop	ebp
	pop	ebx
	ret
EndProc VDD_Restore_RAM

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

Assert_VDD_ptrs ebx,edi
	cld
	cmp	[vgVDD.Vid_Type],Vid_Type_IDC	; Q: Is it IDC
	jne	SHORT VRC_NotIDC		;   N: skip IDC specific restore

	push	edi
	movzx	ecx,[VDD_EMode_Table]		; Count of ports
push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
lea	esi,[ebx.I_MMode]               ; First saved value
pop     ebx
;	lea	esi,[edi.VDD_Stt.I_MMode]	; First saved value
	mov	edi,OFFSET32 VDD_EMode_Table+2	; Table of ports
VRC_Loop1:
	xchg	esi,edi 			; Ports Table into ESI
	lodsw					; load port address
	mov	edx,eax 			; save address
	xchg	esi,edi 			; Value Table into ESI
	lodsb					; load port value
	out	dx,al				; set value
	loop	VRC_Loop1			; Do another port
	pop	edi

VRC_NotIDC:
	mov	edx,3D4h			; DX = port addr 6845 index reg
	xor	ax,ax				; AH = data reg number
	mov	ecx,16				; ECX = register count
push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
lea	esi,[ebx.V_HTotal]              ; ESI = addr of first reg
pop     ebx
;	lea	esi,[edi.VDD_Stt.V_HTotal]	; ESI = addr of first reg
VRC_Loop2:
	mov	ah,[esi]			; data in ah
	out	dx,ax				; set index,data
	inc	esi				; next source address
	inc	al				; next index in al
	loop	VRC_Loop2			; next reg

push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
mov	esi,ebx                         ; ESI = ptr to CRT State struc
pop     ebx
;        mov	al,[edi.VDD_Stt.V_Indx]
mov	al,[esi.V_Indx]
	out	dx,al				; Set index register

	mov	dl,(3DEh AND 0FFh)
	cmp	vgVDD.Vid_Type,Vid_Type_ATT	; Q: Is it ATT/Olivetti?
	jne	short VRC_NotATT		;   N: skip ATT reg restore
	test	[vgVDD.Vid_Flags],fVid_PVC	; Q: Is it Olivetti PVC?
	jz	short VRC_NotPVC		;   N: skip 3DF restore
;	mov	al,[edi.VDD_Stt.V_Mode2]
mov	al,[esi.V_Mode2]
	or	al,80h				; Enable Mode 3 restore
	out	dx,al
;	mov	al,[edi.VDD_Stt.V_Mode3]
mov	al,[esi.V_Mode3]
	inc	edx
	out	dx,al				; Restore ATT mode3 reg
	dec	edx

VRC_NotPVC:
;	mov	al,[edi.VDD_Stt.V_Mode2]
mov	al,[esi.V_Mode2]
	out	dx,al				; Restore ATT mode2 reg

VRC_NotATT:
	mov	dl,(03D9h AND 0FFh)		; CGA Color Reg
;	mov	al,[edi.VDD_Stt.V_Colr]
mov	al,[esi.V_Colr]
	out	dx,al				; set color mode
	ret
EndProc VDD_RestCRTC


;******************************************************************************
;VDD_RestCtlr
;
;DESCRIPTION:
;	Restores the state of the VM's memory addressing state and video
;	on/off state. Note that for CGA, there is nothing to do to restore
;	the memory addressing and so this does nothing but restore the
;	video on/off state.
;
;ENTRY: EBX = VM_Handle of VM to restore to CGA
;	EDI = VDD control block data ptr
;
;EXIT:	
;
;USES:	Flags, EAX, ECX, EDX
;
;==============================================================================
BeginProc VDD_RestCtlr

Assert_VDD_ptrs ebx,edi
	test	[vgVDD.Vid_Flags], fVid_DOff	; Q: Video off?
	jnz	SHORT VDD_Scrn_Off		;   Y: turn off again
    push    ebx
    mov     ebx,[vgVDD.Vid_VM_Handle]
    or	    ebx,ebx
    jnz     SHORT VRC_001
    mov     ebx,[VDD_Focus_VM]
VRC_001:
    add     ebx,[VDD_CB_Off]
    call    VDD_GetSttPtr		    ; EBX = ptr to CRT State struc
    mov     al,[ebx.V_Mode]
    pop     ebx
;	mov	al,[edi.VDD_Stt.V_Mode]
	mov	edx,03D8h
	out	dx,al				; Turn on video
	ret
EndProc VDD_RestCtlr

;******************************************************************************
;VDD_Scrn_Off
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
	push	eax
	push	ebx
	push	edx
	mov	ebx,[vgVDD.Vid_VM_Handle]
	or	ebx,ebx
	jnz	SHORT VSO_001
	mov	ebx,[VDD_Focus_VM]
VSO_001:
	add	ebx,[VDD_CB_Off]
	call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
	mov	al,[ebx.V_Mode]
	mov	edx,03D8h
	and	al,NOT fModVidEna		; Turn off video
	out	dx,al
	or	[vgVDD.Vid_Flags], fVid_DOff	; Video off
	pop	edx
	pop	ebx
	pop	eax
	ret
EndProc VDD_Scrn_Off


;******************************************************************************
;VDD_SaveRegs
;
;DESCRIPTION:
;	This routine is called to update any registers in the controller
;	data structure that are not correct for the VM running in the
;	CGA.
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

        push    edx
	test	[edi.VDD_Flags],fVDD_ModeSet	; Q: Doing mode set?
	jnz	SHORT VSR_Exit			;   N: trapping all ports
	test	[edi.VDD_PIF],fVidNoTrpTxt	; Q: No trapping enabled?
	jz	SHORT VSR_Exit			;   N: No registers to read
	call	VDD_Get_Mode
	cmp	al,3		                ; Q: Text mode?
	ja	SHORT VSR_Exit			;   N: No registers to read
; must read cursor registers
	mov	edx,3d4h
	mov	ecx,V_CAddrL-V_CAddrH+1
	lea	esi,[edi.VDD_Stt.V_CAddrH]
	xchg	esi,edi
	mov	ah, V_CAddrH-V_HTotal
        cld
VSR_02:
	mov	al,ah
	out	dx,al
	inc	edx
	in	al,dx
	stosb
	dec	edx
	inc	ah
	loopd	VSR_02
	xchg	esi,edi
VSR_Exit:
        pop     edx
	clc					; Did not save latches
	ret
EndProc VDD_SaveRegs


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
	push	edi
	lea	esi,[edi.VDD_Stt]
	mov	edi,OFFSET32 VDD_MsgStt
	mov	ecx,(SIZE CGA_State_struc)/4
.ERRNZ (SIZE CGA_State_struc) MOD 4
        cld
	rep movsd				; Save adapter regs
	pop	edi
;	call	VDD_MsgSaveFont 		; Save the font
	and	[vgVDD.Vid_Flags],NOT fVid_DOff ; Cancel global display off
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

	or	[vgVDD.Vid_Flags],fVid_MsgA	; Set message mode Init
	mov	[VDD_Msg_VM],ebx		; This is the message VM
	call	VDD_Scrn_Off			; Turn off the CRT
	call	VDD_MsgRestFontAndClear 	; Restore the font and clear mem
	call	VDD_RestCRTC			; Set display for msg mode
;        call    VDD_Global_Restore
	and	[vgVDD.Vid_Flags],NOT fVid_DOff ; Cancel global display off
	call	VDD_RestCtlr			; Restore mem access, indices
; When in message state, no VM is really attached or running in adapter
	mov	[vgVDD.Vid_VM_Handle],0
	ret
EndProc VDD_RestMsgStt

;******************************************************************************
;VDD_GetSttPtr
;
;DESCRIPTION:
;	Gets pointer to VDD status, either VM's VDD_Stt or we are in message
;	mode and use a special buffer.
;
;ENTRY: [vgVDD.Vid_VM_Handle] = VM with CRT
;	EBX = VDD ptr of VM being saved/restored
;
;EXIT:	EBX = VDD_Stt structure pointer
;
;USES:	Flags, EAX, EDX
;
;==============================================================================
BeginProc VDD_GetSttPtr
	lea	ebx,[ebx.VDD_Stt]
	test	[VgVDD.Vid_Flags],fVid_Msg
	jz	SHORT VPSR_02
	mov	ebx,OFFSET32 VDD_MsgStt
VPSR_02:
	ret
EndProc VDD_GetSttPtr

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
;	mov	edx,pSeqIndx
;	cld
;	mov	al,2
;	out	dx,al
; 	inc	dl
;	IO_Delay
;	mov	al,4
;	out	dx,al
;	mov	esi,OFFSET32 VDD_MsgFont
;	mov	edi,[VDD_PhysA0000]
;	mov	ecx,FontLen/4
;	rep	movsd
;
;; Initialize memory to white blanks
;	mov	al,3
;	out	dx,al
	mov	edi,[VDD_PhysB8000]
	mov	eax, 07200720h
	mov	ecx,25*80/4*2
        cld
	rep stosd

	pop	edi
	ret
EndProc VDD_MsgRestFontAndClear

VxD_CODE_ENDS

	END

