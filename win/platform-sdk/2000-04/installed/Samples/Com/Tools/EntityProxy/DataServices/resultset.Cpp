// Resultset.cpp : Implementation of CResultset
#include "stdafx.h"
#include "Data.h"
#include "Entity.h"
#include "Resultset.h"

/////////////////////////////////////////////////////////////////////////////
// CResultset

STDMETHODIMP CResultset::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IResultset
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CResultset::Count(long* pCount)
{
	HRESULT hr = S_OK;

	*pCount = m_nCount;

	return hr;
}

STDMETHODIMP CResultset::_NewEnum(IUnknown** pVal)
{
	HRESULT hr;
	IUnknown *pThisUnk;

	if (pVal == NULL)
		return E_POINTER;
	*pVal = NULL;

	if (m_pVar == NULL)
	{
		m_pVar = new CFastVariant[m_nCount];
	}

	for (int n = 0; n < m_nCount; n++)
	{
		IEntityData *pEntity;
		
		get_Record(n + 1, &pEntity);

		m_pVar[n] = pEntity;
	}

	typedef CComObject<CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > > enumvar;
	enumvar* pEnumVar = new enumvar;
	_ASSERTE(pEnumVar);

	hr = QueryInterface(IID_IUnknown, (void**)&pThisUnk);

	if (SUCCEEDED(hr))
	{
		hr = pEnumVar->Init(&m_pVar[0], &m_pVar[m_nCount], pThisUnk, AtlFlagNoCopy);
	}
	if (SUCCEEDED(hr))
	{
		hr = pEnumVar->QueryInterface(IID_IEnumVARIANT, (void**)pVal);
		pThisUnk->Release();
	}
	if (FAILED(hr))
		delete pEnumVar;
	return hr;
}

STDMETHODIMP CResultset::get_Record(long nIndex, IEntityData **ppEntity)
{
	HRESULT hr = S_OK;
	if (nIndex < 1)
	{
		hr = DISP_E_BADINDEX;
	}
	else
	{
		CStaticEntity *spEntity = GetEntity(nIndex - 1);

		if (spEntity->m_wszReadStoredProc == L"")
			spEntity->m_wszReadStoredProc = m_wszReadStoredProc;

		if (spEntity->m_wszUpdateStoredProc == L"")
			spEntity->m_wszUpdateStoredProc = m_wszUpdateStoredProc;

		if (spEntity->m_wszDeleteStoredProc == L"")
			spEntity->m_wszDeleteStoredProc = m_wszDeleteStoredProc;

		if (spEntity->m_wszInsertStoredProc == L"")
			spEntity->m_wszInsertStoredProc = m_wszInsertStoredProc;

		if (spEntity->m_wszKeyField == L"")
			spEntity->m_wszKeyField = m_wszKeyField;

		hr = spEntity->QueryInterface(IID_IEntityData, (void**)ppEntity);
	}
	return hr;
}
/*
CStaticEntity *CResultset::GetEntity(long nIndex)
{
	CStaticEntity *pEntity = NULL;
	if (nIndex >= 0)
	{
		int nEntity = nIndex % ENTITY_PER_BANK;
		int nBank = (nIndex - nEntity) / ENTITY_PER_BANK;

		if (nIndex >= m_nCount)
			m_nCount = nIndex + 1;

		if (nBank >= m_nBankCount)
		{
			m_nBankCount++;
			m_pEntity[m_nBankCount - 1] = new CStaticEntity[ENTITY_PER_BANK];
		}
		pEntity = &m_pEntity[nBank][nEntity];
	}
	return pEntity;
}
*/
CStaticEntity *CResultset::GetEntity(long nIndex)
{
	CStaticEntity *pEntity = NULL;
	if (nIndex >= 0)
	{
		if (nIndex >= m_nCount)
		{
			m_nCount = nIndex + 1;
			m_pEntity[nIndex] = new CStaticEntity;
			m_pEntity[nIndex]->AddRef();
		}
		pEntity = m_pEntity[nIndex];
	}
	return pEntity;
}

HRESULT CResultset::Initialise(long nRows)
{
	m_nCount = 0;

	if (m_pVar != NULL)
	{
		delete [] m_pVar;
		m_pVar = NULL;
	}

	return S_OK;
}

void CResultset::DeleteEntity(long nIndex)
{
	if (nIndex >= 0 && nIndex < m_nCount)
	{
		m_pEntity[nIndex]->Release();
//		delete m_pEntity[nIndex];
	}
}

void CResultset::SetCount(long nCount)
{
	m_nCount = nCount;
}
