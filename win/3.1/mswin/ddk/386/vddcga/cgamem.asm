       title   VDD - Virtual Display Device for CGA version 0.0 5/28/87
;******************************************************************************
;
;VDD - Virtual Display Device
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;DESCRIPTION:
;	This module handles all of the manipulation of a VM's video memory
;	space. This includes mapping of physical device, mapping normal RAM for
;	emulation of physical device and null pages.  The pages are disabled,
;	hooked pages when they have not been allocated to the VM and when the
;	mode of the controller has changed. VDD_Mem_Init initializes a VM's
;	page table for video memory. VDD_Mem_AMain, VDD_Mem_ACopy,
;	VDD_Mem_DMain and VDD_Mem_DCopy handle allocation and deallocation of
;	memory handles. VDD_Mem_Null is for VM1 running without the screen. 
;	    Additional routines are used to determine state and keep track of
;	changes to the video memory(i.e. keep track of hardware detection of
;	changes). VDD_Mem_Chg keeps track of which pages have been modified.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE VDD.INC
	INCLUDE CGA.INC
	INCLUDE DEBUG.INC

;******************************************************************************

;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_Msg_NoMainMem:NEAR
	EXTRN	VDD_Msg_NoCopyMem:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Scrn_Size:NEAR

	EXTRN	VDD_Font_Alloc:NEAR
	EXTRN	VDD_Font_Dealloc:NEAR
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysB8000:DWORD
	EXTRN	VDD_Focus_VM:DWORD
        EXTRN   VDD_Grb_MskTab:DWORD
;Table of changed pages after mode change(force save)
PUBLIC	VDD_Mem_Ini_Tab
VDD_Mem_Ini_Tab LABEL DWORD
	DD	000030303h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000030303h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000030303h		; mode 2, 80x43 alpha, 2 bytes/char
	DD	000030303h		; mode 3, 80x43 alpha, 2 bytes/char
	DD	000000F0Fh		; mode 4, 320x200x2 graphics
	DD	000000F0Fh		; mode 5, 320x200x2 graphics
	DD	00000000Fh		; mode 6, 640x200x1 graphics
	DD	0000000FFh		; mode 7, 640x400x1 graphics

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

	mov	[edi.VDD_Get_Mem_Count],0	; no calls to VDD_Get_Mem
;Allocate initial memory for VM as text mode VM
	cmp	[vgVDD.Vid_Type],Vid_Type_IDC	; Q: IDC?
	jne	short VDD_M_NoFont		; N: No font feature
	call	VDD_Font_Alloc			; Allocate IDC Font Copy memory
	jc	SHORT VMVI_Err			; Could not allocate
VDD_M_NoFont:
	call	VDD_Get_Mode
	call	VDD_Mem_AMain			; Allocate main memory
	jc	SHORT VMVI_Err			; Error if can't allocate
VMVI_00:

;Lastly, give access to physical video memory or virtual video memory
	cmp	[VDD_Focus_VM],ebx
	jz	VDD_Mem_Physical
	jmp	VDD_Mem_Virtual
VMVI_Err:
	ret
EndProc VDD_Mem_VMInit

;******************************************************************************
;VDD_Mem_VMSetTyp
;
;DESCRIPTION:
;	Adjust memory state for new VM type.
;
; Pseudo code:
;
;       If (!Grabbing && Windowed)
;           Update copy state with current
;           Update copy pages with current
;       else if (!Grabbing && !Windowed)
;           Deallocate grab memory
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
	call	VDD_State_Update	  ; Update copy state w/ current
        call    VDD_Mem_UpdOff                  ; update DispPag and offset
	call	VDD_Get_Mode
        call    VDD_SetCopyMask                 ; ECX = mask for copy pages
	movzx	edx,WORD PTR [edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	test	edx,edx
	jz	SHORT VMST_GotMask
VMST_Loop:
	shl	ecx,1
	dec	edx
	jnz	SHORT VMST_Loop
VMST_GotMask:
; make accessed pages all of displayed memory
	mov	[edi.VDD_AccPages],ecx	     
	call	VDD_Mem_Update		     ; Update copy pages w/ current
VMST_Exit:
	ret
EndProc VDD_Mem_VMSetTyp

;*****************************************************************************
;
; VDD_SetCopyMask - Set copy page mask = displayed pages mask
;
; ENTRY: EAX = mode
;	 EBX = VM handle
;        EDI = CB ptr
;
; EXIT: ECX = copy page mask
;
; USES: ECX
;
; NOTE	Does not handle non-zero display offset for graphics mode
;     
;==========================================================================
BeginProc VDD_SetCopyMask, PUBLIC

        push    esi
        push    edx
        cmp     eax,3
	jbe	SHORT VSCM_Text
; Graphics modes copy mask is mode based
	pop	edx
	pop	esi
	mov	ecx,VDD_Grb_MskTab[eax*4]       
	ret
VSCM_Text:
	lea	edx,[edi.VDD_Stt]
	lea	esi,[edi.VDD_Pg]		    ; ESI = VPH struc ptr
	call	VDD_Scrn_Size			    ; Set Rows, Cols, PgSz

	mov	ecx,[esi.VPH_MState.VDA_Mem_DPagOff]
	add	ecx,[esi.VPH_PgSz]
        pop     edx
        pop     esi

	cmp	ecx,1000h
        ja      SHORT VSCM_00
        mov     ecx,01                  ; display spans 1 page
	ret
VSCM_00:
        cmp     ecx,2000h
        ja      SHORT VSCM_01
        mov     ecx,03                  ; display spans 2 pages
	ret
VSCM_01:
        cmp     ecx,3000h
        ja      SHORT VSCM_02
        mov     ecx,07                  ; display spans 3 pages
	ret
VSCM_02:
        mov     ecx,0FH                 ; display spans 4 pages
        ret
        
EndProc VDD_SetCopyMask

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
	cmp	[vgVDD.Vid_Type],Vid_Type_IDC	; Q: IDC or ATT (400 line)
	jnz	SHORT VMVD_NotIDC		;   N: all done
	call	VDD_Font_Dealloc		;   Y: dealloc font also
VMVD_NotIDC:
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
;	EAX = VDD mode(must be one of 0-6)
;
;EXIT:	CF = 1 indicates unable to allocate some or all of memory requested
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_AMain,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi,[edi.VDD_Pg]
	call	VDD_Mem_Alloc
	jnc	SHORT VMAM_GotMem
	or	[edi.VDD_EFlags],fVDE_NoMain
	call	VDD_Msg_NoMainMem
	pop	esi
	ret
VMAM_GotMem:
	and	[edi.VDD_EFlags],NOT (fVDE_NoMain+fVDE_NoMMsg)
	pop	esi
	ret
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
	call	VDD_Mem_Alloc
	jnc	SHORT VMAC_GotMem
	or	[edi.VDD_EFlags],fVDE_NoCopy
	call	VDD_Msg_NoCopyMem
	pop	esi
	ret
VMAC_GotMem:
	and	[edi.VDD_EFlags],NOT (fVDE_NoCopy+fVDE_NoCMsg)
	pop	esi
	ret
EndProc VDD_Mem_ACopy

;******************************************************************************
;VDD_Mem_Alloc	    Allocate memory
;
;DESCRIPTION:
;	Allocates the video memory pages according to masks in VPH_AccMsk.
;
;ENTRY: EAX = MODE
;	EBX = VM handle
;	ESI = VPH structure pointer (for Video Page Handle data)
;	EDI = VDD CB ptr
;
;EXIT:	CF = 1 indicates unable to allocate memory
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_Alloc
	mov	[esi.VPH_Mode],al		    ; Set current memory mode
	test	[esi.VPH_hMem],-1		    ; Q: Memory already alloc'd?
	jnz	SHORT VMA_Done			    ;	Y: All done
	VMMCall Test_Sys_VM_Handle		    ; Q: SYS VM?
	jz	SHORT VMA_Done			    ;	Y: not alloc'd memory
	mov	ecx,CGA_Mask			    ; Assume alloc 16k
	mov	eax,CGA_Pages
	cmp	[vgVDD.Vid_Type],Vid_Type_IDC	    ; Q: IDC or ATT (400 line)
	jb	SHORT VMA_CGA			    ;	N: CGA, 16k OK
	mov	ecx,IDC_Mask			    ; Alloc 32k
	mov	eax,IDC_Pages
.ERRE	IDC_Pages EQ ATT_Pages
VMA_CGA:

	push	eax				    ; Save count
	push	ecx				    ; Save alloc masks
	xor	ecx,ecx
	VMMCall _PageAllocate,<eax,PG_HOOKED,ebx,ecx,ecx,ecx,ecx,PageZeroInit+PageLocked>
	or	edx,edx
	jnz	SHORT VMA_Good
	add	esp,8
	stc
	ret
;Save handle and address and initialize Video Page Handle(VPH) structure
VMA_Good:
	and	[edi.VDD_Flags],NOT fVDD_Save	    ; Need to resave all of mem
	mov	[esi.VPH_hMem],eax		    ; Save memory handle
	mov	[esi.VPH_MState.VDA_Mem_Addr],edx   ; Save memory address
	pop	ecx				    ; ECX = 1st 32k mask
	pop	eax				    ; EAX = count
	mov	[esi.VPH_PgCnt],al		    ; pgs alloc'd
	mov	[esi.VPH_PgAllMsk],ecx		    ; Mask of pgs alloc'd
	mov	ecx,eax
	xor	eax,eax
VMA_1:
	mov	[esi.VPH_MState.VDA_Mem_PgMap][eax],al 
	inc	eax
	loopd	VMA_1
	cmp	al,IDC_Pages			    ; Q: On first 32k lin addr?
	jb	SHORT VMA_Unalloc		    ;	Y: Mark rest unalloc'd
VMA_Done:
	clc
	ret
VMA_Unalloc:
	cmp	al,IDC_Pages			    ; Q: On first 32k lin addr?
	jae	VMA_Done			    ;	N: All done
	mov	[esi.VPH_MState.VDA_Mem_PgMap][eax],0FFh ; Page does not exist
	inc	eax
	jmp	VMA_Unalloc
EndProc VDD_Mem_Alloc

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
	mov	edx,0B8h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,8,0>
	pop	edx
	pop	ecx
	clc
	ret
EndProc VDD_Mem_Physical


;******************************************************************************
;VDD_Mem_Update
;
;DESCRIPTION:
;       Using the mask in VDD_AccPages, update copy pages with current pages.
;
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
; Need to copy main memory
	mov	eax,[edi.VDD_AccPages]		    ; Adjust for display offset
	and	eax,[edi.VDD_Pg.VPH_PgAllMsk]
	call	VDD_Adj_Msk			    ; EAX = alloc mask, adjusted
	and	eax,0Fh
	test	eax,eax
	jz	VMU_Done			    ; No changes to copy
	or	BYTE PTR [edi.VDD_CPg.VPH_PgAccMsk],al
	push	eax
        call    VDD_Get_Mode                        ; EAX = Mode
	call	VDD_Mem_ACopy			    ; If needed, alloc more mem
	pop	eax
	and	eax,[edi.VDD_CPg.VPH_PgAllMsk]
	jz	VMU_Done
	mov	ebx,edi
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
VMU_01:
	shr	eax,1
	jnc	SHORT VMU_02
	push	esi
	push	edi

	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	cmp	esi,0FFh
IFDEF DEBUG
	jne	short VMU_D00
	Debug_Out "VDD:Vdd_mem_update Invalid page index"
VMU_D00:
ENDIF
	je	SHORT VMU_SkipPage
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DPagOff]
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]

	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_PgMap][edi]
	cmp	edi,0FFh
IFDEF DEBUG
	jne	short VMU_D01
	Debug_Out "VDD:Vdd_mem_update Invalid page index"
VMU_D01:
ENDIF
	je	SHORT VMU_SkipPage
	shl	edi,12
;	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DPagOff]
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]

	mov	ecx,1000h/4
        cld
	rep movsd
VMU_SkipPage:
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
	movzx	eax,WORD PTR [edi.VDD_Stt.V_AddrH]
	xchg	ah,al				; ECX = display start w/in plane
	shl	eax,1				;   Y: 2 bytes per CRTC address
	push	eax
	and	eax,0FFFh
	mov	[edi.VDD_CPg.VPH_MState.VDA_Mem_DPagOff],eax ; Offset w/in pg
	mov	[edi.VDD_Pg.VPH_MState.VDA_Mem_DPagOff],eax
	pop	eax
	shr	eax,12
	mov	[edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag],al
	mov	[edi.VDD_CPg.VPH_MState.VDA_Mem_DispPag],0 ; Copy always at 0
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
	call	VDD_Adj_Msk			; Adjust bits by start offset
	pop	ecx
	pop	edx
	or	eax,eax
	ret
EndProc VDD_Mem_ModPag


;******************************************************************************
;VDD_Adj_Msk
;
;DESCRIPTION:
;   shift right mask in EAX ECX times
;   Return shifted mask in EAX.
;
;ENTRY: EAX = 1st 32k page mask
;	EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX = page mask adjusted for display offset (bit 0 is start of display)
;	ZF = 1 if EAX is zero
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Adj_Msk
; Adjust page bits by display start offset

	movzx	ecx,WORD PTR [edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	test	ecx,ecx 		; Q: Page 0 displayed?
	jz	SHORT VAB_Exit          ;  Y: No shifting reqd.
	shr	eax,cl			; Adjust pages modified mask
VAB_Exit:
	or	eax,eax
	ret
EndProc VDD_Adj_Msk

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
	test	[edi].VDD_Stt.I_EMSel,fEMSelEMod  ; Q: Is it IDC
						; in extended (640x400) mode?
	jnz	SHORT VDD_MC_400		;   Y: pages are unique
	test	[edi].VDD_Stt.V_Mode2,1 	; Q: ATT extended(640x400) mode?
	jnz	SHORT VDD_MC_400		;   Y: pages are unique

; Normal CGA wraps addresses in range BC000-BFFFF to B8000-BBFFF
;   Adjust pages modified mask accordingly
	mov	ecx,eax 			; Save low bits
	shr	eax,CGA_Pages			; Shift high bits
	or	eax,ecx 			; Merge alligned bits
	and	eax,(1 shl (CGA_Pages))-1	; Mask off extra bits
VDD_MC_400:
	or	[edi.VDD_DirtyPages],eax	; Merge with old dirty bits
	or	[edi.VDD_AccPages],eax
; Now do extended pages
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
;EXIT:	EAX = dirty bits from page B8h to BFh
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VMC_Scan
	mov	edx,0B8h
	mov	ecx,8
; Copy the page table
	push	ecx
	push	edx
	shl	edx,12
	add	edx,[ebx.CB_High_Linear]
	shr	edx,12
	xor	eax,eax 			; EAX = 0
	VMMCall _CopyPageTable,<edx,ecx,<OFFSET32 VDD_PT_Buff>,eax>
	pop	edx
	pop	ecx
; Clear the dirty bits
	push	ecx
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,edx,ecx,-1,eax,PG_IGNORE,eax>
	pop	ecx
; Form bit mask for dirty pages
	mov	edx,OFFSET32 VDD_PT_Buff-4
	xor	eax,eax
VMC_Loop:
	bt	DWORD PTR [edx][ecx*4],6
	rcl	eax,1				; Rotate page dirty in EAX
	loop	VMC_Loop
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
	mov	edx,0B8h
	VMMCall _MapIntoV86,<eax,ebx,edx,8,0,0>
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
;
;EXIT:	CF = 0
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_Virtual, PUBLIC

        test    [edi.VDD_Pg.VPH_hMem],-1
        jz      short VMV_NoVRAM

; Map B8000-B8FFF
	VMMCall _MapIntoV86,<[edi.VDD_Pg.VPH_hMem],ebx,0B8h,CGA_Pages,0,0>
	cmp	[vgVDD.Vid_Type],Vid_Type_ATT	; Q: ATT?
	jz	SHORT VDD_MV_400		;   Y: Map 2nd 16k

	cmp	[vgVDD.Vid_Type],Vid_Type_CGA	; Q: CGA?
        je      SHORT VDD_MVCGA						

	test	[edi.VDD_Stt.I_EMSel],fEMSelEMod; Q: IDC Extended Mode?
	jnz	SHORT VDD_MV_400		;   Y: Map 2nd 16k
VDD_MVCGA:
; Remap same pages at BC000-BFFFF		;   N: Remap 1st 16k
	VMMCall _MapIntoV86,<[edi.VDD_Pg.VPH_hMem],ebx,0BCh,CGA_Pages,0,0>
	clc
	ret
VDD_MV_400:
; Map BC000-BFFFF
	VMMCall _MapIntoV86,<[edi.VDD_Pg.VPH_hMem],ebx,0BCh,CGA_Pages,CGA_Pages,0>
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
	mov	ecx,VDD_Mem_Ini_Tab[eax*4]	; Set PTE accessed masks

	or	[edi.VDD_Flags],fVDD_MInit	; VM's display mem initialized
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
