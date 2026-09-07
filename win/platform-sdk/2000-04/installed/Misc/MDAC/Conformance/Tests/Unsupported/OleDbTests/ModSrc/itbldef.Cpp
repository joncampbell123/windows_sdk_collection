//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1999 Microsoft Corporation.
//
// @doc 
//
// @module ITblDef.CPP | Source for ITableDefinition test module
//

#include "modstandard.hpp"
#include "itbldef.h"
#include "Extralib.h"
#include <process.h>
#include "clib.hpp"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Module Values
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// {{ TCW_MODULE_GLOBALS
DECLARE_MODULE_CLSID = { 0xae3d5c1e, 0x2a0b, 0x11d1, { 0xaf, 0x9d, 0x00, 0xc0, 0x4f, 0xc2, 0x27, 0x93 }};
DECLARE_MODULE_NAME("ITableDefinition");
DECLARE_MODULE_OWNER("Microsoft");
DECLARE_MODULE_DESCRIP("The module tests the ITableDefinition interface");
DECLARE_MODULE_VERSION(795921705);
// TCW_WizardVersion(2)
// TCW_Automation(True)
// }} TCW_MODULE_GLOBALS_END
GUID	guidModule	= { 0xae3d5c1e, 0x2a0b, 0x11d1, { 0xaf, 0x9d, 0x00, 0xc0, 0x4f, 0xc2, 0x27, 0x93 }};

// macros
#define FILL_PROP_SET(RGPROPSET_EL, NPROP, RGPROP, PROP_GUID)		\
	RGPROPSET_EL.cProperties		= NPROP;						\
	RGPROPSET_EL.rgProperties		= RGPROP;						\
	RGPROPSET_EL.guidPropertySet	= PROP_GUID;					\
	if (NULL != RGPROP)												\
		memset(RGPROP, 0, NPROP*sizeof(DBPROP));

#define FILL_PROP(RGPROP_EL, PROPID, VAR_TYPE, VAR_MACRO, VAR_VALUE, OPTION)		\
	memset(&RGPROP_EL, 0, sizeof(DBPROP));											\
	RGPROP_EL.dwPropertyID			= PROPID;										\
	RGPROP_EL.vValue.vt				= VAR_TYPE;										\
	VAR_MACRO(&RGPROP_EL.vValue)	= VAR_VALUE;									\
	RGPROP_EL.dwOptions				= OPTION;

#define INVALID_DBKIND	1001

class TCITableDefinition;
// define argument stru for threading function parameters
typedef struct inparam{
	ULONG				i;
	TCITableDefinition	*pObject;
} CInParam;

// the number of threads that will be created in the multithreading test variations
const int nThreads = 7; // must be less than 100 and greater than 1
// the dispatcher thread function
unsigned WINAPI ThreadProc(LPVOID lpvThreadParam);

ULONG	nResidualTables;

//--------------------------------------------------------------------
// @func Module level initialization routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
BOOL ModuleInit(CThisTestModule * pThisTestModule)
{
	ITableDefinition	*pITableDefinition = NULL;
	BOOL				fInitRes = TRUE;
	HRESULT				hr;

	nResidualTables = 0;

	//Create the session
	if(!ModuleCreateDBSession(pThisTestModule))
		return FALSE;

	g_pIOpenRowset = (IOpenRowset*)pThisTestModule->m_pIUnknown2;
	CConsDesc::s_pSessionIUnknown	= g_pIOpenRowset;
	CConsDesc::s_pConstraints		= new CConstraints(g_pIOpenRowset);

	if (!(hr=VerifyInterface(pThisTestModule->m_pIUnknown2, IID_ITableDefinition, 
				SESSION_INTERFACE, (IUnknown**)&pITableDefinition)))
	{
		odtLog << "ITableDefinition not supported!\n";
		fInitRes = TEST_SKIPPED;;
	}

	if (!(hr=VerifyInterface(pThisTestModule->m_pIUnknown, IID_IDBInitialize, 
				DATASOURCE_INTERFACE, (IUnknown**)&g_pIDBInitialize)))
	{
		odtLog << "IDBInitialize not supported!\n";
		fInitRes = TEST_SKIPPED;
	}

	SAFE_RELEASE(pITableDefinition);
	return fInitRes;
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
	delete CConsDesc::s_pConstraints;
	if (0 < nResidualTables)
		odtLog << "Total number of residual tables: " << nResidualTables << "\n";
	SAFE_RELEASE(g_pIDBInitialize);
	return ModuleReleaseDBSession(pThisTestModule);
}	


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Case Section
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


//--------------------------------------------------------------------
// @class Base Class
//
class TCITableDefinition : public CSessionObject {
protected:
	// add critical section
	CRITICAL_SECTION	m_CriticalSection; 

	// used to check for residual tables
	ULONG				m_nTables;	

	// @cmember pointer to ITableDefinition interface
	ITableDefinition	*m_pITableDefinition;
	ITableDefinitionWithConstraints	*m_pITDWC;
	BOOL				m_fIsIDBInfo;
	// @cmember list of CCol storing info about provider data types
	CList <CCol, CCol&>	m_ColList;

	// @cmember data about table and column name
	ULONG				m_cMaxTableName, m_cMaxColName;
	LPOLESTR			m_pwszInvalidTableChars;
	LPOLESTR			m_pwszInvalidColChars;
	
	// @cmember array of column descriptors on the table (limited operations on it)
	DBCOLUMNDESC		*m_rgColumnDesc;
	ULONG				m_cColumnDesc;

	// @cmembe aray of result for multithreading processing
	HRESULT				m_rgResult[nThreads];
	// @cmember array of table name for multithreading
	LPWSTR				m_rgName[nThreads];

	//  @cmember array of supported match types
//	DBORDINAL			m_cMatchType;
//	DBMATCHTYPE			*m_rgMatchTypes;

	DBMATCHTYPE			m_SupportedMatchType;

	// @cmember Check the result and property status
	BOOL	CheckPropertyStatus(
		GUID		guidPropertySet,	// [in]	property set guid 
		GUID		guidExpectedPS,		// [in] expected prop set guid
		DBPROP		*pProp,				// [in]	property
		HRESULT		hr					// [in] the result
	);

	// @cmember Check the prop status of a list of col desc
	BOOL	CheckColPropStatus(
		ULONG			cColDesc,			// [in] 
		DBCOLUMNDESC	*rgColDesc,			// [in] the size of the propset array
		HRESULT			hr					// [in]	the result of the previous op
	);


	// @cmember Drops the column and checks the result
	HRESULT	DropColumnAndCheck(
		DBID				*pTableID,					// [in]		the table ID
		DBID				*pColumnID,					// [in]		the ID of the column to be dropped
		ITableDefinition	*pITableDefinition = NULL	// [in] pointer to ITableDefinition interface
	);

	// @cmember Drops the table and checks the result
	HRESULT DropTableAndCheck( 
		DBID				*pTableID,					// [in] ID of the table to be dropped
		ITableDefinition	*pITableDefinition = NULL	// [in] pointer to ITableDefinition interface
	);

	// @cmember Gets literal info about tables and columns
	HRESULT	GetLiteralInfo();

	// @cmember Gets the catalog and schema name for a given table
	BOOL	GetCatalogSchemaNames(
		LPWSTR	pwszTableName,		// [in] the name of the table
		LPWSTR	*ppwszCatalogName,	// [out] catalog name
		LPWSTR	*ppwszSchemaName,	// [out] schema name
		CTable	*pTable	= NULL		// [in]		table object (DEFAULT NULL - use m_pTable)
		);

	// @cmember Check whether the array of ColumnDesc sent was preserved
	// all the data should be inside, the only thing that is allowed to be modified <nl>
	// is the property status
	BOOL	IsColumnDescPreserved
	(
		DBCOLUMNDESC	*rgInColumnDesc,	// the array passed to ITableDefinition
		DBCOLUMNDESC	*rgOutColumnDesc,	// the aray returned by ITableDefinition
		ULONG			cColumnDesc			// the size of the arrays
	);

	// @cmember Build a valid table name of a certain length
	WCHAR*	BuildValidName(ULONG, WCHAR*);

	// @cmember Build an invalid table name of a certain length
	WCHAR*	BuildInvalidName(ULONG, WCHAR*, WCHAR*);

	// @cmember Uses m_pTable to get a column description mentioning all the provider types
	void	GetProviderTypes(void);

	// @cmember release the property of all columns
	void	ReleaseAllColumnPropSets(
		DBCOLUMNDESC	*rgColumnDesc=NULL,	// [in]	aray to be release
		ULONG			cColumnDesc=0		// [in] no of elementrs in the array
	);

	// @cmember Passes a type name and retrieves the column from m_ColList with info about that type
	CCol	GetType(WCHAR *pwszTypeName, DBTYPE wType, BOOL *fExists=NULL);

	// @cmember Sets all the columns to maximum size as got from m_ColList (from PROVIDER_TYPES Rowset)
	BOOL	SetMaximumColSize(DBCOLUMNDESC*, ULONG);

	// @cmember Sets all the column sizes to 0 
	BOOL	SetZeroColSize(DBCOLUMNDESC*, ULONG);

	// @cmember Set a property in the property sets of a DBCOLUMNDESC
	BOOL	SetProperty(
				DBPROPSET		**prgPropertySets,						// [in/out]		array of property sets
				ULONG			*pcPropertySets,						// [in/out]		number of property sets
				DBPROPID		propID,									// [in]			property ID
				DBTYPE			wType,									// [in]			value type
				LPVOID			value,									// [in]			property value
				DBPROPOPTIONS	dwOption = DBPROPOPTIONS_OPTIONAL,		// [in]			prop options
				DBID			colid = DB_NULLID,						// [in]			col id
				GUID			guidPropertySet = DBPROPSET_COLUMN,		// [in]			the GUID of the propset
				ULONG			*pcPropSet = NULL,						// [out]		index of property set 
				ULONG			*pcProp =NULL							// [out]		index in the rgProperties array
	);

	// @cmember Set a default property in the property sets of a DBCOLUMNDESC
	BOOL	SetDefaultProperty(
				DBPROPSET		**prgPropertySets,						// [in/out]		array of property sets
				ULONG			*pcPropertySets,						// [in/out]		number of property sets
				VARIANT			*pvalue,								// [in]			property value
				DBPROPOPTIONS	dwOption = DBPROPOPTIONS_OPTIONAL,		// [in]			prop options
				DBID			colid = DB_NULLID,						// [in]			col id
				GUID			guidPropertySet = DBPROPSET_COLUMN,		// [in]			the GUID of the propset
				ULONG			*pcPropSet = NULL,						// [out]		index of property set 
				ULONG			*pcProp =NULL							// [out]		index in the rgProperties array
	);

	// @cmember Compare the list of old columns against the list of new columns and the new column
	BOOL	CheckColInfo(DBCOLUMNDESC*, ULONG, DBCOLUMNDESC*, ULONG, DBCOLUMNDESC*);

	// @cmember Compare the list of old columns against the list of new columns 
	BOOL	AreColEqual(DBCOLUMNDESC*, ULONG, DBCOLUMNDESC*, ULONG);

	// @cmember CheckReturnedDBID about the columns in the table
	//	pIndexID		-	[IN] the DBID asked for
	//	ppIndexID		-	[IN] the DBID created
	BOOL	CheckReturnedDBID(
				DBID	*pIndexID,		// passed to CreateIndex
				DBID	**ppIndexID		// returned by CreateIndex
	);

	// @member Add a column using ITableDefinition::AddColumn and check its addition
	HRESULT	AddColumnAndCheck(
			DBID				*pTableID,					// [IN]  table ID
			DBCOLUMNDESC		*pColumnDesc,				// [IN]  descriptor of the column to be added
			DBID				**ppColumnID,				// [OUT] column ID for the resulted column
			ITableDefinition	*pITableDefinition = NULL	// [IN]  pointer to ITableDefinition 
	);

	// @member Create the table using ITableDefinition::CreateTable and check its creation
	virtual HRESULT	CreateAndCheckTable(
		ITableDefinition	*pITableDefinition,	// [in] pointer to the ITableDefinition interface to be used
		IUnknown			*pUnkOuter,			// [in] pointer to controlling unknown
		DBID				*pTableID,			// [in]	pointer to the input table ID
		ULONG				cColumnDescs,		// [in]	the number of columns to be created
		DBCOLUMNDESC		*rgColumnDescs,		// [in/out] array of columns to be created
		REFIID				riid,				// [in] rowset interface to be returned
		ULONG				cPropertySets,		// [in] number of roset property sets asked
		DBPROPSET			*rgPropertySets,	// [in/out] array of rowset property sets
		DBID				**ppTableID,		// [out] returned table ID
		IUnknown			**ppRowset			// [out] pointer to outer interface
	);

	// @member Create the table using ITableDefinitionWithConstraints::CreateTableWithConstraints and check its creation
	HRESULT			CreateAndCheckTableWithConstraints(
		ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
		IUnknown							*pUnkOuter,			// [in] pointer to controlling unknown
		DBID								*pTableID,			// [in]	pointer to the input table ID
		ULONG								cColumnDescs,		// [in]	the number of columns to be created
		DBCOLUMNDESC						*rgColumnDescs,		// [in/out] array of columns to be created
		ULONG								cConstraintDescs,	// [in]	the number of constraints to be created
		DBCONSTRAINTDESC					*rgConstraintDescs,	// [in/out] array of constraints
		REFIID								riid,				// [in] rowset interface to be returned
		ULONG								cPropertySets,		// [in] number of roset property sets asked
		DBPROPSET							*rgPropertySets,	// [in/out] array of rowset property sets
		DBID								**ppTableID,		// [out] returned table ID
		IUnknown							**ppRowset			// [out] pointer to outer interface
	);

	// @member Check properties on rowseet and table
	BOOL	CheckRwstTblPropStatus(
		ULONG			cPropSet,			// [in] the size of the propset array
		DBPROPSET		*rgPropSet,			// [in] propset array
		HRESULT			hr					// [in]	the result of the previous op
	);

	// @member Create the table using ITableDefinition::CreateTable and check its creation
	// then drops the table
	HRESULT	CCNDropTable(
		DBID			*pTableID,			// [in]	pointer to the input table ID
		ULONG			cColumnDescs,		// [in]	the number of columns to be created
		DBCOLUMNDESC	*rgColumnDesc,		// [in/out] array of columns to be created
		REFIID			riid,				// [in] rowset interface to be returned
		ULONG			cPropertySets,		// [in] number of roset property sets asked
		DBPROPSET		*rgPropertySets,	// [in/out] array of rowset property sets
		BOOL			fNullPPTableID = FALSE	// [in] if TRUE ppTableID will be NULL
	);
						
	// @cmember Check whether the columns were properly created
	// The columns of the m_pTable are checked against the column descriptor array <nl>
	// returned by ITableDefinition::CreateTable method. 
	BOOL	AreColumnsCreatedOK(
		DBCOLUMNDESC	*rgOutColumnDesc,	// [in] the aray returned by ITableDefinition::CreateTable
		ULONG			cColumnDesc,		// [in] the size of the arrays
		DBID			*pTableID = NULL,	// [in] Table ID of the table that is checked
		IUnknown		*pIUnknown = NULL	// [in] pointer to a rowset interface on the table
	);
						

	// @cmember Check whether the property sets asked for rowset was preserved
	// all the data should be inside, the only thing that is allowed to be modified <nl>
	// is the property status
	BOOL	IsRowsetPropSetPreserved
	(
		DBPROPSET	*rgInPropertySets,	// the array passed to ITableDefinition::CreateTable
		DBPROPSET	*rgOutPropertysets,	// the aray returned by ITableDefinition::CreateTable
		ULONG		cPropertySets			// the size of the arrays
	);
						

	// @cmember Check whether the rowset properties were properly created
	// The rowset properties of the m_pTable are checked against the property set<nl>
	// returned by ITableDefinition::CreateTable method. 
	BOOL	IsRowsetPropSetCreatedOK
	(
		DBPROPSET	*rgOutPropertySets,		// the aray returned by ITableDefinition::CreateTable
		ULONG		cPropertySets,			// the size of the arrays
		IUnknown	*pIUnknown				// pointer to rowset interface
	);
				
	
	// @cmember Creates a table with a column of each provider type and returns a rowset
	// to the new created table. The rowset is created with the properties required and it
	// is tested
	BOOL	CreateWithRowsetProps
	(
		ULONG			nProperties,			// number of the properties to be set
		DBPROPID		*rgPropID,				// array with the propIDs
		DBTYPE			*rgDBType,				// type of property values			
		LPVOID			*rgValue,				// values to be set
		DBPROPOPTIONS	*rgOptions,				// options of properties
		struct _GUID	*pRIID					// interface asked on rowset
	);


	// @cmember Creates a table with a column of each provider type and returns a rowset
	// to the new created table. The rowset is created with the property required and it
	// is tested
	int	CreateWithOneRowsetProps
	(
		DBPROPID		PropID,				// Property ID
		DBTYPE			DBType,				// type of property value
		LPVOID			Value,				// value to be set
		struct _GUID	*pRIID				// interface asked on rowset
	);

	// @member Add a constraint to a table using ITableDefinitionWithConstraints::AddConstraint and check its creation
	HRESULT	AddConstraintAndCheck(
		ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
		DBID								*pTableID,							// [in]	pointer to the input table ID
		DBCONSTRAINTDESC					*pConstraintDescs					// [in] new constraint
	);

	// @member Drop a constraint to a table using ITableDefinitionWithConstraints::DropConstraint and check its deletion
	HRESULT	DropConstraintAndCheck(
		ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
		DBID								*pTableID,							// [in]	pointer to the input table ID
		DBID								*pConstraintID						// [in] pointer to the DBID of the constraint
	);

	// creates a table with not nullable column
	HRESULT	CreateTableWithNoNullableColumns(	
		CTable		*pTable,					// @param [IN] pointer to CTable 
		DBCOUNTITEM ulRowCount,					// @parm [IN] # of Rows to insert into table, 1 based
		DBORDINAL	ulIndex = 0,				// @parm [IN] Column Number of index, 1 based (Default=1)
		WCHAR		*pwszTableName = NULL,		// @parm [IN] TableName, if NULL call MakeTableName() (Default=NULL)
		EVALUE		eValue = PRIMARY,			// @parm [IN] Insert PRIMARY or SECONDARY data (Default=PRIMARY)
		BOOL		fFirstUpdateable = FALSE	// @parm [IN] TRUE means first column will be autoinc (Default=FALSE)
	);

	HRESULT		AddUniqueConstraint(
		CTable				*pTable, 
		DBORDINAL			ulCol,
		DBDEFERRABILITY		Deferrability		= 0
	);

	HRESULT		AddUniqueConstraint(
		CTable				*pTable, 
		DBORDINAL			cCol,
		DBORDINAL			*rgCol,
		DBDEFERRABILITY		Deferrability		= 0
	);

	HRESULT		AddPrimaryKeyConstraint(
		CTable		*pTable, 
		DBORDINAL	ulCol,
		DBDEFERRABILITY		Deferrability		= 0
	);

	HRESULT		AddPrimaryKeyConstraint(
		CTable		*pTable, 
		DBORDINAL	cCol,
		DBORDINAL	*rgCol,
		DBDEFERRABILITY		Deferrability		= 0
	);

	HRESULT		AddKeyConstraint(
		DBCONSTRAINTTYPE	ConstraintType,
		CTable				*pTable, 
		DBORDINAL			cCol,
		DBORDINAL			*rgCol,
		DBDEFERRABILITY		Deferrability		= 0
	);

	HRESULT		AddForeignKeyConstraint(
		CTable				*pBaseTable, 
		DBORDINAL			cBaseCol,
		DBORDINAL			*rgBaseCol,
		CTable				*pReferencedTable, 
		DBORDINAL			cReferencedCol,
		DBORDINAL			*rgReferencedCol,
		DBMATCHTYPE			MatchType			= DBMATCHTYPE_FULL,
		DBUPDELRULE			UpdateRule			= DBUPDELRULE_NOACTION,
		DBUPDELRULE			DeleteRule			= DBUPDELRULE_NOACTION,
		DBDEFERRABILITY		Deferrability		= 0
	);

public:

	// @cmember Start a local transaction using the interface pointer passed
	HRESULT	StartTransaction(ITransactionLocal *pITransactionLocal);

	// @cmember Fills in a variant with whatever it needs
	BOOL	FillVariant(VARIANT *pVariant);

	// @cmember Builds a default value for a given column type
	BOOL	GetDefaultValue(
		DBCOLUMNDESC	*pColumnDesc,	// [in]		column for which the default is build
		VARIANT			*pVariant,		// [out]	variant that contains the default value
		CTable			*pTable=NULL	// [in]		CTable to use
		);

	// @cmember CompareColumnDesc
	// check the new column (first parameter against the result of 
	BOOL	CompareColumnDesc(DBCOLUMNDESC*, DBCOLUMNDESC*);

	// @cmember Compare 2 columns got by GetColumnDescOnTable
	BOOL	AreColEqual(DBCOLUMNDESC*, DBCOLUMNDESC *);

	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	virtual unsigned MyThreadProc(ULONG i) {return i;}

	//constructor
	TCITableDefinition(WCHAR *wstrTestCaseName);

	// checks the interfaces on the object, as well as count reference and parent of the rowset
	BOOL	CheckRowset(IUnknown* pIRowset);

	// @cmember GetInfo about the columns in the table
	BOOL	GetColumnInfo(DBID*, ULONG*, DBCOLUMNINFO**, OLECHAR**);

	// @cmember Get an array od Column Descriptors on a Table
	BOOL	GetColumnDescOnTable(DBID*, DBCOLUMNDESC**, ULONG*);

	//destructor
	virtual ~TCITableDefinition(){;}

	// @cmember Get the number of tables in a data sources, based on IDBSchemaRowset
	ULONG GetNoOfTables();
	
	//---------------------------------------------------------------------------
	// TCITableDefinition::Insert
	//
	// CTable				|
	// Insert				|
	// Inserts 1 row in table using IRowsetChange.
	//
	// @mfunc	Insert
	// @rdesc HRESULT indicating success or failure
	// 	@flag S_OK		| Function ran without problem
	// 	@flag E_FAIL    | No Columns Marked for use or SetNewData failed
	//
	//---------------------------------------------------------------------------
	HRESULT Insert(			
		ULONG 			ulRowNumber,		// @parm [IN]	Row number to insert	(0 for next)		
		IUnknown		*pIUnknown,			// @parm [IN]	Pointer to rowset interface
		ULONG			cNulls = 0,			// @parm [IN]	Number of columns to be set to NULL
		ULONG			*rgNulls = NULL,	// @parm [IN]	Array of columns to be set to NULL
		CTable			*pTable = NULL		// @parm [IN]	Table (DEFAULT NULL - use m_pTable)
	);

};


// {{ TCW_TEST_CASE_MAP(TCAddColumn)
//--------------------------------------------------------------------
// @class Testcase for ITableDefinition::AddColumn
//
class TCAddColumn : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddColumn,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember Table doesn't exist => DB_E_NOTABLE
	int Variation_1();
	// @cmember The table is in use => DB_E_TABLEINUSE
	int Variation_2();
	// @cmember *pTableID = DB_NULLID => DB_E_NOTABLE
	int Variation_3();
	// @cmember pTableID = NULL => E_INVALIDARG
	int Variation_4();
	// @cmember Maximum length for table name => S_OK
	int Variation_5();
	// @cmember the table name is 1 char too long => DB_E_NOTABLE
	int Variation_6();
	// @cmember invalid table name => DB_E_NOTABLE
	int Variation_7();
	// @cmember pColumnDesc->dbcid = DB_NULLID
	int Variation_8();
	// @cmember Column name pointer is NULL => DB_E_BADCOLUMNID
	int Variation_9();
	// @cmember column name is an empty string => DB_E_BADCOLUMNID
	int Variation_10();
	// @cmember Maximum length of column name => S_OK
	int Variation_11();
	// @cmember 1-char-too-long column name => DB_E_BADCOLUMNID
	int Variation_12();
	// @cmember Add a column for each provider type, maximum column size => S_OK
	int Variation_13();
	// @cmember Add a column for each known type; ulColumnsize == 0 => S_OK
	int Variation_14();
	// @cmember Add a column for each provider type (table is populated) => S_OK
	int Variation_15();
	// @cmember Precision testing
	int Variation_16();
	// @cmember Scale testing
	int Variation_17();
	// @cmember Invalid value for the property of a column => DB_E_ERRORSOCCURRED
	int Variation_18();
	// @cmember Non-column property => DBPROPSTATUS_NOTSUPPORTED
	int Variation_19();
	// @cmember Inexistent property => DBPROPSTATUS_NOTSUPPORTED
	int Variation_20();
	// @cmember Bad option => DBPROPSTATUS_BADOPTION
	int Variation_21();
	// @cmember Conflicting property => DBPROPSTATUS_CONFLICTING
	int Variation_22();
	// @cmember Bad column ID for a property => ignored
	int Variation_23();
	// @cmember Bad column type (wType in DBCOLUMNDESC)
	int Variation_24();
	// @cmember Invalid provider type name for a column => DB_E_BADTYPE
	int Variation_25();
	// @cmember NULL ppColumnDesc => S_OK
	int Variation_26();
	// @cmember Autoincrementable column - DBPROP_COL_AUTOINCREMENT
	int Variation_27();
	// @cmember DBPROP_COL_DEFAULT
	int Variation_28();
	// @cmember DBPROP_COL_DESCRIPTION
	int Variation_29();
	// @cmember DBPROP_COL_FIXEDLENGTH
	int Variation_30();
	// @cmember DBPROP_COL_NULLABLE
	int Variation_31();
	// @cmember DBPROP_PRIMARYKEY
	int Variation_32();
	// @cmember DBPROP_COL_UNIQUE
	int Variation_33();
	// @cmember Duplicate an existing column => DB_E_DUPLCATECOLUMNID
	int Variation_34();
	// @cmember Abort retaining
	int Variation_35();
	// @cmember Abort without retaining
	int Variation_36();
	// @cmember Commit with retain
	int Variation_37();
	// @cmember Commit without retain
	int Variation_38();
	// @cmember Add a column for each provider type in an empty table => S_OK
	int Variation_39();
	// @cmember Add a column to a table created with a SQL command => S_OK
	int Variation_40();
	// @cmember Drop all the column of a table (via SQL stmt) and then try to add a column => S_OK
	int Variation_41();
	// @cmember Non NULL pTypeInfo in DBCOLUMNDESC => DB_E_BADTYPE
	int Variation_42();
	// @cmember Missmatch between wType and pwszTypeName => DB_E_BADTYPE
	int Variation_43();
	// @cmember Add a not nullable column and try to insert a null value for it
	int Variation_44();
	// @cmember Add a column on a view
	int Variation_45();
	// @cmember eKind != DBKIND_NAME for a column
	int Variation_46();
	// @cmember threads with different column
	int Variation_47();
	// @cmember threads on the same column
	int Variation_48();
	// @cmember add columns of alll known wType => S_OK
	int Variation_49();
	// @cmember Fully Qualified table name
	int Variation_50();
	// @cmember Quoted Table Name
	int Variation_51();
	// @cmember pColumnDesc == NULL => E_INVALIDARG
	int Variation_52();
	// @cmember cPropertySets != 0 and rgPropertySets == NULL in pColumnDesc => E_INVALIDARG
	int Variation_53();
	// @cmember cPropertySets == 0 and rgPropertySets != NULL in pColumnDesc => S_OK
	int Variation_54();
	// @cmember cProperties == 0 and rgProperties != NULL in pColumnDesc's propset => S_OK
	int Variation_55();
	// @cmember cProperties != 0 and rgProperties == NULL in pColumnDesc's propset => E_INVALIDARG
	int Variation_56();
	// @cmember pclsid not null
	int Variation_57();
	// @cmember NULL pwszTypeName => S_OK
	int Variation_58();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
};
// {{ TCW_TESTCASE(TCAddColumn)
#define THE_CLASS TCAddColumn
BEG_TEST_CASE(TCAddColumn, TCITableDefinition, L"Testcase for ITableDefinition::AddColumn")
	TEST_VARIATION(1, 		L"Table doesn't exist => DB_E_NOTABLE")
	TEST_VARIATION(2, 		L"The table is in use => DB_E_TABLEINUSE")
	TEST_VARIATION(3, 		L"*pTableID = DB_NULLID => DB_E_NOTABLE")
	TEST_VARIATION(4, 		L"pTableID = NULL => E_INVALIDARG")
	TEST_VARIATION(5, 		L"Maximum length for table name => S_OK")
	TEST_VARIATION(6, 		L"the table name is 1 char too long => DB_E_NOTABLE")
	TEST_VARIATION(7, 		L"invalid table name => DB_E_NOTABLE")
	TEST_VARIATION(8, 		L"pColumnDesc->dbcid = DB_NULLID")
	TEST_VARIATION(9, 		L"Column name pointer is NULL => DB_E_BADCOLUMNID")
	TEST_VARIATION(10, 		L"column name is an empty string => DB_E_BADCOLUMNID")
	TEST_VARIATION(11, 		L"Maximum length of column name => S_OK")
	TEST_VARIATION(12, 		L"1-char-too-long column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(13, 		L"Add a column for each provider type, maximum column size => S_OK")
	TEST_VARIATION(14, 		L"Add a column for each known type; ulColumnsize == 0 => S_OK")
	TEST_VARIATION(15, 		L"Add a column for each provider type (table is populated) => S_OK")
	TEST_VARIATION(16, 		L"Precision testing")
	TEST_VARIATION(17, 		L"Scale testing")
	TEST_VARIATION(18, 		L"Invalid value for the property of a column => DB_E_ERRORSOCCURRED")
	TEST_VARIATION(19, 		L"Non-column property => DBPROPSTATUS_NOTSUPPORTED")
	TEST_VARIATION(20, 		L"Inexistent property => DBPROPSTATUS_NOTSUPPORTED")
	TEST_VARIATION(21, 		L"Bad option => DBPROPSTATUS_BADOPTION")
	TEST_VARIATION(22, 		L"Conflicting property => DBPROPSTATUS_CONFLICTING")
	TEST_VARIATION(23, 		L"Bad column ID for a property => ignored")
	TEST_VARIATION(24, 		L"Bad column type (wType in DBCOLUMNDESC)")
	TEST_VARIATION(25, 		L"Invalid provider type name for a column => DB_E_BADTYPE")
	TEST_VARIATION(26, 		L"NULL ppColumnDesc => S_OK")
	TEST_VARIATION(27, 		L"Autoincrementable column - DBPROP_COL_AUTOINCREMENT")
	TEST_VARIATION(28, 		L"DBPROP_COL_DEFAULT")
	TEST_VARIATION(29, 		L"DBPROP_COL_DESCRIPTION")
	TEST_VARIATION(30, 		L"DBPROP_COL_FIXEDLENGTH")
	TEST_VARIATION(31, 		L"DBPROP_COL_NULLABLE")
	TEST_VARIATION(32, 		L"DBPROP_PRIMARYKEY")
	TEST_VARIATION(33, 		L"DBPROP_COL_UNIQUE")
	TEST_VARIATION(34, 		L"Duplicate an existing column => DB_E_DUPLCATECOLUMNID")
	TEST_VARIATION(35, 		L"Abort retaining")
	TEST_VARIATION(36, 		L"Abort without retaining")
	TEST_VARIATION(37, 		L"Commit with retain")
	TEST_VARIATION(38, 		L"Commit without retain")
	TEST_VARIATION(39, 		L"Add a column for each provider type in an empty table => S_OK")
	TEST_VARIATION(40, 		L"Add a column to a table created with a SQL command => S_OK")
	TEST_VARIATION(41, 		L"Drop all the column of a table (via SQL stmt) and then try to add a column => S_OK")
	TEST_VARIATION(42, 		L"Non NULL pTypeInfo in DBCOLUMNDESC => DB_E_BADTYPE")
	TEST_VARIATION(43, 		L"Missmatch between wType and pwszTypeName => DB_E_BADTYPE")
	TEST_VARIATION(44, 		L"Add a not nullable column and try to insert a null value for it")
	TEST_VARIATION(45, 		L"Add a column on a view")
	TEST_VARIATION(46, 		L"eKind != DBKIND_NAME for a column")
	TEST_VARIATION(47, 		L"threads with different column")
	TEST_VARIATION(48, 		L"threads on the same column")
	TEST_VARIATION(49, 		L"add columns of alll known wType => S_OK")
	TEST_VARIATION(50, 		L"Fully Qualified table name")
	TEST_VARIATION(51, 		L"Quoted Table Name")
	TEST_VARIATION(52, 		L"pColumnDesc == NULL => E_INVALIDARG")
	TEST_VARIATION(53, 		L"cPropertySets != 0 and rgPropertySets == NULL in pColumnDesc => E_INVALIDARG")
	TEST_VARIATION(54, 		L"cPropertySets == 0 and rgPropertySets != NULL in pColumnDesc => S_OK")
	TEST_VARIATION(55, 		L"cProperties == 0 and rgProperties != NULL in pColumnDesc's propset => S_OK")
	TEST_VARIATION(56, 		L"cProperties != 0 and rgProperties == NULL in pColumnDesc's propset => E_INVALIDARG")
	TEST_VARIATION(57, 		L"pclsid not null")
	TEST_VARIATION(58, 		L"NULL pwszTypeName => S_OK")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(TCDropColumn)
//--------------------------------------------------------------------
// @class Test case for dropping columns
//
class TCDropColumn : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCDropColumn,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// @cmember Check whether the 2 list of columns are identical, excepting column n
//	BOOL	IsColumnDifferention(DBCOLUMNDESC*, ULONG, DBCOLUMNDESC*, ULONG, ULONG);

	// @cmember Check whether the 2 list of columns are identical
//	BOOL	ListEqual(DBCOLUMNDESC*, ULONG, DBCOLUMNDESC*, ULONG);

//	// @cmember Drops a column and check the result against reallity
//	HRESULT	DropColumnAndCheck(DBID*, DBID*, BOOL*);
	
	// {{ TCW_TESTVARS()
	// @cmember pTableID is NULL => E_INVALIDARG
	int Variation_1();
	// @cmember pColumnID in NULL => E_INVALIDARG
	int Variation_2();
	// @cmember *pTableID is DB_NULLID => DB_E_NOTABLE
	int Variation_3();
	// @cmember *pColumnID is DB_NULLID => DB_E_BADCOLUMNID
	int Variation_4();
	// @cmember Maximum length table name => S_OK
	int Variation_5();
	// @cmember 1-char-too long table name => DB_E_NOTABLE
	int Variation_6();
	// @cmember *pTableID contains a NULL table name => DB_E_NOTABLE
	int Variation_7();
	// @cmember *pTableID contains an invalid name => DB_E_NOTABLE
	int Variation_8();
	// @cmember Maximum length for a column name => S_OK
	int Variation_9();
	// @cmember 1-char-too-long column name => DB_E_BADCOLUMNID
	int Variation_10();
	// @cmember *pColumnID contains a NULL column name pointer => E_INVALIDARG
	int Variation_11();
	// @cmember *pColumnID contains an invalid column name => E_INVALID ARG
	int Variation_12();
	// @cmember Drop all the columns of a table => S_OK
	int Variation_13();
	// @cmember Try to delete an inexistent column => DB_E_BADCOLUMNID
	int Variation_14();
	// @cmember Table in use => DB_E_TABLEINUSE
	int Variation_15();
	// @cmember Valid name of an inexistent table => DB_E_NOTABLE
	int Variation_16();
	// @cmember Abort with retaining
	int Variation_17();
	// @cmember Abort without retaining
	int Variation_18();
	// @cmember Commit with retaining
	int Variation_19();
	// @cmember Commit without retaining
	int Variation_20();
	// @cmember threads on different columns
	int Variation_21();
	// @cmember threads on the same column
	int Variation_22();
	// @cmember Drop a column twice => DB_E_BADCOLUMNID
	int Variation_23();
	// @cmember 2 TableDefinition interfaces on the same table, try to drop 2 different columns => S_OK
	int Variation_24();
	// @cmember Drop as many column as possible from a table created with a SQL statement
	int Variation_25();
	// @cmember Drop a column from a 2 column table
	int Variation_26();
	// @cmember drop a column from a 1 column table
	int Variation_27();
	// @cmember drop a column from a non empty table => S_OK
	int Variation_28();
	// @cmember drop a column from an empty table
	int Variation_29();
	// @cmember drop a nullable, an autoincrementable and a default value column
	int Variation_30();
	// @cmember try to drop a column from a view
	int Variation_31();
	// @cmember drop a column added with an alter table command => S_OK
	int Variation_32();
	// @cmember try to drop columns using different DBKIND pattern
	int Variation_33();
	// @cmember Fully Qualified Table Name
	int Variation_34();
	// @cmember Quoted Table Name
	int Variation_35();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
};
// {{ TCW_TESTCASE(TCDropColumn)
#define THE_CLASS TCDropColumn
BEG_TEST_CASE(TCDropColumn, TCITableDefinition, L"Test case for dropping columns")
	TEST_VARIATION(1, 		L"pTableID is NULL => E_INVALIDARG")
	TEST_VARIATION(2, 		L"pColumnID in NULL => E_INVALIDARG")
	TEST_VARIATION(3, 		L"*pTableID is DB_NULLID => DB_E_NOTABLE")
	TEST_VARIATION(4, 		L"*pColumnID is DB_NULLID => DB_E_BADCOLUMNID")
	TEST_VARIATION(5, 		L"Maximum length table name => S_OK")
	TEST_VARIATION(6, 		L"1-char-too long table name => DB_E_NOTABLE")
	TEST_VARIATION(7, 		L"*pTableID contains a NULL table name => DB_E_NOTABLE")
	TEST_VARIATION(8, 		L"*pTableID contains an invalid name => DB_E_NOTABLE")
	TEST_VARIATION(9, 		L"Maximum length for a column name => S_OK")
	TEST_VARIATION(10, 		L"1-char-too-long column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(11, 		L"*pColumnID contains a NULL column name pointer => E_INVALIDARG")
	TEST_VARIATION(12, 		L"*pColumnID contains an invalid column name => E_INVALID ARG")
	TEST_VARIATION(13, 		L"Drop all the columns of a table => S_OK")
	TEST_VARIATION(14, 		L"Try to delete an inexistent column => DB_E_BADCOLUMNID")
	TEST_VARIATION(15, 		L"Table in use => DB_E_TABLEINUSE")
	TEST_VARIATION(16, 		L"Valid name of an inexistent table => DB_E_NOTABLE")
	TEST_VARIATION(17, 		L"Abort with retaining")
	TEST_VARIATION(18, 		L"Abort without retaining")
	TEST_VARIATION(19, 		L"Commit with retaining")
	TEST_VARIATION(20, 		L"Commit without retaining")
	TEST_VARIATION(21, 		L"threads on different columns")
	TEST_VARIATION(22, 		L"threads on the same column")
	TEST_VARIATION(23, 		L"Drop a column twice => DB_E_BADCOLUMNID")
	TEST_VARIATION(24, 		L"2 TableDefinition interfaces on the same table, try to drop 2 different columns => S_OK")
	TEST_VARIATION(25, 		L"Drop as many column as possible from a table created with a SQL statement")
	TEST_VARIATION(26, 		L"Drop a column from a 2 column table")
	TEST_VARIATION(27, 		L"drop a column from a 1 column table")
	TEST_VARIATION(28, 		L"drop a column from a non empty table => S_OK")
	TEST_VARIATION(29, 		L"drop a column from an empty table")
	TEST_VARIATION(30, 		L"drop a nullable, an autoincrementable and a default value column")
	TEST_VARIATION(31, 		L"try to drop a column from a view")
	TEST_VARIATION(32, 		L"drop a column added with an alter table command => S_OK")
	TEST_VARIATION(33, 		L"try to drop columns using different DBKIND pattern")
	TEST_VARIATION(34, 		L"Fully Qualified Table Name")
	TEST_VARIATION(35, 		L"Quoted Table Name")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(TCDropTable)
//--------------------------------------------------------------------
// @class Test Case fo dropping a table
//
class TCDropTable : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	
public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCDropTable,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();
	
	// {{ TCW_TESTVARS()
	// @cmember pTableID is NULL => E_INVALIDARG
	int Variation_1();
	// @cmember *pTableID is DB_NULLID => DB_E_NOTABLE
	int Variation_2();
	// @cmember Maximum length for table name => S_OK
	int Variation_3();
	// @cmember 1-char-too-long table name => DB_E_NOTABLE
	int Variation_4();
	// @cmember The pointer to the table name is NULL => DB_E_NOTABLE
	int Variation_5();
	// @cmember Table name is empty => DB_E_NOTABLE
	int Variation_6();
	// @cmember Invalid table name => DB_E_NOTABLE
	int Variation_7();
	// @cmember A rowset is opened on table => DB_E_TABLEINUSE
	int Variation_8();
	// @cmember Table in use => DB_E_TABLEINUSE
	int Variation_9();
	// @cmember Inexistent table => DB_E_NOTABLE
	int Variation_10();
	// @cmember Abort with retaining
	int Variation_11();
	// @cmember Abort without retaining
	int Variation_12();
	// @cmember Commit with retaining
	int Variation_13();
	// @cmember Commit without retaining
	int Variation_14();
	// @cmember Drop a table created with an SQL command => S_OK
	int Variation_15();
	// @cmember Drop a table created with ITableDefinition, no rowset created => S_OK
	int Variation_16();
	// @cmember Drop a table created with ITableDefinition, rowset released => S_OK
	int Variation_17();
	// @cmember ITableDefinition::CreateTable, insert 10 rows, release rowset, drop the table => S_OK
	int Variation_18();
	// @cmember For each provider type, create a table with a single column and then drop it => S_OK
	int Variation_19();
	// @cmember Drop a table twice => DB_E_NOTABLE
	int Variation_20();
	// @cmember threads on different tables
	int Variation_21();
	// @cmember threads on same table
	int Variation_22();
	// @cmember Fully Qualified Table Name
	int Variation_23();
	// @cmember Quoted Table Name
	int Variation_24();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
};
// {{ TCW_TESTCASE(TCDropTable)
#define THE_CLASS TCDropTable
BEG_TEST_CASE(TCDropTable, TCITableDefinition, L"Test Case fo dropping a table")
	TEST_VARIATION(1, 		L"pTableID is NULL => E_INVALIDARG")
	TEST_VARIATION(2, 		L"*pTableID is DB_NULLID => DB_E_NOTABLE")
	TEST_VARIATION(3, 		L"Maximum length for table name => S_OK")
	TEST_VARIATION(4, 		L"1-char-too-long table name => DB_E_NOTABLE")
	TEST_VARIATION(5, 		L"The pointer to the table name is NULL => DB_E_NOTABLE")
	TEST_VARIATION(6, 		L"Table name is empty => DB_E_NOTABLE")
	TEST_VARIATION(7, 		L"Invalid table name => DB_E_NOTABLE")
	TEST_VARIATION(8, 		L"A rowset is opened on table => DB_E_TABLEINUSE")
	TEST_VARIATION(9, 		L"Table in use => DB_E_TABLEINUSE")
	TEST_VARIATION(10, 		L"Inexistent table => DB_E_NOTABLE")
	TEST_VARIATION(11, 		L"Abort with retaining")
	TEST_VARIATION(12, 		L"Abort without retaining")
	TEST_VARIATION(13, 		L"Commit with retaining")
	TEST_VARIATION(14, 		L"Commit without retaining")
	TEST_VARIATION(15, 		L"Drop a table created with an SQL command => S_OK")
	TEST_VARIATION(16, 		L"Drop a table created with ITableDefinition, no rowset created => S_OK")
	TEST_VARIATION(17, 		L"Drop a table created with ITableDefinition, rowset released => S_OK")
	TEST_VARIATION(18, 		L"ITableDefinition::CreateTable, insert 10 rows, release rowset, drop the table => S_OK")
	TEST_VARIATION(19, 		L"For each provider type, create a table with a single column and then drop it => S_OK")
	TEST_VARIATION(20, 		L"Drop a table twice => DB_E_NOTABLE")
	TEST_VARIATION(21, 		L"threads on different tables")
	TEST_VARIATION(22, 		L"threads on same table")
	TEST_VARIATION(23, 		L"Fully Qualified Table Name")
	TEST_VARIATION(24, 		L"Quoted Table Name")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(TCCreateTable)
//--------------------------------------------------------------------
// @class TestCase for table creation
//
class TCCreateTable : public TCITableDefinition { 
public:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCCreateTable,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
 
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Session interfaces
	int Variation_1();
	// @cmember pTableID is NULL => S_OK
	int Variation_2();
	// @cmember ppTableID is a NULL pointer => S_OK
	int Variation_3();
	// @cmember pTableID and ppTableID both NULL => E_INVALIDARG
	int Variation_4();
	// @cmember *pTableID == DB_NULLID => DB_E_BADTABLEID
	int Variation_5();
	// @cmember cColumnDesc == 0 => E_INVALIDARG or S_OK
	int Variation_6();
	// @cmember rgColumnDesc == NULL => E_INVALIDARG
	int Variation_7();
	// @cmember cPropertySets != 0 and rgPropertySets == NULL => E_INVALIDARG
	int Variation_8();
	// @cmember cPropertySets is 0 and rgPropertySets != NULL => S_OK
	int Variation_9();
	// @cmember for a property sets, cProperties != 0 and rgProperties == NULL => E_INVALIDARG
	int Variation_10();
	// @cmember cProperties == 0 on a property set => S_OK
	int Variation_11();
	// @cmember for a column, rgPropertySets is NULL though cPropertySets != 0 => E_INVALIDARG
	int Variation_12();
	// @cmember dbcid in an element of rgColumnDesc is DB_NULLID => DB_E_BADCOLUMNID
	int Variation_13();
	// @cmember dbcid in an element of rgColumnDesc is NULL => DB_E_BADCOLUMNID
	int Variation_14();
	// @cmember not null pclsid in a DBCOLUMNDESC element
	int Variation_15();
	// @cmember empty column name => DB_E_BADCOLUMNID
	int Variation_16();
	// @cmember empty table name => DB_E_BADTABLEID
	int Variation_17();
	// @cmember Maximum length for table name => S_OK
	int Variation_18();
	// @cmember 1 char too long table name => DB_E_BADTABLEID
	int Variation_19();
	// @cmember Maximum length column name => S_OK
	int Variation_20();
	// @cmember 1 char too long column name => DB_E_BADCOLUMNID
	int Variation_21();
	// @cmember ulColumnSize 0 for a variable-length column => S_OK
	int Variation_22();
	// @cmember For each provider type create and drop a table with a single column => S_OK
	int Variation_23();
	// @cmember invalid table name => DB_E_BADTABLEID
	int Variation_24();
	// @cmember pTableID provides the name of an existing table => DB_E_DUPLICATETABLEID
	int Variation_25();
	// @cmember valid riid and NULL ppRowset => S_OK
	int Variation_26();
	// @cmember invalid column name => DB_E_BADCOLUMNID
	int Variation_27();
	// @cmember invalid type name for a column (NULL, empty or inexistent
	int Variation_28();
	// @cmember invalid wType for a column => DB_E_BADTYPE
	int Variation_29();
	// @cmember wType and pswzTypeName mismatch in a column description => DB_E_BADTYPE
	int Variation_30();
	// @cmember not null pTypeInfo => DB_E_BADTYPE (for the time being
	int Variation_31();
	// @cmember duplicate column identifiers => DB_E_DUPLICATECOLUMNID
	int Variation_32();
	// @cmember improper eKind value in a DBID => DB_E_BADCOLUMNID
	int Variation_33();
	// @cmember invalid property value on column
	int Variation_34();
	// @cmember Invalid column precision => DB_E_BADPRECISION
	int Variation_35();
	// @cmember invalid scale on column => DB_E_BADSCALE
	int Variation_36();
	// @cmember col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_SETIFCHEAP => DB_S_ERRORSOCCURRED
	int Variation_37();
	// @cmember col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_REQUIRED => DB_E_ERRORSOCCURRED
	int Variation_38();
	// @cmember invalid propID for a column propriety
	int Variation_39();
	// @cmember colid in DBPROP of a column property is ignored
	int Variation_40();
	// @cmember Abort retaining
	int Variation_41();
	// @cmember Abort without retaining
	int Variation_42();
	// @cmember Commit retain
	int Variation_43();
	// @cmember Commit non-retaining
	int Variation_44();
	// @cmember Autoincrementable columns
	int Variation_45();
	// @cmember Default values
	int Variation_46();
	// @cmember Column descriptions
	int Variation_47();
	// @cmember Variable length columns
	int Variation_48();
	// @cmember Nullable/non nullable columns
	int Variation_49();
	// @cmember Primary key column
	int Variation_50();
	// @cmember Unique columns
	int Variation_51();
	// @cmember Test conflicting properties
	int Variation_52();
	// @cmember Create a table and ask for a rowset, setting DBPROP_ABORTPRESERVE
	int Variation_53();
	// @cmember Inserting rows after table creation, using the resulting rowset
	int Variation_54();
	// @cmember Delete rows in the created rowset
	int Variation_55();
	// @cmember Aggregation, ask for IID_IUnknown
	int Variation_56();
	// @cmember Create a table and ask for a rowset, setting DBPROP_COMMITPRESERVE
	int Variation_57();
	// @cmember Create a table and ask for a rowset, setting DBPROP_OWNINSERT
	int Variation_58();
	// @cmember Create a table and ask for a rowset, setting DBPROP_BOOKMARKS
	int Variation_59();
	// @cmember Create a table and ask for a rowset, setting DBPROP_CANHOLDROWS
	int Variation_60();
	// @cmember Create a table and ask for a rowset, setting DBPROP_MAYWRITECOLUMN
	int Variation_61();
	// @cmember Ask for a rowset mandatory/optional interface when table creation => S_OK
	int Variation_62();
	// @cmember threads on different tables
	int Variation_63();
	// @cmember threads on the same table
	int Variation_64();
	// @cmember try all interfaces on rowset that can be returned
	int Variation_65();
	// @cmember create with a table property => S_OK
	int Variation_66();
	// @cmember create a table and update a row, using the rowset interface got => S_OK
	int Variation_67();
	// @cmember Fully Qualified Table Name
	int Variation_68();
	// @cmember Quoted table name
	int Variation_69();
	// @cmember Aggregation, non IID_IUnknown
	int Variation_70();
	// @cmember Aggregation -> IRowsetInfo::GetReferencedRowset
	int Variation_71();
	// @cmember Agg session -> IRowsetInfo::GetSpecification
	int Variation_72();
	// @cmember Table -> Agg ColumnRowset, ask for non IID_IUnknown
	int Variation_73();
	// @cmember Table -> Agg ColumnRowset, ask for IID_IUnknown
	int Variation_74();
	// @cmember Table -> Agg ColumnRowset -> IRowsetInfo::GetSpecification
	int Variation_75();
	// @cmember colid is ignored in a non column specific rowset property
	int Variation_76();
	// @cmember colid is DB_NULLID in a column specific rowset property => S_OK
	int Variation_77();
	// @cmember valid, non DB_NULLID colid for a column specific rowset property
	int Variation_78();
	// @cmember invalid colid for a column specific rowset property
	int Variation_79();
	// @cmember Create a table. Retrieve a rowset on it (IRowsetInfo). Check GetSpecification()
	int Variation_80();
	// @cmember DB_NULLID is ignored for non column-related table properties
	int Variation_81();
	// @cmember Column related table property set to bad colid
	int Variation_82();
	// @cmember How many DBTYPE_I4 columns?
	int Variation_83();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
};
// {{ TCW_TESTCASE(TCCreateTable)
#define THE_CLASS TCCreateTable
BEG_TEST_CASE(TCCreateTable, TCITableDefinition, L"TestCase for table creation")
	TEST_VARIATION(1, 		L"Session interfaces")
	TEST_VARIATION(2, 		L"pTableID is NULL => S_OK")
	TEST_VARIATION(3, 		L"ppTableID is a NULL pointer => S_OK")
	TEST_VARIATION(4, 		L"pTableID and ppTableID both NULL => E_INVALIDARG")
	TEST_VARIATION(5, 		L"*pTableID == DB_NULLID => DB_E_BADTABLEID")
	TEST_VARIATION(6, 		L"cColumnDesc == 0 => E_INVALIDARG or S_OK")
	TEST_VARIATION(7, 		L"rgColumnDesc == NULL => E_INVALIDARG")
	TEST_VARIATION(8, 		L"cPropertySets != 0 and rgPropertySets == NULL => E_INVALIDARG")
	TEST_VARIATION(9, 		L"cPropertySets is 0 and rgPropertySets != NULL => S_OK")
	TEST_VARIATION(10, 		L"for a property sets, cProperties != 0 and rgProperties == NULL => E_INVALIDARG")
	TEST_VARIATION(11, 		L"cProperties == 0 on a property set => S_OK")
	TEST_VARIATION(12, 		L"for a column, rgPropertySets is NULL though cPropertySets != 0 => E_INVALIDARG")
	TEST_VARIATION(13, 		L"dbcid in an element of rgColumnDesc is DB_NULLID => DB_E_BADCOLUMNID")
	TEST_VARIATION(14, 		L"dbcid in an element of rgColumnDesc is NULL => DB_E_BADCOLUMNID")
	TEST_VARIATION(15, 		L"not null pclsid in a DBCOLUMNDESC element")
	TEST_VARIATION(16, 		L"empty column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(17, 		L"empty table name => DB_E_BADTABLEID")
	TEST_VARIATION(18, 		L"Maximum length for table name => S_OK")
	TEST_VARIATION(19, 		L"1 char too long table name => DB_E_BADTABLEID")
	TEST_VARIATION(20, 		L"Maximum length column name => S_OK")
	TEST_VARIATION(21, 		L"1 char too long column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(22, 		L"ulColumnSize 0 for a variable-length column => S_OK")
	TEST_VARIATION(23, 		L"For each provider type create and drop a table with a single column => S_OK")
	TEST_VARIATION(24, 		L"invalid table name => DB_E_BADTABLEID")
	TEST_VARIATION(25, 		L"pTableID provides the name of an existing table => DB_E_DUPLICATETABLEID")
	TEST_VARIATION(26, 		L"valid riid and NULL ppRowset => S_OK")
	TEST_VARIATION(27, 		L"invalid column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(28, 		L"invalid type name for a column (NULL, empty or inexistent")
	TEST_VARIATION(29, 		L"invalid wType for a column => DB_E_BADTYPE")
	TEST_VARIATION(30, 		L"wType and pswzTypeName mismatch in a column description => DB_E_BADTYPE")
	TEST_VARIATION(31, 		L"not null pTypeInfo => DB_E_BADTYPE (for the time being")
	TEST_VARIATION(32, 		L"duplicate column identifiers => DB_E_DUPLICATECOLUMNID")
	TEST_VARIATION(33, 		L"improper eKind value in a DBID => DB_E_BADCOLUMNID")
	TEST_VARIATION(34, 		L"invalid property value on column")
	TEST_VARIATION(35, 		L"Invalid column precision => DB_E_BADPRECISION")
	TEST_VARIATION(36, 		L"invalid scale on column => DB_E_BADSCALE")
	TEST_VARIATION(37, 		L"col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_SETIFCHEAP => DB_S_ERRORSOCCURRED")
	TEST_VARIATION(38, 		L"col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_REQUIRED => DB_E_ERRORSOCCURRED")
	TEST_VARIATION(39, 		L"invalid propID for a column propriety")
	TEST_VARIATION(40, 		L"colid in DBPROP of a column property is ignored")
	TEST_VARIATION(41, 		L"Abort retaining")
	TEST_VARIATION(42, 		L"Abort without retaining")
	TEST_VARIATION(43, 		L"Commit retain")
	TEST_VARIATION(44, 		L"Commit non-retaining")
	TEST_VARIATION(45, 		L"Autoincrementable columns")
	TEST_VARIATION(46, 		L"Default values")
	TEST_VARIATION(47, 		L"Column descriptions")
	TEST_VARIATION(48, 		L"Variable length columns")
	TEST_VARIATION(49, 		L"Nullable/non nullable columns")
	TEST_VARIATION(50, 		L"Primary key column")
	TEST_VARIATION(51, 		L"Unique columns")
	TEST_VARIATION(52, 		L"Test conflicting properties")
	TEST_VARIATION(53, 		L"Create a table and ask for a rowset, setting DBPROP_ABORTPRESERVE")
	TEST_VARIATION(54, 		L"Inserting rows after table creation, using the resulting rowset")
	TEST_VARIATION(55, 		L"Delete rows in the created rowset")
	TEST_VARIATION(56, 		L"Aggregation, ask for IID_IUnknown")
	TEST_VARIATION(57, 		L"Create a table and ask for a rowset, setting DBPROP_COMMITPRESERVE")
	TEST_VARIATION(58, 		L"Create a table and ask for a rowset, setting DBPROP_OWNINSERT")
	TEST_VARIATION(59, 		L"Create a table and ask for a rowset, setting DBPROP_BOOKMARKS")
	TEST_VARIATION(60, 		L"Create a table and ask for a rowset, setting DBPROP_CANHOLDROWS")
	TEST_VARIATION(61, 		L"Create a table and ask for a rowset, setting DBPROP_MAYWRITECOLUMN")
	TEST_VARIATION(62, 		L"Ask for a rowset mandatory/optional interface when table creation => S_OK")
	TEST_VARIATION(63, 		L"threads on different tables")
	TEST_VARIATION(64, 		L"threads on the same table")
	TEST_VARIATION(65, 		L"try all interfaces on rowset that can be returned")
	TEST_VARIATION(66, 		L"create with a table property => S_OK")
	TEST_VARIATION(67, 		L"create a table and update a row, using the rowset interface got => S_OK")
	TEST_VARIATION(68, 		L"Fully Qualified Table Name")
	TEST_VARIATION(69, 		L"Quoted table name")
	TEST_VARIATION(70, 		L"Aggregation, non IID_IUnknown")
	TEST_VARIATION(71, 		L"Aggregation -> IRowsetInfo::GetReferencedRowset")
	TEST_VARIATION(72, 		L"Agg session -> IRowsetInfo::GetSpecification")
	TEST_VARIATION(73, 		L"Table -> Agg ColumnRowset, ask for non IID_IUnknown")
	TEST_VARIATION(74, 		L"Table -> Agg ColumnRowset, ask for IID_IUnknown")
	TEST_VARIATION(75, 		L"Table -> Agg ColumnRowset -> IRowsetInfo::GetSpecification")
	TEST_VARIATION(76, 		L"colid is ignored in a non column specific rowset property")
	TEST_VARIATION(77, 		L"colid is DB_NULLID in a column specific rowset property => S_OK")
	TEST_VARIATION(78, 		L"valid, non DB_NULLID colid for a column specific rowset property")
	TEST_VARIATION(79, 		L"invalid colid for a column specific rowset property")
	TEST_VARIATION(80, 		L"Create a table. Retrieve a rowset on it (IRowsetInfo). Check GetSpecification()")
	TEST_VARIATION(81, 		L"DB_NULLID is ignored for non column-related table properties")
	TEST_VARIATION(82, 		L"Column related table property set to bad colid")
	TEST_VARIATION(83, 		L"How many DBTYPE_I4 columns?")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(RODataSource)
//--------------------------------------------------------------------
// @class Test case for operating on a read only data source
//
class RODataSource : public TCDropColumn { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	IUnknown			*m_pIUnknown;
	IUnknown			*m_pIUnknown2;
	ITableDefinition	*m_pITableDefinition2;

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(RODataSource,TCDropColumn);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Add a column
	int Variation_1();
	// @cmember Drop a column
	int Variation_2();
	// @cmember Create a table
	int Variation_3();
	// @cmember Drop a table
	int Variation_4();
	// }} TCW_TESTVARS_END
} ;

// {{ TCW_TESTCASE(RODataSource)
#define THE_CLASS RODataSource
BEG_TEST_CASE(RODataSource, TCDropColumn, L"Test case for operating on a read only data source")
	TEST_VARIATION(1, 		L"Add a column")
	TEST_VARIATION(2, 		L"Drop a column")
	TEST_VARIATION(3, 		L"Create a table")
	TEST_VARIATION(4, 		L"Drop a table")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_Boundary)
//*-----------------------------------------------------------------------
// @class General and boundary/null test for ITableDefinitionWithConstraints::AddConstraint
//
class TCAddConstraint_Boundary : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_Boundary,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Check the session object (QI for interfaces)
	int Variation_1();
	// @cmember pTableID is NULL => E_INVALIDARG
	int Variation_2();
	// @cmember *pTableID is DB_NULLID => DB_E_NOTABLE
	int Variation_3();
	// @cmember Table name is NULL => DB_E_NOTABLE
	int Variation_4();
	// @cmember Table name is the empty string => DB_E_NOTABLE
	int Variation_5();
	// @cmember Table name has maximum length
	int Variation_6();
	// @cmember Table name exceeds maximum length => DB_E_NOTABLE
	int Variation_7();
	// @cmember Table name contains illegal characters
	int Variation_8();
	// @cmember Invalid DBKIND value for the base table
	int Variation_9();
	// @cmember NULL column list in a constraint, column size is not 0 => E_INVALIDARG
	int Variation_10();
	// @cmember NULL column name in a constraint
	int Variation_11();
	// @cmember Empty string for a column name
	int Variation_12();
	// @cmember Maximum length column name in a constraint
	int Variation_13();
	// @cmember Column name exceeds maximum length
	int Variation_14();
	// @cmember Invalid DBKIND for a column DBID of a constraint
	int Variation_15();
	// @cmember Column ID is DB_NULLID
	int Variation_16();
	// @cmember NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)
	int Variation_17();
	// @cmember DBNULL_ID *pReferencedTableID for a foreign key constraint
	int Variation_18();
	// @cmember NULL referenced table name for a foreign key constraint
	int Variation_19();
	// @cmember Emptry string referenced table name for a foreign key constraint
	int Variation_20();
	// @cmember Maximum length referenced table name for a foreign key constraint
	int Variation_21();
	// @cmember Referenced table name longer than allowed for a foreign key constraint
	int Variation_22();
	// @cmember Illegal characters in the referenced table name for a foreign key constraint
	int Variation_23();
	// @cmember Bad constraint type
	int Variation_24();
	// @cmember Empty list of columns for a unique, primay key or foreign key constraint
	int Variation_25();
	// @cmember Non empty list of columns for a check constraint
	int Variation_26();
	// @cmember Not null pwszConstraintText in a unique, PK or FK constraint
	int Variation_27();
	// @cmember NULL pConstraintDesc => E_INVALIDARG
	int Variation_28();
	// @cmember 0 < cColumns and NULL == rgColumnList => E_INVALIDARG
	int Variation_29();
	// @cmember 0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG
	int Variation_30();
	// @cmember Constraint is not foreign key and pReferencedTableID is not NULL
	int Variation_31();
	// @cmember Non empty list of foreign keys for a constraint that is not foreign key
	int Variation_32();
	// @cmember Inexistent referenced table
	int Variation_33();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_Boundary)
#define THE_CLASS TCAddConstraint_Boundary
BEG_TEST_CASE(TCAddConstraint_Boundary, TCITableDefinition, L"General and boundary/null test for ITableDefinitionWithConstraints::AddConstraint")
	TEST_VARIATION(1, 		L"Check the session object (QI for interfaces)")
	TEST_VARIATION(2, 		L"pTableID is NULL => E_INVALIDARG")
	TEST_VARIATION(3, 		L"*pTableID is DB_NULLID => DB_E_NOTABLE")
	TEST_VARIATION(4, 		L"Table name is NULL => DB_E_NOTABLE")
	TEST_VARIATION(5, 		L"Table name is the empty string => DB_E_NOTABLE")
	TEST_VARIATION(6, 		L"Table name has maximum length")
	TEST_VARIATION(7, 		L"Table name exceeds maximum length => DB_E_NOTABLE")
	TEST_VARIATION(8, 		L"Table name contains illegal characters")
	TEST_VARIATION(9, 		L"Invalid DBKIND value for the base table")
	TEST_VARIATION(10, 		L"NULL column list in a constraint, column size is not 0 => E_INVALIDARG")
	TEST_VARIATION(11, 		L"NULL column name in a constraint")
	TEST_VARIATION(12, 		L"Empty string for a column name")
	TEST_VARIATION(13, 		L"Maximum length column name in a constraint")
	TEST_VARIATION(14, 		L"Column name exceeds maximum length")
	TEST_VARIATION(15, 		L"Invalid DBKIND for a column DBID of a constraint")
	TEST_VARIATION(16, 		L"Column ID is DB_NULLID")
	TEST_VARIATION(17, 		L"NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)")
	TEST_VARIATION(18, 		L"DBNULL_ID *pReferencedTableID for a foreign key constraint")
	TEST_VARIATION(19, 		L"NULL referenced table name for a foreign key constraint")
	TEST_VARIATION(20, 		L"Emptry string referenced table name for a foreign key constraint")
	TEST_VARIATION(21, 		L"Maximum length referenced table name for a foreign key constraint")
	TEST_VARIATION(22, 		L"Referenced table name longer than allowed for a foreign key constraint")
	TEST_VARIATION(23, 		L"Illegal characters in the referenced table name for a foreign key constraint")
	TEST_VARIATION(24, 		L"Bad constraint type")
	TEST_VARIATION(25, 		L"Empty list of columns for a unique, primay key or foreign key constraint")
	TEST_VARIATION(26, 		L"Non empty list of columns for a check constraint")
	TEST_VARIATION(27, 		L"Not null pwszConstraintText in a unique, PK or FK constraint")
	TEST_VARIATION(28, 		L"NULL pConstraintDesc => E_INVALIDARG")
	TEST_VARIATION(29, 		L"0 < cColumns and NULL == rgColumnList => E_INVALIDARG")
	TEST_VARIATION(30, 		L"0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG")
	TEST_VARIATION(31, 		L"Constraint is not foreign key and pReferencedTableID is not NULL")
	TEST_VARIATION(32, 		L"Non empty list of foreign keys for a constraint that is not foreign key")
	TEST_VARIATION(33, 		L"Inexistent referenced table")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_UNIQUE)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE
//
class TCAddConstraint_UNIQUE : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_UNIQUE,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Create a table and try to apply the constraint on each column
	int Variation_1();
	// @cmember Set a unique constraint on each type of column
	int Variation_2();
	// @cmember Create a unique constraint that using several columns
	int Variation_3();
	// @cmember Create several unique constraints on a table, no overlapping
	int Variation_4();
	// @cmember Create multiple, overlapping constraints on a table
	int Variation_5();
	// @cmember Create a constraint, drop it and recreate it
	int Variation_6();
	// @cmember Create a constraint on a null column
	int Variation_7();
	// @cmember Create a contraint on a not nullable column
	int Variation_8();
	// @cmember Create a constraint on a column that has a default value
	int Variation_9();
	// @cmember Create a constraint on a column defined as a primary key
	int Variation_10();
	// @cmember Create a constraint on a column defined as a foreign key
	int Variation_11();
	// @cmember Create a constraint on a column that is included in an index
	int Variation_12();
	// @cmember Use the name of an already existing constraint
	int Variation_13();
	// @cmember Constraint is created in a different catalog
	int Variation_14();
	// @cmember create a unique constraint on a non empty table
	int Variation_15();
	// @cmember Create a unique constraints on a column that has doubled values
	int Variation_16();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_UNIQUE)
#define THE_CLASS TCAddConstraint_UNIQUE
BEG_TEST_CASE(TCAddConstraint_UNIQUE, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE")
	TEST_VARIATION(1, 		L"Create a table and try to apply the constraint on each column")
	TEST_VARIATION(2, 		L"Set a unique constraint on each type of column")
	TEST_VARIATION(3, 		L"Create a unique constraint that using several columns")
	TEST_VARIATION(4, 		L"Create several unique constraints on a table, no overlapping")
	TEST_VARIATION(5, 		L"Create multiple, overlapping constraints on a table")
	TEST_VARIATION(6, 		L"Create a constraint, drop it and recreate it")
	TEST_VARIATION(7, 		L"Create a constraint on a null column")
	TEST_VARIATION(8, 		L"Create a contraint on a not nullable column")
	TEST_VARIATION(9, 		L"Create a constraint on a column that has a default value")
	TEST_VARIATION(10, 		L"Create a constraint on a column defined as a primary key")
	TEST_VARIATION(11, 		L"Create a constraint on a column defined as a foreign key")
	TEST_VARIATION(12, 		L"Create a constraint on a column that is included in an index")
	TEST_VARIATION(13, 		L"Use the name of an already existing constraint")
	TEST_VARIATION(14, 		L"Constraint is created in a different catalog")
	TEST_VARIATION(15, 		L"create a unique constraint on a non empty table")
	TEST_VARIATION(16, 		L"Create a unique constraints on a column that has doubled values")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_PRIMARYKEY)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_PRIMARYKEY
//
class TCAddConstraint_PRIMARYKEY : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_PRIMARYKEY,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Create a table and try to apply the constraint on each column
	int Variation_1();
	// @cmember Set a primary key constraint on each type of column
	int Variation_2();
	// @cmember Create a primary key constraint. Drop it. Create a primary key using the same name as before.
	int Variation_3();
	// @cmember Create multiple primary key
	int Variation_4();
	// @cmember Create multiple primary key constraints using successive calls
	int Variation_5();
	// @cmember Successive calls, lists of columns overlap
	int Variation_6();
	// @cmember Primary key constraint on a nullable column
	int Variation_7();
	// @cmember Primary key constraint on a not nullable column
	int Variation_8();
	// @cmember Primary key constraint on a column with a default value
	int Variation_9();
	// @cmember Primary key constraint on a column defined as unique
	int Variation_10();
	// @cmember Primary key constraint on a column defined as primary key
	int Variation_11();
	// @cmember Primary key constraint on a column that already has a primary key constraint
	int Variation_12();
	// @cmember Primary key constraint on a column that is part of a foreign key constraint
	int Variation_13();
	// @cmember Primary key constraint on a column that is part of an index
	int Variation_14();
	// @cmember Primary key constraint on a column on a not empty table
	int Variation_15();
	// @cmember Primary key constraint on a column that already has duplicates (for the whole key)
	int Variation_16();
	// @cmember Primary key constraint that has duplicates for a column in the key, but not for the whole key
	int Variation_17();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_PRIMARYKEY)
#define THE_CLASS TCAddConstraint_PRIMARYKEY
BEG_TEST_CASE(TCAddConstraint_PRIMARYKEY, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_PRIMARYKEY")
	TEST_VARIATION(1, 		L"Create a table and try to apply the constraint on each column")
	TEST_VARIATION(2, 		L"Set a primary key constraint on each type of column")
	TEST_VARIATION(3, 		L"Create a primary key constraint. Drop it. Create a primary key using the same name as before.")
	TEST_VARIATION(4, 		L"Create multiple primary key")
	TEST_VARIATION(5, 		L"Create multiple primary key constraints using successive calls")
	TEST_VARIATION(6, 		L"Successive calls, lists of columns overlap")
	TEST_VARIATION(7, 		L"Primary key constraint on a nullable column")
	TEST_VARIATION(8, 		L"Primary key constraint on a not nullable column")
	TEST_VARIATION(9, 		L"Primary key constraint on a column with a default value")
	TEST_VARIATION(10, 		L"Primary key constraint on a column defined as unique")
	TEST_VARIATION(11, 		L"Primary key constraint on a column defined as primary key")
	TEST_VARIATION(12, 		L"Primary key constraint on a column that already has a primary key constraint")
	TEST_VARIATION(13, 		L"Primary key constraint on a column that is part of a foreign key constraint")
	TEST_VARIATION(14, 		L"Primary key constraint on a column that is part of an index")
	TEST_VARIATION(15, 		L"Primary key constraint on a column on a not empty table")
	TEST_VARIATION(16, 		L"Primary key constraint on a column that already has duplicates (for the whole key)")
	TEST_VARIATION(17, 		L"Primary key constraint that has duplicates for a column in the key, but not for the whole key")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_FOREIGNKEY)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_FOREIGNKEY
//
class TCAddConstraint_FOREIGNKEY : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();
	CTable			*m_pReferenceTable;

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_FOREIGNKEY,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Simple foreign key constraint
	int Variation_1();
	// @cmember Foregin key is made of several columns
	int Variation_2();
	// @cmember Multiple foreign key constraints on a table
	int Variation_3();
	// @cmember Multiple overlapping foreign key constraints
	int Variation_4();
	// @cmember Create a foreign key on each column of the table
	int Variation_5();
	// @cmember Create a foreign key on all columns
	int Variation_6();
	// @cmember Create foreign key and try to drop the reference table
	int Variation_7();
	// @cmember On a nullable column
	int Variation_8();
	// @cmember On a not nullable column
	int Variation_9();
	// @cmember On a column having a default value
	int Variation_10();
	// @cmember On a column defined as unique
	int Variation_11();
	// @cmember On a primary key column
	int Variation_12();
	// @cmember On a column that is part of the index
	int Variation_13();
	// @cmember On a table that is in another catalog
	int Variation_14();
	// @cmember Use different combination for update/delete rules, as well as match type
	int Variation_15();
	// @cmember Referenced table name and base table names have maximum length
	int Variation_16();
	// @cmember Columns in the foreign key (base table and reference table) have different names
	int Variation_17();
	// @cmember Different size in column lists (base table and referenced)
	int Variation_18();
	// @cmember Column types in base table and reference table are different
	int Variation_19();
	// @cmember Invalid value for the update rule
	int Variation_20();
	// @cmember Invalid value for the delete rule
	int Variation_21();
	// @cmember Invalid value for the MatchType
	int Variation_22();
	// @cmember Inexistent column names in the referenced table
	int Variation_23();
	// @cmember Inexistent column name in the base table
	int Variation_24();
	// @cmember Inexistent base table
	int Variation_25();
	// @cmember Inexistent reference table
	int Variation_26();
	// @cmember NULL pConstraintID
	int Variation_27();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_FOREIGNKEY)
#define THE_CLASS TCAddConstraint_FOREIGNKEY
BEG_TEST_CASE(TCAddConstraint_FOREIGNKEY, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_FOREIGNKEY")
	TEST_VARIATION(1, 		L"Simple foreign key constraint")
	TEST_VARIATION(2, 		L"Foregin key is made of several columns")
	TEST_VARIATION(3, 		L"Multiple foreign key constraints on a table")
	TEST_VARIATION(4, 		L"Multiple overlapping foreign key constraints")
	TEST_VARIATION(5, 		L"Create a foreign key on each column of the table")
	TEST_VARIATION(6, 		L"Create a foreign key on all columns")
	TEST_VARIATION(7, 		L"Create foreign key and try to drop the reference table")
	TEST_VARIATION(8, 		L"On a nullable column")
	TEST_VARIATION(9, 		L"On a not nullable column")
	TEST_VARIATION(10, 		L"On a column having a default value")
	TEST_VARIATION(11, 		L"On a column defined as unique")
	TEST_VARIATION(12, 		L"On a primary key column")
	TEST_VARIATION(13, 		L"On a column that is part of the index")
	TEST_VARIATION(14, 		L"On a table that is in another catalog")
	TEST_VARIATION(15, 		L"Use different combination for update/delete rules, as well as match type")
	TEST_VARIATION(16, 		L"Referenced table name and base table names have maximum length")
	TEST_VARIATION(17, 		L"Columns in the foreign key (base table and reference table) have different names")
	TEST_VARIATION(18, 		L"Different size in column lists (base table and referenced)")
	TEST_VARIATION(19, 		L"Column types in base table and reference table are different")
	TEST_VARIATION(20, 		L"Invalid value for the update rule")
	TEST_VARIATION(21, 		L"Invalid value for the delete rule")
	TEST_VARIATION(22, 		L"Invalid value for the MatchType")
	TEST_VARIATION(23, 		L"Inexistent column names in the referenced table")
	TEST_VARIATION(24, 		L"Inexistent column name in the base table")
	TEST_VARIATION(25, 		L"Inexistent base table")
	TEST_VARIATION(26, 		L"Inexistent reference table")
	TEST_VARIATION(27, 		L"NULL pConstraintID")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_CHECK)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE
//
class TCAddConstraint_CHECK : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_CHECK,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Add simple check constraint
	int Variation_1();
	// @cmember Add 2 check constraints on the same column
	int Variation_2();
	// @cmember Add check constraint for each column
	int Variation_3();
	// @cmember Check constraint is always TRUE
	int Variation_4();
	// @cmember Check constraint is always false
	int Variation_5();
	// @cmember Long text for the check constraint
	int Variation_6();
	// @cmember Check constraint is made of white characters
	int Variation_7();
	// @cmember Maximum length for the table name
	int Variation_8();
	// @cmember Create the constraint, drop it, readd it with different text
	int Variation_9();
	// @cmember Use inexistent column names inthe check constraint
	int Variation_10();
	// @cmember Use qualified table column with different table
	int Variation_11();
	// @cmember Set a check condition on an index column
	int Variation_12();
	// @cmember pwszConstraintText is NULL
	int Variation_13();
	// @cmember pwszConstraintText is an empty string
	int Variation_14();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_CHECK)
#define THE_CLASS TCAddConstraint_CHECK
BEG_TEST_CASE(TCAddConstraint_CHECK, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE")
	TEST_VARIATION(1, 		L"Add simple check constraint")
	TEST_VARIATION(2, 		L"Add 2 check constraints on the same column")
	TEST_VARIATION(3, 		L"Add check constraint for each column")
	TEST_VARIATION(4, 		L"Check constraint is always TRUE")
	TEST_VARIATION(5, 		L"Check constraint is always false")
	TEST_VARIATION(6, 		L"Long text for the check constraint")
	TEST_VARIATION(7, 		L"Check constraint is made of white characters")
	TEST_VARIATION(8, 		L"Maximum length for the table name")
	TEST_VARIATION(9, 		L"Create the constraint, drop it, readd it with different text")
	TEST_VARIATION(10, 		L"Use inexistent column names inthe check constraint")
	TEST_VARIATION(11, 		L"Use qualified table column with different table")
	TEST_VARIATION(12, 		L"Set a check condition on an index column")
	TEST_VARIATION(13, 		L"pwszConstraintText is NULL")
	TEST_VARIATION(14, 		L"pwszConstraintText is an empty string")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_Deferrability)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting
//
class TCAddConstraint_Deferrability : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_Deferrability,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Immediate unique constraint
	int Variation_1();
	// @cmember Defferable unique constraint
	int Variation_2();
	// @cmember Deffered unique constraint
	int Variation_3();
	// @cmember Bad deferrability value unique constraint
	int Variation_4();
	// @cmember Immediate primary key constraint
	int Variation_5();
	// @cmember Deferrable primary key constraint
	int Variation_6();
	// @cmember Deferred primary key constraint
	int Variation_7();
	// @cmember Bad deferrability value primary key constraint
	int Variation_8();
	// @cmember Immediate foreign key constraint
	int Variation_9();
	// @cmember Deferrable foreign key constraint
	int Variation_10();
	// @cmember Deferred foreign key constraint
	int Variation_11();
	// @cmember Bad deferrability value foreign key constraint
	int Variation_12();
	// @cmember Immediate check constraint
	int Variation_13();
	// @cmember Deferrable check constraint
	int Variation_14();
	// @cmember Deferred check constraint
	int Variation_15();
	// @cmember Bad deferrability value check constraint
	int Variation_16();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCAddConstraint_Deferrability)
#define THE_CLASS TCAddConstraint_Deferrability
BEG_TEST_CASE(TCAddConstraint_Deferrability, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting")
	TEST_VARIATION(1, 		L"Immediate unique constraint")
	TEST_VARIATION(2, 		L"Defferable unique constraint")
	TEST_VARIATION(3, 		L"Deffered unique constraint")
	TEST_VARIATION(4, 		L"Bad deferrability value unique constraint")
	TEST_VARIATION(5, 		L"Immediate primary key constraint")
	TEST_VARIATION(6, 		L"Deferrable primary key constraint")
	TEST_VARIATION(7, 		L"Deferred primary key constraint")
	TEST_VARIATION(8, 		L"Bad deferrability value primary key constraint")
	TEST_VARIATION(9, 		L"Immediate foreign key constraint")
	TEST_VARIATION(10, 		L"Deferrable foreign key constraint")
	TEST_VARIATION(11, 		L"Deferred foreign key constraint")
	TEST_VARIATION(12, 		L"Bad deferrability value foreign key constraint")
	TEST_VARIATION(13, 		L"Immediate check constraint")
	TEST_VARIATION(14, 		L"Deferrable check constraint")
	TEST_VARIATION(15, 		L"Deferred check constraint")
	TEST_VARIATION(16, 		L"Bad deferrability value check constraint")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END


// {{ TCW_TEST_CASE_MAP(TCCreateTableWithConstraints_NoConstraint)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting
//
class TCCreateTableWithConstraints_NoConstraint : public TCCreateTable { 
public:
	//DECLARE_TEST_CASE_DATA();
	ITableDefinitionWithConstraints		*m_pITableDefinitionWithConstraints;

public:
	static const WCHAR		m_wszTestCaseName[];		
    static const WCHAR		m_wszTestCaseDesc[];		
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCCreateTableWithConstraints_NoConstraint,TCCreateTable);
	// }} TCW_DECLARE_FUNCS_END

	// @member Create the table using ITableDefinition::CreateTable and check its creation
	virtual HRESULT	CreateAndCheckTable(
		ITableDefinition	*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinition interface to be used
		IUnknown			*pUnkOuter,			// [in] pointer to controlling unknown
		DBID				*pTableID,			// [in]	pointer to the input table ID
		ULONG				cColumnDescs,		// [in]	the number of columns to be created
		DBCOLUMNDESC		*rgColumnDesc,		// [in/out] array of columns to be created
		REFIID				riid,				// [in] rowset interface to be returned
		ULONG				cPropertySets,		// [in] number of roset property sets asked
		DBPROPSET			*rgPropertySets,	// [in/out] array of rowset property sets
		DBID				**ppTableID,		// [out] returned table ID
		IUnknown			**ppRowset			// [out] pointer to outer interface
	);
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCCreateTableWithConstraints_NoConstraint)
//#define THE_CLASS TCCreateTableWithConstraints_NoConstraint
//BEG_TEST_CASE(TCCreateTableWithConstraints_NoConstraint, TCCreateTable, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting")
//END_TEST_CASE()
//#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

const WCHAR		TCCreateTableWithConstraints_NoConstraint::m_wszTestCaseName[] = { L"TCCreateTableWithConstraints_NoConstraint" };
const WCHAR		TCCreateTableWithConstraints_NoConstraint::m_wszTestCaseDesc[] = { L"Testcase for ITableDefinitionWithConstraints::CreateTable when no constraints are specified => should expect the same results as for ITableDefinition::CreateTable" };

// {{ TCW_TEST_CASE_MAP(TCCreateTableWithConstraints)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints when constraints are set
//
class TCCreateTableWithConstraints : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCCreateTableWithConstraints,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Create a table and multiple constraint of various types
	int Variation_1();
	// @cmember Table with a unique constraint (on a single column)
	int Variation_2();
	// @cmember Table with a unique constraint (on multiple columns)
	int Variation_3();
	// @cmember Table with a primary key constraint (on a single column)
	int Variation_4();
	// @cmember Table with a primary key constraint (on multiple columns)
	int Variation_5();
	// @cmember Table with a foreign key constraint (single column)
	int Variation_6();
	// @cmember Table with foreign key constraint defined on multiple keys
	int Variation_7();
	// @cmember Table with a simple check constraint
	int Variation_8();
	// @cmember Table with multiple unique constraints defined
	int Variation_9();
	// @cmember Table with unique constraint and foreign key
	int Variation_10();
	// @cmember Table with primary key and check constraint
	int Variation_11();
	// @cmember Deferrable, immediate constraints
	int Variation_12();
	// @cmember Deferred constraints
	int Variation_13();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCCreateTableWithConstraints)
#define THE_CLASS TCCreateTableWithConstraints
BEG_TEST_CASE(TCCreateTableWithConstraints, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints when constraints are set")
	TEST_VARIATION(1, 		L"Create a table and multiple constraint of various types")
	TEST_VARIATION(2, 		L"Table with a unique constraint (on a single column)")
	TEST_VARIATION(3, 		L"Table with a unique constraint (on multiple columns)")
	TEST_VARIATION(4, 		L"Table with a primary key constraint (on a single column)")
	TEST_VARIATION(5, 		L"Table with a primary key constraint (on multiple columns)")
	TEST_VARIATION(6, 		L"Table with a foreign key constraint (single column)")
	TEST_VARIATION(7, 		L"Table with foreign key constraint defined on multiple keys")
	TEST_VARIATION(8, 		L"Table with a simple check constraint")
	TEST_VARIATION(9, 		L"Table with multiple unique constraints defined")
	TEST_VARIATION(10, 		L"Table with unique constraint and foreign key")
	TEST_VARIATION(11, 		L"Table with primary key and check constraint")
	TEST_VARIATION(12, 		L"Deferrable, immediate constraints")
	TEST_VARIATION(13, 		L"Deferred constraints")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCreateTableWithConstraints_Boundary)
//*-----------------------------------------------------------------------
// @class Boundary and NULL testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints
//
class TCCreateTableWithConstraints_Boundary : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCCreateTableWithConstraints_Boundary,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember 0 < cConstraintDescs and NULL == rgConstraintDescs => E_INVALIDARG
	int Variation_1();
	// @cmember 0 == cConstraintDescs and NULL != rgConstraintDescs => S_OK
	int Variation_2();
	// @cmember pTableID is NULL ppTableID is not NULL => S_OK
	int Variation_3();
	// @cmember Bad DBKIND of a constraint ID
	int Variation_4();
	// @cmember NULL pConstraintID in a constraint
	int Variation_5();
	// @cmember NULL column list in a constraint, column size is not 0 => E_INVALIDARG
	int Variation_6();
	// @cmember Empty list of columns for a unique, primay key or foreign key constraint
	int Variation_7();
	// @cmember Non empty list of columns for a check constraint
	int Variation_8();
	// @cmember Non empty list of foreign keys for a constraint that is not foreign key
	int Variation_9();
	// @cmember 0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG
	int Variation_10();
	// @cmember Constraint is not foreign key and pReferencedTableID is not NULL
	int Variation_11();
	// @cmember NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)
	int Variation_12();
	// @cmember NULL referenced table name for a foreign key constraint
	int Variation_13();
	// @cmember DB_NULLID *pReferencedTableID for a foreign key constraint
	int Variation_14();
	// @cmember Empty string referenced table name for a foreign key constraint
	int Variation_15();
	// @cmember Maximum length referenced table name for a foreign key constraint
	int Variation_16();
	// @cmember Referenced table name longer than allowed for a foreign key constraint
	int Variation_17();
	// @cmember Illegal characters in the referenced table name for a foreign key constraint
	int Variation_18();
	// @cmember Bad constraint type
	int Variation_19();
	// @cmember Inexistent referenced table
	int Variation_20();
	// @cmember Empty string for a column name
	int Variation_21();
	// @cmember Maximum length column name in a constraint
	int Variation_22();
	// @cmember Invalid DBKIND for a column DBID of a constraint
	int Variation_23();
	// @cmember Column ID is DB_NULLID
	int Variation_24();
	// @cmember Inexistent column => DB_E_BADCOLUMNID
	int Variation_25();
	// @cmember NULL column name => DB_E_BADCOLUMNID
	int Variation_26();
	// @cmember Column name exceeds maximum length name
	int Variation_27();
	// @cmember Invalid column name
	int Variation_28();
	// @cmember Existent table name => DB_E_DUPLICATETABLEID
	int Variation_29();
	// @cmember Same constraint name in 2 constraints => DB_E_DUPLICATECONSTRAINTID
	int Variation_30();
	// @cmember NULL pConstraintID in 2 constraints => S_OK
	int Variation_31();
	// }} TCW_TESTVARS_END
} ;

// {{ TCW_TESTCASE(TCCreateTableWithConstraints_Boundary)
#define THE_CLASS TCCreateTableWithConstraints_Boundary
BEG_TEST_CASE(TCCreateTableWithConstraints_Boundary, TCITableDefinition, L"Boundary and NULL testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints")
	TEST_VARIATION(1, 		L"0 < cConstraintDescs and NULL == rgConstraintDescs => E_INVALIDARG")
	TEST_VARIATION(2, 		L"0 == cConstraintDescs and NULL != rgConstraintDescs => S_OK")
	TEST_VARIATION(3, 		L"pTableID is NULL ppTableID is not NULL => S_OK")
	TEST_VARIATION(4, 		L"Bad DBKIND of a constraint ID")
	TEST_VARIATION(5, 		L"NULL pConstraintID in a constraint")
	TEST_VARIATION(6, 		L"NULL column list in a constraint, column size is not 0 => E_INVALIDARG")
	TEST_VARIATION(7, 		L"Empty list of columns for a unique, primay key or foreign key constraint")
	TEST_VARIATION(8, 		L"Non empty list of columns for a check constraint")
	TEST_VARIATION(9, 		L"Non empty list of foreign keys for a constraint that is not foreign key")
	TEST_VARIATION(10, 		L"0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG")
	TEST_VARIATION(11, 		L"Constraint is not foreign key and pReferencedTableID is not NULL")
	TEST_VARIATION(12, 		L"NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)")
	TEST_VARIATION(13, 		L"NULL referenced table name for a foreign key constraint")
	TEST_VARIATION(14, 		L"DB_NULLID *pReferencedTableID for a foreign key constraint")
	TEST_VARIATION(15, 		L"Empty string referenced table name for a foreign key constraint")
	TEST_VARIATION(16, 		L"Maximum length referenced table name for a foreign key constraint")
	TEST_VARIATION(17, 		L"Referenced table name longer than allowed for a foreign key constraint")
	TEST_VARIATION(18, 		L"Illegal characters in the referenced table name for a foreign key constraint")
	TEST_VARIATION(19, 		L"Bad constraint type")
	TEST_VARIATION(20, 		L"Inexistent referenced table")
	TEST_VARIATION(21, 		L"Empty string for a column name")
	TEST_VARIATION(22, 		L"Maximum length column name in a constraint")
	TEST_VARIATION(23, 		L"Invalid DBKIND for a column DBID of a constraint")
	TEST_VARIATION(24, 		L"Column ID is DB_NULLID")
	TEST_VARIATION(25, 		L"Inexistent column => DB_E_BADCOLUMNID")
	TEST_VARIATION(26, 		L"NULL column name => DB_E_BADCOLUMNID")
	TEST_VARIATION(27, 		L"Column name exceeds maximum length name")
	TEST_VARIATION(28, 		L"Invalid column name")
	TEST_VARIATION(29, 		L"Existent table name => DB_E_DUPLICATETABLEID")
	TEST_VARIATION(30, 		L"Same constraint name in 2 constraints => DB_E_DUPLICATECONSTRAINTID")
	TEST_VARIATION(31, 		L"NULL pConstraintID in 2 constraints => S_OK")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCDropConstraint)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::DropConstraint
//
class TCDropConstraint : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCDropConstraint,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember pTableID is NULL => E_INVALIDARG
	int Variation_1();
	// @cmember pTableID.uName.pwszName is NULL => DB_E_BADTABLEID
	int Variation_2();
	// @cmember Table name is empty => DB_E_BADTABLEID
	int Variation_3();
	// @cmember Table name is not the one on which the column was created
	int Variation_4();
	// @cmember Inexistent table name => DB_E_NOTABLE
	int Variation_5();
	// @cmember Maximum table name => S_OK
	int Variation_6();
	// @cmember Invalid characters in table's name => DB_E_BADTABLEID
	int Variation_7();
	// @cmember pConstraintID is NUL => E_INVALIDARG
	int Variation_8();
	// @cmember *pConstraintID is DB_NULLID => DB_E_BADID
	int Variation_9();
	// @cmember Invalid DBKIND in the constraint name => DB_E_BADID
	int Variation_10();
	// @cmember NULL constraint name => DB_E_BADID
	int Variation_11();
	// @cmember Constraint name is empty string => DB_E_BADID
	int Variation_12();
	// @cmember Inexistent constraint name => DB_E_BADCONSTRAINTID
	int Variation_13();
	// @cmember Drop a constraint twice
	int Variation_14();
	// @cmember Drop a unique constraint
	int Variation_15();
	// @cmember Drop a primary key constraint
	int Variation_16();
	// @cmember Drop a foreign key constraint
	int Variation_17();
	// @cmember Drop a check constraint
	int Variation_18();
	// @cmember Create several constraints, drop a single constraint
	int Variation_19();
	// @cmember Drop a unique constraint in the referenced table, after the foreign key constraint was added
	int Variation_20();
	// @cmember Try to drop a constraint while a rowset is opened on the table => DB_E_TABLEINUSE
	int Variation_21();
	// @cmember *pTableID is DB_NULLID => DB_E_BADTABLEID
	int Variation_22();
	// @cmember Invalid DBKIND in pTableID => DB_E_BADTABLEID
	int Variation_23();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCDropConstraint)
#define THE_CLASS TCDropConstraint
BEG_TEST_CASE(TCDropConstraint, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::DropConstraint")
	TEST_VARIATION(1, 		L"pTableID is NULL => E_INVALIDARG")
	TEST_VARIATION(2, 		L"pTableID.uName.pwszName is NULL => DB_E_BADTABLEID")
	TEST_VARIATION(3, 		L"Table name is empty => DB_E_BADTABLEID")
	TEST_VARIATION(4, 		L"Table name is not the one on which the column was created")
	TEST_VARIATION(5, 		L"Inexistent table name => DB_E_NOTABLE")
	TEST_VARIATION(6, 		L"Maximum table name => S_OK")
	TEST_VARIATION(7, 		L"Invalid characters in table's name => DB_E_BADTABLEID")
	TEST_VARIATION(8, 		L"pConstraintID is NUL => E_INVALIDARG")
	TEST_VARIATION(9, 		L"*pConstraintID is DB_NULLID => DB_E_BADID")
	TEST_VARIATION(10, 		L"Invalid DBKIND in the constraint name => DB_E_BADID")
	TEST_VARIATION(11, 		L"NULL constraint name => DB_E_BADID")
	TEST_VARIATION(12, 		L"Constraint name is empty string => DB_E_BADID")
	TEST_VARIATION(13, 		L"Inexistent constraint name => DB_E_BADCONSTRAINTID")
	TEST_VARIATION(14, 		L"Drop a constraint twice")
	TEST_VARIATION(15, 		L"Drop a unique constraint")
	TEST_VARIATION(16, 		L"Drop a primary key constraint")
	TEST_VARIATION(17, 		L"Drop a foreign key constraint")
	TEST_VARIATION(18, 		L"Drop a check constraint")
	TEST_VARIATION(19, 		L"Create several constraints, drop a single constraint")
	TEST_VARIATION(20, 		L"Drop a unique constraint in the referenced table, after the foreign key constraint was added")
	TEST_VARIATION(21, 		L"Try to drop a constraint while a rowset is opened on the table => DB_E_TABLEINUSE")
	TEST_VARIATION(22, 		L"*pTableID is DB_NULLID => DB_E_BADTABLEID")
	TEST_VARIATION(23, 		L"Invalid DBKIND in pTableID => DB_E_BADTABLEID")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCAddConstraint_Multithreading)
//*-----------------------------------------------------------------------
// @class Testcase for ITableDefinitionWithConstraints::AddConstraint used in multithreaded calls
//
class TCAddConstraint_Multithreading : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

	DBID				**m_prgTableID;
	DBCONSTRAINTDESC	*m_rgConstraintDesc;

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCAddConstraint_Multithreading,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Each thread creates a different constraint
	int Variation_1();
	// @cmember Some of the threads try to create the same constraint name
	int Variation_2();
	// @cmember Unique constraints with disjoint columns
	int Variation_3();
	// @cmember Unique constraints with overlapping columns
	int Variation_4();
	// @cmember All constraint IDs are generated by the provider
	int Variation_5();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
} ;
// {{ TCW_TESTCASE(TCAddConstraint_Multithreading)
#define THE_CLASS TCAddConstraint_Multithreading
BEG_TEST_CASE(TCAddConstraint_Multithreading, TCITableDefinition, L"Testcase for ITableDefinitionWithConstraints::AddConstraint used in multithreaded calls")
	TEST_VARIATION(1, 		L"Each thread creates a different constraint")
	TEST_VARIATION(2, 		L"Some of the threads try to create the same constraint name")
	TEST_VARIATION(3, 		L"Unique constraints with disjoint columns")
	TEST_VARIATION(4, 		L"Unique constraints with overlapping columns")
	TEST_VARIATION(5, 		L"All constraint IDs are generated by the provider")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCDropConstraint_Multithreading)
//*-----------------------------------------------------------------------
// @class Multithreading test case for ITableDefinitionWithConstraints::DropConstraints
//
class TCDropConstraint_Multithreading : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

	DBID		**m_prgTableID;
	DBID		**m_prgConstraintID;

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCDropConstraint_Multithreading,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Each thread drops a different constraint => all S_OK
	int Variation_1();
	// @cmember Some threads try to drop the same constraint => S_OK and DB_E_BADCONSTRAINTID
	int Variation_2();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
} ;
// {{ TCW_TESTCASE(TCDropConstraint_Multithreading)
#define THE_CLASS TCDropConstraint_Multithreading
BEG_TEST_CASE(TCDropConstraint_Multithreading, TCITableDefinition, L"Multithreading test case for ITableDefinitionWithConstraints::DropConstraints")
	TEST_VARIATION(1, 		L"Each thread drops a different constraint => all S_OK")
	TEST_VARIATION(2, 		L"Some threads try to drop the same constraint => S_OK and DB_E_BADCONSTRAINTID")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCCreateTableWithConstraints_Multithreading)
//*-----------------------------------------------------------------------
// @class Multithreading test case for ITableDefinitionWithConstraints::CreateTableWithConstraints
//
class TCCreateTableWithConstraints_Multithreading : public TCITableDefinition { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

	CCTWCThreadParam	*m_rgThreadParams;

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCCreateTableWithConstraints_Multithreading,TCITableDefinition);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember Each thread builds different table => all S_OK
	int Variation_1();
	// @cmember Some threads build the same table => S_OK and DB_E_DUPLICATETABLEID
	int Variation_2();
	// }} TCW_TESTVARS_END

	unsigned MyThreadProc(ULONG i);
} ;
// {{ TCW_TESTCASE(TCCreateTableWithConstraints_Multithreading)
#define THE_CLASS TCCreateTableWithConstraints_Multithreading
BEG_TEST_CASE(TCCreateTableWithConstraints_Multithreading, TCITableDefinition, L"Multithreading test case for ITableDefinitionWithConstraints::CreateTableWithConstraints")
	TEST_VARIATION(1, 		L"Each thread builds different table => all S_OK")
	TEST_VARIATION(2, 		L"Some threads build the same table => S_OK and DB_E_DUPLICATETABLEID")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// }} END_DECLARE_TEST_CASES()


// {{ TCW_TESTMODULE(ThisModule)
TEST_MODULE(18, ThisModule, gwszModuleDescrip)
	TEST_CASE(1, TCAddColumn)
	TEST_CASE(2, TCDropColumn)
	TEST_CASE(3, TCDropTable)
	TEST_CASE(4, TCCreateTable)
	TEST_CASE(5, RODataSource)
	TEST_CASE(6, TCAddConstraint_Boundary)
	TEST_CASE(7, TCAddConstraint_UNIQUE)
	TEST_CASE(8, TCAddConstraint_PRIMARYKEY)
	TEST_CASE(9, TCAddConstraint_FOREIGNKEY)
	TEST_CASE(10, TCAddConstraint_CHECK)
	TEST_CASE(11, TCAddConstraint_Deferrability)
	TEST_CASE(12, TCCreateTableWithConstraints_NoConstraint)
	TEST_CASE(13, TCCreateTableWithConstraints)
	TEST_CASE(14, TCCreateTableWithConstraints_Boundary)
	TEST_CASE(15, TCDropConstraint)
	TEST_CASE(16, TCAddConstraint_Multithreading)
	TEST_CASE(17, TCDropConstraint_Multithreading)
	TEST_CASE(18, TCCreateTableWithConstraints_Multithreading)
END_TEST_MODULE()
// }} TCW_TESTMODULE_END




//---------------------------------------------------------------------
//
// @cmember Initialize the TCIDefinition data members
//---------------------------------------------------------------------
BOOL TCITableDefinition::Init()
{
	m_pITDWC			= NULL;
	m_pITableDefinition	= NULL;
	m_nTables			= 0;

	InitializeCriticalSection(&m_CriticalSection);
	if(CSessionObject::Init())
	{
		SetDataSourceObject(m_pThisTestModule->m_pIUnknown);
		SetDBSession(m_pThisTestModule->m_pIUnknown2);
		// select ITableDefinition as standard table creation method
		GetModInfo()->UseITableDefinition(TRUE);	
		GetProviderTypes();
		GetLiteralInfo();
		m_nTables = GetNoOfTables();
		m_pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);

		// check for ITableDefinition
		if (!(m_hr=VerifyInterface(m_pIOpenRowset, IID_ITableDefinition, SESSION_INTERFACE, (IUnknown**)&m_pITableDefinition)))
			return FALSE;
		
		m_pIOpenRowset->QueryInterface(IID_ITableDefinitionWithConstraints, (void**)&m_pITDWC);
		
		if (m_pITDWC)
		{
			// find out what values of matching types are supported for foreign keys
			DBORDINAL	rgCol[] = {1};
			CTable		Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
			CTable		Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
			HRESULT		hr;

			TESTC_(Table.CreateTable(0, 1), S_OK);
			TESTC_(Table2.CreateTable(0, 1), S_OK);
	
			m_SupportedMatchType = DBMATCHTYPE_NONE;
			TEST2C_(hr = AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, 
				m_SupportedMatchType), S_OK, DB_E_BADMATCHTYPE);

			if (S_OK != hr)
			{
				m_SupportedMatchType = DBMATCHTYPE_PARTIAL;
				TEST2C_(hr = AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, 
					m_SupportedMatchType), S_OK, DB_E_BADMATCHTYPE);

				if (S_OK != hr)
				{
					m_SupportedMatchType = DBMATCHTYPE_FULL;
					TESTC_(hr = AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, 
						m_SupportedMatchType), S_OK);
				}
			}

		CLEANUP:
			Table.DropTable();
			Table2.DropTable();
		}
		return TRUE;
	}
	return FALSE;
}




//---------------------------------------------------------------------
//
// @cmember constructor
//---------------------------------------------------------------------
TCITableDefinition::TCITableDefinition(WCHAR *wstrTestCaseName):CSessionObject(wstrTestCaseName)
{
	m_fIsIDBInfo			= NULL;
	m_pwszInvalidTableChars	= NULL;
	m_pwszInvalidColChars	= NULL;
	m_pITableDefinition		= NULL;
	m_rgColumnDesc			= NULL;
	m_cColumnDesc			= 0;
} //TCITableDefinition::TCITableDefinition




//-------------------------------------------------------------------------
//
// @cmember Compare two column in the identical position
//------------------------------------------------------------------------- 
BOOL TCITableDefinition::AreColEqual
(
	   DBCOLUMNDESC	*oldCD, 
	   DBCOLUMNDESC *newCD
)
{
	// when this function is called, it is supposed that m_HasAuto... were set by GetTableColumnInfo
	// and unmodified ever since
	ULONG		cPropSet, cProp;
	BOOL		fResult = FALSE;
	DBPROPSET	*pNewPropSet = NULL;
	DBPROPSET	*pOldPropSet = NULL;
	DBPROP		*pNewProp = NULL;
	DBPROP		*pOldProp = NULL;

	// compare everything in the stru
	ASSERT(oldCD->pTypeInfo == newCD->pTypeInfo);
	TESTC(oldCD->pTypeInfo == newCD->pTypeInfo);
	TESTC(oldCD->ulColumnSize == newCD->ulColumnSize);
	TESTC(oldCD->wType == newCD->wType);
	TESTC(!IsNumericType(oldCD->wType) || oldCD->bPrecision == newCD->bPrecision);
	TESTC(	(oldCD->wType != DBTYPE_NUMERIC && oldCD->wType != DBTYPE_DECIMAL) 
			|| oldCD->bScale == newCD->bScale);
	TESTC(m_hr = CompareDBID(oldCD->dbcid, newCD->dbcid, m_pIDBInitialize));

	// invalidated for the time being
	// if the CLSID are to be compared for the lists, then a GUID object has to be allocated and mentained; 
	// during the life of each ColumnDescriptor array
	// if a pointer to m_gTypeGUID of a column is stored we'll have troubles as soon as the value of the column changes
	//if (m_pTable->GetHasCLSID() && !CHECK(oldCD->pclsid == newCD->pclsid, TRUE))
	//	return FALSE;
	
	// compare the properties
	TESTC(oldCD->cPropertySets == newCD->cPropertySets);

	for (cPropSet=0; cPropSet<oldCD->cPropertySets; cPropSet++)
	{
		pOldPropSet = &(oldCD->rgPropertySets[cPropSet]);
		pNewPropSet = &(newCD->rgPropertySets[cPropSet]);

		// compare the props in that prop set
		TESTC(pOldPropSet->guidPropertySet == pNewPropSet->guidPropertySet);
		TESTC(pOldPropSet->cProperties == pNewPropSet->cProperties);

		for (cProp = 0; cProp < pOldPropSet->cProperties; cProp++)
		{
			pOldProp = &(pOldPropSet->rgProperties[cProp]);
			pNewProp = &(pNewPropSet->rgProperties[cProp]);

			// there are no info for some of the properties
			if (	((pNewProp->dwPropertyID == DBPROP_COL_AUTOINCREMENT) && !m_pTable->GetHasAuto()) 
				||	((pNewProp->dwPropertyID == DBPROP_COL_DEFAULT) && !m_pTable->GetHasDefault()) 
				||	((pNewProp->dwPropertyID == DBPROP_COL_UNIQUE) && !m_pTable->GetHasUnique())
			   )
				continue;

			TESTC(pOldProp->dwPropertyID ==	pNewProp->dwPropertyID);
			TESTC(CompareVariant(&(pOldProp->vValue), &(pNewProp->vValue)));
		}
	}
	fResult = TRUE;

CLEANUP:
	return fResult;
} //TCITableDefinition::AreColEqual




//-------------------------------------------------------------------------
//
// @cmember Compare the list of old columns against the list of new columns
//------------------------------------------------------------------------- 
BOOL TCITableDefinition::AreColEqual
(
	   DBCOLUMNDESC	*oldCD, 
	   ULONG		oldlen, 
	   DBCOLUMNDESC *newCD, 
	   ULONG		newlen
)
{
	// when this function is called, it is supposed that m_HasAuto... were set by GetTableColumnInfo
	// and unmodified ever since
	ULONG	i;
	BOOL	fResult = FALSE;

	TESTC(oldlen == newlen);

	for (i=0; i<oldlen; i++)
	{
		TESTC(AreColEqual(&newCD[i], &oldCD[i]));
	}

	fResult = TRUE;

CLEANUP:
	return fResult;
} //TCITableDefinition::AreColEqual




//--------------------------------------------------------------------------
//
// @cmember Check the result and property status
//--------------------------------------------------------------------------
BOOL TCITableDefinition::CheckPropertyStatus(
	GUID	guidPropertySet,	// [in]	property set guid 
	GUID	guidExpectedPS,		// [in] expected prop set guid
	DBPROP	*pProp		,		// [in]	property
	HRESULT hr					// [in] the result
)
{
	BOOL		fRes = TRUE;
	BOOL		fSupported;
	BOOL		fSettable;
	VARTYPE		vtType;

	if (!pProp)
		return TRUE;
	
	// general checking
	if (DBPROPOPTIONS_REQUIRED == pProp->dwOptions && DBPROPSTATUS_OK != pProp->dwStatus)
		fRes = CHECK(FAILED(hr), TRUE);

	if (!(guidPropertySet == guidExpectedPS))
		// check a couple of things and returns
		return COMPARE(pProp->dwStatus, DBPROPSTATUS_NOTSUPPORTED) && COMPARE(S_OK != hr, TRUE);

	fSupported	= SupportedProperty(pProp->dwPropertyID, guidPropertySet);
	fSettable	= SettableProperty(pProp->dwPropertyID, guidPropertySet);

	if (!fSupported && !COMPARE(S_OK != hr, TRUE))
		fRes = FALSE;
		
	// status driven checking
	switch (pProp->dwStatus)
	{
		case DBPROPSTATUS_OK:
			fRes = COMPARE(fSupported != 0, TRUE);
			break;
		case DBPROPSTATUS_NOTSUPPORTED:
			fRes = COMPARE(fSupported == 0, TRUE);
			break;
		case DBPROPSTATUS_NOTSETTABLE:
			fRes = COMPARE(fSupported != 0, TRUE) && CHECK(fSettable == 0, TRUE);
			break;
		case DBPROPSTATUS_BADOPTION:
			fRes =	(pProp->dwOptions != DBPROPOPTIONS_OPTIONAL) 
				&&	COMPARE(DBPROPOPTIONS_REQUIRED != pProp->dwOptions, TRUE);
			break;
		case DBPROPSTATUS_CONFLICTING:
			fRes = COMPARE(fSettable != 0, TRUE);
			break;
		case DBPROPSTATUS_BADVALUE:
			// this could mean not supported property or value
			break;
	}

	// DBPROPSTATUS_BADVALUE can be certainly got only if type of the prop is wrong
	// info about supported discret values of the prop cannot be generacally obtained 
	// before creating an index
	vtType = GetPropInfoType(pProp->dwPropertyID, guidPropertySet);
	if (	DBPROP_COL_DEFAULT != pProp->dwPropertyID
		&&	vtType != pProp->vValue.vt
		&&	!COMPARE(DBPROPSTATUS_OK == pProp->dwStatus, FALSE))
	{
			odtLog << "ERROR: bad propstatus on improper type value on property" << pProp->dwPropertyID << "\n";
			fRes = FALSE;
	}

	return fRes;
} //TCITableDefinition::CheckProperty




//--------------------------------------------------------------------
//
// @cmember Check the prop status of a list of col desc
//	returns TRUE if all status are set consistent with prop and result
//			FALSE otherwise
//--------------------------------------------------------------------
BOOL TCITableDefinition::CheckColPropStatus(
	ULONG			cColDesc,			// [in] 
	DBCOLUMNDESC	*rgColDesc,			// [in] the size of the propset array
	HRESULT			hr					// [in]	the result of the previous op
)
{
	ULONG	cPropSet;
	ULONG	cProp;
	BOOL	fRes = TRUE;
	ULONG	nCol;

	if (!rgColDesc)
		return TRUE;

	for (nCol = 0; nCol < cColDesc; nCol++)
	{
		if (!rgColDesc[nCol].rgPropertySets)
			continue;
		for (cPropSet = 0; cPropSet < rgColDesc[nCol].cPropertySets; cPropSet++)
		{
			if (!rgColDesc[nCol].rgPropertySets[cPropSet].rgProperties)
				continue;
			for (cProp = 0; cProp < rgColDesc[nCol].rgPropertySets[cPropSet].cProperties; cProp++)
			{
				fRes = fRes && CheckPropertyStatus(rgColDesc[nCol].rgPropertySets[cPropSet].guidPropertySet, 
					DBPROPSET_COLUMN, &rgColDesc[nCol].rgPropertySets[cPropSet].rgProperties[cProp], hr);
			}
		}
	}
	return fRes;
} // TCITableDefinition::CheckColPropStatus




//--------------------------------------------------------------------
//
// @cmember CompareColumnDesc
//		pColumnDesc and pNewCol		[IN] parameteres to be compared
//		column addition (table creation) is assumed to have succeeded
// returns TRUE or FALSE
//--------------------------------------------------------------------
BOOL TCITableDefinition::CompareColumnDesc(
	DBCOLUMNDESC *pColumnDesc,	// [in]	column descriptor, as passed to ITableDefinition::AddColumn()
	DBCOLUMNDESC *pNewCol		// [in]	column descriptor, as read from the table after addition
)
{
	DBPROPSET	*pNewPropSet	= NULL;
	DBPROPSET	*pInPropSet		= NULL;
	DBPROP		*pInProp		= NULL;
	ULONG		j;
	ULONG		cPropSet, cProp;
	ULONG		n;
	POSITION	pos, posOld, posFind;
	CCol		col;
	BOOL		fRes = FALSE;
	GUID		gCD;

	TESTC(pColumnDesc->pTypeInfo == pNewCol->pTypeInfo);
	
	PRVTRACE("TCITableDefinition::CompColumnDescreColumnDesc: pColumnDesc->wType = %d, pNewCol->wType = %d\n",  
			pColumnDesc->wType, pNewCol->wType);
	TESTC(pColumnDesc->wType == pNewCol->wType);

	TESTC(CompareDBID(pColumnDesc->dbcid, pNewCol->dbcid, m_pIDBInitialize));

	// if there is a provider type name, the column size, precision and 
	// scale of the column can be checked otherwise skip it there could 
	// be several provider types implemented on the DBTYPE
	if (!pColumnDesc->pwszTypeName)
	{
		// are multiple provider types implemented on the same DBTYPE?
		for (posFind = pos = m_ColList.GetHeadPosition(), n=0; pos;)
		{
			posOld = pos;
			col = m_ColList.GetNext(pos);
			if (col.GetProviderType() == pColumnDesc->wType)
			{
				n++;
				posFind = posOld;
			}
		}
		col = m_ColList.GetAt(posFind);
	}
	else
	{
		BOOL	fExistType;
		col = GetType(pColumnDesc->pwszTypeName, pColumnDesc->wType, &fExistType);
		n = fExistType ? 1: 0;
	}

	// see if there are enough data for a compColumnDescrison
	if (1 != n)
	{
		fRes = TRUE;
		goto CLEANUP;
	}

	// CHECK THE COLUMN SIZE

	// for DBTYPE_STR, DBTYPE_WSTR, DBTYPE_BSTR check whether the 
	// size of the column fits with the spec ones or the maximum
	if ((pColumnDesc->wType == DBTYPE_STR)  || 
		(pColumnDesc->wType == DBTYPE_WSTR) || 
		(pColumnDesc->wType == DBTYPE_BYTES)   )
	{
		LPWSTR	pwszCreateParams = col.GetCreateParams();
		if (col.GetColumnSize() >= pColumnDesc->ulColumnSize)
		{
			if (	0 != pColumnDesc->ulColumnSize 
				&&	pwszCreateParams
				&&	NULL != wcsstr(pwszCreateParams, L"length"))
				TESTC(pColumnDesc->ulColumnSize == pNewCol->ulColumnSize);
		}
		else
		{
			// column size should be set to the max column size allowed
			TESTC(col.GetColumnSize() == pNewCol->ulColumnSize);
		}
	}
	else
		if (!IsFixedLength(pColumnDesc->wType) && DBTYPE_VARNUMERIC != pNewCol->wType)
			TESTC(pNewCol->ulColumnSize == (ULONG)~0);


	// CHECK THE PRECISION
	if (IsNumericType(pColumnDesc->wType) || DBTYPE_DBTIMESTAMP == pColumnDesc->wType)
	{
		if (0 < pColumnDesc->bPrecision)
			if (	DBTYPE_NUMERIC == pColumnDesc->wType 
				||	DBTYPE_VARNUMERIC == pColumnDesc->wType )
			{
				if (DBTYPE_VARNUMERIC != pColumnDesc->wType ) // 
					TESTC(pColumnDesc->bPrecision == pNewCol->bPrecision);
			}
			else
				TESTC(col.GetPrecision() == pNewCol->bPrecision);
	}
	else
		TESTC(pNewCol->bPrecision == (BYTE)~0);


	// CHECK SCALE
	if (	DBTYPE_NUMERIC == pColumnDesc->wType 
		||	DBTYPE_VARNUMERIC == pColumnDesc->wType 
		||	DBTYPE_DECIMAL == pColumnDesc->wType
		||	DBTYPE_DBTIMESTAMP == pColumnDesc->wType)
	{
		if (DBTYPE_VARNUMERIC != pColumnDesc->wType )
		if (	DBTYPE_DBTIMESTAMP != pColumnDesc->wType
			&&	pColumnDesc->bScale >= col.GetMinScale() 
			&&	pColumnDesc->bScale > col.GetMaxScale())
			TESTC(pColumnDesc->bScale == pNewCol->bScale);
	}
	else 
		TESTC(pNewCol->bScale == (BYTE)~0);	// as set trhough col.SetColInfo

	// get the psid
	if (pColumnDesc->pclsid)
		gCD = *pColumnDesc->pclsid;
	else
		gCD = GUID_NULL;
	
	TESTC(!m_pTable->GetHasCLSID() || gCD == *(GUID*)pNewCol->pclsid);

	pNewPropSet	= &(pNewCol->rgPropertySets[0]); // there is only one prop set in there
	if (NULL == pColumnDesc->rgPropertySets)
	{
		fRes = TRUE;
		goto CLEANUP;
	}
	for (cPropSet=0; cPropSet<(pColumnDesc->cPropertySets); cPropSet++)
	{
		pInPropSet	= &(pColumnDesc->rgPropertySets[cPropSet]);
		
		if (pInPropSet->guidPropertySet != DBPROPSET_COLUMN)
		{
			for (cProp=0; cProp < pInPropSet->cProperties; cProp++)
				TESTC(DBPROPSTATUS_NOTSUPPORTED == pInPropSet->rgProperties[cProp].dwStatus);
			continue;
		}

		// compColumnDescre the propPropSet in the column prop set
		ASSERT(pNewPropSet->rgProperties);
		if (NULL == pInPropSet->rgProperties)
			continue;
		for (cProp=0; cProp < pInPropSet->cProperties; cProp++)
		{
			pInProp = &(pInPropSet->rgProperties[cProp]);
			// look for that property inside the resulted column
			if (((pInProp->dwPropertyID == DBPROP_COL_AUTOINCREMENT)	&& !m_pTable->GetHasAuto()	  )	||
				((pInProp->dwPropertyID == DBPROP_COL_DEFAULT)			&& !m_pTable->GetHasDefault() )	||
				((pInProp->dwPropertyID == DBPROP_COL_UNIQUE)			&& !m_pTable->GetHasUnique())    )
				// better check the status, it should be DBPROPSTATUS_NOTSUPPORTED
				continue;

			TESTC(		SupportedProperty(pInProp->dwPropertyID, pInPropSet->guidPropertySet) 
					||	DBPROPSTATUS_NOTSUPPORTED == pInProp->dwStatus);
			TESTC(		SettableProperty(pInProp->dwPropertyID, pInPropSet->guidPropertySet) 
					||	DBPROPSTATUS_NOTSUPPORTED == pInProp->dwStatus);

			// identify the property
			for (	j=0; 
					j< pNewPropSet->cProperties 
						&& pNewPropSet->rgProperties[j].dwPropertyID != pInProp->dwPropertyID; 
					j++);

			if (j>=pNewPropSet->cProperties)
				continue;

			// try to detect the right dbpropstatus
			TESTC(		(DBPROPSTATUS_OK != pInProp->dwStatus) 
					||	CompareVariant(&pInProp->vValue, &pNewPropSet->rgProperties[j].vValue));
		}
	}
	fRes = TRUE;

CLEANUP:
	return fRes;
} //TCITableDefinition::CompColumnDescreColumnDesc




//--------------------------------------------------------------------
//
// @cmember Compare the list of old columns against the list of new columns and the new column
// the place where the newly added column was added is provider specific!
//--------------------------------------------------------------------
BOOL TCITableDefinition::CheckColInfo
(
	   DBCOLUMNDESC	*oldCD, 
	   ULONG		oldlen, 
	   DBCOLUMNDESC *newCD, 
	   ULONG		newlen, 
	   DBCOLUMNDESC *pColumnDesc	// the new column to be add
)
{
	// when this function is called, it is supposed that m_HasAuto... were set by GetTableColumnInfo
	// and unmodified ever since
	BOOL		fNewColumn=FALSE;
	BOOL		fResult = FALSE;
	ULONG		cOldCol;
	ULONG		cNewCol;

	TESTC(newlen == oldlen+1);

	for (cOldCol=cNewCol=0; cNewCol<newlen; cNewCol++)
	{
		if (	(cOldCol < oldlen)
			&&	CompareDBID(oldCD[cOldCol].dbcid, newCD[cNewCol].dbcid, m_pIDBInitialize))
		{
			TESTC(AreColEqual(&oldCD[cOldCol], &newCD[cNewCol]));
			cOldCol++;
		}
		else
		{
			TESTC(!fNewColumn);
			TESTC(CompareColumnDesc(pColumnDesc, &newCD[cOldCol]));
			fNewColumn = TRUE;
		}
	}
	fResult = TRUE;

CLEANUP:
	return fResult;
} //TCITableDefinition::CheckColInfo




//--------------------------------------------------------------------------
//
// @cmember TCITableDefinition::CheckRowset
//--------------------------------------------------------------------------
BOOL TCITableDefinition::CheckRowset(IUnknown *pIRowset)
{
	IRowsetInfo* pInterface = NULL;
	BOOL	hr;

	if (!pIRowset)
		return TRUE;

	if (hr = DefaultObjectTesting(pIRowset, ROWSET_INTERFACE))
	{
		// just for curiosity
		if (VerifyInterface(	pIRowset, 
								IID_IRowsetInfo, 
								ROWSET_INTERFACE, 
								(IUnknown**)&pInterface)) 
		{
			IUnknown*	tempInterface;
			if (pInterface->GetSpecification(IID_IUnknown, &tempInterface) == S_FALSE)
			{ 
				// what if table ID doesn't specify a name?
				PRVTRACE(L"** No object has created the rowset");
			}
			else
			{
				IUnknown	*pIID;
				if (tempInterface->QueryInterface(IID_IGetDataSource, (void**)&pIID) == S_OK)
				{
					PRVTRACE(L"** Rowset was created by a session");
					SAFE_RELEASE(pIID);
				}
				else
					PRVTRACE(L"** Rowset was created by a command");
				SAFE_RELEASE(tempInterface);
			}
			SAFE_RELEASE(pInterface);
		}
	}
	return hr;
} //CTable::CheckRowset




//--------------------------------------------------------------------------
//
// @cmember Fills in a variant with whatever it needs
//--------------------------------------------------------------------------
BOOL TCITableDefinition::FillVariant(VARIANT *pVariant)
{
	BOOL	fRes = FALSE;
	BSTR	pwszValue = L"\'some value\'";
	WCHAR	*pwszStrValue = L"\'value\'";


	TESTC(NULL != pVariant);
	switch (pVariant->vt)
	{
		case DBTYPE_R4:
			V_R4(pVariant) = (float)3.1415926;
			break;
		case DBTYPE_R8:
			V_R8(pVariant) = 2.71828;
			break;
		case DBTYPE_BSTR:
			V_BSTR(pVariant) = SysAllocString(pwszValue);
			break;
		case DBTYPE_STR:
		case DBTYPE_WSTR:
			pVariant->vt		= DBTYPE_BSTR;
			V_BSTR(pVariant)	= SysAllocString(pwszStrValue);
			break;
		case DBTYPE_BYTES:
			V_BYREF(pVariant) = SysAllocString(pwszStrValue);
			break;
	}
	fRes = TRUE;

CLEANUP:
	return fRes;
} // TCITableDefinition::FillVariant




// @cmember Builds a default value for a given column type
BOOL TCITableDefinition::GetDefaultValue(
	DBCOLUMNDESC	*pColumnDesc,	// [in]		column descriptor	
	VARIANT			*pVariant,		// [out]	pointer to the variant that stores the default value
	CTable			*pTable			// [in]		CTable to be used (default NULL)
)
{
	CTable					*pCTable = NULL;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	CCol					col;
	WCHAR					*pwszDefVal = NULL;
	ULONG					ulRowNum = 28;
	BOOL					fRes = FALSE;

	TESTC(NULL != pColumnDesc && NULL != pColumnDesc->pwszTypeName);
	TESTC(NULL != pVariant);
	if (pTable)
		pCTable = pTable;
	else
	{
		pCTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
		pCTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	}

	pCTable->GetColInfo(col, pColumnDesc->pwszTypeName);
	TESTC_(pCTable->GetLiteralAndValue(col, &pwszDefVal, ulRowNum, col.GetColNum(), PRIMARY), S_OK);
	pVariant->vt = DBTYPE_BSTR;
	V_BSTR(pVariant) = SysAllocString(pwszDefVal);
	SAFE_FREE(pwszDefVal);
	fRes = TRUE;

CLEANUP:
	if (!pTable)
		delete pCTable;
	return fRes;
} // TCITableDefinition::GetDefaultValue




//--------------------------------------------------------------------------
//
// @cmember Get the number of tables in a data sources, base on IDBSchemaRowset
//--------------------------------------------------------------------------
ULONG TCITableDefinition::GetNoOfTables()
{
	IDBSchemaRowset		*pInterface = NULL;
	IRowset				*pRowset = NULL;
	ULONG				n=0;
	HROW *				prghRows = NULL;

	if (!VerifyInterface(m_pIOpenRowset,IID_IDBSchemaRowset, SESSION_INTERFACE, (IUnknown**)&pInterface))
	{
		PRVTRACE(L"** I could not get a pointer to IDBSchemaRowset interface");
		return 0;
	}

	if (!CHECK(pInterface->GetRowset(NULL, DBSCHEMA_TABLES, 0, NULL, IID_IRowset, 0, NULL, (IUnknown**)&pRowset), S_OK))
		goto CLEANUP;

	if (!CHECK(pRowset->GetNextRows(0, 0, LONG_MAX/1024, &n, &prghRows), DB_S_ENDOFROWSET))
		goto CLEANUP;

	if (!CHECK(pRowset->ReleaseRows(n, prghRows, NULL, NULL, NULL), S_OK))
		goto CLEANUP;

CLEANUP:
	SAFE_RELEASE(pRowset);
	SAFE_RELEASE(pInterface);
	PROVIDER_FREE(prghRows);
	return n;
} //TCITableDefinition::GetNoOfTables




//----------------------------------------------------------------------
//
// @cmember Drops a column and check the result against reallity
//----------------------------------------------------------------------
HRESULT	TCITableDefinition::DropColumnAndCheck(
	DBID				*pTableID,					// [in]	the table ID
	DBID				*pColumnID,					// [in]	the ID of the column to be dropped
	ITableDefinition	*pITableDefinition/*=NULL*/	// [in] pointer to ITableDefinition interface
)
{
	DBCOLUMNDESC	*OldColList = NULL, *NewColList = NULL;
	ULONG			oldlen = 0, newlen = 0;
	ULONG			i;
	LONG			nCol = -1L;
	HRESULT			hr;
	BOOL			fTableExists	= FALSE;
	BOOL			fColumnExists	= FALSE;
	IOpenRowset		*pIOpenRowset	= NULL;
	IColumnsInfo	*pIColumnsInfo	= NULL;
	DBORDINAL		ulCol;

	if (!pITableDefinition)
		pITableDefinition = m_pITableDefinition;

	// check whether the table and column exist
	if (pTableID)
	{
		TESTC(VerifyInterface(pITableDefinition, IID_IOpenRowset, SESSION_INTERFACE, (IUnknown**)&pIOpenRowset));
		TEST2C_(hr = pIOpenRowset->OpenRowset(NULL, pTableID, NULL, IID_IColumnsInfo, 
			0, NULL, (IUnknown**)&pIColumnsInfo), S_OK, DB_E_NOTABLE);
		SAFE_RELEASE(pIOpenRowset);
		fTableExists = S_OK == hr;
	}
	else
		fTableExists = FALSE;
	
	
	if (fTableExists && pColumnID) //pTableID && pTableID->uName.pwszName)
	{
		// find out whether the column exists
		TEST2C_(hr = pIColumnsInfo->MapColumnIDs(1, pColumnID, &ulCol), S_OK, DB_E_ERRORSOCCURRED); 
		fColumnExists = S_OK == hr;
		SAFE_RELEASE(pIColumnsInfo);
	}

	// this function is based on the current table name
	if (fTableExists)
		GetColumnDescOnTable(pTableID, &OldColList, &oldlen);

	// the CHECK will be done at the caller level, where the expected return is known
	if ( (hr = m_pITableDefinition->DropColumn(pTableID, pColumnID)) != S_OK)
	{
		// call failed
		if (fTableExists) //pTableID && pTableID->uName.pwszName)
		{
			GetColumnDescOnTable(pTableID, &NewColList, &newlen);
			TESTC(AreColEqual(NewColList, newlen, OldColList, oldlen));
			if (!fColumnExists)
			{
				TESTC_(hr, pColumnID? DB_E_BADCOLUMNID: E_INVALIDARG);
			}
			else
			{
				HRESULT	rgValidHRes[] = {DB_E_DROPRESTRICTED, DB_E_TABLEINUSE, DB_SEC_E_PERMISSIONDENIED, XACT_E_XTIONEXISTS, E_FAIL};

				// check the possible results
				TESTC(CheckResult(hr, NUMELEM(rgValidHRes), rgValidHRes));
			}
		}
		else 
		{
			TESTC_(hr, pTableID? DB_E_NOTABLE: E_INVALIDARG);
		}
	}
	else
	{
		// call succeeded
		TESTC(fColumnExists);
//		if (fTableExists) //pTableID && pTableID->uName.pwszName)
//		{
			GetColumnDescOnTable(pTableID, &NewColList, &newlen);
			for (i = 0, nCol = -1; i < oldlen && nCol < 0; i++)
			{
				if (CompareDBID(OldColList[i].dbcid, *pColumnID, m_pIDBInitialize))
					nCol = (ULONG)i;
			}

			TESTC(0 <= nCol);

			// this should work even when the column name is not in the original (old) table
			TESTC(CheckColInfo(NewColList, newlen, OldColList, oldlen, &OldColList[nCol]));
//		}
	}

CLEANUP:
	SAFE_RELEASE(pIOpenRowset);
	SAFE_RELEASE(pIColumnsInfo);
	ReleaseColumnDesc(OldColList, oldlen);
	ReleaseColumnDesc(NewColList, newlen);
	return hr;
} // TCITableDefinition::DropColumnAndCheck




//--------------------------------------------------------------------------
//
// @cmember Drops a table and check the operation
//--------------------------------------------------------------------------
HRESULT TCITableDefinition::DropTableAndCheck(
	DBID				*pTableID,							// [in] ID of the table to be dropped
	ITableDefinition	*pITableDefinition /*=NULL*/		// [in] pointer to ITableDefinition interface
)
{
	BOOL	fExists;
	BOOL	fTableExisted;
	CTable	*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	HRESULT	hRes;

	ASSERT(pTable);
	if (!pITableDefinition)
		pITableDefinition = m_pITableDefinition;

	// check whether the table existed before
	CHECK(pTable->DoesTableExist(pTableID, &fTableExisted), S_OK);

	// drop the table 
	hRes = pITableDefinition->DropTable(pTableID);

	// check that if table was dropped it doesn't exist
	TESTC(fTableExisted || !SUCCEEDED(hRes));
	TESTC_(pTable->DoesTableExist(pTableID, &fExists), S_OK);
	TESTC(!fTableExisted || fExists == !SUCCEEDED(hRes));

CLEANUP:
	delete pTable;
	return hRes;
} //TCITableDefinition::DropTableAndCheck




//--------------------------------------------------------------------------
//
// GetInfo about table identifiers
//--------------------------------------------------------------------------
HRESULT	TCITableDefinition::GetLiteralInfo()
{
	IDBInfo*		pInterface = NULL;
	DBLITERAL		rgLiteral[2]={DBLITERAL_TABLE_NAME, DBLITERAL_COLUMN_NAME};
	DBLITERALINFO*	rgLiteralInfo = NULL;
	ULONG			cLiteralInfo, i;
	OLECHAR*		pCharBuffer = NULL;
	IGetDataSource*	pIGetDataSource=NULL;	// IGetDataSource interface pointer

	m_fIsIDBInfo = FALSE;

	if(!VerifyInterface(m_pIOpenRowset, IID_IGetDataSource, SESSION_INTERFACE, (IUnknown**)&pIGetDataSource))
		return E_FAIL;

	m_hr=pIGetDataSource->GetDataSource(IID_IDBInfo,(IUnknown**)&pInterface);
	SAFE_RELEASE(pIGetDataSource);

	if (!CHECK(pInterface->GetLiteralInfo(2, rgLiteral, &cLiteralInfo, &rgLiteralInfo, &pCharBuffer), S_OK))
		goto CLEANUP;

	for (i=0; i< cLiteralInfo; i++)
	{
		switch (rgLiteralInfo[i].lt)
		{
		case DBLITERAL_TABLE_NAME:
			// get the maximum size of a valid table name and the invalid chars for a table name
			m_cMaxTableName = rgLiteralInfo[i].cchMaxLen;
			PROVIDER_FREE(m_pwszInvalidTableChars);
			m_pwszInvalidTableChars = wcsDuplicate(rgLiteralInfo[i].pwszInvalidChars?
				rgLiteralInfo[i].pwszInvalidChars: L"");
			break;
		case DBLITERAL_COLUMN_NAME:
			m_cMaxColName = rgLiteralInfo[i].cchMaxLen;
			PROVIDER_FREE(m_pwszInvalidColChars);
			m_pwszInvalidColChars = wcsDuplicate(rgLiteralInfo[i].pwszInvalidChars?
				rgLiteralInfo[i].pwszInvalidChars: L"");
			break;
		}
	}

	m_fIsIDBInfo = TRUE;

CLEANUP:
	SAFE_RELEASE(pInterface);
	PROVIDER_FREE(pCharBuffer);
	PROVIDER_FREE(rgLiteralInfo);
	return m_hr;
} //TCITableDefinition::GetLiteralInfo





// @cmember Gets the catalog and schema name for a given table
BOOL	TCITableDefinition::GetCatalogSchemaNames(
	LPWSTR	pwszTableName,		// [in]		the name of the table
	LPWSTR	*ppwszCatalogName,	// [out]	catalog name
	LPWSTR	*ppwszSchemaName,	// [out]	schema name
	CTable	*pTable				// [in]		table object (DEFAULT NULL - use m_pTable)
)
{
    IRowset		*pIRowset = NULL;
	HROW		hRow = DB_NULL_HROW;
	BOOL		bFound = FALSE;
	WCHAR		*pwszTableName1 = NULL;
	CRowset		Rowset;

	if (!pTable)
		pTable = m_pTable;
	TESTC(NULL != pwszTableName);
	TESTC(NULL != ppwszCatalogName);
	TESTC(NULL != ppwszSchemaName);

	//Obtain Schema TABLES Rowset
	//We don't want to put any restrictions, since its not required to support 
	//restrictions...
	TESTC_PROVIDER((Rowset.CreateRowset(SELECT_DBSCHEMA_TABLE, 
		IID_IRowset, pTable, DBACCESSOR_ROWDATA, DBPART_ALL, ALL_COLS_BOUND)==S_OK));
	
	//Try to find the specified row with this table...
	while(!bFound && S_OK == (m_hr = Rowset.GetNextRows(&hRow)))
	{
		//GetData for this row
		TESTC_(Rowset.GetRowData(hRow, &Rowset.m_pData),S_OK);

		//TABLE_NAME column
		DBBINDING* pCatalogBinding	= Rowset.m_rgBinding[0].iOrdinal == 0 ? &Rowset.m_rgBinding[1] : &Rowset.m_rgBinding[0];
		DBBINDING* pSchemaBinding	= Rowset.m_rgBinding[0].iOrdinal == 0 ? &Rowset.m_rgBinding[2] : &Rowset.m_rgBinding[1];
		DBBINDING* pTableBinding	= Rowset.m_rgBinding[0].iOrdinal == 0 ? &Rowset.m_rgBinding[3] : &Rowset.m_rgBinding[2];

		//See if this is the correct row...
		if(STATUS_BINDING(*pTableBinding, Rowset.m_pData)==DBSTATUS_S_OK)
		{
			//TableName
			pwszTableName1 = (WCHAR*)&VALUE_BINDING(*pTableBinding, Rowset.m_pData);
			bFound = FALSE;
			if(CHECK(CompareID(&bFound, pwszTableName, 
					pwszTableName1, m_pIDBInitialize), S_OK) && bFound)
			{	
				//Catalog Name
				if(STATUS_BINDING(*pCatalogBinding, Rowset.m_pData)==DBSTATUS_S_OK)
					*ppwszCatalogName = wcsDuplicate((WCHAR*)&VALUE_BINDING(*pCatalogBinding, Rowset.m_pData));
				//Schema Name
				if(STATUS_BINDING(*pSchemaBinding, Rowset.m_pData)==DBSTATUS_S_OK)
					*ppwszSchemaName = wcsDuplicate((WCHAR*)&VALUE_BINDING(*pSchemaBinding, Rowset.m_pData));
			}
		}

		Rowset.ReleaseRows(hRow);
		hRow = DB_NULL_HROW;
	}

	//Error
	if(!bFound)
	{
		TERROR("Unable to find the table \"" << pwszTableName << "\"in the Schema Rowset?");
		QTESTC(bFound);
		return FALSE;
	}
CLEANUP:
	if (DB_NULL_HROW != hRow)
		Rowset.ReleaseRows(hRow);
	SAFE_RELEASE(pIRowset);
	return TRUE;
} //TCITableDefinition::GetCatalogSchemaNames




//--------------------------------------------------------------------------
//
// @cmember Build a valid table name of a certain length
//--------------------------------------------------------------------------
WCHAR* TCITableDefinition::BuildValidName(ULONG length, WCHAR* pattern)
{
	WCHAR*	pwszBuffer = NULL;
	ULONG	cLen = wcslen(pattern), i;

	SAFE_ALLOC(pwszBuffer, WCHAR, i=(length+1));
	memset(pwszBuffer, 0, i);
	for (i=0; i< length - cLen; i+=cLen)
		wcscat(pwszBuffer, pattern);
	// manage the rest of the characters
	wcsncat(pwszBuffer, pattern, length-i);
CLEANUP:
	return pwszBuffer;
} //TCITableDefinition::BuildValidName




//--------------------------------------------------------------------------
//
// @cmember Build an invalid table name of a certain length
// the pattern is supposed to be shorter than the string to be build
//--------------------------------------------------------------------------
WCHAR* TCITableDefinition::BuildInvalidName(ULONG length, WCHAR* pattern, WCHAR* invchars)
{
	WCHAR*	pwszBuffer=NULL;
	ULONG	cLen = wcslen(pattern), i, cInvLen=wcslen(invchars);

	SAFE_ALLOC(pwszBuffer, WCHAR, length+1);
	if (length > cInvLen+cLen)
	{
		wcscpy(pwszBuffer, pattern);
		wcscat(pwszBuffer, invchars);
	}
	else
	{
		wcscpy(pwszBuffer, L"aa");
		wcsncat(pwszBuffer, invchars, length-wcslen(pwszBuffer));
		return pwszBuffer;
	}
	for (i=wcslen(pwszBuffer); i< length - cLen; i+=cLen)
		wcscat(pwszBuffer, pattern);
	// manage the rest of the characters
	for (; i<length; i++)
		wcscat(pwszBuffer, L"a");

CLEANUP:
	return pwszBuffer;
} //TCITableDefinition::BuildInvalidName




//--------------------------------------------------------------------------
//
// @cmember release the property of all columns
//--------------------------------------------------------------------------
void TCITableDefinition::ReleaseAllColumnPropSets(
	DBCOLUMNDESC	*rgColumnDesc/*=NULL*/,	// [in]	aray to be release
	ULONG			cColumnDesc/*=0*/		// [in] no of elementrs in the array
)
{
	ULONG	i;

	if (NULL == rgColumnDesc)
		rgColumnDesc = m_rgColumnDesc;
	if (0 == cColumnDesc)
		cColumnDesc = m_cColumnDesc;
	for (i=0; i<cColumnDesc; i++)
	{
		FreeProperties(&rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		ASSERT(0 == rgColumnDesc[i].cPropertySets);
		ASSERT(NULL == rgColumnDesc[i].rgPropertySets);
	}
} //TCITableDefinition::ReleaseAllColumnPropSets




//---------------------------------------------------------------------
//
// @cmember Uses m_pTable to get a column description for all types
//---------------------------------------------------------------------
void TCITableDefinition::GetProviderTypes(void)
{
	CList <WCHAR*, WCHAR*>	ListNativeTemp;
	CList <DBTYPE, DBTYPE>	ListDataTypes;

	if (m_pTable)
		delete m_pTable;

	m_pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	// get the provider types list
	// all types appear in CreateColList, so I don't have to use CreateTypeColInfo
	m_pTable->CreateTypeColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	m_pTable->DuplicateColList(m_ColList);
	// in m_rgColumnDesc I would rather load just the types prepared for table creation
	m_pTable->CreateColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	// build the column description array
	m_pTable->BuildColumnDescs(&m_rgColumnDesc);
	m_cColumnDesc = m_pTable->CountColumnsOnTable();
	m_pTable->SetBuildColumnDesc(FALSE);
	ReleaseAllColumnPropSets();
	delete m_pTable;
	m_pTable = NULL;
} //TCITableDefinition::GetProviderTypes




//---------------------------------------------------------------------
//
// @cmember Sets all the columns to maximum size as got from m_ColList 
// (based on information read from PROVIDER_TYPES Rowset)
//
// RESULT:
//		TRUE	- set for every element in column desc array
//		FALSE	- could not be set for every element in thee array
//---------------------------------------------------------------------
BOOL TCITableDefinition::SetMaximumColSize
(
	DBCOLUMNDESC	*rgColumnDesc, 
	ULONG			cColumnDesc
)
{
	POSITION	pos;
	CCol		col;
	ULONG		i;

	for (i=0; i<cColumnDesc; i++)
	{
		// look for the type which fits with column type
		for (	pos=m_ColList.GetHeadPosition(), col=m_ColList.GetNext(pos);
				wcscmp(col.GetProviderTypeName(), rgColumnDesc[i].pwszTypeName) && pos; 
				col=m_ColList.GetNext(pos));
		if (wcscmp(col.GetProviderTypeName(), rgColumnDesc[i].pwszTypeName) )
			continue;
		// set to max column size
		rgColumnDesc[i].ulColumnSize = col.GetColumnSize();
	}
	return TRUE;
} //TCITableDefinition::SetMaximumColSize




//--------------------------------------------------------------------------
//
// @cmember Sets all the column sizes to 0
// RESULT:
//	TRUE	- set for every element in column desc array
//	FALSE	- could not be set for every element in thee array
//--------------------------------------------------------------------------
BOOL TCITableDefinition::SetZeroColSize
(
	DBCOLUMNDESC	*rgColumnDesc, 
	ULONG			cColumnDesc
)
{
	ULONG		i;

	for (i=0; i<cColumnDesc; i++)
	{
		// set column size to 0
		rgColumnDesc[i].ulColumnSize = 0;
	}
	return TRUE;
} //TCITableDefinition::SetZeroColSize




//--------------------------------------------------------------------------
//
// @cmember CheckReturnedDBID about the columns in the table
//	pIndexID		-	[IN] the DBID asked for
//	ppIndexID		-	[IN] the DBID created
//--------------------------------------------------------------------------
BOOL TCITableDefinition::CheckReturnedDBID(
	DBID	*pIndexID,		// passed to CreateIndex
	DBID	**ppIndexID		// returned by CreateIndex
)
{
	if (!pIndexID)
		return *ppIndexID != NULL;

	if (!ppIndexID)
	{
		return TRUE;
	}

	if (*ppIndexID == NULL)
		return FALSE;

	if (pIndexID->eKind != (*ppIndexID)->eKind)	
		return TRUE;

	return CompareDBID(*pIndexID, **ppIndexID);
} //TCITableDefinition::CheckReturnedDBID




//--------------------------------------------------------------------------
//
// @cmember GetInfo about the columns in the table
//	prgInfo			-	[OUT] the array of column information
//	pcColumns		-	[OUT] the size of the array
//	ppStringBuffer	-	[OUT] memory for info in the array
//--------------------------------------------------------------------------
BOOL TCITableDefinition::GetColumnInfo
(
	DBID			*pTableID, 
	ULONG			*pcColumns, 
	DBCOLUMNINFO	**prgInfo, 
	OLECHAR			**ppStringBuffer
)
{
	IColumnsInfo	*pIColumnsInfo;

	if ( (m_hr = m_pIOpenRowset->OpenRowset(NULL, pTableID, NULL, IID_IColumnsInfo, 
				0, NULL, (IUnknown**)&pIColumnsInfo)) != S_OK)
	{
		*prgInfo			= NULL;
		*pcColumns			= 0;
		*ppStringBuffer	= NULL;
		return FALSE;
	}

	if (!CHECK(m_hr = pIColumnsInfo->GetColumnInfo(pcColumns, prgInfo, ppStringBuffer), S_OK))
		return FALSE;

	SAFE_RELEASE(pIColumnsInfo);
	return TRUE;
} //TCITableDefinition::GetColumnInfo




//---------------------------------------------------------------------
//
// @cmember Get an array od Column Descriptors on a Table
//---------------------------------------------------------------------
BOOL TCITableDefinition::GetColumnDescOnTable(DBID *pTableID, DBCOLUMNDESC **prgColumnDesc, ULONG *cColumnDesc)
{
	if (!m_pTable)
		return FALSE;

	// get info in m_pTable->m_ColList
	m_pTable->GetTableColumnInfo(pTableID);
	m_pTable->AddInfoFromColumnsSchemaRowset(m_pITableDefinition, 
		pTableID || DBKIND_NAME != pTableID->eKind? pTableID->uName.pwszName: NULL);
	m_pTable->ColList2ColumnDesc(prgColumnDesc, cColumnDesc);
	return TRUE;
} //TCITableDefinition::GetColumnDescOnTable




//---------------------------------------------------------------------
//
// @cmember Passes a type name and retrieves the column from m_ColList with info about that type
//---------------------------------------------------------------------
CCol TCITableDefinition::GetType(WCHAR *pwszTypeName, DBTYPE wType, BOOL *fExists)
{
	POSITION	pos;
	CCol		col;
	BOOL		fRes = FALSE;

	for (pos = m_ColList.GetHeadPosition(); pos && !fRes; )
	{
		col		= m_ColList.GetNext(pos);
		fRes	=	(0 == wcscmp(col.GetProviderTypeName(), pwszTypeName))
				&&	(wType == col.GetProviderType());
	}

	if (fExists)
		*fExists = fRes;

	return fRes? col : m_ColList.GetHead();
} //TCITableDefinition::GetType




//------------------------------------------------------------------------
//
// @mfunc Check whether the array of ColumnDesc sent was preserved
// @rdesc TRUE or FALSE
// All the data should be inside, the only thing that is allowed to be modified <nl>
// is the property status
//------------------------------------------------------------------------
BOOL TCITableDefinition::IsColumnDescPreserved
(
	DBCOLUMNDESC	*rgInColumnDesc,	// the array passed to ITableDefinition::CreateTable
	DBCOLUMNDESC	*rgOutColumnDesc,	// the aray returned by ITableDefinition::CreateTable
	ULONG			cColumnDesc			// the size of the arrays
)
{
		// when this function is called, it is supposed that m_HasAuto... were set by GetTableColumnInfo
	// and unmodified ever since
	ULONG		i, cPropSets, cProp;
	DBPROPSET	*pInPropSet, *pOutPropSet;

	if (NULL == rgInColumnDesc || NULL == rgOutColumnDesc)
		return (0 == cColumnDesc) || (rgOutColumnDesc == rgInColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		// compare everything in the stru
		if (!CHECK(rgInColumnDesc[i].pTypeInfo == rgOutColumnDesc[i].pTypeInfo, TRUE))
			return FALSE;
		if (!CHECK(rgInColumnDesc[i].ulColumnSize == rgOutColumnDesc[i].ulColumnSize, TRUE))
			return FALSE;
		if (!CHECK(rgInColumnDesc[i].wType == rgOutColumnDesc[i].wType, TRUE))
			return FALSE;
		if (!CHECK(rgInColumnDesc[i].bPrecision == rgOutColumnDesc[i].bPrecision, TRUE))
			return FALSE;
		if (!CHECK(rgInColumnDesc[i].bScale == rgOutColumnDesc[i].bScale, TRUE))
			return FALSE;
		if (!CHECK(rgInColumnDesc[i].pclsid == rgOutColumnDesc[i].pclsid, TRUE))
			return FALSE;
		if (INVALID_DBKIND != rgInColumnDesc[i].dbcid.eKind)
		{
			// CompareDBID with NULL DSO interface since the ids should
			// match exactly
			if (!CHECK(m_hr = CompareDBID(rgInColumnDesc[i].dbcid, 
				rgOutColumnDesc[i].dbcid, NULL), TRUE))
				return FALSE;
		}
		else
			if (INVALID_DBKIND != rgOutColumnDesc[i].dbcid.eKind)
				return FALSE;
		// compare the properties
		if (!CHECK(rgInColumnDesc[i].cPropertySets == rgOutColumnDesc[i].cPropertySets, TRUE))
			return FALSE;
		if (	(NULL == rgInColumnDesc[i].rgPropertySets)
			||	(NULL == rgOutColumnDesc[i].rgPropertySets))
			if (	0 != rgOutColumnDesc[i].cPropertySets 
				&&	!COMPARE(rgInColumnDesc[i].rgPropertySets, rgOutColumnDesc[i].rgPropertySets))
				return FALSE;
			else
				continue;
		for (cPropSets=0; cPropSets<rgInColumnDesc[i].cPropertySets; cPropSets++)
		{
			// compare the props in that prop set
			pInPropSet	= &(rgInColumnDesc[i].rgPropertySets[cPropSets]);
			pOutPropSet	= &(rgOutColumnDesc[i].rgPropertySets[cPropSets]);

			if (!CHECK(pInPropSet->guidPropertySet == pOutPropSet->guidPropertySet, TRUE))
				return FALSE;
			if (!CHECK(pInPropSet->cProperties == pOutPropSet->cProperties, TRUE))
				return FALSE;
			if (	(NULL == pInPropSet->rgProperties)
				||	(NULL == pOutPropSet->rgProperties))
				if (0 != pOutPropSet->cProperties 
					&& !COMPARE(pInPropSet->rgProperties, pOutPropSet->rgProperties))
					return FALSE;
				else
					continue;
			
			// compare the properties
			for (cProp=0; cProp<pInPropSet->cProperties; cProp++)
			{
				if (!CHECK(pInPropSet->rgProperties[cProp].dwPropertyID == 
						   pOutPropSet->rgProperties[cProp].dwPropertyID, TRUE))
					return FALSE;
				if (!CHECK(pInPropSet->rgProperties[cProp].dwOptions == 
						   pOutPropSet->rgProperties[cProp].dwOptions, TRUE))
					return FALSE;
				// use NULL fr DSO interface
				if (!CHECK(m_hr = CompareDBID(pInPropSet->rgProperties[cProp].colid, 
						   pOutPropSet->rgProperties[cProp].colid, 
						   NULL), TRUE))
					return FALSE;
				// protection against assertion errors in CompareVariant
				if (
					 (rgInColumnDesc[i].wType != DBTYPE_GUID)	&&
					 (rgInColumnDesc[i].wType != DBTYPE_BYTES)	&&
					 (rgInColumnDesc[i].wType != DBTYPE_STR)	&&
					 (rgInColumnDesc[i].wType != DBTYPE_WSTR)	&&
					 (rgInColumnDesc[i].wType != DBTYPE_NUMERIC)	&&
					 (rgInColumnDesc[i].wType != DBTYPE_DBTIMESTAMP)
					)
					if (!CHECK(CompareVariant(&pInPropSet->rgProperties[cProp].vValue,
							   &pOutPropSet->rgProperties[cProp].vValue), TRUE))
						return FALSE;
			}
		}
	}
	return TRUE;
} //TCITableDefinition::IsColumnDescPreserved
				



//---------------------------------------------------------------------
//
// @cmember Set a property in the property sets of a DBCOLUMNDESC
//---------------------------------------------------------------------
BOOL TCITableDefinition::SetProperty
(
	DBPROPSET		**prgPropertySets,					// [in/out]		array of property sets
	ULONG			*pcPropertySets,					// [in/out]		number of property sets
	DBPROPID		propID,								// [in]			property ID
	DBTYPE			wType,								// [in]			value type
	LPVOID			value,								// [in]			property value
	DBPROPOPTIONS	dwOption/*=DPROPOPTIONS_OPTIONAL*/,	// [in]			prop options
	DBID			colid/*=DB_NULLID*/,				// [in]			col id
	GUID			guidPropertySet/*DBPROPSET_COLUMN*/,// [in]			the GUID of the propset
	ULONG			*pcPropSet/*=NULL*/,				// [out]		index of property set 
	ULONG			*pcProp/*=NULL*/					// [out]		index in the rgProperties array
)
{
	LONG		i;
	DBPROPSET	*pPropSet;
	BOOL		fRes = FALSE, fFound;
	ULONG		cPropSet, cProp;

	if (wType != DBTYPE_VARIANT)
	{
		TESTC(::SetProperty(propID, guidPropertySet, pcPropertySets, prgPropertySets, value,
			wType, dwOption, colid));
	}
	else
	{
		TESTC(::SetProperty(propID, guidPropertySet, pcPropertySets, prgPropertySets, value,
			VT_I2, dwOption, colid));
	}

	
	fRes = TRUE;

	for (i=(int)(*pcPropertySets - 1), fFound = FALSE; i >= 0 && !fFound; i--)
	{
		fFound = (*prgPropertySets)[i].guidPropertySet == guidPropertySet;
		cPropSet = (ULONG)i;
	}

	TESTC(fFound);

	pPropSet = &(*prgPropertySets[cPropSet]);
	for (i=pPropSet->cProperties - 1, fFound = FALSE; i >= 0 && !fFound; i--)
	{
		if (TRUE == (fFound = pPropSet->rgProperties[i].dwPropertyID == propID))
		{
			cProp = (ULONG)i;
			if (DBTYPE_VARIANT == wType)
			{
				pPropSet->rgProperties[i].vValue.vt = ((VARIANT*)value)->vt;
				FillVariant(&pPropSet->rgProperties[i].vValue);
			}
		}
	}

	if (pcPropSet)
		*pcPropSet = cPropSet;

	if (pcProp)
		*pcProp = cProp;

	TESTC(fFound);
	fRes = TRUE;

CLEANUP:
	return fRes;
} //TCITableDefinition::SetProperty




//---------------------------------------------------------------------------
//
// @cmember Set a default property in the property sets of a DBCOLUMNDESC
//---------------------------------------------------------------------------
BOOL TCITableDefinition::SetDefaultProperty(
	DBPROPSET		**prgPropertySets,						// [in/out]		array of property sets
	ULONG			*pcPropertySets,						// [in/out]		number of property sets
	VARIANT			*pvalue,								// [in]			property value
	DBPROPOPTIONS	dwOption /*= DBPROPOPTIONS_OPTIONAL*/,	// [in]			prop options
	DBID			colid /*= DB_NULLID*/,					// [in]			col id
	GUID			guidPropertySet /*= DBPROPSET_COLUMN*/,	// [in]			the GUID of the propset
	ULONG			*pcPropSet /*= NULL*/,					// [out]		index of property set 
	ULONG			*pcProp /*=NULL*/						// [out]		index in the rgProperties array
)
{
	BOOL	fRes = FALSE;
	ULONG	cPropSet;
	ULONG	cProp;

	TESTC(NULL != pvalue);
	TESTC(SetProperty(prgPropertySets, pcPropertySets, DBPROP_COL_DEFAULT, 
		DBTYPE_I2, 0, dwOption, colid, DBPROPSET_COLUMN, &cPropSet, &cProp));

	VariantCopy(&(prgPropertySets[cPropSet]->rgProperties[cProp].vValue), pvalue);
	if (pcPropSet)
		*pcPropSet = cPropSet;
	if (pcProp)
		*pcProp = cProp;
	fRes = TRUE;

CLEANUP:
	return fRes;
} // TCITableDefinition::SetDefaultProperty




//---------------------------------------------------------------------------
// TCITableDefinition::Insert
//
// CTable				|
// Insert				|
// Inserts 1 row in table using IRowsetNewRow. This function is called
// by the pulic Insert function.
//
// @mfunc	Insert
// @rdesc HRESULT indicating success or failure
// 	@flag S_OK		| Function ran without problem
// 	@flag E_FAIL	| No Columns Marked for use or SetNewData failed
//
//---------------------------------------------------------------------------
HRESULT TCITableDefinition::Insert(			
	ULONG 			ulRowNumber,		// @parm [IN]	Row number to insert	(0 for next)		
	IUnknown		*pIUnknown,			// @parm [IN]	Pointer to rowset interface
	ULONG			cNulls,				// @parm [IN]	Number of columns to be set to NULL (DEFAULT 0)
	ULONG			*rgNulls,			// @parm [IN]	Array of columns to be set to NULL (DEFAULT NULL)
	CTable			*pTable				// @parm [IN]	Table (DEFAULT NULL - use m_pTable)
)
{
	HRESULT				hr				= E_FAIL;
	HACCESSOR			hAccessor		= NULL;
	BYTE*				pData			= NULL;
	ULONG				cBindings		= 0;
	DBBINDING*			rgBindings		= NULL;
	IDBProperties*		pIDBProperties	= NULL;
	IGetDataSource*		pIGetDataSource	= NULL;
	ULONG				nBinding, nNull;
	BOOL				fFound;
	IRowsetChange		*pIRowsetChange	= NULL;
	IAccessor			*pIAccessor		= NULL;
	ULONG				cRefCounts		= ULONG_MAX;

	if (!pTable)
		pTable = m_pTable;

	// Find row number to use on insert
	if (ulRowNumber == 0)
		ulRowNumber = pTable->GetNextRowNumber();

	TESTC(NULL != pIUnknown);
	TESTC(VerifyInterface(pIUnknown, IID_IAccessor, ROWSET_INTERFACE, (IUnknown**)&pIAccessor));
	TESTC(VerifyInterface(pIUnknown, IID_IRowsetChange, ROWSET_INTERFACE, (IUnknown**)&pIRowsetChange));

	//Get the DataSource
	TESTC(VerifyInterface(m_pIOpenRowset, IID_IGetDataSource, SESSION_INTERFACE, (IUnknown**)&pIGetDataSource));
	TESTC_(hr = pIGetDataSource->GetDataSource(IID_IDBProperties, (IUnknown**)&pIDBProperties), S_OK);

	//Get an accessor with all the writeable columns
	TESTC_(hr = GetAccessorAndBindings(pIRowsetChange, 
		DBACCESSOR_ROWDATA,
		&hAccessor, &rgBindings, &cBindings, NULL,
		DBPART_VALUE |DBPART_STATUS |DBPART_LENGTH,
		UPDATEABLE_COLS_BOUND, FORWARD, NO_COLS_BY_REF, NULL,
		NULL, NULL,	DBTYPE_EMPTY, 0, NULL), S_OK);

	// Fill buffer with appropriate data for insert of this row number
	TESTC_(hr = FillInputBindings(
		pTable,
		DBACCESSOR_ROWDATA,		
		cBindings,
		rgBindings,	
		&pData,		
		ulRowNumber,						
		0,
		NULL),S_OK);

	// set the NULL columns
	for (nBinding = 0; nBinding < cBindings; nBinding++)
	{
		for (fFound = FALSE, nNull = 0; nNull < cNulls && !fFound; nNull++)
		{
			if (rgBindings[nBinding].iOrdinal == rgNulls[nNull])
			{
				*(DBSTATUS*)(pData+rgBindings[nBinding].obStatus) = DBSTATUS_S_ISNULL;
				fFound = TRUE;
			}
		}
	}

	// release previous accessor and create a new one
	hr = pIRowsetChange->InsertRow(NULL, hAccessor, pData, NULL);

	if (DB_E_ERRORSOCCURRED == hr)
	{	
		SHORT	*pMyData;

		for (ULONG i=0; i<cBindings; i++)
		{
			pMyData = (SHORT*)(pData+rgBindings[i].obStatus);
			if (DBSTATUS_S_OK != *pMyData && DBSTATUS_E_UNAVAILABLE != *pMyData)
				odtLog << "bindings " << i << "\n";
		}

	}
	// We have successfully inserted if we got this far
	pTable->AddRow(); //increments pTable->m_ulNextRow

CLEANUP:
	if(hAccessor && pIAccessor)
	{
		pIAccessor->ReleaseAccessor(hAccessor, &cRefCounts);
		COMPARE(cRefCounts,0);
	}

	SAFE_RELEASE(pIAccessor);
	FreeAccessorBindings(cBindings, rgBindings);
	PROVIDER_FREE(pData);

	SAFE_RELEASE(pIRowsetChange);
	SAFE_RELEASE(pIGetDataSource);
	SAFE_RELEASE(pIDBProperties);
	return hr;
} //TCITableDefinition::Insert




//--------------------------------------------------------------------------
//
// @cmember StartTransaction method
//	RETURN
//		S_OK	- an isolation level could be set and a transaction started
//		E_FAIL	- could not start a transaction
//--------------------------------------------------------------------------
HRESULT TCITableDefinition::StartTransaction(ITransactionLocal *pITransactionLocal)
{
	ULONG		n, i;
	HRESULT		hr;
	ISOLEVEL	rgIsoLevel[] = {ISOLATIONLEVEL_BROWSE, ISOLATIONLEVEL_CURSORSTABILITY,
								ISOLATIONLEVEL_ISOLATED, ISOLATIONLEVEL_CHAOS};

	if (NULL == pITransactionLocal)
		return E_FAIL;
	
	n = NUMELEM(rgIsoLevel);
	for (i=0; i<n; i++)
	{
		if (S_OK == (hr = pITransactionLocal->StartTransaction(rgIsoLevel[i], 0, NULL, NULL)))
			return S_OK;
	}
	return E_FAIL;
} //TCITableDefinition::StartTransaction




//---------------------------------------------------------------------
//
//	TCITableDefinition::AddColumnAndCheck
//
// Add a column to the table and check the result
// RETURN value is the ret value of ITableDefinition::AddColumn
//---------------------------------------------------------------------
HRESULT TCITableDefinition::AddColumnAndCheck(
		DBID				*pTableID,					// [IN]  table ID
		DBCOLUMNDESC		*pColumnDesc,				// [IN]  descriptor of the column to be added
		DBID				**ppColumnID,				// [OUT] column ID for the resulted column
		ITableDefinition	*pITableDefinition/*= NULL*/// [IN]  pointer to ITableDefinition 
)
// sometimes there is not a unique expected result (e.g. one property could be supported or not)
// using Validate == FALSE you can protect the code from getting errors
{
	DBCOLUMNDESC	*rgOldCD				= NULL;
	ULONG			nOldlen					= 0;
	DBCOLUMNDESC	*rgNewCD				= NULL;
	ULONG			nNewlen					= 0;
	DBCOLUMNDESC	*pOriginalColumnDesc	= NULL;
	BOOL			fRes					= TRUE;
	HRESULT			hr						= E_FAIL;

	pITableDefinition = (NULL == pITableDefinition) ? m_pITableDefinition: pITableDefinition;
	TESTC(NULL != pITableDefinition);

	GetColumnDescOnTable(pTableID, &rgOldCD, &nOldlen);
	if (ppColumnID)
		*ppColumnID = NULL;

	CHECK(NULL == pColumnDesc
		|| NULL != DuplicateColumnDesc(pColumnDesc, 1, &pOriginalColumnDesc), TRUE);

	hr = pITableDefinition->AddColumn(pTableID, pColumnDesc, ppColumnID);

	if (	pOriginalColumnDesc
		&&	!COMPARE(IsColumnDescPreserved(pOriginalColumnDesc, pColumnDesc, 1), TRUE))
	{
		odtLog << "\tERROR: column desc is not preserved\n";
	}

	// check the status of the column properties
	COMPARE(CheckColPropStatus(1, pColumnDesc, hr), TRUE);

	if (FAILED(hr) && ppColumnID)
		COMPARE(NULL == *ppColumnID, TRUE);
	
	// get the new stru of the table
	GetColumnDescOnTable(pTableID, &rgNewCD, &nNewlen);

	// check the result of the operation
	if (SUCCEEDED(hr))
		// get info about the columns in the altered table and compare them, one by one!
		COMPARE(CheckColInfo(rgOldCD, nOldlen, rgNewCD, nNewlen, pColumnDesc), TRUE);
	else
		// make sure that the table stru remained the same
		COMPARE(AreColEqual(rgOldCD, nOldlen, rgNewCD, nNewlen), TRUE);

CLEANUP:
	// save old values into the new ones and release the others
	ReleaseColumnDesc(rgOldCD, nOldlen);
	ReleaseColumnDesc(rgNewCD, nNewlen);
	ReleaseColumnDesc(pOriginalColumnDesc, 1);
	return hr;
} //TCITableDefinition::AddColumnAndCheck




//---------------------------------------------------------------------
//
// @member Create the table using ITableDefinition::CreateTable and 
// check its creation
//---------------------------------------------------------------------
HRESULT TCITableDefinition::CreateAndCheckTable(
	ITableDefinition	*pITableDefinition,	// [in] pointer to the ITableDefinition interface to be used
	IUnknown			*pUnkOuter,			// [in] pointer to controlling unknown
	DBID				*pTableID,			// [in]	pointer to the input table ID
	ULONG				cColumnDescs,		// [in]	the number of columns to be created
	DBCOLUMNDESC		*rgColumnDescs,		// [in/out] array of columns to be created
	REFIID				riid,				// [in] rowset interface to be returned
	ULONG				cPropertySets,		// [in] number of roset property sets asked
	DBPROPSET			*rgPropertySets,	// [in/out] array of rowset property sets
	DBID				**ppTableID,		// [out] returned table ID
	IUnknown			**ppRowset			// [out] pointer to outer interface
)
{
	BOOL				fExists;
	ULONG				cOriginalColumnDescs = 0;
	DBCOLUMNDESC		*rgOriginalColumnDescs = NULL;
	ULONG				cOriginalPropertySets = 0;
	DBPROPSET			*rgOriginalPropertySets = NULL;
	HRESULT				hres;
	DBID				*pNewTableID = NULL;
	CTable				*pTable = new CTable(pITableDefinition, (LPWSTR)gwszModuleName);
	IUnknown			*pIUnknown = NULL; // used to pass the rowset pointer to AreColumnsCreatedOK

	ASSERT(pTable);
	ASSERT(pITableDefinition);
	
	// save the original values 
	CHECK(0 == cColumnDescs || NULL == rgColumnDescs
		|| NULL != DuplicateColumnDesc(rgColumnDescs, cColumnDescs, &rgOriginalColumnDescs), TRUE);
	cOriginalColumnDescs = cColumnDescs;
	CHECK(0 == cPropertySets || NULL == rgPropertySets 
		|| DuplicatePropertySets(cPropertySets, rgPropertySets, &cOriginalPropertySets, &rgOriginalPropertySets), TRUE);

	// go for ITableDefinition::CreateTable
	hres = pITableDefinition->CreateTable(pUnkOuter, pTableID, cColumnDescs, rgColumnDescs, riid, 
		cPropertySets, rgPropertySets, ppTableID, ppRowset);

	if (SUCCEEDED(hres))
	{
		COMPARE(NULL == ppTableID || NULL != *ppTableID, TRUE);
	}
	else
	{
		COMPARE(NULL == ppTableID || NULL == *ppTableID, TRUE);
	}

	if (rgOriginalColumnDescs 
		&& !COMPARE(IsColumnDescPreserved(rgOriginalColumnDescs, rgColumnDescs, cColumnDescs), TRUE))
	{
		odtLog << "\tERROR: array of column desc is not preserved\n";
	}

	if (	rgOriginalPropertySets
		&&	!COMPARE(IsRowsetPropSetPreserved(rgOriginalPropertySets, rgPropertySets, cPropertySets), TRUE))
	{
		odtLog << "\tERROR: the rowset property sets are not preserved\n";
	}

	COMPARE(CheckRwstTblPropStatus(cPropertySets, rgPropertySets, hres), TRUE);
	COMPARE(CheckColPropStatus(cColumnDescs, rgColumnDescs, hres), TRUE);

	pNewTableID = (ppTableID)? (*ppTableID): pTableID;

	// if the table creation was reported as succeding, the table should exist
	if (	SUCCEEDED(hres) 
		&&	(!CHECK(pTable->DoesTableExist(pNewTableID, &fExists), S_OK) || !fExists))
	{
		if (!fExists)
			odtLog << "\tERROR: despite the return value reported, the table doesn't exist\n";
	}

	// if the table creation was reported as failed, the table should not exist
	if (	FAILED(hres) 
		&&	DB_E_DUPLICATETABLEID != hres 
		&&	DB_E_NOTABLE != hres
		&&	pNewTableID
		&&	(!CHECK(pTable->DoesTableExist(pNewTableID, &fExists), S_OK) || fExists))
	{
		if (!fExists)
			odtLog << "\tERROR: despite the return value reported, the table exists\n";
	}

	if (SUCCEEDED(hres))
	{
		pIUnknown = (ppRowset) ? (IUnknown*)(*ppRowset) : NULL;
		COMPARE(AreColumnsCreatedOK(rgColumnDescs, cColumnDescs, pNewTableID, pIUnknown), TRUE);

		if (ppRowset)
		{
			COMPARE(IsRowsetPropSetCreatedOK(rgPropertySets, cPropertySets, *ppRowset), TRUE);
			COMPARE(!ppRowset || CheckRowset(*ppRowset), TRUE);
		}
		COMPARE(CheckReturnedDBID(pTableID, ppTableID), TRUE);
	}
	else
	{
		COMPARE(NULL == *ppRowset, TRUE);
		COMPARE(!ppTableID || NULL == *ppTableID, TRUE);
	}
	
	delete pTable;
	ReleaseColumnDesc(rgOriginalColumnDescs, cOriginalColumnDescs);
	FreeProperties( &cOriginalPropertySets, &rgOriginalPropertySets);
	return hres;
} //TCITableDefinition::CreateAndCheckTable



						
//---------------------------------------------------------------------
//
// @member Create the table using ITableDefinitionWithConstraints::CreateTableWithConstraints and 
// check its creation
//---------------------------------------------------------------------
HRESULT TCITableDefinition::CreateAndCheckTableWithConstraints(
		ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
		IUnknown							*pUnkOuter,			// [in] pointer to controlling unknown
		DBID								*pTableID,			// [in]	pointer to the input table ID
		ULONG								cColumnDescs,		// [in]	the number of columns to be created
		DBCOLUMNDESC						*rgColumnDescs,		// [in/out] array of columns to be created
		ULONG								cConstraintDescs,	// [in]	the number of constraints to be created
		DBCONSTRAINTDESC					*rgConstraintDescs,	// [in/out] array of constraints
		REFIID								riid,				// [in] rowset interface to be returned
		ULONG								cPropertySets,		// [in] number of roset property sets asked
		DBPROPSET							*rgPropertySets,	// [in/out] array of rowset property sets
		DBID								**ppTableID,		// [out] returned table ID
		IUnknown							**ppRowset			// [out] pointer to outer interface
)
{
	BOOL				fExists;
	ULONG				cOriginalColumnDescs = 0;
	DBCOLUMNDESC		*rgOriginalColumnDescs = NULL;
	ULONG				cOriginalPropertySets = 0;
	DBPROPSET			*rgOriginalPropertySets = NULL;
	HRESULT				hres;
	DBID				*pNewTableID = NULL;
	CTable				*pTable = new CTable(pITableDefinitionWithConstraints, (LPWSTR)gwszModuleName);
	IUnknown			*pIUnknown = NULL; // used to pass the rowset pointer to AreColumnsCreatedOK
	CConstraints		Constraints(pITableDefinitionWithConstraints);
	DBORDINAL			index;

	ASSERT(pTable);
	ASSERT(pITableDefinitionWithConstraints);
	
	// save the original values 
	CHECK(0 == cColumnDescs || NULL == rgColumnDescs
		|| NULL != DuplicateColumnDesc(rgColumnDescs, cColumnDescs, &rgOriginalColumnDescs), TRUE);
	cOriginalColumnDescs = cColumnDescs;
	CHECK(0 == cPropertySets || NULL == rgPropertySets 
		|| DuplicatePropertySets(cPropertySets, rgPropertySets, &cOriginalPropertySets, &rgOriginalPropertySets), TRUE);

	// get original constraint set
	if (pTableID && (DBKIND_NAME == pTableID->eKind) && pTableID->uName.pwszName)
	{
		CHECK(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
	}

	// go for ITableDefinition::CreateTable
	hres = pITableDefinitionWithConstraints->CreateTableWithConstraints(pUnkOuter, 
		pTableID, cColumnDescs, rgColumnDescs, cConstraintDescs, rgConstraintDescs,
		riid, cPropertySets, rgPropertySets, ppTableID, ppRowset);

	if (	rgOriginalColumnDescs
		&&	!COMPARE(IsColumnDescPreserved(rgOriginalColumnDescs, rgColumnDescs, cColumnDescs), TRUE))
	{
		odtLog << "\tERROR: array of column desc is not preserved\n";
	}

	if (	rgOriginalPropertySets
		&&	!COMPARE(IsRowsetPropSetPreserved(rgOriginalPropertySets, rgPropertySets, cPropertySets), TRUE))
	{
		odtLog << "\tERROR: the rowset property sets are not preserved\n";
	}

	COMPARE(CheckRwstTblPropStatus(cPropertySets, rgPropertySets, hres), TRUE);
	COMPARE(CheckColPropStatus(cColumnDescs, rgColumnDescs, hres), TRUE);

	pNewTableID = (ppTableID)? (*ppTableID): pTableID;

	// if the table creation was reported as succeding, the table should exist
	if (	SUCCEEDED(hres) 
		&&	(!CHECK(pTable->DoesTableExist(pNewTableID, &fExists), S_OK) || !fExists))
	{
		if (!fExists)
			odtLog << "\tERROR: despite the return value reported, the table doesn't exist\n";
	}

	// if the table creation was reported as failed, the table should not exist
	if (	FAILED(hres) 
		&&	DB_E_DUPLICATETABLEID != hres 
		&&	DB_E_NOTABLE != hres
		&&	pNewTableID
		&&	(!CHECK(pTable->DoesTableExist(pNewTableID, &fExists), S_OK) || fExists))
	{
		if (!fExists)
			odtLog << "\tERROR: despite the return value reported, the table exists\n";
	}


	if (SUCCEEDED(hres))
	{
		pIUnknown = (ppRowset) ? (IUnknown*)(*ppRowset) : NULL;
		COMPARE(AreColumnsCreatedOK(rgColumnDescs, cColumnDescs, pNewTableID, pIUnknown), TRUE);
		if (ppRowset)
		{
			COMPARE(IsRowsetPropSetCreatedOK(rgPropertySets, cPropertySets, *ppRowset), TRUE);
			COMPARE(!ppRowset || CheckRowset(*ppRowset), TRUE);
		}
		COMPARE(CheckReturnedDBID(pTableID, ppTableID), TRUE);

		// check constraint creation
		if (pNewTableID && (DBKIND_NAME == pNewTableID->eKind) && pNewTableID->uName.pwszName)
		{
			CHECK(Constraints.GetConstraints(NULL, NULL, pNewTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
			COMPARE(Constraints.GetConstrNo(), cConstraintDescs);

			for (index = 0; index < cConstraintDescs; index++)
			{
				// BEWARE: if deferred flag was set without deferrable 
				// flag being set, constraint should be immediate
				if (rgConstraintDescs[index].Deferrability == DBDEFERRABILITY_DEFERRABLE)
					rgConstraintDescs[index].Deferrability = 0;
			}
	

			CHECK(Constraints.CheckIncludedConstraints(cConstraintDescs, rgConstraintDescs), S_OK);
		}
	}
	else
	{
		COMPARE(NULL == ppRowset || NULL == *ppRowset, TRUE);
		COMPARE(NULL == ppTableID || NULL == *ppTableID, TRUE);

		// check no constraints existed on the table 
		if (DB_E_DUPLICATETABLEID == hres)
		{
			CHECK(Constraints.AreConstraintsPreserved(), S_OK);
		}
		else
		{
			// make sure no constraints exist for this table name (if not invalid)
			if (pTableID && (DBKIND_NAME == pTableID->eKind) && pTableID->uName.pwszName)
			{
				CHECK(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
				COMPARE(Constraints.GetConstrNo(), 0);
			}
		}
	}
	
	delete pTable;

	ReleaseColumnDesc(rgOriginalColumnDescs, cOriginalColumnDescs);
	FreeProperties( &cOriginalPropertySets, &rgOriginalPropertySets);
	return hres;
} //TCITableDefinition::CreateAndCheckTableWithConstraints




// Add a constraint to a table using ITableDefinitionWithConstraints::AddConstraint and check its creation
HRESULT	TCITableDefinition::AddConstraintAndCheck(
	ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
	DBID								*pTableID,							// [in]	pointer to the input table ID
	DBCONSTRAINTDESC					*pConstraintDesc					// [in] new constraint
)
{
	// this method assumes that the table indicated by pTableID does exist
	HRESULT			hr				= E_FAIL;
	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(pITableDefinitionWithConstraints);

	TESTC(NULL != pITableDefinitionWithConstraints);

	// get all the constraints associated with the table
	if (pTableID && DBKIND_NAME == pTableID->eKind)
	{
		TESTC_(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
	}
	else
	{
		TESTC_(Constraints.GetConstraints(NULL, NULL, NULL, NULL, NULL, NULL), S_OK);
	}
	
	hr = pITableDefinitionWithConstraints->AddConstraint(pTableID, pConstraintDesc);

	// check that the result is one of the expected results
	TESTC(CheckResult(hr, cValidHRes, rgValidHRes));

	if (FAILED(hr))
	{
			CHECK(Constraints.AreConstraintsPreserved(), S_OK);
	}
	else
	{
		TESTC(NULL != pTableID);
		// check that the constraint was added
		DBDEFERRABILITY		def = pConstraintDesc->Deferrability;
		// BEWARE: if deferred flag was set without deferrable 
		// flag being set, constraint should be immediate
		if (pConstraintDesc->Deferrability == DBDEFERRABILITY_DEFERRABLE)
			pConstraintDesc->Deferrability = 0;
		CHECK(Constraints.CheckAddConstraints(1, pConstraintDesc), S_OK);
		pConstraintDesc->Deferrability = def;
	}

	switch (hr)
	{
		case S_OK:
			break;

		case E_INVALIDARG:
			// check the returned value condition (assume only one of them was set, if any)
			TESTC(	NULL == pTableID || NULL == pConstraintDesc 
			||	0 < pConstraintDesc->cForeignKeyColumns && NULL == pConstraintDesc->rgForeignKeyColumnList
			||	0 < pConstraintDesc->cColumns && NULL == pConstraintDesc->rgColumnList);
			break;

		case DB_E_BADID:
		//case DB_E_BADTABLEID:
			// conditions for DB_E_BADTABLEID and DB_E_BADID 
			break;

		case DB_E_DUPLICATECONSTRAINTID:
			// condition for DB_E_DUPLICATECONSTRAINTID
			TESTC(NULL != Constraints[pConstraintDesc->pConstraintID]);
			break;

		case DB_E_BADCONSTRAINTTYPE:
			// condition for DB_E_BADCONSTRAINTTYPE
			TESTC(DBCONSTRAINTTYPE_UNIQUE != pConstraintDesc->ConstraintType);
			TESTC(DBCONSTRAINTTYPE_PRIMARYKEY != pConstraintDesc->ConstraintType);
			TESTC(DBCONSTRAINTTYPE_FOREIGNKEY != pConstraintDesc->ConstraintType);
			TESTC(DBCONSTRAINTTYPE_CHECK != pConstraintDesc->ConstraintType);
			break;

		case DB_E_BADUPDATEDELETERULE:
			TESTC(DBCONSTRAINTTYPE_FOREIGNKEY == pConstraintDesc->ConstraintType);
			break;

		case DB_E_BADMATCHTYPE:
			break;

		case DB_E_BADDEFERRABILITY:
			break;

		case DB_E_BADCONSTRAINTFORM:
			TESTC(		DBCONSTRAINTTYPE_FOREIGNKEY != pConstraintDesc->ConstraintType
					&& (0 < pConstraintDesc->cForeignKeyColumns || NULL != pConstraintDesc->pReferencedTableID)
				||	DBCONSTRAINTTYPE_CHECK == pConstraintDesc->ConstraintType && NULL == pConstraintDesc->pwszConstraintText
				||	DBCONSTRAINTTYPE_CHECK != pConstraintDesc->ConstraintType && NULL != pConstraintDesc->pwszConstraintText
				||	DBCONSTRAINTTYPE_FOREIGNKEY == pConstraintDesc->ConstraintType && 0 == pConstraintDesc->cForeignKeyColumns
				||	(	DBCONSTRAINTTYPE_FOREIGNKEY == pConstraintDesc->ConstraintType
					||	DBCONSTRAINTTYPE_PRIMARYKEY == pConstraintDesc->ConstraintType
					||	DBCONSTRAINTTYPE_UNIQUE == pConstraintDesc->ConstraintType) && 0 == pConstraintDesc->cColumns
				||	NULL == pConstraintDesc->pReferencedTableID && 0 != pConstraintDesc->cForeignKeyColumns);
			break;

		case DB_E_BADCOLUMNID:
			// check that one of the columns in the referred does not exist in the table
			break;

		case DB_E_NOTABLE:
			// pReferencedTableID (for foreign keys) or pTableID does not exist
			break;

		case DB_E_TABLEINUSE:
		case DB_SEC_E_PERMISSIONDENIED:
		case XACT_E_XTIONEXISTS:
			break;

		case E_FAIL:
			// this code is legal; caller should catch it if there
			// is a more meaningful return value
			break;

		default:
			TESTC(FALSE);
	};

CLEANUP:
	return hr;
} //TCITableDefinition::AddConstraintAndCheck




//---------------------------------------------------------------------
//
// Drop a constraint to a table using ITableDefinitionWithConstraints::DropConstraint and check its deletion
HRESULT	TCITableDefinition::DropConstraintAndCheck(
	ITableDefinitionWithConstraints		*pITableDefinitionWithConstraints,	// [in] pointer to the ITableDefinitionWithConstraints interface to be used
	DBID								*pTableID,							// [in]	pointer to the input table ID
	DBID								*pConstraintID						// [in] pointer to the DBID of the constraint
)
{
	HRESULT			hr = E_FAIL;
	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADID,
										DB_E_BADCONSTRAINTID,
										DB_E_BADTABLEID,
										DB_E_DROPRESTRICTED,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	DBORDINAL		ulInitConstraints = 0;

	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(pITableDefinitionWithConstraints);

	TESTC(NULL != pITableDefinitionWithConstraints);

	// get all the constraints associated with the table
	if (pTableID && DBKIND_NAME == pTableID->eKind)
	{
		CHECK(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
		ulInitConstraints = Constraints.GetConstrNo();
	}
	else
	{
		CHECK(Constraints.GetConstraints(NULL, NULL, NULL, NULL, NULL, NULL), S_OK);
	}

	// drop the constraint
	hr = pITableDefinitionWithConstraints->DropConstraint(pTableID, pConstraintID);


	// check that the result is one of the expected results
	COMPARE(CheckResult(hr, cValidHRes, rgValidHRes), TRUE);

	if (FAILED(hr))
	{
			CHECK(Constraints.AreConstraintsPreserved(), S_OK);
	}
	else
	{
		// check that the constraint was dropped
		if (NULL != pTableID && DBKIND_NAME == pTableID->eKind)
		{
			CConstraints	NewConstraints(pITableDefinitionWithConstraints);
			
			CHECK(NewConstraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK);
			COMPARE(NULL == NewConstraints[pConstraintID], TRUE);
			COMPARE(ulInitConstraints-1, NewConstraints.GetConstrNo());
			CHECK(Constraints.CheckIncludedConstraints(NewConstraints.GetConstrNo(),
				NewConstraints.GetConstraints()), S_OK);
		}

	}

CLEANUP:
	return hr;
} //TCITableDefinition::DropConstraintAndCheck




//---------------------------------------------------------------------
//
// @member Create the table using ITableDefinition::CreateTable, 
// check its creation then drops the table and releases the rowset interface
//---------------------------------------------------------------------
HRESULT TCITableDefinition::CCNDropTable(
	DBID			*pTableID,			// [in]	pointer to the input table ID
	ULONG			cColumnDescs,		// [in]	the number of columns to be created
	DBCOLUMNDESC	*rgColumnDescs,		// [in/out] array of columns to be created
	REFIID			riid,				// [in] rowset interface to be returned
	ULONG			cPropertySets,		// [in] number of roset property sets asked
	DBPROPSET		*rgPropertySets,	// [in/out] array of rowset property sets
	BOOL			fNullPPTableID/*=FALSE*/	// [in] if TRUE ppTableID will be NULL
)
{
	HRESULT		hr;
	BOOL		fExists;
	DBID		*pNewTableID	= NULL;
	DBID		*pOutTableID	= NULL;		// [out] returned table ID
	DBID		**ppTableID		= fNullPPTableID? NULL: &pOutTableID;
	IUnknown	*pRowset		= NULL;		// [out] pointer to outer interface
	CTable		*pTable			= new CTable(m_pITableDefinition, (LPWSTR)gwszModuleName);

	hr = CreateAndCheckTable(m_pITableDefinition, NULL, pTableID, cColumnDescs, rgColumnDescs, riid, 
		cPropertySets, rgPropertySets, ppTableID, &pRowset);

	SAFE_RELEASE(pRowset);

	pNewTableID = (ppTableID)? *ppTableID: pTableID;
	if (	pNewTableID
		&&	DB_E_NOTABLE != hr
		&&	DB_E_DUPLICATETABLEID != hr
		&&	CHECK(pTable->DoesTableExist(pNewTableID, &fExists), S_OK)
		&&	fExists)
		CHECK(m_pITableDefinition->DropTable(pNewTableID), S_OK);
	
	delete pTable;
	ReleaseDBID(pOutTableID);
	return hr;
} //TCITableDefinition::CCNDropTable




//--------------------------------------------------------------------------
//
// @cmember Check whether the columns were properly created
// The columns of the m_pTable are checked against the column descriptor array <nl>
// returned by ITableDefinition::CreateTable method. 
//	RETURNS
//		TRUE	- everything is created ok
//		FALSE	- errors
//--------------------------------------------------------------------------
BOOL TCITableDefinition::AreColumnsCreatedOK(
	DBCOLUMNDESC	*rgOutColumnDesc,	// [in] the aray returned by ITableDefinition::CreateTable
	ULONG			cColumnDesc,		// [in] the size of the arrays
	DBID			*pTableID/*=NULL*/,	// [in] Table ID
	IUnknown		*pIUnknown/*=NULL*/	// [in] pointer to a rowset interface on the table
)
{
	ULONG			cCDFromTable;
	BOOL			fTestRes		= TRUE;
	DBCOLUMNDESC	*rgCDFromTable	= NULL;
	ULONG			i;
	CTable			*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);

	// get the descriptions of the columns in the table with DBID == m_pTable->GetTableID()
	// the list of columns are stored in m_pTable->m_colList
	GetColumnDescOnTable(pTableID? pTableID: &m_pTable->GetTableID(), 
		&rgCDFromTable, &cCDFromTable);

	if (cColumnDesc != cCDFromTable)
	{
		odtLog << "the number of columns asked and created does not fit";
		fTestRes = FALSE;
		goto CLEANUP;
	}

	for (i=0; i<cColumnDesc; i++)
	{
		fTestRes = fTestRes && CompareColumnDesc(&rgOutColumnDesc[i], &rgCDFromTable[i]);
	}
	
CLEANUP:
	ReleaseColumnDesc(rgCDFromTable, cCDFromTable);
	delete pTable;
	return fTestRes;
} //TCITableDefinition::AreColumnsCreatedOK




//--------------------------------------------------------------------
//
// @member Check properties on rowseet and table
//	returns TRUE if the prop set is set correctly
//			FALSE otherwise
//--------------------------------------------------------------------
BOOL TCITableDefinition::CheckRwstTblPropStatus(
	ULONG			cPropSets,			// [in] the size of the propset array
	DBPROPSET		*rgPropSets,		// [in] propset array
	HRESULT			hr					// [in]	the result of the previous op
)
{
	GUID	guidExpectedPS;
	ULONG	cPropSet;
	ULONG	cProp;
	BOOL	fRes = TRUE;

	if (!rgPropSets)
		return TRUE;

	for (cPropSet = 0; cPropSet < cPropSets; cPropSet++)
	{
		if (!rgPropSets[cPropSet].rgProperties)
			continue;
		guidExpectedPS = rgPropSets[cPropSet].guidPropertySet == DBPROPSET_ROWSET? DBPROPSET_ROWSET: DBPROPSET_TABLE;
		for (cProp = 0; cProp < rgPropSets[cPropSet].cProperties; cProp++)
		{
			fRes = fRes && CheckPropertyStatus(rgPropSets[cPropSet].guidPropertySet, guidExpectedPS,
				&rgPropSets[cPropSet].rgProperties[cProp], hr);
		}
	}
	return fRes;
} //TCITableDefinition::CheckRwstTblPropStatus
	



//--------------------------------------------------------------------------
//
// @cmember Check whether the property sets asked for rowset was preserved
// all the data should be inside, the only thing that is allowed to be modified <nl>
// is the property status
//--------------------------------------------------------------------------
BOOL TCITableDefinition::IsRowsetPropSetPreserved
(
	DBPROPSET	*rgInPropertySets,	// the array passed to ITableDefinition::CreateTable
	DBPROPSET	*rgOutPropertySets,	// the aray returned by ITableDefinition::CreateTable
	ULONG		cPropertySets			// the size of the arrays
)
{
	ULONG	ps, pc;
	
	if (NULL == rgInPropertySets)
		return TRUE;
	for (ps=0; ps<cPropertySets; ps++)
	{
		// compare the props in that prop set
		if (!CHECK(rgInPropertySets[ps].guidPropertySet == rgOutPropertySets[ps].guidPropertySet, TRUE))
			return FALSE;
		if (!CHECK(rgInPropertySets[ps].cProperties == rgOutPropertySets[ps].cProperties, TRUE))
			return FALSE;
		if (NULL == rgInPropertySets[ps].rgProperties)
			continue;

		for (pc=0; pc<rgInPropertySets[ps].cProperties; pc++)
		{
			if (!CHECK(rgInPropertySets[ps].rgProperties[pc].dwPropertyID == 
					   rgOutPropertySets[ps].rgProperties[pc].dwPropertyID, TRUE))
				return FALSE;
			if (!CHECK(rgInPropertySets[ps].rgProperties[pc].dwOptions == 
					   rgOutPropertySets[ps].rgProperties[pc].dwOptions, TRUE))
				return FALSE;
			// use NULL for DSO interface
			if (!COMPARE(CompareDBID(rgInPropertySets[ps].rgProperties[pc].colid, 
					   rgOutPropertySets[ps].rgProperties[pc].colid,
					   NULL), TRUE))
				return FALSE;
			if (!COMPARE(CompareVariant(&rgInPropertySets[ps].rgProperties[pc].vValue,
					   &rgOutPropertySets[ps].rgProperties[pc].vValue), TRUE))
				return FALSE;
		}
	}
	return TRUE;
} //TCITableDefinition::IsRowsetPropSetPreserved

						


//--------------------------------------------------------------------------
//
// @cmember Check whether the rowset properties were properly created
// The rowset properties of the m_pTable are checked against the property set<nl>
// returned by ITableDefinition::CreateTable method. 
//--------------------------------------------------------------------------
BOOL TCITableDefinition::IsRowsetPropSetCreatedOK
(
	DBPROPSET	*rgOutPropertySets,		// the aray returned by ITableDefinition::CreateTable
	ULONG		cPropertySets,			// the size of the arrays
	IUnknown	*pIUnknown				// pointer to the rowset interface
)
{
	IRowsetInfo		*pIRowsetInfo = NULL;
	DBPROPIDSET		*rgPropIDSet = NULL;
	DBPROPSET		*rgPropertySets = NULL;
	ULONG			n;
	BOOL			fTestRes = TEST_PASS;
	DBPROPSTATUS	ulStatus;
	DBPROPOPTIONS	ulOption;
	DBPROPID		ulPropID;
	BOOL			fSupported;
	BOOL			fSettable;
	VARIANT			*pvValue;
	
	if (!pIUnknown || 0 == cPropertySets || !rgOutPropertySets)
		return TRUE;

	rgPropIDSet = (DBPROPIDSET*) PROVIDER_ALLOC(sizeof(DBPROPIDSET)*cPropertySets);
	for (n=0; n<cPropertySets; n++)
	{
		// get propID in the property set 
		rgPropIDSet[n].guidPropertySet	= rgOutPropertySets[n].guidPropertySet;
		rgPropIDSet[n].cPropertyIDs		= rgOutPropertySets[n].cProperties;
		if (0 == rgOutPropertySets[n].rgProperties)
		{
			rgPropIDSet[n].rgPropertyIDs = NULL;
			continue;
		}
		else
			SAFE_ALLOC(rgPropIDSet[n].rgPropertyIDs, DBPROPID, rgOutPropertySets[n].cProperties);
		for (ULONG i=0; i<rgOutPropertySets[n].cProperties; i++)
		{
			rgPropIDSet[n].rgPropertyIDs[i] = rgOutPropertySets[n].rgProperties[i].dwPropertyID;
		}
	}
	// get the properties of the rowset
	if (!VerifyInterface(pIUnknown, IID_IRowsetInfo, ROWSET_INTERFACE, (IUnknown**)&pIRowsetInfo))
	{
		fTestRes = TEST_FAIL;
		goto CLEANUP;
	}
	else
	{
		m_hr = pIRowsetInfo->GetProperties(cPropertySets, rgPropIDSet, &n, &rgPropertySets);
		ASSERT(n==cPropertySets);
	}

	if (NULL == rgPropertySets)
	{
		fTestRes = TEST_FAIL;
		goto CLEANUP;
	}
	// check the properties
	for (n=0; n<cPropertySets; n++)
	{
		// get propID in the property set 
		if (DBPROPSET_TABLE == rgOutPropertySets[n].guidPropertySet)
			continue;
		if (0 == rgOutPropertySets[n].cProperties)
			continue;
		if (rgPropertySets[n].cProperties != rgOutPropertySets[n].cProperties)
			fTestRes = TEST_FAIL;
		if (NULL == rgOutPropertySets[n].rgProperties)
			continue;
		if (NULL == rgPropertySets[n].rgProperties)
		{
			fTestRes = TEST_FAIL;
			continue;
		}
		// check dbpropstatus
		if (!CHECK(DBPROPSET_ROWSET == rgOutPropertySets[n].guidPropertySet, TRUE))
		{
			for (ULONG i=0; i<rgOutPropertySets[n].cProperties; i++)
				if (!COMPARE(rgOutPropertySets[n].rgProperties[i].dwStatus, DBPROPSTATUS_NOTSUPPORTED))
					fTestRes = TEST_FAIL;
			continue;
		}

		for (ULONG i=0; i<rgOutPropertySets[n].cProperties; i++)
		{
			ulPropID	= rgOutPropertySets[n].rgProperties[i].dwPropertyID;
			ulOption	= rgOutPropertySets[n].rgProperties[i].dwStatus;
			ulStatus	= rgOutPropertySets[n].rgProperties[i].dwOptions;
			fSupported	= SupportedProperty(ulPropID, DBPROPSET_ROWSET);
			fSettable	= SettableProperty(ulPropID, DBPROPSET_ROWSET);
			pvValue		= &rgOutPropertySets[n].rgProperties[i].vValue;
	
			if (!fSupported)
			{
				fTestRes = DBPROPSTATUS_NOTSUPPORTED == ulStatus? fTestRes : TEST_FAIL;
				continue;
			}

			// property is supported => cannot be sure that setting a prop will fail
			// if is settable, the prop should succeed, unless a bad value is tried
			if (DBPROPSTATUS_OK == ulStatus)
			{
				if (ulPropID != rgOutPropertySets[n].rgProperties[i].dwPropertyID)
					fTestRes = TEST_FAIL;
				if (!CompareVariant(pvValue, &rgOutPropertySets[n].rgProperties[i].vValue))
				{
					fTestRes = TEST_FAIL;
					odtLog	<< "Rowset property values missmatch for property propID("
							<< rgOutPropertySets[n].rgProperties[i].dwPropertyID << ")\n";
				}
			}

			switch (ulStatus)
			{
				case DBPROPSTATUS_CONFLICTING:
					fTestRes = (DBPROPOPTIONS_OPTIONAL == ulOption || DBPROPOPTIONS_REQUIRED == ulOption)
						&& pvValue->vt == GetPropInfoType(ulPropID, DBPROPSET_ROWSET)? fTestRes : TEST_FAIL;
					break;
				case DBPROPSTATUS_NOTSET:
					fTestRes = COMPARE(ulOption, DBPROPOPTIONS_OPTIONAL)
						&& pvValue->vt == GetPropInfoType(ulPropID, DBPROPSET_ROWSET)? fTestRes : TEST_FAIL;
					break;
				case DBPROPSTATUS_BADOPTION:
					fTestRes =		(DBPROPOPTIONS_OPTIONAL != ulOption) 
								&&	(DBPROPOPTIONS_REQUIRED)? fTestRes : TEST_FAIL;
					break;
				case DBPROPSTATUS_NOTSETTABLE:
					fTestRes = !fSettable? fTestRes: TEST_FAIL;
					break;
				case DBPROPSTATUS_BADVALUE:
					fTestRes = (VT_EMPTY != pvValue->vt)
						&& (pvValue->vt != GetPropInfoType(ulPropID, DBPROPSET_ROWSET))
						? fTestRes : TEST_FAIL;
			}
		}
	}

CLEANUP:
	SAFE_RELEASE(pIRowsetInfo);
	for (n=0; n<cPropertySets; n++)
		PROVIDER_FREE(rgPropIDSet[n].rgPropertyIDs);
	PROVIDER_FREE(rgPropIDSet);
	FreeProperties( &cPropertySets, &rgPropertySets);
	return fTestRes;
} //TCITableDefinition::IsRowsetPropSetCreatedOK



//--------------------------------------------------------------------------
//
// @cmember Creates a table with a column of each provider type and returns a rowset
// to the new created table. The rowset is created with the property required and it
// is tested
//--------------------------------------------------------------------------
int	TCITableDefinition::CreateWithOneRowsetProps
(
	DBPROPID		PropID,				// Property ID
	DBTYPE			DBType,				// type of property value
	LPVOID			Value,				// value to be set
	struct _GUID	*pRIID				// interface asked on rowset
)
{
	DBTYPE			rgDBTypes[]		= {DBType};
	LPVOID			rgValues[]		= {Value};
	DBPROPOPTIONS	rgOptions[]		= {DBPROPOPTIONS_OPTIONAL};
	DBPROPOPTIONS	rgOptions2[]	= {DBPROPOPTIONS_REQUIRED};


	return	CreateWithRowsetProps(1, &PropID, rgDBTypes, rgValues, rgOptions, pRIID)
		&&	CreateWithRowsetProps(1, &PropID, rgDBTypes, rgValues, rgOptions2, pRIID);
} //TCITableDefinition::CreateWithOneRowsetProps

	



//--------------------------------------------------------------------------
//
// @cmember Creates a table with a column of each provider type and returns a rowset
// to the new created table. The rowset is created with the properties required and it
// is tested
//--------------------------------------------------------------------------
BOOL TCITableDefinition::CreateWithRowsetProps
(
	ULONG			nProperties,			// number of the properties to be set
	DBPROPID		*rgPropID,				// array with the propIDs
	DBTYPE			*rgDBType,				// type of property values			
	LPVOID			*rgValue,				// values to be set
	DBPROPOPTIONS	*rgOptions,				// options of properties
	struct _GUID	*pRIID					// interface asked on rowset
)
{
	BOOL			fRes			= FALSE;
	DBPROPSET		*rgPropertySets = NULL;
	ULONG			n, cPropertySets = 0;
	HRESULT			hr				= E_FAIL;
	CCol			col;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	BOOL			fExists;
	DBID			*pTableID		= NULL;
	IUnknown		*pRowset		= NULL;		// [out] pointer to outer interface
	CTable			*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);

	TESTC(nProperties);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	for (n=0; n<nProperties; n++)
	{
		SetProperty(&rgPropertySets, &cPropertySets, rgPropID[n],
					rgDBType[n], rgValue[n], rgOptions[n], DB_NULLID, DBPROPSET_ROWSET);
	}


	hr = CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, *pRIID,
		cPropertySets, rgPropertySets, &pTableID, &pRowset);

	SAFE_RELEASE(pRowset);

	if (	pTableID
		&&	DB_E_NOTABLE != hr
		&&	DB_E_DUPLICATETABLEID != hr
		&&	CHECK(pTable->DoesTableExist(pTableID, &fExists), S_OK)
		&&	fExists)
		CHECK(m_pITableDefinition->DropTable(pTableID), S_OK);
	
	delete pTable;
	ReleaseDBID(pTableID);

	if (FAILED(hr))
	{
		odtLog << "Could not create a table with a specified property set.\n";
		if (	(1 == nProperties) 
			&& 	(DBPROPSTATUS_OK != rgPropertySets[0].rgProperties[0].dwStatus)
			&&		(	(DBPROPOPTIONS_REQUIRED == rgPropertySets[0].rgProperties[0].dwStatus) 
					&&	!CHECK(hr, DB_E_ERRORSOCCURRED)))
		{
			odtLog << "wrong value for dwStatus in rowset property";
			goto CLEANUP;
		}
	}
	else
		// check the creation of the rowset
		if (	(1 == nProperties) 
			&&	(	(	(DBPROPSTATUS_OK == rgPropertySets[0].rgProperties[0].dwStatus) 
					&&	!CHECK(hr, S_OK))
				||	(	(DBPROPSTATUS_OK != rgPropertySets[0].rgProperties[0].dwStatus) 
					&&	!CHECK(hr, DB_S_ERRORSOCCURRED))))
		{
			odtLog << "wrong value for dwStatus in rowset property";
			goto CLEANUP;
		}

	fRes = TRUE;

CLEANUP:
	FreeProperties( &cPropertySets, &rgPropertySets);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fRes;
} //TCITableDefinition::CreateWithRowsetProps




//--------------------------------------------------------------------------
//
// creates a table with not nullable column
//--------------------------------------------------------------------------
HRESULT TCITableDefinition::CreateTableWithNoNullableColumns(	
	CTable		*pTable,						// @param [IN] pointer to CTable 
	DBCOUNTITEM ulRowCount,						// @parm [IN] # of Rows to insert into table, 1 based
	DBORDINAL	ulIndex /*=0*/,					// @parm [IN] Column Number of index, 1 based (Default=1)
	WCHAR		*pwszTableName/*=NULL*/,		// @parm [IN] TableName, if NULL call MakeTableName() (Default=NULL)
	EVALUE		eValue/*=PRIMARY*/,				// @parm [IN] Insert PRIMARY or SECONDARY data (Default=PRIMARY)
	BOOL		fFirstUpdateable/*=FALSE*/		// @parm [IN] TRUE means first column will be autoinc (Default=FALSE)
)
{
	HRESULT				hr = E_FAIL;
	// customize the table so that no nullable columns is added
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	DBORDINAL			indexCol;
	
	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);

	for (indexCol = 0; indexCol < m_cColumnDesc; indexCol++)
	{
		SetProperty(&rgNewColumnDesc[indexCol].rgPropertySets, &rgNewColumnDesc[indexCol].cPropertySets,
				DBPROP_COL_NULLABLE, 
				VT_BOOL, (LPVOID)VARIANT_FALSE, 
				DBPROPOPTIONS_REQUIRED);
	}

	// by setting DBCOLUMNDESC, all nullable type column are created as nullable 
	pTable->SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	pTable->SetBuildColumnDesc(FALSE);
	
	TESTC_(pTable->CreateTable(ulRowCount, ulIndex, pwszTableName, eValue, fFirstUpdateable), S_OK);
	
	hr = S_OK;

CLEANUP:
	return hr;
} //TCITableDefinition::CreateTableWithNoNullableColumns



HRESULT TCITableDefinition::AddUniqueConstraint(
	CTable				*pTable, 
	DBORDINAL			ulCol,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	return AddKeyConstraint(DBCONSTRAINTTYPE_UNIQUE, pTable, 1, &ulCol, Deferrability);
} //TCITableDefinition::AddUniqueConstraint



HRESULT TCITableDefinition::AddUniqueConstraint(
	CTable				*pTable, 
	DBORDINAL			cCol,
	DBORDINAL			*rgCol,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	return AddKeyConstraint(DBCONSTRAINTTYPE_UNIQUE, pTable, cCol, rgCol, Deferrability);
} //TCITableDefinition::AddUniqueConstraint



HRESULT TCITableDefinition::AddPrimaryKeyConstraint(
	CTable				*pTable, 
	DBORDINAL			ulCol,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	return AddKeyConstraint(DBCONSTRAINTTYPE_PRIMARYKEY, pTable, 1, &ulCol, Deferrability);
} //TCITableDefinition::AddPrimaryKeyConstraint



HRESULT TCITableDefinition::AddPrimaryKeyConstraint(
	CTable				*pTable, 
	DBORDINAL			cCol,
	DBORDINAL			*rgCol,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	return AddKeyConstraint(DBCONSTRAINTTYPE_PRIMARYKEY, pTable, cCol, rgCol, Deferrability);
} //TCITableDefinition::AddPrimaryKeyConstraint



HRESULT TCITableDefinition::AddKeyConstraint(
	DBCONSTRAINTTYPE	ConstraintType,
	CTable				*pTable, 
	DBORDINAL			cCol,
	DBORDINAL			*rgCol,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	HRESULT			hr = E_FAIL;
	CCol			col;
	CKeyConstraint	cons(ConstraintType);

	TESTC(NULL != m_pITDWC);
	TESTC(NULL != pTable);
	TESTC(0 == cCol || NULL != rgCol);
	TESTC(DBCONSTRAINTTYPE_UNIQUE == ConstraintType || DBCONSTRAINTTYPE_PRIMARYKEY == ConstraintType);
	
	TESTC_(cons.SetConstraint(pTable, cCol, rgCol, Deferrability), S_OK);

	TESTC_(pTable->GetColInfo(rgCol[0], col), S_OK);

	hr = AddConstraintAndCheck(m_pITDWC, &pTable->GetTableID(), cons);

CLEANUP:
	return hr;
} // TCITableDefinition::AddKeyConstraint



HRESULT TCITableDefinition::AddForeignKeyConstraint(
	CTable				*pBaseTable, 
	DBORDINAL			cBaseCol,
	DBORDINAL			*rgBaseCol,
	CTable				*pReferencedTable, 
	DBORDINAL			cReferencedCol,
	DBORDINAL			*rgReferencedCol,
	DBMATCHTYPE			MatchType			/*= DBMATCHTYPE_FULL*/,
	DBUPDELRULE			UpdateRule			/*= DBUPDELRULE_NOACTION*/,
	DBUPDELRULE			DeleteRule			/*= DBUPDELRULE_NOACTION*/,
	DBDEFERRABILITY		Deferrability		/*= 0*/
)
{
	HRESULT			hr		= E_FAIL;
	CForeignKeyCons	cons;
	DBORDINAL		ulRow;

	TESTC_(cons.SetConstraint(pBaseTable, cBaseCol, rgBaseCol,
		pReferencedTable, cReferencedCol, rgReferencedCol,
		MatchType, UpdateRule, DeleteRule, Deferrability), S_OK);

	hr = AddConstraintAndCheck(m_pITDWC, &pBaseTable->GetTableID(), cons);

	if (S_OK == hr)
	{
		ulRow = pReferencedTable->GetNextRowNumber();
		// check that you can not add a row to the base table (no nulls are inserted, right?)
		TEST2C_(pBaseTable->Insert(ulRow), DB_E_INTEGRITYVIOLATION, E_FAIL);
		// check that you can add a row to the referenced table
		CHECK(pReferencedTable->Insert(ulRow), S_OK);
		// check that you can now add a row to the base table
		CHECK(pBaseTable->Insert(ulRow), S_OK);
	}

CLEANUP:
	return hr;
} // TCITableDefinition::AddForeignKeyConstraint




//--------------------------------------------------------------------------
//
// @cmember Termination function
//--------------------------------------------------------------------------
BOOL TCITableDefinition::Terminate()
{
	DeleteCriticalSection(&m_CriticalSection);
	ULONG	nTables;
	nTables = GetNoOfTables();
	ReleaseColumnDesc(m_rgColumnDesc, m_cColumnDesc);
	if (nTables>m_nTables)
		odtLog << "Numer of residual tables: " << nTables-m_nTables << "\n";
	nResidualTables += nTables-m_nTables;
	SAFE_RELEASE(m_pITableDefinition);
	SAFE_RELEASE(m_pITDWC);
	ReleaseDBSession();
	ReleaseDataSourceObject();
	m_ColList.RemoveAll();
	PROVIDER_FREE(m_pwszInvalidTableChars);
	PROVIDER_FREE(m_pwszInvalidColChars);
	return(CSessionObject::Terminate());
} //TCITableDefinition::Terminate




// {{ TCW_TC_PROTOTYPE(TCAddColumn)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddColumn - Testcase for ITableDefinition::AddColumn
//|	Created:			09/10/97
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddColumn::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{
		//
		return TRUE;
	}
	return FALSE;
}



// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Table doesn't exist => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_1()
{
	BOOL			fTestRes = TEST_PASS, fExists;
	DBID			TableID;
	DBID			*pColumnID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	TableID.eKind = DBKIND_NAME;
	// CTable::MakeTableName() generates a name that doesn't exist in the catalog
	m_pTable->MakeTableName(NULL);
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// try to add a column on this table that doesn't exist
	if (!CHECK(m_hr = m_pITableDefinition->AddColumn(&TableID, rgColumnDesc, &pColumnID), DB_E_NOTABLE))
		fTestRes = TEST_FAIL;

	if (!CHECK(m_pTable->DoesTableExist(&TableID, &fExists), S_OK) || fExists)
	{
		odtLog << "Table " << TableID.uName.pwszName << " was added by AddColumn!\n";
		fTestRes = TEST_FAIL;
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc The table is in use => DB_E_TABLEINUSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_2()
{
	TBEGIN
	DBID			*pColumnID			= NULL;
	IRowset			*pIRowset			= NULL;
	IRowsetChange	*pIRowsetChange		= NULL;
	IRowsetChange	*pIRowsetChange2	= NULL;
	HRESULT			hr;
	HACCESSOR		hAccessor			= NULL;
	BYTE			*pData				= NULL;
	ULONG			cBindings			= 0;
	DBBINDING		*rgBindings			= NULL;
	const LONG		cColsToBind			= 1;
	LONG			rgColsToBind[cColsToBind];
	HROW			*rghRow				= NULL;
	ULONG			cRowsObtained		= 0;
	DBPROPSET		rgPropSets[1];
	DBPROP			rgProperties[1];
	ULONG			ulRowSize, nRowNo;
	ULONG			nRowsInTable		= 0;
	DBCOLUMNDESC	*rgColumnDesc		= NULL;
	ULONG			cColumnDesc			= 0;
	const ULONG		nRowsToInsert		= 10;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);
	// ask for a IRowsetChange interface
	m_pTable->SetRowset((IUnknown**)&pIRowsetChange);
	if (SettableProperty(DBPROP_IRowsetChange, DBPROPSET_ROWSET))
		m_pTable->SetRIID((struct _GUID*)&IID_IRowsetChange);
	else
		m_pTable->SetRIID((struct _GUID*)&IID_IRowset);

	// Set properties
	FILL_PROP_SET(rgPropSets[0], 1, rgProperties, DBPROPSET_ROWSET);
	FILL_PROP(rgProperties[0], DBPROP_OWNINSERT, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	m_pTable->SetPropertySets(rgPropSets, NUMELEM(rgPropSets));

	// creates the table and populates it, using the returned pIRowsetChange
	TESTC_(hr = m_pTable->CreateTable(0, 0), S_OK);
	
	// check whether iRowsetChange is available
	VerifyInterface(pIRowsetChange, IID_IRowsetChange, ROWSET_INTERFACE, (IUnknown**)&pIRowsetChange2);
	if (pIRowsetChange2)
	{
		for (nRowNo = 0; nRowNo < nRowsToInsert; nRowNo++)
		{
			if (S_OK == Insert(0, (IUnknown*)pIRowsetChange))
				nRowsInTable++;
		}
	}

	TESTC(VerifyInterface(pIRowsetChange, IID_IRowset, ROWSET_INTERFACE, (IUnknown**)&pIRowset));
	
	// get a row
	if (0 < nRowsInTable)
	{
		m_hr = pIRowset->GetNextRows(0, 0,					// hChapter & lRowsOffset
										1,					// number of rows to be retrieved
										&cRowsObtained,		//number of rows actually get
										&rghRow				// array of handlers to rows
										);
		if (!CHECK(m_hr, S_OK))
		{
			odtLog << "insuccess on initial IRowset::GetNextRows\n";
			m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL);
		}
		// test the return value of IRowset::RestartPosition
		CHECK(m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL), S_OK);
		PROVIDER_FREE(rghRow);
		cRowsObtained = 0;		
	}
	else
		odtLog << "Could not insert any row in the table\n";

	
	// check if a column can be added, on the table in use (look for S_OK or DB_E_TABLEINUSE)
	hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID);
	
	if (hr == S_OK)
	{
		odtLog << "Column was added while a rowset was still open on table.\n";

		if (1 > nRowsInTable)
			goto CLEANUP;

		// see how it reacts to RestartPosition and if the new column can be read
		hr = pIRowset->RestartPosition(0); // a non-chaptered rowset
		if (hr == DB_S_COLUMNSCHANGED)
		{
			odtLog << "DB_S_COLUMNSCHANGED returned on pIRowset->RestartPosition\n";

			// get the value of the new column in the first row
			//Get an accessor with all the writeable columns
			rgColsToBind[0] = cColumnDesc-1;
			if (!CHECK(hr = GetAccessorAndBindings(
					pIRowset,									// pIUnknRowsetObject
					DBACCESSOR_ROWDATA,							// accessor flags
					&hAccessor, &rgBindings, &cBindings,		// accessor handler, array and number of bindings created
					&ulRowSize,									// [OUT] row size
					DBPART_VALUE |DBPART_STATUS |DBPART_LENGTH,	// types of bindings to do
					USE_COLS_TO_BIND_ARRAY,						// eColsByRef specifies that cols to be bind 
																// are given as an array
					FORWARD,									// eBindingOrder
					NO_COLS_BY_REF,								// eColsByRef
					NULL, NULL,
					NULL, DBTYPE_EMPTY, 						// ppStringsBufferOut & dwModifier
					1, rgColsToBind								// size and array of cols to be bind
					), S_OK))
			{
				odtLog << "problems getting accessor and bindings\n";
			}

			// alloc memory for the row data
			SAFE_ALLOC(pData, BYTE, ulRowSize);

			// get the rowset window 
			m_hr = pIRowset->GetNextRows(0, 0,					// hChapter & lRowsOffset
											1,					// number of rows to be retrieved
											&cRowsObtained,		//number of rows actually get
											&rghRow				// array of handlers to rows
										);
			if (S_OK != m_hr || !COMPARE(1 == cRowsObtained, TRUE))
			{
				odtLog << "after RestartPosition, could not get the rows\n";
				CHECK(m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL), S_OK);
				goto CLEANUP;
			}

			// read data
			CHECK(m_hr = pIRowset->GetData(rghRow[0], hAccessor, pData), S_OK);
			if ((S_OK != m_hr) || 
				((DBSTATUS_S_OK != ((DATA*)pData)->sStatus) &&
				 !COMPARE(DBSTATUS_S_ISNULL == ((DATA*)pData)->sStatus, TRUE) ))
			{
				odtLog << "can't get the data from the newly created column\n";
			}
			else
				odtLog << "data read!\n";
			CHECK(m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL), S_OK);
		}
	}
	else
	{
		odtLog << "DB_E_TABLEINUSE was returned when a column was to be added on a table whith an open rowset.\n";
		CHECK(hr, DB_E_TABLEINUSE);
	}

CLEANUP:
	m_pTable->SetRowset(NULL);
	SAFE_RELEASE(pIRowsetChange2);
	SAFE_RELEASE(pIRowset);
	SAFE_FREE(pData);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	m_pTable->SetNoOfPropertySets(0);
	m_pTable->SetPropertySets(NULL);
	CHECK(m_pTable->DropTable(), S_OK);
	ReleaseDBID(pColumnID);
	FreeAccessorBindings(cBindings, rgBindings);
	SAFE_FREE(pData);
	SAFE_FREE(rghRow);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc *pTableID = DB_NULLID => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_3()
{
	BOOL			fTestRes		= TEST_PASS;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// try to add a column from this table that doesn't exist
	if (!CHECK(m_pITableDefinition->AddColumn((DBID*)&DB_NULLID, rgColumnDesc, &pColumnID), DB_E_NOTABLE))
		fTestRes = TEST_FAIL;

	ReleaseDBID(pColumnID);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc pTableID = NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_4()
{
	BOOL			fTestRes = TEST_PASS;
	DBID			*pColumnID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// try to add a column from this table that doesn't exist
	if (!CHECK(m_hr = m_pITableDefinition->AddColumn(NULL, rgColumnDesc, &pColumnID), E_INVALIDARG))
		fTestRes = TEST_FAIL;

	ReleaseDBID(pColumnID);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Maximum length for table name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_5()
{
	TBEGIN
	DBID			TableID;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);
	PRVTRACE(L"***TCADDColumn::Variation_5(): TableName=%s, len=%d\n", 
		TableID.uName.pwszName, wcslen(TableID.uName.pwszName));

	TESTC_(m_pTable->CreateTable(0, 0, TableID.uName.pwszName), S_OK);

	// try to add a column to this maximum length name table
	// prepare the column to be added
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], &pColumnID), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(&TableID, FALSE);	// don't want to release the pointer, just the container
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc the table name is 1 char too long => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_6()
{
	TBEGIN
	DBID			TableID;
	DBID			*pColumnID = NULL;
	WCHAR			*pwszTableName;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	GetLiteralInfo();
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	pwszTableName			= BuildValidName(m_cMaxTableName+1, m_pTable->GetTableName());
	// prepare arguments for table creation
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);
	PRVTRACE(L"***TCADDColumn::Variation_6(): TableName=%s, len=%d\n", TableID.uName.pwszName, 
		wcslen(TableID.uName.pwszName));

	TESTC_(m_pTable->CreateTable(0, 0, TableID.uName.pwszName), S_OK);
	
	// try to add a column, spec the table name plus a char as a table name
	PROVIDER_FREE(TableID.uName.pwszName);
	TableID.uName.pwszName = pwszTableName;
	TESTC_(AddColumnAndCheck(&TableID, &rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_NOTABLE);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	ReleaseDBID(&TableID, FALSE);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc invalid table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_7()
{
	BOOL			fTestRes = TEST_PASS;
	DBID			TableID;
	DBID			*pColumnID = NULL;
	ULONG			m, n;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	m = min(m_cMaxTableName, n=5+wcslen(m_pTable->GetTableName()));
	TableID.uName.pwszName	= BuildInvalidName(m, m_pTable->GetTableName(), m_pwszInvalidTableChars);
	PRVTRACE(L"***TCADDColumn::Variation_7(): TableName=%s, len=%d\n", TableID.uName.pwszName, 
		wcslen(TableID.uName.pwszName));

	if (!CHECK(m_hr = m_pITableDefinition->AddColumn(&TableID, &rgColumnDesc[0], &pColumnID), DB_E_NOTABLE))
			fTestRes = TEST_FAIL;

	ReleaseDBID(pColumnID);
	ReleaseDBID(&TableID, FALSE);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc pColumnDesc->dbcid = DB_NULLID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_8()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a DB_NULLID colid to this table
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	memcpy(&rgColumnDesc[cColumnDesc-1].dbcid, (DBID*)&DB_NULLID, sizeof(DBID));
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Column name pointer is NULL => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_9()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK); 

	// try to add a column with a NULL value for name pointer
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = NULL;
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
			&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc column name is an empty string => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_10()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);

	// create a table with cColumnDesc-1 columns
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a NULL value for name pointer
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = wcsDuplicate(L"");
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Maximum length of column name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_11()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	WCHAR			*pwszName		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->MakeTableName(NULL);
	pwszName = wcsDuplicate(m_pTable->GetTableName());

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a maximum size for column name 
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = BuildValidName(m_cMaxColName, pwszName);
	odtLog << "A maximum length column name: " << rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName << 
		", length=" << wcslen(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName) << "\n";
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	PROVIDER_FREE(pwszName);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc 1-char-too-long column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_12()
{
	TBEGIN
	WCHAR			*pwszName		= NULL;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->MakeTableName(NULL);
	pwszName = wcsDuplicate(m_pTable->GetTableName());

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a name too long
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = BuildValidName(m_cMaxColName+1, pwszName);
	odtLog << "A 1 char too long column name: " << rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName << 
		", length=" << wcslen(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName) << "\n";
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	SAFE_FREE(pwszName);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Add a column for each provider type, maximum column size => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_13()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i;
	DBCOLUMNDESC	*pColumnDesc = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	DuplicateColumnDesc(rgColumnDesc, 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName);
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"FirstColumn");
	// for each type in DBCOLUMNDESC, add a column with maximum column size
	SetMaximumColSize(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	for (i=0; i<cColumnDesc; i++)
	{
		// create a table with a single column, equal to the first one in the row
		CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK);

		if (FAILED(m_hr))
			continue;

		// try to add a column from rgColumnDesc[i]
		odtLog <<	"column name:" << rgColumnDesc[i].dbcid.uName.pwszName << 
					" column type:" << rgColumnDesc[i].pwszTypeName << "\n";

		CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], 
			&pColumnID), S_OK);

		CHECK(m_pTable->DropTable(), S_OK);
		ReleaseDBID(pColumnID);
		pColumnID = NULL;
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Add a column for each known type; ulColumnsize == 0 => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_14()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i;
	DBCOLUMNDESC	*pColumnDesc = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	DuplicateColumnDesc(rgColumnDesc, 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName);
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"FirstColumn");
	SetZeroColSize(rgColumnDesc, cColumnDesc);
	// for each type in DBCOLUMNDESC, add a column with maximum column size
	m_pTable->SetColumnDesc(NULL);
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	m_pTable->SetBuildColumnDesc(FALSE);

	for (i=0; i<cColumnDesc; i++)
	{
		// create a table with a single column, equal to the first one in the row
		CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK);

		if (FAILED(m_hr))
			continue;

		// try to add a column from rgColumnDesc[i]
		odtLog <<	"column name:" << rgColumnDesc[i].dbcid.uName.pwszName << 
					" column type:" << rgColumnDesc[i].pwszTypeName << "\n";

		CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], 
			&pColumnID), S_OK);

		CHECK(m_pTable->DropTable(), S_OK);
		ReleaseDBID(pColumnID);
		pColumnID = NULL;
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(15)
//--------------------------------------------------------------------
// @mfunc Add a column for each provider type (table is populated) => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_15()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i;
	DBCOLUMNDESC	*pColumnDesc = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	DuplicateColumnDesc(rgColumnDesc, 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName);
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"FirstColumn");

	m_pTable->SetColumnDesc(NULL);
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	m_pTable->SetBuildColumnDesc(FALSE);

	for (i=0; i<cColumnDesc; i++)
	{
		// try to add a column from rgColumnDesc[i]
		odtLog <<	"column name:" << rgColumnDesc[i].dbcid.uName.pwszName << 
				" column type:" << rgColumnDesc[i].pwszTypeName << "\n";

		// create a popoulated table 
		if (!CHECK(m_pTable->CreateTable(10, 0), S_OK))
			continue;

		CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], 
				&pColumnID), S_OK);

		ReleaseDBID(pColumnID);
		pColumnID = NULL;
		CHECK(m_pTable->DropTable(), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Precision testing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_16()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i;
	WCHAR			*pwszColumnName = NULL;
	BYTE			precision, p[6];
	ULONG			nmax = 6;
	WCHAR			*prefix[]={L"zero", L"one", L"half", L"max", L"big", L"extra"};
	HRESULT			hres;
	DBCOLUMNDESC	*rgColDesc = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	DuplicateColumnDesc(rgColumnDesc, 1, &rgColDesc);
	m_pTable->SetColumnDesc(rgColDesc, 1);
	m_pTable->SetBuildColumnDesc(FALSE);
	SAFE_FREE(rgColDesc[0].dbcid.uName.pwszName);
	rgColDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"initialColumn");

	// for each type in DBCOLUMNDESC, add a column with different precision value
	for (i=0; i<cColumnDesc; i++)
	{

		// try to add a column with different precision values
		odtLog <<	"column name:" << rgColumnDesc[i].dbcid.uName.pwszName << 
					" column type:" << rgColumnDesc[i].pwszTypeName << "\n";
		precision = rgColumnDesc[i].bPrecision; // BuildColumnDesc puts max val in bPrecision!
		p[0] = 0;
		p[1] = 1;
		p[2] = precision/2;
		p[3] = precision;
		if (precision > 254)
			nmax = 4;
		p[4] = precision+1;
		p[5] = (ULONG)precision+10>255? 255: precision+10;

		pwszColumnName = rgColumnDesc[i].dbcid.uName.pwszName;
		rgColumnDesc[i].dbcid.uName.pwszName = NULL;
		
		rgColumnDesc[i].bScale = 0;
		rgColumnDesc[i].ulColumnSize = 0;
		
		for (ULONG j=0; j<nmax; j++)
		{
			odtLog << "\t" << p[j];
			if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
				continue;

			// if needed you can limit wcscat with the max length for col size
			rgColumnDesc[i].dbcid.uName.pwszName = (WCHAR*) PROVIDER_ALLOC( sizeof(WCHAR) * m_cMaxColName);
			wcscpy(rgColumnDesc[i].dbcid.uName.pwszName, prefix[j]);
			wcsncat(rgColumnDesc[i].dbcid.uName.pwszName, pwszColumnName, m_cMaxColName-wcslen(prefix[j]));
			rgColumnDesc[i].bPrecision = p[j];
			hres = S_OK;
			if (rgColumnDesc[i].wType == DBTYPE_NUMERIC && ((j>3) || (j==0)) )
				hres = DB_E_BADPRECISION;

			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
							&rgColumnDesc[i], &pColumnID), hres);

			SAFE_FREE(rgColumnDesc[i].dbcid.uName.pwszName);
			rgColumnDesc[i].dbcid.uName.pwszName = NULL;
			CHECK(m_pTable->DropTable(), S_OK);
			ReleaseDBID(pColumnID);
			pColumnID = NULL;
		}
		odtLog << "\n";
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Scale testing
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_17()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i;
	WCHAR			*pwszName;
	DBCOLUMNDESC	*pColumnDesc = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	DuplicateColumnDesc(rgColumnDesc, 1, &pColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName);
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"Initial_column");
	// create a table with a single column, equal to the first one in the row
	m_pTable->SetColumnDesc(NULL);	// so that m_pTable->rgColumnDesc will not be deallocated next
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	m_pTable->SetBuildColumnDesc(FALSE);

	// for each type in DBCOLUMNDESC, add a column with different precision value
	for (i=0; i<cColumnDesc; i++)
	{
		CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK);
		if (!SUCCEEDED(m_hr))
			continue;

		odtLog <<	"column name: " << rgColumnDesc[i].dbcid.uName.pwszName << 
					" column type: " << rgColumnDesc[i].pwszTypeName << "\n";

		if (	rgColumnDesc[i].wType == DBTYPE_NUMERIC 
			||	rgColumnDesc[i].wType == DBTYPE_DECIMAL
			||	rgColumnDesc[i].wType == DBTYPE_VARNUMERIC )
		{
			const int nScales = 7;
			BYTE	smax, smin, p[nScales], s[nScales], precision;
			ULONG	nmax = 1;
			WCHAR	*prefix[]={L"zero", L"min", L"half", L"max", L"maxpp", L"prec", L"extra_prec"};
			HRESULT	hres[nScales] = {S_OK, S_OK, S_OK, S_OK, S_OK, S_OK, S_OK};
			CCol	col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

			// try to add a column with different scale values
			smin = (BYTE)col.GetMinScale();
			smax = (BYTE)col.GetMaxScale();
			precision = (BYTE)col.GetPrecision();
			for (ULONG k=0; k<nScales; p[k++]=precision);
			s[0] = 0;
			hres[0] = (smin==0)? S_OK: DB_E_BADSCALE;
			if (smin > 0)
			{
				s[1]	= smin;
				nmax++;
			}
			s[nmax++] = (smin+smax)/2;
			s[nmax++] = smax;
			if (smax < 255)
			{
				s[nmax]			= smax+1;
				hres[nmax++]	= DB_E_BADSCALE;
				// check matching the precision
				s[nmax]		= precision;
				hres[nmax]	= (precision>smax) ? DB_E_BADSCALE: S_OK;
				nmax++;
				s[nmax]		= p[nmax]+1;
				hres[nmax++]= DB_E_BADSCALE;
			}

			pwszName = rgColumnDesc[i].dbcid.uName.pwszName;
			rgColumnDesc[i].dbcid.uName.pwszName = NULL;	// protection for inside the loop!
			for (ULONG j=0; j<nmax; j++)
			{
				odtLog << "\t" << s[j];

				// if needed you can limit wcscat with the max length for col size
				if (rgColumnDesc[i].dbcid.uName.pwszName)
					SAFE_FREE(rgColumnDesc[i].dbcid.uName.pwszName);
				rgColumnDesc[i].dbcid.uName.pwszName = (WCHAR*) PROVIDER_ALLOC( sizeof(WCHAR) * m_cMaxColName);
				wcscpy(rgColumnDesc[i].dbcid.uName.pwszName, prefix[j]);
				wcsncat(rgColumnDesc[i].dbcid.uName.pwszName, pwszName, m_cMaxColName-wcslen(prefix[j]));
				rgColumnDesc[i].bPrecision = p[j];
				rgColumnDesc[i].bScale = s[j];

				CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
					&rgColumnDesc[i], &pColumnID), hres[j]);
				ReleaseDBID(pColumnID);
				pColumnID = NULL;
			}
			odtLog << "\n";
			SAFE_FREE(pwszName);
		}
		else
		{
			rgColumnDesc[i].bScale = 128;
			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
						&rgColumnDesc[i], &pColumnID), S_OK);
			ReleaseDBID(pColumnID);
			pColumnID = NULL;
		}
		CHECK(m_pTable->DropTable(), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	m_pTable->SetNoOfColumnDesc(1);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Invalid value for the property of a column => DB_E_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_18()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	DBPROPID		dwPropID = DBPROP_COL_UNIQUE;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a bad property value (prop of a different type than expected)
	SetProperty(&rgColumnDesc[cColumnDesc-1].rgPropertySets, 
				&rgColumnDesc[cColumnDesc-1].cPropertySets, 
				dwPropID, 
				VT_I2, (LPVOID)12, 
				DBPROPOPTIONS_REQUIRED);

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_ERRORSOCCURRED);
	TESTC(VerifyPropStatus(rgColumnDesc[cColumnDesc-1].cPropertySets, 
		rgColumnDesc[cColumnDesc-1].rgPropertySets, dwPropID, DBPROPSET_COLUMN, 
		DBPROPSTATUS_BADVALUE));

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Non-column property => DBPROPSTATUS_NOTSUPPORTED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_19()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	DBPROPID		dwPropID = DBPROP_COL_UNIQUE;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
		
	// try to add a column with a bad property guid (DBPROPSET_INDEX)
	SetProperty(
		&rgColumnDesc[cColumnDesc-1].rgPropertySets, &rgColumnDesc[cColumnDesc-1].cPropertySets,
		dwPropID, 
		VT_BOOL, (LPVOID)VARIANT_TRUE, 
		DBPROPOPTIONS_OPTIONAL, DB_NULLID, DBPROPSET_INDEX);
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
			&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_S_ERRORSOCCURRED);
	TESTC(VerifyPropStatus(rgColumnDesc[cColumnDesc-1].cPropertySets, 
		rgColumnDesc[cColumnDesc-1].rgPropertySets, dwPropID, DBPROPSET_INDEX, DBPROPSTATUS_NOTSUPPORTED));

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Inexistent property => DBPROPSTATUS_NOTSUPPORTED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_20()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK); 

	// try to add a column with a bad property id 
	SetProperty(&rgColumnDesc[cColumnDesc-1].rgPropertySets, 
		&rgColumnDesc[cColumnDesc-1].cPropertySets, 0xffffffff, VT_BOOL, (LPVOID)VARIANT_TRUE);

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
			&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_S_ERRORSOCCURRED);

	TESTC(VerifyPropStatus(rgColumnDesc[cColumnDesc-1].cPropertySets, 
		rgColumnDesc[cColumnDesc-1].rgPropertySets, 0xffffffff, 
		DBPROPSET_COLUMN, DBPROPSTATUS_NOTSUPPORTED));

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Bad option => DBPROPSTATUS_BADOPTION
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_21()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBPROPID		dwPropID		= DBPROP_COL_NULLABLE;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	
	// try to add a column with a bad option
	SetProperty(&rgColumnDesc[cColumnDesc-1].rgPropertySets, &rgColumnDesc[cColumnDesc-1].cPropertySets,
		dwPropID, 
		VT_BOOL, (LPVOID)VARIANT_TRUE, 
		0xf0);
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_ERRORSOCCURRED);

	TESTC(VerifyPropStatus(rgColumnDesc[cColumnDesc-1].cPropertySets,
		rgColumnDesc[cColumnDesc-1].rgPropertySets, dwPropID,
		DBPROPSET_COLUMN,DBPROPSTATUS_BADOPTION));

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Conflicting property => DBPROPSTATUS_CONFLICTING
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_22()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	ULONG			i, nCol;
	CCol			col;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	ULONG			nConfStatus		= 0;
	ULONG			nOKStatus		= 0;

	TESTC_PROVIDER(SettableProperty(DBPROP_COL_PRIMARYKEY, DBPROPSET_COLUMN)
		&&	SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	nCol = cColumnDesc;
	for (i=0; i< cColumnDesc; i++)
	{
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		if (col.GetNullable()!=0)
			nCol = i;
	}

	TESTC(nCol < cColumnDesc);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	// try to add a column with conflicting properties 
	PROVIDER_FREE(rgColumnDesc[nCol].dbcid.uName.pwszName);
	rgColumnDesc[nCol].dbcid.uName.pwszName = wcsDuplicate(L"MyColumn");
	SetProperty(&rgColumnDesc[nCol].rgPropertySets, &rgColumnDesc[nCol].cPropertySets,
		DBPROP_COL_PRIMARYKEY, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	SetProperty(&rgColumnDesc[nCol].rgPropertySets, &rgColumnDesc[nCol].cPropertySets,
		DBPROP_COL_NULLABLE, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[nCol], &pColumnID), DB_E_ERRORSOCCURRED);
	
	if (DBPROPSTATUS_CONFLICTING == rgColumnDesc[nCol].rgPropertySets[0].rgProperties[0].dwStatus)
		nConfStatus++;
	else 
	{
		TESTC(DBPROPSTATUS_OK == rgColumnDesc[nCol].rgPropertySets[0].rgProperties[0].dwStatus);
	}

	if (DBPROPSTATUS_CONFLICTING == rgColumnDesc[nCol].rgPropertySets[0].rgProperties[1].dwStatus)
		nConfStatus++;
	else 
	{
		TESTC(DBPROPSTATUS_OK == rgColumnDesc[nCol].rgPropertySets[0].rgProperties[1].dwStatus);
	}

	TESTC(0 < nConfStatus);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(23)
//--------------------------------------------------------------------
// @mfunc Bad column ID for a property => ignored
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_23()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBID			colID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	ULONG			cCol;
	CCol			col;
	BOOL			fFound			= FALSE;
	BOOL			fExists;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	for (cCol=0; cCol < m_cColumnDesc; cCol++)
	{
		col = GetType(rgColumnDesc[cCol].pwszTypeName, rgColumnDesc[cCol].wType, &fExists);
		fFound = fExists && 1 == col.GetNullable();
		break;
	}

	TESTC_PROVIDER(fFound && SupportedProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN));

	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a property that set coldid to smthg different from DB_NULLID
	colID.eKind = DBKIND_NAME;
	colID.uName.pwszName = wcsDuplicate(L"bogus");
	SetProperty(&rgColumnDesc[cCol].rgPropertySets, &rgColumnDesc[cCol].cPropertySets,
		DBPROP_COL_NULLABLE, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL, colID);
	if (DBKIND_NAME == rgColumnDesc[cCol].dbcid.eKind && rgColumnDesc[cCol].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cCol].dbcid.uName.pwszName);
	rgColumnDesc[cCol].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[cCol].dbcid.uName.pwszName = wcsDuplicate(L"ExtraColumn");
		
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cCol], 
		&pColumnID), S_OK);

	TESTC(VerifyPropStatus(rgColumnDesc[cCol].cPropertySets,
		rgColumnDesc[cCol].rgPropertySets, DBPROP_COL_NULLABLE,
		DBPROPSET_COLUMN, DBPROPSTATUS_OK));

CLEANUP:
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Bad column type (wType in DBCOLUMNDESC)
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_24()
{
	TBEGIN
	DBID			*pColumnID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].pwszTypeName);
	rgColumnDesc[cColumnDesc-1].pwszTypeName = NULL;
	rgColumnDesc[cColumnDesc-1].wType = 0xFFFF;
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADTYPE);

CLEANUP:
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Invalid provider type name for a column => DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_25()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].pwszTypeName);
	rgColumnDesc[cColumnDesc-1].pwszTypeName = wcsDuplicate(L"New_type_name");
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADTYPE);


CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc NULL ppColumnDesc => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_26()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK); 

	// try to add a column with a bad wType
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
			&rgColumnDesc[cColumnDesc-1], NULL), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc Autoincrementable column - DBPROP_COL_AUTOINCREMENT
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_27()
{
	TBEGIN
	DBCOLUMNDESC	*pColumnDesc = NULL;
	ULONG			i;
	CCol			col;
	HRESULT			hres1;	// for DBPROP_COL_AUTOINCREMENT set to VARIANT_TRUE
	HRESULT			hres2;	// for DBPROP_COL_AUTOINCREMENT set to VARIANT_FALSE
	HRESULT			hr;
	BOOL			fReadOnlyAutoInc;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	BOOL			fSupported		= TRUE;
	ULONG			ulStatus;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);

	fReadOnlyAutoInc = !SettableProperty(DBPROP_COL_AUTOINCREMENT, DBPROPSET_COLUMN);
	fSupported		 = SupportedProperty(DBPROP_COL_AUTOINCREMENT, DBPROPSET_COLUMN);
	if (!fSupported)
		odtLog << "DBPROP_COL_AUTOINCREMENT is NOT SUPPORTED!\n";
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);

	SAFE_FREE(pColumnDesc->dbcid.uName.pwszName)
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	m_pTable->SetColumnDesc(pColumnDesc, 1);

	for (i=0; i<cColumnDesc; i++)
	{
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		if (fSupported)
		{
			hres1 = (col.CanAutoInc()) ? S_OK : DB_E_ERRORSOCCURRED;
			hres2 = (!col.CanAutoInc() || !fReadOnlyAutoInc) ? S_OK : DB_E_ERRORSOCCURRED;
			odtLog	<< rgColumnDesc[i].pwszTypeName 
					<< ((col.CanAutoInc()) ? " is AUTOINCREMENTABLE\n" : " is NOT autoincrementable\n");
		}
		else
		{
			hres1 = hres2 = DB_E_ERRORSOCCURRED;
		}

		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK)) 
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as an AUTOINCREMENT COLUMN
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_AUTOINCREMENT, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;

		CHECK(hr = AddColumnAndCheck(&m_pTable->GetTableID(), 
					&rgColumnDesc[i], NULL), hres1);
		
		odtLog << (SUCCEEDED(hr) ? "\tautoinc column added\n" : "\tautoinc column could not be added\n");
		//check the state
		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (S_OK == hr)
		{
			COMPARE(ulStatus, DBPROPSTATUS_OK);
			COMPARE(fSupported, TRUE);
			COMPARE(col.CanAutoInc(), TRUE);
		}
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);

			if (!fSupported)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fReadOnlyAutoInc)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
			else
				COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
		}


		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
		
		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as an non autoincrement column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_AUTOINCREMENT, VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		CHECK(hr = AddColumnAndCheck(&m_pTable->GetTableID(), 
					&rgColumnDesc[i], NULL), hres2);

		odtLog << (SUCCEEDED(hr) ? "\tNOT autoinc column added\n" : "\tNOT autoinc column could not be added\n");
		// check the returned status
		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;

		if (S_OK == hr)
		{
			COMPARE(ulStatus, DBPROPSTATUS_OK);
			COMPARE(fSupported, TRUE);
		}
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
			COMPARE(!col.CanAutoInc(), TRUE);

			if (!fSupported)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fReadOnlyAutoInc)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
			else
				COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_COL_DEFAULT
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_28()
{
	TBEGIN
	DBCOLUMNDESC			*pColumnDesc=NULL;
	ULONG					i;
	HRESULT					hr;
	VARIANT					value;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CTable					*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	BOOL					fSettable = TRUE;
	BOOL					fSupported = TRUE;
	ULONG					ulStatus;

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName);
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	ASSERT(pColumnDesc->dbcid.uName.pwszName);
	m_pTable->SetColumnDesc(pColumnDesc, 1);

	if (!SupportedProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
	{
		odtLog << "DBPROP_COL_DEFAULT is NOT SUPPORTED.\n";
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
	{
		if (!SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
			fSettable = FALSE;
	}

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << rgColumnDesc[i].pwszTypeName;
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a column with a default value
		VariantInit(&value);
		GetDefaultValue(&rgColumnDesc[i], &value, pTable);
		SetDefaultProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
		VariantClear(&value);
		//rgColumnDesc[i].ulColumnSize = 0;

		// since the property could be supported, writeable but unavailable for
		// certain data types, a return value cannot be enforced
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (S_OK == hr)
		{
			odtLog << "\t: column has been added with a DEFAULT VALUE\n";
			COMPARE(ulStatus, DBPROPSTATUS_OK);
			COMPARE(fSupported, TRUE);
		}
		else 
		{
			if (CHECK(hr, DB_E_ERRORSOCCURRED))
			{
				odtLog << "\t: column was added WITHOUT default value\n";
				if (!fSupported)
					COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
				// property could be settable but failing for this data type
				else if (fSettable)
					COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
				else
					COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
			}
			else
			{
				odtLog << "\tERROR: column was NOT added\n";
			}
		}
		CHECK(m_pTable->DropTable(), S_OK);
		// the vt type of the default property could cause problems in VariantClear
		// call PrepareVariantToClear to check and fix
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	delete pTable;
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_COL_DESCRIPTION
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_29()
{
	TBEGIN
	ULONG			i;
	WCHAR			*pwszName		= NULL;
	HRESULT			hr;
	BSTR			pwszDescription = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	HRESULT			hExpRes			= S_OK;
	BOOL			fSettable		= TRUE;
	BOOL			fSupported		= TRUE;
	ULONG			ulStatus;

	if (!SupportedProperty(DBPROP_COL_DESCRIPTION, DBPROPSET_COLUMN))
	{
		odtLog << "DBPROP_COL_DESCRIPTION is NOT SUPPORTED.\n";
		hExpRes		= DB_S_ERRORSOCCURRED;
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
	{
		if (!SettableProperty(DBPROP_COL_DESCRIPTION, DBPROPSET_COLUMN))
		{
			fSettable = FALSE;
			odtLog << "Implementing the desricption property as readonly is strange. Please reconsider!\n";
		}
	}

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);
	pwszName = rgColumnDesc[0].dbcid.uName.pwszName;
	m_pTable->SetColumnDesc(rgColumnDesc, 1);
	pwszDescription = SysAllocString(L"ColumnDescription");

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "column of type " << rgColumnDesc[i].pwszTypeName;

		rgColumnDesc[0].dbcid.uName.pwszName = L"First_column";
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
		{
			if (SUCCEEDED(m_hr))
				CHECK(m_pTable->DropTable(), S_OK);
			continue;
		}

		rgColumnDesc[0].dbcid.uName.pwszName = pwszName;

		// try to add the column described by rgColumnDesc[i] to the table
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, 
			DBPROP_COL_DESCRIPTION, DBTYPE_BSTR, (LPVOID)pwszDescription, DBPROPOPTIONS_OPTIONAL);
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);
				
		if (!fSupported)
			CHECK(hr, DB_S_ERRORSOCCURRED);
		else if (fSettable)
			CHECK(hr, S_OK);
		else if (S_OK != hr)
			CHECK(hr, DB_S_ERRORSOCCURRED);

		if (SUCCEEDED(hr))
			odtLog << ((DB_S_ERRORSOCCURRED == hr) ? ": could not add a description\n" : 
					": description has been added\n");

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus; 
		if (!fSupported)
			COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
		else if (fSettable)
			COMPARE(ulStatus, DBPROPSTATUS_OK);
		else
			COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);

		CHECK(m_pTable->DropTable(), S_OK);
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
	}

	SysFreeString(pwszDescription);
	rgColumnDesc[0].dbcid.uName.pwszName = pwszName;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_COL_FIXEDLENGTH
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_30()
{
	TBEGIN
	HRESULT			hr;
	DBCOLUMNDESC	*pColumnDesc = NULL;
	BOOL			fFixedLength;
	ULONG			i;
	CCol			col;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	BOOL			fSettable = TRUE;
	BOOL			fSupported = TRUE;
	ULONG			ulStatus;

	if (!SupportedProperty(DBPROP_COL_FIXEDLENGTH, DBPROPSET_COLUMN))
	{
		odtLog << "DBPROP_COL_FIXEDLENGTH is NOT SUPPORTED\n";
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
	{
		if (!SettableProperty(DBPROP_COL_FIXEDLENGTH, DBPROPSET_COLUMN))
			fSettable = FALSE;
	}

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	SetMaximumColSize(rgColumnDesc, cColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName)
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	m_pTable->SetColumnDesc(pColumnDesc, 1);

	for (i=0; i<cColumnDesc; i++)
	{
		if (	DBTYPE_STR != rgColumnDesc[i].wType
			&&	DBTYPE_WSTR != rgColumnDesc[i].wType
			&&	DBTYPE_BYTES != rgColumnDesc[i].wType)
			continue;

		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		// just not yet implemented
		// pending for all providers to adjust the provider types schema rowset
		fFixedLength = col.GetIsFixedLength();
		if (fFixedLength)
			odtLog << rgColumnDesc[i].pwszTypeName <<" is fixed length\n";
		else
			odtLog << rgColumnDesc[i].pwszTypeName <<" is variable length\n";

		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a fixed length column
		odtLog << "\ttry to add a fixed length column: ";
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_FIXEDLENGTH, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;

		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (S_OK == hr)
			COMPARE(ulStatus, DBPROPSTATUS_OK);
		else 
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
			
			if (!fSupported)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fSettable)
				COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
			else if (!fSettable)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
		}

		odtLog << ((S_OK == hr)? "ok\n" : "failed\n");
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
		
		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a variable_length
		odtLog << "\ttry to add a variable length column: ";
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_FIXEDLENGTH, VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[i], NULL);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (S_OK == hr)
			COMPARE(ulStatus, DBPROPSTATUS_OK);
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
			if (!fSupported)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fSettable)
				COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
			else 
				COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
		}

		odtLog << ((S_OK == hr)? "ok\n" : "failed\n");

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_COL_NULLABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_31()
{
	TBEGIN
	DBCOLUMNDESC			*pColumnDesc = NULL;
	BOOL					fNotNullable, fNullable;
	ULONG					i;
	VARIANT					value, defval;
	CCol					col;
	HRESULT					hr;
	DBPROP					*pProp			= NULL;
	DBPROP					*pPropDefault	= NULL;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CTable					*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	BOOL					fSettable = TRUE;
	BOOL					fSupported = TRUE;
	ULONG					ulStatus;

	if (!SupportedProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN))
	{
		odtLog << "DBPROP_COL_NULLABLE is NOT SUPPORTED\n";
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
	{
		if (!SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN))
			fSettable = FALSE;
	}

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName)
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	value.vt	= VT_BOOL;

	for (i=0; i<cColumnDesc; i++)
	{
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		fNotNullable	= FALSE;
		fNullable		= FALSE;
		switch (col.GetNullable())
		{
			case 1:
				odtLog << rgColumnDesc[i].pwszTypeName <<" is nullable: \n";
				fNullable = TRUE;
				break;
			case 0:
				odtLog << rgColumnDesc[i].pwszTypeName <<" is not nullable: \n";
				fNotNullable = TRUE;
				break;
			default:
				odtLog << rgColumnDesc[i].pwszTypeName <<": nullable/not nullable information was not found\n";
				break;
		}

		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a nullable one
		//odtLog << "try to add a nullable column\n";
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_NULLABLE, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		
		// try to add a column but do not require the seccess of the operation
		// though the column is declared nullable, adding a column could be unimplemented in the provider
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		if (fNullable)
			CHECK(hr, S_OK);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		
		if (S_OK == hr)
			COMPARE(ulStatus, DBPROPSTATUS_OK);
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
			if (!fSupported)
				COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fSettable)
				COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
			else
				COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
		
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a variable_length
		//odtLog << "try to add a non nullable column\n";
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_NULLABLE, VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		VariantInit(&defval);
		GetDefaultValue(&rgColumnDesc[i], &defval, pTable);
		if (SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
			SetDefaultProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
				&defval, DBPROPOPTIONS_REQUIRED);	// might be not supported
		VariantClear(&defval);
		
		// it is not necessary that a not nullable column be added
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (S_OK == hr)
			COMPARE(ulStatus, DBPROPSTATUS_OK);
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
			if (DBPROPSTATUS_OK == rgColumnDesc[i].rgPropertySets[0].rgProperties[1].dwStatus)
			{
				if (!fSupported)
					COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED);
				else if (fSettable)
					COMPARE(ulStatus, DBPROPSTATUS_BADVALUE);
				else
					COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE);
			}
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	delete pTable;
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_PRIMARYKEY
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_32()
{
	TBEGIN
	DBCOLUMNDESC			*pColumnDesc = NULL;
	ULONG					i;
	VARIANT					value;
	CCol					col;
	HRESULT					hr;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CTable					*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	BOOL					fSettable;
	BOOL					fDefaultIsSettable = SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN);

	fSettable = SettableProperty(DBPROP_COL_PRIMARYKEY, DBPROPSET_COLUMN);

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	// this is to prevent SQL Server limiting the size of
	// the index it creates for primary key or unique columns
	SetZeroColSize(rgColumnDesc, cColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName)
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	m_pTable->SetColumnDesc(pColumnDesc, 1);

	for (i=0; i<cColumnDesc; i++)
	{
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		odtLog << rgColumnDesc[i].pwszTypeName << "\n";
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a PRIMARY KEY  column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_PRIMARYKEY, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);
		if (fDefaultIsSettable)
		{
			VariantInit(&value);
			GetDefaultValue(&rgColumnDesc[i], &value, pTable);
			SetDefaultProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
				&value, DBPROPOPTIONS_OPTIONAL);	// might be not supported
			VariantClear(&value);
		}
		rgColumnDesc[i].bScale = 0;
		
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		if (hr != S_OK)
		{
			CHECK(hr, DB_S_ERRORSOCCURRED);
			COMPARE(FALSE == fSettable
				||	DBPROPSTATUS_OK != rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus
				||	DBPROPSTATUS_OK != rgColumnDesc[i].rgPropertySets[0].rgProperties[1].dwStatus, 
				TRUE);
		}
		else
		{
			COMPARE(fSettable, TRUE);
			odtLog << "\tcolumn added as a primary key\n";
			COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_OK);
		}

		
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
		
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a PRIMARY KEY column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_PRIMARYKEY, VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_OPTIONAL);
		rgColumnDesc[i].bScale = 0;
		
		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		if (hr != S_OK)
		{
			COMPARE(fSettable, FALSE);
			CHECK(hr, DB_S_ERRORSOCCURRED);
		}
		else
		{
			COMPARE(fSettable, TRUE);
			odtLog << "\tcolumn was added with primary property off\n";
			COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_OK);
		}
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	SAFE_DELETE(pTable);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc DBPROP_COL_UNIQUE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_33()
{
	TBEGIN
	DBCOLUMNDESC	*pColumnDesc = NULL;
	ULONG			i;
	CCol			col;
	HRESULT			hr;
	DBPROP			*pProp;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	BOOL			fSettable		= FALSE;
	BOOL			fSupported		= FALSE;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	// this is to prevent SQL Server limiting the size of
	// the index it creates for primary key or unique columns
	SetZeroColSize(rgColumnDesc, cColumnDesc);
	PROVIDER_FREE(pColumnDesc->dbcid.uName.pwszName)
	pColumnDesc->dbcid.uName.pwszName = wcsDuplicate(L"First_column");
	m_pTable->SetColumnDesc(pColumnDesc, 1);
	
	fSupported	= SupportedProperty(DBPROP_COL_UNIQUE, DBPROPSET_COLUMN);
	fSettable	= SettableProperty(DBPROP_COL_UNIQUE, DBPROPSET_COLUMN);

	for (i=0; i<cColumnDesc; i++)
	{
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a UNIQUE column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_UNIQUE, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		odtLog << rgColumnDesc[i].pwszTypeName << "\n";
		odtLog << "\ttry to add a unique column; \n";

		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		FindProperty(
					DBPROP_COL_UNIQUE,
					DBPROPSET_COLUMN,
					rgColumnDesc[i].cPropertySets,
					rgColumnDesc[i].rgPropertySets, 
					&pProp);
		
		if (S_OK == hr)
		{
			COMPARE(pProp->dwStatus, DBPROPSTATUS_OK);
			COMPARE(fSupported, TRUE);
		}
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
		
			if (!fSupported)
				COMPARE(pProp->dwStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fSettable)
				COMPARE(pProp->dwStatus, DBPROPSTATUS_BADVALUE);
			else
				COMPARE(pProp->dwStatus, DBPROPSTATUS_NOTSETTABLE);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
		
		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK))
			continue;

		// try to add the column described by rgColumnDesc[i] to the table
		// and set it as a unique column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			DBPROP_COL_UNIQUE, VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
		rgColumnDesc[i].bScale = 0;
		odtLog << "\ttry to add a not unique column; \n";

		hr = AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], NULL);

		FindProperty(
						DBPROP_COL_UNIQUE,
						DBPROPSET_COLUMN,
						rgColumnDesc[i].cPropertySets,
						rgColumnDesc[i].rgPropertySets, 
						&pProp
					);
		
		if (S_OK == hr)
		{
			COMPARE(pProp->dwStatus, DBPROPSTATUS_OK);
			COMPARE(fSupported, TRUE);
		}
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);
		
			if (!fSupported)
				COMPARE(pProp->dwStatus, DBPROPSTATUS_NOTSUPPORTED);
			else if (fSettable)
				COMPARE(pProp->dwStatus, DBPROPSTATUS_BADVALUE);
			else
				COMPARE(pProp->dwStatus, DBPROPSTATUS_NOTSETTABLE);
		}
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		CHECK(m_pTable->DropTable(), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(34)
//*-----------------------------------------------------------------------
// @mfunc Duplicate an existing column => DB_E_DUPLCATECOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_34()
{
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column to this maximum length name table
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID),DB_E_DUPLICATECOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(35)
//*-----------------------------------------------------------------------
// @mfunc Abort retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_35()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD			= NULL;
	DBCOLUMNDESC		*oldCD			= NULL;
	ULONG				newlen			= 0;
	ULONG				oldlen			= 0;
	DBID				*pColumnID		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;
	BOOL				fTxnStarted		= FALSE;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
		
	// create a table and populate it
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(StartTransaction(pITransactionLocal), S_OK);
	fTxnStarted = TRUE;

	// try to add a column to this maximum length name table
	if (rgColumnDesc[0].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.uName.pwszName = L"transacted_column";

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[0], &pColumnID), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	// abort retaining
	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		CHECK(pITransactionLocal->Abort(NULL, TRUE, FALSE), S_OK);
		// check that it is retaining (another transaction is available)
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (	DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		||	DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal)
	{	// check column was added
		COMPARE(CheckColInfo(oldCD, oldlen, newCD, newlen, &rgColumnDesc[0]), TRUE);
	}
	else
	{	// check column was not added
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);
	newCD	= NULL;
	newlen	= 0;

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	
	SAFE_RELEASE(pITransactionLocal);
	if (rgColumnDesc)
		rgColumnDesc[0].dbcid.uName.pwszName = NULL;
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(36)
//*-----------------------------------------------------------------------
// @mfunc Abort without retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_36()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD			= NULL;
	DBCOLUMNDESC		*oldCD			= NULL;
	ULONG				newlen			= 0;
	ULONG				oldlen			= 0;
	DBID				*pColumnID		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;
	BOOL				fTxnStarted		= FALSE;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
		
	// create a table and populate it
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(StartTransaction(pITransactionLocal), S_OK);
	fTxnStarted = TRUE;

	// try to add a column to this maximum length name table
	if (rgColumnDesc[0].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.uName.pwszName = L"transacted_column";

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[0], &pColumnID), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	// abort without retaining
	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort without retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			SUCCEEDED(m_hr)? XACT_E_NOTRANSACTION : S_OK);
	}
	// check column was not added
	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (	DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		||	DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal)
	{	// check column was added
		COMPARE(CheckColInfo(oldCD, oldlen, newCD, newlen, &rgColumnDesc[0]), TRUE);
	}
	else
	{	// check column was not added
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);
	newCD	= NULL;
	newlen	= 0;

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	
	SAFE_RELEASE(pITransactionLocal);
	if (rgColumnDesc)
		rgColumnDesc[0].dbcid.uName.pwszName = NULL;
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(37)
//*-----------------------------------------------------------------------
// @mfunc Commit with retain
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_37()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal		= NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD	= NULL;
	ULONG				newlen = 0, oldlen		= 0;
	DBID				*pColumnID				= NULL;
	DBCOLUMNDESC		*rgColumnDesc			= NULL;
	ULONG				cColumnDesc				= 0;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	
	// create a table and populate it
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// try to add a column to this maximum length name table
	if (rgColumnDesc[0].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[0].dbcid.uName.pwszName);

	rgColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"transacted_column");

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[0], &pColumnID), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}
	
	// commit with retaining
	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		CHECK(pITransactionLocal->Commit(TRUE, 0, 0), S_OK); //XACTTC_SYNC, 0), S_OK))
		//check the commit with retaining 
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal)
	{	// check column was not added
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	else
	{	// check column was added
		COMPARE(CheckColInfo(oldCD, oldlen, newCD, newlen, &rgColumnDesc[0]), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);
	newCD	= NULL;
	newlen	= 0;

CLEANUP:
	if (rgColumnDesc)
		rgColumnDesc[0].dbcid.uName.pwszName = NULL;
	SAFE_RELEASE(pITransactionLocal);
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(38)
//*-----------------------------------------------------------------------
// @mfunc Commit without retain
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCAddColumn::Variation_38()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal		= NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD	= NULL;
	ULONG				newlen = 0, oldlen		= 0;
	DBID				*pColumnID				= NULL;
	DBCOLUMNDESC		*rgColumnDesc			= NULL;
	ULONG				cColumnDesc				= 0;
	VARIANT				vSupportedTxnDDL;
	BOOL				fDDL_LOCK				= FALSE;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	
	// create a table and populate it
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);
	// start the transaction
	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// try to add a column to this maximum length name table
	if (rgColumnDesc[0].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.uName.pwszName = L"transacted_column";

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[0], &pColumnID), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_pITableDefinition->AddColumn(
				&m_pTable->GetTableID(), 
				&rgColumnDesc[0], 
				&pColumnID), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit without retaining
		CHECK(m_hr = pITransactionLocal->Commit(FALSE, XACTTC_SYNC, 0), S_OK);
		//check the commit without retaining 
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), FAILED(m_hr)? S_OK: XACT_E_NOTRANSACTION);
	}

	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal)
	{	// check column was not added
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	else
	{	// check column was added
		COMPARE(CheckColInfo(oldCD, oldlen, newCD, newlen, &rgColumnDesc[0]), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);

CLEANUP:
	if (rgColumnDesc)
		rgColumnDesc[0].dbcid.uName.pwszName = NULL;
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	SAFE_RELEASE(pITransactionLocal);
	ReleaseDBID(pColumnID);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(39)
//*-----------------------------------------------------------------------
// @mfunc Add a column for each provider type in an empty table => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_39()
{ 
	TBEGIN
	ULONG			i;
	WCHAR			*pwszName;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	pwszName = rgColumnDesc[0].dbcid.uName.pwszName;
	m_pTable->SetColumnDesc(rgColumnDesc, 1);

	// for each type in DBCOLUMNDESC, add a column with maximum column size
	rgColumnDesc[0].dbcid.uName.pwszName = pwszName;
	for (i=0; i<cColumnDesc; i++)
	{
		// try to add a column from rgColumnDesc[i]
		odtLog <<	"column name:" << rgColumnDesc[i].dbcid.uName.pwszName << 
					" column type:" << rgColumnDesc[i].pwszTypeName << "\n";

		rgColumnDesc[0].dbcid.uName.pwszName = L"Initial_column";
		// create a table with a single column, equal to the first one in the row
		if (!CHECK(m_pTable->CreateTable(0, 0), S_OK))
			continue;

		rgColumnDesc[0].dbcid.uName.pwszName = pwszName;

		CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], &pColumnID), S_OK);
		CHECK(m_pTable->DropTable(), S_OK);
		ReleaseDBID(pColumnID);
		pColumnID = NULL;
	}

	rgColumnDesc[0].dbcid.uName.pwszName = pwszName;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(40)
//*-----------------------------------------------------------------------
// @mfunc Add a column to a table created with a SQL command => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_40()
{ 
	TBEGIN
	DBID				*pColumnID			= NULL;
	BOOL				fTableCreation;
	DBCOLUMNDESC		*rgColumnDesc		= NULL;
	ULONG				cColumnDesc			= 0;
	IDBCreateCommand	*pIDBCreateCommand	= NULL;

	// check wheter the commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	// set order for table creation
	fTableCreation = GetModInfo()->UseITableDefinition(FALSE);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column to this maximum length name table
	SAFE_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = wcsDuplicate(L"new_column");
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], &pColumnID), S_OK);

CLEANUP:
	m_pTable->DropTable();
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	GetModInfo()->UseITableDefinition(fTableCreation);
	ReleaseDBID(pColumnID);
	SAFE_RELEASE(pIDBCreateCommand);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(41)
//*-----------------------------------------------------------------------
// @mfunc Drop all the column of a table (via SQL stmt) and then try to add a column => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_41()
{ 
	TBEGIN
	DBID			*pColumnID = NULL;
	BOOL			fTableCreation;
	LONG			rgCol[1] = {1};
	LONG			*prgCol = rgCol;
	ULONG			nCol = 1;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;
	IDBCreateCommand	*pIDBCreateCommand	= NULL;

	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	// create a table with a single column, using a SQL command
	fTableCreation = GetModInfo()->UseITableDefinition(FALSE);
	m_pTable->SetBuildColumnDesc(TRUE);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// drop the only existing column from the table
	TESTC_PROVIDER(S_OK == m_pTable->ExecuteCommand(
						ALTER_TABLE_DROP_COLUMN,
						IID_NULL,
						NULL,						// the table name will be the implicit one
						NULL,						// don't return the command statement
						&nCol, 
						(LONG**)&prgCol,
						EXECUTE_IFNOERROR			// execute the command
						));

	// try to add a column to this table
	SAFE_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"new_column");
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[0], &pColumnID), S_OK);
	
	CHECK(m_pTable->DropTable(), S_OK);

CLEANUP:
	GetModInfo()->UseITableDefinition(fTableCreation);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pColumnID);
	SAFE_RELEASE(pIDBCreateCommand);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(42)
//*-----------------------------------------------------------------------
// @mfunc Non NULL pTypeInfo in DBCOLUMNDESC => DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_42()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a NULL value for name pointer
	rgColumnDesc[cColumnDesc-1].pTypeInfo = (ITypeInfo*)m_pITableDefinition;
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), DB_E_BADTYPE);			

CLEANUP:
	m_pTable->DropTable();
	rgColumnDesc[cColumnDesc-1].pTypeInfo = NULL;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(43)
//*-----------------------------------------------------------------------
// @mfunc Missmatch between wType and pwszTypeName => DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_43()
{ 
	TBEGIN
	ULONG			i;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	// try to add a column with a NULL value for name pointer
	for (i=0; i<cColumnDesc-1 && rgColumnDesc[i].wType == rgColumnDesc[cColumnDesc-1].wType; i++);
	TESTC_PROVIDER(i < cColumnDesc-2);

	if (rgColumnDesc[cColumnDesc-1].pwszTypeName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].pwszTypeName);
	rgColumnDesc[cColumnDesc-1].pwszTypeName = wcsDuplicate(rgColumnDesc[i].pwszTypeName);
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), DB_E_BADTYPE);

CLEANUP:
	m_pTable->DropTable();
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(44)
//*-----------------------------------------------------------------------
// @mfunc Add a not nullable column and try to insert a null value for it
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_44()
{ 
	TBEGIN
	HRESULT			hr;
	DBID			*pColumnID = NULL;
	ULONG			i;
	ULONG			nCol[1];
	CCol			col;
	IRowsetChange	*pIRowsetChange = NULL;
	BOOL			fFound;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	VARIANT			defval;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	
	if (!SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN) &&	2 > m_cColumnDesc)
	{
		TESTB = TEST_SKIPPED;
		goto CLEANUP1;
	}

	m_pTable->SetBuildColumnDesc(FALSE);

	// use all the types from the not nullable one (exclusive)
	m_pTable->SetColumnDesc(&rgColumnDesc[1], cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 0), S_OK);

	if (rgColumnDesc[0].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.uName.pwszName = L"NewColumn";
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets, DBPROP_COL_NULLABLE, 
				VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED, DB_NULLID, DBPROPSET_COLUMN);
	if (SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN)) 
	{		
		VariantInit(&defval);
		GetDefaultValue(&rgColumnDesc[0], &defval);
		SetDefaultProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			&defval, DBPROPOPTIONS_OPTIONAL);	// might be not supported
		VariantClear(&defval);
	}

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[0], &pColumnID), S_OK);
	// must update info about table in m_pTable
	m_pTable->GetTableColumnInfo(&m_pTable->GetTableID());

	for (i=0, fFound=FALSE; !fFound && i<m_pTable->CountColumnsOnTable(); i++)
	{
		col = m_pTable->GetColInfoForUpdate(i+1);
		if (CompareDBID(rgColumnDesc[0].dbcid, *col.GetColID(), m_pIDBInitialize))
		{
			nCol[0]	= col.GetColNum();
			fFound	= TRUE;
		}
	}
	TESTC(fFound);
	TEST2C_(hr = m_pIOpenRowset->OpenRowset(NULL, &m_pTable->GetTableID(), NULL, 
		IID_IRowsetChange, 0, NULL, (IUnknown**)&pIRowsetChange), S_OK, E_NOINTERFACE);
	TESTC_PROVIDER(S_OK == hr);

	// try to insert a null value in a not null column
	hr = Insert(0, pIRowsetChange, NUMELEM(nCol), nCol);
	
	if (DB_E_NOTSUPPORTED != hr)
		CHECK(hr, DB_E_ERRORSOCCURRED);

CLEANUP:
	m_pTable->SetColumnDesc(NULL);
CLEANUP1:
	SAFE_RELEASE(pIRowsetChange);
	CHECK(m_pTable->DropTable(), S_OK);
	if (rgColumnDesc)
		rgColumnDesc[0].dbcid.uName.pwszName = NULL;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(45)
//*-----------------------------------------------------------------------
// @mfunc Add a column on a view
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_45()
{ 
	TBEGIN
	DBID			TableID;
	DBID			*pColumnID = NULL;
	LPWSTR			pwszViewName = L"VirTab";
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	IDBCreateCommand	*pIDBCreateCommand	= NULL;

	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 1), S_OK);

	m_pTable->SetViewName(pwszViewName);
	if ( !CHECK(m_pTable->ExecuteCommand(
						CREATE_VIEW,
						IID_NULL,
						pwszViewName,				// the table name will be the implicit one
						NULL,						// don't return the command statement
						0, 
						NULL,
						EXECUTE_IFNOERROR			// execute the command
						), S_OK) 
		)
	{
		odtLog << "\tA view on the table could not be created\n";
		goto CLEANUP;
	}

	// try to add a column to this maximum length name table
	PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = wcsDuplicate(L"new_column");
	// prepare the DBID to reflect the view
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszViewName;
	TEST3C_(AddColumnAndCheck(&TableID, &rgColumnDesc[cColumnDesc-1], &pColumnID), 
		E_FAIL, DB_E_NOTABLE, DB_SEC_E_PERMISSIONDENIED)

CLEANUP:
	SAFE_RELEASE(pIDBCreateCommand);
	ReleaseDBID(pColumnID);
	// drop the view!
	if (m_pTable->GetTableName())
		m_pTable->ExecuteCommand(
						DROP_VIEW,
						IID_NULL,
						pwszViewName,				// the table name will be the implicit one
						NULL,						// don't return the command statement
						0, 
						NULL,
						EXECUTE_IFNOERROR			// execute the command
						);
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(46)
//*-----------------------------------------------------------------------
// @mfunc eKind != DBKIND_NAME for a column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_46()
// try to use all the DBKIND values
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);

	// try to add a column with different DBKIND values
	odtLog << "Add DBKIND_GUID_NAME column\n";
	if (rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName)
		PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName);
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_GUID_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName	= L"DBKIND_GUID_NAME";
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

	odtLog << "Add DBKIND_GUID_PROPID column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_GUID_PROPID;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.ulPropid	= 0;
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;
	
	odtLog << "Add DBKIND_NAME column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName	= L"DBKIND_NAME";
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), S_OK);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

	odtLog << "Add DBKIND_PGUID_NAME column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_PGUID_NAME;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName	= L"DBKIND_PGUID_NAME";
	rgColumnDesc[cColumnDesc-1].dbcid.uGuid.pguid		= &guidModule;
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

	odtLog << "Add DBKIND_PGUID_PROPID column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_PGUID_PROPID;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.ulPropid	= 0;
	rgColumnDesc[cColumnDesc-1].dbcid.uGuid.pguid		= &guidModule;
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

	odtLog << "Add DBKIND_PROPID column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_PROPID;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.ulPropid	= 0;
	
	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

	odtLog << "Add DBKIND_GUID column\n";
	rgColumnDesc[cColumnDesc-1].dbcid.eKind				= DBKIND_GUID;
	rgColumnDesc[cColumnDesc-1].dbcid.uName.ulPropid	= 0;

	CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), 
		&rgColumnDesc[cColumnDesc-1], &pColumnID), DB_E_BADCOLUMNID);

	ReleaseDBID(pColumnID);
	pColumnID = NULL;

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	rgColumnDesc[cColumnDesc-1].dbcid.uName.pwszName = NULL;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




//---------------------------------------------------------------------
//
// @cmember Thread funtion for TCAddColumn
//---------------------------------------------------------------------
unsigned TCAddColumn::MyThreadProc(ULONG i)
{
	DBCOLUMNDESC	*pColumnDesc = NULL;
	HRESULT			hr;

	if (i>= nThreads)
		return 0;
	DuplicateColumnDesc(m_rgColumnDesc, 1, &pColumnDesc);
	ReleaseDBID(&pColumnDesc->dbcid, FALSE);
	pColumnDesc->dbcid.eKind			= DBKIND_NAME;
	pColumnDesc->dbcid.uName.pwszName	= m_rgName[i];
	Sleep(0); // switch the context

	
  	hr = m_pITableDefinition->AddColumn(&m_pTable->GetTableID(), pColumnDesc, NULL);
	Sleep(0); // switch the context

	pColumnDesc->dbcid.uName.pwszName = NULL;
	ReleaseColumnDesc(pColumnDesc, 1);
	m_rgResult[i] = hr;
	return 1;
} //TCAddColumn::MyThreadProc




// {{ TCW_VAR_PROTOTYPE(47)
//*-----------------------------------------------------------------------
// @mfunc threads with different column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_47()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i;
	LPWSTR			pwszBaseName = NULL;
	WCHAR			number[3];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);

	// create n names and init the results
	for (i=0; i<nThreads; i++)
		m_rgName[i] = NULL;

	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	pwszBaseName = m_pTable->GetTableName();
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_rgName[i] = (WCHAR*)PROVIDER_ALLOC((3+wcslen(pwszBaseName))*sizeof(WCHAR));		
		ASSERT(m_rgName[i]);
		wcscpy(m_rgName[i], pwszBaseName);
		swprintf(number, L"%02d", i);
		wcscat(m_rgName[i], number);
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// creates a table, build rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
		TESTC_(m_rgResult[i], S_OK);
	fTestResult = TEST_PASS;

CLEANUP:
	DropTableAndCheck(&m_pTable->GetTableID());
	for (i=0; i<nThreads; i++)
		PROVIDER_FREE(m_rgName[i]);
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(48)
//*-----------------------------------------------------------------------
// @mfunc threads on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_48()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i, nS_OK, nDB_E_DUPLICATECOLUMNID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_rgName[i] = L"someOtherColumn";		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// creates a table, build rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=nS_OK=nDB_E_DUPLICATECOLUMNID=0; i<nThreads; i++)
	{
		if (m_rgResult[i] == S_OK)
			nS_OK++;
		else if (CHECK(m_rgResult[i], DB_E_DUPLICATECOLUMNID))
			nDB_E_DUPLICATECOLUMNID++;
		else
			goto CLEANUP;
	}
	TESTC(nS_OK== 1 && nDB_E_DUPLICATECOLUMNID == nThreads-1);
	fTestResult = TEST_PASS;

CLEANUP:
	DropTableAndCheck(&m_pTable->GetTableID());
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(49)
//*-----------------------------------------------------------------------
// @mfunc add columns of alll known wType => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_49()
{ 
	TBEGIN
	ULONG			i;
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*pColumnDesc	= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);

	DuplicateColumnDesc(&rgColumnDesc[0], 1, &pColumnDesc);
	PROVIDER_FREE(pColumnDesc[0].dbcid.uName.pwszName);
	pColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"ColoanaInitiala");
	m_pTable->SetColumnDesc(pColumnDesc, 1);

	for (i=0; i<cColumnDesc; i++)
	{
		// create a table with a single column, equal to the first one in the row
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK)) 
			continue;

		// try to add a column from rgColumnDesc[i] with a NULL name and a valid wType
		PROVIDER_FREE(rgColumnDesc[i].pwszTypeName);
		odtLog <<	"column name: " << rgColumnDesc[i].dbcid.uName.pwszName << "\n";
		
		CHECK(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[i], &pColumnID), S_OK);

		CHECK(m_pTable->DropTable(), S_OK);
		ReleaseDBID(pColumnID);
		pColumnID = NULL;
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(50)
//*-----------------------------------------------------------------------
// @mfunc Fully Qualified table name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_50()
{ 
	TBEGIN
	DBID			TableID;
	WCHAR			*pwszQualTableName	= NULL;
	WCHAR			*pwszCatalogName	= NULL;
	WCHAR			*pwszSchemaName		= NULL;
	WCHAR			*pwszTableName		= NULL;
	DBCOLUMNDESC	*rgColumnDesc		= NULL;
	ULONG			cColumnDesc			= 0;

	// create a regular table
	TESTC(NULL != m_pTable);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	pwszTableName = m_pTable->GetTableName();
	TableID.eKind = DBKIND_NAME;

	TESTC(GetCatalogSchemaNames(pwszTableName, &pwszCatalogName, &pwszSchemaName));

	//Construct a fully Qualified TableName...
	TESTC_(m_pTable->GetQualifiedName(pwszCatalogName, pwszSchemaName, pwszTableName, &pwszQualTableName),S_OK);
	TableID.uName.pwszName = pwszQualTableName;

	m_pTable->BuildColumnDescs(&rgColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc=m_pTable->CountColumnsOnTable());
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"AltaColoana");

	TESTC_(AddColumnAndCheck(&TableID, &rgColumnDesc[0], NULL), S_OK);
    
CLEANUP:
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
	m_pTable->DropTable();
    TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(51)
//*-----------------------------------------------------------------------
// @mfunc Quoted Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_51()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = NULL;
	TESTC(NULL != m_pTable);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	m_pTable->GetQuotedName(m_pTable->GetTableName(), &TableID.uName.pwszName);

	m_pTable->BuildColumnDescs(&rgColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc=m_pTable->CountColumnsOnTable());
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"AltaColoana");

	TESTC_(AddColumnAndCheck(&TableID, &rgColumnDesc[0], NULL), S_OK);

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	m_pTable->DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(52)
//*-----------------------------------------------------------------------
// @mfunc pColumnDesc == NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_52()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);
	m_pTable->SetBuildColumnDesc(FALSE);

	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), NULL, &pColumnID), E_INVALIDARG);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(53)
//*-----------------------------------------------------------------------
// @mfunc cPropertySets != 0 and rgPropertySets == NULL in pColumnDesc => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_53()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	rgColumnDesc[cColumnDesc-1].cPropertySets	= (BYTE)(rand()%255+1);
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= NULL;
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), E_INVALIDARG);

CLEANUP:
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	// restore the proper number of properties for the column
	rgColumnDesc[cColumnDesc-1].cPropertySets = 0;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(54)
//*-----------------------------------------------------------------------
// @mfunc cPropertySets == 0 and rgPropertySets != NULL in pColumnDesc => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_54()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPSET		rgPropertySets[1];

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	rgColumnDesc[cColumnDesc-1].cPropertySets	= 0;
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= rgPropertySets;
	memset(rgPropertySets, 0, sizeof(DBPROPSET));

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), S_OK);

CLEANUP:
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= NULL;
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	// restore the property set array
	rgColumnDesc[cColumnDesc-1].rgPropertySets = NULL;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(55)
//*-----------------------------------------------------------------------
// @mfunc cProperties == 0 and rgProperties != NULL in pColumnDesc's propset => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_55()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPSET		rgPropertySets[1];
	DBPROP			rgProperties[1];

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	rgColumnDesc[cColumnDesc-1].cPropertySets	= NUMELEM(rgPropertySets);
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= rgPropertySets;
	memset(rgPropertySets, 0, sizeof(DBPROPSET));
	rgPropertySets[0].guidPropertySet	= DBPROPSET_COLUMN;
	rgPropertySets[0].cProperties		= 0;
	rgPropertySets[0].rgProperties		= rgProperties;

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), S_OK);

CLEANUP:
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= NULL;
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	// restore the prop array
	rgPropertySets[0].rgProperties = NULL;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(56)
//*-----------------------------------------------------------------------
// @mfunc cProperties != 0 and rgProperties == NULL in pColumnDesc's propset => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_56()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPSET		rgPropertySets[1];

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 0), S_OK);

	// built the property set array
	rgColumnDesc[cColumnDesc-1].cPropertySets	= NUMELEM(rgPropertySets);
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= rgPropertySets;
	memset(rgPropertySets, 0, sizeof(DBPROPSET));
	rgPropertySets[0].guidPropertySet	= DBPROPSET_COLUMN;
	rgPropertySets[0].cProperties		= (BYTE)(rand()%255+1);
	rgPropertySets[0].rgProperties		= NULL;

	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID), E_INVALIDARG);

CLEANUP:
	rgColumnDesc[cColumnDesc-1].rgPropertySets	= NULL;
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	// restore the prop counter
	rgPropertySets[0].cProperties = 0;
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(57)
//*-----------------------------------------------------------------------
// @mfunc pclsid not null
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_57()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	HRESULT			hr;
	POSITION		pos;
	CCol			col;
	BOOL			fOleSupported	= FALSE;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_pTable->CreateTable(10, 0), S_OK);

	// set pclsid to IID_NULL
	rgColumnDesc[cColumnDesc-1].pclsid = (GUID*)&IID_NULL;

	// check that pclsid is ignored
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), 
				&rgColumnDesc[cColumnDesc-1], &pColumnID),S_OK);

	TESTC_(m_pTable->DropTable(), S_OK);
	TESTC_(m_pTable->CreateTable(10, 0), S_OK);

	// check DBTYPE_IUNKNOWN is supported
	for (pos = m_ColList.GetHeadPosition(); pos; )
	{
		col = m_ColList.GetNext(pos);
		if (DBTYPE_IUNKNOWN == col.GetProviderType())
		{
			fOleSupported = TRUE;
			break;
		}
	}

	// check whether a DBTYPE_IUNKNOWN column can be created in the table
	rgColumnDesc[cColumnDesc-1].wType	= DBTYPE_IUNKNOWN;
	PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].pwszTypeName);
	if (fOleSupported)
		rgColumnDesc[cColumnDesc-1].pwszTypeName = wcsDuplicate(col.GetProviderTypeName());
	hr = fOleSupported ? S_OK : DB_E_BADTYPE;
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), hr);
	

CLEANUP:
	ReleaseDBID(pColumnID);
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(58)
//*-----------------------------------------------------------------------
// @mfunc NULL pwszTypeName => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddColumn::Variation_58()
{ 
	TBEGIN
	DBID			*pColumnID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc-1);

	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	// try to add a column with a bad wType
	PROVIDER_FREE(rgColumnDesc[cColumnDesc-1].pwszTypeName);
	rgColumnDesc[cColumnDesc-1].pwszTypeName = NULL;
	
	TESTC_(AddColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[cColumnDesc-1], 
		&pColumnID), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	m_pTable->SetNoOfColumnDesc(cColumnDesc);
	ReleaseDBID(pColumnID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddColumn::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
}	// }}
// }}
// }}




// {{ TCW_TC_PROTOTYPE(TCDropColumn)
//*-----------------------------------------------------------------------
//| Test Case:		TCDropColumn - Test case for dropping columns
//|	Created:			09/11/97
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropColumn::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}




/*
//----------------------------------------------------------------------
//
// @cmember Check whether the 2 list of columns are identical, 
// excepting column n
//----------------------------------------------------------------------
BOOL TCDropColumn::IsColumnDifferention(
	DBCOLUMNDESC	*newlist, 
	ULONG			newlen, 
	DBCOLUMNDESC	*oldlist, 
	ULONG			oldlen, 
	ULONG			n
)
{
	ULONG	nNewCol;
	ULONG	nOldCol;

	if (!newlist || !oldlist)
		return oldlist == newlist;
	if (newlen != oldlen - 1)
		return FALSE;
	
	// it is assumed that the columns should be identical up to column n
	// the new column n will be the oldcolumn n+1, the new column n+1		
	// should be identical to the old column n+2, etc
	for (nNewCol = nOldCol = 0; nNewCol < newlen; nNewCol++)
	{
		if (nNewCol == n)
			nOldCol++;
		if (!CompareDBID(newlist[nNewCol].dbcid, oldlist[nOldCol++].dbcid, m_pIDBInitialize))
			return FALSE;
	}
	return TRUE;
} //TCDropColumn::IsColumnDifferention
//----------------------------------------------------------------------
//
// @cmember Check whether the 2 list of columns are identical
//----------------------------------------------------------------------
BOOL TCDropColumn::ListEqual(DBCOLUMNDESC *newlist, ULONG newlen, DBCOLUMNDESC *oldlist, ULONG oldlen)
{
	ULONG	i;

	if (!newlist || !oldlist)
		return newlist == oldlist;
	if (newlen != oldlen)
		return FALSE;
	for (i=0; i<newlen; i++)
		if (!CompareDBID(newlist[i].dbcid, oldlist[i].dbcid, m_pIDBInitialize))
			return FALSE;
	return TRUE;
} //TCDropColumn::ListEqual
*/




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_1()
{
	TBEGIN
	DBID	ColumnID;

	ColumnID.eKind		= DBKIND_NAME;
	ColumnID.uName.pwszName	= L"Gateway2001";

	TESTC_(DropColumnAndCheck(NULL, &ColumnID), E_INVALIDARG);
CLEANUP:
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc pColumnID in NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_2()
{
	TBEGIN

	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), NULL), E_INVALIDARG);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc *pTableID is DB_NULLID => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_3()
{
	TBEGIN
	DBID	ColumnID;
	DBID	TableID			= DB_NULLID;

	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= L"Gateway2001";
	
	TESTC_(DropColumnAndCheck(&TableID, &ColumnID), DB_E_NOTABLE);

CLEANUP:
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc *pColumnID is DB_NULLID => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_4()
{
	TBEGIN
	DBID	ColumnID;

	memset(&ColumnID, 0, sizeof(DBID));
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Maximum length table name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_5()
{
	TBEGIN
	DBID			ColumnID;
	DBID			TableID;
	ULONG			n, cColumns;
	DBCOLUMNINFO	*rgColInfo = NULL;
	LPWSTR			pwszTableName;
	OLECHAR			*pStringBuffer = NULL;
	ULONG			ulOrdinal = 0;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= pwszTableName = BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	PRVTRACE(L"** TCDropColumn::Variation_5(): create a table with max table name: %s, %d\n", 
			pwszTableName, n=wcslen(pwszTableName));

	TESTC_(m_hr = m_pTable->CreateTable(0, 0, pwszTableName), S_OK);

	GetColumnInfo(&TableID, &cColumns, &rgColInfo, &pStringBuffer);
	TESTC(0 < cColumns);
	// check if the first column is bookmark or not
	if (0 == rgColInfo[0].iOrdinal)
		ulOrdinal = 1;

	TESTC(ulOrdinal < cColumns);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= rgColInfo[ulOrdinal].pwszName;

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), S_OK);

CLEANUP:
	CHECK(m_hr = m_pTable->DropTable(), S_OK);
	PROVIDER_FREE(pStringBuffer);
	PROVIDER_FREE(rgColInfo);
	PROVIDER_FREE(pwszTableName);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc 1-char-too long table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_6()
{
	BOOL			fTestRes = TEST_FAIL; 
	DBID			ColumnID;
	DBID			TableID;
	DBCOLUMNDESC	*oldCD=NULL;
	ULONG			oldlen;
	LPWSTR			pwszTableName;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName+1, m_pTable->GetTableName());
	pwszTableName			= BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	
	TESTC_(m_hr = m_pTable->CreateTable(0, 0, pwszTableName), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= wcsDuplicate(oldCD[0].dbcid.uName.pwszName);
	
	TESTC_(DropColumnAndCheck(&TableID, &ColumnID), DB_E_NOTABLE);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	CHECK(m_pTable->DropTable(), S_OK);
	ReleaseDBID(&ColumnID, FALSE);
	ReleaseDBID(&TableID, FALSE);
	PROVIDER_FREE(pwszTableName);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc *pTableID contains a NULL table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_7()
{
	TBEGIN
	DBID	ColumnID;
	DBID	TableID;

	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= L"Gateway2001";
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= NULL;

	TESTC_(DropColumnAndCheck(&TableID, &ColumnID), DB_E_NOTABLE);

CLEANUP:
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc *pTableID contains an invalid name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_8()
{
	TBEGIN
	DBID			ColumnID;
	DBID			TableID;
	ULONG			n;

	m_pTable->MakeTableName(NULL);
	n = min(wcslen(m_pwszInvalidTableChars)+wcslen(m_pTable->GetTableName()), m_cMaxTableName);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildInvalidName(n, m_pTable->GetTableName(), m_pwszInvalidTableChars);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= L"foca";
	
	TESTC_(DropColumnAndCheck(&TableID, &ColumnID), DB_E_NOTABLE);

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Maximum length for a column name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_9()
{
	TBEGIN
	DBID			ColumnID;
	LPWSTR			pwszColumnName;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->MakeTableName(NULL);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= pwszColumnName = BuildValidName(m_cMaxColName, L"aaabcdef");
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	rgColumnDesc[0].dbcid.eKind			= DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName	= pwszColumnName;

	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc 1-char-too-long column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_10()
{
	TBEGIN
	DBID			ColumnID;
	LPWSTR			pwszColumnName = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	
	m_pTable->MakeTableName(NULL);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName = pwszColumnName = BuildValidName(m_cMaxColName, L"aaabcdef"); 
	ColumnID.uName.pwszName	= BuildValidName(m_cMaxColName+1, m_pTable->GetTableName());
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	rgColumnDesc[0].dbcid.eKind				= DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName	= pwszColumnName;
	
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	PROVIDER_FREE(ColumnID.uName.pwszName);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc *pColumnID contains a NULL column name pointer => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_11()
{
	TBEGIN
	DBID			ColumnID;

	// create a table, which contains a maximum size table length
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	ColumnID.eKind				= DBKIND_NAME;
	ColumnID.uName.pwszName		= NULL;
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc *pColumnID contains an invalid column name => E_INVALID ARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_12()
{
	TBEGIN
	DBID			ColumnID;
	LPWSTR			pwszColumnName;
	ULONG			n;

	m_pTable->SetBuildColumnDesc(TRUE);
	m_pTable->MakeTableName(NULL);
	n = min(wcslen(m_pTable->GetTableName())+wcslen(m_pwszInvalidColChars), m_cMaxColName);
	pwszColumnName = BuildInvalidName(n, m_pTable->GetTableName(), m_pwszInvalidColChars);
	// create a table, which contains a maximum size table length
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= pwszColumnName;
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	PROVIDER_FREE(pwszColumnName);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Drop all the columns of a table => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_13()
{
	TBEGIN
	DBID			ColumnID;
	ULONG			n, cColumns;
	DBCOLUMNINFO	*rgColInfo = NULL;
	OLECHAR			*pStringBuffer = NULL;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnInfo(&m_pTable->GetTableID(), &cColumns, &rgColInfo, &pStringBuffer);
	// check for bookmarks
	n = (0 != rgColInfo[0].iOrdinal) ? 0: 1;
	for (; n < cColumns-1; n++)
	{
		// drop column i
		DuplicateDBID(rgColInfo[n].columnid, &ColumnID);
		if ( !CHECK(m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), 
				S_OK))
		{
			odtLog << "cannot drop column " << rgColInfo[n].pwszName << "!\n";
		}
		else
			odtLog << "Column "<< rgColInfo[n].pwszName << " was successfully dropped\n";
		ReleaseDBID(&ColumnID, FALSE);
	}
	// try to drop the last column
	DuplicateDBID(rgColInfo[cColumns-1].columnid, &ColumnID);
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (FAILED(m_hr))
	{
		odtLog << "cannot drop column " << rgColInfo[cColumns-1].pwszName << " (the last one)!\n";
	}
	else
		odtLog << "Column "<< rgColInfo[cColumns-1].pwszName << " (the last column) was successfully dropped\n";
	ReleaseDBID(&ColumnID, FALSE);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	PROVIDER_FREE(rgColInfo);
	PROVIDER_FREE(pStringBuffer);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Try to delete an inexistent column => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_14()
{
	TBEGIN
	DBID			ColumnID;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	ColumnID.eKind				= DBKIND_NAME;
	ColumnID.uName.pwszName		= L"inexistentColumn";
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Table in use => DB_E_TABLEINUSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_15()
{
	TBEGIN
	IRowset			*pIRowset=NULL;
	CCol			col;

	m_pTable->SetRowset((IUnknown**)&pIRowset);
	m_pTable->SetRIID((struct _GUID*)&IID_IRowset);

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	col = m_pTable->GetColInfoForUpdate(1);
	TEST2C_(m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), 
		col.GetColID()), S_OK, DB_E_TABLEINUSE);

	odtLog << ((S_OK == m_hr)? "column was dropped\n": "column has been preserved\n");

CLEANUP:
	m_pTable->SetRowset(NULL);
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Valid name of an inexistent table => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_16()
{
	TBEGIN
	DBID	ColumnID, TableID;

	m_pTable->MakeTableName(NULL);
	DuplicateDBID(m_rgColumnDesc[0].dbcid, &ColumnID);
	
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();
	CHECK(m_hr = DropColumnAndCheck(&TableID, &ColumnID), DB_E_NOTABLE);

	ReleaseDBID(&ColumnID, FALSE);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Abort with retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_17()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD = NULL;
	ULONG				newlen = 0, oldlen = 0;
	CCol				col;
	ULONG				nCol;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);
		
	// create a table 
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	// try to drop a column from table
	nCol	= (ULONG)rand()%m_pTable->CountColumnsOnTable()+1;
	col		= m_pTable->GetColInfoForUpdate(nCol);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	// abort retaining
	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		CHECK(pITransactionLocal->Abort(NULL, TRUE, FALSE), S_OK);
		//check the retaining propriety
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (	DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		||	DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal)
	{	// check column was dropped
		COMPARE(CheckColInfo(newCD, newlen, oldCD, oldlen, &oldCD[nCol-1]), TRUE);
	}
	else
	{	// check column was not dropped
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	SAFE_RELEASE(pITransactionLocal);
	m_hr = m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Abort without retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_18()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD = NULL;
	ULONG				newlen = 0, oldlen = 0;
	CCol				col;
	ULONG				nCol;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));
		
	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	// create a table 
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	// try to drop a column from table
	nCol	= (ULONG)rand()%m_pTable->CountColumnsOnTable()+1;
	col		= m_pTable->GetColInfoForUpdate(nCol);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort non retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		//check the non-retaining propriety
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			SUCCEEDED(m_hr)? XACT_E_NOTRANSACTION: S_OK);
	}

	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (	DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		||	DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal)
	{	// check column was dropped
		COMPARE(CheckColInfo(newCD, newlen, oldCD, oldlen, &oldCD[nCol-1]), TRUE);
	}
	else
	{	// check column was not dropped
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	SAFE_RELEASE(pITransactionLocal);
	m_hr = m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Commit with retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_19()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD = NULL;
	ULONG				newlen = 0, oldlen = 0;
	CCol				col;
	ULONG				nCol;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));
		
	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	// create a table 
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);

	// start the transaction
	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	// try to drop a column from table
	nCol	= (ULONG)rand()%m_pTable->CountColumnsOnTable()+1;
	col		= m_pTable->GetColInfoForUpdate(nCol);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	// abort retaining
	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit retaining
		CHECK(pITransactionLocal->Commit(TRUE, 0, 0), S_OK);
		// check retain property
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	// check column was dropped (oldCD must contains new CD and rgColumnDesc[0])
	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal)
	{	// check column was not dropped
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	else
	{	// check column was dropped
		COMPARE(CheckColInfo(newCD, newlen, oldCD, oldlen, &oldCD[nCol-1]), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	SAFE_RELEASE(pITransactionLocal);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Commit without retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropColumn::Variation_20()
{
	TBEGIN
	ITransactionLocal	*pITransactionLocal = NULL;
	DBCOLUMNDESC		*newCD = NULL, *oldCD = NULL;
	ULONG				newlen = 0, oldlen = 0;
	CCol				col;
	ULONG				nCol;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal, 
			SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));
		
	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	// create a table 
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);
	
	// start the transaction
	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	// try to drop a column from table
	nCol = (ULONG)rand()%m_pTable->CountColumnsOnTable()+1;
	col = m_pTable->GetColInfoForUpdate(nCol);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			CHECK(m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), 
				col.GetColID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit without retaining
		CHECK(m_hr = pITransactionLocal->Commit(FALSE, 0, 0), S_OK);
		// check retaining / nont retaining prop
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			FAILED(m_hr)? S_OK: XACT_E_NOTRANSACTION);
	}

	// check column was dropped (oldCD must contains new CD and rgColumnDesc[0])
	GetColumnDescOnTable(&m_pTable->GetTableID(), &newCD, &newlen);
	if (DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal)
	{	// check column was not dropped
		COMPARE(AreColEqual(oldCD, oldlen, newCD, newlen), TRUE);
	}
	else
	{	// check column was dropped
		COMPARE(CheckColInfo(newCD, newlen, oldCD, oldlen, &oldCD[nCol-1]), TRUE);
	}
	ReleaseColumnDesc(newCD, newlen);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	m_hr = m_pTable->DropTable();
	SAFE_RELEASE(pITransactionLocal);
	TRETURN
}
// }}


//----------------------------------------------------------------------
//
// @cmember Thread function for TCDropColumn
//----------------------------------------------------------------------
unsigned TCDropColumn::MyThreadProc(ULONG i)
{
	DBID			ColumnID;
	HRESULT			hr;

	if (i>= nThreads)
		return 0;
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= m_rgName[i];
	Sleep(0); // switch the context

	hr = m_pITableDefinition->DropColumn(&m_pTable->GetTableID(), &ColumnID);
	Sleep(0); // switch the context

	m_rgResult[i] = hr;
	return 1;
} //TCDropColumn::MyThreadProc


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc threads on different columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_21()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i;
	ULONG			newnThreads;
	CCol			col;

	// create nThreads names and init the results
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	if (m_cColumnDesc < 2)
	{
		odtLog << "There are too few columns in the table\n";
		goto CLEANUP;
	}
	newnThreads = (nThreads <= m_cColumnDesc) ? nThreads: m_cColumnDesc;
	for (i=0; i< newnThreads; i++)
	{
		m_rgResult[i]	= -1;
		col				= m_pTable->GetColInfoForUpdate(i+1);
		m_rgName[i]		= wcsDuplicate(col.GetColName());
		rgThreadParam[i].pObject = this;
		rgThreadParam[i].i		= i;
	}
	
	// creates a table, build m_rgColumnDesc by it
	for (i=0; i<newnThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(newnThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< newnThreads; i++)
	{
		CloseHandle(hrgThread[i]);
		SAFE_FREE(m_rgName[i]);
	}

	for (i=0; i<newnThreads; i++)
		TESTC_(m_rgResult[i], S_OK);
	fTestResult = TEST_PASS;

CLEANUP:
	DropTableAndCheck(&m_pTable->GetTableID());
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc threads on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_22()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i, nS_OK, nDB_E_BADCOLUMNID;
	CCol			col;
	ULONG			nCol;

	// create n names and init the results
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	if (m_cColumnDesc < 1)
	{
		odtLog << "There are too few columns in the table\n";
		goto CLEANUP;
	}
	nCol = (ULONG)rand()%m_pTable->CountColumnsOnTable()+1;
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i]	= -1;
		col				= m_pTable->GetColInfoForUpdate(nCol);
		m_rgName[i]		= wcsDuplicate(col.GetColName());
		rgThreadParam[i].pObject = this;
		rgThreadParam[i].i		= i;
	}
	
	// creates a table, build m_rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
	{
		CloseHandle(hrgThread[i]);
		SAFE_FREE(m_rgName[i]);
	}

	for (i=nS_OK=nDB_E_BADCOLUMNID=0; i<nThreads; i++)
	{
		if (m_rgResult[i] == S_OK)
			nS_OK++;
		else if (CHECK(m_rgResult[i], DB_E_BADCOLUMNID))
			nDB_E_BADCOLUMNID++;
		else 
			goto CLEANUP;
	}
	TESTC(nS_OK == 1 && nDB_E_BADCOLUMNID == nThreads-1);
	fTestResult = TEST_PASS;

CLEANUP:
	DropTableAndCheck(&m_pTable->GetTableID());
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Drop a column twice => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_23()
{ 
	TBEGIN
	CCol	col;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	
	col = m_pTable->GetColInfoForUpdate((ULONG)rand()%m_pTable->CountColumnsOnTable()+1);
	
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), DB_E_BADCOLUMNID);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc 2 TableDefinition interfaces on the same table, try to drop 2 different columns => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_24()
{ 
	TBEGIN
	ITableDefinition	*pITableDefinition = NULL;
	CCol				col;

	TESTC(VerifyInterface(m_pIOpenRowset, IID_ITableDefinition, SESSION_INTERFACE, (IUnknown**)&pITableDefinition));
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	col = m_pTable->GetColInfoForUpdate(1);
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()),	S_OK);

	col = m_pTable->GetColInfoForUpdate(2);
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID(), pITableDefinition), S_OK);

CLEANUP:
	SAFE_RELEASE(pITableDefinition);
	m_hr = m_pTable->DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Drop as many column as possible from a table created with a SQL statement
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_25()
{ 
	TBEGIN
	BOOL				fOldTableCreation = GetModInfo()->UseITableDefinition(FALSE);
	ULONG				i, cColumnsInfo;
	DBCOLUMNINFO		*prgColumnsInfo = NULL;
	OLECHAR				*pStringBuffer = NULL;
	IDBCreateCommand	*pIDBCreateCommand = NULL;
	
	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	GetColumnInfo(&m_pTable->GetTableID(), &cColumnsInfo, &prgColumnsInfo, &pStringBuffer);
	i = (prgColumnsInfo[0].iOrdinal == 0)? 1: 0;
	
	for (; i<cColumnsInfo-1; i++)
	{
		if ( !CHECK(m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &prgColumnsInfo[i].columnid), S_OK) )
			odtLog << "could not drop the column \n";
	}
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &prgColumnsInfo[cColumnsInfo-1].columnid);
	odtLog << ((SUCCEEDED(m_hr)) ? "\tlast column was dropped\n" : "\t could not drop the last column\n");

	CHECK(m_pTable->DropTable(), S_OK);

CLEANUP:
	SAFE_RELEASE(pIDBCreateCommand);
	GetModInfo()->UseITableDefinition(fOldTableCreation);
	PROVIDER_FREE(prgColumnsInfo);
	PROVIDER_FREE(pStringBuffer);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc Drop a column from a 2 column table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_26()
{ 
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 2;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[0].dbcid), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc drop a column from a 1 column table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_27()
{ 
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 1;

	DuplicateColumnDesc(m_rgColumnDesc, cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &rgColumnDesc[0].dbcid);
		
	if (S_OK == m_hr)			
		odtLog << "the only column of the table was dropped\n";
	else
		odtLog << "could not drop the only column of the table\n";

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc drop a column from a non empty table => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_28()
{ 
	TBEGIN
	CCol	col;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);

	col = m_pTable->GetColInfoForUpdate((ULONG)rand()%m_pTable->CountColumnsOnTable()+1);
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc drop a column from an empty table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_29()
{ 
	TBEGIN
	CCol	col;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	col = m_pTable->GetColInfoForUpdate((ULONG)rand()%m_pTable->CountColumnsOnTable()+1);
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), col.GetColID()), S_OK);

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc drop a nullable, an autoincrementable and a default value column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_30()
{
	TBEGIN
	ULONG			nAutoInc;
	ULONG			nNull;
	ULONG			nDefault;
	ULONG			i;
	BOOL			fFound = FALSE;
	VARIANT			defVal;
	DBID			ColumnID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CCol			col;
	CTable					*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	TESTC_PROVIDER(SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN));
	TESTC_PROVIDER(SettableProperty(DBPROP_COL_AUTOINCREMENT, DBPROPSET_COLUMN));
	TESTC_PROVIDER(SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN));

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// get an autoincrementable column
	nAutoInc = cColumnDesc;
	for (i=0; i < cColumnDesc && !fFound; i++)
	{	
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		if (TRUE == (fFound = col.CanAutoInc()))
			nAutoInc = i;
	}

	TESTC(nAutoInc < cColumnDesc);

	// get the nullable and default columns
	nNull		= (nAutoInc+1) % cColumnDesc;
	nDefault	= (nAutoInc+2) % cColumnDesc;
	
	// set autoincrementable property on a column
	SetProperty(&rgColumnDesc[nAutoInc].rgPropertySets, &rgColumnDesc[nAutoInc].cPropertySets, 
		DBPROP_COL_AUTOINCREMENT, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	// set nullable property on a column (also add default for it)
	SetProperty(&rgColumnDesc[nNull].rgPropertySets, &rgColumnDesc[nNull].cPropertySets, 
		DBPROP_COL_NULLABLE, VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	VariantInit(&defVal);
	GetDefaultValue(&rgColumnDesc[nNull], &defVal, pTable);
	SetDefaultProperty(&rgColumnDesc[nNull].rgPropertySets, &rgColumnDesc[nNull].cPropertySets,
		&defVal, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&defVal);
	// set default column
	GetDefaultValue(&rgColumnDesc[nDefault], &defVal, pTable);
	SetDefaultProperty(&rgColumnDesc[nDefault].rgPropertySets, &rgColumnDesc[nDefault].cPropertySets,
		&defVal, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&defVal);

	// create the table with a autoinc column, a nullable one and a default one
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	DuplicateDBID(rgColumnDesc[nAutoInc].dbcid, &ColumnID);
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (S_OK != m_hr)
		odtLog << "could not drop the autoincrementable column\n";
	else
		odtLog << "the autoincrementable column was dropped \n";
	ReleaseDBID(&ColumnID, FALSE);

	DuplicateDBID(rgColumnDesc[nNull].dbcid, &ColumnID);
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (S_OK != m_hr)
		odtLog << "could not drop the nullable column\n";
	else
		odtLog << "the nullable column was dropped \n";
	ReleaseDBID(&ColumnID, FALSE);

	DuplicateDBID(m_rgColumnDesc[nDefault].dbcid, &ColumnID);
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (S_OK != m_hr)
		odtLog << "could not drop the default value column\n";
	else
		odtLog << "the default value column was dropped \n";
	ReleaseDBID(&ColumnID, FALSE);
	CHECK(m_pTable->DropTable(), S_OK);

CLEANUP:
	delete pTable;
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc try to drop a column from a view
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_31()
{ 
	TBEGIN
	LPWSTR			pwszViewName = L"HagiView";	// just a view name
	DBID			TableID;
	CCol			col;
	IDBCreateCommand	*pIDBCreateCommand	= NULL;

	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	// create the view
	m_pTable->SetViewName(pwszViewName);
	if ( S_OK != (m_hr = m_pTable->ExecuteCommand(
						CREATE_VIEW,
						IID_NULL,
						pwszViewName,						// the table name will be the implicit one
						NULL,						// don't return the command statement
						0, 
						NULL,
						EXECUTE_IFNOERROR			// execute the command
						)) 
		)
	{
		odtLog << "\t the view cannot be created\n";
		goto CLEANUP;
	}

	TESTC_(m_hr, S_OK);

	// try to drop a column from the view
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= L"HagiTable";
	col = m_pTable->GetColInfoForUpdate((ULONG)rand()%m_pTable->CountColumnsOnTable()+1);
	m_hr = DropColumnAndCheck(&TableID, col.GetColID());
	odtLog << ((S_OK == m_hr) ? "column was dropped from view\n" : "could not drop column from view\n");

CLEANUP:
	SAFE_RELEASE(pIDBCreateCommand);
	// drop the view!
	if (m_pTable->GetTableName())
		m_hr = m_pTable->ExecuteCommand(
						DROP_VIEW,
						IID_NULL,
						pwszViewName,						// the table name will be the implicit one
						NULL,						// don't return the command statement
						0, 
						NULL,
						EXECUTE_IFNOERROR			// execute the command
						);
	CHECK(m_hr = m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc drop a column added with an alter table command => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_32()
{ 
	TBEGIN
	DBID			ColumnID;
	LPWSTR			pwszColumnName = L"intHagi";	// the name of the integer column
	LPWSTR			pwszColNameAndType = NULL;
	IDBCreateCommand	*pIDBCreateCommand	= NULL;

	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	TESTC(NULL != m_rgColumnDesc[0].pwszTypeName);	// it should be a numeric type
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	
	// build the column name and type argument of the command
	SAFE_ALLOC(pwszColNameAndType, WCHAR, wcslen(pwszColumnName)+wcslen(m_rgColumnDesc[0].pwszTypeName)+2);
	wcscpy(pwszColNameAndType, pwszColumnName);
	wcscat(pwszColNameAndType, L" ");
	wcscat(pwszColNameAndType, m_rgColumnDesc[0].pwszTypeName);

	// create the ALTER SQL statement
	if ( S_OK != (m_hr = m_pTable->ExecuteCommand(
						ALTER_TABLE_ADD,
						IID_NULL,
						pwszColNameAndType,				// the table name will be the implicit one
						NULL,						// don't return the command statement
						0, 
						NULL,
						EXECUTE_IFNOERROR			// execute the command
						)) 
		)
	{
		odtLog << "Could not add a column in the table, using commands\n";
		goto CLEANUP;
	}

	// try to drop the new column from the table
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= pwszColumnName;
	TESTC_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), S_OK);
		
CLEANUP:
	SAFE_RELEASE(pIDBCreateCommand);
	SAFE_FREE(pwszColNameAndType);
	CHECK(m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc try to drop columns using different DBKIND pattern
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_33()
{ 
	TBEGIN
	DBID			ColumnID;
	CCol			col;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);

	// try to drop a column
	col = m_pTable->GetColInfoForUpdate((ULONG)rand()%m_pTable->CountColumnsOnTable()+1);
	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= wcsDuplicate(col.GetColName());
	if (m_cColumnDesc > 2)
	{
		CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), S_OK);
	}
	else
	{
		odtLog << "too few columns, skipped the drop column given by name case \n";
	}
	ReleaseDBID(&ColumnID, FALSE);

	ColumnID.eKind			= DBKIND_GUID;
	memset(&ColumnID.uGuid.guid, 0, sizeof(GUID));
	if (!CHECK(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), DB_E_BADCOLUMNID))
	{
		odtLog << "error at the guid case\n";
	}
	
	ColumnID.eKind			= DBKIND_GUID_NAME;
	memset(&ColumnID.uGuid.guid, 0, sizeof(GUID));
	ColumnID.uName.pwszName	= NULL;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID))
	{
		odtLog << "error at the guid name case\n";
	}
	
	ColumnID.eKind			= DBKIND_GUID_PROPID;
	memset(&ColumnID.uGuid.guid, 0, sizeof(GUID));
	ColumnID.uName.ulPropid	= 0L;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID) )
	{
		odtLog << "error at the guid propid case\n";
	}
	
	ColumnID.eKind			= DBKIND_PGUID_NAME;
	ColumnID.uGuid.pguid	= &guidModule;
	ColumnID.uName.pwszName	= NULL;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID))
	{
		odtLog << "error at the pguid name case\n";
	}
	
	ColumnID.eKind			= DBKIND_PGUID_PROPID;
	ColumnID.uGuid.pguid	= &guidModule;
	ColumnID.uName.ulPropid	= 0L;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID) )
	{
		odtLog << "error at the pguid propid case\n";
	}
	
	ColumnID.eKind			= DBKIND_PROPID;
	ColumnID.uName.ulPropid	= 0L;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID))
	{
		odtLog << "error at the propid case\n";
	}
	
	ColumnID.eKind			= 0xff;
	m_hr = DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID);
	if (!CHECK(m_hr, DB_E_BADCOLUMNID))
	{
		odtLog << "error at the inexistent value for eKind case\n";
	}

CLEANUP:
	CHECK(m_hr = m_pTable->DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(34)
//*-----------------------------------------------------------------------
// @mfunc Fully Qualified Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_34()
{ 
	TBEGIN
	DBID		TableID;
	WCHAR		*pwszQualTableName = NULL;
	WCHAR		*pwszCatalogName = NULL;
	WCHAR		*pwszSchemaName = NULL;
	WCHAR		*pwszTableName = NULL;
	CCol		col;

	// create a regular table
	TESTC(NULL != m_pTable);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	pwszTableName = m_pTable->GetTableName();
	TableID.eKind = DBKIND_NAME;

	TESTC(GetCatalogSchemaNames(pwszTableName, &pwszCatalogName, &pwszSchemaName));

	//Construct a fully Qualified TableName...
	TESTC_(m_pTable->GetQualifiedName(pwszCatalogName, pwszSchemaName, pwszTableName, &pwszQualTableName),S_OK);
	TableID.uName.pwszName = pwszQualTableName;
	col = m_pTable->GetColInfoForUpdate(1);

	TESTC_(DropColumnAndCheck(&TableID, col.GetColID()), S_OK);
    
CLEANUP:
	m_pTable->DropTable();
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
    TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(35)
//*-----------------------------------------------------------------------
// @mfunc Quoted Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropColumn::Variation_35()
{ 
	TBEGIN
	DBID	TableID;
	CCol	col;
	
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = NULL;
	TESTC(NULL != m_pTable);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);

	m_pTable->GetQuotedName(m_pTable->GetTableName(), &TableID.uName.pwszName);
	col = m_pTable->GetColInfoForUpdate(1);

	TESTC_(DropColumnAndCheck(&TableID, col.GetColID()), S_OK);

CLEANUP:
	m_pTable->DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropColumn::Terminate()
{
	// TO DO:  Add your own code here

	// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
}	// }}
// }}
// }}




// {{ TCW_TC_PROTOTYPE(TCDropTable)
//*-----------------------------------------------------------------------
//| Test Case:		TCDropTable - Test Case fo dropping a table
//|	Created:			09/11/97
//*-----------------------------------------------------------------------

//--------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropTable::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{
		// TO DO:  Add your own code here
		return TRUE;
	}
	return FALSE;
}


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_1()
{

	if ( !CHECK(m_hr = m_pITableDefinition->DropTable(NULL), E_INVALIDARG))
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc *pTableID is DB_NULLID => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_2()
{
	DBID	TableID;

	TableID = DB_NULLID;
	if ( !CHECK(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE))
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Maximum length for table name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_3()
{
	BOOL	fTestRes = TEST_FAIL;
	WCHAR*	pwszTableName = NULL; 
	DBID	TableID;

	// create a maximum size table name 
	m_pTable->MakeTableName(NULL);
	GetLiteralInfo();
	pwszTableName = BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	odtLog << "king size table name: " << pwszTableName << ", i.e." << (wcslen(pwszTableName)) << " characters\n";
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0, pwszTableName), S_OK);
	
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszTableName;
	TESTC_(m_hr = m_pITableDefinition->DropTable(&TableID), S_OK);
	fTestRes = TEST_PASS;

CLEANUP:
	m_pTable->DropTable();
	PROVIDER_FREE(pwszTableName);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc 1-char-too-long table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_4()
{
	BOOL	fTestRes = TEST_FAIL;
	WCHAR*	pwszTableName = NULL, *pwszTableName1 = NULL; 
	DBID	TableID;

	// create a maximum size table name 
	m_pTable->MakeTableName(NULL);
	GetLiteralInfo();
	pwszTableName	= BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	pwszTableName1	= BuildValidName(m_cMaxTableName+1, m_pTable->GetTableName());
	odtLog	<< "1 char too long table name: " << pwszTableName1 << ", i.e." 
			<< wcslen(pwszTableName1) << " characters\n";
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0, pwszTableName), S_OK);

	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszTableName1;
	TESTC_(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE);
	fTestRes = TEST_PASS;

CLEANUP:
	CHECK(m_pTable->DropTable(), S_OK);
	PROVIDER_FREE(pwszTableName);
	PROVIDER_FREE(pwszTableName1);
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc The pointer to the table name is NULL => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_5()
{
	DBID	TableID;

	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = NULL;
	if ( !CHECK(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE))
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Table name is empty => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_6()
{
	DBID	TableID;

	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = L"";
	if ( !CHECK(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE))
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Invalid table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_7()
{
	BOOL	fTestRes = TEST_FAIL;
	WCHAR*	pwszTableName = NULL; 
	ULONG	m;
	DBID	TableID;

	// create an invalid table name 
	m_pTable->MakeTableName(NULL);
	GetLiteralInfo();
	m = min(wcslen(m_pTable->GetTableName())+5, m_cMaxTableName);
	pwszTableName = BuildInvalidName(m, m_pTable->GetTableName(), m_pwszInvalidTableChars);
	odtLog << "invalid table name: " << pwszTableName << ", i.e." << wcslen(pwszTableName) << " characters\n";

	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszTableName;

	TESTC_(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE);
	fTestRes = TEST_PASS;

CLEANUP:
	PROVIDER_FREE(pwszTableName);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc A rowset is opened on table => DB_E_TABLEINUSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_8()
{
	BOOL		fTestRes = TEST_FAIL, fExists;
	IRowset*	pIRowset = NULL;
	
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	TESTC_(m_pIOpenRowset->OpenRowset(NULL, &m_pTable->GetTableID(), NULL, IID_IRowset, 
		0, NULL, (IUnknown**)&pIRowset), S_OK);

	fTestRes = TEST_PASS;
	//try to drop the table
	m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID());
	if (DB_E_TABLEINUSE != m_hr && !CHECK(m_hr, S_OK))
	{
		odtLog << "The table was dropped, though in use \n";
		fTestRes = TEST_FAIL;
	}
	else
	{
		odtLog << ((DB_E_TABLEINUSE == m_hr) ? "DB_E_TABLEINUSE returned\n": "S_OK returned\n");

		//check the table still exist
		if (!CHECK(m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK) || 
			(fExists==(S_OK == m_hr)))
			fTestRes = TEST_FAIL;
	}

CLEANUP:
	SAFE_RELEASE(pIRowset);
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Table in use => DB_E_TABLEINUSE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_9()
{
	BOOL		fTestRes = TEST_FAIL, fExists;
	IRowset*	pIRowset = NULL;
	
	m_pTable->SetRIID((struct _GUID*)&IID_IRowset);
	m_pTable->SetRowset((IUnknown**)&pIRowset);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(10, 1), S_OK);
	fTestRes = TEST_PASS;

	//try to release drop the table
	m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID());
	if (DB_E_TABLEINUSE != m_hr && S_OK != m_hr)
	{
		odtLog << "The table was dropped, though in use \n";
		fTestRes = TEST_FAIL;
	}
	else
	{
		odtLog << ((DB_E_TABLEINUSE == m_hr) ? "DB_E_TABLEINUSE returned\n" : "S_OK returned\n");

		//check the table still exist
		if (!CHECK(m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK) || (fExists==(S_OK == m_hr)))
			fTestRes = TEST_FAIL;
	}

CLEANUP:
	m_pTable->SetRowset(NULL);
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Inexistent table => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_10()
{
	WCHAR*	pwszTableName = NULL; 
	DBID	TableID;

	// create a new valid table name 
	m_pTable->MakeTableName(NULL);
	pwszTableName = m_pTable->GetTableName();
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszTableName;
	if (!CHECK(m_hr = m_pITableDefinition->DropTable(&TableID), DB_E_NOTABLE))
		return TEST_FAIL;
	else
		return TEST_PASS;
}
// }}


// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Abort with retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_11()
{
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
				SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0,0), S_OK);

	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
				CHECK(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned on further table access
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, TRUE, FALSE), S_OK);
		// check retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	// check table existence
	CHECK(m_hr = m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DDL_IGNORE != vSupportedTxnDDL.lVal
		&& DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Abort without retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_12()
{
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
				SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0,0), S_OK);

	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
				CHECK(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned on further table operations
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort non retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		// check not retaining
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			FAILED(m_hr)? S_OK : XACT_E_NOTRANSACTION);
	}

	// check table existence
	CHECK(m_hr = m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DDL_IGNORE != vSupportedTxnDDL.lVal
		&& DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Commit with retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_13()
{
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
				SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0,0), S_OK);

	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
				CHECK(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit retaining
		CHECK(m_hr = pITransactionLocal->Commit(TRUE, 0, 0), S_OK);
		// check retaining
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
	}

	// check table existence
	CHECK(m_hr = m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Commit without retaining
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_14()
{
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
				SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0,0), S_OK);

	TESTC_(m_hr = StartTransaction(pITransactionLocal), S_OK);

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
				CHECK(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			TESTC_(DropTableAndCheck(&m_pTable->GetTableID()), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit not retaining
		CHECK(m_hr = pITransactionLocal->Commit(FALSE, 0, 0), S_OK);
		// check not retaining
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			FAILED(m_hr)? S_OK: XACT_E_NOTRANSACTION);
	}

	// check whether the transaction was aborted
	CHECK(m_hr = m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DML == vSupportedTxnDDL.lVal);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Drop a table created with an SQL command => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_15()
{
	TBEGIN
	IDBCreateCommand	*pIDBCreateCommand = NULL;
	
	// check whether commands are supported
	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_IDBCreateCommand,
		SESSION_INTERFACE, (IUnknown**)&pIDBCreateCommand));

	GetModInfo()->UseITableDefinition(FALSE);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK);

CLEANUP:
	SAFE_RELEASE(pIDBCreateCommand);
	GetModInfo()->UseITableDefinition(TRUE);
	m_pTable->DropTable();
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Drop a table created with ITableDefinition, no rowset created => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_16()
{
	BOOL	fTestRes = TEST_FAIL;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK);
	fTestRes = TEST_PASS;

CLEANUP:
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Drop a table created with ITableDefinition, rowset released => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_17()
{
	BOOL	fTestRes = TEST_FAIL;
	IRowset	*pIRowset = NULL;

	m_pTable->SetRowset((IUnknown**)&pIRowset);
	m_pTable->SetRIID((struct _GUID*)&IID_IRowset);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	SAFE_RELEASE(pIRowset);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK);
	fTestRes = TEST_PASS;
CLEANUP:
	m_pTable->SetRowset(NULL);
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc ITableDefinition::CreateTable, insert 10 rows, release rowset, drop the table => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_18()
{
	BOOL	fTestRes = TEST_FAIL;
	IRowset	*pIRowset = NULL;

	m_pTable->SetRowset((IUnknown**)&pIRowset);
	m_pTable->SetRIID((struct _GUID*)&IID_IRowset);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(10, 0), S_OK);
	SAFE_RELEASE(pIRowset);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK);
	fTestRes = TEST_PASS;
CLEANUP:
	m_pTable->SetRowset(NULL);
	m_pTable->DropTable();
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc For each provider type, create a table with a single column and then drop it => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_19()
{
	BOOL			fTestRes = TEST_PASS;
	ULONG			i;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->SetBuildColumnDesc(FALSE);
	m_pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "\t" << rgColumnDesc[i].pwszTypeName;
		m_pTable->SetColumnDesc(NULL);
		m_pTable->SetColumnDesc(&rgColumnDesc[i], 1);
		if (!CHECK(m_hr = m_pTable->CreateTable(0, 0), S_OK))
		{
			fTestRes = TEST_FAIL;
			continue;
		}

		if (!CHECK(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK))
		{
			odtLog << "\n Error for type" << rgColumnDesc[i].pwszTypeName << "\n";
			fTestRes = TEST_FAIL;
		}
		m_pTable->DropTable();
	}
	odtLog << "\n";
	
	m_pTable->SetColumnDesc(NULL);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	return fTestRes;
}
// }}


// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Drop a table twice => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCDropTable::Variation_20()
{
	BOOL	fTestRes = TEST_FAIL;

	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), S_OK);
	TESTC_(m_hr = m_pITableDefinition->DropTable(&m_pTable->GetTableID()), DB_E_NOTABLE);
	fTestRes = TEST_PASS;

CLEANUP:	
	return fTestRes;
}
// }}


//----------------------------------------------------------------------
//
// @cmember Thread procedure for TCDropTable
//----------------------------------------------------------------------
unsigned TCDropTable::MyThreadProc(ULONG i)
{
	DBID			TableID;
	HRESULT			hr;

	if (i>= nThreads)
		return 0;
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_rgName[i];
	Sleep(0); // switch the context

	hr = m_pITableDefinition->DropTable(&TableID);
	Sleep(0); // switch the context

	m_rgResult[i] = hr;
	return 1;
} //TCDropTable::MyThreadProc


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc threads on different tables
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropTable::Variation_21()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i;
	DBID			TableID;

	// create n names and init the results
	for (i=0; i<nThreads; i++)
	{
		m_rgName[i] = NULL;
		m_rgResult[i] = E_FAIL;
	}
	for (i=0; i< nThreads; i++)
	{
		m_pTable->SetBuildColumnDesc(TRUE);
		TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
		m_rgName[i] = wcsDuplicate(m_pTable->GetTableName());		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// creates a table, build rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
		TESTC_(m_rgResult[i], S_OK);
	fTestResult = TEST_PASS;

CLEANUP:
	TableID.eKind = DBKIND_NAME;
	for (i=0; i<nThreads; i++)
	{
		TableID.uName.pwszName = m_rgName[i];
		// if the table was not dropped by the corresponding thread, try again
		if (FAILED(m_rgResult[i]))
			CHECK(DropTableAndCheck(&TableID), S_OK);
		PROVIDER_FREE(m_rgName[i]);
	}
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc threads on same table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropTable::Variation_22()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult		= TEST_FAIL;
	ULONG			i;
	ULONG			nS_OK			= 0;
	ULONG			nDB_E_NOTABLE	= 0;
	LPWSTR			pwszBaseName	= NULL;
	DBID			TableID;

	// create n names and init the results
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_hr = m_pTable->CreateTable(0, 0), S_OK);
	pwszBaseName = m_pTable->GetTableName();
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i]	= E_FAIL;
		m_rgName[i]		= pwszBaseName;
		rgThreadParam[i].pObject = this;
		rgThreadParam[i].i		= i;
	}
	
	// creates a table, build rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=nS_OK=nDB_E_NOTABLE=0; i<nThreads; i++)
	{
		if (m_rgResult[i] == S_OK)
			nS_OK++;
		else if (CHECK(m_rgResult[i], DB_E_NOTABLE))
			nDB_E_NOTABLE++;
		else
		{
			odtLog << "Thread " << i << ": unexpected result " << m_rgResult[i] << "\n";
			goto CLEANUP;
		}
	}

	TESTC(nS_OK == 1 && nDB_E_NOTABLE == nThreads-1);
	fTestResult = TEST_PASS;

CLEANUP:
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszBaseName;
	if (nS_OK < 1)
		CHECK(DropTableAndCheck(&TableID), S_OK);
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Fully Qualified Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropTable::Variation_23()
{ 
	BOOL		fTestRes = TEST_FAIL;
	DBID		TableID;
	WCHAR		*pwszQualTableName = NULL;
	WCHAR		*pwszCatalogName = NULL;
	WCHAR		*pwszSchemaName = NULL;
	WCHAR		*pwszTableName = NULL;

	// create a regular table
	TESTC(NULL != m_pTable);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	pwszTableName = m_pTable->GetTableName();
	TableID.eKind = DBKIND_NAME;

	TESTC(GetCatalogSchemaNames(pwszTableName, &pwszCatalogName, &pwszSchemaName));

	//Construct a fully Qualified TableName...
	TESTC_(m_pTable->GetQualifiedName(pwszCatalogName, pwszSchemaName, pwszTableName, &pwszQualTableName),S_OK);
	TableID.uName.pwszName = pwszQualTableName;

	// drop the table
	TESTC_(DropTableAndCheck(&TableID), S_OK);
	fTestRes = TEST_PASS;
    
CLEANUP:
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
	m_pTable->DropTable();
    return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Quoted Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropTable::Variation_24()
{ 
	BOOL	fTestRes = TEST_FAIL;
	DBID	TableID;
	
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = NULL;
	TESTC(NULL != m_pTable);
	m_pTable->SetBuildColumnDesc(TRUE);
	TESTC_(m_pTable->CreateTable(0, 0), S_OK);
	m_pTable->GetQuotedName(m_pTable->GetTableName(), &TableID.uName.pwszName);

	TESTC_(DropTableAndCheck(&TableID), S_OK);
	fTestRes = TEST_PASS;

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	m_pTable->DropTable();
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropTable::Terminate()
{
	// TO DO:  Add your own code here

	// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
}	// }}
// }}
// }}


// {{ TCW_TC_PROTOTYPE(TCCreateTable)
//*-----------------------------------------------------------------------
//| Test Case:		TCCreateTable - TestCase for table creation
//|	Created:			09/11/97
//*-----------------------------------------------------------------------


//--------------------------------------------------------------------
//
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//--------------------------------------------------------------------------
BOOL TCCreateTable::Init()
{
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{
		return TRUE;
	}
	return FALSE;
}




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Session interfaces
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_1()
{
	return DefaultObjectTesting(m_pITableDefinition, SESSION_INTERFACE);
}
// }}


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_2()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);

CLEANUP:
	// checkings, etc
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc ppTableID is a NULL pointer => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_3()
{
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset,
			0, NULL), S_OK);	
	
CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc pTableID and ppTableID both NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_4()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset,
			0, NULL, TRUE), E_INVALIDARG);
		
CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc *pTableID == DB_NULLID => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_5()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CCNDropTable((DBID*)&DB_NULLID, cColumnDesc, rgColumnDesc, IID_IRowset,
			0, NULL), DB_E_BADTABLEID);
	
CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc cColumnDesc == 0 => E_INVALIDARG or S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_6()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TEST2C_(CCNDropTable(NULL, 0, rgColumnDesc, IID_IRowset, 0, NULL), 
		S_OK, E_INVALIDARG);

CLEANUP:	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc rgColumnDesc == NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_7()
{
	TBEGIN

	
	TESTC_(CCNDropTable(NULL, m_cColumnDesc, NULL, IID_IRowset,
			0, NULL), E_INVALIDARG);

CLEANUP:
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc cPropertySets != 0 and rgPropertySets == NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_8()
{
	TBEGIN
	ULONG			ulNonZero = 2;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset,
			ulNonZero, NULL), E_INVALIDARG);
	
CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc cPropertySets is 0 and rgPropertySets != NULL => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_9()
{
	TBEGIN
	DBPROPSET		rgPropertySets[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropertySets[0], 1, rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], DBPROP_IAccessor, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED)
		
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset,
			0, rgPropertySets), S_OK);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc for a property sets, cProperties != 0 and rgProperties == NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_10()
{
	TBEGIN
	DBPROPSET		rgPropertySets[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	FILL_PROP_SET(rgPropertySets[0], 3, NULL, DBPROPSET_ROWSET)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset,
			1, rgPropertySets), E_INVALIDARG);

CLEANUP:	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}


// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc cProperties == 0 on a property set => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_11()
{
	TBEGIN
	DBPROPSET		rgPropertySets[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropertySets[0], 0, rgProp, DBPROPSET_ROWSET)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropertySets), S_OK);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc for a column, rgPropertySets is NULL though cPropertySets != 0 => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_12()
{
	TBEGIN
	ULONG			cProp = 3;
	DBPROPSET		rgPropSets[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].rgPropertySets	= rgPropSets;
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSets);
	FILL_PROP_SET(rgPropSets[0], cProp, NULL, DBPROPSET_COLUMN)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), E_INVALIDARG);
	
CLEANUP:
	// it is not required to release the memory for rgColumnDesc; it will be release in table destructor
	rgColumnDesc[0].rgPropertySets	= NULL;
	rgColumnDesc[0].cPropertySets	= 0;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc dbcid in an element of rgColumnDesc is DB_NULLID => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_13()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	// set a dbcid of an element in rgColumnDesc to DB_NULLID
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);		// cleans the container
	memcpy(&rgColumnDesc[0].dbcid, &DB_NULLID, sizeof(DBID));

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc dbcid in an element of rgColumnDesc is NULL => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_14()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set a dbcid of an element in rgColumnDesc to DB_NULLID
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);		// cleans the container
	rgColumnDesc[0].dbcid.eKind				= DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName	= NULL;

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc not null pclsid in a DBCOLUMNDESC element
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_15()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	POSITION		pos;
	CCol			col;
	HRESULT			hr;
	BOOL			fOleSupported	= FALSE;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set pclsid to IID_NULL
	rgColumnDesc[0].pclsid = (GUID*)&IID_NULL;

	// check whether this is ignored for a non-DBTYPE_IUNKNOWN column
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);

	// check DBTYPE_IUNKNOWN is supported
	for (pos = m_ColList.GetHeadPosition(); pos; )
	{
		col = m_ColList.GetNext(pos);
		if (DBTYPE_IUNKNOWN == col.GetProviderType())
		{
			fOleSupported = TRUE;
			break;
		}
	}

	// check whether a DBTYPE_IUNKNOWN column can be created in the table
	rgColumnDesc[0].wType	= DBTYPE_IUNKNOWN;
	PROVIDER_FREE(rgColumnDesc[0].pwszTypeName);
	if (fOleSupported)
		rgColumnDesc[0].pwszTypeName = wcsDuplicate(col.GetProviderTypeName());
	hr = fOleSupported ? S_OK : DB_E_BADTYPE;
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), hr);

CLEANUP:
	rgColumnDesc[0].pclsid = NULL;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc empty column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_16()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	// set a dbcid of an element in rgColumnDesc to DB_NULLID
	rgColumnDesc[0].dbcid.eKind				= DBKIND_NAME;
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);		// cleans the container
	rgColumnDesc[0].dbcid.uName.pwszName	= wcsDuplicate(L"");

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc empty table name => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_17()
{
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= L"";

	// create the empty name table
	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTABLEID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Maximum length for table name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_18()
{
	TBEGIN
	DBID	TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// create a maximum size table name 
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName, m_pTable->GetTableName());

	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc 1 char too long table name => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_19()
{
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// create a maximum size table name 
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName = BuildValidName(m_cMaxTableName+1, m_pTable->GetTableName());

	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTABLEID);

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Maximum length column name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_20()
{
	TBEGIN
	LPWSTR	pwszColumnName = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set a dbcid of an element in rgColumnDesc to DB_NULLID
	pwszColumnName = rgColumnDesc[0].dbcid.uName.pwszName;
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName	= BuildValidName(m_cMaxColName, pwszColumnName);
	// save the number of propertySets, to be able to release it ok

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);

CLEANUP:
	PROVIDER_FREE(pwszColumnName);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc 1 char too long column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_21()
{
	TBEGIN
	WCHAR			*pwszColumnName	= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	if (DBKIND_NAME == rgColumnDesc[0].dbcid.eKind)
		pwszColumnName = rgColumnDesc[0].dbcid.uName.pwszName;
	else
		pwszColumnName = wcsDuplicate(L"ColumnName");
	rgColumnDesc[0].dbcid.eKind			= DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName	= BuildValidName(m_cMaxColName+1, pwszColumnName);
	// save the number of propertySets, to be able to release it ok

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	PROVIDER_FREE(pwszColumnName);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc ulColumnSize 0 for a variable-length column => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_22()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// go through the list of column and set the column size to 0 for all variable-length columns
	for (ULONG i=0; i<cColumnDesc; i++)
	{
		if (!IsFixedLength(rgColumnDesc[i].wType))
		{
			rgColumnDesc[i].ulColumnSize = 0;
			odtLog << "Column " << rgColumnDesc[i].dbcid.uName.pwszName << " is set to length 0!\n";
		}

		CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc For each provider type create and drop a table with a single column => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_23()
{
	TBEGIN
	DBCOLUMNDESC	*tempColumnDesc;
	ULONG			i;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// go through the list of column and set the column size to 0 for all variable-length columns
	for (i=0, tempColumnDesc=rgColumnDesc; i<cColumnDesc; i++, tempColumnDesc++)
	{
		odtLog << "Create a table with a single column " << tempColumnDesc->dbcid.uName.pwszName << "\n";
		CHECK(CCNDropTable(NULL, 1, tempColumnDesc, IID_IRowset, 0, NULL), S_OK);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc invalid table name => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_24()
{
	TBEGIN
	WCHAR			*pwszTableName = NULL; 
	ULONG			n, m;
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	m = min(n=wcslen(m_pTable->GetTableName())+5, m_cMaxTableName);
	pwszTableName = BuildInvalidName(m, m_pTable->GetTableName(), m_pwszInvalidTableChars);
	TableID.uName.pwszName	= pwszTableName;

	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTABLEID);

CLEANUP:
	PROVIDER_FREE(pwszTableName);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc pTableID provides the name of an existing table => DB_E_DUPLICATETABLEID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_25()
{
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, &TableID, 
		cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_DUPLICATETABLEID);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc valid riid and NULL ppRowset => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_26()
{
	TBEGIN
	DBID			*pTableID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, 
		cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL, &pTableID, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc invalid column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_27()
{
	TBEGIN
	WCHAR			*pwszColName = NULL; 
	ULONG			n, m;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// create an invalid column name 
	m = min(n=wcslen(rgColumnDesc[0].dbcid.uName.pwszName)+5, m_cMaxColName);
	if (DBKIND_NAME == rgColumnDesc[0].dbcid.eKind)
		pwszColName = rgColumnDesc[0].dbcid.uName.pwszName;
	else
		pwszColName = wcsDuplicate(L"ColumnName");
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName = BuildInvalidName(m, 
		pwszColName, m_pwszInvalidColChars);
	SAFE_FREE(pwszColName);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc invalid type name for a column (NULL, empty or inexistent
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_28()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// build the column type name
	SAFE_FREE(rgColumnDesc[0].pwszTypeName);

	odtLog << "NULL type name\n";
	rgColumnDesc[0].pwszTypeName = NULL;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK); 

	odtLog << "empty type name for a column\n";
	rgColumnDesc[0].pwszTypeName = wcsDuplicate(L"");
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTYPE);

	rgColumnDesc[0].pwszTypeName = wcsDuplicate(L"Pay a trip to Europe");
	odtLog << "invalid type name (" << rgColumnDesc[0].pwszTypeName <<")\n";
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTYPE);
	

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc invalid wType for a column => DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_29()
{
	TBEGIN
	DBTYPE	wInvalidType	= 0xffff;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	PROVIDER_FREE(rgColumnDesc[0].pwszTypeName);
	rgColumnDesc[0].pwszTypeName = NULL;
	rgColumnDesc[0].wType = wInvalidType;

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTYPE);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc wType and pswzTypeName mismatch in a column description => DB_E_BADTYPE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_30()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set an invalid wType
	TESTC_PROVIDER(2 < cColumnDesc);
	
	// each column was assigned a different type; try to mismatch a pwszTypeName and wType for the first col
	PROVIDER_FREE(rgColumnDesc[0].pwszTypeName);
	rgColumnDesc[0].pwszTypeName	= rgColumnDesc[1].pwszTypeName;
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTYPE);

CLEANUP:
	if (cColumnDesc > 1)
		rgColumnDesc[0].pwszTypeName = NULL;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc not null pTypeInfo => DB_E_BADTYPE (for the time being
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_31()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set a not NULL pTypeInfo
	rgColumnDesc[0].pTypeInfo = (ITypeInfo*)rgColumnDesc[1].pwszTypeName;

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADTYPE);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc duplicate column identifiers => DB_E_DUPLICATECOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_32()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set an invalid wType
	TESTC_PROVIDER(2 < cColumnDesc);

	// each column was assigned a different type; try to mismatch a pwszTypeName and wType for the first col
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	DuplicateDBID(rgColumnDesc[1].dbcid, &rgColumnDesc[0].dbcid);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_DUPLICATECOLUMNID);

CLEANUP:
	memset(&rgColumnDesc[0].dbcid, 0, sizeof(DBID));
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc improper eKind value in a DBID => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_33()
{
	TBEGIN
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set improper eKind in a column DBID
	ReleaseDBID(&rgColumnDesc[0].dbcid, FALSE);
	rgColumnDesc[0].dbcid.eKind	= INVALID_DBKIND;

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_BADCOLUMNID);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(34)
//*-----------------------------------------------------------------------
// @mfunc invalid property value on column
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_34()
{
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	LONG			lValue = 12;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set improper value (different type than required) for a property value of a column 
	// DBPROPOPTIONS_REQUIRED
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_COLUMN)
	FILL_PROP(rgProp[0], DBPROP_COL_UNIQUE, DBTYPE_I4, V_I4, lValue, DBPROPOPTIONS_REQUIRED)
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_ERRORSOCCURRED);

	// set improper value (different type) for a column DBPROPOPTIONS_OPTIONAL
	// try to set the property value of the rowset trough a column
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_COLUMN)
	FILL_PROP(rgProp[0], DBPROP_COL_UNIQUE, DBTYPE_I4, V_I4, lValue, DBPROPOPTIONS_OPTIONAL)
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_S_ERRORSOCCURRED);

	// try to set a rowset property through a column, DBPROPOPTIONS_REQUIRED
	rgColumnDesc[0].cPropertySets		= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], DBPROP_BOOKMARKS, DBTYPE_I4, V_I4, lValue, DBPROPOPTIONS_REQUIRED)
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_ERRORSOCCURRED);

	// try to set a rowset property through a column, DBPROPOPTIONS_OPTIONAL
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], DBPROP_BOOKMARKS, DBTYPE_I4, V_I4, lValue, DBPROPOPTIONS_OPTIONAL)
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_S_ERRORSOCCURRED);
	
CLEANUP:
	rgColumnDesc[0].cPropertySets		= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(35)
//*-----------------------------------------------------------------------
// @mfunc Invalid column precision => DB_E_BADPRECISION
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_35()
{
	TBEGIN
	const int		n=8;
	BYTE			testValues[n];
	HRESULT			testResult[n]={S_OK, S_OK, S_OK, S_OK, S_OK, S_OK, DB_E_BADPRECISION, DB_E_BADPRECISION};
	CCol			Col;
	ULONG			i, cPrecision;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	testValues[0] = 0;
	testValues[1] = 1;
	for (i=0; i<cColumnDesc; i++)
	{
		Col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		ASSERT(0 == wcscmp(Col.GetProviderTypeName(), rgColumnDesc[i].pwszTypeName));
		
		if (rgColumnDesc[i].wType == DBTYPE_NUMERIC)
			testResult[0] = DB_E_BADPRECISION;
		else
			testResult[0] = S_OK;

		testResult[6] = S_OK;
		testResult[7] = S_OK;
		if (rgColumnDesc[i].wType == DBTYPE_NUMERIC)
			{
				testResult[6] = DB_E_BADPRECISION;
				testResult[7] = DB_E_BADPRECISION;

			}
		testValues[2] = (BYTE)(Col.GetPrecision()/3);
		testValues[3] = (BYTE)(2*Col.GetPrecision()/3);
		testValues[4] = (BYTE)Col.GetPrecision()-1;
		testValues[5] = (BYTE)Col.GetPrecision();
		testValues[6] = (BYTE)Col.GetPrecision()+1;
		testValues[7] = (BYTE)Col.GetPrecision()+10;
		odtLog	<< rgColumnDesc[i].pwszTypeName  <<"  (" 
				<< rgColumnDesc[i].dbcid.uName.pwszName  << ") max precision =" << testValues[5] << "\n";
		odtLog	<< "\t" << "precisions tested :";
		for (cPrecision=0; cPrecision<n; cPrecision++)
			odtLog << "\t" << (ULONG)testValues[cPrecision];
		odtLog << "\n";

		for (cPrecision=0; cPrecision<n; cPrecision++)
		{
			rgColumnDesc[i].bPrecision		= (BYTE)testValues[cPrecision];
			rgColumnDesc[i].ulColumnSize	= 0;
			rgColumnDesc[i].bScale			= 0;

			if (!CHECK(CCNDropTable(NULL, 1, &rgColumnDesc[i], IID_IRowset, 0, NULL), testResult[cPrecision]))
			{
				odtLog << "Error for precision " << testValues[cPrecision] << "\n";
			}		
		}
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(36)
//*-----------------------------------------------------------------------
// @mfunc invalid scale on column => DB_E_BADSCALE
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_36()
{
	TBEGIN
	const int		n=7;
	BYTE			testValues[n];
	HRESULT			testResult[n]={S_OK, S_OK, S_OK, S_OK, S_OK, DB_E_BADSCALE, DB_E_BADSCALE};
	CCol			Col;
	ULONG			i, cScale, m;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	testValues[0] = 0;
	testValues[1] = 1;
	for (i=0; i<cColumnDesc; i++)
	{
		Col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		ASSERT(0 == wcscmp(Col.GetProviderTypeName(), rgColumnDesc[i].pwszTypeName));

		odtLog << rgColumnDesc[i].pwszTypeName << "\n";
		testResult[0] = S_OK;
		testResult[1] = S_OK;
		testValues[0] = 0;
		testValues[1] = 1;
		testValues[2] = 100;
		testValues[3] = (BYTE)Col.GetPrecision()+1;
		m = 4;
		if ((rgColumnDesc[i].wType == DBTYPE_NUMERIC) || (rgColumnDesc[i].wType == DBTYPE_DECIMAL))
		{
			m = 7;
			if (Col.GetMinScale()>0)
			{
				testResult[0] = DB_E_BADSCALE;
				testResult[1] = DB_E_BADSCALE;
			}
			testResult[4] = S_OK;
			testValues[1] = (BYTE)max(Col.GetMinScale()-1, 0);
			testValues[2] = (BYTE)Col.GetMinScale();
			testValues[3] = (BYTE)((Col.GetMinScale()+Col.GetMaxScale())/2);
			testValues[4] = (BYTE)Col.GetMaxScale();
			testValues[5] = (BYTE)Col.GetMaxScale()+1;
			testValues[6] = (BYTE)Col.GetMaxScale();
			// tests
			for (cScale=0; cScale<m; cScale++)
			{
				rgColumnDesc[i].bScale = (BYTE)testValues[cScale];
				if (cScale+1 == m)
				{
					rgColumnDesc[i].bPrecision = max(1, Col.GetMaxScale()-2);
					odtLog	<< "\nprecision: " << rgColumnDesc[i].bPrecision 
							<< ", scale: " << testValues[cScale] <<"\n";
				}
				else
					odtLog << "\t" << testValues[cScale];

				if (!CHECK(CCNDropTable(NULL, 1, &rgColumnDesc[i], IID_IRowset, 0, NULL), testResult[cScale]))
				{
					odtLog << "Error for scale " << testValues[cScale] << "\n";
				}		
			}
			odtLog << "\n";
		}
		else
		{
			testResult[4] = S_OK;
			for (cScale=0; cScale<m; cScale++)
			{
				rgColumnDesc[i].bScale = (BYTE)testValues[cScale];
				if (cScale+1 == m)
					rgColumnDesc[cScale].bPrecision = Col.GetMaxScale()-1;
				
				if (!CHECK(CCNDropTable(NULL, 1, &rgColumnDesc[i], 
							IID_IRowset, 0, NULL), testResult[cScale]))
				{
					odtLog << "Error for scale " << testValues[cScale] << "\n";
				}
			}
		}
	}
	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(37)
//*-----------------------------------------------------------------------
// @mfunc col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_SETIFCHEAP => DB_S_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_37()
{
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set a column property that doesn't belong to the column, rowset or table propriety group
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_DBINIT)
	FILL_PROP(rgProp[0], DBPROP_AUTH_PASSWORD, DBTYPE_BSTR, V_BSTR, 
		SysAllocString(L"passwd"), DBPROPOPTIONS_OPTIONAL)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_S_ERRORSOCCURRED);

CLEANUP:
	SysFreeString(V_BSTR(&rgProp[0].vValue));
	rgColumnDesc[0].cPropertySets		= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(38)
//*-----------------------------------------------------------------------
// @mfunc col prop is not in the Column, Rowset or Table property group and dwOption is DBPROP_REQUIRED => DB_E_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_38()
{
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set a column property ID that is not valid
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], NUMELEM(rgProp), rgProp, DBPROPSET_DBINIT)
	FILL_PROP(rgProp[0], DBPROP_AUTH_PASSWORD, DBTYPE_BSTR, V_BSTR, 
		SysAllocString(L"passwd"), DBPROPOPTIONS_REQUIRED)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_ERRORSOCCURRED);

CLEANUP:
	SysFreeString(V_BSTR(&rgProp[0].vValue));
	rgColumnDesc[0].cPropertySets		= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(39)
//*-----------------------------------------------------------------------
// @mfunc invalid propID for a column propriety
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_39()
{
	TBEGIN
	DBPROPSET	rgPropSet[1];
	DBPROP		rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// set an inexistent column property with DBPROPOPTIONS_OPTIONAL
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_COLUMN)
	FILL_PROP(rgProp[0], 0xffff, DBTYPE_I4, V_I4, 42, DBPROPOPTIONS_OPTIONAL)
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_S_ERRORSOCCURRED);

	// set an inexistent column property with DBPROPOPTIONS_REQUIRED
	rgColumnDesc[0].cPropertySets	= NUMELEM(rgPropSet);
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_COLUMN)
	FILL_PROP(rgProp[0], 0xffff, DBTYPE_I4, V_I4, 42, DBPROPOPTIONS_REQUIRED)

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_ERRORSOCCURRED);

CLEANUP:
	rgColumnDesc[0].cPropertySets = 0;
	rgColumnDesc[0].rgPropertySets = NULL;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(40)
//*-----------------------------------------------------------------------
// @mfunc colid in DBPROP of a column property is ignored
//
// @rdesc TEST_PASS or TEST_FAIL
//
int TCCreateTable::Variation_40()
{
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPID		rgPropID[]		= {	DBPROP_COL_AUTOINCREMENT, DBPROP_COL_NULLABLE};
	ULONG			i;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].cPropertySets	= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;

	// set a column property that is not ...
	for (i=0; i<NUMELEM(rgPropID); i++)
	{
		if (SettableProperty(rgPropID[i], DBPROPSET_COLUMN))
			break;
	}
	TESTC_PROVIDER(i<NUMELEM(rgPropID));

	rgColumnDesc[0].cPropertySets	= 1;
	rgColumnDesc[0].rgPropertySets	= rgPropSet;
	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_COLUMN)
	FILL_PROP(rgProp[0], rgPropID[i], DBTYPE_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	// set colID to a value different from DB_NULLID	
	rgProp[0].colid.eKind			= DBKIND_NAME;
	rgProp[0].colid.uName.pwszName	= wcsDuplicate(L"someColumn");

	// check colid is ignored for the column property
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);
	COMPARE(DBPROPSTATUS_OK == rgProp[0].dwStatus, TRUE);

	// check colid is ignored for the column property
	rgProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);
	COMPARE(DBPROPSTATUS_OK == rgProp[0].dwStatus, TRUE);

CLEANUP:
	PROVIDER_FREE(rgProp[0].colid.uName.pwszName);
	rgColumnDesc[0].cPropertySets		= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
}
// }}


// {{ TCW_VAR_PROTOTYPE(41)
//*-----------------------------------------------------------------------
// @mfunc Abort retaining
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_41()
{ 
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	DBID				*pTableID = NULL;
	DBPROPSET			*rgPropSet = NULL;
	ULONG				cPropSet = 0;
	IRowsetInfo			*pIRowsetInfo = NULL;
	BOOL				fAbortPreserve = TRUE;
	ULONG				cReadPropSet = 0;
	DBPROPSET			*rgReadPropSet = NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
		SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// if possible, prepare to ask DBPROP_ABORTPRESERVE to be set to VARIANT_FALSE
	if (SettableProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET))
	{
		::SetProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET, &cPropSet, &rgPropSet, 
			(LPVOID)VARIANT_FALSE, VT_BOOL, DBPROPOPTIONS_REQUIRED, DB_NULLID);
	}

	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, 
				cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
				cPropSet, rgPropSet, &pTableID, 
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort retaining
		CHECK(pITransactionLocal->Abort(NULL, TRUE, FALSE), S_OK);
		// check that it is retaining (another transaction is available)
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		fAbortPreserve = !GetProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET, pIRowsetInfo, VARIANT_FALSE);
	}

	// abort retaining and check that the table doesn't exist anymore
	CHECK(m_hr = m_pTable->DoesTableExist(pTableID, &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		|| DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal);
	if (fExists)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
	}

	// test zombie rowset
	if (!fAbortPreserve)
		TESTC_(pIRowsetInfo->GetProperties(0, NULL, &cReadPropSet, &rgReadPropSet), E_UNEXPECTED);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	SAFE_RELEASE(pIRowsetInfo);
	ReleaseDBID(pTableID);
	FreeProperties( &cPropSet, &rgPropSet);
	FreeProperties( &cReadPropSet, &rgReadPropSet);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(42)
//*-----------------------------------------------------------------------
// @mfunc Abort without retaining
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_42()
{ 
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	DBID				*pTableID = NULL;
	DBPROPSET			*rgPropSet = NULL;
	ULONG				cPropSet = 0;
	IRowsetInfo			*pIRowsetInfo = NULL;
	BOOL				fAbortPreserve = TRUE;
	ULONG				cReadPropSet = 0;
	DBPROPSET			*rgReadPropSet = NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
		SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// if possible, prepare to ask DBPROP_ABORTPRESERVE to be set to VARIANT_FALSE
	if (SettableProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET))
	{
		::SetProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET, &cPropSet, &rgPropSet, 
			(LPVOID)VARIANT_FALSE, VT_BOOL, DBPROPOPTIONS_REQUIRED, DB_NULLID);
	}
	
	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, 
				cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
				cPropSet, rgPropSet, &pTableID, 
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// abort non retaining
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		// check that it is retaining (another transaction is available)
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			FAILED(m_hr)? S_OK : XACT_E_NOTRANSACTION);
		fAbortPreserve = !GetProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET, pIRowsetInfo, VARIANT_FALSE);
	}

	// abort retaining and check that the table doesn't exist anymore
	CHECK(m_hr = m_pTable->DoesTableExist(pTableID, &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DDL_IGNORE == vSupportedTxnDDL.lVal
		|| DBPROPVAL_TC_DDL_COMMIT == vSupportedTxnDDL.lVal);
	if (fExists)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
	}

	// test zombie rowset
	if (!fAbortPreserve)
		TESTC_(pIRowsetInfo->GetProperties(0, NULL, &cReadPropSet, &rgReadPropSet), E_UNEXPECTED);

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	SAFE_RELEASE(pIRowsetInfo);
	ReleaseDBID(pTableID);
	FreeProperties( &cPropSet, &rgPropSet);
	FreeProperties(&cReadPropSet, &rgReadPropSet);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(43)
//*-----------------------------------------------------------------------
// @mfunc Commit retain
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_43()
{ 
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	DBID				*pTableID = NULL;
	DBPROPSET			*rgPropSet = NULL;
	ULONG				cPropSet = 0;
	IRowsetInfo			*pIRowsetInfo = NULL;
	BOOL				fCommitPreserve = TRUE;
	ULONG				cReadPropSet = 0;
	DBPROPSET			*rgReadPropSet = NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
		SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// if possible, prepare to ask DBPROP_ABORTPRESERVE to be set to VARIANT_FALSE
	if (SettableProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET))
	{
		::SetProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET, &cPropSet, &rgPropSet, 
			(LPVOID)VARIANT_FALSE, VT_BOOL, DBPROPOPTIONS_REQUIRED, DB_NULLID);
	}
	
	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, 
				cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
				cPropSet, rgPropSet, &pTableID, 
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit retaining
		CHECK(m_hr = pITransactionLocal->Commit(TRUE, 0, 0), S_OK);
		// check that it is retaining (another transaction is available)
		CHECK(m_hr = pITransactionLocal->Abort(NULL, FALSE, FALSE), S_OK);
		fCommitPreserve = !GetProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET, pIRowsetInfo, VARIANT_FALSE);
	}

	// abort retaining and check that the table doesn't exist anymore
	CHECK(m_hr = m_pTable->DoesTableExist(pTableID, &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DML != vSupportedTxnDDL.lVal);
	if (fExists)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
	}

	// test zombie rowset
	if (!fCommitPreserve)
		TESTC_(pIRowsetInfo->GetProperties(0, NULL, &cReadPropSet, &rgReadPropSet), E_UNEXPECTED);
	

CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	SAFE_RELEASE(pIRowsetInfo);
	DropTableAndCheck(pTableID);
	ReleaseDBID(pTableID);
	FreeProperties( &cPropSet, &rgPropSet);
	FreeProperties(&cReadPropSet, &rgReadPropSet);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(44)
//*-----------------------------------------------------------------------
// @mfunc Commit non-retaining
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_44()
{ 
	TBEGIN
	BOOL				fExists;
	ITransactionLocal	*pITransactionLocal = NULL;
	DBID				*pTableID = NULL;
	DBPROPSET			*rgPropSet = NULL;
	ULONG				cPropSet = 0;
	IRowsetInfo			*pIRowsetInfo = NULL;
	BOOL				fCommitPreserve = TRUE;
	ULONG				cReadPropSet = 0;
	DBPROPSET			*rgReadPropSet = NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	VARIANT				vSupportedTxnDDL;

	TESTC_PROVIDER(VerifyInterface(m_pITableDefinition, IID_ITransactionLocal,
		SESSION_INTERFACE, (IUnknown**)&pITransactionLocal));

	VariantInit(&vSupportedTxnDDL);
	TESTC_PROVIDER(GetProperty(DBPROP_SUPPORTEDTXNDDL, DBPROPSET_DATASOURCEINFO, 
			g_pIDBInitialize, &vSupportedTxnDDL));
	
	TESTC(VT_I4 == vSupportedTxnDDL.vt);
	// what values should we accept?
	TESTC_PROVIDER(DBPROPVAL_TC_NONE != vSupportedTxnDDL.lVal);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(StartTransaction(pITransactionLocal), S_OK);

	// if possible, prepare to ask DBPROP_ABORTPRESERVE to be set to VARIANT_FALSE
	if (SettableProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET))
	{
		::SetProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET, &cPropSet, &rgPropSet, 
			(LPVOID)VARIANT_FALSE, VT_BOOL, DBPROPOPTIONS_REQUIRED, DB_NULLID);
	}
	
	switch(vSupportedTxnDDL.lVal)
	{
		case DBPROPVAL_TC_DML:
			//check for XACT_E_XTIONEXISTS
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), XACT_E_XTIONEXISTS);
			break;

		case DBPROPVAL_TC_DDL_IGNORE:
			// the statement is not transacted
		case DBPROPVAL_TC_ALL:
		case DBPROPVAL_TC_DDL_COMMIT:
			CHECK(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, 
				cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
				cPropSet, rgPropSet, &pTableID, 
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;

		case DBPROPVAL_TC_DDL_LOCK:
			// error should be returned
			CHECK(m_hr = m_pITableDefinition->CreateTable(NULL, NULL, cColumnDesc, rgColumnDesc,
				IID_IRowsetInfo, cPropSet, rgPropSet, &pTableID,
				(IUnknown**)&pIRowsetInfo), S_OK);
			break;
			
		default:
			TESTC(FALSE);
	}

	if (DBPROPVAL_TC_DDL_COMMIT != vSupportedTxnDDL.lVal)
	{
		// commit non retaining
		CHECK(m_hr = pITransactionLocal->Commit(FALSE, 0, 0), S_OK);
		// check that it is retaining (another transaction is available)
		CHECK(pITransactionLocal->Abort(NULL, FALSE, FALSE), 
			FAILED(m_hr)? S_OK: XACT_E_NOTRANSACTION);
		fCommitPreserve = !GetProperty(DBPROP_COMMITPRESERVE, DBPROPSET_ROWSET, pIRowsetInfo, VARIANT_FALSE);
	}

	// abort retaining and check that the table doesn't exist anymore
	CHECK(m_hr = m_pTable->DoesTableExist(pTableID, &fExists), S_OK);
	COMPARE(fExists, DBPROPVAL_TC_DML != vSupportedTxnDDL.lVal);
	if (fExists)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
		goto CLEANUP;
	}

	// test zombie rowset
	if (!fCommitPreserve)
		TESTC_(pIRowsetInfo->GetProperties(0, NULL, &cReadPropSet, &rgReadPropSet), E_UNEXPECTED);
	
CLEANUP:
	SAFE_RELEASE(pITransactionLocal);
	SAFE_RELEASE(pIRowsetInfo);
	DropTableAndCheck(pTableID);
	ReleaseDBID(pTableID);
	FreeProperties( &cPropSet, &rgPropSet);
	FreeProperties(&cReadPropSet, &rgReadPropSet);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(45)
//*-----------------------------------------------------------------------
// @mfunc Autoincrementable columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_45()
{ 
	TBEGIN
	ULONG		i;
	HRESULT		resA, resB;
	CCol		col;
	BOOL		fReadWriteAutoInc;
	HRESULT		hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	fReadWriteAutoInc = (GetPropInfoFlags(DBPROP_COL_AUTOINCREMENT, DBPROPSET_COLUMN, 
			m_pThisTestModule->m_pIUnknown, DATASOURCE_INTERFACE) & DBPROPFLAGS_WRITE);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);

	// try all data types
	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";
		// try to create a table in which column rgColumnDesc[i].pwszTypeName is:
		// a) autoincrementable
		// b) not autoincrementable
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		resA = col.CanAutoInc()? S_OK: DB_E_ERRORSOCCURRED;
		resB = !col.CanAutoInc() || fReadWriteAutoInc? S_OK : DB_E_ERRORSOCCURRED;

		// create a table with an autoincrementable column
		rgColumnDesc[i].bScale			= 0;
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_AUTOINCREMENT,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

		CHECK(hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
			0, NULL), resA);

		if (S_OK == hr)
		{
			COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_OK);
		}
		else
		{
			CHECK(hr, DB_E_ERRORSOCCURRED);

			if (!fReadWriteAutoInc && !col.CanAutoInc())
				COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_NOTSETTABLE);
		}

		odtLog << ((hr == S_OK) ? "\tcolumn added for DBPROPCOL_AUTOINCREMENT set to VARIANT_TRUE\n":
			"\tcolumn NOT added for DBPROPCOL_AUTOINCREMENT set to VARIANT_TRUE\n");
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);

		// create a table with a column property value to VARIANT_FALSE for DBPROP_COL_AUTOINCREMENT
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_AUTOINCREMENT,
					VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);

		CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), resB);

		odtLog << ((S_OK == hr) ? "\tcolumn added for DBPROPCOL_AUTOINCREMENT set to VARIANT_FALSE\n":
			"\tcolumn NOT added for DBPROPCOL_AUTOINCREMENT set to VARIANT_FALSE\n");
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(46)
//*-----------------------------------------------------------------------
// @mfunc Default values
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_46()
{ 
	TBEGIN
	ULONG					i;
	CCol					col;
	VARIANT					value;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CTable					*pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	BOOL					fSettable = TRUE;
	BOOL					fSupported = TRUE;
	HRESULT					hr;

	if (!SupportedProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
	{
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
		if (!SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
			fSettable = FALSE;

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";

		// try to create a table in which column rgColumnDesc[i].pwszTypeName has a default value
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

		// create a table with a default value column
		VariantInit(&value);
		GetDefaultValue(&rgColumnDesc[i], &value, pTable);
		SetDefaultProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
			&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
		VariantClear(&value);

		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset,	0, NULL);

		if (S_OK == hr)
		{
			COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_OK);
		}
		else
		{
			odtLog << "Failed on creating a table with a DBPROP_COL_DEFAULT property value set for this type.\n";
			CHECK(hr, DB_E_ERRORSOCCURRED);

			if (!fSupported)
			{
				COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_NOTSUPPORTED);
			}
			else
				if (!fSettable)
				{
					COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_BADVALUE);
				}
				else
					COMPARE(rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus, DBPROPSTATUS_BADVALUE);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	delete pTable;
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(47)
//*-----------------------------------------------------------------------
// @mfunc Column descriptions
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_47()
{ 
	TBEGIN
	ULONG			i;
	CCol			col;
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);
	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";
		// try to create a table in which column rgColumnDesc[i].pwszTypeName has a description
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

		// create a table with a described column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_DESCRIPTION,
					VT_BSTR, (LPVOID)L"description");

		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);

		if (S_OK != hr && !CHECK(hr, DB_S_ERRORSOCCURRED))
		{
			odtLog << "Could not create a table with DBPROP_COL_DESCRIPTION property value set for this type.\n";
		}
		else
		{
			if (DB_S_ERRORSOCCURRED == hr)
				odtLog << "DB_S_ERRORSOCCURRED\n";
		}
		
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(48)
//*-----------------------------------------------------------------------
// @mfunc Variable length columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_48()
{ 
	TBEGIN
	ULONG		i;
	VARIANT		value;
	CCol		col;
	HRESULT		hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	value.vt = VT_BOOL;
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		if (	DBTYPE_STR != rgColumnDesc[i].wType
			&&	DBTYPE_WSTR != rgColumnDesc[i].wType
			&&	DBTYPE_BYTES != rgColumnDesc[i].wType)
			continue;

		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";
		// try to create a table in which column rgColumnDesc[i].pwszTypeName is:
		// a) fixed length
		// b) variable length
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

		// create a table with an fixed length column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_FIXEDLENGTH,
					VT_BOOL, (LPVOID)VARIANT_TRUE);
		odtLog	<< "\ttry to create a table with a fixed length column of type " 
				<< rgColumnDesc[i].pwszTypeName << "\n";
		
		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);

		if (S_OK != hr && !CHECK(hr, DB_S_ERRORSOCCURRED))
		{
			odtLog << "\t\tProblems in creating a table with DBPROP_COL_FIXEDLENGTH property value set to VARIANT_TRUE for this column.\n";
		}
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);
		
		// create a table with a column property value set to VARIANT_FALSE for DBPROP_COL_FIXEDLENGTH
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_FIXEDLENGTH,
					VT_BOOL, (LPVOID)VARIANT_FALSE);
		odtLog	<< "\ttry to create a table with a variable length column of type " 
				<< rgColumnDesc[i].pwszTypeName << "\n";

		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);

		if (S_OK != hr && !CHECK(hr, DB_S_ERRORSOCCURRED))
		{
			odtLog << "\t\tCould not create a table with DBPROP_COL_FIXEDLENGTH property value set to VARIANT_FALSE for this column.\n";
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(49)
//*-----------------------------------------------------------------------
// @mfunc Nullable/non nullable columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_49()
{ 
	BOOL					fTestRes = TEST_PASS;
	ULONG					i;
	CCol					col;
	VARIANT					defval;
	HRESULT					hr;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CTable					*pTable			= new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;
	HRESULT					hExpRes			= S_OK;
	BOOL					fSettable		= TRUE;
	BOOL					fSupported		= TRUE;
	DBPROP					*pPropDefault	= NULL;
	DBPROPSTATUS			ulStatus;

	if (!SupportedProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN))
	{
		hExpRes		= DB_S_ERRORSOCCURRED;
		fSettable	= FALSE;
		fSupported	= FALSE;
	}
	else
		if (!SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN))
			fSettable = FALSE;

	pTable->CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName;
		// try to create a table in which column i is:
		// a) nullable
		// b) not nullable
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);
		switch (col.GetNullable())
		{
			case 0:
				odtLog << " (not nullable)\n";
				break;
			case 1:
				odtLog << " (NULLable)\n";
				break;
			default:
				odtLog << "\n";
		}

		// create a table with a nullable column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_NULLABLE,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		
		odtLog << "\tcreate table with a nullable column: ";
		// create a table with a column property value set to VARIANT_TRUE for DBPROP_COL_NULLABLE
		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);

		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		if (	(S_OK == hr && !COMPARE(ulStatus, DBPROPSTATUS_OK))
			||	(!fSupported && !COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED))
			||	(S_OK != hr && fSettable && !COMPARE(ulStatus, DBPROPSTATUS_BADVALUE))
			||	(S_OK != hr && !fSettable && !COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE))
			||	(!fSupported && !CHECK(hr, DB_E_ERRORSOCCURRED))
			||	(S_OK != hr && !CHECK(hr, DB_E_ERRORSOCCURRED)))
			fTestRes = TEST_FAIL;
		
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	

		switch (col.GetNullable())
		{
			case 0:
				if (!CHECK(hr, DB_E_ERRORSOCCURRED))
					fTestRes = TEST_FAIL;
				break;
			case 1:
				if (!CHECK(hr, S_OK))
					fTestRes = TEST_FAIL;
				break;
			case -1:
				if (S_OK != hr && !CHECK(hr, DB_E_ERRORSOCCURRED))
					fTestRes = TEST_FAIL;
				break;
			default:
				ASSERT(FALSE);
				break;
		}

		odtLog << ((SUCCEEDED(hr)) ? "CREATED\n" : "INSUCCESS\n");

		// create a table with a column property value set to VARIANT_FALSE for DBPROP_COL_NULLABLE
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_NULLABLE,
					VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
		if (SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
		{
			VariantInit(&defval);
			GetDefaultValue(&rgColumnDesc[i], &defval, pTable);
			if (SettableProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
				SetDefaultProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets,
					&defval, DBPROPOPTIONS_REQUIRED);	// might be not supported
			VariantClear(&defval);
		}

		odtLog << "\tcreate table with a not nullable column\n";
		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);
		
		ulStatus = rgColumnDesc[i].rgPropertySets[0].rgProperties[0].dwStatus;
		FindProperty(
			DBPROP_COL_DEFAULT,
			DBPROPSET_COLUMN,
			rgColumnDesc[i].cPropertySets,
			rgColumnDesc[i].rgPropertySets, 
			&pPropDefault
		);

		if (	(!pPropDefault || DBPROPSTATUS_OK == pPropDefault->dwStatus) 
			&&	(	(S_OK == hr && !COMPARE(ulStatus, DBPROPSTATUS_OK))
				||	(!fSupported && !COMPARE(ulStatus, DBPROPSTATUS_NOTSUPPORTED))
				||	(S_OK != hr && fSettable && !COMPARE(ulStatus, DBPROPSTATUS_BADVALUE))
				||	(S_OK != hr && !fSettable && !COMPARE(ulStatus, DBPROPSTATUS_NOTSETTABLE))
				||	(!fSupported && !CHECK(hr, DB_E_ERRORSOCCURRED))
				||	(S_OK != hr && !CHECK(hr, DB_E_ERRORSOCCURRED))))
			fTestRes = TEST_FAIL;

		if (FAILED(hr))
			odtLog << "Could not create a table with DBPROP_COL_NULLABLE property value set to VARIANT_FALSE.\n";

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	delete pTable;
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(50)
//*-----------------------------------------------------------------------
// @mfunc Primary key column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_50()
{ 
	TBEGIN
	ULONG			i;
	CCol			col;
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	HRESULT			hExpRes			= S_OK;

	if (!SupportedProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN))
		hExpRes		= DB_S_ERRORSOCCURRED;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	// this is to prevent SQL Server limiting the size of
	// the index it creates for primary key or unique columns
	SetZeroColSize(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";
		// try to create a table in which column _rgColumnDesc[i].pwszTypeName is:
		// a) primary key
		// b) not primary key
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

		// create a table with an primary key column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_PRIMARYKEY,
					VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_OPTIONAL);

		if (S_OK != (hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL)))
		{
			CHECK(hr, DB_S_ERRORSOCCURRED);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
		
		// create a table with a column property value set to VARIANT_FALSE for DBPROP_COL_PRIMARYKEY
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_PRIMARYKEY,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);

		if (1 != col.GetNullable())
			hExpRes = DB_S_ERRORSOCCURRED;

		if (S_OK != (hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL)))
		{
			CHECK(hr, DB_S_ERRORSOCCURRED);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(51)
//*-----------------------------------------------------------------------
// @mfunc Unique columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_51()
{ 
	TBEGIN
	ULONG			i;
	CCol			col;
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	BOOL			fSupported		= FALSE;

	fSupported = SupportedProperty(DBPROP_COL_DEFAULT, DBPROPSET_COLUMN);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	// this is to prevent SQL Server limiting the size of
	// the index it creates for primary key or unique columns
	SetZeroColSize(rgColumnDesc, cColumnDesc);

	for (i=0; i<cColumnDesc; i++)
	{
		odtLog << "Type " << rgColumnDesc[i].pwszTypeName << "\n";
		// try to create a table in which column rgColumnDesc[i].pwszTypeName is:
		// a) unique
		// b) non-unique
		// there is no info whether a type can or cannot be unique
		col = GetType(rgColumnDesc[i].pwszTypeName, rgColumnDesc[i].wType);

		// create a table with an unique column
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_UNIQUE,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	
		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);
		
		if (S_OK != hr)
		{
			odtLog << "Could not create a table with DBPROP_COL_UNIQUE property value set to VARIANT_TRUE for this column.\n";
			CHECK(hr, DB_E_ERRORSOCCURRED);
		}
		else
		{
			COMPARE(fSupported, TRUE);
		}
		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);

		// create a table with a column property value set to VARIANT_FALSE for DBPROP_COL_UNIQUE
		SetProperty(&rgColumnDesc[i].rgPropertySets, &rgColumnDesc[i].cPropertySets, DBPROP_COL_UNIQUE,
					VT_BOOL, (LPVOID)VARIANT_FALSE, DBPROPOPTIONS_OPTIONAL);

		hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL);

		if (S_OK != hr)
		{
			odtLog << "Could not create a table with DBPROP_COL_UNIQUE property value set to VARIANT_FALSE for this column.\n";
			CHECK(hr, DB_S_ERRORSOCCURRED);
		}
		else
		{
			COMPARE(fSupported, TRUE);
		}

		FreeProperties( &rgColumnDesc[i].cPropertySets, &rgColumnDesc[i].rgPropertySets);	
	}
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(52)
//*-----------------------------------------------------------------------
// @mfunc Test conflicting properties
// a column that is both nullable and primary key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_52()
{ 
	TBEGIN
	CCol			col;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	
	TESTC_PROVIDER(SettableProperty(DBPROP_COL_NULLABLE, DBPROPSET_COLUMN)
		&&	SettableProperty(DBPROP_COL_PRIMARYKEY, DBPROPSET_COLUMN));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);

	// create a table with a column  showing contradiction in properties
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets, DBPROP_COL_PRIMARYKEY,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets, DBPROP_COL_NULLABLE,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), DB_E_ERRORSOCCURRED);

CLEANUP:		
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(53)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_ABORTPRESERVE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_53()
{ 
	TBEGIN
	DBPROPSET		*rgPropertySets = NULL;
	ULONG			cPropertySets = 0;
	CCol			col;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	//SetZeroColSize(rgColumnDesc, cColumnDesc);

	TESTC_PROVIDER(SupportedProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET)
		&&	SettableProperty(DBPROP_ABORTPRESERVE, DBPROPSET_ROWSET));
	SetProperty(&rgPropertySets, &cPropertySets, DBPROP_ABORTPRESERVE,
					VT_BOOL, (LPVOID)VARIANT_TRUE, DBPROPOPTIONS_REQUIRED, DB_NULLID, DBPROPSET_ROWSET);

	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
		cPropertySets, rgPropertySets), S_OK);

CLEANUP:
	FreeProperties( &cPropertySets, &rgPropertySets);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(54)
//*-----------------------------------------------------------------------
// @mfunc Inserting rows after table creation, using the resulting rowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_54()
{ 
	IRowsetChange			*pIRowsetChange	= NULL;
	BOOL					fTestRes		= TEST_FAIL;
	BOOL					fMayWrite;
	ULONG					i;
	const ULONG				nRows			= 10;
	DBID					*pTableID		= NULL;
	ULONG					cPropSets		= 0;
	DBPROPSET				*rgPropSets		= NULL;
	LONG					lUpdateInsert	= DBPROPVAL_UP_INSERT; 
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CList <WCHAR*, WCHAR*>	ListNativeTemp;
	CList <DBTYPE, DBTYPE>	ListDataTypes;
	CTable					*pTable			= NULL;

	pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	// get the provider types list
	pTable->CreateColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	// build the column description array
	pTable->BuildColumnDescs(&rgColumnDesc);
	cColumnDesc = pTable->CountColumnsOnTable();
	pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	//Need to set DBPROP_UPDATABILITY: InsertRow is supported
	if (	!SupportedProperty(DBPROP_IRowsetChange, DBPROPSET_ROWSET)
		||	!SettableProperty(DBPROP_IRowsetChange, DBPROPSET_ROWSET))
	{
		fTestRes = TEST_SKIPPED;
		goto CLEANUP;
	}

	//Need to set DBPROP_UPDATABILITY: InsertRow is supported
	if (	!SupportedProperty(DBPROP_UPDATABILITY, DBPROPSET_ROWSET)
		||	!SettableProperty(DBPROP_UPDATABILITY, DBPROPSET_ROWSET))
	{
		fTestRes = TEST_SKIPPED;
		goto CLEANUP;
	}

	::SetProperty(	DBPROP_UPDATABILITY, DBPROPSET_ROWSET, &cPropSets, &rgPropSets, 
					DBTYPE_I4, (LPVOID)&lUpdateInsert);

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, IID_IRowsetChange, 
		cPropSets, rgPropSets, &pTableID, (IUnknown**)&pIRowsetChange), S_OK);
	fTestRes = TEST_PASS;
	
	// find out if one can write to that table
	pTable->SetTableID(*pTableID);
	pTable->MayWrite(&fMayWrite);
	for (i=0; i<nRows; i++)
	{
		m_hr = Insert(0, pIRowsetChange, 0, NULL, pTable);
		if (DB_E_NOTSUPPORTED != m_hr)
		{
			if (!CHECK(m_hr, S_OK))
				fTestRes = TEST_FAIL;
		}
		else
			fTestRes = TEST_SKIPPED;
	}

CLEANUP:
	SAFE_RELEASE(pIRowsetChange);
	if (pTableID)
		CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID);
	FreeProperties( &cPropSets, &rgPropSets);
	delete pTable;
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(55)
//*-----------------------------------------------------------------------
// @mfunc Delete rows in the created rowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_55()
{ 
	IRowsetChange	*pIRowsetChange = NULL;
	IRowset			*pIRowset = NULL;
	BOOL			fTestRes = TEST_FAIL;
	ULONG			i, nSuccess;
	const ULONG		nRows = 10;
	const int		nProps = 3;
	DBPROPSET		rgPropSets[1];
	DBPROP			rgProperties[nProps];
	HROW			*rghRow = NULL; 
	ULONG			cRowsObtained = 0;
	DBROWSTATUS		rgRowStatus[nRows];
	DBID			*pTableID = NULL;
	BOOL			fMayWrite;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CList <WCHAR*, WCHAR*>	ListNativeTemp;
	CList <DBTYPE, DBTYPE>	ListDataTypes;
	CTable			*pTable = NULL;

	pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	// get the provider types list
	pTable->CreateColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	// build the column description array
	pTable->BuildColumnDescs(&rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc=pTable->CountColumnsOnTable());
	pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProperties), rgProperties, DBPROPSET_ROWSET)
	FILL_PROP(rgProperties[0], DBPROP_IRowsetChange, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)
	FILL_PROP(rgProperties[1], DBPROP_OWNINSERT, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED)
	FILL_PROP(rgProperties[2], DBPROP_OWNUPDATEDELETE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, IID_IRowsetChange, 
		NUMELEM(rgPropSets), rgPropSets, &pTableID, (IUnknown**)&pIRowsetChange), S_OK);	

	pTable->SetTableID(*pTableID);
	pTable->MayWrite(&fMayWrite);
	for (i=0; i<nRows; i++)
	{
		m_hr = Insert(0, pIRowsetChange, 0, NULL, pTable);
		if (DB_E_NOTSUPPORTED == m_hr)
		{
			odtLog << "IRowsetChange->Insert() is not supported\n";
			fTestRes = TEST_SKIPPED;
			goto CLEANUP;
		}
		else
			TESTC_(m_hr, S_OK);
	}

	// get a pointer to IRoewset interface
	if (!VerifyInterface(pIRowsetChange, IID_IRowset, ROWSET_INTERFACE, (IUnknown**)&pIRowset))
	{
		odtLog << "could not get IRowsetChange\n";
		goto CLEANUP;
	}

	m_hr = pIRowset->RestartPosition(NULL);
	if (!CHECK(m_hr = pIRowset->GetNextRows(
				0, 0,					// hChapter & lRowsOffset
				nRows,					// number of rows to be retrieved
				&cRowsObtained,		//number of rows actually get
				&rghRow				// array of handlers to rows
				), S_OK) || nRows != cRowsObtained)
	{
			odtLog << "insuccess on IRowset::GetNextRows\n";
			goto CLEANUP;
	}

	TESTC_(m_hr = pIRowsetChange->DeleteRows(0, cRowsObtained, rghRow, rgRowStatus), S_OK);

	for (i=nSuccess=0; i<nRows; i++)
		if (DBROWSTATUS_S_OK == rgRowStatus[i])
			nSuccess ++;

	if (nSuccess != nRows)
		odtLog << "delete succeeded for " << nRows-nSuccess << " rows out of " << nRows << "\n";
	else
		fTestRes = TEST_PASS;

CLEANUP:
	if (cRowsObtained)
		CHECK(m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL), S_OK);
	PROVIDER_FREE(rghRow);
	SAFE_RELEASE(pIRowsetChange);
	SAFE_RELEASE(pIRowset);
	if (pTableID)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
		ReleaseDBID(pTableID);
	}
	delete pTable;
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(56)
//*-----------------------------------------------------------------------
// @mfunc Aggregation, ask for IID_IUnknown
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_56()
{
	TBEGIN
	HRESULT				hr;
	DBID				*pTableID		= NULL;
	CAggregate			Aggregate(m_pIOpenRowset);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TEST2C_(hr = CreateAndCheckTable(m_pITableDefinition, &Aggregate, NULL, cColumnDesc, rgColumnDesc, 
		IID_IUnknown, 0, NULL, &pTableID, (IUnknown**)&Aggregate.m_pIUnkInner), 
		S_OK, DB_E_NOAGGREGATION);

	TESTC_PROVIDER(S_OK == hr);
	// Verrify Aggregation for this rowset...
	TESTC(Aggregate.VerifyAggregationQI(hr, IID_IRowset));

	
CLEANUP:
	Aggregate.ReleaseInner();
	// checkings, etc
	CHECK(DropTableAndCheck(pTableID), pTableID? S_OK: E_INVALIDARG);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(57)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_COMMITPRESERVE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_57()
{ 
	TBEGIN
	DBPROPID	rgPropID=DBPROP_COMMITPRESERVE;

	COMPARE(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_TRUE, 
					(struct _GUID*)&IID_IRowsetInfo), TRUE);
	TESTC(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_FALSE, 
					(struct _GUID*)&IID_IRowsetInfo));
	
CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(58)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_OWNINSERT
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_58()
{ 
	TBEGIN
	DBPROPID	rgPropID=DBPROP_OWNINSERT;

	COMPARE(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_TRUE, 
					(struct _GUID*)&IID_IRowsetInfo), TRUE);
	TESTC(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_FALSE, 
					(struct _GUID*)&IID_IRowsetInfo));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(59)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_BOOKMARKS
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_59()
{ 
	TBEGIN
	DBPROPID	rgPropID=DBPROP_BOOKMARKS;

	COMPARE(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_TRUE, 
					(struct _GUID*)&IID_IRowsetInfo), TRUE);
	TESTC(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_FALSE, 
					(struct _GUID*)&IID_IRowsetInfo));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(60)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_CANHOLDROWS
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_60()
{ 
	TBEGIN
	DBPROPID	rgPropID=DBPROP_CANHOLDROWS;

	COMPARE(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_TRUE, 
					(struct _GUID*)&IID_IRowsetInfo), TRUE);
	TESTC(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_FALSE, 
					(struct _GUID*)&IID_IRowsetInfo));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(61)
//*-----------------------------------------------------------------------
// @mfunc Create a table and ask for a rowset, setting DBPROP_MAYWRITECOLUMN
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_61()
{ 
	TBEGIN
	DBPROPID	rgPropID=DBPROP_MAYWRITECOLUMN;

	COMPARE(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_TRUE, 
					(struct _GUID*)&IID_IRowsetInfo), TRUE);
	TESTC(CreateWithOneRowsetProps(rgPropID, VT_BOOL, (LPVOID)VARIANT_FALSE, 
					(struct _GUID*)&IID_IRowsetInfo));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(62)
//*-----------------------------------------------------------------------
// @mfunc Ask for a rowset mandatory/optional interface when table creation => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_62()
{ 
	BOOL					fTestRes = TEST_FAIL;
	IUnknown				*pIRowset = NULL;
	DBID					*pTableID = NULL;
	BOOL					fMayWrite;
	HRESULT					hr;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	CList <WCHAR*, WCHAR*>	ListNativeTemp;
	CList <DBTYPE, DBTYPE>	ListDataTypes;
	CTable					*pTable = NULL;

	pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	// get the provider types list
	pTable->CreateColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	// build the column description array
	pTable->BuildColumnDescs(&rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc=pTable->CountColumnsOnTable());
	pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);

	odtLog << "CreateTable and ask for a pointer to IRowset (mandatory)\n";
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					0, NULL), S_OK);

	odtLog << "CreateTable and ask for a pointer to IRowsetChange (optional)\n";
	CHECK(hr = CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, 
		IID_IRowsetChange, 0, NULL, &pTableID, (IUnknown**)&pIRowset), S_OK);

	// try to use the IRowsetChange interface to insert a row
	if (SUCCEEDED(hr))
	{
		pTable->SetTableID(*pTableID);
		pTable->MayWrite(&fMayWrite);
		m_hr = Insert(0, (IRowsetChange*)pIRowset, 0, NULL, pTable);
		if (DB_E_NOTSUPPORTED != m_hr)
		{
			TESTC_(m_hr, S_OK);
		}
		else
		{
			fTestRes = TEST_SKIPPED;
			goto CLEANUP;
		}
	}

	fTestRes = TEST_PASS;

CLEANUP:
	// checkings, etc
	SAFE_RELEASE(pIRowset);
	if (pTableID)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
		ReleaseDBID(pTableID);
	}
	delete pTable;
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END


unsigned TCCreateTable::MyThreadProc(ULONG i)
{
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	DBID			TableID;
	DBID			*pTableID		= NULL;
	HRESULT			hr;

	if (i>= nThreads)
		return 0;
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, m_cColumnDesc);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_rgName[i];
	Sleep(0); // switch the context

	hr = m_pITableDefinition->CreateTable(NULL, m_rgName[i]? &TableID: NULL, 
			m_cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL, 
			m_rgName[i] ? NULL: &pTableID, NULL);
	Sleep(0); // switch the context

	m_rgResult[i] = hr;
	if (!m_rgName[i] && pTableID && DBKIND_NAME == pTableID->eKind)
	{
		m_rgName[i] = pTableID->uName.pwszName;
		SAFE_FREE(pTableID);
	}
	else
		ReleaseDBID(pTableID);
	ReleaseColumnDesc(rgColumnDesc, m_cColumnDesc);
	return 1;
} //TCCreateTable::MyThreadProc


// {{ TCW_VAR_PROTOTYPE(63)
//*-----------------------------------------------------------------------
// @mfunc threads on different tables
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_63()
{
	TBEGIN
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;
	WCHAR			number[3];
	LPWSTR			pwszBaseName = NULL;
	DBID			TableID;

	odtLog << "All table names are generated by user\n";
	m_pTable->MakeTableName(NULL);
	pwszBaseName = m_pTable->GetTableName();
	for (i=0; i< nThreads; i++)
	{
		rgThreadParam[i].pObject = this;
		rgThreadParam[i].i		= i;

		m_rgResult[i] = E_FAIL;
		
		m_rgName[i] = (WCHAR*)PROVIDER_ALLOC((3+wcslen(pwszBaseName))*sizeof(WCHAR));		
		ASSERT(m_rgName[i]);
		wcscpy(m_rgName[i], pwszBaseName);
		swprintf(number, L"%02d", i);
		wcscat(m_rgName[i], number);
	}
	
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	TableID.eKind = DBKIND_NAME;
	for (i=0; i<nThreads; i++)
	{
		if (CHECK(m_rgResult[i], S_OK))
		{
			TableID.uName.pwszName = m_rgName[i];
			CHECK(DropTableAndCheck(&TableID), S_OK);
		}
	}

	odtLog << "All table names are provider generated\n";
	for (i=0; i< nThreads; i++)
	{
		rgThreadParam[i].pObject = this;
		rgThreadParam[i].i		= i;

		m_rgResult[i] = E_FAIL;		
		m_rgName[i] = NULL;
	}
	
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
		CHECK(m_rgResult[i], S_OK);

CLEANUP:
	TableID.eKind = DBKIND_NAME;
	for (i=0; i<nThreads; i++)
	{
		TableID.uName.pwszName = m_rgName[i];
		if (SUCCEEDED(m_rgResult[i]))
			CHECK(DropTableAndCheck(&TableID), S_OK);
		PROVIDER_FREE(m_rgName[i]);
	}

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(64)
//*-----------------------------------------------------------------------
// @mfunc threads on the same table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_64()
{ 
	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	BOOL			fTestResult = TEST_FAIL;
	ULONG			i, nS_OK, nDB_E_DUPLICATETABLEID;
	LPWSTR			pwszBaseName = NULL;
	DBID			TableID;

	m_pTable->MakeTableName(NULL);
	pwszBaseName = m_pTable->GetTableName();
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i]				= E_FAIL;
		m_rgName[i]					= pwszBaseName;
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// creates a table, build rgColumnDesc by it
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=nS_OK=nDB_E_DUPLICATETABLEID=0; i<nThreads; i++)
	{
		if (m_rgResult[i] == S_OK)
			nS_OK++;
		else if (CHECK(m_rgResult[i], DB_E_DUPLICATETABLEID))
			nDB_E_DUPLICATETABLEID++;
		else
		{
			odtLog << "Thread " << i << ": unexpected result " << m_rgResult[i] << "\n";
			goto CLEANUP;
		}
	}

	TESTC(nS_OK == 1 && nDB_E_DUPLICATETABLEID == nThreads-1);
	fTestResult = TEST_PASS;

CLEANUP:
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = pwszBaseName;
	CHECK(DropTableAndCheck(&TableID), S_OK);
	return fTestResult;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(65)
//*-----------------------------------------------------------------------
// @mfunc try all interfaces on rowset that can be returned
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_65()
{ 
	TBEGIN
	const ULONG				nInterfaces = 19;			// number of total interfaces
	const ULONG				nMandatoryInterfaces = 6;	// the number of mandatory interfaces
	const IID*				rgRIIDInterfaces[nInterfaces] = {&IID_IUnknown, &IID_IAccessor, &IID_IColumnsInfo, 
								&IID_IConvertType, &IID_IRowset, &IID_IRowsetInfo, &IID_IChapteredRowset, 
								&IID_IColumnsRowset, &IID_IConnectionPointContainer, &IID_IDBAsynchStatus, 
								&IID_IRowsetChange, &IID_IRowsetFind, &IID_IRowsetIdentity, &IID_IRowsetLocate, 
								&IID_IRowsetResynch, &IID_IRowsetScroll, &IID_IRowsetUpdate, &IID_IRowsetView, 
								&IID_ISupportErrorInfo};
	LPWSTR					rgInterfaceNames[nInterfaces] = {L"IUnknown", L"IAccessor", L"IColumnsInfo", 
								L"IConvertType", L"IRowset", L"IRowsetInfo", L"IChapteredRowset", 
								L"IColumnsRowset", L"IConnectionPointContainer", L"IDBAsynchStatus", 
								L"IRowsetChange", L"IRowsetFind", L"IRowsetIdentity", L"IRowsetLocate", 
								L"IRowsetResynch", L"IRowsetScroll", L"IRowsetUpdate", L"IRowsetView", 
								L"ISupportErrorInfo"};
	const DBPROPID			rgInterfaceID[nInterfaces-1] = {DBPROP_IAccessor, DBPROP_IColumnsInfo, 
								DBPROP_IConvertType, DBPROP_IRowset, DBPROP_IRowsetInfo, DBPROP_IChapteredRowset, 
								DBPROP_IColumnsRowset, DBPROP_IConnectionPointContainer, DBPROP_IDBAsynchStatus, 
								DBPROP_IRowsetChange, DBPROP_IRowsetFind, DBPROP_IRowsetIdentity, DBPROP_IRowsetLocate, 
								DBPROP_IRowsetResynch, DBPROP_IRowsetScroll, DBPROP_IRowsetUpdate, DBPROP_IRowsetView, 
								DBPROP_ISupportErrorInfo};
//	BOOL					fTestRes = TEST_PASS;
	ULONG					i, j;
	IUnknown				*pRowset = NULL;			
	IUnknown				*pIUnknown = NULL;
	DBPROPSET				rgPropertySets[2];
	DBPROP					rgProperties[nInterfaces-1];
	DBPROP					rgProp2[1];
	DBID					*pTableID = NULL;
	HRESULT					hr;
	HRESULT					hRes = S_OK;
	HRESULT					hExpectedRes = S_OK;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	BOOL					fRetrieve;
	BOOL					fPropSupported;

	FILL_PROP_SET(rgPropertySets[0], NUMELEM(rgProperties), rgProperties, DBPROPSET_ROWSET);
	for (i=0; i<nInterfaces-1; i++)
	{
		FILL_PROP(rgProperties[i], rgInterfaceID[i], VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);
		if (!SupportedProperty(rgInterfaceID[i], DBPROPSET_ROWSET))
			hRes = DB_S_ERRORSOCCURRED;
	}
	FILL_PROP_SET(rgPropertySets[1], NUMELEM(rgProp2), rgProp2, DBPROPSET_ROWSET);
	FILL_PROP(rgProp2[0], DBPROP_ISequentialStream, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);
	if (!SupportedProperty(DBPROP_ISequentialStream, DBPROPSET_ROWSET))
		hRes = DB_S_ERRORSOCCURRED;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	
	for (i=0; i<nInterfaces; i++)
	{
		odtLog << "\t" << rgInterfaceNames[i] << ((i<nMandatoryInterfaces)? "(mandatory)\n" : "\n");
		PRVTRACE(L"TCCreateTable::Variation_65(): iteration %d (%s)\n", 
			i, ((i>0) ? rgInterfaceNames[i] : L"IUnknown"));
		fPropSupported = (i>0)? SupportedProperty(rgInterfaceID[i-1], DBPROPSET_ROWSET): TRUE;
		hExpectedRes = ((i>0) && !fPropSupported)? E_NOINTERFACE: hRes;

		// if the interface is mandatory, it has to be supported
		// note that there is not a DBPROP_IUnknown, so the condition is i>nMandatoryInterfaces
		COMPARE(i==0 || fPropSupported || i>=nMandatoryInterfaces, TRUE);

		CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc,
			*rgRIIDInterfaces[i], 0, NULL, 
			FALSE), fPropSupported? S_OK: E_NOINTERFACE);

		pRowset = NULL;
		CHECK(hr = CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc,
			*rgRIIDInterfaces[i], NUMELEM(rgPropertySets), rgPropertySets, 
			&pTableID, &pRowset), hExpectedRes);

		if (S_OK != hr && DB_S_ERRORSOCCURRED != hr)
		{
			if (i<nMandatoryInterfaces)
			{
				odtLog << "could not get the mandatory interface " << rgInterfaceNames[i] << "\n";
				COMPARE(TRUE, FALSE);			
			}
		}
		else
		{
			// try to get all the other interfaces
			if (pRowset)
			{
				fRetrieve = CHECK(pRowset->QueryInterface(IID_IUnknown, 
						(void**)&pIUnknown), S_OK) ? fRetrieve: FALSE;
				SAFE_RELEASE(pIUnknown);
				
				// all the other interfaces considered
				for (j=1; j<nInterfaces; j++)
				{
					if (	(j<nMandatoryInterfaces) 
						&&	(	!COMPARE(rgProperties[j-1].dwStatus, DBPROPSTATUS_OK)
							||	!CHECK(pRowset->QueryInterface(*rgRIIDInterfaces[j], (void**)&pIUnknown), S_OK)))
					{
						odtLog << "Could not retrieve mandatory interface " << rgInterfaceNames[j] << "\n";
						continue;
					}
					SAFE_RELEASE(pIUnknown);
					fRetrieve = TRUE;
					// the interface should be retrievable if and only if the propstatus is ok 
					if (DBPROPSTATUS_OK == rgProperties[j-1].dwStatus)
					{
						fRetrieve = CHECK(pRowset->QueryInterface(*rgRIIDInterfaces[j], 
							(void**)&pIUnknown), S_OK) ? fRetrieve: FALSE;
						SAFE_RELEASE(pIUnknown);
					}
					else
					{
						fRetrieve = CHECK(pRowset->QueryInterface(*rgRIIDInterfaces[j], 
							(void**)&pIUnknown), E_NOINTERFACE) ? fRetrieve: FALSE;
					}
					// check on prop status
					if (SupportedProperty(rgInterfaceID[j-1], DBPROPSET_ROWSET))
					{
						if (SettableProperty(rgInterfaceID[j-1], DBPROPSET_ROWSET))
							fRetrieve = ((DBPROPSTATUS_NOTSET != rgProperties[j-1].dwStatus  
									&& !COMPARE(rgProperties[j-1].dwStatus, DBPROPSTATUS_OK))) ? FALSE: fRetrieve;
						else
							fRetrieve = (DBPROPSTATUS_OK != rgProperties[j-1].dwStatus
									&& !COMPARE(rgProperties[j-1].dwStatus, DBPROPSTATUS_NOTSETTABLE)) ? FALSE: fRetrieve;
					}
					else
					{
						fRetrieve = (!COMPARE(rgProperties[j-1].dwStatus, DBPROPSTATUS_NOTSUPPORTED))? FALSE: fRetrieve;
					}
					if (!fRetrieve)
					{
						odtLog << "ERROR: retrieving interface " << rgInterfaceNames[j] << "\n";
					}
				}
			}
		}
		if (pRowset)
			SAFE_RELEASE(pRowset);
		if (pTableID)
			CHECK(DropTableAndCheck(pTableID), S_OK);
		ReleaseDBID(pTableID);
	}

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(66)
//*-----------------------------------------------------------------------
// @mfunc create with a table property => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_66()
// just try the creation of a table with the specified property
// there is not any method to retrieve the property of the table
{ 
	TBEGIN
	DBPROPSET		rgPropSets[1];
	DBPROP			rgProp[1];
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBID			TableID;
	BOOL			fSupported;
	BOOL			fSettable;
	BOOL			fValidate;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= NULL;

	fSupported	= SupportedProperty(DBPROP_TBL_TEMPTABLE, DBPROPSET_TABLE);
	fSettable	= SettableProperty(DBPROP_TBL_TEMPTABLE, DBPROPSET_TABLE);
	fValidate	= fSettable || !fSupported;

	hr = fSettable? S_OK : DB_E_ERRORSOCCURRED;

	odtLog << "Property is REQUIRED\n";
	// temp table, TableID generated by provider
	odtLog << "\tProvider generated TableID\n";
	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, NUMELEM(rgPropSets), rgPropSets);
	if (fValidate && fSettable || S_OK == hr)
	{
		CHECK(hr, S_OK);
		COMPARE(rgProp[0].dwStatus, DBPROPSTATUS_OK);
	}
	else
	{
		CHECK(hr, DB_E_ERRORSOCCURRED);
		if (fSupported)
		{	COMPARE(DBPROPSTATUS_NOTSETTABLE == rgProp[0].dwStatus, TRUE);}
		else
		{	COMPARE(DBPROPSTATUS_NOTSUPPORTED == rgProp[0].dwStatus, TRUE);}
	}

	// perm table, TableID generated by provider
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
	hr = (S_OK!=hr)? DB_E_ERRORSOCCURRED : S_OK;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), hr);

	// temp table, consumer generated TableID 
	odtLog << "\tConsumer generated TableID\n";
	m_pTable->MakeTableName(NULL);
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = wcsDuplicate(m_pTable->GetTableName());
	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	hr = CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, NUMELEM(rgPropSets), rgPropSets);

	// perm table, consumer generated TableID 
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
	hr = (S_OK!=hr)? DB_E_ERRORSOCCURRED : S_OK;
	CHECK(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), hr);
	
	hr = fSettable? S_OK : DB_S_ERRORSOCCURRED;

	odtLog << "Property is OPTIONAL\n";
	// temp table, TableID generated by provider
	odtLog << "\tProvider generated TableID\n";
	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);
	TEST2C_(hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), S_OK, DB_S_ERRORSOCCURRED);

	// perm table, TableID generated by provider
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_FALSE, DBPROPOPTIONS_OPTIONAL);
	hr = (S_OK!=hr)? DB_S_ERRORSOCCURRED : S_OK;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), hr);

	// temp table, consumer generated TableID 
	odtLog << "\tConsumer generated TableID\n";
	m_pTable->MakeTableName(NULL);
	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = wcsDuplicate(m_pTable->GetTableName());
	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);
	TEST2C_(hr = CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), S_OK, DB_S_ERRORSOCCURRED);

	// perm table, consumer generated TableID 
	FILL_PROP(rgProp[0], DBPROP_TBL_TEMPTABLE, VT_BOOL, V_BOOL, VARIANT_FALSE, DBPROPOPTIONS_OPTIONAL);
	hr = (S_OK!=hr)? DB_S_ERRORSOCCURRED : S_OK;
	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), S_OK);

CLEANUP:
	// checkings, etc
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(67)
//*-----------------------------------------------------------------------
// @mfunc create a table and update a row, using the rowset interface got => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_67()
{ 
	IRowsetChange	*pIRowsetChange	= NULL;
	IRowset			*pIRowset		= NULL;
	IAccessor		*pIAccessor		= NULL;
	BOOL			fTestRes		= TEST_FAIL;
	ULONG			i;
	const ULONG		nInsertRows		= 10;
	BOOL			fMayWrite;
	const ULONG		nRows			= 3;
	DBPROPSET		rgPropSets[1];
	const ULONG		nProps			= 3;
	DBPROP			rgProperties[nProps];
	HROW			*rghRow			= NULL;
	ULONG			cRowsObtained	= 0;
	HACCESSOR		hAccessor		= NULL;
	BYTE			*pData			= NULL;
	ULONG			cBindings		= 0;
	DBBINDING		*rgBindings		= NULL;
	const LONG		cColsToBind		= 1;
	LONG			rgColsToBind[cColsToBind];
	DBID			*pTableID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CList <WCHAR*, WCHAR*>	ListNativeTemp;
	CList <DBTYPE, DBTYPE>	ListDataTypes;
	CTable			*pTable = NULL;

	pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	// get the provider types list
	pTable->CreateColInfo(ListNativeTemp, ListDataTypes, ALLTYPES);
	// build the column description array
	pTable->BuildColumnDescs(&rgColumnDesc);
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc=pTable->CountColumnsOnTable());
	pTable->SetColumnDesc(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProperties), rgProperties, DBPROPSET_ROWSET);
	FILL_PROP(rgProperties[0], DBPROP_IRowsetChange, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	FILL_PROP(rgProperties[1], DBPROP_OWNINSERT, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
	FILL_PROP(rgProperties[2], DBPROP_OWNUPDATEDELETE, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL);

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, IID_IRowsetChange, 1, rgPropSets,
		&pTableID, (IUnknown**)&pIRowsetChange), S_OK);

	pTable->SetTableID(*pTableID);
	pTable->MayWrite(&fMayWrite);
	for (i=0; i<nInsertRows; i++)
	{
		m_hr = Insert(0, pIRowsetChange, 0, NULL, pTable);
		if (DB_E_NOTSUPPORTED != m_hr)
		{
			TESTC_(m_hr, S_OK);
		}
		else
		{
			fTestRes = TEST_SKIPPED;
			goto CLEANUP;
		}

	}

	// get a pointer to IRoewset interface
	if (!VerifyInterface(pIRowsetChange, IID_IRowset, ROWSET_INTERFACE, (IUnknown**)&pIRowset))
	{
		odtLog << "could not get IRowsetChange\n";
		goto CLEANUP;
	}

	if (!VerifyInterface(pIRowset, IID_IAccessor, ROWSET_INTERFACE, (IUnknown**)&pIAccessor))
	{
		odtLog << "could not get IAccessor\n";
		goto CLEANUP;
	}

	m_hr = pIRowset->RestartPosition(NULL);

	if (!CHECK(m_hr = pIRowset->GetNextRows(0, 0,					// hChapter & lRowsOffset
											nRows,					// number of rows to be retrieved
											&cRowsObtained,		//number of rows actually get
											&rghRow				// array of handlers to rows
											), S_OK) || nRows != cRowsObtained)
	{
		odtLog << "insuccess on IRowset::GetNextRows\n";
		goto CLEANUP;
	}

	rgColsToBind[0] = 1;
	if (!CHECK(m_hr = GetAccessorAndBindings(pIRowset,	// pIUnknRowsetObject
					DBACCESSOR_ROWDATA,							// accessor flags
					&hAccessor, &rgBindings, &cBindings, NULL,	// accessor handler, array and number of bindings created
					DBPART_VALUE |DBPART_STATUS |DBPART_LENGTH,	// types of bindings to do
					USE_COLS_TO_BIND_ARRAY,						// eColsByRef specifies that cols to be bind 
																//are gien as an array
					FORWARD,									// eBindingOrder
					NO_COLS_BY_REF,								// eColsByRef
					NULL, NULL,
					NULL, DBTYPE_EMPTY, 						// ppStringsBufferOut & dwModifier
					cColsToBind, rgColsToBind), S_OK))
	{
		odtLog << "problems getting accessor and bindings\n";
		goto CLEANUP;
	}

	// Fill buffer with appropriate data for insert of this row number
	TESTC_(m_hr = FillInputBindings(
		pTable,
		DBACCESSOR_ROWDATA,		
		cBindings,
		rgBindings,	
		&pData,		
		37,						
		0,
		NULL),S_OK);

	if (!CHECK(m_hr = pIRowsetChange->SetData(rghRow[0], hAccessor, pData), S_OK))
	{
		odtLog << "could not update the row\n";
		goto CLEANUP;
	}
	fTestRes = TEST_PASS;

CLEANUP:
	if (pIAccessor && hAccessor)
	{
		CHECK(m_hr = pIAccessor->ReleaseAccessor(hAccessor, NULL), S_OK);
	}
	SAFE_RELEASE(pIAccessor);
	if (pIRowset)
		CHECK(m_hr = pIRowset->ReleaseRows(cRowsObtained, rghRow, NULL, NULL, NULL), S_OK);
	PROVIDER_FREE(rghRow);
	SAFE_RELEASE(pIRowsetChange);
	SAFE_RELEASE(pIRowset);
	if (pTableID)
	{
		CHECK(DropTableAndCheck(pTableID), S_OK);
		ReleaseDBID(pTableID);
	}
	delete pTable;
	FreeAccessorBindings(cBindings, rgBindings);
	PROVIDER_FREE(pData);
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(68)
//*-----------------------------------------------------------------------
// @mfunc Fully Qualified Table Name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_68()
{ 
	BOOL			fTestRes = TEST_FAIL;
	DBID			TableID;
	WCHAR			*pwszQualTableName = NULL;
	WCHAR			*pwszCatalogName = NULL;
	WCHAR			*pwszSchemaName = NULL;
	WCHAR			*pwszTableName = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CTable			*pTable = NULL;

	// create a regular table
	pTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	TESTC(NULL != pTable);
	TESTC_(pTable->CreateTable(0, 0), S_OK);
	pwszTableName = pTable->GetTableName();
	TableID.eKind = DBKIND_NAME;

	TESTC(GetCatalogSchemaNames(pwszTableName, &pwszCatalogName, &pwszSchemaName, pTable));

	//Construct a fully Qualified TableName...
	TESTC_(pTable->GetQualifiedName(pwszCatalogName, pwszSchemaName, pwszTableName, &pwszQualTableName),S_OK);
	TableID.uName.pwszName = pwszQualTableName;

	// get rid of the previous table; keep info about "the empty shell"
	m_pITableDefinition->DropTable(&pTable->GetTableID());

	// create the table with the qualified name
	pTable->BuildColumnDescs(&rgColumnDesc);
	pTable->SetColumnDesc(rgColumnDesc, cColumnDesc=pTable->CountColumnsOnTable());

	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);
	fTestRes = TEST_PASS;
    
CLEANUP:
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
	pTable->DropTable();
	delete pTable;
    return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(69)
//*-----------------------------------------------------------------------
// @mfunc Quoted table name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_69()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	TableID.eKind = DBKIND_NAME;
	TableID.uName.pwszName = NULL;
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	m_pTable->MakeTableName(NULL);
	m_pTable->GetQuotedName(m_pTable->GetTableName(), &TableID.uName.pwszName);
	TESTC_(CCNDropTable(&TableID, cColumnDesc, rgColumnDesc, IID_IRowset, 0, NULL), S_OK);

CLEANUP:
	ReleaseDBID(&TableID, FALSE);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(70)
//*-----------------------------------------------------------------------
// @mfunc Aggregation, non IID_IUnknown
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_70()
{ 
	TBEGIN
	DBID				*pTableID		= NULL;
	CAggregate			Aggregate(m_pIOpenRowset);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	TESTC_(CreateAndCheckTable(m_pITableDefinition, &Aggregate, NULL, cColumnDesc, rgColumnDesc, 
		IID_IRowsetChange, 0, NULL, &pTableID, (IUnknown**)&Aggregate.m_pIUnkInner), 
		DB_E_NOAGGREGATION);

	// Verrify Aggregation for this rowset...
	TESTC(NULL == Aggregate.m_pIUnkInner);

CLEANUP:	
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	Aggregate.ReleaseInner();
	// checkings, etc
	DropTableAndCheck(pTableID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(71)
//*-----------------------------------------------------------------------
// @mfunc Aggregation -> IRowsetInfo::GetReferencedRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_71()
{ 
	TBEGIN
	HRESULT				hr;
	DBID				*pTableID		= NULL;
	CAggregate			Aggregate(m_pIOpenRowset);
    IRowsetInfo			*pIRowsetInfo	= NULL;
	IUnknown			*pIAggregate	= NULL;
	IUnknown			*pIUnkOuter		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	ULONG				cPropSet		= 0;
	DBPROPSET			*prgPropSet		= NULL;
	DBPROPSET			rgPropSet[1];
	DBPROP				rgProp[1];

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	// create a table and open a bookmark rowset on it
	if (SettableProperty(DBPROP_BOOKMARKS, DBPROPSET_ROWSET))
	{
		FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_ROWSET);
		FILL_PROP(rgProp[0], DBPROP_BOOKMARKS, VT_BOOL, V_BOOL, VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);
		prgPropSet	= rgPropSet;
		cPropSet	= 1;
	}
	TEST2C_(hr = CreateAndCheckTable(m_pITableDefinition, &Aggregate, NULL, cColumnDesc, rgColumnDesc, IID_IUnknown, cPropSet, prgPropSet,
		&pTableID, (IUnknown**)&Aggregate.m_pIUnkInner), S_OK, DB_E_NOAGGREGATION);
	TEST_PROVIDER(S_OK == hr);

	//Verify Aggregation for this rowset...
	TESTC(Aggregate.VerifyAggregationQI(hr, IID_IRowsetInfo, (IUnknown**)&pIRowsetInfo));

	//Verify we are hooked up...
	//This call we are using the Rowset and asking for IID_IAggregate, 
	//which is the outer object and should succeed!!!  Kind of cool huh!
	hr = pIRowsetInfo->GetReferencedRowset(0, IID_IAggregate, (IUnknown**)&pIAggregate);
	TESTC(hr==S_OK || hr==DB_E_BADORDINAL ||hr==DB_E_NOTAREFERENCECOLUMN);
	if(hr==DB_E_NOTAREFERENCECOLUMN || hr==DB_E_BADORDINAL)
		TESTC(!GetProperty(DBPROP_BOOKMARKS, DBPROPSET_ROWSET, Aggregate.m_pIUnkInner, VARIANT_TRUE));
	TESTC_PROVIDER(hr==S_OK);	

	//Now make sure the Rowset QI for IUnknown give me the outer
	TESTC_(hr = pIRowsetInfo->GetReferencedRowset(0, IID_IUnknown, (IUnknown**)&pIUnkOuter),S_OK);
	TESTC(VerifyEqualInterface(pIAggregate, pIUnkOuter));

CLEANUP:
	SAFE_RELEASE(pIAggregate);
	SAFE_RELEASE(pIRowsetInfo);
	SAFE_RELEASE(pIUnkOuter);
	Aggregate.ReleaseInner();
	// checkings, etc
	CHECK(DropTableAndCheck(pTableID), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(72)
//*-----------------------------------------------------------------------
// @mfunc Agg session -> IRowsetInfo::GetSpecification
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_72()
{ 
    CAggregate			Aggregate(g_pIDBCreateSession);
	ULONG				fTestRes			= TEST_FAIL;
    IRowsetInfo			*pIRowsetInfo		= NULL;
    ITableDefinition	*pITableDefinition	= NULL;
	IUnknown			*pIAggregate		= NULL;
	ULONG				ulRefCountBefore;
	ULONG				ulRefCountAfter;
	DBCOLUMNDESC		*rgColumnDesc		= NULL;
	ULONG				cColumnDesc			= 0;
	DBID				*pTableID			= NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	//Aggregation
	HRESULT hr = CreateNewSession(NULL, IID_IUnknown, (IUnknown**)&Aggregate.m_pIUnkInner, &Aggregate);

	TEST_PROVIDER(S_OK == hr);

	//Verify Aggregation for this session...
	TESTC(Aggregate.VerifyAggregationQI(hr, IID_ITableDefinition, (IUnknown**)&pITableDefinition));
	
	//Now Create the Rowset
	ulRefCountBefore = Aggregate.GetRefCount();
	// create a table and open a bookmark rowset on it
	TESTC_(CreateAndCheckTable(pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, 
		IID_IRowsetInfo, 0, NULL, &pTableID, (IUnknown**)&pIRowsetInfo), S_OK);
	ulRefCountAfter = Aggregate.GetRefCount();

	//GetSpecification
	TEST2C_(hr = pIRowsetInfo->GetSpecification(IID_IAggregate, (IUnknown**)&pIAggregate),S_OK,S_FALSE);

	if(hr==S_OK)
	{
		TESTC(VerifyEqualInterface(pIAggregate, pITableDefinition));

		//Verify the child correctly addref'd the parent outer.
		//The is an absolute requirement that the child keep the parent outer alive.
		//If it doesn't addref the outer, the outer can be released externally since
		//its not being used anymore due to the fact the outer controls the refcount
		//of the inner.  Many providers incorrectly addref the inner, which does nothing
		//but guareentee the inner survives, but the inner will delegate to the outer
		//and crash since it no longer exists...
		TCOMPARE_(ulRefCountAfter > ulRefCountBefore);
	}
	else
	{
		TWARNING(L"IRowsetInfo::GetSpecification unable to retrieve Parent object!");
	}

	fTestRes = TEST_PASS;

CLEANUP:
	SAFE_RELEASE(pIAggregate);
	SAFE_RELEASE(pIRowsetInfo);
	Aggregate.ReleaseInner();
	// checkings, etc
	if (pITableDefinition)
		CHECK(DropTableAndCheck(pTableID, pITableDefinition), S_OK);
	SAFE_RELEASE(pITableDefinition);
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(73)
//*-----------------------------------------------------------------------
// @mfunc Table -> Agg ColumnRowset, ask for non IID_IUnknown
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_73()
{ 
	TBEGIN
	DBID				*pTableID			= NULL;
	CAggregate			Aggregate(m_pIOpenRowset);
	IRowset				*pIRowset			= NULL;
	IColumnsRowset		*pIColumnsRowset	= NULL;
	DBCOLUMNDESC		*rgColumnDesc		= NULL;
	ULONG				cColumnDesc			= 0;
	DBPROPSET			*rgPropSet			= NULL;
	ULONG				cPropSet			= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	SetSettableProperty(DBPROP_IColumnsRowset, DBPROPSET_ROWSET, &cPropSet, &rgPropSet);
	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, 
		IID_IRowset, cPropSet, rgPropSet, &pTableID, (IUnknown**)&pIRowset), S_OK);

	//Obtain the ColumnsRowset interface [OPTIONAL] interface
	TESTC_PROVIDER(VerifyInterface(pIRowset, IID_IColumnsRowset, ROWSET_INTERFACE, (IUnknown**)&pIColumnsRowset));

	//Try to obtain anything but IID_IUnknown.  
	//This should fail, this is a requirement for COM Aggregation...
	TESTC_(pIColumnsRowset->GetColumnsRowset(&Aggregate, 0, NULL, IID_IRowset, 0, NULL, 
		(IUnknown**)&Aggregate.m_pIUnkInner),DB_E_NOAGGREGATION);

	//Inner object cannot RefCount the outer object - COM rule for CircularRef
	TESTC(Aggregate.GetRefCount() == 1);
	TESTC(Aggregate.m_pIUnkInner == NULL);
	

CLEANUP:
	SAFE_RELEASE(pIColumnsRowset);
	SAFE_RELEASE(pIRowset);	
	Aggregate.ReleaseInner();
	DropTableAndCheck(pTableID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(74)
//*-----------------------------------------------------------------------
// @mfunc Table -> Agg ColumnRowset, ask for IID_IUnknown
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_74()
{ 
	TBEGIN
	HRESULT				hr;
	DBID				*pTableID			= NULL;
	CAggregate			Aggregate(m_pIOpenRowset);
	IRowset				*pIRowset			= NULL;
	IColumnsRowset		*pIColumnsRowset	= NULL;
	DBCOLUMNDESC		*rgColumnDesc		= NULL;
	ULONG				cColumnDesc			= 0;
	DBPROPSET			*rgPropSet			= NULL;
	ULONG				cPropSet			= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	SetSettableProperty(DBPROP_IColumnsRowset, DBPROPSET_ROWSET, &cPropSet, &rgPropSet);
	TEST2C_(hr = CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, 
		IID_IRowset, cPropSet, rgPropSet, &pTableID, (IUnknown**)&pIRowset), S_OK, DB_E_ERRORSOCCURRED);

	if (DB_E_ERRORSOCCURRED == hr)
		TESTC(!GetProperty(DBPROP_IColumnsRowset, DBPROPSET_ROWSET, pIRowset, VARIANT_TRUE));
	TESTC_PROVIDER(S_OK == hr);

	//Obtain the ColumnsRowset interface [OPTIONAL] interface
	TESTC_PROVIDER(VerifyInterface(pIRowset, IID_IColumnsRowset, ROWSET_INTERFACE, (IUnknown**)&pIColumnsRowset));

	//Try to obtain IID_IUnknown.  
	TEST2C_(hr = pIColumnsRowset->GetColumnsRowset(&Aggregate, 0, NULL, IID_IUnknown, 0, NULL, 
		(IUnknown**)&Aggregate.m_pIUnkInner), S_OK, DB_E_NOAGGREGATION);
	TESTC_PROVIDER(S_OK == hr);	
	//Verify Aggregation for this session...
	TESTC(Aggregate.VerifyAggregationQI(hr, IID_IRowset));

CLEANUP:
	SAFE_RELEASE(pIColumnsRowset);
	SAFE_RELEASE(pIRowset);	
	Aggregate.ReleaseInner();
	// checkings, etc
	DropTableAndCheck(pTableID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(75)
//*-----------------------------------------------------------------------
// @mfunc Table -> Agg ColumnRowset -> IRowsetInfo::GetSpecification
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_75()
{ 
	TBEGIN
    CAggregate		Aggregate(m_pIOpenRowset);
	ULONG			fTestRes			= TEST_FAIL;
    IRowset			*pIRowset			= NULL;
    IColumnsRowset	*pIColumnsRowset	= NULL;
	IUnknown		*pIAggregate		= NULL;
	IRowsetInfo		*pIRowsetInfo		= NULL;
	ULONG			ulRefCountBefore;
	ULONG			ulRefCountAfter;
	HRESULT			hr					= S_OK;
	DBCOLUMNDESC	*rgColumnDesc		= NULL;
	ULONG			cColumnDesc			= 0;
	DBPROPSET		*rgPropSet			= NULL;
	ULONG			cPropSet			= 0;
	DBID			*pTableID			= NULL;
	BOOL			fCreatedTable		= FALSE;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	SetSettableProperty(DBPROP_IColumnsRowset, DBPROPSET_ROWSET, &cPropSet, &rgPropSet);
	TEST3C_(hr = CreateAndCheckTable(m_pITableDefinition, &Aggregate, NULL, cColumnDesc, rgColumnDesc, 
		IID_IUnknown, cPropSet, rgPropSet, &pTableID, (IUnknown**)&Aggregate.m_pIUnkInner),
		S_OK, DB_E_ERRORSOCCURRED, DB_E_NOAGGREGATION);

	if (DB_E_ERRORSOCCURRED == hr)
		TESTC(!GetProperty(DBPROP_IColumnsRowset, DBPROPSET_ROWSET, pIRowset, VARIANT_TRUE));

	TESTC_PROVIDER(S_OK == hr);
	fCreatedTable = TRUE;

	//Verify Aggregation
	TESTC(Aggregate.VerifyAggregationQI(hr, IID_IRowset, (IUnknown**)&pIRowset));

	//Obtain the ColumnsRowset interface [OPTIONAL] interface
	TESTC_PROVIDER(VerifyInterface(pIRowset, IID_IColumnsRowset, ROWSET_INTERFACE, (IUnknown**)&pIColumnsRowset));

	//IColumnsRowset::GetRowset
	ulRefCountBefore = Aggregate.GetRefCount();
	TESTC_(hr = pIColumnsRowset->GetColumnsRowset(NULL, 0, NULL, IID_IRowsetInfo, 0, NULL, (IUnknown**)&pIRowsetInfo),S_OK);
	ulRefCountAfter = Aggregate.GetRefCount();

	//GetSpecification
	TEST2C_(hr = pIRowsetInfo->GetSpecification(IID_IAggregate, (IUnknown**)&pIAggregate),S_OK,S_FALSE);

	if(hr==S_OK)
	{
		TESTC(VerifyEqualInterface(pIAggregate, pIRowset));

		//Verify the child correctly addref'd the parent outer.
		//The is an absolute requirement that the child keep the parent outer alive.
		//If it doesn't addref the outer, the outer can be released externally since
		//its not being used anymore due to the fact the outer controls the refcount
		//of the inner.  Many providers incorrectly addref the inner, which does nothing
		//but guareentee the inner survives, but the inner will delegate to the outer
		//and crash since it no longer exists...
		TCOMPARE_(ulRefCountAfter > ulRefCountBefore);
	}
	else
	{
		TWARNING(L"IRowsetInfo::GetSpecification unable to retrieve Parent object!");
	}

CLEANUP:
	SAFE_RELEASE(pIRowsetInfo);
	SAFE_RELEASE(pIAggregate);
	SAFE_RELEASE(pIColumnsRowset);
	SAFE_RELEASE(pIRowset);
	Aggregate.ReleaseInner();
	// checkings, etc
	CHECK(DropTableAndCheck(pTableID), fCreatedTable? S_OK : E_INVALIDARG);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(76)
//*-----------------------------------------------------------------------
// @mfunc colid is ignored in a non column specific rowset property
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_76()
{ 
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBPROPID		dwPropID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	HRESULT			hr;
	DBPROPSTATUS	dwStatus;

	// make sure there is a non column specific rowset property that is settable
	TESTC_PROVIDER(GetNonColSpecProp(DBPROPSET_ROWSET, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].cPropertySets	= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;

	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], dwPropID, DBTYPE_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	// get the result with DB_NULLID
	hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet);

	dwStatus = rgProp[0].dwStatus;
	if (S_OK != hr)
	{
		TESTC_(hr, DB_S_ERRORSOCCURRED);
		TESTC(DBPROPSTATUS_NOTSUPPORTED == dwStatus);
	}
	else
	{
		TESTC(DBPROPSTATUS_OK == dwStatus);
	}

	// set colID to a value different from DB_NULLID	
	rgProp[0].colid.eKind			= DBKIND_NAME;
	rgProp[0].colid.uName.pwszName	= L"someColumn";

	// check colid is ignored for the rowset property
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet), hr);
	TESTC(dwStatus == rgProp[0].dwStatus);

	// check colid is ignored for the column property
	rgProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
		1, rgPropSet), S_OK == hr? S_OK: DB_E_ERRORSOCCURRED);
	TESTC(dwStatus == rgProp[0].dwStatus);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(77)
//*-----------------------------------------------------------------------
// @mfunc colid is DB_NULLID in a column specific rowset property => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_77()
{ 
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBPROPID		dwPropID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	HRESULT			hr;

	// make sure there is a column specific rowset property that is settable
	TESTC_PROVIDER(GetColSpecProp(DBPROPSET_ROWSET, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].cPropertySets	= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;

	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], dwPropID, DBTYPE_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	// explicitly set colID to DB_NULLID	
	rgProp[0].colid = DB_NULLID;

	// check colid is ignored for the rowset property
	hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet);
	if (DB_S_ERRORSOCCURRED == hr)
	{
		TESTC(DBPROPSTATUS_NOTALLSETTABLE == rgProp[0].dwStatus);
	}
	else
	{
		TESTC_(hr, S_OK);
		TESTC(DBPROPSTATUS_OK == rgProp[0].dwStatus);
	}

	// check colid is ignored for the column property
	rgProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	// check colid is ignored for the rowset property
	hr = CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet);
	if (DB_E_ERRORSOCCURRED == hr)
	{
		TESTC(DBPROPSTATUS_NOTALLSETTABLE == rgProp[0].dwStatus);
	}
	else
	{
		TESTC_(hr, S_OK);
		TESTC(DBPROPSTATUS_OK == rgProp[0].dwStatus);
	}


CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(78)
//*-----------------------------------------------------------------------
// @mfunc valid, non DB_NULLID colid for a column specific rowset property
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_78()
{ 
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBPROPID		dwPropID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	memset(&rgProp[0].colid, 0, sizeof(DBID));

	// make sure there is a column specific rowset property that is settable
	TESTC_PROVIDER(GetColSpecProp(DBPROPSET_ROWSET, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].cPropertySets	= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;

	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], dwPropID, DBTYPE_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	// explicitly set colID to DB_NULLID	
	DuplicateDBID(m_rgColumnDesc[0].dbcid, &rgProp[0].colid);

	// check colid is ignored for the rowset property
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet), S_OK);
	COMPARE(DBPROPSTATUS_OK == rgProp[0].dwStatus, TRUE);

	// check colid is ignored for the column property
	rgProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	// check colid is ignored for the rowset property
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet), S_OK);
	COMPARE(DBPROPSTATUS_OK == rgProp[0].dwStatus, TRUE);

CLEANUP:
	ReleaseDBID(&rgProp[0].colid, FALSE);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(79)
//*-----------------------------------------------------------------------
// @mfunc invalid colid for a column specific rowset property
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_79()
{ 
	TBEGIN
	DBPROPSET		rgPropSet[1];
	DBPROP			rgProp[1];
	DBPROPID		dwPropID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	// make sure there is a column specific rowset property that is settable
	TESTC_PROVIDER(GetColSpecProp(DBPROPSET_ROWSET, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	rgColumnDesc[0].cPropertySets	= 0;
	rgColumnDesc[0].rgPropertySets	= NULL;

	FILL_PROP_SET(rgPropSet[0], 1, rgProp, DBPROPSET_ROWSET)
	FILL_PROP(rgProp[0], dwPropID, DBTYPE_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_OPTIONAL)

	// set colID to a value different from DB_NULLID	
	rgProp[0].colid.eKind			= DBKIND_NAME;
	rgProp[0].colid.uName.pwszName	= L"someColumn";

	// check colid is ignored for the rowset property
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet), DB_S_ERRORSOCCURRED);
	COMPARE(DBPROPSTATUS_BADCOLUMN == rgProp[0].dwStatus, TRUE);

	// check colid is ignored for the column property
	rgProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 1, rgPropSet), DB_E_ERRORSOCCURRED);
	COMPARE(DBPROPSTATUS_BADCOLUMN == rgProp[0].dwStatus, TRUE);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(80)
//*-----------------------------------------------------------------------
// @mfunc Create a table. Retrieve a rowset on it (IRowsetInfo). Check GetSpecification()
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_80()
{ 
	IRowsetInfo				*pIRowsetInfo	= NULL;
	IOpenRowset				*pIOpenRowset	= NULL;
	BOOL					fTestRes		= TEST_FAIL;
	DBID					*pTableID		= NULL;
	DBCOLUMNDESC			*rgColumnDesc	= NULL;
	ULONG					cColumnDesc		= 0;
	HRESULT					hr;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	TESTC_(CreateAndCheckTable(m_pITableDefinition, NULL, NULL, cColumnDesc, rgColumnDesc, IID_IRowsetInfo, 
		0, NULL, &pTableID, (IUnknown**)&pIRowsetInfo), S_OK);

	hr = pIRowsetInfo->GetSpecification(IID_IOpenRowset, (IUnknown**)&pIOpenRowset);
	if (S_FALSE != hr)
	{
		TESTC_(hr, S_OK);
		TESTC(VerifyEqualInterface(m_pITableDefinition, pIOpenRowset));
	}

	fTestRes = TEST_PASS;
	
CLEANUP:
	SAFE_RELEASE(pIRowsetInfo);
	SAFE_RELEASE(pIOpenRowset);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	if (pTableID)
		CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID);
	return fTestRes;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(81)
//*-----------------------------------------------------------------------
// @mfunc DB_NULLID is ignored for non column-related table properties
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_81()
{ 
	TBEGIN
	DBPROPSET		rgPropSets[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPID		dwPropID;

	// make sure there is a non column specific rowset property that is settable
	TESTC_PROVIDER(GetNonColSpecProp(DBPROPSET_TABLE, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], dwPropID, VT_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	// setting a colid != DB_NULLID for a table property should result in error
	rgProp[0].colid.eKind		= DBKIND_NAME;
	rgProp[0].colid.uName.pwszName	= L"AliBaba";
	
	CHECK(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), S_OK);

	// TableID generated by provider
	FILL_PROP(rgProp[0], dwPropID, VT_BOOL, V_BOOL, 
		VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), S_OK);
	
CLEANUP:
	// checkings, etc
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(82)
//*-----------------------------------------------------------------------
// @mfunc Column related table property set to bad colid
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_82()
{ 
	TBEGIN
	DBPROPSET		rgPropSets[1];
	DBPROP			rgProp[1];
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	DBPROPID		dwPropID;

	// make sure there is a non column specific rowset property that is settable
	TESTC_PROVIDER(GetColSpecProp(DBPROPSET_TABLE, &dwPropID));

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);

	FILL_PROP_SET(rgPropSets[0], NUMELEM(rgProp), rgProp, DBPROPSET_TABLE);
	FILL_PROP(rgProp[0], dwPropID, VT_BOOL, V_BOOL, 
		VARIANT_TRUE, DBPROPOPTIONS_REQUIRED);

	// setting a colid != DB_NULLID for a table property should result in error
	rgProp[0].colid.eKind		= DBKIND_NAME;
	rgProp[0].colid.uName.pwszName	= L"AliBaba";
	
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), DB_S_ERRORSOCCURRED);

	// TableID generated by provider
	FILL_PROP(rgProp[0], dwPropID, VT_BOOL, V_BOOL, 
		VARIANT_FALSE, DBPROPOPTIONS_REQUIRED);
	TESTC_(CCNDropTable(NULL, cColumnDesc, rgColumnDesc, IID_IRowset, 
					NUMELEM(rgPropSets), rgPropSets), DB_E_ERRORSOCCURRED);
	
CLEANUP:
	// checkings, etc
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(83)
//*-----------------------------------------------------------------------
// @mfunc How many DBTYPE_I4 columns?
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTable::Variation_83()
{ 
	return TEST_PASS;
	/*
	TBEGIN
	CTable			*pCTable		= new CTable(m_pThisTestModule->m_pIUnknown2, 
										(LPWSTR)gwszModuleName);
	DBTYPE			dbType			= DBTYPE_I4;
	BYTE			precision		= 0;
	BYTE			scale			= 0;
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	const ULONG		MAX_VAL			= 2048;
	ULONG			cColumnDesc		= MAX_VAL / 2;
	ULONG			iCol;
	ULONG			offset			= MAX_VAL / 4;

	pCTable->SetBuildColumnDesc(FALSE);
	for (;0 < offset;)
	{
		SAFE_ALLOC(rgColumnDesc, DBCOLUMNDESC, cColumnDesc);
		memset(rgColumnDesc, 0, cColumnDesc*sizeof(DBCOLUMNDESC));
		for (iCol = 0; iCol < cColumnDesc; iCol++)
		{
			// fill in a column
			rgColumnDesc[iCol].wType					= dbType;
			rgColumnDesc[iCol].bPrecision				= precision;
			rgColumnDesc[iCol].bScale					= scale;
			rgColumnDesc[iCol].dbcid.eKind				= DBKIND_NAME;
			SAFE_ALLOC(rgColumnDesc[iCol].dbcid.uName.pwszName, WCHAR, 10);
			swprintf(rgColumnDesc[iCol].dbcid.uName.pwszName, L"C%dCol", iCol);
		}

		pCTable->SetColumnDesc(rgColumnDesc, cColumnDesc);
		hr = pCTable->CreateTable(100, 0);
		if (SUCCEEDED(hr))
		{
			pCTable->DropTable();
			if (cColumnDesc < MAX_VAL)
				cColumnDesc += offset;
			else
				break;
		}
		else
		{
			if (cColumnDesc < offset)
				break;
			cColumnDesc -= offset;
		}
		offset /= 2;
	}
	odtLog << "Max no of columns : " << cColumnDesc << "\n";

CLEANUP:
	pCTable->DropTable();

	SAFE_DELETE(pCTable);
	TRETURN
	*/
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//--------------------------------------------------------------------
// @mfunc TestCase Termination Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCreateTable::Terminate()
{
	// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
}	// }}
// }}
// }}



// {{ TCW_TC_PROTOTYPE(RODataSource)
//*-----------------------------------------------------------------------
//| Test Case:		RODataSource - Test case for operating on a read only data source
//| Created:  	10/13/97
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL RODataSource::Init()
{
	BOOL			fResults		= TEST_SKIPPED;
	IDBInitialize	*pIDBInitialize	= NULL;
	IDBProperties	*pIDBProperties	= NULL;
	DBPROPSET		*rgPropSets		= NULL;
	ULONG			cPropSets		= 0;
	HRESULT			hr				= NOERROR;
	DBPROP			*rgProperties	= NULL;
	const int		n=1;
	const ULONG		nINIT_MODE = 0;
	DBPROPIDSET		rgPropIDSet[n];
	DBPROPID		rgPropID1[1];
	ULONG			nActualSize		= n;

	m_pIUnknown				= NULL;
	m_pIUnknown2			= NULL;
	m_pITableDefinition2	= NULL;
	m_pTable				= NULL; 

	// {{ TCW_INIT_BASECLASS_CHECK
	if(!TCDropColumn::Init())
	// }}
		goto CLEANUP2;

	if (!SettableProperty(DBPROP_INIT_MODE, DBPROPSET_DBINIT))
	{
		odtLog << "DBPROP_INIT_MODE is not supported or is not a read-write property\n";
		fResults = TEST_SKIPPED;
		goto CLEANUP2;
	}

	// create a spare table on the read-write datasource
	m_hr = m_pTable->CreateTable(10, 0);

	m_pITableDefinition2	= m_pITableDefinition;
	m_pITableDefinition		= NULL;
	fResults = FALSE;

	if (!CHECK(m_hr, S_OK)) 
	{
		odtLog << "could not create atable on the read-write data source!\n";
		goto CLEANUP;
	}

	// Setup the arrays needed for init, based on string LTM passed to us
	if(!GetInitProps(&cPropSets, &rgPropSets))
		goto CLEANUP;


	// Get our initial connection to the provider, asking for IDBInitialize since 
	// we must initialize before anything else
	if(!SUCCEEDED(m_hr = GetModInfo()->CreateProvider(NULL, IID_IDBInitialize, (IUnknown**)&pIDBInitialize)))
		goto CLEANUP;

	// Get IDBProperties Pointer
	pIDBProperties = NULL;
	if (!VerifyInterface(pIDBInitialize, IID_IDBProperties, DATASOURCE_INTERFACE, (IUnknown**)&pIDBProperties))
		goto CLEANUP;

	// Set the properties before we Initialize, only if we have Properties to set...
	if (FAILED(m_hr = pIDBProperties->SetProperties(cPropSets, rgPropSets)))
		goto CLEANUP;

	// free the structures
	FreeProperties(&cPropSets, &rgPropSets);
	rgPropSets	= NULL;
	cPropSets	= 0;
	SetProperty(&rgPropSets, &cPropSets, DBPROP_INIT_MODE, VT_I4, (LPVOID)DB_MODE_READ, DBPROPOPTIONS_REQUIRED, DB_NULLID, 
			DBPROPSET_DBINIT);
	// Set the properties before we Initialize, only if we have Properties to set...
	if (FAILED(m_hr = pIDBProperties->SetProperties(cPropSets, rgPropSets)))
	{
		FindProperty(
			DBPROP_INIT_MODE, 
			DBPROPSET_DBINIT, 
			cPropSets, 
			rgPropSets, 
			&rgProperties);
		if (DBPROPSTATUS_OK != rgProperties->dwStatus)
			odtLog << "Read only property could not be initialized!\n";
		goto CLEANUP1;
	}
	else
	{
		PRVTRACE("RODataSource::Init(): Apparently the read only property was set\n");
		FindProperty(
			DBPROP_INIT_MODE, 
			DBPROPSET_DBINIT, 
			cPropSets, 
			rgPropSets, 
			&rgProperties);
		if (DBPROPSTATUS_OK != rgProperties->dwStatus)
			odtLog << "ERROR: Read only property could not be initialized in fact!\n";
	}
	FreeProperties( &cPropSets, &rgPropSets);

	// Initialize
	if(!GCHECK(m_hr = pIDBInitialize->Initialize(), S_OK))
	{
		odtLog << wszInitializeFailed;	
		goto CLEANUP1;
	}

	// check that the read only property was set
	rgPropID1[0]							= DBPROP_INIT_MODE;
	rgPropIDSet[nINIT_MODE].cPropertyIDs	= 1;
	rgPropIDSet[nINIT_MODE].rgPropertyIDs	= rgPropID1;
	rgPropIDSet[nINIT_MODE].guidPropertySet	= DBPROPSET_DBINIT;

	CHECK(m_hr = pIDBProperties->GetProperties(nActualSize, rgPropIDSet, &cPropSets, &rgPropSets), S_OK);
	if (	(DBPROPSTATUS_OK == rgPropSets[nINIT_MODE].rgProperties[0].dwStatus) 
		&&	(	(VT_I4 != rgPropSets[nINIT_MODE].rgProperties[0].vValue.vt)
			||	(DB_MODE_READ != V_I4(&rgPropSets[nINIT_MODE].rgProperties[0].vValue))))
	{
		odtLog << "despite ok reports, DBPROP_INIT_MODE is not set to read only\n";
		goto CLEANUP1;
	}

	// Obtain IDBCreateSesson, placing the new DSO interface 
	// in CThisTestModule's m_pIUnknown, so that all testcases can use 
	// it via their back pointer to this object.  No need to call AddRef 
	// here as we will own it, rather than the test module.	 
	if (!VerifyInterface(pIDBInitialize, IID_IDBCreateSession, DATASOURCE_INTERFACE, 
			(IUnknown**)&m_pIUnknown))
		goto CLEANUP1;		
						 
	// Create a DB session object
	// Set the m_pIUnknown2 to IOpenRowset
	if (!GCHECK(((IDBCreateSession*)m_pIUnknown)->CreateSession(
			NULL, IID_IOpenRowset, (IUnknown **)&m_pIUnknown2), S_OK))	
		goto CLEANUP1;		

	SetDBSession(m_pIUnknown2);	// calls ReleaseDBSession Before
	// switch the interface on ItableDefinition
	if (!(m_hr=VerifyInterface(m_pIUnknown2, IID_ITableDefinition, SESSION_INTERFACE, 
		(IUnknown**)&m_pITableDefinition)))
		goto CLEANUP1;
	
	fResults = TRUE;
CLEANUP1:
	FreeProperties( &cPropSets, &rgPropSets);
CLEANUP:
	SAFE_RELEASE(pIDBProperties);
	SAFE_RELEASE(pIDBInitialize);
	return fResults;

CLEANUP2:
	if (m_pITableDefinition)
	{
		m_pITableDefinition2	= m_pITableDefinition;
		m_pITableDefinition		= NULL;
	}
	return fResults;
} 


// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Add a column
//
// @rdesc TEST_PASS or TEST_FAIL
//
int RODataSource::Variation_1()
{ 
	
	BOOL			fResults = TEST_PASS;
	DBID			*pColumnID = NULL;
	HRESULT			hr;
	DBCOLUMNDESC	*rgColumnDesc = NULL;
	ULONG			cColumnDesc = m_pTable->CountColumnsOnTable();

	m_pTable->BuildColumnDescs(&rgColumnDesc);
	TESTC(0 < cColumnDesc);

	PROVIDER_FREE(rgColumnDesc->dbcid.uName.pwszName);
	FreeProperties( &rgColumnDesc->cPropertySets, &rgColumnDesc->rgPropertySets);
	rgColumnDesc[0].dbcid.uName.pwszName = wcsDuplicate(L"newColumn");
		
	if (!CHECK(hr = m_pITableDefinition->AddColumn(&m_pTable->GetTableID(), rgColumnDesc, &pColumnID), 
			DB_SEC_E_PERMISSIONDENIED))
		fResults = TEST_FAIL;

	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
CLEANUP:
	return fResults;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Drop a column
//
// @rdesc TEST_PASS or TEST_FAIL
//
int RODataSource::Variation_2()
{ 
	TBEGIN;
	DBID			ColumnID;
	DBCOLUMNDESC	*oldCD			= NULL;
	ULONG			oldlen			= 0;

	GetColumnDescOnTable(&m_pTable->GetTableID(), &oldCD, &oldlen);
	TESTC_PROVIDER(1 < oldlen);

	ColumnID.eKind			= DBKIND_NAME;
	ColumnID.uName.pwszName	= oldCD[0].dbcid.uName.pwszName;
	TEST2C_(DropColumnAndCheck(&m_pTable->GetTableID(), &ColumnID), S_OK, DB_SEC_E_PERMISSIONDENIED);

CLEANUP:
	ReleaseColumnDesc(oldCD, oldlen);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Create a table
//
// @rdesc TEST_PASS or TEST_FAIL
//
int RODataSource::Variation_3()
{ 
	BOOL	fResults	= TEST_PASS;
	DBID	*pTableID	= NULL;

	if (!CHECK(m_pITableDefinition->CreateTable(NULL, NULL, m_cColumnDesc, m_rgColumnDesc, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_SEC_E_PERMISSIONDENIED))
	{
		fResults = TEST_FAIL;
		m_pITableDefinition->DropTable(pTableID);
	}
	ReleaseDBID(pTableID);
	return fResults;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Drop a table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int RODataSource::Variation_4()
{ 
	BOOL	fResults = TEST_PASS;
	BOOL	fExists;

	if (	!CHECK(m_pITableDefinition->DropTable(&m_pTable->GetTableID()), DB_SEC_E_PERMISSIONDENIED)
		||	!CHECK(m_pTable->DoesTableExist(&m_pTable->GetTableID(), &fExists), S_OK)
		||	!fExists)
		fResults = TEST_FAIL;
	return fResults;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL RODataSource::Terminate()
{ 

// {{ TCW_TERM_BASECLASS_CHECK2
	SAFE_RELEASE(m_pITableDefinition);
	m_pITableDefinition		= m_pITableDefinition2;
	m_pITableDefinition2	= NULL;
	SetDBSession(m_pITableDefinition);
	if (m_pTable)
	{
		m_hr = m_pTable->DropTable();
		delete m_pTable;
		m_pTable = NULL;
	}
	return(TCDropColumn::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END

// Hack to defile CoInitializeEx
WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

unsigned WINAPI ThreadProc(LPVOID lpvThreadParam)
{
	CoInitializeEx(NULL, 0);
	TCITableDefinition	*pObject = ((CInParam*)lpvThreadParam)->pObject;
	pObject->MyThreadProc(((CInParam*)lpvThreadParam)->i);
	CoUninitialize();
	return 1;
} //ThreadProc

// {{ TCW_TC_PROTOTYPE(TCAddConstraint_Boundary)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_Boundary - General and boundary/null test for ITableDefinitionWithConstraints::AddConstraint
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_Boundary::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Check the session object (QI for interfaces)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_1()
{ 
	return DefaultObjectTesting(m_pITDWC, SESSION_INTERFACE);
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_2()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");

	CHECK(AddConstraintAndCheck(m_pITDWC, NULL, consFK), E_INVALIDARG);
	CHECK(AddConstraintAndCheck(m_pITDWC, NULL, consPK), E_INVALIDARG);
	CHECK(AddConstraintAndCheck(m_pITDWC, NULL, consUnique), E_INVALIDARG);
	CHECK(AddConstraintAndCheck(m_pITDWC, NULL, consCheck), E_INVALIDARG);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc *pTableID is DB_NULLID => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_3()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;

	DuplicateDBID(DB_NULLID, &TableID);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Table name is NULL => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_4()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");

	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= NULL;

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Table name is the empty string => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_5()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");

	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= L"";

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Table name has maximum length
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_6()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;

	// create a table with maximum length
	BaseTable.MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName, BaseTable.GetTableName());

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0, TableID.uName.pwszName), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	// create the text of the constraint
	TESTC_(BaseTable.GetColInfo(1 , col), S_OK);	
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	CHECK(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Table name exceeds maximum length => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_7()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;

	// create a table with maximum length
	BaseTable.MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildValidName(m_cMaxTableName+1, BaseTable.GetTableName());

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	// create the text of the constraint
	TESTC_(BaseTable.GetColInfo(1 , col), S_OK);	
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Table name contains illegal characters
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_8()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;
	ULONG				m, n;

	// create a table with maximum length
	BaseTable.MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	m = min(m_cMaxTableName, n=5+wcslen(BaseTable.GetTableName()));
	TableID.uName.pwszName	= BuildInvalidName(m, BaseTable.GetTableName(), m_pwszInvalidTableChars);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	// create the text of the constraint
	TESTC_(BaseTable.GetColInfo(1 , col), S_OK);	
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Invalid DBKIND value for the base table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_9()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				TableID;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;

	// create a table with maximum length
	BaseTable.MakeTableName(NULL);
	memset(&TableID, 0, sizeof(DBID));
	TableID.eKind			= 0xce;

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the check constraint
	// create the text of the constraint
	TESTC_(BaseTable.GetColInfo(1 , col), S_OK);	
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consFK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consPK), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consUnique), DB_E_NOTABLE, DB_E_BADTABLEID);
	TEST2C_(AddConstraintAndCheck(m_pITDWC, &TableID, consCheck), DB_E_NOTABLE, DB_E_BADTABLEID);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc NULL column list in a constraint, column size is not 0 => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_10()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1, 2, 3};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pConsDesc;
	DBID				*p1 = NULL, *p2 = NULL;

	// create a table with maximum length
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consUnique;
	p1 = pConsDesc->rgColumnList;
	pConsDesc->rgColumnList	= NULL;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), E_INVALIDARG);
	pConsDesc->rgColumnList = p1;

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consPK;
	p1 = pConsDesc->rgColumnList;
	pConsDesc->rgColumnList	= NULL;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), E_INVALIDARG);
	pConsDesc->rgColumnList = p1;

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	p1 = pConsDesc->rgColumnList;
	pConsDesc->rgColumnList	= NULL;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), E_INVALIDARG);

	p2 = pConsDesc->rgForeignKeyColumnList;
	pConsDesc->rgForeignKeyColumnList	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), E_INVALIDARG);

	pConsDesc->rgColumnList = p1;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), E_INVALIDARG);
	pConsDesc->rgColumnList = p1;
	pConsDesc->rgForeignKeyColumnList = p2;

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc NULL column name in a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_11()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1, 2, 3};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pConsDesc;

	// create a table with maximum length
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= NULL;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[2], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgForeignKeyColumnList[1].uName.pwszName	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);
	
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[1], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgForeignKeyColumnList[1].uName.pwszName	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consUnique;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCOLUMNID);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consPK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= NULL;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Empty string for a column name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_12()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1, 2, 3};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pConsDesc;

	// create a table with maximum length
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[2], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgForeignKeyColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);
	
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[1], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgForeignKeyColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consUnique;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCOLUMNID);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consPK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind			= DBKIND_NAME;
	pConsDesc->rgColumnList[1].uName.pwszName	= wcsDuplicate(L"");
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Maximum length column name in a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_13()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;

	WCHAR				*pwszName			= NULL;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;

	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CCol				col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	BaseTable.SetColumnDesc(rgColumnDesc, cColumnDesc);
	BaseTable.SetBuildColumnDesc(FALSE);
	BaseTable.MakeTableName(NULL);
	pwszName = wcsDuplicate(BaseTable.GetTableName());

	if (rgColumnDesc[0].dbcid.uName.pwszName)
		SAFE_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName = BuildValidName(m_cMaxColName, pwszName);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	
	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, NUMELEM(rgCol), rgCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);

	// set up the check constraint
	TESTC_(BaseTable.GetColInfo(rgCol[0], col), S_OK);
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), S_OK);

CLEANUP:
	SAFE_FREE(pwszName);
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	CHECK(BaseTable.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Column name exceeds maximum length
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_14()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;

	WCHAR				*pwszName			= NULL;
	WCHAR				*pwszColName		= NULL;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;

	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CCol				col;

	DBCONSTRAINTDESC	*pConsDesc;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	BaseTable.SetColumnDesc(rgColumnDesc, cColumnDesc);
	BaseTable.SetBuildColumnDesc(FALSE);
	BaseTable.MakeTableName(NULL);
	pwszName = wcsDuplicate(BaseTable.GetTableName());

	if (rgColumnDesc[0].dbcid.uName.pwszName)
		SAFE_FREE(rgColumnDesc[0].dbcid.uName.pwszName);
	rgColumnDesc[0].dbcid.eKind = DBKIND_NAME;
	rgColumnDesc[0].dbcid.uName.pwszName = BuildValidName(m_cMaxColName, pwszName);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	
	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, NUMELEM(rgCol), rgCol), S_OK);

	pwszColName = BuildValidName(m_cMaxColName+1, pwszName);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	SAFE_FREE(pConsDesc->rgForeignKeyColumnList[0].uName.pwszName);
	pConsDesc->rgForeignKeyColumnList[0].uName.pwszName = wcsDuplicate(pwszColName);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);
	pConsDesc = consUnique;
	SAFE_FREE(pConsDesc->rgColumnList[0].uName.pwszName);
	pConsDesc->rgColumnList[0].uName.pwszName = wcsDuplicate(pwszColName);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);
	pConsDesc = consPK;
	SAFE_FREE(pConsDesc->rgColumnList[0].uName.pwszName);
	pConsDesc->rgColumnList[0].uName.pwszName = wcsDuplicate(pwszColName);

	// set up the check constraint
	TESTC_(BaseTable.GetColInfo(rgCol[0], col), S_OK);
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(pwszColName) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, pwszColName);
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCOLUMNID);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCOLUMNID);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), E_FAIL);

CLEANUP:
	SAFE_FREE(pwszColName);
	SAFE_FREE(pwszName);
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	CHECK(BaseTable.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Invalid DBKIND for a column DBID of a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_15()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1, 2, 3};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pConsDesc;

	// create a table with maximum length
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind = 0xfe;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind = 0xfe;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[2], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind = 0xfe;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);
	
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[1], FALSE);
	pConsDesc->rgForeignKeyColumnList[1].eKind = 0xfe;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consUnique;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind = 0xee;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCOLUMNID);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consPK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	pConsDesc->rgColumnList[1].eKind = 0xac;
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Column ID is DB_NULLID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_16()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1, 2, 3};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pConsDesc;

	// create a table with maximum length
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	DuplicateDBID(DB_NULLID, &pConsDesc->rgColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	DuplicateDBID(DB_NULLID, &pConsDesc->rgColumnList[1]);
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[2], FALSE);
	DuplicateDBID(DB_NULLID, &pConsDesc->rgForeignKeyColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);
	
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	pConsDesc = consFK;
	ReleaseDBID(&pConsDesc->rgForeignKeyColumnList[1], FALSE);
	DuplicateDBID(DB_NULLID, &pConsDesc->rgForeignKeyColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCOLUMNID);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consUnique;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	memset(&pConsDesc->rgColumnList[1], 0, sizeof(DBID));
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCOLUMNID);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pConsDesc = consPK;
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);
	memset(&pConsDesc->rgColumnList[1], 0, sizeof(DBID));
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_17()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;

	// create the referenced table in this catalog
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	consFK.SetReferencedTableID(NULL);

	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);

	CHECK(consFK.ReleaseForeignKeyColumnList(), S_OK);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc DBNULL_ID *pReferencedTableID for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_18()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;

	// create the referenced table in this catalog
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	consFK.SetReferencedTableID((DBID*)&DB_NULLID);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc NULL referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_19()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	DBID				ReferencedTableID;

	// create the referenced table in this catalog
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= NULL;
	consFK.SetReferencedTableID(&ReferencedTableID);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Emptry string referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_20()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	DBID				ReferencedTableID;

	// create the referenced table in this catalog
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= L"";
	consFK.SetReferencedTableID(&ReferencedTableID);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Maximum length referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_21()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	WCHAR				*pwszTableName = NULL;

	// create the referenced table in this catalog
	// create a table with maximum length
	ReferencedTable.MakeTableName(NULL);
	pwszTableName	= BuildValidName(m_cMaxTableName, ReferencedTable.GetTableName());
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0, pwszTableName));

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), S_OK);

CLEANUP:
	SAFE_FREE(pwszTableName);
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Referenced table name longer than allowed for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_22()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	WCHAR				*pwszTableName = NULL;
	DBCONSTRAINTDESC	*pConsDesc = consFK;

	// create the referenced table in this catalog
	// create a table with maximum length
	ReferencedTable.MakeTableName(NULL);
	pwszTableName	= BuildValidName(m_cMaxTableName, ReferencedTable.GetTableName());
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0, pwszTableName));

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	SAFE_FREE(pwszTableName);
	ReleaseDBID(pConsDesc->pReferencedTableID, FALSE);
	pConsDesc->pReferencedTableID->eKind = DBKIND_NAME;
	pConsDesc->pReferencedTableID->uName.pwszName = BuildValidName(m_cMaxTableName + 1, ReferencedTable.GetTableName());

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADTABLEID);

CLEANUP:
	SAFE_FREE(pwszTableName);
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Illegal characters in the referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_23()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	DBID				ReferencedTableID;
	DBORDINAL			m, n;

	// create the referenced table in this catalog
	// create a table with maximum length
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);
	m = min(m_cMaxTableName, n=5+wcslen(ReferencedTable.GetTableName()));
	ReferencedTableID.uName.pwszName	= BuildInvalidName(m, ReferencedTable.GetTableName(), m_pwszInvalidTableChars);
	consFK.SetReferencedTableID(&ReferencedTableID);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADTABLEID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	ReleaseDBID(&ReferencedTableID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Bad constraint type
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_24()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CConsDesc			cons((DBCONSTRAINTTYPE)0xfd);

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);

	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_BADCONSTRAINTTYPE);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Empty list of columns for a unique, primay key or foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_25()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;

	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);
	consFK.ReleaseColumnList();
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);
	
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);
	consFK.ReleaseForeignKeyColumnList();
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);

	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);
	consFK.ReleaseForeignKeyColumnList();
	consFK.ReleaseColumnList();
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);
	consUnique.ReleaseColumnList();
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCONSTRAINTFORM);

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgCol), rgCol), S_OK);
	consPK.ReleaseColumnList();
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	CHECK(BaseTable.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc Non empty list of columns for a check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_26()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pcons = consCheck;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;
	DBID				rgColList;
	DBID				rgForeignKeyColList;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);

	// set up the check constraint
	// create the text of the constraint
	TESTC_(BaseTable.GetColInfo(1 , col), S_OK);	
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	DuplicateDBID(*col.GetColID(), &rgColList);
	DuplicateDBID(*col.GetColID(), &rgForeignKeyColList);

	// not empty rgColumnList
	pcons->cColumns = 1;
	pcons->rgColumnList = &rgColList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), DB_E_BADCONSTRAINTFORM);

	// not empty rgForeignKeyColumnList
	pcons->cColumns = 0;
	pcons->rgColumnList = NULL;
	pcons->cForeignKeyColumns = 1;
	pcons->rgForeignKeyColumnList = &rgForeignKeyColList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), DB_E_BADCONSTRAINTFORM);

	// not empty rgColumnList and rgForeignKeyColumnList
	pcons->cColumns = 1;
	pcons->rgColumnList = &rgColList;
	pcons->cForeignKeyColumns = 1;
	pcons->rgForeignKeyColumnList = &rgForeignKeyColList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	pcons->cColumns = 0;
	pcons->rgColumnList = NULL;
	pcons->cForeignKeyColumns = 0;
	pcons->rgForeignKeyColumnList = NULL;
	ReleaseDBID(&rgColList, FALSE);
	ReleaseDBID(&rgForeignKeyColList, FALSE);
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc Not null pwszConstraintText in a unique, PK or FK constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_27()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	DBCONSTRAINTDESC	*pcons;

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);
	pcons = consFK;
	pcons->pwszConstraintText = L"fdfd";

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pcons = consUnique;
	pcons->pwszConstraintText = L"fdfd";

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	pcons = consPK;
	pcons->pwszConstraintText = L"fdfd";

	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), DB_E_BADCONSTRAINTFORM);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCONSTRAINTFORM);
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	pcons = consFK;
	pcons->pwszConstraintText = NULL;
	pcons = consPK;
	pcons->pwszConstraintText = NULL;
	pcons = consUnique;
	pcons->pwszConstraintText = NULL;
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc NULL pConstraintDesc => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_28()
{ 
	TBEGIN

	CHECK(AddConstraintAndCheck(m_pITDWC, &m_pTable->GetTableID(), NULL), E_INVALIDARG);

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc 0 < cColumns and NULL == rgColumnList => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_29()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	CHECK(consFK.ReleaseColumnList(), S_OK);
	((DBCONSTRAINTDESC*)consFK)->cColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consFK)->cColumns = 0;

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	CHECK(consUnique.ReleaseColumnList(), S_OK);
	((DBCONSTRAINTDESC*)consUnique)->cColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consUnique)->cColumns = 0;

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	CHECK(consPK.ReleaseColumnList(), S_OK);
	((DBCONSTRAINTDESC*)consPK)->cColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consPK)->cColumns = 0;

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");
	((DBCONSTRAINTDESC*)consCheck)->cColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consCheck)->cColumns = 0;

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc 0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_30()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(consFK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, m_SupportedMatchType), S_OK);
	CHECK(consFK.ReleaseForeignKeyColumnList(), S_OK);
	((DBCONSTRAINTDESC*)consFK)->cForeignKeyColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consFK), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consFK)->cForeignKeyColumns = 0;

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = 0;

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = 0;

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = 3;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), E_INVALIDARG);
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = 0;

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc Constraint is not foreign key and pReferencedTableID is not NULL
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_31()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBID				ReferencedTableID;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				*pwszColValue		= NULL;
	CCol				col;
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= (LPWSTR)0x01234567;
	
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0, 0), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consUnique)->pReferencedTableID = &ReferencedTableID;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consUnique)->pReferencedTableID = NULL;

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consPK)->pReferencedTableID = &ReferencedTableID;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consPK)->pReferencedTableID = NULL;

	// set up the check constraint
	TESTC_(BaseTable.GetColInfo(rgBaseCol[0], col), S_OK);
	TESTC_(BaseTable.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	consCheck.SetConstraintText(pwszConstraintText);

	((DBCONSTRAINTDESC*)consCheck)->pReferencedTableID = &ReferencedTableID;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consCheck)->pReferencedTableID = NULL;

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc Non empty list of foreign keys for a constraint that is not foreign key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_32()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	const DBORDINAL		cColumns	= 2;
	DBID				rgColumnList[cColumns];

	rgColumnList[0].eKind				= DBKIND_NAME;
	rgColumnList[0].uName.pwszName	= (LPWSTR)0x01234567;
	rgColumnList[1].eKind				= 0xfe;
	rgColumnList[1].uName.pwszName	= (LPWSTR)0x01234567;
	
	TESTC_(BaseTable.CreateTable(0, 0), S_OK);

	// set up the unique constraint
	TESTC_(consUnique.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consUnique)->rgForeignKeyColumnList = rgColumnList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consUnique), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consUnique)->rgForeignKeyColumnList = NULL;

	// set up the primary key constraint
	TESTC_(consPK.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol), S_OK);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consPK)->rgForeignKeyColumnList = rgColumnList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consPK), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consPK)->rgForeignKeyColumnList = NULL;

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consCheck)->rgForeignKeyColumnList = rgColumnList;
	CHECK(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), consCheck), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consCheck)->rgForeignKeyColumnList = NULL;

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc Inexistent referenced table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Boundary::Variation_33()
{ 
	TBEGIN
	CTable					BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable					Table(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons			cons;
	DBORDINAL				rgBaseCol[]			= {1};
	DBORDINAL				rgReferencedCol[]	= {1};

	// create a table
	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC_(cons.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol,
		&BaseTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// set referenced table to an inexistent table name
	Table.MakeTableName(NULL);
	cons.SetReferencedTableID(&Table.GetTableID());

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_NOTABLE);

CLEANUP:
	BaseTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_Boundary::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAddConstraint_UNIQUE)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_UNIQUE - Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_UNIQUE::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		return NULL != m_pITDWC? TRUE: TEST_SKIPPED;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Create a table and try to apply the constraint on each column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_1()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			index;
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;

	// create the unique constraint

	// create the base table
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// build the constraint

	// try to set all the columns as unique
	pConsDesc->cColumns = Table.CountColumnsOnTable();
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	for (index=1; index <= pConsDesc->cColumns; index++)
	{
		CCol	col;

		TESTC_(Table.GetColInfo(index, col), S_OK);
		DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[index-1]);
	}

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Set a unique constraint on each type of column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_2()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL			index;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	for (index=1; index <= Table.CountColumnsOnTable(); index++)
	{
		CCol	col;

		TESTC_(Table.GetColInfo(index, col), S_OK);
		DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

		odtLog << col.GetProviderTypeName() << "\n";
		if (CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK))
			TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), pConsDesc->pConstraintID), S_OK);

		// release the DBID content of the column in the constraint description
		ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	}

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Create a unique constraint that using several columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_3()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			index;
	DBORDINAL			indxCol;
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 2 + rand() % (Table.CountColumnsOnTable()-1);
	odtLog << "Constraint has " << pConsDesc->cColumns << " columns\n";
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	for (index=0, indxCol=1; index < pConsDesc->cColumns; indxCol++)
		
	{
		CCol	col;

		if (	(pConsDesc->cColumns - index > Table.CountColumnsOnTable()-indxCol)
			||	rand() %2)
		{
			// put this column in the list
			TESTC_(Table.GetColInfo(indxCol, col), S_OK);
			DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[index++]);
		}
	}

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Create several unique constraints on a table, no overlapping
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_4()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_PROVIDER(4 <= Table.CountColumnsOnTable());

	// alloc memory for column array
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 2 columns and create a unique constraint on them
	TESTC_(Table.GetColInfo(4 + rand() % (Table.CountColumnsOnTable()-3), col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);
	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// release some of the mamory used before
	SAFE_FREE(pConsDesc->pConstraintID->uName.pwszName);
	ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);

	// choose another 2 columns and create a unique constraint
	CHECK(cons.MakeConstraintName(), S_OK);
	TESTC_(Table.GetColInfo(3, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Create multiple, overlapping constraints on a table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_5()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_PROVIDER(3 <= Table.CountColumnsOnTable());

	// alloc memory for column array
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 2 columns and create a unique constraint on them
	TESTC_(Table.GetColInfo(3, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);
	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// release some of the memory used before
	SAFE_FREE(pConsDesc->pConstraintID->uName.pwszName);
	ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	ReleaseDBID(&pConsDesc->rgColumnList[1], FALSE);

	// choose another 2 columns and create a unique constraint
	CHECK(cons.MakeConstraintName(), S_OK);
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);
	TESTC_(Table.GetColInfo(3, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint, drop it and recreate it
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_6()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1 + rand() % Table.CountColumnsOnTable(), col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint and recreate it
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), pConsDesc->pConstraintID), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint on a null column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_7()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	// by setting DBCOLUMNDESC, all nullable type column are created as nullable 
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_PROVIDER(S_OK == Table.GetColInfo(IsCColNullable, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Create a contraint on a not nullable column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_8()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	// create a new table with a NOT NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_PROVIDER(S_OK == Table.GetColInfo(IsCColNotNullable, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint on a column that has a default value
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_9()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	VARIANT				value;
	DBORDINAL			indexCol;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	Table.CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	// by setting DBCOLUMNDESC, all nullable type column are created as nullable 
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	// create a default value on one of the columns
	VariantInit(&value);
	indexCol = 1 + rand() % m_cColumnDesc;
	if (rgNewColumnDesc[indexCol-1].pwszTypeName)
		odtLog << "Default value on type " << rgNewColumnDesc[indexCol-1].pwszTypeName << "\n";
	GetDefaultValue(&rgNewColumnDesc[indexCol-1], &value, &Table);
	SetDefaultProperty(&rgNewColumnDesc[indexCol-1].rgPropertySets, &rgNewColumnDesc[indexCol-1].cPropertySets,
				&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&value);

	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_PROVIDER(S_OK == Table.CreateTable(0, 0));

	// identify a nullable column
	TESTC_(Table.GetColInfo(indexCol, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint on a column defined as a primary key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_10()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	DBORDINAL			indexCol;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	Table.CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	// by setting DBCOLUMNDESC, all nullable type column are created as nullable 
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	// create a default value on one of the columns
	indexCol = 1 + rand() % m_cColumnDesc;
	if (rgNewColumnDesc[indexCol-1].pwszTypeName)
		odtLog << "Default value on type " << rgNewColumnDesc[indexCol-1].pwszTypeName << "\n";
	SetProperty(&rgNewColumnDesc[indexCol-1].rgPropertySets, &rgNewColumnDesc[indexCol-1].cPropertySets,
				DBPROP_COL_PRIMARYKEY, 
				VT_BOOL, (LPVOID)VARIANT_TRUE, 
				DBPROPOPTIONS_REQUIRED);

	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_PROVIDER(S_OK == Table.CreateTable(0, 0));

	// identify a nullable column
	TESTC_(Table.GetColInfo(indexCol, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint on a column defined as a foreign key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_11()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol					col, col2;
	DBCOLUMNDESC			*rgNewColumnDesc = NULL;
	DBORDINAL				indexCol;
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons			cons2(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc2 = (DBCONSTRAINTDESC*)cons2;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())
	indexCol = 1 + rand() % Table.CountColumnsOnTable();
	TESTC_(Table.GetColInfo(indexCol, col), S_OK);
	TESTC_(Table2.GetColInfo(indexCol, col2), S_OK);

	// prepare and add the unique constraint on the referenced table
	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col2.GetColID(), pConsDesc->rgColumnList);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table2.GetTableID(), cons), S_OK);
	
	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc2->cColumns = 1;
	SAFE_ALLOC(pConsDesc2->rgColumnList, DBID, pConsDesc2->cColumns);
	memset(pConsDesc2->rgColumnList, 0, sizeof(pConsDesc2->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc2->rgColumnList);

	// set foreign column
	pConsDesc2->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc2->rgForeignKeyColumnList, DBID, pConsDesc2->cForeignKeyColumns);
	memset(pConsDesc2->rgForeignKeyColumnList, 0, sizeof(pConsDesc2->rgForeignKeyColumnList));
	DuplicateDBID(*col2.GetColID(), pConsDesc2->rgForeignKeyColumnList);

	// set reference table
	SAFE_ALLOC(pConsDesc2->pReferencedTableID, DBID, 1);
	DuplicateDBID(Table2.GetTableID(), pConsDesc2->pReferencedTableID);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons2), S_OK);

	// prepare and add the unique constraint on the column involved in a foreign key 
	// alloc memory for column array
	ReleaseDBID(pConsDesc->rgColumnList, FALSE);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	SAFE_FREE(pConsDesc->pConstraintID->uName.pwszName);
	pConsDesc->pConstraintID->uName.pwszName = wcsDuplicate(L"Laleaua");
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Create a constraint on a column that is included in an index
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_12()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol					col;

	// create a table and an index
	TESTC_(Table.CreateTable(0, 1), S_OK);
	
	// select a column (the one used in index)
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	
CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Use the name of an already existing constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_13()
{ 
	TBEGIN
	CTable					Table(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable					ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	CUniqueCons				UniqueCons;
	CPrimaryKeyCons			PKCons;
	CCheckCons				CheckCons;
	CForeignKeyCons			FKCons;
	DBORDINAL				rgCol[] = {1};

	// create a table
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1), S_OK);
	
	// create a unique constraint
	TESTC_(UniqueCons.SetConstraint(&Table, 1), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), UniqueCons), S_OK);

	// create a check constraint
	CheckCons.SetLTCheckConstraint(&Table, 1, 60);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), CheckCons), S_OK);

	// create a PK constraint
	TESTC_(PKCons.SetConstraint(&Table, 1), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), PKCons), S_OK);

	// create a FK constraint
	TESTC_(FKCons.SetConstraint(&Table, NUMELEM(rgCol), rgCol, 
		&ReferencedTable, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), FKCons), S_OK);

	// add the constraint again, this time on other column (if possible)
	TESTC_(cons.SetConstraint(&Table, 2), S_OK);

	odtLog << "Unique constraint duplicate unique constraint name\n";
	cons.SetConstraintID(UniqueCons.GetConstraintID());
	CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_DUPLICATECONSTRAINTID);

	odtLog << "Unique constraint duplicate check constraint name\n";
	cons.SetConstraintID(CheckCons.GetConstraintID());
	CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_DUPLICATECONSTRAINTID);

	odtLog << "Unique constraint duplicate primary key constraint name\n";
	cons.SetConstraintID(PKCons.GetConstraintID());
	CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_DUPLICATECONSTRAINTID);

	odtLog << "Unique constraint duplicate foreign key constraint name\n";
	cons.SetConstraintID(FKCons.GetConstraintID());
	CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_DUPLICATECONSTRAINTID);

CLEANUP:
	CHECK(Table.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Constraint is created in a different catalog
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_14()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol					col;
	DBORDINAL				indxCat = 0;
	CCatalogs				Catalogs(m_pIOpenRowset);
	IDBProperties			*pIDBProperties = NULL;
	ULONG					cPropSets = 0;
	DBPROPSET				*rgPropSets = NULL;
	WCHAR					*pwszCurrentCatalog = NULL;
	WCHAR					*pwszQualTableName = NULL;
	WCHAR					*pwszCatalogName = NULL;
	WCHAR					*pwszSchemaName = NULL;

	TESTC_PROVIDER(SettableProperty(DBPROP_CURRENTCATALOG, DBPROPSET_DATASOURCE, m_pIDBInitialize));
	TESTC_PROVIDER(1 < Catalogs.cCatalogs());

	// create a table and an index
	TESTC_(Table.CreateTable(0, 0), S_OK);
	
	// select column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// set DBID of the table to its qualified form
	TESTC(DBKIND_NAME == Table.GetTableID().eKind);

	TESTC(GetCatalogSchemaNames(Table.GetTableName(), &pwszCatalogName, &pwszSchemaName));

	//Construct a fully Qualified TableName...
	TESTC_(m_pTable->GetQualifiedName(pwszCatalogName, pwszSchemaName, Table.GetTableName(), &pwszQualTableName),S_OK);
	Table.SetTableName(pwszQualTableName);

	// change the current catalog
	TESTC(GetProperty(DBPROP_CURRENTCATALOG, DBPROPSET_DATASOURCE, m_pIDBInitialize,
		&pwszCurrentCatalog));
	TESTC(NULL != pwszCurrentCatalog);

	SAFE_ALLOC(rgPropSets, DBPROPSET, 1);
	rgPropSets[0].cProperties = 0;
	rgPropSets[0].rgProperties = NULL;
	rgPropSets[0].guidPropertySet = DBPROPSET_DATASOURCE;
	TESTC(VerifyInterface(m_pIDBInitialize, IID_IDBProperties, DATASOURCE_INTERFACE, (IUnknown**)&pIDBProperties));

	if (0 == wcscmp(pwszCurrentCatalog, Catalogs[0]))
		indxCat++;

	TESTC_(::SetProperty(DBPROP_CURRENTCATALOG, DBPROPSET_DATASOURCE, &cPropSets, &rgPropSets, VT_BSTR, (LPVOID)Catalogs[indxCat]), S_OK);
	TESTC_PROVIDER(S_OK == pIDBProperties->SetProperties(cPropSets, rgPropSets));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// try to make column unique
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
	FreeProperties(&cPropSets, &rgPropSets);
	SAFE_FREE(pwszCurrentCatalog);
	SAFE_RELEASE(pIDBProperties);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc create a unique constraint on a non empty table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_15()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol					col;

	// create a table and an index
	TESTC_(Table.CreateTable(10, 0), S_OK);
	
	// select column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Create a unique constraints on a column that has doubled values
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_UNIQUE::Variation_16()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons				cons;
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol					col;

	// create a table and an index
	TESTC_(Table.CreateTable(10, 0), S_OK);
	// double one of the rows
	TESTC_PROVIDER(S_OK == Table.Insert(5));
	TESTC_PROVIDER(S_OK == Table.Insert(7));
	
	// select column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_UNIQUE::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAddConstraint_PRIMARYKEY)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_PRIMARYKEY - Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_PRIMARYKEY
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_PRIMARYKEY::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return (NULL != m_pITDWC) ? TRUE: TEST_SKIPPED;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Create a table and try to apply the constraint on each column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_1()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			index;
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;

	// create the base table
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// build the constraint

	// try to set all the columns as primary key
	pConsDesc->cColumns = Table.CountColumnsOnTable();
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	for (index=1; index <= pConsDesc->cColumns; index++)
	{
		CCol	col;

		TESTC_(Table.GetColInfo(index, col), S_OK);
		DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[index-1]);
	}

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Set a primary key constraint on each type of column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_2()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			index;
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// build the constraint

	// try to set all the columns as primary key
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	for (index=1; index <= Table.CountColumnsOnTable(); index++)
	{
		CCol	col;

		TESTC_(Table.GetColInfo(index, col), S_OK);
		DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

		odtLog << col.GetProviderTypeName() << "\n";

		if (CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK))
			TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
		ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	}

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Create a primary key constraint. Drop it. Create a primary key using the same name as before.
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_3()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// build the constraint

	// try to set all the columns as primary key
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	// create constraint with the same name as before, different column
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Create multiple primary key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_4()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);

	// create constraint with the same name as before, different column
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Create multiple primary key constraints using successive calls
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_5()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// build the constraint

	// try to set all the columns as primary key
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	ReleaseDBID(&pConsDesc->rgColumnList[0], FALSE);
	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	// create constraint with the same name as before, different column
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Successive calls, lists of columns overlap
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_6()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// build the constraint

	// try to set all the columns as primary key
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, 2);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[0]);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);

	// create constraint with the same name as before, different column
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a nullable column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_7()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_TRUE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_PROVIDER(S_OK == Table.GetColInfo(IsCColNullable, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a not nullable column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_8()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column with a default value
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_9()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	VARIANT				value;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	Table.CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	VariantInit(&value);
	odtLog << "Default value on type " << rgNewColumnDesc[0].pwszTypeName << "\n";
	GetDefaultValue(&rgNewColumnDesc[0], &value, &Table);
	SetDefaultProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
				&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&value);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column defined as unique
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_10()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	TESTC_PROVIDER(SettableProperty(DBPROP_COL_UNIQUE, DBPROPSET_COLUMN, m_pIDBInitialize));

	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_UNIQUE, 
			VT_BOOL, (LPVOID)VARIANT_TRUE, 
			DBPROPOPTIONS_REQUIRED);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_OPTIONAL);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	

	TEST2C_(Table.CreateTable(0, 0), S_OK, DB_S_ERRORSOCCURRED);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column defined as primary key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_11()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;

	TESTC_PROVIDER(SettableProperty(DBPROP_COL_PRIMARYKEY, DBPROPSET_COLUMN, m_pIDBInitialize));

	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_PRIMARYKEY, 
			VT_BOOL, (LPVOID)VARIANT_TRUE, 
			DBPROPOPTIONS_REQUIRED);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_OPTIONAL);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TEST2C_(Table.CreateTable(0, 0), S_OK, DB_S_ERRORSOCCURRED);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column that already has a primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_12()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// another constraint ID
	CHECK(cons.MakeConstraintName(), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column that is part of a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_13()
{ 
	TBEGIN
	CTable					BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL				indexCol = 1;

	// create a base table without nullable columns and a reference table
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(BaseTable.CountColumnsOnTable() == ReferencedTable.CountColumnsOnTable())

	// prepare and add the unique constraint on the referenced table
	TESTC_(AddUniqueConstraint(&ReferencedTable, indexCol), S_OK);
	
	// build the foreign key constraint, use corresponding columns in tables
	TESTC_(AddForeignKeyConstraint(&BaseTable, 1, &indexCol, 
		&ReferencedTable, 1, &indexCol), S_OK);

	// prepare and add the unique constraint on the column involved in a foreign key 
	TESTC_(AddUniqueConstraint(&BaseTable, indexCol), S_OK);
	
CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column that is part of an index
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_14()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0, 1), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column on a not empty table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_15()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 10), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint on a column that already has duplicates (for the whole key)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_16()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	// create a base table without nullable columns
	TESTC_(CreateTableWithNoNullableColumns(&Table, 10), S_OK);
	TESTC_(Table.Insert(7), S_OK);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), E_FAIL);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Primary key constraint that has duplicates for a column in the key, but not for the whole key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_PRIMARYKEY::Variation_17()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	VARIANT				value;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	HRESULT				hr;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	Table.CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	// create a new table with a NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	VariantInit(&value);
	GetDefaultValue(&rgNewColumnDesc[0], &value, &Table);
	SetDefaultProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
				&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&value);
	Table.SetColumnDesc(rgNewColumnDesc+1, m_cColumnDesc-1);
	Table.SetBuildColumnDesc(FALSE);
	SetProperty(&rgNewColumnDesc[1].rgPropertySets, &rgNewColumnDesc[1].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	
	TEST2C_(hr = Table.CreateTable(10, 0), S_OK, DB_S_ERRORSOCCURRED);

	TEST2C_(hr = m_pITableDefinition->AddColumn(&Table.GetTableID(), 
		rgNewColumnDesc, NULL), S_OK, DB_E_ERRORSOCCURRED);
	TESTC(S_OK == hr);

	// identify a nullable column
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// build the primary key
	DuplicateDBID(rgNewColumnDesc[0].dbcid, pConsDesc->rgColumnList);
	DuplicateDBID(*col.GetColID(), &pConsDesc->rgColumnList[1]);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.SetColumnDesc(NULL, 0);
	ReleaseColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_PRIMARYKEY::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAddConstraint_FOREIGNKEY)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_FOREIGNKEY - Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_FOREIGNKEY
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_FOREIGNKEY::Init()
{ 
	m_pReferenceTable = NULL;
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		m_pReferenceTable = new CTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
		TESTC_(m_pReferenceTable->CreateTable(0, 1), S_OK);
		return (NULL != m_pITDWC) ? TRUE: TEST_SKIPPED;
	} 
CLEANUP:
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Simple foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_1()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col, col2;
	DBORDINAL				indexCol = 1;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);

	TESTC_(Table.CreateTable(0, 0), S_OK);
	
	// select columns from tables
	TESTC(Table.CountColumnsOnTable() == m_pReferenceTable->CountColumnsOnTable())
	TESTC_(Table.GetColInfo(indexCol, col), S_OK);
	TESTC_(m_pReferenceTable->GetColInfo(indexCol, col2), S_OK);

	// build the foreign key constraint, use corresponding columns in tables
	cons.AddColumn(col.GetColID());
	cons.AddForeignKeyColumn(col2.GetColID());
	cons.SetReferencedTableID(&m_pReferenceTable->GetTableID());

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Foregin key is made of several columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_2()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				rgCol[] = {1, 2};

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(Table.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList+1);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 2;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	TESTC_(Table2.GetColInfo(2, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList+1);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Multiple foreign key constraints on a table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_3()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				rgCol[] = {1, 2};
	DBORDINAL				rgCol2[] = {3, 4};

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));
	TESTC_(Table.GetColInfo(rgCol[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(Table.GetColInfo(rgCol[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList+1);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 2;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));
	TESTC_(m_pReferenceTable->GetColInfo(rgCol[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList+1);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol2), rgCol2), S_OK);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// set a second foreign constraint
	TESTC_(cons.MakeConstraintName(), S_OK);
	
	ReleaseDBID(pConsDesc->rgColumnList, FALSE);
	ReleaseDBID(pConsDesc->rgColumnList+1, FALSE);
	ReleaseDBID(pConsDesc->rgForeignKeyColumnList, FALSE);
	ReleaseDBID(pConsDesc->rgForeignKeyColumnList+1, FALSE);
	
	TESTC_(Table.GetColInfo(rgCol2[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(Table.GetColInfo(rgCol2[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList+1);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol2[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol2[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList+1);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Multiple overlapping foreign key constraints
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_4()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				rgCol[] = {1, 2};
	DBORDINAL				rgCol2[] = {3, 2};

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc->cColumns = 2;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));
	TESTC_(Table.GetColInfo(rgCol[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(Table.GetColInfo(rgCol[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList+1);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 2;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));
	TESTC_(m_pReferenceTable->GetColInfo(rgCol[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList+1);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol2), rgCol2), S_OK);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// set a second foreign constraint
	cons.SetConstraintName(L"ADoua");
	
	ReleaseDBID(pConsDesc->rgColumnList, FALSE);
	ReleaseDBID(pConsDesc->rgColumnList+1, FALSE);
	ReleaseDBID(pConsDesc->rgForeignKeyColumnList, FALSE);
	ReleaseDBID(pConsDesc->rgForeignKeyColumnList+1, FALSE);
	
	TESTC_(Table.GetColInfo(rgCol2[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);
	TESTC_(Table.GetColInfo(rgCol2[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList+1);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol2[0], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	TESTC_(m_pReferenceTable->GetColInfo(rgCol2[1], col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList+1);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Create a foreign key on each column of the table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_5()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				indexCol;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make room for columns id
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));

	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));

	for (indexCol = 1; indexCol <= Table.CountColumnsOnTable(); indexCol++)
	{
		if (!CHECK(Table.GetColInfo(indexCol, col), S_OK))
			continue;

		odtLog << col.GetProviderTypeName() << "\n";

		// make the referenced columns unique
		DropConstraintAndCheck(m_pITDWC, &Table2.GetTableID(), col.GetColID());
		if (S_OK != AddUniqueConstraint(&Table2, indexCol))
			continue;

		// build the foreign key constraint, use corresponding columns in tables
		// set column on base table
		ReleaseDBID(pConsDesc->rgColumnList, FALSE);
		DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

		// set foreign column
		if (!CHECK(Table2.GetColInfo(indexCol, col), S_OK))
			continue;
		ReleaseDBID(pConsDesc->rgForeignKeyColumnList, FALSE);
		DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

		// add the foreign key constraint
		if (CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK))
		{
			// drop the constraint
			CHECK(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
		}
	}

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Create a foreign key on all columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_6()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				indexCol;
	DBORDINAL				*rgCol = NULL;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make room for columns id
	pConsDesc->cColumns = Table.CountColumnsOnTable();
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));

	pConsDesc->cForeignKeyColumns = Table.CountColumnsOnTable();
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));

	SAFE_ALLOC(rgCol, DBORDINAL, Table.CountColumnsOnTable());

	for (indexCol = 1; indexCol <= Table.CountColumnsOnTable(); indexCol++)
	{
		rgCol[indexCol-1] = indexCol;
	
		// set column on base table
		TESTC_(Table.GetColInfo(indexCol, col), S_OK);
		DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

		// set foreign column
		TESTC_(Table2.GetColInfo(indexCol, col), S_OK);
		DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);
	}

	// make the referenced columns unique
	TESTC_PROVIDER(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol));

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(rgCol);
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Create foreign key and try to drop the reference table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_7()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1), S_OK);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	CHECK(DropTableAndCheck(&Table2.GetTableID()), DB_E_DROPRESTRICTED);
	CHECK(DropColumnAndCheck(&Table2.GetTableID(), col.GetColID()), DB_E_DROPRESTRICTED);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc On a nullable column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_8()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons		cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};

	// create a new table with a NOT NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_TRUE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_PROVIDER(S_OK == Table.GetColInfo(1, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc On a not nullable column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_9()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons		cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};

	// create a new table with a NOT NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	SetProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);

	// identify a nullable column
	TESTC_PROVIDER(S_OK == Table.GetColInfo(1, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc On a column having a default value
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_10()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons		cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};
	VARIANT				value;
	CList<WCHAR *,WCHAR *>	NativeTypesList;
	CList<DBTYPE,DBTYPE>	ProviderTypesList;

	Table.CreateTypeColInfo(NativeTypesList, ProviderTypesList);
	// create a new table with a NOT NULLABLE column
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	VariantInit(&value);
	GetDefaultValue(&rgNewColumnDesc[0], &value, &Table);
	SetDefaultProperty(&rgNewColumnDesc[0].rgPropertySets, &rgNewColumnDesc[0].cPropertySets,
				&value, DBPROPOPTIONS_REQUIRED);	// might be not supported
	VariantClear(&value);
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);
	
	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);

	TESTC_PROVIDER(S_OK == Table.GetColInfo(1, col));

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc On a column defined as unique
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_11()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCol					col;
	CForeignKeyCons			cons(NULL, m_SupportedMatchType);
	DBCONSTRAINTDESC		*pConsDesc = (DBCONSTRAINTDESC*)cons;
	DBORDINAL				rgCol[] = {1};

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// select a column
	TESTC(Table.CountColumnsOnTable() == Table2.CountColumnsOnTable())

	// build the foreign key constraint, use corresponding columns in tables
	// set column on base table
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, pConsDesc->cColumns*sizeof(pConsDesc->rgColumnList));
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	// set foreign column
	pConsDesc->cForeignKeyColumns = 1;
	SAFE_ALLOC(pConsDesc->rgForeignKeyColumnList, DBID, pConsDesc->cForeignKeyColumns);
	memset(pConsDesc->rgForeignKeyColumnList, 0, pConsDesc->cForeignKeyColumns*sizeof(pConsDesc->rgForeignKeyColumnList));
	TESTC_(Table2.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgForeignKeyColumnList);

	// set referenced table
	cons.SetReferencedTableID(&Table2.GetTableID());

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, NUMELEM(rgCol), rgCol), S_OK);

	// make the base column unique
	TESTC_(AddUniqueConstraint(&Table, NUMELEM(rgCol), rgCol, 0), S_OK);

	// add the foreign key constraint
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc On a primary key column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_12()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL				rgCol[] = {1};

	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, rgCol[0], (DBDEFERRABILITY)0), S_OK);

	// make the base column unique
	TESTC_(AddPrimaryKeyConstraint(&Table, rgCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc On a column that is part of the index
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_13()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL				rgCol[] = {1};

	TESTC_(Table.CreateTable(0, 1), S_OK);
	TESTC_(Table2.CreateTable(0, 1), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1, (DBDEFERRABILITY)0), S_OK);

	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc On a table that is in another catalog
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_14()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CCatalogs			Catalogs(m_pIOpenRowset);
	WCHAR				*pwszCurrentCatalog = NULL;
	WCHAR				*pwszCatalogName	= NULL;
	WCHAR				*pwszSchemaName		= NULL;
	WCHAR				*pwszQualTableName	= NULL;

	TESTC_PROVIDER(SettableProperty(DBPROP_CURRENTCATALOG, DBPROPSET_DATASOURCE, m_pIDBInitialize));
	TESTC_PROVIDER(1 < Catalogs.cCatalogs());

	// store the name of the current catalog
	TESTC(Catalogs.GetCurrentCatalog(&pwszCurrentCatalog));

	// change the current catalog
	TESTC_(Catalogs.ChangeCurrentCatalog(), S_OK);

	// create the referenced table in this catalog
	TESTC_PROVIDER(S_OK == ReferencedTable.CreateTable(0, 0));
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);
	TESTC(GetCatalogSchemaNames(ReferencedTable.GetTableName(), &pwszCatalogName, &pwszSchemaName));

	//Construct a fully Qualified TableName...
	TESTC_(BaseTable.GetQualifiedName(pwszCatalogName, pwszSchemaName, ReferencedTable.GetTableName(), &pwszQualTableName),S_OK);
	ReferencedTable.SetTableName(pwszQualTableName);

	// switch back the current catalog
	TESTC_(Catalogs.SetCurrentCatalog(pwszCurrentCatalog), S_OK);

	// create the base table
	BaseTable.SetTableName(NULL);
	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// create the 
	CHECK(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType), S_OK);

CLEANUP:
	if (pwszCurrentCatalog)
		// restore the name of the current catalog
		COMPARE(Catalogs.SetCurrentCatalog(pwszCurrentCatalog), TRUE);

	SAFE_FREE(pwszCurrentCatalog);
	SAFE_FREE(pwszCatalogName);
	SAFE_FREE(pwszSchemaName);
	SAFE_FREE(pwszQualTableName);
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Use different combination for update/delete rules, as well as match type
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_15()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL				rgCol[] = {1};
	
	DBUPDELRULE				rgUPDLValues[] = {DBUPDELRULE_NOACTION, DBUPDELRULE_CASCADE,
								DBUPDELRULE_SETNULL, DBUPDELRULE_SETDEFAULT};
	WCHAR					*rgUPDLValuesText[] = {L"DBUPDELRULE_NOACTION", L"DBUPDELRULE_CASCADE",
								L"DBUPDELRULE_SETNULL", L"DBUPDELRULE_SETDEFAULT"};
	DBORDINAL				indexDelete;
	DBORDINAL				indexUpdate;
	
	DBMATCHTYPE				rgMatchType[] = {DBMATCHTYPE_NONE, DBMATCHTYPE_FULL, DBMATCHTYPE_PARTIAL};
	WCHAR					*rgMatchTypeText[] = {L"DBMATCHTYPE_NONE", L"DBMATCHTYPE_FULL", L"DBMATCHTYPE_PARTIAL"};
	DBORDINAL				indexMatchType;

	DBORDINAL				cSuccess = 0;
	HRESULT					hr;

	TESTC_(Table.CreateTable(0, 1), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1, (DBDEFERRABILITY)0), S_OK);

	for (indexDelete = 0; indexDelete < NUMELEM(rgUPDLValues); indexDelete++)
	{
		// all delete rules
		odtLog << "\t" << rgUPDLValuesText[indexDelete] << "\n";
		for (indexUpdate = 0; indexUpdate < NUMELEM(rgUPDLValues); indexUpdate++)
		{
			// all update rules
			odtLog << "\t\t" << rgUPDLValuesText[indexUpdate] << "\n";
			for (indexMatchType = 0; indexMatchType < NUMELEM(rgMatchType); indexMatchType++)
			{
				CForeignKeyCons	cons;

				// all match type rules
				odtLog << "\t\t\t" << rgMatchTypeText[indexMatchType];
				
				cons.SetConstraint(&Table, NUMELEM(rgCol), rgCol, 
					&Table2, NUMELEM(rgCol), rgCol, 
					rgMatchType[indexMatchType], 
					rgUPDLValues[indexUpdate], 
					rgUPDLValues[indexDelete], 
					(DBDEFERRABILITY)0);

				hr = AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons);

				if (S_OK == hr)
				{
					odtLog << "\n";
					cSuccess++;
					CHECK(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), 
						cons.GetConstraintID()), S_OK);
				}
				else if (DB_E_BADUPDATEDELETERULE != hr && DB_E_BADMATCHTYPE!= hr)
				{
					CHECK(hr, S_OK);
				}
				else
					odtLog << "\tunsupported " << ((DB_E_BADUPDATEDELETERULE == hr) ? "rule" : "match type") << "\n";
			}
		}
	}

	TESTC(0 < cSuccess);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Referenced table name and base table names have maximum length
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_16()
{ 
	TBEGIN
	CTable					Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable					Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL				rgCol[] = {1};
	WCHAR					*pwszTableName = NULL;

	Table.MakeTableName(NULL);
	pwszTableName = BuildValidName(m_cMaxTableName, Table.GetTableName());

	TESTC_(Table.CreateTable(0, 0, pwszTableName), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1, (DBDEFERRABILITY)0), S_OK);

	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);

CLEANUP:
	SAFE_FREE(pwszTableName);
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Columns in the foreign key (base table and reference table) have different names
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_17()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBCOLUMNDESC		*rgNewColumnDesc = NULL;
	DBORDINAL			rgCol[] = {1};

	// the name of the column should be different
	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgNewColumnDesc);
	ReleaseDBID(&rgNewColumnDesc[0].dbcid, FALSE);
	rgNewColumnDesc[0].dbcid.eKind			= DBKIND_NAME;
	rgNewColumnDesc[0].dbcid.uName.pwszName	= wcsDuplicate(L"AnotherColumn");
	Table.SetColumnDesc(rgNewColumnDesc, m_cColumnDesc);
	Table.SetBuildColumnDesc(FALSE);

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1, (DBDEFERRABILITY)0), S_OK);

	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol), rgCol, m_SupportedMatchType), S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Different size in column lists (base table and referenced)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_18()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgCol[] = {1};
	DBORDINAL			rgCol2[] = {1, 2};

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&Table2, 1, (DBDEFERRABILITY)0), S_OK);

	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol2), rgCol2, m_SupportedMatchType), E_INVALIDARG);
	TESTC_(AddForeignKeyConstraint(&Table, NUMELEM(rgCol), rgCol, &Table2, NUMELEM(rgCol2), rgCol2, m_SupportedMatchType), E_INVALIDARG);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Column types in base table and reference table are different
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_19()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {3};
	CCol				col;
	HRESULT				hr;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// find 2 columns of different types (DBTYPE_I4 and DBTYPE_R4)
	TESTC_PROVIDER(S_OK == ReferencedTable.GetColInfo(col, DBTYPE_I4));
	rgReferencedCol[0] = col.GetColNum();
	TESTC_PROVIDER(S_OK == BaseTable.GetColInfo(col, DBTYPE_R4));
	rgBaseCol[0] = col.GetColNum();

	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	CHECK(hr = AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), E_INVALIDARG);

	if (S_OK == hr)
	{
		// challenge the constraint
		TESTC_(ReferencedTable.Insert(12), S_OK);
		TESTC_(BaseTable.Insert(12), S_OK);
	}

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Invalid value for the update rule
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_20()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	CHECK(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, 0xFF), DB_E_BADUPDATEDELETERULE);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Invalid value for the delete rule
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_21()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	CHECK(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, DBUPDELRULE_NOACTION, 0xFF), DB_E_BADUPDATEDELETERULE);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Invalid value for the MatchType
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_22()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	CHECK(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		0xFF), DB_E_BADMATCHTYPE);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Inexistent column names in the referenced table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_23()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		cons;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(cons.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// release the referenced column and set inexistent name
	SAFE_FREE(((DBCONSTRAINTDESC*)cons)->rgColumnList[0].uName.pwszName);
	((DBCONSTRAINTDESC*)cons)->rgColumnList[0].uName.pwszName = wcsDuplicate(L"NuExista");

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Inexistent column name in the base table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_24()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		cons;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(cons.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// release the referenced column and set inexistent name
	SAFE_FREE(((DBCONSTRAINTDESC*)cons)->rgForeignKeyColumnList[0].uName.pwszName);
	((DBCONSTRAINTDESC*)cons)->rgForeignKeyColumnList[0].uName.pwszName = wcsDuplicate(L"NuExista");

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_BADCOLUMNID);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Inexistent base table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_25()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		cons;

	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// since base table name has to be inexistent, use reference table name instead
	TESTC_(cons.SetConstraint(&ReferencedTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	TESTC_(BaseTable.MakeTableName(NULL), S_OK);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_NOTABLE);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc Inexistent reference table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_26()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		cons;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	
	// since base table name has to be inexistent, use reference table name instead
	TESTC_(cons.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&BaseTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// fill-in an inexistent table name
	TESTC_(ReferencedTable.MakeTableName(NULL), S_OK);
	ReleaseDBID(((DBCONSTRAINTDESC*)cons)->pReferencedTableID, FALSE);
	TESTC_(DuplicateDBID(ReferencedTable.GetTableID(), ((DBCONSTRAINTDESC*)cons)->pReferencedTableID), S_OK);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), DB_E_NOTABLE);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc NULL pConstraintID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_FOREIGNKEY::Variation_27()
{ 
	TBEGIN
	CTable				BaseTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBORDINAL			rgBaseCol[] = {1};
	DBORDINAL			rgReferencedCol[] = {1};
	CForeignKeyCons		cons;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	TESTC_(cons.SetConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol, 
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, m_SupportedMatchType), S_OK);

	// release pConstraintID
	ReleaseDBID(((DBCONSTRAINTDESC*)cons)->pConstraintID, TRUE);
	((DBCONSTRAINTDESC*)cons)->pConstraintID = NULL;

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), S_OK);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_FOREIGNKEY::Terminate()
{ 
	if (m_pReferenceTable)
		m_pReferenceTable->DropTable();
	delete m_pReferenceTable;

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAddConstraint_CHECK)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_CHECK - Testcase for ITableDefinitionWithConstraints::AddConstraint used with DBCONSTRAINTTYPE_UNIQUE
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_CHECK::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Add simple check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_1()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				wszText[] = L" IS NOT NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Add 2 check constraints on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_2()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;
	WCHAR				wszText[]			= L" IS NOT NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// change the name of the constraint helper object
	CHECK(cons.MakeConstraintName(), S_OK);
	
	// create the text of the constraint
	TESTC_(Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Add check constraint for each column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_3()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;
	DBORDINAL			indexCol;
	WCHAR				wszText[]			= L" IS NOT NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	for (indexCol = 0; indexCol < Table.CountColumnsOnTable(); indexCol++)
	{
		// get the first column of the table
		TESTC_(Table.GetColInfo(indexCol+1, col), S_OK);

		odtLog << col.GetColName() << "\n";

		// change the name of the constraint helper object
		cons.SetConstraintName(col.GetColName());
		
		// create the text of the constraint
		SAFE_FREE(pwszColValue);
		SAFE_FREE(pwszConstraintText);
		if (S_OK == Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY))
		{		
			SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
			wcscpy(pwszConstraintText, col.GetColName());
			wcscat(pwszConstraintText, L" < ");
			wcscat(pwszConstraintText, pwszColValue);
		}
		else
		{
			SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
			wcscpy(pwszConstraintText, col.GetColName());
			wcscat(pwszConstraintText, wszText);
		}
		cons.SetConstraintText(pwszConstraintText);
		// add the constraint and check its addition
		CHECK(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	}

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Check constraint is always TRUE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_4()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons(L"TRUE");

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Check constraint is always false
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_5()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons(L"FALSE");

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Long text for the check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_6()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	const DBORDINAL		cTextLen = 1001;
	WCHAR				wszText[cTextLen];

	TESTC_(Table.CreateTable(0, 0), S_OK);

	swprintf(wszText, L"%1000s", L"TRUE");
	cons.SetConstraintText(wszText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Check constraint is made of white characters
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_7()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons(L"  \t");

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Maximum length for the table name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_8()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;
	WCHAR				*pwszTableName		= NULL;

	// set a long table name
	m_pTable->MakeTableName(NULL);
	pwszTableName = BuildValidName(m_cMaxTableName, m_pTable->GetTableName());
	TESTC_(Table.CreateTable(0, 0, pwszTableName), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);

	// create the text of the constraint
	TESTC_(Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	SAFE_FREE(pwszTableName);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Create the constraint, drop it, readd it with different text
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_9()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;
	WCHAR				wszText[]			= L" IS NOT NULL";

	// create a table with an index
	TESTC_(Table.CreateTable(0, 1), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);

	// create the text of the constraint
	TESTC_(Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	
	// set a different text
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Use inexistent column names inthe check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_10()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons(L"InexistentCol < 10");
	CCol				col;

	// create a table with an index
	TESTC_(Table.CreateTable(0, 1), S_OK);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Use qualified table column with different table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_11()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;

	// create a table with an index
	TESTC_(Table.CreateTable(0, 1), S_OK);
	TESTC_(Table2.CreateTable(0, 1), S_OK);

	// get the first column of the table
	TESTC_(Table2.GetColInfo(1 , col), S_OK);

	// create the text of the constraint
	TESTC_(Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(Table2.GetTableName()) + wcslen(col.GetColName()) 
		+ wcslen(pwszColValue) + 4);
	wcscpy(pwszConstraintText, Table2.GetTableName());
	wcscat(pwszConstraintText, L".");
	wcscat(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Set a check condition on an index column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_12()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				*pwszColValue		= NULL;

	// create a table with an index
	TESTC_(Table.CreateTable(0, 1), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);

	// create the text of the constraint
	TESTC_(Table.GetLiteralAndValue(col, &pwszColValue, rand(), col.GetColNum(), PRIMARY), S_OK);
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(pwszColValue) + 3);
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, L" < ");
	wcscat(pwszConstraintText, pwszColValue);
	cons.SetConstraintText(pwszConstraintText);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	SAFE_FREE(pwszColValue);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc pwszConstraintText is NULL
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_13()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	cons.SetConstraintText(NULL);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc pwszConstraintText is an empty string
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_CHECK::Variation_14()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons(L"");

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_CHECK::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCAddConstraint_Deferrability)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_Deferrability - Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_Deferrability::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 





// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Immediate unique constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_1()
{ 
	TBEGIN
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(Table.CreateTable(3, 0), S_OK);

	// create an immediate unique constraint
	TESTC_(AddUniqueConstraint(&Table, 1, (DBDEFERRABILITY)0), S_OK);
	
	// check that adding row with seed 2 again will fail
	TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Defferable unique constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_2()
{ 
	TBEGIN
	HRESULT	hr;
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(Table.CreateTable(3, 0), S_OK);

	// create a deferrable unique constraint
	TEST2C_(hr = AddUniqueConstraint(&Table, 1, DBDEFERRABILITY_DEFERRABLE), S_OK, DB_E_BADDEFERRABILITY);
	
	// check that adding row with seed 2 again will fail
	if (S_OK == hr)
	{
		TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);
	}
	else
		odtLog << "Deferrable unique constraints are not supported\n";

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Deffered unique constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_3()
{ 
	TBEGIN
	HRESULT	hr;
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(Table.CreateTable(3, 0), S_OK);

	// create an deferred unique constraint
	TEST2C_(hr = AddUniqueConstraint(&Table, 1, 
		DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED), S_OK, DB_E_BADDEFERRABILITY);
	
	// check that adding row with seed 2 again will fail
	if (S_OK == hr)
	{
		TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);
	}
	else
		odtLog << "Deferred unique constraints are not supported\n";

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Bad deferrability value unique constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_4()
{ 
	TBEGIN
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(Table.CreateTable(0), S_OK);

	// create a unique constraint
	TESTC_(AddUniqueConstraint(&Table, 1, 
		~(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED)), DB_E_BADDEFERRABILITY);
	
CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Immediate primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_5()
{ 
	TBEGIN
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(CreateTableWithNoNullableColumns(&Table, 3), S_OK);

	// create an immediate unique constraint
	TESTC_(AddPrimaryKeyConstraint(&Table, 1, (DBDEFERRABILITY)0), S_OK);
	
	// check that adding row with seed 2 again will fail
	TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Deferrable primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_6()
{ 
	TBEGIN
	HRESULT	hr;
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(CreateTableWithNoNullableColumns(&Table, 3), S_OK);

	// create a deferrable unique constraint
	TEST2C_(hr = AddPrimaryKeyConstraint(&Table, 1, DBDEFERRABILITY_DEFERRABLE), S_OK, DB_E_BADDEFERRABILITY);
	
	// check that adding row with seed 2 again will fail
	if (S_OK == hr)
	{
		TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);
	}
	else
		odtLog << "Deferrable primary key constraints are not supported\n";

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Deferred primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_7()
{ 
	TBEGIN
	HRESULT	hr;
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(CreateTableWithNoNullableColumns(&Table, 3), S_OK);

	// create an deferred primary key constraint
	TEST2C_(hr = AddPrimaryKeyConstraint(&Table, 1, 
		DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED), S_OK, DB_E_BADDEFERRABILITY);
	
	// check that adding row with seed 2 again will fail
	if (S_OK == hr)
	{
		TESTC_(Table.Insert(2), DB_E_INTEGRITYVIOLATION);
	}
	else
		odtLog << "Deferred primary key constraints are not supported\n";

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Bad deferrability value primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_8()
{ 
	TBEGIN
	CTable	Table(m_pITDWC, (LPWSTR)gwszModuleName);

	// create a table
	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// create a primary key  constraint
	TESTC_(AddPrimaryKeyConstraint(&Table, 1, 
		~(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED)), DB_E_BADDEFERRABILITY);
	
CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Immediate foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_9()
{ 
	TBEGIN
	CTable		BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable		ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL	rgBaseCol[]			= {1};
	DBORDINAL	rgReferencedCol[]	= {1};

	// create a table
	TESTC_(BaseTable.CreateTable(0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);

	// create a unique constraint in the refereced table
	TESTC_PROVIDER(S_OK == AddUniqueConstraint(&ReferencedTable, 1));

	// create an immediate foreign key constraint
	TESTC_(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol,
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, DBUPDELRULE_NOACTION, DBUPDELRULE_NOACTION,
		(DBDEFERRABILITY)0), S_OK);
	
	// check that adding row with seed 2 again will fail
	TESTC_(BaseTable.Insert(2), DB_E_INTEGRITYVIOLATION);

	// check adding row in the referenced table is ok
	TESTC_(ReferencedTable.Insert(2), S_OK);
	TESTC_(BaseTable.Insert(2), S_OK);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Deferrable foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_10()
{ 
	TBEGIN
	HRESULT		hr;
	CTable		BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable		ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL	rgBaseCol[]			= {1};
	DBORDINAL	rgReferencedCol[]	= {1};

	// create a table
	TESTC_(BaseTable.CreateTable(0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);

	// create a unique constraint in the refereced table
	TESTC_PROVIDER(S_OK == AddUniqueConstraint(&ReferencedTable, 1));

	// create a deferrable foreign key constraint
	TEST2C_(hr = AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol,
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, DBUPDELRULE_NOACTION, DBUPDELRULE_NOACTION,
		DBDEFERRABILITY_DEFERRABLE), S_OK, DB_E_BADDEFERRABILITY);

	if (S_OK == hr)
	{
		// check that adding row with seed 2 again will fail
		TESTC_(BaseTable.Insert(2), DB_E_INTEGRITYVIOLATION);

		// check adding row in the referenced table is ok
		TESTC_(ReferencedTable.Insert(2), S_OK);
		TESTC_(BaseTable.Insert(2), S_OK);
	}
	else
		odtLog << "Deferrable foreign key not supported\n";

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Deferred foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_11()
{ 
	TBEGIN
	HRESULT		hr;
	CTable		BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable		ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL	rgBaseCol[]			= {1};
	DBORDINAL	rgReferencedCol[]	= {1};

	// create a table
	TESTC_(BaseTable.CreateTable(0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);

	// create a unique constraint in the refereced table
	TESTC_PROVIDER(S_OK == AddUniqueConstraint(&ReferencedTable, 1));

	// create a deferrable foreign key constraint
	TEST2C_(hr = AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol,
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, DBUPDELRULE_NOACTION, DBUPDELRULE_NOACTION,
		DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED), 
		S_OK, DB_E_BADDEFERRABILITY);
	
	if (S_OK == hr)
	{
		// check that adding row with seed 2 again will fail
		TESTC_(BaseTable.Insert(2), S_OK);

		// check adding row in the referenced table is ok
		TESTC_(ReferencedTable.Insert(2), S_OK);
		TESTC_(BaseTable.Insert(2), S_OK);
	}
	else
		odtLog << "Deferred foreign key not supported\n";

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Bad deferrability value foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_12()
{ 
	TBEGIN
	CTable		BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable		ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBORDINAL	rgBaseCol[]			= {1};
	DBORDINAL	rgReferencedCol[]	= {1};

	// create a table
	TESTC_(BaseTable.CreateTable(0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);

	// create a unique constraint in the refereced table
	TESTC_PROVIDER(S_OK == AddUniqueConstraint(&ReferencedTable, 1));

	// create an immediate foreign key constraint
	TESTC_(AddForeignKeyConstraint(&BaseTable, NUMELEM(rgBaseCol), rgBaseCol,
		&ReferencedTable, NUMELEM(rgReferencedCol), rgReferencedCol, 
		m_SupportedMatchType, DBUPDELRULE_NOACTION, DBUPDELRULE_NOACTION,
		~(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED)), DB_E_BADDEFERRABILITY);
	
	// check adding row in the referenced table is ok
	TESTC_(ReferencedTable.Insert(2), S_OK);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Immediate check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_13()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons			cons;
	CCol				col;
	WCHAR				*pwszConstraintText = NULL;
	WCHAR				wszText[] = L" IS NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	cons.SetDeferrability((DBDEFERRABILITY)0);

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// add a row that does not have NULL in the checked column
	TESTC_PROVIDER(2 < Table.CountColumnsOnTable());
	TESTC_(Table.Insert(3), DB_E_INTEGRITYVIOLATION);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Deferrable check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_14()
{ 
	TBEGIN
	HRESULT			hr;
	CTable			Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons		cons;
	CCol			col;
	WCHAR			*pwszConstraintText = NULL;
	WCHAR			wszText[] = L" IS NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	cons.SetDeferrability(DBDEFERRABILITY_DEFERRABLE);

	// add the constraint and check its addition
	TEST2C_(hr = AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK, DB_E_BADDEFERRABILITY);

	if (S_OK == hr)
	{
	// add a row that does not have NULL in the checked column
	TESTC_PROVIDER(2 < Table.CountColumnsOnTable());
	TESTC_(Table.Insert(3), DB_E_INTEGRITYVIOLATION);
	}
	else
		odtLog << "Deferrable check constraints are not supported\n";

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Deferred check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_15()
{ 
	TBEGIN
	HRESULT			hr;
	CTable			Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons		cons;
	CCol			col;
	WCHAR			*pwszConstraintText = NULL;
	WCHAR			wszText[] = L" IS NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	cons.SetDeferrability(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED);

	// add the constraint and check its addition
	TEST2C_(hr = AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK, DB_E_BADDEFERRABILITY);

	if (S_OK == hr)
	{
		// add a row that does not have NULL in the checked column
		TESTC_(Table.Insert(3), S_OK);
	}
	else
		odtLog << "Deferred check constraints are not supported\n";

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Bad deferrability value check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Deferrability::Variation_16()
{ 
	TBEGIN
	CTable			Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons		cons;
	CCol			col;
	WCHAR			*pwszConstraintText = NULL;
	WCHAR			wszText[] = L" IS NULL";

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// get the first column of the table
	TESTC_(Table.GetColInfo(1 , col), S_OK);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColName()) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColName());
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	cons.SetDeferrability(~(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED));

	// add the constraint and check its addition
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADDEFERRABILITY);

CLEANUP:
	SAFE_FREE(pwszConstraintText);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_Deferrability::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCreateTableWithConstraints_NoConstraint)
//*-----------------------------------------------------------------------
//| Test Case:		TCCreateTableWithConstraints_NoConstraint - Testcase for ITableDefinitionWithConstraints::AddConstraint used with deferrability setting
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCreateTableWithConstraints_NoConstraint::Init()
{ 
	ITableDefinitionWithConstraints	*pITableDefinitionWithConstraints	= NULL;

	if(TCCreateTable::Init())
	{
		// check for ITableDefinition
		if (!(m_hr=VerifyInterface(m_pIOpenRowset, IID_ITableDefinitionWithConstraints, SESSION_INTERFACE, (IUnknown**)&pITableDefinitionWithConstraints)))
		{
			odtLog << "Error: ITableDefinitionWithConstraints is not supported\n";
			return TEST_SKIPPED;
		}

		SAFE_RELEASE(pITableDefinitionWithConstraints);
		return TRUE;
	}
	return FALSE;
} 



//---------------------------------------------------------------------
//
// @member Create the table using ITableDefinition::CreateTable and 
// check its creation
//---------------------------------------------------------------------
HRESULT	TCCreateTableWithConstraints_NoConstraint::CreateAndCheckTable(
	ITableDefinition	*pITableDefinition,	// [in] pointer to the ITableDefinition interface to be used
	IUnknown			*pUnkOuter,			// [in] pointer to controlling unknown
	DBID				*pTableID,			// [in]	pointer to the input table ID
	ULONG				cColumnDescs,		// [in]	the number of columns to be created
	DBCOLUMNDESC		*rgColumnDescs,		// [in/out] array of columns to be created
	REFIID				riid,				// [in] rowset interface to be returned
	ULONG				cPropertySets,		// [in] number of roset property sets asked
	DBPROPSET			*rgPropertySets,	// [in/out] array of rowset property sets
	DBID				**ppTableID,		// [out] returned table ID
	IUnknown			**ppRowset			// [out] pointer to outer interface
)
{
	HRESULT								hr		= E_FAIL;
	ITableDefinitionWithConstraints		*pITDWC = NULL;

	TESTC(NULL != pITableDefinition);
	TESTC(VerifyInterface(pITableDefinition, IID_ITableDefinitionWithConstraints, 
		SESSION_INTERFACE, (IUnknown**)&pITDWC));

	hr = CreateAndCheckTableWithConstraints(pITDWC, pUnkOuter, pTableID,
		cColumnDescs, rgColumnDescs, 0, NULL, 
		riid, cPropertySets, rgPropertySets,
		ppTableID, ppRowset);

CLEANUP:
	SAFE_RELEASE(pITDWC);
	return hr;
} //TCCreateTableWithConstraints_NoConstraint::CreateAndCheckTable



						
// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCCreateTableWithConstraints_NoConstraint::Terminate()
{ 
	return(TCCreateTable::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCreateTableWithConstraints)
//*-----------------------------------------------------------------------
//| Test Case:		TCCreateTableWithConstraints - Testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints when constraints are set
//| Created:  	7/29/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCreateTableWithConstraints::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Create a table and multiple constraint of various types
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_1()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Table with a unique constraint (on a single column)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_2()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[0].dbcid);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Table with a unique constraint (on multiple columns)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_3()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[0].dbcid);
	consUnique.AddColumn(&rgColumnDesc[3].dbcid);
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Table with a primary key constraint (on a single column)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_4()
{ 
	TBEGIN
	DBID				TableID;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CPrimaryKeyCons		consPK;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a primary key constraint on the second column
	consPK.AddColumn(&rgColumnDesc[0].dbcid);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consPK, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Table with a primary key constraint (on multiple columns)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_5()
{ 
	TBEGIN
	DBID				TableID;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CPrimaryKeyCons		consPK;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a primary key constraint on the second column
	consPK.AddColumn(&rgColumnDesc[1].dbcid);
	consPK.AddColumn(&rgColumnDesc[3].dbcid);
	consPK.AddColumn(&rgColumnDesc[2].dbcid);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consPK, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Table with a foreign key constraint (single column)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_6()
{ 
	TBEGIN
	DBID				TableID;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBORDINAL			indexCol		= 2;
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consFK(NULL, m_SupportedMatchType);
	CCol				col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, indexCol, (DBDEFERRABILITY)0), S_OK);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(indexCol, col), S_OK);
	consFK.AddColumn(col.GetColID());
	consFK.AddForeignKeyColumn(&rgColumnDesc[indexCol-1].dbcid);
	consFK.SetReferencedTableID(&ReferencedTable.GetTableID());
	TESTC_(consFK.MakeConstraintName(), S_OK);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consFK, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Table with foreign key constraint defined on multiple keys
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_7()
{ 
	TBEGIN
	DBID				TableID;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBORDINAL			rgColumns[]		= {2, 1};
	CTable				ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consFK(NULL, m_SupportedMatchType);
	CCol				col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, NUMELEM(rgColumns), rgColumns, (DBDEFERRABILITY)0), S_OK);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(rgColumns[0], col), S_OK);
	consFK.AddColumn(col.GetColID());
	TESTC_(ReferencedTable.GetColInfo(rgColumns[1], col), S_OK);
	consFK.AddColumn(col.GetColID());

	consFK.AddForeignKeyColumn(&rgColumnDesc[rgColumns[0]-1].dbcid);
	consFK.AddForeignKeyColumn(&rgColumnDesc[rgColumns[1]-1].dbcid);

	consFK.SetReferencedTableID(&ReferencedTable.GetTableID());
	TESTC_(consFK.MakeConstraintName(), S_OK);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consFK, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Table with a simple check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_8()
{ 
	TBEGIN
	DBID				TableID;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CCheckCons			cons;
	WCHAR				*pwszConstraintText	= NULL;
	WCHAR				wszText[] = L" IS NOT NULL";

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// build constraint text
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(rgColumnDesc[0].dbcid.uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, rgColumnDesc[0].dbcid.uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, cons, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	SAFE_FREE(pwszConstraintText);
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Table with multiple unique constraints defined
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_9()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CUniqueCons		consUnique1;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	consUnique.MakeConstraintName();

	// set second unique constraints on the firstcolumn
	consUnique1.AddColumn(&rgColumnDesc[0].dbcid);
	consUnique1.MakeConstraintName();

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consUnique1);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Table with unique constraint and foreign key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_10()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	consUnique.MakeConstraintName();

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	consForeignKey.MakeConstraintName();

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consUnique);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Table with primary key and check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_11()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CPrimaryKeyCons	consPK;
	CCheckCons		consCheck;
	WCHAR			*pwszConstraintText	= NULL;
	WCHAR			wszText[] = L" IS NOT NULL";


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);
	SetProperty(&rgColumnDesc[1].rgPropertySets, &rgColumnDesc[1].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// set a primary key constraint on the first column
	consPK.AddColumn(&rgColumnDesc[0].dbcid);

	// build constraint text
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[1].dbcid.eKind);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(rgColumnDesc[1].dbcid.uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, rgColumnDesc[1].dbcid.uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	consCheck.SetConstraintText(pwszConstraintText);

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consPK);
	ConsDescArray.AddConsDesc(consCheck);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	SAFE_FREE(pwszConstraintText);
	CHECK(DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Deferrable, immediate constraints
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_12()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	CCheckCons		consCheck;
	WCHAR			*pwszConstraintText	= NULL;
	WCHAR			wszText[] = L" IS NULL";
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	HRESULT			hr;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	consUnique.SetDeferrability(DBDEFERRABILITY_DEFERRABLE);

	// try to create a table with a deferrable unique constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consUnique, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	consPrimaryKey.SetDeferrability(DBDEFERRABILITY_DEFERRABLE);

	// try to create a table with a deferrable primary key constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	consForeignKey.SetDeferrability(DBDEFERRABILITY_DEFERRABLE);

	// try to create a table with a deferrable foreign key constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consForeignKey, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// build constraint text
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[4].dbcid.eKind);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(rgColumnDesc[4].dbcid.uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, rgColumnDesc[4].dbcid.uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	consCheck.SetConstraintText(pwszConstraintText);
	consCheck.SetDeferrability(DBDEFERRABILITY_DEFERRABLE);

	// try to create a table with a deferrable check constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consCheck, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}
	
	
CLEANUP:
	SAFE_FREE(pwszConstraintText);
	DropTableAndCheck(&TableID);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Deferred constraints
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints::Variation_13()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	CCheckCons		consCheck;
	WCHAR			*pwszConstraintText	= NULL;
	WCHAR			wszText[] = L" IS NULL";
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	HRESULT			hr;
	CTable			Table(m_pITableDefinition, (LPWSTR)gwszModuleName);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	consUnique.SetDeferrability(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED);

	// try to create a table with a deferrable unique constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consUnique, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// try to insert a duplicate value in the second column and check it is possible
		Table.SetTableID(TableID);
		if (CHECK(Table.GetTableColumnInfo(&TableID), S_OK))
		{
			CHECK(Table.Insert(1), S_OK);
			CHECK(Table.Insert(1), S_OK);
		}
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	consPrimaryKey.SetDeferrability(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED);

	// try to create a table with a deferrable primary key constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// try to insert a duplicate value in the primary key column and check it is possible
		Table.SetTableID(TableID);
		if (CHECK(Table.GetTableColumnInfo(&TableID), S_OK))
		{
			CHECK(Table.Insert(1), S_OK);
			CHECK(Table.Insert(1), S_OK);
		}
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	consForeignKey.SetDeferrability(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED);

	// try to create a table with a deferrable foreign key constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consForeignKey, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// try to insert a row base table (referenced table is empty)
		Table.SetTableID(TableID);
		if (CHECK(Table.GetTableColumnInfo(&TableID), S_OK))
		{
			CHECK(Table.Insert(7), S_OK);
			CHECK(Table.Insert(5), S_OK);
		}
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}

	// build constraint text
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[4].dbcid.eKind);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(rgColumnDesc[4].dbcid.uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, rgColumnDesc[4].dbcid.uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	consCheck.SetConstraintText(pwszConstraintText);
	consCheck.SetDeferrability(DBDEFERRABILITY_DEFERRABLE | DBDEFERRABILITY_DEFERRED);

	// try to create a table with a deferrable check constraint
	hr = CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consCheck, IID_IRowset, 0, NULL, NULL, NULL);

	if (S_OK == hr)
	{
		// try to insert a not null value in the checked column 
		Table.SetTableID(TableID);
		if (CHECK(Table.GetTableColumnInfo(&TableID), S_OK))
		{
			CHECK(Table.Insert(1), S_OK);
			CHECK(Table.Insert(10), S_OK);
		}
		// drop the table
		CHECK(DropTableAndCheck(&TableID, m_pITableDefinition), S_OK);
	}
	else
	{
		CHECK(hr, DB_E_BADDEFERRABILITY);
	}
	
	
CLEANUP:
	SAFE_FREE(pwszConstraintText);
	DropTableAndCheck(&TableID);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCCreateTableWithConstraints::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCreateTableWithConstraints_Boundary)
//*-----------------------------------------------------------------------
//| Test Case:		TCCreateTableWithConstraints_Boundary - Boundary and NULL testcase for ITableDefinitionWithConstraints::CreateTableWithConstraints
//| Created:  	9/3/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCreateTableWithConstraints_Boundary::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc 0 < cConstraintDescs and NULL == rgConstraintDescs => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_1()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		3, NULL, 
		IID_IRowset, 0, NULL, NULL, NULL), E_INVALIDARG);
	
CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc 0 == cConstraintDescs and NULL != rgConstraintDescs => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_2()
{ 
	TBEGIN
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		cons;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	
	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	cons.AddColumn(&rgColumnDesc[0].dbcid);
	
	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		0, cons, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(this->DropTableAndCheck(&TableID), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL ppTableID is not NULL => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_3()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID			*pTableID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);
	
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(pTableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Bad DBKIND of a constraint ID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_4()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID			*pTableID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	ReleaseDBID(((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID, FALSE);
	((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID->eKind = 0xb2;

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);
	
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc NULL pConstraintID in a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_5()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID			*pTableID		= NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	ReleaseDBID(((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID, TRUE);
	((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID = NULL;

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);
	
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(pTableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc NULL column list in a constraint, column size is not 0 => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_6()
{ 
	// this tests rgColumnList for PK, FK and unique constraints
	// as well as rgForeignKeyColumnList for FK
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID				*pTableID		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CConsDescArray		ConsDescArray;
	CUniqueCons			consUnique;
	CUniqueCons			cons;
	CPrimaryKeyCons		consPrimaryKey;
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL			rgReferencedCol[] = {1};
	CCol				col;
	DBCONSTRAINTDESC	*pConsDesc;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	cons.AddColumn(&rgColumnDesc[0].dbcid);
	pConsDesc = consUnique;
	pConsDesc->cColumns		= 2;
	pConsDesc->rgColumnList	= NULL;
	ConsDescArray.AddConsDesc(cons);
	ConsDescArray.AddConsDesc(consUnique);
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), E_INVALIDARG);


	// set a primary key on the first column of the table
	pConsDesc = consPrimaryKey;
	pConsDesc->cColumns		= 5;
	pConsDesc->rgColumnList	= NULL;
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), E_INVALIDARG);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	pConsDesc = consForeignKey;
	pConsDesc->cColumns		= 1;
	pConsDesc->rgColumnList	= NULL;
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), E_INVALIDARG);

	// the same check for foreign key column list
	consForeignKey.ReleaseForeignKeyColumnList();
	pConsDesc->cForeignKeyColumns = 1;
	pConsDesc->cColumns = 0;
	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), E_INVALIDARG);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Empty list of columns for a unique, primay key or foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_7()
{ 
	// this tests rgColumnList for PK, FK and unique constraints
	// as well as rgForeignKeyColumnList for FK
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID				*pTableID		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CConsDescArray		ConsDescArray;
	CUniqueCons			consUnique;
	CUniqueCons			cons;
	CPrimaryKeyCons		consPrimaryKey;
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL			rgReferencedCol[] = {1};
	CCol				col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	cons.AddColumn(&rgColumnDesc[0].dbcid);
	ConsDescArray.AddConsDesc(cons);
	ConsDescArray.AddConsDesc(consUnique);
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);


	// set a primary key on the first column of the table
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

	// the same check for foreign key column list
	consForeignKey.ReleaseForeignKeyColumnList();
	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Non empty list of columns for a check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_8()
{
	// this tests the case when rgColumnList or rgForeignKeyColumnList 
	// are not empty in a check constraint
	TBEGIN
	CCheckCons			consCheck;
	DBCONSTRAINTDESC	*pcons = consCheck;
	WCHAR				*pwszConstraintText	= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	WCHAR				wszText[]		= L" IS NOT NULL";

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	// create the text of the constraint
	SAFE_FREE(pwszConstraintText);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(rgColumnDesc[0].dbcid.uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, rgColumnDesc[0].dbcid.uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	consCheck.SetConstraintText(pwszConstraintText);

	// not empty rgColumnList
	pcons->cColumns = 1;
	pcons->rgColumnList = &rgColumnDesc[0].dbcid;
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consCheck, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

	// not empty rgForeignKeyColumnList
	pcons->cColumns = 0;
	pcons->rgColumnList = NULL;
	pcons->cForeignKeyColumns = 1;
	pcons->rgForeignKeyColumnList = &rgColumnDesc[0].dbcid;
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consCheck, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

	// not empty rgColumnList and rgForeignKeyColumnList
	pcons->cColumns = 1;
	pcons->rgColumnList = &rgColumnDesc[0].dbcid;
	pcons->cForeignKeyColumns = 1;
	pcons->rgForeignKeyColumnList = &rgColumnDesc[0].dbcid;
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consCheck, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	pcons->cColumns = 0;
	pcons->rgColumnList = NULL;
	pcons->cForeignKeyColumns = 0;
	pcons->rgForeignKeyColumnList = NULL;
	SAFE_FREE(pwszConstraintText);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Non empty list of foreign keys for a constraint that is not foreign key
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_9()
{ 
	TBEGIN
	CForeignKeyCons		consFK;
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	const DBORDINAL		cColumns		= 2;
	DBID				rgColumnList[cColumns];
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	rgColumnList[0].eKind				= DBKIND_NAME;
	rgColumnList[0].uName.pwszName	= (LPWSTR)0x01234567;
	rgColumnList[1].eKind				= 0xfe;
	rgColumnList[1].uName.pwszName	= (LPWSTR)0x01234567;
	
	// set up the unique constraint
	consUnique.AddColumn(&rgColumnDesc[0].dbcid);
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consUnique)->rgForeignKeyColumnList = rgColumnList;
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consUnique)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consUnique)->rgForeignKeyColumnList = NULL;

	// set up the primary key constraint
	consPK.AddColumn(&rgColumnDesc[0].dbcid);
	consPK.AddColumn(&rgColumnDesc[1].dbcid);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consPK)->rgForeignKeyColumnList = rgColumnList;
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPK, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consPK)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consPK)->rgForeignKeyColumnList = NULL;

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = cColumns;
	((DBCONSTRAINTDESC*)consCheck)->rgForeignKeyColumnList = rgColumnList;
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consCheck, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consCheck)->cForeignKeyColumns = 0;
	((DBCONSTRAINTDESC*)consCheck)->rgForeignKeyColumnList = NULL;

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc 0 < cForeignKeyColumns and NULL == rgForeignKeyColumnList => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_10()
{ 
	// this tests rgColumnList for PK, FK and unique constraints
	// as well as rgForeignKeyColumnList for FK
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	DBID				*pTableID		= NULL;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	CUniqueCons			cons;
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL			rgReferencedCol[] = {1};

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a simple foreign key constraint
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	consForeignKey.ReleaseForeignKeyColumnList();
	((DBCONSTRAINTDESC*)consForeignKey)->cForeignKeyColumns = 1;
	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), E_INVALIDARG);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Constraint is not foreign key and pReferencedTableID is not NULL
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_11()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CUniqueCons			consUnique;
	CPrimaryKeyCons		consPK;
	CCheckCons			consCheck;
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the unique constraint
	consUnique.AddColumn(&rgColumnDesc[0].dbcid);
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	((DBCONSTRAINTDESC*)consUnique)->pReferencedTableID = &ReferencedTable.GetTableID();
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consUnique)->pReferencedTableID = NULL;

	// set up the primary key constraint
	consPK.AddColumn(&rgColumnDesc[0].dbcid);
	consPK.AddColumn(&rgColumnDesc[1].dbcid);
	((DBCONSTRAINTDESC*)consPK)->pReferencedTableID = &ReferencedTable.GetTableID();
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPK, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consPK)->pReferencedTableID = NULL;

	// set up the check constraint
	consCheck.SetConstraintText(L"TRUE != FALSE");
	((DBCONSTRAINTDESC*)consCheck)->pReferencedTableID = &ReferencedTable.GetTableID();
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consCheck, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);
	((DBCONSTRAINTDESC*)consCheck)->pReferencedTableID = NULL;

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc NULL pReferencedTableID for a foreign key constraint (with and without cForeignKeyColumns being 0)
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_12()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID(NULL);

	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	CHECK(consForeignKey.MakeConstraintName(), S_OK);
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTFORM);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc NULL referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_13()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;
	DBID				ReferencedTableID;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= NULL;
	consForeignKey.SetReferencedTableID(&ReferencedTableID);

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADTABLEID);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc DB_NULLID *pReferencedTableID for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_14()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID((DBID*)&DB_NULLID);

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADTABLEID);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Empty string referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_15()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;
	DBID				ReferencedTableID;

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= L"";
	consForeignKey.SetReferencedTableID(&ReferencedTableID);

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADTABLEID);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Maximum length referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_16()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;
	WCHAR				*pwszTableName	= NULL;
	CUniqueCons			cons;

	// create support for foreign key constraint
	ReferencedTable.MakeTableName(NULL);
	pwszTableName = BuildValidName(m_cMaxTableName, ReferencedTable.GetTableName());
	TESTC_(ReferencedTable.CreateTable(0, 0, pwszTableName), S_OK);
	CHECK(AddUniqueConstraint(&ReferencedTable, 1), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), S_OK);

CLEANUP:
	CHECK(DropTableAndCheck(pTableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	SAFE_FREE(pwszTableName);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Referenced table name longer than allowed for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_17()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;
	DBID				ReferencedTableID;

	// create support for foreign key constraint
	ReferencedTable.MakeTableName(NULL);
	ReferencedTableID.eKind				= DBKIND_NAME;
	ReferencedTableID.uName.pwszName	= BuildValidName(m_cMaxTableName+1, ReferencedTable.GetTableName());
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID(&ReferencedTableID);

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	TEST2C_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADTABLEID, DB_E_NOTABLE);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	ReleaseDBID(&ReferencedTableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Illegal characters in the referenced table name for a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_18()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	CCol				col;
	DBID				ReferencedTableID;
	DBORDINAL			m, n;

	// create support for foreign key constraint
	ReferencedTable.MakeTableName(NULL);
	ReferencedTableID.eKind				= DBKIND_NAME;
	m = min(m_cMaxTableName, n=5+wcslen(ReferencedTable.GetTableName()));
	ReferencedTableID.uName.pwszName	= BuildInvalidName(m, ReferencedTable.GetTableName(), m_pwszInvalidTableChars);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID(&ReferencedTableID);

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddForeignKeyColumn(col.GetColID());
	
	TEST2C_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADTABLEID, DB_E_NOTABLE);

CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	ReleaseDBID(&ReferencedTableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Bad constraint type
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_19()
{ 
	TBEGIN
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;
	DBCONSTRAINTDESC	ConsDesc;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;

	// set up the check constraint
	memset(&ConsDesc, 0, sizeof(DBCONSTRAINTDESC));
	ConsDesc.ConstraintType = 0x4b;

	TESTC_(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, &ConsDesc, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_BADCONSTRAINTTYPE);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Inexistent referenced table
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_20()
{ 
	TBEGIN
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		consForeignKey(NULL, m_SupportedMatchType);
	DBCOLUMNDESC		*rgColumnDesc	= NULL;
	ULONG				cColumnDesc		= 0;
	DBID				*pTableID		= NULL;

	// create support for foreign key constraint
	ReferencedTable.MakeTableName(NULL);

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind
		&& rgColumnDesc[0].dbcid.uName.pwszName);

	// set up the check constraint
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	consForeignKey.AddColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	
	CHECK(CreateAndCheckTableWithConstraints(
		m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, 
		&pTableID, NULL), DB_E_NOTABLE);

CLEANUP:
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	ReleaseDBID(pTableID, TRUE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Empty string for a column name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_21()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	DBID			colid;

	colid.eKind				= DBKIND_NAME;
	colid.uName.pwszName	= L"";

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&colid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, NULL, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&colid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, NULL, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&colid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, NULL, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Maximum length column name in a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_22()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	WCHAR			*pwszColName = NULL;


	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// build the column name
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind);
	pwszColName = rgColumnDesc[0].dbcid.uName.pwszName;
	rgColumnDesc[0].dbcid.uName.pwszName	= BuildValidName(m_cMaxColName, pwszColName);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[0].dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), S_OK);
	CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID, TRUE);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), S_OK);
	CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID, TRUE);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), S_OK);
	CHECK(DropTableAndCheck(pTableID), S_OK);
	ReleaseDBID(pTableID, TRUE);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	SAFE_FREE(pwszColName);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Invalid DBKIND for a column DBID of a constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_23()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	DBID			dbcid;

	dbcid.eKind = 0x3c;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	((DBCONSTRAINTDESC*)consUnique)->cColumns		= 1;
	((DBCONSTRAINTDESC*)consUnique)->rgColumnList	= &dbcid;
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	((DBCONSTRAINTDESC*)consUnique)->cColumns		= 0;
	((DBCONSTRAINTDESC*)consUnique)->rgColumnList	= NULL;

	// set a primary key on the first column of the table
	((DBCONSTRAINTDESC*)consPrimaryKey)->cColumns		= 1;
	((DBCONSTRAINTDESC*)consPrimaryKey)->rgColumnList	= &dbcid;
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	((DBCONSTRAINTDESC*)consUnique)->cColumns		= 0;
	((DBCONSTRAINTDESC*)consUnique)->rgColumnList	= NULL;

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	((DBCONSTRAINTDESC*)consForeignKey)->cForeignKeyColumns		= 1;
	((DBCONSTRAINTDESC*)consForeignKey)->rgForeignKeyColumnList	= &dbcid;
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	((DBCONSTRAINTDESC*)consForeignKey)->cForeignKeyColumns		= 0;
	((DBCONSTRAINTDESC*)consForeignKey)->rgForeignKeyColumnList	= NULL;
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Column ID is DB_NULLID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_24()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	WCHAR			*pwszColName = NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// build the column name
	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind);
	pwszColName = rgColumnDesc[0].dbcid.uName.pwszName;
	rgColumnDesc[0].dbcid.uName.pwszName	= BuildValidName(m_cMaxColName, pwszColName);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn((DBID*)&DB_NULLID);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn((DBID*)&DB_NULLID);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn((DBID*)&DB_NULLID);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	SAFE_FREE(pwszColName);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Inexistent column => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_25()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc-1;
	TESTC_PROVIDER(1 < m_cColumnDesc);

	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[cColumnDesc].dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[cColumnDesc].dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[cColumnDesc].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	cColumnDesc = m_cColumnDesc;
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc NULL column name => DB_E_BADCOLUMNID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_26()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	DBID			dbcid;

	dbcid.eKind				= DBKIND_NAME;
	dbcid.uName.pwszName	= NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc Column name exceeds maximum length name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_27()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	DBID			dbcid;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind);
	dbcid.eKind				= DBKIND_NAME;
	dbcid.uName.pwszName	= BuildValidName(m_cMaxColName+1, rgColumnDesc[0].dbcid.uName.pwszName);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc Invalid column name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_28()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			*pTableID = NULL;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;
	DBID			dbcid;
	DBORDINAL		m, n;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	TESTC_PROVIDER(DBKIND_NAME == rgColumnDesc[0].dbcid.eKind);
	m = min(m_cMaxColName, n=5+wcslen(rgColumnDesc[0].dbcid.uName.pwszName));
	dbcid.eKind				= DBKIND_NAME;
	dbcid.uName.pwszName	= BuildInvalidName(m, rgColumnDesc[0].dbcid.uName.pwszName, m_pwszInvalidColChars);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consUnique, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&dbcid);
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consPrimaryKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());
	CHECK(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, NULL, 
		cColumnDesc, rgColumnDesc, 
		1, consForeignKey, 
		IID_IRowset, 0, NULL, &pTableID, NULL), DB_E_BADCOLUMNID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc Existent table name => DB_E_DUPLICATETABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_29()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), DB_E_DUPLICATETABLEID);

CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc Same constraint name in 2 constraints => DB_E_DUPLICATECONSTRAINTID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_30()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	consPrimaryKey.SetConstraintName(consUnique.GetConstraintID()->uName.pwszName);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), DB_E_DUPLICATECONSTRAINTID);
	
CLEANUP:
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc NULL pConstraintID in 2 constraints => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Boundary::Variation_31()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);
	ReleaseDBID(((DBCONSTRAINTDESC*)consUnique)->pConstraintID, TRUE);
	((DBCONSTRAINTDESC*)consUnique)->pConstraintID = NULL;

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);
	ReleaseDBID(((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID);
	((DBCONSTRAINTDESC*)consPrimaryKey)->pConstraintID = NULL;

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCCreateTableWithConstraints_Boundary::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END



// {{ TCW_TC_PROTOTYPE(TCDropConstraint)
//*-----------------------------------------------------------------------
//| Test Case:		TCDropConstraint - Testcase for ITableDefinitionWithConstraints::DropConstraint
//| Created:  	8/9/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropConstraint::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc pTableID is NULL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_1()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, NULL, cons), E_INVALIDARG);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc pTableID.uName.pwszName is NULL => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_2()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBID				TableID;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= NULL;
	TESTC_(DropConstraintAndCheck(m_pITDWC, &TableID, pConsDesc->pConstraintID), DB_E_BADTABLEID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Table name is empty => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_3()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	DBID				TableID;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= L"";
	TESTC_(DropConstraintAndCheck(m_pITDWC, &TableID, pConsDesc->pConstraintID), 
		DB_E_BADTABLEID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Table name is not the one on which the column was created
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_4()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;
	HRESULT				hr;

	TESTC_(Table.CreateTable(0, 0), S_OK);
	TESTC_(Table2.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	CHECK(hr = DropConstraintAndCheck(m_pITDWC, &Table2.GetTableID(), pConsDesc->pConstraintID), S_OK);

	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), pConsDesc->pConstraintID), 
		(S_OK == hr)? DB_E_BADCONSTRAINTID: S_OK);

CLEANUP:
	Table.DropTable();
	Table2.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Inexistent table name => DB_E_NOTABLE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_5()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CTable				Table2(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	DBCONSTRAINTDESC	*pConsDesc = (DBCONSTRAINTDESC*)cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// alloc memory for column array
	pConsDesc->cColumns = 1;
	SAFE_ALLOC(pConsDesc->rgColumnList, DBID, pConsDesc->cColumns);
	memset(pConsDesc->rgColumnList, 0, sizeof(pConsDesc->rgColumnList));

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);
	DuplicateDBID(*col.GetColID(), pConsDesc->rgColumnList);

	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(Table2.MakeTableName(NULL), S_OK);
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table2.GetTableID(), pConsDesc->pConstraintID), DB_E_NOTABLE);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Maximum table name => S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_6()
{ 
	TBEGIN
	CTable			Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons		cons;
	CCol			col;
	WCHAR			*pwszTableName = NULL;

	// create the base table with maximum name length
	Table.MakeTableName(NULL);
	pwszTableName = BuildValidName(m_cMaxTableName, Table.GetTableName());
	TESTC_(Table.CreateTable(0, 0, pwszTableName), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), 
		cons.GetConstraintID()), S_OK);

CLEANUP:
	Table.DropTable();
	SAFE_FREE(pwszTableName);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Invalid characters in table's name => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_7()
{ 
	TBEGIN
	CTable			Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons		cons;
	CCol			col;
	DBORDINAL		m, n;
	DBID			TableID;

	// create the base table 
	TESTC_(Table.CreateTable(0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// set an invalid table name
	m = min(m_cMaxColName, n=5+wcslen(Table.GetTableName()));
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= BuildInvalidName(m, Table.GetTableName(), m_pwszInvalidColChars);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &TableID, cons.GetConstraintID()), DB_E_BADTABLEID);

CLEANUP:
	Table.DropTable();
	ReleaseDBID(&TableID, FALSE);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc pConstraintID is NUL => E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_8()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint and recreate it
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), NULL), E_INVALIDARG);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc *pConstraintID is DB_NULLID => DB_E_BADID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_9()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	cons.SetConstraintID((DBID*)&DB_NULLID);
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Invalid DBKIND in the constraint name => DB_E_BADID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_10()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	ReleaseDBID(((DBCONSTRAINTDESC*)cons)->pConstraintID, FALSE);
	((DBCONSTRAINTDESC*)cons)->pConstraintID->eKind = 0xa0;

	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADID);

CLEANUP:
	((DBCONSTRAINTDESC*)cons)->pConstraintID->eKind = DBKIND_NAME;
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc NULL constraint name => DB_E_BADID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_11()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	cons.SetConstraintName(NULL);

	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Constraint name is empty string => DB_E_BADID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_12()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	cons.SetConstraintName(L"");

	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Inexistent constraint name => DB_E_BADCONSTRAINTID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_13()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	cons.MakeConstraintName();
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Drop a constraint twice
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_14()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose a column and create a unique constraint on it
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), DB_E_BADCONSTRAINTID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Drop a unique constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_15()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose a column and create a unique constraint on it
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Drop a primary key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_16()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CPrimaryKeyCons		cons;
	CCol				col;

	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// choose a column and create a primary key constraint on it
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Drop a foreign key constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_17()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		cons(NULL, m_SupportedMatchType);
	CCol				col;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(BaseTable.GetColInfo(1, col), S_OK);
	cons.AddForeignKeyColumn(col.GetColID());

	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	cons.AddColumn(col.GetColID());

	cons.SetReferencedTableID(&ReferencedTable.GetTableID());

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), S_OK);

CLEANUP:
	CHECK(BaseTable.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Drop a check constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_18()
{ 
	TBEGIN
	CTable		Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CCheckCons	cons;
	CCol		col;
	WCHAR		*pwszConstraintText = NULL;
	WCHAR		wszText[] = L" IS NOT NULL";

	TESTC_(CreateTableWithNoNullableColumns(&Table, 0), S_OK);

	// choose a column and create a primary key constraint on it
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	// build constraint text
	TESTC_PROVIDER(DBKIND_NAME == col.GetColID()->eKind);
	SAFE_ALLOC(pwszConstraintText, WCHAR, 1 + wcslen(col.GetColID()->uName.pwszName) + wcslen(wszText));
	wcscpy(pwszConstraintText, col.GetColID()->uName.pwszName);
	wcscat(pwszConstraintText, wszText);
	cons.SetConstraintText(pwszConstraintText);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

CLEANUP:
	Table.DropTable();
	SAFE_FREE(pwszConstraintText);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Create several constraints, drop a single constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_19()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBID			TableID;
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray;
	CUniqueCons		consUnique;
	CPrimaryKeyCons	consPrimaryKey;
	CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);
	DBORDINAL		rgReferencedCol[] = {1};
	CCol			col;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	m_pTable->MakeTableName(NULL);
	TableID.eKind			= DBKIND_NAME;
	TableID.uName.pwszName	= m_pTable->GetTableName();

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, rgReferencedCol[0], (DBDEFERRABILITY)0), S_OK);

	// set a unique constraint on the second column
	consUnique.AddColumn(&rgColumnDesc[1].dbcid);

	// set a primary key on the first column of the table
	consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

	// set a simple foreign key constraint
	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	consForeignKey.AddColumn(col.GetColID());
	consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
	consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

	// add all constraint to the array
	ConsDescArray.AddConsDesc(consUnique);
	ConsDescArray.AddConsDesc(consForeignKey);
	ConsDescArray.AddConsDesc(consPrimaryKey);

	TESTC_(CreateAndCheckTableWithConstraints(m_pITDWC, NULL, &TableID, cColumnDesc, rgColumnDesc, 
		ConsDescArray, ConsDescArray, 
		IID_IRowset, 0, NULL, NULL, NULL), S_OK);

	CHECK(DropConstraintAndCheck(m_pITDWC, &TableID, consForeignKey),	S_OK);
	CHECK(DropConstraintAndCheck(m_pITDWC, &TableID, consUnique),	S_OK);
	CHECK(DropConstraintAndCheck(m_pITDWC, &TableID, consPrimaryKey),	S_OK);
	
CLEANUP:
	CHECK(DropTableAndCheck(&TableID), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Drop a unique constraint in the referenced table, after the foreign key constraint was added
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_20()
{ 
	TBEGIN
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CForeignKeyCons		cons(NULL, m_SupportedMatchType);
	CUniqueCons			consUnique;
	CUniqueCons			consUnique1;
	CCol				col;

	TESTC_(BaseTable.CreateTable(0, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);

	consUnique.SetConstraint(&ReferencedTable, 1);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &ReferencedTable.GetTableID(), consUnique), S_OK);

	consUnique1.SetConstraint(&ReferencedTable, 2);
	TESTC_(AddConstraintAndCheck(m_pITDWC, &ReferencedTable.GetTableID(), consUnique1), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(BaseTable.GetColInfo(1, col), S_OK);
	cons.AddForeignKeyColumn(col.GetColID());

	TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
	cons.AddColumn(col.GetColID());

	cons.SetReferencedTableID(&ReferencedTable.GetTableID());

	TESTC_(AddConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), S_OK);

	// drop the unique constraints on the referenced table
	CHECK(DropConstraintAndCheck(m_pITDWC, &ReferencedTable.GetTableID(), consUnique), 
		DB_E_DROPRESTRICTED);
	CHECK(DropConstraintAndCheck(m_pITDWC, &ReferencedTable.GetTableID(), consUnique1), 
		S_OK);

	// drop the foreign key constraint on the base table
	CHECK(DropConstraintAndCheck(m_pITDWC, &BaseTable.GetTableID(), cons), S_OK);

	// drop the unique constraint on the referenced table
	CHECK(DropConstraintAndCheck(m_pITDWC, &ReferencedTable.GetTableID(), consUnique), 
		S_OK);

CLEANUP:
	CHECK(BaseTable.DropTable(), S_OK);
	CHECK(ReferencedTable.DropTable(), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Try to drop a constraint while a rowset is opened on the table => DB_E_TABLEINUSE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_21()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;
	IRowset				*pIRowset	= NULL;
	IRowsetChange		*pIRowsetChange	= NULL;
	HRESULT				hr;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose a column and create a unique constraint on it
	TESTC_(Table.GetColInfo(3, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// open a rowset on the table 
	CHECK(Table.Insert(7), S_OK);
	CHECK(Table.Insert(7), DB_E_INTEGRITYVIOLATION);
	TESTC_(hr = m_pIOpenRowset->OpenRowset(NULL, &Table.GetTableID(), NULL, 
		IID_IRowsetChange, 0, NULL, (IUnknown**)&pIRowsetChange), S_OK);

	// drop the constraint 
	TEST2C_(hr = DropConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), 
		S_OK, DB_E_TABLEINUSE);

	if (S_OK == hr)
	{
		odtLog << "Constraint was dropped\n";
		CHECK(Table.Insert(7), S_OK);
	}
	else
		CHECK(Table.Insert(7), DB_E_INTEGRITYVIOLATION);


CLEANUP:
	SAFE_RELEASE(pIRowsetChange);
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc *pTableID is DB_NULLID => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_22()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TESTC_(DropConstraintAndCheck(m_pITDWC, (DBID*)&DB_NULLID, cons), DB_E_BADTABLEID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Invalid DBKIND in pTableID => DB_E_BADTABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint::Variation_23()
{ 
	TBEGIN
	CTable				Table(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	CUniqueCons			cons;
	CCol				col;
	DBID				TableID;

	TESTC_(Table.CreateTable(0, 0), S_OK);

	// choose 1 column and create a unique constraint on them
	TESTC_(Table.GetColInfo(1, col), S_OK);

	// Add constraint
	cons.AddColumn(col.GetColID());
	TESTC_(AddConstraintAndCheck(m_pITDWC, &Table.GetTableID(), cons), S_OK);

	// drop the constraint 
	TableID.eKind = 0x44;
	TESTC_(DropConstraintAndCheck(m_pITDWC, &TableID, cons), DB_E_BADTABLEID);

CLEANUP:
	Table.DropTable();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCDropConstraint::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END



// {{ TCW_TC_PROTOTYPE(TCAddConstraint_Multithreading)
//*-----------------------------------------------------------------------
//| Test Case:		TCAddConstraint_Multithreading - Testcase for ITableDefinitionWithConstraints::AddConstraint used in multithreaded calls
//| Created:  	9/14/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCAddConstraint_Multithreading::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




//---------------------------------------------------------------------
//
// @cmember Thread funtion for TCAddConstraint_Multithreading
//---------------------------------------------------------------------
unsigned TCAddConstraint_Multithreading::MyThreadProc(ULONG i)
{
	HRESULT			hr;

	if (i>= nThreads)
		return 0;

	Sleep(0); // switch the context

  	hr = m_pITDWC->AddConstraint(m_prgTableID[i], &m_rgConstraintDesc[i]);
	Sleep(0); // switch the context

	m_rgResult[i] = hr;
	return 1;
} //TCAddConstraint_Multithreading::MyThreadProc




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Each thread creates a different constraint
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Multithreading::Variation_1()
{
	TBEGIN
	DBORDINAL		cThreads = nThreads;
	CConsDesc		*pConsDesc = NULL;
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable			BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;
	
	DBORDINAL		rgCol[] = {1};
	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(m_pITDWC);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1), S_OK);

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(m_rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	memset(m_rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		if (0 == i)
		{
			pConsDesc = new CPrimaryKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CPrimaryKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else if (i%2)
		{
			pConsDesc = new CUniqueCons(m_rgConstraintDesc+i);
			TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else
		{
			pConsDesc = new CForeignKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CForeignKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1, rgCol, &ReferencedTable, 1, rgCol, m_SupportedMatchType), S_OK);
		}

		pConsDesc->MakeConstraintName();
		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (!CHECK(m_rgResult[i], S_OK))
		{
			if (	m_rgConstraintDesc[i].pConstraintID
				&&	DBKIND_NAME == m_rgConstraintDesc[i].pConstraintID->eKind
				&&	m_rgConstraintDesc[i].pConstraintID->uName.pwszName)
				odtLog << "Constraint " << m_rgConstraintDesc[i].pConstraintID->uName.pwszName << " failed to be added\n";
			else
				odtLog << "Constraint no " << i << " failed to be added\n";

			COMPARE(CheckResult(m_rgResult[i], cValidHRes, rgValidHRes), TRUE);
		}
		else
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(1, &m_rgConstraintDesc[i]), S_OK);
			}
		}
	}

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &m_rgConstraintDesc, TRUE), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Some of the threads try to create the same constraint name
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Multithreading::Variation_2()
{ 
	TBEGIN
	DBORDINAL		cThreads = nThreads;
	CConsDesc		*pConsDesc = NULL;
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable			BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;

	const ULONG		cSameName	= 5;
	DBORDINAL		cDuplicates	= 0;

	DBORDINAL		rgCol[] = {1};
	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(m_pITDWC);

	TESTC_PROVIDER(cSameName <= nThreads);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1), S_OK);

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(m_rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	memset(m_rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		if (0 == i)
		{
			pConsDesc = new CPrimaryKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CPrimaryKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else if (i%2)
		{
			pConsDesc = new CUniqueCons(m_rgConstraintDesc+i);
			TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else
		{
			pConsDesc = new CForeignKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CForeignKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1, rgCol, &ReferencedTable, 1, rgCol, m_SupportedMatchType), S_OK);
		}

		if (0 == i || i >= cSameName)
			pConsDesc->MakeConstraintName();
		else
			// several constraints have the same name
			pConsDesc->SetConstraintID(m_rgConstraintDesc[0].pConstraintID);
		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (S_OK != m_rgResult[i])
		{
			COMPARE(CompareDBID(*m_rgConstraintDesc[0].pConstraintID, 
					*m_rgConstraintDesc[i].pConstraintID), TRUE);
			
			if (CHECK(m_rgResult[i], DB_E_DUPLICATECONSTRAINTID))
				cDuplicates++;

			if (	m_rgConstraintDesc[i].pConstraintID
				&&	DBKIND_NAME == m_rgConstraintDesc[i].pConstraintID->eKind
				&&	m_rgConstraintDesc[i].pConstraintID->uName.pwszName)
				odtLog << "Constraint " << m_rgConstraintDesc[i].pConstraintID->uName.pwszName << " failed to be added\n";
			else
				odtLog << "Constraint no " << i << " failed to be added\n";
		}
		else
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(1, &m_rgConstraintDesc[i]), S_OK);
			}
		}
	}

	// exactly cSameName - 1 ops should have failed
	TESTC(cSameName - 1 == cDuplicates);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &m_rgConstraintDesc, TRUE), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Unique constraints with disjoint columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Multithreading::Variation_3()
{ 
	TBEGIN
	DBORDINAL		cThreads = nThreads;
	CConsDesc		*pConsDesc = NULL;
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable			BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;

	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(m_pITDWC);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_PROVIDER(nThreads <= BaseTable.CountColumnsOnTable());

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(m_rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	memset(m_rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		pConsDesc = new CUniqueCons(m_rgConstraintDesc+i);
		TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1+i), S_OK);
		pConsDesc->MakeConstraintName();
		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (CHECK(m_rgResult[i], S_OK))
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(1, &m_rgConstraintDesc[i]), S_OK);
			}
		}
	}


CLEANUP:
	BaseTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &m_rgConstraintDesc, TRUE), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Unique constraints with overlapping columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Multithreading::Variation_4()
{ 
	TBEGIN
	DBORDINAL		cThreads = nThreads;
	CConsDesc		*pConsDesc = NULL;
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable			BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;

	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(m_pITDWC);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_PROVIDER(nThreads <= BaseTable.CountColumnsOnTable());

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(m_rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	memset(m_rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		pConsDesc = new CUniqueCons(m_rgConstraintDesc+i);
		TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		pConsDesc->MakeConstraintName();
		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (CHECK(m_rgResult[i], S_OK))
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(1, &m_rgConstraintDesc[i]), S_OK);
			}
		}
	}


CLEANUP:
	BaseTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &m_rgConstraintDesc, TRUE), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc All constraint IDs are generated by the provider
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCAddConstraint_Multithreading::Variation_5()
{ 
	TBEGIN
	DBORDINAL		cThreads = nThreads;
	CConsDesc		*pConsDesc = NULL;
	CTable			ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable			BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	ULONG			i;
	
	DBORDINAL		rgCol[] = {1};
	HRESULT			rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADTABLEID,
										DB_E_DUPLICATECONSTRAINTID,
										DB_E_BADID,
										DB_E_BADCONSTRAINTTYPE,
										DB_E_BADUPDATEDELETERULE,
										DB_E_BADMATCHTYPE,
										DB_E_BADDEFERRABILITY,
										DB_E_BADCONSTRAINTFORM,
										DB_E_BADCOLUMNID,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG			cValidHRes = NUMELEM(rgValidHRes);
	CConstraints	Constraints(m_pITDWC);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1), S_OK);

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(m_rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	memset(m_rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		if (0 == i)
		{
			pConsDesc = new CPrimaryKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CPrimaryKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else if (i%2)
		{
			pConsDesc = new CUniqueCons(m_rgConstraintDesc+i);
			TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else
		{
			pConsDesc = new CForeignKeyCons(m_rgConstraintDesc+i);
			TESTC_(((CForeignKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1, rgCol, &ReferencedTable, 1, rgCol, m_SupportedMatchType), S_OK);
		}

		pConsDesc->SetConstraintID(NULL);
		
		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (CHECK(m_rgResult[i], S_OK))
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK)
				&&	m_rgConstraintDesc[i].pConstraintID
				)
			{
				CHECK(Constraints.CheckIncludedConstraints(1, &m_rgConstraintDesc[i]), S_OK);
			}
		}
	}

	COMPARE(Constraints.GetConstrNo(), nThreads);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &m_rgConstraintDesc, TRUE), S_OK);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCAddConstraint_Multithreading::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCDropConstraint_Multithreading)
//*-----------------------------------------------------------------------
//| Test Case:		TCDropConstraint_Multithreading - Multithreading test case for ITableDefinitionWithConstraints::DropConstraints
//| Created:  	9/16/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCDropConstraint_Multithreading::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




//---------------------------------------------------------------------
//
// @cmember Thread funtion for TCDropConstraint_Multithreading
//---------------------------------------------------------------------
unsigned TCDropConstraint_Multithreading::MyThreadProc(ULONG i)
{
	HRESULT			hr;

	if (i>= nThreads)
		return 0;

	Sleep(0); // switch the context

  	hr = m_pITDWC->DropConstraint(m_prgTableID[i], m_prgConstraintID[i]);
	Sleep(0); // switch the context

	m_rgResult[i] = hr;
	return 1;
} //TCDropConstraint_Multithreading::MyThreadProc




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Each thread drops a different constraint => all S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint_Multithreading::Variation_1()
{ 
	TBEGIN
	DBORDINAL			cThreads = nThreads;
	CConsDesc			*pConsDesc = NULL;
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned			rgIDThread[nThreads];
	HANDLE				hrgThread[nThreads];
	CInParam			rgThreadParam[nThreads];
	ULONG				i;
	DBCONSTRAINTDESC	*rgConstraintDesc = NULL;
	DBORDINAL			rgCol[] = {1};
	HRESULT				rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADID,
										DB_E_BADCONSTRAINTID,
										DB_E_BADTABLEID,
										DB_E_DROPRESTRICTED,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG				cValidHRes = NUMELEM(rgValidHRes);
	CConstraints		Constraints(m_pITDWC);

	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, 1), S_OK);

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	SAFE_ALLOC(m_prgConstraintID, DBID*, nThreads);
	memset(rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		// create a constraint
		if (0 == i)
		{
			pConsDesc = new CPrimaryKeyCons(rgConstraintDesc+i);
			TESTC_(((CPrimaryKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else if (i%2)
		{
			pConsDesc = new CUniqueCons(rgConstraintDesc+i);
			TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else
		{
			pConsDesc = new CForeignKeyCons(rgConstraintDesc+i);
			TESTC_(((CForeignKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1, rgCol, &ReferencedTable, 1, rgCol, m_SupportedMatchType), S_OK);
		}

		pConsDesc->MakeConstraintName();
		TESTC_(m_pITDWC->AddConstraint(m_prgTableID[i], &rgConstraintDesc[i]), S_OK);

		m_prgConstraintID[i] = rgConstraintDesc[i].pConstraintID;

		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (!CHECK(m_rgResult[i], S_OK))
		{
			if (	rgConstraintDesc[i].pConstraintID
				&&	DBKIND_NAME == rgConstraintDesc[i].pConstraintID->eKind
				&&	rgConstraintDesc[i].pConstraintID->uName.pwszName)
				odtLog << "Constraint " << rgConstraintDesc[i].pConstraintID->uName.pwszName << " failed to be added\n";
			else
				odtLog << "Constraint no " << i << " failed to be dropped\n";

			COMPARE(CheckResult(m_rgResult[i], cValidHRes, rgValidHRes), TRUE);
		}
		else
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				COMPARE(NULL == Constraints[m_prgConstraintID[i]], TRUE);
			}
		}
	}

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &rgConstraintDesc, TRUE), S_OK);
	SAFE_FREE(m_prgConstraintID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Some threads try to drop the same constraint => S_OK and DB_E_BADCONSTRAINTID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCDropConstraint_Multithreading::Variation_2()
{ 
	TBEGIN
	DBORDINAL			cThreads = nThreads;
	CConsDesc			*pConsDesc = NULL;
	CTable				ReferencedTable(m_pITDWC, (LPWSTR)gwszModuleName);
	CTable				BaseTable(m_pITDWC, (LPWSTR)gwszModuleName);

	unsigned			rgIDThread[nThreads];
	HANDLE				hrgThread[nThreads];
	CInParam			rgThreadParam[nThreads];
	ULONG				i;
	DBCONSTRAINTDESC	*rgConstraintDesc = NULL;
	DBORDINAL			rgCol[] = {1};
	HRESULT				rgValidHRes[]	= {	S_OK,
										E_FAIL,
										E_INVALIDARG,
										DB_E_BADID,
										DB_E_BADCONSTRAINTID,
										DB_E_BADTABLEID,
										DB_E_DROPRESTRICTED,
										DB_E_NOTABLE,
										DB_E_TABLEINUSE,
										DB_SEC_E_PERMISSIONDENIED,
										XACT_E_XTIONEXISTS,
									}; 
	ULONG				cValidHRes = NUMELEM(rgValidHRes);
	CConstraints		Constraints(m_pITDWC);

	DBORDINAL			ulFailed	= 0;
	const DBORDINAL		cSameName	= 5;

	TESTC_PROVIDER(cSameName <= nThreads);
	TESTC_(CreateTableWithNoNullableColumns(&BaseTable, 0), S_OK);
	TESTC_(ReferencedTable.CreateTable(0), S_OK);
	TESTC_(AddUniqueConstraint(&BaseTable, 1), S_OK);

	SAFE_ALLOC(m_prgTableID, DBID*, nThreads);
	SAFE_ALLOC(rgConstraintDesc, DBCONSTRAINTDESC, nThreads);
	SAFE_ALLOC(m_prgConstraintID, DBID*, nThreads);
	memset(rgConstraintDesc, 0, nThreads * sizeof(DBCONSTRAINTDESC));
	for (i=0; i< nThreads; i++)
	{
		m_rgResult[i] = -1;
		m_prgTableID[i]	= &BaseTable.GetTableID();

		// create a constraint
		if (0 == i)
		{
			pConsDesc = new CPrimaryKeyCons(rgConstraintDesc+i);
			TESTC_(((CPrimaryKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else if (i%2)
		{
			pConsDesc = new CUniqueCons(rgConstraintDesc+i);
			TESTC_(((CUniqueCons*)pConsDesc)->SetConstraint(&BaseTable, 1), S_OK);
		}
		else
		{
			pConsDesc = new CForeignKeyCons(rgConstraintDesc+i);
			TESTC_(((CForeignKeyCons*)pConsDesc)->SetConstraint(&BaseTable, 1, rgCol, &ReferencedTable, 1, rgCol, m_SupportedMatchType), S_OK);
		}

		pConsDesc->MakeConstraintName();
		TESTC_(m_pITDWC->AddConstraint(m_prgTableID[i], &rgConstraintDesc[i]), S_OK);

		if (i < cSameName)
			m_prgConstraintID[i] = rgConstraintDesc[0].pConstraintID;
		else
			m_prgConstraintID[i] = rgConstraintDesc[i].pConstraintID;

		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		if (S_OK != m_rgResult[i])
		{
			CHECK(m_rgResult[i], DB_E_BADCONSTRAINTID);
			ulFailed++;

			if (	rgConstraintDesc[i].pConstraintID
				&&	DBKIND_NAME == rgConstraintDesc[i].pConstraintID->eKind
				&&	rgConstraintDesc[i].pConstraintID->uName.pwszName)
				odtLog << "Constraint " << rgConstraintDesc[i].pConstraintID->uName.pwszName << " failed to be added\n";
			else
				odtLog << "Constraint no " << i << " failed to be dropped\n";

			COMPARE(CheckResult(m_rgResult[i], cValidHRes, rgValidHRes), TRUE);
		}
		else
		{
			// get all the constraints associated with the table
			if (	m_prgTableID[i] 
				&&	DBKIND_NAME == m_prgTableID[i]->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, m_prgTableID[i]->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				COMPARE(NULL == Constraints[m_prgConstraintID[i]], TRUE);
			}
		}
	}

	TESTC(cSameName - 1 == ulFailed);

CLEANUP:
	BaseTable.DropTable();
	ReferencedTable.DropTable();
	SAFE_FREE(m_prgTableID);
	CHECK(FreeConstraintDesc(&cThreads, &rgConstraintDesc, TRUE), S_OK);
	SAFE_FREE(m_prgConstraintID);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCDropConstraint_Multithreading::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCCreateTableWithConstraints_Multithreading)
//*-----------------------------------------------------------------------
//| Test Case:		TCCreateTableWithConstraints_Multithreading - Multithreading test case for ITableDefinitionWithConstraints::CreateTableWithConstraints
//| Created:  	9/17/99
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCCreateTableWithConstraints_Multithreading::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCITableDefinition::Init())
	// }}
	{ 
		// TO DO:  Add your own code here 
		return TRUE;
	} 
	return FALSE;
} 




//---------------------------------------------------------------------
//
// @cmember Thread funtion for TCCreateTableWithConstraints_Multithreading
//---------------------------------------------------------------------
unsigned TCCreateTableWithConstraints_Multithreading::MyThreadProc(ULONG i)
{
	HRESULT			hr;

	if (i>= nThreads)
		return 0;

	Sleep(0); // switch the context

  	hr = m_pITDWC->CreateTableWithConstraints(
		m_rgThreadParams[i].m_pUnkOuter,
		m_rgThreadParams[i].m_pTableID,
		m_rgThreadParams[i].m_cColumnDescs,
		m_rgThreadParams[i].m_rgColumnDescs,
		m_rgThreadParams[i].m_cConstraintDescs,
		m_rgThreadParams[i].m_rgConstraintDescs,
		*m_rgThreadParams[i].m_piid,
		m_rgThreadParams[i].m_cPropertySets,
		m_rgThreadParams[i].m_rgPropertySets,
		m_rgThreadParams[i].m_ppTableID,
		m_rgThreadParams[i].m_ppRowset
		);
	Sleep(0); // switch the context

	m_rgThreadParams[i].m_hr = hr;
	return 1;
} //TCCreateTableWithConstraints_Multithreading::MyThreadProc




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc Each thread builds different table => all S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Multithreading::Variation_1()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray[nThreads];
	DBORDINAL		i;
	CCol			col;

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	CConstraints	Constraints(m_pITDWC);

	m_rgThreadParams = NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1, (DBDEFERRABILITY)0), S_OK);

	m_rgThreadParams = new CCTWCThreadParam[nThreads];
	for (i=0; i< nThreads; i++)
	{
		CUniqueCons		consUnique;
		CPrimaryKeyCons	consPrimaryKey;
		CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);

		m_rgThreadParams[i].m_hr			= E_FAIL;
		m_rgThreadParams[i].m_op			= CREATETABLE_CONSTRAINTS;
		
		m_rgThreadParams[i].m_pUnkOuter		= NULL;
		m_rgThreadParams[i].m_pTableID		= NULL;
		
		m_rgThreadParams[i].m_cColumnDescs	= cColumnDesc;
		m_rgThreadParams[i].m_rgColumnDescs	= rgColumnDesc;
		
		// create the array of constraints
			// set a unique constraint on the second column
			consUnique.AddColumn(&rgColumnDesc[1].dbcid);

			// set a primary key on the first column of the table
			consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

			// set a simple foreign key constraint
			TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
			consForeignKey.AddColumn(col.GetColID());
			consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
			consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

			// add all constraint to the array
			ConsDescArray[i].AddConsDesc(consUnique);
			ConsDescArray[i].AddConsDesc(consForeignKey);
			ConsDescArray[i].AddConsDesc(consPrimaryKey);

		m_rgThreadParams[i].m_cConstraintDescs = ConsDescArray[i];
		m_rgThreadParams[i].m_rgConstraintDescs = ConsDescArray[i];
		
		m_rgThreadParams[i].m_piid			= NULL;
		m_rgThreadParams[i].m_cPropertySets	= 0;
		m_rgThreadParams[i].m_rgPropertySets= NULL;
	
		SAFE_ALLOC(m_rgThreadParams[i].m_ppTableID, DBID*, 1);
		//*m_rgThreadParams[i].m_ppTableID	= NULL;
		m_rgThreadParams[i].m_ppRowset		= NULL;

		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		DBID	*pTableID = NULL;

		if (m_rgThreadParams[i].m_ppTableID)
			pTableID = (*m_rgThreadParams[i].m_ppTableID);
		else if (m_rgThreadParams[i].m_pTableID)
			pTableID = m_rgThreadParams[i].m_pTableID;

		if (CHECK(m_rgThreadParams[i].m_hr, S_OK))
		{
			COMPARE(NULL != pTableID, TRUE);

			// get all the constraints associated with the table
			if (	pTableID
				&&	DBKIND_NAME == pTableID->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(m_rgThreadParams[i].m_cConstraintDescs, m_rgThreadParams[i].m_rgConstraintDescs), S_OK);
			}

			// drop the newly created table
			CHECK(m_pITableDefinition->DropTable(pTableID), S_OK);
		}
	}

CLEANUP:
	delete [] m_rgThreadParams;
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Some threads build the same table => S_OK and DB_E_DUPLICATETABLEID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCCreateTableWithConstraints_Multithreading::Variation_2()
{ 
	TBEGIN
	CTable			ReferencedTable(m_pThisTestModule->m_pIUnknown2, (LPWSTR)gwszModuleName);
	DBCOLUMNDESC	*rgColumnDesc	= NULL;
	ULONG			cColumnDesc		= 0;
	CConsDescArray	ConsDescArray[nThreads];
	DBORDINAL		i;
	CCol			col;

	unsigned		rgIDThread[nThreads];
	HANDLE			hrgThread[nThreads];
	CInParam		rgThreadParam[nThreads];
	CConstraints	Constraints(m_pITDWC);
	WCHAR			*pwszBaseName = NULL;

	WCHAR			number[3];
	DBORDINAL		cSameName	= 5;
	DBORDINAL		cDuplicates	= 0;

	TESTC_PROVIDER(cSameName <= nThreads);
	m_rgThreadParams = NULL;

	DuplicateColumnDesc(m_rgColumnDesc, m_cColumnDesc, &rgColumnDesc);
	cColumnDesc = m_cColumnDesc;
	// release the properties on all column and make the first column not nullable
	ReleaseAllColumnPropSets(rgColumnDesc, cColumnDesc);
	SetProperty(&rgColumnDesc[0].rgPropertySets, &rgColumnDesc[0].cPropertySets,
			DBPROP_COL_NULLABLE, 
			VT_BOOL, (LPVOID)VARIANT_FALSE, 
			DBPROPOPTIONS_REQUIRED);

	// create support for foreign key constraint
	TESTC_(ReferencedTable.CreateTable(0, 0), S_OK);
	// make the referenced columns unique
	TESTC_(AddUniqueConstraint(&ReferencedTable, 1, (DBDEFERRABILITY)0), S_OK);

	m_pTable->MakeTableName(NULL);
	pwszBaseName = m_pTable->GetTableName();

	m_rgThreadParams = new CCTWCThreadParam[nThreads];
	for (i=0; i< nThreads; i++)
	{
		CUniqueCons		consUnique;
		CPrimaryKeyCons	consPrimaryKey;
		CForeignKeyCons	consForeignKey(NULL, m_SupportedMatchType);

		m_rgThreadParams[i].m_hr			= E_FAIL;
		m_rgThreadParams[i].m_op			= CREATETABLE_CONSTRAINTS;
		
		m_rgThreadParams[i].m_pUnkOuter		= NULL;
		
		SAFE_ALLOC(m_rgThreadParams[i].m_pTableID, DBID, 1);
		m_rgThreadParams[i].m_pTableID->eKind = DBKIND_NAME;

		SAFE_ALLOC(m_rgThreadParams[i].m_pTableID->uName.pwszName, WCHAR,
			(3+wcslen(pwszBaseName)));		
		wcscpy(m_rgThreadParams[i].m_pTableID->uName.pwszName, pwszBaseName);
		swprintf(number, L"%02d", (i < cSameName)? 0: i);
		wcscat(m_rgThreadParams[i].m_pTableID->uName.pwszName, number);
		
		m_rgThreadParams[i].m_cColumnDescs	= cColumnDesc;
		m_rgThreadParams[i].m_rgColumnDescs	= rgColumnDesc;
		
		// create the array of constraints
			// set a unique constraint on the second column
			consUnique.AddColumn(&rgColumnDesc[1].dbcid);

			// set a primary key on the first column of the table
			consPrimaryKey.AddColumn(&rgColumnDesc[0].dbcid);

			// set a simple foreign key constraint
			TESTC_(ReferencedTable.GetColInfo(1, col), S_OK);
			consForeignKey.AddColumn(col.GetColID());
			consForeignKey.AddForeignKeyColumn(&rgColumnDesc[0].dbcid);
			consForeignKey.SetReferencedTableID(&ReferencedTable.GetTableID());

			// add all constraint to the array
			ConsDescArray[i].AddConsDesc(consUnique);
			ConsDescArray[i].AddConsDesc(consForeignKey);
			ConsDescArray[i].AddConsDesc(consPrimaryKey);

		m_rgThreadParams[i].m_cConstraintDescs = ConsDescArray[i];
		m_rgThreadParams[i].m_rgConstraintDescs = ConsDescArray[i];
		
		m_rgThreadParams[i].m_piid			= NULL;
		m_rgThreadParams[i].m_cPropertySets	= 0;
		m_rgThreadParams[i].m_rgPropertySets= NULL;
	
		m_rgThreadParams[i].m_ppTableID		= NULL;
		m_rgThreadParams[i].m_ppRowset		= NULL;

		rgThreadParam[i].pObject	= this;
		rgThreadParam[i].i			= i;
	}
	
	// start the threads
	for (i=0; i<nThreads; i++)
	{
		hrgThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc,
							(void*)&rgThreadParam[i],
							0, 
							&rgIDThread[i]);		// input param
		TESTC(0 != hrgThread[i]);
	}

	WaitForMultipleObjects(nThreads, (CONST HANDLE*)hrgThread, TRUE, 1000);
	Sleep(1000);
	for (i=0; i< nThreads; i++)
		CloseHandle(hrgThread[i]);

	for (i=0; i<nThreads; i++)
	{	
		DBID	*pTableID = NULL;

		if (m_rgThreadParams[i].m_ppTableID)
			pTableID = (*m_rgThreadParams[i].m_ppTableID);
		else if (m_rgThreadParams[i].m_pTableID)
			pTableID = m_rgThreadParams[i].m_pTableID;

		if (S_OK == m_rgThreadParams[i].m_hr)
		{
			COMPARE(NULL != pTableID, TRUE);

			// get all the constraints associated with the table
			if (	pTableID
				&&	DBKIND_NAME == pTableID->eKind 
				&&	CHECK(Constraints.GetConstraints(NULL, NULL, pTableID->uName.pwszName, NULL, NULL, NULL), S_OK))
			{
				CHECK(Constraints.CheckIncludedConstraints(m_rgThreadParams[i].m_cConstraintDescs, m_rgThreadParams[i].m_rgConstraintDescs), S_OK);
			}

			// drop the newly created table
			CHECK(m_pITableDefinition->DropTable(pTableID), S_OK);
		}
		else
		{
			if (CHECK(m_rgThreadParams[i].m_hr, DB_E_DUPLICATETABLEID))
				cDuplicates++;
		}
	}

	TESTC(cDuplicates + 1 == cSameName);

CLEANUP:
	delete [] m_rgThreadParams;
	CHECK(ReferencedTable.DropTable(), S_OK);
	ReleaseColumnDesc(rgColumnDesc, cColumnDesc);
	SAFE_FREE(pwszBaseName);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCCreateTableWithConstraints_Multithreading::Terminate()
{ 
	// TO DO:  Add your own code here 

// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCITableDefinition::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END

