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
; Serial.ASM	VxD to provide VxD level COMM services for NS8250/16550AFN.
;******************************************************************************
;
; TITLE:	SERIAL.ASM
;
;******************************************************************************

	.386p

;******************************************************************************
;			INCLUDE files
;******************************************************************************

Create_SERIAL_Service_Table	equ	TRUE

SERIAL_Dynamic	equ	1

	.xlist
	include	VMM.INC
	include	VCOMM.INC
	include	DEBUG.INC
	include	OPTTEST.INC
	include	VPICD.INC
	include	INTERNAL.INC
	include	INS8250.INC
	include	VCD.INC
	.list

;******************************************************************************
;			Virtual Device Declaration
;******************************************************************************

Declare_Virtual_Device	SERIAL, SERIAL_Major_Version,SERIAL_Minor_Version, \
		SERIAL_Control, Undefined_Device_ID, VCD_Init_Order,,,


;******************************************************************************
;			Locked Data
;******************************************************************************
VxD_Locked_Data_Seg

		PUBLIC	Serial_Functions
		PUBLIC	PortInfoHandle
		PUBLIC	SysVMHandle

	align	4

Serial_Functions	label	DWORD
	dd	OFFSET32 _PortSetState
	dd	OFFSET32 _PortGetState
	dd	OFFSET32 _PortSetup
	dd	OFFSET32 _PortTransmitChar
	dd	OFFSET32 _PortClose
	dd	OFFSET32 _PortGetQueueStatus
	dd	OFFSET32 _PortClearError
	dd	OFFSET32 _PortSetModemStatusShadow
	dd	OFFSET32 _PortGetProperties
	dd	OFFSET32 _PortEscapeFunction
	dd	OFFSET32 _PortPurge
	dd	OFFSET32 _PortSetEventMask
	dd	OFFSET32 _PortGetEventMask
	dd	OFFSET32 _PortWrite
	dd	OFFSET32 _PortRead
	dd	OFFSET32 _PortEnableNotification
	dd	OFFSET32 _PortSetReadCallBack
	dd	OFFSET32 _PortSetWriteCallBack
	dd	OFFSET32 _PortGetModemStatus
	dd	OFFSET32 _PortGetCommConfig
	dd	OFFSET32 _PortSetCommConfig
	dd	OFFSET32 _PortGetWin32Error
	dd	0

.errnz	($ - Serial_Functions - SIZE _PortFunctions)

	align	4

PortInfoHandle	dd	0
SysVMHandle		dd	0

SrvTab	label	dword
	dd	OFFSET32	ModemStatus	; [0] modem status interrupt
	dd	OFFSET32	XmitEmpty	; [4] Tx holding reg. interrupt
	dd	OFFSET32	DataAvail	; [8] Rx data avail or
						; [18] if 16550/16550A
	dd	OFFSET32	LineStat	; [C] receiver line status int

ExtTab	label	dword
	dd	OFFSET32 ExtComDummy	; function 0: never mind
	dd	OFFSET32 ExtCom_FN1	; Set X-Off
	dd	OFFSET32 ExtCom_FN2	; clear X-Off
	dd	OFFSET32 ExtCom_FN3	; Set RTS
	dd	OFFSET32 ExtCom_FN4	; Clear RTS
	dd	OFFSET32 ExtCom_FN5	; Set DSR
	dd	OFFSET32 ExtCom_FN6	; Clear DSR
	dd	OFFSET32 ExtCom_FN7	; Reset Printer
	dd	OFFSET32 ExtComDummy	; Get Max LPT port
	dd	OFFSET32 ExtComDummy	; Get Max COM Port
	dd	OFFSET32 ExtCom_FN10	; Get COM port base and IRQ
	dd	OFFSET32 ExtCom_FN10	; Get COM port base & IRQ
	dd	OFFSET32 ExtCom_FN12	; set break
	dd	OFFSET32 ExtCom_FN13	; clear break

BaudRateByIndexTable label word
	dw 1047     ; CBR_110
	dw 384	    ; CBR_300
	dw 192	    ; CBR_600
	dw 96	    ; CBR_1200
	dw 48	    ; CBR_2400
	dw 24	    ; CBR_4800
	dw 12	    ; CBR_9600
	dw 8	    ; CBR_14400
	dw 6	    ; CBR_19200
	dw 0	    ;	 0FF19h  (reserved)
	dw 0	    ;	 0FF1Ah  (reserved)
	dw 3	    ; CBR_38400
	dw 0	    ;	 0FF1Ch  (reserved)
	dw 0	    ;	 0FF1Dh  (reserved)
	dw 0	    ;	 0FF1Eh  (reserved)
	dw 2	    ; CBR_56000

VxD_Locked_Data_Ends

;******************************************************************************
;			Locked code
;******************************************************************************

VxD_Locked_Code_Seg

;******************************************************************************
;
; SERIAL_Control
;
; Description:		Dispatcher for system messages.
;
; Entry:
; Exit:
; Uses:
;******************************************************************************

BeginProc SERIAL_Control

	Control_Dispatch VM_Not_Executeable, Serial_Not_Executeable
	Control_Dispatch SYS_DYNAMIC_DEVICE_INIT, Serial_Device_Init

IFDEF	DEBUG
	Control_Dispatch Debug_Query,	Serial_Debug_Query
ENDIF
	clc
	ret

EndProc SERIAL_Control

VxD_Locked_Code_Ends

VxD_My_PAGEABLE_DATA_Seg

		PUBLIC	COM1Name

COM1Name	db	'COM1',0
COM2Name	db	'COM2',0
COM3Name	db	'COM3',0
COM4Name	db	'COM4',0

VxD_My_PAGEABLE_DATA_Ends

;******************************************************************************
;		PAGEABLE DATA
;==============================================================================

VxD_My_Pageable_Data_Seg


		PUBLIC	Serial_Name

Serial_Name	db	'SERIAL',0

VxD_My_Pageable_Data_Ends

;******************************************************************************
;		PAGEABLE CODE
;==============================================================================

VxD_My_Pageable_Code_Seg

		EXTRN	GetCOMPort:NEAR
		EXTRN	ReleaseCOMPort:NEAR
		EXTRN	Serial_Device_Init:NEAR
		EXTRN	Terminate:NEAR
		EXTRN	MaskIRQ:NEAR
		EXTRN	UnMaskIRQ:NEAR
		EXTRN	KickTx:NEAR
		EXTRN	MSRWait:NEAR
		EXTRN	Notify_Owner:NEAR
IFDEF	DEBUG
		EXTRN	Serial_Debug_Query:NEAR
ENDIF
		EXTRN	ExtComDummy:NEAR
		EXTRN	ExtCom_FN1:NEAR
		EXTRN	ExtCom_FN2:NEAR
		EXTRN	ExtCom_FN3:NEAR
		EXTRN	ExtCom_FN4:NEAR
		EXTRN	ExtCom_FN5:NEAR
		EXTRN	ExtCom_FN6:NEAR
		EXTRN	ExtCom_FN7:NEAR
		EXTRN	ExtCom_FN10:NEAR

 		EXTRN	_PortSetState:NEAR
 		EXTRN	_PortGetState:NEAR
 		EXTRN	_PortSetup:NEAR
 		EXTRN	_PortTransmitChar:NEAR
 		EXTRN	_PortClose:NEAR
 		EXTRN	_PortGetQueueStatus:NEAR
 		EXTRN	_PortClearError:NEAR
 		EXTRN	_PortSetModemStatusShadow:NEAR
 		EXTRN	_PortGetProperties:NEAR
 		EXTRN	_PortEscapeFunction:NEAR
 		EXTRN	_PortPurge:NEAR
 		EXTRN	_PortSetEventMask:NEAR
 		EXTRN	_PortGetEventMask:NEAR
 		EXTRN	_PortWrite:NEAR
 		EXTRN	_PortRead:NEAR
 		EXTRN	_PortEnableNotification:NEAR
 		EXTRN	_PortOpen:NEAR
 		EXTRN	_PortSetReadCallBack:NEAR
 		EXTRN	_PortSetWriteCallBack:NEAR
 		EXTRN	_PortGetModemStatus:NEAR
		EXTRN	_PortGetCommConfig:NEAR
		EXTRN	_PortSetCommConfig:NEAR
		EXTRN	_PortGetWin32Error:NEAR

VxD_My_Pageable_Code_Ends

VxD_Locked_Code_Seg

;***
; SetCom100
;
; Description:
;	Copy a given DCB into the appropriate DEB. 
;	This does it selectively.
;
; Entry:
;		EBX -> DCB
;		ESI -> PortInformation
;		ECX = mask of relevant fields.
;
; Exit:		ESI -> PortInformation
;
; Uses:
;		EAX,ECX,FLAGS
;
BeginProc SetCom100, PUBLIC

	push	esi			; save ESI
	lea	esi,[esi.ComDCB]	; to reduce code size

	mov	[esi._DCB.DCBLength], SIZE _DCB
	TestReg	ecx,fBaudRate		; is caller interested in baud rate?
	jz	@F
	mov	eax,[ebx._DCB.BaudRate]
	mov	[esi._DCB.BaudRate],eax
@@:
	TestReg	ecx,fBitMask
	jz	@F
	mov	eax,[ebx._DCB.Bitmask]
	mov	[esi._DCB.Bitmask],eax
@@:
	TestReg	ecx,fXonLim
	jz	@F
	mov	eax,[ebx._DCB.XonLim]
	mov	[esi._DCB.XonLim],eax
@@:
	TestReg	ecx,fXoffLim
	jz	@F
	mov	eax,[ebx._DCB.XoffLim]
	mov	[esi._DCB.XoffLim],eax
@@:
	TestReg	ecx,fLCR
	jz	@F
	mov	al,[ebx._DCB.ByteSize]
	mov	[esi._DCB.ByteSize],al
	mov	al,[ebx._DCB.Parity]
	mov	[esi._DCB.Parity],al
	mov	al,[ebx._DCB.StopBits]
	mov	[esi._DCB.StopBits],al
@@:
	TestReg	ecx,fXonChar
	jz	@F
	mov	al,[ebx._DCB.XonChar]
	mov	[esi._DCB.XonChar],al
@@:
	TestReg	ecx,fXoffChar
	jz	@F
	mov	al,[ebx._DCB.XoffChar]
	mov	[esi._DCB.XoffChar],al
@@:
	TestReg	ecx,fErrorChar
	jz	@F
	mov	al,[ebx._DCB.ErrorChar]
	mov	[esi._DCB.ErrorChar],al
@@:
	TestReg	ecx,fEofChar
	jz	@F
	mov	al,[ebx._DCB.EofChar]
	mov	[esi._DCB.EofChar],al
@@:
	TestReg	ecx,fEvtChar1
	jz	@F
	mov	al,[ebx._DCB.EvtChar1]
	mov	[esi._DCB.EvtChar1],al
@@:
	TestReg	ecx,fEvtChar2
	jz	@F
	mov	al,[ebx._DCB.EvtChar2]
	mov	[esi._DCB.EvtChar2],al
@@:
	TestReg	ecx,fRlsTimeout
	jz	@F
	mov	eax,[ebx._DCB.RlsTimeout]
	mov	[esi._DCB.RlsTimeout],eax
@@:
	TestReg	ecx,fDsrTimeout
	jz	@F
	mov	eax,[ebx._DCB.DsrTimeout]
	mov	[esi._DCB.DsrTimeout],eax
@@:
	TestReg	ecx,fCtsTimeout
	jz	@F
	mov	eax,[ebx._DCB.CtsTimeout]
	mov	[esi._DCB.CtsTimeout],eax
@@:
	TestReg	ecx,fTxDelay
	jz	@F
	mov	eax,[ebx._DCB.TxDelay]
	mov	[esi._DCB.TxDelay],eax
@@:
	mov	ax,[ebx._DCB.wReserved]
	mov	[esi._DCB.wReserved],ax

	pop	esi
	ret

EndProc SetCom100

;***
; SetCom200
;
; Description:	Based on whether or not a timeout has been specified for
;		each signal, set up a mask byte which is used to mask off
;		lines for which we wish to detect timeouts. 0 indicates that
;		the line is to be ignored.
;
;		Also set up mask to indicate those lines which are set for
;		infinite timeout. -1 indicated that.
;
; Entry:	EBX -> DCB
;		ESI -> PortInformation
; Exit:
;		EBX -> DCB32
;		AH = lines to check
;		AL = lines with infinite timeout
;
; Uses:
;		EAX,ECX,FLAGS
;***************************************************************************** 
BeginProc SetCom200, PUBLIC
	xor	eax,eax		; get mask of lines with timeout = 0
	xor	ecx,ecx
	call	SetCom210
	not	al		; invert result to get lines to check
	and	al,ACE_CTS+ACE_DSR+ACE_RLSD
	xchg	ah,al
	dec	ecx		; get mask of infinite timeouts

SetCom210:
	cmp	[ebx._DCB.RlsTimeout],ecx ; timeout set to passed value ?
	jne	SetCom220		; No
	or	al,ACE_RLSD		; Yes, show checking line

SetCom220:
	cmp	[ebx._DCB.CtsTimeOut],ecx ; Timeout set to passed value ?
	jne	SetCom230		; No
	or	al,ACE_CTS		; Yes, show checking line

SetCom230:
	cmp	[ebx._DCB.DsrTimeOut],ecx ; Timeout set to passed value ?
	jne	SetCom240		; No
	or	al,ACE_DSR		; Yes, show checking line

SetCom240:
	ret

EndProc SetCom200

;***
; SetCom300
;
; Description:
;	Calculate the correct BAUDRATE divisor for the COMM chip
;
;	Note that the baudrate is allowed to be any integer in the
;	range 2-19200. The divisor is computed as 115,200/baudrate.
;
; Entry:
;	EBX -> DCB32
; Exit:
;	EBX -> DCB32
;	ECX = baudrate (0 if error AX = error code if invalid baud rate)
; Uses:
;	EAX,ECX, flags
;
BeginProc SetCom300, PUBLIC

	push	edx
	mov	ecx,[ebx._DCB.BaudRate]	; get requested baud rate
	xor	eax,eax			; assume error
	cmp	ecx,1C200h
	je	SetCom115200
	cmp	ecx,CBR_110		; Q: baudrate specified as an index?
	jae	by_index		;  Y:
	cmp	ecx,2			;  N: by value (range check it)
	jnae	SetCom310		; Below range

SetCom115200:
	xor	edx,edx			; EDX:EAX = 115,200
	mov	eax,01C200h
	div	ecx			; (EAX) = 115,200/baud

SetCom310:
	mov	ecx,eax			; (ECX) = baud rate, or error code (0)
	mov	eax,IE_BAUDRATE		; set error code incase bad baud
	pop	edx
	ret

by_index:
	cmp	ecx,CBR_56000		; Q: above supported?
	ja	SetCom115200		;  Y: return error
	push	ebx
	mov	ebx,ecx
	sub	ebx,CBR_110
	shl	ebx,1
	movzx	eax,WORD PTR [ebx+BaudRateByIndexTable] ; get divisor
	pop	ebx
	jmp	SetCom310		; Y: return error

EndProc SetCom300

;***
; SetCom400
;
; Description:	Check the line config. (parity,stop,byte size)
;
; Entry:	EBX -> DCB32
; Exit:
;		EBX -> DCB32
;		NC -> OK, else error (AX = error code)
;		AL = LCR, AH = RxMask
;		EDI (Bits 15:8) = flags mask (to remove parity checking)
;		EDI (Bits 7:0) = Error mask (to remove parity error)
; Uses:
;		EAX,ECX,EDI,FLAGS
;
BeginProc SetCom400, PUBLIC

	movzx	eax,WORD PTR [ebx._DCB.ByteSize] ; al = byte size, ah = parity
	cmp	ah,SpaceParity		; parity out of range >
	ja	SetCom470		;  Yes, return error
	mov	edi,0FF00h+ACE_OR+ACE_PE+ACE_FE+ACE_BI
	or	ah,ah			; is parity 'NONE'?
	jnz	SetCom410		;  No, something is there for parity
	xor	edi,(fParity*256)+ACE_PE ; disable parity checking

SetCom410:
	cmp	al,8			; Byte size out of range?
	ja	SetCom460		;  Yes, return error

SetCom420:
	sub	al,5			; Shift byte size to bits 0,1
	.errnz	ACE_WLS-00000011b	; Word length must be these bits
	jc	SetCom460		; Byte size is illegal, return error
	add	ah,ah			; map parity to ACE bits
	jz	SetCom430		; 0=>0,1=>1,2+.3,3=>5,4=>7
	dec	ah

SetCom430:
	shl	ah,3			; align with 8250 parity bits
	or	al,ah			; add to byte size

	.errnz	NoParity-0
	.errnz	OddParity-1
	.errnz EvenParity-2
	.errnz MarkParity-3
	.errnz SpaceParity-4
	.errnz ACE_PEN-00001000b
	.errnz ACE_PSB-00110000b
	.errnz ACE_EPS-00010000b
	.errnz	ACE_SP-00100000b

	or	al,ACE_2SB		; Assume 2 stop bits
	mov	ah,[ebx._DCB.StopBits]	; get # of stop bits 0=1,1/2= .GT. 1
	or	ah,ah			; out of range ?
	js	SetCom470		;  Yes, return error
	jz	SetCom440		; One stop bit
	sub	ah,2
	jz	SetCom450		; 2 stop bits
	jns	SetCom470		; Not 1.5, return error
	test	al,ACE_WLS		; 1.5 stop bits, 5 bit words?
	jnz	SetCom470		;  No, illegal

	.errnz OneStopBit-0
	.errnz One5StopBits-1
	.errnz TwoStopBits-2
	.errnz ACE_5BW

SetCom440:
	and	al,NOT ACE_2SB		;Show 1 (or 1.5) stop bit(s)

 ;
 ; From the byte size, get a mask to be used for stripping
 ; off unused bits as the characters are received.
 ;
SetCom450:
	push	edx
	mov	cl,[ebx._DCB.ByteSize]	;Get data byte size
	mov	edx,00FFh		;Turn into mask by shifting bits
	shl	edx,cl
	mov	ah,dh			;Return mask in ah
	pop	edx
	clc				;Show all is fine
	ret

SetCom460:
	mov	eax,IE_ByteSize		;Show byte size is wrong
	stc				;Show error
	ret

SetCom470:
	mov	eax,IE_Default		;Show something is wrong
	stc				;Show error
	ret

EndProc SetCom400

VxD_Locked_Code_Ends

VxD_My_Pageable_Code_Seg

BeginDoc
;******************************************************************************
;
; RecCom:
;
; Description:
;		Receive a byte from channel (if available).
;
; Entry:
;	ESI -> PortInformation struct
; Exit:
;	'Z' clear if data available
;	AL = byte
;	Else if 'Z' set, no data/ error, AX = error code AX = 0 => no data
;
; Uses:
;	C standard
;
;==============================================================================
EndDoc
BeginProc RecCom, Public

	push	esi
	push	edi
 ;
 ; Before removing any characters from the input queue, check to see
 ; if XON needs to be issued. If it needs to be issued, set the
 ; flag that will force it and arm transmit interrupts.
 ;
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck>	; Enq ot Etx ack?
	jz	RecCom32		; No
	test	[esi.HSFlag],EnqReceived+HHSDropped ; Enq recvd/lines dropped?
	jz	RecCom60		; No
	jmp	RecCom34

RecCom32:
	test	[esi.HSFlag],HSSent	; handshake sent?
	jz	RecCom60		;  No XOFF sent & no lines dropped

RecCom34:
	mov	eax,[esi.pData.QInCount] ; get current count of input chars
	cmp	eax,[esi.ComDCB.XonLim]	; see if at Xon limit
	ja	RecCom60		; Not yest

 ;
 ; If any hardware lines are down, then raise them. Then see
 ; about sending XON.
 ;
	mov	edx,[esi.Port]		; get the port
	mov	ah,[esi.HHSLines]	; get hardware lines mask
	cli				; handle this as a critical section
	mov	cl,[esi.HSFlag]		; get handshaking flags
	or	ah,ah			; any hw lines to play with ?
	jz	RecCom40		;  NO
	add	dl,ACE_MCR		;  --> modem control register
	in	al,dx
	or	al,ah			; Turn on the hardware bits
	IO_Delay
	IO_Delay
	out	dx,al
	and	cl,NOT HHSDropped	; show hardware lines back up

RecCom40:
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ; enq or Etx Ack?
	jz	RecCom47		; No
	test	cl,EnqReceived		; did we receive Enq?
	jz	RecCom55		; No
	and	cl,NOT EnqReceived
	jmp	RecCom50

RecCom47:
	test	cl,XOffSent		; Did we send XOFF?
	jz	RecCom55		;  No
	and	cl,NOT XoffSent		; remove XOFF sent flag

RecCom50:
	or	cl,XONPending		; show XON or ACK must be sent
	call	KickTx			; kick xmit if needed

RecCom55:
	mov	[esi.HSFlag],cl		; store handshaking flag
	sti				; can allow interrupts now

 ;
 ; Now we can get down to the business at hand, and remove a character
 ; from the receive queue. If a communications error exists, we return
 ; that and nothing else.
 ;
RecCom60:
	xor	eax,eax
	or	eax,[esi.pData.dwCommError] ; any errors ?
	jnz	RecCom100		;  Yes, return error code
	or	eax,[esi.pData.QInCount] ; get current input char count
	jz	RecCom90		; No chars in q
	mov	edi,[esi.pData.QInAddr]	; get q pointer

	mov	ebx,[esi.pData.QInGet]	; also get the index to head
	mov	al,[ebx+edi]		; finally get the byte from queue
	inc	ebx			; update q index
	cmp	ebx,[esi.pData.QInSize]	; wrap-around ?
	jc	RecCom70		;  No wrap
	xor	ebx,ebx			; wrap by zeroing the index

RecCom70:
	mov	[esi.pData.QInGet],ebx	; save new head pointer
	dec	[esi.pData.QInCount]		; dec # of bytes in queue

	mov	ecx,[esi.pData.QInCount]; Q: have we read below trigger
	jae	RecCom80		;  N:
	and	[esi.NotifyFlagsHI], NOT CN_RECEIVE ;allow timeout notify again

RecCom80:
	or	esp,esp			; reset PSW.Z
	pop	edi
	pop	esi
	ret

;
; No characters in the input queue. Check to see if EOF was received, and
; return it if it was. Otherwise show no characters.
;
RecCom90:
	TestMem	[esi.ComDCB.BitMask],fBinary	; are we doing binary stuff?
	jnz	RecCom95		;  Yes, show no characters
	mov	al,[esi.ComDCB.EofChar]	; assume eof
	test	[esi.EFlags],fEOF	; has end of file char been received?
	jnz	RecCom80		; show no more characters

RecCom95:
	xor	eax,eax			; Show no more characters

; Return with 'Z' to show error or no characters

RecCom100:
	xor	ecx,ecx			; set PSW.Z
	pop	edi
	pop	esi
	ret

EndProc RecCom

VxD_My_Pageable_Code_Ends

VxD_Locked_Code_Seg

;***
; TXI - xmit a char immediately.
;	Places the char in a location that guarantees it to be the next
;	char transmitted.
; Entry:
;	AH = char
;	ESI -> DEB
; Exit:
;	None
; Uses:
;	C standard
;
BeginProc TXI, PUBLIC, NO_PROLOG

	or	[esi.EFlags],fTxImmed	; show char to xmit
	mov	[esi.ImmedChar],ah	; set char
	jmp	KickTx			; kick transmit interrupt just in case

EndProc TXI

BeginDoc
;******************************************************************************
;
; StaCom:
;
; Description:
;		returns status of open channel.
;		Returns the number of bytes in both queues.
; Entry:
;		ESI = Portinformation struct
;		EBX -> Ptr to status structure to be updated.
; Exit:
;		AX = error word
;		status structure is updated.
; Uses:
;		C standard
;
;==============================================================================
EndDoc
BeginProc StaCom, Public

	or	ebx,ebx			; Null pointer ?
	jz	StaCom25		; yes, return error code
;
; Need to get the status for a com port.  Since not all the
; status is contained within EFlags, it has to be assembled.
; Also note that currently there is no way to specify RLSD
; as a handshaking line, so fRLSDHold is always returned false.
;
	mov	edx,[esi.AddrMSRShadow]
	mov	al,[edx]		;Get state of hardware lines
	and	al,[esi.OutHHSLines]	;Mask off required bits
	xor	al,[esi.OutHHSLines]	;1 = line low
	shr	al,4			;align bits, al = fCTSHold + fDSRHold
	.errnz	ACE_CTS-00010000b
	.errnz	ACE_DSR-00100000b
	.errnz	fCTSHold-00000001b
	.errnz	fDSRHold-00000010b

	mov	ah,[esi.HSFlag]		;Get fXOffHold+fXOffSent
	and	ah,XOffReceived+XOffSent
	or	al,ah

	.errnz	 XOffReceived-fXOFFHold
	.errnz	 XOffSent-fXOFFSent

	mov	ah,[esi.EFlags]		;Get fEOF+fTxImmed
	and	ah,fEOF+fTxImmed
	or	al,ah

	mov	ecx,[esi.pData.QInCount];Get input queue count
	mov	edx,[esi.pData.QOutCount] ;Get tx queue count

	movzx	eax,al
	mov	[ebx._COMSTAT.BitMask],eax
	mov	[ebx._COMSTAT.cbInQue],ecx
	mov	[ebx._COMSTAT.cbOutQue],edx

StaCom25:
	ret

EndProc StaCom

BeginDoc
;******************************************************************************
;
; ExtnFcn:
;
; Description:
;		some extended functions.
;		Functions currently implemented:
;
;	0: Dummy   - Ignored
;	1: SETXOFF - Exactly as if X-OFF character has been received.
;	2: SETXON  - Exactly as if X-ON character has been received.
;	3: SETRTS  - Set the RTS signal
;	4: CLRRTS  - Clear the RTS signal
;	5: SETDTR  - Set the DTR signal
;	6: CLRDTR  - Clear the DTR signal
;	7: RESETDEV- Yank on reset line if available (LPT devices)
;	8: GETLPTMAX - ignored
;	9: GETCOMMAX - ignored
;	10: GETCOMBASEIRQ - return base and IRQ of COMM port
;	11: GETCOMBASEIRQ1 - -do-
;	12: SETBREAK - set break condition
;	13: CLEARBREAK - clear break condition
;
; Entry:
;	ESI -> Port information EBX = function code.
; Exit:
;	EAX = Comm error word or return value of subfunction
; Uses:
;	C standard
;
;==============================================================================
EndDoc
BeginProc ExtnFcn, Public

	push	edi
	mov	edx,[esi.Port]		; get base address
	shl	ebx,2
	cli				; exclusive access.
	call	DWORD PTR [ebx+ExtTab]	; call function
	sti
	jc	ExtCom40		; jump if sub returns data in EAX
	mov	eax,[esi.pData.dwCommError]
ExtCom40:
	pop	edi
	ret

EndProc ExtnFcn

BeginDoc
;******************************************************************************
;
; ExtCom_FN12
;
; Description:
;		Sets break condition. Suspends character transmission, 
;		and places the transmission line in a break state until 
;		Clear break is called.
;		Clamps transmit data line low. Doesn't wait for the
;		transmitter holding register and shift registers to empty.
; Entry:
;		ESI -> Port data
;		EDX = port base
; Exit:
;		CLC
; Uses:
;		C standard.
;
;==============================================================================
EndDoc
BeginProc ExtCom_FN12, Public

	mov	ecx,0FF00h+ACE_SB	; will be setting break
	jmp	ClrBrk10
	.errnz	BreakSet-ACE_SB		; must be same

EndProc ExtCom_FN12

BeginDoc
;******************************************************************************
;
; ExtCom_FN13
;
; Description:
;		Clears break condition.
;		Releases any BREAK clamp on Tx data line
; Entry:
;		ESI -> port data
;		EDX -> Port base
; Exit:
;		CLC
; Uses:
;		C standard.
;
;==============================================================================
EndDoc
BeginProc ExtCom_FN13, Public

	mov	ecx,(NOT ACE_SB) SHL 8
	.errnz	BreakSet-ACE_SB		; must be same bits

ClrBrk10:
	and	[esi.HSFlag],ch		; Set or clear the BreakSet Bit
	or	[esi.HSFlag],cl
 ;
 ; CH = mask to remove bits in the line Control reg
 ; CL = mask to turn on the bits in the line control reg
 ;
	add	dl,ACE_LCR		; --> LCR
	in	al,dx			; old control value
	and	al,ch			; turn off undesired bits
	or	al,cl			; turn on desired bits
	IO_Delay
	out	dx,al			; new LCR value
	clc				; caller gets error dword
	ret

EndProc ExtCom_FN13

BeginDoc
;******************************************************************************
;
; ReadCommString
;
; Description:
;	Return immediately, read a max of n bytes.
;
; Entry:
;		ESI -> port information struct
;		EDI -> receive buffer
;		ECX = max bytes to read.
; Exit:
;		'Z' clear if data is available, AX = # bytes read.
;		'Z' set => error/no data. AX = 0 => no data.
; Uses:
;		C standard
;
;==============================================================================
EndDoc
BeginProc ReadCommString, Public

	push	esi
	push	edi

	xor	eax,eax
	TestMem	[esi.pData.MiscFlags],IgnoreCommError	; Q: ignore comm error?
	jnz	@F				; Y: no need to do this stuff.
	or	eax,[esi.pData.dwCommError]	; any errors?
	jnz	RecStr100		; Yes, return error code
@@:
	or	eax,[esi.pData.QInCount] ; any chars in input queue
	jz	RecStr90		; no chars in queue

	cmp	ecx,eax			; Q:more chars available than can read?
	jbe	RecStr30		;   N:
	mov	ecx,eax			;   Y: adjust # of chars to read
RecStr30:
	push	ecx
	mov	edx,[esi.pData.QInSize]
	mov	eax,[esi.pData.QInGet]
	sub	edx,eax			; EDX = # of bytes before end of buf
	cmp	edx,ecx			; Q: more avail than can read?
	jbe	RecStr40		;   N:
	mov	edx,ecx			;   Y: adjust avail count
RecStr40:
	xchg	ecx,edx			; ECX = # bytes for 1st copy
	sub	edx,ecx			; EDX = # bytes for 2nd copy

	push	esi
	mov	ebx,[esi.pData.QInAddr]
	mov	esi,ebx
	add	esi,eax			; esi-> first char in buffer
	cld
	rep	movsb			; do first copy
	mov	ecx,edx
	jecxz	RecStr50		; jump if no 2nd copy needed
	mov	esi,ebx			; ESI -> start of buffer
	rep	movsb
RecStr50:
	sub	esi,ebx			; esi -> new QInGet
	mov	ebx,esi
	pop	esi
	pop	ecx
	cli
	mov	[esi.pData.QInGet],ebx	; update QInGet
	sub	[esi.pData.QInCount],ecx ; update count
	mov	eax,[esi.pData.QInCount]

	and	[esi.NotifyFlagsHI],NOT CN_RECEIVE ; allow TO notify again

	sti
 ;
 ; Check to see if XON needs to be issued. If it needs to be issued, set the
 ;  flag that will force it and arm transmit interrupts.
 ;
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ; Enq or Etx ack?
	jz	@F
	test	[esi.HSFlag],EnqReceived+HHSDropped ; Enq recvd or lines dropped?
	jz	RecStr80		; No enq recvd or no lines dropped
	jmp	RecStr60
@@:
	test	[esi.HSFlag],HSSent	; handshake sent?
	jz	RecStr80		;  NO XOFF sent & no lines dropped

RecStr60:
					; eax = current count of input chars.
	cmp	eax,[esi.ComDCB.XonLim]	; see if at XON limit
	ja	RecStr80		; No yet
 ;
 ; If any HW lines are down, raise them. Then see about sending XON.
 ;
	mov	edx,[esi.Port]		;Get the port
	mov	ah,[esi.HHSLines] 	;Get hardware lines mask
	push	ecx
	cli				;Handle this as a critical section
	mov	cl,[esi.HSFlag]		;Get handshaking flags
	or	ah,ah			;Any hardware lines to play with?
	jz	@F			;  No
	add	dl,ACE_MCR		;--> Modem control register
	in	al,dx
	or	al,ah			;Turn on the hardware bits
	IO_Delay
	IO_Delay
	out	dx,al
	and	cl,NOT HHSDropped	;Show hardware lines back up

@@:
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ;Enq or Etx Ack?
	jz	@F			;  No
	test	cl,EnqReceived		;Did we receive Enq?
	jz	RecStr70		;  No
	and	cl,NOT EnqReceived
	jmp	RecStr65

@@:
	test	cl,XOffSent		;Did we send XOFF?
	jz	RecStr70		;  No
	and	cl,NOT XOffSent 	;Remove XOFF sent flag

RecStr65:
	or	cl,XOnPending		;Show XON or ACK must be sent
	call	KickTx			;Kick xmit if needed

RecStr70:
	mov	[esi.HSFlag],cl		;Store handshake flag
	sti				;Can allow interrupts now
	pop	ecx

RecStr80:
	mov	eax, ecx
	or	esp,esp			;Reset PSW.Z
	pop	edi
	pop	esi
	ret
 ;
 ; No characters in the input queue.  Check to see if EOF
 ; was received, and return it if it was.  Otherwise show
 ; no characters.
 ;
RecStr90:
	TestMem	[esi.ComDCB.BitMask],fBinary	;Are we doing binary stuff?
	jnz	RecStr95		;  Yes, show no characters
	mov	al,[esi.ComDCB.EofChar]	;Assume EOF
	test	[esi.EFlags],fEOF 	;Has end of file char been received?
	jnz	RecStr80		;  Yes, show end of file

RecStr95:
	xor	eax,eax			;Show no more characters

; Return with 'Z' to show error or no characters

RecStr100:
	xor	ecx,ecx			;Set PSW.Z
	pop	edi
	pop	esi
	ret

EndProc ReadCommString

BeginDoc
;******************************************************************************
;
; WriteCommString
;
; Description:
;		The given buffer is sent to the passed port if possible.
;	Once the output queue is detected as being full, a CE_TXFULL error
;	will be indicated and AX will be returned as the # of chars actually
;	queued.
;
; Entry:
;	ESI -> Portinformation struct
;	ECX = # bytes to write
;	EDI -> buffer.
; Exit:
;	EAX = # bytes queued.
; Uses:
;	ALL
;
;==============================================================================
EndDoc
BeginProc WriteCommString, Public

	push	ecx			; save count
	call	MSRWait			; see if lines are correct for output
	pop	ecx
	push	ecx
	jnz	cws_error		; timeout occured, return error
	mov	edx,[esi.pData.QOutSize] ; see if queue is full
	sub	edx,[esi.pData.QOutCount] ; edx = # of chars free in queue
	jle	scs_full		;  There is no room in the queue

scs_loop:
	push	ecx			; save count left to send
	cmp	ecx,edx			; Q: room for buffer in queue
	jbe	@F			;   Y:
	mov	ecx,edx			;   N: adjust size to send
@@:
	push	ecx			; save # of chars which will be copied
	push	esi
	push	edi
	mov	ebx,[esi.pData.QOutAddr] ; --> output queue
	mov	edx,[esi.pData.QOutSize]
	mov	edi,[esi.pData.QOutPut]	; get index into queue
	sub	edx,edi			; EDX = # of free chars befor end of q
	cmp	edx,ecx
	jbe	@F
	mov	edx,ecx
@@:
	xchg	edx,ecx			; ECX = # of chars for 1st copy
	sub	edx,ecx			; EDX = # of chars for 2nd copy
	pop	esi			; ESI -> SRC buffer
	add	edi,ebx			; EDI -> current pos in q
	cld
	rep	movsb			; copy fist section
	mov	ecx,edx
	jecxz	@F
	mov	edi,ebx			; circle back to start of queue
	rep	movsb
@@:
	sub	edi,ebx			; EDI = last index into queue
	mov	edx,edi
	mov	edi,esi			; last location in src buffer
	pop	esi			; ESI --> COMDEB
	pop	ebx			; # of chars copied
	cli
	mov	[esi.pData.QOutPut],edx	; new index into queue
	add	[esi.pData.QOutCount],ebx
	mov	edx,[esi.pData.QOutCount] ; get new count
	cmp	edx,[esi.SendTrigger]	; Q: Have we overshot the trigger ?
	jb	scs_stillbelowtrigger	;    N: do not start looking
	and	[esi.NotifyFlagsHI], NOT CN_TRANSMIT ; start looking now

scs_stillbelowtrigger:
	call	KickTx
	sti
	pop	ecx
	sub	ecx,ebx			; # of chars left to send
	jnz	scs_full_2		;  jump if none
scs_exit:
	pop	eax
	sub	eax,ecx			; EAX = # transfered
	ret

scs_full:
	cli
	call	KickTx
	sti
scs_full_2:
	or	[esi.pData.dwCommError],CE_TXFULL
	jmp	scs_exit

cws_error:
	pop	eax
	sub	eax,ecx			; EAX = # transfered
cws_exit:
	ret

EndProc WriteCommString

VxD_Locked_Code_Ends

VxD_My_Pageable_Code_Seg

BeginDoc
;******************************************************************************
;
; TrmCom:
;
; Description:
;		terminates (closes) a channel.
;		Waits for any outbound data to be transmitted, drop the
;		hardware handshaking lines, and disable interrupts. If the
;		output queue contained data when it was closed, an error
;		will be returned.
; Entry:
;		ESI -> PortInformation struct
; Exit:
;		EAX = 0	=> no error
;		EAX = 8000h	=> invalid device ID
;		EAX = -2 => output queue timed out.
; Uses:
;		Everything
;
;==============================================================================
EndDoc
BeginProc TrmCom, Public

	or	[esi.MiscFlags],Discard	; Show discarding serial data
	xor	ecx, ecx
	mov	[esi.pData.dwCommError],ecx	; Clear error flaggs.
	mov	[esi.pData.QInCount],ecx ; show no chars in input queue
	call	RecCom			; send XON if needed.

;
; We have to wait for the output queue to empty.   To do this,
; a timer will be created.  If no character has been transmitted
; when the timeout occurs, then an error will be indicated and
; the port closed anyway.  If the timer cannot be created, then
; just loop until the queue empties, which will be better than
; discarding charatcers if there are any
;
	test	[esi.HSFlag],HHSAlwaysDown ; Q: handshaking ever up ?
	jnz	TermCom17		;      N: skip wait loop

TermCom10:
	mov	ecx,[esi.pData.QOutCount] ; get current queue count
	jecxz	TermCom30		; no chars in queue

	VMMCall	Get_System_Time
	mov	edi,eax

TermCom15:
	cmp	[esi.pData.QOutCount],ecx ; queue count changed ?
	jne	TermCom10		; yes, restart timeout

	VMMCall	Get_System_Time
	sub	eax,edi
	cmp	eax, Timeout		; Q: timeout reached ?
	jb	TermCom15

TermCom17:
	mov	ecx,TimeoutError	; Yes, show timeout error

TermCom30:
	mov	edx,[esi.Port]		; get port base address
	call	Terminate		; the real work is done here
	mov	eax,ecx			; set return code

	ret
EndProc TrmCom

VxD_My_Pageable_Code_Ends

;******************************************************************************
;
;		I N T E R R U P T	T I M E		C O D E
;==============================================================================

VxD_Locked_Code_Seg

BeginDoc
;******************************************************************************
;
; Flush
;
; Description:
;		Flush pending reads/writes. It does so before returning
;		to the caller.
;
; Entry:
;		ESI = PortInformation
;		EBX = 0 => transmit queue, 1=> receive queue
; Exit:
;		none.
; Uses:
;		C standard.
;
;==============================================================================
EndDoc
BeginProc Flush, Public

	push	edi

	mov	ecx,_PortData.QOutCount-_PortData.QInCount ; # of bytes to zero
	lea	edi,[esi.pData.QInCount] ; --> receive Queue data
	or	bl,bl			; xmit q ?
	jnz	Flush10			; No, input Q
	add	edi,ecx			; YES, --> xmit Q data
	mov	edx,[esi.pData.QOutCount] ; save for determining whether to
					; send Xmit notification.....
Flush10:
	cld
	shr	ecx,2			; convert to DWORDS.
	xor	eax,eax
	cli
	rep	stosd
	sti
	.errnz	_PortData.QInGet-_PortData.QInCount-4
	.errnz	_PortData.QInPut-_PortData.QInGet-4
	.errnz	_PortData.QOutCount-_PortData.QInPut-4
	.errnz	_PortData.QOutGet-_PortData.QOutCount-4
	.errnz	_Portdata.QOutPut-_PortData.QOutGet-4

        or      bl,bl                   ; Rx queue?
        jz      Flush30                 ;  No, xmit queue

;
; If the queue to be cleared is the receive queue, any
; hardware handshake must be cleared to prevent a possible
; deadlock situation.  Since we just zeroed the queue count,
; a quick call to $RecCom should do wonders to clear any
; receive handshake (i.e. send XON if needed).
;
Flush20:
	call	RecCom			;Take care of handshakes here
	and	[esi.pData.NotifyFlagsHI], NOT CN_RECEIVE
	jmp	Flush40

Flush30:
	or	edx,edx			; Q: Was there any data ?
	jz	Flush40			;  N: no need to inform anyone
	cmp	[esi.SendTrigger],0
	je	Flush40			; no need to call
	mov	eax, CN_TRANSMIT	; we have fallen below the trigger
	call	Notify_Owner		; notify the owner of the port

Flush40:
	pop	edi
	ret
EndProc Flush

;*****************************************************************************
;
; Serial_IRQHandler
;
; Description:	Serial_IRQHandler is called by VPICD with EDX -> IRQStruc
;		We walk thru all PortList structures for this IRQ and handle
;		the interrupt
; Entry:
;		EAX = IRQHandle
;		EDX -> IRQStruc ** Interrupts disabled **
; Exit:
;		CLC if handled, STC if reflect
;
; Uses:		ALL
;=============================================================================
BeginProc Serial_IRQHandler, High_Freq, PUBLIC

	mov	esi, [edx.PIStruc]		; ESI -> PortInformation
	or	esi, esi			; Q: Does it exist ?
	jz	SIRQ_Reflect			;    N: reflect it.

	cmp	[edx.VirtCnt], 1		; Q: virtualized > 1 ?
	ja	SIRQ_VirtMult_Start		;    Y:

	push	eax
	push	edx
	call	Serial_PortHandler		; Q: handled it ?
	pop	edx
	pop	eax
	jc	SIRQ_Reflect			;    N:

SIRQ_Handled_It:
	VxDCall	VPICD_Phys_EOI			;    Y: eoi it
	cmp	[edx.OldMask], 0		; Q: Was irq unmasked orig?
	je	SIRQ_Reflect			;    Y: reflect it
	ret					;    N: clears carry

SIRQ_Reflect:
	stc
	ret

SIRQ_VirtMultRestart:
	mov	esi, [edx.PIStruc]
	jmp	SIRQ_VirtMult

SIRQ_VirtMult_Start:
	xor	ebx, ebx

SIRQ_VirtMult:
	push	eax
	push	edx
	inc	bh
	xor	bl, bl

SIRQ_MultLoop:
	push	ebx
	call	Serial_PortHandler
	pop	ebx
	jc	SIRQ_NotHandledByThisOne
	or	bl, 1

SIRQ_NotHandledByThisOne:
	mov	esi, [esi.NextPIStruc]
	or	esi, esi
	jnz	SIRQ_MultLoop
	test	bl, bl				; Q: Did anyone handle it ?
	pop	edx
	pop	eax
	jnz	SIRQ_VirtMultRestart		;    Y: Try again
	dec	bh
	jz	SIRQ_Reflect
	jmp	SIRQ_Handled_It

EndProc Serial_IRQHandler

;******************************************************************************
;
; Serial_PortHandler
;
; Description:	Serial_PortHandler is called by Serial_IRQHandler with
;		ESI = PortInformation structure for the port.
;		Serial_PortHandler checks to see if the interrupt is for
;		us and processes it if so. Else, it lets it go.
;		The interrupts are prioritized in the following order
;
;		1. Line status
;		2. read data avail
;		3. xmit buffer empty
;		4. modem service interrupt
;
;		This handler continues to service till all interrupts have
;		been satisfied
; Entry:
;		ESI -> ComDEB for the port.
;		Interrupt disabled.
; Exit:
;		CLC if for us, else STC.
; Uses:
;		ALL except ESI.
;******************************************************************************

BeginProc Serial_PortHandler, High_Freq, PUBLIC

	test	[esi.pData.LossByte],1	; Q: Do we own the IRQ ?
	jnz	IntReflect		;    N: get out

	mov	edx,[esi.Port]		; get COMM I/O port
	add	dl,ACE_IIDR		; --> interrupt ID register
	in	al,dx
	test	al,1			; Q: interrupts pending
	jnz	IntReflect		;   N:
	mov	ecx,[esi.AddrEvtDWord]	; get the address
	mov	ecx,[ecx]		; get the event dword
	push	ecx
	jmp	IntLoop10

InterruptLoop_ChkTx:
	cmp	[esi.pData.QOutCount],0	; output queue empty?
	je	InterruptLoop		;  Y: Don;t chk Tx
	pop	edx
	push	edx
	dec	edx			; to IER
	.errnz	ACE_IIDR-ACE_IER-1
	in	al,dx
	and	al,NOT ACE_ETBEI	; disable it
	IO_Delay
	IO_Delay
	out	dx,al
	or	al,ACE_ETBEI		; enable it again
	IO_Delay
	IO_Delay
	out	dx,al
	IO_Delay
	IO_Delay
	IO_Delay
	out	dx,al

InterruptLoop:
	pop	edx			; Get ID reg I/O address
	in	al,dx			; get interrupt ID
	test	al,1			; interrupts need servicing ?
	jnz	IntLoop20		; NO, all done

IntLoop10:
	and	eax,07h
	push	edx			; save ID register
	jmp	[SrvTab+eax*2]		; service the interrupt

IntLoop20:
	mov	eax,[esi.EvtMask]	; mask the event dword to only the
	mov	ebx,[esi.AddrEvtDWord]	; get address
	and	eax,[ebx]		; get only user specified bits
	mov	dword ptr [ebx],eax	; set the event dword
	pop	ebx
	test	[esi.NotifyFlagsHI],CN_Notify
	jz	ci_exit
	not	ebx
	and	eax,ebx			; bits set in ax are new events
	jz	ci_exit			; no new event

ci_new_events:
	mov	eax,CN_EVENT
	call	notify_owner

ci_exit:

;
; We acquire the port by number (1-4). So if the port handle returned
; to us by VCOMM_Acquire_Port is > 4, then we assume that it is actually
; a valid VCD COM handle. In that case, we update the VCD_Last_Use for
; contention detection. If this appears dangerous, we must add a service
; to VCD which takes a handle, validates it and then updates the VCD_Last_Use
; entry.
;
	mov	edx,[esi.VCD_Data]
	or	dh,dh			; Q: is this a valid handle ?
	jz	NP_NoVCD		;    N:

	VMMCall	Get_Last_Updated_System_Time
	mov	[edx.VCD_Last_Use],eax

NP_NoVCD:
	clc				; we handled it
	ret

IntReflect:
	stc
	ret

EndProc Serial_PortHandler

;******************************************************************************
;
; ModemStatus
;
; Description:		modem status interrupt handler
;
; Entry:		ESI -> DEB
;			EDX = port.IIDR
;
; Returns:		none
; Uses:
;			EAX,EBX,ECX,EDI,FLAGS.
;
;==============================================================================
BeginProc ModemStatus,PUBLIC

; get the modem status value and shadow it for MSRWait

	add	dl,ACE_MSR-ACE_IIDR	; --> modem status register
	in	al,dx
	mov	ebx,[esi.AddrMSRShadow]
	mov	byte ptr [ebx],al	; save MSR data for others
	mov	ch,al			; save a local copy

; Create the event mask for the delta dignals

	movzx	eax,al
	mov	ah,al			; just a lot of shifting
	shr	eax,2
	shr	ah,1
	shr	eax,3
	and	eax,EV_CTS+EV_DSR+EV_RLSD+EV_Ring
	mov	ebx,[esi.AddrEvtDWord]
	or	dword ptr [ebx],eax

	mov	ah,ch
	shr	ah,2
	and	eax,EV_CTSS+EV_DSRS
	or	dword ptr [ebx],eax

	mov	ah,ch
	shr	ah,3
	and	eax,EV_RLSDS
	or	dword ptr [ebx],eax

	mov	ah,ch
	shl	ah,3
	and	eax,EV_RingTe
	or	dword ptr [ebx],eax

	.errnz	EV_CTS-0000000000001000b
	.errnz	EV_DSR-0000000000010000b
	.errnz	EV_RLSD-0000000000100000b
	.errnz	EV_Ring-0000000100000000b

	.errnz	EV_CTSS-0000010000000000b
	.errnz	EV_DSRS-0000100000000000b
	.errnz	EV_RLSDS-0001000000000000b
	.errnz	EV_RingTe-0010000000000000b

	.errnz	ACE_DCTS-00000001b
	.errnz	ACE_DDSR-00000010b
	.errnz	ACE_DRLSD-00001000b
	.errnz	ACE_RI-01000000b

	.errnz	ACE_TERI-00000100b
	.errnz	ACE_CTS-00010000b
	.errnz	ACE_DSR-00100000b
	.errnz	ACE_RLSD-10000000b

ModemStatus10:
	mov	al,[esi.OutHHSLines]	; get output HW handshake lines
	or	al,al			; any lines that must be set?
	jz	ModemStatus40		; no HW handshake or output
	and	ch,al			; mask bits of interest
	cmp	ch,al			; lines set for xmit?
	je	ModemStatus20		;  Yes
	or	[esi.HSFlag],HHSDown	; show hw lines have dropped

ModemStatus30:
	jmp	InterruptLoop

ModemStatus40:
	jmp	InterruptLoop_ChkTx

; lines are set for xmit. Kick an xmit interrupt if needed

ModemStatus20:
	and	[esi.HSFlag],NOT (HHSDown OR HHSAlwaysDown)
					; show HW lines back up.
	mov	ecx,[esi.pData.QOutCount] ; output queue empty?
	jecxz	ModemStatus30		;  Yes, return to interruptloop
	jmp	FakeXmitEmpty		; restart transmit

EndProc ModemStatus

;******************************************************************************
;
; LineStat
;
; Description:  Line Status Interrupt Handler
;		Break detection is handled and set in the event word if
;		enabled.  Other errors (overrun, parity, framing) are
;		saved for the data available interrupt.
;
;		This routine used to fall into DataAvail for the bulk of 
;		its processing. This is no longer the case...  
;		A very popular internal modem seems to operate differently 
;		than a real 8250 when parity errors occur.  Falling
;		into the DataAvail handler on a parity error caused the 
;		same character to be received twice.  Having this routine 
;		save the LSR status, and return to InterruptLoop fixes the
;		problem, and still works on real COMM ports.  The extra 
;		overhead isn't a big deal since this routine is only
;		entered when there is an exception like a parity error.
;
;		This routine is JUMPED to, and will perform a JUMP back into
;		the dispatch loop.
;
; Entry:
;		ESI --> DEB
;		EDX =  Port.IIDR
; Exit:
;		None
; Uses:
;		EAX,EBX,FLAGS
;==============================================================================

BeginProc LineStat,PUBLIC

	mov	ebx,[esi.AddrEvtDWord]
	or	BYTE PTR [ebx],EV_Err	;Show line status error

	add	dl,ACE_LSR-ACE_IIDR	;--> Line Status Register
	in	al,dx
	test	al,ACE_PE+ACE_FE+ACE_OR ;Parity, Framing, Overrun error?
	jz	@f

	mov	[esi.LSRShadow],al	;yes, save status for DataAvail
@@:
	test	al,ACE_BI		;Break detect?
	jz	InterruptLoop_ChkTx	;Not break detect interrupt

	or	BYTE PTR [ebx],EV_Break ;Show break

	jmp	InterruptLoop_ChkTx

LineStat   endp

;******************************************************************************
;
; DataAvail
;
; Description:
;		The available character is read and stored in the input queue.
;		If the queue has reached the point that a handshake is needed,
;		one is issued (if enabled).  EOF detection, Line Status errors,
;		and lots of other stuff is checked.
;
;	This routine is jumped to, and will perform a jump back into
;	the dispatch loop.
; Entry:
;		ESI -> DEB
;		EDX = Port.IIDR
; Exit:
;		None.
; Uses:
;		EAX,EBX,ECX,EDI,FLAGS
;
;==============================================================================

BeginProc DataAvail,PUBLIC

	VMMCall	Get_Last_Updated_System_Time	; update this for VCOMM
	mov	ecx,[esi.RxTimeAddr]
	mov	DWORD PTR [ecx],eax

	sub	dl,ACE_IIDR-ACE_RBR	;--> receiver buffer register
	in	al,dx			;Read received character

	and	[esi.NotifyFlagsHI], NOT CN_Idle ; flag as not idle

IFDEF	DEBUG
	inc	[esi.NumDataInts]
ENDIF
	mov	ah,[esi.LSRShadow]	;what did the last Line Status intrpt
	mov	bh,ah			;  have to say?
	or	ah,ah
	jz	DataAvailNoLSRErr

IFDEF	DEBUG
	test	ah,ACE_OR
	jz	@F
	inc	[esi.OverrunErrors]
@@:
	test	ah,ACE_PE
	jz	@F
	inc	[esi.ParityErrors]
@@:
	test	ah,ACE_FE
	jz	@F
	inc	[esi.FramingErrors]
@@:
ENDIF
	and	ah,[esi.ErrorMask]	;there was an error, record it
	or	byte ptr [esi.pData.dwCommError],ah
	mov	[esi.LSRShadow],0
	.errnz	ACE_OR-CE_OVERRUN	;Must be the same bits
	.errnz	ACE_PE-CE_RXPARITY
	.errnz	ACE_FE-CE_FRAME
	.errnz	ACE_BI-CE_BREAK

DataAvailNoLSRErr:
 ;
 ; Regardless of the character received, flag the event in case
 ; the user wants to see it.
 ;
	mov	ecx,[esi.AddrEvtDWord]
	or	byte ptr [ecx],EV_RxChar ;Show a character received
	.errnz HIGH EV_RxChar
 ;
 ; Check the input queue, and see if there is room for another
 ; character.  If not, or if the end of file character has already
 ; been received, then go declare overflow.
 ;
DataAvail00:

	mov	ecx,[esi.pData.QInCount] ;Get queue count (used later too)
	cmp	ecx,[esi.pData.QInSize]	;Is queue full?
	jge	DataAvail20		;  Yes, comm overrun
	test	[esi.EFlags],fEOF 	;Has end of file been received?
	jnz	DataAvail20		;  Yes - treat as overflow
 ;
 ; Test to see if there was a parity error, and replace
 ; the character with the parity character if so
 ;
	test	bh,ACE_PE		;Parity error
	jz	DataAvail25		;  No
	TestMem	[esi.ComDCB.BitMask],fPErrChar ;Parity error 
					;  replacement character?
	jz	DataAvail25		;  No
	mov	al,[esi.ComDCB.ErrorChar] ;  Yes, get parity replacement char
 ;
 ; Skip all other processing except event checking and the queing
 ; of the parity error replacement character
 ;
	jmp	DataAvail80		;Skip all but event check, queing

DataAvail20:
IFDEF	DEBUG
	inc	[esi.ArtificialErr]	; Due to queue overrun
ENDIF
	or	[esi.pData.dwCommError],CE_RXOVER ;Show queue overrun
	jmp	DataAvail50
 ;
 ; See if we need to strip null characters, and skip
 ; queueing if this is one.  Also remove any parity bits.
 ;
DataAvail25:
	and	al,[esi.RxMask]		;Remove any parity bits
	jnz	DataAvail30		;Not a Null character
	TestMem	[esi.ComDCB.BitMask],fNullStrip  ;Are we stripping 
					;  received nulls?
	jnz	DataAvail50		;  Yes, put char in the bit bucket
 ;
 ; Check to see if we need to check for EOF characters, and if so
 ; see if this character is it.
 ;
DataAvail30:
	TestMem	[esi.ComDCB.BitMask],fBinary ;Is this binary stuff?
	jnz	DataAvail60		;  Yes, skip EOF check
	cmp	al,[esi.ComDCB.EOFChar]	;Is this the EOF character?
	jnz	DataAvail60		;  No, see about queing the charcter
	or	[esi.EFlags],fEOF 	;Set end of file flag
DataAvail50:
	jmp	DataAvail140		;Skip the queing process
 ;
 ; If output XOn/XOff is enabled, see if the character just received
 ; is either an XOn or XOff character.  If it is, then set or
 ; clear the XOffReceived flag as appropriate.
 ;
DataAvail60:
	TestMem	[esi.ComDCB.BitMask],fOutX ;Output handshaking?
	jz	DataAvail80		;  No
	cmp	al,[esi.ComDCB.XoffChar] ;Is this an X-Off character?
	jnz	DataAvail70		;  No, see about XOn or Ack
	or	[esi.HSFlag],XOffReceived ;Show XOff received, ENQ or ETX [rkh]
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ;Enq or Etx Ack?
	jz	DataAvail50		;  No
	cmp	ecx,[esi.ComDCB.XonLim]	;See if at XOn limit
	ja	DataAvail50		;  No
	and	[esi.HSFlag],NOT XOffReceived ;Show ENQ or ETX not received
	and	[esi.HSFlag], NOT XOnPending+XOffSent
	mov	al, [esi.ComDCB.XonChar]
	call	OutHandshakingChar
	jmp	DataAvail50		;Done

DataAvail70:
	cmp	al,[esi.ComDCB.XonChar]	;Is this an XOn character?
	jnz	DataAvail80		;  No, just a normal character
	and	[esi.HSFlag],NOT XOffReceived
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ;Enq or Etx Ack?
	jz	DataAvail75		;  No - jump to FakeXmitEmpty to get
					;	transmitting going again
	and	[esi.HSFlag],NOT EnqSent

DataAvail75:
	jmp	FakeXmitEmpty		;Restart transmit
 ;
 ; Now see if this is a character for which we need to set an event as
 ; having occured. If it is, then set the appropriate event flag
 ;
DataAvail80:
	cmp	al,[esi.ComDCB.EvtChar1] ;Is it the event generating character?
	jne	DataAvail90		;  No
	mov	ebx,[esi.AddrEvtDWord]
	or	byte ptr [ebx],EV_RxFlag   ;Show received specific character
 ;
 ; Finally, a valid character that we want to keep, and we have
 ; room in the queue. Place the character in the queue.
 ; If the discard flag is set, then discard the character
 ;
DataAvail90:
	test	[esi.MiscFlags],Discard	;Discarding characters ?
	jnz	DataAvail50		;  Yes

	mov	edi,[esi.pData.QInAddr]	;Get queue base pointer

	mov	ebx,[esi.pData.QInPut]	;Get index into queue
	mov	BYTE PTR [ebx+edi],al	;Store the character
	inc	ebx			;Update queue index
	cmp	ebx,[esi.pData.QInSize]	;See if time for wrap-around
	jc	DataAvail100		;Not time to wrap
	xor	ebx,ebx			;Wrap-around is a new zero pointer

DataAvail100:
	mov	[esi.pData.QInPut],ebx	;Store updated pointer
	inc	ecx			;And update queue population
	mov	[esi.pData.QInCount],ecx
 ;
 ; If flow control has been enabled, see if we are within the
 ; limit that requires us to halt the host's transmissions
 ;
	cmp	ecx,[esi.XOffPoint]	;Time to see about XOff?
	jc	DataAvail120		;  Not yet
	test	[esi.HSFlag],HSSent	;Handshake already sent?
	jnz	DataAvail120		;  Yes, don't send it again

	mov	ah,[esi.HHSLines] 	;Should hardware lines be dropped?
	or	ah,ah			;  (i.e. do we have HW HS enabled?)
	jz	DataAvail110		;  No
	add	dl,ACE_MCR		;  Yes
	in	al,dx			;Clear the necessary bits
	not	ah
	and	al,ah
	or	[esi.HSFlag],HHSDropped	;Show lines have been dropped
	out	dx,al			;  and drop the lines
	sub	dl,ACE_MCR

DataAvail110:
	TestMem	[esi.ComDCB.BitMask],fInX	;Input Xon/XOff handshaking
	jz	DataAvail120		;  No
	or	[esi.HSFlag], XOffSent
	mov	al, [esi.ComDCB.XoffChar]
	call	OutHandshakingChar

DataAvail120:
	cmp	ecx, [esi.RecvTrigger]	;Q: time to call owner's callback?
	jb	short DataAvail130	;   N:

	test	[esi.NotifyFlagsHI], CN_RECEIVE
	jnz	short DataAvail140	; jump if notify already sent and
					;   data in buffer hasn't dropped
					;   below threshold
	push	OFFSET32 DataAvail140
	mov	eax, CN_RECEIVE
%OUT probably should just set a flag and notify after EOI
	jmp	notify_owner

DataAvail130:
	and	[esi.NotifyFlagsHI], NOT CN_RECEIVE

DataAvail140:
	pop	edx
	push	edx
	add	dl, ACE_LSR-ACE_IIDR
	in	al, dx
	test	al, ACE_DR		;Q: more data available?
	jz	@F			;   N:
	sub	dl, ACE_LSR		;   Y: go read it
	IO_Delay
	IO_Delay
	in	al, dx			;Read available character
	jmp	DataAvail00
@@:
	jmp	InterruptLoop_ChkTx

EndProc DataAvail


BeginProc OutHandshakingChar, NO_PROLOG

	add	dl, ACE_LSR
	mov	ah, al
@@:
	in	al, dx
	test	al, ACE_THRE
	jz	@B
	sub	dl, ACE_LSR
	mov	al, ah
	out	dx, al
	ret

EndProc OutHandshakingChar

;****
;
; FakeXmitEmpty
;
; Description:
;		Is JUMPED to from various places AND JUMPS out to
;		variuos places!!!
; Entry:
;		ESI -> DEB
;		EDX = Port.IIDR
; Exit:		None
; Uses:		EAX,EBX,ECX,EDI
;
BeginProc FakeXmitEmpty,PUBLIC

	pop	edx
	push	edx

; "KICK" the xmitter empty interrupt routine into operation.

	dec	dl
	.errnz	ACE_IIDR-ACE_IER-1
	in	al,dx			; get current IER state
	test	al,ACE_ETBEI		; interrupt enabled?
	jnz	@F			;   Yes, don't reenable it
	or	al,ACE_ETBEI		;   No, enable it
	out	dx,al
	IO_Delay
	IO_Delay
	out	dx,al			; 8250,8250-B bug requires 2 outs
@@:
	add	dl,ACE_LSR-ACE_IER	; --> line status reg
	IO_Delay
	IO_Delay
	in	al,dx			; is Xmit really empty?
	sub	dl,ACE_LSR-ACE_THR	; --> xmitter holding reg
	test	al,ACE_THRE
	jnz	XmitEmpty5		;  Y: send next char
	jmp	InterruptLoop		;  N: return to processing loop
EndProc FakeXmitEmpty

;******************************************************************************
;
; XmitEmpty
;
; Description:
;		handles the case whne transmitter empty interrupt occurs.
;		This is JUMPED TO AND OUT OF!!!!
;
; Entry:	EDX = Port.IIDR
;		ESI -> DEB
; Exit:		None
; Uses:		EAX,EBX,ECX,EDI,FLAGS
;
;==============================================================================
BeginProc XmitEmpty,PUBLIC

	add	dl,ACE_LSR-ACE_IIDR	;--> Line Status Register
	IO_Delay
	in	al,dx			;Is xmit really empty?
	sub	dl,ACE_LSR-ACE_THR	;--> Transmitter Holding Register
	test	al,ACE_THRE
	jz	XmitEmpty90		;Transmitter not empty, cannot send
 ;
 ; If the hardware handshake lines are down, then XOff/XOn cannot
 ; be sent.  If they are up and XOff/XOn has been received, still
 ; allow us to transmit an XOff/XOn character.  It will make
 ; a dead lock situation less possible (even though there are
 ; some which could happen that cannot be handled).
 ;
XmitEmpty5:
	mov	ah,[esi.HSFlag]		;Get handshaking flag
	test	ah,HHSDown+BreakSet	;Hardware lines down or break set?
	jnz	XmitEmpty100		;  Yes, cannot transmit

; Give priority to any handshake character waiting to be
; sent.  If there are none, then check to see if there is
; an "immediate" character to be sent.  If not, try the queue.

XmitEmpty10:
	TestMem	[esi.ComDCB.BitMask],<fEnqAck+fEtxAck> ;Enq or Etx Ack?
	jnz	XmitEmpty40		;  Yes

XmitEmpty15:
	test	ah,HSPending		;XOff or XOn pending
	jz	XmitEmpty40		;  No

XmitEmpty20:
	and	ah,NOT XOnPending+XOffSent
	mov	al,[esi.ComDCB.XonChar]	;Get XOn character

XmitEmpty30:
	mov	[esi.HSFlag],ah		;Save updated handshake flag
	jmp	XmitEmpty110		;Go output the character

; If any of the lines which were specified for a timeout are low, then
; don't send any characters.  Note that by putting the check here,
; XOff and Xon can still be sent even though the lines might be low.

; Also test to see if a software handshake was received.  If so,
; then transmission cannot continue.  By delaying the software check
; to here, XOn/XOff can still be issued even though the host told
; us to stop transmission.

XmitEmpty40:
	test	ah,CannotXmit		;Anything preventing transmission?
	jnz	XmitEmpty100		; Yes, disarm and exit

; If a character has been placed in the single character "transmit
; immediately" buffer, clear that flag and pick up that character
; without affecting the transmitt queue.

XmitEmpty45:
	test	[esi.EFlags],fTxImmed	;Character to xmit immediately?
	jz	XmitEmpty515		;  No, try the queue
	and	[esi.EFlags],NOT fTxImmed ;Clear xmit immediate flag
	mov	al,[esi.ImmedChar]	;Get char to xmit
	jmp	XmitEmpty110		;Transmit the character

XmitEmpty515:
	mov	ecx,[esi.pData.QOutCount] ;Output queue empty?
	jecxz	XmitEmpty90		;  Yes, go set an event

	test	[esi.EFlags],fNOFifo OR fNoTxFifo OR fFIFOForcedOff
					; Q: Is FIFO present ?
	jnz	XmitEmptyNoOptimize	;	       N: no optimization

	TestMem	[esi.ComDCB.BitMask],<fEtxAck OR fEnqAck> ; Q: is Etx/Enq in use?
	jz	XmitEmptyOptimize	; N: try to optimize

XmitEmptyNoOptimize:
	TestMem	[esi.ComDCB.BitMask],fEtxAck	;Etx Ack?
	jz	XmitEmpty55		;  No
	mov	ecx,[esi.QOutMod]	;Get number bytes sent since last ETX
	cmp	ecx,[esi.ComDCB.XonLim] ;At Etx limit yet?
	jne	XmitEmpty51		;  No, inc counter
	mov	[esi.QOutMod],0		;  Yes, zero counter
	or	[esi.HSFlag],EtxSent	;Show ETX sent
	jmp	XE_sendXOFF

; no more characters to transmit. Flag this as an event.

XmitEmpty90:
	mov	ebx,[esi.AddrEvtDWord]
	or	byte ptr [ebx],EV_TxEmpty
 ;
 ; Cannot continue transmitting (for any of a number of reasons).
 ; Disable the transmit interrupt.  When it's time resume, the
 ; transmit interrupt will be reenabled, which will generate an
 ; interrupt.
 ;
XmitEmpty100:
	inc	edx			;--> Interrupt Enable Register
	.errnz	ACE_IER-ACE_THR-1
	in	al,dx			;I don't know why it has to be read
	and	al,NOT ACE_ETBEI	;  first, but it works this way
XmitEmpty110:
	IO_Delay
	IO_Delay
	out	dx,al
	jmp	InterruptLoop

XmitEmpty51:
	inc	ecx			; Update counter
	mov	[esi.QOutMod],ecx	; Save counter
	jmp	XmitEmpty59		; Send queue character

XmitEmpty55:
	TestMem	[esi.ComDCB.BitMask],fEnqAck	;Enq Ack?
	jz	XmitEmpty59		;  No, send queue character
	mov	ecx,[esi.QOutMod]	;Get number bytes sent since last ENQ
	or	ecx,ecx			;At the front again?
	jnz	XmitEmpty56		;  No, inc counter
	mov	[esi.QOutMod],1		;  Yes, send ENQ
	or	[esi.HSFlag],EnqSent	;Show ENQ sent
XE_sendXOFF:
	mov	al,[esi.ComDCB.XoffChar]
	jmp	XmitEmpty110		;Go output the character

XmitEmpty56:
	inc	ecx			;Update counter
	cmp	ecx,[esi.ComDCB.XonLim]	;At end of our out buffer len?
	jne	XmitEmpty58		;  No
	xor	ecx,ecx			;Show at front again.

XmitEmpty58:
	mov	[esi.QOutMod],ecx	;Save counter

XmitEmpty59:
	mov	edi,[esi.pData.QOutAddr] ; get queue base pointer
	mov	ebx,[esi.pData.QOutGet]	; get ptr into queue
	mov	al,[ebx+edi]		; get char

	inc	ebx			; update q pointer
	cmp	ebx,[esi.pData.QOutSize] ; time for wrap-around?
	jc	XmitEmpty60		;  Nope
	xor	ebx,ebx			; wrap by zeroing index

XmitEmpty60:
	mov	[esi.pData.QOutGet],ebx	; save queue index
	mov	ecx,[esi.pData.QOutCount] ; output queue empty?
	dec	ecx			; dec # of bytes in queue
	mov	[esi.pData.QOutCount],ecx ;  and save new population

	out	dx,al			; send char

XmitEmptyMerge:

	mov	ebx,[esi.AddrEvtDWord]
	or	dword ptr [ebx],EV_TXCHAR ; set the flag so that user can see

	cmp	ecx,[esi.SendTrigger]	; Q: time to call owners' callback?
	jae	InterruptLoop		;  N:

	test	[esi.NotifyFlagsHI],CN_TRANSMIT
	jnz	InterruptLoop		; jump if notify has been sent and
					; data in buffer hasn't raised above
					; threshold
	push	OFFSET32 InterruptLoop
	mov	eax,CN_TRANSMIT
	jmp	notify_owner

EndProc XmitEmpty

PUBLIC XmitEmptyOptimize
XmitEmptyOptimize:

	movzx	ecx,[esi.pData.TxFifoTrigger]	; Max # of chars to write
	cmp	ecx,[esi.pData.QOutCount]	; Q: below # chars avail ?
	jbe	XEO_LessThanSizeOfFIFO		;    Y: go write them
	mov	ecx,[esi.pData.QOutCount]	;    N: write only so many.

XEO_LessThanSizeOfFIFO:

	sub	[esi.pData.QOutCount],ecx ; final count of chars left
	mov	edi,[esi.pData.QOutAddr] ; Base of Xmit Queue
	mov	ebx,[esi.pData.QOutGet]	; offset into q to get data from

XEO_CharBlastLoop:

	mov	al,[ebx+edi]
	out	dx,al

	inc	ebx
	cmp	ebx,[esi.pData.QOutSize]
	jc	XEO_NoWrapYet
	xor	ebx,ebx

XEO_NoWrapYet:

	loop	XEO_CharBlastLoop

	mov	[esi.pData.QOutGet],ebx
	mov	ecx,[esi.pData.QOutCount]
	jmp	XmitEmptyMerge

;******************************************************************************
;
; Serial_Not_Executeable
;
;
; VPICD masks off the physical interrupt at VM_Not_Executeable time if no
; VM has the IRQ unmasked. Since VPICD does not keep track of how many
; times a VxD has unmasked an interrupt, it masks off the interrupt we are
; handling. So, we call VPICD_Physically_Unmask for the interrupts handled by
; us.
;
; Entry:
;	EBX = VM Handle to destroy
;
; Exit:
;	Carry set if error
; Uses:
;	ALL
;==============================================================================
BeginProc Serial_Not_Executeable,PUBLIC

	mov	esi,[PortInfoHandle]
	or	esi,esi
	jz	NNE_Done			; No active ports left
	VMMCall	List_Get_First

NNE_Loop:
	jz	NNE_Done			; No more active ports
	push	eax				; save list handle
	mov	eax,[eax.IRQHandle]		; get irq handle
	or	eax, eax
	jz	@F
	VxDCall	VPICD_Physically_Unmask		; unmask it
@@:
	pop	eax
	VMMCall	List_Get_Next
	jmp	NNE_Loop

NNE_Done:
	clc
	ret

EndProc Serial_Not_Executeable

VxD_Locked_Code_Ends

	end
