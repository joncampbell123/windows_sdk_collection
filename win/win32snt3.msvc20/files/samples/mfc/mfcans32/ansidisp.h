//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansidisp.h
//
//  Contents:   ANSI Wrappers for Unicode Dispatch Interfaces and APIs.
//
//  Classes:    CTypeLibA        - ANSI wrapper object for ITypeLib.
//              CTypeInfoA       - ANSI wrapper object for TypeInfo.
//              CTypeCompA       - ANSI wrapper object for TypeComp.
//              CCreateTypeLibA  - ANSI wrapper object for CreateTypeLib.
//              CCreateTypeInfoA - ANSI wrapper object for CreateTypeInfo.
//              CEnumVARIANTA    - ANSI wrapper object for EnumVARIANT.
//              CDispatchA       - ANSI wrapper object for Dispatch.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#if defined(__cplusplus)
interface ITypeLibA;
interface ITypeInfoA;
interface ITypeCompA;
interface ICreateTypeLibA;
interface ICreateTypeInfoA;
interface IEnumVARIANTA;
interface IDispatchA;
#else
typedef interface ITypeLibA ITypeLibA;
typedef interface ITypeInfoA ITypeInfoA;
typedef interface ITypeCompA ITypeCompA;
typedef interface ICreateTypeLibA ICreateTypeLibA;
typedef interface ICreateTypeInfoA ICreateTypeInfoA;
typedef interface IEnumVARIANTA IEnumVARIANTA;
typedef interface IDispatchA IDispatchA;
#endif

typedef ITypeLibA * LPTYPELIBA;
typedef ITypeInfoA * LPTYPEINFOA;
typedef ITypeCompA * LPTYPECOMPA;
typedef ICreateTypeLibA * LPCREATETYPELIBA;
typedef ICreateTypeInfoA * LPCREATETYPEINFOA;
typedef IEnumVARIANTA * LPENUMVARIANTA;
typedef IDispatchA * LPDISPATCHA;


/*---------------------------------------------------------------------*/
/*                         ANSI BSTR API                               */
/*---------------------------------------------------------------------*/

typedef char FAR* BSTRA;
typedef BSTRA * LPBSTRA;

STDAPI_(BSTRA) SysAllocStringA(const char FAR*);
STDAPI_(BSTRA) SysAllocStringLenA(const char FAR*, unsigned int);
STDAPI_(int)   SysReAllocStringA(BSTRA FAR*, const char FAR*);
STDAPI_(int)   SysReAllocStringLenA(BSTRA FAR*, const char FAR*, unsigned int);
STDAPI_(unsigned int) SysStringLenA(BSTRA);
STDAPI_(void)  SysFreeStringA(BSTRA);


/*---------------------------------------------------------------------*/
/*                         ANSI SafeArray API                          */
/*---------------------------------------------------------------------*/

typedef SAFEARRAY SAFEARRAYA;

STDAPI
SafeArrayAllocDescriptorA(unsigned int cDims, SAFEARRAYA FAR* FAR* ppsaOut);

STDAPI SafeArrayAllocDataA(SAFEARRAYA FAR* psa);

STDAPI_(SAFEARRAYA FAR*)
SafeArrayCreateA(
	VARTYPE vt,
	unsigned int cDims,
	SAFEARRAYBOUND FAR* rgsabound);

STDAPI SafeArrayDestroyDescriptorA(SAFEARRAYA FAR* psa);

STDAPI SafeArrayDestroyDataA(SAFEARRAYA FAR* psa);

STDAPI SafeArrayDestroyA(SAFEARRAYA FAR* psa);

STDAPI SafeArrayRedimA(SAFEARRAYA FAR* psa, SAFEARRAYBOUND FAR* psaboundNew);

STDAPI_(unsigned int) SafeArrayGetDimA(SAFEARRAYA FAR* psa);

STDAPI_(unsigned int) SafeArrayGetElemsizeA(SAFEARRAYA FAR* psa);

STDAPI
SafeArrayGetUBoundA(
	SAFEARRAYA FAR* psa,
	unsigned int nDim,
	long FAR* plUbound);

STDAPI
SafeArrayGetLBoundA(
	SAFEARRAYA FAR* psa,
	unsigned int nDim,
	long FAR* plLbound);

STDAPI SafeArrayLockA(SAFEARRAYA FAR* psa);

STDAPI SafeArrayUnlockA(SAFEARRAYA FAR* psa);

STDAPI SafeArrayAccessDataA(SAFEARRAYA FAR* psa, void HUGEP* FAR* ppvData);

STDAPI SafeArrayUnaccessDataA(SAFEARRAYA FAR* psa);

STDAPI
SafeArrayGetElementA(
	SAFEARRAYA FAR* psa,
	long FAR* rgIndices,
	void FAR* pv);

STDAPI
SafeArrayPutElementA(
	SAFEARRAYA FAR* psa,
	long FAR* rgIndices,
	void FAR* pv);

STDAPI
SafeArrayCopyA(
	SAFEARRAYA FAR* psa,
	SAFEARRAYA FAR* FAR* ppsaOut);



/*---------------------------------------------------------------------*/
/*                         ANSI VARIANT API                            */
/*---------------------------------------------------------------------*/

typedef struct FARSTRUCT tagVARIANTA VARIANTA;
typedef struct FARSTRUCT tagVARIANTA FAR* LPVARIANTA;
typedef struct FARSTRUCT tagVARIANTA VARIANTARGA;
typedef struct FARSTRUCT tagVARIANTA FAR* LPVARIANTARGA;

struct FARSTRUCT tagVARIANTA{
	VARTYPE vt;
	unsigned short wReserved1;
	unsigned short wReserved2;
	unsigned short wReserved3;
	union {
	  unsigned char bVal;              /* VT_UI1                */
	  short    iVal;              /* VT_I2                */
	  long     lVal;              /* VT_I4                */
	  float    fltVal;            /* VT_R4                */
	  double       dblVal;            /* VT_R8                */
	  VARIANT_BOOL bool;              /* VT_BOOL              */
	  SCODE    scode;             /* VT_ERROR             */
	  CY       cyVal;             /* VT_CY                */
	  DATE     date;              /* VT_DATE              */
	  BSTRA    bstrVal;           /* VT_BSTR              */
	  IUnknown     FAR* punkVal;      /* VT_UNKNOWN           */
	  IDispatchA   FAR* pdispVal;     /* VT_DISPATCH          */
	  SAFEARRAYA   FAR* parray;       /* VT_ARRAY|*           */

	  unsigned char    FAR* pbVal;        /* VT_BYREF|VT_UI1       */
	  short    FAR* piVal;        /* VT_BYREF|VT_I2       */
	  long     FAR* plVal;        /* VT_BYREF|VT_I4       */
	  float    FAR* pfltVal;      /* VT_BYREF|VT_R4       */
	  double       FAR* pdblVal;      /* VT_BYREF|VT_R8       */
	  VARIANT_BOOL FAR* pbool;        /* VT_BYREF|VT_BOOL     */
	  SCODE    FAR* pscode;       /* VT_BYREF|VT_ERROR    */
	  CY       FAR* pcyVal;       /* VT_BYREF|VT_CY       */
	  DATE     FAR* pdate;        /* VT_BYREF|VT_DATE     */
	  BSTRA    FAR* pbstrVal;     /* VT_BYREF|VT_BSTR     */
	  IUnknown   FAR* FAR* ppunkVal;  /* VT_BYREF|VT_UNKNOWN  */
	  IDispatchA FAR* FAR* ppdispVal; /* VT_BYREF|VT_DISPATCH */
	  SAFEARRAYA FAR* FAR* pparray;   /* VT_BYREF|VT_ARRAY|*  */
	  VARIANTA     FAR* pvarVal;      /* VT_BYREF|VT_VARIANT  */

	  void     FAR* byref;        /* Generic ByRef        */
	}
#if defined(NONAMELESSUNION) || (defined(_MAC) && !defined(__cplusplus) && !defined(_MSC_VER))
	u
#endif
	;
};

STDAPI_(void)
VariantInitA(VARIANTARGA FAR* pvarg);

STDAPI
VariantClearA(VARIANTARGA FAR* pvarg);

STDAPI
VariantCopyA(
	VARIANTARGA FAR* pvargDest,
	VARIANTARGA FAR* pvargSrc);

STDAPI
VariantCopyIndA(
	VARIANTA FAR* pvarDest,
	VARIANTARGA FAR* pvargSrc);

STDAPI
VariantChangeTypeA(
	VARIANTARGA FAR* pvargDest,
	VARIANTARGA FAR* pvarSrc,
	unsigned short wFlags,
	VARTYPE vt);

STDAPI
VariantChangeTypeExA(
	VARIANTARGA FAR* pvargDest,
	VARIANTARGA FAR* pvarSrc,
	LCID lcid,
	unsigned short wFlags,
	VARTYPE vt);


/*---------------------------------------------------------------------*/
/*                   ANSI VARIANT Coercion API                         */
/*---------------------------------------------------------------------*/

#ifndef NOI1
STDAPI VarUI1FromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, unsigned char FAR* pbOut);
STDAPI VarUI1FromDispA(IDispatchA FAR* pdispIn, LCID lcid, unsigned char FAR* pbOut);
#endif //!NOI1

STDAPI VarI2FromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, short FAR* psOut);
STDAPI VarI2FromDispA(IDispatchA FAR* pdispIn, LCID lcid, short FAR* psOut);

STDAPI VarI4FromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, long FAR* plOut);
STDAPI VarI4FromDispA(IDispatchA FAR* pdispIn, LCID lcid, long FAR* plOut);

STDAPI VarR4FromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, float FAR* pfltOut);
STDAPI VarR4FromDispA(IDispatchA FAR* pdispIn, LCID lcid, float FAR* pfltOut);

STDAPI VarR8FromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, double FAR* pdblOut);
STDAPI VarR8FromDispA(IDispatchA FAR* pdispIn, LCID lcid, double FAR* pdblOut);

STDAPI VarDateFromStrA(char FAR* strIn, LCID lcid,
					   unsigned long dwFlags, DATE FAR* pdateOut);
STDAPI VarDateFromDispA(IDispatchA FAR* pdispIn, LCID lcid, DATE FAR* pdateOut);
STDAPI VarCyFromStrA(char FAR* strIn, LCID lcid,
					 unsigned long dwFlags, CY FAR* pcyOut);
STDAPI VarCyFromDispA(IDispatchA FAR* pdispIn, LCID lcid, CY FAR* pcyOut);

#ifndef NOI1
STDAPI VarBstrFromUI1A(unsigned char bVal, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
#endif //!NOI1
STDAPI VarBstrFromI2A(short iVal, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromI4A(long lIn, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromR4A(float fltIn, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromR8A(double dblIn, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromCyA(CY cyIn, LCID lcid,
					  unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromDateA(DATE dateIn, LCID lcid,
						unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromDispA(IDispatchA FAR* pdispIn, LCID lcid,
						unsigned long dwFlags, BSTRA FAR* pbstrOut);
STDAPI VarBstrFromBoolA(VARIANT_BOOL boolIn, LCID lcid,
						unsigned long dwFlags, BSTRA FAR* pbstrOut);

STDAPI VarBoolFromStrA(char FAR* strIn, LCID lcid,
					   unsigned long dwFlags, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromDispA(IDispatchA FAR* pdispIn, LCID lcid,
						VARIANT_BOOL FAR* pboolOut);


/***********************************************************************/
/*                                                                     */
/*                  ANSI Support for Dispatch.h Elements               */
/*                                                                     */
/***********************************************************************/

/*---------------------------------------------------------------------*/
/*                       ANSI Dispatch Typedefs                        */
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/*                       ANSI Dispatch Typedefs                        */
/*---------------------------------------------------------------------*/

typedef struct tagEXCEPINFOA {
	unsigned short wCode;             /* An error code describing the error. */
					  /* Either (but not both) the wCode or */
					  /* scode fields must be set */
	unsigned short wReserved;

	BSTRA bstrSource;       /* A textual, human readable name of the
				   source of the exception. It is up to the
				   IDispatch implementor to fill this in.
				   Typically this will be an application name. */

	BSTRA bstrDescription;   /* A textual, human readable description of the
				   error. If no description is available, NULL
				   should be used. */

	BSTRA bstrHelpFile;      /* Fully qualified drive, path, and file name
				   of a help file with more information about
				   the error.  If no help is available, NULL
				   should be used. */

	unsigned long dwHelpContext;
				/* help context of topic within the help file. */

	void * pvReserved;

	/* Use of this field allows an application to defer filling in
	   the bstrDescription, bstrHelpFile, and dwHelpContext fields
	   until they are needed.  This field might be used, for example,
	   if loading the string for the error is a time-consuming
	   operation. If deferred fill-in is not desired, this field should
	   be set to NULL. */
#ifdef _MAC
# ifdef _MSC_VER
	HRESULT (STDAPICALLTYPE * pfnDeferredFillIn)(struct tagEXCEPINFOA *);
# else
	STDAPICALLTYPE HRESULT (* pfnDeferredFillIn)(struct tagEXCEPINFOA *);
# endif
#else
	HRESULT (STDAPICALLTYPE * pfnDeferredFillIn)(struct tagEXCEPINFOA *);
#endif

	SCODE scode;        /* An SCODE describing the error. */

} EXCEPINFOA, * LPEXCEPINFOA;

typedef struct tagVARDESCA {
	MEMBERID memid;
	CHAR * lpstrSchema;     /* reserved for future use */
	union {
	  /* VAR_PERINSTANCE - the offset of this variable within the instance */
	  unsigned long oInst;

	  /* VAR_CONST - the value of the constant */
	  VARIANTA * lpvarValue;

	};
	ELEMDESC elemdescVar;
	unsigned short wVarFlags;
	VARKIND varkind;
} VARDESCA, * LPVARDESCA;

typedef struct tagPARAMDATAA {
	CHAR * szName;      /* parameter name */
	VARTYPE vt;         /* parameter type */
} PARAMDATAA, * LPPARAMDATAA;

typedef struct tagMETHODDATAA {
	CHAR * szName;      /* method name */
	PARAMDATAA * ppdata;    /* pointer to an array of PARAMDATAs */
	DISPID dispid;      /* method ID */
	unsigned int iMeth;     /* method index */
	CALLCONV cc;        /* calling convention */
	unsigned int cArgs;     /* count of arguments */
	unsigned short wFlags;  /* same wFlags as on IDispatch::Invoke() */
	VARTYPE vtReturn;
} METHODDATAA, * LPMETHODDATAA;

typedef struct tagINTERFACEDATAA {
	METHODDATAA * pmethdata;    /* pointer to an array of METHODDATAs */
	unsigned int cMembers;  /* count of members */
} INTERFACEDATAA, * LPINTERFACEDATAA;

typedef union tagBINDPTRA {
	FUNCDESC * lpfuncdesc;
	VARDESCA * lpvardesc;
	ITypeCompA * lptcomp;
} BINDPTRA, * LPBINDPTRA;

typedef struct tagDISPPARAMSA{
	VARIANTARGA * rgvarg;
	DISPID * rgdispidNamedArgs;
	unsigned int cArgs;
	unsigned int cNamedArgs;
} DISPPARAMSA, * LPDISPPARAMSA;

typedef DISPPARAMS * LPDISPPARAMS;


/*---------------------------------------------------------------------*/
/*                             ITypeLibA                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ITypeLibA

DECLARE_INTERFACE_(ITypeLibA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* ITypeLib methods */
	STDMETHOD_(unsigned int,GetTypeInfoCount)(THIS) PURE;

	STDMETHOD(GetTypeInfo)(THIS_
	  unsigned int index, ITypeInfoA FAR* FAR* pptinfo) PURE;

	STDMETHOD(GetTypeInfoType)(THIS_
	  unsigned int index, TYPEKIND FAR* ptypekind) PURE;

	STDMETHOD(GetTypeInfoOfGuid)(THIS_
	  REFGUID guid, ITypeInfoA FAR* FAR* pptinfo) PURE;

	STDMETHOD(GetLibAttr)(THIS_
	  TLIBATTR FAR* FAR* pptlibattr) PURE;

	STDMETHOD(GetTypeComp)(THIS_
	  ITypeCompA FAR* FAR* pptcomp) PURE;

	STDMETHOD(GetDocumentation)(THIS_
	  int index,
	  BSTRA FAR* pbstrName,
	  BSTRA FAR* pbstrDocString,
	  unsigned long FAR* pdwHelpContext,
	  BSTRA FAR* pbstrHelpFile) PURE;

	STDMETHOD(IsName)(THIS_
	  CHAR FAR* szNameBuf,
	  unsigned long lHashVal,
	  int FAR* lpfName) PURE;

	STDMETHOD(FindName)(THIS_
	  CHAR FAR* szNameBuf,
	  unsigned long lHashVal,
	  ITypeInfoA FAR* FAR* rgptinfo,
	  MEMBERID FAR* rgmemid,
	  unsigned short FAR* pcFound) PURE;

	STDMETHOD_(void, ReleaseTLibAttr)(THIS_ TLIBATTR FAR* ptlibattr) PURE;
};

typedef ITypeLibA FAR* LPTYPELIBA;


/*---------------------------------------------------------------------*/
/*                            ITypeInfoA                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ITypeInfoA

DECLARE_INTERFACE_(ITypeInfoA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* ITypeInfo methods */
	STDMETHOD(GetTypeAttr)(THIS_ TYPEATTR FAR* FAR* pptypeattr) PURE;

	STDMETHOD(GetTypeComp)(THIS_ ITypeCompA FAR* FAR* pptcomp) PURE;

	STDMETHOD(GetFuncDesc)(THIS_
	  unsigned int index, FUNCDESC FAR* FAR* ppfuncdesc) PURE;

	STDMETHOD(GetVarDesc)(THIS_
	  unsigned int index, VARDESCA FAR* FAR* ppvardesc) PURE;

	STDMETHOD(GetNames)(THIS_
	  MEMBERID memid,
	  BSTRA FAR* rgbstrNames,
	  unsigned int cMaxNames,
	  unsigned int FAR* pcNames) PURE;

	STDMETHOD(GetRefTypeOfImplType)(THIS_
	  unsigned int index, HREFTYPE FAR* phreftype) PURE;

	STDMETHOD(GetImplTypeFlags)(THIS_
	  unsigned int index, int FAR* pimpltypeflags) PURE;

	STDMETHOD(GetIDsOfNames)(THIS_
	  char FAR* FAR* rgszNames,
	  unsigned int cNames,
	  MEMBERID FAR* rgmemid) PURE;

	STDMETHOD(Invoke)(THIS_
	  void FAR* pvInstance,
	  MEMBERID memid,
	  unsigned short wFlags,
	  DISPPARAMSA FAR *pdispparamsA,
	  VARIANTA FAR *pvarResult,
	  EXCEPINFOA FAR *pexcepinfo,
	  unsigned int FAR *puArgErr) PURE;

	STDMETHOD(GetDocumentation)(THIS_
	  MEMBERID memid,
	  BSTRA FAR* pbstrName,
	  BSTRA FAR* pbstrDocString,
	  unsigned long FAR* pdwHelpContext,
	  BSTRA FAR* pbstrHelpFile) PURE;

	STDMETHOD(GetDllEntry)(THIS_
	  MEMBERID memid,
	  INVOKEKIND invkind,
	  BSTRA FAR* pbstrDllName,
	  BSTRA FAR* pbstrName,
	  unsigned short FAR* pwOrdinal) PURE;

	STDMETHOD(GetRefTypeInfo)(THIS_
	  HREFTYPE hreftype, ITypeInfoA FAR* FAR* pptinfo) PURE;

	STDMETHOD(AddressOfMember)(THIS_
	  MEMBERID memid, INVOKEKIND invkind, void FAR* FAR* ppv) PURE;

	STDMETHOD(CreateInstance)(THIS_
	  IUnknown FAR* punkOuter,
	  REFIID riid,
	  void FAR* FAR* ppvObj) PURE;

	STDMETHOD(GetMops)(THIS_ MEMBERID memid, BSTRA FAR* pbstrMops) PURE;

	STDMETHOD(GetContainingTypeLib)(THIS_
	  ITypeLibA FAR* FAR* pptlib, unsigned int FAR* pindex) PURE;

	STDMETHOD_(void, ReleaseTypeAttr)(THIS_ TYPEATTR FAR* ptypeattr) PURE;
	STDMETHOD_(void, ReleaseFuncDesc)(THIS_ FUNCDESC FAR* pfuncdesc) PURE;
	STDMETHOD_(void, ReleaseVarDesc)(THIS_ VARDESCA FAR* pvardesc) PURE;
};

typedef ITypeInfoA FAR* LPTYPEINFOA;


/*---------------------------------------------------------------------*/
/*                            ITypeCompA                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ITypeCompA

DECLARE_INTERFACE_(ITypeCompA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* ITypeComp methods */
	STDMETHOD(Bind)(THIS_
	  CHAR FAR* szName,
	  unsigned long lHashVal,
	  unsigned short wflags,
	  ITypeInfoA FAR* FAR* pptinfo,
	  DESCKIND FAR* pdesckind,
	  BINDPTRA FAR* pbindptr) PURE;

	STDMETHOD(BindType)(THIS_
	  CHAR FAR* szName,
	  unsigned long lHashVal,
	  ITypeInfoA FAR* FAR* pptinfoA,
	  ITypeCompA FAR* FAR* pptcompA) PURE;
};

typedef ITypeCompA FAR* LPTYPECOMPA;


/*---------------------------------------------------------------------*/
/*                         ICreateTypeLibA                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ICreateTypeLibA

DECLARE_INTERFACE_(ICreateTypeLibA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* ICreateTypeLib methods */
	STDMETHOD(CreateTypeInfo)(THIS_
	  CHAR FAR* szName,
	  TYPEKIND tkind,
	  ICreateTypeInfoA FAR* FAR* lplpictinfo) PURE;

	STDMETHOD(SetName)(THIS_ CHAR FAR* szName) PURE;

	STDMETHOD(SetVersion)(THIS_
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

	STDMETHOD(SetGuid) (THIS_ REFGUID guid) PURE;

	STDMETHOD(SetDocString)(THIS_ CHAR FAR* szDoc) PURE;

	STDMETHOD(SetHelpFileName)(THIS_ CHAR FAR* szHelpFileName) PURE;

	STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

	STDMETHOD(SetLcid)(THIS_ LCID lcid) PURE;

	STDMETHOD(SetLibFlags)(THIS_ unsigned int uLibFlags) PURE;

	STDMETHOD(SaveAllChanges)(THIS) PURE;
};

typedef ICreateTypeLibA FAR* LPCREATETYPELIBA;


/*---------------------------------------------------------------------*/
/*                         ICreateTypeInfoA                            */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ICreateTypeInfoA

DECLARE_INTERFACE_(ICreateTypeInfoA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* ICreateTypeInfo methods */
	STDMETHOD(SetGuid)(THIS_ REFGUID guid) PURE;

	STDMETHOD(SetTypeFlags)(THIS_ unsigned int uTypeFlags) PURE;

	STDMETHOD(SetDocString)(THIS_ CHAR FAR* pstrDoc) PURE;

	STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

	STDMETHOD(SetVersion)(THIS_
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

	STDMETHOD(AddRefTypeInfo)(THIS_
	  ITypeInfoA FAR* ptinfo, HREFTYPE FAR* phreftype) PURE;

	STDMETHOD(AddFuncDesc)(THIS_
	  unsigned int index, FUNCDESC FAR* pfuncdesc) PURE;

	STDMETHOD(AddImplType)(THIS_
	  unsigned int index, HREFTYPE hreftype) PURE;

	STDMETHOD(SetImplTypeFlags)(THIS_
	  unsigned int index, int impltypeflags) PURE;

	STDMETHOD(SetAlignment)(THIS_ unsigned short cbAlignment) PURE;

	STDMETHOD(SetSchema)(THIS_ CHAR FAR* lpstrSchema) PURE;

	STDMETHOD(AddVarDesc)(THIS_
	  unsigned int index, VARDESCA FAR* pvardesc) PURE;

	STDMETHOD(SetFuncAndParamNames)(THIS_
	  unsigned int index, CHAR FAR* FAR* rgszNames, unsigned int cNames) PURE;

	STDMETHOD(SetVarName)(THIS_
	  unsigned int index, CHAR FAR* szName) PURE;

	STDMETHOD(SetTypeDescAlias)(THIS_
	  TYPEDESC FAR* ptdescAlias) PURE;

	STDMETHOD(DefineFuncAsDllEntry)(THIS_
	  unsigned int index, CHAR FAR* szDllName, CHAR FAR* szProcName) PURE;

	STDMETHOD(SetFuncDocString)(THIS_
	  unsigned int index, CHAR FAR* szDocString) PURE;

	STDMETHOD(SetVarDocString)(THIS_
	  unsigned int index, CHAR FAR* szDocString) PURE;

	STDMETHOD(SetFuncHelpContext)(THIS_
	  unsigned int index, unsigned long dwHelpContext) PURE;

	STDMETHOD(SetVarHelpContext)(THIS_
	  unsigned int index, unsigned long dwHelpContext) PURE;

	STDMETHOD(SetMops)(THIS_
	  unsigned int index, BSTRA bstrMops) PURE;

	STDMETHOD(SetTypeIdldesc)(THIS_
	  IDLDESC FAR* pidldesc) PURE;

	STDMETHOD(LayOut)(THIS) PURE;
};

typedef ICreateTypeInfoA FAR* LPCREATETYPEINFOA;


/*---------------------------------------------------------------------*/
/*                          IEnumVARIANTA                              */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE IEnumVARIANTA

DECLARE_INTERFACE_(IEnumVARIANTA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* IEnumVARIANT methods */
	STDMETHOD(Next)(
	  THIS_ unsigned long celt, VARIANTA FAR* rgvar, unsigned long FAR* pceltFetched) PURE;
	STDMETHOD(Skip)(THIS_ unsigned long celt) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ IEnumVARIANTA FAR* FAR* ppenum) PURE;
};

typedef IEnumVARIANTA FAR* LPENUMVARIANTA;


/*---------------------------------------------------------------------*/
/*                             IDispatchA                              */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE IDispatchA

DECLARE_INTERFACE_(IDispatchA, IUnknown)
{
	//BEGIN_INTERFACE

	/* IUnknown methods */
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void * * ppvObj) PURE;
	STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
	STDMETHOD_(unsigned long, Release)(THIS) PURE;

	/* IDispatch methods */
	STDMETHOD(GetTypeInfoCount)(THIS_ unsigned int * pctinfo) PURE;

	STDMETHOD(GetTypeInfo)(
	  THIS_
	  unsigned int itinfo,
	  LCID lcid,
	  ITypeInfoA * * pptinfo) PURE;

	STDMETHOD(GetIDsOfNames)(
	  THIS_
	  REFIID riid,
	  char * * rgszNames,
	  unsigned int cNames,
	  LCID lcid,
	  DISPID * rgdispid) PURE;

	STDMETHOD(Invoke)(
	  THIS_
	  DISPID dispidMember,
	  REFIID riid,
	  LCID lcid,
	  unsigned short wFlags,
	  DISPPARAMSA * pdispparamsA,
	  VARIANTA * pvarResult,
	  EXCEPINFOA * pexcepinfo,
	  unsigned int * puArgErr) PURE;
};

typedef IDispatchA * LPDISPATCHA;


/*---------------------------------------------------------------------*/
/*                         ANSI Dispatch API                           */
/*---------------------------------------------------------------------*/

#define LHashValOfNameA(lcid, szName) \
	LHashValOfNameSysA(SYS_WIN32, lcid, szName)

STDAPI
LoadTypeLibA(const char * szFile, ITypeLibA * * pptlib);

STDAPI
LoadRegTypeLibA(
	REFGUID rguid,
	unsigned short wVerMajor,
	unsigned short wVerMinor,
	LCID lcid,
	ITypeLibA * * pptlib);

STDAPI
QueryPathOfRegTypeLibA(
	REFGUID guid,
	unsigned short wMaj,
	unsigned short wMin,
	LCID lcid,
	LPBSTRA lpbstrPathName);

STDAPI
RegisterTypeLibA(
	ITypeLibA * ptlib,
	char * szFullPath,
	char * szHelpDir);

STDAPI
CreateTypeLibA(SYSKIND syskind, const char * szFile, ICreateTypeLibA * * ppctlib);


/*---------------------------------------------------------------------*/
/*                         ANSI Dispatch API                           */
/*---------------------------------------------------------------------*/

STDAPI
DispGetParamA(
	DISPPARAMSA * pdispparamsA,
	unsigned int position,
	VARTYPE vtTarg,
	VARIANTA * pvarResult,
	unsigned int * puArgErr);

STDAPI
DispGetIDsOfNamesA(
	ITypeInfoA * ptinfo,
	char * * rgszNames,
	unsigned int cNames,
	DISPID * rgdispid);

STDAPI
DispInvokeA(
	void * _this,
	ITypeInfoA * ptinfo,
	DISPID dispidMember,
	unsigned short wFlags,
	DISPPARAMSA * pparamsA,
	VARIANTA * pvarResult,
	EXCEPINFOA * pexcepinfo,
	unsigned int * puArgErr);

STDAPI
CreateDispTypeInfoA(
	INTERFACEDATAA * pidata,
	LCID lcid,
	ITypeInfoA * * pptinfo);

STDAPI
CreateStdDispatchA(
	IUnknown * punkOuter,
	void * pvThis,
	ITypeInfoA * ptinfo,
	IUnknown * * ppunkStdDisp);



//
//  Forward declarations
//
class CTypeLibA;
class CTypeInfoA;
class CTypeCompA;
class CCreateTypeLibA;
class CCreateTypeInfoA;
class CEnumVARIANTA;
class CDispatchA;


//+--------------------------------------------------------------------------
//
//  Class:      CTypeLibA
//
//  Synopsis:   Class definition of ITypeLibA
//
//---------------------------------------------------------------------------
class CTypeLibA : CAnsiInterface
{
public:
	// *** ITypeLibA methods ***
	STDMETHOD_(unsigned int,GetTypeInfoCount)(VOID);

	STDMETHOD(GetTypeInfo)(
	  unsigned int index, ITypeInfoA * * pptinfo);

	STDMETHOD(GetTypeInfoType)(
	  unsigned int index, TYPEKIND * ptypekind);

	STDMETHOD(GetTypeInfoOfGuid)(
	  REFGUID guid, ITypeInfoA * * pptinfo);

	STDMETHOD(GetLibAttr)(
	  TLIBATTR * * pptlibattr);

	STDMETHOD(GetTypeComp)(
	  ITypeCompA * * pptcomp);

	STDMETHOD(GetDocumentation)(
	  int index,
	  BSTRA * pbstrName,
	  BSTRA * pbstrDocString,
	  unsigned long * pdwHelpContext,
	  BSTRA * pbstrHelpFile);

	STDMETHOD(IsName)(
	  CHAR * szNameBuf,
	  unsigned long lHashVal,
	  int * lpfName);

	STDMETHOD(FindName)(
	  CHAR * szNameBuf,
	  unsigned long lHashVal,
	  ITypeInfoA * * rgptinfo,
	  MEMBERID * rgmemid,
	  unsigned short * pcFound);

	STDMETHOD_(void, ReleaseTLibAttr)( TLIBATTR * ptlibattr);

	inline CTypeLibA(LPUNKNOWN pUnk, ITypeLib * pWide) :
			CAnsiInterface(ID_ITypeLib, pUnk, (LPUNKNOWN)pWide) {};

	inline ITypeLib * GetWide() const
			{ return (ITypeLib *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CTypeInfoA
//
//  Synopsis:   Class definition of ITypeInfoA
//
//---------------------------------------------------------------------------
class CTypeInfoA : CAnsiInterface
{
public:
	// *** ITypeInfoA methods ***
	STDMETHOD(GetTypeAttr)(TYPEATTR FAR* FAR* pptypeattr);

	STDMETHOD(GetTypeComp)(ITypeCompA FAR* FAR* pptcomp);

	STDMETHOD(GetFuncDesc)(
	  unsigned int index, FUNCDESC FAR* FAR* ppfuncdesc);

	STDMETHOD(GetVarDesc)(
	  unsigned int index, VARDESCA FAR* FAR* ppvardesc);

	STDMETHOD(GetNames)(
	  MEMBERID memid,
	  BSTRA FAR* rgbstrNames,
	  unsigned int cMaxNames,
	  unsigned int FAR* pcNames);

	STDMETHOD(GetRefTypeOfImplType)(
	  unsigned int index, HREFTYPE FAR* phreftype);

	STDMETHOD(GetImplTypeFlags)(
	  unsigned int index, int FAR* pimpltypeflags);

	STDMETHOD(GetIDsOfNames)(
	  char FAR* FAR* rgszNames,
	  unsigned int cNames,
	  MEMBERID FAR* rgmemid);

	STDMETHOD(Invoke)(
	  void FAR* pvInstance,
	  MEMBERID memid,
	  unsigned short wFlags,
	  DISPPARAMSA FAR *pdispparamsA,
	  VARIANTA FAR *pvarResult,
	  EXCEPINFOA FAR *pexcepinfo,
	  unsigned int FAR *puArgErr);

	STDMETHOD(GetDocumentation)(
	  MEMBERID memid,
	  BSTRA FAR* pbstrName,
	  BSTRA FAR* pbstrDocString,
	  unsigned long FAR* pdwHelpContext,
	  BSTRA FAR* pbstrHelpFile);

	STDMETHOD(GetDllEntry)(
	  MEMBERID memid,
	  INVOKEKIND invkind,
	  BSTRA FAR* pbstrDllName,
	  BSTRA FAR* pbstrName,
	  unsigned short FAR* pwOrdinal);

	STDMETHOD(GetRefTypeInfo)(
	  HREFTYPE hreftype, ITypeInfoA FAR* FAR* pptinfo);

	STDMETHOD(AddressOfMember)(
	  MEMBERID memid, INVOKEKIND invkind, void FAR* FAR* ppv);

	STDMETHOD(CreateInstance)(
	  IUnknown FAR* punkOuter,
	  REFIID riid,
	  void FAR* FAR* ppvObj);

	STDMETHOD(GetMops)(MEMBERID memid, BSTRA FAR* pbstrMops);

	STDMETHOD(GetContainingTypeLib)(
	  ITypeLibA FAR* FAR* pptlib, unsigned int FAR* pindex);

	STDMETHOD_(void, ReleaseTypeAttr)(TYPEATTR FAR* ptypeattr);
	STDMETHOD_(void, ReleaseFuncDesc)(FUNCDESC FAR* pfuncdesc);
	STDMETHOD_(void, ReleaseVarDesc)(VARDESCA FAR* pvardesc);

	inline CTypeInfoA(LPUNKNOWN pUnk, ITypeInfo * pWide) :
			CAnsiInterface(ID_ITypeInfo, pUnk, (LPUNKNOWN)pWide) {};

	inline ITypeInfo * GetWide() const
			{ return (ITypeInfo *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CTypeCompA
//
//  Synopsis:   Class definition of ITypeCompA
//
//---------------------------------------------------------------------------
class CTypeCompA : CAnsiInterface
{
public:
	// *** ITypeCompA methods ***
	STDMETHOD(Bind)(
	  CHAR * szName,
	  unsigned long lHashVal,
	  unsigned short wflags,
	  ITypeInfoA * * pptinfo,
	  DESCKIND * pdesckind,
	  BINDPTRA * pbindptr);

	STDMETHOD(BindType)(
	  CHAR * szName,
	  unsigned long lHashVal,
	  ITypeInfoA * * pptinfoA,
	  ITypeCompA * * pptcompA);

	inline CTypeCompA(LPUNKNOWN pUnk, ITypeComp * pWide) :
			CAnsiInterface(ID_ITypeComp, pUnk, (LPUNKNOWN)pWide) {};

	inline ITypeComp * GetWide() const
			{ return (ITypeComp *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CCreateTypeLibA
//
//  Synopsis:   Class definition of ICreateTypeLibA
//
//---------------------------------------------------------------------------
class CCreateTypeLibA : CAnsiInterface
{
public:
	// *** ICreateTypeLibA methods ***
	STDMETHOD(CreateTypeInfo)(
	  CHAR * szName,
	  TYPEKIND tkind,
	  ICreateTypeInfoA * * lplpictinfo);

	STDMETHOD(SetName)( CHAR * szName);

	STDMETHOD(SetVersion)(
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum);

	STDMETHOD(SetGuid) ( REFGUID guid);

	STDMETHOD(SetDocString)( CHAR * szDoc);

	STDMETHOD(SetHelpFileName)( CHAR * szHelpFileName);

	STDMETHOD(SetHelpContext)( unsigned long dwHelpContext);

	STDMETHOD(SetLcid)( LCID lcid);

	STDMETHOD(SetLibFlags)( unsigned int uLibFlags);

	STDMETHOD(SaveAllChanges)(VOID);

	inline CCreateTypeLibA(LPUNKNOWN pUnk, ICreateTypeLib * pWide) :
			CAnsiInterface(ID_ICreateTypeLib, pUnk, (LPUNKNOWN)pWide) {};

	inline ICreateTypeLib * GetWide() const
			{ return (ICreateTypeLib *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CCreateTypeInfoA
//
//  Synopsis:   Class definition of ICreateTypeInfoA
//
//---------------------------------------------------------------------------
class CCreateTypeInfoA : CAnsiInterface
{
public:
	// *** ICreateTypeInfoA methods ***
	STDMETHOD(SetGuid)( REFGUID guid);

	STDMETHOD(SetTypeFlags)( unsigned int uTypeFlags);

	STDMETHOD(SetDocString)( CHAR * pstrDoc);

	STDMETHOD(SetHelpContext)( unsigned long dwHelpContext);

	STDMETHOD(SetVersion)(
	  unsigned short wMajorVerNum, unsigned short wMinorVerNum);

	STDMETHOD(AddRefTypeInfo)(
	  ITypeInfoA * ptinfo, HREFTYPE * phreftype);

	STDMETHOD(AddFuncDesc)(
	  unsigned int index, FUNCDESC * pfuncdesc);

	STDMETHOD(AddImplType)(
	  unsigned int index, HREFTYPE hreftype);

	STDMETHOD(SetImplTypeFlags)(
	  unsigned int index, int impltypeflags);

	STDMETHOD(SetAlignment)( unsigned short cbAlignment);

	STDMETHOD(SetSchema)( CHAR * lpstrSchema);

	STDMETHOD(AddVarDesc)(
	  unsigned int index, VARDESCA * pvardesc);

	STDMETHOD(SetFuncAndParamNames)(
	  unsigned int index, CHAR * * rgszNames, unsigned int cNames);

	STDMETHOD(SetVarName)(
	  unsigned int index, CHAR * szName);

	STDMETHOD(SetTypeDescAlias)(
	  TYPEDESC * ptdescAlias);

	STDMETHOD(DefineFuncAsDllEntry)(
	  unsigned int index, CHAR * szDllName, CHAR * szProcName);

	STDMETHOD(SetFuncDocString)(
	  unsigned int index, CHAR * szDocString);

	STDMETHOD(SetVarDocString)(
	  unsigned int index, CHAR * szDocString);

	STDMETHOD(SetFuncHelpContext)(
	  unsigned int index, unsigned long dwHelpContext);

	STDMETHOD(SetVarHelpContext)(
	  unsigned int index, unsigned long dwHelpContext);

	STDMETHOD(SetMops)(
	  unsigned int index, BSTRA bstrMops);

	STDMETHOD(SetTypeIdldesc)(
	  IDLDESC * pidldesc);

	STDMETHOD(LayOut)(VOID);

	inline CCreateTypeInfoA(LPUNKNOWN pUnk, ICreateTypeInfo * pWide) :
			CAnsiInterface(ID_ICreateTypeInfo, pUnk, (LPUNKNOWN)pWide) {};

	inline ICreateTypeInfo * GetWide() const
			{ return (ICreateTypeInfo *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CEnumVARIANTA
//
//  Synopsis:   Class definition of IEnumVARIANTA
//
//---------------------------------------------------------------------------
class CEnumVARIANTA : CAnsiInterface
{
public:
	// *** IEnumVARIANTA methods ***
	STDMETHOD(Next)(
	   unsigned long celt, VARIANTA * rgvar, unsigned long * pceltFetched);
	STDMETHOD(Skip)( unsigned long celt);
	STDMETHOD(Reset)(VOID);
	STDMETHOD(Clone)( IEnumVARIANTA * * ppenum);

	inline CEnumVARIANTA(LPUNKNOWN pUnk, IEnumVARIANT * pWide) :
			CAnsiInterface(ID_IEnumVARIANT, pUnk, (LPUNKNOWN)pWide) {};

	inline IEnumVARIANT * GetWide() const
			{ return (IEnumVARIANT *)m_pObj; };
};


//+--------------------------------------------------------------------------
//
//  Class:      CDispatchA
//
//  Synopsis:   Class definition of IDispatchA
//
//---------------------------------------------------------------------------
class CDispatchA : CAnsiInterface
{
public:
	// *** IDispatchA methods ***
	STDMETHOD(GetTypeInfoCount)(unsigned int * pctinfo);

	STDMETHOD(GetTypeInfo)(
	  unsigned int itinfo,
	  LCID lcid,
	  ITypeInfoA * * pptinfo);

	STDMETHOD(GetIDsOfNames)(
	  REFIID riid,
	  char * * rgszNames,
	  unsigned int cNames,
	  LCID lcid,
	  DISPID * rgdispid);

	STDMETHOD(Invoke)(
	  DISPID dispidMember,
	  REFIID riid,
	  LCID lcid,
	  unsigned short wFlags,
	  DISPPARAMSA * pdispparamsA,
	  VARIANTA * pvarResult,
	  EXCEPINFOA * pexcepinfo,
	  unsigned int * puArgErr);

	inline CDispatchA(LPUNKNOWN pUnk, IDispatch * pWide) :
			CAnsiInterface(ID_IDispatch, pUnk, (LPUNKNOWN)pWide) {};

	inline IDispatch * GetWide() const
			{ return (IDispatch *)m_pObj; };
};


#ifndef NOERRORINFO
//+--------------------------------------------------------------------------
//
//  Class:      CErrorInfoA
//
//  Synopsis:   Class definition of IErrorInfoA
//
//---------------------------------------------------------------------------
class CErrorInfoA : CAnsiInterface
{
public:
	// *** IErrorInfoA methods ***
	STDMETHOD(GetGUID)(GUID *pguid);
	STDMETHOD(GetSource)(BSTRA *pbstrSource);
	STDMETHOD(GetDescription)(BSTRA *pbstrDescription);
	STDMETHOD(GetHelpFile)(BSTRA *pbstrHelpFile);
	STDMETHOD(GetHelpContext)(DWORD *pdwHelpContext);

	inline CErrorInfoA(LPUNKNOWN pUnk, IErrorInfo * pObj) :
			CAnsiInterface(ID_IErrorInfo, pUnk, (LPUNKNOWN)pObj) {};

	inline IErrorInfo * GetWide() const
			{ return (IErrorInfo *)m_pObj; };
};

//+--------------------------------------------------------------------------
//
//  Class:      CCreateErrorInfoA
//
//  Synopsis:   Class definition of ICreateErrorInfoA
//
//---------------------------------------------------------------------------
class CCreateErrorInfoA : CAnsiInterface
{
public:
	// *** ICreateErrorInfoA methods ***
	STDMETHOD(SetGUID)(REFGUID rguid);
	STDMETHOD(SetSource)(LPSTR szSource);
	STDMETHOD(SetDescription)(LPSTR szDescription);
	STDMETHOD(SetHelpFile)(LPSTR szHelpFile);
	STDMETHOD(SetHelpContext)(DWORD dwHelpContext);

	inline CCreateErrorInfoA(LPUNKNOWN pUnk, ICreateErrorInfo * pObj) :
			CAnsiInterface(ID_ICreateErrorInfo, pUnk, (LPUNKNOWN)pObj) {};

	inline ICreateErrorInfo * GetWide() const
			{ return (ICreateErrorInfo *)m_pObj; };
};

#endif //!NOERRORINFO
