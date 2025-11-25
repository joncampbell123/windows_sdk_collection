#ifndef _MSC_VER
#pragma once
#endif

class CDocument;

class COleObject : public IOleObject  
{
	public:

	unsigned long	m_cRef;
	Boolean			m_fOpen;
	CDocument*		m_pDocument;

	COleObject	(CDocument * pDoc);
	~COleObject	(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

    // *** IOleObject methods ***
    virtual HRESULT STDMETHODCALLTYPE	SetClientSite(LPOLECLIENTSITE pClientSite);
    virtual HRESULT STDMETHODCALLTYPE	GetClientSite(LPLPOLECLIENTSITE ppClientSite);
    virtual HRESULT STDMETHODCALLTYPE	SetHostNames(const char * szContainerApp, const char * szContainerObj);
    virtual HRESULT STDMETHODCALLTYPE	Close(unsigned long dwSaveOption);
    virtual HRESULT STDMETHODCALLTYPE	SetMoniker(unsigned long dwWhichMoniker, LPMONIKER pmk);
    virtual HRESULT STDMETHODCALLTYPE	GetMoniker(unsigned long dwAssign, unsigned long dwWhichMoniker, 
										LPLPMONIKER ppmk);
    virtual HRESULT STDMETHODCALLTYPE	InitFromData(LPDATAOBJECT pDataObject,
										unsigned long fCreation,
										unsigned long dwReserved);
    virtual HRESULT STDMETHODCALLTYPE	GetClipboardData(unsigned long dwReserved, 
										LPLPDATAOBJECT ppDataObject);
    virtual HRESULT STDMETHODCALLTYPE	DoVerb(long iVerb,
										EventRecord * pEvt, 
										LPOLECLIENTSITE pActiveSite, 
										long lindex,
										WindowPtr pWndParent,
										LPCRECT lprcPosRect);
    virtual HRESULT STDMETHODCALLTYPE	EnumVerbs(LPLPENUMOLEVERB ppenumOleVerb);
    virtual HRESULT STDMETHODCALLTYPE	Update(void);
    virtual HRESULT STDMETHODCALLTYPE	IsUpToDate(void);
    virtual HRESULT STDMETHODCALLTYPE	GetUserClassID(CLSID * pClsid);
    virtual HRESULT STDMETHODCALLTYPE	GetUserType(unsigned long dwFormOfType, char * * pszUserType);
    virtual HRESULT STDMETHODCALLTYPE	SetExtent(unsigned long dwDrawAspect, LPSIZEL lpsizel);
    virtual HRESULT STDMETHODCALLTYPE	GetExtent(unsigned long dwDrawAspect, LPSIZEL lpsizel);
    virtual HRESULT STDMETHODCALLTYPE	Advise(LPADVISESINK pAdvSink, unsigned long * pdwConnection);
    virtual HRESULT STDMETHODCALLTYPE	Unadvise(unsigned long dwConnection);
    virtual HRESULT STDMETHODCALLTYPE	EnumAdvise(LPLPENUMSTATDATA ppenumAdvise);
    virtual HRESULT STDMETHODCALLTYPE	GetMiscStatus(unsigned long dwAspect, unsigned long * pdwStatus);
    virtual HRESULT STDMETHODCALLTYPE	SetColorScheme(LPOLECOLORSCHEME lpColorScheme);

	void						OpenEdit(LPOLECLIENTSITE pActiveSite);
};
