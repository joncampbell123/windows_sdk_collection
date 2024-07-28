
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
// Bring in the external GUIDs we need....
DEFINE_OLEGUID(IID_IUnknown,            0x00000000L, 0, 0);
DEFINE_OLEGUID(IID_IClassFactory,       0x00000001L, 0, 0);
DEFINE_OLEGUID(IID_IMarshal,            0x00000003L, 0, 0);

#include <avifile.h>
#include "handler.h"

/*	-	-	-	-	-	-	-	-	*/

EXTERN_C BOOL PASCAL FAR LibMain(
	HINSTANCE	hInstance,
	HGLOBAL	segDS,
	UINT	cbHeapSize,
	LPCSTR	pszCmdLine)
{
	return TRUE;
}

/*	-	-	-	-	-	-	-	-	*/

EXTERN_C BOOL FAR PASCAL _export WEP(
	BOOL	fSystemExit)
{
	return TRUE;
}

/*	-	-	-	-	-	-	-	-	*/

STDAPI DllCanUnloadNow(
	void)
{
	return ResultFromScode((fLocked || uUseCount) ? S_FALSE : S_OK);
}

/*	-	-	-	-	-	-	-	-	*/

STDAPI DllGetClassObject(
	const CLSID FAR&	rclsid,
	const IID FAR&	riid,
	void FAR* FAR*	ppv)
{
	HRESULT	hresult;

	hresult = CAVIFileCF::Create(rclsid, riid, ppv);
	return hresult;
}

/*	-	-	-	-	-	-	-	-	*/

HRESULT CAVIFileCF::Create(
	const CLSID FAR&	rclsid,
	const IID FAR&	riid,
	void FAR* FAR*	ppv)
{
	CAVIFileCF FAR*	pAVIFileCF;
	IUnknown FAR*	pUnknown;
	HRESULT hresult;

	pAVIFileCF = new FAR CAVIFileCF(rclsid, &pUnknown);
	if (pAVIFileCF == NULL)
		return ResultFromScode(E_OUTOFMEMORY);
	hresult = pUnknown->QueryInterface(riid, ppv);
	if (FAILED(GetScode(hresult)))
		delete pAVIFileCF;
	return hresult;
}

/*	-	-	-	-	-	-	-	-	*/

CAVIFileCF::CAVIFileCF(
	const CLSID FAR&	rclsid,
	IUnknown FAR* FAR*	ppUnknown)
{
	m_clsid = rclsid;
	m_refs = 0;
	*ppUnknown = this;
}

/*	-	-	-	-	-	-	-	-	*/

STDMETHODIMP CAVIFileCF::QueryInterface(
	const IID FAR&	iid,
	void FAR* FAR*	ppv)
{
	if (iid == IID_IUnknown)
		*ppv = this;
	else if (iid == IID_IClassFactory)
		*ppv = this;
	else
		return ResultFromScode(E_NOINTERFACE);
	AddRef();
	return NULL;
}

/*	-	-	-	-	-	-	-	-	*/

STDMETHODIMP_(ULONG) CAVIFileCF::AddRef()
{
	return ++m_refs;
}

/*	-	-	-	-	-	-	-	-	*/

STDMETHODIMP_(ULONG) CAVIFileCF::Release()
{
	if (!--m_refs) {
		delete this;
		return 0;
	}
	return m_refs;
}

/*	-	-	-	-	-	-	-	-	*/

STDMETHODIMP CAVIFileCF::CreateInstance(
	IUnknown FAR*	pUnknownOuter,
	const IID FAR&	riid,
	void FAR* FAR*	ppv)
{
	HRESULT	    hresult;
	// Actually create a real object using the CAVIFile class....

	// !!! We should really make sure they're using IAVIFile or IMarshal!
	if (m_clsid == CLSID_DIBSEQFileReader) {
	    hresult = CAVIFile::Create(pUnknownOuter, riid, ppv);
	    return hresult;
	} else {
	    return ResultFromScode(E_UNEXPECTED);
	}
}

/*	-	-	-	-	-	-	-	-	*/

STDMETHODIMP CAVIFileCF::LockServer(
	BOOL	fLock)
{
	fLocked = fLock;
	return NULL;
}

/*	-	-	-	-	-	-	-	-	*/
