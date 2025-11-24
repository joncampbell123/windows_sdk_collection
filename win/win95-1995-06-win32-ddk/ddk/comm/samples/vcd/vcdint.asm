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
TITLE vcdint.asm -
;******************************************************************************
;
;   Title:	vcdint.asm
;
;==============================================================================
.386

.xlist
	INCLUDE VMM.Inc
	INCLUDE Debug.Inc

	INCLUDE VCD.INC

	INCLUDE COMDEV.INC
	INCLUDE IBMCOM.INC
.list

VxD_CODE_SEG
EXTRN	VCD_Assign:NEAR
VxD_CODE_ENDS

VxD_DATA_SEG

VCD_Assign_Addr     dd	OFFSET32 VCD_Assign ;allow for indirect call to VCD_Assign

;
; addresses of relocated routines
;
PUBLIC	VCD_Setup_Ptrs_Addr, VCD_Xmit_CallBack_Addr
VCD_Setup_Ptrs_Addr	    dd OFFSET32 VCD_Setup_Ptrs
VCD_Xmit_CallBack_Addr	    dd OFFSET32 VCD_Xmit_CallBack

SrvTab label dword
VCD_ModemStatus_Addr	dd OFFSET32 VCD_ModemStatus ;Modem Status Interrupt
VCD_XmitEmpty_Addr	dd OFFSET32 VCD_XmitEmpty   ;Tx Holding Reg. Interrupt
VCD_DataAvail_Addr	dd OFFSET32 VCD_DataAvail   ;Rx Data Available Interrupt
VCD_LineStat_Addr	dd OFFSET32 VCD_LineStat    ;Reciever Line Status Interrupt

ClearIntTab label byte
	db  UART_MSR			;clear Modem Status Interrupt
	db  UART_IIR			;clear Tx Holding Reg. Interrupt
	db  UART_RBR			;clear Rx Data Available Interrupt
	db  UART_LSR			;clear Reciever Line Status Interrupt

VxD_DATA_ENDS


VxD_IDATA_SEG

EXTRN COM_30_Support:BYTE

;
; addresses of relocated routines
;
PUBLIC	VCD_Sharable_COMM_Int_Addr, VCD_COMM_Int_Addr
VCD_Sharable_COMM_Int_Addr  dd OFFSET32 VCD_Sharable_COMM_Int
VCD_COMM_Int_Addr	    dd OFFSET32 VCD_COMM_Int

VxD_IDATA_ENDS


VxD_ICODE_SEG

;******************************************************************************
;
;   VCD_Relocate_COMM_Int
;
;   DESCRIPTION:    Check INI flag and relocate the ring0 copy of COMM.DRV's
;		    int handler into heap memory if a 3.0 driver is being
;		    used.
;
;   ENTRY:	    none
;
;   EXIT:	    none
;
;   USES:	    anything
;
;==============================================================================
BeginProc VCD_Relocate_COMM_Int

IFNDEF Debugable30COMINT
	xor	eax, eax		    ; default=FALSE
	xor	esi, esi		    ; Win386 section
	mov	edi, OFFSET32 COM_30_Support
	VMMCall Get_Profile_Boolean
	or	eax, eax
	jz	no_30_support

	mov	eax, OFFSET32 VCD_End_Relocatable
	sub	eax, OFFSET32 VCD_Begin_Relocatable ; eax = # of bytes to relocate
	add	eax, 3
	and	al, NOT 3
	push	eax
	VMMCall _HeapAllocate, <eax, 0>
	pop	ecx
	or	eax, eax		    ;Q: got memory?
	jz	short alloc_failed	    ;	N:
	shr	ecx, 2			    ; # of dwords to copy
	mov	edi, eax
	mov	esi, OFFSET32 VCD_Begin_Relocatable
	cld
	rep	movsd

;
; Relocate proc pointers by adding offset from position in ICODE to position
; in the heap.	(EAX=heap location)
;
	sub	eax, OFFSET32 VCD_Begin_Relocatable ;eax = relocation offset
	add	[VCD_Setup_Ptrs_Addr], eax
	add	[VCD_Xmit_CallBack_Addr], eax
	add	[VCD_Sharable_COMM_Int_Addr], eax
	add	[VCD_COMM_Int_Addr], eax
	add	[VCD_ModemStatus_Addr], eax
	add	[VCD_XmitEmpty_Addr], eax
	add	[VCD_DataAvail_Addr], eax
	add	[VCD_LineStat_Addr], eax
	ret

alloc_failed:
	Debug_Out 'VCD warning:  failed to get #ECX bytes for relocating 3.0 CommInt'

no_30_support:
;
; Flag no support for 3.0 com drivers
;
	xor	eax, eax
	mov	[VCD_Setup_Ptrs_Addr], eax
	mov	[VCD_Sharable_COMM_Int_Addr], eax
	mov	[VCD_COMM_Int_Addr], eax
ENDIF
	ret

EndProc VCD_Relocate_COMM_Int

VxD_ICODE_ENDS

IFDEF Debugable30COMINT
VxD_CODE_SEG
ELSE
VxD_ICODE_SEG
ENDIF

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
; Start of relocatable portion of code
;
Dword_Align
VCD_Begin_Relocatable label byte

;******************************************************************************
;
;   VCD_Setup_Ptrs
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> COM data structure
;		    EDI -> VCD_CB_Struc
;		    Client_DS:SI -> Windows COMDEB structure for the port
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Setup_Ptrs

	Client_Ptr_Flat edi, ds, si		; edi = linear address of COMDEB
	mov	[esi.VCD_COMDEB], edi

	movzx	eax, word ptr [edi.QInAddr+2]
	VMMcall _SelectorMapFlat, <ebx, eax, 0>
	movzx	ecx, word ptr [edi.QInAddr]
	add	eax, ecx			; eax = linear adr of QInAddr
	mov	[esi.VCD_QIn], eax

	movzx	eax, word ptr [edi.QOutAddr+2]
	VMMcall _SelectorMapFlat, <ebx, eax, 0>
	movzx	ecx, word ptr [edi.QOutAddr]
	add	eax, ecx			; eax = linear adr of QOutAddr
	mov	[esi.VCD_QOut], eax

	mov	[edi.QInSegment], esi		; put our COM struc ptr in COMDEB
	ret

EndProc VCD_Setup_Ptrs


;******************************************************************************
;
;   VCD_Xmit_CallBack
;
;   DESCRIPTION:
;
;   ENTRY:	    Client_DS:SI -> COMDEB
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Xmit_CallBack

	Client_Ptr_Flat edi, ds, si	; edi = linear address of COMDEB
	mov	esi, [edi.QInSegment]	; esi -> COM data struc
	VMMcall Get_Cur_VM_Handle
	cmp	[esi.VCD_Owner], ebx	; Q: current owner?
	je	SHORT VCD_xmit_OK	;    Y:

	call	[VCD_Assign_Addr]	; call indirect to VCD_Assign
	or	ecx, ecx		;Q: port assigned?
	jz	short VCD_xmit_OK	;   Y: send
	or	byte ptr EvtWord[edi],EV_TxEmpty
	jmp	short xmit_cb_exit

VCD_xmit_OK:
	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl,UART_LSR		;--> Line Status Register
	in	al,dx			;Is xmit really empty?
	test	al,LSR_THRE
	jz	SHORT xmit_cb_exit	;Transmitter not empty, cannot send
	pushfd
	cli
	call	VCD_XmitEmpty
	mov	[edi.ForcedInt], 0	; clear flag set by COMM.DRV
	popfd
xmit_cb_exit:
	VMMCall Simulate_Far_Ret
	ret

EndProc VCD_Xmit_CallBack


;******************************************************************************
;
;   VCD_Enable_THRE
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> COM data structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_Enable_THRE
	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl, UART_LSR
	in	al, dx
	and	al, LSR_THRE		; Check transmitter holding reg status
	jz	short int_dont_kick_tx

	sub	dl, UART_LSR - UART_IER
	in	al, dx
	test	al, IER_THRE
	jnz	short already_enabled

	or	al, IER_THRE
	out	dx, al			; 8250, 8250-B bug requires
	IO_Delay			; writing register twice
	out	dx, al

already_enabled:
	jmp	VCD_XmitEmpty

int_dont_kick_tx:
	ret

EndProc VCD_Enable_THRE


;******************************************************************************
;
;   VCD_COMM_Int
;
;   DESCRIPTION:    This is the communications interrupt service routine for
;		    RS232 communications.  When an RS232 event occurs the
;		    interrupt vectors here.  This routine determines who the
;		    caller was and services the appropriate interrupt.	The
;		    interrupts are prioritized in the following order:
;
;			1.  line status interrupt
;			2.  read data available interrupt
;			3.  transmit buffer empty interrupt
;			4.  modem service interrupt
;
;		    This routine continues to service until all interrupts
;		    have been satisfied.
;
;   ENTRY:	    ESI -> COM data structure
;
;   EXIT:
;
;   USES:	    Anything
;
;==============================================================================
IFDEF SHARE_IRQ
BeginProc VCD_Sharable_COMM_Int, High_Freq
	mov	al, [esi.VCD_Virt_IIR]	; use saved interrupt identity first
	jmp	SHORT Int_start
EndProc VCD_Sharable_COMM_Int
ENDIF

BeginProc VCD_COMM_Int, High_Freq

IntLoop10:
	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl, UART_IIR
	in	al, dx			; get interrupt identity
Int_start:
	Queue_Out "IIR=#AL"

	mov	edi, [esi.VCD_COMDEB]
	test	al, 1			;Interrupt need servicing?
	jnz	short IntLoop20 	;No, all done

	movzx	eax, al
	shl	eax, 1
	call	SrvTab[eax]		;Service the Interrupt
	jmp	IntLoop10

IntLoop20:
	mov	ax, EvtMask[edi]	;Mask the event word to only the
	and	EvtWord[edi], ax	;  user specified bits
	ret

EndProc VCD_COMM_Int


;******************************************************************************
;
;   VCD_LineStat
;
;   DESCRIPTION:
; This routine used to fall into DataAvail for the bulk of its processing.
; This is no longer the case...  A very popular internal modem seems to
; operate differently than a real 8250 when parity errors occur.  Falling
; into the DataAvail handler on a parity error caused the same character
; to be received twice.  Having this routine save the LSR status, and
; return to InterruptLoop fixes the problem, and still works on real COMM
; ports.  The extra overhead isn't a big deal since this routine is only
; entered when there is an exception like a parity error.
;
;
;   ENTRY:	    ESI -> COM data structure
;		    EDI -> COMDEB structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_LineStat
	or	byte ptr EvtWord[edi],EV_Err	;Show line status error

	mov	edx, [esi.VCD_IObase]		; Get start of ports
	add	dl,UART_LSR			;--> line status register
	in	al, dx

	test	al, LSR_PE+LSR_FE+LSR_OE	;Parity, Framing, Overrun error?
	jz	short @f

	mov	LSRShadow[edi], al		;yes, save status for DataAvail
@@:
	test	al, LSR_BI			;Break detect?
	jz	short linestat_ret		;Not break detect interrupt

	or	byte ptr EvtWord[edi],EV_Break	;Show break
linestat_ret:
	ret

EndProc VCD_LineStat


;******************************************************************************
;
;   VCD_DataAvail
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> COM data structure
;		    EDI -> COMDEB structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_DataAvail

	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl, UART_RBR
	in	al,dx			;Read received character

	mov	ah,LSRShadow[edi]	;what did the last Line Status intrpt
	mov	bh,ah			;  have to say?
	or	ah,ah
	jz	short @f

	and	ah,ErrorMask[edi]	;there was an error, record it
	or	byte ptr ComErr[edi],ah
	mov	LSRShadow[edi],0
@@:
	.errnz LSR_OE-CE_OVERRUN	;Must be the same bits
	.errnz LSR_PE-CE_RXPARITY
	.errnz LSR_FE-CE_FRAME
	.errnz LSR_BI-CE_BREAK		;Little extra error code they never told
					;  you about

; Regardless of the character received, flag the event in case
; the user wants to see it.

	or	byte ptr EvtWord[edi],EV_RxChar   ;Show a character received
	.errnz HIGH EV_RxChar

; Check the input queue, and see if there is room for another
; character.  If not, or if the end of file character has already
; been received, then go declare overflow.

	movzx	ecx,QInCount[edi]	;Get queue count (used later too)
	cmp	cx,QInSize[edi] 	;Is queue full?
	jge	short DataAvailOverrun	;  Yes, comm overrun
	test	EFlags[edi],fEOF	;Has end of file been received?
	jnz	short DataAvailOverrun	;  Yes - treat as overflow

; Test to see if there was a parity error, and replace
; the character with the parity character if so

	test	bh,LSR_PE		;Parity error
	jnz	short DataAvailParity	;  Yes

; See if we need to strip null characters, and skip
; queueing if this is one.  Also remove any parity bits.

DataAvail25:
	and	al,RxMask[edi]		;Remove any parity bits
	jz	short DataAvailNull	; Is NULL char

; Check to see if we need to check for EOF characters, and if so
; see if this character is it.

DataAvail30:
	test	Flags[edi],fBinary	;Is this binary stuff?
	jnz	short DataAvail60	;  Yes, skip EOF check
	cmp	al,EOFChar[edi] 	;Is this the EOF character?
	je	short DataAvailEOF	;  Yes

; If output XOn/XOff is enabled, see if the character just received
; is either an XOn or XOff character.  If it is, then set or
; clear the XOffReceived flag as appropriate.

DataAvail60:
	test	Flags2[edi],fOutX	;Output handshaking?
	jz	short DataAvail80	;  No
	cmp	al,XOFFChar[edi]	;Is this an X-Off character?
	je	DataAvailXoff		;  Yes

; see about XOn or Ack

	cmp	al,XONChar[edi] 	;Is this an XOn character?
	je	DataAvailXon		;  Yes

; Now see if this is a character for which we need to set an event as
; having occured. If it is, then set the appropriate event flag

DataAvail80:
	cmp	al,EVTChar[edi] 	;Is it the event generating character?
	je	short DataAvailEventChr ; Yes

; Finally, a vaild character that we want to keep, and we have
; room in the queue. Place the character in the queue.
; If the discard flag is set, then discard the character

DataAvail90:
	test	HSFlag[edi],Discard	;Discarding characters ?
	jnz	short DataAvail50	;  Yes

	push	esi
	mov	esi, [esi.VCD_QIn]	;Get queue base pointer
	movzx	ebx,QInPut[edi] 	;Get index into queue
	mov	[ebx][esi], al		;Store the character
	pop	esi
	inc	ebx			;Update queue index
	cmp	bx,QInSize[edi] 	;See if time for wrap-around
	jae	short DataAvailWrap
DataAvail100:
	mov	QInPut[edi],bx		;Store updated pointer
	inc	ecx			;And update queue population
	mov	QInCount[edi],cx

; If flow control has been enabled, see if we are within the
; limit that requires us to halt the host's transmissions

	cmp	cx,XOffPoint[edi]	;Time to see about XOff?
	jae	short DataAvailTestXoff

DataAvail50:
	ret

DataAvailParity:
	test	Flags2[edi],fPErrChar	;Parity error replacement character?
	jz	DataAvail25		;  No
	mov	al,PEChar[edi]		;  Yes, get parity replacement char

; Skip all other processing except event checking and the queing
; of the parity error replacement character

	jmp	short DataAvail80	;Skip all but event check, queing

DataAvailNull:
	test	Flags2[edi],fNullStrip	;Are we stripping received nulls?
	jnz	DataAvail50		;  Yes, put char in the bit bucket
	jmp	DataAvail30

DataAvailEOF:
	or	EFlags[edi],fEOF	;Set end of file flag
	jmp	DataAvail50

DataAvailOverrun:
	or	byte ptr ComErr[edi],CE_RXOVER ;Show queue overrun
	jmp	DataAvail50		;Skip the queing process

DataAvailWrap:
	xor	ebx,ebx 		;Wrap-around is a new zero pointer
	jmp	short DataAvail100

DataAvailTestXoff:
	mov	cl,HSFlag[edi]		;Get handshake flag
	test	cl,HSSent+XOffPending	;Handshake already sent?
	jnz	DataAvail50		;  Yes, don't send it again
	mov	ah,HHSLines[edi]	;Should hardware lines be dropped?
	or	ah,ah			;  (i.e. do we have HW HS enabled?)
	jz	short DataAvail110	;  No
	add	dl,UART_MCR		;  Yes
	in	al,dx			;Clear the nessecary bits
	not	ah
	and	al,ah
	or	cl,HHSDropped		;Show lines have been dropped
	mov	HSFlag[edi],cl		;Save updated handshake flags [rkhx] 3.00.01/002
	out	dx,al			;  and drop the lines
DataAvail110:
	test	Flags2[edi],fInX	;Input Xon/XOff handshaking
	jz	DataAvail50		;  No
	or	cl,XOffPending		;Show XOFF needed
	mov	HSFlag[edi],cl		;Save updated handshake flags [rkhx] 3.00.01/002
	call	VCD_Enable_THRE 	;Get pending char's from buffer
	jmp	DataAvail50

DataAvailEventChr:
	or	byte ptr EvtWord[edi],EV_RxFlag   ;Show received specific character
	jmp	DataAvail90

DataAvailXoff:
	or	HSFlag[edi],XOffReceived ;Show XOff received, ENQ or ETX [rkh]
	test	Flags[edi],fEnqAck+fEtxAck ;Enq or Etx Ack?
	jz	DataAvail50		;  No
	movzx	ecx,QInCount[edi]	;Get current count of input chars
	cmp	cx,XONLim[edi]		;See if at XOn limit
	ja	DataAvail50		;  No
	and	HSFlag[edi],NOT XOffReceived ;Show ENQ or ETX not received
	or	HSFlag[edi], XOnPending
	jmp	short DataAvail75	;Done

DataAvailXon:
	and	HSFlag[edi],NOT XOffReceived
	test	Flags[edi],fEnqAck+fEtxAck ;Enq or Etx Ack?
	jz	short DataAvail75	;  No
	and	HSFlag[edi],NOT EnqSent
DataAvail75:
	call	VCD_Enable_THRE
	jmp	DataAvail50		;Done
EndProc VCD_DataAvail


;******************************************************************************
;
;   VCD_XmitEmpty
;
;   DESCRIPTION:
;
;   ENTRY:	    ESI -> COM data structure
;		    EDI -> COMDEB structure
;
;   EXIT:
;
;   USES:
;
;==============================================================================
BeginProc VCD_XmitEmpty
	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl,UART_LSR		;--> Line Status Register
	in	al,dx			;Is xmit really empty?
	test	al,LSR_THRE
	jz	XmitEmpty90		;Transmitter not empty, cannot send
XmitEmpty5:
	sub	dl,UART_LSR-UART_THR	;--> Transmitter Holding Register

; If the hardware handshake lines are down, then XOff/XOn cannot
; be sent.  If they are up and XOff/XOn has been received, still
; allow us to transmit an XOff/XOn character.  It will make
; a dead lock situation less possible (even though there are
; some which could happen that cannot be handled).

	mov	ah,HSFlag[edi]		;Get handshaking flag
	test	ah,HHSDown+BreakSet	;Hardware lines down or break set?
	jnz	XmitEmpty100		;  Yes, cannot transmit

; Give priority to any handshake character waiting to be
; sent.  If there are none, then check to see if there is
; an "immediate" character to be sent.  If not, try the queue.

XmitEmpty10:
	test	Flags[edi],fEnqAck+fEtxAck ;Enq or Etx Ack?
	jnz	short XmitEmpty40	;  Yes
XmitEmpty15:
	test	ah,HSPending		;XOff or XOn pending
	jnz	XmitEmptyXOnOff 	;  Yes

; If any of the lines which were specified for a timeout are low, then
; don't send any characters.  Note that by putting the check here,
; XOff and Xon can still be sent even though the lines might be low.

; Also test to see if a software handshake was received.  If so,
; then transmission cannot continue.  By delaying the software check
; to here, XOn/XOff can still be issued even though the host told
; us to stop transmission.

XmitEmpty40:
	test	ah,CannotXmit		;Anything preventing transmission?
	jnz	XmitEmpty100		;  Yes, disarm and exit

; If a character has been placed in the single character "transmit
; immediately" buffer, clear that flag and pick up that character
; without affecting the transmitt queue.

XmitEmpty45:
	test	EFlags[edi],fTxImmed	;Character to xmit immediately?
	jnz	XmitEmptyImmed		;  Yes

XmitEmpty515:
	movzx	ecx,QOutCount[edi]	;Output queue empty?
	jecxz	short XmitEmpty90J	;  Yes, go set an event
	test	Flags[edi],fEtxAck	;Etx Ack?
	jnz	short XmitEmptyEtxAck

XmitEmpty55:
	test	Flags[edi],fEnqAck	;Enq Ack?
	jnz	short XmitEmptyEnqAck	; Yes, send Ack
XmitEmpty59:
	push	esi
	mov	esi, [esi.VCD_QOut]	;Get queue base pointer

	movzx	ebx,QOutGet[edi]	;Get pointer into queue
	mov	al,[ebx][esi]		;Get the character
	pop	esi

	inc	ebx			;Update queue pointer
	cmp	bx,QOutSize[edi]	;See if time for wrap-around
	jae	short XmitEmptyWrap	;time for wrap
XmitEmpty60:
	mov	QOutGet[edi],bx 	;Save queue index
	movzx	ecx,QOutCount[edi]	;Output queue empty?
	dec	ecx			;Dec # of bytes in queue
	mov	QOutCount[edi],cx	;  and save new population

; Finally!  Transmit the character

XmitEmpty70:
	out	dx,al			;Send char
	ret

XmitEmpty90J:
	jmp	XmitEmpty90

XmitEmptyEtxAck:
	movzx	ecx,QOutMod[edi]	;Get number bytes sent since last ETX
	cmp	cx,XONLim[edi]		;At Etx limit yet?
	jne	short XmitEmpty51	;  No, inc counter
	mov	QOutMod[edi],0		;  Yes, zero counter
	or	HSFlag[edi],EtxSent	;Show ETX sent
	mov	al,XOFFChar[edi]	;Get ETX char
	jmp	short XmitEmpty70	;Go output the character

XmitEmpty51:
	inc	ecx			; Update counter
	mov	QOutMod[edi],cx 	; Save counter
	jmp	short XmitEmpty59	; Send queue character

XmitEmptyEnqAck:
	movzx	ecx,QOutMod[edi]	;Get number bytes sent since last ENQ
	or	ecx, ecx		;At the front again?
	jnz	short XmitEmpty56	;  No, inc counter
	mov	QOutMod[edi],1		;  Yes, send ENQ
	or	HSFlag[edi],EnqSent	;Show ENQ sent
	mov	al,XOFFChar[edi]	;Get ENQ char
	jmp	short XmitEmpty70	;Go output the character

XmitEmpty56:
	inc	ecx			;Update counter
	cmp	cx,XONLim[edi]		;At end of our out buffer len?
	jne	short XmitEmpty58	;  No
	xor	ecx, ecx		;Show at front again.

XmitEmpty58:
	mov	QOutMod[edi],cx 	;Save counter
	jmp	XmitEmpty59

XmitEmptyWrap:
	xor	ebx,ebx 		;Wrap by zeroing the index
	jmp	short XmitEmpty60

XmitEmptyImmed:
	and	EFlags[edi],NOT fTxImmed ;Clear xmit immediate flag
	mov	al,ImmedChar[edi]	;Get char to xmit
	jmp	XmitEmpty70		;Transmit the character

XmitEmptyXOnOff:
	test	ah,XOffPending		;Is it XOff that needs to be sent?
	jz	short XmitEmpty20	;  No, it must be XOn
	and	ah,NOT XOffPending	;Clear XOff pending
	or	ah,XOffSent		;Show XOff sent
	mov	al,XOFFChar[edi]	;Get XOff character
	jmp	short XmitEmpty30	;Send the character

XmitEmpty20:
	and	ah,NOT XOnPending+XOffSent
	mov	al,XONChar[edi] 	;Get XOn character
XmitEmpty30:
	mov	HSFlag[edi],ah		;Save updated handshake flag
	jmp	XmitEmpty70		;Go output the character

; Nothing immediate, see if there is a character in the
; transmit queue, and remove one if there is.

XmitEmpty50:
	test	ah,AckPending		;Ack need to be sent?
	jz	XmitEmpty515		;  No
	and	ah,NOT AckPending	;  turn offf Ack need to be sent
	mov	al,XONChar[edi] 	;Get Ack character
	mov	HSFlag[edi],ah		;Save updated handshake flag
	jmp	XmitEmpty70		;Go output the character

; No more characters to transmit.  Flag this as an event.

XmitEmpty90:
	or	byte ptr EvtWord[edi],EV_TxEmpty

; Cannot continue transmitting (for any of a number of reasons).
; Disable the transmit interrupt.  When it's time resume, the
; transmit interrupt will be reenabled, which will generate an
; interrupt.

XmitEmpty100:
	inc	dl			;--> Interrupt Enable Register
	.errnz	 UART_IER-UART_THR-1
	in	al,dx			;I don't know why it has to be read
	and	al,NOT IER_THRE 	;  first, but it works this way
	jmp	XmitEmpty70		;Set new interrupt value
EndProc VCD_XmitEmpty


;******************************************************************************
;
;   VCD_ModemStatus
;
;   DESCRIPTION:    Modem Status Interrupt Handler
;
;   ENTRY:	    ESI -> COM data structure
;		    EDI -> COMDEB structure
;
;   EXIT:
;
;   USES
;
;==============================================================================
BeginProc VCD_ModemStatus

; Get the modem status value and shadow it for MSRWait.

	mov	edx, [esi.VCD_IObase]	; Get start of ports
	add	dl,UART_MSR		 ;--> Modem Status Register
	in	al,dx
	mov	MSRShadow[edi],al	;Save MSR data for others
	mov	ch,al			;Save a local copy

; Create the event mask for the delta signals

	movzx	eax, al
	mov	ah,al			;Just a lot of shifting
	shr	eax, 2
	shr	ah, 1
	shr	eax, 3
	and	ax,EV_CTS+EV_DSR+EV_RLSD+EV_Ring
	or	EvtWord[edi],ax

	mov	ah,ch				;[rkh]...
	shr	ah, 2
	and	ax,EV_CTSS+EV_DSRS
	or	EvtWord[edi],ax

	mov	ah,ch
	shr	ah, 3
	and	ax,EV_RLSD
	or	EvtWord[edi],ax

	mov	ah,ch
	shl	ah, 3
	and	ax,EV_RingTe
	or	EvtWord[edi],ax

	.errnz	EV_CTS	 -0000000000001000b
	.errnz	EV_DSR	 -0000000000010000b
	.errnz	EV_RLSD  -0000000000100000b
	.errnz	EV_Ring  -0000000100000000b

	.errnz	EV_CTSS  -0000010000000000b	;[rkh]
	.errnz	EV_DSRS  -0000100000000000b
	.errnz	EV_RLSDS -0001000000000000b
	.errnz	EV_RingTe-0010000000000000b

	.errnz	MSR_DCTS -00000001b
	.errnz	MSR_DDSR -00000010b
	.errnz	MSR_DRLSD-00001000b
	.errnz	MSR_RING -01000000b

	.errnz	MSR_TERI -00000100b		;[rkh]
	.errnz	MSR_CTS  -00010000b
	.errnz	MSR_DSR  -00100000b
	.errnz	MSR_RLSD -10000000b

ModemStatus10:
	mov	al,OutHHSLines[edi]	;Get output hardware handshake lines
	or	al,al			;Any lines that must be set?
	jnz	short ModemStatus40	;Yes hardware handshake on output
ModemStatus30:
	ret

ModemStatus40:
	and	ch,al			;Mask bits of interest
	cmp	ch,al			;Lines set for Xmit?
	je	short ModemStatus20	;  Yes
	or	HSFlag[edi],HHSDown	;Show hardware lines have dropped
	jmp	short ModemStatus30

; Lines are set for xmit.  Kick an xmit interrupt if needed

ModemStatus20:
	and	HSFlag[edi],NOT HHSDown ;Show hardware lines back up
	call	VCD_Enable_THRE 	;Get pending char's from buffer
	jmp	short ModemStatus30

EndProc VCD_ModemStatus

VCD_End_Relocatable label byte

IFDEF Debugable30COMINT
VxD_CODE_ENDS
ELSE
VxD_ICODE_ENDS
ENDIF


VxD_CODE_SEG

;******************************************************************************
;
;   VCD_Eat_Interrupt
;
;   DESCRIPTION:    Clear the last interrupt, by repetitively reading IIR until
;		    it is 1, and reading the other necessary ports to clear the
;		    IIR specified interrupt.  i.e.
;
;			read MSR to clear modem status interrupt
;			read IIR to clear Tx holding reg. interrupt
;			read RBR to clear Rx data available interrupt
;			read LSR to clear reciever line status interrupt
;
;   ENTRY:	    EBX = Current COM owner
;		    ESI = Com data structure
;
;   EXIT:	    int request from COM controller has been cleared
;
;   USES:	    Flags
;
;==============================================================================
BeginProc VCD_Eat_Interrupt

	push	eax
chk_more_ints:
	mov	edx, [esi.VCD_IObase]		; DX = Base of IO regs for port
	add	dl, UART_IIR
	in	al, dx				; get interrupt identity
	Queue_Out "eat IIR=#AL"
	test	al, 1				;Interrupt need servicing?
	jnz	short int_eaten 		;No, all done

	movzx	eax, al
	shr	eax, 1
	sub	dl, UART_IIR			; dx = base
	add	dl, ClearIntTab[eax]		;Calc port to read to clear int
	in	al, dx
	jmp	chk_more_ints

int_eaten:
	pop	eax
	ret

EndProc VCD_Eat_Interrupt

VxD_CODE_ENDS

END
