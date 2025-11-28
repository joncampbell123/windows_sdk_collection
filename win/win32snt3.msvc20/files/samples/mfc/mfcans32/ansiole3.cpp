//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansiole3.cpp
//
//  Contents:   ANSI Wrappers for Unicode Ole2 Interfaces and APIs.
//
//  Functions:  ReadClassStgA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//+--------------------------------------------------------------------------
//
//  Routine:    CreateDataCacheA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateDataCacheA(LPUNKNOWN pUnkOuterA, REFCLSID rclsid,
					REFIID iid, LPVOID FAR* ppv)
{
	TraceSTDAPIEnter("CreateDataCacheA");
	LPUNKNOWN pUnkOuter;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnkOuterA, &pUnkOuter);
	if (FAILED(hResult))
		return hResult;

	hResult = CreateDataCache(pUnkOuter, rclsid, iid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(iid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppv);

	if (pUnk)
		pUnk->Release();

Error:
	if (pUnkOuter)
		pUnkOuter->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateDefaultHandlerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateDefaultHandlerA(REFCLSID clsid, LPUNKNOWN pUnkOuter,
					REFIID riid, LPVOID FAR* lplpObj)
{
	TraceSTDAPIEnter("OleCreateDefaultHandlerA");
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;

	// Note: Since OleCreateDefaultHandler doesn't increase pUnkOuter's
	// reference count, we can't create a wrapper for it here and then
	// release it, because we'd leave the default handler with a pointer
	// to garbage.  Instead, we insist on having pUnkOuter "pre-wrapped"
	// by the caller.

	hResult = OleCreateDefaultHandler(clsid, pUnkOuter, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)lplpObj);

	if (pUnk)
		 pUnk->Release();

Error:
	return hResult;
}



//+--------------------------------------------------------------------------
//
//  Routine:    OleDrawA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleDrawA(LPUNKNOWN pUnknownA, DWORD dwAspect, HDC hdcDraw,
					LPCRECT lprcBounds)
{
	TraceSTDAPIEnter("OleDrawA");
	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnknownA, &pUnk);
	if (FAILED(hResult))
	return hResult;

	hResult = OleDraw(pUnk, dwAspect, hdcDraw, lprcBounds);

	if (pUnk)
		pUnk->Release();

	return hResult;
}



//+--------------------------------------------------------------------------
//
//  Routine:    OleNoteObjectVisibleA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleNoteObjectVisibleA(LPUNKNOWN pUnknown, BOOL fVisible)
{
	TraceSTDAPIEnter("OleNoteObjectVisibleA");
	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnknown, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = OleNoteObjectVisible(pUnk, fVisible);

	if (pUnk)
		pUnk->Release();

	return hResult;
}



//+--------------------------------------------------------------------------
//
//  Routine:    OleRunA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleRunA(LPUNKNOWN pUnknown)
{
	TraceSTDAPIEnter("OleRunA");
	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnknown, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = OleRun(pUnk);

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleSetContainedObjectA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleSetContainedObjectA(LPUNKNOWN pUnknown, BOOL fContained)
{
	TraceSTDAPIEnter("OleSetContainedObjectA");
	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnknown, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = OleSetContainedObject(pUnk, fContained);

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ReadClassStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI ReadClassStgA(LPSTORAGEA pStgA, CLSID * pclsid)
{
	TraceSTDAPIEnter("ReadClassStgA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = ReadClassStg(pStg, pclsid);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    WriteClassStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI WriteClassStgA(LPSTORAGEA pStgA, REFCLSID rclsid)
{
	TraceSTDAPIEnter("WriteClassStgA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = WriteClassStg(pStg, rclsid);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ReadClassStmA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI ReadClassStmA(LPSTREAMA pStmA, CLSID * pclsid)
{
	TraceSTDAPIEnter("ReadClassStmA");
	LPSTREAM pStm;
	HRESULT hResult;


	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = ReadClassStm(pStm, pclsid);

	if (pStm)
		pStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    WriteClassStmA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI WriteClassStmA(LPSTREAMA pStmA, REFCLSID rclsid)
{
	TraceSTDAPIEnter("WriteClassStmA");
	LPSTREAM pStm;
	HRESULT hResult;


	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = WriteClassStm(pStm, rclsid);

	if (pStm)
		pStm->Release();

	return hResult;
}



//+--------------------------------------------------------------------------
//
//  Routine:    WriteFmtUserTypeStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI WriteFmtUserTypeStgA(LPSTORAGEA pStgA, CLIPFORMAT cf,
		LPSTR lpszUserTypeA)
{
	TraceSTDAPIEnter("WriteFmtUserTypeStgA");

	LPSTORAGE pStg;
	OLECHAR szUserType[MAX_STRING], * pszUserType;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	if (lpszUserTypeA)
	{
		ConvertStringToW(lpszUserTypeA, szUserType);
		pszUserType = szUserType;
	}
	else
		pszUserType = NULL;

	hResult = WriteFmtUserTypeStg(pStg, cf, pszUserType);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ReadFmtUserTypeStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI ReadFmtUserTypeStgA(LPSTORAGEA pStgA, CLIPFORMAT * pcf,
		LPSTR * lplpszUserTypeA)
{
	TraceSTDAPIEnter("ReadFmtUserTypeStgA");
	LPSTORAGE pStg;
	LPOLESTR *lplpszUserType;
	LPOLESTR lpszUserType;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	if (lplpszUserTypeA)
		lplpszUserType = &lpszUserType;
	else
		lplpszUserType = NULL;

	hResult = ReadFmtUserTypeStg(pStg, pcf, lplpszUserType);
	if (FAILED(hResult))
		goto Error;

	if (lplpszUserType)
	{
		hResult = ConvertStringToA(lpszUserType, lplpszUserTypeA);
		ConvertStringFree(lpszUserType);
	}

Error:
	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleQueryLinkFromDataA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleQueryLinkFromDataA(LPDATAOBJECTA pSrcDataObjectA)
{
	TraceSTDAPIEnter("OleQueryLinkFromDataA");
	LPDATAOBJECT pSrcDataObject;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pSrcDataObjectA, &pSrcDataObject);
	if (FAILED(hResult))
		return (hResult);

	hResult = OleQueryLinkFromData(pSrcDataObject);

	if (pSrcDataObject)
		pSrcDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleQueryCreateFromDataA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleQueryCreateFromDataA(LPDATAOBJECTA pSrcDataObjectA)
{
	TraceSTDAPIEnter("OleQueryCreateFromDataA");
	LPDATAOBJECT pSrcDataObject;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pSrcDataObjectA, &pSrcDataObject);
	if (FAILED(hResult))
		return (hResult);

	hResult = OleQueryCreateFromData(pSrcDataObject);

	if (pSrcDataObject)
		pSrcDataObject->Release();

	return hResult;
}



//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateA(REFCLSID rclsid, REFIID riid, DWORD renderopt,
		LPFORMATETC pFormatEtc, LPOLECLIENTSITEA pClientSiteA,
		LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateA");
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error;

	hResult = OleCreate(rclsid, riid, renderopt, pFormatEtc, pClientSite,
			pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error:
	if (pClientSite)
		pClientSite->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateFromDataA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateFromDataA(LPDATAOBJECTA pSrcDataObjA, REFIID riid,
		DWORD renderopt, LPFORMATETC pFormatEtc,
		LPOLECLIENTSITEA pClientSiteA, LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateFromDataA");
	LPDATAOBJECT pSrcDataObj;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pSrcDataObjA, &pSrcDataObj);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error1;

	hResult = OleCreateFromData(pSrcDataObj, riid, renderopt, pFormatEtc,
			pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error1;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error1:
	if (pClientSite)
		pClientSite->Release();

Error:
	if (pSrcDataObj)
		pSrcDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateLinkFromDataA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateLinkFromDataA(LPDATAOBJECTA pSrcDataObjA, REFIID riid,
		DWORD renderopt, LPFORMATETC pFormatEtc,
		LPOLECLIENTSITEA pClientSiteA, LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateLinkFromDataA");
	LPDATAOBJECT pSrcDataObj;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pSrcDataObjA, &pSrcDataObj);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error1;

	hResult = OleCreateLinkFromData(pSrcDataObj, riid, renderopt, pFormatEtc,
			pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error1;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error1:
	if (pClientSite)
		pClientSite->Release();

Error:
	if (pSrcDataObj)
		pSrcDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateStaticFromDataA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateStaticFromDataA(LPDATAOBJECTA pSrcDataObjA, REFIID iid,
		DWORD renderopt, LPFORMATETC pFormatEtc,
		LPOLECLIENTSITEA pClientSiteA, LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateStaticFromDataA");
	LPDATAOBJECT pSrcDataObj;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pSrcDataObjA, &pSrcDataObj);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error1;

	hResult = OleCreateStaticFromData(pSrcDataObj, iid, renderopt, pFormatEtc,
			pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error1;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(iid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error1:
	if (pClientSite)
		pClientSite->Release();

Error:
	if (pSrcDataObj)
		pSrcDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateLinkA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateLinkA(LPMONIKERA pmkLinkSrcA, REFIID riid, DWORD renderopt,
		LPFORMATETC lpFormatEtc, LPOLECLIENTSITEA pClientSiteA,
		LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateLinkA");
	LPMONIKER pmkLinkSrc;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIMonikerWFromA(pmkLinkSrcA, &pmkLinkSrc);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error1;

	hResult = OleCreateLink(pmkLinkSrc, riid, renderopt, lpFormatEtc,
			pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error1;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error1:
	if (pClientSite)
		pClientSite->Release();

Error:
	if (pmkLinkSrc)
		pmkLinkSrc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateLinkToFileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateLinkToFileA(LPCSTR lpszFileNameA, REFIID riid,
		DWORD renderopt, LPFORMATETC lpFormatEtc,
		LPOLECLIENTSITEA pClientSiteA, LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateLinkToFileA");
	OLECHAR szFileName[MAX_STRING], * pszFileName;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	if (lpszFileNameA)
	{
		ConvertStringToW(lpszFileNameA, szFileName);
		pszFileName = szFileName;
	}
	else
		pszFileName = NULL;

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error;

	hResult = OleCreateLinkToFile(pszFileName, riid, renderopt, lpFormatEtc,
			pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error:
	if (pClientSite)
		pClientSite->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleCreateFromFileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleCreateFromFileA(REFCLSID rclsid, LPCSTR lpszFileNameA, REFIID riid,
		DWORD renderopt, LPFORMATETC lpFormatEtc,
		LPOLECLIENTSITEA pClientSiteA, LPSTORAGEA pStgA, LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleCreateFromFileA");
	OLECHAR szFileName[MAX_PATH], *pszFileName;
	LPOLECLIENTSITE pClientSite;
	LPSTORAGE pStg;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	if (lpszFileNameA != NULL)
	{
		pszFileName = szFileName;
		ConvertStringToW(lpszFileNameA, pszFileName);
	}
	else
		pszFileName = NULL;

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error;

	hResult = OleCreateFromFile(rclsid, pszFileName, riid, renderopt,
			lpFormatEtc, pClientSite, pStg, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStg)
		pStg->Release();

	if (pUnk)
		pUnk->Release();

Error:
	if (pClientSite)
		pClientSite->Release();

	TraceSTDAPIExit("OleCreateFromFileA", hResult);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleLoadA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleLoadA(LPSTORAGEA pStgA, REFIID riid, LPOLECLIENTSITEA pClientSiteA,
		LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleLoadA");
	LPSTORAGE pStg;
	LPOLECLIENTSITE pClientSite;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		goto Error;

	hResult = OleLoad(pStg, riid, pClientSite, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pUnk)
		pUnk->Release();

	if (pClientSite)
		pClientSite->Release();

Error:
	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleSaveA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleSaveA(LPPERSISTSTORAGEA pPSA, LPSTORAGEA pStgA, BOOL fSameAsLoad)
{
	TraceSTDAPIEnter("OleSaveA");
	LPPERSISTSTORAGE pPS;
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIPersistStorageWFromA(pPSA, &pPS);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		goto Error;

	hResult = OleSave(pPS, pStg, fSameAsLoad);

	if (pStg)
		pStg->Release();

Error:
	if (pPS)
		pPS->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleLoadFromStreamA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleLoadFromStreamA(LPSTREAMA pStmA, REFIID iidInterface,
		LPVOID * ppvObj)
{
	TraceSTDAPIEnter("OleLoadFromStreamA");
	LPSTREAM pStm;
	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = OleLoadFromStream(pStm, iidInterface, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(iidInterface);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObj);

	if (pStm)
		pStm->Release();

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleSaveToStreamA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleSaveToStreamA(LPPERSISTSTREAMA pPStmA, LPSTREAMA pStmA)
{
	TraceSTDAPIEnter("OleSaveToStreamA");
	LPPERSISTSTREAM pPStm;
	LPSTREAM pStm;
	HRESULT hResult;


	hResult = WrapIPersistStreamWFromA(pPStmA, &pPStm);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		goto Error;

	hResult = OleSaveToStream(pPStm, pStm);

	if (pStm)
		pStm->Release();

Error:
	if (pPStm)
		pPStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    RegisterDragDropA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI RegisterDragDropA(HWND hwnd, LPDROPTARGETA pDropTargetA)
{
	TraceSTDAPIEnter("RegisterDragDropA");
	LPDROPTARGET pDropTarget;
	HRESULT hResult;


	hResult = WrapIDropTargetWFromA(pDropTargetA, &pDropTarget);
	if (FAILED(hResult))
		return hResult;

	hResult = RegisterDragDrop(hwnd, pDropTarget);
	if (FAILED(hResult))
	{
		if (pDropTarget)
			pDropTarget->Release();
		return hResult;
	}

	if (!SetProp(hwnd, "Ole2AnsiProp", (HANDLE)pDropTarget))
	{
		RevokeDragDrop(hwnd);
		if (pDropTarget)
			pDropTarget->Release();
		return ResultFromScode(E_FAIL);
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    DoDragDropA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI DoDragDropA(LPDATAOBJECTA pDataObjA, LPDROPSOURCE pDropSource,
		DWORD dwOKEffects, LPDWORD pdwEffect)
{
	TraceSTDAPIEnter("DoDragDropA");
	LPDATAOBJECT pDataObj;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pDataObjA, &pDataObj);
	if (FAILED(hResult))
		return hResult;

	hResult = DoDragDrop(pDataObj, pDropSource, dwOKEffects, pdwEffect);

	if (pDataObj)
		pDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    RevokeDragDropA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI RevokeDragDropA(HWND hwnd)
{
	TraceSTDAPIEnter("RevokeDragDropA");
	LPDROPTARGET pDropTarget;
	HRESULT hResult;


	hResult = RevokeDragDrop(hwnd);
	if (FAILED(hResult))
		return hResult;

	pDropTarget = (LPDROPTARGET)GetProp(hwnd, "Ole2AnsiProp");
	if (pDropTarget)
		pDropTarget->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleSetClipboardA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleSetClipboardA(LPDATAOBJECTA pDataObjA)
{
	TraceSTDAPIEnter("OleSetClipboardA");
	LPDATAOBJECT pDataObj;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pDataObjA, &pDataObj);
	if (FAILED(hResult))
		return hResult;

	hResult = OleSetClipboard(pDataObj);

	if (pDataObj)
		pDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleGetClipboardA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleGetClipboardA(LPDATAOBJECTA * ppDataObjA)
{
	TraceSTDAPIEnter("OleGetClipboardA");
	LPDATAOBJECT pDataObj;
	HRESULT hReturn;
	HRESULT hResult;


	*ppDataObjA = NULL;

	hReturn = OleGetClipboard(&pDataObj);
	if (FAILED(hReturn))
		return hReturn;

	hResult = WrapIDataObjectAFromW(pDataObj, ppDataObjA);
	if (FAILED(hResult))
		hReturn = hResult;

	if (pDataObj)
		pDataObj->Release();

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleIsCurrentClipboardA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleIsCurrentClipboardA(LPDATAOBJECTA pDataObjA)
{
	TraceSTDAPIEnter("OleIsCurrentClipboardA");
	LPDATAOBJECT pDataObj;
	HRESULT hResult;


	hResult = WrapIDataObjectWFromA(pDataObjA, &pDataObj);
	if (FAILED(hResult))
		return hResult;

	hResult = OleIsCurrentClipboard(pDataObj);

	if (pDataObj)
		pDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleSetMenuDescriptorA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleSetMenuDescriptorA(HOLEMENU holemenu, HWND hwndFrame,
		HWND hwndActiveObject, LPOLEINPLACEFRAMEA lpFrameA,
	LPOLEINPLACEACTIVEOBJECTA lpActiveObjectA)
{
	TraceSTDAPIEnter("OleSetMenuDescriptorA");
	LPOLEINPLACEFRAME lpFrame;
	LPOLEINPLACEACTIVEOBJECT lpActiveObject;
	HRESULT hResult;


	hResult = WrapIOleInPlaceFrameWFromA(lpFrameA, &lpFrame);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIOleInPlaceActiveObjectWFromA(lpActiveObjectA, &lpActiveObject);
	if (FAILED(hResult))
	goto Error;

	hResult = OleSetMenuDescriptor(holemenu, hwndFrame, hwndActiveObject,
		lpFrame, lpActiveObject);

	if (lpActiveObject)
		lpActiveObject->Release();

Error:
	if (lpFrame)
		lpFrame->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleTranslateAcceleratorA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleTranslateAcceleratorA(LPOLEINPLACEFRAMEA lpFrameA,
		LPOLEINPLACEFRAMEINFO lpFrameInfo, LPMSG lpmsg)
{
	TraceSTDAPIEnter("OleTranslateAcceleratorA");
	LPOLEINPLACEFRAME lpFrame;
	HRESULT hResult;


	hResult = WrapIOleInPlaceFrameWFromA(lpFrameA, &lpFrame);
	if (FAILED(hResult))
		return hResult;

	hResult = OleTranslateAccelerator(lpFrame, lpFrameInfo, lpmsg);

	if (lpFrame)
		lpFrame->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleIsRunningA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(BOOL) OleIsRunningA(LPOLEOBJECTA pObjectA)
{
	TraceSTDAPIEnter("OleIsRunningA");

	LPOLEOBJECT pObject;
	HRESULT hResult;
   BOOL    fResult;


	hResult = WrapIOleObjectWFromA(pObjectA, &pObject);
	if (FAILED(hResult))
		return FALSE;

	fResult = OleIsRunning(pObject);

	if (pObject)
		pObject->Release();

	return fResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ReleaseStgMediumA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(void) ReleaseStgMediumA(LPSTGMEDIUMA pStgMediumA)
{
	TraceSTDAPIEnter("ReleaseStgMediumA");

	ConvertSTGMEDIUMFree(0, pStgMediumA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateOleAdviseHolderA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateOleAdviseHolderA(LPOLEADVISEHOLDERA * ppOAHolderA)
{
	TraceSTDAPIEnter("CreateOleAdviseHolderA");
	LPOLEADVISEHOLDER pOAHolder;
	HRESULT hResult;


	*ppOAHolderA = NULL;

	hResult = CreateOleAdviseHolder(&pOAHolder);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIOleAdviseHolderAFromW(pOAHolder, ppOAHolderA);

	if (pOAHolder)
		pOAHolder->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleGetIconOfFileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(HGLOBAL) OleGetIconOfFileA(LPSTR lpszPathA, BOOL fUseFileAsLabel)
{
	TraceSTDAPIEnter("OleGetIconOfFileA");

	OLECHAR szPath[MAX_PATH];
	OLECHAR *pPath;


	if (lpszPathA)
	{
		ConvertStringToW(lpszPathA, szPath);
		pPath = szPath;
	}
	else
		pPath = NULL;

	return OleGetIconOfFile(pPath, fUseFileAsLabel);
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleGetIconOfClassA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(HGLOBAL) OleGetIconOfClassA(REFCLSID rclsid, LPSTR lpszLabelA,
		BOOL fUseTypeAsLabel)
{
	TraceSTDAPIEnter("OleGetIconOfClassA");

	OLECHAR szLabel[MAX_PATH];
	OLECHAR *pLabel;

	if (lpszLabelA != NULL)
	{
		ConvertStringToW(lpszLabelA, szLabel);
		pLabel = szLabel;
	}
	else
		pLabel = NULL;

	return OleGetIconOfClass(rclsid, pLabel, fUseTypeAsLabel);
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleMetafilePictFromIconAndLabelA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(HGLOBAL) OleMetafilePictFromIconAndLabelA(HICON hIcon,
		LPSTR lpszLabelA, LPSTR lpszSourceFileA, UINT iIconIndex)
{
	TraceSTDAPIEnter("OleMetafilePictFromIconAndLabelA");

	LPOLESTR lpszLabel;
	LPOLESTR lpszSourceFile;
	HGLOBAL hGlobal;
	HRESULT hResult;


	hGlobal = NULL;

	hResult = ConvertStringToW(lpszLabelA, &lpszLabel);
	if (FAILED(hResult))
		return NULL;

	hResult = ConvertStringToW(lpszSourceFileA, &lpszSourceFile);
	if (FAILED(hResult))
		goto Error;

	hGlobal = OleMetafilePictFromIconAndLabel(hIcon, lpszLabel,
			lpszSourceFile, iIconIndex);

	ConvertStringFree(lpszSourceFile);

Error:
	ConvertStringFree(lpszLabel);

	return hGlobal;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleRegGetUserTypeA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleRegGetUserTypeA(REFCLSID clsid, DWORD dwFormOfType,
		LPSTR * pszUserTypeA)
{
	TraceSTDAPIEnter("OleRegGetUserTypeA");
	LPOLESTR pszUserType;
   HRESULT hResult;


	hResult = OleRegGetUserType(clsid, dwFormOfType, &pszUserType);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToA(pszUserType, pszUserTypeA);

	ConvertStringFree(pszUserType);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleRegEnumFormatEtcA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleRegEnumFormatEtcA(REFCLSID clsid, DWORD dwDirection,
		LPENUMFORMATETCA * ppenumA)
{
	TraceSTDAPIEnter("OleRegEnumFormatEtcA");
	LPENUMFORMATETC penum;
	HRESULT hResult;


	*ppenumA = NULL;

	hResult = OleRegEnumFormatEtc(clsid, dwDirection, &penum);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumFORMATETCAFromW(penum, ppenumA);

	if (penum)
		penum->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleRegEnumVerbsA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleRegEnumVerbsA(REFCLSID clsid, LPENUMOLEVERBA * ppenumA)
{
	TraceSTDAPIEnter("OleRegEnumVerbsA");
	LPENUMOLEVERB penum;
	HRESULT hResult;


	*ppenumA = NULL;

	hResult = OleRegEnumVerbs(clsid, &penum);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumOLEVERBAFromW(penum, ppenumA);

	if (penum)
		penum->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    GetHGlobalFromILockBytesA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI GetHGlobalFromILockBytesA(LPLOCKBYTESA plkbytA, HGLOBAL * phglobal)
{
	TraceSTDAPIEnter("GetHGlobalFromILockBytesA");
	LPLOCKBYTES plkbyt;
	HRESULT hResult;


	hResult = WrapILockBytesWFromA(plkbytA, &plkbyt);
	if (FAILED(hResult))
		return hResult;

	hResult = GetHGlobalFromILockBytes(plkbyt, phglobal);

	if (plkbyt)
		plkbyt->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateILockBytesOnHGlobalA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateILockBytesOnHGlobalA(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
		LPLOCKBYTESA * pplkbytA)
{
	TraceSTDAPIEnter("CreateILockBytesOnHGlobalA");
	LPLOCKBYTES plkbyt;
	HRESULT hResult;


	*pplkbytA = NULL;

	hResult = CreateILockBytesOnHGlobal(hGlobal, fDeleteOnRelease, &plkbyt);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapILockBytesAFromW(plkbyt, pplkbytA);

	if (plkbyt)
		plkbyt->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    GetHGlobalFromStreamA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI GetHGlobalFromStreamA(LPSTREAMA pStmA, HGLOBAL * phglobal)
{
	TraceSTDAPIEnter("GetHGlobalFromStreamA");
	LPSTREAM pStm;
	HRESULT hResult;


	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetHGlobalFromStream(pStm, phglobal);

	if (pStm)
		pStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateStreamOnHGlobalA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateStreamOnHGlobalA(HGLOBAL hGlobal, BOOL fDeleteOnRelease,
		LPSTREAMA * ppstmA)
{
	TraceSTDAPIEnter("CreateStreamOnHGlobalA");
	LPSTREAM pstm;
	HRESULT hResult;


	*ppstmA = NULL;

	hResult = CreateStreamOnHGlobal(hGlobal, fDeleteOnRelease, &pstm);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIStreamAFromW(pstm, ppstmA);

	if (pstm)
		pstm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleDoAutoConvertA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleDoAutoConvertA(LPSTORAGEA pStgA, LPCLSID pClsidNew)
{
	TraceSTDAPIEnter("OleDoAutoConvertA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = OleDoAutoConvert(pStg, pClsidNew);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    GetConvertStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI GetConvertStgA(LPSTORAGEA pStgA)
{
	TraceSTDAPIEnter("GetConvertStgA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = GetConvertStg(pStg);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    SetConvertStgA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI SetConvertStgA(LPSTORAGEA pStgA, BOOL fConvert)
{
	TraceSTDAPIEnter("SetConvertStgA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = SetConvertStg(pStg, fConvert);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleConvertOLESTREAMToIStorageA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleConvertOLESTREAMToIStorageA(LPOLESTREAM lpolestream,
		LPSTORAGEA pStgA, const DVTARGETDEVICE * ptd)
{
	TraceSTDAPIEnter("OleConvertOLESTREAMToIStorageA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = OleConvertOLESTREAMToIStorage(lpolestream, pStg, ptd);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    OleConvertIStorageToOLESTREAMA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Note:       See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI OleConvertIStorageToOLESTREAMA(LPSTORAGEA pStgA,
		LPOLESTREAM lpolestream)
{
	TraceSTDAPIEnter("OleConvertIStorageToOLESTREAMA");
	LPSTORAGE pStg;
	HRESULT hResult;


	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = OleConvertIStorageToOLESTREAM(pStg, lpolestream);

	if (pStg)
		pStg->Release();

	return hResult;
}
