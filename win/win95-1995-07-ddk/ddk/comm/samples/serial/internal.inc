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

Serial_Major_Version	equ	1
Serial_Minor_Version	equ	0

;***
;
; PortInformation - Port specific data structure.
;
;

PortInformation	struc

  pData		_PortData <>		; port data: has to be first
  ComDCB	_DCB	<>		; DCB : device control block

  Port          dd      0               ;Base I/O Address
  NotifyHandle	dd	0
  NotifyFlags	dd	0
  ReadNotifyHandle	dd	0
  ReadNotifyRefData	dd	0
  WriteNotifyHandle	dd	0
  WriteNotifyRefData	dd	0
  RecvTrigger	dd     -1		; char count threshold for calling
  SendTrigger	dd	0		; char count threshold for calling

; The following fields are specific to com ports only

  XOffPoint     dd      0               ;Q count where XOff is sent
  EvtMask       dd      0               ;Mask of events to check for
  AddrEvtDWord  dd      0               ;address of Event flags
  AddrMSRShadow dd      0               ;addr of Modem Status Register Shadow
  EFlags        db      0               ;Extended flags
  ErrorMask     db      0               ;Default error-checking mask
  RxMask        db      0               ;Character mask
  ImmedChar     db      0               ;Char to be transmitted immediately
  HSFlag        db      0               ;Handshake flag
  HHSLines      db      0               ;8250 DTR/RTS bits for handshaking
  OutHHSLines   db      0               ;Lines that must be high to output
  MSRMask       db      0               ;Mask of Modem Lines to check
  MSRInfinite   db      0               ;Mask of MSR lines that must be high
  IntVecNum     db      0               ;Interrupt vector number
  LSRShadow	db	0		;Line Status Register shadow
  QOutMod	dd	0		;characters sent mod xOnLim ENQ/ETX 
  VCD_data	dd	0
  VCDflags	db	0
  IRQHandle	dd	0		; irq's handle
  ReferenceData	dd	0		; reference data of enablenotification
  MiscFlags	db	0		;still more flags
  IRQn		db	0		; Which irq
  MyIRQStruc	dd	0		; IRQ struc for me
  OwnerVM	dd	0		; Owner for this port
  MyName	dd	0		; name of port
  RxFifoTrigger	db	0		; what is the RxTrigger ?
  TxFifoTrigger	db	0		; How many bytes to blast
  ContentionHnd	dd	0		; Who handles contention?
  ContentionRes	dd	0		; resource to contend for
  RxTimeAddr	dd	0		; When was the last char received.
  NextPIStruc	dd	0		; next port info on this IRQ

IFDEF	DEBUG
  ArtificialErr	dd	0		; How many Due to Buffer full ETC.
  ParityErrors	dd	0		; parity error
  OverrunErrors	dd	0		; overrun errors
  FramingErrors	dd	0		; framing errors
  NumDataInts	dd	0		; How many data avail interrupts
ENDIF

PortInformation	ends

.errnz	(SIZE _DCB) and 3
.errnz	(SIZE _PortData) and 3
.errnz	(SIZE PortInformation) and 3


MAXCOM		EQU	3		; COM 0,1,2,3


; flag equates in EFlags
fUseDSR     equ 00000001b		; set, if DSR is significant
fNoFIFO     equ 00000010b		; set, if no FIFO on port
fFIFOchkd   equ 00000100b		; set, if FIFO has been checked
fFIFOpre    equ 00001000b		; FIFO enabled when port opened
fNoTxFifo   equ 00010000b		; Tx FIFO should not be used

fEFlagsMask equ fUseDSR OR fFIFOpre OR fFIFOchkd OR fNoFIFO  OR fNoTxFifo
					; flags which shouldn't be cleared

fDontUse1   equ	00100000b		; Read next comments
fDontUse2   equ	01000000b		; read next comments...

.errnz fEFlagsMask AND (fEOF OR fTxImmed) ; can't overlap with either of the
					  ; bits that are folded into 
					  ; COMS32_BitMask1

fFifoForcedOff  equ	10000000b

;
; Values for NotifyFlags
;
CN_RecvSent	equ CN_RECEIVE SHL 8
CN_TransSent	equ CN_TRANSMIT SHL 8

CN_Idle 	equ 10000000b
CN_Notify	equ 01000000b
CN_CharReceived	equ 00100000b

; CN_EventSent	equ 00000100b
; CN_TransSent	equ 00000010b
; CN_RecvSent	equ 00000001b

NotifyFlagsLO	equ byte ptr NotifyFlags
NotifyFlagsHI	equ byte ptr NotifyFlags+1

;
; Values for the handshake flag
;
; BreakSet        - True if break was set - stops transmission
; XOffSent        - True if we have sent the XOff character
; XOffPending     - True if XOff character needs to be sent
; XOnPending      - True if XOn  character needs to be sent
; HHSDown         - True if host dropped required hardware lines
; HHSDropped      - True if we dropped our hardware handshake lines
; XOffReceived    - True if XOff received from host
; HSPending       - Mask to return non-zero if XOn or Xoff must be sent
; HSReceived      - Mask to return non-zero if handshake has been
;                   received from host stopping transmission
; CannotXmit      - Mask to return non-zero if any condition
;                   exists which prevents us from tranmitting.
; HSSent          - Mask to return non-zero if we sent a handshake


XOffPending     equ     00000001b       ;XOff needs to be sent
EnqPending      equ     00000001b       ;Enq needs to be sent [rkh]
EtxPending      equ     00000001b       ;Etx needs to be sent

HHSDropped      equ     00000010b       ;Our hardware handshake lines are down

XOnPending      equ     00000100b       ;XOn needs to be sent
AckPending      equ     00000100b       ;Ack needs to be sent (ENQ/ACK & ETX/ACK)

XOffReceived    equ     00001000b       ;XOff character received
EnqSent         equ     00001000b       ;Enq has been sent
EtxSent         equ     00001000b       ;Etx has been sent

XOffSent        equ     00010000b       ;XOff has been sent
EnqReceived     equ     00010000b       ;Enq character received (ENQ/ACK)
EtxReceived     equ     00010000b       ;Etx character received (ETX/ACK)

HHSDown         equ     00100000b       ;Host hardware handshake lines are down

BreakSet        equ     01000000b       ;Break has been set

HHSAlwaysDown	equ	10000000b	;set if host handshake lines were never
					;  detected high

HSPending       equ     XOffPending+XOnPending
HSReceived      equ     XOffReceived+HHSDown
HSSent          equ     XOffSent+HHSDropped
CannotXmit	equ	HSPending+HSReceived+BreakSet

; values for MiscFlags

Discard 	equ	00000001b	;Discard recevied data
Unmasked_IRQ	equ	00000010b	;have we unmasked IRQ
TxQInternal	equ	00000100b	;have we allocated internal Txbuffer ?
RxQInternal	equ	00001000b	;have we allocated internal RxBuffer ?
TxQSet		equ	00010000b	;have we set a transmit queue ?
ClrTimer	equ	00100000b	;don't use timer logic
IgnoreCommError	equ	01000000b	;ignore pending comm error (PortRead).
SetComStateDone	equ	10000000b	;SetCommState has been already called.

DiscardBit	equ	00000000b
Unmasked_IRQBit	equ	00000001b
TxQInternalBit	equ	00000010b
RxQInternalBit	equ	00000011b
TxQSetBit	equ	00000100b
ClrTimerBit	equ	00000101b
IgnoreCommErrorBit equ	00000110b
SetComStateDoneBit equ	00000111b

DELAY_TIME     equ   200     ;Delay at least 200 milliseconds

IRQStruc	STRUC

  IRQNumber	db	0		; Which IRQ does this struct belong to?
  VirtCnt 	db	0		; how many times virtualized
  OldMask 	db	0		; what was old mask ?
  FillerIS	db	0
  MyIRQHandle	dd	0		; IRQ handle for the irq
  PIStruc	dd	0

IRQStruc	ENDS
