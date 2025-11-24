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
; Title:	SERINIT.ASM
;
;******************************************************************************

	.386p

	.xlist
	include	VMM.INC
	include	OPTTEST.INC
	include	VCOMM.INC
	include	DEBUG.INC
	include	INTERNAL.INC
	include	INS8250.INC
	include	CONFIGMG.INC
	.list

VxD_Pageable_Code_Seg

	EXTRN	_PortOpen:NEAR

VxD_Pageable_Code_Ends

VxD_Pageable_Data_Seg

	EXTRN	PortInfoHandle:DWORD
	EXTRN	Serial_Functions:BYTE
	EXTRN	SysVMHandle:DWORD

IRQInfoHandle	dd	0

CommControl	db	'Settings',0
Settings	dd	?
prd_Temp	dd	?

;***
;
; SettingsInfo: struct to read from registry per COM port
;

SettingsInfo	STRUC

COMFifoOn	db	?		; Q: How to deal with FIFO ?
COMTxFifoSize	db	?		; Q: How to deal with Xmit FIFO ?
COMDSROn	db	?		; Q: Should we force DSR on ?
COMRxTrigger	db	?		; Q: two bit combo for the trigger

SettingsInfo	ENDS

.errnz SIZE SettingsInfo - 4

VxD_Pageable_Data_Ends

VxD_ICODE_Seg

;******************************************************************************
;
; Serial_Device_Init
;
; Description:
;		On snowball, this gets called at Device_Init time
;		On Chicago, it gets called at Sys_Dynamic_Device_Init
;		time. If registers itself as a port driver with VCOMM
;		and loads if it can find the port(s) being configured.
;		On Snowball, it looks at BDA and add all ports while
;		on Chicago, it configures only the given port.
;
; Entry:
;		None.
; Exit:	
;		if port(s) could be configured then NC
;		else CY
;
; Uses: All
;
;******************************************************************************
BeginProc Serial_Device_Init,PUBLIC

	cmp	[SysVMHandle],0
	jne	NDI_Load_Done

	VMMCall	Get_Sys_VM_Handle
	mov	[SysVMHandle],ebx		; save

	VxDCall	VCOMM_Get_Version		; Q: VCOMM present ?
	jc	NDI_NoLoad			;    N: fail to load.

	mov	eax,LF_Alloc_Error
	mov	ecx,SIZE PortInformation	; Create per port list.
	VMMCall	List_Create
	jc	NDI_NoLoad
	mov	[PortInfoHandle],esi

	xor	eax,eax
	mov	ecx,SIZE IRQStruc		; Create per irq list.
	VMMCall	List_Create
	jc	NDI_NoLoad
	mov	[IRQInfoHandle],esi

	VxDCall	_VCOMM_Register_Port_Driver,<OFFSET32 _S_DriverControl>

NDI_Load_Done:
	clc

NDI_NoLoad:
	ret

EndProc Serial_Device_Init

VxD_ICODE_Ends

VxD_Pageable_Code_Seg

;******************************************************************************
;
; void S_DriverControl(DWORD fCode, DWORD DevNode, DWORD DCRefData,...)
;
; Description:
;
;	Called by VCOMM to perform various actions.
;
; Entry:
;	fCode	= code of function to perform
;	DevNode -> devnode in plug and play scheme
;	DCRefData = reference data to refer to while calling VCOMM back.
;
; Exit:
;	None
; Uses:
;	C style
;==============================================================================

BeginProc S_DriverControl, CCALL, esp, PUBLIC

ArgVar	fCode,DWORD
ArgVar	DevNode,DWORD
ArgVar	DCRefData,DWORD

	EnterProc

	mov	eax,fCode
	or	eax,eax
	.errnz	DC_Initialize
	jz	_InitFunction

	TRAP
	LeaveProc
	return

EndProc S_DriverControl

;******************************************************************************
;
; void InitFunction(DWORD fCode, DWORD DevNode, DWORD DCRefData, 
;		    DWORD AllocBase,DWORD AllocIrq, char *oldname);
;
; Description:
;
;	Called by VCOMM to initialize a port.
;
; Entry:
;	fCode	= code of function to perform
;	DevNode -> devnode in plug and play scheme
;	DCRefData = reference data to refer to while calling VCOMM back.
;	AllocBase = allocated base
;	AllocIrq  = allocated irq
;	oldname -> name of port.
;
; Exit:
;	None
; Uses:
;	C style
;==============================================================================

BeginProc InitFunction,CCALL,esp,PUBLIC

ArgVar	fCode,DWORD
ArgVar	DevNode,DWORD
ArgVar	DCRefData,DWORD
ArgVar	AllocBase,DWORD
ArgVar	AllocIrq,DWORD
ArgVar	oldname,DWORD

	EnterProc

	SaveReg	<esi,edi,ebx>

	cmp	AllocIrq, 0
	je	IF_Done
;
; We use the following algorithm:
;	1) Look in the list of ports known to us so far for the base
;	   address.
;	2) If we know the base address, return without adding the
;	   port.
;	3) If we don't know the base address, allocate a list, attach it,
;	   fill it with values and return.
;
	mov	esi,[PortInfoHandle]
	VMMCall	List_Get_First
	jz	IF_NewPort
	mov	ecx,AllocBase

IF_CheckIfknownBase:
	cmp	[eax.Port],ecx		; Q: Do we know about the port?
	je	IF_Done			;    Y:
	VMMCall	List_Get_Next
	jnz	IF_CheckIfKnownBase

IF_NewPort:
	VMMCall	List_Allocate		; allocate space
	jc	IF_Done
	VMMCall	List_Attach		; attach it
	mov	edi,eax			; EDI -> PortInfo
	mov	ebx,eax			; save ptr
	xor	eax,eax
	mov	ecx,(SIZE PortInformation)/4
	.errnz	(SIZE PortInformation and 3)
	rep	stosd			; zero it out

	mov	DWORD PTR [ebx._PortData.PDLength],(10Ah SHL 16) OR SIZE _PortData
	mov	[ebx._PortData.PDFunctions], OFFSET32 Serial_Functions
	mov	[ebx._PortData.PDNumFunctions],(SIZE _PortFunctions)/4
	mov	WORD PTR [ebx.ComDCB.XonChar],01311h ; CTRL+S CTRL+Q
	dec	[ebx.RecvTrigger]	; will become -1.
	mov	[ebx.RxFifoTrigger],ACE_TRIG08
	mov	[ebx.TxFifoTrigger],16	; blast 16 bytes.

	lea	esi,Settings		; comm settings
	mov	DWORD PTR [esi], 2 OR (16 SHL 8) OR (ACE_TRIG08 SHL 24)
					; (Fifo, TxFifo, no DSR, 8byte Rxfifo)
	mov	prd_Temp,SIZE SettingsInfo	; size of entry

	VxDCall	_CONFIGMG_Read_Registry_Value,< \
		DevNode, 0, OFFSET32 CommControl, REG_BINARY, \
		esi, OFFSET32 prd_Temp, CM_REGISTRY_HARDWARE>
	test	eax,eax			; Q: success ?
	jnz	IF_Defaults_OK
	mov	cl,[esi.COMRxTrigger]	; save RXFIFO trigger bits.
	mov	[ebx.RxFifoTrigger],cl
	cmp	[esi.COMDSROn],0	; Q: Force DSR ?
	jz	IF_NoForceDsr		;    N:
	SetFlag	[ebx.EFlags],fUseDSR

IF_NoForceDsr:
	cmp	[esi.COMFifoOn],1
	ja	IF_FifoDefault_OK
	jb	IF_NoFifo		; force it OFF if = 0
	SetFlag	[ebx.EFlags],fFIFOchkd	; force it ON if = 1
	jmp	IF_FifoDefault_OK

IF_NoFifo:
	SetFlag	[ebx.EFlags], fNoFIFO OR fFIFOchkd	; force OFF

IF_FifoDefault_OK:
	cmp	[esi.COMTxFifoSize],1 ; Q: Turn off TxFifo ?
	jbe	IF_TurnFifoOff		;    Y:
	mov	cl,[esi.COMTxFifoSize] ; get fifo trigger size
	mov	[ebx.TxFifoTrigger],cl	  ; save it.
	jmp	IF_Defaults_OK

IF_TurnFifoOff:
	SetFlag	[ebx.EFlags], fNoTxFifo	;    Y:

IF_Defaults_OK:
	mov	ecx,AllocBase
	mov	[ebx.Port],ecx		; save
	mov	ecx,AllocIrq
	mov	[ebx.IRQn],cl		; save

	mov	esi,[IRQInfoHandle]
	VMMCall	List_Get_First
	jz	IF_NewIRQStruc

IF_CheckForKnownIrq:
	cmp	[eax.IRQNumber],cl		; Q: Found irq ?
	je	IF_IrqStrucFound		;    Y:
	VMMCall	List_Get_Next
	jnz	IF_CheckForKnownIrq

IF_NewIRQStruc:
	VMMCall	List_Allocate
	VMMCall	List_Attach

	mov	DWORD PTR [eax.IRQNumber],ecx
	.errnz	(MyIRQHandle - IRQNumber - 4)
	xor	ecx, ecx
	mov	[eax.PIStruc], ecx
	mov	[eax.MyIRQHandle], ecx

IF_IrqStrucFound:
	mov	[ebx.MyIRQStruc],eax

	mov	edi,oldname			; EDI -> name of port
	mov	esi,edi				; ESI -> name of port
	xor	eax,eax
	xor	ecx,ecx
	dec	ecx				; ECX = -1.
	repne	scasb				; search for 0
	neg	ecx
	dec	ecx				; ECX = strlen(oldname)

	SaveReg	<ecx>
	VMMCall	_HeapAllocate,<ecx,0>
	mov	edi,eax				; EDI -> buffer to copy name to
	RestoreReg <ecx>
	or	eax,eax
	jz	IF_DeallocAndQuit
	mov	[ebx.MyName],eax		; save my name
	rep	movsb				; copy the name

	VxDCall	_VCOMM_Add_Port,<DCRefData, OFFSET32 _PortOpen,eax>
	VxDCall	_VCOMM_Get_Contention_Handler,<oldname>
	mov	[ebx.ContentionHnd],eax
	test	eax,eax				; Q:Contention handler present?
	jz	IF_Done				;   N: get out
	VxDCall	_VCOMM_Map_Name_To_Resource,<oldname>
	mov	[ebx.ContentionRes],eax
	jmp	IF_Done

IF_DeallocAndQuit:
	call	DeAllocData			; EXPECTS EBX->PortInformation

IF_Done:
	RestoreReg <ebx,edi,esi>

	LeaveProc
	return

EndProc InitFunction

;******
;
; DeAllocData
;
; Description:
;	Deallocs the IRQStruc associated with the PortInformation
;	if VirtCnt is 0, also deallocs the PortInformation struct
; Entry:
;	EBX = PortInformation
; Exit:
;	None
; Uses:
;	C style
;
BeginProc DeAllocData,PUBLIC

	SaveReg	<esi>

	mov	esi,[IRQInfoHandle]
	mov	eax,[ebx.MyIRQStruc]
	or	eax,eax				; Q: Is MyIrqStruc alloc'ed ?
	jz	DAD_NoIrqStrucToDealloc		;    N:
	cmp	[eax.VirtCnt],0			; Q: Is virtualize count == 0?
	jnz	DAD_NoIrqStrucToDealloc		;    N:
	VMMCall	List_Remove
	VMMCall	List_Deallocate

DAD_NoIrqStrucToDealloc:
	cmp	[ebx.MyName],0
	je	DAD_NoNameToDealloc
	VMMCall	_HeapFree,<[ebx.MyName],0>

DAD_NoNameToDealloc:
	mov	esi,[PortInfoHandle]
	mov	eax,ebx
	VMMCall	List_Remove
	VMMCall	List_Deallocate

	RestoreReg <esi>
	ret

EndProc DeAllocData

VxD_Pageable_Code_Ends

	end
