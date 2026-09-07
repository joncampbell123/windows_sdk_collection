#include "pooleddata.h"

#ifndef DBNTWIN32
#define DBNTWIN32
#include <SQL.h>
#include <SQLEXT.h>
#include <ODBCSS.H>
#endif
#define SQL_FAILED(rc)  ( (rc) != SQL_SUCCESS && (rc) != SQL_SUCCESS_WITH_INFO)
#define ASSERT_SQL_SUCCESS(rc) { _ASSERTE(rc==SQL_SUCCESS || rc==SQL_SUCCESS_WITH_INFO);}
#define PROCESS_SQL_ERROR(_rc) if (SQL_FAILED(_rc)) { char _szError[1024]; ATLTRACE ("ODBC Execution failed in file %s at line %d, code: %#x\n", __FILE__, __LINE__, _rc); if (m_spObjectContext.p != NULL) m_spObjectContext->SetAbort(); GetODBCError(_szError); return AtlReportError(CLSID_Proxy, _szError, IID_IDataProxy, E_FAIL); }

extern void GetError(HENV henv, HDBC hdbc, HSTMT hstmt, BSTR * sErr);

/*
	If you are replacing the CODBCData class (CProxy derives from CODBCData)
	with another data access class e.g. using OLEDB then you will need to 
	derive it from CPooledData, and (re)implement the following protected members:

	The members will be called in the order shown below..

	1. ManualConstructData
		- create a connection to your data source from a given string

	2. CloseCurrentStatement
		- close any command that has been sent to the data store via ExecuteSQL
		i.e. freeing up any binding and cursor data

	3. Enlist
		- enlist the current data connection into the DTC transaction
		that is held within the Object Context

	4. ExecuteSQL
		- send a string command to the data source, and deal with the
		possible single row, single field return value.

	5. NextResultsetData
		- a query may return multiple resultsets this function is called
		to make the next resultset available
  
	6. BindResultMetaData
		- gather the metadata from a resultset into the internal structures

	7. FetchData (potentially called many times)
		- fetch the next row of a resultset and place it's data into the
		metadata structures (Binding can make this a trivial implementation)

    8. DeactivateData
		- IObjectControl::Deactivate has been called, clean up, especially
		if you are being pooled.
*/

class CODBCData : public CPooledData
{
private:
// Data Items
	HENV m_henv;
	HDBC m_hdbc;
	HSTMT m_hstmt;

// Private Members
	void GetODBCError(char *szError);
	HRESULT GetConnection(BSTR sDsn, BOOL bUseObjectPool);
	HRESULT Enlist(IObjectContext * pObjectContext, const CLSID &clsid, const IID &iid);

protected:
	CString m_wszConnection;

	CODBCData()
	{
		m_henv = 0;
		m_hdbc = 0;
		m_hstmt= 0;
	}

	~CODBCData()
	{
		if (m_hstmt)
			SQLFreeStmt(m_hstmt, SQL_DROP);
		if (m_hdbc)
		{
			SQLDisconnect(m_hdbc);	
			SQLFreeConnect(m_hdbc);
		}
		if (m_henv)
			SQLFreeEnv(m_henv);

		m_hstmt = 0;
		m_henv = 0;
		m_hdbc = 0;
	}

	STDMETHOD(ManualConstructData)(BSTR strConstruct)
	{
		HRESULT hr = GetConnection(strConstruct, m_bObjectPooled);
		if (FAILED(hr))
		{
			LPCSTR szError = "Could not connect to the database.";
			return AtlReportError(CLSID_Proxy, szError, IID_IDataProxy, hr);
		}
		m_bConstructed = TRUE;
		return S_OK;
	}

	virtual HRESULT ExecuteSQL(LPCWSTR wszSQL, long *pResult = NULL);
	virtual HRESULT BindResultMetaData(FieldMetaData **ppMetaData, short *pCount);
	virtual inline HRESULT FetchData()
	{
		HRESULT hr = S_OK;
		RETCODE rc;

		rc = SQLFetch(m_hstmt);

		PROCESS_SQL_ERROR(rc);

		return hr;
	}
	virtual void CloseCurrentStatement();
    virtual void STDMETHODCALLTYPE DeactivateData(void)
	{
		m_spObjectContext.Release();
		return;
	}
	virtual HRESULT NextResultsetData(long *pResult);
};
