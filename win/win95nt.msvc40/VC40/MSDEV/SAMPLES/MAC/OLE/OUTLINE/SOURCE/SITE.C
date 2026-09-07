/*****************************************************************************\
*                                                                             *
*    OleContainerSite.c                                                       *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "App.h"
#include "Doc.h"
#include "Site.h"
#include "OleDebug.h"
#include "Util.h"

// OLDNAME: OleContainerSite.c

//#pragma segment OleContainerSiteInitSeg
void OleContainerSiteInit(OleContainerSitePtr pOleContainerSite, struct ContainerSiteImpl* pIContainerSite, IUnknown* pIUnknown)
{
	pOleContainerSite->m_pIContainerSite = 		pIContainerSite;
	pOleContainerSite->m_pIUnknown =			pIUnknown;
	
	pOleContainerSite->m_pOleObj				= nil;
	pOleContainerSite->m_pViewObj				= nil;
	pOleContainerSite->m_pPersistStorage		= nil;
	
#if qOleInPlace
	OleInPlaceContainerSiteInit(pOleContainerSite);
#endif
}

//#pragma segment OleContainerSiteSeg
void OleContainerSiteDispose(OleContainerSitePtr pOleContainerSite)
{
#if qOleInPlace
	OleInPlaceContainerSiteDispose(pOleContainerSite);
#endif
}

//#pragma segment OleContainerSiteSeg
unsigned long OleContainerSiteAddRef(OleContainerSitePtr pOleContainerSite)
{
	return (*pOleContainerSite->m_pIUnknown->lpVtbl->AddRef)(pOleContainerSite->m_pIUnknown);
}

//#pragma segment OleContainerSiteSeg
unsigned long OleContainerSiteRelease(OleContainerSitePtr pOleContainerSite)
{
	return (*pOleContainerSite->m_pIUnknown->lpVtbl->Release)(pOleContainerSite->m_pIUnknown);
}

//#pragma segment OleContainerSiteSeg
HRESULT OleContainerSiteLock(OleContainerSitePtr pOleContainerSite, Boolean fLock, Boolean fLastUnlockReleases)
{
    HRESULT hrErr;

    OleContainerSiteAddRef(pOleContainerSite);       // artificial AddRef to make object stable

    hrErr = CoLockObjectExternal(pOleContainerSite->m_pIUnknown, fLock, fLastUnlockReleases);
	ASSERTNOERROR(hrErr);
	
    OleContainerSiteRelease(pOleContainerSite);       // release artificial AddRef above

    return hrErr;
}

//#pragma segment OleContainerSiteSeg
HRESULT OleContainerSiteQueryInterface(OleContainerSitePtr pOleContainerSite, REFIID riid, void* * lplpvObj)
{
	return (*pOleContainerSite->m_pIUnknown->lpVtbl->QueryInterface)(pOleContainerSite->m_pIUnknown, riid, lplpvObj);
}

//#pragma segment OleContainerSiteSeg
HRESULT OleContainerSiteLocalQueryInterface(OleContainerSitePtr pOleContainerSite, REFIID riid, void* * lplpvObj)
{
#if qOleInPlace
	return OleInPlaceContainerSiteLocalQueryInterface(pOleContainerSite, riid, lplpvObj);
#endif

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleContainerSiteSeg
Boolean OleContainerSiteIsIPActive(OleContainerSitePtr pOleContainerSite)
{
#if qOleInPlace
	return OleInPlaceContainerSiteIsIPActive(pOleContainerSite);
#endif

	return false;
}

//#pragma segment OleContainerSiteSeg
Boolean OleContainerSiteIsUIActive(OleContainerSitePtr pOleContainerSite)
{
#if qOleInPlace
	return OleInPlaceContainerSiteIsUIActive(pOleContainerSite);
#endif

	return false;
}

//#pragma segment OleContainerSiteSeg
Boolean OleContainerSiteIsUIVisible(OleContainerSitePtr pOleContainerSite)
{
#if qOleInPlace
	return OleInPlaceContainerSiteIsUIVisible(pOleContainerSite);
#endif

	return false;
}

#if qOleInPlace
// OLDNAME: OleInPlaceContainerSiteInt.c

static IOleInPlaceSiteVtbl		gOleInPlaceContainerSiteVtbl;

//#pragma segment OleInPlaceContainerSiteIntInitSeg
void OleInPlaceContainerSiteInitInterfaces(void)
{
	// OleInPlace::Site method table
	{
		IOleInPlaceSiteVtbl*		p;
		
		p = &gOleInPlaceContainerSiteVtbl;
		
		p->QueryInterface			= IOleInPlaceContainerSiteQueryInterface;
		p->AddRef					= IOleInPlaceContainerSiteAddRef;
		p->Release					= IOleInPlaceContainerSiteRelease;
		p->GetWindow				= IOleInPlaceContainerSiteGetWindow;
		p->ContextSensitiveHelp		= IOleInPlaceContainerSiteContextSensitiveHelp;
		p->CanInPlaceActivate		= IOleInPlaceContainerSiteCanInPlaceActivate;
		p->OnInPlaceActivate		= IOleInPlaceContainerSiteOnInPlaceActivate;
		p->OnUIActivate				= IOleInPlaceContainerSiteOnUIActivate;
		p->OnUIVisible				= IOleInPlaceContainerSiteOnUIVisible;
		p->GetObjectRects			= IOleInPlaceContainerSiteGetObjectRects;
		p->GetWindowContext			= IOleInPlaceContainerSiteGetWindowContext;
		p->Scroll					= IOleInPlaceContainerSiteScroll;
		p->OnUIDeactivate			= IOleInPlaceContainerSiteOnUIDeactivate;
		p->OnInPlaceDeactivate		= IOleInPlaceContainerSiteOnInPlaceDeactivate;
		p->DiscardUndoState			= IOleInPlaceContainerSiteDiscardUndoState;
		p->DeactivateAndUndo		= IOleInPlaceContainerSiteDeactivateAndUndo;
		p->OnPosRectChange			= IOleInPlaceContainerSiteOnPosRectChange;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OleInPlaceContainerSiteIntSeg
void OleInPlaceContainerISiteInit(OleInPlaceContainerSiteImplPtr pOleInPlaceContainerSiteImpl, struct OleContainerSiteRec* pOleContainerSite)
{
	pOleInPlaceContainerSiteImpl->lpVtbl				= &gOleInPlaceContainerSiteVtbl;
	pOleInPlaceContainerSiteImpl->lpIOleContainerSite	= pOleContainerSite;
	pOleInPlaceContainerSiteImpl->cRef					= 0;
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteQueryInterface(LPOLEINPLACESITE lpThis, REFIID riid, void * * ppvObj)
{
	OleContainerSitePtr		pOleContainerSite;
	
	OleDbgEnterInterface();
	
	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleContainerSiteQueryInterface(pOleContainerSite, riid, ppvObj);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerSiteAddRef(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
	
	OleDbgEnterInterface();
	
	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleContainerSiteAddRef(pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP_(unsigned long) IOleInPlaceContainerSiteRelease(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
	
	OleDbgEnterInterface();
	
	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleContainerSiteRelease(pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteGetWindow(LPOLEINPLACESITE lpThis, WindowPtr * lphwnd)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	ASSERTCOND(lphwnd != nil);
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
	*lphwnd = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
	ASSERTCOND(*lphwnd != nil);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteContextSensitiveHelp(LPOLEINPLACESITE lpThis, unsigned long fEnterMode)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceSite::ContextSensitiveHelp: Please implement me!");

	return ResultFromScode(E_NOTIMPL);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteCanInPlaceActivate(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteCanInPlaceActivate(pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnInPlaceActivate(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnInPlaceActivate(pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnUIActivate(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnUIActivate(pOleContainerSite);
}
    
//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnUIVisible(LPOLEINPLACESITE lpThis, unsigned long visible)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnUIVisible(pOleContainerSite, (Boolean)visible);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteGetObjectRects(LPOLEINPLACESITE lpThis,
						Rect * lprcPosRect,		// server allocs, container fills, server frees
						RgnHandle clipRgn,		// defines vis rgn
						RgnHandle frameRgn,		// containing window strucRgn
						RgnHandle cliRgn)		// all container app strucRgns
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteGetObjectRects(
				pOleContainerSite,
				lprcPosRect,
				clipRgn,
				frameRgn,
				cliRgn);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteGetWindowContext(LPOLEINPLACESITE lpThis,
						LPOLEINPLACEFRAME * lplpFrame,
                        LPOLEINPLACEUIWINDOW * lplpDoc,
                        Rect *lprcPosRect,
						RgnHandle clipRgn,
						RgnHandle frameRgn,
						RgnHandle cliRgn,
                        LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteGetWindowContext(
				pOleContainerSite,
				lplpFrame,
				lplpDoc,
				lprcPosRect,
				clipRgn,
				frameRgn,
				cliRgn,
				lpFrameInfo);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteScroll(LPOLEINPLACESITE lpThis, long scrollExtent)
{
	OleDbgEnterInterface();

	return ResultFromScode(E_NOTIMPL);
}
       
//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnUIDeactivate(LPOLEINPLACESITE lpThis, unsigned long fUndoable)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnUIDeactivate(pOleContainerSite, (Boolean)fUndoable);
}
      
//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnInPlaceDeactivate(LPOLEINPLACESITE lpThis)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnInPlaceDeactivate(pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteDiscardUndoState(LPOLEINPLACESITE lpThis)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceSite::DiscardUndoState: Please implement me!");

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteDeactivateAndUndo(LPOLEINPLACESITE lpThis)
{
	OleDbgEnterInterface();

	ASSERTCONDSZ(0, "IOleInPlaceSite::DeactivateAndUndo: Please implement me!");

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteIntSeg
STDMETHODIMP IOleInPlaceContainerSiteOnPosRectChange(LPOLEINPLACESITE lpThis, LPCRECT lprcPosRect)
{
	OleContainerSitePtr		pOleContainerSite;
		
	OleDbgEnterInterface();

	pOleContainerSite = ((OleInPlaceContainerSiteImplPtr)lpThis)->lpIOleContainerSite;

	return OleInPlaceContainerSiteOnPosRectChange(pOleContainerSite, lprcPosRect);
}

// OLDNAME: OleInPlaceContainerSite.c
extern ApplicationPtr			gApplication;
extern OleApplicationPtr		gOleApp;

//#pragma segment OleInPlaceContainerSiteInitSeg
void OleInPlaceContainerSiteInit(OleContainerSitePtr pOleContainerSite)
{
	OleInPlaceContainerISiteInit(&pOleContainerSite->inplace.m_Site, pOleContainerSite);
}

//#pragma segment OleInPlaceContainerSiteSeg
void OleInPlaceContainerSiteDispose(OleContainerSitePtr pOleContainerSite)
{
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteLocalQueryInterface(OleContainerSitePtr pOleContainerSite, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IOleInPlaceSite))
	{
		*lplpvObj = &pOleContainerSite->inplace.m_Site;
		OleContainerSiteAddRef(pOleContainerSite);
		return NOERROR;
	}

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteGetObjectRects(OleContainerSitePtr pOleContainerSite,
						Rect * pPosRect,		// server allocs, container fills, server frees
						RgnHandle clipRgn,		// defines vis rgn
						RgnHandle frameRgn,		// containing window strucRgn
						RgnHandle cliRgn)		// all container app strucRgns
{
	GrafPtr			oldPort;
	WindowPtr		pWindow;
	Rect			rBounds;
	OleDocumentPtr	pOleDocument;
	RgnHandle		rgnFloats;
	Boolean			fToolsEnabled,
					fUIVisible;
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
	pWindow = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
	ASSERTCOND(pWindow != nil);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetBoundsProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetBoundsProcPtr)(pOleContainerSite->m_pIContainerSite, &rBounds);

	GetPort(&oldPort);
	SetPort(pWindow);
	
	// position rect must be in global coordinates
	*pPosRect = rBounds;
	LocalToGlobalRect(pPosRect);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr)(pOleContainerSite->m_pIContainerSite, clipRgn);

	CopyRgn(((WindowPeek)pWindow)->strucRgn, frameRgn);

#if qFrameTools
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr != nil);
	pOleDocument = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr)(pOleContainerSite->m_pIContainerSite);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr != nil);
	fToolsEnabled = (*pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr)(pOleDocument->m_pIDoc);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr != nil);
	fUIVisible = (*pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr)(pOleDocument->m_pIDoc);

	if (fToolsEnabled && fUIVisible)
	{
		rgnFloats = NewRgn();
		ASSERTCOND(rgnFloats != nil);

		ASSERTCOND(gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr != nil);
		(*gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr)(gApplication, rgnFloats);

		UnionRgn(frameRgn, rgnFloats, frameRgn);
		DiffRgn(clipRgn, rgnFloats, clipRgn);

		DisposeRgn(rgnFloats);
	}
#endif

	// get union of all window struc regions
	GetUnionWindowStrucRgns(cliRgn);
	
	SetPort(oldPort);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteGetWindowContext(OleContainerSitePtr pOleContainerSite,
						LPOLEINPLACEFRAME * ppFrame,
                        LPOLEINPLACEUIWINDOW * ppDoc,
                        Rect *pPosRect,
						RgnHandle clipRgn,
						RgnHandle frameRgn,
						RgnHandle cliRgn,
                        LPOLEINPLACEFRAMEINFO pFrameInfo)
{
	SIZEL	sizel;

	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == false);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIActive == false);

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetIPFrameProcPtr != nil);
	*ppFrame = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetIPFrameProcPtr)(pOleContainerSite->m_pIContainerSite);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetIPUIWindowProcPtr != nil);
	*ppDoc = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetIPUIWindowProcPtr)(pOleContainerSite->m_pIContainerSite);

	OleInPlaceContainerSiteGetObjectRects(pOleContainerSite, pPosRect, clipRgn, frameRgn, cliRgn);
	
	pFrameInfo->version      = 1;
	pFrameInfo->recordLength = sizeof(OLEINPLACEFRAMEINFO);

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
	pFrameInfo->frameWindow = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
	ASSERTCOND(pFrameInfo->frameWindow != nil);

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetAppCreatorProcPtr != nil);
	pFrameInfo->signature = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetAppCreatorProcPtr)(pOleContainerSite->m_pIContainerSite);

	GetCurrentProcess(&pFrameInfo->psn);

#ifdef DISABLED
	// We need the AdjustMenus call so we can update the checkmarks on our DbgICntr menu.
	pFrameInfo->fAdjustMenus = true;
#endif

	sizel.cx = pPosRect->right - pPosRect->left;
	sizel.cy = pPosRect->bottom - pPosRect->top;

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_SetExtentProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_SetExtentProcPtr)(pOleContainerSite->m_pIContainerSite, &sizel);

	return NOERROR;
}

HRESULT OleInPlaceContainerSiteCanInPlaceActivate(OleContainerSitePtr pOleContainerSite)
{
	unsigned long	drawAspect;

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetDrawAspectProcPtr != nil);
	drawAspect = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetDrawAspectProcPtr)(pOleContainerSite->m_pIContainerSite);

	if (drawAspect == DVASPECT_ICON)
		return ResultFromScode(S_FALSE);
	else
		return ResultFromScode(S_OK);
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnInPlaceActivate(OleContainerSitePtr pOleContainerSite)
{
	HRESULT		hrErr;
	
	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == false);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIActive == false);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIVisible == false);

	if (pOleContainerSite->inplace.m_fIPActive)
		return NOERROR;

	hrErr = NOERROR;
	
	// this should be null when we first go inplace active
	ASSERTCOND(pOleContainerSite->inplace.m_pObject == nil);

	/* OLE2NOTE: to avoid LRPC problems it is important to cache the
	**    IOleInPlaceObject* pointer and NOT to call QueryInterface
	**    each time it is needed.
	*/
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleObjectProcPtr != nil);
	pOleContainerSite->inplace.m_pObject = (LPOLEINPLACEOBJECT)(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleObjectProcPtr)(
					pOleContainerSite->m_pIContainerSite,
					&IID_IOleInPlaceObject);
	ASSERTCOND(pOleContainerSite->inplace.m_pObject != nil);

	if (!pOleContainerSite->inplace.m_fServerRunning)
	{
        /* OLE2NOTE: it is VERY important that an in-place container
        **    that also support linking to embeddings properly manage
        **    the running of its in-place objects. in an outside-in
        **    style in-place container, when the user clicks
        **    outside of the in-place active object, the object gets
        **    UIDeactivated and the object hides its window. in order
        **    to make the object fast to reactivate, the container
        **    deliberately does not call IOleObject::Close. the object
        **    stays running in the invisible unlocked state. the idea
        **    here is if the user simply clicks outside of the object
        **    and then wants to double click again to re-activate the
        **    object, we do not want this to be slow. if we want to
        **    keep the object running, however, we MUST Lock it
        **    running. otherwise the object will be in an unstable
        **    state where if a linking client does a "silent-update"
        **    (eg. UpdateNow from the Links dialog), then the in-place
        **    server will shut down even before the object has a chance
        **    to be saved back in its container. this saving normally
        **    occurs when the in-place container closes the object. also
        **    keeping the object in the unstable, hidden, running,
        **    not-locked state can cause problems in some scenarios.
        **    ICntrOtl keeps only one object running. if the user
        **    intiates a DoVerb on another object, then that last
        **    running in-place active object is closed. a more
        **    sophistocated in-place container may keep more object running.
        **    this lock gets unlocked in OleInPlaceContainerSiteCloseOleObject.
        */

		pOleContainerSite->inplace.m_fServerRunning = true;
		
		ASSERTCOND(pOleContainerSite->inplace.m_pObject != nil);
		hrErr = OleLockRunning((LPUNKNOWN)pOleContainerSite->inplace.m_pObject, true, false);
		ASSERTNOERROR(hrErr);
	}

	pOleContainerSite->inplace.m_fIPActive = true;

	return hrErr;
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnInPlaceDeactivate(OleContainerSitePtr pOleContainerSite)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == true);

	if (!pOleContainerSite->inplace.m_fIPActive)
		return NOERROR;

	ASSERTCONDSZ(!pOleContainerSite->inplace.m_fUIActive, "Call OnUIDeactivate before calling OnIPDeactivate");

	hrErr = OleInPlaceContainerSiteOnUIDeactivate(pOleContainerSite, false);
	ASSERTNOERROR(hrErr);
	
	ASSERTCOND(pOleContainerSite->inplace.m_pObject != nil);
	if (pOleContainerSite->inplace.m_pObject)
	{
		OleStdRelease((LPUNKNOWN)pOleContainerSite->inplace.m_pObject);
		pOleContainerSite->inplace.m_pObject = nil;
	}

	pOleContainerSite->inplace.m_fIPActive        = false;
	pOleContainerSite->inplace.m_fUIActive        = false;
	pOleContainerSite->inplace.m_fUIVisible       = false;

	pOleContainerSite->inplace.m_fChangesUndoable = false;
		
	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnUIActivate(OleContainerSitePtr pOleContainerSite)
{
	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == true);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIActive == false);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIVisible == false);
	
	if (pOleContainerSite->inplace.m_fUIActive)
		return NOERROR;

	ASSERTCONDSZ(pOleContainerSite->inplace.m_fIPActive, "Call OnIPActivate before OnUIActivate");

	// make sure our background status is current
	gApplication->m_InBackground = !AmIFrontProcess();

	if (!gApplication->m_InBackground)
		gOleApp->container.inplace.m_fIgnoreSuspendEvent = true;
	gOleApp->container.inplace.m_fIgnoreResumeEvent = false;

	pOleContainerSite->inplace.m_fUIActive  = true;
	pOleContainerSite->inplace.m_fUIVisible = true;		// going uiactive implies going uivisible(true)

#ifdef __OBSOLETE__
	OleInPlaceContainerSiteUpdatePosition(pOleContainerSite);
#endif

	// force a redraw (after m_fUIVisible is set to true)
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr)(pOleContainerSite->m_pIContainerSite);

	return NOERROR;	
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnUIDeactivate(OleContainerSitePtr pOleContainerSite, Boolean fUndoable)
{	
	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == true);

	if (!pOleContainerSite->inplace.m_fUIActive)
		return NOERROR;
	
	pOleContainerSite->inplace.m_fUIActive  = false;
	pOleContainerSite->inplace.m_fUIVisible = false;		// going uideactive implies going uivisible(false)

	// reset allocated border space (indicates container can show its tools)
	ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_InPlaceSetBorderSpaceProcPtr != nil);
	(*gOleApp->m_pIApp->lpVtbl->m_InPlaceSetBorderSpaceProcPtr)(gOleApp->m_pIApp, NULL);

	if (pOleContainerSite->inplace.m_fMeKilledInPlace)
	{
		ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_ShowProcPtr != nil);
		(*gOleApp->m_pIApp->lpVtbl->m_ShowProcPtr)(gOleApp->m_pIApp);
		pOleContainerSite->inplace.m_fMeKilledInPlace = false;
	}

	// force a redraw (after m_fUIVisible is set to false)
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr)(pOleContainerSite->m_pIContainerSite);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnUIVisible(OleContainerSitePtr pOleContainerSite, Boolean fVisible)
{	
	ASSERTCOND(pOleContainerSite->inplace.m_fIPActive == true);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIActive == true);
	ASSERTCOND(pOleContainerSite->inplace.m_fUIVisible != fVisible);
	
	ASSERTCOND(pOleContainerSite->inplace.m_pObject != nil);
	
	if (pOleContainerSite->inplace.m_fUIVisible == fVisible)
		return NOERROR;

	pOleContainerSite->inplace.m_fUIVisible = fVisible;

	// make sure our background status is current
	gApplication->m_InBackground = !AmIFrontProcess();

	if (fVisible)
	{
#if qFrameTools
		DocumentPtr		pDocument;

		ASSERTCOND(gApplication->vtbl->m_GetCurrentDocProcPtr != nil);
		pDocument = (*gApplication->vtbl->m_GetCurrentDocProcPtr)(gApplication);

		if (pDocument->m_fShowFrameTools)
		{
			ASSERTCOND(gApplication->vtbl->m_ShowFrameToolsProcPtr != nil);
			(*gApplication->vtbl->m_ShowFrameToolsProcPtr)(gApplication);
		}
		else
		{
			ASSERTCOND(gApplication->vtbl->m_HideFrameToolsProcPtr != nil);
			(*gApplication->vtbl->m_HideFrameToolsProcPtr)(gApplication);
		}
#endif

		// if the server is coming visible we need to ignore the next suspend event
		if (!gApplication->m_InBackground)
			gOleApp->container.inplace.m_fIgnoreSuspendEvent = true;
		gOleApp->container.inplace.m_fIgnoreResumeEvent = false;
	}
	else
	{
		OleDocumentPtr		pOleDoc;

		ASSERTCOND(gOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
		pOleDoc = (*gOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(gOleApp->m_pIApp);

		if (pOleDoc->container.inplace.m_fIgnoreOnUIVisible)
		{
			pOleDoc->container.inplace.m_fIgnoreOnUIVisible = false;
		}
		else
		{
			WindowPtr		pWindow;
			DocumentPtr		pDocument;

#if qFrameTools
			if (gApplication->m_InBackground)
			{
				ASSERTCOND(gApplication->vtbl->m_HideFrameToolsProcPtr != nil);
				(*gApplication->vtbl->m_HideFrameToolsProcPtr)(gApplication);
			}
#endif

			ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
			pWindow = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
			ASSERTCOND(pWindow != nil);

			HiliteWindow(pWindow, false);

			ASSERTCOND(gApplication->vtbl->m_FindDocProcPtr != nil);
			pDocument = (*gApplication->vtbl->m_FindDocProcPtr)(gApplication, pWindow);

			if (pDocument->m_Active)
			{
				ASSERTCOND(pDocument->vtbl->m_DoActivateProcPtr != nil);
				(*pDocument->vtbl->m_DoActivateProcPtr)(pDocument, false);
			}
		}
	}

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_DrawNowProcPtr)(pOleContainerSite->m_pIContainerSite);

	return NOERROR;
}

//#pragma segment OleInPlaceContainerSiteSeg
HRESULT OleInPlaceContainerSiteOnPosRectChange(OleContainerSitePtr pOleContainerSite, LPCRECT prcPosRect)
{
	HRESULT			hrErr;
	WindowPtr		oldPort;
	WindowPtr		pWindow;
	RgnHandle		cliRgn;
	Rect			rBounds;
	RgnHandle		clipRgn;
	RgnHandle		frameRgn;
	OleDocumentPtr	pOleDocument;
	RgnHandle		rgnFloats;
	Boolean			fToolsEnabled,
					fUIVisible;

	// if the inplace object isn't uivisible we shouldn't call the SetObjectRects method
	if (!pOleContainerSite->inplace.m_fUIVisible)
		return NOERROR;

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
	pWindow = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
	ASSERTCOND(pWindow != nil);

	GetPort(&oldPort);
	SetPort(pWindow);
	
	rBounds = *prcPosRect;
		
	GlobalToLocalRect(&rBounds);
	
	// let the site change the size and position
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_OnPosRectChangeProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_OnPosRectChangeProcPtr)(pOleContainerSite->m_pIContainerSite, &rBounds);
	
	LocalToGlobalRect(&rBounds);
	
	clipRgn = NewRgn();
	ASSERTCOND(clipRgn != nil);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr)(pOleContainerSite->m_pIContainerSite, clipRgn);
	
	frameRgn = NewRgn();
	ASSERTCOND(frameRgn != nil);
	CopyRgn(((WindowPeek)pWindow)->strucRgn, frameRgn);

#if qFrameTools
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr != nil);
	pOleDocument = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr)(pOleContainerSite->m_pIContainerSite);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr != nil);
	fToolsEnabled = (*pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr)(pOleDocument->m_pIDoc);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr != nil);
	fUIVisible = (*pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr)(pOleDocument->m_pIDoc);

	if (fToolsEnabled && fUIVisible)
	{
		rgnFloats = NewRgn();
		ASSERTCOND(rgnFloats != nil);

		ASSERTCOND(gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr != nil);
		(*gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr)(gApplication, rgnFloats);

		UnionRgn(frameRgn, rgnFloats, frameRgn);
		DiffRgn(clipRgn, rgnFloats, clipRgn);

		DisposeRgn(rgnFloats);
	}
#endif

	SetPort(oldPort);

	cliRgn = NewRgn();
	ASSERTCOND(cliRgn != nil);
	GetUnionWindowStrucRgns(cliRgn);

	hrErr = pOleContainerSite->inplace.m_pObject->lpVtbl->SetObjectRects(pOleContainerSite->inplace.m_pObject,
								&rBounds, clipRgn, frameRgn, cliRgn);
	ASSERTNOERROR(hrErr);
	
	DisposeRgn(cliRgn);
	DisposeRgn(clipRgn);
	DisposeRgn(frameRgn);
	
	return hrErr;
}

//#pragma segment OleInPlaceContainerSiteSeg
void OleInPlaceContainerSiteUpdatePosition(OleContainerSitePtr pOleContainerSite)
{
	HRESULT			hrErr;
	WindowPtr		oldPort;
	WindowPtr		pWindow;
	RgnHandle		cliRgn;
	Rect			rBounds;
	RgnHandle		clipRgn;
	RgnHandle		frameRgn;
	OleDocumentPtr	pOleDocument;
	RgnHandle		rgnFloats;
	Boolean			fToolsEnabled,
					fUIVisible;

	// if the inplace object isn't uivisible we shouldn't call the SetObjectRects method
	if (!pOleContainerSite->inplace.m_fUIVisible)
		return;

	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr != nil);
	pWindow = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetWindowProcPtr)(pOleContainerSite->m_pIContainerSite);
	ASSERTCOND(pWindow != nil);

	GetPort(&oldPort);
	SetPort(pWindow);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetBoundsProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetBoundsProcPtr)(pOleContainerSite->m_pIContainerSite, &rBounds);
	
	LocalToGlobalRect(&rBounds);
	
	clipRgn = NewRgn();
	ASSERTCOND(clipRgn != nil);
	
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr != nil);
	(*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetClipRgnProcPtr)(pOleContainerSite->m_pIContainerSite, clipRgn);

	frameRgn = NewRgn();
	ASSERTCOND(frameRgn != nil);
	CopyRgn(((WindowPeek)pWindow)->strucRgn, frameRgn);

#if qFrameTools
	ASSERTCOND(pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr != nil);
	pOleDocument = (*pOleContainerSite->m_pIContainerSite->lpVtbl->m_GetOleDocumentProcPtr)(pOleContainerSite->m_pIContainerSite);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr != nil);
	fToolsEnabled = (*pOleDocument->m_pIDoc->lpVtbl->m_FrameToolsEnabledProcPtr)(pOleDocument->m_pIDoc);

	ASSERTCOND(pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr != nil);
	fUIVisible = (*pOleDocument->m_pIDoc->lpVtbl->m_IsUIVisibleProcPtr)(pOleDocument->m_pIDoc);

	if (fToolsEnabled && fUIVisible)
	{
		rgnFloats = NewRgn();
		ASSERTCOND(rgnFloats != nil);

		ASSERTCOND(gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr != nil);
		(*gApplication->vtbl->m_FloatWindowsStrucRgnsProcPtr)(gApplication, rgnFloats);

		UnionRgn(frameRgn, rgnFloats, frameRgn);
		DiffRgn(clipRgn, rgnFloats, clipRgn);

		DisposeRgn(rgnFloats);
	}
#endif
	SetPort(oldPort);

	cliRgn = NewRgn();
	ASSERTCOND(cliRgn != nil);
	GetUnionWindowStrucRgns(cliRgn);

	hrErr = pOleContainerSite->inplace.m_pObject->lpVtbl->SetObjectRects(pOleContainerSite->inplace.m_pObject,
								&rBounds, clipRgn, frameRgn, cliRgn);
	ASSERTNOERROR(hrErr);
	
	DisposeRgn(cliRgn);
	DisposeRgn(clipRgn);
	DisposeRgn(frameRgn);
}

//#pragma segment OleInPlaceContainerSiteSeg
Boolean OleInPlaceContainerSiteIsIPActive(OleContainerSitePtr pOleContainerSite)
{
	return pOleContainerSite->inplace.m_fIPActive;
}

//#pragma segment OleInPlaceContainerSiteSeg
Boolean OleInPlaceContainerSiteIsUIActive(OleContainerSitePtr pOleContainerSite)
{
	return pOleContainerSite->inplace.m_fUIActive;
}

//#pragma segment OleInPlaceContainerSiteSeg
Boolean OleInPlaceContainerSiteIsUIVisible(OleContainerSitePtr pOleContainerSite)
{
	return pOleContainerSite->inplace.m_fUIVisible;
}

//#pragma segment OleInPlaceContainerSiteSeg
void OleInPlaceContainerSiteIPDeactivate(OleContainerSitePtr pOleContainerSite)
{
	HRESULT			hrErr;

	if (pOleContainerSite->inplace.m_pObject)
	{
		pOleContainerSite->inplace.m_fMeKilledInPlace = true;

		ASSERTCOND(pOleContainerSite->inplace.m_pObject->lpVtbl->InPlaceDeactivate != nil);
		hrErr = pOleContainerSite->inplace.m_pObject->lpVtbl->InPlaceDeactivate(pOleContainerSite->inplace.m_pObject);
		ASSERTNOERROR(hrErr);

		gApplication->m_InBackground = !AmIFrontProcess();
	}
}

//#pragma segment OleInPlaceContainerSiteSeg
void OleInPlaceContainerSiteUIDeactivate(OleContainerSitePtr pOleContainerSite)
{
	HRESULT			hrErr;

	if (pOleContainerSite->inplace.m_pObject)
	{
		pOleContainerSite->inplace.m_fMeKilledInPlace = true;

		ASSERTCOND(pOleContainerSite->inplace.m_pObject->lpVtbl->UIDeactivate != nil);
		hrErr = pOleContainerSite->inplace.m_pObject->lpVtbl->UIDeactivate(pOleContainerSite->inplace.m_pObject);
		ASSERTNOERROR(hrErr);

		gApplication->m_InBackground = !AmIFrontProcess();
	}
}

 
#endif // qOleInPlace
