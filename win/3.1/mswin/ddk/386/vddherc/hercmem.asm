       title   VDD - Virtual Display Device for HERC version 0.0 5/28/87
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;DESCRIPTION:
;	    This module handles all of the manipulation of a VM's video memory
;	space. This includes mapping of physical device, mapping normal RAM for
;	emulation of physical device and null pages.  The pages are disabled,
;	hooked pages when they have not been allocated to the VM and when the
;	mode of the controller has changed. VDD_Mem_Init initializes a VM's
;	page table for video memory. VDD_Mem_AMain, VDD_Mem_ACopy,
;	VDD_Mem_DMain and VDD_Mem_DCopy handle allocation and deallocation of
;	memory handles. VDD_Mem_NewPage adds to that set. VDD_Mem_Remap handles
;	the changes due to a mode change. VDD_Mem_Null and VDD_Mem_Disable
;	handle the transition states. Note that VDD_Mem_Null
;	is for VM1 running without the screen. VDD_Mem_Physical maps the
;	physical display.
;	    Additional routines are used to determine state and keep track of
;	changes to the video memory(i.e. keep track of hardware detection of
;	changes).  VDD_Mem_ChkPag and VDD_Mem_Chg are two entry points for
;	the routine that keeps track of which pages have been modified.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE VDD.INC
	INCLUDE HERC.INC
	INCLUDE DEBUG.INC

;******************************************************************************

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_Msg_NoMainMem:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_Get_Mode:NEAR

;	EXTRN	VDD_Font_Alloc:NEAR
;	EXTRN	VDD_Font_Dealloc:NEAR
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysB0000:DWORD
	EXTRN	VDD_Focus_VM:DWORD

; Make room for copy of all video RAM PTEs
PUBLIC	VDD_PT_Buff
VDD_PT_Size EQU 16
VDD_PT_Buff DD	VDD_PT_Size DUP (?)

VxD_DATA_ENDS

VxD_CODE_SEG
;******************************************************************************
;VDD_Mem_VMCreate
;
;DESCRIPTION:
;
;ENTRY: EBX = VM handle
;
;EXIT:	If error, CF = 1
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_VMCreate,PUBLIC

;*******
; First, initialize all the structures associated with memory
	xor	eax,eax

	lea	edx,[edi.VDD_CPg]
	mov	[edx.VPH_hMem],eax
	mov	[edx.VPH_PgCnt],al		; No copy pages allocated
	mov	[edx.VPH_PgAllMsk],eax
	mov	[edx.VPH_PgAccMsk],eax

	lea	edx,[edi.VDD_Pg]
	mov	[edx.VPH_hMem],eax
	mov	[edx.VPH_PgCnt],al		; No main pages allocated
	mov	[edx.VPH_PgAllMsk],eax
	mov	[edx.VPH_PgAccMsk],eax
	ret
EndProc VDD_Mem_VMCreate

;******************************************************************************
;VDD_Mem_VMInit
;
;DESCRIPTION:
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	If error, CF = 1
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_VMInit,PUBLIC
;
; Allocate initial memory for VM as text mode VM
;
;	cmp	[vgVDD.Vid_Type],Vid_Type_HERC112; Q: Herc 112?
;	jne	short VDD_M_NoFont		 ; N: No font feature
;	call	VDD_Font_Alloc			; Allocate Font Copy memory
;	jc	SHORT VMVI_Err			; Could not allocate
;VDD_M_NoFont:
        mov     [edi.VDD_Get_Mem_Count],0 ; no calls to VDD_Get_Mem

	call	VDD_Get_Mode
	call	VDD_Mem_AMain			; Allocate main memory
	jc	SHORT VMVI_Err			; Error if can't allocate
VMVI_00:

;Lastly, give access to physical video memory or virtual video memory
	cmp	[VDD_Focus_VM],ebx
	jz	VDD_Mem_Physical
	jmp	VDD_Mem_Virtual
	clc
VMVI_Err:
	ret
EndProc VDD_Mem_VMInit

;******************************************************************************
;VDD_Mem_VMSetTyp
;
;DESCRIPTION: Adjust memory state for new VM type.
;
;       If (!Grabbing && Windowed)
;           Update copy state with current
;           Update copy pages with current
;       else if (!Grabbing && !Windowed)
;           Deallocate copy memory
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_Mem_VMSetTyp,PUBLIC

	test	[edi.VDD_Flags],fVDD_Grab ; Q: Grabbing?
	jnz	SHORT VMST_Exit		  ;   Y: Do nothing

	test	[edi.VDD_Flags],fVDD_Win  ; Q: Windowed?
        jnz     short VMST_Update         ;   Y:
	call	VDD_Mem_DCopy		  ;   N: Dealloc the copy mem
        jmp     SHORT VMST_Exit
VMST_Update:
	call	VDD_State_Update	  ; Update copy w/current

	mov	eax,[edi.VDD_Pg.VPH_PgAllMsk]	; Get main mem allocated mask
	mov	[edi.VDD_AccPages],eax		; Update all mem allocated
	call	VDD_Mem_Update			; Update copy pages
VMST_Exit:
	ret
EndProc VDD_Mem_VMSetTyp

;******************************************************************************
;VDD_Mem_VMDestroy
;
;DESCRIPTION:
;	Delete memory allocated
;
;ENTRY: EBX = VM handle
;
;EXIT:	CF = 1 (makes VMCreate return with error on failed alloc)
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_Mem_VMDestroy,PUBLIC
	call	VDD_Mem_DMain			; Deallocate main pages
	call	VDD_Mem_DCopy			; Deallocate copy pages
	stc
	ret
EndProc VDD_Mem_VMDestroy

;******************************************************************************
;VDD_Mem_AMain	    Allocate main memory
;
;DESCRIPTION:
;	Allocates the video memory main pages. It determines what video
;	pages to allocate based on pages accessed. Note that the
;	pages accessed is truncated immediately after a mode set to the
;	pages that are normally used plus the pages that are not blank. See
;	the VDDINT module for further information. Subsequent to mode
;	changes, the access mask keeps track of which pages have been
;	changed by the VM. If the user has specified max allocation of
;	video memory to the VM, then the VM always has at least 128k of
;	video memory allocated. More is allocated as it is accessed.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;	EAX = VDD mode(must be one of 0-3)
;
;EXIT:	CF = 1 indicates unable to allocate some or all of memory requested
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_AMain,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi,[edi.VDD_Pg]
Vid_Mem_Alloc:
	mov	[esi.VPH_Mode],al		    ; Set current memory mode
	test	[esi.VPH_hMem],-1		    ; Q: Memory already alloc'd?
	jnz	SHORT VMAM_Done 		    ;	Y: All done
	VMMCall Test_Sys_VM_Handle		    ; Q: SYS VM?
	jz	SHORT VMAM_Done 		    ;	Y: not alloc'd memory
	mov	ecx,HERC_Mask			    ; Assume alloc 16k
	mov	eax,HERC_Pages
	push	eax				    ; Save count
	push	ecx				    ; Save alloc masks
	xor	ecx,ecx
	VMMCall _PageAllocate,<eax,PG_HOOKED,ebx,ecx,ecx,ecx,ecx,PageZeroInit+PageLocked>
	or	edx,edx
	jnz	SHORT VMAM_Good
	or	[edi.VDD_EFlags],fVDE_NoMain
	add	esp,8
	pop	esi
	stc
	ret
;
;Save handle and address and initialize Video Page Handle(VPH) structure
;
VMAM_Good:
	and	[edi.VDD_EFlags],NOT (fVDE_NoMain+fVDE_NoMMsg)
	and	[edi.VDD_Flags],NOT fVDD_Save	    ; Need to resave all of mem
	mov	[esi.VPH_hMem],eax		    ; Save memory handle
	mov	[esi.VPH_MState.VDA_Mem_Addr],edx   ; Save memory address
	pop	ecx				    ; ECX = 1st 32k mask
	pop	eax				    ; EAX = count
	mov	[esi.VPH_PgCnt],al		    ; pgs alloc'd
	mov	[esi.VPH_PgAllMsk],ecx		    ; Mask of pgs alloc'd
	mov	ecx,eax
	xor	eax,eax
VMAM_1:
	mov	[esi.VPH_MState.VDA_Mem_PgMap][eax],al 
	inc	eax
	loopd	VMAM_1
	cmp	al,HERC_Pages			    ; Q: On first 32k lin addr?
	jb	SHORT VMAM_Unalloc		    ;	Y: Mark rest unalloc'd
VMAM_Done:
	pop	esi
	clc
	ret
VMAM_Unalloc:
	cmp	al,HERC_Pages			    ; Q: On first 32k lin addr?
	jae	VMAM_Done			    ;	N: All done
	mov	[esi.VPH_MState.VDA_Mem_PgMap][eax],0FFh ; Page does not exist
	inc	eax
	jmp	VMAM_Unalloc
EndProc VDD_Mem_AMain

;******************************************************************************
;VDD_Mem_ACopy	    Allocate copy memory
;
;DESCRIPTION:
;	Allocates the video memory copy pages according to masks in VPH_AccMsk.
;
;ENTRY: EAX = MODE
;	EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1 indicates unable to allocate memory
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_ACopy,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi,[edi.VDD_CPg]
	jmp	Vid_Mem_Alloc
EndProc VDD_Mem_ACopy

;******************************************************************************
;VDD_Mem_DMain	    Deallocate all of the main memory
;
;DESCRIPTION:
;	Deallocates all the pages of the main memory
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1
;
;USES:	Flags, EAX, ECX, EDX
;
;CALLS: PageFree
;
BeginProc VDD_Mem_DMain,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi,[edi.VDD_Pg]
Vid_Mem_Delete:
	xor	eax,eax
	mov	[esi.VPH_MState.VDA_Mem_Addr],eax
	xchg	eax,[esi.VPH_hMem]
	or	eax,eax
	jz	SHORT VMDM_Ex
	VMMCall _PageFree,<eax,0>
	mov	ecx,VPHMax/4
	xor	eax,eax
	mov	[esi.VPH_PgAllMsk],eax
	dec	eax
	mov	[esi.VPH_Mode],al
	push	edi
	lea	edi,[esi.VPH_MState.VDA_Mem_PgMap]
        cld
	rep stosd
	pop	edi
VMDM_Ex:
	pop	esi
	ret
EndProc VDD_Mem_DMain

;******************************************************************************
;VDD_Mem_DCopy	    Deallocate all of the copy memory
;
;DESCRIPTION:
;	Deallocates all the pages of the copy memory
;
;ENTRY: EBX = VM handle
;
;EXIT:	CF = 1(so error in VDD_Mem_ACopy is reported)
;
;USES:	Flags, EAX, ECX, EDX
;
;CALLS: Vid_Mem_Dealloc
;
;ASSUMES:
;
BeginProc VDD_Mem_DCopy,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi, [edi.VDD_CPg]
	jmp	Vid_Mem_Delete
EndProc VDD_Mem_DCopy

;******************************************************************************
;VDD_Mem_Physical   Map VRAM to physical device
;
;DESCRIPTION:
;	Sets up page table so that VRAM addresses go to physical device.
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_Physical,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ecx
	push	edx
; First, map all pages of VIDEO memory
	mov	edx,0B0h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,16,0>
	pop	edx
	pop	ecx
	clc
	ret
EndProc VDD_Mem_Physical

;******************************************************************************
;VDD_Mem_Update
;
;DESCRIPTION:
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_Update,PUBLIC

Assert_VDD_ptrs ebx,edi
	pushad
;
; Need to copy main memory
;
	mov	eax,[edi.VDD_AccPages]	    ; Adjust for display offset
	test	eax,eax
	jz	SHORT VMU_Done	      	    ; No changes to copy
	or	BYTE PTR [edi.VDD_CPg.VPH_PgAccMsk],al
	push	eax
	movzx	eax,[edi.VDD_Mode]
	call	VDD_Mem_ACopy		    ; If needed, alloc more mem
	pop	eax
	and	eax,[edi.VDD_CPg.VPH_PgAllMsk]
	jz	SHORT VMU_Done
	mov	ebx,edi
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
VMU_01:
	shr	eax,1
	jnc	SHORT VMU_02
	push	esi
	push	edi

	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]

	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_PgMap][edi]
	shl	edi,12
;	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]

	mov	ecx,1000h/4
        cld
	rep movsd
	pop	edi
	pop	esi
VMU_02:
	inc	esi
	inc	edi
	or	al,al
	jnz	SHORT VMU_01
	mov	edi,ebx
VMU_Done:
	xor	eax,eax
	mov	[edi.VDD_AccPages],eax
	popad
	ret
EndProc VDD_Mem_Update

;******************************************************************************
;VDD_Mem_ChkPag     Check the page table entries of the video memory
;
;DESCRIPTION:
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX -- bits for corresponding PTEs are set if Page Written
;	ZF - 1 if no changes, 0 if any page changed
;
;USES:	Flags, EAX
;
;ASSUMES:
;	Pages are mapped at xxx
;
BeginProc VDD_Mem_ChkPag,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	edx
	push	ecx
	call	VDD_Mem_Chg
	pop	ecx
	pop	edx
	ret
EndProc VDD_Mem_ChkPag

;******************************************************************************
;VDD_Mem_UpdOff
;
;DESCRIPTION:
;	Call VDD_Mem_UpdOff to update the offset of the displayed memory
;	from the start of the video memory.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX = display memory offset
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_UpdOff,PUBLIC

Assert_VDD_ptrs ebx,edi
	xor	eax, eax
	test	[edi.VDD_Stt.V_Mode], fModPage1 ;Q: graphics page 1?
	jz	SHORT page_0			;   N:
	mov	eax, 8				;   Y: start at page 8
page_0:
	mov	[edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag],al
	ret
EndProc VDD_Mem_UpdOff

;******************************************************************************
;VDD_Mem_ModPag
;
;DESCRIPTION:
;	Call VDD_Mem_Chg to get all of video modified page bits and then
;	shifts that according to which page is beeing displayed
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX -- bits for corresponding PTEs are set if displayed page modified
;	ZF - 1 if no changes, 0 if any page changed
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_ModPag,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	edx
	push	ecx
	call	VDD_Mem_Chg
	mov	eax,[edi.VDD_AccPages]
	pop	ecx
	pop	edx
	or	eax,eax
	ret
EndProc VDD_Mem_ModPag

;******************************************************************************
;VDD_Mem_Chg
;
;DESCRIPTION:
;	Save the dirty bits, then clear the bits in the page table.
;	VDD_AccPages is cumulative until a VDD_Mem_Update call is made.
;	VDD_DirtyPages is cumulative until a save is done. Thus, VDD_DirtyPages
;	keeps track of which pages of virtual EGA memory need to be updated.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX -- bits for corresponding PTEs are set if Page Written
;	ZF = 1 if no dirty pages
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_Chg,PUBLIC

Assert_VDD_ptrs ebx,edi
	call	VMC_Scan			; EAX = dirty bits, pg 0 = bit 0
	test	eax,eax 			; Q: Dirty pages?
	jz	SHORT VMC_Exit			;   N: Done
	push	eax
	VMMCall Get_System_Time
	mov	[edi.VDD_ModTime],eax		; Save last time modified
	pop	eax
;
; All Hercules Pages are unique.
;
	or	[edi.VDD_DirtyPages],eax	; Merge with old dirty bits
	or	[edi.VDD_AccPages],eax

VMC_Exit:
	mov	eax,[edi.VDD_DirtyPages]	; Get old dirty pages
	test	eax,eax
	ret
EndProc VDD_Mem_Chg

;******************************************************************************
;VMC_Scan
;
;DESCRIPTION: Scans PTEs for dirty bits, also clears dirty bits.
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX = dirty bits from page B0h to BFh
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VMC_Scan
	mov	edx,0B0h
	mov	ecx,16
;
; Copy the page table
;
	push	ecx
	push	edx
	shl	edx,12
	add	edx,[ebx.CB_High_Linear]
	shr	edx,12
	xor	eax,eax 		; EAX = 0
	VMMCall _CopyPageTable,<edx,ecx,<OFFSET32 VDD_PT_Buff>,eax>
	pop	edx
	pop	ecx
;
; Clear the dirty bits
;
	push	ecx
	xor	eax,eax 		; EAX = 0
	VMMCall _ModifyPageBits,<ebx,edx,ecx,-1,eax,PG_IGNORE,eax>
	pop	ecx
;
; Form bit mask for dirty pages
;
	mov	edx,OFFSET32 VDD_PT_Buff-4
	xor	eax,eax
VMC_Loop:
	bt	DWORD PTR [edx][ecx*4],6
	rcl	eax,1			; Rotate page dirty in EAX
	loop	VMC_Loop
;%OUT VMC_Scan return all pages dirty for now.
;        mov     eax, 0FFFFh
	ret
EndProc VMC_Scan

;******************************************************************************
;VDD_Mem_MapNull
;
;DESCRIPTION: Maps null page at page #EDX
;
;ENTRY: EDX = page number
;	EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_MapNull,PUBLIC
	push	edx
	VMMCall _GetNulPageHandle
	pop	edx
	xor	ecx,ecx
	VMMCall _MapIntoV86,<eax,ebx,edx,1,ecx,ecx>
	ret
EndProc VDD_Mem_MapNull


;******************************************************************************
;VDD_Mem_Null	    Map null memory into video address space
;
;DESCRIPTION:
;	Map null memory into all of VM's video address space
;
;ENTRY: EBX = VM handle
;	EDX = address of faulting PTE
;
;EXIT:	if error, CF = 1
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_Null,PUBLIC

Assert_VDD_ptrs ebx,edi
	VMMCall _GetNulPageHandle
	mov	edx,0B0h
	VMMCall _MapIntoV86,<eax,ebx,edx,16,0,0>
	ret
EndProc VDD_Mem_Null

;******************************************************************************
;VDD_Mem_Virtual    Map Virtual VRAM into VRAM addresses
;
;DESCRIPTION:
;	Maps Virtual VRAM into VRAM addresses for Display Apps running
;	in background.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_Virtual, PUBLIC

        test    [edi.VDD_Pg.VPH_hMem],-1
        jz      short VMV_NoVRAM

; Map B0000-BFFFF
	VMMCall _MapIntoV86,<[edi.VDD_Pg.VPH_hMem],ebx,0B0h,HERC_Pages,0,0>
MVRet:
	clc
	ret

VMV_NoVRAM:
        call    VDD_Mem_Null
        jmp     SHORT MVRet

EndProc VDD_Mem_Virtual

;******************************************************************************
;VDD_Mem_I10Done
;
;DESCRIPTION: This routine clears the dirty bits in the extended memory since
;	we are just completing a mode state initialization and we know what
;	those pages were set to(0 for graphics modes, white spaces for text
;	modes) and will initialize them later if they are really being used.
;
;ENTRY: EBX = VM Handle, EDI = CB ptr
;
;EXIT:	Page table entries cleared for VM
;
;USES:	Flags, EAX, ECX 
;
BeginProc VDD_Mem_I10Done,PUBLIC

Assert_VDD_ptrs ebx,edi
	call	VDD_Get_Mode
	VMMCall Test_Sys_VM_Handle		; Q: System VM?
	jz	short VMI10_Exit
        
	or	[edi.VDD_Flags],fVDD_MInit	; VM's display mem initialized

        mov     ecx,01                          ; Assume simple text mode
        cmp     al,HERC_MODE_Text               ; Q: Text mode?
        je      SHORT VMI10_GotMask             ;   Y:

        mov     ecx,01Fh                        ; Assume 4K RAM font mode
        test    al,HERC_MODE_4KText             ; Q: 4K RAM font mode?
        jnz     SHORT VMI10_GotMask             ;   Y:

	mov	ecx,0FFFFh                      ; 64K for graphics or 48K ram font mode
        test    al,HERC_MODE_48KText            ; Q: 48K RAM font mode?
        jnz     SHORT VMI10_GotMask             ;   Y:

        mov     ecx,0FFh
	test	[edi.VDD_Stt.V_Mode], fModPage1 ;Q: graphics page 1?
        jz      SHORT VMI10_GotMask
        shl     ecx,8
VMI10_GotMask:
	mov	[edi.VDD_DirtyPages],ecx	;   according to mode
	mov	[edi.VDD_AccPages],ecx
	mov	[edi.VDD_Pg.VPH_PgAccMsk],ecx
VMI10_Exit:
	ret
EndProc VDD_Mem_I10Done

VxD_CODE_ENDS

	END



