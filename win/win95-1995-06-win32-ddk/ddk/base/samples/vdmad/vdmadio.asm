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
TITLE vdmadio.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989, 1990
;
;   Title:	vdmadio.asm
;
;   Version:	1.00
;
;==============================================================================

	.386p

.XLIST
	include VMM.INC
	include VDMAD.INC
	include DMASYS.INC
	include Debug.INC
.LIST

VxD_IDATA_SEG

Begin_VxD_IO_Table VDMAD_IO_Table
	VxD_IO	DMA_B0, VDMAD_Base	; DMA base register for Channel 0
	VxD_IO	DMA_C0, VDMAD_Count	; DMA count register for Channel 0
	VxD_IO	DMA_B1, VDMAD_Base	; DMA base register for Channel 1
	VxD_IO	DMA_C1, VDMAD_Count	; DMA count register for Channel 1
	VxD_IO	DMA_B2, VDMAD_Base	; DMA base register for Channel 2
	VxD_IO	DMA_C2, VDMAD_Count	; DMA count register for Channel 2
	VxD_IO	DMA_B3, VDMAD_Base	; DMA base register for Channel 3
	VxD_IO	DMA_C3, VDMAD_Count	; DMA count register for Channel 3
	VxD_IO	DMA_B4, VDMAD_Base	; DMA base register for Channel 4
	VxD_IO	DMA_C4, VDMAD_Count	; DMA count register for Channel 4
	VxD_IO	DMA_B5, VDMAD_Base	; DMA base register for Channel 5
	VxD_IO	DMA_C5, VDMAD_Count	; DMA count register for Channel 5
	VxD_IO	DMA_B6, VDMAD_Base	; DMA base register for Channel 6
	VxD_IO	DMA_C6, VDMAD_Count	; DMA count register for Channel 6
	VxD_IO	DMA_B7, VDMAD_Base	; DMA base register for Channel 7
	VxD_IO	DMA_C7, VDMAD_Count	; DMA count register for Channel 7

	VxD_IO	DMA1_Status,	 VDMAD_Status
	VxD_IO	DMA1_SoftReq,	 VDMAD_SoftReq
	VxD_IO	DMA1_SingleMask, VDMAD_MaskSingle
	VxD_IO	DMA1_Mode,	 VDMAD_Mode
	VxD_IO	DMA1_CLR_FF,	 VDMAD_CLR_FF
	VxD_IO	DMA1_Reset,	 VDMAD_Reset
	VxD_IO	DMA1_ResetMask,  VDMAD_ResetMask
	VxD_IO	DMA1_Mask,	 VDMAD_Mask

	VxD_IO	DMA2_Status,	 VDMAD_Status
	VxD_IO	DMA2_SoftReq,	 VDMAD_SoftReq
	VxD_IO	DMA2_SingleMask, VDMAD_MaskSingle
	VxD_IO	DMA2_Mode,	 VDMAD_Mode
	VxD_IO	DMA2_CLR_FF,	 VDMAD_CLR_FF
	VxD_IO	DMA2_Reset,	 VDMAD_Reset
	VxD_IO	DMA2_ResetMask,  VDMAD_ResetMask
	VxD_IO	DMA2_Mask,	 VDMAD_Mask

	VxD_IO	DMA_P0, VDMAD_Page	; DMA page register for Channel 0
	VxD_IO	DMA_P1, VDMAD_Page	; DMA page register for Channel 1
	VxD_IO	DMA_P2, VDMAD_Page	; DMA page register for Channel 2
	VxD_IO	DMA_P3, VDMAD_Page	; DMA page register for Channel 3
	VxD_IO	DMA_P5, VDMAD_Page	; DMA page register for Channel 5
	VxD_IO	DMA_P6, VDMAD_Page	; DMA page register for Channel 6
	VxD_IO	DMA_P7, VDMAD_Page	; DMA page register for Channel 7
End_VxD_IO_Table VDMAD_IO_Table
VxD_IDATA_ENDS

VxD_LOCKED_DATA_SEG
EXTRN   VDMAD_Machine_Type:BYTE
VxD_LOCKED_DATA_ENDS


VxD_DATA_SEG

EXTRN VDMAD_Check_TC:DWORD
EXTRN VDMAD_DMA1_status:BYTE
EXTRN VDMAD_DMA2_status:BYTE
EXTRN DMA_Channels:BYTE
EXTRN DMA_Ctrl1:BYTE
EXTRN DMA_Ctrl2:BYTE
EXTRN VDMAD_CB_Offset:DWORD

PUBLIC page_ports
PUBLIC base_ports
PUBLIC count_ports
PUBLIC EISA_high_page_ports
PUBLIC EISA_high_count_ports

page_ports	    db	DMA_P0, DMA_P1, DMA_P2, DMA_P3
		    db	DMA_P4, DMA_P5, DMA_P6, DMA_P7
base_ports	    db	DMA_B0, DMA_B1, DMA_B2, DMA_B3
		    db	DMA_B4, DMA_B5, DMA_B6, DMA_B7
count_ports	    db	DMA_C0, DMA_C1, DMA_C2, DMA_C3
		    db	DMA_C4, DMA_C5, DMA_C6, DMA_C7

; supplemental EISA ports.  Note that channel 4 doesn't exist
EISA_high_page_ports   dw  DMA_E_P0, DMA_E_P1, DMA_E_P2, DMA_E_P3 
                       dw  0,        DMA_E_P5, DMA_E_P6, DMA_E_P7
EISA_high_count_ports  dw  DMA_E_C0, DMA_E_C1, DMA_E_C2, DMA_E_C3 
                       dw  0,        DMA_E_C5, DMA_E_C6, DMA_E_C7 

VxD_DATA_ENDS


VxD_CODE_SEG

EXTRN VDMAD_DMA_Complete:NEAR
EXTRN VDMAD_Get_DMA_Handle:NEAR
EXTRN VDMAD_Partial_DMA:NEAR
EXTRN VDMAD_Get_DMA_Handle_For_Ctrl:NEAR
EXTRN VDMAD_Check_Ctrl_TC:NEAR
EXTRN VDMAD_Read_DMA_Status:NEAR


;******************************************************************************
; I/O TRAPPING
;******************************************************************************


;******************************************************************************
;
;   GetControllerState
;
;   DESCRIPTION:    We have 2 records of DMA information, 1 for each
;		    controller.  Given the I/O port #, determine which
;		    channel the I/O is for and return a pointer to the
;		    the correct controller's info.
;
;   ENTRY:	    DX port address
;
;   EXIT:	    ESI -> to DMA_Controller_State
;		    Carry set, if controller 2
;
;   USES:
;
;==============================================================================

BeginProc GetControllerState

	mov	esi, OFFSET32 DMA_Ctrl1
	cmp	dx, DMA_Ctrl_2		;Q: controller 2?
	jb	short gco_exit		;   N:	  (Carry set)
	add	esi, SIZE DMA_Controller_State	; (clear Carry)
gco_exit:
	cmc				; set Carry, if controller 2
	ret

EndProc GetControllerState

;******************************************************************************
;
;   VDMAD_GetChannel_FromPage
;
;   DESCRIPTION:    Given an DMA Page I/O port address, determine which
;		    channel the I/O is directed to/from.
;
;   ENTRY:	    DX port address
;
;   EXIT:	    ECX = channel # (0-3)
;		    ESI -> DMA_Controller_State
;
;==============================================================================
BeginProc VDMAD_GetChannel_FromPage

	mov	esi, OFFSET32 DMA_Ctrl1

IFDEF DEBUG
	cmp	dx, DMA_Page_Min	;Q: page register?
	jb	short not_page_reg	;   N:
	cmp	dx, DMA_Page_Max	;Q: page register?
	ja	short not_page_reg	;   N:
ENDIF

	push	eax
	mov	edi, OFFSET32 page_ports
	mov	ecx, 8
	mov	al, dl
	cld
	repne	scasb
	pop	eax
	jne	short not_page_reg
	sub	edi, OFFSET32 page_ports + 1
	mov	ecx, edi
	cmp	cl, 4			;Q: chan (4-7)?
	jb	short gcp_exit		;   N:
	sub	cl, 4			;   Y:	switch to controller 2	chn 0-3
	add	esi, SIZE DMA_Controller_State	; point to 2nd controller data
gcp_exit:
	ret

not_page_reg:
	Debug_Out 'VDMAD_GetChannel_FromPage called with #dx not a page register'
	ret

EndProc VDMAD_GetChannel_FromPage


;******************************************************************************
;
;   GetChannel
;
;   DESCRIPTION:    Given an DMA I/O port address, determine which channel the
;		    I/O is directed to/from.
;
;   ENTRY:	    EBX = VM Handle
;		    DX port address
;
;   EXIT:	    ECX = channel # (0-3)
;		    ESI -> DMA_Controller_State
;
;   USES:
;
;==============================================================================
BeginProc GetChannel

;assumptions that this procedure makes:
.ERRE DMA_Ctrl_1 LT DMA_Ctrl_2	  ; assume that controller 1 ports are lower
.ERRNZ DMA_B1 - DMA_B0 - 2	  ; ports sequential, 1 apart
.ERRNZ DMA_B2 - DMA_B0 - 4
.ERRNZ DMA_B3 - DMA_B0 - 6
.ERRNZ DMA_C0 - DMA_B0 - 1
.ERRNZ DMA_C1 - DMA_B0 - 3
.ERRNZ DMA_C2 - DMA_B0 - 5
.ERRNZ DMA_C3 - DMA_B0 - 7

.ERRNZ DMA_B5 - DMA_B4 - 4	  ; ports sequential, 2 apart
.ERRNZ DMA_B6 - DMA_B4 - 8
.ERRNZ DMA_B7 - DMA_B4 - 12
.ERRNZ DMA_C4 - DMA_B4 - 2
.ERRNZ DMA_C5 - DMA_B4 - 6
.ERRNZ DMA_C6 - DMA_B4 - 10
.ERRNZ DMA_C7 - DMA_B4 - 14

	mov	esi, OFFSET32 DMA_Ctrl1

	mov	cx, dx
	cmp	cx, DMA_Ctrl_1_Max	;Q: controller 1?
	jbe	short ctrl_1		;   Y:
	sub	cx, DMA_Ctrl_2
	shr	cx, 1
	add	esi, SIZE DMA_Controller_State	; point to 2nd controller data
ctrl_1: shr	cl, 1			; chan # (0-3) for either controller
	cmp	cl, 4			;Q: base or count port?
	jb	short gc_exit		;   Y:
	xor	ecx, ecx
gc_exit:
	ret

EndProc GetChannel


;******************************************************************************
;
;   Get_VM_FlipFlop_Ptr
;
;   DESCRIPTION:    Return pointer to virtual flipflop state for VM.
;
;   ENTRY:	    EBX = VM Handle
;		    ESI -> DMA_Controller_State
;
;   EXIT:	    EDI -> flipflop byte for ctrl 1 or ctrl 2
;
;   USES:	    None.
;
;==============================================================================
BeginProc Get_VM_FlipFlop_Ptr

	Assert_VM_Handle ebx

	mov	edi, [VDMAD_CB_Offset]
	lea	edi, [edi+ebx.DMA_flipflop]

	cmp	esi, OFFSET32 DMA_Ctrl1
	jz	SHORT gvmff_ret

IFDEF DEBUG
	cmp	esi, OFFSET32 DMA_Ctrl2
	jz	SHORT gvmff_okay
	Debug_Out "VDMAD: GetVMFlipFlopPtr called with invalid ctrl ptr (#ESI)"
gvmff_okay:
ENDIF
	inc	edi

gvmff_ret:
	ret

EndProc Get_VM_FlipFlop_Ptr


;******************************************************************************
;
;   VDMAD_Status
;
;   DESCRIPTION:    Virtualize 8237 Status/Command port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Status

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_WriteCommand>

VDMAD_GetStatus:
	call	GetControllerState
	xor	al, al
	xchg	al, [esi.CTL_status]
	DMA_Q_OUT "Get Status (#al)"
	ret

VDMAD_WriteCommand:
	or	al, al			;Q: valid command?
	jz	short cmd_exit
IFDEF DEBUG
	Debug_Out "VDMAD: attempt to program the DMA with a bad command value #al"
	Fatal_Error
ELSE
	VMMCall Crash_Cur_VM		    ; Will not return
ENDIF

cmd_exit:
	ret

EndProc VDMAD_Status


;******************************************************************************
;
;   VDMAD_Reset
;
;   DESCRIPTION:    Virtualize 8237 Reset command port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Reset

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetReset:
	call	GetControllerState
	xor	al, al
	mov	[esi.CTL_status], al
	mov	[esi.CTL_request], al
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	mov	byte ptr [edi], al

	xor	ecx, ecx		    ; set channels to DMA_single_mode
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	mov	cl, 4
reset_modes:
	mov	[eax.mode], DMA_single_mode
        and     [eax.ext_mode], _16_bit_xfer
	add	eax, SIZE DMA_Channel_Data
	loop	reset_modes

IFDEF allow_partial_virtualization
	mov	[esi.CTL_mode_set], 1111b
ENDIF

	movzx	eax, [esi.CTL_mask]
	mov	[esi.CTL_mask], 0Fh	; mask all channels
	Debug_Out "Reset #dx"
	call	VDMAD_Notify_all
	ret

EndProc VDMAD_Reset


;******************************************************************************
;
;   VDMAD_Mask
;
;   DESCRIPTION:    Virtualize 8237 Mask port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Mask

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetMask:
	Debug_Out "Set Mask #dx=#al"
	call	GetControllerState
	xchg	[esi.CTL_mask], al
	movzx	eax, al
	call	VDMAD_Notify_all
	ret

EndProc VDMAD_Mask


;******************************************************************************
;
;   VDMAD_ResetMask
;
;   DESCRIPTION:    Virtualize 8237 Clear Mask port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_ResetMask

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetResetMask:
	Debug_Out "Reset Mask (#dx)"
	call	GetControllerState
	movzx	eax, [esi.CTL_mask]
	mov	[esi.CTL_mask], 0
	call	VDMAD_Notify_all
	ret

EndProc VDMAD_ResetMask


;******************************************************************************
;
;   VDMAD_Mode
;
;   DESCRIPTION:    Virtualize 8237 Mode port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Mode

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetMode:
	DMA_Q_OUT "Set Mode #bx=#al",,edx

	mov	ah, al
	and	ah, DMA_mode_mask
;;	  cmp	  ah, DMA_cascade	  ;Q: cascade mode?
;;	  je	  short bad_mode	  ;   Y:
;;	  test	al, DMA_AutoInit	;Q: autoinit transfer?
;;	  jnz	short bad_mode		;   Y: we can't handle it!
	test	al, DMA_AdrDec		;Q: reverse transfer?
	jz	short normal_xfer	;   N:
bad_mode:
IFDEF DEBUG
	Debug_Out "VDMAD:  attempt to program DMA controller with an illegal MODE (#al)"
	Fatal_Error
ELSE
	VMMCall Crash_Cur_VM		    ; Will not return
ENDIF
		
normal_xfer:

	xor	ah, ah
	call	GetControllerState
	movzx	ecx, al
	and	ecx, DMA_chan_sel	; isolate channel # bits
					; byte offset to mode byte for channel

IFDEF allow_partial_virtualization
	btr	[esi.CTL_mode_set], cx	; indicate that mode has been set
ENDIF

	xchg	edi, eax
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	xchg	edi, eax		; edi -> channel data

	and	al, NOT DMA_chan_sel	; clear channel # bits
	mov	[edi.mode], al		; store virtual mode
	mov	ah, [edi.ext_mode]
	bt	[esi.CTL_Flags], ecx	;Q: word port?
	jnc	short mode_ctl_1	;   N:
	or	ah, _16_bit_xfer	;   Y: default to word channel
	jmp	short mode_ext_done
mode_ctl_1:
	and	ah, NOT _16_bit_xfer	;   N: default to byte channel
mode_ext_done:
	and	ah, NOT (Programmed_IO + Transfer_Data + Write_Mem)
	test	al, DMA_type_write	;Q: memory write?
	jz	short mode_not_write	;   N:
	or	ah, Write_Mem + Transfer_Data ;Y: transfer to memory
	jmp	short mode_ext_done2
mode_not_write:
	test	al, DMA_type_read	;Q: memory read?
	jz	short mode_ext_done2	;   N:
	or	ah, Transfer_Data	;   Y: transfer from memory
					;ELSE verify mode!!
mode_ext_done2:
    ;
    ; copy undocumented auto-init bit
    ;
	mov	dl, al
	and	dl, DMA_AutoInit
	shr	dl, 3
	and	ah, NOT PS2_AutoInit
	or	ah, dl
	mov	[edi.ext_mode], ah	; store new extended mode

	xor	edx, edx		; check mask bit
	CallRet VDMAD_NotifyOwner

EndProc VDMAD_Mode


;******************************************************************************
;
;   VDMAD_SoftReq
;
;   DESCRIPTION:    Virtualize 8237 Software Request port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_SoftReq

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetSoftReq:

IFDEF DEBUG
	test	al, DMA_Set_Request
	jz	short vsr_d00
	Debug_Out "VDMAD: Set SoftReq #dx=#al"
vsr_d00:
ENDIF

	call	GetControllerState
	movzx	ecx, al
	and	cl, DMA_chan_sel	; isolate channel # bits
	shr	al, 2			; shift set/reset bit to bit 0
	and	al, 1			; isolate the set/reset bit
	mov	ah, NOT 1
	shl	al, cl			; move set/reset bit to channel position
	rol	ah, cl			; move clear mask to channel position
	and	ah, [esi.CTL_request]	; reset channel's bit
	or	al, ah			; or request bit into request register
	mov	[esi.CTL_request], al	; store result
	or	edx, 1			; always notify
	CallRet VDMAD_NotifyOwner

EndProc VDMAD_SoftReq


;******************************************************************************
;
;   VDMAD_MaskSingle
;
;   DESCRIPTION:    Virtualize 8237 Mask single channel port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_MaskSingle

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_MaskCh:
	call	GetControllerState
	movzx	ecx, al
	and	cl, DMA_chan_sel	; isolate channel # bits
	shr	al, 2			; shift set/reset bit to bit 0
	and	al, 1			; isolate the set/reset bit
	mov	ah, NOT 1		; mask for chn 0
	shl	al, cl			; move set/reset bit to channel position
	rol	ah, cl			; mask for chn n
	and	ah, [esi.CTL_mask]	; reset channel's bit
	or	al, ah			; or mask bit into mask register
	mov	[esi.CTL_mask], al	; store result
	DMA_Q_OUT "MaskCh #bx=#al",,edx
	or	edx, 1			; always notify
	CallRet VDMAD_NotifyOwner

EndProc VDMAD_MaskSingle


;******************************************************************************
;
;   VDMAD_CLR_FF
;
;   DESCRIPTION:    Virtualize 8237 Clear flipflop command port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_CLR_FF

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO VDMAD_Bad_Read, Fall_Through

VDMAD_SetClrFF:
	DMA_Q_OUT "Clear FlipFlop (#ax)",edx
	call	GetControllerState
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	mov	byte ptr [edi], 0
	ret

EndProc VDMAD_CLR_FF


;******************************************************************************
;
;   update_adr_byte
;
;   DESCRIPTION:    DMA addresses are kept as 3 byte linear addresses, but
;		    writing new addresses is done a byte at a time, so this
;		    routine handles replacing the correct byte of the linear
;		    address with the new address byte.	Also word DMA channels
;		    still have a linear byte address, but are programmed with
;		    word addresses, so this routine handles shifting the new
;		    address bytes to the left by 1 to align properly with the
;		    virtual byte address.
;
;   ENTRY:	    AL is new adr byte
;		    AH is byte #, 0-low base, 1-high base, 2-page, 3-hi pg
;		    ECX is channel #
;		    ESI points to controller data
;
;   EXIT:
;
;   USES:	    flags
;
;==============================================================================
BeginProc update_adr_byte

	pushad
	mov	bl, ah			; get byte #
	movzx	eax, al 		; make sure high 3 bytes are clear
	mov	edx, 0FFFFFF00h 	; mask
	bt	[esi.CTL_Flags], ecx	;Q: word port?
	jnc	short byte_base 	;   N:

	cmp	bl, 2			;Q: updating page byte?
	je	short mask_a16		;   Y: mask out low bit of page
	ja	short byte_base 	;   N: updating hi-page byte, do nothing
	shl	eax, 1			;   N: shift new byte & mask to adjust
	rol	edx, 1			;      new byte word address to linear
	or	bl, bl			;Q: updating LSB?
	jnz	short byte_base 	;   N:
	and	dl, NOT 1		;   Y: clear bit 0 so A0 is cleared
	jmp	short byte_base 	;      byte address
mask_a16:
	and	al, NOT 1
	or	dl, 1

byte_base:
	push	eax
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	lea	esi, [eax.cur_addr]
	pop	eax
	mov	cl, bl
	shl	cl, 3			; 0 - 0, 1 - 8, 2 - 16, 3 - 24
	shl	eax, cl 		; rotate new byte &
	rol	edx, cl 		; mask into correct byte position
	and	edx, dword ptr [esi]	; get unmasked bits from current adr
	or	edx, eax		; add new byte of adr
;
; For EISA machines, a write to base or low page ports implicitly clears
; the high page port.  Emulate that here by clearing the top 8 bits of the
; computed address if we are not accessing the high page port.
;
        
        cmp     bl,3                    ; Q: high page port?
        je      keep_high_page          ;    N:
        IF_NOT_EISA_JMP <short keep_high_page>
        and     edx, 00FFFFFFh          ;    Y: turn off high eight bits

keep_high_page:
	mov	dword ptr [esi], edx	; save new adr (cur_addr)
	mov	dword ptr [esi+4], edx	; (pgm_addr)
	.errnz	pgm_addr - cur_addr - 4
	popad
	ret

EndProc update_adr_byte


;******************************************************************************
;
;   VDMAD_Base
;
;   DESCRIPTION:    Virtualize 8237 Base port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Base

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO <SHORT VDMAD_GetBase>, Fall_Through

VDMAD_SetBase:
	DMA_Q_OUT "Set Base #bx=#al",,edx
	call	GetChannel
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	mov	ah, [edi]		; 0 for 1st byte, 1 for 2nd byte
	call	update_adr_byte
	xor	ah, 1
	mov	[edi], ah
	jnz	short skip_base_notify	; jump if not 2nd byte
	xor	edx, edx		; check mask bit
	CallRet VDMAD_NotifyOwner

VDMAD_GetBase:
	call	GetChannel
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	inc	edx			; get the count port
	cmp	edx,DMA_Ctrl_1_Max
	jbe	short count_base_diff_1
	inc	edx

count_base_diff_1:
	cmp	byte ptr [edi], 0	; update partial DMA info if reading
	jnz	SHORT return_next_adr	;   low byte of address
        push    ebx
	push	edi			; save flipflop pointer
	call	VDMAD_Partial_DMA	; Do partial DMA, updates cur_addr
	pop	edi
        pop     ebx
return_next_adr:
	call	VDMAD_Get_DMA_Handle_For_Ctrl	; eax -> DMA_Channel_Data

        test    [eax.callbacks], VDMAD_Callbacks_ReadNotify
        jz      SHORT skip_gb_notify
        mov     edx, VDMAD_Callback_ReadBase
        call    VDMAD_NotifyOwner

skip_gb_notify:
	lea	edx, [eax.cur_addr]
	movzx	eax, byte ptr [edi]	; if flipflop is clear, then get low byte
					; else get high byte
	xor	byte ptr [edi], 1	; change the state of the flipflop
	mov	eax, [eax+edx]		; A0-A31 or A8-A31+xxxxxxxx
	bt	[esi.CTL_Flags], ecx	;Q: word port?
	jnc	short get_byte_base	;   N:
	shr	eax, 1			;   Y: convert al = A0-A7 to A1-A8 or
					;		    A8-A15 to A9-A16
get_byte_base:
	DMA_Q_OUT "Get Base #edx=#al"
skip_base_notify:
	ret

EndProc VDMAD_Base


;******************************************************************************
;
;   VDMAD_Count
;
;   DESCRIPTION:    Virtualize 8237 Count port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Count

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO <SHORT VDMAD_GetCount>, Fall_Through

VDMAD_SetCount:
	DMA_Q_OUT "Set Count #bx=#al",,edx
	call	GetChannel
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	push	eax
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	lea	edx, [eax.cur_count]
	pop	eax
	push	ecx
	movzx	ecx, byte ptr [edi]	; if flipflop is clear, then set low byte
					; else set high byte
	xor	byte ptr [edi], 1	; change the state of the flipflop
	mov	[edx+ecx], al		; cur_count
	mov	[edx+ecx+4], al 	; pgm_count
;
; On EISA machines, writes to the count port implicitly clear the 
; high count port.  Emulate that here by clearing the top 8 bits of the
; count.
;
        IF_NOT_EISA_JMP <short keep_high_count>
        mov     byte ptr [edx+2], 0     ; cur_count
        mov     byte ptr [edx+2+4], 0   ; pgm_count
 
keep_high_count:
	pop	ecx
	.errnz	pgm_count - cur_count - 4
	cmp	byte ptr [edi], 0	;Q: both bytes set?
	jne	short skip_cnt_notify	;   N:
	xor	edx, edx		; check mask bit
	CallRet VDMAD_NotifyOwner

VDMAD_GetCount:
	call	GetChannel
	call	Get_VM_FlipFlop_Ptr	; edi -> controller's flipflop state
	cmp	byte ptr [edi], 0	; update partial DMA info if reading
	jnz	SHORT return_next_cnt	;   low byte of count
        push    ebx
	push	edi
	call	VDMAD_Partial_DMA	; Do partial DMA, updates cur_count
	pop	edi
        pop     ebx
return_next_cnt:
	call	VDMAD_Get_DMA_Handle_For_Ctrl
        test    [eax.callbacks], VDMAD_Callbacks_ReadNotify
        jz      SHORT skip_gc_notify
        mov     edx, VDMAD_Callback_ReadCount
        call    VDMAD_NotifyOwner

skip_gc_notify:
	lea	edx, [eax.cur_count]
	movzx	ecx, byte ptr [edi]	; if flipflop is clear, then get low byte
					; else get high byte
	xor	byte ptr [edi], 1	; change the state of the flipflop
	mov	al, [ecx+edx]
	DMA_Q_OUT "Get Count #edx=#al"

skip_cnt_notify:
	ret

EndProc VDMAD_Count


;******************************************************************************
;
;   VDMAD_Page
;
;   DESCRIPTION:    Virtualize 8237 Page port
;
;   ENTRY:	    AL = Byte to be output (if output)
;		    EBX = VM handle
;		    EDX = Port address
;		    ECX = 0 for byte input, 4 for byte output, others emulated
;
;   EXIT:	    AL = Byte input (if input)
;
;   USES:	    Everything
;
;==============================================================================
BeginProc VDMAD_Page

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_SetPage>

VDMAD_GetPage:
	call	VDMAD_GetChannel_FromPage
	call	VDMAD_Get_DMA_Handle_For_Ctrl

        test    [eax.callbacks], VDMAD_Callbacks_ReadNotify
        jz      SHORT skip_gp_notify

        push    edx
        mov     edx, VDMAD_Callback_ReadPage
        call    VDMAD_NotifyOwner
        pop     edx

skip_gp_notify:
	movzx	eax, word ptr [eax.cur_addr+2]	; al = A16-A23
	bt	[esi.CTL_Flags], ecx	;Q: word port?
	jnc	short gp_done		;   N:
	and	al, 11111110b		;   Y: al = address lines A17-A23
gp_done:
	DMA_Q_OUT "Get Page #edx=#al"
	ret

VDMAD_SetPage:
	DMA_Q_OUT "Set Page #bx=#al",,edx

	call	VDMAD_GetChannel_FromPage
	mov	ah, 2			; 3rd byte of address
	call	update_adr_byte
	xor	edx, edx		; check mask bit
	CallRet SHORT VDMAD_NotifyOwner

EndProc VDMAD_Page


;******************************************************************************
;
;   VDMAD_Bad_Read
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

BeginProc VDMAD_Bad_Read

IFDEF DEBUG_notify
	Debug_Out "Read attempted on port #dx"
ENDIF
	in	al, dx
	ret

EndProc VDMAD_Bad_Read

;******************************************************************************
;
;   VDMAD_Notify_all
;
;   DESCRIPTION:    Notify all channels for current controller whose masked
;		    state changed.
;
;   ENTRY:	    EBX = VM handle
;		    ESI is pointer to controller data
;		    EAX old mask
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VDMAD_Notify_all

	movzx	edx, [esi.CTL_mask]
	xor	eax, edx
	mov	edx, 1			; always notify
	mov	ecx, 3
notify_lp:
	bt	ax, cx			;Q: channel masked changed?
	jnc	short dont_call 	;   N:
	call	VDMAD_NotifyOwner	;   Y: notify the owner

dont_call:
	loop	notify_lp
	ret

EndProc VDMAD_Notify_all


;******************************************************************************
;
;   VDMAD_NotifyOwner
;
;   DESCRIPTION:    Notify channel owner of state change
;
;   ENTRY:	    ECX is channel # within controller (0-3)
;		    EBX = VM handle
;		    ESI is pointer to controller data
;		    EDX = 0, notify only if channel is unmasked
;		    EDX = 1, always notify
;
;                   Extended read callbacks use EDX to signal the
;                   callback type:
;                      VDMAD_Callback_ReadCount    
;                      VDMAD_Callback_ReadPage     
;                      VDMAD_Callback_ReadBase     
;
;   EXIT:	    nothing
;
;   USES:	    flags
;
;==============================================================================

BeginProc VDMAD_NotifyOwner

	Assert_Cur_VM_Handle ebx

	pushad
	call	VDMAD_Get_DMA_Handle_For_Ctrl	; EAX = DMA handle

	cmp	[eax.call_back], 0	;Q: call-back defined?
	je	short no_callback	;   N: then just return!

	or	edx, edx		;Q: check mask?
	jne	short nt_notify 	;   N: call the call-back

	movzx	edx, [esi.CTL_mask]
	bt	edx, ecx		;Q: channel masked?
	jc	short nt_masked 	;   Y: don't need to call
nt_notify:				;   N: call owner's call-back
	xor	ecx, ecx
	call	[eax.call_back]

nt_masked:
no_callback:
	popad
	ret

EndProc VDMAD_NotifyOwner


;******************************************************************************
; routines to check and deal with DMA terminal counts at port I/O time
;******************************************************************************

;******************************************************************************
;
;   VDMAD_NoCheck
;
;   DESCRIPTION: 
;
;   ENTRY:       Nothing.                
;
;   EXIT:        Nothing.
;
;   USES:        Nothing.        
;
;==============================================================================
BeginProc VDMAD_NoCheck
        pushfd
        push    eax
        cli
        call    VDMAD_Read_DMA_Status
        pop     eax
        popfd
	ret

EndProc   VDMAD_NoCheck

;******************************************************************************
;
;   VDMAD_IO_Check_TC
;
;   DESCRIPTION:    Check for a channel reaching Terminal Count in either
;		    controller.  If one does, then complete the tranfer.
;		    This routine is called at DMA port I/O time.
;
;   ENTRY: EBX = Current VM Handle.
;
;   EXIT:  None.
;
;   USES:  None.
;
;==============================================================================

BeginProc VDMAD_IO_Check_TC

	pushfd
	pushad

	Assert_Cur_VM_Handle ebx

	pushfd				; don't let HW_Check_TC slip in here
	cli

	call	VDMAD_Read_DMA_Status

	xor	eax, eax		; process all pending TC status
	xchg	al, [VDMAD_DMA1_Status]
	xchg	ah, [VDMAD_DMA2_Status]

	popfd

IFDEF DEBUG
	xchg	al, ah
	DMA_Q_OUT 'VDMAD_IO_Check_TC 1/2:#AX'
	xchg	al, ah
ENDIF
	push	eax

	mov	esi, OFFSET32 DMA_Ctrl1 	; update/complete Ctrl 1 DMA
	call	VDMAD_Check_Ctrl_TC

	pop	eax
	mov	al, ah
	mov	esi, OFFSET32 DMA_Ctrl2 	; update/complete Ctrl 2 DMA
	call	VDMAD_Check_Ctrl_TC

	popad
	popfd
	ret

EndProc VDMAD_IO_Check_TC

VxD_CODE_ENDS

END

