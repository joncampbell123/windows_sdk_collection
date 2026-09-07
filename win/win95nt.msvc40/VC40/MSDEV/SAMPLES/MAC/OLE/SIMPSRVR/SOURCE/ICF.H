#ifndef _MSC_VER
#pragma once
#endif

#ifndef _CLASSFACTORY_H_
#define _CLASSFACTORY_H_

#include "compobj.h"

class CServerObject;

class CClassFactory : public IClassFactory  
{
public:

	unsigned long			m_cRef;

	CClassFactory(void);
	~CClassFactory(void);

    // *** IUnknown methods ***
	virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID iid, void * * ppv);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

	virtual HRESULT STDMETHODCALLTYPE	CreateInstance(LPUNKNOWN lpUnkOuter, REFIID riid, void* * lplpvObj);
	virtual HRESULT STDMETHODCALLTYPE	LockServer(unsigned long fLock);
};

#endif // _CLASSFACTORY_H_
