//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansiole1.h
//
//  Contents:   ANSI Wrappers for Unicode Ole2 Interfaces and APIs.
//
//  Classes:    CDropTargetA
//              CPersistStorageA
//              CPersistStreamA
//              CPersistFileA
//              CEnumOLEVERBA
//              COleObjectA
//              COleClientSiteA
//              CRunnableObjectA
//              COleItemCOntainerA
//              COleAdviseHolderA
//              COleLinkA
//              COleInPlaceObjectA
//              COleInPlaceActiveObjectA
//              COleInPlaceFrameA
//              COleInPlaceSiteA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------



#if defined(__cplusplus)
interface IOleClientSiteA;
interface IOleItemContainerA;
interface IOleInPlaceFrameA;
#else
typedef interface IOleClientSiteA IOleClientSiteA;
typedef interface IOleItemContainerA IOleItemContainerA;
typedef interface IOleInPlaceFrameA IOleInPlaceFrameA;
#endif

typedef IOleClientSiteA * LPOLECLIENTSITEA;
typedef IOleItemContainerA * LPOLEITEMCONTAINERA;
typedef IOleInPlaceFrameA * LPOLEINPLACEFRAMEA;

#define LPPARSEDISPLAYNAMEA     LPOLEITEMCONTAINERA
#define LPOLECONTAINERA         LPOLEITEMCONTAINERA

#define LPOLEWINDOWA            LPOLEINPLACEFRAMEA
#define LPOLEINPLACEUIWINDOWA   LPOLEINPLACEFRAMEA

#define LPPERSISTSTREAMA        LPMONIKERA




/*---------------------------------------------------------------------*/
/*                         IEnumOLEVERBA                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumOLEVERBA

DECLARE_INTERFACE_(IEnumOLEVERBA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IEnumOLEVERB methods ***
	STDMETHOD(Next) (THIS_ ULONG celt, LPOLEVERBA rgelt, ULONG * pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumOLEVERBA * * ppenm) PURE;
};
typedef IEnumOLEVERBA * LPENUMOLEVERBA;


/*---------------------------------------------------------------------*/
/*                         IOleObjectA                                 */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleObjectA

DECLARE_INTERFACE_(IOleObjectA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleObject methods ***
	STDMETHOD(SetClientSite) (THIS_ LPOLECLIENTSITEA pClientSiteA) PURE;
	STDMETHOD(GetClientSite) (THIS_ LPOLECLIENTSITEA * ppCLientSiteA) PURE;
	STDMETHOD(SetHostNames) (THIS_ LPCSTR szContainerApp, LPCSTR szContainerObj) PURE;
	STDMETHOD(Close) (THIS_ DWORD dwSaveOption) PURE;
	STDMETHOD(SetMoniker) (THIS_ DWORD dwWhichMoniker, LPMONIKERA pmkA) PURE;
	STDMETHOD(GetMoniker) (THIS_ DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKERA * ppmkA) PURE;
	STDMETHOD(InitFromData) (THIS_ LPDATAOBJECTA pDataObjectA,
				BOOL fCreation,
				DWORD dwReserved) PURE;
	STDMETHOD(GetClipboardData) (THIS_ DWORD dwReserved,
				LPDATAOBJECTA * ppDataObjectA) PURE;
	STDMETHOD(DoVerb) (THIS_ LONG iVerb,
				LPMSG lpmsg,
				LPOLECLIENTSITEA pActiveSiteA,
				LONG lindex,
				HWND hwndParent,
				LPCRECT lprcPosRect) PURE;
	STDMETHOD(EnumVerbs) (THIS_ LPENUMOLEVERBA * ppenumOleVerbA) PURE;
	STDMETHOD(Update) (THIS) PURE;
	STDMETHOD(IsUpToDate) (THIS) PURE;
	STDMETHOD(GetUserClassID) (THIS_ CLSID * pClsid) PURE;
	STDMETHOD(GetUserType) (THIS_ DWORD dwFormOfType, LPSTR * pszUserType) PURE;
	STDMETHOD(SetExtent) (THIS_ DWORD dwDrawAspect, LPSIZEL lpsizel) PURE;
	STDMETHOD(GetExtent) (THIS_ DWORD dwDrawAspect, LPSIZEL lpsizel) PURE;

	STDMETHOD(Advise)(THIS_ LPADVISESINKA pAdvSinkA, DWORD * pdwConnection) PURE;
	STDMETHOD(Unadvise)(THIS_ DWORD dwConnection) PURE;
	STDMETHOD(EnumAdvise) (THIS_ LPENUMSTATDATAA * ppenumAdviseA) PURE;
	STDMETHOD(GetMiscStatus) (THIS_ DWORD dwAspect, DWORD * pdwStatus) PURE;
	STDMETHOD(SetColorScheme) (THIS_ LPLOGPALETTE lpLogpal) PURE;
};
typedef IOleObjectA * LPOLEOBJECTA;


/*---------------------------------------------------------------------*/
/*                         IOleClientSiteA                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleClientSiteA

DECLARE_INTERFACE_(IOleClientSiteA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleClientSite methods ***
	STDMETHOD(SaveObject) (THIS) PURE;
	STDMETHOD(GetMoniker) (THIS_ DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKERA * ppmkA) PURE;
	STDMETHOD(GetContainer) (THIS_ LPOLECONTAINERA * ppContainerA) PURE;
	STDMETHOD(ShowObject) (THIS) PURE;
	STDMETHOD(OnShowWindow) (THIS_ BOOL fShow) PURE;
	STDMETHOD(RequestNewObjectLayout) (THIS) PURE;
};
typedef IOleClientSiteA * LPOLECLIENTSITEA;


/*---------------------------------------------------------------------*/
/*                         IRunnableObjectA                            */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IRunnableObjectA

DECLARE_INTERFACE_(IRunnableObjectA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IRunnableObject methods ***
	STDMETHOD(GetRunningClass) (THIS_ LPCLSID lpClsid) PURE;
	STDMETHOD(Run) (THIS_ LPBINDCTXA pbcA) PURE;
	STDMETHOD_(BOOL, IsRunning) (THIS) PURE;
	STDMETHOD(LockRunning)(THIS_ BOOL fLock, BOOL fLastUnlockCloses) PURE;
	STDMETHOD(SetContainedObject)(THIS_ BOOL fContained) PURE;
};
typedef IRunnableObjectA * LPRUNNABLEOBJECTA;


/*---------------------------------------------------------------------*/
/*                         IOleItemContainerA                          */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleItemContainerA

DECLARE_INTERFACE_(IOleItemContainerA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IParseDisplayName method ***
	STDMETHOD(ParseDisplayName) (THIS_ LPBCA pbcA, LPSTR lpszDisplayName,
		ULONG * pchEaten, LPMONIKERA * ppmkOutA) PURE;

	// *** IOleContainer methods ***
	STDMETHOD(EnumObjects) (THIS_ DWORD grfFlags, LPENUMUNKNOWN * ppenumUnknown) PURE;
	STDMETHOD(LockContainer) (THIS_ BOOL fLock) PURE;

	// *** IOleItemContainer methods ***
	STDMETHOD(GetObject) (THIS_ LPSTR lpszItem, DWORD dwSpeedNeeded,
		LPBINDCTXA pbcA, REFIID riid, LPVOID * ppvObject) PURE;
	STDMETHOD(GetObjectStorage) (THIS_ LPSTR lpszItem, LPBINDCTXA pbcA,
		REFIID riid, LPVOID * ppvStorage) PURE;
	STDMETHOD(IsRunning) (THIS_ LPSTR lpszItem) PURE;
};
typedef IOleItemContainerA * LPOLEITEMCONTAINERA;


/*---------------------------------------------------------------------*/
/*                         IOleAdviseHolderA                           */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleAdviseHolderA

DECLARE_INTERFACE_(IOleAdviseHolderA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppv) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleAdviseHolder methods ***
	STDMETHOD(Advise)(THIS_ LPADVISESINKA pAdviseA, DWORD * pdwConnection) PURE;
	STDMETHOD(Unadvise)(THIS_ DWORD dwConnection) PURE;
	STDMETHOD(EnumAdvise)(THIS_ LPENUMSTATDATAA * ppenumAdviseA) PURE;

	STDMETHOD(SendOnRename)(THIS_ LPMONIKERA pmkA) PURE;
	STDMETHOD(SendOnSave)(THIS) PURE;
	STDMETHOD(SendOnClose)(THIS) PURE;
};
typedef IOleAdviseHolderA * LPOLEADVISEHOLDERA;


/*---------------------------------------------------------------------*/
/*                            IOleLinkA                                */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleLinkA

DECLARE_INTERFACE_(IOleLinkA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleLink methods ***
	STDMETHOD(SetUpdateOptions) (THIS_ DWORD dwUpdateOpt) PURE;
	STDMETHOD(GetUpdateOptions) (THIS_ LPDWORD pdwUpdateOpt) PURE;
	STDMETHOD(SetSourceMoniker) (THIS_ LPMONIKERA pmkA, REFCLSID rclsid) PURE;
	STDMETHOD(GetSourceMoniker) (THIS_ LPMONIKERA * ppmkA) PURE;
	STDMETHOD(SetSourceDisplayName) (THIS_ LPCSTR lpszDisplayName) PURE;
	STDMETHOD(GetSourceDisplayName) (THIS_ LPSTR * lplpszDisplayName) PURE;
	STDMETHOD(BindToSource) (THIS_ DWORD bindflags, LPBINDCTXA pbcA) PURE;
	STDMETHOD(BindIfRunning) (THIS) PURE;
	STDMETHOD(GetBoundSource) (THIS_ LPUNKNOWN * ppUnk) PURE;
	STDMETHOD(UnbindSource) (THIS) PURE;
	STDMETHOD(Update) (THIS_ LPBINDCTXA pbcA) PURE;
};
typedef IOleLinkA * LPOLELINKA;


/*---------------------------------------------------------------------*/
/*                        IOleInPlaceObjectA                           */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleInPlaceObjectA

DECLARE_INTERFACE_(IOleInPlaceObjectA, IOleWindow)
{
   // *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
	STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

	// *** IOleInPlaceObject methods ***
	STDMETHOD(InPlaceDeactivate) (THIS) PURE;
	STDMETHOD(UIDeactivate) (THIS) PURE;
	STDMETHOD(SetObjectRects) (THIS_ LPCRECT lprcPosRect, LPCRECT lprcClipRect) PURE;
	STDMETHOD(ReactivateAndUndo) (THIS) PURE;

};
typedef IOleInPlaceObjectA * LPOLEINPLACEOBJECTA;


/*---------------------------------------------------------------------*/
/*                        IOleInPlaceActiveObjectA                     */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleInPlaceActiveObjectA

DECLARE_INTERFACE_(IOleInPlaceActiveObjectA, IOleWindow)
{
   // *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
	STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

	// *** IOleInPlaceActiveObject methods ***
	STDMETHOD(TranslateAccelerator) (THIS_ LPMSG lpmsg) PURE;
	STDMETHOD(OnFrameWindowActivate) (THIS_ BOOL fActivate) PURE;
	STDMETHOD(OnDocWindowActivate) (THIS_ BOOL fActivate) PURE;
	STDMETHOD(ResizeBorder) (THIS_ LPCRECT lprectBorder, LPOLEINPLACEUIWINDOWA lpUIWindowA, BOOL fFrameWindow) PURE;
	STDMETHOD(EnableModeless) (THIS_ BOOL fEnable) PURE;

};
typedef IOleInPlaceActiveObjectA * LPOLEINPLACEACTIVEOBJECTA;


/*---------------------------------------------------------------------*/
/*                         IOleInPlaceFrameA                           */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleInPlaceFrameA

DECLARE_INTERFACE_(IOleInPlaceFrameA, IOleWindow)
{
   // *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
	STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

	// *** IOleInPlaceUIWindow methods ***
	STDMETHOD(GetBorder) (THIS_ LPRECT lprectBorder) PURE;
	STDMETHOD(RequestBorderSpace) (THIS_ LPCBORDERWIDTHS lpborderwidths) PURE;
	STDMETHOD(SetBorderSpace) (THIS_ LPCBORDERWIDTHS lpborderwidths) PURE;
	STDMETHOD(SetActiveObject) (THIS_ LPOLEINPLACEACTIVEOBJECTA lpActiveObjectA,
					LPCSTR lpszObjName) PURE;


	// *** IOleInPlaceFrame methods ***
	STDMETHOD(InsertMenus) (THIS_ HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) PURE;
	STDMETHOD(SetMenu) (THIS_ HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) PURE;
	STDMETHOD(RemoveMenus) (THIS_ HMENU hmenuShared) PURE;
	STDMETHOD(SetStatusText) (THIS_ LPCSTR lpszStatusText) PURE;
	STDMETHOD(EnableModeless) (THIS_ BOOL fEnable) PURE;
	STDMETHOD(TranslateAccelerator) (THIS_ LPMSG lpmsg, WORD wID) PURE;
};
typedef IOleInPlaceFrameA * LPOLEINPLACEFRAMEA;


/*---------------------------------------------------------------------*/
/*                         IOleInPlaceSiteA                            */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IOleInPlaceSiteA

DECLARE_INTERFACE_(IOleInPlaceSiteA, IOleWindow)
{
   // *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) PURE;
	STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) PURE;

	// *** IOleInPlaceSite methods ***
	STDMETHOD(CanInPlaceActivate) (THIS) PURE;
	STDMETHOD(OnInPlaceActivate) (THIS) PURE;
	STDMETHOD(OnUIActivate) (THIS) PURE;
	STDMETHOD(GetWindowContext) (THIS_ LPOLEINPLACEFRAMEA * lplpFrameA,
						LPOLEINPLACEUIWINDOWA * lplpDocA,
						LPRECT lprcPosRect,
						LPRECT lprcClipRect,
						LPOLEINPLACEFRAMEINFO lpFrameInfo) PURE;
	STDMETHOD(Scroll) (THIS_ SIZE scrollExtent) PURE;
	STDMETHOD(OnUIDeactivate) (THIS_ BOOL fUndoable) PURE;
	STDMETHOD(OnInPlaceDeactivate) (THIS) PURE;
	STDMETHOD(DiscardUndoState) (THIS) PURE;
	STDMETHOD(DeactivateAndUndo) (THIS) PURE;
	STDMETHOD(OnPosRectChange) (THIS_ LPCRECT lprcPosRect) PURE;
};
typedef IOleInPlaceSiteA * LPOLEINPLACESITEA;



//
//  Forward declarations
//
class CDropTargetA;
class CPersistStorageA;
class CPersistFileA;
class CEnumOLEVERBA;
class COleObjectA;
class COleClientSiteA;
class CRunnableObjectA;
class COleItemContainerA;
class COleAdviseHolderA;
class COleLinkA;
class COleInPlaceObjectA;
class COleInPlaceActiveObjectA;
class COleInPlaceFrameA;
class COleInPlaceSiteA;



//+--------------------------------------------------------------------------
//
//  Class:      CDropTargetA
//
//  Synopsis:   Class definition of IDropTargetA
//
//---------------------------------------------------------------------------
class CDropTargetA : CAnsiInterface
{
public:
	// *** IDropTarget methods ***
	STDMETHOD(DragEnter) (LPDATAOBJECTA pDataObjA, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragOver) (DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);
	STDMETHOD(DragLeave) (VOID);
	STDMETHOD(Drop) (LPDATAOBJECTA pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect);

	inline CDropTargetA(LPUNKNOWN pUnk, IDropTarget * pWide) :
			CAnsiInterface(ID_IDropTarget, pUnk, (LPUNKNOWN)pWide) {};

	inline IDropTarget * GetWide() const
			{ return (IDropTarget *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CPersistStorageA
//
//  Synopsis:   Class definition of IPersistStorageA
//
//---------------------------------------------------------------------------
class CPersistStorageA : CAnsiInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistStorage methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(InitNew) (LPSTORAGEA pStgA);
	STDMETHOD(Load) (LPSTORAGEA pStgA);
	STDMETHOD(Save) (LPSTORAGEA pStgSaveA, BOOL fSameAsLoad);
	STDMETHOD(SaveCompleted) (LPSTORAGEA pStgNewA);
	STDMETHOD(HandsOffStorage) (VOID);

	inline CPersistStorageA(LPUNKNOWN pUnk, IPersistStorage * pWide) :
			CAnsiInterface(ID_IPersistStorage, pUnk, (LPUNKNOWN)pWide) {};

	inline IPersistStorage * GetWide() const
			{ return (IPersistStorage *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CPersistFileA
//
//  Synopsis:   Class definition of IPersistFileA
//
//---------------------------------------------------------------------------
class CPersistFileA : CAnsiInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistFile methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(Load) (LPCSTR lpszFileName, DWORD grfMode);
	STDMETHOD(Save) (LPCSTR lpszFileName, BOOL fRemember);
	STDMETHOD(SaveCompleted) (LPCSTR lpszFileName);
	STDMETHOD(GetCurFile) (LPSTR FAR* lplpszFileName);

	inline CPersistFileA(LPUNKNOWN pUnk, IPersistFile * pWide) :
			CAnsiInterface(ID_IPersistFile, pUnk, (LPUNKNOWN)pWide) {};

	inline IPersistFile * GetWide() const
			{ return (IPersistFile *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumOLEVERBA
//
//  Synopsis:   Class definition of IEnumOLEVERBA
//
//---------------------------------------------------------------------------
class CEnumOLEVERBA : CAnsiInterface
{
public:
	// *** IEnumOLEVERB methods ***
	STDMETHOD(Next) (ULONG celt, LPOLEVERBA rgelt, ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumOLEVERBA * * ppenm);

	inline CEnumOLEVERBA(LPUNKNOWN pUnk, IEnumOLEVERB * pWide) :
			CAnsiInterface(ID_IEnumOLEVERB, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumOLEVERB * GetWide() const
			{ return (IEnumOLEVERB *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleObjectA
//
//  Synopsis:   Class definition of IOleObjectA
//
//---------------------------------------------------------------------------
class COleObjectA : CAnsiInterface
{
public:
	// *** IOleObject methods ***
	STDMETHOD(SetClientSite) (LPOLECLIENTSITEA pClientSiteA);
	STDMETHOD(GetClientSite) (LPOLECLIENTSITEA * ppCLientSiteA);
	STDMETHOD(SetHostNames) (LPCSTR szContainerApp, LPCSTR szContainerObj);
	STDMETHOD(Close) (DWORD dwSaveOption);
	STDMETHOD(SetMoniker) (DWORD dwWhichMoniker, LPMONIKERA pmkA);
	STDMETHOD(GetMoniker) (DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKERA * ppmkA);
	STDMETHOD(InitFromData) (LPDATAOBJECTA pDataObjectA,
				BOOL fCreation,
				DWORD dwReserved);
	STDMETHOD(GetClipboardData) (DWORD dwReserved,
				LPDATAOBJECTA * ppDataObjectA);
	STDMETHOD(DoVerb) (LONG iVerb,
				LPMSG lpmsg,
				LPOLECLIENTSITEA pActiveSiteA,
				LONG lindex,
				HWND hwndParent,
				LPCRECT lprcPosRect);
	STDMETHOD(EnumVerbs) (LPENUMOLEVERBA * ppenumOleVerbA);
	STDMETHOD(Update) (VOID);
	STDMETHOD(IsUpToDate) (VOID);
	STDMETHOD(GetUserClassID) (CLSID * pClsid);
	STDMETHOD(GetUserType) (DWORD dwFormOfType, LPSTR * pszUserType);
	STDMETHOD(SetExtent) (DWORD dwDrawAspect, LPSIZEL lpsizel);
	STDMETHOD(GetExtent) (DWORD dwDrawAspect, LPSIZEL lpsizel);

	STDMETHOD(Advise)(LPADVISESINKA pAdvSinkA, DWORD * pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise) (LPENUMSTATDATAA * ppenumAdviseA);
	STDMETHOD(GetMiscStatus) (DWORD dwAspect, DWORD * pdwStatus);
	STDMETHOD(SetColorScheme) (LPLOGPALETTE lpLogpal);

	inline COleObjectA(LPUNKNOWN pUnk, IOleObject * pWide) :
			CAnsiInterface(ID_IOleObject, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleObject * GetWide() const
			{ return (IOleObject *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleClientSiteA
//
//  Synopsis:   Class definition of IOleClientSiteA
//
//---------------------------------------------------------------------------
class COleClientSiteA : CAnsiInterface
{
public:
	// *** IOleClientSite methods ***
	STDMETHOD(SaveObject) (VOID);
	STDMETHOD(GetMoniker) (DWORD dwAssign, DWORD dwWhichMoniker,
				LPMONIKERA * ppmkA);
	STDMETHOD(GetContainer) (LPOLECONTAINERA * ppContainerA);
	STDMETHOD(ShowObject) (VOID);
	STDMETHOD(OnShowWindow) (BOOL fShow);
	STDMETHOD(RequestNewObjectLayout) (VOID);

	inline COleClientSiteA(LPUNKNOWN pUnk, IOleClientSite * pWide) :
			CAnsiInterface(ID_IOleClientSite, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleClientSite * GetWide() const
			{ return (IOleClientSite *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CRunnableObjectA
//
//  Synopsis:   Class definition of IRunnableObjectA
//
//---------------------------------------------------------------------------
class CRunnableObjectA : CAnsiInterface
{
public:
	// *** IRunnableObject methods ***
	STDMETHOD(GetRunningClass) (LPCLSID lpClsid);
	STDMETHOD(Run) (LPBINDCTXA pbcA);
	STDMETHOD_(BOOL, IsRunning) (VOID);
	STDMETHOD(LockRunning)(BOOL fLock, BOOL fLastUnlockCloses);
	STDMETHOD(SetContainedObject)(BOOL fContained);

	inline CRunnableObjectA(LPUNKNOWN pUnk, IRunnableObject * pWide) :
			CAnsiInterface(ID_IRunnableObject, pUnk, (LPUNKNOWN)pWide) {};

	inline IRunnableObject * GetWide() const
			{ return (IRunnableObject *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleItemContainerA
//
//  Synopsis:   Class definition of IOleItemContainerA
//
//---------------------------------------------------------------------------
class COleItemContainerA : CAnsiInterface
{
public:
	// *** IParseDisplayName method ***
	STDMETHOD(ParseDisplayName) (LPBCA pbcA, LPSTR lpszDisplayName,
		ULONG * pchEaten, LPMONIKERA * ppmkAOut);

	// *** IOleContainer methods ***
	STDMETHOD(EnumObjects) (DWORD grfFlags, LPENUMUNKNOWN * ppenumUnknown);
	STDMETHOD(LockContainer) (BOOL fLock);

	// *** IOleItemContainer methods ***
	STDMETHOD(GetObject) (LPSTR lpszItem, DWORD dwSpeedNeeded,
		LPBINDCTXA pbcA, REFIID riid, LPVOID * ppvObject);
	STDMETHOD(GetObjectStorage) (LPSTR lpszItem, LPBINDCTXA pbcA,
		REFIID riid, LPVOID * ppvStorage);
	STDMETHOD(IsRunning) (LPSTR lpszItem);

	inline COleItemContainerA(LPUNKNOWN pUnk, IOleItemContainer * pWide) :
			CAnsiInterface(ID_IOleItemContainer, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleItemContainer * GetWide() const
			{ return (IOleItemContainer *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleAdviseHolderA
//
//  Synopsis:   Class definition of IOleAdviseHolderA
//
//---------------------------------------------------------------------------
class COleAdviseHolderA : CAnsiInterface
{
public:
	// *** IOleAdviseHolder methods ***
	STDMETHOD(Advise)(LPADVISESINKA pAdviseA, DWORD * pdwConnection);
	STDMETHOD(Unadvise)(DWORD dwConnection);
	STDMETHOD(EnumAdvise)(LPENUMSTATDATAA * ppenumAdviseA);

	STDMETHOD(SendOnRename)(LPMONIKERA pmkA);
	STDMETHOD(SendOnSave)(VOID);
	STDMETHOD(SendOnClose)(VOID);

	inline COleAdviseHolderA(LPUNKNOWN pUnk, IOleAdviseHolder * pWide) :
			CAnsiInterface(ID_IOleAdviseHolder, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleAdviseHolder * GetWide() const
			{ return (IOleAdviseHolder *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleLinkA
//
//  Synopsis:   Class definition of IOleLinkA
//
//---------------------------------------------------------------------------
class COleLinkA : CAnsiInterface
{
public:
	// *** IOleLink methods ***
	STDMETHOD(SetUpdateOptions) (DWORD dwUpdateOpt);
	STDMETHOD(GetUpdateOptions) (LPDWORD pdwUpdateOpt);
	STDMETHOD(SetSourceMoniker) (LPMONIKERA pmkA, REFCLSID rclsid);
	STDMETHOD(GetSourceMoniker) (LPMONIKERA * ppmkA);
	STDMETHOD(SetSourceDisplayName) (LPCSTR lpszDisplayName);
	STDMETHOD(GetSourceDisplayName) (LPSTR * lplpszDisplayName);
	STDMETHOD(BindToSource) (DWORD bindflags, LPBINDCTXA pbcA);
	STDMETHOD(BindIfRunning) (VOID);
	STDMETHOD(GetBoundSource) (LPUNKNOWN * ppUnk);
	STDMETHOD(UnbindSource) (VOID);
	STDMETHOD(Update) (LPBINDCTXA pbcA);

	inline COleLinkA(LPUNKNOWN pUnk, IOleLink * pWide) :
			CAnsiInterface(ID_IOleLink, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleLink * GetWide() const
			{ return (IOleLink *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceObjectA
//
//  Synopsis:   Class definition of IOleInPlaceObjectA
//
//---------------------------------------------------------------------------
class COleInPlaceObjectA : CAnsiInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND * lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceObject methods ***
	STDMETHOD(InPlaceDeactivate) (VOID);
	STDMETHOD(UIDeactivate) (VOID);
	STDMETHOD(SetObjectRects) (LPCRECT lprcPosRect, LPCRECT lprcClipRect);
	STDMETHOD(ReactivateAndUndo) (VOID);

	inline COleInPlaceObjectA(LPUNKNOWN pUnk, IOleInPlaceObject * pWide) :
			CAnsiInterface(ID_IOleInPlaceObject, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleInPlaceObject * GetWide() const
			{ return (IOleInPlaceObject *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceActiveObjectA
//
//  Synopsis:   Class definition of IOleInPlaceActiveObjectA
//
//---------------------------------------------------------------------------
class COleInPlaceActiveObjectA : CAnsiInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND * lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceActiveObject methods ***
	STDMETHOD(TranslateAccelerator) (LPMSG lpmsg);
	STDMETHOD(OnFrameWindowActivate) (BOOL fActivate);
	STDMETHOD(OnDocWindowActivate) (BOOL fActivate);
	STDMETHOD(ResizeBorder) (LPCRECT lprectBorder, LPOLEINPLACEUIWINDOWA lpUIWindowA, BOOL fFrameWindow);
	STDMETHOD(EnableModeless) (BOOL fEnable);

	inline COleInPlaceActiveObjectA(LPUNKNOWN pUnk, IOleInPlaceActiveObject * pWide) :
			CAnsiInterface(ID_IOleInPlaceActiveObject, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleInPlaceActiveObject * GetWide() const
			{ return (IOleInPlaceActiveObject *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceFrameA
//
//  Synopsis:   Class definition of IOleInPlaceFrameA
//
//---------------------------------------------------------------------------
class COleInPlaceFrameA : CAnsiInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND * lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceUIWindow methods ***
	STDMETHOD(GetBorder) (LPRECT lprectBorder);
	STDMETHOD(RequestBorderSpace) (LPCBORDERWIDTHS lpborderwidths);
	STDMETHOD(SetBorderSpace) (LPCBORDERWIDTHS lpborderwidths);
	STDMETHOD(SetActiveObject) (LPOLEINPLACEACTIVEOBJECTA lpActiveObjectA,
					LPCSTR lpszObjName);

	// *** IOleInPlaceFrame methods ***
	STDMETHOD(InsertMenus) (HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
	STDMETHOD(SetMenu) (HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
	STDMETHOD(RemoveMenus) (HMENU hmenuShared);
	STDMETHOD(SetStatusText) (LPCSTR lpszStatusText);
	STDMETHOD(EnableModeless) (BOOL fEnable);
	STDMETHOD(TranslateAccelerator) (LPMSG lpmsg, WORD wID);

	inline COleInPlaceFrameA(LPUNKNOWN pUnk, IOleInPlaceFrame * pWide) :
			CAnsiInterface(ID_IOleInPlaceFrame, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleInPlaceFrame * GetWide() const
			{ return (IOleInPlaceFrame *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      COleInPlaceSiteA
//
//  Synopsis:   Class definition of IOleInPlaceSiteA
//
//---------------------------------------------------------------------------
class COleInPlaceSiteA : CAnsiInterface
{
public:
	// *** IOleWindow methods ***
	STDMETHOD(GetWindow) (HWND * lphwnd);
	STDMETHOD(ContextSensitiveHelp) (BOOL fEnterMode);

	// *** IOleInPlaceSite methods ***
	STDMETHOD(CanInPlaceActivate) (VOID);
	STDMETHOD(OnInPlaceActivate) (VOID);
	STDMETHOD(OnUIActivate) (VOID);
	STDMETHOD(GetWindowContext) (LPOLEINPLACEFRAMEA * lplpFrameA,
						LPOLEINPLACEUIWINDOWA * lplpDocA,
						LPRECT lprcPosRect,
						LPRECT lprcClipRect,
						LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll) (SIZE scrollExtent);
	STDMETHOD(OnUIDeactivate) (BOOL fUndoable);
	STDMETHOD(OnInPlaceDeactivate) (VOID);
	STDMETHOD(DiscardUndoState) (VOID);
	STDMETHOD(DeactivateAndUndo) (VOID);
	STDMETHOD(OnPosRectChange) (LPCRECT lprcPosRect);

	inline COleInPlaceSiteA(LPUNKNOWN pUnk, IOleInPlaceSite * pWide) :
			CAnsiInterface(ID_IOleInPlaceSite, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleInPlaceSite * GetWide() const
			{ return (IOleInPlaceSite *)m_pObj; };
};
