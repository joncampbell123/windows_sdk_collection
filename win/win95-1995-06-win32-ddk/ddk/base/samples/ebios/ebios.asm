;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

PAGE 58,132
;******************************************************************************
TITLE ebios.asm -
;******************************************************************************
;
;   Title:	ebios.asm
;
;   Version:	1.00
;
;==============================================================================
	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.XLIST
	INCLUDE VMM.INC
	INCLUDE Debug.INC

	Create_EBIOS_Service_Table EQU 1    ; EBIOS service table created
	INCLUDE EBIOS.INC
	INCLUDE SYSINFO.INC
	INCLUDE v86mmgr.inc
	INCLUDE arena.inc
.LIST

;******************************************************************************
;		 V I R T U A L	 D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device EBIOS, 1, 0, EBIOS_Control, EBIOS_Device_ID, EBIOS_Init_Order

;
; High 16 bits is real EBIOS start segment address
; Low 16 bits is above segment address rounded down to a page boundary
;
EBIOS_page equ EBIOS_DDB.DDB_Reference_Data

VxD_PAGEABLE_DATA_SEG

;******************************************************************************
;	       P O S T - I N I T   R E A D - O N L Y   D A T A
;******************************************************************************
; Data written only during initialization.  Afterwards, they are read-only.

	public	EB_Unused_Start,EB_Unused_Size,EB_Num_Pages,EB_NoMap
EB_Unused_Start dd	0
EB_Unused_Size	dd	0
EB_Num_Pages	dd	0

EB_NoMap	db	0	; != 0 if we don't need to do any maps for page

VxD_PAGEABLE_DATA_ENDS

VxD_ICODE_SEG

;******************************************************************************
;
;   EBIOS_Device_Init
;
;   DESCRIPTION:
;
;   ENTRY:	    EBX = SYS VM's handle
;
;   EXIT:	    Carry clear if no error
;
;   USES:	    Nothing
;
;   ASSUMES:
;
;==============================================================================

BeginProc EBIOS_Device_Init

	push	ebp
	mov	ebp, esp
	sub	esp, 9*4		    ; reverve 9 dwords for array
	mov	edi, esp
	VMMCall _Get_Device_V86_Pages_Array <0, edi, 0>

	mov	edx, [EBIOS_page]
	movzx	edx, dx 		    ; Just page number
	mov	ecx, 1			    ; Default size is one

	VMMCall _GetFirstV86Page
	xchg	edx, eax
	cmp	eax, edx		    ;Q: below first V86 page?
	ja	short chk_page_assignment   ;	N:
    ;
    ; EBIOS device starts <= FirstV86Page, assumed size of 1 is "correct".
    ;	We do not need to do any mapping for this page.
    ;
	mov	[EB_Num_Pages],ecx
	inc	[EB_NoMap]
	add	eax, ecx
	dec	eax
	cmp	eax, edx		    ;Q: all pages?
	jbe	init_ChkPart		    ;	Y: don't need to assign them!
	sub	eax, edx		    ;	N: eax = # of pages above
	mov	ecx, eax		    ;	   ecx = # of pages to assign
	mov	eax, edx
	inc	eax			    ;	   eax = first page to assign
	jmp	DEBFAR chk_page_assignment2

chk_page_assignment:
	cmp	eax,0A0h		    ; Above 640k boundary?
	jb	short ComputeSz 	    ; No
    ;
    ; EBIOS is > A000:0. It may be in a UMB. Check it out. If it is in a UMB
    ;	we do not need to manage it as V86MMGR will take care of it.
    ;	Assumed size of 1 is "correct".
    ;
	mov	[EB_Num_Pages],ecx
	push	eax

	VxDCall V86MMGR_Get_Version

	cmp	eax,030Ah
	jb	short chk_page_assignment2p	; We'll have to manage page.
	pop	ecx
	xor	ebx,ebx 			; Global check

	VxDCall V86MMGR_GetPgStatus

	xchg	ecx,eax
	test	ecx,V86PS_UMB			; UMB page?
	mov	ecx,[EB_Num_Pages]
	jz	short chk_page_assignment2	; No we have to manage it
	inc	[EB_NoMap]
	jmp	DEBFAR init_ChkPart

ComputeSz:
	mov	edx,eax

	VMMCall _GetLastV86Page
	xchg	eax,edx
	cmp	eax,edx
IFDEF DEBUG
	ja	short EB10
	debug_out "EBIOS page is above FirstV86Page and below or = LastV86Page????"
EB10:
ENDIF
	jbe	DEBFAR Ebios_Fatal
	cmp	eax,96h
	jb	short SetDefCnt
	mov	ecx,0A0h
	sub	ecx,eax
SetDefCnt:
	mov	[EB_Num_Pages],ecx
	jmp	short chk_page_assignment2

chk_page_assignment2p:
	pop	eax
chk_page_assignment2:
chk_pages:
	bt	[edi], eax		    ;Q: page assigned?
	jc	short page_assigned	    ;	Y: error!
	inc	eax
	loop	chk_pages
					    ; all pages are free, so assign them

	mov	eax, [EBIOS_page]
	movzx	eax, ax 		    ; Just page number
	VMMCall _Assign_Device_V86_Pages <eax, [EB_Num_Pages], 0, 0>   ;global

	or	eax,eax
IFDEF DEBUG
	jnz	short EDID10
	debug_out "_Assign_Device_V86_Pages failed for EBIOS page."
EDID10:
ENDIF
	jz	short Ebios_Fatal
init_ChkPart:
	mov	edx, [EBIOS_page]

	VMMCall _GetFirstV86Page

	cmp	dx, ax			    ;Q: below first V86 page?
	jb	short init_done 	    ;  Y:
	cmp	dx, 0A0h		    ;Q: Above A0h?
	jae	short init_done 	    ;  Y:
	test	edx,0FFFF0000h		    ;Q: partial EBIOS situation?
	jz	short init_done 	    ;  N:
	mov	eax,edx
	shr	eax,16			    ; Size in paras of unused part
	movzx	edx,dx
	shl	edx,8			    ; Page number to segment
	mov	[EB_Unused_Start],edx
	mov	[EB_Unused_Size],eax
init_done:
	mov	esp, ebp
	pop	ebp

no_EBIOS:
	clc
	ret

page_assigned:
IFDEF DEBUG
	shl	esi, 5
	add	eax, esi
	Debug_Out 'EBIOS: page already assigned (#eax)'
ENDIF
Ebios_Fatal:
	Fatal_Error

EndProc EBIOS_Device_Init

VxD_ICODE_ENDS

VxD_LOCKED_CODE_SEG

;******************************************************************************
;
;   EBIOS_Control
;
;   DESCRIPTION:    dispatch control messages to the correct handlers
;
;   ENTRY:
;
;   EXIT:	    Carry clear if no error
;
;   USES:
;
;==============================================================================

BeginProc EBIOS_Control

	Control_Dispatch Device_Init, EBIOS_Device_Init
	Control_Dispatch Sys_VM_Init, EBIOS_Map_pages
	Control_Dispatch Create_VM,   EBIOS_Map_pages
	clc
	ret

EndProc EBIOS_Control

VxD_LOCKED_CODE_ENDS


VxD_PAGEABLE_CODE_SEG

BeginDoc
;******************************************************************************
;
;   EBIOS_Get_Version
;
;   DESCRIPTION:    Get EBIOS device version and location/size of EBIOS pages
;
;   ENTRY:
;
;   EXIT:	    IF Carry clear
;			EAX is version
;			EDX is page #
;			ECX is # of pages
;		    ELSE EBIOS device not installed, and EBIOS pages are
;			not allocated
;
;   USES:	FLAGS,EAX,ECX,EDX
;
;==============================================================================
EndDoc

BeginProc EBIOS_Get_Version, SERVICE, RARE

	mov	eax, 200h
	mov	edx, [EBIOS_page]
	movzx	edx, dx 		    ; Just page number
	mov	ecx, [EB_Num_Pages]
	clc
	ret

EndProc EBIOS_Get_Version

BeginDoc
;******************************************************************************
;
;   EBIOS_Get_Unused_Mem  **<EBIOS  VERSION must be >= 2.00>**
;
;   DESCRIPTION:    Get information about part of the EBIOS region which
;		    is not used by EBIOS.
;
;		    On some machines the EBIOS region is not a multiple
;		    of pages in size on a page boundary. Because of the
;		    requirement for the EBIOS region to be rounded to page
;		    boundaries (system performance related issue), part of
;		    the EBIOS region may be unused. This service IDs this
;		    region.
;
;		    NOTE: That in cases where the EBIOS region is below
;			  FirstV86Page this service must return with
;			  carry set.
;
;		    The use of this service is intended for the DOSMGR
;		    device so that the unused portion of the EBIOS region
;		    can be assigned to one of the VMs (typically the SYS VM)
;		    and get used for something. This only occurs if the region
;		    is contiguous with the main V86 memory (start of unused
;		    region is (LastV86Page + 1) SHL 12). NOTE HOWEVER that
;		    it is UP TO THE CALLER to evaluate this. This service
;		    must not fail to ID the unused EBIOS region just because
;		    LastV86Page + 1 doesn't butt up against the unused EBIOS
;		    region.
;
;   ENTRY:	    None
;
;   EXIT:	    Carry Clear
;			EAX = SEGMENT address of start of unused EBIOS region
;			ECX = size of region in PARAGRAPHS (may be 0 indicating
;			      that there is no unused EBIOS region).
;			Zero Flag Set
;			    Unused EBIOS region is GLOBAL. The region is in
;				the SAME physical page in ALL VMs.
;			Zero Flag Clear
;			    Unused EBIOS region is LOCAL. The region is in
;				a different physical page in each VM.
;		    Carry Set
;			EBIOS device not installed, OR All EBIOS memory
;			is used by EBIOS (no unused EBIOS memory).
;
;   USES:
;	EAX,ECX,FLAGS
;
;==============================================================================
EndDoc

BeginProc EBIOS_Get_Unused_Mem, SERVICE, RARE

	xor	eax,eax 		; SET ZERO (also clear carry),
					;   EBIOS is global
	mov	eax,[EB_Unused_Start]
	mov	ecx,[EB_Unused_Size]	; Is 0 if no unused EBIOS
	ret

EndProc EBIOS_Get_Unused_Mem

;******************************************************************************
;
;   EBIOS_Map_pages
;
;   DESCRIPTION:    Map EBIOS pages into the VM
;
;   ENTRY:	    EBX = Handle of VM being initialized
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc EBIOS_Map_pages, VMCREATE

	cmp	[EB_NoMap],0		; Need to map?
	jne	short EB_Map_Done	; No
	mov	eax, [EBIOS_page]
	movzx	eax, ax 		; Just page number
	VMMCall _PhysIntoV86 <eax, ebx, eax, [EB_Num_Pages], 0>
EB_Map_Done:
	clc
	ret

EndProc EBIOS_Map_pages

VxD_PAGEABLE_CODE_ENDS


;******************************************************************************
;******************************************************************************
;
; Real mode initialization code
;
;******************************************************************************

VxD_REAL_INIT_SEG

BeginProc ebios_init

	test	bx, Duplicate_Device_ID
	jnz	no_ebios_fnd	    ; don't load if an ebios device has already
				    ; loaded!

	stc			    ; set carry so that if the BIOS doesn't
				    ; modify anything, then we will recognize
				    ; a failure
    ;
    ; Some machines have EBIOS but do not implement the EBIOS int 15h APIs.
    ;
IFNDEF	ARBEBIOS
	mov	ah, 0C0h
	int	15h
	jc	short no_StdEbios_fnd  ; jump if carry signifies no support
	or	ah, ah
	jnz	short no_StdEbios_fnd  ; jump if ah wasn't set to 0
	test	es:[bx.SD_feature1], SF1_EBIOS_allocated
	jz	short no_StdEbios_fnd
	xor	ax, ax
	mov	es, ax
	mov	ah, 0C1h	    ; get segment adr of EBIOS
	int	15h
	jc	short no_StdEbios_fnd
	mov	ax, es		    ; get EBIOS segment address
	or	ax, ax
	jz	short no_StdEbios_fnd	; jump if es = 0

;
;   Make sure Int 12h agrees with the EBIOS start.  If not then there is no
;   unused memory in the EBIOS page
;
	push	ax
	int	12h
	shl	ax, 10-4	    ; Convert size from K to segment (*1024/16)
	pop	dx
	cmp	dx, ax		    ; Q: End of DOS land = Start of EBIOS?
	mov	ax, dx		    ; (does not change flags)
	je	SHORT DOEbios	    ; Must exactly match to use extra
	xor	dx, dx		    ; Eat whole page -- No partial EBIOS free

    ;
    ; AX = para address of EBIOS
    ;
DOEbios:
	shr	ax, 8		    ; convert to a page #
	and	dx, 0000000011111111B  ; Para offset to REAL start of EBIOS
ELSE
    ;
    ; Under ARBEBIOS Always EBIOS at page 9F
    ;
DOEbios:
	mov	ax, 9Fh
	xor	dx, dx
ENDIF
	shl	edx, 16 	    ; Para offset to REAL start in high 16 bits
	mov	dx, ax		    ; REF data is EBIOS page # and offset
	mov	bx, OFFSET exc_ebios_page
	mov	cx, 1		    ; Default size is 1 page
	cmp	ax, 096h	    ; In last 40K of 640K DOS land?
	jb	short NoPrivMark    ; NO, is low memory EBIOS page
	cmp	ax, 0A0h	    ; Above or = A0 (probably in a UMB)?
	jae	short NoPrivMark    ; Yes, no private mark
	mov	cx, 0A0h
	sub	cx, ax		    ; Size in pages
	push	bx
PMrkLp:
	mov	[bx], ax	    ; Mark page as private to EBIOS
	inc	bx
	inc	bx
	loop	PMrkLp
	pop	bx
	jmp	short YesPrivMark

NoPrivMark:
	xor	bx, bx		    ; No priv mark if low mem EBIOS
YesPrivMark:
	xor	si, si
	mov	ax, Device_Load_Ok
	jmp	short init_exit

    ;
    ; Try to detect HOSEBAG EBIOS machines by checking to see if DOS 640k
    ;	land doesn't actually end at 640k (A000) like it should
    ;
    ;	NOTE THAT THERE ARE SOME MACHINES THAT IMPLEMENT THE INT 15 API
    ;	BUT LIE!!!!!! LIKE AT&T.
    ;
no_StdEbios_fnd:
	mov	ax,5100h	    ; Get current PSP (win386 PSP)
	int	21h
	mov	es,bx
	mov	ax,word ptr es:[2]
	cmp	ax,0A000h	    ; Ends at 640k like it should?
	jae	short no_ebios_fnd  ; Yes, no EBIOS
	cmp	ax,9600h	    ; This is 600k ( > 40k EBIOS is very unlikely)
	jb	short no_ebios_fnd
        mov     dx,ax
        and     dx,00FFh
        cmp     dx,00FFh            ; Is it exactly one para at end of page?
	jne	short DOEbios	    ; No, assume EBIOS
    ;
    ; The end of DOS land is one paragraph short of the last page. NOTE: We
    ;   no longer hard-code this test to 9FFFh, because we need to check
    ;   the last para in the SAME way loader\load.asm does, otherwise we may
    ;   end up with an EBIOS page # that conflicts with Last_VM_Page.
    ;
    ; This might be due to a guy like DOS 5 who has a wraith arena here for
    ;   linking in the high memory UMBs. Check if the paragraph contains an
    ;   arena signature and an owner of 8 (magic "system owner").
    ;
	push	es
	mov	es,ax
	cmp	es:[arena_signature],arena_signature_normal
	jne	short DoEbiosP
	cmp	es:[arena_owner],8
DoEbiosP:
	pop	es
	jne	short DOEbios
no_ebios_fnd:
	xor	bx, bx
	xor	si, si
	xor	edx, edx	    ; ref data is 0
	mov	ax, Abort_Device_Load OR No_Fail_Message
init_exit:
	ret

exc_ebios_page	dw  0, 0, 0, 0, 0, 0, 0, 0, 0

EndProc ebios_init


VxD_REAL_INIT_ENDS


	END ebios_init
