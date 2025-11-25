#ifndef _MSC_VER
#pragma once
#endif

#ifndef _COLECLIENTSITE_H
#define _COLECLIENTSITE_H_

class CObjectSite;

class COleClientSite : public IOleClientSite
{
	public:
	
	CObjectSite 	*m_pObjectSite;
	unsigned long	m_refCount;		// interface refcount for debugging purposes
	WindowPtr		m_pWindow;
	
	COleClientSite	(void);
	~COleClientSite	(void);
	
	void			InitClientSite	(CObjectSite * pObjectSite);
	void			Delete			(void);

	HRESULT			STDMETHODCALLTYPE QueryInterface(REFIID riid, void * * ppvObj);
	unsigned long 	STDMETHODCALLTYPE AddRef();
	unsigned long 	STDMETHODCALLTYPE Release();
	
	HRESULT 		STDMETHODCALLTYPE SaveObject();
	HRESULT 		STDMETHODCALLTYPE GetMoniker(unsigned long dwAssign,unsigned long dwWhichMoniker, LPMONIKER *ppmk);
	HRESULT			STDMETHODCALLTYPE GetContainer(LPOLECONTAINER * ppContainer);
	HRESULT			STDMETHODCALLTYPE ShowObject();
	HRESULT			STDMETHODCALLTYPE OnShowWindow(unsigned long fShow);
	HRESULT			STDMETHODCALLTYPE RequestNewObjectLayout();
	
	static			COleClientSite *  Create(CObjectSite * pObjSite);
};

#endif // _COLECLIENTSITE_H_
