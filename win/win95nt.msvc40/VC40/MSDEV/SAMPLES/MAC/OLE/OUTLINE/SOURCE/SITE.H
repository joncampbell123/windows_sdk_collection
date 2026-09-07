/*****************************************************************************\
*                                                                             *
*    OleContainerSite.h                                                       *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __SITE_H__
#define __SITE_H__

#ifdef __MWERKS__
#if qOle
struct ContainerSiteImpl;
#endif
#endif

#if qOleInPlace
// OLDNAME: OleInPlaceContainerSiteInt.h
void OleInPlaceContainerSiteInitInterfaces(void);

struct OleInPlaceContainerSiteImpl {
	IOleInPlaceSiteVtbl*		lpVtbl;
	struct OleContainerSiteRec*	lpIOleContainerSite;
	unsigned long				cRef;
};

typedef struct OleInPlaceContainerSiteImpl OleInPlaceContainerSiteImpl, *OleInPlaceContainerSiteImplPtr;

void OleInPlaceContainerISiteInit(OleInPlaceContainerSiteImplPtr pOleInPlaceContainerSiteImpl, struct OleContainerSiteRec* pOleContainerSite);

// OleInPlace::Site methods
STDMETHODIMP IOleInPlaceContainerSiteQueryInterface(LPOLEINPLACESITE lpThis, REFIID riid, void * * ppvObj);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerSiteAddRef(LPOLEINPLACESITE lpThis);
STDMETHODIMP_(unsigned long) IOleInPlaceContainerSiteRelease(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteGetWindow(LPOLEINPLACESITE lpThis, WindowPtr * lphwnd);
STDMETHODIMP IOleInPlaceContainerSiteContextSensitiveHelp(LPOLEINPLACESITE lpThis, unsigned long fEnterMode);
STDMETHODIMP IOleInPlaceContainerSiteCanInPlaceActivate(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteOnInPlaceActivate(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteOnUIActivate(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteOnUIVisible(LPOLEINPLACESITE lpThis, unsigned long visible);
STDMETHODIMP IOleInPlaceContainerSiteGetObjectRects(LPOLEINPLACESITE lpThis,
						Rect * lprcPosRect,		// server allocs, container fills, server frees
						RgnHandle clipRgn,		// defines vis rgn
						RgnHandle frameRgn,		// containing window strucRgn
						RgnHandle cliRgn);		// all container app strucRgns
STDMETHODIMP IOleInPlaceContainerSiteGetWindowContext(LPOLEINPLACESITE lpThis,
						LPOLEINPLACEFRAME * lplpFrame,
                        LPOLEINPLACEUIWINDOW * lplpDoc,
                        Rect *lprcPosRect,
						RgnHandle clipRgn,
						RgnHandle frameRgn,
						RgnHandle cliRgn,
                        LPOLEINPLACEFRAMEINFO lpFrameInfo);
STDMETHODIMP IOleInPlaceContainerSiteScroll(LPOLEINPLACESITE lpThis, long scrollExtent);
STDMETHODIMP IOleInPlaceContainerSiteOnUIDeactivate(LPOLEINPLACESITE lpThis, unsigned long fUndoable);
STDMETHODIMP IOleInPlaceContainerSiteOnInPlaceDeactivate(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteDiscardUndoState(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteDeactivateAndUndo(LPOLEINPLACESITE lpThis);
STDMETHODIMP IOleInPlaceContainerSiteOnPosRectChange(LPOLEINPLACESITE lpThis, LPCRECT lprcPosRect);

// OLDNAME: OleInPlaceContainerSite.h
struct OleInPlaceContainerSiteRec {
	Boolean						m_fIPActive;			// is object in-place active (undo valid)
	Boolean						m_fUIActive;
	Boolean						m_fUIVisible;			// is object's in-place window visible

	Boolean						m_fMeKilledInPlace;
	
	Boolean						m_fInsideOutObject;		// is obj inside-out (visible when loaded)
	Boolean						m_fChangesUndoable;		// can in-place object do undo
	Boolean						m_fServerRunning;		// is in-place server running
	
	LPOLEINPLACEOBJECT			m_pObject;
	
	OleInPlaceContainerSiteImpl	m_Site;
};

typedef struct OleInPlaceContainerSiteRec OleInPlaceContainerSiteRec, *OleInPlaceContainerSitePtr;

void OleInPlaceContainerSiteInit(struct OleContainerSiteRec* pOleContainerSite);
void OleInPlaceContainerSiteDispose(struct OleContainerSiteRec* pOleContainerSite);

HRESULT OleInPlaceContainerSiteLocalQueryInterface(struct OleContainerSiteRec* pOleContainerSite, REFIID riid, void* * lplpvObj);
HRESULT OleInPlaceContainerSiteGetObjectRects(struct OleContainerSiteRec* pOleContainerSite,
						Rect * pPosRect,		// server allocs, container fills, server frees
						RgnHandle clipRgn,		// defines vis rgn
						RgnHandle frameRgn,		// containing window strucRgn
						RgnHandle cliRgn);		// all container app strucRgns
HRESULT OleInPlaceContainerSiteGetWindowContext(struct OleContainerSiteRec* pOleContainerSite,
						LPOLEINPLACEFRAME * ppFrame,
                        LPOLEINPLACEUIWINDOW * ppDoc,
                        Rect *pPosRect,
						RgnHandle clipRgn,
						RgnHandle frameRgn,
						RgnHandle cliRgn,
                        LPOLEINPLACEFRAMEINFO pFrameInfo);
HRESULT OleInPlaceContainerSiteCanInPlaceActivate(struct OleContainerSiteRec* pOleContainerSite);
HRESULT OleInPlaceContainerSiteOnInPlaceActivate(struct OleContainerSiteRec* pOleContainerSite);
HRESULT OleInPlaceContainerSiteOnInPlaceDeactivate(struct OleContainerSiteRec* pOleContainerSite);
HRESULT OleInPlaceContainerSiteOnUIActivate(struct OleContainerSiteRec* pOleContainerSite);
HRESULT OleInPlaceContainerSiteOnUIDeactivate(struct OleContainerSiteRec* pOleContainerSite, Boolean fUndoable);
HRESULT OleInPlaceContainerSiteOnPosRectChange(struct OleContainerSiteRec* pOleContainerSite, LPCRECT prcPosRect);
HRESULT OleInPlaceContainerSiteOnUIVisible(struct OleContainerSiteRec* pOleContainerSite, Boolean fVisible);

void OleInPlaceContainerSiteUpdatePosition(struct OleContainerSiteRec* pOleContainerSite);
Boolean OleInPlaceContainerSiteIsIPActive(struct OleContainerSiteRec* pOleContainerSite);
Boolean OleInPlaceContainerSiteIsUIActive(struct OleContainerSiteRec* pOleContainerSite);
Boolean OleInPlaceContainerSiteIsUIVisible(struct OleContainerSiteRec* pOleContainerSite);
void OleInPlaceContainerSiteIPDeactivate(struct OleContainerSiteRec* pOleContainerSite);
void OleInPlaceContainerSiteUIDeactivate(struct OleContainerSiteRec* pOleContainerSite);

#endif  // qOleInPlace


// OLDNAME: OleContainerSiteInterface.h

typedef WindowPtr (*OleContainerSiteGetWindowProcPtr)(struct ContainerSiteImpl* lpThis);
typedef struct OleDocumentRec* (*OleContainerSiteGetOleDocumentProcPtr)(struct ContainerSiteImpl* lpThis);
typedef OSType (*OleContainerSiteGetAppCreatorProcPtr)(struct ContainerSiteImpl* lpThis);
typedef void (*OleContainerSiteGetBoundsProcPtr)(struct ContainerSiteImpl* lpThis, Rect* rBounds);
typedef LPUNKNOWN (*OleContainerSiteGetOleObject)(struct ContainerSiteImpl* lpThis, REFIID riid);
typedef unsigned long (*OleContainerSiteGetDrawAspect)(struct ContainerSiteImpl* lpThis);
typedef void (*OleContainerSiteSetExtent)(struct ContainerSiteImpl* lpThis, LPSIZEL lpsizel);

#if qOleInPlace
typedef void (*OleContainerSiteGetClipRgnProcPtr)(struct ContainerSiteImpl* lpThis, RgnHandle clipRgn);
typedef LPOLEINPLACEFRAME (*OleContainerSiteGetIPFrameProcPtr)(struct ContainerSiteImpl* lpThis);
typedef LPOLEINPLACEUIWINDOW (*OleContainerSiteGetIPUIWindowProcPtr)(struct ContainerSiteImpl* lpThis);
typedef void (*OleContainerSiteDrawNowProcPtr)(struct ContainerSiteImpl* lpThis);
typedef void (*OleContainerSiteOnPosRectChangeProcPtr)(struct ContainerSiteImpl* lpThis, Rect* rBounds);
#endif

struct IContainerSiteVtbl {
	OleContainerSiteGetWindowProcPtr				m_GetWindowProcPtr;
	OleContainerSiteGetOleDocumentProcPtr			m_GetOleDocumentProcPtr;
	OleContainerSiteGetAppCreatorProcPtr			m_GetAppCreatorProcPtr;
	OleContainerSiteGetBoundsProcPtr				m_GetBoundsProcPtr;
	OleContainerSiteGetOleObject					m_GetOleObjectProcPtr;
	OleContainerSiteGetDrawAspect					m_GetDrawAspectProcPtr;
	OleContainerSiteSetExtent						m_SetExtentProcPtr;

#if qOleInPlace
	OleContainerSiteGetClipRgnProcPtr				m_GetClipRgnProcPtr;

	OleContainerSiteOnPosRectChangeProcPtr			m_OnPosRectChangeProcPtr;
	
	OleContainerSiteGetIPFrameProcPtr				m_GetIPFrameProcPtr;
	OleContainerSiteGetIPUIWindowProcPtr			m_GetIPUIWindowProcPtr;

	OleContainerSiteDrawNowProcPtr					m_DrawNowProcPtr;
#endif
};

typedef struct IContainerSiteVtbl IContainerSiteVtbl;

struct ContainerSiteImpl {
	IContainerSiteVtbl*			lpVtbl;
};

typedef struct ContainerSiteImpl ContainerSiteImpl, *ContainerSiteImplPtr;

// OLDNAME: OleContainerSite.h
struct OleContainerSiteRec {

	struct ContainerSiteImpl*			m_pIContainerSite;
	
	IUnknown*							m_pIUnknown;

	LPOLEOBJECT							m_pOleObj;						// ptr to IOleObject* when loaded
	LPVIEWOBJECT						m_pViewObj;						// ptr to IViewObject* when loaded
	LPPERSISTSTORAGE					m_pPersistStorage;				// ptr to IPersistStorage* when loaded

#if qOleInPlace
	OleInPlaceContainerSiteRec			inplace;
#endif
};

typedef struct OleContainerSiteRec OleContainerSiteRec, *OleContainerSitePtr;

void OleContainerSiteInit(OleContainerSitePtr pOleContainerSite, struct ContainerSiteImpl* pIContainerSite, IUnknown* pIUnknown);
void OleContainerSiteDispose(OleContainerSitePtr pOleContainerSite);

unsigned long OleContainerSiteAddRef(OleContainerSitePtr pOleContainerSite);
unsigned long OleContainerSiteRelease(OleContainerSitePtr pOleContainerSite);
HRESULT OleContainerSiteLock(OleContainerSitePtr pOleContainerSite, Boolean fLock, Boolean fLastUnlockReleases);
HRESULT OleContainerSiteQueryInterface(OleContainerSitePtr pOleContainerSite, REFIID riid, void* * lplpvObj);		// query the document for interfaces
HRESULT OleContainerSiteLocalQueryInterface(OleContainerSitePtr pOleContainerSite, REFIID riid, void* * lplpvObj);	// query only OleDoc for interfaces

Boolean OleContainerSiteIsIPActive(OleContainerSitePtr pOleContainerSite);
Boolean OleContainerSiteIsUIActive(OleContainerSitePtr pOleContainerSite);
Boolean OleContainerSiteIsUIVisible(OleContainerSitePtr pOleContainerSite);


#endif
