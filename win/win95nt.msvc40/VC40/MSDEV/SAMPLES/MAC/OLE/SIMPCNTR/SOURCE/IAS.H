#ifndef _MSC_VER
#pragma once
#endif

class CObjectSite;

interface CAdviseSink : public IAdviseSink
{
	public:
	
	CObjectSite		*m_pObjectSite;
	unsigned long	m_refCount;		// interface refcount for debugging purposes
	
	CAdviseSink(void);
	~CAdviseSink(void);
	
	void			InitAdviseSink	(CObjectSite * pObjectSite);
	void			Delete			(void);

	HRESULT			STDMETHODCALLTYPE QueryInterface(REFIID riid, void * * ppv);
	unsigned long 	STDMETHODCALLTYPE AddRef();
	unsigned long 	STDMETHODCALLTYPE Release();
	void 			STDMETHODCALLTYPE OnDataChange(FORMATETC * pFormatetc,STGMEDIUM * pStgmed);
	void 			STDMETHODCALLTYPE OnViewChange(unsigned long dwAspect, long lindex);
	void 			STDMETHODCALLTYPE OnRename(LPMONIKER pmk);
	void 			STDMETHODCALLTYPE OnSave();
	void 			STDMETHODCALLTYPE OnClose();
};
