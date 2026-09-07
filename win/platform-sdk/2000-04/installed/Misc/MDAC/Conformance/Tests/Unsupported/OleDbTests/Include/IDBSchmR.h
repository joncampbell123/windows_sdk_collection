//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1998 Microsoft Corporation.  
//
// @doc
//
// @module IDBSCHMR.H | Header file for IDBSchemaRowset test module.
//
// @rev 01 | 03-21-95 | Microsoft | Created
// @rev 02 | 02-23-98 | Microsoft | Updated
//

#ifndef _IDBSCHMR_H_
#define _IDBSCHMR_H_

#ifndef TESTPREP
	#include "oledb.h" 			// OLE DB Header Files
	#include "oledberr.h"

	#include "privlib.h"		// Private Library
#endif


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define	GET_ROWS 20
#define MAXPROP 5
#define MAXRESTRICTION 7

//-----------------------------------------------------------------------------
// ENUM
//-----------------------------------------------------------------------------
enum ETXN		{ETXN_COMMIT, ETXN_ABORT};
typedef enum tagROW_COUNT
{
	DEFAULT,		// Use the default for the schema obtained in GetSchemaInfo.
	MIN_VALUE,		// Row count must be at least some number if any rows, warning if 0.
	MIN_REQUIRED,	// Row count must be at least some number
	EXACT_VALUE		// Row count must be exactly some number
} ROW_COUNT;

//-----------------------------------------------------------------------------
// String constants
//-----------------------------------------------------------------------------
const WCHAR	wszErrorCreatingSession[] =		L"Error Creating Session, CreateDBSession FAILED\n";
const WCHAR wszVerifyInterfaceFailed[] =	L"Verify Interface FAILED\n";
const WCHAR wszGetSchemasFailed[] =			L"IDBSchemaRowset::GetSchemas FAILED\n";
const WCHAR wszGetRowsetFailed[] =			L"IDBSchemaRowset::GetRowset FAILED\n";
const WCHAR wszGetColumnInfoFailed[]=		L"IColumnsInfo::GetColumnInfo FAILED\n";
const WCHAR wszRowsetIsNull[]=				L"Rowset Is Null\n";
const WCHAR wszDropProc[]=					L"DROP PROCEDURE %s";
const WCHAR wszCreateProc[]=				L"CREATE PROCEDURE %s (@IntIn int =1, @IntOut int =2 OUTPUT) AS Select * from %s RETURN (1)";
const WCHAR wszProcDesignator[]=			L"_proc";

const WCHAR wszCATALOG[]=					L"nstl";
const WCHAR wszSCHEMA[]=					L"ODBC";
const WCHAR wszTABLETYPE[]=					L"SYSTEM TABLE";

const WCHAR wszGRANTOR[]=					L"ODBC";
const WCHAR wszGRANTEE[]=					L"ODBC2";

const WCHAR	wszTABLETYPE_ALIAS[]			=L"ALIAS";
const WCHAR	wszTABLETYPE_BASETABLE[]		=L"TABLE";
const WCHAR	wszTABLETYPE_SYNONYM[]			=L"SYNONYM";
const WCHAR	wszTABLETYPE_SYSTEMTABLE[]		=L"SYSTEM TABLE";
const WCHAR	wszTABLETYPE_VIEW[]				=L"VIEW";
const WCHAR	wszTABLETYPE_GLOBALTEMPORARY[]	=L"GLOBAL TEMPORARY";
const WCHAR	wszTABLETYPE_LOCALTEMPORARY[]	=L"LOCAL TEMPORARY";

const WCHAR wszCONSTRAINT_TYPE[]			=L"UNIQUE";
const WCHAR wszCREATESTORPROC[]				=L"Create procedure STORPROC as select * from %s";	
const WCHAR wszSTORPROC[]					=L"STORPROC";

const WCHAR wszPUBS[]						=L"Pubs";
const WCHAR wszNWIND[]						=L"NWIND";

const WCHAR wszDBSCHEMA_ASSERTIONS[]		=L"DBSCHEMA_ASSERTIONS";
const WCHAR wszDBSCHEMA_CATALOGS[]			=L"DBSCHEMA_CATALOGS";
const WCHAR wszDBSCHEMA_CHARACTER_SETS[]	=L"DBSCHEMA_CHARACTER_SETS";
const WCHAR wszDBSCHEMA_CHECK_CONSTRAINTS[]	=L"DBSCHEMA_CHECK_CONSTRAINTS";
const WCHAR wszDBSCHEMA_COLLATIONS[]		=L"DBSCHEMA_COLLATIONS";
const WCHAR wszDBSCHEMA_COLUMN_DOMAIN_USAGE[]=L"DBSCHEMA_COLUMN_DOMAIN_USAGE";
const WCHAR wszDBSCHEMA_COLUMN_PRIVILEGES[]	=L"DBSCHEMA_COLUMN_PRIVILEGES";
const WCHAR wszDBSCHEMA_COLUMNS[]			=L"DBSCHEMA_COLUMNS";
const WCHAR wszDBSCHEMA_CONSTRAINT_COLUMN_USAGE[]=L"DBSCHEMA_CONSTRAINT_COLUMN_USAGE";
const WCHAR wszDBSCHEMA_CONSTRAINT_TABLE_USAGE[]=L"DBSCHEMA_CONSTRAINT_TABLE_USAGE";
const WCHAR wszDBSCHEMA_FOREIGN_KEYS[]		=L"DBSCHEMA_FOREIGN_KEYS";
const WCHAR wszDBSCHEMA_INDEXES[]			=L"DBSCHEMA_INDEXES";
const WCHAR wszDBSCHEMA_KEY_COLUMN_USAGE[]	=L"DBSCHEMA_KEY_COLUMN_USAGE";
const WCHAR wszDBSCHEMA_PRIMARY_KEYS[]		=L"DBSCHEMA_PRIMARY_KEYS";
const WCHAR wszDBSCHEMA_PROCEDURE_COLUMNS[]	=L"DBSCHEMA_PROCEDURE_COLUMNS";
const WCHAR wszDBSCHEMA_PROCEDURE_PARAMETERS[]=L"DBSCHEMA_PROCEDURE_PARAMETERS";
const WCHAR wszDBSCHEMA_PROCEDURES[]		=L"DBSCHEMA_PROCEDURES";
const WCHAR wszDBSCHEMA_PROVIDER_TYPES[]	=L"DBSCHEMA_PROVIDER_TYPES";
const WCHAR wszDBSCHEMA_REFERENTIAL_CONSTRAINTS[]=L"DBSCHEMA_REFERENTIAL_CONSTRAINTS";
const WCHAR wszDBSCHEMA_SCHEMATA[]			=L"DBSCHEMA_SCHEMATA";
const WCHAR wszDBSCHEMA_SQL_LANGUAGES[]		=L"DBSCHEMA_SQL_LANGUAGES";
const WCHAR wszDBSCHEMA_STATISTICS[]		=L"DBSCHEMA_STATISTICS";
const WCHAR wszDBSCHEMA_TABLES[]			=L"DBSCHEMA_TABLES";
const WCHAR wszDBSCHEMA_TABLES_INFO[]		=L"DBSCHEMA_TABLES_INFO";
const WCHAR wszDBSCHEMA_TABLE_CONSTRAINTS[]	=L"DBSCHEMA_TABLE_CONSTRAINTS";
const WCHAR wszDBSCHEMA_TABLE_PRIVILEGES[]	=L"DBSCHEMA_TABLE_PRIVILEGES";
const WCHAR wszDBSCHEMA_TRANSLATIONS[]		=L"DBSCHEMA_TRANSLATIONS";
const WCHAR wszDBSCHEMA_TRUSTEE[]			=L"DBSCHEMA_TRUSTEE";
const WCHAR wszDBSCHEMA_USAGE_PRIVILEGES[]	=L"DBSCHEMA_USAGE_PRIVILEGES";
const WCHAR wszDBSCHEMA_VIEW_COLUMN_USAGE[]	=L"DBSCHEMA_VIEW_COLUMN_USAGE";
const WCHAR wszDBSCHEMA_VIEW_TABLE_USAGE[]	=L"DBSCHEMA_VIEW_TABLE_USAGE";
const WCHAR wszDBSCHEMA_VIEWS[]				=L"DBSCHEMA_VIEWS";
const WCHAR wszDBSCHEMA_GUID[]				=L"DBSCHEMA_GUID not recognized or not supported";
const WCHAR wszNOROWS[]						=L"No rows returned";

const WCHAR wszABORTPRESERVE[]				=L"ABORTPRESERVE";
const WCHAR wszACCESSORDER[]				=L"ACCESSORDER";
const WCHAR wszAPPENDONLY[]					=L"APPENDONLY";
const WCHAR wszBLOCKINGSTORAGEOBJECTS[]		=L"BLOCKINGSTORAGEOBJECTS";
const WCHAR wszBOOKMARKINFO[]				=L"BOOKMARKINFO";
const WCHAR wszBOOKMARKS[]					=L"BOOKMARKS";
const WCHAR wszBOOKMARKSKIPPED[]			=L"BOOKMARKSKIPPED";
const WCHAR wszBOOKMARKTYPE[]				=L"BOOKMARKTYPE";
const WCHAR wszCACHEDEFERRED[]				=L"CACHEDEFERRED";
const WCHAR wszCANFETCHBACKWARDS[]			=L"CANFETCHBACKWARDS";
const WCHAR wszCANHOLDROWS[]				=L"CANHOLDROWS";
const WCHAR wszCANRELEASELOCKS[]			=L"CANRELEASELOCKS";
const WCHAR wszCANSCROLLBACKWARDS[]			=L"CANSCROLLBACKWARDS";
const WCHAR wszCHANGEINSERTEDROWS[]			=L"CHANGEINSERTEDROWS";
const WCHAR wszCOLUMNRESTRICT[]				=L"COLUMNRESTRICT";
const WCHAR wszCOMMANDTIMEOUT[]				=L"COMMANDTIMEOUT";
const WCHAR wszCOMMITPRESERVE[]				=L"COMMITPRESERVE";
const WCHAR wszDEFERRED[]					=L"DEFERRED";
const WCHAR wszDELAYSTORAGEOBJECTS[]		=L"DELAYSTORAGEOBJECTS";
const WCHAR wszFILTERCOMPAREOPS[]			=L"FILTERCOMPAREOPS";
const WCHAR wszFINDCOMPAREOPS[]				=L"FINDCOMPAREOPS";
const WCHAR wszHIDDENCOLUMNS[]				=L"HIDDENCOLUMNS";
const WCHAR wszIMMOBILEROWS[]				=L"IMMOBILEROWS";
const WCHAR wszLITERALBOOKMARKS[]			=L"LITERALBOOKMARKS";
const WCHAR wszLITERALIDENTITY[]			=L"LITERALIDENTITY";
const WCHAR wszLOCKMODE[]					=L"LOCKMODE";
const WCHAR wszMAXOPENROWS[]				=L"MAXOPENROWS";
const WCHAR wszMAXPENDINGROWS[]				=L"MAXPENDINGROWS";
const WCHAR wszMAXROWS[]					=L"MAXROWS";
const WCHAR wszMAYWRITECOLUMN[]				=L"MAYWRITECOLUMN";
const WCHAR wszMEMORYUSAGE[]				=L"MEMORYUSAGE";
const WCHAR wszNOTIFICATIONGRANULARITY[]	=L"NOTIFICATIONGRANULARITY";
const WCHAR wszNOTIFICATIONPHASES[]			=L"NOTIFICATIONPHASES";
const WCHAR wszNOTIFYCOLUMNSET[]			=L"NOTIFYCOLUMNSET";
const WCHAR wszNOTIFYROWDELETE[]			=L"NOTIFYROWDELETE";
const WCHAR wszNOTIFYROWFIRSTCHANGE[]		=L"NOTIFYROWFIRSTCHANGE";
const WCHAR wszNOTIFYROWINSERT[]			=L"NOTIFYROWINSERT";
const WCHAR wszNOTIFYROWRESYNCH[]			=L"NOTIFYROWRESYNCH";
const WCHAR wszNOTIFYROWSETRELEASE[]		=L"NOTIFYROWSETRELEASE";
const WCHAR wszNOTIFYROWSETFETCHPOSITIONCHANGE[]=L"NOTIFYROWSETFETCHPOSITIONCHANGE";
const WCHAR wszNOTIFYROWUNDOCHANGE[]		=L"NOTIFYROWUNDOCHANGE";
const WCHAR wszNOTIFYROWUNDODELETE[]		=L"NOTIFYROWUNDODELETE";
const WCHAR wszNOTIFYROWUNDOINSERT[]		=L"NOTIFYROWUNDOINSERT";
const WCHAR wszNOTIFYROWUPDATE[]			=L"NOTIFYROWUPDATE";
const WCHAR wszORDEREDBOOKMARKS[]			=L"ORDEREDBOOKMARKS";
const WCHAR wszOTHERINSERT []				=L"OTHERINSERT";
const WCHAR wszOTHERUPDATEDELETE[]			=L"OTHERUPDATEDELETE";
const WCHAR wszOWNINSERT[]					=L"OWNINSERT";
const WCHAR wszOWNUPDATEDELETE[]			=L"OWNUPDATEDELETE";
const WCHAR wszPROPERTIESINERROR[]			=L"PROPERTIESINERROR";
const WCHAR wszQUICKRESTART[]				=L"QUICKRESTART";
const WCHAR wszREENTRANTEVENTS[]			=L"REENTRANTEVENTS";
const WCHAR wszREMOVEDELETED[]				=L"REMOVEDELETED";
const WCHAR wszREPORTMULTIPLECHANGES[]		=L"REPORTMULTIPLECHANGES";
const WCHAR wszRETURNPENDINGINSERTS[]		=L"RETURNPENDINGINSERTS";
const WCHAR wszROWRESTRICT[]				=L"ROWRESTRICT";
const WCHAR wszROWSET_ASYNCH[]				=L"ROWSET_ASYNCH";
const WCHAR wszROWTHREADMODEL[]				=L"ROWTHREADMODEL";
const WCHAR wszSERVERCURSOR[]				=L"SERVERCURSOR";
const WCHAR wszSERVERDATAONINSERT[]			=L"SERVERDATAONINSERT";
const WCHAR wszTRANSACTEDOBJECT[]			=L"TRANSACTEDOBJECT";
const WCHAR wszUPDATABILITY[]				=L"UPDATABILITY";
const WCHAR wszUNIQUEROWS[]					=L"UNIQUEROWS";
const WCHAR wszSTRONGIDENTITY[]				=L"STRONGIDENTITY";
const WCHAR wszIAccessor[]					=L"IAccessor";
const WCHAR wszIChapteredRowset[]			=L"IChapteredRowset";
const WCHAR wszIColumnsInfo[]				=L"IColumnsInfo";
const WCHAR wszIColumnsRowset[]				=L"IColumnsRowset";
const WCHAR wszIConnectionPointContainer[]	=L"IConnectionPointContainer";
const WCHAR wszIConvertType[]				=L"IConvertType";
const WCHAR wszIDBAsynchStatus[]			=L"IDBAsynchStatus";
const WCHAR wszILockBytes[]					=L"ILockBytes";
const WCHAR wszIMultipleResults[]			=L"IMultipleResults";
const WCHAR wszIParentRowset[]				=L"IParentRowset";
const WCHAR wszIRowset[]					=L"IRowset";
const WCHAR wszIRowsetChange[]				=L"IRowsetChange";
const WCHAR wszIRowsetCurrentIndex[]		=L"IRowsetCurrentIndex";
// const WCHAR wszIRowsetExactScroll[]			=L"IRowsetExactScroll";
const WCHAR wszIRowsetFind[]				=L"IRowsetFind";
const WCHAR wszIRowsetIdentity[]			=L"IRowsetIdentity";
const WCHAR wszIRowsetIndex[]				=L"IRowsetIndex";
const WCHAR wszIRowsetInfo[]				=L"IRowsetInfo";
const WCHAR wszIRowsetLocate[]				=L"IRowsetLocate";
const WCHAR wszIRowsetRefresh[]				=L"IRowsetRefresh";
const WCHAR wszIRowsetResynch[]				=L"IRowsetResynch";
const WCHAR wszIRowsetScroll[]				=L"IRowsetScroll";
const WCHAR wszIRowsetUpdate[]				=L"IRowsetUpdate";
const WCHAR wszIRowsetView[]				=L"IRowsetView";
const WCHAR wszISupportErrorInfo[]			=L"ISupportErrorInfo";
const WCHAR wszISequentialStream[]			=L"ISequentialStream";
const WCHAR wszIStorage[]					=L"IStorage";
const WCHAR wszIStream[]					=L"IStream";
const WCHAR wszROWSETPROP[]					=L"Rowset Property NOT recognized";
const WCHAR wszRESTRICTION[]				=L"Restriction not known, so can't test\n";
const WCHAR wszRESTRICTIONNOTSUPPORTED[]	=L"This schema/restriction is not supported\n";

// Alter table strings for Primary, Foriegnkeys.
const WCHAR g_wszAddPrimaryKey[]			=L"ALTER TABLE %s ADD CONSTRAINT pk%d%s PRIMARY KEY (%s)";
const WCHAR g_wszAddPrimaryKey2[]			=L"ALTER TABLE %s ADD CONSTRAINT pk%d%s PRIMARY KEY (%s, %s)";
const WCHAR g_wszAddForeignKey[]			=L"ALTER TABLE %s ADD CONSTRAINT fk%d%s FOREIGN KEY (%s) REFERENCES %s (%s)";
const WCHAR g_wszAddForeignKey2[]			=L"ALTER TABLE %s ADD CONSTRAINT fk%d%s FOREIGN KEY (%s, %s) REFERENCES %s (%s, %s)";
const WCHAR g_wszDropPrimaryKeyConstraint[]	=L"ALTER TABLE %s DROP CONSTRAINT pk%d%s";
const WCHAR g_wszDropForeignKeyConstraint[]	=L"ALTER TABLE %s DROP CONSTRAINT fk%d%s";

typedef DWORD RESTRICTIONS;

enum RESTRICTIONSENUM
{
	ZERO	= 0x0,
	FIRST	= 0x1,
	SECOND	= 0x2,
	THIRD	= 0x4,
	FOURTH	= 0x8,
	FIFTH	= 0x10,
	SIXTH	= 0x20,
	SEVENTH = 0x40,
	ALLRES	= 0x80

};

typedef DWORD SUPPORTEDRESTRICTIONS;

enum SUPPORTEDRESTRICTIONSENUM
{
	one =	0x1,
	two =	0x2,
	three = 0x4,
	four =	0x8,
	five =	0x10,
	six =	0x20,
	seven = 0x40

};

enum REQUESTED_SCHEMA
{
	SUPPORTED,		// only a supported schema
	SPECIFIC		// schema, supported or not, that I am requesting
};

struct SUP_RESTRICT
{
	GUID schema;
	ULONG bitmask;
};

#define TYPE_I1 signed char
#define TYPE_I2 SHORT
#define TYPE_I4 LONG
#define TYPE_I8 LARGE_INTEGER
#define TYPE_UI1 BYTE
#define TYPE_UI2 unsigned short
#define TYPE_UI4 unsigned int
#define TYPE_UI8 ULARGE_INTEGER
#define TYPE_R4 float
#define TYPE_R8 double
#define TYPE_CY LARGE_INTEGER
#define TYPE_DECIMAL DBDECIMAL
#define TYPE_NUMERIC DBNUMERIC
#define TYPE_DATE DATE
#define TYPE_BOOL VARIANT_BOOL
#define TYPE_BYTES BYTE *
#define TYPE_BSTR BSTR
#define TYPE_STR char *
#define TYPE_WSTR wchar_t *
#define TYPE_VARIANT VARIANT
#define TYPE_IDISPATCH Dispatch *
#define TYPE_IUNKNOWN IUnknown *
#define TYPE_GUID GUID
#define TYPE_ERROR SCODE
#define TYPE_BYREF void *
#define TYPE_ARRAY SAFEARRAY *
#define TYPE_VECTOR DBVECTOR
#define TYPE_DBDATE DBDATE
#define TYPE_DBTIME DBTIME
#define TYPE_DBTIMESTAMP DBTIMESTAMP


//-----------------------------------------------------------------------------
// ROWSET_PROPERTIES
//
// For now, I only want BOOL properties
//
//-----------------------------------------------------------------------------
/*#define cROWSET_PROPERTIES 30
const DBPROPID rgROWSET_PROPERTIES[cROWSET_PROPERTIES]=
{
	DBPROP_BLOCKINGSTORAGEOBJECTS,
	DBPROP_BOOKMARKS,
	DBPROP_BOOKMARKSKIPPED,
	DBPROP_CACHEDEFERRED,
	DBPROP_CANFETCHBACKWARDS,
	DBPROP_CANHOLDROWS,
	DBPROP_CANSCROLLBACKWARDS,
	DBPROP_COLUMNRESTRICT,
	DBPROP_COMMITPRESERVE,
	DBPROP_DEFERRED,
	DBPROP_DELAYSTORAGEOBJECTS,
	DBPROP_IMMOBILEROWS,
	DBPROP_LITERALBOOKMARKS,
	DBPROP_LITERALIDENTITY,
	DBPROP_MAYWRITECOLUMN,
	DBPROP_ORDEREDBOOKMARKS,
	DBPROP_OTHERINSERT,
	DBPROP_OTHERUPDATEDELETE,
	DBPROP_OWNINSERT,
	DBPROP_OWNUPDATEDELETE,
	DBPROP_QUICKRESTART,
	DBPROP_REENTRANTEVENTS,
	DBPROP_REMOVEDELETED,
	DBPROP_REPORTMULTIPLECHANGES,
	DBPROP_ROWRESTRICT,
	DBPROP_SERVERCURSOR,
	DBPROP_TRANSACTEDOBJECT,
	DBPROP_IAccessor,
	DBPROP_IColumnsInfo,
	DBPROP_IColumnsRowset
} ;

//-----------------------------------------------------------------------------
// COLUMN_PROPERTIES
//-----------------------------------------------------------------------------
#define cCOLUMN_PROPERTIES 6
const DBPROPID rgCOLUMN_PROPERTIES[cCOLUMN_PROPERTIES]=
{
	DBPROP_COL_DEFAULT,
	DBPROP_COL_DESCRIPTION,
	DBPROP_COL_NULLABLE,
	DBPROP_COL_UNIQUE,
	DBPROP_COL_PRIMARYKEY,
	DBPROP_COL_AUTOINCREMENT
} ;

//-----------------------------------------------------------------------------
// DATASOURCE_PROPERTIES
//-----------------------------------------------------------------------------
#define cDATASOURCE_PROPERTIES 1
const DBPROPID rgDATASOURCE_PROPERTIES[cDATASOURCE_PROPERTIES]=
{
	DBPROP_CURRENTCATALOG
} ;

//-----------------------------------------------------------------------------
// DBINIT_PROPERTIES
//-----------------------------------------------------------------------------
#define cDBINIT_PROPERTIES 17
const DBPROPID rgDBINIT_PROPERTIES[cDBINIT_PROPERTIES]=
{
	DBPROP_AUTH_CACHE_AUTHINFO,
	DBPROP_AUTH_ENCRYPT_PASSWORD,
	DBPROP_AUTH_INTEGRATED,
	DBPROP_AUTH_MASK_PASSWORD,
	DBPROP_AUTH_PASSWORD,
	DBPROP_AUTH_PERSIST_ENCRYPTED,
	DBPROP_AUTH_PERSIST_SENSITIVE_AUTHINFO,
	DBPROP_AUTH_USERID,
	//DBPROP_INIT_CONNECT,
	DBPROP_INIT_DATASOURCE,
	DBPROP_INIT_HWND,
	DBPROP_INIT_IMPERSONATION_LEVEL,
	DBPROP_INIT_LOCATION,
	DBPROP_INIT_MODE,
	DBPROP_INIT_PROMPT,
	DBPROP_INIT_PROTECTION_LEVEL,
	DBPROP_INIT_TIMEOUT
} ;

//-----------------------------------------------------------------------------
// ROWSETINDEX_PROPERTIES
//-----------------------------------------------------------------------------
#define cROWSETINDEX_PROPERTIES 10
const DBPROPID rgROWSETINDEX_PROPERTIES[cROWSETINDEX_PROPERTIES]=
{
	DBPROP_INDEX_PRIMARYKEY,
	DBPROP_INDEX_UNIQUE,
	DBPROP_INDEX_CLUSTERED,
	DBPROP_INDEX_TYPE,
	DBPROP_INDEX_FILLFACTOR,
	DBPROP_INDEX_INITIALSIZE,
	DBPROP_INDEX_NULLS,
	DBPROP_INDEX_SORTBOOKMARKS,
	DBPROP_INDEX_AUTOUPDATE,
	DBPROP_INDEX_NULLCOLLATION
};

//-----------------------------------------------------------------------------
// TABLE_PROPERTIES
//-----------------------------------------------------------------------------
#define cTABLE_PROPERTIES 1
const DBPROPID rgTABLE_PROPERTIES[cTABLE_PROPERTIES]=
{
	DBPROP_TBL_TEMPTABLE
};
*/

//-----------------------------------------------------------------------------
// MANDATORY SCHEMA
//-----------------------------------------------------------------------------
#define cMANDATORY_SCHEMA 3
#define cSCHEMA 31
//-----------------------------------------------------------------------------
// ROWSET RIIDs, want non-mandatory
//-----------------------------------------------------------------------------

#define cROWSET_RIID  18

const WCHAR	*	rgwszRIID[]={
				L"IID_IAccessor",
				L"IID_IColumnsInfo",
				L"IID_IColumnsRowset",
				L"IID_IConnectionPointContainer",
				L"IID_IConvertType",
				L"IID_IRowset",
				L"IID_IRowsetChange",
				L"IID_IRowsetIdentity",
				L"IID_IRowsetInfo",
				L"IID_IRowsetLocate",
				L"IID_IRowsetResynch",
				L"IID_IRowsetScroll",
				L"IID_IRowsetUpdate",
				L"IID_ISupportErrorInfo",
				L"IID_IChapteredRowset",
				L"IID_IDBAsynchStatus",
				L"IID_IRowsetFind",
				L"IID_IRowsetView"

};
const IID *	rgRowsetIID[]={
				&IID_IAccessor,
				&IID_IColumnsInfo,
				&IID_IColumnsRowset,
				&IID_IConnectionPointContainer,
				&IID_IConvertType,
				&IID_IRowset,
				&IID_IRowsetChange,
				&IID_IRowsetIdentity,
				&IID_IRowsetInfo,
				&IID_IRowsetLocate,
				&IID_IRowsetResynch,
				&IID_IRowsetScroll,
				&IID_IRowsetUpdate,
				&IID_ISupportErrorInfo,
				&IID_IChapteredRowset,
				&IID_IDBAsynchStatus,
				&IID_IRowsetFind,
				&IID_IRowsetView
};
const DBPROPID	rgRowsetDBPROP_IID[]={
				DBPROP_IAccessor,
				DBPROP_IColumnsInfo,
				DBPROP_IColumnsRowset,
				DBPROP_IConnectionPointContainer,
				DBPROP_IConvertType,
				DBPROP_IRowset,
				DBPROP_IRowsetChange,
				DBPROP_IRowsetIdentity,
				DBPROP_IRowsetInfo,
				DBPROP_IRowsetLocate,
				DBPROP_IRowsetResynch,
				DBPROP_IRowsetScroll,
				DBPROP_IRowsetUpdate,
				DBPROP_ISupportErrorInfo,
				DBPROP_IChapteredRowset,
				DBPROP_IDBAsynchStatus,
				DBPROP_IRowsetFind,
				DBPROP_IRowsetView
};

//-----------------------------------------------------------------------------
// DBTYPES
//-----------------------------------------------------------------------------
#define cDBTYPES 34
const DBTYPE 	rgDBTYPES[cDBTYPES]=
{
	DBTYPE_EMPTY,
	DBTYPE_NULL,
	DBTYPE_RESERVED,
	DBTYPE_I1,
	DBTYPE_I2,
	DBTYPE_I4,
	DBTYPE_I8,
	DBTYPE_UI1,
	DBTYPE_UI2,
	DBTYPE_UI4,
	DBTYPE_UI8,
	DBTYPE_R4,
	DBTYPE_R8,
	DBTYPE_CY,
	DBTYPE_NUMERIC,
	DBTYPE_DECIMAL,
	DBTYPE_DATE,
	DBTYPE_BOOL,
	DBTYPE_BYTES,
	DBTYPE_BSTR,
	DBTYPE_STR,
	DBTYPE_WSTR,
	DBTYPE_VARIANT,
	DBTYPE_IDISPATCH,
	DBTYPE_IUNKNOWN,
	DBTYPE_GUID,
	DBTYPE_ERROR,
	DBTYPE_BYREF,
	DBTYPE_ARRAY,
	DBTYPE_VECTOR,
	DBTYPE_UDT,
	DBTYPE_DBDATE,
	DBTYPE_DBTIME,
	DBTYPE_DBTIMESTAMP
};

//-----------------------------------------------------------------------------
// COLUMNS
//-----------------------------------------------------------------------------
#define cCOLUMNS 28
#define cCOLUMNS_RESTRICTIONS 4
const DBTYPE	rgtypeCOLUMNS[cCOLUMNS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_BOOL,
				DBTYPE_WSTR,
				DBTYPE_UI4,
				DBTYPE_BOOL,
				DBTYPE_UI2,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_UI2,
				DBTYPE_I2,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
const WCHAR	*	rgwszCOLUMNS[cCOLUMNS]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"ORDINAL_POSITION",
				L"COLUMN_HASDEFAULT",
				L"COLUMN_DEFAULT",
				L"COLUMN_FLAGS",
				L"IS_NULLABLE",
				L"DATA_TYPE",
				L"TYPE_GUID",
				L"CHARACTER_MAXIMUM_LENGTH",
				L"CHARACTER_OCTET_LENGTH",
				L"NUMERIC_PRECISION",
				L"NUMERIC_SCALE",
				L"DATETIME_PRECISION",
				L"CHARACTER_SET_CATALOG",
				L"CHARACTER_SET_SCHEMA",
				L"CHARACTER_SET_NAME",
				L"COLLATION_CATALOG",
				L"COLLATION_SCHEMA",
				L"COLLATION_NAME",
				L"DOMAIN_CATALOG",
				L"DOMAIN_SCHEMA",
				L"DOMAIN_NAME",
				L"DESCRIPTION"
};
//-----------------------------------------------------------------------------
// TABLES
//-----------------------------------------------------------------------------
#define cTABLES 9 
#define cTABLES_RESTRICTIONS 4
const WCHAR	*	rgwszTABLES[cTABLES]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"TABLE_TYPE",
				L"TABLE_GUID",
				L"DESCRIPTION",
				L"TABLE_PROPID",
				L"DATE_CREATED",
				L"DATE_MODIFIED"
};
const DBTYPE	rgtypeTABLES[cTABLES]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_WSTR,
				DBTYPE_UI4,
				DBTYPE_DATE,
				DBTYPE_DATE
};
//-----------------------------------------------------------------------------
// TYPES
//-----------------------------------------------------------------------------
#define cPROVIDER_TYPES 21
#define cPROVIDER_TYPES_RESTRICTIONS 2
const WCHAR	*	rgwszPROVIDER_TYPES[cPROVIDER_TYPES]={
				L"TYPE_NAME",
				L"DATA_TYPE",
				L"COLUMN_SIZE",
				L"LITERAL_PREFIX",
				L"LITERAL_SUFFIX",
				L"CREATE_PARAMS",
				L"IS_NULLABLE",
				L"CASE_SENSITIVE",
				L"SEARCHABLE",
				L"UNSIGNED_ATTRIBUTE",
				L"FIXED_PREC_SCALE",
				L"AUTO_UNIQUE_VALUE",
				L"LOCAL_TYPE_NAME",
				L"MINIMUM_SCALE",
				L"MAXIMUM_SCALE",
				L"GUID",
				L"TYPELIB",
				L"VERSION",
				L"IS_LONG",
				L"BEST_MATCH",
				L"IS_FIXEDLENGTH"
};
const DBTYPE	rgtypePROVIDER_TYPES[cPROVIDER_TYPES]={
				DBTYPE_WSTR,
				DBTYPE_UI2,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
 				DBTYPE_UI4,
 				DBTYPE_BOOL,
 				DBTYPE_BOOL,
				DBTYPE_BOOL,
  				DBTYPE_WSTR,
				DBTYPE_I2,
				DBTYPE_I2,
 				DBTYPE_GUID,
 				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_BOOL
};
//-----------------------------------------------------------------------------
// ASSERTIONS
//-----------------------------------------------------------------------------
#define cASSERTIONS 6
#define cASSERTIONS_RESTRICTIONS 3
const WCHAR	*	rgwszASSERTIONS[cASSERTIONS]= {
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME",
				L"IS_DEFERRABLE",
				L"INITIALLY_DEFERRED",
				L"DESCRIPTION"
};
const DBTYPE	rgtypeASSERTIONS[cASSERTIONS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// CHARACTER_SETS
//-----------------------------------------------------------------------------
#define cCHARACTER_SETS	8
#define cCHARACTER_SETS_RESTRICTIONS 3
const WCHAR	*	rgwszCHARACTER_SETS[cCHARACTER_SETS]={
				L"CHARACTER_SET_CATALOG",
				L"CHARACTER_SET_SCHEMA",
				L"CHARACTER_SET_NAME",
				L"FORM_OF_USE",
				L"NUMBER_OF_CHARACTERS",
				L"DEFAULT_COLLATE_CATALOG",
				L"DEFAULT_COLLATE_SCHEMA",
				L"DEFAULT_COLLATE_NAME"
};
const DBTYPE	rgtypeCHARACTER_SETS[cCHARACTER_SETS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_I8,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// CHECK_CONSTRAINTS
//-----------------------------------------------------------------------------
#define cCHECK_CONSTRAINTS 5
#define cCHECK_CONSTRAINTS_RESTRICTIONS 3
const WCHAR	*	rgwszCHECK_CONSTRAINTS[cCHECK_CONSTRAINTS]={
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME",
				L"CHECK_CLAUSE",
				L"DESCRIPTION"
};
const DBTYPE	rgtypeCHECK_CONSTRAINTS[cCHECK_CONSTRAINTS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// COLLATIONS 
//-----------------------------------------------------------------------------
#define cCOLLATIONS 7
#define cCOLLATIONS_RESTRICTIONS 3
const WCHAR	*	rgwszCOLLATIONS[cCOLLATIONS]={
				L"COLLATION_CATALOG",
				L"COLLATION_SCHEMA",
				L"COLLATION_NAME",
				L"CHARACTER_SET_CATALOG",
				L"CHARACTER_SET_SCHEMA",
				L"CHARACTER_SET_NAME",
				L"PAD_ATTRIBUTE"
};
const DBTYPE	rgtypeCOLLATIONS[cCOLLATIONS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// COLUMN_DOMAIN_USAGE
//-----------------------------------------------------------------------------
#define cCOLUMN_DOMAIN_USAGE 9
#define cCOLUMN_DOMAIN_USAGE_RESTRICTIONS 4
const WCHAR	*	rgwszCOLUMN_DOMAIN_USAGE[cCOLUMN_DOMAIN_USAGE]=	{
				L"DOMAIN_CATALOG",
				L"DOMAIN_SCHEMA",
				L"DOMAIN_NAME",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID"
};
const DBTYPE	rgtypeCOLUMN_DOMAIN_USAGE[cCOLUMN_DOMAIN_USAGE]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4
};
//-----------------------------------------------------------------------------
// COLUMN_PRIVILEGES
//-----------------------------------------------------------------------------
#define cCOLUMN_PRIVILEGES 10
#define cCOLUMN_PRIVILEGES_RESTRICTIONS 6
const WCHAR	*	rgwszCOLUMN_PRIVILEGES[cCOLUMN_PRIVILEGES]=	{
				L"GRANTOR",
				L"GRANTEE",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"PRIVILEGE_TYPE",
				L"IS_GRANTABLE"
};
const DBTYPE	rgtypeCOLUMN_PRIVILEGES[cCOLUMN_PRIVILEGES]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_BOOL
};
//-----------------------------------------------------------------------------
// CONSTRAINT_COLUMN_USAGE 
//-----------------------------------------------------------------------------
#define cCONSTRAINT_COLUMN_USAGE 9
#define cCONSTRAINT_COLUMN_USAGE_RESTRICTIONS 4
const WCHAR	*	rgwszCONSTRAINT_COLUMN_USAGE[cCONSTRAINT_COLUMN_USAGE]=	{
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME"
};
const DBTYPE	rgtypeCONSTRAINT_COLUMN_USAGE[cCONSTRAINT_COLUMN_USAGE]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// CONSTRAINT_TABLE_USAGE 
//-----------------------------------------------------------------------------
#define cCONSTRAINT_TABLE_USAGE 6
#define cCONSTRAINT_TABLE_USAGE_RESTRICTIONS 3
const WCHAR	*	rgwszCONSTRAINT_TABLE_USAGE[cCONSTRAINT_TABLE_USAGE]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME"
};
const DBTYPE	rgtypeCONSTRAINT_TABLE_USAGE[cCONSTRAINT_TABLE_USAGE]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// KEY_COLUMN_USAGE 
//-----------------------------------------------------------------------------
#define cKEY_COLUMN_USAGE 10 
#define cKEY_COLUMN_USAGE_RESTRICTIONS 7
const WCHAR	*	rgwszKEY_COLUMN_USAGE[cKEY_COLUMN_USAGE]={
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"ORDINAL_POSITION"
};
const DBTYPE	rgtypeKEY_COLUMN_USAGE[cKEY_COLUMN_USAGE]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4
};
//-----------------------------------------------------------------------------
// PROCEDURES 
//-----------------------------------------------------------------------------
#define cPROCEDURES 8 
#define cPROCEDURES_RESTRICTIONS 4
#define cPROCEDURE_TYPES 3

const WCHAR *	rgwszPROCEDURE_TYPES[cPROCEDURE_TYPES]={
				L"DB_PT_UNKNOWN",
				L"DB_PT_PROCEDURE",
				L"DB_PT_FUNCTION"
};

const WCHAR	*	rgwszPROCEDURES[cPROCEDURES]={
				L"PROCEDURE_CATALOG",
				L"PROCEDURE_SCHEMA",
				L"PROCEDURE_NAME",
				L"PROCEDURE_TYPE",
				L"PROCEDURE_DEFINITION",
				L"DESCRIPTION",
				L"DATE_CREATED",
				L"DATE_MODIFIED"
};
const DBTYPE	rgtypePROCEDURES[cPROCEDURES]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_I2,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_DATE,
				DBTYPE_DATE
};
//-----------------------------------------------------------------------------
// REFERENTIAL_CONSTRAINTS
//-----------------------------------------------------------------------------
#define cREFERENTIAL_CONSTRAINTS 10
#define cREFERENTIAL_CONSTRAINTS_RESTRICTIONS 3
const WCHAR	*	rgwszREFERENTIAL_CONSTRAINTS[cREFERENTIAL_CONSTRAINTS]={
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME",
				L"UNIQUE_CONSTRAINT_CATALOG",
				L"UNIQUE_CONSTRAINT_SCHEMA",
				L"UNIQUE_CONSTRAINT_NAME",
				L"MATCH_OPTION",
				L"UPDATE_RULE",
				L"DELETE_RULE",
				L"DESCRIPTION"
};
const DBTYPE	rgtypeREFERENTIAL_CONSTRAINTS[cREFERENTIAL_CONSTRAINTS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// SCHEMATA 
//-----------------------------------------------------------------------------
#define cSCHEMATA 6
#define cSCHEMATA_RESTRICTIONS 3
const WCHAR	*	rgwszSCHEMATA[cSCHEMATA]={
				L"CATALOG_NAME",
				L"SCHEMA_NAME",
				L"SCHEMA_OWNER",
				L"DEFAULT_CHARACTER_SET_CATALOG",
				L"DEFAULT_CHARACTER_SET_SCHEMA",
				L"DEFAULT_CHARACTER_SET_NAME"
};
const DBTYPE	rgtypeSCHEMATA[cSCHEMATA]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// SQL_LANGUAGES 
//-----------------------------------------------------------------------------
#define cSQL_LANGUAGES 7 
#define cSQL_LANGUAGES_RESTRICTIONS 0
const WCHAR	*	rgwszSQL_LANGUAGES[cSQL_LANGUAGES]={
				L"SQL_LANGUAGE_SOURCE",
				L"SQL_LANGUAGE_YEAR",
				L"SQL_LANGUAGE_CONFORMANCE",
				L"SQL_LANGUAGE_INTEGRITY",
				L"SQL_LANGUAGE_IMPLEMENTATION",
				L"SQL_LANGUAGE_BINDING_STYLE",
				L"SQL_LANGUAGE_PROGRAMMING_LANGUAGE"
};
const DBTYPE	rgtypeSQL_LANGUAGES[cSQL_LANGUAGES]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
};
//-----------------------------------------------------------------------------
// TABLES_INFO
//-----------------------------------------------------------------------------
#define cTABLES_INFO 14 
#define cTABLES_INFO_RESTRICTIONS 4
const WCHAR	*	rgwszTABLES_INFO[cTABLES_INFO]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"TABLE_TYPE",
				L"TABLE_GUID",
				L"BOOKMARKS",
				L"BOOKMARK_TYPE",
				L"BOOKMARK_DATATYPE",
				L"BOOKMARK_MAXIMUM_LENGTH",
				L"BOOKMARK_INFORMATION",
				L"TABLE_VERSION",
				L"CARDINALITY",
				L"DESCRIPTION",
				L"TABLE_PROPID",
};
const DBTYPE	rgtypeTABLES_INFO[cTABLES_INFO]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_BOOL,
				DBTYPE_I4,
				DBTYPE_UI2,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_I8,
				DBTYPE_UI8,
				DBTYPE_WSTR,
				DBTYPE_UI4
};
//-----------------------------------------------------------------------------
// TABLE_CONSTRAINTS 
//-----------------------------------------------------------------------------
#define cTABLE_CONSTRAINTS 10
#define cTABLE_CONSTRAINTS_RESTRICTIONS 7
const WCHAR	*	rgwszTABLE_CONSTRAINTS[cTABLE_CONSTRAINTS]={
				L"CONSTRAINT_CATALOG",
				L"CONSTRAINT_SCHEMA",
				L"CONSTRAINT_NAME",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"CONSTRAINT_TYPE",
				L"IS_DEFERRABLE",
				L"INITIALLY_DEFERRED",
				L"DESCRIPTION"
};
const DBTYPE	rgtypeTABLE_CONSTRAINTS[cTABLE_CONSTRAINTS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_WSTR

};
//-----------------------------------------------------------------------------
// TABLE_PRIVILEGES 
//-----------------------------------------------------------------------------
#define cTABLE_PRIVILEGES 7
#define cTABLE_PRIVILEGES_RESTRICTIONS 5
const WCHAR	*	rgwszTABLE_PRIVILEGES[cTABLE_PRIVILEGES]= {
				L"GRANTOR",
				L"GRANTEE",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"PRIVILEGE_TYPE",
				L"IS_GRANTABLE"
};
const DBTYPE	rgtypeTABLE_PRIVILEGES[cTABLE_PRIVILEGES]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL
};
//-----------------------------------------------------------------------------
// TRANSLATIONS 
//-----------------------------------------------------------------------------
#define cTRANSLATIONS 9
#define cTRANSLATIONS_RESTRICTIONS 3
const WCHAR	*	rgwszTRANSLATIONS[cTRANSLATIONS]={
				L"TRANSLATION_CATALOG",
				L"TRANSLATION_SCHEMA",
				L"TRANSLATION_NAME",
				L"SOURCE_CHARACTER_SET_CATALOG",
				L"SOURCE_CHARACTER_SET_SCHEMA",
				L"SOURCE_CHARACTER_SET_NAME",
				L"TARGET_CHARACTER_SET_CATALOG",
				L"TARGET_CHARACTER_SET_SCHEMA",
				L"TARGET_CHARACTER_SET_NAME"
};
const DBTYPE	rgtypeTRANSLATIONS[cTRANSLATIONS]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};

//-----------------------------------------------------------------------------
// TRUSTEE
//-----------------------------------------------------------------------------
#define cTRUSTEE 4
#define cTRUSTEE_RESTRICTIONS 4
const WCHAR	*	rgwszTRUSTEE[]={
				L"TRUSTEE_NAME",
				L"TRUSTEE_GUID",
				L"TRUSTEE_PROPID",
				L"TRUSTEE_TYPE"
};
const DBTYPE	rgtypeTRUSTEE[]= {
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4
};

//-----------------------------------------------------------------------------
// USAGE_PRIVILEGES 
//-----------------------------------------------------------------------------
#define cUSAGE_PRIVILEGES 8
#define cUSAGE_PRIVILEGES_RESTRICTIONS 6
#define cOBJECT_TYPES 4

const WCHAR *	rgwszOBJECT_TYPES[cOBJECT_TYPES]={
				L"DOMAIN",
				L"CHARACTER_SET",
				L"COLLATION",
				L"TRANSLATION"
};
const WCHAR	*	rgwszUSAGE_PRIVILEGES [cUSAGE_PRIVILEGES ]=	{
				L"GRANTOR",
				L"GRANTEE",
				L"OBJECT_CATALOG",
				L"OBJECT_SCHEMA",
				L"OBJECT_NAME",
				L"OBJECT_TYPE",
				L"PRIVILEGE_TYPE",
				L"IS_GRANTABLE"
};
const DBTYPE	rgtypeUSAGE_PRIVILEGES [cUSAGE_PRIVILEGES ]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL
};
//-----------------------------------------------------------------------------
// VIEW_TABLE_USAGE
//-----------------------------------------------------------------------------
#define cVIEW_TABLE_USAGE 6
#define cVIEW_TABLE_USAGE_RESTRICTIONS 3
const WCHAR	*	rgwszVIEW_TABLE_USAGE[cVIEW_TABLE_USAGE]={
				L"VIEW_CATALOG",
				L"VIEW_SCHEMA",
				L"VIEW_NAME",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME"
};
const DBTYPE	rgtypeVIEW_TABLE_USAGE[cVIEW_TABLE_USAGE]= {
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// VIEW_COLUMN_USAGE 
//-----------------------------------------------------------------------------
#define cVIEW_COLUMN_USAGE 9
#define cVIEW_COLUMN_USAGE_RESTRICTIONS 3
const WCHAR	*	rgwszVIEW_COLUMN_USAGE[cVIEW_COLUMN_USAGE]=	{
				L"VIEW_CATALOG",
				L"VIEW_SCHEMA",
				L"VIEW_NAME",
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID"
};
const DBTYPE	rgtypeVIEW_COLUMN_USAGE[cVIEW_COLUMN_USAGE]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4
};

//-----------------------------------------------------------------------------
// VIEWS 
//-----------------------------------------------------------------------------
#define cVIEWS 9
#define cVIEWS_RESTRICTIONS 3
const WCHAR	*	rgwszVIEWS[cVIEWS]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"VIEW_DEFINITION",
				L"CHECK_OPTION",
				L"IS_UPDATABLE",
				L"DESCRIPTION",
				L"DATE_CREATED",
				L"DATE_MODIFIED"
};
const DBTYPE	rgtypeVIEWS[cVIEWS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_WSTR,
				DBTYPE_DATE,
				DBTYPE_DATE
};

//-----------------------------------------------------------------------------
// CATALOGS
//-----------------------------------------------------------------------------
#define cCATALOGS 2
#define cCATALOGS_RESTRICTIONS 1
const WCHAR	*	rgwszCATALOGS[cCATALOGS]={
				L"CATALOG_NAME",
				L"DESCRIPTION",
};
const DBTYPE	rgtypeCATALOGS[cCATALOGS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
};
//-----------------------------------------------------------------------------
// INDEXES 
//-----------------------------------------------------------------------------
#define cINDEXES 25
#define cINDEXES_RESTRICTIONS 5
#define cINDEX_TYPES 4

const WCHAR *	rgwszINDEX_TYPES[cINDEX_TYPES]={
				L"DBPROP_IT_BTREE",
				L"DBPROP_IT_HASH",
				L"DBPROP_IT_CONTENT",
				L"DBPROP_IT_OTHER"
};
const WCHAR	*	rgwszINDEXES[cINDEXES]=	{
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"INDEX_CATALOG",
				L"INDEX_SCHEMA",
				L"INDEX_NAME",
				L"PRIMARY_KEY",
				L"UNIQUE",
				L"CLUSTERED",
				L"TYPE",
				L"FILL_FACTOR",
				L"INITIAL_SIZE",
				L"NULLS",
				L"SORT_BOOKMARKS",
				L"AUTO_UPDATE",
				L"NULL_COLLATION",
				L"ORDINAL_POSITION",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"COLLATION",
				L"CARDINALITY",
				L"PAGES",
				L"FILTER_CONDITION",
				L"INTEGRATED"
};
const DBTYPE	rgtypeINDEXES[cINDEXES]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_UI2,
				DBTYPE_I4,
				DBTYPE_I4,
				DBTYPE_I4,
				DBTYPE_BOOL,
				DBTYPE_BOOL,
				DBTYPE_I4,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_I2,
				DBTYPE_UI8,
				DBTYPE_I4,
				DBTYPE_WSTR,
				DBTYPE_BOOL
};

//-----------------------------------------------------------------------------
// STATISTICS 
//-----------------------------------------------------------------------------
#define cSTATISTICS	4
#define cSTATISTICS_RESTRICTIONS 3
const WCHAR	*	rgwszSTATISTICS[cSTATISTICS]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"CARDINALITY"
};
const DBTYPE	rgtypeSTATISTICS[cSTATISTICS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_UI8,
};
//-----------------------------------------------------------------------------
// PROCEDURE_PARAMETERS
//-----------------------------------------------------------------------------
#define cPROCEDURE_PARAMETERS 17
#define cPROCEDURE_PARAMETERS_RESTRICTIONS 4 
const WCHAR	*	rgwszPROCEDURE_PARAMETERS[cPROCEDURE_PARAMETERS]={
				L"PROCEDURE_CATALOG",
				L"PROCEDURE_SCHEMA",
				L"PROCEDURE_NAME",
				L"PARAMETER_NAME",
				L"ORDINAL_POSITION",
				L"PARAMETER_TYPE",
				L"PARAMETER_HASDEFAULT",
				L"PARAMETER_DEFAULT",
				L"IS_NULLABLE",
				L"DATA_TYPE",
				L"CHARACTER_MAXIMUM_LENGTH",
				L"CHARACTER_OCTET_LENGTH",
				L"NUMERIC_PRECISION",
				L"NUMERIC_SCALE",
				L"DESCRIPTION",
				L"TYPE_NAME",
				L"LOCAL_TYPE_NAME"
};
const DBTYPE	rgtypePROCEDURE_PARAMETERS[cPROCEDURE_PARAMETERS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_UI2,
				DBTYPE_UI2,
				DBTYPE_BOOL,
				DBTYPE_WSTR,
				DBTYPE_BOOL,
				DBTYPE_UI2,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_UI2,
				DBTYPE_I2,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// FOREIGN KEYS
//-----------------------------------------------------------------------------
#define cFOREIGN_KEYS 18
#define cFOREIGN_KEYS_RESTRICTIONS 6 
const WCHAR	*	rgwszFOREIGN_KEYS[cFOREIGN_KEYS]={
				L"PK_TABLE_CATALOG",
				L"PK_TABLE_SCHEMA",
				L"PK_TABLE_NAME",
				L"PK_COLUMN_NAME",
				L"PK_COLUMN_GUID",
				L"PK_COLUMN_PROPID",
				L"FK_TABLE_CATALOG",
				L"FK_TABLE_SCHEMA",
				L"FK_TABLE_NAME",
				L"FK_COLUMN_NAME",
				L"FK_COLUMN_GUID",
				L"FK_COLUMN_PROPID",
				L"ORDINAL",
				L"UPDATE_RULE",
				L"DELETE_RULE",
				L"PK_NAME",
				L"FK_NAME",
				L"DEFERRABILITY",
};
const DBTYPE	rgtypeFOREIGN_KEYS[cFOREIGN_KEYS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_I2
};
//-----------------------------------------------------------------------------
// PRIMARY KEYS
//-----------------------------------------------------------------------------
#define cPRIMARY_KEYS 8
#define cPRIMARY_KEYS_RESTRICTIONS 3 
const WCHAR	*	rgwszPRIMARY_KEYS[cPRIMARY_KEYS]={
				L"TABLE_CATALOG",
				L"TABLE_SCHEMA",
				L"TABLE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"ORDINAL",
				L"PK_NAME"
};
const DBTYPE	rgtypePRIMARY_KEYS[cPRIMARY_KEYS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_WSTR
};
//-----------------------------------------------------------------------------
// PROCEDURE_COLUMNS
//-----------------------------------------------------------------------------
#define cPROCEDURE_COLUMNS 16
#define cPROCEDURE_COLUMNS_RESTRICTIONS 4 
const WCHAR	*	rgwszPROCEDURE_COLUMNS[cPROCEDURE_COLUMNS]={
				L"PROCEDURE_CATALOG",
				L"PROCEDURE_SCHEMA",
				L"PROCEDURE_NAME",
				L"COLUMN_NAME",
				L"COLUMN_GUID",
				L"COLUMN_PROPID",
				L"ROWSET_NUMBER",
				L"ORDINAL_POSITION",
				L"IS_NULLABLE",
				L"DATA_TYPE",
				L"TYPE_GUID",
				L"CHARACTER_MAXIMUM_LENGTH",
				L"CHARACTER_OCTET_LENGTH",
				L"NUMERIC_PRECISION",
				L"NUMERIC_SCALE",
				L"DESCRIPTION"
};
const DBTYPE	rgtypePROCEDURE_COLUMNS[cPROCEDURE_COLUMNS]={
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_WSTR,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_BOOL,
				DBTYPE_UI2,
				DBTYPE_GUID,
				DBTYPE_UI4,
				DBTYPE_UI4,
				DBTYPE_UI2,
				DBTYPE_I2,
				DBTYPE_WSTR
};


//-----------------------------------------------------------------------------
// Updateable Properties
//-----------------------------------------------------------------------------
#define cUPDATE_PROPERTIES 6
const DBPROPID rgUpdateProperties[cUPDATE_PROPERTIES]=
{
	DBPROP_APPENDONLY,
	DBPROP_OTHERINSERT,
	DBPROP_OTHERUPDATEDELETE,
	DBPROP_REMOVEDELETED,
	DBPROP_IRowsetChange,
	DBPROP_IRowsetUpdate
};
const WCHAR * rgwszUpdateProperties[cUPDATE_PROPERTIES]=
{
	L"DBPROP_APPENDONLY",
	L"DBPROP_OTHERINSERT",
	L"DBPROP_OTHERUPDATEDELETE",
	L"DBPROP_REMOVEDELETED",
	L"DBPROP_IRowsetChange",
	L"DBPROP_IRowsetUpdate"
};

inline ULONG
GetLongRandomNumber()
{

	// We don't want a number more than 4 characters.
	LONG num = rand()%99999;

	return (num < 0)? (0-num):(num);

}


const GUID * g_prgSchemas[] = 
{
	&DBSCHEMA_ASSERTIONS,
	&DBSCHEMA_CATALOGS,
	&DBSCHEMA_CHARACTER_SETS,
	&DBSCHEMA_COLLATIONS,
	&DBSCHEMA_COLUMNS,
	&DBSCHEMA_CHECK_CONSTRAINTS,
	&DBSCHEMA_CONSTRAINT_COLUMN_USAGE,
	&DBSCHEMA_CONSTRAINT_TABLE_USAGE,
	&DBSCHEMA_KEY_COLUMN_USAGE,
	&DBSCHEMA_REFERENTIAL_CONSTRAINTS,
	&DBSCHEMA_TABLE_CONSTRAINTS,
	&DBSCHEMA_COLUMN_DOMAIN_USAGE,
	&DBSCHEMA_INDEXES,
	&DBSCHEMA_COLUMN_PRIVILEGES,
	&DBSCHEMA_TABLE_PRIVILEGES,
	&DBSCHEMA_USAGE_PRIVILEGES,
	&DBSCHEMA_PROCEDURES,
	&DBSCHEMA_SCHEMATA,
	&DBSCHEMA_SQL_LANGUAGES,
	&DBSCHEMA_STATISTICS,
	&DBSCHEMA_TABLES,
	&DBSCHEMA_TABLES_INFO,
	&DBSCHEMA_TRANSLATIONS,
	&DBSCHEMA_PROVIDER_TYPES,
	&DBSCHEMA_VIEWS,
	&DBSCHEMA_VIEW_COLUMN_USAGE,
	&DBSCHEMA_VIEW_TABLE_USAGE,
	&DBSCHEMA_PROCEDURE_PARAMETERS,
	&DBSCHEMA_FOREIGN_KEYS,
	&DBSCHEMA_PRIMARY_KEYS,
	&DBSCHEMA_PROCEDURE_COLUMNS
};


#endif 	//_IDBSCHMR_H_


