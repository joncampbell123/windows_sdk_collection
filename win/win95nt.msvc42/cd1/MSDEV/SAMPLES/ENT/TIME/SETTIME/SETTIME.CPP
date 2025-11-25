// settime.cpp : Implementation of WinMain
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

// To fully complete this project follow these steps

// You will need the new MIDL compiler to build this project.  Additionally,
// if you are building the proxy stub DLL, you will need new headers and libs.

// 1) Add a custom build step to settime.idl
//                You can select all of the .IDL files by holding Ctrl and clicking on
//                each of them.
//
//                Description
//                        Running MIDL
//                Build Command(s)
//                        midl settime.idl
//                Outputs
//                        settime.tlb
//                        settime.h
//                        settime_i.c
//
// NOTE: You must use the MIDL compiler from NT 4.0, 
// preferably 3.00.15 or greater

// 2) Add a custom build step to the project to register the DLL
//                For this, you can select all projects at once
//                Description
//                        Registering OLE Server...
//                Build Command(s)
//                        $(TargetPath) /RegServer
//                        echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg"
//                Outputs
//                        $(OutDir)\regsvr32.trg

// 3) To add UNICODE support, follow these steps
//                Select Build|Configurations...
//                Press Add...
//                Change the configuration name to Unicode Release
//                Change the "Copy Settings From" combo to settime - Win32 Release
//                Press OK
//                Press Add...
//                Change the configuration name to Unicode Debug
//                Change the "Copy Settings From" combo to settime - Win32 Debug
//                Press OK
//                Press "Close"
//                Select Build|Settings...
//                Select the two UNICODE projects and press the C++ tab.
//                Select the "General" category
//                Add _UNICODE to the Preprocessor definitions
//                Press the Link tab.
//                Select the "Output" category
//                Specify wWinMainCRTStartup as the entry point
//                Select the Unicode Debug project
//                Press the "General" tab
//                Specify DebugU for the intermediate and output directories
//                Select the Unicode Release project
//                Press the "General" tab
//                Specify ReleaseU for the intermediate and output directories

// 4) Proxy stub DLL
//                To build a separate proxy/stub DLL, 
//                run nmake -f ps.mak in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
#include "settime.h"
#include "STimeObj.h"

#define IID_DEFINED
#include "settime_i.c"


CExeModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
        OBJECT_ENTRY(CLSID_MySettime, CMySettimeObject, "SETTIME.MySettimeObject.1", "SETTIME.MySettimeObject.1", IDS_MYSETTIME_DESC, THREADFLAGS_BOTH)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
//
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine, int nShowCmd)
{
        //HRESULT hRes = CoInitialize(NULL);
//  If you are running on NT 4.0 or higher you can use the following call
//        instead to make the EXE free threaded.
//  This means that calls come in on a random RPC thread
        HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        _ASSERTE(SUCCEEDED(hRes));
        _Module.Init(ObjectMap, hInstance);
        _Module.dwThreadID = GetCurrentThreadId();
        TCHAR szTokens[] = _T("-/");

        LPTSTR lpszToken = _tcstok(lpCmdLine, szTokens);
        while (lpszToken != NULL)
        {
                if (_tcsicmp(lpszToken, _T("UnregServer"))==0)
                        return _Module.RemoveRegistry();
                else if (_tcsicmp(lpszToken, _T("RegServer"))==0)
                        return _Module.UpdateRegistry(TRUE);
                lpszToken = _tcstok(NULL, szTokens);
        }

        hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE);
        _ASSERTE(SUCCEEDED(hRes));

        MSG msg;
        while (GetMessage(&msg, 0, 0, 0))
                DispatchMessage(&msg);

        _Module.RevokeClassObjects();

        CoUninitialize();
        return 0;
}
