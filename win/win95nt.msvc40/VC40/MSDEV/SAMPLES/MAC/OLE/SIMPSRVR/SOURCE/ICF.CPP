#include "hdrs.h"

ASSERTDATA


CClassFactory::CClassFactory(void)
{
	ENTER_INTERFACE("CClassFactory::CClassFactory");

	m_cRef  = 1; 
}


CClassFactory::~CClassFactory(void)
{
	ENTER_INTERFACE("CClassFactory::~CClassFactory");
}


HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, void * * ppv)
{
	ENTER_INTERFACE("CClassFactory::QueryInterface");

	*ppv = NULL;

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown || riid == IID_IClassFactory)
	{
		AddRef();
		*ppv = this;
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CClassFactory::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CClassFactory::AddRef", m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CClassFactory::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CClassFactory::Release",m_cRef);

	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN lpUnkOuter, REFIID riid, void* * ppvObj)
{
	ENTER_INTERFACE("CClassFactory::CreateInstance");

	// Our object doesn't support aggregation
	if (lpUnkOuter)
		return ResultFromScode(CLASS_E_NOAGGREGATION);

	*ppvObj = NULL;

	ASSERTCOND(gApp != NULL);

	gApp->CreateNewDocument(false);
	ASSERTCOND(gApp->m_document);

	// Apparently we ran out of memory!
	if (gApp->m_document == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	return gApp->m_document->QueryInterface(riid, ppvObj);
}


HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(unsigned long fLock)
{
	ENTER_INTERFACE("CClassFactory::LockServer");

	return gApp->OleLockApplication((Boolean)fLock, true);
}
