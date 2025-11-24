/* Copyright (c) 1992-1994, Microsoft Corporation, all rights reserved
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

/* Definition of the network-related constants
*/

#if (WINVER >= 0x400)

#ifndef NETCONS_INCLUDED

#define NETCONS_INCLUDED

#define UNLEN       256                 // Maximum user name length
#define PWLEN       256                 // Maximum password length
#define DNLEN       15                  // Maximum domain name length
#define NETBIOS_NAME_LEN  16            // NetBIOS net name (bytes)

#define LM20_PWLEN  14                  // LM 2.0 Maximum password length

/* Constants used with encryption
*/

#define CRYPT_KEY_LEN           7
#define CRYPT_TXT_LEN           8
#define ENCRYPTED_PWLEN         16
#define SESSION_PWLEN           24
#define SESSION_CRYPT_KLEN      21

#endif  /* NETCONS_INCLUDED */

#else   /* WINVER >= 0x400 */

#ifndef UNLEN
#include <lmcons.h>
#endif

#endif  /* WINVER >= 0x400 */

/* Maximum sizes for RAS information
*/

#if (WINVER >= 0x400)

#define RAS_MaxEntryName      256
#define RAS_MaxDeviceName     128
#define RAS_MaxDeviceType     16
#define RAS_MaxPhoneNumber    128
#define RAS_MaxCallbackNumber RAS_MaxPhoneNumber

#else   /* WINVER >= 0x400 */

#define RAS_MaxEntryName      20
#define RAS_MaxDeviceName     32
#define RAS_MaxDeviceType     16
#define RAS_MaxParamKey       32
#define RAS_MaxParamValue     128
#define RAS_MaxPhoneNumber    128
#define RAS_MaxCallbackNumber 48

#endif  /* WINVER >= 0x400 */

#define RAS_MaxIpAddress      15
#define RAS_MaxIpxAddress     21

typedef HANDLE              HRASCONN;
typedef HRASCONN    FAR*    LPHRASCONN;

/* Identifies an active RAS connection.  (See RasEnumConnections)
*/

typedef struct tagRasConnW {
    DWORD    dwSize;
    HRASCONN hrasconn;
    WCHAR    szEntryName[ RAS_MaxEntryName + 1 ];

#if (WINVER >= 0x400)
    WCHAR    szDeviceType[ RAS_MaxDeviceType + 1 ];
    WCHAR    szDeviceName[ RAS_MaxDeviceName + 1 ];
#endif  /* WINVER >= 0x400 */

}   RASCONNW, FAR* LPRASCONNW;

typedef struct tagRasConnA {
    DWORD    dwSize;
    HRASCONN hrasconn;
    char     szEntryName[ RAS_MaxEntryName + 1 ];

#if (WINVER >= 0x400)
    char     szDeviceType[ RAS_MaxDeviceType + 1 ];
    char     szDeviceName[ RAS_MaxDeviceName + 1 ];
#endif  /* WINVER >= 0x400 */

}   RASCONNA, FAR* LPRASCONNA;

#ifdef UNICODE
typedef RASCONNW    RASCONN;
typedef LPRASCONNW  LPRASCONN;
#else  /* UNICODE */
typedef RASCONNA    RASCONN;
typedef LPRASCONNA  LPRASCONN;
#endif /* UNICODE */


/* Enumerates intermediate states to a connection.  (See RasDial)
*/
#define RASCS_PAUSED 0x1000
#define RASCS_DONE   0x2000

typedef enum
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
    RASCS_Projected,

#if (WINVER >= 0x400)
    RASCS_StartAuthentication,
    RASCS_SendCallback,
    RASCS_CallbackComplete,
#endif  /* WINVER >= 0x400 */

    RASCS_Interactive = RASCS_PAUSED,
    RASCS_RetryAuthentication,
    RASCS_CallbackSetByCaller,
    RASCS_PasswordExpired,

    RASCS_Connected = RASCS_DONE,
    RASCS_Disconnected,

#if (WINVER >= 0x400)
    RASCS_StartClosing,
    RASCS_Closing,
    RASCS_Closed
#endif  /* WINVER >= 0x400 */

}   RASCONNSTATE, FAR* LPCONNSTATE;

/* Describes the status of a RAS connection.  (See RasConnectionStatus)
*/

typedef struct tagRasConnStatusW {
    DWORD        dwSize;
    RASCONNSTATE rasconnstate;
    DWORD        dwError;
    WCHAR        szDeviceType[ RAS_MaxDeviceType + 1 ];
    WCHAR        szDeviceName[ RAS_MaxDeviceName + 1 ];
}   RASCONNSTATUSW, FAR* LPRASCONNSTATUSW;

typedef struct tagRasConnStatusA {
    DWORD        dwSize;
    RASCONNSTATE rasconnstate;
    DWORD        dwError;
    char         szDeviceType[ RAS_MaxDeviceType + 1 ];
    char         szDeviceName[ RAS_MaxDeviceName + 1 ];
}   RASCONNSTATUSA, FAR* LPRASCONNSTATUSA;

#ifdef UNICODE
typedef RASCONNSTATUSW   RASCONNSTATUS;
typedef LPRASCONNSTATUSW LPRASCONNSTATUS;
#else  /* UNICODE */
typedef RASCONNSTATUSA   RASCONNSTATUS;
typedef LPRASCONNSTATUSA LPRASCONNSTATUS;
#endif /* UNICODE */

/* Describes connection establishment parameters.  (See RasDial)
*/
typedef struct  tagRasDialParamsW  {
    DWORD   dwSize;
    WCHAR   szEntryName[ RAS_MaxEntryName + 1 ];
    WCHAR   szPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
    WCHAR   szCallbackNumber[ RAS_MaxCallbackNumber + 1 ];
    WCHAR   szUserName[ UNLEN + 1 ];
    WCHAR   szPassword[ PWLEN + 1 ];
    WCHAR   szDomain[ DNLEN + 1 ];
}   RASDIALPARAMSW, FAR* LPRASDIALPARAMSW;

typedef struct  tagRasDialParamsA  {
    DWORD   dwSize;
    char    szEntryName[ RAS_MaxEntryName + 1 ];
    char    szPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
    char    szCallbackNumber[ RAS_MaxCallbackNumber + 1 ];
    char    szUserName[ UNLEN + 1 ];
    char    szPassword[ PWLEN + 1 ];
    char    szDomain[ DNLEN + 1 ];
}   RASDIALPARAMSA, FAR* LPRASDIALPARAMSA;

#ifdef UNICODE
typedef RASDIALPARAMSW   RASDIALPARAMS;
typedef LPRASDIALPARAMSW LPRASDIALPARAMS;
#else  /* UNICODE */
typedef RASDIALPARAMSA   RASDIALPARAMS;
typedef LPRASDIALPARAMSA LPRASDIALPARAMS;
#endif /* UNICODE */

/* Describes extended connection establishment options.  (See RasDial)
*/
typedef struct  tagRasDialExtensions {
    DWORD   dwSize;
    DWORD   dwfOptions;
    HWND    hwndParent;
    DWORD   reserved;
}   RASDIALEXTENSIONS, FAR* LPRASDIALEXTENSIONS;

/* 'dwfOptions' bit flags.
*/
#define RDEOPT_UsePrefixSuffix           0x00000001
#define RDEOPT_PausedStates              0x00000002
#define RDEOPT_IgnoreModemSpeaker        0x00000004
#define RDEOPT_SetModemSpeaker           0x00000008
#define RDEOPT_IgnoreSoftwareCompression 0x00000010
#define RDEOPT_SetSoftwareCompression    0x00000020

/* Describes an enumerated RAS phone book entry name.  (See RasEntryEnum)
*/
typedef struct  tagRasEntryNameW   {
    DWORD   dwSize;
    WCHAR   szEntryName[RAS_MaxEntryName+1];
}   RASENTRYNAMEW, FAR* LPRASENTRYNAMEW;

typedef struct  tagRasEntryNameA   {
    DWORD   dwSize;
    char    szEntryName[RAS_MaxEntryName+1];
}   RASENTRYNAMEA, FAR* LPRASENTRYNAMEA;

#ifdef UNICODE
typedef RASENTRYNAMEW    RASENTRYNAME;
typedef LPRASENTRYNAMEW  LPRASENTRYNAME;
#else  /* UNICODE */
typedef RASENTRYNAMEA    RASENTRYNAME;
typedef LPRASENTRYNAMEA  LPRASENTRYNAME;
#endif /* UNICODE */

/* Protocol code to projection data structure mapping.
*/
typedef enum tagRasProjection {
    RASP_Amb = 0x10000,
    RASP_PppNbf = 0x803F,
    RASP_PppIpx = 0x802B,
    RASP_PppIp = 0x8021
}   RASPROJECTION, FAR* LPRASPROJECTION;

/* Describes the result of a RAS AMB (Authentication Message Block)
** projection.  This protocol is used with NT 3.1 and OS/2 1.3 downlevel
** RAS servers.
*/
typedef struct tagRasAMBW {
    DWORD dwSize;
    DWORD dwError;
    WCHAR szNetBiosError[ NETBIOS_NAME_LEN + 1 ];
    BYTE  bLana;
}   RASAMBW, FAR* LPRASAMBW;

typedef struct tagRasAMBA {
    DWORD dwSize;
    DWORD dwError;
    char  szNetBiosError[ NETBIOS_NAME_LEN + 1 ];
    BYTE  bLana;
}   RASAMBA, FAR* LPRASAMBA;

#ifdef UNICODE
typedef RASAMBW   RASAMB;
typedef LPRASAMBW LPRASAMB;
#else  /* UNICODE */
typedef RASAMBA   RASAMB;
typedef LPRASAMBA LPRASAMB;
#endif /* UNICODE */

/* Describes the result of a PPP NBF (NetBEUI) projection.
*/
typedef struct tagRasPPPNBFW {
    DWORD dwSize;
    DWORD dwError;
    DWORD dwNetBiosError;
    WCHAR szNetBiosError[ NETBIOS_NAME_LEN + 1 ];
    WCHAR szWorkstationName[ NETBIOS_NAME_LEN + 1 ];
    BYTE  bLana;
}   RASPPPNBFW, FAR* LPRASPPPNBFW;

typedef struct tagRasPPPNBFA {
    DWORD dwSize;
    DWORD dwError;
    DWORD dwNetBiosError;
    char  szNetBiosError[ NETBIOS_NAME_LEN + 1 ];
    char  szWorkstationName[ NETBIOS_NAME_LEN + 1 ];
    BYTE  bLana;
}   RASPPPNBFA, FAR* LPRASPPPNBFA;

#ifdef UNICODE
typedef RASPPPNBFW   RASPPPNBF;
typedef LPRASPPPNBFW LPRASPPPNBF;
#else  /* UNICODE */
typedef RASPPPNBFA   RASPPPNBF;
typedef LPRASPPPNBFA LPRASPPPNBF;
#endif /* UNICODE */

/* Describes the results of a PPP IPX (Internetwork Packet Exchange)
** projection.
*/
typedef struct tagRasPPPIPXW {
    DWORD dwSize;
    DWORD dwError;
    WCHAR szIpxAddress[ RAS_MaxIpxAddress + 1 ];
}   RASPPPIPXW, FAR* LPRASPPPIPXW;


typedef struct tagRasPPPIPXA {
    DWORD dwSize;
    DWORD dwError;
    char  szIpxAddress[ RAS_MaxIpxAddress + 1 ];
}   RASPPPIPXA, FAR* LPRASPPPIPXA;

#ifdef UNICODE
typedef RASPPPIPXW   RASPPPIPX;
typedef LPRASPPPIPXW LPRASPPPIPX;
#else  /* UNICODE */
typedef RASPPPIPXA   RASPPPIPX;
typedef LPRASPPPIPXA LPRASPPPIPX;
#endif /* UNICODE */

/* Describes the results of an PPP IP (Internet) projection.
*/
typedef struct tagRasPPPIPW {
    DWORD dwSize;
    DWORD dwError;
    WCHAR szIpAddress[ RAS_MaxIpAddress + 1 ];
}   RASPPPIPW, FAR* LPRASPPPIPW;

typedef struct tagRasPPPIPA {
    DWORD dwSize;
    DWORD dwError;
    char  szIpAddress[ RAS_MaxIpAddress + 1 ];
}   RASPPPIPA, FAR* LPRASPPPIPA;

#ifdef UNICODE
typedef RASPPPIPW    RASPPPIP;
typedef LPRASPPPIPW  LPRASPPPIP;
#else  /* UNICODE */
typedef RASPPPIPA    RASPPPIP;
typedef LPRASPPPIPA  LPRASPPPIP;
#endif /* UNICODE */

/* Prototypes for caller's RasDial callback handler.  Arguments are the
** message ID (currently always WM_RASDIALEVENT), the current RASCONNSTATE and
** the error that has occurred (or 0 if none).  Extended arguments are the
** handle of the RAS connection and an extended error code.
*/
#define WM_RASDIALEVENT 0xCCCD
typedef VOID (WINAPI *RASDIALFUNC)( UINT, RASCONNSTATE, DWORD );
typedef VOID (WINAPI *RASDIALFUNC1)( HRASCONN, UINT, RASCONNSTATE, DWORD, DWORD );


/* External RAS API function prototypes.
*/
DWORD APIENTRY RasDialA( LPRASDIALEXTENSIONS, LPSTR, LPRASDIALPARAMSA, DWORD,
                   LPVOID, LPHRASCONN );

DWORD APIENTRY RasDialW( LPRASDIALEXTENSIONS, LPWSTR, LPRASDIALPARAMSW, DWORD,
                   LPVOID, LPHRASCONN );

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

DWORD APIENTRY RasGetProjectionInfoA( HRASCONN, RASPROJECTION, LPVOID,
                   LPDWORD );

DWORD APIENTRY RasGetProjectionInfoW( HRASCONN, RASPROJECTION, LPVOID,
                   LPDWORD );

DWORD APIENTRY RasCreatePhonebookEntryA ( HWND, LPSTR );

DWORD APIENTRY RasCreatePhonebookEntryW ( HWND, LPWSTR );

DWORD APIENTRY RasEditPhonebookEntryA ( HWND, LPSTR, LPSTR );

DWORD APIENTRY RasEditPhonebookEntryW ( HWND, LPWSTR, LPWSTR );

DWORD APIENTRY RasSetEntryDialParamsA (LPSTR, LPRASDIALPARAMSA, BOOL);

DWORD APIENTRY RasSetEntryDialParamsW (LPWSTR, LPRASDIALPARAMSW, BOOL);

DWORD APIENTRY RasGetEntryDialParamsA (LPSTR, LPRASDIALPARAMSA, LPBOOL);

DWORD APIENTRY RasGetEntryDialParamsW (LPWSTR, LPRASDIALPARAMSW, LPBOOL);

#ifdef UNICODE
#define RasDial                 RasDialW
#define RasEnumConnections      RasEnumConnectionsW
#define RasEnumEntries          RasEnumEntriesW
#define RasGetConnectStatus     RasGetConnectStatusW
#define RasGetErrorString       RasGetErrorStringW
#define RasHangUp               RasHangUpW
#define RasGetProjectionInfo    RasGetProjectionInfoW
#define RasCreatePhonebookEntry RasCreatePhonebookEntryW
#define RasEditPhonebookEntry   RasEditPhonebookEntryW
#define RasSetEntryDialParams   RasSetEntryDialParamsW
#define RasGetEntryDialParams   RasGetEntryDialParamsW
#else  /* UNICODE */
#define RasDial                 RasDialA
#define RasEnumConnections      RasEnumConnectionsA
#define RasEnumEntries          RasEnumEntriesA
#define RasGetConnectStatus     RasGetConnectStatusA
#define RasGetErrorString       RasGetErrorStringA
#define RasHangUp               RasHangUpA
#define RasGetProjectionInfo    RasGetProjectionInfoA
#define RasCreatePhonebookEntry RasCreatePhonebookEntryA
#define RasEditPhonebookEntry   RasEditPhonebookEntryA
#define RasSetEntryDialParams   RasSetEntryDialParamsA
#define RasGetEntryDialParams   RasGetEntryDialParamsA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif // _RAS_H_
