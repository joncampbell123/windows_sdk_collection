/*** 
*dispatch.h - OLE Automation definitions.
*
*  Copyright (C) 1992-1993, Microsoft Corporation.  All Rights Reserved.
*
*Purpose:
*  This file defines the Ole Automation interfaces and APIs.
*
*Implementation Notes:
*  This file requires ole2.h
*
*****************************************************************************/

#ifndef _DISPATCH_H_
#define _DISPATCH_H_

#include "variant.h"

#ifndef BEGIN_INTERFACE
# define BEGIN_INTERFACE
#endif

#if defined(NONAMELESSUNION) || (defined(_MAC) && !defined(__cplusplus) && !defined(_MSC_VER))
# define UNION_NAME(X) X
#else
# define UNION_NAME(X)
#endif


DEFINE_OLEGUID(IID_IDispatch,		0x00020400L, 0, 0);
DEFINE_OLEGUID(IID_IEnumVARIANT,	0x00020404L, 0, 0);
DEFINE_OLEGUID(IID_ITypeInfo,		0x00020401L, 0, 0);
DEFINE_OLEGUID(IID_ITypeLib,		0x00020402L, 0, 0);
DEFINE_OLEGUID(IID_ITypeComp,		0x00020403L, 0, 0);
DEFINE_OLEGUID(IID_ICreateTypeInfo,	0x00020405L, 0, 0);
DEFINE_OLEGUID(IID_ICreateTypeLib,	0x00020406L, 0, 0);

#ifdef WIN32
DEFINE_OLEGUID(IID_IDispatchW,		0x00020407L, 0, 0);
DEFINE_OLEGUID(IID_ITypeInfoW,		0x00020408L, 0, 0);
DEFINE_OLEGUID(IID_ITypeLibW,		0x00020409L, 0, 0);
DEFINE_OLEGUID(IID_ITypeCompW,		0x00020410L, 0, 0);
DEFINE_OLEGUID(IID_ICreateTypeInfoW,	0x00020411L, 0, 0);
DEFINE_OLEGUID(IID_ICreateTypeLibW,	0x00020412L, 0, 0);
#endif


/* forward declarations */
#ifdef __cplusplus

  interface IDispatch;
  interface IEnumVARIANT;
  interface ITypeInfo;
  interface ITypeLib;
  interface ITypeComp;
  interface ICreateTypeInfo;
  interface ICreateTypeLib;
  
  #ifdef WIN32
  interface IDispatchW;
  interface ITypeInfoW;
  interface ITypeLibW;  
  interface ITypeCompW;  
  interface ICreateTypeInfoW;
  interface ICreateTypeLibW;
  #endif

#else

  typedef interface IDispatch IDispatch;
  typedef interface IEnumVARIANT IEnumVARIANT;
  typedef interface ITypeInfo ITypeInfo;
  typedef interface ITypeLib ITypeLib;
  typedef interface ITypeComp ITypeComp;
  typedef interface ICreateTypeInfo ICreateTypeInfo;
  typedef interface ICreateTypeLib ICreateTypeLib;
  
  #ifdef WIN32  
  typedef interface IDispatchW IDispatchW;
  typedef interface ITypeInfoW ITypeInfoW;
  typedef interface ITypeLibW ITypeLibW;
  typedef interface ITypeCompW ITypeCompW;
  typedef interface ICreateTypeInfoW ICreateTypeInfoW;
  typedef interface ICreateTypeLibW ICreateTypeLibW;
  #endif
	  
#endif


/* IDispatch related error codes */

#define DISP_ERROR(X) MAKE_SCODE(SEVERITY_ERROR, FACILITY_DISPATCH, X)

#define DISP_E_UNKNOWNINTERFACE		DISP_ERROR(1)
#define DISP_E_MEMBERNOTFOUND		DISP_ERROR(3)
#define DISP_E_PARAMNOTFOUND		DISP_ERROR(4)
#define DISP_E_TYPEMISMATCH		DISP_ERROR(5)
#define DISP_E_UNKNOWNNAME		DISP_ERROR(6)
#define DISP_E_NONAMEDARGS		DISP_ERROR(7)
#define DISP_E_BADVARTYPE		DISP_ERROR(8)
#define DISP_E_EXCEPTION		DISP_ERROR(9)
#define DISP_E_OVERFLOW			DISP_ERROR(10)
#define DISP_E_BADINDEX			DISP_ERROR(11)
#define DISP_E_UNKNOWNLCID		DISP_ERROR(12)
#define DISP_E_ARRAYISLOCKED		DISP_ERROR(13)
#define DISP_E_BADPARAMCOUNT		DISP_ERROR(14)
#define DISP_E_PARAMNOTOPTIONAL		DISP_ERROR(15)
#define DISP_E_BADCALLEE		DISP_ERROR(16)
#define DISP_E_NOTACOLLECTION		DISP_ERROR(17)


#define TYPE_ERROR(X) MAKE_SCODE(SEVERITY_ERROR, FACILITY_DISPATCH, X)

#define TYPE_E_IOERROR			TYPE_ERROR(   57)
#define TYPE_E_COMPILEERROR		TYPE_ERROR(   90)
#define TYPE_E_CANTCREATETMPFILE	TYPE_ERROR(  322)
#define TYPE_E_ILLEGALINDEX		TYPE_ERROR(  341)
#define TYPE_E_IDNOTFOUND		TYPE_ERROR( 1000)
#define TYPE_E_BUFFERTOOSMALL		TYPE_ERROR(32790)
#define TYPE_E_READONLY 		TYPE_ERROR(32791)
#define TYPE_E_INVDATAREAD		TYPE_ERROR(32792)
#define TYPE_E_UNSUPFORMAT		TYPE_ERROR(32793)
#define TYPE_E_ALREADYCONTAINSNAME	TYPE_ERROR(32794)
#define TYPE_E_NOMATCHINGARITY		TYPE_ERROR(32795)
#define TYPE_E_REGISTRYACCESS		TYPE_ERROR(32796)
#define TYPE_E_LIBNOTREGISTERED 	TYPE_ERROR(32797)
#define TYPE_E_DUPLICATEDEFINITION	TYPE_ERROR(32798)
#define TYPE_E_USAGE			TYPE_ERROR(32799)
#define TYPE_E_DESTNOTKNOWN		TYPE_ERROR(32800)
#define TYPE_E_UNDEFINEDTYPE		TYPE_ERROR(32807)
#define TYPE_E_QUALIFIEDNAMEDISALLOWED	TYPE_ERROR(32808)
#define TYPE_E_INVALIDSTATE		TYPE_ERROR(32809)
#define TYPE_E_WRONGTYPEKIND		TYPE_ERROR(32810)
#define TYPE_E_ELEMENTNOTFOUND		TYPE_ERROR(32811)
#define TYPE_E_AMBIGUOUSNAME		TYPE_ERROR(32812)
#define TYPE_E_TYPEMISMATCH		TYPE_ERROR(32813)
#define TYPE_E_DLLFUNCTIONNOTFOUND	TYPE_ERROR(32814)
#define TYPE_E_BADMODULEKIND		TYPE_ERROR(35005)
#define TYPE_E_WRONGPLATFORM		TYPE_ERROR(35006)
#define TYPE_E_ALREADYBEINGLAIDOUT	TYPE_ERROR(35007)
#define TYPE_E_CANTLOADLIBRARY		TYPE_ERROR(40010)


/* if not already picked up from olenls.h */
#ifndef _LCID_DEFINED
typedef unsigned long LCID;
# define _LCID_DEFINED
#endif


/*---------------------------------------------------------------------*/
/*                            BSTR API                                 */
/*---------------------------------------------------------------------*/

#ifdef WIN32

  #ifdef UNICODE

    #define SysAllocString	SysAllocStringW
    #define SysAllocStringLen	SysAllocStringLenW
    #define SysReAllocString	SysReAllocStringW
    #define SysReAllocStringLen	SysReAllocStringLenW
    #define SysFreeString	SysFreeStringW
    #define SysStringLen	SysStringLenW
	  
  #else  /* ANSI */
  
    #define SysAllocString	SysAllocStringA
    #define SysAllocStringLen	SysAllocStringLenA
    #define SysReAllocString	SysReAllocStringA
    #define SysReAllocStringLen	SysReAllocStringLenA
    #define SysFreeString	SysFreeStringA
    #define SysStringLen	SysStringLenA
	  
  #endif

  /* ANSI BSTR API */
  STDAPI_(BSTR) SysAllocStringA(char FAR*);
  STDAPI_(int)  SysReAllocStringA(LPBSTR, char FAR*);
  STDAPI_(BSTR) SysAllocStringLenA(char FAR*, unsigned int);
  STDAPI_(int)  SysReAllocStringLenA(LPBSTR, char FAR*, unsigned int);
  STDAPI_(void) SysFreeStringA(BSTR);
  STDAPI_(unsigned int) SysStringLenA(BSTR);

  /* Unicode BSTR API */
  STDAPI_(WBSTR) SysAllocStringW(LPWSTR);
  STDAPI_(int)   SysReAllocStringW(LPWBSTR, LPWSTR);
  STDAPI_(WBSTR) SysAllocStringLenW(LPWSTR, unsigned int);
  STDAPI_(int)   SysReAllocStringLenW(LPWBSTR, LPWSTR, unsigned int);
  STDAPI_(void)  SysFreeStringW(WBSTR);
  STDAPI_(unsigned int)  SysStringLenW(WBSTR);

  STDAPI_(BSTR)  SysStringWtoA(WBSTR wbstr, unsigned int cp);
  STDAPI_(WBSTR) SysStringAtoW(BSTR bstr, unsigned int cp);

#else /* WIN16 or _MAC */

  STDAPI_(BSTR) SysAllocString(char FAR*);
  STDAPI_(int)  SysReAllocString(BSTR FAR*, char FAR*);
  STDAPI_(BSTR) SysAllocStringLen(char FAR*, unsigned int);
  STDAPI_(int)  SysReAllocStringLen(BSTR FAR*, char FAR*, unsigned int);
  STDAPI_(void) SysFreeString(BSTR);  
  STDAPI_(unsigned int) SysStringLen(BSTR);
  
#endif	  


/*---------------------------------------------------------------------*/
/*                            Time API                                 */
/*---------------------------------------------------------------------*/

STDAPI_(int)
DosDateTimeToVariantTime(
    unsigned short wDosDate,
    unsigned short wDosTime,
    double FAR* pvtime);

STDAPI_(int)
VariantTimeToDosDateTime(
    double vtime,
    unsigned short FAR* pwDosDate,
    unsigned short FAR* pwDosTime);


/*---------------------------------------------------------------------*/
/*                          SafeArray API                              */
/*---------------------------------------------------------------------*/

STDAPI
SafeArrayAllocDescriptor(unsigned int cDims, SAFEARRAY FAR* FAR* ppsaOut);

STDAPI SafeArrayAllocData(SAFEARRAY FAR* psa);

STDAPI_(SAFEARRAY FAR*)
SafeArrayCreate(
    VARTYPE vt,
    unsigned int cDims,
    SAFEARRAYBOUND FAR* rgsabound);

STDAPI SafeArrayDestroyDescriptor(SAFEARRAY FAR* psa);

STDAPI SafeArrayDestroyData(SAFEARRAY FAR* psa);

STDAPI SafeArrayDestroy(SAFEARRAY FAR* psa);

STDAPI SafeArrayRedim(SAFEARRAY FAR* psa, SAFEARRAYBOUND FAR* psaboundNew);

STDAPI_(unsigned int) SafeArrayGetDim(SAFEARRAY FAR* psa);

STDAPI_(unsigned int) SafeArrayGetElemsize(SAFEARRAY FAR* psa);

STDAPI
SafeArrayGetUBound(SAFEARRAY FAR* psa, unsigned int nDim, long FAR* plUbound);

STDAPI
SafeArrayGetLBound(SAFEARRAY FAR* psa, unsigned int nDim, long FAR* plLbound);

STDAPI SafeArrayLock(SAFEARRAY FAR* psa);

STDAPI SafeArrayUnlock(SAFEARRAY FAR* psa);

STDAPI SafeArrayAccessData(SAFEARRAY FAR* psa, void FAR* HUGEP* ppvData);

STDAPI SafeArrayUnaccessData(SAFEARRAY FAR* psa);

STDAPI
SafeArrayGetElement(
    SAFEARRAY FAR* psa,
    long FAR* rgIndices,
    void FAR* pv);

STDAPI
SafeArrayPutElement(
    SAFEARRAY FAR* psa,
    long FAR* rgIndices,
    void FAR* pv);

STDAPI
SafeArrayCopy(
    SAFEARRAY FAR* psa,
    SAFEARRAY FAR* FAR* ppsaOut);


/*---------------------------------------------------------------------*/
/*                           VARIANT API                               */
/*---------------------------------------------------------------------*/

STDAPI_(void)
VariantInit(VARIANTARG FAR* pvarg);

STDAPI
VariantClear(VARIANTARG FAR* pvarg);

STDAPI
VariantCopy(
    VARIANTARG FAR* pvargDest,
    VARIANTARG FAR* pvargSrc);

STDAPI
VariantCopyInd(
    VARIANT FAR* pvarDest,
    VARIANTARG FAR* pvargSrc);

STDAPI
VariantChangeType(
    VARIANTARG FAR* pvargDest,
    VARIANTARG FAR* pvarSrc,
    unsigned short wFlags,
    VARTYPE vt);

#define VARIANT_NOVALUEPROP 1


/*---------------------------------------------------------------------*/
/*                     VARTYPE Coersion API                            */
/*---------------------------------------------------------------------*/

/* Note: The routines that convert *from* a string are defined
 * to take a char* rather than a BSTR because no allocation is
 * required, and this makes the routines a bit more generic.
 * They may of course still be passed a BSTR as the strIn param.
 */
   
STDAPI VarI2FromI4(long lIn, short FAR* psOut);
STDAPI VarI2FromR4(float fltIn, short FAR* psOut);
STDAPI VarI2FromR8(double dblIn, short FAR* psOut);
STDAPI VarI2FromCy(CY cyIn, short FAR* psOut);
STDAPI VarI2FromDate(DATE dateIn, short FAR* psOut);
STDAPI VarI2FromStr(char FAR* strIn, LCID lcid, short FAR* psOut);
STDAPI VarI2FromDisp(IDispatch FAR* pdispIn, short FAR* psOut);
STDAPI VarI2FromBool(VARIANT_BOOL boolIn, short FAR* psOut);

STDAPI VarI4FromI2(short sIn, long FAR* plOut);
STDAPI VarI4FromR4(float fltIn, long FAR* plOut);
STDAPI VarI4FromR8(double dblIn, long FAR* plOut);
STDAPI VarI4FromCy(CY cyIn, long FAR* plOut);
STDAPI VarI4FromDate(DATE dateIn, long FAR* plOut);
STDAPI VarI4FromStr(char FAR* strIn, LCID lcid, long FAR* plOut);
STDAPI VarI4FromDisp(IDispatch FAR* pdispIn, long FAR* plOut);
STDAPI VarI4FromBool(VARIANT_BOOL boolIn, long FAR* plOut);

STDAPI VarR4FromI2(short sIn, float FAR* pfltOut);
STDAPI VarR4FromI4(long lIn, float FAR* pfltOut);
STDAPI VarR4FromR8(double dblIn, float FAR* pfltOut);
STDAPI VarR4FromCy(CY cyIn, float FAR* pfltOut);
STDAPI VarR4FromDate(DATE dateIn, float FAR* pfltOut);
STDAPI VarR4FromStr(char FAR* strIn, LCID lcid, float FAR* pfltOut);
STDAPI VarR4FromDisp(IDispatch FAR* pdispIn, float FAR* pfltOut);
STDAPI VarR4FromBool(VARIANT_BOOL boolIn, float FAR* pfltOut);

STDAPI VarR8FromI2(short sIn, double FAR* pdblOut);
STDAPI VarR8FromI4(long lIn, double FAR* pdblOut);
STDAPI VarR8FromR4(float fltIn, double FAR* pdblOut);
STDAPI VarR8FromCy(CY cyIn, double FAR* pdblOut);
STDAPI VarR8FromDate(DATE dateIn, double FAR* pdblOut);
STDAPI VarR8FromStr(char FAR* strIn, LCID lcid, double FAR* pdblOut);
STDAPI VarR8FromDisp(IDispatch FAR* pdispIn, double FAR* pdblOut);
STDAPI VarR8FromBool(VARIANT_BOOL boolIn, double FAR* pdblOut);

STDAPI VarDateFromI2(short sIn, DATE FAR* pdateOut);
STDAPI VarDateFromI4(long lIn, DATE FAR* pdateOut);
STDAPI VarDateFromR4(float fltIn, DATE FAR* pdateOut);
STDAPI VarDateFromR8(double dblIn, DATE FAR* pdateOut);
STDAPI VarDateFromCy(CY cyIn, DATE FAR* pdateOut);
STDAPI VarDateFromStr(char FAR* strIn, LCID lcid, DATE FAR* pdateOut);
STDAPI VarDateFromDisp(IDispatch FAR* pdispIn, DATE FAR* pdateOut);
STDAPI VarDateFromBool(VARIANT_BOOL boolIn, DATE FAR* pdateOut);

STDAPI VarCyFromI2(short sIn, CY FAR* pcyOut);
STDAPI VarCyFromI4(long lIn, CY FAR* pcyOut);
STDAPI VarCyFromR4(float fltIn, CY FAR* pcyOut);
STDAPI VarCyFromR8(double dblIn, CY FAR* pcyOut);
STDAPI VarCyFromDate(DATE dateIn, CY FAR* pcyOut);
STDAPI VarCyFromStr(char FAR* strIn, LCID lcid, CY FAR* pcyOut);
STDAPI VarCyFromDisp(IDispatch FAR* pdispIn, CY FAR* pcyOut);
STDAPI VarCyFromBool(VARIANT_BOOL boolIn, CY FAR* pcyOut);

STDAPI VarBstrFromI2(short iVal, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromI4(long lIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromR4(float fltIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromR8(double dblIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromCy(CY cyIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromDate(DATE dateIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromDisp(IDispatch FAR* pdispIn, LCID lcid, BSTR FAR* pbstrOut);
STDAPI VarBstrFromBool(VARIANT_BOOL boolIn, LCID lcid, BSTR FAR* pbstrOut);

STDAPI VarBoolFromI2(short sIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromI4(long lIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromR4(float fltIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromR8(double dblIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromDate(DATE dateIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromCy(CY cyIn, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromStr(char FAR* strIn, LCID lcid, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromDisp(IDispatch FAR* pdispIn, VARIANT_BOOL FAR* pboolOut);

#ifdef WIN32

STDAPI VarI2FromWStr(LPWSTR wstrIn, LCID lcid, short FAR* psOut);
STDAPI VarI2FromDispW(IDispatchW FAR* pdispwIn, short FAR* psOut);

STDAPI VarI4FromWStr(LPWSTR wstrIn, LCID lcid, long FAR* plOut);
STDAPI VarI4FromDispW(IDispatchW FAR* pdispwIn, long FAR* plOut);

STDAPI VarR4FromWStr(LPWSTR wstrIn, LCID lcid, float FAR* pfltOut);
STDAPI VarR4FromDispW(IDispatchW FAR* pdispwIn, float FAR* pfltOut);

STDAPI VarR8FromWStr(LPWSTR wstrIn, LCID lcid, double FAR* pdblOut);
STDAPI VarR8FromDispW(IDispatchW FAR* pdispwIn, double FAR* pdblOut);

STDAPI VarDateFromWStr(LPWSTR wstrIn, LCID lcid, DATE FAR* pdateOut);
STDAPI VarDateFromDispW(IDispatchW FAR* pdispwIn, DATE FAR* pdateOut);

STDAPI VarCyFromWStr(LPWSTR wstrIn, LCID lcid, CY FAR* pcyOut);
STDAPI VarCyFromDispW(IDispatchW FAR* pdispwIn, CY FAR* pcyOut);

STDAPI VarBstrFromDispW(IDispatchW FAR* pdispwIn, LCID lcid, BSTR FAR* pbstrOut);

STDAPI VarBoolFromWStr(LPWSTR wstrIn, LCID lcid, VARIANT_BOOL FAR* pboolOut);
STDAPI VarBoolFromDispW(IDispatchW FAR* pdispwIn, VARIANT_BOOL FAR* pboolOut);

STDAPI VarWBstrFromI2(short sIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromI4(long lIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromR4(float fltIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromR8(double dblIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromDate(DATE dateIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromCy(CY cyIn, LCID lcid, WBSTR FAR* pwbstr);
STDAPI VarWBstrFromDisp(IDispatch FAR* pdispIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromBool(VARIANT_BOOL boolIn, LCID lcid, WBSTR FAR* pwbstrOut);
STDAPI VarWBstrFromDispW(IDispatchW FAR* pdispwIn, LCID lcid, WBSTR FAR* pwbstrOut);

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                             ITypeLib                                */
/*---------------------------------------------------------------------*/

typedef enum tagSYSKIND {
      SYS_WIN16
    , SYS_WIN32
    , SYS_MAC
#ifdef _MAC
    , SYS_FORCELONG = 2147483647
#endif
} SYSKIND;

typedef struct FARSTRUCT tagTLIBATTR {
    GUID guid;			/* globally unique library id */
    LCID lcid;			/* locale of the TypeLibrary */
    SYSKIND syskind;
    unsigned short wMajorVerNum;/* major version number	*/
    unsigned short wMinorVerNum;/* minor version number	*/
} TLIBATTR, FAR* LPTLIBATTR;


#undef  INTERFACE
#define INTERFACE ITypeLib

DECLARE_INTERFACE_(ITypeLib, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeLib methods */
    STDMETHOD_(unsigned int,GetTypeInfoCount)(THIS) PURE;

    STDMETHOD(GetTypeInfo)(THIS_
      unsigned int index, ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetTypeInfoOfGuid)(THIS_
      REFGUID guid, ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetLibAttr)(THIS_
      TLIBATTR FAR* FAR* pptlibattr) PURE;

    STDMETHOD(GetTypeComp)(THIS_
      ITypeComp FAR* FAR* pptcomp) PURE;

    STDMETHOD(GetDocumentation)(THIS_
      int index,
      BSTR FAR* pbstrName,
      BSTR FAR* pbstrDocString,
      unsigned long FAR* pdwHelpContext,
      BSTR FAR* pbstrHelpFile) PURE;

    STDMETHOD(IsName)(THIS_
      char FAR* szNameBuf, unsigned short wHashVal, int FAR* pfName) PURE;

    STDMETHOD_(void, ReleaseTLibAttr)(THIS_ TLIBATTR FAR* ptlibattr) PURE;

    STDMETHOD(Load)(THIS_
      IStorage FAR* pstg, char FAR* szFileName) PURE;
};

typedef ITypeLib FAR* LPTYPELIB;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE ITypeLibW

DECLARE_INTERFACE_(ITypeLibW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeLib methods */
    STDMETHOD_(unsigned int,GetTypeInfoCount)(THIS) PURE;

    STDMETHOD(GetTypeInfo)(THIS_
      unsigned int index, ITypeInfoW FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetTypeInfoOfGuid)(THIS_
      REFGUID guid, ITypeInfoW FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetLibAttr)(THIS_
      TLIBATTR FAR* FAR* pptlibattr) PURE;

    STDMETHOD(GetTypeComp)(THIS_
      ITypeCompW FAR* FAR* pptcomp) PURE;

    STDMETHOD(GetDocumentation)(THIS_
      int index,
      WBSTR FAR* pwbstrName,
      WBSTR FAR* pwbstrDocString,
      unsigned long FAR* pdwHelpContext,
      WBSTR FAR* pwbstrHelpFile) PURE;

    STDMETHOD(IsName)(THIS_
      LPWSTR szNameBuf, unsigned short wHashVal, int FAR* pfName) PURE;

    STDMETHOD_(void, ReleaseTLibAttr)(THIS_ TLIBATTR FAR* ptlibattr) PURE;

    STDMETHOD(Load)(THIS_
      IStorage FAR* pstg, LPWSTR szFileName) PURE;
};

typedef ITypeLibW FAR* LPTYPELIBW;

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                            ITypeInfo                                */
/*---------------------------------------------------------------------*/


typedef long DISPID;
typedef DISPID MEMBERID;

#define MEMBERID_NIL DISPID_UNKNOWN
#define ID_DEFAULTINST	-2

typedef unsigned long HREFTYPE;

typedef enum tagTYPEKIND {
      TKIND_ENUM = 0
    , TKIND_RECORD
    , TKIND_MODULE
    , TKIND_INTERFACE
    , TKIND_DISPATCH
    , TKIND_COCLASS
    , TKIND_ALIAS
    , TKIND_UNION
    , TKIND_ENCUNION
    , TKIND_Class
    , TKIND_TYPE
    , TKIND_CONNECTOR
    , TKIND_MAX			/* end of enum marker */
#ifdef _MAC
    , TKIND_FORCELONG = 2147483647
#endif
} TYPEKIND;


typedef struct FARSTRUCT tagTYPEDESC {
    union {
      /* VT_PTR - the pointed-at type */
      struct FARSTRUCT tagTYPEDESC FAR* lptdesc;

      /* VT_CARRAY */
      struct FARSTRUCT tagARRAYDESC FAR* lpadesc;

      /* VT_FUNCPTR */
      struct FARSTRUCT tagFUNCDESC FAR* lpfdesc;

      /* VT_USERDEFINED - this is used to get a TypeInfo for the UDT */
      HREFTYPE hreftype;

    }UNION_NAME(u);
    VARTYPE vt;
} TYPEDESC;

typedef struct FARSTRUCT tagARRAYDESC {
    TYPEDESC tdescElem;		/* element type */
    unsigned short cDims;	/* dimension count */
    SAFEARRAYBOUND rgbounds[1];	/* variable length array of bounds */
} ARRAYDESC;

typedef struct FARSTRUCT tagIDLDESC {
    BSTR bstrIDLInfo;
    unsigned short wIDLFlags;	/* IN, OUT, etc */
} IDLDESC, FAR* LPIDLDESC;

#ifdef WIN32
typedef struct FARSTRUCT tagWIDLDESC {
    WBSTR wbstrIDLInfo;
    unsigned short wIDLFlags;	/* IN, OUT, etc */
} WIDLDESC, FAR* LPWIDLDESC;
#endif

#define IDLFLAG_NONE	0
#define IDLFLAG_FIN	0x1
#define IDLFLAG_FOUT	0x2
#define IDLFLAG_FSTRING	0x4

typedef struct FARSTRUCT tagELEMDESC {
    TYPEDESC tdesc;		/* the type of the element */
    IDLDESC idldesc;		/* info for remoting the element */ 
} ELEMDESC, FAR* LPELEMDESC;

#ifdef WIN32
typedef struct FARSTRUCT tagWELEMDESC {
    TYPEDESC tdesc;		/* the type of the element */
    WIDLDESC idldesc;		/* info for remoting the element */ 
} WELEMDESC, FAR* LPWELEMDESC;
#endif


typedef struct FARSTRUCT tagTYPEATTR {
    GUID guid;			/* the GUID of the TypeInfo */
    LCID lcid;			/* locale of member names and doc strings */
    unsigned long dwReserved;
    MEMBERID memidConstructor;	/* ID of constructor, MEMBERID_NIL if none */
    MEMBERID memidDestructor;	/* ID of destructor, MEMBERID_NIL if none */
    char FAR* lpstrSchema;	/* reserved for future use */
    unsigned long cbSizeInstance;/* the size of an instance of this type */
    TYPEKIND typekind;		/* the kind of type this typeinfo describes */
    unsigned short cFuncs;	/* number of functions */
    unsigned short cVars;	/* number of variables / data members */
    unsigned short cImplTypes;	/* number of implemented interfaces */
    unsigned short cbSizeVft;	/* the size of this types virtual func table */
    unsigned short cbAlignment;	/* the alignment for an instance of this type */
    unsigned short wTypeFlags;
    unsigned short wMajorVerNum;/* major version number */
    unsigned short wMinorVerNum;/* minor version number */
    TYPEDESC tdescAlias;	/* if typekind == TKIND_ALIAS this specifies
				   the type for which this type is an alias */
    IDLDESC idldescType;        /* IDL attributes of the described type */
} TYPEATTR, FAR* LPTYPEATTR;

#ifdef WIN32
typedef struct FARSTRUCT tagWTYPEATTR {
    GUID guid;
    LCID lcid;
    unsigned long dwReserved;
    MEMBERID memidConstructor;
    MEMBERID memidDestructor;
    LPWSTR lpwstrSchema;
    unsigned long cbSizeInstance;
    TYPEKIND typekind;
    unsigned short cFuncs;
    unsigned short cVars;
    unsigned short cImplTypes;
    unsigned short cbSizeVft;
    unsigned short cbAlignment;
    unsigned short wTypeFlags;
    unsigned short wMajorVerNum;
    unsigned short wMinorVerNum;
    TYPEDESC tdescAlias;
    WIDLDESC idldescType;
} WTYPEATTR, FAR* LPWTYPEATTR;
#endif

typedef struct FARSTRUCT tagDISPPARAMS{
    VARIANTARG FAR* rgvarg;
    DISPID FAR* rgdispidNamedArgs;
    unsigned int cArgs;
    unsigned int cNamedArgs;
} DISPPARAMS;


typedef struct FARSTRUCT tagEXCEPINFO {
    unsigned short wCode;             /* An error code describing the error. */
    unsigned short wReserved;

    BSTR bstrSource;	    /* A textual, human readable name of the
			       source of the exception. It is up to the
			       IDispatch implementor to fill this in.
			       Typically this will be an application name. */

    BSTR bstrDescription;   /* A textual, human readable description of the
			       error. If no description is available, NULL
			       should be used. */

    BSTR bstrHelpFile;      /* Fully qualified drive, path, and file name
			       of a help file with more information about
			       the error.  If no help is available, NULL
			       should be used. */

    unsigned long dwHelpContext;
			    /* help context of topic within the help file. */

    void FAR* pvReserved;

    /* Use of this field allows an application to defer filling in
       the bstrDescription, bstrHelpFile, and dwHelpContext fields
       until they are needed.  This field might be used, for example,
       if loading the string for the error is a time-consuming
       operation. If deferred fill-in is not desired, this field should
       be set to NULL. */
#ifdef _MAC
# ifdef _MSC_VER
    HRESULT (STDAPICALLTYPE FAR* pfnDeferredFillIn)(struct tagEXCEPINFO FAR*);
# else
    STDAPICALLTYPE HRESULT (FAR* pfnDeferredFillIn)(struct tagEXCEPINFO FAR*);
# endif
#else
    HRESULT (STDAPICALLTYPE FAR* pfnDeferredFillIn)(struct tagEXCEPINFO FAR*);
#endif

    unsigned long dwReserved;
} EXCEPINFO, FAR* LPEXCEPINFO;


#ifdef WIN32
typedef struct FARSTRUCT tagWEXCEPINFO {
    unsigned short wCode;
    unsigned short wReserved;
    WBSTR wbstrSource;	    
    WBSTR wbstrDescription;
    WBSTR wbstrHelpFile;
    unsigned long dwHelpContext;
    void FAR* pvReserved;
    HRESULT (STDAPICALLTYPE FAR* pfnDeferredFillIn)(struct tagWEXCEPINFO FAR*);
    unsigned long dwReserved;
} WEXCEPINFO, FAR* LPWEXCEPINFO;
#endif

typedef enum tagCALLCONV {
      CC_CDECL = 1
    , CC_MSCPASCAL
    , CC_PASCAL = CC_MSCPASCAL
    , CC_MACPASCAL
    , CC_STDCALL
    , CC_RESERVED
    , CC_SYSCALL
    , CC_MAX			/* end of enum marker */
#ifdef _MAC
    , CC_FORCELONG = 2147483647
#endif
} CALLCONV;

typedef enum tagFUNCKIND {
      FUNC_VIRTUAL
    , FUNC_PUREVIRTUAL
    , FUNC_NONVIRTUAL
    , FUNC_STATIC
    , FUNC_DISPATCH
#ifdef _MAC
    , FUNC_FORCELONG = 2147483647
#endif
} FUNCKIND;

/* Flags for IDispatch::Invoke */
#define DISPATCH_METHOD		0x1
#define DISPATCH_PROPERTYGET	0x2
#define DISPATCH_PROPERTYPUT	0x4
#define DISPATCH_PROPERTYPUTREF	0x8

typedef enum tagINVOKEKIND {
      INVOKE_FUNC = DISPATCH_METHOD
    , INVOKE_PROPERTYGET = DISPATCH_PROPERTYGET
    , INVOKE_PROPERTYPUT = DISPATCH_PROPERTYPUT
    , INVOKE_PROPERTYPUTREF = DISPATCH_PROPERTYPUTREF
#ifdef _MAC
    , INVOKE_FORCELONG = 2147483647
#endif
} INVOKEKIND;

typedef struct FARSTRUCT tagFUNCDESC {
    MEMBERID memid;
    SCODE FAR* lprgscode;
    ELEMDESC FAR* lprgelemdescParam;  /* array of parameter types */
    FUNCKIND funckind;
    INVOKEKIND invkind;
    CALLCONV callconv;
    short cParams;
    short cParamsOpt;
    short oVft;
    short cScodes;
    ELEMDESC elemdescFunc;
    unsigned short wFuncFlags;
} FUNCDESC, FAR* LPFUNCDESC;

#ifdef WIN32
typedef struct FARSTRUCT tagWFUNCDESC {
    MEMBERID memid;
    SCODE FAR* lprgscode;
    WELEMDESC FAR* lprgelemdescParam;  /* array of parameter types */
    FUNCKIND funckind;
    INVOKEKIND invkind;
    CALLCONV callconv;
    short cParams;
    short cParamsOpt;
    short oVft;
    short cScodes;
    WELEMDESC elemdescFunc;
    unsigned short wFuncFlags;
} WFUNCDESC, FAR* LPWFUNCDESC;
#endif

typedef enum tagVARKIND {
      VAR_PERINSTANCE
    , VAR_STATIC
    , VAR_CONST
    , VAR_DISPATCH
#ifdef _MAC
    , VAR_FORCELONG = 2147483647
#endif
} VARKIND;

typedef enum tagIMPLTYPEFLAGS {
      IMPL_NONE      = 0
    , IMPL_PLUG      = 1
    , IMPL_JACK      = 2
    , IMPL_OPTIONAL  = 4
    , IMPL_COCLASS   = 8
#ifdef _MAC
    , IMPL_FORCELONG = 2147483647
#endif
} IMPLTYPEFLAGS;

typedef enum tagVARDESCFLAGS {
      VDESC_NONE     = 0
    , VDESC_OPTIONAL = 1
#ifdef _MAC
    , VDESC_FORCELONG= 2147483647
#endif
} VARDESCFLAGS;

typedef struct FARSTRUCT tagVARDESC {
    MEMBERID memid;
    char FAR* lpstrSchema;		/* reserved for future use */
    union {
      /* VAR_PERINSTANCE - the offset of this variable within the instance */
      unsigned long oInst;

      /* VAR_CONST - the value of the constant */
      VARIANT FAR* lpvarValue;

    }UNION_NAME(u);
    ELEMDESC elemdescVar;
    unsigned short wVarFlags;
    VARKIND varkind;
} VARDESC, FAR* LPVARDESC;

#ifdef WIN32
typedef struct FARSTRUCT tagWVARDESC {
    MEMBERID memid;
    LPWSTR lpwstrSchema;	/* reserved for future use */
    union {
      unsigned long oInst;
      VARIANT FAR* lpvarValue;
    }UNION_NAME(u);
    WELEMDESC elemdescVar;
    unsigned short wVarFlags;
    VARKIND varkind;
} WVARDESC, FAR* LPWVARDESC;
#endif

typedef enum tagTYPEFLAGS {
      TYPEFLAG_FAPPOBJECT = 1
    , TYPEFLAG_FCANCREATE = 2
#ifdef _MAC
    , TYPEFLAG_FORCELONG  = 2147483647
#endif
} TYPEFLAGS;

typedef enum tagFUNCFLAGS {
      FUNCFLAG_FRESTRICTED= 1
    , FUNCFLAG_FOPTIONAL  = 2
#ifdef _MAC
    , FUNCFLAG_FORCELONG  = 2147483647
#endif
} FUNCFLAGS;

typedef enum tagVARFLAGS {
      VARFLAG_FREADONLY   = 1
#ifdef _MAC
    , VARFLAG_FORCELONG   = 2147483647
#endif
} VARFLAGS;


#undef  INTERFACE
#define INTERFACE ITypeInfo

DECLARE_INTERFACE_(ITypeInfo, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeInfo methods */
    STDMETHOD(GetTypeAttr)(THIS_ TYPEATTR FAR* FAR* pptypeattr) PURE;

    STDMETHOD(GetTypeComp)(THIS_ ITypeComp FAR* FAR* pptcomp) PURE;

    STDMETHOD(GetFuncDesc)(THIS_
      unsigned int index, FUNCDESC FAR* FAR* ppfuncdesc) PURE;

    STDMETHOD(GetVarDesc)(THIS_
      unsigned int index, VARDESC FAR* FAR* ppvardesc) PURE;

    STDMETHOD(GetNames)(THIS_
      MEMBERID memid,
      BSTR FAR* rgbstrNames,
      unsigned int cMaxNames,
      unsigned int FAR* pcNames) PURE;

    STDMETHOD(GetRefTypeOfImplType)(THIS_
      unsigned int index, HREFTYPE FAR* phreftype) PURE;

    STDMETHOD(GetImplTypeFlags)(THIS_
      unsigned int index, IMPLTYPEFLAGS FAR* pimpltypeflags) PURE;

    STDMETHOD(GetIDsOfNames)(THIS_
      char FAR* FAR* rgszNames,
      unsigned int cNames,
      MEMBERID FAR* rgmemid) PURE;

    STDMETHOD(Invoke)(THIS_
      void FAR* pvInstance,
      MEMBERID memid,
      unsigned short wFlags,
      DISPPARAMS FAR *pdispparams,
      VARIANT FAR *pvarResult,
      EXCEPINFO FAR *pexcepinfo,
      unsigned int FAR *puArgErr) PURE;

    STDMETHOD(GetDocumentation)(THIS_
      MEMBERID memid,
      BSTR FAR* pbstrName,
      BSTR FAR* pbstrDocString,
      unsigned long FAR* pdwHelpContext,
      BSTR FAR* pbstrHelpFile) PURE;

    STDMETHOD(GetDllEntry)(THIS_
      MEMBERID memid,
      BSTR FAR* pbstrDllName,
      BSTR FAR* pbstrName,
      unsigned short FAR* pwOrdinal) PURE;

    STDMETHOD(GetRefTypeInfo)(THIS_
      HREFTYPE hreftype, ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(AddressOfMember)(THIS_
      MEMBERID memid, INVOKEKIND invkind, void FAR* FAR* ppv) PURE;

    STDMETHOD(CreateInstance)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;

    STDMETHOD(GetMops)(THIS_ MEMBERID memid, BSTR FAR* pbstrMops) PURE;

    STDMETHOD(GetContainingTypeLib)(THIS_
      ITypeLib FAR* FAR* pptlib, unsigned int FAR* pindex) PURE;

    STDMETHOD_(void, ReleaseTypeAttr)(THIS_ TYPEATTR FAR* ptypeattr) PURE;
    STDMETHOD_(void, ReleaseFuncDesc)(THIS_ FUNCDESC FAR* pfuncdesc) PURE;
    STDMETHOD_(void, ReleaseVarDesc)(THIS_ VARDESC FAR* pvardesc) PURE;
};

typedef ITypeInfo FAR* LPTYPEINFO;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE ITypeInfoW

DECLARE_INTERFACE_(ITypeInfoW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeInfo methods */
    STDMETHOD(GetTypeAttr)(THIS_ WTYPEATTR FAR* FAR* pptypeattr) PURE;

    STDMETHOD(GetTypeComp)(THIS_ ITypeCompW FAR* FAR* pptcomp) PURE;

    STDMETHOD(GetFuncDesc)(THIS_
      unsigned int index, WFUNCDESC FAR* FAR* ppfuncdesc) PURE;

    STDMETHOD(GetVarDesc)(THIS_
      unsigned int index, WVARDESC FAR* FAR* ppvardesc) PURE;

    STDMETHOD(GetNames)(THIS_
      MEMBERID memid,
      WBSTR FAR* rgwbstrNames,
      unsigned int cMaxNames,
      unsigned int FAR* pcNames) PURE;

    STDMETHOD(GetRefTypeOfImplType)(THIS_
      unsigned int index, HREFTYPE FAR* phreftype) PURE;

    STDMETHOD(GetImplTypeFlags)(THIS_
      unsigned int index, IMPLTYPEFLAGS FAR* pimpltypeflags) PURE;

    STDMETHOD(GetIDsOfNames)(THIS_
      WCHAR FAR* FAR* rgszNames,
      unsigned int cNames,
      MEMBERID FAR* rgmemid) PURE;

    STDMETHOD(Invoke)(THIS_
      void FAR* pvInstance,
      MEMBERID memid,
      unsigned short wFlags,
      DISPPARAMS FAR *pdispparams,
      VARIANT FAR *pvarResult,
      WEXCEPINFO FAR *pexcepinfo,
      unsigned int FAR *puArgErr) PURE;

    STDMETHOD(GetDocumentation)(THIS_
      MEMBERID memid,
      WBSTR FAR* pwbstrName,
      WBSTR FAR* pwbstrDocString,
      unsigned long FAR* pdwHelpContext,
      WBSTR FAR* pwbstrHelpFile) PURE;

    STDMETHOD(GetDllEntry)(THIS_
      MEMBERID memid,
      WBSTR FAR* pwbstrDllName,
      WBSTR FAR* pwbstrName,
      unsigned short FAR* pwOrdinal) PURE;

    STDMETHOD(GetRefTypeInfo)(THIS_
      HREFTYPE hreftype, ITypeInfoW FAR* FAR* pptinfo) PURE;

    STDMETHOD(AddressOfMember)(THIS_
      MEMBERID memid, INVOKEKIND invkind, void FAR* FAR* ppv) PURE;

    STDMETHOD(CreateInstance)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;

    STDMETHOD(GetMops)(THIS_ MEMBERID memid, WBSTR FAR* pwbstrMops) PURE;

    STDMETHOD(GetContainingTypeLib)(THIS_
      ITypeLibW FAR* FAR* pptlib, unsigned int FAR* pindex) PURE;

    STDMETHOD_(void, ReleaseTypeAttr)(THIS_ WTYPEATTR FAR* ptypeattr) PURE;
    STDMETHOD_(void, ReleaseFuncDesc)(THIS_ WFUNCDESC FAR* pfuncdesc) PURE;
    STDMETHOD_(void, ReleaseVarDesc)(THIS_ WVARDESC FAR* pvardesc) PURE;
};

typedef ITypeInfoW FAR* LPTYPEINFOW;

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                            ITypeComp                                */
/*---------------------------------------------------------------------*/

typedef enum tagDESCKIND {
      DESCKIND_NONE = 0
    , DESCKIND_FUNCDESC
    , DESCKIND_VARDESC
    , DESCKIND_TYPECOMP
    , DESCKIND_IMPLICITAPPOBJ
    , DESCKIND_MAX		/* end of enum marker */
#ifdef _MAC
    , DESCKIND_FORCELONG = 2147483647
#endif
} DESCKIND;

typedef union tagBINDPTR {
    FUNCDESC FAR* lpfuncdesc;
    VARDESC FAR* lpvardesc;
    ITypeComp FAR* lptcomp;
} BINDPTR, FAR* LPBINDPTR;


#undef  INTERFACE
#define INTERFACE ITypeComp

DECLARE_INTERFACE_(ITypeComp, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeComp methods */
    STDMETHOD(Bind)(THIS_
      char FAR* szName,
      unsigned short wHashVal,
      unsigned short wflags,
      ITypeInfo FAR* FAR* pptinfo,
      DESCKIND FAR* pdesckind,
      BINDPTR FAR* pbindptr) PURE;

    STDMETHOD(BindType)(THIS_
      char FAR* szName,
      unsigned short wHashVal,
      ITypeInfo FAR* FAR* pptinfo,
      ITypeComp FAR* FAR* pptcomp) PURE;
};

typedef ITypeComp FAR* LPTYPECOMP;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE ITypeCompW

DECLARE_INTERFACE_(ITypeCompW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ITypeComp methods */
    STDMETHOD(Bind)(THIS_
      LPWSTR szName,
      unsigned short wHashVal,
      unsigned short wflags,
      ITypeInfoW FAR* FAR* pptinfo,
      DESCKIND FAR* pdesckind,
      BINDPTR FAR* pbindptr) PURE;

    STDMETHOD(BindType)(THIS_
      LPWSTR szName,
      unsigned short wHashVal,
      ITypeInfoW FAR* FAR* pptinfo,
      ITypeCompW FAR* FAR* pptcomp) PURE;
};

typedef ITypeCompW FAR* LPWTYPECOMP;

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                         ICreateTypeLib                              */
/*---------------------------------------------------------------------*/


#undef  INTERFACE
#define INTERFACE ICreateTypeLib

DECLARE_INTERFACE_(ICreateTypeLib, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ICreateTypeLib methods */
    STDMETHOD(CreateTypeInfo)(THIS_
      char FAR* szName,
      TYPEKIND tkind,
      ICreateTypeInfo FAR* FAR* lplpictinfo) PURE;

    STDMETHOD(SetName)(THIS_ char FAR* szName) PURE;

    STDMETHOD(SetVersion)(THIS_
      unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

    STDMETHOD(SetGuid) (THIS_ REFGUID guid) PURE;

    STDMETHOD(SetDocString)(THIS_ char FAR* szDoc) PURE;

    STDMETHOD(SetHelpFileName)(THIS_ char FAR* szHelpFileName) PURE;

    STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

    STDMETHOD(SetLcid)(THIS_ LCID lcid) PURE;

    STDMETHOD(SaveAllChanges)(THIS_ IStorage FAR* pstg) PURE;
};

typedef ICreateTypeLib FAR* LPCREATETYPELIB;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE ICreateTypeLibW

DECLARE_INTERFACE_(ICreateTypeLibW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ICreateTypeLib methods */
    STDMETHOD(CreateTypeInfo)(THIS_
      LPWSTR szName,
      TYPEKIND tkind,
      ICreateTypeInfoW FAR* FAR* lplpictinfo) PURE;

    STDMETHOD(SetName)(THIS_ LPWSTR szName) PURE;

    STDMETHOD(SetVersion)(THIS_
      unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

    STDMETHOD(SetGuid) (THIS_ REFGUID guid) PURE;

    STDMETHOD(SetDocString)(THIS_ LPWSTR szDoc) PURE;

    STDMETHOD(SetHelpFileName)(THIS_ LPWSTR szHelpFileName) PURE;

    STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

    STDMETHOD(SetLcid)(THIS_ LCID lcid) PURE;

    STDMETHOD(SaveAllChanges)(THIS_ IStorage FAR* pstg) PURE;
};

typedef ICreateTypeLibW FAR* LPCREATETYPELIBW;

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                         ICreateTypeInfo                             */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE ICreateTypeInfo

DECLARE_INTERFACE_(ICreateTypeInfo, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ICreateTypeInfo methods */
    STDMETHOD(SetGuid)(THIS_ REFGUID guid) PURE;

    STDMETHOD(SetTypeFlags)(THIS_ unsigned int uTypeFlags) PURE;

    STDMETHOD(SetDocString)(THIS_ char FAR* pstrDoc) PURE;

    STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

    STDMETHOD(SetVersion)(THIS_
      unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

    STDMETHOD(AddRefTypeInfo)(THIS_
      ITypeInfo FAR* ptinfo, HREFTYPE FAR* phreftype) PURE;

    STDMETHOD(AddFuncDesc)(THIS_
      unsigned int index, FUNCDESC FAR* pfuncdesc) PURE;

    STDMETHOD(AddImplType)(THIS_
      unsigned int index, HREFTYPE hreftype) PURE;

    STDMETHOD(SetImplTypeFlags)(THIS_
      unsigned int index, IMPLTYPEFLAGS impltypeflags) PURE;

    STDMETHOD(SetAlignment)(THIS_ unsigned short cbAlignment) PURE;

    STDMETHOD(SetSchema)(THIS_ char FAR* lpstrSchema) PURE;

    STDMETHOD(AddVarDesc)(THIS_
      unsigned int index, VARDESC FAR* pvardesc) PURE;

    STDMETHOD(SetFuncAndParamNames)(THIS_
      unsigned int index, char FAR* FAR* rgszNames, unsigned int cNames) PURE;

    STDMETHOD(SetVarName)(THIS_
      unsigned int index, char FAR* szName) PURE;

    STDMETHOD(SetTypeDescAlias)(THIS_
      TYPEDESC FAR* ptdescAlias) PURE;

    STDMETHOD(DefineFuncAsDllEntry)(THIS_
      unsigned int index, char FAR* szDllName, char FAR* szProcName) PURE;

    STDMETHOD(SetFuncDocString)(THIS_
      unsigned int index, char FAR* szDocString) PURE;

    STDMETHOD(SetVarDocString)(THIS_
      unsigned int index, char FAR* szDocString) PURE;

    STDMETHOD(SetFuncHelpContext)(THIS_
      unsigned int index, unsigned long dwHelpContext) PURE;

    STDMETHOD(SetVarHelpContext)(THIS_
      unsigned int index, unsigned long dwHelpContext) PURE;

    STDMETHOD(SetMops)(THIS_
      unsigned int index, BSTR bstrMops) PURE;

    STDMETHOD(SetTypeIdldesc)(THIS_
      IDLDESC FAR* pidldesc) PURE;

    STDMETHOD(LayOut)(THIS) PURE;
};

typedef ICreateTypeInfo FAR* LPCREATETYPEINFO;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE ICreateTypeInfoW

DECLARE_INTERFACE_(ICreateTypeInfoW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* ICreateTypeInfo methods */
    STDMETHOD(SetGuid)(THIS_ REFGUID guid) PURE;

    STDMETHOD(SetTypeFlags)(THIS_ unsigned int uTypeFlags) PURE;

    STDMETHOD(SetDocString)(THIS_ LPWSTR pstrDoc) PURE;

    STDMETHOD(SetHelpContext)(THIS_ unsigned long dwHelpContext) PURE;

    STDMETHOD(SetVersion)(THIS_
      unsigned short wMajorVerNum, unsigned short wMinorVerNum) PURE;

    STDMETHOD(AddRefTypeInfo)(THIS_
      ITypeInfoW FAR* ptinfo, HREFTYPE FAR* phreftype) PURE;

    STDMETHOD(AddFuncDesc)(THIS_
      unsigned int index, WFUNCDESC FAR* pfuncdesc) PURE;

    STDMETHOD(AddImplType)(THIS_
      unsigned int index, HREFTYPE hreftype) PURE;

    STDMETHOD(SetImplTypeFlags)(THIS_
      unsigned int index, IMPLTYPEFLAGS impltypeflags) PURE;

    STDMETHOD(SetAlignment)(THIS_ unsigned short cbAlignment) PURE;

    STDMETHOD(SetSchema)(THIS_ LPWSTR lpwstrSchema) PURE;

    STDMETHOD(AddVarDesc)(THIS_
      unsigned int index, WVARDESC FAR* pvardesc) PURE;

    STDMETHOD(SetFuncAndParamNames)(THIS_
      unsigned int index, LPWSTR FAR* rgszNames, unsigned int cNames) PURE;

    STDMETHOD(SetVarName)(THIS_
      unsigned int index, LPWSTR szName) PURE;

    STDMETHOD(SetTypeDescAlias)(THIS_
      TYPEDESC FAR* ptdescAlias) PURE;

    STDMETHOD(DefineFuncAsDllEntry)(THIS_
      unsigned int index, LPWSTR szDllName, char FAR* szProcName) PURE;

    STDMETHOD(SetFuncDocString)(THIS_
      unsigned int index, LPWSTR szDocString) PURE;

    STDMETHOD(SetVarDocString)(THIS_
      unsigned int index, LPWSTR szDocString) PURE;

    STDMETHOD(SetFuncHelpContext)(THIS_
      unsigned int index, unsigned long dwHelpContext) PURE;

    STDMETHOD(SetVarHelpContext)(THIS_
      unsigned int index, unsigned long dwHelpContext) PURE;

    STDMETHOD(SetMops)(THIS_
      unsigned int index, WBSTR bstrMops) PURE;

    STDMETHOD(SetTypeIdldesc)(THIS_
      WIDLDESC FAR* pidldesc) PURE;

    STDMETHOD(LayOut)(THIS) PURE;
};

typedef ICreateTypeInfoW FAR* LPCREATETYPEINFOW;

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                         TypeInfo APIs                               */
/*---------------------------------------------------------------------*/

/* compute a 16bit hash value for the given name
 */
STDAPI_(unsigned short)
WHashValOfName(char FAR* szName);

/* load the typelib from the file with the given filename
 */
STDAPI
LoadTypeLib(char FAR* szFile, ITypeLib FAR* FAR* pptlib);

/* load registered typelib
 */
STDAPI
LoadRegTypeLib(
    REFGUID rguid,
    unsigned short wVerMajor,
    unsigned short wVerMinor,
    LCID lcid,
    ITypeLib FAR* FAR* pptlib);

/* add typelib to registry
 */
STDAPI
RegisterTypeLib(
    ITypeLib FAR* ptlib,
    char FAR* szFullPath,
    char FAR* szHelpDir);

/* remove typelib from registry
 */
STDAPI
DeregisterTypeLib(
    REFGUID rguid,
    unsigned short wVerMajor,
    unsigned short wVerMinor,
    LCID lcid);

STDAPI
CreateTypeLib(SYSKIND syskind, ICreateTypeLib FAR* FAR* ppctlib);


#ifdef WIN32

STDAPI_(unsigned short)
WHashValOfNameW(LPWSTR szName);

STDAPI
LoadTypeLibW(LPWSTR szFile, ITypeLibW FAR* FAR* pptlib);

STDAPI
LoadRegTypeLibW(
    REFGUID rguid,
    unsigned short wVerMajor,
    unsigned short wVerMinor,
    LCID lcid,
    ITypeLibW FAR* FAR* pptlib);

STDAPI
RegisterTypeLibW(
    ITypeLibW FAR* ptlib,
    LPWSTR szFullPath,
    LPWSTR szHelpDir);

STDAPI
CreateTypeLibW(SYSKIND syskind, ICreateTypeLibW FAR* FAR* ppctlib);

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                          IEnumVARIANT                               */
/*---------------------------------------------------------------------*/

#undef  INTERFACE
#define INTERFACE IEnumVARIANT

DECLARE_INTERFACE_(IEnumVARIANT, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* IEnumVARIANT methods */
    STDMETHOD(Next)(
      THIS_ unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched) PURE;
    STDMETHOD(Skip)(THIS_ unsigned long celt) PURE;
    STDMETHOD(Reset)(THIS) PURE;
    STDMETHOD(Clone)(THIS_ IEnumVARIANT FAR* FAR* ppenum) PURE;
};

typedef IEnumVARIANT FAR* LPENUMVARIANT;


/*---------------------------------------------------------------------*/
/*                             IDispatch                               */
/*---------------------------------------------------------------------*/


#undef  INTERFACE
#define INTERFACE IDispatch

DECLARE_INTERFACE_(IDispatch, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ unsigned int FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      unsigned int itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      char FAR* FAR* rgszNames,
      unsigned int cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      unsigned short wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      unsigned int FAR* puArgErr) PURE;
};

typedef IDispatch FAR* LPDISPATCH;


#ifdef WIN32

#undef  INTERFACE
#define INTERFACE IDispatchW

DECLARE_INTERFACE_(IDispatchW, IUnknown)
{
    BEGIN_INTERFACE

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void FAR* FAR* ppvObj) PURE;
    STDMETHOD_(unsigned long, AddRef)(THIS) PURE;
    STDMETHOD_(unsigned long, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ unsigned int FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      unsigned int itinfo,
      LCID lcid,
      ITypeInfoW FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      WCHAR FAR* FAR* rgszNames,
      unsigned int cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      unsigned short wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      WEXCEPINFO FAR* pexcepinfo,
      unsigned int FAR* puArgErr) PURE;
};

typedef IDispatchW FAR* LPWDISPATCH;

#endif /* WIN32 */

/* DISPID reserved for the standard "value" property */
#define DISPID_VALUE	0

/* DISPID reserved to indicate an "unknown" name */
#define DISPID_UNKNOWN	-1

/* The following DISPID is reserved to indicate the param
 * that is the right-hand-side (or "put" value) of a PropertyPut
 */
#define DISPID_PROPERTYPUT -3

/* DISPID reserved for the standard "NewEnum" method */
#define DISPID_NEWENUM	-4

/* DISPID reserved for the standard "Evaluate" method */
#define DISPID_EVALUATE	-5


/*---------------------------------------------------------------------*/
/*                   IDispatch implementation support                  */
/*---------------------------------------------------------------------*/

typedef struct FARSTRUCT tagPARAMDATA {
    char FAR* szName;		/* parameter name */
    VARTYPE vt;			/* parameter type */
} PARAMDATA, FAR* LPPARAMDATA;

typedef struct FARSTRUCT tagMETHODDATA {
    char FAR* szName;		/* method name */
    PARAMDATA FAR* ppdata;	/* pointer to an array of PARAMDATAs */
    DISPID dispid;		/* method ID */
    unsigned int iMeth;		/* method index */
    CALLCONV cc;		/* calling convention */
    unsigned int cArgs;		/* count of arguments */
    unsigned short wFlags;	/* same wFlags as on IDispatch::Invoke() */
    VARTYPE vtReturn;
} METHODDATA, FAR* LPMETHODDATA;

typedef struct FARSTRUCT tagINTERFACEDATA {
    METHODDATA FAR* pmethdata;	/* pointer to an array of METHODDATAs */
    unsigned int cMembers;	/* count of members */
} INTERFACEDATA, FAR* LPINTERFACEDATA;



/* Locate the parameter indicated by the given position, and
 * return it coerced to the given target VARTYPE (vtTarg).
 */
STDAPI
DispGetParam(
    DISPPARAMS FAR* pdispparams,
    unsigned int position,
    VARTYPE vtTarg,
    VARIANT FAR* pvarResult,
    unsigned int FAR* puArgErr);

/* Automatic TypeInfo driven implementation of IDispatch::GetIDsOfNames()
 */ 
STDAPI
DispGetIDsOfNames(
    ITypeInfo FAR* ptinfo,
    char FAR* FAR* rgszNames,
    unsigned int cNames,
    DISPID FAR* rgdispid);

/* Automatic TypeInfo driven implementation of IDispatch::Invoke()
 */
STDAPI
DispInvoke(
    void FAR* _this,
    ITypeInfo FAR* ptinfo,
    DISPID dispidMember,
    unsigned short wFlags,
    DISPPARAMS FAR* pparams,
    VARIANT FAR* pvarResult,
    EXCEPINFO FAR* pexcepinfo,
    unsigned int FAR* puArgErr);

/* Construct a TypeInfo from an interface data description
 */
STDAPI
CreateDispTypeInfo(
    INTERFACEDATA FAR* pidata,
    LCID lcid,
    ITypeInfo FAR* FAR* pptinfo);

/* Create an instance of the standard TypeInfo driven IDispatch
 * implementation.
 */
STDAPI
CreateStdDispatch(
    IUnknown FAR* punkOuter,
    void FAR* pvThis,
    ITypeInfo FAR* ptinfo,
    IUnknown FAR* FAR* ppunkStdDisp);


#ifdef WIN32

typedef struct FARSTRUCT tagWPARAMDATA {
    WCHAR FAR* szName;		/* parameter name */
    VARTYPE vt;			/* parameter type */
} WPARAMDATA, FAR* LPWPARAMDATA;

typedef struct FARSTRUCT tagWMETHODDATA {
    WCHAR FAR* szName;		/* method name */
    WPARAMDATA FAR* ppdata;	/* pointer to an array of PARAMDATAs */
    DISPID dispid;		/* method ID */
    unsigned int iMeth;		/* method index */
    CALLCONV cc;		/* calling convention */
    unsigned int cArgs;		/* count of arguments */
    unsigned short wFlags;		/* same wFlags as on IDispatch::Invoke() */
    VARTYPE vtReturn;
} WMETHODDATA, FAR* LPWMETHODDATA;

typedef struct FARSTRUCT tagWINTERFACEDATA {
    WMETHODDATA FAR* pmethdata;	/* pointer to an array of METHODDATAs */
    unsigned int cMembers;		/* count of members */
} WINTERFACEDATA, FAR* LPWINTERFACEDATA;


/* Locate the parameter indicated by the given position, and
 * return it coerced to the given target VARTYPE (vtTarg).
 */
STDAPI
DispGetParamW(
    DISPPARAMS FAR* pdispparams,
    unsigned int position,
    VARTYPE vtTarg,
    VARIANT FAR* pvarResult,
    unsigned int FAR* puArgErr);

/* Automatic TypeInfo driven implementation of IDispatch::GetIDsOfNames()
 */ 
STDAPI
DispGetIDsOfNamesW(
    ITypeInfoW FAR* ptinfo,
    WCHAR FAR* FAR* rgszNames,
    unsigned int cNames,
    DISPID FAR* rgdispid);

/* Automatic TypeInfo driven implementation of IDispatch::Invoke()
 */
STDAPI
DispInvokeW(
    void FAR* _this,
    ITypeInfoW FAR* ptinfo,
    DISPID dispidMember,
    unsigned short wFlags,
    DISPPARAMS FAR* pparams,
    VARIANT FAR* pvarResult,
    WEXCEPINFO FAR* pexcepinfo,
    unsigned int FAR* puArgErr);

/* Construct a TypeInfo from an interface data description
 */
STDAPI
CreateDispTypeInfoW(
    WINTERFACEDATA FAR* pidata,
    LCID lcid,
    ITypeInfoW FAR* FAR* pptinfo);

/* Create an instance of the standard TypeInfo driven IDispatch
 * implementation.
 */
STDAPI
CreateStdDispatchW(
    IUnknown FAR* punkOuter,
    void FAR* pvThis,
    ITypeInfoW FAR* ptinfo,
    IUnknown FAR* FAR* ppunkStdDispW);

#endif /* WIN32 */


/*---------------------------------------------------------------------*/
/*                    Active Object Registration API                   */
/*---------------------------------------------------------------------*/

STDAPI
RegisterActiveObject(
    IUnknown FAR* punk,
    REFCLSID rclsid,
    void FAR* pvReserved,
    unsigned long FAR* pdwRegister);

STDAPI
RevokeActiveObject(
    unsigned long dwRegister,
    void FAR* pvReserved);

STDAPI
GetActiveObject(
    REFCLSID rclsid,
    void FAR* pvReserved,
    IUnknown FAR* FAR* ppunk);


#undef UNION_NAME

#endif /* _DISPATCH_H_ */
