/*************************************************************************
**
**    OLE 2.0 Standard Utilities
**
**    olestd.h
**
**    This file contains file contains data structure defintions,
**    function prototypes, constants, etc. for the common OLE 2.0
**    utilities.
**    These utilities include the following:
**          Debuging Assert/Verify macros
**          HIMETRIC conversion routines
**          reference counting debug support
**          OleStd API's for common compound-document app support
**
**    (c) Copyright Microsoft Corp. 1990 - 1994 All Rights Reserved
**
*************************************************************************/


#ifndef _OLESTD_H_
#define _OLESTD_H_


#include <Dialogs.h>
#include <Events.h>

#ifdef _DEBUG
#include "uidebug.h"
#endif


//Hook type used in all structures.
typedef unsigned int (*LPFNOLEUIHOOK)(DialogPtr, EventRecord *, short *, long);

// String table defines...
#define  IDS_OLESTDNOCREATEFILE   700
#define  IDS_OLESTDNOOPENFILE     701
#define  IDS_OLESTDDISKFULL       702


/*
 * Some C interface declaration stuff
 */

#if ! defined(__cplusplus)
typedef struct tagINTERFACEIMPL {
        IUnknownVtbl*			lpVtbl;
        void*                   lpBack;
        int                     cRef;   // interface specific ref count.
} INTERFACEIMPL, *LPINTERFACEIMPL;

#define INIT_INTERFACEIMPL(lpIFace, pVtbl, pBack)   \
        ((lpIFace)->lpVtbl = pVtbl, \
            ((LPINTERFACEIMPL)(lpIFace))->lpBack = (void*)pBack,   \
            ((LPINTERFACEIMPL)(lpIFace))->cRef = 0  \
        )

#if defined( _DEBUG )
#define OleDbgQueryInterfaceMethod(lpUnk)   \
        ((lpUnk) != NULL ? ((LPINTERFACEIMPL)(lpUnk))->cRef++ : 0)
#define OleDbgAddRefMethod(lpThis, iface)   \
        ((LPINTERFACEIMPL)(lpThis))->cRef++

#if _DEBUGLEVEL >= 2
#define OleDbgReleaseMethod(lpThis, iface) \
        (--((LPINTERFACEIMPL)(lpThis))->cRef == 0 ? \
            OleDbgOut("\t" iface "* RELEASED (cRef == 0)\n"),1 : \
             (((LPINTERFACEIMPL)(lpThis))->cRef < 0) ? \
                ( \
                    DebugBreak(), \
                    OleDbgOut(  \
                        "\tERROR: " iface "* RELEASED TOO MANY TIMES\n") \
                ),1 : \
                1)

#else       // if _DEBUGLEVEL < 2
#define OleDbgReleaseMethod(lpThis, iface) \
        (--((LPINTERFACEIMPL)(lpThis))->cRef == 0 ? \
            1 : \
             (((LPINTERFACEIMPL)(lpThis))->cRef < 0) ? \
                ( \
                    OleDbgOut(  \
                        "\tERROR: " iface "* RELEASED TOO MANY TIMES\n") \
        ),1 : \
                1)

#endif      // if _DEBUGLEVEL < 2

#else       // ! defined (_DEBUG)

#define OleDbgQueryInterfaceMethod(lpUnk)
#define OleDbgAddRefMethod(lpThis, iface)
#define OleDbgReleaseMethod(lpThis, iface)

#endif      // if defined( _DEBUG )

#endif      // ! defined(__cplusplus)

/*
 * Some docfiles stuff
 */

#define STGM_DFRALL (STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_DENY_WRITE)
#define STGM_DFALL (STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE)
#define STGM_SALL (STGM_READWRITE | STGM_SHARE_EXCLUSIVE)

/*
 * Some moniker stuff
 */

// Delimeter used to separate ItemMoniker pieces of a composite moniker
#if defined( _MAC )
#define OLESTDDELIM ":"
#else
#define OLESTDDELIM "\\"
#endif

/*
 * Some Concurrency stuff
 */

/* standard Delay (in 60th secs) to wait before retrying an LRPC call.
**    this value is returned from IMessageFilter::RetryRejectedCall
*/
#define OLESTDRETRYDELAY    (unsigned long)900

/* Cancel the pending outgoing LRPC call.
**    this value is returned from IMessageFilter::RetryRejectedCall
*/
#define OLESTDCANCELRETRY   (unsigned long)-1

/*
 * Some Clipboard Copy/Paste & Drag/Drop support stuff
 */

//Macro to set all FormatEtc fields
#define SETFORMATETC(fe, cf, asp, td, med, li)   \
    ((fe).cfFormat=cf, \
     (fe).dwAspect=asp, \
     (fe).ptd=td, \
     (fe).tymed=med, \
     (fe).lindex=li)

//Macro to set interesting FormatEtc fields defaulting the others.
#define SETDEFAULTFORMATETC(fe, cf, med)  \
    ((fe).cfFormat=cf, \
     (fe).dwAspect=DVASPECT_CONTENT, \
     (fe).ptd=NULL, \
     (fe).tymed=med, \
     (fe).lindex=-1)

// Macro to test if two FormatEtc structures match
#define IsEqualFORMATETC(fe1, fe2)  \
    ((fe1).cfFormat == (fe2).cfFormat \
     && ((fe1).tymed & (fe2).tymed) \
     && (!(fe1).dwAspect||!(fe2).dwAspect||((fe1).dwAspect&(fe2).dwAspect)) \
     && (fe1).ptd == (fe2).ptd)

#ifdef __UNUSED__
// Clipboard format strings
#define CF_EMBEDSOURCE      "Embed Source"
#define CF_EMBEDDEDOBJECT   "Embedded Object"
#define CF_LINKSOURCE       "Link Source"
#define CF_OBJECTDESCRIPTOR "Object Descriptor"
#define CF_LINKSRCDESCRIPTOR "Link Source Descriptor"
#define CF_OWNERLINK        "OwnerLink"
#define CF_FILENAME         "FileName"
#endif

/* The OLEUIPASTEFLAG enumeration is used by the OLEUIPASTEENTRY structure.
 *
 * OLEUIPASTE_ENABLEICON    If the container does not specify this flag for the entry in the
 *   OLEUIPASTEENTRY array passed as input to OleUIPasteSpecial, the DisplayAsIcon button will be
 *   unchecked and disabled when the the user selects the format that corresponds to the entry.
 *
 * OLEUIPASTE_PASTEONLY     Indicates that the entry in the OLEUIPASTEENTRY array is valid for pasting only.
 * OLEUIPASTE_PASTE         Indicates that the entry in the OLEUIPASTEENTRY array is valid for pasting. It
 *   may also be valid for linking if any of the following linking flags are specified.
 *
 * If the entry in the OLEUIPASTEENTRY array is valid for linking, the following flags indicate which link
 * types are acceptable by OR'ing together the appropriate OLEUIPASTE_LINKTYPE<#> values.
 * These values correspond as follows to the array of link types passed to OleUIPasteSpecial:
 *   OLEUIPASTE_LINKTYPE1=arrLinkTypes[0]
 *   OLEUIPASTE_LINKTYPE2=arrLinkTypes[1]
 *   OLEUIPASTE_LINKTYPE3=arrLinkTypes[2]
 *   OLEUIPASTE_LINKTYPE4=arrLinkTypes[3]
 *   OLEUIPASTE_LINKTYPE5=arrLinkTypes[4]
 *   OLEUIPASTE_LINKTYPE6=arrLinkTypes[5]
 *   OLEUIPASTE_LINKTYPE7=arrLinkTypes[6]
 *  OLEUIPASTE_LINKTYPE8=arrLinkTypes[7]
 *
 * where,
 *   unsigned int arrLinkTypes[8] is an array of registered clipboard formats for linking. A maximium of 8 link
 *   types are allowed.
 */

typedef enum tagOLEUIPASTEFLAG
{
   OLEUIPASTE_ENABLEICON    = 2048,     // enable display as icon
   OLEUIPASTE_PASTEONLY     = 0,
   OLEUIPASTE_PASTE         = 512,
   OLEUIPASTE_LINKANYTYPE   = 1024,
   OLEUIPASTE_LINKTYPE1     = 1,
   OLEUIPASTE_LINKTYPE2     = 2,
   OLEUIPASTE_LINKTYPE3     = 4,
   OLEUIPASTE_LINKTYPE4     = 8,
   OLEUIPASTE_LINKTYPE5     = 16,
   OLEUIPASTE_LINKTYPE6     = 32,
   OLEUIPASTE_LINKTYPE7     = 64,
   OLEUIPASTE_LINKTYPE8     = 128
} OLEUIPASTEFLAG;

/*
 * PasteEntry structure
 * --------------------
 * An array of OLEUIPASTEENTRY entries is specified for the PasteSpecial dialog
 * box. Each entry includes a FORMATETC which specifies the formats that are
 * acceptable, a string that is to represent the format in the  dialog's list
 * box, a string to customize the result text of the dialog and a set of flags
 * from the OLEUIPASTEFLAG enumeration.  The flags indicate if the entry is
 * valid for pasting only, linking only or both pasting and linking. If the
 * entry is valid for linking, the flags indicate which link types are
 * acceptable by OR'ing together the appropriate OLEUIPASTE_LINKTYPE<#> values.
 * These values correspond to the array of link types as follows:
 *   OLEUIPASTE_LINKTYPE1=arrLinkTypes[0]
 *   OLEUIPASTE_LINKTYPE2=arrLinkTypes[1]
 *   OLEUIPASTE_LINKTYPE3=arrLinkTypes[2]
 *   OLEUIPASTE_LINKTYPE4=arrLinkTypes[3]
 *   OLEUIPASTE_LINKTYPE5=arrLinkTypes[4]
 *   OLEUIPASTE_LINKTYPE6=arrLinkTypes[5]
 *   OLEUIPASTE_LINKTYPE7=arrLinkTypes[6]
 *   OLEUIPASTE_LINKTYPE8=arrLinkTypes[7]
 *   unsigned int arrLinkTypes[8]; is an array of registered clipboard formats
 *                        for linking. A maximium of 8 link types are allowed.
 */

typedef struct tagOLEUIPASTEENTRY
{
   FORMATETC        fmtetc;            // Format that is acceptable. The paste
                                       //   dialog checks if this format is
                                       //   offered by the object on the
                                       //   clipboard and if so offers it for
                                       //   selection to the user.
   const char*      lpstrFormatName;   // String that represents the format to the user. Any %s
                                       //   in this string is replaced by the FullUserTypeName
                                       //   of the object on the clipboard and the resulting string
                                       //   is placed in the list box of the dialog. Atmost
                                       //   one %s is allowed. The presence or absence of %s indicates
                                       //   if the result text is to indicate that data is
                                       //   being pasted or that an object that can be activated by
                                       //   an application is being pasted. If %s is
                                       //   present, the result-text says that an object is being pasted.
                                       //   Otherwise it says that data is being pasted.
   const char*      lpstrResultText;   // String to customize the result text of the dialog when
                                       //  the user selects the format correspoding to this
                                       //  entry. Any %s in this string is replaced by the the application
                                       //  name or FullUserTypeName of the object on
                                       //  the clipboard. Atmost one %s is allowed.
   unsigned long    dwFlags;           // Values from OLEUIPASTEFLAG enum
   unsigned long    dwScratchSpace;    // Scratch space available to be used
                                       //   by routines which loop through an
                                       //   IEnumFORMATETC* to mark if the
                                       //   PasteEntry format is available.
                                       //   this field CAN be left uninitialized.
} OLEUIPASTEENTRY, *POLEUIPASTEENTRY, *LPOLEUIPASTEENTRY;

#define OLESTDDROP_NONE         0
#define OLESTDDROP_DEFAULT      1
#define OLESTDDROP_NONDEFAULT   2


#ifdef __UNUSED__

/*
 * Some misc stuff
 */

#define EMBEDDINGFLAG "Embedding"     // Cmd line switch for launching a srvr

#define HIMETRIC_PER_INCH   2540      // number HIMETRIC units per inch
#define PTS_PER_INCH        72        // number points (font size) per inch

#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

#endif


/****** DEBUG Stuff *****************************************************/

#ifdef _DEBUG

#if !defined( _DBGTRACE )
#define _DEBUGLEVEL 2
#else
#define _DEBUGLEVEL _DBGTRACE
#endif

#if !defined( _DEBUG )

#if !defined(_OLE_ASSERTS)
#define ASSERTDATA
#endif

#define OLEDBGASSERTDATA
#define OleDbgAssert(a)
#define OleDbgAssertSz(a, b)
#define OleDbgVerify(a)
#define OleDbgVerifySz(a, b)

#else   // ! NOASSERT

#if !defined(_OLE_ASSERTS)
#define ASSERTDATA			static char _szAssertFile[]=__FILE__;
#define Assert(a)		{ if (!(a)) __UIASSERTCONDSZ(#a, NULL, _szAssertFile, __LINE__); }
#define AssertSz(a, b)	{ if (!(a)) __UIASSERTCONDSZ(#a, b, _szAssertFile, __LINE__); }
#endif

#define OLEDBGASSERTDATA    ASSERTDATA

#define OleDbgAssert	Assert

#define OleDbgAssertSz  AssertSz

#define OleDbgVerify(a) \
        OleDbgAssert(a)

#define OleDbgVerifySz(a, b)    \
        OleDbgAssertSz(a, b)

#endif  // ! NOASSERT


#define OLEDBGDATA_MAIN(szPrefix)   \
        char  *g_szDbgPrefix = szPrefix;    \
        OLEDBGASSERTDATA

#define OLEDBGDATA  \
        extern char *g_szDbgPrefix;    \
        OLEDBGASSERTDATA

#define OLEDBG_BEGIN(lpsz) \
        OleDbgPrintAlways(g_szDbgPrefix,lpsz,1);

#define OLEDBG_END  \
        OleDbgPrintAlways(g_szDbgPrefix,"End\n",-1);

#define OleDbgOut(lpsz) \
        OleDbgPrintAlways(g_szDbgPrefix,lpsz,0)

#define OleDbgOutNoPrefix(lpsz) \
        OleDbgPrint("",lpsz,0)

#define OleDbgOutRefCnt(lpsz,lpObj,refcnt)      \
        OleDbgPrintRefCntAlways(g_szDbgPrefix,lpsz,lpObj,(unsigned long)refcnt)

#define OleDbgOutRect(lpsz,lpRect)      \
        OleDbgPrintRectAlways(g_szDbgPrefix,lpsz,lpRect)

#define OleDbgOutHResult(lpsz,hr)   \
        OleDbgPrintScodeAlways(g_szDbgPrefix,lpsz,GetScode(hr))

#define OleDbgOutScode(lpsz,sc) \
        OleDbgPrintScodeAlways(g_szDbgPrefix,lpsz,sc)

#define OleDbgOut1(lpsz)    \
        OleDbgPrint(1,g_szDbgPrefix,lpsz,0)

#define OleDbgOutNoPrefix1(lpsz)    \
        OleDbgPrint(1,"",lpsz,0)

#define OLEDBG_BEGIN1(lpsz)    \
        OleDbgPrint(1,g_szDbgPrefix,lpsz,1);

#define OLEDBG_END1 \
        OleDbgPrint(1,g_szDbgPrefix,"End\n",-1);

#define OleDbgOutRefCnt1(lpsz,lpObj,refcnt)     \
        OleDbgPrintRefCnt(1,g_szDbgPrefix,lpsz,lpObj,(unsigned long)refcnt)

#define OleDbgOutRect1(lpsz,lpRect)     \
        OleDbgPrintRect(1,g_szDbgPrefix,lpsz,lpRect)

#define OleDbgOut2(lpsz)    \
        OleDbgPrint(2,g_szDbgPrefix,lpsz,0)

#define OleDbgOutNoPrefix2(lpsz)    \
        OleDbgPrint(2,"",lpsz,0)

#define OLEDBG_BEGIN2(lpsz)    \
        OleDbgPrint(2,g_szDbgPrefix,lpsz,1);

#define OLEDBG_END2 \
        OleDbgPrint(2,g_szDbgPrefix,"End\n",-1);

#define OleDbgOutRefCnt2(lpsz,lpObj,refcnt)     \
        OleDbgPrintRefCnt(2,g_szDbgPrefix,lpsz,lpObj,(unsigned long)refcnt)

#define OleDbgOutRect2(lpsz,lpRect)     \
        OleDbgPrintRect(2,g_szDbgPrefix,lpsz,lpRect)

#define OleDbgOut3(lpsz)    \
        OleDbgPrint(3,g_szDbgPrefix,lpsz,0)

#define OleDbgOutNoPrefix3(lpsz)    \
        OleDbgPrint(3,"",lpsz,0)

#define OLEDBG_BEGIN3(lpsz)    \
        OleDbgPrint(3,g_szDbgPrefix,lpsz,1);

#define OLEDBG_END3 \
        OleDbgPrint(3,g_szDbgPrefix,"End\n",-1);

#define OleDbgOutRefCnt3(lpsz,lpObj,refcnt)     \
        OleDbgPrintRefCnt(3,g_szDbgPrefix,lpsz,lpObj,(unsigned long)refcnt)

#define OleDbgOutRect3(lpsz,lpRect)     \
        OleDbgPrintRect(3,g_szDbgPrefix,lpsz,lpRect)

#define OleDbgOut4(lpsz)    \
        OleDbgPrint(4,g_szDbgPrefix,lpsz,0)

#define OleDbgOutNoPrefix4(lpsz)    \
        OleDbgPrint(4,"",lpsz,0)

#define OLEDBG_BEGIN4(lpsz)    \
        OleDbgPrint(4,g_szDbgPrefix,lpsz,1);

#define OLEDBG_END4 \
        OleDbgPrint(4,g_szDbgPrefix,"End\n",-1);

#define OleDbgOutRefCnt4(lpsz,lpObj,refcnt)     \
        OleDbgPrintRefCnt(4,g_szDbgPrefix,lpsz,lpObj,(unsigned long)refcnt)

#define OleDbgOutRect4(lpsz,lpRect)     \
        OleDbgPrintRect(4,g_szDbgPrefix,lpsz,lpRect)

#else   //  !_DEBUG

#define OLEDBGDATA_MAIN(szPrefix)
#define OLEDBGDATA
#define OleDbgAssert(a)
#define OleDbgAssertSz(a, b)
#define OleDbgVerify(a)         (a)
#define OleDbgVerifySz(a, b)    (a)
#define OleDbgOutHResult(lpsz,hr)
#define OleDbgOutScode(lpsz,sc)
#define OLEDBG_BEGIN
#define OLEDBG_END
#define OleDbgOut(lpsz)
#define OleDbgOut1(lpsz)
#define OleDbgOut2(lpsz)
#define OleDbgOut3(lpsz)
#define OleDbgOut4(lpsz)
#define OleDbgOutNoPrefix(lpsz)
#define OleDbgOutNoPrefix1(lpsz)
#define OleDbgOutNoPrefix2(lpsz)
#define OleDbgOutNoPrefix3(lpsz)
#define OleDbgOutNoPrefix4(lpsz)
#define OLEDBG_BEGIN1(lpsz)
#define OLEDBG_BEGIN2(lpsz)
#define OLEDBG_BEGIN3(lpsz)
#define OLEDBG_BEGIN4(lpsz)
#define OLEDBG_END1
#define OLEDBG_END2
#define OLEDBG_END3
#define OLEDBG_END4
#define OleDbgOutRefCnt(lpsz,lpObj,refcnt)
#define OleDbgOutRefCnt1(lpsz,lpObj,refcnt)
#define OleDbgOutRefCnt2(lpsz,lpObj,refcnt)
#define OleDbgOutRefCnt3(lpsz,lpObj,refcnt)
#define OleDbgOutRefCnt4(lpsz,lpObj,refcnt)
#define OleDbgOutRect(lpsz,lpRect)
#define OleDbgOutRect1(lpsz,lpRect)
#define OleDbgOutRect2(lpsz,lpRect)
#define OleDbgOutRect3(lpsz,lpRect)
#define OleDbgOutRect4(lpsz,lpRect)

#endif  //  _DEBUG


//Function prototypes

//OLESTD.C
STDAPI_(int) SetDCToAnisotropic(WindowPtr hDC, Rect* lprcPhysical, Rect* lprcLogical, Rect* lprcWindowOld, Rect* lprcViewportOld);
STDAPI_(int) SetDCToDrawInHimetricRect(WindowPtr, Rect*, Rect*, Rect*, Rect*);
STDAPI_(int) ResetOrigDC(WindowPtr, int, Rect*, Rect*);

STDAPI_(void) XformRectInPixelsToHimetric(WindowPtr, Rect*, Rect*);
STDAPI_(void) XformRectInHimetricToPixels(WindowPtr, Rect*, Rect*);
STDAPI_(void) XformSizeInPixelsToHimetric(WindowPtr, LPSIZEL, LPSIZEL);
STDAPI_(void) XformSizeInHimetricToPixels(WindowPtr, LPSIZEL, LPSIZEL);
STDAPI_(int) XformWidthInHimetricToPixels(WindowPtr, int);
STDAPI_(int) XformWidthInPixelsToHimetric(WindowPtr, int);
STDAPI_(int) XformHeightInHimetricToPixels(WindowPtr, int);
STDAPI_(int) XformHeightInPixelsToHimetric(WindowPtr, int);
STDAPI_(unsigned long) AreRectsEqual(Rect* lprc1, Rect* lprc2);


STDAPI_(void) ParseCmdLine(char*, unsigned long *, char*);

STDAPI_(unsigned long) OleStdIsOleLink(LPUNKNOWN lpUnk);
STDAPI_(LPUNKNOWN) OleStdQueryInterface(LPUNKNOWN lpUnk, REFIID riid);
STDAPI_(LPSTORAGE) OleStdCreateRootStorage(char* lpszStgName, unsigned long grfMode);
STDAPI_(LPSTORAGE) OleStdOpenRootStorage(char* lpszStgName, unsigned long grfMode);
STDAPI_(LPSTORAGE) OleStdOpenOrCreateRootStorage(char* lpszStgName, unsigned long grfMode);
STDAPI_(LPSTORAGE) OleStdCreateChildStorage(LPSTORAGE lpStg, char* lpszStgName);
STDAPI_(LPSTORAGE) OleStdOpenChildStorage(LPSTORAGE lpStg, char* lpszStgName, unsigned long grfMode);
STDAPI_(unsigned long) OleStdCommitStorage(LPSTORAGE lpStg);

STDAPI_(LPSTORAGE) OleStdCreateStorageOnHGlobal(
        Handle hGlobal,
        unsigned long fDeleteOnRelease,
        unsigned long dwgrfMode
);
STDAPI_(LPSTORAGE) OleStdCreateTempStorage(unsigned long fUseMemory, unsigned long grfMode);
STDAPI OleStdDoConvert(LPSTORAGE lpStg, REFCLSID rClsidNew);
STDAPI_(unsigned long) OleStdGetTreatAsFmtUserType(
        REFCLSID        rClsidApp,
        LPSTORAGE       lpStg,
        CLSID*      lpclsid,
        ResType* lpcfFmt,
        char**      lplpszType
);
STDAPI OleStdDoTreatAsClass(char* lpszUserType, REFCLSID rclsid, REFCLSID rclsidNew);
STDAPI_(unsigned long) OleStdSetupAdvises(LPOLEOBJECT lpOleObject, unsigned long dwDrawAspect,
                    char* lpszContainerApp, char* lpszContainerObj,
                    LPADVISESINK lpAdviseSink);
STDAPI OleStdSwitchDisplayAspect(
        LPOLEOBJECT             lpOleObj,
        unsigned long*          lpdwCurAspect,
        unsigned long           dwNewAspect,
        PicHandle               hMetaPict,
        unsigned long           fDeleteOldAspect,
        unsigned long           fSetupViewAdvise,
        LPADVISESINK            lpAdviseSink,
        Boolean*		        lpfMustUpdate
);
STDAPI OleStdSetIconInCache(LPOLEOBJECT lpOleObj, PicHandle hMetaPict);
STDAPI_(Handle) OleStdGetData(
        LPDATAOBJECT        lpDataObj,
        ResType          cfFormat,
        DVTARGETDEVICE* lpTargetDevice,
        unsigned long               dwAspect,
        LPSTGMEDIUM         lpMedium
);
STDAPI_(void) OleStdMarkPasteEntryList(
        LPDATAOBJECT        lpSrcDataObj,
        LPOLEUIPASTEENTRY   lpPriorityList,
        int                 cEntries
);
STDAPI_(int) OleStdGetPriorityClipboardFormat(
        LPDATAOBJECT        lpSrcDataObj,
        LPOLEUIPASTEENTRY   lpPriorityList,
        int                 cEntries
);
STDAPI_(unsigned long) OleStdIsDuplicateFormat(
        LPFORMATETC         lpFmtEtc,
        LPFORMATETC         arrFmtEtc,
        int                 nFmtEtc
);
STDAPI_(void) OleStdRegisterAsRunning(LPUNKNOWN lpUnk, LPMONIKER lpmkFull, unsigned long* lpdwRegister);
STDAPI_(void) OleStdRevokeAsRunning(unsigned long* lpdwRegister);
STDAPI_(void) OleStdNoteFileChangeTime(char* lpszFileName, unsigned long dwRegister);
STDAPI_(void) OleStdNoteObjectChangeTime(unsigned long dwRegister);
STDAPI OleStdGetOleObjectData(
        LPPERSISTSTORAGE    lpPStg,
        LPFORMATETC         lpformatetc,
        LPSTGMEDIUM         lpMedium,
        unsigned long                fUseMemory
);
STDAPI OleStdGetLinkSourceData(
        LPMONIKER           lpmk,
        LPCLSID             lpClsID,
        LPFORMATETC         lpformatetc,
        LPSTGMEDIUM         lpMedium
);
STDAPI_(Handle) OleStdGetObjectDescriptorData(
        CLSID               clsid,
        unsigned long               dwAspect,
        SIZEL               sizel,
        POINTL              pointl,
        unsigned long               dwStatus,
        char*               lpszFullUserTypeName,
        char*               lpszSrcOfCopy
);
STDAPI_(Handle) OleStdGetObjectDescriptorDataFromOleObject(
        LPOLEOBJECT         lpOleObj,
        char*               lpszSrcOfCopy,
        unsigned long               dwAspect,
        POINTL              pointl
);
STDAPI_(Handle) OleStdFillObjectDescriptorFromData(
        LPDATAOBJECT       lpDataObject,
        LPSTGMEDIUM        lpmedium,
        ResType*    lpcfFmt
);
STDAPI_(unsigned long) OleStdCopyMetafilePict(Handle hpictin, Handle* phpictout);
STDAPI_(Handle) OleStdGetMetafilePictFromOleObject(
        LPOLEOBJECT         lpOleObj,
        unsigned long               dwDrawAspect
);
STDAPI OleStdQueryOleObjectData(LPFORMATETC lpformatetc);
STDAPI OleStdQueryLinkSourceData(LPFORMATETC lpformatetc);
STDAPI OleStdQueryObjectDescriptorData(LPFORMATETC lpformatetc);
STDAPI OleStdQueryFormatMedium(LPFORMATETC lpformatetc, TYMED tymed);
STDAPI_(void) OleStdCreateTempFileMoniker(FSSpecPtr pPrefixFSP, unsigned int* lpuUnique, LPMONIKER* lplpmk);
STDAPI_(LPMONIKER) OleStdGetFirstMoniker(LPMONIKER lpmk);
STDAPI_(unsigned long) OleStdGetLenFilePrefixOfMoniker(LPMONIKER lpmk);
STDAPI_(void*) OleStdMalloc(unsigned long ulSize);
STDAPI_(void*) OleStdRealloc(void* pmem, unsigned long ulSize);
STDAPI_(void) OleStdFree(void* pmem);
STDAPI_(unsigned long) OleStdGetSize(void* pmem);
STDAPI_(void) OleStdFreeString(char* lpsz, LPMALLOC lpMalloc);
STDAPI_(char*) OleStdCopyString(char* lpszSrc, LPMALLOC lpMalloc);
STDAPI_(unsigned long) OleStdGetItemToken(char* lpszSrc, char* lpszDst, int nMaxChars);

STDAPI_(void*) OleStdMemSet(void* pDest, int c, size_t count);

STDAPI_(void) OleStdInitVtbl(void* lpVtbl, unsigned int nSizeOfVtbl);
STDMETHODIMP OleStdNullMethod(LPUNKNOWN lpThis);
STDAPI_(unsigned long) OleStdCheckVtbl(void* lpVtbl, unsigned int nSizeOfVtbl, char* lpszIface);
// STDAPI_(unsigned long) OleStdVerifyRelease(LPUNKNOWN lpUnk, char* lpszMsg);
STDAPI_(unsigned long) OleStdVerifyRelease(LPUNKNOWN lpUnk);
STDAPI_(unsigned long) OleStdRelease(LPUNKNOWN lpUnk);

STDAPI_(WindowPtr) OleStdCreateDC(DVTARGETDEVICE* ptd);
STDAPI_(WindowPtr) OleStdCreateIC(DVTARGETDEVICE* ptd);
// STDAPI_(DVTARGETDEVICE*) OleStdCreateTargetDevice(LPPRINTDLG lpPrintDlg);
STDAPI_(unsigned long) OleStdDeleteTargetDevice(DVTARGETDEVICE* ptd);
STDAPI_(DVTARGETDEVICE *) OleStdCopyTargetDevice(DVTARGETDEVICE* ptdSrc);
STDAPI_(unsigned long) OleStdCopyFormatEtc(LPFORMATETC petcDest, LPFORMATETC petcSrc);

/*
 * Functions prototypes used in geticon.c
 */
STDAPI_(unsigned long) OleStdGetUserTypeOfClass(REFCLSID rclsid, char *lpszUserType, unsigned int cch, HKEY hKey);
STDAPI_(unsigned long) OleStdGetAuxUserType(REFCLSID rclsid, short wAuxUserType,  char *lpszAuxUserType,
                                      int cch, HKEY hKey);
/*
 * Functions prototypes used in enumfetc.c
 */
STDAPI_(void) EnumFORMATETCInitInterface(void);
STDAPI_(LPENUMFORMATETC) OleStdEnumFmtEtc_Create(unsigned short wCount, LPFORMATETC lpEtc);


STDAPI_(void) OleDbgPrint(
        int     nDbgLvl,
        char*   lpszPrefix,
        char*   lpszMsg,
        int     nIndent
);
STDAPI_(void) OleDbgPrintAlways(char* lpszPrefix, char* lpszMsg, int nIndent);
STDAPI_(void) OleDbgSetDbgLevel(int nDbgLvl);
STDAPI_(int) OleDbgGetDbgLevel( void );
STDAPI_(void) OleDbgIndent(int n);
STDAPI_(void) OleDbgPrintRefCnt(
        int         nDbgLvl,
        char*       lpszPrefix,
        char*       lpszMsg,
        void*      lpObj,
        unsigned long       refcnt
);
STDAPI_(void) OleDbgPrintRefCntAlways(
        char*       lpszPrefix,
        char*       lpszMsg,
        void*      lpObj,
        unsigned long       refcnt
);
STDAPI_(void) OleDbgPrintRect(
        int         nDbgLvl,
        char*       lpszPrefix,
        char*       lpszMsg,
        Rect*      lpRect
);
STDAPI_(void) OleDbgPrintRectAlways(
        char*       lpszPrefix,
        char*       lpszMsg,
        Rect*      lpRect
);
STDAPI_(void) OleDbgPrintScodeAlways(char* lpszPrefix, char* lpszMsg, SCODE sc);



STDAPI OleStdGetInplaceClipRgn(RgnHandle rgn);
// gets the union of all window structure rgns in the inplace server.
// pass in a previously allocated rgn handle.  Overwrites its contents

STDAPI OleStdBlastWindows(RgnHandle rgn);
// walks entire window list removing any part of a window that overlaps w/
// the input rgn

STDAPI OleStdBlastWindow(WindowPtr window, RgnHandle rgn);
//same as above, but only does one window

STDAPI OleStdUnblastWindows(RgnHandle rgn);
// walks entire window list calling each window's WDEF to
// recalc rgns. This ensures that window's struc/cont rgns
// are in a consistent and correct state.
// if rgn is non-null (in the case that the container is to remain in the
// foreground, then this api causes an update event for that area (PaintOne)

STDAPI OleStdTranslateAccelerator(
		LPOLEINPLACEFRAME lpFrame,
		LPOLEINPLACEFRAMEINFO lpFrameInfo,
		EventRecord * lpEvent);


#endif // _OLESTD_H_
