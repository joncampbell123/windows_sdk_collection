	page	,132
;---------------------------Module-Header-------------------------------;
; Module Name: IBMCOM.ASM
;
; !!!
;
; Created: Fri 06-Feb-1987 10:45:12
; Author:  Walt Moore [waltm]
;
; Copyright (c) 1985-1987  Microsoft Corporation
;
; General Description:
;
; History:
;						
;	***************************************************************
;       Tue Aug 30 12:52:00      MAJOR FIX TO HANDLE 8250B             
;       ---------------------------------------------------------------
;	
;	8250B has the following peculiar charactersistic
;             . The very first time (after reset) the Tx Holding Empty
;               interrupt is enabled, an immediate interrupt is generated
;
;             . After the first time, switching the Tx Holding Empty
;               interrupt enable bit from disabled to enabled will NOT
;               generate an immediate interrupt (unlike in 8250)
;       Because of this the KICKTX routine fails to set the transmit cycle
;       on if the machine has a 8250B
;	
;       This has been taken care as follows:
;             . For the very first byte that is being transmitted, KICKTX
;               is used to generate the first Tx Holding Empty interrupt
;             . Subsequently, whenever we find that the transmit buffer
;               is empty, we use a SOFTWARE INT (either INT 0BH, or INT 0CH)
;               to force the first character out, once this is done the
;               Tx Holding Empty interrupt will be generated once the buffer
;               really is empty
;             . Now we no longer disable the Tx Holding Empty interrupt
;               in the Xmit ISR to ensure that even m/cs with 8250, use
;               the software int to kick the tx interrupt on after the
;               first time.
;             . The software interrupt is also forced whenever an X-ON 
;               character is received.	
;
;       The code that implements the above logic is marked out with a line
;       asterixes.	
;	------------------------------------------------------------------
;	-by- Amit Chatterjee [amitc]    
;       ******************************************************************
;
;	062587	HSFlag and Evtmask in DoLPT.  These fields do not exist
;		for LPT type devices.  The code which manipulated them
;		was removed
;
;		KickTx from $SndCom - interrupts were not disabled when
;		calling KickTx.
;
;		$SetCom - added CLD at the start
;
;		$SetQue - movsw ==> stosw
;
;       111285  Changed the Timeout from 7 to 30 seconds.
;
;       110885  Forgot to set EV_RxChar event when a character
;               was received.
;
;       102985  INS8250, INS8250B bug with enabling interrupts.
;               Setting ACE_ETBEI in the Interrupt Enable Register
;               will cause an immediate interrupt regardless of
;               whether the transmitter register is empty or not.
;               The first interrupt MAY also be missed.
;
;               The first case is not a problem since we only enable
;               interrupts if the transmitter register is empty.  The
;               second problem was showing up on Microsoft System Cards
;               in PC-XTs.  The first interrupt was missed after a cold
;               boot.  National claims the fix is to write the register
;               twice, which SEEMS to work...
;
;               Added timeout code to $TRMCOM.  If the number of
;               characters in the output queue doesn't decrease
;               in "Timeout" seconds, then the port will be closed
;               anyway.  Also flushed the input queue and added a
;               discard-input flag for the data available interrupt
;               code to discard any input received while terminating
;               a port.  $TRMCOM will return an error code if it
;               discarded any output data.
;
;               Removed infinite timeout test in MSRWait routine.
;               Still bad, but it will timeout around 65 seconds
;               instead of never.
;
;       102785  LPT initialization code was jumping to InitCom90,
;               which was setting EFlags[si] to null.  Well, LPTs
;               don't have an EFlags field, so the null was getting
;               stuffed over the LSB of BIOSPortLoc of the next LPT
;               device.
;
;       101185  Save interrupt vector when opening a comm port
;               and restore it when closing.  Would you believe
;               there are actually programs that assume the
;               vector points to a non-specific 8259 ACK and
;               an IRET!
;
;       100985  Added MS-NET support to gain exclusive control
;               of an LPT port if DOS 3.x and not running in as
;               a server, receiver, or messenger.   Required to
;               keep another application, such as command.com
;               from closing the stream or mixing their output
;               with ours.
;-----------------------------------------------------------------------;

        title   IBMCom - IBM PC, PC-XT, PC-AT Communications Interface

        .xlist
        include cmacros.inc
        include comdev.inc
        include ins8250.inc
	include ibmcom.inc
        .list

	externFP CreateSystemTimer
	externFP KillSystemTimer
	externFP get_int_vector


sBegin	Data

        public  Comm1
        public  Comm2
Comm1   ComDEB  <,,,RS232B+0,,,,,,,,,,,,,,,,,,,,,,,,>
Comm2   ComDEB  <,,,RS232B+2,,,,,,,,,,,,,,,,,,,,,,,,>

        public  LPT1
        public  LPT2
        public  LPT3
LPT1    LptDEB  <,,,LPTB+0>             ;LPT1 device equipment block
LPT2    LptDEB  <,,,LPTB+2>             ;LPT2 device equipment block
LPT3    LptDEB  <,,,LPTB+4>             ;LPT3 device equipment block


$MachineID	db	0		;IBM Machine ID
InitRetry       dw      0               ;LPT Retry count
First_After_Boot db	0ffh

sEnd		Data



HighSeg         segment at 0FFFFh
                org     0Eh
MachineID       label   byte
HighSeg         Ends



sBegin	Code
assumes cs,Code
assumes ds,Data

page

;----------------------------Public Routine-----------------------------;
;
; $INICOM - Initialize A Port
;
; Initalizes the requested port if present, and sets
; up the port with the given attributes when they are valid.
; This routine also initializes communications buffer control
; variables.  This routine is passed the address of a device
; control block.
;
; The RLSD, CTS, and DSR signals should be ignored by all COM
; routines if the corresponding timeout values are 0.
;
; For the LPT ports, a check is performed to see if the hardware
; is present (via the LPT port addresses based at 408h.  If the
; port is unavailable, an error is returned.  If the port is
; available, then the DEB is set up for the port.  $SETCOM will
; be called to set up the DEB so that there will be something
; valid to pass back to the caller when he inquires the DEB.
;
; No hardware initialization will be performed to prevent the
; RESET line from being asserted and resetting the printer every
; time this routine is called.
;
; Entry:
;	EX:BX --> Device Control Block with all fields set.
; Returns:
;	AX = 0 if no errors occured
; Error Returns:
;	AX = initialization error code otherwise
; Registers Preserved:
;	None
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	$INICOM
$INICOM proc	near

        push    si                      ;As usual, save register variables
        push    di
        mov     ah,es:ID[bx]            ;Get device i.d.
	call	GetDEB			;--> DEB for this device
        jnc     InitCom30               ;Valid device

InitCom10:
        mov     ax,IE_BadID             ;Show bad device

InitCom20:
        jmp     InitCom100


InitCom30:
        push    ds
        mov     di,BIOSPortLoc[si]      ;Get offset to port address
	mov	ax,HighSeg		;Get Machine ID
;	errnz	HighSeg-0FFFFH
        mov     ds,ax
	assumes ds,HighSeg

        mov     al,[MachineID]


;       Determine if the hardware for the requested device is present. This is
;       flagged at boot time by the BIOS, which places the I/O address of the
;       comm port at 0:400 or 0:402 if the hardware is present.  LPT port
;       addresses are placed at 0:408, 0:40A, or 0:40C

        xor     cx,cx                   ;0 = port not present or unavailable
        mov     ds,cx                   ;Point DS: at ROS Save Area.
        assumes ds,nothing

        xchg    wo [di],cx              ;Get port, show unavailable
        pop     ds                      ;Done playing with DS (for a while)
	assumes ds,Data

        mov     [$MachineID],al         ;Save now that DS is restored
        mov     ax,IE_Hardware          ;Hardware present?
        jcxz    InitCom20               ;  No, return error code
        mov     Port[si],cx             ;Save the port address
        test    di,LPTB-RS232B          ;Is this an LPT port?
        jz      InitCom40               ;  No, set up interrupt stuff
	call	$SETCOM 		;Set up LPT Device
        mov     ax,30                   ;Assume timeout for an AT (~2 mSec)
        cmp     [$MachineID],0FCh       ;Is this an AT?
        je      InitCom35               ;  Yes
        mov     ax,10                   ;  No, set XT timeout (~2 mSec)

InitCom35:
        mov     [InitRetry],ax


;	Check to see if running under DOS 3.x and need to gain
;	exclusive control of the spooler for the given LPT port.

	call	TestInt2F		;See if int 2F calls allowed
	jc	InitCom37		;Cannot make int 2F calls
	mov	dl,ID[si]		;Set port number for checking
	and	dx,11b			;  for Int 2F
	mov	ax,Lock 		;Gain exclusive control
	int	2Fh
	mov	ax,Open 		;Start a new stream
	int	2Fh
	xor	cx,cx			;Zero BIOS port address just
	mov	bx,BIOSPortLoc[si]	;  incase redirector stuffed
	push	ds			;  a non-zero value there
	mov	ds,cx
	assumes ds,nothing

	mov	word ptr [bx],cx
	pop	ds
	assumes ds,Data

InitCom37:
	jmp	      InitCom95 	;That's all


InitCom40:
	push	di			;AX,DI:DX will have return values
	call	get_int_vector		;Get interrupt vector for port/deb
	mov	wo Mask8259[si],ax	;Save 8259 int mask, int vector
	errnz	IntVecNum-Mask8259-1

        cli                             ;Disable 8259 interrupts for
        in      al,INTA1                ;  the given COM port
        or      al,Mask8259[si]
        pause
        out     INTA1,al
        sti

	push	es			;Ask DOS for the old interrupt vector
	push	bx			;  so we can restore it when we're done
	mov	al,ah
	mov	ah,35h
	int	21h
	mov	wo OldIntVec[si][0],bx
	mov	wo OldIntVec[si][2],es
	pop	bx
	pop	es
	assumes es,nothing


;       Ask DOS to set up the interrupt vector we want.

        push    ds                      ;Save original DS
	mov	al,IntVecNum[si]	;Interrupt vector number		     ;Get the interrupt number
	mov	ds,di			;Interrupt handler address in ds:dx
	assumes ds,nothing

        mov     ah,25h                  ;DOS Set Vector Function
        int     21h                     ;Set the DOS vector
        pop     ds                      ;Original DS
	assumes ds,Data
	pop	di

InitCom60:
        mov     dx,cx                   ;Set comm card address
        xor     ax,ax                   ;Need a zero
        inc     dx                      ;--> Interrupt Enable Register
        errnz   ACE_IER-ACE_RBR-1
        out     dx,al                   ;Turn off interrupts
        add     dl,ACE_MCR-ACE_IER      ;--> Modem Control Register
        pause                           ;Delay
	in	al,dx
	and	al,ACE_DTR+ACE_RTS	;Leave DTR, RTS high if already so
	pause				;  but tri-state IRQ line
	out	dx,al

InitCom70:
        push    es                      ;Zero queue counts and indexes
        push    ds
        pop     es
	assumes es,Data

        lea     di,QInCount[si]
        mov     cx,(EFlags-QInCount)/2
        errnz   <(EFlags-QInCount) AND 1>
	xor	ax,ax
        rep     stosw
        pop     es
	assumes es,nothing


        errnz   QInGet-QInCount-2
        errnz   QInPut-QInGet-2
        errnz   QOutCount-QInPut-2
        errnz   QOutGet-QOutCount-2
	errnz	QOutPut-QOutGet-2
        errnz   EFlags-QOutPut-2        ;First non-queue item

        mov     HSFlag[si],al           ;Show no handshakes yet
        mov     EvtWord[si],ax          ;Show no events


;       Call $SETCOM to perform further hardware initialization.

InitCom80:
        call    $SETCOM                 ;Set up Comm Device
        jz      InitCom85               ;Success
        push    ax                      ;Failure, save error code
	call	Terminate45		;Restore port address, int vec
        pop     ax                      ;Restore error code and exit
        jmp     short InitCom100

InitCom85:
        cli
        in      al,INTA1                ;Enable interrupts at 8259
        mov     ah,Mask8259[si]         ;Zero turns it on.
        not     ah
        and     al,ah
        out     INTA1,al
        sti

InitCom90:
	mov	EFlags[si],0		;Clear internal state

InitCom95:
	xor	ax,ax			;Return AX = 0 to show success
	mov	ComErr[si],ax		;Get rid of any bogus init error

InitCom100:
        pop     di
        pop     si
        ret

$INICOM endp
page

;----------------------------Public Routine-----------------------------;
;
; $SNDIMM - Send A Character Immediately
;
; This routine either sends a character to the port immediately,
; or places the character in a special location which is used by
; the next transmit interrupt to transmit the character prior to
; those in the normal transmit queue.
;
; For LPT ports, the character is always sent immediately.
;
; Entry:
;	AH = Device ID
;	AL = Character
; Returns:
;	AX = 0
; Error Returns:
;	AX = 8000H if Bad ID
;	AX = 4000H if couldn't send because another character
;	     transmitted "immediately" is waiting to be sent
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	$SNDIMM
$SNDIMM proc	near

        push    si
        call    GetDEB                  ;Get pointer to the DEB
        jc      SendImm30               ;Bad ID, return an error
        jns     SendImm10               ;Its a COM port


;       For LPT ports, call $SNDCOM to do the dirty work.  If $SNDCOM
;       returns an error code, map it to 4000h.

	call	$SNDCOM 		;Do the dirty work here
        or      ax,ax                   ;Error occur?
        jnz     SendImm40               ;  Yes, return 4000h
        jmp     short SendImm20         ;  No, show all is OK


SendImm10:
        test    EFlags[si],fTxImmed     ;Another char waiting "immediately"?
        jnz     SendImm40               ;  Yes, return error
        mov     ah,al                   ;Set char for TXI
        cli                             ;TXI is critical section code
        call    TXI                     ;Set character to tx immediately
        sti

SendImm20:
        xor     ax,ax                   ;Show all is OK

SendImm30:
        pop     si
        ret

SendImm40:
        mov     ax,4000h                ;In case we cannot send
        pop     si
        ret

$SNDIMM endp
page

;----------------------------Public Routine-----------------------------;
;
; $SNDCOM - Send Byte To Port
;
; The given byte is sent to the passed port if possible.
; If the output queue is full, an error will be returned.
;
; Entry:
;	AH = Device ID
;	AL = Character
; Returns:
;	AX = 0
; Error Returns:
;	AX = error code
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	$SNDCOM
$SNDCOM proc	near

        push    si
        push    di
        call    GetDEB                  ;--> DEB
        jc      SendCom40               ;Invalid ID
        jns     SendCom20               ;Its a COM port


;       Handle the transmission of a LPT character.  The ROM BIOS int 17
;       call will be made to do the transmission.  The port address will
;       be restored during the call, then zeroed out upon return.

SendCom10:
        xor     ch,ch                   ;Show xmit character
        call    DoLPT                   ;Do the work here
        jmp     short SendCom40         ;Return the status to caller


;       Send a character to a COM port.  Return an error if control
;       line timeout occurs or there is no room in the output queue.

SendCom20:
        push    ax                      ;Save character
	

        call    MSRWait                 ;See if lines are correct for output
        pop     ax                      ;Restore char
        jnz     SendCom60               ;Timeout occured, return error
        mov     cx,QOutSize[si]         ;See if queue is full
        cmp     cx,QOutCount[si]
        jle     SendCom50               ;There is no room in the queue
        les     di,QOutAddr[si]         ;--> output queue
	assumes es,nothing

        mov     bx,QOutPut[si]          ;Get index into queue
        mov     es:[bx][di],al          ;Store the byte
        inc     bx                      ;Update index
        cmp     bx,cx                   ;Wrap time?
        jc      SendCom30               ;  No
        xor     bx,bx                   ;Wrap-around is a new zero pointer

SendCom30:

;******************************************************************************
;ForcedInt is used as a flag in the case the forced software interrupt is
;generated.
;First_After_Boot is 0ffh when the driver is loaded and for the very first
;time the software int is not necessary

        cli
        mov     QOutPut[si],bx          ;Store updated pointer
	mov	ax,QOutCount[si]	; get the count
	inc	ax 		        ; have the updated value in AX for test later
        mov     QOutCount[si],ax        ;Update queue population
	push	ax
        call    KickTx                  ;Make sure xmit interrupt is armed
	sti
	pop	ax
	cmp	First_After_Boot,0ffh	; is this the very first char sent ?   
	jz	Forced_Int_Not_Req	; first int comes after enable
	cmp	ax,1			; 1st char in new packet
	jnz	Forced_Int_Not_Req
	mov	ForcedInt[si],ax	; set forced interrupt flag
	cmp	IntVecNum[si],0ch	; is it int 0ch ?
	jnz	vector_is_0bh		; no, it then has to be int 0bh
	int	0ch			; force the int
	jmp	short Forced_Int_Not_req
vector_is_0bh:
	int	0bh			; vector for the other comms port
Forced_Int_Not_Req:
	mov	First_After_Boot,0      ; reset
        xor     ax,ax                   ;Show no error (that we know of)

;****************************************************************************

SendCom40:
        pop     di
        pop     si
        ret

SendCom50:
        or      by ComErr+1[si],HIGH CE_TXFULL
        errnz   <LOW CE_TXFULL>

SendCom60:
        mov     ax,ComErr[si]           ;Return error code to caller
        jmp     SendCom40

$SNDCOM endp

page

;----------------------------Private-Routine----------------------------;
;
; TimerProc - Decrement Timeout Counter
;
; The timer is decremented by one.  Since Windows is a
; non-preemptive system, there will not be two timers
; running at the same time.
;
; Entry:
;	None
; Returns:
;	None
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

	assumes ds,nothing
	assumes es,nothing

TicCount	dw	0		;Timeout counter

TimerProc	proc	far

	dec	[TicCount]
	ret

TimerProc	endp
page

;----------------------------Public Routine-----------------------------;
;
; $TRMCOM - Terminate Communications Channel
;
; Wait for any outbound data to be transmitted, drop the hardware
; handshaking lines, and disable interrupts.  If the output queue
; contained data when it was closed, an error will be returned
;
; LPT devices have it easy.  They just need to restore the I/O port
; address.
;
; Entry:
;	AH = Device ID
; Returns:
;	AX = 0
; Error Returns:
;	AX = 8000h if invalid device ID
;	AX = -2 if output queue timeout occured
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	$TRMCOM
$TRMCOM proc	near

        push    si
        push    di
	xor	cx,cx			;Show no error if LPT port
	call	GetDEB
	jc	TermCom60		;ID is invalid, return error
	js	TermCom30		;Port is a LPT port
	push	ax			;Save port id
	or	HSFlag[si],Discard	;Show discarding serial data
	mov	ComErr[si],cx		;Clear error flags
	mov	QInCount[si],cx 	;Show no chars in input queue
	call	$RECCOM 		;Send XON if needed


;-----------------------------------------------------------------------;
;	We have to wait for the output queue to empty.	To do this,
;	a timer will be created.  If no character has been transmitted
;	when the timeout occurs, then an error will be indicated and
;	the port closed anyway.  If the timer cannot be created, then
;	just loop until the queue empties, which will be better than
;	discarding charatcers if there are any
;-----------------------------------------------------------------------;


	mov	cx,QOutCount[si]	;Any chars in output queue?
	jcxz	TermCom20		;  No, skip timer stuff

	mov	ax,1000 		;Create a 1 second timer
	mov	bx,CodeOFFSET TimerProc ;--> timer processor
	farPtr	lpTime,cs,bx
	cCall	CreateSystemTimer,<ax,lpTime>
	assumes es,nothing

TermCom10:
	mov	cx,QOutCount[si]	;Get current queue count
	jcxz	TermCom16		;No characters in queue
	mov	[TicCount],Timeout	;Restart timeout counter

TermCom12:
	cmp	QOutCount[si],cx	;Queue count change?
	jne	TermCom10		;  Yes, restart timeout
	cmp	[TicCount],0		;Timeout reached?
	jge	TermCom12		;  No, keep waiting
	mov	cx,TimeoutError 	;  Yes, show timeout error

TermCom16:
; Clear the TicCount so we don't get "Segment contents trashed" RIPs
	mov	[TicCount],0		; (Fri 30-Oct-1987 : bobgu)
	or	ax,ax			;Was the timer created?
	jz	TermCom20		;  No, cannot kill it
	push	cx			;Save timeout error code
	cCall	KillSystemTimer,<ax>
	assumes es,nothing
	pop	cx

TermCom20:
	pop	ax			;Restore cid

TermCom30:
	mov	dx,Port[si]		;Get port base address
	call	Terminate		;The real work is done here
	mov	ax,cx			;Set return code

TermCom60:
        pop     di
        pop     si
        ret

$TRMCOM endp
page

;----------------------------Private-Routine----------------------------;
;
; Terminate - Terminate Device
;
; Restore the port I/O address and make sure that interrupts are off
;
; Entry:
;	AH = Device Id.
;	DX = Device I/O port address.
;	SI --> DEB
; Returns:
;	AX = 0
; Error Returns:
;	AX = -1
; Registers Destroyed:
;	AX,BX,DX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

		public	Terminate	;Public for debugging
Terminate	proc	near

        or      ah,ah                   ;LPT port?
	jns	Terminate10		;  No, process COM port
	errnz	LPTx-10000000b


;-----------------------------------------------------------------------;
;	Check to see if running and need to release exclusive
;	control of the spooler for the given LPT port.
;-----------------------------------------------------------------------;

	call	TestInt2F		;See if int 2F calls allowed
	jc	Terminate5		;Cannot make int 2F calls
	mov	dl,ID[si]		;Set port number for checking
	and	dx,11b			;  for Int 2F
	mov	ax,Close		;Close stream
	int	2Fh
	mov	ax,Unlock		;Release exclusive control
	int	2Fh

Terminate5:
	mov	dx,Port[si]		;Restore port address
	jmp	short Terminate50	;That's all


;-----------------------------------------------------------------------;
;	It a com port!
;
;       We delay for a bit while the last character finishes transmitting
;       Then we drop DTR and RTS, and disable the interrupt generation at
;       the 8250.  Even if fRTSDisable or fDTRDisable is set, those lines
;       will be dropped when the port is closed.
;-----------------------------------------------------------------------;

Terminate10:
        inc     dx                      ;Disable chip interrupts
        errnz   ACE_IER-ACE_RBR-1
        xor     ax,ax
        out     dx,al
        add     dl,ACE_LSR-ACE_IER      ;--> line status register
        pause

Terminate20:
        in      al,dx                   ;Wait until xmit is empty
        and     al,ACE_THRE+ACE_TSRE
        cmp     al,ACE_THRE+ACE_TSRE
        jne     Terminate20             ;Not empty yet

Terminate30:
        dec     dx                      ;--> Modem Status Register
        errnz   ACE_LSR-ACE_MCR-1
	in	al,dx
	and	al,ACE_DTR+ACE_RTS	;Leave DTR, RTS high if already so
	pause				;  but tri-state IRQ line
	out	dx,al
        sub     dl,ACE_MCR-ACE_RBR      ;Set back to base address

Terminate40:
        cli
	in	al,INTA1		;Get INT masks from 8259 IMR Reg.
        or      al,Mask8259[si]         ;Disable IRQ-3 or IRQ-4
        pause
        out     INTA1,al
        sti

Terminate45:
	push	ds			;Save original DS
	mov	al,IntVecNum[si]	;Get the interrupt number
	lds	dx,OldIntVec[si]	;Int handler address in ds:dx
	assumes ds,nothing

	mov	ah,25h			;DOS Set Vector Function
	int	21h			;Set the DOS vector
	pop	ds			;Original DS
	assumes ds,Data

	mov	dx,Port[si]		;Get port address to restore


Terminate50:                            ;Also called from $INICOM !
        mov     bx,BIOSPortLoc[si]      ;Get offset where port addr goes
        push    ds
        xor     ax,ax                   ;Indicate no error (also BIOS data seg)
        mov     ds,ax                   ;Point DS: at BIOS Save Area.
	assumes ds,nothing

        mov     wo [bx],dx              ;Restore I/O addr.
        pop     ds
	assumes ds,Data

        ret                             ;Port is closed and deallocated

Terminate	endp
page

;----------------------------Private-Routine----------------------------;
;
; MSRWait - Modem Status Register Wait
;
; This routine checks the modem status register for CTS, DSR,
; and/or RLSD signals.	If a timeout occurs while checking,
; the appropriate error code will be returned.
;
; This routine will not check for any signal with a corresponding
; time out value of 0 (ignore line).
;
; Entry:
;	SI --> DEB
; Returns:
;	AL = error code
;	ComErr[si] updated
;	'Z' set if no timeout
; Error Returns:
;	None
; Registers Destroyed:
;	AX,CX,DX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	MSRWait 		;Public for debugging
MSRWait proc	near

        push    di

MSRRestart:
        xor     di,di                   ;Init Timer

MSRWait10:
        mov     cx,11                   ;Init Delay counter

MSRWait20:
        xor     dh,dh                   ;Init error accumulator
        mov     al,MSRShadow[si]        ;Get Modem Status
        and     al,MSRMask[si]          ;Only leave bits of interest
        xor     al,MSRMask[si]          ;0 = line high
        jz      MSRWait90               ;All lines of interest are high
        mov     ah,al

        cmp     di,RLSTimeout[si]       ;RLSD timeout yet?
        jb      MSRWait30               ;  No
        shl     ah,1                    ;Line Signal Detect low?
        jnc     MSRWait30               ;  No, it's high
        or      dh,CE_RLSDTO            ;Show modem status timeout
        errnz   ACE_RLSD-10000000b

MSRWait30:
        cmp     di,DSRTimeout[si]       ;DSR timeout yet?
        jb      MSRWait40               ;  No
        shr     ah,1                    ;Data Set Ready low?
        shr     ah,1
        jnc     MSRWait40               ;  No, it's high
        or      dh,CE_DSRTO             ;Show data set ready timeout
        errnz   ACE_DSR-00100000b

MSRWait40:
        cmp     di,CTSTimeout[si]       ;CTS timeout yet?
        jb      MSRWait50               ;  No
        shr     ah,1                    ;CTS low?
        jnc     MSRWait50               ;  No, it's high
        or      dh,CE_CTSTO             ;Show clear to send timeout
        errnz   ACE_CTS-00010000b

MSRWait50:
        or      dh,dh                   ;Any timeout occur?
        jnz     MSRWait80               ;  Yes

        cmp     [$MachineID],0FCh       ;Is this a PC-AT?
        je      MSRWait60               ;  Yes, use ROM function
        loop    MSRWait20               ;  No, continue until timeout
        jmp     short MSRWait70         ;Should have taken about a millisecond

MSRWait60:
        push    bx                      ;Special SALMON ROM routine to delay
	push	di
        xor     cx,cx                   ;Number of Microseconds to delay
	mov	dx,1000 		;  in CX:DX
        mov     ah,86h
        int     15h                     ;Wait 1 millisecond
        pop     di
        pop     bx

MSRWait70:
        inc     di                      ;Timer +1
        jmp     MSRWait10               ;Until Timeout or Good status

MSRWait80:
        xor     ah,ah
        mov     al,dh
        or      by ComErr[si],al        ;Return updated status
        errnz   <HIGH CE_CTSTO>
        errnz   <HIGH CE_DSRTO>
        errnz   <HIGH CE_RLSDTO>

MSRWait90:
        or      al,al                   ;Set 'Z' if no timeout
        pop     di
        ret

MSRWait endp
page

;----------------------------Public Routine-----------------------------;
;
; $SETCOM - Set Communications parameters
;
; Re-initalizes the requested port if present, and sets up the
; port with the given attributes when they are valid.
;
; For LPT ports, just copies whatever is given since it's ignored
; anyway.
;
; Entry:
;	ES:BX --> DCB with all fields set.
; Returns:
;	'Z' Set if no errors occured
;	AX = 0
; Error Returns:
;	'Z' clear if errors occured
;	AX = initialization error code.
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

	public	$SETCOM
$SETCOM proc	near

	cld
        push    si
        push    di
        mov     ah,es:ID[bx]            ;Get device i.d.
        call    GetDEB                  ;Get DEB pointer in SI
        mov     ax,IE_BadID             ;Assume unknown device
        jc      SetCom10                ;Invalid device, return error
        jns     SetCom20                ;COM port
        call    SetCom100               ;Copy the DCB

SetCom5:
        xor     ax,ax                   ;Show no error

SetCom10:
        or      ax,ax                   ;Set/clear 'Z'
        pop     di                      ;  and exit
        pop     si
        ret


;-----------------------------------------------------------------------;
;       Have a comm device, check all the serial parameters to make
;       sure they are correct before moving the new DCB into our space
;       and changing the ACE parameters.
;-----------------------------------------------------------------------;

SetCom20:
        call    SetCom300               ;Baud rate valid?
        jcxz    SetCom10                ;  No, return error
        call    SetCom400               ;Byte size/parity/stop bits correct?
        jc      SetCom10                ;  No, return error


;       The parameters seem correct.  Copy the DCB into our space and
;       initialize the ACE with the new parameters

        mov     dx,Port[si]             ;Disable interrupts from the 8250
        inc     dx
        errnz   ACE_IER-1
        xor     ax,ax
        out     dx,al

        call    SetCom100               ;Copy the DCB
	mov	bx,si			;Set ES:BX --> DCB
        call    SetCom200               ;Get timeout masks
        xchg    al,ah                   ;Want them in the correct registers
        mov     wo MSRMask[si],ax
        errnz   MSRInfinite-MSRMask-1

        call    SetCom400               ;Get line control byte
        push    ax                      ;  and save LCR value
        inc     dx                      ;--> LCR
        inc     dx
        errnz   ACE_LCR-ACE_IER-2
        or      al,ACE_DLAB             ;Want access to divisor latch
        out     dx,al
        mov     RxMask[si],ah           ;Save Receive character mask
        mov     ax,di                   ;Get flags mask, error mask
        and     Flags[si],ah            ;Disable parity checking if no parity
        mov     ErrorMask[si],al        ;Save line status error mask

        call    SetCom300               ;Get baud rate
        sub     dl,ACE_LCR-ACE_DLL      ;--> LSB of divisor latch
        mov     al,cl
        out     dx,al
        mov     al,ch
        inc     dx                      ;--> MSB of divisor latch
        errnz   ACE_DLM-ACE_DLL-1
        pause
        out     dx,al
        inc     dx                      ;--> LCR and clear divisor access bit
        inc     dx
        errnz   ACE_LCR-ACE_DLM-2
        pop     ax
        out     dx,al

        inc     dx                      ;--> Modem Control Register
        errnz   ACE_MCR-ACE_LCR-1



;-----------------------------------------------------------------------;
;       Compute initial state of DTR and RTS.  If they have been disabled,
;       then do not raise them, and disallow being used as a handshaking
;       line.  Also compute the bits to use as hardware handshake bits
;       (DTR and/or RTS as indicated, qualified with the disabled flags).
;-----------------------------------------------------------------------;


        mov     al,Flags[si]            ;Align DTR/RTS disable flags for 8250
	and	al,fRTSDisable+fDTRDisable
        rol     al,1                    ;d0 = DTR, d2 = RTS  (1 = disabled)
        shr     al,1                    ;'C'= DTR, d1 = RTS
        adc     al,0                    ;d0 = DTR, d1 = RTS
        errnz   fRTSDisable-00000010b
        errnz   fDTRDisable-10000000b
        errnz   ACE_DTR-00000001b
        errnz   ACE_RTS-00000010b

        mov     ah,al                   ;Save disable mask
        xor     al,ACE_DTR+ACE_RTS+ACE_OUT2
        out     dx,al                   ;Set Modem Control Register

	mov	al,Flags2[si]		;Get hardware handshake flags
        rol     al,1                    ;Align flags as needed
        rol     al,1
        rol     al,1
        and     al,ACE_DTR+ACE_RTS      ;Mask bits of interest
        not     ah                      ;Want inverse of disable mask
        and     al,ah                   ;al = bits to handshake with
        mov     HHSLines[si],al         ;Save for interrupt code

        errnz   fDTRFlow-00100000b
        errnz   fRTSFlow-01000000b
        errnz    ACE_DTR-00000001b
        errnz    ACE_RTS-00000010b


	mov	al,Flags[si]		;Compute the mask for the output
	shl	al,1			;  hardware handshake lines
        and     al,ACE_DSR+ACE_CTS
        mov     OutHHSLines[si],al

        errnz   fOutXCTSFlow-00001000b
        errnz   fOutXDSRFlow-00010000b
        errnz        ACE_CTS-00010000b
        errnz        ACE_DSR-00100000b



;       Compute the queue count where XOff should be issued (or hardware
;       lines dropped).  This will prevent having to do it at interrupt
;       time.

        mov     ax,QInSize[si]          ;Get where they want it
	sub	ax,XOFFLim[si]		;  and compute queue count
        mov     XOffPoint[si],ax

	sub	dl,ACE_MCR		;Delay a bit waiting for things
	call	initialize_delay
        add     dl,ACE_MSR              ;--> Modem Status reg
        pause
        in      al,dx                   ;Throw away 1st status read
        pause
        in      al,dx                   ;Save 2nd for MSRWait (Clear MSR int)
        mov     MSRShadow[si],al


;-----------------------------------------------------------------------;
;       Now, at last, interrupts can be enabled.  Don't enable the
;       transmitter empty interrupt.  It will be enabled by the first
;       call to KickTx.
;-----------------------------------------------------------------------;

        sub     dl,ACE_MSR-ACE_IER      ;--> Interrupt Enable Register
        mov     al,ACE_ERBFI+ACE_ELSI+ACE_EDSSI
        cli
        out     dx,al                   ;Enable interrupts.
        add     dl,ACE_LSR-ACE_IER      ;--> Line Status Register
        pause
        in      al,dx                   ;Clear any Line Status interrupt
        sub     dl,ACE_LSR              ;--> Receiver Buffer Register
        pause
        in      al,dx                   ;Clear any Received Data interrupt
        sti
        jmp     SetCom5                 ;All done

$SETCOM endp
page

;----------------------------Private-Routine----------------------------;
;
; SetCom100
;
;  Copy the given DCB into the appropriate DEB.  The check has
;  already been made to determine that the ID was valid, so
;  that check can be skipped.
;
; Entry:
;	ES:BX --> DCB
;	DS:SI --> DEB
; Returns:
;	DS:SI --> DEB
;	ES     =  Data
; Error Returns:
;	None
; Registers Destroyed:
;	AX,CX,ES,DI,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

SetCom100	proc	near
        push    si                      ;Save DEB pointer
        mov     di,si
        mov     si,bx
        push    es
        mov     ax,ds
        pop     ds
	assumes ds,nothing

        mov     es,ax
	assumes es,Data

	mov	cx,DCBSize
        rep     movsb
        mov     ds,ax
	assumes ds,Data

        pop     si                      ;Restore DEB pointer
        ret

SetCom100	endp
page

;----------------------------Private-Routine----------------------------;
;
; SetCom200
;
; Based on whether or not a timeout has been specified for each
; signal, set up a mask byte which is used to mask off lines for
; which we wish to detect timeouts.  0 indicates that the line is
; to be ignored.
;
; Also set up a mask to indicate those lines which are set for
; infinite timeout.  1 indicates that the line has infinite
; timeout.
;
; Entry:
;	ES:BX --> DCB
; Returns:
;	ES:BX --> DCB
;	AH = lines to check
;	AL = lines with infinite timeout
; Error Returns:
;	None
; Registers Destroyed:
;	AX,CX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

SetCom200	proc	near

        xor     ax,ax
        xor     cx,cx                   ;Get mask of lines with timeout = 0
        call    SetCom210
        not     al                      ;Invert result to get lines to check
        and     al,ACE_CTS+ACE_DSR+ACE_RLSD
        xchg    ah,al
        dec     cx                      ;Get mask of infinite timeouts

SetCom210:
        cmp     es:RLSTimeout[bx],cx    ;Timeout set to passed value?
        jne     SetCom220               ;  No
        or      al,ACE_RLSD             ;  Yes, show checking line

SetCom220:
        cmp     es:CTSTimeout[bx],cx    ;Timeout set to passed value?
        jne     SetCom230               ;  No
        or      al,ACE_CTS              ;  Yes, show checking line

SetCom230:
        cmp     es:DSRTimeout[bx],cx    ;Timeout set to passed value?
        jne     SetCom240               ;  No
        or      al,ACE_DSR              ;  Yes, show checking line

SetCom240:
        ret

SetCom200	endp
page

;----------------------------Private-Routine----------------------------;
;
; SetCom300
;
; Calculate the correct baudrate divisor for the comm chip.
;
; Note that the baudrate is allowed to be any integer in the
; range 2-19200.  The divisor is computed as 115,200/baudrate.
;
; Entry:
;	ES:BX --> DCB
; Returns:
;	ES:BX --> DCB
;	CX = baudrate
; Error Returns:
;	CX = 0 if error
;	AX = error code if invalid baud rate
; Registers Destroyed:
;	AX,CX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

SetCom300	proc	near

        push    dx
        mov     cx,es:BaudRate[bx]      ;Get requested baud rate
        xor     ax,ax                   ;Assume error
	cmp	cx,2			;Within valid range?
        jnae    SetCom310               ;  No, return error
        cmp     cx,19201
        jae     SetCom310               ;  No, return error

        mov     dx,1                    ;(dx:ax) = 115,200
        mov     ax,0C200h
        div     cx                      ;(ax) = 115,200/baud

SetCom310:
        mov     cx,ax                   ;(cx) = baud rate, or error code (0)
        mov     ax,IE_Baudrate          ;Set error code incase bad baud
        pop     dx
        ret

SetCom300	endp
page

;----------------------------Private-Routine----------------------------;
;
; SetCom400
;
; Check the line configuration (Parity, Stop bits, Byte size)
;
; Entry:
;	ES:BX --> DCB
; Returns:
;	ES:BX --> DCB
;	'C' clear if OK
;	AL = Line Control Register
;	AH = RxMask
;	DI[15:8] = Flags mask (to remove parity checking)
;	DI[7:0]  = Error mask (to remove parity error)
; Error Returns:
;	'C' set if error
;	AX = error code
; Registers Destroyed:
;	AX,CX,DI,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

SetCom400	proc	near

        mov     ax,wo es:ByteSize[bx]   ;al = byte size, ah = parity
        cmp     ah,SpaceParity          ;Parity out of range?
        ja      SetCom470               ;  Yes, return error
        mov     di,0FF00h+ACE_OR+ACE_PE+ACE_FE+ACE_BI
        or      ah,ah                   ;Is parity "NONE"?
        jnz     SetCom410               ;  No, something is there for parity
        xor     di,(fParity*256)+ACE_PE ;Disable parity checking

SetCom410:
        cmp     al,8                    ;Byte size out of range?
        ja      SetCom460               ;  Yes, error

SetCom420:
        sub     al,5                    ;Shift byte size to bits 0&1
        errnz   ACE_WLS-00000011b       ;Word length must be these bits
        jc      SetCom460               ;Byte size is illegal, return error
        add     ah,ah                   ;Map parity to ACE bits
        jz      SetCom430               ;0=>0, 1=>1, 2=>3, 3=>5, 4=>7
        dec     ah

SetCom430:
        shl     ah,1                    ;Align with 8250 parity bits
        shl     ah,1
        shl     ah,1
        or      al,ah                   ;Add to byte size

        errnz   NoParity-0
        errnz   OddParity-1
        errnz   EvenParity-2
        errnz   MarkParity-3
        errnz   SpaceParity-4
        errnz   ACE_PEN-00001000b
        errnz   ACE_PSB-00110000b
        errnz   ACE_EPS-00010000b
        errnz    ACE_SP-00100000b


        or      al,ACE_2SB              ;Assume 2 stop bits
        mov     ah,es:StopBits[bx]      ;Get # of stop bits 0=1,1/2= .GT. 1
        or      ah,ah                   ;Out of range?
        js      SetCom470               ;  Yes, return error
        jz      SetCom440               ;One stop bit
        sub     ah,2
        jz      SetCom450               ;Two stop bits
        jns     SetCom470               ;Not 1.5, return error
        test    al,ACE_WLS              ;1.5 stop bits, 5 bit words?
        jnz     SetCom470               ;  No, illegal
        errnz   OneStopBit-0
        errnz   One5StopBits-1
        errnz   TwoStopBits-2
        errnz   ACE_5BW

SetCom440:
        and     al,NOT ACE_2SB          ;Show 1 (or 1.5) stop bit(s)


;       From the byte size, get a mask to be used for stripping
;       off unused bits as the characters are received.

SetCom450:
        push    dx
        mov     cl,es:ByteSize[bx]      ;Get data byte size
        mov     dx,00FFh                ;Turn into mask by shifting bits
        shl     dx,cl
        mov     ah,dh                   ;Return mask in ah
        pop     dx
        clc                             ;Show all is fine
        ret

SetCom460:
        mov     ax,IE_ByteSize          ;Show byte size is wrong
        stc                             ;Show error
        ret

SetCom470:
        mov     ax,IE_Default           ;Show something is wrong
        stc                             ;Show error
        ret

SetCom400	endp
page

;----------------------------Private-Routine----------------------------;
;
; TestInt2F - Test For Interrupt 2F
;
; A test is made to see if interrupt 2F can be made.
; This interrupt is used to gain exclusive access to
; a spooled LPT port.  Calls via Int17 are allowed to
; succeed, but calls to the DOS will fail, including
; calls like "Close All Spool Files", which command.com
; will issue every time it displays it's prompt.
;
; Entry:
;	None
; Returns:
;	'C' set if OK to use int 2F
;	'C' clear if we cannot use int 2F
; Error Returns:
;	None
; Registers Destroyed:
;	AX,FLAGS
; History:
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

	assumes ds,Data
	assumes es,nothing

TestInt2F	proc	near

	mov	ax,3000h		;Get DOS version number
	int	21h
	cmp	al,3			;DOS 3.x?
	jne	NoInt2Fs		;  No, disallow int 2Fh
	mov	ax,0B800h		;Test network configuration
	int	2Fh
	or	al,al			;Network not running at all
	jz	NoInt2Fs		;No, disallow int 2Fh
	test	bx,0000000011000100b	;SRV, RCV, or MSG?
	jz	TextInt2FExit		;  No, allow int 2Fh ('C' clear)

NoInt2Fs:
	stc				;'C' set, int 2F not allowed

TextInt2FExit:
	ret

TestInt2F	endp
page

;----------------------------Private-Routine----------------------------;
;
; initialize_delay - Delay After Initializing the ACE
;
; A delay loop based on the BIOS timer is entered while waiting for
; the 8250 to settle.  This use to be done as a loop, but there are
; two problems with that:
;
;   1)	processor speed
;
;   2)	Win386 where the ACE is a virtual device and we'll trap on
;	every I/O operation
;
; To get around this, we will use the BIOS area clock to determine
; when to terminate the loop.
;
; Entry:
;	DX = ACE_RBR
; Returns:
;	None
; Error Returns:
;	None
; Registers Destroyed:
;	AL,CX,FLAGS
; History:
;	Thu 20-Aug-1987 12:24:47 -by-  Walt Moore [waltm]
;	created
;-----------------------------------------------------------------------;

;------------------------------Pseudo-Code------------------------------;
; {
; }
;-----------------------------------------------------------------------;

BIOSTIME	equ	046Ch
TIC_MSEC	equ	55		;A tick is 55 milliseconds
DELAY_TIME	equ	500		;Delay at leasy 500 milliseconds


	assumes ds,nothing
	assumes es,nothing


initialize_delay proc	near

	push	ds
	xor	cx,cx			;Point to the BIOS data area
	mov	ds,cx
	assumes ds,nothing

	mov	ch,((DELAY_TIME+TIC_MSEC-1)/TIC_MSEC)+1

init_delay_outer_loop:
	mov	cl,ds:[BIOSTIME]	;Current timer value

init_delay_loop:
	in	al,dx			;Read it once
	cmp	cl,ds:[BIOSTIME]	;Wait 0-55 milliseconds
	je	init_delay_loop 	;Timer hasn't advanced
	dec	ch			;Waited enough ticks?
	jnz	init_delay_outer_loop	;  No, wait for more ticks

	pop	ds
	assumes ds,nothing

	ret

initialize_delay endp


        include ibmcom1.asm

ifdef DEBUG
	public	InitCom10
	public	InitCom20
	public	InitCom30
	public	InitCom35
	public	InitCom37
	public	InitCom40
	public	InitCom60
	public	InitCom70
	public	InitCom80
	public	InitCom85
	public	InitCom90
	public	InitCom95
	public	InitCom100
	public	SendImm10
	public	SendImm20
	public	SendImm30
	public	SendImm40
	public	SendCom10
	public	SendCom20
	public	SendCom30
	public	SendCom40
	public	SendCom50
	public	SendCom60
	public	TermCom10
	public	TermCom12
	public	TermCom16
	public	TermCom20
	public	TermCom30
	public	TermCom60
	public	Terminate5
	public	Terminate10
	public	Terminate20
	public	Terminate30
	public	Terminate40
	public	Terminate45
	public	Terminate50
	public	MSRRestart
	public	MSRWait10
	public	MSRWait20
	public	MSRWait30
	public	MSRWait40
	public	MSRWait50
	public	MSRWait60
	public	MSRWait70
	public	MSRWait80
	public	MSRWait90
	public	SetCom5
	public	SetCom10
	public	SetCom20
	public	SetCom210
	public	SetCom220
	public	SetCom230
	public	SetCom240
	public	SetCom310
	public	SetCom410
	public	SetCom420
	public	SetCom430
	public	SetCom440
	public	SetCom450
	public	SetCom460
	public	SetCom470
	public	NoInt2Fs
	public	TextInt2FExit
	public	initialize_delay
	public	init_delay_outer_loop
	public	init_delay_loop
endif

sEnd    code
End
