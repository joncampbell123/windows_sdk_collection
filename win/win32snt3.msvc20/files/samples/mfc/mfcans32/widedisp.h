//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widedisp.h
//
//  Contents:   Unicode Wrappers for ANSI Dispatch Interfaces.
//
//  Classes:    CTypeLibW        - Unicode wrapper object for ITypeLibA.
//              CTypeInfoW       - Unicode wrapper object for TypeInfoA.
//              CTypeCompW       - Unicode wrapper object for TypeCompA.
//              CCreateTypeLibW  - Unicode wrapper object for CreateTypeLibA.
//              CCreateTypeInfoW - Unicode wrapper object for CreateTypeInfoA.
//              CEnumVARIANTW    - Unicode wrapper object for EnumVARIANTA.
//              CDispatchW       - Unicode wrapper object for DispatchA.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------


//
//  Forward declarations
//
class CTypeLibW;
class CTypeInfoW;
class CTypeCompW;
class CCreateTypeLibW;
class CCreateTypeInfoW;
class CEnumVARIANTW;
class CDispatchW;


//+--------------------------------------------------------------------------
//
//  Class:      CTypeLibW
//
//  Synopsis:   Class definition of ITypeLibW
//
//---------------------------------------------------------------------------
class CTypeLibW : CWideInterface
{
public:
	// *** ITypeLibW methods ***
	STDMETHOD_(unsigned int,GetTypeInfoCount)(VOID);

	STDMETHOD(GetTypeInfo)(
	  unsigned int index, ITypeInfo FAR* FAR* pptinfo);

	STDMETHOD(GetTypeInfoType)(
	  unsigned int index, TYPEKIND FAR* ptypekind);

	STDMETHOD(GetTypeInfoOfGuid)(
	  REFGUID guid, ITypeInfo FAR* FAR* pptinfo);

	STDMETHOD(GetLibAttr)(
	  TLIBATTR FAR* FAR* pptlibattr);

	STDMETHOD(GetTypeComp)(
	  ITypeComp FAR* FAR* pptcomp);

	STDMETHOD(GetDocumentation)(
	  int index,
	  BSTR FAR* pbstrName,
	  BSTR FAR* pbstrDocString,
	  unsigned long FAR* pdwHelpContext,
	  BSTR FAR* pbstrHelpFile);

	STDMETHOD(IsName)(
	  OLECHAR FAR* szNameBuf,
	  unsigned long lHashVal,
	  int FAR* lpfName);

	STDMETHOD(FindName)(
	  OLECHAR FAR* szNameBuf,
	  unsigned long lHashVal,
	  ITypeInfo FAR* FAR* rgptinfo,
	  MEMBERID FAR* rgmemid,
	  unsigned short FAR* pcFound);

	STDMETHOD_(void, ReleaseTLibAttr)( TLIBATTR FAR* ptlibattr);

	inline CTypeLibW(LPUNKNOWN pUnk, ITypeLibA * pANSI) :
			CWideInterface(ID_ITypeLib, pUnk, (LPUNKNOWN)pANSI) {};

	inline ITypeLibA * GetANSI() const
		{ return (ITypeLibA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CTypeInfoW
//
//  Synopsis:   Class definition of ITypeInfoW
//
//---------------------------------------------------------------------------
class CTypeInfoW : CWideInterface
{
public:
	// *** ITypeInfoW methods ***
	STDMETHOD(GetTypeAttr)( TYPEATTR FAR* FAR* pptypeattr);

	STDMETHOD(GetTypeComp)( ITypeComp FAR* FAR* pptcomp);

	STDMETHOD(GetFuncDesc)(
	  unsigned int index, FUNCDESC FAR* FAR* ppfuncdesc);

	STDMETHOD(GetVarDesc)(
	  unsigned int index, VARDESC FAR* FAR* ppvardesc);

	STDMETHOD(GetNames)(
	  MEMBERID memid,
	  BSTR FAR* rgbstrNames,
	  unsigned int cMaxNames,
	  unsigned int FAR* pcNames);

	STDMETHOD(GetRefTypeOfImplType)(
	  unsigned int index, HREFTYPE FAR* phreftype);

	STDMETHOD(GetImplTypeFlags)(
	  unsigned int index, int FAR* pimpltypeflags);

	STDMETHOD(GetIDsOfNames)(
	  OLECHAR FAR* FAR* rgszNames,
	  unsigned int cNames,
	  MEMBERID FAR* rgmemid);

	STDMETHOD(Invoke)(
	  void FAR* pvInstance,
	  MEMBERID memid,
	  unsigned short wFlags,
	  DISPPARAMS FAR *pdispparams,
	  VARIANT FAR *pvarResult,
	  EXCEPINFO FAR *pexcepinfo,
	  unsigned int FAR *puArgErr);

	STDMETHOD(GetDocumentation)(
	  MEMBERID memid,
	  BSTR FAR* pbstrName,
	  BSTR FAR* pbstrDocString,
	  unsigned long FAR* pdwHelpContext,
	  BSTR FAR* pbstrHelpFile);

	STDMETHOD(GetDllEntry)(
	  MEMBERID memid,
	  INVOKEKIND invkind,
	  BSTR FAR* pbstrDllName,
	  BSTR FAR* pbstrName,
	  unsigned short FAR* pwOrdinal);

	STDMETHOD(GetRefTypeInfo)(
	  HREFTYPE hreftype, ITypeInfo FAR* FAR* pptinfo);

	STDMETHOD(AddressOfMember)(
	  MEMBERID memid, INVOKEKIND invkind, void FAR* FAR* ppv);

	STDMETHOD(CreateInstance)(
	  IUnknown FAR* punkOuter,
	  REFIID riid,
	  void FAR* FAR* ppvObj);

	STDMETHOD(GetMops)( MEMBERID memid, BSTR FAR* pbstrMops);

	STDMETHOD(GetContainingTypeLib)(
	  ITypeLib FAR* FAR* pptlib, unsigned int FAR* pindex);

	STDMETHOD_(void, ReleaseTypeAttr)( TYPEATTR FAR* ptypeattr);
	STDMETHOD_(void, ReleaseFuncDesc)( FUNCDESC FAR* pfuncdesc);
	STDMETHOD_(void, ReleaseVarDesc)( VARDESC FAR* pvardesc);

	inline CTypeInfoW(LPUNKNOWN pUnk, ITypeInfoA * pANSI) :
			CWideInterface(ID_ITypeInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline ITypeInfoA * GetANSI() const
		{ return (ITypeInfoA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CTypeCompW
//
//  Synopsis:   Class definition of ITypeCompW
//
//---------------------------------------------------------------------------
class CTypeCompW : CWideInterface
{
public:
	// *** ITypeCompW methods ***
	STDMETHOD(Bind)(
	  OLECHAR FAR* szName,
	  unsigned long lHashVal,
	  unsigned short wflags,
	  ITypeInfo FAR* FAR* pptinfo,
	  DESCKIND FAR* pdesckind,
	  BINDPTR FAR* pbindptr);

	STDMETHOD(BindType)(
	  OLECHAR FAR* szName,
	  unsigned long lHashVal,
	  ITypeInfo FAR* FAR* pptinfo,
	  ITypeComp FAR* FAR* pptcomp);

	inline CTypeCompW(LPUNKNOWN pUnk, ITypeCompA * pANSI) :
			CWideInterface(ID_ITypeComp, pUnk, (LPUNKNOWN)pANSI) {};

	inline ITypeCompA * GetANSI() const
		{ return (ITypeCompA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CCreateTypeLibW
//
//  Synopsis:   Class definition of ICreateTypeLibW
//
//---------------------------------------------------------------------------
class CCreateTypeLibW : CWideInterface
{
public:
	// *** ICreateTypeLibW methods ***
	STDMETHOD(CreateTypeInfo)(
	  OLECHAR FAR* szName,
	  TYPEKIND tkind,
	  ICreateTypeInfo FAR* FAR* lplpictinfo);

	STDMETHOD(SetName)( OLECHAR FAR* szName);

	STDMETHOD(SetVersion)(
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum);

	STDMETHOD(SetGuid) ( REFGUID guid);

	STDMETHOD(SetDocString)( OLECHAR FAR* szDoc);

	STDMETHOD(SetHelpFileName)( OLECHAR FAR* szHelpFileName);

	STDMETHOD(SetHelpContext)( unsigned long dwHelpContext);

	STDMETHOD(SetLcid)( LCID lcid);

	STDMETHOD(SetLibFlags)( unsigned int uLibFlags);

	STDMETHOD(SaveAllChanges)(VOID);

	inline CCreateTypeLibW(LPUNKNOWN pUnk, ICreateTypeLibA * pANSI) :
			CWideInterface(ID_ICreateTypeLib, pUnk, (LPUNKNOWN)pANSI) {};

	inline ICreateTypeLibA * GetANSI() const
		{ return (ICreateTypeLibA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CCreateTypeInfoW
//
//  Synopsis:   Class definition of ICreateTypeInfoW
//
//---------------------------------------------------------------------------
class CCreateTypeInfoW : CWideInterface
{
public:
	// *** ICreateTypeInfoW methods ***
	STDMETHOD(SetGuid)( REFGUID guid);

	STDMETHOD(SetTypeFlags)( unsigned int uTypeFlags);

	STDMETHOD(SetDocString)( OLECHAR FAR* pstrDoc);

	STDMETHOD(SetHelpContext)( unsigned long dwHelpContext);

	STDMETHOD(SetVersion)(
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum);

	STDMETHOD(AddRefTypeInfo)(
	  ITypeInfo FAR* ptinfo, HREFTYPE FAR* phreftype);

	STDMETHOD(AddFuncDesc)(
	  unsigned int index, FUNCDESC FAR* pfuncdesc);

	STDMETHOD(AddImplType)(
	  unsigned int index, HREFTYPE hreftype);

	STDMETHOD(SetImplTypeFlags)(
	  unsigned int index, int impltypeflags);

	STDMETHOD(SetAlignment)( unsigned short cbAlignment);

	STDMETHOD(SetSchema)( OLECHAR FAR* lpstrSchema);

	STDMETHOD(AddVarDesc)(
	  unsigned int index, VARDESC FAR* pvardesc);

	STDMETHOD(SetFuncAndParamNames)(
	  unsigned int index, OLECHAR FAR* FAR* rgszNames, unsigned int cNames);

	STDMETHOD(SetVarName)(
	  unsigned int index, OLECHAR FAR* szName);

	STDMETHOD(SetTypeDescAlias)(
	  TYPEDESC FAR* ptdescAlias);

	STDMETHOD(DefineFuncAsDllEntry)(
	  unsigned int index, OLECHAR FAR* szDllName, OLECHAR FAR* szProcName);

	STDMETHOD(SetFuncDocString)(
	  unsigned int index, OLECHAR FAR* szDocString);

	STDMETHOD(SetVarDocString)(
	  unsigned int index, OLECHAR FAR* szDocString);

	STDMETHOD(SetFuncHelpContext)(
	  unsigned int index, unsigned long dwHelpContext);

	STDMETHOD(SetVarHelpContext)(
	  unsigned int index, unsigned long dwHelpContext);

	STDMETHOD(SetMops)(
	  unsigned int index, BSTR bstrMops);

	STDMETHOD(SetTypeIdldesc)(
	  IDLDESC FAR* pidldesc);

	STDMETHOD(LayOut)(VOID);

	inline CCreateTypeInfoW(LPUNKNOWN pUnk, ICreateTypeInfoA * pANSI) :
			CWideInterface(ID_ICreateTypeInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline ICreateTypeInfoA * GetANSI() const
		{ return (ICreateTypeInfoA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CEnumVARIANTW
//
//  Synopsis:   Class definition of IEnumVARIANTW
//
//---------------------------------------------------------------------------
class CEnumVARIANTW : CWideInterface
{
public:
	// *** IEnumVARIANTW methods ***
	STDMETHOD(Next)(
	   unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
	STDMETHOD(Skip)( unsigned long celt);
	STDMETHOD(Reset)(VOID);
	STDMETHOD(Clone)( IEnumVARIANT FAR* FAR* ppenum);

	inline CEnumVARIANTW(LPUNKNOWN pUnk, IEnumVARIANTA * pANSI) :
			CWideInterface(ID_IEnumVARIANT, pUnk, (LPUNKNOWN)pANSI) {};

	inline IEnumVARIANTA * GetANSI() const
		{ return (IEnumVARIANTA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CDispatchW
//
//  Synopsis:   Class definition of IDispatchW
//
//---------------------------------------------------------------------------
class CDispatchW : CWideInterface
{
public:
	// *** IDispatchW methods ***
	STDMETHOD(GetTypeInfoCount)(unsigned int * pctinfo);

	STDMETHOD(GetTypeInfo)(
	  unsigned int itinfo,
	  LCID lcid,
	  ITypeInfo * * pptinfo);

	STDMETHOD(GetIDsOfNames)(
	  REFIID riid,
	  LPOLESTR * rgszNames,
	  unsigned int cNames,
	  LCID lcid,
	  DISPID * rgdispid);

	STDMETHOD(Invoke)(
	  DISPID dispidMember,
	  REFIID riid,
	  LCID lcid,
	  unsigned short wFlags,
	  DISPPARAMS * pdispparams,
	  VARIANT * pvarResult,
	  EXCEPINFO * pexcepinfo,
	  unsigned int * puArgErr);

	inline CDispatchW(LPUNKNOWN pUnk, IDispatchA * pANSI) :
			CWideInterface(ID_IDispatch, pUnk, (LPUNKNOWN)pANSI) {};

	inline IDispatchA * GetANSI() const
		{ return (IDispatchA *)m_pObj; };
};


#ifndef NOERRORINFO
//+--------------------------------------------------------------------------
//
//  Class:      CErrorInfoW
//
//  Synopsis:   Class definition of IErrorInfoW
//
//---------------------------------------------------------------------------
class CErrorInfoW : CWideInterface
{
public:
	// *** IErrorInfoW methods ***
	STDMETHOD(GetGUID)(GUID *pguid);
	STDMETHOD(GetSource)(BSTR *pbstrSource);
	STDMETHOD(GetDescription)(BSTR *pbstrDescription);
	STDMETHOD(GetHelpFile)(BSTR *pbstrHelpFile);
	STDMETHOD(GetHelpContext)(DWORD *pdwHelpContext);

	inline CErrorInfoW(LPUNKNOWN pUnk, IErrorInfoA * pANSI) :
			CWideInterface(ID_IErrorInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline IErrorInfoA * GetANSI() const
		{ return (IErrorInfoA *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CCreateErrorInfoW
//
//  Synopsis:   Class definition of ICreateErrorInfoW
//
//---------------------------------------------------------------------------
class CCreateErrorInfoW : CWideInterface
{
public:
	// *** ICreateErrorInfoW methods ***
	STDMETHOD(SetGUID)(REFGUID rguid);
	STDMETHOD(SetSource)(LPOLESTR szSource);
	STDMETHOD(SetDescription)(LPOLESTR szDescription);
	STDMETHOD(SetHelpFile)(LPOLESTR szHelpFile);
	STDMETHOD(SetHelpContext)(DWORD dwHelpContext);

	inline CCreateErrorInfoW(LPUNKNOWN pUnk, ICreateErrorInfoA * pANSI) :
			CWideInterface(ID_ICreateErrorInfo, pUnk, (LPUNKNOWN)pANSI) {};

	inline ICreateErrorInfoA * GetANSI() const
		{ return (ICreateErrorInfoA *)m_pObj; };
};

#endif //!NOERRORINFO
