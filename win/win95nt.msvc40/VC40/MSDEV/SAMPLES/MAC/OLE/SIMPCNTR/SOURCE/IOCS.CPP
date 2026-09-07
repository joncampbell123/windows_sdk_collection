#include "hdrs.h"

ASSERTDATA


COleClientSite::COleClientSite(void)
{
	m_refCount    = 0;
}


COleClientSite::~COleClientSite(void)
{
}


void COleClientSite::InitClientSite(CObjectSite * pObjectSite)
{
	m_pObjectSite = pObjectSite;
}


HRESULT	COleClientSite::QueryInterface(REFIID riid, void * * ppvObj)
{
	return m_pObjectSite->QueryInterface(riid, ppvObj);
}


unsigned long COleClientSite::AddRef()
{
	++m_refCount;
	
	return m_pObjectSite->AddRef();
}


unsigned long COleClientSite::Release()
{
	--m_refCount;
	
	return m_pObjectSite->Release();
}


HRESULT COleClientSite::SaveObject()
{	
	LPPERSISTSTORAGE	lpPS;
	HRESULT				hresult;
	LPOLEOBJECT			pObj;
	
	ASSERT(m_pObjectSite->m_pObject != NULL);
	
	pObj = m_pObjectSite->m_pObject;
	
	hresult = pObj->QueryInterface(IID_IPersistStorage, (void * *)&lpPS);
	
	ASSERTCOND(GetScode(hresult) == S_OK);
	
	if (hresult==NOERROR)
	{	
		ASSERT(m_pObjectSite->m_pObjectStg != NULL);
		ASSERT(m_pObjectSite->m_pRootStg   != NULL);
	
		hresult=OleSave(lpPS, m_pObjectSite->m_pObjectStg, true);
		
		FailScode("\pOleSave()", GetScode(hresult));
		
		if ( hresult == NOERROR )
		{		
			hresult=lpPS->SaveCompleted(m_pObjectSite->m_pObjectStg);
			
			FailScode("\pSaveCompleted()", GetScode(hresult));
		}
		
		lpPS->Release();
	}

	return ResultFromScode(S_OK);
}


HRESULT COleClientSite::GetMoniker(unsigned long /* dwAssign */,
										unsigned long /* dwWhichMoniker */, LPMONIKER *ppmk)
{	
	*ppmk=NULL;
	
	return ResultFromScode(E_NOTIMPL);
}


HRESULT	COleClientSite::GetContainer(LPOLECONTAINER * ppContainer)
{
	*ppContainer=NULL;
	
	return ResultFromScode(E_NOTIMPL);
}


HRESULT	COleClientSite::ShowObject()
{
	return NOERROR;
}


HRESULT	COleClientSite::OnShowWindow(unsigned long fShow)
{
	if (fShow)
	{	
		m_pObjectSite->m_fObjectOpen = true;
		SetPort(m_pObjectSite->m_pWindowObjSite);
		InvalRect(&m_pObjectSite->m_pWindowObjSite->portRect);
	}
	else
	{
		gApp->BringToForeground(true);
		m_pObjectSite->m_fObjectOpen = false;
		SetPort(m_pObjectSite->m_pWindowObjSite);
		InvalRect(&m_pObjectSite->m_pWindowObjSite->portRect);
	}

	return ResultFromScode(S_OK);
}


HRESULT	COleClientSite::RequestNewObjectLayout()
{
	return ResultFromScode(E_NOTIMPL);
}


void COleClientSite::Delete(void)
{
	
}

