#ifndef _MSC_VER
#pragma once
#endif


#ifndef _COBJECTSITE_H_
#define _COBJECTSITE_H_

class CAdviseSink;
class COleClientSite;

class CObjectSite : public IUnknown
{
	public:
	
	LPOLEOBJECT		m_pObject;
	LPSTORAGE		m_pRootStg,
					m_pObjectStg;
	CAdviseSink		*m_pAdviseSink;
	COleClientSite	*m_pOleClientSite;
	SIZEL			m_ObjSizeL;
	WindowPtr		m_pWindowObjSite;
	short			m_refCount;
	unsigned long	m_dwConnection;
	Boolean			m_fObjectOpen;
	
	CObjectSite		(void);
	~CObjectSite	(void);

	void			InitObjectSite(WindowPtr pWindow);
	
    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE 		QueryInterface	(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE AddRef			(void);
    virtual unsigned long STDMETHODCALLTYPE Release			(void);

	void			Delete			(void);
    void			DeleteObject	(void);
};

#endif // _COBJECTSITE_H_
