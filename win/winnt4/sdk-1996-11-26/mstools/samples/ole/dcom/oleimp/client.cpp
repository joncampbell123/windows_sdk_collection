// ===========================================================================
// File: C L I E N T . C P P
// 
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
//  This is the client-portion of the OLE impersonation sample. This
// application uses the CLSID_CObject class implemented by the OLEIMP.CPP
// module. Pass the machine-name to instantiate the object on, or pass no
// arguments to instantiate the object on the same machine. See the README.TXT
// file for further details.
// 
// This sample may be compiled as either UNICODE or ANSI.
// 
// Copyright 1996 Microsoft Corporation.  All Rights Reserved.
// ===========================================================================

// %%Includes: ---------------------------------------------------------------
#define INC_OLE2
#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include <tchar.h>
#include <conio.h>

// %%GUIDs: ------------------------------------------------------------------
DEFINE_GUID(CLSID_CObject, 0x35b79d1, 0xd6d3, 0x11cf, 0xb9, 0xd4, 0x0, 0xaa, 0x0, 0xa2, 0x16, 0xe0);

// ---------------------------------------------------------------------------
// %%Function: Message
// 
//  Formats and displays a message to the console.
// ---------------------------------------------------------------------------
 void
Message(LPTSTR szPrefix, HRESULT hr)
{
    LPTSTR   szMessage;

    if (hr == S_OK)
        {
        _tprintf(szPrefix);
        _tprintf(TEXT("\n"));
        return;
        }
 
    if (HRESULT_FACILITY(hr) == FACILITY_WINDOWS)
        hr = HRESULT_CODE(hr);
 
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
        (LPTSTR)&szMessage,
        0,
        NULL );

    _tprintf (TEXT("%s: %s(%lx)\n"), szPrefix, szMessage, hr);
    
    LocalFree(szMessage);
}  // Message

// ---------------------------------------------------------------------------
// %%Function: main
// ---------------------------------------------------------------------------
 void __cdecl
main(int argc, CHAR **argv)
{
    HRESULT hr;
    COSERVERINFO csi, *pcsi=NULL;
    WCHAR wsz [MAX_PATH];
	LPCLASSFACTORY pcf = NULL;

    // initialize COM for free-threaded use
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
        {
        Message(TEXT("Client: CoInitializeEx"), hr);
        exit(hr);
        }

	// initialize the security layer
	hr = CoInitializeSecurity (
					NULL,
					-1,
					NULL,
					NULL,
					RPC_C_AUTHN_LEVEL_CONNECT,
					RPC_C_IMP_LEVEL_IMPERSONATE,
					NULL,
					0,
					NULL);
	if (FAILED(hr))
		{
		Message(TEXT("Client: CoInitializeSecurity"), hr);
		exit(hr);
		}

    // allow a machine-name as the command-line argument
    if (argc > 1)
        {
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[1], -1,
            wsz, MAX_PATH);
		memset (&csi, 0, sizeof (csi));
        csi.pwszName = wsz;
        pcsi = &csi;
        } 

	// get the class factory for CObject on the argv[1] machine
	hr = CoGetClassObject (CLSID_CObject, CLSCTX_SERVER, pcsi,
							IID_IClassFactory, (LPVOID*)&pcf);
	if (FAILED(hr))
		Message(TEXT("Client: CoGetClassObject"), hr);
	else
		{
        LPUNKNOWN   punk = NULL;

	    // create a remote instance of the object
	    Message(TEXT("Client: Creating Instance..."), S_OK);
	    hr = pcf->CreateInstance(NULL, IID_IUnknown, (LPVOID*)&punk);
	    if (FAILED(hr))
	        Message(TEXT("Client: CoCreateInstanceEx"), hr);
	    else
	        {
			// wait until the user wants us to release the object
			Message(TEXT("Client: Press any key to release object..."), S_OK);
			getch();

	        // let go of the object
	        punk->Release();
	        }
		pcf->Release();
		}
    CoUninitialize();
    Message(TEXT("Client: Done"), S_OK);
}  // main

// EOF =======================================================================

