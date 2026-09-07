//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//|	Title:	INFO.C
//|		This module contains the functions which handle the Info menu items.
//|			This module relies on RESULTS and EXECUTE to 
//|		This file contains the actual code to execute SQL Statements and
//|			display them.  This file is dependent on the SA Tool data structures
//|			and the independent module RESULTS.
//|
//|		NOTE:  Due to the timing of this sample, only the 1.0 GetInfo constants
//|			are shown.  To see all GetInfo constants for a 2.0 driver, use the
//|			ODBC Test Tool which comes with this SDK.
//*---------------------------------------------------------------------------------
#include "headers.h"

VSZFile;

//*---------------------------------------------------------------------------------
//|	Defines and macros
//*---------------------------------------------------------------------------------
#define MAXNAME			35
#define MAXPARMS		18
#define MAXSQL			300

#define szCOMMA			","
#define szBLANK			" "

typedef struct tagNEWPIPE {
	HWND			hwnd;
	HINSTANCE	hInst;
	char			szName[MAXNAME];
	BOOL			fSuccess;
	} NEWPIPE;

//*---------------------------------------------------------------------------------
//|	Local Function Prototypes
//*---------------------------------------------------------------------------------
BOOL CALLBACK EditPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void NewPipe(NEWPIPE FAR * np);
BOOL CALLBACK NewPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DoPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void DoPipe(RESULTSSET FAR * rs, CHILDINFO FAR * ci, LPSTR szPipeName,
			int FAR xPipes[], int cbCnt);
void DoPipeByValue(RESULTSSET FAR * rs, CHILDINFO FAR * ci, LPSTR sqlpsql,
		LPSTR szparms, int FAR xPipes[], int cbCnt, LPSTR szPipeName);
void PrepareParmList(LPSTR str);
SDWORD RefreshList(HWND hDlg, RESULTSSET FAR * rs, CHILDINFO FAR * ci, int state, 
			LPSTR szQual, LPSTR szOwner, LPSTR szName, int cbCols);
SDWORD RefreshPipeList(HWND hDlg);


//*---------------------------------------------------------------------------------
//|	Global variables
//*---------------------------------------------------------------------------------
extern dCSEG(char) szMSSansSerif[];
extern dCSEG(char) szPIPES[];
extern dCSEG(char) szFONT[];
extern char			 OutStr[MAXBUFF];
extern dCSEG(char) szVALUE[];
dCSEG(char) szCore[]					=	"Core";
dCSEG(char) szLvl1[]					=	"Level 1";
dCSEG(char) szLvl2[]					=	"Level 2";
dCSEG(char) szYes[]					=	"Yes";
dCSEG(char) szNo[]					=	"No";
dCSEG(char) szODBCFunctions[]		=	"Functions";
dCSEG(char) szODBCDataSources[]	=	"Data Sources";
dCSEG(char) szODBCDataTypes[]		=	"Data Types";
dCSEG(char)	szGetInfoTitle[]		=	"Get Info";
dCSEG(char) szQualifier[]			=	"Qualifier";
dCSEG(char) szOwner[]				=	"Owner";
dCSEG(char) szName[]					=	"Name";
dCSEG(char) szType[]					=	"Type";
dCSEG(char) szSQL[]					=	"Sql";
dCSEG(char) szPARMS[]				=	"Parms";
dCSEG(char) szPARMOPT[]				=	"ParmOpt";
dCSEG(char) szDELETEOPT[]			=	"Delete";
dCSEG(char) szBothTypes[]			=	"'%s','%s'";
dCSEG(char) szOneType[]				=  "'%s'";
dCSEG(char) szBlank[]				=	" ";
dCSEG(char) szTABLETYPE[]			= 	"TABLE";
dCSEG(char) szVIEWTYPE[]			= 	"VIEW";
dCSEG(char)	szVALUE[]				=	"value";
dCSEG(char)	szADDRESS[]				=	"address";
dCSEG(char) szDeletePipe[]			=	"Delete pipe %s?";
dCSEG(char)	szEditPipe[]			=	"Edit Pipe";
dCSEG(char) szDuplicatePipe[]		=	"Pipe already exists";
dCSEG(char) szInstalled[]			=	"Installed";
dCSEG(char) szDROPPROCSEMI[]		=  "Drop Procedure (with semi-colon)";

static char szErrorMsgTitle[]		=	"Error";



struct {
	UWORD				fFunction;						// Identifier for SQLGetFunctions
	LPSTR				szLevel;							// Conformance Level
	int				idFunction;						// String table identifier for function name
	} ODBCFunctions[] = {
// fFunction								szLevel 				idFunction
//	-------------------------------	---------------	--------------------------------
	SQL_API_SQLALLOCCONNECT,			(LPSTR)szCore,		idsSQLAllocConnect,
	SQL_API_SQLALLOCENV,					(LPSTR)szCore,		idsSQLAllocEnv,
	SQL_API_SQLALLOCSTMT,				(LPSTR)szCore,		idsSQLAllocStmt,
	SQL_API_SQLBINDCOL,					(LPSTR)szCore,		idsSQLBindCol,
	SQL_API_SQLCANCEL,					(LPSTR)szCore,		idsSQLCancel,
	SQL_API_SQLCOLATTRIBUTES,			(LPSTR)szCore,		idsSQLColAttributes,
	SQL_API_SQLCONNECT,					(LPSTR)szCore,		idsSQLConnect,
	SQL_API_SQLDESCRIBECOL,				(LPSTR)szCore,		idsSQLDescribeCol,
	SQL_API_SQLDISCONNECT,				(LPSTR)szCore,		idsSQLDisconnect,
	SQL_API_SQLERROR,						(LPSTR)szCore,		idsSQLError,
	SQL_API_SQLEXECDIRECT,				(LPSTR)szCore,		idsSQLExecDirect,
	SQL_API_SQLEXECUTE,					(LPSTR)szCore,		idsSQLExecute,
	SQL_API_SQLFETCH,						(LPSTR)szCore,		idsSQLFetch,
	SQL_API_SQLFREECONNECT,				(LPSTR)szCore,		idsSQLFreeConnect,
	SQL_API_SQLFREEENV,					(LPSTR)szCore,		idsSQLFreeEnv,
	SQL_API_SQLFREESTMT,					(LPSTR)szCore,		idsSQLFreeStmt,
	SQL_API_SQLGETCURSORNAME,			(LPSTR)szCore,		idsSQLGetCursorName,
	SQL_API_SQLNUMRESULTCOLS,			(LPSTR)szCore,		idsSQLNumResultCols,
	SQL_API_SQLPREPARE,					(LPSTR)szCore,		idsSQLPrepare,
	SQL_API_SQLROWCOUNT,					(LPSTR)szCore,		idsSQLRowCount,
	SQL_API_SQLSETCURSORNAME,			(LPSTR)szCore,		idsSQLSetCursorName,
	SQL_API_SQLSETPARAM,					(LPSTR)szCore,		idsSQLSetParam,
	SQL_API_SQLTRANSACT,					(LPSTR)szCore,		idsSQLTransact,
//---- Level 1 Conformance ----------------------------------	
	SQL_API_SQLCOLUMNS,					(LPSTR)szLvl1,		idsSQLColumns,
	SQL_API_SQLDRIVERCONNECT,			(LPSTR)szLvl1,		idsSQLDriverConnect,
	SQL_API_SQLGETCONNECTOPTION,		(LPSTR)szLvl1,		idsSQLGetConnectOption,
	SQL_API_SQLGETDATA,					(LPSTR)szLvl1,		idsSQLGetData,
	SQL_API_SQLGETFUNCTIONS,			(LPSTR)szLvl1,		idsSQLGetFunctions,
	SQL_API_SQLGETSTMTOPTION,			(LPSTR)szLvl1,		idsSQLGetStmtOption,
	SQL_API_SQLGETTYPEINFO,				(LPSTR)szLvl1,		idsSQLGetTypeInfo,
	SQL_API_SQLPARAMDATA,				(LPSTR)szLvl1,		idsSQLParamData,
	SQL_API_SQLPUTDATA,					(LPSTR)szLvl1,		idsSQLPutData,
	SQL_API_SQLSETCONNECTOPTION,		(LPSTR)szLvl1,		idsSQLSetConnectOption,
	SQL_API_SQLSETSTMTOPTION,			(LPSTR)szLvl1,		idsSQLSetStmtOption,
	SQL_API_SQLSPECIALCOLUMNS,			(LPSTR)szLvl1,		idsSQLSpecialColumns,
	SQL_API_SQLSTATISTICS,				(LPSTR)szLvl1,		idsSQLStatistics,
	SQL_API_SQLTABLES,					(LPSTR)szLvl1,		idsSQLTables,
//---- Level 2 Conformance ----------------------------------
	SQL_API_SQLBROWSECONNECT,			(LPSTR)szLvl2,		idsSQLBrowseConnect,
	SQL_API_SQLCOLUMNPRIVILEGES,		(LPSTR)szLvl2,		idsSQLColumnPrivileges,
	SQL_API_SQLDATASOURCES,				(LPSTR)szLvl2,		idsSQLDataSources,
	SQL_API_SQLDESCRIBEPARAM,			(LPSTR)szLvl2,		idsSQLDescribeParam,
	SQL_API_SQLEXTENDEDFETCH,			(LPSTR)szLvl2,		idsSQLExtendedFetch,
	SQL_API_SQLFOREIGNKEYS,				(LPSTR)szLvl2,		idsSQLForeignKeys,
	SQL_API_SQLMORERESULTS,				(LPSTR)szLvl2,		idsSQLMoreResults,
	SQL_API_SQLNATIVESQL,				(LPSTR)szLvl2,		idsSQLNativeSql,
	SQL_API_SQLNUMPARAMS,				(LPSTR)szLvl2,		idsSQLNumParams,
	SQL_API_SQLPARAMOPTIONS,			(LPSTR)szLvl2,		idsSQLParamOptions,
	SQL_API_SQLPRIMARYKEYS,				(LPSTR)szLvl2,		idsSQLPrimaryKeys,
	SQL_API_SQLPROCEDURECOLUMNS,		(LPSTR)szLvl2,		idsSQLProcedureColumns,
	SQL_API_SQLPROCEDURES,				(LPSTR)szLvl2,		idsSQLProcedures,
	SQL_API_SQLSETPOS,					(LPSTR)szLvl2,		idsSQLSetPos,
	SQL_API_SQLSETSCROLLOPTIONS,		(LPSTR)szLvl2,		idsSQLSetScrollOptions,
	SQL_API_SQLTABLEPRIVILEGES,		(LPSTR)szLvl2,		idsSQLTablePrivileges,
	};


//
// Generic prototype for bitmap structures
// 
typedef struct tagINFOSTRUCT {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} INFOSTRUCT;
typedef INFOSTRUCT FAR * lpINFOSTRUCT;

typedef struct tagDEXSTRUCT {
	int				idsStr;							// String index number
	} DEXSTRUCT;
typedef DEXSTRUCT FAR * lpDEXSTRUCT;
	
//
//	Structure for SQL_FETCH_DIRECTION
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoFetchDir[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FD_FETCH_NEXT,				idsSQL_FD_FETCH_NEXT,
	SQL_FD_FETCH_FIRST,				idsSQL_FD_FETCH_FIRST,
	SQL_FD_FETCH_LAST,				idsSQL_FD_FETCH_LAST,
	SQL_FD_FETCH_PREV,				idsSQL_FD_FETCH_PREV,
	SQL_FD_FETCH_ABSOLUTE,			idsSQL_FD_FETCH_ABSOLUTE,
	SQL_FD_FETCH_RELATIVE,			idsSQL_FD_FETCH_RELATIVE,
	SQL_FD_FETCH_RESUME,				idsSQL_FD_FETCH_RESUME,
	};
	
//
//	Structure for SQL_DEFAULT_TXN_ISOLATION
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoTXNIsolation[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_TXN_READ_UNCOMMITTED,		idsSQL_TXN_READ_UNCOMMITTED,
	SQL_TXN_READ_COMMITTED,			idsSQL_TXN_READ_COMMITTED,
	SQL_TXN_REPEATABLE_READ,		idsSQL_TXN_REPEATABLE_READ,
	SQL_TXN_SERIALIZABLE,			idsSQL_TXN_SERIALIZABLE,
	SQL_TXN_VERSIONING,				idsSQL_TXN_VERSIONING,
	};
	
//
//	Structure for SQL_SCROLL_CONCURRENCY
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoConcurr[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_SCCO_READ_ONLY,				idsSQL_SCCO_READ_ONLY,
	SQL_SCCO_LOCK,						idsSQL_SCCO_LOCK,
	SQL_SCCO_OPT_TIMESTAMP,			idsSQL_SCCO_OPT_TIMESTAMP,
	SQL_SCCO_OPT_VALUES,				idsSQL_SCCO_OPT_VALUES,
	};
	
//
//	Structure for SQL_SCROLL_OPTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoScrollOptions[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_SO_FORWARD_ONLY,				idsSQL_SO_FORWARD_ONLY,
	SQL_SO_KEYSET_DRIVEN,			idsSQL_SO_KEYSET_DRIVEN,
	SQL_SO_DYNAMIC,					idsSQL_SO_DYNAMIC,
	SQL_SO_MIXED,						idsSQL_SO_MIXED,
	};
	
//
//	Structure for SQL_TXN_ISOLATION_OPTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoTxnIsoOptions[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_TXN_READ_UNCOMMITTED,		idsSQL_TXN_READ_UNCOMMITTED,
	SQL_TXN_READ_COMMITTED,			idsSQL_TXN_READ_COMMITTED,
	SQL_TXN_REPEATABLE_READ,		idsSQL_TXN_REPEATABLE_READ,
	SQL_TXN_SERIALIZABLE,			idsSQL_TXN_SERIALIZABLE,
	SQL_TXN_VERSIONING,				idsSQL_TXN_VERSIONING,
	};

//
//	Structure for SQL_CONVERT_FUNCTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoConvert[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FN_CVT_CONVERT,				idsSQL_FN_CVT_CONVERT,
	};

//
//	Structure for SQL_NUMERIC_FUNCTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoNumeric[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FN_NUM_ABS,					idsSQL_FN_NUM_ABS,
	SQL_FN_NUM_ACOS,					idsSQL_FN_NUM_ACOS,
	SQL_FN_NUM_ASIN,					idsSQL_FN_NUM_ASIN,
	SQL_FN_NUM_ATAN,					idsSQL_FN_NUM_ATAN,
	SQL_FN_NUM_ATAN2,					idsSQL_FN_NUM_ATAN2,
	SQL_FN_NUM_CEILING,				idsSQL_FN_NUM_CEILING,
	SQL_FN_NUM_COS,					idsSQL_FN_NUM_COS,
	SQL_FN_NUM_COT,					idsSQL_FN_NUM_COT,
	SQL_FN_NUM_EXP,					idsSQL_FN_NUM_EXP,
	SQL_FN_NUM_FLOOR,					idsSQL_FN_NUM_FLOOR,
	SQL_FN_NUM_LOG,					idsSQL_FN_NUM_LOG,
	SQL_FN_NUM_MOD,					idsSQL_FN_NUM_MOD,
	SQL_FN_NUM_RAND,					idsSQL_FN_NUM_RAND,
	SQL_FN_NUM_PI,						idsSQL_FN_NUM_PI,
	SQL_FN_NUM_SIGN,					idsSQL_FN_NUM_SIGN,
	SQL_FN_NUM_SIN,					idsSQL_FN_NUM_SIN,
	SQL_FN_NUM_SQRT,					idsSQL_FN_NUM_SQRT,
	SQL_FN_NUM_TAN,					idsSQL_FN_NUM_TAN,
	};

//
//	Structure for SQL_STRING_FUNCTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoStrings[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FN_STR_ASCII,					idsSQL_FN_STR_ASCII,
	SQL_FN_STR_CHAR,					idsSQL_FN_STR_CHAR,
	SQL_FN_STR_CONCAT,				idsSQL_FN_STR_CONCAT,
	SQL_FN_STR_INSERT,				idsSQL_FN_STR_INSERT,
	SQL_FN_STR_LEFT,					idsSQL_FN_STR_LEFT,
	SQL_FN_STR_LTRIM,					idsSQL_FN_STR_LTRIM,
	SQL_FN_STR_LENGTH,				idsSQL_FN_STR_LENGTH,
	SQL_FN_STR_LOCATE,				idsSQL_FN_STR_LOCATE,
	SQL_FN_STR_LCASE,					idsSQL_FN_STR_LCASE,
	SQL_FN_STR_REPEAT,				idsSQL_FN_STR_REPEAT,
	SQL_FN_STR_REPLACE,				idsSQL_FN_STR_REPLACE,
	SQL_FN_STR_RIGHT,					idsSQL_FN_STR_RIGHT,
	SQL_FN_STR_RTRIM,					idsSQL_FN_STR_RTRIM,
	SQL_FN_STR_SUBSTRING,			idsSQL_FN_STR_SUBSTRING,
	SQL_FN_STR_UCASE,					idsSQL_FN_STR_UCASE,
	};

//
//	Structure for SQL_SYSTEM_FUNCTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoSystem[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FN_SYS_USERNAME,				idsSQL_FN_SYS_USERNAME,
	SQL_FN_SYS_DBNAME,				idsSQL_FN_SYS_DBNAME,
	SQL_FN_SYS_IFNULL,				idsSQL_FN_SYS_IFNULL,
	};

//
//	Structure for SQL_TIMEDATE_FUNCTIONS
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoTimeDate[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_FN_TD_NOW,						idsSQL_FN_TD_NOW,
	SQL_FN_TD_CURDATE,				idsSQL_FN_TD_CURDATE,
	SQL_FN_TD_DAYOFMONTH,			idsSQL_FN_TD_DAYOFMONTH,
	SQL_FN_TD_DAYOFWEEK,				idsSQL_FN_TD_DAYOFWEEK,
	SQL_FN_TD_DAYOFYEAR,				idsSQL_FN_TD_DAYOFYEAR,
	SQL_FN_TD_MONTH,					idsSQL_FN_TD_MONTH,
	SQL_FN_TD_QUARTER,				idsSQL_FN_TD_QUARTER,
	SQL_FN_TD_WEEK,					idsSQL_FN_TD_WEEK,
	SQL_FN_TD_YEAR,					idsSQL_FN_TD_YEAR,
	SQL_FN_TD_CURTIME,				idsSQL_FN_TD_CURTIME,
	SQL_FN_TD_HOUR,					idsSQL_FN_TD_HOUR,
	SQL_FN_TD_MINUTE,					idsSQL_FN_TD_MINUTE,
	SQL_FN_TD_SECOND,					idsSQL_FN_TD_SECOND,
	};

//
//	Structure for SQL_CONVERT_xxxx
//
struct {
	UDWORD			fVal;								// Which constant
	int				idsStr;							// String index number
	} GetInfoConvertTypes[] = {
//	fVal									idsStr
//	-----------------------------	-------------------------------
	SQL_CVT_BIGINT,					idsCVT_BIGINT,
	SQL_CVT_BINARY,					idsCVT_BINARY,
	SQL_CVT_BIT,						idsCVT_BIT,
	SQL_CVT_CHAR,						idsCVT_CHAR,
	SQL_CVT_DATE,						idsCVT_DATE,
	SQL_CVT_DECIMAL,					idsCVT_DECIMAL,
	SQL_CVT_DOUBLE,					idsCVT_DOUBLE,
	SQL_CVT_FLOAT,						idsCVT_FLOAT,
	SQL_CVT_INTEGER,					idsCVT_INTEGER,
	SQL_CVT_LONGVARBINARY,			idsCVT_LONGVARBINARY,
	SQL_CVT_LONGVARCHAR,				idsCVT_LONGVARCHAR,
	SQL_CVT_NUMERIC,					idsCVT_NUMERIC,
	SQL_CVT_REAL,						idsCVT_REAL,
	SQL_CVT_SMALLINT,					idsCVT_SMALLINT,
	SQL_CVT_TIME,						idsCVT_TIME,
	SQL_CVT_TIMESTAMP,				idsCVT_TIMESTAMP,
	SQL_CVT_TINYINT,					idsCVT_TINYINT,
	SQL_CVT_VARBINARY,				idsCVT_VARBINARY,
	SQL_CVT_VARCHAR,					idsCVT_VARCHAR,
	};
	
//
//	Structure for SQL_ODBC_API_CONFORMANCE
//
struct {
	int				idsStr;							// String index number
	} GetInfoAPIConform[] = {
//	idsStr
//	-----------------------------
	idsSQL_CONFORM_NONE,
	idsSQL_LEVEL1_NONE,
	idsSQL_LEVEL2_NONE,
	};
	
//
//	Structure for SQL_ODBC_SAG_API_CONFORMANCE
//
struct {
	int				idsStr;							// String index number
	} GetInfoSAGCLIConform[] = {
//	idsStr
//	-----------------------------
	idsSQL_NONSAG,
	idsSQL_SAG,
	};
	
//
//	Structure for SQL_ODBC_SQL_CONFORMANCE
//
struct {
	int				idsStr;							// String index number
	} GetInfoODBCSQL[] = {
//	idsStr
//	-----------------------------
	idsCONFORM_MINIMUM,
	idsCONFORM_CORE,
	idsCONFORM_EXTENDED,
	};
	
//
//	Structure for SQL_CONCAT_NULL_BEHAVIOR
//
struct {
	int				idsStr;							// String index number
	} GetInfoConcat[] = {
//	idsStr
//	-----------------------------
	idsCONCAT_0,
	idsCONCAT_1,
	};
	
//
//	Structure for SQL_CURSOR_COMMIT_BEHAVIOR
//
struct {
	int				idsStr;							// String index number
	} GetInfoCommit[] = {
//	idsStr
//	-----------------------------
	idsCOMMIT_0,
	idsCOMMIT_1,
	idsCOMMIT_2,
	};
	
//
//	Structure for SQL_CURSOR_ROLLBACK_BEHAVIOR
//
struct {
	int				idsStr;							// String index number
	} GetInfoRollback[] = {
//	idsStr
//	-----------------------------
	idsROLLBACK_0,
	idsROLLBACK_1,
	idsROLLBACK_2,
	};
	
//
//	Structure for SQL_IDENTIFIER_CASE
//
struct {
	int				idsStr;							// String index number
	} GetInfoIDCase[] = {
//	idsStr
//	-----------------------------
	0,
	idsUPPERCASE,
	idsLOWERCASE,
	idsCASESENSITIVE,
	idsNOTCASESENSITIVE,
	};
	
//
//	Structure for SQL_TXN_CAPABLE
//
struct {
	int				idsStr;							// String index number
	} GetInfoTxnCapable[] = {
//	idsStr
//	-----------------------------
	idsNOTRANS,
	idsDMLSUPPORT,
	idsDDLSUPPORT,
	};

//
//	The following structure is used to retrieve information about the driver.  There
//		are 5 types of GetInfo structures:
//				INT16			16-bit value
//				INT32			32-bit value
//				STRVAL		String value
//				DEXVAL		Indexed item (eg: 0-x)
//				BITVAL		Bit-mask value
//
char				szGetInfo[MAXBUFF];
UWORD				cb16;
UDWORD			cb32;
#define			INT16					1
#define			INT32					2
#define			STRVAL				3
#define			DEXVAL				4
#define			BITVAL				5
struct {
	UWORD				fInfoType;						// What we're looking for
	int				fOutType;						// string, 16-bit or 32-bit
	PTR				rgbInfoValue;					// Output buffer
	SWORD				cbInfoMax;						// Size of output buffer
	void FAR *		ptr;								// Generic constant structure
	int				cbNum;							// Count of items in ptr
	} GetInfo[] = {
//	fInfoType								fOutType			rgbInfoValue		cbInfoMax		ptr							cbNum
//	--------------------					-----------		-----------------	------------- 	----------------------	--------------------------
// Connection and environment info
	SQL_ACTIVE_CONNECTIONS,				INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_ACTIVE_STATEMENTS,				INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_DATA_SOURCE_NAME,				STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_DRIVER_HENV,						INT32,			&cb32,				sizeof(HENV),	NULL,							0,
	SQL_DRIVER_HDBC,						INT32,			&cb32,				sizeof(HDBC),	NULL,							0,
	SQL_DRIVER_HSTMT,						INT32,			&cb32,				sizeof(HSTMT),	NULL,							0,
	SQL_DRIVER_NAME,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_DRIVER_VER,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_FETCH_DIRECTION,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoFetchDir,			NumItems(GetInfoFetchDir),
	SQL_ODBC_API_CONFORMANCE,			DEXVAL,			&cb16,				sizeof(cb16),	GetInfoAPIConform,		NumItems(GetInfoAPIConform),
	SQL_ODBC_SAG_CLI_CONFORMANCE,		DEXVAL,			&cb16,				sizeof(cb16),	GetInfoSAGCLIConform,	NumItems(GetInfoSAGCLIConform),
	SQL_ODBC_SQL_CONFORMANCE,			DEXVAL,			&cb16,				sizeof(cb16),	GetInfoODBCSQL,			NumItems(GetInfoODBCSQL),
	SQL_ODBC_SQL_OPT_IEF,				STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_ODBC_VER,							STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_PROCEDURES,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_ROW_UPDATES,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_SEARCH_PATTERN_ESCAPE,			STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_SERVER_NAME,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
// DBMS Info
	SQL_DATABASE_NAME,					STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_DBMS_NAME,							STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_DBMS_VER,							STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
//	Status Info
	SQL_ACCESSIBLE_TABLES,				STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_ACCESSIBLE_PROCEDURES,			STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_CONCAT_NULL_BEHAVIOR,			DEXVAL,			&cb16,				sizeof(cb16),	GetInfoConcat,				NumItems(GetInfoConcat),
	SQL_CURSOR_COMMIT_BEHAVIOR,		DEXVAL,			&cb16,				sizeof(cb16),	GetInfoCommit,				NumItems(GetInfoCommit),
	SQL_CURSOR_ROLLBACK_BEHAVIOR,		DEXVAL,			&cb16,				sizeof(cb16),	GetInfoRollback,			NumItems(GetInfoRollback),
	SQL_DATA_SOURCE_READ_ONLY,			STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_DEFAULT_TXN_ISOLATION,			BITVAL,			&cb32,				sizeof(cb32),	GetInfoTXNIsolation,		NumItems(GetInfoTXNIsolation),
	SQL_EXPRESSIONS_IN_ORDERBY,		STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_IDENTIFIER_CASE,					DEXVAL,			&cb16,				sizeof(cb16),	GetInfoIDCase,				NumItems(GetInfoIDCase),
	SQL_IDENTIFIER_QUOTE_CHAR,			INT16,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_MAX_COLUMN_NAME_LEN,			INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MAX_CURSOR_NAME_LEN,			INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MAX_OWNER_NAME_LEN,				INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MAX_PROCEDURE_NAME_LEN,		INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MAX_QUALIFIER_NAME_LEN,      INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MAX_TABLE_NAME_LEN,				INT16,			&cb16,				sizeof(cb16),	NULL,							0,
	SQL_MULT_RESULT_SETS,				STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_MULTIPLE_ACTIVE_TXN,			STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_OUTER_JOINS,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_OWNER_TERM,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_PROCEDURE_TERM,					STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_QUALIFIER_NAME_SEPARATOR,		STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_QUALIFIER_TERM,					STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_SCROLL_CONCURRENCY,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoConcurr,			NumItems(GetInfoConcurr),
	SQL_SCROLL_OPTIONS,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoScrollOptions,	NumItems(GetInfoScrollOptions),
	SQL_TABLE_TERM,						STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
	SQL_TXN_CAPABLE,						DEXVAL,			&cb16,				sizeof(cb16),	GetInfoTxnCapable,		NumItems(GetInfoTxnCapable),
	SQL_TXN_ISOLATION_OPTION,			BITVAL,			&cb32,				sizeof(cb32),	GetInfoTxnIsoOptions,	NumItems(GetInfoTxnIsoOptions),
	SQL_USER_NAME,							STRVAL,			szGetInfo,			MAXBUFF,			NULL,							0,
//	Scalar functions
	SQL_CONVERT_FUNCTIONS,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvert,			NumItems(GetInfoConvert),
	SQL_NUMERIC_FUNCTIONS,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoNumeric,			NumItems(GetInfoNumeric),
	SQL_STRING_FUNCTIONS,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoStrings,			NumItems(GetInfoStrings),
	SQL_SYSTEM_FUNCTIONS,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoSystem,				NumItems(GetInfoSystem),
	SQL_TIMEDATE_FUNCTIONS,				BITVAL,			&cb32,				sizeof(cb32), 	GetInfoTimeDate,			NumItems(GetInfoTimeDate),
// Conversions
	SQL_CONVERT_BIGINT,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_BINARY,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_BIT,						BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_CHAR,						BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_DATE,						BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_DECIMAL,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_DOUBLE,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_FLOAT,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_INTEGER,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_LONGVARBINARY,			BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_LONGVARCHAR,			BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_NUMERIC,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_REAL,						BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_SMALLINT,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_TIME,						BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_TIMESTAMP,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_TINYINT,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_VARBINARY,				BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	SQL_CONVERT_VARCHAR,					BITVAL,			&cb32,				sizeof(cb32),	GetInfoConvertTypes,		NumItems(GetInfoConvertTypes),	
	};

	
//*---------------------------------------------------------------------------------
//| GetIndexVal:
//|	Call this function to retrieve the string values for all items which meet
//|	the bitwise and condition.
//| Parms:
//|	in			rs							Pointer to the results set
//|	in			szConstant				The constant value being retrieve
//|	in			is							Structure with resource ids and values
//|	in			maxdex					Number of items in struct
//|	in			mask						The value to compare against
//|	in			szOut						Output buffer for retrieval
//|	in			cbOut						Size of output buffer
//| Returns:              
//|	Nothing
//*---------------------------------------------------------------------------------
BOOL GetBitVal(RESULTSSET FAR * rs, LPSTR szConstant,
				lpINFOSTRUCT is, int maxdex, UDWORD mask,
				LPSTR szVal, int cbVal)
{
	int				tdex;
	ROWDATA FAR * 	rd;
	COLORREF			rgbDft=GetDefaultRGB();
	
	for(tdex=0;  tdex<maxdex;  tdex++) 
		if(is[tdex].fVal & mask) {
			rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
			SetColumnData(0, rd, szConstant);
			iLoadString(is[tdex].idsStr, (LPSTR)szVal, cbVal);
			SetColumnData(1, rd, (LPSTR)szVal);
			if(AddRowData(rs, rd) == LB_ERRSPACE)
				return FALSE;
			}
 	return TRUE;
}

	
//*---------------------------------------------------------------------------------
//| GetIndexVal:
//|	Call this function to retrieve the string value for a constant.
//| Parms:
//|	in			rs							Pointer to the results set
//|	in			szConstant				The constant value being retrieve
//|	in			dex						String index value 
//|	in			szOut						Output buffer for retrieval
//|	in			cbOut						Size of output buffer
//| Returns:              
//|	FALSE if there is an error
//*---------------------------------------------------------------------------------
BOOL GetIndexVal(RESULTSSET FAR * rs, LPSTR szConstant, 
		int dex, LPSTR szOut, int cbOut)
{
	ROWDATA FAR * 	rd;
	COLORREF			rgbDft=GetDefaultRGB();
				
	rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
	SetColumnData(0, rd, szConstant);
	iLoadString(dex, (LPSTR)szOut, cbOut);
	SetColumnData(1, rd, (LPSTR)szOut);
	if(AddRowData(rs, rd) == LB_ERRSPACE)
		return FALSE;
	return TRUE;
}

	
//*---------------------------------------------------------------------------------
//| DisplayGetInfo:
//|	This function goes through all of the SQLGetInfo constants defined in the
//|	ODBC reference guide and displays them in a results set.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	TRUE if successful,
//|	FALSE on error
//*---------------------------------------------------------------------------------
BOOL DisplayGetInfo(CHILDINFO FAR * ci)
{
	char						szConstant[40];
	char						szTitle[MAXBUFF];
	char						szVal[MAXBUFF];
	int 						dex;
	lpDEXSTRUCT				lpDEX;
	lpINFOSTRUCT			lpINFS;
	RESULTSSET FAR * 		rs;
	ROWDATA FAR *			rd;
	RETCODE					retcode;
	COLORREF					rgbDft=GetDefaultRGB();

	//
	//	Create a hard coded results set with 2 columns
	//
	lstrcpy((LPSTR)szTitle, (LPSTR)ci->szClientTitle);
	lstrcat((LPSTR)szTitle, (LPSTR)szDash);
	lstrcat((LPSTR)szTitle, (LPSTR)szGetInfoTitle);
	rs = GetConnectWindowResultsNode(ci);
	if(!CreateResultsSet(rs, ci->hwndClient, ci->hInst, 2, (LPSTR)szTitle))
		return FALSE;	

	//
	// Set the meta data
	//
	SetMetaDataColumn(rs, 0, (LPSTR)"fInfoType", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 40, 0,
			45, TA_LEFT);
	SetMetaDataColumn(rs, 1, (LPSTR)"Value", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 25, 0,
			35, TA_LEFT);

	//
	// Now create the MDI child window which will hold the results.
	//
	if(!CreateResultsWindow(ci, rs)) 
		goto exit00;

   
	//
	// Loop through the control structure and check each fInfoType.  Certain
	//		types require extra processing.
	//
	for(dex=0;  dex<NumItems(GetInfo);  dex++) {
		if(GetInfo[dex].fInfoType == SQL_DRIVER_HSTMT)				// Input arg also
			*(HSTMT FAR *)GetInfo[dex].rgbInfoValue = ci->hstmt;
			
		retcode = SQLGetInfo(ci->hdbc, 
					GetInfo[dex].fInfoType, 
					GetInfo[dex].rgbInfoValue, 
					GetInfo[dex].cbInfoMax, 
					NULL);
		if(retcode != SQL_SUCCESS)
			PrintErrors(ci);

		iLoadString(idsGetInfoBase+GetInfo[dex].fInfoType, 
					(LPSTR)szConstant, sizeof(szConstant));

		switch(GetInfo[dex].fInfoType) {
			case SQL_DRIVER_HENV:
			case SQL_DRIVER_HDBC:
			case SQL_DRIVER_HSTMT:
				rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
				SetColumnData(0, rd, szConstant);
				wsprintf(szVal, "%04X:%04X", HIWORD(cb32), LOWORD(cb32));
				SetColumnData(1, rd, (LPSTR)szVal);
				if(AddRowData(rs, rd) == LB_ERRSPACE)
					goto exit00;
				break;
				
			//
			//	The default action is taken when we only need to display the
			//		value as is.  We can use the structure to figure out what
			//		format it is in.
			//
			default:
				rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
				SetColumnData(0, rd, szConstant);
				switch(GetInfo[dex].fOutType) {
					case INT16:
						wsprintf(szVal, "%d", cb16);
						SetColumnData(1, rd, (LPSTR)szVal);
						if(AddRowData(rs, rd) == LB_ERRSPACE)
							goto exit00;
						break;
						
					case INT32:
						wsprintf(szVal, "%ld", cb32);
						SetColumnData(1, rd, (LPSTR)szVal);
						if(AddRowData(rs, rd) == LB_ERRSPACE)
							goto exit00;
						break;

					case DEXVAL:
						lpDEX = (lpDEXSTRUCT)GetInfo[dex].ptr;
						if(!GetIndexVal(rs, (LPSTR)szConstant, 
									lpDEX[cb16].idsStr,
									(LPSTR)szVal, sizeof(szVal)))
							goto exit00;
						break;
						
					case BITVAL:
						lpINFS = (lpINFOSTRUCT)GetInfo[dex].ptr;
						if(!GetBitVal(rs, (LPSTR)szConstant,
									lpINFS,
									GetInfo[dex].cbNum, cb32,
									(LPSTR)szVal, sizeof(szVal)))
							goto exit00;
						break;
						
					default: 
						SetColumnData(1, rd, (LPSTR)GetInfo[dex].rgbInfoValue);
						if(AddRowData(rs, rd) == LB_ERRSPACE)
							goto exit00;
						break;
					}
			}
			
		}
		
	return TRUE;
	
exit00:
	return FALSE;
}

	
//*---------------------------------------------------------------------------------
//| DisplayODBCFunctions:
//|	This function will enumerate all of the ODBC functions in a results set
//|		and indicate which ones are supported.  The results set is attatched
//|		as a valid results window on the current child.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	TRUE if successful,
//|	FALSE on error
//*---------------------------------------------------------------------------------
BOOL DisplayODBCFunctions(CHILDINFO FAR * ci)
{
	UWORD						fSupport;
	char						szFuncName[35];
	char						szTitle[MAXBUFF];
	int 						dex;
	RESULTSSET FAR * 		rs;
	ROWDATA FAR *			rd;
	RETCODE					retcode;
	COLORREF					rgbDft=GetDefaultRGB();
	//
	//	Create a hard coded results set with 3 columns
	//
	lstrcpy((LPSTR)szTitle, (LPSTR)ci->szClientTitle);
	lstrcat((LPSTR)szTitle, (LPSTR)szDash);
	lstrcat((LPSTR)szTitle, (LPSTR)szODBCFunctions);
	rs = GetConnectWindowResultsNode(ci);
	if(!CreateResultsSet(rs, ci->hwndClient, ci->hInst, 3, (LPSTR)szTitle))
		return FALSE;	

	//
	// Set the meta data
	//
	SetMetaDataColumn(rs, 0, (LPSTR)"Function", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 20, 0,
			25, TA_LEFT);
	SetMetaDataColumn(rs, 1, (LPSTR)"Conformance", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 10, 0,
			10, TA_LEFT);
	SetMetaDataColumn(rs, 2, (LPSTR)"Supported", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 10, 0,
			10, TA_LEFT);

	//
	// Now create the MDI child window which will hold the results.
	//
	if(!CreateResultsWindow(ci, rs)) 
		goto exit00;

   
	//
	// Loop through the control structure and check each function.  For each row
	//		add a record with the function name, conformance level, and Yes if
	//		supported, No if not.
	//
	for(dex=0;  dex<NumItems(ODBCFunctions);  dex++) {
		retcode = SQLGetFunctions(ci->hdbc, 
					ODBCFunctions[dex].fFunction, &fSupport);
		if(retcode != SQL_SUCCESS)
			PrintErrors(ci);
		iLoadString(ODBCFunctions[dex].idFunction, (LPSTR)szFuncName, sizeof(szFuncName));
		rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
		SetColumnData(0, rd, szFuncName);
		SetColumnData(1, rd, ODBCFunctions[dex].szLevel);
		SetColumnData(2, rd, (fSupport) ? (LPSTR)szYes : (LPSTR)szNo);
		AddRowData(rs, rd);
		}
		
	return TRUE;
	
exit00:
	SQLFreeStmt(ci->hstmt, SQL_CLOSE);
	return FALSE;
}


//*---------------------------------------------------------------------------------
//| DisplayODBCDataSources:
//|	This function will enumerate all of the ODBC Data sources.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	TRUE if successful,
//|	FALSE on error
//*---------------------------------------------------------------------------------
BOOL DisplayODBCDataSources(CHILDINFO FAR * ci)
{  
	HENV						henv;
	RESULTSSET FAR * 		rs;
	ROWDATA FAR *			rd;
	RETCODE					retcode;
	char						szDSN[SQL_MAX_DSN_LENGTH + 1];
	char						szDesc[MAXBUFF];
	char						szTitle[MAXBUFF];
	COLORREF					rgbDft=GetDefaultRGB();

	//
	//	Create a hard coded results set with 2 columns
	//
	lstrcpy((LPSTR)szTitle, (LPSTR)ci->szClientTitle);
	lstrcat((LPSTR)szTitle, (LPSTR)szDash);
	lstrcat((LPSTR)szTitle, (LPSTR)szODBCDataSources);
	rs = GetConnectWindowResultsNode(ci);
	if(!CreateResultsSet(rs, ci->hwndClient, ci->hInst, 2, (LPSTR)szTitle))
		return FALSE;	

	//
	// Set the meta data
	//
	SetMetaDataColumn(rs, 0, (LPSTR)"Driver", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 15, 0,
			15, TA_LEFT);
	SetMetaDataColumn(rs, 1, (LPSTR)"Description", 
			GetTypeName(SQL_TYPE, SQL_CHAR), SQL_CHAR, 35, 0,
			35, TA_LEFT);

	//
	// Now create the MDI child window which will hold the results.
	//
	if(!CreateResultsWindow(ci, rs)) 
		goto exit00;

   
	//
	// Loop through each data source and add it to the results set.
	//
	SQLAllocEnv(&henv);
	retcode = SQLDataSources(henv, SQL_FETCH_FIRST, szDSN, sizeof(szDSN),
					NULL, szDesc, sizeof(szDesc), NULL);
	while(retcode != SQL_NO_DATA_FOUND) {
		if(retcode != SQL_SUCCESS)
			PrintErrors(ci);
		rd = AllocateRowData(rs, rgbDft, RDATA_DEFAULT_BKGRND);
		SetColumnData(0, rd, szDSN);
		SetColumnData(1, rd, szDesc);
		AddRowData(rs, rd);
		retcode = SQLDataSources(henv, SQL_FETCH_NEXT, szDSN, sizeof(szDSN),
					NULL, szDesc, sizeof(szDesc), NULL);
		}
	SQLFreeEnv(henv);	
	return TRUE;
	
exit00:
	return FALSE;
}



//*---------------------------------------------------------------------------------
//| DisplayODBCDataTypes:
//|	This function will enumerate data type information.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	TRUE if successful,
//|	FALSE on error
//*---------------------------------------------------------------------------------
BOOL DisplayODBCDataTypes(CHILDINFO FAR * ci)
{  
	RESULTSSET FAR * 		rs;
	RETCODE					retcode;
	SWORD						cbCols;
	char						szTitle[MAXBUFF];

	//
	//	We'll use SQLGetTypeInfo for this query.  Since this function can return more
	//		than the standard types, we must first execute the query and then create
	//		the results set.
	//
	lstrcpy((LPSTR)szTitle, (LPSTR)ci->szClientTitle);
	lstrcat((LPSTR)szTitle, (LPSTR)szDash);
	lstrcat((LPSTR)szTitle, (LPSTR)szODBCDataTypes);
	retcode = SQLGetTypeInfo(ci->hstmt, SQL_ALL_TYPES);
	if(retcode != SQL_SUCCESS) {
		PrintErrors(ci);
		return FALSE;
		}

	if(!(cbCols = GetNumResultsCols(ci->hstmt)))
		return FALSE;

	rs = GetConnectWindowResultsNode(ci);
	if(!CreateResultsSet(rs, ci->hwndClient, ci->hInst, cbCols, (LPSTR)szTitle))
		return FALSE;	

	//
	// Set the meta data
	//
	SetMetaDataFromSql(ci->hwndOut, ci->hstmt, rs, cbCols);

	//
	// Now create the MDI child window which will hold the results.
	//
	if(!CreateResultsWindow(ci, rs)) 
		goto exit00;

   
	//
	// Loop through each data source and add it to the results set.
	//
	FetchAllRecordsToResults(ci->hwndOut, ci->hstmt, rs, cbCols, TRUE);
	SQLFreeStmt(ci->hstmt, SQL_CLOSE);

	return TRUE;
	
exit00:
	return FALSE;
}



//*---------------------------------------------------------------------------------
//| EditPipe:
//|	This function allows the user to create a new pipe.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	TRUE if successful,
//|	FALSE on error
//*---------------------------------------------------------------------------------
void EditPipe(CHILDINFO FAR * ci)
{
	DLGPROC  dlgproc;
	HWND		fHwnd=GetFocus();

	dlgproc = MakeProcInstance(EditPipeWndProc, ci->hInst);
	if(-1 == DialogBoxParam(ci->hInst, 
			MAKEINTRESOURCE(IDD_EDIT_PIPE),
			ci->hwnd, 
			dlgproc, (LPARAM)ci))
		MessageBox(NULL, "Could not open dialog box.",
			"Pipe", MB_ICONEXCLAMATION);
	FreeProcInstance((FARPROC) dlgproc);
	
	if(fHwnd)
		SetFocus(fHwnd);
}



//*------------------------------------------------------------------------
//| IsValidParms:
//|	Verify that the parameters specified are the correct comma
//|	separated format.
//| Parms:
//|	hwnd					Window handle for errors
//|	szParms				The null terminated list of parms
//| Returns:              
//|	TRUE if they are valid, FALSE on error
//*------------------------------------------------------------------------
BOOL WINAPI IsValidParms(HWND hwnd, LPSTR szParms)
{
	LPSTR			str=szParms, nstr;
	char			sztmp[MAXSQL];
	int			iNum, iCnt=0;

	lstrcpy(sztmp, szParms);
	nstr = str = strtok(sztmp, szCOMMA);
	while(str) {
		++iCnt;
		if(!(strlen(str) == 1 && *str == '0')) {
			iNum = atoi(str);
			while(*str) {
				if(*str < '0' ||
					*str > '9')
					goto invalidint;
				++str;
				}
			
			// It was not 0, so if atoi returned 0 it was invalid
			if(!iNum)
				goto invalidint;
			}
		
		nstr = str = strtok(NULL, szCOMMA);
		}
		
	if(iCnt <= MAXPARMS)
		return TRUE;
	else {
		szMessageBox(hwnd,
				MB_ICONEXCLAMATION | MB_OK,
				(LPSTR)szErrorMsgTitle,
				iLoadString(idsTooManyParms, OutStr, MAXBUFF),
				iCnt,
				MAXPARMS);
		return FALSE;
		}

invalidint:
	szMessageBox(hwnd, 
			MB_ICONEXCLAMATION | MB_OK,
			(LPSTR)szErrorMsgTitle,
			iLoadString(idsInvalidInt, OutStr, MAXBUFF),
			nstr);
			
	return FALSE;
}


//*------------------------------------------------------------------------
//| EditPipeWndProc:
//|	Message handler for creating a new pipe.
//| Parms:
//|	in			Standard window parms
//| Returns:              
//|	Depends on message
//*------------------------------------------------------------------------
BOOL CALLBACK EditPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static CHILDINFO FAR *	ci;
	static					cbNames;
	static char				szName[MAXNAME];

	switch(msg) {
		case WM_INITDIALOG:
			{
			ci = (CHILDINFO FAR *)lParam;
			CenterDialog(hDlg);

			SendMessage(GetDlgItem(hDlg, IDC_NAME), CB_LIMITTEXT, MAXPARMS, 0L);
			SendMessage(GetDlgItem(hDlg, IDE_SQL), EM_LIMITTEXT, MAXSQL, 0L);
			SendMessage(GetDlgItem(hDlg, IDE_PARMS), EM_LIMITTEXT, (MAXNAME*2), 0L);
			CheckRadioButton(hDlg, IDR_VALUE, IDR_ADDRESS, IDR_VALUE);
			SendMessage(hDlg, USER_RESETLIST, 0, 0L);
			SendMessage(hDlg, USER_SETSTATES, 0, 0L);
			}
			return TRUE;		


		// This user message is sent when the list needs to be refreshed
		case USER_RESETLIST:
			{
			LPSTR			str, addstr;

			addstr = str = (LPSTR)GetMemory(1000);
			if(!addstr)
				return TRUE;
			cbNames = 0;
			if(str) {
				SendMessage(GetDlgItem(hDlg, IDC_NAME), CB_RESETCONTENT, 0, 0L);
				if(GetPrivateProfileString((LPSTR)szPIPES, NULL, NULL,
							str, 1000, szLABINI)) 
					while(*addstr) {
						++cbNames;
						SendMessage(GetDlgItem(hDlg, IDC_NAME),
											CB_ADDSTRING, 0,
											(LPARAM)(LPSTR)addstr);
						addstr = addstr + lstrlen(addstr) + 1;
						}
				}
			ReleaseMemory(addstr);
			if(cbNames) 
				SendMessage(GetDlgItem(hDlg, IDC_NAME), CB_SETCURSEL, 0, 0L);
			SendMessage(hDlg, USER_SETDEFAULTS, 0, 0L);
			SendMessage(hDlg, USER_SETSTATES, 0, 0L);
			}
			return TRUE;
			
		// This user defined message will set the state of controls
		case USER_SETSTATES:
			EnableWindow(GetDlgItem(hDlg, IDB_DELETE), cbNames);
			EnableWindow(GetDlgItem(hDlg, IDE_SQL), cbNames);
			EnableWindow(GetDlgItem(hDlg, IDR_VALUE), cbNames);
			EnableWindow(GetDlgItem(hDlg, IDR_ADDRESS), cbNames);
			EnableWindow(GetDlgItem(hDlg, IDE_PARMS), cbNames);
			EnableWindow(GetDlgItem(hDlg, IDOK), cbNames);
			return TRUE;

		// This user defined message is for setting default values
		case USER_SETDEFAULTS: 
			{
			char 	szParmType[10];
			char	szSql[MAXSQL];
			char	szParms[MAXBUFF];
			
			HWND	hName = GetDlgItem(hDlg, IDC_NAME);
			
			if(cbNames == 0) {					// No current driver
				SetDlgItemText(hDlg, IDE_SQL, "");
				SetDlgItemText(hDlg, IDE_PARMS, "");
				return TRUE;
				}
			SendMessage(hName, CB_GETLBTEXT,
					(WPARAM)SendMessage(hName, CB_GETCURSEL, 0, 0L),
					(LPARAM)(LPSTR)szName);
			if(GetPrivateProfileString(szName, szSQL, NULL, szSql, sizeof(szSql), szLABINI))
				SetDlgItemText(hDlg, IDE_SQL, szSql);
			else
				SetDlgItemText(hDlg, IDE_SQL, "");
			if(GetPrivateProfileString(szName, szPARMS, NULL, szParms, sizeof(szParms), szLABINI))
				SetDlgItemText(hDlg, IDE_PARMS, szParms);
			else
				SetDlgItemText(hDlg, IDE_PARMS, "");
			if(GetPrivateProfileString(szName, szPARMOPT, NULL, szParmType, sizeof(szParmType), szLABINI))
				if(lstrcmpi(szVALUE, szParmType) == 0)
					CheckRadioButton(hDlg, IDR_VALUE, IDR_ADDRESS, IDR_VALUE);
				else
					CheckRadioButton(hDlg, IDR_VALUE, IDR_ADDRESS, IDR_ADDRESS);
			CheckDlgButton(hDlg, IDX_DELETE, GetPrivateProfileInt(szName, szDELETEOPT, 0, szLABINI));
			}
			return TRUE;
			
		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDB_NEW:
					{
					NEWPIPE	np;
					
					np.hwnd = hDlg;
					np.hInst = ci->hInst;
					NewPipe(&np);
					if(np.fSuccess) {
						lstrcpy(szName, np.szName);
						SendMessage(GetDlgItem(hDlg, IDC_NAME), CB_SETCURSEL,
								(WPARAM)SendMessage(GetDlgItem(hDlg, IDC_NAME), CB_ADDSTRING, 
									0,	(LPARAM)(LPSTR)szName), 0L);
						if(cbNames)
							SendMessage(hDlg, USER_SETDEFAULTS, 0, 0L);
						else
							SendMessage(hDlg, USER_RESETLIST, 0, 0L);
						}
					}					
					return TRUE;
					
				case IDB_DELETE:
					GetText(GetDlgItem(hDlg, IDC_NAME), szName);
					wsprintf(OutStr, szDeletePipe, (LPSTR)szName);
					if(IDOK == MessageBox(hDlg, OutStr, szEditPipe, MB_OKCANCEL)) {
						WritePrivateProfileString(szName, NULL, NULL, szLABINI);
						WritePrivateProfileString(szPIPES, szName, NULL, szLABINI);
						SendMessage(hDlg, USER_RESETLIST, 0, 0L);
						}
					return TRUE;
					
				//
				//	Read in the info from the dialog, validate the parms, write to file
				//
				case IDOK:
					{
					char	szSql[MAXSQL];
					char	szParms[MAXBUFF];

					GetDlgItemText(hDlg, IDC_NAME, (LPSTR)szName, sizeof(szName));

					GetDlgItemText(hDlg, IDE_PARMS, szParms, sizeof(szParms));
					if(IsValidParms(hDlg, szParms)) {
						WritePrivateProfileString(szName, szPARMS, szParms, szLABINI);
						
						GetDlgItemText(hDlg, IDE_SQL, szSql, sizeof(szSql));
						WritePrivateProfileString(szName, szSQL, szSql, szLABINI);

						if(IsDlgButtonChecked(hDlg, IDR_VALUE))                   
							WritePrivateProfileString(szName, szPARMOPT, szVALUE, szLABINI);
						else
							WritePrivateProfileString(szName, szPARMOPT, szADDRESS, szLABINI);

						WritePrivateProfileString(szName, szDELETEOPT, 
							(IsDlgButtonChecked(hDlg, IDX_DELETE)) ? (LPSTR)"1" : (LPSTR)"0",
							szLABINI);
						}
					}
					return TRUE;
            
            case IDCANCEL:
					EndDialog(hDlg, IDCANCEL);;
					return TRUE;
				}
			// Now check for control notification messages
			switch(HIWORD(lParam)) {
				case CBN_SELENDOK:
				case CBN_KILLFOCUS:
					SendMessage(hDlg, USER_SETDEFAULTS, TRUE, 0L);
					return TRUE;
				
				default:
					break;
				}
			break;


		default:
			return FALSE;
		}
	return FALSE;
}


//*---------------------------------------------------------------------------------
//| NewPipe:
//|	This function allows the user to create a new pipe.
//| Parms:
//|	in			ci							CHILDINFO information
//| Returns:              
//|	Nothing
//*---------------------------------------------------------------------------------
void NewPipe(NEWPIPE FAR * np)
{
	DLGPROC  dlgproc;
	HWND		fHwnd=GetFocus();

	dlgproc = MakeProcInstance(NewPipeWndProc, np->hInst);
	if(-1 == DialogBoxParam(np->hInst, 
			MAKEINTRESOURCE(IDD_NEW_PIPE),
			np->hwnd, 
			dlgproc, (LPARAM)np))
		MessageBox(NULL, "Could not open dialog box.",
			"Pipe", MB_ICONEXCLAMATION);
	FreeProcInstance((FARPROC) dlgproc);
	
	if(fHwnd)
		SetFocus(fHwnd);
}


//*------------------------------------------------------------------------
//| NewPipeWndProc:
//|	Message handler for creating a new pipe.
//| Parms:
//|	in			Standard window parms
//| Returns:              
//|	Depends on message
//*------------------------------------------------------------------------
BOOL CALLBACK NewPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char					szName[MAXNAME];
	static NEWPIPE FAR *	np;

	switch(msg) {
		case WM_INITDIALOG:
			{
			np = (NEWPIPE FAR *)lParam;
			CenterDialog(hDlg);
			SendMessage(GetDlgItem(hDlg, IDE_NAME), EM_LIMITTEXT, MAXNAME, 0L);
			}
			return TRUE;		


		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				case IDOK:
					{
					char szTmp[MAXNAME];

					//
					// Don't allow names with [,], or = in them, nor any
					// reserved section names
					//
					GetText(GetDlgItem(hDlg, IDE_NAME), (LPSTR)szName);
					if(!ValidName((LPSTR)szName) ||
						!*szName ||
						!lstrcmpi((LPSTR)szName, szSCREEN) ||
						!lstrcmpi((LPSTR)szName, szFONT) ||
						!lstrcmpi((LPSTR)szName, szCONNECTOPTIONS) ||
						!lstrcmpi((LPSTR)szName, szPIPES)) {

						szMessageBox(hDlg,
								MB_ICONEXCLAMATION | MB_OK,
								(LPSTR)szErrorMsgTitle,
								iLoadString(idsInvalidName, OutStr, MAXBUFF),
								(LPSTR)szName);
						return TRUE;
						}
					if(GetPrivateProfileString(szPIPES, szName, NULL,
							szTmp, sizeof(szTmp), szLABINI)) 
						MessageBox(hDlg, szDuplicatePipe, szEditPipe, MB_OK);
					else {
						lstrcpy(np->szName, szName);
						np->fSuccess = TRUE;
						WritePrivateProfileString(szPIPES, szName, szInstalled, szLABINI);
						EndDialog(hDlg, IDOK);
						}
					}
					return TRUE;
            
            case IDCANCEL:
					np->fSuccess = FALSE;
					EndDialog(hDlg, IDCANCEL);
					return TRUE;
				}
			break;

		default:
			return FALSE;
		}
	return FALSE;
}



//*---------------------------------------------------------------------------------
//| HandlePipe:
//|	This function will use the active results set and run use pipes against it.
//| Parms:
//|	lpci					Connection window information
//|	lpri					Ative results set
//| Returns:              
//|	Nothing.
//*---------------------------------------------------------------------------------
void HandlePipe(lpCHILDINFO lpci, lpRESULTSINFO lpri)
{
	DLGPROC  				dlgproc;
	HWND						fHwnd=GetFocus();

	dlgproc = MakeProcInstance(DoPipeWndProc, lpci->hInst);
	if(-1 == DialogBoxParam(lpci->hInst, 
			MAKEINTRESOURCE(IDD_DO_PIPE),
			lpci->hwnd, 
			dlgproc, (LPARAM)(lpRESULTSINFO)lpri))
		MessageBox(NULL, "Could not open dialog box.",
			"HandlePipe", MB_ICONEXCLAMATION);
	FreeProcInstance((FARPROC) dlgproc);
	
	if(fHwnd)
		SetFocus(fHwnd);
}


//*------------------------------------------------------------------------
//| DoPipeWndProc:
//|	Handle dialog messages for IDD_DO_PIPE.
//| Parms:
//|	in			Standard window parms
//| Returns:              
//|	Depends on message
//*------------------------------------------------------------------------
BOOL CALLBACK DoPipeWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static CHILDINFO FAR *		ci;
	static RESULTSSET FAR *		rs;
	static SDWORD				cbPipes;

	switch(msg) {
		case WM_INITDIALOG:
			{
			lpRESULTSINFO rwi;
			rwi = (lpRESULTSINFO)lParam;
			ci = rwi->ci;
			rs = rwi->rs;
			CenterDialog(hDlg);
			cbPipes = RefreshPipeList(GetDlgItem(hDlg, IDL_PIPES));
			EnableWindow(GetDlgItem(hDlg, IDL_PIPES), (int)(cbPipes));
			EnableWindow(GetDlgItem(hDlg, IDOK), (int)(cbPipes));
			}
			return TRUE;		


		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)) {
				// User has clicked OK.  Retrieve an array of the selected indexes
				// and run the current pipe against each.  Finally see if this
				// pipe wants to delete items.
				case IDOK:
					{
					int 			cbCnt;
					int FAR *	xSel;
					int			dex;
					char			szPipeName[MAXBUFF];

					Busy(TRUE);
					cbCnt = (int)SendMessage(rs->hwndList, LB_GETSELCOUNT, 0, 0L);
					xSel = (int FAR *)GetMemory(sizeof(int) * cbCnt);
					if(!xSel) {
						Busy(FALSE);
						return TRUE;
						}
					SendMessage(rs->hwndList, LB_GETSELITEMS, cbCnt, (LPARAM)(int FAR *)xSel);
					SendMessage(GetDlgItem(hDlg, IDL_PIPES), LB_GETTEXT,
								(WPARAM)SendMessage(GetDlgItem(hDlg, IDL_PIPES), LB_GETCURSEL, 0, 0L),
								(LPARAM)(LPSTR)szPipeName);
					DoPipe(rs, ci, (LPSTR)szPipeName, xSel, cbCnt);
					if(GetPrivateProfileInt(szPipeName, szDELETEOPT, 0, szLABINI))
						for(dex=cbCnt-1;  dex>=0;  dex--)
							SendMessage(rs->hwndList, LB_DELETESTRING, xSel[dex], 0L);
					Busy(FALSE);
					ReleaseMemory(xSel);
					}
					return TRUE;
            
            case IDCANCEL:
            	EndDialog(hDlg, IDCANCEL);
					return TRUE;
				}
			return TRUE;

		default:
			return FALSE;
		}
	return FALSE;
}




//*---------------------------------------------------------------------------------
//| RefreshPipeList:
//|	This function will reset the list of pipes based on the values returned
//|		from GetPipeName.  Having this extra level of abstraction allows us
//|		to change the location of the pipes without affecting this code.
//| Parms:
//|	in			hwnd						Window handle to list box to fill
//| Returns:              
//|	Number of items selected
//*---------------------------------------------------------------------------------
SDWORD RefreshPipeList(HWND hDlg)
{
#define MAX_PIPE_SIZE 4000
	LPSTR		szPipes, str;
	SDWORD count=0;
	
	szPipes = (LPSTR)GetMemory(MAX_PIPE_SIZE);
	if(!szPipes) 
		return 0;
	
	SendMessage(hDlg, LB_RESETCONTENT, 0, 0L);
	GetPipeNames((LPSTR)szPipes, MAX_PIPE_SIZE);
	str = szPipes;
	while(*str) {
		SendMessage(hDlg, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str);
		str += lstrlen(str) + 1;
		++count;
		}
	if(count)
		SendMessage(hDlg, LB_SETCURSEL, 0, 0L);

	ReleaseMemory(szPipes);
	return count;
}


//*---------------------------------------------------------------------------------
//| DoPipe:
//|	This function will implement a pipe against the object which is passed in.
//| Parms:
//|	in			rs							Pointer to results set describing data
//|	in			ci							Connection window information
//|	in			szPipeName				Name of pipe to use
//|	in			xPipes					Array of items to pipe
//|	in			cbCnt						Number of items
//| Returns:              
//|	Nothing.
//*---------------------------------------------------------------------------------
void DoPipe(RESULTSSET FAR * rs, CHILDINFO FAR * ci, LPSTR szPipeName,
			int FAR xPipes[], int cbCnt)
{
	SDWORD				cbDataAtExec=SQL_DATA_AT_EXEC;
	int					dex;
	int					cParm;
	char					szpsql[200];
	char					szparms[35];
	char					parmopt[10];
	UWORD					cParmCnt=0;
	LPSTR					str=szparms;
	LPSTR					numstr=szparms;
	ROWDATA FAR * 		rd;
	RETCODE				retcode;

	//
	// Make sure we can retrieve the pipe from the .ini file.  Also get the parameter
	//		values if they are available.
	//
	if(!GetPrivateProfileString(szPipeName, szSQL, NULL, szpsql, sizeof(szpsql), szLABINI)) {
		szWrite(ci->hwndOut, 
					GetidsString(idsPipeNotFound, OutStr, MAXBUFF), 
					(LPSTR)szPipeName);
		return;
		}
	GetPrivateProfileString(szPipeName, szPARMS, NULL, szparms, sizeof(szparms), szLABINI);
	GetPrivateProfileString(szPipeName, szPARMOPT, NULL, parmopt, sizeof(parmopt), szLABINI);

	//
	// If there are parameters to set, set each one based on user desription
	//
	if(str && *str)
		PrepareParmList(str);
	SQLFreeStmt(ci->hstmt, SQL_CLOSE);
	
	//
	// What type of parameter passing to do?  value means substitue text and execute,
	//		address means use parameter data.  The following will handle the former, the
	//		next more complicated routine will pass parameters.
	//
	if(lstrcmpi(parmopt, szVALUE) == 0) {
		DoPipeByValue(rs, ci, szpsql, str, xPipes, cbCnt, (LPSTR)szPipeName);
		return;
		}
   
   //
   // Now prepare the user's statement, return on error
   //
	retcode = SQLPrepare(ci->hstmt, szpsql, SQL_NTS);
	if(retcode != SQL_SUCCESS) {
		PrintErrors(ci);
		return;
		}
	
	//
	// For each parameter, make sure it's in our range, then see which mode we want,
	//		address (param data) or value (textual substitution).
	//
	while(*str) {
		++cParmCnt;
		cParm = lpatoi(str);
		if(cParm > rs->cbColumns) 
			szWrite(ci->hwndOut, 
					GetidsString(idsInvalidParamValue, OutStr, MAXBUFF), 
					cParm, (LPSTR)szPipeName);
		else {
			retcode = SQLBindParameter(ci->hstmt, 
					cParmCnt, SQL_PARAM_INPUT,
					SQL_C_CHAR, SQL_CHAR, 
					rs->md[cParm-1].precision,
					rs->md[cParm-1].scale, 
					(PTR FAR *)(cParm - 1), 
					rs->md[cParm-1].precision,
					&cbDataAtExec);
			if(retcode != SQL_SUCCESS) 
				PrintErrors(ci);
			}
		str += lstrlen(str) + 1;
		}	


	//
	// For each row selected, retrieve the row data structure associated with it,
	//		then do an execute.  When prompted for SQL_NEED_DATA, substitute the
	//		correct parameter address.
	//
	for(dex=0;  dex<cbCnt;  dex++) { 
		int		cNeedParm;
		rd = (ROWDATA FAR *)SendMessage(rs->hwndList, LB_GETITEMDATA, (WPARAM)xPipes[dex], 0L);
		retcode = SQLExecute(ci->hstmt);
		switch(retcode) {
			//
			// User had parameter data which we are being prompted for.  Since we
			//		did the SQLSetParam using the index number, we simply use that
			//		value to index into our column data and give the driver what
			//		it requires.
			//
			case SQL_NEED_DATA:
				retcode = SQLParamData(ci->hstmt, (PTR FAR *)&cNeedParm);
				while(retcode == SQL_NEED_DATA) {
					retcode = SQLPutData(ci->hstmt, rd->cd[cNeedParm].szCols, SQL_NTS);
					retcode = SQLParamData(ci->hstmt, (PTR FAR *)&cNeedParm);
					}
				break;
			
			case SQL_SUCCESS:
				CheckForResults(ci);
				break;
				
			default:
				PrintErrors(ci);
				break;
			}
		}
		
	SQLFreeStmt(ci->hstmt, SQL_CLOSE);
	return;
}



//*---------------------------------------------------------------------------------
//| PrepareParmList:
//|	The user will enter a list of numbers separated by columns which will
//|	designate which parms go for what marker.  We will turn this list into
//|	a double-null terminated list which can be used later for retrieval.
//| Parms:
//|	in			str						Pointer to string to work on
//| Returns:              
//|	Nothing.
//*---------------------------------------------------------------------------------
void PrepareParmList(LPSTR str)
{
	LPSTR tmpstr=str;
	LPSTR	lststr=tmpstr;

	//
	// Convert parm list into a double-null terminated list
	//
	while(tmpstr) {
		if((tmpstr = _fstrchr(str, ','))) {
			lststr = tmpstr + 1;
			*tmpstr++ = '\0';
			}
		else {
			lststr += lstrlen(lststr) + 1;
			*lststr = '\0';
			}
		}
}			



//*---------------------------------------------------------------------------------
//| DoPipeByValue:
//|	This function will process all of the selcted values by creating a
//|	statement which has all parameters embedded in it.
//|
//|	Note:  	There are some servers which use a semi-colon for the name of
//|				a stored procedure, but which cannot handle doing a drop of the
//|				object with this name.  If the pipe name is the reserved name
//|				of "Drop Procedure (with semi-colon)" then we will strip off the
//|				name since this can't really be done any other way.
//| Parms:
//|	in			rs							Results set pointer
//|	in			ci							Child information
//|	in			szUserSQL					Statement with parameter markers
//|	in			szParms					Parameter list, double null terminated
//|	in			xPipes					Array of indexes to use for param data
//|	in			cbCnt						Number of records to process
//|	in			szPipeName				Pipe names
//| Returns:              
//|	Nothing.
//*---------------------------------------------------------------------------------
void DoPipeByValue(RESULTSSET FAR * rs, CHILDINFO FAR * ci, LPSTR szUserSQL,
		LPSTR szParms, int FAR xPipes[], int cbCnt, LPSTR szPipeName)
{
	char					szUserSQLCopy[300];
	char					sqlstmt[300];
	LPSTR					szParmStrOut;
	LPSTR					szParmStrIn;
	LPSTR					szParmStrLast;
	LPSTR					str=szParms;
	int					dex;
	int					cParm;
	ROWDATA FAR *		rd;
	BOOL					fSemiProc=FALSE;
	
	// Handle special case of a procedure name with a semi-colon
	if(lstrcmp(szPipeName, szDROPPROCSEMI) == 0)
		fSemiProc = TRUE;
	
	//
	// For each record selected, create a statement which can be executed by finding
	//		parameter markers and replacing them at run time.
	//
	for(dex=0;  dex<cbCnt;  dex++) {
		_fmemset(sqlstmt, 0, sizeof(sqlstmt));
		_fmemset(szUserSQLCopy, 0, sizeof(szUserSQLCopy));
		lstrcpy(szUserSQLCopy, szUserSQL);
		szParmStrOut = sqlstmt;
		szParmStrIn = szUserSQLCopy;
		szParmStrLast = szParmStrIn;
		str = szParms;
		rd = (ROWDATA FAR *)SendMessage(rs->hwndList, LB_GETITEMDATA, (WPARAM)xPipes[dex], 0L);
		while(*str) {
			cParm = lpatoi(str);
			if(cParm > rs->cbColumns) 
				szWrite(ci->hwndOut, 
					GetidsString(idsInvalidParamValue, OutStr, MAXBUFF), 
					cParm, (LPSTR)szPipeName);
			else if(szParmStrIn && *szParmStrIn) {
				if((szParmStrIn = _fstrchr(szParmStrIn, '?'))) {
					*szParmStrIn++ = '\0';
					lstrcpy(szParmStrOut, szParmStrLast);
					_fstrcat(szParmStrOut, rd->cd[cParm-1].szCols);
					// Remove semi-colon for special case of drop procedure
					if(fSemiProc) {
						LPSTR		str = _fstrchr(szParmStrOut, ';');
						if(str)
							*str = '\0';
						}
					szParmStrLast = szParmStrIn; 
					}
				else
					lstrcpy(szParmStrOut, szParmStrLast);					// end of list
				}
			str += lstrlen(str) + 1;
			}
		if(*szParmStrLast)
			_fstrcat(szParmStrOut, szParmStrLast);
		ExecuteCmds(ci, sqlstmt);
		}
		
	return;
}
