/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//		 INS8250 ACE Register Offsets And Bit Definitions


#define ACE_RBR		0		//Receiver Buffer
#define ACE_THR		0		//Transmit Holding Register

#define ACE_IER		1		//Interrupt Enable
#define	ACE_ERBFI	 0x01	//Received Data Available
#define	ACE_ETBEI	 0x02	// 00000010b	Transmitter Holding Register Empty
#define	ACE_ELSI	 0x04	// 00000100b	Receiver Line Status
#define	ACE_EDSSI	 0x08	// 00001000b	Modem Status

#define ACE_IIDR	2		//Interrupt Identification
#define	ACE_IIP		 0x01	// 00000001b	Inverted Interrupt Pending (0=int)
#define   ACE_IID	 0x06	// 00000110b	Interrupt ID
#define   ACE_MSI	 0x00	// 00000000b	Modem Status
#define   ACE_THREI  0x02	// 00000010b	Transmitter Holding Register Empty
#define   ACE_RDAI	 0x04	// 00000100b	Received Data Available
#define   ACE_RLSI	 0x06	// 00000110b	Receiver Line Status

#define ACE_LCR 	3		//Line Control
#define   ACE_WLS	0x03	// 00000011b	Word Length Select Bits
#define   ACE_WLS0	0x01	// 00000001b	Word Length Select Bit 0
#define   ACE_WLS1	0x02	// 00000010b	Word Length Select Bit 1
#define   ACE_5BW	0x00	// 00000000b	5 Bit Words
#define   ACE_6BW	0x01	// 00000001b	6 Bit Words
#define   ACE_7BW	0x02	// 00000010b	7 Bit Words
#define   ACE_8BW	0x03	// 00000011b	8 Bit Words
#define   ACE_STB	0x04	// 00000100b	Stop Bits
#define   ACE_1SB	0x00	// 00000000b	1 Stop Bits (1.5 for 5 bit words)
#define   ACE_2SB	0x04	// 00000100b	2 Stop Bits
#define   ACE_PEN	0x08	// 00001000b	Parity Enable
#define   ACE_PSB	0x30	// 00110000b	Parity select bits
#define   ACE_EPS	0x10	// 00010000b	Even Parity Select
#define   ACE_SP	0x20	// 00100000b	Stick Parity
#define   ACE_SB	0x40	// 01000000b	Set Break
#define   ACE_DLAB	0x80	// 10000000b	Divisor Latch Access Bit

#define ACE_MCR 	4		//Modem Control
#define   ACE_DTR	0x01	// 00000001b	Data Terminal ready
#define   ACE_RTS	0x02	// 00000010b	Request To Send
#define   ACE_OUT1	0x04	// 00000100b	Output Line 1
#define   ACE_OUT2	0x08	// 00001000b	Output Line 2
#define   ACE_LOOP	0x10	// 00010000b	Loopback

#define ACE_LSR 	5		//Line Status
#define   ACE_DR	0x01	// 00000001b	Data Ready
#define   ACE_OR	0x02	// 00000010b	Overrun Error
#define   ACE_PE	0x04	// 00000100b	Parity Error
#define   ACE_FE	0x08	// 00001000b	Framing Error
#define   ACE_BI	0x10	// 00010000b	Break Interrupt
#define   ACE_THRE	0x20	// 00100000b	Transmitter Holding Register Empty
#define   ACE_TSRE	0x40	// 01000000b	Transmitter Shift Register Empty

#define ACE_MSR 	6		//Modem Status
#define   ACE_DCTS	0x01	// 00000001b	Delta Clear to Send
#define   ACE_DDSR	0x02	// 00000010b	Delta Data Set Ready
#define   ACE_TERI	0x04	// 00000100b	Trailing Edge Ring Indicator
#define   ACE_DRLSD 0x08	// 00001000b	Delta Receive Line Signal Detect
#define   ACE_CTS	0x10	// 00010000b	Clear To Send
#define   ACE_DSR	0x20	// 00100000b	Data Set ready
#define   ACE_RI	0x40	// 01000000b	Ring Indicator
#define   ACE_RLSD	0x80	// 10000000b	Receive Line Signal Detect

#define ACE_DLL 	0		//LSB Baud Rate Divisor

#define ACE_DLM 	1		//MSB Baud Rate Divisor
