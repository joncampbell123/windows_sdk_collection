
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:28:56 1999
 */
/* Compiler settings for xenroll.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __xenroll_h__
#define __xenroll_h__

/* Forward Declarations */ 

#ifndef __ICEnroll_FWD_DEFINED__
#define __ICEnroll_FWD_DEFINED__
typedef interface ICEnroll ICEnroll;
#endif 	/* __ICEnroll_FWD_DEFINED__ */


#ifndef __ICEnroll2_FWD_DEFINED__
#define __ICEnroll2_FWD_DEFINED__
typedef interface ICEnroll2 ICEnroll2;
#endif 	/* __ICEnroll2_FWD_DEFINED__ */


#ifndef __ICEnroll3_FWD_DEFINED__
#define __ICEnroll3_FWD_DEFINED__
typedef interface ICEnroll3 ICEnroll3;
#endif 	/* __ICEnroll3_FWD_DEFINED__ */


#ifndef __IEnroll_FWD_DEFINED__
#define __IEnroll_FWD_DEFINED__
typedef interface IEnroll IEnroll;
#endif 	/* __IEnroll_FWD_DEFINED__ */


#ifndef __IEnroll2_FWD_DEFINED__
#define __IEnroll2_FWD_DEFINED__
typedef interface IEnroll2 IEnroll2;
#endif 	/* __IEnroll2_FWD_DEFINED__ */


#ifndef __CEnroll_FWD_DEFINED__
#define __CEnroll_FWD_DEFINED__

#ifdef __cplusplus
typedef class CEnroll CEnroll;
#else
typedef struct CEnroll CEnroll;
#endif /* __cplusplus */

#endif 	/* __CEnroll_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "wincrypt.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICEnroll_INTERFACE_DEFINED__
#define __ICEnroll_INTERFACE_DEFINED__

/* interface ICEnroll */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICEnroll;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("43F8F288-7A20-11D0-8F06-00C04FC295E1")
    ICEnroll : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE createFilePKCS10( 
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [in] */ BSTR wszPKCS10FileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE acceptFilePKCS7( 
            /* [in] */ BSTR wszPKCS7FileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE createPKCS10( 
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [retval][out] */ BSTR __RPC_FAR *pPKCS10) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE acceptPKCS7( 
            /* [in] */ BSTR PKCS7) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE getCertFromPKCS7( 
            /* [in] */ BSTR wszPKCS7,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrCert) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE enumProviders( 
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrProvName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE enumContainers( 
            /* [in] */ DWORD dwIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE freeRequestInfo( 
            /* [in] */ BSTR PKCS7OrPKCS10) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreType( 
            /* [in] */ BSTR bstrType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreType( 
            /* [in] */ BSTR bstrType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreType( 
            /* [in] */ BSTR bstrType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreName( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreType( 
            /* [in] */ BSTR bstrType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ContainerName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContainer) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ContainerName( 
            /* [in] */ BSTR bstrContainer) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProvider) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderName( 
            /* [in] */ BSTR bstrProvider) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderType( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderType( 
            /* [in] */ DWORD dwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_KeySpec( 
            /* [retval][out] */ DWORD __RPC_FAR *pdw) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_KeySpec( 
            /* [in] */ DWORD dw) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UseExistingKeySet( 
            /* [retval][out] */ BOOL __RPC_FAR *fUseExistingKeys) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UseExistingKeySet( 
            /* [in] */ BOOL fUseExistingKeys) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GenKeyFlags( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_GenKeyFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DeleteRequestCert( 
            /* [retval][out] */ BOOL __RPC_FAR *fDelete) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DeleteRequestCert( 
            /* [in] */ BOOL fDelete) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WriteCertToCSP( 
            /* [retval][out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WriteCertToCSP( 
            /* [in] */ BOOL fBool) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SPCFileName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SPCFileName( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PVKFileName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PVKFileName( 
            /* [in] */ BSTR bstr) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithm( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithm( 
            /* [in] */ BSTR bstr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICEnrollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICEnroll __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICEnroll __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICEnroll __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createFilePKCS10 )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [in] */ BSTR wszPKCS10FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptFilePKCS7 )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createPKCS10 )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [retval][out] */ BSTR __RPC_FAR *pPKCS10);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptPKCS7 )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR PKCS7);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *getCertFromPKCS7 )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrCert);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumProviders )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrProvName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumContainers )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *freeRequestInfo )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR PKCS7OrPKCS10);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreType )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContainerName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContainer);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContainerName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrContainer);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProvider);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstrProvider);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeySpec )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeySpec )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseExistingKeySet )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fUseExistingKeys);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseExistingKeySet )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BOOL fUseExistingKeys);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GenKeyFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GenKeyFlags )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRequestCert )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fDelete);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRequestCert )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BOOL fDelete);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToCSP )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToCSP )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SPCFileName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SPCFileName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PVKFileName )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PVKFileName )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            ICEnroll __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            ICEnroll __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        END_INTERFACE
    } ICEnrollVtbl;

    interface ICEnroll
    {
        CONST_VTBL struct ICEnrollVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICEnroll_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICEnroll_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICEnroll_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICEnroll_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICEnroll_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICEnroll_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICEnroll_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICEnroll_createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)	\
    (This)->lpVtbl -> createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)

#define ICEnroll_acceptFilePKCS7(This,wszPKCS7FileName)	\
    (This)->lpVtbl -> acceptFilePKCS7(This,wszPKCS7FileName)

#define ICEnroll_createPKCS10(This,DNName,Usage,pPKCS10)	\
    (This)->lpVtbl -> createPKCS10(This,DNName,Usage,pPKCS10)

#define ICEnroll_acceptPKCS7(This,PKCS7)	\
    (This)->lpVtbl -> acceptPKCS7(This,PKCS7)

#define ICEnroll_getCertFromPKCS7(This,wszPKCS7,pbstrCert)	\
    (This)->lpVtbl -> getCertFromPKCS7(This,wszPKCS7,pbstrCert)

#define ICEnroll_enumProviders(This,dwIndex,dwFlags,pbstrProvName)	\
    (This)->lpVtbl -> enumProviders(This,dwIndex,dwFlags,pbstrProvName)

#define ICEnroll_enumContainers(This,dwIndex,pbstr)	\
    (This)->lpVtbl -> enumContainers(This,dwIndex,pbstr)

#define ICEnroll_freeRequestInfo(This,PKCS7OrPKCS10)	\
    (This)->lpVtbl -> freeRequestInfo(This,PKCS7OrPKCS10)

#define ICEnroll_get_MyStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_MyStoreName(This,pbstrName)

#define ICEnroll_put_MyStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_MyStoreName(This,bstrName)

#define ICEnroll_get_MyStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_MyStoreType(This,pbstrType)

#define ICEnroll_put_MyStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_MyStoreType(This,bstrType)

#define ICEnroll_get_MyStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_MyStoreFlags(This,pdwFlags)

#define ICEnroll_put_MyStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_MyStoreFlags(This,dwFlags)

#define ICEnroll_get_CAStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_CAStoreName(This,pbstrName)

#define ICEnroll_put_CAStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_CAStoreName(This,bstrName)

#define ICEnroll_get_CAStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_CAStoreType(This,pbstrType)

#define ICEnroll_put_CAStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_CAStoreType(This,bstrType)

#define ICEnroll_get_CAStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_CAStoreFlags(This,pdwFlags)

#define ICEnroll_put_CAStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_CAStoreFlags(This,dwFlags)

#define ICEnroll_get_RootStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RootStoreName(This,pbstrName)

#define ICEnroll_put_RootStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RootStoreName(This,bstrName)

#define ICEnroll_get_RootStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RootStoreType(This,pbstrType)

#define ICEnroll_put_RootStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RootStoreType(This,bstrType)

#define ICEnroll_get_RootStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RootStoreFlags(This,pdwFlags)

#define ICEnroll_put_RootStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RootStoreFlags(This,dwFlags)

#define ICEnroll_get_RequestStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RequestStoreName(This,pbstrName)

#define ICEnroll_put_RequestStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RequestStoreName(This,bstrName)

#define ICEnroll_get_RequestStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RequestStoreType(This,pbstrType)

#define ICEnroll_put_RequestStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RequestStoreType(This,bstrType)

#define ICEnroll_get_RequestStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RequestStoreFlags(This,pdwFlags)

#define ICEnroll_put_RequestStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RequestStoreFlags(This,dwFlags)

#define ICEnroll_get_ContainerName(This,pbstrContainer)	\
    (This)->lpVtbl -> get_ContainerName(This,pbstrContainer)

#define ICEnroll_put_ContainerName(This,bstrContainer)	\
    (This)->lpVtbl -> put_ContainerName(This,bstrContainer)

#define ICEnroll_get_ProviderName(This,pbstrProvider)	\
    (This)->lpVtbl -> get_ProviderName(This,pbstrProvider)

#define ICEnroll_put_ProviderName(This,bstrProvider)	\
    (This)->lpVtbl -> put_ProviderName(This,bstrProvider)

#define ICEnroll_get_ProviderType(This,pdwType)	\
    (This)->lpVtbl -> get_ProviderType(This,pdwType)

#define ICEnroll_put_ProviderType(This,dwType)	\
    (This)->lpVtbl -> put_ProviderType(This,dwType)

#define ICEnroll_get_KeySpec(This,pdw)	\
    (This)->lpVtbl -> get_KeySpec(This,pdw)

#define ICEnroll_put_KeySpec(This,dw)	\
    (This)->lpVtbl -> put_KeySpec(This,dw)

#define ICEnroll_get_ProviderFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_ProviderFlags(This,pdwFlags)

#define ICEnroll_put_ProviderFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_ProviderFlags(This,dwFlags)

#define ICEnroll_get_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> get_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll_put_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> put_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll_get_GenKeyFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_GenKeyFlags(This,pdwFlags)

#define ICEnroll_put_GenKeyFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_GenKeyFlags(This,dwFlags)

#define ICEnroll_get_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> get_DeleteRequestCert(This,fDelete)

#define ICEnroll_put_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> put_DeleteRequestCert(This,fDelete)

#define ICEnroll_get_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToCSP(This,fBool)

#define ICEnroll_put_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToCSP(This,fBool)

#define ICEnroll_get_SPCFileName(This,pbstr)	\
    (This)->lpVtbl -> get_SPCFileName(This,pbstr)

#define ICEnroll_put_SPCFileName(This,bstr)	\
    (This)->lpVtbl -> put_SPCFileName(This,bstr)

#define ICEnroll_get_PVKFileName(This,pbstr)	\
    (This)->lpVtbl -> get_PVKFileName(This,pbstr)

#define ICEnroll_put_PVKFileName(This,bstr)	\
    (This)->lpVtbl -> put_PVKFileName(This,bstr)

#define ICEnroll_get_HashAlgorithm(This,pbstr)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,pbstr)

#define ICEnroll_put_HashAlgorithm(This,bstr)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,bstr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICEnroll_createFilePKCS10_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR DNName,
    /* [in] */ BSTR Usage,
    /* [in] */ BSTR wszPKCS10FileName);


void __RPC_STUB ICEnroll_createFilePKCS10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_acceptFilePKCS7_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR wszPKCS7FileName);


void __RPC_STUB ICEnroll_acceptFilePKCS7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_createPKCS10_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR DNName,
    /* [in] */ BSTR Usage,
    /* [retval][out] */ BSTR __RPC_FAR *pPKCS10);


void __RPC_STUB ICEnroll_createPKCS10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_acceptPKCS7_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR PKCS7);


void __RPC_STUB ICEnroll_acceptPKCS7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_getCertFromPKCS7_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR wszPKCS7,
    /* [out][retval] */ BSTR __RPC_FAR *pbstrCert);


void __RPC_STUB ICEnroll_getCertFromPKCS7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_enumProviders_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [in] */ DWORD dwFlags,
    /* [out][retval] */ BSTR __RPC_FAR *pbstrProvName);


void __RPC_STUB ICEnroll_enumProviders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_enumContainers_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [out][retval] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB ICEnroll_enumContainers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll_freeRequestInfo_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR PKCS7OrPKCS10);


void __RPC_STUB ICEnroll_freeRequestInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_MyStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ICEnroll_get_MyStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_MyStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ICEnroll_put_MyStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_MyStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB ICEnroll_get_MyStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_MyStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrType);


void __RPC_STUB ICEnroll_put_MyStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_MyStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_MyStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_MyStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_MyStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_CAStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ICEnroll_get_CAStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_CAStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ICEnroll_put_CAStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_CAStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB ICEnroll_get_CAStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_CAStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrType);


void __RPC_STUB ICEnroll_put_CAStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_CAStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_CAStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_CAStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_CAStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RootStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ICEnroll_get_RootStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RootStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ICEnroll_put_RootStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RootStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB ICEnroll_get_RootStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RootStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrType);


void __RPC_STUB ICEnroll_put_RootStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RootStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_RootStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RootStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_RootStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RequestStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ICEnroll_get_RequestStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RequestStoreName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ICEnroll_put_RequestStoreName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RequestStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB ICEnroll_get_RequestStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RequestStoreType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrType);


void __RPC_STUB ICEnroll_put_RequestStoreType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_RequestStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_RequestStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_RequestStoreFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_RequestStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_ContainerName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrContainer);


void __RPC_STUB ICEnroll_get_ContainerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_ContainerName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrContainer);


void __RPC_STUB ICEnroll_put_ContainerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_ProviderName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrProvider);


void __RPC_STUB ICEnroll_get_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_ProviderName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstrProvider);


void __RPC_STUB ICEnroll_put_ProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_ProviderType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwType);


void __RPC_STUB ICEnroll_get_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_ProviderType_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwType);


void __RPC_STUB ICEnroll_put_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_KeySpec_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdw);


void __RPC_STUB ICEnroll_get_KeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_KeySpec_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dw);


void __RPC_STUB ICEnroll_put_KeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_ProviderFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_ProviderFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_ProviderFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_ProviderFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_UseExistingKeySet_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fUseExistingKeys);


void __RPC_STUB ICEnroll_get_UseExistingKeySet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_UseExistingKeySet_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BOOL fUseExistingKeys);


void __RPC_STUB ICEnroll_put_UseExistingKeySet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_GenKeyFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB ICEnroll_get_GenKeyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_GenKeyFlags_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB ICEnroll_put_GenKeyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_DeleteRequestCert_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fDelete);


void __RPC_STUB ICEnroll_get_DeleteRequestCert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_DeleteRequestCert_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BOOL fDelete);


void __RPC_STUB ICEnroll_put_DeleteRequestCert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_WriteCertToCSP_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB ICEnroll_get_WriteCertToCSP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_WriteCertToCSP_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB ICEnroll_put_WriteCertToCSP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_SPCFileName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB ICEnroll_get_SPCFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_SPCFileName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstr);


void __RPC_STUB ICEnroll_put_SPCFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_PVKFileName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB ICEnroll_get_PVKFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_PVKFileName_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstr);


void __RPC_STUB ICEnroll_put_PVKFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll_get_HashAlgorithm_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB ICEnroll_get_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll_put_HashAlgorithm_Proxy( 
    ICEnroll __RPC_FAR * This,
    /* [in] */ BSTR bstr);


void __RPC_STUB ICEnroll_put_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICEnroll_INTERFACE_DEFINED__ */


#ifndef __ICEnroll2_INTERFACE_DEFINED__
#define __ICEnroll2_INTERFACE_DEFINED__

/* interface ICEnroll2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICEnroll2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("704ca730-c90b-11d1-9bec-00c04fc295e1")
    ICEnroll2 : public ICEnroll
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE addCertTypeToRequest( 
            /* [in] */ BSTR CertType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE addNameValuePairToSignature( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR Value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WriteCertToUserDS( 
            /* [retval][out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WriteCertToUserDS( 
            /* [in] */ BOOL fBool) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnableT61DNEncoding( 
            /* [retval][out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableT61DNEncoding( 
            /* [in] */ BOOL fBool) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICEnroll2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICEnroll2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICEnroll2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICEnroll2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createFilePKCS10 )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [in] */ BSTR wszPKCS10FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptFilePKCS7 )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createPKCS10 )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [retval][out] */ BSTR __RPC_FAR *pPKCS10);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptPKCS7 )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR PKCS7);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *getCertFromPKCS7 )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrCert);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumProviders )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrProvName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumContainers )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *freeRequestInfo )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR PKCS7OrPKCS10);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContainerName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContainer);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContainerName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrContainer);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProvider);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstrProvider);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeySpec )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeySpec )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseExistingKeySet )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fUseExistingKeys);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseExistingKeySet )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fUseExistingKeys);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GenKeyFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GenKeyFlags )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRequestCert )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fDelete);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRequestCert )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fDelete);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToCSP )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToCSP )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SPCFileName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SPCFileName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PVKFileName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PVKFileName )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *addCertTypeToRequest )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR CertType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *addNameValuePairToSignature )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR Value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToUserDS )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToUserDS )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableT61DNEncoding )( 
            ICEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableT61DNEncoding )( 
            ICEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        END_INTERFACE
    } ICEnroll2Vtbl;

    interface ICEnroll2
    {
        CONST_VTBL struct ICEnroll2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICEnroll2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICEnroll2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICEnroll2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICEnroll2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICEnroll2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICEnroll2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICEnroll2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICEnroll2_createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)	\
    (This)->lpVtbl -> createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)

#define ICEnroll2_acceptFilePKCS7(This,wszPKCS7FileName)	\
    (This)->lpVtbl -> acceptFilePKCS7(This,wszPKCS7FileName)

#define ICEnroll2_createPKCS10(This,DNName,Usage,pPKCS10)	\
    (This)->lpVtbl -> createPKCS10(This,DNName,Usage,pPKCS10)

#define ICEnroll2_acceptPKCS7(This,PKCS7)	\
    (This)->lpVtbl -> acceptPKCS7(This,PKCS7)

#define ICEnroll2_getCertFromPKCS7(This,wszPKCS7,pbstrCert)	\
    (This)->lpVtbl -> getCertFromPKCS7(This,wszPKCS7,pbstrCert)

#define ICEnroll2_enumProviders(This,dwIndex,dwFlags,pbstrProvName)	\
    (This)->lpVtbl -> enumProviders(This,dwIndex,dwFlags,pbstrProvName)

#define ICEnroll2_enumContainers(This,dwIndex,pbstr)	\
    (This)->lpVtbl -> enumContainers(This,dwIndex,pbstr)

#define ICEnroll2_freeRequestInfo(This,PKCS7OrPKCS10)	\
    (This)->lpVtbl -> freeRequestInfo(This,PKCS7OrPKCS10)

#define ICEnroll2_get_MyStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_MyStoreName(This,pbstrName)

#define ICEnroll2_put_MyStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_MyStoreName(This,bstrName)

#define ICEnroll2_get_MyStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_MyStoreType(This,pbstrType)

#define ICEnroll2_put_MyStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_MyStoreType(This,bstrType)

#define ICEnroll2_get_MyStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_MyStoreFlags(This,pdwFlags)

#define ICEnroll2_put_MyStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_MyStoreFlags(This,dwFlags)

#define ICEnroll2_get_CAStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_CAStoreName(This,pbstrName)

#define ICEnroll2_put_CAStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_CAStoreName(This,bstrName)

#define ICEnroll2_get_CAStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_CAStoreType(This,pbstrType)

#define ICEnroll2_put_CAStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_CAStoreType(This,bstrType)

#define ICEnroll2_get_CAStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_CAStoreFlags(This,pdwFlags)

#define ICEnroll2_put_CAStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_CAStoreFlags(This,dwFlags)

#define ICEnroll2_get_RootStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RootStoreName(This,pbstrName)

#define ICEnroll2_put_RootStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RootStoreName(This,bstrName)

#define ICEnroll2_get_RootStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RootStoreType(This,pbstrType)

#define ICEnroll2_put_RootStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RootStoreType(This,bstrType)

#define ICEnroll2_get_RootStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RootStoreFlags(This,pdwFlags)

#define ICEnroll2_put_RootStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RootStoreFlags(This,dwFlags)

#define ICEnroll2_get_RequestStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RequestStoreName(This,pbstrName)

#define ICEnroll2_put_RequestStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RequestStoreName(This,bstrName)

#define ICEnroll2_get_RequestStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RequestStoreType(This,pbstrType)

#define ICEnroll2_put_RequestStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RequestStoreType(This,bstrType)

#define ICEnroll2_get_RequestStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RequestStoreFlags(This,pdwFlags)

#define ICEnroll2_put_RequestStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RequestStoreFlags(This,dwFlags)

#define ICEnroll2_get_ContainerName(This,pbstrContainer)	\
    (This)->lpVtbl -> get_ContainerName(This,pbstrContainer)

#define ICEnroll2_put_ContainerName(This,bstrContainer)	\
    (This)->lpVtbl -> put_ContainerName(This,bstrContainer)

#define ICEnroll2_get_ProviderName(This,pbstrProvider)	\
    (This)->lpVtbl -> get_ProviderName(This,pbstrProvider)

#define ICEnroll2_put_ProviderName(This,bstrProvider)	\
    (This)->lpVtbl -> put_ProviderName(This,bstrProvider)

#define ICEnroll2_get_ProviderType(This,pdwType)	\
    (This)->lpVtbl -> get_ProviderType(This,pdwType)

#define ICEnroll2_put_ProviderType(This,dwType)	\
    (This)->lpVtbl -> put_ProviderType(This,dwType)

#define ICEnroll2_get_KeySpec(This,pdw)	\
    (This)->lpVtbl -> get_KeySpec(This,pdw)

#define ICEnroll2_put_KeySpec(This,dw)	\
    (This)->lpVtbl -> put_KeySpec(This,dw)

#define ICEnroll2_get_ProviderFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_ProviderFlags(This,pdwFlags)

#define ICEnroll2_put_ProviderFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_ProviderFlags(This,dwFlags)

#define ICEnroll2_get_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> get_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll2_put_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> put_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll2_get_GenKeyFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_GenKeyFlags(This,pdwFlags)

#define ICEnroll2_put_GenKeyFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_GenKeyFlags(This,dwFlags)

#define ICEnroll2_get_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> get_DeleteRequestCert(This,fDelete)

#define ICEnroll2_put_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> put_DeleteRequestCert(This,fDelete)

#define ICEnroll2_get_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToCSP(This,fBool)

#define ICEnroll2_put_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToCSP(This,fBool)

#define ICEnroll2_get_SPCFileName(This,pbstr)	\
    (This)->lpVtbl -> get_SPCFileName(This,pbstr)

#define ICEnroll2_put_SPCFileName(This,bstr)	\
    (This)->lpVtbl -> put_SPCFileName(This,bstr)

#define ICEnroll2_get_PVKFileName(This,pbstr)	\
    (This)->lpVtbl -> get_PVKFileName(This,pbstr)

#define ICEnroll2_put_PVKFileName(This,bstr)	\
    (This)->lpVtbl -> put_PVKFileName(This,bstr)

#define ICEnroll2_get_HashAlgorithm(This,pbstr)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,pbstr)

#define ICEnroll2_put_HashAlgorithm(This,bstr)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,bstr)


#define ICEnroll2_addCertTypeToRequest(This,CertType)	\
    (This)->lpVtbl -> addCertTypeToRequest(This,CertType)

#define ICEnroll2_addNameValuePairToSignature(This,Name,Value)	\
    (This)->lpVtbl -> addNameValuePairToSignature(This,Name,Value)

#define ICEnroll2_get_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToUserDS(This,fBool)

#define ICEnroll2_put_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToUserDS(This,fBool)

#define ICEnroll2_get_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> get_EnableT61DNEncoding(This,fBool)

#define ICEnroll2_put_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> put_EnableT61DNEncoding(This,fBool)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICEnroll2_addCertTypeToRequest_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [in] */ BSTR CertType);


void __RPC_STUB ICEnroll2_addCertTypeToRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll2_addNameValuePairToSignature_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR Value);


void __RPC_STUB ICEnroll2_addNameValuePairToSignature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll2_get_WriteCertToUserDS_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB ICEnroll2_get_WriteCertToUserDS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll2_put_WriteCertToUserDS_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB ICEnroll2_put_WriteCertToUserDS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll2_get_EnableT61DNEncoding_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB ICEnroll2_get_EnableT61DNEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll2_put_EnableT61DNEncoding_Proxy( 
    ICEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB ICEnroll2_put_EnableT61DNEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICEnroll2_INTERFACE_DEFINED__ */


#ifndef __ICEnroll3_INTERFACE_DEFINED__
#define __ICEnroll3_INTERFACE_DEFINED__

/* interface ICEnroll3 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICEnroll3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c28c2d95-b7de-11d2-a421-00c04f79fe8e")
    ICEnroll3 : public ICEnroll2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InstallPKCS7( 
            /* [in] */ BSTR PKCS7) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSupportedKeySpec( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwKeySpec) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyLen( 
            /* [in] */ BOOL fMin,
            /* [in] */ BOOL fExchange,
            /* [retval][out] */ DWORD __RPC_FAR *pdwKeySize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAlgs( 
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD algClass,
            /* [retval][out] */ DWORD __RPC_FAR *pdwAlgID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAlgName( 
            /* [in] */ DWORD algID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ReuseHardwareKeyIfUnableToGenNew( 
            /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ReuseHardwareKeyIfUnableToGenNew( 
            /* [retval][out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HashAlgID( 
            /* [in] */ DWORD hashAlgID) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HashAlgID( 
            /* [retval][out] */ DWORD __RPC_FAR *hashAlgID) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LimitExchangeKeyToEncipherment( 
            /* [in] */ BOOL fLimitExchangeKeyToEncipherment) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LimitExchangeKeyToEncipherment( 
            /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableSMIMECapabilities( 
            /* [in] */ BOOL fEnableSMIMECapabilities) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnableSMIMECapabilities( 
            /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICEnroll3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICEnroll3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICEnroll3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICEnroll3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createFilePKCS10 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [in] */ BSTR wszPKCS10FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptFilePKCS7 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createPKCS10 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR DNName,
            /* [in] */ BSTR Usage,
            /* [retval][out] */ BSTR __RPC_FAR *pPKCS10);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptPKCS7 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR PKCS7);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *getCertFromPKCS7 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR wszPKCS7,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrCert);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumProviders )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrProvName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumContainers )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *freeRequestInfo )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR PKCS7OrPKCS10);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContainerName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrContainer);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContainerName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrContainer);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrProvider);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstrProvider);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeySpec )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeySpec )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseExistingKeySet )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fUseExistingKeys);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseExistingKeySet )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fUseExistingKeys);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GenKeyFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GenKeyFlags )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRequestCert )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fDelete);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRequestCert )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fDelete);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToCSP )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToCSP )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SPCFileName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SPCFileName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PVKFileName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PVKFileName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR bstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *addCertTypeToRequest )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR CertType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *addNameValuePairToSignature )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR Value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToUserDS )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToUserDS )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableT61DNEncoding )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableT61DNEncoding )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallPKCS7 )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BSTR PKCS7);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            ICEnroll3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSupportedKeySpec )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwKeySpec);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetKeyLen )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fMin,
            /* [in] */ BOOL fExchange,
            /* [retval][out] */ DWORD __RPC_FAR *pdwKeySize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumAlgs )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD algClass,
            /* [retval][out] */ DWORD __RPC_FAR *pdwAlgID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAlgName )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD algID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReuseHardwareKeyIfUnableToGenNew )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReuseHardwareKeyIfUnableToGenNew )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgID )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ DWORD hashAlgID);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgID )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *hashAlgID);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LimitExchangeKeyToEncipherment )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fLimitExchangeKeyToEncipherment);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LimitExchangeKeyToEncipherment )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSMIMECapabilities )( 
            ICEnroll3 __RPC_FAR * This,
            /* [in] */ BOOL fEnableSMIMECapabilities);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSMIMECapabilities )( 
            ICEnroll3 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities);
        
        END_INTERFACE
    } ICEnroll3Vtbl;

    interface ICEnroll3
    {
        CONST_VTBL struct ICEnroll3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICEnroll3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICEnroll3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICEnroll3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICEnroll3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICEnroll3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICEnroll3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICEnroll3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICEnroll3_createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)	\
    (This)->lpVtbl -> createFilePKCS10(This,DNName,Usage,wszPKCS10FileName)

#define ICEnroll3_acceptFilePKCS7(This,wszPKCS7FileName)	\
    (This)->lpVtbl -> acceptFilePKCS7(This,wszPKCS7FileName)

#define ICEnroll3_createPKCS10(This,DNName,Usage,pPKCS10)	\
    (This)->lpVtbl -> createPKCS10(This,DNName,Usage,pPKCS10)

#define ICEnroll3_acceptPKCS7(This,PKCS7)	\
    (This)->lpVtbl -> acceptPKCS7(This,PKCS7)

#define ICEnroll3_getCertFromPKCS7(This,wszPKCS7,pbstrCert)	\
    (This)->lpVtbl -> getCertFromPKCS7(This,wszPKCS7,pbstrCert)

#define ICEnroll3_enumProviders(This,dwIndex,dwFlags,pbstrProvName)	\
    (This)->lpVtbl -> enumProviders(This,dwIndex,dwFlags,pbstrProvName)

#define ICEnroll3_enumContainers(This,dwIndex,pbstr)	\
    (This)->lpVtbl -> enumContainers(This,dwIndex,pbstr)

#define ICEnroll3_freeRequestInfo(This,PKCS7OrPKCS10)	\
    (This)->lpVtbl -> freeRequestInfo(This,PKCS7OrPKCS10)

#define ICEnroll3_get_MyStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_MyStoreName(This,pbstrName)

#define ICEnroll3_put_MyStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_MyStoreName(This,bstrName)

#define ICEnroll3_get_MyStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_MyStoreType(This,pbstrType)

#define ICEnroll3_put_MyStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_MyStoreType(This,bstrType)

#define ICEnroll3_get_MyStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_MyStoreFlags(This,pdwFlags)

#define ICEnroll3_put_MyStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_MyStoreFlags(This,dwFlags)

#define ICEnroll3_get_CAStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_CAStoreName(This,pbstrName)

#define ICEnroll3_put_CAStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_CAStoreName(This,bstrName)

#define ICEnroll3_get_CAStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_CAStoreType(This,pbstrType)

#define ICEnroll3_put_CAStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_CAStoreType(This,bstrType)

#define ICEnroll3_get_CAStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_CAStoreFlags(This,pdwFlags)

#define ICEnroll3_put_CAStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_CAStoreFlags(This,dwFlags)

#define ICEnroll3_get_RootStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RootStoreName(This,pbstrName)

#define ICEnroll3_put_RootStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RootStoreName(This,bstrName)

#define ICEnroll3_get_RootStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RootStoreType(This,pbstrType)

#define ICEnroll3_put_RootStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RootStoreType(This,bstrType)

#define ICEnroll3_get_RootStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RootStoreFlags(This,pdwFlags)

#define ICEnroll3_put_RootStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RootStoreFlags(This,dwFlags)

#define ICEnroll3_get_RequestStoreName(This,pbstrName)	\
    (This)->lpVtbl -> get_RequestStoreName(This,pbstrName)

#define ICEnroll3_put_RequestStoreName(This,bstrName)	\
    (This)->lpVtbl -> put_RequestStoreName(This,bstrName)

#define ICEnroll3_get_RequestStoreType(This,pbstrType)	\
    (This)->lpVtbl -> get_RequestStoreType(This,pbstrType)

#define ICEnroll3_put_RequestStoreType(This,bstrType)	\
    (This)->lpVtbl -> put_RequestStoreType(This,bstrType)

#define ICEnroll3_get_RequestStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RequestStoreFlags(This,pdwFlags)

#define ICEnroll3_put_RequestStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RequestStoreFlags(This,dwFlags)

#define ICEnroll3_get_ContainerName(This,pbstrContainer)	\
    (This)->lpVtbl -> get_ContainerName(This,pbstrContainer)

#define ICEnroll3_put_ContainerName(This,bstrContainer)	\
    (This)->lpVtbl -> put_ContainerName(This,bstrContainer)

#define ICEnroll3_get_ProviderName(This,pbstrProvider)	\
    (This)->lpVtbl -> get_ProviderName(This,pbstrProvider)

#define ICEnroll3_put_ProviderName(This,bstrProvider)	\
    (This)->lpVtbl -> put_ProviderName(This,bstrProvider)

#define ICEnroll3_get_ProviderType(This,pdwType)	\
    (This)->lpVtbl -> get_ProviderType(This,pdwType)

#define ICEnroll3_put_ProviderType(This,dwType)	\
    (This)->lpVtbl -> put_ProviderType(This,dwType)

#define ICEnroll3_get_KeySpec(This,pdw)	\
    (This)->lpVtbl -> get_KeySpec(This,pdw)

#define ICEnroll3_put_KeySpec(This,dw)	\
    (This)->lpVtbl -> put_KeySpec(This,dw)

#define ICEnroll3_get_ProviderFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_ProviderFlags(This,pdwFlags)

#define ICEnroll3_put_ProviderFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_ProviderFlags(This,dwFlags)

#define ICEnroll3_get_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> get_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll3_put_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> put_UseExistingKeySet(This,fUseExistingKeys)

#define ICEnroll3_get_GenKeyFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_GenKeyFlags(This,pdwFlags)

#define ICEnroll3_put_GenKeyFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_GenKeyFlags(This,dwFlags)

#define ICEnroll3_get_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> get_DeleteRequestCert(This,fDelete)

#define ICEnroll3_put_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> put_DeleteRequestCert(This,fDelete)

#define ICEnroll3_get_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToCSP(This,fBool)

#define ICEnroll3_put_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToCSP(This,fBool)

#define ICEnroll3_get_SPCFileName(This,pbstr)	\
    (This)->lpVtbl -> get_SPCFileName(This,pbstr)

#define ICEnroll3_put_SPCFileName(This,bstr)	\
    (This)->lpVtbl -> put_SPCFileName(This,bstr)

#define ICEnroll3_get_PVKFileName(This,pbstr)	\
    (This)->lpVtbl -> get_PVKFileName(This,pbstr)

#define ICEnroll3_put_PVKFileName(This,bstr)	\
    (This)->lpVtbl -> put_PVKFileName(This,bstr)

#define ICEnroll3_get_HashAlgorithm(This,pbstr)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,pbstr)

#define ICEnroll3_put_HashAlgorithm(This,bstr)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,bstr)


#define ICEnroll3_addCertTypeToRequest(This,CertType)	\
    (This)->lpVtbl -> addCertTypeToRequest(This,CertType)

#define ICEnroll3_addNameValuePairToSignature(This,Name,Value)	\
    (This)->lpVtbl -> addNameValuePairToSignature(This,Name,Value)

#define ICEnroll3_get_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToUserDS(This,fBool)

#define ICEnroll3_put_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToUserDS(This,fBool)

#define ICEnroll3_get_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> get_EnableT61DNEncoding(This,fBool)

#define ICEnroll3_put_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> put_EnableT61DNEncoding(This,fBool)


#define ICEnroll3_InstallPKCS7(This,PKCS7)	\
    (This)->lpVtbl -> InstallPKCS7(This,PKCS7)

#define ICEnroll3_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define ICEnroll3_GetSupportedKeySpec(This,pdwKeySpec)	\
    (This)->lpVtbl -> GetSupportedKeySpec(This,pdwKeySpec)

#define ICEnroll3_GetKeyLen(This,fMin,fExchange,pdwKeySize)	\
    (This)->lpVtbl -> GetKeyLen(This,fMin,fExchange,pdwKeySize)

#define ICEnroll3_EnumAlgs(This,dwIndex,algClass,pdwAlgID)	\
    (This)->lpVtbl -> EnumAlgs(This,dwIndex,algClass,pdwAlgID)

#define ICEnroll3_GetAlgName(This,algID,pbstr)	\
    (This)->lpVtbl -> GetAlgName(This,algID,pbstr)

#define ICEnroll3_put_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)	\
    (This)->lpVtbl -> put_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)

#define ICEnroll3_get_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)	\
    (This)->lpVtbl -> get_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)

#define ICEnroll3_put_HashAlgID(This,hashAlgID)	\
    (This)->lpVtbl -> put_HashAlgID(This,hashAlgID)

#define ICEnroll3_get_HashAlgID(This,hashAlgID)	\
    (This)->lpVtbl -> get_HashAlgID(This,hashAlgID)

#define ICEnroll3_put_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)	\
    (This)->lpVtbl -> put_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)

#define ICEnroll3_get_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)	\
    (This)->lpVtbl -> get_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)

#define ICEnroll3_put_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)	\
    (This)->lpVtbl -> put_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)

#define ICEnroll3_get_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)	\
    (This)->lpVtbl -> get_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICEnroll3_InstallPKCS7_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ BSTR PKCS7);


void __RPC_STUB ICEnroll3_InstallPKCS7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll3_Reset_Proxy( 
    ICEnroll3 __RPC_FAR * This);


void __RPC_STUB ICEnroll3_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll3_GetSupportedKeySpec_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwKeySpec);


void __RPC_STUB ICEnroll3_GetSupportedKeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll3_GetKeyLen_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ BOOL fMin,
    /* [in] */ BOOL fExchange,
    /* [retval][out] */ DWORD __RPC_FAR *pdwKeySize);


void __RPC_STUB ICEnroll3_GetKeyLen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll3_EnumAlgs_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [in] */ DWORD algClass,
    /* [retval][out] */ DWORD __RPC_FAR *pdwAlgID);


void __RPC_STUB ICEnroll3_EnumAlgs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICEnroll3_GetAlgName_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ DWORD algID,
    /* [retval][out] */ BSTR __RPC_FAR *pbstr);


void __RPC_STUB ICEnroll3_GetAlgName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll3_put_ReuseHardwareKeyIfUnableToGenNew_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew);


void __RPC_STUB ICEnroll3_put_ReuseHardwareKeyIfUnableToGenNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll3_get_ReuseHardwareKeyIfUnableToGenNew_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew);


void __RPC_STUB ICEnroll3_get_ReuseHardwareKeyIfUnableToGenNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll3_put_HashAlgID_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ DWORD hashAlgID);


void __RPC_STUB ICEnroll3_put_HashAlgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll3_get_HashAlgID_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *hashAlgID);


void __RPC_STUB ICEnroll3_get_HashAlgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll3_put_LimitExchangeKeyToEncipherment_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ BOOL fLimitExchangeKeyToEncipherment);


void __RPC_STUB ICEnroll3_put_LimitExchangeKeyToEncipherment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll3_get_LimitExchangeKeyToEncipherment_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment);


void __RPC_STUB ICEnroll3_get_LimitExchangeKeyToEncipherment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE ICEnroll3_put_EnableSMIMECapabilities_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [in] */ BOOL fEnableSMIMECapabilities);


void __RPC_STUB ICEnroll3_put_EnableSMIMECapabilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE ICEnroll3_get_EnableSMIMECapabilities_Proxy( 
    ICEnroll3 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities);


void __RPC_STUB ICEnroll3_get_EnableSMIMECapabilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICEnroll3_INTERFACE_DEFINED__ */


#ifndef __IEnroll_INTERFACE_DEFINED__
#define __IEnroll_INTERFACE_DEFINED__

/* interface IEnroll */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_IEnroll;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("acaa7838-4585-11d1-ab57-00c04fc295e1")
    IEnroll : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE createFilePKCS10WStr( 
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [in] */ LPCWSTR wszPKCS10FileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE acceptFilePKCS7WStr( 
            /* [in] */ LPCWSTR wszPKCS7FileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE createPKCS10WStr( 
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs10Blob) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE acceptPKCS7Blob( 
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7) = 0;
        
        virtual PCCERT_CONTEXT STDMETHODCALLTYPE getCertContextFromPKCS7( 
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7) = 0;
        
        virtual HCERTSTORE STDMETHODCALLTYPE getMyStore( void) = 0;
        
        virtual HCERTSTORE STDMETHODCALLTYPE getCAStore( void) = 0;
        
        virtual HCERTSTORE STDMETHODCALLTYPE getROOTHStore( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE enumProvidersWStr( 
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out] */ LPWSTR __RPC_FAR *pbstrProvName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE enumContainersWStr( 
            /* [in] */ DWORD dwIndex,
            /* [out] */ LPWSTR __RPC_FAR *pbstr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE freeRequestInfoBlob( 
            /* [in] */ CRYPT_DATA_BLOB pkcs7OrPkcs10) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreNameWStr( 
            /* [in] */ LPWSTR szwName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreTypeWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreTypeWStr( 
            /* [in] */ LPWSTR szwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MyStoreFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MyStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreNameWStr( 
            /* [in] */ LPWSTR szwName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreTypeWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreTypeWStr( 
            /* [in] */ LPWSTR szwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CAStoreFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CAStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreNameWStr( 
            /* [in] */ LPWSTR szwName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreTypeWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreTypeWStr( 
            /* [in] */ LPWSTR szwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RootStoreFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RootStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreNameWStr( 
            /* [in] */ LPWSTR szwName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreTypeWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreTypeWStr( 
            /* [in] */ LPWSTR szwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequestStoreFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RequestStoreFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ContainerNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwContainer) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ContainerNameWStr( 
            /* [in] */ LPWSTR szwContainer) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szwProvider) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderNameWStr( 
            /* [in] */ LPWSTR szwProvider) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderType( 
            /* [out] */ DWORD __RPC_FAR *pdwType) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderType( 
            /* [in] */ DWORD dwType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_KeySpec( 
            /* [out] */ DWORD __RPC_FAR *pdw) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_KeySpec( 
            /* [in] */ DWORD dw) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProviderFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProviderFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UseExistingKeySet( 
            /* [out] */ BOOL __RPC_FAR *fUseExistingKeys) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UseExistingKeySet( 
            /* [in] */ BOOL fUseExistingKeys) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GenKeyFlags( 
            /* [out] */ DWORD __RPC_FAR *pdwFlags) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_GenKeyFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DeleteRequestCert( 
            /* [out] */ BOOL __RPC_FAR *fDelete) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DeleteRequestCert( 
            /* [in] */ BOOL fDelete) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WriteCertToUserDS( 
            /* [out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WriteCertToUserDS( 
            /* [in] */ BOOL fBool) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnableT61DNEncoding( 
            /* [out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableT61DNEncoding( 
            /* [in] */ BOOL fBool) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WriteCertToCSP( 
            /* [out] */ BOOL __RPC_FAR *fBool) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WriteCertToCSP( 
            /* [in] */ BOOL fBool) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SPCFileNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szw) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SPCFileNameWStr( 
            /* [in] */ LPWSTR szw) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PVKFileNameWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szw) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PVKFileNameWStr( 
            /* [in] */ LPWSTR szw) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithmWStr( 
            /* [out] */ LPWSTR __RPC_FAR *szw) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithmWStr( 
            /* [in] */ LPWSTR szw) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RenewalCertificate( 
            /* [out] */ PCCERT_CONTEXT __RPC_FAR *ppCertContext) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_RenewalCertificate( 
            /* [in] */ PCCERT_CONTEXT pCertContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddCertTypeToRequestWStr( 
            /* [in] */ LPWSTR szw) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddNameValuePairToSignatureWStr( 
            /* [in] */ LPWSTR Name,
            /* [in] */ LPWSTR Value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddExtensionsToRequest( 
            /* [in] */ PCERT_EXTENSIONS pCertExtensions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddAuthenticatedAttributesToPKCS7Request( 
            /* [in] */ PCRYPT_ATTRIBUTES pAttributes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePKCS7RequestFromRequest( 
            /* [in] */ PCRYPT_DATA_BLOB pRequest,
            /* [in] */ PCCERT_CONTEXT pSigningCertContext,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs7Blob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnrollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnroll __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnroll __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createFilePKCS10WStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [in] */ LPCWSTR wszPKCS10FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptFilePKCS7WStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPCWSTR wszPKCS7FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createPKCS10WStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs10Blob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptPKCS7Blob )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);
        
        PCCERT_CONTEXT ( STDMETHODCALLTYPE __RPC_FAR *getCertContextFromPKCS7 )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getMyStore )( 
            IEnroll __RPC_FAR * This);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getCAStore )( 
            IEnroll __RPC_FAR * This);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getROOTHStore )( 
            IEnroll __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumProvidersWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out] */ LPWSTR __RPC_FAR *pbstrProvName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumContainersWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out] */ LPWSTR __RPC_FAR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *freeRequestInfoBlob )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ CRYPT_DATA_BLOB pkcs7OrPkcs10);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreTypeWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContainerNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwContainer);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContainerNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwContainer);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwProvider);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szwProvider);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeySpec )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeySpec )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseExistingKeySet )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fUseExistingKeys);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseExistingKeySet )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ BOOL fUseExistingKeys);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GenKeyFlags )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GenKeyFlags )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRequestCert )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fDelete);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRequestCert )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ BOOL fDelete);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToUserDS )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToUserDS )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableT61DNEncoding )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableT61DNEncoding )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToCSP )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToCSP )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SPCFileNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SPCFileNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PVKFileNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PVKFileNameWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithmWStr )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithmWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RenewalCertificate )( 
            IEnroll __RPC_FAR * This,
            /* [out] */ PCCERT_CONTEXT __RPC_FAR *ppCertContext);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RenewalCertificate )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCCERT_CONTEXT pCertContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddCertTypeToRequestWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNameValuePairToSignatureWStr )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ LPWSTR Name,
            /* [in] */ LPWSTR Value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddExtensionsToRequest )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCERT_EXTENSIONS pCertExtensions);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddAuthenticatedAttributesToPKCS7Request )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCRYPT_ATTRIBUTES pAttributes);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePKCS7RequestFromRequest )( 
            IEnroll __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pRequest,
            /* [in] */ PCCERT_CONTEXT pSigningCertContext,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs7Blob);
        
        END_INTERFACE
    } IEnrollVtbl;

    interface IEnroll
    {
        CONST_VTBL struct IEnrollVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnroll_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnroll_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnroll_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnroll_createFilePKCS10WStr(This,DNName,Usage,wszPKCS10FileName)	\
    (This)->lpVtbl -> createFilePKCS10WStr(This,DNName,Usage,wszPKCS10FileName)

#define IEnroll_acceptFilePKCS7WStr(This,wszPKCS7FileName)	\
    (This)->lpVtbl -> acceptFilePKCS7WStr(This,wszPKCS7FileName)

#define IEnroll_createPKCS10WStr(This,DNName,Usage,pPkcs10Blob)	\
    (This)->lpVtbl -> createPKCS10WStr(This,DNName,Usage,pPkcs10Blob)

#define IEnroll_acceptPKCS7Blob(This,pBlobPKCS7)	\
    (This)->lpVtbl -> acceptPKCS7Blob(This,pBlobPKCS7)

#define IEnroll_getCertContextFromPKCS7(This,pBlobPKCS7)	\
    (This)->lpVtbl -> getCertContextFromPKCS7(This,pBlobPKCS7)

#define IEnroll_getMyStore(This)	\
    (This)->lpVtbl -> getMyStore(This)

#define IEnroll_getCAStore(This)	\
    (This)->lpVtbl -> getCAStore(This)

#define IEnroll_getROOTHStore(This)	\
    (This)->lpVtbl -> getROOTHStore(This)

#define IEnroll_enumProvidersWStr(This,dwIndex,dwFlags,pbstrProvName)	\
    (This)->lpVtbl -> enumProvidersWStr(This,dwIndex,dwFlags,pbstrProvName)

#define IEnroll_enumContainersWStr(This,dwIndex,pbstr)	\
    (This)->lpVtbl -> enumContainersWStr(This,dwIndex,pbstr)

#define IEnroll_freeRequestInfoBlob(This,pkcs7OrPkcs10)	\
    (This)->lpVtbl -> freeRequestInfoBlob(This,pkcs7OrPkcs10)

#define IEnroll_get_MyStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_MyStoreNameWStr(This,szwName)

#define IEnroll_put_MyStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_MyStoreNameWStr(This,szwName)

#define IEnroll_get_MyStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_MyStoreTypeWStr(This,szwType)

#define IEnroll_put_MyStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_MyStoreTypeWStr(This,szwType)

#define IEnroll_get_MyStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_MyStoreFlags(This,pdwFlags)

#define IEnroll_put_MyStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_MyStoreFlags(This,dwFlags)

#define IEnroll_get_CAStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_CAStoreNameWStr(This,szwName)

#define IEnroll_put_CAStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_CAStoreNameWStr(This,szwName)

#define IEnroll_get_CAStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_CAStoreTypeWStr(This,szwType)

#define IEnroll_put_CAStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_CAStoreTypeWStr(This,szwType)

#define IEnroll_get_CAStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_CAStoreFlags(This,pdwFlags)

#define IEnroll_put_CAStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_CAStoreFlags(This,dwFlags)

#define IEnroll_get_RootStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_RootStoreNameWStr(This,szwName)

#define IEnroll_put_RootStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_RootStoreNameWStr(This,szwName)

#define IEnroll_get_RootStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_RootStoreTypeWStr(This,szwType)

#define IEnroll_put_RootStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_RootStoreTypeWStr(This,szwType)

#define IEnroll_get_RootStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RootStoreFlags(This,pdwFlags)

#define IEnroll_put_RootStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RootStoreFlags(This,dwFlags)

#define IEnroll_get_RequestStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_RequestStoreNameWStr(This,szwName)

#define IEnroll_put_RequestStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_RequestStoreNameWStr(This,szwName)

#define IEnroll_get_RequestStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_RequestStoreTypeWStr(This,szwType)

#define IEnroll_put_RequestStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_RequestStoreTypeWStr(This,szwType)

#define IEnroll_get_RequestStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RequestStoreFlags(This,pdwFlags)

#define IEnroll_put_RequestStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RequestStoreFlags(This,dwFlags)

#define IEnroll_get_ContainerNameWStr(This,szwContainer)	\
    (This)->lpVtbl -> get_ContainerNameWStr(This,szwContainer)

#define IEnroll_put_ContainerNameWStr(This,szwContainer)	\
    (This)->lpVtbl -> put_ContainerNameWStr(This,szwContainer)

#define IEnroll_get_ProviderNameWStr(This,szwProvider)	\
    (This)->lpVtbl -> get_ProviderNameWStr(This,szwProvider)

#define IEnroll_put_ProviderNameWStr(This,szwProvider)	\
    (This)->lpVtbl -> put_ProviderNameWStr(This,szwProvider)

#define IEnroll_get_ProviderType(This,pdwType)	\
    (This)->lpVtbl -> get_ProviderType(This,pdwType)

#define IEnroll_put_ProviderType(This,dwType)	\
    (This)->lpVtbl -> put_ProviderType(This,dwType)

#define IEnroll_get_KeySpec(This,pdw)	\
    (This)->lpVtbl -> get_KeySpec(This,pdw)

#define IEnroll_put_KeySpec(This,dw)	\
    (This)->lpVtbl -> put_KeySpec(This,dw)

#define IEnroll_get_ProviderFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_ProviderFlags(This,pdwFlags)

#define IEnroll_put_ProviderFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_ProviderFlags(This,dwFlags)

#define IEnroll_get_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> get_UseExistingKeySet(This,fUseExistingKeys)

#define IEnroll_put_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> put_UseExistingKeySet(This,fUseExistingKeys)

#define IEnroll_get_GenKeyFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_GenKeyFlags(This,pdwFlags)

#define IEnroll_put_GenKeyFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_GenKeyFlags(This,dwFlags)

#define IEnroll_get_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> get_DeleteRequestCert(This,fDelete)

#define IEnroll_put_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> put_DeleteRequestCert(This,fDelete)

#define IEnroll_get_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToUserDS(This,fBool)

#define IEnroll_put_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToUserDS(This,fBool)

#define IEnroll_get_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> get_EnableT61DNEncoding(This,fBool)

#define IEnroll_put_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> put_EnableT61DNEncoding(This,fBool)

#define IEnroll_get_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToCSP(This,fBool)

#define IEnroll_put_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToCSP(This,fBool)

#define IEnroll_get_SPCFileNameWStr(This,szw)	\
    (This)->lpVtbl -> get_SPCFileNameWStr(This,szw)

#define IEnroll_put_SPCFileNameWStr(This,szw)	\
    (This)->lpVtbl -> put_SPCFileNameWStr(This,szw)

#define IEnroll_get_PVKFileNameWStr(This,szw)	\
    (This)->lpVtbl -> get_PVKFileNameWStr(This,szw)

#define IEnroll_put_PVKFileNameWStr(This,szw)	\
    (This)->lpVtbl -> put_PVKFileNameWStr(This,szw)

#define IEnroll_get_HashAlgorithmWStr(This,szw)	\
    (This)->lpVtbl -> get_HashAlgorithmWStr(This,szw)

#define IEnroll_put_HashAlgorithmWStr(This,szw)	\
    (This)->lpVtbl -> put_HashAlgorithmWStr(This,szw)

#define IEnroll_get_RenewalCertificate(This,ppCertContext)	\
    (This)->lpVtbl -> get_RenewalCertificate(This,ppCertContext)

#define IEnroll_put_RenewalCertificate(This,pCertContext)	\
    (This)->lpVtbl -> put_RenewalCertificate(This,pCertContext)

#define IEnroll_AddCertTypeToRequestWStr(This,szw)	\
    (This)->lpVtbl -> AddCertTypeToRequestWStr(This,szw)

#define IEnroll_AddNameValuePairToSignatureWStr(This,Name,Value)	\
    (This)->lpVtbl -> AddNameValuePairToSignatureWStr(This,Name,Value)

#define IEnroll_AddExtensionsToRequest(This,pCertExtensions)	\
    (This)->lpVtbl -> AddExtensionsToRequest(This,pCertExtensions)

#define IEnroll_AddAuthenticatedAttributesToPKCS7Request(This,pAttributes)	\
    (This)->lpVtbl -> AddAuthenticatedAttributesToPKCS7Request(This,pAttributes)

#define IEnroll_CreatePKCS7RequestFromRequest(This,pRequest,pSigningCertContext,pPkcs7Blob)	\
    (This)->lpVtbl -> CreatePKCS7RequestFromRequest(This,pRequest,pSigningCertContext,pPkcs7Blob)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnroll_createFilePKCS10WStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPCWSTR DNName,
    /* [in] */ LPCWSTR Usage,
    /* [in] */ LPCWSTR wszPKCS10FileName);


void __RPC_STUB IEnroll_createFilePKCS10WStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_acceptFilePKCS7WStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPCWSTR wszPKCS7FileName);


void __RPC_STUB IEnroll_acceptFilePKCS7WStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_createPKCS10WStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPCWSTR DNName,
    /* [in] */ LPCWSTR Usage,
    /* [out] */ PCRYPT_DATA_BLOB pPkcs10Blob);


void __RPC_STUB IEnroll_createPKCS10WStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_acceptPKCS7Blob_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);


void __RPC_STUB IEnroll_acceptPKCS7Blob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


PCCERT_CONTEXT STDMETHODCALLTYPE IEnroll_getCertContextFromPKCS7_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);


void __RPC_STUB IEnroll_getCertContextFromPKCS7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HCERTSTORE STDMETHODCALLTYPE IEnroll_getMyStore_Proxy( 
    IEnroll __RPC_FAR * This);


void __RPC_STUB IEnroll_getMyStore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HCERTSTORE STDMETHODCALLTYPE IEnroll_getCAStore_Proxy( 
    IEnroll __RPC_FAR * This);


void __RPC_STUB IEnroll_getCAStore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HCERTSTORE STDMETHODCALLTYPE IEnroll_getROOTHStore_Proxy( 
    IEnroll __RPC_FAR * This);


void __RPC_STUB IEnroll_getROOTHStore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_enumProvidersWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [in] */ DWORD dwFlags,
    /* [out] */ LPWSTR __RPC_FAR *pbstrProvName);


void __RPC_STUB IEnroll_enumProvidersWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_enumContainersWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [out] */ LPWSTR __RPC_FAR *pbstr);


void __RPC_STUB IEnroll_enumContainersWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_freeRequestInfoBlob_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ CRYPT_DATA_BLOB pkcs7OrPkcs10);


void __RPC_STUB IEnroll_freeRequestInfoBlob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_MyStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwName);


void __RPC_STUB IEnroll_get_MyStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_MyStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwName);


void __RPC_STUB IEnroll_put_MyStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_MyStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwType);


void __RPC_STUB IEnroll_get_MyStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_MyStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwType);


void __RPC_STUB IEnroll_put_MyStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_MyStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_MyStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_MyStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_MyStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_CAStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwName);


void __RPC_STUB IEnroll_get_CAStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_CAStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwName);


void __RPC_STUB IEnroll_put_CAStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_CAStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwType);


void __RPC_STUB IEnroll_get_CAStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_CAStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwType);


void __RPC_STUB IEnroll_put_CAStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_CAStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_CAStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_CAStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_CAStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RootStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwName);


void __RPC_STUB IEnroll_get_RootStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RootStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwName);


void __RPC_STUB IEnroll_put_RootStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RootStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwType);


void __RPC_STUB IEnroll_get_RootStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RootStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwType);


void __RPC_STUB IEnroll_put_RootStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RootStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_RootStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RootStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_RootStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RequestStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwName);


void __RPC_STUB IEnroll_get_RequestStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RequestStoreNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwName);


void __RPC_STUB IEnroll_put_RequestStoreNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RequestStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwType);


void __RPC_STUB IEnroll_get_RequestStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RequestStoreTypeWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwType);


void __RPC_STUB IEnroll_put_RequestStoreTypeWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RequestStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_RequestStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RequestStoreFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_RequestStoreFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_ContainerNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwContainer);


void __RPC_STUB IEnroll_get_ContainerNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_ContainerNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwContainer);


void __RPC_STUB IEnroll_put_ContainerNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_ProviderNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szwProvider);


void __RPC_STUB IEnroll_get_ProviderNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_ProviderNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szwProvider);


void __RPC_STUB IEnroll_put_ProviderNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_ProviderType_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwType);


void __RPC_STUB IEnroll_get_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_ProviderType_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwType);


void __RPC_STUB IEnroll_put_ProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_KeySpec_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdw);


void __RPC_STUB IEnroll_get_KeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_KeySpec_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dw);


void __RPC_STUB IEnroll_put_KeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_ProviderFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_ProviderFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_ProviderFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_ProviderFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_UseExistingKeySet_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fUseExistingKeys);


void __RPC_STUB IEnroll_get_UseExistingKeySet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_UseExistingKeySet_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ BOOL fUseExistingKeys);


void __RPC_STUB IEnroll_put_UseExistingKeySet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_GenKeyFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwFlags);


void __RPC_STUB IEnroll_get_GenKeyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_GenKeyFlags_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IEnroll_put_GenKeyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_DeleteRequestCert_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fDelete);


void __RPC_STUB IEnroll_get_DeleteRequestCert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_DeleteRequestCert_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ BOOL fDelete);


void __RPC_STUB IEnroll_put_DeleteRequestCert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_WriteCertToUserDS_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB IEnroll_get_WriteCertToUserDS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_WriteCertToUserDS_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB IEnroll_put_WriteCertToUserDS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_EnableT61DNEncoding_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB IEnroll_get_EnableT61DNEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_EnableT61DNEncoding_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB IEnroll_put_EnableT61DNEncoding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_WriteCertToCSP_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fBool);


void __RPC_STUB IEnroll_get_WriteCertToCSP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_WriteCertToCSP_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ BOOL fBool);


void __RPC_STUB IEnroll_put_WriteCertToCSP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_SPCFileNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szw);


void __RPC_STUB IEnroll_get_SPCFileNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_SPCFileNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szw);


void __RPC_STUB IEnroll_put_SPCFileNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_PVKFileNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szw);


void __RPC_STUB IEnroll_get_PVKFileNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_PVKFileNameWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szw);


void __RPC_STUB IEnroll_put_PVKFileNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_HashAlgorithmWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ LPWSTR __RPC_FAR *szw);


void __RPC_STUB IEnroll_get_HashAlgorithmWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_HashAlgorithmWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szw);


void __RPC_STUB IEnroll_put_HashAlgorithmWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll_get_RenewalCertificate_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [out] */ PCCERT_CONTEXT __RPC_FAR *ppCertContext);


void __RPC_STUB IEnroll_get_RenewalCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll_put_RenewalCertificate_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCCERT_CONTEXT pCertContext);


void __RPC_STUB IEnroll_put_RenewalCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_AddCertTypeToRequestWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR szw);


void __RPC_STUB IEnroll_AddCertTypeToRequestWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_AddNameValuePairToSignatureWStr_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ LPWSTR Name,
    /* [in] */ LPWSTR Value);


void __RPC_STUB IEnroll_AddNameValuePairToSignatureWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_AddExtensionsToRequest_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCERT_EXTENSIONS pCertExtensions);


void __RPC_STUB IEnroll_AddExtensionsToRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_AddAuthenticatedAttributesToPKCS7Request_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCRYPT_ATTRIBUTES pAttributes);


void __RPC_STUB IEnroll_AddAuthenticatedAttributesToPKCS7Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll_CreatePKCS7RequestFromRequest_Proxy( 
    IEnroll __RPC_FAR * This,
    /* [in] */ PCRYPT_DATA_BLOB pRequest,
    /* [in] */ PCCERT_CONTEXT pSigningCertContext,
    /* [out] */ PCRYPT_DATA_BLOB pPkcs7Blob);


void __RPC_STUB IEnroll_CreatePKCS7RequestFromRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnroll_INTERFACE_DEFINED__ */


#ifndef __IEnroll2_INTERFACE_DEFINED__
#define __IEnroll2_INTERFACE_DEFINED__

/* interface IEnroll2 */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_IEnroll2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c080e199-b7df-11d2-a421-00c04f79fe8e")
    IEnroll2 : public IEnroll
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InstallPKCS7Blob( 
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSupportedKeySpec( 
            /* [out] */ DWORD __RPC_FAR *pdwKeySpec) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyLen( 
            /* [in] */ BOOL fMin,
            /* [in] */ BOOL fExchange,
            /* [out] */ DWORD __RPC_FAR *pdwKeySize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAlgs( 
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD algClass,
            /* [out] */ DWORD __RPC_FAR *pdwAlgID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAlgNameWStr( 
            /* [in] */ DWORD algID,
            /* [out] */ LPWSTR __RPC_FAR *ppwsz) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ReuseHardwareKeyIfUnableToGenNew( 
            /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ReuseHardwareKeyIfUnableToGenNew( 
            /* [out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_HashAlgID( 
            /* [in] */ DWORD hashAlgID) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HashAlgID( 
            /* [retval][out] */ DWORD __RPC_FAR *hashAlgID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHStoreMy( 
            /* [in] */ HCERTSTORE hStore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHStoreCA( 
            /* [in] */ HCERTSTORE hStore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHStoreROOT( 
            /* [in] */ HCERTSTORE hStore) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHStoreRequest( 
            /* [in] */ HCERTSTORE hStore) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LimitExchangeKeyToEncipherment( 
            /* [in] */ BOOL fLimitExchangeKeyToEncipherment) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LimitExchangeKeyToEncipherment( 
            /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableSMIMECapabilities( 
            /* [in] */ BOOL fEnableSMIMECapabilities) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnableSMIMECapabilities( 
            /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnroll2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnroll2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnroll2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createFilePKCS10WStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [in] */ LPCWSTR wszPKCS10FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptFilePKCS7WStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPCWSTR wszPKCS7FileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *createPKCS10WStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPCWSTR DNName,
            /* [in] */ LPCWSTR Usage,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs10Blob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *acceptPKCS7Blob )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);
        
        PCCERT_CONTEXT ( STDMETHODCALLTYPE __RPC_FAR *getCertContextFromPKCS7 )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getMyStore )( 
            IEnroll2 __RPC_FAR * This);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getCAStore )( 
            IEnroll2 __RPC_FAR * This);
        
        HCERTSTORE ( STDMETHODCALLTYPE __RPC_FAR *getROOTHStore )( 
            IEnroll2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumProvidersWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD dwFlags,
            /* [out] */ LPWSTR __RPC_FAR *pbstrProvName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *enumContainersWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out] */ LPWSTR __RPC_FAR *pbstr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *freeRequestInfoBlob )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ CRYPT_DATA_BLOB pkcs7OrPkcs10);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MyStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MyStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CAStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CAStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RootStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RootStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreTypeWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RequestStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RequestStoreFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContainerNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwContainer);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ContainerNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwContainer);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szwProvider);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szwProvider);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderType )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwType);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderType )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_KeySpec )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_KeySpec )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProviderFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProviderFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseExistingKeySet )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fUseExistingKeys);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseExistingKeySet )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fUseExistingKeys);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GenKeyFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwFlags);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GenKeyFlags )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwFlags);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DeleteRequestCert )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fDelete);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DeleteRequestCert )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fDelete);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToUserDS )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToUserDS )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableT61DNEncoding )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableT61DNEncoding )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WriteCertToCSP )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fBool);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WriteCertToCSP )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fBool);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SPCFileNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SPCFileNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PVKFileNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PVKFileNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithmWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ LPWSTR __RPC_FAR *szw);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithmWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RenewalCertificate )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ PCCERT_CONTEXT __RPC_FAR *ppCertContext);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RenewalCertificate )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCCERT_CONTEXT pCertContext);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddCertTypeToRequestWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR szw);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddNameValuePairToSignatureWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ LPWSTR Name,
            /* [in] */ LPWSTR Value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddExtensionsToRequest )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCERT_EXTENSIONS pCertExtensions);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddAuthenticatedAttributesToPKCS7Request )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCRYPT_ATTRIBUTES pAttributes);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePKCS7RequestFromRequest )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pRequest,
            /* [in] */ PCCERT_CONTEXT pSigningCertContext,
            /* [out] */ PCRYPT_DATA_BLOB pPkcs7Blob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallPKCS7Blob )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnroll2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSupportedKeySpec )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwKeySpec);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetKeyLen )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fMin,
            /* [in] */ BOOL fExchange,
            /* [out] */ DWORD __RPC_FAR *pdwKeySize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumAlgs )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD algClass,
            /* [out] */ DWORD __RPC_FAR *pdwAlgID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAlgNameWStr )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD algID,
            /* [out] */ LPWSTR __RPC_FAR *ppwsz);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReuseHardwareKeyIfUnableToGenNew )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReuseHardwareKeyIfUnableToGenNew )( 
            IEnroll2 __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgID )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ DWORD hashAlgID);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgID )( 
            IEnroll2 __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *hashAlgID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHStoreMy )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ HCERTSTORE hStore);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHStoreCA )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ HCERTSTORE hStore);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHStoreROOT )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ HCERTSTORE hStore);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHStoreRequest )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ HCERTSTORE hStore);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LimitExchangeKeyToEncipherment )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fLimitExchangeKeyToEncipherment);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LimitExchangeKeyToEncipherment )( 
            IEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableSMIMECapabilities )( 
            IEnroll2 __RPC_FAR * This,
            /* [in] */ BOOL fEnableSMIMECapabilities);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableSMIMECapabilities )( 
            IEnroll2 __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities);
        
        END_INTERFACE
    } IEnroll2Vtbl;

    interface IEnroll2
    {
        CONST_VTBL struct IEnroll2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnroll2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnroll2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnroll2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnroll2_createFilePKCS10WStr(This,DNName,Usage,wszPKCS10FileName)	\
    (This)->lpVtbl -> createFilePKCS10WStr(This,DNName,Usage,wszPKCS10FileName)

#define IEnroll2_acceptFilePKCS7WStr(This,wszPKCS7FileName)	\
    (This)->lpVtbl -> acceptFilePKCS7WStr(This,wszPKCS7FileName)

#define IEnroll2_createPKCS10WStr(This,DNName,Usage,pPkcs10Blob)	\
    (This)->lpVtbl -> createPKCS10WStr(This,DNName,Usage,pPkcs10Blob)

#define IEnroll2_acceptPKCS7Blob(This,pBlobPKCS7)	\
    (This)->lpVtbl -> acceptPKCS7Blob(This,pBlobPKCS7)

#define IEnroll2_getCertContextFromPKCS7(This,pBlobPKCS7)	\
    (This)->lpVtbl -> getCertContextFromPKCS7(This,pBlobPKCS7)

#define IEnroll2_getMyStore(This)	\
    (This)->lpVtbl -> getMyStore(This)

#define IEnroll2_getCAStore(This)	\
    (This)->lpVtbl -> getCAStore(This)

#define IEnroll2_getROOTHStore(This)	\
    (This)->lpVtbl -> getROOTHStore(This)

#define IEnroll2_enumProvidersWStr(This,dwIndex,dwFlags,pbstrProvName)	\
    (This)->lpVtbl -> enumProvidersWStr(This,dwIndex,dwFlags,pbstrProvName)

#define IEnroll2_enumContainersWStr(This,dwIndex,pbstr)	\
    (This)->lpVtbl -> enumContainersWStr(This,dwIndex,pbstr)

#define IEnroll2_freeRequestInfoBlob(This,pkcs7OrPkcs10)	\
    (This)->lpVtbl -> freeRequestInfoBlob(This,pkcs7OrPkcs10)

#define IEnroll2_get_MyStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_MyStoreNameWStr(This,szwName)

#define IEnroll2_put_MyStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_MyStoreNameWStr(This,szwName)

#define IEnroll2_get_MyStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_MyStoreTypeWStr(This,szwType)

#define IEnroll2_put_MyStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_MyStoreTypeWStr(This,szwType)

#define IEnroll2_get_MyStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_MyStoreFlags(This,pdwFlags)

#define IEnroll2_put_MyStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_MyStoreFlags(This,dwFlags)

#define IEnroll2_get_CAStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_CAStoreNameWStr(This,szwName)

#define IEnroll2_put_CAStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_CAStoreNameWStr(This,szwName)

#define IEnroll2_get_CAStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_CAStoreTypeWStr(This,szwType)

#define IEnroll2_put_CAStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_CAStoreTypeWStr(This,szwType)

#define IEnroll2_get_CAStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_CAStoreFlags(This,pdwFlags)

#define IEnroll2_put_CAStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_CAStoreFlags(This,dwFlags)

#define IEnroll2_get_RootStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_RootStoreNameWStr(This,szwName)

#define IEnroll2_put_RootStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_RootStoreNameWStr(This,szwName)

#define IEnroll2_get_RootStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_RootStoreTypeWStr(This,szwType)

#define IEnroll2_put_RootStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_RootStoreTypeWStr(This,szwType)

#define IEnroll2_get_RootStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RootStoreFlags(This,pdwFlags)

#define IEnroll2_put_RootStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RootStoreFlags(This,dwFlags)

#define IEnroll2_get_RequestStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> get_RequestStoreNameWStr(This,szwName)

#define IEnroll2_put_RequestStoreNameWStr(This,szwName)	\
    (This)->lpVtbl -> put_RequestStoreNameWStr(This,szwName)

#define IEnroll2_get_RequestStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> get_RequestStoreTypeWStr(This,szwType)

#define IEnroll2_put_RequestStoreTypeWStr(This,szwType)	\
    (This)->lpVtbl -> put_RequestStoreTypeWStr(This,szwType)

#define IEnroll2_get_RequestStoreFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_RequestStoreFlags(This,pdwFlags)

#define IEnroll2_put_RequestStoreFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_RequestStoreFlags(This,dwFlags)

#define IEnroll2_get_ContainerNameWStr(This,szwContainer)	\
    (This)->lpVtbl -> get_ContainerNameWStr(This,szwContainer)

#define IEnroll2_put_ContainerNameWStr(This,szwContainer)	\
    (This)->lpVtbl -> put_ContainerNameWStr(This,szwContainer)

#define IEnroll2_get_ProviderNameWStr(This,szwProvider)	\
    (This)->lpVtbl -> get_ProviderNameWStr(This,szwProvider)

#define IEnroll2_put_ProviderNameWStr(This,szwProvider)	\
    (This)->lpVtbl -> put_ProviderNameWStr(This,szwProvider)

#define IEnroll2_get_ProviderType(This,pdwType)	\
    (This)->lpVtbl -> get_ProviderType(This,pdwType)

#define IEnroll2_put_ProviderType(This,dwType)	\
    (This)->lpVtbl -> put_ProviderType(This,dwType)

#define IEnroll2_get_KeySpec(This,pdw)	\
    (This)->lpVtbl -> get_KeySpec(This,pdw)

#define IEnroll2_put_KeySpec(This,dw)	\
    (This)->lpVtbl -> put_KeySpec(This,dw)

#define IEnroll2_get_ProviderFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_ProviderFlags(This,pdwFlags)

#define IEnroll2_put_ProviderFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_ProviderFlags(This,dwFlags)

#define IEnroll2_get_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> get_UseExistingKeySet(This,fUseExistingKeys)

#define IEnroll2_put_UseExistingKeySet(This,fUseExistingKeys)	\
    (This)->lpVtbl -> put_UseExistingKeySet(This,fUseExistingKeys)

#define IEnroll2_get_GenKeyFlags(This,pdwFlags)	\
    (This)->lpVtbl -> get_GenKeyFlags(This,pdwFlags)

#define IEnroll2_put_GenKeyFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_GenKeyFlags(This,dwFlags)

#define IEnroll2_get_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> get_DeleteRequestCert(This,fDelete)

#define IEnroll2_put_DeleteRequestCert(This,fDelete)	\
    (This)->lpVtbl -> put_DeleteRequestCert(This,fDelete)

#define IEnroll2_get_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToUserDS(This,fBool)

#define IEnroll2_put_WriteCertToUserDS(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToUserDS(This,fBool)

#define IEnroll2_get_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> get_EnableT61DNEncoding(This,fBool)

#define IEnroll2_put_EnableT61DNEncoding(This,fBool)	\
    (This)->lpVtbl -> put_EnableT61DNEncoding(This,fBool)

#define IEnroll2_get_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> get_WriteCertToCSP(This,fBool)

#define IEnroll2_put_WriteCertToCSP(This,fBool)	\
    (This)->lpVtbl -> put_WriteCertToCSP(This,fBool)

#define IEnroll2_get_SPCFileNameWStr(This,szw)	\
    (This)->lpVtbl -> get_SPCFileNameWStr(This,szw)

#define IEnroll2_put_SPCFileNameWStr(This,szw)	\
    (This)->lpVtbl -> put_SPCFileNameWStr(This,szw)

#define IEnroll2_get_PVKFileNameWStr(This,szw)	\
    (This)->lpVtbl -> get_PVKFileNameWStr(This,szw)

#define IEnroll2_put_PVKFileNameWStr(This,szw)	\
    (This)->lpVtbl -> put_PVKFileNameWStr(This,szw)

#define IEnroll2_get_HashAlgorithmWStr(This,szw)	\
    (This)->lpVtbl -> get_HashAlgorithmWStr(This,szw)

#define IEnroll2_put_HashAlgorithmWStr(This,szw)	\
    (This)->lpVtbl -> put_HashAlgorithmWStr(This,szw)

#define IEnroll2_get_RenewalCertificate(This,ppCertContext)	\
    (This)->lpVtbl -> get_RenewalCertificate(This,ppCertContext)

#define IEnroll2_put_RenewalCertificate(This,pCertContext)	\
    (This)->lpVtbl -> put_RenewalCertificate(This,pCertContext)

#define IEnroll2_AddCertTypeToRequestWStr(This,szw)	\
    (This)->lpVtbl -> AddCertTypeToRequestWStr(This,szw)

#define IEnroll2_AddNameValuePairToSignatureWStr(This,Name,Value)	\
    (This)->lpVtbl -> AddNameValuePairToSignatureWStr(This,Name,Value)

#define IEnroll2_AddExtensionsToRequest(This,pCertExtensions)	\
    (This)->lpVtbl -> AddExtensionsToRequest(This,pCertExtensions)

#define IEnroll2_AddAuthenticatedAttributesToPKCS7Request(This,pAttributes)	\
    (This)->lpVtbl -> AddAuthenticatedAttributesToPKCS7Request(This,pAttributes)

#define IEnroll2_CreatePKCS7RequestFromRequest(This,pRequest,pSigningCertContext,pPkcs7Blob)	\
    (This)->lpVtbl -> CreatePKCS7RequestFromRequest(This,pRequest,pSigningCertContext,pPkcs7Blob)


#define IEnroll2_InstallPKCS7Blob(This,pBlobPKCS7)	\
    (This)->lpVtbl -> InstallPKCS7Blob(This,pBlobPKCS7)

#define IEnroll2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnroll2_GetSupportedKeySpec(This,pdwKeySpec)	\
    (This)->lpVtbl -> GetSupportedKeySpec(This,pdwKeySpec)

#define IEnroll2_GetKeyLen(This,fMin,fExchange,pdwKeySize)	\
    (This)->lpVtbl -> GetKeyLen(This,fMin,fExchange,pdwKeySize)

#define IEnroll2_EnumAlgs(This,dwIndex,algClass,pdwAlgID)	\
    (This)->lpVtbl -> EnumAlgs(This,dwIndex,algClass,pdwAlgID)

#define IEnroll2_GetAlgNameWStr(This,algID,ppwsz)	\
    (This)->lpVtbl -> GetAlgNameWStr(This,algID,ppwsz)

#define IEnroll2_put_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)	\
    (This)->lpVtbl -> put_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)

#define IEnroll2_get_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)	\
    (This)->lpVtbl -> get_ReuseHardwareKeyIfUnableToGenNew(This,fReuseHardwareKeyIfUnableToGenNew)

#define IEnroll2_put_HashAlgID(This,hashAlgID)	\
    (This)->lpVtbl -> put_HashAlgID(This,hashAlgID)

#define IEnroll2_get_HashAlgID(This,hashAlgID)	\
    (This)->lpVtbl -> get_HashAlgID(This,hashAlgID)

#define IEnroll2_SetHStoreMy(This,hStore)	\
    (This)->lpVtbl -> SetHStoreMy(This,hStore)

#define IEnroll2_SetHStoreCA(This,hStore)	\
    (This)->lpVtbl -> SetHStoreCA(This,hStore)

#define IEnroll2_SetHStoreROOT(This,hStore)	\
    (This)->lpVtbl -> SetHStoreROOT(This,hStore)

#define IEnroll2_SetHStoreRequest(This,hStore)	\
    (This)->lpVtbl -> SetHStoreRequest(This,hStore)

#define IEnroll2_put_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)	\
    (This)->lpVtbl -> put_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)

#define IEnroll2_get_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)	\
    (This)->lpVtbl -> get_LimitExchangeKeyToEncipherment(This,fLimitExchangeKeyToEncipherment)

#define IEnroll2_put_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)	\
    (This)->lpVtbl -> put_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)

#define IEnroll2_get_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)	\
    (This)->lpVtbl -> get_EnableSMIMECapabilities(This,fEnableSMIMECapabilities)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnroll2_InstallPKCS7Blob_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ PCRYPT_DATA_BLOB pBlobPKCS7);


void __RPC_STUB IEnroll2_InstallPKCS7Blob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_Reset_Proxy( 
    IEnroll2 __RPC_FAR * This);


void __RPC_STUB IEnroll2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_GetSupportedKeySpec_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwKeySpec);


void __RPC_STUB IEnroll2_GetSupportedKeySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_GetKeyLen_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fMin,
    /* [in] */ BOOL fExchange,
    /* [out] */ DWORD __RPC_FAR *pdwKeySize);


void __RPC_STUB IEnroll2_GetKeyLen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_EnumAlgs_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [in] */ DWORD algClass,
    /* [out] */ DWORD __RPC_FAR *pdwAlgID);


void __RPC_STUB IEnroll2_EnumAlgs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_GetAlgNameWStr_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ DWORD algID,
    /* [out] */ LPWSTR __RPC_FAR *ppwsz);


void __RPC_STUB IEnroll2_GetAlgNameWStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll2_put_ReuseHardwareKeyIfUnableToGenNew_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fReuseHardwareKeyIfUnableToGenNew);


void __RPC_STUB IEnroll2_put_ReuseHardwareKeyIfUnableToGenNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll2_get_ReuseHardwareKeyIfUnableToGenNew_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *fReuseHardwareKeyIfUnableToGenNew);


void __RPC_STUB IEnroll2_get_ReuseHardwareKeyIfUnableToGenNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll2_put_HashAlgID_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ DWORD hashAlgID);


void __RPC_STUB IEnroll2_put_HashAlgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll2_get_HashAlgID_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *hashAlgID);


void __RPC_STUB IEnroll2_get_HashAlgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_SetHStoreMy_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ HCERTSTORE hStore);


void __RPC_STUB IEnroll2_SetHStoreMy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_SetHStoreCA_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ HCERTSTORE hStore);


void __RPC_STUB IEnroll2_SetHStoreCA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_SetHStoreROOT_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ HCERTSTORE hStore);


void __RPC_STUB IEnroll2_SetHStoreROOT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnroll2_SetHStoreRequest_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ HCERTSTORE hStore);


void __RPC_STUB IEnroll2_SetHStoreRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll2_put_LimitExchangeKeyToEncipherment_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fLimitExchangeKeyToEncipherment);


void __RPC_STUB IEnroll2_put_LimitExchangeKeyToEncipherment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll2_get_LimitExchangeKeyToEncipherment_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fLimitExchangeKeyToEncipherment);


void __RPC_STUB IEnroll2_get_LimitExchangeKeyToEncipherment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IEnroll2_put_EnableSMIMECapabilities_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [in] */ BOOL fEnableSMIMECapabilities);


void __RPC_STUB IEnroll2_put_EnableSMIMECapabilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IEnroll2_get_EnableSMIMECapabilities_Proxy( 
    IEnroll2 __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *fEnableSMIMECapabilities);


void __RPC_STUB IEnroll2_get_EnableSMIMECapabilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnroll2_INTERFACE_DEFINED__ */



#ifndef __XENROLLLib_LIBRARY_DEFINED__
#define __XENROLLLib_LIBRARY_DEFINED__

/* library XENROLLLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_XENROLLLib;

EXTERN_C const CLSID CLSID_CEnroll;

#ifdef __cplusplus

class DECLSPEC_UUID("43F8F289-7A20-11D0-8F06-00C04FC295E1")
CEnroll;
#endif
#endif /* __XENROLLLib_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_xenroll_0249 */
/* [local] */ 

extern "C" IEnroll * WINAPI PIEnrollGetNoCOM(void);
extern "C" IEnroll2 * WINAPI PIEnroll2GetNoCOM(void);
#define CRYPT_ENUM_ALL_PROVIDERS  0x1


extern RPC_IF_HANDLE __MIDL_itf_xenroll_0249_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xenroll_0249_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


