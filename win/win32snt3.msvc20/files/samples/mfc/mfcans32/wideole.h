//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       wideole.h
//
//  Contents:   Unicode Wrappers for ANSI Ole2 Interfaces and APIs.
//
//  Classes:    CDropTargetW
//              CPersistStorageW
//              CPersistFileW
//              CEnumOLEVERBW
//              COleObjectW
//              COleClientSiteW
//              COleContainerW
//              COleItemContainerW
//              COleAdviseHolderW
//              COleLinkW
//              COleInPlaceObjectW
//              COleInPlaceActiveObjectW
//              COleInPlaceFrameW
//              COleInPlaceSiteW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//
//  Forward declarations
//
class CDropTargetW;
class CPersistStorageW;
class CPersistFileW;
class CEnumOLEVERBW;
class COleObjectW;
class COleClientSiteW;
class CRunnableObjectW;
class COleItemContainerW;
class COleAdviseHolderW;
class COleLinkW;
class COleInPlaceObjectW;
class COleInPlaceActiveObjectW;
class COleInPlaceFrameW;
class COleInPlaceSiteW;



//+--------------------------------------------------------------------------
//
//  Class:      CDropTargetW
//
//  Synopsis:   Class definition of IDropTargetW
//
//---------------------------------------------------------------------------
class CDropTargetW : CWideInterface
{
public:
	// *** IDropTarget methods ***
	STDMETHOD(DragEnter) (LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragOver) (DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragLeave) (VOID);
	STDMETHOD(Drop) (LPDATAOBJECT pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);

	inline CDropTargetW(LPUNKNOWN pUnk, IDropTargetA * pANSI) :
			CWideInterface(ID_IDropTarget, pUnk, (LPUNKNOWN)pANSI) {};

	inline IDropTargetA * GetANSI() const
		{ return (IDropTargetA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CPersistStorageW
//
//  Synopsis:   Class definition of IPersistStorageW
//
//---------------------------------------------------------------------------
class CPersistStorageW : CWideInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistStorage methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(InitNew) (LPSTORAGE pStg);
	STDMETHOD(Load) (LPSTORAGE pStg);
	STDMETHOD(Save) (LPSTORAGE pStgSave, BOOL fSameAsLoad);
	STDMETHOD(SaveCompleted) (LPSTORAGE pStgNew);
	STDMETHOD(HandsOffStorage) (VOID);

	inline CPersistStorageW(LPUNKNOWN pUnk, IPersistStorageA * pANSI) :
			CWideInterface(ID_IPersistStorage, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPersistStorageA * GetANSI() const
		{ return (IPersistStorageA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CPersistFileW
//
//  Synopsis:   Class definition of IPersistFileW
//
//---------------------------------------------------------------------------
class CPersistFileW : CWideInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistFile methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(Load) (LPCOLESTR lpszFileName, DWORD grfMode);
	STDMETHOD(Save) (LPCOLESTR lpszFileName, BOOL fRemember);
	STDMETHOD(SaveCompleted) (LPCOLESTR lpszFileName);
	STDMETHOD(GetCurFile) (LPOLESTR FAR* lplpszFileName);

	inline CPersistFileW(LPUNKNOWN pUnk, IPersistFileA * pANSI) :
			CWideInterface(ID_IPersistFile, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPersistFileA * GetANSI() const
		{ return (IPersistFileA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumOLEVERBW
//
//  Synopsis:   Class definition of IEnumOLEVERBW
//
//---------------------------------------------------------------------------
class CEnumOLEVERBW : CWideInterface
{
public:
	// *** IEnumOLEVERB methods ***
	STDMETHOD(Next) (ULONG celt, LPOLEVERB rgelt, ULONG FAR* pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumOLEVERB FAR* FAR* ppenm);

	inline CEnumOLEVERBW(LPUNKNOWN pUnk, IEnumOLEVERBA * pANSI) :
			CWideInterface(ID_IEnumOLEVERB, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumOLEVERBA * GetANSI() const
		{ return (IEnumOLEVERBA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleObjectW
//
//  Synopsis:   Class definition of IOleObjectW
//
//---------------------------------------------------------------------------
class COleObjectW : CWideInterface
{
public:
	// *** IOleObject methods ***
	STDMETHOD(SetClientSite) (LPOLECLIENTSITE pClientSite);
	STDMETHOD(GetClientSite) (LPOLECLIENTSITE FAR* ppClientSite);
	STDMETHOD(SetHostNames) (LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
	STDMETHOD(Close) (DWORD dwSaveOption);
	STDMETHOD(SetMoniker) (DWORD dwWhichMoniker, LPMONIKER pmk);
	STDMETHOD(GetMoniker) (DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKER FAR* ppmk);
	STDMETHOD(InitFromData) (LPDATAOBJECT pDataObject,
				BOOL fCreation,
				DWORD dwReserved);
	STDMETHOD(GetClipboardData) (DWORD dwReserved,
				LPDATAOBJECT FAR* ppDataObject);
	STDMETHOD(DoVerb) (LONG iVerb,
				LPMSG lpmsg,
				LPOLECLIENTSITE pActiveSite,
				LONG lindex,
				HWND hwndParent,
				LPCRECT lprcPosRect);
	STDMETHOD(EnumVerbs) (LPENUMOLEVERB FAR* ppenumOleVerb);
	STDMETHOD(Update) (VOID);
	STDMETHOD(IsUpToDate) (VOID);
	STDMETHOD(GetUserClassID) (CLSID FAR* pClsid);
	STDMETHOD(GetUserType) (DWORD dwFormOfType, LPOLESTR FAR* pszUserType);
	STDMETHOD(SetExtent) (DWORD dwDrawAspect, LPSIZEL lpsizel);
	STDMETHOD(GetExtent) (DWORD dwDrawAspect, LPSIZEL lpsizel);

	STDMETHOD(Advise)(LPADVISESINK pAdvSink, DWORD FAR* pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise) (LPENUMSTATDATA FAR* ppenumAdvise);
	STDMETHOD(GetMiscStatus) (DWORD dwAspect, DWORD FAR* pdwStatus);
	STDMETHOD(SetColorScheme) (LPLOGPALETTE lpLogpal);

	inline COleObjectW(LPUNKNOWN pUnk, IOleObjectA * pANSI) :
			CWideInterface(ID_IOleObject, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleObjectA * GetANSI() const
		{ return (IOleObjectA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleClientSiteW
//
//  Synopsis:   Class definition of IOleClientSiteW
//
//---------------------------------------------------------------------------
class COleClientSiteW : CWideInterface
{
public:
	// *** IOleClientSite methods ***
	STDMETHOD(SaveObject) (VOID);
	STDMETHOD(GetMoniker) (DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKER FAR* ppmk);
	STDMETHOD(GetContainer) (LPOLECONTAINER FAR* ppContainer);
	STDMETHOD(ShowObject) (VOID);
	STDMETHOD(OnShowWindow) (BOOL fShow);
	STDMETHOD(RequestNewObjectLayout) (VOID);

	inline COleClientSiteW(LPUNKNOWN pUnk, IOleClientSiteA * pANSI) :
			CWideInterface(ID_IOleClientSite, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleClientSiteA * GetANSI() const
		{ return (IOleClientSiteA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CRunnableObjectW
//
//  Synopsis:   Class definition of IRunnableObjectW
//
//---------------------------------------------------------------------------
class CRunnableObjectW : CWideInterface
{
public:
	// *** IRunnableObject methods ***
	STDMETHOD(GetRunningClass) (LPCLSID lpClsid);
	STDMETHOD(Run) (LPBINDCTX pbc);
	STDMETHOD_(BOOL, IsRunning) (VOID);
	STDMETHOD(LockRunning)(BOOL fLock, BOOL fLastUnlockCloses);
	STDMETHOD(SetContainedObject)(BOOL fContained);

	inline CRunnableObjectW(LPUNKNOWN pUnk, IRunnableObjectA * pANSI) :
			CWideInterface(ID_IRunnableObject, pUnk, (LPUNKNOWN)pANSI) {};

	inline IRunnableObjectA * GetANSI() const
		{ return (IRunnableObjectA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleItemContainerW
//
//  Synopsis:   Class definition of IOleItemContainerW
//
//---------------------------------------------------------------------------
class COleItemContainerW : CWideInterface
{
public:
	// *** IParseDisplayName method ***
	STDMETHOD(ParseDisplayName) (LPBC pbc, LPOLESTR lpszDisplayName,
		ULONG FAR* pchEaten, LPMONIKER FAR* ppmkOut);

	// *** IOleContainer methods ***
	STDMETHOD(EnumObjects) (DWORD grfFlags, LPENUMUNKNOWN FAR* ppenumUnknown);
	STDMETHOD(LockContainer) (BOOL fLock);

	// *** IOleItemContainer methods ***
	STDMETHOD(GetObject) (LPOLESTR lpszItem, DWORD dwSpeedNeeded,
		LPBINDCTX pbc, REFIID riid, LPVOID FAR* ppvObject);
	STDMETHOD(GetObjectStorage) (LPOLESTR lpszItem, LPBINDCTX pbc,
		REFIID riid, LPVOID FAR* ppvStorage);
	STDMETHOD(IsRunning) (LPOLESTR lpszItem);

	inline COleItemContainerW(LPUNKNOWN pUnk, IOleItemContainerA * pANSI) :
			CWideInterface(ID_IOleItemContainer, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleItemContainerA * GetANSI() const
		{ return (IOleItemContainerA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleAdviseHolderW
//
//  Synopsis:   Class definition of IOleAdviseHolderW
//
//---------------------------------------------------------------------------
class COleAdviseHolderW : CWideInterface
{
public:
	// *** IOleAdviseHolder methods ***
	STDMETHOD(Advise)(LPADVISESINK pAdvise, DWORD FAR* pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise)(LPENUMSTATDATA FAR* ppenumAdvise);

	STDMETHOD(SendOnRename)(LPMONIKER pmk);
	STDMETHOD(SendOnSave)(VOID);
	STDMETHOD(SendOnClose)(VOID);

	inline COleAdviseHolderW(LPUNKNOWN pUnk, IOleAdviseHolderA * pANSI) :
			CWideInterface(ID_IOleAdviseHolder, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleAdviseHolderA * GetANSI() const
		{ return (IOleAdviseHolderA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleLinkW
//
//  Synopsis:   Class definition of IOleLinkW
//
//---------------------------------------------------------------------------
class COleLinkW : CWideInterface
{
public:
	// *** IOleLink methods ***
	STDMETHOD(SetUpdateOptions) (DWORD dwUpdateOpt);
	STDMETHOD(GetUpdateOptions) (LPDWORD pdwUpdateOpt);
	STDMETHOD(SetSourceMoniker) (LPMONIKER pmk, REFCLSID rclsid);
	STDMETHOD(GetSourceMoniker) (LPMONIKER FAR* ppmk);
	STDMETHOD(SetSourceDisplayName) (LPCOLESTR lpszDisplayName);
	STDMETHOD(GetSourceDisplayName) (LPOLESTR FAR* lplpszDisplayName);
	STDMETHOD(BindToSource) (DWORD bindflags, LPBINDCTX pbc);
	STDMETHOD(BindIfRunning) (VOID);
	STDMETHOD(GetBoundSource) (LPUNKNOWN FAR* ppUnk);
	STDMETHOD(UnbindSource) (VOID);
	STDMETHOD(Update) (LPBINDCTX pbc);

	inline COleLinkW(LPUNKNOWN pUnk, IOleLinkA * pANSI) :
			CWideInterface(ID_IOleLink, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleLinkA * GetANSI() const
		{ return (IOleLinkA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceObjectW
//
//  Synopsis:   Class definition of IOleInPlaceObjectW
//
//---------------------------------------------------------------------------
class COleInPlaceObjectW : CWideInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND FAR* lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceObject methods ***
	STDMETHOD(InPlaceDeactivate) (VOID);
	STDMETHOD(UIDeactivate) (VOID);
	STDMETHOD(SetObjectRects) (LPCRECT lprcPosRect, LPCRECT lprcClipRect);
	STDMETHOD(ReactivateAndUndo) (VOID);

	inline COleInPlaceObjectW(LPUNKNOWN pUnk, IOleInPlaceObjectA * pANSI) :
			CWideInterface(ID_IOleInPlaceObject, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleInPlaceObjectA * GetANSI() const
		{ return (IOleInPlaceObjectA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceActiveObjectW
//
//  Synopsis:   Class definition of IOleInPlaceActiveObjectW
//
//---------------------------------------------------------------------------
class COleInPlaceActiveObjectW : CWideInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND FAR* lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceActiveObject methods ***
	STDMETHOD(TranslateAccelerator) (LPMSG lpmsg);
	STDMETHOD(OnFrameWindowActivate) (BOOL fActivate);
	STDMETHOD(OnDocWindowActivate) (BOOL fActivate);
	STDMETHOD(ResizeBorder) (LPCRECT lprectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, BOOL fFrameWindow);
	STDMETHOD(EnableModeless) (BOOL fEnable);

	inline COleInPlaceActiveObjectW(LPUNKNOWN pUnk, IOleInPlaceActiveObjectA * pANSI) :
			CWideInterface(ID_IOleInPlaceActiveObject, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleInPlaceActiveObjectA * GetANSI() const
		{ return (IOleInPlaceActiveObjectA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceFrameW
//
//  Synopsis:   Class definition of IOleInPlaceFrameW
//
//---------------------------------------------------------------------------
class COleInPlaceFrameW : CWideInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND FAR* lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceUIWindow methods ***
	STDMETHOD(GetBorder) (LPRECT lprectBorder);
	STDMETHOD(RequestBorderSpace) (LPCBORDERWIDTHS lpborderwidths);
	STDMETHOD(SetBorderSpace) (LPCBORDERWIDTHS lpborderwidths);
	STDMETHOD(SetActiveObject) (LPOLEINPLACEACTIVEOBJECT lpActiveObject,
					LPCOLESTR lpszObjName);


	// *** IOleInPlaceFrame methods ***
	STDMETHOD(InsertMenus) (HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	STDMETHOD(SetMenu) (HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
	STDMETHOD(RemoveMenus) (HMENU hmenuShared);
	STDMETHOD(SetStatusText) (LPCOLESTR lpszStatusText);
	STDMETHOD(EnableModeless) (BOOL fEnable);
	STDMETHOD(TranslateAccelerator) (LPMSG lpmsg, WORD wID);

	inline COleInPlaceFrameW(LPUNKNOWN pUnk, IOleInPlaceFrameA * pANSI) :
			CWideInterface(ID_IOleInPlaceFrame, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleInPlaceFrameA * GetANSI() const
		{ return (IOleInPlaceFrameA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceSiteW
//
//  Synopsis:   Class definition of IOleInPlaceSiteW
//
//---------------------------------------------------------------------------
class COleInPlaceSiteW : CWideInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND FAR* lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceSite methods ***
	STDMETHOD(CanInPlaceActivate) (VOID);
	STDMETHOD(OnInPlaceActivate) (VOID);
	STDMETHOD(OnUIActivate) (VOID);
	STDMETHOD(GetWindowContext) (LPOLEINPLACEFRAME FAR* lplpFrame,
						LPOLEINPLACEUIWINDOW FAR* lplpDoc,
						LPRECT lprcPosRect,
						LPRECT lprcClipRect,
						LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll) (SIZE scrollExtent);
	STDMETHOD(OnUIDeactivate) (BOOL fUndoable);
	STDMETHOD(OnInPlaceDeactivate) (VOID);
	STDMETHOD(DiscardUndoState) (VOID);
	STDMETHOD(DeactivateAndUndo) (VOID);
	STDMETHOD(OnPosRectChange) (LPCRECT lprcPosRect);

	inline COleInPlaceSiteW(LPUNKNOWN pUnk, IOleInPlaceSiteA * pANSI) :
			CWideInterface(ID_IOleInPlaceSite, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleInPlaceSiteA * GetANSI() const
		{ return (IOleInPlaceSiteA *)m_pObj; };
};
