;******************************************************************************
;
;VDDMEM - Virtual Display Device Memory Handler
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986-1991
;
;DESCRIPTION:
;	    This module handles all of the manipulation of a VM's video memory
;	space. This includes mapping of physical device, mapping normal RAM for
;	emulation of physical device and null pages.  The pages are disabled 
;	and hooked when they have not been allocated to the VM and when the
;	mode of the controller has changed. 
;
;   VDD_Mem_Init 
;    - initializes a VM's page table for video memory. 
;   VDD_Mem_AMain, VDD_Mem_ACopy, VDD_Mem_DMain and VDD_Mem_DCopy 
;    - handle allocation and deallocation of memory handles. 
;   VDD_Mem_Remap 
;    - handles the changes due to a mode change. 
;   VDD_Mem_Null, VDD_Mem_Disable
;    - handle the transition states. Note that VDD_Mem_Null is for VM1 
;      running without the screen. 
;   VDD_Mem_Physical 
;    - maps the physical EGA 
;   VDD_Mem_Physical2 
;    - maps the physical EGA, offset by 32k for graphics addresses
;      (Lin Adr A0000-A7FFF is mapped to physical A8000-AFFFF).
;   VDD_Mem_Valid 
;    - determines if the current memory mapping is valid with the current
;      video state. 
;   VDD_Mem_ChkPag,VDD_Mem_Chg
;    - are two entry points for the routine that keeps track of which 
;      pages have been modified.
;
;	Additional routines are used to determine state and keep track of
;	changes to the video memory(i.e. keep track of hardware detection of
;	changes).  
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE OPTTEST.INC
	INCLUDE VDD.INC
	INCLUDE PAGESWAP.INC
	INCLUDE EGA.INC
	INCLUDE DEBUG.INC


;******************************************************************************
; EXTRN routines
;
VxD_CODE_SEG
	EXTRN	VDD_Msg_NoMainMem:NEAR
	EXTRN	VDD_Msg_NoCopyMem:NEAR
	EXTRN	VDD_Msg_Exclusive:NEAR
	EXTRN	VDD_State_Update:NEAR
	EXTRN	VDD_Get_Mode:NEAR
	EXTRN	VDD_Scrn_Size:NEAR
	EXTRN	VDD_Attach2:NEAR
	EXTRN	VDD_Restore2:NEAR
	EXTRN	VDD_Detach2:NEAR
IFDEF VGA
	EXTRN	VDD_Init_DAC:NEAR
	EXTRN	VDD_Scrn_Size:NEAR
	EXTRN	VDD_SaveCtlr:NEAR
        EXTRN   VDD_Scrn_Off:NEAR
        EXTRN   VDD_RestIndx:NEAR
ENDIF
VxD_CODE_ENDS

;******************************************************************************
; EXTRN data
;
VxD_DATA_SEG
	Extern_vgVDD
	EXTRN	VDD_CB_Off:DWORD
	EXTRN	VDD_PhysA0000:DWORD
	EXTRN	VDD_2EGA_Start:DWORD
	EXTRN	VDD_Msg_VM:DWORD
        EXTRN   VDD_Grb_MskTab:DWORD

; Make room for copy of all video RAM PTEs
PUBLIC	VDD_PT_Buff
VDD_PT_Size EQU 32
VDD_PT_Buff DD	VDD_PT_Size DUP (?)

; Make room for temp copy of VPH_MState's PgMap during reallocation
VDD_PgMap_Temp	DB	VPHMax DUP (?)	; Map video pg to mem handle offset

	ALIGN 4
; Tables of video memory addressing based on G_Misc reg value
VRAM_PTE_Start LABEL DWORD
	DD	0A0h				; A0000h to BFFFFh
	DD	0A0h				; A0000h to AFFFFh
	DD	0B0h				; B0000h to B7FFFh
	DD	0B8h				; B8000h to BFFFFh

VRAM_PTE_Cnt LABEL DWORD
	DD	32				 
	DD	16
	DD	8
	DD	8

; VDD_WPlane maps write planes to memory plane by:
; 0&?=0,not 0&1&?=1,not 0&not 1&2=2, not 0&not 1&not 2&3=3, not any = 0
VDD_WPlane  DB	0,0,1,0,2,0,2,0,3,0,1,0,2,0,1,0

	ALIGN 4
    ;
    ; Table of changed pages after mode change(force save)
    ; Table indexed by mode
    ; Each entry is mask for the 1st 32K with 
    ; LSByte being the mask for Plane 0 and MSByte for Plane 3.
    ;
PUBLIC	VDD_Mem_Ini_Tab
VDD_Mem_Ini_Tab LABEL DWORD
	DD	000030303h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000030303h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000030303h		; mode 2, 80x43 alpha, 2 bytes/char
	DD	000030303h		; mode 3, 80x43 alpha, 2 bytes/char
	DD	000000F0Fh		; mode 4, 320x200x2 graphics
	DD	000000F0Fh		; mode 5, 320x200x2 graphics
	DD	00000000Fh		; mode 6, 640x200x1 graphics
	DD	000030303h		; mode 7, 80x43 alpha, 2 bytes/char
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000030303h		; mode B looks like text (0-3)
	DD	000000000h		; mode C not supported
	DD	003030303h		; mode D, 320x200 graphics, 4 32k planes
	DD	00F0F0F0Fh		; mode E, 640x200 graphics, 4 32k planes
	DD	0007F007Fh		; mode F, 640x350 graphics, 2 28k planes
	DD	07F7F7F7Fh		; mode 10,640*350 graphics, 4 28k planes
IFDEF	VGA
	DD	0000000FFh		; mode 11, 640x480 graphics, 1 38k pln
	DD	0FFFFFFFFh		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln
ENDIF

    ;
    ; Table of changed pages after mode change(force save)
    ; Table indexed by mode
    ; Each entry is mask for the 2nd 32K with 
    ; LSByte being the mask for Plane 0 and MSByte for Plane 3.
    ;
PUBLIC	VDD_Mem_Ini_Tab2
VDD_Mem_Ini_Tab2 LABEL DWORD
	DD	000000000h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 4, 320x200x2 graphics
	DD	000000000h		; mode 5, 320x200x2 graphics
	DD	000000000h		; mode 6, 640x200x1 graphics
	DD	000000000h		; mode 7 not supported
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000000000h		; mode B looks like text (0-3)
	DD	000000000h		; mode C not supported
	DD	000000000h		; mode D, 320x200 graphics, 4 32k planes
	DD	000000000h		; mode E, 640x200 graphics, 4 32k planes
	DD	000000000h		; mode F, 640x350 graphics, 2 28k planes
	DD	000000000h		; mode 10,640x350 graphics, 4 28k planes
IFDEF	VGA
	DD	000000003h		; mode 11, 640x480 graphics, 1 38k pln
	DD	003030303h		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln
ENDIF

    ;
    ; Table of max allocation masks for a given mode
    ; Table indexed by mode
    ; Each entry is mask for the 1st 32K with 
    ; LSByte being the mask for Plane 0 and MSByte for Plane 3.
    ;
VDD_MaxAlloc_Pg_Msk LABEL DWORD
	DD	000FF00FFh		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000FF00FFh		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000FF00FFh		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000FF00FFh		; mode 3, 80x25 alpha, 2 bytes/char
	DD	0000000FFh		; mode 4, 320x200x2 graphics
	DD	0000000FFh		; mode 5, 320x200x2 graphics
	DD	0000000FFh		; mode 6, 640x200x1 graphics
	DD	000FF00FFh		; mode 7, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000FF00FFh		; mode B, text mode mapped for font load
	DD	000000000h		; mode C not supported
	DD	0FFFFFFFFh		; mode D, 320x200 graphics, 4 32k planes
	DD	0FFFFFFFFh		; mode E, 640x200 graphics, 4 32k planes
	DD	000FF00FFh		; mode F, 640x350 graphics, 2 28k planes
	DD	0FFFFFFFFh		; mode 10,640*350 graphics, 4 28k planes
IFDEF	VGA
	DD	0000000FFh		; mode 11, 640x480 graphics, 1 38k pln
	DD	0FFFFFFFFh		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln
ENDIF

    ;
    ; Table of max allocation masks for a given mode
    ; Table indexed by mode
    ; Each entry is mask for the 2nd 32K with 
    ; LSByte being the mask for Plane 0 and MSByte for Plane 3.
    ;
VDD_MaxAlloc2_Pg_Msk	LABEL DWORD
	DD	000000000h		; mode 0, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 1, 40x25 alpha, 2 bytes/char
	DD	000000000h		; mode 2, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 3, 80x25 alpha, 2 bytes/char
	DD	000000000h		; mode 4, 320x200x2 graphics
	DD	000000000h		; mode 5, 320x200x2 graphics
	DD	000000000h		; mode 6, 640x200x1 graphics
	DD	000000000h		; mode 7 not supported
	DD	000000000h		; mode 8 not supported
	DD	000000000h		; mode 9 not supported
	DD	000000000h		; mode A not supported
	DD	000000000h		; mode B, text mode mapped for font load
	DD	000000000h		; mode C not supported
	DD	0FFFFFFFFh		; mode D, 320x200 graphics, 4 32k planes
	DD	0FFFFFFFFh		; mode E, 640x200 graphics, 4 32k planes
	DD	000FF00FFh		; mode F, 640*350 graphics, 2 28k planes
	DD	0FFFFFFFFh		; mode 10,640*350 graphics, 4 28k planes
IFDEF	VGA
	DD	0000000FFh		; mode 11, 640x480 graphics, 1 38k pln
	DD	0FFFFFFFFh		; mode 12, 640x480 graphics, 4 38k plns
	DD	0000000FFh		; mode 13, 320x200x8 graphics, 1 64k pln
ENDIF

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

; Initialize all the structures associated with memory
	xor	eax,eax

	lea	edx,[edi.VDD_CPg]
	mov	[edx.VPH_hMem],eax
	mov	[edx.VPH_PgCnt],al		; No copy pages allocated
	mov	[edx.VPH_PgAllMsk],eax
	mov	[edx.VPH_Pg2AllMsk],eax
	mov	[edx.VPH_PgAccMsk],eax
	mov	[edx.VPH_Pg2AccMsk],eax
	dec	eax
	lea	edx,[edx.VPH_MState.VDA_Mem_PgMap]
	mov	DWORD PTR [edx],eax
	mov	DWORD PTR [edx+4],eax
	mov	DWORD PTR [edx+8],eax
	mov	DWORD PTR [edx+12],eax
	mov	DWORD PTR [edx+16],eax
	mov	DWORD PTR [edx+20],eax
	mov	DWORD PTR [edx+24],eax
	mov	DWORD PTR [edx+28],eax
	inc	eax

	lea	edx,[edi.VDD_Pg]
	mov	[edx.VPH_hMem],eax
	mov	[edx.VPH_PgCnt],al		; No main pages allocated
	mov	[edx.VPH_PgAllMsk],eax
	mov	[edx.VPH_Pg2AllMsk],eax
	mov	[edx.VPH_PgAccMsk],eax
	mov	[edx.VPH_Pg2AccMsk],eax
	dec	eax
	lea	edx,[edx.VPH_MState.VDA_Mem_PgMap]
	mov	DWORD PTR [edx],eax
	mov	DWORD PTR [edx+4],eax
	mov	DWORD PTR [edx+8],eax
	mov	DWORD PTR [edx+12],eax
	mov	DWORD PTR [edx+16],eax
	mov	DWORD PTR [edx+20],eax
	mov	DWORD PTR [edx+24],eax
	mov	DWORD PTR [edx+28],eax
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

	VMMCall Test_Sys_VM_Handle
	jz	SHORT VMVI_MemAlloced
	mov	[edi.VDD_Get_Mem_Count],0	; no calls to VDD_Get_Mem
;Allocate initial memory for VM as text mode VM
IFDEF	VGA
	mov	eax,12h
ELSE
	mov	eax,10h
ENDIF
	TestMem [edi.VDD_PIF],fVidHghRsGrfxMd	; Q: Alloc hi res mode mem?
	jnz	SHORT VMVI_ModeSet              ;   Y: 
	mov	eax,4
	TestMem [edi.VDD_PIF],fVidLowRsGrfxMd	; Q: Alloc lo res mode mem?
	jnz	SHORT VMVI_ModeSet              ;   Y:
	mov	eax,3				; Assume text mode alloc
VMVI_ModeSet:
	mov	ecx,VDD_Mem_Ini_Tab[eax*4]
	mov	[edi.VDD_Pg.VPH_PgAccMsk],ecx	; set default mem alloc by mode
	mov	ecx,VDD_Mem_Ini_Tab2[eax*4]
	mov	[edi.VDD_Pg.VPH_Pg2AccMsk],ecx
	call	VDD_Mem_AMain			; Allocate main memory
	jc	SHORT VMVI_Err			; Error if can't allocate
VMVI_MemAlloced:

IFDEF	VGA
	call	VDD_Init_DAC                    ; initialize DAC memory
ENDIF

IFDEF	PEGA
; Initialize and allocate shadow memory
	call	VDD_Alloc_PShdw 		; Allocate PSHDW memory
	jc	SHORT VMVI_Err			; Error if can't allocate
ENDIF

;Lastly, disable access to VM's video memory
	SetFlag [edi.VDD_Flags],fVDD_MEna	; Force mem disable
	call	VDD_Mem_Disable
	clc
VMVI_Err:
	ret
EndProc VDD_Mem_VMInit

;*****************************************************************************
;
; VDD_SetEGAMemSize : set ega memory size in BIOS data area
;
; ENTRY: EBX = VM handle
;        EDI = VDD CB ptr
;
; EXIT: BIOS video mem size set for SYS VM and Exclusive/nonbk VM
;
;
;*****************************************************************************
IFNDEF	VGA
BeginProc VDD_SetEGAMemSize,PUBLIC

	VMMCall Test_Sys_VM_Handle		; Q: System VM?
	jz	SHORT VSE_256K			;   Y: Give it 256k EGA
	TestMem [ebx.CB_VM_Status],VMStat_Exclusive ; Q: Exclusive VM?
	jz	SHORT VSE_Exit			    ;   N: Proceed
	TestMem [ebx.CB_VM_Status],VMStat_Background ; Q: Does it run bkgrnd?
	jnz	SHORT VSE_Exit			     ;   Y: Proceed
VSE_256K:
	or	BYTE PTR ds:[487h],060h 	     ;   N: Give VM a 256k EGA
VSE_Exit:
        ret

EndProc VDD_SetEGAMemSize

ENDIF

;******************************************************************************
;VDD_Mem_VMSetTyp
;
;DESCRIPTION:
;	Adjust memory state for new VM type.
; 
;   if (!Grabbing)  {
;       if (Windowed)
;           Update copy state,copy pages with current
;       else if (!Windowed)
;           Deallocate grab memory
;   }
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_Mem_VMSetTyp,PUBLIC

	TestMem [edi.VDD_Flags],fVDD_Grab	; Q: Grabbing?
	jnz	SHORT VMST_Exit			;   Y: Exit
	TestMem [edi.VDD_Flags],fVDD_Win	; Q: Windowed?
	jnz	SHORT VMST_Update		;   Y: Update copy state/memory
	call	VDD_Mem_DCopy			;   N: Dealloc the copy mem
        jmp     SHORT VMST_Exit
VMST_Update:
; Update copy memory
	call	VDD_State_Update		; Update copy w/ current
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
	call	VDD_Mem_Update
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
	cmp	eax,7
	je	SHORT VSCM_Text

; Graphics modes copy mask is mode based
	pop	edx
	pop	esi
	mov	ecx,VDD_Grb_MskTab[eax*4]       
	ret
VSCM_Text:
IFDEF	PEGA
	mov	edx,[edi.VDD_Cur_CRTC]
ELSE
	lea	edx,[edi.VDD_Stt.CRTC]
ENDIF
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

IFDEF	PEGA
	call	VDD_Dealloc_PShdw		; Deallocate PShdw memory
ENDIF

	stc
	ret
EndProc VDD_Mem_VMDestroy

;*****************************************************************************
;
; VDD_CheckMmgrCallOK - Check if it is ok to call the Mmgr 
;                             for allocate/reallocate
;
; ENTRY: NONE
; EXIT:  Carry cleared if it is ok to call mmgr
;        Carry set if it is not
;
; USES:
;
BeginProc VDD_CheckMmgrCallOK

	TestMem [Vid_Flags],fVid_PageSwapInst ; Q: PageSwap device installed?
        jz      SHORT VCM_MmgrCallOK          ;   N:
        VxDcall PageSwap_Test_IO_Valid        ; carry set if I/O invalid.
        ret

VCM_MmgrCallOK:
        clc
        ret

EndProc VDD_CheckMmgrCallOK

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
;	EAX = VDD mode(must be one of 0-6,B,D,E,10 and for VGA 11,12 or 13)
;
;EXIT:	CF = 1 indicates unable to allocate some or all of memory requested
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Mem_AMain,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	VMMCall Test_Sys_VM_Handle		    ; Q: SYS VM?
	jz	VMAM_Done			    ;	Y: not alloc'd memory
	lea	esi,[edi.VDD_Pg]
	mov	[esi.VPH_Mode],al		    ; Set current memory mode
	mov	ecx,[esi.VPH_PgAccMsk]		    ; ECX = 1st 32k accessed
	and	ecx,VDD_MaxAlloc_Pg_Msk[eax*4]	    ; ECX = alloc
	mov	edx,[esi.VPH_Pg2AccMsk] 	    ; EDX = 2nd 32k accessed
	and	edx,VDD_MaxAlloc2_Pg_Msk[eax*4]     ; EDX = alloc
	TestMem [edi.VDD_PIF],fVidTextMd	    ; Q: Alloc text mode mem?
	jz	SHORT VMAM_A0
	or	ecx,30003h
VMAM_A0:
	TestMem [edi.VDD_PIF],fVidLowRsGrfxMd	    ; Q: Alloc lo res mode mem?
	jz	SHORT VMAM_A1
	or	ecx,0F0Fh
VMAM_A1:
	TestMem [edi.VDD_PIF],fVidHghRsGrfxMd	    ; Q: Alloc hi res mode mem?
	jz	SHORT VMAM_A2
IFDEF	EGA
; Enough for MODE 10 application
	or	ecx,VDD_MaxAlloc_Pg_Msk[10h*4]
	or	edx,VDD_MaxAlloc2_Pg_Msk[10h*4]
ENDIF
IFDEF	VGA
; Enough for MODE 12 application
	or	ecx,VDD_MaxAlloc_Pg_Msk[12h*4]
	or	edx,VDD_MaxAlloc2_Pg_Msk[12h*4]
ENDIF
VMAM_A2:
; Now calculate number of pages needed to satisfy alloc masks in ECX,EDX
	call	VDD_CountPages
IFDEF	DEBUG
	test	eax,eax
	jnz	SHORT VMAM_D00
Debug_Out   "VDD: Allocate memory of size 0"
VMAM_D00:
ENDIF
	cmp	[esi.VPH_hMem],0		    ; Q: Memory already alloc'd?
	jnz	SHORT VMAM_ReAlloc		    ;	Y: Do mem realloc

;Do original allocate of memory to VM
	push	eax				    ; Save count
	push	ecx				    ; Save alloc masks
	push	edx
	xor	ecx,ecx
	mov	edx, PageZeroInit+PageLocked
	TestMem [ebx.CB_VM_Status], VMStat_Suspended;Q: VM suspended?
	jz	SHORT VMAM_locked		    ;	N: alloc memory locked
	xor	edx, edx			    ;	Y: alloc unlocked
VMAM_locked:
        call    VDD_CheckMmgrCallOK
	jc	short VMAM_NotNow		    ; Can't make mem calls now....
	VMMCall _PageAllocate,<eax,PG_HOOKED,ebx,ecx,ecx,ecx,ecx,edx>
	or	edx,edx
	jnz	SHORT VMAM_Good
VMAM_NoMainMem:
	or	[edi.VDD_EFlags],fVDE_NoMain
	call	VDD_Msg_NoMainMem
VMAM_NotNow:
	add	esp,12
	pop	esi
	stc
	ret
;Save handle and address and initialize Video Page Handle(VPH) structure
VMAM_Good:
	ClrFlag [edi.VDD_EFlags], <(fVDE_NoMain+fVDE_NoMMsg)>
	ClrFlag [edi.VDD_Flags], fVDD_Save	    ; Need to resave all of mem
	mov	[esi.VPH_hMem],eax		    ; Save memory handle
	mov	[esi.VPH_MState.VDA_Mem_Addr],edx   ; Save memory address
	pop	edx				    ; EDX = 2nd 32k mask
	pop	ecx				    ; ECX = 1st 32k mask
	pop	eax				    ; EAX = count
	call	VDD_Set_VPH			    ; Set up rest of VPH

VMAM_Done:
	pop	esi
	clc
	ret

; Attempt to shrink allocation with saved memory.
;   Allocate all accessed plus currently allocated. This prevents loss of
;	memory that in real mode would still be there.
;	THIS GUARANTEES AL > VPH_PgCnt... preventing infinite loop
VMAM_ReRealloc:
	add	esp,12
	mov	ecx,[esi.VPH_PgAllMsk]
	mov	edx,[esi.VPH_Pg2AllMsk]
	or	ecx,[esi.VPH_PgAccMsk]
	or	edx,[esi.VPH_Pg2AccMsk]
	call	VDD_CountPages			    ; EAX = new alloc count
VMAM_Realloc:

	cmp	edx,[esi.VPH_Pg2AllMsk] 	    ; Q: Any changes to alloc?
	jnz	SHORT VMAM_R0			    ;	Y: Go do it
	cmp	ecx,[esi.VPH_PgAllMsk]		    ; Q: Any changes to alloc?
	jz	VMAM_Done			    ;	N: All done
VMAM_R0:
	push	eax				    ; Save count
	push	ecx
	push	edx
	cmp	al,[esi.VPH_PgCnt]		    ; Q: Allocate same?
	je	SHORT VMAM_HavePages		    ;	Y: No realloc call
						    ;	N: Q: Allocate less?
	ja	SHORT VMAM_R1			    ;	    N: Go realloc
    ;
    ; Shrink memory allocated only if PIF bit for retaining memory is OFF.
    ; When fVidRetainAllo is set, there is more memory than required
    ; and the PgMap array in the page handle structure has contiguous pages
    ; so there is no need to set VPH structure variables.
    ;
	TestMem [edi.VDD_PIF],fVidRetainAllo	    ; Q: Skip shrinking?
	jnz	SHORT VMAM_MemRetained		    ;	Y: retain old mem handle

	TestMem [edi.VDD_Flags],fVDD_MdSvRam	    ; Q: RAM save?
	jnz	VMAM_ReRealloc			    ;   Y: Redo realloc
VMAM_R1:
        call    VDD_CheckMmgrCallOK
	jc	short VMAM_NotNow		    ; Can't make mem calls now....

	VMMCall _PageReallocate,<[esi.VPH_hMem],eax,PageZeroInit+PageLocked>
	or	edx,edx
	jnz	SHORT VMAM_Remapped
        jmp     VMAM_NoMainMem

VMAM_HavePages:
	mov	eax,[esi.VPH_hMem]
	mov	edx,[esi.VPH_MState.VDA_Mem_Addr]
; Successful reallocation, must remap existing data appropriately
VMAM_Remapped:
	ClrFlag [edi.VDD_EFlags], <fVDE_NoMain+fVDE_NoMMsg>
;Look at fVDD_Save only if we are in a mode set
	TestMem [edi.VDD_Flags],fVDD_ModeSet
        jz      SHORT VMAM_PreserveOldPages         
	TestMem [edi.VDD_Flags],fVDD_Save	    ; Q: Data saved in mem?
	jz	VMAM_Good			    ;   N: Map normally
						    ;	Y: Add new pages
VMAM_PreserveOldPages:
	mov	[esi.VPH_hMem],eax		    ; Save memory handle
	mov	[esi.VPH_MState.VDA_Mem_Addr],edx   ; Save memory address
	pop	edx
	pop	ecx
	pop	eax
	call	VDD_ReSet_VPH			    ; Set up rest of VPH
	pop	esi
	clc
	ret
VMAM_MemRetained:
	add	esp,12
	pop	esi
        clc
	ret

; EAX = count of bits set in ECX,EDX
VDD_CountPages:
	xor	eax,eax
	push	ecx
	push	edx
VCPg_1:
	shr	ecx,1
	jz	SHORT VCPg_2
	jnc	VCPg_1
	inc	eax
	jmp	VCPg_1
VCPg_2:
	jnc	SHORT VCPg_3
	inc	eax
VCPg_3:
	shr	edx,1
	jnz	VCPg_2
	jnc	SHORT VCPg_4
	inc	eax
VCPg_4:
	pop	edx
	pop	ecx
	ret

EndProc VDD_Mem_AMain

;******************************************************************************
;VDD_Reset_VPH
;
;DESCRIPTION:
;	Fix up page contents and PgMap after realloc call.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;	ESI = VDD Page Handle ptr
;	EAX = count of pages allocated
;	ECX = allocation map mask for first 32k
;	EDX = allocation map mask for second 32k
;
;EXIT:	CF = 1 indicates unable to allocate some or all of memory requested
;
;USES:	Flags, EAX, ECX, EDX
;
;ASSUMES:
;
BeginProc VDD_ReSet_VPH

	push	ecx
	push	esi
	push	edi
	lea	esi,[esi.VPH_MState.VDA_Mem_PgMap] ; Save current PgMap
	mov	edi, OFFSET32 VDD_PgMap_Temp
	mov	ecx, VPHMax/4
.ERRNZ	VPHMax MOD 4
        cld
	rep movsd
	pop	edi				; Restore VDD CB ptr
	pop	esi				; Restore VDD page handle ptr
	pop	ecx				; Restore alloc map for 1st 32k
	call	VDD_Set_VPH			; Set new map

;;;Trace_Out   "VDD: Move main memory for remap"
; Now we might need to copy memory about...
; Note that when memory is copied it is always copied from a lower ordinal
; page to a higher ordinal page. For instance if a new page is allocated
; for text, which by default has two pages for text:0,1 and two pages for
; font:2,3, the new organization of the pages would be: 0,1,2 text pages
; and 3,4 for the font. Old page 3 must be copied to 4 and then old page
; 2 should be copied to 3. In the case where pages are added onto the end,
; no copying is necessary.
	mov	eax,VPHMax			; EAX = old pages index
.ERRE	VPHMax LT 80h				; sign bit not set for valid pgs
VR_VPH_CopyLoop:
	dec	eax
	jz	SHORT VR_VPH_Done		; page zero is always the same
	movsx	ecx,VDD_PgMap_Temp[eax] 	; ECX = old page for this posn
	movsx	edx,[esi.VPH_MState.VDA_Mem_PgMap][eax] ; EDX = new page
	cmp	cl,dl				; Q: Old >= new?
	jae	VR_VPH_CopyLoop 		;   Y: Check next page
	shl	ecx,12				; ECX = old page offset
	shl	edx,12				; EDX = new page offset
						; Q: New page exist?
	js	VR_VPH_CopyLoop 		;   N: Check next page
	push	esi				;   N: Copy old to new
	push	edi
	mov	edi,[esi.VPH_MState.VDA_Mem_Addr]
	mov	esi,edi
	add	edi,edx 			; EDI = new page address
	add	esi,ecx 			; ESI = old page address
	mov	ecx,4096/4
        cld
	rep movsd				; Copy the page
	pop	edi
	pop	esi
	jmp	VR_VPH_CopyLoop 		; Check next page
VR_VPH_Done:
	ret
EndProc VDD_ReSet_VPH

;******************************************************************************
;VDD_Set_VPH
;
;DESCRIPTION:
;	Fix up page PgMap after alloc or realloc call.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;	ESI = VDD Page Handle ptr
;	EAX = count of pages allocated
;	ECX = allocation map mask for first 32k
;	EDX = allocation map mask for second 32k
;
;EXIT:	CF = 1 indicates unable to allocate some or all of memory requested
;
;USES:	Flags, EAX, ECX, EDX
;
;ASSUMES:
;
BeginProc VDD_Set_VPH

	mov	[esi.VPH_PgCnt],al		    ; pgs alloc'd
	mov	[esi.VPH_PgAllMsk],ecx		    ; Mask of pgs alloc'd
	mov	[esi.VPH_Pg2AllMsk],edx 	    ; Extended memory allocated

	pushfd
	push	ebx
	push	esi
	push	edi
	mov	ebx, ecx			    ; ebx = VPH_PgAllMsk

;
; init page map to all 0FFh's (not allocated)
;
	xor	eax, eax
	dec	eax
	mov	ecx, 64 / 4
	lea	edi,[esi.VPH_MState.VDA_Mem_PgMap]
	cld
	rep	stosd

	inc	al				    ; al = 0
	mov	ah, [esi.VPH_PgCnt]
	lea	edi,[esi.VPH_MState.VDA_Mem_Pg2Map]
	lea	esi,[esi.VPH_MState.VDA_Mem_PgMap]
	mov	ecx, 4				    ; allocate pages for 4 planes
VSV_0:
	push	ecx
	mov	ecx, 8				    ; 8 pages for first 32k
VSV_1:
	shr	ebx,1				    ; Q: This page allocated?
	jnc	SHORT VSV_2			    ;	N: Next page
	mov	[esi], al			    ;	Y: save offset
	inc	al				    ;	   next offset
	cmp	al, ah				    ; Q: Have we done all pages?
	jae	SHORT VSV_allocated		    ;	Y: Mark rest unalloc'd
VSV_2:
	inc	esi				    ; Next page
	loop	VSV_1
	mov	ecx, 8				    ; 8 pages for 2nd 32k
VSV_3:
	shr	edx,1				    ; Q: This page allocated?
	jnc	SHORT VSV_4			    ;	N: Next page
	mov	[edi], al			    ;	Y: save offset
	inc	al				    ;	   next offset
	cmp	al, ah				    ; Q: Have we done all pages?
	jae	SHORT VSV_allocated		    ;	Y: Mark rest unalloc'd
VSV_4:
	inc	edi				    ; Next page
	loop	VSV_3
	pop	ecx
	loop	VSV_0
	jmp	SHORT VSV_exit

VSV_allocated:
	pop	ecx				    ; discard outer loop count

VSV_exit:
	pop	edi
	pop	esi
	pop	ebx
	popfd
	ret

EndProc VDD_Set_VPH

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
	mov	[esi.VPH_Mode],al		    ; set mode
    ;
    ; Load ECX = 1st 32K mask, EDX = 2nd 32K mask
    ;
	mov	ecx,[esi.VPH_PgAccMsk]
	mov	edx,[esi.VPH_Pg2AccMsk]
	and	ecx,VDD_MaxAlloc_Pg_Msk[eax*4]	    
	and	edx,VDD_MaxAlloc2_Pg_Msk[eax*4]	    
	cmp	ecx,[esi.VPH_PgAllMsk]		    ; Q: Any changes to alloc?
	jnz	SHORT VMAC_R0                       ;   Y:
        cmp     edx,[esi.VPH_Pg2AllMsk]             ; Q: Any changes to alloc?
        jz      VMAC_Done 		            ;	N: All done
VMAC_R0:
	push	ecx
        push    edx
	call	VDD_Mem_DCopy
        pop     edx
	pop	ecx

; Now calculate number of pages needed to satisfy alloc masks in ECX,EDX
	call	VDD_CountPages
	test	eax,eax
	jz	SHORT VMAC_Done
;Allocate copy memory to VM
	push	eax				    ; Save count
	push	ecx				    ; Save alloc masks
        push    edx
	xor	ecx,ecx
        call    VDD_CheckMmgrCallOK
	jc	short VMAC_NotNow		    ; Can't make mem calls now....
	VMMCall _PageAllocate,<eax,PG_HOOKED,ebx,ecx,ecx,ecx,ecx,PageZeroInit+PageLocked>
	or	edx,edx
	jnz	SHORT VMAC_Good
VMAC_NoCopyMem:
	SetFlag [edi.VDD_EFlags],fVDE_NoCopy
	call	VDD_Msg_NoCopyMem
VMAC_NotNow:
	add	esp,12
	pop	esi
	stc
	ret

VMAC_Good:
	ClrFlag [edi.VDD_EFlags], <fVDE_NoCopy+fVDE_NoCMsg>
	mov	[esi.VPH_hMem],eax		    ; Save memory handle
	mov	[esi.VPH_MState.VDA_Mem_Addr],edx   ; Save memory address
	pop	edx				    ; EDX = 2nd 32k mask
	pop	ecx				    ; ECX = 1st 32k mask
	pop	eax				    ; EAX = count
	call	VDD_Set_VPH			    ; Set up VDA_Mem_PgMap

%OUT what does this do? \/ \/ \/ xxx
	TestMem [edi.VDD_Flags],fVDD_Win
        jz      SHORT VMAC_Done

	mov	ecx,[edi.VDD_CPg.VPH_PgAccMsk]
        push    edx
	movzx	edx,WORD PTR [edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag]
	test	edx,edx
	jz	SHORT VMAC_GotMask
VMAC_Loop:
	shl	ecx,1
	dec	edx
	jnz	SHORT VMAC_Loop
VMAC_GotMask:
        pop     edx
	or	[edi.VDD_DirtyPages],ecx
	or	[edi.VDD_AccPages],ecx
%OUT what does this do? /\ /\ /\ xxx

VMAC_Done:
	pop	esi
	clc
	ret
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
VDD_Mem_Delete:
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
;CALLS: VDD_Mem_Dealloc
;
;ASSUMES:
;
BeginProc VDD_Mem_DCopy,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	lea	esi, [edi.VDD_CPg]
	jmp	VDD_Mem_Delete
EndProc VDD_Mem_DCopy

;******************************************************************************
;VDD_Mem_Disable    Clear present bit in VMs Page table for VRAM
;
;DESCRIPTION:
;	Clears present bit for all of video memory
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags
;
BeginProc VDD_Mem_Disable,PUBLIC

Assert_VDD_ptrs ebx,edi
IFDEF	PEGA
	TestMem [edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdw  ; Q: Is mem enabled?
	jnz	SHORT VDD_MDis_Enabled		;   Y: disable it
ENDIF
	TestMem [edi.VDD_Flags],fVDD_MEna	; Q: Is mem enabled?
	jz	SHORT VDD_MDis_Ex		;   N: Nothing to do
VDD_MDis_Enabled:
	push	eax
	push	ecx
	push	edx
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0A0h,16,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0B8h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono
	jz	SHORT VMDis_01
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0B0h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
VMDis_01:
ENDIF
	pop	edx
	pop	ecx
	pop	eax
IFDEF	PEGA
	and	[edi.VDD_Stt.V_Flag_PEGA],NOT fVPa_PShdw ; Mem not enabled
ENDIF
	ClrFlag [edi.VDD_Flags], fVDD_MEna	; Mem not enabled
VDD_MDis_Ex:
	ret
EndProc VDD_Mem_Disable

;******************************************************************************
;VDD_Mem_Physical   Map VRAM to physical device
;
;DESCRIPTION:
;	Sets up page table so that VRAM addresses go to physical device.
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_Physical,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	ecx
	push	edx
; First, map all pages of VIDEO memory
	mov	[edi.VDD_MMode],VDD_MMPhys	; Mark VRAM in physical mode
	mov	edx,0A0h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,16,0>
	mov	edx,0B8h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,8,0>
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono
	jz	SHORT VMP_MapDone
	mov	edx,0B0h			; EDX = start of physical VRAM
	VMMCall _PhysIntoV86,<edx,ebx,edx,8,0>
ENDIF
VMP_MapDone:
; Next, map unavailable null memory for all pages that are not logically
;	available in the EGA.
	cmp	[VDD_Msg_VM], 0 		; Q: Message mode?
	jnz	SHORT VMP_NoUnmap		;   Y: Don't unmap pages
	VMMCall Test_Sys_VM_Handle		; Q: System VM?
	jz	SHORT VMP_NoUnmap		;   Y: Don't unmap pages
	test	[edi.VDD_Stt.G_Misc],0Ch        ; Q: Mapped 128K at A000 ?
        jz      SHORT VMP_NoUnMap               ;   Y: Don't unmap pages
	call	VDD_Unmapped			;   N: Unmap some pages
VMP_NoUnmap:
	SetFlag [edi.VDD_Flags],fVDD_MEna	; Memory is enabled
	ClrFlag [edi.VDD_Flags], fVDD_MonoMem
	pop	edx
	pop	ecx
	ret
EndProc VDD_Mem_Physical

;******************************************************************************
;VDD_Mem_Physical2  Map VRAM to 2nd EGA physical device
;
;DESCRIPTION:
;	Sets up page table so that VRAM addresses go to 2nd half of EGA
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:
;
;USES:	Flags, EAX
;
;ASSUMES: Only called for graphics mode apps(memory mapped at A0000h)
;
BeginProc VDD_Mem_Physical2,PUBLIC

	push	ecx
	push	edx
	mov	[edi.VDD_MMode],VDD_MM2nd	; Mark VRAM in 2nd EGA mode
	mov	edx,[VDD_2EGA_Start]		; 2nd EGA start of phys mem
	shr	edx, 12 			; convert phys addr to page #
	VMMCall _PhysIntoV86,<edx,ebx,0A0h,16,0>
	call	VDD_Unmapped			; Unmap subset of pgs
	SetFlag [edi.VDD_Flags],fVDD_MEna	; Memory is enabled
	ClrFlag [edi.VDD_Flags], fVDD_MonoMem
	pop	edx
	pop	ecx
	ret
EndProc VDD_Mem_Physical2

;******************************************************************************
;VDD_Unmapped
;
;DESCRIPTION: Disable non-existant pages and put null memory in other pages
;	that exist but are not available.
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
BeginProc VDD_Unmapped

Assert_VDD_ptrs ebx,edi

	test	[edi.VDD_Stt.G_Misc],8		; Q: Mapped mem start at A0000?
IFDEF	VGAMONO
	jz	VDD_U_Grap			;   Y: go unmap higher memory
ELSE
	jz	SHORT VDD_U_Grap		;   Y: go unmap higher memory
ENDIF
	test	[edi.VDD_Stt.G_Misc],4		;   N: Q: B8000-BFFFF mapped?
	jnz	SHORT VDD_U_CGA 		;	Y: Unmap A0000 for 64k

;Unmap A0000 to AFFFF and B8000 to BFFFF	;	N: B0000-B7FFF is mapped
IFNDEF	VGAMONO
Debug_Out "VDD: VDD does not support MONO mapping"
ENDIF
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0A0h,16,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0B8h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono
	jnz	SHORT VMUnm_01
ENDIF
	ClrFlag [edi.VDD_Flags], fVDD_MEna	; No Mem enabled
VMUnm_01:
	ret
VDD_U_CGA:
;A0000 for 64k not mapped
	xor	eax,eax
	VMMCall _ModifyPageBits,<ebx,0A0h,16,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono
	jz	SHORT VMUnm_02
;B0000 for 32k not mapped
VMUnm_B000:
	xor	eax,eax
	VMMCall _ModifyPageBits,<ebx,0B0h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
VMUnm_02:
ENDIF
	ret

;Graphics mapping, depending on machine state, give VM 64k or 32k+32k not avail
;			    or for VGA, 28k + 36k not available
VDD_U_Grap:
	TestMem [edi.VDD_Flags],fVDD_256	; Q: 256k VM?
	jnz	SHORT VDD_UG_00 		;   Y: Unmap CGA area
; A8000-AFFFF not mapped
IFDEF	VGA
	cmp	[edi.VDD_MMode],VDD_MM2nd	; Q: 2nd EGA?
	jz	SHORT VDD_U_2VGA		;   Y: Unmap 36k
ENDIF
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0A8h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
VDD_UG_00:
; B8000-BFFFF not mapped
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0B8h,8,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono		; If mono supported, unmap
	jnz	SHORT VMUnm_B000		;   B0000-B7FFF also
ENDIF
	ret
IFDEF	VGA
VDD_U_2VGA:
; A7000-AFFFF unmapped
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,0A7h,9,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
	jmp	VDD_UG_00
ENDIF

EndProc VDD_Unmapped


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
	cmp	[edi.VDD_Pg.VPH_Mode],7 	    ; Q: Mode have copy mem?
	ja	SHORT VMU_Done			    ;	N: just clear access
; Need to copy main memory
	mov	eax,[edi.VDD_AccPages]		    ; Adjust for display offset
	and	eax,[edi.VDD_Pg.VPH_PgAllMsk]
	mov	edx,[edi.VDD_AccPages2]
	and	edx,[edi.VDD_Pg.VPH_Pg2AllMsk]
	call	VDD_Adj_Msk			    ; EAX = alloc mask, adjusted
	and	eax,0Fh
	jz	SHORT VMU_Done			    ; No changes to copy
	or	BYTE PTR [edi.VDD_CPg.VPH_PgAccMsk],al
	push	eax
	movzx	eax,[edi.VDD_ModeEGA]
	call	VDD_Mem_ACopy			    ; If needed, alloc more mem
	pop	eax
	and	eax,[edi.VDD_CPg.VPH_PgAllMsk]	    ; Only copy what's alloc'd
	jz	SHORT VMU_Done
	call	VDD_Mem_MakeCopy
VMU_Done:
	xor	eax,eax
	mov	[edi.VDD_AccPages],eax
	mov	[edi.VDD_AccPages2],eax
	popad
	ret
EndProc VDD_Mem_Update


;******************************************************************************
;VDD_Mem_MakeCopy
;
;DESCRIPTION:
;
;ENTRY: EDI = VDD CB ptr
;	EAX = Pages to copy
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_Mem_MakeCopy, PUBLIC
	push	ebx
	mov	ebx,edi
	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_DispPag]
	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_DispPag]

        cld
MMC_01:
	shr	eax,1
	jnc	SHORT MMC_02
	push	esi
	push	edi

	movzx	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_PgMap][esi]
	cmp	esi,0FFh
IFDEF DEBUG
	jne	short MMC_D00
	Debug_Out "VDD:Mem_MakeCopy invalid page index"
MMC_D00:
ENDIF
	je	SHORT MMC_SkipPage
	shl	esi,12
	add	esi,[ebx.VDD_Pg.VPH_MState.VDA_Mem_Addr]

	movzx	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_PgMap][edi]
	cmp	edi,0FFh
IFDEF DEBUG
	jne	short MMC_D01
	Debug_Out "VDD:Mem_MakeCopy invalid page index"
MMC_D01:
ENDIF
	je	SHORT MMC_SkipPage
	shl	edi,12
	add	edi,[ebx.VDD_CPg.VPH_MState.VDA_Mem_Addr]

	mov	ecx,1000h/4
	rep movsd
MMC_SkipPage:
	pop	edi
	pop	esi
MMC_02:
	inc	esi
	inc	edi
	or	al,al
	jnz	MMC_01
	mov	edi,ebx
	pop	ebx
	ret
EndProc VDD_Mem_MakeCopy

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
	movzx	eax,WORD PTR [edi.VDD_Stt.C_AddrH]
	xchg	ah,al				; ECX = display start w/in plane
	test	[edi.VDD_Stt.S_MMode],4 	; Q: Odd/even mode?
	jnz	SHORT VCD_NTxt			;   N: one byte per CRTC addr
VCD_IsTxt:
	shl	eax,1				;   Y: 2 bytes per CRTC address
VCD_NTxt:
	push	eax
	and	eax,0FFFh
	mov	[edi.VDD_CPg.VPH_MState.VDA_Mem_DPagOff],eax ; Offset w/in pg
	mov	[edi.VDD_Pg.VPH_MState.VDA_Mem_DPagOff],eax
	pop	eax
	shr	eax,12
	cmp	eax,32  			; Q: Within 128k max addressable?
	jbe	short MMC_D000			;   Y: Continue
	Debug_Out "VDD:Mem_UpdOff invalid page index"
	xor	eax,eax 			;   N: Set it to ZERO
MMC_D000:
	mov	[edi.VDD_Pg.VPH_MState.VDA_Mem_DispPag],al
	mov	[edi.VDD_CPg.VPH_MState.VDA_Mem_DispPag],0 ; Copy always at 0
	ret
EndProc VDD_Mem_UpdOff

;******************************************************************************
;VDD_Mem_ModPag
;
;DESCRIPTION:
;	Call VDD_Mem_Chg to get all of video modified page bits and then
;	shifts that according to which page is being displayed
;
;ENTRY: EBX = VM_Handle
;	EDI = VDD CB ptr
;
;EXIT:	EAX - bits for corresponding PTEs are set if displayed page modified
;	ZF  - 0 if any displayed page changed
;
;USES:	Flags, EAX
;
BeginProc VDD_Mem_ModPag,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	edx
	push	ecx
	call	VDD_Mem_Chg
	mov	eax,[edi.VDD_AccPages]
	mov	edx,[edi.VDD_AccPages2]
	call	VDD_Adj_Msk			; Adjust bits by start offset
	or	eax,eax
	pop	ecx
	pop	edx
	ret
EndProc VDD_Mem_ModPag

;******************************************************************************
;VDD_Adj_Msk
;
;DESCRIPTION:
;   if (1st 32K displayed)
;	shift right mask in EAX ECX times
;   else if (2nd 32K displayed)
;	shift right mask in EDX by (ECX-8) times
;   Return shifted mask in EAX.
;
;ENTRY: EAX = 1st 32k page mask
;	EDX = 2nd 32k page mask
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
					
	cmp	cl,8			; Q: 1st 32k displayed?
	jb	SHORT VAB_Loop		;   Y: use current value in EAX
	sub	cl,8			;   N: Adjust Display Page number
	mov	eax,edx 		;      use mask for 2nd 32k
	test	ecx,ecx 		; Q: Zero shift?
        jz      SHORT VAB_Exit          ;  Y: No shifting reqd.
VAB_Loop:
	shr	eax,cl			; Adjust pages modified mask
VAB_Exit:
	or	eax,eax
	ret
EndProc VDD_Adj_Msk

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
	movzx	ecx, [edi.VDD_Stt.G_Misc]
	and	cl,0Ch
	cmp	cl,0Ch
	jz	SHORT VMC_Text
	cmp	cl,08h
	jz	SHORT VMC_Text
IFDEF	VGAMONO
	cmp	[edi.VDD_ModeEGA],7
	jz	SHORT VMC_TextMono
ENDIF
	cmp	[edi.VDD_ModeEGA],3
	jbe	SHORT VMC_TextSpcl

; Graphics mode, just scan pages currently mapped
	call	VMC_Scan			; EAX = dirty bits, pg 0 = bit 0
	movzx	edx,ah				; EDX = 2nd 32k bits
	or	eax,eax 			; Q: Any dirty bits?
	jz	SHORT VMC_Done			;   N: Exit
	push	eax
	VMMCall Get_System_Time
	mov	[edi.VDD_ModTime],eax		; Save last time modified
	pop	eax

; Assume all four planes modified
	mov	dh,dl
	push	edx
	shl	edx,16
	or	edx,[esp]			; EDX = 2nd 32k of pages mask
	add	esp,4
	mov	ah,al
	push	eax
	shl	eax,16
	or	eax,[esp]			; EAX = 1st 32k of pages mask
	add	esp,4

VMC_Scanned:
	test	eax,eax 			; Q: Dirty pages first 32k?
	jz	SHORT VMC_Ext			;   N: Merge extended pages
	or	[edi.VDD_DirtyPages],eax	; Set dirty pages
	or	[edi.VDD_AccPages],eax
; Now do extended pages
VMC_Ext:
	test	edx,edx 			; Q: Dirty pages 2nd 32k?
	jz	SHORT VMC_Done			;   N: All done
	or	[edi.VDD_DirtyPages2],edx	;   Y: Merge extended pages
	or	[edi.VDD_AccPages2],edx
VMC_Done:
	mov	eax,[edi.VDD_DirtyPages]	; Get old dirty pages
	test	eax,eax
	ret
; Actually in graphics mode here, but mostly in text mode so we need
;	to save the text mode modified pages.
VMC_TextMono:
	mov	ecx,08h 			; G_Misc = 08h (Mem at B0000h)
	jmp	SHORT VMC_Text
VMC_TextSpcl:
	mov	ecx,0Ch 			; G_Misc = 0Ch (Mem at B8000h)
VMC_Text:
	push	ecx
	mov	edx,0A0h			; First scan A0000-A7FFF
	mov	ecx,8
	call	VMC_ScanFont			; AL = font pages accessed
	pop	ecx
	shl	eax,16				; Font is on page 2
	push	eax
	call	VMC_Scan			; Now scan text pages
	mov	ah,al				; Text is on planes 0 and 1
	or	eax,[esp]			; Merge with font plane
	add	esp,4
	xor	edx,edx 			; No 2nd 32k dirty in text modes
	or	eax,eax 			; Q: Any dirty bits?
	jz	VMC_Done			;   N: Exit
	push	eax
	VMMCall Get_System_Time
	mov	[edi.VDD_ModTime],eax		; Save last time modified
	pop	eax
	jmp	VMC_Scanned

;*******
; This routine scans the page table for dirty bits
;   ENTRY: ECX = G_Misc register masked with 0Ch(just the RAM map bits)
;   EXIT:   AX = bits set for dirty pages, bit 0 = first page
;
VMC_Scan:
	mov	edx,VRAM_PTE_Start[ecx]
	mov	ecx,VRAM_PTE_Cnt[ecx]
;*******
; This routine scans the page table for dirty bits
;   ENTRY: EDX = start page number to scan
;	   ECX = count of pages to scan
;   EXIT:  EAX = bits set for dirty pages, bit 0 = first page
;		    Note that upper 16 bits will always be zero
VMC_ScanFont:
; Copy the page table
	push	edx
	push	ecx
	shl	edx,12
	add	edx,[ebx.CB_High_Linear]
	shr	edx,12
	xor	eax,eax 			; EAX = 0
	VMMCall _CopyPageTable,<edx,ecx,<OFFSET32 VDD_PT_Buff>,eax>

; Form bit mask for dirty pages
	mov	ecx, [esp]
	mov	edx,OFFSET32 VDD_PT_Buff-4
	xor	eax,eax
VMC_Loop:
	bt	DWORD PTR [edx][ecx*4],6
	rcl	eax,1				; Rotate page dirty in EAX
	loop	VMC_Loop
	pop	ecx
	pop	edx

	or	eax, eax			;Q: any dirty pages?
	jz	short VMC_none_dirty		;   N:
	push	eax				;   Y: clear the dirty bits
	xor	eax,eax 			; EAX = 0
	VMMCall _ModifyPageBits,<ebx,edx,ecx,-1,eax,PG_IGNORE,eax>
	pop	eax

VMC_none_dirty:
	ret
EndProc VDD_Mem_Chg

;******************************************************************************
;VDD_Mem_Test_256k
;
;DESCRIPTION:
;	Assumes that Dirty Pages has been updated.  Checks the 2nd pages
;	mask and returns in in EDX.  Will set fVDD_256 if it is non-zero and
;	inform the user that this VM cannot run in the background.
;
;ENTRY: EBX = VM handle
;	EDI = VDD CB ptr
;
;EXIT:	EDX = bits for 2nd 128k PTEs are set if Page Written
;	ZF = 1 if no dirty pages in 2nd 128k
;
;USES:	Flags, EDX
;
BeginProc   VDD_Mem_Test_256k,PUBLIC

IFDEF	VGA
	VMMCall Test_Sys_VM_Handle
	jz	SHORT VMT256_Yes
ENDIF
	mov	edx,[edi.VDD_DirtyPages2]
	or	edx,[edi.VDD_AccPages2]
	jz	SHORT VMT256_Exit
VMT256_Yes:
	bts	[edi.VDD_Flags],fVDD_256Bit
	jc	SHORT VMT256_Exit
	call	VDD_Detach2
	cmp	ebx, [Vid_VM_Handle]	    ; Q: Attached VM?
	je	SHORT VMT256_Exit	    ;	Y: don't do message now!
	call	VDD_Msg_Exclusive
VMT256_Exit:
	ret
EndProc     VDD_Mem_Test_256k

;******************************************************************************
;VDD_Mem_Valid	    Validate current mode with the memory mode
;
;DESCRIPTION:
;	Makes sure current mode is consistent with the current memory mode.
;	Also makes sure that complex graphic modes are emulatable. If not,
;	memory is disabled.
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD ptr
;
;EXIT:	none
;
;USES:	Flags
;
BeginProc VDD_Mem_Valid,PUBLIC

Assert_VDD_ptrs ebx,edi
	TestMem [edi.VDD_Flags],fVDD_MEna
	jnz	SHORT VMV_Chk
	ret
VMV_Chk:
; First, check the mode
	push	eax
	call	VDD_Get_Mode
	cmp	al,7				; Q: CGA mode?
IFDEF	VGAMONO
	jbe	SHORT VMV_CGA			;   Y: make sure CGA/MONO map
ELSE
	jb	SHORT VMV_CGA			;   Y: make sure CGA mapping
ENDIF
	cmp	al,0Bh				; Q: CGA font loading mode?
	je	SHORT VMV_EGA			;   Y: make sure char font map
	cmp	al,10h				; Q: EGA mode?
	ja	SHORT VMV_Invalid		;   N: Higher modes invalid
	cmp	ebx,[Vid_VM_Handle2]		; Q: VRAM in 2nd EGA?
	jz	SHORT VMV_Ex			;   Y: mapping is OK
VMV_Invalid:
	call	VDD_Mem_Disable
VMV_Ex:
	pop	eax
	ret

; CGA mode, make sure memory mapped for CGA, else disable memory
VMV_CGA:
	TestMem [edi.VDD_Flags],fVDD_Font	; Q: mem mapped for font load?
	jnz	VMV_Invalid			;   Y: remap for CGA
	test	[edi.VDD_Stt.G_Misc],08h	; Q: A000 mapping in text mode?
IFDEF	VGAMONO
	jz	SHORT VMV_EGA			;   Y: must be loading font
	TestMem [Vid_Flags],fVid_Mono		;   N: Q: Mono mapping support?
	jnz	SHORT VMV_Ex			;	N: Return
	test	[edi.G_Misc],04h		; Q: B000 mapping in text mode?
	jnz	SHORT VMV_NotMono		;   N: Make sure mem at B8000
	TestMem [edi.VDD_Flags],fVDD_MonoMem	;   Y: Make sure mem at B0000
	jz	SHORT VMV_Invalid
	jmp	SHORT VMV_Ex
VMV_NotMono:
	TestMem [edi.VDD_Flags],fVDD_MonoMem	; Q: Mem at B0000
	jnz	SHORT VMV_Invalid		;   Y: invalid mapping
	jmp	SHORT VMV_Ex			;   N: good mapping
ELSE
	jnz	SHORT VMV_Ex			;	N: Return
ENDIF

; EGA or font load mode, make sure memory mapped for EGA, correct plane
VMV_EGA:
	TestMem [edi.VDD_Flags],fVDD_Font	; Q: Mapped for font load?
	jz	VMV_Invalid			;   N: disable memory
	test	[edi.VDD_Stt.S_Mask],4		; Q: Writing FONT plane?
	jz	VMV_Invalid			;   N: Disable memory
	jmp	SHORT VMV_Ex			;   Y: Return
EndProc VDD_Mem_Valid


;******************************************************************************
;VDD_Mem_Remap	    Remap VRAM after a page fault in non-attached VM
;
;DESCRIPTION:
;	Maps the correct memory for the VM's state and video mode.
;	Map mode specific virtual VRAM for background display VM or window VM.
;	If background display VM or window VM is running in complex graphics
;	    mode, then attach 2nd EGA and map physical memory offset by 8000h.
;	Only called for modes 0-6, B,D,E and 10-13
;		and modes 7,F if VGAMONO
;
;ENTRY: EBX = VM handle
;	EAX(and V_ModeEGA) = mode
;	ECX = page number(relative to mode start) of page fault
;
;EXIT:	CF = 1 indicates cannot remap memory to mode in EAX
;
;USES:	Flags, EAX, ECX, EDX, ESI
;
BeginProc VDD_Mem_Remap,PUBLIC

Assert_VDD_ptrs ebx,edi
	cmp	[edi.VDD_LastMode],al		    ; Q: Same mode as VMDA saw?
	jz	SHORT VDD_MR_0			    ;	N: continue
	mov	[edi.VDD_LastMode],-1		    ;	Y: invalidate the mode
VDD_MR_0:
	cmp	al,11h				    ; Q: Can we map this one?
	jae	SHORT VDD_MR_VGAGrap		    ;	N: No, return CF=1

VDD_MR_01:
	cmp	[edi.VDD_MMode],VDD_MMPhys	    ; Q: Was it in physical?
	je	SHORT VDD_MR_Phys		    ;	Y: go remap the memory

	cmp	[edi.VDD_MMode],VDD_MM2nd	    ; Q: Was it in 2nd EGA?
	je	SHORT VDD_MR_2EGA		    ;	Y: possibly remap memory

	cmp	[edi.VDD_MMode],7		    ; Q: Was it in EGA mode?
IFDEF	VGAMONO
	ja	SHORT VDD_MR_EGA		    ;	Y: check if compatible
ELSE
	jae	SHORT VDD_MR_EGA		    ;	Y: check if compatible
ENDIF

;*******
; VRAM was in CGA mode(0-7), controller must be in that mode or remap memory
VDD_MR_CGA:
	mov	[edi.VDD_MMode],al		    ; Save the current mode
	cmp	al,7				    ; Q: Are we now in CGA mode?
IFDEF	VGAMONO
	ja	SHORT VDD_MR_ToEGA		    ;	N: go remap the memory
ELSE
	jae	SHORT VDD_MR_ToEGA		    ;	N: go remap the memory
ENDIF
	test	[edi.VDD_Stt.G_Misc],08h	    ; Q: Address at A000h?
	jnz	SHORT VDD_MR_CGA0		    ;	N: Regular CGA mode
	mov	al,0Bh				    ;	Y: Must be font load,
	jmp	SHORT VDD_MR_CGA		    ;	    map mode B
VDD_MR_CGA0:
	ClrFlag [edi.VDD_Flags], fVDD_Font
	jmp	SHORT VDD_MR_DoIt

;*******
; VRAM was in unmappable mode, return error
VDD_MR_VGAGrap:
	SetFlag [edi.VDD_Flags],fVDD_256	    ; Indicate needs all of mem
	call	VDD_Msg_Exclusive
	stc
	ret

;*******
; VRAM was in EGA mode, map it to EGA or CGA mode
VDD_MR_EGA:
	mov	[edi.VDD_MMode],al		    ; Save the current mode
	cmp	al,7				    ; Q: Are we now in EGA mode?
IFDEF	VGAMONO
	jbe	SHORT VDD_MR_ToCGA		    ;	N: go remap memory
ELSE
	jb	SHORT VDD_MR_ToCGA		    ;	N: go remap memory
ENDIF
	cmp	al,0Bh				    ; Q: In font load?
	je	SHORT VDD_MR_DoIt		    ;	Y: Go remap memory

;*******
; Mode not emulated, so have to run on hardware somehow
VDD_MR_2ndEGA:
	ClrFlag [edi.VDD_Flags], fVDD_Font
	cmp	ebx,[Vid_VM_Handle2]		    ; Q: Already attached?
	jnz	SHORT VDD_MR_2Att		    ;	N: Go attach
	call	VDD_Restore2			    ;	Y: Do restore
	call	VDD_Mem_Physical2		    ;	    and map the memory
	clc
	jmp	SHORT VDD_MR_2Ex

VDD_MR_2Att:
	call	VDD_Attach2			    ; Attach this VM to 2nd EGA
						    ;	if error, return CF=1
VDD_MR_2Ex:
IFDEF	PEGA
	pushfd
	call	VDD_CMap_PShdw			    ; Map PShdw if necessary
	popfd
ENDIF
	ret

;*******
; VRAM was in 2nd EGA mode, map it to EGA or CGA mode
VDD_MR_2EGA:

;*******
; VRAM was in physical mode, map it to EGA or CGA mode
VDD_MR_Phys:
	mov	[edi.VDD_MMode],al		    ; Save the current mode
	cmp	al,7				    ; Q: Are we now in EGA mode?
IFDEF	VGAMONO
	ja	SHORT VDD_MR_ToEGA		    ;	Y: check if emulatable
ELSE
	jae	SHORT VDD_MR_ToEGA		    ;	Y: check if emulatable
ENDIF
VDD_MR_ToCGA:
	cmp	ebx,[Vid_VM_Handle2]		    ; Q: Attached to 2nd EGA?
	jnz	SHORT VDD_MR_Not2		    ;	N: Continue
	call	VDD_Detach2			    ;	Y: Detach it

VDD_MR_Not2:
	test	[edi.VDD_Stt.G_Misc],08h	    ; Q: Address at A000h?
	jnz	SHORT VDD_MT_CGA0		    ;	N: Regular CGA mode
	mov	al,0Bh				    ;	Y: Must be font load,
	jmp	SHORT VDD_MR_Phys		    ;	    map mode B
VDD_MT_CGA0:
	ClrFlag [edi.VDD_Flags], fVDD_Font
	jmp	SHORT VDD_MR_DoIt		    ;	Remap VRAM for CGA mode

;*******
; To EGA mode, check if we can emulate this
;	Only mode we emulate is mode B, font loading
VDD_MR_ToEGA:
	cmp	al,0Bh				    ; Q: is state mappable?
	jnz	VDD_MR_2ndEGA			    ;	N: Go set up 2nd EGA
	SetFlag [edi.VDD_Flags],fVDD_Font	    ;	Y: mapped for font load

VDD_MR_DoIt:
	cmp	[edi.VDD_Pg.VPH_Mode],al	    ; Q: Mem mode change?
	jz	SHORT VDD_MR_RemapAcc		    ;	N: don't chg mem alloc
	mov	ecx,VDD_Mem_Ini_Tab[eax*4]	    ; ECX = default access
	mov	edx,VDD_Mem_Ini_Tab2[eax*4]	    ; EDX = default access
	TestMem [edi.VDD_Flags],fVDD_ModeSet	    ; Q: During mode set?
	jnz	SHORT VDD_MR_NewAcc		    ;	Y: replace PgAccMsk
	or	ecx, [edi.VDD_Pg.VPH_PgAccMsk]	    ;	N: modify PgAccMsk
	or	edx, [edi.VDD_Pg.VPH_Pg2AccMsk]
VDD_MR_NewAcc:
	mov	[edi.VDD_Pg.VPH_PgAccMsk],ecx
	mov	[edi.VDD_Pg.VPH_Pg2AccMsk],edx
	jmp	SHORT VDD_MR_Realloc

VDD_MR_RemapAcc:
	push	eax
	call	VDD_Mem_Chg
	mov	ecx, eax
	call	VDD_Mem_Test_256k		; EDX = 2nd 128k pages mask
	pop	eax
	cmp	al,3				; Q: Text mode save?
	jbe	SHORT VDD_MR_Text		;   Y: Alloc font and text
IFDEF	VGAMONO
	cmp	al,7				; Q: Text mode save?
	je	SHORT VDD_MR_Text		;   Y: Alloc font and text
ENDIF
	cmp	al,6				; Q: Mode 4,5 or 6?
	ja	SHORT VDD_MR_NotOddEven 	;   N: Alloc all accessed
	and	ecx,0000000FFh			;   Y: Alloc only plane 0
VDD_MR_Text:
	and	ecx,000FF00FFh			; Text, alloc plane 0 and 2
	xor	edx,edx 			; Don't save 2nd 32k
VDD_MR_NotOddEven:
	or	[edi.VDD_PG.VPH_PgAccMsk],ecx	; Indicate memory accessed
	or	[edi.VDD_PG.VPH_Pg2AccMsk],edx

VDD_MR_Realloc:
	call	VDD_Mem_AMain			    ; Allocate mem?
VDD_MR_MapMem:
; Map the plane enabled for write
;   If write plane does not equal read plane, then we must be in character
;   font mode, where the write plane is the one we want.
;
	movzx	eax,[edi.VDD_Stt.S_Mask]	    ; EAX = write plane mask
	and	al,0Fh
	mov	al,VDD_WPlane[eax]		    ; EAX = plane to map
						    ; 8 PTEs/plane
	shl	eax,3				    ; EAX = plane's PTEs offset
	movzx	edx, [edi.VDD_Stt.G_Misc]
	and	edx,0Ch
	mov	ecx,VRAM_PTE_Cnt[edx]		    ; ECX = # of pages to map
	mov	edx,VRAM_PTE_Start[edx] 	    ; EDX = video start pg num
; EAX = page offset into memory handle for plane desired
; EDX = start linear page num to map
; ECX = count of pages to map
	call	VDD_MR_MapIt
	call	VDD_Unmapped			    ; Set PTEs for unmapped mem
	SetFlag [edi.VDD_Flags],fVDD_MEna	    ; Memory is enabled
IFDEF	PEGA
	call	VDD_CMap_PShdw
ENDIF
	clc
	ret
EndProc VDD_Mem_Remap

BeginProc VDD_MR_MapIt
;;Trace_Out "Mapping memory for VM #EBX, #CX pages at #DX, offset by #AX"
	push	esi
	ClrFlag [edi.VDD_Flags], fVDD_MonoMem
	cmp	dl,0B0h
	jnz	SHORT VMRMI_00
	SetFlag [edi.VDD_Flags],fVDD_MonoMem
VMRMI_00:
	push	eax
	push	ecx
	push	edx
	VMMCall _GetNulPageHandle
	mov	esi,eax
	pop	edx
	pop	ecx
	pop	eax
VMRMI_0:
	push	eax
	push	ecx
	push	edx
	mov	ecx,[edi.VDD_Pg.VPH_hMem]
	mov	al,[edi.VDD_Pg.VPH_MState.VDA_Mem_PgMap][eax]
	test	al,al
	jns	SHORT VMRMI_1
	TestMem [edi.VDD_Flags], fVDD_ModeSet	;Q: in INT 10 mode switch?
	jz	SHORT VMRMI_disable_page	;   N: disable page
	mov	ecx,esi 			;   Y: map nul memory
	xor	eax,eax
VMRMI_1:
	VMMCall _MapIntoV86,<ecx,ebx,edx,1,eax,0>
VMRMI_2:
	pop	edx
	pop	ecx
	pop	eax
	inc	edx
	inc	eax
	loopd	VMRMI_0
	pop	esi
	ret

VMRMI_disable_page:
	xor	eax, eax
	VMMCall _ModifyPageBits,<ebx,edx,1,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
	jmp	VMRMI_2

EndProc VDD_MR_MapIt

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
;USES:	Flags, EAX, ECX
;
BeginProc VDD_Mem_MapNull,PUBLIC
	push	edx
	VMMCall _GetNulPageHandle
	pop	edx
	push	edx
	xor	ecx,ecx
	VMMCall _MapIntoV86,<eax,ebx,edx,1,ecx,ecx>
	pop	edx
	ret
EndProc VDD_Mem_MapNull


;******************************************************************************
;VDD_Mem_CalcPage    Calculate the page number
;
;DESCRIPTION:
;	Calculate the video page number from address of PTE
;
;ENTRY: EAX = Number of faulting PTE
;	EBX = VM handle
;	EDI = VM's VDD CB ptr
;
;EXIT:	If Carry clear
;	   ECX = page number within current mode's memory
;	Else page is not within currently enabled memory
;
;USES:	Flags
;
BeginProc VDD_Mem_CalcPage,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	eax
	movzx	ecx, [edi.VDD_Stt.G_Misc]
	and	cl,0Ch
	sub	eax,VRAM_PTE_Start[ecx]
	jc	SHORT mcp_disabled
	cmp	eax, VRAM_PTE_Cnt[ecx]
	jb	SHORT mcp_enabled

IFDEF	DEBUG
mcp_disabled:
	push	ecx
	mov	ecx,VRAM_PTE_Start[ecx]
	add	eax, ecx
Trace_Out   "faulting page, #EAX, not VRAM page, VRAM start at #ECX"
	sub	eax, ecx
	pop	ecx
ENDIF
	clc

mcp_enabled:
	cmc					; clear Carry for pg enabled

IFNDEF DEBUG
mcp_disabled:
ENDIF
	mov	cl,al				; ECX = VRAM PTE number
	pop	eax
	ret
EndProc VDD_Mem_CalcPage

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
;USES:	Flags
;
BeginProc VDD_Mem_Null,PUBLIC

Assert_VDD_ptrs ebx,edi
VEB_MapNull:
	SetFlag [edi.VDD_Flags],fVDD_MEna	; Mem is enabled and null
	VMMCall _GetNulPageHandle
	mov	edx,0A0h
	push	eax
	VMMCall _MapIntoV86,<eax,ebx,edx,16,0,0>
	pop	eax
	push	eax
	mov	edx,0B8h
	VMMCall _MapIntoV86,<eax,ebx,edx,8,0,0>
	pop	eax
IFDEF	VGAMONO
	TestMem [Vid_Flags],fVid_Mono
	jz	SHORT VMN_Exit
	mov	edx,0B0h
	VMMCall _MapIntoV86,<eax,ebx,edx,8,0,0>
ENDIF
VMN_Exit:
	ret
EndProc VDD_Mem_Null


;******************************************************************************
;VDD_Mem_I10Done
;
;DESCRIPTION: This routine clears the dirty bits in the extended memory since
;	we are just completing a mode state initialization and we know what
;	those pages were set to(0 for graphics modes, white spaces for text
;	modes) and will initialize them later if they are really being used.
;	It also checks for modes that we do not support in the background or
;	modes that do not allow 2nd half EGA in background.  Finally, it
;	remaps the memory for full screen VMs.
;
;ENTRY: none
;
;EXIT:	Page table entries cleared for VM
;
;USES:	Flags, EAX, EBX, ECX, EDX, ESI
;
BeginProc VDD_Mem_I10Done,PUBLIC

Assert_VDD_ptrs ebx,edi
	SetFlag [edi.VDD_Flags],fVDD_MInit	; VM's display mem initialized
IFDEF	VGA
	cmp	ebx,[Vid_VM_HandleRun]		; Q: This VM running in ctrlr?
	jnz	SHORT VMI_00			;   N: No need to save state
        call    VDD_Scrn_Off                    ; turn display off to save attr/DAC regs
	call	VDD_SaveCtlr			; Save controller state
	ClrFlag [Vid_Flags], fVid_DOff		; cancel display off
        clc
        call    VDD_RestIndx
VMI_00:
ENDIF
        mov     [edi.VDD_ModeEGA],0FFh          ; invalidate mode and 
	call	VDD_Get_Mode                    ; get current mode
	VMMCall Test_Sys_VM_Handle		; Q: System VM?
IFDEF VGA
	jz	VMI10_Exit
ELSE
	jz	SHORT VMI10_Exit
ENDIF
        call    VDD_Mem_UpdOff                  ; update DispPag and offset
	call	VDD_Mem_Chg			; Clear PTE accessed bits
	TestMem [edi.VDD_Flags],fVDD_MdSvRam	; Q: RAM saved?
	jnz	SHORT VMI_02			;   Y: Keep current alloc masks

; If the RAM was not saved across the mode change, we set all of the accessed
;   masks to a minimum state for the display. We know that the remaining
;   pages are all either ASCII white blanks in text mode or zeroes in graphics.
	movzx	eax,[edi.VDD_ModeEGA]
	mov	ecx,VDD_Mem_Ini_Tab[eax*4]	; Set PTE accessed masks
%OUT Check this out for VGAMONO
        cmp     al,3
        ja      SHORT VMI_NotText1
        push    eax
        mov     eax,[edi.VDD_DirtyPages]
        or      eax,[edi.VDD_AccPages]
        and     eax,00FF0000h                   ; get accessed font pages
        or      ecx,eax        
        pop     eax
VMI_NotText1:
IFDEF	VGA
	lea	esi,[edi.VDD_Pg]
	lea	edx,[edi.VDD_Stt.CRTC]
	push	eax
	push	ecx
	call	VDD_Scrn_Size			;   Y: Text mode processing
	pop	ecx
	pop	eax
        cmp     al,011h
        je      SHORT VMI_NotText
        cmp     al,013h
        je      SHORT VMI_NotText
        
	cmp	[esi.VPH_PgSz],2000h
	jb	SHORT VMI_NotText
	or	ecx, 707h			; requires 3 pages
VMI_NotText:
ENDIF
	mov	[edi.VDD_DirtyPages],ecx	;   according to mode
	mov	[edi.VDD_AccPages],ecx
	mov	[edi.VDD_Pg.VPH_PgAccMsk],ecx
	mov	ecx,VDD_Mem_Ini_Tab2[eax*4]
	mov	[edi.VDD_DirtyPages2],ecx
	mov	[edi.VDD_AccPages2],ecx
	mov	[edi.VDD_Pg.VPH_Pg2AccMsk],ecx
	cmp	ebx,[Vid_VM_Handle]		; Q: Attached VM?
	jnz	SHORT VMI_01			;   N: Don't clear mem saved
	ClrFlag [edi.VDD_Flags], fVDD_Save	;   Y: Mem not saved yet
VMI_01:
%OUT These needed? \/ \/ \/ xxx
;;;	push	eax
	call	VDD_Mem_Amain			; Realloc the memory
;;;	pop	eax
;;;	call	VDD_SetCopyMask
; make accessed pages all of displayed memory
;;;	mov	[edi.VDD_CPg.VPH_PgAccMsk],ecx
;;;	call	VDD_Mem_ACopy			; Realloc the copy memory
%OUT These needed? /\ /\ /\ xxx
VMI_02:
	call	VDD_Mem_Test_256k
VMI10_Exit:
	cmp	ebx,[Vid_VM_Handle]		; Q: Attached VM?
	jz	VDD_Mem_Physical		;   Y: Remap the memory
	ret
EndProc VDD_Mem_I10Done

IFDEF	PEGA
;******************************************************************************
;NOTES ON PARADISE SHADOW MEMORY
;   This is scratchpad memory that the BIOS uses to keep track of register
;   and switch values.
;
;   It is allocated and initialized when the VM is created and mapped into the
;   VM's address space when the magic I/O combination is done(see VDDTIO). VM1
;   is trapped to get the original state to initialize each VM with. Note that
;   if VM1 does not access this RAM prior to starting up a VM, this logic will
;   fail miserably as the shadow ram will never get allocated.
;

;******************************************************************************
;VDD_CMap_PShdw     Conditionally map shadow memory
;
;DESCRIPTION: Maps shadow mem into VM's address space if shadow memory enabled
;
;ENTRY: EBX = VM handle
;
;EXIT:	none
;
;USES:	Flags
;
;ASSUMES: PShdw mem has been allocated
;
BeginProc VDD_CMap_PShdw

Assert_VDD_ptrs ebx,edi
	TestMem [Vid_Flags],fVid_PEGA		    ; Q: PEGA card?
	jnz	SHORT VDD_Is_PEGA		    ;	Y: check if shadow ena
	ret					    ;	N: Don't map shadow mem
VDD_Is_PEGA:
	test	[edi.VDD_Stt.VC_PShdw],fShdwEna
	jnz	SHORT VDD_Map_PShdw
	ret
EndProc VDD_CMap_PShdw

;******************************************************************************
;VDD_Map_PShdw
;
;DESCRIPTION: Maps shadow mem int VM's address space
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags
;
;ASSUMES: PShdw mem has been allocated
;
BeginProc VDD_Map_PShdw,PUBLIC

Assert_VDD_ptrs ebx,edi
	TestMem [Vid_Flags],fVid_PEGA
IFDEF	DEBUG
	jnz	SHORT VMPS_DBG0
Debug_Out "Map unallocated shadow memory"
VMPS_DBG0:
ENDIF
	jz	SHORT VMPS_Ex
	SetFlag [edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdw
	push	eax
	push	ecx
	push	edx
	call	VDD_Mem_Chg			    ; Save which pages changed
	xor	eax,eax
	VMMCall _MapIntoV86,<[edi.VDD_hPMem],ebx,0A0h,1,eax,eax>
	pop	edx
	pop	ecx
	pop	eax
VMPS_Ex:
	ret
EndProc VDD_Map_PShdw

;******************************************************************************
;VDD_Unmap_PShdw     Unmap shadow memory from VM's address space
;
;DESCRIPTION: Restore's PTE to what it would be if shadow memory did not exist.
;
;ENTRY: EBX = VM handle
;
;EXIT:	none
;
;USES:	Flags
;
;ASSUMES: PShdw mem has been allocated
;
BeginProc VDD_Unmap_PShdw,PUBLIC

Assert_VDD_ptrs ebx,edi
	btr	[edi.VDD_Stt.V_Flag_PEGA],fVPa_PShdwBit
	jnc	SHORT VMDW_Ex
	push	eax
	push	ecx
	push	edx
	TestMem [edi.VDD_Flags],fVDD_MEna	; Q: Is mem enabled?
	jz	SHORT VMDW_Dis			;   N: Just disable this memory
	cmp	ebx,[Vid_VM_Handle]		; Q: Attached VM?
	jz	SHORT VMDW_Phys 		;   Y: remap to physical mem
	cmp	ebx,[Vid_VM_Handle2]		; Q: Attached to 2nd EGA?
	jz	SHORT VMDW_Phys2		;   Y: remap to physical mem
; For background or windowed VM, force remap if access memory
VMDW_Dis:
	xor	eax,eax
	VMMCall _ModifyPageBits,<ebx,0A0h,1,<NOT P_AVAIL>,eax,PG_HOOKED,eax>
VMDW_Ret:
	pop	edx
	pop	ecx
	pop	eax
VMDW_Ex:
	ret
VMDW_Phys2:
	mov	edx,[VDD_2EGA_Start]		; 2nd EGA start of phys VRAM
	shr	edx,12				; Make it page number
	jmp	SHORT VMDW_Phys3
VMDW_Phys:
	mov	edx,0A0h			; EDX = start page of VRAM
VMDW_Phys3:
	xor	eax,eax
	VMMCall _PhysIntoV86,<edx,ebx,0A0h,1,eax> ; Map a physical page at A0000
	jmp	VMDW_Ret
EndProc VDD_Unmap_PShdw

;******************************************************************************
;VDD_Alloc_PShdw       Allocate shadow memory
;
;DESCRIPTION: Allocates PEGA shadow memory page
;
;ENTRY: EBX = VM handle
;
;EXIT:	ZF = 1 if succesful allocate, memory handle in VDD_hPMem
;
;USES:	Flags, EAX, EDX
;
;ASSUMES: Called when physical PSHDW memory enabled
;
BeginProc VDD_Alloc_PShdw, PUBLIC

Assert_VDD_ptrs ebx,edi
	TestMem [Vid_Flags],fVid_PEGA		; Q: PEGA card?
	jz	SHORT VAPS_Exit
;;;Debug_Out "Allocating PSHDW"

        xor     edx,edx
        call    VDD_CheckMmgrCallOK                     
	jc	SHORT VAPS_00			; Can't make mem calls now....
	xor	eax,eax
	mov	edx, PageLocked
	TestMem [ebx.CB_VM_Status], VMStat_Suspended;Q: VM suspended?
	jz	SHORT VAPS_locked		;   N: alloc memory locked
	xor	edx, edx			;   Y: alloc unlocked
VAPS_locked:
	VMMCall _PageAllocate,<1,PG_Hooked,ebx,eax,eax,eax,eax,edx>
VAPS_00:
	mov	[edi.VDD_PMemPtr],edx
	or	edx,edx 			; Q: Succesful allocate?
	jnz	SHORT VDD_APS_Ini		;   Y: Save handle and init
	VMMCall _GetNulPageHandle		;   N: ZF=0, PMem is null
	mov	[edi.VDD_hPMem],eax
	or	eax,eax
VAPS_Exit:
	ret

; Initialize the shadow RAM
VDD_APS_Ini:
	mov	[edi.VDD_hPMem],eax
	push	ecx
	push	esi
	push	edi
	mov	edi,DWORD PTR [edi.VDD_PMemPtr] ; EDI = addr of mem to init
	mov	esi,[VDD_PhysA0000]		;   Y: use physical RAM to init
	VMMCall Test_Sys_VM_Handle		; Q: initializing SYS VM?
	jz	SHORT VDD_IPS0			;   Y: use phys mem to init
	mov	esi,ebx
	VMMCall Get_Sys_VM_Handle
	xchg	esi,ebx
	add	esi,[VDD_CB_Off]		; ESI = VM1 shadow mem
	mov	esi,dword ptr [esi.VDD_PMemPtr]

VDD_IPS0:
	mov	ecx,1024/4			; Shadow memory is 1K at most
	cld
	rep movsd				; Init newly alloc'd shadow mem
	pop	edi
	pop	esi
	pop	ecx
	cmp	eax,eax
	ret
EndProc VDD_Alloc_PShdw

;******************************************************************************
;VDD_Dealloc_PShdw	 Deallocate shadow memory
;
;DESCRIPTION: Deallocates PEGA shadow memory page
;
;ENTRY: EBX = VM handle
;
;EXIT:
;
;USES:	Flags, EAX, ECX, EDX
;
;CALLS:
;
;ASSUMES:
;
BeginProc VDD_Dealloc_PShdw,PUBLIC

Assert_VDD_ptrs ebx,edi
	TestMem [Vid_Flags],fVid_PEGA		; Q: PEGA?
	jz	SHORT VDDDP_Ex
IFDEF	DEBUG
	cmp	[edi.VDD_PMemPtr],0		; Q: Already deallocated?
	jnz	SHORT VDDDP_0			;   Y: very strange
Debug_Out "VDD: Dealloc PShdw - not allocated"
VDDDP_0:
ENDIF
	xor	eax,eax
	mov	[edi.VDD_PMemPtr],eax		; Pointer no longer valid
	VMMCall _PageFree,<[edi.VDD_hPMem],eax>
VDDDP_Ex:
	ret
Endproc VDD_Dealloc_PShdw


;**************************
; VDD_Mem_QPShdw    Did page fault occur on disabled shadow memory?
;
; ENTRY:EBX = VM handle
;	EDI = VM's VDD CB ptr
;	EAX = faulting PTE page number
;
; EXIT: CF=1, ZF=1 wrong mode for PShdw page(not enabled)
;	CF=1, ZF=0 wrong address for PShdw page
;	CF=0, Is PShdw page
BeginProc VDD_Mem_QPShdw,PUBLIC

Assert_VDD_ptrs ebx,edi
	TestMem [Vid_Flags],fVid_PEGA		; Q: PEGA card?
	stc
	jz	SHORT VMQW_Ex			;   N: Return CF=1, ZF=1
	test	[edi.VDD_Stt.VC_PShdw],fShdwEna ; Q: Shadow enabled?
	stc
	jz	SHORT VMQW_Ex			;   N: Return CF=1, ZF=1
	cmp	eax,0A0h			; Q: A0000 page?
	stc
	jnz	SHORT VMQW_Ex			;   N: Return CF=1, ZF=0
	cmp	eax,eax 			;   Y: is PShdw memory PTE
VMQW_Ex:
	ret
EndProc VDD_Mem_QPShdw

ENDIF


VxD_CODE_ENDS

	END

