/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1991-1995 Microsoft Corporation

Module Name:

    Winreg.h

Abstract:

    This module contains the function prototypes and constant, type and
    structure definitions for the Windows 32-Bit Registry API.

--*/

#ifndef _WINREG_
#define _WINREG_


#ifdef __cplusplus
extern "C" {
#endif


//
// Requested Key access mask type.
//

typedef ACCESS_MASK REGSAM;

//
// Type definitions.
//

DECLARE_HANDLE(HKEY);
typedef HKEY *PHKEY;

//
// Reserved Key Handles.
//

#define HKEY_CLASSES_ROOT           (( HKEY ) 0x80000000 )
#define HKEY_CURRENT_USER           (( HKEY ) 0x80000001 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_USERS                  (( HKEY ) 0x80000003 )
#define HKEY_PERFORMANCE_DATA       (( HKEY ) 0x80000004 )
#if(WINVER >= 0x0400)
#define HKEY_CURRENT_CONFIG         (( HKEY ) 0x80000005 )
#define HKEY_DYN_DATA               (( HKEY ) 0x80000006 )

/*NOINC*/
#ifndef _PROVIDER_STRUCTS_DEFINED
#define _PROVIDER_STRUCTS_DEFINED

#define PROVIDER_KEEPS_VALUE_LENGTH 0x1
struct val_context {
    int valuelen;       // the total length of this value
    LPVOID value_context;   // provider's context
    LPVOID val_buff_ptr;    // where in the ouput buffer the value is.
};

typedef struct val_context FAR *PVALCONTEXT;

#ifndef UNICODE_ONLY
typedef struct pvalueA {           // Provider supplied value/context.
    LPSTR   pv_valuename;          // The value name pointer
    int pv_valuelen;
    LPVOID pv_value_context;
    DWORD pv_type;
}PVALUEA, FAR *PPVALUEA;
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
typedef struct pvalueW {           // Provider supplied value/context.
    LPWSTR  pv_valuename;          // The value name pointer
    int pv_valuelen;
    LPVOID pv_value_context;
    DWORD pv_type;
}PVALUEW, FAR *PPVALUEW;
#endif //!ANSI_ONLY
#ifdef UNICODE
typedef PVALUEW PVALUE;
typedef PPVALUEW PPVALUE;
#else
typedef PVALUEA PVALUE;
typedef PPVALUEA PPVALUE;
#endif // UNICODE

typedef
DWORD _cdecl
QUERYHANDLER (LPVOID keycontext, PVALCONTEXT val_list, DWORD num_vals,
          LPVOID outputbuffer, DWORD FAR *total_outlen, DWORD input_blen);

typedef QUERYHANDLER FAR *PQUERYHANDLER;

typedef struct provider_info {
    PQUERYHANDLER pi_R0_1val;
    PQUERYHANDLER pi_R0_allvals;
    PQUERYHANDLER pi_R3_1val;
    PQUERYHANDLER pi_R3_allvals;
    DWORD pi_flags;    // capability flags (none defined yet).
    LPVOID pi_key_context;
}REG_PROVIDER;

typedef struct provider_info FAR *PPROVIDER;

#ifndef UNICODE_ONLY
typedef struct value_entA {
    LPSTR   ve_valuename;
    DWORD ve_valuelen;
    DWORD ve_valueptr;
    DWORD ve_type;
}VALENTA, FAR *PVALENTA;
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
typedef struct value_entW {
    LPWSTR  ve_valuename;
    DWORD ve_valuelen;
    DWORD ve_valueptr;
    DWORD ve_type;
}VALENTW, FAR *PVALENTW;
#endif //!ANSI_ONLY
#ifdef UNICODE
typedef VALENTW VALENT;
typedef PVALENTW PVALENT;
#else
typedef VALENTA VALENT;
typedef PVALENTA PVALENT;
#endif // UNICODE

#endif // not(_PROVIDER_STRUCTS_DEFINED)
/*INC*/

#endif /* WINVER >= 0x0400 */

//
// Default values for parameters that do not exist in the Win 3.1
// compatible APIs.
//

#define WIN31_CLASS                 NULL

//
// API Prototypes.
//


WINADVAPI
LONG
APIENTRY
RegCloseKey (
    HKEY hKey
    );

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegConnectRegistryA (
    LPSTR lpMachineName,
    HKEY hKey,
    PHKEY phkResult
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegConnectRegistryW (
    LPWSTR lpMachineName,
    HKEY hKey,
    PHKEY phkResult
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegConnectRegistry  RegConnectRegistryW
#else
#define RegConnectRegistry  RegConnectRegistryA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegCreateKeyA (
    HKEY hKey,
    LPCSTR lpSubKey,
    PHKEY phkResult
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegCreateKeyW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    PHKEY phkResult
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegCreateKey  RegCreateKeyW
#else
#define RegCreateKey  RegCreateKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegCreateKeyExA (
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Reserved,
    LPSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegCreateKeyExW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD Reserved,
    LPWSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegCreateKeyEx  RegCreateKeyExW
#else
#define RegCreateKeyEx  RegCreateKeyExA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegDeleteKeyA (
    HKEY hKey,
    LPCSTR lpSubKey
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegDeleteKeyW (
    HKEY hKey,
    LPCWSTR lpSubKey
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegDeleteKey  RegDeleteKeyW
#else
#define RegDeleteKey  RegDeleteKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegDeleteValueA (
    HKEY hKey,
    LPCSTR lpValueName
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegDeleteValueW (
    HKEY hKey,
    LPCWSTR lpValueName
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegDeleteValue  RegDeleteValueW
#else
#define RegDeleteValue  RegDeleteValueA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumKeyA (
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpName,
    DWORD cbName
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumKeyW (
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpName,
    DWORD cbName
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegEnumKey  RegEnumKeyW
#else
#define RegEnumKey  RegEnumKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumKeyExA (
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpName,
    LPDWORD lpcbName,
    LPDWORD lpReserved,
    LPSTR lpClass,
    LPDWORD lpcbClass,
    PFILETIME lpftLastWriteTime
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumKeyExW (
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpName,
    LPDWORD lpcbName,
    LPDWORD lpReserved,
    LPWSTR lpClass,
    LPDWORD lpcbClass,
    PFILETIME lpftLastWriteTime
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegEnumKeyEx  RegEnumKeyExW
#else
#define RegEnumKeyEx  RegEnumKeyExA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumValueA (
    HKEY hKey,
    DWORD dwIndex,
    LPSTR lpValueName,
    LPDWORD lpcbValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegEnumValueW (
    HKEY hKey,
    DWORD dwIndex,
    LPWSTR lpValueName,
    LPDWORD lpcbValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegEnumValue  RegEnumValueW
#else
#define RegEnumValue  RegEnumValueA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegFlushKey (
    HKEY hKey
    );

WINADVAPI
LONG
APIENTRY
RegGetKeySecurity (
    HKEY hKey,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor,
    LPDWORD lpcbSecurityDescriptor
    );

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegLoadKeyA (
    HKEY    hKey,
    LPCSTR  lpSubKey,
    LPCSTR  lpFile
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegLoadKeyW (
    HKEY    hKey,
    LPCWSTR  lpSubKey,
    LPCWSTR  lpFile
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegLoadKey  RegLoadKeyW
#else
#define RegLoadKey  RegLoadKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegNotifyChangeKeyValue (
    HKEY hKey,
    BOOL bWatchSubtree,
    DWORD dwNotifyFilter,
    HANDLE hEvent,
    BOOL fAsynchronus
    );

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegOpenKeyA (
    HKEY hKey,
    LPCSTR lpSubKey,
    PHKEY phkResult
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegOpenKeyW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    PHKEY phkResult
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegOpenKey  RegOpenKeyW
#else
#define RegOpenKey  RegOpenKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegOpenKeyExA (
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegOpenKeyExW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegOpenKeyEx  RegOpenKeyExW
#else
#define RegOpenKeyEx  RegOpenKeyExA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyA (
    HKEY hKey,
    LPSTR lpClass,
    LPDWORD lpcbClass,
    LPDWORD lpReserved,
    LPDWORD lpcSubKeys,
    LPDWORD lpcbMaxSubKeyLen,
    LPDWORD lpcbMaxClassLen,
    LPDWORD lpcValues,
    LPDWORD lpcbMaxValueNameLen,
    LPDWORD lpcbMaxValueLen,
    LPDWORD lpcbSecurityDescriptor,
    PFILETIME lpftLastWriteTime
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyW (
    HKEY hKey,
    LPWSTR lpClass,
    LPDWORD lpcbClass,
    LPDWORD lpReserved,
    LPDWORD lpcSubKeys,
    LPDWORD lpcbMaxSubKeyLen,
    LPDWORD lpcbMaxClassLen,
    LPDWORD lpcValues,
    LPDWORD lpcbMaxValueNameLen,
    LPDWORD lpcbMaxValueLen,
    LPDWORD lpcbSecurityDescriptor,
    PFILETIME lpftLastWriteTime
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegQueryInfoKey  RegQueryInfoKeyW
#else
#define RegQueryInfoKey  RegQueryInfoKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryValueA (
    HKEY hKey,
    LPCSTR lpSubKey,
    LPSTR lpValue,
    PLONG   lpcbValue
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryValueW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    LPWSTR lpValue,
    PLONG   lpcbValue
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegQueryValue  RegQueryValueW
#else
#define RegQueryValue  RegQueryValueA
#endif // !UNICODE

#if(WINVER >= 0x0400)
#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryMultipleValuesA (
    HKEY hKey,
    PVALENTA val_list,
    DWORD num_vals,
    LPSTR lpValueBuf,
    LPDWORD ldwTotsize
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryMultipleValuesW (
    HKEY hKey,
    PVALENTW val_list,
    DWORD num_vals,
    LPWSTR lpValueBuf,
    LPDWORD ldwTotsize
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegQueryMultipleValues  RegQueryMultipleValuesW
#else
#define RegQueryMultipleValues  RegQueryMultipleValuesA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryValueExA (
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegQueryValueExW (
    HKEY hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegQueryValueEx  RegQueryValueExW
#else
#define RegQueryValueEx  RegQueryValueExA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegReplaceKeyA (
    HKEY     hKey,
    LPCSTR  lpSubKey,
    LPCSTR  lpNewFile,
    LPCSTR  lpOldFile
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegReplaceKeyW (
    HKEY     hKey,
    LPCWSTR  lpSubKey,
    LPCWSTR  lpNewFile,
    LPCWSTR  lpOldFile
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegReplaceKey  RegReplaceKeyW
#else
#define RegReplaceKey  RegReplaceKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegRestoreKeyA (
    HKEY hKey,
    LPCSTR lpFile,
    DWORD   dwFlags
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegRestoreKeyW (
    HKEY hKey,
    LPCWSTR lpFile,
    DWORD   dwFlags
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegRestoreKey  RegRestoreKeyW
#else
#define RegRestoreKey  RegRestoreKeyA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegSaveKeyA (
    HKEY hKey,
    LPCSTR lpFile,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegSaveKeyW (
    HKEY hKey,
    LPCWSTR lpFile,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegSaveKey  RegSaveKeyW
#else
#define RegSaveKey  RegSaveKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegSetKeySecurity (
    HKEY hKey,
    SECURITY_INFORMATION SecurityInformation,
    PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegSetValueA (
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD dwType,
    LPCSTR lpData,
    DWORD cbData
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegSetValueW (
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD dwType,
    LPCWSTR lpData,
    DWORD cbData
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegSetValue  RegSetValueW
#else
#define RegSetValue  RegSetValueA
#endif // !UNICODE


#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegSetValueExA (
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegSetValueExW (
    HKEY hKey,
    LPCWSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegSetValueEx  RegSetValueExW
#else
#define RegSetValueEx  RegSetValueExA
#endif // !UNICODE

#ifndef UNICODE_ONLY
WINADVAPI
LONG
APIENTRY
RegUnLoadKeyA (
    HKEY    hKey,
    LPCSTR lpSubKey
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
LONG
APIENTRY
RegUnLoadKeyW (
    HKEY    hKey,
    LPCWSTR lpSubKey
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define RegUnLoadKey  RegUnLoadKeyW
#else
#define RegUnLoadKey  RegUnLoadKeyA
#endif // !UNICODE

//
// Remoteable System Shutdown APIs
//

#ifndef UNICODE_ONLY
WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownA(
    LPSTR lpMachineName,
    LPSTR lpMessage,
    DWORD dwTimeout,
    BOOL bForceAppsClosed,
    BOOL bRebootAfterShutdown
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownW(
    LPWSTR lpMachineName,
    LPWSTR lpMessage,
    DWORD dwTimeout,
    BOOL bForceAppsClosed,
    BOOL bRebootAfterShutdown
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define InitiateSystemShutdown  InitiateSystemShutdownW
#else
#define InitiateSystemShutdown  InitiateSystemShutdownA
#endif // !UNICODE


#ifndef UNICODE_ONLY
WINADVAPI
BOOL
APIENTRY
AbortSystemShutdownA(
    LPSTR lpMachineName
    );
#endif //!UNICODE_ONLY
#ifndef ANSI_ONLY
WINADVAPI
BOOL
APIENTRY
AbortSystemShutdownW(
    LPWSTR lpMachineName
    );
#endif //!ANSI_ONLY
#ifdef UNICODE
#define AbortSystemShutdown  AbortSystemShutdownW
#else
#define AbortSystemShutdown  AbortSystemShutdownA
#endif // !UNICODE

#ifdef __cplusplus
}
#endif


#endif // _WINREG_
