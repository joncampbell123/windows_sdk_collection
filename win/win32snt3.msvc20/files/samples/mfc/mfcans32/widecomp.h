//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:   widecomp.h
//
//  Contents:   Unicode wrappers for ANSI CompObj Interfaces.
//
//  Classes:    CEnumStringW - ANSI wrapper object for IEnumString.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//+--------------------------------------------------------------------------
//
//  Class:      CUnknownW
//
//  Synopsis:   Class definition of IUnknownW
//
//---------------------------------------------------------------------------
class CUnknownW : public CInterface
{
public:
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(unsigned long, AddRef)(void);
	STDMETHOD_(unsigned long, Release)(void);

	CUnknownW(LPUNKNOWN punk, IDINTERFACE idRef = ID_NULL);
	void AddInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk);
	long FreeInterface(IDINTERFACE idInterface);

public:
	UINT        m_idPrimary;
	LPUNKNOWN   m_pInterfaces[ID_SIZE];
};


//+--------------------------------------------------------------------------
//
//  Class:      CEnumStringW
//
//  Synopsis:   Class definition of IEnumStringA
//
//---------------------------------------------------------------------------
class CEnumStringW : CWideInterface
{
public:
	// *** IEnumStringA methods ***
	STDMETHOD(Next) (ULONG celt,
					   LPOLESTR * rgelt,
					   ULONG FAR* pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumString * * ppenm);

	inline CEnumStringW(LPUNKNOWN pUnk, IEnumStringA * pANSI) :
			CWideInterface(ID_IEnumString, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumStringA * GetANSI() const
		{ return (IEnumStringA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CClassFactoryW
//
//  Synopsis:   Class definition of IClassFactoryW
//
//---------------------------------------------------------------------------
class CClassFactoryW : CWideInterface
{
public:
	// *** IClassFactory methods ***
	STDMETHOD(CreateInstance) (LPUNKNOWN pUnkOuter,
				  REFIID riid,
				  LPVOID FAR* ppvObject);
	STDMETHOD(LockServer) (BOOL fLock);

	inline CClassFactoryW(LPUNKNOWN pUnk, IClassFactory * pObj) :
			CWideInterface(ID_IClassFactory, pUnk, (LPUNKNOWN)pObj) {};

	inline IClassFactory * GetANSI() const
			{ return (IClassFactory *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CMarshalW
//
//  Synopsis:   Class definition of IMarshalW
//
//---------------------------------------------------------------------------
class CMarshalW : CWideInterface
{
public:
	// *** IMarshal methods ***
	STDMETHOD(GetUnmarshalClass)(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPCLSID pCid);
	STDMETHOD(GetMarshalSizeMax)(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPDWORD pSize);
	STDMETHOD(MarshalInterface)(LPSTREAM pStm, REFIID riid,
			LPVOID pv, DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags);
	STDMETHOD(UnmarshalInterface)(LPSTREAM pStm, REFIID riid,
			LPVOID FAR* ppv);
	STDMETHOD(ReleaseMarshalData)(LPSTREAM pStm);
	STDMETHOD(DisconnectObject)(DWORD dwReserved);

	inline CMarshalW(LPUNKNOWN pUnk, IMarshalA * pANSI) :
			CWideInterface(ID_IMarshal, pUnk, (LPUNKNOWN)pANSI) {};

	inline IMarshalA * GetANSI() const
		{ return (IMarshalA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CStdMarshalInfoW
//
//  Synopsis:   Class definition of IStdMarshalInfoW
//
//---------------------------------------------------------------------------
class CStdMarshalInfoW : CWideInterface
{
public:
	// *** IStdMarshalInfo methods ***
	STDMETHOD(GetClassForHandler)(DWORD dwMemctx, LPVOID pvMemctx,
			LPCLSID pclsid);

	inline CStdMarshalInfoW(LPUNKNOWN pUnk, IStdMarshalInfoA * pANSI) :
			CWideInterface(ID_IStdMarshalInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline IStdMarshalInfoA * GetANSI() const
		{ return (IStdMarshalInfoA *)m_pObj; };
};
