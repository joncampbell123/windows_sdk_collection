#include "hdrs.h"

ASSERTDATA


CDataObject::CDataObject(CDocument * pDoc)
{
	ENTER_INTERFACE("CDataObject::CDataObject");

	m_cRef      = 0;
	m_pDocument = pDoc;
}


CDataObject::~CDataObject(void)
{
	ENTER_INTERFACE("CDataObject::~CDataObject");
}


HRESULT STDMETHODCALLTYPE CDataObject::QueryInterface(REFIID riid, void * * ppv)
{
	ENTER_INTERFACE("CDataObject::QueryInterface");

	return m_pDocument->QueryInterface(riid, ppv);
}


unsigned long STDMETHODCALLTYPE CDataObject::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CDataObject::AddRef",m_cRef);

	return m_pDocument->AddRef();
}


unsigned long STDMETHODCALLTYPE CDataObject::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CDataObject::Release",m_cRef);

	return m_pDocument->Release();
}


HRESULT STDMETHODCALLTYPE CDataObject::GetData(LPFORMATETC pformatetcIn, LPSTGMEDIUM pmedium)
{
	ENTER_INTERFACE("CDataObject::GetData");

	if ((pformatetcIn->cfFormat == cfPict) &&
		(pformatetcIn->dwAspect == DVASPECT_CONTENT) &&
		(pformatetcIn->tymed == TYMED_MFPICT))
	{
		PicHandle hPict = m_pDocument->OleGetPict();

		if (!hPict)
			return ResultFromScode(E_FAIL);

		pmedium->tymed = TYMED_MFPICT;
		pmedium->hGlobal = (Handle)hPict;
		pmedium->pUnkForRelease = NULL;

		return ResultFromScode(S_OK);
	}

	return ResultFromScode(DATA_E_FORMATETC);
}


HRESULT STDMETHODCALLTYPE CDataObject::GetDataHere(LPFORMATETC /* pformatetc */, LPSTGMEDIUM /* pmedium */)
{
	ENTER_INTERFACE("CDataObject::GetDataHere");

	return ResultFromScode(DATA_E_FORMATETC);
}


HRESULT STDMETHODCALLTYPE CDataObject::QueryGetData(LPFORMATETC pformatetc)
{
	SCODE	sc = DATA_E_FORMATETC;

	ENTER_INTERFACE("CDataObject::QueryGetData");

	if ((pformatetc->cfFormat == cfPict) &&
		(pformatetc->dwAspect == DVASPECT_CONTENT) &&
		(pformatetc->tymed == TYMED_MFPICT))
		sc = S_OK;

	return ResultFromScode(sc);
}


HRESULT STDMETHODCALLTYPE CDataObject::GetCanonicalFormatEtc(LPFORMATETC pformatetc,
                            			   LPFORMATETC pformatetcOut)
{
	ENTER_INTERFACE("CDataObject::GetCanonicalFormatEtc");

	HRESULT	hr;

	if (!pformatetcOut)
		return ResultFromScode(E_INVALIDARG);

	pformatetcOut->ptd = NULL;

	if (!pformatetc)
		return ResultFromScode(E_INVALIDARG);

	if ((hr = QueryGetData(pformatetc)) != NOERROR)
		return hr;

	*pformatetcOut = *pformatetc;

	if (pformatetc->ptd == NULL)
		return ResultFromScode(DATA_S_SAMEFORMATETC);
	else
	{
		pformatetcOut->ptd = NULL;
		return NOERROR;
	}
}


HRESULT STDMETHODCALLTYPE CDataObject::SetData(LPFORMATETC /* pformatetc */, 
    						 STGMEDIUM  * /* pmedium */,
                             unsigned long /* fRelease */)
{
	ENTER_INTERFACE("CDataObject::SetData");

	return ResultFromScode(DATA_E_FORMATETC);
}


HRESULT STDMETHODCALLTYPE CDataObject::EnumFormatEtc(unsigned long /* dwDirection */,
                            	   LPENUMFORMATETC * ppenumFormatEtc)
{
	ENTER_INTERFACE("CDataObject::EnumFormatEtc");

	*ppenumFormatEtc = NULL;

	return ResultFromScode(OLE_S_USEREG);
}


HRESULT STDMETHODCALLTYPE CDataObject::DAdvise(FORMATETC * pFormatetc, 
    						 unsigned long advf, 
                    		 LPADVISESINK pAdvSink, 
                    		 unsigned long * pdwConnection)
{
	HRESULT		hr;

	ENTER_INTERFACE("CDataObject::DAdvise");

	if (!m_pDocument->m_DataAdviseHolder)
	{
		hr = CreateDataAdviseHolder(&m_pDocument->m_DataAdviseHolder);
		FailScode("\pCreateDataAdviseHolder()", GetScode(hr));
	}

	if (GetScode(hr) == S_OK)
		return m_pDocument->m_DataAdviseHolder->Advise(this, pFormatetc, advf, pAdvSink, pdwConnection);
	else
		return hr;
}


HRESULT STDMETHODCALLTYPE CDataObject::DUnadvise(unsigned long dwConnection)
{
	ENTER_INTERFACE("CDataObject::DUnadvise");

	ASSERT(m_pDocument->m_DataAdviseHolder);
	return m_pDocument->m_DataAdviseHolder->Unadvise(dwConnection);
}


HRESULT STDMETHODCALLTYPE CDataObject::EnumDAdvise(LPENUMSTATDATA * ppenumAdvise)
{
	ENTER_INTERFACE("CDataObject::EnumDAdvise");

	*ppenumAdvise = NULL;

	ASSERT(m_pDocument->m_DataAdviseHolder);
	return m_pDocument->m_DataAdviseHolder->EnumAdvise(ppenumAdvise);
}
