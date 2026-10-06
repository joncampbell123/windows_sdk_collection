;
; Copyright (c) Microsoft Corporation,  1988
;

	PAGE	,132
	TITLE	COM1  - Communications Device Driver for MS OS/2
.286P	 
;============================================================================
;
; -- COM.ASM --
;
;	This driver handles the COM1 communications 
;
;	This is a very basic interrupt driven device driver. It will only
;	support the following:
;
;		*	COM1
;		*	9600 baud, 1 stop bit, no parity, and no handshaking.
;				(can be changed via EQU's in com.inc)
;	
; Register conventions:
;
;	DS:SI and SS:BP are set up by the strategy routine before dispatching
;	to the appropriate handler.
;
;	Each handler must preserve SS:BP and return control to an Exit label,
;	but may trash DS:SI, ES:DI, and even SP.  See ComExit for details.
;
;	Register convention exceptions are noted in the procdure header.
;
;============================================================================

	INCLUDE COMDEFS.INC

MAIN_DATA SEGMENT WORD PUBLIC 'DATA'

	 ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

;============================================================================
;
; Device Headers -
;
;
IF DEBUG
	PUBLIC	COM1DEV
ENDIF

COM1DEV LABEL WORD
	DW	-1,-1				; Sysinit handles the segment part
	DW	DEV_CHAR_DEV OR DEVLEV_1 OR DEV_30
	DW	OFFSET MAIN_CODE:COM_STRAT
	DW	0				; Reserved
	DB	'COM1    '
	DB	8 DUP (?)			; More reserved
;
; End of the device header.
; 
;============================================================================


page
;============================================================================
;
; Dispatch Table -
;	We dispatch based on the requested function code via this table.
;
IF DEBUG
	PUBLIC	DISPTAB
ENDIF
	EVEN
DISPTAB LABEL WORD

	DW	OFFSET MAIN_CODE:COM_INIT		;0: Init
	DW	OFFSET MAIN_CODE:COMEXIT1		;1: Media Check (Block Devices Only)
	DW	OFFSET MAIN_CODE:COMEXIT1		;2: Build BPB (Block Devices Only)
	DW	OFFSET MAIN_CODE:COMEXIT1		;3: Reserved (used to be Ioctl Input)
	DW	OFFSET MAIN_CODE:COM_READ		;4: Input (Read)
	DW	OFFSET MAIN_CODE:COM_NDREAD	;5: Non-Destructive Input, no wait.
	DW	OFFSET MAIN_CODE:COM_RSTAT		;6: Input Status
	DW	OFFSET MAIN_CODE:COM_RFLUSH	;7: Input Flush
	DW	OFFSET MAIN_CODE:COM_WRITE		;8: Output (Write)
	DW	OFFSET MAIN_CODE:COM_WRITE		;9: Output (Write) with verify
	DW	OFFSET MAIN_CODE:COM_WSTAT		;10 (AH): Output Status
	DW	OFFSET MAIN_CODE:COM_WFLUSH	;11 (BH): Output Flush
	DW	OFFSET MAIN_CODE:COM_EXIT		;12 (CH): Reserved (used to be Ioctl Output)
	DW	OFFSET MAIN_CODE:COM_OPEN		;13 (DH): Device Open
	DW	OFFSET MAIN_CODE:COM_CLOSE		;14 (EH): Device Close
	DW	OFFSET MAIN_CODE:COMEXIT1		;15 (FH): Removable Media
	DW	OFFSET MAIN_CODE:COMEXIT1		;16 (10H): Generic Ioctl
	DW	OFFSET MAIN_CODE:COMEXIT1		;17 (11H): Reset Media
	DW	OFFSET MAIN_CODE:COMEXIT1		;18 (12H): Get Logical Drive Map
	DW	OFFSET MAIN_CODE:COMEXIT1		;19 (13H): Set Logical Drive Map
	DW	OFFSET MAIN_CODE:DEINSTALL 	;20 (14H): DeInstall

MAXCMD = (($ - DispTab)/2) - 1

SRVTAB	LABEL	WORD
	DW	OFFSET	MAIN_CODE:MX_INT		; Modem status interrupt
	DW	OFFSET	MAIN_CODE:TX_INT		; Tx holding reg. empty int.
	DW	OFFSET	MAIN_CODE:RX_INT		; Rx data avail int.
	DW	OFFSET	MAIN_CODE:LX_INT		; Line status int.

;============================================================================
;
; VARIABLES -
;
IF DEBUG
	PUBLIC CURPID,DEVHLP
ENDIF
	EVEN
CURPID		DD	(?)			; Pointer to current process ID from LDT infoseg
DEVHLP		DD	(?)			; Pointer to device driver help routine

IF DEBUG
	PUBLIC	COM1,QIN,QOUT
ENDIF
	EVEN
COM1	COMINFO <QIN,QOUT,COM1_OFF>		; COM1 port information
	EVEN
QIN	CHARQUEUE <QI_SIZE>			; Input queue for COM1
	db	QI_SIZE-1 DUP (0)
QOUT	CHARQUEUE <QO_SIZE>			; Output queue for COM1
	db	QO_SIZE-1 DUP (0)

END_OF_DATA 	LABEL	BYTE

PORT0	LABEL WORD
	PORTDATA <PORT1, VEC1, (not MASK1) and 0ffh>
	PORTDATA <PORT2, VEC2, (not MASK2) and 0ffh>
	EVEN

NPORTS = (($ - PORT0)/SIZE PORTDATA)

MAIN_DATA	ENDS


PAGE
MAIN_CODE	SEGMENT WORD PUBLIC 'CODE'
	 ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

;============================================================================
;
; - COM_STRAT - 
;	
;	This routine will dispatch to the proper device driver function
;	based on the request code in the request packet.
;
;	Device Driver strategy entry point.
;	ENTRY:
;		ES:BX ==>  Pointer to the device driver REQUEST block
;
IF DEBUG
	PUBLIC	COM_STRAT
ENDIF
COM_STRAT	PROC	FAR
	ASSUME	DS:MAIN_DATA
IF	DEBUG
		INT	3
ENDIF
	MOV	SI,OFFSET MAIN_DATA:COM1		; Pointer to COM1 information
	SUB	SP,SIZE LOCALDATA		; reserve space for LocalData,
	MOV	BP,SP				; ...referenced with positive offsets

	MOV	[BP].LD_PTRSAVE._OFF,BX     	; Save the virtual address
	MOV	[BP].LD_PTRSAVE._SEG,ES     	; ...of the Request Packet.
IF DEBUG
	PUBLIC	PULL_REQUEST
PULL_REQUEST:
ENDIF
	MOV	AL,ES:[BX].PKTCMD
	CBW

	CMP	AL,MAXCMD
	JA	COMEXIT1			; bad command.

	ADD	AX,AX
	MOV	BX,AX
;
; On entry to each of the command handlers:
;
;	DS		   = Driver data
;	DS:SI		   = COM1 device info block.
;	SS:BP		   = LocalData structure
;	SS:[BP].LD_PTRSAVE = Far pointer to the request block
;
IF DEBUG
	PUBLIC	JMP_REQUEST
JMP_REQUEST:
ENDIF
	JMP	DISPTAB[BX]	;Go execute command

COM_STRAT	ENDP

PAGE
;============================================================================
;
; - COM_EXIT - All routines return through this path
;
; On transfer to a label within ComExit:
;
;	SS:BP		   = LocalData structure
;	SS:[BP].LD_PTRSAVE = Far pointer to the request block
;
;   Notice that the stack doesn't have to be balanced when you get to here.
; You can be down 6 pushes deep, and jump to here, just so long as SS:BP
; points to the LocalData structure.  SP will be set from BP.
;   The procedure PhysToVirt takes advantage of this.
IF DEBUG
	PUBLIC	COM_EXIT,COMEXIT0,COMEXIT1,COMEXIT2
	PUBLIC  COMEXIT3,COMEXIT4,COMEXIT5
ENDIF

COM_EXIT PROC FAR
    ASSUME DS:MAIN_DATA

COMEXIT0:			;0100h Done, no error
	MOV	AX,STDON			
	JMP	SHORT FINIAL_EXIT

COMEXIT1:			;8103h Done, error = "unknown command"
	MOV	AX,STDON+STERR+ERROR_I24_BAD_COMMAND
	JMP	SHORT FINIAL_EXIT

COMEXIT2:			;8102h Done, error = "device not ready"
	MOV	AX,STDON+STERR+ERROR_I24_NOT_READY
	JMP	SHORT FINIAL_EXIT

COMEXIT3:			;8101h Done, error = "unknown unit"
	MOV	AX,STDON+STERR+ERROR_I24_BAD_UNIT
	JMP	SHORT FINIAL_EXIT

COMEXIT4:			;0300h Done, device busy, no error
	MOV	AX,STDON+STBUI
	JMP	SHORT FINIAL_EXIT

COMEXIT5:			;810Ch Done, error = "general failure"
	MOV	AX,STDON+STERR+ERROR_I24_GEN_FAILURE

; 
; All exits come to this point before returning.
;
FINIAL_EXIT:

	LDS	BX,[BP].LD_PTRSAVE
	MOV	[BX].PKTSTATUS,AX		;Mark operation complete

	MOV	SP,BP
	ADD	SP,SIZE LOCALDATA		;And de-allocate local data space

	RET
COM_EXIT ENDP

PAGE
;============================================================================
;
;	- COM_READ -
;		
;	PURPOSE:
;		Read data received by the RS-232 int.
;
;	ENTRY:
;		DS:SI 		===> Com info structure for the device.
;		[BP].LD_PTRSAVE  =   Address of the request block
;		
;		13-BYTE Request header
;		BYTE	Media descriptor (unused by char devices)
;		DWORD	Transfer address
;		WORD	Byte count
;		DWORD	Starting sector number (unused by char devices)
;		WORD	Reserved
;		
COM_READ	PROC	NEAR
IF DEBUG
	PUBLIC	COM_READ
ENDIF
	ASSUME	DS:MAIN_DATA

	LES	BX,[BP].LD_PTRSAVE	; es:bx -> reauest block
	MOV	CX,ES:[BX].IOCOUNT	; cx = how many bytes to read.
	JCXZ	READ_DONE		; all done.
	MOV	BL,1			; indicate no chars read yet.
	
READ_TRY_LOOP:
;-------------------
; Now we can get down to business, and remove a char from the input queue.
;
;	DS:SI ---> COMINFO, DS = dataseg, SS:BP --> local data, BL = mark, 
;	CX = count of chars to read.
;
	PUSH	BX
	MOV	BX,[SI].CI_QINP		; get pointer to queue
	MOV	DL,DEVHLP_QUEUEREAD	; al = queueread (ds:bx = Q)
IF DEBUG
	PUBLIC	PULL_CHAR_Q
PULL_CHAR_Q:
ENDIF
	CALL	[DEVHLP]		;
	POP	BX			;
; 
; AL = char, BL = mark, CX = count, SS:BP --> local data, DS:SI --> cominfo
;
	JNC	READ_PASSED		; we got a char from the input Q.
;
; We could not get a char so, if we are not waiting, wait.
;
	OR	BL,BL			; exit if empty and we we're 
	JZ	READ_DONE		; ...waiting already
	MOV	AL,CB_QIN		;
	PUSH	CX			;
	CALL	PROC_BLOCK		; else wait for a bit (enables ints)
	POP	CX			;
	JNC	READ_DATA_READY		; data is ready; procrun woke us up.
	JNZ	READ_DONE		; interrupted, so return
	
READ_DATA_READY:
	
	MOV	BL,0			; set mark
	JMP	SHORT	READ_TRY_LOOP	; lets try again.
;
;-----------------

IF DEBUG
	PUBLIC	READ_PASSED
ENDIF
READ_PASSED:
	STI
	LES	BX,[BP].LD_PTRSAVE	; ES:BX --> request block
	PUSH	BX
	PUSH	CX			; Save count to read
	PUSH	SI			
	PUSH	ES
	PUSH	DS			; ds:si --> cominfo struc
	PUSH	AX			; AL = char read from the queue.
	MOV	CX,1			; We're only going to write 1 char.
	MOV	AX,ES:[BX].IOPDATA._HI	; Physical xfer address_high.
	MOV	BX,ES:[BX].IOPDATA._LO	; "		address_low.
	MOV	DH,01			; ES:DI (virt) -> xfer data area.
	CALL	PHYS_TO_VIRT		;
					; If this fails, it won't return
	POP	AX
	STOSB				; Store the char at the destination.
	POP	DS			; need to get back our data segment.
	PUSH	DS
	CALL	UN_PHYS_TO_VIRT		; Don't need address anymore.
	STI				; ...location.
	POP	DS
	POP	ES
	POP	SI
	POP	CX
	POP	BX
;
; Inc source pointer in the packet to point to the next location to store
; the read information at..
;	
	ADD	ES:[BX].IOPDATA._LO,1	; 
	ADC	ES:[BX].IOPDATA._HI,0	;
	
	MOV	BL,1			; Try one more time.
	LOOP	READ_TRY_LOOP		;

READ_DONE:

	LDS	BX,[BP].LD_PTRSAVE	; DS:BX --> request block
	SUB	DS:[BX].IOCOUNT,CX	; Sub bytes remaining from request.
	JMP	COMEXIT0		; Done, no error

COM_READ	ENDP

PAGE
;=============================================================================
;
; - COM_NDREAD -
;	
; 	Read a character from the input queue without actualy taking it out.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
COM_NDREAD	PROC	NEAR
IF DEBUG 
	PUBLIC	COM_NDREAD
ENDIF
	ASSUME	DS:MAIN_DATA
	MOV	SI,[SI].CI_QINP			; pointer to input Q.
	CMP	[SI].QCOUNT,0			; check if Q. is empty.
	JE	NDREAD_EMPTY			; Q. is empty.
	ADD	SI,[SI].QCHROUT			; Adjust by char index
	MOV	AL,[SI].QBASE			; Base of the Q.
	LES	BX,[BP].LD_PTRSAVE		; ---> org. request block
	MOV	ES:[BX].NDRBYTE,AL		;
	JMP	COMEXIT0			; "Done, No error"

;
; No char waiting to be read, so set busy bit and return
;
NDREAD_EMPTY:

	JMP	COMEXIT4			; "Done, Device busy, No error"

COM_NDREAD	ENDP

PAGE
;============================================================================
;
;	COM_RSTAT/COM_WSTAT - get input/output status of the device.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
;	USES: everything but bp:
;
IF DEBUG
	PUBLIC COM_RSTAT, COM_WSTAT
ENDIF
COM_RSTAT:
	ASSUME	DS:MAIN_DATA
	MOV	BX,[SI].CI_QINP			; get pointer to input Q.
	XOR	DX,DX				; is the Q. empty
	JMP	SHORT COM_STAT1			
COM_WSTAT:
	ASSUME	DS:MAIN_DATA
	MOV	BX,[SI].CI_QOUTP		; get pointer to output Q.
	MOV	DX,[BX].QSIZE			; is the output Q. full

COM_STAT1:

	CMP	DX,[BX].QCOUNT			; Then return STATUS = ok.
	JNZ	COM_STAT2			;
	JMP	COMEXIT4			; Done, Busy, No error.

COM_STAT2:
	JMP	COMEXIT0			; Done, No error.

PAGE
;============================================================================
;	
;	COM_RFLUSH/COM_WFLUSH	Flush the input and output queue.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
IF DEBUG
	PUBLIC COM_RFLUSH, COM_WFLUSH
ENDIF
COM_RFLUSH:
	ASSUME	DS:MAIN_DATA
	MOV	BX,[SI].CI_QINP			; Point to input Q.
	JMP	SHORT FLUSH_Q			;
COM_WFLUSH:
	ASSUME	DS:MAIN_DATA
	MOV	BX,[SI].CI_QOUTP		; Point to output Q.
FLUSH_Q:
	MOV	DL,DEVHLP_QUEUEFLUSH		;
	CALL	[DEVHLP]			; QFLUSH (DS:BX -> Q.)
	JMP	COMEXIT0

PAGE
;============================================================================
; - COM_WRITE - Output information to the given com device
;
;	Purpose:
;		This routine will output the information that has been passed
;	to it to the com device specified.  If output can't be right away,
;	then we will wait. 
;
;	ENTRY:
;
;	    DS:SI -> Com Info structure for the device.
;	    [BP].ld_ptrsave = address of the request block, which looks like:
;
;		13-BYTE request header
;		BYTE	media descriptor (unused by char devices)
;		DWORD	transfer address
;		WORD	byte count
;		DWORD	starting sector number (unused by char devices)
;		WORD	reserved
;
;	EXIT:
;		Status byte in the device driver packet set to reflect 
;		what we were able to do.
;
IF DEBUG
	PUBLIC COM_WRITE
ENDIF
COM_WRITE	PROC	NEAR
	ASSUME	DS:MAIN_DATA
;		
; Lets find out where we are reading from.
;
	LES	BX,[BP].LD_PTRSAVE	; Get the packet address.
	MOV	CX,ES:[BX].IOCOUNT	; How many do we need to transfer
	JCXZ	COM_WRT_END_OK		; Nothing to write.
;
; loop that will output all the characters to the device.
;
IF DEBUG
	PUBLIC	COM_WRITE_LOOP
ENDIF
COM_WRITE_LOOP:
	PUSH	BX
	PUSH	CX
	PUSH	ES	

	PUSH	SI
	PUSH	DS
	MOV	CX,1			; We only are getting one char at this point.
	MOV	AX,ES:[BX].IOPDATA._HI	; Get the address ( hi 16 bits)
	MOV	BX,ES:[BX].IOPDATA._LO	; Get the address ( low 16 bits)
	MOV	DX,0			; Leave in ds:si 
IF DEBUG
	PUBLIC	GET_VIRT_ADDR
GET_VIRT_ADDR:
ENDIF

	CALL	PHYS_TO_VIRT		; ---> If error, this does not return

	LODSB				; Get the char to be output.
	POP	DS

	PUSH	DS			; need to get back our data seg.
	PUSH	AX
	CALL	UN_PHYS_TO_VIRT		; Don't need address anymore.
	POP	AX
	STI				; Enable interrupts
	POP	DS
	POP	SI
;
; AL = Byte to be output at this point, DS:SI points to com device structure
;
	CALL	SEND_CHAR		; send the char to the output q.

	POP	ES
	POP	CX
	POP	BX

	ADD	ES:[BX].IOPDATA._LO,1	; point to the next char.
	ADC	ES:[BX].IOPDATA._HI,0	;
	
	LOOP	COM_WRITE_LOOP		; Do it till all the data is output.
;
; every thing went O.K. so return done, no error.
;
COM_WRT_END_OK:	

	LDS	BX,[BP].LD_PTRSAVE	; ds:bx --> request block
	SUB	DS:[BX].IOCOUNT,CX	; sub bytes remaining form packet.

	JMP	COMEXIT0
COM_WRITE	ENDP

PAGE
;============================================================================
;
; COM_OPEN
;	For COM1 assign the requested device to the calling PID.
;	Verify the PID if the device is already open.
;	Count number opens on this device by this process.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
IF DEBUG
	PUBLIC	COM_OPEN
ENDIF
COM_OPEN	PROC	NEAR
	ASSUME DS:MAIN_DATA
	CMP	[SI].CI_PORT,0	; Does the com port exist (ci_port != 0)
	JNE	COM_OPEN_EXIST	; ...Yes !!
	JMP	COMEXIT3	; return(No Such Device)
;
; The com port exists, so lets try to open it. 
;
COM_OPEN_EXIST:
	PUSH	DS
	LDS	BX,[CURPID]	; get dos current process id.
	MOV	BX,[BX]		; ...(CURPID)
	POP	DS
	CMP	[SI].CI_PID,BX	; if (CI_PID == CURPID)
	JE	OPEN_OK		; ...then, I already own it.
;
; The device does not belong to me at this time.
; See if it belongs to anybody else
;
	CMP	[SI].CI_NOPENS,0; if (ci_nopens = 0)
	JE	OPEN_SET_UP	; ...OK!

;
; It is opened by another process, so we can't get access to it.
;
	JMP	COMEXIT5	; Else, Return(General Failure)

OPEN_SET_UP:
;
; It is a free device so we will take it.
;
	PUSH	BX		; save CurPid

	CALL	SET_PARMS	; Set up port paramaters
	CALL	SET_QUEUES	; set up input and output q's
	CALL	SET_VEC		; try to set up int vec and clear of 40:0
;	
; If we could not get the needed vectors then we can't open
;
	JNC	OPEN_REC_PID
	JMP	COMEXIT5	; Return(General Failure)

OPEN_REC_PID:
;
; Mark as our's now.
;
	POP	[SI].CI_PID	; CI_PID = CURPID

OPEN_OK:
;
; Record the open
;
	INC	[SI].CI_NOPENS	; Count the open calls
 
;
; This sould be done only on the first open
;
	CMP	[SI].CI_NOPENS,1	; are we open yet ?
	JA	OPEN_FIRST_SKIP		; yse, so don't init.
	XOR	AX,AX			; init.
	MOV	[SI].CI_COMERR,AX	; (no errors)
	MOV	[SI].CI_BFLAGS,AL	; (no processes blocked)
OPEN_FIRST_SKIP:
	JMP	COMEXIT0		; Done, no error

COM_OPEN 	ENDP


PAGE
;============================================================================
;
; - COM_CLOSE-
;
;	If we own the device and this is the last close, then we release
;	the global vector, and deallocate the device.
;
;	** NOTE ** 
;			No errors are possible.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
IF DEBUG
	PUBLIC	COM_CLOSE
ENDIF
COM_CLOSE	PROC NEAR
	ASSUME DS:MAIN_DATA
	XOR	DX,DX
	CMP	[SI].CI_NOPENS,DL	;
	JE	CLOSE_OK		; Jump if already closed. 

CLOSE_Q_CHECK:
	CMP	[SI].CI_NOPENS,1	; while (ci_nopens <=1)
	JA	CLOSE_Q_EMPTY		;	do problock(cb_oflush)
	MOV	DI,[SI].CI_QOUTP		;
	CMP	[DI].QCOUNT,DX		; check for empty q.
	JZ	CLOSE_Q_EMPTY		;
	PUSH	AX
	PUSH	DX
;
; We  wait till the output buffer is empty.
;

	MOV	AL,CB_OFLUSH		; block on output flush.
	CALL	PROC_BLOCK		;
	POP	DX
	POP	AX			
	JNC	CLOSE_Q_CHECK		; q empty, procrun woke us up.
CLOSE_Q_EMPTY:
	STI
	DEC	[SI].CI_NOPENS		;
	JNZ	CLOSE_OK		;
;	
; shut off ints at the 8250
;
	PUSH	AX
	PUSH	DX

	MOV	AX, DX			; AL := 0, AH := 0
	MOV	DX, [SI].CI_PORT
	INC	DX			; --> Interrupt Enable Register
	OUT	DX, AL			; Disable Chip interrupts

	ADD	DL, R_LINES-R_INTEN	; --> Line Status Register
	IODELAY

CLOSE_WAIT:
	IN	AL, DX			; Wait until xmit is empty
	AND	AL, LS_THRE+LS_TSRE
	CMP	AL, LS_THRE+LS_TSRE
	JNE	CLOSE_WAIT
	DEC	DX			; --> Modem Control Register

	MOV	AL, AH			; AL := 0
	OUT	DX, AL			; Clear MCR, dropping DTR and RTS

	CALL	REL_VEC			; Release the interrupt vector
	POP	DX
	POP	AX
	MOV	[SI].CI_PID,DX		; Not allocated (DX=0)

CLOSE_OK:
	MOV	[SI].CI_COMERR, DX	; No more errors - we're closed.(DX=0)
	JMP	COMEXIT0		;done, no error

COM_CLOSE 	ENDP

PAGE
;============================================================================
;
;	Deinstall -
;
;	We do not allow this procedure to be de-installed.
;
;
DEINSTALL	PROC	NEAR
		JMP	COMEXIT1
DEINSTALL	ENDP


PAGE
;============================================================================
;
; ----- DEVICE DRIVER SUPPORT ROUTINES -------
;
;============================================================================

;============================================================================
;
; SET_VEC
;	Set the global interrupt vector to point to our own routine,
;	and unmask the 8259 mask bit.
;	If this fails, someone else already owns the vector.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
;	EXIT:   flags = CARRY SET on error
;
;	Crunches    AX, BX, DX, ES
;	Preserves   SI, BP, DS
IF DEBUG
	PUBLIC	SET_VEC
ENDIF
SET_VEC PROC NEAR
    ASSUME DS:MAIN_DATA
	MOV	AX,[SI].CI_ISR			; DS:AX = our handler
	MOV	BL,[SI].CI_IRQ			; BX = interrupt to be set
	XOR	BH,BH
	MOV	DX,(0000 OR DEVHlP_SETIRQ)      ; dh=0: our routines don't share.
	CALL	[DEVHLP]			; SetIRQ(ax=handler,bx=int,dh=no share)
	JC	SET_VEC90
	CLI					; No error, so we have the port.
	IN	AL,INTA1			; Get the current mask of 8259 Int Mask Reg Addr
	AND	AL,[SI].CI_IMR			; Turn off the appropriate bit
	IODELAY
	OUT	INTA1,AL			; And set up new mask.
	STI

	XOR	AX, AX				; Clear carry flag.
	LES	BX,DWORD PTR [SI].CI_PPORT 	; get pointer to ROM data area (40:0)
	MOV	ES:[BX],AX			; zap it

SET_VEC90:
	RET
SET_VEC ENDP


;============================================================================
;
; REL_VEC
;	Release the global interrupt vector pointing to our own routine.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
;	Crunches    AX, BX, DX, ES
;	Preserves   SI, BP, DS
IF DEBUG
	PUBLIC	REL_VEC
ENDIF
REL_VEC PROC NEAR
	CLI
	IN	AL,INTA1		;Get the current mask of 8259 Int Mask Reg Addr
	MOV	AH,[SI].CI_IMR		;Turn off the appropriate bit
	NOT	AH
	OR	AL,AH
	IODELAY 			;needed????
	OUT	INTA1,AL		;And set up new mask.
	STI

	MOV	BL,[SI].CI_IRQ		;bx = Interrupt to be Removed from
	XOR	BH,BH
	MOV	DL,DEVHLP_UNSETIRQ
	CALL	[DEVHLP]		;UnSetIRQ(bx=int)

	LES	BX,DWORD PTR [SI].CI_PPORT ; get pointer to ROM data area (40:0)
	MOV	AX,[SI].CI_PORT		; get the saved port address
	MOV	ES:[BX],AX		; restore it in RomData
	STI
	RET

REL_VEC ENDP


;============================================================================
;
; -  Phys_To_Virt - convert physical address to a virtual one
;
;   Because DevHlp_PhysToVirt can err, and because it's usually called after
;   some number of pushes, this routine is useful.
;
;   If the DevHlp fails, the function can not continue.  Because at ComExit,
;   only SS:BP need be preserved, this routine works.  Even if the stack is
;   unbalanced at ComExit, it'll correct itself by resetting from BP.
;
;   WARNING: Routine requires SS:BP be unchanged from the Strategy routine
;   WARNING: Procedure can be called at TASK TIME ONLY
;
;   Input:   
;	
;	AX:BX = physical address to convert
;	CX =  size of the segment.
;	DS =  our data segment
;	DH =  0 for DS:SI, 1 for ES:DI
;	SS:BP -> LocalData structure	*** This is important! ***
;
;   Output:  
;	
;	DS:SI =  virtual address, if DH was 0
;	ES:DI =  virtual addrsss, if DH was 1
;	Interrupts  disabled.		<-------- Notice <--------
;
;	Uses	ax bx cx dx (ds:si or es:di)
;
IF DEBUG
    PUBLIC  PHYS_TO_VIRT
ENDIF
PHYS_TO_VIRT  PROC

    ASSUME DS:MAIN_DATA

	PUSH	BP
	MOV	DL, DEVHLP_PHYSTOVIRT
	CLI				; don't allow any int's
	CALL	[DEVHLP]
    ASSUME DS:NOTHING
	POP	BP
	JNC	P2V_NO_ERROR
IF DEBUG
	INT	3			; INT3 - DevHlp_PhysToVirt failed.
ENDIF
	STI				; restore interrupts
	JMP	COMEXIT5		; General Failure
P2V_NO_ERROR:	
	RET
PHYS_TO_VIRT  ENDP

;============================================================================
;
; -  Un_Phys_To_Virt - cancel conversion of physical address to virtual.
;
;   Because Un_DevHlp_PhysToVirt can err, and because it's usually called after
;   some number of pushes, this routine is useful.
;
;   If the DevHlp fails, the function can not continue.  Because at ComExit,
;   only SS:BP need be preserved, this routine works.  Even if the stack is
;   unbalanced at ComExit, it'll correct itself by resetting from BP.
;
;   WARNING: Routine requires SS:BP be unchanged from the Strategy routine
;   WARNING: Procedure can be called at TASK TIME ONLY
;
;   Input:   
;	
;	SS:BP -> LocalData structure	*** This is important! ***
;
;   Output:  
;	
;	'Z'	Cleared if no address mode change.
;	'Z'	Set if address mode change.
;
IF DEBUG
    PUBLIC  UN_PHYS_TO_VIRT
ENDIF
UN_PHYS_TO_VIRT  PROC

    ASSUME DS:MAIN_DATA

	PUSH	BP
	MOV	DL, DEVHLP_UNPHYSTOVIRT
	CLI				; don't allow any int's
	PUSH	ES
	CALL	[DEVHLP]
    ASSUME DS:NOTHING
	POP	ES
	POP	BP
	JNZ	P2UV_NO_ERROR
IF DEBUG
	INT	3			; INT3 - DevHlp_UnPhysToVirt
ENDIF
	STI				; restore interrupts
	JMP	COMEXIT5		; General Failure
P2UV_NO_ERROR:	
	STI				; restore interrupts
	RET
UN_PHYS_TO_VIRT  ENDP


;============================================================================
;  - SEND_CHAR -
;
;	PURPOSE:
;		Send a charater by putting it into the output queue, and
;	by making user that the TX interrupt is actively talking chars out of 
;	the q.  If the device output queue is full, the calling process wil
;	be blocked (paused) util there is room in the queue. 
;
;	NOTE:
;		Any processes that are restarted, are done at interrupt time
;		by the routine RUN_PROC routine.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;		AL = Char to be writen to com
;		
;	EXIT: 
;		CY set if interrupted
;
IF DEBUG
	PUBLIC SEND_CHAR
ENDIF

SEND_CHAR	PROC	NEAR
	ASSUME 	DS:MAIN_DATA
	PUSH	BP
	CLI	

SEND_CHR_START:
	MOV	BX,[SI].CI_QOUTP		; get pointer to the output Q.
	MOV	DL,DEVHLP_QUEUEWRITE		;
	CALL	[DEVHLP]			;
	
ASSUME	DS:MAIN_DATA
	JNC	SEND_CHAR_QUEUED		; jump if q'ed, else full
	PUSH	AX				;
	CALL	KICK_TX				; Make sure TXINT is active
	MOV	AL,CB_QOUT			; Wait for Q. space.
	CALL	PROC_BLOCK			;
	POP	AX				;
	JNC	SEND_CHR_START			;
	JMP	SHORT	SEND_CHAR_END		;

SEND_CHAR_QUEUED:
	CALL	KICK_TX				; Make sure TXINT is active.
	CLC					; Ret with NC, char was Q'ed.

SEND_CHAR_END:
	STI					;
	POP	BP				;
	RET
SEND_CHAR	ENDP
	

;============================================================================
;
;  - Proc_Block -
;
; Block the current task until reactivated by an interrupt routine, interrupted,
; or some timeout.
;
; Use Protect-mode-CS for high word of Block ID address.  We can get this, in
; either mode, from the Device Header at the start of the file.
;
; Use AL+SI for the low word of the Block ID address.
;
; NOTE ----> Has to be called with interrupts off! <----
;
; ENTRY:
;		DS:SI	===>  COM information structure.
;               AL    = Block flag (only one bit set)
;
; EXIT:		Carry flag clear if ProcRun caused wakeup,
;	        Else carry flag set:
;		 	Zero flag clear if interrupted,
;			Zero flag set if timed out.
;
; USES:    AX, BX, CX, DX
;
IF DEBUG
	PUBLIC	PROC_BLOCK
ENDIF
PROC_BLOCK PROC NEAR
    ASSUME DS:MAIN_DATA
	PUSH	SI
	PUSH	DI
	PUSH	BP
	PUSH	DS
	PUSH	ES

	MOV	DX,(0 SHL 8) OR DEVHLP_PROCBLOCK; Yes, it's interruptable.
	MOV	CX, -1				; Assume no timeout
	MOV	DI, CX				

;	CMP	AL,CB_OFLUSH
;	JNZ	BLOCK_NO_DRAIN
;	XOR	DI,DI
;	MOV	CX,100				;Let output queue drain, 100 ms timeout

BLOCK_NO_DRAIN:
;
; Block proc key will equal CS+SI+AL
;
	XOR	AH,AH				; BX = AL + SI
	MOV	BX,AX
	ADD	BX,SI
	CLI	; no interrupts between setting the block flag and
		; actually blocking to avoid race condition where
		; the flag could be cleared before blocking.

	OR	[SI].CI_BFLAGS,AL
	PUSH	BX
	XOR	BX,BX
	MOV	AX,[BX].SDEVPROTCS	 	; From device header at start of DS.
	POP	BX

	CALL	[DEVHLP]			;Block this process (enables ints)

	POP	ES
	POP	DS
	POP	BP
	POP	DI
	POP	SI
	RET
PROC_BLOCK ENDP

;============================================================================
;
; Proc_Run
;
; Call DevHlp's ProcRun on the appropriate address, if we've recorded a process
; waiting at that address.  Use cs:al+si for Block ID address.
;
;  ENTRY:
;		DS:SI	===>  COM information structure.
; 		AL    = Block flag (only one bit set)
; 		SI    = Offset of Comm info block
;
;  RETURNS	Nothing
;  CRUNCHES	Everything!
IF DEBUG
	PUBLIC	PROC_RUN
ENDIF
PROC_RUN PROC NEAR
	TEST	[SI].CI_BFLAGS,AL	;See if a process waits for us
	JZ	NO_BLOCKED_PROC			;Jump if not
	MOV	DL,DEVHLP_PROCRUN
	XOR	AH,AH			; bx = al + si
	MOV	BX,AX
	ADD	BX,SI
	NOT	AL			;Clear block flag
	AND	[SI].CI_BFLAGS,AL
	PUSH	BX
	XOR	BX,BX
	MOV	AX,[BX].SDEVPROTCS	 ; From device header at start of DS.
	POP	BX

	CALL	[DEVHLP]		;ProcRun(ax:bx=block ID)
NO_BLOCKED_PROC:
	RET
PROC_RUN ENDP

;=========================================================================
;
; - SET_QUEUES -
;
;	Initialize both queues for one comm port.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
;	CRUNCHES    BX, DX
;
IF DEBUG
	PUBLIC	SET_QUEUES
ENDIF
SET_QUEUES PROC NEAR
    ASSUME DS:MAIN_DATA
	MOV	BX,[SI].CI_QINP		; get pointer to queue
	MOV	DL,DEVHLP_QUEUEINIT
	CALL	[DEVHLP]

	MOV	BX,[SI].CI_QOUTP	; get pointer to queue
	MOV	DL,DEVHLP_QUEUEINIT
	CALL	[DEVHLP]
	RET
SET_QUEUES ENDP


;============================================================================
;
; - SET_PARMS - set up the comunication parameters of the com port.
;
;	ENTRY:
;		DS:SI	===>  COM information structure.
;
;	EXIT:	    
;		Interrupts enabled.
;
; 	This procedure is called from ComOpen.
;
;	Even though SetBaud returns CY if error, this procedure ignores it.
;
;
;============================================================================
IF DEBUG
	PUBLIC	SET_PARMS
ENDIF
SET_PARMS 	PROC  NEAR
    ASSUME DS:MAIN_DATA
	MOV	DX,[SI].CI_PORT		; which port to work with.

	PUSH	DX
	XOR	AL,AL
	ADD	DX,R_MODMC		; Point at the Modem Control Reg
	OUT	DX,AL			; No interrupts whilst we play with vectors
	POP	DX			; set out2 to 0 to turn interrupts off.

	CLI				; we don't want to be interrupted.

IF DEBUG
	PUBLIC	SETUP_DEVICE
SETUP_DEVICE:
ENDIF
	CALL	SET_LINEC		; Set Line Control (fixed 8, n, 1)
;
; AX will contain the buad rate we desire.
;
	MOV	AX, BAUD_DFLT		; We want to set it to 9600 baud.
	CALL	SET_BAUD		; Set baud rate 
	
	MOV	DX,[SI].CI_PORT		; Get I/O port again
;
; Enable the interrupt to the card again.
;
	ADD	 DX,R_MODMC	 	;Modem control register
	MOV	 AL,MC_DFLT		;... use the default
	IODELAY
	OUT	 DX,AL		 	;Enable the Card..
	ADD	 DX,R_INTEN-R_MODMC	;Interrupt enable reg.
;
; Set up the interrupt enable register for data ready interrupt.
;
	MOV	AL,IE_DFLT		; no interrupts now.
;
; note IE_TX will be turned on by the first write. (kick_tx)
;
	IODELAY				
	OUT	DX,AL			; Enable interrupts on
					; (IE_RX+IE_LX)
	STI
	
	RET
SET_PARMS ENDP

;=========================================================================
;
;	KickTx - Kick Transmitter
;
;	"Kick" the transmitter interrupt routine into operation.
;	If the Transmitter Holding Register isn't empty, then
;	nothing needs to be done.  If it is empty, then the xmit
;	interrupt needs to enabled in the IER.
;
;	Entry:	DS:SI -> ComInfo data structure.
;		INTERRUPTS DISABLED!
;
;	Exit:	None
;
;	USES:   ax,dx,flags
IF DEBUG
    PUBLIC  KICK_TX
ENDIF
KICK_TX	PROC
	MOV	DX,[SI].CI_PORT 	;Get device I/O address
	ADD	DL,R_LINES		;Point at the line status reg
	IN	AL,DX			;And get it
	AND	AL,LS_THRE		;Check transmitter holding reg status
	JZ	KICK_TX_DONE		;Busy, interrupt will hit soon enough

	SUB	DL,R_LINES-R_INTEN	;--> Interrupt enable register
	IODELAY
	IN	AL,DX			;Get current IER state
	TEST	AL,IE_TX		;Tx Interrupt already enabled?
	JNZ	KICK_TX_DONE		;  Yes, don't reenable it
	OR	AL,IE_TX		;  No, enable it
	IODELAY
	OUT	DX,AL

KICK_TX_DONE:
	RET

KICK_TX	ENDP


;============================================================================
;
; - SET_LINEC - Set up the line control information.
;
;   Sets up comm card framing information, such as byte size, parity, and
; number of stop bits.
;
;	Entry:
;		DS:SI -> Com infromation structure.
;
IF DEBUG
	PUBLIC	SET_LINEC
ENDIF
SET_LINEC 	PROC 	NEAR
	ASSUME DS:MAIN_DATA
	MOV	DX,[SI].CI_PORT
	ADD	DX,R_LINEC		;Point at the line control reg
;
; set AL 8 data bits, 1 stop , no parity
;
	MOV	AL, LC_DFLT	
	IODELAY
	OUT	DX,AL			;And output to COM card to set it.

	RET
SET_LINEC 	ENDP

;=========================================================================
;
; -- SET_BAUD --  Set the baudrate for the port.
;
;   ENTRY:
;		AX  =  baudrate to set. (not verified)
;		DS:SI -> Com infromation structure.
;
;   Crunches	 AX, CX, DX
;
IF DEBUG
	PUBLIC	SET_BAUD
ENDIF
SET_BAUD PROC NEAR
    ASSUME DS:MAIN_DATA
;
; Save baud rate for use later.
;
	MOV	CX,AX			; cx holds baud rate
;
; do math to convert user value to serial card readable value
;
	MOV	DX,1
	MOV	AX,0c200h		;dx:ax = 115200. (1c200h)
	DIV	CX			;ax = dx:ax/baud (in cx)
	MOV	CX,AX			;cx = quotient
;
; Now proceed to set the actual baud rate that has been obtained by
; doing the above division.
;
	MOV	DX,[SI].CI_PORT
	ADD	DX,R_LINEC		;Point at the line control reg
	IN	AL,DX			;Get existing copy
	MOV	AH,AL			;Save it for later
	OR	AL,LC_DLAB		;Set DLAB
	IODELAY
	OUT	DX,AL			;Enable Baud Latch
	ADD	DX,R_BAUDH-R_LINEC	;MSB of Baud Latch
	MOV	AL,CH
	IODELAY
	OUT	DX,AL			;Set MSB of Div
	DEC	DX
	MOV	AL,CL
	IODELAY
	OUT	DX,AL			;Set LSB of Div
	ADD	DX,R_LINEC-R_BAUDL	;Back to line control reg
	MOV	AL,AH
	IODELAY
	OUT	DX,AL			;Turn off DLAB
	RET

SET_BAUD ENDP

;=========================================================================
;
; COM_INT
;
; PURPOSE:
;	Interrupt handler for COM1.  This is the communications
;	interrupt service routine for RS232 communications.  When an RS232
;	event occurs the interrupt vectors here.  This routine determines
;	who the caller was and services the appropriate interrupt.  The
;	interrupts are prioritized in the following order:
;		1. line status interrupt
;		2. read data available interrupt
;		3. transmit buffer empty interrupt
;		4. modem service interrupt
;	This routine continues to service until all interrupts have been
;	satisfied.
;
; entry:	none
; exit:		none
; modified:	none
;
IF DEBUG
	PUBLIC	COM_INT
ENDIF
COM_INT	PROC	FAR
	
	ASSUME DS:MAIN_DATA

	PUSH	SI
	PUSH	DS
	PUSH	AX
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	DI
	PUSH	BP
	PUSH	ES
	MOV	SI,OFFSET MAIN_DATA:COM1	; get com1 def.
	MOV	DX,[SI].CI_PORT		;Get Comm card I/O address
	INC	DX
	INC	DX			;Interrupt Id Reg.
IF DEBUG
	PUBLIC	COM_INT_AGAIN
ENDIF
COM_INT_AGAIN:
	IN	AL,DX			;Get Interrupt Id.
	TEST	AL,1			;Interrupt need servicing?
	JNZ	COM_INT_DONE		;Brif not, all done..
	XOR	AH,AH			;Using ax for interrupt dispatch.

	MOV	BX,AX
	PUSH	DX			;Save port
	CALL	SRVTAB[BX]		;Service the Interrupt..
	POP	DX
	JMP	SHORT COM_INT_AGAIN	;Until all interrupts serviced

COM_INT_DONE:
	MOV	AL,EOI		;Send End-of-Interrupt to 8259
	OUT	INTA0,AL

	POP	ES
	POP	BP
	POP	DI
	POP	DX
	POP	CX
	POP	BX
	POP	AX
	POP	DS
	POP	SI
	RET

COM_INT ENDP


;=========================================================================
;	LX_INT	-	line status interrupt service (highest)
;
IF DEBUG
	PUBLIC LX_INT
ENDIF
LX_INT	PROC	NEAR
	ASSUME	DS:MAIN_DATA

	MOV	DX,[SI].CI_PORT		; Set up base port.
	ADD 	DX, R_LINES		; Now point to line status
	IN	AL,DX			;
	AND	AL,0FH			; Mask of non-error info.
	XOR	AH,AH			;
	MOV	[SI].CI_COMERR,AX	; Set as an error
	RET
LX_INT	ENDP

;=========================================================================
;	RX_INT	-	Data available interupt (second)
;
IF DEBUG
	PUBLIC	RX_INT	
ENDIF
RX_INT	PROC	NEAR
	ASSUME	DS:MAIN_DATA
;
; Get the char from the data port.
;
	MOV	DX,[SI].CI_PORT		; get the base port.
	IN	AL,DX			; get the char.

	CMP	[SI].CI_COMERR,0	; Have we received a read error?
	JE	RX_INT_NO_ERR		; ...No, We did not run across an error.
;
; We had a read error, so replace with some normal character.
;
	MOV	AL,04H			; use as a ERROR char. (EOT)
	MOV	[SI].CI_COMERR,0	; Say no more read errors.

RX_INT_NO_ERR:
;
; Now try to place it in the input q.
;
; note-
;	if we can't place it in the q., then we will have input 
;	queue overflow.
;
	MOV	BX,[SI].CI_QINP		; get pinter to the q.
	MOV	DL,DEVHLP_QUEUEWRITE
IF DEBUG
	PUBLIC	INT_PUSH_CHAR_Q
INT_PUSH_CHAR_Q:
ENDIF
	CALL	[DEVHLP]		; do it.
	
;
; Wake up any proc's that are waiting on input.
;
	MOV	AL,CB_QIN		;
	CALL	PROC_RUN		;
	RET
RX_INT	ENDP				;

;============================================================================
;
;	tx holding reg empty interrupt service (third)
;
;	entry:
;		ds:si	--> com info structure
;		dx	=   base com port.
;
IF DEBUG
	PUBLIC	TX_INT
ENDIF
TX_INT	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	MOV	DX,[SI].CI_PORT		; get the base com port.
;
; double check to see if the reg is really empty.
;
	ADD 	DL, R_LINES		; --> line status
	IN	AL,DX			;
	TEST	AL,LS_THRE		; is xmit really empty?
	JNZ	TXINT_CHECK_Q		;
;
; Transmitter not empty, so...
;
	RET

TXINT_CHECK_Q:
;  tx_reg is empty, so....
;
; see if there is a chareter in the transmit queue, and remove it if possible.
;
	MOV	BX,[SI].CI_QOUTP	; Get pointer to queue.
	MOV	CX,[BX].QSIZE		; CX = qsize
	SHR	CX,1			; = qsize/2
	SUB	CX,[BX].QCOUNT		; = qsize/2 - qcount
	MOV	DL,DEVHLP_QUEUEREAD	; Ret in al, ds:bx = q.
IF DEBUG
	PUBLIC	INT_PULL_CHAR_Q
INT_PULL_CHAR_Q:
ENDIF
	CALL	[DEVHLP]
	JNC	TXINT_SEND		; Jump if we got one.
;
; The q is empty: no chars to xmit, so disable the transmit interrupt.
;
	MOV	DX,[SI].CI_PORT		; Get the base port
	INC	DX			; Int enable reg.
	IN	AL,DX			; Get the current value.
	AND	AL, NOT IE_TX		; Turn it off.
	IODELAY				;
	OUT	DX,AL			; Set new interrupt reg.
	
; 
; Wake up proc's that are waiting for output q space.
;
	MOV	AL,CB_OFLUSH		;
	CALL	PROC_RUN		;
	MOV	AL,CB_QOUT		;
	CALL	PROC_RUN		;
	RET				;
;--------------
;
; Finally! transmit the character.

TXINT_SEND:
	MOV	DX,[SI].CI_PORT		; Get the base port.
	IODELAY				;
	OUT 	DX,AL			; Write char to the port.

	CMP	CX,0			; CX = (qsize/2 - qcount)
	JL	TXINT_101		; Jump if Q. > half full
	MOV	AL,CB_QOUT		;
	CALL	PROC_RUN		; Wake proc's waiting for q space.
TXINT_101:
	RET	
TX_INT	ENDP

;============================================================================
; MODEM STATUS INTERRUPT (lowest)
;
IF DEBUG 
	PUBLIC	MX_INT
ENDIF
MX_INT	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	RET
MX_INT	ENDP

END_OF_CODE	LABEL	BYTE
;
; End of the resident code.
;============================================================================

PAGE
;============================================================================
;
; - COMINIT -
;
;	One time intialization code, called at system boot time.
;
; The Init routine will be called to set up the com1 info. It will save
; the DevHlp address, set up the process id address, and init the port.
;
;	Entry:	Local data contains the address of the command packet
;
;	Exit:	Return to the correct exit entry point based on whether
;		we were able to install or not.
;
IF DEBUG
	PUBLIC	COM_INIT
ENDIF
COM_INIT PROC NEAR
	ASSUME	DS:MAIN_DATA
	LES	BX,[BP].LD_PTRSAVE		; ES:BX -> request packet
	MOV	AX,ES:[BX].IOPDATA._SEG 	; Get and save the VIRTUAL DevHlp
	MOV	[DEVHLP]._SEG,AX		; ...address_high.
	MOV	AX,ES:[BX].IOPDATA._OFF 	; Get and save the VIRTUAL DevHlp
	MOV	[DEVHLP]._OFF,AX		; ...address_low.
;
; Save the address of the current process ID variable in the DOS
;
IF DEBUG
    	PUBLIC	SAVE_PID
SAVE_PID:
ENDIF
	MOV	AL,DOSVAR_CURPID		;request pointer to DOS' current pid
	MOV	DL,DEVHLP_GETDOSVAR
	CALL	[DEVHLP]			;get pointer to the current Pid pointer
	JC	INIT_ERROR			;Jump if error
;
; Note that the ptr is BI-MODAL: valid in either protect or real mode.
;
	MOV	ES,AX				; get segment in es
	MOV	AX,ES:[BX]._SEG			; save current proccess id
	MOV	[CURPID]._SEG,AX		; ... for future reference.
	MOV	AX,ES:[BX]._OFF
	MOV	[CURPID]._OFF,AX
;
; Initialize the port based on the data stored in the com1 structure
;
	MOV	BX,OFFSET MAIN_CODE:COM_INT	; hardware interrup handler
	MOV	SI,OFFSET MAIN_DATA:COM1		; Point at the configuration info for COM1
	CALL	INIT_PARMS			; Init com port data for use later
	JC	INIT_ERROR
;
; We get to here after successfully doing all the initialization work during
; the COM1 initialize.
;
	MOV	AX,OFFSET MAIN_CODE:END_OF_CODE
	MOV	CX,OFFSET MAIN_DATA:END_Of_DATA
	LDS	BX,[BP].LD_PTRSAVE	     	; DS:BX -> request block
;
; set the end data in the packet.
;
	MOV	[BX].INITPEND._OFF,AX
	MOV	[BX].INITPEND._SEG,CX

;
; We are done with the INIT so exit.
;
	JMP	COMEXIT0	    		; Done, no error.


; ----------------------
;
; There's been an error.  It's either from or DevHlp_GetDOSVar, or from Parse. 
; We can't install, so to tell MS OS/2 that we:
;
;   - set the number of units to 0
;   - set the ending code segment offset to 0
;   - set the ending data segment offset to 0
;   - set the error bit in the status word
;
; We jump to ComExit5, which sets the error bit in the status word.  
;
INIT_ERROR:

	LDS	BX,[BP].LD_PTRSAVE	    ; DS:BX -> request block
	XOR	AX,AX
	MOV	[BX].INITCUNIT, AL	    ; Set #units to 0.
	MOV	[BX].INITPEND._OFF,AX
	MOV	[BX].INITPEND._SEG,AX
	JMP	COMEXIT5		    ; Done, error = "general failure"

COM_INIT ENDP

;=========================================================================
;
; INIT_PARMS -
;
;   Initialize one port's information structure, but don't set up the hardware.
;   Set the ci_port field to 0 if the port is to be inaccessible.
;
;   Parameters	    - si = Offset of the information structure
;
;=========================================================================
IF DEBUG
    PUBLIC  INIT_PARMS
ENDIF

INIT_PARMS PROC NEAR
    ASSUME DS:MAIN_DATA
	MOV	[SI].CI_ISR,BX			; set the interrupt handler offset.
	LES	BX, DWORD PTR [SI].CI_PPORT	; get pointer to ROM data (40:0)
	MOV     DX,ES:[BX]			; See if a comm card is there
	MOV	[SI].CI_PORT,DX 		; Set port address
	OR	DX,DX				; see if a comm port is present
	JZ	INIT_PARMS_NO_PORT 		; Nothing to do, if not there.
;
; Check to see if the port is a valid com port.
;
	MOV	BX,OFFSET MAIN_DATA:PORT0
	MOV	CX,NPORTS

INIT_PARMS_CHECK_PORT:
	CMP	DX,[BX].PD_PORT
	JZ	INIT_PARMS_PORT_FOUND
	ADD	BX,SIZE PORTDATA
	LOOP	INIT_PARMS_CHECK_PORT
	MOV	[SI].CI_PORT,0			;explicitly zero port value
	JMP	SHORT INIT_PARMS_NO_PORT	;Bad/unknown port address

INIT_PARMS_PORT_FOUND:
	MOV	AL,[BX].PD_IMR		; Set interrupt mask
	MOV	[SI].CI_IMR,AL		;
	MOV	AL,[BX].PD_IRQ		; Set irq level
	MOV	[SI].CI_IRQ,AL		;
	CLC				; no error.
	RET

INIT_PARMS_NO_PORT:
	STC				; error.
	RET

INIT_PARMS 	ENDP


MAIN_CODE	ENDS
		END
