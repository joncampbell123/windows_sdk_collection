/*****************************************************************************\
*                                                                             *
*    OleInPlaceServerDocument.c                                               *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "Gopher.h"
#include "App.h"
#include "Line.h"
#include "Doc.h"
#include "OleDebug.h"
#include "OleXcept.h"
#include "Const.h"
#include "Doc.h"
#if qFrameTools
	#include "Layers.h"
#endif
#include "Util.h"
#include "PObj.h"
#include "stdio.h"
#include <ToolUtils.h>
#include <Resources.h>

// OLDNAME: OleServerDocInterface.c

extern OleApplicationPtr		gOleApp;

static IOleObjectVtbl			gServerDocIOleObjectVtbl;
static IPersistStorageVtbl		gServerDocIPersistStorageVtbl;

#if qOleTreatAs
static IStdMarshalInfoVtbl		gServerDocIStdMarshalInfoVtbl;
#endif	// qOleTreatAs

//#pragma segment OleServerDocInterfaceInitSeg
void OleServerDocInitInterfaces(void)
{
	// ServerDoc::IOleObject method table
	{
		IOleObjectVtbl*		p;

		p = &gServerDocIOleObjectVtbl;

		p->QueryInterface		= IServerDocOleObjectQueryInterface;
		p->AddRef				= IServerDocOleObjectAddRef;
		p->Release				= IServerDocOleObjectRelease;
		p->SetClientSite		= IServerDocOleObjectSetClientSite;
		p->GetClientSite		= IServerDocOleObjectGetClientSite;
		p->SetHostNames			= IServerDocOleObjectSetHostNames;
		p->Close				= IServerDocOleObjectClose;
		p->SetMoniker			= IServerDocOleObjectSetMoniker;
		p->GetMoniker			= IServerDocOleObjectGetMoniker;
		p->InitFromData			= IServerDocOleObjectInitFromData;
		p->GetClipboardData		= IServerDocOleObjectGetClipboardData;
		p->DoVerb				= IServerDocOleObjectDoVerb;
		p->EnumVerbs			= IServerDocOleObjectEnumVerbs;
		p->Update				= IServerDocOleObjectUpdate;
		p->IsUpToDate			= IServerDocOleObjectIsUpToDate;
		p->GetUserClassID		= IServerDocOleObjectGetUserClassID;
		p->GetUserType			= IServerDocOleObjectGetUserType;
		p->SetExtent			= IServerDocOleObjectSetExtent;
		p->GetExtent			= IServerDocOleObjectGetExtent;
		p->Advise				= IServerDocOleObjectAdvise;
		p->Unadvise				= IServerDocOleObjectUnadvise;
		p->EnumAdvise			= IServerDocOleObjectEnumAdvise;
		p->GetMiscStatus		= IServerDocOleObjectGetMiscStatus;
		p->SetColorScheme		= IServerDocOleObjectSetColorScheme;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	// ServerDoc::IPersistStorage method table
	{
		IPersistStorageVtbl*	p;

		p = &gServerDocIPersistStorageVtbl;

		p->QueryInterface		= IServerDocPersistStorageQueryInterface;
		p->AddRef				= IServerDocPersistStorageAddRef;
		p->Release				= IServerDocPersistStorageRelease;
		p->GetClassID			= IServerDocPersistStorageGetClassID;
		p->IsDirty				= IServerDocPersistStorageIsDirty;
		p->InitNew				= IServerDocPersistStorageInitNew;
		p->Load					= IServerDocPersistStorageLoad;
		p->Save					= IServerDocPersistStorageSave;
		p->SaveCompleted		= IServerDocPersistStorageSaveCompleted;
		p->HandsOffStorage		= IServerDocPersistStorageHandsOffStorage;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

#if qOleTreatAs
	// ServerDoc::IStdMarshalInfo method table
	{
		IStdMarshalInfoVtbl*	p;

		p = &gServerDocIStdMarshalInfoVtbl;

		p->QueryInterface		= IServerDocStdMarshalInfoQueryInterface;
		p->AddRef				= IServerDocStdMarshalInfoAddRef;
		p->Release				= IServerDocStdMarshalInfoRelease;
		p->GetClassForHandler	= IServerDocStdMarshalInfoGetClassForHandler;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
#endif // qOleTreatAs

}

//#pragma segment OleServerDocInterfaceSeg
void ServerDocIOleObjectInit(ServerDocOleObjectImplPtr pServerDocOleObjectImpl, struct OleDocumentRec* pOleDoc)
{
	pServerDocOleObjectImpl->lpVtbl			= &gServerDocIOleObjectVtbl;
	pServerDocOleObjectImpl->lpOleDoc		= pOleDoc;
	pServerDocOleObjectImpl->cRef			= 0;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectQueryInterface(LPOLEOBJECT lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocOleObjectAddRef(LPOLEOBJECT lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocOleObjectRelease(LPOLEOBJECT lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectSetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE lpclientSite)
{
	OleDocumentPtr			pOleDoc;
	OleServerDocPtr			pOleServerDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;
	
	// SetClientSite is only valid to call on an embedded object
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	ASSERTCOND((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kEmbeddedDocumentType);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) != kEmbeddedDocumentType)
		return ResultFromScode(E_UNEXPECTED);

	// if we currently have a client site ptr, then release it.
	if (pOleServerDoc->m_lpOleClientSite)
		OleStdRelease((LPUNKNOWN)pOleServerDoc->m_lpOleClientSite);
		
	pOleServerDoc->m_lpOleClientSite = (LPOLECLIENTSITE)lpclientSite;
	if (lpclientSite)
		lpclientSite->lpVtbl->AddRef(lpclientSite);

	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetClientSite(LPOLEOBJECT lpThis, LPOLECLIENTSITE * lplpClientSite)
{
	OleServerDocPtr		pOleServerDoc;
	
	OleDbgEnterInterface();
	
	pOleServerDoc = &(((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc)->server;

	pOleServerDoc->m_lpOleClientSite->lpVtbl->AddRef(pOleServerDoc->m_lpOleClientSite);
	*lplpClientSite = pOleServerDoc->m_lpOleClientSite;

	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectSetHostNames(LPOLEOBJECT lpThis, const char* szContainerApp, const char* szContainerObj)
{
	OleDocumentPtr		pOleDoc;
	OleServerDocPtr		pOleServerDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;
	
	strcpy(pOleServerDoc->m_szContainerApp, szContainerApp);
	strcpy(pOleServerDoc->m_szContainerObj, szContainerObj);

	sprintf(pOleServerDoc->m_szObjectName, "%s - %s", szContainerApp, szContainerObj);

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetWindowTitleProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetWindowTitleProcPtr)(pOleDoc->m_pIDoc, (StringPtr)c2pstr(pOleServerDoc->m_szObjectName));
	p2cstr((StringPtr)pOleServerDoc->m_szObjectName);

	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectClose(LPOLEOBJECT lpThis, unsigned long dwSaveOption)
{
	OleDocumentPtr	pOleDoc;
	YNCResult		res;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

    /* OLE2NOTE: in order to have a stable app and doc during the
    **    process of closing, we intially AddRef the App and Doc ref
    **    cnts and later Release them. These initial AddRefs are
    **    artificial; they simply guarantee that these objects do not
    **    get destroyed until the end of this routine.
    */
	OleAppAddRef(gOleApp);
	OleDocAddRef(pOleDoc);

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_DoCloseProcPtr != nil);
	res = (*pOleDoc->m_pIDoc->lpVtbl->m_DoCloseProcPtr)(
				pOleDoc->m_pIDoc,
				(Boolean)(dwSaveOption == OLECLOSE_PROMPTSAVE ? true : false),
				(YNCResult)(dwSaveOption == OLECLOSE_SAVEIFDIRTY ? yesResult : noResult),
				false);
	
    /* OLE2NOTE: this call forces all external connections to our
    **    object to close down and therefore guarantees that we receive
    **    all releases associated with those external connections.
    */
	CoDisconnectObject(pOleDoc->m_pIUnknown, 0);
	
	// release artificial AddRef above
	OleDocRelease(pOleDoc);
	OleAppRelease(gOleApp);
		
	return (res == yesResult || res == noResult) ? NOERROR : ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectSetMoniker(LPOLEOBJECT lpThis, unsigned long dwWhichMoniker, LPMONIKER lpmk)
{
	OleDocumentPtr			pOleDoc;
	OleServerDocPtr			pOleServerDoc;
	LPMONIKER				lpmkFull;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;
	
	if (pOleServerDoc->m_lpOleClientSite == NULL)
		return ResultFromScode(E_FAIL);

	pOleServerDoc->m_lpOleClientSite->lpVtbl->GetMoniker(
										pOleServerDoc->m_lpOleClientSite,
										OLEGETMONIKER_ONLYIFTHERE,
										OLEWHICHMK_OBJFULL,
										&lpmkFull);

	OleDocRenamedUpdate(pOleDoc, lpmkFull);
	
	if (lpmkFull)
		OleStdRelease((LPUNKNOWN)lpmkFull);

	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetMoniker(LPOLEOBJECT lpThis, unsigned long dwAssign, unsigned long dwWhichMoniker, LPMONIKER * lplpmk)
{
	OleDocumentPtr			pOleDoc;
	OleServerDocPtr			pOleServerDoc;
	HRESULT					hrErr;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;
	
	*lplpmk = NULL;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);

	if (pOleServerDoc->m_lpOleClientSite != NULL)
	{
		ASSERTCOND((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kEmbeddedDocumentType);
		
		hrErr = pOleServerDoc->m_lpOleClientSite->lpVtbl->GetMoniker(
										pOleServerDoc->m_lpOleClientSite,
										dwAssign,
										dwWhichMoniker,
										lplpmk);
		ASSERTNOERROR(hrErr);
	}
	else if ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kFromFileDocumentType)
	{
		FSSpec fs;
		
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_GetFSSpecProcPtr)(pOleDoc->m_pIDoc, &fs);
		hrErr = CreateFileMonikerFSp(&fs, lplpmk);
		ASSERTNOERROR(hrErr);
	}
	else
		hrErr = ResultFromScode(E_FAIL);

	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectInitFromData(LPOLEOBJECT lpThis, LPDATAOBJECT lpDataObject, unsigned long fCreation, unsigned long reserved)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetClipboardData(LPOLEOBJECT lpThis, unsigned long reserved, LPDATAOBJECT * lplpDataObject)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectDoVerb(LPOLEOBJECT lpThis, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, long lindex, WindowPtr hwndParent, LPCRECT lprcPosRect)
{
	OleDocumentPtr			pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	return OleServerDocOleObjectDoVerb(pOleDoc, lVerb, lpmsg, lpActiveSite, lindex, hwndParent, lprcPosRect);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectEnumVerbs(LPOLEOBJECT lpThis, LPENUMOLEVERB * lplpenumOleVerb)
{
	OleDbgEnterInterface();
	
	*lplpenumOleVerb = NULL;

	return ResultFromScode(OLE_S_USEREG);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectUpdate(LPOLEOBJECT lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: a server-only app is always "up-to-date".
    **    a container-app which contains links where the link source
    **    has changed since the last update of the link would be
    **    considered "out-of-date". the "Update" method instructs the
    **    object to get an update from any out-of-date links.
    */

    return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectIsUpToDate(LPOLEOBJECT lpThis)
{
	OleDbgEnterInterface();

    /* OLE2NOTE: a server-only app is always "up-to-date".
    **    a container-app which contains links where the link source
    **    has changed since the last update of the link would be
    **    considered "out-of-date".
    */

    return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetUserClassID(LPOLEOBJECT lpThis, LPCLSID lpclsid)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, lpclsid, nil);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetUserType(LPOLEOBJECT lpThis, unsigned long dwFormOfType, char* * lpszUserType)
{
	OleDbgEnterInterface();

	return ResultFromScode(OLE_S_USEREG);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectSetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lpsizel)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

#if qOleInPlace
    if (dwDrawAspect != DVASPECT_CONTENT)
		return ResultFromScode(S_FALSE);

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;

	OleInPlaceServerDocSetExtent(pOleDoc, lpsizel);

	return ResultFromScode(S_OK);
#endif

	return ResultFromScode(S_FALSE);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetExtent(LPOLEOBJECT lpThis, unsigned long dwDrawAspect, LPSIZEL lpsizel)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	
	if (dwDrawAspect == DVASPECT_CONTENT ||
		dwDrawAspect == DVASPECT_THUMBNAIL ||
		dwDrawAspect == DVASPECT_DOCPRINT)
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr)(pOleDoc->m_pIDoc, nil, lpsizel);
		return S_OK;
	}

	return ResultFromScode(E_INVALIDARG);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectAdvise(LPOLEOBJECT lpThis, LPADVISESINK lpAdvSink, unsigned long* lpdwConnection)
{
	OleServerDocPtr		pOleServerDoc;
	HRESULT				hrErr;
	
	OleDbgEnterInterface();
	
	pOleServerDoc = &(((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc)->server;

	if (pOleServerDoc->m_lpOleAdviseHolder == NULL)
	{
		hrErr = CreateOleAdviseHolder(&pOleServerDoc->m_lpOleAdviseHolder);
		ASSERTNOERROR(hrErr);
		if (hrErr != NOERROR)
			return hrErr;
	}
	
	hrErr = pOleServerDoc->m_lpOleAdviseHolder->lpVtbl->Advise(
							pOleServerDoc->m_lpOleAdviseHolder,
							lpAdvSink,
							lpdwConnection);
	ASSERTNOERROR(hrErr);

	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectUnadvise(LPOLEOBJECT lpThis, unsigned long dwConnection)
{
	OleServerDocPtr		pOleServerDoc;
	HRESULT				hrErr;

	OleDbgEnterInterface();
	
	pOleServerDoc = &(((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc)->server;

	if (pOleServerDoc->m_lpOleAdviseHolder == NULL)
		return ResultFromScode(E_FAIL);

	hrErr = pOleServerDoc->m_lpOleAdviseHolder->lpVtbl->Unadvise(pOleServerDoc->m_lpOleAdviseHolder, dwConnection);
	ASSERTNOERROR(hrErr);
	
	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectEnumAdvise(LPOLEOBJECT lpThis, LPENUMSTATDATA * lplpenumAdvise)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectGetMiscStatus(LPOLEOBJECT lpThis, unsigned long dwAspect, unsigned long * lpdwStatus)
{
	OleDocumentPtr	pOleDoc;
	OleDocumentType	docType;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	
	*lpdwStatus = 0;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	docType = (*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc);
	
	if (docType == kNewDocumentType || docType == kFromFileDocumentType)
		*lpdwStatus |= OLEMISC_CANLINKBYOLE1;

#if qOleInPlace
		if (dwAspect == DVASPECT_CONTENT)
			*lpdwStatus |= (OLEMISC_INSIDEOUT | OLEMISC_ACTIVATEWHENVISIBLE);
#endif

	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectSetColorScheme(LPOLEOBJECT lpThis, LPOLECOLORSCHEME lpLogpal)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocOleObjectLockObject(LPOLEOBJECT lpThis, unsigned long fLock)
{
	OleDbgEnterInterface();

return ResultFromScode(E_FAIL);
}

//#pragma segment OleServerDocInterfaceSeg
void ServerDocIPersistStorageInit(ServerDocPersistStorageImplPtr pServerDocPersistStorageImpl, struct OleDocumentRec* pOleDoc)
{
	pServerDocPersistStorageImpl->lpVtbl		= &gServerDocIPersistStorageVtbl;
	pServerDocPersistStorageImpl->lpOleDoc		= pOleDoc;
	pServerDocPersistStorageImpl->cRef			= 0;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageQueryInterface(LPPERSISTSTORAGE lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocPersistStorageAddRef(LPPERSISTSTORAGE lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocPersistStorageRelease(LPPERSISTSTORAGE lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageGetClassID(LPPERSISTSTORAGE lpThis, LPCLSID lpClassID)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr != nil);
	return (*pOleDoc->m_pIDoc->lpVtbl->m_GetClassIDProcPtr)(pOleDoc->m_pIDoc, lpClassID, nil);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageIsDirty(LPPERSISTSTORAGE lpThis)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDirtyProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsDirtyProcPtr)(pOleDoc->m_pIDoc))
		hrErr = NOERROR;
	else
		hrErr = ResultFromScode(S_FALSE);

	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageInitNew(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg)
{
	OleDocumentPtr		pOleDoc;
	HRESULT				hrErr;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocPersistStorageImplPtr)lpThis)->lpOleDoc;

	hrErr = OleServerDocInitNewEmbedded(pOleDoc);
	ASSERTNOERROR(hrErr);
	
	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageLoad(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg)
{
	OleDocumentPtr		pOleDoc;
	OleServerDocPtr		pOleServerDoc;
	HRESULT				hrErr;
	
	OleDbgEnterInterface();

	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	ASSERTCOND((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kUnknownDocumentType);

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDocTypeProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetDocTypeProcPtr)(pOleDoc->m_pIDoc, kEmbeddedDocumentType);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_LoadFromStgProcPtr != nil);
	hrErr = (*pOleDoc->m_pIDoc->lpVtbl->m_LoadFromStgProcPtr)(pOleDoc->m_pIDoc, lpStg);
	ASSERTNOERROR(hrErr);
	
	{
		OutlineDocPtr	pOutlineDoc;
		DocumentPtr		pDocument;

		pOutlineDoc = (OutlineDocPtr)((OutlineDocDocImplPtr)pOleDoc->m_pIDoc)->lpOleOutlineDoc;

		pOutlineDoc->m_LineList->m_fForceUpdate = true;
		LineListUpdateView(pOutlineDoc->m_LineList);

		pDocument = (DocumentPtr)pOutlineDoc;

		ASSERTCOND(pDocument->vtbl->m_SetDirtyProcPtr != nil);
		(*pDocument->vtbl->m_SetDirtyProcPtr)(pDocument, false);
	}

	if (hrErr == NOERROR)
	{
		pOleServerDoc->m_dwStorageMode = STGMODE_NORMAL;
		
		if (GetConvertStg(lpStg) == NOERROR)
		{
			SetConvertStg(lpStg, FALSE);

			// set document dirty
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr)(pOleDoc->m_pIDoc, true);
			
			OleDocSetSizeChanged(pOleDoc, true);
		}
	}

	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageSave(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStg, unsigned long fSameAsLoad)
{
	OleDocumentPtr			pOleDoc;
	OleServerDocPtr			pOleServerDoc;
	HRESULT					hrErr;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr != nil);
	hrErr = (*pOleDoc->m_pIDoc->lpVtbl->m_SaveToStgProcPtr)(pOleDoc->m_pIDoc, 0, lpStg, true);
	ASSERTNOERROR(hrErr);
	
	pOleServerDoc->m_fSaveWithSameAsLoad = fSameAsLoad;
	pOleServerDoc->m_fNoScribbleMode = true;

	if (hrErr == NOERROR)
		pOleServerDoc->m_dwStorageMode = STGMODE_NOSCRIBBLE;
		
	return hrErr;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageSaveCompleted(LPPERSISTSTORAGE lpThis, LPSTORAGE lpStgNew)
{
	OleDocumentPtr			pOleDoc;
	OleServerDocPtr			pOleServerDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc;
	pOleServerDoc = &pOleDoc->server;
	
	pOleServerDoc->m_dwStorageMode = STGMODE_NORMAL;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	
	if (((pOleServerDoc->m_fSaveWithSameAsLoad && lpStgNew == NULL) || lpStgNew)
			&& ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) != kEmbeddedDocumentType))
        return ResultFromScode(E_INVALIDARG);

	if (lpStgNew || pOleServerDoc->m_fSaveWithSameAsLoad)
	{
		if (pOleServerDoc->m_fNoScribbleMode)
		{
			// set document dirty
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr)(pOleDoc->m_pIDoc, false);
	
			OleServerDocSendAdvise(pOleDoc, OLE_ONSAVE, NULL, 0);
		}

		pOleServerDoc->m_fSaveWithSameAsLoad = false;
	}
	pOleServerDoc->m_fNoScribbleMode = false;
	
	return NOERROR;
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocPersistStorageHandsOffStorage(LPPERSISTSTORAGE lpThis)
{
	OleServerDocPtr			pOleServerDoc;

	OleDbgEnterInterface();
	
	pOleServerDoc = &(((ServerDocOleObjectImplPtr)lpThis)->lpOleDoc)->server;

	pOleServerDoc->m_dwStorageMode = STGMODE_HANDSOFF;

	return NOERROR;
}

#if qOleTreatAs

//#pragma segment OleServerDocInterfaceSeg
void ServerDocIStdMarshalInfoInit(ServerDocStdMarshalInfoImplPtr pServerDocStdMarshalInfoImpl, struct OleDocumentRec* pOleDoc)
{
	pServerDocStdMarshalInfoImpl->lpVtbl		= &gServerDocIStdMarshalInfoVtbl;
	pServerDocStdMarshalInfoImpl->lpOleDoc		= pOleDoc;
	pServerDocStdMarshalInfoImpl->cRef			= 0;
}

//#pragma segment OlekDocInterfaceSeg
STDMETHODIMP IServerDocStdMarshalInfoQueryInterface(LPSTDMARSHALINFO lpThis, REFIID riid, void* * lplpvObj)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocStdMarshalInfoImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, lplpvObj);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocStdMarshalInfoAddRef(LPSTDMARSHALINFO lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocStdMarshalInfoImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP_(unsigned long) IServerDocStdMarshalInfoRelease(LPSTDMARSHALINFO lpThis)
{
	OleDocumentPtr	pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((ServerDocStdMarshalInfoImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleServerDocInterfaceSeg
STDMETHODIMP IServerDocStdMarshalInfoGetClassForHandler(LPSTDMARSHALINFO lpThis, unsigned long dwDestContext, void* pvDestContext, LPCLSID lpClassID)
{
	OleDbgEnterInterface();

	// OLE2NOTE: we only handle LOCAL marshal context.
	if (dwDestContext != MSHCTX_LOCAL || pvDestContext != NULL)
		return ResultFromScode(E_INVALIDARG);

	/* OLE2NOTE: we must return our REAL clsid, NOT the clsid that we
	**    are pretending to be if a "TreatAs" is in effect.
	*/
	*lpClassID = CLSID_Application;		// REVIEW: may want to use IDoc to retrieve it
	
	return NOERROR;
}

// OLDNAME: OleServerDocument.c
extern ApplicationPtr		gApplication;
extern OleApplicationPtr	gOleApp;

#ifndef _MSC_VER
#define STGMEDIUM_HGLOBAL	u.hGlobal
#else
#define STGMEDIUM_HGLOBAL	hGlobal
#endif

//#pragma segment OleServerDocInitSeg
void OleServerDocInit(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;
	
	pOleServerDoc->m_cPseudoObj				= 0;
	pOleServerDoc->m_lpOleClientSite		= NULL;
	pOleServerDoc->m_lpOleAdviseHolder		= NULL;
	pOleServerDoc->m_lpDataAdviseHolder		= NULL;
	pOleServerDoc->m_fEnableDraw			= true;
	pOleServerDoc->m_dwStorageMode			= STGMODE_HANDSOFF;
	pOleServerDoc->m_fSendDataOnStop		= false;

	pOleServerDoc->m_fSaveWithSameAsLoad	= false;

	pOleServerDoc->m_szObjectName[0]		= '\0';

	pOleServerDoc->m_szContainerApp[0]		= '\0';
	pOleServerDoc->m_szContainerObj[0]		= '\0';
	
	pOleServerDoc->m_DataChanged			= false;
	pOleServerDoc->m_SizeChanged			= false;

	pOleServerDoc->m_fSetFileMenu			= false;
	pOleServerDoc->m_sOldCloseMenuItem[0]	= 0;
	pOleServerDoc->m_sOldSaveMenuItem[0] 	= 0;
	pOleServerDoc->m_sOldSaveAsMenuItem[0]	= 0;
	pOleServerDoc->m_sOldQuitMenuItem[0] 	= 0;

	ServerDocIOleObjectInit(&pOleServerDoc->m_OleObject, pOleDoc);
	ServerDocIPersistStorageInit(&pOleServerDoc->m_PersistStorage, pOleDoc);
	
#if qOleTreatAs
	ServerDocIStdMarshalInfoInit(&pOleServerDoc->m_StdMarshalInfo, pOleDoc);
#endif // qOleTreatAs

#if qOleInPlace
	OleInPlaceServerDocInit(pOleDoc);
#endif
}

//#pragma segment OleServerDocInitSeg
void OleServerDocDispose(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;

	if (pOleServerDoc->m_lpDataAdviseHolder)
	{
		OleStdVerifyRelease((LPUNKNOWN)pOleServerDoc->m_lpDataAdviseHolder);
		pOleServerDoc->m_lpDataAdviseHolder = nil;
	}
		
	if (pOleServerDoc->m_lpOleAdviseHolder)
	{
		OleStdVerifyRelease((LPUNKNOWN)pOleServerDoc->m_lpOleAdviseHolder);
		pOleServerDoc->m_lpOleAdviseHolder = nil;
	}

	if (pOleServerDoc->m_lpOleClientSite)
	{
		OleStdRelease((LPUNKNOWN)pOleServerDoc->m_lpOleClientSite);
		pOleServerDoc->m_lpOleClientSite = nil;
	}
	
#if qOleInPlace
	OleInPlaceServerDocDispose(pOleDoc);
#endif
}

//#pragma segment OleServerDocSeg
void OleServerDocShow(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr		pOleServerDoc;
	OleDocumentType		docType;
	
	pOleServerDoc = &pOleDoc->server;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	docType = (*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc);
	
	if (docType == kEmbeddedDocumentType && pOleServerDoc->m_lpOleClientSite != NULL)
	{
		/* OLE2NOTE: we must also ask our container to show itself if
		**    it is not already visible and to scroll us into view. we
		**    must make sure to call this BEFORE showing our server's
		**    window and taking focus. we do not want our container's
		**    window to end up on top.
		*/
		pOleServerDoc->m_lpOleClientSite->lpVtbl->ShowObject(pOleServerDoc->m_lpOleClientSite);


#if qOleInPlace
		if (OleInPlaceServerDocIsInPlace(pOleDoc))
		{
			OleInPlaceServerDocShow(pOleDoc, BEGIN_IP_OnUIActivate);
		}
		else
		{
#endif
	        /* OLE2NOTE: if we are an embedded object and we are not
	        **    in-place active in our containers window, we must inform our
	        **    embedding container that our window is opening.
	        **    the container must now hatch our object.
	        */
         	pOleServerDoc->m_lpOleClientSite->lpVtbl->OnShowWindow(pOleServerDoc->m_lpOleClientSite, true);
#if qOleInPlace
		}
#endif
	}
}

//#pragma segment OleServerDocSeg
void OleServerDocHide(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;

#if qOleInPlace
	if (!OleInPlaceServerDocIsInPlace(pOleDoc))
#endif
	{
		if (pOleServerDoc->m_lpOleClientSite != nil)
			pOleServerDoc->m_lpOleClientSite->lpVtbl->OnShowWindow(pOleServerDoc->m_lpOleClientSite, false);
	}

#if qOleInPlace
	if (OleInPlaceServerDocIsInPlace(pOleDoc))
		OleInPlaceServerDocHide(pOleDoc, END_IP_OnDocDeactivate);
#endif
}

//#pragma segment OleServerDocSeg
void OleServerDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive)
{
#if qOleInPlace
	if (OleInPlaceServerDocIsInPlace(pOleDoc))
	{
		OleInPlaceServerDocDoActivate(pOleDoc, becomingActive);
	
		// if we are inplace active, the file menu items are not ours, therefore we do not update them
		return;
	}
#endif

	// if a embedded document is changing state update the menus accordingly
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kEmbeddedDocumentType)
	{
		if (becomingActive)
		{
			Str255		str;
			
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr)(gOleApp->m_pIApp, cmdClose, pOleDoc->server.m_sOldCloseMenuItem);

			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSave, pOleDoc->server.m_sOldSaveMenuItem);

			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSaveAs, pOleDoc->server.m_sOldSaveAsMenuItem);
			
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_GetCmdItemProcPtr)(gOleApp->m_pIApp, cmdQuit, pOleDoc->server.m_sOldQuitMenuItem);

			GetIndString(str, kServerMenu_STRs, kServerMenu_STRs_Close);
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdClose, str);

			GetIndString(str, kServerMenu_STRs, kServerMenu_STRs_Save);
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSave, str);

			GetIndString(str, kServerMenu_STRs, kServerMenu_STRs_SaveAs);
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSaveAs, str);
			
			GetIndString(str, kServerMenu_STRs, kServerMenu_STRs_Quit);
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdQuit, str);

			pOleDoc->server.m_fSetFileMenu = true;
		}
		else if (pOleDoc->server.m_fSetFileMenu)
		{
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdClose, pOleDoc->server.m_sOldCloseMenuItem);

			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSave, pOleDoc->server.m_sOldSaveMenuItem);

			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdSaveAs, pOleDoc->server.m_sOldSaveAsMenuItem);
			
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_SetCmdItemProcPtr)(gOleApp->m_pIApp, cmdQuit, pOleDoc->server.m_sOldQuitMenuItem);

			pOleDoc->server.m_fSetFileMenu = false;
		}
	}
}

//#pragma segment OleServerDocSeg
void OleServerDocDoSave(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr		pOleServerDoc;
	HRESULT				hrErr;
	
	pOleServerDoc = &pOleDoc->server;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kEmbeddedDocumentType)
	{
		// To update the container, we must ask our container to save us
		ASSERTCOND(pOleServerDoc->m_lpOleClientSite != nil);
		hrErr = pOleServerDoc->m_lpOleClientSite->lpVtbl->SaveObject(pOleServerDoc->m_lpOleClientSite);
		ASSERTNOERROR(hrErr);
	}
}

//#pragma segment OleServerDocSeg
void OleServerDocDoClose(OleDocumentPtr pOleDoc, Boolean askUserToSave, YNCResult defaultAnswer, Boolean quitting)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;
	
	OleAppAddRef(gOleApp);
	OleDocAddRef(pOleDoc);
	
	/* OLE2NOTE: force all pseudo objects to close. this informs all
	**    linking clients of pseudo objects to release their PseudoObj.
	*/
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_CloseAllPseudoObjsProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_CloseAllPseudoObjsProcPtr)(pOleDoc->m_pIDoc);

    /* OLE2NOTE: send last OnDataChange notification to clients
    **    that have registered for data notifications when object
    **    stops running (ADVF_DATAONSTOP), if the data in our
    **    object has ever changed. it is best to only send this
    **    notification if necessary.
    */
	if (pOleServerDoc->m_fSendDataOnStop && pOleServerDoc->m_lpDataAdviseHolder)
	{
		OleServerDocSendAdvise(pOleDoc, OLE_ONDATACHANGE, 0, ADVF_DATAONSTOP);
		pOleServerDoc->m_fSendDataOnStop = false;
	}

	if (pOleServerDoc->m_lpDataAdviseHolder)
	{
		OleStdVerifyRelease((LPUNKNOWN)pOleServerDoc->m_lpDataAdviseHolder);
		pOleServerDoc->m_lpDataAdviseHolder = nil;
	}

#if qOleInPlace
	if (OleInPlaceServerDocIsInPlace(pOleDoc))
		OleInPlaceServerDocIPDeactivate(pOleDoc, END_IP_OnIPDeactivate);
#endif

	// inform all linking clients that we are closing
	if (pOleServerDoc->m_lpOleAdviseHolder)
		OleServerDocSendAdvise(pOleDoc, OLE_ONCLOSE, 0, 0);
	
	if (pOleServerDoc->m_lpOleAdviseHolder)
	{
		OleStdVerifyRelease((LPUNKNOWN)pOleServerDoc->m_lpOleAdviseHolder);
		pOleServerDoc->m_lpOleAdviseHolder = nil;
	}
	
	if (pOleServerDoc->m_lpOleClientSite)
	{
		OleStdRelease((LPUNKNOWN)pOleServerDoc->m_lpOleClientSite);
		pOleServerDoc->m_lpOleClientSite = nil;
	}

	OleDocRelease(pOleDoc);
	OleAppRelease(gOleApp);
}

//#pragma segment OleServerDocSeg
void OleServerDocDoIdle(OleDocumentPtr pOleDoc)
{
#if qOleInPlace
	OleInPlaceServerDocDoIdle(pOleDoc);
#endif

	// Check to see if any data has changed. If so advise
	if (pOleDoc->server.m_DataChanged || pOleDoc->server.m_SizeChanged)
		OleServerDocSendAdvise(pOleDoc, OLE_ONDATACHANGE, NULL, 0);

	pOleDoc->server.m_DataChanged = false;
	pOleDoc->server.m_SizeChanged = false;
}

//#pragma segment OleDocumentSeg
void OleServerDocSetDirty(OleDocumentPtr pOleDoc, Boolean fDirty)
{
	if (fDirty)
	{
		OleServerDocSetDataChanged(pOleDoc, fDirty);
		pOleDoc->server.m_fSendDataOnStop = true;
	}
}


//#pragma segment OleDocumentSeg
void OleServerDocSetDataChanged(OleDocumentPtr pOleDoc, Boolean fDataChanged)
{
	pOleDoc->server.m_DataChanged = fDataChanged;
}

//#pragma segment OleDocumentSeg
void OleServerDocSetSizeChanged(OleDocumentPtr pOleDoc, Boolean fSizeChanged)
{
	pOleDoc->server.m_SizeChanged = fSizeChanged;
}


//#pragma segment OleServerDocSeg
HRESULT OleServerDocInitNewEmbedded(OleDocumentPtr pOleDoc)
{
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
	ASSERTCOND((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kUnknownDocumentType);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDocTypeProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetDocTypeProcPtr)(pOleDoc->m_pIDoc, kEmbeddedDocumentType);
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_SetDirtyProcPtr)(pOleDoc->m_pIDoc, true);
	
	return NOERROR;
}

//#pragma segment OleServerDocSeg
HRESULT OleServerDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);
	if (!(*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
	{
		if (IsEqualIID(riid, &IID_IOleObject))
		{
			*lplpvObj = &pOleServerDoc->m_OleObject;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
		else if (IsEqualIID(riid, &IID_IPersistStorage))
		{
			*lplpvObj = &pOleServerDoc->m_PersistStorage;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
		else if (IsEqualIID(riid, &IID_IDataObject))
		{
			*lplpvObj = &pOleDoc->m_DataObject;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}

#if qOleTreatAs
		else if (IsEqualIID(riid, &IID_IStdMarshalInfo))
		{
			*lplpvObj = &pOleServerDoc->m_StdMarshalInfo;
			OleDocAddRef(pOleDoc);
			return NOERROR;
		}
#endif // qOleTreatAs	

#if qOleInPlace
		else
			return OleInPlaceServerDocLocalQueryInterface(pOleDoc, riid, lplpvObj);
#endif
	}

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleServerDocSeg
void OleServerDocSendAdvise(OleDocumentPtr pOleDoc, OLE_NOTIFICATION wAdvise, LPMONIKER lpmkDoc, unsigned long dwAdvf)
{
	OleServerDocPtr		pOleServerDoc;
	
	pOleServerDoc = &pOleDoc->server;
	
	switch(wAdvise)
	{
		case OLE_ONDATACHANGE:
			{
				if (pOleServerDoc->m_fEnableDraw)
				{	
					pOleServerDoc->m_DataChanged = false;
					
					if (pOleServerDoc->m_lpDataAdviseHolder)
					{
						pOleServerDoc->m_lpDataAdviseHolder->lpVtbl->SendOnDataChange(
												pOleServerDoc->m_lpDataAdviseHolder,
												(LPDATAOBJECT)&pOleDoc->m_DataObject,
												0,
												dwAdvf);
												
						 OleStdNoteObjectChangeTime(pOleDoc->m_dwRegROT);
					}
				}
			}
			break;
		
		case OLE_ONCLOSE:
			{
				if (pOleServerDoc->m_lpOleAdviseHolder)
					pOleServerDoc->m_lpOleAdviseHolder->lpVtbl->SendOnClose(pOleServerDoc->m_lpOleAdviseHolder);
			}
			break;
		
		case OLE_ONSAVE:
			{
				if (pOleServerDoc->m_lpOleAdviseHolder)
					pOleServerDoc->m_lpOleAdviseHolder->lpVtbl->SendOnSave(pOleServerDoc->m_lpOleAdviseHolder);
			
	            /* OLE2NOTE: inform any clients of pseudo objects
	            **    within our document, that our document has been saved.
	            */
				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_InformAllPseudoObjectsDocSavedProcPtr != nil);
				(*pOleDoc->m_pIDoc->lpVtbl->m_InformAllPseudoObjectsDocSavedProcPtr)(pOleDoc->m_pIDoc, lpmkDoc);

				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr != nil);
				if ((*pOleDoc->m_pIDoc->lpVtbl->m_GetDocTypeProcPtr)(pOleDoc->m_pIDoc) == kFromFileDocumentType)
					OleStdNoteFileChangeTime(pOleServerDoc->m_szObjectName, pOleDoc->m_dwRegROT);
			}
			break;
			
		case OLE_ONRENAME:
			{
				if (lpmkDoc && pOleServerDoc->m_lpOleAdviseHolder)
					pOleServerDoc->m_lpOleAdviseHolder->lpVtbl->SendOnRename(pOleServerDoc->m_lpOleAdviseHolder, lpmkDoc);

				/* OLE2NOTE: inform any clients of pseudo objects within our document,
				** that our document's Moniker has changed.
				*/
				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_InformAllPseudoObjectsDocRenamedProcPtr != nil);
				(*pOleDoc->m_pIDoc->lpVtbl->m_InformAllPseudoObjectsDocRenamedProcPtr)(pOleDoc->m_pIDoc, lpmkDoc);
			}
			break;
	}
}

//#pragma segment OleServerDocSeg
/* OleServerDocPseudoObjLockDoc
** ----------------------------
**    Add a lock on the Doc on behalf of the PseudoObject. the Doc may not
**    close while the Doc exists.
**
**    when a pseudo object is first created, it calls this method to
**    guarantee that the document stays alive (PseudoObjInit).
**    when a pseudo object is destroyed, it call
**    OleServerDocPseudoObjUnlockDoc to release this hold on the document.
*/
void OleServerDocPseudoObjLockDoc(OleDocumentPtr pOleDoc)
{
	OleServerDocPtr pServerDoc = &pOleDoc->server;
    unsigned long cPseudoObj;

    cPseudoObj = ++pServerDoc->m_cPseudoObj;

    OleDocLock(pOleDoc, true /* fLock */, 0 /* not applicable */);
}


//#pragma segment OleServerDocSeg
/* OleServerDocPseudoObjUnlockDoc
** ------------------------------
**    Release the lock on the Doc on behalf of the PseudoObject. if this was
**    the last lock on the Doc, then it will shutdown.
*/
void OleServerDocPseudoObjUnlockDoc(OleDocumentPtr pOleDoc)
{
    unsigned long cPseudoObj;
	OleServerDocPtr pServerDoc = &pOleDoc->server;

    /* OLE2NOTE: when there are no active pseudo objects in the Doc and
    **    the Doc is not visible, and if there are no outstanding locks
    **    on the Doc, then this is a "silent update"
    **    situation. our Doc is being used programatically by some
    **    client; it is NOT accessible to the user because it is
    **    NOT visible. thus since all Locks have been released, we
    **    will close the document. if the app is only running due
    **    to the presence of this document, then the app will now
    **    also shut down.
    */
    cPseudoObj = --pServerDoc->m_cPseudoObj;

    OleDocLock(pOleDoc, false /* fLock */, true /* fLastUnlockReleases */);
}


//#pragma segment OleServerDocSeg
HRESULT OleServerDocOleObjectDoVerb(OleDocumentPtr pOleDoc, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, long lindex, WindowPtr hwndParent, LPCRECT lprcPosRect)
{
	SCODE	sc;
	
	sc = S_OK;
	
	switch(lVerb)
	{
		default:
			if (lVerb < 0)
				return ResultFromScode(OLEOBJ_S_INVALIDVERB);

			// intentionally fall through...
		
		case 0:
		case OLEIVERB_SHOW:
#if qOleInPlace
			{
				HRESULT		hrErr;
				
				hrErr = OleInPlaceServerDocDoInPlaceEdit(pOleDoc, lVerb, lpmsg, lpActiveSite, hwndParent, lprcPosRect);
				if (hrErr == NOERROR)
					break;
			}
#endif
			// intentionally fall through...
			
		case 1:
		case OLEIVERB_OPEN:
			{
				ProcessSerialNumber		psn;
				WindowPtr				pWindow;
				Rect					rOldBounds;
				Boolean					fVisible;

#if qOleInPlace
				// if we're currently inplace, deactivate inplace
				// so that we can change to open editing
				if (OleInPlaceServerDocIsInPlace(pOleDoc))
					OleInPlaceServerDocIPDeactivate(pOleDoc, END_IP_OnIPDeactivate);
#endif

				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr != nil);
				fVisible = (*pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr)(pOleDoc->m_pIDoc);

				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
				pWindow = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

				if (!fVisible)
				{
					rOldBounds = pWindow->portRect;

					// make sure the window is back in its default position
					MoveWindow(pWindow, 50, 205, false);
					SizeWindow(pWindow, 400, 130, false);

					(*pOleDoc->m_pIDoc->lpVtbl->m_ResizeProcPtr)(
									pOleDoc->m_pIDoc, &rOldBounds,
									400 - kRowHeadingsWidth,
									130 - kColumnHeadingsHeight);
				}

				ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_ShowProcPtr != nil);
				(*pOleDoc->m_pIDoc->lpVtbl->m_ShowProcPtr)(pOleDoc->m_pIDoc);

				GetCurrentProcess(&psn);
				SetFrontProcess(&psn);

				SelectWindow(pWindow);
			}
			break;
			
		case OLEIVERB_HIDE:
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr)(pOleDoc->m_pIDoc);
			break;

#if qOleInPlace
		case OLEIVERB_UIACTIVATE:
            /* OLE2NOTE: if our window is already open (visible) then
            **    we can NOT activate in-place.
            */
            ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr != nil);
            if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr)(pOleDoc->m_pIDoc)
            		&& !pOleDoc->server.inplace.m_fIPActive)
            	sc = OLE_E_NOT_INPLACEACTIVE;
            else
			{
				HRESULT		hrErr;
				
				hrErr = OleInPlaceServerDocUIActivate(pOleDoc, BEGIN_IP_OnUIActivate);
				ASSERTNOERROR(hrErr);
			}
			break;
#endif
	}

	return ResultFromScode(sc);
}


//#pragma segment OleServerDocSeg
HRESULT OleServerDocDataObjectGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium)
{
	lpMedium->tymed = 0;
	lpMedium->pUnkForRelease = NULL;	// we transfer ownership to caller
	lpMedium->STGMEDIUM_HGLOBAL = NULL;
	
#ifdef __NEVER__	
	if (lpFormatetc->cfFormat == gOleApp->m_cfApplication)
	{
		if (!(lpFormatetc->tymed & TYMED_HGLOBAL))
			return ResultFromScode(DATA_E_FORMATETC);
		
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetDataProcPtr)(pOleDoc->m_pIDoc);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL)
			return ResultFromScode(E_OUTOFMEMORY);
		
		lpMedium->tymed = TYMED_HGLOBAL;
		
		return NOERROR;
	}
	else
#endif // NEVER
	
	if (lpFormatetc->cfFormat == 'PICT'
			&& (lpFormatetc->dwAspect & (DVASPECT_CONTENT | DVASPECT_DOCPRINT)))
	{
		if (!(lpFormatetc->tymed & TYMED_MFPICT))
			return ResultFromScode(DATA_E_FORMATETC);
			
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetMetafilePictDataProcPtr)(pOleDoc->m_pIDoc, nil);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL)
			return ResultFromScode(E_OUTOFMEMORY);
		
		lpMedium->tymed = TYMED_MFPICT;
		
		return NOERROR;
	}
	else if (lpFormatetc->cfFormat == 'PICT'
			&& (lpFormatetc->dwAspect & DVASPECT_ICON))
	{
		if (!(lpFormatetc->tymed & TYMED_MFPICT))
			return ResultFromScode(DATA_E_FORMATETC);
	}
	else if (lpFormatetc->cfFormat == 'TEXT')
	{
		if (!(lpFormatetc->tymed & TYMED_HGLOBAL))
			return ResultFromScode(DATA_E_FORMATETC);
			
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr != nil);
		lpMedium->STGMEDIUM_HGLOBAL = (*pOleDoc->m_pIDoc->lpVtbl->m_GetTextDataProcPtr)(pOleDoc->m_pIDoc, nil);
		ASSERTCOND(lpMedium->STGMEDIUM_HGLOBAL != nil);
		
		if (!lpMedium->STGMEDIUM_HGLOBAL)
			return ResultFromScode(E_OUTOFMEMORY);

		lpMedium->tymed = TYMED_HGLOBAL;

		return NOERROR;
	}
	
	return ResultFromScode(DATA_E_FORMATETC);
}

//#pragma segment OleServerDocSeg
HRESULT OleServerDocDataObjectQueryGetData(OleDocumentPtr pOleDoc, LPFORMATETC lpFormatetc)
{
	if ( // lpFormatetc->cfFormat == gOleApp->m_cfApplication ||
		lpFormatetc->cfFormat == 'TEXT')
		return OleStdQueryFormatMedium(lpFormatetc, TYMED_HGLOBAL);
	else if (lpFormatetc->cfFormat == 'PICT'
			&& (lpFormatetc->dwAspect & (DVASPECT_CONTENT | DVASPECT_CONTENT | DVASPECT_DOCPRINT)))
		return OleStdQueryFormatMedium(lpFormatetc, TYMED_MFPICT);
		
	return ResultFromScode(DATA_E_FORMATETC);
}

//#pragma segment OleServerDocSeg
HRESULT OleServerDocDataObjectAdvise(OleDocumentPtr pOleDoc, FORMATETC * lpFormatetc, unsigned long advf, LPADVISESINK lpAdvSink, unsigned long * lpdwConnection)
{
	OleServerDocPtr			pOleServerDoc;
	HRESULT					hrErr;
	
	pOleServerDoc = &pOleDoc->server;
	
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
		return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);

	if (pOleServerDoc->m_lpDataAdviseHolder == NULL)
	{
		hrErr = CreateDataAdviseHolder(&pOleServerDoc->m_lpDataAdviseHolder);
		ASSERTNOERROR(hrErr);
		if (hrErr != NOERROR)
			return hrErr;
	}
	
	hrErr = pOleServerDoc->m_lpDataAdviseHolder->lpVtbl->Advise(
							pOleServerDoc->m_lpDataAdviseHolder,
							(LPDATAOBJECT)&pOleDoc->m_DataObject,
							lpFormatetc,
							advf,
							lpAdvSink,
							lpdwConnection);
	ASSERTNOERROR(hrErr);

	return hrErr;
}

//#pragma segment OleServerDocSeg
HRESULT OleServerDocDataObjectUnadvise(OleDocumentPtr pOleDoc, unsigned long dwConnection)
{
	OleServerDocPtr			pOleServerDoc;
	HRESULT					hrErr;
	
	pOleServerDoc = &pOleDoc->server;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsDataTransferDocProcPtr)(pOleDoc->m_pIDoc))
		return ResultFromScode(OLE_E_ADVISENOTSUPPORTED);

	if (pOleServerDoc->m_lpDataAdviseHolder == NULL)
		return ResultFromScode(E_FAIL);

	hrErr = pOleServerDoc->m_lpDataAdviseHolder->lpVtbl->Unadvise(
						pOleServerDoc->m_lpDataAdviseHolder,
						dwConnection);
	ASSERTNOERROR(hrErr);
	
	return hrErr;
}

//#pragma segment OleServerDocSeg
void OleServerDocRenamedUpdate(OleDocumentPtr pOleDoc, LPMONIKER lpmkDoc)
{
	// OLE2NOTE: inform any linking clients that the document has been renamed.
	OleServerDocSendAdvise(pOleDoc, OLE_ONRENAME, lpmkDoc, 0);
}

/* OleServerDocPasteFormatFromData
** -------------------------------
**
**    Paste a particular data format from a IDataObject*. The
**    IDataObject* may come from the clipboard (GetClipboard) or from a
**    drag/drop operation.
**
**    NOTE: If fLink is specified then FALSE is returned because the
**    Server only version of the app can not support links.
**
**    Returns TRUE if data was successfully pasted.
**            FALSE if data could not be pasted.
*/
//#pragma segment OleServerDocSeg
void OleServerDocPasteFormatFromData(
        OleDocumentPtr          pOleDoc,
        ResType              	cfFormat,
        LPDATAOBJECT            pSrcDataObj,
        Boolean                 fLink
)
{
    OleApplicationPtr pOleApp = gOleApp;
    Handle       hData;
    STGMEDIUM    medium;

    if (fLink) {
        /* We should paste a Link to the data, but we do not support links */
        return;

    } else {

        if (cfFormat == 'TEXT') {

            hData = OleStdGetData(
                    pSrcDataObj,
                    'TEXT',
                    NULL,
					DVASPECT_CONTENT,
                    (LPSTGMEDIUM)&medium
            );
			if (hData == NULL)
				return;

			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PasteTextDataProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_PasteTextDataProcPtr)(pOleDoc->m_pIDoc, hData);
			
            // OLE2NOTE: we must free data handle by releasing the medium
            ReleaseStgMedium((LPSTGMEDIUM)&medium);
        }
    }
}

void OleServerDocDoCopyRange(OleDocumentPtr pOleDoc)
{
	OleDocumentPtr 		pSrcOleDoc = pOleDoc->m_pSrcDocOfCopy;
	OleServerDocPtr		pServerDoc = &pOleDoc->server;
	LPMONIKER 			lpmkDoc = NULL;
	LPMONIKER 			lpmkItem = NULL;
	void*				pCopiedRange = nil;

	/* If source document is able to provide a moniker, then the
	**    destination document (lpDestOutlineDoc) should offer
	**    CF_LINKSOURCE via its IDataObject interface that it gives
	**    to the clipboard or the drag/drop operation.
	**
	**    OLE2NOTE: we want to ask the source document if it can
	**    produce a moniker, but we do NOT want to FORCE moniker
	**    assignment at this point. we only want to FORCE moniker
	**    assignment later if a Paste Link occurs (ie. GetData for
	**    CF_LINKSOURCE). if the source document is able to give
	**    a moniker, then we store a pointer to the source document
	**    so we can ask it at a later time to get the moniker. we
	**    also save the range of the current selection so we can
	**    generate a proper item name later when Paste Link occurs.
	**    Also we need to give a string which identifies the source
	**    of the copy in the CF_OBJECTDESCRIPTOR format. this
	**    string is used to display in the PasteSpecial dialog. we
	**    get and store a TEMPFORUSER moniker which identifies the
	**    source of copy.
	*/
	lpmkDoc = OleDocGetFullMoniker(pSrcOleDoc, OLEGETMONIKER_TEMPFORUSER);
	if (lpmkDoc != NULL) {
	
		pOleDoc->m_fLinkSourceAvail = true;

        ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr != nil);
        pCopiedRange = (pOleDoc->m_pIDoc->lpVtbl->m_GetCopiedRangeProcPtr)(pOleDoc->m_pIDoc);

		/* store a "TEMPFORUSER" version of the moniker which
		**    identifies the source of the copy, so that we can use
		**    it to render CF_OBJECTDESCRIPTOR format later. this
		**    format is used to pass information that is necessary
		**    to display the PasteSpecial dialog. we need to
		**    display the string form of the moniker which
		**    identifies the source of the copy, but we do NOT want
		**    to force moniker assignment yet.
		*/
		ASSERTCOND(pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeRelMonikerProcPtr != nil);
		lpmkItem = (*pSrcOleDoc->m_pIDoc->lpVtbl->m_GetRangeRelMonikerProcPtr)(
					pSrcOleDoc->m_pIDoc,
					pCopiedRange,
					OLEGETMONIKER_TEMPFORUSER);
		if (lpmkItem) {
				CreateGenericComposite(
						lpmkDoc,
						lpmkItem,
						&pServerDoc->m_lpSrcMonikerOfCopy
				);
				OleStdRelease((LPUNKNOWN)lpmkItem);
		}
		OleStdRelease((LPUNKNOWN)lpmkDoc);
	}
}

// OLDNAME: OleOutlineServerDoc
//#pragma segment OleOutlineServerDocInitSeg
void OleOutlineServerDocInit(OleOutlineDocPtr pOleOutlineDoc)
{
#if qOleTreatAs
	pOleOutlineDoc->server.m_clsidTreatAs 			= CLSID_NULL;
	pOleOutlineDoc->server.m_pszTreatAsType 		= NULL;
#endif // qOleTreatAs
}

//#pragma segment OleOutlineServerDocSeg
HRESULT OleOutlineServerDocGetItemObject(OleOutlineDocPtr pOleOutlineDoc, char* pszItem, unsigned long SpeedNeeded, LPBINDCTX pbc, REFIID riid, void** ppvObject)
{
    PseudoObjPtr pPseudoObj;
    NameTablePtr pNameTable = ((OutlineDocPtr)pOleOutlineDoc)->m_NameTable;

    *ppvObject = NULL;

    /* Get the PseudoObj which corresponds to an item name. if the item
    **    name does NOT exist in the name table then NO object is
    **    returned. the ServerNameTable_GetPseudoObj routine finds a
    **    name entry corresponding to the item name, it then checks if
    **    a PseudoObj has already been allocated. if so, it returns the
    **    existing object, otherwise it allocates a new PseudoObj.
    */
    pPseudoObj = NameTableGetPseudoObj(
            pNameTable,
            pszItem,
            &pOleOutlineDoc->m_OleDoc
    );

    if (! pPseudoObj) {
        *ppvObject = NULL;
        return ResultFromScode(MK_E_NOOBJECT);
    }

    // return the desired interface pointer of the pseudo object.
    return PseudoObjQueryInterface(pPseudoObj, riid, ppvObject);
}

//#pragma segment OleOutlineServerDocSeg
HRESULT OleOutlineServerDocIsItemRunning(OleOutlineDocPtr pOleOutlineDoc, char* pszItem)
{
    NameTablePtr pNameTable = ((OutlineDocPtr)pOleOutlineDoc)->m_NameTable;
    NamePtr	pName;

    pName = NameTableFindName(pNameTable, pszItem);

    return pName ? NOERROR : ResultFromScode(MK_E_NOOBJECT);
}

//#pragma segment OleOutlineServerDocSeg
/* OleOutlineServerDocGetRangeRelMoniker
** -------------------------------------
**    Retrieve the relative item moniker which identifies the given
**    selection (lplrSel).
**
**    Returns NULL if a moniker can NOT be created.
*/
LPMONIKER OleOutlineServerDocGetRangeRelMoniker(
        OleOutlineDocPtr        pOleOutlineDoc,
        void*					pRange,
        unsigned long           dwAssign
)
{
    OutlineDocPtr 	pOutlineDoc = (OutlineDocPtr)pOleOutlineDoc;
    NameTablePtr 	pNameTable = pOutlineDoc->m_NameTable;
    NamePtr 		pName;
    LPMONIKER 		lpmk;
	LineListPtr		pLineList;
	LineRangePtr	plrCopied = (LineRangePtr)pRange;
	
	pLineList = ((OutlineDocPtr)pOleOutlineDoc)->m_LineList;
	
    pName = NameTableFindNamedRange(pNameTable, plrCopied);

    if (pName) {
        /* the selection range already has a name assigned */
        CreateItemMoniker(OLESTDDELIM, NameGetText(pName), &lpmk);
    } else {
        char 	szbuf[kMaxNameLen];
    	Handle 	hStr;

		hStr = Get1Resource('CSTR', kNewNamePrefix_CSTR);
        switch (dwAssign) {

            case OLEGETMONIKER_FORCEASSIGN:

                /* Force the assignment of the name. This is called when a
                **    Paste Link actually occurs. At this point we want to
                **    create a Name and add it to the NameTable in order to
                **    track the source of the link. This name (as all
                **    names) will be updated upon editing of the document.
                */
                sprintf(
                        szbuf,
                        "%s %ld",
                        (char*)*hStr,
                        ++pOutlineDoc->m_nNextRangeNo
                );

                pName = NameCreate(szbuf, plrCopied->m_nStartLine, plrCopied->m_nEndLine);
				ASSERTCOND(pName != nil);
				
				NameTableAddName(pNameTable, pName);
                break;

            case OLEGETMONIKER_TEMPFORUSER:

                /* Create a name to show to the user in the Paste
                **    Special dialog but do NOT yet incur the overhead
                **    of adding a Name to the NameTable. The Moniker
                **    generated should be useful to display to the user
                **    to indicate the source of the copy, but will NOT
                **    be used to create a link directly (the caller
                **    should ask again for a moniker specifying FORCEASSIGN).
                **    we will generate the name that would be the next
                **    auto-generated range name, BUT will NOT actually
                **    increment the range counter.
                */
                sprintf(
                        szbuf,
                        "%s %ld",
                        (char*)*hStr,
                        pOutlineDoc->m_nNextRangeNo+1
                );
                break;

            case OLEGETMONIKER_ONLYIFTHERE:

                /* the caller only wants a name if one has already been
                **    assigned. we have already above checked if the
                **    current selection has a name, so we will simply
                **    return NULL here.
                */
                return NULL;    // no moniker is assigned

            default:
                return NULL;    // unknown flag given
        }

        CreateItemMoniker(OLESTDDELIM, szbuf, &lpmk);
    }
    return lpmk;
}


//#pragma segment OleOutlineServerDocSeg
/* OleOutlineServerDocGetRangeFullMoniker
** --------------------------------------
**    Retrieve the full absolute moniker which identifies the given
**    selection (lplrSel).
**    this moniker is created as a composite of the absolute moniker for
**    the entire document appended with an item moniker which identifies
**    the selection relative to the document.
**    Returns NULL if a moniker can NOT be created.
*/
LPMONIKER OleOutlineServerDocGetRangeFullMoniker(
        OleOutlineDocPtr        pOleOutlineDoc,
        void*					pRange,
        unsigned long           dwAssign
)
{
    LPMONIKER lpmkDoc = NULL;
    LPMONIKER lpmkItem = NULL;
    LPMONIKER lpmkFull = NULL;

    lpmkDoc = OleDocGetFullMoniker(
            &pOleOutlineDoc->m_OleDoc,
            dwAssign
    );
    if (! lpmkDoc) return NULL;

    lpmkItem = OleOutlineServerDocGetRangeRelMoniker(
            pOleOutlineDoc,
            pRange,
            dwAssign
    );
    if (lpmkItem) {
        CreateGenericComposite(lpmkDoc, lpmkItem, (LPMONIKER *)&lpmkFull);
        OleStdRelease((LPUNKNOWN)lpmkItem);
    }

    if (lpmkDoc)
        OleStdRelease((LPUNKNOWN)lpmkDoc);

    return lpmkFull;
}

#if qOleInPlace

// OLDNAME: OleInPlaceServerInterface.c
static IOleInPlaceObjectVtbl			gOleInPlaceObjectVtbl;
static IOleInPlaceActiveObjectVtbl		gOleInPlaceActiveObjectVtbl;

//#pragma segment OleInPlaceServerInterfaceInitSeg
void OleInPlaceServerInitInterfaces(void)
{
	// OleInPlace::Object method table
	{	
		IOleInPlaceObjectVtbl*		p;
		
		p = &gOleInPlaceObjectVtbl;
		
		p->QueryInterface					= IOleInPlaceServerObjectQueryInterface;
		p->AddRef							= IOleInPlaceServerObjectAddRef;
		p->Release							= IOleInPlaceServerObjectRelease;
		p->GetWindow						= IOleInPlaceServerObjectGetWindow;
		p->ContextSensitiveHelp				= IOleInPlaceServerObjectContextSensitiveHelp;
		p->InPlaceDeactivate				= IOleInPlaceServerObjectInPlaceDeactivate;
		p->UIDeactivate						= IOleInPlaceServerObjectUIDeactivate;
		p->SetObjectRects					= IOleInPlaceServerObjectSetObjectRects;
		p->ReactivateAndUndo				= IOleInPlaceServerObjectReactivateAndUndo;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
	
	// OleInPlace::ActiveObject method table
	{
		IOleInPlaceActiveObjectVtbl*		p;
		
		p = &gOleInPlaceActiveObjectVtbl;
		
		p->QueryInterface					= IOleInPlaceServerActiveObjectQueryInterface;
		p->AddRef							= IOleInPlaceServerActiveObjectAddRef;
		p->Release							= IOleInPlaceServerActiveObjectRelease;
		p->GetWindow						= IOleInPlaceServerActiveObjectGetWindow;
		p->ContextSensitiveHelp				= IOleInPlaceServerActiveObjectContextSensitiveHelp;
		p->TranslateAccelerator				= IOleInPlaceServerActiveObjectTranslateAccelerator;
		p->OnFrameWindowActivate			= IOleInPlaceServerActiveObjectOnFrameWindowActivate;
		p->OnDocWindowActivate				= IOleInPlaceServerActiveObjectOnDocWindowActivate;
		p->ResizeBorder						= IOleInPlaceServerActiveObjectResizeBorder;
		p->EnableModeless					= IOleInPlaceServerActiveObjectEnableModeless;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OleInPlaceServerInterfaceSeg
void OleInPlaceServerIObjectInit(OleInPlaceServerObjectImplPtr pOleInPlaceServerObjectImpl, struct OleDocumentRec* pOleDoc)
{
	pOleInPlaceServerObjectImpl->lpVtbl		= &gOleInPlaceObjectVtbl;
	pOleInPlaceServerObjectImpl->lpOleDoc	= pOleDoc;
	pOleInPlaceServerObjectImpl->cRef		= 0;
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectQueryInterface(LPOLEINPLACEOBJECT lpThis, REFIID riid, void * * ppvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, ppvObj);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP_(unsigned long) IOleInPlaceServerObjectAddRef(LPOLEINPLACEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP_(unsigned long) IOleInPlaceServerObjectRelease(LPOLEINPLACEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectGetWindow(LPOLEINPLACEOBJECT lpThis, WindowPtr * lphwnd)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(lphwnd != nil);
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	*lphwnd = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

	return NOERROR;
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectContextSensitiveHelp(LPOLEINPLACEOBJECT lpThis, unsigned long fEnterMode)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectInPlaceDeactivate(LPOLEINPLACEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	if (pOleDoc->server.inplace.m_fUIVisible)
		pOleDoc->server.inplace.m_fInvalidate = false;

	return OleInPlaceServerDocIPDeactivate(pOleDoc, END_IP_OnIPDeactivate);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectUIDeactivate(LPOLEINPLACEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;
	
	if (pOleDoc->server.inplace.m_fUIVisible)
		pOleDoc->server.inplace.m_fInvalidate = false;

	return OleInPlaceServerDocUIDeactivate(pOleDoc, END_IP_OnUIDeactivate);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectSetObjectRects(LPOLEINPLACEOBJECT lpThis,
					LPCRECT lprcPosRect,
					RgnHandle clipRgn,
					RgnHandle frameRgn,
					RgnHandle cliRgn
					)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerObjectImplPtr)lpThis)->lpOleDoc;

	return OleInPlaceServerDocSetObjectRects(pOleDoc,
					lprcPosRect,
					clipRgn,
					frameRgn,
					cliRgn);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerObjectReactivateAndUndo(LPOLEINPLACEOBJECT lpThis)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}


//#pragma segment OleInPlaceServerInterfaceSeg
void OleInPlaceServerIActiveObjectInit(OleInPlaceServerActiveObjectImplPtr pOleInPlaceServerActiveObjectImpl, struct OleDocumentRec* pOleDoc)
{
	pOleInPlaceServerActiveObjectImpl->lpVtbl		= &gOleInPlaceActiveObjectVtbl;
	pOleInPlaceServerActiveObjectImpl->lpOleDoc		= pOleDoc;
	pOleInPlaceServerActiveObjectImpl->cRef			= 0;
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectQueryInterface(LPOLEINPLACEACTIVEOBJECT lpThis, REFIID riid, void * * ppvObj)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocQueryInterface(pOleDoc, riid, ppvObj);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP_(unsigned long) IOleInPlaceServerActiveObjectAddRef(LPOLEINPLACEACTIVEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocAddRef(pOleDoc);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP_(unsigned long) IOleInPlaceServerActiveObjectRelease(LPOLEINPLACEACTIVEOBJECT lpThis)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	return OleDocRelease(pOleDoc);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectGetWindow(LPOLEINPLACEACTIVEOBJECT lpThis, WindowPtr * lphwnd)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();
	
	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	ASSERTCOND(lphwnd != nil);
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	*lphwnd = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

	return NOERROR;
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectContextSensitiveHelp(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fEnterMode)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectTranslateAccelerator(LPOLEINPLACEACTIVEOBJECT lpThis, EventRecord * lpmsg)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectOnFrameWindowActivate(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fActivate)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	if (!fActivate)
		pOleDoc->server.inplace.m_fInvalidate = false;

	return OleInPlaceServerDocOnDocWindowActivate(pOleDoc, (Boolean)fActivate);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectOnDocWindowActivate(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fActivate)
{
	OleDocumentPtr		pOleDoc;

	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	if (!fActivate)
		pOleDoc->server.inplace.m_fInvalidate = false;

	return OleInPlaceServerDocOnDocWindowActivate(pOleDoc, (Boolean)fActivate);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectResizeBorder(LPOLEINPLACEACTIVEOBJECT lpThis, LPCRECT lprectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, unsigned long fFrameWindow)
{
	OleDbgEnterInterface();

Debugger();
	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceServerInterfaceSeg
STDMETHODIMP IOleInPlaceServerActiveObjectEnableModeless(LPOLEINPLACEACTIVEOBJECT lpThis, unsigned long fEnable)
{
	OleDocumentPtr		pOleDoc;
	
	OleDbgEnterInterface();

	pOleDoc = ((OleInPlaceServerActiveObjectImplPtr)lpThis)->lpOleDoc;

	return OleInPlaceServerDocEnableModeless(pOleDoc, (Boolean)fEnable);
}

// OLDNAME: OleInPlaceServerDocument.c
#define ESCAPEKEY		27

extern char					gApplicationName[];
extern ApplicationPtr		gApplication;
extern OleApplicationPtr	gOleApp;

//#pragma segment OleInPlaceServerDocInitSeg
void OleInPlaceServerDocInit(OleDocumentPtr pOleDoc)
{
	// by default a doc deactivate kills the inplace session.
	pOleDoc->server.inplace.m_fKillInPlace = true;

	// by default we force an invalide when calling OleUnSetInPlaceWindow
	pOleDoc->server.inplace.m_fInvalidate = true;

	OleInPlaceServerIObjectInit(&pOleDoc->server.inplace.m_Object, pOleDoc);
	OleInPlaceServerIActiveObjectInit(&pOleDoc->server.inplace.m_ActiveObject, pOleDoc);
}

//#pragma segment OleInPlaceServerDocInitSeg
void OleInPlaceServerDocDispose(OleDocumentPtr pOleDoc)
{
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocLocalQueryInterface(OleDocumentPtr pOleDoc, REFIID riid, void* * lplpvObj)
{
	OleInPlaceServerDocPtr		pOleInPlaceServerDoc;

	pOleInPlaceServerDoc = &pOleDoc->server.inplace;

	// windows code also return this interface for IsEqualIID(riid, &IID_IOleWindow). Is this right?
	// REVIEW: this should come out, testing only
	ASSERTCOND(IsEqualIID(riid, &IID_IOleWindow) == false);

	if (IsEqualIID(riid, &IID_IOleInPlaceObject))
	{
		*lplpvObj = &pOleInPlaceServerDoc->m_Object;
		OleDocAddRef(pOleDoc);
		return NOERROR;
	}
	else if (IsEqualIID(riid, &IID_IOleInPlaceActiveObject))
	{
		*lplpvObj = &pOleInPlaceServerDoc->m_ActiveObject;
		OleDocAddRef(pOleDoc);
		return NOERROR;
	}

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleInPlaceServerDocSeg
Boolean OleInPlaceServerDocIsInPlace(OleDocumentPtr pOleDoc)
{
	return pOleDoc->server.inplace.m_fIPActive;
}

//#pragma segment OleInPlaceServerDocSeg
Boolean OleInPlaceServerDocIsUIActive(OleDocumentPtr pOleDoc)
{
	return pOleDoc->server.inplace.m_fUIActive;
}

//#pragma segment OleInPlaceServerDocSeg
Boolean OleInPlaceServerDocIsUIVisible(OleDocumentPtr pOleDoc)
{
	return pOleDoc->server.inplace.m_fUIVisible;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocDoInPlaceEdit(OleDocumentPtr pOleDoc, long lVerb, EventRecord* lpmsg, LPOLECLIENTSITE lpActiveSite, WindowPtr hwndParent, LPCRECT lprcPosRect)
{
	HRESULT		hrErr;

    /* OLE2NOTE: lpActiveSite should be used only for InPlace PLAYing.
    **    This app does not do inplace PLAYing, so it never uses
    **    lpActiveSite.
    */

    /* InPlace activation can only be done if the ClientSite is non-NULL. */
	if (!pOleDoc->server.m_lpOleClientSite)
		return ResultFromScode(E_FAIL);

	// we definitely shouldn't be UIActive at this point
	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == false);

	if (!pOleDoc->server.inplace.m_fIPActive)
		return OleInPlaceServerDocIPActivate(pOleDoc, lprcPosRect);
	else if (!pOleDoc->server.inplace.m_fUIActive)
		return OleInPlaceServerDocUIActivate(pOleDoc, BEGIN_IP_OnUIActivate);
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocIPActivate(OleDocumentPtr pOleDoc, LPCRECT lprcPosRect)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fIPActive == false);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == false);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIVisible == false);

	if (pOleDoc->server.inplace.m_fIPActive)
		return NOERROR;

	// if the object is in open mode then we do not want to do inplace activation.
	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr != nil);
	if ((*pOleDoc->m_pIDoc->lpVtbl->m_IsVisibleProcPtr)(pOleDoc->m_pIDoc))
		return ResultFromScode(E_FAIL);

	// If SetExtent hasn't been called the extent we're tracking is still
	// {0, 0}. In that case we need to update it with our real extent.
	if (!pOleDoc->server.inplace.m_ptExtent.h && !pOleDoc->server.inplace.m_ptExtent.v)
	{
		SIZEL	sizel;

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_GetExtentProcPtr)(pOleDoc->m_pIDoc, nil, &sizel);

		// Store the current object extent so we can compare it later and
		// determine if the container is trying to go inplace while zoomed.
		OleInPlaceServerDocSetExtent(pOleDoc, &sizel);
	}

	// set this now in case we fail part way through so the
	// ipdeactivate code knows it needs to execute.
	pOleDoc->server.inplace.m_fIPActive = true;

	TRY
	{
		// get the client site
		hrErr = (*pOleDoc->server.m_lpOleClientSite->lpVtbl->QueryInterface)(
						pOleDoc->server.m_lpOleClientSite,
						&IID_IOleInPlaceSite,
						(void*)&pOleDoc->server.inplace.m_pSite);
		FailOleErr(hrErr);

		// make sure it can do inplace
		hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->CanInPlaceActivate)(pOleDoc->server.inplace.m_pSite);
		if (hrErr != S_OK)
			Failure(E_ABORT, kOleErrorMessage);

		ASSERTCOND(pOleDoc->server.inplace.m_pFrame == nil);
		ASSERTCOND(pOleDoc->server.inplace.m_pUIWindow == nil);

		/* OLE2NOTE: The server should fill in the "recordLength" field so
		**    that the container can tell what size structure the server is
		**    expecting. this enables this structure to be easily extended
		**    in future releases of OLE. the container should check this
		**    field so that it doesn't try to use fields that do not exist
		**    since the server may be using an old structure definition.
		*/
		bzero(&pOleDoc->server.inplace.m_FrameInfo, sizeof(OLEINPLACEFRAMEINFO));

		ASSERTCOND(pOleDoc->server.inplace.m_hPosRgn == nil);
		ASSERTCOND(pOleDoc->server.inplace.m_hClipRgn == nil);
		ASSERTCOND(pOleDoc->server.inplace.m_hFrameRgn == nil);
		ASSERTCOND(pOleDoc->server.inplace.m_hCliRgn == nil);

		pOleDoc->server.inplace.m_hPosRgn = NewRgn();
		ASSERTCOND(pOleDoc->server.inplace.m_hPosRgn != nil);

		pOleDoc->server.inplace.m_hClipRgn = NewRgn();
		ASSERTCOND(pOleDoc->server.inplace.m_hClipRgn != nil);

		pOleDoc->server.inplace.m_hFrameRgn = NewRgn();
		ASSERTCOND(pOleDoc->server.inplace.m_hFrameRgn != nil);

		pOleDoc->server.inplace.m_hCliRgn = NewRgn();
		ASSERTCOND(pOleDoc->server.inplace.m_hCliRgn != nil);

		{
			Rect	posRect;

			hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->GetWindowContext)(
							pOleDoc->server.inplace.m_pSite,
							&pOleDoc->server.inplace.m_pFrame,
							&pOleDoc->server.inplace.m_pUIWindow,
							&posRect,
							pOleDoc->server.inplace.m_hClipRgn,
							pOleDoc->server.inplace.m_hFrameRgn,
							pOleDoc->server.inplace.m_hCliRgn,
							&pOleDoc->server.inplace.m_FrameInfo);
			ASSERTNOERROR(hrErr);

		FailOleErr(hrErr);

			ASSERTCOND(pOleDoc->server.inplace.m_pFrame != nil);
			ASSERTCOND(pOleDoc->server.inplace.m_pUIWindow != nil);
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.recordLength == sizeof(OLEINPLACEFRAMEINFO));
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.version == 1);
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.frameWindow != nil);
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.signature != 0L);
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.hCmdKeys == nil);
			ASSERTCOND(pOleDoc->server.inplace.m_FrameInfo.numCmds == 0);

#if qAssert
			{
				ProcessInfoRec	pi;
				OSErr			osErr;

				pi.processInfoLength = sizeof(pi);
				pi.processName       = NULL;
				pi.processAppSpec    = NULL;

				osErr = GetProcessInformation(&pOleDoc->server.inplace.m_FrameInfo.psn, &pi);
				if (osErr != noErr)
					ASSERTCONDSZ(0, "FrameInfo.psn is invalid!");
			}
#endif

			if (pOleDoc->server.inplace.m_FrameInfo.frameWindow)
				pOleDoc->server.inplace.m_pWindowParent = pOleDoc->server.inplace.m_FrameInfo.frameWindow;
			else
			{
				hrErr = (*pOleDoc->server.inplace.m_pUIWindow->lpVtbl->GetWindow)(
								pOleDoc->server.inplace.m_pUIWindow,
								&pOleDoc->server.inplace.m_pWindowParent);
				ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

				ASSERTCOND(pOleDoc->server.inplace.m_pWindowParent != nil);
			}

#ifdef DISABLED
			// If the size we receive from GetWindowContext differs from the extent that
			// we remember that means the container document is zoomed. We don't support
			// zooming so we need to fail here and start in open editing mode.
			if (pOleDoc->server.inplace.m_ptExtent.h != (posRect.right - posRect.left) ||
				pOleDoc->server.inplace.m_ptExtent.v != (posRect.bottom - posRect.top))
			{

		FailOleErr(hrErr);
			}
#endif

			posRect.left   -= kRowHeadingsWidth;
			posRect.top    -= kColumnHeadingsHeight;
			posRect.right  += 15;
			posRect.bottom += 15;

			RectRgn(pOleDoc->server.inplace.m_hPosRgn, &posRect);
		}

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
		pOleDoc->server.inplace.m_pWindow = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);
		ASSERTCOND(pOleDoc->server.inplace.m_pWindow != nil);

		// we shouldn't have a menubar at this point
		ASSERTCOND(pOleDoc->server.inplace.m_hOleMBar == nil);

		// only create the OleMBar when first going inplace
		if (!pOleDoc->server.inplace.m_hOleMBar)
		{
			hrErr = OleNewMBar(&pOleDoc->server.inplace.m_hOleMBar);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);

			ASSERTCOND(pOleDoc->server.inplace.m_hOleMBar != nil);

			hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->InsertMenus)(
							pOleDoc->server.inplace.m_pFrame,
							pOleDoc->server.inplace.m_hOleMBar);
			ASSERTNOERROR(hrErr);

		FailOleErr(hrErr);
		}

		hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->OnInPlaceActivate)(pOleDoc->server.inplace.m_pSite);
		ASSERTNOERROR(hrErr);

		FailOleErr(hrErr);

		// continue by going uiactive
		hrErr = OleInPlaceServerDocUIActivate(pOleDoc, BEGIN_IP_OnIPActivate);
	}
	CATCH
	{
		hrErr = OleInPlaceServerDocIPDeactivate(pOleDoc, END_IP_Failure);
		ASSERTNOERROR(hrErr);

		hrErr = ResultFromScode(E_UNEXPECTED);

		NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}

HRESULT OleInPlaceServerDocIPDeactivate(OleDocumentPtr pOleDoc, short wWhy)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fIPActive == true);

	if (!pOleDoc->server.inplace.m_fIPActive)
		return NOERROR;

	if (pOleDoc->server.inplace.m_fUIActive)
	{
		hrErr = OleInPlaceServerDocUIDeactivate(pOleDoc, wWhy);
		ASSERTNOERROR(hrErr);
	}

	// set this now in case of reentrancy
	pOleDoc->server.inplace.m_fIPActive = false;

	if (pOleDoc->server.inplace.m_hOleMBar)
	{
		if (pOleDoc->server.inplace.m_pFrame)
		{
			hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->RemoveMenus)(
							pOleDoc->server.inplace.m_pFrame,
							pOleDoc->server.inplace.m_hOleMBar);
			ASSERTNOERROR(hrErr);
		}

		hrErr = OleDisposeMBar(pOleDoc->server.inplace.m_hOleMBar);
		ASSERTNOERROR(hrErr);
		pOleDoc->server.inplace.m_hOleMBar = nil;
	}

	if (pOleDoc->server.inplace.m_pFrame)
	{
		OleStdRelease((LPUNKNOWN)pOleDoc->server.inplace.m_pFrame);
		pOleDoc->server.inplace.m_pFrame = NULL;
	}

	if (pOleDoc->server.inplace.m_pUIWindow)
	{
		OleStdRelease((LPUNKNOWN)pOleDoc->server.inplace.m_pUIWindow);
		pOleDoc->server.inplace.m_pUIWindow = NULL;
	}

	if (pOleDoc->server.inplace.m_hPosRgn)
	{
		DisposeRgn(pOleDoc->server.inplace.m_hPosRgn);
		pOleDoc->server.inplace.m_hPosRgn = nil;
	}

	if (pOleDoc->server.inplace.m_hClipRgn)
	{
		DisposeRgn(pOleDoc->server.inplace.m_hClipRgn);
		pOleDoc->server.inplace.m_hClipRgn = nil;
	}

	if (pOleDoc->server.inplace.m_hFrameRgn)
	{
		DisposeRgn(pOleDoc->server.inplace.m_hFrameRgn);
		pOleDoc->server.inplace.m_hFrameRgn = nil;
	}

	if (pOleDoc->server.inplace.m_hCliRgn)
	{
		DisposeRgn(pOleDoc->server.inplace.m_hCliRgn);
		pOleDoc->server.inplace.m_hCliRgn = nil;
	}

	if (pOleDoc->server.inplace.m_pSite)
	{
		if (wWhy != END_IP_Failure)
		{
			hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->OnInPlaceDeactivate)(pOleDoc->server.inplace.m_pSite);
			ASSERTNOERROR(hrErr);
		}

		OleStdRelease((LPUNKNOWN)pOleDoc->server.inplace.m_pSite);
		pOleDoc->server.inplace.m_pSite = nil;
	}

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr != nil);
	(*pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr)(pOleDoc->m_pIDoc);

	return NOERROR;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocUIActivate(OleDocumentPtr pOleDoc, short wWhy)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fIPActive == true);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == false);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIVisible == false);

	if (pOleDoc->server.inplace.m_fUIActive)
		return NOERROR;

	// set this now in case we fail part way through so the UIDeactivate
	// code knows it needs to execute.
	pOleDoc->server.inplace.m_fUIActive = true;

	TRY
	{
		// container might be activating us in a different window so we need
		// to update the parent window pointer.
		if (wWhy == BEGIN_IP_OnUIActivate)
		{
			hrErr = (*pOleDoc->server.inplace.m_pUIWindow->lpVtbl->GetWindow)(
							pOleDoc->server.inplace.m_pUIWindow,
							&pOleDoc->server.inplace.m_pWindowParent);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);

			ASSERTCOND(pOleDoc->server.inplace.m_pWindowParent != nil);
		}

		hrErr = OleInPlaceServerDocSetActiveObject(pOleDoc);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		hrErr = OleInPlaceServerDocNegotiateFrameSpace(pOleDoc);
		// ASSERTNOERROR(hrErr);
		// FailOleErr(hrErr);

#if qFrameTools
		// if frame tools are enabled our window might have moved when we
		// negotiated for frame space so passing true for the second parameter
		// forces this routine to update our position.
		hrErr = OleInPlaceServerDocPositionWindow(pOleDoc, true);
#else
		// we pass true for the second parameter on a uiactivate in case our
		// position has changed. that forces this routine to update our position.
		hrErr = OleInPlaceServerDocPositionWindow(pOleDoc, (Boolean)(wWhy == BEGIN_IP_OnUIActivate));
#endif
		FailOleErr(hrErr);

		hrErr = OleInPlaceServerDocSetInPlaceWindow(pOleDoc);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		hrErr = OleSetInPlaceRects(
				pOleDoc->server.inplace.m_hPosRgn,
				pOleDoc->server.inplace.m_hClipRgn,
				pOleDoc->server.inplace.m_hFrameRgn,
				pOleDoc->server.inplace.m_hCliRgn,
				NULL);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->OnUIActivate)(pOleDoc->server.inplace.m_pSite);
		ASSERTCOND(hrErr == NOERROR || hrErr == ResultFromScode(OLE_E_NOT_FRONT_PROCESS));
		if (hrErr != NOERROR && hrErr != ResultFromScode(OLE_E_NOT_FRONT_PROCESS))
		{
			// set the uivisible flag to force the OleInPlaceServerDocHide
			// routine to get called. this will unset the inplace window, etc.
			pOleDoc->server.inplace.m_fUIVisible = true;

			// time to get outta here!
			FailOleErr(hrErr);
		}

		if (hrErr == NOERROR)
		{
			// if no error was returned by OnUIActivate we can continue by
			// going uivisible.
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_ShowProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_ShowProcPtr)(pOleDoc->m_pIDoc);
		}
		else
		{
			// we only get here if the container returned OLE_E_NOT_FRONT_PROCESS.
			// in this case we leave outselves uiactive so that a single click in
			// the container window causes us to go uivisible.
			OleUnSetInPlaceWindow(false);

			// pretend like we succeeded so we don't cause an open edit
			hrErr = NOERROR;
		}
	}
	CATCH
	{
		hrErr = OleInPlaceServerDocUIDeactivate(pOleDoc, END_IP_Failure);
		ASSERTNOERROR(hrErr);

		hrErr = GetOleFailure();

		NO_PROPAGATE;
	}
	ENDTRY

	return hrErr;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocUIDeactivate(OleDocumentPtr pOleDoc, short wWhy)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == true);

	if (!pOleDoc->server.inplace.m_fUIActive)
		return NOERROR;

	if (pOleDoc->server.inplace.m_fUIVisible)
		OleInPlaceServerDocHide(pOleDoc, wWhy);

	// set this now in case of reentrancy
	pOleDoc->server.inplace.m_fUIActive = false;

	if (pOleDoc->server.inplace.m_pFrame)
	{
#ifdef __OBSOLETE__
		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->SetBorderSpace)(
						pOleDoc->server.inplace.m_pFrame,
						NULL);
#endif

		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->SetActiveObject)(
						pOleDoc->server.inplace.m_pFrame, NULL, NULL);
		ASSERTNOERROR(hrErr);
	}

	if (pOleDoc->server.inplace.m_pUIWindow)
	{
		hrErr = (*pOleDoc->server.inplace.m_pUIWindow->lpVtbl->SetActiveObject)(
						pOleDoc->server.inplace.m_pUIWindow, NULL, NULL);
		ASSERTNOERROR(hrErr);
	}

	if (wWhy != END_IP_Failure)
	{
		hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->OnUIDeactivate)(
						pOleDoc->server.inplace.m_pSite, false);
		ASSERTNOERROR(hrErr);
	}

	if (wWhy == END_IP_OnUIDeactivate)
	{
		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr)(pOleDoc->m_pIDoc);
	}

	return hrErr;
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocShow(OleDocumentPtr pOleDoc, short wWhy)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fIPActive == true);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == true);
	ASSERTCOND(pOleDoc->server.inplace.m_fUIVisible == false);

	if (pOleDoc->server.inplace.m_fUIVisible)
		return;

	// set this now in case we fail part way through so the
	// hide code knows it needs to execute.
	pOleDoc->server.inplace.m_fUIVisible = true;

	TRY
	{
		Rect		posRect;
		WindowPtr	pWindow;

		if (wWhy == BEGIN_IP_OnDocActivate)
		{
			hrErr = OleInPlaceServerDocPositionWindow(pOleDoc, true);
			FailOleErr(hrErr);

			hrErr = OleInPlaceServerDocSetInPlaceWindow(pOleDoc);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);

			hrErr = OleSetInPlaceRects(
					pOleDoc->server.inplace.m_hPosRgn,
					pOleDoc->server.inplace.m_hClipRgn,
					pOleDoc->server.inplace.m_hFrameRgn,
					pOleDoc->server.inplace.m_hCliRgn,
					NULL);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);

			hrErr = OleInPlaceServerDocSetActiveObject(pOleDoc);
			ASSERTNOERROR(hrErr);
			FailOleErr(hrErr);
		}

		ASSERTCOND(gApplication->vtbl->m_GetFrontDocWindowProcPtr != nil);
		pWindow = (*gApplication->vtbl->m_GetFrontDocWindowProcPtr)(gApplication);

		hrErr = OleClipWindows(pWindow);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);

		ASSERTCOND(pOleDoc->server.inplace.m_hSavedMBar == nil);
		pOleDoc->server.inplace.m_hSavedMBar = GetMenuBar();
		ASSERTCOND(pOleDoc->server.inplace.m_hSavedMBar != nil);

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_PreInPlaceInsertMenusProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_PreInPlaceInsertMenusProcPtr)(pOleDoc->m_pIDoc);

		{
			short	beforeID1;
			short	beforeID3;
			short	beforeID5;

			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetInPlaceBeforeMenuIDsProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_GetInPlaceBeforeMenuIDsProcPtr)(pOleDoc->m_pIDoc, &beforeID1, &beforeID3, &beforeID5);

			OleInsertMenus(pOleDoc->server.inplace.m_hOleMBar, beforeID1, beforeID3, beforeID5);
		}

		DrawMenuBar();

		if (wWhy == BEGIN_IP_OnDocActivate)
		{
			hrErr = pOleDoc->server.inplace.m_pSite->lpVtbl->OnUIVisible(pOleDoc->server.inplace.m_pSite, true);
			FailOleErr(hrErr);
		}

		ShowWindow(pOleDoc->server.inplace.m_pWindow);
		SelectWindow(pOleDoc->server.inplace.m_pWindow);

		hrErr = OleSetInFrontOf(&pOleDoc->server.inplace.m_FrameInfo.psn);

		if (hrErr == NOERROR)
		{
			ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_WaitContextSwitchProcPtr != nil);
			(*gOleApp->m_pIApp->lpVtbl->m_WaitContextSwitchProcPtr)(gOleApp->m_pIApp, true, false);

			// make sure the container window is hilited
			if (pOleDoc->server.inplace.m_pWindowParent)
				HiliteWindow(pOleDoc->server.inplace.m_pWindowParent, true);
		}
		else
			OleInPlaceServerDocHide(pOleDoc, END_IP_OnDocDeactivate);
	}
	CATCH
	{
		PostCmd(cmdOpenVerb);

		NO_PROPAGATE;
	}
	ENDTRY
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocHide(OleDocumentPtr pOleDoc, short wWhy)
{
	HRESULT		hrErr;
#if qFrameTools
	WindowPtr	pSavedLayer;
#endif

	if (!pOleDoc->server.inplace.m_fUIVisible)
		return;

	// set this now in case of reentrancy
	pOleDoc->server.inplace.m_fUIVisible = false;

	// move the window off screen now to avoid screen refresh problems.
	MoveWindow(pOleDoc->server.inplace.m_pWindow, 10000, 10000, false);

#if qFrameTools
	pSavedLayer = SwapLayer(gApplication->m_pDocLayer);
#endif

	hrErr = OleUnSetInPlaceWindow(pOleDoc->server.inplace.m_fInvalidate);
	ASSERTNOERROR(hrErr);

#if qFrameTools
	SetLayer(pSavedLayer);
#endif

	// set this back to the default
	pOleDoc->server.inplace.m_fInvalidate = true;

	if (pOleDoc->server.inplace.m_hSavedMBar)
	{
		SetMenuBar(pOleDoc->server.inplace.m_hSavedMBar);
		DisposeHandle(pOleDoc->server.inplace.m_hSavedMBar);
		pOleDoc->server.inplace.m_hSavedMBar = nil;
	}

	DrawMenuBar();

	// if the data or size has changed make sure the container has
	// a current presentation.
	if (pOleDoc->server.m_DataChanged || pOleDoc->server.m_SizeChanged)
	{
		OleServerDocSendAdvise(pOleDoc, OLE_ONDATACHANGE, NULL, 0);

		pOleDoc->server.m_DataChanged = false;
		pOleDoc->server.m_SizeChanged = false;
	}

	if (wWhy == END_IP_OnDocDeactivate)
	{
		hrErr = pOleDoc->server.inplace.m_pSite->lpVtbl->OnUIVisible(pOleDoc->server.inplace.m_pSite, false);
		ASSERTNOERROR(hrErr);

		ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr != nil);
		(*pOleDoc->m_pIDoc->lpVtbl->m_HideProcPtr)(pOleDoc->m_pIDoc);
	}
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocPositionWindow(OleDocumentPtr pOleDoc, Boolean fGetPosition)
{
	HRESULT		hrErr;
	Rect		rOldPos,
				rNewPos;

	ASSERTCOND(pOleDoc->server.inplace.m_hClipRgn &&
			   pOleDoc->server.inplace.m_hFrameRgn &&
			   pOleDoc->server.inplace.m_hCliRgn &&
			   pOleDoc->server.inplace.m_hPosRgn);

	if (fGetPosition)
	{
		hrErr = (*pOleDoc->server.inplace.m_pSite->lpVtbl->GetObjectRects)(
							pOleDoc->server.inplace.m_pSite,
							&rNewPos,
							pOleDoc->server.inplace.m_hClipRgn,
							pOleDoc->server.inplace.m_hFrameRgn,
							pOleDoc->server.inplace.m_hCliRgn);
		if (hrErr != NOERROR)
			return hrErr;

#ifdef DISABLED
		// If the size we receive from GetObjectRects differs from the extent that
		// we remember that means the container document is zoomed. We don't support
		// zooming so we need to fail here and start in open editing mode.
		if (pOleDoc->server.inplace.m_ptExtent.h != (rNewPos.right - rNewPos.left) ||
			pOleDoc->server.inplace.m_ptExtent.v != (rNewPos.bottom - rNewPos.top))
		{
			return ResultFromScode(E_FAIL);
		}
#endif

		rNewPos.left   -= kRowHeadingsWidth;
		rNewPos.top    -= kColumnHeadingsHeight;
		rNewPos.right  += 15;
		rNewPos.bottom += 15;

		RectRgn(pOleDoc->server.inplace.m_hPosRgn, &rNewPos);
	}
	else
		rNewPos = (**pOleDoc->server.inplace.m_hPosRgn).rgnBBox;

	rOldPos = pOleDoc->server.inplace.m_pWindow->portRect;

	MoveWindow(pOleDoc->server.inplace.m_pWindow, rNewPos.left, rNewPos.top, false);
	SizeWindow(pOleDoc->server.inplace.m_pWindow, (short)(rNewPos.right - rNewPos.left), (short)(rNewPos.bottom - rNewPos.top), false);

	(*pOleDoc->m_pIDoc->lpVtbl->m_ResizeProcPtr)(
					pOleDoc->m_pIDoc, &rOldPos,
					(short)(rNewPos.right - rNewPos.left - kRowHeadingsWidth),
					(short)(rNewPos.bottom - rNewPos.top - kColumnHeadingsHeight));

	return NOERROR;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocSetInPlaceWindow(OleDocumentPtr pOleDoc)
{
	HRESULT			hrErr;
	EventRecord		theEvent;
	unsigned long	ulTimeout = TickCount() + 60;

	while (true)
	{
		hrErr = OleSetInPlaceWindow(pOleDoc->server.inplace.m_pWindow, &pOleDoc->server.inplace.m_FrameInfo);
		if (hrErr != ResultFromScode(OLE_E_IPBUSY))
			break;

		if (TickCount() > ulTimeout)
		{
			ASSERTCONDSZ(0, "Timed out waiting for dll to unbusy!");
			break;
		}

		// yield time to other applications
		WaitNextEvent(0, &theEvent, 3, NULL);
	}

	return hrErr;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocSetActiveObject(OleDocumentPtr pOleDoc)
{
	HRESULT		hrErr;

	if (pOleDoc->server.inplace.m_pFrame)
	{
		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->SetActiveObject)(
						pOleDoc->server.inplace.m_pFrame,
						(LPOLEINPLACEACTIVEOBJECT)&pOleDoc->server.inplace.m_ActiveObject,
						gApplicationName);
		if (hrErr != NOERROR)
			return hrErr;
	}

	if (pOleDoc->server.inplace.m_pUIWindow)
	{
		hrErr = (*pOleDoc->server.inplace.m_pUIWindow->lpVtbl->SetActiveObject)(
						pOleDoc->server.inplace.m_pUIWindow,
						(LPOLEINPLACEACTIVEOBJECT)&pOleDoc->server.inplace.m_ActiveObject,
						gApplicationName);
		if (hrErr != NOERROR)
			return hrErr;
	}
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocNegotiateFrameSpace(OleDocumentPtr pOleDoc)
{
	HRESULT		hrErr;

	if (pOleDoc->server.inplace.m_pFrame)
	{
#if qFrameTools
		Rect	rBorder;

		ASSERTCOND(gApplication->vtbl->m_FrameSpaceNeededProcPtr != nil);
		(*gApplication->vtbl->m_FrameSpaceNeededProcPtr)(gApplication, &rBorder);

		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->SetBorderSpace)(
						pOleDoc->server.inplace.m_pFrame,
						&rBorder);

		if (hrErr == NOERROR)
		{
			ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_EnableFrameToolsProcPtr != nil);
			(*pOleDoc->m_pIDoc->lpVtbl->m_EnableFrameToolsProcPtr)(pOleDoc->m_pIDoc, true, false);
		}
#else
		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->SetBorderSpace)(
						pOleDoc->server.inplace.m_pFrame,
						NULL);
		if (hrErr != NOERROR)
			return hrErr;
#endif
	}

	if (pOleDoc->server.inplace.m_pUIWindow)
	{
		// we won't ask for any space on the document window
		hrErr = (*pOleDoc->server.inplace.m_pUIWindow->lpVtbl->SetBorderSpace)(
						pOleDoc->server.inplace.m_pUIWindow,
						NULL);
		if (hrErr != NOERROR)
			return hrErr;
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocDoActivate(OleDocumentPtr pOleDoc, Boolean becomingActive)
{
	HRESULT		hrErr;

	ASSERTCOND(OleInPlaceServerDocIsInPlace(pOleDoc) == true);

	if (!becomingActive && pOleDoc->server.inplace.m_fKillInPlace)
	{
		hrErr = OleInPlaceServerDocOnDocWindowActivate(pOleDoc, false);
		ASSERTNOERROR(hrErr);
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocDoKeyDown(OleDocumentPtr pOleDoc, EventRecord* pEvent)
{
	if ((pEvent->message & charCodeMask) == ESCAPEKEY)
	{
		if (pOleDoc->server.inplace.m_fUIVisible)
			SetFrontProcess(&pOleDoc->server.inplace.m_FrameInfo.psn);
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocUpdateMenus(OleDocumentPtr pOleDoc)
{
	static Boolean	fInAdjustMenus = false;
	HRESULT			hrErr;

	if (!pOleDoc->server.inplace.m_fUIVisible)
		return;

	if (pOleDoc->server.inplace.m_FrameInfo.fAdjustMenus && !fInAdjustMenus)
	{
		// We guard this call so that we don't attempt to call the container's
		// AdjustMenus method while inside another AdjustMenus call.
		fInAdjustMenus = true;

		ASSERTCOND(pOleDoc->server.inplace.m_pFrame != nil);
		hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->AdjustMenus)(
									pOleDoc->server.inplace.m_pFrame,
									pOleDoc->server.inplace.m_hOleMBar);
		ASSERTNOERROR(hrErr);

		fInAdjustMenus = false;
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocDoMenuKey(OleDocumentPtr pOleDoc, EventRecord* pEvent)
{
	HRESULT		hrErr;

	if (!pOleDoc->server.inplace.m_fUIActive)
		return;

	ASSERTCOND(pOleDoc->server.inplace.m_pFrame != nil);

	// passing 0 for the last argument indicates that MenuKey
	// failed to find a menu item associated with this key.
	hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->TranslateAccelerator)(
								pOleDoc->server.inplace.m_pFrame,
								pEvent, 0);
	ASSERTNOERROR(hrErr);
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocDoMenu(OleDocumentPtr pOleDoc, EventRecord* pEvent, short menuID, short menuItem)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleDoc->server.inplace.m_fUIActive == true);
	ASSERTCOND(pOleDoc->server.inplace.m_pFrame != nil);

	hrErr = (*pOleDoc->server.inplace.m_pFrame->lpVtbl->TranslateAccelerator)(
								pOleDoc->server.inplace.m_pFrame,
								pEvent,
								(long)((((long)menuID) << 16) | menuItem));
	ASSERTNOERROR(hrErr);
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocHideOthersCmd(OleDocumentPtr pOleDoc)
{
	// if the user choose Hide Others while we have a uivisible session
	// we cause the container to come to the foreground which uideactivates
	// the inplace session. if we don't do anything the inplace server
	// window is left floating without a container window behind it.
	if (pOleDoc->server.inplace.m_fUIVisible)
		SetFrontProcess(&pOleDoc->server.inplace.m_FrameInfo.psn);
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocEnableModeless(OleDocumentPtr pOleDoc, Boolean fEnable)
{
	// Note: if fEnable is false the dialog is coming up

	OleMaskMouse(fEnable);

	// when the container is about to show a dialog the server gets
	// suspended but we don't want to kill the inplace session
	// pOleDoc->server.inplace.m_fKillInPlace = fEnable;

	if (fEnable)
	{
		// dehilite the menu before coming foreward
		// HiliteMenu(0);
	}
	else
	{
		// ignore the next suspend event
		gOleApp->server.inplace.m_fIgnoreSuspendEvent = true;
	}

	return NOERROR;
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocOnDocWindowActivate(OleDocumentPtr pOleDoc, Boolean fActivate)
{
	HRESULT		hrErr;

	if (pOleDoc->server.inplace.m_fUIVisible == (fActivate ? true : false))
		return NOERROR;

	if (fActivate)
		OleInPlaceServerDocShow(pOleDoc, BEGIN_IP_OnDocActivate);
	else
		OleInPlaceServerDocHide(pOleDoc, END_IP_OnDocDeactivate);

	return NOERROR;
}

//#pragma segment OleInPlaceServerDocSeg
Boolean OleInPlaceServerDocProcessEvent(OleDocumentPtr pOleDoc, EventRecord* pEvent)
{
	HRESULT		hrErr;

	if (pEvent->what != app1Evt)
		return false;

	pEvent->what = mouseDown;

#if qFrameTools
	ASSERTCOND(gApplication->vtbl->m_IsClickInFrameToolsProcPtr != nil);
	if ((*gApplication->vtbl->m_IsClickInFrameToolsProcPtr)(gApplication, pEvent))
		return false;
#endif

	hrErr = OleSendLowLevelEvent(&pOleDoc->server.inplace.m_FrameInfo.psn, pEvent);
	ASSERTNOERROR(hrErr);

	return true;
}

//#pragma segment OleInPlaceServerDocSeg
Boolean OleInPlaceServerDocSetDefaultCursor(OleDocumentPtr pOleDoc)
{
	static Cursor	cursor;
	Point			mPt;
	WindowPtr		pWindow,
					pOverWindow;
	short			part;
	HRESULT			hrErr;

	if (!pOleDoc->server.inplace.m_fUIVisible)
		return false;

	ASSERTCOND(pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr != nil);
	pWindow = (*pOleDoc->m_pIDoc->lpVtbl->m_GetWindowProcPtr)(pOleDoc->m_pIDoc);

	GetMouse(&mPt);
	LocalToGlobal(&mPt);

	part = FindWindow(mPt, &pOverWindow);

	if (pWindow == pOverWindow)
	{
		CursPtr		pNewCursor;

		if (part == inContent)
			return false;

		switch (OleWhichGrowHandle(pWindow, mPt))
		{
			case 1:
			case 8:
				ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
				(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(kCursorBackwardResize));
				return true;

			case 3:
			case 6:
				ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
				(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(kCursorForwardResize));
				return true;

			case 2:
			case 7:
				ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
				(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(kCursorVertResize));
				return true;

			case 4:
			case 5:
				ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
				(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, *GetCursor(kCursorHorizResize));
				return true;
		}

		return false;
	}
	else
	{
		hrErr = OleGetCursor(&cursor, pWindow);
		ASSERTNOERROR(hrErr);

		ASSERTCOND(gApplication->vtbl->m_SetIdleCursorProcPtr != nil);
		(*gApplication->vtbl->m_SetIdleCursorProcPtr)(gApplication, &cursor);

		return true;
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocDoIdle(OleDocumentPtr pOleDoc)
{
	Rect		r;
	GrafPtr		oldPort;
	SIZEL		sizel;

	if (!pOleDoc->server.inplace.m_pSite || !pOleDoc->server.m_SizeChanged)
		return;

	GetPort(&oldPort);
	SetPort(pOleDoc->server.inplace.m_pWindow);

	r = pOleDoc->server.inplace.m_pWindow->portRect;
	LocalToGlobalRect(&r);

	r.left   += kRowHeadingsWidth;
	r.top    += kColumnHeadingsHeight;
	r.right  -= 15;
	r.bottom -= 15;

	SetPort(oldPort);

	sizel.cx = r.right - r.left;
	sizel.cy = r.bottom - r.top;
	OleInPlaceServerDocSetExtent(pOleDoc, &sizel);

	if (pOleDoc->server.inplace.m_fUIVisible)
		pOleDoc->server.inplace.m_pSite->lpVtbl->OnPosRectChange(pOleDoc->server.inplace.m_pSite, &r);
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocSetExtent(struct OleDocumentRec* pOleDoc, LPSIZEL lpsizel)
{
	// if the size has changed update our values and send an OnDataChange message
	if (pOleDoc->server.inplace.m_ptExtent.h != lpsizel->cx ||
		pOleDoc->server.inplace.m_ptExtent.v != lpsizel->cy)
	{
		pOleDoc->server.inplace.m_ptExtent.h = lpsizel->cx;
		pOleDoc->server.inplace.m_ptExtent.v = lpsizel->cy;

		OleDocSetSizeChanged(pOleDoc, true);
	}
}

//#pragma segment OleInPlaceServerDocSeg
void OleInPlaceServerDocUpdateWindowPosition(OleDocumentPtr pOleDoc)
{
	Rect		r;
	GrafPtr		oldPort;
	SIZEL		sizel;

	if (!pOleDoc->server.inplace.m_pSite)
		return;

	GetPort(&oldPort);
	SetPort(pOleDoc->server.inplace.m_pWindow);

	r = pOleDoc->server.inplace.m_pWindow->portRect;
	LocalToGlobalRect(&r);

	r.left   += kRowHeadingsWidth;
	r.top    += kColumnHeadingsHeight;
	r.right  -= 15;
	r.bottom -= 15;

	SetPort(oldPort);

	sizel.cx = r.right - r.left;
	sizel.cy = r.bottom - r.top;
	OleInPlaceServerDocSetExtent(pOleDoc, &sizel);

	if (pOleDoc->server.inplace.m_fUIVisible)
		pOleDoc->server.inplace.m_pSite->lpVtbl->OnPosRectChange(pOleDoc->server.inplace.m_pSite, &r);
}

//#pragma segment OleInPlaceServerDocSeg
HRESULT OleInPlaceServerDocSetObjectRects(OleDocumentPtr pOleDoc,
					LPCRECT prcPosRect,
					RgnHandle clipRgn,
					RgnHandle frameRgn,
					RgnHandle cliRgn)
{
	Rect	posRect;

	ASSERTCOND(clipRgn != nil);
	ASSERTCOND(frameRgn != nil);
	ASSERTCOND(cliRgn != nil);

	posRect = *prcPosRect;

#ifdef DISABLED
	// If the new size differs from the extent that we remember that means the
	// container is trying to zoom in or out. We don't support zooming so we call
	// ourselves with an OPEN verb so that we exit inplace and do an open edit.
	if (pOleDoc->server.inplace.m_ptExtent.h != (posRect.right - posRect.left) ||
		pOleDoc->server.inplace.m_ptExtent.v != (posRect.bottom - posRect.top))
	{
		OleServerDocOleObjectDoVerb(pOleDoc, OLEIVERB_OPEN, NULL, NULL, -1, NULL, NULL);
		return;
	}
#endif

	posRect.left   -= kRowHeadingsWidth;
	posRect.top    -= kColumnHeadingsHeight;
	posRect.right  += 15;
	posRect.bottom += 15;

	ASSERTCOND(pOleDoc->server.inplace.m_hPosRgn != nil);
	ASSERTCOND(pOleDoc->server.inplace.m_hClipRgn != nil);
	ASSERTCOND(pOleDoc->server.inplace.m_hFrameRgn != nil);
	ASSERTCOND(pOleDoc->server.inplace.m_hCliRgn != nil);

	RectRgn(pOleDoc->server.inplace.m_hPosRgn, &posRect);
	CopyRgn(clipRgn, pOleDoc->server.inplace.m_hClipRgn);
	CopyRgn(frameRgn, pOleDoc->server.inplace.m_hFrameRgn);
	CopyRgn(cliRgn, pOleDoc->server.inplace.m_hCliRgn);

	if (!pOleDoc->server.inplace.m_fUIVisible)
		return NOERROR;

	OleSetParentRgns(clipRgn, frameRgn, cliRgn);
	OleSizeObjectWindow(pOleDoc->server.inplace.m_pWindow, &posRect, true);

	OleSetInPlaceRects(
			pOleDoc->server.inplace.m_hPosRgn,
			pOleDoc->server.inplace.m_hClipRgn,
			pOleDoc->server.inplace.m_hFrameRgn,
			pOleDoc->server.inplace.m_hCliRgn,
			NULL);

	return NOERROR;
}

// OLDNAME: OleOutlineInPlaceServer.c
//#pragma segment OleOutlineInPlaceServerSeg
void OleOutlineInPlaceServerDoUnknownMenuKey(ApplicationPtr pApp, EventRecord* pEvent)
{
	OleOutlineDocPtr		pOleOutlineDoc;
	
	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);
	
	if (pOleOutlineDoc == nil)
		return;
	
	OleInPlaceServerDocDoMenuKey(&pOleOutlineDoc->m_OleDoc, pEvent);
}

//#pragma segment OleOutlineInPlaceServerSeg
void OleOutlineInPlaceServerDoUnknownMenuItem(ApplicationPtr pApp, EventRecord* pEvent, short menuID, short menuItem)
{
	OleOutlineDocPtr		pOleOutlineDoc;
	
	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);
	
	if (pOleOutlineDoc == nil)
		return;
	
	OleInPlaceServerDocDoMenu(&pOleOutlineDoc->m_OleDoc, pEvent, menuID, menuItem);
}

//#pragma segment OleOutlineInPlaceServerSeg
void OleOutlineInPlaceServerHideOthersCmd(ApplicationPtr pApp)
{
	OleOutlineDocPtr		pOleOutlineDoc;
	
	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);
	
	if (pOleOutlineDoc == nil)
		return;

	OleInPlaceServerDocHideOthersCmd(&pOleOutlineDoc->m_OleDoc);
}

//#pragma segment OleOutlineInPlaceServerSeg
void OleOutlineInPlaceServerProcessEvent(ApplicationPtr pApp, EventRecord* pEvent)
{
	OleOutlineDocPtr	pOleOutlineDoc;
	
	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);
	
	if (!(pOleOutlineDoc && OleInPlaceServerDocProcessEvent(&pOleOutlineDoc->m_OleDoc, pEvent)))
	{
		OleOutlineAppPtr 	pOleOutlineApp;
		
		pOleOutlineApp = (OleOutlineAppPtr)pApp;
		
		ASSERTCOND(pOleOutlineApp->vtbl->m_ProcessEventProcPtr != nil);
		(*pOleOutlineApp->vtbl->m_ProcessEventProcPtr)(pApp, pEvent);
	}
}



#endif // qOleInPlace


#endif // qOleTreatAs
