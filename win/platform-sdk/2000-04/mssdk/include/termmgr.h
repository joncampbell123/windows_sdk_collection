
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Tue Oct 05 15:14:55 1999
 */
/* Compiler settings for termmgr.idl:
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

#ifndef __termmgr_h__
#define __termmgr_h__

/* Forward Declarations */ 

#ifndef __ITTerminalControl_FWD_DEFINED__
#define __ITTerminalControl_FWD_DEFINED__
typedef interface ITTerminalControl ITTerminalControl;
#endif 	/* __ITTerminalControl_FWD_DEFINED__ */


#ifndef __ITTerminalManager_FWD_DEFINED__
#define __ITTerminalManager_FWD_DEFINED__
typedef interface ITTerminalManager ITTerminalManager;
#endif 	/* __ITTerminalManager_FWD_DEFINED__ */


#ifndef __TerminalManager_FWD_DEFINED__
#define __TerminalManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class TerminalManager TerminalManager;
#else
typedef struct TerminalManager TerminalManager;
#endif /* __cplusplus */

#endif 	/* __TerminalManager_FWD_DEFINED__ */


/* header files for imported files */
#include "tapi3if.h"
#include "tapi3ds.h"
#include "msp.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_termmgr_0000 */
/* [local] */ 

/* Copyright (c) 1998-1999  Microsoft Corporation  */


extern RPC_IF_HANDLE __MIDL_itf_termmgr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_termmgr_0000_v0_0_s_ifspec;

#ifndef __ITTerminalControl_INTERFACE_DEFINED__
#define __ITTerminalControl_INTERFACE_DEFINED__

/* interface ITTerminalControl */
/* [object][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ITTerminalControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AED6483B-3304-11d2-86F1-006008B0E5D2")
    ITTerminalControl : public IUnknown
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AddressHandle( 
            /* [out] */ MSP_HANDLE __RPC_FAR *phtAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConnectTerminal( 
            /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ DWORD __RPC_FAR *pdwNumPins,
            /* [out] */ IPin __RPC_FAR *__RPC_FAR *ppPins) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CompleteConnectTerminal( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisconnectTerminal( 
            /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunRenderFilter( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopRenderFilter( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITTerminalControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITTerminalControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITTerminalControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITTerminalControl __RPC_FAR * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AddressHandle )( 
            ITTerminalControl __RPC_FAR * This,
            /* [out] */ MSP_HANDLE __RPC_FAR *phtAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectTerminal )( 
            ITTerminalControl __RPC_FAR * This,
            /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
            /* [in] */ DWORD dwReserved,
            /* [out][in] */ DWORD __RPC_FAR *pdwNumPins,
            /* [out] */ IPin __RPC_FAR *__RPC_FAR *ppPins);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CompleteConnectTerminal )( 
            ITTerminalControl __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DisconnectTerminal )( 
            ITTerminalControl __RPC_FAR * This,
            /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
            /* [in] */ DWORD dwReserved);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RunRenderFilter )( 
            ITTerminalControl __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopRenderFilter )( 
            ITTerminalControl __RPC_FAR * This);
        
        END_INTERFACE
    } ITTerminalControlVtbl;

    interface ITTerminalControl
    {
        CONST_VTBL struct ITTerminalControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITTerminalControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITTerminalControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITTerminalControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITTerminalControl_get_AddressHandle(This,phtAddress)	\
    (This)->lpVtbl -> get_AddressHandle(This,phtAddress)

#define ITTerminalControl_ConnectTerminal(This,pGraph,dwReserved,pdwNumPins,ppPins)	\
    (This)->lpVtbl -> ConnectTerminal(This,pGraph,dwReserved,pdwNumPins,ppPins)

#define ITTerminalControl_CompleteConnectTerminal(This)	\
    (This)->lpVtbl -> CompleteConnectTerminal(This)

#define ITTerminalControl_DisconnectTerminal(This,pGraph,dwReserved)	\
    (This)->lpVtbl -> DisconnectTerminal(This,pGraph,dwReserved)

#define ITTerminalControl_RunRenderFilter(This)	\
    (This)->lpVtbl -> RunRenderFilter(This)

#define ITTerminalControl_StopRenderFilter(This)	\
    (This)->lpVtbl -> StopRenderFilter(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_get_AddressHandle_Proxy( 
    ITTerminalControl __RPC_FAR * This,
    /* [out] */ MSP_HANDLE __RPC_FAR *phtAddress);


void __RPC_STUB ITTerminalControl_get_AddressHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_ConnectTerminal_Proxy( 
    ITTerminalControl __RPC_FAR * This,
    /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
    /* [in] */ DWORD dwReserved,
    /* [out][in] */ DWORD __RPC_FAR *pdwNumPins,
    /* [out] */ IPin __RPC_FAR *__RPC_FAR *ppPins);


void __RPC_STUB ITTerminalControl_ConnectTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_CompleteConnectTerminal_Proxy( 
    ITTerminalControl __RPC_FAR * This);


void __RPC_STUB ITTerminalControl_CompleteConnectTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_DisconnectTerminal_Proxy( 
    ITTerminalControl __RPC_FAR * This,
    /* [in] */ IGraphBuilder __RPC_FAR *pGraph,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ITTerminalControl_DisconnectTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_RunRenderFilter_Proxy( 
    ITTerminalControl __RPC_FAR * This);


void __RPC_STUB ITTerminalControl_RunRenderFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalControl_StopRenderFilter_Proxy( 
    ITTerminalControl __RPC_FAR * This);


void __RPC_STUB ITTerminalControl_StopRenderFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITTerminalControl_INTERFACE_DEFINED__ */


#ifndef __ITTerminalManager_INTERFACE_DEFINED__
#define __ITTerminalManager_INTERFACE_DEFINED__

/* interface ITTerminalManager */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ITTerminalManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7170F2DE-9BE3-11D0-A009-00AA00B605A4")
    ITTerminalManager : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDynamicTerminalClasses( 
            /* [in] */ DWORD dwMediaTypes,
            /* [out][in] */ DWORD __RPC_FAR *pdwNumClasses,
            /* [out] */ IID __RPC_FAR *pTerminalClasses) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateDynamicTerminal( 
            /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
            /* [in] */ IID iidTerminalClass,
            /* [in] */ DWORD dwMediaType,
            /* [in] */ TERMINAL_DIRECTION Direction,
            /* [in] */ MSP_HANDLE htAddress,
            /* [out] */ ITTerminal __RPC_FAR *__RPC_FAR *ppTerminal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITTerminalManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITTerminalManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITTerminalManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITTerminalManager __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDynamicTerminalClasses )( 
            ITTerminalManager __RPC_FAR * This,
            /* [in] */ DWORD dwMediaTypes,
            /* [out][in] */ DWORD __RPC_FAR *pdwNumClasses,
            /* [out] */ IID __RPC_FAR *pTerminalClasses);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateDynamicTerminal )( 
            ITTerminalManager __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
            /* [in] */ IID iidTerminalClass,
            /* [in] */ DWORD dwMediaType,
            /* [in] */ TERMINAL_DIRECTION Direction,
            /* [in] */ MSP_HANDLE htAddress,
            /* [out] */ ITTerminal __RPC_FAR *__RPC_FAR *ppTerminal);
        
        END_INTERFACE
    } ITTerminalManagerVtbl;

    interface ITTerminalManager
    {
        CONST_VTBL struct ITTerminalManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITTerminalManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITTerminalManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITTerminalManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITTerminalManager_GetDynamicTerminalClasses(This,dwMediaTypes,pdwNumClasses,pTerminalClasses)	\
    (This)->lpVtbl -> GetDynamicTerminalClasses(This,dwMediaTypes,pdwNumClasses,pTerminalClasses)

#define ITTerminalManager_CreateDynamicTerminal(This,pOuterUnknown,iidTerminalClass,dwMediaType,Direction,htAddress,ppTerminal)	\
    (This)->lpVtbl -> CreateDynamicTerminal(This,pOuterUnknown,iidTerminalClass,dwMediaType,Direction,htAddress,ppTerminal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalManager_GetDynamicTerminalClasses_Proxy( 
    ITTerminalManager __RPC_FAR * This,
    /* [in] */ DWORD dwMediaTypes,
    /* [out][in] */ DWORD __RPC_FAR *pdwNumClasses,
    /* [out] */ IID __RPC_FAR *pTerminalClasses);


void __RPC_STUB ITTerminalManager_GetDynamicTerminalClasses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTerminalManager_CreateDynamicTerminal_Proxy( 
    ITTerminalManager __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
    /* [in] */ IID iidTerminalClass,
    /* [in] */ DWORD dwMediaType,
    /* [in] */ TERMINAL_DIRECTION Direction,
    /* [in] */ MSP_HANDLE htAddress,
    /* [out] */ ITTerminal __RPC_FAR *__RPC_FAR *ppTerminal);


void __RPC_STUB ITTerminalManager_CreateDynamicTerminal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITTerminalManager_INTERFACE_DEFINED__ */



#ifndef __TERMMGRLib_LIBRARY_DEFINED__
#define __TERMMGRLib_LIBRARY_DEFINED__

/* library TERMMGRLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_TERMMGRLib;

EXTERN_C const CLSID CLSID_TerminalManager;

#ifdef __cplusplus

class DECLSPEC_UUID("7170F2E0-9BE3-11D0-A009-00AA00B605A4")
TerminalManager;
#endif
#endif /* __TERMMGRLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


