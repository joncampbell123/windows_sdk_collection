PAGE 58,132
;******************************************************************************
TITLE VFD.ASM - Virtual Floppy Disk Device
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp. 1986-1990.  All rights reserved.
;
;   Module:   VFD.ASM - Virtual Floppy Disk Controller Driver
;
;   Version:  2.00
;
;   Date:
;
;   Author:   RP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   14-Dec-1989 RAL Added hack for IBM Model 70 Floppy FIFO garbage
;   10-Jan-1990 RAP Removed FIFO hack - it isn't needed in 3.0, because the
;		    physical DMA is set after the channel is unmasked, so it
;		    is ready as soon as the FIFO is enabled and the FIFO can
;		    get the right data when it does its initial data grab
;   15-Oct-1991 RAL Added code to set Compaq machines to HIGH speed
;
;==============================================================================
	.386p


;******************************************************************************
;
;   This Device hooks INT 13's and checks for the high bit to be clear in the
;   drive specification (DL).  If the bit is clear, then Begin_Critical_Section
;   is called and timer port trapping is disabled.  When the bios INT 13 handler
;   IRET's, then timer port trapping is re-enabled and End_Critical_Section
;   is called.	INT 13's that do have the high bit of DL set are simply
;   reflected so that VHD can handle them.
;
;   Also if the INT 13 is a format command (AL=5), then VDMAD is called to
;   disable DMA channel #3.  This is done, because the BIOS programmes the
;   the DMA channel for a large random DMA transfer that never occurs (the
;   disk controller never actually performs DMA during a format track operation)
;   By ignoring this random programming of the DMA channel, VDMAD is able to
;   work with allocating a smaller (more realistic) DMA buffer.
;
;******************************************************************************



;******************************************************************************
;			L O C A L   C O N S T A N T S
;******************************************************************************

	.XLIST
	INCLUDE VMM.Inc
	INCLUDE VTD.Inc
	INCLUDE VDMAD.Inc
	INCLUDE Debug.Inc
	INCLUDE VPICD.Inc
	INCLUDE OptTest.Inc
	.LIST


;******************************************************************************
;		V I R T U A L	D E V I C E   D E C L A R A T I O N
;******************************************************************************

Declare_Virtual_Device VFD, 2, 0, VFD_Control, VFD_Device_ID, VFD_Init_Order


VFD_INT13_Params_Struc struc
INT13_AL    db	?
INT13_AH    db	?
VFD_INT13_Params_Struc ends

INT13_AX equ word ptr INT13_AL

VxD_DATA_SEG

EXTRN VFD_Verify_Hack_INI_Switch:BYTE
EXTRN VFD_Enable_Compaq_Slime:BYTE

VFD_CB_Offset		dd  ?
VFD_HighReads_Enabled	dd  ?
VFD_DMA_Handle		dd  ?

VFD_Compaq_Hack 	db  False

VxD_DATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   VHD_Sys_Critical_Init
;
;
;------------------------------------------------------------------------------

BeginProc VFD_Sys_Critical_Init

	mov	eax, 13h
	mov	esi, OFFSET32 VFD_Int_13
	VMMcall Hook_V86_Int_Chain

	mov	eax, 2				; floppy is always DMA chn #2
	mov	esi, OFFSET32 VFD_DMA2_Handler
	VxDCall VDMAD_Virtualize_Channel
	mov	[VFD_DMA_Handle], eax

	push	ebx
	VMMCall _Allocate_Device_CB_Area, <<SIZE VFD_INT13_Params_Struc>, 0>
	test	eax, eax
	jnz	SHORT CB_OK
	Debug_Out "VFD:  Could not alloc control block data area space"
	VMMcall Fatal_Memory_Error

CB_OK:
	mov	[VFD_CB_Offset], eax
	pop	ebx

;
;   Fix for Compaq BIOS.  It won't allow reads in memory above segment E000h.
;   It converts them to DMA verify operations.	This hack converts them BACK!
;
	or	eax, -1
	xor	esi, esi
	mov	edi, OFFSET32 VFD_Verify_Hack_INI_Switch
	VMMCall Get_Profile_Boolean
	mov	[VFD_HighReads_Enabled], eax

;
;   Fix for Zenith BIOS.  Zenith does not unmask the floppy drive when
;   they boot from the hard disk.  This command tells VPICD to make the
;   floppy IRQ GLOBAL.	That is, any VM can get an interrupt on IRQ 6.
;
VFD_Init_Zenith_Hack:
	mov	eax, 6
	xor	ebx, ebx
	VxDcall VPICD_Force_Default_Owner	; Ignore any errors!

;
;   Slime for Compaq computers.  Compaq slows down the entire system when
;   the floppy is being accessed so that they can run some stupid copy protected
;   programs.  We don't really care about these stupid programs, so we will
;   force this behivior off.
;
	or	eax, -1
	xor	esi, esi
	mov	edi, OFFSET32 VFD_Enable_Compaq_Slime
	VMMcall Get_Profile_Boolean
	test	eax, eax
	jz	SHORT VFD_Init_Exit

;
;   Look for 03COMPAQ in the BIOS at F000:FFE8h to detect a Compaq machine
;
	Begin_Touch_1st_Meg
	cmp	DWORD PTR ds:[0FFFE8h], 'OC30'
	End_Touch_1st_Meg
	jne	SHORT VFD_Init_Exit

	Begin_Touch_1st_Meg
	cmp	DWORD PTR ds:[0FFFECh], 'QAPM'
	End_Touch_1st_Meg
	jne	SHORT VFD_Init_Exit

	in	al, 86h
	mov	ah, al
	and	ah, 11000000b
	cmp	ah, 0C0h
	jne	SHORT VFD_Init_Exit
	and	al, 3Fh
	or	al, 40h
	IO_Delay
	out	86h, al
	mov	[VFD_Compaq_Hack], True
	Trace_Out "VFD NOTICE:  Turning on HIGH processor speed on Compaq"

VFD_Init_Exit:
	clc					; No error
	ret

EndProc VFD_Sys_Critical_Init

VxD_ICODE_ENDS


VxD_CODE_SEG

;******************************************************************************
;
;   VFD_Control
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VFD_Control

	Control_Dispatch Sys_Critical_Init, VFD_Sys_Critical_Init
	Control_Dispatch Sys_Critical_Exit, <SHORT VFD_Sys_Critical_Exit>
	clc
	ret

EndProc VFD_Control

;******************************************************************************
;
;   VFD_Sys_Critical_Exit
;
;   DESCRIPTION:
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VFD_Sys_Critical_Exit

	cmp	[VFD_Compaq_Hack], True
	jne	SHORT VFD_SCE_Exit

	in	al, 86h
	mov	ah, al
	and	ah, 11000000b
	cmp	ah, 40h
	jne	SHORT VFD_SCE_Exit
	or	al, 0C0h
	IO_Delay
	out	86h, al

VFD_SCE_Exit:
	clc
	ret

EndProc VFD_Sys_Critical_Exit


;******************************************************************************
;
;   VFD_Int_13
;
;   DESCRIPTION:    Enter critical section for all floppy related INT 13's and
;		    disable the DMA channel for AT's machines during a format
;		    command since a random DMA transfer is programmed when no
;		    DMA is actually going to take place.
;
;   ENTRY:
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VFD_Int_13, High_Freq

	test	[ebp.Client_DL], 80h		;Q: high bit set?
	jnz	short INT13_exit		;   Y: not for floppy!

	mov	ecx, (Block_Svc_Ints OR Block_Enable_Ints)
	VMMCall Begin_Critical_Section
	VxDCall VTD_Disable_Trapping

	movzx	eax, [ebp.Client_AX]
	mov	esi, [VFD_CB_Offset]
	mov	[esi][ebx.INT13_AX], ax
	cmp	ah, 2				;Q: read command?
	jne	short hook_int13_iret		;   N:
	cmp	[VFD_HighReads_Enabled], 0	;   Y: Q: hack enabled?
	jne	short hook_int13_iret		;	 Y:
	mov	[esi][ebx.INT13_AX], 0		;	 N: 0 saved AX, so
						;	    DMA won't be modified
hook_int13_iret:
	xor	eax, eax
	mov	esi, OFFSET32 VFD_I13_Iret
	VMMCall Call_When_VM_Returns

INT13_exit:
	stc					; Reflect to next handler
	ret

EndProc VFD_Int_13


;------------------------------------------------------------------------------


BeginProc VFD_I13_Iret, High_Freq

	VxDcall VTD_Enable_Trapping
	mov	eax, [VFD_CB_Offset]
	mov	[eax][ebx.INT13_AX], 0

	TestMem [ebp.Client_EFlags], CF_Mask	; If the Int 13h failed, make
	jz	SHORT VFD_i13ret_exit		;   sure the BIOS didn't leave
						;   DMA channel 2 unmasked
	mov	eax, [VFD_DMA_Handle]		;   (some do on some errors).
	VxDcall VDMAD_Get_Virt_State		;   This will free up the DMA
	test	dl, DMA_Masked			;   buffer now instead of
	jnz	SHORT VFD_i13ret_exit		;   waiting for it to timeout.

	or	dl, DMA_Masked			; Set virtual state to masked
	VxDcall VDMAD_Set_Virt_State		;   and let VDMAD handle it.
	VxDcall VDMAD_Default_Handler

VFD_i13ret_exit:
	VMMjmp	End_Critical_Section

EndProc VFD_I13_Iret


;******************************************************************************
;
;   VFD_DMA2_Handler
;
;   DESCRIPTION:    Handles the virtualization of DMA Channel #2, actually
;		    is just checks to see if the VM is in the middle of a
;		    format command, if it is, then the DMA is not virtualized,
;		    else the routine jumps to VDMAD's default handler.
;
;   ENTRY:	    EAX = DMA handle
;		    EBX = VM handle
;
;   EXIT:
;
;   USES:
;
;==============================================================================

BeginProc VFD_DMA2_Handler

	pushad
	mov	edi, [VFD_CB_Offset]
	add	edi, ebx
	movzx	ecx, [edi.INT13_AX]
	jecxz	SHORT vfd_dma_exit

	movzx	edi, ch 		; EDI = INT 13h cmd
	VxDCall VDMAD_Get_Virt_State
	cmp	edi, 5			;Q: DMA during INT 13h format?
	je	SHORT handle_format	;   Y: set correct count
	cmp	edi, 4			;Q: DMA during INT 13h verify?
	je	SHORT vfd_ignore_dma	;   Y: ignore DMA
	cmp	edi, 2
	jne	SHORT vfd_dma_exit

; hack for broken compaq floppy ROM, which doesn't allow for floppy reads to
; linear addresses 0E0000h - 0FFFFFh

	test	dl, DMA_type_read OR DMA_type_write
	jnz	short vfd_dma_exit
	mov	edi, esi
	sub	edi, [ebx.CB_High_Linear]
	cmp	edi, 0E0000h
	jb	short vfd_dma_exit
	Trace_Out 'VFD converting DMA mode #dl (dest = #edi)'
	or	dl, DMA_type_write	    ; convert verify to write
	jmp	short set_virt

handle_format:
	mov	edi, dword ptr ds:[1Eh * 4]
	movzx	ecx, di
	shr	edi, 16
	shl	edi, 4
	add	edi, ecx
	movzx	ecx, byte ptr [edi+4]	    ; sectors per track
	shl	ecx, 2			    ; table consists of 4 bytes per sector

set_virt:
	VxDCall VDMAD_Set_Virt_State

vfd_dma_exit:
	popad
	VxDJmp	VDMAD_Default_Handler	    ; pass on to default handler

vfd_ignore_dma:
	test	dl, DMA_masked		;Q: channel masked?
	jnz	short dh_masked 	;   Y: must have just been masked, so
					;      free the buffer/region

	push	ebx
	push	edx
	xor	bl, bl
	mov	bh, TRUE
	xor	edx, edx
	VxDCall VDMAD_Set_Region_Info
	pop	edx
	pop	ebx
	VxDCall VDMAD_Set_Phys_State
	VxDCall VDMAD_UnMask_Channel
	jmp	short vfd_verify_exit

dh_masked:
	xor	ebx, ebx
	xor	esi, esi
	xor	ecx, ecx
	xor	edx, edx
	VxDCall VDMAD_Set_Region_Info
	VxDCall VDMAD_Mask_Channel
vfd_verify_exit:
	popad
	ret

EndProc VFD_DMA2_Handler


VxD_CODE_ENDS

	END
