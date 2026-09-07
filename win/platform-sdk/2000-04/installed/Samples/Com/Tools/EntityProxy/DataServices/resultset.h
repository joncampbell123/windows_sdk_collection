// Resultset.h : Declaration of the CResultset

#ifndef __RESULTSET_H_
#define __RESULTSET_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CResultset
class ATL_NO_VTABLE CResultset : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CResultset, &CLSID_Resultset>,
	public IMarshal,
	public IPersistStream,
	public IPersistStreamInit,
	public ISupportErrorInfo,
	public IDispatchImpl<IResultset, &IID_IResultset, &LIBID_DataServices>
{

public:
	void Clear(bool bDelete)
	{

		if (bDelete && m_pVar != NULL)
		{
			delete [] m_pVar;
		}

		for (int n = 0; n < m_nCount; n++)
		{
			m_pEntity[n]->Release();
		}
		
		m_pVar = NULL;
		m_nCount = 0;
		m_nAlloc = 0;
	}

	CResultset()
	{
		m_pVar = NULL;
		m_nCount = 0;
		m_nAlloc = 0;
		Clear(false);
	}

	~CResultset()
	{	
		Clear(true);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RESULTSET)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CResultset)
	COM_INTERFACE_ENTRY(IResultset)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY2(IDispatch, IResultset)
	COM_INTERFACE_ENTRY(IMarshal)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStream)
	COM_INTERFACE_ENTRY(IPersistStream)
END_COM_MAP()

// IMarshal (Actually we delegate to IPersistStream's implementation here)
	STDMETHOD(GetUnmarshalClass) (REFIID riid, void * pv, DWORD dwDestContext, void * pvDestContext, DWORD mshlFlags, CLSID* pCid )
	{		
		*pCid = GetObjectCLSID();
		return S_OK;
	}
	
	STDMETHOD(GetMarshalSizeMax) (REFIID riid, void * pv, DWORD dwDestContext, void * pvDestContext, DWORD mshlFlags, ULONG* pSize )
	{
		_ULARGE_INTEGER ul;
		HRESULT hr;

		ul.LowPart = 0;
		ul.HighPart = 0;
		hr = GetSizeMax(&ul);

		if (SUCCEEDED(hr))
			*pSize = ul.LowPart;

		return hr;
	}

	STDMETHOD(MarshalInterface)  (IStream* pStm, REFIID riid, void *pv, DWORD dwDestContext, void * pvDestContext, DWORD mshlFlags)
	{
		return Save(pStm, FALSE);
	}

	STDMETHOD(UnmarshalInterface)(IStream* pStm, REFIID riid, void ** ppv )
	{
		HRESULT hr = Load(pStm);

		if (SUCCEEDED(hr))
		{
			hr = QueryInterface(riid, ppv);
		}
		return hr;
	}

	STDMETHOD (ReleaseMarshalData)(IStream *pStm)
	{
		return E_NOTIMPL;
	}
    
    STDMETHOD (DisconnectObject)( DWORD dwReserved)
	{
		// Proxy and Object are not connected -- nothing to do
		return E_NOTIMPL;
	}

// IPersistStreamInit
// IPersistStream
// IPersist
	STDMETHOD(GetClassID)(GUID * pClassID)
	{
		if (pClassID == NULL)
			return E_POINTER;

		*pClassID = GetObjectCLSID();

		return S_OK;
	}

	STDMETHOD(IsDirty)()
	{
		HRESULT hr = S_OK;

		return hr;
	}

	STDMETHOD(Load)(IStream * pStm)
	{
		long nSize = 0, nVer = 0, nCount;
		HRESULT hr = S_OK;
		CStaticEntity * spFirstEntity;

		*pStm >> (long)nVer; // version

		if (nVer < COLLECTION_VERSION)
			return STG_E_OLDFORMAT;

		*pStm >> nCount; // count
		*pStm >> m_nAlloc; // count

		*pStm >> m_wszUpdateStoredProc;
		*pStm >> m_wszReadStoredProc;
		*pStm >> m_wszDeleteStoredProc;
		*pStm >> m_wszInsertStoredProc;
		*pStm >> m_wszKeyField;

		if (nCount > 0)
			spFirstEntity = GetEntity(0);

		for (int n = 0; n < nCount && SUCCEEDED(hr); n++)
		{
			CStaticEntity * spEntity = GetEntity(n);
			hr = spEntity->LoadFromStream(pStm, (bool)(n==0));
			
			// Since we only copy one set of metadata for the whole resultset
			// copy that one set into each entity.
			if (n > 0)
			{
				long nFields, nField;

				hr = spFirstEntity->Count(&nFields);

				if (SUCCEEDED(hr))
				{
					for (nField = 0; nField < nFields; nField++)
					{
						spEntity->m_keyMap.m_aKey[nField] = spFirstEntity->m_keyMap.m_aKey[nField];
					}
					spEntity->m_wszUpdateStoredProc = m_wszUpdateStoredProc;
					spEntity->m_wszReadStoredProc = m_wszReadStoredProc;
					spEntity->m_wszDeleteStoredProc = m_wszDeleteStoredProc;
					spEntity->m_wszInsertStoredProc = m_wszInsertStoredProc;
					spEntity->m_wszKeyField = m_wszKeyField;
				}
			}
		}

		return hr;
	}

	STDMETHOD(Save)(IStream * pStm, int fClearDirty)
	{
		long nSize = 0;
		HRESULT hr = S_OK;

		*pStm << (long)COLLECTION_VERSION; // version
		*pStm << m_nCount; // count
		*pStm << m_nAlloc; // count

		*pStm << (LPCWSTR)m_wszUpdateStoredProc;
		*pStm << (LPCWSTR)m_wszReadStoredProc;
		*pStm << (LPCWSTR)m_wszDeleteStoredProc;
		*pStm << (LPCWSTR)m_wszInsertStoredProc;
		*pStm << (LPCWSTR)m_wszKeyField;

		for (int n = 0; (n < m_nCount && SUCCEEDED(hr)); n++)
		{
			CStaticEntity * spEntity = GetEntity(n);
			hr = spEntity->SaveToStream(pStm, FALSE, (bool)(n==0));
		}
		return hr;
	}

	STDMETHOD(GetSizeMax)(_ULARGE_INTEGER * pCbSize)
	{
		if (pCbSize == NULL)
			return E_POINTER;
			
		HRESULT hr = S_OK;
		unsigned long nSize = 0;
		unsigned long nMarshalSize = 0;

		nSize += sizeof((long)COLLECTION_VERSION); // version
		nSize += sizeof(m_nCount); // count
		nSize += sizeof(m_nAlloc); // count

		CFastVariant update((LPCWSTR)m_wszUpdateStoredProc);
		CFastVariant read((LPCWSTR)m_wszReadStoredProc);
		CFastVariant insert((LPCWSTR)m_wszInsertStoredProc);
		CFastVariant deletesp((LPCWSTR)m_wszDeleteStoredProc);
		CFastVariant key((LPCWSTR)m_wszKeyField);

		nSize += GetVariantSize(update);
		nSize += GetVariantSize(insert);
		nSize += GetVariantSize(deletesp);
		nSize += GetVariantSize(read);
		nSize += GetVariantSize(key);

		for (int n = 0; (n < m_nCount && SUCCEEDED(hr)); n++)
		{
			CStaticEntity * spEntity = GetEntity(n);
			nSize += spEntity->GetPersistSize((bool)(n==0));
		}
		pCbSize->LowPart = nSize;
		return hr;
	}

	STDMETHOD(InitNew)()
	{
		Clear(true);

		return S_OK;
	}

public:
// Data
	CStaticEntity *m_pEntity[ENTITY_BANKS * ENTITY_PER_BANK];
	CFastVariant *m_pVar;
	long m_nCount;
	long m_nAlloc;

	CString m_wszReadStoredProc;
	CString m_wszUpdateStoredProc;
	CString m_wszDeleteStoredProc;
	CString m_wszInsertStoredProc;
	CString m_wszKeyField;

// Implementation
	CStaticEntity *GetEntity(long nIndex);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IResultset
public:
	void SetCount(long nCount);
	void DeleteEntity(long nIndex);
	HRESULT Initialise(long nRows);
	STDMETHOD(get_Record)(long nIndex, /*[out, retval]*/ IEntityData* *pVal);
	STDMETHOD(Count)(long* pi4);
	STDMETHOD(_NewEnum)(IUnknown** ppunk);	
};

class CDynamicResultset : public CComObject<CResultset>
{
public:
	STDMETHOD_(ULONG, Release)()
	{
		ULONG l = InternalRelease();
		if (l == 0)
			delete this;
		return 1;
	}
};

#endif //__RESULTSET_H_
