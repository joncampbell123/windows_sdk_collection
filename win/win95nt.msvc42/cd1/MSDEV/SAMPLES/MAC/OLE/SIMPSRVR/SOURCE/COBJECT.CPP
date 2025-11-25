#include "hdrs.h"

ASSERTDATA


CObject::CObject(void)
{
	m_cRef = 0;
}


HRESULT STDMETHODCALLTYPE CObject::QueryInterface(REFIID riid, void * * ppv)
{
	*ppv = NULL;

	ENTER_INTERFACE("CObject::QueryInterface");

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppv = this;
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CObject::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CObject::AddRef",m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CObject::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CObject::Release",m_cRef);

	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}
