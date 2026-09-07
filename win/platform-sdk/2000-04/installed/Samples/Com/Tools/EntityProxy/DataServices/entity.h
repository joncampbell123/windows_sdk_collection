	
// Static.h : Declaration of the CEntity

#ifndef __STATIC_H_
#define __STATIC_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEntity
class ATL_NO_VTABLE CEntity : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CEntity, &CLSID_Entity>,
	public ISupportErrorInfo,
	public IPersistStreamInit,
	public IMarshal,
	public IPersistStream,
	public IDispatchImpl<IEntityData, &IID_IEntityData, &LIBID_DataServices>,
	public IDispatchImpl<IEntity, &IID_IEntity, &LIBID_DataServices>
{
	DECLARE_FIXED_ALLOC(CEntity);

public:
	CEntity()
	{
		Clear();
	}

	~CEntity()
	{
		ClearArrays();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ENTITY)

BEGIN_COM_MAP(CEntity)
	COM_INTERFACE_ENTRY(IEntity)
	COM_INTERFACE_ENTRY(IEntityData)
	COM_INTERFACE_ENTRY2(IDispatch, IEntity)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IMarshal)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStream)
	COM_INTERFACE_ENTRY(IPersistStream)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IEntity
public:
	STDMETHOD(put_Recordset)(/*[in]*/ _Recordset* newVal);
	STDMETHOD(get_ReadCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_KeyField)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_KeyField)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_UpdateCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ReadStoredProcedure)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ReadStoredProcedure)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_UpdateStoredProcedure)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_UpdateStoredProcedure)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_InsertCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_InsertStoredProcedure)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_InsertStoredProcedure)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_DeleteCommand)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_DeleteStoredProcedure)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_DeleteStoredProcedure)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Field)(VARIANT *Index, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(put_Field)(VARIANT *Index, /*[in]*/ VARIANT *newVal);
	STDMETHOD(get_EditMode)(/*[out, retval]*/ ColEditModeEnum *pVal);
	STDMETHOD(put_EditMode)(/*[in]*/ ColEditModeEnum newVal);
	STDMETHOD(get_Key)(long Index, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(long Index, /*[in]*/ BSTR newVal);
	STDMETHOD(Insert)(long Index, BSTR Key, VARIANT *Value);
	STDMETHOD(Initialize)(long Size);
	STDMETHOD(_NewEnum)(/*[out, retval]*/ LPUNKNOWN *pVal);
	STDMETHOD(Item)(VARIANT *pIndex, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(Count)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Add)(VARIANT *pItem, VARIANT *pKey = &vtMissing, VARIANT *pBefore = &vtMissing, VARIANT *pAfter = &vtMissing);
	STDMETHOD(Remove)(VARIANT *pIndex);
	STDMETHOD(Append)(VARIANT *pItem, VARIANT *pKey);
// IMarshal
// IMarshal (Actually we delegate to IPersistStream's implementation here)
	STDMETHOD(GetUnmarshalClass) (REFIID riid, void * pv, DWORD dwDestContext, void * pvDestContext, DWORD mshlFlags, CLSID* pCid )
	{		
		*pCid = GetObjectCLSID();
		return S_OK;
	}
	
	unsigned long GetPersistSize(bool bWithMetaData = true)
	{
		_ULARGE_INTEGER ul;
		HRESULT hr;

		ul.LowPart = 0;
		ul.HighPart = 0;
		hr = GetPersistSizeMax(&ul, bWithMetaData);

		if (SUCCEEDED(hr))
			return ul.LowPart;
		else
			return 0;
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
		// Nothing to do
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
		HRESULT hr = S_FALSE;

		if (m_bRequiresSave)
		{
			hr = S_OK;
		}
		return hr;
	}

	HRESULT LoadFromStream(IStream * pStm, bool bWithMetaData = true)
	{
		long nSize = 0,  n = 0, nVer = 0;
		HRESULT hr = S_OK;

		*pStm >> (long)nVer; // version

		if (nVer < ARRAY_VERSION)
			return STG_E_OLDFORMAT;

		*pStm >> m_nMaxCount; // count
		
		Initialize(m_nMaxCount);

		*pStm >> m_bRequiresSave; // count
		*pStm >> m_EditMode; // count

		if (bWithMetaData)
		{
			*pStm >> m_wszUpdateStoredProc;
			*pStm >> m_wszReadStoredProc;
			*pStm >> m_wszDeleteStoredProc;
			*pStm >> m_wszInsertStoredProc;
			*pStm >> m_wszKeyField;
		}

		for (n = 0; (n < m_nMaxCount && SUCCEEDED(hr)); n++)
		{
			if (bWithMetaData)
				*pStm >> m_keyMap.m_aKey[n];
			*pStm >> (VARTYPE)m_Type[n];
			hr = m_pContents[n].ReadFromStream(pStm);
		}
		m_keyMap.m_nSize = m_nMaxCount;
		return hr;
	}

	STDMETHOD(Load)(IStream * pStm)
	{
		return LoadFromStream(pStm, true);;
	}

	STDMETHOD(Save)(IStream * pStm, int fClearDirty)
	{
		return SaveToStream(pStm, fClearDirty, true);
	}

	HRESULT SaveToStream(IStream * pStm, int fClearDirty, bool bWithMetaData = true)
	{
		long nSize = 0, n = 0;
		HRESULT hr = S_OK;

		*pStm << (long)ARRAY_VERSION; // version
		*pStm << m_nMaxCount; // count
		*pStm << m_bRequiresSave; // save
		*pStm << m_EditMode; // edit

		if (bWithMetaData)
		{
			*pStm << (LPCWSTR)m_wszUpdateStoredProc;
			*pStm << (LPCWSTR)m_wszReadStoredProc;
			*pStm << (LPCWSTR)m_wszDeleteStoredProc;
			*pStm << (LPCWSTR)m_wszInsertStoredProc;
			*pStm << (LPCWSTR)m_wszKeyField;
		}

		for (n = 0; (n < m_nMaxCount && SUCCEEDED(hr)); n++)
		{
			if (bWithMetaData)
				*pStm << (LPCWSTR)m_keyMap.m_aKey[n];
			*pStm << (VARTYPE)m_Type[n];
			m_pContents[n].WriteToStream(pStm);
		}

		if (fClearDirty)
			m_bRequiresSave = false;

		return hr;
	}

	STDMETHOD(GetSizeMax)(_ULARGE_INTEGER * pCbSize)
	{
		return GetPersistSizeMax(pCbSize, true);
	}

	HRESULT GetPersistSizeMax(_ULARGE_INTEGER * pCbSize, bool bWithMetaData = true)
	{
		if (pCbSize == NULL)
			return E_POINTER;
			
		long nSize = 0, n = 0;
		HRESULT hr = S_OK;

		CFastVariant update((LPCWSTR)m_wszUpdateStoredProc);
		CFastVariant read((LPCWSTR)m_wszReadStoredProc);
		CFastVariant insert((LPCWSTR)m_wszInsertStoredProc);
		CFastVariant deletesp((LPCWSTR)m_wszDeleteStoredProc);
		CFastVariant key((LPCWSTR)m_wszKeyField);

		nSize += sizeof((long)ARRAY_VERSION); // version
		nSize += sizeof(m_nMaxCount); // count
		nSize += sizeof(m_bRequiresSave); // save
		nSize += sizeof(m_EditMode); // edit

		nSize += GetVariantSize(update);
		nSize += GetVariantSize(insert);
		nSize += GetVariantSize(deletesp);
		nSize += GetVariantSize(read);
		nSize += GetVariantSize(key);

		for (n = 0; n < m_nMaxCount; n++)
		{
			if (bWithMetaData)
			{
				CFastVariant vKey((LPCWSTR)m_keyMap.m_aKey[n]);

				nSize += GetVariantSize(vKey);
			}
			nSize += sizeof(long); // edit
			nSize += GetVariantSize(m_pContents[n]);
		}
		pCbSize->LowPart = nSize;
		return hr;
	}

	STDMETHOD(InitNew)()
	{
		HRESULT hr = S_OK;
	
		Clear();

		return hr;
	}
// Data:
	
	CString m_wszUpdateStoredProc;
	CString m_wszReadStoredProc;
	CString m_wszDeleteStoredProc;
	CString m_wszInsertStoredProc;
	CString m_wszKeyField;

	CStaticMap m_keyMap;
	CFastVariant* m_pContents;
	VARTYPE* m_Type;

private:
	ColEditModeEnum m_EditMode;
	long m_nCount;
	long m_nMaxCount;
	bool m_bRequiresSave;
	void ClearArrays();

// Methods:
	WCHAR* ExtractParams(WCHAR *wszParams);

public:
	void Clear();
	HRESULT InternalInsert(long Index, LPCWSTR wszKey, VARIANT* pValue, VARTYPE vt);
	HRESULT InternalInsert(long Index, LPCWSTR wszKey, WCHAR* wszValue);
	HRESULT InternalInsert(long Index, LPCWSTR wszKey, long* pnValue);
};

class CStaticEntity : public CComObject<CEntity>
{
	DECLARE_FIXED_ALLOC(CStaticEntity);

public:

};

#endif //__STATIC_H_
