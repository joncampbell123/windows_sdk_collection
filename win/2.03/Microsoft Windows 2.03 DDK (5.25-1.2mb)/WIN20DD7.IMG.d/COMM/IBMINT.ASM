	page	,132
;---------------------------Module-Header-------------------------------;
; Module Name: IBMINT.ASM
;
; Created: Fri 06-Feb-1987 10:45:12
; Author:  **** ***** [*****]
;
; Copyright (c) 1985-1987  Microsoft Corporation
;
; General Description:
;	This file contains the interrupt time routines for the
;	IBM Windows communications driver.
;
;	The interrupt code is preloaded and fixed.
; History:
;
;-----------------------------------------------------------------------;

        subttl  Communications Hardware Interrupt Service Routines

	.xlist
	include cmacros.inc
	include comdev.inc
	include ibmcom.inc
	include ins8250.inc
	.list


createSeg	_INTERRUPT,IntCode,word,public,CODE
sBegin	IntCode
assumes cs,IntCode


DataSeg label   word
        dw      0                       ;Data Segment.  Required for interrupts
deb_2F8 label  word
	dw	0
deb_3F8 label  word
	dw	0
page

;--------------------------Interrupt Hanlder----------------------------;
; COM_2F8 - Interrupt handler for com port at 2F8
;
; Setup is performed for the main interrupt handler.  This involves
; setting the index of which
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	ALL
; Registers Destroyed:
;	None
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

COM_2F8 proc	far			;Interrupt server enters here

        push    si
	mov	si,[deb_2F8]		;Get address of correct deb to use
        jmp     short CommInt

COM_2F8 endp
page

;--------------------------Interrupt Hanlder----------------------------;
; COM_3F8 - Interrupt handler for com port at 3F8
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	ALL
; Registers Destroyed:
;	None
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

COM_3F8 proc	far			;Interrupt server enters here

        push    si
	mov	si,[deb_3F8]		;Get address of correct deb to use
;	jmp	short CommInt
	errn$	CommInt

COM_3F8 endp
page

;--------------------------Interrupt Hanlder----------------------------;
; CommInt - Interrupt handler for com1 and com2
;
; Interrupt handlers for COM1: and COM2:.  This is the communications
; interrupt service routine for RS232 communications.  When an RS232
; event occurs the interrupt vectors here.  This routine determines
; who the caller was and services the appropriate interrupt.  The
; interrupts are prioritized in the following order:
;
;	  1.  line status interrupt
;	  2.  read data available interrupt
;	  3.  transmit buffer empty interrupt
;	  4.  modem service interrupt
;
; This routine continues to service until all interrupts have been
; satisfied.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	ALL
; Registers Destroyed:
;	None
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,nothing
	assumes es,nothing

CommInt proc	far

        push    ds
        mov     ds,[DataSeg]            ;Get data seg
;	assumes ds,Data

        push    ax
        push    bx
        push    cx
        push    dx
        push    di
        push    es

        mov     dx,Port[si]             ;Get comm I/O port
        add     dl,ACE_IIDR             ;--> Interrupt ID Register
        jmp     short IntLoop10

;	Dispatch table for interrupt types

SrvTab  label   word
        dw      ModemStatus             ;Modem Status Interrupt
        dw      XmitEmpty               ;Tx Holding Reg. Interrupt
        dw      DataAvail               ;Rx Data Available Interrupt
        dw      LineStat                ;Reciever Line Status Interrupt


InterruptLoop:
        pop     dx                      ;Get ID reg I/O address

IntLoop10:
        in      al,dx                   ;Get Interrupt Id
        test    al,1                    ;Interrupt need servicing?
        jnz     IntLoop20               ;No, all done
        xor     ah,ah
        mov     di,ax
        push    dx                      ;Save Id register
	jmp	SrvTab[di]		;Service the Interrupt

IntLoop20:
        mov     ax,EvtMask[si]          ;Mask the event word to only the
        and     EvtWord[si],ax          ;  user specified bits
        pop     es
	assumes es,nothing

        pop     di
        pop     dx
        pop     cx
        pop     bx
        mov     al,EOI                  ;Send End-of-Interrupt
	out	INTA0,al
        sti
        pop     ax
        pop     ds
	assumes ds,nothing

        pop     si
        iret

CommInt endp
page

;----------------------------Private-Routine----------------------------;
;
; LineStat - Line Status Interrupt Handler
;
; Break detection is handled and set in the event word if
; enabled.  Other errors (overrun, parity, framing) are
; ORed into the ComErr flag.
;
; Since the Data Available interrupt handler needs to read
; the Line Status Register, share the same code between the
; two.	This resolves conflicts where reading the LSR clears
; error bits that the Data Available interrupt handler needs
; (i.e. parity error).
;
; This routine is jumped to, and will perform a jump back into
; the dispatch loop.
;
; Entry:
;	DS:SI --> DEB
;	DX     =  Port.IIDR
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AX,BX,CX,DI,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

		public	LineStat	;Public for debugging
LineStat	proc	near

        or      by EvtWord[si],EV_Err   ;Show line status error
;	jmp	DataAvail		;Same handler
	errn$	DataAvail

LineStat	endp
page

;----------------------------Private-Routine----------------------------;
;
; DataAvail - Data Available Interrupt Handler
;
; The available character is read and stored in the input queue.
; If the queue has reached the point that a handshake is needed,
; one is issued (if enabled).  EOF detection, Line Status errors,
; and lots of other stuff is checked.
;
; This routine is jumped to, and will perform a jump back into
; the dispatch loop.
;
; Entry:
;	DS:SI --> DEB
;	DX     =  Port.IIDR
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AX,BX,CX,DI,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

		public	DataAvail	;public for debugging
DataAvail	proc	near

        add     dl,ACE_LSR-ACE_IIDR     ;--> line status register
        in      al,dx                   ;Get line status
        mov     ah,al
        mov     bh,al                   ;Will need even another copy
        sub     dl,ACE_LSR-ACE_RBR      ;Get any data
        in      al,dx
        test    ah,ACE_BI               ;Break detect?
        jz      DataAvail10             ;Not break detect interrupt
        or      by EvtWord[si],EV_Break ;Show break

DataAvail10:
        and     ah,ErrorMask[si]        ;Combine error flags with prev. errors
        or      by ComErr[si],ah

        errnz   ACE_OR-CE_OVERRUN       ;Must be the same bits
        errnz   ACE_PE-CE_RXPARITY
        errnz   ACE_FE-CE_FRAME
        errnz   ACE_BI-CE_BREAK         ;Little extra error code they never told
                                        ;  you about


DataAvail20:
        shr     bh,1                    ;Data available?
        jnc     DataAvail50             ;No data available, exit
        errnz   ACE_DR-00000001b


;	Regardless of the character received, flag the event incase
;	the user wants to see it.

	or	by EvtWord[si],EV_RxChar;Show a character received
	errnz	<HIGH EV_RxChar>


;       Check the input queue, and see if there is room for another
;       character.  If not, or if the end of file character has already
;       been received, then go declare overflow.

        mov     cx,QInCount[si]         ;Get queue count (used later too)
        cmp     cx,QInSize[si]          ;Is queue full?
        jge     DataAvail40             ;  Yes, comm overrun
        test    EFlags[si],fEOF         ;Has end of file been received?
        jnz     DataAvail40             ;  Yes - treat as overflow



;       Test to see if there was a parity error, and replace
;       the character with the parity character if so


        test    bh,ACE_PE SHR 1         ;Parity error
        jz      DataAvail25             ;  No
        test    Flags2[si],fPErrChar    ;Parity error replacement character?
        jz      DataAvail25             ;  No
        mov     al,PEChar[si]           ;  Yes, get parity replacement char

;       Skip all other processing except event checking and the queing
;       of the parity error replacement character

        jmp     short DataAvail80       ;Skip all but event check, queing



;       See if we need to strip null characters, and skip
;       queueing if this is one.  Also remove any parity bits.

DataAvail25:
        and     al,RxMask[si]           ;Remove any parity bits
        jnz     DataAvail30             ;Not a Null character
        test    Flags2[si],fNullStrip   ;Are we stripping received nulls?
        jnz     DataAvail50             ;  Yes, put char in the bit bucket


;       Check to see if we need to check for EOF characters, and if so
;       see if this character is it.

DataAvail30:
        test    Flags[si],fBinary       ;Is this binary stuff?
        jnz     DataAvail60             ;  Yes, skip EOF check
        cmp     al,EOFChar[si]          ;Is this the EOF character?
        jnz     DataAvail60             ;  No, see about queing the charcter
        or      EFlags[si],fEOF         ;Set end of file flag
        jmp     short DataAvail50       ;Skip the queing process


DataAvail40:
	or	by ComErr[si],CE_RXOVER ;Show queue overrun

DataAvail50:
        jmp     InterruptLoop




;       If output XOn/XOff is enabled, see if the character just received
;       is either an XOn or XOff character.  If it is, then set or
;       clear the XOffReceived flag as appropriate.

DataAvail60:
        test    Flags2[si],fOutX        ;Output XOn/XOff?
        jz      DataAvail80             ;  No
	cmp	al,XOFFChar[si] 	;Is this an X-Off character?
        jnz     DataAvail70             ;  No, see about XOn
        or      HSFlag[si],XOffReceived ;Show XOff received
        jmp     DataAvail50             ;Done

DataAvail70:
	cmp	al,XONChar[si]		;Is this an XOn character?
        jnz     DataAvail80             ;  No, just a normal character
        and     HSFlag[si],NOT XOffReceived
	call	int_kick_tx		;Kick the tx interrupt back on
        jmp     DataAvail50             ;Done



;       Now see if this is a character for which we need to set an event as
;       having occurred. If it is, then set the appropriate event flag.

DataAvail80:
	cmp	al,EVTChar[si]		;Is it the event generating character?
        jne     DataAvail90             ;  No
        or      by EvtWord[si],EV_RxFlag;Show received specific character



;       Finally, a vaild character that we want to keep, and we have
;       room in the queue. Place the character in the queue.
;	If the discard flag is set, then discard the character

DataAvail90:
	test	HSFlag[si],Discard	;Discarding characters ?
	jnz	DataAvail50		;  Yes
        les     di,QInAddr[si]          ;Get queue base pointer
	assumes es,nothing

        mov     bx,QInPut[si]           ;Get index into queue
        mov     es:[bx][di],al          ;Store the character
        inc     bx                      ;Update queue index
        cmp     bx,QInSize[si]          ;See if time for wrap-around
        jc      DataAvail100            ;Not time to wrap
        xor     bx,bx                   ;Wrap-around is a new zero pointer

DataAvail100:
        mov     QInPut[si],bx           ;Store updated pointer
        inc     cx                      ;And update queue population
        mov     QInCount[si],cx



;       If flow control has been enabled, see if we are within the
;       limit that requires us to halt the host's transmissions

        cmp     cx,XOffPoint[si]        ;Time to see about XOff?
        jc      DataAvail120            ;  Not yet
        mov     cl,HSFlag[si]           ;Get handshake flag
        test    cl,HSSent+XOffPending   ;Handshake already sent?
        jnz     DataAvail120            ;  Yes, don't send it again

        mov     ah,HHSLines[si]         ;Should hardware lines be dropped?
        or      ah,ah                   ;
        jz      DataAvail110            ;  No
        add     dl,ACE_MCR              ;  Yes
        in      al,dx                   ;Clear the nessecary bits
        not     ah
        and     al,ah
        or      cl,HHSDropped           ;Show lines have been dropped
        out     dx,al                   ;  and drop the lines


DataAvail110:
        test    Flags2[si],fInX         ;Input Xon/XOff handshaking
        jz      DataAvail115            ;  No
        or      cl,XOffPending          ;Show XOFF needed
	call	int_kick_tx		;Kick transmitter if needed

DataAvail115:
        mov     HSFlag[si],cl           ;Save updated handshake flags

DataAvail120:
        jmp     InterruptLoop

DataAvail	endp
page

;----------------------------Private-Routine----------------------------;
;
; XmitEmpty - Transmitter Register Empty
;
; Entry:
;	DS:SI --> DEB
;	DX     =  Port.IIDR
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AX,BX,CX,DI,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

		public	XmitEmpty
XmitEmpty	proc	near

        add     dl,ACE_LSR-ACE_IIDR     ;--> Line Status Register
        in      al,dx                   ;Is xmit really empty?
        test    al,ACE_THRE
        jz      XmitEmpty90             ;Transmitter not empty, cannot send
        sub     dl,ACE_LSR-ACE_THR      ;--> Transmitter Holding Register


;       If the hardware handshake lines are down, then XOff/XOn cannot
;       be sent.  If they are up and XOff/XOn has been received, still
;       allow us to transmit an XOff/XOn character.  It will make
;       a dead lock situation less possible (even though there are
;       some which could happen that cannot be handled).

        mov     ah,HSFlag[si]           ;Get handshaking flag
        test    ah,HHSDown+BreakSet     ;Hardware lines down or break set?
        jnz     XmitEmpty100            ;  Yes, cannot transmit



;       Give priority to any handshake character waiting to be
;       sent.  If there are none, then check to see if there is
;       an "immediate" character to be sent.  If not, try the queue.

XmitEmpty10:
        test    ah,HSPending            ;XOff or XOn pending
        jz      XmitEmpty40             ;  No
        test    ah,XOffPending          ;Is it XOff that needs to be sent?
        jz      XmitEmpty20             ;  No, it must be XOn
        and     ah,NOT XOffPending      ;Clear XOff pending
        or      ah,XOffSent             ;Show XOff sent
	mov	al,XOFFChar[si] 	;Get XOff character
        jmp     short XmitEmpty30       ;Send the character

XmitEmpty20:
        and     ah,NOT XOnPending+XOffSent
	mov	al,XONChar[si]		;Get XOn character

XmitEmpty30:
        mov     HSFlag[si],ah           ;Save updated handshake flag
        jmp     short XmitEmpty70       ;Go output the character




;       If any of the lines which were specified for a timeout are low, then
;       don't send any characters.  Note that by putting the check here,
;       XOff and Xon can still be sent even though the lines might be low.

;       Also test to see if a software handshake was received.  If so,
;       then transmission cannot continue.  By delaying the software check
;       to here, XOn/XOff can still be issued even though the host told
;       us to stop transmission.

XmitEmpty40:
        test    ah,CannotXmit           ;Anything preventing transmission?
        jnz     XmitEmpty100            ;  Yes, disarm and exit


;       If a character has been placed in the single character "transmit
;       immediately" buffer, clear that flag and pick up that character
;       without affecting the transmitt queue.

        test    EFlags[si],fTxImmed     ;Character to xmit immediately?
        jz      XmitEmpty50             ;  No, try the queue
        and     EFlags[si],NOT fTxImmed ;Clear xmit immediate flag
        mov     al,ImmedChar[si]        ;Get char to xmit
        jmp     short XmitEmpty70       ;Transmit the character



;       Nothing immediate, see if there is a character in the
;       transmit queue, and remove one if there is.

XmitEmpty50:
        mov     cx,QOutCount[si]        ;Output queue empty?
        jcxz    XmitEmpty90             ;  Yes, go set an event
        les     di,QOutAddr[si]         ;Get address in ES:DI
	assumes es,nothing

        mov     bx,QOutGet[si]          ;Get pointer into queue
        mov     al,es:[bx][di]          ;Get the character
        inc     bx                      ;Update queue pointer
        cmp     bx,QOutSize[si]         ;See if time for wrap-around
        jc      XmitEmpty60             ;Not time for wrap
        xor     bx,bx                   ;Wrap by zeroing the index

XmitEmpty60:
        mov     QOutGet[si],bx          ;Save queue index
        dec     cx                      ;Dec # of bytes in queue
        mov     QOutCount[si],cx        ;  and save new population



;       Finally!  Transmit the character

XmitEmpty70:
        out     dx,al                   ;Send char
        jmp     InterruptLoop



;       No more characters to transmit.  Flag this as an event.

XmitEmpty90:
        or      by EvtWord[si],EV_TxEmpty


;       Cannot continue transmitting (for any of a number of reasons).
;       Disable the transmit interrupt.  When it's time resume, the
;       transmit interrupt will be reenabled, which will generate an
;       interrupt.

XmitEmpty100:
        inc     dx                      ;--> Interrupt Enable Register
        errnz   ACE_IER-ACE_THR-1
        in      al,dx                   ;I don't know why it has to be read
        and     al,NOT ACE_ETBEI        ;  first, but it works this way
        jmp     XmitEmpty70             ;Set new interrupt value

XmitEmpty	endp
page

;----------------------------Private-Routine----------------------------;
;
; ModemStatus - Modem Status Interrupt Handler
;
; Entry:
;	DS:SI --> DEB
;	DX     =  Port.IIDR
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AX,BX,CX,DI,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

		public	ModemStatus	;Public for debugging
ModemStatus	proc	near

;       Get the modem status value and shadow it for MSRWait.

        add     dl,ACE_MSR-ACE_IIDR     ;--> Modem Status Register
        in      al,dx
        mov     MSRShadow[si],al        ;Save MSR data for others
        mov     ch,al                   ;Save a local copy


;       Create the event mask for the delta signals

        mov     ah,al                   ;Just a lot of shifting
        shr     ax,1
        shr     ax,1
        shr     ah,1
        mov     cl,3
        shr     ax,cl
        and     ax,EV_CTS+EV_DSR+EV_RLSD+EV_Ring
        or      EvtWord[si],ax

        errnz      EV_CTS-0000000000001000b
        errnz      EV_DSR-0000000000010000b
        errnz     EV_RLSD-0000000000100000b
        errnz     EV_Ring-0000000100000000b

        errnz    ACE_DCTS-00000001b
        errnz    ACE_DDSR-00000010b
        errnz   ACE_DRLSD-00001000b
        errnz      ACE_RI-01000000b


ModemStatus10:
        mov     al,OutHHSLines[si]      ;Get output hardware handshake lines
        or      al,al                   ;Any lines that must be set?
        jz      ModemStatus30           ;No hardware handshake on output
        and     ch,al                   ;Mask bits of interest
        cmp     ch,al                   ;Lines set for Xmit?
        je      ModemStatus20           ;  Yes
        or      HSFlag[si],HHSDown      ;Show hardware lines have dropped
        jmp     short ModemStatus30


;       Lines are set for xmit.  Kick an xmit interrupt if needed

ModemStatus20:
        and     HSFlag[si],NOT HHSDown  ;Show hardware lines back up
	call	int_kick_tx		;Kick xmit interrupts if needed


ModemStatus30:
        jmp     InterruptLoop

ModemStatus	endp
page

;----------------------------Private-Routine----------------------------;
;
; int_kick_tx - Kick Transmitter
;
; "Kick" the transmitter interrupt routine into operation.
; If the Transmitter Holding Register isn't empty, then
; nothing needs to be done.  If it is empty, then the xmit
; interrupt needs to enabled in the IER.
;
; Entry:
;	DS:SI --> DEB
;	INTERRUPTS DISABLED!
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AX,DX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

int_kick_tx	proc	near

;       cli                             ;Done by caller
        mov     dx,Port[si]             ;Get device I/O address
        add     dl,ACE_LSR              ;Point at the line status reg
        in      al,dx                   ;And get it
        and     al,ACE_THRE             ;Check transmitter holding reg status
	jz	int_dont_kick_tx	;Busy, interrupt will hit soon enough

        sub     dl,ACE_LSR-ACE_IER      ;--> Interrupt enable register
        in      al,dx                   ;Get current IER state
        test    al,ACE_THREI            ;Interrupt already enabled?
	jnz	int_dont_kick_tx	;  Yes, don't reenable it
        or      al,ACE_THREI            ;  No, enable it
        out     dx,al
	pause				;8250, 8250-B bug requires
	out	dx,al			;  writting register twice

int_dont_kick_tx:
;       sti                             ;Done by caller
        ret

int_kick_tx	endp
page

;-----------------------------Public-Routine----------------------------;
;
; get_int_vector - get interrupt vector and IRQ
;
; The interrupt vector number and the handler address are returned
; to the caller.
;
; Entry:
;	DS:SI --> DEB
;	CH = Port base address (3F8 or 2F8)
;	DS = Data
; Returns:
;	AH = IRQ number
;	AL = 8259 Mask
;	DI:DX --> interrupt handler
; Error Returns:
;	None
; Registers Destroyed:
;	FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

;	assumes ds,Data
	assumes es,nothing

		public	get_int_vector
get_int_vector	proc	far

        cmp     ch,3                    ;Is it at 3xx?
	je	get_int_vect_3F8	;  Yes

	mov	deb_2F8,si		;COM port at 2F8
	mov	dx,IntCodeOFFSET COM_2F8
        mov     ax,IRQ3*256+00001000b   ;Int vector + 8259 mask
	jmp	short get_int_vect_exit

get_int_vect_3F8:
	mov	deb_3F8,si		;COM port at 3F8
	mov	dx,IntCodeOFFSET COM_3F8
        mov     ax,IRQ4*256+00010000b   ;Int vector + 8259 mask

get_int_vect_exit:
	mov	[DataSeg],ds		;Save for interrupt code
	mov	di,cs			;Return DI:DX --> int handler
	ret

get_int_vector	endp

page
ifdef DEBUG
	public	InterruptLoop
	public	IntLoop10
	public	IntLoop20
	public	DataAvail10
	public	DataAvail20
	public	DataAvail25
	public	DataAvail30
	public	DataAvail40
	public	DataAvail50
	public	DataAvail60
	public	DataAvail70
	public	DataAvail80
	public	DataAvail90
	public	DataAvail100
	public	DataAvail110
	public	DataAvail115
	public	DataAvail120
	public	XmitEmpty10
	public	XmitEmpty20
	public	XmitEmpty30
	public	XmitEmpty40
	public	XmitEmpty50
	public	XmitEmpty60
	public	XmitEmpty70
	public	XmitEmpty90
	public	XmitEmpty100
	public	ModemStatus10
	public	ModemStatus20
	public	ModemStatus30
	public	int_dont_kick_tx
	public	get_int_vect_3F8
	public	get_int_vect_exit
endif

sEnd	IntCode
end
