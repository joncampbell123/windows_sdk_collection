;
; Copyright (c) Microsoft Corporation,  1988
;
;

;=========================================================================
;
;	8250 Comm Chip Port Equates
;	Register offsets:
R_DATA	equ	0	;Rx/Tx data
R_BAUDL equ	R_DATA	;Low byte baud rate divisor
R_BAUDH equ	R_DATA+1;High byte baud rate divisor
R_INTEN equ	1	;Interrupt enable
R_INTID equ	2	;Interrupt identification
R_LINEC equ	3	;Line Control
R_MODMC equ	4	;Modem Control
R_LINES equ	5	;Line Status
R_MODMS equ	6	;Modem Status

;=========================================================================
;	Interrupt enable register definitions:
;
IE_RX	= 01h		;Read data available
IE_TX	= 02h		;Transmit buffer empty
IE_LX	= 04h		;Line status change
IE_MX	= 08h		;Modem status change
IE_DFLT = IE_RX + IE_LX

;=========================================================================
;
;	Interrupt identification register definitions:
II_NONE = 01h		;No interrupts pending
II_MX	= 00h		;Modem status interrupt pending -- ZERO
II_TX	= 02h		;Tx interrupt pending
II_RX	= 04h		;Tx interrupt pending
II_LX	= 06h		;Line status interrupt pending

;=========================================================================
;
;	Line control register definitions:
LC_BITS5  = 00h 	;5 data bits
LC_BITS6  = 01h 	;6 data bits
LC_BITS7  = 02h 	;7 data bits
LC_BITS8  = 03h 	;8 data bits
LC_BMASK  = 03h 	;Data bits mask
LC_STOP1  = 00h 	;1 stop bit
LC_STOP2  = 04h 	;2 stop bits
LC_SMASK  = 04h 	;Stop bits mask
LC_PNONE  = 00h 	;No parity
LC_PODD   = 08h 	;Odd parity
LC_PEVEN  = 18h 	;Even parity
LC_PSTUCK1= 28h 	;Parity bit always set
LC_PSTUCK0= 38h 	;Parity bit always clear
LC_PMASK  = 38h 	;Parity mask
LC_BREAK  = 40h 	;Transmit break
LC_DLAB   = 80h 	;Divisor latch access bit
LC_MASK   = 7fh 	;Documented line control register bits
LC_DFLT	  = LC_BITS8+LC_STOP1+LC_PNONE	; default line control information.

;=========================================================================
;
;	Modem control register definitions:
MC_DTR	 = 01h		;Data terminal ready
MC_RTS	 = 02h		;Request to send
MC_OUT1  = 04h		;Output 1
MC_OUT2  = 08h		;Output 2
MC_LOOP  = 10h		;Loopback mode
MC_DFLT  = MC_DTR + MC_RTS + MC_OUT2

;=========================================================================
;
;	Line status register definitions:
LS_DR	 = 01h		;Data ready
LS_OERR  = 02h		;Overrun error
LS_PERR  = 04h		;Parity error
LS_FERR  = 08h		;Framing error
LS_BI	 = 10h		;Break interrupt
LS_THRE  = 20h		;Tx holding register empty
LS_TSRE  = 40h		;Tx shift register empty

;=========================================================================
;
;	Modem status register definitions:
MS_DCTS  = 01h		;Delta clear to send
MS_DDSR  = 02h		;Delta data set ready
MS_TERI  = 04h		;Trailing edge of ring indicator
MS_DRLSD = 08h		;Delta receiver line signal detect
MS_CTS	 = 10h		;Clear to send
MS_DSR	 = 20h		;Data set ready
MS_RI	 = 40h		;Ring indicator
MS_RLSD  = 80h		;Receiver line signal detect

;=========================================================================
;
;
INTA0	= 20h		; X'20' 8259 Interrupt control port
INTA1	= 21h		; X'21' 8259 Interrupt mask register port
EOI	= 20h		; X'20' 8259 End-of-Interrupt ack
IRQBASE = 8		; master 8259 base vector

COM_SEG = 40h		;ROM data segment for COM ports
COM1_OFF = 0		;COM1 presence indicator (and I/O port address)
PORT1	= 3f8h		; port for COM1
VEC1	= 4		; IRQ for COM1
MASK1	= 10h		; IRQ4 IMR bit

COM2_OFF = 2		;COM2 presence indicator (and I/O port address)
PORT2	= 2f8h		; port for COM2
VEC2	= 3		; IRQ for COM2
MASK2	= 08h		; IRQ3 IMR bit
