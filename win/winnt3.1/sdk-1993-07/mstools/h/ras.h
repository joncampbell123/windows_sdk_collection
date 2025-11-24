/* Copyright (c) 1992, Microsoft Corporation, all rights reserved
**
** ras.h
** Remote Access external API
** Public header for external API clients
*/

#ifndef _RAS_H_
#define _RAS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNLEN
#include <lmcons.h>
#endif


#define RAS_MaxEntryName      20
#define RAS_MaxDeviceName     32
#define RAS_MaxDeviceType     16
#define RAS_MaxParamKey       32
#define RAS_MaxParamValue     128
#define RAS_MaxPhoneNumber    128
#define RAS_MaxCallbackNumber 48


DECLARE_HANDLE( HRASCONN );
#define LPHRASCONN HRASCONN*


/* Identifies an active RAS connection.  (See RasEnumConnections)
*/
#define RASCONNW struct tagRASCONNW

RASCONNW
{
    DWORD    dwSize;
    HRASCONN hrasconn;
    WCHAR    szEntryName[ RAS_MaxEntryName + 1 ];
};

#define RASCONNA struct tagRASCONNA

RASCONNA
{
    DWORD    dwSize;
    HRASCONN hrasconn;
    CHAR     szEntryName[ RAS_MaxEntryName + 1 ];
};

#ifdef UNICODE
#define RASCONN RASCONNW
#else
#define RASCONN RASCONNA
#endif

#define LPRASCONNW RASCONNW*
#define LPRASCONNA RASCONNA*
#define LPRASCONN  RASCONN*


/* Enumerates intermediate states to a connection.  (See RasDial)
*/
#define RASCS_PAUSED 0x1000
#define RASCS_DONE   0x2000

#define RASCONNSTATE enum tagRASCONNSTATE

RASCONNSTATE
{
    RASCS_OpenPort = 0,
    RASCS_PortOpened,
    RASCS_ConnectDevice,
    RASCS_DeviceConnected,
    RASCS_AllDevicesConnected,
    RASCS_Authenticate,
    RASCS_AuthNotify,
    RASCS_AuthRetry,
    RASCS_AuthCallback,
    RASCS_AuthChangePassword,
    RASCS_AuthProject,
    RASCS_AuthLinkSpeed,
    RASCS_AuthAck,
    RASCS_ReAuthenticate,
    RASCS_Authenticated,
    RASCS_PrepareForCallback,
    RASCS_WaitForModemReset,
    RASCS_WaitForCallback,

    RASCS_Interactive = RASCS_PAUSED,
    RASCS_RetryAuthentication,
    RASCS_CallbackSetByCaller,
    RASCS_PasswordExpired,

    RASCS_Connected = RASCS_DONE,
    RASCS_Disconnected
};

#define LPRASCONNSTATE RASCONNSTATE*


/* Describes the status of a RAS connection.  (See RasConnectionStatus)
*/
#define RASCONNSTATUSW struct tagRASCONNSTATUSW

RASCONNSTATUSW
{
    DWORD        dwSize;
    RASCONNSTATE rasconnstate;
    DWORD        dwError;
    WCHAR        szDeviceType[ RAS_MaxDeviceType + 1 ];
    WCHAR        szDeviceName[ RAS_MaxDeviceName + 1 ];
};

#define RASCONNSTATUSA struct tagRASCONNSTATUSA

RASCONNSTATUSA
{
    DWORD        dwSize;
    RASCONNSTATE rasconnstate;
    DWORD        dwError;
    CHAR         szDeviceType[ RAS_MaxDeviceType + 1 ];
    CHAR         szDeviceName[ RAS_MaxDeviceName + 1 ];
};

#ifdef UNICODE
#define RASCONNSTATUS RASCONNSTATUSW
#else
#define RASCONNSTATUS RASCONNSTATUSA
#endif

#define LPRASCONNSTATUSW RASCONNSTATUSW*
#define LPRASCONNSTATUSA RASCONNSTATUSA*
#define LPRASCONNSTATUS  RASCONNSTATUS*


/* Describes connection establishment parameters.  (See RasDial)
*/
#define RASDIALPARAMSW struct tagRASDIALPARAMSW

RASDIALPARAMSW
{
    DWORD dwSize;
    WCHAR szEntryName[ RAS_MaxEntryName + 1 ];
    WCHAR szPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
    WCHAR szCallbackNumber[ RAS_MaxCallbackNumber + 1 ];
    WCHAR szUserName[ UNLEN + 1 ];
    WCHAR szPassword[ PWLEN + 1 ];
    WCHAR szDomain[ DNLEN + 1 ];
};

#define RASDIALPARAMSA struct tagRASDIALPARAMSA

RASDIALPARAMSA
{
    DWORD dwSize;
    CHAR  szEntryName[ RAS_MaxEntryName + 1 ];
    CHAR  szPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
    CHAR  szCallbackNumber[ RAS_MaxCallbackNumber + 1 ];
    CHAR  szUserName[ UNLEN + 1 ];
    CHAR  szPassword[ PWLEN + 1 ];
    CHAR  szDomain[ DNLEN + 1 ];
};

#ifdef UNICODE
#define RASDIALPARAMS RASDIALPARAMSW
#else
#define RASDIALPARAMS RASDIALPARAMSA
#endif

#define LPRASDIALPARAMSW RASDIALPARAMSW*
#define LPRASDIALPARAMSA RASDIALPARAMSA*
#define LPRASDIALPARAMS  RASDIALPARAMS*


/* Describes an enumerated RAS phone book entry name.  (See RasEntryEnum)
*/
#define RASENTRYNAMEW struct tagRASENTRYNAMEW

RASENTRYNAMEW
{
    DWORD dwSize;
    WCHAR szEntryName[ RAS_MaxEntryName + 1 ];
};

#define RASENTRYNAMEA struct tagRASENTRYNAMEA

RASENTRYNAMEA
{
    DWORD dwSize;
    CHAR  szEntryName[ RAS_MaxEntryName + 1 ];
};

#ifdef UNICODE
#define RASENTRYNAME RASENTRYNAMEW
#else
#define RASENTRYNAME RASENTRYNAMEA
#endif

#define LPRASENTRYNAMEW RASENTRYNAMEW*
#define LPRASENTRYNAMEA RASENTRYNAMEA*
#define LPRASENTRYNAME  RASENTRYNAME*


/* Prototype for caller's RasDial callback handler.  Arguments are the message
** ID (currently always WM_RASDIALEVENT), the current RASCONNSTATE and the
** error that has occurred (or 0 if none).
*/
#define WM_RASDIALEVENT 0xCCCD
typedef VOID (WINAPI *RASDIALFUNC)( UINT, RASCONNSTATE, DWORD );


/* External RAS API function prototypes.
*/
DWORD APIENTRY RasDialA( LPSTR, LPSTR, LPRASDIALPARAMSA, LPVOID, RASDIALFUNC,
                   LPHRASCONN );

DWORD APIENTRY RasDialW( LPWSTR, LPWSTR, LPRASDIALPARAMSW, LPVOID, RASDIALFUNC,
                   LPHRASCONN );

DWORD APIENTRY RasEnumConnectionsA( LPRASCONNA, LPDWORD, LPDWORD );

DWORD APIENTRY RasEnumConnectionsW( LPRASCONNW, LPDWORD, LPDWORD );

DWORD APIENTRY RasEnumEntriesA( LPSTR, LPSTR, LPRASENTRYNAMEA, LPDWORD,
                   LPDWORD );

DWORD APIENTRY RasEnumEntriesW( LPWSTR, LPWSTR, LPRASENTRYNAMEW, LPDWORD,
                   LPDWORD );

DWORD APIENTRY RasGetConnectStatusA( HRASCONN, LPRASCONNSTATUSA );

DWORD APIENTRY RasGetConnectStatusW( HRASCONN, LPRASCONNSTATUSW );

DWORD APIENTRY RasGetErrorStringA( UINT, LPSTR, DWORD );

DWORD APIENTRY RasGetErrorStringW( UINT, LPWSTR, DWORD );

DWORD APIENTRY RasHangUpA( HRASCONN );

DWORD APIENTRY RasHangUpW( HRASCONN );


#ifdef UNICODE
#define RasDial             RasDialW
#define RasEnumConnections  RasEnumConnectionsW
#define RasEnumEntries      RasEnumEntriesW
#define RasGetConnectStatus RasGetConnectStatusW
#define RasGetErrorString   RasGetErrorStringW
#define RasHangUp           RasHangUpW
#else
#define RasDial             RasDialA
#define RasEnumConnections  RasEnumConnectionsA
#define RasEnumEntries      RasEnumEntriesA
#define RasGetConnectStatus RasGetConnectStatusA
#define RasGetErrorString   RasGetErrorStringA
#define RasHangUp           RasHangUpA
#endif

#ifdef __cplusplus
}
#endif

#endif // _RAS_H_
