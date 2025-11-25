//****************************************************************************
//
//		       Microsoft NT Remote Access Service
//
//		       Copyright 1992-93
//
//
//  Revision History
//
//
//  5/26/92	Gurdeep Singh Pall	Created
//
//
//  Description: This file contains all structure and constant definitions for
//		 RAS Manager Component.
//
//****************************************************************************


#ifndef _RASMAN_
#define _RASMAN_

#include <windows.h>

#define WM_RASAPICOMPLETE   0xCCCC	// From the "user" window msg range

#define RASMAN_SERVICE_NAME "RASMAN"

#define MAX_MEDIA_NAME	    16
#define MAX_PORT_NAME	    16
#define MAX_USERKEY_SIZE    132
#define MAX_DEVICE_NAME     32
#define MAX_DEVICETYPE_NAME 16
#define MAX_PARAM_KEY_SIZE  32
#define MAX_XPORT_NAME	    128 // ??
#define MAX_IDENTIFIER_SIZE 32
#define MAX_STAT_NAME	    32
#define MAX_CHALLENGE_SIZE  8
#define MAX_RESPONSE_SIZE   24
#define MAX_USERNAME_SIZE   256
#define MAX_LAN_NETS	    16

typedef  DWORD	HPORT ;

enum RASMAN_STATUS {

	OPEN	= 0,

	CLOSED	= 1,

	UNKNOWN = 2
}   ;

typedef enum RASMAN_STATUS	RASMAN_STATUS ;


enum RASMAN_USAGE {

	CALL_IN     = 0,

	CALL_OUT    = 1,

	CALL_IN_OUT = 2,

	CALL_NONE   = 3

} ;

typedef enum RASMAN_USAGE	RASMAN_USAGE ;


enum RAS_FORMAT {

	Number	    = 0,

	String	    = 1

} ;

typedef enum RAS_FORMAT	RAS_FORMAT ;


union RAS_VALUE {

	DWORD	Number ;

	struct	{
		DWORD	Length ;
		PCHAR	Data ;
		} String ;
} ;

typedef union RAS_VALUE	RAS_VALUE ;



enum RASMAN_STATE {

	CONNECTING	= 0,

	LISTENING	= 1,

	CONNECTED	= 2,

	DISCONNECTING	= 3,

	DISCONNECTED	= 4

} ;

typedef enum RASMAN_STATE	RASMAN_STATE ;


enum RASMAN_DISCONNECT_REASON {

    USER_REQUESTED	= 0,

    REMOTE_DISCONNECTION= 1,

    HARDWARE_FAILURE	= 2,

    NOT_DISCONNECTED	= 3
} ;

typedef enum RASMAN_DISCONNECT_REASON	RASMAN_DISCONNECT_REASON ;


struct RAS_PARAMS {

    CHAR	P_Key	[MAX_PARAM_KEY_SIZE] ;

    RAS_FORMAT	P_Type ;

    BYTE	P_Attributes ;

    RAS_VALUE	P_Value ;

} ;

typedef struct RAS_PARAMS	RAS_PARAMS ;


struct RASMAN_PORT {

    HPORT		P_Handle ;

    CHAR		P_PortName  [MAX_PORT_NAME] ;

    RASMAN_STATUS	P_Status ;

    RASMAN_USAGE	P_ConfiguredUsage ;

    RASMAN_USAGE	P_CurrentUsage ;

    CHAR		P_UserKey   [MAX_USERKEY_SIZE] ;

    CHAR		P_Identifier[MAX_IDENTIFIER_SIZE] ;

    CHAR		P_MediaName [MAX_MEDIA_NAME] ;

    CHAR		P_DeviceType[MAX_DEVICETYPE_NAME] ;

    CHAR		P_DeviceName[MAX_DEVICE_NAME] ;

} ;

typedef struct RASMAN_PORT	RASMAN_PORT ;


struct RASMAN_PORTINFO {

    WORD	PI_NumOfParams ;

    RAS_PARAMS	PI_Params[1] ;

} ;

typedef struct RASMAN_PORTINFO RASMAN_PORTINFO ;


struct RASMAN_DEVICE {

    CHAR	D_Name	[MAX_DEVICE_NAME] ;

} ;

typedef struct RASMAN_DEVICE	RASMAN_DEVICE ;


struct RASMAN_DEVICEINFO {

    WORD	DI_NumOfParams ;

    RAS_PARAMS	DI_Params[1] ;

} ;

typedef struct RASMAN_DEVICEINFO   RASMAN_DEVICEINFO ;



enum RAS_PROTOCOLTYPE {

	ASYBEUI     = 0x80D5,

	IPX	    = 0x8138,

	IP	    = 0x0800,

	ARP	    = 0x0806,

	APPLETALK   = 0x80F3,

	XNS	    = 0x0600,

	RASAUTH     = 0x8FFF,

	INVALID_TYPE= 0x2222
} ;
typedef enum RAS_PROTOCOLTYPE RAS_PROTOCOLTYPE ;


struct RASMAN_PROTOCOLINFO {

    CHAR		PI_XportName	[MAX_XPORT_NAME] ;

    RAS_PROTOCOLTYPE	PI_Type ;

} ;

typedef struct RASMAN_PROTOCOLINFO RASMAN_PROTOCOLINFO ;

struct	RASMAN_ROUTEINFO {

    BYTE	RI_LanaNum ;

    WCHAR	RI_XportName	[MAX_XPORT_NAME] ;

} ;

typedef struct RASMAN_ROUTEINFO    RASMAN_ROUTEINFO ;

struct	RASMAN_INFO {

    RASMAN_STATUS		RI_PortStatus ;

    RASMAN_STATE		RI_ConnState ;

    DWORD			RI_LastError ;

    RASMAN_USAGE		RI_CurrentUsage ;

    CHAR			RI_DeviceTypeConnecting [MAX_DEVICETYPE_NAME] ;

    CHAR			RI_DeviceConnecting [MAX_DEVICE_NAME] ;

    RASMAN_DISCONNECT_REASON	RI_DisconnectReason ;

    DWORD			RI_OwnershipFlag ;

    CHAR			RI_Identifier [MAX_IDENTIFIER_SIZE] ;

    DWORD			RI_ConnectDuration ;
} ;

typedef struct RASMAN_INFO	  RASMAN_INFO ;



struct	RAS_STATISTICS {

    WORD    S_NumOfStatistics ;

    ULONG   S_Statistics[1] ;

} ;

typedef struct RAS_STATISTICS	RAS_STATISTICS ;


struct RASMAN_MACFEATURES {

    ULONG  SendFeatureBits;   // A bit field of compression/features sendable

    ULONG  RecvFeatureBits;   // A bit field of compression/features receivable

    ULONG  MaxSendFrameSize;  // Maximum frame size that can be sent
                              //   must be less than or equal to default
    ULONG  MaxRecvFrameSize;  // Maximum frame size that can be rcvd
                              //   must be less than or equal to default
    ULONG   LinkSpeed ;
};

typedef struct RASMAN_MACFEATURES  RASMAN_MACFEATURES ;



//* RAS Manager entrypoint Prototypes
//

DWORD APIENTRY RasPortOpen(PCHAR, PCHAR, PCHAR, HPORT*, HANDLE);

DWORD APIENTRY RasPortClose(HPORT);

DWORD APIENTRY RasPortEnum(PBYTE, PWORD, PWORD);

DWORD APIENTRY RasPortGetInfo(HPORT, PBYTE, PWORD);

DWORD APIENTRY RasPortSetInfo(HPORT, RASMAN_PORTINFO*);

DWORD APIENTRY RasPortDisconnect(HPORT, HANDLE);

DWORD APIENTRY RasPortSend(HPORT, PBYTE, WORD);

DWORD APIENTRY RasPortReceive(HPORT, PBYTE, PWORD, DWORD, HANDLE);

DWORD APIENTRY RasPortListen(HPORT, DWORD, HANDLE);

DWORD APIENTRY RasPortConnectComplete(HPORT);

DWORD APIENTRY RasPortGetStatistics(HPORT, PBYTE, PWORD);

DWORD APIENTRY RasPortClearStatistics(HPORT);

DWORD APIENTRY RasDeviceEnum(PCHAR, PBYTE, PWORD, PWORD);

DWORD APIENTRY RasDeviceGetInfo(HPORT, PCHAR, PCHAR, PBYTE, PWORD);

DWORD APIENTRY RasDeviceSetInfo(HPORT, PCHAR, PCHAR, RASMAN_DEVICEINFO*);

DWORD APIENTRY RasDeviceConnect(HPORT, PCHAR, PCHAR, DWORD, HANDLE);

DWORD APIENTRY RasGetInfo(HPORT, RASMAN_INFO*);

DWORD APIENTRY RasGetBuffer(PBYTE*, PWORD);

DWORD APIENTRY RasFreeBuffer(PBYTE);

DWORD APIENTRY RasProtocolEnum(PBYTE, PWORD, PWORD);

DWORD APIENTRY RasAllocateRoute(HPORT, RAS_PROTOCOLTYPE, BOOL, RASMAN_ROUTEINFO*);

DWORD APIENTRY RasActivateRoute(HPORT, RAS_PROTOCOLTYPE, RASMAN_ROUTEINFO*);

DWORD APIENTRY RasDeAllocateRoute(HPORT, RAS_PROTOCOLTYPE);

DWORD APIENTRY RasCompressionGetInfo(HPORT, RASMAN_MACFEATURES*);

DWORD APIENTRY RasCompressionSetInfo(HPORT, RASMAN_MACFEATURES*);

DWORD APIENTRY RasGetUserCredentials(PBYTE, PLUID, PWCHAR, PBYTE, PBYTE) ;

DWORD APIENTRY RasRequestNotification (HPORT, HANDLE) ;

DWORD APIENTRY RasEnumLanNets (DWORD *, UCHAR *) ;

DWORD APIENTRY RasInitialize () ;

DWORD _RasmanInit();

VOID _RasmanEngine();

#endif
