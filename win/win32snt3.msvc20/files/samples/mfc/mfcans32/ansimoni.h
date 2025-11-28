//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansimoni.h
//
//  Contents:   ANSI Wrappers for Unicode Moniker Interfaces and APIs.
//
//  Classes:    CBindCtxA
//              CMonikerA
//              CRunningObjectTableA
//              CEnumMonikerA
//
//  Functions:  BindMonikerA
//              MkParseDisplayNameA
//              MonikerRelativePathToA
//              MonikerCommonPrefixWithA
//              CreateBindCtxA
//              CreateGenericCompositeA
//              GetClassFileA
//              CreateFileMonikerA
//              CreateItemMonikerA
//              CreateAntiMonikerA
//              CreatePointerMonikerA
//              GetRunningObjectTableA
//              IBindCtxAFromW
//              IMonikerAFromW
//              IRunningObjectTableAFromW
//              IEnumMonikerAFromW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


#if defined(__cplusplus)
interface IDataObjectA;
#else
typedef interface IDataObjectA IDataObjectA;
#endif

typedef IDataObjectA * LPDATAOBJECTA;

typedef struct tagOLEVERBA
{
	LONG    lVerb;
	LPSTR   lpszVerbName;
	DWORD   fuFlags;
	DWORD   grfAttribs;
} OLEVERBA, * LPOLEVERBA;


/*---------------------------------------------------------------------*/
/*                          IDropTargetA                               */
/*---------------------------------------------------------------------*/

#undef INTERFACE
#define INTERFACE   IDropTargetA

DECLARE_INTERFACE_(IDropTargetA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IDropTarget methods ***
	STDMETHOD(DragEnter) (THIS_ LPDATAOBJECTA pDataObjA, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect) PURE;
	STDMETHOD(DragOver) (THIS_ DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect) PURE;
	STDMETHOD(DragLeave) (THIS) PURE;
	STDMETHOD(Drop) (THIS_ LPDATAOBJECTA pDataObj, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect) PURE;
};
typedef IDropTargetA * LPDROPTARGETA;


/*---------------------------------------------------------------------*/
/*                        IPersistStorageA                             */
/*---------------------------------------------------------------------*/

#undef INTERFACE
#define INTERFACE   IPersistStorageA

DECLARE_INTERFACE_(IPersistStorageA, IPersist)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IPersist methods ***
	STDMETHOD(GetClassID) (THIS_ LPCLSID lpClassID) PURE;

	// *** IPersistStorage methods ***
	STDMETHOD(IsDirty) (THIS) PURE;
	STDMETHOD(InitNew) (THIS_ LPSTORAGEA pStgA) PURE;
	STDMETHOD(Load) (THIS_ LPSTORAGEA pStgA) PURE;
	STDMETHOD(Save) (THIS_ LPSTORAGEA pStgSaveA, BOOL fSameAsLoad) PURE;
	STDMETHOD(SaveCompleted) (THIS_ LPSTORAGEA pStgNewA) PURE;
	STDMETHOD(HandsOffStorage) (THIS) PURE;
};
typedef IPersistStorageA * LPPERSISTSTORAGEA;


/*---------------------------------------------------------------------*/
/*                          IPersistStreamA                            */
/*---------------------------------------------------------------------*/

#undef INTERFACE
#define INTERFACE   IPersistStreamA

DECLARE_INTERFACE_(IPersistStreamA, IPersist)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IPersist methods ***
	STDMETHOD(GetClassID) (THIS_ LPCLSID lpClassID) PURE;

	// *** IPersistStream methods ***
	STDMETHOD(IsDirty) (THIS) PURE;
	STDMETHOD(Load) (THIS_ LPSTREAMA pStmA) PURE;
	STDMETHOD(Save) (THIS_ LPSTREAMA pStmA,
					BOOL fClearDirty) PURE;
	STDMETHOD(GetSizeMax) (THIS_ ULARGE_INTEGER * pcbSize) PURE;
};
typedef IPersistStreamA * LPPERSISTSTREAMA;


/*---------------------------------------------------------------------*/
/*                          IPersistFileA                              */
/*---------------------------------------------------------------------*/

#undef INTERFACE
#define INTERFACE   IPersistFileA

DECLARE_INTERFACE_(IPersistFileA, IPersist)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IPersist methods ***
	STDMETHOD(GetClassID) (THIS_ LPCLSID lpClassID) PURE;

	// *** IPersistFile methods ***
	STDMETHOD(IsDirty) (THIS) PURE;
	STDMETHOD(Load) (THIS_ LPCSTR lpszFileName, DWORD grfMode) PURE;
	STDMETHOD(Save) (THIS_ LPCSTR lpszFileName, BOOL fRemember) PURE;
	STDMETHOD(SaveCompleted) (THIS_ LPCSTR lpszFileName) PURE;
	STDMETHOD(GetCurFile) (THIS_ LPSTR * lplpszFileName) PURE;
};
typedef IPersistFileA * LPPERSISTFILEA;


#if defined(__cplusplus)
interface IMonikerA;
interface IEnumMonikerA;
interface IRunningObjectTableA;
#else
typedef interface IMonikerA IMonikerA;
typedef interface IEnumMonikerA IEnumMonikerA;
typedef interface IRunningObjectTableA IRunningObjectTableA;
#endif

typedef IMonikerA * LPMONIKERA;
typedef IEnumMonikerA * LPENUMMONIKERA;
typedef IRunningObjectTableA * LPRUNNINGOBJECTTABLEA;


/*---------------------------------------------------------------------*/
/*                          IBindCtxA                                  */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IBindCtxA

DECLARE_INTERFACE_(IBindCtxA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IBindCtx methods ***
	STDMETHOD(RegisterObjectBound) (THIS_ LPUNKNOWN punk) PURE;
	STDMETHOD(RevokeObjectBound) (THIS_ LPUNKNOWN punk) PURE;
	STDMETHOD(ReleaseBoundObjects) (THIS) PURE;

	STDMETHOD(SetBindOptions) (THIS_ LPBIND_OPTS pbindopts) PURE;
	STDMETHOD(GetBindOptions) (THIS_ LPBIND_OPTS pbindopts) PURE;
	STDMETHOD(GetRunningObjectTable) (THIS_ LPRUNNINGOBJECTTABLEA  *
		pprotA) PURE;
	STDMETHOD(RegisterObjectParam) (THIS_ LPSTR lpszKey, LPUNKNOWN punk) PURE;
	STDMETHOD(GetObjectParam) (THIS_ LPSTR lpszKey, LPUNKNOWN * ppunk) PURE;
	STDMETHOD(EnumObjectParam) (THIS_ LPENUMSTRINGA * ppenumA) PURE;
	STDMETHOD(RevokeObjectParam) (THIS_ LPSTR lpszKey) PURE;
};
typedef IBindCtxA * LPBCA;
typedef IBindCtxA * LPBINDCTXA;


/*---------------------------------------------------------------------*/
/*                          IMonikerA                                  */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IMonikerA

DECLARE_INTERFACE_(IMonikerA, IPersistStreamA)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IPersist methods ***
	STDMETHOD(GetClassID) (THIS_ LPCLSID lpClassID) PURE;

	// *** IPersistStream methods ***
	STDMETHOD(IsDirty) (THIS) PURE;
	STDMETHOD(Load) (THIS_ LPSTREAMA pStmA) PURE;
	STDMETHOD(Save) (THIS_ LPSTREAMA pStmA,
					BOOL fClearDirty) PURE;
	STDMETHOD(GetSizeMax) (THIS_ ULARGE_INTEGER * pcbSize) PURE;

	// *** IMoniker methods ***
	STDMETHOD(BindToObject) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeftA,
		REFIID riidResult, LPVOID * ppvResult) PURE;
	STDMETHOD(BindToStorage) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeftA,
		REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD(Reduce) (THIS_ LPBCA pbca, DWORD dwReduceHow, LPMONIKERA *
		ppmkToLeft, LPMONIKERA * ppmkReducedA) PURE;
	STDMETHOD(ComposeWith) (THIS_ LPMONIKERA pmkRightA, BOOL fOnlyIfNotGeneric,
		LPMONIKERA * ppmkCompositeA) PURE;
	STDMETHOD(Enum) (THIS_ BOOL fForward, LPENUMMONIKERA * ppenumMonikerA)
		PURE;
	STDMETHOD(IsEqual) (THIS_ LPMONIKERA pmkOtherMonikerA) PURE;
	STDMETHOD(Hash) (THIS_ LPDWORD pdwHash) PURE;
	STDMETHOD(IsRunning) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeft, LPMONIKERA
		pmkNewlyRunning) PURE;
	STDMETHOD(GetTimeOfLastChange) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeftA,
		FILETIME * pfiletime) PURE;
	STDMETHOD(Inverse) (THIS_ LPMONIKERA * ppmkA) PURE;
	STDMETHOD(CommonPrefixWith) (THIS_ LPMONIKERA pmkOther, LPMONIKERA *
		ppmkPrefix) PURE;
	STDMETHOD(RelativePathTo) (THIS_ LPMONIKERA pmkOther, LPMONIKERA *
		ppmkRelPath) PURE;
	STDMETHOD(GetDisplayName) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeftA,
		LPSTR * lplpszDisplayName) PURE;
	STDMETHOD(ParseDisplayName) (THIS_ LPBCA pbca, LPMONIKERA pmkToLeftA,
		LPSTR lpszDisplayName, ULONG * pchEaten,
		LPMONIKERA * ppmkOutA) PURE;
	STDMETHOD(IsSystemMoniker) (THIS_ LPDWORD pdwMksys) PURE;
};


/*---------------------------------------------------------------------*/
/*                        IRunningObjectTableA                         */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IRunningObjectTableA

DECLARE_INTERFACE_(IRunningObjectTableA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IRunningObjectTable methods ***
	STDMETHOD(Register) (THIS_ DWORD grfFlags, LPUNKNOWN punkObject,
		LPMONIKERA pmkObjectNameA, DWORD * pdwRegister) PURE;
	STDMETHOD(Revoke) (THIS_ DWORD dwRegister) PURE;
	STDMETHOD(IsRunning) (THIS_ LPMONIKERA pmkObjectNameA) PURE;
	STDMETHOD(GetObject) (THIS_ LPMONIKERA pmkObjectNameA,
		LPUNKNOWN * ppunkObject) PURE;
	STDMETHOD(NoteChangeTime) (THIS_ DWORD dwRegister, FILETIME * pfiletime) PURE;
	STDMETHOD(GetTimeOfLastChange) (THIS_ LPMONIKERA pmkObjectNameA, FILETIME * pfiletime) PURE;
	STDMETHOD(EnumRunning) (THIS_ LPENUMMONIKERA * ppenumMonikerA ) PURE;
};
typedef IRunningObjectTableA * LPRUNNINGOBJECTTABLEA;


/*---------------------------------------------------------------------*/
/*                          IEnumMonikerA                              */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumMonikerA

DECLARE_INTERFACE_(IEnumMonikerA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IEnumOleDataObject methods ***
	STDMETHOD(Next) (THIS_ ULONG celt, LPMONIKERA * rgeltA, ULONG * pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumMonikerA * * ppenmA) PURE;
};
typedef IEnumMonikerA * LPENUMMONIKERA;



//
//  Forward declarations
//
class CBindCtxA;
class CMonikerA;
class CRunningObjectTableA;
class CEnumMonikerA;



//+--------------------------------------------------------------------------
//
//  Class:      CBindCtxA
//
//  Synopsis:   Class definition of IBindCtxA
//
//---------------------------------------------------------------------------
class CBindCtxA : CAnsiInterface
{
public:
	// *** IBindCtx methods ***
	STDMETHOD(RegisterObjectBound) (LPUNKNOWN punk);
	STDMETHOD(RevokeObjectBound) (LPUNKNOWN punk);
	STDMETHOD(ReleaseBoundObjects) (VOID);

	STDMETHOD(SetBindOptions) (LPBIND_OPTS pbindopts);
	STDMETHOD(GetBindOptions) (LPBIND_OPTS pbindopts);
	STDMETHOD(GetRunningObjectTable) (LPRUNNINGOBJECTTABLEA  *
		pprotA);
	STDMETHOD(RegisterObjectParam) (LPSTR lpszKey, LPUNKNOWN punk);
	STDMETHOD(GetObjectParam) (LPSTR lpszKey, LPUNKNOWN * ppunk);
	STDMETHOD(EnumObjectParam) (LPENUMSTRINGA * ppenumA);
	STDMETHOD(RevokeObjectParam) (LPSTR lpszKey);

	inline CBindCtxA(LPUNKNOWN pUnk, IBindCtx * pWide) :
			CAnsiInterface(ID_IBindCtx, pUnk, (LPUNKNOWN)pWide) {};

	inline IBindCtx * GetWide() const
			{ return (IBindCtx *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CMonikerA
//
//  Synopsis:   Class definition of IMonikerA
//
//---------------------------------------------------------------------------
class CMonikerA : CAnsiInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistStream methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(Load) (LPSTREAMA pStmA);
	STDMETHOD(Save) (LPSTREAMA pStmA,
					BOOL fClearDirty);
	STDMETHOD(GetSizeMax) (ULARGE_INTEGER * pcbSize);

	// *** IMoniker methods ***
	STDMETHOD(BindToObject) (LPBCA pbca, LPMONIKERA pmkToLeftA,
		REFIID riidResult, LPVOID * ppvResult);
	STDMETHOD(BindToStorage) (LPBCA pbca, LPMONIKERA pmkToLeftA,
		REFIID riid, LPVOID * ppvObj);
	STDMETHOD(Reduce) (LPBCA pbca, DWORD dwReduceHowFar, LPMONIKERA *
		ppmkToLeft, LPMONIKERA * ppmkReducedA);
	STDMETHOD(ComposeWith) (LPMONIKERA pmkRightA, BOOL fOnlyIfNotGeneric,
		LPMONIKERA * ppmkCompositeA);
	STDMETHOD(Enum) (BOOL fForward, LPENUMMONIKERA * ppenumMonikerA);
	STDMETHOD(IsEqual) (LPMONIKERA pmkOtherMonikerA);
	STDMETHOD(Hash) (LPDWORD pdwHash);
	STDMETHOD(IsRunning) (LPBCA pbca, LPMONIKERA pmkToLeft, LPMONIKERA
		pmkNewlyRunning);
	STDMETHOD(GetTimeOfLastChange) (LPBCA pbca, LPMONIKERA pmkToLeftA,
		FILETIME * pfiletime);
	STDMETHOD(Inverse) (LPMONIKERA * ppmkA);
	STDMETHOD(CommonPrefixWith) (LPMONIKERA pmkOther, LPMONIKERA *
		ppmkPrefix);
	STDMETHOD(RelativePathTo) (LPMONIKERA pmkOther, LPMONIKERA *
		ppmkRelPath);
	STDMETHOD(GetDisplayName) (LPBCA pbca, LPMONIKERA pmkToLeftA,
		LPSTR * lplpszDisplayName);
	STDMETHOD(ParseDisplayName) (LPBCA pbca, LPMONIKERA pmkToLeftA,
		LPSTR lpszDisplayName, ULONG * pchEaten,
		LPMONIKERA * ppmkOutA);
	STDMETHOD(IsSystemMoniker) (LPDWORD pdwMksys);

	inline CMonikerA(LPUNKNOWN pUnk, IMoniker * pWide) :
			CAnsiInterface(ID_IMoniker, pUnk, (LPUNKNOWN)pWide) {};

	inline IMoniker * GetWide() const
			{ return (IMoniker *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CRunningObjectTableA
//
//  Synopsis:   Class definition of IRunningObjectTableA
//
//---------------------------------------------------------------------------
class CRunningObjectTableA : CAnsiInterface
{
public:
	// *** IRunningObjectTable methods ***
	STDMETHOD(Register) (DWORD grfFlags, LPUNKNOWN punkObject,
		LPMONIKERA pmkObjectNameA, DWORD * pdwRegister);
	STDMETHOD(Revoke) (DWORD dwRegister);
	STDMETHOD(IsRunning) (LPMONIKERA pmkObjectNameA);
	STDMETHOD(GetObject) (LPMONIKERA pmkObjectNameA,
		LPUNKNOWN * ppunkObject);
	STDMETHOD(NoteChangeTime) (DWORD dwRegister, FILETIME * pfiletime);
	STDMETHOD(GetTimeOfLastChange) (LPMONIKERA pmkObjectNameA, FILETIME * pfiletime);
	STDMETHOD(EnumRunning) (LPENUMMONIKERA * ppenumMonikerA );

	inline CRunningObjectTableA(LPUNKNOWN pUnk, IRunningObjectTable * pWide) :
			CAnsiInterface(ID_IRunningObjectTable, pUnk, (LPUNKNOWN)pWide) {};

	inline IRunningObjectTable * GetWide() const
			{ return (IRunningObjectTable *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumMonikerA
//
//  Synopsis:   Class definition of IEnumMonikerA
//
//---------------------------------------------------------------------------
class CEnumMonikerA : CAnsiInterface
{
public:
	// *** IEnumOleDataObject methods ***
	STDMETHOD(Next) (ULONG celt, LPMONIKERA * rgeltA, ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumMonikerA * * ppenmA);

	inline CEnumMonikerA(LPUNKNOWN pUnk, IEnumMoniker * pWide) :
			CAnsiInterface(ID_IEnumMoniker, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumMoniker * GetWide() const
			{ return (IEnumMoniker *)m_pObj; };
};
