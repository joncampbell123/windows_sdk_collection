//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       WideCtl.h
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CPropertyNotifySinkW class

class CPropertyNotifySinkW : CWideInterface
{
public:
	// IPropertyNotifySinkW methods
	STDMETHOD(OnChanged)(DISPID dispid);
	STDMETHOD(OnRequestEdit)(DISPID dispid);

	inline CPropertyNotifySinkW(LPUNKNOWN pUnk, IPropertyNotifySinkA* pANSI) :
			CWideInterface(ID_IPropertyNotifySink, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPropertyNotifySinkA* GetANSI() const
			{ return (IPropertyNotifySinkA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CProvideClassInfoW class

class CProvideClassInfoW : CWideInterface
{
public:
	// IProvideClassInfoW methods
	STDMETHOD(GetClassInfo)(LPTYPEINFO FAR* ppTI);

	inline CProvideClassInfoW(LPUNKNOWN pUnk, IProvideClassInfoA* pANSI) :
			CWideInterface(ID_IProvideClassInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline IProvideClassInfoA* GetANSI() const
			{ return (IProvideClassInfoA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointContainerW class

class CConnectionPointContainerW : CWideInterface
{
public:
	// IConnectionPointContainerW methods
	STDMETHOD(EnumConnectionPoints)(
				LPENUMCONNECTIONPOINTS FAR* ppEnum);
	STDMETHOD(FindConnectionPoint)(
				REFIID iid, LPCONNECTIONPOINT FAR* ppCP);

	inline CConnectionPointContainerW(LPUNKNOWN pUnk, IConnectionPointContainerA* pANSI) :
			CWideInterface(ID_IConnectionPointContainer, pUnk, (LPUNKNOWN)pANSI) {};

	inline IConnectionPointContainerA* GetANSI() const
			{ return (IConnectionPointContainerA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionPointsW class

class CEnumConnectionPointsW : CWideInterface
{
public:
	// IEnumConnectionPointsW methods
	STDMETHOD(Next)(
				ULONG cConnections,
				LPCONNECTIONPOINT FAR* rgpcn, ULONG FAR* lpcFetched);
	STDMETHOD(Skip)(ULONG cConnections);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(LPENUMCONNECTIONPOINTS FAR* ppEnum);

	inline CEnumConnectionPointsW(LPUNKNOWN pUnk, IEnumConnectionPointsA* pANSI) :
			CWideInterface(ID_IEnumConnectionPoints, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumConnectionPointsA* GetANSI() const
			{ return (IEnumConnectionPointsA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointW class

class CConnectionPointW : CWideInterface
{
public:
	// IConnectionPointW methods
	STDMETHOD(GetConnectionInterface)(IID FAR* pIID);
	STDMETHOD(GetConnectionPointContainer)(
				LPCONNECTIONPOINTCONTAINER FAR* ppCPC);
	STDMETHOD(Advise)(LPUNKNOWN pUnkSink, DWORD FAR* pdwCookie);
	STDMETHOD(Unadvise)(DWORD dwCookie);
	STDMETHOD(EnumConnections)(LPENUMCONNECTIONS FAR* ppEnum);

	inline CConnectionPointW(LPUNKNOWN pUnk, IConnectionPointA* pANSI) :
			CWideInterface(ID_IConnectionPoint, pUnk, (LPUNKNOWN)pANSI) {};

	inline IConnectionPointA* GetANSI() const
			{ return (IConnectionPointA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionsW class

class CEnumConnectionsW : CWideInterface
{
public:
	// IEnumConnectionsW methods
	STDMETHOD(Next)(
				ULONG cConnections,
				LPCONNECTDATA rgcd, ULONG FAR* lpcFetched);
	STDMETHOD(Skip)(ULONG cConnections);
	STDMETHOD(Reset)();
	STDMETHOD(Clone)(LPENUMCONNECTIONS FAR* ppecn);

	inline CEnumConnectionsW(LPUNKNOWN pUnk, IEnumConnectionsA* pANSI) :
			CWideInterface(ID_IEnumConnections, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumConnectionsA* GetANSI() const
			{ return (IEnumConnectionsA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// COleControlW class

class COleControlW : CWideInterface
{
public:
	// IOleControlW methods
	STDMETHOD(GetControlInfo)(LPCONTROLINFO pCI);
	STDMETHOD(OnMnemonic)(LPMSG pMsg);
	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid);
	STDMETHOD(FreezeEvents)(BOOL bFreeze);

	inline COleControlW(LPUNKNOWN pUnk, IOleControlA* pANSI) :
			CWideInterface(ID_IOleControl, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleControlA* GetANSI() const
			{ return (IOleControlA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// COleControlSiteW class

class COleControlSiteW : CWideInterface
{
public:
	// IOleControlSiteW methods
	STDMETHOD(OnControlInfoChanged)();
	STDMETHOD(LockInPlaceActive)(BOOL fLock);
	STDMETHOD(GetExtendedControl)(LPDISPATCH FAR* ppDisp);
	STDMETHOD(TransformCoords)(POINTL FAR* lpptlHimetric,
		POINTF FAR* lpptfContainer, DWORD flags);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)();

	inline COleControlSiteW(LPUNKNOWN pUnk, IOleControlSiteA* pANSI) :
			CWideInterface(ID_IOleControlSite, pUnk, (LPUNKNOWN)pANSI) {};

	inline IOleControlSiteA* GetANSI() const
			{ return (IOleControlSiteA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CSimpleFrameSiteW class

class CSimpleFrameSiteW : CWideInterface
{
public:
	// ISimpleFrameSite methods
	STDMETHOD(PreMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD FAR * lpdwCookie);
	STDMETHOD(PostMessageFilter)(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
		LRESULT FAR* lplResult, DWORD dwCookie);

	inline CSimpleFrameSiteW(LPUNKNOWN pUnk, ISimpleFrameSiteA* pANSI) :
			CWideInterface(ID_ISimpleFrameSite, pUnk, (LPUNKNOWN)pANSI) {};

	inline ISimpleFrameSiteA* GetANSI() const
			{ return (ISimpleFrameSiteA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPersistStreamInitW class

class CPersistStreamInitW : CWideInterface
{
public:
	// IPersistW methods
	STDMETHOD(GetClassID)(LPCLSID lpClassID);

	// IPersistStreamInitW methods
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)(LPSTREAM pStm);
	STDMETHOD(Save)(LPSTREAM pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER FAR* pcbSize);
	STDMETHOD(InitNew)();

	inline CPersistStreamInitW(LPUNKNOWN pUnk, IPersistStreamInitA* pANSI) :
			CWideInterface(ID_IPersistStreamInit, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPersistStreamInitA* GetANSI() const
			{ return (IPersistStreamInitA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CClassFactory2W class

class CClassFactory2W : CWideInterface
{
public:
	//  IClassFactoryW methods
	STDMETHOD(CreateInstance) (LPUNKNOWN pUnkOuter,
				  REFIID riid, LPVOID FAR* ppvObject);
	STDMETHOD(LockServer) (BOOL fLock);

	//  IClassFactory2W methods
	STDMETHOD(GetLicInfo)(LPLICINFO pLicInfo);
	STDMETHOD(RequestLicKey)(DWORD dwReserved, BSTR FAR* pbstrKey);
	STDMETHOD(CreateInstanceLic)(LPUNKNOWN pUnkOuter,
			LPUNKNOWN pUnkReserved, REFIID riid, BSTR bstrKey,
			LPVOID FAR* ppvObject);

	inline CClassFactory2W(LPUNKNOWN pUnk, IClassFactory2A* pANSI) :
			CWideInterface(ID_IClassFactory2, pUnk, (LPUNKNOWN)pANSI) {};

	inline IClassFactory2A* GetANSI() const
			{ return (IClassFactory2A*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CSpecifyPropertyPagesW class

class CSpecifyPropertyPagesW : CWideInterface
{
public:
	// ISpecifyPropertyPagesA interface
	STDMETHOD(GetPages)(CAUUID FAR* pPages);

	inline CSpecifyPropertyPagesW(LPUNKNOWN pUnk, ISpecifyPropertyPagesA* pANSI) :
			CWideInterface(ID_ISpecifyPropertyPages, pUnk, (LPUNKNOWN)pANSI) {};

	inline ISpecifyPropertyPagesA* GetANSI() const
			{ return (ISpecifyPropertyPagesA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPerPropertyBrowsingW class

class CPerPropertyBrowsingW : CWideInterface
{
public:
	// IPerPropertyBrowsingA interface
	STDMETHOD(GetDisplayString)(DISPID dispid, BSTR FAR* lpbstr);
	STDMETHOD(MapPropertyToPage)(DISPID dispid, LPCLSID lpclsid);
	STDMETHOD(GetPredefinedStrings)(DISPID dispid,
		CALPOLESTR FAR* lpcaStringsOut, CADWORD FAR* lpcaCookiesOut);
	STDMETHOD(GetPredefinedValue)(DISPID dispid, DWORD dwCookie,
		VARIANT FAR* pvarOut);

	inline CPerPropertyBrowsingW(LPUNKNOWN pUnk, IPerPropertyBrowsingA* pANSI) :
			CWideInterface(ID_IPerPropertyBrowsing, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPerPropertyBrowsingA* GetANSI() const
			{ return (IPerPropertyBrowsingA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyPageSiteW class

class CPropertyPageSiteW : CWideInterface
{
public:
	// IPropertyPageSiteW methods
	STDMETHOD(OnStatusChange)(DWORD flags);
	STDMETHOD(GetLocaleID)(LCID FAR* pLocaleID);
	STDMETHOD(GetPageContainer)(LPUNKNOWN FAR* ppUnk);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg);

	inline CPropertyPageSiteW(LPUNKNOWN pUnk, IPropertyPageSiteA* pANSI) :
			CWideInterface(ID_IPropertyPageSite, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPropertyPageSiteA* GetANSI() const
			{ return (IPropertyPageSiteA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPropertyPage2W class

class CPropertyPage2W : CWideInterface
{
public:
	// IPropertyPageW methods
	STDMETHOD(SetPageSite)(LPPROPERTYPAGESITE pPageSite);
	STDMETHOD(Activate)(HWND hwndParent, LPCRECT rect,
				BOOL bModal);
	STDMETHOD(Deactivate)();
	STDMETHOD(GetPageInfo)(LPPROPPAGEINFO pPageInfo);
	STDMETHOD(SetObjects)(ULONG cObjects, LPUNKNOWN FAR* ppunk);
	STDMETHOD(Show)(UINT nCmdShow);
	STDMETHOD(Move)(LPCRECT prect);
	STDMETHOD(IsPageDirty)();
	STDMETHOD(Apply)();
	STDMETHOD(Help)(LPCOLESTR lpszHelpDir);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg);

	// IPropertyPage2W methods
	STDMETHOD(EditProperty)(DISPID dispid);

	inline CPropertyPage2W(LPUNKNOWN pUnk, IPropertyPage2A* pANSI) :
			CWideInterface(ID_IPropertyPage2, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPropertyPage2A* GetANSI() const
			{ return (IPropertyPage2A*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CFontW class

class CFontW : CWideInterface
{
public:
	// IFontW methods
	STDMETHOD(get_Name)(BSTR FAR* pname);
	STDMETHOD(put_Name)(BSTR name);
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
	STDMETHOD(Clone)(IFont FAR* FAR* lplpfont);
	STDMETHOD(IsEqual)(IFont FAR * lpFontOther);
	STDMETHOD(SetRatio)(long cyLogical, long cyHimetric);
	STDMETHOD(QueryTextMetrics)(LPTEXTMETRICW lptm);
	STDMETHOD(AddRefHfont)(HFONT hfont);
	STDMETHOD(ReleaseHfont)(HFONT hfont);

	inline CFontW(LPUNKNOWN pUnk, IFontA* pANSI) :
			CWideInterface(ID_IFont, pUnk, (LPUNKNOWN)pANSI) {};

	inline IFontA* GetANSI() const
			{ return (IFontA*)m_pObj; };
};


/////////////////////////////////////////////////////////////////////////////
// CPictureW class

class CPictureW : CWideInterface
{
public:
	// IPictureW methods
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
	STDMETHOD(SaveAsFile)(LPSTREAM lpstream, BOOL fSaveMemCopy,
		LONG FAR * lpcbSize);
	STDMETHOD(get_Attributes)(DWORD FAR * lpdwAttr);

	inline CPictureW(LPUNKNOWN pUnk, IPictureA* pANSI) :
			CWideInterface(ID_IPicture, pUnk, (LPUNKNOWN)pANSI) {};

	inline IPictureA* GetANSI() const
			{ return (IPictureA*)m_pObj; };
};
