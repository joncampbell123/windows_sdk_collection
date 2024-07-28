       title   CGA Trap I/O -- Emulate CGA regs  Version 3.00
;******************************************************************************
;
; VDDTIO.ASM - Virtual CGA I/O Register emulation
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;   January, 1987
;
;DESCRIPTION:
;	Emulate CGA I/O registers.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE CGA.INC
	INCLUDE VDD.INC
	INCLUDE DEBUG.INC

;******************************************************************************
VxD_IDATA_SEG

; Note that these I/O ports are always trapped so they can be in IDATA seg
;	since they are only referenced at initialization.
Begin_VxD_IO_Table VDD_IDC_IO_Table
	VxD_IO	13C6h, VDD_Tr_IO_EMode		; Master Mode
	VxD_IO	23C6h, VDD_Tr_IO_EMode		; Extended Mode Select
	VxD_IO	27C6h, VDD_Tr_IO_EMode		; Screen Blank (Timeout)
	VxD_IO	2BC6h, VDD_Tr_IO_EMode		; Underline Control
End_VxD_IO_Table VDD_IDC_IO_Table

Begin_VxD_IO_Table VDD_ATT_IO_Table
	VxD_IO	pMode2, VDD_Tr_IO_Mode2 		; ATT/Oli MODE 2
	VxD_IO	pMode3, VDD_Tr_IO_Mode3 		; ATT/Oli MODE 3/stat 2
End_VxD_IO_Table VDD_ATT_IO_Table

VxD_IDATA_ENDS


;******************************************************************************
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD

PUBLIC	CGANoSnow
CGANoSnow   dd	0

Begin_VxD_IO_Table VDD_IO_Table
	VxD_IO	pIndx6845Colr, VDD_Tr_IO_Indx		; 6845 index register
	VxD_IO	pData6845Colr, VDD_Tr_IO_Data		; 6845 data register
	VxD_IO	pModeColr, VDD_Tr_IO_Mode		; mode select register
	VxD_IO	pColrCGA, VDD_Tr_IO_Colr		; color select register
	VxD_IO	pStatColr, VDD_Tr_IO_Stat		; status register
	VxD_IO	pLPen1Colr, VDD_Tr_IO_Lpen		; light pen latch clear
	VxD_IO	pLPen2Colr, VDD_Tr_IO_Lpen		; light pen latch set
End_VxD_IO_Table VDD_IO_Table

;**********
; Ports for Extended Mode Registers for IDC 640x400 Plasma Display
;
;	The position offset in the table of the register code
;	corresponds to the offset in the CGA state structure where
;	the register is kept
VDD_EMode_Count EQU	4
PUBLIC VDD_EMode_Table
VDD_EMode_Table LABEL WORD
	DW	VDD_EMode_Count ; Size of this table (number of entries)
	DW	13C6h		; Master Mode
	DW	23C6h		; Extended Mode Select
	DW	27C6h		; Screen Blank (Timeout)
	DW	2BC6h		; Underline Control
.ERRNZ	(VDD_EMode_Count+1)-($-VDD_EMode_Table)/2

VxD_DATA_ENDS

;******************************************************************************
VxD_ICODE_SEG

;******************************************************************************
; VDD_IO_Init
;
;DESCRIPTION:
;	Sets up trapping for VDD I/O ports
;
;ENTRY: EBX = VM1 handle
;	EDI = VDD CB ptr for VM1
;
;EXIT:	none
;
;USES:	Flags
;
;==============================================================================
BeginProc VDD_IO_Init,PUBLIC
	push	edi
	mov	edi, OFFSET32 VDD_IO_Table	; Table of ports
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
	cmp	[vgVDD.Vid_Type],Vid_Type_IDC	; Y: It is an IDC
	jnz	SHORT VIOI_NotIDC
	mov	edi, OFFSET32 VDD_IDC_IO_Table	; Table of ports for IDC
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
VIOI_NotIDC:
	cmp	[vgVDD.Vid_Type],Vid_Type_ATT	; Y: It is an ATT?
	jnz	SHORT VIOI_Exit
	mov	edi, OFFSET32 VDD_ATT_IO_Table	; Table of ports for ATT
	VMMCall Install_Mult_IO_Handlers	; Install the port traps
VIOI_Exit:
	pop	edi
	ret
EndProc VDD_IO_Init


VxD_ICODE_ENDS


;******************************************************************************
VxD_CODE_SEG

	EXTRN	VDD_Font_Unaccess:NEAR
	EXTRN	VDD_Font_Access:NEAR
        EXTRN   VDD_Get_Mode:NEAR
;******************************************************************************
; VDD_IO_SetTrap
;
;DESCRIPTION:
;   Sets up trapping for VDD I/O ports when VM type or focus changes
;
; Pseudo code:
;
;   if (Msg mode initialisation)
;        Disable trapping;
;   else if (Not Attached VM || Doing a mode set
;               || Trapping enabled in all modes(in PIF))
;        Enable trapping;
;   else {
;        if (trapping enabled in current mode(in PIF))
;           Enable trapping;
;        else
;           Disable trapping;
;   }
;       
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD CB ptr
;
;EXIT:	none
;
;USES:	Flags, EAX, ECX, EDX
;
;==============================================================================
BeginProc VDD_IO_SetTrap,PUBLIC

Assert_VDD_ptrs ebx,edi
	push	esi
	mov	esi, OFFSET32 VDD_IO_Table	; Table of ports
	movzx	ecx, WORD PTR [esi]		; Get count of ports
	add	esi,SIZE VxD_IOT_Hdr
	test	[vgVDD.Vid_Flags], fVid_MsgI	; Q: Message mode Init?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	cmp	ebx,[vgVDD.Vid_VM_Handle]	; Q: Attached VM?
	jnz	SHORT VIOST_Enable_Local	;   N: Do trapping
	test	[edi.VDD_Flags],fVDD_ModeSet	; Q: Doing mode set?
	jnz	SHORT VIOST_Enable_Local	;   Y: trap all ports

mVidNoTrapPIF EQU fVidNoTrpTxt+fVidNoTrpLRGrfx+fVidNoTrpHRGrfx
	test	[edi.VDD_PIF],mVidNoTrapPIF	;   Y: Q: No trapping enabled?
	jz	SHORT VIOST_Enable_Local	;	N: trap all ports
        call    VDD_Get_Mode                    ; EAX = Mode
	cmp	[edi.VDD_Mode],3		; Q: Text mode?
	ja	SHORT VIOST_00			;   N: continue
	test	[edi.VDD_PIF],fVidNoTrpTxt	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping

VIOST_00:					; Graphics mode
mVidNoTrapGrp EQU fVidNoTrpLRGrfx+fVidNoTrpHRGrfx
	test	[edi.VDD_PIF],mVidNoTrapGrp	; Q: No trapping enabled?
	jnz	SHORT VIOST_Disable_Local	;   Y: No trapping
	jmp	SHORT VIOST_Enable_Local	;   N: Do trapping

VIOST_Enable_Local:
	movzx	edx,WORD PTR [esi]
	add	esi,SIZE VxD_IO_Struc
	VMMCall Enable_Local_Trapping
	loopd	VIOST_Enable_Local
	pop	esi
	ret
VIOST_Disable_Local:
	movzx	edx,WORD PTR [esi]
	add	esi,SIZE VxD_IO_Struc
	VMMCall Disable_Local_Trapping
	loopd	VIOST_Disable_Local
	pop	esi
	ret

EndProc VDD_IO_SetTrap       

;******************************************************************************
; VDD_Tr_IO_xxxx      I/O trap on video display ports
;
;DESCRIPTION:
;	These routines are invoked when a physical VM or a normal VM other than
;	VM1 attempts to access the CGA I/O ports.  For a physical VM, output
;	values are saved and return is made to perform the I/O on behalf of the
;	VM.  For a normal VM, output values are saved and input values are
;	emulated.  For these VM's no actual I/O is performed.
;

;******************************************************************************
; VDD_TIO_Setup
;
;DESCRIPTION:
;	Trap set up and common processing. After setting up EDI and evoking
;	the Emulate_Non_Byte_IO routine, this routine is CALLed.
;	If I/O for currently attached VM:
;		if input, do input and return to caller's caller
;		else do output and return ZF=0(output) to caller
;	Else return to caller with CF=0(not owner),
;		ZF=1 for input and ZF=0 for output
;
;ENTRY: EBX = VM Handle
;	EDI = VDD CB ptr
;	AL = byte to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O (BYTE ONLY!)
;	[ESP+4] = return from routine that called this routine
;
;EXIT:	AL = emulated input value from port (if input)
;	CLC (CF=0) => I/O completed or emulated here (I did it)
;	STC (CF=1) => Do the I/O operation upon return from here (You do it)
;	NOTE: Returns one level up for physical input
;
;USES:	AL (if input)
;
BeginProc VDD_TIO_Setup
	cmp	[vgVDD.Vid_VM_Handle],ebx	; Q: Is this attached VM?
	je	SHORT VTIOS_Phys		;   Y: this is physical I/O
	or	cl,cl
	ret

VTIOS_Phys:
	or	cl,cl				; Q: Port output?
	jz	SHORT VTIOS_PInput		;   N: go do input

	out	dx,al
	stc					; CF = 1(display owner)
	ret

; Physical input: do input and return
VTIOS_PInput:
	add	esp,4				; Do not return to trap routine
	in	al,dx				; Do input
	ret
EndProc VDD_TIO_Setup

;******************************************************************************
; VDD_Tr_IO_Indx
;
;DESCRIPTION:
;	6845 index register
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), EDI
;
BeginProc VDD_Tr_IO_Indx
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup			; Setup and do I/O if attached
	jz	SHORT VDD_TII_Ex		; Ignore input (write only reg)
VDD_TII_Save:
	mov	[edi.VDD_Stt.V_Indx],al 	; Save index for  VM
VDD_TII_Ex:
	ret
EndProc VDD_Tr_IO_Indx

;******************************************************************************
; VDD_Tr_IO_Data
;
;DESCRIPTION:
;	6845 data register
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Data
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	movzx	ecx,[edi.VDD_Stt.V_Indx]	; ECX = index
	jz	short VDD_TI_Data		; Go do data register input
	mov	[edi.VDD_Stt.V_HTotal][ecx],al	; Save value of reg
	ret

; 6845 data input - this returns values for write only registers also!
VDD_TI_Data:
	mov	al,[edi.VDD_Stt.V_HTotal][ecx]	; Get value of selected reg
	ret
EndProc VDD_Tr_IO_Data

;******************************************************************************
; VDD_Tr_IO_Mode
;
;DESCRIPTION:
;	mode select register
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Mode
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	jz	SHORT VDD_TM_Ex 		; Ignore input (write only reg)
	mov	[edi.VDD_Stt.V_Mode],al 	; Save mode register value
VDD_TM_Ex:
	ret
EndProc VDD_Tr_IO_Mode

;******************************************************************************
; VDD_Tr_IO_Colr
;
;DESCRIPTION:
;	color select register
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Colr
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	jz	SHORT VDD_TIC_Ex		; Ignore input
	mov	[edi.VDD_Stt.V_Colr],al 	; Save color select value
VDD_TIC_Ex:
	ret
EndProc VDD_Tr_IO_Colr

;******************************************************************************
; VDD_Tr_IO_Stat
;
;DESCRIPTION:
;	status register
;	Performance Enhancement for MS Word, Wordstar 2000 and others
;	Always Simulate Status Register Reads
;	This may permit "snow" on IBM CGA, unless CGANoSnow is specified
;	in .INI file, in which case physical value is returned.  Note that
;	this does not guarantee no snow, just minimizes it.
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Stat

	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
        or      cl,cl
	jnz	SHORT VDD_TIS_Ex		; Ignore output (read only reg)
	call	VDD_Next_Stat
VDD_TIS_Ex:
	ret

VDD_Next_Stat:
; N Times On, M Times Toggle Algorithm:
VDD_VERT_PERIOD equ	12		; Must be at lease 11 for Wordstar 2000
VDD_HORIZ_PERIOD equ	255		; Trial Value must not be too big
	test	[CGANoSnow],-1
	jnz	SHORT VIVS_PhysInput
	mov	al,[edi.VDD_Stt.V_Stat] 	; Get old value of pseudo status
	cmp	[edi.VDD_Stt.V_VertCnt],0	; Q: In Simulated VRTC?
	je	SHORT VIVS_NotVert		;   N: continue
	dec	[edi.VDD_Stt.V_VertCnt] 	;   Y: count down vert retrace
	jmp	SHORT VIVS_Merge
VIVS_NotVert:
	dec	[edi.VDD_Stt.V_HorizCnt]	; count down horiz
	jz	SHORT VIVS_EndHoriz		; End of period
; Note Most General Algorithm would have duration counter for horizontal here
	xor	al,fstatDots+fstatEna		; Toggle dot stream bit
	jmp	short VIVS_SaveMerge
VIVS_EndHoriz:
	mov	[edi.VDD_Stt.V_VertCnt],VDD_VERT_PERIOD
	mov	[edi.VDD_Stt.V_HorizCnt],VDD_HORIZ_PERIOD
	or	al,fstatDots+fstatEna		; Turn Retrace Active Bits ON
VIVS_SaveMerge:
	mov	[edi.VDD_Stt.V_Stat],al 	; Save value of pseudo status
VIVS_Merge:
	mov	bl,al
	in	al,dx
	and	al,NOT fstatDots+fstatEna	; Use rest of physical bits
	or	al,bl
	ret
VIVS_PhysInput:
	in	al,dx
	ret
EndProc VDD_Tr_IO_Stat

;******************************************************************************
; VDD_Tr_IO_LPen
;
;DESCRIPTION:
;	Light pen not supported by virtual CGA, do I/O if physical
;
;	light pen latch clear - not supported for virtual machine
;	light pen latch set - not supported for virtual machine
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Lpen
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup			; Ignore except physical out
	ret
EndProc VDD_Tr_IO_Lpen

;******************************************************************************
; VDD_Tr_IO_Mode2
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Mode2
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	jz	SHORT VDD_TO2_Ex		; Ignore input
;	mov	ah,[edi.VDD_Stt.V_Mode2]	; Get old Mode 2
	mov	[edi.VDD_Stt.V_Mode2],al	; Save Mode 2
;	and	eax,101h
;	cmp	al,ah
;	jz	SHORT VDD_TO2_Ex
;	cmp	[vgVDD.Vid_VM_Handle],ebx	; Is this attached VM?
;	jz	SHORT VDD_TO2_Ex
;	call	VDD_Mem_Virtual 		;   N: remap virtual memory
VDD_TO2_Ex:
	ret
EndProc VDD_Tr_IO_Mode2

;******************************************************************************
; VDD_Tr_IO_Mode3
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_Mode3
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	jz	SHORT VDD_TO3_Ex		; Ignore input
	test	[edi.VDD_Stt.V_Mode2],80h	; Q: This port enabled?
	jz	SHORT VDD_TO3_Ex		;   N: ignore
	or	[edi.VDD_Flags],fVDD_Md3
	mov	[edi.VDD_Stt.V_Mode3],al	; Save Mode 3
VDD_TO3_Ex:
	ret
EndProc VDD_Tr_IO_Mode3

;******************************************************************************
; VDD_Tr_IO_EMode
;
;DESCRIPTION:
;
;ENTRY: EBX = VM Handle
;	AL/AX/EAX = data to output to port (if output)
;	DX = port address for I/O.
;	ECX = type of I/O
;
;EXIT:	AL = emulated input value from port (if input)
;
;USES:	Flags, AL (if input), ECX, EDI
;
BeginProc VDD_Tr_IO_EMode
	mov	edi,ebx
	add	edi,[VDD_CB_Off]		; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup
	push	eax				; save output value
        push    edi
 	pushfd					; save flags
	mov	eax,edx 			; port number into ax
	movzx	ecx,[VDD_EMode_Table]		; number of ports in table
	mov	edi,OFFSET32 VDD_EMode_Table+2	; first entry
	cld					; search forward
	repne scasw				; find port in table
	jne	SHORT VDD_TIE_NotValid
	sub	edi,OFFSET32 VDD_EMode_Table+2+2; Distance
	mov	ecx,edi 			; transfer to ecx
	shr	ecx,1				; convert to 0 base index
	popfd					; restore flags (ZF from Setup)
        pop     edi
	pop	eax				; restore out value
	jz	SHORT VDD_TIE_In		; ZF means input

; Output requested, save (track) values
	pushfd
	cmp	cl,I_EMSel-I_MMode		; Q: Extended Mode Select?
	jne	SHORT VDD_TIE_Save		;   N: Nothing Special to do
	popfd
	pushfd
	push	eax
	jc	SHORT VDD_TIE_FontOK		; if CF=1, Display owner

; emulate font feature as required for background task
	mov	ah,[edi.VDD_Stt.I_EMsel]	; prior value
	and	ax,fEMSelFont+(fEMSelFont shl 8); Mask al and ah
	cmp	al,ah				; Q: Font/Video mode change?
	je	SHORT VDD_TIE_FontOK		; N: continue
	test	al,al				; Q: Font mode bit now on?
	jnz	SHORT VDD_TIE_Font		; Y: Do Font
	call	VDD_Font_Unaccess		; N: Undo Font
	jmp	SHORT VDD_TIE_FontOK		; continue
VDD_TIE_Font:
	call	VDD_Font_Access
VDD_TIE_FontOK:
	pop	eax
VDD_TIE_Save:
	mov	[edi.VDD_Stt.I_MMode][ecx],al	; save new value
	popfd
	ret

; Input requested by non-owner of display
VDD_TIE_In:
	mov	al,[edi.VDD_Stt.I_MMode][ecx]	; use saved value
	ret
; Unknown port, ignore output, do physical input if not already done
VDD_TIE_NotValid:
	popfd
        pop     edi
	pop	eax
	jz	SHORT VDD_TIE_NVIn
	ret
; Unknown port input, do physical input
VDD_TIE_NVIn:
	in	al,dx
	ret
EndProc VDD_Tr_IO_EMode

VxD_CODE_ENDS

	END

