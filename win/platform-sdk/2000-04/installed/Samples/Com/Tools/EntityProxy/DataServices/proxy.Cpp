// Proxy.cpp : Implementation of CProxy
#include "stdafx.h"
#include "Data.h"
#include "Entity.h"
#include "Dynamic.h"
#include "Resultset.h"
#include "odbcdata.h"
#include "Proxy.h"
#include <wchar.h>

/////////////////////////////////////////////////////////////////////////////

// CProxy

STDMETHODIMP CProxy::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IDataProxy
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT CProxy::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	ActivateData(m_spObjectContext.p);
	return hr;
} 

BOOL CProxy::CanBePooled()
{
	return CanConnectionBePooled();
} 

void CProxy::Deactivate()
{
	DeactivateData();
} 

STDMETHODIMP CProxy::Construct(IDispatch * pUnk)
{
	return ConstructData(pUnk);
}

STDMETHODIMP CProxy::Execute(BSTR SQLString, long *pResult)
{
	HRESULT hr = S_OK;
	WCHAR wszSQL[MAX_FIELD_DATA_SIZE];

	CloseCurrentStatement();
	hr = QueryEnlist(CLSID_Proxy, IID_IDataProxy);

	if (SUCCEEDED(hr))
	{
		WCSNCPY(wszSQL, SQLString, SysStringLen(SQLString));
		hr = ExecuteSQL(wszSQL, pResult);
	}
	return hr;
}

STDMETHODIMP CProxy::ManualConstruct(BSTR ConstructString)
{
	HRESULT hr = E_FAIL;

	hr = ManualConstructData(ConstructString);

	return hr;
}

STDMETHODIMP CProxy::EntityDelete(IEntityData *pEntity, long *Result)
{
	HRESULT hr = S_OK;
	long nResult;
	CComBSTR bstrCommand;

	hr = pEntity->get_DeleteCommand(&bstrCommand);

	if (SUCCEEDED(hr))
		hr = Execute(bstrCommand, &nResult);

	if (SUCCEEDED(hr))
	{
		hr = pEntity->put_EditMode(colEditDeleted);
		*Result = nResult;
	}
	return hr;
}

STDMETHODIMP CProxy::EntityInsert(IEntityData *pEntity, long *Result)
{
	HRESULT hr = S_OK;
	long nResult;
	CComBSTR bstrCommand;

	hr = pEntity->get_InsertCommand(&bstrCommand);

	if (SUCCEEDED(hr))
		hr = Execute(bstrCommand, &nResult);

	if (SUCCEEDED(hr))
	{
		hr = pEntity->put_EditMode(colEditNone);
		*Result = nResult;
	}
	return hr;
}

STDMETHODIMP CProxy::EntityUpdate(IEntityData *pEntity, long *Result)
{
	HRESULT hr = S_OK;
	long nResult;
	CComBSTR bstrCommand;
	ColEditModeEnum mode = colEditNone;

	hr = pEntity->get_EditMode(&mode);
	
	if (SUCCEEDED(hr))
	{
		if (mode == colEditDelete)
		{
			hr = pEntity->get_DeleteCommand(&bstrCommand);
			if (SUCCEEDED(hr))
				hr = pEntity->put_EditMode(colEditDeleted);
		}
		else if (mode == colEditAdd)
		{
			hr = pEntity->get_InsertCommand(&bstrCommand);
			if (SUCCEEDED(hr) && bstrCommand.m_str != NULL)
			{
				hr = pEntity->put_EditMode(colEditNone);
			}
			else
			{
				hr = pEntity->get_UpdateCommand(&bstrCommand);
				if (SUCCEEDED(hr))
					hr = pEntity->put_EditMode(colEditNone);
			}
		}
		else
		{
			hr = pEntity->get_UpdateCommand(&bstrCommand);
			if (SUCCEEDED(hr))
				hr = pEntity->put_EditMode(colEditNone);
		}

		hr = Execute(bstrCommand, &nResult);
		if (SUCCEEDED(hr))
			*Result = nResult;
	}

	return hr;
}

STDMETHODIMP CProxy::EntityRead(IEntityData *pEntity, long *Results)
{
	HRESULT hr = S_OK;
	long nRowsAffected = 0;
	short nsCount = 0;

	if (pEntity == NULL)
	{
		return E_INVALIDARG;
	}

	if (SUCCEEDED(hr))
	{
		HRESULT hr = E_FAIL;
		CComPtr<IEntityData> spEntity(pEntity);
		CComBSTR bstr;
		FieldMetaData *pMetaData = NULL;

		CloseCurrentStatement();
		hr = QueryEnlist(CLSID_Proxy, IID_IDataProxy);
		if (SUCCEEDED(hr))
		{
			spEntity->get_ReadCommand(&bstr);

			hr = ExecuteSQL(bstr);
			if (SUCCEEDED(hr))
			{
				hr = BindResultMetaData(&pMetaData, &nsCount);
				if (SUCCEEDED(hr))
				{
					hr = PopulateStaticEntity(pMetaData, spEntity);
					if (SUCCEEDED(hr))
						hr = spEntity->put_EditMode(colEditNone);
				}
				delete [] pMetaData;
			}
		}
	}
	return hr;
}

STDMETHODIMP CProxy::EntityQuery(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, IResultset **ppResults)
{
	HRESULT hr = S_OK;

	CloseCurrentStatement();

	hr = InternalLookup(SQLString, Result, ReadStoredProc, UpdateStoredProc, DeleteStoredProc, InsertStoredProc, KeyField);

	if (SUCCEEDED(hr))
	{
		hr = CollectResultRows(ppResults);
	}

	return hr;
}

HRESULT CProxy::CollectResultRows(IResultset **ppResults)
{
	HRESULT hr = S_OK;
	CDynamicResultset* spResults = NULL;
	CStaticEntity* spEntity = NULL;
	long nEntity = 0;

	spResults = new CDynamicResultset;
	spResults->Initialise(m_nRows);

	spEntity = spResults->GetEntity(nEntity);

	hr = InternalBindLookup(spEntity);

	if (SUCCEEDED(hr))
	{
		spResults->m_wszReadStoredProc = m_wszReadStoredProc;
		spResults->m_wszUpdateStoredProc = m_wszUpdateStoredProc;
		spResults->m_wszDeleteStoredProc = m_wszDeleteStoredProc;
		spResults->m_wszInsertStoredProc = m_wszInsertStoredProc;
		spResults->m_wszKeyField = m_wszKeyField;

		if (SUCCEEDED(hr) && spEntity != NULL)
		{
			do
			{
				spEntity->put_EditMode(colEditNone);
				nEntity += 1;
				spEntity = spResults->GetEntity(nEntity);
			} while ((nEntity < (ENTITY_BANKS * ENTITY_PER_BANK) - 1) && SUCCEEDED(InternalNextEntity(spEntity)));
			spResults->DeleteEntity(nEntity);
			spResults->SetCount(nEntity);
		}
		else if (FAILED(hr))
		{
			spResults->DeleteEntity(0);
			spResults->SetCount(0);
		}
	}
	else
	{
		spResults->DeleteEntity(0);
		spResults->SetCount(0);
	}
	hr = spResults->QueryInterface(IID_IResultset, (void**)ppResults);
	if (FAILED(hr))
	{
		delete spResults;
		*ppResults = NULL;
	}
	ClearArray();

	return hr;
}

HRESULT CProxy::BindStaticEntity(FieldMetaData *pMetaData, short nCount, CStaticEntity* spEntity)
{
	HRESULT hr = S_OK;
	RETCODE rc = 0;
	int nCol;

	hr = FetchData();

	if (SUCCEEDED(hr))
	{
		spEntity->Initialize(nCount);

		for (nCol = 1; nCol <= nCount; nCol++)
		{
			long nIndex = nCol-1;
			FieldMetaData *pMeta = &pMetaData[nIndex];

			if (pMeta->nDataBindLen <= 0)
			{
				*(long*)pMeta->pData = 0;
				if (pMeta->nNullable > 0)
				{
					pMeta->bNull = true;
				}
			}
			else
			{
				pMeta->bNull = false;
			}

			if (pMeta->nDataLen > 0)
			{
				if (pMeta->bNull == true)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, &vtNull, pMeta->vt);
				}
				else if (pMeta->vt == VT_BSTR)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, (WCHAR*)pMeta->pData);
				}
				else if (pMeta->vt == VT_I2)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, &_variant_t(*(short*)pMeta->pData), pMeta->vt);
				}
				else if (pMeta->vt == VT_I4)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, (long*)pMeta->pData);
				}
				else if (pMeta->vt == VT_UI1)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, &_variant_t(*(BYTE*)pMeta->pData), pMeta->vt);
				}
				else if (pMeta->vt == VT_DATE)
				{
					WCHAR* pStr = wcsstr(pMeta->pData, L".");
					pStr[0] = L'\0';
					CFastVariant varval(pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_DATE)))
					{
						hr = spEntity->InternalInsert(nCol, pMeta->wszName, &varval, pMeta->vt);
					}
				}
				else if (pMeta->vt == VT_DECIMAL)
				{
					CFastVariant varval((WCHAR*)pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_DECIMAL)))
					{
						hr = spEntity->InternalInsert(nCol, pMeta->wszName, &varval, pMeta->vt);
					}
				}
				else if (pMeta->vt == VT_CY)
				{
					CFastVariant varval((WCHAR*)pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_CY)))
					{
						hr = spEntity->InternalInsert(nCol, pMeta->wszName, &varval, pMeta->vt);
					}
				}
				else if (pMeta->vt == VT_BOOL)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, &_variant_t(*(bool*)pMeta->pData), pMeta->vt);
				}
				else if (pMeta->vt == VT_NULL)
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, &vtNull, pMeta->vt);
				}
				else
				{
					hr = spEntity->InternalInsert(nCol, pMeta->wszName, (WCHAR*)pMeta->pData);
				}
			}
			if (FAILED(hr))
				return AtlReportError(CLSID_Proxy, (LPCSTR)"Failed to bind entity data", IID_IDataProxy, hr);
		}
	}
	return hr;
}

HRESULT CProxy::PopulateStaticEntity(FieldMetaData *pMetaData, IEntityData *pEntity)
{
	HRESULT hr = S_OK;
	RETCODE rc = 0;
	int nCol;
	long nCount = 0;
	
	hr = FetchData();

	if (SUCCEEDED(hr))
	{
		pEntity->Count(&nCount);

		for (nCol = 1; nCol <= nCount; nCol++)
		{
			long nIndex = nCol-1;
			FieldMetaData *pMeta = &pMetaData[nIndex];

			if (pMeta->nDataBindLen <= 0)
			{
				*(long*)pMeta->pData = 0;
				if (pMeta->nNullable > 0)
					pMeta->vt = VT_NULL;
			}

			if (pMeta->nDataLen > 0)
			{
				if (pMeta->vt == VT_BSTR)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t((WCHAR*)pMeta->pData));
				}
				else if (pMeta->vt == VT_I2)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t(*(short*)pMeta->pData));
				}
				else if (pMeta->vt == VT_I4)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t(*(long*)pMeta->pData));
				}
				else if (pMeta->vt == VT_UI1)// || vt == VT_I4 || vt == VT_UI1)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t(*(BYTE*)pMeta->pData));
				}
				else if (pMeta->vt == VT_DATE)
				{
					WCHAR* pStr = wcsstr(pMeta->pData, L".");
					pStr[0] = L'\0';
					CFastVariant varval(pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_DATE)))
					{
						hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &varval);
					}
				}
				else if (pMeta->vt == VT_DECIMAL)
				{
					CFastVariant varval((WCHAR*)pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_DECIMAL)))
					{
						hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &varval);
					}
				}
				else if (pMeta->vt == VT_CY)
				{
					CFastVariant varval((WCHAR*)pMeta->pData);
					if (SUCCEEDED(varval.ChangeType(VT_CY)))
					{
						hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &varval);
					}
				}
				else if (pMeta->vt == VT_BOOL)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t(*(bool*)pMeta->pData));
				}
				else if (pMeta->vt == VT_NULL)
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &vtNull);
				}
				else
				{
					hr = pEntity->put_Field(&_variant_t(pMeta->wszName), &_variant_t((WCHAR*)pMeta->pData));
				}
			}
		}
		if (FAILED(hr))
			hr = AtlReportError(CLSID_Proxy, (LPCSTR)"Failed to populate entity", IID_IDataProxy, hr);
	}
	return hr;
}

STDMETHODIMP CProxy::EntityLoad(BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, VARIANT KeyValue, long *Result, IEntityData **ppEntity)
{
	HRESULT hr = S_OK;
	long nRowsAffected;
	short nsCount = 0;
	WCHAR wszSQL[MAX_FIELD_DATA_SIZE];
	CStaticEntity *spEntity = NULL;
	FieldMetaData* pMetaData = NULL;

	CloseCurrentStatement();
	hr = QueryEnlist(CLSID_Proxy, IID_IDataProxy);
	if (SUCCEEDED(hr))
	{
		SETCOMPLETE;

		BuildExecCommand(wszSQL, ReadStoredProc, KeyField, &KeyValue);

		hr = ExecuteSQL(wszSQL);

		if (SUCCEEDED(hr))
		{
			spEntity = new CStaticEntity;

			hr = BindResultMetaData(&pMetaData, &nsCount);
			if (SUCCEEDED(hr))
			{
				hr = BindStaticEntity(pMetaData, nsCount, spEntity);

				if (SUCCEEDED(hr))
				{
					spEntity->put_ReadStoredProcedure(ReadStoredProc);
					spEntity->put_UpdateStoredProcedure(UpdateStoredProc);
					spEntity->put_DeleteStoredProcedure(DeleteStoredProc);
					spEntity->put_InsertStoredProcedure(InsertStoredProc);
					spEntity->put_KeyField(KeyField);

					nRowsAffected = 1; 
					hr = spEntity->put_EditMode(colEditNone);
					if (SUCCEEDED(hr))
						hr = spEntity->QueryInterface(IID_IEntityData, (void**)ppEntity);
					*Result = nRowsAffected;
				}
			}
			if (FAILED(hr))
			{
				delete spEntity;
				*ppEntity = NULL;
			}
		}
		delete [] pMetaData;
	}
	return hr;
}

STDMETHODIMP CProxy::EntityLookup(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField, IEntityData **ppEntity)
{
	HRESULT hr = S_OK;
	CStaticEntity *spEntity = new CStaticEntity;

	CloseCurrentStatement();

	hr = InternalLookup(SQLString, Result, ReadStoredProc, UpdateStoredProc, DeleteStoredProc, InsertStoredProc, KeyField);
	if (SUCCEEDED(hr))
	{
		hr = InternalBindLookup(spEntity);

		if (SUCCEEDED(hr) && m_nCount > 0)
		{
			spEntity->m_wszReadStoredProc = m_wszReadStoredProc;
			spEntity->m_wszUpdateStoredProc = m_wszUpdateStoredProc;
			spEntity->m_wszDeleteStoredProc = m_wszDeleteStoredProc;
			spEntity->m_wszInsertStoredProc = m_wszInsertStoredProc;
			spEntity->m_wszKeyField = m_wszKeyField;
			hr = spEntity->put_EditMode(colEditNone);
			if (SUCCEEDED(hr))
				hr = spEntity->QueryInterface(IID_IEntityData, (void**)ppEntity);
		}
	}
	if (FAILED(hr) || m_nCount <= 0)
	{
		delete spEntity;
		*ppEntity = NULL;
	}
	return hr;
}

HRESULT CProxy::InternalLookup(BSTR SQLString, long *Result, BSTR ReadStoredProc, BSTR UpdateStoredProc, BSTR DeleteStoredProc, BSTR InsertStoredProc, BSTR KeyField)
{
	HRESULT hr = S_OK;
	WCHAR wszSQL[MAX_FIELD_DATA_SIZE];

	hr = QueryEnlist(CLSID_Proxy, IID_IDataProxy);

	if (SUCCEEDED(hr))
	{
		SETCOMPLETE;

		if (ReadStoredProc != NULL)
			m_wszReadStoredProc.AssignCopy(SysStringLen(ReadStoredProc), ReadStoredProc);
		else
			m_wszReadStoredProc.Empty();
		
		if (UpdateStoredProc != NULL)
			m_wszUpdateStoredProc.AssignCopy(SysStringLen(UpdateStoredProc), UpdateStoredProc);
		else
			m_wszUpdateStoredProc.Empty();

		if (DeleteStoredProc != NULL)
			m_wszDeleteStoredProc.AssignCopy(SysStringLen(DeleteStoredProc), DeleteStoredProc);
		else
			m_wszDeleteStoredProc.Empty();

		if (InsertStoredProc != NULL)
			m_wszInsertStoredProc.AssignCopy(SysStringLen(InsertStoredProc), InsertStoredProc);
		else
			m_wszInsertStoredProc.Empty();

		if (KeyField != NULL)
			m_wszKeyField.AssignCopy(SysStringLen(KeyField), KeyField);
		else
			m_wszKeyField.Empty();

		WCSNCPY(wszSQL, SQLString, SysStringLen(SQLString));
		
		hr = ExecuteSQL(wszSQL);
		if (SUCCEEDED(hr))
		{
			*Result = 1;
		}
	}
	return hr;
}

HRESULT CProxy::InternalBindLookup(CStaticEntity *spEntity)
{
	HRESULT hr = S_OK;

	hr = BindResultMetaData(&m_MetaData, &m_nCount);
	if (m_nCount > 0)
	{
		hr = BindStaticEntity(m_MetaData, m_nCount, spEntity);
	}
	return hr;
}

HRESULT CProxy::InternalNextEntity(CStaticEntity *spEntity)
{
	HRESULT hr = S_OK;

	hr = BindStaticEntity(m_MetaData, m_nCount, spEntity);

	return hr;
}

STDMETHODIMP CProxy::NextEntity(IEntityData **ppEntity)
{
	HRESULT hr = S_OK;
	CStaticEntity *spEntity = new CStaticEntity;
	hr = InternalNextEntity(spEntity);
	if (SUCCEEDED(hr))
	{
		spEntity->m_wszReadStoredProc = m_wszReadStoredProc;
		spEntity->m_wszUpdateStoredProc = m_wszUpdateStoredProc;
		spEntity->m_wszDeleteStoredProc = m_wszDeleteStoredProc;
		spEntity->m_wszInsertStoredProc = m_wszInsertStoredProc;
		spEntity->m_wszKeyField = m_wszKeyField;
		hr = spEntity->put_EditMode(colEditNone);
		if (SUCCEEDED(hr))
			hr = spEntity->QueryInterface(IID_IEntityData, (void**)ppEntity);
	}
	else
	{
		delete spEntity;
		*ppEntity = NULL;
	}
	return hr;
}

STDMETHODIMP CProxy::LookupFree()
{
	CloseCurrentStatement();
	m_nCount = -1;
	return S_OK;
}

STDMETHODIMP CProxy::NextResultset(long *pResult, IResultset **ppResults)
{
	HRESULT hr = S_OK;

	hr = NextResultsetData(pResult);

	if (SUCCEEDED(hr))
	{
		hr = CollectResultRows(ppResults);
	}

	if (FAILED(hr))
		return AtlReportError(CLSID_Proxy, (LPCSTR)"Failed to retrieve data", IID_IDataProxy, hr);
	else
		return hr;
}

STDMETHODIMP CProxy::NextLookup(long *pResult, IEntityData **ppEntity)
{
	HRESULT hr = S_OK;
	CStaticEntity *spEntity = new CStaticEntity;

	hr = NextResultsetData(pResult);

	if (SUCCEEDED(hr))
	{
		hr = InternalBindLookup(spEntity);

		if (SUCCEEDED(hr))
		{
			spEntity->m_wszReadStoredProc = m_wszReadStoredProc;
			spEntity->m_wszUpdateStoredProc = m_wszUpdateStoredProc;
			spEntity->m_wszDeleteStoredProc = m_wszDeleteStoredProc;
			spEntity->m_wszInsertStoredProc = m_wszInsertStoredProc;
			spEntity->m_wszKeyField = m_wszKeyField;
			hr = spEntity->put_EditMode(colEditNone);
			if (SUCCEEDED(hr))
				hr = spEntity->QueryInterface(IID_IEntityData, (void**)ppEntity);
		}
	}
	if (FAILED(hr))
	{
		delete spEntity;
		*ppEntity = NULL;
		return AtlReportError(CLSID_Proxy, (LPCSTR)"Failed to bind entity data", IID_IDataProxy, hr);
	}
	return hr;
}
