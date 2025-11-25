//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// AnyEnum

// Copy(ish) constructor
// For reasons of performance, we tend to pass a HRESULT *  and set this if anything
// goes wrong, rather than resort to C++ exception handling.  The instantiator is
// responsible for checking the HRESULT and deleting us if we're deemed worthless.
template<class IEnum, class BaseType>
AnyEnum<IEnum, BaseType>::AnyEnum( const AnyEnum & copy, HRESULT * phr )
: m_cRef(1)
{
    *phr = copy.m_pIEnum->Clone( &m_pIEnum );
}

// Our QueryInterface is a little degenerate.  We will give back an IUnknown,
// but we can't actually tell the IID of the IEnum we are covering, so we can't
// (easily) check that say, IID_IEnumPins, is realy us.  If you try and get
// anything other than an IUnknown, we'll tell you to E_NOINTERFACE.
template<class IEnum, class BaseType>
STDMETHODIMP AnyEnum<IEnum, BaseType>::QueryInterface( REFIID iid, void ** ppv )
{
    HRESULT hr = S_OK;
    if (ppv)
    {
        *ppv = 0;
        if ( iid == IID_IUnknown )          *ppv = static_cast<IUnknown *>(this);
        else                                hr = E_NOINTERFACE;
        if ( *ppv )                         AddRef();
    }
    else hr = E_POINTER;
    return E_UNEXPECTED;
}

template<class IEnum, class BaseType>
STDMETHODIMP_(ULONG) AnyEnum<IEnum, BaseType>::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

template<class IEnum, class BaseType>
STDMETHODIMP_(ULONG) AnyEnum<IEnum, BaseType>::Release()
{
    const ULONG count = InterlockedDecrement(&m_cRef);
	if (count==0) delete this;
	return count;
}

// Delagate Next() to our inner IEnum
template<class IEnum, class BaseType>
STDMETHODIMP AnyEnum<IEnum, BaseType>::Next( ULONG cBase, BaseType ** ppBase, ULONG * pcFetched )
{
    return m_pIEnum->Next( cBase, ppBase, pcFetched );
}

// Delagate Skip() to our inner IEnum
template<class IEnum, class BaseType>
STDMETHODIMP AnyEnum<IEnum, BaseType>::Skip( ULONG cBase )
{
    return m_pIEnum->Skip(cBase);
}

// Delagate Reset() to our inner IEnum
template<class IEnum, class BaseType>
STDMETHODIMP AnyEnum<IEnum, BaseType>::Reset( void )
{
    return m_pIEnum->Reset();
}

// Our Clone() method clones the outer, then clones the inner.
// This method will have to be overriden in derived classes
// that introduce new attributes.
template<class IEnum, class BaseType>
STDMETHODIMP AnyEnum<IEnum, BaseType>::Clone( IEnum ** ppEnum )
{
    return CloneAny( *this, ppEnum );
}



// ConstrictedEnum

// This is the meat.  We must retrieve BaseObjects individually and only pass them back to
// our caller if they meet Select's criteria (whatever that might be).  pcFetched must be a
// pointer to a ULONG.  ppBase may be null, in which case we don't pass back any objects, we
// merely count them (this is mainly for the benefit of Skip()).
template<class IEnum, class BaseType>
STDMETHODIMP ConstrictedEnum<IEnum, BaseType>::Next( ULONG cBase, BaseType ** ppBase, ULONG * pcFetched )
{
	HRESULT hr = S_OK;
		
	BaseType * lcl_pBase;
	ULONG lcl_fetched;

    if ( pcFetched )
    {
    	*pcFetched = 0;

		while ( cBase > *pcFetched )
		{
			hr = m_pIEnum->Next( 1, &lcl_pBase, &lcl_fetched );
			if (FAILED(hr)) break;
			if (lcl_fetched == 0) break;

			if ( Select( &lcl_pBase ) )
			{
				// Yes, client wants it counted
				++*pcFetched;
				// Does client want the pointers?
				if ( ppBase )
				{
					// yes
					*ppBase = lcl_pBase;
					++ppBase;
				}
				else lcl_pBase->Release();
			}
			else
			{
				// Select doesn't want it, throw it away!
				lcl_pBase->Release();
			}
		}
    }
    else hr = E_POINTER;

	return hr;
}

// We can't use our inherited or inner Skip(), since these won't honour
// Select()'s selection criteria.
template<class IEnum, class BaseType>
STDMETHODIMP ConstrictedEnum<IEnum, BaseType>::Skip( ULONG cBase )
{
    ULONG cFetched;
    return Next( cBase, 0, &cFetched );
}

template<class IEnum, class BaseType>
STDMETHODIMP ConstrictedEnum<IEnum, BaseType>::Skip( ULONG cBase, ULONG * pcCount )
{
	return Next( cBase, 0, pcCount );
}



// CImpIEnumFilters

BOOL CImpIEnumFilters::Select( IBaseFilter ** ppIFilter )
{
    ASSERT( ppIFilter );
   	void * this_interface;
    const BOOL result = SUCCEEDED((*ppIFilter)->QueryInterface( m_iid, &this_interface ));
	if ( result )
	{
		(*ppIFilter)->Release();
		(*ppIFilter) = reinterpret_cast<IBaseFilter*>(this_interface);
	}
	return result;
}

STDMETHODIMP CImpIEnumFilters::Clone( IEnumFilters  ** ppEnum )
{
    return CloneAny( *this, ppEnum );
}



// CImpIEnumUnconnectedFilters

BOOL CImpIEnumUnconnectedFilters::Select( IBaseFilter ** ppIFilter )
{
    HRESULT hr;
    BOOL connected = FALSE;
    IBaseFilter & iFilter = **ppIFilter;
    IEnumPins * pIEnumPins = 0;
    hr = iFilter.EnumPins( &pIEnumPins );
    do
    {
        IPin *pIPin, *pConnectedToPin;
        ULONG fetched;
        hr = pIEnumPins->Next( 1, &pIPin, &fetched );
        if FAILED(hr) break;
        if ( fetched != 1 ) break;
        pIPin->ConnectedTo( &pConnectedToPin );
        if (pConnectedToPin)
        {
            connected = TRUE;
            pConnectedToPin->Release();
        }
        pIPin->Release();
    } while (!connected);
    pIEnumPins->Release();
    return !connected;
}

STDMETHODIMP CImpIEnumUnconnectedFilters::Clone( IEnumFilters  ** ppEnum )
{
    return CloneAny( *this, ppEnum );
}



// CImpIEnumPins

template<PIN_DIRECTION DIR>
BOOL CImpIEnumPins<DIR>::Select( IPin ** ppIPin )
{
	HRESULT hr;
	PIN_INFO PinInfo;
	hr = (*ppIPin)->QueryPinInfo( &PinInfo );
	ASSERT( SUCCEEDED(hr) );
	return PinInfo.dir == DIR;
}

template<PIN_DIRECTION DIR>
STDMETHODIMP CImpIEnumPins<DIR>::Clone( IEnumPins  ** ppEnum )
{
    return CloneAny( *this, ppEnum );
}



// CImpIEnumOverGraph

// This is the meat.  We must retrieve BaseObjects individually and only pass them back to
// our caller if the meet Select's criteria (whatever that might be).  When we've exhausted
// one filter, we call NextFilter() to go to the next one.
template<class EnumPinClass>
STDMETHODIMP CImpIEnumOverGraph<EnumPinClass>::Next( ULONG cBase, IPin ** ppBase, ULONG  *pcFetched )
{
	HRESULT hr;
			
	ULONG lcl_fetched;
	*pcFetched = 0;

    if (m_pIEnum)
    {
        m_crit_sec.Lock();
        while ( cBase > 0 )
	    {
            hr = EnumPinClass::Next( cBase, ppBase, &lcl_fetched );
		    if (FAILED(hr)) break;
		    cBase -= lcl_fetched;
		    *pcFetched += lcl_fetched;
		    ppBase += lcl_fetched;
		    if ( hr==S_FALSE )
		    {
			    hr = NextFilter();
			    if ( hr != NOERROR ) break;
		    }
	    }
        m_crit_sec.Unlock();
    }
    else hr = m_hr;
	
    return hr;
}

// This method jumps us between filters
template<class EnumPinClass>
HRESULT CImpIEnumOverGraph<EnumPinClass>::NextFilter()
{
	if (m_pIEnum)
	{
    	HRESULT hr;
		hr = m_pIEnum->Release();
		ASSERT( SUCCEEDED(hr) );
		m_pIEnum = 0;
	}

	IBaseFilter * pIFilter;
	ULONG lcl_count;
	m_hr = m_pIEnumFilters->Next( 1, &pIFilter, &lcl_count );
	ASSERT( SUCCEEDED(m_hr) );
	if ( m_hr == NOERROR )
	{
		ASSERT( lcl_count == 1 );
		m_hr = pIFilter->EnumPins( &m_pIEnum );
		ASSERT( SUCCEEDED(m_hr) );
		pIFilter->Release();
	}
	return m_hr;
}

template<class EnumPinClass>
STDMETHODIMP CImpIEnumOverGraph<EnumPinClass>::Reset( void )
{
	m_hr = m_pIEnumFilters->Reset();
	if SUCCEEDED(m_hr) NextFilter();
	return m_hr;
}

template<class EnumPinClass>
STDMETHODIMP CImpIEnumOverGraph<EnumPinClass>::Clone( IEnumPins ** ppEnum )
{
    return CloneAny( *this, ppEnum );
}
