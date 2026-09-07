// Behavior.h : Declaration of the CBehavior

#ifndef __BEHAVIOR_H_
#define __BEHAVIOR_H_

#include "resource.h"       // main symbols
#include "EventSink.h"

/////////////////////////////////////////////////////////////////////////////
// CBehavior
class ATL_NO_VTABLE CBehavior : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBehavior, &CLSID_Behavior>,
	public IDispatchImpl<IBehavior, &IID_IBehavior, &LIBID_ATLBEHAVELib>,
	public IElementBehavior
{
public:
	CComPtr<IElementBehaviorSite>	m_spSite;
	CComPtr<IElementBehaviorSiteOM>	m_spOMSite;
	CComPtr<IHTMLElement>			m_spElem;
	CComObject<CEventSink>*			m_pEventSink;
	DWORD							m_dwCookie;
	CComVariant						m_varColor;
	CComVariant						m_varBackColor;
	long							m_iTextColorIndex;
	long							m_iBackColorIndex;
	static char*					m_szMethodNames[];
	static DISPID					m_dispidMethodIDs[];

	CBehavior()
	{
		m_pEventSink = NULL;
		m_dwCookie = 0;
		m_iTextColorIndex = COLOR_HIGHLIGHTTEXT;
		m_iBackColorIndex = COLOR_HIGHLIGHT;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BEHAVIOR)
DECLARE_NOT_AGGREGATABLE(CBehavior)

BEGIN_COM_MAP(CBehavior)
	COM_INTERFACE_ENTRY(IBehavior)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IElementBehavior)
END_COM_MAP()

// IElementBehavior
public:
	STDMETHOD(Detach)(void);
	STDMETHOD(Init)(IElementBehaviorSite* pBehaviorSite);
	STDMETHOD(Notify)(DWORD event,
					  VARIANT* pVar);

// IDispatch overrides
    STDMETHOD(GetIDsOfNames)(REFIID riid,LPOLESTR* rgszNames,UINT cNames,LCID lcid,DISPID* rgdispid);
    STDMETHOD(Invoke)(DISPID dispid,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams,VARIANT* pVarResult,EXCEPINFO* pExcepInfo,unsigned int* puArgErr);

// Helper functions
	void ShowBehavior();
	void Restore();
	void SetBkColor(long iIndex);
	void SetTextColor(long iIndex);
	void GetEventObject(CComPtr<IHTMLEventObj>& event);
};

#endif //__BEHAVIOR_H_
