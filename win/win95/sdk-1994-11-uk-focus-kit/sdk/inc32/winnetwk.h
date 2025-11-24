/*++

Copyright (c) 1991-1995  Microsoft Corporation

Module Name:

    winnetwk.h

Abstract:

    Standard WINNET Header File for WIN32

Environment:

    User Mode -Win32

Notes:

    optional-notes

Revision History:

    08-Oct-1991     danl
	created from winnet 3.10.05 version.

    10-Dec-1991     Johnl
	Updated to conform to Win32 Net API Spec. vers 0.4

    01-Apr-1992     JohnL
	Changed CONNECTION_REMEMBERED flag to CONNECT_UPDATE_PROFILE
	Updated WNetCancelConnection2 to match spec.

    23-Apr-1992     Johnl
	Added error code mappings.  Changed byte counts to character counts.

    27-May-1992     ChuckC
	Made into .x file.

	18-Aug-1993     LenS
	Added Chicago extensions.

--*/

#ifndef _WINNETWK_
#define _WINNETWK_

#ifdef __cplusplus
extern "C" {
#endif

#define MNRENTRY    DWORD FAR PASCAL

//
// RESOURCE ENUMERATION
//

#define RESOURCE_CONNECTED      0x00000001
#define RESOURCE_GLOBALNET      0x00000002
#define RESOURCE_REMEMBERED     0x00000003
#define RESOURCE_RECENT         0x00000004
#define RESOURCE_CONTEXT        0x00000005

#define RESOURCETYPE_ANY        0x00000000  /* Note that this is not a bit mask value */
#define RESOURCETYPE_DISK       0x00000001
#define RESOURCETYPE_PRINT      0x00000002
#define RESOURCETYPE_RESERVED   0x00000004  /* Reserved for a bit mask version of RESOURCETYPE_ANY */

#define RESOURCEUSAGE_CONNECTABLE   0x00000001
#define RESOURCEUSAGE_CONTAINER     0x00000002
#define RESOURCEUSAGE_NOLOCALDEVICE 0x00000004
#define RESOURCEUSAGE_SIBLING       0x00000008
#define RESOURCEUSAGE_RESERVED      0x80000000
#define RESOURCEUSAGE_ALL           (RESOURCEUSAGE_CONNECTABLE | RESOURCEUSAGE_CONTAINER)


#define RESOURCEDISPLAYTYPE_GENERIC        0x00000000
#define RESOURCEDISPLAYTYPE_DOMAIN         0x00000001
#define RESOURCEDISPLAYTYPE_SERVER         0x00000002
#define RESOURCEDISPLAYTYPE_SHARE          0x00000003
#define RESOURCEDISPLAYTYPE_SHAREADMIN     0x00000004
#define RESOURCEDISPLAYTYPE_DIRECTORY      0x00000005
#define RESOURCEDISPLAYTYPE_NETWORK        0x00000006
#define RESOURCEDISPLAYTYPE_ROOT           0x00000007

typedef struct  _NETRESOURCEA {
    DWORD    dwScope;
    DWORD    dwType;
    DWORD    dwDisplayType;
    DWORD    dwUsage;
    LPSTR    lpLocalName;
    LPSTR    lpRemoteName;
    LPSTR    lpComment ;
    LPSTR    lpProvider;
}NETRESOURCEA, *LPNETRESOURCEA;
typedef struct  _NETRESOURCEW {
    DWORD    dwScope;
    DWORD    dwType;
    DWORD    dwDisplayType;
    DWORD    dwUsage;
    LPWSTR   lpLocalName;
    LPWSTR   lpRemoteName;
    LPWSTR   lpComment ;
    LPWSTR   lpProvider;
}NETRESOURCEW, *LPNETRESOURCEW;
#ifdef UNICODE
typedef NETRESOURCEW NETRESOURCE;
typedef LPNETRESOURCEW LPNETRESOURCE;
#else
typedef NETRESOURCEA NETRESOURCE;
typedef LPNETRESOURCEA LPNETRESOURCE;
#endif // UNICODE

MNRENTRY
WNetOpenEnumA (
     DWORD          dwScope,
     DWORD          dwType,
     DWORD          dwUsage,
     LPNETRESOURCEA lpNetResource,
     LPHANDLE       lphEnum
    );
MNRENTRY
WNetOpenEnumW (
     DWORD          dwScope,
     DWORD          dwType,
     DWORD          dwUsage,
     LPNETRESOURCEW lpNetResource,
     LPHANDLE       lphEnum
    );
#ifdef UNICODE
#define WNetOpenEnum  WNetOpenEnumW
#else
#define WNetOpenEnum  WNetOpenEnumA
#endif // UNICODE

MNRENTRY
WNetEnumResourceA (
     HANDLE  hEnum,
     LPDWORD lpcCount,
     LPVOID  lpBuffer,
     LPDWORD lpBufferSize
    );
MNRENTRY
WNetEnumResourceW (
     HANDLE  hEnum,
     LPDWORD lpcCount,
     LPVOID  lpBuffer,
     LPDWORD lpBufferSize
    );
#ifdef UNICODE
#define WNetEnumResource  WNetEnumResourceW
#else
#define WNetEnumResource  WNetEnumResourceA
#endif // UNICODE

MNRENTRY
WNetCloseEnum (
    HANDLE   hEnum
    );

#define WNFMT_MULTILINE         0x01
#define WNFMT_ABBREVIATED       0x02
#define WNFMT_INENUM            0x10

MNRENTRY
WNetFormatNetworkNameA (
    LPSTR   lpProvider,
    LPSTR   lpRemoteName,
    LPSTR   lpFormattedName,
    LPDWORD lpnLength,
    DWORD   dwFlags,
    DWORD   dwAveCharPerLine
    );
MNRENTRY
WNetFormatNetworkNameW (
    LPWSTR  lpProvider,
    LPWSTR  lpRemoteName,
    LPWSTR  lpFormattedName,
    LPDWORD lpnLength,
    DWORD   dwFlags,
    DWORD   dwAveCharPerLine
    );
#ifdef UNICODE
#define WNetFormatNetworkName  WNetFormatNetworkNameW
#else
#define WNetFormatNetworkName  WNetFormatNetworkNameA
#endif // UNICODE

MNRENTRY
WNetGetResourceParentA (
    LPNETRESOURCEA lpNetResource,
    LPVOID         lpBuffer,
    LPDWORD        cbBuffer
    );
MNRENTRY
WNetGetResourceParentW (
    LPNETRESOURCEW lpNetResource,
    LPVOID         lpBuffer,
    LPDWORD        cbBuffer
    );
#ifdef UNICODE
#define WNetGetResourceParent  WNetGetResourceParentW
#else
#define WNetGetResourceParent  WNetGetResourceParentA
#endif // UNICODE

MNRENTRY
WNetGetResourceInformationA (
	LPNETRESOURCEA lpNetResource,
	LPVOID lpBuffer,
	LPDWORD cbBuffer,
	LPSTR *lplpSystem
    );
MNRENTRY
WNetGetResourceInformationW (
	LPNETRESOURCEW lpNetResource,
	LPVOID lpBuffer,
	LPDWORD cbBuffer,
	LPWSTR *lplpSystem
    );
#ifdef UNICODE
#define WNetGetResourceInformation  WNetGetResourceInformationW
#else
#define WNetGetResourceInformation  WNetGetResourceInformationA
#endif // UNICODE

//
//  CONNECTIONS
//

#define NETPROPERTY_PERSISTENT      1
#define CONNECT_UPDATE_PROFILE      0x00000001
#define CONNECT_UPDATE_RECENT       0x00000002
#define CONNECT_TEMPORARY           0x00000004
#define CONNECT_INTERACTIVE         0x00000008
#define CONNECT_PROMPT              0x00000010
#define CONNECT_NEED_DRIVE          0x00000020
#define CONNECT_REFCOUNT            0x00000040
#define CONNECT_REDIRECT            0x00000080
#define CONNECT_LOCALDRIVE          0x00000100

MNRENTRY
WNetAddConnectionA (
    LPSTR   lpRemoteName,
    LPSTR   lpPassword,
    LPSTR   lpLocalName
    );
MNRENTRY
WNetAddConnectionW (
    LPWSTR   lpRemoteName,
    LPWSTR   lpPassword,
    LPWSTR   lpLocalName
    );
#ifdef UNICODE
#define WNetAddConnection  WNetAddConnectionW
#else
#define WNetAddConnection  WNetAddConnectionA
#endif // UNICODE

MNRENTRY
WNetAddConnection2A (
    LPNETRESOURCEA lpNetResource,
    LPSTR          lpPassword,
    LPSTR          lpUserName,
    DWORD          dwFlags
    );
MNRENTRY
WNetAddConnection2W (
     LPNETRESOURCEW lpNetResource,
     LPWSTR         lpPassword,
     LPWSTR         lpUserName,
     DWORD          dwFlags
    );
#ifdef UNICODE
#define WNetAddConnection2  WNetAddConnection2W
#else
#define WNetAddConnection2  WNetAddConnection2A
#endif // UNICODE

MNRENTRY
WNetAddConnection3A (
    HWND            hwndOwner,
    LPNETRESOURCEA  lpNetResource,
    LPSTR           lpUserID,
    LPSTR           lpPassword,
    DWORD           dwFlags
    );
MNRENTRY
WNetAddConnection3W (
    HWND            hwndOwner,
    LPNETRESOURCEW  lpNetResource,
    LPWSTR          lpUserID,
    LPWSTR          lpPassword,
    DWORD           dwFlags
    );
#ifdef UNICODE
#define WNetAddConnection3  WNetAddConnection3W
#else
#define WNetAddConnection3  WNetAddConnection3A
#endif // UNICODE

MNRENTRY
WNetUseConnectionA (
    HWND            hwndOwner,
    LPNETRESOURCEA  lpNetResource,
    LPSTR           lpUserID,
    LPSTR           lpPassword,
    DWORD           dwFlags,
	LPSTR           lpAccessName,
	LPDWORD         lpBufferSize,
	LPDWORD         lpResult
    );
MNRENTRY
WNetUseConnectionW (
    HWND            hwndOwner,
    LPNETRESOURCEW  lpNetResource,
    LPWSTR          lpUserID,
    LPWSTR          lpPassword,
    DWORD           dwFlags,
	LPWSTR          lpAccessName,
	LPDWORD         lpBufferSize,
	LPDWORD         lpResult
    );
#ifdef UNICODE
#define WNetUseConnection  WNetUseConnectionW
#else
#define WNetUseConnection  WNetUseConnectionA
#endif // UNICODE

MNRENTRY
WNetCancelConnectionA (
     LPSTR   lpName,
     BOOL    fForce
    );
MNRENTRY
WNetCancelConnectionW (
     LPWSTR  lpName,
     BOOL    fForce
    );
#ifdef UNICODE
#define WNetCancelConnection  WNetCancelConnectionW
#else
#define WNetCancelConnection  WNetCancelConnectionA
#endif // UNICODE

MNRENTRY
WNetCancelConnection2A (
     LPSTR   lpName,
     DWORD   dwFlags,
     BOOL    fForce
    );
MNRENTRY
WNetCancelConnection2W (
     LPWSTR  lpName,
     DWORD   dwFlags,
     BOOL    fForce
    );
#ifdef UNICODE
#define WNetCancelConnection2  WNetCancelConnection2W
#else
#define WNetCancelConnection2  WNetCancelConnection2A
#endif // UNICODE

MNRENTRY
WNetGetConnectionA (
     LPSTR   lpLocalName,
     LPSTR   lpRemoteName,
     LPDWORD lpnLength
    );
MNRENTRY
WNetGetConnectionW (
     LPWSTR  lpLocalName,
     LPWSTR  lpRemoteName,
     LPDWORD lpnLength
    );
#ifdef UNICODE
#define WNetGetConnection  WNetGetConnectionW
#else
#define WNetGetConnection  WNetGetConnectionA
#endif // UNICODE

MNRENTRY
WNetSetConnectionA (
    LPSTR   lpName,
    DWORD   dwProperties,
    LPVOID  pvValue
    );
MNRENTRY
WNetSetConnectionW (
    LPWSTR  lpName,
    DWORD   dwProperties,
    LPVOID  pvValue
    );
#ifdef UNICODE
#define WNetSetConnection  WNetSetConnectionW
#else
#define WNetSetConnection  WNetSetConnectionA
#endif // UNICODE

typedef struct _NETCONNECTINFOSTRUCT {
    DWORD cbStructure;
    DWORD dwFlags;
    DWORD dwSpeed;	
    DWORD dwDelay;
    DWORD dwOptDataSize;
} NETCONNECTINFOSTRUCT,  *LPNETCONNECTINFOSTRUCT;

#define WNCON_FORNETCARD        0x00000001
#define WNCON_NOTROUTED         0x00000002
#define WNCON_SLOWLINK          0x00000004
#define WNCON_DYNAMIC           0x00000008

#define WNMedium802_3           1
#define WNMedium802_5           2
#define WNMediumFddi            3
#define WNMediumLocalTalk       4
#define WNMediumDIX             5

MNRENTRY
MultinetGetConnectionPerformanceA(
	LPNETRESOURCEA         lpNetResource,
	LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct
	);
MNRENTRY
MultinetGetConnectionPerformanceW(
	LPNETRESOURCEW         lpNetResource,
	LPNETCONNECTINFOSTRUCT lpNetConnectInfoStruct
	);
#ifdef UNICODE
#define MultinetGetConnectionPerformance  MultinetGetConnectionPerformanceW
#else
#define MultinetGetConnectionPerformance  MultinetGetConnectionPerformanceA
#endif // UNICODE

MNRENTRY
MultinetGetErrorTextA(
    LPSTR lpErrorTextBuf,
    LPDWORD lpnErrorBufSize,
    LPSTR lpProviderNameBuf,
    LPDWORD lpnNameBufSize
	);
MNRENTRY
MultinetGetErrorTextW(
    LPWSTR lpErrorTextBuf,
    LPDWORD nErrorBufSize,
    LPWSTR lpProviderNameBuf,
    LPDWORD lpnNameBufSize
	);
#ifdef UNICODE
#define MultinetGetErrorText  MultinetGetErrorTextW
#else
#define MultinetGetErrorText  MultinetGetErrorTextA
#endif // UNICODE

//
//  CONNECTION DIALOGS
//

typedef struct _CONNECTDLGSTRUCTA {
    DWORD cbStructure;       /* size of this structure in bytes */
    HWND hwndOwner;          /* owner window for the dialog */
    LPNETRESOURCEA lpConnRes;/* Requested Resource info    */
    DWORD dwFlags;           /* flags (see below) */
    DWORD dwDevNum;          /* number of device connected to */
} CONNECTDLGSTRUCTA, FAR *LPCONNECTDLGSTRUCTA;
typedef struct _CONNECTDLGSTRUCTW {
    DWORD cbStructure;       /* size of this structure in bytes */
    HWND hwndOwner;          /* owner window for the dialog */
    LPNETRESOURCEW lpConnRes;/* Requested Resource info    */
    DWORD dwFlags;           /* flags (see below) */
    DWORD dwDevNum;          /* number of device connected to */
} CONNECTDLGSTRUCTW, FAR *LPCONNECTDLGSTRUCTW;
#ifdef UNICODE
typedef CONNECTDLGSTRUCTW CONNECTDLGSTRUCT;
typedef LPCONNECTDLGSTRUCTW LPCONNECTDLGSTRUCT;
#else
typedef CONNECTDLGSTRUCTA CONNECTDLGSTRUCT;
typedef LPCONNECTDLGSTRUCTA LPCONNECTDLGSTRUCT;
#endif // UNICODE

#define CONNDLG_RO_PATH     0x00000001 /* Resource path should be read-only    */
#define CONNDLG_CONN_POINT  0x00000002 /* Netware -style movable connection point enabled */
#define CONNDLG_USE_MRU     0x00000004 /* Use MRU combobox  */
#define CONNDLG_HIDE_BOX    0x00000008 /* Hide persistent connect checkbox */

/*
 * NOTE:  Set at most ONE of the below flags.  If neither flag is set, 
 *        then the persistence is set to whatever the user chose during
 *        a previous connection
 */         
#define CONNDLG_PERSIST     0x00000010 /* Force persistent connection */
#define CONNDLG_NOT_PERSIST 0x00000020 /* Force connection NOT persistent */

MNRENTRY
WNetConnectionDialog (
    HWND  hwnd,
    DWORD dwType
    );

MNRENTRY
WNetConnectionDialog1A (
    LPCONNECTDLGSTRUCTA lpConnDlgStruct
	);
MNRENTRY
WNetConnectionDialog1W (
    LPCONNECTDLGSTRUCTW lpConnDlgStruct
	);
#ifdef UNICODE
#define WNetConnectionDialog1  WNetConnectionDialog1W
#else
#define WNetConnectionDialog1  WNetConnectionDialog1A
#endif // UNICODE
	
MNRENTRY
WNetAddConnectionDialogA (
    HWND    hParent,
    LPSTR   lpszRemoteName,
    DWORD   dwType
    );
MNRENTRY
WNetAddConnectionDialogW (
    HWND    hParent,
    LPWSTR  lpszRemoteName,
    DWORD   dwType
    );
#ifdef UNICODE
#define WNetAddConnectionDialog  WNetAddConnectionDialogW
#else
#define WNetAddConnectionDialog  WNetAddConnectionDialogA
#endif // UNICODE

MNRENTRY
WNetRestoreConnectionA (
    HWND    hwndParent,
    LPCSTR  lpDevice
    );
MNRENTRY
WNetRestoreConnectionW (
    HWND    hwndParent,
    LPCWSTR lpDevice
    );
#ifdef UNICODE
#define WNetRestoreConnection  WNetRestoreConnectionW
#else
#define WNetRestoreConnection  WNetRestoreConnectionA
#endif // UNICODE

MNRENTRY
WNetDisconnectDialog (
    HWND  hwnd,
    DWORD dwType
    );

typedef struct _DISCDLGSTRUCTA {
    DWORD           cbStructure;      /* size of this structure in bytes */
    HWND            hwndOwner;        /* owner window for the dialog */
    LPSTR           lpLocalName;      /* local device name */
    LPSTR           lpRemoteName;     /* network resource name */
    DWORD           dwFlags;          /* flags */
} DISCDLGSTRUCTA, FAR *LPDISCDLGSTRUCTA;
typedef struct _DISCDLGSTRUCTW {
    DWORD           cbStructure;      /* size of this structure in bytes */
    HWND            hwndOwner;        /* owner window for the dialog */
    LPWSTR          lpLocalName;      /* local device name */
    LPWSTR          lpRemoteName;     /* network resource name */
    DWORD           dwFlags;          /* flags */
} DISCDLGSTRUCTW, FAR *LPDISCDLGSTRUCTW;
#ifdef UNICODE
typedef DISCDLGSTRUCTW DISCDLGSTRUCT;
typedef LPDISCDLGSTRUCTW LPDISCDLGSTRUCT;
#else
typedef DISCDLGSTRUCTA DISCDLGSTRUCT;
typedef LPDISCDLGSTRUCTA LPDISCDLGSTRUCT;
#endif // UNICODE

#define DISC_UPDATE_PROFILE         0x00000001
#define DISC_NO_FORCE               0x00000040

MNRENTRY
WNetDisconnectDialog1A (
    LPDISCDLGSTRUCTA lpConnDlgStruct
	);
MNRENTRY
WNetDisconnectDialog1W (
    LPDISCDLGSTRUCTW lpConnDlgStruct
	);
#ifdef UNICODE
#define WNetDisconnectDialog1  WNetDisconnectDialog1W
#else
#define WNetDisconnectDialog1  WNetDisconnectDialog1A
#endif // UNICODE

//
//  ERRORS
//

MNRENTRY
WNetGetLastErrorA (
     LPDWORD  lpError,
     LPSTR    lpErrorBuf,
     DWORD    nErrorBufSize,
     LPSTR    lpNameBuf,
     DWORD    nNameBufSize
    );
MNRENTRY
WNetGetLastErrorW (
     LPDWORD  lpError,
     LPWSTR   lpErrorBuf,
     DWORD    nErrorBufSize,
     LPWSTR   lpNameBuf,
     DWORD    nNameBufSize
    );
#ifdef UNICODE
#define WNetGetLastError  WNetGetLastErrorW
#else
#define WNetGetLastError  WNetGetLastErrorA
#endif // UNICODE

//
// PASSWORD CACHE.
//

#ifndef PCE_STRUCT_DEFINED

#define PCE_STRUCT_DEFINED      /* for benefit of pcache.h */

struct PASSWORD_CACHE_ENTRY {
    WORD cbEntry;               /* size of this entry in bytes, incl. pad */
    WORD cbResource;            /* size of resource name in bytes */
    WORD cbPassword;            /* size of password in bytes */
    BYTE iEntry;                /* index number of this entry, for MRU */
    BYTE nType;                 /* type of entry (see below) */
    char abResource[1];         /* resource name (may not be ASCIIZ at all) */
};

#define PCE_MEMORYONLY		0x01	/* for flags field when adding */

/*
    Typedef for the callback routine passed to the enumeration functions.
    It will be called once for each entry that matches the criteria
    requested.  It returns TRUE if it wants the enumeration to
    continue, FALSE to stop.
*/
typedef BOOL (FAR PASCAL *CACHECALLBACK)( struct PASSWORD_CACHE_ENTRY FAR *pce, DWORD dwRefData );

#endif  /* PCE_STRUCT_DEFINED */

MNRENTRY
WNetCachePassword (
    LPSTR pbResource,
    WORD  cbResource,
    LPSTR pbPassword,
    WORD  cbPassword,
    BYTE  nType,
    UINT  fnFlags
    );

MNRENTRY
WNetGetCachedPassword (
    LPSTR  pbResource,
    WORD   cbResource,
    LPSTR  pbPassword,
    LPWORD pcbPassword,
    BYTE   nType
    );

MNRENTRY
WNetRemoveCachedPassword (
    LPSTR pbResource,
    WORD  cbResource,
    BYTE  nType
    );

MNRENTRY
WNetEnumCachedPasswords (
    LPSTR pbPrefix,
    WORD  cbPrefix,
    BYTE  nType,
    CACHECALLBACK pfnCallback,
	DWORD dwRefData
    );

//
// AUTHENTICATION AND LOGON/LOGOFF
//

#define LOGON_DONE          0x00000001
#define LOGON_PRIMARY       0x00000002
#define LOGON_MUST_VALIDATE 0x00000004

#define LOGOFF_PENDING  1
#define LOGOFF_COMMIT   2
#define LOGOFF_CANCEL   3

MNRENTRY
WNetLogonA (
    LPCSTR lpProvider,
    HWND hwndOwner
    );
MNRENTRY
WNetLogonW (
    LPCWSTR lpProvider,
    HWND hwndOwner
    );
#ifdef UNICODE
#define WNetLogon  WNetLogonW
#else
#define WNetLogon  WNetLogonA
#endif // UNICODE

MNRENTRY
WNetGetHomeDirectoryA (
    LPCSTR  lpProviderName,
    LPSTR   lpDirectory,
    LPDWORD lpBufferSize
    );
MNRENTRY
WNetGetHomeDirectoryW (
    LPCWSTR lpProviderName,
    LPWSTR  lpDirectory,
    LPDWORD lpBufferSize
    );
#ifdef UNICODE
#define WNetGetHomeDirectory  WNetGetHomeDirectoryW
#else
#define WNetGetHomeDirectory  WNetGetHomeDirectoryA
#endif // UNICODE

MNRENTRY
WNetVerifyPasswordA (
    LPCSTR     lpszPassword,
    BOOL FAR * pfMatch
    );
MNRENTRY
WNetVerifyPasswordW (
    LPCWSTR    lpszPassword,
    BOOL FAR * pfMatch
    );
#ifdef UNICODE
#define WNetVerifyPassword  WNetVerifyPasswordW
#else
#define WNetVerifyPassword  WNetVerifyPasswordA
#endif // UNICODE

//
//  MISCELLANEOUS
//

#define UNIVERSAL_NAME_INFO_LEVEL	1
#define REMOTE_NAME_INFO_LEVEL		2

typedef struct _UNIVERSAL_NAME_INFOA {
	LPSTR lpszUniversalName;
} UNIVERSAL_NAME_INFOA, *LPUNIVERSAL_NAME_INFOA;
typedef struct _UNIVERSAL_NAME_INFOW {
	LPWSTR lpszUniversalName;
} UNIVERSAL_NAME_INFOW, *LPUNIVERSAL_NAME_INFOW;
#ifdef UNICODE
typedef UNIVERSAL_NAME_INFOW UNIVERSAL_NAME_INFO;
typedef LPUNIVERSAL_NAME_INFOW LPUNIVERSAL_NAME_INFO;
#else
typedef UNIVERSAL_NAME_INFOA UNIVERSAL_NAME_INFO;
typedef LPUNIVERSAL_NAME_INFOA LPUNIVERSAL_NAME_INFO;
#endif // UNICODE

typedef struct _REMOTE_NAME_INFOA {
	LPSTR lpszUniversalName;
	LPSTR lpszConnectionName;
	LPSTR lpszRemainingPath;
} REMOTE_NAME_INFOA, *LPREMOTE_NAME_INFOA;
typedef struct _REMOTE_NAME_INFOW {
	LPWSTR lpszUniversalName;
	LPWSTR lpszConnectionName;
	LPWSTR lpszRemainingPath;
} REMOTE_NAME_INFOW, *LPREMOTE_NAME_INFOW;
#ifdef UNICODE
typedef REMOTE_NAME_INFOW REMOTE_NAME_INFO;
typedef LPREMOTE_NAME_INFOW LPREMOTE_NAME_INFO;
#else
typedef REMOTE_NAME_INFOA REMOTE_NAME_INFO;
typedef LPREMOTE_NAME_INFOA LPREMOTE_NAME_INFO;
#endif // UNICODE

MNRENTRY
WNetGetUniversalNameA (
	 LPSTR    lpLocalPath,
	 DWORD    dwInfoLevel,
	 LPVOID   lpBuffer,
	 LPDWORD  lpBufferSize
    );
MNRENTRY
WNetGetUniversalNameW (
	 LPWSTR   lpLocalPath,
	 DWORD    dwInfoLevel,
	 LPVOID   lpBuffer,
	 LPDWORD  lpBufferSize
    );
#ifdef UNICODE
#define WNetGetUniversalName  WNetGetUniversalNameW
#else
#define WNetGetUniversalName  WNetGetUniversalNameA
#endif // UNICODE

MNRENTRY
WNetGetUserA (
     LPSTR    lpName,
     LPSTR    lpUserName,
     LPDWORD  lpnLength
    );
MNRENTRY
WNetGetUserW (
     LPWSTR   lpName,
     LPWSTR   lpUserName,
     LPDWORD  lpnLength
    );
#ifdef UNICODE
#define WNetGetUser  WNetGetUserW
#else
#define WNetGetUser  WNetGetUserA
#endif // UNICODE

typedef struct _NETINFOSTRUCT{
    DWORD cbStructure;
    DWORD dwProviderVersion;
    DWORD dwStatus;
    DWORD dwCharacteristics;
    DWORD dwHandle;
    WORD  wNetType;
    DWORD dwPrinters;
    DWORD dwDrives;
} NETINFOSTRUCT, FAR *LPNETINFOSTRUCT;

#define NETINFO_DLL16       0x00000001  /* Provider running as 16 bit Winnet Driver */
#define NETINFO_DISKRED	    0x00000004  /* Provider requires disk redirections to connect*/
#define NETINFO_PRINTERRED	0x00000008  /* Provider requires printer redirections to connect */

MNRENTRY
WNetGetNetworkInformationA (
    LPSTR           lpProvider,
    LPNETINFOSTRUCT lpNetInfoStruct
    );
MNRENTRY
WNetGetNetworkInformationW (
    LPWSTR          lpProvider,
    LPNETINFOSTRUCT lpNetInfoStruct
    );
#ifdef UNICODE
#define WNetGetNetworkInformation  WNetGetNetworkInformationW
#else
#define WNetGetNetworkInformation  WNetGetNetworkInformationA
#endif // UNICODE

MNRENTRY
WNetGetProviderNameA (
    DWORD   dwNetType,
    LPSTR   lpProviderName,
    LPDWORD lpBufferSize
    );
MNRENTRY
WNetGetProviderNameW (
    DWORD   dwNetType,
    LPWSTR  lpProviderName,
    LPDWORD lpBufferSize
    );
#ifdef UNICODE
#define WNetGetProviderName  WNetGetProviderNameW
#else
#define WNetGetProviderName  WNetGetProviderNameA
#endif // UNICODE

// Network types

#define     WNNC_NET_MSNET      0x00010000
#define     WNNC_NET_LANMAN     0x00020000
#define     WNNC_NET_NETWARE    0x00030000
#define     WNNC_NET_VINES      0x00040000
#define     WNNC_NET_10NET      0x00050000
#define     WNNC_NET_LOCUS      0x00060000
#define     WNNC_NET_SUN_PC_NFS 0x00070000
#define     WNNC_NET_LANSTEP    0x00080000
#define     WNNC_NET_9TILES     0x00090000
#define     WNNC_NET_LANTASTIC  0x000A0000
#define     WNNC_NET_AS400      0x000B0000
#define     WNNC_NET_FTP_NFS    0x000C0000
#define     WNNC_NET_PATHWORKS  0x000D0000
#define     WNNC_NET_LIFENET    0x000E0000
#define     WNNC_NET_POWERLAN   0x000F0000

//
// EXTENSIONS FOR SHELL
//



typedef UINT (FAR PASCAL *PFNGETPROFILEPATHA)(
	LPCSTR pszUsername,
	LPSTR  pszBuffer,
	UINT   cbBuffer
	);
typedef UINT (FAR PASCAL *PFNGETPROFILEPATHW)(
	LPCWSTR pszUsername,
	LPWSTR  pszBuffer,
	UINT    cbBuffer
	);
#ifdef UNICODE
typedef PFNGETPROFILEPATHW PFNGETPROFILEPATH;
#else
typedef PFNGETPROFILEPATHA PFNGETPROFILEPATH;
#endif // UNICODE

typedef UINT (FAR PASCAL *PFNRECONCILEPROFILEA)(
	LPCSTR pszCentralFile,
	LPCSTR pszLocalFile,
	DWORD  dwFlags
	);
typedef UINT (FAR PASCAL *PFNRECONCILEPROFILEW)(
	LPCWSTR pszCentralFile,
	LPCWSTR pszLocalFile,
	DWORD  dwFlags
	);
#ifdef UNICODE
typedef PFNRECONCILEPROFILEW PFNRECONCILEPROFILE;
#else
typedef PFNRECONCILEPROFILEA PFNRECONCILEPROFILE;
#endif // UNICODE

#define RP_LOGON	0x01		/* if set, do for logon, else for logoff */
#define RP_INIFILE	0x02		/* if set, reconcile .INI file, else reg. hive */

typedef BOOL (FAR PASCAL *PFNPROCESSPOLICIESA)(
	HWND   hwnd,
	LPCSTR pszPath,
	LPCSTR pszUsername,
	LPCSTR pszComputername,
	DWORD  dwFlags
	);
typedef BOOL (FAR PASCAL *PFNPROCESSPOLICIESW)(
	HWND	hwnd,
	LPCWSTR pszPath,
	LPCWSTR pszUsername,
	LPCWSTR pszComputername,
	DWORD  dwFlags
	);
#ifdef UNICODE
typedef PFNPROCESSPOLICIESW PFNPROCESSPOLICIES;
#else
typedef PFNPROCESSPOLICIESA PFNPROCESSPOLICIES;
#endif // UNICODE

// flags for ProcessPolicies
#define PP_DISPLAYERRORS	0x01	/* if set, display error messages, else fail silently if error */

//
//  STATUS CODES
//
//  This section is provided for backward compatibility.  Use of the ERROR_*
//  codes is preferred.  The WN_* error codes may not be available in future
//  releases.
//

// General

#define WN_SUCCESS             NO_ERROR
#define WN_NO_ERROR            NO_ERROR
#define WN_RETRY               ERROR_RETRY
#define WN_CANCEL              ERROR_CANCELLED
#define WN_CONTINUE            ERROR_CONTINUE
#define WN_NOT_SUPPORTED       ERROR_NOT_SUPPORTED
#define WN_NOT_INITIALIZING    ERROR_ALREADY_INITIALIZED
#define WN_NET_ERROR	       ERROR_UNEXP_NET_ERR
#define WN_MORE_DATA	       ERROR_MORE_DATA
#define WN_BAD_POINTER	       ERROR_INVALID_ADDRESS
#define WN_BAD_VALUE	       ERROR_INVALID_PARAMETER
#define WN_BAD_PASSWORD        ERROR_INVALID_PASSWORD
#define WN_ACCESS_DENIED       ERROR_ACCESS_DENIED
#define WN_FUNCTION_BUSY       ERROR_BUSY
#define WN_WINDOWS_ERROR       ERROR_UNEXP_NET_ERR
#define WN_OUT_OF_MEMORY       ERROR_NOT_ENOUGH_MEMORY
#define WN_NO_NETWORK	       ERROR_NO_NETWORK
#define WN_EXTENDED_ERROR      ERROR_EXTENDED_ERROR
#define WN_BAD_HANDLE	       ERROR_INVALID_HANDLE

// Connection

#define WN_NOT_CONNECTED       ERROR_NOT_CONNECTED
#define WN_OPEN_FILES          ERROR_OPEN_FILES
#define WN_DEVICE_IN_USE       ERROR_DEVICE_IN_USE
#define WN_BAD_NETNAME	       ERROR_BAD_NET_NAME
#define WN_BAD_LOCALNAME       ERROR_BAD_DEVICE
#define WN_ALREADY_CONNECTED   ERROR_ALREADY_ASSIGNED
#define WN_DEVICE_ERROR        ERROR_GEN_FAILURE
#define WN_CONNECTION_CLOSED   ERROR_CONNECTION_UNAVAIL
#define WN_NO_NET_OR_BAD_PATH  ERROR_NO_NET_OR_BAD_PATH
#define WN_BAD_PROVIDER        ERROR_BAD_PROVIDER
#define WN_CANNOT_OPEN_PROFILE ERROR_CANNOT_OPEN_PROFILE
#define WN_BAD_PROFILE	       ERROR_BAD_PROFILE
#define WN_NO_MORE_DEVICES     ERROR_NO_MORE_DEVICES 
#define WN_BAD_DEV_TYPE        ERROR_BAD_DEV_TYPE

// Enumeration

#define WN_NO_MORE_ENTRIES     ERROR_NO_MORE_ITEMS
#define WN_NOT_CONTAINER       ERROR_NOT_CONTAINER

// Authentication

#define WN_BAD_USER            ERROR_BAD_USERNAME
#define WN_NOT_AUTHENTICATED   ERROR_NOT_AUTHENTICATED
#define WN_NOT_LOGGED_ON       ERROR_NOT_LOGGED_ON
#define WN_NOT_VALIDATED       ERROR_NO_LOGON_SERVERS

#ifdef __cplusplus
}
#endif

#endif  // _WINNETWK_
