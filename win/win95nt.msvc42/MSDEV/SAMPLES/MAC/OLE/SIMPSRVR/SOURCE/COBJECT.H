#ifndef _COBJECT_H_
#define _COBJECT_H_

#include "ole2.h"

class CObject : public IUnknown {

public:

	unsigned long		m_cRef;

	CObject(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
#ifdef __powerc
	virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);
#else    
	virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);
#endif
};

#endif // _COBJECT_H_