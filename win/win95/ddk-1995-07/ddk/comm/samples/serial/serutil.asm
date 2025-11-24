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
;
; Title:	SERUTIL.ASM
;
;******************************************************************************

	.386p

	.xlist
	include	VMM.INC
	include	VPICD.INC
	include	VCOMM.INC
	include	DEBUG.INC
	include	OPTTEST.INC
	include	INTERNAL.INC
	include	INS8250.INC
	.list

VxD_My_Pageable_Data_Seg

	EXTRN	Serial_Name:BYTE
	EXTRN	PortInfoHandle:DWORD

VxD_My_Pageable_Data_Ends

VxD_Locked_Data_Seg

	EXTRN	SysVMHandle:DWORD
	EXTRN	TimeOutHandle:DWORD

VxD_Locked_Data_Ends

VxD_My_Pageable_Code_Seg

;******************************************************************************
;
; GetCOMPort
;
; Description:
;		Calls VCOMM to acquire the port
; Entry:
;	ESI -> PortInformation struct
; Exit:
;	NC iff successful
; Uses:
;	Flags,EAX,ECX
;
;******************************************************************************
BeginProc GetCOMPort, PUBLIC

	push	edx

	mov	ecx,[esi.ContentionHnd]	; get the contention handler
	test	ecx,ecx
	jz	GCP_Done
	mov	eax,[esi.ContentionRes]	; get the resource
	test	eax,eax
	jz	GCP_Failed
	cCall	ecx,<ACQUIRE_RESOURCE,eax,OFFSET32 _Notify_Loss,esi,1>
	mov	[esi.VCD_Data],eax	; save handle

GCP_Failed:
	cmp	eax,1			; CY if 0, else NC.
GCP_Done:
	pop	edx
	ret
EndProc GetCOMPort

;******************************************************************************
;
; ReleaseCOMPort
;
; Description:
;		Calls VCOMM to release a com port
;
; Entry:
;	ESI -> PortInformation struct
; Exit:
;	NONE
; Uses:
;	C style
;
;******************************************************************************
BeginProc ReleaseCOMPort,PUBLIC

	mov	ecx,[esi.VCD_Data]
	jecxz	RCP_Done

RCP_OwnerShipSet:
	cCall	[esi.ContentionHnd],<RELEASE_RESOURCE, ecx, \
			OFFSET32 _Notify_Loss>

RCP_Done:
	ret

EndProc ReleaseCOMPort

;******************************************************************************
;
; StealPort
;
; Description:
;	Calls VCOMM to see if we can get the port.
; Entry:
;	ESI -> DEB
; Exit:
;	NZ if success, else failure
; Uses:
;	C style - ECX
;******************************************************************************
BeginProc StealPort,PUBLIC

	push	ecx

	TestMem	[esi.pData.LossByte],1	; Q: Have we lost the port ?
	jz	SP_Success		;    N:

	cCall	[esi.ContentionHnd],<STEAL_RESOURCE,[esi.VCD_Data], \
				OFFSET32 _Notify_Loss>
	test	eax,eax			; Q: Did it succeed ?
	jz	SP_Done			;    N:
	ClrFlag	[esi.pData.LossByte],1	;    Y: say we own the port

SP_Success:
	test	esi,esi			; esi -> Port handle

SP_Done:
	pop	ecx
	ret

EndProc StealPort

;****
;
; WaitForXmitEmpty
;
; Description:
;	We need to wait for the transmit register to empty. However, 
;	with some comm cards, this seems impossible (Doesn't work 
;	with win3.1 either). Maybe has something to do with using 
;	the FIFO. Anyway, now we use a timeout of 200 ms for such
;	machines so that we don't hang.
; Entry:
;	EDX = Line status register.
; Exit:
;	None
; Uses:
;	Flags.
;
BeginProc WaitForXmitEmpty

	push	ecx
	push	eax

	sti

	VMMCall	Get_Last_Updated_System_Time
	mov	ecx, eax
	add	ecx, 200		; wait for 200 ms.

wfxe_loop:
	in	al, dx			; read line status
	and	al, ACE_THRE+ACE_TSRE
	cmp	al, ACE_THRE+ACE_TSRE
	je	@F
	VMMCall	Get_Last_Updated_System_Time
	cmp	eax, ecx
	jb	wfxe_loop
@@:
	pop	eax
	pop	ecx
	ret

EndProc WaitForXmitEmpty

;******************************************************************************
;
; Terminate
;
; Description:	terminate device
;		Restore the port I/O address and make sure that interrupts
;		are off.
;
; Entry:
;		EDX = device port address
;		ESI -> PortInformation struct
;
; Exit:
;		EAX = 0 => success
;		EAX = -1 => error
; Uses:
;		EAX,EBX,EDX,EDI,FLAGS.
;==============================================================================
BeginProc Terminate, NO_PROLOG, PUBLIC

	test	[esi.pData.LossByte],1		; Q: Do we have the port ?
	jz	Terminate_SerialOwnsPort	;    Y: Continue
	call	StealPort			;    N: Try to steal it
	jz	Terminate_LostPort		;       Couldn't steal it

Terminate_SerialOwnsPort:
  ;
  ;
  ;   When the OUT2 bit is reset to 0 to disable interrupts, many ports
  ;   generate an interrupt which can not be identified, because the the
  ;   interrupt ID register will not be set.  To work around this hardware
  ;   problem we first mask the IRQ, then set the port into loopback mode
  ;   and output a NULL to generate a receive interrupt request.  Then we
  ;   reset OUT2 and unmask the IRQ.  This will cause the interrupt to occur
  ;   and the interrupt handler will be able to correctly identify the
  ;   interrupt as coming from the com port.
  ;
	inc	edx			; disable chip interrupts
	.errnz	ACE_IER-ACE_RBR-1
	mov	al, ACE_ERBFI		; except receive
	out	dx,al

	add	dl,ACE_LSR-ACE_IER	; --> line status register
	IO_Delay

	call	WaitForXmitEmpty

	xor	al,al
	test	[esi.EFlags],fFIFOpre	; Q: leave FIFO enable?
	jz	@F
	mov	al,ACE_TRIG08 OR ACE_EFIFO OR ACE_CRFIFO OR ACE_CTFIFO
@@:
	sub	dl,ACE_LSR-ACE_FCR
	out	dx,al
	IO_Delay
	call	MaskIRQ
	add	dl,ACE_MCR-ACE_FCR	; --> Modem control reg
	in	al,dx
	IO_Delay
	mov	ah, al
	or	al, ACE_LOOP		; turn on loopback
	out	dx, al
	IO_Delay
	sub	dl, ACE_MCR-ACE_THR
	xor	al, al
	out	dx, al			; output a NULL to generate an int
	IO_Delay
	add	dl, ACE_LSR-ACE_THR

	call	WaitForXmitEmpty

	mov	al, ah
	dec	dl			; now clear OUT2 and loopback
	.errnz	ACE_LSR-ACE_MCR-1
	and	al,ACE_DTR+ACE_RTS	; leave DTR,RTS high if already so
	out	dx,al			; but tri-state IRQ line

	call	UnmaskIRQ		; this will cause the receive int
					; to occur and be processed
	sub	dl, ACE_MCR-ACE_IER	; clear the receive int enable
	xor	al, al
	out	dx, al
	dec	dl
	.errnz	ACE_IER-ACE_RBR-1

	call	MaskIRQ

Terminate_LostPort:

	push	ecx			; save these
	push	ebx

	mov	edi,[esi.MyIRQStruc]	; get our IRQ struct
	dec	[edi.VirtCnt]		; Q: Have we virtualized it > once
	jnz	TerminateUnmask		;  Y: unmask it now

	movzx	eax,[esi.IRQn]
	VxDCall	VPICD_Get_Virtualization_Count
	cmp	eax,1			; Me + default ?
	ja	TerminateUnmaskDef	;  > Me+default. 
	cmp	[edi.OldMask], 0	; Q: Was IRQ unmasked to begin with ?
	jne	TerminateSkipUnmaskDef	;    N: leave it masked.

TerminateUnmaskDef:
	call	UnMaskIRQ

TerminateSkipUnmaskDef:
	xor	eax, eax
	xchg	eax, [esi.IRQHandle]
	VxDCall	VPICD_Force_Default_Behavior
	jmp	Terminate47

TerminateUnmask:
	cli
	mov	eax, [edi.PIStruc]
	cmp	eax, esi
	jne	TerminateUnlinkLoop
	mov	eax, [eax.NextPIStruc]
	mov	[edi.PIStruc], eax
	jmp	TerminateUnlinkOut

TerminateUnlinkLoop:
	cmp	[eax.NextPIStruc], esi
	je	TerminateUnlinkIt
	mov	eax, [eax.NextPIStruc]
	jmp	TerminateUnlinkLoop

TerminateUnlinkIt:
	mov	ecx, [esi.NextPIStruc]
	mov	[eax.NextPIStruc], ecx

TerminateUnlinkOut:
	sti
	call	UnMaskIRQ		; Unmask it

Terminate47:
	pop	ebx			; original EBX
	call	ReleaseCOMPort
	pop	ecx			; original ECX

Terminate50:				; also called from IniCom ???
	xor	eax,eax			; port closed and deallocated.
	ret

EndProc Terminate

;******************************************************************************
;
; StrCmp
;
; Entry:
;	EAX -> PortInformation struct
;	ESI -> PortInfoHandle
;	EDI -> Name to look for
; Exit:
;	Z if found, else NZ
; Uses:
;	Everything except EAX,ESI,EDI
;==============================================================================
BeginProc StrCmp, PUBLIC

	push	edi
	mov	edx,[eax.MyName]	; EDX -> Name in the PortInfo struct
	xor	ecx,ecx

sc_lp:
	mov	cl,[edx]
	mov	ch,[edi]
	or	ecx,ecx
	jz	sc_success
	call	MyLower
	xchg	cl,ch
	call	MyLower
	inc	edx
	inc	edi
	cmp	cl,ch
	jz	sc_lp

sc_failed:
	or	esi,esi			; ESI won't be zero......

sc_success:
	pop	edi
	ret

EndProc StrCmp

;***
;
; MyLower
;
; Description:
;	converts an upper case letter into a lower case one.
; Entry:
;	CL = char
; Exit:
;	CL = lower cased char
; Uses:
;	CL
;
BeginProc MyLower,NO_PROLOG

	cmp	cl,'A'
	jb	ml_done
	cmp	cl,'Z'
	ja	ml_done
	or	cl,20h
ml_done:
	ret

EndProc MyLower

VxD_My_Pageable_Code_Ends

VxD_Locked_Code_Seg

BeginProc Notify_Loss,CCALL,esp

ArgVar	refData,DWORD
ArgVar	fAction,DWORD

	EnterProc

	mov	eax,RefData
	mov	ecx,fAction
	or	[eax.pData.LossByte],1	; assume loss of port
	jecxz	NL_Losing
	and	[eax.pData.LossByte],NOT 1

NL_Losing:

	LeaveProc
	return

EndProc Notify_Loss

;***
;
; MaskIRQ
;
; Description:		masks irq for the com port.
;
; Entry:		ESI -> Portinformation struct
; Exit:
;			AL = 0, if was unmasked, else -1 if already masked.
; Uses:
BeginProc MaskIRQ, NO_PROLOG, PUBLIC

	push	ebx
	push	ecx

	mov	eax,[esi.IRQHandle]
	or	eax,eax
	jz	MaskIRQ_Done			; handle doesn't exist
	mov	ebx,[SysVMHandle]
	VxDCall	VPICD_Get_Complete_Status	; get the status
	mov	al,-1				; assume masked
	test	cl,0100b			; is IRQ physically masked ?
	jnz	MaskIRQ_Done			; yes, get out
	mov	eax,[esi.IRQHandle]
	VxDCall	VPICD_Physically_Mask
	xor	al,al				; was not masked.
MaskIRQ_Done:
	pop	ecx
	pop	ebx
	ret

EndProc MaskIRQ

;***
;
; UnMaskIRQ
;
; Description:		Unmasks the irq of the port.
;
; Entry:		ESI -> PortInformation struct
; Exit:		None.
; Uses:
;
BeginProc UnMaskIRQ, NO_PROLOG, PUBLIC

	push	eax
	mov	eax,[esi.IRQHandle]
	or	eax,eax
	jz	UnMaskIRQ_Done
	VxDCall	VPICD_Physically_Unmask
UnMaskIRQ_Done:
	pop	eax
	ret

EndProc UnMaskIRQ

;***
;
; KickTx
;
; Descirption:	"Kick" the xmitter interrupt routine into operation.
;		If the transmitter holding register isn't empty, then
;		nothing needs to be done. If it is empty, then the xmit
;		interrupt needs to be enabled in the IER.
;
; Entry:	ESI-> PortInformation struct, 
;		interrupts are disabled.
;
; Exit:		None.
;
; Uses:		EAX,EDX,flags
;
BeginProc KickTx,Public

	test	[esi.pData.LossByte],1	; Q: Do we still own the port ?
	jnz	can_we_steal		;    N:

enable_int:
	mov	edx,[esi.Port]		; get device I/O address
	add	dl,ACE_IER		; --> interrupt enable register
	in	al,dx			; get current IER state
	test	al,ACE_ETBEI		; interrupts already enabled?
	jnz	KickTx10		;  yes, don't re-enable it
	or	al,ACE_ETBEI		;  No, enable it
	out	dx,al			; 8250, 8250-B requires
	IO_Delay
	out	dx,al			; writing register twice.

KickTx10:
	ret

can_we_steal:
	call	StealPort		; call VCOMM to see if we can steal
					; the port back
	jnz	enable_int		; jump, if we got it
	TRAP
;
; flush out the queue
;
	xor	eax,eax
	mov	[esi.pData.QOutCount],eax
	mov	[esi.pData.QOutMod],eax
	mov	eax,[esi.pData.QOutGet]
	mov	[esi.pData.QOutPut],eax
	ret

EndProc KickTx

;***
;
; MSRWait
;
; Description:	This routine checks the modem status register for CTS,DSR,
;		and/or RLSD signals. If a timeout occurs while checking,
;		the appropriate error code will be returned.
;
;		This routine will not check for any signal with a corresponding
;		time out value of 0 (ignore line).
; Entry:
;		ESI -> PortInformation struct
; Exit:
;		AL = error code, [esi.pData.dwCommError] updated, 
;		'Z' set if no timeout
; Uses:
;		EAX,ECX,EDX,Flags.
;
BeginProc MSRWait, PUBLIC

	push	edi

MSRRestart:
	xor	edi,edi			; init timer

MSRWait10:
	mov	ecx,11			; init delay counter (used on non-ATs)

MSRWait20:
	xor	dh,dh			; init error accumulator
	mov	eax,[esi.AddrMsrShadow]	; get address of MSR shadow
	mov	al,[eax]		; get modem status
	and	al,[esi.MSRMask]	; only leave bits of interest.
	xor	al,[esi.MSRMask]	; 0 = line high
	jz	MSRWait90		; All lines of interest are high
	mov	ah,al			; AH has 1 bits for down lines

	shl	ah,1			; line signal detect low?
	jnc	MSRWait30		;  No, it is high
	.errnz	ACE_RLSD-10000000b
	cmp	edi,[esi.ComDCB.RlsTimeout]; RLSD timeout yet?
	jb	MSRWait30		;  NO
	or	dh,CE_RLSDTO		; show modem status timeout

MSRWait30:
	shl	ah,1			; data set ready low
	shl	ah,1
	.errnz	ACE_DSR-00100000b
	jnc	MSRWait40		; No, it's high
	cmp	edi,[esi.ComDCB.DsrTimeOut]; DSR timout yet ?
	jb	MSRWait40		;  NO
	or	dh,CE_DSRTO		; show data set ready timeout

MSRWait40:
	shl	ah,1			; CTS low?
	jnc	MSRWait50		;  No, it's high
	.errnz	ACE_CTS-00010000b
	cmp	edi,[esi.ComDCB.CtsTimeout] ; cts timeout yet?
	jb	MSRWait50		;  NO
	or	dh,CE_CTSTO		; show clear to send timeout

MSRWait50:
	or	dh,dh			; any timeout yet?
	jnz	MSRWait80		;   Yes

	loop	MSRWait20		; continue until timeout
					; should have taken about 1ms.
	inc	edi			; timer+1
	jmp	MSRWait10		; until timeout or good status

MSRWait80:
	xor	ah,ah
	mov	al,dh
	or	BYTE PTR [esi.pData.dwCommError],al ; return updated status
	.errnz	HIGH CE_CTSTO
	.errnz	HIGH CE_DSRTO
	.errnz	HIGH CE_RLSDTO

MSRWait90:
	or	al,al			; set 'Z' if no timeout
	pop	edi
	ret

EndProc MSRWait

;******************************************************************************
;
; ExtCom_FN1
;
; Description:
;		similar to receiving an X-OFF char. Buffered xmission of
;		chars is halted until and X-ON char is received, or until
;		we fake that with a clear X-Off call.
; Entry:
;		ESI -> PortInformation struct
;		EDX = port base, interrupts off.
; Exit:
;		CLC.
; Uses:
;		Flags
;==============================================================================
BeginProc ExtCom_FN1, PUBLIC

	or	[esi.HSFlag],XOffReceived
PUBLIC	ExtComDummy
ExtComDummy:
	clc
	ret
EndProc ExtCom_FN1

;******************************************************************************
;
; ExtCom_FN2
;
; Description:
;		Similar to receiving X-Off char. Buffered xmission is
;		restarted.
; Entry:
;		ESI -> PortInformation struct
;		EDX = port base, interrupts disabled.
; Exit:
;		CLC
; Uses:
;		Same as KickTx + flags.
;==============================================================================
BeginProc ExtCom_FN2, PUBLIC

	and	[esi.HSFlag],NOT XoffReceived
	call	KickTx			; kick transmitter interrupts on
	clc
	ret
EndProc ExtCom_FN2

;******************************************************************************
;
; ExtCom_FN3
;
; Description:
;		Set the RTS signal active
; Entry:
;		ESI --> PortInformation struct
;		EDX = port base, interrupts OFF
; Exit:
;		CLC
; Uses:
;		EAX,EDX
;==============================================================================
BeginProc ExtCom_FN3, PUBLIC

	add	dl,ACE_MCR		; --> modem control reg
	in	al,dx			; get current settings
	or	al,ACE_RTS		; set RTS
	IO_Delay
	out	dx,al			; and update
	clc
	ret
EndProc ExtCom_FN3

;******************************************************************************
;
; ExtCom_FN4
;
; Description:
;		Set the RTS signal INACTIVE.
; Entry:
;		ESI --> PortInformation struct
;		EDX = port base, interrupts off
; Exit:
;		CLC
; Uses:
;		EDX,EAX
;==============================================================================
BeginProc ExtCom_FN4, PUBLIC

	add	dl,ACE_MCR		; --> modem control reg
	in	al,dx			; get current settings
	and	al,NOT ACE_RTS		; clear RTS
	IO_Delay
	out	dx,al			; update
	clc
	ret
EndProc ExtCom_FN4

;******************************************************************************
;
; ExtCom_FN5
;
; Description:
;		Set DTR
; Entry:
;		ESI -> PortInformation struct
;		EDX = port base, interrupts off
; Exit:
;		CLC
; Uses:
;		EAX,EDX
;==============================================================================
BeginProc ExtCom_FN5, PUBLIC

	add	dl,ACE_MCR		; --> modem control reg
	in	al,dx			; current settings
	or	al,ACE_DTR		; set DTR
	IO_Delay
	out	dx,al			; update
	clc
	ret
EndProc ExtCom_FN5

;******************************************************************************
;
; ExtCom_FN6
;
; Description:
;		Clear DTR signal
; Entry:
;		ESI -> PortInformation struct
;		EDX = port base, interrupts off
; Exit:
;		CLC
; Uses:
;		EAX,EDX
;==============================================================================
BeginProc ExtCom_FN6, PUBLIC

	add	dl,ACE_MCR		; --> Modem control reg
	in	al,dx			; current settings
	and	al,NOT ACE_DTR		; turn it off
	IO_delay
	out	dx,al			; update
	clc
	ret
EndProc ExtCom_FN6

;******************************************************************************
;
; ExtCom_FN7
;
; Description:
;		Assert the RESET line on an LPT port, useless for us.
; Entry:
;		ESI -> PortInformation struct
;		EDX = port base, interrupts off
; Exit:
;		CLC
; Uses:
;		None.
;==============================================================================
BeginProc ExtCom_FN7, PUBLIC
	sti
	clc
	ret
EndProc ExtCom_FN7

;******************************************************************************
;
; ExtCom_FN10
;
; Description:
;		Get COM port base and IRQ
; Entry:
;		EDX = port base
;		ESI -> PortInformation struct
; Exit:
;		EAX = (IRQ,base)
; Uses:
;		FLAGS,eax,edx
;==============================================================================
BeginProc ExtCom_FN10, PUBLIC

	movzx	eax,[esi.IRQn]
	shl	eax,16
	mov	ax,WORD PTR [esi.Port]
	stc
	ret
EndProc ExtCom_FN10

;******************************************************************************
;
; Notify_Owner:
;
; Description:	notifies owner of events
;
; Entry:	EAX = message ID
;		ESI -> PortInformation
; Exit:	
;		None
; Uses:
;		None.
;==============================================================================
BeginProc Notify_Owner, PUBLIC

	pushad
	or	[esi.NotifyFlagsHI],al	; AL has notifications to send
	mov	ebx,[esi.AddrEvtDWord]
	push	dword ptr [ebx]		; sub-event
	push	eax			; event
	cmp	eax,CN_EVENT
	jne	NO_NotEvent
	push	[esi.ReferenceData]	; reference data
	push	esi			; hPort
	call	[esi.NotifyHandle]
	jmp	NO_Done

NO_NotEvent:
	cmp	eax,CN_RECEIVE
	jne	NO_NotReceive
	push	[esi.ReadNotifyRefData]	; reference data
	push	esi			; hPort
	call	[esi.ReadNotifyHandle]
	jmp	NO_Done

NO_NotReceive:
	cmp	eax,CN_TRANSMIT
	jne	NO_Done
	push	[esi.WriteNotifyRefData]; reference data
	push	esi			; hPort
	call	[esi.WriteNotifyHandle]
NO_Done:
	add	esp,16			; clear stack
	popad
	ret
EndProc Notify_Owner

;******************************************************************************
;
; TimerProc
;
; Description:
;	Checks if any port is active. If a port owner wants a receive
;	notification and none has been sent and at least one byte is in the
;	queue, then it will call Notify_Owner for that port
;
; Entry:
;	EDX -> PortInfoHandle
; Exit:
;	None
; Uses:
;	ALL
;==============================================================================

BeginProc TimerProc, PUBLIC

	mov	[TimeOutHandle],0	; reset timer handle

	mov	esi,edx			; ESI -> PortinfoHandle
	VMMCall	List_Get_First

TP_Lp:
	jz	TP_Done			; end of list

	TestMem	[eax.MiscFlags],ClrTimer ; Q: Owner wants timer notifications?
	jnz	TP_Next			;   N:
	cmp	[eax.RecvTrigger],-1	; Q: Owner wants notification?
	je	TP_Next			;  N: skip notify
	cmp	[eax.pData.QInCount],0	; Q: anything in input queue ?
	je	TP_Next			;  N: skip notify
	test	[eax.NotifyFlagsHI],CN_RECEIVE ;Q: timeout notify sent ?
	jnz	TP_Next			;  N: skip notify
	xor	[eax.NotifyFlagsHI],CN_IDLE ; Q: first timer call ?
	js	TP_Next			;  Y: skip notify

	push	eax
	push	esi
	mov	esi,eax			; ESI -> PortInformation struct
	mov	eax,CN_RECEIVE		; message id.
	call	Notify_Owner
	pop	esi
	pop	eax

TP_Next:
	VMMCall	List_Get_Next
	jmp	TP_Lp

TP_Done:
	mov	eax,100
	mov	edx,esi			; RefData = PortInfoHandle
	mov	esi,OFFSET32 TimerProc
	VMMCall	Set_Global_Time_Out
	mov	[TimeOutHandle],esi	; save timer handle

	ret

EndProc TimerProc

VxD_Locked_Code_Ends

	end
