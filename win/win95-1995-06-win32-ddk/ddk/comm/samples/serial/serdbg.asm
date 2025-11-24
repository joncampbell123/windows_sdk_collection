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

	PAGE	58,132
;******************************************************************************
;
; Title:	SERDBG.ASM
;
;==============================================================================

.386p
	.xlist
	INCLUDE	VMM.INC
	INCLUDE	DEBUG.INC
	INCLUDE	VCOMM.INC
	INCLUDE	INS8250.INC
	INCLUDE	INTERNAL.INC
	INCLUDE	VPICD.INC
	.list

VxD_Locked_Data_Seg
		EXTRN	PortInfoHandle:DWORD
		EXTRN	SysVMHandle:DWORD
VxD_Locked_Data_Ends

VxD_Locked_Code_Seg

IFDEF	DEBUG
;******************************************************************************
;
; Serial_Debug_Query
;
; Description:
;		help in debugging/ performance analysis
;
; Entry:
;		None.
;
; Exit:
;		NC
;
; Uses:
;		ALL
;==============================================================================

BeginProc Serial_Debug_Query, PUBLIC

	Trace_Out "SERIAL information service: "

Serial_Debug_Continue:
	Trace_Out " "
	Trace_Out "[1] Dump the list of Port handles for open ports"
	Trace_Out "[2] Dump the list of Port base/IRQs"
	Trace_Out "[3] Dump Port stats (unfinished)"
	Trace_Out "[4] Fiddle with IRQ status"
	Trace_Out "[5] Reset error statistics"
	Trace_Out "Enter selection or press [ESC] to exit: ",NO_EOL
	VMMCall	In_Debug_Chr
	Trace_Out " "
	jz	Serial_Debug_Exit

	cmp	al,'1'
	je	Serial_Debug_ListHandles
	cmp	al,'2'
	je	Serial_Debug_DumpBaseIRQs
	cmp	al,'3'
	je	Serial_Debug_PortNum_Lp
	cmp	al,'4'
	je	Serial_Debug_Fiddle_IRQ
	cmp	al,'5'
	jne	Serial_Debug_Continue

Serial_Debug_Reset_Stats:
	call	Select_Port
	jz	Serial_Debug_Continue
	xor	eax,eax
	mov	[esi.FramingErrors],eax
	mov	[esi.OverrunErrors],eax
	mov	[esi.ParityErrors],eax
	mov	[esi.ArtificialErr],eax
	mov	[esi.NumDataInts],eax
	jmp	Serial_Debug_Continue

Serial_Debug_Fiddle_IRQ:

	mov	bl,[esi.IRQn]
	Trace_Out "Port #al, IRQ #bl Port handle #esi"
	mov	eax,[esi.IRQHandle]
	mov	ebx,[SysVMHandle]
	VxDCall	VPICD_Get_Complete_Status	; get the status
	mov	bl,[esi.IRQn]
	Trace_Out "Complete IRQ status: #ecx"
	Trace_Out "Vir Pir VirVM MaskVM PiS PMask ViS VirPend"
	Trace_Out " "
	Trace_Out "[1] Physically mask IRQ #bl"
	Trace_Out "[2] Physically unmask IRQ #bl"
	Trace_Out "Press selection or press [ESC] to go to main menu: ",NO_EOL

Serial_InChr_Loop:
	VMMCall	In_Debug_Chr
	Trace_Out " "
	jz	Serial_Debug_Continue
	sub	al,'1'
	jc	Serial_InChr_Loop
	cmp	al,1
	ja	Serial_InChr_Loop
	mov	eax,[esi.IRQHandle]
	jne	Serial_Dbg_Mask_IRQ
	VxDCall	VPICD_Physically_Unmask
	jmp	Serial_Debug_Continue

Serial_Dbg_Mask_IRQ:
	VxDCall	VPICD_Physically_Mask
	jmp	Serial_Debug_Continue

Serial_Debug_PortNum_Lp:

	call	Select_Port
	jz	Serial_Debug_Continue
	Trace_Out " Port #al, Port Handle: #esi"
	cmp	[esi.IRQHandle],0
	je	Serial_Debug_Closed_Port
	mov	ebx,[esi.pData.dwCommError]
	Trace_Out " Pending errors: #ebx"
	mov	ebx,[esi.NotifyHandle]
	mov	ecx,[esi.NotifyFlags]
	Trace_Out " Procedure to notify: #ebx Notify flags set: #ecx"
	mov	ebx,[esi.RecvTrigger]
	mov	ecx,[esi.SendTrigger]
	Trace_Out " Receive trigger: #ebx, Send trigger #ecx"
	mov	ebx,[esi.AddrEvtDWord]
	mov	ecx,[esi.AddrMSRShadow]
	Trace_Out " Address of EvtDword #ebx, MSRShadow #ecx"
	mov	ebx,[ebx]
	mov	ecx,[ecx]
	Trace_Out " EvtDWORD: #ebx, MSRShadow: #ecx"
	mov	ebx,[esi.ComDCB.BaudRate]
	mov	ecx,[esi.ComDCB.BitMask]
	Trace_Out " BaudRate: #ebx, BitMask #ecx"
	movzx	ebx,[esi.ComDCB.ByteSize]
	movzx	ecx,[esi.ComDCB.Parity]
	movzx	eax,[esi.ComDCB.StopBits]
	Trace_Out " Parity: #ecx, ByteSize: #ebx, StopBits: #eax"
	mov	eax,[esi.IRQHandle]
	mov	ebx,[SysVMHandle]
	VxDCall	VPICD_Get_Complete_Status	; get the status
	Trace_Out "Complete IRQ status: #ecx"
	Trace_Out "Vir Pir VirVM MaskVM PiS PMask ViS VirPend"
	mov	eax,[esi.OverrunErrors]
	mov	ebx,[esi.ArtificialErr]
	Trace_Out "Line Overrun: #eax, Buffer Overruns: #ebx"
	mov	eax,[esi.ParityErrors]
	mov	ebx,[esi.FramingErrors]
	Trace_Out "Parity Errors: #eax, Framing errors: #ebx"
	mov	eax,[esi.pData.QInCount]
	mov	ebx,[esi.pData.QOutCount]
	Trace_Out "#bytes in Receive Q: #eax, #bytes in xmit Q: #ebx"
	mov	eax,[esi.NumDataInts]
	Trace_Out "# data avail Ints #eax"
	movzx	eax,[esi.pData.LossByte]
	Trace_Out "Port ownership: #eax"
	Trace_Out " "
	jmp	Serial_Debug_Continue

Serial_Debug_Closed_Port:
	Trace_Out "Port #al is not open"
	jmp	Serial_Debug_Continue

Serial_Debug_ListHandles:

	Trace_Out " "
	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First

Serial_Debug_List_lp:
	jz	Serial_Debug_Continue
	mov	ebx,[eax.Port]
	Trace_Out "Port #ebx:	Handle: #eax"
	VMMCall	List_Get_Next
	jmp	Serial_Debug_List_lp

Serial_Debug_DumpBaseIRQs:
	Trace_Out " "
	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First

Serial_Debug_DBIRQ_lp:
	jz	Serial_Debug_Continue
	mov	ecx,[eax.Port]
	movzx	edx,[eax.IRQn]
	mov	ebx,[eax.MyName]
	mov	ebx,[ebx]
	Trace_Out "COMM Name #ebx:	Base #ecx	IRQ #edx"

DBIRQ_Next:
	VMMCall	List_Get_Next
	jmp	Serial_Debug_DBIRQ_lp

Serial_Debug_Exit:
	clc
	ret

EndProc Serial_Debug_Query

;
; Select_Port
;
; Exit: if NZ, ESI -> PortInformation
;	else no port was selected.
; Uses:
;	All
BeginProc Select_Port 

	xor	ebx,ebx			; count of list
	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First

SP_Selectlp:
	jz	SP_AskForInput
	Trace_Out " "
	mov	ecx,[eax.Port]
	inc	ebx
	Trace_Out "[#ebx]	Port Base #ecx"
	VMMCall	List_Get_Next
	jnz	SP_Selectlp

SP_AskForInput:
	Trace_Out "Select the port or press [ESC] to go to main menu: ",NO_EOL
	VMMCall	In_Debug_Chr
	Trace_Out " "
	jz	Select_Invalid
	sub	al,'0'
	or	al,al
	jz	Select_Invalid
	cmp	al,bl
	ja	Select_Invalid_1
	VMMCall	List_Get_First

SP_GetList_Lp:
	dec	ebx
	jz	Select_Done
	VMMCall	List_Get_Next
	jmp	SP_GetList_Lp

Select_Done:
	mov	esi,eax				; ESI -> PortInformation.
	or	esi,esi				; Set NZ flag

Select_Invalid:
	ret

Select_Invalid_1:
	xor	eax,eax
	ret

EndProc Select_Port

ENDIF

VxD_Locked_Code_Ends
	end
