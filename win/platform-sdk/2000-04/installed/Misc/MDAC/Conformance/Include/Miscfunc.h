//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1999 Microsoft Corporation.  
//
// @doc 
//
// @module MiscFunc Header Module  | Header file for Miscellaneous Private Library Functions
//
// @comm
// Special Notes...:	(OPTIONAL NOTES FOR SPECIAL CIRCUMSTANCES)
//
// <nl><nl>
// Revision History:<nl>
//	
//	[00] MM-DD-YY	EMAIL_NAME	ACTION PERFORMED... <nl>
//	[01] 10-05-95	Microsoft	Created <nl>
//	[02] 10-26-95	Microsoft	Added comments and WSTR2DBTYPE function <nl>
//	[02] 12-01-96	Microsoft	Updated for release <nl>
//
// @head3 MISCFUNC Elements|
//
//---------------------------------------------------------------------------

#ifndef _MISCFUNC_H_
#define _MISCFUNC_H_

#include "modcommon.hpp"	// Needed for CThisTestModule
#include "ctable.hpp"		// Needed for CompareData
#include "CModInfo.hpp"		// Needed for CModInfo
#include <stdlib.h>			// Needed for Compare SafeArrays
#include <math.h>			// Needed for pow function
#include <time.h>			// Needed for time function


//-----------------------------------------------------------------------------
// ENUM
//-----------------------------------------------------------------------------
// @enum ECOMPARE_FREE | Used for compare data routine
enum ECOMPARE_FREE{
	COMPARE_FREE,			// @emem compare data and free the memory refereced by the consumer's buffer
	COMPARE_ONLY,			// @emem compare data only.  Do not attempt to free memory
	FREE_ONLY				// @emem free memory only.  Do not attemp to compare data
};

// @enum ECOMPARE_LEVEL | Used for compare data routine
enum ECOMPARE_LEVEL{
	COMPARE_ALL,			// @emem compare data for all columns and binding parts. even if an error is encountered
	COMPARE_UNTIL_ERROR		// @emem stop comparing and exit after first failure occurs
};

// @enum STATEMENTKIND |
enum STATEMENTKIND
{
	eSELECT,
	eINSERT,
	eDELETE,
	eUPDATE,
	eInsertERROR,
	eSelectERROR,
	eSQL,
	eNOCOMMAND
};

// @enum EPREPARE |
enum EPREPARE
{
	PREPARE,
	IMPLICITPREPARE,
	UNPREPARE,
	BOTH,
	NEITHER
};

// INTERFACEMAP
typedef struct _INTERFACEMAP
{
	EINTERFACE	eInterface;
	const IID*	pIID;
	WCHAR*		pwszName;
	BOOL		fMandatory;
	DWORD		dwConfLevel;
	DBPROPID	dwPropertyID;
} INTERFACEMAP;


	
//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// @func  BOOL|
//        MiscFunc ModuleCreateDBSession|
//        Takes information in pThisTestModule and uses it to
//		  create a data source object on the provider, 
//        do initialization if required, and retrieve an 
//		  IDBCreateCommand pointer.  The IDBCreateCommand pointer
//		  is stored in pThisTestModule->m_pIUnknown to be
//		  used later by the test case.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL ModuleCreateDBSession(
 CThisTestModule * pThisTestModule	//@parm [IN] Pointer to module object
);

//---------------------------------------------------------------------------
// @func  BOOL|
//        MiscFunc ModuleReleaseDBSession|
//        Releases the DB Session's IDBCreateCommand interface
//		  retrieved in ModuleCreateDBSession.
//
//---------------------------------------------------------------------------
DLL_EXPORT  BOOL ModuleReleaseDBSession(
 CThisTestModule * pThisTestModule	//@parm [IN] Pointer to module object
);


//--------------------------------------------------------------------
// @func BOOL|
//		  MiscFunc GetInterfaceArray
//		  Returns an array of interface IIDs and associated info for a particular object
//
//--------------------------------------------------------------------
DLL_EXPORT  BOOL GetInterfaceArray
(
	EINTERFACE eInterface, 
	ULONG* pcInterfaces, 
	INTERFACEMAP** prgInterfaces
);


//--------------------------------------------------------------------
// @func BOOL|
//		  MiscFunc DBGUIDtoEINTERFACE
//		  Converts known DBGUID types to corresponding EINTERFACE.
//
//--------------------------------------------------------------------
DLL_EXPORT  EINTERFACE DBGUIDtoEINTERFACE
(
	REFGUID	rguid
);


//--------------------------------------------------------------------
// @func BOOL|
//		 MiscFunc IsConfLevel|
//		 Checks if the Conformance Level of the provider is >= to the "required"
//		 conformance level.
//
// @rdesc Success or Failure
// 	@flag  TRUE  | Required
//	@flag  FALSE | Not Required
//--------------------------------------------------------------------
DLL_EXPORT  BOOL IsConfLevel(DWORD dwProvLevel, DWORD dwReqLevel);


//--------------------------------------------------------------------
// @func BOOL|
//		  MiscFunc IsReqProperty|
//		  Checks if the Property is required for the 
//        conformance level of the provider. 
//
//--------------------------------------------------------------------
DLL_EXPORT  BOOL IsReqProperty
(
	DBPROPID	dwPropertyID,
	GUID		guidPropertySet
);


//--------------------------------------------------------------------
// @func BOOL|
//		  MiscFunc IsReqInterface|
//		  Checks if the interface is required on the given object for the 
//        conformance level of the provider. 
//
//--------------------------------------------------------------------
DLL_EXPORT  BOOL IsReqInterface
(
	EINTERFACE eInterface, 
	REFIID riid
);


//--------------------------------------------------------------------
// @func BOOL|
//		  MiscFunc IsValidInterface|
//		  Checks if the interface is valid on the given object. 
//
//--------------------------------------------------------------------
DLL_EXPORT  BOOL IsValidInterface
(
	EINTERFACE eInterface, 
	REFIID riid
);


//Determines wiether a property is usable in the Tests.
//The property is required (for the level) or were not using Strict
DLL_EXPORT inline BOOL IsUsableProperty(DBPROPID dwPropertyID, GUID guidPropertySet);

//Determines wiether an Interface property is usable in the Tests.
//The Interface is required (for the level) or were not using Strict
DLL_EXPORT inline BOOL IsUsableInterface(EINTERFACE eInterface, REFIID riid);


//--------------------------------------------------------------------
// @func BOOL|
//		Miscfunc VerifyInterface
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL VerifyInterface
(
	IUnknown*		pIUnkIn,								// [IN]	Existing interface to QI on
	REFIID			riid,									// [IN] Interface ID to be returned
	EINTERFACE		eInterface	= UNKNOWN_INTERFACE,		// [IN]	interface Type of pIUnkIn
	IUnknown**		ppIUnkOut	= NULL						// [OUT] Interface returned
);	


DLL_EXPORT BOOL VerifyEqualInterface(IUnknown* pInterface1, IUnknown* pInterface2);


//---------------------------------------------------------------------------
// @func  BOOL|
//        MiscFunc IsProviderReadOnly|
//        Check whether the provider is read only.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL IsProviderReadOnly(
 IUnknown * pSessionIUnknown	//@parm [IN] Pointer to object
);


//---------------------------------------------------------------------------
// @func  BOOL|
//        MiscFunc GetProviderName|
//        Return the Providers string Name.
//
//---------------------------------------------------------------------------
DLL_EXPORT WCHAR* GetProviderName(
 IUnknown * pSessionIUnknown	//@parm [IN] Pointer to object
);


/////////////////////////////////////////////////////////////////////////////
// HRESULT ConvertToMBCS
//
/////////////////////////////////////////////////////////////////////////////
DLL_EXPORT HRESULT	ConvertToMBCS(LPCWSTR pwsz, CHAR* psz, ULONG cStrLen, UINT CodePage = CP_ACP);
DLL_EXPORT CHAR*	ConvertToMBCS(LPCWSTR pwsz, UINT CodePage = CP_ACP);


/////////////////////////////////////////////////////////////////////////////
// HRESULT ConvertToWCHAR
//
/////////////////////////////////////////////////////////////////////////////
DLL_EXPORT HRESULT ConvertToWCHAR(LPCSTR psz, WCHAR* pwsz, ULONG cStrLen);
DLL_EXPORT WCHAR* ConvertToWCHAR(LPCSTR psz);

/////////////////////////////////////////////////////////////////////////////
// WCHAR* wcsDuplicate
//
/////////////////////////////////////////////////////////////////////////////
DLL_EXPORT WCHAR* wcsDuplicate(LPCWSTR pwsz);


/////////////////////////////////////////////////////////////////////////////
// String Manipulations
//
/////////////////////////////////////////////////////////////////////////////
DLL_EXPORT WCHAR* FindCharacter(WCHAR* pwszStart, WCHAR* pwszEnd, WCHAR wChar);
DLL_EXPORT WCHAR* FindSubString(WCHAR* pwszBuffer, WCHAR* pwszKeyword, BOOL fCaseSensitive = FALSE);

DLL_EXPORT WCHAR* SkipWhiteSpace(WCHAR* pwszStart, WCHAR* pwszEnd = NULL, WCHAR* pwszWhiteSpace = L" ");
DLL_EXPORT HRESULT	ReplaceString(WCHAR* pwszBuffer, WCHAR* pwszTokens, WCHAR* pwszReplace);



/////////////////////////////////////////////////////////////////////////////
// Storage Object routines
//
/////////////////////////////////////////////////////////////////////////////
// 64bit TODO - Change the ULONGs to DBLENGTH after the stream interfaces are modified for 64 bit.
DLL_EXPORT HRESULT StorageRead(REFIID riid, IUnknown* pIUnknown, void* pBuffer, ULONG cBytes, ULONG* pcBytesRead = NULL, ULONG ulOffset = 0);
DLL_EXPORT HRESULT StorageWrite(REFIID riid, IUnknown* pIUnknown, void* pBuffer, ULONG cBytes, ULONG* pcBytesWrote = NULL, ULONG ulOffset = 0);


//---------------------------------------------------------------------------
//	@func BOOL|
//  MiscFunc CompareData|
//  The function compares the data in the user's buffer with the data at the backend 
//	table. The data is retrieved from the rowset by an accessor. The status, length and value 
//	bindings for each column are checked as specified in the accessor.  
//
//	This routine only checks data retrieved by DBBINDIO_READWRITE and DBBINDIO_READCOLUMNSBYREF
//	accessors. DBBINDIO_READBYREF accessor is not supported by Kagera and we are testing it.
//
// @rdesc Comparision results
// @flag TRUE | Data is the same as the backend.  
// @flag FALSE | Data is not the same as the backend.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareData 
(
	DBORDINAL		cColumns,		//@parm [in]	the count of rgColumnsOrd
	DB_LORDINAL*	rgColumnsOrd,	//@parm [in]	the array of column ordinals in the backend table.
									//				The column ordinals in the backend table is 
									//				not the same as ordinals in the rowset.  
	DBCOUNTITEM		cRow,			//@parm[in]	the row number of the data at the backend table
	void*			pData,			//@parm[in]	the pointer to the buffer which contains the data
									//				to be compared with
	DBCOUNTITEM		cBinding,		//@parm[in]	the count of the rgBinding
	DBBINDING*		rgBinding,		//@parm[in]	the binding information of the accessor which 
									//				retrieved the data
	CSchema*		pSchema,		//@parm[in]	The pointer to CTable object from which the 
									//				the rowset was created.
	IMalloc*		pIMalloc,		//@parm [in]	the IMalloc pointer used to free memory.
													//				can not be NULL.
	EVALUE			eValue	=	PRIMARY,			//@parmopt [in]	whether use PRIMARY or SECONDARY to make a data
													//
	ECOMPARE_FREE	eCompareFree = COMPARE_FREE,	//@parmopt[in]		
													//COMPARE_FREE			
													//compare data and free the memory refereced 
													//by the consumer's buffer
													//COMPARE_ONLY
													//compare data only.  Do not attempt to free memory
													//FREE_ONLY
													//free memory only.  Do not attemp to compare data
													// (default = COMPARE_FREE)
	ECOMPARE_LEVEL eCompareLevel = COMPARE_ALL,		//@paramopt [in] Whether or not to stop when
													//first error is encountered.  Default = COMPARE_ALL, meaning
													//comparison continues even after an error occurs
	BOOL			bCompareValue = TRUE 			// @paramopt [in] flag to tell whether or not to compare Value when
													// status is null.  Default is TRUE (i.e. Compare Value when Status is
													// null.  when bCompareValue == FALSE then value field is not compared.
); 

//---------------------------------------------------------------------------
//	@func BOOL|
//  MiscFunc CompareBuffer|
//  The function compares the data in two buffers based on the binding
//	structure.  It assumes the two buffers share the same binding structure.
//	When fSetData is TRUE, pSetData should points to a buffer that is used
//	for setdata/setnewdata.  The status of the buffer should only be 
//	DBCOLUMNSTATUS_OK or DBCOLUMNSTATUS_NULL.
//
// @rdesc Comparision results
// @flag TRUE | Data in the two buffers is the same.
// @flag FALSE | Data in the two buffers is not the same..
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL	CompareBuffer
(
	void*			pGetData,						//@parm[in]: the pointer to one buffer.  
	void*			pSetData,						//@parm[in]: the pointer to the second buffer
													//			This should be the buffer to 
													//			set the data with
	DBCOUNTITEM		cBinding,						//@parm[in]: the count of rgBinding	
	DBBINDING*		rgBinding,						//@parm[in]: the binding structure of the buffer
	IMalloc*		pIMalloc,						//@parm[in]: IMalloc pointer for free memory
	BOOL			fSetData			= FALSE,	//@parm[in]: pSetData is used for set data.
	BOOL			fReadColumnsByRef	= FALSE,	
													//@parm[in]: TRUE if the columns are binded by an accessor
													//			of type DBBINDIO_READCOLUMNSBYREF
	ECOMPARE_FREE	eCompareFree		= COMPARE_FREE,	
													//@parm[in]:		
													//COMPARE_FREE		
													//compare data and free the memory refereced 
													//by the consumer's buffer
													//COMPARE_ONLY		
													//compare data only.  Do not attempt to free memory
													//FREE_ONLY			
													//free memory only.  Do not attemp to compare data
	BOOL		fAllowUnavailable		= FALSE,
													//@parm[in]: TRUE if DBSTATUS_S_UNAVAILABLE is a valid value in GetData
													//			 e.g. GetData after a SetData or an InsertRow with DBSTATUS_S_DEFAULT
													//			 default value FALSE
	DBCOUNTITEM		cSetBindings		= 0,
	DBBINDING*		rgSetBindings		= NULL
);


//---------------------------------------------------------------------------
// @func BOOL|
// MiscFunc CompareSafeArray|
// Compare two SafeArrays. Free memory pointed by pBackEndData if required.
//
// @rdesc TRUE if data is the same as the backend.  FALSE otherwise.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareSafeArray(
SAFEARRAY	*pConsumerData,	//@parm [in] the pointer to a safearray in the consumer's buffer
SAFEARRAY	*pBackEndData,	//@parm [in] the pointer to a safeaarry at the backend
DBTYPE		dwType,			//@parm [in] the expected DBType of the data in the safearray.
							//		It will be checked against the base vt type of 
							//		the safearray.  If dwType==INVALID_DBTYPE, the 
							//		SafeArray's base type will not be checked.  It
							//		can not be ORed with any type modifers.
IMalloc		*pIMalloc,		//@parm [in] pointer to IMalloc.  Can be NULL if
							//		fFreeMemory is FALSE.
BOOL		fFreeMemory=TRUE	//parmopt [in] whether to free the memory pointed
							//		by pBackEndData	(Default = TRUE)

);

//---------------------------------------------------------------------------
//	@func BOOL|
//  MiscFunc CompareVector|
//  Compare two vectors.  Free memory pointed by pBackEndData if required.  
//
//  @rdesc TRUE if data is the same as the backend.  FALSE otherwise.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareVector(
DBVECTOR	*pConsumerData,	//@parm [in] the pointer to the vector in consumer's buffer
DBVECTOR	*pBackEndData,	//@parm [in] the pointer to the vector at the backend
DBTYPE		dwType,			//@parm [in] the DBType indicator of the data.  It can
							//			  not be ORed with any DBType modifiers.
DBLENGTH	ulSize,			//@parm [in] the size of one element in the vector.
							//			  Only valid for DBTYPE_BYTES
BYTE		bPrecision,		//@parm [in] the precision
							//			  Only valid for DBTYPE_DBNUMERIC or DBTYPE_DECIMAL
BYTE		bScale,			//@parm [in] the scale
							//			  Only valid for DBTYPE_DBNUMERIC or DBTYPE_DECIMAL
BOOL		fFreeMemory=TRUE//@parmopt [in] whether to free the memory pointed
							//			  by pBackEndData (default = TRUE)
);

//---------------------------------------------------------------------------
//	@func	BOOL|
//          MiscFunc CompareDBTypeData|
//          Compare two data of any DBTYPE. 
//			Free memory pointed by pBackEndData if requested.
//
//  @rdesc TRUE if data is the same as the backend.  FALSE otherwise.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareDBTypeData(
void		*pConsumerData,				//@parm [in] the pointer to consumer data
void		*pBackEndData,				//@parm [in] the pointer to data at the backend
DBTYPE		dwType,						//@parm [in] the DBType of the data.  It can not be ORed with
										//			  any DBType modifers.
DBLENGTH	ulBackEndSize,				//@parm [in] the size of the data, only valid for DBTYPE_BYTES 
										//or DBTYPE_VARNUMERIC
BYTE		bPrecision,					//@parm [in] the precision
										//			  Only valid for DBTYPE_DBNUMERIC or DBTYPE_DECIMAL
BYTE		bScale,						//@parm [in] the scale
										//			  Only valid for DBTYPE_DBNUMERIC or DBTYPE_DECIMAL
IMalloc		*pIMalloc,					//@parm [in] the pointer tp IMalloc for freeing memory.
										//			  pIMalloc can not NULL if fFreeMemory is FALSE.
BOOL		fFreeMemory=TRUE,			//@parmopt [in] whether to free the memory pointed
										//			by pBackEndData. (default = TRUE)
DBTYPE		wBackEndType=DBTYPE_EMPTY,	//@parm [in]: data type of the backend, default DBTYPE_EMPTY.

DBLENGTH	cbConsumerSize=0			//@parm [in]: the size of data pointed to by pConsumerData, only 
										//valid for DBTYPE_VARNUMERIC
);

//---------------------------------------------------------------------------
// @func: BOOL|
//        MiscFunc CompareDateTime|
//        Compare two date time structs.  Does not attempt to free memory
//		  The DBType can be either DBTYPE_DATESTRUCT, DBTYPE_TIMESTRUCT, or
//		  DBTYPE_TIMESTAMPSTRUCT
//
// @rdesc: TRUE if data is the same as the backend.  FALSE otherwise.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareDateTime(
	void *pConsumerData,	//@parm [in] Pointer to the date time struct in the consumer's buffer.
	void *pBackEndData,		//@parm [in] Pointer to the date time struct at the backend.
	DBTYPE dwType			//@parm [in] the type indicator
);

//---------------------------------------------------------------------------
// @func BOOL|
//       MiscFunc CompareVariant|
//       Compare two variants.  Does not attempt to free memory
//
// @rdesc TRUE if data is the same as the backend.  FALSE otherwise.
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareVariant
(
VARIANT *pVariantOrg,	//@parm [in] Pointer to the variant in the consumer's buffer.
VARIANT *pVariantCpy,	//@parm [in] Pointer to the variant at the backend.
BOOL	fCaseSensitive	= TRUE
);

//---------------------------------------------------------------------------
// @func BOOL|
//       MiscFunc CompareDBID|
//       Compares to DBIDs, TRUE indicates that they are the same
//
//---------------------------------------------------------------------------
DLL_EXPORT BOOL CompareDBID(
	const DBID& x,							//@parm [IN] First DBID to compare
	const DBID& y,							//@parm [IN] Second DBID to compare
	IUnknown	*pIUnknown = NULL	//@parm [IN] DSO interface 
);


//---------------------------------------------------------------------------
// @func BOOL|
//       MiscFunc CreateUniqueDBID|
//
//---------------------------------------------------------------------------
DLL_EXPORT HRESULT CreateUniqueDBID(
	DBID*		pDBID,					//@parm [IN] Pointer to storage for new DBID
	BOOL		fInitialize = FALSE
);


DLL_EXPORT HRESULT VariantToString(VARIANT* pVariant, WCHAR* pwsz, ULONG ulMaxSize, BOOL fDispVarBool = TRUE);
DLL_EXPORT HRESULT StringToVariant(WCHAR* pwsz, VARTYPE vt, VARIANT* pVariant);

DLL_EXPORT HRESULT SafeArrayToString(SAFEARRAY* pSafeArray, DBTYPE wType, WCHAR* pwsz, DBLENGTH ulMaxSize);
DLL_EXPORT HRESULT StringToSafeArray(WCHAR* pwsz, DBTYPE wType, SAFEARRAY** ppSafeArray);

DLL_EXPORT HRESULT StringToVector(WCHAR* pwszBuffer, DBTYPE wType, DBVECTOR* pVector);
DLL_EXPORT HRESULT VectorToString(DBVECTOR* pVector, DBTYPE wType, WCHAR* pwszBuffer, DBLENGTH ulMaxSize);


//---------------------------------------------------------------------------
// @func	void *|
//          MiscFunc WSTR2DBTYPE|
//          Converts a DB_WSTR to any other DB_TYPE
// @rdesc	Pointer to converted data, of type dbDestType.  Caller
//			must free this memory using IMalloc::Free.	
//
//---------------------------------------------------------------------------
DLL_EXPORT VARIANT* DBTYPE2VARIANT(
	void*		pvSource,	//@parm [IN] Value to be converted, must NOT be null
	DBTYPE		wType		//@parm [IN] DBTYPE of Value
);

//---------------------------------------------------------------------------
// @func	void *|
//          MiscFunc WSTR2DBTYPE|
//          Converts a DB_WSTR to any other DB_TYPE
// @rdesc	Obsolete, using USHORT for the returned count of bytes is not
//			adequate.  Newer tests used should the other overloaded func
//
//---------------------------------------------------------------------------
DLL_EXPORT void*  WSTR2DBTYPE(
	WCHAR *		wszSource,	//@parm [IN] String to be converted, must NOT be null
	DBTYPE		dbDestType,	//@parm [IN] DBTYPE to convert to
	USHORT *	pcb,		//@parm [OUT] Pointer to USHORT to store count 
							//		of bytes.  This is ignored unless
							//		dbDestType == DBTYPE_BYTES or DBTYPE_VARNUMERIC
	BOOL		fLaxConvert	= FALSE	//@param [IN] optional Whether to use strict error checking. DEFAULT = FALSE
);


//---------------------------------------------------------------------------
// @func	void *|
//          MiscFunc WSTR2DBTYPE_EX|
//          Converts a DB_WSTR to any other DB_TYPE
// @rdesc	Pointer to converted data, of type dbDestType.  Caller
//			must free this memory using IMalloc::Free.	
//
//---------------------------------------------------------------------------
DLL_EXPORT void*  WSTR2DBTYPE_EX(
	WCHAR *		wszSource,	//@parm [IN] String to be converted, must NOT be null
	DBTYPE		dbDestType,	//@parm [IN] DBTYPE to convert to
	ULONG *		pcb,		//@parm [OUT] Pointer to USHORT to store count 
							//		of bytes.  This is ignored unless
							//		dbDestType == DBTYPE_BYTES or DBTYPE_VARNUMERIC
	BOOL		fLaxConvert	= FALSE	//@param [IN] optional Whether to use strict error checking. DEFAULT = FALSE
);

//---------------------------------------------------------------------------
//	@func LONG|
//        MiscFunc GetDBTypeSize|
//        Return the length of a DBTYPE.  It returns 0 for any data type ORed with
//
//	DBTYPE_RESERVED, DBTYPE_ARRAY, DBTYPE_VECTOR.  It returns 0 for DBTYPE_EMPTY, 
//	DBTYPE_NULL, and any variable length data types.  It returns INVALID_DBTYPE_SIZE 
//	for invalid input.  If the data is ORed with DBTYPE_BYREF, the bit flag is ignored
//
//---------------------------------------------------------------------------
DLL_EXPORT LONG GetDBTypeSize(
DBTYPE dwDBType		//@parm [in] The DBType to compute 
);

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc CleanUpVector|
// Clean up any memory allocated for the vector.
//
//-----------------------------------------------------------------------------
DLL_EXPORT void	CleanUpVector(
 DBVECTOR	*pConsumerData, //@parm  [in] the pointer to the vector
 DBTYPE		dwType			//@parm [in] the dwType of the element in the vector
							//			 It can only ORed with DBTYPE_VECTOR
 );

//-----------------------------------------------------------------------------
// @func DBTYPE|
//        MiscFunc VARTYPE2DBTYPE|
//        Return the equivalent DBTYPE for a valid base VARTYPE of a SafeArray.
//		  The base type of a safearry is restricted to a subset of the variant type.  
//		  Neither the VT_RESERVED and VT_BYREF flags can be set.  
//		  VT_EMPTY and VT_NULL are not valid.
//
//-----------------------------------------------------------------------------
DLL_EXPORT DBTYPE	VARTYPE2DBTYPE(
VARTYPE	vt	//@parm [in] The vt tag of the safearray
);

//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc CountRowsOnRowset|
// Counts rows on rowset.
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT CountRowsOnRowset(
 IRowset* pIRowset, //@parm [in] Rowset object
 DBCOUNTITEM* pcRows		//@parm [out] Count of rows
);

//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc GetAccessorAndBindings|
// Creates an Accessor and bindings.  The DBTYPE returned by
// IColumnsInfo for the command/rowset is used for each column's binding.
// NOTE:  The user may create an accessor with a flag of PARAMETER_DATA,
// but the bindings will be based on column information rather than 
// parameter information. The user should use GetParameterAccessorAndBindings
// to generate a valid accessor for PARAMETER_DATA (one in which the coercions
// in the bindings will be valid for the specified parameter number).
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT 	GetAccessorAndBindings(
	IUnknown *			pIUnkObject,				// @parm [IN]  Rowset, Command, or RowObject to create bindings for
	DBACCESSORFLAGS		dwAccessorFlags,			// @parm [IN]  Properties of the Accessor
	HACCESSOR *			phAccessor=NULL,			// @parmopt [OUT] Accessor created
	DBBINDING **		prgDBBINDING=NULL,			// @parmopt [OUT] Array of DBBINDINGS
	DBCOUNTITEM *		cBindings=NULL,				// @parmopt [OUT] Count of bindings
	DBLENGTH *			cbRowSize=NULL,				// @parmopt [OUT] Length of a row, DATA	
	DBPART				dwPart = DBPART_VALUE |		// @parmopt [IN]  Types of binding to do (Value,
						DBPART_STATUS |				// Status, and/or Length)	
						DBPART_LENGTH,
	DWORD				dwColsToBind = ALL_COLS_BOUND,// @parmopt [IN] Which columns will be used in the bindings
	ECOLUMNORDER		eBindingOrder = FORWARD,	// @parmopt [IN]  Order to bind columns in accessor												
	ECOLS_BY_REF		eColsByRef = NO_COLS_BY_REF,// @parmopt [IN]  Which column types to bind by reference
	DBCOLUMNINFO **		prgDBCOLUMNINFO = NULL,		// @parmopt [OUT] Array of DBCOLUMNINFO
	DBORDINAL *			cCols = NULL,				// @parmopt [OUT] Count of Columns, also count of ColInfo elements
	WCHAR **			ppStringsBuffer = NULL,		// @parmopt [OUT] ppStringsBuffer				
	DBTYPE				dwModifier = DBTYPE_EMPTY,	// @parmopt [IN]  Modifier to be OR'd with every binding's type.
													// Note, this modifying is in addition to that specified by eColsByRef.
													// No OR'ing is done if dwModifier is DBTYPE_EMPTY, except that specified
													// by eColsByRef
	DBORDINAL			cColsToBind = 0,			// @parmopt [IN]  Used only if dwColsToBind = USE_COLS_TO_BIND_ARRAY
													// specifies the number of elements in rgColsToBind array
	LONG_PTR *			rgColsToBind = NULL,		// @parmopt [IN]  Used only if dwColsToBind = USE_COLS_TO_BIND_ARRAY
													// Specifies array of column ordinals to be bound.
	DB_UPARAMS *		rgColOrdering = NULL,		// @parmopt [IN] Corresponds to what numbering the client wants to
													// follow. Such as bind col2,col4,col6 but because of param data,
													// want the ordering to be 1,3,5. This array would have the 1,3,5.
	ECOLS_MEM_PROV_OWNED 
						eColsMemProvOwned = NO_COLS_OWNED_BY_PROV,	//@paramopt [IN] Which columns' memory is to be owned by the provider
	DBPARAMIO			eParamIO = DBPARAMIO_NOTPARAM,		//@paramopt [IN] Parameter type to specify for eParmIO
	BLOBTYPE			dwBlobType = NO_BLOB_COLS,	//@paramopt [IN] How to bind the blob colums
	DBBINDSTATUS**		prgStatus = NULL			// @parmopt [OUT] Array of DBBINDSTATUS
);

//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc FreeAccessorBindings |
// Frees the memory assoicated with rgBindings
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT 	FreeAccessorBindings(
	DBCOUNTITEM	cBindings,				// @parmopt [IN] Count of bindings
	DBBINDING*	rgDBBINDING 			// @parmopt [IN] Array of DBBINDINGS
);



//----------------------------------------------------------------------
// MiscFunc VerifyPropertyInfo |
// Verify PropertyInfo and Display any error status
// 
// @mfunc BOOL|
// 
//----------------------------------------------------------------------
BOOL VerifyPropertyInfo(HRESULT hrReturned, ULONG cPropInfoSets, DBPROPINFOSET* rgPropInfoSets, OLECHAR* pDescBuffer, BOOL fAlwaysTrace = FALSE);


//----------------------------------------------------------------------
// MiscFunc VerifyProperties |
// Verify Properties and Display any error status
// 
// @mfunc BOOL|
// 
//----------------------------------------------------------------------
BOOL VerifyProperties(HRESULT hrReturned, ULONG cPropSets, DBPROPSET* rgPropSets, BOOL fOpenRowset = TRUE, BOOL fAlwaysTrace = FALSE);


//----------------------------------------------------------------------
// MiscFunc VerifyPropertiesInError |
// Verify Properties and Display any error status
// 
// @mfunc BOOL|
// 
//----------------------------------------------------------------------
BOOL VerifyPropertiesInError(HRESULT hrReturned, IUnknown* pIUnknown);


//----------------------------------------------------------------------
// MiscFunc VerifyBindings |
// Verify Bindings and Display any error binding status
// 
// @mfunc BOOL|
// 
//----------------------------------------------------------------------
DLL_EXPORT BOOL VerifyBindings(HRESULT hrReturned, DBCOUNTITEM cBindings, DBBINDING* rgBindings, void* pData);


//----------------------------------------------------------------------
// MiscFunc VerifyColAccess |
// Verify DBCOLUMNACCESS structures and Display any error binding status
// 
// @mfunc BOOL|
// 
//----------------------------------------------------------------------
DLL_EXPORT BOOL VerifyColAccess(HRESULT hrReturned, DBORDINAL cColAccess, DBCOLUMNACCESS* rgColAccess);


//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc FreeColAccess |
// Frees the memory assoicated with rgColAccess
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT 	FreeColAccess(
	DBORDINAL		cColAccess,				// @parmopt [IN] Count of ColAccess
	DBCOLUMNACCESS*	rgColAccess, 			// @parmopt [IN] Array of ColAccess
	BOOL			fFreeOuter = TRUE		// @parmopt [IN] Whether or not to free rgColAccess
);


//-----------------------------------------------------------------------------
// @func void|
// MiscFunc PrintRowset|
// Prints rowset as strings
//
//-----------------------------------------------------------------------------
DLL_EXPORT void PrintRowset(
	IRowset * pIRowset		// @parm Rowset to print
);

//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc GetStringAccessorAndBindings|
// Bind all columns as strings
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT GetStringAccessorAndBindings(
	IUnknown *			pIUnkObject,			// @parm [IN]  Rowset, Command, or RowObject to create bindings for
	DBCOLUMNINFO *		rgDBCOLUMNINFO,			// @parm [IN]  Array of Column info
	DBORDINAL			cDBCOLUMNINFO,			// @parm [IN]  Count of Column info
	HACCESSOR *			phAccessorOut,			// @parm [OUT] Accessor created
	DBBINDING **		prgDBBINDINGOut,		// @parm [OUT] Array of DBBINDINGS
	DBCOUNTITEM *		pcBindingsOut,			// @parm [OUT] Count of bindings
	DBLENGTH *			pcbRowSizeOut			// @parm [OUT] Length of a row, DATA	
);


DLL_EXPORT BOOL FindColInfo(IUnknown* pIUnknown, DBID* pColumnID, DBORDINAL iOrdinal = 0, DBCOLUMNINFO* pColInfo = NULL, WCHAR** ppStringBuffer = NULL);

//-----------------------------------------------------------------------------
// @func HRESULT|
// MiscFunc FillInputBindings|
// Fill in data buffer with values that can be read by the hAccessor on input
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT FillInputBindings(				
	CSchema* pSchema,					// @parm [IN] Table object on which to do call MakeData	
	DBACCESSORFLAGS dwAccessorFlags,	// @parm [IN] Flags for accessor.  If its a PARAMETERDATA accessor, 
										// the rgColOrds contains the ordinals to be passed to MakeData
										// to generate the data for each parameter, with each element
										// corresponding to a parameter (ie, element 0 correpsonds to parameter 1 and so on).
										// If its a ROWDATA or a ROWDATA | PARAMETERDATA accessor, rgColOrds
										// is simply the array of all columns IN THE ROWSET, and the bindings
										// are used to determine which columns are bound and thus which ordinals
										// are passed to MakeData to generate the correct values.  Note the
										// PARAMETERDATA only accessor scenario will require the user to build
										// an array with only the columns that pertain to each parameter, and in the 
										// correct order, whereas the ROWDATA accessor scenario will require the 
										// user to pass an array with all columns, regardless of which ones are bound.
	DBCOUNTITEM cBindings,				// @parm [IN] Number of bindings
	DBBINDING * rgBindings,				// @parm [IN] Bindings of hAccessor
	BYTE ** ppData,						// @parm [IN/OUT] Data buffer to be filled.  User must free 
										// *ppData unless they pass a non null *ppData when calling, which
										// is large enough to hold a row of data.
	DBCOUNTITEM ulRowNum,				// @parm [IN] Row number to create data with
	DBORDINAL cColOrds,					// @parm [IN] Count of elements in rgColOrds
	DB_LORDINAL * rgColOrds,			// @parm [IN] For PARAMETERDATA accessors, each element in the array 
										// specifies the iNumber of a column to be bound.  This iNumber should 
										// be the same as that returned by IColumnsInfo.
										// This iNumber is always ordered the same as the column list in the command 
										// specification, thus the user can use the command specification col list
										// order to determine the appropriate iNumber.
										// For ROWDATA accessors, the array must contain the table ordinals for
										// all cols in the rowset, regardless of which ones are bound.  The bindings
										// will be used to determine which ones are bound and thus what ordinals are
										// passed to MakeData.
	EVALUE eValue = PRIMARY,			// @parmopt [IN] Type of data to create	
	IUnknown* pIUnkObject = NULL,		// Pointer to object
	DWORD	dwColsToBind = UPDATEABLE_COLS_BOUND	// @parmopt [IN] Which columns will be used in the bindings
);

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc ReleaseInputBindingsMemory|
// Function to be called to clean up any out of line memory allocated
// by FillInputBindings.
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT ReleaseInputBindingsMemory(			
		DBCOUNTITEM cBindings,				// @parm [IN] Number of bindings
		DBBINDING * rgBindings,			// @parm [IN] Bindings of hAccessor
		BYTE * pData,					// @parm [IN] Input Data buffer 
		BOOL fFreeData = FALSE			// @parmopt [IN] Whether or not to free pData 
		);


//-----------------------------------------------------------------------------
// @func void|
// MiscFunc GetModInfo|
//	This function returns a CModInfo object pointer
//
//-----------------------------------------------------------------------------
DLL_EXPORT CModInfo* GetModInfo();

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc GetRootTable|
//	This function returns a CTable object pointer
//
//-----------------------------------------------------------------------------
DLL_EXPORT CTable* GetRootTable();

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc CreateModInfo|
//	This function Creates a ModInfo object
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL CreateModInfo(CThisTestModule* pCThisTestModule);

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc GetInitInfo|
//	This function Release a ModInfo object
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL ReleaseModInfo(CThisTestModule* pCThisTestModule);

//-----------------------------------------------------------------------------
// @func void|
// MiscFunc GetInitProps|
//	This function parses the wszInitString (obtained from the LTM) to find
//	the necessary init options, and builds the correct arrays needed
//	to pass to IDBInitialize::Initialize().  User must call delete rather
//	than IMalloc->Free on *prgOptionIDs and prgOptionVals, as well as ClearVariant
//	on all members of the *prgOptionVals array. 
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL GetInitProps
(
	ULONG*				pcPropSets,				//@parm [IN/OUT]:	Pointer to memory to hold count of DBPROPSET structs.
	DBPROPSET**			prgPropSets				//@parm [IN/OUT]:	Pointer to memory to hold an array of DBPROPSET
												//					this routine allocates and populates.
												//					*NOTE:* User must call FreeProperties 
 );


//-----------------------------------------------------------------------------
// @func LPWSTR|
// MiscFunc BSTR2WSTR|											
//
// Returns the WCHAR string contained in the BSTR, but with a NULL terminator.
// If error occurs, NULL is returned.  Caller is responsible
// for freeing the returned string, but if fFreeBstr is set, the function 
// calls SysFreeString on the input bstr.
// If the input bstr is NULL, the string returned is "NULL!"
//
//-----------------------------------------------------------------------------
DLL_EXPORT LPWSTR BSTR2WSTR(
			BSTR	bstr,			//@parm [IN] BSTR to convert to WSTR
			BOOL fFreeBstr = FALSE	//@parmopt [IN] Flag indicating if the input bstr 
									//is freed by this function.
);

//--------------------------------------------------------------------
// @func BOOL | SupportedProperty
//
// This function should return TRUE if the fuction succeeded and 
// the property is supported by the Provider.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL SupportedProperty(DBPROPID PropertyID, GUID guidPropertySet, IUnknown* pIUnknown,EINTERFACE eCoType=DATASOURCE_INTERFACE);

//--------------------------------------------------------------------
// @func BOOL | SettableProperty
//
// This function should return TRUE if the fuction succeeded and 
// the property is settable by the Provider.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL SettableProperty(DBPROPID PropertyID, GUID guidPropertySet, IUnknown* pIUnknown,EINTERFACE eCoType=DATASOURCE_INTERFACE);

//--------------------------------------------------------------------
// @func BOOL | GetPropInfoFlags
//
// This function should return the DBPROPFLAGS of the Property.
//
//--------------------------------------------------------------------
DLL_EXPORT DBPROPFLAGS GetPropInfoFlags(DBPROPID PropertyID, GUID guidPropertySet, IUnknown* pIUnknown,EINTERFACE eCoType=DATASOURCE_INTERFACE);

//--------------------------------------------------------------------
// @func BOOL | GetPropInfo
//
// This function should return the DBPROPINFO of the requested Property.
//
//--------------------------------------------------------------------
DLL_EXPORT DBPROPINFO* GetPropInfo(DBPROPID PropertyID, GUID guidPropertySet, IUnknown* pIUnknown,EINTERFACE eCoType=DATASOURCE_INTERFACE);

//--------------------------------------------------------------------
// @func BOOL | GetProperty
//
// This function should return TRUE if the fuction succeeded and 
// the boolean value of a property.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL GetProperty(
	DBPROPID PropertyID, 
	GUID guidPropertySet, 
	IUnknown* pIUnknown,
	VARIANT_BOOL bValue = VARIANT_TRUE
);

//--------------------------------------------------------------------
// @func BOOL | GetProperty
//
// This function should return TRUE if the fuction succeeded and 
// any value type of a property.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL GetProperty(
	DBPROPID PropertyID, 
	GUID guidPropertySet, 
	IUnknown* pIUnknown, 
	VARIANT_BOOL* pbValue
);

//--------------------------------------------------------------------
// @func BOOL | GetProperty
//
// This function should return TRUE if the fuction succeeded and 
// any value type of a property.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL GetProperty(
	DBPROPID PropertyID, 
	GUID guidPropertySet, 
	IUnknown* pIUnknown, 
	ULONG_PTR* pulValue
);

//--------------------------------------------------------------------
// @func BOOL | GetProperty
//
// This function should return TRUE if the fuction succeeded and 
// any value type of a property.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL GetProperty(
	DBPROPID PropertyID, 
	GUID guidPropertySet, 
	IUnknown* pIUnknown, 
	WCHAR** ppValue
);

//--------------------------------------------------------------------
// @func BOOL | GetProperty
//
// This function should return TRUE if the fuction succeeded and 
// any value type of a property.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL GetProperty(
	DBPROPID PropertyID, 
	GUID guidPropertySet, 
	IUnknown* pIUnknown, 
	VARIANT* pVariant
);

//--------------------------------------------------------------------
// @func VOID | FreeProperties
//
// This function should return TRUE if the fuction succeeded and 
// free all the memory in a DBPROPSET structure.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL FreeProperties(
	ULONG*			pcPropSets, 
	DBPROPSET**		prgPropSets
);

//--------------------------------------------------------------------
// @func VOID | FreeProperties
//
// This function should return TRUE if the fuction succeeded and 
// free all the memory in a DBPROPINFOSET structure.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL FreeProperties(
	ULONG* pcPropInfoSets, 
	DBPROPINFOSET** prgPropInfoSets,
	OLECHAR** pDescBuffer = NULL
);

//--------------------------------------------------------------------
// @func VOID | FreeProperties
//
// This function should return TRUE if the fuction succeeded and 
// free all the memory in a DBPROPIDSET structure.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL FreeProperties(
	ULONG*			pcPropIDSets, 
	DBPROPIDSET**	prgPropIDSets
);

//--------------------------------------------------------------------
// @func void | 
//	Miscfunc DuplicatePropertySets |
//	The function duplicates the memory allocated for an array of property sets
//
// Returns BOOL.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL DuplicatePropertySets
(
	ULONG		cPropSets,		// [IN]the number of elements in the array
	DBPROPSET*	rgPropSets,		// [IN] pointer to the array 
	ULONG*		pcPropSets,		// [OUT] pointer to the output size 
	DBPROPSET**	prgPropSets		// [OUT] pointer to the output array 
);


//Set Helpers
DLL_EXPORT HRESULT CreateVariant(VARIANT* pVariant, DBTYPE wType, void* pv);
DLL_EXPORT HRESULT SetProperty(DBPROPID PropertyID, GUID guidPropertySet, ULONG* pcPropSets, DBPROPSET** prgPropSets, DBTYPE wType, ULONG_PTR ulValue, DBPROPOPTIONS dwOptions = DBPROPOPTIONS_REQUIRED, const DBID& colid = DB_NULLID);
DLL_EXPORT HRESULT SetProperty(DBPROPID PropertyID, GUID guidPropertySet, ULONG* pcPropSets, DBPROPSET** prgPropSets, DBTYPE wType, void* pv, DBPROPOPTIONS dwOptions = DBPROPOPTIONS_REQUIRED, const DBID& colid = DB_NULLID);
DLL_EXPORT HRESULT CompactProperties(ULONG* pcPropSets, DBPROPSET** prgPropSets, DBTYPE wType);

//-----------------------------------------------------------------------------
// @func BOOL|
// MiscFunc	CheckRowsetProperty |
// This function should return TRUE if the property in gPropertySet is
// currently supported on the object. Currently for command and rowset 
// objects only. It is user error to pass something other than
// a command or rowset interface pointer and iid.
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL CheckProperty(
	IMalloc *		pIMalloc,	// @parm  [IN] malloc ptr to free memory
	EINTERFACE		eInterface,	// @parm  [IN] indicates object to set property on
	IUnknown *		pIObject,	// @parm  [IN] Rowset ptr or Command ptr
	DBPROPID		prop,		// @parm  [IN] Property 
	DBPROPSTATUS *	stat		// @parm  [IN/OUT] Property Status
);

//-----------------------------------------------------------------------------
// @func HRESULT |
// MiscFunc SetRowsetProperty	|
// Set Property, returns result of either ICommand::QI for 
// ICommandProperty or ICommandProperties::SetProperties.
// Does all the work of build correct property structures.
// Just tell it what property set and property. 
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT SetRowsetProperty(
	IUnknown*	pIUnknown,		// @parm  [IN] Command ptr
	GUID		gPropertySet,	// @parm  [IN] Property Set
	DBPROPID	dbPropertyID,	// @parm  [IN] Property
	BOOL		fSetFlag = TRUE, // @parm  [IN] SET or UNSET
	enum DBPROPOPTIONSENUM dwOptions=DBPROPOPTIONS_REQUIRED, // @parm  [IN] Option
	BOOL		fLogFailure = FALSE // @parm  [IN] Send failure msg to log
);

//-----------------------------------------------------------------------------
// @func HRESULT |
// MiscFunc SetRowsetProperty	|
// Set Property, returns result of either ICommand::QI for 
// ICommandProperty or ICommandProperties::SetProperties.
// Does all the work of build correct property structures.
// Just tell it what property set and property. 
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT SetRowsetProperty(
	IUnknown*	pIUnknown,		// @parm  [IN] Command ptr
	GUID		gPropertySet,	// @parm  [IN] Property Set
	DBPROPID	dbPropertyID,	// @parm  [IN] Property
	LONG_PTR	dwPropValue,	// @parm  [IN] Property value
	enum DBPROPOPTIONSENUM dwOptions=DBPROPOPTIONS_REQUIRED, // @parm  [IN] Option
	BOOL		fLogFailure = FALSE // @parm  [IN] Send failure msg to log
);

//-----------------------------------------------------------------------------
// @func HRESULT |
// MiscFunc PropertyStatus |
// Print Property Status, returns FALSE is status is not OK
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL PropertyStatus(
	DBPROPSTATUS dbPropStatus	// @parm  [IN] Property Status
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc RowsetBindingStatus |
// Print Binding Status, returns FALSE is status is not OK
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL RowsetBindingStatus(
	DBSTATUS dbStatus			// @parm  [IN] Binding Status
);

//-----------------------------------------------------------------------------
// @func HRESULT |
// MiscFunc SetCommandText |
// Set Command Text, returns result of ICommand::QI for ICommandText or 
// ICommandText::SetCommandText or CTable::SOMEFUNCTION. Command text can
// be set a couple of ways: 1) request any StmtKd or 2) pass StmtKd as eSQL 
// and pass in your own text in sqlStmt. 
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT SetCommandText(
	IMalloc	*		pIMalloc,		//@parm [IN] Malloc ptr to free statement
	ICommand *		pICommand,		//@parm [IN] Command ptr
	CTable *		pTable,			//@parm [IN] CTable ptr to use CreateSqlStmt function
	WCHAR *			pTableName2,	//@parm [IN] 2nd CTable ptr if EQUERY requires 2 tables
	STATEMENTKIND	StmtKd,			//@parm [IN] Kind of Statement
	EQUERY			sqlStmt,		//@parm [IN] EQUERY from Private Library
	WCHAR *			pStmt,			//@parm [IN] Text of statement if StmtKd == eSQL
	DBORDINAL *		pcColumns=NULL,	//@parm [IN/OUT] Count of columns.
	DB_LORDINAL **	prgColumns=NULL,//@parm [IN/OUT] Array of column numbers
	CTable *		pTable2 = NULL,	//@parm [IN] Second table object, if needed 
	WCHAR**			ppCmdText = NULL //@parm [OUT] Cmd Text set.
);

//-----------------------------------------------------------------------------
// @func HRESULT |
// MiscFunc PrepareCommand |
// PrepareCommand, returns result of either ICommand::QI for ICommandPrepare 
// or for ICommand::methods (Prepare or/and Unprepare
//
//-----------------------------------------------------------------------------
DLL_EXPORT HRESULT PrepareCommand(
	ICommand *		pICommand,		// @parm  [IN] Command ptr					
	EPREPARE		ePrepare,		// @parm  [IN] Prepare, Unprepare, or Both
	ULONG			cExpectedRuns	// @parm  [IN] Passed to Unprepare only
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc IsFixedLength |
// Returns TRUE is type is fixed length 
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL IsFixedLength(
	DBTYPE wType	// @parm  [IN] type 
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc IsNumericType |
// Returns TRUE is type is numeric
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL IsNumericType(
	DBTYPE wType	// @parm  [IN] type 
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc IsScaleType |
// Returns TRUE is type can contain a scale
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL IsScaleType(
	DBTYPE wType	// @parm  [IN] type 
);


//-----------------------------------------------------------------------------
// @func BYTE |
// MiscFunc bNumericPrecision |
// Returns Precision of the type
//
//-----------------------------------------------------------------------------
DLL_EXPORT BYTE bNumericPrecision(
	DBTYPE wType,	// @parm  [IN] type 
	CCol & rcol		// @parm  [IN] CCol ref
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc AddCharToVarNumericVal |
// Returns TRUE if successfull.
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL AddCharToVarNumericVal(
	WCHAR wLetter,
	DB_VARNUMERIC * pVarNumeric,
	USHORT	cbSize
);

//-----------------------------------------------------------------------------
// @func BOOL |
// MiscFunc AddCharToNumericVal |
// Returns TRUE if successfull.
//
//-----------------------------------------------------------------------------
DLL_EXPORT BOOL AddCharToNumericVal(
	WCHAR wLetter,
	DB_NUMERIC * pnum
);


//--------------------------------------------------------------------
// @func BOOL | 
// MiscFunc CompareStrings |
// Returns BOOL.
//
//--------------------------------------------------------------------
DLL_EXPORT BOOL CompareStrings
(
	const WCHAR * wszString1,	// @parm [IN] Provider string
	const WCHAR * wszString2,	// @parm [IN] Const Static string
	BOOL		  fPrint = FALSE// @parm [IN] Print the Provider string to the screen
);

//--------------------------------------------------------------------
// @func BOOL | Help function for index server
//				setting customer properties in the select list
//
// Returns BOOL.
//
//--------------------------------------------------------------------

DLL_EXPORT BOOL SetIndexServerProperty
(
	ICommandText *pICommandText	// @parm [IN] Poiter to ICommandText
);

//--------------------------------------------------------------------
// Copied from Extralib, they refer to reference count
//--------------------------------------------------------------------
DLL_EXPORT  ULONG GetRefCount(IUnknown* pIUnknown);
DLL_EXPORT  ULONG SetRefCount(IUnknown* pIUnknown, LONG iCount);


//////////////////////////////////////////////////////////////////////////////
// Refcount functions
//
//////////////////////////////////////////////////////////////////////////////
DLL_EXPORT  BOOL VerifyRefCounts(DBREFCOUNT ulActRefCount, DBREFCOUNT ulExpRefCount);
DLL_EXPORT  BOOL VerifyRefCounts(DBCOUNTITEM cRefCounts, DBREFCOUNT* rgActRefCounts, DBREFCOUNT ulExpRefCount);


//--------------------------------------------------------------------
// @func BOOL | 
//	Miscfunc DefaultObjectTesting |
//	The function checks the validity of a interface belonging to an object 
//	(the interface is accessed thru a ptr). It will check: 
//	- an interface which doesn't belong to the object that owns the interface
//	- mandatory interfaces on the object can be got thru QI
//	- can get IUnknown
//	- IID_NULL returns in error E_NOINTERFACE
//	- QI with a NULL ptr results in E_INVALIDARG
//	- reference count works
//
// Returns BOOL.
//
//--------------------------------------------------------------------
DLL_EXPORT  BOOL DefaultObjectTesting
(
	IUnknown*		pIUnkObject,							// [IN]	Interface (Object) to test
	EINTERFACE		eInterface		= UNKNOWN_INTERFACE,	// [IN] Interface Type of pIUnknown
	BOOL			fInitializedDSO = TRUE					// [IN] Flag DSO initialized
);


DLL_EXPORT  BOOL DefaultInterfaceTesting
(
	IUnknown*		pIUnkObject,			// [IN]	Interface (Object) to test
	EINTERFACE		eInterface,				// [IN] Object Type of pIUnknown
	REFIID			riid					// [IN] Interface to test specifically
);

//--------------------------------------------------------------------
//	The following functions perform some basic verification of the 
//  respective interfaces.
//--------------------------------------------------------------------
DLL_EXPORT  BOOL DefTestInterface(IDBInitialize* pIDBI);
DLL_EXPORT  BOOL DefTestInterface(IDBProperties* pIDBP);
DLL_EXPORT  BOOL DefTestInterface(IDBCreateSession* pIDBCR);
DLL_EXPORT  BOOL DefTestInterface(IPersist* pIP);
DLL_EXPORT  BOOL DefTestInterface(IOpenRowset* pIOR);
DLL_EXPORT  BOOL DefTestInterface(IGetDataSource* pIGDS);
DLL_EXPORT  BOOL DefTestInterface(ISessionProperties* pISP);
DLL_EXPORT  BOOL DefTestInterface(IDBCreateCommand* pIDBCR);
DLL_EXPORT  BOOL DefTestInterface(IDBSchemaRowset* pIDBSR);
DLL_EXPORT  BOOL DefTestInterface(IAccessor* pIA);
DLL_EXPORT  BOOL DefTestInterface(IColumnsInfo* pICI);
DLL_EXPORT  BOOL DefTestInterface(IConvertType* pICT);
DLL_EXPORT  BOOL DefTestInterface(IRowset* pIR, BOOL fClean = TRUE);
DLL_EXPORT  BOOL DefTestInterface(IRowsetInfo* pIRI);
DLL_EXPORT  BOOL DefTestInterface(IRow* pIRow);
DLL_EXPORT  BOOL DefTestInterface(ISequentialStream* pISS);


//--------------------------------------------------------------------
// @func void | 
//	Miscfunc ReleaseColumnDesc |
//	The function releases the memory allocated for an array of column descriptors
//
// Returns void.
//
//--------------------------------------------------------------------
DLL_EXPORT void ReleaseColumnDesc
(
	DBCOLUMNDESC*	rgColumnDesc,		// pointer to the array 
	DBORDINAL		cColumnDesc,		// the number of elements in the array
	BOOL			fFreeArray = TRUE	// whether rgColumnDesc should be freed
);


//--------------------------------------------------------------------
// @func void | 
//	Miscfunc DuplicateColumnDesc |
//	The function duplicates the memory allocated for an array of column descriptors
//
// Returns DBCOLUMNDESC*.
//
//--------------------------------------------------------------------
DLL_EXPORT DBCOLUMNDESC *DuplicateColumnDesc
(
	DBCOLUMNDESC*	rgColumnDesc,	// [IN] pointer to the array 
	DBORDINAL		cColumnDesc,		// [IN]the number of elements in the array
	DBCOLUMNDESC**	prgColumnDesc	// [IN/OUT] pointer to the output array 
);


//---------------------------------------------------------------------------
// DuplicateDBID
//
// @func	HRESULT	DuplicateDBID
// Copies y into x. If a problem is encountered, x = GUID_NULL. Client
// is responsible for allocating memory for x before calling this function. 
// Client is responsible for releasing memory for x.
//
// This version should actually duplicate the DBID, i.e. copy eKind, building a similar uName.pwszName, etc
//
// @rdesc Comparision results
//  @flag NOERROR | Data in the two buffers is the same.
//  @flag E_FAIL  | Data in the two buffers is not the same..
//		
//-------------------------------------------------------------------------------------
DLL_EXPORT HRESULT DuplicateDBID(
	const DBID& y,			// @parm || [IN] Orginal
	DBID*	px				// @parm || [OUT] Copy of y
);


//---------------------------------------------------------------------------
// ReleaseDBID
//
// @func	HRESULT	ReleaseDBID
// Releases all the data referred inside the DBID, according to its eKind
// if fDrop == TRUE, the pointer is also released
// @rdesc Comparision results
//  @flag NOERROR | Data in the two buffers is the same.
//  @flag E_FAIL  | Data in the two buffers is not the same..
//		
//-------------------------------------------------------------------------------------
DLL_EXPORT void ReleaseDBID(
	DBID*	pDBID,			// @parm || [IN] DBID to be released
	BOOL	fDrop = TRUE	// whether the ptr should be freed
);




// @cmember Returns the guid that corresponds with the EQUERY. <nl>
DLL_EXPORT BOOL GetSchemaGUID(
	EQUERY		eQuery,			//[IN]  Schema query
	GUID *		pGuid			//[OUT] Schema quid
);
	

//---------------------------------------------------------------------------
// ConvertToWSTR
//
// @func	ConvertToWSTR
// Converts the contents of pStringData to Unicode
// Consumer is responsible for freeing memory returned in pwszOut
//		
//-------------------------------------------------------------------------------------
DLL_EXPORT void ConvertToWSTR(
	void *pStringData,			// @parm || [IN] pointer to string data (can be str, bstr, wstr)
	DBTYPE wSrcType,				// @parm || [IN] type of string data
	WCHAR **pwszOut				// @parm || [OUT] converted to Unicode (provider is responsible for freeing)
);


//--------------------------------------------------------------------
// DecimalDiv10Rem
//
// @func	BOOL DecimalDiv10Rem
// Divides the DECIMAL value indicated by pDecimal by ten
// and returns the result in pDecResult.
// Any remainder is returned in pbRemainder
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL DecimalDiv10Rem(
	DECIMAL * pDecimal,			// @parm || [IN] pointer to a DECIMAL struct
	DECIMAL * pDecResult,		// @parm || [OUT] pointer to DECIMAL to return new value
	BYTE * pbRemainder			// @parm || [OUT] The remainder is returned here.
);

//--------------------------------------------------------------------
// ScaleDecimal
//
// @func	BOOL ScaleDecimal
// Scales a Decimal value down to scale indicated by bScale
// Note: this function can only scale a value down
// Returns TRUE if no truncation occurred.
// Returns FALSE if fractional truncation occurred.
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL ScaleDecimal(
	DECIMAL * pDec,				// @parm || [IN] pointer to a DECIMAL struct
	BYTE bScale						// @parm || [IN] Indicates requested new scale.
);


//--------------------------------------------------------------------
// CompareDecimal
//
// @func	BOOL CompareDecimal
// Compares two DECIMAL values.
// This function performs the necessary scaling and returns TRUE if
// the two DECIMALs are numerically equivalent.
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL CompareDecimal(
	DECIMAL *pDec1,				// @parm || [IN] pointer to a DECIMAL struct
	DECIMAL *pDec2					// @parm || [IN] pointer to a DECIMAL struct
);

//--------------------------------------------------------------------
// NumericDiv10Rem
//
// @func	BOOL NumericDiv10Rem
// Divides the DBTYPE_NUMERIC value indicated by pNum by ten
// and returns the result in pNumResult.
// Any remainder is returned in pbRemainder
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL NumericDiv10Rem(
	DB_NUMERIC * pNumeric,
	DB_NUMERIC * pNumResult,
	BYTE * pbRemainder
);

//--------------------------------------------------------------------
// ScaleNumeric
//
// @func	BOOL ScaleNumeric
// Scales a Numeric value down to scale indicated by bScale
// Returns TRUE if no truncation occurred.
// Returns FALSE if fractional truncation occurred.
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL ScaleNumeric(DB_NUMERIC *pNum, BYTE bScale);

//--------------------------------------------------------------------
// CompareNumeric
//
// @func	BOOL CompareNumeric
// Compares two DBTYPE_NUMERIC values loosely
// This function performs the necessary scaling and returns TRUE if
// the two NUMERICs are numerically equivalent.
// i.e. scale and precision do not have to be equal
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL CompareNumeric(
	DB_NUMERIC *pNum1, 
	DB_NUMERIC *pNum2
);

//--------------------------------------------------------------------
// VarNumericDiv10Rem
//
// @func	BOOL VarNumericDiv10Rem
// Divides the DBTYPE_VARNUMERIC value indicated by pNum by ten
// and returns the result in pNumResult.
// Any remainder is returned in pbRemainder
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL VarNumericDiv10Rem(
	DB_VARNUMERIC * pVarNumeric,
	DB_VARNUMERIC * pVarNumResult,
	ULONG	cbVarNumericValSize,
	BYTE * pbRemainder
);

//--------------------------------------------------------------------
// ScaleVarNumeric
//
// @func	BOOL ScaleVarNumeric
// Scales a Numeric value down to scale indicated by bScale
// Returns TRUE if no truncation occurred.
// Returns FALSE if fractional truncation occurred.
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL ScaleVarNumeric(DB_VARNUMERIC *pVarNum, USHORT cbVarNumericValSize, SBYTE sbScale);

//--------------------------------------------------------------------
// CompareVarNumeric
//
// @func	BOOL CompareVarNumeric
// Compares two DBTYPE_VARNUMERIC values loosely
// This function performs the necessary scaling and returns TRUE if
// the two NUMERICs are numerically equivalent.
// i.e. scale and precision do not have to be equal
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL CompareVarNumeric(
	DB_VARNUMERIC	*pVarNum1, 
	USHORT			cbVarNum1,
	DB_VARNUMERIC	*pVarNum2,
	USHORT			cbVarNum2
);

//--------------------------------------------------------------------
// @func Print the list of properties and the associated error code
//       from the array returned by SetProperties.
//
//  @rdesc None
//--------------------------------------------------------------------
DLL_EXPORT void DumpPropertyErrors
(
    ULONG cPropSets,
	DBPROPSET* rgPropSets,
    ULONG* pcErrors = NULL,
    ULONG* pcInvalidProperties = NULL
);

//--------------------------------------------------------------------
// CompareWCHARData
//
// @func	BOOL CompareWCHARData
// Compares two DBTYPE_WSTR values given the backend type, precision,
// and scale by converting the WSTR values back to the native type
// then comparing as native.  This was necessary to avoid string format
// differences between MakeData and the provider's WCHAR result.  For
// example: MakeData returns "1992-01-01 10:12:20.0", provider returns
// "1992-01-01 10:12:20.000000000" for DBTYPE_DBTIMESTAMP.  These are 
// the same but the strings differ.  
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL CompareWCHARData(
	LPVOID pConsumerData,		// @parm [in]: Pointer to WCHAR version of consumer data
	LPVOID pBackEndData,		// @parm [in]: Pointer to WCHAR version of backend data
	DBTYPE wBackEndType,		// @parm [in]: the DBType of the backend data, no modifiers
	DBLENGTH ulBackEndSize,		// @parm [in]: size in bytes of the backend data, as it might contain embedded nulls.
	DBLENGTH ulConsumerSize		// @parm [in]: size in bytes of the consumer data, as it might contain embedded nulls.
);

//----------------------------------------------------------------------
// FindIntlSetting
//
// @func	DWORD FindIntlSetting
// Fetches registry key that enables international data
//
//-----------------------------------------------------------------------
DLL_EXPORT DWORD FindIntlSetting();


////////////////////////////////////////////////////////////////
// BOOL SetDCLibraryVersion(ULONG ulVersion)
//
// Takes a pointer to IDataConvert (from msdadc)
// and sets the version 
////////////////////////////////////////////////////////////////
DLL_EXPORT BOOL SetDCLibraryVersion(IUnknown *pIUnknown, ULONG ulVersion);


////////////////////////////////////////////////////////////////
// __int64 PrivLib_wtoi64(pwszbigint)
//
// Converts source string to __int64
////////////////////////////////////////////////////////////////
DLL_EXPORT __int64 PrivLib_wtoi64(const WCHAR *pwszbigint);


//--------------------------------------------------------------------
// MapWCHAR
//
// @mfunc MapWCHAR
// returns the uppercase character
//--------------------------------------------------------------------
DLL_EXPORT WCHAR MapWCHAR
(
	 WCHAR wch,
	 DWORD dwMapFlags
);


//--------------------------------------------------------------------
// Maps a string based on user Flags
//
// @mfunc W95LCMapString
//
//--------------------------------------------------------------------
void W95LCMapString
(
	 WCHAR *pwszSource,
	 DWORD dwMapFlags
);


//--------------------------------------------------------------------
// GetLocaleUnderscore
//
// @mfunc GetLocalUnderscore
// Returns a character that is the local equivalent to an underscore
//--------------------------------------------------------------------
DLL_EXPORT WCHAR GetLocalUnderscore();


//--------------------------------------------------------------------
// CheckVariant
//
// @func	BOOL CheckVariant
// Checks a variant's value depending on its type. Presently checks
// only a few types. Checks can be added for more types.
//		
//--------------------------------------------------------------------
DLL_EXPORT BOOL CheckVariant(
	VARIANT* pVar
);


// @func Returns Process ID as a string. Used to create table name.
// Client must IMalloc->Free returned string.
WCHAR * GetPid(void);

// @func Returns RandomNumber. Used to create table name.
// Client must IMalloc->Free returned string.
WCHAR * GetRandomNumber(void);	

//---------------------------------------------------------------------------
// iswcharMappable
// Returns true if the input unicode character can safely be mapped to 
// the active ANSI code page.
//
// @mfunc iswcharMappable
//---------------------------------------------------------------------------
BOOL iswcharMappable(WCHAR wch);

// Initialization code to work with the Conformance provider
BOOL InitializeConfProv(CThisTestModule* pThisTestModule);

// Cleanup table created for Conformance Provider
BOOL ConfProvTerminate();

//--------------------------------------------------------------------
// MakeObjectName
//
// @func	WCHAR * MakeObjectName
// Makes an object name of a given maximum size starting with
// pwszModuleName.
//		
//--------------------------------------------------------------------
DLL_EXPORT WCHAR * MakeObjectName(WCHAR * pwszModuleName, ULONG ulMaxNameLen);



//--------------------------------------------------------------------
// CoCreate
//
// @func	HRESULT CoCreate
// Checks for support of CoCreateInstanceEx, then calls CoCreateInstanceEx
// or CoCreateInstance as appropriate
//		
//--------------------------------------------------------------------
HRESULT CoCreate
(
	REFCLSID rclsid, 
	LPUNKNOWN pUnkOuter, 
	DWORD dwClsContext, 
	REFIID riid, 
	LPVOID * ppv 
);

//--------------------------------------------------------------------
// CoInit
//
// @func HRESULT CoInit
// Calls CoInitializeEx or CoInitialize as appropriate
//		
//--------------------------------------------------------------------
HRESULT CoInit(DWORD dwCoInit);


//--------------------------------------------------------------------
// CoCreateInstanceEx
//
// @func	WINOLEAPI CoCreateInstanceEx
// Extended version of CoCreateInstance found in ole32.dll when DCOM
// is installed.
//--------------------------------------------------------------------
WINOLEAPI CoCreateInstanceEx(REFCLSID, IUnknown *, DWORD, COSERVERINFO *, ULONG, MULTI_QI *);

//--------------------------------------------------------------------
// CoInitializeEx
//
// @func	WINOLEAPI CoInitializeEx
// Extended version of CoInitialize found in ole32.dll when DCOM
// is installed.
//--------------------------------------------------------------------
WINOLEAPI  CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

//---------------------------------------------------------------------------
// FetchRowScopedQuery
//
// This function retrieves the row scoped query that is in the ini file or
// that has been specified in the initialization string.
// This must be a query that can be issued to row scoped commands (i.e. 
// commands whose parent is a Row object). 
// Also, this query must return all the childs rows and can be
// called from any row in the hierarchy.
// This would be equivalent to Monarch SQL's "select * from (DEFAULT_SCOPE)"
//--------------------------------------------------------------------------
DLL_EXPORT WCHAR* FetchRowScopedQuery(EQUERY eQuery);


//--------------------------------------------------------------------
// GetIIDString
//
// @func	WCHAR * GetIIDString
// Helper for converting IID into useful string.  
//--------------------------------------------------------------------
LPCWSTR GetIIDString(REFIID iid);


//--------------------------------------------------------------------
// CompareID
//
// @func	HRESULT CompareID
// Helper for comparing 2 identifiers.  
//--------------------------------------------------------------------
HRESULT CompareID(
	BOOL		*pfResult,			//[out] pointer to result
	WCHAR		*pwszFrontEndID,	// [in]	first identifier
	WCHAR		*pwszBackEndID,		// [in]	second iderntifier
	IUnknown	*pIUnknown = NULL	// [in]	pointer to datasource interface
);

//--------------------------------------------------------------------------
//
// GetQuoteLiteralInfo
//--------------------------------------------------------------------------
HRESULT	GetQuoteLiteralInfo(
	IUnknown	*pIUnknown,			// [in]	initialized DSO interface
	WCHAR		*pwcsQuotePrefix,	// [out] prefix char
	WCHAR		*pwcsQuoteSuffix	// [out] suffix char
);

//--------------------------------------------------------------------------
//
// IScopedOperations_Delete
// wrapper for calling IScopedOperations::Delete and doing  basic general checking
//--------------------------------------------------------------------------
HRESULT IScopedOperations_Delete(
	IScopedOperations	*pIScopedOperations,	// [in] interface to use
	DBCOUNTITEM			cRows,					// [in] number of copy ops	
	WCHAR				**rgpwszURLs,			// [in] source URLs
	DWORD				dwDeleteFlags,			// [in] copy flags
	DBSTATUS			*rgdwStatus				// [out] filled on output
);

//--------------------------------------------------------------------------
//
// wrapper for calling IScopedOperations::Copy and doing  basic general checking
//--------------------------------------------------------------------------
HRESULT IScopedOperations_Copy(
	IScopedOperations	*pIScopedOperations,	// [in] interface to use
	DBCOUNTITEM			cRows,					// [in] number of copy ops	
	WCHAR				**rgpwszSourceURLs,		// [in] source URLs
	WCHAR				**rgpwszDestURLs,		// [in] destination URLs
	DWORD				dwCopyFlags,			// [in] copy flags
	IAuthenticate		*pIAuthenticate,		// [in] authentication interface
	DBSTATUS			*rgdwStatus,			// [out] filled on output
	WCHAR				**rgpwszNewURLs,		// [in/out] new URLs
	WCHAR				**ppStringsBufferURLs	// [out] buffer for URL strings
);

//--------------------------------------------------------------------------
//
// wrapper for calling IScopedOperations::Move and doing  basic general checking
//--------------------------------------------------------------------------
HRESULT IScopedOperations_Move(
	IScopedOperations	*pIScopedOperations,	// [in] interface to use
	DBCOUNTITEM			cRows,					// [in] number of copy ops	
	WCHAR				**rgpwszSourceURLs,		// [in] source URLs
	WCHAR				**rgpwszDestURLs,		// [in] destination URLs
	DWORD				dwMoveFlags,			// [in] operation flags
	IAuthenticate		*pIAuthenticate,		// [in] authentication interface
	DBSTATUS			*rgdwStatus,			// [out] filled on output
	WCHAR				**rgpwszNewURLs,		// [in/out] new URLs
	WCHAR				**ppStringsBufferURLs	// [out] buffer for URL strings
);

//--------------------------------------------------------------------------
//
// wrapper for calling IScopedOperations::OpenRowset and doing  basic general checking
//--------------------------------------------------------------------------
HRESULT IScopedOperations_OpenRowset(
	IScopedOperations	*pIScopedOperations,	// [in] interface to use
	IUnknown			*pUnkOuter,				// [in] controlling IUnknown of the rowset
	DBID				*pTableID,				// [in] URL of the row
	DBID				*pIndexID,				// [in] should be ignored
	REFIID				riid,					// [in] interface to be retrieved
	ULONG				cPropertySets,			// [in] number of elements in prop array
	DBPROPSET			*rgPropertySets,		// [in|out] property array
	IUnknown			**ppRowset				// [out] rowset interface
);

//--------------------------------------------------------------------------
//
// wrapper for calling IScopedOperations::Bind and doing  basic general checking
//--------------------------------------------------------------------------
HRESULT IScopedOperations_Bind(
	IScopedOperations	*pIScopedOperations,	// [in] interface to use
	IUnknown			*pUnkOuter,				// [in] controlling IUnknown
	LPCOLESTR			pwszURL,				// [in] object to be bound
	DBBINDURLFLAG		dwBindFlags,			// [in] flags to be used for binding
	REFGUID				rguid,					// [in] indicates the type of the object being requested
	REFIID				riid,					// [in] requested interface
	IAuthenticate		*pAuthenticate,			// [in] pointer to IAuthenticate interface to be used
	DBIMPLICITSESSION	*pImplSession,			// [in] implicit session	
	DBBINDURLSTATUS		*pdwBindStatus,			// [out] bind status
	IUnknown			**ppUnk					// [out] interface on the bound object
);

//--------------------------------------------------------------------------
//
// Basic checking for operation status
//--------------------------------------------------------------------------
BOOL CheckStatus(
	DBSTATUS		dbStatus,
	DBCOUNTITEM		cValidStatus,
	DBSTATUS		*rgValidStatus
);

//--------------------------------------------------------------------------------
//
// Check whether the return value is a legal one
//--------------------------------------------------------------------------------
BOOL CheckResult(
	HRESULT			hr,
	DBCOUNTITEM		cValidRes,
	HRESULT			*rgValidRes
);




//--------------------------------------------------------------------------------
//
// Release memory associated with a DBCONSTRAINTDESC array
//--------------------------------------------------------------------------------
HRESULT DLL_EXPORT FreeConstraintDesc(
	DBORDINAL			*pcConstraints,			// @parmopt [IN] Count of constraint
	DBCONSTRAINTDESC	**prgConstraints, 		// @parmopt [IN] Array of constraint desc
	BOOL				fFreeOuter = TRUE		// @parmopt [IN] Whether or not to free *prgConstraints
);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// String comparisons used for restrictions, etc
//		This function compares two international WCHAR strings for equality, 
//		using a call to CompareStringA (CompareStringW not supported Win95).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LONG RelCompareString(LPWSTR pwsz1, LPWSTR pwsz2);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// String comparisons used for restrictions, etc
//		This function compares two international CHAR strings for equality, 
//		using a call to CompareStringA (CompareStringW not supported Win95).
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LONG RelCompareString(LPSTR psz1, LPSTR psz2);

//--------------------------------------------------------------------------------
//
// Check for DBID having valid pwszName
//--------------------------------------------------------------------------------
BOOL DBIDHasName(DBID dbid);

//--------------------------------------------------------------------------------
//
// Maps a OLE DB DBTYPE to a VARIANT value
// The mappings are based on ADO
//--------------------------------------------------------------------------------
DLL_EXPORT HRESULT	MapDBTYPE2VARIANT
(
	void *		pvSource,
	DBTYPE		wType,
	ULONG		cbData,
	VARIANT **	ppVariant
);

//--------------------------------------------------------------------------------
//
// Returns TRUE if a DBTYPE maps directly to a VT type
//--------------------------------------------------------------------------------
DLL_EXPORT BOOL IsVariantType
(
	DBTYPE	wType
);

//--------------------------------------------------------------------------------
//
// Returns the ODBC SQL Type corresponding to a variant type
//--------------------------------------------------------------------------------
DLL_EXPORT WCHAR * VariantTypeToSQLType
(
	DBTYPE wType
);


//--------------------------------------------------------------------------------
//
// Returns TRUE if the DBTYPE is compatible with a VARIANT
//--------------------------------------------------------------------------------
DLL_EXPORT BOOL IsVariantCompatible
(
	DBTYPE	wType,
	CCol *	pCol = NULL
);


//-----------------------------------------------------------------------------
// String constants
//-----------------------------------------------------------------------------
const WCHAR	wszInitializeFailed[] = L"IDBInitialize::Initialize failed\n";	 //@const
const WCHAR wszMemoryAllocationError[] = L"Error allocating memory\n";//@const
const WCHAR wszInvalidStatusBinding[] = L"Invalid status returned for column ";//@const
const WCHAR wszForColumn[] = L" returned for column ";//@const
const WCHAR	wszMaxLengthGreaterThanLength[] = L"MaxLength of consumer's buffer is large enough.  No truncation should have occured at column ";//@const
const WCHAR	wszLengthZeroForNULL[] = L"Length for NULL data should be zero at column ";//@const
const WCHAR wszCanNotConvertData[] = L"Can not convert data for column ";//@const
const WCHAR	wszCanNotMakeData[] = L"Can not make data for column ";//@const

const WCHAR wszLengthInconsist[] = L"The length for data is incorrect for column ";
const WCHAR wszReservedUsed[] = L"DBTYPE_RESERVED is used for DBTYPE for column ";
const WCHAR wszTableNotNULL[] = L"Data is not NULL at the backend for column ";
const WCHAR wszTableNULL[] = L"Data is NULL at the backend but the status binding does not indicate for column ";
const WCHAR wszTypeModifierExclusive[] = L"More than one type modifier is used for column ";
const WCHAR	wszInvalidDBTYPE[] = L"Invalid DBType in the binding structure for column ";
const WCHAR	wszInvalidValueBinding[] = L"Invalid value returned for column ";
const WCHAR wszPointerNotNULL[] = L"The pointer should be NULL through ReadByColumns accessors for NULL data for column ";
const WCHAR wszBindStatusForNULL[] = L"Has to bind status for NULL value for column ";
const WCHAR	wszColumnNotUpdatable[] = L"Not updatable for column ";
const WCHAR wszAtBinding[] = L" at binding structure ";
const WCHAR wszInvalidStatus[] = L"Invalid status value ";
const WCHAR wszSafeArrayNotEqual[] = L"The safe arrays are not equal.\n";
const WCHAR	wszVectorNotEqual[] = L"The vectors are not equal.\n";

const WCHAR wszStatusNotEqual[]=L"Status bindings is not equal at column: ";
const WCHAR wszLengthNotEqual[]=L"Length binding is not equal at column: ";
const WCHAR wszValueNotEqual[]=L"Value binding is not equal at column: ";
const WCHAR wszStatusOKOrNULL[]=L"Status for SetData buffer is not OK or NULL at column: ";

const WCHAR  wszPropQIBad[]=L"QI for ICommandPrepare failed";
const WCHAR  wszPrepGood[]=L"Command prepared successfully";
const WCHAR  wszUnprepGood[]=L"Command unprepared successfully";
const WCHAR  wszDefaultError[]=L"Default return";
const WCHAR  wszInsertERROR[]=L"Insert into ";
const WCHAR	 wszSelectERROR[]=L"Select from ";

//-----------------------------------------------------------------------------
//	Constants Defines
//-----------------------------------------------------------------------------
#define DATA_SIZE				2002
#define INVALID_DBTYPE_SIZE		-1
#define INVALID_DBTYPE			(DBTYPE_ARRAY | DBTYPE_BYREF)

#ifndef MAXNUMERICLEN
	#define MAXNUMERICLEN		16
#endif

#endif // _MISCFUNC_H_
