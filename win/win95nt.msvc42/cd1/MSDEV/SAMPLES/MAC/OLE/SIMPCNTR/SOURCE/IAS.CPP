#include "hdrs.h"

// ASSERTDATA

CAdviseSink::CAdviseSink(void)
{
	m_refCount = 0;          
}


CAdviseSink::~CAdviseSink(void)
{
}


HRESULT	CAdviseSink::QueryInterface(REFIID riid, void * * ppv)
{
	return m_pObjectSite->QueryInterface(riid, ppv);
}


unsigned long CAdviseSink::AddRef()
{
	++m_refCount;
	
	return m_pObjectSite->AddRef();
}


unsigned long CAdviseSink::Release()
{
	--m_refCount;
	
	return m_pObjectSite->Release();
}


void CAdviseSink::InitAdviseSink(CObjectSite * pObjectSite)
{
	m_pObjectSite = pObjectSite;
}


void CAdviseSink::OnDataChange(FORMATETC * /* pFormatetc */, STGMEDIUM * /* pStgmed */)
{
}


void CAdviseSink::OnViewChange(unsigned long /* dwAspect */, long /* lindex */)
{	
	GrafPtr		theOldPort;
	LPOLEOBJECT	pObj;
	WindowPtr	pWindow;
	SIZEL		sizel;
	
	pWindow = m_pObjectSite->m_pWindowObjSite;
	pObj    = m_pObjectSite->m_pObject;
	sizel	= m_pObjectSite->m_ObjSizeL;
	
	GetPort(&theOldPort);	
	SetPort(pWindow);
	
	InvalRect(&pWindow->portRect);

	SetPort(theOldPort);
}


void CAdviseSink::OnRename(LPMONIKER /* pmk */)
{
}


void CAdviseSink::OnSave()
{
}


void CAdviseSink::OnClose()
{
}


void CAdviseSink::Delete(void)
{
}
