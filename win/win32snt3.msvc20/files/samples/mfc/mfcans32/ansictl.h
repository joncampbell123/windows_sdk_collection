//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       AnsiCtl.h
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    27-May-94   johnels     Created.
//
//---------------------------------------------------------------------------


typedef interface IOleControlA FAR* LPOLECONTROLA;
typedef interface IOleControlSiteA FAR* LPOLECONTROLSITEA;
typedef interface ISimpleFrameSiteA FAR* LPSIMPLEFRAMESITEA;
typedef interface IPersistStreamInitA FAR* LPPERSISTSTREAMINITA;
typedef interface IPropertyNotifySinkA FAR* LPPROPERTYNOTIFYSINKA;
typedef interface IProvideClassInfoA FAR* LPPROVIDECLASSINFOA;
typedef interface IConnectionPointContainerA FAR* LPCONNECTIONPOINTCONTAINERA;
typedef interface IEnumConnectionPointsA FAR* LPENUMCONNECTIONPOINTSA;
typedef interface IConnectionPointA FAR* LPCONNECTIONPOINTA;
typedef interface IEnumConnectionsA FAR* LPENUMCONNECTIONSA;
typedef struct tagCONNECTDATAA FAR* LPCONNECTDATAA;
typedef interface IClassFactory2A FAR* LPCLASSFACTORY2A;
typedef interface ISpecifyPropertyPagesA FAR* LPSPECIFYPROPERTYPAGESA;
typedef interface IPerPropertyBrowsingA FAR* LPPERPROPERTYBROWSINGA;
typedef interface IPropertyPageSiteA FAR* LPPROPERTYPAGESITEA;
typedef struct tagPROPPAGEINFOA FAR* LPPROPPAGEINFOA;
typedef interface IPropertyPageA FAR* LPPROPERTYPAGEA;
typedef interface IPropertyPage2A FAR* LPPROPERTYPAGE2A;
typedef struct tagOCPFIPARAMSA FAR* LPOCPFIPARAMSA;
typedef struct tagFONTDESCA FAR* LPFONTDESCA;
typedef interface IFontA FAR* LPFONTA;
typedef interface IFontDispA FAR* LPFONTDISPA;
typedef interface IPictureA FAR* LPPICTUREA;
typedef interface IPictureDispA FAR* LPPICTUREDISPA;


//////////////////////////////////////////////////////////////////////////////
// IPropertyNotifySinkA interface

#undef  INTERFACE
#define INTERFACE IPropertyNotifySinkA

DECLARE_INTERFACE_(IPropertyNotifySinkA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPropertyNotifySinkA methods
	STDMETHOD(OnChanged)(THIS_ DISPID dispid) PURE;
	STDMETHOD(OnRequestEdit)(THIS_ DISPID dispid) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IProvideClassInfoA interface

#undef  INTERFACE
#define INTERFACE IProvideClassInfoA

DECLARE_INTERFACE_(IProvideClassInfoA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IProvideClassInfoA methods
	STDMETHOD(GetClassInfo)(THIS_ LPTYPEINFOA FAR* ppTI) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IConnectionPointContainerA interface

#undef  INTERFACE
#define INTERFACE IConnectionPointContainerA

DECLARE_INTERFACE_(IConnectionPointContainerA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IConnectionPointContainerA methods
	STDMETHOD(EnumConnectionPoints)(
				THIS_ LPENUMCONNECTIONPOINTSA FAR* ppEnum) PURE;
	STDMETHOD(FindConnectionPoint)(
				THIS_ REFIID iid, LPCONNECTIONPOINTA FAR* ppCP) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IEnumConnectionPointsA interface

#undef  INTERFACE
#define INTERFACE IEnumConnectionPointsA

DECLARE_INTERFACE_(IEnumConnectionPointsA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumConnectionPointsA methods
	STDMETHOD(Next)(
				THIS_ ULONG cConnections,
				LPCONNECTIONPOINTA FAR* rgpcn, ULONG FAR* lpcFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG cConnections) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ LPENUMCONNECTIONPOINTSA FAR* ppEnum) PURE;
};

//////////////////////////////////////////////////////////////////////////////
// IConnectionPointA interface

#undef  INTERFACE
#define INTERFACE IConnectionPointA

DECLARE_INTERFACE_(IConnectionPointA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IConnectionPointA methods
	STDMETHOD(GetConnectionInterface)(THIS_ IID FAR* pIID) PURE;
	STDMETHOD(GetConnectionPointContainer)(
				THIS_ LPCONNECTIONPOINTCONTAINERA FAR* ppCPC) PURE;
	STDMETHOD(Advise)(THIS_ LPUNKNOWN pUnkSink, DWORD FAR* pdwCookie) PURE;
	STDMETHOD(Unadvise)(THIS_ DWORD dwCookie) PURE;
	STDMETHOD(EnumConnections)(THIS_ LPENUMCONNECTIONSA FAR* ppEnum) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// CONNECTDATAA structure

typedef struct tagCONNECTDATAA
{
	LPUNKNOWN pUnk;
	DWORD dwCookie;
} CONNECTDATAA;


//////////////////////////////////////////////////////////////////////////////
// IEnumConnectionsA interface

#undef  INTERFACE
#define INTERFACE IEnumConnectionsA

DECLARE_INTERFACE_(IEnumConnectionsA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IEnumConnectionsA methods
	STDMETHOD(Next)(
				THIS_ ULONG cConnections,
				LPCONNECTDATAA rgcd, ULONG FAR* lpcFetched) PURE;
	STDMETHOD(Skip)(THIS_ ULONG cConnections) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ LPENUMCONNECTIONSA FAR* ppecn) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IOleControlA interface

#undef  INTERFACE
#define INTERFACE IOleControlA

DECLARE_INTERFACE_(IOleControlA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleControlA methods
	STDMETHOD(GetControlInfo)(THIS_ LPCONTROLINFO pCI) PURE;
	STDMETHOD(OnMnemonic)(THIS_ LPMSG pMsg) PURE;
	STDMETHOD(OnAmbientPropertyChange)(THIS_ DISPID dispid) PURE;
	STDMETHOD(FreezeEvents)(THIS_ BOOL bFreeze) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IOleControlSiteA interface

#undef  INTERFACE
#define INTERFACE IOleControlSiteA

DECLARE_INTERFACE_(IOleControlSiteA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleControlSiteA methods
	STDMETHOD(OnControlInfoChanged)(THIS) PURE;
	STDMETHOD(LockInPlaceActive)(THIS_ BOOL fLock) PURE;
	STDMETHOD(GetExtendedControl)(THIS_ LPDISPATCHA FAR* ppDisp) PURE;
	STDMETHOD(TransformCoords)(THIS_ POINTL FAR* lpptlHimetric,
		POINTF FAR* lpptfContainer, DWORD flags) PURE;
	STDMETHOD(TranslateAccelerator)(THIS_ LPMSG lpMsg, DWORD grfModifiers) PURE;
	STDMETHOD(OnFocus)(THIS_ BOOL fGotFocus) PURE;
	STDMETHOD(ShowPropertyFrame)(THIS) PURE;
};

#define XFORMCOORDS_POSITION            0x1
#define XFORMCOORDS_SIZE                0x2
#define XFORMCOORDS_HIMETRICTOCONTAINER 0x4
#define XFORMCOORDS_CONTAINERTOHIMETRIC 0x8


//////////////////////////////////////////////////////////////////////////////
// ISimpleFrameSiteA interface

#undef  INTERFACE
#define INTERFACE ISimpleFrameSiteA

DECLARE_INTERFACE_(ISimpleFrameSiteA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ISimpleFrameSite methods
	STDMETHOD(PreMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD FAR * lpdwCookie) PURE;
	STDMETHOD(PostMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD dwCookie) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IPersistStreamInitA interface

#undef  INTERFACE
#define INTERFACE IPersistStreamInitA

DECLARE_INTERFACE_(IPersistStreamInitA, IPersist)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPersistA methods
	STDMETHOD(GetClassID)(THIS_ LPCLSID lpClassID) PURE;

	// IPersistStreamInitA methods
	STDMETHOD(IsDirty)(THIS) PURE;
	STDMETHOD(Load)(THIS_ LPSTREAMA pStm) PURE;
	STDMETHOD(Save)(THIS_ LPSTREAMA pStm, BOOL fClearDirty) PURE;
	STDMETHOD(GetSizeMax)(THIS_ ULARGE_INTEGER FAR* pcbSize) PURE;
	STDMETHOD(InitNew)(THIS) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IClassFactory2A interface

#undef  INTERFACE
#define INTERFACE IClassFactory2A

DECLARE_INTERFACE_(IClassFactory2A, IClassFactory)
{
	//  IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IClassFactoryA methods
	STDMETHOD(CreateInstance)(THIS_ LPUNKNOWN pUnkOuter, REFIID riid,
			LPVOID FAR* ppvObject) PURE;
	STDMETHOD(LockServer)(THIS_ BOOL fLock) PURE;

	//  IClassFactory2A methods
	STDMETHOD(GetLicInfo)(THIS_ LPLICINFO pLicInfo) PURE;
	STDMETHOD(RequestLicKey)(THIS_ DWORD dwReserved, BSTRA FAR* pbstrKey) PURE;
	STDMETHOD(CreateInstanceLic)(THIS_ LPUNKNOWN pUnkOuter,
			LPUNKNOWN pUnkReserved, REFIID riid, BSTRA bstrKey,
			LPVOID FAR* ppvObject) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// ISpecifyPropertyPagesA interface

#undef  INTERFACE
#define INTERFACE ISpecifyPropertyPagesA

DECLARE_INTERFACE_(ISpecifyPropertyPagesA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ISpecifyPropertyPagesA interface
	STDMETHOD(GetPages)(THIS_ CAUUID FAR* pPages) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// CALPSTR structure

typedef struct tagCALPSTR
{
	ULONG cElems;
	LPSTR FAR* pElems;

} CALPSTR;


//////////////////////////////////////////////////////////////////////////////
// IPerPropertyBrowsingA interface

#undef  INTERFACE
#define INTERFACE IPerPropertyBrowsingA

DECLARE_INTERFACE_(IPerPropertyBrowsingA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPerPropertyBrowsingA interface
	STDMETHOD(GetDisplayString)(THIS_ DISPID dispid, BSTRA FAR* lpbstr) PURE;
	STDMETHOD(MapPropertyToPage)(THIS_ DISPID dispid, LPCLSID lpclsid) PURE;
	STDMETHOD(GetPredefinedStrings)(THIS_ DISPID dispid,
		CALPSTR FAR* lpcaStringsOut, CADWORD FAR* lpcaCookiesOut) PURE;
	STDMETHOD(GetPredefinedValue)(THIS_ DISPID dispid, DWORD dwCookie,
		VARIANTA FAR* pvarOut) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IPropertyPageSiteA interface

#undef  INTERFACE
#define INTERFACE IPropertyPageSiteA

DECLARE_INTERFACE_(IPropertyPageSiteA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPropertyPageSiteA methods
	STDMETHOD(OnStatusChange)(THIS_ DWORD flags) PURE;
	STDMETHOD(GetLocaleID)(THIS_ LCID FAR* pLocaleID) PURE;
	STDMETHOD(GetPageContainer)(THIS_ LPUNKNOWN FAR* ppUnk) PURE;
	STDMETHOD(TranslateAccelerator)(THIS_ LPMSG lpMsg) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// PROPPAGEINFOA structure

typedef struct tagPROPPAGEINFOA
{
	size_t cb;
	LPSTR pszTitle;
	SIZE size;
	LPSTR pszDocString;
	LPSTR pszHelpFile;
	DWORD dwHelpContext;

} PROPPAGEINFOA;


//////////////////////////////////////////////////////////////////////////////
// IPropertyPageA interface

#undef  INTERFACE
#define INTERFACE IPropertyPageA

DECLARE_INTERFACE_(IPropertyPageA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPropertyPageA methods
	STDMETHOD(SetPageSite)(THIS_ LPPROPERTYPAGESITEA pPageSite) PURE;
	STDMETHOD(Activate)(THIS_ HWND hwndParent, LPCRECT rect,
				BOOL bModal) PURE;
	STDMETHOD(Deactivate)(THIS) PURE;
	STDMETHOD(GetPageInfo)(THIS_ LPPROPPAGEINFOA pPageInfo) PURE;
	STDMETHOD(SetObjects)(THIS_ ULONG cObjects, LPUNKNOWN FAR* ppunk) PURE;
	STDMETHOD(Show)(THIS_ UINT nCmdShow) PURE;
	STDMETHOD(Move)(LPCRECT prect) PURE;
	STDMETHOD(IsPageDirty)(THIS) PURE;
	STDMETHOD(Apply)(THIS) PURE;
	STDMETHOD(Help)(THIS_ LPCSTR lpszHelpDir) PURE;
	STDMETHOD(TranslateAccelerator)(THIS_ LPMSG lpMsg) PURE;
};


//////////////////////////////////////////////////////////////////////////////
// IPropertyPage2A interface

#undef  INTERFACE
#define INTERFACE IPropertyPage2A

DECLARE_INTERFACE_(IPropertyPage2A, IPropertyPageA)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IPropertyPageA methods
	STDMETHOD(SetPageSite)(THIS_ LPPROPERTYPAGESITEA pPageSite) PURE;
	STDMETHOD(Activate)(THIS_ HWND hwndParent, LPCRECT rect,
				BOOL bModal) PURE;
	STDMETHOD(Deactivate)(THIS) PURE;
	STDMETHOD(GetPageInfo)(THIS_ LPPROPPAGEINFOA pPageInfo) PURE;
	STDMETHOD(SetObjects)(THIS_ ULONG cObjects, LPUNKNOWN FAR* ppunk) PURE;
	STDMETHOD(Show)(THIS_ UINT nCmdShow) PURE;
	STDMETHOD(Move)(LPCRECT prect) PURE;
	STDMETHOD(IsPageDirty)(THIS) PURE;
	STDMETHOD(Apply)(THIS) PURE;
	STDMETHOD(Help)(THIS_ LPCSTR lpszHelpDir) PURE;
	STDMETHOD(TranslateAccelerator)(THIS_ LPMSG lpMsg) PURE;

	// IPropertyPage2A methods
	STDMETHOD(EditProperty)(THIS_ DISPID dispid) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// OCPFIPARAMSA structure (parameters for OleCreatePropertyFrameIndirectA)

typedef struct tagOCPFIPARAMSA
{
	ULONG cbStructSize;
	HWND hWndOwner;
	int x;
	int y;
	LPSTR lpszCaption;
	ULONG cObjects;
	LPUNKNOWN FAR* lplpUnk;
	ULONG cPages;
	CLSID FAR* lpPages;
	LCID lcid;
	DISPID dispidInitialProperty;

} OCPFIPARAMSA;


/////////////////////////////////////////////////////////////////////////////
// IFontA interface

#undef INTERFACE
#define INTERFACE IFontA

DECLARE_INTERFACE_(IFontA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IFontA methods
	STDMETHOD(get_Name)(THIS_ BSTRA FAR* pname) PURE;
	STDMETHOD(put_Name)(THIS_ BSTRA name) PURE;
	STDMETHOD(get_Size)(THIS_ CY FAR* psize) PURE;
	STDMETHOD(put_Size)(THIS_ CY size) PURE;
	STDMETHOD(get_Bold)(THIS_ BOOL FAR* pbold) PURE;
	STDMETHOD(put_Bold)(THIS_ BOOL bold) PURE;
	STDMETHOD(get_Italic)(THIS_ BOOL FAR* pitalic) PURE;
	STDMETHOD(put_Italic)(THIS_ BOOL italic) PURE;
	STDMETHOD(get_Underline)(THIS_ BOOL FAR* punderline) PURE;
	STDMETHOD(put_Underline)(THIS_ BOOL underline) PURE;
	STDMETHOD(get_Strikethrough)(THIS_ BOOL FAR* pstrikethrough) PURE;
	STDMETHOD(put_Strikethrough)(THIS_ BOOL strikethrough) PURE;
	STDMETHOD(get_Weight)(THIS_ short FAR* pweight) PURE;
	STDMETHOD(put_Weight)(THIS_ short weight) PURE;
	STDMETHOD(get_Charset)(THIS_ short FAR* pcharset) PURE;
	STDMETHOD(put_Charset)(THIS_ short charset) PURE;
	STDMETHOD(get_hFont)(THIS_ HFONT FAR* phfont) PURE;
	STDMETHOD(Clone)(THIS_ IFontA FAR* FAR* lplpfont) PURE;
	STDMETHOD(IsEqual)(THIS_ IFontA FAR * lpFontOther) PURE;
	STDMETHOD(SetRatio)(THIS_ long cyLogical, long cyHimetric) PURE;
	STDMETHOD(QueryTextMetrics)(THIS_ LPTEXTMETRICA lptm) PURE;
	STDMETHOD(AddRefHfont)(THIS_ HFONT hfont) PURE;
	STDMETHOD(ReleaseHfont)(THIS_ HFONT hfont) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IFontDispA interface

#undef INTERFACE
#define INTERFACE IFontDispA

DECLARE_INTERFACE_(IFontDispA, IDispatchA)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IDispatch methods
	STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

	STDMETHOD(GetTypeInfo)(
	  THIS_
	  UINT itinfo,
	  LCID lcid,
	  ITypeInfoA FAR* FAR* pptinfo) PURE;

	STDMETHOD(GetIDsOfNames)(
	  THIS_
	  REFIID riid,
	  LPSTR FAR* rgszNames,
	  UINT cNames,
	  LCID lcid,
	  DISPID FAR* rgdispid) PURE;

	STDMETHOD(Invoke)(
	  THIS_
	  DISPID dispidMember,
	  REFIID riid,
	  LCID lcid,
	  WORD wFlags,
	  DISPPARAMSA FAR* pdispparams,
	  VARIANTA FAR* pvarResult,
	  EXCEPINFOA FAR* pexcepinfo,
	  UINT FAR* puArgErr) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// FONTDESCA structure

typedef struct tagFONTDESCA
{
	UINT  cbSizeofstruct;
	LPSTR lpstrName;
	CY    cySize;
	SHORT sWeight;
	SHORT sCharset;
	BOOL  fItalic;
	BOOL  fUnderline;
	BOOL  fStrikethrough;
} FONTDESCA;


/////////////////////////////////////////////////////////////////////////////
// IPictureA interface

DECLARE_INTERFACE_(IPictureA, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IPictureA methods
	STDMETHOD(get_Handle)(THIS_ OLE_HANDLE FAR* phandle) PURE;
	STDMETHOD(get_hPal)(THIS_ OLE_HANDLE FAR* phpal) PURE;
	STDMETHOD(get_Type)(THIS_ short FAR* ptype) PURE;
	STDMETHOD(get_Width)(THIS_ OLE_XSIZE_HIMETRIC FAR* pwidth) PURE;
	STDMETHOD(get_Height)(THIS_ OLE_YSIZE_HIMETRIC FAR* pheight) PURE;
	STDMETHOD(Render)(THIS_ HDC hdc, long x, long y, long cx, long cy,
		OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
		OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
		LPRECT lprcWBounds) PURE;
	STDMETHOD(set_hPal)(THIS_ OLE_HANDLE hpal) PURE;
	STDMETHOD(get_CurDC)(THIS_ HDC FAR * phdcOut) PURE;
	STDMETHOD(SelectPicture)(THIS_
		HDC hdcIn, HDC FAR * phdcOut, OLE_HANDLE FAR * phbmpOut) PURE;
	STDMETHOD(get_KeepOriginalFormat)(THIS_ BOOL * pfkeep) PURE;
	STDMETHOD(put_KeepOriginalFormat)(THIS_ BOOL fkeep) PURE;
	STDMETHOD(PictureChanged)(THIS) PURE;
	STDMETHOD(SaveAsFile)(THIS_ LPSTREAMA lpstream, BOOL fSaveMemCopy,
		LONG FAR * lpcbSize) PURE;
	STDMETHOD(get_Attributes)(THIS_ DWORD FAR * lpdwAttr) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IPictureDispA interface

DECLARE_INTERFACE_(IPictureDispA, IDispatchA)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;

	// IDispatchA methods
	STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

	STDMETHOD(GetTypeInfo)(
	  THIS_
	  UINT itinfo,
	  LCID lcid,
	  ITypeInfoA FAR* FAR* pptinfo) PURE;

	STDMETHOD(GetIDsOfNames)(
	  THIS_
	  REFIID riid,
	  LPSTR FAR* rgszNames,
	  UINT cNames,
	  LCID lcid,
	  DISPID FAR* rgdispid) PURE;

	STDMETHOD(Invoke)(
	  THIS_
	  DISPID dispidMember,
	  REFIID riid,
	  LCID lcid,
	  WORD wFlags,
	  DISPPARAMSA FAR* pdispparams,
	  VARIANTA FAR* pvarResult,
	  EXCEPINFOA FAR* pexcepinfo,
	  UINT FAR* puArgErr) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Property sheet APIs

STDAPI OleCreatePropertyFrameA(HWND hwndOwner, UINT x, UINT y,
	LPCSTR lpszCaption, ULONG cObjects, LPUNKNOWN FAR* ppUnk,
	ULONG cPages, LPCLSID pPageClsID, LCID lcid,
	DWORD dwReserved, LPVOID pvReserved);

STDAPI OleCreatePropertyFrameIndirectA(LPOCPFIPARAMSA lpParams);


/////////////////////////////////////////////////////////////////////////////
// Font APIs

STDAPI OleCreateFontIndirectA(LPFONTDESCA lpfd,
		REFIID riid, LPVOID FAR* lplpvObj);


/////////////////////////////////////////////////////////////////////////////
// CPropertyNotifySinkA class

class CPropertyNotifySinkA : CAnsiInterface
{
public:
	// IPropertyNotifySinkA methods
	STDMETHOD(OnChanged)(DISPID dispid);
	STDMETHOD(OnRequestEdit)(DISPID dispid);

	inline CPropertyNotifySinkA(LPUNKNOWN pUnk, IPropertyNotifySink* pWide) :
			CAnsiInterface(ID_IPropertyNotifySink, pUnk, (LPUNKNOWN)pWide) {};

	inline IPropertyNotifySink* GetWide() const
			{ return (IPropertyNotifySink*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CProvideClassInfoA class

class CProvideClassInfoA : CAnsiInterface
{
public:
	// IProvideClassInfoA methods
	STDMETHOD(GetClassInfo)(LPTYPEINFOA FAR* ppTI);

	inline CProvideClassInfoA(LPUNKNOWN pUnk, IProvideClassInfo* pWide) :
			CAnsiInterface(ID_IProvideClassInfo, pUnk, (LPUNKNOWN)pWide) {};

	inline IProvideClassInfo* GetWide() const
			{ return (IProvideClassInfo*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointContainerA class

class CConnectionPointContainerA : CAnsiInterface
{
public:
	// IConnectionPointContainerA methods
	STDMETHOD(EnumConnectionPoints)(
				LPENUMCONNECTIONPOINTSA FAR* ppEnum);
	STDMETHOD(FindConnectionPoint)(
				REFIID iid, LPCONNECTIONPOINTA FAR* ppCP);

	inline CConnectionPointContainerA(LPUNKNOWN pUnk, IConnectionPointContainer* pWide) :
			CAnsiInterface(ID_IConnectionPointContainer, pUnk, (LPUNKNOWN)pWide) {};

	inline IConnectionPointContainer* GetWide() const
			{ return (IConnectionPointContainer*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionPointsA class

class CEnumConnectionPointsA : CAnsiInterface
{
public:
	// IEnumConnectionPointsA methods
	STDMETHOD(Next)(
				ULONG cConnections,
				LPCONNECTIONPOINTA FAR* rgpcn, ULONG FAR* lpcFetched);
	STDMETHOD(Skip)(ULONG cConnections);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(LPENUMCONNECTIONPOINTSA FAR* ppEnum);

	inline CEnumConnectionPointsA(LPUNKNOWN pUnk, IEnumConnectionPoints* pWide) :
			CAnsiInterface(ID_IEnumConnectionPoints, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumConnectionPoints* GetWide() const
			{ return (IEnumConnectionPoints*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointA class

class CConnectionPointA : CAnsiInterface
{
public:
	// IConnectionPointA methods
	STDMETHOD(GetConnectionInterface)(IID FAR* pIID);
	STDMETHOD(GetConnectionPointContainer)(
				LPCONNECTIONPOINTCONTAINERA FAR* ppCPC);
	STDMETHOD(Advise)(LPUNKNOWN pUnkSink, DWORD FAR* pdwCookie);
	STDMETHOD(Unadvise)(DWORD dwCookie);
	STDMETHOD(EnumConnections)(LPENUMCONNECTIONSA FAR* ppEnum);

	inline CConnectionPointA(LPUNKNOWN pUnk, IConnectionPoint* pWide) :
			CAnsiInterface(ID_IConnectionPoint, pUnk, (LPUNKNOWN)pWide) {};

	inline IConnectionPoint* GetWide() const
			{ return (IConnectionPoint*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionsA class

class CEnumConnectionsA : CAnsiInterface
{
public:
	// IEnumConnectionsA methods
	STDMETHOD(Next)(
				ULONG cConnections,
				LPCONNECTDATAA rgcd, ULONG FAR* lpcFetched);
	STDMETHOD(Skip)(ULONG cConnections);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(LPENUMCONNECTIONSA FAR* ppecn);

	inline CEnumConnectionsA(LPUNKNOWN pUnk, IEnumConnections* pWide) :
			CAnsiInterface(ID_IEnumConnections, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumConnections* GetWide() const
			{ return (IEnumConnections*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// COleControlA class

class COleControlA : CAnsiInterface
{
public:
	// IOleControlA methods
	STDMETHOD(GetControlInfo)(LPCONTROLINFO pCI);
	STDMETHOD(OnMnemonic)(LPMSG pMsg);
	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid);
	STDMETHOD(FreezeEvents)(BOOL bFreeze);

	inline COleControlA(LPUNKNOWN pUnk, IOleControl* pWide) :
			CAnsiInterface(ID_IOleControl, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleControl* GetWide() const
			{ return (IOleControl*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// COleControlSiteA class

class COleControlSiteA : CAnsiInterface
{
public:
	// IOleControlSiteA methods
	STDMETHOD(OnControlInfoChanged)();
	STDMETHOD(LockInPlaceActive)(BOOL fLock);
	STDMETHOD(GetExtendedControl)(LPDISPATCHA FAR* ppDisp);
	STDMETHOD(TransformCoords)(POINTL FAR* lpptlHimetric,
		POINTF FAR* lpptfContainer, DWORD flags);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)();

	inline COleControlSiteA(LPUNKNOWN pUnk, IOleControlSite* pWide) :
			CAnsiInterface(ID_IOleControlSite, pUnk, (LPUNKNOWN)pWide) {};

	inline IOleControlSite* GetWide() const
			{ return (IOleControlSite*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CSimpleFrameSiteA class

class CSimpleFrameSiteA : CAnsiInterface
{
public:
	// ISimpleFrameSite methods
	STDMETHOD(PreMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD FAR * lpdwCookie);
	STDMETHOD(PostMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD dwCookie);

	inline CSimpleFrameSiteA(LPUNKNOWN pUnk, ISimpleFrameSite* pWide) :
			CAnsiInterface(ID_ISimpleFrameSite, pUnk, (LPUNKNOWN)pWide) {};

	inline ISimpleFrameSite* GetWide() const
			{ return (ISimpleFrameSite*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPersistStreamInitA class

class CPersistStreamInitA : CAnsiInterface
{
public:
	// IPersistA methods
	STDMETHOD(GetClassID)(LPCLSID lpClassID);

	// IPersistStreamInitA methods
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)(LPSTREAMA pStm);
	STDMETHOD(Save)(LPSTREAMA pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER FAR* pcbSize);
	STDMETHOD(InitNew)();

	inline CPersistStreamInitA(LPUNKNOWN pUnk, IPersistStreamInit* pWide) :
			CAnsiInterface(ID_IPersistStreamInit, pUnk, (LPUNKNOWN)pWide) {};

	inline IPersistStreamInit* GetWide() const
			{ return (IPersistStreamInit*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CClassFactory2A class

class CClassFactory2A : CAnsiInterface
{
public:
	//  IClassFactoryA methods
	STDMETHOD(CreateInstance) (LPUNKNOWN pUnkOuter,
				  REFIID riid, LPVOID FAR* ppvObject);
	STDMETHOD(LockServer) (BOOL fLock);

	//  IClassFactory2A methods
	STDMETHOD(GetLicInfo)(LPLICINFO pLicInfo);
	STDMETHOD(RequestLicKey)(DWORD dwReserved, BSTRA FAR* pbstrKey);
	STDMETHOD(CreateInstanceLic)(LPUNKNOWN pUnkOuter,
			LPUNKNOWN pUnkReserved, REFIID riid, BSTRA bstrKey,
			LPVOID FAR* ppvObject);

	inline CClassFactory2A(LPUNKNOWN pUnk, IClassFactory2* pWide) :
			CAnsiInterface(ID_IClassFactory2, pUnk, (LPUNKNOWN)pWide) {};

	inline IClassFactory2* GetWide() const
			{ return (IClassFactory2*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CSpecifyPropertyPagesA class

class CSpecifyPropertyPagesA : CAnsiInterface
{
public:
	// ISpecifyPropertyPagesA interface
	STDMETHOD(GetPages)(CAUUID FAR* pPages);

	inline CSpecifyPropertyPagesA(LPUNKNOWN pUnk, ISpecifyPropertyPages* pWide) :
			CAnsiInterface(ID_ISpecifyPropertyPages, pUnk, (LPUNKNOWN)pWide) {};

	inline ISpecifyPropertyPages* GetWide() const
			{ return (ISpecifyPropertyPages*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPerPropertyBrowsingA class

class CPerPropertyBrowsingA : CAnsiInterface
{
public:
	// IPerPropertyBrowsingA interface
	STDMETHOD(GetDisplayString)(DISPID dispid, BSTRA FAR* lpbstr);
	STDMETHOD(MapPropertyToPage)(DISPID dispid, LPCLSID lpclsid);
	STDMETHOD(GetPredefinedStrings)(DISPID dispid,
		CALPSTR FAR* lpcaStringsOut, CADWORD FAR* lpcaCookiesOut);
	STDMETHOD(GetPredefinedValue)(DISPID dispid, DWORD dwCookie,
		VARIANTA FAR* pvarOut);

	inline CPerPropertyBrowsingA(LPUNKNOWN pUnk, IPerPropertyBrowsing* pWide) :
			CAnsiInterface(ID_IPerPropertyBrowsing, pUnk, (LPUNKNOWN)pWide) {};

	inline IPerPropertyBrowsing* GetWide() const
			{ return (IPerPropertyBrowsing*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyPageSiteA class

class CPropertyPageSiteA : CAnsiInterface
{
public:
	// IPropertyPageSiteA methods
	STDMETHOD(OnStatusChange)(DWORD flags);
	STDMETHOD(GetLocaleID)(LCID FAR* pLocaleID);
	STDMETHOD(GetPageContainer)(LPUNKNOWN FAR* ppUnk);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg);

	inline CPropertyPageSiteA(LPUNKNOWN pUnk, IPropertyPageSite* pWide) :
			CAnsiInterface(ID_IPropertyPageSite, pUnk, (LPUNKNOWN)pWide) {};

	inline IPropertyPageSite* GetWide() const
			{ return (IPropertyPageSite*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyPage2A class

class CPropertyPage2A : CAnsiInterface
{
public:
	// IPropertyPageA methods
	STDMETHOD(SetPageSite)(LPPROPERTYPAGESITEA pPageSite);
	STDMETHOD(Activate)(HWND hwndParent, LPCRECT rect,
				BOOL bModal);
	STDMETHOD(Deactivate)();
	STDMETHOD(GetPageInfo)(LPPROPPAGEINFOA pPageInfo);
	STDMETHOD(SetObjects)(ULONG cObjects, LPUNKNOWN FAR* ppunk);
	STDMETHOD(Show)(UINT nCmdShow);
	STDMETHOD(Move)(LPCRECT prect);
	STDMETHOD(IsPageDirty)();
	STDMETHOD(Apply)();
	STDMETHOD(Help)(LPCSTR lpszHelpDir);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg);

	// IPropertyPage2A methods
	STDMETHOD(EditProperty)(DISPID dispid);

	inline CPropertyPage2A(LPUNKNOWN pUnk, IPropertyPage2* pWide) :
			CAnsiInterface(ID_IPropertyPage2, pUnk, (LPUNKNOWN)pWide) {};

	inline IPropertyPage2* GetWide() const
			{ return (IPropertyPage2*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CFontA class

class CFontA : CAnsiInterface
{
public:
	// IFontA methods
	STDMETHOD(get_Name)(BSTRA FAR* pname);
	STDMETHOD(put_Name)(BSTRA name);
	STDMETHOD(get_Size)(CY FAR* psize);
	STDMETHOD(put_Size)(CY size);
	STDMETHOD(get_Bold)(BOOL FAR* pbold);
	STDMETHOD(put_Bold)(BOOL bold);
	STDMETHOD(get_Italic)(BOOL FAR* pitalic);
	STDMETHOD(put_Italic)(BOOL italic);
	STDMETHOD(get_Underline)(BOOL FAR* punderline);
	STDMETHOD(put_Underline)(BOOL underline);
	STDMETHOD(get_Strikethrough)(BOOL FAR* pstrikethrough);
	STDMETHOD(put_Strikethrough)(BOOL strikethrough);
	STDMETHOD(get_Weight)(short FAR* pweight);
	STDMETHOD(put_Weight)(short weight);
	STDMETHOD(get_Charset)(short FAR* pcharset);
	STDMETHOD(put_Charset)(short charset);
	STDMETHOD(get_hFont)(HFONT FAR* phfont);
	STDMETHOD(Clone)(IFontA FAR* FAR* lplpfont);
	STDMETHOD(IsEqual)(IFontA FAR * lpFontOther);
	STDMETHOD(SetRatio)(long cyLogical, long cyHimetric);
	STDMETHOD(QueryTextMetrics)(LPTEXTMETRICA lptm);
	STDMETHOD(AddRefHfont)(HFONT hfont);
	STDMETHOD(ReleaseHfont)(HFONT hfont);

	inline CFontA(LPUNKNOWN pUnk, IFont* pWide) :
			CAnsiInterface(ID_IFont, pUnk, (LPUNKNOWN)pWide) {};

	inline IFont* GetWide() const
			{ return (IFont*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPictureA class

class CPictureA : CAnsiInterface
{
public:
	// IPictureA methods
	STDMETHOD(get_Handle)(OLE_HANDLE FAR* phandle);
	STDMETHOD(get_hPal)(OLE_HANDLE FAR* phpal);
	STDMETHOD(get_Type)(short FAR* ptype);
	STDMETHOD(get_Width)(OLE_XSIZE_HIMETRIC FAR* pwidth);
	STDMETHOD(get_Height)(OLE_YSIZE_HIMETRIC FAR* pheight);
	STDMETHOD(Render)(HDC hdc, long x, long y, long cx, long cy,
		OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
		OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
		LPRECT lprcWBounds);
	STDMETHOD(set_hPal)(OLE_HANDLE hpal);
	STDMETHOD(get_CurDC)(HDC FAR * phdcOut);
	STDMETHOD(SelectPicture)(
		HDC hdcIn, HDC FAR * phdcOut, OLE_HANDLE FAR * phbmpOut);
	STDMETHOD(get_KeepOriginalFormat)(BOOL * pfkeep);
	STDMETHOD(put_KeepOriginalFormat)(BOOL fkeep);
	STDMETHOD(PictureChanged)();
	STDMETHOD(SaveAsFile)(LPSTREAMA lpstream, BOOL fSaveMemCopy,
		LONG FAR * lpcbSize);
	STDMETHOD(get_Attributes)(DWORD FAR * lpdwAttr);

	inline CPictureA(LPUNKNOWN pUnk, IPicture* pWide) :
			CAnsiInterface(ID_IPicture, pUnk, (LPUNKNOWN)pWide) {};

	inline IPicture* GetWide() const
			{ return (IPicture*)m_pObj; };
};
