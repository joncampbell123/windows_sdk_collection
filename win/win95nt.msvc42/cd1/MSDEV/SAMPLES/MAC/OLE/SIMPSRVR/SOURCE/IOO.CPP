#include "hdrs.h"

ASSERTDATA


COleObject::COleObject(CDocument * pDoc)
{
	ENTER_INTERFACE("COleObject::COleObject");

	m_cRef      = 0;
	m_fOpen     = false;
	m_pDocument = pDoc;
}


COleObject::~COleObject(void)
{
	ENTER_INTERFACE("COleObject::~COleObject");
}


HRESULT STDMETHODCALLTYPE COleObject::QueryInterface(REFIID riid, void * * ppv)
{
	ENTER_INTERFACE("COleObject::QueryInterface");

	return m_pDocument->QueryInterface(riid, ppv);
}


unsigned long STDMETHODCALLTYPE COleObject::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("COleObject::AddRef",m_cRef);

	return m_pDocument->AddRef();
}


unsigned long STDMETHODCALLTYPE COleObject::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("COleObject::Release",m_cRef);

	return m_pDocument->Release();
}


HRESULT STDMETHODCALLTYPE COleObject::SetClientSite(LPOLECLIENTSITE pClientSite)
{
	ENTER_INTERFACE("COleObject::SetClientSite");

	if (m_pDocument->m_OleClientSite)
		m_pDocument->m_OleClientSite->Release();

	m_pDocument->m_OleClientSite = pClientSite;

	if (m_pDocument->m_OleClientSite)
		m_pDocument->m_OleClientSite->AddRef();

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::GetClientSite(LPOLECLIENTSITE * ppClientSite)
{
	ENTER_INTERFACE("COleObject::GetClientSite");

	*ppClientSite = m_pDocument->m_OleClientSite;

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::SetHostNames(const char * szContainerApp,
								 const char * szContainerObj)
{
	char	windowName[256];

	ENTER_INTERFACE("COleObject::SetHostNames");

	sprintf(windowName, "%s - %s", szContainerApp, szContainerObj);
	SetWTitle(m_pDocument->m_pWindow, c2pstr(windowName));

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::Close(unsigned long /* dwSaveOption */)
{
	ENTER_INTERFACE("COleObject::Close");

	gApp->AddRef();
	AddRef();

	m_pDocument->HandleClose();

	// release artificial AddRef above
	Release();
	gApp->Release();

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::SetMoniker(unsigned long /* dwWhichMoniker */, LPMONIKER /* pmk */)
{
	ENTER_INTERFACE("COleObject::SetMoniker");

	return ResultFromScode(E_NOTIMPL);
}


HRESULT STDMETHODCALLTYPE COleObject::GetMoniker(unsigned long /* dwAssign */, 
										unsigned long /* dwWhichMoniker */, 
										LPMONIKER * /* ppmk */)
{
	ENTER_INTERFACE("COleObject::GetMoniker");

	return ResultFromScode(E_NOTIMPL);
}


HRESULT STDMETHODCALLTYPE COleObject::InitFromData(LPDATAOBJECT /* pDataObject */,
    							  unsigned long /* fCreation */,
                				  unsigned long /* dwReserved */)
{
	ENTER_INTERFACE("COleObject::InitFromData");

	return ResultFromScode(S_FALSE);
}


HRESULT STDMETHODCALLTYPE COleObject::GetClipboardData(unsigned long /* dwReserved */, 
        							 LPDATAOBJECT * ppDataObject)
{
	ENTER_INTERFACE("COleObject::GetClipboardData");

	*ppDataObject = NULL;
	return ResultFromScode(E_NOTIMPL);
}


HRESULT STDMETHODCALLTYPE COleObject::DoVerb(long iVerb,
                			EventRecord * /* lpmsg */, 
                			LPOLECLIENTSITE pActiveSite, 
                			long /* lindex */,
                			WindowPtr /* hwndParent */,
                			LPCRECT /* lprcPosRect */)
{
	ENTER_INTERFACE("COleObject::DoVerb");

	switch (iVerb)
	{
		default:
			if (iVerb < 0)
				return ResultFromScode(OLEOBJ_S_INVALIDVERB);

			// intentionally fall through...
		
		case OLEIVERB_SHOW:
		case OLEIVERB_PRIMARY:
		case OLEIVERB_OPEN:
			OpenEdit(pActiveSite);
			break;

		case OLEIVERB_HIDE:
			m_pDocument->HideDocument();
			break;
	}

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::EnumVerbs(LPENUMOLEVERB * /* ppenumOleVerb */)
{
	ENTER_INTERFACE("COleObject::EnumVerbs");

	return ResultFromScode(OLE_S_USEREG);
}


HRESULT STDMETHODCALLTYPE COleObject::Update(void)
{
	ENTER_INTERFACE("COleObject::Update");

	m_pDocument->OleSendOnDataChange();

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::IsUpToDate(void)
{
	ENTER_INTERFACE("COleObject::IsUpToDate");

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::GetUserClassID(CLSID * pClsid)
{
	ENTER_INTERFACE("COleObject::GetUserClassID");

	m_pDocument->m_PersistStorage->GetClassID(pClsid);

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE COleObject::GetUserType(unsigned long /* dwFormOfType */, char * * /* pszUserType */)
{
	ENTER_INTERFACE("COleObject::GetUserType");

	return ResultFromScode(OLE_S_USEREG);
}


HRESULT STDMETHODCALLTYPE COleObject::SetExtent(unsigned long /* dwDrawAspect */, LPSIZEL /* lpsizel */)
{
	ENTER_INTERFACE("COleObject::SetExtent");

	return ResultFromScode(E_NOTIMPL);
}


HRESULT STDMETHODCALLTYPE COleObject::GetExtent(unsigned long dwDrawAspect, LPSIZEL lpsizel)
{
	ENTER_INTERFACE("COleObject::GetExtent");

	// we only support DVASPECT_CONTENT
	if (dwDrawAspect == DVASPECT_CONTENT)
	{
		lpsizel->cx = m_pDocument->m_sizel.cx;
		lpsizel->cy = m_pDocument->m_sizel.cy;

		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_FAIL);
}


HRESULT STDMETHODCALLTYPE COleObject::Advise(LPADVISESINK pAdvSink, unsigned long * pdwConnection)
{
	HRESULT 	hr;

	ENTER_INTERFACE("COleObject::Advise");

	if (!m_pDocument->m_OleAdviseHolder)
	{
		hr = CreateOleAdviseHolder(&m_pDocument->m_OleAdviseHolder);
		FailScode("\pCreateOleAdviseHolder()", GetScode(hr));
	}

	if (m_pDocument->m_OleAdviseHolder)
		return m_pDocument->m_OleAdviseHolder->Advise(pAdvSink, pdwConnection);
	else
		return hr;
}


HRESULT STDMETHODCALLTYPE COleObject::Unadvise(unsigned long dwConnection)
{
	ENTER_INTERFACE("COleObject::Unadvise");

	ASSERT(m_pDocument->m_OleAdviseHolder);
	return m_pDocument->m_OleAdviseHolder->Unadvise(dwConnection);
}


HRESULT STDMETHODCALLTYPE COleObject::EnumAdvise(LPENUMSTATDATA * ppenumAdvise)
{
	ENTER_INTERFACE("COleObject::EnumAdvise");

	*ppenumAdvise = NULL;

	ASSERT(m_pDocument->m_OleAdviseHolder);
	return m_pDocument->m_OleAdviseHolder->EnumAdvise(ppenumAdvise);
}


HRESULT STDMETHODCALLTYPE COleObject::GetMiscStatus(unsigned long /* dwAspect */, unsigned long * pdwStatus)
{
	ENTER_INTERFACE("COleObject::GetMiscStatus");

	*pdwStatus = NULL;

	return ResultFromScode(OLE_S_USEREG);
}


HRESULT STDMETHODCALLTYPE COleObject::SetColorScheme(LPOLECOLORSCHEME /* lpColorScheme */)
{
	ENTER_INTERFACE("COleObject::SetColorScheme");

	return ResultFromScode(S_OK);
}


void COleObject::OpenEdit(LPOLECLIENTSITE /* pActiveSite */)
{
	m_pDocument->ShowDocument();

	gApp->BringToForeground(false);

	gApp->SelectTheWindow(m_pDocument->m_pWindow);
}
