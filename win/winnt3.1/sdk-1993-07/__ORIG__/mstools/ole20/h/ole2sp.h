/* ole2sp.h - semi-private info; only for test apps within the development group
*/

#if !defined( _OLE2SP_H_ )
#define _OLE2SP_H_

#include <shellapi.h>

// For MAC, M_PROLOG and M_EPILOG are macros which assist us in setting up the A5
// world for a DLL when a method in the DLL is called from outside the DLL.

#ifdef _MAC

#define _MAX_PATH 260

#ifdef __cplusplus

class  CSetA5
{
public:
    CSetA5 (ULONG savedA5){ A5save = SetA5(savedA5);}
    ~CSetA5 (){ SetA5(A5save);}

private:
    ULONG A5save;
};

pascal long     GetA5(void) = 0x2E8D;

#define M_PROLOG(where) CSetA5 Dummy((where)->savedA5) 
#define SET_A5          ULONG savedA5
#define GET_A5()        savedA5 = GetA5()

// These macros assist Mac in manually saving/setting/restoring A5 in routines that contain
// goto's.

#define A5_PROLOG(where) ULONG A5save = SetA5(where->savedA5)
#define RESTORE_A5()     SetA5(A5save)

// Lets MAC name our segments without ifdef's.

#define NAME_SEG(x)

#endif // ccplus

#else

#define M_PROLOG(where)
#define SET_A5
#define GET_A5()
#define A5_PROLOG(where)
#define RESTORE_A5()
#define NAME_SEG(x)


#define IGetProcAddress(a,b) GetProcAddress((a),(b))

#endif


#define ReportResult(a,b,c,d) ResultFromScode(b)

#ifdef OLE2SHIP
#ifdef WIN32
#define MAP16(v16)
#define MAP32(v32) v32
#define MAP1632(v16,v32)   v32
#else
#define MAP16(v16) v16
#define MAP32(v32)
#define MAP1632(v16,v32)   v16
#endif
#endif

/****** Misc defintions ***************************************************/

#ifdef __TURBOC__
#define implement struct huge
#else
#define implement struct 
#endif
#define ctor_dtor private
#define implementations private
#define shared_state private

// helpers for internal methods and functions which follow the same convention
// as the external ones

#ifdef __cplusplus
#define INTERNALAPI_(type) extern "C" type 
#else 
#define INTERNALAPI_(type) type 
#endif 

#define INTERNAL HRESULT 
#define INTERNAL_(type) type
#define FARINTERNAL HRESULT FAR
#define FARINTERNAL_(type) type FAR
#define NEARINTERNAL HRESULT NEAR
#define NEARINTERNAL_(type) type NEAR



//BEGIN REVIEW: We may not need all the following ones

#define OT_LINK     1L
#define OT_EMBEDDED 2L
#define OT_STATIC   3L


//END REVIEW .....


/****** Old Error Codes    ************************************************/

#define S_OOM               E_OUTOFMEMORY
#define S_BADARG            E_INVALIDARG
#define S_BLANK             E_BLANK
#define S_FORMAT            E_FORMAT
#define S_NOT_RUNNING       E_NOTRUNNING
#define E_UNSPEC            E_FAIL


/****** Macros for nested clases ******************************************/

/* To overcome problems with nested classes on MAC
 *
 * NC(a,b) is used to define a member function of a nested class:
 *
 * STDMETHODIMP_(type) NC(ClassName,NestedClassName)::MemberFunction(...)
 *
 * DECLARE_NC(a,b) is used within a class declaration to let a nested class
 * access it container class:
 *
 * class ClassName {
 *     ..............
 *
 *     class NestedClassName {
 *         .............
 *     };
 *     DECLARE_NC(ClassName,NestedClassName)
 *     ..............
 * };
 */

#ifdef _MAC

#define NESTED_CLASS(a,b) struct a##_##b 
#define NC(a,b) a##__##b
#define NC1(a,b) a##_##b
#define DECLARE_NC(a,b) typedef a##::##b a##__##b; friend a##__##b;
#define DECLARE_NC2(a,b) typedef a##::a##_##b a##__##b; friend a##__##b;

#else

#define NC(a,b) a##::##b
#define DECLARE_NC(a,b) friend b;

#endif  


/****** More Misc defintions **********************************************/


// LPLPVOID should not be made a typedef.  typedef won't compile; worse
// within complicated macros the compiler generates unclear error messages
//
#define LPLPVOID void FAR * FAR *

#define UNREFERENCED(a) ((void)(a))

#ifndef BASED_CODE
#ifdef WIN32
#define BASED_CODE
#else
#define BASED_CODE __based(__segname("_CODE"))
#endif
#endif


/****** Standard IUnknown Implementation **********************************/

/*
 *      The following macro declares a nested class CUnknownImpl,
 *      creates an object of that class in the outer class, and
 *      declares CUnknownImpl to be a friend of the outer class.  After
 *      writing about 20 class headers, it became evident that the
 *      implementation of CUnknownImpl was very similar in all cases,
 *      and this macro captures the similarity.  The classname
 *      parameter is the name of the outer class WITHOUT the leading
 *      "C"; i.e., for CFileMoniker, classname is FileMoniker.
 */

#define noError return NOERROR

#ifdef _MAC

#define STDUNKDECL(cclassname,classname) NESTED_CLASS(cclassname, CUnknownImpl):IUnknown { public: \
    NC1(cclassname,CUnknownImpl)( cclassname FAR * p##classname ) { m_p##classname = p##classname;} \
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPLPVOID ppvObj); \
    STDMETHOD_(ULONG,AddRef)(THIS); \
    STDMETHOD_(ULONG,Release)(THIS); \
    private: cclassname FAR* m_p##classname; }; \
    DECLARE_NC2(cclassname, CUnknownImpl) \
    NC(cclassname, CUnknownImpl) m_Unknown;

#else  // _MAC

#define STDUNKDECL( ignore, classname ) implement CUnknownImpl:IUnknown { public: \
    CUnknownImpl( C##classname FAR * p##classname ) { m_p##classname = p##classname;} \
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPLPVOID ppvObj); \
    STDMETHOD_(ULONG,AddRef)(THIS); \
    STDMETHOD_(ULONG,Release)(THIS); \
    private: C##classname FAR* m_p##classname; }; \
    DECLARE_NC(C##classname, CUnknownImpl) \
    CUnknownImpl m_Unknown;
#endif

/*
 *      The following macro implements all the methods of a nested
 *      CUnknownImpl class EXCEPT FOR QUERYINTERFACE.  This macro was
 *      written after about 20 classes were written in which the
 *      implementations of CUnknownImpl were all the same.
 */

#define STDUNKIMPL(classname) \
STDMETHODIMP_(ULONG) NC(C##classname,CUnknownImpl)::AddRef( void ){ \
    return ++m_p##classname->m_refs; } \
STDMETHODIMP_(ULONG) NC(C##classname,CUnknownImpl)::Release( void ){ \
    if (--m_p##classname->m_refs == 0) { delete m_p##classname; return 0; } \
    return m_p##classname->m_refs;}


/*
 *      The following macro implements class::CUnknownImpl::QueryInterface IN
 *      THE SPECIAL CASE IN WHICH THE OUTER CLASS PRESENTS ONLY ONE INTERFACE
 *      OTHER THAN IUNKNOWN AND IDEBUG.  This is not universally the case,
 *      but it is common enough that this macro will save time and space.
 */

#ifdef _DEBUG
#define STDDEB_QI(classname) \
    if (iidInterface == IID_IDebug) {*ppv = (void FAR *)&(m_p##classname->m_Debug); return 0;} else
#else
#define STDDEB_QI(classname)
#endif

#define STDUNK_QI_IMPL(classname, interfacename) \
STDMETHODIMP NC(C##classname,CUnknownImpl)::QueryInterface \
    (REFIID iidInterface, void FAR * FAR * ppv) { \
    if (iidInterface == IID_IUnknown) {\
        *ppv = (void FAR *)&m_p##classname->m_Unknown;\
        AddRef(); noError;\
    } else if (iidInterface == IID_I##interfacename) { \
        *ppv = (void FAR *) &(m_p##classname->m_##classname); \
        m_p##classname->m_pUnkOuter->AddRef(); return NOERROR; \
    } else \
        STDDEB_QI(classname) \
        {*ppv = NULL; return ResultFromScode(E_NOINTERFACE);} \
}


/*
 *      The following macro implements the IUnknown methods inherited
 *      by the implementation of another interface.  The implementation
 *      is simply to delegate all calls to m_pUnkOuter.  Parameters:
 *      ocname is the outer class name, icname is the implementation
 *      class name.
 *      
 */

#define STDUNKIMPL_FORDERIVED(ocname, icname) \
STDMETHODIMP NC(C##ocname,C##icname)::QueryInterface \
(REFIID iidInterface, LPLPVOID ppvObj) { \
    return m_p##ocname->m_pUnkOuter->QueryInterface(iidInterface, ppvObj);} \
STDMETHODIMP_(ULONG) NC(C##ocname,C##icname)::AddRef(void) { \
    return m_p##ocname->m_pUnkOuter->AddRef(); } \
STDMETHODIMP_(ULONG) NC(C##ocname,C##icname)::Release(void) { \
    return m_p##ocname->m_pUnkOuter->Release(); }


/****** Debug defintions **************************************************/

#include <debug.h>


/****** Other API defintions **********************************************/

//  Utility function not in the spec; in ole2.dll.
//  Read and write length-prefixed strings.  Open/Create stream.
//  ReadStringStream does allocation, returns length of
//  required buffer (strlen + 1 for terminating null)

STDAPI  ReadStringStream( LPSTREAM pstm, LPSTR FAR * ppsz );
STDAPI  WriteStringStream( LPSTREAM pstm, LPCSTR psz );
STDAPI  OpenOrCreateStream( IStorage FAR * pstg, const char FAR * pwcsName,
                                                      IStream FAR* FAR* ppstm);


// read and write ole control stream (in ole2.dll)
STDAPI  WriteOleStg (LPSTORAGE pstg, IOleObject FAR* pOleObj, 
            CLIPFORMAT cfFormat, LPSTREAM FAR* ppstmOut);
STDAPI  ReadOleStg (LPSTORAGE pstg, DWORD FAR* pdwFlags, 
                DWORD FAR* pdwOptUpdate, CLIPFORMAT FAR* pcfFormat, 
                LPMONIKER FAR* ppmk, LPSTREAM FAR* pstmOut);
STDAPI ReadM1ClassStm(LPSTREAM pStm, CLSID FAR* pclsid);
STDAPI WriteM1ClassStm(LPSTREAM pStm, REFCLSID rclsid);


// low level reg.dat access (in compobj.dll)
STDAPI CoGetInProcDll(REFCLSID rclsid, BOOL fServer, LPSTR lpszDll, int cbMax);
STDAPI CoGetLocalExe(REFCLSID rclsid, LPSTR lpszExe, int cbMax);
STDAPI CoGetClassExt(LPCSTR lpszExt, LPCLSID pclsid);
STDAPI CoGetPSClsid(REFIID iid, LPCLSID lpclsid);


// simpler alternatives to public apis
STDAPI_(int) StringFromGUID2(REFGUID rguid, LPSTR lpsz, int cbMax);
#define StringFromCLSID2(rclsid, lpsz, cbMax) \
    StringFromGUID2(rclsid, lpsz, cbMax)

#define StringFromIID2(riid, lpsz, cbMax) \
    StringFromGUID2(riid, lpsz, cbMax)

STDAPI_(int) Ole1ClassFromCLSID2(REFCLSID rclsid, LPSTR lpsz, int cbMax);
STDAPI_(BOOL) GUIDFromString(LPCSTR lpsz, LPGUID pguid);
STDAPI CLSIDFromOle1Class(LPCSTR lpsz, LPCLSID lpclsid);
STDAPI       CoOpenClassKey(REFCLSID clsid, HKEY FAR* lphkeyClsid);

INTERNAL CreateStandardMalloc(DWORD memctx, IMalloc FAR* FAR* ppMalloc);
// were public; now not
STDAPI  SetDocumentBitStg(LPSTORAGE pStg, BOOL fDocument);
STDAPI  GetDocumentBitStg(LPSTORAGE pStg);


//***************************************************************************
//* Object synchronization stuff
//***************************************************************************

typedef DWORD COCS[4];
typedef COCS FAR* LPCOCS;

typedef struct _TABLELOCK {
    COCS cocs;
    DWORD dwCount;
} TABLELOCK, FAR* LPTABLELOCK;

#ifdef WIN32

EXTERN_C void WINAPI CoInitializeCriticalSection(LPCOCS pcs);
EXTERN_C void WINAPI CoEnterCriticalSection(LPCOCS pcs);
EXTERN_C void WINAPI CoLeaveCriticalSection(LPCOCS pcs);
EXTERN_C void WINAPI CoDeleteCriticalSection(LPCOCS pcs);

EXTERN_C void WINAPI _LockTable( LPTABLELOCK ptl, BOOL fWrite, DWORD * pdw );
EXTERN_C BOOL WINAPI _UnlockTable( LPTABLELOCK ptl, BOOL fWrite, DWORD * pdw );

#define LockTable(pTableLock,fWrite)  \
  TableLockRetry:\
  DWORD dwTableLockRetry;\
  _LockTable(pTableLock,fWrite,&dwTableLockRetry);


#define UnlockTable(pTableLock,fWrite) \
  if ( _UnlockTable(pTableLock, fWrite, &dwTableLockRetry) ) \
      goto TableLockRetry;

#define InitializeTableLock(pTableLock) \
         (CoInitializeCriticalSection(&pTableLock->cocs),pTableLock->dwCount=0)

#define DeleteTableLock(pTableLock) \
         (CoDeleteCriticalSection(&pTableLock->cocs),pTableLock->dwCount=0)


EXTERN_C DWORD GetSyncTimeout(void);

#else

#define CoInitializeCriticalSection(pcs)
#define CoEnterCriticalSection(pcs)
#define CoLeaveCriticalSection(pcs)
#define CoDeleteCriticalSection(pcs)

#define LockTable(pTableLock,fWrite)
#define UnlockTable(pTableLock,fWrite)
#define InitializeTableLock(pTableLock)
#define DeleteTableLock(pTableLock)

#define GetSyncTimeout() ((DWORD) 60000L)

#endif

/*
 * Shared memory allocation routines
 */
STDAPI_(void FAR*) SharedMemAlloc(ULONG size, DWORD id);
STDAPI_(void FAR*) SharedMemReAlloc(void FAR* pmem, ULONG newsize, DWORD id);
STDAPI_(void) SharedMemFree(void FAR* pmem, DWORD id);
STDAPI_(DWORD) SharedMemSize(void FAR* pmem, DWORD id);
STDAPI_(int) MemType(void FAR* pv);

#ifdef WIN32 
EXTERN_C DWORD GetCallTimeout(void);
#else
#define GetCallTimeout() ((DWORD) 5000L)
#endif


/*
 * Some docfiles stuff
 */

#define STGM_DFRALL (STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_DENY_WRITE)
#define STGM_DFALL (STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE)
#define STGM_SALL (STGM_READWRITE | STGM_SHARE_EXCLUSIVE)


#endif // _OLE2SP_H_
