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
//
// Ext_Enum.h
//
//
// Contents:
//

#ifndef _Ext_Enum_h
#define _Ext_Enum_h

#pragma warning( disable : 4514 )

// A helper template & macro so that we only release non-null interface
// pointers and we set the pointer to null afterwards so that we don't
// release it again.
template<class Interface> void SafeRelPtrIPtr( Interface * * ppIUnk )
{
    if ( *ppIUnk )
    {
        (*ppIUnk)->Release();
        (*ppIUnk) = 0;
    }
}

#define SafeRelIPtr( pI ) SafeRelPtrIPtr( &(pI) )

// A template function for cloning.  We always clone an Implementation, but
// supply a pointer to an Interface.  We need both to parameterize this function.
template<class Implementation, class Interface>
static inline HRESULT CloneAny( const Implementation & _this, Interface ** ppI)
{
    HRESULT hr;

    if (ppI)
    {
        *ppI = 0;
        // Relies on Implementation having defined this kind of "copy" constructor.
        Implementation *const p = new Implementation( _this, &hr );
        if (p)
        {
            if SUCCEEDED(hr)    *ppI = p;
            else                delete p;
        }
        else hr = E_OUTOFMEMORY;
    }
    else hr = E_POINTER;
    return hr;
}

// AnyEnum presents a simple concrete class that can cover any enumerator that conforms to
// the standard COM enumerator interface.  You'll probably never instantiate one of these,
// but it gives us a good foundation from which to build derived enumerators with special
// behaviour.

template<class IEnum, class BaseType> class AnyEnum : public IEnum
{
protected:
	long    m_cRef;                             // Local ref count
	IEnum * m_pIEnum;                           // Pointer to a "real" enumarator that will do most of the work

	// Protected default constructor, derived classes relying on this must set m_pIEnum themselves.
    AnyEnum()                                   : m_cRef(1), m_pIEnum(0)  {}

public:
    virtual ~AnyEnum()                          { SafeRelIPtr( m_pIEnum ); }
    AnyEnum( const AnyEnum & copy, HRESULT * phr );

    // IUnknown interface
	STDMETHODIMP            QueryInterface( REFIID iid, void ** ppv );
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

    // IEnum interface
    STDMETHODIMP            Next( ULONG cBase, BaseType ** ppBase, ULONG * pcFetched );
    STDMETHODIMP            Skip( ULONG cBase );
	STDMETHODIMP            Reset( void );
	STDMETHODIMP            Clone( IEnum ** ppEnum );
};


// ConstrictedEnum
//
// An abstract template class extending AnyEnum by adding a Select() method.
// Derived classes must implement this method.  For each object that our inner
// enumerator passes back to us, we ask Select() if it meets its selection
// criteria, if it does, we pass the object back to our caller, otherwise we
// ask our inner enumerator for another candidate.
//
// Next() is enhanced to call Select() for each candidate.  Skip() is re-implemented
// since its skip count must now be in terms of objects that meet Select()'s criteria.
// A new Skip() is implemented where we can explicity get a count of the number of
// candidates skipped.

template<class IEnum, class BaseType> class ConstrictedEnum : public AnyEnum<IEnum,BaseType>
{
protected:
    ConstrictedEnum()                       {}

    // Pure virtual Selector method.
	virtual BOOL Select( BaseType ** ppBase ) =0;

public:
    ConstrictedEnum( const ConstrictedEnum & copy, HRESULT * phr )
    : AnyEnum<IEnum,BaseType>( copy, phr )  {}

    STDMETHODIMP            Next( ULONG cBase, BaseType ** ppBase, ULONG * pcFetched );
	STDMETHODIMP            Skip( ULONG cBase );
	STDMETHODIMP            Skip( ULONG cBase, ULONG * pcCount );
};


// CImpIEnumFilters
//
// This class not only restricts ennumeration to IBaseFilters that support a specific interface,
// it also returns pointers to THAT interface, rather than just IBaseFilter pointers.

class CImpIEnumFilters : public ConstrictedEnum<IEnumFilters, IBaseFilter>
{
private:
	REFIID	m_iid;

protected:
	CImpIEnumFilters( REFIID iid )                  : m_iid(iid)                {}
    BOOL	Select( IBaseFilter ** ppIFilter );

public:
    CImpIEnumFilters( const CImpIEnumFilters & copy, HRESULT * phr )
    : ConstrictedEnum<IEnumFilters,IBaseFilter>( copy, phr ), m_iid( copy.m_iid )   {}

    CImpIEnumFilters( IFilterGraph * pIFilterGraph, REFIID iid )
	: m_iid(iid)
	{
		HRESULT hr = pIFilterGraph->EnumFilters( &m_pIEnum );
		ASSERT( SUCCEEDED(hr) );
	}

	STDMETHODIMP Clone( IEnumFilters  ** ppEnum );
};


// CImpIEnumUnconnectedFilters
//
// Enumerates filters over a filter graph, but only returns those who have none of their pins connected

class CImpIEnumUnconnectedFilters : public ConstrictedEnum<IEnumFilters, IBaseFilter>
{
protected:
    BOOL	Select( IBaseFilter ** ppIFilter );

public:
    CImpIEnumUnconnectedFilters( const CImpIEnumUnconnectedFilters & copy, HRESULT * phr )
    : ConstrictedEnum<IEnumFilters,IBaseFilter>( copy, phr ) {}

    CImpIEnumUnconnectedFilters( IFilterGraph * pIFilterGraph )
	{
		HRESULT hr = pIFilterGraph->EnumFilters( &m_pIEnum );
		ASSERT( SUCCEEDED(hr) );
	}

    STDMETHODIMP Clone( IEnumFilters  ** ppEnum );
};


template<PIN_DIRECTION DIR> class CImpIEnumPins : public ConstrictedEnum<IEnumPins, IPin>
{
protected:
	CImpIEnumPins()                                                 {}
    BOOL	Select( IPin ** ppIPin );

public:
	CImpIEnumPins( const CImpIEnumPins & copy, HRESULT * phr )
    : ConstrictedEnum<IEnumPins, IPin>( copy, phr )                 {}

    CImpIEnumPins( IBaseFilter * pIFilter )
	{
		HRESULT hr = pIFilter->EnumPins( &m_pIEnum );
		ASSERT ( SUCCEEDED(hr) );
	}

	STDMETHODIMP Clone( IEnumPins  ** ppEnum );
};

typedef CImpIEnumPins<PINDIR_INPUT>     CImpIEnumPinsIn;
typedef CImpIEnumPins<PINDIR_OUTPUT>    CImpIEnumPinsOut;
typedef AnyEnum<IEnumPins, IPin>        CImpIEnumPinsBoth;

template<class EnumPinClass> class CImpIEnumOverGraph : public EnumPinClass
{
private:
	CImpIEnumOverGraph()            {}
protected:
    HRESULT         m_hr;
	IEnumFilters	* m_pIEnumFilters;
    CCritSec        m_crit_sec;


    HRESULT NextFilter();           // Iterates us through the filters

public:
	~CImpIEnumOverGraph()           { SafeRelIPtr(m_pIEnumFilters); }

    CImpIEnumOverGraph( const CImpIEnumOverGraph & copy, HRESULT * phr )
    : EnumPinClass( copy, phr )
    {
        if SUCCEEDED(*phr)
        {
            *phr = copy.m_pIEnumFilters->Clone( &m_pIEnumFilters );
            m_hr = SUCCEEDED(*phr) ? copy.m_hr : *phr;
        }
        else m_pIEnumFilters = 0;
    }

    CImpIEnumOverGraph( IFilterGraph * pIFilterGraph )
	{
		HRESULT hr;
		hr = pIFilterGraph->EnumFilters( &m_pIEnumFilters );
		ASSERT( SUCCEEDED(hr) );
		hr = NextFilter();
		ASSERT( SUCCEEDED(hr) );
	}

	STDMETHODIMP Next( ULONG cBase, IPin ** ppBase, ULONG * pcFetched );
	STDMETHODIMP Reset( void );
	STDMETHODIMP Clone( IEnumPins ** ppEnum );
};


typedef CImpIEnumOverGraph<CImpIEnumPinsIn>     CImpIEnumPinsInOverGraph;
typedef CImpIEnumOverGraph<CImpIEnumPinsOut>    CImpIEnumPinsOutOverGraph;
typedef CImpIEnumOverGraph<CImpIEnumPinsBoth>   CImpIEnumPinsOverGraph;

#include "Ext_Enum.inl"

#endif
