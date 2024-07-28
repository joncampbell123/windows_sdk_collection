
/*
**	
**	FACTORY.C - All of the class factory stuff for our handler
**
*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <compobj.h>
#include <mmsystem.h>
#define	INITGUID
#include <initguid.h>
#include <avifile.h>
#include "wavefile.h"

//
// Here are the function prototypes for the methods for the class factory
//
STDMETHODIMP WaveFactoryQueryInterface(LPCLASSFACTORY pcf, REFIID iid, void FAR* FAR* ppv);
STDMETHODIMP_(ULONG) WaveFactoryAddRef(LPCLASSFACTORY pcf);
STDMETHODIMP_(ULONG) WaveFactoryRelease(LPCLASSFACTORY pcf);
STDMETHODIMP WaveFactoryCreateInstance(LPCLASSFACTORY pcf, IUnknown FAR* pUnknownOuter, REFIID riid, void FAR* FAR* ppv);
STDMETHODIMP WaveFactoryLockServer(LPCLASSFACTORY pcf, BOOL fLock);

//
// And here's the Vtbl...
//
IClassFactoryVtbl	WaveFactoryVtbl = {
			WaveFactoryQueryInterface,
			WaveFactoryAddRef,
			WaveFactoryRelease,
			WaveFactoryCreateInstance,
			WaveFactoryLockServer};

/*	-	-	-	-	-	-	-	-	*/

BOOL PASCAL FAR LibMain(
	HINSTANCE	hInstance,
	HGLOBAL	segDS,
	UINT	cbHeapSize,
	LPCSTR	pszCmdLine)
{
	return TRUE;
}

/*	-	-	-	-	-	-	-	-	*/

BOOL FAR PASCAL _export WEP(
	BOOL	fSystemExit)
{
	return TRUE;
}

/*	-	-	-	-	-	-	-	-	*/

//
// Can we be unloaded? Only if our reference count AND lock counts are both
// zero.
//
STDAPI DllCanUnloadNow(
	void)
{
	return ResultFromScode((uLockCount || uUseCount) ? S_FALSE : S_OK);
}

/*	-	-	-	-	-	-	-	-	*/

//
// Create a new class factory object.  We must allocate the space for our
// structure ourselves since we're using C.
//
HRESULT WaveFactoryCreate(
	REFCLSID 	rclsid,
	REFIID		riid,
	void FAR* FAR*	ppv)
{
	LPWAVEFACTORY 	pcf;
	IUnknown FAR*	pUnknown;
	HRESULT hresult;

	/*
	** Allocate the new class factory object from the heap
	*/
	pcf = (LPWAVEFACTORY)GlobalAllocPtr(GMEM_MOVEABLE,sizeof(WAVEFACTORY));
	if (pcf == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	/* Initialize our structure */
 	pcf->lpVtbl = &WaveFactoryVtbl;
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

/*	-	-	-	-	-	-	-	-	*/

//
// Retrieve the class object from the DLL... Create an instance.
//
STDAPI DllGetClassObject(
	REFCLSID	rclsid,
	REFIID		riid,
	void FAR* FAR*	ppv)
{
	HRESULT	hresult;

	/*
	** Create a class factory object...
	*/
	hresult = WaveFactoryCreate(rclsid, riid, ppv);
	return hresult;
}

/*	-	-	-	-	-	-	-	-	*/


//
// Query Interface.. We support Unknown and our own interface
//
STDMETHODIMP WaveFactoryQueryInterface(
	LPCLASSFACTORY pcf,
	REFIID		iid,
	void FAR* FAR*	ppv)
{
	if (IsEqualIID(iid, &IID_IUnknown))
		*ppv = pcf;
	else if (IsEqualIID(iid, &IID_IClassFactory))
		*ppv = pcf;
	else
		return ResultFromScode(E_NOINTERFACE);
	// remember to increase our reference count
	WaveFactoryAddRef(pcf);
	return NULL;
}

/*	-	-	-	-	-	-	-	-	*/

//
// Increase our reference count
//
STDMETHODIMP_(ULONG) WaveFactoryAddRef(LPCLASSFACTORY pcf)
{
	LPWAVEFACTORY pwf = (LPWAVEFACTORY) pcf;
	
	return ++pwf->ulRef;
}

/*	-	-	-	-	-	-	-	-	*/

// 
// Decrease our reference count -- free the memory for the structure if we're
// down to zero.
//
STDMETHODIMP_(ULONG) WaveFactoryRelease(LPCLASSFACTORY pcf)
{
	LPWAVEFACTORY pwf = (LPWAVEFACTORY) pcf;
	
	if (!--pwf->ulRef) {
		GlobalFreePtr(pwf);
		return 0;
	}
	return pwf->ulRef;
}

/*	-	-	-	-	-	-	-	-	*/

//
// Create an object of our class
//
STDMETHODIMP WaveFactoryCreateInstance(
	LPCLASSFACTORY	pcf,
	IUnknown FAR*	pUnknownOuter,
	REFIID		riid,
	void FAR* FAR*	ppv)
{
	// Actually create a real object using the CAVIFile class....
	return WaveFileCreate(pUnknownOuter, riid, ppv);
}

/*	-	-	-	-	-	-	-	-	*/

//
// The standard LockServer function.  
//
STDMETHODIMP WaveFactoryLockServer(
	LPCLASSFACTORY	pcf,
	BOOL	fLock)
{
	if (fLock)
	    uLockCount++;
	else
	    uLockCount--;
	return NULL;
}

/*	-	-	-	-	-	-	-	-	*/
