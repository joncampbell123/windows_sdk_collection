/*++

     Copyright (c) 1996 Intel Corporation
     Copyright (c) 1996 Microsoft Corporation
     All Rights Reserved
     
     Permission is granted to use, copy and distribute this software and 
     its documentation for any purpose and without fee, provided, that 
     the above copyright notice and this statement appear in all copies. 
     Intel makes no representations about the suitability of this 
     software for any purpose.  This software is provided "AS IS."  
     
     Intel specifically disclaims all warranties, express or implied, 
     and all liability, including consequential and other indirect 
     damages, for the use of this software, including liability for 
     infringement of any proprietary rights, and including the 
     warranties of merchantability and fitness for a particular purpose. 
     Intel does not assume any responsibility for any errors which may 
     appear in this software nor any responsibility to update it.

     
Module Name:

    inst_lsp.cpp

Abstract:

    This module contains the installation routines for the winsock2 layered
    service provider sample.  

Author:

    bugs@brandy.jf.intel.com
    
Revision History:

--*/

#include "warnoff.h"
#include <windows.h>
#include "ws2spi.h"
#include <rpc.h>
#include <stdio.h>
#include "install.h"
#define LAYERED_PROVIDER_NAME L"LAYERED_PROVIDER"

VOID
UninstallMyProvider()
{
    INT Errno;
    
    WSCDeinstallProvider(
        &LayeredProviderGuid,
        &Errno);
}
        
INT
InstallMyProvider(
    PDWORD CatalogId
    )
/*++
--*/
{
    WSAPROTOCOL_INFOW  proto_info;
    int               install_result;
    int               install_error;

    // Create a PROTOCOL_INFO to install for our provider DLL.
    proto_info.dwServiceFlags1 = 0;
    proto_info.dwServiceFlags2 = 0;
    proto_info.dwServiceFlags3 = 0;
    proto_info.dwServiceFlags4 = 0;
    proto_info.dwProviderFlags = PFL_HIDDEN;
    proto_info.ProviderId      = LayeredProviderGuid;  
    proto_info.dwCatalogEntryId = 0;   // filled in by system
    proto_info.ProtocolChain.ChainLen = LAYERED_PROTOCOL;
        // Do  not  need  to  fill  in  chain  for LAYERED_PROTOCOL or
        // BASE_PROTOCOL
    proto_info.iVersion = 0;
    proto_info.iAddressFamily = AF_INET;
    proto_info.iMaxSockAddr = 16;
    proto_info.iMinSockAddr = 16;
    proto_info.iSocketType = SOCK_STREAM;
    proto_info.iProtocol = IPPROTO_TCP;   // mimic TCP/IP
    proto_info.iProtocolMaxOffset = 0;
    proto_info.iNetworkByteOrder = BIGENDIAN;
    proto_info.iSecurityScheme = SECURITY_PROTOCOL_NONE;
    proto_info.dwMessageSize = 0;  // stream-oriented
    proto_info.dwProviderReserved = 0xdeadbabe;
    wcscpy(
        proto_info.szProtocol,
        LAYERED_PROVIDER_NAME);

    install_result = WSCInstallProvider(
        &LayeredProviderGuid,
        L"lsp.dll",                   // lpszProviderDllPath
        & proto_info,                 // lpProtocolInfoList
        1,                            // dwNumberOfEntries
        & install_error);             // lpErrno
    *CatalogId = proto_info.dwCatalogEntryId;
    
    return(install_result);
    
} // Install_My_Layer

INT
InstallNewChain(
    LPWSAPROTOCOL_INFOW BaseProtocolInfoBuff,
    DWORD               LayeredProviderCatalogId,
    HKEY                ConfigRegisteryKey
    )
{
    WSAPROTOCOL_INFOW ProtocolChainProtoInfo;
    WCHAR             DebugPrefix[] = L"LAYERED ";
    INT               ReturnCode;
    INT               Errno;
    UUID              NewChainId;
    RPC_STATUS        Status;
    PUCHAR            GuidString;
    HKEY              NewKey;
    DWORD             KeyDisposition;
    BOOL              Continue;
    
    ReturnCode = NO_ERROR;
    
    // We are only layering on top of base providers

    if (BaseProtocolInfoBuff->ProtocolChain.ChainLen == BASE_PROTOCOL){
        Continue = FALSE;

        // Get a new GUID for the protocol chain we are about to install
        Status = UuidCreate(
            &NewChainId);
        if (RPC_S_OK == Status){

            //Get the string representaion of the GUID
            Status = UuidToString(
                &NewChainId,
                &GuidString);
            if (RPC_S_OK == Status){
                // Write down the GUID  in the registry so we know who to
                // uninstall
                RegCreateKeyEx(
                    ConfigRegisteryKey,                 // hkey
                    (LPCSTR)GuidString,                 // lpszSubKey
                    0,                                  // dwReserved
                    NULL,                               // lpszClass
                    REG_OPTION_NON_VOLATILE,            // fdwOptions
                    KEY_ALL_ACCESS,                     // samDesired
                    NULL,                               // lpSecurityAttributes
                    & NewKey,                           // phkResult
                    & KeyDisposition                    // lpdwDisposition
                    );
                RpcStringFree(&GuidString);
                
                Continue =TRUE;
            } //if
            else{
                printf("UuidToString() Failed\n");
            } //else
        } //if
        else{
            printf("UuidCreate Failed\n");
        } //else

        if (Continue){
            
            ProtocolChainProtoInfo = *BaseProtocolInfoBuff;

            ProtocolChainProtoInfo.ProviderId = NewChainId;
            
            wcscpy(
                ProtocolChainProtoInfo.szProtocol,
                DebugPrefix);
            wcscat(
                ProtocolChainProtoInfo.szProtocol,
                BaseProtocolInfoBuff->szProtocol);
        
            ProtocolChainProtoInfo.ProtocolChain.ChainLen = 2;
            ProtocolChainProtoInfo.ProtocolChain.ChainEntries[0] =
                LayeredProviderCatalogId;
            ProtocolChainProtoInfo.ProtocolChain.ChainEntries[1] =
                BaseProtocolInfoBuff->dwCatalogEntryId;
            
            ReturnCode = WSCInstallProvider(
                &NewChainId,
                L"lsp.dll",
                &ProtocolChainProtoInfo,
                1,
                &Errno);
        } //if
    } //if
    return(ReturnCode);
}

int
main( int argc, char** argv)
{
    LPWSAPROTOCOL_INFOW   ProtocolInfoBuff = NULL;
    DWORD                ProtocolInfoBuffSize = 0;
    INT                  ErrorCode;
    INT                  EnumResult;
    LONG                 lresult;
    HKEY                 NewKey;
    DWORD                KeyDisposition;
    GUID                 ProviderID;
    INT                  Index;
    DWORD                CatalogEntryId;
    CHAR                 GuidStringBuffer[40];
    DWORD                GuidStringBufferLen;
    FILETIME             FileTime;
    BOOL                 EntryIdFound;
    
    // See if we are installing or deinstalling
    lresult = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,                     // hkey
        CONFIGURATION_KEY,                      // lpszSubKey
        0,                                      // dwReserved
        KEY_ALL_ACCESS,                        // samDesired
        & NewKey                               // phkResult
        );

    if (ERROR_SUCCESS == lresult){
        // The layered provider is installed so we are going uninstall.

        //
        // Enumerate all the provider IDs we stored on install and deinstall
        // the providers
        //
        printf("Removing Installed Layered Providers\n");
            
        Index = 0;
        GuidStringBufferLen = sizeof(GuidStringBuffer);
        lresult = RegEnumKeyEx(
            NewKey,               //hKey
            Index,                // Index of subkey
            &GuidStringBuffer[0],    // Buffer to hold key name
            &GuidStringBufferLen,  // Length of buffer
            NULL,                 // Reserved
            NULL,                 // Class buffer
            NULL,                 // Class buffer length
            &FileTime              // Last write time
            );

        printf("Removing layered provider protocol chains\n");
        while (lresult != ERROR_NO_MORE_ITEMS){
            UuidFromString(
                (PUCHAR) GuidStringBuffer,
                &ProviderID);
            // Deinstall the provider chain we installed
            WSCDeinstallProvider(
                &ProviderID,
                &ErrorCode);
            // Delete our registry key
            RegDeleteKey(
                NewKey,
                &GuidStringBuffer[0]);
            
            GuidStringBufferLen = sizeof(GuidStringBuffer);
        lresult = RegEnumKeyEx(
            NewKey,               //hKey
            Index,                // Index of subkey
            &GuidStringBuffer[0],    // Buffer to hold key name
            &GuidStringBufferLen,  // Length of buffer
            NULL,                 // Reserved
            NULL,                 // Class buffer
            NULL,                 // Class buffer length
            &FileTime              // Last write time
            );
            
        } //while

        // Clen up the registry
        RegCloseKey(
            NewKey);
        RegDeleteKey(
            HKEY_LOCAL_MACHINE,
            CONFIGURATION_KEY);
        
        // Uninstall the real provider
        UninstallMyProvider();
    } //if
    else{

        RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,                 // hkey
            CONFIGURATION_KEY,                  // lpszSubKey
            0,                                  // dwReserved
            NULL,                               // lpszClass
            REG_OPTION_NON_VOLATILE,            // fdwOptions
            KEY_ALL_ACCESS,                     // samDesired
            NULL,                               // lpSecurityAttributes
            & NewKey,                           // phkResult
            & KeyDisposition                    // lpdwDisposition
            );
        // Install a dummy PROTOCOL_INFO for the layered provider.
        lresult = InstallMyProvider(
            &CatalogEntryId);
        if (NO_ERROR == lresult){
            //
            // Enumerate the installed providers and chains
            //
            printf("Scanning Installed Providers\n");
            // Call WSCEnumProtocols with a zero length buffer so we know what
            // size to  send in to get all the installed PROTOCOL_INFO
            // structs. 
            WSCEnumProtocols(
                NULL,                     // lpiProtocols
                ProtocolInfoBuff,         // lpProtocolBuffer
                & ProtocolInfoBuffSize,   // lpdwBufferLength
                & ErrorCode);             // lpErrno

            ProtocolInfoBuff = (LPWSAPROTOCOL_INFOW)
                malloc(ProtocolInfoBuffSize);
            if (ProtocolInfoBuff){
                printf("Installing Layered Providers\n");
            
                EnumResult = WSCEnumProtocols(
                    NULL,                     // lpiProtocols
                    ProtocolInfoBuff,         // lpProtocolBuffer
                    & ProtocolInfoBuffSize,   // lpdwBufferLength
                    & ErrorCode);
                
                if (SOCKET_ERROR != EnumResult){

                    // Find our provider entry to get our catalog entry ID
                    EntryIdFound = FALSE;
                    for (Index =0; Index < EnumResult; Index++){
                        if (ProtocolInfoBuff[Index].ProviderId ==
                            LayeredProviderGuid){
                            
                            CatalogEntryId =
                                ProtocolInfoBuff[Index].dwCatalogEntryId;
                            EntryIdFound = TRUE;
                        } //if
                    } //for
                    if (EntryIdFound){
                        for (Index =0; Index < EnumResult; Index++){
                            InstallNewChain(
                                &ProtocolInfoBuff[Index],
                                CatalogEntryId,
                                NewKey);                        
                        } //for
                    } //if
                } //if
            } //if
        } //if
    } //else
    return(0);
}
    
