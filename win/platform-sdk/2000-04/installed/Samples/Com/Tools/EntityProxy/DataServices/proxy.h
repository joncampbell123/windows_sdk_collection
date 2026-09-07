// Proxy.h : Declaration of the CProxy

#ifndef __PROXY_H_
#define __PROXY_H_

#include "resource.h"       // main symbols
#include <mtx.h>

/////////////////////////////////////////////////////////////////////////////
// CProxy
class ATL_NO_VTABLE CProxy : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CProxy, &CLSID_Proxy>,
	public IObjectControl,
	public IObjectConstruct,
	public ISupportErrorInfo,
	public IDispatchImpl<IDataProxy, &IID_IDataProxy, &LIBID_DataServices>,
	public CODBCData
{

public:
	CProxy()
	{
		m_nCount = -1;
		vtNull.vt = VT_NULL;
		m_nRows = 0;
		m_MetaData = NULL;
	}

	~CProxy()
	{
		ClearArray();
	}

	void ClearArray()
	{
		if (m_MetaData != NULL)
		{
			delete [] m_MetaData;
			m_MetaData = NULL;
		}
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PROXY)

DECLARE_GET_CONTROLLING_UNKNOWN()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CProxy)
	COM_INTERFACE_ENTRY(IDataProxy)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectConstruct)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
END_COM_MAP()


	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IObjectControl
public:
    virtual HRESULT STDMETHODCALLTYPE Activate(void);
    virtual void STDMETHODCALLTYPE Deactivate(void);
    virtual BOOL STDMETHODCALLTYPE CanBePooled(void);

//IObjectConstruct
	STDMETHODIMP Construct(IDispatch * pUnk);

// IDataProxy
public:
	_ConnectionPtr m_spConnection;
	STDMETHOD(EntityInsert)(IEntityData *Entity, /*[out, retval]*/ long *Result);
	STDMETHOD(EntityDelete)(/*[in]*/ IEntityData *Entity, /*[out, retval]*/ long *Result);
	STDMETHOD(LookupFree)();
	STDMETHOD(NextEntity)(/*[out, retval]*/ IEntityData **ppEntity);
	STDMETHOD(EntityLookup)(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, IEntityData **ppEntity);
	STDMETHOD(EntityRead)(IEntityData *pEntity, long *Results);
	STDMETHOD(EntityUpdate)(IEntityData *pEntity, long *Result);
	STDMETHOD(ManualConstruct)( BSTR ConstructString);
	STDMETHOD(Execute)(BSTR SQLString, long *Result);
	STDMETHOD(EntityLoad)(BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, VARIANT KeyValue, long *Result, IEntityData **ppEntity);
	STDMETHOD(RSUpdate)(_Recordset *rs);
	STDMETHOD(RSQueryRW)(BSTR QueryText, VARIANT *Result, _Recordset **rs);
	STDMETHOD(RSQuery)(BSTR QueryText, VARIANT *Result, _Recordset ** Recordset);
	STDMETHOD(EntityQuery)(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, IResultset **ppResults);
	STDMETHOD(NextResultset)(long *Result, IResultset **Results);
	STDMETHOD(NextLookup)(long *Result, IEntityData **ppEntity);

	FieldMetaData *m_MetaData;
	short m_nCount;
	long m_nRows;

private:
	HRESULT InternalNextEntity(CStaticEntity *spEntity);
	HRESULT InternalLookup(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField);
	HRESULT InternalBindLookup(CStaticEntity *spEntity);
	HRESULT BindStaticEntity(FieldMetaData *MetaData, short nCount, CStaticEntity* spEntity);
	HRESULT PopulateStaticEntity(FieldMetaData *MetaData, IEntityData *pEntity);
	HRESULT CollectResultRows(IResultset **ppResults);

	CFastVariant vtNull;

	CString m_wszReadStoredProc;
	CString m_wszUpdateStoredProc;
	CString m_wszDeleteStoredProc;
	CString m_wszInsertStoredProc;
	CString m_wszKeyField;
};

#endif //__PROXY_H_
