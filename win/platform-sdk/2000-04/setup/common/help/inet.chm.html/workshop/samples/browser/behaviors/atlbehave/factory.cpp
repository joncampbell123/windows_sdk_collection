// Factory.cpp : Implementation of CFactory
#include "stdafx.h"
#include "AtlBehave.h"
#include "Factory.h"

/////////////////////////////////////////////////////////////////////////////
// CFactory
	STDMETHODIMP
CFactory::FindBehavior(
	LPOLESTR pchNameSpace,
	LPOLESTR pchTagName,
	IUnknown* pUnkArg,
	IElementBehavior** ppBehavior)
{
HRESULT					hr;
CComObject<CBehavior>*	pBehavior;

	// Create a behavior object
	hr = CComObject<CBehavior>::CreateInstance( &pBehavior );
	if ( SUCCEEDED(hr) )
		return pBehavior->QueryInterface( __uuidof(IElementBehavior), (void**)ppBehavior );
	else
		return hr;
}

