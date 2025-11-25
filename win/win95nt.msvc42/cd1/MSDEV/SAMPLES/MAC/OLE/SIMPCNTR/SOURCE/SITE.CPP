#include "hdrs.h"

ASSERTDATA

CObjectSite::CObjectSite(void)
{
	m_pObject    	 = NULL;
	m_pRootStg   	 = NULL;
	m_pObjectStg 	 = NULL;
	m_pAdviseSink	 = NULL;
	m_pOleClientSite = NULL;
	m_dwConnection   = 0;
	m_fObjectOpen	 = false;

	m_refCount = 0;
}


CObjectSite::~CObjectSite(void)
{
}


void CObjectSite::InitObjectSite(WindowPtr pWindow)
{
	Boolean 	fResult = true;
	
	m_pWindowObjSite = pWindow;

	m_pAdviseSink = new CAdviseSink;
	if (!m_pAdviseSink)
		fResult = false;
	m_pAdviseSink->InitAdviseSink(this);

	m_pOleClientSite = new COleClientSite;
	if (!m_pOleClientSite)
	{
		if (m_pAdviseSink)
			delete m_pAdviseSink;
		fResult = false;
	} 

	m_pOleClientSite->InitClientSite(this);
}


HRESULT CObjectSite::QueryInterface(REFIID riid, void * * ppvObj)
{
	*ppvObj = NULL;
	
	if ( IsEqualIID(riid, IID_IUnknown) )
	{
		AddRef();
		*ppvObj = this;
		
		return ResultFromScode(S_OK);
	}
	
	if ( IsEqualIID(riid, IID_IOleClientSite) )
	{
		AddRef();
		*ppvObj = m_pOleClientSite;
		
		return ResultFromScode(S_OK);
	}
	
	if ( IsEqualIID(riid, IID_IAdviseSink) )
	{
		AddRef();
		*ppvObj = m_pAdviseSink;
		
		return ResultFromScode(S_OK);
	}
	
	// request for unsupported interface
	
	return ResultFromScode(E_NOINTERFACE);
}



unsigned long CObjectSite::AddRef()
{
	return ++m_refCount;
}


unsigned long CObjectSite::Release()
{
	if (--m_refCount == 0)
	{
		m_pOleClientSite->Delete();
		m_pAdviseSink->Delete();
		delete this;
		return 0;
	}
	
	return m_refCount;
}


void CObjectSite::DeleteObject(void)
{
	long	ref;
	HRESULT	hr;
	
	ASSERT(m_pObject != NULL);
	ASSERT(m_pRootStg != NULL);
	ASSERT(m_pObjectStg != NULL);
	
	hr = m_pObject->Close(OLECLOSE_SAVEIFDIRTY);
	FailScode("\pClose()", GetScode(hr));
		
	ref = m_pObject->Release();
	ASSERT(ref == 0);	

	ref = m_pObjectStg->Release();
	ASSERT(ref == 0);	
	
	ref = m_pRootStg->Release();
	ASSERT(ref == 0);
	
//	hr = CoDisconnectObject((LPUNKNOWN)this,0);
//	FailScode("\pClose()", GetScode(hr));
	
	m_pObject    = NULL;
	m_pObjectStg = NULL;
	m_pRootStg   = NULL;		
}


void CObjectSite::Delete(void)
{
	ASSERT(m_pOleClientSite);
	ASSERT(m_pAdviseSink);
	
	if (m_pObject)
		DeleteObject();
		
	Release();	
}
