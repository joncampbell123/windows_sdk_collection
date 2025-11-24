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

#ifndef Not_VxD

/* ASM
; This allows other VxDs to call our functions directly
Begin_Service_Table COMMDRVR

COMMDRVR_Service		COMMDRVR_Get_Version,LOCAL

COMMDRVR_Service		__ComApiBreak
COMMDRVR_Service		__ComApiBufOvr
COMMDRVR_Service		__ComApiClear
COMMDRVR_Service		__ComApiClose
COMMDRVR_Service		__ComApiIoctl
COMMDRVR_Service		__ComApiOpen
COMMDRVR_Service		__ComApiPrtclDel
COMMDRVR_Service		__ComApiPrtclIns
COMMDRVR_Service		__ComApiPrtclIoctl
COMMDRVR_Service		__ComApiPurge
COMMDRVR_Service		__ComApiQueryState
COMMDRVR_Service		__ComApiQueryStatus
COMMDRVR_Service		__ComApiRead
COMMDRVR_Service		__ComApiSetState
COMMDRVR_Service		__ComApiWrite

COMMDRVR_Service		__ComAsyncCallBack
COMMDRVR_Service		__ComAsyncGetReadPtr
COMMDRVR_Service		__ComAsyncRead

COMMDRVR_Service		__ComPrtclIoctl
COMMDRVR_Service		__ComPrtclWriteNext

COMMDRVR_Service		__ComRegisterPort
COMMDRVR_Service		__ComRegisterPrtcl

COMMDRVR_Service		__ComApiPort
COMMDRVR_Service		__ComApiQueryInfo

End_Service_Table   COMMDRVR
*/
#endif

// Version numbers
#define		COMM_Major_Ver				0x03
#define		COMM_Minor_Ver				0x0A

#ifndef TRUE
#define	TRUE	1
#endif


/*ASM
iodelay macro		     ;;macro to insure that an instruction
	jmp	$+2	     ;;  fetch occurs between IN and/or OUT
	jmp	$+2	     ;;  instructions on the PC-AT machine
endm
*/

//XLATOFF
#define		IoDelay()		{_asm _emit 0xEB  _asm _emit 0x00  _asm _emit 0xEB  _asm _emit 0x00}
//XLATON


// These are the variable types defined within COMM
// All buffers are BYTE* since void* is impossible to
// increment/decrement

#ifdef COM_INTERNALS
typedef			struct tagCOM_CHNL *	COM_CHNL_HDL;
typedef			struct tagCOM_PRTCL *	COM_PRTCL_INST;
#else
typedef			DWORD					COM_CHNL_HDL;
typedef			DWORD					COM_PRTCL_INST;
#endif

typedef			WORD					COM_ERROR;
typedef			DWORD					COM_FIND_HDL;
typedef			DWORD					COM_MODEM_HDL;
typedef			unsigned			COM_MSG;
typedef			DWORD					COM_PORT_HDL;
typedef			DWORD					COM_PRTCL_HDL;
typedef			unsigned			COM_SIZE;
#ifdef IS_16
typedef			BYTE far *		COM_NAME;
#else
typedef			BYTE *				COM_NAME;
#endif


// These are the constants defined within COMM

#define			COM_ERR_OK					0
#define			COM_ERR_TX_FULL			1				// The buffer has no room - try later
#define			COM_ERR_NOT_MINE		2				// IOCTL doesn't recognize the command

#define			COM_ERR_BREAK				10			// First exit error - break loop
																				// This one MUST remain first exit err

#define			COM_ERR_NO_MEM			11			// Couldn't alloc mem
#define			COM_ERR_NOT_FOUND		12			// ??
#define			COM_ERR_IN_USE			13
#define			COM_ERR_UNKNOWN			14
#define			COM_ERR_BAD_CMD			15
#define			COM_ERR_BAD_LEN			16
#define			COM_ERR_BAD_RATE		17
#define			COM_ERR_BAD_SIZE		18
#define			COM_ERR_BAD_PARITY	19
#define			COM_ERR_BAD_PARAM		20

#define			COM_ERR_RX_FULL			21			// Rx queue overflowed
#define			COM_ERR_RX_OVERRUN	22			// Lost 1+ bytes in port
#define			COM_ERR_RX_PARITY		23			// Parity error
#define			COM_ERR_RX_FRAME		24			// Framing error
#define			COM_ERR_RX_BREAK		25			// Break



// Used by purge, break, clear, ComApiBuf___
#define			COM_READ						0x01
#define			COM_WRITE						0x02


// Used by OnOff - nibbles MUST be aligned
#define			COM_ON_MSG					0x01
#define			COM_ON_RX						0x02
#define			COM_ON_TX						0x04

#define			COM_OFF_MSG					0x10
#define			COM_OFF_RX					0x20
#define			COM_OFF_TX					0x40


#define			COM_MSG_LINE_CHANGE				0x0001
#define			COM_MSG_MODEM_CHANGE			0x0002
#define			COM_MSG_PAUSED						0x0003
#define			COM_MSG_READ_ASYNC				0x0004
#define			COM_MSG_READ_DATA					0x0005
#define			COM_MSG_READ_HIGH					0x0006
#define			COM_MSG_READ_LOW					0x0007
#define			COM_MSG_READ_OVERRUN			0x0008
#define			COM_MSG_READ_TIMEOUT			0x0009
#define			COM_MSG_READ_TIMEOUT_1ST	0x000A
#define			COM_MSG_RESUMED						0x000B
#define			COM_MSG_WRITE_HIGH				0x000C
#define			COM_MSG_WRITE_LOW					0x000D
#define			COM_MSG_WRITE_OVERRUN			0x000E
#define			COM_MSG_WRITE_TIMEOUT			0x000F
#define			COM_MSG_WRITE_TIMEOUT_1ST	0x0010


// time-outs
// compression types
struct tagCOM_DCB {
			WORD						wLen;
			WORD						wVer;
			DWORD						baudPort;									// Speed we talk at
			DWORD						baudRaw;									// speed on the wire
			DWORD						baudComp;									// Top speed w/ full compression
			DWORD						DupSwitch;								// 1/2 duplex - change direction time
			DWORD						ReadBufLen;
			DWORD						WriteBufLen;
			WORD						TxDelay;									// 0 == no delay (milliseconds)
			BYTE						ByteSize;
			BYTE						Parity;
			BYTE						StopBits;
			BYTE						fParity:1;								// Parity checking?
			BYTE						fError:1;									// Does error-free transmission
			BYTE						fCompress:1;							// Does compression
			};
typedef struct tagCOM_DCB COM_DCB;


// Port type
struct tagCOM_INFO {
			WORD						wLen;
			WORD						wVer;
			DWORD						maxBaudPort;							// Speed we talk at
			DWORD						maxBaudRaw;								// speed on the wire
			DWORD						maxBaudComp;							// Top speed w/ full compression
			DWORD						DupSwitch;								// 1/2 duplex - change direction time
			WORD						wNumPorts;
			BYTE						fOpen:1;									// True if the port is open
			};
typedef struct tagCOM_INFO COM_INFO;


// Port type
struct tagCOM_STAT {
			WORD						wLen;
			WORD						wVer;
			WORD						wInQue;											// in port's queue
			WORD						wOutQue;
			WORD						wInBuf;											// in buffer (includes protocols)
			WORD						wOutBuf;
			BYTE						fCd:1;
			BYTE						fRlsd:1;
			BYTE						fDsr:1;
			BYTE						fCts:1;
			BYTE						fTxNext:1;									// TxNext pending
			};
typedef struct tagCOM_STAT COM_STAT;


struct tagCOM_FAX {
			int							iDummy;
			};
typedef struct tagCOM_FAX COM_FAX;

#ifdef NEVER
struct tagCOM_IOCTL {
			WORD						wLen;
			WORD						wVer;
			WORD						wCmd;
			WORD						wErr;
#ifdef IS_16
			void far				*pvDataWrite;
			void far				*pvDataRead;
#else
			void						*pvDataWrite;						// Data sent to call (READ/ONLY)
			void						*pvDataRead;						// Data returned from call
#endif
			WORD						uWriteLen;
			WORD						uReadLen;
			};
typedef struct tagCOM_IOCTL COM_IOCTL;
#endif

// List of port entry points
struct tagCOM_PORT_FUNC {
			COM_ERROR				(*fnBreak) (COM_PORT_HDL hPort, WORD wFlags);
			COM_ERROR				(*fnClear) (COM_PORT_HDL hPort, WORD wFlags);
			COM_ERROR				(*fnClose) (COM_PORT_HDL hPort);
			BYTE						*(*fnGetReadPtr) (COM_PORT_HDL hPort, BYTE *pcRead);
			BYTE						*(*fnGetWritePtr) (COM_PORT_HDL hPort);
			COM_ERROR				(*fnIoctl) (COM_PORT_HDL hPort, WORD wCmd, void *pIoctl);
			COM_ERROR				(*fnOnOff) (COM_PORT_HDL hPort, WORD wFlags);
			COM_ERROR				(*fnOpen) (COM_CHNL_HDL h,COM_PORT_HDL *ph,BYTE *pW,unsigned uW,BYTE *pR,unsigned uR);
			COM_ERROR				(*fnPurge) (COM_PORT_HDL hPort, WORD uFlags);
			COM_ERROR				(*fnQueryInfo) (unsigned uNum, COM_INFO *pInfo);
			COM_ERROR				(*fnQueryState) (COM_PORT_HDL hPort, COM_DCB *pDcb);
			COM_ERROR				(*fnQueryStatus) (COM_PORT_HDL hPort, COM_STAT *pStat);
			void						(*fnSetPtr) (COM_PORT_HDL hPort, BYTE *pcWrite, unsigned uWriteLen, BYTE *pcRead, unsigned uReadLen);
			COM_ERROR				(*fnSetState) (COM_PORT_HDL hPort, COM_DCB *pDcb);
			COM_ERROR				(*fnWrite) (COM_PORT_HDL hPort, BYTE *pcBuf, unsigned uLen);
			COM_ERROR				(*fnWriteNext) (COM_PORT_HDL hPort, BYTE chr);
			};
typedef struct tagCOM_PORT_FUNC COM_PORT_FUNC;

// Used by port to register with the COMM driver
struct tagCOM_PORT_REG {
			WORD						wLen;
			WORD						wVer;
			COM_NAME				psName;
			COM_PORT_HDL		hPort;
			COM_PORT_FUNC		Funcs;
			};
typedef struct tagCOM_PORT_REG COM_PORT_REG;

struct tagCOM_MODEM_FUNC {
			WORD						wLen;
			WORD						wVer;
			COM_ERROR				(*fnAnswer) (COM_MODEM_HDL hModem);
			COM_ERROR				(*fnAutoAnswer) (COM_MODEM_HDL hModem);
			COM_ERROR				(*fnDial) (COM_MODEM_HDL hModem, COM_NAME psNumber);
			COM_ERROR				(*fnHangUp) (COM_MODEM_HDL hModem);
			};
typedef struct tagCOM_MODEM_FUNC COM_MODEM_FUNC;

struct tagCOM_PRTCL_PROP {
			WORD						wLen;
			WORD						wVer;
			COM_NAME				psName;
			COM_SIZE				uBufMinLen;
			};
typedef struct tagCOM_PRTCL_PROP COM_PRTCL_PROP;

// Note: fnIoctl (for SOME calls) and fnQuery can be passed a NULL
//			 hPrtcl.  In this case - return for the protocol, not an
//			 instance.
struct tagCOM_PRTCL_FUNC {
			COM_ERROR				(*fnBreak) (COM_PRTCL_HDL hPrtcl, WORD wFlags);
			COM_ERROR				(*fnCallBack) (COM_PRTCL_HDL hPrtcl, WORD wMsg, BYTE *pcBuf, LONG *plParam);
			COM_ERROR				(*fnClear) (COM_PRTCL_HDL hPrtcl, WORD wFlags);
			COM_ERROR				(*fnCtor) (COM_CHNL_HDL hChnl, COM_PRTCL_INST hP, COM_PRTCL_HDL *phPrtcl, BYTE *pWrite, \
													unsigned uWriteLen, BYTE *pRead, unsigned uReadLen);
			COM_ERROR				(*fnDtor) (COM_PRTCL_HDL hPrtcl);
			COM_ERROR				(*fnIoctl) (COM_PRTCL_HDL hPrtcl, WORD wCmd, void *pIoctl);
			COM_ERROR				(*fnPurge) (COM_PRTCL_HDL hPrtcl, WORD wFlags);
			COM_ERROR				(*fnQueryState) (COM_PRTCL_HDL hPrtcl, COM_DCB *pDcb);
			COM_ERROR				(*fnQueryStatus) (COM_PRTCL_HDL hPrtcl, COM_STAT *pStat);
			COM_ERROR				(*fnRead) (COM_PRTCL_HDL hPrtcl, BYTE *pcBuf, unsigned *puLen);
			void						(*fnSetPtr) (COM_PRTCL_HDL hPrtcl, BYTE *pcWrite, unsigned uWriteLen, BYTE *pcRead, unsigned uReadLen);
			COM_ERROR				(*fnSetState) (COM_PRTCL_HDL hPrtcl, COM_DCB *pDcb);
			COM_ERROR				(*fnWrite) (COM_PRTCL_HDL hPrtcl, BYTE *pcBuf, unsigned *puLen);
			COM_ERROR				(*fnWriteNext) (COM_PRTCL_HDL hPrtcl, BYTE *pChr);
			};
typedef struct tagCOM_PRTCL_FUNC COM_PRTCL_FUNC;

// Used by protocol to register with the COMM driver
struct tagCOM_PRTCL_REG {
			WORD						wLen;
			WORD						wVer;
			WORD						wInQueMin;
			WORD						wOutQueMin;
			WORD						wInRes;
			WORD						wOutRes;
			COM_NAME				psName;
			COM_PRTCL_FUNC	Funcs;
			};
typedef struct tagCOM_PRTCL_REG COM_PRTCL_REG;


//XLATOFF

#define	Debug_Out_Service _Debug_Out_Service
#define	Trace_Out_Service _Trace_Out_Service

#ifndef Not_VxD

long Create_Semaphore (long TokenCount);
void Destroy_Semaphore (long Sem);
void Wait_Semaphore (long Sem,long Flags);
void Signal_Semaphore (long Sem);

void *  _cdecl MapFlat (char cSeg, char cOff);

void *  _cdecl VmmHeapAllocate (DWORD nBytes, DWORD uFlags);
void    _cdecl VmmHeapFree (void *pMem, DWORD uFlags);
DWORD   _cdecl VmmHeapGetSize (void *pMem, DWORD uFlags);
void *  _cdecl VmmHeapReAllocate (void *pMem, DWORD nBytes, DWORD uFlags);

DWORD   _cdecl GetCurVmHandle (void);
DWORD   _cdecl GetNextVmHandle (DWORD VmHdl);
DWORD   _cdecl GetSysVmHandle (void);

void	_cdecl VpicdPhysicallyMask (DWORD hIrq);
void	_cdecl VpicdPhysicallyUnmask (DWORD hIrq);

COM_ERROR ComApiBreak (COM_CHNL_HDL hChnl, WORD uFlags);
COM_ERROR ComApiBufOvr (COM_PRTCL_INST hPrtcl, WORD wFlags, BYTE *pLoc, unsigned uDel, BYTE *pIns, unsigned *puLen);
COM_ERROR ComApiClear (COM_CHNL_HDL hChnl, WORD uFlags);
COM_ERROR ComApiClose (COM_CHNL_HDL hChnl);
COM_ERROR ComApiIoctl (COM_CHNL_HDL hChnl, WORD wCmd, void *pIoctl);
COM_ERROR ComApiPrtclDel (COM_CHNL_HDL hChnl, COM_PRTCL_INST hPrtcl);
COM_ERROR ComApiPrtclIns (COM_CHNL_HDL hChnl, COM_PRTCL_INST *phPrtcl, COM_NAME pName, COM_PRTCL_INST hPrtclNext);
COM_ERROR ComApiPrtclIoctl (COM_PRTCL_INST hPrtcl, WORD wCmd, void *pIoctl);
COM_ERROR ComApiPurge (COM_CHNL_HDL hChnl, WORD uFlags);
COM_ERROR ComApiQueryState (COM_CHNL_HDL hChnl, COM_DCB *pDcb);
COM_ERROR ComApiQueryStatus (COM_CHNL_HDL hChnl, COM_STAT *pStat);
COM_ERROR ComApiPort (COM_CHNL_HDL hChnl, WORD wFlags);
COM_ERROR ComApiRead (COM_CHNL_HDL hChnl, BYTE *pBuf, unsigned uNum, unsigned *puNum);
COM_ERROR ComApiSetState (COM_CHNL_HDL hChnl, COM_DCB *pDcb);
COM_ERROR ComApiWrite (COM_CHNL_HDL hChnl, BYTE *pBuf, unsigned uNum, unsigned *puNum);

void			ComAsyncCallBack (COM_CHNL_HDL hChnl, WORD wMsg, BYTE *pBuf, LONG lParam);
BYTE			*ComAsyncGetReadPtr (COM_CHNL_HDL hChnl);
void			ComAsyncRead (COM_CHNL_HDL hChnl, BYTE *pcRead);

COM_ERROR ComPrtclIoctl (COM_PRTCL_INST hPrtcl, WORD wCmd, void *pIoctl);
COM_ERROR ComPrtclWriteNext (COM_PRTCL_INST hPrtcl, BYTE chr);

COM_ERROR ComRegisterPort (COM_PORT_REG *pPortReg);
COM_ERROR ComRegisterPrtcl (COM_PRTCL_REG *pPrtclReg);

COM_ERROR ComApiOpen (COM_CHNL_HDL *phChnl, BYTE *psPort, BYTE *psModem, UINT cbInQue, UINT cbOutQue);
COM_ERROR ComApiQueryInfo (COM_NAME *psPort, COM_INFO *pInfo);
#endif

//XLATON
