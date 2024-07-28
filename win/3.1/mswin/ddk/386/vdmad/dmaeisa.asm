PAGE 58,132
;******************************************************************************
TITLE dmaeisa.asm -
;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp.  All Rights Reserved, 1989, 1990
;
;   Title:	dmaeisa.asm -
;
;   Version:	1.00
;
;   Date:	15-Dec-1989
;
;   Author:	RAP
;
;------------------------------------------------------------------------------
;
;   Change log:
;
;      DATE	REV		    DESCRIPTION
;   ----------- --- -----------------------------------------------------------
;   15-Dec-1989 RAP
;
;==============================================================================


	.386p

;******************************************************************************
;			      I N C L U D E S
;******************************************************************************

.XLIST
	include VMM.INC

	include VDMAD.INC
	include DMASYS.INC

	include Debug.INC
.LIST


VxD_IDATA_SEG

; IO ports specific to EISA's with extended DMA capabilities, these ports are
; only trapped, if EISA machine is detected
;
Begin_VxD_IO_Table VDMAD_EISA_IO_Table
	VxD_IO	DMA_E_IS,  VDMAD_EISA_ChainMode_Status1
	VxD_IO	DMA_E_CS,  VDMAD_EISA_ChainMode_Status2
	VxD_IO	DMA_E_EM1, VDMAD_EISA_Ext_Mode
	VxD_IO	DMA_E_EM2, VDMAD_EISA_Ext_Mode

	VxD_IO	DMA_E_P0,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P1,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P2,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P3,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P5,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P6,  VDMAD_EISA_High_Page
	VxD_IO	DMA_E_P7,  VDMAD_EISA_High_Page

	VxD_IO	DMA_E_C0,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C1,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C2,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C3,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C5,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C6,  VDMAD_EISA_High_Count
	VxD_IO	DMA_E_C7,  VDMAD_EISA_High_Count

	VxD_IO	DMA_E_S00, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S01, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S02, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S10, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S11, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S12, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S20, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S21, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S22, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S30, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S31, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S32, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S50, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S51, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S52, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S60, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S61, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S62, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S70, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S71, VDMAD_EISA_Stop
	VxD_IO	DMA_E_S72, VDMAD_EISA_Stop
End_VxD_IO_Table VDMAD_EISA_IO_Table

VxD_IDATA_ENDS


VxD_DATA_SEG

EXTRN VDMAD_Check_TC:DWORD
EXTRN VDMAD_DMA1_status:BYTE
EXTRN VDMAD_DMA2_status:BYTE
EXTRN DMA_Channels:BYTE
EXTRN DMA_Ctrl1:BYTE
EXTRN VDMAD_EISA_Size_Ini:BYTE

PUBLIC DMA_EISA_Ext_Modes

DMA_EISA_Ext_Modes	label byte
DMA_EISA_Ext_Mode_0	db  DMA_EM_8bit
DMA_EISA_Ext_Mode_1	db  DMA_EM_8bit
DMA_EISA_Ext_Mode_2	db  DMA_EM_8bit
DMA_EISA_Ext_Mode_3	db  DMA_EM_8bit
DMA_EISA_Ext_Mode_4	db  DMA_EM_16bit_wc
DMA_EISA_Ext_Mode_5	db  DMA_EM_16bit_wc
DMA_EISA_Ext_Mode_6	db  DMA_EM_16bit_wc
DMA_EISA_Ext_Mode_7	db  DMA_EM_16bit_wc

VxD_DATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   VDMAD_EISA_Init
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
BeginProc VDMAD_EISA_Init

; hook EISA specific DMA ports
;
	mov	edi, OFFSET32 VDMAD_EISA_IO_Table
	VMMCall Install_Mult_IO_Handlers    ; Install the EISA port traps

	xor	esi, esi
	mov	edi, OFFSET32 VDMAD_EISA_Size_Ini
	VMMCall Get_Profile_String
next_size_string:
	jc	DEBFAR no_more_size_strings

	push	edx
	VMMCall Convert_Decimal_String
	cmp	eax, 7			    ;Q: valid channel spec?
IFDEF DEBUG
	jbe	short chn_ok
	Debug_Out "VDMAD:  illegal channel specified in SYSTEM.INI (",/noeol
dump_ini_string:
	mov	esi, edi
	VMMCall Out_Debug_String
	Trace_Out '=', /noeol
	mov	esi, [esp]
	VMMCall Out_Debug_String
	Trace_Out ')'
	jmp	short skip_ini
chn_ok:
ELSE
	ja	short skip_ini
ENDIF
	mov	ecx, eax		    ; save channel #
	mov	esi, edx
	cld
skip_white_space:
	lodsb
	or	al, al			    ;Q: end of string?
	jz	short bad_size		    ;	Y: ignore it!
	cmp	al, ' '
	jbe	skip_white_space
	cmp	al, ',' 		    ;Q: valid separator?
	jne	short bad_size		    ;	N: ignore ini
	mov	edx, esi
	VMMCall Convert_Decimal_String
	test	al, 7			    ;Q: valid size?
	jnz	short bad_size		    ;	N: ignore ini
	shr	eax, 3			    ; 8->1, 16->2, 32->3
	dec	eax			    ; 8->0, 16->1, 32->2
	cmp	eax, 2
	ja	short bad_size		    ;	N: ignore ini
	cmp	al, 1			    ;Q: size = 16?
	jne	short got_size		    ;	N:
	mov	al, 3			    ;	Y: assume EISA 16-bit
	cmp	byte ptr [edx], 'w'	    ;Q: specifying ISA word?
	je	short isa_16		    ;	Y: set with EISA 16-bit mode
	cmp	byte ptr [edx], 'W'	    ;Q: specifying ISA word?
	jne	short got_size		    ;	N: set with EISA 16-bit mode
isa_16:
	mov	al, 1			    ;	Y: set with EISA 16-bit mode
got_size:
	xchg	eax, ecx
	VxDCall VDMAD_Set_EISA_Adr_Mode

IFNDEF DEBUG
bad_size:
ENDIF
skip_ini:
	pop	edx
	VMMCall Get_Next_Profile_String
	jmp	next_size_string

no_more_size_strings:
	clc
	ret

IFDEF DEBUG
bad_size:
	Debug_Out "VDMAD:  illegal channel size specified in SYSTEM.INI (",/noeol
	jmp	dump_ini_string
ENDIF

EndProc VDMAD_EISA_Init

VxD_ICODE_ENDS


VxD_CODE_SEG

EXTRN	update_adr_byte:NEAR
EXTRN	VDMAD_GetChannel_FromPage:NEAR
EXTRN	VDMAD_Get_DMA_Handle_For_Ctrl:NEAR
EXTRN	VDMAD_NotifyOwner:NEAR


;******************************************************************************
;
;   VDMAD_EISA_ChainMode_Status1
;
;   DESCRIPTION:    Virtualize EISA Extension - Chain Mode for 1st 4 channels
;		    and Interrupt Status Register
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
BeginProc VDMAD_EISA_ChainMode_Status1

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_SetChainMode>

VDMAD_GetIntStatus:
	Trace_Out "VDMAD Warning:  reading EISA interrupt status"
	xor	al, al
	ret

EndProc VDMAD_EISA_ChainMode_Status1


;******************************************************************************
;
;   VDMAD_EISA_ChainMode_Status2
;
;   DESCRIPTION:    Virtualize EISA Extension - Chain Mode for 2nd 4 channels
;		    and Chaining Mode Status Register
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
BeginProc VDMAD_EISA_ChainMode_Status2

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_SetChainMode>

VDMAD_GetChainStatus:
	Trace_Out "VDMAD Warning:  reading EISA chaining mode status"
	xor	al, al
ignore_chainmode:
	ret

VDMAD_SetChainMode:
	mov	cl, al
	and	cl, 1100b	    ; isolate mode control bits
	jz	short ignore_chainmode
	Debug_Out "VDMAD error:  attempt to set chaining mode with #dx, #al"
	VMMCall Crash_Cur_VM

EndProc VDMAD_EISA_ChainMode_Status2


;******************************************************************************
;
;   VDMAD_EISA_Ext_Mode
;
;   DESCRIPTION:    Virtualize EISA Extension - Extended Mode
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
BeginProc VDMAD_EISA_Ext_Mode

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT Set_EISA_ExtMode>

	in	al, dx			; this isn't defined
	ret

Set_EISA_ExtMode:
	Debug_Out "physically setting EISA extended mode:  out #dx, #al"
	out	dx, al			; physically set extended mode

	mov	esi, OFFSET32 DMA_Ctrl1 ; set esi to point to DMA data
	cmp	dx, DMA_E_EM1
	mov	edx, OFFSET32 DMA_EISA_Ext_Modes
	je	short emode_for_1st
	add	esi, SIZE DMA_Controller_State	; point to 2nd controller data'
	add	edx, 4
emode_for_1st:
	movzx	ecx, al
	and	cl, DMA_EM_Chan_Mask	; mask out non-chan # bits
	mov	[ecx][edx], al
	and	al, DMA_EM_Chan_Size
	btr	[esi.CTL_flags], ecx	; clear word cnt bit
	cmp	al, DMA_EM_16bit_wc
	jne	short not_word_cnt_chn
	bts	[esi.CTL_flags], ecx	; set word cnt bit
not_word_cnt_chn:
	ret

EndProc VDMAD_EISA_Ext_Mode


;******************************************************************************
;
;   VDMAD_EISA_High_Page
;
;   DESCRIPTION:    Virtualize EISA Extension - High Count Byte
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
BeginProc VDMAD_EISA_High_Page
	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_SetHiPage>

VDMAD_GetHiPage:
	sub	edx, DMA_E_Hoff
	call	VDMAD_GetChannel_FromPage
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	movzx	eax, word ptr [eax.cur_addr+3]	; al = A24-A31
	DMA_Q_OUT "Get High Page #edx=#al"
	ret

VDMAD_SetHiPage:
	DMA_Q_OUT "Set High Page #bx=#al",,edx

	call	VDMAD_GetChannel_FromPage
	mov	ah, 3			; 4th byte of address
	call	update_adr_byte
	xor	edx, edx		; check mask bit
	CallRet VDMAD_NotifyOwner

EndProc VDMAD_EISA_High_Page


;******************************************************************************
;
;   VDMAD_EISA_High_Count
;
;   DESCRIPTION:    Virtualize EISA Extension - High Count Byte
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
BeginProc VDMAD_EISA_High_Count

	call	[VDMAD_Check_TC]
	mov	esi, OFFSET32 DMA_Ctrl1
	Dispatch_Byte_IO <SHORT VDMAD_GetHiCount>, Fall_Through

VDMAD_SetHiCount:
	DMA_Q_OUT "Set High Count #bx=#al",,edx
	call	channel_from_hicount_port
	mov	edx, eax
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	mov	byte ptr [eax.cur_count+2], dl	; set high count byte
	mov	byte ptr [eax.pgm_count+2], dl	; set high count byte
	xor	edx, edx			; check mask bit
	CallRet VDMAD_NotifyOwner

VDMAD_GetHiCount:
	call	channel_from_hicount_port
	call	VDMAD_Get_DMA_Handle_For_Ctrl
	mov	al, byte ptr [eax.cur_count+2]	; get high count byte
	DMA_Q_OUT "Get High Count #edx=#al"
	ret

channel_from_hicount_port:
	movzx	ecx, dl
	and	cl, 7
	cmp	dx, DMA_E_C5
	jb	short hic_1st
	add	esi, SIZE DMA_Controller_State	; point to 2nd controller data
	shr	cl, 1
hic_1st:
	dec	cl
	shr	cl, 1
	ret

EndProc VDMAD_EISA_High_Count


;******************************************************************************
;
;   VDMAD_EISA_Stop
;
;   DESCRIPTION:    Virtualize EISA Extension - Stop Registers
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
BeginProc VDMAD_EISA_Stop

	call	[VDMAD_Check_TC]
	Dispatch_Byte_IO Fall_Through, <SHORT VDMAD_SetStop>

VDMAD_GetStop:
	Trace_Out "VDMAD Warning:  attempt to read EISA stop register"
	xor	al, al
ignore_stop:
	ret

VDMAD_SetStop:
	or	al, al
	jz	short ignore_stop
	Debug_Out "VDMAD Error:  writing EISA stop register #dx, #al"
	VMMCall Crash_Cur_VM

EndProc VDMAD_EISA_Stop


VxD_CODE_ENDS


END
