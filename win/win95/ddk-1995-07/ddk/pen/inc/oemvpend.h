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

//;****************************************************************************
//;
//; OEMVPEND.H
//;
//; This file contains virtual pen device hardware specific information
//;
//; that gets stuffed into the PENINFO structure at enable time.
//;
//;****************************************************************************


#define BORDER_DIST	250

#define CX_RAW_WIDTH		8300
#define CY_RAW_HEIGHT		6240

#define CX_RAW_WIDTH_510	(9130-(BORDER_DIST*2))
#define CY_RAW_HEIGHT_510	(5910-(BORDER_DIST*2))

#define W_DISTINCT_WIDTH		(CX_RAW_WIDTH/2)	//500 pts/in
#define W_DISTINCT_HEIGHT		(CY_RAW_HEIGHT/2)	//500 pts/in
#define W_DISTINCT_WIDTH_510	(CX_RAW_WIDTH_510/2)
#define W_DISTINCT_HEIGHT_510	(CY_RAW_HEIGHT_510/2)
#define W_OFFSET_X_510			(-BORDER_DIST/2)
#define W_OFFSET_Y_510			(-BORDER_DIST/2)

#define N_SAMPLING_RATE			(200/3)
#define N_SAMPLING_DIST			0

#define C_PENS			1

#define L_PDC_LOWORD_510	PDC_PROXIMITY | PDC_RANGE | PDC_BARREL1
#define L_PDC_LOWORD	PDC_INTEGRATED | PDC_PROXIMITY | PDC_RANGE | PDC_BARREL1
#define L_PDC_HIWORD	0

#define WACOM_PRESSURE_MAX		70
#define WACOM_PRESSURE_DISTINCT	WACOM_PRESSURE_MAX

#define SS_BORDER_DIST		250
#define SS_RAW_WIDTH		(17750-(SS_BORDER_DIST*4))
#define SS_RAW_HEIGHT		(11500-(SS_BORDER_DIST*4))
#define SS_DISTINCT_WIDTH	(SS_RAW_WIDTH/4)	// 500 lpi
#define SS_DISTINCT_HEIGHT	(SS_RAW_HEIGHT/4)	// 500 lpi

#define SS_OFFSET_X 		(-SS_BORDER_DIST/2)
#define SS_OFFSET_Y			(-SS_BORDER_DIST/2)


// Prototype list for OEM routines
DWORD OEM_API(DWORD dwMsg, DWORD lParam1, DWORD lParam2);


#ifdef CPQ

//***********************************************************************
//   COMPAQ CONCERTO EQUATES AND MACROS					*
//                                                                      *
//   Copyright (C) 1991-95 by Microsoft Inc.                            *
//   Copyright (C) 1992 by Compaq Computer Corp. Inc.                   *
//                                                                      *
//***********************************************************************

#define	CPQ_BORDER_DIST		250

#define	CPQ_CX_RAW_WIDTH	7560
#define	CPQ_CY_RAW_HEIGHT	5670

#define	CPQ_W_DISTINCT_WIDTH	3780
#define	CPQ_W_DISTINCT_HEIGHT	2835

#define	CPQ_N_SAMPLING_RATE	100
#define	CPQ_N_SAMPLING_DIST	0

#define	CPQ_N_POINTS_PER_INCH	500

#define	CPQ_C_PENS		1

#define	CPQ_L_PDC_LOWORD	PDC_INTEGRATED+PDC_PROXIMITY+PDC_RANGE+PDC_BARREL1
#define	CPQ_L_PDC_HIWORD	0

//
// Register definitions for the SPRATLY asic
//
#define	DATAREG		0x2465		// read/write data register
#define	INDEXADDRREG	0x2065		// selects index address
#define	MISCOPTIONREG	0x3465		// misc. opt register

//
// MISC options register bit definitions
//
#define	MISC_MASK	0x0E0		// digitizer bits in misc opt regis
#define	FIFO_ENABLE	0x20		// digitizer FIFO enable bit

//
// Packet size register bit definitions
//
#define	FIFO_EMPTY	0x80		// set when FIFO is empty
//
// Index address definitions for SPRATLY
//
#define	FIFOADDR	0x9E		// FIFO index address
#define	PACKSIZEREGADDR	0x9D		// packet size register index address
#define	MBOXSTATADDR	0x0A8		// sec. MBOX full/empty reg addr
#define	SECMBOXADDR	0x0A1		// sec. MBOX reg addr

//
// Bit definitions for SPRATLY registers
//
#define	ENABLEFIFO 	0x20		// misc opt reg FIFO enable bit
#define	SECMBOXFULL	0x08		// sec. MBOX full bit
#define	FIFOPTRRESET	0x20		// FIFO reset bit in pktsizereg

//
// Digitizer commands
//
// Format of the digitizer mode byte
//         MSB 7   6     5     4     3     2     1   LSB 0
//        -------------------------------------------------
//        |  0  | RES | RES | RES | RES | PRX | INC | MOD | 
//        -------------------------------------------------
// Where
//	MOD - this bit is set when the digitizer is in stream mode (continuous
//	      data) and is reset when the digitizer is in track mode 
//	      (only sends data when the tip switch or barrel button is
//	      pressed).
//	INC - This bit is set to indicate incremental mode (only sends data 
//	      when the pen has moved a specified distance).
//	PRX - This bit is set to indicate the digitizer should only send
//	      data when the pen is in proximity.
//
// We set up the digitizer for stream mode, and not incremental 
// or proximity modes.
//
#define	RESET_CMD	0x80		// resets the digitizer FW.
#define	MODE_CMD	0x81		// selects mode (mode is next cmd byte)
#define	XINCR_CMD	0x82		// sets x incr (incr is next byte sent)
#define	YINCR_CMD	0x83		// sets y incr (incr is next byte sent)
#define	GETMODE_CMD	0x84		// Gets the current digitizer mode

#define	STREAM_MOD	0x1		// selects stream mode (continuous pts)
#define	INCR_MOD	0x2		// selects incr mode 
#define	PROX_MOD	0x4		// selects prox mode 

#define	EXTCMOS_INDEX	0x74		// ECMOS index register
#define	EXTCMOS_DATA	0x76		// ECMOS data register

#define	SMI_SWITCH_ORIENT	0x1E	// SMI function to change orientation
#define	SMI_PORT		0x10	// Port to generate software SMI

#define	ORIENT_BYTE	0x19		// ECMOS orientation byte
#define	IRQ_BYTE	0x19		// ECMOS pen IRQ byte

#define	DEFAULT_ORIENT	0		// Restore orientation upon exit

#define	ORIENT_SHIFT	0		// shift to get orientation in low bits
#define	IRQ_SHIFT	2		// shift to get irq in low bits

#define	ORIENT_MASK	3		// mask to isolate orientation bits
#define	IRQ_MASK	0x0C		// mask to isolate irq bits

#define	CHKSUM_START	0x10		// first byte of checksummed area
#define	CHKSUM_LOW	0x5F		// low order byte of checksum
#define	CHKSUM_HIGH	0x5E		// high order byte of checksum

#define	MBOX_WAIT_COUNT	1000		// Number of times to check MBOX ready
#define	MS_WAIT_COUNT	100		// wait loop counter in enable.asm

#define	USEC_COUNT	139		// number of reads from port 84h
					// to equal 100 usec.
//************************************************************************
//
// Format of COMPAQ data packet:
//
//   Byte  MSB 7   6     5     4     3     2     1   LSB 0
//        -------------------------------------------------
//    0   | PR  | CMD | RES | RES | RES | RES | SW1 | SW0 | 
//        -------------------------------------------------
//    1   | X15 | X14 | X13 | X12 | X11 | X10 | X9  | X8  |
//        -------------------------------------------------
//    2   | X7  | X6  | X5  | X4  | X3  | X2  | X1  | X0  |
//        -------------------------------------------------
//    3   | Y15 | Y14 | Y13 | Y12 | Y11 | Y10 | Y9  | Y8  |
//        -------------------------------------------------
//    4   | Y7  | Y6  | Y5  | Y4  | Y3  | Y2  | Y1  | Y0  |
//        -------------------------------------------------
//
//	SW0 = Tip switch status, 0 = not pressed.
//	SW1 = Barrel switch status, 0 = not pressed.
//	CMD = set when response to command is in the FIFO
//	PR = Proximity, 1 = in proximity, 0 = out of proximity.
//************************************************************************

//
// The following equates are used by the interrupt routines in
// concerto.c.
//
#define	CPQPEN_PACKET_SIZE	5
#define	CPQPEN_IN_RANGE		0x80		// Proximity bit.
#define	CPQ_TIP_DOWN		0x1		// Tip switch pressed.
#define	CPQ_BARREL_DOWN		0x2		// Barrel switch pressed.
#define	CMD_RESPONSE		0x40		// command response

//
// The following equates are for the system timer and are used in
// the wait routines.
//
#define	LATCH_CNT0	0	// command to latch counter 0
#define	COUNTS_MILLI	1193*2	// number of counts in a millisecond (from ROM)
#define	cnt_0		0x40	// count read register, timer 0
#define	cnt_mode	0x43	// mode cmd register, timer 0

#define CPQ_READ_INDEX_PORT(reg, data) _asm xor eax,eax \
	_asm mov dx,INDEXADDRREG	\
	_asm mov al,reg 		\
	_asm out dx,al			\
	_asm mov dx,DATAREG 		\
	_asm in al,dx			\
	_asm mov data,al

#define CPQ_WRITE_INDEX_PORT(reg, data) _asm xor eax,eax \
	_asm mov dx,INDEXADDRREG	\
	_asm mov al,reg 		\
	_asm out dx,al			\
	_asm mov dx,DATAREG		\
	_asm mov al,data		\
	_asm out dx,al



#endif
