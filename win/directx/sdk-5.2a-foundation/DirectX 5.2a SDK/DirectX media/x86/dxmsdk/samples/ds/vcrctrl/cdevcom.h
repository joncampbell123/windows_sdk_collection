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
//	filename: cdevcom.h
//
//	header for devcom.cpp - External Device Communications Class
//		for an SVO-type RS-422 Controlled Tape Machine and
//		SVBK-type RS-232 Controlled machines
//
//
#ifndef CDEVCOM_H
#define	CDEVCOM_H	1

//----------------------------------------------------------
//
// Structures and definitions for this implementation
//
// communications and low-level protocol defines
// device ports
#define DEV_PORT_SIM	1
#define DEV_PORT_COM1	2	// standard serial ports
#define DEV_PORT_COM2	3
#define DEV_PORT_COM3	4
#define DEV_PORT_COM4	5
#define DEV_PORT_DIAQ	6	// Diaquest driver
#define DEV_PORT_ARTI	7	// ARTI driver
#define DEV_PORT_1394	8	// IEEE 1394 Bus
#define DEV_PORT_USB	9	// Universal Serial Bus
#define DEV_PORT_MIN	DEV_PORT_SIM
#define DEV_PORT_MAX	DEV_PORT_USB

// machine commands - ****The order of these must match the order
// of the commands in the command list in devcom.cpp!!!
#define PLAY	0
#define STOP	1
#define READ_TC	2
#define TIMER_MODE_SELECT_LTC 3
#define REWIND	4
#define FREEZE_ON 5
#define FREEZE_OFF 6
#define STANDBY_OFF 7
#define STANDBY_ON 8
#define DEVICE_TYPE_REQUEST 9
#define REQUEST_STATUS 10
#define EJECT 11
#define LOCAL_ON 12
#define LOCAL_OFF 13
#define FFWD 14
#define RECORD 15
#define DEVICE_CLEAR 16

// error values
#define DEV_COMM_ERR_COMMAND_NOT_SENT_YET	1L
#define DEV_COMM_ERR_QUEUE_OVERFLOW			2L
#define DEV_COMM_ERR_COMMUNICATION_ERROR	3L
#define DEV_COMM_ERR_RESPONSE_MISMATCH		4L
#define DEV_COMM_ERR_RESPONSE_TIMEOUT		5L
#define DEV_COMM_ERR_RESPONSE_CHECKSUM		6L
#define DEV_COMM_ERR_COMMAND_MISSING		7L
#define DEV_COMM_ERR_RESPONSE_OVERFLOW		8L
#define DEV_COMM_ERR_RESPONSE_MISSING		9L
#define DEV_COMM_ERR_COMMAND_NOT_SUPPORTED	10L

// low level machine status structure filled in after
// REQUEST_STATUS command from above.  This structure would
// grow in a full implementation
typedef struct tagVCRSTATUS{
	BOOL bCassetteOut;	// OATRUE means no cassette
	BOOL bLocal;		// OATRUE means front panel switch in local
} VCRSTATUS;

typedef VCRSTATUS far *PVCRSTATUS;

typedef struct tagCOMMANDOBJECT {
	int Command;
	int Cookie;
	DWORD Timestamp;
} COMMANDOBJECT;
typedef COMMANDOBJECT far *PCOMMANDOBJECT;

typedef struct tagRESPONSEOBJECT {
	int Cookie;
	int ResponseByteCount;
	TCHAR buf[16];
} RESPONSEOBJECT;
typedef RESPONSEOBJECT far *PRESPONSEOBJECT;

//----------------------------------------------------------
//
//	The communications object
//
//----------------------------------------------------------
class CDevCom : public CCritSec{
public:
	// constructor
	CDevCom(CBaseFilter *pFilter, HRESULT *phr);
	
	~CDevCom();

	// all of these methods return 0 for success
	// Initialize Device Port accepts the following:
	//	DEV_PORT_COM1 | DEV_PORT_COM2 |	DEV_PORT_COM3 | 
	//	DEV_PORT_COM3 | DEV_PORT_DIAQ | DEV_PORT_ARTI etc.
	DWORD OpenDevPort(int port);

	// Close Device Port
	DWORD CloseDevPort(void);

	// Flush Device Response Buffer
	DWORD FlushDevResponseBuffer(void);

	// Send command to device.
	DWORD SendDevCmd(int, int *);

	// Read response from device - accepts pointer to response buffer
	// Must pass cookie in to get proper command.  If command hasn't
	// actually been sent, method returns with and error value
	DWORD GetDevResponse(TCHAR *, int);
	// this one sits around (by sleeping) until either the proper
	// response comes back, a timeout occurs, or an error happens.
	DWORD Wait4DevResponse(TCHAR *, int);

	// convert response to device type request to a string
	void ProcessDeviceTypeResponse( TCHAR *bufin, TCHAR *bufout);

	// convert status request response to something meaningful
	BOOL ProcessDeviceStatusResponse( TCHAR *bufin, PVCRSTATUS pStatus );
	
	// convert raw machine timecode to something usable
	DWORD ProcessVcrTC(TCHAR * RawTC, DWORD * pdwFCM, long * ptimecode, 
		long * puserbits);
	
protected:
	CCritSec  *m_pCritSec;					// Object we use for locking

private:
	CTimecode *m_pTimecode;
	CBaseFilter *m_pFilter;					// our filter
	
	static HANDLE	m_hDevPort;				// handle of opened comm port
	static TCHAR *	m_pcDevPort;			// name of opened comm port
	static BYTE		m_bLastCmd;
	static BYTE		m_LastMotionCmd;		// need this for effective 
											//  simulation
	static DCB 		m_dcb;					// initialize this with 
											//   GetCommState
	static int 		m_PendingResponseCnt;	// for synchronizing responses
	static BOOL		m_bSimulateHardware;	// TRUE if no VCR connected
	static COMMTIMEOUTS m_ctmoTimeout;		// communications timeout structure
	static COMSTAT	m_csCommStat;			// communications status structure
	static int		m_CurCmdCookie;			// used for command queueing
	static int		m_LastSentCmdCookie;	// so we know immediately which 
											//   command was just sent

	DWORD GetSimulatedResponse( TCHAR *buf );	// for the big fake out
	BOOL m_isMotionCmd( BYTE Cmd );	
	PCOMMANDOBJECT AddCommand(int Cmd, int Cookie);
	void RemoveCommand(PCOMMANDOBJECT pCmd);
	DWORD ReallySendDevCmd(int, int);
	DWORD ReallyGetDevResponse( TCHAR *buf, int bytecnt );
	BOOL TestResponse( TCHAR *buf, int Count );
	DWORD BufferResponse(void);
	DWORD GetBufferedResponse( TCHAR *buf, int Cookie);
	BOOL IsResponseBuffered( int Cookie, int *index);
	void GetSimulatedTimecode(TCHAR * timecode, long framecount);
};

#endif		// #ifndef CDEVCOM_H
// eof cdevcom.h
