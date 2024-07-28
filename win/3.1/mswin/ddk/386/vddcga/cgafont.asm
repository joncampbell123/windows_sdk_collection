       title   VDD - Virtual Display Device for CGA version 3.00
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: NM, MDW
;
;   (C) Copyright Microsoft Corp. 1987-1989
;   (C) Copyright Compaq Computer Corp. 1987-1989
;
;   July, 1987
;
;DESCRIPTION:
;	This module includes support routines used exclusively with
;	the Compaq Computer Corp. Plasma Display found on Portable III
;	style units.  These units have a CGA superset controller called
;	the Integrated Display Controller (IDC).  It supports user
;	fonts in text mode and 640x400 resolution in graphics mode.
;	The routines included here save, and restore the font memory,
;	allocate and deallocate font ram copy memory, and map and unmap
;	the font copy ram into the video address space for background
;	or window VM's.
;
;	Note the font support provided at present permits full screen
;	old applications to use the programmable font feature if there
;	is a real IDC in the system.  This may be of limited value
;	without support for extended text modes such as 80x40 text mode.
;	It will however allow operations such as "mode" which reload
;	the font ram to do so without bothering the display ram.
;	The old applications are isolated from each other so that one
;	app loading a font does not affect another.  If the application
;	runs in a window, font operations will be captured but text will
;	be displayed in the default windows font.  If the application
;	is switched to a full screen, the font operations performed
;	while in a window will be effective.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC

	INCLUDE VDD.INC
	INCLUDE CGA.INC
	INCLUDE DEBUG.INC

;******************************************************************************

VxD_DATA_SEG
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysB8000:DWORD
VxD_DATA_ENDS

VxD_CODE_SEG
	EXTRN	VDD_Mem_Virtual:NEAR


;******************************************************************************
;VDD_Font_Alloc
;
;DESCRIPTION:
;	Allocate two pages of RAM for IDC Font Copy used to save,
;	restore, and emulate the IDC Programmable Font Feature.
;	Sufficient space to hold the font copy pages has been reserved
;	in the copy memory pseudo page table for these pages.
;	Note these pages do not appear in VDD_Cnt.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1 Indicates unable to allocate memory
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Alloc

	pushad
	test	[edi.VDD_hFont],-1
	jnz	SHORT VFAM_Done
	xor	ecx,ecx
	VMMCall _PageAllocate,<IDC_FPages,PG_HOOKED,ebx,ecx,ecx,ecx,ecx,PageZeroInit+PageLocked>
	or	edx,edx
	jnz	SHORT VFAM_Good
	or	[edi.VDD_EFlags],fVDE_NoMain
	popad
	stc
	ret
VFAM_Good:
	mov	[edi.VDD_hFont],eax		    ; Save memory handle
	mov	[edi.VDD_AddrFont],edx		    ; Save memory address
	VMMCall Test_Sys_VM_Handle
	jnz	SHORT VFAM_Copy 		    ; Copy font from VM1
	call	VDD_Font_Save			    ; Save from hardware
	jmp	SHORT VFAM_Done
; Copy VM1's saved font
VFAM_Copy:
	VMMCall Get_Sys_VM_Handle
	add	ebx,[VDD_CB_Off]
	mov	esi,[ebx.VDD_AddrFont]
	mov	edi,[edi.VDD_AddrFont]
	mov	ecx,(4096*IDC_FPages)/4 	    ; bytes of font RAM
	cld
	rep movsd				    ; copy from Sys VM
VFAM_Done:
	popad
	clc
	ret
EndProc VDD_Font_Alloc

;******************************************************************************
;VDD_Font_Dealloc
;
;DESCRIPTION:
;	Free the 2 pages of IDC Font Copy RAM
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Dealloc

	xor	eax,eax
	mov	[edi.VDD_AddrFont],eax
	xchg	eax,[edi.VDD_hFont]
	or	eax,eax
	jz	SHORT VDFM_Ex
	VMMCall _PageFree,<eax,0>
VDFM_Ex:
	ret
EndProc VDD_Font_Dealloc

;******************************************************************************
;VDD_Font_Save
;
;DESCRIPTION:
;	Copy the real IDC Font RAM to the IDC Font Copy RAM
;	Called only after VDD_Save
;	Video is already off.  (from SetFocus)
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Save

	push	ecx
	push	edx
	push	esi
	push	edi

	mov	dx,023C6h			    ; Extended Mode Select Reg
	mov	al,[edi].VDD_Stt.I_EMSel	    ; Get last mode used
	or	al,fEMSelFont			    ; make sure font selected
	out	dx,al				    ; WRITE DESIRED VALUE
	mov	edi,[edi.VDD_AddrFont]
	mov	esi,[VDD_PhysB8000]		    ; IDC RAM in virtual space
	mov	ecx,4096*IDC_FPages		    ; bytes of font RAM
	cld
	rep movsb				    ; copy out of IDC

	pop	edi
	call	VDD_Video_Mode			    ; Reset IDC Font Mode
	pop	esi
	pop	edx
	pop	ecx
	ret
EndProc VDD_Font_Save

;******************************************************************************
;VDD_Font_Restore
;
;DESCRIPTION:
;	Copy the IDC Font Copy RAM to the real IDC Font RAM
;	Called before VDD_Restore
;	Video is already off. (from SetFocus)
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Restore

	push	ecx
	push	esi
	push	edi

; Put IDC into Font Access Mode
	mov	dx,023C6h			    ; Extended Mode Select Reg
	mov	al,[edi].VDD_Stt.I_EMSel	    ; Get last mode used
	or	al,fEMSelFont			    ; make sure font selected
	out	dx,al				    ; WRITE DESIRED VALUE

; Copy the Font to the hardware
	mov	esi,[edi.VDD_AddrFont]
	mov	edi,[VDD_PhysB8000]		    ; IDC RAM in virtual space
	cld
	rep movsb				    ; copy into IDC

; Put IDC into Video Access Mode
	pop	edi
	call	VDD_Video_Mode			    ; Reset IDC Font Mode
	pop	esi
	pop	ecx
	ret
EndProc VDD_Font_Restore

;******************************************************************************
;VDD_Video_Mode
;
;DESCRIPTION:
;	Put IDC into Video Access Mode
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Video_Mode

	push	eax
	push	edx
	mov	dx,023C6h			; Extended Mode Select Reg
	mov	al,[edi.VDD_Stt.I_EMSel]	    ; Get last mode used
	and	al,not fEMSelFont		    ; font not selected
	out	dx,al				    ; WRITE NEW VALUE
	pop	edx
	pop	eax
	ret
EndProc VDD_Video_Mode

;******************************************************************************
;VDD_Restore_Mode	Reset IDC Video/Font Mode
;
;DESCRIPTION:
;	Turn off video or on as last set by application
;	For IDC Set Video RAM or Font RAM Access as last set by application
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;USES:	
;
;ASSUMES:
;	IDC in CGA Mode
;
;==============================================================================
BeginProc VDD_Restore_Mode

	push	eax
	push	edx
	mov	dx,023C6h			    ; Extended Mode Select Reg
	mov	al,[edi.VDD_Stt.I_EMSel]	    ; Get last mode used
	out	dx,al
	pop	edx
	pop	eax
	ret
EndProc VDD_Restore_Mode

;******************************************************************************
;VDD_Font_Access
;
;DESCRIPTION:
;	Make the IDC Font Copy RAM appear in the IDC RAM virtual address
;	(instead of the Display Copy RAM).  This allows the tracking of
;	Font RAM changes done by a Window VM.  This emulates the way
;	the IDC Hardware allows access to the Font RAM.
;	This function will be requested from the I/O Port trapping
;	function at the time of a write to the mode register for a VM
;	that is not the current display owner.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Access

	push	eax
	push	ecx
	push	edx
	VMMCall _MapIntoV86,<[edi.VDD_hFont],ebx,0B8h,IDC_FPages,0,0>
	pop	edx
	pop	ecx
	pop	eax
	ret
EndProc VDD_Font_Access

;******************************************************************************
;VDD_Font_Unaccess
;
;DESCRIPTION:
;	Make the IDC Display Copy RAM appear in the IDC RAM virtual
;	address (instead of the Font Copy RAM).  This is the inverse
;	function to VDD_Font_Access.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_Font_Unaccess

	push	eax
	push	ecx
	push	edx
	call	VDD_Mem_Virtual
	pop	edx
	pop	ecx
	pop	eax
	ret
EndProc VDD_Font_Unaccess

VxD_CODE_ENDS
	    end
