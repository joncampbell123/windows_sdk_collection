;
; Copyright (c) Microsoft Corporation,  1988
;
	PAGE	,132
	TITLE	COM1  - Communications Device Driver for MS OS/2
.286P	 
;============================================================================
;
; COM
;	Basic Communications Device Driver for MS OS/2
;
;	This driver handles the COM1 communications ports on
;	compatiable computers. It is a very basic device driver. It will only
;	support one baud rate and no handshaking and is non-interrupt driven.
;
; Register conventions:
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
	DW	OFFSET MAIN_CODE:COM_EXIT		;1: Media Check (Block Devices Only)
	DW	OFFSET MAIN_CODE:COM_EXIT		;2: Build BPB (Block Devices Only)
	DW	OFFSET MAIN_CODE:COM_EXIT		;3: Reserved (used to be Ioctl Input)
	DW	OFFSET MAIN_CODE:COM_READ		;4: Input (Read)
	DW	OFFSET MAIN_CODE:COM_NDREAD	        ;5: Non-Destructive Input, no wait.
	DW	OFFSET MAIN_CODE:COM_RSTAT		;6: Input Status
	DW	OFFSET MAIN_CODE:COM_RFLUSH	        ;7: Input Flush
	DW	OFFSET MAIN_CODE:COM_WRITE		;8: Output (Write)
	DW	OFFSET MAIN_CODE:COM_WRITE		;9: Output (Write) with verify
	DW	OFFSET MAIN_CODE:COM_WSTAT		;10: Output Status
	DW	OFFSET MAIN_CODE:COM_WFLUSH	        ;11: Output Flush
	DW	OFFSET MAIN_CODE:COM_EXIT		;12: Reserved (used to be Ioctl Output)
	DW	OFFSET MAIN_CODE:COM_OPEN		;13: Device Open
	DW	OFFSET MAIN_CODE:COM_CLOSE		;14: Device Close
	DW	OFFSET MAIN_CODE:COM_EXIT		;15: Removable Media
	DW	OFFSET MAIN_CODE:COM_EXIT		;16: Generic Ioctl
	DW	OFFSET MAIN_CODE:COM_EXIT		;17: Reset Media
	DW	OFFSET MAIN_CODE:COM_EXIT		;18: Get Logical Drive Map
	DW	OFFSET MAIN_CODE:COM_EXIT		;19: Set Logical Drive Map
	DW	OFFSET MAIN_CODE:DEINSTALL 	        ;20: DeInstall

MAXCMD = (($ - DispTab)/2) - 1

;============================================================================
;
; variables -
;
IF DEBUG
	PUBLIC CURPID,DEVHLP,MASKTAB
ENDIF
	EVEN
CURPID		DD	(?)			; Pointer to current process ID from LDT infoseg
DEVHLP		DD	(?)			; Pointer to device driver help routine
MASKTAB 	db	1fh, 3fh, 7fh, 0ffh

PORT0	LABEL WORD
	PORTDATA <PORT1, VEC1, (not MASK1) and 0ffh>
	PORTDATA <PORT2, VEC2, (not MASK2) and 0ffh>
	EVEN
NPORTS = (($ - PORT0)/SIZE PORTDATA)


IF DEBUG
	PUBLIC	COM1
ENDIF
	EVEN
COM1	COMINFO <COM1_OFF>			; COM1 port information

END_OF_DATA 	LABEL	BYTE

MAIN_DATA	ENDS


page
MAIN_CODE	SEGMENT WORD PUBLIC 'CODE'
	 ASSUME CS:MAIN_CODE,DS:MAIN_DATA,ES:NOTHING,SS:NOTHING

;============================================================================
;
; - COM_STRAT - 
;
;	Device Driver strategy entry point.
;	Parameters	- es:bx = Pointer to the request block
;
COM_STRAT	PROC	FAR
	ASSUME	DS:MAIN_DATA
IF	DEBUG
		INT	3
ENDIF
	MOV	SI,OFFSET MAIN_DATA:COM1	; Pointer to COM1 information
	SUB	SP,SIZE LOCALDATA		; reserve space for LocalData,
	MOV	BP,SP				; ...referenced with positive offsets

	MOV	[BP].LD_PTRSAVE._OFF,BX     	; Save the virtual address
	MOV	[BP].LD_PTRSAVE._SEG,ES     	; ...of the Request Packet.
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

	JMP	DISPTAB[BX]	;Go execute command

COM_STRAT	ENDP

page
;============================================================================
;
; - COM_EXIT - All routines return through this path
;
; On transfer to a label within ComExit:
;	ss:bp		   = LocalData structure
;	ss:[bp].ld_ptrsave = Far pointer to the request block
;
;   Notice that the stack doesn't have to be balanced when you get to here.
; You can be down 6 pushes deep, and jump to here, just so long as SS:BP
; points to the LocalData structure.  SP will be set from BP.
;   The procedure PhysToVirt takes advantage of this.

IF DEBUG
	PUBLIC	COM_EXIT,COMEXIT0,COMEXIT1,COMEXIT2,COMEXIT3,COMEXIT4,COMEXIT5
ENDIF

COM_EXIT PROC FAR
    ASSUME DS:MAIN_DATA

COMEXIT0:					;0100h Done, no error
	MOV	AX,STDON			
	JMP	SHORT FINIAL_EXIT

COMEXIT1:					;8103h Done, error = "unknown command"
	MOV	AX,STDON+STERR+ERROR_I24_BAD_COMMAND
	JMP	SHORT FINIAL_EXIT

COMEXIT2:					;8102h Done, error = "device not ready"
	MOV	AX,STDON+STERR+ERROR_I24_NOT_READY
	JMP	SHORT FINIAL_EXIT

COMEXIT3:					;8101h Done, error = "unknown unit"
	MOV	AX,STDON+STERR+ERROR_I24_BAD_UNIT
	JMP	SHORT FINIAL_EXIT

COMEXIT4:					;0300h Done, device busy, no error
	MOV	AX,STDON+STBUI
	JMP	SHORT FINIAL_EXIT

COMEXIT5:					;810Ch Done, error = "general failure"
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

page
;============================================================================
;
COM_READ	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_READ	ENDP

page
;=============================================================================
COM_NDREAD	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_NDREAD	ENDP

page
;============================================================================
COM_RSTAT	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_RSTAT	ENDP

page
;============================================================================
COM_RFLUSH	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_RFLUSH	ENDP

page
;============================================================================
; - COM_WRITE - Output information to the given com device
;
;	Purpose:
;		This routine will output the information that has been passed
;	to it to the com device specified.  If output can't be right away,
;	then we will wait. 
;
;	Input:
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
;	Output:
;		Status byte in the device driver packet set to reflect 
;		what we were able to do.
;
;	Routines Needed:
;
;=============================================================================
IF DEBUG
	PUBLIC	COM_WRITE
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
; -- note --
; 	AT THIS POINT WE WILL WAIT TILL WE CAN OUTPUT THE CHARACTER TO 
;       THE DEVICE.
;
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
; AL = Byte to be output at this point,
; AH = routine in com int to be called.
; DS:SI points to com device structure
;
	MOV	AH, 2			; 
	PUSHF				;
	PUSH	CS
	CALL	COM_INT			; fake the com interrupt.
					;
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

	JMP	COM_EXIT
COM_WRITE	ENDP

page
;=============================================================================
COM_WSTAT	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_WSTAT	ENDP

page
;=============================================================================
COM_WFLUSH	PROC	NEAR
	ASSUME	DS:MAIN_DATA
	JMP	COM_EXIT
COM_WFLUSH	ENDP

page
;============================================================================
;
; COM_OPEN
;	For COM1 assign the requested device to the calling PID.
;	Verify the PID if the device is already open.
;	Count number opens on this device by this process.
;
;	Entry:	 SI	= Pointer to Comm Dev Info Block
;
;============================================================================
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
	JMP	COMEXIT5	; Else, Return(General Failure)

OPEN_SET_UP:
;
; It is a free device so we will take it.
;
	PUSH	BX		; save CurPid

	CALL	SETPARMS	; Set up port paramaters
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
 
	JMP	COMEXIT0	; Done, no error

COM_OPEN 	ENDP


page
;============================================================================
;
; - COM_CLOSE-
;
;	If we own the device and this is the last close, then we release
;	the global vector, and deallocate the device.
;	No errors are possible.
;
;============================================================================
IF DEBUG
	PUBLIC	COM_CLOSE
ENDIF
COM_CLOSE	PROC NEAR
	ASSUME DS:MAIN_DATA

	CMP	[SI].CI_NOPENS,0	;
	JE	CLOSE_OK		; Jump if already closed. 
;
; -------- We might have to wait till the output buffer is empty.
;

	CMP	[SI].CI_NOPENS,1	; 
	JNA	CLOSE_LAST		; we are open many times.
; 
; This device is open more thatn once by the same process
; so, just decrement the number of opens and return
;
	DEC	[SI].CI_NOPENS		;
	JMP	COMEXIT0		; 	

CLOSE_LAST:
	MOV	[SI].CI_PID,0		; Show not allocated anymore (DX=0)
	MOV	[SI].CI_NOPENS,0	; show no more opens

CLOSE_OK:
	JMP	COMEXIT0		;done, no error
COM_CLOSE 	ENDP

page
;============================================================================
;
DEINSTALL	PROC	NEAR
		JMP	COM_EXIT
DEINSTALL	ENDP


page
;============================================================================
;
; ----- DEVICE DRIVER SUPPORT ROUTINES -------
;
;============================================================================

;============================================================================
;
; PhysToVirt convert physical address to a virtual one
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
	CLI
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
;  - COM_INT -
;
;	PURPOSE:
;		This routine will handle a request to perform some type of
;		I/O action.  The action to be taken will be based on 
;		the value in AH.  
;
;	INPUT:
;		DS:SI = pointer to com information.
;		AL = Char to be writen to com
;		AH = 
;			0	= execute LX_INT
;			1	= execute RX_INT
;			2	= execute TX_INT
;			3	= execute MX_INT
;		
;	EXIT: 
;		Char writen to the com device
;
;============================================================================
IF DEBUG
	PUBLIC	SERVICE_TAB
ENDIF

SERVICE_TAB	LABEL	WORD
	DW	LX_INT		; Line status interrupt handler.
	DW	RX_INT		; Receive data interrupt handler.
	DW	TX_INT		; Transmitter empty interrupt handler.
	DW	MX_INT		; Modem status interrrupt handler.

IF DEBUG
	PUBLIC	COM_INT
ENDIF
COM_INT	PROC
	PUSHA

	XOR	BX,BX		; clear out bx
	MOV	BL,AH		; move routine number
	ADD	BX,BX		; make into a word pointer.
	CALL	SERVICE_TAB[BX] ; call the routine to handle the I/O.

	POPA

	IRET
COM_INT	ENDP

IF DEBUG
	PUBLIC	LX_INT
ENDIF
LX_INT	PROC	NEAR
LX_INT	ENDP

IF DEBUG
	PUBLIC	RX_INT
ENDIF
RX_INT	PROC	NEAR
RX_INT	ENDP

;============================================================================
;  - TX_INT -
;
;	PURPOSE:
;		This routine will take the character in AL and output it to 
;		the serial port (com1).  If it can't be writen right away, then
;		we will wait till the port can accept the char.
;			<---- NOTE ---->
;		this method will lock up the cpu, thus not allowing any other
;		processes to access that port, and maybe slowing down the whole
;		system.
;
;	INPUT:
;		DS:SI = pointer to com information.
;		AL = Char to be writen to com
;		
;	EXIT: 
;		Char writen to the com device
;
;============================================================================

IF DEBUG
	PUBLIC	TX_INT
ENDIF
TX_INT	PROC	NEAR
	MOV	DX,[SI].CI_PORT			; get the base port address.
	ADD	DX,R_LINES			; access the line status reg
;
; We need to find out if the Transmitter holding register is empty before
; we can seed anything.  (Later this will be accomplished via an interrupt.)
	PUSH	AX				; Save AL
WRT_PORT_CHK:
	IODELAY					; let the port recover.
	IN	AL,DX				; ... and read it.
	AND	AL,LS_THRE			; Lets see if the THRE is 
	JZ	WRT_PORT_CHK			; ... empty.
;
; it is ready to accept the char.
;	
	POP	AX
	MOV	DX,[SI].CI_PORT			; get the base port address.
	IODELAY					;
	OUT	DX,AL				; send the char.
	
	RET					;

TX_INT	ENDP

IF DEBUG
	PUBLIC	MX_INT
ENDIF
MX_INT	PROC	NEAR
MX_INT	ENDP

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

	LES	BX, DWORD PTR [SI].CI_PPORT	;get pointer to ROM data (40:0)
	MOV     DX,ES:[BX]			;See if a comm card is there
	MOV	[SI].CI_PORT,DX 		;Set port address
	OR	DX,DX				;see if a comm port is present
	JZ	INIT_PARMS_NO_PORT 		;Nothing to do, if not there.
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

INIT_PARMS_NO_PORT:
		STC				; error.
		RET

INIT_PARMS_PORT_FOUND:
		CLC				; no error.
		RET

INIT_PARMS 	ENDP

;============================================================================
;
; - SETPARMS - set up the comunication parameters of the com port.
;
;	 Entry:     DS:SI -> ComInfo data structure.
;	 Exit:	    Interrupts enabled.
;
; This procedure is called from ComOpen.
;
; Even though SetBaud returns CY if error, this procedure ignores it.
;
;
;============================================================================
IF DEBUG
	PUBLIC	SETPARMS
ENDIF
SETPARMS 	PROC  NEAR
    ASSUME DS:MAIN_DATA
	MOV	DX,[SI].CI_PORT		; which port to work with.

	PUSH	DX
	XOR	AL,AL
	ADD	DX,R_MODMC		; Point at the Modem Control Reg
	OUT	DX,AL			; No interrupts whilst we play with vectors
	POP	DX			; set out2 to 0 to turn interrupts off.

	CLI				; we don't want to be interrupted.

	MOV	DX,[SI].CI_PORT		;Get I/O port again
	CALL	SET_LINEC		; Set Line Control (fixed 8, n, 1)

	MOV	DX,[SI].CI_PORT		;Get I/O port again
	CALL	SET_BAUD		; Set baud rate (fixed 9600 baud) 
	
	MOV	DX,[SI].CI_PORT		;Get I/O port again
;
; Enable the interrupt to the card again.
;
	ADD	 DX,R_MODMC	 	;Modem control register
	MOV	 AL,MC_DFLT		;... use the default
	IODELAY
	OUT	 DX,AL		 	;Enable the Card..
	ADD	 DX,R_INTEN-R_MODMC	;Interrupt enable reg.
;
; Set up the interrupt enable register
;
;	MOV	AL,IE_DFLT		; no interrupts now.

	XOR	AL,AL			; 

	IODELAY				
	OUT	DX,AL			; Enable interrupts on
					; (IE_RX+IE_TX+IE_LX+IE_MX)
	STI
	
	RET
SETPARMS ENDP


;============================================================================
;
; - SET_LINEC - Set up the line control information.
;
;   Sets up comm card framing information, such as byte size, parity, and
; number of stop bits.
;
;	Parameters	- si	= Pointer to COM device info block
;
;============================================================================
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
	MOV	AL, LC_BITS8+LC_STOP1+LC_PNONE	
	IODELAY
	OUT	DX,AL			;And output to COM card to set it.

	RET
SET_LINEC 	ENDP

;============================================================================
;
; - SET_BAUD - Set up the baud rate for the port
;
;   Sets up comm card baud rate
;
;	Parameters	- si	= Pointer to COM device info block
;
;============================================================================
IF DEBUG
	PUBLIC	SET_BAUD
ENDIF
SET_BAUD 	PROC 	NEAR
	ASSUME DS:MAIN_DATA
;
; get the com base port
;
	MOV	DX,[SI].CI_PORT
;
; Set DLAB to 1 first

	ADD	DX,R_LINEC		;Point at the line control reg
	IN	AL,DX			; get the current line control
	OR	AL,LC_DLAB		; set dlab to 1 and output it
	OUT	DX,AL			; ...to the COM card to set it.
;
; Now point to the baud rate divisor latch LS
;
	MOV	DX,[SI].CI_PORT
	ADD	DX,R_BAUDL		;Point at the buad rate ctrl reg (low)
;
; set 9600 baud
;
	XOR	AL,AL
	OR	AL,0CH			;set to 9600 baud (FIXED)
	IODELAY
	OUT	DX,AL			;And output to COM card to set it.
;
; now point to MS divisor latch 
;
	MOV	DX,[SI].CI_PORT
	ADD	DX,R_BAUDH		;Point at the buad rate ctrl reg (low)
;
; set 9600 baud
;
	XOR	AL,AL
	IODELAY
	OUT	DX,AL			;And output to COM card to set it.
;
; Now set DLAB back to 0
;
	MOV	DX,[SI].CI_PORT
;
; Set DLAB FIRST

	ADD	DX,R_LINEC		;Point at the line control reg
;
; set AL 8 data bits, 1 stop , no parity
;	
	IN	AL,DX			; get the current line control reg
	AND	AL,LC_MASK		; set dlab back to 0 and output
	IODELAY
	OUT	DX,AL			; ...to the COM card to set it.

	RET
SET_BAUD	ENDP

END_OF_CODE	LABEL	BYTE

page
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

MAIN_CODE	ENDS
		END
