//--------------------------------------------------------------------
// Microsoft OLE DB Test
//
// Copyright 1995-1999 Microsoft Corporation.  
//
// @doc 
//
// @module Error Header file Module | This module contains definition information
// for OLE DB strings
//
// @comm
// Special Notes...:	(OPTIONAL NOTES FOR SPECIAL CIRCUMSTANCES)
//
// <nl><nl>
// Revision History:<nl>
//	
//
// @head3 OLE DB strings Elements|
//
//---------------------------------------------------------------------------

#ifndef _STRINGS_H_
#define _STRINGS_H_


/////////////////////////////////////////////////////////////////////////////
// CString
//
/////////////////////////////////////////////////////////////////////////////
class DLL_EXPORT CString
{
public:

//Constructors
	CString();
	CString(LPCSTR lpsz);
	virtual ~CString();

	//methods
	operator LPCTSTR() const;           // as a C string
	const CString& operator=(LPCSTR lpsz);
	BOOL operator==(LPCSTR lpsz);
	BOOL operator==(CString& rCString);

	// concatentation operator
	const CString& operator + (LPCSTR lpsz);

//Implementation
protected:
	CHAR* m_pszString;
};




/////////////////////////////////////////////////////////////////////////////
// CWString
//
/////////////////////////////////////////////////////////////////////////////
class DLL_EXPORT CWString
{
public:

//Constructors
	CWString();
	CWString(LPCWSTR lpwsz);
	CWString(CWString &String);
	virtual ~CWString();

	//methods
	operator LPCWSTR() const;           // as a C string
	const CWString& operator=(LPCWSTR lpwsz);
	const CWString& operator=(CWString wszString);
	BOOL operator==(LPCWSTR lpsz);
	BOOL operator==(CWString& rCWString);

	// concatentation operator
	const CWString& operator + (LPCWSTR lpwsz);

//Implementation
protected:
	WCHAR* m_pwszString;
};




///////////////////////////////////////////////////////////////
// Defines
//
///////////////////////////////////////////////////////////////
#define VALUE_WCHAR(value) value, L#value
#define VALUE_CHAR(value) value, #value

typedef struct _WIDENAMEMAP
{
	LONG		lItem;		// Item
	WCHAR*		pwszName;	// Name
} WIDENAMEMAP;


typedef struct _NAMEMAP
{
	LONG		lItem;		// Item
	CHAR*		pszName;	// Name
} NAMEMAP;


typedef struct _WIDEGUIDMAP
{
	const GUID*		pGuid;		// Guid
	WCHAR*			pwszName;	// Name
} WIDEGUIDMAP;


typedef struct _GUIDMAP
{
	const GUID*		pGuid;		// Guid
	CHAR*			pszName;	// Name
} GUIDMAP;


typedef struct _WIDEDBIDMAP
{
	const DBID*		pDBID;		// DBID
	WCHAR*			pqszName;	// Name
} WIDEDBIDMAP;


typedef struct _INITPROPINFOMAP
{
	DBPROPID	dwPropertyID;
	VARTYPE		vt;
	WCHAR*		pwszName;
	WCHAR*		pwszDesc;
} INITPROPINFOMAP;
	

extern const ULONG g_cInitPropInfoMap;
extern const INITPROPINFOMAP g_rgInitPropInfoMap[];

extern const ULONG g_cObjTypeMap;
extern const WIDEGUIDMAP g_rgObjTypeMap[];

extern const ULONG g_cRowColMap;
extern const WIDEDBIDMAP g_rgRowColMap[];


////////////////////////////////////////////////////////////////////////////
// Extened Error Info
//
////////////////////////////////////////////////////////////////////////////
DLL_EXPORT WCHAR*	GetMapName(REFGUID guid, ULONG cGuidMap, const WIDEGUIDMAP* rgGuidMap);
DLL_EXPORT WCHAR*	GetMapName(LONG lItem, ULONG cNameMap, const WIDENAMEMAP* rgNameMap);
DLL_EXPORT LONG		GetMapName(WCHAR* pwsz, ULONG cNameMap, const WIDENAMEMAP* rgNameMap);

DLL_EXPORT CHAR*	GetMapName(REFGUID guid, ULONG cGuidMap, const GUIDMAP* rgGuidMap);
DLL_EXPORT CHAR*	GetMapName(LONG lItem, ULONG cNameMap, const NAMEMAP* rgNameMap);
DLL_EXPORT LONG		GetMapName(CHAR* psz, ULONG cNameMap, const NAMEMAP* rgNameMap);

DLL_EXPORT WCHAR*	GetErrorName(HRESULT hr);
DLL_EXPORT WCHAR*	GetPropSetName(REFGUID guidPropertySet);
DLL_EXPORT WCHAR*	GetPropertyName(DBPROPID dwPropertyID, REFGUID guidPropertySet);
DLL_EXPORT WCHAR*	GetStaticPropDesc(DBPROPID dwPropertyID, REFGUID guidPropertySet = DBPROPSET_DBINIT);

DLL_EXPORT WCHAR*	GetStatusName(DBSTATUS dwStatus);
DLL_EXPORT WCHAR*	GetPropStatusName(DBSTATUS dwStatus);
DLL_EXPORT WCHAR*	GetRowStatusName(DBROWSTATUS dwRowStatus);
DLL_EXPORT WCHAR*	GetBindStatusName(DBBINDSTATUS dwBindStatus);
DLL_EXPORT WCHAR*	GetInterfaceName(REFIID riid);
DLL_EXPORT DBSTATUS GetStatusValue(WCHAR* pwszName);

DLL_EXPORT WCHAR*	GetDBTypeName(DBTYPE wType);
DLL_EXPORT DBTYPE	GetDBType(WCHAR* pwszName);

DLL_EXPORT EQUERY	GetSQLTokenValue(WCHAR* pwszName);
DLL_EXPORT ULONG	GetSQLTokenMap(NAMEMAP** prgNameMap);

DLL_EXPORT WCHAR* GetObjectTypeName(REFGUID rguid);
DLL_EXPORT WCHAR* GetBindURLStatusName(DBBINDURLSTATUS dwBindStatus);

#endif	//_STRINGS_H_
