//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
//	   filename: cdevcom.c
//
//     External Device Communication Functions
//
//---------------------------------------------------------
//
//	NOTES:
//	 1. Code is un-optimized for clarity
//	 2. Minimal error checking.
//	 3. Protocol is tested with SVO-5800 (RS-422) and SVBK-10 (RS-232) machines
//	 4. Timecode reading is supported, but dealing with multiple
//		calling threads is nasty, so be sure you know what you're
//		doing before you change anything.
//	 
//
//---------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <streams.h>
#include <mmsystem.h>

#include "ctimecod.h"
#include "cdevcom.h" 

//----------------------------------------------------------
// structures

// SVO-5800/SVBK-1 Command Structure
typedef struct tagSV_CMD{
	BYTE bCmd1;		// Command byte 1 - lo nybble has data byte
					//  count
	BYTE bCmd2;		// Command byte 2
	BYTE bCmd3;		// Data if required by command, or checksum if none
	BYTE bCmd4;		// Checksum if 1 data byte
	BYTE bByteCnt;	// How many bytes in the command (including checksum)
	int iRespCnt;	// How many bytes in the response
} SV_CMD;

// SVO Commands - this is the command table.  
// ***IT MUST BE IN THE SAME ORDER AS THE COMMAND LIST IN CDEVCOM.H!!!!!
static SV_CMD SvoCmdTbl[] = {
	{0x20,1,0x21,0,3,3},		// Play
	{0x20,0,0x20,0,3,3},		// Stop
	{0x61,0x0c,0x11,0x7e,4,11},	// Request LTC
	{0x41,0x36,1,0x78,4,3},		// Timer Mode Select: LTC
	{0x20,0x20,0x40,0,3,3},   	// Rewind
	{0x20,0x6b,0x8b,0,3,3},		// Freeze on
	{0x20,0x6a,0x8a,0,3,3},		// Freeze off
	{0x20,0x04,0x24,0,3,3},		// Standby off
	{0x20,0x05,0x25,0,3,3},		// Standby on
	{0x00,0x11,0x11,0,3,5},		// Device Type Request
	{0x61,0x20,0x08,0x89,4,11},	// Status Request - 8 bytes
	{0x20,0x0f,0x2f,0,3,3},		// Eject
	{0x00,0x0c,0x0c,0,3,3},		// Local On
	{0x00,0x1d,0x1d,0,3,3},		// Local Off
	{0x20,0x10,0x30,0,3,3},		// Fast Forward
	{0x20,0x02,0x22,0,3,3},		// Record
	{0xFF,0,0,0,0,0}			// Device Clear
	};

// device type request table
typedef struct tagSVO_DEVTYPE{
	BYTE bData1;		// 1st data byte of response
	BYTE bData2;		// 2nd data byte of response
} SVO_DEVTYPE;

static SVO_DEVTYPE SvoDevTbl[] = {
	{0x80, 0x0c},
	{0x81, 0x0c},
	{0x80, 0x08},
	{0x80, 0x08},
	{0x81, 0x08},
	{0x10, 0x2c},
	{0x11, 0x2c},
	{0x10, 0x28},
	{0x11, 0x28},
	{0x10, 0x20},
	{0x11, 0x20},
	{0x10, 0x21},
	{0x11, 0x21},
	{0x20, 0x51}
};

const int DevTypeCnt = sizeof(SvoDevTbl);

const TCHAR *strSvoDevID[] = {
	"EVO-9850",
	"EVO-9850P",
	"EVO-9800",
	"EVO-9800A",
	"EVO-9800P",
	"SVO-9600",
	"SVO-9620",
	"SVP-9000",
	"SVP-9020",
	"SVO-5800",
	"SVO-5800P",
	"SVO-5600",
	"SVO-5600P",
	"UVW-1800",
};
const TCHAR *strUnknownDevID =
	{"Unknown Device"};

// SVBK Command Table
// ***IT MUST BE IN THE SAME ORDER AS THE COMMAND LIST IN CDEVCOM.H!!!!!
static SV_CMD SvbkCmdTbl[] = {
	{0x3A,0,0,0,1,1},		// Play
	{0x3F,0,0,0,1,1},		// Stop
	{0xA0,0,0,0,1,9},		// Request LTC
	{0xFF,0,0,0,0,0},		// Timer Mode Select: LTC
	{0xAC,0,0,0,1,1},   	// Rewind
	{0x4F,0,0,0,1,1},		// Freeze on
	{0x3A,0,0,0,1,1},		// Freeze off
	{0xFF,0,0,0,0,0},		// Standby off
	{0xFF,0,0,0,0,0},		// Standby on
	{0x8F,0,0,0,1,1},		// Device Type Request
	{0xD7,0,0,0,1,5},		// Status Request - 5 bytes
	{0x2A,0,0,0,1,2},		// Eject
	{0xFF,0,0,0,0,0},		// Local On
	{0xFF,0,0,0,0,0},		// Local Off
	{0xAB,0,0,0,1,1},		// Fast Forward
	{0xFA,0xFA,0xCA,0,3,1},	// Record
	{0x56,0,0,0,1,1}		// Device Clear
	};

const TCHAR *strSvbkDevID =
	{"SVBK-1"};
const int SvbkDevType = 0x80;

// which machine's command table are we using?
SV_CMD *pVcrCmdTable = SvbkCmdTbl;
int VcrType;				// 0 for SVO, 1 for SVBK
static TCHAR SvbkBuf[16];	// for multi-byte commands

// for simulation mode
static BYTE SvoAck[] = {		
	0x10,0x01,0x11};
static DWORD SvoAckCount = 3L;
static BYTE SvbkAck[] = {		
	0x0A};
static DWORD SvbkAckCount = 1L;
static TCHAR strDevID[] = "Pretend Machine";

static DWORD SimulatedFramecount = 0L;
static long lNDFTable[] = {1080000L,108000L,18000L,1800L,300L,30L,10L,1L};

// COM port stuff
#define COMM_TIMEOUT_MS = 60;

static TCHAR DevPortCom1[] = "COM1";
static TCHAR DevPortCom2[] = "COM2";
static TCHAR DevPortCom3[] = "COM3";
static TCHAR DevPortCom4[] = "COM4";
static TCHAR buf[20];

// init static members
HANDLE CDevCom::m_hDevPort = 0;			
TCHAR * CDevCom::m_pcDevPort = NULL;
BYTE CDevCom::m_bLastCmd = 0;          
BYTE CDevCom::m_LastMotionCmd = 0;
int CDevCom::m_CurCmdCookie = 1;
int CDevCom::m_LastSentCmdCookie = -1;
DCB CDevCom::m_dcb = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0};
	
int CDevCom::m_PendingResponseCnt = 0;
BOOL CDevCom::m_bSimulateHardware = FALSE;
COMMTIMEOUTS CDevCom::m_ctmoTimeout = {
	0,0,0,0,0};
COMSTAT CDevCom::m_csCommStat = {
	0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,};
// Command Queue stuff to deal with multi-threading
#define COMMANDLISTSIZE 10
#define RESPONSE_SLEEP_TIME 5	// in milliseconds
#define RESPONSE_RETRY_MAX	20	// should never have to wait for more than this
								// numer of sleep times
// Use Generic list template class to build command queue. Use of
// such a queue will allow the communications class to be thread safe.
static CGenericList<COMMANDOBJECT> CommandList("List of Pending Commands");

//intialize an empty array of command objects
COMMANDOBJECT CommandObjects[] = {
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff},
	{-1,-1, 0xffff}
};

// initialize an empty array of response objects - we don't create a linked 
// list of these because we don't expect to every have more that two or three
// buffered responses
RESPONSEOBJECT ResponseObject[] = {
	{ -1, -1, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ -1, -1, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ -1, -1, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
#define MAX_RESPONSE_OBJECTS 3

//---------------------------------------------------------
//
// CDevCom constructor
//
//---------------------------------------------------------
CDevCom::CDevCom(CBaseFilter *pFilter, HRESULT *phr)
{
	m_pFilter = pFilter;
	// need some help with timecode
	m_pTimecode = new CTimecode();
	if (m_pTimecode == NULL) {
        *phr = E_OUTOFMEMORY;
       return;
	}
	*phr = S_OK;
	return;
	
}
//---------------------------------------------------------
//
// Destructor
//
//---------------------------------------------------------
CDevCom::~CDevCom()
{

}

//---------------------------------------------------------
//
//	OpenDevPort 
//
//	Tries to open the requested port.  If port is not available,
//	enters "simulation" mode
//
//---------------------------------------------------------
DWORD 
CDevCom::OpenDevPort( int Port )
{
	TCHAR resp[20];
	DWORD LastError;
	DWORD dwCommError;
	int Cookie;

	timeBeginPeriod(1);	// setup 1ms timer for timestamping
	m_bSimulateHardware = FALSE;	// this may have been set TRUE previously
	// get the port
	switch (Port){
		case DEV_PORT_COM1:
			m_pcDevPort = DevPortCom1;
			break;
		case DEV_PORT_COM2:
			m_pcDevPort = DevPortCom2;
			break;
		case DEV_PORT_COM3:
			m_pcDevPort = DevPortCom3;
			break;
		case DEV_PORT_COM4:
			m_pcDevPort = DevPortCom4;
			break;
		case DEV_PORT_SIM:
		case DEV_PORT_DIAQ:
		case DEV_PORT_ARTI:
		default:
			if (m_hDevPort)
				CloseDevPort();
			m_bSimulateHardware = TRUE;
			return 0L;
	}
	// open the port
	if ( (m_hDevPort = CreateFile( m_pcDevPort,	GENERIC_READ | GENERIC_WRITE,
    				0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
    				NULL )) == (HANDLE) -1 ){
		// we're in simulation mode
		DWORD dwTemp = GetLastError();
		m_bSimulateHardware = TRUE;
		return 1L;
	}

	// get the Device Control Block
	if ( !GetCommState( m_hDevPort, &m_dcb ) ) {
		CloseDevPort();
		m_bSimulateHardware = TRUE;
   		return 1L;
	}

	// put in our values
	m_dcb.BaudRate = 38400;
    m_dcb.fDtrControl = DTR_CONTROL_ENABLE;
    m_dcb.fDsrSensitivity = FALSE;
    m_dcb.fOutX = FALSE;
    m_dcb.fInX = FALSE;
    m_dcb.fNull = FALSE;
    m_dcb.fRtsControl = RTS_CONTROL_ENABLE;
    m_dcb.fAbortOnError = TRUE;
  	m_dcb.ByteSize = 8;
    m_dcb.Parity = ODDPARITY;
    m_dcb.StopBits = ONESTOPBIT;

	if ( !SetCommState(m_hDevPort, &m_dcb) ){
		// failure forces simulation mode
		m_bSimulateHardware = TRUE;
		return 1L;
	}

	// set the comm timeout
	if ( !GetCommTimeouts(m_hDevPort, &m_ctmoTimeout) )	{
		CloseDevPort();
		m_bSimulateHardware = TRUE;
   		return 1L;
	}
	// timeout after COMM_TIMEOUT_MS milliseconds
	m_ctmoTimeout.ReadIntervalTimeout = 15L;
	m_ctmoTimeout.ReadTotalTimeoutMultiplier = 10L;
	m_ctmoTimeout.ReadTotalTimeoutConstant = 60L;
	m_ctmoTimeout.WriteTotalTimeoutMultiplier = 1L;
	m_ctmoTimeout.WriteTotalTimeoutConstant = 60L;
	if ( !SetCommTimeouts(m_hDevPort, &m_ctmoTimeout) )	{	
		CloseDevPort();
		m_bSimulateHardware = TRUE;
   		return 1L;
	}

	// now let's see if there is some hardware there
	// try an SVO type machine first
	pVcrCmdTable = SvoCmdTbl;
	VcrType = 0;
	if (SendDevCmd(DEVICE_TYPE_REQUEST, &Cookie) != 0L)	{	
		GetDevResponse(resp, Cookie);	//cleanup
		CloseDevPort();
		m_bSimulateHardware = TRUE;
		m_PendingResponseCnt = 0;
   		return 1L;
	}

	if ( GetDevResponse(resp, Cookie) != 0L){
		// try the SVBK now
		pVcrCmdTable = SvbkCmdTbl;
		VcrType = 1;
		FlushDevResponseBuffer();	// clean out the UART
		ClearCommError( m_hDevPort, &dwCommError, &m_csCommStat );
		// need to change the baud rate among other things
		m_dcb.BaudRate = 9600;
		m_dcb.Parity = NOPARITY;
    	if ( !SetCommState(m_hDevPort, &m_dcb) ){
			// set simulation mode
			LastError = GetLastError();
			m_bSimulateHardware = TRUE;
			return 1L;
		}
		Sleep(30L);	// give the machine time to recover
		SendDevCmd(DEVICE_CLEAR, &Cookie);
		GetDevResponse(resp, Cookie);
		// DEVICE_CLEAR puts the machine in pause, so put it in stop
		SendDevCmd(STOP, &Cookie);
		GetDevResponse(resp, Cookie);
		Sleep(30L);					// give the machine time to recover
		FlushDevResponseBuffer();	// clean out the UART
		ClearCommError( m_hDevPort, &dwCommError, &m_csCommStat );
		if (SendDevCmd(DEVICE_TYPE_REQUEST, &Cookie) != 0L)	{
			GetDevResponse(resp, Cookie);	//cleanup
			CloseDevPort();
			m_bSimulateHardware = TRUE;
			m_PendingResponseCnt = 0;
   			return 1L;
		}
		// we don't actually check the response value here because
		// it is not always consistant.
		if ( GetDevResponse(resp, Cookie) != 0L){
			CloseDevPort();
			m_bSimulateHardware = TRUE;
			return 1L;
		}
		// cleanup
		ClearCommError( m_hDevPort, &dwCommError, &m_csCommStat );
		FlushDevResponseBuffer();	// clean out the UART
		return 0L;
	}
	 return 0L;
}

//---------------------------------------------------------
//
//	CloseDevPort
//
//	Shuts things down
//
//---------------------------------------------------------
DWORD 
CDevCom::CloseDevPort( void )
{
	DWORD LastError;

	timeEndPeriod(1);	// restore timer period

	if ( m_hDevPort && !m_bSimulateHardware )
	{	// close the port
		if (DeleteFile(m_pcDevPort)) {
			LastError = GetLastError();
			return DEV_COMM_ERR_COMMUNICATION_ERROR;
		}
		m_pcDevPort = NULL;
		if ( !CloseHandle( m_hDevPort ) ) {
			LastError = GetLastError();
			return DEV_COMM_ERR_COMMUNICATION_ERROR;
		}
		m_hDevPort = 0;
		return 0L;
	}
	m_bSimulateHardware = FALSE;
	return 0L;
}

//---------------------------------------------------------
//
//	FlushDevResponseBuffer
//
//	A cleanup method
//
//---------------------------------------------------------
DWORD 
CDevCom::FlushDevResponseBuffer( void )
{
	DWORD LastError;
	DWORD dwRetByteCnt;
	DWORD dwCommError;
	char buf[16];

	if (m_bSimulateHardware)
		return 0L;
	m_PendingResponseCnt = 0;
	if (PurgeComm(m_hDevPort, PURGE_RXCLEAR)) {
		LastError = GetLastError();
		return DEV_COMM_ERR_COMMUNICATION_ERROR;
	}
	ClearCommError( m_hDevPort, &dwCommError, &m_csCommStat );
	if (m_csCommStat.cbInQue) {
		if ( !ReadFile( m_hDevPort, buf, (DWORD)m_csCommStat.cbInQue,
		 (LPDWORD)&dwRetByteCnt, (LPOVERLAPPED)NULL ) ){
			LastError = GetLastError();
			// could stand a bit better error handling here
			return DEV_COMM_ERR_COMMUNICATION_ERROR;
		}
	}
			
	return 0L;
}

//---------------------------------------------------------
//
//	SendDevCmd
//
//	Sends command to device (or queues it up for xmission).
//	Returned a Cookie so the caller can find the
//	correct response
//					
//---------------------------------------------------------
DWORD
CDevCom::SendDevCmd( int Cmd, int *pCookie)
{
	CAutoLock lock(this);
	PCOMMANDOBJECT pCmdObj;
	DWORD dwret;

	// return immediately if command is not supported
	if (pVcrCmdTable[Cmd].bCmd1 == 0xFF)
		return DEV_COMM_ERR_COMMAND_NOT_SUPPORTED;

	// get a new cookie
	if (m_CurCmdCookie++ == 0)
			m_CurCmdCookie++;
	// queue the command if the response has not yet been
	// pulled for the previously sent command
	if (m_PendingResponseCnt){
		// some extra instructions here for debugging
		pCmdObj = AddCommand(Cmd, m_CurCmdCookie);
		if (pCmdObj == NULL){
			ASSERT(pCmdObj);
			return DEV_COMM_ERR_QUEUE_OVERFLOW;
		}
		*pCookie = m_CurCmdCookie;
		return 0L;
	}
	dwret = ReallySendDevCmd( Cmd, m_CurCmdCookie);
	*pCookie = m_CurCmdCookie;
	return dwret;
}

//---------------------------------------------------------
//
//	ReallySendDevCmd
//
//	This does the actual sending of queued commands
//
//---------------------------------------------------------
DWORD
CDevCom::ReallySendDevCmd( int Cmd, int Cookie )
{
	SV_CMD *svCommand;
	BYTE bCmdSequenceCnt;
	DWORD dwBytesWritten;
	DWORD LastError;
	DWORD dwret = 0L;
	TCHAR *pCmd;
	int i;

	m_bLastCmd = Cmd;
	if ( m_isMotionCmd(Cmd) )	// do we still need to know this?
		m_LastMotionCmd = Cmd;
	
	if (!m_bSimulateHardware) {
		// get the command from the table and shoot it out
		svCommand = &pVcrCmdTable[Cmd];
		// we handle the two machine types differently because the SVBK
		// machine might have multiple command/response pair sequences
		if (VcrType == 0) {	// SVO has one sequence and can send multiple bytes
			i = (int)svCommand->bByteCnt;
			bCmdSequenceCnt = 1;
		}
		else {	// SVBK only sends out one byte per sequence
			i = 1;
			bCmdSequenceCnt = svCommand->bByteCnt;
		}
		pCmd = (TCHAR *)svCommand;
		while (bCmdSequenceCnt) {	// do this loop for each sequence
			// send out the command
			if ( !WriteFile( m_hDevPort, (TCHAR *)pCmd, i,
					(LPDWORD)&dwBytesWritten, NULL ) ) {
					LastError = GetLastError();
					dwret = DEV_COMM_ERR_COMMUNICATION_ERROR;
			}
			if (VcrType == 1 && bCmdSequenceCnt > 1) {
				// must pick up response byte if another sequence is pending
				m_PendingResponseCnt++;
				ReallyGetDevResponse(SvbkBuf,1);
				// point to the next command byte in the sequence
				pCmd++;
				// special handling for SVBK's Eject command
				if (svCommand->bCmd1 == 0x2a)
					Sleep(1000);	// wait while EJECT completes
			}
			bCmdSequenceCnt--;
		}
	}
	m_LastSentCmdCookie = Cookie;
	m_PendingResponseCnt++;
	return dwret;
}

//---------------------------------------------------------
//
//	GetDevResponse
//
//	Returns device response to command.  If command
//	hasn't really been sent yet (still in queue), error
//	is returned in the DWORD
//
//---------------------------------------------------------
DWORD 
CDevCom::GetDevResponse( TCHAR *buf, int Cookie )
{
	CAutoLock lock(this);
	
	DWORD dwret = 0L;
	int temp;
	PCOMMANDOBJECT ptemp;
	POSITION posNextCommand;
	BOOL bNeedCommand = FALSE;
	
	// is this command's response buffered?
	if ( IsResponseBuffered(Cookie, &temp) ) {
		GetBufferedResponse( buf, Cookie );
		// don't send another command out since
		// a response wasn't cleared out of the COMM port
		return dwret;
	}

	// was this the last command sent?
	if (Cookie == m_LastSentCmdCookie) {
		// get the response
		dwret = ReallyGetDevResponse( buf, pVcrCmdTable[m_bLastCmd].iRespCnt );
		if (dwret)
			return dwret;
	}
	else {
		// since it wasn't, is a response pending?
		if (m_PendingResponseCnt) {
			dwret = BufferResponse();
			if (dwret)
				return dwret;
		}
		bNeedCommand = TRUE;
	}
	
	// since the command hasn't been sent it must be in the queue, 
	// so send something
	temp = CommandList.GetCount();
	if (temp) {
		posNextCommand = CommandList.GetHeadPosition();
		ptemp = CommandList.Get(posNextCommand);
		dwret = ReallySendDevCmd( ptemp->Command, ptemp->Cookie );
		ptemp = CommandList.RemoveHead();
		RemoveCommand(ptemp);
		return DEV_COMM_ERR_COMMAND_NOT_SENT_YET;
	}
	else 
		if (bNeedCommand)
			return DEV_COMM_ERR_COMMAND_MISSING;

	return dwret;	
}
//---------------------------------------------------------
//
//	ReallyGetDevResponse
//
//	Actually handles communications to get device response
//
//---------------------------------------------------------
DWORD 
CDevCom::ReallyGetDevResponse( TCHAR *buf, int bytecnt )
{
	DWORD dwRetByteCnt; 
	DWORD LastError;

	m_PendingResponseCnt--;
	if (m_bSimulateHardware) {
		(GetSimulatedResponse(buf));
	}
	else {
		if ( !ReadFile( m_hDevPort, buf, (DWORD)bytecnt,
			 (LPDWORD)&dwRetByteCnt, (LPOVERLAPPED)NULL ) ){
			LastError = GetLastError();
			// could stand a bit better error handling here
			return DEV_COMM_ERR_COMMUNICATION_ERROR;
		}
		if ( dwRetByteCnt != (DWORD)bytecnt ) {
			return DEV_COMM_ERR_RESPONSE_MISMATCH;
		}
		// SVBK doesn't use checksums and its timecode reading 
		// is a bit unpredictable
		if (VcrType == 0) {
			if ( TestResponse(buf, dwRetByteCnt) )
				return DEV_COMM_ERR_RESPONSE_CHECKSUM;
		}
		else {
			// sometimes an extra wierd character is sent
			// during timecode reading, so we need to watch for it
			if (bytecnt == pVcrCmdTable[READ_TC].iRespCnt) {
				// this was a timecode response, so check for multiple
				// 0xA1's at the beginning of the buffer.  If we have
				// more than one, we need to get another byte out of
				// the receive buffer
				if (buf[1] == (TCHAR)0xA1) {
					if ( !ReadFile( m_hDevPort, 
						&buf[pVcrCmdTable[READ_TC].iRespCnt],
						1L,	(LPDWORD)&dwRetByteCnt, (LPOVERLAPPED)NULL ) ){
						LastError = GetLastError();
						// could stand a bit better error handling here
							return DEV_COMM_ERR_COMMUNICATION_ERROR;
					}
					if ( dwRetByteCnt != 1L )
						return DEV_COMM_ERR_RESPONSE_MISMATCH;
				}
			}
		}
	}
	return S_OK;
}
//---------------------------------------------------------
//
//	BufferResponse
//
//	Pulls a response out of the COMM device for the most
//	recently send command
//
//---------------------------------------------------------
DWORD 
CDevCom::BufferResponse(void)
{
	int i; 
	// get an available slot in the array
	for (i=0; i<MAX_RESPONSE_OBJECTS; i++) {
		if (ResponseObject[i].Cookie == -1)
			break;
	}
	if (i>=MAX_RESPONSE_OBJECTS)
		return DEV_COMM_ERR_RESPONSE_OVERFLOW;
	// put in the cookie and response byte count
	ResponseObject[i].Cookie = m_LastSentCmdCookie;
	ResponseObject[i].ResponseByteCount = pVcrCmdTable[m_bLastCmd].iRespCnt;
	// get the response
	return ReallyGetDevResponse( ResponseObject[i].buf,
		ResponseObject[i].ResponseByteCount );
}
//---------------------------------------------------------
//
//	GetBufferedResponse
//
//	Pulls a response out of the response array
//
//---------------------------------------------------------
DWORD 
CDevCom::GetBufferedResponse( TCHAR *buf, int Cookie)
{
	int i; 
	
	if ( !IsResponseBuffered( Cookie, &i ) )
		return DEV_COMM_ERR_RESPONSE_MISSING;
	
	// copy the response into the return buffer
	memcpy( buf, ResponseObject[i].buf, ResponseObject[i].ResponseByteCount);
		
	// Clear the entry
	ResponseObject[i].Cookie = -1;
	ResponseObject[i].ResponseByteCount = -1;
	// get the response
	return S_OK;
}

//---------------------------------------------------------
//
//	IsResponseBuffered
//
//	Searches the response array for the given cookie
//
//---------------------------------------------------------
BOOL
CDevCom::IsResponseBuffered( int Cookie, int *index)
{
	int i; 
	// find the response in the array
	for (i=0; i<MAX_RESPONSE_OBJECTS; i++) {
		if (ResponseObject[i].Cookie == Cookie) {
			*index = i;
			return TRUE;
		}
	}
	return FALSE;
}

//---------------------------------------------------------
//
//	Wait4DevResponse
//
//	Wrapper method that handles retries
//
//---------------------------------------------------------
DWORD
CDevCom::Wait4DevResponse( TCHAR *buf, int Cookie )
{
	DWORD dwret;
	int i = 0;
	do {
		if (i > RESPONSE_RETRY_MAX)
			return DEV_COMM_ERR_RESPONSE_TIMEOUT;
		dwret = GetDevResponse(buf, Cookie);
		if (dwret != 0L && dwret != DEV_COMM_ERR_COMMAND_NOT_SENT_YET)
			return dwret;
		if (dwret) 
			Sleep(RESPONSE_SLEEP_TIME);
	}while (dwret);
	return dwret;
}
//---------------------------------------------------------
//
//	GetSimulatedResponse
//
//	Used for simulation mode
//
//---------------------------------------------------------
DWORD 
CDevCom::GetSimulatedResponse( TCHAR *buf )
{
	switch (m_bLastCmd)	{
		case READ_TC:
			if (m_LastMotionCmd == PLAY || m_LastMotionCmd == FREEZE_OFF) {
				SimulatedFramecount++;
				if ( SimulatedFramecount > 2591999L )
					SimulatedFramecount = 0;
			}
			else if (m_LastMotionCmd == REWIND)	{
				if (SimulatedFramecount >2591999L)
					SimulatedFramecount = 2591999L;
				else
					SimulatedFramecount-=10;
			}
			GetSimulatedTimecode(buf, SimulatedFramecount);
			return 0L;

		case DEVICE_TYPE_REQUEST:
			strcpy(buf, strDevID);
			return 0L;

		case PLAY:
	 	case STOP:
		case TIMER_MODE_SELECT_LTC:
		case REWIND:
		case FREEZE_ON:
		case FREEZE_OFF:
		case STANDBY_OFF:
		default:
			if (VcrType == 0)
				strncpy(buf, (TCHAR *)SvoAck, SvoAckCount);
			else
				strncpy(buf, (TCHAR *)SvbkAck, SvbkAckCount);
			return 0L;
	}
	return 0L;
}
//---------------------------------------------------------
//
//	m_isMotionCmd
//
//	private helper method to differentiate Motion commands
//	from things like Timecode requests (sometimes timecode
//	requests need to be handled differently)
//
//---------------------------------------------------------
BOOL
CDevCom::m_isMotionCmd( BYTE Cmd ) {
 	switch (Cmd) {
		case READ_TC:
		case TIMER_MODE_SELECT_LTC:
			return FALSE;
		case PLAY:
		case STOP:
		case REWIND:
		case FFWD:
		case RECORD:
		case FREEZE_ON:
		case FREEZE_OFF:
		case STANDBY_OFF:
		case EJECT:
			return TRUE;
		default:
			return FALSE;
	}
}
//---------------------------------------------------------
//
//	ProcessDeviceTypeResponse
//
// Convert response to device type request to string if possible
//
//---------------------------------------------------------
void
CDevCom::ProcessDeviceTypeResponse( TCHAR *bufin, TCHAR *bufout)
{
	int i;
	if (m_bSimulateHardware) {
		strcpy( bufout, bufin );
		return;
	}

	// handle the two machine types differently
	if (VcrType == 0) {
		// compare the 2 data bytes of the response to the list of known
		// machine types.  If we find a match, return the corresponding 
		// device type string
		for (i=0; i<DevTypeCnt; i++) {
			if ( bufin[2] != SvoDevTbl[i].bData1 )
				continue;
			if ( bufin[3] != SvoDevTbl[i].bData2 )
				continue;
			strcpy( bufout, strSvoDevID[i] );
			return;
		}
		// unknown machine
		strcpy( bufout, strUnknownDevID );
	}
	else		
		if (bufin[0] & (TCHAR)SvbkDevType)
			strcpy( bufout, strSvbkDevID );
		else
			// unknown machine
			strcpy( bufout, strUnknownDevID );
	return;
}

//---------------------------------------------------------
//
//	ProcessDeviceStatusResponse
//
// Stuff status structure with response to status request
//	returns FALSE if SVBK returns a NAK
//
//---------------------------------------------------------
BOOL
CDevCom::ProcessDeviceStatusResponse( TCHAR *bufin, PVCRSTATUS pStatus )
{
	if (m_bSimulateHardware) {
		pStatus->bCassetteOut = OAFALSE;
		pStatus->bLocal = OAFALSE;
		return FALSE;
	}

	// we only process two bits of information right now, and 
	// it's hardcoded for clarity and simplicity
	if (VcrType == 0) {	// SVO
		if (bufin[2] & 0x20)
			pStatus->bCassetteOut = OATRUE;
		else
			pStatus->bCassetteOut = OAFALSE;

		if (bufin[2] & 0x01)
			pStatus->bLocal = OATRUE;
		else
			pStatus->bLocal = OAFALSE;
	}
	else { // SVBK
		if (bufin[0] == 0x0b)	// machine can send a NAK if not ready
			return TRUE;
		if (bufin[0] & 0x08)
			pStatus->bCassetteOut = OATRUE;
		else 
			pStatus->bCassetteOut = OAFALSE;
			// dummy response for remote/local because that 
			// info is not available
		pStatus->bLocal = OAFALSE;
	}
	return FALSE;
}
//---------------------------------------------------------
//
//	AddCommand
//
// Insert pending command in empty space in array
//
//---------------------------------------------------------
PCOMMANDOBJECT
CDevCom::AddCommand( int Cmd, int Cookie ){

	int i;
	POSITION pos;
	
	for (i=0; i<COMMANDLISTSIZE; i++) {
		if (CommandObjects[i].Command == -1) {
			CommandObjects[i].Command = Cmd;
			CommandObjects[i].Cookie = Cookie;
			CommandObjects[i].Timestamp = timeGetTime();
			pos = CommandList.AddTail( &CommandObjects[i] );
			return &CommandObjects[i];
		}
	}
	return NULL; //Null means no more space
}
//---------------------------------------------------------
//
//	RemoveCommand
//
// Clear entry in command array
//
//---------------------------------------------------------
void
CDevCom::RemoveCommand( PCOMMANDOBJECT pCmdObj )
{
	pCmdObj->Command = -1;
	pCmdObj->Cookie = -1;
	return;
}
//---------------------------------------------------------
//
//	TestResponse
//
// Calculates checksum on response and compares it to embedded
// checksum.  Returns FALSE if OK, TRUE if not
//
//---------------------------------------------------------
BOOL
CDevCom::TestResponse( TCHAR *buf, int Count )
{
	BYTE CalcCheckSum = 0;
	int i;

	for ( i=0; i<(Count-1); i++) {
		CalcCheckSum += (BYTE)buf[i];
	}
	if ( CalcCheckSum != (BYTE)buf[(Count-1)] )
		return TRUE;
	return FALSE;
}
//---------------------------------------------------------
//
//	ProcessVcrTC - converts the timecode response to a binary
//	framecount.  Handles both SVO and SVBK timecode formats
//
//---------------------------------------------------------
DWORD
CDevCom::ProcessVcrTC( TCHAR *inbuf, DWORD *dwFCM, long *ptimecode, 
					  long *puserbits)
{
	BYTE btemp;
	WORD wtemp;
	static TCHAR outbuf[12];
	static TIMECODE_SAMPLE tcsTimecode;
	int i=0;
	TCHAR *ptemp;

	// look at first byte to determine SVO or SVBK
	if ((inbuf[0] & 0x70) != 0x70 ) { // SVO's command value
		// SVBK timecode format is 8 byte ASCII, Hx10 first
		// hours.  Can't tell whether it's drop or nondrop, 
		// so we'll stay nondrop SVBK also has an unpredictable 
		// number of 0xA1's at the head of the buffer, so we must correct.
		while ( inbuf[i] == (TCHAR)0xA1 || inbuf[i] == (TCHAR)0xA2 ) {
			i++;
		}
		ptemp =&inbuf[i];

		wtemp = AM_TIMECODE_30NONDROP;
		// hours
		outbuf[0] = ptemp[0];
		outbuf[1] = ptemp[1];
		outbuf[2] = ':';

		// minutes
		outbuf[3] = ptemp[2];
		outbuf[4] = ptemp[3];
		outbuf[5] = ':';

		// seconds
		outbuf[6] = ptemp[4];
		outbuf[7] = ptemp[5];
		outbuf[8] = ':';

		// frames
		outbuf[9] = ptemp[6];
		outbuf[10] = ptemp[7];

		// terminate
		outbuf[11] = 0;

		// convert to binary
		tcsTimecode.timecode.wFrameRate = wtemp;
		m_pTimecode->ConvertStringToTimecode(outbuf, &tcsTimecode); 
		*ptimecode = tcsTimecode.timecode.dwFrames;
		*dwFCM = (DWORD)wtemp;
		*puserbits = (DWORD)0L; // forget userbits
	}
	else { // SVO
		// SVO t/c format is this:
		// 		BIT7 BIT6 BIT5 BIT4 BIT3 BIT2 BIT1 BIT0
		//DATA1	 x	  DF	 10F   |		 1F
		//DATA2	 x		 10S	   |		 1S
		//DATA3	 x		 10M	   |		 1M
		//DATA4	 x	  x		 10H   |		 1H
		//
		// It lives in bytes 2 thru 5 in the response from the machine, and
		// userbits live in bytes 6 thru 9

		// just yank em out and put em in the output buffer
		// get the dropframe flag
		if ( inbuf[2] & 0x40 )
			wtemp = AM_TIMECODE_30DROP;
		else
			wtemp = AM_TIMECODE_30NONDROP;

		// hours
		btemp = inbuf[5] & 0x30;
		btemp = btemp>>(4*sizeof(TCHAR));
		btemp += '0';
		outbuf[0] = btemp;
		outbuf[1] = (inbuf[5] & 0x0f) + '0';
		outbuf[2] = ':';

		// minutes
		btemp = inbuf[4] & 0x70;
		btemp = btemp>>(4*sizeof(TCHAR));
		outbuf[3] = btemp + '0';
		outbuf[4] = (inbuf[4] & 0x0f) + '0';
		outbuf[5] = ':';

		// seconds
		btemp = inbuf[3] & 0x70;
		btemp = btemp>>(4*sizeof(TCHAR));
		outbuf[6] = btemp + '0';
		outbuf[7] = (inbuf[3] & 0x0f) + '0';
		if ( AM_TIMECODE_30DROP == wtemp )
			outbuf[8] = ';';
		else
			outbuf[8] = ':';

		// frames
		btemp = inbuf[2] & 0x30;
		btemp = btemp>>(4*sizeof(TCHAR));
		outbuf[9] = btemp + '0';
		outbuf[10] = (inbuf[2] & 0x0f) + '0';

		// terminate
		outbuf[11] = 0;

		// convert to binary
		tcsTimecode.timecode.wFrameRate = wtemp;
		m_pTimecode->ConvertStringToTimecode(outbuf, &tcsTimecode); 
		*ptimecode = tcsTimecode.timecode.dwFrames;
		*dwFCM = (DWORD)wtemp;
		*puserbits = (DWORD)inbuf[6];
	}
	return 0;
}

//---------------------------------------------------------
//
//	For no-hardware mode
//
//---------------------------------------------------------
void
CDevCom::GetSimulatedTimecode( TCHAR * timecode, long framecount )
{
	long temp;
	int i;

	if (VcrType == 0) {
		// SVO t/c format is this:
		// 		BIT7 BIT6 BIT5 BIT4 BIT3 BIT2 BIT1 BIT0
		//DATA1	 x	  DF	 10F   |		 1F
		//DATA2	 x		 10S	   |		 1S
		//DATA3	 x		 10M	   |		 1M
		//DATA4	 x	  x		 10H   |		 1H
		//
		// It lives in bytes 2 thru 5 in the response from the machine
		// Bytes 6 thru 9 are user bits, and 10 is the checksum
		
		// put in the command
		timecode[0] = 0x74;
		timecode[1] = 0x04;

		// convert the frame count to Svo t/c format
		temp = framecount / lNDFTable[0];	// hours
		framecount -= temp * lNDFTable[0];
		timecode[5] = (TCHAR)temp << (4*sizeof(TCHAR));
		temp = framecount / lNDFTable[1];
		framecount -= temp * lNDFTable[1];
		timecode[5] |= temp;	

		temp = framecount / lNDFTable[2];	// minutes
		framecount -= temp * lNDFTable[2];
		timecode[4] = (TCHAR)temp << (4*sizeof(TCHAR));
		temp = framecount / lNDFTable[3];
		framecount -= temp * lNDFTable[3];
		timecode[4] |= temp;

		temp = framecount / lNDFTable[4];	// seconds
		framecount -= temp * lNDFTable[4];
		timecode[3] = (TCHAR)temp << (4*sizeof(TCHAR));
		temp = framecount / lNDFTable[5];
		framecount -= temp * lNDFTable[5];
		timecode[3] |= temp;

		temp = framecount / lNDFTable[6];	// frames
		framecount -= temp * lNDFTable[6];
		timecode[2] = (TCHAR)temp << (4*sizeof(TCHAR));
		timecode[2] |= framecount;

		// do the checksum
		timecode[10] = 0;
		for (i=0; i<11; i++)
		{
			timecode[10] +=timecode[i];
		}
	}
	else {
		// SVBK timecode format is 8 byte ASCII, Hx10 first
		// hours.  Can't tell whether it's drop or nondrop, so we'll stay 
		// nondrop
		temp = framecount / lNDFTable[0];	// hours
		framecount -= temp * lNDFTable[0];
		timecode[0] = (TCHAR)temp | 0x30;
		temp = framecount / lNDFTable[1];
		framecount -= temp * lNDFTable[1];
		timecode[1] = temp | 0x30;	

		temp = framecount / lNDFTable[2];	// minutes
		framecount -= temp * lNDFTable[2];
		timecode[2] = (TCHAR)temp | 0x30;
		temp = framecount / lNDFTable[3];
		framecount -= temp * lNDFTable[3];
		timecode[3] = temp | 0x30;

		temp = framecount / lNDFTable[4];	// seconds
		framecount -= temp * lNDFTable[4];
		timecode[4] = (TCHAR)temp | 0x30;
		temp = framecount / lNDFTable[5];
		framecount -= temp * lNDFTable[5];
		timecode[5] = temp | 0x30;

		temp = framecount / lNDFTable[6];	// frames
		framecount -= temp * lNDFTable[6];
		timecode[6] = (TCHAR)temp | 0x30;
		timecode[7] = framecount | 0x30;
	}
	return;
}

// eof cdevcom.cpp
