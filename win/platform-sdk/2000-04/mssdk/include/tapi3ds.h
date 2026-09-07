
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Tue Oct 05 15:14:28 1999
 */
/* Compiler settings for tapi3ds.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
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

#ifndef __tapi3ds_h__
#define __tapi3ds_h__

/* Forward Declarations */ 

#ifndef __ITAMMediaFormat_FWD_DEFINED__
#define __ITAMMediaFormat_FWD_DEFINED__
typedef interface ITAMMediaFormat ITAMMediaFormat;
#endif 	/* __ITAMMediaFormat_FWD_DEFINED__ */


#ifndef __ITAllocatorProperties_FWD_DEFINED__
#define __ITAllocatorProperties_FWD_DEFINED__
typedef interface ITAllocatorProperties ITAllocatorProperties;
#endif 	/* __ITAllocatorProperties_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "strmif.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_tapi3ds_0000 */
/* [local] */ 

/* Copyright (c) 1998-1999  Microsoft Corporation  */


extern RPC_IF_HANDLE __MIDL_itf_tapi3ds_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_tapi3ds_0000_v0_0_s_ifspec;

#ifndef __ITAMMediaFormat_INTERFACE_DEFINED__
#define __ITAMMediaFormat_INTERFACE_DEFINED__

/* interface ITAMMediaFormat */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_ITAMMediaFormat;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0364EB00-4A77-11D1-A671-006097C9A2E8")
    ITAMMediaFormat : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaFormat( 
            /* [retval][out] */ AM_MEDIA_TYPE __RPC_FAR *__RPC_FAR *ppmt) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MediaFormat( 
            /* [in] */ const AM_MEDIA_TYPE __RPC_FAR *pmt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITAMMediaFormatVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITAMMediaFormat __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITAMMediaFormat __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITAMMediaFormat __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaFormat )( 
            ITAMMediaFormat __RPC_FAR * This,
            /* [retval][out] */ AM_MEDIA_TYPE __RPC_FAR *__RPC_FAR *ppmt);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MediaFormat )( 
            ITAMMediaFormat __RPC_FAR * This,
            /* [in] */ const AM_MEDIA_TYPE __RPC_FAR *pmt);
        
        END_INTERFACE
    } ITAMMediaFormatVtbl;

    interface ITAMMediaFormat
    {
        CONST_VTBL struct ITAMMediaFormatVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITAMMediaFormat_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITAMMediaFormat_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITAMMediaFormat_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITAMMediaFormat_get_MediaFormat(This,ppmt)	\
    (This)->lpVtbl -> get_MediaFormat(This,ppmt)

#define ITAMMediaFormat_put_MediaFormat(This,pmt)	\
    (This)->lpVtbl -> put_MediaFormat(This,pmt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITAMMediaFormat_get_MediaFormat_Proxy( 
    ITAMMediaFormat __RPC_FAR * This,
    /* [retval][out] */ AM_MEDIA_TYPE __RPC_FAR *__RPC_FAR *ppmt);


void __RPC_STUB ITAMMediaFormat_get_MediaFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITAMMediaFormat_put_MediaFormat_Proxy( 
    ITAMMediaFormat __RPC_FAR * This,
    /* [in] */ const AM_MEDIA_TYPE __RPC_FAR *pmt);


void __RPC_STUB ITAMMediaFormat_put_MediaFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITAMMediaFormat_INTERFACE_DEFINED__ */


#ifndef __ITAllocatorProperties_INTERFACE_DEFINED__
#define __ITAllocatorProperties_INTERFACE_DEFINED__

/* interface ITAllocatorProperties */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_ITAllocatorProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1BC3C90-BCFE-11D1-9745-00C04FD91AC0")
    ITAllocatorProperties : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAllocatorProperties( 
            /* [in] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllocatorProperties( 
            /* [out] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAllocateBuffers( 
            /* [in] */ BOOL bAllocBuffers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllocateBuffers( 
            /* [out] */ BOOL __RPC_FAR *pbAllocBuffers) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBufferSize( 
            /* [in] */ DWORD BufferSize) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBufferSize( 
            /* [out] */ DWORD __RPC_FAR *pBufferSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITAllocatorPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITAllocatorProperties __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITAllocatorProperties __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAllocatorProperties )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [in] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllocatorProperties )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [out] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAllocateBuffers )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [in] */ BOOL bAllocBuffers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllocateBuffers )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *pbAllocBuffers);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetBufferSize )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [in] */ DWORD BufferSize);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBufferSize )( 
            ITAllocatorProperties __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pBufferSize);
        
        END_INTERFACE
    } ITAllocatorPropertiesVtbl;

    interface ITAllocatorProperties
    {
        CONST_VTBL struct ITAllocatorPropertiesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITAllocatorProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITAllocatorProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITAllocatorProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITAllocatorProperties_SetAllocatorProperties(This,pAllocProperties)	\
    (This)->lpVtbl -> SetAllocatorProperties(This,pAllocProperties)

#define ITAllocatorProperties_GetAllocatorProperties(This,pAllocProperties)	\
    (This)->lpVtbl -> GetAllocatorProperties(This,pAllocProperties)

#define ITAllocatorProperties_SetAllocateBuffers(This,bAllocBuffers)	\
    (This)->lpVtbl -> SetAllocateBuffers(This,bAllocBuffers)

#define ITAllocatorProperties_GetAllocateBuffers(This,pbAllocBuffers)	\
    (This)->lpVtbl -> GetAllocateBuffers(This,pbAllocBuffers)

#define ITAllocatorProperties_SetBufferSize(This,BufferSize)	\
    (This)->lpVtbl -> SetBufferSize(This,BufferSize)

#define ITAllocatorProperties_GetBufferSize(This,pBufferSize)	\
    (This)->lpVtbl -> GetBufferSize(This,pBufferSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_SetAllocatorProperties_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [in] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties);


void __RPC_STUB ITAllocatorProperties_SetAllocatorProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_GetAllocatorProperties_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [out] */ ALLOCATOR_PROPERTIES __RPC_FAR *pAllocProperties);


void __RPC_STUB ITAllocatorProperties_GetAllocatorProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_SetAllocateBuffers_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [in] */ BOOL bAllocBuffers);


void __RPC_STUB ITAllocatorProperties_SetAllocateBuffers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_GetAllocateBuffers_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *pbAllocBuffers);


void __RPC_STUB ITAllocatorProperties_GetAllocateBuffers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_SetBufferSize_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [in] */ DWORD BufferSize);


void __RPC_STUB ITAllocatorProperties_SetBufferSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAllocatorProperties_GetBufferSize_Proxy( 
    ITAllocatorProperties __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pBufferSize);


void __RPC_STUB ITAllocatorProperties_GetBufferSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITAllocatorProperties_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


