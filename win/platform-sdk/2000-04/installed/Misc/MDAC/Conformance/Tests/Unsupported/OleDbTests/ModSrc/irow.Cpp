//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright (C) 1995-1999 Microsoft Corporation
//
// @doc  
//
// @module IRow.cpp | This module tests the OLEDB IRow interface 
//

#include "MODStandard.hpp"		// Standard headers			
#include "IRow.h"				// IRow header
#include "ExtraLib.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Module Values
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// {{ TCW_MODULE_GLOBALS
DECLARE_MODULE_CLSID = { 0xf61ea340, 0x2b1e, 0x11d2, { 0xa9, 0x8d, 0x00, 0xc0, 0x4f, 0x94, 0xa7, 0x17} };
DECLARE_MODULE_NAME("IRow");
DECLARE_MODULE_OWNER("Microsoft");
DECLARE_MODULE_DESCRIP("IRow interface test");
DECLARE_MODULE_VERSION(838086926);
// TCW_WizardVersion(2)
// TCW_Automation(False)
// }} TCW_MODULE_GLOBALS_END


//////////////////////////////////////////////////////////////////////////
// Globals
//
//////////////////////////////////////////////////////////////////////////
ULONG cInterfaceIIDs = 0;
INTERFACEMAP* rgInterfaceIIDs = NULL;



//--------------------------------------------------------------------
// @func Module level initialization routine
//
// @rdesc Success or Failure
// 		@flag  TRUE  | Successful initialization
//		@flag  FALSE | Initialization problems
//
BOOL ModuleInit(CThisTestModule * pThisTestModule)
{	
	//Obtain the Interface IIDs for the Row object
	if(GetInterfaceArray(ROW_INTERFACE, &cInterfaceIIDs, &rgInterfaceIIDs))
		return CommonModuleInit(pThisTestModule, IID_IRow, SIZEOF_TABLE, ROW_INTERFACE);

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
    return CommonModuleTerminate(pThisTestModule);
}	




////////////////////////////////////////////////////////////////////////////
//  TCIRow
//
////////////////////////////////////////////////////////////////////////////
class TCIRow : public CRowset
{
public:
	//constructors
	TCIRow(WCHAR* pwszTestCaseName = INVALID(WCHAR*));
	virtual ~TCIRow();

	//methods
	virtual BOOL		Init();
	virtual BOOL		Terminate();

	//IRow
	virtual BOOL		VerifyGetColumns
						(
							CRowObject*			pCRowObject,
							ULONG				iRow,
							ECOLS_BOUND			eColsToBind		= ALL_COLS_BOUND,			
							BLOBTYPE			dwBlobType		= NO_BLOB_COLS,
							ECOLUMNORDER		eBindingOrder	= FORWARD,		
							ECOLS_BY_REF		eColsByRef		= NO_COLS_BY_REF,				
							DBTYPE				dwModifier		= DBTYPE_EMPTY,
							ULONG				cColsToBind		= 0,
							ULONG*				rgColsToBind    = NULL,
							DBPART				dwPart			= DBPART_VALUE|DBPART_LENGTH|DBPART_STATUS
						);

	virtual BOOL		VerifyGetColumnsAllRows
						(
							CRowset*			pCRowset,
							ECOLS_BOUND			eColsToBind		= ALL_COLS_BOUND,			
							BLOBTYPE			dwBlobType		= NO_BLOB_COLS,
							ECOLUMNORDER		eBindingOrder	= FORWARD,		
							ECOLS_BY_REF		eColsByRef		= NO_COLS_BY_REF,				
							DBTYPE				dwModifier		= DBTYPE_EMPTY,
							ULONG				cColsToBind		= 0,
							ULONG*				rgColsToBind    = NULL,
							DBPART				dwPart			= DBPART_VALUE|DBPART_LENGTH|DBPART_STATUS
						);


	virtual BOOL		VerifyGetSourceRowset
						(
							REFIID riid, 
							IUnknown** ppIRowset = NULL, 
							HROW* phRow = NULL
						);

	virtual BOOL		VerifyOpen
						(
							IUnknown*			pIUnkOuter,
							CRowObject*			pCRowObject,
							REFGUID				rGuidType,
							REFIID				riid			= IID_IUnknown,
							ECOLS_BOUND			eColsToBind		= ALL_COLS_BOUND,			
							BLOBTYPE			dwBlobType		= NO_BLOB_COLS,
							ECOLUMNORDER		eBindingOrder	= FORWARD,		
							ECOLS_BY_REF		eColsByRef		= NO_COLS_BY_REF,				
							DBTYPE				dwModifier		= DBTYPE_EMPTY,
							ULONG				cColsToBind		= 0,
							ULONG*				rgColsToBind    = NULL
						);

	virtual BOOL		VerifyOpenAllRows
						(
							CRowset*			pCRowset,
							REFGUID				rGuidType,
							REFIID				riid			= IID_IUnknown,
							ECOLS_BOUND			eColsToBind		= ALL_COLS_BOUND,			
							BLOBTYPE			dwBlobType		= NO_BLOB_COLS,
							ECOLUMNORDER		eBindingOrder	= FORWARD,		
							ECOLS_BY_REF		eColsByRef		= NO_COLS_BY_REF,				
							DBTYPE				dwModifier		= DBTYPE_EMPTY,
							ULONG				cColsToBind		= 0,
							ULONG*				rgColsToBind    = NULL
						);


	virtual BOOL		VerifyOpenAllInterfaces
						(
							EINTERFACE eInterface, 
							REFGUID rGuidType
						);

	virtual BOOL		VerifyOpenWithOpenObjects
						(
							REFGUID rGuidtype, 
							REFIID riid
						);

	//Thread Methods
	static ULONG WINAPI Thread_VerifyGetColumns(LPVOID pv);
	static ULONG WINAPI Thread_VerifyGetSourceRowset(LPVOID pv);
	static ULONG WINAPI Thread_VerifyOpenWithOpenObjects(LPVOID pv);

	//Interface
	virtual IRow*		const pIRow();

	//Data
	CRowObject*			m_pCRowObject;
};




////////////////////////////////////////////////////////////////////////////
//  TCIRow::TCIRow
//
////////////////////////////////////////////////////////////////////////////
TCIRow::TCIRow(WCHAR * wstrTestCaseName)	: CRowset(wstrTestCaseName) 
{
	m_pCRowObject	= NULL;
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::~TCIRow
//
////////////////////////////////////////////////////////////////////////////
TCIRow::~TCIRow()
{
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::Init
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::Init()
{
	TBEGIN
	HROW hRow = NULL;
	
	//Create the new row object
	m_pCRowObject = new CRowObject;
	TESTC(m_pCRowObject != NULL);

	TESTC(CRowset::Init());

	//May require IRowsetLocate to position on Blobs
	SetSettableProperty(DBPROP_IRowsetLocate);

	//Create the Rowset object
	TESTC_(CreateRowset(), S_OK);
	
	//Obtain the First row...
	TESTC_(GetNextRows(&hRow),S_OK);

	//Now create the row object.
	TEST2C_(m_pCRowObject->CreateRowObject(pIRowset(), hRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

CLEANUP:
	ReleaseRows(hRow);
	TRETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::Terminate
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::Terminate()
{
	SAFE_DELETE(m_pCRowObject);
	return CRowset::Terminate();
}



////////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyGetColumns
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyGetColumns
(
	CRowObject*			pCRowObject,
	ULONG				iRow,
	ECOLS_BOUND			eColsToBind,			
	BLOBTYPE			dwBlobType,
	ECOLUMNORDER		eBindingOrder,		
	ECOLS_BY_REF		eColsByRef,				
	DBTYPE				dwModifier,
	ULONG				cColsToBind,
	ULONG*				rgColsToBind,
	DBPART				dwPart
)
{
	//Delegate
	return pCRowObject->VerifyGetColumns(iRow, pTable(), eColsToBind, dwBlobType, eBindingOrder, eColsByRef, dwModifier, cColsToBind, rgColsToBind, dwPart);
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyGetColumnsAllRows
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyGetColumnsAllRows
(
	CRowset*			pCRowset,
	ECOLS_BOUND			eColsToBind,			
	BLOBTYPE			dwBlobType,
	ECOLUMNORDER		eBindingOrder,		
	ECOLS_BY_REF		eColsByRef,				
	DBTYPE				dwModifier,
	ULONG				cColsToBind,
	ULONG*				rgColsToBind,
	DBPART				dwPart
)
{
	TBEGIN
	HRESULT hr = S_OK;

	ULONG iRow,cRowsObtained = 0;
	HROW* rghRows = NULL;
	CRowset RowsetA;

	//Default rowset
	if(pCRowset == NULL)
	{
		pCRowset = &RowsetA;

		//May require IRowsetLocate to position on Blobs
		if(dwBlobType != NO_BLOB_COLS)
			pCRowset->SetSettableProperty(DBPROP_IRowsetLocate);
		TESTC_(pCRowset->CreateRowset(),S_OK);
	}

	//Restart the position.
	TESTC_(pCRowset->RestartPosition(),S_OK);

	//loop through the rowset, retrieve one row at a time
	for(iRow=1; iRow<=pCRowset->m_ulTableRows; iRow++)	
	{
		//GetNextRow 
		CRowObject RowObjectA;
		TESTC_(pCRowset->GetNextRows(0, 1, &cRowsObtained, &rghRows),S_OK);
		
		//Create the row object from this row
		TEST2C_(RowObjectA.CreateRowObject(pCRowset->pIRowset(), rghRows[0]), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

		//Verify Row Object
		if(!VerifyGetColumns(&RowObjectA, iRow, eColsToBind, dwBlobType, eBindingOrder, eColsByRef, dwModifier, cColsToBind, rgColsToBind, dwPart))
		{
			//Data incorrect for this row!
			TERROR("Data was incorrect for row " << iRow);
			QTESTC(FALSE);
		}

		//release the row handle
		TESTC_(pCRowset->ReleaseRows(cRowsObtained, rghRows),S_OK);
		PROVIDER_FREE(rghRows);
	}

CLEANUP:
	RowsetA.ReleaseRows(cRowsObtained, rghRows);
	PROVIDER_FREE(rghRows);
	TRETURN
}



///////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyGetSourceRowset
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyGetSourceRowset
(
	REFIID riid, 
	IUnknown** ppIRowset, 
	HROW* phRow
)
{
	TBEGIN
	HRESULT hr = S_OK;
	IUnknown* pIUnknown = NULL;
	HROW hRow = NULL;

	//IRow::GetSourceRowset...
	TESTC_(hr = m_pCRowObject->GetSourceRowset(riid, &pIUnknown, &hRow),S_OK);

	//Verify the rowset returned
	TESTC(DefaultObjectTesting(pIUnknown, ROWSET_INTERFACE));

CLEANUP:
	if(ppIRowset)
		*ppIRowset = pIUnknown;
	else
		SAFE_RELEASE(pIUnknown);
	if(phRow)
		*phRow = hRow;
	else
		ReleaseRows(hRow);
	TRETURN
}


	
///////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyOpen
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyOpen
(
	IUnknown*			pIUnkOuter,
	CRowObject*			pCRowObject,
	REFGUID				rGuidType,
	REFIID				riid,
	ECOLS_BOUND			eColsToBind,			
	BLOBTYPE			dwBlobType,
	ECOLUMNORDER		eBindingOrder,		
	ECOLS_BY_REF		eColsByRef,				
	DBTYPE				dwModifier,
	ULONG				cColsToBind,
	ULONG*				rgColsToBind
)
{
	TBEGIN
	HRESULT hr = S_OK;
	ULONG i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures...
	TESTC_(hr = m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, eColsToBind, dwBlobType, eBindingOrder, eColsByRef, dwModifier, cColsToBind, rgColsToBind),S_OK);
	
	//IRow::Open (for all bound columns)
	for(i=0; i<cColAccess; i++)
	{
		//Open wil mainly only be able to be called for columns containing objects.
		//But some providers might be able to open streams, or other types of objects ontop
		//of non-object valued columns.
		hr = m_pCRowObject->Open(pIUnkOuter, &rgColAccess[i].columnid, rGuidType, riid);
		TEST4C_(hr, S_OK, DB_E_NOTFOUND, DB_E_OBJECTMISMATCH, DB_E_NOAGGREGATION);

		if(SUCCEEDED(hr))
		{
			//Default object testing is done by privlib in CRowObject::Open helper.
			
		}
		else
		{
			//Only Allow no aggregation if using a controlling unknown
			if(hr == DB_E_NOAGGREGATION)
				TESTC(pIUnkOuter != NULL);
	
			//Produce a error if this was an IUnknown column and it could not be opened...
			//At the very least a consumer should be able to open the column as the default
			//native object type (GUID_NULL, and IID_IUnknown)
			if(rgColAccess[i].wType==DBTYPE_IUNKNOWN && rGuidType == GUID_NULL && riid == IID_IUnknown)
				TOUTPUT("WARNING:  IUnknown column unable to be opened with IRow::Open(GUID_NULL, IID_IUnknown...)?");
		}
	}
	
	//DBROWCOL_DEFAULTSTREAM
	//Now that we have done the returned columns, lets also see if there is a default stream object.
	TEST4C_(hr = m_pCRowObject->Open(pIUnkOuter, &DBROWCOL_DEFAULTSTREAM, DBGUID_STREAM, IID_IStream), S_OK, DB_E_NOTFOUND, DB_E_BADCOLUMNID, DB_E_NOAGGREGATION);

	//Only Allow no aggregation if using a controlling unknown
	if(hr == DB_E_NOAGGREGATION)
		TESTC(pIUnkOuter != NULL);
	
CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	TRETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyOpenAllRows
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyOpenAllRows
(
	CRowset*			pCRowset,
	REFGUID				rGuidType,
	REFIID				riid,
	ECOLS_BOUND			eColsToBind,			
	BLOBTYPE			dwBlobType,
	ECOLUMNORDER		eBindingOrder,
	ECOLS_BY_REF		eColsByRef,				
	DBTYPE				dwModifier,
	ULONG				cColsToBind,
	ULONG*				rgColsToBind
)
{
	TBEGIN
	HRESULT hr = S_OK;

	ULONG iRow,cRowsObtained = 0;
	HROW* rghRows = NULL;
	CRowset RowsetA;

	//Default rowset
	if(pCRowset == NULL)
	{
		pCRowset = &RowsetA;
		TESTC_(pCRowset->CreateRowset(),S_OK);
	}

	//loop through the rowset, retrieve one row at a time
	for(iRow=1; iRow<=pCRowset->m_ulTableRows; iRow++)
	{
		//GetNextRow 
		CRowObject RowObjectA;
		TESTC_(pCRowset->GetNextRows(0, 1, &cRowsObtained, &rghRows),S_OK);
		
		//Create the row object from this row
		TEST2C_(RowObjectA.CreateRowObject(pCRowset->pIRowset(), rghRows[0]), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

		//Verify Row Object
		TESTC(VerifyOpen(NULL, &RowObjectA, rGuidType, riid, eColsToBind, dwBlobType, eBindingOrder, eColsByRef, dwModifier, cColsToBind, rgColsToBind));

		//release the row handle
		TESTC_(pCRowset->ReleaseRows(cRowsObtained, rghRows),S_OK);
		PROVIDER_FREE(rghRows);
	}

CLEANUP:
	pCRowset->ReleaseRows(cRowsObtained, rghRows);
	PROVIDER_FREE(rghRows);
	TRETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyOpenAllInterfaces
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyOpenAllInterfaces(EINTERFACE eInterface, REFGUID rGuidType)
{
	TBEGIN
	HRESULT hr = S_OK;

	//Obtain the Rowset interfaces...
	ULONG cIIDs = 0;
	INTERFACEMAP* rgIIDs = NULL;
	GetInterfaceArray(eInterface, &cIIDs, &rgIIDs);

	//Loop through all the columns...
	for(ULONG iCol=0; iCol<m_pCRowObject->m_cColAccess; iCol++)
	{
		//For every [MANDATORY] interface
		for(ULONG iInterface=0; iInterface<cIIDs; iInterface++)
		{
			//IRow::Open
			//All interface testing is done with the IRow::Open helper...
			hr = m_pCRowObject->Open(NULL, &m_pCRowObject->m_rgColAccess[iCol].columnid, rGuidType, *rgIIDs[iInterface].pIID);
					
			//Determine results
			if(rgIIDs[iInterface].fMandatory)
			{
				//[MANDATORY]
				if(hr!=S_OK && hr!=DB_E_NOTFOUND && hr!=DB_E_OBJECTMISMATCH)
				{
					CHECK(hr, S_OK);
					TOUTPUT_(L"ERROR: Interface Incorrect for " << GetInterfaceName(*rgIIDs[iInterface].pIID) << "\n");
				}
			}
			else
			{
				//[OPTIONAL]
				if(hr!=S_OK && hr!=DB_E_NOTFOUND && hr!=DB_E_OBJECTMISMATCH && hr!=E_NOINTERFACE)
				{
					CHECK(hr, S_OK);
					TOUTPUT_(L"ERROR: Interface Incorrect for " << GetInterfaceName(*rgIIDs[iInterface].pIID) << "\n");
				}
			}
		}
	}

	TRETURN
}

		


////////////////////////////////////////////////////////////////////////////
//  TCIRow::VerifyOpenWithOpenObjects
//
////////////////////////////////////////////////////////////////////////////
BOOL TCIRow::VerifyOpenWithOpenObjects(REFGUID rGuidType, REFIID riid)
{
	TBEGIN
	IUnknown* pIUnknown = NULL;
	IUnknown* pIUnknown2 = NULL;
	IUnknown* pIRowset = NULL;
	IUnknown* pIRowset2 = NULL;

	ULONG cBytes, cBytes2;
	BYTE pBuffer[5000];
	BYTE pBuffer2[5000];
	
	//Loop through all the columns...
	for(ULONG iCol=0; iCol<m_pCRowObject->m_cColAccess; iCol++)
	{
		//Open the column as a stream object (and keep it open)
		HRESULT hr = m_pCRowObject->Open(NULL, &m_pCRowObject->m_rgColAccess[iCol].columnid, rGuidType, riid, &pIUnknown);
		TEST3C_(hr, S_OK, DB_E_NOTFOUND, DB_E_OBJECTMISMATCH);

		if(hr==S_OK)
		{
			if(rGuidType == DBGUID_STREAM)
			{
				//Read the first stream
				TEST2C_(StorageRead(riid, pIUnknown, pBuffer, sizeof(pBuffer), &cBytes), S_OK, S_FALSE);
				TESTC(cBytes <= 5000);
			}
			else if(rGuidType == DBGUID_ROW)
			{
				//GetSourceRowset
				ASSERT(riid == IID_IRow);
				TEST2C_(((IRow*)pIUnknown)->GetSourceRowset(IID_IRowset, &pIRowset, NULL),S_OK,DB_E_NOSOURCEOBJECT);
			}

			//Try Opening the Object again
			hr = m_pCRowObject->Open(NULL, &m_pCRowObject->m_rgColAccess[iCol].columnid, rGuidType, riid, &pIUnknown2);
			TEST4C_(hr, S_OK, DB_E_NOTFOUND, DB_E_OBJECTMISMATCH, DB_E_OBJECTOPEN);

			if(hr==S_OK)
			{
				if(rGuidType == DBGUID_STREAM)
				{
					//Read the Second Stream
					TEST2C_(StorageRead(riid, pIUnknown2, pBuffer2, sizeof(pBuffer2), &cBytes2), S_OK, S_FALSE);
					TESTC(cBytes == cBytes2);

					//Compare the Objects...
					TESTC(memcmp(pBuffer, pBuffer2, cBytes)==0);
				}
				else if(rGuidType == DBGUID_ROW)
				{
					//GetSourceRowset
					ASSERT(riid == IID_IRow);
					TEST2C_(((IRow*)pIUnknown)->GetSourceRowset(IID_IRowset, &pIRowset2, NULL),S_OK,DB_E_NOSOURCEOBJECT);
					if(pIRowset && pIRowset2)
						TESTC(VerifyEqualInterface(pIRowset, pIRowset2));
				}
			}
		}

		//Release all Objects
		SAFE_RELEASE(pIUnknown);
		SAFE_RELEASE(pIUnknown2);
		SAFE_RELEASE(pIRowset);
		SAFE_RELEASE(pIRowset2);
	}

CLEANUP:
	//Release all Objects
	SAFE_RELEASE(pIUnknown);
	SAFE_RELEASE(pIUnknown2);
	SAFE_RELEASE(pIRowset);
	SAFE_RELEASE(pIRowset2);
	TRETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::Thread_VerifyGetColumns
//
////////////////////////////////////////////////////////////////////////////
ULONG TCIRow::Thread_VerifyGetColumns(void* pv)
{
	THREAD_BEGIN

	//Thread Stack Variables
	TCIRow* pThis = (TCIRow*)THREAD_FUNC;
	ASSERT(pThis);

	ThreadSwitch(); //Let the other thread(s) catch up

	//IRow::GetColumns
	TESTC(pThis->VerifyGetColumns(pThis->m_pCRowObject, FIRST_ROW, ALL_COLS_BOUND));

	ThreadSwitch(); //Let the other thread(s) catch up

CLEANUP:
	THREAD_RETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::Thread_VerifyGetSourceRowset
//
////////////////////////////////////////////////////////////////////////////
ULONG TCIRow::Thread_VerifyGetSourceRowset(void* pv)
{
	THREAD_BEGIN

	//Thread Stack Variables
	TCIRow* pThis = (TCIRow*)THREAD_FUNC;
	ASSERT(pThis);

	ThreadSwitch(); //Let the other thread(s) catch up

	//IRow::GetColumns
	TESTC(pThis->VerifyGetSourceRowset(IID_IRowset));
	
	ThreadSwitch(); //Let the other thread(s) catch up

CLEANUP:
	THREAD_RETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::Thread_VerifyOpenWithOpenObjects
//
////////////////////////////////////////////////////////////////////////////
ULONG TCIRow::Thread_VerifyOpenWithOpenObjects(void* pv)
{
	THREAD_BEGIN

	//Thread Stack Variables
	TCIRow* pThis		= (TCIRow*)THREAD_FUNC;
	GUID	rGuidType	= *(GUID*)THREAD_ARG1;
	IID		riid		= *(IID*)THREAD_ARG2;
	ASSERT(pThis);

	ThreadSwitch(); //Let the other thread(s) catch up

	//IRow::Open
	TESTC(pThis->VerifyOpenWithOpenObjects(rGuidType, riid));
	
	ThreadSwitch(); //Let the other thread(s) catch up

CLEANUP:
	THREAD_RETURN
}


////////////////////////////////////////////////////////////////////////////
//  TCIRow::pIRow
//
////////////////////////////////////////////////////////////////////////////
IRow* const TCIRow::pIRow()
{
	ASSERT(m_pCRowObject);
	return m_pCRowObject->pIRow();
}


// {{ TCW_TEST_CASE_MAP(TCIRow_IUnknown)
//*-----------------------------------------------------------------------
// @class IRow IUnknown scenarios
//
class TCIRow_IUnknown : public TCIRow { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCIRow_IUnknown,TCIRow);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember IUnknown - QI Mandatory Interfaces
	int Variation_1();
	// @cmember IUnknown - QI Optional Interfaces
	int Variation_2();
	// @cmember IUnknown - AddRef / Release
	int Variation_3();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCIRow_IUnknown)
#define THE_CLASS TCIRow_IUnknown
BEG_TEST_CASE(TCIRow_IUnknown, TCIRow, L"IRow IUnknown scenarios")
	TEST_VARIATION(1, 		L"IUnknown - QI Mandatory Interfaces")
	TEST_VARIATION(2, 		L"IUnknown - QI Optional Interfaces")
	TEST_VARIATION(3, 		L"IUnknown - AddRef / Release")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCIRow_GetColumns)
//*-----------------------------------------------------------------------
// @class IRow::GetColumns
//
class TCIRow_GetColumns : public TCIRow { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCIRow_GetColumns,TCIRow);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember GetColumns - All columns - no BLOBs
	int Variation_1();
	// @cmember GetColumns - All Columns - BLOBs
	int Variation_2();
	// @cmember GetColumns - same column bound numerous times
	int Variation_3();
	// @cmember GetColumns - BLOB Columns only
	int Variation_4();
	// @cmember GetColumns - Each Column Seperatly
	int Variation_5();
	// @cmember GetColumns - Asking for just Rowset columns
	int Variation_6();
	// @cmember GetColumns - Asking for just Extra Row Columns
	int Variation_7();
	// @cmember GetColumns - IUnknown Columns - native
	int Variation_8();
	// @cmember Empty
	int Variation_9();
	// @cmember GetColumns - Not Binding Value, pData is NULL
	int Variation_10();
	// @cmember GetColumns - Value not bound for some columns
	int Variation_11();
	// @cmember Empty
	int Variation_12();
	// @cmember Vectors - Some valid, some non-existent columns - DB_S_ERRORSOCCURRED
	int Variation_13();
	// @cmember Vectors - All non-existent columns - DB_E_ERRORSOCCURRED
	int Variation_14();
	// @cmember Vectors - No Vector Columns - S_OK
	int Variation_15();
	// @cmember Vectors - No Vectors and Non-Existent Columns - DB_E_ERRORSOCCURRED
	int Variation_16();
	// @cmember Vectors - Only Vector Columns - S_OK
	int Variation_17();
	// @cmember Vectors - Only Non-Existent Vector Columns - DB_E_ERRORSOCCURRED
	int Variation_18();
	// @cmember Vectors - Valid Vectors and Non-Existent Columns - DB_S_ERRORSOCCURRED
	int Variation_19();
	// @cmember Vectors - Valid Non-Vectors and Non-Existent Vector Columns - DB_S_ERRORSOCCURRED
	int Variation_20();
	// @cmember Empty
	int Variation_21();
	// @cmember Boundary - 0 columns -[0, NULL] and [0, valid]  no-op
	int Variation_22();
	// @cmember Boundary - [Valid, NULL] - E_INVALIDARG
	int Variation_23();
	// @cmember Empty
	int Variation_24();
	// @cmember Boundary - columnid - all valid dbkinds
	int Variation_25();
	// @cmember Boundary - columnid - invalid dbkinds
	int Variation_26();
	// @cmember Empty
	int Variation_27();
	// @cmember Boundary - cbMaxLen - ignored for fixed length types
	int Variation_28();
	// @cmember Boundary - cbMaxLen - DBSTATUS_S_TRUNCATION
	int Variation_29();
	// @cmember Boundary - cbMaxLen - invalid
	int Variation_30();
	// @cmember Empty
	int Variation_31();
	// @cmember Boundary - wType - all valid types and modifieres
	int Variation_32();
	// @cmember Boundary - wType - invalid types and invalid type modifiers
	int Variation_33();
	// @cmember Empty
	int Variation_34();
	// @cmember Boundary - bPrecision - make sure ignored on input for all types, except NUMERIC.
	int Variation_35();
	// @cmember Boundary - bScale - make sure ignored on input for all types, except NUMERIC.
	int Variation_36();
	// @cmember Boundary - Output Only - make sure cbDataLen, dwStatus, dwReserverd are ignored on input
	int Variation_37();
	// @cmember Boundary - Input - Make sure all pointers and input args are not changed on output, pData pointer, columnid [including all union pointers], cbMaxLen, dwReserved, wType.
	int Variation_38();
	// @cmember Empty
	int Variation_39();
	// @cmember Empty
	int Variation_40();
	// @cmember Empty
	int Variation_41();
	// @cmember Empty
	int Variation_42();
	// @cmember Buffered Mode - All Columns - no BLOBs
	int Variation_43();
	// @cmember Buffered Mode - All Columns - BLOBs
	int Variation_44();
	// @cmember Buffered Mode - All Columns - Just extra columns
	int Variation_45();
	// @cmember Empty
	int Variation_46();
	// @cmember Aggregation - Command - > Row
	int Variation_47();
	// @cmember Aggregation - Session - > Row
	int Variation_48();
	// @cmember Aggregation - Rowset -> Row
	int Variation_49();
	// @cmember Empty
	int Variation_50();
	// @cmember Empty
	int Variation_51();
	// @cmember Properties - DBPROP_IRow
	int Variation_52();
	// @cmember Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_ROWOBJECT
	int Variation_53();
	// @cmember Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_SINGLETON
	int Variation_54();
	// @cmember Empty
	int Variation_55();
	// @cmember Properties - All Row Object Properties
	int Variation_56();
	// @cmember Properties - All Row Object Properties Optional
	int Variation_57();
	// @cmember Empty
	int Variation_58();
	// @cmember Threads - GetColumns seperate threads
	int Variation_59();
	// @cmember Empty
	int Variation_60();
	// @cmember Boundary - DB_E_DELETEDROW
	int Variation_61();
	// @cmember Boundary - DB_E_DELETEDROW - Bufferred Mode
	int Variation_62();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCIRow_GetColumns)
#define THE_CLASS TCIRow_GetColumns
BEG_TEST_CASE(TCIRow_GetColumns, TCIRow, L"IRow::GetColumns")
	TEST_VARIATION(1, 		L"GetColumns - All columns - no BLOBs")
	TEST_VARIATION(2, 		L"GetColumns - All Columns - BLOBs")
	TEST_VARIATION(3, 		L"GetColumns - same column bound numerous times")
	TEST_VARIATION(4, 		L"GetColumns - BLOB Columns only")
	TEST_VARIATION(5, 		L"GetColumns - Each Column Seperatly")
	TEST_VARIATION(6, 		L"GetColumns - Asking for just Rowset columns")
	TEST_VARIATION(7, 		L"GetColumns - Asking for just Extra Row Columns")
	TEST_VARIATION(8, 		L"GetColumns - IUnknown Columns - native")
	TEST_VARIATION(9, 		L"Empty")
	TEST_VARIATION(10, 		L"GetColumns - Not Binding Value, pData is NULL")
	TEST_VARIATION(11, 		L"GetColumns - Value not bound for some columns")
	TEST_VARIATION(12, 		L"Empty")
	TEST_VARIATION(13, 		L"Vectors - Some valid, some non-existent columns - DB_S_ERRORSOCCURRED")
	TEST_VARIATION(14, 		L"Vectors - All non-existent columns - DB_E_ERRORSOCCURRED")
	TEST_VARIATION(15, 		L"Vectors - No Vector Columns - S_OK")
	TEST_VARIATION(16, 		L"Vectors - No Vectors and Non-Existent Columns - DB_E_ERRORSOCCURRED")
	TEST_VARIATION(17, 		L"Vectors - Only Vector Columns - S_OK")
	TEST_VARIATION(18, 		L"Vectors - Only Non-Existent Vector Columns - DB_E_ERRORSOCCURRED")
	TEST_VARIATION(19, 		L"Vectors - Valid Vectors and Non-Existent Columns - DB_S_ERRORSOCCURRED")
	TEST_VARIATION(20, 		L"Vectors - Valid Non-Vectors and Non-Existent Vector Columns - DB_S_ERRORSOCCURRED")
	TEST_VARIATION(21, 		L"Empty")
	TEST_VARIATION(22, 		L"Boundary - 0 columns -[0, NULL] and [0, valid]  no-op")
	TEST_VARIATION(23, 		L"Boundary - [Valid, NULL] - E_INVALIDARG")
	TEST_VARIATION(24, 		L"Empty")
	TEST_VARIATION(25, 		L"Boundary - columnid - all valid dbkinds")
	TEST_VARIATION(26, 		L"Boundary - columnid - invalid dbkinds")
	TEST_VARIATION(27, 		L"Empty")
	TEST_VARIATION(28, 		L"Boundary - cbMaxLen - ignored for fixed length types")
	TEST_VARIATION(29, 		L"Boundary - cbMaxLen - DBSTATUS_S_TRUNCATION")
	TEST_VARIATION(30, 		L"Boundary - cbMaxLen - invalid")
	TEST_VARIATION(31, 		L"Empty")
	TEST_VARIATION(32, 		L"Boundary - wType - all valid types and modifieres")
	TEST_VARIATION(33, 		L"Boundary - wType - invalid types and invalid type modifiers")
	TEST_VARIATION(34, 		L"Empty")
	TEST_VARIATION(35, 		L"Boundary - bPrecision - make sure ignored on input for all types, except NUMERIC.")
	TEST_VARIATION(36, 		L"Boundary - bScale - make sure ignored on input for all types, except NUMERIC.")
	TEST_VARIATION(37, 		L"Boundary - Output Only - make sure cbDataLen, dwStatus, dwReserverd are ignored on input")
	TEST_VARIATION(38, 		L"Boundary - Input - Make sure all pointers and input args are not changed on output, pData pointer, columnid [including all union pointers], cbMaxLen, dwReserved, wType.")
	TEST_VARIATION(39, 		L"Empty")
	TEST_VARIATION(40, 		L"Empty")
	TEST_VARIATION(41, 		L"Empty")
	TEST_VARIATION(42, 		L"Empty")
	TEST_VARIATION(43, 		L"Buffered Mode - All Columns - no BLOBs")
	TEST_VARIATION(44, 		L"Buffered Mode - All Columns - BLOBs")
	TEST_VARIATION(45, 		L"Buffered Mode - All Columns - Just extra columns")
	TEST_VARIATION(46, 		L"Empty")
	TEST_VARIATION(47, 		L"Aggregation - Command - > Row")
	TEST_VARIATION(48, 		L"Aggregation - Session - > Row")
	TEST_VARIATION(49, 		L"Aggregation - Rowset -> Row")
	TEST_VARIATION(50, 		L"Empty")
	TEST_VARIATION(51, 		L"Empty")
	TEST_VARIATION(52, 		L"Properties - DBPROP_IRow")
	TEST_VARIATION(53, 		L"Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_ROWOBJECT")
	TEST_VARIATION(54, 		L"Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_SINGLETON")
	TEST_VARIATION(55, 		L"Empty")
	TEST_VARIATION(56, 		L"Properties - All Row Object Properties")
	TEST_VARIATION(57, 		L"Properties - All Row Object Properties Optional")
	TEST_VARIATION(58, 		L"Empty")
	TEST_VARIATION(59, 		L"Threads - GetColumns seperate threads")
	TEST_VARIATION(60, 		L"Empty")
	TEST_VARIATION(61, 		L"Boundary - DB_E_DELETEDROW")
	TEST_VARIATION(62, 		L"Boundary - DB_E_DELETEDROW - Bufferred Mode")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCIRow_GetSourceRowset)
//*-----------------------------------------------------------------------
// @class IRow::GetSourceRowset
//
class TCIRow_GetSourceRowset : public TCIRow { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCIRow_GetSourceRowset,TCIRow);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember riid - All Mandatory interfaces
	int Variation_1();
	// @cmember Empty
	int Variation_2();
	// @cmember Optional Args - [any riid, NULL, NULL] - S_OK
	int Variation_3();
	// @cmember Optional Args - [any riid, NULL, valid] - S_OK with hRow
	int Variation_4();
	// @cmember Optional Args - [IID_IUnknown, valid, NULL] - S_OK with Rowset
	int Variation_5();
	// @cmember Optional Args - [IID_IRowset, valid, valid] - S_OK with Rowset and hRow
	int Variation_6();
	// @cmember Empty
	int Variation_7();
	// @cmember Sequence - Execute -> Row -> GetSourceRowset
	int Variation_8();
	// @cmember Sequence - Execute -> Rowset -> Row -> GetSourceRowset
	int Variation_9();
	// @cmember Sequence - OpenRowset -> Row -> Release Rowset -> GetSourceRowset
	int Variation_10();
	// @cmember Sequence - OpenRowset -> Rowset -> Row -> Release Rowset -> GetSourceRowset
	int Variation_11();
	// @cmember Empty
	int Variation_12();
	// @cmember Multiple - GetSourceRowset from numerous child row objects
	int Variation_13();
	// @cmember Multiple - Numerous Rowsets open, verify correct parent
	int Variation_14();
	// @cmember Empty
	int Variation_15();
	// @cmember Aggregation - Rowset -> Agg Row -> GetSourceRowset
	int Variation_16();
	// @cmember Aggregation - Agg Rowset -> Row -> GetSourceRowset
	int Variation_17();
	// @cmember Empty
	int Variation_18();
	// @cmember Threads - GetSourceRowset
	int Variation_19();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCIRow_GetSourceRowset)
#define THE_CLASS TCIRow_GetSourceRowset
BEG_TEST_CASE(TCIRow_GetSourceRowset, TCIRow, L"IRow::GetSourceRowset")
	TEST_VARIATION(1, 		L"riid - All Mandatory interfaces")
	TEST_VARIATION(2, 		L"Empty")
	TEST_VARIATION(3, 		L"Optional Args - [any riid, NULL, NULL] - S_OK")
	TEST_VARIATION(4, 		L"Optional Args - [any riid, NULL, valid] - S_OK with hRow")
	TEST_VARIATION(5, 		L"Optional Args - [IID_IUnknown, valid, NULL] - S_OK with Rowset")
	TEST_VARIATION(6, 		L"Optional Args - [IID_IRowset, valid, valid] - S_OK with Rowset and hRow")
	TEST_VARIATION(7, 		L"Empty")
	TEST_VARIATION(8, 		L"Sequence - Execute -> Row -> GetSourceRowset")
	TEST_VARIATION(9, 		L"Sequence - Execute -> Rowset -> Row -> GetSourceRowset")
	TEST_VARIATION(10, 		L"Sequence - OpenRowset -> Row -> Release Rowset -> GetSourceRowset")
	TEST_VARIATION(11, 		L"Sequence - OpenRowset -> Rowset -> Row -> Release Rowset -> GetSourceRowset")
	TEST_VARIATION(12, 		L"Empty")
	TEST_VARIATION(13, 		L"Multiple - GetSourceRowset from numerous child row objects")
	TEST_VARIATION(14, 		L"Multiple - Numerous Rowsets open, verify correct parent")
	TEST_VARIATION(15, 		L"Empty")
	TEST_VARIATION(16, 		L"Aggregation - Rowset -> Agg Row -> GetSourceRowset")
	TEST_VARIATION(17, 		L"Aggregation - Agg Rowset -> Row -> GetSourceRowset")
	TEST_VARIATION(18, 		L"Empty")
	TEST_VARIATION(19, 		L"Threads - GetSourceRowset")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCIRow_Open)
//*-----------------------------------------------------------------------
// @class IRow::Open
//
class TCIRow_Open : public TCIRow { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCIRow_Open,TCIRow);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	// {{ TCW_TESTVARS()
	// @cmember GUID_NULL - All columns as Default Object
	int Variation_1();
	// @cmember DBGUID_STREAM - All Mandatory and Optional TStream interfaces
	int Variation_2();
	// @cmember DBGUID_STREAM - Compare data with expected
	int Variation_3();
	// @cmember DBGUID_STREAM - Twice on the same column
	int Variation_4();
	// @cmember DBGUID_STREAM - Multiple Objects open on different columns
	int Variation_5();
	// @cmember DBGUID_STREAM - Bufferred Mode
	int Variation_6();
	// @cmember Empty
	int Variation_7();
	// @cmember DBGUID_ROW - All Mandatory and Optional TRow interfaces
	int Variation_8();
	// @cmember DBGUID_ROW - Get all row columns and verify
	int Variation_9();
	// @cmember DBGUID_ROW - GetSourceRowset
	int Variation_10();
	// @cmember DBGUID_ROW - Twice on the same column
	int Variation_11();
	// @cmember DBGUID_ROW - Multiple Objects open on different columns
	int Variation_12();
	// @cmember DBGUID_ROW - Bufferred Mode
	int Variation_13();
	// @cmember Empty
	int Variation_14();
	// @cmember DBGUID_ROWSET - All Mandatory and Optional TRowset interfaces
	int Variation_15();
	// @cmember DBGUID_ROWSET - Twice on the same column
	int Variation_16();
	// @cmember DBGUID_ROWSET - Buffered mode
	int Variation_17();
	// @cmember Empty
	int Variation_18();
	// @cmember DEFAULTSTREAM
	int Variation_19();
	// @cmember DEFAULTSTREAM - Buffered mode
	int Variation_20();
	// @cmember Empty
	int Variation_21();
	// @cmember Boundary - E_INVALIDARG
	int Variation_22();
	// @cmember Boundary - DB_E_BADCOLUMNID - invalid DBID
	int Variation_23();
	// @cmember Boundary - DB_E_BADCOLUMNID - shortcut DBID that doesn't exist
	int Variation_24();
	// @cmember Boundary - DB_E_OBJECTMISMATCH
	int Variation_25();
	// @cmember Boundary - DB_E_OBJECTOPEN
	int Variation_26();
	// @cmember Boundary - DB_E_NOTFOUND
	int Variation_27();
	// @cmember Boundary - DB_E_DELETEDROW
	int Variation_28();
	// @cmember Empty
	int Variation_29();
	// @cmember Aggregation - Row -> Open -> GetSourceRow
	int Variation_30();
	// @cmember Empty
	int Variation_31();
	// @cmember Threads - Open over the same column
	int Variation_32();
	// @cmember Threads - Open over different columns
	int Variation_33();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCIRow_Open)
#define THE_CLASS TCIRow_Open
BEG_TEST_CASE(TCIRow_Open, TCIRow, L"IRow::Open")
	TEST_VARIATION(1, 		L"GUID_NULL - All columns as Default Object")
	TEST_VARIATION(2, 		L"DBGUID_STREAM - All Mandatory and Optional TStream interfaces")
	TEST_VARIATION(3, 		L"DBGUID_STREAM - Compare data with expected")
	TEST_VARIATION(4, 		L"DBGUID_STREAM - Twice on the same column")
	TEST_VARIATION(5, 		L"DBGUID_STREAM - Multiple Objects open on different columns")
	TEST_VARIATION(6, 		L"DBGUID_STREAM - Bufferred Mode")
	TEST_VARIATION(7, 		L"Empty")
	TEST_VARIATION(8, 		L"DBGUID_ROW - All Mandatory and Optional TRow interfaces")
	TEST_VARIATION(9, 		L"DBGUID_ROW - Get all row columns and verify")
	TEST_VARIATION(10, 		L"DBGUID_ROW - GetSourceRowset")
	TEST_VARIATION(11, 		L"DBGUID_ROW - Twice on the same column")
	TEST_VARIATION(12, 		L"DBGUID_ROW - Multiple Objects open on different columns")
	TEST_VARIATION(13, 		L"DBGUID_ROW - Bufferred Mode")
	TEST_VARIATION(14, 		L"Empty")
	TEST_VARIATION(15, 		L"DBGUID_ROWSET - All Mandatory and Optional TRowset interfaces")
	TEST_VARIATION(16, 		L"DBGUID_ROWSET - Twice on the same column")
	TEST_VARIATION(17, 		L"DBGUID_ROWSET - Buffered mode")
	TEST_VARIATION(18, 		L"Empty")
	TEST_VARIATION(19, 		L"DEFAULTSTREAM")
	TEST_VARIATION(20, 		L"DEFAULTSTREAM - Buffered mode")
	TEST_VARIATION(21, 		L"Empty")
	TEST_VARIATION(22, 		L"Boundary - E_INVALIDARG")
	TEST_VARIATION(23, 		L"Boundary - DB_E_BADCOLUMNID - invalid DBID")
	TEST_VARIATION(24, 		L"Boundary - DB_E_BADCOLUMNID - shortcut DBID that doesn't exist")
	TEST_VARIATION(25, 		L"Boundary - DB_E_OBJECTMISMATCH")
	TEST_VARIATION(26, 		L"Boundary - DB_E_OBJECTOPEN")
	TEST_VARIATION(27, 		L"Boundary - DB_E_NOTFOUND")
	TEST_VARIATION(28, 		L"Boundary - DB_E_DELETEDROW")
	TEST_VARIATION(29, 		L"Empty")
	TEST_VARIATION(30, 		L"Aggregation - Row -> Open -> GetSourceRow")
	TEST_VARIATION(31, 		L"Empty")
	TEST_VARIATION(32, 		L"Threads - Open over the same column")
	TEST_VARIATION(33, 		L"Threads - Open over different columns")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END

// {{ TCW_TEST_CASE_MAP(TCTransactions)
//*-----------------------------------------------------------------------
// @class IRow inside Transactions
//
class TCTransactions : public CTransaction { 
private:
	// @cmember Static array of variations
	DECLARE_TEST_CASE_DATA();

public:
	// {{ TCW_DECLARE_FUNCS
	// @cmember Execution Routine
	DECLARE_TEST_CASE_FUNCS(TCTransactions,CTransaction);
	// }} TCW_DECLARE_FUNCS_END
	
	// @cmember Initialization Routine
	virtual BOOL Init();
	// @cmember Termination Routine
	virtual BOOL Terminate();

	//Helpers
	virtual BOOL VerifyTransaction(BOOL fCommit, BOOL fRetaining);

	// {{ TCW_TESTVARS()
	// @cmember ABORT with fRetaining TRUE
	int Variation_1();
	// @cmember ABORT with fRetaining FALSE
	int Variation_2();
	// @cmember COMMIT with fRetaining TRUE
	int Variation_3();
	// @cmember COMMIT with fRetaining FALSE
	int Variation_4();
	// }} TCW_TESTVARS_END
} ;
// {{ TCW_TESTCASE(TCTransactions)
#define THE_CLASS TCTransactions
BEG_TEST_CASE(TCTransactions, CTransaction, L"IRow inside Transactions")
	TEST_VARIATION(1, 		L"ABORT with fRetaining TRUE")
	TEST_VARIATION(2, 		L"ABORT with fRetaining FALSE")
	TEST_VARIATION(3, 		L"COMMIT with fRetaining TRUE")
	TEST_VARIATION(4, 		L"COMMIT with fRetaining FALSE")
END_TEST_CASE()
#undef THE_CLASS
// }} TCW_TESTCASE_END
// }} TCW_TEST_CASE_MAP_END



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test Case Section
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// }} END_DECLARE_TEST_CASES()

// {{ TCW_TESTMODULE(ThisModule)
TEST_MODULE(5, ThisModule, gwszModuleDescrip)
	TEST_CASE(1, TCIRow_IUnknown)
	TEST_CASE(2, TCIRow_GetColumns)
	TEST_CASE(3, TCIRow_GetSourceRowset)
	TEST_CASE(4, TCIRow_Open)
	TEST_CASE(5, TCTransactions)
END_TEST_MODULE()
// }} TCW_TESTMODULE_END



////////////////////////////////////////////////////////////////////////////
//  TCTransactionsVerifyTransaction
//
////////////////////////////////////////////////////////////////////////////
BOOL TCTransactions::VerifyTransaction(BOOL fCommit, BOOL fRetaining)
{
	HROW				hRow;
	IGetRow*			pIGetRow = NULL;
	BOOL				fPreserving = FALSE;
	CRowset				RowsetA;
	CRowObject			RowObjectA;
	ISequentialStream*	pISeqStream = NULL;
	IRowsetInfo*		pIRowsetInfo = NULL;
	HROW				hSourceRow = NULL;

	//start a transaction
	TESTC(StartTransaction(SELECT_ALLFROMTBL, (IUnknown**)&pIGetRow, 0, NULL));
	TESTC_(RowsetA.CreateRowset(pIGetRow),S_OK);

	//Obtain a row object - before we commit/abort the transaction
	TESTC_(RowsetA.GetNextRows(&hRow),S_OK);
	TESTC_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hRow),S_OK);

	//commit the transaction with fRetaining==TRUE
	if(fCommit)
	{
		TESTC(GetCommit(fRetaining))
		fPreserving = m_fCommitPreserve;
	}
	else
	{
		TESTC(GetAbort(fRetaining))
		fPreserving = m_fAbortPreserve;
	}
	
	if(fPreserving)
	{
		//fully functional

		//IRow::GetColumns
		TESTC(RowObjectA.VerifyGetColumns(FIRST_ROW, RowsetA.pTable()));

		//IRow::GetSourceRowset
		TESTC_(RowObjectA.GetSourceRowset(IID_IRowsetInfo, (IUnknown**)&pIRowsetInfo, NULL), S_OK);
		TESTC(DefaultObjectTesting(pIRowsetInfo, ROWSET_INTERFACE));

		//IRow::Open (open the default stream - note it may not exist for every row)
		//NOTE: Our helper takes care of all validation of the returned object...
		TEST2C_(RowObjectA.Open(NULL, &DBROWCOL_DEFAULTSTREAM, DBGUID_STREAM, 
			IID_ISequentialStream, (IUnknown**)&pISeqStream),S_OK, DB_E_BADCOLUMNID);
	}
	else
	{
		//zombie

		//IRow::GetColumns
		TESTC_(RowObjectA.GetColumns(RowObjectA.m_cColAccess, RowObjectA.m_rgColAccess),E_UNEXPECTED);

		//IRow::GetSourceRowset
		TESTC_(RowObjectA.GetSourceRowset(IID_IRowsetInfo, (IUnknown**)&pIRowsetInfo, &hSourceRow), E_UNEXPECTED);
		TESTC(hSourceRow == NULL);
		TESTC(pIRowsetInfo == NULL);

		//IRow::GetSourceRowset
		//This time not asking for an HROW, as its more difficult for the provider to determine
		//zombie, if just getting a parent object, vs. row handle (ie: ConfProv)
		TESTC_(RowObjectA.GetSourceRowset(IID_IRowsetInfo, (IUnknown**)&pIRowsetInfo, NULL), E_UNEXPECTED);
		TESTC(pIRowsetInfo == NULL);

		//IRow::Open (open the default stream - note it may not exist for every row)
		//NOTE: Our helper takes care of all validation of the returned object...
		TESTC_(RowObjectA.Open(NULL, &DBROWCOL_DEFAULTSTREAM, DBGUID_STREAM, 
			IID_ISequentialStream, (IUnknown**)&pISeqStream), E_UNEXPECTED);
		TESTC(pISeqStream == NULL);
		
	}

CLEANUP:
	SAFE_RELEASE(pIGetRow);
	SAFE_RELEASE(pISeqStream);
	SAFE_RELEASE(pIRowsetInfo);
	 
	//clean up.
	CleanUpTransaction(fRetaining ? S_OK : XACT_E_NOTRANSACTION);
	TRETURN
}



// {{ TCW_TC_PROTOTYPE(TCIRow_IUnknown)
//*-----------------------------------------------------------------------
//| Test Case:		TCIRow_IUnknown - IRow IUnknown scenarios
//| Created:  	8/5/98
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCIRow_IUnknown::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCIRow::Init())
	// }}
	{ 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc IUnknown - QI Mandatory Interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_IUnknown::Variation_1()
{ 
	//Do some default IUnknown interface testing
	return DefaultObjectTesting(pIRow(), ROW_INTERFACE);
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc IUnknown - QI Optional Interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_IUnknown::Variation_2()
{ 
	//Do some default IUnknown interface testing
	return DefaultObjectTesting(pIRow(), ROW_INTERFACE);
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc IUnknown - AddRef / Release
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_IUnknown::Variation_3()
{ 
	//Do some default IUnknown interface testing
	return DefaultObjectTesting(pIRow(), ROW_INTERFACE);
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCIRow_IUnknown::Terminate()
{ 
// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCIRow::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCIRow_GetColumns)
//*-----------------------------------------------------------------------
//| Test Case:		TCIRow_GetColumns - IRow::GetColumns
//| Created:  	8/5/98
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCIRow_GetColumns::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCIRow::Init())
	// }}
	{ 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - All columns - no BLOBs
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_1()
{ 
	TBEGIN

	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - All Columns - BLOBs
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_2()
{ 
	TBEGIN
	
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, ALL_COLS_BOUND, BLOB_LONG));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - same column bound numerous times
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_3()
{ 
	TBEGIN
	IColumnsInfo* pIColumnsInfo = NULL;
	ULONG i,cColumns=0;	
	DBCOLUMNINFO* rgColumnInfo = NULL;
	WCHAR* pStringBuffer = NULL;
	ULONG  cColOrds = 0;
	ULONG* rgColOrds = NULL;

	//Use a new rowset, and ask for a non-forward-only cursor, 
	//so we can obtain the data multiple times.
	CRowset RowsetA;
	RowsetA.SetSettableProperty(DBPROP_IRowsetLocate);
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Get the ColumnInfo
	TESTC(VerifyInterface(pIRow(), IID_IColumnsInfo, ROW_INTERFACE, (IUnknown**)&pIColumnsInfo));
	TESTC_(pIColumnsInfo->GetColumnInfo(&cColumns, &rgColumnInfo, &pStringBuffer),S_OK);
	SAFE_ALLOC(rgColOrds, ULONG, cColumns);

	//Loop through each column seperatly...
	for(i=0; i<cColumns; i++)
	{
		//Fill in the Col Ordinals with nukerous duplicates
		cColOrds = 0;
		for(ULONG iDup=0; iDup<i; iDup++)
		{
			rgColOrds[iDup] = rgColumnInfo[i].iOrdinal;
			cColOrds++;	
		}
		
		//Loop through all the rows in the rowset, verify the columns...
		TESTC(VerifyGetColumnsAllRows(&RowsetA, USE_COLS_TO_BIND_ARRAY, BLOB_LONG, FORWARD,
					NO_COLS_BY_REF,	DBTYPE_EMPTY, cColOrds, rgColOrds));
	}

CLEANUP:
	SAFE_FREE(rgColumnInfo);
	SAFE_FREE(pStringBuffer);
	SAFE_RELEASE(pIColumnsInfo);
	SAFE_FREE(rgColOrds);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - BLOB Columns only
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_4()
{ 
	TBEGIN
	
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, BLOB_COLS_BOUND, BLOB_LONG));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - Each Column Seperatly
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_5()
{ 
	TBEGIN
	IColumnsInfo* pIColumnsInfo = NULL;
	ULONG i,cColumns=0;	
	DBCOLUMNINFO* rgColumnInfo = NULL;
	WCHAR* pStringBuffer = NULL;

	//Get the ColumnInfo
	TESTC(VerifyInterface(pIRow(), IID_IColumnsInfo, ROW_INTERFACE, (IUnknown**)&pIColumnsInfo));
	TESTC_(pIColumnsInfo->GetColumnInfo(&cColumns, &rgColumnInfo, &pStringBuffer),S_OK);

	//Loop through each column seperatly...
	for(i=0; i<cColumns; i++)
	{
		//Loop through all the rows in the rowset, verify the columns...
		TESTC(VerifyGetColumnsAllRows(NULL, USE_COLS_TO_BIND_ARRAY, BLOB_LONG, FORWARD,		
					NO_COLS_BY_REF,	DBTYPE_EMPTY, 1, &rgColumnInfo[i].iOrdinal));
	}

CLEANUP:
	SAFE_FREE(rgColumnInfo);
	SAFE_FREE(pStringBuffer);
	SAFE_RELEASE(pIColumnsInfo);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - Asking for just Rowset columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_6()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	ULONG cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;

	//Create ColAccess structures from the Rowset bindings
	TESTC_(hr = m_pCRowObject->BindingsToColAccess(m_cBindings, m_rgBinding, m_pData, &cColAccess, &rgColAccess),S_OK);
	
	//IRow::GetColumns
	TESTC_(hr = m_pCRowObject->GetColumns(cColAccess, rgColAccess),S_OK);

	//Compare Data for this row object
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - Asking for just Extra Row Columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_7()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	ULONG cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;

	ULONG cColumns = 0;
	ULONG* rgColOrdinals = NULL;

	//Obtain the just the Extra columns
	TESTC_(hr = m_pCRowObject->GetExtraColumnInfo(&cColumns, NULL, NULL, &rgColOrdinals),S_OK);

	//Create the ColAccess Structures for just the extra columns...
	TESTC_(hr = m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, USE_COLS_TO_BIND_ARRAY, BLOB_LONG, FORWARD, NO_COLS_BY_REF, DBTYPE_EMPTY, cColumns, rgColOrdinals),S_OK);
							 
	//IRow::GetColumns
	TESTC_(hr = m_pCRowObject->GetColumns(cColAccess, rgColAccess),S_OK);

	//Compare Data for this row object
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	SAFE_FREE(rgColOrdinals);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - IUnknown Columns - native
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_8()
{ 
	TBEGIN
	
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, BLOB_COLS_BOUND, BLOB_IID_IUNKNOWN));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_9()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - Not Binding Value, pData is NULL
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_10()
{ 
	TBEGIN

	//Only Bind the LENGTH and STATUS (pData == NULL - indicates VALUE is not bound)
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, ALL_COLS_BOUND, BLOB_LONG, FORWARD, NO_COLS_BY_REF, DBTYPE_EMPTY, 0, NULL, DBPART_LENGTH|DBPART_STATUS));

	//Also make sure we bind the IUnknown columns as NULL as well...
	TESTC(VerifyGetColumnsAllRows(NULL, ALL_COLS_BOUND, BLOB_IID_IUNKNOWN, FORWARD, NO_COLS_BY_REF, DBTYPE_EMPTY, 0, NULL, DBPART_LENGTH|DBPART_STATUS));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc GetColumns - Value not bound for some columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_11()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	ULONG i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;

	//Create ColAccess structures from the Rowset bindings
	TESTC_(hr = m_pCRowObject->BindingsToColAccess(m_cBindings, m_rgBinding, m_pData, &cColAccess, &rgColAccess),S_OK);
	
	//For some columns, don't bind the value
	for(i=0; i<cColAccess; i++)
	{
		//Don't bind value for every other column...
		//NOTE: We have one allocated pData that all the structures point into, so there
		//is nothing to free, we just set this pointer to NULL...
		if(i%2)
			rgColAccess[i].pData = NULL;
	}

	//IRow::GetColumns
	TESTC_(hr = m_pCRowObject->GetColumns(cColAccess, rgColAccess),S_OK);

	//Compare Data for this row object
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

	//Make sure on output, that our pData's are unchanged!
	for(i=0; i<cColAccess; i++)
	{
		if(i%2)
		{
			TESTC(rgColAccess[i].pData == NULL);
		}
		else
		{
			TESTC(rgColAccess[i].pData != NULL);
		}
	}

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_12()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END








// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Vectors - Some valid, some non-existent columns - DB_S_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_13()
{ 
	TBEGIN
	DBORDINAL cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	DBORDINAL cColAccess2 = 0;
	DBCOLUMNACCESS* rgColAccess2 = NULL;
	void* pData = NULL;
	void* pData2 = NULL;
	DBORDINAL cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;

	//Create the ColAccess Structures (for all columns)...
	//NOTE: We do this twice, since we want to test invalid columnid within valid ids.
	//A memcpy is not sufficient, since ColAccess contains ColIds which have to be allocated,
	//as well as memory location buffers, etc.  Easier to just call twice...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess2, &rgColAccess2, &pData2, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now create complete ColAccess array, containing:
	//Format = 1 Invalid, All Valid, 2 Invalid, All Valid, 1 Invalid
	cColInvalid = 1 + cColAccess + 2 + cColAccess2 + 1;
	SAFE_ALLOC(rgColInvalid, DBCOLUMNACCESS, cColInvalid);
	//Create "Invalid" ColIDs
	memset(rgColInvalid, 0, sizeof(DBCOLUMNACCESS) * cColInvalid);
	//Create First set of All Valid
	memcpy(&rgColInvalid[1], rgColAccess, sizeof(DBCOLUMNACCESS)*cColAccess);
	//Create Second set of All Valid
	memcpy(&rgColInvalid[1 + cColAccess + 2], rgColAccess2, sizeof(DBCOLUMNACCESS)*cColAccess2);

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid),DB_S_ERRORSOCCURRED);

	//Verify Status'
	TESTC(rgColInvalid[0].dwStatus == DBSTATUS_E_DOESNOTEXIST);
	TESTC(rgColInvalid[1 + cColAccess + 0].dwStatus == DBSTATUS_E_DOESNOTEXIST);
	TESTC(rgColInvalid[1 + cColAccess + 1].dwStatus == DBSTATUS_E_DOESNOTEXIST);
	TESTC(rgColInvalid[1 + cColAccess + 2 + cColAccess2].dwStatus == DBSTATUS_E_DOESNOTEXIST);

	//Compare First All Valid Columns
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, &rgColInvalid[1], FIRST_ROW, pTable()));
	//Compare Second All Valid Columns
	TESTC(m_pCRowObject->CompareColAccess(cColAccess2, &rgColInvalid[1 + cColAccess + 2], FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	FreeColAccess(cColAccess2, rgColAccess2);
	SAFE_FREE(pData);
	SAFE_FREE(pData2);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Vectors - All non-existent columns - DB_E_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_14()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now replace the Array with (unique/nonexistent) columns
	for(i=0; i<cColInvalid; i++)
	{
		//Create a unique ColID name
		TESTC_(CreateUniqueDBID(&rgColInvalid[i].columnid),S_OK);
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid),DB_E_ERRORSOCCURRED);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
		TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Vectors - No Vector Columns - S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_15()
{ 
	TBEGIN
	
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, NOVECTOR_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Vectors - No Vectors and Non-Existent Columns - DB_E_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_16()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, NOVECTOR_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now replace the Array with (unique/nonexistent) columns
	for(i=0; i<cColInvalid; i++)
	{
		//Create a unique ColID name
		TESTC_(CreateUniqueDBID(&rgColInvalid[i].columnid),S_OK);
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid),DB_E_ERRORSOCCURRED);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
		TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Vectors - Only Vector Columns - S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_17()
{ 
	TBEGIN
	
	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(NULL, VECTOR_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Vectors - Only Non-Existent Vector Columns - DB_E_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_18()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, VECTOR_COLS_BOUND),S_OK);
	
	//Now replace the Array with (unique/nonexistent) columns
	for(i=0; i<cColInvalid; i++)
	{
		//Create a unique ColID name
		TESTC_(CreateUniqueDBID(&rgColInvalid[i].columnid),S_OK);
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid), cColInvalid ? DB_E_ERRORSOCCURRED : S_OK);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
		TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Vectors - Valid Vectors and Non-Existent Columns - DB_S_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_19()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;
	ULONG cNonVectors = 0;
	HRESULT hrExpected = S_OK;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now for all non-vectors, create a new invalid colid
	for(i=0; i<cColInvalid; i++)
	{
		//Create a unique ColID name (for all non-vectors)
		if(!(rgColInvalid[i].wType & DBTYPE_VECTOR))
		{
			TESTC_(CreateUniqueDBID(&rgColInvalid[i].columnid),S_OK);
			cNonVectors++;
		}
	}

	if(cNonVectors)
		hrExpected = (cColInvalid == cNonVectors) ? DB_E_ERRORSOCCURRED : DB_S_ERRORSOCCURRED;

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid), hrExpected);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
	{
		if(rgColInvalid[i].wType & DBTYPE_VECTOR)
		{
			TESTC(rgColInvalid[i].dwStatus == DBSTATUS_S_OK || rgColInvalid[i].dwStatus == DBSTATUS_S_ISNULL);
			TESTC(m_pCRowObject->CompareColAccess(1, &rgColInvalid[i], FIRST_ROW, pTable()));
		}
		else
		{
			//Only non-vectors where changed to non-existent columnids
			TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);
		}
	}

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc Vectors - Valid Non-Vectors and Non-Existent Vector Columns - DB_S_ERRORSOCCURRED
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_20()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;
	ULONG cVectors = 0;
	HRESULT hrExpected = S_OK;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now for all non-vectors, create a new invalid colid
	for(i=0; i<cColInvalid; i++)
	{
		//Create a unique ColID name (for all non-vectors)
		if(rgColInvalid[i].wType & DBTYPE_VECTOR)
		{
			TESTC_(CreateUniqueDBID(&rgColInvalid[i].columnid),S_OK);
			cVectors++;
		}
	}

	if(cVectors)
		hrExpected = (cColInvalid == cVectors) ? DB_E_ERRORSOCCURRED : DB_S_ERRORSOCCURRED;

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid), hrExpected);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
	{
		if(rgColInvalid[i].wType & DBTYPE_VECTOR)
		{
			//Only vectors where changed to non-existent columnids
			TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);
		}
		else
		{
			TESTC(rgColInvalid[i].dwStatus == DBSTATUS_S_OK || rgColInvalid[i].dwStatus == DBSTATUS_S_ISNULL);
			TESTC(m_pCRowObject->CompareColAccess(1, &rgColInvalid[i], FIRST_ROW, pTable()));
		}
	}

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_21()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Boundary - 0 columns -[0, NULL] and [0, valid]  no-op
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_22()
{ 
	TBEGIN
		
	//GetColumns - with (0, NULL)
	TESTC(VerifyGetColumnsAllRows(NULL, USE_COLS_TO_BIND_ARRAY, BLOB_LONG, FORWARD,
				NO_COLS_BY_REF,	DBTYPE_EMPTY, 0, NULL));

	//GetColumns - with (0, valid)
	TESTC_(m_pCRowObject->GetColumns(0, INVALID(DBCOLUMNACCESS*)), S_OK);

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Boundary - [Valid, NULL] - E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_23()
{ 
	TBEGIN
		
	//GetColumns - with (Valid, NULL)- E_INVALIDARG
	TESTC_(m_pCRowObject->GetColumns(5, NULL), E_INVALIDARG);

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END








// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_24()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Boundary - columnid - all valid dbkinds
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_25()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;
	ULONG cNonVectors = 0;
	HRESULT hrExpected = S_OK;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Loop through all columns
	for(i=0; i<cColInvalid; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColInvalid[i];
			
		//Valid DBKINDs are DBKIND_GUID_NAME(0) through DBKIND_GUID(6)
		//So create values from 0 - 6 (inclusive- ie: plus 1)...
		ReleaseDBID(&pColAccess->columnid, FALSE/*fDrop*/);
		pColAccess->columnid.eKind = i % (DBKIND_GUID+1);

		//Now Create a unique colid for this type...
		TESTC_(CreateUniqueDBID(&pColAccess->columnid),S_OK);
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid), cColInvalid ? DB_E_ERRORSOCCURRED : S_OK);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
	{
		//All columns were fabricated guids, (so we could test remoting with all column types)
		TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);
	}

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc Boundary - columnid - invalid dbkinds
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_26()
{ 
	TBEGIN
	DBORDINAL i,cColInvalid = 0;
	DBCOLUMNACCESS* rgColInvalid = NULL;
	void* pData = NULL;
	ULONG cNonVectors = 0;
	HRESULT hrExpected = S_OK;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColInvalid, &rgColInvalid, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Loop through all columns
	for(i=0; i<cColInvalid; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColInvalid[i];
			
		//Valid DBKINDs are DBKIND_GUID_NAME(0) through DBKIND_GUID(6)
		//So create values from 7...
		ReleaseDBID(&pColAccess->columnid, FALSE/*fDrop*/);
		pColAccess->columnid.eKind = DBKIND_GUID+1;
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColInvalid, rgColInvalid), cColInvalid ? DB_E_ERRORSOCCURRED : S_OK);

	//Verify Status'
	for(i=0; i<cColInvalid; i++)
	{
		//All columns were fabricated guids, (so we could test remoting with all column types)
		TESTC(rgColInvalid[i].dwStatus == DBSTATUS_E_DOESNOTEXIST);
	}

CLEANUP:
	FreeColAccess(cColInvalid, rgColInvalid);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_27()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc Boundary - cbMaxLen - ignored for fixed length types
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_28()
{ 
	TBEGIN
	DBORDINAL i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Now make sure the provider ignores cbMaxLen for fixed length types
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];
		
		//The provider is supposed to ignore cbMaxLen for Fixed Length data types,
		//as well as the modifiers (DBTYPE_BYREF and DBTYPE_ARRAY and DBTYPE_VECTOR)
		if(IsFixedLength(pColAccess->wType) || 
			(pColAccess->wType & (DBTYPE_BYREF|DBTYPE_ARRAY|DBTYPE_VECTOR)))
		{
			//NOTE: We want to vary this value, since -1 (ULONG_MAX) might cause problems if there
			//using this value for allocations, and 0 might cause problems is using for remoting
			//for the next location in the buffer, etc.  We also want to vary the value since the
			//proivder / remoting might have special checks against fixed values for protection...
			pColAccess->cbMaxLen = i ? (-1/*ULONG_MAX*//i) : 0; //Protect against divide by 0
		}
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColAccess, rgColAccess),S_OK);
	//Compare Data for this row object
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc Boundary - cbMaxLen - DBSTATUS_S_TRUNCATION
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_29()
{ 
	TBEGIN
	DBORDINAL i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;
	ULONG cTruncated = 0;
	HRESULT hrExpected = S_OK;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Loop through all columns
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];
		
		//Truncate all variable string length types...
		if(IsFixedLength(pColAccess->wType))
		{
			//NOTE: cbMaxLength is ignored for fixed length types...
			pColAccess->cbMaxLen = 0;	
		}
		else
		{
			//NOTE: We want to vary this value, since it may have different effects on different
			//types and setting it to 0 may go through a different code path than ~0, etc...
			pColAccess->cbMaxLen = cTruncated;
			if(pColAccess->wType == DBTYPE_STR || pColAccess->wType == DBTYPE_WSTR)
			{
				//We only increment if a string, since we want the first string data
				//to have a 0 byte cbMaxLen (not enough for a NULL terminator)
				cTruncated++;
				
				//NOTE: Truncation is considered a "warning" and not an error.  So normally you
				//would expect S_OK to be returned, but since we set the first Variable Length
				//column to cbMaxLen=0, this should be DataOverFlow, since their is not even enough
				//room for the NULL terminator, so if the provider actually returns "Truncation" the 
				//consumer would crash since their is not null terminator, and would be whatever was left
				//in the buffer...
				hrExpected = (cTruncated==cColAccess) ? DB_E_ERRORSOCCURRED : DB_S_ERRORSOCCURRED;
			}
		}
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColAccess, rgColAccess), hrExpected);

	//Verify the returned status, and data...
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];

		//Verify the Status
		switch(pColAccess->dwStatus)
		{
			case DBSTATUS_S_OK:
				//NOTE: cbMaxLength is ignored for fixed length types...
				if(!IsFixedLength(pColAccess->wType))
					TESTC(pColAccess->cbDataLen <= pColAccess->cbMaxLen);
				break;

			case DBSTATUS_S_ISNULL:
				//NULL data is an exception since it doesn't matter what cbMaxLen was...
				break;

			case DBSTATUS_S_TRUNCATED:
				//NOTE: cbMaxLength is ignored for fixed length types...
				TESTC(!IsFixedLength(pColAccess->wType));
				TESTC(pColAccess->cbDataLen >= pColAccess->cbMaxLen);
				break;

			case DBSTATUS_E_DATAOVERFLOW:
				//NOTE: Truncation is considered a "warning" and not an error.  So normally you
				//would expect S_OK to be returned, but since we set the first Variable Length
				//column to cbMaxLen=0, this should be DataOverFlow, since their is not even enough
				//room for the NULL terminator, so if the provider actually returns "Truncation" the 
				//consumer would crash since their is not null terminator, and would be whatever was left
				//in the buffer...
				TESTC(!IsFixedLength(pColAccess->wType));
				TESTC(pColAccess->cbMaxLen == 0);

				//Can't compare data in this case...
				continue;
				break;
					
			default:
				TESTC(FALSE);	//Not an appropiate status...
		};

		//Compare Data for this column
		TESTC(m_pCRowObject->CompareColAccess(1, pColAccess, FIRST_ROW, pTable()));
	}

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc Boundary - cbMaxLen - invalid
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_30()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_31()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc Boundary - wType - all valid types and modifieres
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_32()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc Boundary - wType - invalid types and invalid type modifiers
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_33()
{ 
	TBEGIN
	DBORDINAL i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;
	ULONG cInvalid = 0;
	HRESULT hr = S_OK;

	//Create the ColAccess Structures (for all columns)...
	//NOTE: We will bind all columns as DBTYPE_VECTOR, 
	//we have to do this ahead of time so that all the offsets and buffer sizes will be large
	//enough, we can't juist spin through the types later and change them since the buffer
	//won't have enough room, and will be pointing to the wrong spots...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, 
		ALL_COLS_BOUND, BLOB_LONG, FORWARD, NO_COLS_BY_REF, DBTYPE_VECTOR),S_OK);
	
	//Loop through all (original) column access structures
	//Since the above already put on DBTYPE_VECTOR we need to know the type before doing this...
	for(i=0; i<m_pCRowObject->m_cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &m_pCRowObject->m_rgColAccess[i];

		//Although we are trying for an error senario, (not all types converted to DBTYPE_VECTOR)
		//The provider may actually support this conversion.  We can't just specifiy 
		//an invalid type, since that will more than likely be caught way up front, and not actually
		//hit the conversion code we are looking for (for example: in remoting an invalid type
		//is not even sent the to the provider, whereas an unsupported conversion goes all
		//the way across and will fail on return, this is what we want).

		//See if this is a problem converting for the provider
		TEST2C_(hr = m_pCRowObject->pIConvertType()->CanConvert(pColAccess->wType, pColAccess->wType | DBTYPE_VECTOR, DBCONVERTFLAGS_COLUMN),S_OK,S_FALSE);
		if(hr != S_OK)
			cInvalid++;
	}

	//IRow::GetColumns
	TEST3C_(hr = m_pCRowObject->GetColumns(cColAccess, rgColAccess), S_OK, DB_S_ERRORSOCCURRED, DB_E_ERRORSOCCURRED);
	
	//Possibilities:
	//1.  Everything can be converted fine (cInvalid==0)	- S_OK
	//2.  Some column could be converted but not all		- DB_S_ERROROCCURRED
	//3.  All columns coulnd not be converted				- DB_E_ERRORSOCCURRED
	//The exception is that some column even though indicated could not be converted, could still
	//be convergted if the status was ISNULL - since now conversion is required for this senario...
	//Thats why we allow S_OK in all the following senarios...
	if(cInvalid == 0)
		TESTC_(hr, S_OK);
	if(cInvalid < cColAccess)
		TEST2C_(hr, S_OK, DB_S_ERRORSOCCURRED);
	if(cInvalid && cInvalid == cColAccess)
		TEST3C_(hr, S_OK, DB_S_ERRORSOCCURRED, DB_E_ERRORSOCCURRED);

	//Loop through all columns
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];

		//Verify the result
		if(pColAccess->dwStatus == DBSTATUS_S_OK || pColAccess->dwStatus == DBSTATUS_S_ISNULL)
		{
			//Verify the data returned (successfully converted this)
			TESTC(m_pCRowObject->CompareColAccess(1, pColAccess, FIRST_ROW, pTable()));
		}
		else
		{
			TESTC(pColAccess->dwStatus == DBSTATUS_E_CANTCONVERTVALUE);
		}
	}

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(34)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_34()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(35)
//*-----------------------------------------------------------------------
// @mfunc Boundary - bPrecision - make sure ignored on input for all types, except NUMERIC.
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_35()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(36)
//*-----------------------------------------------------------------------
// @mfunc Boundary - bScale - make sure ignored on input for all types, except NUMERIC.
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_36()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(37)
//*-----------------------------------------------------------------------
// @mfunc Boundary - Output Only - make sure cbDataLen, dwStatus, dwReserverd are ignored on input
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_37()
{ 
	TBEGIN
	DBORDINAL i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Loop through all columns
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];
	
		//Set input params that are supposed to be ignored to a "known" value
		pColAccess->dwStatus	= DBSTATUS_S_OK + i + 1; //Everything beyond S_OK is interesting
		pColAccess->cbDataLen	= (-1) - i;
		pColAccess->dwReserved	= i;
	}

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColAccess, rgColAccess), S_OK);

	//Compare Data for this column
	TESTC(m_pCRowObject->CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(38)
//*-----------------------------------------------------------------------
// @mfunc Boundary - Input - Make sure all pointers and input args are not changed on output, pData pointer, columnid [including all union pointers], cbMaxLen, dwReserved, wType.
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_38()
{ 
	TBEGIN
	DBORDINAL i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	DBCOLUMNACCESS* rgColAccessCopy = NULL;
	void* pData = NULL;

	//Create the ColAccess Structures (for all columns)...
	TESTC_(m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG),S_OK);
	
	//Make more interesting data for some of the items
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess = &rgColAccess[i];
	
		//Every other column (so some succeed)
		if(i%2 == 0)
		{
			//columnid
			ReleaseDBID(&pColAccess->columnid, FALSE/*fDrop*/);
			pColAccess->columnid.eKind = i % (DBKIND_GUID+1);
			TESTC_(CreateUniqueDBID(&pColAccess->columnid),S_OK);
		}

		//The consumer might have used this reserver flag for internal storage,
		//so the provider should not be looking at it or setting it...
		pColAccess->dwReserved = i;
	}

	//Make a "snapshot" copy to compare against
	SAFE_ALLOC(rgColAccessCopy, DBCOLUMNACCESS, cColAccess);
	memcpy(rgColAccessCopy, rgColAccess, cColAccess*sizeof(DBCOLUMNACCESS));

	//IRow::GetColumns
	TESTC_(m_pCRowObject->GetColumns(cColAccess, rgColAccess), cColAccess ? DB_S_ERRORSOCCURRED : S_OK);

	//Make more interesting data for some of the items
	for(i=0; i<cColAccess; i++)
	{
		DBCOLUMNACCESS* pColAccess		= &rgColAccess[i];
		DBCOLUMNACCESS* pColAccessCopy	= &rgColAccessCopy[i];

		//Verify non-output items have not changed from inputs
		TESTC(memcmp(&pColAccess->columnid, &pColAccessCopy->columnid, sizeof(DBID))==0);
		TESTC(pColAccess->pData			== pColAccessCopy->pData);
		TESTC(pColAccess->cbMaxLen		== pColAccessCopy->cbMaxLen);
		TESTC(pColAccess->dwReserved	== pColAccessCopy->dwReserved);
		TESTC(pColAccess->wType			== pColAccessCopy->wType);
		TESTC(pColAccess->bPrecision	== pColAccessCopy->bPrecision);
		TESTC(pColAccess->bScale		== pColAccessCopy->bScale);

		//Compare Data for this column
		if(i%2 == 0)
		{
			TESTC(pColAccess->dwStatus == DBSTATUS_E_DOESNOTEXIST);
		}
		else
		{
			TESTC(m_pCRowObject->CompareColAccess(1, pColAccess, FIRST_ROW, pTable()));
		}
	}

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(rgColAccessCopy);
	SAFE_FREE(pData);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(39)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_39()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(40)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_40()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(41)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_41()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(42)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_42()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(43)
//*-----------------------------------------------------------------------
// @mfunc Buffered Mode - All Columns - no BLOBs
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_43()
{ 
	TBEGIN

	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(&RowsetA, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(44)
//*-----------------------------------------------------------------------
// @mfunc Buffered Mode - All Columns - BLOBs
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_44()
{ 
	TBEGIN
	
	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Loop through all the rows in the rowset, verify the columns...
	TESTC(VerifyGetColumnsAllRows(&RowsetA, ALL_COLS_BOUND, BLOB_LONG));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(45)
//*-----------------------------------------------------------------------
// @mfunc Buffered Mode - All Columns - Just extra columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_45()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	ULONG cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;

	ULONG cColumns = 0;
	ULONG* rgColOrdinals = NULL;
	HROW hRow = NULL;

	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;
	CRowObject RowObjectA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Obtain the First row...
	TESTC_(RowsetA.GetNextRows(&hRow),S_OK);

	//Now create the row object.
	TEST2C_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

	//Obtain the just the Extra columns
	TESTC_(hr = RowObjectA.GetExtraColumnInfo(&cColumns, NULL, NULL, &rgColOrdinals),S_OK);

	//Create the ColAccess Structures for just the extra columns...
	TESTC_(hr = RowObjectA.CreateColAccess(&cColAccess, &rgColAccess, &pData, NULL, ALL_COLS_BOUND, BLOB_LONG, FORWARD, NO_COLS_BY_REF, DBTYPE_EMPTY, cColumns, rgColOrdinals),S_OK);
							 
	//IRow::GetColumns
	TESTC_(hr = RowObjectA.GetColumns(cColAccess, rgColAccess),S_OK);

	//Compare Data for this row object
	TESTC(RowObjectA.CompareColAccess(cColAccess, rgColAccess, FIRST_ROW, pTable()));

CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	SAFE_FREE(rgColOrdinals);
	RowsetA.ReleaseRows(hRow);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(46)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_46()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(47)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Command - > Row
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_47()
{ 
	TBEGIN
    CAggregate Aggregate;
	IUnknown* pIUnkInner = NULL;
	DWORD dwValue = 0;
	HRESULT hr = S_OK;

	CCommand CommandA;
	CommandA.SetProperty(DBPROP_IRow);
	
	//Create a command object
	TESTC_PROVIDER(CommandA.CreateCommand()==S_OK);

	//Try the invalid senario first... (asking for non-IUnknown)
	TESTC_(hr = CommandA.SetCommandText(SELECT_ALLFROMTBL),S_OK);
	TESTC_(hr = CommandA.Execute(&Aggregate, IID_IRow, &pIUnkInner), DB_E_NOAGGREGATION);

	//Create a row object from IOpenRowset asking for IID_IRow.
	//But we are setting the DBPROP_IRow property, so the results is a row object not a rowset...
	TEST2C_(hr = CommandA.SetProperties(), S_OK, DB_E_ERRORSOCCURRED);
	if(SUCCEEDED(hr))
	{
		TEST4C_(hr = CommandA.Execute(&Aggregate, IID_IUnknown, &pIUnkInner), S_OK, DB_S_NOTSINGLETON, DB_E_NOAGGREGATION, DB_E_ERRORSOCCURRED);
		Aggregate.SetUnkInner(pIUnkInner);
	}

	if(hr == DB_E_ERRORSOCCURRED)
	{
		//The only reason DB_E_ERRORSOCCURRED should be returned would be if the provider doesn't
		//support singleton row objects to be returned from OpenRowset or Execute.
				
		//NOTE: Sigleton objects are very useful, but none-the-less not required.
		//Verify the result with the advertised property value...
		TESTC(::GetProperty(DBPROP_OLEOBJECTS, DBPROPSET_DATASOURCEINFO, g_pIDBCreateSession, &dwValue));
		TESTC(!(dwValue & DBPROPVAL_OO_SINGLETON));
	}
	else
	{
		if(hr == DB_S_NOTSINGLETON)
			hr = S_OK;

		//Verify Aggregation for this row...
		TESTC_PROVIDER(Aggregate.VerifyAggregationQI(hr, IID_IRow));
	}

CLEANUP:
	SAFE_RELEASE(pIUnkInner);
	Aggregate.ReleaseInner();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(48)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Session - > Row
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_48()
{ 
	TBEGIN
    CAggregate Aggregate;
	IUnknown* pIUnkInner = NULL;
	DWORD dwValue = 0;
	HRESULT hr = S_OK;

	COpenRowset OpenRowsetA;
	OpenRowsetA.SetProperty(DBPROP_IRow);

	//Try the invalid senario first... (asking for non-IUnknown)
	TESTC_(hr = OpenRowsetA.CreateOpenRowset(IID_IRow, &pIUnkInner, &Aggregate, ROW_INTERFACE), DB_E_NOAGGREGATION);

	//Create a row object from IOpenRowset asking for IID_IRow.
	//But we are setting the DBPROP_IRow property, so the results is a row object not a rowset...
	OpenRowsetA.SetProperty(DBPROP_IRow);
	TEST4C_(hr = OpenRowsetA.CreateOpenRowset(IID_IUnknown, &pIUnkInner, &Aggregate, ROW_INTERFACE), S_OK, DB_S_NOTSINGLETON, DB_E_NOAGGREGATION, DB_E_ERRORSOCCURRED);
	Aggregate.SetUnkInner(pIUnkInner);

	if(hr == DB_E_ERRORSOCCURRED)
	{
		//The only reason DB_E_ERRORSOCCURRED should be returned would be if the provider doesn't
		//support singleton row objects to be returned from OpenRowset or Execute.
				
		//NOTE: Sigleton objects are very useful, but none-the-less not required.
		//Verify the result with the advertised property value...
		TESTC(::GetProperty(DBPROP_OLEOBJECTS, DBPROPSET_DATASOURCEINFO, g_pIDBCreateSession, &dwValue));
		TESTC(!(dwValue & DBPROPVAL_OO_SINGLETON));
	}
	else
	{
		if(hr == DB_S_NOTSINGLETON)
			hr = S_OK;

		//Verify Aggregation for this row...
		TESTC_PROVIDER(Aggregate.VerifyAggregationQI(hr, IID_IRow));
	}

CLEANUP:
	SAFE_RELEASE(pIUnkInner);
	Aggregate.ReleaseInner();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(49)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Rowset -> Row
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_49()
{ 
	TBEGIN
	HROW hRow = NULL;
    CAggregate Aggregate;
	IUnknown* pIUnkInner = NULL;
	CRowset RowsetA;
	HRESULT hr = S_OK;

	//Create a rowset
	TESTC_(RowsetA.CreateRowset(),S_OK)

	//Obtain the second row object
	TESTC_(RowsetA.GetRow(SECOND_ROW, &hRow),S_OK);
	
	//Aggregation
	hr = RowsetA.GetRowFromHROW(&Aggregate, hRow, IID_IUnknown, &pIUnkInner);
	Aggregate.SetUnkInner(pIUnkInner);
	
	//Verify Aggregation...
	TESTC_PROVIDER(Aggregate.VerifyAggregationQI(hr, IID_IRow));

CLEANUP:
	RowsetA.ReleaseRows(hRow);
	SAFE_RELEASE(pIUnkInner);
	Aggregate.ReleaseInner();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(50)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_50()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(51)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_51()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(52)
//*-----------------------------------------------------------------------
// @mfunc Properties - DBPROP_IRow
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_52()
{ 
	TBEGIN
	IUnknown* pIUnknown = NULL;
	HRESULT hr = S_OK;

	//Make sure the required Row Object properties (including DBPROP_IRow) are supported
	for(ULONG i=0; i<cInterfaceIIDs; i++)
	{
		if(rgInterfaceIIDs[i].dwPropertyID)
		{
			//Make sure this property is supported.
			if(SupportedProperty(rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET))
			{
				//See is this interface is available
				TEST2C_(hr = m_pCRowObject->pIRow()->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown), S_OK, E_NOINTERFACE);
				if(SUCCEEDED(hr))
				{
					TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));
					SAFE_RELEASE(pIUnknown);
				}
				else
				{
					//Make sure this is not one of the mandatory interfaces
					//All of the mandotry interfaces must be supported
					TESTC(!rgInterfaceIIDs[i].fMandatory);
				}
			}
			else
			{
				//Make sure this is not one of the mandatory properties.
				//All of the mandotry properties must be supported
				TESTC(!rgInterfaceIIDs[i].fMandatory);

				//Make sure this interface is not supported, since the property is not
				if(E_NOINTERFACE != m_pCRowObject->pIRow()->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown))
					TERROR(L"Interface available although property failed for " << GetInterfaceName(*rgInterfaceIIDs[i].pIID));
			}
		}
	}

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END








// {{ TCW_VAR_PROTOTYPE(53)
//*-----------------------------------------------------------------------
// @mfunc Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_ROWOBJECT
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_53()
{ 
	TBEGIN
	DWORD dwValue = 0;

	//If we have made it pass ModuleInit, then the provider supports Row Objects
	//So make sure they also advertise they support row objects through the property.
	//Required for early bound applications, vs. runtime applications
	TESTC(::GetProperty(DBPROP_OLEOBJECTS, DBPROPSET_DATASOURCEINFO, g_pIDBCreateSession, &dwValue));
	TESTC(BITSET(dwValue, DBPROPVAL_OO_ROWOBJECT));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(54)
//*-----------------------------------------------------------------------
// @mfunc Properties - DBPROP_OLEOBJECTS - DBPROPVAL_OO_SINGLETON
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_54()
{ 
	TBEGIN
	CRowset RowsetA;
	IUnknown* pIUnknown = NULL;
	DWORD dwValue = 0;

	ULONG cPropSets = 0;
	DBPROPSET* rgPropSets = NULL;
	::SetProperty(DBPROP_IRow, DBPROPSET_ROWSET, &cPropSets, &rgPropSets);

	//NOTE: Sigleton objects are very useful, but none-the-less not required.
	//Verify the result with the advertised property value...
	if(::GetProperty(DBPROP_OLEOBJECTS, DBPROPSET_DATASOURCEINFO, g_pIDBCreateSession, &dwValue) &&
		dwValue & DBPROPVAL_OO_SINGLETON)
	{
		//Create a row object from ICommand::Execute asking for IID_IRow
		TEST2C_(RowsetA.pTable()->CreateRowset(SELECT_ALLFROMTBL, IID_IRow, 0, NULL, &pIUnknown), S_OK, DB_S_NOTSINGLETON);
		TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));
		SAFE_RELEASE(pIUnknown);

		//Create a row object from IOpenRowset asking for IID_IRown.
		//But we are setting the DBPROP_IRow property, so the results is a row object not a rowset...
		TEST2C_(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IRow, 0, NULL, &pIUnknown), S_OK, DB_S_NOTSINGLETON);
		TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));
		SAFE_RELEASE(pIUnknown);

		//Create a row object from ICommand::Execute asking for IID_IUnknown.
		//But we are setting the DBPROP_IRow property, so the results is a row object not a rowset...
		TEST2C_(RowsetA.pTable()->CreateRowset(SELECT_ALLFROMTBL, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown),S_OK,DB_S_NOTSINGLETON);
		TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));
		SAFE_RELEASE(pIUnknown);

		//Create a row object from IOpenRowset asking for IID_IRown.
		//But we are setting the DBPROP_IRow property, so the results is a row object not a rowset...
		TEST2C_(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown), S_OK, DB_S_NOTSINGLETON);
		TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));
		SAFE_RELEASE(pIUnknown);
	}
	else
	{
		TESTC_(RowsetA.pTable()->CreateRowset(SELECT_ALLFROMTBL, IID_IRow, 0, NULL, &pIUnknown),E_NOINTERFACE);
		TESTC_(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IRow, 0, NULL, &pIUnknown),E_NOINTERFACE);
		TESTC_(RowsetA.pTable()->CreateRowset(SELECT_ALLFROMTBL, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown),E_NOINTERFACE);
		TESTC_(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown),E_NOINTERFACE);
	}

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	::FreeProperties(&cPropSets, &rgPropSets);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(55)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_55()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(56)
//*-----------------------------------------------------------------------
// @mfunc Properties - All Row Object Properties
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_56()
{ 
	TBEGIN
	CRowset RowsetA;
	IUnknown* pIUnknown = NULL;
	IUnknown* pIUnknown2 = NULL;

	ULONG cPropSets = 0;
	DBPROPSET* rgPropSets = NULL;
	HRESULT hr = S_OK;

	//Set all Row Object Properties (REQUIRED)
	for(ULONG i=0; i<cInterfaceIIDs; i++)
		if(rgInterfaceIIDs[i].dwPropertyID)
			::SetProperty(rgInterfaceIIDs[i].dwPropertyID, 	DBPROPSET_ROWSET, &cPropSets, &rgPropSets);
	
	//OpenRowset
	TEST3C_(hr = RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown),S_OK,DB_S_NOTSINGLETON,DB_E_ERRORSOCCURRED);

	//Verify a Row object is returned...
	if(pIUnknown)
		TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));

	//Verify returned Properties...
	for(i=0; i<cInterfaceIIDs; i++)
	{		
		if(rgInterfaceIIDs[i].dwPropertyID)
		{
			TCOMPARE_(VerifyPropStatus(cPropSets, rgPropSets, rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET, DBPROPSTATUS_OK));
			
			DBPROP* pProp = NULL;
			TESTC(FindProperty(rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET, cPropSets, rgPropSets, &pProp)); 
			
			//If the property succeeded, then the interface must be available.
			//(ie: provider can always return extra functionalty, but must at least the mininal asked for)
			if(pProp->dwStatus == DBPROPSTATUS_OK)
			{
				if(SUCCEEDED(hr))
				{
					//Then all the properties must have succeeded	
					TESTC(VerifyPropSetStatus(cPropSets, rgPropSets, DBPROPSTATUS_OK));

					//Make sure the interface is available
					if(S_OK != pIUnknown->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown2))
						TERROR(L"Interface not available although property succeeded for " << GetInterfaceName(*rgInterfaceIIDs[i].pIID));
					TESTC(DefaultObjectTesting(pIUnknown2, ROW_INTERFACE));
					SAFE_RELEASE(pIUnknown2);
				}
			}
			else
			{
				//Some providers still don't met the spec and fail DBPROP_I*,
				//since the property DBPROP_I* was set and the interface IID_I* was not asked
				//for.  This is wrong, and against the spec, which states that asking for 
				//DBPROP_I* is just like implicilty asking for IID_I*.  Test it here, 
				//to make sure the provider truely doesn't support this interface, 
				//so we don't skip them incorrectly

				//The mandatory properties cannot fail...
				TESTC(!rgInterfaceIIDs[i].fMandatory);

				if(SUCCEEDED(hr))
				{
					//Make sure the interface is not available
					TESTC_(pIUnknown->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown2), E_NOINTERFACE);
				}

				//Make sure asking for the IID also fails...
				if(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, *rgInterfaceIIDs[i].pIID, 0, NULL, &pIUnknown)!=E_NOINTERFACE)
					TERROR(L"Property Incorrect for " << GetPropertyName(rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET) << "\n");
			}
		}
	}

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	SAFE_RELEASE(pIUnknown2);
	::FreeProperties(&cPropSets, &rgPropSets);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(57)
//*-----------------------------------------------------------------------
// @mfunc Properties - All Row Object Properties Optional
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_57()
{ 
	TBEGIN
	CRowset RowsetA;
	IUnknown* pIUnknown = NULL;
	IUnknown* pIUnknown2 = NULL;
	HRESULT hr = S_OK;

	ULONG cPropSets = 0;
	DBPROPSET* rgPropSets = NULL;

	//Set all Row Object Properties (Mandatory-REQUIRED, Optional-OPTIONAL)
	for(ULONG i=0; i<cInterfaceIIDs; i++)
	{
		if(rgInterfaceIIDs[i].dwPropertyID)
			::SetProperty(rgInterfaceIIDs[i].dwPropertyID, 	DBPROPSET_ROWSET, &cPropSets, &rgPropSets, (void*)VARIANT_TRUE, DBTYPE_BOOL, rgInterfaceIIDs[i].fMandatory ? DBPROPOPTIONS_REQUIRED : DBPROPOPTIONS_OPTIONAL);
	}
	
	//OpenRowset
	TEST3C_(hr = RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IUnknown, cPropSets, rgPropSets, &pIUnknown),S_OK,DB_S_NOTSINGLETON,DB_S_ERRORSOCCURRED);

	//Verify a Row object is returned...
	TESTC(DefaultObjectTesting(pIUnknown, ROW_INTERFACE));

	//Verify returned Properties...
	for(i=0; i<cInterfaceIIDs; i++)
	{
		if(rgInterfaceIIDs[i].dwPropertyID)
			TCOMPARE_(VerifyPropStatus(cPropSets, rgPropSets, rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET, DBPROPSTATUS_OK));
	}

	//NOTE: The spec indicates that DB_S_ERRORSOCCURRED, superceeds DB_S_NOTSINGLETON.
	//So if there are any property errors then DB_S_ERRORSOCCURRED is returned.  DB_S_NOTSINGLETON
	//will only be returned in the case that no property errors...
	if(hr == DB_S_NOTSINGLETON)
	{
		//Then all the properties must have succeeded	
		TESTC(VerifyPropSetStatus(cPropSets, rgPropSets, DBPROPSTATUS_OK));
	}

	//For all properties that did succeed, the interface should be available...
	for(i=0; i<cInterfaceIIDs; i++)
	{
		if(rgInterfaceIIDs[i].dwPropertyID)
		{
			DBPROP* pProp = NULL;
			TESTC(FindProperty(rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET, cPropSets, rgPropSets, &pProp)); 
			
			//If the property succeeded, then the interface must be available.
			//(ie: provider can always return extra functionalty, but must at least the mininal asked for)
			if(pProp->dwStatus == DBPROPSTATUS_OK)
			{
				if(SUCCEEDED(hr))
				{
					if(S_OK != pIUnknown->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown2))
						TERROR(L"Interface not available although property succeeded for " << GetInterfaceName(*rgInterfaceIIDs[i].pIID));
					
					TESTC(DefaultObjectTesting(pIUnknown2, ROW_INTERFACE));
					SAFE_RELEASE(pIUnknown2);
				}
			}
			else
			{
				//Some providers still don't met the spec and fail DBPROP_I*,
				//since the property DBPROP_I* was set and the interface IID_I* was not asked
				//for.  This is wrong, and against the spec, which states that asking for 
				//DBPROP_I* is just like implicilty asking for IID_I*.  Test it here, 
				//to make sure the provider truely doesn't support this interface, 
				//so we don't skip them incorrectly

				//The mandatory properties cannot fail...
				TESTC(!rgInterfaceIIDs[i].fMandatory);

				if(SUCCEEDED(hr))
				{
					//Make sure the interface is not available
					TESTC_(pIUnknown->QueryInterface(*rgInterfaceIIDs[i].pIID, (void**)&pIUnknown2), E_NOINTERFACE);
				}

				//Make sure asking for the IID also fails...
				if(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, *rgInterfaceIIDs[i].pIID, 0, NULL, &pIUnknown)!=E_NOINTERFACE)
					TERROR(L"Property Incorrect for " << GetPropertyName(rgInterfaceIIDs[i].dwPropertyID, DBPROPSET_ROWSET) << "\n");
			}
		}
	}
	

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	SAFE_RELEASE(pIUnknown2);
	::FreeProperties(&cPropSets, &rgPropSets);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(58)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_58()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(59)
//*-----------------------------------------------------------------------
// @mfunc Threads - GetColumns seperate threads
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_59()
{ 
	TBEGIN
	INIT_THREADS(MAX_THREADS);	
	
	//Setup Thread Arguments
	THREADARG T1Arg = { this };

	//Create Threads
	CREATE_THREADS(Thread_VerifyGetColumns, &T1Arg);

	START_THREADS();
	END_THREADS();	

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(60)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_60()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(61)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_DELETEDROW
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_61()
{ 
	TBEGIN
	ULONG cRowsObtained = 0;
	HROW* rghRows = NULL;
	HROW hLastRow = NULL;
	
	CRowsetChange RowsetA;
	CRowObject RowObjectA;
	HRESULT hr = S_OK;

	//Create a new rowset
	//Provider must support IRowsetChange
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);
	
	//Obtain the last row handle
	//We could just do (-1, 1) or (0, -1) but that would require fetching backwards...
	TEST3C_(hr = RowsetA.GetNextRows(0, 1000, &cRowsObtained, &rghRows), S_OK, DB_S_ENDOFROWSET, DB_S_ROWLIMITEXCEEDED);
	TESTC(cRowsObtained && rghRows);
	hLastRow = rghRows[cRowsObtained - 1];

	//Now create the row object.
	TEST2C_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hLastRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

	//While the row object is open, delete the row out from under it...
	//NOTE: The other senario, deleting the row handle before creating 
	//the row object is done in the IGetRow test
	TESTC_(RowsetA.DeleteRow(hLastRow),S_OK);
	RowsetA.pTable()->SubtractRow();

	//Now try and use the row object
	TESTC_(RowObjectA.GetColumns(RowObjectA.m_cColAccess, RowObjectA.m_rgColAccess), DB_E_DELETEDROW);

CLEANUP:
	RowsetA.ReleaseRows(cRowsObtained, rghRows);
	PROVIDER_FREE(rghRows);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(62)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_DELETEDROW - Bufferred Mode
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetColumns::Variation_62()
{ 
	TBEGIN
	HROW hRow = NULL;
	CRowsetUpdate RowsetA;
	CRowObject RowObjectA;

	//Create a new rowset
	//Provider must support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);
	
	//Obtain the first row handle
	TESTC_(RowsetA.GetNextRows(&hRow),S_OK);

	//Now create the row object.
	TEST2C_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);

	//While the row object is open, delete the row out from under it...
	//NOTE: The other senario, deleting the row handle before creating 
	//the row object is done in the IGetRow test
	TESTC_(RowsetA.DeleteRow(hRow),S_OK);
	
	//Now try and use the row object
	TESTC_(RowObjectA.GetColumns(RowObjectA.m_cColAccess, RowObjectA.m_rgColAccess), DB_E_DELETEDROW);

	//Actually Update the backend
	TESTC_(RowsetA.UpdateRow(hRow),S_OK);
	RowsetA.pTable()->SubtractRow();
	TESTC_(RowObjectA.GetColumns(RowObjectA.m_cColAccess, RowObjectA.m_rgColAccess), DB_E_DELETEDROW);

CLEANUP:
	RowsetA.ReleaseRows(hRow);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCIRow_GetColumns::Terminate()
{ 
// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCIRow::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCIRow_GetSourceRowset)
//*-----------------------------------------------------------------------
//| Test Case:		TCIRow_GetSourceRowset - IRow::GetSourceRowset
//| Created:  	8/5/98
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCIRow_GetSourceRowset::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCIRow::Init())
	// }}
	{ 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc riid - All Mandatory interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_1()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	IUnknown* pIUnknown = NULL;

	//Obtain the Rowset interfaces...
	ULONG cRowsetIIDs = 0;
	INTERFACEMAP* rgRowsetIIDs = NULL;
	GetInterfaceArray(ROWSET_INTERFACE, &cRowsetIIDs, &rgRowsetIIDs);

	//For every [MANDATORY] interface, try to object the parent rowset object...
	for(ULONG i=0; i<cRowsetIIDs; i++)
	{
		//IRow::GetSourceRowset
		hr = m_pCRowObject->GetSourceRowset(*rgRowsetIIDs[i].pIID, (IUnknown**)&pIUnknown, NULL);
				
		//Determine results
		if(rgRowsetIIDs[i].fMandatory)
		{
			//[MANDATORY]
			if(hr!=S_OK && hr!=DB_E_NOSOURCEOBJECT)
			{
				CHECK(hr, S_OK);
				TOUTPUT_(L"ERROR: Interface Incorrect for " << GetInterfaceName(*rgRowsetIIDs[i].pIID) << "\n");
			}
		}
		else
		{
			//[OPTIONAL]
			if(hr!=S_OK && hr!=DB_E_NOSOURCEOBJECT && hr!=E_NOINTERFACE)
			{
				CHECK(hr, S_OK);
				TOUTPUT_(L"ERROR: Interface Incorrect for " << GetInterfaceName(*rgRowsetIIDs[i].pIID) << "\n");
			}
		}
		SAFE_RELEASE(pIUnknown);
	}

	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_2()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc Optional Args - [any riid, NULL, NULL] - S_OK
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_3()
{ 
	TBEGIN

	//Get the Original Reference count...
	ULONG ulOrgRefCount = GetRefCount(pIRowset());

	//IRow::GetSourceRowset
	TESTC_(m_pCRowObject->GetSourceRowset(IID_IUnknown, NULL, NULL), E_INVALIDARG);

	//Make sure the refcount was not incresed since we don't have a pointer to release...
	TESTC(ulOrgRefCount == GetRefCount(pIRowset()));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc Optional Args - [any riid, NULL, valid] - S_OK with hRow
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_4()
{ 
	TBEGIN
	HROW hRow = NULL;

	//IRow::GetSourceRowset
	TESTC_(m_pCRowObject->GetSourceRowset(IID_IUnknown, NULL, &hRow),S_OK);

	//Verify this row handle...
	TESTC(VerifyRowHandles(hRow, FIRST_ROW));

CLEANUP:
	ReleaseRows(hRow);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc Optional Args - [IID_IUnknown, valid, NULL] - S_OK with Rowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_5()
{ 
	TBEGIN
	IUnknown* pIUnknown = NULL;

	//IRow::GetSourceRowset
	TESTC_(m_pCRowObject->GetSourceRowset(IID_IUnknown, &pIUnknown, NULL),S_OK);

	//Verify the rowset returned...
	TESTC(DefaultObjectTesting(pIUnknown, ROWSET_INTERFACE));

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc Optional Args - [IID_IRowset, valid, valid] - S_OK with Rowset and hRow
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_6()
{ 
	TBEGIN
	IUnknown* pIUnknown = NULL;
	HROW hRow = NULL;

	//IRow::GetSourceRowset
	TESTC_(m_pCRowObject->GetSourceRowset(IID_IUnknown, &pIUnknown, &hRow),S_OK);

	//Verify the rowset returned...
	TESTC(DefaultObjectTesting(pIUnknown, ROWSET_INTERFACE));

	//Verify this row handle...
	TESTC(VerifyRowHandles(hRow, FIRST_ROW));

CLEANUP:
	ReleaseRows(hRow);
	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_7()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc Sequence - Execute -> Row -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_8()
{ 
	TBEGIN
	CRowset RowsetA;
	CRowObject RowObjectA;
	IRow* pIRow = NULL;
	IUnknown* pIUnknown = NULL;

	//Create a row object from ICommand::Execute
	TEST2C_(RowsetA.pTable()->CreateRowset(SELECT_ALLFROMTBL, IID_IRow, 0, NULL, (IUnknown**)&pIRow),S_OK,DB_S_NOTSINGLETON);

	//Verify the Row object returned...
	TESTC_(RowObjectA.SetRowObject(pIRow),S_OK);
	TESTC(RowObjectA.VerifyGetColumns(FIRST_ROW, RowsetA.pTable(), ALL_COLS_BOUND));

	//Try to get back top the creating rowset object...
	TEST2C_(RowObjectA.GetSourceRowset(IID_IUnknown, &pIUnknown, NULL), S_OK, DB_E_NOSOURCEOBJECT);

CLEANUP:
	SAFE_RELEASE(pIRow);
	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc Sequence - Execute -> Rowset -> Row -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_9()
{ 
	TBEGIN
	CRowset RowsetA;
	CRowObject RowObjectA;
	IUnknown* pIUnknown = NULL;
	HROW hRow = NULL;
	HRESULT hr = S_OK;

	//Create a rowset from ICommand::Execute
	TESTC_(RowsetA.CreateRowset(SELECT_ALLFROMTBL),S_OK);

	//Create the Row Object from the rowset...
	TESTC_(RowsetA.GetNextRows(&hRow),S_OK);
	TEST2C_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);
	TESTC(RowObjectA.VerifyGetColumns(FIRST_ROW, RowsetA.pTable(), ALL_COLS_BOUND));

	//Try to get back top the creating rowset object...
	TEST2C_(hr = RowObjectA.GetSourceRowset(IID_IUnknown, &pIUnknown, NULL), S_OK, DB_E_NOSOURCEOBJECT);
	if(SUCCEEDED(hr))
	{
		VerifyEqualInterface(pIUnknown, RowsetA.pIRowset());
	}

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	RowsetA.ReleaseRows(hRow);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc Sequence - OpenRowset -> Row -> Release Rowset -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_10()
{ 
	TBEGIN
	CRowset RowsetA;
	CRowObject RowObjectA;
	IRow* pIRow = NULL;
	IUnknown* pIUnknown = NULL;

	//Create a row object from IOpenRowset
	TEST2C_(RowsetA.pTable()->CreateRowset(USE_OPENROWSET, IID_IRow, 0, NULL, (IUnknown**)&pIRow),S_OK,DB_S_NOTSINGLETON);

	//Verify the Row object returned...
	TESTC_(RowObjectA.SetRowObject(pIRow),S_OK);
	TESTC(RowObjectA.VerifyGetColumns(FIRST_ROW, RowsetA.pTable(), ALL_COLS_BOUND));

	//Try to get back top the creating rowset object...
	TEST2C_(RowObjectA.GetSourceRowset(IID_IUnknown, &pIUnknown, NULL), S_OK, DB_E_NOSOURCEOBJECT);

CLEANUP:
	SAFE_RELEASE(pIRow);
	SAFE_RELEASE(pIUnknown);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc Sequence - OpenRowset -> Rowset -> Row -> Release Rowset -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_11()
{ 
	TBEGIN
	CRowset RowsetA;
	CRowObject RowObjectA;
	IUnknown* pIUnknown = NULL;
	HROW hRow = NULL;
	HRESULT hr = S_OK;

	//Create a rowset from IOpenRowset
	TESTC_(RowsetA.CreateRowset(USE_OPENROWSET),S_OK);

	//Create the Row Object from the rowset...
	TESTC_(RowsetA.GetNextRows(2, ONE_ROW, &hRow),S_OK);
	TEST2C_(RowObjectA.CreateRowObject(RowsetA.pIRowset(), hRow), S_OK, DB_S_NOROWSPECIFICCOLUMNS);
	TESTC(RowObjectA.VerifyGetColumns(THIRD_ROW, RowsetA.pTable(), ALL_COLS_BOUND));

	//Release the Rowset.
	TESTC_(RowsetA.ReleaseRows(hRow),S_OK);
	RowsetA.DropRowset();
	
	//Try to get back top the creating rowset object...
	TEST2C_(RowObjectA.GetSourceRowset(IID_IUnknown, &pIUnknown, &hRow), S_OK, DB_E_NOSOURCEOBJECT);

	//Verify the row handle returned...
	TESTC_(RowsetA.CreateRowset(pIUnknown),S_OK);
	TESTC(RowsetA.VerifyRowHandles(ONE_ROW, &hRow, THIRD_ROW));
	TESTC_(RowsetA.ReleaseRows(hRow),S_OK);

	//The Rowset returned from GetSourceRowset, usally jsut returns the Rowset with a NFP
	//in the same place.  But since the user has completley released their refcounts, the rowset
	//may or may not still be alive by the providers implementation.  If they internally keep a reference
	//on it for the row, then the NFP will basically be where we left off, if not then it will
	//be at the start of the rowset.
	RowObjectA.ReleaseRowObject();
	TESTC_(RowsetA.GetNextRows(&hRow),S_OK);
	TESTC(RowsetA.VerifyRowHandles(ONE_ROW, &hRow, FOURTH_ROW) || RowsetA.VerifyRowHandles(ONE_ROW, &hRow, FIRST_ROW));

CLEANUP:
	SAFE_RELEASE(pIUnknown);
	RowsetA.ReleaseRows(hRow);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_12()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc Multiple - GetSourceRowset from numerous child row objects
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_13()
{ 
	TBEGIN
	CRowset RowsetA;
	IUnknown* pIUnknown = NULL;
	HRESULT hr = S_OK;
	CRowObject** rgpCRowObjects = NULL;
	ULONG iRow, cRowsObtained = 0;
	HROW* rghRows = NULL;

	//Create a rowset from IOpenRowset
	TESTC_(RowsetA.CreateRowset(DBPROP_CANHOLDROWS),S_OK);

	//Grab as many rows as possible...
	TEST3C_(RowsetA.GetNextRows(0, 1000, &cRowsObtained, &rghRows), S_OK, DB_S_ENDOFROWSET, DB_S_ROWLIMITEXCEEDED);

	//Create the Row Objects from the rowset, (leave active)
	SAFE_ALLOC(rgpCRowObjects, CRowObject*, cRowsObtained);
	memset(rgpCRowObjects, 0, cRowsObtained * sizeof(CRowObject*));
	
	//Loop over all the rows Obtained
	for(iRow=0; iRow<cRowsObtained; iRow++)
	{
		//Create the Row object
		CRowObject* pCRowObject = new CRowObject;
		rgpCRowObjects[iRow] = pCRowObject;
		TEST2C_(pCRowObject->CreateRowObject(RowsetA.pIRowset(), rghRows[iRow]), S_OK, DB_S_NOROWSPECIFICCOLUMNS);
	
		//Try to get back to the creating rowset
		TEST2C_(pCRowObject->GetSourceRowset(IID_IUnknown, &pIUnknown, NULL), S_OK, DB_E_NOSOURCEOBJECT);
		if(pIUnknown)
		{
			//Make sure its returning the original object
			TESTC(VerifyEqualInterface(pIUnknown, RowsetA.pIRowset()));
		}
		SAFE_RELEASE(pIUnknown);
	}
	
CLEANUP:
	for(iRow=0; iRow<cRowsObtained && rgpCRowObjects; iRow++)
		SAFE_DELETE(rgpCRowObjects[iRow]);
	SAFE_FREE(rgpCRowObjects);
	SAFE_RELEASE(pIUnknown);
	RowsetA.ReleaseRows(cRowsObtained, rghRows);
	PROVIDER_FREE(rghRows);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Multiple - Numerous Rowsets open, verify correct parent
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_14()
{ 
	TBEGIN
	const ULONG cRowsets = 5;
	CRowset Rowsets[cRowsets];
	CRowObject RowObjects[cRowsets];
	
	HROW rghRows[cRowsets];
	HROW hRow = NULL;
	IUnknown* pIRowset = NULL;
	HRESULT hr = S_OK;

	//Create rowset(s) from IOpenRowset
	for(ULONG i=0; i<cRowsets; i++)
	{
		//Create Rowsets
		TESTC_(Rowsets[i].CreateRowset(DBPROP_IRowsetIdentity),S_OK);

		//Grab different rows...
		//We use the loop counter as the offset into the rowset of which row to retrive...
		TESTC_(Rowsets[i].GetNextRows(i, 1, &rghRows[i]), S_OK);

		//Create Row Objects...
		TEST2C_(RowObjects[i].CreateRowObject(Rowsets[i].pIRowset(), rghRows[i]), S_OK, DB_S_NOROWSPECIFICCOLUMNS);
	}

	//Now try and Obtain the Source Rowsets...
	for(i=0; i<cRowsets; i++)
	{
		//Try to get back to the creating rowset
		TEST2C_(hr = RowObjects[i].GetSourceRowset(IID_IRowset, &pIRowset, &hRow), S_OK, DB_E_NOSOURCEOBJECT);
		if(SUCCEEDED(hr))
		{
			//Make sure its returning the original object
			TESTC(VerifyEqualInterface(pIRowset, Rowsets[i].pIRowset()));

			//Make sure the row returned is correct...
			TESTC(Rowsets[i].IsSameRow(rghRows[i], hRow));

			//Make sure the original next fetch position is unchanged...
			//GetSourceRowset should not be affecting the NFP what-so-ever...
			TESTC(Rowsets[i].VerifyRowHandles(1, &hRow, FIRST_ROW+i));
		}
		SAFE_RELEASE(pIRowset);
		Rowsets[i].ReleaseRows(hRow);
	}

	//Now that all rows are released, reverify the NFP is unchanged...
	for(i=0; i<cRowsets; i++)
	{
		//Need to release the row object first, since internally it may hold a row handle...
		RowObjects[i].ReleaseRowObject();
			
		//Grab the next row...
		TESTC_(Rowsets[i].GetNextRows(0, 1, &rghRows[i]), S_OK);

		//Verify the position
		TESTC(Rowsets[i].VerifyRowHandles(1, &rghRows[i], FIRST_ROW+i+1));

		//Release the row...
		TESTC_(Rowsets[i].ReleaseRows(rghRows[i]),S_OK);
	}

CLEANUP:
	SAFE_RELEASE(pIRowset);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_15()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Rowset -> Agg Row -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_16()
{ 
	TBEGIN
	HROW hRow = NULL;
    CAggregate Aggregate(pIRowset());
	IUnknown* pIAggregate  = NULL;
	IGetRow* pIGetRow = NULL;
	IRow* pIRow = NULL;
	IUnknown* pIUnkInner = NULL;
	HRESULT hr = S_OK;

	CRowset		RowsetA;
	CRowObject	RowObjectA;

	//Obtain the second row
	TESTC_(RowsetA.CreateRowset(),S_OK);
	TESTC_(RowsetA.GetRow(SECOND_ROW, &hRow),S_OK);
	
	//Obtain a row object...
	hr = RowsetA.GetRowFromHROW(&Aggregate, hRow, IID_IUnknown, (IUnknown**)&pIUnkInner);
	Aggregate.SetUnkInner(pIUnkInner);
	
	//Verify Aggregation for this row...
	TESTC_PROVIDER(Aggregate.VerifyAggregationQI(hr, IID_IRow, (IUnknown**)&pIRow));
	TESTC_(RowObjectA.SetRowObject(pIRow),S_OK);

	//IRow::GetSourceRowset
	TESTC_( hr = RowObjectA.GetSourceRowset(IID_IAggregate, (IUnknown**)&pIAggregate, NULL),E_NOINTERFACE);
	TEST2C_(hr = RowObjectA.GetSourceRowset(IID_IGetRow,	(IUnknown**)&pIGetRow, NULL), S_OK, DB_E_NOSOURCEOBJECT);

	if(hr==S_OK)
	{
		TESTC(VerifyEqualInterface(RowsetA.pIRowset(), pIGetRow));
	}
	else
	{
		TWARNING(L"IRow::GetSourceRowset unable to retrieve Parent object!");
	}

CLEANUP:
	RowsetA.ReleaseRows(hRow);
	SAFE_RELEASE(pIAggregate);
	SAFE_RELEASE(pIGetRow);
	SAFE_RELEASE(pIRow);
	SAFE_RELEASE(pIUnkInner);
	RowObjectA.ReleaseRowObject();
	Aggregate.ReleaseInner();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Agg Rowset -> Row -> GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_17()
{ 
	TBEGIN
	HROW hRow = NULL;
    CAggregate Aggregate(pIRowset());
	IRow* pIRow = NULL;
	IGetRow* pIGetRow = NULL;
	IUnknown* pIAggregate  = NULL;
	COpenRowset OpenRowsetA;
	CRowset RowsetA;
	IUnknown* pIUnkInner = NULL;
	ULONG ulRefCountBefore, ulRefCountAfter;

	//Create a rowset that is Aggregated...
	HRESULT hr = OpenRowsetA.CreateOpenRowset(IID_IUnknown, (IUnknown**)&pIUnkInner, &Aggregate);
	Aggregate.SetUnkInner(pIUnkInner);

	//Verify Aggregation for this rowset...
	TESTC_PROVIDER(Aggregate.VerifyAggregationQI(hr, IID_IGetRow, (IUnknown**)&pIGetRow));

	//Obtain the second row
	TESTC_(RowsetA.CreateRowset(pIGetRow),S_OK);
	TESTC_(RowsetA.GetRow(SECOND_ROW, &hRow),S_OK);
	
	//Now Create the Row (non-aggregated)
	//NOTE: The Aggregated row senario (GetRowFromHROW) is done in the IGetRow test since
	//its explicitly interested in that method.  In IRow were maininly interested in testing
	//GetSourceRowset so we need to do the Aggregated Rowset -> Row senario...
	ulRefCountBefore = Aggregate.GetRefCount();
	TEST2C_(hr = pIGetRow->GetRowFromHROW(NULL, hRow, IID_IRow, (IUnknown**)&pIRow),S_OK,DB_S_NOROWSPECIFICCOLUMNS);
	ulRefCountAfter = Aggregate.GetRefCount();
	
	//IRow::GetSourceRowset
	TEST2C_(hr = pIRow->GetSourceRowset(IID_IAggregate, (IUnknown**)&pIAggregate, NULL),S_OK,DB_E_NOSOURCEOBJECT);

	if(hr==S_OK)
	{
		TESTC(VerifyEqualInterface(pIAggregate, pIGetRow));

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
		TWARNING(L"IRow::GetSourceRowset unable to retrieve Parent object!");
	}


CLEANUP:
	SAFE_RELEASE(pIAggregate);
	SAFE_RELEASE(pIRow);
	SAFE_RELEASE(pIGetRow);
	RowsetA.ReleaseRows(hRow);
	SAFE_RELEASE(pIUnkInner);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_18()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc Threads - GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_GetSourceRowset::Variation_19()
{ 
	TBEGIN
	INIT_THREADS(MAX_THREADS);	
	
	//Setup Thread Arguments
	THREADARG T1Arg = { this };

	//Create Threads
	CREATE_THREADS(Thread_VerifyGetSourceRowset, &T1Arg);

	START_THREADS();
	END_THREADS();	

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCIRow_GetSourceRowset::Terminate()
{ 
// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCIRow::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCIRow_Open)
//*-----------------------------------------------------------------------
//| Test Case:		TCIRow_Open - IRow::Open
//| Created:  	8/5/98
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCIRow_Open::Init()
{ 
	// {{ TCW_INIT_BASECLASS_CHECK
	if(TCIRow::Init())
	// }}
	{ 
		return TRUE;
	} 
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc GUID_NULL - All columns as Default Object
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_1()
{ 
	TBEGIN
	
	//IRow::Open
	TESTC(VerifyOpenAllRows(NULL, GUID_NULL, IID_IUnknown, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_STREAM - All Mandatory and Optional TStream interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_2()
{ 
	TBEGIN
	
	//Open all columns as Mandatory and Optional stream interfaces
	TESTC(VerifyOpenAllInterfaces(STREAM_INTERFACE, DBGUID_STREAM))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_STREAM - Compare data with expected
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_3()
{ 
	TBEGIN
	
	//IRow::Open
	TESTC(VerifyOpenAllRows(NULL, DBGUID_STREAM, IID_ISequentialStream, ALL_COLS_BOUND));
	TESTC(VerifyOpenAllRows(NULL, DBGUID_STREAM, IID_IStream, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_STREAM - Twice on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_4()
{ 
	TBEGIN
	
	//Multiple Objects open
	TESTC(VerifyOpenWithOpenObjects(DBGUID_STREAM, IID_ISequentialStream))
	TESTC(VerifyOpenWithOpenObjects(DBGUID_STREAM, IID_IStream))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(5)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_STREAM - Multiple Objects open on different columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_5()
{ 
	TBEGIN
	
	//Multiple Objects open
	TESTC(VerifyOpenWithOpenObjects(DBGUID_STREAM, IID_ISequentialStream))
	TESTC(VerifyOpenWithOpenObjects(DBGUID_STREAM, IID_IStream))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(6)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_STREAM - Bufferred Mode
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_6()
{ 
	TBEGIN
	
	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//IRow::Open
	TESTC(VerifyOpenAllRows(&RowsetA, DBGUID_STREAM, IID_IStream, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(7)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_7()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(8)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - All Mandatory and Optional TRow interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_8()
{ 
	TBEGIN
	
	//Open all columns as Mandatory and Optional interfaces
	TESTC(VerifyOpenAllInterfaces(ROW_INTERFACE, DBGUID_ROW))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(9)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - Get all row columns and verify
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_9()
{ 
	TBEGIN
	
	//IRow::Open
	TESTC(VerifyOpenAllRows(NULL, DBGUID_ROW, IID_IRow, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(10)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - GetSourceRowset
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_10()
{ 
	TBEGIN
	
	//IRow::Open
	//This helper will call GetSourceRowset from the returned row object... 
	TESTC(VerifyOpenWithOpenObjects(DBGUID_ROW, IID_IRow));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(11)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - Twice on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_11()
{ 
	TBEGIN
	
	//Multiple Objects open
	TESTC(VerifyOpenWithOpenObjects(DBGUID_ROW, IID_IRow))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(12)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - Multiple Objects open on different columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_12()
{ 
	TBEGIN
	
	//Multiple Objects open
	TESTC(VerifyOpenWithOpenObjects(DBGUID_ROW, IID_IRow))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(13)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROW - Bufferred Mode
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_13()
{ 
	TBEGIN
	
	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//IRow::Open
	TESTC(VerifyOpenAllRows(&RowsetA, DBGUID_ROW, IID_IRow, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(14)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_14()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(15)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROWSET - All Mandatory and Optional TRowset interfaces
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_15()
{ 
	TBEGIN
	
	//Open all columns as Mandatory and Optional interfaces
	TESTC(VerifyOpenAllInterfaces(ROWSET_INTERFACE, DBGUID_ROWSET))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(16)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROWSET - Twice on the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_16()
{ 
	TBEGIN
	
	//Multiple Objects open
	TESTC(VerifyOpenWithOpenObjects(DBGUID_ROWSET, IID_IRowset))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(17)
//*-----------------------------------------------------------------------
// @mfunc DBGUID_ROWSET - Buffered mode
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_17()
{ 
	TBEGIN
	
	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Multiple Objects open
	TESTC(VerifyOpenAllRows(&RowsetA, DBGUID_ROWSET, IID_IRowset, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END





// {{ TCW_VAR_PROTOTYPE(18)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_18()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(19)
//*-----------------------------------------------------------------------
// @mfunc DEFAULTSTREAM
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_19()
{ 
	TBEGIN
	
	//VerifyOpenWithOpenObjects also takes case of the DefaultStream object...
	TESTC(VerifyOpenWithOpenObjects(DBGUID_STREAM, IID_IStream))

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(20)
//*-----------------------------------------------------------------------
// @mfunc DEFAULTSTREAM - Buffered mode
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_20()
{ 
	TBEGIN
	
	//Create a buffered mode rowset
	CRowsetUpdate RowsetA;

	//Not all providers will support IRowsetUpdate
	TESTC_PROVIDER(RowsetA.CreateRowset()==S_OK);

	//Multiple Objects open
	TESTC(VerifyOpenAllRows(&RowsetA, DBGUID_STREAM, IID_IStream, ALL_COLS_BOUND));

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(21)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_21()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END








// {{ TCW_VAR_PROTOTYPE(22)
//*-----------------------------------------------------------------------
// @mfunc Boundary - E_INVALIDARG
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_22()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(23)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_BADCOLUMNID - invalid DBID
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_23()
{ 
	TBEGIN
	DBID* pColumnID = NULL;
	DBKIND iKind = 0;
	SAFE_ALLOC(pColumnID, DBID, 1);

	//Loop through all possible DBKINDs
	for(iKind=0; iKind<DBKIND_GUID+1; iKind++)
	{
		//Now Create a unique colid for this type...
		pColumnID->eKind = iKind % (DBKIND_GUID+1);
		TESTC_(CreateUniqueDBID(pColumnID, TRUE/*fInitialize*/),S_OK);

		//DB_E_BADCOLUMNID - invalid DBID
		TESTC_(m_pCRowObject->Open(NULL, pColumnID, GUID_NULL, IID_IUnknown), DB_E_BADCOLUMNID);

		//Error depends upon order of validation in the provider
		TEST2C_(m_pCRowObject->Open(NULL, pColumnID, DBGUID_ROWSET, IID_IRowset), DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);
		TEST2C_(m_pCRowObject->Open(NULL, pColumnID, DBGUID_ROW, IID_IRow), DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);
		TEST2C_(m_pCRowObject->Open(NULL, pColumnID, DBGUID_STREAM, IID_ISequentialStream), DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);
		ReleaseDBID(pColumnID, FALSE/*fDrop*/);
	}

CLEANUP:
	ReleaseDBID(pColumnID, TRUE/*fDrop*/);
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(24)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_BADCOLUMNID - shortcut DBID that doesn't exist
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_24()
{ 
	TBEGIN

	//Loop through all possible DBROWCOLs
	for(ULONG i=0; i<g_cRowColMap; i++)
	{
		//S_OK or DB_E_BADCOLUMNID (depending if column is available...)
		//NOTE: The special columns are not returned in colinfo...
		TEST2C_(m_pCRowObject->Open(NULL, g_rgRowColMap[i].pDBID, GUID_NULL, IID_IUnknown), S_OK, DB_E_BADCOLUMNID);
		
		//Error depends upon order of validation in the provider
		TEST3C_(m_pCRowObject->Open(NULL, g_rgRowColMap[i].pDBID, DBGUID_ROWSET, IID_IRowset), S_OK, DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);
		TEST3C_(m_pCRowObject->Open(NULL, g_rgRowColMap[i].pDBID, DBGUID_ROW, IID_IRow), S_OK, DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);
		TEST3C_(m_pCRowObject->Open(NULL, g_rgRowColMap[i].pDBID, DBGUID_STREAM, IID_ISequentialStream), S_OK, DB_E_BADCOLUMNID, DB_E_OBJECTMISMATCH);

		//NOTE: Positive and negative testing are done in the helper.
		//Also beyond that we should be able to obtain data from the special column
		//TODO: Although this would be a bunch of special casing since its not returned in colinfo
		//and would require hard coding the bindings, etc.  (we already do DBGUID_DEFAULTSTREAM else where)
	}

CLEANUP:
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END






// {{ TCW_VAR_PROTOTYPE(25)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_OBJECTMISMATCH
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_25()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(26)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_OBJECTOPEN
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_26()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(27)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_NOTFOUND
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_27()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(28)
//*-----------------------------------------------------------------------
// @mfunc Boundary - DB_E_DELETEDROW
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_28()
{ 
	// TO DO:  Add your own code here 
	return TRUE;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(29)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_29()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(30)
//*-----------------------------------------------------------------------
// @mfunc Aggregation - Row -> Open -> GetSourceRow
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_30()
{ 
	TBEGIN
	HRESULT hr = S_OK;
	ULONG i,cColAccess = 0;
	DBCOLUMNACCESS* rgColAccess = NULL;
	void* pData = NULL;
	IUnknown* pUnkInner = NULL;
	CAggregate Aggregate;

	//NOTE: The Aggregated Row object senario (Agg Row -> Open -> GetSourceRow) is handled
	//in the GetSourceRow test.  In this senario we are mainly interested in have the object
	//opened aggregated, hence the reason for testing IRow::Open...

	//Create the ColAccess Structures...
	TESTC_(hr = m_pCRowObject->CreateColAccess(&cColAccess, &rgColAccess, &pData),S_OK);
	
	//IRow::Open (for all bound columns)
	for(i=0; i<cColAccess; i++)
	{
		DBTYPE wBaseType = rgColAccess[i].wType & ~(DBTYPE_BYREF | DBTYPE_ARRAY | DBTYPE_VECTOR);

		//First verify the invalid senario (asking for non-IUnknown with Aggregation)
		TESTC_(hr = m_pCRowObject->Open(&Aggregate, &rgColAccess[i].columnid, GUID_NULL, IID_IRow, &pUnkInner),DB_E_NOAGGREGATION);

		//Open wil mainly only be able to be called for columns containing objects.
		//But some providers might be able to open streams, or other types of objects ontop
		//of non-object valued columns.
		hr = m_pCRowObject->Open(&Aggregate, &rgColAccess[i].columnid, GUID_NULL, IID_IUnknown, &pUnkInner);
		Aggregate.SetUnkInner(pUnkInner);
		TEST4C_(hr, S_OK, DB_E_NOTFOUND, DB_E_OBJECTMISMATCH, DB_E_NOAGGREGATION);

		if(SUCCEEDED(hr))
		{
			//You should only be able to aggregate object valued columns.
			TESTC(wBaseType==DBTYPE_IUNKNOWN || wBaseType==DBTYPE_IDISPATCH);

			//Verify Aggregation
			TESTC(Aggregate.VerifyAggregationQI(hr, IID_ISequentialStream));
		}
		else
		{
			if(hr == DB_E_NOAGGREGATION)
			{
				//You should be able to aggregate object valued columns, unless the provider 
				//has already created them or is for some reason unable to recreate them...
				if(wBaseType==DBTYPE_IUNKNOWN || wBaseType==DBTYPE_IDISPATCH)
					TOUTPUT("WARNING:  IUnknown column ( " << i << ") unable to be aggregated with IRow::Open()");
			}
			else
			{
				if(hr == DB_E_OBJECTMISMATCH)
				{
					TESTC(!(wBaseType==DBTYPE_IUNKNOWN || wBaseType==DBTYPE_IDISPATCH));
				}
				else
				{
					//Column data must be null?
				}
			}
		}

		Aggregate.ReleaseInner();
	}
	
	//DBROWCOL_DEFAULTSTREAM
	//Now that we have done the returned columns, lets also see if there is a default stream object.
	TEST4C_(hr = m_pCRowObject->Open(&Aggregate, &DBROWCOL_DEFAULTSTREAM, DBGUID_STREAM, IID_IStream), S_OK, DB_E_NOTFOUND, DB_E_BADCOLUMNID, DB_E_NOAGGREGATION);
	Aggregate.SetUnkInner(pUnkInner);

	//Verify Aggregation
	if(SUCCEEDED(hr))
		TESTC(Aggregate.VerifyAggregationQI(hr, IID_ISequentialStream));
	
CLEANUP:
	FreeColAccess(cColAccess, rgColAccess);
	SAFE_FREE(pData);
	SAFE_RELEASE(pUnkInner);
	Aggregate.ReleaseInner();
	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(31)
//*-----------------------------------------------------------------------
// @mfunc Empty
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_31()
{ 
	return TEST_SKIPPED;
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_VAR_PROTOTYPE(32)
//*-----------------------------------------------------------------------
// @mfunc Threads - Open over the same column
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_32()
{ 
	TBEGIN
	INIT_THREADS(MAX_THREADS);	
	
	//Setup Thread Arguments
	THREADARG T1Arg = { this, (void*)&GUID_NULL, (void*)&IID_IUnknown };

	//Create Threads
	CREATE_THREADS(Thread_VerifyOpenWithOpenObjects, &T1Arg);

	START_THREADS();
	END_THREADS();	

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(33)
//*-----------------------------------------------------------------------
// @mfunc Threads - Open over different columns
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCIRow_Open::Variation_33()
{ 
	TBEGIN
	INIT_THREADS(MAX_THREADS);	
	
	//Setup Thread Arguments
	THREADARG T1Arg = { this, (void*)&GUID_NULL, (void*)&IID_IUnknown };

	//Create Threads
	CREATE_THREADS(Thread_VerifyOpenWithOpenObjects, &T1Arg);

	START_THREADS();
	END_THREADS();	

	TRETURN
} 
// }} TCW_VAR_PROTOTYPE_END


// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCIRow_Open::Terminate()
{ 
// {{ TCW_TERM_BASECLASS_CHECK2
	return(TCIRow::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END


// {{ TCW_TC_PROTOTYPE(TCTransactions)
//*-----------------------------------------------------------------------
//| Test Case:		TCTransactions - IRow inside Transactions
//| Created:  	8/5/98
//*-----------------------------------------------------------------------

//*-----------------------------------------------------------------------
// @mfunc TestCase Initialization Routine
//
// @rdesc TRUE or FALSE
//
BOOL TCTransactions::Init()
{ 
	if(CTransaction::Init())
	{
	   	//register interface to be tested
		if(RegisterInterface(ROWSET_INTERFACE, IID_IGetRow, 0, NULL)) 
			return TRUE;
	}

	//Not all providers have to support transactions
	//If a required interface, an error would have been posted by VerifyInterface
	TEST_PROVIDER(m_pITransactionLocal != NULL);
	return FALSE;
} 




// {{ TCW_VAR_PROTOTYPE(1)
//*-----------------------------------------------------------------------
// @mfunc ABORT with fRetaining TRUE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCTransactions::Variation_1()
{ 
	return VerifyTransaction(FALSE/*fCommit*/, TRUE/*fRetaining*/);
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(2)
//*-----------------------------------------------------------------------
// @mfunc ABORT with fRetaining FALSE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCTransactions::Variation_2()
{ 
	return VerifyTransaction(FALSE/*fCommit*/, FALSE/*fRetaining*/);
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(3)
//*-----------------------------------------------------------------------
// @mfunc COMMIT with fRetaining TRUE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCTransactions::Variation_3()
{ 
	return VerifyTransaction(TRUE/*fCommit*/, TRUE/*fRetaining*/);
} 
// }} TCW_VAR_PROTOTYPE_END




// {{ TCW_VAR_PROTOTYPE(4)
//*-----------------------------------------------------------------------
// @mfunc COMMIT with fRetaining FALSE
//
// @rdesc TEST_PASS or TEST_FAIL 
//
int TCTransactions::Variation_4()
{ 
	return VerifyTransaction(FALSE/*fCommit*/, FALSE/*fRetaining*/);
} 
// }} TCW_VAR_PROTOTYPE_END



// {{ TCW_TERMINATE_METHOD
//*-----------------------------------------------------------------------
// @mfunc TestCase Termination Routine 
//
// @rdesc TEST_PASS or TEST_FAIL 
//
BOOL TCTransactions::Terminate()
{ 
// {{ TCW_TERM_BASECLASS_CHECK2
	return(CTransaction::Terminate());
} 	// }}
// }} TCW_TERMINATE_METHOD_END
// }} TCW_TC_PROTOTYPE_END





