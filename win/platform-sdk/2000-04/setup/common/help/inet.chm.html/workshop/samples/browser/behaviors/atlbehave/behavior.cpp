// Behavior.cpp : Implementation of CBehavior
#include "stdafx.h"
#include "AtlBehave.h"
#include "EventSink.h"
#include "Behavior.h"

/////////////////////////////////////////////////////////////////////////////
// CBehavior

#define DISPID_BKCOLOR 1
#define DISPID_TEXTCOLOR 2

char* CBehavior::m_szMethodNames[] = {"backColor","textColor",NULL};
DISPID CBehavior::m_dispidMethodIDs[] = {DISPID_BKCOLOR,DISPID_TEXTCOLOR,-1};


// IElementBehavior implemenation

	STDMETHODIMP
CBehavior::Detach(
	void)
{
	// Release cached interface pointers
	m_pEventSink->Release();
	m_spElem.Release();
	m_spOMSite.Release();

	return S_OK;
}

	STDMETHODIMP
CBehavior::Init(
	IElementBehaviorSite* pBehaviorSite)
{
HRESULT	hr;

	// Cache the IElementBehaviorSite interface pointer
	m_spSite = pBehaviorSite;

	// Cache the IElementBehaviorSiteOM interface pointer
	hr = m_spSite->QueryInterface( __uuidof(IElementBehaviorSiteOM), (void**)&m_spOMSite );

	return S_OK;
}

	STDMETHODIMP
CBehavior::Notify(
	DWORD event,
	VARIANT* pVar)
{
HRESULT					hr=S_OK;
CComPtr<IHTMLElement>	spElem;

	switch ( event )
	{
	case BEHAVIOREVENT_CONTENTCHANGE:	// End tag of element has been parsed (we can get at attributes)
		break;

	case BEHAVIOREVENT_DOCUMENTREADY:	// HTML document has been parsed (we can get at the document object model)
		if ( m_spSite )
		{
			hr = m_spSite->GetElement( &m_spElem );
			if ( SUCCEEDED(hr) )
			{
				// Create and connect our event sink
				hr = CComObject<CEventSink>::CreateInstance( &m_pEventSink );
				if ( SUCCEEDED(hr) )
				{
				CComPtr<IHTMLStyle>	spStyle;
				HRESULT				hr;

					hr = m_spElem->get_style( &spStyle );
					if ( SUCCEEDED(hr) )
					{
						spStyle->get_color( &m_varColor );
						spStyle->get_backgroundColor( &m_varBackColor );
					}
					m_pEventSink->m_pBehavior = this;
					hr = AtlAdvise( m_spElem, m_pEventSink, DIID_HTMLElementEvents, &m_dwCookie );
				}
			}
		}

	default:
		break;
	}

	return S_OK;
}

// IDispatch overides

	HRESULT
CBehavior::GetIDsOfNames(
	REFIID riid,
	LPOLESTR* rgszNames,
	UINT cNames,
	LCID lcid,
	DISPID* rgdispid)
{
HRESULT	hr=DISP_E_UNKNOWNNAME;

USES_CONVERSION;

	if ( ! rgszNames
	  || ! rgdispid )
		return E_POINTER;

	for ( int i=0; m_szMethodNames[i]!=NULL; i++ )
	{
		if ( 0 == _tcsicmp( m_szMethodNames[i], OLE2A(*rgszNames) ) )
		{
			*rgdispid = m_dispidMethodIDs[i];
			hr = S_OK;
		}
	}

	return hr;
}

	HRESULT
CBehavior::Invoke(
	DISPID dispid, 
	REFIID riid, 
	LCID lcid, 
	WORD wFlags, 
	DISPPARAMS* pDispParams, 
	VARIANT* pVarResult, 
	EXCEPINFO* pExcepInfo, 
	unsigned int* puArgErr)
{
HRESULT	hr=DISP_E_MEMBERNOTFOUND;

    switch ( dispid )
    {
    case DISPID_BKCOLOR:
    case DISPID_TEXTCOLOR:
		if ( wFlags & DISPATCH_PROPERTYPUT )
		{
			if ( ! pDispParams )
				return E_POINTER;

			if ( 1 != pDispParams->cArgs
			  || ! pDispParams->rgvarg
			  || VT_I4 != V_VT(pDispParams->rgvarg) )
				return E_INVALIDARG;

			if ( dispid == DISPID_BKCOLOR )
				SetBkColor( V_I4(pDispParams->rgvarg) );
			else
				SetTextColor( V_I4(pDispParams->rgvarg) );
			hr = S_OK;
		}
		else
			if ( wFlags & DISPATCH_PROPERTYGET ) 
			{
				if ( ! pVarResult )
					return E_POINTER;
				VariantInit( pVarResult );
				V_VT(pVarResult) = VT_I4;
				if ( dispid == DISPID_BKCOLOR )
					V_I4(pVarResult) = m_iBackColorIndex;
				else
					V_I4(pVarResult) = m_iTextColorIndex;
				hr = S_OK;
			}
		break;
	default:
		break;
    }

    return hr;
}

// Helper functions

	void
CBehavior::ShowBehavior()
{
CComPtr<IHTMLStyle>	spStyle;
HRESULT				hr;

	hr = m_spElem->get_style( &spStyle );
	if ( SUCCEEDED(hr) )
	{
	DWORD	color;
	char	buf[8];
		color = GetSysColor( m_iTextColorIndex );
		wsprintf( buf, "#%02x%02x%02x", GetRValue(color), GetGValue(color), GetBValue(color) );
		spStyle->put_color( CComVariant(buf) );

		color = GetSysColor( m_iBackColorIndex );
		wsprintf( buf, "#%02x%02x%02x", GetRValue(color), GetGValue(color), GetBValue(color) );
		spStyle->put_backgroundColor( CComVariant(buf) );
	}
}

	void
CBehavior::Restore()
{
CComPtr<IHTMLStyle>		spStyle;
HRESULT					hr;

	hr = m_spElem->get_style( &spStyle );
	if ( SUCCEEDED(hr) )
	{
		spStyle->put_color( m_varColor );
		spStyle->put_backgroundColor( m_varBackColor );
	}
}

inline
	void
CBehavior::SetTextColor(long iIndex)
{
	m_iTextColorIndex = iIndex;
}

inline
	void
CBehavior::SetBkColor(long iIndex)
{
	m_iBackColorIndex = iIndex;
}

	void
CBehavior::GetEventObject(CComPtr<IHTMLEventObj>& event)
// Use this function when you want to access the window.event object
// Unused in this sample
{
CComPtr<IDispatch>		spDisp;
CComPtr<IHTMLWindow2>	spWindow;
HRESULT					hr;

	hr = m_spElem->get_document( &spDisp );
	if ( SUCCEEDED(hr) )
	{
		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if ( spDoc )
		{
			spDoc->get_parentWindow( &spWindow );
			if ( spWindow )
				spWindow->get_event( &event );
		}
	}

	return;
}
