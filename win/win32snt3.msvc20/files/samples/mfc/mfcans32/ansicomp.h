//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       AnsiComp.cpp
//
//  Contents:   ANSI Wrappers for Unicode CompObj Interfaces and APIs.
//
//  Classes:    CEnumStringA - ANSI wrapper object for IEnumString.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


interface IStreamA;
typedef IStreamA * LPSTREAMA;

typedef char FAR* BSTRA;


/*---------------------------------------------------------------------*/
/*                            IEnumStringA                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IEnumStringA

DECLARE_INTERFACE_(IEnumStringA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IEnumString methods ***
	STDMETHOD(Next) (THIS_ ULONG celt,
					   char * * rgelt,
					   ULONG * pceltFetched) PURE;
	STDMETHOD(Skip) (THIS_ ULONG celt) PURE;
	STDMETHOD(Reset) (THIS) PURE;
	STDMETHOD(Clone) (THIS_ IEnumStringA * * ppenm) PURE;
};
typedef IEnumStringA * LPENUMSTRINGA;


/*---------------------------------------------------------------------*/
/*                            IMarshalA                                */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IMarshalA

DECLARE_INTERFACE_(IMarshalA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IMarshal methods ***
	STDMETHOD(GetUnmarshalClass)(THIS_ REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPCLSID pCid) PURE;
	STDMETHOD(GetMarshalSizeMax)(THIS_ REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPDWORD pSize) PURE;
	STDMETHOD(MarshalInterface)(THIS_ LPSTREAMA pStm, REFIID riid,
			LPVOID pv, DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags) PURE;
	STDMETHOD(UnmarshalInterface)(THIS_ LPSTREAMA pStm, REFIID riid,
			LPVOID FAR* ppv) PURE;
	STDMETHOD(ReleaseMarshalData)(THIS_ LPSTREAMA pStm) PURE;
	STDMETHOD(DisconnectObject)(THIS_ DWORD dwReserved) PURE;
};
typedef IMarshalA * LPMARSHALA;



/*---------------------------------------------------------------------*/
/*                            IStdMarshalInfoA                                */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IStdMarshalInfoA

DECLARE_INTERFACE_(IStdMarshalInfoA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IStdMarshalInfo methods ***
	STDMETHOD(GetClassForHandler)(THIS_ DWORD dwMemctx, LPVOID pvMemctx,
			LPCLSID pclsid) PURE;
};
typedef IStdMarshalInfoA * LPSTDMARSHALINFOA;


#ifndef NOERRORINFO
/*---------------------------------------------------------------------*/
/*                     IErrorInfoA/ICreateErrorInfoA                   */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE   IErrorInfoA

DECLARE_INTERFACE_(IErrorInfoA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** IErrorInfo methods ***
	STDMETHOD(GetGUID)(THIS_ GUID *pguid) PURE;
	STDMETHOD(GetSource)(THIS_ BSTRA *pbstrSource) PURE;
	STDMETHOD(GetDescription)(THIS_ BSTRA *pbstrDescription) PURE;
	STDMETHOD(GetHelpFile)(THIS_ BSTRA *pbstrHelpFile) PURE;
	STDMETHOD(GetHelpContext)(THIS_ DWORD *pdwHelpContext) PURE;

};
typedef IErrorInfoA * LPERRORINFOA;


#undef  INTERFACE
#define INTERFACE  ICreateErrorInfoA

DECLARE_INTERFACE_(ICreateErrorInfoA, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG,Release) (THIS) PURE;

	// *** ICreateErrorInfo methods ***
	STDMETHOD(SetGUID)(THIS_ REFGUID rguid) PURE;
	STDMETHOD(SetSource)(THIS_ LPSTR szSource) PURE;
	STDMETHOD(SetDescription)(THIS_ LPSTR szDescription) PURE;
	STDMETHOD(SetHelpFile)(THIS_ LPSTR szHelpFile) PURE;
	STDMETHOD(SetHelpContext)(THIS_ DWORD dwHelpContext) PURE;
};
typedef ICreateErrorInfoA * LPCREATEERRORINFOA;

#endif //!NOERRORINFO


//+--------------------------------------------------------------------------
//
//  Class:      CUnknownA
//
//  Synopsis:   Class definition of IUnknownA
//
//---------------------------------------------------------------------------
class CUnknownA : public CInterface
{
public:
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(unsigned long, AddRef)(void);
	STDMETHOD_(unsigned long, Release)(void);

	CUnknownA(LPUNKNOWN punk, IDINTERFACE idRef = ID_NULL);
	void AddInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk);
	long FreeInterface(IDINTERFACE idInterface);

public:
	UINT        m_idPrimary;
	LPUNKNOWN   m_pInterfaces[ID_SIZE];
};


//+--------------------------------------------------------------------------
//
//  Class:      CEnumStringA
//
//  Synopsis:   Class definition of IEnumStringA
//
//---------------------------------------------------------------------------
class CEnumStringA : CAnsiInterface
{
public:
	// *** IEnumStringA methods ***
	STDMETHOD(Next) (ULONG celt,
					   char * * rgelt,
					   ULONG FAR* pceltFetched);
	STDMETHOD(Skip) (ULONG celt);
	STDMETHOD(Reset) (VOID);
	STDMETHOD(Clone) (IEnumStringA * * ppenm);

	inline CEnumStringA(LPUNKNOWN pUnk, IEnumString * pObj) :
			CAnsiInterface(ID_IEnumString, pUnk, (LPUNKNOWN)pObj) {};

	inline IEnumString * GetWide() const
			{ return (IEnumString *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CClassFactoryA
//
//  Synopsis:   Class definition of IClassFactoryA
//
//---------------------------------------------------------------------------
class CClassFactoryA : CAnsiInterface
{
public:
	// *** IClassFactory methods ***
	STDMETHOD(CreateInstance) (LPUNKNOWN pUnkOuter,
				  REFIID riid,
				  LPVOID FAR* ppvObject);
	STDMETHOD(LockServer) (BOOL fLock);

	inline CClassFactoryA(LPUNKNOWN pUnk, IClassFactory * pObj) :
			CAnsiInterface(ID_IClassFactory, pUnk, (LPUNKNOWN)pObj) {};

	inline IClassFactory * GetWide() const
			{ return (IClassFactory *)m_pObj; };
};

typedef IClassFactory * LPCLASSFACTORYA;


//+--------------------------------------------------------------------------
//
//  Class:      CMarshalA
//
//  Synopsis:   Class definition of IMarshalA
//
//---------------------------------------------------------------------------
class CMarshalA : CAnsiInterface
{
public:
	// *** IMarshal methods ***
	STDMETHOD(GetUnmarshalClass)(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPCLSID pCid);
	STDMETHOD(GetMarshalSizeMax)(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPDWORD pSize);
	STDMETHOD(MarshalInterface)(LPSTREAMA pStm, REFIID riid,
			LPVOID pv, DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags);
	STDMETHOD(UnmarshalInterface)(LPSTREAMA pStm, REFIID riid,
			LPVOID FAR* ppv);
	STDMETHOD(ReleaseMarshalData)(LPSTREAMA pStm);
	STDMETHOD(DisconnectObject)(DWORD dwReserved);

	inline CMarshalA(LPUNKNOWN pUnk, IMarshal * pObj) :
			CAnsiInterface(ID_IMarshal, pUnk, (LPUNKNOWN)pObj) {};

	inline IMarshal * GetWide() const
			{ return (IMarshal *)m_pObj; };
};

//+--------------------------------------------------------------------------
//
//  Class:      CStdMarshalInfoA
//
//  Synopsis:   Class definition of IStdMarshalInfoA
//
//---------------------------------------------------------------------------
class CStdMarshalInfoA : CAnsiInterface
{
public:
	// *** IStdMarshalInfo methods ***
	STDMETHOD(GetClassForHandler)(DWORD dwMemctx, LPVOID pvMemctx,
			LPCLSID pclsid);

	inline CStdMarshalInfoA(LPUNKNOWN pUnk, IStdMarshalInfo * pObj) :
			CAnsiInterface(ID_IStdMarshalInfo, pUnk, (LPUNKNOWN)pObj) {};

	inline IStdMarshalInfo * GetWide() const
			{ return (IStdMarshalInfo *)m_pObj; };
};
