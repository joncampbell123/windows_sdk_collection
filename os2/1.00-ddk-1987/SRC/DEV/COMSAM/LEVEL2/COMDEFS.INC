;	SCCSID = @(#)comdefs.inc	7.2 86/08/22
;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;

IFDEF COMDEBUG
  DEBUG = 1
ELSE
  DEBUG = 0
ENDIF

;**     IODelay - delay long enough for the bus to catch up to the cpu
IODELAY MACRO
        JMP     SHORT $+2
ENDM

	INCLUDE DOSMAC.INC
	INCLUDE DEVHLP.INC
	INCLUDE DEVSYM.INC
	INCLUDE ERROR.INC
	INCLUDE COM.INC 	;local

PORTDATA STRUC
	PD_PORT DW	(?)	;IO base address
	PD_IRQ	DB	(?)	;Interrupt vector
	PD_IMR	DB	(?)	;Interrupt mask
PORTDATA ENDS

;
;	for DevHlp_GetDOSVar call
;
DOSVAR_CURPID		= 2		;LocINFOseg (1st word is CurPid)

CR			= 0dh		;Carriage Return
LF			= 0ah		;Line Feed
XONEQU			= 11h		;DC1 ^Q
XOFFEQU 		= 13h		;DC3 ^S

TICKTIME		= 31		; # milliseconds elapsed per timertick.

WO	EQU	WORD PTR
BY	EQU	BYTE PTR

FAR_POINTER STRUC
	_OFF	DW	(?)	;Offset of far pointer
	_SEG	DW	(?)	;Segment of far pointer
FAR_POINTER ENDS

PHYSADDR STRUC
	_LO	DW	(?)	;Low word of a 32-bit pointer
	_HI	DW	(?)	;High word of a 32-bit pointer
PHYSADDR ENDS

; -----------------------------------------------------------------------------
;
;	ComInfo.ci_comerr definitions
;
CE_RXOVER	= 0001H 	; Receive queue overflow
CE_OVERRUN	= 0002H 	; Receive overrun error
CE_RXPARITY	= 0004H 	; Receive Parity error
CE_FRAME	= 0008H 	; Receive framing error
CE_BREAK	= 0010H 	; Never told them about this! (?)
CE_CTSTO	= 0020H 	; CTS Timeout
CE_DSRTO	= 0040H 	; DSR Timeout
CE_RLSDTO	= 0080H 	; RLSD Timeout
CE_TXFULL	= 0100H 	; TX Queue is full
CE_PTO		= 0200H 	; LPTx Timeout
CE_IOE		= 0400H 	; LPTx I/O Error
CE_DNS		= 0800H 	; LPTx Device not selected
CE_OOP		= 1000H 	; LPTx Out-of-pag\per
;		= 2000H 	;
;		= 4000H 	;
CE_MODE 	= 8000H 	; Requested mode unsupported.

; -----------------------------------------------------------------------------
;
;	ComInfo.ci_bflags definitions
;
CB_QIN		= 01h		;Blocked on empty input queue
CB_QOUT 	= 02h		;Blocked on full out queue (see Timeout, below)
CB_BREAK	= 04h		;Blocked on transmit break timeout
CB_TXIMM	= 08h		;Blocked on tx immediate storage cell
CB_OFLUSH	= 10h		;Blocked on non-empty output queue
CB_CLEAR	= 20h		;Blocked on transmit clear timeout
;CB_QOUT_TIMEOUT = 60000	; Block on full output queue for only 1 minute.

;Default baud rate must be in the range of:
;
;  110, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600  !!! ONLY !!!

BAUD_110	= 110
BAUD_300	= 300
BAUD_600	= 600
BAUD_1200	= 1200
BAUD_2400	= 2400
BAUD_4800	= 4800
BAUD_9600	= 9600
BAUD_19200	= 19200
BAUD_38400	= 38400
BAUD_57600	= 57600
BAUD_DFLT	= BAUD_9600

EOFCHAR_DFLT	= 01Ah		; ^Z = default EOF character.

;Queue sizes should be even
QI_SIZE = 1024			; input  queue size
QO_SIZE = 128			; output queue size
MINQFREE= 10			; minimum free space in queue

; -----------------------------------------------------------------------------
;
;	Comm port information structure
;
ComInfo struc
	CI_QINP  	DW	(?)		; offset of Input  Queue
	CI_QOUTP	DW	(?)		; offset of Output Queue

	CI_PPORT   	DW	(?)		; offset  of port address (and presence check)
	CI_PPORT2  	DW	COM_SEG		; segment "" (40:)
	CI_PORT    	DW	0		; I/O port for this guy, 0 if non-existant

	CI_IRQ	   	DB	(?)		; Interrupt number
	CI_IMR	   	DB	(?)		; Interrupt mask register value
	CI_NOPENS  	DB	0		; Count of outstanding opens
	CI_BFLAGS  	DB	0		; Block flags

	CI_ISR	   	DW	0		; Interrupt service routine address
	CI_PID	   	DW	0		; Pid of owner (0=free)

	CI_PECHAR   	DB	0		; Parity error replacement char
	CI_EOFCHAR  	DB	EOFCHAR_DFLT	; EOF replacement character.

	CI_COMERR     	DW 	0		; Non-zero if I/O error.
ComInfo ends

; -----------------------------------------------------------------------------
;
; Local Data Area
;   LocalData structure is allocated on the stack and intialized by the
;   Strategy routine for each device driver instance.
;
LOCALDATA STRUC
    LD_PTRSAVE 	DD   (?)		; Far pointer to the request header
LOCALDATA ENDS
