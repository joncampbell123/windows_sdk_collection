/*--------------------------------------------------------------------------
  CODBC.H - Low-level ODBC object classes

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/

#ifndef __CODBC_H
#define __CODBC_H

#include    <sql.h>
#include    <sqlext.h>

#if defined(_AFX) && !defined(NOODBCEX)
#define CRIT_CHECK_EX(f)    { if( !Success(m_rc)) \
								ThrowODBCException(this, SQL_API_##f, m_rc); }
#define CHECK_EX(f)         { if (!Success(m_rc)) \
								ThrowODBCException(this, SQL_API_##f, m_rc); }
#elif !defined(_AFX) && !defined(NOODBCEX)
	// Without the MFC exception handling, we'll display a (very) simple
	// message box that at least displays the error code if the action was
	// not successful.  In the absence of the MFC exception handler, we'll
	// only check for exceptions in constructors and destructors, where it
	// is not possible for the executing program to check m_rc or Success if
	// the construction or destruction fails.
	//
	// This simple message box could be replaced by more elaborate application
	// specific error handling; it is only done this way as an example.
#define CRIT_CHECK_EX(f)    if( !Success(m_rc) ) \
							{   char szMsg[15]; \
								wsprintf(szMsg, "RETCODE %d", m_rc); \
								MessageBox(GetActiveWindow(), szMsg, \
									"Error:", MB_ICONINFORMATION | MB_OK); }
#define CHECK_EX(f)
#else
#define CRIT_CHECK_EX(f)
#define CHECK_EX(f)
#endif

#ifdef  _AFX
#define ODBC_OBJECT         : public CObject
#else
#define ODBC_OBJECT
#endif


typedef PTR FAR *LPPTR;
typedef SWORD FAR *LPSWORD;
typedef UWORD FAR *LPUWORD;
typedef SDWORD FAR *LPSDWORD;
typedef UDWORD FAR *LPUDWORD;


////////////////////////////////////////////////////////////////////////////
// Classes declared
//
class CODBCException;                     // ODBC exception object

class CODBC;                              // ODBC base object
	class CENV;                            // ODBC environment object
	class CDBC;                            // ODBC connection object
	class CSTMT;                           // ODBC statement object


////////////////////////////////////////////////////////////////////////////
// CODBC
class CODBC ODBC_OBJECT {

#ifdef _AFX
	DECLARE_DYNAMIC(CODBC);                 // Required by MFC
#endif

	// Public members
	public:

		RETCODE m_rc;

		virtual RETCODE Error(LPSTR  szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax,
							LPSWORD  pcbErrorMsg) = 0;

		BOOL Success(RETCODE rc);

	// Protected members
	protected:

		CODBC();
		virtual ~CODBC();

		RETCODE Error(HENV   henv,
					HDBC     hdbc,
					HSTMT    hstmt,
					LPSTR    szSqlState,
					LPSDWORD pfNativeError,
					LPSTR    szErrorMsg,
					SWORD    cbErrorMsgMax,
					LPSWORD  pcbErrorMsg);

	// Private members
	private:

		CODBC(const CODBC& odbcSrc);          // No implementation
		void operator=(const CODBC& odbcSrc); // No implementation

};


#ifdef _AFX
////////////////////////////////////////////////////////////////////////////
// CODBCException
class CODBCException : public CException {

	DECLARE_DYNAMIC(CODBCException);       // Required by MFC

	// Public members
	public:

		CODBCException(CODBC* podbc, UWORD fFunction, RETCODE rc);

		CODBC*  m_podbc;                   // ODBC object
		UWORD   m_fFunction;               // ODBC function code
		RETCODE m_rc;                      // ODBC return code
};

void ThrowODBCException(CODBC* podbc, UWORD fFunction, RETCODE rc=SQL_ERROR);
#endif


////////////////////////////////////////////////////////////////////////////
// CENV
class CENV : public CODBC {

#ifdef _AFX
	DECLARE_DYNAMIC(CENV);                  // Required by MFC
#endif

	// Public members:
	public:

		CENV();
		~CENV();

		HENV    m_henv;                    // ODBC environment handle

		operator HENV() const;       // CENV to HENV conversion

		virtual RETCODE Error(LPSTR  szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH-1,
							LPSWORD  pcbErrorMsg = NULL);

		// ODBC functions
		CDBC*   AllocConnect();

		RETCODE DataSources(UWORD fDirection,
							LPSTR szDSN,
							LPSTR szDescription,
							SWORD cbDescription=255);
	   RETCODE DataSources(UWORD   fDirection,
							LPSTR   szDSN,
							SWORD   cbDSNMax,
							LPSWORD pcbDSN,
							LPSTR   szDescription,
							SWORD   cbDescriptionMax,
							LPSWORD pcbDescription);

		RETCODE Transact(UWORD fType);

	// Protected members:
	protected:

};


////////////////////////////////////////////////////////////////////////////
// CDBC
class CDBC : public CODBC {

#ifdef _AFX
	DECLARE_DYNAMIC(CDBC);                  // Required by MFC
#endif

	// Public members
	public:

		CDBC(CENV& env);
		CDBC(CENV* penv);
		~CDBC();

		HDBC    m_hdbc;                    // ODBC connection handle
		CENV*   m_penv;                    // Associated environment object

		operator HDBC() const;        // CDBC to HDBC  conversion
		operator CENV*() const;       // CDBC to CENV* conversion
		operator CENV&() const;       // CDBC to CENV& conversion

		virtual RETCODE Error(LPSTR  szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH-1,
							LPSWORD  pcbErrorMsg = NULL);

		// ODBC functions
		CSTMT*  AllocStmt();

		RETCODE BrowseConnect(LPCSTR szConnStrIn,
							LPSTR    szConnStrOut,
							SWORD    cbConnStrOutMax);
		RETCODE BrowseConnect(LPCSTR szConnStrIn,
							SWORD    cbConnStrIn,
							LPSTR    szConnStrOut,
							SWORD    cbConnStrOutMax,
							LPSWORD  pcbConnStrOut = NULL);

		RETCODE Connect(LPCSTR szDSN,
						LPCSTR szUID,
						LPCSTR szAuthStr = NULL);
		RETCODE Connect(LPCSTR szDSN,
						SWORD  cbDSN,
						LPCSTR szUID,
						SWORD  cbUID,
						LPCSTR szAuthStr,
						SWORD  cbAuthStr);

		RETCODE Disconnect();

		RETCODE DriverConnect(
					HWND   hwnd,
					LPCSTR szConnStrIn,
					UWORD  fDriverCompletion = SQL_DRIVER_COMPLETE_REQUIRED);
		RETCODE DriverConnect(
					HWND   hwnd,
					LPCSTR szConnStrIn,
					LPSTR  szConnStrOut,
					SWORD  cbConnStrOutMax,
					UWORD  fDriverCompletion = SQL_DRIVER_COMPLETE_REQUIRED);
		RETCODE DriverConnect(
					HWND    hwnd,
					LPCSTR  szConnStrIn,
					SWORD   cbConnStrIn,
					LPSTR   szConnStrOut,
					SWORD   cbConnStrOutMax,
					LPSWORD pcbConnStrOut,
					UWORD   fDriverCompletion = SQL_DRIVER_COMPLETE_REQUIRED);

		RETCODE GetConnectOption(UWORD fOption, PTR pvParam);

		RETCODE GetFunctions(UWORD fFunction, LPUWORD pfExists);

		RETCODE GetInfo(UWORD   fInfoType,
						PTR     rgbInfoValue,
						SWORD   cbInfoValueMax = 4,
						LPSWORD pcbInfoValue = NULL);

		RETCODE NativeSql(LPCSTR szSqlStrIn,
						LPSTR    szSqlStr,
						SDWORD   cbSqlStrMax,
						LPSDWORD pcbSqlStr = NULL);
		RETCODE NativeSql(LPCSTR szSqlStrIn,
						SDWORD   cbSqlStrIn,
						LPSTR    szSqlStr,
						SDWORD   cbSqlStrMax,
						LPSDWORD pcbSqlStr = NULL);

		RETCODE SetConnectOption(UWORD fOption, UDWORD vParam);

		RETCODE Transact(UWORD fType);

	// Protected members:
	protected:

		void Initialize(CENV* penv);
};


////////////////////////////////////////////////////////////////////////////
// CSTMT
class CSTMT : public CODBC {

#ifdef _AFX
	DECLARE_DYNAMIC(CSTMT);                 // Required by MFC
#endif

	// Public members
	public:

		CSTMT(CDBC& dbc);
		CSTMT(CDBC* pdbc);
		~CSTMT();

		HSTMT   m_hstmt;                   // ODBC statement handle
		CDBC*   m_pdbc;                    // Associated connection  object
		CENV*   m_penv;                    // Associated environment object

		operator HSTMT() const;      // CSTMT to HSTMT conversion
		operator CDBC*() const;      // CSTMT to CDBC* conversion
		operator CDBC&() const;      // CSTMT to CDBC& conversion
		operator CENV*() const;      // CSTMT to CENV* conversion
		operator CENV&() const;      // CSTMT to CENV& conversion

		virtual RETCODE Error(LPSTR  szSqlState,
							LPSDWORD pfNativeError,
							LPSTR    szErrorMsg,
							SWORD    cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH-1,
							LPSWORD  pcbErrorMsg = NULL);

		// ODBC functions
		RETCODE BindCol(UWORD    icol,
						SWORD    fCType,
						PTR      rgbValue,
						SDWORD   cbValueMax,
						LPSDWORD pcbValue);

		RETCODE Cancel();

		RETCODE ColAttributes(UWORD  icol,
							UWORD    fDescType,
							LPSDWORD pfDesc);
		RETCODE ColAttributes(UWORD  icol,
							UWORD    fDescType,
							PTR      rgbDesc,
							SWORD    cbDescMax,
							LPSWORD  pcbDesc = NULL);
		RETCODE ColAttributes(UWORD  icol,
							UWORD    fDescType,
							PTR      rgbDesc,
							SWORD    cbDescMax,
							LPSWORD  pcbDesc,
							LPSDWORD pfDesc);

		RETCODE ColumnPrivileges(LPCSTR szTableQualifier,
								LPCSTR  szTableOwner,
								LPCSTR  szTableName,
								LPCSTR  szColumnName);
		RETCODE ColumnPrivileges(LPCSTR szTableQualifier,
								SWORD   cbTableQualifier,
								LPCSTR  szTableOwner,
								SWORD   cbTableOwner,
								LPCSTR  szTableName,
								SWORD   cbTableName,
								LPCSTR  szColumnName,
								SWORD   cbColumnName);

		RETCODE Columns(LPCSTR  szTableQualifier,
						LPCSTR  szTableOwner,
						LPCSTR  szTableName,
						LPCSTR  szColumnName);
		RETCODE Columns(LPCSTR  szTableQualifier,
						SWORD   cbTableQualifier,
						LPCSTR  szTableOwner,
						SWORD   cbTableOwner,
						LPCSTR  szTableName,
						SWORD   cbTableName,
						LPCSTR  szColumnName,
						SWORD   cbColumnName);

		RETCODE DescribeCol(UWORD    icol,
							LPSTR    szColName,
							SWORD    cbColNameMax,
							LPSWORD  pcbColName,
							LPSWORD  pfSqlType,
							LPUDWORD pcbColDef,
							LPSWORD  pibScale,
							LPSWORD  pfNullable);

		RETCODE DescribeParam(UWORD  ipar,
							LPSWORD  pfSqlType,
							LPUDWORD pcbColDef,
							LPSWORD  pibScale,
							LPSWORD  pfNullable);

		RETCODE ExecDirect(LPCSTR szSqlStr,
						SDWORD    cbSqlStr = SQL_NTS);

		RETCODE Execute();

		RETCODE ExtendedFetch(UWORD  fFetchType,
							SDWORD   irow,
							LPUWORD  rgfRowStatus);
		RETCODE ExtendedFetch(UWORD  fFetchType,
							SDWORD   irow,
							LPUDWORD pcrow,
							LPUWORD  rgfRowStatus);

		RETCODE Fetch();
		RETCODE Fetch(UWORD  fFetchType,
					SDWORD   irow,
					LPUWORD  rgfRowStatus);
		RETCODE Fetch(UWORD  fFetchType,
					SDWORD   irow,
					LPUDWORD pcrow,
					LPUWORD  rgfRowStatus);

		RETCODE ForeignKeys(LPCSTR szPkTableQualifier,
							LPCSTR szPkTableOwner,
							LPCSTR szPkTableName,
							LPCSTR szFkTableQualifier,
							LPCSTR szFkTableOwner,
							LPCSTR szFkTableName);
		RETCODE ForeignKeys(LPCSTR szPkTableQualifier,
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
							SWORD  cbFkTableName);

		RETCODE Close();
		RETCODE Unbind();
		RETCODE ResetParams();
		RETCODE FreeStmt(UWORD fOption);

		RETCODE GetCursorName(LPSTR szCursor,
							SWORD   cbCursorMax,
							LPSWORD pcbCursor = NULL);

		RETCODE GetData(UWORD    icol,
						SWORD    fCType,
						PTR      rgbValue,
						SDWORD   cbValueMax,
						LPSDWORD pcbValue);

		RETCODE GetStmtOption(UWORD fOption, PTR pvParam);

		RETCODE GetTypeInfo(SWORD fSqlType);

		RETCODE MoreResults();

		RETCODE NumParams(LPSWORD pcpar);

		RETCODE NumResultCols(LPSWORD pccol);

		RETCODE ParamData(LPPTR prgbValue);

		RETCODE ParamOptions(UDWORD crow, LPUDWORD pirow);

		RETCODE Prepare(LPCSTR szSqlStr,
						SDWORD cbSqlStr = SQL_NTS);

		RETCODE PrimaryKeys(LPCSTR szTableQualifier,
							LPCSTR szTableOwner,
							LPCSTR szTableName);
		RETCODE PrimaryKeys(LPCSTR szTableQualifier,
							SWORD  cbTableQualifier,
							LPCSTR szTableOwner,
							SWORD  cbTableOwner,
							LPCSTR szTableName,
							SWORD  cbTableName);

		RETCODE ProcedureColumns(LPCSTR szProcQualifier,
								LPCSTR  szProcOwner,
								LPCSTR  szProcName,
								LPCSTR  szColumnName);
		RETCODE ProcedureColumns(LPCSTR szProcQualifier,
								SWORD   cbProcQualifier,
								LPCSTR  szProcOwner,
								SWORD   cbProcOwner,
								LPCSTR  szProcName,
								SWORD   cbProcName,
								LPCSTR  szColumnName,
								SWORD   cbColumnName);

		RETCODE Procedures(LPCSTR szProcQualifier,
						LPCSTR    szProcOwner,
						LPCSTR    szProcName);
		RETCODE Procedures(LPCSTR szProcQualifier,
						SWORD     cbProcQualifier,
						LPCSTR    szProcOwner,
						SWORD     cbProcOwner,
						LPCSTR    szProcName,
						SWORD     cbProcName);

		RETCODE PutData(PTR rgbValue, SDWORD cbValue);

		RETCODE RowCount(LPSDWORD pcrow);

		RETCODE SetCursorName(LPCSTR szCursor,
							SWORD    cbCursor = SQL_NTS);

		RETCODE SetParam(UWORD   ipar,
						SWORD    fCType,
						SWORD    fSqlType,
						UDWORD   cbColDef,
						SWORD    ibScale,
						PTR      rgbValue,
						LPSDWORD pcbValue = NULL);

		RETCODE SetPos(UWORD irow,
						BOOL fRefresh = TRUE,
						BOOL fLock = FALSE);

		RETCODE SetScrollOptions(UWORD fConcurrency = SQL_CONCUR_READ_ONLY,
								SDWORD crowKeyset = SQL_SCROLL_FORWARD_ONLY,
								UWORD  crowRowset = 1);

		RETCODE SetStmtOption(UWORD fOption, UDWORD vParam);

		RETCODE SpecialColumns(UWORD fColType,
							LPCSTR   szTableQualifier,
							LPCSTR   szTableOwner,
							LPCSTR   szTableName,
							UWORD    fScope = SQL_SCOPE_TRANSACTION,
							UWORD    fNullable = SQL_NULLABLE);
		RETCODE SpecialColumns(UWORD fColType,
							LPCSTR   szTableQualifier,
							SWORD    cbTableQualifier,
							LPCSTR   szTableOwner,
							SWORD    cbTableOwner,
							LPCSTR   szTableName,
							SWORD    cbTableName,
							UWORD    fScope = SQL_SCOPE_TRANSACTION,
							UWORD    fNullable = SQL_NULLABLE);

		RETCODE Statistics(LPCSTR  szTableQualifier,
							LPCSTR szTableOwner,
							LPCSTR szTableName,
							UWORD  fUnique = SQL_INDEX_UNIQUE,
							UWORD  fAccuracy = SQL_QUICK);
		RETCODE Statistics(LPCSTR  szTableQualifier,
							SWORD  cbTableQualifier,
							LPCSTR szTableOwner,
							SWORD  cbTableOwner,
							LPCSTR szTableName,
							SWORD  cbTableName,
							UWORD  fUnique = SQL_INDEX_UNIQUE,
							UWORD  fAccuracy = SQL_QUICK);

		RETCODE TablePrivileges(LPCSTR szTableQualifier,
								LPCSTR szTableOwner,
								LPCSTR szTableName);
		RETCODE TablePrivileges(LPCSTR szTableQualifier,
								SWORD  cbTableQualifier,
								LPCSTR szTableOwner,
								SWORD  cbTableOwner,
								LPCSTR szTableName,
								SWORD  cbTableName);

		RETCODE Tables(LPCSTR  szTableQualifier,
						LPCSTR szTableOwner,
						LPCSTR szTableName,
						LPCSTR szTableType);
		RETCODE Tables(LPCSTR  szTableQualifier,
						SWORD  cbTableQualifier,
						LPCSTR szTableOwner,
						SWORD  cbTableOwner,
						LPCSTR szTableName,
						SWORD  cbTableName,
						LPCSTR szTableType,
						SWORD  cbTableType);

	// Protected members
	protected:
	
		void Initialize(CDBC* pdbc);
};


// Inline functions --------------------------------------------------------
#include    "codbc.inl"

#endif // __CODBC_H
