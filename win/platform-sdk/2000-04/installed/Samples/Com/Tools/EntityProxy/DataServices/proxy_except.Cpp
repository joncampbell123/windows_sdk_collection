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
// CProxy methods that require exceptions turned on

STDMETHODIMP CProxy::RSQuery(BSTR QueryText, VARIANT *Result, _Recordset **rs)
{
	HRESULT hr = S_OK;

	if (m_spObjectContext.p != NULL)
		m_spObjectContext->SetComplete();

	try
	{
		_CommandPtr spCommand(__uuidof(Command));
		_RecordsetPtr spRecordset(__uuidof(Recordset));

		if ((_Connection*)m_spConnection == NULL)
		{
			m_spConnection.CreateInstance(__uuidof(Connection));
			m_spConnection->Open(_bstr_t(m_wszConnection), "", "", -1);
		}

		spCommand->PutActiveConnection((_Connection*)m_spConnection);
		spCommand->CommandText = QueryText;
		spCommand->CommandType = adCmdText;
		spCommand->Parameters->Append(spCommand->CreateParameter("RETURN_VALUE", adInteger, adParamReturnValue, sizeof(int)));

		spRecordset->CursorLocation = adUseClient;
		spRecordset->Open((_Command*)spCommand, &vtMissing, adOpenForwardOnly, adLockReadOnly, -1);
		spCommand->PutRefActiveConnection(NULL);

		try
		{
			if (spRecordset->Fields->Count > 0)
			{
				spRecordset->PutRefActiveConnection(NULL);
				*rs = spRecordset.Detach();
			}
		}
		catch (_com_error e)
		{
		}

		if (spCommand->Parameters->Count > 0)
			*Result = spCommand->Parameters->Item["RETURN_VALUE"]->Value;
		else
			*Result = _variant_t(1L);
	}
	catch (_com_error e)
	{
		*Result = _variant_t(-1L);
		if (m_spObjectContext.p != NULL)
			m_spObjectContext->SetAbort();

		hr = AtlReportError(CLSID_Proxy, (LPCSTR)e.Description(), IID_IDataProxy, e.Error());
	}
	return hr;
}

STDMETHODIMP CProxy::RSQueryRW(BSTR QueryText, VARIANT *Result, _Recordset **rs)
{
	HRESULT hr = S_OK;

	if (m_spObjectContext.p != NULL)
		m_spObjectContext->SetComplete();

	try
	{
		_CommandPtr spCommand(__uuidof(Command));
		_RecordsetPtr spRecordset(__uuidof(Recordset));

		if ((_Connection*)m_spConnection == NULL)
		{
			m_spConnection.CreateInstance(__uuidof(Connection));
			m_spConnection->Open(_bstr_t(m_wszConnection), "", "", -1);
		}

		spCommand->PutActiveConnection((_Connection*)m_spConnection);
		spCommand->CommandText = QueryText;
		spCommand->CommandType = adCmdText;
		spCommand->Parameters->Append(spCommand->CreateParameter("RETURN_VALUE", adInteger, adParamReturnValue, sizeof(int)));

		spRecordset->CursorLocation = adUseClient;
		spRecordset->Open((_Command*)spCommand, &vtMissing,  adOpenStatic,  adLockBatchOptimistic, -1);

		spCommand->PutRefActiveConnection(NULL);
		try
		{
			if (spRecordset->Fields->Count > 0)
			{
				spRecordset->PutRefActiveConnection(NULL);
				*rs = spRecordset.Detach();
			}
		}
		catch (_com_error e)
		{
		}

		if (spCommand->Parameters->Count > 0)
			*Result = spCommand->Parameters->Item["RETURN_VALUE"]->Value;
		else
			*Result = _variant_t(1L);
	}
	catch (_com_error e)
	{
		*Result = _variant_t(-1L);

		if (m_spObjectContext.p != NULL)
			m_spObjectContext->SetAbort();

		hr = AtlReportError(CLSID_Proxy, (LPCSTR)e.Description(), IID_IDataProxy, e.Error());
	}
	return hr;
}

STDMETHODIMP CProxy::RSUpdate(_Recordset *rs)
{
	HRESULT hr = S_OK;

	if (m_spObjectContext.p != NULL)
		m_spObjectContext->SetComplete();

	try
	{
		if ((_Connection*)m_spConnection == NULL)
		{
			m_spConnection.CreateInstance(__uuidof(Connection));
			m_spConnection->Open(_bstr_t(m_wszConnection), "", "", -1);
		}

		_RecordsetPtr spRecordset(__uuidof(Recordset));
		spRecordset->PutActiveConnection((_Connection*)m_spConnection);
		spRecordset->PutRefSource(rs);
		spRecordset->put_CursorLocation(adUseClient);
		spRecordset->Open(&vtMissing, &vtMissing,adOpenStatic,adLockBatchOptimistic,-1);
		spRecordset->UpdateBatch(adAffectAll);
		spRecordset->putref_ActiveConnection(NULL);
	}
	catch(_com_error e)
	{
		if (m_spObjectContext.p != NULL)
			m_spObjectContext->SetAbort();

		hr = AtlReportError(CLSID_Proxy, (LPCSTR)e.Description(), IID_IDataProxy, e.Error());
	}
	return hr;
}
