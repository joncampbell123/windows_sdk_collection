//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widemoni.h
//
//  Contents:   Unicode Wrappers for ANSI Moniker Interfaces.
//
//  Classes:    CBindCtxW
//              CMonikerW
//              CRunningObjectTableW
//              CEnumMonikerW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//
//  Forward declarations
//
class CBindCtxW;
class CMonikerW;
class CRunningObjectTableW;
class CEnumMonikerW;



//+--------------------------------------------------------------------------
//
//  Class:      CBindCtxW
//
//  Synopsis:   Class definition of IBindCtxW
//
//---------------------------------------------------------------------------
class CBindCtxW : CWideInterface
{
public:
	// *** IBindCtx methods ***
	STDMETHOD(RegisterObjectBound) (LPUNKNOWN punk);
	STDMETHOD(RevokeObjectBound) (LPUNKNOWN punk);
	STDMETHOD(ReleaseBoundObjects) (VOID);

	STDMETHOD(SetBindOptions) (LPBIND_OPTS pbindopts);
	STDMETHOD(GetBindOptions) (LPBIND_OPTS pbindopts);
	STDMETHOD(GetRunningObjectTable) (LPRUNNINGOBJECTTABLE * pprot);
	STDMETHOD(RegisterObjectParam) (LPOLESTR lpszKey, LPUNKNOWN punk);
	STDMETHOD(GetObjectParam) (LPOLESTR lpszKey, LPUNKNOWN * ppunk);
	STDMETHOD(EnumObjectParam) (LPENUMSTRING * ppenum);
	STDMETHOD(RevokeObjectParam) (LPOLESTR lpszKey);

	inline CBindCtxW(LPUNKNOWN pUnk, IBindCtxA * pANSI) :
			CWideInterface(ID_IBindCtx, pUnk, (LPUNKNOWN)pANSI) {};

	inline IBindCtxA * GetANSI() const
		{ return (IBindCtxA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CMonikerW
//
//  Synopsis:   Class definition of IMonikerW
//
//---------------------------------------------------------------------------
class CMonikerW : CWideInterface
{
public:
	// *** IPersist methods ***
	STDMETHOD(GetClassID) (LPCLSID lpClassID);

	// *** IPersistStream methods ***
	STDMETHOD(IsDirty) (VOID);
	STDMETHOD(Load) (LPSTREAM pStm);
	STDMETHOD(Save) (LPSTREAM pStm,
					BOOL fClearDirty);
	STDMETHOD(GetSizeMax) (ULARGE_INTEGER * pcbSize);

	// *** IMoniker methods ***
	STDMETHOD(BindToObject) (LPBC pbc, LPMONIKER pmkToLeft,
		REFIID riidResult, LPVOID * ppvResult);
	STDMETHOD(BindToStorage) (LPBC pbc, LPMONIKER pmkToLeft,
		REFIID riid, LPVOID * ppvObj);
	STDMETHOD(Reduce) (LPBC pbc, DWORD dwReduceHowFar, LPMONIKER *
		ppmkToLeft, LPMONIKER * ppmkReduced);
	STDMETHOD(ComposeWith) (LPMONIKER pmkRight, BOOL fOnlyIfNotGeneric,
		LPMONIKER * ppmkComposite);
	STDMETHOD(Enum) (BOOL fForward, LPENUMMONIKER * ppenumMoniker);
	STDMETHOD(IsEqual) (LPMONIKER pmkOtherMoniker);
	STDMETHOD(Hash) (LPDWORD pdwHash);
	STDMETHOD(IsRunning) (LPBC pbc, LPMONIKER pmkToLeft, LPMONIKER
		pmkNewlyRunning);
	STDMETHOD(GetTimeOfLastChange) (LPBC pbc, LPMONIKER pmkToLeft,
		FILETIME * pfiletime);
	STDMETHOD(Inverse) (LPMONIKER * ppmk);
	STDMETHOD(CommonPrefixWith) (LPMONIKER pmkOther, LPMONIKER *
		ppmkPrefix);
	STDMETHOD(RelativePathTo) (LPMONIKER pmkOther, LPMONIKER *
		ppmkRelPath);
	STDMETHOD(GetDisplayName) (LPBC pbc, LPMONIKER pmkToLeft,
		LPOLESTR * lplpszDisplayName);
	STDMETHOD(ParseDisplayName) (LPBC pbc, LPMONIKER pmkToLeft,
		LPOLESTR lpszDisplayName, ULONG * pchEaten,
		LPMONIKER * ppmkOut);
	STDMETHOD(IsSystemMoniker) (LPDWORD pdwMksys);

	inline CMonikerW(LPUNKNOWN pUnk, IMonikerA * pANSI) :
			CWideInterface(ID_IMoniker, pUnk, (LPUNKNOWN)pANSI) {};

	inline IMonikerA * GetANSI() const
		{ return (IMonikerA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CRunningObjectTableW
//
//  Synopsis:   Class definition of IRunningObjectTableW
//
//---------------------------------------------------------------------------
class CRunningObjectTableW : CWideInterface
{
public:
	// *** IRunningObjectTable methods ***
	STDMETHOD(Register) (DWORD grfFlags, LPUNKNOWN punkObject,
		LPMONIKER pmkObjectName, DWORD * pdwRegister);
	STDMETHOD(Revoke) (DWORD dwRegister);
	STDMETHOD(IsRunning) (LPMONIKER pmkObjectName);
	STDMETHOD(GetObject) (LPMONIKER pmkObjectName,
		LPUNKNOWN * ppunkObject);
	STDMETHOD(NoteChangeTime) (DWORD dwRegister, FILETIME * pfiletime);
	STDMETHOD(GetTimeOfLastChange) (LPMONIKER pmkObjectName, FILETIME * pfiletime);
	STDMETHOD(EnumRunning) (LPENUMMONIKER * ppenumMoniker );

	inline CRunningObjectTableW(LPUNKNOWN pUnk, IRunningObjectTableA * pANSI) :
			CWideInterface(ID_IRunningObjectTable, pUnk, (LPUNKNOWN)pANSI) {};

	inline IRunningObjectTableA * GetANSI() const
		{ return (IRunningObjectTableA *)m_pObj; };
};



//+--------------------------------------------------------------------------
//
//  Class:      CEnumMonikerW
//
//  Synopsis:   Class definition of IEnumMonikerW
//
//---------------------------------------------------------------------------
class CEnumMonikerW : CWideInterface
{
public:
	// *** IEnumOleDataObject methods ***
	STDMETHOD(Next) (ULONG celt, LPMONIKER * rgelt, ULONG * pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumMoniker * * ppenm);

	inline CEnumMonikerW(LPUNKNOWN pUnk, IEnumMonikerA * pANSI) :
			CWideInterface(ID_IEnumMoniker, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumMonikerA * GetANSI() const
		{ return (IEnumMonikerA *)m_pObj; };
};
