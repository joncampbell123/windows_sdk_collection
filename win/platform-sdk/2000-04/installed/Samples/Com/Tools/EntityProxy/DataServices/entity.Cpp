// Static.cpp : Implementation of CEntity
#include "stdafx.h"
#include "Data.h"
#include "Entity.h"

/////////////////////////////////////////////////////////////////////////////
// CEntity

IMPLEMENT_FIXED_ALLOC(CEntity, 100);
IMPLEMENT_FIXED_ALLOC(CStaticEntity, 100);

WCHAR* CEntity::ExtractParams(WCHAR *wszParams)
{
	register long nCol;
	WCHAR *wszParam = wszParams;
	register VARIANT* pVal;
	register LPCWSTR wszName;
	int nChars = 0;

	if (m_nMaxCount <= 0) return wszParams;

	for (nCol = 0; nCol < m_nMaxCount; nCol++)
	{
		pVal = &m_pContents[nCol];
		wszName = (LPCWSTR)m_keyMap.m_aKey[nCol];
		wszParam[0] = L'@';
		wszParam++;

		wszParam += WriteNamedParam(wszParam, wszName, pVal);
	}
	wszParam[-2] = L'\0';
	return wszParams;
}


STDMETHODIMP CEntity::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IEntity
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CEntity::Clear()
{
	m_bRequiresSave = false;
	m_EditMode = colEditNone;
	m_nMaxCount = 0;
	m_nCount = 0;
	m_wszUpdateStoredProc.Empty();
	m_wszReadStoredProc.Empty();
	m_wszKeyField.Empty();
	m_wszDeleteStoredProc.Empty();
	m_wszInsertStoredProc.Empty();
	m_keyMap.RemoveAll();
	m_pContents = NULL;
	m_Type = NULL;

	ClearArrays();
}

void CEntity::ClearArrays()
{
	m_keyMap.ClearArray();

	if (m_pContents != NULL)
	{
		delete [] m_pContents;
		m_pContents = NULL;
	}
	if (m_Type != NULL)
	{
		delete [] m_Type;
		m_Type = NULL;
	}
}

STDMETHODIMP CEntity::Initialize(long Size)
{
	HRESULT hr = S_OK;
	Clear();

	m_nMaxCount = Size;
	ClearArrays();

	m_keyMap.Initialize(Size);
	ATLASSERT(m_pContents == NULL);
	ATLASSERT(m_Type == NULL);
	m_pContents = new CFastVariant[Size + 1];
	m_Type = new VARTYPE[Size + 1];

	return hr;
}

STDMETHODIMP CEntity::_NewEnum(LPUNKNOWN * pVal)
{
	if (pVal == NULL)
		return E_POINTER;
	*pVal = NULL;
	typedef CComObject<CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > > enumvar;
	enumvar* pEnumVar = new enumvar;
	_ASSERTE(pEnumVar);
	HRESULT hRes = pEnumVar->Init(&m_pContents[0], &m_pContents[m_nMaxCount], NULL, AtlFlagCopy);
	if (SUCCEEDED(hRes))
		hRes = pEnumVar->QueryInterface(IID_IEnumVARIANT, (void**)pVal);
	if (FAILED(hRes))
		delete pEnumVar;
	return hRes;
}

STDMETHODIMP CEntity::Count(long *Count)
{
	*Count = m_nMaxCount;

	return S_OK;
}

STDMETHODIMP CEntity::Item(VARIANT *Index, VARIANT *Value)
{
	HRESULT hr = S_OK;
	long nIndex = 0;
	if (Index->vt == VT_BSTR)
	{
		nIndex = m_keyMap.Lookup(Index->bstrVal);
	}
	else
	{
		_variant_t vIndex(*Index);
		nIndex = (long)vIndex - 1;
	}

	if (nIndex >= 0 && nIndex <= m_nMaxCount)
	{
		if (SUCCEEDED(hr))
			hr = VariantCopy(Value, &m_pContents[nIndex]);
	}
	else
		return AtlReportError(GetObjectCLSID(), "Cannot locate item", IID_IEntity, E_FAIL);
	return hr;
}

HRESULT CEntity::InternalInsert(long Index, LPCWSTR wszKey, VARIANT* pValue, VARTYPE vt)
{
	HRESULT hr = S_OK;
	if (Index <= m_nMaxCount && Index > 0)
	{
		m_keyMap.AddNoLen(wszKey, Index - 1);

		hr = VariantCopy(&m_pContents[Index - 1], pValue);
		m_Type[Index - 1] = vt;

		m_bRequiresSave = true;
		if (Index > m_nCount)
		{
			m_nCount = Index;
		}
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized to size required", IID_IEntity, E_FAIL);
	return hr;
}

HRESULT CEntity::InternalInsert(long Index, LPCWSTR wszKey, WCHAR* wszValue)
{
	HRESULT hr = S_OK;
	if (Index <= m_nMaxCount && Index > 0)
	{
		m_keyMap.AddNoLen(wszKey, Index - 1);

		ATLASSERT(m_pContents[Index - 1].vt == VT_EMPTY);

		m_pContents[Index - 1].bstrVal = SysAllocString(wszValue);
		m_pContents[Index - 1].vt = VT_BSTR;
		m_Type[Index - 1] = VT_BSTR;

		m_bRequiresSave = true;
		if (Index > m_nCount)
		{
			m_nCount = Index;
		}
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized to size required", IID_IEntity, E_FAIL);
	return hr;
}

HRESULT CEntity::InternalInsert(long Index, LPCWSTR wszKey, long* pnValue)
{
	HRESULT hr = S_OK;
	if (Index <= m_nMaxCount && Index > 0)
	{
		m_keyMap.AddNoLen(wszKey, Index - 1);

		ATLASSERT(m_pContents[Index - 1].vt == VT_EMPTY);

		m_pContents[Index - 1].lVal = *pnValue;
		m_pContents[Index - 1].vt = VT_I4;
		m_Type[Index - 1] = VT_I4;

		m_bRequiresSave = true;
		if (Index > m_nCount)
		{
			m_nCount = Index;
		}
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized to size required", IID_IEntity, E_FAIL);
	return hr;
}

STDMETHODIMP CEntity::Insert(long Index, BSTR Key, VARIANT* pValue)
{
	HRESULT hr = S_OK;
	if (Index <= m_nMaxCount && Index > 0)
	{
		m_keyMap.Add(Key, SysStringLen(Key), Index - 1);

		hr = VariantCopy(&m_pContents[Index - 1], pValue);
		m_Type[Index - 1] = pValue->vt;

		m_bRequiresSave = true;
		if (Index > m_nCount)
			m_nCount = Index;
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized to size required", IID_IEntity, E_FAIL);
	return hr;
}

STDMETHODIMP CEntity::Add(VARIANT *pItem, VARIANT *pKey, VARIANT *pBefore, VARIANT *pAfter)
{
	HRESULT hr = S_OK;

	if (pKey->vt != VT_BSTR)
	{
		hr = DISP_E_TYPEMISMATCH;
	}
	else if (pBefore->vt != VT_ERROR || pAfter->vt != VT_ERROR)
	{
		hr = DISP_E_BADPARAMCOUNT;
	}
	else
	{
		hr = Insert(m_nCount + 1, pKey->bstrVal, pItem);
	}
	return hr;
}

STDMETHODIMP CEntity::Remove(VARIANT *pItem)
{
	return E_NOTIMPL;
}

STDMETHODIMP CEntity::get_EditMode(ColEditModeEnum *pVal)
{
	if (m_bRequiresSave && m_EditMode == colEditNone)
	{
		m_EditMode = colEditInProgress;
	}
	*pVal = m_EditMode;

	return S_OK;
}

STDMETHODIMP CEntity::put_EditMode(ColEditModeEnum newVal)
{
	m_EditMode = newVal;
	if (m_EditMode == colEditNone)
		m_bRequiresSave = false;
	return S_OK;
}

STDMETHODIMP CEntity::get_Key(long Index, BSTR *pVal)
{
	if (Index > 0 && Index <= m_nMaxCount)
	{
		*pVal = _bstr_t(m_keyMap.m_aKey[Index - 1]).copy();
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized", IID_IEntity, E_FAIL);
	return S_OK;
}

STDMETHODIMP CEntity::put_Key(long Index, BSTR newVal)
{
	if (Index > 0 && Index <= m_nMaxCount)
	{
		int nLen = SysStringLen(newVal);	
		m_keyMap.m_aKey[Index -1].AssignCopy(nLen, newVal);
		m_bRequiresSave = true;
		if (Index > m_nCount)
			m_nCount = Index;
	}
	else
		return AtlReportError(GetObjectCLSID(), "Array has not been initialized", IID_IEntity, E_FAIL);

	return S_OK;
}

STDMETHODIMP CEntity::get_Field(VARIANT *Index, VARIANT *pVal)
{
	return Item(Index, pVal);
}

STDMETHODIMP CEntity::put_Field(VARIANT *Index, VARIANT* newVal)
{
	HRESULT hr = S_OK;
	long nIndex = 0;
	if (Index->vt == VT_BSTR)
	{
		nIndex = m_keyMap.Lookup(Index->bstrVal);
	}
	else
	{
		_variant_t vIndex(*Index);
		nIndex = (long)vIndex - 1;
	}

	if (nIndex >= 0 && nIndex <= m_nMaxCount)
	{
		if ((nIndex+1) > m_nCount)
			m_nCount = (nIndex+1);
		
		if (newVal->vt == m_Type[nIndex] || newVal->vt == VT_NULL)
		{
			hr = VariantCopy(&m_pContents[nIndex], newVal);
		}
		else
		{
			hr = VariantChangeType(&m_pContents[nIndex], newVal, 0, m_Type[nIndex]);
		}

// Currently there is a clear separation between NULL and empty strings.
// If you require empty strings to be treated as NULLs then uncomment the following
// code. 
/*
		if (m_pContents[nIndex].vt == VT_BSTR)
		{
			if (SysStringLen(m_pContents[nIndex].bstrVal) <= 0)
			{
				VariantClear(&m_pContents[nIndex]);
				m_pContents[nIndex].vt = VT_NULL;
			}
		}
*/
		m_bRequiresSave = true;
	}
	else
		return AtlReportError(GetObjectCLSID(), "Cannot locate item", IID_IEntity, E_FAIL);
	return hr;
}

STDMETHODIMP CEntity::get_UpdateStoredProcedure(BSTR *pVal)
{
	*pVal = _bstr_t(m_wszUpdateStoredProc).copy();

	return S_OK;
}

STDMETHODIMP CEntity::put_UpdateStoredProcedure(BSTR newVal)
{
	int nLen = SysStringLen(newVal);
	m_wszUpdateStoredProc.AssignCopy(nLen, newVal);
	m_bRequiresSave = true;

	return S_OK;
}

STDMETHODIMP CEntity::get_ReadStoredProcedure(BSTR *pVal)
{
	*pVal = _bstr_t(m_wszReadStoredProc).copy();

	return S_OK;
}

STDMETHODIMP CEntity::put_ReadStoredProcedure(BSTR newVal)
{
	int nLen = SysStringLen(newVal);
	m_wszReadStoredProc.AssignCopy(nLen, newVal);
	m_bRequiresSave = true;

	return S_OK;
}

STDMETHODIMP CEntity::get_UpdateCommand(BSTR *pVal)
{
	WCHAR wszParams[4096];
	WCHAR wszSQL[4196];
	
	wszParams[0] = L'\0';
	wszSQL[0] = L'\0';

	if (m_wszUpdateStoredProc[0] != L'\0')
	{
//		SWPRINTF(wszSQL, L"EXEC %s %s", (LPCWSTR)m_wszUpdateStoredProc, (LPCWSTR)ExtractParams(wszParams));
		WriteChars(wszSQL, L"EXEC", (LPCWSTR)m_wszUpdateStoredProc, (LPCWSTR)ExtractParams(wszParams));
		*pVal = SysAllocString(wszSQL);
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}

STDMETHODIMP CEntity::get_KeyField(BSTR *pVal)
{
	*pVal = _bstr_t(m_wszKeyField).copy();

	return S_OK;
}

STDMETHODIMP CEntity::put_KeyField(BSTR newVal)
{
	int nLen = SysStringLen(newVal);
	m_wszKeyField.AssignCopy(nLen, newVal);
	m_bRequiresSave = true;

	return S_OK;
}

STDMETHODIMP CEntity::get_ReadCommand(BSTR *pVal)
{
	WCHAR wszSQL[4196];
	_variant_t val;
	long nIndex;

	wszSQL[0] = L'\0';

	nIndex = m_keyMap.Lookup(m_wszKeyField);

	if (nIndex >= 0 && m_wszReadStoredProc[0] != L'\0')
	{
		val = m_pContents[nIndex];
		BuildExecCommand(wszSQL, m_wszReadStoredProc, m_wszKeyField, &val);		
		*pVal = SysAllocString(wszSQL);
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}

STDMETHODIMP CEntity::Append(VARIANT *pItem, VARIANT *pKey)
{
	HRESULT hr = S_OK;

	if (pKey->vt != VT_BSTR)
	{
		hr = DISP_E_TYPEMISMATCH;
	}
	else
	{
		hr = Insert(m_nCount + 1, pKey->bstrVal, pItem);
	}
	return hr;
}

STDMETHODIMP CEntity::get_DeleteStoredProcedure(BSTR *pVal)
{
	*pVal = _bstr_t(m_wszDeleteStoredProc).copy();

	return S_OK;
}

STDMETHODIMP CEntity::put_DeleteStoredProcedure(BSTR newVal)
{
	long nLen = SysStringLen(newVal);
	m_wszDeleteStoredProc.AssignCopy(nLen, newVal);
	m_bRequiresSave = true;
	return S_OK;
}

STDMETHODIMP CEntity::get_DeleteCommand(BSTR *pVal)
{
	WCHAR wszSQL[4196];
	_variant_t val;
	long nIndex;

	wszSQL[0] = L'\0';

	nIndex = m_keyMap.Lookup(m_wszKeyField);

	if (nIndex >= 0 && m_wszDeleteStoredProc[0] != L'\0')
	{
		val = m_pContents[nIndex];
		BuildExecCommand(wszSQL, m_wszDeleteStoredProc, m_wszKeyField, &val);		
		*pVal = SysAllocString(wszSQL);
	}
	else
	{
		*pVal = NULL;
	}
	return S_OK;
}

STDMETHODIMP CEntity::get_InsertStoredProcedure(BSTR *pVal)
{
	*pVal = _bstr_t(m_wszInsertStoredProc).copy();

	return S_OK;
}

STDMETHODIMP CEntity::put_InsertStoredProcedure(BSTR newVal)
{
	long nLen = SysStringLen(newVal);
	m_wszInsertStoredProc.AssignCopy(nLen, newVal);
	m_bRequiresSave = true;
	return S_OK;
}

STDMETHODIMP CEntity::get_InsertCommand(BSTR *pVal)
{
	HRESULT hr = S_OK;
	WCHAR wszParams[4096];
	WCHAR wszSQL[4196];
	
	wszParams[0] = L'\0';
	wszSQL[0] = L'\0';

	if (m_wszInsertStoredProc[0] != L'\0')
	{
		WriteChars(wszSQL, L"EXEC", (LPCWSTR)m_wszInsertStoredProc, (LPCWSTR)ExtractParams(wszParams));
//		SWPRINTF(wszSQL, L"EXEC %s %s", (LPCWSTR)m_wszInsertStoredProc, (LPCWSTR)ExtractParams(wszParams));
		*pVal = SysAllocString(wszSQL);
	}
	else
	{
		*pVal = NULL;
	}
	return hr;
}
