#include "stdafx.h"
#include "Data.h"
#include "Entity.h"
#include "Dynamic.h"
#include "Resultset.h"
#include "odbcdata.h"
#include "Proxy.h"

HRESULT CODBCData::BindResultMetaData(FieldMetaData **ppMetaData, short *pCount)
{
	short nCol;
	HRESULT hr = S_OK;
	RETCODE rc = 0;
	short nsChars = 0;
	WCHAR wszFieldName[FIELDNAME_MAXSIZE];

	rc = SQLNumResultCols(m_hstmt, pCount);

	PROCESS_SQL_ERROR(rc);

	if (*ppMetaData == NULL)
	{
		*ppMetaData = new FieldMetaData[(*pCount) + 1];
	}

	for (nCol = 1; nCol <= *pCount; nCol++)
	{
		short nIndex = nCol-1;

		FieldMetaData *pMeta = &(*ppMetaData)[nIndex];

		rc = SQLDescribeColW(m_hstmt, (short)nCol, wszFieldName, (short)FIELDNAME_MAXSIZE - 1, &nsChars, (short*)&pMeta->sqldatatype, &pMeta->nDataLen, &pMeta->nScale, &pMeta->nNullable);
		PROCESS_SQL_ERROR(rc);

		pMeta->wszName.AssignCopy(nsChars, wszFieldName);

		pMeta->vt = VT_EMPTY;
		pMeta->bNull = false;

		switch(pMeta->sqldatatype)
		{
		case SQL_TINYINT:
			pMeta->nDataLen = sizeof(long);
			pMeta->vt = VT_UI1;
			pMeta->sqltype = SQL_C_SLONG;
			break;
		case SQL_SMALLINT:
			pMeta->nDataLen = sizeof(long);
			pMeta->vt = VT_I2;
			pMeta->sqltype = SQL_C_SLONG;
			break;
		case SQL_NUMERIC:
		case SQL_INTEGER:
			pMeta->nDataLen = sizeof(long);
			pMeta->vt = VT_I4;
			pMeta->sqltype = SQL_C_SLONG;
			break;
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TYPE_TIMESTAMP:
		case 11: // datetime?
			pMeta->nDataLen = MAX_FIELD_DATA_SIZE - 1;
			pMeta->vt = VT_DATE;
			pMeta->sqltype = SQL_C_WCHAR;
			break;
		case SQL_BIT:
			pMeta->nDataLen = sizeof(bool);
			pMeta->vt = VT_BOOL;
			pMeta->sqltype = SQL_C_SLONG;
			break;
		case SQL_DECIMAL:
			pMeta->nDataLen = MAX_FIELD_DATA_SIZE - 1;
			if (pMeta->nScale == 4)
				pMeta->vt = VT_CY;
			else
				pMeta->vt = VT_DECIMAL;
			pMeta->sqltype = SQL_C_WCHAR;
			break;
		case SQL_VARCHAR:
		case SQL_CHAR:
		case SQL_LONGVARCHAR:
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		default:
			pMeta->nDataLen = MAX_FIELD_DATA_SIZE - 1;
			pMeta->vt = VT_BSTR;
			pMeta->sqltype = SQL_C_WCHAR;
			break;
		}

		rc = SQLBindCol(m_hstmt, (short)nCol, pMeta->sqltype, (void*)pMeta->pData, MAX_FIELD_DATA_SIZE - 1, &pMeta->nDataBindLen);
		PROCESS_SQL_ERROR(rc);		
	}
	return hr;
}

HRESULT CODBCData::GetConnection(BSTR sDsn, BOOL bUseObjectPool)
{
	RETCODE rc;
	HRESULT hr= S_OK;
	
	// We're object pooling, so we don't need connection pooling
	if (bUseObjectPool)
	{
		rc = SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (void*)SQL_CP_OFF, SQL_IS_INTEGER);
		ASSERT_SQL_SUCCESS(rc);
	}
	else
	{
		rc = SQLSetEnvAttr(SQL_NULL_HENV, SQL_ATTR_CONNECTION_POOLING, (void*)SQL_CP_ONE_PER_DRIVER, SQL_IS_INTEGER);
		ASSERT_SQL_SUCCESS(rc); 
	}

	rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HENV, &m_henv);
	ASSERT_SQL_SUCCESS(rc);
	
	rc = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
	ASSERT_SQL_SUCCESS(rc);

	rc = SQLSetEnvAttr(m_henv, SQL_ATTR_CP_MATCH, (SQLPOINTER) SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);
	ASSERT_SQL_SUCCESS(rc);

	rc = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);

	if (SQL_FAILED(rc))
		return E_FAIL;

	rc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_ODBC_CURSORS, NULL, SQL_CUR_USE_ODBC);

	if (SQL_FAILED(rc))
		return E_FAIL;
	WCHAR szConn[1024];
	WORD nSize = 1023;
	m_wszConnection.AssignCopy(SysStringLen(sDsn), sDsn);

	rc = SQLDriverConnectW(m_hdbc, NULL, (SQLWCHAR*)(LPCWSTR)m_wszConnection, SQL_NTS, (SQLWCHAR *)szConn, 1023, (short *)&nSize, (UWORD)SQL_DRIVER_NOPROMPT);
	if (SQL_FAILED(rc))
	{
		SQLFreeEnv(m_henv);
		return E_FAIL;
	}
	rc = SQLAllocStmt(m_hdbc, &m_hstmt); // generic stmt
	if (SQL_FAILED(rc))
	{
		SQLFreeConnect(m_hdbc);
		SQLFreeEnv(m_henv);

		return E_FAIL;
	}
	return S_OK;
}

HRESULT CODBCData::Enlist(IObjectContext * pObjectContext, const CLSID &clsid, const IID &iid)
{
	HRESULT hr = E_FAIL;
	_ASSERTE(pObjectContext);
	
	IObjectContextInfo	* pObjTx = NULL;
	hr = pObjectContext->QueryInterface(IID_IObjectContextInfo, (void **)&pObjTx);
	if (pObjTx)
	{
		ITransaction * pTx = NULL;
		hr = pObjTx->GetTransaction ((IUnknown **)&pTx);
		RETCODE rc;
		if (pTx)
		{
			rc = SQLSetConnectAttr(m_hdbc,SQL_ATTR_ENLIST_IN_DTC, pTx, 0);
			if (SQL_FAILED(rc))
			{
				char szError[1024];

				ATLTRACE ("Enlist failed: %#x\n", rc);
				
				GetODBCError(szError);
				hr = AtlReportError(clsid, szError, iid, E_FAIL);
			}
			else
			{
				hr = S_OK;
			}
			pTx->Release();
		}
		pObjTx->Release();
	}
	return hr;
}

void CODBCData::GetODBCError(char *szError)
{
	char szSqlState[64];
	SDWORD NativeError;
	SWORD cbErrorMsg;
	SQLError(m_henv, m_hdbc, m_hstmt, (SQLCHAR*)szSqlState, &NativeError, (SQLCHAR*)szError, 1023, &cbErrorMsg);
}

HRESULT CODBCData::ExecuteSQL(LPCWSTR wszSQL, long *pResult)
{
	RETCODE rc = 0;
	HRESULT hr = S_OK;
	long nResult = -1;
	long nData = -1;
	long nDataLen = sizeof(long);
	long nDataBindLen = sizeof(long);
	SQLSMALLINT sqltype = SQL_C_SLONG;

	SETCOMPLETE;

	rc = SQLExecDirectW(m_hstmt, (LPWSTR)wszSQL, SQL_NTS);
	
	PROCESS_SQL_ERROR(rc);

	// Only ask for results if we have been asked for results.
	if (pResult != NULL)
	{
		rc = SQLRowCount(m_hstmt, &nResult);

		PROCESS_SQL_ERROR(rc);
		
	// This may fail (so don't check return value) if there is no data to return
		rc = SQLFetch(m_hstmt);

	//	PROCESS_SQL_ERROR(rc);

		if (!SQL_FAILED(rc))
		{
			rc = SQLGetData(m_hstmt, (SQLUSMALLINT)1, sqltype, (void*)&nData, nDataLen, &nDataBindLen);
		//	PROCESS_SQL_ERROR(rc);
		}

		if (nData != -1)
		{
			nResult = nData;
		}
		
		*pResult = nResult;
	}

	return hr;
}

void CODBCData::CloseCurrentStatement()
{
	SQLCloseCursor(m_hstmt);
	SQLFreeStmt(m_hstmt, SQL_UNBIND);
	SQLFreeStmt(m_hstmt, SQL_CLOSE);
}

HRESULT CODBCData::NextResultsetData(long *pResult)
{
	RETCODE rc = 0;
	HRESULT hr = S_OK;

	rc = SQLMoreResults(m_hstmt);

	if (pResult != NULL)
	{
		rc = SQLRowCount(m_hstmt, pResult);

		PROCESS_SQL_ERROR(rc);
	}
	return hr;
}
