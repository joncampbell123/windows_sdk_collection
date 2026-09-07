#ifndef __POOLEDDATA_H
#define __POOLEDDATA_H

typedef struct tagFieldMetaData
{
	DECLARE_FIXED_ALLOC(tagFieldMetaData);
public:
	CString wszName;
	VARTYPE vt;
	bool bNull;
	short sqltype;
	short sqldatatype;
	WCHAR pData[MAX_FIELD_DATA_SIZE];
	unsigned long nDataLen;
	long nDataBindLen;
	short nScale;
	short nNullable;
} FieldMetaData;

class CPooledData
{

protected:
	CComPtr<IObjectContext> m_spObjectContext;

	GUID m_txuow;
	BOOL m_bObjectPooled;
	BOOL m_bConstructed;
	BOOL m_bShouldEnlist;

	CPooledData()
	{
		m_bConstructed = FALSE;
		m_bShouldEnlist = FALSE;
		m_txuow = GUID_NULL;
		m_bObjectPooled = FALSE;
	}

	~CPooledData()
	{
	}

	STDMETHOD(ActivateData)(IObjectContext * pObjectContext)
	{
		//	should only do this when we are being activated in different 
		//	tx units of work
		m_bShouldEnlist = FALSE;

		if (!m_bObjectPooled)
			return S_OK;

		HRESULT hr = S_OK;
		GUID txUOW = GUID_NULL;
		IObjectContextInfo	* pObjTx = NULL;
		hr = pObjectContext->QueryInterface(IID_IObjectContextInfo, (void **)&pObjTx);
		if (pObjTx)
		{		
			pObjTx -> GetTransactionId (&txUOW);
			pObjTx -> Release();
		}

		if (txUOW != m_txuow)
		{
			m_bShouldEnlist = TRUE;	
			m_txuow = txUOW;
		}
		return hr;
	} 

	STDMETHODIMP ConstructData(IDispatch * pUnk)
	{
		if (!pUnk)
			return E_UNEXPECTED;
		HRESULT hr = E_FAIL;
		IObjectConstructString * pString = NULL;
		hr = pUnk -> QueryInterface(IID_IObjectConstructString, (void **)&pString);
		if (pString)
		{
			CComBSTR s;
			pString -> get_ConstructString(&s);
			m_bObjectPooled = TRUE;
			hr = ManualConstruct(s);
			pString -> Release();
		}
		return hr;
	}

    virtual BOOL STDMETHODCALLTYPE CanConnectionBePooled(void)
	{
		return m_bObjectPooled;
	}

	virtual HRESULT QueryEnlist(const CLSID &clsid, const IID &iid)
	{
		HRESULT hr = S_OK;

		if (m_bShouldEnlist)
		{
			hr = Enlist(m_spObjectContext, clsid, iid);
			if (FAILED(hr))
			{
				SETABORT;
				hr = AtlReportError(clsid, (LPCSTR)"Failed to enlist in transaction", iid, hr);
			}
		}
		return hr;
	}

	// The pure virtual elements that the derived class must provide
	STDMETHOD(ManualConstruct)(BSTR ConstructString) = 0;
    virtual void STDMETHODCALLTYPE DeactivateData(void) = 0;
	virtual HRESULT Enlist(IObjectContext * pObjectContext, const CLSID &clsid, const IID &iid) = 0;
};

#endif