/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
//--------------------------------------------------------------------------;
//
//  File: Factory1.c
//
//  Abstract:  All of the class factory stuff for our handler
//
//
//  Contents:
//      WEP()
//      DllCanUnloadNow()
//      SVXFactoryCreate()
//      DllGetClassObject()
//      SVXFactoryQueryInterface()
//      SVXFactoryAddRef()
//      SVXFactoryRelease()
//      SVXFactoryCreateInstance()
//      SVXFactoryLockServer()
//
//--------------------------------------------------------------------------;
#define INITGUID
#define INC_OLE2

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <mmsystem.h>
#include <vfw.h>

#include "svxfile.h"


HMODULE ghModule = NULL;    // global HMODULE/HINSTANCE for resource access

//
// Here are the function prototypes for the methods for the class factory
//
STDMETHODIMP SVXFactoryQueryInterface(
    LPCLASSFACTORY pcf, 
    REFIID iid, 
    void FAR* FAR* ppv);
STDMETHODIMP_(ULONG) SVXFactoryAddRef(
    LPCLASSFACTORY pcf);
STDMETHODIMP_(ULONG) SVXFactoryRelease(
    LPCLASSFACTORY pcf);
STDMETHODIMP SVXFactoryCreateInstance(
    LPCLASSFACTORY pcf, 
    IUnknown FAR* pUnknownOuter, 
    REFIID riid, 
    void FAR* FAR* ppv);
STDMETHODIMP SVXFactoryLockServer(
    LPCLASSFACTORY pcf, 
    BOOL fLock);
HRESULT SVXFactoryCreate(
    REFCLSID    rclsid,
    REFIID      riid,
    void FAR* FAR* ppv);

//
// And here's the Vtbl...
//
IClassFactoryVtbl SVXFactoryVtbl = 
{
    SVXFactoryQueryInterface,
    SVXFactoryAddRef,
    SVXFactoryRelease,
    SVXFactoryCreateInstance,
    SVXFactoryLockServer
};

//--------------------------------------------------------------------------;
//
//  EXTERN_C DllMain
//
//  Description:
//
//
//  Arguments:
//      HANDLE:
//
//      DWORD:
//
//      LPVOID:
//
//  Return (EXTERN_C):
//
//--------------------------------------------------------------------------;
EXTERN_C BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID);
EXTERN_C BOOL APIENTRY DllMain(
HANDLE hModule, 
DWORD dwReason, 
LPVOID lpReserved )
{
	switch( dwReason) 
	{
		case DLL_PROCESS_ATTACH:
			if(ghModule == NULL)
				ghModule = (HMODULE)hModule;
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}


//--------------------------------------------------------------------------;
//
//  BOOL WEP
//
//  Description:
//
//
//  Arguments:
//      BOOL fSystemExit:
//
//  Return (BOOL):
//
//--------------------------------------------------------------------------;

BOOL FAR PASCAL WEP(
	BOOL	fSystemExit)
{
	return TRUE;
}

//--------------------------------------------------------------------------;
//
//  STDAPI DllCanUnloadNow
//
//  Description:
//      Can we be unloaded? Only if our reference count AND lock counts are both
//      zero.
//
//  Arguments:
//      None.
//
//  Return (STDAPI):
//
//--------------------------------------------------------------------------;

STDAPI DllCanUnloadNow(void)
{
	return ResultFromScode((uLockCount || uUseCount) ? S_FALSE : S_OK);
}


//--------------------------------------------------------------------------;
//
//  HRESULT SVXFactoryCreate
//
//  Description:
//      Create a new class factory object.  We must allocate the space for our
//      structure ourselves since we're using C.
//
//  Arguments:
//      REFCLSID rclsid:
//
//      REFIID riid:
//
//  Return (HRESULT):
//
//--------------------------------------------------------------------------;

HRESULT SVXFactoryCreate
(
	REFCLSID 	    rclsid,
	REFIID		    riid,
	void FAR* FAR*	ppv
)
{

LPSVXFACTORY 	pcf;
IUnknown FAR*	pUnknown;
HRESULT         hresult;

	/*
	** Allocate the new class factory object from the heap
	*/
	pcf = (LPSVXFACTORY)GlobalAllocPtr(GMEM_MOVEABLE,sizeof(SVXFACTORY));
	if (pcf == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	/* Initialize our structure */
 	pcf->lpVtbl = &SVXFactoryVtbl;
	pcf->clsid = (CLSID FAR *)rclsid;
	pcf->ulRef = 0;
	pUnknown = (IUnknown FAR *)pcf;

	/*
	** Get the specified interface from the class factory
	** (If it's not IClassFactory, it will fail....)
	*/
	hresult = pUnknown->lpVtbl->QueryInterface(pUnknown, riid, ppv);
	
	if (FAILED(GetScode(hresult)))
		GlobalFreePtr(pcf);
	return hresult;
}

//--------------------------------------------------------------------------;
//
//  STDAPI DllGetClassObject
//
//  Description:
//      Retrieve the class object from the DLL... Create an instance.
//
//  Arguments:
//      REFCLSID rclsid:
//
//      REFIID riid:
//
//  Return (STDAPI):
//
//--------------------------------------------------------------------------;

STDAPI DllGetClassObject
(
	REFCLSID	    rclsid,
	REFIID		    riid,
	void FAR* FAR*	ppv
)
{
	HRESULT	hresult;

	/*
	** Create a class factory object...
	*/
	hresult = SVXFactoryCreate(rclsid, riid, ppv);
	return hresult;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFactoryQueryInterface
//
//  Description:
//      Query Interface.. We support Unknown and our own interface
//
//  Arguments:
//      LPCLASSFACTORY pcf:
//
//      REFIID iid:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFactoryQueryInterface
(
	LPCLASSFACTORY  pcf,
	REFIID		    iid,
	void FAR* FAR*	ppv
)
{
	if (IsEqualIID(iid, &IID_IUnknown))
		*ppv = pcf;
	else if (IsEqualIID(iid, &IID_IClassFactory))
		*ppv = pcf;
	else
		return ResultFromScode(E_NOINTERFACE);
	// remember to increase our reference count
	SVXFactoryAddRef(pcf);
	return NOERROR;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXFactoryAddRef
//
//  Description:
//      Increase our reference count
//
//  Arguments:
//      LPCLASSFACTORY pcf:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXFactoryAddRef(LPCLASSFACTORY pcf)
{
	LPSVXFACTORY pwf = (LPSVXFACTORY) pcf;
	
	return ++pwf->ulRef;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP_(ULONG) SVXFactoryRelease
//
//  Description:
//      Decrease our reference count -- free the memory for the structure if we're
//      down to zero.
//
//  Arguments:
//      LPCLASSFACTORY pcf:
//
//  Return (STDMETHODIMP_(ULONG)):
//
//--------------------------------------------------------------------------;

STDMETHODIMP_(ULONG) SVXFactoryRelease(LPCLASSFACTORY pcf)
{
	LPSVXFACTORY pwf = (LPSVXFACTORY) pcf;
	
	if (!--pwf->ulRef) {
		GlobalFreePtr(pwf);
		return 0;
	}
	return pwf->ulRef;
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFactoryCreateInstance
//
//  Description:
//      Create an object of our class
//
//  Arguments:
//      LPCLASSFACTORY pcf:
//
//      IUnknown FAR* pUnknownOuter:
//
//      REFIID riid:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFactoryCreateInstance
(
	LPCLASSFACTORY	pcf,
	IUnknown FAR*	pUnknownOuter,
	REFIID		    riid,
	void FAR* FAR*	ppv
)
{
	// Actually create a real object using the CAVIFile class....
	return SVXFileCreate(pUnknownOuter, riid, ppv);
}

//--------------------------------------------------------------------------;
//
//  STDMETHODIMP SVXFactoryLockServer
//
//  Description:
//      The standard LockServer function.  
//
//  Arguments:
//      LPCLASSFACTORY pcf:
//
//      BOOL fLock:
//
//  Return (STDMETHODIMP):
//
//--------------------------------------------------------------------------;

STDMETHODIMP SVXFactoryLockServer
(
	LPCLASSFACTORY	pcf,
	BOOL	        fLock
)
{
	if (fLock)
	    uLockCount++;
	else
	    uLockCount--;
	return NOERROR;
}
