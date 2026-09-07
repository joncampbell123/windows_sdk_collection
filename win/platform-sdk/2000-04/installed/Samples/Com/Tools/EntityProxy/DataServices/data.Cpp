// Data.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f Dataps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "Data.h"

#include "Data_i.c"
#include "Entity.h"
#include "Dynamic.h"
#include "odbcdata.h"
#include "Proxy.h"
#include "Resultset.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_Proxy, CProxy)
OBJECT_ENTRY(CLSID_Entity, CEntity)
OBJECT_ENTRY(CLSID_Dynamic, CDynamic)
OBJECT_ENTRY(CLSID_Resultset, CResultset)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_DataServices);
        DisableThreadLibraryCalls(hInstance);
		LoadLibraryW(L"dbnmpntw.dll");
		LoadLibraryW(L"odbc32.dll");
		LoadLibraryW(L"sqlsrv32.dll");
		LoadLibraryW(L"nddeapi.dll");
		LoadLibraryW(L"winspool.drv");
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
        _Module.Term();
	}
	_Module.Lock();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
//    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
	return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}


