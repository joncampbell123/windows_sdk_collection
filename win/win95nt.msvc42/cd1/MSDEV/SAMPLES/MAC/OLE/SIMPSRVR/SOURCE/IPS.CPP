#include "hdrs.h"

ASSERTDATA

#undef DEFINE_OLEGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID name = { l, w1, w2, b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 }

#define DEFINE_OLEGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)


DEFINE_OLEGUID(CLSID_SimpSrvr, 0x00000410L, 0, 0);
#undef DEFINE_OLEGUID



CPersistStorage::CPersistStorage(CDocument * pDoc)
{
	ENTER_INTERFACE("CPersistStorage::CPersistStorage");

	m_cRef      = 0;
	m_pDocument = pDoc;
}


CPersistStorage::~CPersistStorage(void)
{
	ENTER_INTERFACE("CPersistStorage::~CPersistStorage");

	ReleaseStreamsAndStorage();
}


HRESULT STDMETHODCALLTYPE CPersistStorage::QueryInterface(REFIID riid, void * * ppv)
{
	ENTER_INTERFACE("IPersistStorage::QueryInterface");

	return m_pDocument->QueryInterface(riid, ppv);
}


unsigned long STDMETHODCALLTYPE CPersistStorage::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CPersistStorage::AddRef", m_cRef);

	return m_pDocument->AddRef();
}


unsigned long STDMETHODCALLTYPE CPersistStorage::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CPersistStorage::Release",m_cRef);

	return m_pDocument->Release();
}


HRESULT STDMETHODCALLTYPE CPersistStorage::GetClassID(LPCLSID lpClassID)
{
	ENTER_INTERFACE("IPersistStorage::GetClassID");

	*lpClassID = CLSID_SimpSrvr;

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::IsDirty(void)
{
	ENTER_INTERFACE("IPersistStorage::IsDirty");

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::InitNew(LPSTORAGE pStg)
{
	ENTER_INTERFACE("IPersistStorage::InitNew");

	ASSERT(pStg != NULL);

	ASSERT(m_pDocument->m_DocType == kNewDocumentType);
	m_pDocument->m_DocType = kEmbeddedDocumentType;

	ReleaseStreamsAndStorage();

	m_pDocument->m_lpStorage = pStg;
	m_pDocument->m_lpStorage->AddRef();

	CreateStreams();

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::Load(LPSTORAGE pStg)
{
	ENTER_INTERFACE("IPersistStorage::Load");

	ASSERT(pStg != NULL);

	ASSERT(m_pDocument->m_DocType == kNewDocumentType);
	m_pDocument->m_DocType = kEmbeddedDocumentType;

	ReleaseStreamsAndStorage();

	m_pDocument->m_lpStorage = pStg;
	m_pDocument->m_lpStorage->AddRef();

	OpenStreams();

	m_pDocument->OleLoadFromStorage();

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::Save(LPSTORAGE pStgSave, unsigned long fSameAsLoad)
{
	ENTER_INTERFACE("IPersistStorage::Save");

	if (!fSameAsLoad)
	{
		ReleaseStreamsAndStorage();

		m_pDocument->m_lpStorage = pStgSave;
		m_pDocument->m_lpStorage->AddRef();

		CreateStreams();
	}

	m_pDocument->OleSaveToStorage();

	m_pDocument->m_fSaveWithSameAsLoad = (Boolean) fSameAsLoad;
	m_pDocument->m_fNoScribbleMode     = true;

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::SaveCompleted(LPSTORAGE pStgNew)
{
	ENTER_INTERFACE("IPersistStorage::SaveCompleted");

	if (pStgNew || m_pDocument->m_fSaveWithSameAsLoad)
	{
		if (m_pDocument->m_fNoScribbleMode)
		{
			ASSERT(m_pDocument->m_OleAdviseHolder);
			m_pDocument->m_OleAdviseHolder->SendOnSave();
		}
		m_pDocument->m_fSaveWithSameAsLoad = false;
	}
	m_pDocument->m_fNoScribbleMode = false;

	return ResultFromScode(S_OK);
}


HRESULT STDMETHODCALLTYPE CPersistStorage::HandsOffStorage(void)
{
	ENTER_INTERFACE("IPersistStorage::HandsOffStorage");

	ReleaseStreamsAndStorage();

	return ResultFromScode(S_OK);
}

 
void CPersistStorage::CreateStreams(void)
{
	ENTER_INTERFACE("IPersistStorage::CreateStreams");

	if (m_pDocument->m_lpStream)
		m_pDocument->m_lpStream->Release();

	m_pDocument->m_lpStorage->CreateStream("SHAPES",
											STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
											0,
											0,
											&m_pDocument->m_lpStream);
}


void CPersistStorage::OpenStreams(void)
{
	ENTER_INTERFACE("IPersistStorage::OpenStreams");

	if (m_pDocument->m_lpStream)
		m_pDocument->m_lpStream->Release();

	m_pDocument->m_lpStorage->OpenStream("SHAPES",
									 	 0,
									 	 STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
									 	 0,
									 	 &m_pDocument->m_lpStream);
}


void CPersistStorage::ReleaseStreamsAndStorage(void)
{
	ENTER_INTERFACE("IPersistStorage::ReleaseStreamsAndStorage");

	if (m_pDocument->m_lpStream)
	{
		m_pDocument->m_lpStream->Release();
		m_pDocument->m_lpStream = NULL;
	}

	if (m_pDocument->m_lpStorage)
	{
		m_pDocument->m_lpStorage->Release();
		m_pDocument->m_lpStorage = NULL;
	}
}
