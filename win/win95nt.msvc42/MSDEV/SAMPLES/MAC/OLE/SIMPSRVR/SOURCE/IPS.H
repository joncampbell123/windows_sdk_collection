#ifndef _PPCMAC
#pragma once
#endif

class CDocument;


class CPersistStorage : public IPersistStorage  
{
public:

	unsigned long			m_cRef;
	CDocument*				m_pDocument;

	CPersistStorage(CDocument * pDoc);
	~CPersistStorage(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

    // *** IPersist methods ***
    virtual HRESULT STDMETHODCALLTYPE	GetClassID(LPCLSID lpClassID);

    // *** IPersistStorage methods ***
    virtual HRESULT STDMETHODCALLTYPE	IsDirty(void);
    virtual HRESULT STDMETHODCALLTYPE	InitNew(LPSTORAGE pStg);
    virtual HRESULT STDMETHODCALLTYPE	Load(LPSTORAGE pStg);
    virtual HRESULT STDMETHODCALLTYPE	Save(LPSTORAGE pStgSave, unsigned long fSameAsLoad);
    virtual HRESULT STDMETHODCALLTYPE	SaveCompleted(LPSTORAGE pStgNew);
    virtual HRESULT STDMETHODCALLTYPE	HandsOffStorage(void); 
    
    void								CreateStreams(void);
    void								OpenStreams(void);
    void								ReleaseStreamsAndStorage(void);
};

