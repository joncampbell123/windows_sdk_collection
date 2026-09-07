//
//	This file was automatically generated from the IDL files 
//	included with the WBEM SDK in the \include directory.  If you
//  experience problems compiling this file you can re-generate it
//  by running NMAKE (or another MAKE utility) from within the 
//	\include directory.
//
// Copyright 1999 Microsoft Corporation
//
//
//=================================================================

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Nov 12 12:48:10 1999
 */
/* Compiler settings for wbemads.idl:
    Os (OptLev=s), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __wbemads_h__
#define __wbemads_h__

/* Forward Declarations */ 

#ifndef __IWMIExtension_FWD_DEFINED__
#define __IWMIExtension_FWD_DEFINED__
typedef interface IWMIExtension IWMIExtension;
#endif 	/* __IWMIExtension_FWD_DEFINED__ */


#ifndef __WMIExtension_FWD_DEFINED__
#define __WMIExtension_FWD_DEFINED__

#ifdef __cplusplus
typedef class WMIExtension WMIExtension;
#else
typedef struct WMIExtension WMIExtension;
#endif /* __cplusplus */

#endif 	/* __WMIExtension_FWD_DEFINED__ */


#ifndef __IWMIExtension_FWD_DEFINED__
#define __IWMIExtension_FWD_DEFINED__
typedef interface IWMIExtension IWMIExtension;
#endif 	/* __IWMIExtension_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "wbemdisp.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __WMIEXTENSIONLib_LIBRARY_DEFINED__
#define __WMIEXTENSIONLib_LIBRARY_DEFINED__

/* library WMIEXTENSIONLib */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_WMIEXTENSIONLib;

#ifndef __IWMIExtension_INTERFACE_DEFINED__
#define __IWMIExtension_INTERFACE_DEFINED__

/* interface IWMIExtension */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWMIExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("adc1f06e-5c7e-11d2-8b74-00104b2afb41")
    IWMIExtension : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WMIObjectPath( 
            /* [retval][out] */ BSTR __RPC_FAR *strWMIObjectPath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWMIObject( 
            /* [retval][out] */ ISWbemObject __RPC_FAR *__RPC_FAR *objWMIObject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetWMIServices( 
            /* [retval][out] */ ISWbemServices __RPC_FAR *__RPC_FAR *objWMIServices) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWMIExtensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWMIExtension __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWMIExtension __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWMIExtension __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWMIExtension __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWMIExtension __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWMIExtension __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWMIExtension __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WMIObjectPath )( 
            IWMIExtension __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strWMIObjectPath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWMIObject )( 
            IWMIExtension __RPC_FAR * This,
            /* [retval][out] */ ISWbemObject __RPC_FAR *__RPC_FAR *objWMIObject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWMIServices )( 
            IWMIExtension __RPC_FAR * This,
            /* [retval][out] */ ISWbemServices __RPC_FAR *__RPC_FAR *objWMIServices);
        
        END_INTERFACE
    } IWMIExtensionVtbl;

    interface IWMIExtension
    {
        CONST_VTBL struct IWMIExtensionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWMIExtension_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWMIExtension_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWMIExtension_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWMIExtension_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWMIExtension_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWMIExtension_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWMIExtension_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWMIExtension_get_WMIObjectPath(This,strWMIObjectPath)	\
    (This)->lpVtbl -> get_WMIObjectPath(This,strWMIObjectPath)

#define IWMIExtension_GetWMIObject(This,objWMIObject)	\
    (This)->lpVtbl -> GetWMIObject(This,objWMIObject)

#define IWMIExtension_GetWMIServices(This,objWMIServices)	\
    (This)->lpVtbl -> GetWMIServices(This,objWMIServices)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWMIExtension_get_WMIObjectPath_Proxy( 
    IWMIExtension __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strWMIObjectPath);


void __RPC_STUB IWMIExtension_get_WMIObjectPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWMIExtension_GetWMIObject_Proxy( 
    IWMIExtension __RPC_FAR * This,
    /* [retval][out] */ ISWbemObject __RPC_FAR *__RPC_FAR *objWMIObject);


void __RPC_STUB IWMIExtension_GetWMIObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWMIExtension_GetWMIServices_Proxy( 
    IWMIExtension __RPC_FAR * This,
    /* [retval][out] */ ISWbemServices __RPC_FAR *__RPC_FAR *objWMIServices);


void __RPC_STUB IWMIExtension_GetWMIServices_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWMIExtension_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WMIExtension;

#ifdef __cplusplus

class DECLSPEC_UUID("f0975afe-5c7f-11d2-8b74-00104b2afb41")
WMIExtension;
#endif
#endif /* __WMIEXTENSIONLib_LIBRARY_DEFINED__ */

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


