;
; Copyright (c) Microsoft Corporation,  1988
;
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


	INCLUDE DEVHLP.INC
	INCLUDE DEVSYM.INC	;local
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

WO	EQU	WORD PTR
BY	EQU	BYTE PTR

LONG_PTR STRUC
	_OFF	DW	(?)	;Offset of far pointer
	_SEG	DW	(?)	;Segment of far pointer
LONG_PTR ENDS

PHYSADDR STRUC
	_LO	DW	(?)	;Low word of a 32-bit pointer
	_HI	DW	(?)	;High word of a 32-bit pointer
PHYSADDR ENDS

;
;	Comm port information structure
;
ComInfo struc
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

	CI_COMERR     	DW 	0		; Non-zero if I/O error.
ComInfo ends
;
; Copyright (c) Microsoft Corporation,  1988
;

;=========================================================================
;
; Local Data Area
;   LocalData structure is allocated on the stack and intialized by the
;   Strategy routine for each device driver instance.
;
LOCALDATA STRUC
    LD_PTRSAVE 	DD   (?)		; Far pointer to the request header
LOCALDATA ENDS
