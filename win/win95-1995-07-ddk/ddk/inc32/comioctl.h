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

/* H2INCSWITCHES -d */


#define		COM_IOCTL_MSG_QUERY_CALLBACK		0
#define		COM_IOCTL_MSG_SET_CALLBACK			1
#define		COM_IOCTL_MSG_QUERY_REGS				2
#define		COM_IOCTL_MSG_SET_REGS					3

#define		COM_IOCTL_MSG_HW_QUERY					4
#define		COM_IOCTL_MSG_HW_SET						5
#define		COM_IOCTL_MSG_X_QUERY						6
#define		COM_IOCTL_MSG_X_SET							7
#define		COM_IOCTL_MSG_X_ON							8
#define		COM_IOCTL_MSG_X_OFF							9

// Private messages start at this #
#define		COM_IOCTL_PRIVATE								0x8000



struct tagCOM_IOCTL_CALLBACK {
			WORD					wMsg;
			DWORD					dwNum;
			DWORD					dwAlt;
			};
typedef struct tagCOM_IOCTL_CALLBACK COM_IOCTL_CALLBACK;


struct tagCOM_IOCTL_REGS {
			BYTE					LineCtrl;
			BYTE					MdmCtrl;
			BYTE					LineStat;
			BYTE					MdmStat;
			BYTE					LineMask;					// Will only change these bits on a set
			BYTE					MdmMask;
			};
typedef struct tagCOM_IOCTL_REGS COM_IOCTL_REGS;


struct tagCOM_IOCTL_X {
			DWORD					XonLim;
			DWORD					XoffLim;
			BYTE					XonChr;
			BYTE					XoffChr;
			BYTE					fOutX:1;
			BYTE					fInX:1;
			BYTE					fXoffHold:1;
			BYTE					fXoffSent:1;
			};
typedef struct tagCOM_IOCTL_X COM_IOCTL_X;


struct tagCOM_IOCTL_HW {
			DWORD					onLim;
			DWORD					offLim;
			WORD					fRtsOn:1;						// Turn these guys ON
			WORD					fDtrOn:1;						// F: turn OFF
			WORD					fRtsRx:1;						// Hi/Lo to start/stop
			WORD					fDtrRx:1;						// the other guy when we Rx
			WORD					fRlsdTx:1;					// Must be Hi for us to Tx
			WORD					fRiTx:1;
			WORD					fDsrTx:1;
			WORD					fCtsTx:1;
			WORD					fRlsdHold:1;				// We are waiting on this guy
			WORD					fRiHold:1;
			WORD					fDsrHold:1;
			WORD					fCtsHold:1;
			};
typedef struct tagCOM_IOCTL_HW COM_IOCTL_HW;
