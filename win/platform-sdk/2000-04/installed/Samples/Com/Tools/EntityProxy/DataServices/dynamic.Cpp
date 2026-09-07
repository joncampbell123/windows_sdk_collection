// Dynamic.cpp : Implementation of CDynamic
#include "stdafx.h"
#include "Data.h"
#include "Dynamic.h"

/////////////////////////////////////////////////////////////////////////////
// CDynamic

void CDynamic::Clear()
{
	m_nReserve = 50;
	m_nGrow = 50;
	m_bRequiresSave = false;

	m_VarVect.clear();
	m_IndexMap.clear();
	m_MapIndex.clear();

	m_VarVect.reserve(m_nReserve);
}

STDMETHODIMP CDynamic::InterfaceSupportsErrorInfo(REFIID riid)
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

STDMETHODIMP CDynamic::Count(long * pVal)
{
	// STL is not thread-safe
	// so use a Critical Section to project
	// it's delecate structures
	Lock();

	if (pVal == NULL)
		return E_POINTER;
	*pVal = m_VarVect.size();

	// Anyone else can play, now!
	Unlock();
	return S_OK;
}

STDMETHODIMP CDynamic::Item(VARIANT* pIndex, VARIANT * pVal)
{
	if (pVal == NULL)
		return E_POINTER;

	long nIndex = 0;

	pVal->vt = VT_EMPTY;
	pVal->punkVal = NULL;

	// How are we being addressed - by key or by index?
	if (pIndex->vt == VT_BSTR)
	{
		_bstr_t bstrKey(pIndex->bstrVal);
		nIndex = m_IndexMap[bstrKey];
	}
	else // by index
	{
		_variant_t varIndex(pIndex);
		nIndex = (long)varIndex;
	}
	
	// use 1-based index, VB like
	if (((long)nIndex < 1) || ((long)nIndex > (m_VarVect.size())))
		return E_INVALIDARG;
	VariantCopy(pVal, &m_VarVect[(long)nIndex-1]);
	return S_OK;
}

STDMETHODIMP CDynamic::_NewEnum(IUnknown** pVal)
{
	// STL Vectors are held in contiguous memory - so
	// we can use CComEnum to get the items out - cool!
	if (pVal == NULL)
		return E_POINTER;
	*pVal = NULL;
	typedef CComObject<CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT> > > enumvar;
	enumvar* pEnumVar = new enumvar;
	_ASSERTE(pEnumVar);
	HRESULT hRes = pEnumVar->Init(m_VarVect.begin(), m_VarVect.end(), NULL, AtlFlagCopy);
	if (SUCCEEDED(hRes))
		hRes = pEnumVar->QueryInterface(IID_IEnumVARIANT, (void**)pVal);
	if (FAILED(hRes))
		delete pEnumVar;
	return hRes;
}

STDMETHODIMP CDynamic::Add(VARIANT *pItem, VARIANT *pKey, VARIANT *pBefore, VARIANT *pAfter)
{
	// create a CFastVariant and add it to the collection
	HRESULT hr = E_FAIL;
	long nDistance, nCount, nRemap;
	_bstr_t bstrKey;

	// STL is not thread-safe
	// so use a Critical Section to project
	// it's delecate structures
	Lock();

	if (pBefore->vt != VT_ERROR && pAfter->vt != VT_ERROR)
	{
		hr = DISP_E_BADPARAMCOUNT;
	}
	else
	{
		long nAdvance;
		_bstr_t bstrKeyIn;
		varvect::iterator it = m_VarVect.begin();
		int nCap = m_VarVect.capacity();
		if (nCap > m_nReserve)
		{
			m_nReserve += m_nGrow;
			m_VarVect.reserve(m_nReserve);
		}

		// Where in the vector should the new item go?
		if (pBefore->vt == VT_BSTR)
		{
			bstrKeyIn = pBefore->bstrVal;
			nAdvance = m_IndexMap[bstrKeyIn] - 1;
			std::advance(it, nAdvance);
		}
		else if (pAfter->vt == VT_BSTR)
		{
			bstrKeyIn = pAfter->bstrVal;
			nAdvance = m_IndexMap[bstrKeyIn];
			std::advance(it, nAdvance);
		}
		else
		{
			it = m_VarVect.end();
		}

		it = m_VarVect.insert(it, *pItem);
		nDistance = std::distance( m_VarVect.begin(), it) + 1;

		// If we don't have a key, just fabricate one from
		// the location in the vector.
		if (pKey->vt == VT_BSTR)
		{
			bstrKeyIn = pKey->bstrVal;
		}
		else
		{
			WCHAR wszKey[64];
			swprintf(wszKey, L"%lu", nDistance);
			bstrKeyIn = wszKey;
		}

		// Since we are adding a new entry to the vector,
		// we need to adjust all the affected items in the
		// lookup maps to compensate for the newly increase
		// index into the Vector.
		for (nCount = m_VarVect.size() - 1; nCount >= nDistance; nCount--)
		{
			bstrKey = m_MapIndex[nCount];
			nRemap = m_IndexMap[bstrKey] + 1;

			m_IndexMap[bstrKey] = nRemap;
			m_MapIndex.erase(nCount);
			m_MapIndex[nRemap] = bstrKey;
		}

		// Add the new entry into the Index Maps - there will be a newly
		// opened space in the logical sequence by the above code.
		m_IndexMap.insert(bstr2long::value_type(bstrKeyIn.copy(), nDistance));
		m_MapIndex.insert(long2bstr::value_type(nDistance, bstrKeyIn.copy()));
		m_bRequiresSave = true;
	}
	// Anyone else can play, now!
	Unlock();
	return S_OK;
}

STDMETHODIMP CDynamic::Remove(VARIANT *pItem)
{
	// STL is not thread-safe
	// so use a Critical Section to project
	// it's delecate structures
	Lock();

	// create a CFastVariant and add it to the collection
	long nIndex, nCount, nRemap;
	_bstr_t bstrKey;
	varvect::iterator it = m_VarVect.begin();

	if (pItem->vt == VT_BSTR)
	{
		_bstr_t bstrKey(pItem->bstrVal);
		nIndex = m_IndexMap[bstrKey];
	}
	else
	{
		_variant_t varIndex(pItem);
		nIndex = (long)varIndex;
	}

	// Remove the item from the vector,
	// and delete it's index map entries
	std::advance(it, nIndex - 1);
	m_VarVect.erase(it);
	bstrKey = m_MapIndex[nIndex];
	m_MapIndex.erase(nIndex);
	m_IndexMap.erase(bstrKey);

	// Since we are removing an entry from the vector,
	// we need to adjust all the affected items in the
	// lookup maps to compensate for the newly decreased
	// index into the Vector.
	for (nCount = nIndex + 1; nCount <= m_VarVect.size(); nCount++)
	{
		bstrKey = m_MapIndex[nCount];
		nRemap = m_IndexMap[bstrKey];

		m_IndexMap[bstrKey] = nRemap - 1;
		m_MapIndex.erase(nCount);
		m_MapIndex[nRemap-1] = bstrKey;
	}
	m_bRequiresSave = true;

	Unlock();	
	return S_OK;
}
