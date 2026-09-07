//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1998 Microsoft Corporation.
//
// @doc 
//
// @module IDBSCHMR.CPP | Source file for IDBSchemaRowset test
//
//--------------------------------------------------------------------

#include "modstandard.hpp"
#define	 DBINITCONSTANTS	// Must be defined to initialize constants in oledb.h
#define  INITGUID			// For IID_ITransactionOptions, etc.
// #define	 deprecated			// For deprecated properties returned by some providers
#include "IDBSchmR.h"

#define CLEANUP(x) if(x) goto CLEANUP;
#define TERMINATE(x) if(x) return FALSE;
#define CONTINUE(x) if (x) return TRUE;

#define	PI	(IUnknown *)
#define PPI (IUnknown **)
#define PPPI (IUnknown ***)
#define WC (WCHAR *)

/*
// Macros to prevent PRVTRACE from printing any trace stuff.
#undef PRVTRACE
#define PRVTRACE	if(0)Internal_Trace
#define RowsetBindingStatus if(0)RowsetBindingStatus
*/

#define INIT Init_Stuff();
#define FREE Free_Stuff();

// free other pointer
#define RESTRICTNOTSUPPORTED(x) if(!(m_currentBitMask & x))m_fAtLeast1UnsupportedRestrictionIsSet=TRUE;

IDBSchemaRowset * g_pIDBSchemaRowset=NULL;
ULONG cSchemas;
GUID * rgSchemas;
ULONG * rgRestrictions=NULL;

// Global strings for primary and foreign keys.
WCHAR *g_pwszAddPrimaryKeyOnTable1=NULL;
WCHAR *g_pwszAddPrimaryKeyOnTable2=NULL;
WCHAR *g_pwszAddForeignKeyOnTable1=NULL;
WCHAR *g_pwszDropPrimaryKeyConstraint1=NULL;
WCHAR *g_pwszDropForeignKeyConstraint1=NULL;
WCHAR *g_pwszDropPrimaryKeyConstraint2=NULL;
CTable	*g_pKeyTable1=NULL;
CTable  *g_pKeyTable2=NULL;
BOOL	g_fKeysOnTable=FALSE;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TraceSchemaName
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ULONG TraceSchemaName
(
	GUID guid,
	BOOL fToScreen,
	BOOL fAddNewLine
)
{
	LPWSTR pwszSchemaName = L"";

	// Find the schema
	if(IsEqualGUID(DBSCHEMA_ASSERTIONS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_ASSERTIONS;
	else if(IsEqualGUID(DBSCHEMA_CATALOGS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_CATALOGS;
	else if(IsEqualGUID(DBSCHEMA_CHARACTER_SETS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_CHARACTER_SETS;
	else if(IsEqualGUID(DBSCHEMA_CHECK_CONSTRAINTS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_CHECK_CONSTRAINTS;
	else if(IsEqualGUID(DBSCHEMA_COLLATIONS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_COLLATIONS;
	else if(IsEqualGUID(DBSCHEMA_COLUMN_DOMAIN_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_COLUMN_DOMAIN_USAGE;
	else if(IsEqualGUID(DBSCHEMA_COLUMN_PRIVILEGES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_COLUMN_PRIVILEGES;
	else if(IsEqualGUID(DBSCHEMA_COLUMNS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_COLUMNS;
	else if(IsEqualGUID(DBSCHEMA_CONSTRAINT_COLUMN_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_CONSTRAINT_COLUMN_USAGE;
	else if(IsEqualGUID(DBSCHEMA_CONSTRAINT_TABLE_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_CONSTRAINT_TABLE_USAGE;
	else if(IsEqualGUID(DBSCHEMA_FOREIGN_KEYS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_FOREIGN_KEYS;
	else if(IsEqualGUID(DBSCHEMA_INDEXES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_INDEXES;
	else if(IsEqualGUID(DBSCHEMA_KEY_COLUMN_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_KEY_COLUMN_USAGE;
	else if(IsEqualGUID(DBSCHEMA_PRIMARY_KEYS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_PRIMARY_KEYS;
	else if(IsEqualGUID(DBSCHEMA_PROCEDURE_COLUMNS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_PROCEDURE_COLUMNS;
	else if(IsEqualGUID(DBSCHEMA_PROCEDURE_PARAMETERS, guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_PROCEDURE_PARAMETERS;
	else if(IsEqualGUID(DBSCHEMA_PROCEDURES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_PROCEDURES;
	else if(IsEqualGUID(DBSCHEMA_PROVIDER_TYPES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_PROVIDER_TYPES;
	else if(IsEqualGUID(DBSCHEMA_REFERENTIAL_CONSTRAINTS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_REFERENTIAL_CONSTRAINTS;
	else if(IsEqualGUID(DBSCHEMA_SCHEMATA,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_SCHEMATA;
	else if(IsEqualGUID(DBSCHEMA_SQL_LANGUAGES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_SQL_LANGUAGES;
	else if(IsEqualGUID(DBSCHEMA_STATISTICS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_STATISTICS;
	else if(IsEqualGUID(DBSCHEMA_TABLES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TABLES;
	else if(IsEqualGUID(DBSCHEMA_TABLES_INFO,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TABLES_INFO;
	else if(IsEqualGUID(DBSCHEMA_TABLE_CONSTRAINTS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TABLE_CONSTRAINTS;
	else if(IsEqualGUID(DBSCHEMA_TABLE_PRIVILEGES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TABLE_PRIVILEGES;
	else if(IsEqualGUID(DBSCHEMA_TRANSLATIONS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TRANSLATIONS;
	else if(IsEqualGUID(DBSCHEMA_TRUSTEE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_TRUSTEE;
	else if(IsEqualGUID(DBSCHEMA_USAGE_PRIVILEGES,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_USAGE_PRIVILEGES;
	else if(IsEqualGUID(DBSCHEMA_VIEW_COLUMN_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_VIEW_COLUMN_USAGE;
	else if(IsEqualGUID(DBSCHEMA_VIEW_TABLE_USAGE,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_VIEW_TABLE_USAGE;
	else if(IsEqualGUID(DBSCHEMA_VIEWS,guid))
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_VIEWS;
	else
 		pwszSchemaName = (LPWSTR)wszDBSCHEMA_GUID;

	PRVTRACE(L"%s",pwszSchemaName);
	if(fAddNewLine)
		PRVTRACE(L"\n");

	if(fToScreen)
	{
		odtLog << pwszSchemaName;
		if(fAddNewLine)
			odtLog <<ENDL;
	}
	
	return 0;
}

void TracePropertyName
(
	DBPROPID prop,	
 	BOOL fToScreen,
	BOOL fAddNewLine
)
{
 	LPWSTR pwszProp = L"PROPERTY unknown";
	
	
	if(prop==DBPROP_ABORTPRESERVE)
		pwszProp = (LPWSTR)wszABORTPRESERVE;
	if(prop==DBPROP_ACCESSORDER)
		pwszProp = (LPWSTR)wszACCESSORDER;
	if(prop==DBPROP_APPENDONLY)
		pwszProp = (LPWSTR)wszAPPENDONLY;
	else if(prop==DBPROP_BLOCKINGSTORAGEOBJECTS)
		pwszProp = (LPWSTR)wszBLOCKINGSTORAGEOBJECTS;
	else if(prop==DBPROP_BOOKMARKINFO)
		pwszProp = (LPWSTR)wszBOOKMARKINFO;
	else if(prop==DBPROP_BOOKMARKS)
		pwszProp = (LPWSTR)wszBOOKMARKS;
	else if(prop==DBPROP_BOOKMARKSKIPPED)
		pwszProp = (LPWSTR)wszBOOKMARKSKIPPED;
	else if(prop==DBPROP_BOOKMARKTYPE)
		pwszProp = (LPWSTR)wszBOOKMARKTYPE;
	else if(prop==DBPROP_CACHEDEFERRED)
		pwszProp = (LPWSTR)wszCACHEDEFERRED;
	else if(prop==DBPROP_CANFETCHBACKWARDS)
		pwszProp = (LPWSTR)wszCANFETCHBACKWARDS;
	else if(prop==DBPROP_CANHOLDROWS)
		pwszProp = (LPWSTR)wszCANHOLDROWS;
	else if(prop==DBPROP_CANSCROLLBACKWARDS)
		pwszProp = (LPWSTR)wszCANSCROLLBACKWARDS;
	else if(prop==DBPROP_CHANGEINSERTEDROWS)
		pwszProp = (LPWSTR)wszCHANGEINSERTEDROWS;
	else if(prop==DBPROP_COLUMNRESTRICT)
		pwszProp = (LPWSTR)wszCOLUMNRESTRICT;
	else if(prop==DBPROP_COMMANDTIMEOUT)
		pwszProp = (LPWSTR)wszCOMMANDTIMEOUT;
	else if(prop==DBPROP_COMMITPRESERVE)
		pwszProp = (LPWSTR)wszCOMMITPRESERVE;
	else if(prop==DBPROP_DEFERRED)
		pwszProp = (LPWSTR)wszDEFERRED;
	else if(prop==DBPROP_DELAYSTORAGEOBJECTS)
		pwszProp = (LPWSTR)wszDELAYSTORAGEOBJECTS;
	else if(prop==DBPROP_FILTERCOMPAREOPS)
		pwszProp = (LPWSTR)wszFILTERCOMPAREOPS;
	else if(prop==DBPROP_FINDCOMPAREOPS)
		pwszProp = (LPWSTR)wszFINDCOMPAREOPS;
	else if(prop==DBPROP_HIDDENCOLUMNS)
		pwszProp = (LPWSTR)wszHIDDENCOLUMNS;
	else if(prop==DBPROP_IMMOBILEROWS)
		pwszProp = (LPWSTR)wszIMMOBILEROWS;
	else if(prop==DBPROP_LITERALBOOKMARKS)
		pwszProp = (LPWSTR)wszLITERALBOOKMARKS;
	else if(prop==DBPROP_LITERALIDENTITY)
		pwszProp = (LPWSTR)wszLITERALIDENTITY;
	else if(prop==DBPROP_LOCKMODE)
		pwszProp = (LPWSTR)wszLOCKMODE;
	else if(prop==DBPROP_MAXOPENROWS)
		pwszProp = (LPWSTR)wszMAXOPENROWS;
	else if(prop==DBPROP_MAXPENDINGROWS)
		pwszProp = (LPWSTR)wszMAXPENDINGROWS;
	else if(prop==DBPROP_MAXROWS)
		pwszProp = (LPWSTR)wszMAXROWS;
	else if(prop==DBPROP_MAYWRITECOLUMN)
		pwszProp = (LPWSTR)wszMAYWRITECOLUMN;
	else if(prop==DBPROP_MEMORYUSAGE)
		pwszProp = (LPWSTR)wszMEMORYUSAGE;
	else if(prop==DBPROP_NOTIFICATIONGRANULARITY)
		pwszProp = (LPWSTR)wszNOTIFICATIONGRANULARITY;
	else if(prop==DBPROP_NOTIFICATIONPHASES)
		pwszProp = (LPWSTR)wszNOTIFICATIONPHASES;
	else if(prop==DBPROP_NOTIFYCOLUMNSET)
		pwszProp = (LPWSTR)wszNOTIFYCOLUMNSET;
	else if(prop==DBPROP_NOTIFYROWDELETE)
		pwszProp = (LPWSTR)wszNOTIFYROWDELETE;
	else if(prop==DBPROP_NOTIFYROWFIRSTCHANGE)
		pwszProp = (LPWSTR)wszNOTIFYROWFIRSTCHANGE;
	else if(prop==DBPROP_NOTIFYROWINSERT)
		pwszProp = (LPWSTR)wszNOTIFYROWINSERT;
	else if(prop==DBPROP_NOTIFYROWRESYNCH)
		pwszProp = (LPWSTR)wszNOTIFYROWRESYNCH;
	else if(prop==DBPROP_NOTIFYROWSETRELEASE)
		pwszProp = (LPWSTR)wszNOTIFYROWSETRELEASE;
	else if(prop==DBPROP_NOTIFYROWSETFETCHPOSITIONCHANGE)
		pwszProp = (LPWSTR)wszNOTIFYROWSETFETCHPOSITIONCHANGE;
	else if(prop==DBPROP_NOTIFYROWSETFETCHPOSITIONCHANGE)
		pwszProp = (LPWSTR)wszNOTIFYROWSETFETCHPOSITIONCHANGE;
	else if(prop==DBPROP_NOTIFYROWUNDOCHANGE)
		pwszProp = (LPWSTR)wszNOTIFYROWUNDOCHANGE;
	else if(prop==DBPROP_NOTIFYROWUNDOCHANGE)
		pwszProp = (LPWSTR)wszNOTIFYROWUNDOCHANGE;
	else if(prop==DBPROP_NOTIFYROWUNDODELETE)
		pwszProp = (LPWSTR)wszNOTIFYROWUNDODELETE;
	else if(prop==DBPROP_NOTIFYROWUNDOINSERT)
		pwszProp = (LPWSTR)wszNOTIFYROWUNDOINSERT;
	else if(prop==DBPROP_NOTIFYROWUPDATE)
		pwszProp = (LPWSTR)wszNOTIFYROWUPDATE;
	else if(prop==DBPROP_ORDEREDBOOKMARKS)
		pwszProp = (LPWSTR)wszORDEREDBOOKMARKS;
	else if(prop==DBPROP_OTHERINSERT)
		pwszProp = (LPWSTR)wszOTHERINSERT;
	else if(prop==DBPROP_OTHERUPDATEDELETE)
		pwszProp = (LPWSTR)wszOTHERUPDATEDELETE;
	else if(prop==DBPROP_OWNINSERT)
		pwszProp = (LPWSTR)wszOWNINSERT;
	else if(prop==DBPROP_OWNUPDATEDELETE)
		pwszProp = (LPWSTR)wszOWNUPDATEDELETE;
	else if(prop==DBPROP_QUICKRESTART)
		pwszProp = (LPWSTR)wszQUICKRESTART;
	else if(prop==DBPROP_REENTRANTEVENTS)
		pwszProp = (LPWSTR)wszREENTRANTEVENTS;
	else if(prop==DBPROP_REMOVEDELETED)
		pwszProp = (LPWSTR)wszREMOVEDELETED;
	else if(prop==DBPROP_REPORTMULTIPLECHANGES)
		pwszProp = (LPWSTR)wszREPORTMULTIPLECHANGES;
	else if(prop==DBPROP_RETURNPENDINGINSERTS)
		pwszProp = (LPWSTR)wszRETURNPENDINGINSERTS;
	else if(prop==DBPROP_ROWRESTRICT)
		pwszProp = (LPWSTR)wszROWRESTRICT;
	else if(prop==DBPROP_ROWSET_ASYNCH)
		pwszProp = (LPWSTR)wszROWSET_ASYNCH;
	else if(prop==DBPROP_ROWTHREADMODEL)
		pwszProp = (LPWSTR)wszROWTHREADMODEL;
	else if(prop==DBPROP_SERVERCURSOR)
		pwszProp = (LPWSTR)wszSERVERCURSOR;
	else if(prop==DBPROP_SERVERDATAONINSERT)
		pwszProp = (LPWSTR)wszSERVERDATAONINSERT;
	else if(prop==DBPROP_TRANSACTEDOBJECT)
		pwszProp = (LPWSTR)wszTRANSACTEDOBJECT;
	else if(prop==DBPROP_UNIQUEROWS)
		pwszProp = (LPWSTR)wszUNIQUEROWS;
	else if(prop==DBPROP_UPDATABILITY)
		pwszProp = (LPWSTR)wszUPDATABILITY;
	else if(prop==DBPROP_STRONGIDENTITY)
		pwszProp = (LPWSTR)wszSTRONGIDENTITY;
	else if(prop==DBPROP_IAccessor)
		pwszProp = (LPWSTR)wszIAccessor;
	else if(prop==DBPROP_IChapteredRowset)
		pwszProp = (LPWSTR)wszIChapteredRowset;
	else if(prop==DBPROP_IColumnsInfo)
		pwszProp = (LPWSTR)wszIColumnsInfo;
	else if(prop==DBPROP_IColumnsRowset)
		pwszProp = (LPWSTR)wszIColumnsRowset;
	else if(prop==DBPROP_IConnectionPointContainer)
		pwszProp = (LPWSTR)wszIConnectionPointContainer;
	else if(prop==DBPROP_IConvertType)
		pwszProp = (LPWSTR)wszIConvertType;
	else if(prop==DBPROP_IDBAsynchStatus)
		pwszProp = (LPWSTR)wszIDBAsynchStatus;
	else if(prop==DBPROP_ILockBytes)
		pwszProp = (LPWSTR)wszILockBytes;
	else if(prop==DBPROP_IMultipleResults)
		pwszProp = (LPWSTR)wszIMultipleResults;
	else if(prop==DBPROP_IParentRowset)
		pwszProp = (LPWSTR)wszIParentRowset;
	else if(prop==DBPROP_IRowset)
		pwszProp = (LPWSTR)wszIRowset;
	else if(prop==DBPROP_IRowsetChange)
		pwszProp = (LPWSTR)wszIRowsetChange;
	else if(prop==DBPROP_IRowsetCurrentIndex)
		pwszProp = (LPWSTR)wszIRowsetCurrentIndex;
// IRowsetExactScroll is deprecated and shouldn't be returned by any providers.
//	else if(prop==DBPROP_IRowsetExactScroll)
//		pwszProp = (LPWSTR)wszIRowsetExactScroll;
	else if(prop==DBPROP_IRowsetFind)
		pwszProp = (LPWSTR)wszIRowsetFind;
	else if(prop==DBPROP_IRowsetIdentity)
		pwszProp = (LPWSTR)wszIRowsetIdentity;
	else if(prop==DBPROP_IRowsetInfo)
		pwszProp = (LPWSTR)wszIRowsetInfo;
	else if(prop==DBPROP_IRowsetIndex)
		pwszProp = (LPWSTR)wszIRowsetIndex;
	else if(prop==DBPROP_IRowsetInfo)
		pwszProp = (LPWSTR)wszIRowsetInfo;
	else if(prop==DBPROP_IRowsetLocate)
		pwszProp = (LPWSTR)wszIRowsetLocate;
	else if(prop==DBPROP_IRowsetRefresh)
		pwszProp = (LPWSTR)wszIRowsetRefresh;
	else if(prop==DBPROP_IRowsetResynch)
		pwszProp = (LPWSTR)wszIRowsetResynch;
	else if(prop==DBPROP_IRowsetScroll)
		pwszProp = (LPWSTR)wszIRowsetScroll;
	else if(prop==DBPROP_IRowsetUpdate)
		pwszProp = (LPWSTR)wszIRowsetUpdate;
	else if(prop==DBPROP_IRowsetView)
		pwszProp = (LPWSTR)wszIRowsetView;
	else if(prop==DBPROP_ISupportErrorInfo)
		pwszProp = (LPWSTR)wszISupportErrorInfo;
	else if(prop==DBPROP_ISequentialStream)
		pwszProp = (LPWSTR)wszISequentialStream;
	else if(prop==DBPROP_IStorage)
		pwszProp = (LPWSTR)wszIStorage;
	else if(prop==DBPROP_IStream)
		pwszProp = (LPWSTR)wszIStream;
	else if(prop==DBPROP_ISupportErrorInfo)
		pwszProp = (LPWSTR)wszISupportErrorInfo;

//	ASSERT(wcscmp(pwszProp, L"PROPERTY unknown"));

 	PRVTRACE(L"%s",pwszProp);
	if(fToScreen)
		odtLog << L"\t" << pwszProp;

	if(fAddNewLine)
	{
		PRVTRACE(L"\n");
		if(fToScreen)
			odtLog << L"\n";
	}

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GetRestrictionsFromSchema
//
// This returns the bitmask for the schema restrictions
// based on the schema.
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL TraceRestrictions
(
	ULONG * restrictions,
	ULONG schemaIndex, 
	ULONG * bitmask,
	BOOL fOutLog
)
{
	ULONG mask=0;

	mask = restrictions[schemaIndex];


	if(mask!=0)
	{
		if((mask) & FIRST)
		{
			PRVTRACE(L"1 ");
			if(fOutLog)
				odtLog << L"1 ";
		}
		if((mask) & SECOND)
		{
			PRVTRACE(L"2 ");
			if(fOutLog)
				odtLog << L"2 ";
		}
		if((mask) & THIRD)
		{
			PRVTRACE(L"3 ");
			if(fOutLog)
				odtLog << L"3 ";
		}
		if((mask) & FOURTH)
		{
			PRVTRACE(L"4 ");
			if(fOutLog)
				odtLog << L"4 ";
		}
		if((mask) & FIFTH)
		{
			PRVTRACE(L"5 ");
			if(fOutLog)
				odtLog << L"5 ";
		}
		if((mask) & SIXTH)
		{
			PRVTRACE(L"6 ");
			if(fOutLog)
				odtLog << L"6 ";
		}
		if((mask) & SEVENTH)
		{
			PRVTRACE(L"7 ");
			if(fOutLog)
				odtLog << L"7 ";
		}


		PRVTRACE(L"\n");
		if(fOutLog)
			odtLog << ENDL;

		if(bitmask)
			*bitmask = mask;

		return TRUE;
	}
	return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Base Class Section
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CSchemaTest : public CSessionObject
{
	public:

		// Static variables.  (Set once but wont change values).
		// @todo change them to real static variable.

		ULONG		m_cRowsetPropSet;
		DBPROPSET *	m_rgRowsetPropSet;

		BOOL m_fAtLeast1UnsupportedRestrictionIsSet;

		BOOL m_fPrintedSchemasAndRestrictions;

		BOOL m_PrintSchemaName;

		// Capture restrictions for this provider
		BOOL m_fDontCaptureRestrictions;

		// @cmember Pass all restrictions as init'd variants
		BOOL m_fRestrictionsAsInitVariants;

		// @cmember Do I want to pass in restriction when I know restriction is not supported?
		BOOL m_fPassUnsupportedRestrictions;

		// @cmember Global hresult
		HRESULT m_HR;

		// @cmember Variation level return
		BOOL m_fResult;

		// @cmember Was a bookmark requested for rowset
		BOOL m_fBOOKMARK_REQUESTED;

		// @cmember Was a bookmark found on rowset
		BOOL m_fBOOKMARK_FOUND;

		// @cmember Use first restriction
		BOOL m_fRes1;

		// @cmember Use second restriciton
		BOOL m_fRes2;

		// @cmember Use third restriciton
		BOOL m_fRes3;

		// @cmember Use fourth restriciton
		BOOL m_fRes4;

		// @cmember Use FIFTH restriction
		BOOL m_fRes5;

		// @cmember use sixth restriciton
		BOOL m_fRes6;

		// @cmember use seventh restriciton
		BOOL m_fRes7;

		// @cmember If catalog is found
		BOOL m_fMyTableCatalogFound;

		// @cmember If Schema is found
		BOOL m_fMyTableSchemaFound;

		// @cmember If first table name is found
		BOOL m_MyTableName1Found;

		// @cmember If second table name is found
		BOOL m_MyTableName2Found;

		// @cmember If column is found
		BOOL m_MyColumnNameFound;

		// @cmember If data type is found
		BOOL m_MyTableTypeFound;

		// @cmember Count of restricitons used 
		ULONG	m_cErrors;

		// @cmember Expect count of column in schema
		ULONG	m_cColumns;				

		// @cmember Expected count of restrictions in schema
		ULONG	m_cRestrictions;	

		// @cmember Count of restrictions being tested
 		ULONG	m_cRestrictionsCurrent;	

		// @cmember Count of schemas provider supports
		ULONG	m_cSchemasSupported;	

		// @cmember Count of Bindings
		ULONG	m_cDBBINDING;			

		// @cmember Count of Property Sets
		ULONG	m_cDBPROPSET;
		
		// @cmember Count of DBPROPINFOSETs
		ULONG	m_cDBPROPINFOSET;

		// @cmember Count of rowset DBPROPINFOSETs, including provider specific
		ULONG	m_cRowsetDBPROPINFOSET;

		// @cmember Count of columns in rowset
		ULONG	m_cDBCOLUMNINFO;		

		// @cmember Current schema being tested
		GUID	m_guid;					

		// @cmember Interface pointer I want returned
		IID		m_iid;					

		// Bitmask of which restrictions are requested
		RESTRICTIONS	m_restrict;
	
		// Array of FIRST property
		DBPROP	m_rgDBPROP[MAXPROP];

		// @cmember Array of Property Sets
		DBPROPSET m_rgDBPROPSET[MAXPROP];

		// @cmember CCol object
		CCol m_col;
	
		// @cmember First restriction
		WCHAR * m_wszR1;

		// @cmember Second restriction
		WCHAR * m_wszR2;

		// @cmember Third restriction
		WCHAR * m_wszR3;

		// @cmember Fourth restriction
		WCHAR * m_wszR4;

		// @cmember Fifth restriction
		WCHAR * m_wszR5;

		// @cmember Sixth restriction
		WCHAR * m_wszR6;

		// @cmember Seventh restriciton
		WCHAR * m_wszR7;

		// @cmember Non-string restriction (Currently only for Provider Type)
		ULONG	m_ulR;

		// @cmember Bool restriction (Currently only for Best Match in Provider Types)
		TYPE_BOOL m_fR;
		
		// @cmember Rowset pointer should be NULL
		BOOL m_fRowsetPointerNULL;

		// @cmember Count of Restrictions should be 0
		BOOL m_fCountRestrictionsNULL;

		// @cmember Range of Restrictions shoulbd be NULL
		BOOL m_fRangeRestrictionsNULL;

		// @cmember Count of Property Sets should be 0
		BOOL m_fCountPropSetNULL;

		// @cmember Range of Property Sets should be NULL
		BOOL m_fRangePropSetNULL;

		// Array of restrictions, according to spec, no schema has more than 7
  		VARIANT	m_rgvarRestrict[MAXRESTRICTION];	

		// Do I want to copy the restriction strings into my class members
		BOOL m_fCaptureRestrictions;

		// Print Supported Schemas prior to Testing
		BOOL m_fPrintSupportedSchemas;

		// DBTYPE
		DBTYPE m_DataTypeRestriction;

		// Object Type
		SHORT m_ProcedureTypeRestriction;

		// BEST MATCH
		TYPE_BOOL m_BestMatchRestriction;

		// Structure of guid and restrictions on guid
		ULONG  m_currentBitMask;

		// if backend is sql server
		BOOL	m_bSqlServer;


		///////////////
		// Dynamic
		//////////////

		// @cmember Supported Property sets
		DBPROP *	m_rgSupportedProperties;

		// @cmember Count of supported property sets
		ULONG		m_cSupportedProperties;

		// @cmember Range of Restrictions that are supported
		ULONG * m_rgRestrictionSupport;

		// @cmember Expected column names
		WCHAR ** m_rgColumnNames;			

		// @cmember Expected column types
 		DBTYPE * m_rgColumnTypes;			

		// @cmember Array of schemas provider supports
		GUID *	m_rgSchemas;			

		// @cmember Interface pointer
		IDBSchemaRowset *	m_pIDBSchemaRowset;		

		// @cmember Rowset pointer
		IRowset *	m_pIRowset;				

		// @cmember Count of DBPROPINFOSETs
		DBPROPINFOSET *	m_rgDBPROPINFOSET;

		// @cmember Rowset property sets, including provider specific
		DBPROPINFOSET *	m_rgRowsetDBPROPINFOSET;

		// @cmember Description Buffer
		WCHAR *	m_pDescBuffer;

		// @cmember Array of column information in rowset
		DBCOLUMNINFO *	m_rgDBCOLUMNINFO;		

		// @cmember Buffer of strings for column information 
		WCHAR *	m_pStringsBuffer;		

		// @cmember Array of Bindings
		DBBINDING *	m_rgDBBINDING;
		
		// @cmember Aggregation Pointer
		IUnknown * m_punkOuter;

		// @cmember IAccessor pointer
		IAccessor * m_pIAccessor;

		// @cmember IRowsetInfo pointer
		IRowsetInfo * m_pIRowsetInfo;

		// @cmember IColumnsInfo pointer
		IColumnsInfo * m_pIColumnsInfo;

		// @cmember IRowsetChange
		IRowsetChange * m_pIRowsetChange;

		// @cmember IUnknown
		IUnknown * m_pIUnknown;

		// @cmember m_pwszDropProc
		WCHAR * m_pwszDropProc;

		// @cmember m_eRowCount - how to interpret row count below
		ROW_COUNT m_eRowCount;

		// @cmember m_lRowCount - count of rows to expect from schema rowset
		ULONG m_ulRowCount;

		// @cmember Restrictions
		// If I can't find pubs for sql server or nwind for access, 
		// I'll take first catalog I find.
		WCHAR * m_pwszCatalogRestriction;
		WCHAR * m_pwszSchemaRestriction;
		WCHAR * m_pwszTableRestriction;
		WCHAR * m_pwszColumnRestriction;

		WCHAR * m_pwszAssertion_ConstraintRestriction;
		WCHAR * m_pwszCheck_ConstraintRestriction;
		WCHAR * m_pwszKey_Column_Usage_ConstraintRestriction;
		WCHAR * m_pwszReferential_ConstraintRestriction;
		WCHAR * m_pwszTable_ConstraintRestriction;
		
		WCHAR * m_pwszCharacter_SetRestriction;
		WCHAR * m_pwszCollationRestriction;
		WCHAR * m_pwszDomainRestriction;
		WCHAR * m_pwszGrantorRestriction;
		WCHAR * m_pwszGranteeRestriction;
		WCHAR * m_pwszPK_TableRestriction;
		WCHAR * m_pwszFK_TableRestriction;
		WCHAR * m_pwszIndexRestriction;
		WCHAR * m_pwszProcedureRestriction;
		WCHAR * m_pwszProcedureColumnsRestriction;
		WCHAR * m_pwszParameterRestriction;
		TYPE_UI2 m_uiProcedureType;
		WCHAR * m_pwszSchema_OwnerRestriction;
		WCHAR * m_pwszTable_TypeRestriction;
		WCHAR * m_pwszConstraint_TypeRestriction;
		WCHAR * m_pwszTranslationReplace;
		WCHAR * m_pwszObjectRestriction;
		WCHAR * m_pwszObject_TypeRestriction;
		WCHAR * m_pwszViewRestriction;

		//--------------------
		//  Functions
		//--------------------

		// @cmember Constructor
		CSchemaTest(const LPWSTR wszTestCaseName): CSessionObject(wszTestCaseName)
		{
			INIT

			m_bSqlServer = FALSE;
			m_rgDBPROPINFOSET=NULL;
			m_pDescBuffer=NULL;
			m_cDBPROPINFOSET=0;
			m_rgSupportedProperties=NULL;
			m_cSupportedProperties=0;
			m_fAtLeast1UnsupportedRestrictionIsSet=FALSE;


			m_fPrintedSchemasAndRestrictions=FALSE;
			m_currentBitMask = 0;
			m_fDontCaptureRestrictions = FALSE;

			m_rgColumnNames=NULL;			
			m_rgColumnTypes=NULL;			
			m_rgSchemas=NULL;			
			m_pIDBSchemaRowset=NULL;		
			m_pIRowset=NULL;				
			m_pDescBuffer=NULL;
			m_rgDBCOLUMNINFO=NULL;		
			m_pStringsBuffer=NULL;		
			m_rgDBBINDING=NULL;
			m_punkOuter=NULL;
			m_pIAccessor=NULL;
			m_pIRowsetInfo=NULL;
			m_pIColumnsInfo=NULL;
			m_pIRowsetChange=NULL;
			m_pIUnknown = NULL;
			m_PrintSchemaName=TRUE;

			m_pwszCatalogRestriction=NULL;
			m_pwszSchemaRestriction=NULL;
			m_pwszTableRestriction=NULL;
			m_pwszColumnRestriction=NULL;
			m_pwszAssertion_ConstraintRestriction=NULL;
			m_pwszCheck_ConstraintRestriction=NULL;
			m_pwszKey_Column_Usage_ConstraintRestriction=NULL;
			m_pwszReferential_ConstraintRestriction=NULL;
			m_pwszTable_ConstraintRestriction=NULL;
			m_pwszCharacter_SetRestriction=NULL;
			m_pwszCollationRestriction=NULL;
			m_pwszDomainRestriction=NULL;
			m_pwszGrantorRestriction=NULL;
			m_pwszGranteeRestriction=NULL;
			m_pwszPK_TableRestriction=NULL;
			m_pwszFK_TableRestriction=NULL;
			m_pwszIndexRestriction=NULL;
			m_pwszProcedureRestriction=NULL;
			m_pwszParameterRestriction=NULL;
			m_pwszProcedureColumnsRestriction=NULL;

			m_uiProcedureType=0;
			m_pwszSchema_OwnerRestriction=NULL;
			m_pwszConstraint_TypeRestriction=NULL;
			m_pwszTable_TypeRestriction=NULL;
			m_pwszTranslationReplace=NULL;
			m_pwszObjectRestriction=NULL;
			m_pwszObject_TypeRestriction=NULL;
			m_pwszViewRestriction=NULL;
			m_rgRestrictionSupport=NULL;

			m_rgRestrictionSupport=NULL;
			m_pwszProcedureColumnsRestriction=NULL;

			m_fPrintSupportedSchemas=TRUE;
			m_fBOOKMARK_REQUESTED=FALSE;
			m_fBOOKMARK_FOUND=FALSE;
			m_fRes1=TRUE;
			m_fRes2=TRUE;
			m_fRes3=TRUE;
			m_fRes4=TRUE;
			m_fRes5=TRUE;
			m_fRes6=TRUE;
			m_fRes7=TRUE;
			m_fMyTableCatalogFound=FALSE;
			m_fMyTableSchemaFound=FALSE;
			m_MyTableName1Found=FALSE;
			m_MyTableName2Found=FALSE;
			m_MyColumnNameFound=FALSE;
			m_MyTableTypeFound=FALSE;
			m_cErrors=0;
			m_cColumns=0;				
			m_cRestrictions=0;	
			m_cRestrictionsCurrent=0;	
			m_cSchemasSupported=0;	
			m_cDBBINDING=0;			
			m_cDBPROPSET=0;
			m_cDBCOLUMNINFO=0;		
			m_iid=IID_NULL;					
			m_guid=GUID_NULL;					
			m_restrict=0;
			m_HR=E_FAIL;
			m_fResult=FALSE;
			m_fCaptureRestrictions=FALSE;
			m_DataTypeRestriction=0;
			m_ProcedureTypeRestriction=0;


			m_fRowsetPointerNULL=FALSE;
			m_fCountRestrictionsNULL=FALSE;
			m_fRangeRestrictionsNULL=FALSE;
			m_fCountPropSetNULL=FALSE;
			m_fRangePropSetNULL=FALSE;
			m_fPassUnsupportedRestrictions=FALSE;
			m_fRestrictionsAsInitVariants=FALSE;
			
			
			m_wszR1=NULL;
			m_wszR2=NULL;
			m_wszR3=NULL;
			m_wszR4=NULL;
			m_wszR5=NULL;
			m_wszR6=NULL;
			m_wszR7=NULL;
			m_ulR=0;
			m_fR = FALSE;

			m_cRowsetDBPROPINFOSET = 0;
			m_rgRowsetDBPROPINFOSET = NULL;
			m_cRowsetPropSet=0;
			m_rgRowsetPropSet=NULL;
			m_eRowCount = MIN_VALUE;
			m_ulRowCount = 1;
			m_pwszTableName = NULL;
			m_prgColInfo = NULL;
			m_cColInfo = 0;
			m_pwszStringsBuffer = NULL;
			m_fPrimaryKey=FALSE;
			m_fForeignKey=FALSE;

  			for(ULONG index=0;index<MAXRESTRICTION;index++)
				VariantInit(&(m_rgvarRestrict[index]));
		};

		// @cmember Destructor
		virtual ~CSchemaTest()
		{
			FREE

			ASSERT(!m_rgColumnNames);			
			ASSERT(!m_rgColumnTypes);			
			ASSERT(!m_rgSchemas);			
			ASSERT(!m_pIDBSchemaRowset);		
			ASSERT(!m_pIRowset);				
			ASSERT(!m_rgDBCOLUMNINFO);		
			ASSERT(!m_pStringsBuffer);		
			ASSERT(!m_rgDBBINDING);	
			ASSERT(!m_punkOuter);
			ASSERT(!m_pIAccessor);
			ASSERT(!m_pIRowsetInfo);
			ASSERT(!m_pIColumnsInfo);
			ASSERT(!m_pIRowsetChange);
			ASSERT(!m_pIUnknown);
			ASSERT(!m_pwszAssertion_ConstraintRestriction);
			ASSERT(!m_pwszCheck_ConstraintRestriction);
			ASSERT(!m_pwszKey_Column_Usage_ConstraintRestriction);
			ASSERT(!m_pwszReferential_ConstraintRestriction);
			ASSERT(!m_pwszTable_ConstraintRestriction);
			ASSERT(!m_pwszCharacter_SetRestriction);
			ASSERT(!m_pwszCollationRestriction);
			ASSERT(!m_pwszDomainRestriction);
			ASSERT(!m_pwszGrantorRestriction);
			ASSERT(!m_pwszGranteeRestriction);
			ASSERT(!m_pwszPK_TableRestriction);
			ASSERT(!m_pwszFK_TableRestriction);
			ASSERT(!m_pwszProcedureRestriction);
			ASSERT(!m_pwszParameterRestriction);
			ASSERT(!m_pwszProcedureColumnsRestriction);
			ASSERT(!m_pwszSchema_OwnerRestriction);
			ASSERT(!m_pwszConstraint_TypeRestriction);
			ASSERT(!m_pwszTable_TypeRestriction);
			ASSERT(!m_pwszTranslationReplace);
			ASSERT(!m_pwszObjectRestriction);
			ASSERT(!m_pwszObject_TypeRestriction);
			ASSERT(!m_pwszViewRestriction);
			ASSERT(!m_rgRestrictionSupport);
			ASSERT(!m_rgRestrictionSupport);
			ASSERT(!m_pwszProcedureColumnsRestriction);

			for(ULONG index=0;index<MAXRESTRICTION;index++)
				VariantClear(&(m_rgvarRestrict[index]));
  			for(index=0;index<MAXPROP;index++)
				VariantClear(&(m_rgDBPROP[index].vValue));

			SAFE_FREE(m_pwszTableName);
			SAFE_FREE(m_prgColInfo);
			SAFE_FREE(m_pwszStringsBuffer);
		};

		// @cmember Init
		BOOL Init(void);

		// @cmember Terminate
		BOOL Terminate(void);

 		// Check Results of Method
		BOOL CheckResults(IUnknown * pIRowset, IID iid);

		// Check Riid can be used
		BOOL CheckRIID(	IUnknown * pColRowset,IID iid);

		// Verify Metadata of schema
		BOOL CheckAgainstIColumnsInfo(void);

		// GetRow
		HRESULT GetRow(void);

		// Set any interesting params and execute test method
		HRESULT GetRowset(BOOL fFreeRowset=TRUE);

		// Free Variation stuff
		void Free_Stuff(void);

		// Init Variation Stuff
		void Init_Stuff(void);
 
		// Get Next Schema Information, includes cRestrictions and rgRestrictions
		BOOL GetSchemaInfo(
			REQUESTED_SCHEMA schemaType,			// [IN] if I want the schema supported, or not, or I have a specific schema in mind
			ULONG ulIndexOfSchemaRequesting,		// [IN] Index of Schema
			GUID schema=GUID_NULL					// [IN] index of schema (in header file)
		);

		// Verify Rowset for column names, column data types, and restrictions met.
		// All TRACEable data will be put to debug window.
		BOOL VerifyRowset(IUnknown * pIUnknown = NULL);
			
		// @cmember VerifyRow, verifies restricitons only.
		BOOL VerifyRow(
			GUID			m_guid,
			ULONG			iRow,
			BYTE *			pRow
		);	

		// @cmember SetRestriction
		void SetRestriction(ULONG bit);

		// @cmember ClearRestriction
		void ClearRestriction(ULONG bit);

		// @cmember IsSchemaSupported
		BOOL IsSchemaSupported(GUID schema);

		// @cmember TestSchemaRestrictions
		BOOL TestSchemaRestrictions(GUID schema,ULONG bit, ROW_COUNT eRowCount=DEFAULT, ULONG cExpRows=0);

		// @cmember Test Data Returned from IDBSchema
		BOOL TestReturnData(ULONG iRow,DATA * pColumn,ULONG bit,BOOL * fRes,WCHAR * wszRes, BOOL fNullable=TRUE);

		// @cmember Test the Table Name Restriction
		void SetRestriction(RESTRICTIONSENUM ebit,ULONG ulRestriction,WCHAR ** pwszR,WCHAR * wszRestriction);

		BOOL PrepareParams_ASSERTIONS();
		BOOL VerifyRow_ASSERTIONS(ULONG iRow,BYTE * pData);	
		
		BOOL PrepareParams_CATALOGS();
		BOOL VerifyRow_CATALOGS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_CHARACTER_SETS();
		BOOL VerifyRow_CHARACTER_SETS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_CHECK_CONSTRAINTS();
		BOOL VerifyRow_CHECK_CONSTRAINTS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_COLLATIONS();
		BOOL VerifyRow_COLLATIONS(ULONG iRow,BYTE * pData);

		BOOL PrepareParams_COLUMN_DOMAIN_USAGE();
		BOOL VerifyRow_COLUMN_DOMAIN_USAGE(ULONG iRow,BYTE * pData);

		BOOL PrepareParams_COLUMN_PRIVILEGES();
		BOOL VerifyRow_COLUMN_PRIVILEGES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_COLUMNS();
		BOOL VerifyRow_COLUMNS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_CONSTRAINT_COLUMN_USAGE();
		BOOL VerifyRow_CONSTRAINT_COLUMN_USAGE(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_CONSTRAINT_TABLE_USAGE();
		BOOL VerifyRow_CONSTRAINT_TABLE_USAGE(ULONG iRow,BYTE * pData);
		
		BOOL PrepareParams_FOREIGN_KEYS();
		BOOL VerifyRow_FOREIGN_KEYS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_INDEXES();
		BOOL VerifyRow_INDEXES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_KEY_COLUMN_USAGE();
		BOOL VerifyRow_KEY_COLUMN_USAGE(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_PRIMARY_KEYS();
		BOOL VerifyRow_PRIMARY_KEYS(ULONG iRow,BYTE * pData);

		BOOL PrepareParams_PROCEDURE_COLUMNS();
		BOOL VerifyRow_PROCEDURE_COLUMNS(ULONG iRow,BYTE * pData);

		BOOL PrepareParams_PROCEDURE_PARAMETERS();
		BOOL VerifyRow_PROCEDURE_PARAMETERS(ULONG iRow, BYTE * pData);

		BOOL PrepareParams_PROCEDURES();
		BOOL VerifyRow_PROCEDURES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_PROVIDER_TYPES();
		BOOL VerifyRow_PROVIDER_TYPES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_REFERENTIAL_CONSTRAINTS();
		BOOL VerifyRow_REFERENTIAL_CONSTRAINTS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_SCHEMATA();
		BOOL VerifyRow_SCHEMATA(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_SQL_LANGUAGES();
		BOOL VerifyRow_SQL_LANGUAGES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_STATISTICS();
		BOOL VerifyRow_STATISTICS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TABLE_CONSTRAINTS();
		BOOL VerifyRow_TABLE_CONSTRAINTS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TABLE_PRIVILEGES();
		BOOL VerifyRow_TABLE_PRIVILEGES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TABLES();
		BOOL VerifyRow_TABLES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TABLES_INFO();
		BOOL VerifyRow_TABLES_INFO(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TRANSLATIONS();
		BOOL VerifyRow_TRANSLATIONS(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_TRUSTEE();
		BOOL VerifyRow_TRUSTEE(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_USAGE_PRIVILEGES();
		BOOL VerifyRow_USAGE_PRIVILEGES(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_VIEW_COLUMN_USAGE();
		BOOL VerifyRow_VIEW_COLUMN_USAGE(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_VIEW_TABLE_USAGE();
		BOOL VerifyRow_VIEW_TABLE_USAGE(ULONG iRow,BYTE * pData);	

		BOOL PrepareParams_VIEWS();
		BOOL VerifyRow_VIEWS(ULONG iRow,BYTE * pData);	

		// Capture Restrictions
		BOOL CaptureRestrictions();
		void FreeRestrictions();

		BOOL Find_Catalog_and_Schema();
		BOOL Find_TableInfo();
		BOOL Find_Assertion_Constraint();
		BOOL Find_Character_Set();
		BOOL Find_Collation();
		BOOL Find_Domain();
		BOOL Find_Grantor_and_Grantee();
		BOOL Find_Key_Column_Usage_Constraint();
		BOOL Find_Procedure();
		BOOL Find_ProcedureColumn();
		BOOL Find_Parameter();
		BOOL Find_Referential_Constraint();
		BOOL Find_Constraint_Type();
		BOOL Find_Translation();
		BOOL Find_View();
		BOOL Find_Check_Constraint();
		BOOL Find_BestMatch();
		BOOL Find_PK_and_FK();


		BOOL Release_Catalog_and_Schema();
		BOOL Release_TableInfo();
		BOOL Release_Assertion_Constraint();
		BOOL Release_Character_Set();
		BOOL Release_Collation();
		BOOL Release_Domain();
		BOOL Release_Grantor_and_Grantee();
		BOOL Release_Key_Column_Usage_Constraint();
		BOOL Release_Procedure();
		BOOL Release_ProcedureColumn();
		BOOL Release_Parameter();
		BOOL Release_Referential_Constraint();
		BOOL Release_Constraint_Type();
		BOOL Release_Translation();
		BOOL Release_View();
		BOOL Release_Check_Constraint();
		BOOL Release_Index_Type();
		BOOL Release_PK_and_FK();


		HRESULT GetAllPropertySets();
		HRESULT FreeAllPropertySets();

		HRESULT ShouldTestSchemaRestriction(GUID schema,ULONG restrictions);
		HRESULT GetRowsetPropertySet();
		ULONG	NumberofRestrictions(GUID schema);
		
		BOOL PropIsBool(DBPROPID prop);
		BOOL IsRowsetPropertySupported(DBPROPID propid);
		BOOL IsRowsetPropertySet(GUID guidPropset);

		// Functions for creating tables with Primary and foreign keys.
		BOOL InitKeysOnTable();
		BOOL TerminateKeysOnTable();

		// Returns true if it is known the schema guid for index has a catalog restriction.
		ULONG SchemaCatalogRestriction(ULONG ulIndex);

		// Returns true if it is known the schema guid for index has a schema restriction.
		ULONG SchemaSchemaRestriction(ULONG ulIndex);

		// Returns true if it is known the schema guid for index has a table name restriction.
		ULONG SchemaTableRestriction(ULONG ulIndex);

		// Sets catalog restriction for those schemas that support it to prevent excessive run times.
		void LimitRestrictions(ULONG ulIndex);

private:
	ULONG m_ulTableOrdinal;
	LPWSTR m_pwszStringsBuffer;
	DBCOLUMNINFO * m_prgColInfo;
	ULONG m_cColInfo;
	LPWSTR m_pwszTableName;
protected:
	BOOL m_fPrimaryKey;
	BOOL m_fForeignKey;

	// Returns the count of columns and column info for the given table
	HRESULT GetColumnInfo(LPBYTE pData, DBBINDING * pBinding);

	void SetRowCount(ROW_COUNT eRowCount, ULONG ulRowCount);

	virtual HRESULT GetNextRows(DBROWOFFSET lOffset, DBROWCOUNT cRows, DBCOUNTITEM* pcRowsObtained, HROW** prghRow);
};	

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Init
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::Init()
{

	m_cRowsetPropSet=0;
	m_rgRowsetPropSet=NULL;
	
	if(COLEDB::Init())	
	{
		m_pwszDropProc=NULL;

		// Set DSO pointer
		if(m_pThisTestModule->m_pIUnknown)
		{
			m_pIDBInitialize = (IDBInitialize *)m_pThisTestModule->m_pIUnknown;
			m_pIDBInitialize->AddRef();
		}

		// Set CSession pointer
		SetDBSession((IDBCreateCommand *)m_pThisTestModule->m_pIUnknown2);
	
		// Set Table pointer
		SetTable((CTable *)m_pThisTestModule->m_pVoid, DELETETABLE_NO);		

		// Get IDBSchemaRowset pointer
		if(m_pIOpenRowset)
		{
			if(FAILED(m_pIOpenRowset->QueryInterface(
				IID_IDBSchemaRowset,
				(void **) &m_pIDBSchemaRowset)))
				return FALSE;

		}
		else //(m_pIDBCreateCommand)
		{
			if(FAILED(m_pIDBCreateCommand->QueryInterface(
				IID_IDBSchemaRowset,
				(void **) &m_pIDBSchemaRowset)))
				return FALSE;
		}

		// First check for support for alter table syntax.
		ULONG ulSQLSupport = 0;
		GetProperty(DBPROP_SQLSUPPORT, DBPROPSET_DATASOURCEINFO, m_pIDBInitialize, &ulSQLSupport);
		
		if( ulSQLSupport & DBPROPVAL_SQL_ANSI89_IEF )
			InitKeysOnTable();

		// Get Supported Schemas
		if(FAILED(m_pIDBSchemaRowset->GetSchemas(
			&m_cSchemasSupported,
			&m_rgSchemas,
			&m_rgRestrictionSupport)))
		{
			m_pIDBSchemaRowset->Release();
			m_pIDBSchemaRowset = NULL;
			return FALSE;
		}

		if(m_cSchemasSupported==0)
		{
			odtLog << L"No schemas supported\n";
			return FALSE;
		}
		if(FAILED(GetAllPropertySets()))
			return FALSE;

		if(FAILED(GetRowsetPropertySet()))
			return FALSE;

		if(!m_fDontCaptureRestrictions)
			CaptureRestrictions();
	
		return TRUE;
	}  
	return FALSE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Terminate
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::Terminate()
{

	// Release everything from the init
	SAFE_RELEASE(m_pIDBSchemaRowset);

	// Free the memory
	PROVIDER_FREE(m_rgSchemas);
	PROVIDER_FREE(m_rgRestrictionSupport);
	PROVIDER_FREE(m_rgSupportedProperties);

	// Free the Properties
	FreeProperties(&m_cDBPROPINFOSET, &m_rgDBPROPINFOSET);
	FreeProperties(&m_cRowsetDBPROPINFOSET, &m_rgRowsetDBPROPINFOSET, &m_pDescBuffer);
	FreeProperties(&m_cRowsetPropSet, &m_rgRowsetPropSet);

	FreeRestrictions();

	ULONG ulSQLSupport = 0;
	GetProperty(DBPROP_SQLSUPPORT, DBPROPSET_DATASOURCEINFO, m_pIDBInitialize, &ulSQLSupport);
	
	if( ulSQLSupport & DBPROPVAL_SQL_ANSI89_IEF )
		TerminateKeysOnTable();

	ReleaseDBSession();
	SAFE_RELEASE(m_pIDBInitialize);

	if(!COLEDB::Terminate())
		return FALSE;

	return(CTestCases::Terminate());
}

// Is the property set GUID one of the Rowset property GUIDs
BOOL CSchemaTest::IsRowsetPropertySet(GUID guidPropset)
{
	// Loop through all the Rowset property info from the DBPROPSET_ROWSETALL
	// Includes provider specific
	for (ULONG iPropertyInfoSet=0;
		iPropertyInfoSet < m_cRowsetDBPROPINFOSET;
		iPropertyInfoSet++
		)
		if (m_rgRowsetDBPROPINFOSET[iPropertyInfoSet].guidPropertySet == guidPropset)
			return TRUE;

	return FALSE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Is rowset property supported by temp table
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::IsRowsetPropertySupported(DBPROPID propid)
{
	ULONG i,j;
	ULONG iRowsetSet=0;

	for(i=0;i<m_rgRowsetPropSet[0].cProperties;i++)
	{
		if(propid==m_rgRowsetPropSet[0].rgProperties[i].dwPropertyID)
		{
			if (m_rgRowsetPropSet[0].rgProperties[i].dwStatus != DBPROPSTATUS_NOTSUPPORTED)
			{
				PRVTRACE(L"Property is supported\n");
				goto CHECK_WRITABLE;
			}
		}
	}
	PRVTRACE(L"Property is not supported\n");
	return FALSE;


CHECK_WRITABLE:
	for(i=0;i<m_cDBPROPINFOSET;i++)
	{
		if(IsEqualGUID(m_rgDBPROPINFOSET[i].guidPropertySet,DBPROPSET_ROWSET))
		{
			for(j=0;j<m_rgDBPROPINFOSET[i].cPropertyInfos;j++)
			{
				if(m_rgDBPROPINFOSET[i].rgPropertyInfos[j].dwPropertyID==propid)
				{
					if(m_rgDBPROPINFOSET[i].rgPropertyInfos[j].dwFlags &DBPROPFLAGS_WRITE)
					{
						PRVTRACE(L"Property is writable\n");
						return TRUE;
					}
					else
					{
						PRVTRACE(L"Property is not writable\n");
						//odtLog << L"Property is not writable\n";
						return FALSE;
					}
				}
			}
		}
	}
 	
	return FALSE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Find all rowset properties that temp table supports
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HRESULT CSchemaTest::GetRowsetPropertySet()
{
	HRESULT			hr=E_FAIL;
	IRowsetInfo *	pIRowsetInfo=NULL;

	if(!m_pIDBSchemaRowset)
		goto CLEANUP;

	// generate rowset with types as schema
	// since it is mandatory
	if(FAILED(hr=m_pIDBSchemaRowset->GetRowset(
		NULL,
		DBSCHEMA_PROVIDER_TYPES,
		0,
		NULL,
		IID_IRowsetInfo,
		0,
		NULL,
		(IUnknown **) &pIRowsetInfo)))
		goto CLEANUP;
	if(FAILED(hr=pIRowsetInfo->GetProperties(
		0,
		NULL,
		&m_cRowsetPropSet,
		&m_rgRowsetPropSet)))
		goto CLEANUP;

CLEANUP:

	if(pIRowsetInfo)
		pIRowsetInfo->Release();

	return hr;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Is Property a VT_BOOL
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::PropIsBool(DBPROPID prop)
{
	if (( prop == DBPROP_ABORTPRESERVE) ||
		 (prop == DBPROP_APPENDONLY) ||
 		 (prop == DBPROP_BLOCKINGSTORAGEOBJECTS) ||
		 (prop == DBPROP_BOOKMARKS) ||
		 (prop == DBPROP_BOOKMARKSKIPPED) ||
		 (prop == DBPROP_CACHEDEFERRED) ||
		 (prop == DBPROP_CANFETCHBACKWARDS) ||
		 (prop == DBPROP_CANHOLDROWS) ||
		 (prop == DBPROP_CANSCROLLBACKWARDS) ||
		 (prop == DBPROP_CHANGEINSERTEDROWS) ||
		 (prop == DBPROP_COLUMNRESTRICT) ||
		 (prop == DBPROP_COMMITPRESERVE) ||
		 (prop == DBPROP_DEFERRED) ||
		 (prop == DBPROP_DELAYSTORAGEOBJECTS) ||
		 (prop == DBPROP_IMMOBILEROWS) ||
		 (prop == DBPROP_LITERALBOOKMARKS) ||
		 (prop == DBPROP_LITERALIDENTITY) ||
		 (prop == DBPROP_MAYWRITECOLUMN) ||
		 (prop == DBPROP_ORDEREDBOOKMARKS) ||
		 (prop == DBPROP_OTHERINSERT) ||
		 (prop == DBPROP_OTHERUPDATEDELETE) ||
		 (prop == DBPROP_OWNINSERT) ||
		 (prop == DBPROP_OWNUPDATEDELETE) ||
 		 (prop == DBPROP_QUICKRESTART) ||
		 (prop == DBPROP_REENTRANTEVENTS) ||
		 (prop == DBPROP_REMOVEDELETED) ||
		 (prop == DBPROP_REPORTMULTIPLECHANGES) ||
		 (prop == DBPROP_RETURNPENDINGINSERTS) ||
		 (prop == DBPROP_ROWRESTRICT) ||
		 (prop == DBPROP_SERVERCURSOR) ||
		 (prop == DBPROP_STRONGIDENTITY) ||
		 (prop == DBPROP_TRANSACTEDOBJECT) ||
		 (prop == DBPROP_OTHERINSERT) ||
		 (prop == DBPROP_IAccessor) ||
		 (prop == DBPROP_IColumnsInfo) ||		 
		 (prop == DBPROP_IColumnsRowset) ||
 		 (prop == DBPROP_IConnectionPointContainer) ||
		 (prop == DBPROP_IRowset) ||
 		 (prop == DBPROP_IRowsetChange) ||
 		 (prop == DBPROP_IRowsetIdentity) ||
 		 (prop == DBPROP_IRowsetInfo) ||
		 (prop == DBPROP_IRowsetLocate) ||
 		 (prop == DBPROP_IRowsetResynch) ||
		 (prop == DBPROP_IRowsetScroll) ||
		 (prop == DBPROP_IRowsetUpdate) ||
 		 (prop == DBPROP_ISupportErrorInfo) ||
		 (prop == DBPROP_ILockBytes) ||
 		 (prop == DBPROP_ISequentialStream) ||
		 (prop == DBPROP_IStorage) ||
 		 (prop == DBPROP_IStream))
		return TRUE;
	else
		return FALSE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HRESULT CSchemaTest::GetAllPropertySets()
{

	HRESULT hr=E_FAIL;
	ULONG index1=0;
	ULONG index2=0;
	IDBProperties * pIDBProperties=NULL;

	// make sure I have a DSO object
	if(!m_pIDBInitialize)
		goto CLEANUP;

	// get necessary pointer
	if(!CHECK(hr=m_pIDBInitialize->QueryInterface(IID_IDBProperties,(void**)&pIDBProperties),S_OK))
		goto CLEANUP;

	// get property info
	if(FAILED(hr=pIDBProperties->GetPropertyInfo(0,NULL,&m_cDBPROPINFOSET,&m_rgDBPROPINFOSET,&m_pDescBuffer)))
		goto CLEANUP;

	// Get all the rowset properties
//	DBPROPID	* prgRowsetPropertyIDs;
	DBPROPIDSET rgRowsetIDSET[1];
	rgRowsetIDSET[0].rgPropertyIDs=NULL;
	rgRowsetIDSET[0].cPropertyIDs=0;
	rgRowsetIDSET[0].guidPropertySet=DBPROPSET_ROWSETALL;

	if(FAILED(hr=pIDBProperties->GetPropertyInfo(1,rgRowsetIDSET,&m_cRowsetDBPROPINFOSET,&m_rgRowsetDBPROPINFOSET,NULL)))
		goto CLEANUP;

	for(index1=0;index1<m_cDBPROPINFOSET;index1++)
	{
		if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_COLUMN))
			PRVTRACE(L"[DBPROPSET_COLUMN]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DATASOURCE))
			PRVTRACE(L"[DBPROPSET_DATASOURCE]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DATASOURCEALL))
			PRVTRACE(L"[DBPROPSET_DATASOURCEALL]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DATASOURCEINFOALL))
			PRVTRACE(L"[DBPROPSET_DATASOURCEINFOALL]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DATASOURCEINFO))
			PRVTRACE(L"[DBPROPSET_DATASOURCEINFO]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DBINIT))
			PRVTRACE(L"[DBPROPSET_DBINIT]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_DBINITALL))
			PRVTRACE(L"[DBPROPSET_DBINITALL]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_INDEX))
			PRVTRACE(L"[DBPROPSET_INDEX]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_ROWSET))
			PRVTRACE(L"[DBPROPSET_ROWSET]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_ROWSETALL))
			PRVTRACE(L"[DBPROPSET_ROWSETALL]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_SESSION))
			PRVTRACE(L"[DBPROPSET_SESSION]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_SESSIONALL))
			PRVTRACE(L"[DBPROPSET_SESSIONALL]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_TABLE))
			PRVTRACE(L"[DBPROPSET_TABLE]\n");
		else if(IsEqualGUID(m_rgDBPROPINFOSET[index1].guidPropertySet,DBPROPSET_PROPERTIESINERROR))
			PRVTRACE(L"[DBPROPSET_PROPERTIESINERROR]\n");
		else
			PRVTRACE(L"[Unknown DBPROPSET]\n");

		for(index2=0;index2<m_rgDBPROPINFOSET[index1].cPropertyInfos;index2++)
		{
			
			PRVTRACE(L"[desc=%s],[propid=%d],[dwflags=%d],[vartype=%d]\n",
					m_rgDBPROPINFOSET[index1].rgPropertyInfos[index2].pwszDescription,
					m_rgDBPROPINFOSET[index1].rgPropertyInfos[index2].dwPropertyID,
					m_rgDBPROPINFOSET[index1].rgPropertyInfos[index2].dwFlags,
					m_rgDBPROPINFOSET[index1].rgPropertyInfos[index2].vtType);
		}

	}

CLEANUP:

	
	if(pIDBProperties)
		pIDBProperties->Release();

	return hr;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ShouldTestSchemaRestriction
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HRESULT CSchemaTest::ShouldTestSchemaRestriction(GUID schema,ULONG restrictions)
{
	// Figure out the return code
	for(ULONG index=0; index < m_cSchemasSupported; index++)
	{
		if(IsEqualGUID(schema,m_rgSchemas[index]))
		{
			if( (ALLRES & restrictions) || 
				(m_rgRestrictionSupport[index] & restrictions) )
				return S_OK;
			
			if(restrictions <= NumberofRestrictions(schema))
				return E_INVALIDARG;

			break;
		}
	}

	return E_INVALIDARG;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NumberofRestrictions
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ULONG CSchemaTest::NumberofRestrictions(GUID schema)
{
	// all schemas with 0 restrictions
	// don't need to check schemas anyway
	if(IsEqualGUID(schema,DBSCHEMA_SQL_LANGUAGES))
		return 0;

	// all schemas with 1 retriction
	if(IsEqualGUID(schema,DBSCHEMA_CATALOGS))
		return 1;

	// all schemas with 2 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_PROVIDER_TYPES))
		return 3;

	// all schemas with 3 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_ASSERTIONS) ||
		IsEqualGUID(schema,DBSCHEMA_CHARACTER_SETS) ||
		IsEqualGUID(schema,DBSCHEMA_CHECK_CONSTRAINTS) ||
		IsEqualGUID(schema,DBSCHEMA_COLLATIONS) ||
		IsEqualGUID(schema,DBSCHEMA_CONSTRAINT_TABLE_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_PRIMARY_KEYS) ||
		IsEqualGUID(schema,DBSCHEMA_REFERENTIAL_CONSTRAINTS) ||
		IsEqualGUID(schema,DBSCHEMA_SCHEMATA) ||
		IsEqualGUID(schema,DBSCHEMA_STATISTICS) ||
		IsEqualGUID(schema,DBSCHEMA_TRANSLATIONS) ||
		IsEqualGUID(schema,DBSCHEMA_VIEW_COLUMN_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_VIEW_TABLE_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_VIEWS))
		return 7;

	// all schemas with 4 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_COLUMN_DOMAIN_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_COLUMNS) ||
		IsEqualGUID(schema,DBSCHEMA_CONSTRAINT_COLUMN_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_PROCEDURE_COLUMNS) ||
		IsEqualGUID(schema,DBSCHEMA_PROCEDURE_PARAMETERS) ||
		IsEqualGUID(schema,DBSCHEMA_PROCEDURES) ||
		IsEqualGUID(schema,DBSCHEMA_TABLES) || 
		IsEqualGUID(schema,DBSCHEMA_TABLES_INFO)) 
		return 15;

	// all schemas with 5 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_INDEXES) ||
		IsEqualGUID(schema,DBSCHEMA_TABLE_PRIVILEGES))
		return 31;
	
	// all schemas with 6 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_COLUMN_PRIVILEGES) ||
		IsEqualGUID(schema,DBSCHEMA_FOREIGN_KEYS) ||
		IsEqualGUID(schema,DBSCHEMA_USAGE_PRIVILEGES))
		return 63;

	// all schemas with 7 restrictions
	if(IsEqualGUID(schema,DBSCHEMA_KEY_COLUMN_USAGE) ||
		IsEqualGUID(schema,DBSCHEMA_TABLE_CONSTRAINTS))
		return 127;

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Set Bit
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CSchemaTest::SetRestriction(ULONG bit)
{
	m_restrict |= bit;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Clear Bit
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CSchemaTest::ClearRestriction(ULONG bit)
{
	m_restrict &= ~(bit);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Schema Restrictions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::TestSchemaRestrictions(GUID schema, ULONG bit, ROW_COUNT eRowCount, ULONG cExpRows)
{
	HRESULT hr	  = E_FAIL;
	HRESULT ExpHR = S_OK;
	
	// Initialize the needed pointers
	INIT;

	// This is not initialized on purpose
	ExpHR=ShouldTestSchemaRestriction(schema,bit);

	if(FAILED(ExpHR)) 
	{
		odtLog <<wszRESTRICTIONNOTSUPPORTED;
		m_fPassUnsupportedRestrictions = TRUE;
	}

	// Set the correct info
	SetRestriction(bit);
	m_iid = IID_IRowset;

	if(GetSchemaInfo(SPECIFIC,0,schema))
	{
		// Update expected row count if required for this variation.
		if (eRowCount != DEFAULT)
			SetRowCount(eRowCount, cExpRows);

		// Test method with invalid schema guid
		hr=GetRowset();

		// Provider either returns E_INVALIDARG or DB_E_NOTSUPPORTED
		if( (FAILED(ExpHR) && (hr == E_INVALIDARG || hr == DB_E_NOTSUPPORTED)) ||
			(CHECK(hr, ExpHR)) )
			m_fResult = TEST_PASS;
	}

	FREE;

	return m_fResult;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Schema Restrictions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::TestReturnData(ULONG iRow, DATA *	pColumn, ULONG bit, BOOL * fRes, WCHAR * wszRes, BOOL fNullable)
{
	BOOL fResults = TRUE;

	if(m_currentBitMask & bit || !fNullable)
	{
		// Check Status and Value
		if( (*fRes) && 
			((!COMPARE((iRow>=1), TRUE)) || 
			 (!COMPARE(pColumn->sStatus, DBSTATUS_S_OK)) ||
			 ((m_restrict & bit) &&
			  ((!COMPARE(pColumn->ulLength, wcslen(wszRes)*2)) ||
			   (!COMPARE(0, RelCompareString(wszRes, (TYPE_WSTR)pColumn->bValue)))))) )
			*fRes = fResults = FALSE;
	}
	else
	{
		COMPARE(pColumn->sStatus, DBSTATUS_S_ISNULL);
		COMPARE(pColumn->ulLength, 0);
	}

	return fResults;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test the Restriction for IDBSchemaRowset
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CSchemaTest::SetRestriction(RESTRICTIONSENUM bit, ULONG ulRestriction, WCHAR ** pwszR, WCHAR * wszRestriction)
{
	// Check if the restriction is supported
	if((m_currentBitMask & bit) || m_fPassUnsupportedRestrictions)
	{
		if((m_restrict & bit) || (m_restrict & ALLRES))
		{
			// if NULL set to a bogus restriction
			m_rgvarRestrict[ulRestriction-1].bstrVal = SysAllocString(wszRestriction ? wszRestriction : L"BogusRestriction");
			*pwszR = wcsDuplicate(wszRestriction ? wszRestriction : L"BogusRestriction");

			m_rgvarRestrict[ulRestriction-1].vt = VT_BSTR;
			m_cRestrictionsCurrent ++;
			SetRestriction(bit);
		}
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Init_Stuff
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CSchemaTest::Init_Stuff(void)
{
	m_fBOOKMARK_REQUESTED=FALSE;
	m_fBOOKMARK_FOUND=FALSE;
	m_fRes1=TRUE;
	m_fRes2=TRUE;
	m_fRes3=TRUE;
	m_fRes4=TRUE;
	m_fRes5=TRUE;
	m_fRes6=TRUE;
	m_fRes7=TRUE;
	m_fMyTableCatalogFound=FALSE;
	m_fMyTableSchemaFound=FALSE;
	m_MyTableName1Found=FALSE;
	m_MyTableName2Found=FALSE;
	m_MyColumnNameFound=FALSE;
	m_MyTableTypeFound=FALSE;
	
	m_cErrors=0;
	m_cColumns=0;				
	m_cRestrictions=0;	
	m_cRestrictionsCurrent=0;	
	m_cDBBINDING=0;			
	m_cDBPROPSET=0;
	m_cDBCOLUMNINFO=0;		
	m_iid=IID_NULL;					
	m_guid=GUID_NULL;					
	m_restrict=0;
	m_HR=E_FAIL;
	m_fResult=FALSE;
	m_fAtLeast1UnsupportedRestrictionIsSet=FALSE;


	m_fRowsetPointerNULL=FALSE;
	m_fCountRestrictionsNULL=FALSE;
	m_fRangeRestrictionsNULL=FALSE;
	m_fCountPropSetNULL=FALSE;
	m_fRangePropSetNULL=FALSE;
	m_fPassUnsupportedRestrictions=FALSE;
	m_fRestrictionsAsInitVariants=FALSE;

	
	m_wszR1=NULL;
	m_wszR2=NULL;
	m_wszR3=NULL;
	m_wszR4=NULL;								    
	m_wszR5=NULL;
	m_wszR6=NULL;
	m_wszR7=NULL;
	m_ulR=0;

	for(ULONG index=0;index<MAXRESTRICTION;index++)
		VariantInit(&(m_rgvarRestrict[index]));

	for(index=0;index<MAXPROP;index++)
		VariantInit(&(m_rgDBPROP[index].vValue));


}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Free_Stuff
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CSchemaTest::Free_Stuff(void)
{
	// don't need to be freed because these aren't dynamic
 	m_rgColumnNames=NULL;
	m_rgColumnTypes=NULL;
	
	SAFE_RELEASE_(m_pIRowset);
	SAFE_RELEASE_(m_pIAccessor);
	SAFE_RELEASE_(m_pIRowsetInfo);
	SAFE_RELEASE_(m_pIColumnsInfo);
	SAFE_RELEASE_(m_pIRowsetChange);
	SAFE_RELEASE_(m_pIUnknown);

	PROVIDER_FREE(m_rgDBCOLUMNINFO);
	PROVIDER_FREE(m_pStringsBuffer);

	for(ULONG index=0;index<MAXRESTRICTION;index++)
  		VariantClear(&(m_rgvarRestrict[index]));

	for(index=0;index<MAXPROP;index++)
		VariantClear(&(m_rgDBPROP[index].vValue));

	// Free the Restrictions
	PROVIDER_FREE(m_wszR1);
	PROVIDER_FREE(m_wszR2);
	PROVIDER_FREE(m_wszR3);
	PROVIDER_FREE(m_wszR4);
	PROVIDER_FREE(m_wszR5);
	PROVIDER_FREE(m_wszR6);
	PROVIDER_FREE(m_wszR7);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// GetRowset, execute method and check results
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HRESULT CSchemaTest::GetRowset(BOOL fFreeRowset)
{
	IUnknown * pIUnknown = NULL;
	IUnknown **ppIUnknown=&pIUnknown;

	if(!m_fCaptureRestrictions && m_PrintSchemaName)
		TraceSchemaName(m_guid, TRUE, TRUE);

	if(m_iid==IID_IRowset)
		ppIUnknown = (IUnknown **)&m_pIRowset;
	else if(m_iid==IID_IAccessor)
		ppIUnknown = (IUnknown **)&m_pIAccessor;
	else if(m_iid==IID_IRowsetInfo)
		ppIUnknown = (IUnknown **)&m_pIRowsetInfo;
	else if(m_iid==IID_IColumnsInfo)
		ppIUnknown = (IUnknown **)&m_pIColumnsInfo;
	else if(m_iid==IID_IRowsetChange)
		ppIUnknown = (IUnknown **)&m_pIRowsetChange;
	else if(m_iid==IID_IUnknown)
		ppIUnknown = (IUnknown **)&m_pIUnknown;

	// Now get Schema rowset.
	m_HR=m_pIDBSchemaRowset->GetRowset(
		m_punkOuter, m_guid, (m_fCountRestrictionsNULL ? 0 : m_cRestrictions),
		(m_fRangeRestrictionsNULL ? NULL : m_rgvarRestrict), m_iid,
		(m_fCountPropSetNULL ? 0 : m_cDBPROPSET), (m_fRangePropSetNULL ? 0 : m_rgDBPROPSET),
		(m_fRowsetPointerNULL ? NULL : ppIUnknown));

	if (SUCCEEDED(m_HR))
		CheckResults(*ppIUnknown, m_iid);

	if (fFreeRowset && ppIUnknown)
		SAFE_RELEASE(*ppIUnknown);

	return m_HR;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This routine should be called when the information for the next schema (guid) is needed
// I either want the next supported schema or a specific schema.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::GetSchemaInfo
(
	REQUESTED_SCHEMA schemaType,			// [IN] if I want the schema supported, or not, or I have a specific schema in mind
	ULONG ulIndexOfSchemaRequesting,		// [IN] Index of supported Schema
	GUID schema								// [IN] specific schema
)
{
	BOOL fResult = FALSE;
	m_cRestrictionsCurrent = 0;

	// Set default row count information to expect 1 row but only
	// warn if we get less
	SetRowCount(MIN_VALUE, 1);

	// Initialize Variants
	VariantInit(&m_rgvarRestrict[0]);
	VariantInit(&m_rgvarRestrict[1]);
	VariantInit(&m_rgvarRestrict[2]);
	VariantInit(&m_rgvarRestrict[3]);
	VariantInit(&m_rgvarRestrict[4]);
	VariantInit(&m_rgvarRestrict[5]);
	VariantInit(&m_rgvarRestrict[6]);

	// grab schema from list of supported schemas
	if(schemaType==SUPPORTED)
		memcpy(&m_guid,&(m_rgSchemas[ulIndexOfSchemaRequesting]),sizeof(GUID));
	else if(schemaType==SPECIFIC)
		memcpy(&m_guid,&schema,sizeof(GUID));
	else
		return FALSE;

	for(ULONG index=0;index<m_cSchemasSupported;index++)
	{
		if(IsEqualGUID(m_guid,m_rgSchemas[index])) {
			m_currentBitMask = m_rgRestrictionSupport[index];
			break;
		}
	}

 	PRVTRACE(L"Getting ");
	TraceSchemaName(m_guid,FALSE,TRUE);
 	PRVTRACE(L"\n");

	// Find the schema
	if(IsEqualGUID(DBSCHEMA_ASSERTIONS,m_guid))
		fResult= PrepareParams_ASSERTIONS();
	else if(IsEqualGUID(DBSCHEMA_CATALOGS,m_guid))
		fResult= PrepareParams_CATALOGS();
	else if(IsEqualGUID(DBSCHEMA_CHARACTER_SETS,m_guid))
 		fResult= PrepareParams_CHARACTER_SETS();
	else if(IsEqualGUID(DBSCHEMA_CHECK_CONSTRAINTS,m_guid))
 		fResult= PrepareParams_CHECK_CONSTRAINTS();
	else if(IsEqualGUID(DBSCHEMA_COLLATIONS,m_guid))
 		fResult= PrepareParams_COLLATIONS();
	else if(IsEqualGUID(DBSCHEMA_COLUMN_DOMAIN_USAGE,m_guid))
 		fResult= PrepareParams_COLUMN_DOMAIN_USAGE();
	else if(IsEqualGUID(DBSCHEMA_COLUMN_PRIVILEGES,m_guid))
 		fResult= PrepareParams_COLUMN_PRIVILEGES();
	else if(IsEqualGUID(DBSCHEMA_COLUMNS,m_guid))
		fResult= PrepareParams_COLUMNS();
	else if(IsEqualGUID(DBSCHEMA_CONSTRAINT_COLUMN_USAGE,m_guid))
 		fResult= PrepareParams_CONSTRAINT_COLUMN_USAGE();
	else if(IsEqualGUID(DBSCHEMA_CONSTRAINT_TABLE_USAGE,m_guid))
 		fResult= PrepareParams_CONSTRAINT_TABLE_USAGE();
	else if(IsEqualGUID(DBSCHEMA_FOREIGN_KEYS,m_guid))
 		fResult= PrepareParams_FOREIGN_KEYS();
	else if(IsEqualGUID(DBSCHEMA_INDEXES,m_guid))
 		fResult= PrepareParams_INDEXES();
	else if(IsEqualGUID(DBSCHEMA_KEY_COLUMN_USAGE,m_guid))
 		fResult= PrepareParams_KEY_COLUMN_USAGE();
	else if(IsEqualGUID(DBSCHEMA_PRIMARY_KEYS,m_guid))
 		fResult= PrepareParams_PRIMARY_KEYS();
	else if(IsEqualGUID(DBSCHEMA_PROCEDURE_COLUMNS,m_guid))
 		fResult= PrepareParams_PROCEDURE_COLUMNS();
	else if(IsEqualGUID(DBSCHEMA_PROCEDURE_PARAMETERS, m_guid))
 		fResult= PrepareParams_PROCEDURE_PARAMETERS();
	else if(IsEqualGUID(DBSCHEMA_PROCEDURES,m_guid))
 		fResult= PrepareParams_PROCEDURES();
	else if(IsEqualGUID(DBSCHEMA_PROVIDER_TYPES,m_guid))
		fResult= PrepareParams_PROVIDER_TYPES();
	else if(IsEqualGUID(DBSCHEMA_REFERENTIAL_CONSTRAINTS,m_guid))
 		fResult= PrepareParams_REFERENTIAL_CONSTRAINTS();
	else if(IsEqualGUID(DBSCHEMA_SCHEMATA,m_guid))
 		fResult= PrepareParams_SCHEMATA();
	else if(IsEqualGUID(DBSCHEMA_SQL_LANGUAGES,m_guid))
 		fResult= PrepareParams_SQL_LANGUAGES();
	else if(IsEqualGUID(DBSCHEMA_STATISTICS,m_guid))
 		fResult= PrepareParams_STATISTICS();
	else if(IsEqualGUID(DBSCHEMA_TABLES,m_guid))
		fResult= PrepareParams_TABLES();
	else if(IsEqualGUID(DBSCHEMA_TABLES_INFO,m_guid))
		fResult= PrepareParams_TABLES_INFO();
	else if(IsEqualGUID(DBSCHEMA_TABLE_CONSTRAINTS,m_guid))
 		fResult= PrepareParams_TABLE_CONSTRAINTS();
	else if(IsEqualGUID(DBSCHEMA_TABLE_PRIVILEGES,m_guid))
 		fResult= PrepareParams_TABLE_PRIVILEGES();
	else if(IsEqualGUID(DBSCHEMA_TRANSLATIONS,m_guid))
 		fResult= PrepareParams_TRANSLATIONS();
	else if(IsEqualGUID(DBSCHEMA_TRUSTEE,m_guid))
		fResult = PrepareParams_TRUSTEE();
	else if(IsEqualGUID(DBSCHEMA_USAGE_PRIVILEGES,m_guid))
 		fResult= PrepareParams_USAGE_PRIVILEGES();
	else if(IsEqualGUID(DBSCHEMA_VIEW_COLUMN_USAGE,m_guid))
 		fResult= PrepareParams_VIEW_COLUMN_USAGE();
	else if(IsEqualGUID(DBSCHEMA_VIEW_TABLE_USAGE,m_guid))
 		fResult= PrepareParams_VIEW_TABLE_USAGE();
	else if(IsEqualGUID(DBSCHEMA_VIEWS,m_guid))
 		fResult= PrepareParams_VIEWS();
	else
		return FALSE;

	if(!m_fRestrictionsAsInitVariants)
	{
		if(m_rgvarRestrict[0].vt == VT_EMPTY)
			ClearRestriction(FIRST);
		if(m_rgvarRestrict[1].vt == VT_EMPTY)
			ClearRestriction(SECOND);
		if(m_rgvarRestrict[2].vt == VT_EMPTY)
			ClearRestriction(THIRD);
		if(m_rgvarRestrict[3].vt == VT_EMPTY)
			ClearRestriction(FOURTH);
		if(m_rgvarRestrict[4].vt == VT_EMPTY)
			ClearRestriction(FIFTH);
		if(m_rgvarRestrict[5].vt == VT_EMPTY)
			ClearRestriction(SIXTH);
		if(m_rgvarRestrict[6].vt == VT_EMPTY)
			ClearRestriction(SEVENTH);
	}

	return fResult;
}	
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRow
//
//	Pass through function to VerifyRow for specific schema
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow
(
	GUID	m_guid,
	ULONG	iRow,
	BYTE * pRow
)
{
		// 1. ASSERTIONS
		if(IsEqualGUID(m_guid,DBSCHEMA_ASSERTIONS))
 			return VerifyRow_ASSERTIONS(iRow,pRow);
		// 2. CATALOGS
		else if(IsEqualGUID(m_guid,DBSCHEMA_CATALOGS))
 			return VerifyRow_CATALOGS(iRow,pRow);
		// 3. CHARACTER_SETS
		else if(IsEqualGUID(m_guid,DBSCHEMA_CHARACTER_SETS))
 			return VerifyRow_CHARACTER_SETS(iRow,pRow);
		// 4. CHECK_CONSTRAINTS
		else if(IsEqualGUID(m_guid,DBSCHEMA_CHECK_CONSTRAINTS))
 			return VerifyRow_CHECK_CONSTRAINTS(iRow,pRow);
		// 5. COLLATIONS
		else if(IsEqualGUID(m_guid,DBSCHEMA_COLLATIONS))
 			return VerifyRow_COLLATIONS(iRow,pRow);
		// 6. COLUMN_DOMAIN_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_COLUMN_DOMAIN_USAGE))
 			return VerifyRow_COLUMN_DOMAIN_USAGE(iRow,pRow);
		// 7. COLUMN_PRIVILEGES
		else if(IsEqualGUID(m_guid,DBSCHEMA_COLUMN_PRIVILEGES))
 			return VerifyRow_COLUMN_PRIVILEGES(iRow,pRow);
		// 8. COLUMNS
		else if(IsEqualGUID(m_guid,DBSCHEMA_COLUMNS))
 			return VerifyRow_COLUMNS(iRow,pRow);
		// 9. CONSTRAINT_COLUMN_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_CONSTRAINT_COLUMN_USAGE))
 			return VerifyRow_CONSTRAINT_COLUMN_USAGE(iRow,pRow);
		// 10. CONSTRAINT_TABLE_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_CONSTRAINT_TABLE_USAGE))
 			return VerifyRow_CONSTRAINT_TABLE_USAGE(iRow,pRow);
		// 11. FOREIGN_KEYS
		else if(IsEqualGUID(m_guid,DBSCHEMA_FOREIGN_KEYS))
 			return VerifyRow_FOREIGN_KEYS(iRow,pRow);
		// 12. INDEXES
		else if(IsEqualGUID(m_guid,DBSCHEMA_INDEXES))
 			return VerifyRow_INDEXES(iRow,pRow);
		// 13. KEY_COLUMN_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_KEY_COLUMN_USAGE))
 			return VerifyRow_KEY_COLUMN_USAGE(iRow,pRow);
 		// 14. PRIMARY_KEYS
		else if(IsEqualGUID(m_guid,DBSCHEMA_PRIMARY_KEYS))
 			return VerifyRow_PRIMARY_KEYS(iRow,pRow);
 		// 14. PROCEDURE_COLUMNS
		else if(IsEqualGUID(m_guid,DBSCHEMA_PROCEDURE_COLUMNS))
 			return VerifyRow_PROCEDURE_COLUMNS(iRow,pRow);
		// 15. PROCEDURE_PARAMETERS
		else if(IsEqualGUID(m_guid,DBSCHEMA_PROCEDURE_PARAMETERS))
			return VerifyRow_PROCEDURE_PARAMETERS(iRow,pRow);
		// 16. PROCEDURES
		else if(IsEqualGUID(m_guid,DBSCHEMA_PROCEDURES))
 			return VerifyRow_PROCEDURES(iRow,pRow);
		// 17. PROVIDER TYPES
		else if(IsEqualGUID(m_guid,DBSCHEMA_PROVIDER_TYPES))
 			return VerifyRow_PROVIDER_TYPES(iRow,pRow);
		// 18. REFERENTIAL_CONSTRAINTS
		else if(IsEqualGUID(m_guid,DBSCHEMA_REFERENTIAL_CONSTRAINTS))
 			return VerifyRow_REFERENTIAL_CONSTRAINTS(iRow,pRow);
		// 19. SCHEMATA
		else if(IsEqualGUID(m_guid,DBSCHEMA_SCHEMATA))
 			return VerifyRow_SCHEMATA(iRow+1,pRow);
		// 20. SQL_LANGUAGES
		else if(IsEqualGUID(m_guid,DBSCHEMA_SQL_LANGUAGES))
 			return VerifyRow_SQL_LANGUAGES(iRow,pRow);
		// 21. STATISTICS
		else if(IsEqualGUID(m_guid,DBSCHEMA_STATISTICS))
 			return VerifyRow_STATISTICS(iRow,pRow);
		// 22. TABLE_CONSTRAINTS
		else if(IsEqualGUID(m_guid,DBSCHEMA_TABLE_CONSTRAINTS))
 			return VerifyRow_TABLE_CONSTRAINTS(iRow,pRow);
		// 23. TABLE_PRIVILEGES
		else if(IsEqualGUID(m_guid,DBSCHEMA_TABLE_PRIVILEGES))
 			return VerifyRow_TABLE_PRIVILEGES(iRow,pRow);
		// 24. TABLES
		else if(IsEqualGUID(m_guid,DBSCHEMA_TABLES))
 			return VerifyRow_TABLES(iRow,pRow);
		// 25. TABLES_INFO
		else if(IsEqualGUID(m_guid,DBSCHEMA_TABLES_INFO))
 			return VerifyRow_TABLES_INFO(iRow,pRow);
		// 26. TRANSLATIONS
		else if(IsEqualGUID(m_guid,DBSCHEMA_TRANSLATIONS))
 			return VerifyRow_TRANSLATIONS(iRow,pRow);
		// 27. USAGE_PRIVILEGES
		else if(IsEqualGUID(m_guid,DBSCHEMA_USAGE_PRIVILEGES))
 			return VerifyRow_USAGE_PRIVILEGES(iRow,pRow);
		// 28. VIEW_COLUMN_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_VIEW_COLUMN_USAGE))
 			return VerifyRow_VIEW_COLUMN_USAGE(iRow,pRow);
		// 29. VIEW_TABLE_USAGE
		else if(IsEqualGUID(m_guid,DBSCHEMA_VIEW_TABLE_USAGE))
 			return VerifyRow_VIEW_TABLE_USAGE(iRow,pRow);
		// 30. VIEWS
		else if(IsEqualGUID(m_guid,DBSCHEMA_VIEWS))
 			return VerifyRow_VIEWS(iRow,pRow);
		else {
			odtLog << L"Warning - Provider specific schema rowset found.\n";
			return FALSE;
		}

	return FALSE;
}
//--------------------------------------------------------------------
// Capture Restrictions from schema rowsets returned, if schema is 
// supported
//
//--------------------------------------------------------------------
BOOL CSchemaTest::CaptureRestrictions()
{
	// Need to turn Capture off
	m_fCaptureRestrictions = TRUE;

	Find_Catalog_and_Schema();
	Find_TableInfo();
	Find_Assertion_Constraint();
	Find_Character_Set();
	Find_Collation();
	Find_Domain();
	Find_Grantor_and_Grantee();
	Find_Key_Column_Usage_Constraint();
	Find_Procedure(); 

	//I'm wrapping all procedure stuff into one place
	//Find_ProcedureColumn();
	//Find_Parameter();
	Find_Referential_Constraint();
	Find_Constraint_Type();
	Find_Translation();
	Find_View();
	Find_BestMatch();
	Find_PK_and_FK();

	// Need to turn Capture off
	m_fDontCaptureRestrictions = TRUE;
	m_fCaptureRestrictions=FALSE;

	return TRUE;
}
//--------------------------------------------------------------------
// Free Restrictions that were captured
//--------------------------------------------------------------------
void CSchemaTest::FreeRestrictions()
{
	Release_Catalog_and_Schema();
	Release_TableInfo();
	Release_Assertion_Constraint();
	Release_Character_Set();
	Release_Collation();
	Release_Domain();
	Release_Grantor_and_Grantee();
	Release_Key_Column_Usage_Constraint();
	Release_Procedure();
	Release_Parameter();
	Release_Referential_Constraint();
	Release_Constraint_Type();
	Release_Translation();
	Release_View();
	Release_PK_and_FK();

	// Need to Capture the Restrictions
	m_fDontCaptureRestrictions = FALSE;
}

//--------------------------------------------------------------------
// Is this schema supported
//--------------------------------------------------------------------
BOOL CSchemaTest::IsSchemaSupported(GUID schema)
{
	// grab schema from list of supported schemas
	for(ULONG index=0; index<m_cSchemasSupported; index++)
	{
		if(IsEqualGUID(schema,m_rgSchemas[index]))
			return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------
// Find Catalog
//
// With DBSCHEMA_CATALOGS, will fill m_pwszCatalogRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Catalog_and_Schema()
{
	// Initialize the needed pointers
	INIT;

	// Call the GetProperty for the CurrentCatalog
	GetProperty(DBPROP_CURRENTCATALOG, DBPROPSET_DATASOURCE, m_pIDBInitialize, &m_pwszCatalogRestriction);

	// Call the GetProperty for the UserName
	GetProperty(DBPROP_USERNAME, DBPROPSET_DATASOURCEINFO, m_pIDBInitialize, &m_pwszSchemaRestriction);

	FREE;
	
	return TRUE;
}
//--------------------------------------------------------------------
// Find Catalog
//
// With DBSCHEMA_CATALOGS, will fill m_pwszCatalogRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Catalog_and_Schema()
{
	// Free the memory
	PROVIDER_FREE(m_pwszCatalogRestriction);
	PROVIDER_FREE(m_pwszSchemaRestriction);

	return TRUE;
}
//--------------------------------------------------------------------
// Find Tables
//
// With DBSCHEMA_TABLES, will fill m_pwszTableRestriction and m_pwszTable_TypeRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_TableInfo()
{
	// Initialize the needed pointers
	INIT;

	// Get Index Name
	if(m_pTable->GetIndexName()) {
		PROVIDER_FREE(m_pwszIndexRestriction);
		m_pwszIndexRestriction = wcsDuplicate(m_pTable->GetIndexName());
	}

	// Get Table Name
	if(m_pTable->GetTableName()) {
		VARIANT Variant;
		PROVIDER_FREE(m_pwszTableRestriction);
		m_pwszTableRestriction = wcsDuplicate(m_pTable->GetTableName());
		m_pwszTable_TypeRestriction = wcsDuplicate(L"TABLE");

		// Adjust for identifier case
		VariantInit(&Variant);
		GetProperty(DBPROP_IDENTIFIERCASE, 
			DBPROPSET_DATASOURCEINFO, m_pIDBInitialize, &Variant);
		switch(Variant.lVal)
		{
			case DBPROPVAL_IC_UPPER:
				_wcsupr(m_pwszTableRestriction);
				break;
			case DBPROPVAL_IC_LOWER:
				_wcslwr(m_pwszTableRestriction);
				break;
		}

	}
	
	// Get Column Name
	CCol col;
	ULONG m_cOrdinals = m_pTable->CountColumnsOnTable();

	for(ULONG ulIndex = 1; m_cOrdinals >= ulIndex; ulIndex++)
	{
		if(SUCCEEDED(m_pTable->GetColInfo(1,col)) && col.GetColName()) 
		{
			// Copy the new values
			m_DataTypeRestriction = col.GetProviderType();

			PROVIDER_FREE(m_pwszColumnRestriction);
			m_pwszColumnRestriction = wcsDuplicate(col.GetColName());
			break;
		}
	}

	FREE;

	return TRUE;
}
//--------------------------------------------------------------------
// Release Tables
//
// With DBSCHEMA_TABLES, will free m_pwszTableRestriction, m_pwszColumnRestriction, and m_pwszIndexRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_TableInfo()
{
	// Free the memory
	PROVIDER_FREE(m_pwszTableRestriction);
	PROVIDER_FREE(m_pwszTable_TypeRestriction);
	PROVIDER_FREE(m_pwszColumnRestriction);
	PROVIDER_FREE(m_pwszIndexRestriction);
	return TRUE;
}

//--------------------------------------------------------------------
// Find Assertion Constraint
//
// With DBSCHEMA_ASSERTIONS, will fill m_pwszAssertion_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Assertion_Constraint()
{
	// Initialize the needed pointers
	INIT;

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_ASSERTIONS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
 	m_iid = IID_IRowset;

	// If this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_ASSERTIONS))
		GetRowset();

	FREE;

	return TRUE;
}
//--------------------------------------------------------------------
// Find Assertion Constraint
//
// With DBSCHEMA_ASSERTIONS, will fill m_pwszAssertion_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Assertion_Constraint()
{
	// Free the memory
	PROVIDER_FREE(m_pwszAssertion_ConstraintRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Character Set
//
// With DBSCHEMA_CHARACTER_SETS, will fill m_pwszCharacter_SetRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Character_Set()
{
	// Initialize the needed pointers
	INIT;

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_CHARACTER_SETS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_CHARACTER_SETS))
		GetRowset();

	FREE;

	return TRUE;
}
//--------------------------------------------------------------------
// Find Character Set
//
// With DBSCHEMA_CHARACTER_SETS, will fill m_pwszCharacter_SetRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Character_Set()
{
	// Free the memory
	PROVIDER_FREE(m_pwszCharacter_SetRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Check Constraint
//
// With DBSCHEMA_CHECK_CONSTRAINTS, will fill m_pwszCheck_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Check_Constraint()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_CHECK_CONSTRAINTS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_CHECK_CONSTRAINTS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Check Constraint
//
// With DBSCHEMA_CHECK_CONSTRAINTS, will fill m_pwszCheck_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Check_Constraint()
{
	// Free the memory
	PROVIDER_FREE(m_pwszCheck_ConstraintRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Collation
//
// With DBSCHEMA_COLLATIONS, will fill m_pwszCollationRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Collation()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_COLLATIONS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_COLLATIONS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Collation
//
// With DBSCHEMA_COLLATIONS, will fill m_pwszCollationRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Collation()
{
	// Free the memory
	PROVIDER_FREE(m_pwszCollationRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Domain
//
// With DBSCHEMA_COLUMN_DOMAIN_USAGE, will fill m_pwszDomainRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Domain()
{
	INIT;

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_COLUMN_DOMAIN_USAGE))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= FOURTH;
 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_COLUMN_DOMAIN_USAGE))
		GetRowset();

	FREE;

	return TRUE;
}
//--------------------------------------------------------------------
// Find Domain
//
// With DBSCHEMA_COLUMN_DOMAIN_USAGE, will fill m_pwszDomainRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Domain()
{
	// Free the memory
	PROVIDER_FREE(m_pwszDomainRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Grantor and Grantee
//
// With DBSCHEMA_COLUMN_PRIVILEGES, will fill m_pwszGrantorRestriction and m_pwszGranteeRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Grantor_and_Grantee()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_COLUMN_PRIVILEGES))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= THIRD;
	m_restrict |= FOURTH;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_COLUMN_PRIVILEGES))
	{
		// Since we're restricting COLUMN_PRIVILEGES to a single column
		// we need to adjust expected row count to one row minimum
		SetRowCount(MIN_REQUIRED, 1);

		GetRowset();
	}

	FREE

	return TRUE;

}
//--------------------------------------------------------------------
// Find Grantor and Grantee
//
// With DBSCHEMA_COLUMN_PRIVILEGES, will fill m_pwszGrantorRestriction and m_pwszGranteeRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Grantor_and_Grantee()
{
	// Free the memory
	PROVIDER_FREE(m_pwszGrantorRestriction);
	PROVIDER_FREE(m_pwszGranteeRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Key Column Usage Constraint
//
// With DBSCHEMA_KEY_COLUMN_USAGE, will fill m_pwszKey_Column_Usage_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Key_Column_Usage_Constraint()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_KEY_COLUMN_USAGE))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= FOURTH;
	m_restrict |= FIFTH;
	m_restrict |= SIXTH;
	m_restrict |= SEVENTH;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_KEY_COLUMN_USAGE))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Key Column Usage Constraint
//
// With DBSCHEMA_KEY_COLUMN_USAGE, will fill m_pwszKey_Column_Usage_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Key_Column_Usage_Constraint()
{
	// Free the memory
	PROVIDER_FREE(m_pwszKey_Column_Usage_ConstraintRestriction);
	return TRUE;

}
//--------------------------------------------------------------------
// Find Procedure
//
// With DBSCHEMA_PROCEDURE, will fill in 
// m_pwszProcedureRestriction and m_pwszProcedure_Type, 
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Procedure()
{
	BOOL			fSucceed		= FALSE;
	ICommandText *	pICommandText	= NULL;
	WCHAR *			pwszProcName	= NULL;
	WCHAR *			pSQL			= NULL;
	WCHAR *			pwszDBMSName	= NULL;

	// Initialize the needed pointers
	INIT;
	
	if(!m_pIDBInitialize) {
		odtLog << L"m_pIDBInitialize is null\n";
		return TEST_FAIL;
	}

	// Call the GetProperty for the DBMS Name
	if( (GetProperty(DBPROP_DBMSNAME, DBPROPSET_DATASOURCEINFO, m_pIDBInitialize, &pwszDBMSName)) &&
		(!wcscmp(pwszDBMSName, L"Microsoft SQL Server")) )
		m_bSqlServer = TRUE;
	else
	{
		odtLog << L"Not a sql server so can't test stored proc restrictions\n";
		goto CLEANUP;
	}

	if(!(m_pTable->m_pICommand))
		goto CLEANUP;

	if(!CHECK(m_pTable->m_pICommand->QueryInterface(IID_ICommandText,(void**)&pICommandText),S_OK))
		goto CLEANUP;

	pwszProcName = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(m_pTable->GetTableName()) +
		wcslen(wszProcDesignator)+1) * sizeof(WCHAR));

	if(!pwszProcName) {
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}

	wcscpy(pwszProcName, m_pTable->GetTableName());
	wcscat(pwszProcName, wszProcDesignator);

	m_pwszDropProc = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(wszDropProc) +
		wcslen(pwszProcName)+1) * sizeof(WCHAR));

	if(!m_pwszDropProc) {
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}

	swprintf(m_pwszDropProc, wszDropProc, pwszProcName);

	pSQL = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(wszCreateProc) + 
		wcslen(pwszProcName) +
		wcslen(m_pTable->GetTableName()) +
		1) * sizeof(WCHAR));

	if(!pSQL)
	{
		COMPARE(pSQL,NULL);
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}

	swprintf(pSQL, wszCreateProc, pwszProcName, m_pTable->GetTableName());

//CREATE PROCEDURE <procname> (@IntIn int =1, @IntOut int =2 OUTPUT) AS Select * from %s RETURN (1)";
// will give me:
//	1 input
//	1 output
//	1 return

	if(!CHECK(pICommandText->SetCommandText(DBGUID_DBSQL,m_pwszDropProc),S_OK))
		goto CLEANUP;

	pICommandText->Execute(NULL,IID_NULL,NULL,NULL,NULL);

	if(!CHECK(pICommandText->SetCommandText(DBGUID_DBSQL,pSQL),S_OK))
		goto CLEANUP;

	if(!CHECK(pICommandText->Execute(NULL,IID_NULL,NULL,NULL,NULL),S_OK))
		goto CLEANUP;

	// fill these in
	m_pwszProcedureRestriction  = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(pwszProcName) +
		1) * sizeof(WCHAR*));

	if(!m_pwszProcedureRestriction)
	{
		COMPARE(m_pwszProcedureRestriction,NULL);
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}
		
	swprintf(m_pwszProcedureRestriction, pwszProcName);

	//WCHAR * m_pwszProcedureColumnsRestriction;
	// since we are doing select * from table we can use the 
	// column we found previously
	m_pwszProcedureColumnsRestriction  = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(m_pwszColumnRestriction) +
		1) * sizeof(WCHAR*));

	if(!m_pwszProcedureColumnsRestriction)
	{
		COMPARE(m_pwszProcedureColumnsRestriction,NULL);
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}
		
	swprintf(m_pwszProcedureColumnsRestriction, m_pwszColumnRestriction);

	//WCHAR * m_pwszParameterRestriction;
	m_pwszParameterRestriction  = (WCHAR *) PROVIDER_ALLOC(
		(wcslen(L"IntIn") +
		1) * sizeof(WCHAR*));

	if(!m_pwszParameterRestriction)
	{
		COMPARE(m_pwszParameterRestriction,NULL);
		odtLog << L"out of memory\n";
		goto CLEANUP;
	}
		
	swprintf(m_pwszParameterRestriction, L"IntIn");
	
	
	//WCHAR *	m_pwszProcedureType;
	// default will be return value but I'll have
	// to add more variations in the Procedure Parameter class
	// to test all 4 types.
	m_ProcedureTypeRestriction = DBPARAMTYPE_RETURNVALUE;
	m_uiProcedureType = DBPARAMTYPE_RETURNVALUE;

	fSucceed = TRUE;


CLEANUP:

	FREE;
	
	SAFE_RELEASE(pICommandText);

	// Free the memory
	PROVIDER_FREE(pSQL);
	PROVIDER_FREE(pwszProcName);
	PROVIDER_FREE(pwszDBMSName);
	
	return fSucceed;
}
//--------------------------------------------------------------------
// Find Procedure
//
// With DBSCHEMA_PROCEDURE, will fill in m_pwszProcedureRestriction and m_pwszProcedure_Type
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Procedure()
{
	BOOL fSucceed=FALSE;
	
	ICommandText * pICommandText=NULL;

	if(m_bSqlServer)
	{

		if(!(m_pTable->m_pICommand))
			goto CLEANUP;

		if(!CHECK(m_pTable->m_pICommand->QueryInterface(IID_ICommandText,(void**)&pICommandText),S_OK))
			goto CLEANUP;

		if (m_pwszDropProc)
		{
			if(!CHECK(pICommandText->SetCommandText(DBGUID_DBSQL,m_pwszDropProc),S_OK))
				goto CLEANUP;

			if(!CHECK(pICommandText->Execute(NULL,IID_NULL,NULL,NULL,NULL),S_OK))
				goto CLEANUP;
		}

	CLEANUP:

		SAFE_RELEASE(pICommandText);

		// Free the memory
		PROVIDER_FREE(m_pwszDropProc);
		PROVIDER_FREE(m_pwszProcedureRestriction);
		PROVIDER_FREE(m_pwszProcedureColumnsRestriction);
		PROVIDER_FREE(m_pwszParameterRestriction);
	}

	return TRUE;
}
//--------------------------------------------------------------------
// Find Find_ProcedureColumn
//
// With DBSCHEMA_PROCEDURE_COLUMNS, will fill in 
// m_pwszProcedureColumnRestriction 
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_ProcedureColumn()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_PROCEDURE_COLUMNS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= THIRD;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_PROCEDURE_COLUMNS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Procedure
//
// With DBSCHEMA_PROCEDURE, will fill in m_pwszProcedureRestriction and m_pwszProcedure_Type
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_ProcedureColumn()
{
	// Free the memory
	PROVIDER_FREE(m_pwszProcedureColumnsRestriction);
	return TRUE;
}

//--------------------------------------------------------------------
// Find Parameter
//
// With DBSCHEMA_PROCEDURE_PARAMETER, will fill in m_pwszParameterRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Parameter()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_PROCEDURE_PARAMETERS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= THIRD;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_PROCEDURE_PARAMETERS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Parameter
//
// With DBSCHEMA_PROCEDURE_PARAMETER, will fill in m_pwszParameterRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Parameter()
{
	// Free the memory
	PROVIDER_FREE(m_pwszParameterRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Referential Constraint
//
// With DBSCHEMA_REFERENTIAL_CONSTRAINTS, will fill m_pwszReferential_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Referential_Constraint()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_REFERENTIAL_CONSTRAINTS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_REFERENTIAL_CONSTRAINTS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Referential Constraint
//
// With DBSCHEMA_REFERENTIAL_CONSTRAINTS, will fill m_pwszReferential_ConstraintRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Referential_Constraint()
{
	// Free the memory
	PROVIDER_FREE(m_pwszReferential_ConstraintRestriction);
	return TRUE;

}
//--------------------------------------------------------------------
// Find Table Constraint
//
// With DBSCHEMA_TABLE_CONSTRAINTS, will fill m_pwszTable_ConstraintRestriction
// and m_pwszTable_Constraint_Type
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Constraint_Type()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_TABLE_CONSTRAINTS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= FOURTH;
	m_restrict |= FIFTH;
	m_restrict |= SIXTH;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_TABLE_CONSTRAINTS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Table Constraint
//
// With DBSCHEMA_TABLE_CONSTRAINTS, will fill m_pwszTable_ConstraintRestriction
// and m_pwszTable_Constraint_Type
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Constraint_Type()
{
	// Free the memory
	PROVIDER_FREE(m_pwszTable_ConstraintRestriction);
	PROVIDER_FREE(m_pwszConstraint_TypeRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find Translation
//
// With DBSCHEMA_TRANSLATIONS, will fill in m_pwszTranslationReplace
//
// While the translation should be the same as the 
// character set, I'm not taking any chances
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_Translation()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_TRANSLATIONS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_TRANSLATIONS))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find Translation
//
// With DBSCHEMA_TRANSLATIONS, will fill in m_pwszTranslationReplace
//
// While the translation should be the same as the 
// character set, I'm not taking any chances
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_Translation()
{
	// Free the memory
	PROVIDER_FREE(m_pwszTranslationReplace);
	return TRUE;

}
//--------------------------------------------------------------------
// Find View
//
// With DBSCHEMA_VIEW_TABLE_USAGE, will fill in m_pwszViewRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_View()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_VIEW_TABLE_USAGE))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;

 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_VIEW_TABLE_USAGE))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find View
//
// With DBSCHEMA_VIEW_TABLE_USAGE, will fill in m_pwszViewRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_View()
{
	// Free the memory
	PROVIDER_FREE(m_pwszViewRestriction);
	return TRUE;
}
//--------------------------------------------------------------------
// Find BestMatch for DBSCHEMA_PROVIDER_TYPES
//
// With DBSCHEMA_VIEW_TABLE_USAGE, will fill in m_pwszViewRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_BestMatch()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_PROVIDER_TYPES))
		return FALSE;

	m_restrict |= FIRST;
 	m_iid = IID_IRowset;

	// if this is a schema I want then get information for schema
	if(GetSchemaInfo(SPECIFIC,0,DBSCHEMA_PROVIDER_TYPES))
		GetRowset();

	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find BestMatch for DBSCHEMA_PROVIDER_TYPES
//
// With DBSCHEMA_FOREIGN_KEYS, 		
// m_pwszPK_TableRestriction, m_pwszFK_TableRestriction;
//--------------------------------------------------------------------
BOOL CSchemaTest::Find_PK_and_FK()
{
	INIT

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	if(!IsSchemaSupported(DBSCHEMA_FOREIGN_KEYS))
		return FALSE;

	m_restrict |= FIRST;
	m_restrict |= SECOND;
	m_restrict |= FOURTH;
	m_restrict |= FIFTH;

 	m_iid = IID_IRowset;

	if(m_fCaptureRestrictions)
	{
		// PK TABLE_NAME
		if ((g_pKeyTable2) && (g_pKeyTable2->GetTableName()))
		{
			m_pwszPK_TableRestriction = (WCHAR *) PROVIDER_ALLOC
				((wcslen((WCHAR *) g_pKeyTable2->GetTableName())*sizeof(WCHAR)) + sizeof(WCHAR));
			
			if(m_pwszPK_TableRestriction)
				wcscpy(m_pwszPK_TableRestriction,(TYPE_WSTR) g_pKeyTable2->GetTableName());
		}

		if ((g_pKeyTable1) && (g_pKeyTable1->GetTableName()))
		{
			// FK TABLE_NAME
			m_pwszFK_TableRestriction = (WCHAR *) PROVIDER_ALLOC
				((wcslen((WCHAR *) g_pKeyTable1->GetTableName())*sizeof(WCHAR)) + sizeof(WCHAR));
			if(m_pwszFK_TableRestriction)
				wcscpy(m_pwszFK_TableRestriction,(TYPE_WSTR)g_pKeyTable1->GetTableName());
		}
		
	}


	FREE

	return TRUE;
}
//--------------------------------------------------------------------
// Find BestMatch for DBSCHEMA_PROVIDER_TYPES
//
// With DBSCHEMA_VIEW_TABLE_USAGE, will fill in m_pwszViewRestriction
//--------------------------------------------------------------------
BOOL CSchemaTest::Release_PK_and_FK()
{
	// Free the memory
	PROVIDER_FREE(m_pwszPK_TableRestriction);
	PROVIDER_FREE(m_pwszFK_TableRestriction);
	return TRUE;
}

//--------------------------------------------------------------------
// ASSERTIONS
// 1. Constraint Catalog
// 2. Constraint Schema
// 3. Constraint Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_ASSERTIONS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **) rgwszASSERTIONS;
	m_rgColumnTypes = (DBTYPE *) rgtypeASSERTIONS;
	
	// Set the count of columns and restrictions
	m_cColumns = cASSERTIONS;
	m_cRestrictions = cASSERTIONS_RESTRICTIONS;

	// Set Constraint Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszAssertion_ConstraintRestriction);
	
	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowASSERTIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_ASSERTIONS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszAssertion_ConstraintRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cASSERTIONS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:ASSERTIONS:", iRow,m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// CONSTRAINT CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR) pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_ASSERTIONS:CONSTRAINT CATALOG restriction failed\n";
							m_fRes1 = FALSE;
							fResults = FALSE;
						}
					}
				}

				PRVTRACE(L"= %s\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2://CONSTRAINT SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR) pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_ASSERTIONS:CONSTRAINT SCHEMA restriction failed\n";
							m_fRes1 = FALSE;
							fResults = FALSE;
						}
					}
				}

				PRVTRACE(L"= %s\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3://CONSTRAINT NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR) pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_ASSERTIONS:CONSTRAINT NAME restriction failed\n";
							m_fRes3 = FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszAssertion_ConstraintRestriction = (TYPE_WSTR) PROVIDER_ALLOC
						((wcslen((TYPE_WSTR) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszAssertion_ConstraintRestriction)
						wcscpy(m_pwszAssertion_ConstraintRestriction,(TYPE_WSTR) pColumn->bValue);
				}

				PRVTRACE(L"= %s\n",(WCHAR *)pColumn->bValue);
				break;
			case 4: //IS_DEFERRABLE
			case 5: //INITIALLY_DEFERRED
				if(*(TYPE_BOOL *)pColumn->bValue==VARIANT_TRUE)
					PRVTRACE(L"TRUE\n");
				else
					PRVTRACE(L"FALSE\n");
				break;
			case 6: // DESCRIPTION
					PRVTRACE(L"= %s\n",(TYPE_WSTR)pColumn->bValue );
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"ASSERTIONS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - ASSERTIONS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
				
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// CATALOGS
// 1. Catalog Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_CATALOGS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **) rgwszCATALOGS;
	m_rgColumnTypes = (DBTYPE *) rgtypeCATALOGS;
	
	// Set the count of columns and restrictions
	m_cColumns = cCATALOGS;
	m_cRestrictions = cCATALOGS_RESTRICTIONS;

	// Set Catalog Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCATALOGS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_CATALOGS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCATALOGS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"CATALOGS:Row[%lu],Col[%d,%s]:", 
			iRow, 
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// CATALOG_NAME
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR) pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_CATALOGS:CATALOG_NAME restriction failed\n";
							m_fRes1 = FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"= %s\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2://DESCRIPTION
				PRVTRACE(L"= %s\n",(TYPE_WSTR)pColumn->bValue );
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"CATALOGS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - CATALOGS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
			
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
	}
	return fResults;
}

//--------------------------------------------------------------------
// CHARACTER_SETS
// 1. Character Set Catalog 
// 2. Character Set Schema
// 3. Character Set Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_CHARACTER_SETS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCHARACTER_SETS;
	m_rgColumnTypes = (DBTYPE *)rgtypeCHARACTER_SETS;

	// Set the count of columns and restrictions
	m_cColumns = cCHARACTER_SETS;
	m_cRestrictions = cCHARACTER_SETS_RESTRICTIONS;

	// Set Character Sets Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszCharacter_SetRestriction);

	PRVTRACE(L"GetSchemaInfo::CHARACTER_SETS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCHARACTER_SETS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_CHARACTER_SETS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_fCaptureRestrictions)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCHARACTER_SETS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:CHARACTER_SETS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// CHARACTER SET CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{	
							odtLog << L"VerifyRow_CHARACTER_SETS:CHARACTER SET CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// CHARACTER SET SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_CHARACTER_SETS:CHARACTER SET SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:// CHARACTER_SET_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_CHARACTER_SETS:CHARACTER SET NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszCharacter_SetRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszCharacter_SetRestriction)
						wcscpy(m_pwszCharacter_SetRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}
				
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// FORM_OF_USE
			case 6:// DEFAULT_COLLATE_CATALOG
			case 7:// DEFAULT_COLLATE_SCHEMA
			case 8:// DEFAULT_COLLATE_NAME
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// NUMBER OF CHARACTERS
				PRVTRACE(L"%d\n",*(TYPE_I8 *)pColumn->bValue);
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"CHARACTER SETS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - CHARACTER SETS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// CHECK_CONSTRAINTS
// 1. Constraint Catalog
// 2. Constraint Schema
// 3. Constraint Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_CHECK_CONSTRAINTS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCHECK_CONSTRAINTS;
	m_rgColumnTypes = (DBTYPE *)rgtypeCHECK_CONSTRAINTS;

	// Set the count of columns and restrictions
	m_cColumns = cCHECK_CONSTRAINTS;
	m_cRestrictions = cCHECK_CONSTRAINTS_RESTRICTIONS;
	
	// Set Constraint Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszCheck_ConstraintRestriction);

	PRVTRACE(L"GetSchemaInfo::CHECK_CONSTRAINTS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCHECK_CONSTRAINTS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_CHECK_CONSTRAINTS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszCheck_ConstraintRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCHECK_CONSTRAINTS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:CHECK_CONSTRAINTS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// CHECK_CONSTRAINTS CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{	
							odtLog << L"VerifyRow_CHECK_CONSTRAINTS:CHECK_CONSTRAINTS CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// CHECK_CONSTRAINTS SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog <<  L"VerifyRow_CHECK_CONSTRAINTS:CHECK_CONSTRAINTS SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:// CHECK_CONSTRAINTS NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<L"VerifyRow_CHECK_CONSTRAINTS:CHECK_CONSTRAINTS NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszCheck_ConstraintRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszCheck_ConstraintRestriction)
						wcscpy(m_pwszCheck_ConstraintRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// CHECK_CLAUSE
			case 5:// DESCRIPTION
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"CHECK CONSTRAINTS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - CHECK CONSTRAINTS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// COLLATIONS
// 1. Collation Catalog
// 2. Collation Schema
// 3. Collation Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_COLLATIONS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCOLLATIONS;
	m_rgColumnTypes = (DBTYPE *)rgtypeCOLLATIONS;

	// Set the count of columns and restrictions
	m_cColumns = cCOLLATIONS;
	m_cRestrictions = cCOLLATIONS_RESTRICTIONS;
	
	// Set Collation Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszCollationRestriction);

	PRVTRACE(L"GetSchemaInfo::COLLATIONS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCOLLATIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_COLLATIONS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;
    
	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszCollationRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCOLLATIONS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:COLLATIONS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// COLLATIONS CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{	
							odtLog <<  L"VerifyRow_COLLATIONS:COLLATIONS CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// COLLATIONS SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_COLLATIONS:COLLATIONS SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:// COLLATIONS_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_COLLATIONS:COLLATIONS NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszCollationRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszCollationRestriction)
						wcscpy(m_pwszCollationRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// CHARACTER SET CATALOG
			case 5:// CHARACTER SET SCHEMA
			case 6:// CHARACTER SET NAME
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 7:// PAD ATTRIBUTE
				if( (!COMPARE(0, wcscmp((TYPE_WSTR)pColumn->bValue,L"NO PAD")) && (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"PAD SPACE"))))
				{
					odtLog << L"VerifyRow_COLLATIONS:PAD ATTRIBUTE expected NO PAD or PAD SPACE but recieved " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"COLLATIONS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - COLLATIONS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// COLUMN_DOMAIN_USAGE
// 1. Domain Catalog
// 2. Domain Schema
// 3. Domain Name
// 4. Column Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_COLUMN_DOMAIN_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCOLUMN_DOMAIN_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeCOLUMN_DOMAIN_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cCOLUMN_DOMAIN_USAGE;
	m_cRestrictions = cCOLUMN_DOMAIN_USAGE_RESTRICTIONS;
	
	// Set Domain Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszDomainRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszColumnRestriction);

	PRVTRACE(L"GetSchemaInfo::COLUMN_DOMAIN_USAGE\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowDOMAIN_COLUMN_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_COLUMN_DOMAIN_USAGE(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszDomainRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCOLUMN_DOMAIN_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:COLUMN_DOMAIN_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// DOMAIN CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{	
							odtLog << L"VerifyRow_COLUMN_DOMAIN_USAGE:DOMAIN CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// DOMAIN SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog <<  L"VerifyRow_COLUMN_DOMAIN_USAGE:DOMAIN SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:// DOMAIN_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_COLUMN_DOMAIN_USAGE:DOMAIN NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszDomainRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszDomainRestriction)
						wcscpy(m_pwszDomainRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE CATALOG
			case 5:// TABLE SCHEMA
			case 6:// TABLE NAME
			case 7:// COLUMN NAME
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 8:// COLUMN GUID
				PRVTRACE(L"\n");
			case 9:// COLUMN PROPID
				PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"COLUMN DOMAIN USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - COLUMN DOMAIN USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
				// Have to flag error.
				odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
				fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// COLUMN_PRIVILEGES
// 1. Table Catalog
// 2. Table Schema 
// 3. Table Name
// 4. Column Name
// 5. Grantor
// 6. Grantee
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_COLUMN_PRIVILEGES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCOLUMN_PRIVILEGES;
	m_rgColumnTypes = (DBTYPE *)rgtypeCOLUMN_PRIVILEGES;

	// Set the count of columns and restrictions
	m_cColumns = cCOLUMN_PRIVILEGES;
	m_cRestrictions = cCOLUMN_PRIVILEGES_RESTRICTIONS;
	
	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszColumnRestriction);
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszGrantorRestriction);
	SetRestriction(SIXTH, 6, &m_wszR6, m_pwszGranteeRestriction);

	// Set expected row count.  Since we create a table and we know 
	// how many columns there are we must have at least that number of 
	// rows, unless restricted to a given column.
	if (!m_wszR4)
		SetRowCount(MIN_REQUIRED, m_pTable->CountColumnsOnTable());

	PRVTRACE(L"GetSchemaInfo::COLUMN_PRIVILEGES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCOLUMN_PRIVILEGES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_COLUMN_PRIVILEGES(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
    DATA *	pColumn=NULL;	// Data Structure
	CCol	col;
	BOOL	fResults=TRUE;

	int test=0;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszGrantorRestriction && m_pwszGranteeRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCOLUMN_PRIVILEGES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"COLUMN_PRIVILEGES:Row[%lu],Col[%d,%s]:", 
			iRow,
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// GRANTOR
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{	
							odtLog << L"VerifyRow_COLUMN_PRIVILEGES:GRANTOR 5 restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszGrantorRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszGrantorRestriction)
						wcscpy(m_pwszGrantorRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}

				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// GRANTEE
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog <<  L"VerifyRow_COLUMN_PRIVILEGES:GRANTEE 6 restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				if(m_fCaptureRestrictions)
				{
					m_pwszGranteeRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszGranteeRestriction)
						wcscpy(m_pwszGranteeRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:// TABLE_CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog <<  L"VerifyRow_COLUMN_PRIVILEGES:TABLE_CATALOG 1 restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE_SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_COLUMN_PRIVILEGES:TABLE_SCHEMA 2 restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// TABLE_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_COLUMN_PRIVILEGES:TABLE_NAME 3 restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 6:// COLUMN_NAME
				if(m_restrict & FOURTH)
				{
					test=wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue);
					
					if(!COMPARE(0, wcsicmp((WCHAR *)m_wszR4,(WCHAR *)pColumn->bValue)))
					{
						if(m_fRes4)
						{
							odtLog <<  L"VerifyRow_COLUMN_PRIVILEGES:COLUMN_NAME 4 restriction failed\n";
							m_fRes4 = FALSE;
							fResults = FALSE;
						}

					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 7:// COLUMN_GUID
				// TODO: Can I do anything interesting with a GUID?
				PRVTRACE(L"\n");
				break;
			case 8:// COLUMN_PROPID
				PRVTRACE(L"%ul\n",*(TYPE_UI4 *)pColumn->bValue);
				break;
			case 9:// PRIVILEGE_TYPE
				if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SELECT")) && 
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"DELETE")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"INSERT")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"UPDATE")) &&
 					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"REFERENCES")))
				{
					odtLog << L"VerifyRow_COLUMN_PRIVILEGES:PRIVILEGE_TYPE expected SELECT,DELETE,INSERT,UPDATE, or REFERENCES but received " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 10:// IS_GRANTABLE
				if((*(TYPE_BOOL *)pColumn->bValue)==VARIANT_TRUE)
					PRVTRACE(L"YES\n");
				else
					PRVTRACE(L"NO\n");
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"COLUMN PRIVILEGES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - COLUMN PRIVILEGES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}	
		else
		{
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}


	return fResults;
}

//--------------------------------------------------------------------
// COLUMNS
// 1. Table Catalog
// 2. Table Schema 
// 3. Table Name
// 4. Column Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_COLUMNS()
{	
	// Set the Schema column Names and Types
	m_cColumns = cCOLUMNS;
	m_rgColumnNames = (WCHAR **)rgwszCOLUMNS;
	m_rgColumnTypes = (DBTYPE *)rgtypeCOLUMNS;

	// Set the count of columns and restrictions
	m_cRestrictions = cCOLUMNS_RESTRICTIONS;

	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszColumnRestriction);

	// Set an Invalid Table Restriction
	if(m_restrict & FIFTH) {
		SetRestriction(FIFTH,5, &m_wszR5, m_pwszTableRestriction);
		m_cRestrictions = 5;
	}

	// Set expected row count.  Since we create a table and we know 
	// how many columns there are we must have at least that number of 
	// rows
	SetRowCount(MIN_REQUIRED, m_pTable->CountColumnsOnTable());

	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCOLUMNS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_COLUMNS(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind=0;		// Binding Count
	ULONG	iIndex = 0;		// Index into rgColInfo array
	CCol	col;
	BOOL	fResults=TRUE;
	ULONG	cColumns = 0;
	ULONG	iOrdinalBinding = 6+!m_rgDBCOLUMNINFO[0].iOrdinal;

	// Per spec, the columns rowset must list columns a user has access to, therefore the user
	// must have access to the associated table.  Retrieve columns info for the table if this is a new
	// table.
	if (S_OK != GetColumnInfo(pData, m_rgDBBINDING))
		return FALSE;

	// If we don't have an m_prgColInfo array, then GetColumnInfo returns S_FALSE above.

	// Make our best guess of the index
	iIndex = m_ulTableOrdinal-1+!m_prgColInfo[0].iOrdinal;

	// If the provider returned the data out of order we have to find the ordinal
	ASSERT(m_rgDBBINDING[iOrdinalBinding].iOrdinal == 7);
	if (STATUS_BINDING(m_rgDBBINDING[iOrdinalBinding], pData) == DBSTATUS_S_OK)
	{
		ULONG iOrdinal = (ULONG)VALUE_BINDING(m_rgDBBINDING[iOrdinalBinding], pData);

		// If this isn't the right index ordinals won't match
		if (m_prgColInfo[iIndex].iOrdinal != iOrdinal)
		{
			// Find the ordinal value in m_prgColInfo array as this row may not be in order
			for (ULONG iColInfo = 0; iColInfo < m_cColumns; iColInfo++)
			{
				if (m_prgColInfo[iColInfo].iOrdinal == iOrdinal)
				{
					iIndex = iColInfo;
					break;
				}
			}
		}
	}

	// Make sure we don't crash, but this will cause lots of errors
	if (!COMPARE(iIndex <= m_cColInfo - !m_prgColInfo[0].iOrdinal, TRUE))
		iIndex = 0;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCOLUMNS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		DBBINDING * pBind = &m_rgDBBINDING[iBind];
		ULONG iOrdinal = m_rgDBCOLUMNINFO[iBind].iOrdinal;
		DBSTATUS ulStatus = STATUS_BINDING(*pBind, pData);
	    DATA *	pColumn=(DATA *) (pData + pBind->obStatus);
		BYTE * pValue = pColumn->bValue;

		switch(iOrdinal)
		{
			// TABLE_CATALOG
			case 1:
				fResults &= TestReturnData(iRow,pColumn,FIRST,&m_fRes1,m_wszR1);
				break;

			// TABLE_SCHEMA
			case 2:
				fResults &= TestReturnData(iRow,pColumn,SECOND,&m_fRes2,m_wszR2);
				break;

			// TABLE_NAME
			case 3:
				fResults &= TestReturnData(iRow,pColumn,THIRD,&m_fRes3,m_wszR3);
				break;

			// COLUMN_NAME
			case 4:
				fResults &= TestReturnData(iRow,pColumn,FOURTH,&m_fRes4,m_wszR4);
				break;

			case 5:	// COLUMN_GUID
				break;
			case 13:// TYPE_GUID
				break;
			case 6:	//COLUMN_PROPID
				break;
			case 7:	// ORDINAL_POSITION
				if (ulStatus == DBSTATUS_S_OK)
				{
					// Status is OK, warning if ordinals don't match
					// Providers are not required to return results in ordinal order, since
					// sort is only on catalog, schema, and name, so only a warning.
					// If restriction is only on column then the ordinal may not match anyway.
					if (!m_fRes4)
					{						
						COMPAREW(m_ulTableOrdinal, (ULONG)VALUE_BINDING(*pBind, pData));
						if (m_ulTableOrdinal != (ULONG)VALUE_BINDING(*pBind, pData))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Ordinal didn't match expected value.\n";
					}
					else
					{
						// We will assume if the index for the matching ordinal contains
						// the proper column name that it was the right ordinal.
						if (RelCompareString(m_wszR4, m_prgColInfo[iIndex].pwszName))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Ordinal didn't match expected value.\n";
					}
				}
				else if (ulStatus == DBSTATUS_S_ISNULL)
				{
					// Status is NULL.  While this is allowed, it's not normally the case, so warn
					COMPAREW(ulStatus, DBSTATUS_S_OK);
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"DBSTATUS_S_ISNULL returned.\n";
				}
				else
				{
					// Otherwise this is a failure. 
					if (!COMPARE(ulStatus, DBSTATUS_S_OK))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Invalid value returned.\n";
				}
				break;
			case 10:// COLUMN FLAGS
				// This column cannot contain NULL
				if (COMPARE(ulStatus, DBSTATUS_S_OK) &&
					!COMPARE(m_prgColInfo[iIndex].dwFlags, *(DBCOLUMNFLAGS *)pValue))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid value returned.\n";
				break;
			case 14:// CHARACTER_MAXIMUM_LENGTH
				switch(m_prgColInfo[iIndex].wType)
				{
//					case DBTYPE_BOOL:  // We assume BOOL is not a "bit" column per spec.
					case DBTYPE_BYTES:
					case DBTYPE_STR:
					case DBTYPE_WSTR:
						if (COMPARE(ulStatus, DBSTATUS_S_OK))
						{
							if (m_prgColInfo[iIndex].ulColumnSize == ~0 && 
								!COMPARE(*(ULONG *)pValue, 0))
								odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
									L"Must be 0 for column without defined max length.\n";
							else if (!COMPARE(*(ULONG *)pValue, m_prgColInfo[iIndex].ulColumnSize))
								odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
									L"Invalid value returned.\n";
						}
						else
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Cannot be NULL for variable length column.\n";
						break;
					// NULL for all other data types
					default:
						if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Status was not DBSTATUS_S_ISNULL for non-char, binary, or bit column\n";
				}
				break;
			case 15:// CHARACTER_OCTET_LENGTH
				switch(m_prgColInfo[iIndex].wType)
				{
					case DBTYPE_BYTES:
					case DBTYPE_STR:
					case DBTYPE_WSTR:
						if (COMPARE(ulStatus, DBSTATUS_S_OK))
						{
							if (m_prgColInfo[iIndex].ulColumnSize == ~0 && 
								!COMPARE(*(ULONG *)pValue, 0))
								odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
									L"Must be 0 for column without defined max length.\n";
							else 
							{
								ULONG ulOctetLen = m_prgColInfo[iIndex].ulColumnSize;
								if (m_prgColInfo[iIndex].wType == DBTYPE_WSTR)
									ulOctetLen *= sizeof(WCHAR);

								if (!COMPARE(*(ULONG *)pValue, ulOctetLen))
									odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
										L"Invalid value returned.\n";
							}
						}
						break;
					// NULL for all other data types
					default:
						if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Status was not DBSTATUS_S_ISNULL for non-char, binary, or bit column\n";
				}
				break;
			case 18:// DATETIME_PRECISION
				switch(m_prgColInfo[iIndex].wType)
				{
					case DBTYPE_DBTIMESTAMP:
						if (COMPARE(ulStatus, DBSTATUS_S_OK) && 
							!COMPARE(*(ULONG *)pValue, (ULONG)m_prgColInfo[iIndex].bScale))
								odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
									L"Invalid value returned.\n";
						break;
					// NULL for all other data types
					default:
						if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Status was not DBSTATUS_S_ISNULL for non-char, binary, or bit column\n";
				}
				break;
			case 8:	// COLUMN_HASDEFAULT
				// We can't actually try the default value except for our test table, so we don't know
				// if it actually would work or not for other tables.
				// Note that COLUMN_DEFAULT may be NULL even if HASDEFAULT is TRUE but the converse
				// is not true.
				
				if (COMPARE(ulStatus, DBSTATUS_S_OK))
				{

					// Make sure we know the proper binding for DBCOLUMN_DEFAULT
					ASSERT (m_rgDBBINDING[8+!m_rgDBCOLUMNINFO[0].iOrdinal].iOrdinal == 9);
					DBSTATUS ulDefaultStatus = STATUS_BINDING(m_rgDBBINDING[8+!m_rgDBCOLUMNINFO[0].iOrdinal], pData);

					// If the COLUMN_DEFAULT is non-NULL, then HASDEFAULT should be TRUE, otherwise
					// we're not sure
					if (S_OK == ulDefaultStatus && !COMPARE(*(VARIANT_BOOL *)pValue, VARIANT_TRUE))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Invalid value returned.\n";

					// TODO: If the current table is the automaketable for this test try to use the default
					// value and make sure it matches expected.
				}
				else
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid status returned.\n";
				break;
			case 9: // COLUMN DEFAULT
				if (ulStatus == DBSTATUS_S_OK)
				{
					// For now, just make sure the length is right so we read the default value
					if (!COMPARE(wcslen((WCHAR *)pValue)*sizeof(WCHAR), LENGTH_BINDING(m_rgDBBINDING[iBind], pData)))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Default value has the wrong length.\n";

					// TODO: If the current table is the automaketable for this test try to use the default
					// value and make sure it matches expected.  
				}
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid status returned.\n";
				break;
			case 11:// IS_NULLABLE
			{
				VARIANT_BOOL fNullable = m_prgColInfo[iIndex].dwFlags & DBCOLUMNFLAGS_ISNULLABLE ? VARIANT_TRUE : VARIANT_FALSE;
				if (!COMPARE(fNullable, *(VARIANT_BOOL *)pValue))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid value returned.\n";
					break;
			}
			case 12:// DATA_TYPE
				// This column cannot contain NULL
				if (COMPARE(ulStatus, DBSTATUS_S_OK) &&
					!COMPARE(m_prgColInfo[iIndex].wType,	*(DBTYPE *)pValue))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid value returned.\n";
				break;
			case 16:// NUMERIC_PRECISION
				if (IsNumericType(m_prgColInfo[iIndex].wType))
				{
					if (COMPARE(ulStatus, DBSTATUS_S_OK) && 
						!COMPARE(*(USHORT *)pValue, (USHORT)m_prgColInfo[iIndex].bPrecision))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Invalid value returned.\n";
				}
				// NULL for all other data types
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Status was not DBSTATUS_S_ISNULL for non-numeric column\n";
				break;
			case 17:// NUMERIC_SCALE
				if (m_prgColInfo[iIndex].wType == DBTYPE_DECIMAL ||
					m_prgColInfo[iIndex].wType == DBTYPE_NUMERIC ||
					m_prgColInfo[iIndex].wType == DBTYPE_VARNUMERIC)
				{
					if (COMPARE(ulStatus, DBSTATUS_S_OK))
					{
						SHORT sScale = (SHORT)m_prgColInfo[iIndex].bScale;

						// Account for negative scale values
						if (m_prgColInfo[iIndex].dwFlags & DBCOLUMNFLAGS_SCALEISNEGATIVE)
							sScale = -sScale;

						if (!COMPARE(*(SHORT *)pValue, sScale))
							odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
								L"Invalid value returned.\n";
					}
				}
				// NULL for all other data types
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Status was not DBSTATUS_S_ISNULL for non-DECIMAL, NUMERIC column\n";
				break;
			case 19:// CHARACTER_SET_CATALOG
			case 20:// CHARACTER_SET_SCHEMA
			case 21:// CHARACTER_SET_NAME
			case 22:// COLLATION_CATALOG
			case 23:// COLLATION_SCHEMA
			case 24:// COLLATION_NAME
			case 25:// DOMAIN_CATALOG
			case 26:// DOMAIN_SCHEMA
			case 27:// DOMAIN_NAME
				if (ulStatus == DBSTATUS_S_OK)
				{
					// For now, just make sure the length is right so we read the value
					if (!COMPARE(wcslen((WCHAR *)pValue)*sizeof(WCHAR), LENGTH_BINDING(m_rgDBBINDING[iBind], pData)))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Length is incorrect.\n";

					// We really don't expect empty strings for any of these
					if (!COMPARE(wcslen((WCHAR *)pValue) > 0, TRUE))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Empty string returned.\n";
					
				}
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid status returned.\n";
				break;
			case 28:// DESCRIPTION
				// If Description is valid it may still be an empty string.  All we test
				// here is if it's null terminated properly.
				if (ulStatus == DBSTATUS_S_OK)
				{
					if (!COMPARE(wcslen((WCHAR *)pValue)*sizeof(WCHAR), LENGTH_BINDING(m_rgDBBINDING[iBind], pData)))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Length is incorrect.\n";
				}
				// Description is allowed to be NULL
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Unexpected status value found.\n";
				break;
			// PROVIDER SPECIFIC
			default:
				if (iRow == 1)
				{
					if(!m_rgDBCOLUMNINFO[iBind].iOrdinal) 
					{
						if(!COMPARE(GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset), TRUE))
							odtLog << L"COLUMNS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - COLUMNS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}

				// We expect to retrieve the column successfully
				if (ulStatus != DBSTATUS_S_OK &&
					ulStatus != DBSTATUS_S_ISNULL &&
					!COMPARE(ulStatus, DBSTATUS_S_OK))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Invalid status returned.\n";

				break;
		}
	}
	
	return fResults;
}

//--------------------------------------------------------------------
// CONSTRAINT_COLUMN_USAGE
// 1. Table Catalog 
// 2. Table Schema 
// 3. Table Name
// 4. Column Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_CONSTRAINT_COLUMN_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCONSTRAINT_COLUMN_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeCONSTRAINT_COLUMN_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cCONSTRAINT_COLUMN_USAGE;
	m_cRestrictions = cCONSTRAINT_COLUMN_USAGE_RESTRICTIONS;

	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszColumnRestriction);

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCONSTRAINT_COLUMN_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_CONSTRAINT_COLUMN_USAGE
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCONSTRAINT_COLUMN_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:CONSTRAINT_COLUMN_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);					  

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
				switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
				{
				case 1:// TABLE_CATALOG
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog <<  L"VerifyRow_CONSTRAINT_COLUMN_USAGE:TABLE_CATALOG restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 2:// TABLE_SCHEMA
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog <<  L"VerifyRow_CONSTRAINT_COLUMN_USAGE:TABLE_SCHEMA restriction failed\n";
								m_fRes2=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
					break;
				case 3:	// TABLE_NAME
					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog <<  L"VerifyRow_CONSTRAINT_COLUMN_USAGE:TABLE_NAME restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 4:// COLUMN_NAME
					if(m_restrict & FOURTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes4)
							{
								odtLog << L"VerifyRow_CONSTRAINT_COLUMN_USAGE:COLUMN_NAME restriction failed\n";
								m_fRes4=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 5:	// COLUMN_GUID
						PRVTRACE(L"\n");
						break;
				case 6:	//COLUMN_PROPID
						PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
						break;
				case 7: // CONSTRAINT CATALOG
				case 8: // CONSTRAINT SCHEMA
				case 9: // CONSTRAINT NAME
						PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
						break;
				default:
					// We found a column not spec'd for this schema rowset, print a warning.
					if (iRow == 1)
					{
						if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
						{
							if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
								odtLog << L"CONSTRAINT COLUMN USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
						}
						else
							odtLog << L"Warning - CONSTRAINT COLUMN USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
					}
					break;
				}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// CONSTRAINT_TABLE_USAGE
// 1. Table Catalog 
// 2. Table Schema 
// 3. Table Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_CONSTRAINT_TABLE_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszCONSTRAINT_TABLE_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeCONSTRAINT_TABLE_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cCONSTRAINT_TABLE_USAGE;
	m_cRestrictions = cCONSTRAINT_TABLE_USAGE_RESTRICTIONS;
	
	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);

	PRVTRACE(L"GetSchemaInfo::CONSTRAINT_TABLE_USAGE\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowCONSTRAINT_TABLE_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_CONSTRAINT_TABLE_USAGE
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cCONSTRAINT_TABLE_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:CONSTRAINT_TABLE_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);				  

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// TABLE_CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_CONSTRAINT_TABLE_USAGE:TABLE_CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// TABLE_SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_CONSTRAINT_TABLE_USAGE:TABLE_SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// TABLE_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_CONSTRAINT_TABLE_USAGE:TABLE_NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4: // CONSTRAINT CATALOG
			case 5:	// CONSTRAINT SCHEMA
			case 6:	//CONSTRAINT NAME
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"CONSTRAINT TABLE USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - CONSTRAINT TABLE USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// FOREIGN_KEYS
// 1. PK_TABLE_CATALOG
// 2. PK TABLE SCHEMA
// 3. PK TABLE NAME
// 4. FK TABLE CATALOG
// 5. FK TABLE SCHEMA
// 6. FK TABLE ANEM
//
// Caveat: These restrictions will be tested as part of ad-hoc for the
// following reasons, The following code is a place holder in case
// the problems listed below are ever fixed:
// 1. Can't change private library's create table because not all
// drivers will support PK and FK systax, create table would fail
// 2. Can't use alter table statement because I can only add column
// that allow nulls and pk and fk are not nullable.
//
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_FOREIGN_KEYS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszFOREIGN_KEYS;
	m_rgColumnTypes = (DBTYPE *)rgtypeFOREIGN_KEYS;

	// Set the count of columns and restrictions
	m_cColumns = cFOREIGN_KEYS;
	m_cRestrictions = cFOREIGN_KEYS_RESTRICTIONS;

	// Set PK Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszPK_TableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszCatalogRestriction);
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszSchemaRestriction);
	SetRestriction(SIXTH, 6, &m_wszR6, m_pwszFK_TableRestriction);

	// Set expected row count
	if (m_fForeignKey)
		// We created a foreign key, so there must be one row
		SetRowCount(MIN_REQUIRED, 1);
	
	PRVTRACE(L"GetSchemaInfo::FOREIGN_KEYS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowFOREIGN_KEYS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_FOREIGN_KEYS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cFOREIGN_KEYS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:FOREIGN_KEYS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
				case 1:// PK TABLE_CATALOG
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog << L"VerifyRow_FOREIGN_KEYS:PK TABLE_CATALOG restriction failed\n";
								m_fRes1=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 2:// PK TABLE_SCHEMA
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog << L"VerifyRow_FOREIGN_KEYS:PK TABLE_SCHEMA restriction failed\n";
								m_fRes2=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 3:	// PK TABLE_NAME
					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog << L"VerifyRow_FOREIGN_KEYS:PK TABLE_NAME restriction failed\n";
								m_fRes3=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 4: // PK COLUMN NAME
				case 10:// FK_COLUMN NAME
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 14:// UPDATE RULE
				case 15:// DELETE RULE
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"CASCADE")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SET NULL"))	&&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SET DEFAULT")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"NO ACTION")))
					{
						odtLog << L"VerifyRow_FOREIGN_KEYS:UPDATE or DELETE RULE expected CASCADE,SET NULL,SET DEFAULT, or NO ACTION but received " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults=FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 5: // PK COLUMN GUID
				case 11:// FK COLUMN GUID
				case 16:// PK_NAME
				case 17:// FK_NAME
				case 18:// DEFERRABILITY
					PRVTRACE(L"\n");
				case 6: // PK COLUMN PROPID
				case 12:// FK COLUMN PROPID
				case 13:// ORDINAL
						PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
						break;
				case 7:// FK TABLE_CATALOG
					if(m_restrict & FOURTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes4)
							{
								odtLog <<  L"VerifyRow_FOREIGN_KEYS:FK TABLE_CATALOG restriction failed\n";
								m_fRes4=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 8:// FK TABLE_SCHEMA
					if(m_restrict & FIFTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes5)
							{
								odtLog << L"VerifyRow_FOREIGN_KEYS:FK TABLE_SCHEMA restriction failed\n";
								m_fRes5=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 9:	// FK TABLE_NAME
					if(m_restrict & SIXTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes6)
							{
								odtLog << L"VerifyRow_FOREIGN_KEYS:FK TABLE_NAME restriction failed\n";
								m_fRes6=FALSE;
								fResults=FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				default:
					// We found a column not spec'd for this schema rowset, print a warning.
					if (iRow == 1)
					{
						if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
						{
							if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
								odtLog << L"FOREIGN KEYS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
						}
						else
							odtLog << L"Warning - FOREIGN KEYS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
					}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults=FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// INDEXES
// 1. Table Catalog 
// 2. Table Schema
// 3. Index Name
// 4. Type
// 5. Table Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_INDEXES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszINDEXES;
	m_rgColumnTypes = (DBTYPE *)rgtypeINDEXES;

	// Set the count of columns and restrictions
	m_cColumns = cINDEXES;
	m_cRestrictions = cINDEXES_RESTRICTIONS;
	
	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszIndexRestriction);

	if((m_currentBitMask & FOURTH) || m_fPassUnsupportedRestrictions)
	{
		if((m_restrict & FOURTH)|| (m_restrict & ALLRES))
		{
			m_ulR = DBPROPVAL_IT_BTREE;
			m_rgvarRestrict[3].vt = VT_UI2;
			m_rgvarRestrict[3].lVal = DBPROPVAL_IT_BTREE;
			m_cRestrictionsCurrent ++;
			SetRestriction(FOURTH);

			RESTRICTNOTSUPPORTED(FOURTH)
		}
	}

	// TABLE NAME
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszTableRestriction);

	PRVTRACE(L"GetSchemaInfo::INDEXES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowINDEXES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_INDEXES
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cINDEXES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		DBBINDING * pBind = &m_rgDBBINDING[iBind];
		ULONG iOrdinal = m_rgDBCOLUMNINFO[iBind].iOrdinal;
		DBSTATUS ulStatus = STATUS_BINDING(*pBind, pData);
	    DATA *	pColumn=(DATA *) (pData + pBind->obStatus);
		BYTE * pValue = pColumn->bValue;

		PRVTRACE(L"INDEXES:Row[%lu],Col[%d,%s]:", 
			iRow,
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// TABLE_CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog  << L"VerifyRow_INDEXES:TABLE_CATALOG 1 restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// TABLE_SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog  << L"VerifyRow_INDEXES:TABLE_SCHEMA 2 restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// TABLE_NAME
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_INDEXES:TABLE_NAME 5 restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// INDEX_CATALOG
			case 5:	// INDEX_SCHEMA
			case 18:// COLUMN_NAME
			case 24:// FILTER_CONDIITON
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 6:// INDEX_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_INDEXES:INDEX_NAME 3 restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 7: // PRIMARY KEY
			case 8: // UNIQUE
			case 9: // CLUSTERED
			case 14:// SORTBOOKMARKS
			case 15:// AUTOUPDATE
			case 25:// INTEGRATED
					if((*(TYPE_BOOL *)pColumn->bValue)==VARIANT_TRUE)
						PRVTRACE(L"TRUE\n");
					else
						PRVTRACE(L"FALSE\n");
					break;
			case 10:// TYPE
					if(m_restrict & FOURTH)
					{
						if(m_ulR!=(*(TYPE_UI2*)pColumn->bValue))
						{
							odtLog <<  L"VerifyRow_INDEXES:INDEX_TYPE 4 restriction failed\n";
							m_fRes4=FALSE;
							fResults = FALSE;
						}
					}
					PRVTRACE(L"%d\n",*(TYPE_UI2 *)pColumn->bValue);
					break;
			case 11:// FILLFACTOR
				// This column cannot be NULL
				if (COMPARE(ulStatus, DBSTATUS_S_OK))
				{
					ULONG ulFillFactor = *(ULONG *)pValue;

					if (!COMPARE(ulFillFactor > 0, TRUE) ||
						!COMPARE(ulFillFactor <= 100, TRUE))
						odtLog << L"FillFactor: Invalid value returned.\n";
				}
				break;
			case 12:// INITIALSIZE
			case 13:// NULLS
			case 16:// NULCOLLATION
			case 22:// CARDINALITY
			case 23:// PAGES
					PRVTRACE(L"%d\n",*(TYPE_I4 *)pColumn->bValue);
					break;
			case 19: // COLUMN_GUID
					PRVTRACE(L"\n");
					break;
			case 21:// COLLATION
					PRVTRACE(L"%d\n",*(TYPE_I2 *)pColumn->bValue);
					break;
			case 20:// COLUMN_PROPID
			case 17:// ORDINAL_POSITION
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"INDEXES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - INDEXES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
		}
		else
		{
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// KEY_COLUMN_USAGE
// 1. Constraint Catalog Name
// 2. Constraint Schema Name
// 3. Constraint Name
// 4. Table Catalog 
// 5. Table Schema 
// 6. Table Name
// 7. Column Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_KEY_COLUMN_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszKEY_COLUMN_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeKEY_COLUMN_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cKEY_COLUMN_USAGE;
	m_cRestrictions = cKEY_COLUMN_USAGE_RESTRICTIONS;
	
	// Set Constraint Restrictions
	SetRestriction(FIRST,  1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND, 2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD,  3, &m_wszR3, m_pwszKey_Column_Usage_ConstraintRestriction);
	SetRestriction(FOURTH, 4, &m_wszR4, m_pwszCatalogRestriction);
	SetRestriction(FIFTH,  5, &m_wszR5, m_pwszSchemaRestriction);
	SetRestriction(SIXTH,  6, &m_wszR6, m_pwszTableRestriction);
	SetRestriction(SEVENTH,7, &m_wszR7, m_pwszColumnRestriction);
		
	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowKEY_COLUMN_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_KEY_COLUMN_USAGE
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 	iBind;			// Binding Count
    DATA *	pColumn;		// Data Structure
	CCol	col;
	BOOL	fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszKey_Column_Usage_ConstraintRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cKEY_COLUMN_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		PRVTRACE(L"Row[%lu],Col[%s]:KEY_COLUMN_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// CONSTRAINT CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog <<  L"VerifyRow_KEY_COLUMN_USAGE:CONSTRAINT CATALOG 1 restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// CONSTRAINT SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_KEY_COLUMN_USAGE:CONSTRAINT SCHEMA 2 restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// CONSTRAINT NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszKey_Column_Usage_ConstraintRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszKey_Column_Usage_ConstraintRestriction)
						wcscpy(m_pwszKey_Column_Usage_ConstraintRestriction,(TYPE_WSTR) pColumn->bValue);
					
				}

				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_KEY_COLUMN_USAGE:CONSTRAINT NAME 3 restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE_CATALOG
				if(m_restrict & FOURTH)
				{
					if(!COMPARE(0,wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes4)
						{
							odtLog <<  L"VerifyRow_KEY_COLUMN_USAGE:TABLE_CATALOG 4 restriction failed\n";
							m_fRes4=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// TABLE_SCHEMA
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_KEY_COLUMN_USAGE:TABLE_SCHEMA 5 restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 6:	// TABLE_NAME
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog <<  L"VerifyRow_KEY_COLUMN_USAGE:TABLE_NAME 6 restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 7:// COLUMN NAME
				if(m_restrict & SEVENTH)
				{
					if(0,wcsicmp((TYPE_WSTR)m_wszR7,(TYPE_WSTR)pColumn->bValue))
					{
						if(m_fRes7)
						{
							odtLog << L"VerifyRow_KEY_COLUMN_USAGE:COLUMN NAME 7 restriction failed\n";
							m_fRes7=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 8: // COLUMN_GUID
					PRVTRACE(L"\n");
					break;
			case 9: // COLUMN PROPID
			case 10:// ORDINAL POSITION
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (!m_rgDBCOLUMNINFO[iBind].iOrdinal)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"KEY COLUMN USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - KEY COLUMN USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// PRIMARY_KEYS
// 1. Table Catalog 
// 2. Table Name
// 3. Table Schema
//
// Caveat: this will be handled in ad-hoc see PrepareParams_Foreign_keys()
// for explanation.
//
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_PRIMARY_KEYS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszPRIMARY_KEYS;
	m_rgColumnTypes = (DBTYPE *)rgtypePRIMARY_KEYS;

	// Set the count of columns and restrictions
	m_cColumns = cPRIMARY_KEYS;
	m_cRestrictions = cPRIMARY_KEYS_RESTRICTIONS;

	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszPK_TableRestriction);

	// Set expected count of rows
	if (m_fPrimaryKey)
		// We created a primary key, so there must be one row
		SetRowCount(MIN_REQUIRED, 1);
	
	PRVTRACE(L"GetSchemaInfo::PRIMARY_KEYS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowPRIMARY_KEYS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_PRIMARY_KEYS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cPRIMARY_KEYS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		DBBINDING * pBind = &m_rgDBBINDING[iBind];
		ULONG iOrdinal = m_rgDBCOLUMNINFO[iBind].iOrdinal;
		DBSTATUS ulStatus = STATUS_BINDING(*pBind, pData);
	    DATA *	pColumn=(DATA *) (pData + pBind->obStatus);
		BYTE * pValue = pColumn->bValue;
		
		PRVTRACE(L"Row[%lu],Col[%s]:PRIMARY_KEYS", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// TABLE CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog <<  L"VerifyRow_PRIMARY_KEYS:TABLE CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// TABLE SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_PRIMARY_KEYS:TABLE SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// TABLE NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_PRIMARY_KEYS:TABLE NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// COLUMN NAME
			case 8:// PK_NAME
				if (ulStatus == DBSTATUS_S_OK)
				{
					// For now, just make sure the length is right so we read the default value
					if (!COMPARE(wcslen((WCHAR *)pValue)*sizeof(WCHAR), LENGTH_BINDING(m_rgDBBINDING[iBind], pData)))
						odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
							L"Default value has the wrong length.\n";

					// TODO: If the current table is the primary table for this test make sure it matches expected.  
				}
				else if (!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid status returned.\n";
				break;
			case 5: // COLUMN_GUID
			case 6: // COLUMN PROPID
			case 7: // ORDINAL POSITION
				if (ulStatus != DBSTATUS_S_OK &&
					!COMPARE(ulStatus, DBSTATUS_S_ISNULL))
					odtLog << m_pwszTableName << L": " << rgwszCOLUMNS[iOrdinal-1] << L": " <<
						L"Invalid status returned.\n";
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"PRIMARY KEYS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - PRIMARY KEYS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// PROCEDURE_COLUMNS
// 1. Procedure Catalog 
// 2. Procedure Schema
// 3. Procedure Name
// 4. Column Name
//
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_PROCEDURE_COLUMNS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszPROCEDURE_COLUMNS;
	m_rgColumnTypes = (DBTYPE *)rgtypePROCEDURE_COLUMNS;

	// Set the count of columns and restrictions
	m_cColumns = cPROCEDURE_COLUMNS;
	m_cRestrictions = cPROCEDURE_COLUMNS_RESTRICTIONS;
	
	// Set Procedure Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszProcedureRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszProcedureColumnsRestriction);

	PRVTRACE(L"GetSchemaInfo::PROCEDURE_COLUMNS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowPROCEDURE_COLUMNS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_PROCEDURE_COLUMNS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cPROCEDURE_COLUMNS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:PROCEDURE_COLUMNS", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// PROCEDURE CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog <<  L"VerifyRow_PROCEDURE_COLUMNS:PROCEDURE CATALOG 1 restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// PROCEDURE SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_PROCEDURE_COLUMNS:PROCEDURE SCHEMA 2 restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// PROCEDURE NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_PROCEDURE_COLUMNS:PROCEDURE NAME 3 restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// COLUMN NAME
					if(m_fCaptureRestrictions)
					{
						m_pwszProcedureColumnsRestriction = (WCHAR *) PROVIDER_ALLOC
							((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszProcedureColumnsRestriction)
							wcscpy(m_pwszProcedureColumnsRestriction,(TYPE_WSTR) pColumn->bValue);
					}

					if(m_restrict & FOURTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes4)
							{
								odtLog << L"VerifyRow_PROCEDURE_COLUMNS:COLUMN NAME 4 restriction failed\n";
								m_fRes4=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 5: // COLUMN_GUID
			case 11:// TYPE_GUID
					PRVTRACE(L"\n");
					break;
			case 6: // COLUMN PROPID
			case 7: // ROWSET NUMBER
			case 8: // ORDINAL POSITION
			case 12:// CHARACTER_MAXIMUM_LENGTH
			case 13:// CHARACTER_OCTET_LENGTH
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			case 9:// IS_NULLABLE
					PRVTRACE(L"%d\n",*(TYPE_BOOL *)pColumn->bValue);
					break;
			case 10:// DATA_TYPE
			case 14:// NUMERIC_PRECISION
					PRVTRACE(L"%d\n",*(TYPE_UI2 *)pColumn->bValue);
					break;
			case 15:// NUMERIC_SCALE
					PRVTRACE(L"%d\n",*(TYPE_I2 *)pColumn->bValue);
					break;
			case 16:// DESCRIPTION
					PRVTRACE(L"%d\n",(TYPE_WSTR)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"PROCEDURE COLUMNS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - PROCEDURE COLUMNS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// PROCEDURE_PARAMETERS
// 1. Procedure Catalog
// 2. Procedure Schema
// 3. Procedure Name
// 4. Parameter Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_PROCEDURE_PARAMETERS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszPROCEDURE_PARAMETERS;
	m_rgColumnTypes = (DBTYPE *)rgtypePROCEDURE_PARAMETERS;

	// Set the count of columns and restrictions
	m_cColumns = cPROCEDURE_PARAMETERS;
	m_cRestrictions = cPROCEDURE_PARAMETERS_RESTRICTIONS;
	
	// Set Procedure Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszProcedureRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszParameterRestriction);

	PRVTRACE(L"GetSchemaInfo::PROCEDURES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowPROCEDURE_PARAMETERS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_PROCEDURE_PARAMETERS(ULONG iRow, BYTE * pData)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	
	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszParameterRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cPROCEDURE_PARAMETERS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:PROCEDURE_PARAMETERS", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// PROCEDURE CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// PROCEDURE SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// PROCEDURE_NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsnicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue,wcslen(m_wszR3))))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE_NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
					else
					{
						// Check the length of the proc name
						if( (wcslen(m_wszR3) != wcslen((TYPE_WSTR)pColumn->bValue)) &&
							(!COMPARE(wcsicmp((TYPE_WSTR)&pColumn->bValue[wcslen(m_wszR3)*sizeof(WCHAR)], L";1")==0,TRUE)) )
						{
							odtLog <<  L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE_NAME length did not match\n";
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// PARAMETER NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszParameterRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszParameterRestriction)
						wcscpy(m_pwszParameterRestriction,(TYPE_WSTR) pColumn->bValue);
				}

				if(m_restrict & FOURTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes4)
						{
							odtLog << L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE NAME restriction failed\n";
							m_fRes4=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5: // ORDINAL POSITION
			case 6: // PARAMETER TYPE
			case 10:// DATA TYPE
					PRVTRACE(L"%d\n",*(TYPE_UI2 *)pColumn->bValue);
					break;
			case 7: // PARAMETER_HASDEFAULT
			case 9: // IS NULLABLE
					if((*(TYPE_BOOL *)pColumn->bValue)==VARIANT_TRUE)
						PRVTRACE(L"TRUE\n");
					else
						PRVTRACE(L"FALSE\n");
					break;
			case 11:// CHARACTER_MAXIMUM_LENGTH
			case 12:// CHARACTER_OCTECT_LENGTH
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			case 13:// NUMERIC_PRECISION
					PRVTRACE(L"%d\n",*(TYPE_UI2 *)pColumn->bValue);
					break;
			case 14:// NUMERIC_SCALE
					PRVTRACE(L"%d\n",*(TYPE_I2 *)pColumn->bValue);
					break;
			case 8: // PARAMETER DEFAULT
			case 15:// DESCRIPTION
			case 16:// TYPE_NAME
			case 17:// LOCAL_TYPE_NAME
					PRVTRACE(L"'%s'\n",(TYPE_WSTR *)pColumn->bValue);
					break;

			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"PROCEDURE PARAMETERS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - PROCEDURE PARAMETERS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// PROCEDURES
// 1. Catalog Name
// 2. Schema Name
// 3. Procedure Name
// 4. Procedure Type
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_PROCEDURES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszPROCEDURES;
	m_rgColumnTypes = (DBTYPE *)rgtypePROCEDURES;

	// Set the count of columns and restrictions
	m_cColumns = cPROCEDURES;
	m_cRestrictions = cPROCEDURES_RESTRICTIONS;
	
	// Set Procedure Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszProcedureRestriction);

	if((m_currentBitMask & FOURTH) || m_fPassUnsupportedRestrictions)
	{
		if((m_restrict & FOURTH)|| (m_restrict & ALLRES))
		{
			if(m_ProcedureTypeRestriction || m_fPassUnsupportedRestrictions)
			{
				m_ulR = (m_fPassUnsupportedRestrictions) ? DB_PT_PROCEDURE : m_ProcedureTypeRestriction;
				m_rgvarRestrict[3].vt = VT_I2;
				V_I2(&m_rgvarRestrict[3]) = (SHORT)m_ulR;
				m_cRestrictionsCurrent ++;
				SetRestriction(FOURTH);

				RESTRICTNOTSUPPORTED(FOURTH)

			}
		}
	}

	PRVTRACE(L"GetSchemaInfo::PROCEDURES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowPROCEDURES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_PROCEDURES
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszProcedureRestriction && m_ProcedureTypeRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cPROCEDURES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"PROCEDURES:Row[%lu],Col[%d.%s]:", 
			iRow, 
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// PROCEDURE_CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog <<  L"VerifyRow_PROCEDURES:PROCEDURE_CATALOG 1 restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// PROCEDURE_SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog <<  L"VerifyRow_PROCEDURES:PROCEDURE_SCHEMA 2 restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// PROCEDURE_NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszProcedureRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszProcedureRestriction)
						wcscpy(m_pwszProcedureRestriction,(TYPE_WSTR) pColumn->bValue);
				}

				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsnicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue,wcslen(m_wszR3))))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_PROCEDURES:PROCEDURE_NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
					else
					{
						// Check the length of the proc name
						if( (wcslen(m_wszR3) != wcslen((TYPE_WSTR)pColumn->bValue)) &&
							(!COMPARE(wcsicmp((TYPE_WSTR)&pColumn->bValue[wcslen(m_wszR3)*sizeof(WCHAR)], L";1")==0,TRUE)) )
						{
							odtLog <<  L"VerifyRow_PROCEDURE_PARAMETERS:PROCEDURE_NAME length did not match\n";
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:	// PROCEDURE_TYPE
					if(m_fCaptureRestrictions)
						m_ProcedureTypeRestriction = *(TYPE_I2 *) pColumn->bValue;

					if(m_restrict & FOURTH)
					{
						if((TYPE_I2) m_ulR  != (*(TYPE_I2 *)pColumn->bValue))
						{
							if(m_fRes4)
							{
								odtLog << L"VerifyRow_PROCEDURES:DATA_TYPE restriction failed\n";
								m_fRes4=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"%d\n",*(TYPE_I2 *)pColumn->bValue);
					break;
			case 5:// PROCEDURE DEFINITION
			case 6:// DESCRIPTION
					PRVTRACE(L"%d\n",(TYPE_WSTR)pColumn->bValue);
					break;
			// DATE_CREATED
			// DATE_MODIFIED
			case 7:
			case 8:
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"PROCEDURES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - PROCEDURES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// PROVIDER_TYPES
// 1. Data Type
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_PROVIDER_TYPES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszPROVIDER_TYPES;
	m_rgColumnTypes = (DBTYPE *)rgtypePROVIDER_TYPES;

	// Set the count of columns and restrictions
	m_cColumns = cPROVIDER_TYPES;
	m_cRestrictions = cPROVIDER_TYPES_RESTRICTIONS;

	if((m_currentBitMask & FIRST) || m_fPassUnsupportedRestrictions)
	{
		if((m_restrict & FIRST) || (m_restrict & ALLRES))
		{
			if(m_DataTypeRestriction)
			{
				m_ulR = m_DataTypeRestriction;
				m_rgvarRestrict[0].vt = VT_UI2;
				m_rgvarRestrict[0].iVal	= m_DataTypeRestriction;		
				m_cRestrictionsCurrent ++;
				SetRestriction(FIRST);

				RESTRICTNOTSUPPORTED(FIRST)

			}
		}
	}
	if((m_currentBitMask & SECOND) || m_fPassUnsupportedRestrictions)
	{
		if((m_restrict & SECOND) || (m_restrict & ALLRES))
		{
			m_fR = (m_BestMatchRestriction==VARIANT_TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
			m_rgvarRestrict[1].vt = VT_BOOL;
			m_rgvarRestrict[1].boolVal	= (m_BestMatchRestriction==VARIANT_TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
			m_cRestrictionsCurrent ++;
			SetRestriction(SECOND);

			RESTRICTNOTSUPPORTED(SECOND)
		}
	}

	PRVTRACE(L"GetSchemaInfo::PROVIDER_TYPES\n");
	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRow_PROVIDER_TYPES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_PROVIDER_TYPES
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cPROVIDER_TYPES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"PROVIDER_TYPES:Row[%lu],Col[%d,%s]:", 
			iRow, 
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1: // TYPE_NAME
			case 4: // LITERAL_PREFIX
			case 5: // LITERAL_SUFFIX
			case 6: // CREATE PARAMS
			case 13:// LOCAL_TYPE_NAME
			case 17:// TYPELIB
			case 18:// VERSION
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 2:// DATA TYPE
					if(m_restrict & FIRST)
					{
						if(m_ulR!= (*(TYPE_UI2 *)pColumn->bValue))
						{
							if(m_fRes1)
							{
								odtLog << L"VerifyRow_PROVIDER_TYPES:TYPE NAME restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"%d\n",*(TYPE_UI2 *)pColumn->bValue);
					break;
			case 3:	// COLUMN SIZE
			case 9: // SEARCHABLE
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
						break;
			case 7: // IS_NULLABLE
			case 8: // CASE_SENSITIVE
			case 10:// UNSIGNED_ATTRIBUTE
			case 11:// FIXED PREC SCALE
			case 12:// AUTO_UNIQUE_VALUE
			case 19:// IS_LONG
			case 21:// IS_FIXEDLENGTH
					if((*(TYPE_BOOL *)pColumn->bValue)==VARIANT_TRUE)
						PRVTRACE(L"TRUE\n");
					else
						PRVTRACE(L"FALSE\n");
					break;
			case 14:// MINIMUM_SCALE
			case 15: // MAXIMUM_SCALE
					PRVTRACE(L"%d\n",*(TYPE_I2 *)pColumn->bValue);
						break;
			case 20: // BEST_MATCH
					if(m_fCaptureRestrictions)
						m_BestMatchRestriction = *(TYPE_BOOL *) pColumn->bValue;

					if(m_restrict & SECOND)
					{
						if(m_fR != (*(TYPE_BOOL *)pColumn->bValue))
						{
							odtLog << L"VerifyRow_PROVIDER_TYPES:BEST_MATCH restriction failed\n";
							fResults = FALSE;
						}
					}
					
					if((*(TYPE_BOOL *)pColumn->bValue)==VARIANT_TRUE)
						PRVTRACE(L"TRUE\n");
					else
						PRVTRACE(L"FALSE\n");
					break;

			case 16:// GUID
					PRVTRACE(L"\n");
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"PROVIDER TYPES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - PROVIDER TYPES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// REFERENTIAL_CONSTRAINTS
// 1. Constraint Catalog 
// 2. Constraint Schema 
// 3. Constraint Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_REFERENTIAL_CONSTRAINTS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszREFERENTIAL_CONSTRAINTS;
	m_rgColumnTypes = (DBTYPE *)rgtypeREFERENTIAL_CONSTRAINTS;

	// Set the count of columns and restrictions
	m_cColumns = cREFERENTIAL_CONSTRAINTS;
	m_cRestrictions = cREFERENTIAL_CONSTRAINTS_RESTRICTIONS;
	
	// Set Constraint Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszReferential_ConstraintRestriction);

	PRVTRACE(L"GetSchemaInfo::REFERENTIAL_CONSTRAINTS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowREFERENTIAL_CONSTRAINTS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_REFERENTIAL_CONSTRAINTS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszReferential_ConstraintRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cREFERENTIAL_CONSTRAINTS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"REFERENTIAL_CONSTRAINTS:Row[%lu],Col[%d,%s]:", 
			iRow, 
			m_rgDBCOLUMNINFO[iBind].iOrdinal,
			m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
 			// TODO: not currently supported
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:	 // CONSTRAINT_CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:CONSTRAINT_CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:	 // CONSTRAINT_SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:CONSTRAINT_SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	 // CONSTRAINT_NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszReferential_ConstraintRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR *)) + sizeof(WCHAR));
					if(m_pwszReferential_ConstraintRestriction)
						wcscpy(m_pwszReferential_ConstraintRestriction,(TYPE_WSTR) pColumn->bValue);
				}

				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:CONSTRAINT_NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:	 // UNIQUE_CONSTRAINT_CATALOG
			case 5:	 // UNIQUE_CONSTRAINT_SCHEMA
			case 6:	 // UNIQUE_CONSTRAINT_NAME
			case 10: // DESCRIPTION
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 7:  // MATCH_OPTION
				if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"NONE")) && 
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"PARTIAL"))&&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"FULL")))
				{
					odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:MATCH OPTION expected NONE/PARTIAL/FULL but recieved " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;

			case 8:  // UPDATE_RULE
			case 9:  // DELETE_RULL
				if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"CASCASE")) && 
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SET NULL"))	&&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SET DEFAULT"))&&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"NO ACTION")))
				{
					odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:UPDATE/DELETE RULE expected SELECT,DELETE,INSERT,UPDATE, or REFERENCES but recieved " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"REFERENTIAL CONSTRAINTS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - REFERENTIAL CONSTRAINTS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}

	}
	return fResults;
}

//--------------------------------------------------------------------
// SCHEMATA
// 1. Catalog Name
// 2. Schema Name
// 3. Schema Owner
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_SCHEMATA()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszSCHEMATA;
	m_rgColumnTypes = (DBTYPE *)rgtypeSCHEMATA;

	// Set the count of columns and restrictions
	m_cColumns = cSCHEMATA;
	m_cRestrictions = cSCHEMATA_RESTRICTIONS;
	
	// Set Schema Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszSchemaRestriction);

	PRVTRACE(L"GetSchemaInfo::SCHEMATA\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowSCHEMATA
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_SCHEMATA
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cSCHEMATA <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn=(DATA *)(pData + m_rgDBBINDING[iBind].obStatus);

		if(pColumn->sStatus == DBSTATUS_S_OK)
		{
			switch(m_rgDBBINDING[iBind].iOrdinal)
			{
			case 1:// CATALOG_NAME
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog << L"VerifyRow_SCHEMATA:CATALOG_NAME restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;

			case 2:// SCHEMA_NAME
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog <<  L"VerifyRow_SCHEMATA:SCHEMA_NAME restriction failed\n";
								m_fRes2=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;

			case 3:	// SCHEMA_OWNER
					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog << L"VerifyRow_SCHEMATA:SCHEMA_OWNER restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;

			case 4: // DEFAULT_CHARACTER_SET_CATALOG
			case 5:	// DEFAULT_CHARACTER_SET_SCHEMA
			case 6: // DEFAULT_CHARACTER_SET_NAME
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"SCHEMATA: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - SCHEMATA provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// SQL_LANGUAGES
// No Constraints
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_SQL_LANGUAGES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszSQL_LANGUAGES;
	m_rgColumnTypes = (DBTYPE *)rgtypeSQL_LANGUAGES;
	
	// Set the count of columns and restrictions
	m_cColumns = cSQL_LANGUAGES;
	m_cRestrictions = cSQL_LANGUAGES_RESTRICTIONS;
	
	m_cRestrictionsCurrent = 0;
	
	PRVTRACE(L"GetSchemaInfo::SQL_LANGUAGES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowSQL_LANGUAGES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_SQL_LANGUAGES
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cSQL_LANGUAGES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:VerifyRow_SQL_LANGUAGES:", iRow,m_rgDBCOLUMNINFO[iBind].pwszName);
		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(iBind)
			{
			case 1: // SQL LANGUAGE SOURCE
					if(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"ISO 9075"))
					{
						odtLog << L"VerifyRow_SQL_LANGUAGES:SQL_LANGUAGE_SOURCE expected ISO 9075 but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 2: // SQL LANGUAGE YEAR
					if(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"ISO 9075"))
					{
						odtLog << L"VerifyRow_SQL_LANGUAGES:SQL_LANGUAGE_YEAR expected 1992 but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 3: // SQL LANGUAGE CONFORMANCE
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"ENTRY")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"INTERMEDIATE")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"FULL")))
					{
						odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:SQL_LANGUAGE_CONFORMANCE expected ENTRY/INTERMEDIATE/FULL but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 4: // SQL LANGUAGE INTEGRITY
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"YES")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"NO")))
					{
						odtLog << L"VerifyRow_REFERENTIAL_CONSTRAINTS:SQL_LANGUAGE_INTEGRITY expected YES/NO but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 5: // SQL LANGUAGE IMPLEMENTATION, should be null if SQL_LANGUAGE_SOURCE is 'ISO_9075'
			case 6: // SQL LANGUAGE BINDING SYTLE
			case 7: // SQL LANGUAGE PROGRAMMING LANGUAGE
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			default:
					PRVTRACE(L"%s not expected\n",m_rgDBCOLUMNINFO[iBind].pwszName);
					break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// STATISTICS
// 1. Catalog Name
// 2. Schema Name
// 3. Table Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_STATISTICS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszSTATISTICS;
	m_rgColumnTypes = (DBTYPE *)rgtypeSTATISTICS;
	
	// Set the count of columns and restrictions
	m_cColumns = cSTATISTICS;
	m_cRestrictions = cSTATISTICS_RESTRICTIONS;
	
	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);

	PRVTRACE(L"GetSchemaInfo::STATISTICS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowSTATISTICS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_STATISTICS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cSTATISTICS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:STATISTICS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);
		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1:// TABLE CATALOG
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog << L"VerifyRow_STATISTICS:TABLE_CATALOG restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 2:// TABLE SCHEMA
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog << L"VerifyRow_STATISTICS:TABLE_SCHEMA restriction failed\n";
								m_fRes2=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 3:	//TABLE NAME
					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog << L"VerifyRow_STATISTICS:TABLE_NAME restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 4: // CARDINALITY
					PRVTRACE(L"%d\n",*(TYPE_I4 *)pColumn->bValue);
					break;
			default:
					PRVTRACE(L"%s not expected\n",m_rgDBCOLUMNINFO[iBind].pwszName);
					break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// TABLE_CONSTRAINTS
// 1. Constraint Catalog 
// 2. Constraint Schema 
// 3. Constraint Name
// 4. Table Catalog
// 5. Table Schema
// 6. Table Name
// 7. Constraint Type
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TABLE_CONSTRAINTS()
{	
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszTABLE_CONSTRAINTS;
	m_rgColumnTypes = (DBTYPE *)rgtypeTABLE_CONSTRAINTS;

	// Set the count of columns and restrictions
	m_cColumns = cTABLE_CONSTRAINTS;
	m_cRestrictions = cTABLE_CONSTRAINTS_RESTRICTIONS;

	// Set Constraint Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTable_ConstraintRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszCatalogRestriction);
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszSchemaRestriction);
	SetRestriction(SIXTH, 6, &m_wszR6, m_pwszTableRestriction);
	SetRestriction(THIRD, 7, &m_wszR7, m_pwszConstraint_TypeRestriction);

	PRVTRACE(L"GetSchemaInfo::TABLE_CONSTRAINTS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTABLE_CONSTRAINTS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TABLE_CONSTRAINTS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszTable_ConstraintRestriction && m_pwszConstraint_TypeRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTABLE_CONSTRAINTS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:TABLE_CONSTRAINTS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);
		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://CONSTRAINT CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// CONSTRAINT SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// CONSTRAINT NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszTable_ConstraintRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
					if(m_pwszTable_ConstraintRestriction)
						wcscpy(m_pwszTable_ConstraintRestriction,(TYPE_WSTR) pColumn->bValue);
				}
				
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE CATALOG
				if(m_restrict & FOURTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes4)
						{
							odtLog << L"VerifyRow_TABLE_CONSTRAINTS:TABLE CATALOG restriction failed\n";
							m_fRes4=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// TABLE SCHEMA
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_TABLE_CONSTRAINTS:TABLE SCHEMA restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 6:	// TABLE NAME
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog <<  L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT NAME restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
				break;
			case 7: // CONSTRAINT TYPE
					if(m_fCaptureRestrictions)
					{
						m_pwszConstraint_TypeRestriction = (WCHAR *) PROVIDER_ALLOC((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszConstraint_TypeRestriction)
							wcscpy(m_pwszConstraint_TypeRestriction,(WCHAR *) pColumn->bValue);
					}

					if(m_restrict & SEVENTH)
					{
						if(!COMPARE(0, wcsicmp((WCHAR* )m_wszR7,(const WCHAR *)pColumn->bValue)))
						{
							if(m_fRes7)
							{
								odtLog <<  L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT NAME restriction failed\n";
								m_fRes7=FALSE;
								fResults = FALSE;
							}
						}
					}
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"UNIQUE")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"PRIMARY KEY")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"FOREIGN KEY"))&&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"CHECK")))
					{
						odtLog << L"VerifyRow_TABLE_CONSTRAINTS:CONSTRAINT TYPE expected UNIQUE/PRIMARYKEY/FOREIGNKEY/CHECK but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 8: // IS_DEFERRABLE
			case 9:	// INITIALLY_DEFERRED
				if(*(TYPE_BOOL *)pColumn->bValue==VARIANT_TRUE)
					PRVTRACE(L"TRUE\n");
				else
					PRVTRACE(L"FALSE\n");
				break;
			case 10: // DESCRIPTION
					PRVTRACE(L"%d\n",(TYPE_WSTR *)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"TABLE CONSTRAINTS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - TABLE CONSTRAINTS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}
	return fResults;
}

//--------------------------------------------------------------------
// TABLE_PRIVILEGES
// 1. Catalog Name
// 2. Schema Name
// 3. Table Name
// 4. Grantor
// 5. Grantee
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TABLE_PRIVILEGES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszTABLE_PRIVILEGES;
	m_rgColumnTypes = (DBTYPE *)rgtypeTABLE_PRIVILEGES;

	// Set the count of columns and restrictions
	m_cColumns = cTABLE_PRIVILEGES;
	m_cRestrictions = cTABLE_PRIVILEGES_RESTRICTIONS;

	// Set Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszGrantorRestriction);
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszGranteeRestriction);

	PRVTRACE(L"GetSchemaInfo::TABLE_PRIVILEGES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTABLE_PRIVILEGES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TABLE_PRIVILEGES
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTABLE_PRIVILEGES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:TABLE_PRIVILEGES:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);
		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://GRANTOR
				if(m_restrict & FOURTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes4)
						{
							odtLog << L"VerifyRow_TABLE_PRIVILEGES:GRANTOR restriction failed\n";
							m_fRes4=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
				break;
			case 2:// GRANTEE
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_TABLE_PRIVILEGES:GRANTEE restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// TABLE CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_TABLE_PRIVILEGES:TABLE CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog << L"VerifyRow_TABLE_PRIVILEGES:TABLE SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// TABLE NAME
				if(m_restrict & THIRD)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes3)
						{
							odtLog <<  L"VerifyRow_TABLE_PRIVILEGES:TABLE NAME restriction failed\n";
							m_fRes3=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 6:	// PRIVILEGE TYPE
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SELECT")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"INSERT")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"DELETE")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"REFERENCES")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"UPDATE")))
					{
						odtLog << L"VerifyRow_TABLE_PRIVILEGES:PRIVILEGE TYPE expected SELECT/INSERT/DELETE/REFERENCES/UPDATE but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 7: // IS_GRANTABLE
				if(*(TYPE_BOOL *)pColumn->bValue==VARIANT_TRUE)
					PRVTRACE(L"TRUE\n");
				else
					PRVTRACE(L"FALSE\n");
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"TABLE PRIVILEGES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - TABLE PRIVILEGES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}

//--------------------------------------------------------------------
// TABLES
// 1. Catalog Name
// 2. Schema Name
// 3. Table Name
// 4. Table Type
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TABLES()
{
	// Set the Schema column Count, Names and Types
	m_cColumns = cTABLES;
	m_rgColumnNames = (WCHAR **) rgwszTABLES;
	m_rgColumnTypes = (DBTYPE *) rgtypeTABLES;
	
	// Set the count of restrictions
	m_cRestrictions = cTABLES_RESTRICTIONS;

	// Set Valid Table Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszTable_TypeRestriction);

	// Set an Invalid Table Restriction
	if(m_restrict & FIFTH) {
		SetRestriction(FIFTH,5, &m_wszR5, m_pwszTableRestriction);
		m_cRestrictions = 5;
	}

	// Set expected row count.  Since we create a table there must be 
	// at least one.
	SetRowCount(MIN_REQUIRED, 1);

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTABLES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TABLES(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind;			// Binding Count
    DATA *	pColumn;		// Data Structure
	CCol	col;
	BOOL	fResults = TRUE;

	// Try to open a rowset on the table
	GetColumnInfo(pData, m_rgDBBINDING);
	
	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTABLES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
		{
			// TABLE_CATALOG
			case 1:
				fResults &= TestReturnData(iRow,pColumn,FIRST,&m_fRes1,m_wszR1);
				break;

			// TABLE_SCHEMA
			case 2:
				fResults &= TestReturnData(iRow,pColumn,SECOND,&m_fRes2,m_wszR2);
				break;

			// TABLE_NAME
			case 3:
				fResults &= TestReturnData(iRow,pColumn,THIRD,&m_fRes3,m_wszR3);
				break;

			// TABLE_TYPE
			case 4:
				fResults &= TestReturnData(iRow,pColumn,FOURTH,&m_fRes4,m_wszR4,FALSE);

				// Check the only spec'ed Types
				if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"ALIAS")) && 
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"TABLE")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYNONYM")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYSTEM TABLE")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"GLOBAL TEMPORARY")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"LOCAL TEMPORARY")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYSTEM VIEW")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"VIEW")))
				{
					odtLog << L"Provider specific TABLE TYPE was returned by the provider: " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}
				break;
			
			// TABLE_GUID
			case 5:
				if(pColumn->sStatus == DBSTATUS_S_OK)
					COMPARE(pColumn->ulLength, sizeof(GUID));
				else if(pColumn->sStatus == DBSTATUS_S_ISNULL)
					COMPARE(pColumn->ulLength, 0);
				break;
			
			// DESCRIPTION
			case 6:
				if(pColumn->sStatus == DBSTATUS_S_ISNULL)
					COMPARE(pColumn->ulLength, 0);
				break;

			// TABLE_PROPID
			case 7:
				if(pColumn->sStatus == DBSTATUS_S_OK)
					COMPARE(pColumn->ulLength, sizeof(ULONG));
				else if(pColumn->sStatus == DBSTATUS_S_ISNULL)
					COMPARE(pColumn->ulLength, 0);
				break;

			// DATE_CREATED
			// DATE_MODIFIED
			case 8:
			case 9:
				if(pColumn->sStatus == DBSTATUS_S_OK)
					COMPARE(pColumn->ulLength, sizeof(DATE));
				else if(pColumn->sStatus == DBSTATUS_S_ISNULL)
					COMPARE(pColumn->ulLength, 0);
				break;

			// PROVIDER SPECIFIC
			default:
				if (iRow == 1)
				{
					if(!m_rgDBCOLUMNINFO[iBind].iOrdinal) 
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"TABLES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - TABLES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
		}
	}

	return fResults;
}

//--------------------------------------------------------------------
// TABLES_INFO
// 1. Catalog Name
// 2. Schema Name
// 3. Table Name
// 4. Table Type
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TABLES_INFO()
{
	// Set the Schema column Count, Names and Types
	m_cColumns = cTABLES_INFO;
	m_rgColumnNames = (WCHAR **) rgwszTABLES_INFO;
	m_rgColumnTypes = (DBTYPE *) rgtypeTABLES_INFO;
	
	// Set the count of restrictions
	m_cRestrictions = cTABLES_INFO_RESTRICTIONS;

	// Set Valid TableInfo Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszTable_TypeRestriction);

	// Set an Invalid TableInfo Restriction
	if(m_restrict & FIFTH) {
		SetRestriction(FIFTH,5, &m_wszR5, m_pwszTableRestriction);
		m_cRestrictions = 5;
	}

	// Set expected row count.  Since we create a table there must be 
	// at least one.
	SetRowCount(MIN_REQUIRED, 1);

	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTABLES_INFO
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TABLES_INFO(ULONG iRow, BYTE * pData)
{
	ULONG 	iBind;			// Binding Count
    DATA *	pColumn;		// Data Structure
	CCol	col;
	BOOL	fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTABLES_INFO <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
		{
			// TABLE_CATALOG
			case 1:
				fResults &= TestReturnData(iRow,pColumn,FIRST,&m_fRes1,m_wszR1);
				break;

			// TABLE_SCHEMA
			case 2:
				fResults &= TestReturnData(iRow,pColumn,SECOND,&m_fRes2,m_wszR2);
				break;

			// TABLE_NAME
			case 3:
				fResults &= TestReturnData(iRow,pColumn,THIRD,&m_fRes3,m_wszR3);
				break;

			// TABLE_TYPE
			case 4:
				fResults &= TestReturnData(iRow,pColumn,FOURTH,&m_fRes4,m_wszR4);

				// Check the only spec'ed Types
				if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"ALIAS")) && 
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"TABLE")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYNONYM")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYSTEM TABLE")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"GLOBAL TEMPORARY")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYSTEM TEMPORARY")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"SYSTEM VIEW")) &&
					(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"VIEW")))
				{
					odtLog << L"VerifyRow_TABLES_INFO:TABLE TYPE expected ALIAS/TABLE/SYNONYM/SYSTEM TABLE/GLOBAL/SYSTEM/VIEW but recieved " 
						<< (TYPE_WSTR)pColumn->bValue << ENDL;
					fResults = FALSE;
				}

				break;
			
			// TABLE_GUID
			case 5:
				break;
			
			// BOOKMARKS
			case 6:
				break;

			// BOOKMARK_TYPE
			// BOOKMARK_DATATYPE
			// BOOKMARK_MAXIMUM
			// BOOKMARK_INFORMATION
			case 7:
			case 8:
			case 9:
			case 10:
				break;

			// TABLE_VERSION
			case 11:
				break;

			// CARDINALITY
			case 12:
				break;

			// DESCRIPTION
			case 13:
				break;

			// TABLE_PROPID
			case 14:
				break;

			// PROVIDER SPECIFIC
			default:
				if (iRow == 1)
				{
					if(!m_rgDBCOLUMNINFO[iBind].iOrdinal) 
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"TABLES_INFO: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - TABLES_INFO provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
		}
	}

	return fResults;
}

//--------------------------------------------------------------------
// TRANSLATIONS
// 1. Catalog Name
// 2. Schema Name
// 3. Translation Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TRANSLATIONS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszTRANSLATIONS;
	m_rgColumnTypes = (DBTYPE *)rgtypeTRANSLATIONS;

	// Set the count of columns and restrictions
	m_cColumns = cTRANSLATIONS;
	m_cRestrictions = cTRANSLATIONS_RESTRICTIONS;
	
	// Set Transalation Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTranslationReplace);

	PRVTRACE(L"GetSchemaInfo::TRANSLATIONS\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTRANSLATIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TRANSLATIONS
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszTranslationReplace)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTRANSLATIONS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:TRANSLATIONS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
				switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
				{
				case 1:// TABLE_CATALOG
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog <<  L"VerifyRow_TRANSLATIONS: TRANSLATION CATALOG restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 2:// TABLE_SCHEMA
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog <<  L"VerifyRow_TRANSLATIONS: TRANSLATION SCHEMA restriction failed\n";
								m_fRes2=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 3:// Translation Name
					if(m_fCaptureRestrictions)
					{
						m_pwszTranslationReplace = (TYPE_WSTR) PROVIDER_ALLOC
							((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszTranslationReplace)
							wcscpy(m_pwszTranslationReplace,(TYPE_WSTR) pColumn->bValue);
					}

					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog <<  L"VerifyRow_TRANSLATIONS: TRANSLATION NAME restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
					break;
				case 4:	// SOURCE CHARACTER SET CATALOG
				case 5: // SOURCE CHARACTER SET SCHEMA
				case 6: // SOURCE CHARACTER SET NAME
				case 7: // TARGET CHARACTER SET CATALOG
				case 8: // TARGET CHARACTER SET SCHEMA
				case 9: // TARGET CHARACTER SET NAME
						PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
						break;
				default:
					// We found a column not spec'd for this schema rowset, print a warning.
					if (iRow == 1)
					{
						if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
						{
							if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
								odtLog << L"TRANSLATIONS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
						}
						else
							odtLog << L"Warning - TRANSLATIONS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
					}
					break;
				}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}


//--------------------------------------------------------------------
// TRUSTEE
// 1. TRUSTEE_NAME
// 2. TRUSTEE_GUID
// 3. TRUSTEE_TYPE
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_TRUSTEE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszTRUSTEE;
	m_rgColumnTypes = (DBTYPE *)rgtypeTRUSTEE;

	// Set the count of columns and restrictions
	m_cColumns = cTRUSTEE;
	m_cRestrictions = cTRUSTEE_RESTRICTIONS;

	odtLog << L"*** Add restriction values for DBSCHEMA_TRUSTEE.\n";

	// Set TRUSTEE Restrictions
//	SetRestriction(FIRST, 1, &m_wszR1, m_pwszTrusteeName);
//	SetRestriction(SECOND,2, &m_wszR2, m_pwszTrusteeGUID);
//	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTrusteePROPID);
//	SetRestriction(FOURTH,4, &m_wszR4, m_pwszTrusteeType);

	PRVTRACE(L"GetSchemaInfo::TRUSTEE\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowTRANSLATIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_TRUSTEE
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszTranslationReplace)
		return FALSE;

	odtLog << L"*** Currently not testing values returned from DBSCHEMA_TRUSTEE.\n";

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cTRUSTEE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:TRUSTEE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
				switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
				{
				case 1:// TRUSTEE_NAME
/*
					if(m_restrict & FIRST)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes1)
							{
								odtLog <<  L"VerifyRow_TRUSTEE: TRUSTEE_NAME restriction failed\n";
								m_fRes1=FALSE;
								fResults = FALSE;
							}
						}
					}
*/
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 2:// TRUSTEE_GUID
/*
					if(m_restrict & SECOND)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes2)
							{
								odtLog <<  L"VerifyRow_TRUSTEE: TRUSTEE_GUID restriction failed\n";
								m_fRes2=FALSE;
								fResults = FALSE;
							}
						}
					}
*/
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
				case 3:// TRUSTEE_PROPID
/*
					if(m_fCaptureRestrictions)
					{
						m_pwszTranslationReplace = (TYPE_WSTR) PROVIDER_ALLOC
							((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszTranslationReplace)
							wcscpy(m_pwszTranslationReplace,(TYPE_WSTR) pColumn->bValue);
					}

					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog <<  L"VerifyRow_TRUSTEE: TRUSTEE_PROPID restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
*/
					PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
					break;
				case 4:	// TRUSTEE_TYPE
						PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
						break;
				default:
					// We found a column not spec'd for this schema rowset, print a warning.
					if (iRow == 1)
					{
						if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
						{
							if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
								odtLog << L"TRANSLATIONS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
						}
						else
							odtLog << L"Warning - TRANSLATIONS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
					}
					break;
				}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}


//--------------------------------------------------------------------
// USAGE_PRIVILGES
// 1. Catalog Name
// 2. Schema Name
// 3. Object Name
// 4. Object Type
// 5. Grantor
// 6. Grantee
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_USAGE_PRIVILEGES()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszUSAGE_PRIVILEGES;
	m_rgColumnTypes = (DBTYPE *)rgtypeUSAGE_PRIVILEGES;

	// Set the count of columns and restrictions
	m_cColumns = cUSAGE_PRIVILEGES;
	m_cRestrictions = cUSAGE_PRIVILEGES_RESTRICTIONS;
	
	// Set Object Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszObjectRestriction);
	SetRestriction(FOURTH,4, &m_wszR4, m_pwszObject_TypeRestriction);
	SetRestriction(FIFTH, 5, &m_wszR5, m_pwszGrantorRestriction);
	SetRestriction(SIXTH, 6, &m_wszR6, m_pwszGranteeRestriction);

	PRVTRACE(L"GetSchemaInfo::USAGE_PRIVILGES\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowUSAGE_PRIVILEGES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_USAGE_PRIVILEGES
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszObjectRestriction && m_pwszObject_TypeRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cUSAGE_PRIVILEGES <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:USAGE_PRIVILEGES:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://GRANTOR
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog <<  L"VerifyRow_USAGE_PRIVILEGES:GRANTOR restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// GRANTEE
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog << L"VerifyRow_USAGE_PRIVILEGES:GRANTEE restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// OBJECT CATALOG
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_USAGE_PRIVILEGES:OBJECT CATALOG restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// OBJECT SCHEMA
				if(m_restrict & SECOND)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR2,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes2)
						{
							odtLog <<  L"VerifyRow_USAGE_PRIVILEGES:OBJECT SCHEMA restriction failed\n";
							m_fRes2=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 5:// OBJECT NAME
					if(m_fCaptureRestrictions)
					{
						m_pwszObjectRestriction = (WCHAR *) PROVIDER_ALLOC
							((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszObjectRestriction)
							wcscpy(m_pwszObjectRestriction,(TYPE_WSTR) pColumn->bValue);
					}
					
					if(m_restrict & THIRD)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR3,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes3)
							{
								odtLog << L"VerifyRow_USAGE_PRIVILEGES:OBJECT NAME restriction failed\n";
								m_fRes3=FALSE;
								fResults = FALSE;
							}
						}
					}
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 6:	// OBJECT TYPE
					if(m_restrict & FOURTH)
					{
						if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR4,(TYPE_WSTR)pColumn->bValue)))
						{
							if(m_fRes4)
							{
								odtLog << L"VerifyRow_USAGE_PRIVILEGES:OBJECT TYPE restriction failed\n";
								m_fRes4=FALSE;
								fResults = FALSE;
							}
						}
					}
					if(m_fCaptureRestrictions)
					{
						m_pwszObject_TypeRestriction = (WCHAR *) PROVIDER_ALLOC
							((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR)) + sizeof(WCHAR));
						if(m_pwszObject_TypeRestriction)
							wcscpy(m_pwszObject_TypeRestriction,(TYPE_WSTR) pColumn->bValue);
					}
					if( (0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"DOMAIN")) && 
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"CHARACTER SET")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"COLLATION")) &&
						(0!=wcscmp((TYPE_WSTR)pColumn->bValue,L"TRANSLATION")))
					{
						odtLog << L"VerifyRow_USAGE_PRIVILEGES:OBJECT TYPE expected DOMAIN/CHARACTERSET/COLLATION/TRANSLATION but recieved " 
							<< (TYPE_WSTR)pColumn->bValue << ENDL;
						fResults = FALSE;
					}
					PRVTRACE(L"'%s'\n",(WCHAR *)pColumn->bValue);
					break;
			case 7: // PRIVILEGE TYPE
  					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 8: // IS_GRANTABLE
				if(*(TYPE_BOOL *)pColumn->bValue==VARIANT_TRUE)
					PRVTRACE(L"TRUE\n");
				else
					PRVTRACE(L"FALSE\n");
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"USAGE PRIVILEGES: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - USAGE PRIVILEGES provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
			
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}

//--------------------------------------------------------------------
// VIEW_COLUMN_USAGE
// 1. Catalog Name
// 2. Schema Name
// 3. View Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_VIEW_COLUMN_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszVIEW_COLUMN_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeVIEW_COLUMN_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cVIEW_COLUMN_USAGE;
	m_cRestrictions = cVIEW_COLUMN_USAGE_RESTRICTIONS;
	
	// Set View Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszViewRestriction);

	PRVTRACE(L"GetSchemaInfo::VIEW_COLUMN_USAGE\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowVIEW_COLUMN_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_VIEW_COLUMN_USAGE
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cVIEW_COLUMN_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:VIEW_COLUMN_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://VIEW CATALOG
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_VIEW_COLUMN_USAGE: VIEW CATALOG restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// VIEW SCHEMA
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog << L"VerifyRow_VIEW_COLUMN_USAGE:VIEW SCHEMA restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// VIEW NAME
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_VIEW_COLUMN_USAGE:VIEW NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE CATALOG
			case 5:// TABLE SCHEMA
			case 6:// TABLE NAME
			case 7:// COLUMN NAME
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 8: // COLUMN GUID
  					PRVTRACE(L"\n");
					break;
			case 9: // COLUMN PROPID
					PRVTRACE(L"%d\n",*(TYPE_UI4 *)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"VIEW COLUMN USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - VIEW COLUMN USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}

//--------------------------------------------------------------------
// VIEW_TABLE_USAGE
// 1. Catalog Name
// 2. Schema Name
// 3. View Name
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_VIEW_TABLE_USAGE()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszVIEW_TABLE_USAGE;
	m_rgColumnTypes = (DBTYPE *)rgtypeVIEW_TABLE_USAGE;

	// Set the count of columns and restrictions
	m_cColumns = cVIEW_TABLE_USAGE;
	m_cRestrictions = cVIEW_TABLE_USAGE_RESTRICTIONS;
	
	// Set View Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszViewRestriction);

	PRVTRACE(L"GetSchemaInfo::VIEW_TABLE_USAGE\n");
	return TRUE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowVIEW_TABLE_USAGE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_VIEW_TABLE_USAGE
(
	ULONG iRow,
	BYTE * pData
)
{

	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// don't need to go farther if I have what I'm looking for
	if(m_fCaptureRestrictions && m_pwszViewRestriction)
		return FALSE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cVIEW_TABLE_USAGE <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);
		
		PRVTRACE(L"Row[%lu],Col[%s]:VIEW_TABLE_USAGE:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://VIEW CATALOG
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_VIEW_TABLE_USAGE: VIEW CATALOG restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// VIEW SCHEMA
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog << L"VerifyRow_VIEW_TABLE_USAGE:VIEW SCHEMA restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// VIEW NAME
				if(m_fCaptureRestrictions)
				{
					m_pwszViewRestriction = (WCHAR *) PROVIDER_ALLOC
						((wcslen((WCHAR *) pColumn->bValue)*sizeof(WCHAR*)) + sizeof(WCHAR));
					if(m_pwszViewRestriction)
						wcscpy(m_pwszViewRestriction,(TYPE_WSTR) pColumn->bValue);
				}

				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_VIEW_TABLE_USAGE:VIEW NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// TABLE CATALOG
			case 5:// TABLE SCHEMA
			case 6:// TABLE NAME
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"VIEW TABLE USAGE: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - VIEW TABLE USAGE provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	
	return fResults;
}

//--------------------------------------------------------------------
// VIEWS
// 1. Table Catalog 
// 2. Table Schema 
// 3. Table View 
//--------------------------------------------------------------------
BOOL CSchemaTest::PrepareParams_VIEWS()
{
	// Set the Schema column Names and Types
	m_rgColumnNames = (WCHAR **)rgwszVIEWS;
	m_rgColumnTypes = (DBTYPE *)rgtypeVIEWS;

	// Set the count of columns and restrictions
	m_cColumns = cVIEWS;
	m_cRestrictions = cVIEWS_RESTRICTIONS;
	
	// Set View Restrictions
	SetRestriction(FIRST, 1, &m_wszR1, m_pwszCatalogRestriction);
	SetRestriction(SECOND,2, &m_wszR2, m_pwszSchemaRestriction);
	SetRestriction(THIRD, 3, &m_wszR3, m_pwszTableRestriction);
		
	PRVTRACE(L"GetSchemaInfo::VIEWS\n");
	return TRUE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	VerifyRowVIEWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRow_VIEWS
(
	ULONG iRow,
	BYTE * pData
)
{
	ULONG 			iBind;			// Binding Count
    DATA *			pColumn;		// Data Structure
	CCol			col;
	BOOL			fResults = TRUE;

	// Check the count of columns returned
	if(iRow == 1)
		COMPARE(cVIEWS <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// check each column we're bound to.
	for (iBind=0; iBind < m_cDBBINDING; iBind++)
	{
		// grab column
		pColumn = (DATA *) (pData + m_rgDBBINDING[iBind].obStatus);

		PRVTRACE(L"Row[%lu],Col[%s]:VIEWS:", iRow, m_rgDBCOLUMNINFO[iBind].pwszName);

		if(pColumn->sStatus==DBSTATUS_S_OK)
		{
			switch(m_rgDBCOLUMNINFO[iBind].iOrdinal)
			{
			case 1://TABLE CATALOG
				if(m_restrict & FIFTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR5,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes5)
						{
							odtLog << L"VerifyRow_VIEWS: TABLE CATALOG restriction failed\n";
							m_fRes5=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 2:// TABLE SCHEMA
				if(m_restrict & SIXTH)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR6,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes6)
						{
							odtLog <<  L"VerifyRow_VIEWS:TABLE SCHEMA restriction failed\n";
							m_fRes6=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 3:	// TABLE NAME
				if(m_restrict & FIRST)
				{
					if(!COMPARE(0, wcsicmp((TYPE_WSTR)m_wszR1,(TYPE_WSTR)pColumn->bValue)))
					{
						if(m_fRes1)
						{
							odtLog << L"VerifyRow_VIEWS:TABLE NAME restriction failed\n";
							m_fRes1=FALSE;
							fResults = FALSE;
						}
					}
				}
				PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
				break;
			case 4:// VIEW_DEFINITION
			case 7:// DESCRIPTION
					PRVTRACE(L"'%s'\n",(TYPE_WSTR)pColumn->bValue);
					break;
			case 5:// CHECK_OPTION
			case 6:// IS_UPDATEABLE
				if(*(TYPE_BOOL *)pColumn->bValue==VARIANT_TRUE)
					PRVTRACE(L"TRUE\n");
				else
					PRVTRACE(L"FALSE\n");
				break;
			// DATE_CREATED
			// DATE_MODIFIED
			case 8:
			case 9:
				break;
			default:
				// We found a column not spec'd for this schema rowset, print a warning.
				if (iRow == 1)
				{
					if (m_rgDBCOLUMNINFO[iBind].iOrdinal == 0)
					{
						if(!GetProperty(DBPROP_BOOKMARKS,DBPROPSET_ROWSET,m_pIRowset))
							odtLog << L"VIEWS: Bookmark column was found but the DBPROP_BOOKMARKS was not set.\n";
					}
					else
						odtLog << L"Warning - VIEWS provider specific column name: " << m_rgDBCOLUMNINFO[iBind].pwszName << "\n";
				}
				break;
			}
		}
		else if (pColumn->sStatus==DBSTATUS_S_TRUNCATED)
		{
			// Have to flag error.
			odtLog << L"DBSTATUS_S_TRUNCATED: " << (TYPE_WSTR)pColumn->bValue << L"\n";
			// We shouldn't be flagging an error for LONG columns, we expect truncation.
			if (!(m_rgDBCOLUMNINFO[iBind].dwFlags & DBCOLUMNFLAGS_ISLONG))
				fResults = FALSE;
		}
		else
		{
			PRVTRACE(L"%s=",m_rgDBCOLUMNINFO[iBind].pwszName);
			RowsetBindingStatus((DBSTATUSENUM)pColumn->sStatus);
		}
	}

	return fResults;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::CheckResults(IUnknown * pIRowset, IID iid)
{	
	BOOL fResult = TRUE;

	// if I didn't get rowset back when I expected FIRST, return error
	if(SUCCEEDED(m_HR))
	{ 
		if( ((m_iid==IID_IRowset) && (!m_pIRowset)) ||
			((m_iid==IID_IAccessor) && (!m_pIAccessor)) ||
			((m_iid==IID_IRowsetInfo) && (!m_pIRowsetInfo)) ||
			((m_iid==IID_IColumnsInfo) && (!m_pIColumnsInfo))||
			((m_iid==IID_IRowsetChange) && (!m_pIRowsetChange)) )
		{	
			odtLog <<  L"CheckResults:expected rowset but didn't get FIRST\n";
			return FALSE;
		}

   		// check that the riid pointer can be used,
		// regardless of which riid it is
		fResult &= CheckRIID(pIRowset, iid);
		fResult &= VerifyRowset(pIRowset);			
	}
	else // make sure rowset if empty
	{
		if( ((m_iid==IID_IRowset) && (m_pIRowset)) ||
			((m_iid==IID_IAccessor) && (m_pIAccessor)) ||
			((m_iid==IID_IRowsetInfo) && (m_pIRowsetInfo))||
			((m_iid==IID_IColumnsInfo) && (m_pIColumnsInfo))||
			((m_iid==IID_IRowsetChange) && (m_pIRowsetChange)))
		{
			odtLog << L"CheckResults: method returned error and valid rowset pointer!\n";
			return FALSE;
		}
	}

	return fResult;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CheckAgainstIColumnsInfo
//
// Checks the DBCOLUMNINFO array returned by IColumnsInfo::GetColumnsInfo against
// the what the spec says the column name, type and order should be
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::CheckAgainstIColumnsInfo()
{
	BOOL	fResult			= TRUE;
	BOOL	fCountNotChecked= FALSE;
	ULONG	ulIndex			= 0;

	COMPARE(m_cColumns <= (m_cDBCOLUMNINFO - !m_rgDBCOLUMNINFO[0].iOrdinal), TRUE);

	// Make sure the rowset is not null
	if((!m_pIRowset) || (!m_cDBCOLUMNINFO) || (!m_rgDBCOLUMNINFO) || (!m_pStringsBuffer)) {
		odtLog << L"CheckAgainstIColumnsInfo:Either rowset or DBCOLUMNINFO variables are null."<<ENDL;
		return FALSE;
	}

	// Check for the Bookmark
	if(!m_rgDBCOLUMNINFO[0].iOrdinal)
		m_fBOOKMARK_FOUND=TRUE;
	else
		m_fBOOKMARK_FOUND=FALSE;
	
	// Check column ordering, column name and data type, skip bookmark column
	for(ulIndex = (0 + m_fBOOKMARK_FOUND); ulIndex < m_cDBCOLUMNINFO; ulIndex ++)
	{
		// only check spec'd columns, not any provider-specific columns
		if((ulIndex-m_fBOOKMARK_FOUND) < m_cColumns)
		{
			// Check column ordinal
			if(m_rgDBCOLUMNINFO[ulIndex].iOrdinal != (ulIndex + 1 - m_fBOOKMARK_FOUND))
			{
				odtLog << L"ORDINAL ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex
						<< L"]: Expected "<< ulIndex  	<< L", Received "	<< m_rgDBCOLUMNINFO[ulIndex].iOrdinal	<< ENDL;
				fResult = FALSE;
			}

			// If there is a name in this column, check to make sure it is correct
			if( (m_rgDBCOLUMNINFO[ulIndex].pwszName) && 
				(!COMPARE(0, wcsicmp(m_rgDBCOLUMNINFO[ulIndex].pwszName,m_rgColumnNames[ulIndex-m_fBOOKMARK_FOUND]))) )
			{
				odtLog << L"NAME ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L","
					<< ulIndex 	<< L"]: Expected " 	<< 	m_rgColumnNames[ulIndex-m_fBOOKMARK_FOUND] << L", Received "
					<< m_rgDBCOLUMNINFO[ulIndex].pwszName << ENDL;
				fResult = FALSE;
			}

			// Check column type
			if(m_rgDBCOLUMNINFO[ulIndex].wType != m_rgColumnTypes[ulIndex-m_fBOOKMARK_FOUND])
			{
				odtLog << L"TYPE ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  << L"]: Expected "
						<< 	m_rgColumnTypes[ulIndex-m_fBOOKMARK_FOUND] 	<< L", Received " << m_rgDBCOLUMNINFO[ulIndex].wType << ENDL;
				fResult = FALSE;
			}

			// Check column size
			if((!IsFixedLength(m_rgDBCOLUMNINFO[ulIndex].wType)) && (!m_rgDBCOLUMNINFO[ulIndex].ulColumnSize))
			{
				odtLog << L"COLUMNSIZE ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  << L"]: "
						<< L"Column Size for variable length Column is ZERO, expected non-zero." << ENDL;
				fResult = FALSE;
			}
			else if((IsFixedLength(m_rgDBCOLUMNINFO[ulIndex].wType)) && 
					((ULONG)GetDBTypeSize(m_rgDBCOLUMNINFO[ulIndex].wType) != m_rgDBCOLUMNINFO[ulIndex].ulColumnSize))
			{
				odtLog << L"COLUMNSIZE ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  << L"]: Expected "
						<< L"Column Size for the Column is incorrect. " << ENDL;
				fResult = FALSE;
			}

			// Check column precision and scale
			if(IsNumericType(m_rgDBCOLUMNINFO[ulIndex].wType))
			{
				ULONG MaxPrecision = 0;

				// Switch on the Type
				switch(m_rgDBCOLUMNINFO[ulIndex].wType)
				{
					case DBTYPE_I1:
					case DBTYPE_UI1:
						MaxPrecision = 3;
						break;
					case DBTYPE_I2:
					case DBTYPE_UI2:
						MaxPrecision = 5;
						break;
					case DBTYPE_R4:
						MaxPrecision = 7;
						break;
					case DBTYPE_I4:
					case DBTYPE_UI4:
						MaxPrecision = 10;
						break;
					case DBTYPE_R8:
						MaxPrecision = 16;
						break;
					case DBTYPE_I8:
					case DBTYPE_CY:
						MaxPrecision = 19;
						break;
					case DBTYPE_UI8:
						MaxPrecision = 20;
						break;
				}

				// Precision is valid
				if (((m_rgDBCOLUMNINFO[ulIndex].wType != DBTYPE_DECIMAL) && 
					 (m_rgDBCOLUMNINFO[ulIndex].wType != DBTYPE_NUMERIC)) && 
					(MaxPrecision != m_rgDBCOLUMNINFO[ulIndex].bPrecision) )
				{
					odtLog << L"PRECISION ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  << L"]: Expected "
							<< 	MaxPrecision 	<< L", Received " << m_rgDBCOLUMNINFO[ulIndex].bPrecision << ENDL;
					fResult = FALSE;
				}

				// Scale is  ~0
				if (((m_rgDBCOLUMNINFO[ulIndex].wType != DBTYPE_DECIMAL) && 
					 (m_rgDBCOLUMNINFO[ulIndex].wType != DBTYPE_NUMERIC)) && 
					((BYTE)(~0) != m_rgDBCOLUMNINFO[ulIndex].bScale) )
				{
					odtLog << L"SCALE ["	<< m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  
						<< L"]: Expected the Scale to be ~0" << L", Received " << m_rgDBCOLUMNINFO[ulIndex].bScale << ENDL;
					fResult = FALSE;
				}
			}
			else
			{
				// Precision and scale are ~0
				if (((BYTE)(~0) != m_rgDBCOLUMNINFO[ulIndex].bPrecision) || ((BYTE)(~0) != m_rgDBCOLUMNINFO[ulIndex].bScale))
				{
					odtLog <<L"PRECISION & SCALE [" << m_rgDBCOLUMNINFO[ulIndex].pwszName << L"," << ulIndex  <<"]:non-numeric expect ~0."<<ENDL;
					fResult = FALSE;
				}
			}
		}
	}

	return fResult;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	CheckRIID, QI for every mandatory interface on this object
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::CheckRIID(IUnknown * pColRowset, IID iid)
{
	HRESULT		hr			= E_FAIL;
	ULONG		cErrors		= 0;
	IUnknown*	pInterface	= NULL;

	if(!pColRowset || iid == IID_NULL)
		return FALSE;
	
	// QI for IID_IRowset
	if(FAILED(hr=pColRowset->QueryInterface(IID_IRowset,(void **) &pInterface)))
		cErrors++;
	
	SAFE_RELEASE(pInterface);

	// QI for IID_IAccessor
	if(FAILED(hr=pColRowset->QueryInterface(IID_IAccessor,(void **) &pInterface)))
		cErrors++;
	
	SAFE_RELEASE(pInterface);

	// QI for IID_IColumnsInfo
	if(FAILED(hr=pColRowset->QueryInterface(IID_IColumnsInfo,(void **) &pInterface)))
		cErrors++;
	
	SAFE_RELEASE(pInterface);
	
	// QI for IID_IRowsetInfo
	if(FAILED(hr=pColRowset->QueryInterface(IID_IRowsetInfo,(void **) &pInterface)))
		cErrors++;
	
	SAFE_RELEASE(pInterface);

	// QI for IID_IConvertType
	if(FAILED(hr=pColRowset->QueryInterface(IID_IConvertType,(void **) &pInterface)))
		cErrors++;
	
	SAFE_RELEASE(pInterface);

	if(!cErrors)
		return TRUE;
	else 
		return FALSE;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// VerifyRowset, verify rowset and row order
//
// 1) Verify Maxlength, Precision, and Scale for non-applicable datatypes
// 2) Check ComputeMode
// 3) Order of rows is order of columns in query
//
// ----------------------------------------------
// 1. IColumnsInfo::GetColumnInfo, build binding (from GetAccessorAndBindings)
// 2. IAccessor::CreateAccessor (from GetAccessorAndBindings)
// Loop
//    3. IRowset::GetNextRows
//    4. IRowset::GetData
//    5. IRowset::ReleaseRows
// End Loop
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BOOL CSchemaTest::VerifyRowset(IUnknown * pIUnknown)
{
	BOOL		fVerifyRow = TRUE;
	BOOL		fFirstTimeThruLoop = TRUE;
	HRESULT		hr = E_FAIL;
	BOOL		fReleaseRowset = FALSE;			// whether I should release pIRowset;
	HACCESSOR	hAccessor=DB_NULL_HACCESSOR;	// handle to accessor
	ULONG		iBind = 0;						// current binding
	ULONG		iRow = 0;						// current row
	ULONG		cbRowSize = 0;					// size of row
	ULONG		cRowsObtained = 0;				// total rows obtained from getnextrows
	ULONG		cTotalRows = 0;					// total row count
	BYTE *		pRow = NULL;					// row of Data
	HROW *		prghRows = NULL;					// array of hrows
	IAccessor * pIAccessor = NULL;
	ULONG		ulRowsToVerify;

	m_fResult = FALSE;
	m_fRes1=m_fRes2=m_fRes3=m_fRes4=m_fRes5=m_fRes6=m_fRes7=TRUE;

	// if I don't have the rowset pointer, I need to go get FIRST
	if (!pIUnknown)
	{
		if(m_iid == IID_IColumnsInfo)
			pIUnknown = m_pIColumnsInfo;
		else if(m_iid == IID_IAccessor)
			pIUnknown = m_pIAccessor;
		else if(m_iid == IID_IRowsetChange)
			pIUnknown = m_pIRowsetChange;
		else if(m_iid == IID_IRowsetInfo)
			pIUnknown = m_pIRowsetInfo;
		else if((m_iid == IID_IRowset) && (!m_pIRowset))
			return FALSE;
	}
	
	if(pIUnknown && !m_pIRowset && FAILED(hr=pIUnknown->QueryInterface(IID_IRowset,(void **)&m_pIRowset)) )
		return FALSE;

	if (!VerifyInterface(m_pIRowset, IID_IAccessor, ROWSET_INTERFACE, (IUnknown**)&pIAccessor))
		goto CLEANUP;

	fReleaseRowset = TRUE;

	// get bindings and column info
	if(!CHECK(hr=GetAccessorAndBindings(
		m_pIRowset,									// @parmopt [IN]  Rowset to create Accessor for
		DBACCESSOR_ROWDATA,							// @parmopt [IN]  Properties of the Accessor
		&hAccessor,									// @parmopt [OUT] Accessor created
		&m_rgDBBINDING,								// @parmopt [OUT] Array of DBBINDINGS
		&m_cDBBINDING,								// @parmopt [OUT] Count of bindings
		&cbRowSize,									// @parmopt [OUT] Length of a row, DATA	
		DBPART_VALUE|DBPART_STATUS |DBPART_LENGTH,
		ALL_COLS_BOUND,								// @parmopt [IN]  Which columns will be used in the bindings
		FORWARD,									// @parmopt [IN]  Order to bind columns in accessor												
		NO_COLS_BY_REF,								// @parmopt [IN]  Which column types to bind by reference
		&m_rgDBCOLUMNINFO,							// @parmopt [OUT] Array of DBCOLUMNINFO
		&m_cDBCOLUMNINFO,							// @parmopt [OUT] Count of Columns, also count of ColInfo elements
		&m_pStringsBuffer,
		DBTYPE_EMPTY,								// @parmopt [IN]  Modifier to be OR'd with each binding type.
		0,											// @parmopt [IN]  Used only if eColsToBind = USE_COLS_TO_BIND_ARRAY
		NULL,										// @parmopt [IN]  Used only if eColsToBind = USE_COLS_TO_BIND_ARRAY												 
		NULL,										// @parmopt [IN]  Corresponds to what ordinals are specified for each binding, if 
		NO_COLS_OWNED_BY_PROV, 						// @parmopt [IN]  Which columns' memory is to be owned by the provider
		DBPARAMIO_NOTPARAM,							// @parmopt [IN]  Parameter kind specified for all bindings 
		BLOB_LONG,									// @parmopt [IN]  how to bind BLOB Columns
		NULL),S_OK))								// @parmopt [OUT] returned status array from CreateAccessor
		goto CLEANUP;

	// Don't need to check if only capturing restrictions
	if(!m_fCaptureRestrictions)
		fVerifyRow &= CheckAgainstIColumnsInfo();

	// check if we need to go on
	if((!m_cDBBINDING) || (!m_cDBCOLUMNINFO) || (!m_rgDBBINDING) || (!m_rgDBCOLUMNINFO))
		goto CLEANUP;

	if(m_fPassUnsupportedRestrictions)
	{
		m_fResult = TRUE;
		goto CLEANUP;
	}

	// create space for row of data
	pRow = (BYTE *) PROVIDER_ALLOC(cbRowSize);
	if(!pRow)
		goto CLEANUP;

	if(GetModInfo()->GetDebugMode() & DEBUGMODE_FULL &&
		(m_guid == DBSCHEMA_COLUMNS ||
		m_guid == DBSCHEMA_TABLES))
		ulRowsToVerify = 90000;
	else
		// This should be larger than NUMROWS_CHUNK to force at least one additional
		// GetNextRows call below.
		ulRowsToVerify = 100;
 
	// Process all the rows, NUMROWS_CHUNK rows at a time
	while(1)
	{
		// get rows to process
		hr=GetNextRows(0,30,&cRowsObtained,&prghRows);


		TEST4C_(hr, S_OK, DB_S_ENDOFROWSET, DB_S_STOPLIMITREACHED, DB_S_ROWLIMITEXCEEDED);


		// verify that we have rows to process
		if(cRowsObtained==0)
		{
			if (CHECK(hr, DB_S_ENDOFROWSET))
				break;
			else
				goto CLEANUP;
		}

		// Only verify required number of rows
		if (cTotalRows < ulRowsToVerify)
		{
			// Make sure we don't call verify with no valid restrictions.
			ASSERT((m_restrict != ALLRES) || (m_currentBitMask == 0));

			// Loop over rows obtained, getting data for each
			for(iRow=0;iRow<cRowsObtained;iRow++)
			{
				cTotalRows++;

				// get row
				if(FAILED(hr=m_pIRowset->GetData(prghRows[iRow],hAccessor,pRow)))
					goto CLEANUP;

				// make sure we got the row
				if(pRow==NULL)
					goto CLEANUP;

 				// do something with row
				fVerifyRow &= VerifyRow(m_guid,cTotalRows,pRow);
			}
		}
		else
			cTotalRows += cRowsObtained;

		if (!CHECK(hr=m_pIRowset->ReleaseRows(cRowsObtained,prghRows,NULL,NULL,NULL),S_OK))
			goto CLEANUP;

		fFirstTimeThruLoop = FALSE;
	}

	switch (m_eRowCount)
	{	
		case MIN_VALUE:
			// Warn if we didn't get any rows but expected some
			if (m_ulRowCount && !cTotalRows)
				odtLog << L"Warning - this rowset didn't return any rows so no values were checked.\n";

			// Fail if we got some rows but less than minimum expected
			if (cTotalRows)
				TESTC(cTotalRows >= m_ulRowCount);
			break;
		case MIN_REQUIRED:
			// Fail if we got less than minimum expected
			TESTC(cTotalRows >= m_ulRowCount);
			break;
		case EXACT_VALUE:
	//		ASSERT(cTotalRows == m_ulRowCount);
			TESTC(cTotalRows == m_ulRowCount);
			break;
		default:
			ASSERT(!L"Unexpected row count value for schema rowset.");
	}

	// If we made it to here and verified each row then set success
	if (fVerifyRow)
		m_fResult = TRUE;

CLEANUP:

	// Clear any table name information that might be left over
	SAFE_FREE(m_pwszTableName);

	if (pIAccessor && hAccessor != DB_NULL_HACCESSOR)
		CHECK(pIAccessor->ReleaseAccessor(hAccessor, NULL), S_OK);

	SAFE_RELEASE(pIAccessor);
	SAFE_RELEASE(m_pIRowset);

	m_cDBCOLUMNINFO = 0;
	m_cDBBINDING = 0;

	// Free the memory
	PROVIDER_FREE(m_rgDBCOLUMNINFO);
	PROVIDER_FREE(m_pStringsBuffer);
	PROVIDER_FREE(m_rgDBBINDING);
	PROVIDER_FREE(pRow);
	PROVIDER_FREE(prghRows);

	return m_fResult;
}

ULONG CSchemaTest::SchemaCatalogRestriction(ULONG ulIndex)
{
	GUID guidSchema = m_rgSchemas[ulIndex];

	// If the GUID is one for which we know a catalog restriction exists then return FIRST.
	// Catalog restriction is always the first one.
	// This is most of them but we can't use negative logic 'cause we might get a guid we
	// don't recognize.
	if (guidSchema == DBSCHEMA_ASSERTIONS ||
		guidSchema == DBSCHEMA_CATALOGS ||
		guidSchema == DBSCHEMA_CHARACTER_SETS ||
		guidSchema == DBSCHEMA_CHECK_CONSTRAINTS ||
		guidSchema == DBSCHEMA_COLLATIONS ||
		guidSchema == DBSCHEMA_COLUMN_DOMAIN_USAGE ||
		guidSchema == DBSCHEMA_COLUMN_PRIVILEGES ||
		guidSchema == DBSCHEMA_COLUMNS ||
		guidSchema == DBSCHEMA_CONSTRAINT_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_CONSTRAINT_TABLE_USAGE ||
		guidSchema == DBSCHEMA_FOREIGN_KEYS ||
		guidSchema == DBSCHEMA_INDEXES ||
		guidSchema == DBSCHEMA_KEY_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_PRIMARY_KEYS ||
		guidSchema == DBSCHEMA_PROCEDURE_COLUMNS ||
		guidSchema == DBSCHEMA_PROCEDURE_PARAMETERS ||
		guidSchema == DBSCHEMA_PROCEDURES ||
		guidSchema == DBSCHEMA_REFERENTIAL_CONSTRAINTS ||
		guidSchema == DBSCHEMA_SCHEMATA ||
		guidSchema == DBSCHEMA_STATISTICS ||
		guidSchema == DBSCHEMA_TABLE_CONSTRAINTS ||
		guidSchema == DBSCHEMA_TABLE_PRIVILEGES ||
		guidSchema == DBSCHEMA_TABLES ||
		guidSchema == DBSCHEMA_TABLES_INFO ||
		guidSchema == DBSCHEMA_TRANSLATIONS ||
		guidSchema == DBSCHEMA_USAGE_PRIVILEGES ||
		guidSchema == DBSCHEMA_VIEW_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_VIEW_TABLE_USAGE ||
		guidSchema == DBSCHEMA_VIEWS)
			return FIRST;

	// else return ZERO restrictions
	return ZERO;
}

ULONG CSchemaTest::SchemaSchemaRestriction(ULONG ulIndex)
{
	GUID guidSchema = m_rgSchemas[ulIndex];

	// If the GUID is one for which we know a catalog restriction exists then return FIRST.
	// Catalog restriction is always the first one.
	// This is most of them but we can't use negative logic 'cause we might get a guid we
	// don't recognize.
	if (guidSchema == DBSCHEMA_ASSERTIONS ||
		guidSchema == DBSCHEMA_CHARACTER_SETS ||
		guidSchema == DBSCHEMA_CHECK_CONSTRAINTS ||
		guidSchema == DBSCHEMA_COLLATIONS ||
		guidSchema == DBSCHEMA_COLUMN_DOMAIN_USAGE ||
		guidSchema == DBSCHEMA_COLUMN_PRIVILEGES ||
		guidSchema == DBSCHEMA_COLUMNS ||
		guidSchema == DBSCHEMA_CONSTRAINT_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_CONSTRAINT_TABLE_USAGE ||
		guidSchema == DBSCHEMA_FOREIGN_KEYS ||
		guidSchema == DBSCHEMA_INDEXES ||
		guidSchema == DBSCHEMA_KEY_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_PRIMARY_KEYS ||
		guidSchema == DBSCHEMA_PROCEDURE_COLUMNS ||
		guidSchema == DBSCHEMA_PROCEDURE_PARAMETERS ||
		guidSchema == DBSCHEMA_PROCEDURES ||
		guidSchema == DBSCHEMA_REFERENTIAL_CONSTRAINTS ||
		guidSchema == DBSCHEMA_SCHEMATA ||
		guidSchema == DBSCHEMA_STATISTICS ||
		guidSchema == DBSCHEMA_TABLE_CONSTRAINTS ||
		guidSchema == DBSCHEMA_TABLE_PRIVILEGES ||
		guidSchema == DBSCHEMA_TABLES ||
		guidSchema == DBSCHEMA_TABLES_INFO ||
		guidSchema == DBSCHEMA_TRANSLATIONS ||
		guidSchema == DBSCHEMA_USAGE_PRIVILEGES ||
		guidSchema == DBSCHEMA_VIEW_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_VIEW_TABLE_USAGE ||
		guidSchema == DBSCHEMA_VIEWS)
			return SECOND;

	// else return ZERO restrictions
	return ZERO;
}

ULONG CSchemaTest::SchemaTableRestriction(ULONG ulIndex)
{
	GUID guidSchema = m_rgSchemas[ulIndex];

	// If the GUID is one for which we know a catalog restriction exists then return FIRST.
	// Catalog restriction is always the first one.
	// This is most of them but we can't use negative logic 'cause we might get a guid we
	// don't recognize.
	if (guidSchema == DBSCHEMA_COLUMN_PRIVILEGES ||
		guidSchema == DBSCHEMA_COLUMNS ||
		guidSchema == DBSCHEMA_CONSTRAINT_COLUMN_USAGE ||
		guidSchema == DBSCHEMA_CONSTRAINT_TABLE_USAGE ||
		guidSchema == DBSCHEMA_FOREIGN_KEYS ||		// This is really PK_TABLE_NAME, but it will suffice
		guidSchema == DBSCHEMA_INDEXES ||			// This is really INDEX_NAME, but we always use table name
		guidSchema == DBSCHEMA_PRIMARY_KEYS ||
		guidSchema == DBSCHEMA_STATISTICS ||
		guidSchema == DBSCHEMA_TABLE_PRIVILEGES ||
		guidSchema == DBSCHEMA_TABLES ||
		guidSchema == DBSCHEMA_TABLES_INFO ||
		guidSchema == DBSCHEMA_VIEWS)
			return THIRD;
	else if	(guidSchema == DBSCHEMA_KEY_COLUMN_USAGE ||
			guidSchema == DBSCHEMA_TABLE_CONSTRAINTS)
		return SIXTH;

	// else return ZERO restrictions
	return ZERO;
}


void CSchemaTest::LimitRestrictions(ULONG ulIndex)
{

	// Don't pass restrictions for most schemas, but for those that have a catalog, schema, 
	// or table name limit to current catalog, schema, table.  This is done to keep the test
	// run time shorter. It took several hours on some providers without these restrictions.
	// Without a catalog restriction ALL catalogs (databases) should be returned.
	SetRestriction(SchemaCatalogRestriction(ulIndex));
	SetRestriction(SchemaSchemaRestriction(ulIndex));
	SetRestriction(SchemaTableRestriction(ulIndex));
}

void CSchemaTest::SetRowCount(ROW_COUNT eRowCount, ULONG ulRowCount)
{
	m_eRowCount = eRowCount;
	m_ulRowCount = ulRowCount;
}

HRESULT CSchemaTest::GetNextRows(DBROWOFFSET lOffset, DBROWCOUNT cRows, DBCOUNTITEM* pcRowsObtained, HROW** prghRows)
{
	ASSERT(prghRows);
	TBEGIN
	
	DBCOUNTITEM cRowsObtained = 0;
	DBCOUNTITEM i=0;
	
	//Record if we passed in consumer allocated array...
	HROW* rghRowsInput = *prghRows;

	//GetNextRows
	HRESULT hr = m_pIRowset->GetNextRows(NULL, lOffset, cRows, &cRowsObtained, prghRows);
	
	//Verify Correct values returned
	if(SUCCEEDED(hr))
	{
		if(hr == S_OK)
		{
			TESTC(cRowsObtained==(DBCOUNTITEM)ABS(cRows));
		}
		else
		{
			TESTC(cRowsObtained < (DBCOUNTITEM)ABS(cRows));
		}

		//Verify row array
		for(i=0; i<cRowsObtained; i++)
		{
			TESTC(*prghRows != NULL);
			TESTC((*prghRows)[i]!=DB_NULL_HROW)
		}
	}
	else
	{
		TESTC(cRowsObtained == 0);
	}

	//Verify output array, depending upon consumer or provider allocated...
	if(rghRowsInput)
	{
		//This is a users allocated static array,
		//This had better not be nulled out by the provider, if non-null on input
		TESTC(*prghRows == rghRowsInput);
	}
	else
	{
		TESTC(cRowsObtained ? *prghRows != NULL : *prghRows == NULL);
	}

CLEANUP:
	if(pcRowsObtained)
		*pcRowsObtained = cRowsObtained;
	return hr;
}


HRESULT CSchemaTest::GetColumnInfo(LPBYTE pData, DBBINDING * pBinding)
{
	IOpenRowset * pIOpenRowset = NULL;
	IColumnsInfo * pIColumnsInfo = NULL;
	DBID dbid = DB_NULLID;
	HRESULT hr = E_FAIL;
	ULONG iBind = 0, iFirst = 0;
	LPWSTR pwszCat = NULL;
	LPWSTR pwszSch = NULL;
	LPWSTR pwszTable = NULL;
	LPWSTR pwszQualifiedName = NULL;

	// pData buffer contains bindings to TABLE_CATALOG, TABLE_SCHEMA, and TABLE_NAME, in that 
	// order.
	
	// Skip any bookmark
	if (!pBinding[iBind].iOrdinal)
		iFirst++;

	for (iBind = 0; iBind < 3; iBind++)
	{
		// If value isn't bound then we assume NULL
		if (!VALUE_IS_BOUND(pBinding[iBind+iFirst]))
			continue;

		// Get a pointer to catalog, schema, and table name
		if (STATUS_IS_BOUND(pBinding[iBind+iFirst]))
		{
			// Check the status, which should always be OK or NULL
			if (STATUS_BINDING(pBinding[iBind+iFirst], pData) != DBSTATUS_S_OK)
			{
				COMPARE(STATUS_BINDING(pBinding[iBind+iFirst], pData), DBSTATUS_S_ISNULL);
				continue;
			}
		}

		// If status wasn't bound we assume OK, and length doesn't matter for null terminated string
		switch(pBinding[iBind+iFirst].iOrdinal)
		{
			case 1:
				pwszCat = (LPWSTR)&VALUE_BINDING(pBinding[iBind+iFirst], pData);
				break;
			case 2:
				pwszSch = (LPWSTR)&VALUE_BINDING(pBinding[iBind+iFirst], pData);
				break;
			case 3:
				pwszTable = (LPWSTR)&VALUE_BINDING(pBinding[iBind+iFirst], pData);
				break;
			default:
				COMPARE(pBinding[iBind+iFirst].iOrdinal < 4, TRUE);
				COMPARE(pBinding[iBind+iFirst].iOrdinal > 0, TRUE);
		}
	}

	// HACK FOR ORACLE
	if (pwszSch && wcslen(pwszSch) == 0)
		pwszSch = NULL;

	// Now we have either NULL or valid pointers to catalog, schema, table names
	TESTC_(m_pTable->GetQualifiedName(pwszCat, pwszSch, 
							pwszTable,&pwszQualifiedName, TRUE), S_OK);

	// If we already have information for this table then just don't bother
	if (m_pwszTableName && !wcscmp(pwszQualifiedName, m_pwszTableName))
	{
		m_ulTableOrdinal++;

		// If we couldn't actually retrieve the colinfo for this table then
		// return S_FALSE
		if (!m_prgColInfo)
			return S_FALSE;
		// Otherwise return success
		else
			return S_OK;
	}

	// Copy the table name into member var
	SAFE_FREE(m_pwszTableName);
	m_pwszTableName = pwszQualifiedName; 

	// Initialize out params
	m_ulTableOrdinal = 1;	// Note that schema rowset never reports bookmarks
	SAFE_FREE(m_prgColInfo);
	SAFE_FREE(m_pwszStringsBuffer);

	// Get IOpenRowset interface
	TESTC(VerifyInterface(m_pThisTestModule->m_pIUnknown2, IID_IOpenRowset, SESSION_INTERFACE, (IUnknown**)&pIOpenRowset));

	// Create DBID from table name
	dbid.eKind = DBKIND_NAME;
	dbid.uName.pwszName = m_pwszTableName;

	// Call OpenRowset
	hr = pIOpenRowset->OpenRowset(NULL, &dbid, NULL, IID_IColumnsInfo, 
		0, NULL, (IUnknown **)&pIColumnsInfo);

	if (FAILED(hr))
		odtLog << m_pwszTableName << L": Unable to open table.\n";

	// It's possible the table was deleted between the time we called GetRowset and attempting
	// to open the table, so we have to allow DB_E_NOTABLE, but post a warning.
	switch(hr)
	{
		case DB_E_NOTABLE:
		case DB_SEC_E_PERMISSIONDENIED:
			TESTW_(hr, S_OK);
			goto CLEANUP;
		default: 
			TESTC_(hr, S_OK);
	}

	// Make sure we've got a valid interface ptr
	TESTC(pIColumnsInfo != NULL);

	// Now call GetColumnInfo.  We already know cColumns so no need to return in member
	// var.
	TESTC_(hr = pIColumnsInfo->GetColumnInfo(&m_cColInfo, &m_prgColInfo, &m_pwszStringsBuffer), S_OK);

CLEANUP:

	SAFE_RELEASE(pIOpenRowset);
	SAFE_RELEASE(pIColumnsInfo);

	return hr;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Module Values
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// {{ TCW_MODULE_GLOBALS
DECLARE_MODULE_CLSID = { 0x4ace5741, 0x13bb, 0x11cf, { 0x89, 0x0f, 0x00, 0xaa, 0x00, 0xb5, 0xa9, 0x1b }};
DECLARE_MODULE_NAME("IDBSchemaRowset");
DECLARE_MODULE_OWNER("Microsoft");
DECLARE_MODULE_DESCRIP("test module for IDBSchemaRowset");
DECLARE_MODULE_VERSION(795921705);
// TCW_WizardVersion(2)
// TCW_Automation(True)
// }} TCW_MODULE_GLOBALS_END


//--------------------------------------------------------------------
// @func Initialises up indexes, Primary and Foreign keys on the primary table.
//	Should call TerminateKeysOnTable() upon failure of InitKeysOnTable or completion of test.
//
// Global values set.   g_fPrimaryKey  (TRUE if Primary key is available on the table).
//						g_fForeignKey  (TRUE if Foreign key is available on the table).
//						g_fIndexes	   (TRUE if indexes are available on the table.)
//						g_pExtraTable	Creates it.
//	
BOOL CSchemaTest::InitKeysOnTable()
{
	CCol	TempCol1, TempCol2;
	LONG	lrand1 =0;
	LONG	lrand2 =0;
	LONG	lrand3 =0;
	ULONG		i;
	BOOL	fSuccess = FALSE;


	// Create the second table.
	g_pKeyTable1 = new CTable( (IDBCreateCommand *)m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName, NONULLS);

	if (!g_pKeyTable1)
	{
		odtLog << wszMemoryAllocationError;
		return FALSE;
	}
	
	g_pKeyTable2 = new CTable( (IDBCreateCommand *)m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName, NONULLS);

	if (!g_pKeyTable2)
	{
		odtLog << wszMemoryAllocationError;
		// g_pKeyTable1 will be cleaned up in TerminateKeysOnTable();
		return FALSE;
	}

	if (FAILED(g_pKeyTable2->CreateTable(0)))
		goto CLEANUP;
	
	if (FAILED(g_pKeyTable1->CreateTable(0)))
		goto CLEANUP;
	
	// Now alter the tables to create Primary keys and foreign keys.
	for (i = 1; i <= g_pKeyTable1->CountColumnsOnTable(); i++)
	{
		if (FAILED(g_pKeyTable1->GetColInfo(i, TempCol1)))
			goto CLEANUP;

		// if it is  not a long column and is fixed length We found our candidate for Primary and Foreign Key.
		if (!TempCol1.GetIsLong() &&
			!TempCol1.GetNullable())		// Primary keys typically can't be nullable columns
		{
			fSuccess = TRUE;
			break;
		}
	
	}
	if (! fSuccess )
		goto CLEANUP;

	fSuccess = FALSE;
	for (i = 1; i <= g_pKeyTable2->CountColumnsOnTable(); i++)
	{
		if (FAILED(g_pKeyTable2->GetColInfo(i, TempCol2)))
			goto CLEANUP;

		// if it is  not a long column and is fixed length We found our candidate for Primary and Foreign Key.
		if (// (TempCol2.GetProviderType() == DBTYPE_STR) && 
			!(TempCol2.GetIsLong()) &&
			!TempCol2.GetNullable())		// Primary keys typically can't be nullable columns
		{
			fSuccess = TRUE;
			break;
		}
	
	}
	if (! fSuccess )
		goto CLEANUP;

	// Now alloc space for the  strings.
	g_pwszAddPrimaryKeyOnTable1 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszAddPrimaryKey)*sizeof (WCHAR)) + sizeof (WCHAR)	
													+	(wcslen(g_pKeyTable1->GetTableName())*sizeof (WCHAR))
													+	2 * (wcslen(TempCol1.GetColName())*sizeof (WCHAR))
													+	20	// Extra space for the KeyNumber (Random number).
													);
	if (!g_pwszAddPrimaryKeyOnTable1)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	g_pwszAddPrimaryKeyOnTable2 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszAddPrimaryKey)*sizeof (WCHAR)) + sizeof (WCHAR)	
													+	(wcslen(g_pKeyTable2->GetTableName())*sizeof (WCHAR))
													+	2 * (wcslen(TempCol2.GetColName())*sizeof (WCHAR))
													+	20	// Extra space for the KeyNumber (Random number).
													);
	if (!g_pwszAddPrimaryKeyOnTable2)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	
	g_pwszAddForeignKeyOnTable1 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszAddForeignKey)*sizeof (WCHAR)) + sizeof (WCHAR)	
													+	(wcslen(g_pKeyTable1->GetTableName())*sizeof (WCHAR))
													+	2 * (wcslen(TempCol1.GetColName())*sizeof (WCHAR))
													+	(wcslen(g_pKeyTable1->GetTableName())*sizeof (WCHAR))
													+	(wcslen(TempCol1.GetColName())*sizeof (WCHAR))
													+	20	// Extra space for the KeyNumber (Random number).
													);
	if (!g_pwszAddForeignKeyOnTable1)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	
	g_pwszDropPrimaryKeyConstraint1 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszDropPrimaryKeyConstraint)*sizeof (WCHAR)) + sizeof (WCHAR)	
														+	(wcslen(g_pKeyTable1->GetTableName())*sizeof (WCHAR))
														+	(wcslen(TempCol1.GetColName())*sizeof (WCHAR))
														+	20	// Extra space for the KeyNumber (Random number).
														);
	if (!g_pwszDropPrimaryKeyConstraint1)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	g_pwszDropPrimaryKeyConstraint2 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszDropPrimaryKeyConstraint)*sizeof (WCHAR)) + sizeof (WCHAR)	
														+	(wcslen(g_pKeyTable2->GetTableName())*sizeof (WCHAR))
														+	(wcslen(TempCol2.GetColName())*sizeof (WCHAR))
														+	20	// Extra space for the KeyNumber (Random number).
														);
	if (!g_pwszDropPrimaryKeyConstraint2)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	g_pwszDropForeignKeyConstraint1 = (WCHAR *)PROVIDER_ALLOC(	(wcslen(g_wszDropForeignKeyConstraint)*sizeof (WCHAR)) + sizeof (WCHAR)	
														+	(wcslen(g_pKeyTable1->GetTableName())*sizeof (WCHAR))
														+	(wcslen(TempCol1.GetColName())*sizeof (WCHAR))
														+	20	// Extra space for the KeyNumber (Random number).
														);
	if (!g_pwszDropForeignKeyConstraint1)
	{
		odtLog << wszMemoryAllocationError;
		goto CLEANUP;
	}

	// Add primary key on Table 1.
	swprintf(g_pwszAddPrimaryKeyOnTable1, g_wszAddPrimaryKey, 
		g_pKeyTable1->GetTableName(), (lrand1 = GetLongRandomNumber()), TempCol1.GetColName(), TempCol1.GetColName());

	// Add primary key on table 2.
	swprintf(g_pwszAddPrimaryKeyOnTable2, g_wszAddPrimaryKey, 
		g_pKeyTable2->GetTableName(), (lrand2 = GetLongRandomNumber()), TempCol2.GetColName(), TempCol2.GetColName());

	// Add foriegn key on table 1.  (from table 2).
	swprintf(g_pwszAddForeignKeyOnTable1, g_wszAddForeignKey, 
		g_pKeyTable1->GetTableName(), (lrand3 = GetLongRandomNumber()), TempCol1.GetColName(), TempCol1.GetColName(),
		g_pKeyTable2->GetTableName(), TempCol2.GetColName() );

	// Drop primary key on table 1.
	swprintf(g_pwszDropPrimaryKeyConstraint1, g_wszDropPrimaryKeyConstraint, 
		g_pKeyTable1->GetTableName(), lrand1, TempCol1.GetColName());
	
	// Drop primary key on table 2.
	swprintf(g_pwszDropPrimaryKeyConstraint2, g_wszDropPrimaryKeyConstraint, 
		g_pKeyTable2->GetTableName(), lrand2, TempCol2.GetColName());
	
	// Drop the foreign key on table 1.
	swprintf(g_pwszDropForeignKeyConstraint1, g_wszDropForeignKeyConstraint, 
		g_pKeyTable1->GetTableName(), lrand3, TempCol1.GetColName());

	if( !CHECK(g_pKeyTable1->BuildCommand(g_pwszAddPrimaryKeyOnTable1,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable1->m_pICommand), S_OK) )
		goto CLEANUP;
	
	// Set m_fKeyOnTable to Success so that we can drop them.(even if One constraint goes through).
	g_fKeysOnTable = TRUE;
	
	
	if( !SUCCEEDED(g_pKeyTable2->BuildCommand(g_pwszAddPrimaryKeyOnTable2,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable2->m_pICommand)) )
		goto CLEANUP;

	m_fPrimaryKey = TRUE; // There should be a minimum of one row in PRIMARY_KEYS rowset

	if( !SUCCEEDED(g_pKeyTable1->BuildCommand(g_pwszAddForeignKeyOnTable1,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable1->m_pICommand)) )
		goto CLEANUP;

	m_fForeignKey = TRUE; // There should be a minimum of one row in FOREIGN_KEYS rowset

	// Now we add rows.
	// First add rows in 2nd Table and then on 1st table.
	// Start with a table with 1 rows	
	if (FAILED(g_pKeyTable2->Insert(1)))
		goto CLEANUP;
	
	if (FAILED(g_pKeyTable1->Insert(1)))
		goto CLEANUP;
	
	fSuccess = TRUE;

CLEANUP:

	if (!fSuccess)
	{
		// CLEANUP memory incase of failure.
		PROVIDER_FREE(g_pwszAddPrimaryKeyOnTable1);
		PROVIDER_FREE(g_pwszAddPrimaryKeyOnTable2);
		PROVIDER_FREE(g_pwszAddForeignKeyOnTable1);
		PROVIDER_FREE(g_pwszDropPrimaryKeyConstraint1);
		PROVIDER_FREE(g_pwszDropPrimaryKeyConstraint2);
		PROVIDER_FREE(g_pwszDropForeignKeyConstraint1);
	}

	return fSuccess;

}


//--------------------------------------------------------------------
// @func Drops Key constraintes, indexes etc, created in InitKeysOnTable()
//	Drops the extra table after dropping the constraints.
BOOL CSchemaTest::TerminateKeysOnTable()
{
	// Drop the constraints.

	if (g_fKeysOnTable)
	{
		// We still need to flag an error if the Drop constraint fails.
		CHECK(g_pKeyTable1->BuildCommand(g_pwszDropPrimaryKeyConstraint1,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable1->m_pICommand), S_OK);
		
		CHECK(g_pKeyTable1->BuildCommand(g_pwszDropForeignKeyConstraint1,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable2->m_pICommand), S_OK);
		
		CHECK(g_pKeyTable2->BuildCommand(g_pwszDropPrimaryKeyConstraint2,		// SQL STMT
			IID_NULL, EXECUTE_ALWAYS, NULL, NULL, NULL,	NULL, NULL,	&g_pKeyTable1->m_pICommand), S_OK);
	}

	if (g_pKeyTable1)
	{
		// remove table from database
		(g_pKeyTable1)->DropTable();

		// delete CTable object
		delete g_pKeyTable1;

		g_pKeyTable1 = NULL;
	}
	
	if (g_pKeyTable2)
	{
		// remove table from database
		(g_pKeyTable2)->DropTable();

		// delete CTable object
		delete g_pKeyTable2;

		g_pKeyTable2 = NULL;
	}

	// CLEANUP memory incase of failure.
	PROVIDER_FREE(g_pwszAddPrimaryKeyOnTable1);
	PROVIDER_FREE(g_pwszAddPrimaryKeyOnTable2);
	PROVIDER_FREE(g_pwszAddForeignKeyOnTable1);
	PROVIDER_FREE(g_pwszDropPrimaryKeyConstraint1);
	PROVIDER_FREE(g_pwszDropPrimaryKeyConstraint2);
	PROVIDER_FREE(g_pwszDropForeignKeyConstraint1);

	return TRUE;
}

//--------------------------------------------------------------------
// @func Module level initialization routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
BOOL ModuleInit(CThisTestModule * pThisTestModule)
{
	HRESULT	hr;

	// Need to initialize these in ModuleInit to make sure they get reset if test is
	// run against two providers.
	g_fKeysOnTable=FALSE;
	g_pIDBSchemaRowset=NULL;
	cSchemas=0;
	rgSchemas=NULL;
	rgRestrictions=NULL;

	// Get connection and session objects
	if (ModuleCreateDBSession(pThisTestModule))
	{

		// Fail gracefully and quit module if we don't support IDBSchemaRowset
		if (FAILED(hr = pThisTestModule->m_pIUnknown2->QueryInterface(
			IID_IDBSchemaRowset,(void **)&g_pIDBSchemaRowset)))
		{
			// Make sure we returned E_NOINTERFACE if we've failed
			if (pThisTestModule->m_pError->Validate(hr,	
								LONGSTRING(__FILE__), __LINE__, E_NOINTERFACE))
				odtLog << L"IDBSchemaRowset is not supported.\n";

			return TEST_SKIPPED;
		}

		// Just Print out What is Supported
		if (FAILED(g_pIDBSchemaRowset->GetSchemas(&cSchemas, 
												  &rgSchemas, &rgRestrictions)))
		{
			g_pIDBSchemaRowset->Release();
			return FALSE;
		}
		
		for(ULONG index=0; index<cSchemas; index++)
		{
			TraceSchemaName(rgSchemas[index],TRUE,TRUE);
			TraceRestrictions(rgRestrictions,index,NULL,FALSE);
		}
		
		// Create a table we'll use for the whole test module,
		pThisTestModule->m_pVoid = new CTable(
			(IUnknown *)pThisTestModule->m_pIUnknown2,(LPWSTR)gwszModuleName);

		if (!pThisTestModule->m_pVoid)
		{
			odtLog << wszMemoryAllocationError;
			return FALSE;
		}

		// Start with a table with 1 rows								 
		if (!CHECK(((CTable *)pThisTestModule->m_pVoid)->CreateTable(2), S_OK))
			return FALSE;
	
		return TRUE;
	}
	
	return FALSE;
}	
  
//--------------------------------------------------------------------
// @func Module level termination routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
BOOL ModuleTerminate(CThisTestModule * pThisTestModule)
{
	// We still own the table since all of our testcases
	// have only used it and not deleted it.
	if (pThisTestModule->m_pVoid)
	{
		// Remove table from database
		((CTable *)pThisTestModule->m_pVoid)->DropTable();

		// Delete CTable object
		delete (CTable*)pThisTestModule->m_pVoid;
		pThisTestModule->m_pVoid = NULL;
	}

	SAFE_RELEASE(g_pIDBSchemaRowset);
	SAFE_FREE(rgSchemas);
	SAFE_FREE(rgRestrictions);

	return ModuleReleaseDBSession(pThisTestModule);
}	


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Case Section
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// {{ TCW_TEST_CASE_MAP(CGetRowset)
//--------------------------------------------------------------------
// @class test IDBSchemaRowset::GetRowset
//
class CGetRowset : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CGetRowset,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember S_OK: All supported schemas, no restrictions, no props
	int Variation_1();
	// @cmember E_INVALIDARG: rguidSchema = DBPROPSET_ROWSET
	int Variation_2();
	// @cmember E_INVALIDARG: rguidSchema = GUID_NULL
	int Variation_3();
	// @cmember DB_E_NOTSUPPORTED: provider does not support restrictions
	int Variation_4();
	// @cmember S_OK: pass all restrictions
	int Variation_5();
	// @cmember S_OK: less than max restrictions
	int Variation_6();
	// @cmember E_INVALIDARG: more than max restrictions
	int Variation_7();
	// @cmember E_NOINTERFACE: riid is not initialized
	int Variation_8();
	// @cmember S_OK: restrictions valid for 1st and 3rd restrictions
	int Variation_9();
	// @cmember S_OK: 2 cmd objects open on session
	int Variation_10();
	// @cmember DB_E_ERRORSOCCURED: update properties on read-only rowset
	int Variation_11();
	// @cmember DB_S_ERRORSOCCURED: DBPROP_IRowsetInfo & DBPROP_IRowsetChange
	int Variation_12();
	// @cmember S_OK: DBPROP_BOOKMARKS
	int Variation_13();
	// @cmember S_OK: empty variants = no restricions passed
	int Variation_14();
	// @cmember E_NOINTERFACE or S_OK: all rowset riids
	int Variation_15();
	// @cmember E_NOINTERFACE: riid = IID_IDBProperties
	int Variation_16();
	// @cmember E_INVALIDARG: ppRowset = NULL
	int Variation_17();
	// @cmember E_INVALIDARG: cRestrictions > 0, rgRestrictions = NULL
	int Variation_18();
	// @cmember E_INVALIDARG: cProperty != 0, rgProperties = NULL
	int Variation_19();
	// @cmember E_INVALIDARG: cPropertySets > 0, rgPropertySets = NULL
	int Variation_20();
	// @cmember E_INVALIDARG: invalid restriction with wrong VT type
	int Variation_21();
	// @cmember S_OK: request DBPROP_IColumnsRowset and, iid = IID_IColumnsRowset
	int Variation_22();
	// @cmember S_OK: riid = IID_IRowsetInfo, call IRowset::GetSpecification
	int Variation_23();
	// @cmember S_OK: riid = IID_IUnknown, get IRowset from IUnknown
	int Variation_24();
	// @cmember S_OK: open rowset from schema, try to open rowset on command
	int Variation_25();
	// @cmember S_OK: IRowsetScroll
	int Variation_26();
	// @cmember E_INVALIDARG: Schema not supported
	int Variation_27();
	// @cmember S_OK: Empty result set, pass first restriction that matches second restriction
	int Variation_28();
	// @cmember S_OK: Empty result set, pass non-matching value for each restriction
	int Variation_29();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CGetRowset)
#define THE_CLASS CGetRowset
BEG_TEST_CASE(CGetRowset, CSchemaTest, L"test IDBSchemaRowset::GetRowset")
	TEST_VARIATION(1, 		L"S_OK: All supported schemas, no restrictions, no props")
	TEST_VARIATION(2, 		L"E_INVALIDARG: rguidSchema = DBPROPSET_ROWSET")
	TEST_VARIATION(3, 		L"E_INVALIDARG: rguidSchema = GUID_NULL")
	TEST_VARIATION(4, 		L"DB_E_NOTSUPPORTED: provider does not support restrictions")
	TEST_VARIATION(5, 		L"S_OK: pass all restrictions")
	TEST_VARIATION(6, 		L"S_OK: less than max restrictions")
	TEST_VARIATION(7, 		L"E_INVALIDARG: more than max restrictions")
	TEST_VARIATION(8, 		L"E_NOINTERFACE: riid is not initialized")
	TEST_VARIATION(9, 		L"S_OK: restrictions valid for 1st and 3rd restrictions")
	TEST_VARIATION(10, 		L"S_OK: 2 cmd objects open on session")
	TEST_VARIATION(11, 		L"DB_E_ERRORSOCCURED: update properties on read-only rowset")
	TEST_VARIATION(12, 		L"DB_S_ERRORSOCCURED: DBPROP_IRowsetInfo & DBPROP_IRowsetChange")
	TEST_VARIATION(13, 		L"S_OK: DBPROP_BOOKMARKS")
	TEST_VARIATION(14, 		L"S_OK: empty variants = no restricions passed")
	TEST_VARIATION(15, 		L"E_NOINTERFACE or S_OK: all rowset riids")
	TEST_VARIATION(16, 		L"E_NOINTERFACE: riid = IID_IDBProperties")
	TEST_VARIATION(17, 		L"E_INVALIDARG: ppRowset = NULL")
	TEST_VARIATION(18, 		L"E_INVALIDARG: cRestrictions > 0, rgRestrictions = NULL")
	TEST_VARIATION(19, 		L"E_INVALIDARG: cProperty != 0, rgProperties = NULL")
	TEST_VARIATION(20, 		L"E_INVALIDARG: cPropertySets > 0, rgPropertySets = NULL")
	TEST_VARIATION(21, 		L"E_INVALIDARG: invalid restriction with wrong VT type")
	TEST_VARIATION(22, 		L"S_OK: request DBPROP_IColumnsRowset and, iid = IID_IColumnsRowset")
	TEST_VARIATION(23, 		L"S_OK: riid = IID_IRowsetInfo, call IRowset::GetSpecification")
	TEST_VARIATION(24, 		L"S_OK: riid = IID_IUnknown, get IRowset from IUnknown")
	TEST_VARIATION(25, 		L"S_OK: open rowset from schema, try to open rowset on command")
	TEST_VARIATION(26, 		L"S_OK: IRowsetScroll")
	TEST_VARIATION(27, 		L"E_INVALIDARG: Schema not supported")
	TEST_VARIATION(28, 		L"S_OK: Empty result set, pass first restriction that matches second restriction")
	TEST_VARIATION(29, 		L"S_OK: Empty result set, pass non-matching value for each restriction")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CGetSchema)
//--------------------------------------------------------------------
// @class test IDBSchemaRowset::GetSchemas
//
class CGetSchema : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CGetSchema,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember E_INVALIDARG: pcSchemas = NULL
	int Variation_1();
	// @cmember S_OK: *prgSchemas = NULL, pcSchemas = 0
	int Variation_2();
	// @cmember E_INVALIDARG: prgRestrictionSupported = NULL
	int Variation_3();
	// @cmember E_INVALIDARG: prgSchemas = NULL
	int Variation_4();
	// @cmember S_OK: open schema rowset, try to open rowset from command object
	int Variation_5();
	// @cmember S_OK: don't initialize variables before sending
	int Variation_6();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CGetSchema)
#define THE_CLASS CGetSchema
BEG_TEST_CASE(CGetSchema, CSchemaTest, L"test IDBSchemaRowset::GetSchemas")
	TEST_VARIATION(1, 		L"E_INVALIDARG: pcSchemas = NULL")
	TEST_VARIATION(2, 		L"S_OK: *prgSchemas = NULL, pcSchemas = 0")
	TEST_VARIATION(3, 		L"E_INVALIDARG: prgRestrictionSupported = NULL")
	TEST_VARIATION(4, 		L"E_INVALIDARG: prgSchemas = NULL")
	TEST_VARIATION(5, 		L"S_OK: open schema rowset, try to open rowset from command object")
	TEST_VARIATION(6, 		L"S_OK: don't initialize variables before sending")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CRowsetProperties)
//--------------------------------------------------------------------
// @class test rowset properties on IDBSchemaRowset
//
class CRowsetProperties : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CRowsetProperties,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember S_OK: all rowset properties as required
	int Variation_1();
	// @cmember S_OK: all rowset properties as setifcheap
	int Variation_2();
	// @cmember S_OK: all rowset properties, 2 at time, 1 setifcheap, 1 required
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CRowsetProperties)
#define THE_CLASS CRowsetProperties
BEG_TEST_CASE(CRowsetProperties, CSchemaTest, L"test rowset properties on IDBSchemaRowset")
	TEST_VARIATION(1, 		L"S_OK: all rowset properties as required")
	TEST_VARIATION(2, 		L"S_OK: all rowset properties as setifcheap")
	TEST_VARIATION(3, 		L"S_OK: all rowset properties, 2 at time, 1 setifcheap, 1 required")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CNonRowsetProperties)
//--------------------------------------------------------------------
// @class Make sure all non-rowset properties fail
//
class CNonRowsetProperties : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CNonRowsetProperties,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember DB_E_ERRORSOCCURRED: non-rowset property sets, all properties in that set
	int Variation_1();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CNonRowsetProperties)
#define THE_CLASS CNonRowsetProperties
BEG_TEST_CASE(CNonRowsetProperties, CSchemaTest, L"Make sure all non-rowset properties fail")
	TEST_VARIATION(1, 		L"DB_E_ERRORSOCCURRED: non-rowset property sets, all properties in that set")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CAssertions)
//--------------------------------------------------------------------
// @class test restrictions on assertion schema
//
class CAssertions : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CAssertions,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ConstraintCatalog
	int Variation_1();
	// @cmember ConstraintSchema
	int Variation_2();
	// @cmember ConstraintName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CAssertions)
#define THE_CLASS CAssertions
BEG_TEST_CASE(CAssertions, CSchemaTest, L"test restrictions on assertion schema")
	TEST_VARIATION(1, 		L"ConstraintCatalog")
	TEST_VARIATION(2, 		L"ConstraintSchema")
	TEST_VARIATION(3, 		L"ConstraintName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CCatalogs)
//--------------------------------------------------------------------
// @class test restrictions on catalog schema
//
class CCatalogs : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CCatalogs,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember CatalogName
	int Variation_1();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CCatalogs)
#define THE_CLASS CCatalogs
BEG_TEST_CASE(CCatalogs, CSchemaTest, L"test restrictions on catalog schema")
	TEST_VARIATION(1, 		L"CatalogName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CCharacterSets)
//--------------------------------------------------------------------
// @class test restrictions on character set schema
//
class CCharacterSets : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CCharacterSets,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember CharacterSetCatalog
	int Variation_1();
	// @cmember CharacterSetSchema
	int Variation_2();
	// @cmember CharacterSetName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CCharacterSets)
#define THE_CLASS CCharacterSets
BEG_TEST_CASE(CCharacterSets, CSchemaTest, L"test restrictions on character set schema")
	TEST_VARIATION(1, 		L"CharacterSetCatalog")
	TEST_VARIATION(2, 		L"CharacterSetSchema")
	TEST_VARIATION(3, 		L"CharacterSetName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CCheckConstraints)
//--------------------------------------------------------------------
// @class test restrictions on check constraint schema
//
class CCheckConstraints : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CCheckConstraints,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ConstraintCatalog
	int Variation_1();
	// @cmember ConstraintSchema
	int Variation_2();
	// @cmember ConstraintName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CCheckConstraints)
#define THE_CLASS CCheckConstraints
BEG_TEST_CASE(CCheckConstraints, CSchemaTest, L"test restrictions on check constraint schema")
	TEST_VARIATION(1, 		L"ConstraintCatalog")
	TEST_VARIATION(2, 		L"ConstraintSchema")
	TEST_VARIATION(3, 		L"ConstraintName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CCollations)
//--------------------------------------------------------------------
// @class test restrictions on collations schema
//
class CCollations : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CCollations,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember CollationCatalog
	int Variation_1();
	// @cmember CollationSchema
	int Variation_2();
	// @cmember CollationName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CCollations)
#define THE_CLASS CCollations
BEG_TEST_CASE(CCollations, CSchemaTest, L"test restrictions on collations schema")
	TEST_VARIATION(1, 		L"CollationCatalog")
	TEST_VARIATION(2, 		L"CollationSchema")
	TEST_VARIATION(3, 		L"CollationName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CColumnDomainUsage)
//--------------------------------------------------------------------
// @class test restrictions on column domain schema
//
class CColumnDomainUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CColumnDomainUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember DomainCatalog
	int Variation_1();
	// @cmember DomainSchema
	int Variation_2();
	// @cmember DominName
	int Variation_3();
	// @cmember ColumnName
	int Variation_4();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CColumnDomainUsage)
#define THE_CLASS CColumnDomainUsage
BEG_TEST_CASE(CColumnDomainUsage, CSchemaTest, L"test restrictions on column domain schema")
	TEST_VARIATION(1, 		L"DomainCatalog")
	TEST_VARIATION(2, 		L"DomainSchema")
	TEST_VARIATION(3, 		L"DominName")
	TEST_VARIATION(4, 		L"ColumnName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CColumnPrivileges)
//--------------------------------------------------------------------
// @class test restrictions on column privileges schema
//
class CColumnPrivileges : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CColumnPrivileges,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember ColumnName
	int Variation_4();
	// @cmember Grantor
	int Variation_5();
	// @cmember Grantee
	int Variation_6();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CColumnPrivileges)
#define THE_CLASS CColumnPrivileges
BEG_TEST_CASE(CColumnPrivileges, CSchemaTest, L"test restrictions on column privileges schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"ColumnName")
	TEST_VARIATION(5, 		L"Grantor")
	TEST_VARIATION(6, 		L"Grantee")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CColumns)
//--------------------------------------------------------------------
// @class test restrictions on column schema
//
class CColumns : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CColumns,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember ColumnName
	int Variation_4();
	// @cmember All Supported Restrictions
	int Variation_5();
	// @cmember All Restrictions
	int Variation_6();
	// @cmember Extra Restriction
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CColumns)
#define THE_CLASS CColumns
BEG_TEST_CASE(CColumns, CSchemaTest, L"test restrictions on column schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"ColumnName")
	TEST_VARIATION(5, 		L"All Supported Restrictions")
	TEST_VARIATION(6, 		L"All Restrictions")
	TEST_VARIATION(7, 		L"Extra Restriction")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CConstraintColumnUsage)
//--------------------------------------------------------------------
// @class test restrictions on constrain column usage
//
class CConstraintColumnUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CConstraintColumnUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember ColumnName
	int Variation_4();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CConstraintColumnUsage)
#define THE_CLASS CConstraintColumnUsage
BEG_TEST_CASE(CConstraintColumnUsage, CSchemaTest, L"test restrictions on constrain column usage")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"ColumnName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CConstraintTableUsage)
//--------------------------------------------------------------------
// @class test restrictions on constraint table usage
//
class CConstraintTableUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CConstraintTableUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CConstraintTableUsage)
#define THE_CLASS CConstraintTableUsage
BEG_TEST_CASE(CConstraintTableUsage, CSchemaTest, L"test restrictions on constraint table usage")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CForeignKeys)
//--------------------------------------------------------------------
// @class test restrictions on foreign key schema
//
class CForeignKeys : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CForeignKeys,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember PKTableName and FKTableName restrictions.
	int Variation_1();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CForeignKeys)
#define THE_CLASS CForeignKeys
BEG_TEST_CASE(CForeignKeys, CSchemaTest, L"test restrictions on foreign key schema")
	TEST_VARIATION(1, 		L"PKTableName and FKTableName restrictions.")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CIndexes)
//--------------------------------------------------------------------
// @class test restrictions on index schema
//
class CIndexes : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CIndexes,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember IndexName
	int Variation_3();
	// @cmember Type
	int Variation_4();
	// @cmember TableName
	int Variation_5();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CIndexes)
#define THE_CLASS CIndexes
BEG_TEST_CASE(CIndexes, CSchemaTest, L"test restrictions on index schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"IndexName")
	TEST_VARIATION(4, 		L"Type")
	TEST_VARIATION(5, 		L"TableName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CKeyColumnUsage)
//--------------------------------------------------------------------
// @class test restrictions on key column usage
//
class CKeyColumnUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CKeyColumnUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ConstraintCatalog
	int Variation_1();
	// @cmember ConstraintSchema
	int Variation_2();
	// @cmember ConstraintName
	int Variation_3();
	// @cmember TableCatalog
	int Variation_4();
	// @cmember TableSchema
	int Variation_5();
	// @cmember TableName
	int Variation_6();
	// @cmember ColumnName
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CKeyColumnUsage)
#define THE_CLASS CKeyColumnUsage
BEG_TEST_CASE(CKeyColumnUsage, CSchemaTest, L"test restrictions on key column usage")
	TEST_VARIATION(1, 		L"ConstraintCatalog")
	TEST_VARIATION(2, 		L"ConstraintSchema")
	TEST_VARIATION(3, 		L"ConstraintName")
	TEST_VARIATION(4, 		L"TableCatalog")
	TEST_VARIATION(5, 		L"TableSchema")
	TEST_VARIATION(6, 		L"TableName")
	TEST_VARIATION(7, 		L"ColumnName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CPrimaryKeys)
//--------------------------------------------------------------------
// @class test restrictions on primary key schema
//
class CPrimaryKeys : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CPrimaryKeys,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CPrimaryKeys)
#define THE_CLASS CPrimaryKeys
BEG_TEST_CASE(CPrimaryKeys, CSchemaTest, L"test restrictions on primary key schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CProcedureColumns)
//--------------------------------------------------------------------
// @class test restrictions on procedure column schema
//
class CProcedureColumns : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CProcedureColumns,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ProcedureCatalog
	int Variation_1();
	// @cmember ProcedureSchema
	int Variation_2();
	// @cmember ProcedureName
	int Variation_3();
	// @cmember ColumnName
	int Variation_4();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CProcedureColumns)
#define THE_CLASS CProcedureColumns
BEG_TEST_CASE(CProcedureColumns, CSchemaTest, L"test restrictions on procedure column schema")
	TEST_VARIATION(1, 		L"ProcedureCatalog")
	TEST_VARIATION(2, 		L"ProcedureSchema")
	TEST_VARIATION(3, 		L"ProcedureName")
	TEST_VARIATION(4, 		L"ColumnName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CProcedureParameters)
//--------------------------------------------------------------------
// @class test restrictions on procedure parameters
//
class CProcedureParameters : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CProcedureParameters,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ProcedureCatalog
	int Variation_1();
	// @cmember ProcedureSchema
	int Variation_2();
	// @cmember ProcedureName
	int Variation_3();
	// @cmember ParameterName - Input
	int Variation_4();
	// @cmember Parameter Name - Input/Output
	int Variation_5();
	// @cmember Parameter Name - RETURN_VALUE
	int Variation_6();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CProcedureParameters)
#define THE_CLASS CProcedureParameters
BEG_TEST_CASE(CProcedureParameters, CSchemaTest, L"test restrictions on procedure parameters")
	TEST_VARIATION(1, 		L"ProcedureCatalog")
	TEST_VARIATION(2, 		L"ProcedureSchema")
	TEST_VARIATION(3, 		L"ProcedureName")
	TEST_VARIATION(4, 		L"ParameterName - Input")
	TEST_VARIATION(5, 		L"Parameter Name - Input/Output")
	TEST_VARIATION(6, 		L"Parameter Name - RETURN_VALUE")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CProcedures)
//--------------------------------------------------------------------
// @class test restrictions on procedures schema
//
class CProcedures : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CProcedures,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ProcedureCatalog
	int Variation_1();
	// @cmember ProcedureSchema
	int Variation_2();
	// @cmember ProcedureName
	int Variation_3();
	// @cmember ProcedureType
	int Variation_4();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CProcedures)
#define THE_CLASS CProcedures
BEG_TEST_CASE(CProcedures, CSchemaTest, L"test restrictions on procedures schema")
	TEST_VARIATION(1, 		L"ProcedureCatalog")
	TEST_VARIATION(2, 		L"ProcedureSchema")
	TEST_VARIATION(3, 		L"ProcedureName")
	TEST_VARIATION(4, 		L"ProcedureType")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CProviderTypes)
//--------------------------------------------------------------------
// @class test restrictions on provider types
//
class CProviderTypes : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CProviderTypes,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember DataType
	int Variation_1();
	// @cmember BestMatch
	int Variation_2();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CProviderTypes)
#define THE_CLASS CProviderTypes
BEG_TEST_CASE(CProviderTypes, CSchemaTest, L"test restrictions on provider types")
	TEST_VARIATION(1, 		L"DataType")
	TEST_VARIATION(2, 		L"BestMatch")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CReferentialContraints)
//--------------------------------------------------------------------
// @class test restrictions on referential constraint schema
//
class CReferentialContraints : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CReferentialContraints,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ConstraintCatalog
	int Variation_1();
	// @cmember ConstraintSchema
	int Variation_2();
	// @cmember ConstraintName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CReferentialContraints)
#define THE_CLASS CReferentialContraints
BEG_TEST_CASE(CReferentialContraints, CSchemaTest, L"test restrictions on referential constraint schema")
	TEST_VARIATION(1, 		L"ConstraintCatalog")
	TEST_VARIATION(2, 		L"ConstraintSchema")
	TEST_VARIATION(3, 		L"ConstraintName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CSchemata)
//--------------------------------------------------------------------
// @class test restrictions on schemata schema
//
class CSchemata : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CSchemata,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember CatalogName
	int Variation_1();
	// @cmember SchemaName
	int Variation_2();
	// @cmember SchemaOwner
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CSchemata)
#define THE_CLASS CSchemata
BEG_TEST_CASE(CSchemata, CSchemaTest, L"test restrictions on schemata schema")
	TEST_VARIATION(1, 		L"CatalogName")
	TEST_VARIATION(2, 		L"SchemaName")
	TEST_VARIATION(3, 		L"SchemaOwner")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CSqlLanguages)
//--------------------------------------------------------------------
// @class test restrictions on sql languages schema
//
class CSqlLanguages : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CSqlLanguages,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CSqlLanguages)
#define THE_CLASS CSqlLanguages
BEG_TEST_CASE(CSqlLanguages, CSchemaTest, L"test restrictions on sql languages schema")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CStatistics)
//--------------------------------------------------------------------
// @class test restrictions on statistics schema
//
class CStatistics : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CStatistics,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CStatistics)
#define THE_CLASS CStatistics
BEG_TEST_CASE(CStatistics, CSchemaTest, L"test restrictions on statistics schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CTableConstraints)
//--------------------------------------------------------------------
// @class test restrictions on table contraint schema
//
class CTableConstraints : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CTableConstraints,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ConstraintCatalog
	int Variation_1();
	// @cmember ConstraintSchema
	int Variation_2();
	// @cmember ConstraintName
	int Variation_3();
	// @cmember TableCatalog
	int Variation_4();
	// @cmember TableSchema
	int Variation_5();
	// @cmember TableName
	int Variation_6();
	// @cmember ConstraintType
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CTableConstraints)
#define THE_CLASS CTableConstraints
BEG_TEST_CASE(CTableConstraints, CSchemaTest, L"test restrictions on table contraint schema")
	TEST_VARIATION(1, 		L"ConstraintCatalog")
	TEST_VARIATION(2, 		L"ConstraintSchema")
	TEST_VARIATION(3, 		L"ConstraintName")
	TEST_VARIATION(4, 		L"TableCatalog")
	TEST_VARIATION(5, 		L"TableSchema")
	TEST_VARIATION(6, 		L"TableName")
	TEST_VARIATION(7, 		L"ConstraintType")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CTablePrivileges)
//--------------------------------------------------------------------
// @class test restrictions on table privileges
//
class CTablePrivileges : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CTablePrivileges,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember Grantor
	int Variation_4();
	// @cmember Grantee
	int Variation_5();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CTablePrivileges)
#define THE_CLASS CTablePrivileges
BEG_TEST_CASE(CTablePrivileges, CSchemaTest, L"test restrictions on table privileges")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"Grantor")
	TEST_VARIATION(5, 		L"Grantee")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CTables)
//--------------------------------------------------------------------
// @class test restricitons on table schema
//
class CTables : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CTables,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember TableType
	int Variation_4();
	// @cmember All Supported Restrictions
	int Variation_5();
	// @cmember All Restrictions
	int Variation_6();
	// @cmember Extra Restriction
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CTables)
#define THE_CLASS CTables
BEG_TEST_CASE(CTables, CSchemaTest, L"test restricitons on table schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"TableType")
	TEST_VARIATION(5, 		L"All Supported Restrictions")
	TEST_VARIATION(6, 		L"All Restrictions")
	TEST_VARIATION(7, 		L"Extra Restriction")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CTablesInfo)
//--------------------------------------------------------------------
// @class test restricitons on table schema
//
class CTablesInfo : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CTablesInfo,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// @cmember TableType
	int Variation_4();
	// @cmember All Supported Restrictions
	int Variation_5();
	// @cmember All Restrictions
	int Variation_6();
	// @cmember Extra Restriction
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CTablesInfo)
#define THE_CLASS CTablesInfo
BEG_TEST_CASE(CTablesInfo, CSchemaTest, L"test restricitons on table schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
	TEST_VARIATION(4, 		L"TableType")
	TEST_VARIATION(5, 		L"All Supported Restrictions")
	TEST_VARIATION(6, 		L"All Restrictions")
	TEST_VARIATION(7, 		L"Extra Restriction")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CTranslations)
//--------------------------------------------------------------------
// @class test restrictions on translation schema
//
class CTranslations : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CTranslations,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TranslationCatalog
	int Variation_1();
	// @cmember TranslationSchema
	int Variation_2();
	// @cmember TranslationName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CTranslations)
#define THE_CLASS CTranslations
BEG_TEST_CASE(CTranslations, CSchemaTest, L"test restrictions on translation schema")
	TEST_VARIATION(1, 		L"TranslationCatalog")
	TEST_VARIATION(2, 		L"TranslationSchema")
	TEST_VARIATION(3, 		L"TranslationName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CUsagePrivileges)
//--------------------------------------------------------------------
// @class test restrictions on usage privileges schema
//
class CUsagePrivileges : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CUsagePrivileges,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ObjectCatalog
	int Variation_1();
	// @cmember ObjectSchema
	int Variation_2();
	// @cmember ObjectName
	int Variation_3();
	// @cmember ObjectType
	int Variation_4();
	// @cmember Grantor
	int Variation_5();
	// @cmember Grantee
	int Variation_6();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CUsagePrivileges)
#define THE_CLASS CUsagePrivileges
BEG_TEST_CASE(CUsagePrivileges, CSchemaTest, L"test restrictions on usage privileges schema")
	TEST_VARIATION(1, 		L"ObjectCatalog")
	TEST_VARIATION(2, 		L"ObjectSchema")
	TEST_VARIATION(3, 		L"ObjectName")
	TEST_VARIATION(4, 		L"ObjectType")
	TEST_VARIATION(5, 		L"Grantor")
	TEST_VARIATION(6, 		L"Grantee")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CViewColumnUsage)
//--------------------------------------------------------------------
// @class test restrictions on view column usage schema
//
class CViewColumnUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CViewColumnUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ViewCatalog
	int Variation_1();
	// @cmember ViewSchema
	int Variation_2();
	// @cmember ViewName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CViewColumnUsage)
#define THE_CLASS CViewColumnUsage
BEG_TEST_CASE(CViewColumnUsage, CSchemaTest, L"test restrictions on view column usage schema")
	TEST_VARIATION(1, 		L"ViewCatalog")
	TEST_VARIATION(2, 		L"ViewSchema")
	TEST_VARIATION(3, 		L"ViewName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CViewTableUsage)
//--------------------------------------------------------------------
// @class test restrictions on view table usage
//
class CViewTableUsage : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CViewTableUsage,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember ViewCatalog
	int Variation_1();
	// @cmember ViewSchema
	int Variation_2();
	// @cmember ViewName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CViewTableUsage)
#define THE_CLASS CViewTableUsage
BEG_TEST_CASE(CViewTableUsage, CSchemaTest, L"test restrictions on view table usage")
	TEST_VARIATION(1, 		L"ViewCatalog")
	TEST_VARIATION(2, 		L"ViewSchema")
	TEST_VARIATION(3, 		L"ViewName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CViews)
//--------------------------------------------------------------------
// @class test restrictions on view schema
//
class CViews : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CViews,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember TableCatalog
	int Variation_1();
	// @cmember TableSchema
	int Variation_2();
	// @cmember TableName
	int Variation_3();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CViews)
#define THE_CLASS CViews
BEG_TEST_CASE(CViews, CSchemaTest, L"test restrictions on view schema")
	TEST_VARIATION(1, 		L"TableCatalog")
	TEST_VARIATION(2, 		L"TableSchema")
	TEST_VARIATION(3, 		L"TableName")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(CZombie)
//--------------------------------------------------------------------
// @class testing IDBSchemaRowset in zombie situation
//
class CZombie : public CTransaction { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CZombie,CTransaction);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	int TestTxnGetRowset(ETXN eTxn,BOOL fRetaining,	GUID schema);
	int TestTxnGetSchemas(ETXN eTxn,BOOL fRetaining);

	
	// {{ TCW_TESTVARS()
	// @cmember GetRowset: Commit with fRetaining set to TRUE
	int Variation_1();
	// @cmember GetRowset: Commit with fRetaining set to FALSE
	int Variation_2();
	// @cmember GetRowset: Abort with fRetaining set to TRUE
	int Variation_3();
	// @cmember GetRowset: Abort with fRetaining set to FALSE
	int Variation_4();
	// @cmember GetSchemas: Commit with fRetaining set to TRUE
	int Variation_5();
	// @cmember GetSchemas: Commit with fRetaining set to FALSE
	int Variation_6();
	// @cmember GetSchemas: Abort with fRetaining set to TRUE
	int Variation_7();
	// @cmember GetSchemas: Abort with fRetaining set to FALSE
	int Variation_8();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(CZombie)
#define THE_CLASS CZombie
BEG_TEST_CASE(CZombie, CTransaction, L"testing IDBSchemaRowset in zombie situation")
	TEST_VARIATION(1, 		L"GetRowset: Commit with fRetaining set to TRUE")
	TEST_VARIATION(2, 		L"GetRowset: Commit with fRetaining set to FALSE")
	TEST_VARIATION(3, 		L"GetRowset: Abort with fRetaining set to TRUE")
	TEST_VARIATION(4, 		L"GetRowset: Abort with fRetaining set to FALSE")
	TEST_VARIATION(5, 		L"GetSchemas: Commit with fRetaining set to TRUE")
	TEST_VARIATION(6, 		L"GetSchemas: Commit with fRetaining set to FALSE")
	TEST_VARIATION(7, 		L"GetSchemas: Abort with fRetaining set to TRUE")
	TEST_VARIATION(8, 		L"GetSchemas: Abort with fRetaining set to FALSE")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(ExtendedErrors)
//--------------------------------------------------------------------
// @class testing extended errors on IDBSchemaRowset
//
class ExtendedErrors : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(ExtendedErrors,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember Valid GetSchemas Call with previous error object existing
	int Variation_1();
	// @cmember Valid GetRowset call with previous error object existing
	int Variation_2();
	// @cmember Valid GetSchemas call with previous error object existing
	int Variation_3();
	// @cmember Invalid GetRowset call with previous error object existing
	int Variation_4();
	// @cmember Invalid GetSchemas call with previous error object existing
	int Variation_5();
	// @cmember Invalid GetRowset call with previous error object existing
	int Variation_6();
	// @cmember Open schema rowset  DBSCHEMA_ASSERTIONS -- E_INVALIDARG
	int Variation_7();
	// }} TCW_TESTVARS_END
};
// {{ TCW_TESTCASE(ExtendedErrors)
#define THE_CLASS ExtendedErrors
BEG_TEST_CASE(ExtendedErrors, CSchemaTest, L"testing extended errors on IDBSchemaRowset")
	TEST_VARIATION(1, 		L"Valid GetSchemas Call with previous error object existing")
	TEST_VARIATION(2, 		L"Valid GetRowset call with previous error object existing")
	TEST_VARIATION(3, 		L"Valid GetSchemas call with previous error object existing")
	TEST_VARIATION(4, 		L"Invalid GetRowset call with previous error object existing")
	TEST_VARIATION(5, 		L"Invalid GetSchemas call with previous error object existing")
	TEST_VARIATION(6, 		L"Invalid GetRowset call with previous error object existing")
	TEST_VARIATION(7, 		L"Open schema rowset  DBSCHEMA_ASSERTIONS -- E_INVALIDARG")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(CBugRegressions)
//*-----------------------------------------------------------------------
// @class Test case for bug regressions
//
class CBugRegressions : public CSchemaTest { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(CBugRegressions,CSchemaTest);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember GetRowset on schema TABLES after executing RETURN statement
	int Variation_1();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(CBugRegressions)
#define THE_CLASS CBugRegressions
BEG_TEST_CASE(CBugRegressions, CSchemaTest, L"Test case for bug regressions")
	TEST_VARIATION(1, 		L"GetRowset on schema TABLES after executing RETURN statement")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// }} END_DECLARE_TEST_CASES()

// {{ TCW_TESTMODULE(ThisModule)
TEST_MODULE(38, ThisModule, gwszModuleDescrip)
	TEST_CASE(1, CGetRowset)
	TEST_CASE(2, CGetSchema)
	TEST_CASE(3, CRowsetProperties)
	TEST_CASE(4, CNonRowsetProperties)
	TEST_CASE(5, CAssertions)
	TEST_CASE(6, CCatalogs)
	TEST_CASE(7, CCharacterSets)
	TEST_CASE(8, CCheckConstraints)
	TEST_CASE(9, CCollations)
	TEST_CASE(10, CColumnDomainUsage)
	TEST_CASE(11, CColumnPrivileges)
	TEST_CASE(12, CColumns)
	TEST_CASE(13, CConstraintColumnUsage)
	TEST_CASE(14, CConstraintTableUsage)
	TEST_CASE(15, CForeignKeys)
	TEST_CASE(16, CIndexes)
	TEST_CASE(17, CKeyColumnUsage)
	TEST_CASE(18, CPrimaryKeys)
	TEST_CASE(19, CProcedureColumns)
	TEST_CASE(20, CProcedureParameters)
	TEST_CASE(21, CProcedures)
	TEST_CASE(22, CProviderTypes)
	TEST_CASE(23, CReferentialContraints)
	TEST_CASE(24, CSchemata)
	TEST_CASE(25, CSqlLanguages)
	TEST_CASE(26, CStatistics)
	TEST_CASE(27, CTableConstraints)
	TEST_CASE(28, CTablePrivileges)
	TEST_CASE(29, CTables)
	TEST_CASE(30, CTablesInfo)
	TEST_CASE(31, CTranslations)
	TEST_CASE(32, CUsagePrivileges)
	TEST_CASE(33, CViewColumnUsage)
	TEST_CASE(34, CViewTableUsage)
	TEST_CASE(35, CViews)
	TEST_CASE(36, CZombie)
	TEST_CASE(37, ExtendedErrors)
	TEST_CASE(38, CBugRegressions)
END_TEST_MODULE()
// }} TCW_TESTMODULE_END


// {{ TCW_TC_PROTOTYPE(CGetRowset)
//*-----------------------------------------------------------------------
//| Test Case:		CGetRowset - test IDBSchemaRowset::GetRowset
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CGetRowset::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc S_OK: All supported schemas, no restrictions, no props
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_1()
{
	// for each valid schema, test method
	for(ULONG ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT;

		// Don't need Restrictions and Properties
		m_fCountPropSetNULL		 = TRUE;
		m_fRangePropSetNULL		 = TRUE;
		m_fCountRestrictionsNULL = TRUE;
		m_fRangeRestrictionsNULL = TRUE;

		m_restrict  = ZERO;
		m_iid		= IID_IRowset;

		// Check to make sure it's a schema I want and get its info
		if(GetSchemaInfo(SUPPORTED, ulIndex))
		{

			// Test method
			if(!CHECK(GetRowset(), S_OK))
				m_cErrors++;
		}
		
		FREE;
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: rguidSchema = DBPROPSET_ROWSET
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_2()
{
	INIT;

	// Don't need Restrictions and Properties
	m_fCountPropSetNULL		 = TRUE;
	m_fRangePropSetNULL		 = TRUE;
	m_fCountRestrictionsNULL = TRUE;
	m_fRangeRestrictionsNULL = TRUE;

	m_guid  = DBPROPSET_ROWSET;
	m_iid	= IID_IRowset;

	// Test method with invalid schema guid
	if(CHECK(GetRowset(), E_INVALIDARG))
		m_fResult = TEST_PASS;

	FREE;
	
	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: rguidSchema = GUID_NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_3()
{
	INIT;

	// Don't need Restrictions and Properties
	m_fCountPropSetNULL		 = TRUE;
	m_fRangePropSetNULL		 = TRUE;
	m_fCountRestrictionsNULL = TRUE;
	m_fRangeRestrictionsNULL = TRUE;

	m_iid = IID_IRowset;

	// Test method when schema guid == GUID_NULL
	if(CHECK(GetRowset(), E_INVALIDARG))
		m_fResult = TEST_PASS;

	FREE;

	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc DB_E_NOTSUPPORTED: provider does not support restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_4()
{
	// for each valid schema, test method
	for(ULONG ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
  		m_fPassUnsupportedRestrictions=TRUE;

		m_iid = IID_IRowset;
		m_restrict = ALLRES;

		// don't need properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// If provider supports X of X restrictions
			// no need to test this return code,
			// however, if provider supports less than
			// all restrictions for a schema, check
			// against this return code
			if(m_fAtLeast1UnsupportedRestrictionIsSet)
			{
				GetRowset();

				// test method
				// Both E_INVALIDARG and DB_E_NOTSUPPORTED are valid
				if((m_HR != DB_E_NOTSUPPORTED) && (m_HR != E_INVALIDARG))
					m_cErrors++;
			}
		}
		
		FREE;
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc S_OK: pass all restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_5()
{
	ULONG cErrors = 0;

	// for each schema guid, test method
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
 		INIT
		m_iid = IID_IRowset;
		SetRestriction(ALLRES);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check if this is a schema I want then
		// get it's info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// Override the default rows expected for COLUMNS and COLUMNPRIVILEGES
			// rowsets because they restrict down to an individual column, 1 row min.
			if (m_guid == DBSCHEMA_COLUMNS ||
				m_guid == DBSCHEMA_COLUMN_PRIVILEGES ||
				m_guid == DBSCHEMA_CONSTRAINT_COLUMN_USAGE)
				SetRowCount(MIN_REQUIRED, 1);

			// test method
			if(!CHECK(GetRowset(), S_OK))
				cErrors++;

			// Check for error in rowset verification
			if (!m_fResult)
				cErrors++;
		}
		FREE
	}

	if(cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc S_OK: less than max restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_6()
{
	// for each schema guid, test method
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		// Array of restrictions will have all restrictions, if possible
		SetRestriction(ALLRES);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// make sure this is a schema I'm interested in then
		// get it's information
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			if(m_cRestrictions>1)
			{

				// don't really have an meaningful test
				// for rowset returned so make sure rowset
				// isn't examined.
				m_restrict = ZERO;

				// just want to change the number passed in
				m_cRestrictions --;
				
				// Override the default rows expected for COLUMNS and COLUMNPRIVILEGES
				// rowsets because they restrict down to an individual column, 1 row min.
				if (m_guid == DBSCHEMA_COLUMNS ||
					m_guid == DBSCHEMA_COLUMN_PRIVILEGES ||
					m_guid == DBSCHEMA_CONSTRAINT_COLUMN_USAGE)
					SetRowCount(MIN_REQUIRED, 1);

				// test method
				if(!CHECK(GetRowset(), S_OK))
					m_cErrors++;

			}
		}
		FREE
	}

	if (m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: more than max restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_7()
{
	ULONG m_cErrors=0;

	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
	 	SetRestriction(ALLRES);
 		m_iid = IID_IRowset;

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// TODO: remove if statement when their restrictions are supported
			if(m_guid!=DBSCHEMA_CATALOGS &&
				m_guid!=DBSCHEMA_SCHEMATA)
			{
				m_cRestrictions ++;

				// test method
				if(!CHECK(GetRowset(), E_INVALIDARG))
					m_cErrors++;
			}
		}
		FREE
	}


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc E_NOINTERFACE: riid is not initialized
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_8()
{
	// Intentionally initialized to a value that should be a bogus iid.  This was previously uninitialized
	// but then sometimes would happen to equal IID_NULL or some other valid iid.
	IID			iid={ 0xffffffff, 0xffff, 0xffff, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }};
	IRowset *	pIRowset=NULL;
	ULONG		cErrors=0;

	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		if(!CHECK(m_pIDBSchemaRowset->GetRowset(
			NULL,
			rgSchemas[ulIndex],
			0,
			NULL,
			iid,
			0,
			NULL,
			(IUnknown **) &pIRowset),E_NOINTERFACE))
			cErrors++;

		if(!COMPARE(pIRowset,NULL))
		{
			pIRowset->Release();
			cErrors++;
		}
	}

	if(cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc S_OK: restrictions valid for 1st and 3rd restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_9()
{
	ULONG m_cErrors=0;

	// for each schema guid, test method
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		SetRestriction(FIRST);
		SetRestriction(THIRD);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check if this is a schema I want then
		// get it's info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// want to make sure we have first and
			// third restrictions available
			if((m_restrict & FIRST)	&&
				(m_restrict & THIRD))
			{
				// TODO: remove if statement when their restrictions are supported
				if(m_guid!=DBSCHEMA_CATALOGS &&
					m_guid!=DBSCHEMA_SCHEMATA)
				{
					// test method
					if(!CHECK(GetRowset(), S_OK))
						m_cErrors++;
				}
			}
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else 
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc S_OK: 2 cmd objects open on session
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_10()
{
	ULONG			ulIndex=0;
	ICommand *		pICommand = NULL;

	// If the provider doesn't support commands then 
	// m_pIDBCreateCommand is NULL
	if (!m_pIDBCreateCommand)
	{
		odtLog << L"Commands not supported.\n";
		return TEST_SKIPPED;
	}
	
	// create a second object open on the session object
	if(FAILED(m_pIDBCreateCommand->CreateCommand(
		NULL,
		IID_ICommand,
		(IUnknown **)&pICommand)))
		goto CLEANUP;

	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// test method
			if(!CHECK(GetRowset(), S_OK))
				m_cErrors++;
		}
		// release rowset
		FREE
	}

CLEANUP:

	if(pICommand)
		pICommand->Release();

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc DB_E_ERRORSOCCURED: update properties on read-only rowset
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_11()
{
	ULONG	ulIndex=0;
	ULONG	ulIndexProperties=0;
	ULONG	ulprop=0;

	m_PrintSchemaName=FALSE;

	// for each invalid property
	for(ulIndexProperties=0;ulIndexProperties<cUPDATE_PROPERTIES;ulIndexProperties++)
	{
		INIT;

		if( (IsRowsetPropertySupported(rgUpdateProperties[ulIndexProperties])) || 
			(m_pTable->m_pICommand	&&
			(GetProperty(rgUpdateProperties[ulIndexProperties],DBPROPSET_ROWSET,m_pTable->m_pICommand))))
		{
			odtLog << L"---" << rgwszUpdateProperties[ulIndexProperties] << L"-\n" ;
			
			// for each valid schema, test method
			for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
			{
				m_iid = IID_IRowset;
				LimitRestrictions(ulIndex);

				memset(&m_rgDBPROP[0], 0, sizeof(DBPROP));
				m_rgDBPROP[0].dwPropertyID = rgUpdateProperties[ulIndexProperties];
				m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
				m_rgDBPROP[0].vValue.vt = VT_BOOL;
				m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;

				// Doesn't need to change each time
				m_cDBPROPSET = 1;
				m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
				m_rgDBPROPSET[0].cProperties = 1;
				m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

				// check to make sure it's a schema I want and get its info
				if(GetSchemaInfo(SUPPORTED,ulIndex))
				{
					// since I'm not placing restrictions on the rowset
					// all I need to do is check that what I what came back,
					// not that is was the only thing that came back
					TraceSchemaName(m_guid,FALSE,TRUE);

					// test method
					if(!CHECK(GetRowset(), DB_E_ERRORSOCCURRED))
						m_cErrors++;

					for(ulprop=0; ulprop<m_rgDBPROPSET[0].cProperties; ulprop++)
					{
						// Since this is a r/o rowset the update property is conflicting with DBPROP_UPDATABILITY
						if(!COMPARE(m_rgDBPROPSET[0].rgProperties[ulprop].dwStatus, DBPROPSTATUS_CONFLICTING))
						{
							odtLog << L"Expected DBPROPSTATUS_CONFLICTING for " 
							<< m_rgDBPROPSET[0].rgProperties[ulprop].dwPropertyID
							<< L", received " 
							<< m_rgDBPROPSET[0].rgProperties[ulprop].dwStatus 
							<< ENDL;

							m_cErrors++;
						}
					}
				}
			}

			// release rowset
			FREE;
		}
		else
			odtLog << L"Property not supported or not writable, "
			<< rgwszUpdateProperties[ulIndexProperties]
			<< ENDL;
	}

	m_PrintSchemaName=TRUE;

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc DB_S_ERRORSOCCURED: DBPROP_IRowsetInfo & DBPROP_IRowsetChange
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_12()
{
	ULONG			ulIndex=0;
	HRESULT		expectedHR=E_FAIL;

	if(!IsRowsetPropertySupported(DBPROP_IRowsetInfo))
	{
		L"IRowsetInfo is not supported\n";
		return TEST_PASS;
	}
	if(!IsRowsetPropertySupported(DBPROP_IRowsetChange))
	{
		L"IRowsetChange is not supported\n";
		return TEST_PASS;
	}


	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		LimitRestrictions(ulIndex);

		m_rgDBPROP[0].dwPropertyID = DBPROP_IRowsetInfo;
		m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		m_rgDBPROP[0].vValue.vt = VT_BOOL;
		m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;

		m_rgDBPROP[1].dwPropertyID = DBPROP_IRowsetChange;
		m_rgDBPROP[1].dwOptions = DBPROPOPTIONS_REQUIRED;
		m_rgDBPROP[1].vValue.vt = VT_BOOL;
		m_rgDBPROP[1].vValue.boolVal = VARIANT_TRUE;

		// Doesn't need to change each time
		m_cDBPROPSET = 1;
		m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
		m_rgDBPROPSET[0].cProperties = 2;
		m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			
			GetRowset();

			if((m_rgDBPROP[0].dwStatus & DBPROPSTATUS_NOTSETTABLE) ||
				(m_rgDBPROP[1].dwStatus & DBPROPSTATUS_NOTSETTABLE) ||
				m_HR != S_OK)
			{
				if(!CHECK(m_HR, S_OK))
					m_cErrors++;
			}
		}
		// release rowset
		FREE
	}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc S_OK: DBPROP_BOOKMARKS
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_13()
{
	ULONG			ulIndex=0;

	if(!IsRowsetPropertySupported(DBPROP_BOOKMARKS))
	{
		odtLog << L"Bookmark not supported\n";
		return TEST_PASS;
	}

	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

//		LimitRestrictions(ulIndex);

		m_rgDBPROP[0].dwPropertyID = DBPROP_BOOKMARKS;
		m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		m_rgDBPROP[0].vValue.vt = VT_BOOL;
		m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;


		// Doesn't need to change each time
		m_cDBPROPSET = 1;
		m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
		m_rgDBPROPSET[0].cProperties = 1;
		m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			
			GetRowset();

			if(m_HR != S_OK)
			{
				if(!(m_rgDBPROP[0].dwStatus & DBPROPSTATUS_NOTSETTABLE)) 
				{	
					if(!CHECK(m_HR, S_OK))
						m_cErrors++;
				}
			}
				
		}
		// release rowset
		FREE
	}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc S_OK: empty variants = no restricions passed
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_14()
{
	ULONG			ulIndex=0;

	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT 
		m_fRestrictionsAsInitVariants = TRUE;
		m_iid = IID_IRowset;
		m_restrict = ZERO;

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			
			// test method
			if(!CHECK(GetRowset(), S_OK))
				m_cErrors++;
				
		}
		// release rowset
		FREE
	}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc E_NOINTERFACE or S_OK: all rowset riids
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_15()
{
	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
 		INIT

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;
  
		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// loop thru each Rowset interface
			for(ULONG ulIndexRIID=0;ulIndexRIID<cROWSET_RIID;ulIndexRIID++)
			{

				// get next rowset interface
				odtLog << L"   ###   " << rgwszRIID[ulIndexRIID] << L"\n";
				m_iid = (*rgRowsetIID[ulIndexRIID]);

				// test method, free rowset
				GetRowset();
				
				// Since some providers can extend the mandatory
				// interface list, I will assume that if that do so,
				// it is not an error
				if(m_HR!=S_OK)
					if(!CHECK(m_HR,E_NOINTERFACE))
						m_cErrors++;
			}
		}
		m_rgColumnNames = NULL;
		m_rgColumnTypes = NULL;
		FREE
	}
	
	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc E_NOINTERFACE: riid = IID_IDBProperties
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_16()
{
	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
		m_iid=IID_IDBProperties; 

		LimitRestrictions(ulIndex);

		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// test method
			if(!CHECK(GetRowset(), E_NOINTERFACE))
				m_cErrors++;
		}
		// free rowset
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: ppRowset = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_17()
{
	ULONG m_cErrors=0;

	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;
  
   		// don't pass rowset
		m_fRowsetPointerNULL = TRUE;

		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
 			m_iid = IID_IRowset;

			// test method
			if(!CHECK(GetRowset(), E_INVALIDARG))
				m_cErrors++;
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: cRestrictions > 0, rgRestrictions = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_18()
{
	ULONG m_cErrors=0;

	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT

		SetRestriction(ALLRES);

		// Don't pass rgRestricitons
		m_fRangeRestrictionsNULL = TRUE;

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			m_iid = IID_IRowset;

			// test method
			if(!CHECK(GetRowset(), E_INVALIDARG))
				m_cErrors++;
			
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: cProperty != 0, rgProperties = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_19()
{
	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT

		LimitRestrictions(ulIndex);

		m_cDBPROPSET = 1;

		// set up properties for rowset
		m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
		m_rgDBPROPSET[0].cProperties = 1;
		m_rgDBPROPSET[0].rgProperties = NULL;


		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
  			m_iid = IID_IRowset;

			// test method
			if(!CHECK(GetRowset(), E_INVALIDARG))
				m_cErrors++;
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: cPropertySets > 0, rgPropertySets = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_20()
{
	// loop thru each schema
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT

		m_cDBPROPSET=1;

		LimitRestrictions(ulIndex);

		// Don't pass rgPropSet
		m_fRangePropSetNULL = TRUE;

		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
 			m_iid = IID_IRowset;

			// test method
			if(!CHECK(GetRowset(), E_INVALIDARG))
				m_cErrors++;
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: invalid restriction with wrong VT type
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_21()
{
	ULONG		ulIndex=0;
	HRESULT		hrTemp = E_FAIL;
	IDispatch * pIDispatch = new CDispatch;

	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		ULONG index;
		INIT
		m_iid = IID_IRowset;
		m_restrict = ALLRES;

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{

			// Clear the variant, so we can overwrite below.
			for(index=0;index<MAXRESTRICTION;index++)
				VariantClear(&(m_rgvarRestrict[index]));

			memset(m_rgvarRestrict, 0, MAXRESTRICTION * sizeof(VARIANT));

			if(m_restrict & FIRST)
			{
				m_rgvarRestrict[0].vt = VT_UNKNOWN;
				m_rgvarRestrict[0].punkVal = m_pIDBSchemaRowset;
			}
			if(m_restrict & SECOND)
			{
				m_rgvarRestrict[1].vt = VT_DISPATCH;
				m_rgvarRestrict[1].pdispVal = pIDispatch;
			}
			if(m_restrict & THIRD)
			{
				m_rgvarRestrict[2].vt = VT_ERROR;
				m_rgvarRestrict[2].scode = hrTemp;
			}
			if(m_restrict & FOURTH)
			{
				m_rgvarRestrict[3].vt = VT_BYREF|VT_UNKNOWN;
				m_rgvarRestrict[3].ppunkVal = (IUnknown **)&m_pIDBSchemaRowset;
			}
			if(m_restrict & FIFTH)
			{
				m_rgvarRestrict[4].vt = VT_BYREF|VT_DISPATCH;
				m_rgvarRestrict[4].ppdispVal = &pIDispatch;
			}
			if(m_restrict & SIXTH)
			{
				m_rgvarRestrict[5].vt = VT_BYREF|VT_ERROR;
				m_rgvarRestrict[5].pscode = &hrTemp;
			}
			if(m_restrict & SEVENTH)
			{
				m_rgvarRestrict[6].vt = VT_BYREF|VT_ERROR;
				m_rgvarRestrict[6].pscode = &hrTemp;
			}

			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			if(m_restrict != ALLRES)
			{
				// test method
				if(!CHECK(GetRowset(), E_INVALIDARG))
					m_cErrors++;
			}

			// Set the restrictions back to 0 so FREE doesn't actually free them
			memset(m_rgvarRestrict, 0, MAXRESTRICTION * sizeof(VARIANT));
		}

		// release rowset
		FREE
	}

	SAFE_RELEASE(pIDispatch);

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc S_OK: request DBPROP_IColumnsRowset and, iid = IID_IColumnsRowset
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_22()
{
	ULONG			ulIndex=0;

	if(!IsRowsetPropertySupported(DBPROP_IColumnsRowset))
	{
		odtLog << L"IColumnsRowset is not supported.\n";
		return TEST_SKIPPED;
	}


	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IColumnsRowset;

		LimitRestrictions(ulIndex);

		m_rgDBPROP[0].dwPropertyID = DBPROP_IColumnsRowset;
		m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		m_rgDBPROP[0].vValue.vt = VT_BOOL;
		m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;


		// Doesn't need to change each time
		m_cDBPROPSET = 1;
		m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
		m_rgDBPROPSET[0].cProperties = 1;
		m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			
			GetRowset();

			// test method
			if((m_rgDBPROP[0].dwStatus & DBPROPSTATUS_NOTSETTABLE) ||
				m_HR != S_OK)
			{
				if(!CHECK(m_HR, S_OK))
					m_cErrors++;
			}

				
		}
		// release rowset
		FREE
	}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc S_OK: riid = IID_IRowsetInfo, call IRowset::GetSpecification
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_23()
{
	ULONG			ulIndex=0;
	ULONG			iRiid=0;
	HRESULT			hr=0;

	IUnknown *		pIUnknown=NULL;


	/*
	for(iRiid=0;iRiid<5;iRiid++)
	{
		if(iRiid==0)
		{
			m_iid = IID_IAccessor;
			odtLog << L"IID_IAccessor\n";
		}
		if(iRiid==1)
		{
			m_iid = IID_IColumnsInfo;
			odtLog << L"IID_IColumnsInfo\n";
		}
		if(iRiid==2)
		{
			m_iid = IID_IConvertType;
			odtLog << L"IID_IConvertType\n";
		}
		if(iRiid==3)
		{
			m_iid = IID_IRowset;
			odtLog << L"IID_IRowset\n";
		}
		if(iRiid==4)
		{
			m_iid = IID_IRowsetInfo;
			odtLog << L"IRowsetInfo\n";
		}
	*/
	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IRowsetInfo;

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			if(!CHECK(GetRowset(FALSE), S_OK))
				m_cErrors++;

			if (m_pIRowsetInfo)
			{
				// GetSpecification should succeed
				hr = m_pIRowsetInfo->GetSpecification(IID_IDBSchemaRowset,&pIUnknown);

				if (hr == S_FALSE)
				{
					// Kagera - Temp table has no way of getting back to kagera's objects so it
					// returns S_FALSE

					// While this is allowed it's certainly unusual, print warning
					odtLog << L"WARNING - GetSpecification returned S_FALSE.\n";
					odtLog << L"This is allowed but may signify a failure for this provider.\n";

					// If we claim we didn't return an object make sure
					COMPARE(pIUnknown, NULL);
				}
				else
				{
					// We should have succeeded and returned a valid object pointer
					if (CHECK(hr, S_OK) && COMPARE(pIUnknown != NULL, TRUE))
					{
						IOpenRowset * pIOpenRowset = NULL;

						// Schema rowsets are always opened via IDBSchemaRowset, therefore GetSpecification
						// should return a session interface.
						if (COMPARE(VerifyInterface(pIUnknown, IID_IOpenRowset, SESSION_INTERFACE, (IUnknown**)&pIOpenRowset),TRUE))
						{
							pIOpenRowset->Release();
							pIOpenRowset = NULL;
						}

						pIUnknown->Release();
						pIUnknown=NULL;
					}
				}
			}
				
		}
		// release rowset
		FREE
	}
	//}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;}
// }}


// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc S_OK: riid = IID_IUnknown, get IRowset from IUnknown
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_24()
{
	ULONG			ulIndex=0;


	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IUnknown;

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			if(!CHECK(GetRowset(), S_OK))
				m_cErrors++;
		}
		// release rowset
		FREE
	}


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc S_OK: open rowset from schema, try to open rowset on command
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_25()
{
	ULONG				ulIndex=0;
	IUnknown *			pICommandRowset=NULL;
	ICommandText *		pICommandText=NULL;
	WCHAR *				pSQLText=NULL;
	IDBCreateCommand *	pIDBCreateCommand=NULL;

	if(m_pIDBCreateCommand)
	{
		if(!CHECK(m_pIDBCreateCommand->CreateCommand(NULL,IID_ICommandText,(IUnknown**)&pICommandText),S_OK))
			return TEST_FAIL;
	}
	else
	{
		// Check to see if commands are supported
		if(!VerifyInterface(m_pIOpenRowset, IID_IDBCreateCommand, 
							SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand))
			goto CLEANUP;
		
		if(!CHECK(pIDBCreateCommand->CreateCommand(NULL,IID_ICommandText,(IUnknown**)&pICommandText),S_OK))
		{
			m_cErrors++;
			goto CLEANUP;
		}
	}

	if(!CHECK(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,m_pTable->GetTableName(),&pSQLText,NULL,NULL),S_OK))
	{
		m_cErrors++;
		goto CLEANUP;
	}

	if(!CHECK(pICommandText->SetCommandText(DBGUID_DBSQL,pSQLText),S_OK))
	{
		m_cErrors++;
		goto CLEANUP;
	}

	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		LimitRestrictions(ulIndex);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// since I'm not placing restrictions on the rowset
			// all I need to do is check that what I what came back,
			// not that is was the only thing that came back
			
			GetRowset(FALSE);

			if(!CHECK(pICommandText->Execute(NULL,IID_IUnknown,NULL,NULL,&pICommandRowset),S_OK))
				m_cErrors++;

			if(pICommandRowset)
			{
				pICommandRowset->Release();
				pICommandRowset=NULL;
			}

			if(!CHECK(m_HR, S_OK))
				m_cErrors++;
		}
		// release rowset
		FREE
	}
CLEANUP:

	SAFE_RELEASE(pICommandText);
	SAFE_RELEASE(pIDBCreateCommand);
	
	// Free the memory
	PROVIDER_FREE(pSQLText);

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc S_OK: IRowsetScroll
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_26()
{

	ULONG			ulIndex=0;
	BOOL	fFound=FALSE;
	IRowsetInfo*	pIRowsetInfo=NULL;
	HRESULT	hrExpect = S_OK;

	if(!IsRowsetPropertySupported(DBPROP_IRowsetScroll))
	{
		odtLog << L"DBPROP_IRowsetScroll is not supported.\n";
		hrExpect = DB_E_ERRORSOCCURRED;
	} 


	// for each valid schema, test method
	for(ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT
 		m_iid = IID_IRowsetInfo;

		LimitRestrictions(ulIndex);

		m_rgDBPROP[0].dwPropertyID = DBPROP_IRowsetScroll;
		m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
		m_rgDBPROP[0].vValue.vt = VT_BOOL;
		m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;

		// Doesn't need to change each time
		m_cDBPROPSET = 1;
		m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
		m_rgDBPROPSET[0].cProperties = 1;
		m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			m_HR=m_pIDBSchemaRowset->GetRowset(
				NULL,
				m_guid,
				m_cRestrictionsCurrent,
				m_rgvarRestrict,
				m_iid,
				m_cDBPROPSET,
				m_rgDBPROPSET,
				(IUnknown **) &pIRowsetInfo);

			// Even if GetPropertyInfo reports IRowsetScroll is not supported it's not an
			// error to provide bonus functionality.  We allow this because some providers
			// have different rowset functionality for schema rowsets.
			if(SUCCEEDED(m_HR))
			{
				// make sure property is returned as TRUE
				// from IRowsetInfo->GetProperties
				//ULONG			cPropIDSet=1;
				//DBPROPIDSET		rgPropIDSet[1];
				//DBPROPID		rgPropID[1];
				ULONG			cPropSets=0;
				DBPROPSET *		rgPropSets=NULL;
				ULONG			i,j;

				//rgPropIDSet[0].rgPropertyIDs = rgPropID;
				//rgPropIDSet[0].cPropertyIDs = 1;
				//rgPropIDSet[0].guidPropertySet = DBPROPSET_ROWSET;

				if(!CHECK(m_HR=pIRowsetInfo->GetProperties(
					0/*cPropIDSet*/,
					NULL/*rgPropIDSet*/,
					&cPropSets,
					&rgPropSets),S_OK))
				{
					m_cErrors++;
				}
				else
				{
					for(i=0;i<cPropSets;i++)
					{
						if(rgPropSets[i].guidPropertySet==DBPROPSET_ROWSET)
						{
							for(j=0;j<rgPropSets[i].cProperties;j++)
							{
								if(rgPropSets[i].rgProperties[j].dwPropertyID == DBPROP_IRowsetScroll)
								{
									if(rgPropSets[i].rgProperties[j].vValue.boolVal==FALSE)
									{
										odtLog << L"Property is not TRUE\n";
										m_cErrors++;
										fFound = FALSE;
									}
									else
									{
										fFound = TRUE;
									}
									goto END;
								}
							}
						}
					}

				}

				// todo: release mem for properties
END:			if(rgPropSets)
				{
					for(i=0;i<cPropSets;i++)
					{
						if(rgPropSets[i].rgProperties)
						{
							PROVIDER_FREE(rgPropSets[i].rgProperties);
							rgPropSets[i].rgProperties = NULL;
						}
					}
					PROVIDER_FREE(rgPropSets);
					rgPropSets=NULL;
				}


			}
			else
			{
				// If GetRowset failed it better be with the expected return code
				if (!CHECK(m_HR, hrExpect))
					m_cErrors++;
			}

			if(pIRowsetInfo)
			{
				pIRowsetInfo->Release();
				pIRowsetInfo=NULL;
			}
		}
		// release rowset
		FREE
	}

//CLEANUP:


	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(27)
//--------------------------------------------------------------------
// @mfunc E_INVALIDARG: Schema not supported
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetRowset::Variation_27()
{
	TESTRESULT fTestResult=TEST_PASS;

	// Loop through all schemas in spec
	for(ULONG ulIndex=0;ulIndex<NUMELEM(g_prgSchemas);ulIndex++)
	{
		// If the schema isn't supported then try to call GetRowset
		if (!IsSchemaSupported(*g_prgSchemas[ulIndex]))
		{
			INIT

			m_iid = IID_IRowset;
			m_restrict = ZERO;

			// don't need restrictions
			m_fCountRestrictionsNULL = TRUE;
			m_fRangeRestrictionsNULL = TRUE;

			// don't need properties
			m_fCountPropSetNULL = TRUE;
			m_fRangePropSetNULL = TRUE;

			m_guid=*g_prgSchemas[ulIndex];

			// test method
			if (!CHECK(GetRowset(), E_INVALIDARG))
				fTestResult = TEST_FAIL;

			FREE
		}
	}

	return fTestResult;
}


// }}




// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc S_OK: Empty result set, pass first restriction that matches second restriction
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int CGetRowset::Variation_28()
{ 
	ULONG m_cErrors=0;

	// for each schema guid, test method
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		SetRestriction(FIRST);
		SetRestriction(SECOND);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check if this is a schema I want then
		// get it's info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			// want to make sure we have first
			// restriction available
			if(m_restrict & FIRST && m_restrict & SECOND)
			{
				// The restriction vt type must match between first and second restriction.
				// Provider MAY OPTIONALLY support conversions, but is not required to.
				if (m_rgvarRestrict[0].vt == m_rgvarRestrict[1].vt)
				{
					VARIANT vTemp;

					// Save the value for the second restriction
					VariantInit(&vTemp);
					VariantCopy(&vTemp, &m_rgvarRestrict[1]);

					// Clear out all restrictions
					for(ULONG index=0;index<MAXRESTRICTION;index++)
						VariantClear(&(m_rgvarRestrict[index]));
		
					// Now reset the first restriction to the second restriction's value
					SetRestriction(ZERO);
					SetRestriction(FIRST);
					VariantCopy(&m_rgvarRestrict[0], &vTemp);
					VariantClear(&vTemp);
					
					// TODO: remove if statement when their restrictions are supported
					if(m_guid!=DBSCHEMA_CATALOGS &&
						m_guid!=DBSCHEMA_SCHEMATA)
					{

						// Expect no rows back
						SetRowCount(EXACT_VALUE, 0);

						// test method
						if(!CHECK(GetRowset(), S_OK))
							m_cErrors++;
					}

				}
			}
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else 
		return TEST_PASS;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc S_OK: Empty result set, pass non-matching value for each restriction
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int CGetRowset::Variation_29()
{ 
	ULONG m_cErrors=0;
	ULONG ulRestrict;

	// for each schema guid, test method
	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT
		m_iid = IID_IRowset;

		SetRestriction(ALLRES);

		// no properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;


		// check if this is a schema I want then
		// get it's info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{

			// for all possible restrictions
			for (ulRestrict = 0; ulRestrict < MAXRESTRICTION; ulRestrict++)
			{
				VARIANT vTemp;

				// If restriction not supported
				if(!(m_restrict & (1 << ulRestrict)))
					continue;

				// It's impossible to come up with a nonmatching
				// value for boolean restrictions without first going 
				// the rowset, so we'll skip boolean (VARIANT_BOOL)
				// restrictions.
				if (V_VT(&m_rgvarRestrict[ulRestrict]) == VT_BOOL)
					continue;

				// Save the valid value for the restriction
				VariantInit(&vTemp);
				VariantCopy(&vTemp, &m_rgvarRestrict[ulRestrict]);

				// Now reset to a bogus nonmatching value.  We assume all available
				// bits set on will not match any restriction.
				if (V_VT(&m_rgvarRestrict[ulRestrict]) == VT_BSTR)
					memset(V_BSTR(&m_rgvarRestrict[ulRestrict]), 0xF,
						wcslen(V_BSTR(&m_rgvarRestrict[ulRestrict]))*sizeof(WCHAR));
				else
					// For all other types just set all bits on. At this time there
					// are no BYREF restrictions.
					memset(&m_rgvarRestrict[ulRestrict].bVal, 0xF, 
						sizeof(VARIANT)-sizeof(VARTYPE)-sizeof(USHORT)*3);


				// TODO: remove if statement when their restrictions are supported
				if(m_guid!=DBSCHEMA_CATALOGS &&
					m_guid!=DBSCHEMA_SCHEMATA)
				{
					// Over-ride the row count expected, we MUST get 0 rows
					SetRowCount(EXACT_VALUE, 0);

					// test method
					if(!CHECK(GetRowset(TRUE), S_OK))
					{
						m_cErrors++;
						odtLog << L"Failure on restriction " << ulRestrict+1 << L"\n";
					}
				}
		
				// Put the original value back
				VariantCopy(&m_rgvarRestrict[ulRestrict], &vTemp);
				VariantClear(&vTemp);
			}
		}
		FREE
	}

	if(m_cErrors)
		return TEST_FAIL;
	else 
		return TEST_PASS;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CGetRowset::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CGetSchema)
//*-----------------------------------------------------------------------
//| Test Case:		CGetSchema - test IDBSchemaRowset::GetSchemas
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CGetSchema::Init()
{
	// DON'T need restrictions in order to test schema
	m_fDontCaptureRestrictions = TRUE;

	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: pcSchemas = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_1()
{
	INIT

	GUID *	rgSchemas=NULL;
	ULONG	cSchemas=0;
	ULONG * rgRestrictionSupport=NULL;
	
	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(NULL,&rgSchemas,&rgRestrictionSupport),E_INVALIDARG))
		m_fResult = TEST_PASS;

	if((!COMPARE(rgRestrictionSupport,NULL)) ||	
		(!COMPARE(rgSchemas,NULL)))
		m_fResult = TEST_FAIL;

	FREE

	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc S_OK: *prgSchemas = NULL, pcSchemas = 0
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_2()
{
	INIT

	GUID *	rgSchemas=NULL;
	ULONG	cSchemas=0;
	ULONG * rgRestrictionSupport=NULL;

	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,&rgSchemas,&rgRestrictionSupport),S_OK))
		m_fResult = TEST_PASS;

	// Free the memory
	PROVIDER_FREE(rgSchemas);
	PROVIDER_FREE(rgRestrictionSupport);

	// free the array
	FREE

	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: prgRestrictionSupported = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_3()
{
	INIT

	GUID *	rgSchemas=NULL;
	ULONG	cSchemas=0;
	ULONG * rgRestrictionSupport=NULL;

	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,&rgSchemas,NULL),E_INVALIDARG))
		m_fResult = TEST_PASS;

	if((!COMPARE(cSchemas,0)) ||	
		(!COMPARE(rgSchemas,NULL)))
		m_fResult = TEST_FAIL;
		

	// free the array
	FREE

	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc E_INVALIDARG: prgSchemas = NULL
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_4()
{
	INIT

	GUID *	rgSchemas=NULL;
	ULONG	cSchemas=0;
	ULONG * rgRestrictionSupport=NULL;
	
	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,NULL,&rgRestrictionSupport),E_INVALIDARG))
		m_fResult = TEST_PASS;

	if((!COMPARE(cSchemas,0)) ||	
		(!COMPARE(rgRestrictionSupport,NULL)))
		m_fResult = TEST_FAIL;

	FREE

	return m_fResult;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc S_OK: open schema rowset, try to open rowset from command object
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_5()
{
	INIT

	GUID *				rgSchemas=NULL;
	ULONG				cSchemas=0;
	ULONG *				rgRestrictionSupport=NULL;
	ULONG				ulIndex=0;
	IUnknown *			pICommandRowset=NULL;
	ICommandText *		pICommandText=NULL;
	WCHAR *				pSQLText=NULL;
	IDBCreateCommand *	pIDBCreateCommand=NULL;

	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,&rgSchemas,&rgRestrictionSupport),S_OK))
		m_fResult = TEST_PASS;

	if(m_pIDBCreateCommand)
	{
		if(!CHECK(m_pIDBCreateCommand->CreateCommand(NULL,IID_ICommandText,(IUnknown**)&pICommandText),S_OK))
			return TEST_FAIL;
	}
	else
	{
		// Check to see if commands are supported
		if(!VerifyInterface(m_pIOpenRowset, IID_IDBCreateCommand, 
							SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand))
			goto CLEANUP;
		
		if(!CHECK(pIDBCreateCommand->CreateCommand(NULL,IID_ICommandText,(IUnknown**)&pICommandText),S_OK))
		{
			m_cErrors++;
			goto CLEANUP;
		}
	}

	if(!CHECK(m_pTable->CreateSQLStmt(SELECT_ALLFROMTBL,m_pTable->GetTableName(),&pSQLText,NULL,NULL),S_OK))
	{
		m_cErrors++;
		goto CLEANUP;
	}

	if(!CHECK(pICommandText->SetCommandText(DBGUID_DBSQL,pSQLText),S_OK))
	{
		m_cErrors++;
		goto CLEANUP;
	}


	if(!CHECK(pICommandText->Execute(NULL,IID_IUnknown,NULL,NULL,&pICommandRowset),S_OK))
		m_cErrors++;

	if(pICommandRowset)
	{
		pICommandRowset->Release();
		pICommandRowset=NULL;
	}


CLEANUP:

	// Free the memory
	PROVIDER_FREE(rgSchemas);
	PROVIDER_FREE(rgRestrictionSupport);
	PROVIDER_FREE(pSQLText);

	// free the array
	FREE

	if(pICommandText)
		pICommandText->Release();

	if(pIDBCreateCommand)
		pIDBCreateCommand->Release();

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;

}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc S_OK: don't initialize variables before sending
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CGetSchema::Variation_6()
{
	INIT

	GUID *	rgSchemas;
	ULONG	cSchemas;
	ULONG * rgRestrictionSupport;

	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,&rgSchemas,&rgRestrictionSupport),S_OK))
		m_fResult = TEST_PASS;

	for(ULONG index=0;index<cSchemas;index++)
	{
		TraceSchemaName(rgSchemas[index],FALSE,TRUE);
		TraceRestrictions(rgRestrictions,index,NULL,TRUE);
	}

	// Free the memory
	PROVIDER_FREE(rgSchemas);
	PROVIDER_FREE(rgRestrictionSupport);

	// free the array
	FREE

	return m_fResult;
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CGetSchema::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CRowsetProperties)
//*-----------------------------------------------------------------------
//| Test Case:		CRowsetProperties - test rowset properties on IDBSchemaRowset
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CRowsetProperties::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc S_OK: all rowset properties as required
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CRowsetProperties::Variation_1()
{
	ULONG iDBPROPSET_ROWSET=0;
	ULONG	i=0;
	ULONG	j=0;
	ULONG	ulExpRows = 1;

	m_PrintSchemaName=FALSE;

	for(ULONG index=0;index<m_cRowsetPropSet;index++)
	{
		if(IsEqualGUID(m_rgRowsetPropSet[index].guidPropertySet,DBPROPSET_ROWSET))
	 		iDBPROPSET_ROWSET = index;
	}

	// each schema
	for(j=0;j<m_cSchemasSupported;j++)
	{
		// each property
		for(i=0;i<m_rgRowsetPropSet[iDBPROPSET_ROWSET].cProperties;i++)
		{
			// can't do this check if I go thru IRowsetInfo->GetProperties
			//if(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwFlags & DBPROPFLAGS_WRITE)
			//{
				// each schema
				INIT
					
				LimitRestrictions(j);

				if(GetSchemaInfo(SUPPORTED,j))
				{

					// Doesn't need to change each time
					m_cDBPROPSET = 1;
					m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
					m_rgDBPROPSET[0].cProperties = 1;
					m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

					// Get next property
					m_rgDBPROP[0].dwPropertyID=m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID;
					m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;

					if(PropIsBool(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID))
					{
						m_rgDBPROP[0].vValue.vt = VT_BOOL;
						m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;
					}


					// make sure it is a rowset pointer
					m_iid = IID_IRowset;
					
					if (i == 0)
						TraceSchemaName(m_guid, TRUE, TRUE);

					TracePropertyName(
						m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID,	
						TRUE,
						TRUE);

					// Set expected row count for the schemas we know the count for given
					// the restrictions applied.  LimitRestrictions will restrict to the current
					// catalog, schema, table (m_pTable).
					if (m_guid == DBSCHEMA_TABLES ||
						m_guid == DBSCHEMA_TABLES_INFO)
					{
						// We know the exact number of rows to be returned (1 in this case)
						SetRowCount(EXACT_VALUE, 1);
					}
					else if (m_guid == DBSCHEMA_COLUMNS)
					{
						// We know the exact number of rows to be returned (cColumns in this case)
						SetRowCount(EXACT_VALUE, m_pTable->CountColumnsOnTable());
					}	

					GetRowset(TRUE);
					
					// need to check that property is writable before
					// I think it is an error that it could not be set
					if(m_HR!=S_OK && m_HR!=DB_E_ERRORSOCCURRED)
					{
						if(!CHECK(m_HR, S_OK))
							m_cErrors++;
					}

				}
				FREE
			//}
		}
	}
	m_PrintSchemaName=TRUE;
	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc S_OK: all rowset properties as setifcheap
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CRowsetProperties::Variation_2()
{
	ULONG iDBPROPSET_ROWSET=0;
	ULONG	i=0;
	ULONG	j=0;

	m_PrintSchemaName=FALSE;

	for(ULONG index=0;index<m_cRowsetPropSet;index++)
	{
		if(IsEqualGUID(m_rgRowsetPropSet[index].guidPropertySet,DBPROPSET_ROWSET))
	 		iDBPROPSET_ROWSET = index;
	}

	// each property
	//for(i=0;i<m_rgRowsetPropSet[iDBPROPSET_ROWSET].cProperties;i++)
	{
		//TracePropertyName(
		//	m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID,	
		//	TRUE,
		//	FALSE);

		// each schema
		for(j=0;j<m_cSchemasSupported;j++)
		{
			// can't do this check if I go thru IRowsetInfo->GetProperties
			//if(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwFlags & DBPROPFLAGS_WRITE)
			//{
				// each schema
				INIT

				LimitRestrictions(j);

				if(GetSchemaInfo(SUPPORTED,j))
				{

					// Doesn't need to change each time
					m_cDBPROPSET = 1;
					m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
					m_rgDBPROPSET[0].cProperties = 1;
					m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

					// Get next property
					m_rgDBPROP[0].dwPropertyID=m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID;
					m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_SETIFCHEAP;

					if(PropIsBool(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID))
					{
						m_rgDBPROP[0].vValue.vt = VT_BOOL;
						m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;
					}


					// make sure it is a rowset pointer
					m_iid = IID_IRowset;


					GetRowset();
					
					// need to check that property is writable before
					// I think it is an error that it could not be set
					if(m_HR!=S_OK && m_HR!=DB_S_ERRORSOCCURRED)
					{
						if(!CHECK(m_HR, S_OK))
							m_cErrors++;
					}

				}
				FREE
			//}
		}
	}
	m_PrintSchemaName=TRUE;
	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc S_OK: all rowset properties, 2 at time, 1 setifcheap, 1 required
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CRowsetProperties::Variation_3()
{
	ULONG iDBPROPSET_ROWSET=0;
	ULONG	i=0;
	ULONG	j=0;

	m_PrintSchemaName=FALSE;

	for(ULONG index=0;index<m_cRowsetPropSet;index++)
	{
		if(IsEqualGUID(m_rgRowsetPropSet[index].guidPropertySet,DBPROPSET_ROWSET))
	 		iDBPROPSET_ROWSET = index;
	}

	// each property
	//for(i=0;i<(m_rgRowsetPropSet[iDBPROPSET_ROWSET].cProperties-1);i++)
	{
		//TracePropertyName(
		//	m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID,	
		//	TRUE,
		//	FALSE);

		// each schema
		for(j=0;j<m_cSchemasSupported;j++)
		{
				// each schema
				INIT

				LimitRestrictions(j);

				if(GetSchemaInfo(SUPPORTED,j))
				{

					// Doesn't need to change each time
					m_cDBPROPSET = 1;
					m_rgDBPROPSET[0].guidPropertySet = DBPROPSET_ROWSET;
					m_rgDBPROPSET[0].cProperties = 2;
					m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;

					// Get next property
					m_rgDBPROP[0].dwPropertyID=m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID;
					m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
					if(PropIsBool(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i].dwPropertyID))
					{
						m_rgDBPROP[0].vValue.vt = VT_BOOL;
						m_rgDBPROP[0].vValue.boolVal = VARIANT_TRUE;
					}

					// Get next property
					m_rgDBPROP[1].dwPropertyID=m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i+1].dwPropertyID;
					m_rgDBPROP[1].dwOptions = DBPROPOPTIONS_SETIFCHEAP;
					if(PropIsBool(m_rgRowsetPropSet[iDBPROPSET_ROWSET].rgProperties[i+1].dwPropertyID))
					{
						m_rgDBPROP[1].vValue.vt = VT_BOOL;
						m_rgDBPROP[1].vValue.boolVal = VARIANT_TRUE;
					}

					// make sure it is a rowset pointer
					m_iid = IID_IRowset;

					GetRowset();

					
					if(m_HR!=S_OK && m_HR!=DB_E_ERRORSOCCURRED && m_HR != DB_S_ERRORSOCCURRED)
					{
						if(!CHECK(m_HR, S_OK))
							m_cErrors++;
					}
					// only an error if the property status is not
					// DBPROPSTATUS_OK
					// DBPROPSTATUS_NOTSETTABLE
					// DBPROPSTATUS_CONFLICTING
					/*PRVTRACE(L"value=%d,%d,%d\n",DBPROPSTATUS_OK,DBPROPSTATUS_NOTSETTABLE,DBPROPSTATUS_CONFLICTING);

					if((m_rgDBPROP[0].dwStatus == DBPROPSTATUS_OK) ||
						(m_rgDBPROP[0].dwStatus == DBPROPSTATUS_NOTSETTABLE) ||
						(m_rgDBPROP[0].dwStatus == DBPROPSTATUS_CONFLICTING))
					{
						if(!CHECK(m_HR, S_OK))
							m_cErrors++;
					}
					else
					{
						m_cErrors++;

						odtLog << L"Failure of first: Status of "
							<< m_rgDBPROP[0].dwPropertyID 
							<<L" property ==" 
							<< m_rgDBPROP[0].dwStatus << ENDL;
					}

					if((m_rgDBPROP[1].dwStatus == DBPROPSTATUS_OK) ||
						(m_rgDBPROP[1].dwStatus == DBPROPSTATUS_NOTSETTABLE) ||
						(m_rgDBPROP[1].dwStatus == DBPROPSTATUS_CONFLICTING))
					{
						;
						//odtLog << L"Success: Status of "<< m_rgDBPROP[0].dwPropertyID 
						//<<L" property ==" << m_rgDBPROP[0].dwStatus << L" -- Status of "
						//<<m_rgDBPROP[1].dwPropertyID <<L" property ==" << m_rgDBPROP[1].dwStatus << ENDL;
						if(!CHECK(m_HR, S_OK))
							m_cErrors++;
					}
					else
					{
						m_cErrors++;

						odtLog << L"Failure of second: Status of "
							<<m_rgDBPROP[1].dwPropertyID 
							<<L" property ==" 
							<< m_rgDBPROP[1].dwStatus << ENDL;

					}*/

				}
				FREE
			//}
		}
	}
	m_PrintSchemaName=TRUE;

	if(m_cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CRowsetProperties::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CNonRowsetProperties)
//*-----------------------------------------------------------------------
//| Test Case:		CNonRowsetProperties - Make sure all non-rowset properties fail
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CNonRowsetProperties::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DB_E_ERRORSOCCURRED: non-rowset property sets, all properties in that set
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CNonRowsetProperties::Variation_1()
{
	ULONG	i=0;
	ULONG	j=0;

	m_PrintSchemaName=FALSE;

	for(ULONG index=0;index<m_cDBPROPINFOSET;index++)
	{
		if(!IsRowsetPropertySet(m_rgDBPROPINFOSET[index].guidPropertySet))
		{
			if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_COLUMN))
				odtLog << L"[DBPROPSET_COLUMN]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DATASOURCE))
				odtLog << L"[DBPROPSET_DATASOURCE]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DATASOURCEALL))
				odtLog << L"[DBPROPSET_DATASOURCEALL]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DATASOURCEINFOALL))
				odtLog << L"[DBPROPSET_DATASOURCEINFOALL]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DATASOURCEINFO))
				odtLog << L"[DBPROPSET_DATASOURCEINFO]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DBINIT))
				odtLog << L"[DBPROPSET_DBINIT]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_DBINITALL))
				odtLog << L"[DBPROPSET_DBINITALL]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_INDEX))
				odtLog << L"[DBPROPSET_INDEX]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_ROWSET))
				odtLog << L"[DBPROPSET_ROWSET]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_ROWSETALL))
				odtLog << L"[DBPROPSET_ROWSETALL]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_SESSION))
				odtLog << L"[DBPROPSET_SESSION]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_SESSIONALL))
				odtLog << L"[DBPROPSET_SESSIONALL]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_TABLE))
				odtLog << L"[DBPROPSET_TABLE]\n";
			else if(IsEqualGUID(m_rgDBPROPINFOSET[index].guidPropertySet,DBPROPSET_PROPERTIESINERROR))
				odtLog << L"[DBPROPSET_PROPERTIESINERROR]\n";
			else
				odtLog << L"[Unknown DBPROPSET]\n";


			// each property
		//	for(i=0;i<m_rgDBPROPINFOSET[index].cPropertyInfos;i++)
			{
				odtLog << L"---" << m_rgDBPROPINFOSET[index].rgPropertyInfos[i].pwszDescription << ENDL;
	
				// each schema
				for(j=0;j<m_cSchemasSupported;j++)
				{
					INIT

					LimitRestrictions(j);

					// Doesn't need to change each time
					m_cDBPROPSET = 1;
					m_rgDBPROP[0].dwPropertyID=m_rgDBPROPINFOSET[index].rgPropertyInfos[i].dwPropertyID;
					m_rgDBPROP[0].dwOptions = DBPROPOPTIONS_REQUIRED;
					//m_rgDBPROP[0].vValue.vt = VT_BOOL;
					//m_rgDBPROP[0].vValue.bool = TRUE;

					m_rgDBPROPSET[0].guidPropertySet = m_rgDBPROPINFOSET[index].guidPropertySet;
					m_rgDBPROPSET[0].cProperties = 1;
					m_rgDBPROPSET[0].rgProperties = m_rgDBPROP;
					m_iid = IID_IRowset;

					// each schema
					if(GetSchemaInfo(SUPPORTED,j))
					{

						// test method with invalid schema guid
						CHECK(m_HR=GetRowset(), DB_E_ERRORSOCCURRED);
					}
 					FREE
				}
			}
		}
	}
	m_PrintSchemaName=TRUE;
	return TEST_PASS;
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CNonRowsetProperties::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}

// {{ TCW_TC_PROTOTYPE(CAssertions)
//*-----------------------------------------------------------------------
//| Test Case:		CAssertions - test restrictions on assertion schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CAssertions::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ConstraintCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CAssertions::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_ASSERTIONS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ConstraintSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CAssertions::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_ASSERTIONS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ConstraintName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CAssertions::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_ASSERTIONS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CAssertions::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CCatalogs)
//*-----------------------------------------------------------------------
//| Test Case:		CCatalogs - test restrictions on catalog schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCatalogs::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc CatalogName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCatalogs::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_CATALOGS, FIRST);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCatalogs::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CCharacterSets)
//*-----------------------------------------------------------------------
//| Test Case:		CCharacterSets - test restrictions on character set schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCharacterSets::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc CharacterSetCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCharacterSets::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_CHARACTER_SETS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc CharacterSetSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCharacterSets::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_CHARACTER_SETS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc CharacterSetName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCharacterSets::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_CHARACTER_SETS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCharacterSets::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CCheckConstraints)
//*-----------------------------------------------------------------------
//| Test Case:		CCheckConstraints - test restrictions on check constraint schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCheckConstraints::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ConstraintCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCheckConstraints::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_CHECK_CONSTRAINTS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ConstraintSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCheckConstraints::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_CHECK_CONSTRAINTS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ConstraintName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCheckConstraints::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_CHECK_CONSTRAINTS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCheckConstraints::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CCollations)
//*-----------------------------------------------------------------------
//| Test Case:		CCollations - test restrictions on collations schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCollations::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc CollationCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCollations::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_COLLATIONS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc CollationSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCollations::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_COLLATIONS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc CollationName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CCollations::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_COLLATIONS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CCollations::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CColumnDomainUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CColumnDomainUsage - test restrictions on column domain schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumnDomainUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DomainCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnDomainUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_DOMAIN_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DomainSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnDomainUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_DOMAIN_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc DominName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnDomainUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_DOMAIN_USAGE, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnDomainUsage::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_DOMAIN_USAGE, FOURTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumnDomainUsage::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CColumnPrivileges)
//*-----------------------------------------------------------------------
//| Test Case:		CColumnPrivileges - test restrictions on column privileges schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumnPrivileges::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Grantor
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, FIFTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Grantee
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumnPrivileges::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMN_PRIVILEGES, SIXTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumnPrivileges::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CColumns)
//*-----------------------------------------------------------------------
//| Test Case:		CColumns - test restrictions on column schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumns::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, FOURTH, MIN_REQUIRED, 1);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc All Supported Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, (FIRST|SECOND|THIRD|FOURTH), MIN_REQUIRED, 1);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc All Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, ALLRES, MIN_REQUIRED, 1);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Extra Restriction
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CColumns::Variation_7()
{
	return TestSchemaRestrictions(DBSCHEMA_COLUMNS, FIFTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CColumns::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CConstraintColumnUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CConstraintColumnUsage - test restrictions on constrain column usage
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CConstraintColumnUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintColumnUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_COLUMN_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintColumnUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_COLUMN_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintColumnUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_COLUMN_USAGE, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintColumnUsage::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_COLUMN_USAGE, FOURTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CConstraintColumnUsage::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CConstraintTableUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CConstraintTableUsage - test restrictions on constraint table usage
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CConstraintTableUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintTableUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_TABLE_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintTableUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_TABLE_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CConstraintTableUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_CONSTRAINT_TABLE_USAGE, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CConstraintTableUsage::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CForeignKeys)
//*-----------------------------------------------------------------------
//| Test Case:		CForeignKeys - test restrictions on foreign key schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CForeignKeys::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc PKTableName and FKTableName restrictions.
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CForeignKeys::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_FOREIGN_KEYS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CForeignKeys::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CIndexes)
//*-----------------------------------------------------------------------
//| Test Case:		CIndexes - test restrictions on index schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CIndexes::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CIndexes::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_INDEXES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CIndexes::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_INDEXES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc IndexName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CIndexes::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_INDEXES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Type
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CIndexes::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_INDEXES, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CIndexes::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_INDEXES, FIFTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CIndexes::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CKeyColumnUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CKeyColumnUsage - test restrictions on key column usage
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CKeyColumnUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ConstraintCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ConstraintSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ConstraintName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, FIFTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, SIXTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CKeyColumnUsage::Variation_7()
{
	return TestSchemaRestrictions(DBSCHEMA_KEY_COLUMN_USAGE, SEVENTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CKeyColumnUsage::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CPrimaryKeys)
//*-----------------------------------------------------------------------
//| Test Case:		CPrimaryKeys - test restrictions on primary key schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CPrimaryKeys::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CPrimaryKeys::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_PRIMARY_KEYS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CPrimaryKeys::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_PRIMARY_KEYS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CPrimaryKeys::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_PRIMARY_KEYS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CPrimaryKeys::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CProcedureColumns)
//*-----------------------------------------------------------------------
//| Test Case:		CProcedureColumns - test restrictions on procedure column schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedureColumns::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ProcedureCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureColumns::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_COLUMNS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ProcedureSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureColumns::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_COLUMNS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ProcedureName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureColumns::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_COLUMNS, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ColumnName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureColumns::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_COLUMNS, FOURTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedureColumns::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CProcedureParameters)
//*-----------------------------------------------------------------------
//| Test Case:		CProcedureParameters - test restrictions on procedure parameters
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedureParameters::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ProcedureCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ProcedureSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ProcedureName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ParameterName - Input
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Parameter Name - Input/Output
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Parameter Name - RETURN_VALUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedureParameters::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURE_PARAMETERS, FOURTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedureParameters::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CProcedures)
//*-----------------------------------------------------------------------
//| Test Case:		CProcedures - test restrictions on procedures schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedures::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ProcedureCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedures::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ProcedureSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedures::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ProcedureName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedures::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ProcedureType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProcedures::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_PROCEDURES, FOURTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProcedures::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CProviderTypes)
//*-----------------------------------------------------------------------
//| Test Case:		CProviderTypes - test restrictions on provider types
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProviderTypes::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc DataType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProviderTypes::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_PROVIDER_TYPES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc BestMatch
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CProviderTypes::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_PROVIDER_TYPES, SECOND);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CProviderTypes::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CReferentialContraints)
//*-----------------------------------------------------------------------
//| Test Case:		CReferentialContraints - test restrictions on referential constraint schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CReferentialContraints::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ConstraintCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CReferentialContraints::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_REFERENTIAL_CONSTRAINTS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ConstraintSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CReferentialContraints::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_REFERENTIAL_CONSTRAINTS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ConstraintName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CReferentialContraints::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_REFERENTIAL_CONSTRAINTS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CReferentialContraints::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CSchemata)
//*-----------------------------------------------------------------------
//| Test Case:		CSchemata - test restrictions on schemata schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CSchemata::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc CatalogName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CSchemata::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_SCHEMATA, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc SchemaName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CSchemata::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_SCHEMATA, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc SchemaOwner
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CSchemata::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_SCHEMATA, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CSchemata::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CSqlLanguages)
//*-----------------------------------------------------------------------
//| Test Case:		CSqlLanguages - test restrictions on sql languages schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CSqlLanguages::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CSqlLanguages::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CStatistics)
//*-----------------------------------------------------------------------
//| Test Case:		CStatistics - test restrictions on statistics schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CStatistics::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CStatistics::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_STATISTICS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CStatistics::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_STATISTICS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CStatistics::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_STATISTICS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CStatistics::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CTableConstraints)
//*-----------------------------------------------------------------------
//| Test Case:		CTableConstraints - test restrictions on table contraint schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTableConstraints::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ConstraintCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ConstraintSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ConstraintName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, FIFTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, SIXTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc ConstraintType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTableConstraints::Variation_7()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_CONSTRAINTS, SEVENTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTableConstraints::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CTablePrivileges)
//*-----------------------------------------------------------------------
//| Test Case:		CTablePrivileges - test restrictions on table privileges
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTablePrivileges::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablePrivileges::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_PRIVILEGES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablePrivileges::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_PRIVILEGES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablePrivileges::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_PRIVILEGES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Grantor
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablePrivileges::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_PRIVILEGES, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Grantee
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablePrivileges::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLE_PRIVILEGES, FIFTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTablePrivileges::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CTables)
//*-----------------------------------------------------------------------
//| Test Case:		CTables - test restricitons on table schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTables::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc TableType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc All Supported Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, (FIRST|SECOND|THIRD|FOURTH));
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc All Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, ALLRES);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Extra Restriction
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTables::Variation_7()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES, FIFTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTables::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CTablesInfo)
//*-----------------------------------------------------------------------
//| Test Case:		CTablesInfo - test restricitons on table schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTablesInfo::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc TableType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc All Supported Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, (FIRST|SECOND|THIRD|FOURTH));
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc All Restrictions
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, ALLRES);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Extra Restriction
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTablesInfo::Variation_7()
{
	return TestSchemaRestrictions(DBSCHEMA_TABLES_INFO, FIFTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTablesInfo::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CTranslations)
//*-----------------------------------------------------------------------
//| Test Case:		CTranslations - test restrictions on translation schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTranslations::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TranslationCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTranslations::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_TRANSLATIONS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TranslationSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTranslations::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_TRANSLATIONS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TranslationName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CTranslations::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_TRANSLATIONS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CTranslations::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CUsagePrivileges)
//*-----------------------------------------------------------------------
//| Test Case:		CUsagePrivileges - test restrictions on usage privileges schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CUsagePrivileges::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ObjectCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ObjectSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ObjectName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, THIRD);
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc ObjectType
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_4()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, FOURTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Grantor
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_5()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, FIFTH);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Grantee
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CUsagePrivileges::Variation_6()
{
	return TestSchemaRestrictions(DBSCHEMA_USAGE_PRIVILEGES, SIXTH);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CUsagePrivileges::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CViewColumnUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CViewColumnUsage - test restrictions on view column usage schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViewColumnUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ViewCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewColumnUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_COLUMN_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ViewSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewColumnUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_COLUMN_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ViewName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewColumnUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_COLUMN_USAGE, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViewColumnUsage::Terminate()
{
	
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CViewTableUsage)
//*-----------------------------------------------------------------------
//| Test Case:		CViewTableUsage - test restrictions on view table usage
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViewTableUsage::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ViewCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewTableUsage::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_TABLE_USAGE, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ViewSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewTableUsage::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_TABLE_USAGE, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ViewName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViewTableUsage::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEW_TABLE_USAGE, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViewTableUsage::Terminate()
{
	
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CViews)
//*-----------------------------------------------------------------------
//| Test Case:		CViews - test restrictions on view schema
//|	Created:			09/23/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViews::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc TableCatalog
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViews::Variation_1()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEWS, FIRST);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc TableSchema
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViews::Variation_2()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEWS, SECOND);
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc TableName
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CViews::Variation_3()
{
	return TestSchemaRestrictions(DBSCHEMA_VIEWS, THIRD);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CViews::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CZombie)
//*-----------------------------------------------------------------------
//| Test Case:		CZombie - testing IDBSchemaRowset in zombie situation
//|	Created:			09/24/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CZombie::Init()
{
	// Check to see if Transactions are usable
	if(!IsUsableInterface(SESSION_INTERFACE, IID_ITransactionLocal))
		return TEST_SKIPPED;

	// Initialize to a invalid pointer
	m_pITransactionLocal = INVALID(ITransactionLocal*);
	
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CTransaction::Init())
	// }}
	{
		// Register Interface with Zombie
		if(RegisterInterface(SESSION_INTERFACE, IID_IDBSchemaRowset,0,NULL))
		{
			// This is a total hack for Sql Server.  They cannot create an additional connection
			// within a transaction, and the privlib transaction helper functions create a rowset,
			// leave it active, and then when GetRowset is called to create an additional rowset
			// we need to have one more available command object.  When not using an ini file this
			// is done automatically the create the CTable, but if using an ini file we need to
			// create the second command here.
			if (!m_pIDBCreateCommand || m_pCTable->m_pICommand ||
				(m_pIDBCreateCommand &&
				CHECK(m_pIDBCreateCommand->CreateCommand(NULL,IID_ICommand,
					(IUnknown**)&m_pCTable->m_pICommand),S_OK)))
				return TRUE;
		}
	}

	// Check to see if ITransaction is supported
    if(!m_pITransactionLocal)
		return TEST_SKIPPED;

    // Clear the bad pointer value
	if(m_pITransactionLocal == INVALID(ITransactionLocal*))
		m_pITransactionLocal = NULL;


	return FALSE;
}

//*-----------------------------------------------------------------------
// @mfunc TestTxn
// Tests commit/abort with respect to IAccessor on commands
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::TestTxnGetRowset
(						 
	ETXN eTxn,
	BOOL fRetaining,
	GUID schema
)
{
	
	BOOL				fSuccess = FALSE;
	ULONG				index=0;
	IDBSchemaRowset *	pIDBSchemaRowset=NULL;
	IRowset *			pIRowset=NULL;

	if (!StartTransaction(SELECT_ALLFROMTBL, (IUnknown **)&pIDBSchemaRowset, 
						0, NULL, NULL, ISOLATIONLEVEL_READUNCOMMITTED))
		goto CLEANUP;

	if (eTxn == ETXN_COMMIT)
	{
		//Commit the transaction, with retention as specified
		if(!GetCommit(fRetaining))
			goto CLEANUP;
	}
	else
	{
		//Abort the transaction, with retention as specified
		if(!GetAbort(fRetaining))
			goto CLEANUP;
	}

	//Make sure everything still works after commit or abort
 	fSuccess = CHECK(pIDBSchemaRowset->GetRowset(
		NULL,
		schema,
		0,
		NULL,
		IID_IRowset,
		0,
		NULL,
		PPI &pIRowset),S_OK);

		
CLEANUP:

	if(pIDBSchemaRowset)
		pIDBSchemaRowset->Release();

	if(pIRowset)
		pIRowset->Release();

	//Return code of Commit/Abort will vary depending on whether
	//or not we have an open txn, so adjust accordingly
	if (fRetaining)
		CleanUpTransaction(S_OK);
	else
		CleanUpTransaction(XACT_E_NOTRANSACTION);
	
	if (fSuccess)
		return TEST_PASS;
	else
		return TEST_FAIL;

}

int CZombie::TestTxnGetSchemas
(						 
	ETXN eTxn,
	BOOL fRetaining
)
{
	
	BOOL				fSuccess = FALSE;
	ULONG				index=0;
	IDBSchemaRowset *	pIDBSchemaRowset=NULL;

	ULONG				cTxnSchemas=0;
	GUID *				rgTxnSchemas=NULL;
	ULONG *				rgTxnRestrictions=NULL;

	if (!StartTransaction(SELECT_ALLFROMTBL, (IUnknown **)&pIDBSchemaRowset, 
						0, NULL, NULL, ISOLATIONLEVEL_READUNCOMMITTED))
		goto CLEANUP;

	if (eTxn == ETXN_COMMIT)
	{
		//Commit the transaction, with retention as specified
		if(!GetCommit(fRetaining))
			goto CLEANUP;
	}
	else
	{
		//Abort the transaction, with retention as specified
		if(!GetAbort(fRetaining))
			goto CLEANUP;
	}

	//Make sure everything still works after commit or abort
 	fSuccess = CHECK(pIDBSchemaRowset->GetSchemas(
		&cTxnSchemas,&rgTxnSchemas,&rgTxnRestrictions),S_OK);

		
CLEANUP:

	// Free the memory
	PROVIDER_FREE(rgTxnSchemas);
	PROVIDER_FREE(rgTxnRestrictions);

	if(pIDBSchemaRowset)
		pIDBSchemaRowset->Release();


	//Return code of Commit/Abort will vary depending on whether
	//or not we have an open txn, so adjust accordingly
	if (fRetaining)
		CleanUpTransaction(S_OK);
	else
		CleanUpTransaction(XACT_E_NOTRANSACTION);
	
	if (fSuccess)
		return TEST_PASS;
	else
		return TEST_FAIL;

}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc GetRowset: Commit with fRetaining set to TRUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_1()
{
	for(ULONG index=0;index<cSchemas;index++)
		TestTxnGetRowset(ETXN_ABORT, TRUE,rgSchemas[index]);

	return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc GetRowset: Commit with fRetaining set to FALSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_2()
{
	for(ULONG index=0;index<cSchemas;index++)
	{
		TestTxnGetRowset(ETXN_ABORT, FALSE,rgSchemas[index]);
	}
	return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc GetRowset: Abort with fRetaining set to TRUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_3()
{
	for(ULONG index=0;index<cSchemas;index++)
		TestTxnGetRowset(ETXN_COMMIT, TRUE, rgSchemas[index]);

	return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc GetRowset: Abort with fRetaining set to FALSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_4()
{
	for(ULONG index=0;index<cSchemas;index++)
	{
		TestTxnGetRowset(ETXN_COMMIT, FALSE, rgSchemas[index]);
	}
	return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc GetSchemas: Commit with fRetaining set to TRUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_5()
{
	return TestTxnGetSchemas (ETXN_COMMIT,TRUE);
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc GetSchemas: Commit with fRetaining set to FALSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_6()
{
	return TestTxnGetSchemas (ETXN_COMMIT,FALSE);
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc GetSchemas: Abort with fRetaining set to TRUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_7()
{
	return TestTxnGetSchemas (ETXN_ABORT,TRUE);
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc GetSchemas: Abort with fRetaining set to FALSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int CZombie::Variation_8()
{
	return TestTxnGetSchemas (ETXN_ABORT,TRUE);
}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL CZombie::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CTransaction::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(ExtendedErrors)
//*-----------------------------------------------------------------------
//| Test Case:		ExtendedErrors - testing extended errors on IDBSchemaRowset
//|	Created:			09/24/96
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL ExtendedErrors::Init()
{
	//Create an object for checking extended errors, which will use
	//m_pError to increment the error count as needed.
	m_pExtError = new CExtError(m_pThisTestModule->m_ProviderClsid, m_pError);
	
	if (!m_pExtError)
		return FALSE;
	
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Valid GetSchemas Call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_1()
{
	INIT

	GUID *		rgSchemas=NULL;
	ULONG		cSchemas=0;
	ULONG *		rgRestrictionSupport=NULL;

	//For each method of the interface, first create an error object on
	//the current thread, try get a success from the IDBSchemaRowset method.
	//We then check extended errors to verify the right extended error behavior.

	m_fResult = FALSE;

	//create an error object
	m_pExtError->CauseError();
	
	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,&rgSchemas,&rgRestrictionSupport),S_OK))
 		//Do extended check following GetSchemas
		m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);

	// Free the memory
	PROVIDER_FREE(rgSchemas);
	PROVIDER_FREE(rgRestrictionSupport);

	FREE
		
	if(m_fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Valid GetRowset call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_2()
{
	BOOL fResult= TRUE; 
	
	//For each method of the interface, first create an error object on
	//the current thread, try get a success from the IDBSchemaRowset method.
	//We then check extended errors to verify the right extended error behavior.

	// for each valid schema, test method
	for(ULONG ulIndex=0;ulIndex<m_cSchemasSupported;ulIndex++)
	{
		INIT

		m_iid = IID_IRowset;
		m_restrict = ZERO;

		// don't need restrictions
		m_fCountRestrictionsNULL = TRUE;
		m_fRangeRestrictionsNULL = TRUE;

		// don't need properties
		m_fCountPropSetNULL = TRUE;
		m_fRangePropSetNULL = TRUE;

		// check to make sure it's a schema I want and 
		// get its info
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			if(!m_fCaptureRestrictions)
				TraceSchemaName(m_guid, FALSE,TRUE);

			//create an error object
			m_pExtError->CauseError();
	

			if(fResult &= CHECK(m_HR=m_pIDBSchemaRowset->GetRowset(
								m_punkOuter,
								m_guid,
								0,
								NULL,
								m_iid,
								0,
								0,
								 (IUnknown **) &m_pIRowset), S_OK))
			{
 				//Do extended check following GetRowset
				fResult &= XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);
				fResult &= CheckResults(m_pIRowset,m_iid);
			}
		
		}
		FREE
	}

	if(fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Valid GetSchemas call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_3()
{
	INIT

	GUID *		rgSchemas=NULL;
	ULONG		cSchemas=0;
	ULONG *		rgRestrictionSupport=NULL;

	//For each method of the interface, first create an error object on
	//the current thread, try get a failure from the IDBSchemaRowset method.
	//We then check extended errors to verify the right extended error behavior.

	m_fResult = FALSE;

	//create an error object
	m_pExtError->CauseError();
	
	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(NULL,&rgSchemas,&rgRestrictionSupport),E_INVALIDARG))
 		//Do extended check following GetSchemas
		m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);

	if((!COMPARE(rgRestrictionSupport,NULL)) ||	
		(!COMPARE(rgSchemas,NULL)))
		m_fResult &= FALSE;

	FREE
		
	if(m_fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Invalid GetRowset call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_4()
{
	//first create an error object on the current thread, try get a failure from
	//the IDBSchemaRowset method. We then check extended errors to verify the 
	//right extended error behavior.

	INIT
	
	m_fResult = FALSE;

	m_iid = IID_IRowset;
	m_guid = DBPROPSET_ROWSET;

	// no restrictions
	m_fCountRestrictionsNULL = TRUE;
	m_fRangeRestrictionsNULL = TRUE;

	// no properties
	m_fCountPropSetNULL = TRUE;
	m_fRangePropSetNULL = TRUE;

	//create an error object
	m_pExtError->CauseError();
	
	//invalid m_guid
	if(CHECK(m_HR=m_pIDBSchemaRowset->GetRowset(
								m_punkOuter,
								m_guid,
								0,
								NULL,
								m_iid,
								0,
								0,
	  						    (IUnknown **) &m_pIRowset), E_INVALIDARG))

 	//Do extended check following GetRowset
	m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);
				
	FREE
	if(m_fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Invalid GetSchemas call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_5()
{
	INIT

	GUID *		rgSchemas=NULL;
	ULONG		cSchemas=0;
	ULONG *		rgRestrictionSupport=NULL;

	//For each method of the interface, with no error object on
	//the current thread, try get a failure from the IDBSchemaRowset method.
	//We then check extended errors to verify the right extended error behavior.

	m_fResult = FALSE;

	// test method
	if(CHECK(m_HR = m_pIDBSchemaRowset->GetSchemas(&cSchemas,NULL,&rgRestrictionSupport),E_INVALIDARG))
 		//Do extended check following GetSchemas
		m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);

	if((!COMPARE(cSchemas,0)) ||	
		(!COMPARE(rgRestrictionSupport,NULL)))
		m_fResult &= FALSE;

	FREE
		
	if(m_fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Invalid GetRowset call with previous error object existing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_6()
{
	ULONG cErrors=0;

	//For each method of the interface, with no error object on
	//the current thread, try get a failure from the IDBSchemaRowset method.
	//We then check extended errors to verify the right extended error behavior.

	for(ULONG ulIndex = 0; ulIndex < m_cSchemasSupported; ulIndex++)
	{
		INIT

		m_iid=IID_IDBProperties; 
		m_restrict = ZERO;

		// don't pass restrictions
		m_fCountRestrictionsNULL = TRUE;
		m_fRangeRestrictionsNULL = TRUE;

		// don't pass restrictions
		m_fCountRestrictionsNULL = TRUE;
		m_fRangeRestrictionsNULL = TRUE;

		// if this is a schema I want then get information for schema
		if(GetSchemaInfo(SUPPORTED,ulIndex))
		{
			PRVTRACE(L"Before call\n");
			//invalid m_iid
			CHECK(m_HR=m_pIDBSchemaRowset->GetRowset(
						m_punkOuter,
						m_guid,
						0,
						0,
						m_iid,
						0,
						0,
	  					(IUnknown **) &m_pIRowset), E_NOINTERFACE);
 			//Do extended check following GetRowset
			PRVTRACE(L"After call\n");
			m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);
			if(!m_fResult)
				cErrors++;
	
		}
		// free rowset
		FREE
	}

	if(cErrors)
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Open schema rowset  DBSCHEMA_ASSERTIONS -- E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int ExtendedErrors::Variation_7()
{

	INIT
	
	m_fResult = FALSE;

	m_iid = IID_IRowset;

	//create an error object
	m_pExtError->CauseError();
	
	//invalid m_guid
	if(CHECK(m_HR=m_pIDBSchemaRowset->GetRowset(
								NULL,
								DBSCHEMA_ASSERTIONS,
								0,
								NULL,
								m_iid,
								0,
								0,
	  						    (IUnknown **) &m_pIRowset), E_INVALIDARG))

 	//Do extended check following GetRowset
	m_fResult = XCHECK(m_pIDBSchemaRowset, IID_IDBSchemaRowset, m_HR);
				
	FREE
	if(m_fResult)
		return TEST_PASS;
	else
		return TEST_FAIL;


}
// }}


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL ExtendedErrors::Terminate()
{
	//free error object
	if (m_pExtError)
		delete m_pExtError;
	m_pExtError = NULL;

	// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(CBugRegressions)
//*-----------------------------------------------------------------------
//| Test Case:		CBugRegressions - Test case for bug regressions
//| Created:  	7/2/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL CBugRegressions::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(CSchemaTest::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 





// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc GetRowset on schema TABLES after executing RETURN statement
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int CBugRegressions::Variation_1()
{ 
	// To repro this bug we must execute a statement that just contains "RETURN".
	// We can just hard-code this since it's specific to this bug.
	LPWSTR pwszReturn = L"RETURN";
	IRowset * pIRowset = NULL;
	BOOL fResult = TEST_FAIL;

	// Must have commands
	if(!(m_pTable->m_pICommand))
		return TEST_SKIPPED;

	// Execute the RETURN statement.  This may not be supported, so don't check return
	// code.
	m_pTable->BuildCommand(pwszReturn,		// SQL STMT
			IID_IRowset, EXECUTE_ALWAYS, 0, NULL, NULL,	NULL, (IUnknown **)&pIRowset);

	// Shouldn't return a rowset
	TESTC(pIRowset == NULL);

	// Now execute a valid statement
	TESTC_(m_pTable->ExecuteCommand(SELECT_ORDERBYNUMERIC, IID_IRowset, NULL,
		NULL, NULL, NULL, EXECUTE_IFNOERROR, 0, NULL, NULL, (IUnknown**)&pIRowset), S_OK);

	TESTC(pIRowset != NULL);

	// Now get schema tables with table name restriction	
	TESTC(TestSchemaRestrictions(DBSCHEMA_TABLES, THIRD));

	fResult = TEST_PASS;

CLEANUP:

	SAFE_RELEASE(pIRowset);

	return fResult;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL CBugRegressions::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(CSchemaTest::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END

