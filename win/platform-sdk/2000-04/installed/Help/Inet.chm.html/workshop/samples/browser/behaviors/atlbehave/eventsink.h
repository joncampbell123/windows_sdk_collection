// EventSink.h : Declaration of the CEventSink

#ifndef __EVENTSINK_H_
#define __EVENTSINK_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEventSink

class CBehavior;

class ATL_NO_VTABLE CEventSink : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEventSink, &CLSID_EventSink>,
	public IDispatchImpl<IEventSink, &IID_IEventSink, &LIBID_ATLBEHAVELib>
{
public:
	CBehavior*	m_pBehavior;

	CEventSink()
	{
		m_pBehavior = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EVENTSINK)
DECLARE_NOT_AGGREGATABLE(CEventSink)

BEGIN_COM_MAP(CEventSink)
	COM_INTERFACE_ENTRY2(HTMLElementEvents,IEventSink)
	COM_INTERFACE_ENTRY(IEventSink)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IDispatchImpl override
	STDMETHOD(Invoke)(DISPID dispidMember,
					  REFIID riid,
					  LCID lcid,
					  WORD wFlags,
					  DISPPARAMS* pdispparams,
					  VARIANT* pvarResult,
					  EXCEPINFO* pexcepinfo,
					  UINT* puArgErr);
// Event Handlers
public:
	void OnMouseOver();
	void OnMouseOut();
};

#endif //__EVENTSINK_H_
