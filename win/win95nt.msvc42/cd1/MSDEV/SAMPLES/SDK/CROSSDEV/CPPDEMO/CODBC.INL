/*--------------------------------------------------------------------------
  CODBC.INL - Low-level ODBC object classes inline functions

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/

#ifndef __CODBC_INL
#define __CODBC_INL

#if defined(_AFX) && defined(_DEBUG) && !defined(new)
#define new DEBUG_NEW                  // Debugging version of new (MFC)
#endif

#if !defined(_AFX) || !defined(_DEBUG)
#define ASSERT(x)   (x)
#endif

////////////////////////////////////////////////////////////////////////////
// CODBC
inline CODBC::CODBC()
{
	m_rc = SQL_SUCCESS;
	return;
}

inline CODBC::~CODBC()
{
	return;
}

inline RETCODE CODBC::Error(HENV    henv,
							HDBC     hdbc,
							HSTMT    hstmt,
							LPSTR    szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax,
							LPSWORD  cbErrorMsg)
{
	m_rc = ::SQLError(henv, hdbc, hstmt,
					(UCHAR FAR *)szSqlState,
					pfNativeError,
					(UCHAR FAR *)szErrorMsg,
					cbErrorMsgMax, cbErrorMsg);
	return m_rc;
}

inline BOOL CODBC::Success(RETCODE rc)
{
	return (rc >= 0);
}


#ifdef _AFX
////////////////////////////////////////////////////////////////////////////
// CODBCException
inline CODBCException::CODBCException(CODBC*   podbc,
										UWORD   fFunction,
										RETCODE rc)
{
	m_podbc     = podbc;
	m_fFunction = fFunction;
	m_rc        = rc;
	return;
}

inline void ThrowODBCException(CODBC*  podbc,
								UWORD   fFunction,
								RETCODE rc)
{
	THROW(new CODBCException(podbc, fFunction, rc));
	return;
}
#endif


////////////////////////////////////////////////////////////////////////////
// CENV
inline CENV::CENV()
{
	m_henv = SQL_NULL_HENV;
	m_rc   = ::SQLAllocEnv(&m_henv);
	CRIT_CHECK_EX(SQLALLOCENV);
	return;
}

inline CENV::~CENV()
{
	if (m_henv != SQL_NULL_HENV) {
		m_rc = ::SQLFreeEnv(m_henv);
		CRIT_CHECK_EX(SQLFREEENV);
	}
	return;
}

inline CENV::operator HENV() const
{
	return m_henv;
}

inline RETCODE CENV::Error(LPSTR    szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax,
							LPSWORD  pcbErrorMsg)
{
	ASSERT(m_henv != SQL_NULL_HENV);
	return CODBC::Error(m_henv, SQL_NULL_HDBC, SQL_NULL_HSTMT,
						szSqlState, pfNativeError, szErrorMsg,
						cbErrorMsgMax, pcbErrorMsg);
}

inline CDBC* CENV::AllocConnect()
{
	ASSERT(m_henv != SQL_NULL_HENV);
	return new CDBC(this);
}

inline RETCODE CENV::DataSources(UWORD fDirection,
								LPSTR   szDSN,
								LPSTR   szDescription,
								SWORD   cbDescription)
{
	return DataSources(fDirection, szDSN, SQL_MAX_DSN_LENGTH-1, NULL,
							szDescription, cbDescription, NULL);
}

inline RETCODE CENV::DataSources(UWORD fDirection,
								LPSTR   szDSN,
								SWORD   cbDSNMax,
								LPSWORD pcbDSN,
								LPSTR   szDescription,
								SWORD   cbDescriptionMax,
								LPSWORD pcbDescription)
{
	ASSERT(m_henv != SQL_NULL_HENV);
	m_rc = ::SQLDataSources(m_henv, fDirection,
							(UCHAR FAR *)szDSN, cbDSNMax, pcbDSN,
							(UCHAR FAR *)szDescription, cbDescriptionMax,
							pcbDescription);
	CHECK_EX(SQLDATASOURCES);
	return m_rc;
}

inline RETCODE CENV::Transact(UWORD fType)
{
	m_rc = ::SQLTransact(m_henv, SQL_NULL_HDBC, fType);
	CHECK_EX(SQLTRANSACT);
	return m_rc;
}


////////////////////////////////////////////////////////////////////////////
// CDBC
inline CDBC::CDBC(CENV& env)
{
	Initialize(&env);
	return;
}

inline CDBC::CDBC(CENV* penv)
{
	Initialize(penv);
	return;
}

inline void CDBC::Initialize(CENV* penv)
{
	ASSERT(penv != NULL);

	m_hdbc = SQL_NULL_HDBC;
	m_penv = penv;
	m_rc   = ::SQLAllocConnect(*penv, &m_hdbc);
	CRIT_CHECK_EX(SQLALLOCCONNECT);
	return;
}

inline CDBC::~CDBC()
{
	if (m_hdbc != SQL_NULL_HDBC) {
		m_rc = ::SQLFreeConnect(m_hdbc);
		CRIT_CHECK_EX(SQLFREECONNECT);
	}
	return;
}

inline CDBC::operator HDBC() const
{
	return m_hdbc;
}

inline CDBC::operator CENV*() const
{
	ASSERT(m_penv != NULL);
	return m_penv;
}

inline CDBC::operator CENV&() const
{
	ASSERT(m_penv != NULL);
	return *m_penv;
}

inline RETCODE CDBC::Error(LPSTR    szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax,
							LPSWORD  pcbErrorMsg)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	return CODBC::Error(SQL_NULL_HENV, m_hdbc, SQL_NULL_HSTMT,
						szSqlState, pfNativeError, szErrorMsg,
						cbErrorMsgMax, pcbErrorMsg);
}

inline CSTMT* CDBC::AllocStmt()
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	return new CSTMT(this);
}

inline RETCODE CDBC::BrowseConnect(LPCSTR szConnStrIn,
									LPSTR  szConnStrOut,
									SWORD  cbConnStrOutMax)
{
	return BrowseConnect(szConnStrIn, SQL_NTS, szConnStrOut, cbConnStrOutMax);
}

inline RETCODE CDBC::BrowseConnect(LPCSTR  szConnStrIn,
									SWORD   cbConnStrIn,
									LPSTR   szConnStrOut,
									SWORD   cbConnStrOutMax,
									LPSWORD pcbConnStrOut)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLBrowseConnect(m_hdbc, (UCHAR FAR *)szConnStrIn, cbConnStrIn,
								(UCHAR FAR *)szConnStrOut, cbConnStrOutMax,
								pcbConnStrOut);
	CHECK_EX(SQLBROWSECONNECT);
	return m_rc;
}

inline RETCODE CDBC::Connect(LPCSTR szDSN, LPCSTR szUID, LPCSTR szAuthStr)
{
	return Connect(szDSN, SQL_NTS, szUID, SQL_NTS, szAuthStr, SQL_NTS);
}

inline RETCODE CDBC::Connect(LPCSTR szDSN,
							SWORD    cbDSN,
							LPCSTR   szUID,
							SWORD    cbUID,
							LPCSTR   szAuthStr,
							SWORD    cbAuthStr)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLConnect(m_hdbc, (UCHAR FAR *)szDSN, cbDSN,
						(UCHAR FAR *)szUID, cbUID, 
						(UCHAR FAR *)szAuthStr, cbAuthStr);
	CHECK_EX(SQLCONNECT);
	return m_rc;
}

inline RETCODE CDBC::Disconnect()
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLDisconnect(m_hdbc);
	CHECK_EX(SQLDISCONNECT);
	return m_rc;
}

inline RETCODE CDBC::DriverConnect(HWND   hwnd,
									LPCSTR szConnStrIn,
									UWORD  fDriverCompletion)
{
	return DriverConnect(hwnd, szConnStrIn, SQL_NTS,
							NULL, 0, NULL, fDriverCompletion);
}

inline RETCODE CDBC::DriverConnect(HWND   hwnd,
									LPCSTR szConnStrIn,
									LPSTR  szConnStrOut,
									SWORD  cbConnStrOutMax,
									UWORD  fDriverCompletion)
{
	return DriverConnect(hwnd, szConnStrIn, SQL_NTS,
					szConnStrOut, cbConnStrOutMax, NULL, fDriverCompletion);
}

inline RETCODE CDBC::DriverConnect(HWND    hwnd,
									LPCSTR  szConnStrIn,
									SWORD   cbConnStrIn,
									LPSTR   szConnStrOut,
									SWORD   cbConnStrOutMax,
									LPSWORD pcbConnStrOut,
									UWORD   fDriverCompletion)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
#ifndef _MAC
	m_rc = ::SQLDriverConnect(m_hdbc, hwnd,
						(UCHAR FAR *)szConnStrIn, cbConnStrIn,
						(UCHAR FAR *)szConnStrOut, cbConnStrOutMax,
						pcbConnStrOut, fDriverCompletion);
#else
	m_rc = ::SQLDriverConnect(m_hdbc, GetWrapperWindow(hwnd),
						(UCHAR FAR *)szConnStrIn, cbConnStrIn,
						(UCHAR FAR *)szConnStrOut, cbConnStrOutMax,
						pcbConnStrOut, fDriverCompletion);
#endif
	CHECK_EX(SQLDRIVERCONNECT);
	return m_rc;
}

inline RETCODE CDBC::GetConnectOption(UWORD fOption, PTR pvParam)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLGetConnectOption(m_hdbc, fOption, pvParam);
	CHECK_EX(SQLGETCONNECTOPTION);
	return m_rc;
}

inline RETCODE CDBC::GetFunctions(UWORD fFunction, LPUWORD pfExists)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLGetFunctions(m_hdbc, fFunction, pfExists);
	CHECK_EX(SQLGETFUNCTIONS);
	return m_rc;
}

inline RETCODE CDBC::GetInfo(UWORD fInfoType,
							PTR     rgbInfoValue,
							SWORD   cbInfoValueMax,
							LPSWORD pcbInfoValue)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLGetInfo(m_hdbc, fInfoType, 
						rgbInfoValue, cbInfoValueMax, pcbInfoValue);
	CHECK_EX(SQLGETFUNCTIONS);
	return m_rc;
}

inline RETCODE CDBC::NativeSql(LPCSTR   szSqlStrIn,
								LPSTR    szSqlStr,
								SDWORD   cbSqlStrMax,
								LPSDWORD pcbSqlStr)
{
	return NativeSql(szSqlStrIn, SQL_NTS, szSqlStr, cbSqlStrMax, pcbSqlStr);
}

inline RETCODE CDBC::NativeSql(LPCSTR   szSqlStrIn,
								SDWORD   cbSqlStrIn,
								LPSTR    szSqlStr,
								SDWORD   cbSqlStrMax,
								LPSDWORD pcbSqlStr)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLNativeSql(m_hdbc, (UCHAR FAR *)szSqlStrIn, cbSqlStrIn,
							(UCHAR FAR *)szSqlStr, cbSqlStrMax, pcbSqlStr);
	CHECK_EX(SQLNATIVESQL);
	return m_rc;
}

inline RETCODE CDBC::SetConnectOption(UWORD fOption, UDWORD vParam)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLSetConnectOption(m_hdbc, fOption, vParam);
	CHECK_EX(SQLSETCONNECTOPTION);
	return m_rc;
}

inline RETCODE CDBC::Transact(UWORD fType)
{
	ASSERT(m_hdbc != SQL_NULL_HDBC);
	m_rc = ::SQLTransact(SQL_NULL_HENV, m_hdbc, fType);
	CHECK_EX(SQLTRANSACT);
	return m_rc;
}


////////////////////////////////////////////////////////////////////////////
// CSTMT
inline CSTMT::CSTMT(CDBC& dbc)
{
	Initialize(&dbc);
	return;
}

inline CSTMT::CSTMT(CDBC* pdbc)
{
	Initialize(pdbc);
	return;
}

inline void CSTMT::Initialize(CDBC* pdbc)
{
	ASSERT(pdbc != NULL);

	m_hstmt = SQL_NULL_HSTMT;
	m_pdbc  = pdbc;
	m_penv  = (CENV*)pdbc;

    m_rc    = ::SQLAllocStmt(HDBC(*pdbc), &m_hstmt);
	CRIT_CHECK_EX(SQLALLOCSTMT);
	return;
}

inline CSTMT::~CSTMT()
{
	if (m_hstmt != SQL_NULL_HSTMT) {
		m_rc = ::SQLFreeStmt(m_hstmt, SQL_DROP);
		CRIT_CHECK_EX(SQLFREESTMT);
	}
	return;
}

inline CSTMT::operator HSTMT() const
{
	return m_hstmt;
}

inline CSTMT::operator CDBC*() const
{
	ASSERT(m_pdbc != NULL);
	return m_pdbc;
}

inline CSTMT::operator CDBC&() const
{
	ASSERT(m_pdbc != NULL);
	return *m_pdbc;
}

inline CSTMT::operator CENV*() const
{
	ASSERT(m_penv != NULL);
	return m_penv;
}

inline CSTMT::operator CENV&() const
{
	ASSERT(m_penv != NULL);
	return *m_penv;
}

inline RETCODE CSTMT::Error(LPSTR   szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax,
							LPSWORD  pcbErrorMsg)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	return CODBC::Error(SQL_NULL_HENV, SQL_NULL_HDBC, m_hstmt,
						szSqlState, pfNativeError, szErrorMsg,
						cbErrorMsgMax, pcbErrorMsg);
}

inline RETCODE CSTMT::BindCol(UWORD icol,
							SWORD    fCType,
							PTR      rgbValue,
							SDWORD   cbValueMax,
							LPSDWORD pcbValue)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLBindCol(m_hstmt, icol, fCType, rgbValue, cbValueMax, pcbValue);
	CHECK_EX(SQLBINDCOL);
	return m_rc;
}

inline RETCODE CSTMT::Cancel()
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLCancel(m_hstmt);
	CHECK_EX(SQLCANCEL);
	return m_rc;
}

inline RETCODE CSTMT::ColAttributes(UWORD   icol,
									UWORD    fDescType,
									LPSDWORD pfDesc)
{
	return ColAttributes(icol, fDescType, NULL, 0, NULL, pfDesc);
}

inline RETCODE CSTMT::ColAttributes(UWORD   icol,
									UWORD    fDescType,
									PTR      rgbDesc,
									SWORD    cbDescMax,
									LPSWORD  pcbDesc)
{
	return ColAttributes(icol, fDescType, rgbDesc, cbDescMax, pcbDesc, NULL);
}

inline RETCODE CSTMT::ColAttributes(UWORD   icol,
									UWORD    fDescType,
									PTR      rgbDesc,
									SWORD    cbDescMax,
									LPSWORD  pcbDesc,
									LPSDWORD pfDesc)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLColAttributes(m_hstmt, icol, fDescType, rgbDesc,
								cbDescMax, pcbDesc, pfDesc);
	CHECK_EX(SQLCOLATTRIBUTES);
	return m_rc;
}

inline RETCODE CSTMT::ColumnPrivileges(LPCSTR szTableQualifier,
									LPCSTR  szTableOwner,
									LPCSTR  szTableName,
									LPCSTR  szColumnName)
{
	return ColumnPrivileges(szTableQualifier, SQL_NTS,
							szTableOwner, SQL_NTS,
							szTableName, SQL_NTS,
							szColumnName, SQL_NTS);
}

inline RETCODE CSTMT::ColumnPrivileges(LPCSTR  szTableQualifier,
										SWORD   cbTableQualifier,
										LPCSTR  szTableOwner,
										SWORD   cbTableOwner,
										LPCSTR  szTableName,
										SWORD   cbTableName,
										LPCSTR  szColumnName,
										SWORD   cbColumnName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLColumnPrivileges(m_hstmt,
						(UCHAR FAR *)szTableQualifier, cbTableQualifier,
						(UCHAR FAR *)szTableOwner, cbTableOwner,
						(UCHAR FAR *)szTableName, cbTableName,
						(UCHAR FAR *)szColumnName, cbColumnName);
	CHECK_EX(SQLCOLUMNPRIVILEGES);
	return m_rc;
}

inline RETCODE CSTMT::Columns(LPCSTR   szTableQualifier,
								LPCSTR  szTableOwner,
								LPCSTR  szTableName,
								LPCSTR  szColumnName)
{
	return Columns(szTableQualifier, SQL_NTS,
					szTableOwner, SQL_NTS,
					szTableName, SQL_NTS,
					szColumnName, SQL_NTS);
}

inline RETCODE CSTMT::Columns(LPCSTR   szTableQualifier,
								SWORD   cbTableQualifier,
								LPCSTR  szTableOwner,
								SWORD   cbTableOwner,
								LPCSTR  szTableName,
								SWORD   cbTableName,
								LPCSTR  szColumnName,
								SWORD   cbColumnName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLColumns(m_hstmt,
						(UCHAR FAR *)szTableQualifier, cbTableQualifier,
						(UCHAR FAR *)szTableOwner, cbTableOwner,
						(UCHAR FAR *)szTableName, cbTableName,
						(UCHAR FAR *)szColumnName, cbColumnName);
	CHECK_EX(SQLCOLUMNS);
	return m_rc;
}

inline RETCODE CSTMT::DescribeCol(UWORD     icol,
									LPSTR    szColName,
									SWORD    cbColNameMax,
									LPSWORD  pcbColName,
									LPSWORD  pfSqlType,
									LPUDWORD pcbColDef,
									LPSWORD  pibScale,
									LPSWORD  pfNullable)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLDescribeCol(m_hstmt, icol,
							(UCHAR FAR *)szColName, cbColNameMax,
							pcbColName, pfSqlType, pcbColDef,
							pibScale, pfNullable);
	CHECK_EX(SQLDESCRIBECOL);
	return m_rc;
}

inline RETCODE CSTMT::DescribeParam(UWORD   ipar,
									LPSWORD  pfSqlType,
									LPUDWORD pcbColDef,
									LPSWORD  pibScale,
									LPSWORD  pfNullable)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLDescribeParam(m_hstmt, ipar, pfSqlType, pcbColDef,
									pibScale, pfNullable);
	CHECK_EX(SQLDESCRIBEPARAM);
	return m_rc;
}

inline RETCODE CSTMT::ExecDirect(LPCSTR szSqlStr,
								SDWORD   cbSqlStr)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLExecDirect(m_hstmt, (UCHAR FAR *)szSqlStr, cbSqlStr);
	CHECK_EX(SQLEXECDIRECT);
	return m_rc;
}

inline RETCODE CSTMT::Execute()
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLExecute(m_hstmt);
	CHECK_EX(SQLEXECUTE);
	return m_rc;
}

inline RETCODE CSTMT::ExtendedFetch(UWORD  fFetchType,
									SDWORD   irow,
									LPUWORD  rgfRowStatus)
{
	return ExtendedFetch(fFetchType, irow, NULL, rgfRowStatus);
}

inline RETCODE CSTMT::ExtendedFetch(UWORD  fFetchType,
									SDWORD   irow,
									LPUDWORD pcrow,
									LPUWORD  rgfRowStatus)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLExtendedFetch(m_hstmt, fFetchType, irow, pcrow, rgfRowStatus);
	CHECK_EX(SQLEXTENDEDFETCH);
	return m_rc;
}

inline RETCODE CSTMT::Fetch()
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLFetch(m_hstmt);
	CHECK_EX(SQLFETCH);
	return m_rc;
}

inline RETCODE CSTMT::Fetch(UWORD  fFetchType,
							SDWORD   irow,
							LPUWORD  rgfRowStatus)
{
	return ExtendedFetch(fFetchType, irow, NULL, rgfRowStatus);
}

inline RETCODE CSTMT::Fetch(UWORD  fFetchType,
							SDWORD   irow,
							LPUDWORD pcrow,
							LPUWORD  rgfRowStatus)
{
	return ExtendedFetch(fFetchType, irow, pcrow, rgfRowStatus);
}

inline RETCODE CSTMT::ForeignKeys(LPCSTR szPkTableQualifier,
									LPCSTR szPkTableOwner,
									LPCSTR szPkTableName,
									LPCSTR szFkTableQualifier,
									LPCSTR szFkTableOwner,
									LPCSTR szFkTableName)
{
	return ForeignKeys(szPkTableQualifier, SQL_NTS,
					szPkTableOwner, SQL_NTS,
					szPkTableName, SQL_NTS,
					szFkTableQualifier, SQL_NTS,
					szFkTableOwner, SQL_NTS,
					szFkTableName, SQL_NTS);
						
}

inline RETCODE CSTMT::ForeignKeys(LPCSTR szPkTableQualifier,
									SWORD  cbPkTableQualifier,
									LPCSTR szPkTableOwner,
									SWORD  cbPkTableOwner,
									LPCSTR szPkTableName,
									SWORD  cbPkTableName,
									LPCSTR szFkTableQualifier,
									SWORD  cbFkTableQualifier,
									LPCSTR szFkTableOwner,
									SWORD  cbFkTableOwner,
									LPCSTR szFkTableName,
									SWORD  cbFkTableName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLForeignKeys(m_hstmt,
						(UCHAR FAR *)szPkTableQualifier, cbPkTableQualifier,
						(UCHAR FAR *)szPkTableOwner, cbPkTableOwner,
						(UCHAR FAR *)szPkTableName, cbPkTableName,
						(UCHAR FAR *)szFkTableQualifier, cbFkTableQualifier,
						(UCHAR FAR *)szFkTableOwner, cbFkTableOwner,
						(UCHAR FAR *)szFkTableName, cbFkTableName);
	CHECK_EX(SQLFOREIGNKEYS);
	return m_rc;
}

inline RETCODE CSTMT::Close()
{
	return FreeStmt(SQL_CLOSE);
}

inline RETCODE CSTMT::Unbind()
{
	return FreeStmt(SQL_UNBIND);
}

inline RETCODE CSTMT::ResetParams()
{
	return FreeStmt(SQL_RESET_PARAMS);
}

inline RETCODE CSTMT::FreeStmt(UWORD fOption)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	ASSERT(fOption != SQL_DROP);
	m_rc = ::SQLFreeStmt(m_hstmt, fOption);
	CHECK_EX(SQLFREESTMT);
	return m_rc;
}

inline RETCODE CSTMT::GetCursorName(LPSTR szCursor,
									SWORD   cbCursorMax,
									LPSWORD pcbCursor)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLGetCursorName(m_hstmt, (UCHAR FAR *)szCursor,
								cbCursorMax, pcbCursor);
	CHECK_EX(SQLGETCURSORNAME);
	return m_rc;
}

inline RETCODE CSTMT::GetData(UWORD    icol,
								SWORD    fCType,
								PTR      rgbValue,
								SDWORD   cbValueMax,
								LPSDWORD pcbValue)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLGetData(m_hstmt, icol, fCType, rgbValue, cbValueMax, pcbValue);
	CHECK_EX(SQLGETDATA);
	return m_rc;
}

inline RETCODE CSTMT::GetStmtOption(UWORD fOption, PTR pvParam)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLGetStmtOption(m_hstmt, fOption, pvParam);
	CHECK_EX(SQLGETSTMTOPTION);
	return m_rc;
}

inline RETCODE CSTMT::GetTypeInfo(SWORD fSqlType)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLGetTypeInfo(m_hstmt, fSqlType);
	CHECK_EX(SQLGETINFO);
	return m_rc;
}

inline RETCODE CSTMT::MoreResults()
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLMoreResults(m_hstmt);
	CHECK_EX(SQLMORERESULTS);
	return m_rc;
}

inline RETCODE CSTMT::NumParams(LPSWORD pcpar)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLNumParams(m_hstmt, pcpar);
	CHECK_EX(SQLNUMPARAMS);
	return m_rc;
}

inline RETCODE CSTMT::NumResultCols(LPSWORD pccol)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLNumResultCols(m_hstmt, pccol);
	CHECK_EX(SQLNUMRESULTCOLS);
	return m_rc;
}

inline RETCODE CSTMT::ParamData(LPPTR prgbValue)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLParamData(m_hstmt, prgbValue);
	CHECK_EX(SQLPARAMDATA);
	return m_rc;
}

inline RETCODE CSTMT::ParamOptions(UDWORD crow, LPUDWORD pirow)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLParamOptions(m_hstmt, crow, pirow);
	CHECK_EX(SQLPARAMOPTIONS);
	return m_rc;
}

inline RETCODE CSTMT::Prepare(LPCSTR szSqlStr,
								SDWORD cbSqlStr)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLPrepare(m_hstmt, (UCHAR FAR *)szSqlStr, cbSqlStr);
	CHECK_EX(SQLPREPARE);
	return m_rc;
}

inline RETCODE CSTMT::PrimaryKeys(LPCSTR szTableQualifier,
									LPCSTR szTableOwner,
									LPCSTR szTableName)
{
	return PrimaryKeys(szTableQualifier, SQL_NTS,
						szTableOwner, SQL_NTS,
						szTableName, SQL_NTS);
}

inline RETCODE CSTMT::PrimaryKeys(LPCSTR szTableQualifier,
									SWORD  cbTableQualifier,
									LPCSTR szTableOwner,
									SWORD  cbTableOwner,
									LPCSTR szTableName,
									SWORD  cbTableName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLPrimaryKeys(m_hstmt,
							(UCHAR FAR *)szTableQualifier, cbTableQualifier,
							(UCHAR FAR *)szTableOwner, cbTableOwner,
							(UCHAR FAR *)szTableName, cbTableName);
	CHECK_EX(SQLPRIMARYKEYS);
	return m_rc;
}

inline RETCODE CSTMT::ProcedureColumns(LPCSTR szProcQualifier,
										LPCSTR  szProcOwner,
										LPCSTR  szProcName,
										LPCSTR  szColumnName)
{
	return ProcedureColumns(szProcQualifier, szProcOwner,
							szProcName, szColumnName);
}

inline RETCODE CSTMT::ProcedureColumns(LPCSTR szProcQualifier,
										SWORD   cbProcQualifier,
										LPCSTR  szProcOwner,
										SWORD   cbProcOwner,
										LPCSTR  szProcName,
										SWORD   cbProcName,
										LPCSTR  szColumnName,
										SWORD   cbColumnName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLProcedureColumns(m_hstmt,
							(UCHAR FAR *)szProcQualifier, cbProcQualifier,
							(UCHAR FAR *)szProcOwner, cbProcOwner,
							(UCHAR FAR *)szProcName, cbProcName,
							(UCHAR FAR *)szColumnName, cbColumnName);
	CHECK_EX(SQLPROCEDURECOLUMNS);
	return m_rc;
}

inline RETCODE CSTMT::Procedures(LPCSTR szProcQualifier,
								LPCSTR    szProcOwner,
								LPCSTR    szProcName)
{
	return Procedures(szProcQualifier, SQL_NTS,
					szProcOwner, SQL_NTS,
					szProcName, SQL_NTS);
}

inline RETCODE CSTMT::Procedures(LPCSTR szProcQualifier,
								SWORD     cbProcQualifier,
								LPCSTR    szProcOwner,
								SWORD     cbProcOwner,
								LPCSTR    szProcName,
								SWORD     cbProcName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLProcedures(m_hstmt,
						(UCHAR FAR *)szProcQualifier, cbProcQualifier,
						(UCHAR FAR *)szProcOwner, cbProcOwner,
						(UCHAR FAR *)szProcName, cbProcName);
	CHECK_EX(SQLPROCEDURES);
	return m_rc;
}

inline RETCODE CSTMT::PutData(PTR rgbValue, SDWORD cbValue)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLPutData(m_hstmt, rgbValue, cbValue);
	CHECK_EX(SQLPUTDATA);
	return m_rc;
}

inline RETCODE CSTMT::RowCount(LPSDWORD pcrow)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLRowCount(m_hstmt, pcrow);
	CHECK_EX(SQLROWCOUNT);
	return m_rc;
}

inline RETCODE CSTMT::SetCursorName(LPCSTR szCursor,
									SWORD    cbCursor)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSetCursorName(m_hstmt, (UCHAR FAR *)szCursor, cbCursor);
	CHECK_EX(SQLSETCURSORNAME);
	return m_rc;
}

inline RETCODE CSTMT::SetParam(UWORD   ipar,
								SWORD    fCType,
								SWORD    fSqlType,
								UDWORD   cbColDef,
								SWORD    ibScale,
								PTR      rgbValue,
								LPSDWORD pcbValue)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSetParam(m_hstmt, ipar, fCType, fSqlType,
						cbColDef, ibScale, rgbValue, pcbValue);
	CHECK_EX(SQLSETPARAM);
	return m_rc;
}

inline RETCODE CSTMT::SetPos(UWORD irow,
								BOOL fRefresh,
								BOOL fLock)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSetPos(m_hstmt, irow, fRefresh, fLock);
	CHECK_EX(SQLSETPOS);
	return m_rc;
}

inline RETCODE CSTMT::SetScrollOptions(UWORD fConcurrency,
										SDWORD crowKeyset,
										UWORD  crowRowset)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSetScrollOptions(m_hstmt, fConcurrency,
								crowKeyset, crowRowset);
	CHECK_EX(SQLSETSCROLLOPTIONS);
	return m_rc;
}


inline RETCODE CSTMT::SetStmtOption(UWORD fOption, UDWORD vParam)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSetStmtOption(m_hstmt, fOption, vParam);
	CHECK_EX(SQLSETSTMTOPTION);
	return m_rc;
}

inline RETCODE CSTMT::SpecialColumns(UWORD fColType,
									LPCSTR   szTableQualifier,
									LPCSTR   szTableOwner,
									LPCSTR   szTableName,
									UWORD    fScope,
									UWORD    fNullable)
{
	return SpecialColumns(fColType, szTableQualifier, SQL_NTS,
						szTableOwner, SQL_NTS,
						szTableName, SQL_NTS,
						fScope, fNullable);
}

inline RETCODE CSTMT::SpecialColumns(UWORD fColType,
									LPCSTR   szTableQualifier,
									SWORD    cbTableQualifier,
									LPCSTR   szTableOwner,
									SWORD    cbTableOwner,
									LPCSTR   szTableName,
									SWORD    cbTableName,
									UWORD    fScope,
									UWORD    fNullable)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLSpecialColumns(m_hstmt, fColType,
							(UCHAR FAR *)szTableQualifier, cbTableQualifier,
							(UCHAR FAR *)szTableOwner, cbTableOwner,
							(UCHAR FAR *)szTableName, cbTableName,
							fScope, fNullable);
	CHECK_EX(SQLSPECIALCOLUMNS);
	return m_rc;
}

inline RETCODE CSTMT::Statistics(LPCSTR  szTableQualifier,
									LPCSTR szTableOwner,
									LPCSTR szTableName,
									UWORD  fUnique,
									UWORD  fAccuracy)
{
	return Statistics(szTableQualifier, SQL_NTS,
					szTableOwner, SQL_NTS,
					szTableName, SQL_NTS,
					fUnique, fAccuracy);
}

inline RETCODE CSTMT::Statistics(LPCSTR  szTableQualifier,
									SWORD  cbTableQualifier,
									LPCSTR szTableOwner,
									SWORD  cbTableOwner,
									LPCSTR szTableName,
									SWORD  cbTableName,
									UWORD  fUnique,
									UWORD  fAccuracy)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLStatistics(m_hstmt,
						(UCHAR FAR *)szTableQualifier, cbTableQualifier,
						(UCHAR FAR *)szTableOwner, cbTableOwner,
						(UCHAR FAR *)szTableName, cbTableName,
						fUnique, fAccuracy);
	CHECK_EX(SQLSTATISTICS);
	return m_rc;
}

inline RETCODE CSTMT::TablePrivileges(LPCSTR szTableQualifier,
										LPCSTR szTableOwner,
										LPCSTR szTableName)
{
	return TablePrivileges(szTableQualifier, SQL_NTS,
							szTableOwner, SQL_NTS,
							szTableName, SQL_NTS);
}

inline RETCODE CSTMT::TablePrivileges(LPCSTR szTableQualifier,
										SWORD  cbTableQualifier,
										LPCSTR szTableOwner,
										SWORD  cbTableOwner,
										LPCSTR szTableName,
										SWORD  cbTableName)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLTablePrivileges(m_hstmt,
						(UCHAR FAR *)szTableQualifier, cbTableQualifier,
						(UCHAR FAR *)szTableOwner, cbTableOwner,
						(UCHAR FAR *)szTableName, cbTableName);
	CHECK_EX(SQLTABLEPRIVILEGES);
	return m_rc;
}

inline RETCODE CSTMT::Tables(LPCSTR  szTableQualifier,
								LPCSTR szTableOwner,
								LPCSTR szTableName,
								LPCSTR szTableType)
{
	return Tables(szTableQualifier, SQL_NTS,
				szTableOwner, SQL_NTS,
				szTableName, SQL_NTS,
				szTableType, SQL_NTS);
}

inline RETCODE CSTMT::Tables(LPCSTR  szTableQualifier,
								SWORD  cbTableQualifier,
								LPCSTR szTableOwner,
								SWORD  cbTableOwner,
								LPCSTR szTableName,
								SWORD  cbTableName,
								LPCSTR szTableType,
								SWORD  cbTableType)
{
	ASSERT(m_hstmt != SQL_NULL_HSTMT);
	m_rc = ::SQLTables(m_hstmt,
						(UCHAR FAR *)szTableQualifier, cbTableQualifier,
						(UCHAR FAR *)szTableOwner, cbTableOwner,
						(UCHAR FAR *)szTableName, cbTableName,
						(UCHAR FAR *)szTableType, cbTableType);
	CHECK_EX(SQLTABLES);
	return m_rc;
}


#endif // __CODBC_INL
