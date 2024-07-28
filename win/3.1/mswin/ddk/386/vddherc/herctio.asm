       title   HERC Trap I/O -- Emulate HERC regs  Version 3.00
;******************************************************************************
;
; VDDTIO.ASM - Virtual HERC I/O Register emulation
;
;   Author: MDW PAB
;
;   (C) Copyright MICROSOFT Corp. 1986 - 1989
;
;   January, 1987
;
;DESCRIPTION:
;	Emulate HERC I/O registers.
;
;******************************************************************************

	.386p

	INCLUDE VMM.INC
	INCLUDE HERC.INC
	INCLUDE VDD.INC
	INCLUDE DEBUG.INC


;******************************************************************************
VxD_DATA_SEG
	EXTRN	vgVDD:BYTE
	EXTRN	VDD_CB_Off:DWORD

Begin_VxD_IO_Table VDD_IO_Table
	VxD_IO	pIndx6845Mono, VDD_Tr_IO_Indx  ; 6845 index register
	VxD_IO	pData6845Mono, VDD_Tr_IO_Data  ; 6845 data register
	VxD_IO	pSetLtPen, VDD_Tr_IO_Lpen      ; light pen latch set
	VxD_IO	pReSetLtPen, VDD_Tr_IO_Lpen    ; light pen latch clear
	VxD_IO	pStatMono, VDD_Tr_IO_Stat      ; status register
	VxD_IO	pModeMono, VDD_Tr_IO_Mode      ; mode select register
        VxD_IO  pGrphCntrl, VDD_Tr_IO_Cntrl    ; Graphics Control reg.
End_VxD_IO_Table VDD_IO_Table

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
	pop	edi
	ret
EndProc VDD_IO_Init


VxD_ICODE_ENDS


;******************************************************************************
VxD_CODE_SEG


;******************************************************************************
; VDD_IO_SetTrap
;
;DESCRIPTION:
;	Sets up trapping for VDD I/O ports when VM type or focus changes
;
;ENTRY: EBX = VM handle
;	EDI = VM's VDD CB ptr
;	AL = 0 - disable trapping
;	AL != 0 - enable trapping
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

	or	al, al
	jz	short VIOST_Disable_Local

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
;	VM1 attempts to access the HERC I/O ports.  For a physical VM, output
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
; VDD_Tr_IO_Stat
;
;DESCRIPTION:
;	status register
;	Performance Enhancement for MS Word, Wordstar 2000 and others
;	Always Simulate Status Register Reads
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

HORIZ_PERIOD	equ	12		; Must be at lease 11 for Wordstar 2000
VERT_PERIOD	equ	128		; Trial Value must not be too big

	mov	al,[edi.VDD_Stt.V_Stat]	;Get old value of pseudo status
	xor	al,fstatdots			;Toggle Dots bit.

	cmp	[edi.VDD_Stt.V_HorizCnt],0	;End of horizontal retrace?
	je	SHORT toggle_H
	or	al,fstatHSync			;HSync default high.
	dec	[edi.VDD_Stt.V_HorizCnt]

check_vsync:
	cmp	[edi.VDD_Stt.V_VertCnt],0	;End of vertical retrace?
	je	SHORT toggle_V
	or	al,fstatVSync			;VSync default high.
	dec	[edi.VDD_Stt.V_VertCnt]

stat_end:
	mov	[edi.VDD_Stt.V_Stat],al 	;Store new value.
	ret

toggle_H:
	and	al,NOT fstatHSync		;Clear HSync bit.
	mov	[edi.VDD_Stt.V_HorizCnt],HORIZ_PERIOD
	jmp	SHORT check_vsync

toggle_V:
	and	al,NOT fstatVSync		;Clear VSync bit.
	mov	[edi.VDD_Stt.V_VertCnt],VERT_PERIOD
	jmp	stat_end	 	

IF 0
VDD_Next_Stat:
; N Times On, M Times Toggle Algorithm:
VDD_VERT_PERIOD equ	12		; Must be at lease 11 for Wordstar 2000
VDD_HORIZ_PERIOD equ	255		; Trial Value must not be too big
	mov	al,[edi.VDD_Stt.V_Stat] 	; Get old value of pseudo status
	xor	al,fstatdots			;Toggle Dots bit.
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
ENDIF

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
; VDD_Tr_IO_Cntrl
;
;DESCRIPTION:
;        Graphics switch control register
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
;
BeginProc VDD_Tr_IO_Cntrl

; ignore all changing of the configuration port
;
	or	al, 1			    ;force graphics to be enabled
	mov	edi,ebx
	add	edi,[VDD_CB_Off]	    ; EDI = VDD control blk ptr
	Emulate_Non_Byte_IO
	call	VDD_TIO_Setup		    ; Ignore except physical out
	jz	short VDD_TC_Ex 	    ; Ignore input (write only reg)
; Output requested, save (track) the new value
;;	  mov	  [edi.VDD_Stt.V_Cntrl],al    ; Save Control register value
VDD_TC_Ex:
        ret
EndProc VDD_Tr_IO_Cntrl

VxD_CODE_ENDS

	END

