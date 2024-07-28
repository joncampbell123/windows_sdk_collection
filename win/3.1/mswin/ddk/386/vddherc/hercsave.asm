       title   VDD - Virtual Display Device for HERC version 3.00
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
	INCLUDE HERC.INC
	INCLUDE DEBUG.INC
	INCLUDE VDD.INC

;******************************************************************************

;******************************************************************************
;
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_Focus_VM:DWORD
	EXTRN	VDD_Msg_VM:DWORD
Public VDD_Grb_MskTab
; Mask for memory to save for a grab
VDD_Grb_MskTab	LABEL DWORD
	DD	000000001h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000001h		; mode 1, 48K RAM FONT mode
	DD	0000000FFh		; mode 2, graphics mode

PUBLIC VDD_MsgStt	
VDD_MsgStt	    DB SIZE HERC_State_struc DUP (?)

PUBLIC	VDD_Rest_Event_Handle
VDD_Rest_Event_Handle DD  ?

; Save/restore addresses initialized by Sys_Critical_Init
PUBLIC	VDD_PhysB0000
VDD_PhysB0000	    DD	?		; Address of physical mem at B0000

VxD_DATA_ENDS

VxD_CODE_SEG
	EXTRN	VDD_Get_Mode:NEAR
        EXTRN   VDD_Mem_UpdOff:NEAR
	EXTRN	VDD_Mem_Chg:NEAR
	EXTRN	VDD_Mem_Null:NEAR
	EXTRN	VDD_Mem_Physical:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_VMDA_Grab:NEAR
	EXTRN	VDD_Mem_ACopy:NEAR
	EXTRN	VDD_IO_SetTrap:NEAR

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

; Tell VM switching to background
        VMMCall Test_Sys_VM_Handle
        jnz     SHORT VS_00
        call    VDD_Mem_Null
	mov	edx,edi
	mov	esi,OFFSET32 VDD_Backgrnd_Event
	mov	eax,VDD_Pri_Device_Boost
	mov	ecx,0
	VMMCall Call_Priority_VM_Event		; Tell VM it is losing screen

VS_00:        
; Save video state
	call	VDD_Scrn_Off			; Turn off screen
	call	VDD_Get_Mode			; VDD_Mode = mode
	mov	[edi.VDD_CPg.VPH_Mode],al
	stc
	call	VDD_SRAM			; HERC mode save
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
	call	VDD_Restore			; Make sure focus restored
	pop	edi
	pop	ebx

VEBD_00:
	call	VDD_Mem_Chg
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

	xor	esi,esi
	xchg	esi,[VDD_Rest_Event_Handle]
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
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: Just the controller state?
        jz	  VRE_04			  ;   Y: Do ctlr state restore

IFDEF	DEBUG
	test	[vgVDD.Vid_VM_Handle],-1
	jz	SHORT VRE_D01
;Debug_Out   "VDD: Restore when another VM attached to display!!"
VRE_D01:
ENDIF
	mov	[vgVDD.Vid_VM_Handle],ebx	; Set new handle
	call	VDD_Scrn_Off			; Turn off the CRT
        mov     al,1
	call	VDD_IO_SetTrap			; Set up I/O trapping
	call	VDD_Get_Mode			; AL = mode
	call	VDD_Mem_Physical		; Remap physical memory
	call	VDD_RestCRTC			; Restore display (CRTC)
	mov	esi,[VDD_PhysB0000]		; EDI = start of VRAM
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
        call    VDD_Mem_UpdOff
	call	VDD_Get_Mode
	lea	esi,[edi.VDD_CPg]
	mov	[esi.VPH_Mode],al		; Save for VMDOSAPP
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
	mov	esi,[VDD_PhysB0000]
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
        mov     esi,[VDD_PhysB0000]
	test	[edi.VDD_Stt.V_Mode], fModPage1 ;Q: graphics page 1?
        jz      SHORT VSRAM_VidAtB000
        add     esi,8000h               ; 32K offset in video mem
VSRAM_VidAtB000:
	call	VDD_Save_RAM
	pop	ebx
	jmp	SHORT VSRAM_Exit
; Not enough grab memory, return error
VSRAM_NoGrab:
	add	esp,8
	stc
	jmp	SHORT VSRAM_Quit

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

	mov	al,[edi.VDD_Mode]
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
	cmp	al,16				; Q: done with all pages?
	jb	SHORT srer2			;   N: go do next
	pop	edi
	pop	esi
	pop	ebp
	ret
EndProc VDD_Save_RAM

;******************************************************************************
;VDD_Clear_RAM	  Clear HERC video RAM
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
	mov	ecx,(HERC_Pages)*4096	; bytes to clear for HERC
	mov	edi,[VDD_PhysB0000]
	xor	eax,eax
        cld
	rep stosb			; Bytes so DMA can function
	pop	edi
	ret
EndProc VDD_Clear_RAM

;******************************************************************************
;VDD_Restore_RAM    Restore HERC video RAM
;
;DESCRIPTION:
;	Restores physical VRAM from VDD_Pg. Note that we don't care
;	that this sets the access bit since it is at a different PTE than
;	the one associated with the VM
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
	mov	al, [edi.VDD_Mode]
	mov	ebp, 1
	cmp	al, HERC_MODE_Text		;Q: text mode?
	je	short vrer1			;   Y: restore 1 page
	mov	ebp, 1Fh
	test	al, HERC_MODE_4KText		;Q: 4K Ram font mode?
	jnz	short vrer1			;   Y: restore 5 pages
	mov	ebp, [ebx.VPH_PgAllMsk] 	;   N: restore all pages for
						;      graphics or 48K Ram font
vrer1:
	cld
	xor	eax,eax 			; Start page 0
	mov	edi,esi 			; EDI = addr of physical mem
vrer2:
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
	cmp	al,16				; Q: done with all pages?
	jb	vrer2				;   N: go do next
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
;	Restore physical CRTC controller.
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
	mov	edx,pIndx6845Mono	; DX = port addr 6845 index reg
	xor	ax,ax			; AH = data reg number
	mov	ecx,16			; ECX = register count
push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
lea	esi,[ebx.V_HTotal]              ; ESI = addr of first reg
pop     ebx
;lea	esi,[edi.VDD_Stt.V_HTotal]; ESI = addr of first reg
VRC_Loop1:
	mov	ah,[esi]		; data in ah
	out	dx,ax			; set index,data
	inc	esi			; next source address
	inc	al			; next index in al
	loop	VRC_Loop1		; next reg

	cmp	[vgVDD.Vid_Type],Vid_Type_HERC112
	jne	short VCR_Not112

push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
lea	esi,[ebx.V_xMode]                         ; ESI = ptr to CRT State struc
pop     ebx
;lea	esi,[edi.VDD_Stt.V_xMode]; EBX = addr of first reg
	mov	al,20			; AL = data reg number
	mov	ecx,3			; ECX = register count
VCR_Loop2:
	mov	ah,[esi]		; data in ah
	out	dx,ax			; set index,data
	inc	esi			; next source address
	inc	al			; next index in al
	loop	VCR_Loop2		; next reg

VCR_Not112:
	mov	dx,pModeMono		;Mode register.
push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
mov	al,[ebx.V_Mode]                        ; ESI = ptr to CRT State struc
pop     ebx
;mov	al,[edi.VDD_Stt.V_Mode]
	out	dx,al			; Turn on video
	ret
EndProc VDD_RestCRTC


;******************************************************************************
;VDD_RestCtlr
;
;DESCRIPTION:
;	Restores the state of the VM's memory addressing state and video
;	on/off state. Note that for HERC, there is nothing to do to restore
;	the memory addressing and so this does nothing but restore the
;	video on/off state.
;
;ENTRY: EBX = VM_Handle of VM to restore to HERC
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
	mov	edx,pModeMono
push    ebx
mov     ebx,edi
call	VDD_GetSttPtr			; EBX = ptr to CRT State struc
mov	al,[ebx.V_Mode]                         ; ESI = ptr to CRT State struc
pop     ebx
;mov	al,[edi.VDD_Stt.V_Mode]
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
	mov	edx,pModeMono
	add	ebx,[VDD_CB_Off]
	mov	al,[ebx.VDD_Stt.V_Mode]
	and	al,NOT fModVidEna		; Turn off video
	out	dx,al
	or	[vgVDD.Vid_Flags], fVid_DOff	; Video off
	pop	edx
	pop	ebx
	pop	eax
	ret
EndProc VDD_Scrn_Off

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
	mov	ecx,(SIZE HERC_State_struc)/4
.ERRNZ (SIZE HERC_State_struc) MOD 4
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
	mov	edi,[VDD_PhysB0000]
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
