// EventSink.cpp : Implementation of CEventSink
#include "stdafx.h"
#include "AtlBehave.h"
#include "Behavior.h"
#include "EventSink.h"

/////////////////////////////////////////////////////////////////////////////
// CEventSink

	STDMETHODIMP
CEventSink::Invoke(
	DISPID dispidMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS* pdispparams,
	VARIANT* pvarResult,
	EXCEPINFO* pexcepinfo,
	UINT* puArgErr)
{
	switch ( dispidMember )
	{
	case DISPID_HTMLELEMENTEVENTS_ONMOUSEOVER:
			OnMouseOver();
		break;

	case DISPID_HTMLELEMENTEVENTS_ONMOUSEOUT:
			OnMouseOut();
		break;

	default:
		break;
	}

	return S_OK;
}

// Event handlers
	void
CEventSink::OnMouseOver()
{
	if ( m_pBehavior )
		m_pBehavior->ShowBehavior();
}

	void
CEventSink::OnMouseOut()
{
	if ( m_pBehavior )
		m_pBehavior->Restore();
}
