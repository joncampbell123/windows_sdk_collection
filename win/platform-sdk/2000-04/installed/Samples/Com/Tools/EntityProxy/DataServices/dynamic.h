	
// Dynamic.h : Declaration of the CDynamic

#ifndef __DYNAMIC_H_
#define __DYNAMIC_H_

#include "resource.h"       // main symbols

bool operator<(const CFastVariant l, const CFastVariant r);
bool operator==(const CFastVariant l, const CFastVariant r);

//static CFastVariant vtMissing(VT_ERROR);
typedef std::vector<CFastVariant, std::allocator<CFastVariant> > varvect;

typedef std::map<_bstr_t, long> bstr2long;
typedef std::map<long, _bstr_t> long2bstr;
/////////////////////////////////////////////////////////////////////////////
// CDynamic
class ATL_NO_VTABLE CDynamic : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CDynamic, &CLSID_Dynamic>,
	public ISupportErrorInfo,
	public IMarshal,
	public IPersistStream,
	public IPersistStreamInit,
	public IDispatchImpl<IDynamic, &IID_IDynamic, &LIBID_DataServices>
{
public:
	CDynamic()
	{
		Clear();
	}

	~CDynamic()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DYNAMIC)
DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CDynamic)
	COM_INTERFACE_ENTRY(IDynamic)
	COM_INTERFACE_ENTRY2(IDispatch, IDynamic)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IMarshal)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStream)
	COM_INTERFACE_ENTRY(IPersistStream)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IDynamic
public:
	STDMETHOD(_NewEnum)(/*[out, retval]*/ IUnknown** ppunk);
	STDMETHOD(Item)(VARIANT *pIndex, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(Count)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Add)(VARIANT *pItem, VARIANT *pKey = &vtMissing, VARIANT *pBefore = &vtMissing, VARIANT *pAfter = &vtMissing);
	STDMETHOD(Remove)(VARIANT *pIndex);
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

	STDMETHOD(Load)(IStream * pStm)
	{
		long nSize = 0, colCount = 0, n = 0, nVer = 0;
		HRESULT hr = S_OK;

		hr = Count(&colCount);

		*pStm >> (long)nVer; // version

		if (nVer < COLLECTION_VERSION)
			return STG_E_OLDFORMAT;

		*pStm >> colCount; // count
		*pStm >> m_bRequiresSave; // count

		for (n = 1; (n <= colCount && SUCCEEDED(hr)); n++)
		{
			CFastVariant v;
			CFastVariant vKey;

			*pStm >> vKey;

			hr = v.ReadFromStream(pStm);
			if (SUCCEEDED(hr))
				hr = Add(&v, &vKey);
		}

		return hr;
	}

	STDMETHOD(Save)(IStream * pStm, int fClearDirty)
	{
		long nSize = 0, colCount = 0, n = 0;
		HRESULT hr = S_OK;

		hr = Count(&colCount);

		*pStm << (long)COLLECTION_VERSION; // version
		*pStm << colCount; // count
		*pStm << m_bRequiresSave; // save

		for (n = 1; (n <= colCount && SUCCEEDED(hr)); n++)
		{
			CFastVariant v;
			CFastVariant item(n);
			CFastVariant vKey;

			hr = Item(&item, &v);
			vKey = (LPCWSTR)m_MapIndex[n];
			*pStm << vKey;

			hr = v.WriteToStream(pStm);
		}

		if (fClearDirty)
			m_bRequiresSave = false;

		return hr;
	}

	STDMETHOD(GetSizeMax)(_ULARGE_INTEGER * pCbSize)
	{
		if (pCbSize == NULL)
			return E_POINTER;
			
		long nSize = 0, colCount = 0, n = 0;
		HRESULT hr = S_OK;
		VARIANT vItem;

		hr = Count(&colCount);

		nSize += sizeof((long)COLLECTION_VERSION); // version
		nSize += sizeof(colCount); // count
		nSize += sizeof(m_bRequiresSave); // save

		for (n = 1; n <= colCount; n++)
		{
			CFastVariant item(n);
			CFastVariant vKey((BSTR)m_MapIndex[n]);

			nSize += GetVariantSize(vKey);

			hr = Item(&item, &vItem); 
			nSize += GetVariantSize(vItem);
			VariantClear(&vItem);
		}
		pCbSize->LowPart = nSize;
		return hr;
	}

	STDMETHOD(InitNew)()
	{
		long n;
		long count;
		HRESULT hr = S_OK;

		hr = Count(&count);
		if (SUCCEEDED(hr))
		{
			for (n=0;(n < count && SUCCEEDED(hr)); n++)
			{
				_variant_t vIndex(n);
				hr = Remove(&vIndex);
			}
			m_bRequiresSave = false;
		}
		return hr;
	}

private:
// Data:
	varvect m_VarVect;
	bstr2long m_IndexMap;
	long2bstr m_MapIndex;
	bool m_bRequiresSave;
	int m_nGrow;
	int m_nReserve;

// Methods:
	void Clear();
};

#endif //__DYNAMIC_H_
