PAGE 58,132
;******************************************************************************
TITLE NETBUFF.ASM - Network buffer allocation services
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989
;
;   Title:	NETBUFF.ASM - Network buffer allocation services
;
;   Version:	1.00
;
;   Date:	12-Mar-1989
;
;   Author:	RAL
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   12-Mar-1989 RAL Original
;   17-Jul-1989 RAL Most functionallity moved to V86MMGR
;   27-Oct-1989 RAL Cleaned up and added code for error conditions
;   29-Oct-1989 RAL Finished documentation
;
;==============================================================================

	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

	INCLUDE VMM.Inc
	INCLUDE Debug.Inc
	INCLUDE NBLocal.Inc
	INCLUDE V86MMGR.Inc
	INCLUDE Shell.Inc

;******************************************************************************

	PUBLIC	HCB_Segment

;==============================================================================


VxD_IDATA_SEG

	EXTRN NetBuff_Error_Title:BYTE
	EXTRN NetBuff_Error_Text:BYTE

VxD_IDATA_ENDS



VxD_DATA_SEG

	EXTRN	VN_CB_Offset:DWORD


HCB_Free_List	dd	0			; Init to 0 in case can't alloc
Free_HCB_Next_Ptr   EQU DWORD PTR [0]

HCB_Segment	dw	?


VxD_DATA_ENDS


;******************************************************************************
;		  I N I T I A L I Z A T I O N	C O D E
;******************************************************************************


VxD_ICODE_SEG

;******************************************************************************
;
;   NetBuff_Init_Complete
;
;   DESCRIPTION:
;	Called from the "Init_Complete" system control call.  This procedure
;	allocates a page for hook control blocks, maps it into global V86
;	address space, and adds every HCB to a free list.
;
;   ENTRY:
;	EBX = System VM Handle
;
;   EXIT:
;	If carry flag is clear then
;	    Initialized successfully (Hook Control Blocks allocated)
;	else
;	    ERROR:  Could not allocate memory for / Could not map HCB page
;
;   USES:
;	All except EBX
;
;==============================================================================

BeginProc NetBuff_Init_Complete

	VMMcall _Allocate_Global_V86_Data_Area, <0, <GVDAParaAlign OR GVDAInquire>>
	cmp	eax, 800h
	jb	SHORT NB_IC_Must_Alloc_Page
	mov	edi, eax
	VMMcall _Allocate_Global_V86_Data_Area, <eax, GVDAParaAlign>
	test	eax, eax
IFDEF DEBUG
	jnz	SHORT NB_IC_Okie_Dokie
	Debug_Out "NetBuff Error:  Could not allocate V86 data area after MMGR returned size #EDI"
NB_IC_Okie_Dokie:
ENDIF
	jz	SHORT NB_IC_Must_Alloc_Page
	xchg	eax, edi
	jmp	SHORT NB_IC_Create_Free_List

;
;   Allocate space for the hook control blocks.
;
NB_IC_Must_Alloc_Page:
	VMMcall _PageAllocate, <1,PG_SYS,0,0,0,0,0,PageLocked>
	test	eax, eax
	jz	SHORT NB_IC_Cant_Alloc
	mov	esi, edx			; ESI = Linear addr to map
	mov	ecx, 1000h			; One full page
	VxDcall V86MMGR_Map_Pages		; Map it
	jc	SHORT NB_IC_Cant_Map
	mov	eax, 1000h

;
;   Create a linked list of Hook Control Blocks.
;   At this point EAX = Size of region allocated, EDI = Linear address
;
NB_IC_Create_Free_List:
	shr	edi, 4
	mov	[HCB_Segment], di
	shl	edi, 4

	lea	eax, [edi+eax-SIZE NCB_Struc]
	add	edi, HCB_Header_Size
	mov	[HCB_Free_List], edi
NBI_Free_Loop:
	lea	ecx, [edi+HCB_Size]
	cmp	ecx, eax
	jae	SHORT NBI_End_Free_List
	mov	[edi.HCB_Next], ecx
	mov	edi, ecx
	jmp	NBI_Free_Loop

NBI_End_Free_List:
	mov	[edi.HCB_Next], 0

	clc
	ret


;
;   Unable to map the page.  Handle is in EAX.	Free it and display error msg.
;
NB_IC_Cant_Map:
	VMMcall _PageFree, <eax, 0>

NB_IC_Cant_Alloc:
	mov	eax, MB_OK + MB_ICONEXCLAMATION
	mov	edi, OFFSET32 NetBuff_Error_Title
	mov	ecx, OFFSET32 NetBuff_Error_Text
	xor	esi, esi
	VxDcall Shell_Message

	stc
	ret

EndProc NetBuff_Init_Complete


VxD_ICODE_ENDS


;******************************************************************************
;	   " S E R V I C E S "	 C A L L E D   B Y   V N E T B I O S
;******************************************************************************

VxD_CODE_SEG

;******************************************************************************
;
;   NetBuff_Alloc_HCB
;
;   DESCRIPTION:
;	This procedure allocates a hook control block.	It does not copy
;	any data into the HCB.	Note that the linear address will be in the
;	first Mb but it will be valid in all VM contexts since the HCB
;	memory is mapped globally.
;
;   ENTRY:
;	None
;
;   EXIT:
;	If carry clear the
;	    EAX -> Hook control block
;	else
;	    Error -- Could not allocate HCB
;
;   USES:
;	EAX, Flags
;
;==============================================================================

BeginProc NetBuff_Alloc_HCB, PUBLIC

	mov	eax, [HCB_Free_List]		; EAX -> HCB to allocate
	test	eax, eax			; Q: Empty list?
	jz	SHORT NB_AHCB_Error		;    Y: No free HCBs!
						;    N: Remove it from list
	push	[eax.HCB_Next]			; Put next ptr on stack
	pop	[HCB_Free_List] 		; First free = EAX.Next
	clc					; Success
	ret

NB_AHCB_Error:
	Debug_Out "ERROR:  Unable to allocate Hook Control Block"
	stc
	ret

EndProc NetBuff_Alloc_HCB


;******************************************************************************
;
;   NetBuff_Free_HCB
;
;   DESCRIPTION:
;	This procedure places an HCB back in the free pool.
;
;   ENTRY:
;	EAX -> Allocated HCB
;
;   EXIT:
;	None
;
;   USES:
;	EAX, Flags
;
;==============================================================================

BeginProc NetBuff_Free_HCB, PUBLIC

	push	[HCB_Free_List]
	mov	[HCB_Free_List], eax
	pop	[eax.HCB_Next]
	ret

EndProc NetBuff_Free_HCB


;******************************************************************************
;
;   NetBuff_Map_Buffer
;
;   DESCRIPTION:
;	This procedure maps a specified memory range into an addess space
;	that is addressable in every virtual machine.  If it is successful
;	then it returns a map handle and the V86 Segment:Offset of the
;	mapped memory.
;
;   ENTRY:
;	ECX = Size of buffer in bytes
;	EBX = Cur_VM_Handle
;	ESI = Seg:Offset of buffer to map (Seg in high word, Off in low word)
;	VM in proper state (PM exec or V86 exec) to match ESI
;
;   EXIT:
;	If carry clear
;	    High word of EDI = V86 segment of mapped buffer
;	    Low word of EDI  = V86 offset of mapped buffer
;	    ESI = Map handle (0 if nothing mapped)
;	else
;	    ERROR:  Could not map buffer
;	    ESI = 0
;
;   USES:
;	EDI, ESI, Flags
;
;==============================================================================

BeginProc NetBuff_Map_Buffer, PUBLIC

	jecxz	NB_MB_Nothing_To_Buffer 	; Len 0 means no buffer
	test	esi, esi
	jz	SHORT NB_MB_Nothing_To_Buffer	; 0 pointer means no buffer

	pushad

	Assert_Cur_VM_Handle ebx

	test	[ebx.CB_VM_Status], VMStat_PM_Exec
	jnz	SHORT NB_MB_PM_Pointer

	movzx	eax, si
	shr	esi, 16
	shl	esi, 4
	add	esi, eax
	add	esi, [ebx.CB_High_Linear]
	jmp	SHORT NB_MB_Have_Lin_Addr

NB_MB_PM_Pointer:
	movzx	edi, si
	shr	esi, 16
	push	ecx
	VMMcall _SelectorMapFlat, <ebx, esi, 0>
	pop	ecx
	lea	esi, [eax+edi]

NB_MB_Have_Lin_Addr:
	VxDcall V86MMGR_Map_Pages
	jc	SHORT NB_MB_Error

	mov	[esp.Pushad_ESI], esi
	shl	edi, 12
	shr	di, 12
	mov	[esp.Pushad_EDI], edi

	popad
	clc
	ret

NB_MB_Error:
	Debug_Out "ERROR:  Unable to map network buffer"
	popad
	xor	esi, esi
	mov	edi, esi
	stc
	ret

NB_MB_Nothing_To_Buffer:
	xor	esi, esi			; Clears carry flag too!
	ret

EndProc NetBuff_Map_Buffer


;******************************************************************************
;
;   NetBuff_Release_Buffer
;
;   DESCRIPTION:
;	This procedure "unmaps" memory that was mapped using NetBuff_Map_-
;	Buffer.  Note that it is OK to pass a map handle of zero.  A zero
;	handle will be ignored.
;
;   ENTRY:
;	ESI = Map handle (0 is OK and ignored)
;
;   EXIT:
;	No failure -- Assumes caller knows what he's doing.
;
;   USES:
;	Nothing
;
;==============================================================================

BeginProc NetBuff_Release_Buffer, PUBLIC

	test	esi, esi
	jz	SHORT NB_RB_Exit

	VxDcall V86MMGR_Free_Page_Map_Region

NB_RB_Exit:
	ret

EndProc NetBuff_Release_Buffer



VxD_CODE_ENDS

	END
