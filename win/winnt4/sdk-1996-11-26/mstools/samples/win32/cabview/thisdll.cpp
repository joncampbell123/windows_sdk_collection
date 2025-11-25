//*******************************************************************************************
//
// Filename : ThisDll.cpp
//	
//				Generic OLE DLL routines
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************


#include "pch.h"

// ********************************************************
// Initialize GUIDs
//
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <cguid.h>
#include "thisguid.h"
//#undef INITGUID
#pragma data_seg()

#include "thisdll.h"

#include "resource.h"

CThisDll g_ThisDll;

// ******************************************************************
// ******************************************************************
// DllMain
STDAPI_(BOOL) APIENTRY DllMain(
   HINSTANCE hDll, 
   DWORD dwReason, 
   LPVOID lpReserved)
{
   switch(dwReason)
   {
      case DLL_PROCESS_ATTACH:
         g_ThisDll.SetInstance(hDll);

         // Initialize the various modules.
         //

         //
         // Tell system to fix up alignment faults at runtime
         //

         SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT);

         break;
   
      case DLL_PROCESS_DETACH:
         break;
     
      case DLL_THREAD_ATTACH:
         break;

      case DLL_THREAD_DETACH:
         break;

      default:
         break;
     
   } // switch

   return(TRUE);
}

 
// ******************************************************************
// DllCanUnloadNow
STDAPI DllCanUnloadNow()
{
	HRESULT retval = (HRESULT)((g_ThisDll.m_cRef.GetRef() == 0)
		&& (g_ThisDll.m_cLock.GetRef() == 0) ? S_OK : S_FALSE);
	return(retval);
}


// Procedure for uninstalling this DLL (given an INF file)
void CALLBACK Uninstall(HWND hwndStub, HINSTANCE hInstance, LPSTR lpszCmdLine,
	int nCmdShow)
{
	RUNDLLPROC pfnCheckAPI = Uninstall;

	static char szFmt[] = "rundll.exe setupx.dll,InstallHinfSection DefaultUninstall 132 %s";

	if (!lpszCmdLine || lstrlen(lpszCmdLine)>=MAX_PATH)
	{
		return;
	}
	if (MessageBox(hwndStub, MAKEINTRESOURCE(IDS_SUREUNINST),
			MAKEINTRESOURCE(IDS_THISDLL), MB_YESNO|MB_ICONSTOP) != IDYES)
	{
		return;
	}
   	

	char szCmd[sizeof(szFmt)+MAX_PATH];
	wsprintf(szCmd, szFmt, lpszCmdLine);

	// Note that I use START.EXE, to minimize the chance that this DLL will still be loaded
	// when SETUPX tries to delete it
	ShellExecute(hwndStub, NULL, "start.exe", szCmd, NULL, SW_SHOWMINIMIZED);
}


// CThisDll::CThisDll() 

// ********************************************************************
class CThisDllClassFactory : public IClassFactory
{
public:
	// *** IUnknown methods ***
	STDMETHODIMP QueryInterface(REFIID riid,
		LPVOID FAR* ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);
 
	// *** IClassFactory methods ***
	STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter,
		REFIID riid,
		LPVOID FAR* ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);

private:
	CRefDll m_cRefDll;

	CRefCount m_cRef;
};

// ******************************************************************
// ******************************************************************
// DllGetClassObject
STDAPI DllGetClassObject(
   REFCLSID rclsid, 
   REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;

	if(! (rclsid == CLSID_ThisDll))
	{
		return(E_FAIL);
	}

	CThisDllClassFactory *pcf = new CThisDllClassFactory;
	if (!pcf)
	{
		return(E_OUTOFMEMORY);
	}

	// Note if the QueryInterface fails, the Release will delete the object
	pcf->AddRef();
	HRESULT hRes = pcf->QueryInterface(riid, ppvObj);
	pcf->Release(); 
 
	return(hRes);
}


// ***********************************************************************
// ***********************************************************************
// CImpIClassFactory member functions

// *** IUnknown methods ***
STDMETHODIMP CThisDllClassFactory::QueryInterface(REFIID riid, 
   LPVOID FAR* ppvObj)
{
	*ppvObj = NULL;
 
	// Any interface on this object is the object pointer
	if((riid == IID_IUnknown) || (riid == IID_IClassFactory))
	{
		*ppvObj = (LPVOID)(IClassFactory *)this;
	}

	if(*ppvObj)
	{
		((LPUNKNOWN)*ppvObj)->AddRef();
		return NOERROR;
	}

	return(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CThisDllClassFactory::AddRef(void)
{
	return(m_cRef.AddRef());
}


STDMETHODIMP_(ULONG) CThisDllClassFactory::Release(void)
{
	if (!m_cRef.Release())
	{
   		delete this;
		return(0);
	}

	return(m_cRef.GetRef());
}
 

// *** IClassFactory methods ***
STDMETHODIMP CThisDllClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
        REFIID riid,
        LPVOID FAR* ppvObj)
{
	// we do not support aggregation
	if(pUnkOuter)
	{
		return(CLASS_E_NOAGGREGATION);
	}

	return(::CreateInstance(riid, ppvObj));
}


STDMETHODIMP CThisDllClassFactory::LockServer(BOOL fLock)
{
	if(fLock)
	{
		g_ThisDll.m_cLock.AddRef();
	}
	else
	{
		g_ThisDll.m_cLock.Release();
	}

	return(NOERROR);
}


void *  __cdecl operator new(unsigned int nSize)
{
	return((LPVOID)LocalAlloc(LMEM_FIXED, nSize));
}


void  __cdecl operator delete(void *pv)
{
	LocalFree((HLOCAL)pv);
}


extern "C" int __cdecl _purecall()
{
	return(0);
}
