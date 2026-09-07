
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Tue Oct 05 15:14:47 1999
 */
/* Compiler settings for msp.idl:
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

#ifndef __msp_h__
#define __msp_h__

/* Forward Declarations */ 

#ifndef __ITMSPAddress_FWD_DEFINED__
#define __ITMSPAddress_FWD_DEFINED__
typedef interface ITMSPAddress ITMSPAddress;
#endif 	/* __ITMSPAddress_FWD_DEFINED__ */


/* header files for imported files */
#include "tapi3if.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_msp_0000 */
/* [local] */ 

/* Copyright (c) 1998-1999  Microsoft Corporation  */
typedef long __RPC_FAR *MSP_HANDLE;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_msp_0000_0001
    {	ADDRESS_TERMINAL_AVAILABLE	= 0,
	ADDRESS_TERMINAL_UNAVAILABLE	= ADDRESS_TERMINAL_AVAILABLE + 1
    }	MSP_ADDRESS_EVENT;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_msp_0000_0002
    {	CALL_NEW_STREAM	= 0,
	CALL_STREAM_FAIL	= CALL_NEW_STREAM + 1,
	CALL_TERMINAL_FAIL	= CALL_STREAM_FAIL + 1,
	CALL_STREAM_NOT_USED	= CALL_TERMINAL_FAIL + 1,
	CALL_STREAM_ACTIVE	= CALL_STREAM_NOT_USED + 1,
	CALL_STREAM_INACTIVE	= CALL_STREAM_ACTIVE + 1
    }	MSP_CALL_EVENT;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_msp_0000_0003
    {	CALL_CAUSE_UNKNOWN	= 0,
	CALL_CAUSE_BAD_DEVICE	= CALL_CAUSE_UNKNOWN + 1,
	CALL_CAUSE_CONNECT_FAIL	= CALL_CAUSE_BAD_DEVICE + 1,
	CALL_CAUSE_LOCAL_REQUEST	= CALL_CAUSE_CONNECT_FAIL + 1,
	CALL_CAUSE_REMOTE_REQUEST	= CALL_CAUSE_LOCAL_REQUEST + 1,
	CALL_CAUSE_MEDIA_TIMEOUT	= CALL_CAUSE_REMOTE_REQUEST + 1,
	CALL_CAUSE_MEDIA_RECOVERED	= CALL_CAUSE_MEDIA_TIMEOUT + 1
    }	MSP_CALL_EVENT_CAUSE;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_msp_0000_0004
    {	ME_ADDRESS_EVENT	= 0,
	ME_CALL_EVENT	= ME_ADDRESS_EVENT + 1,
	ME_TSP_DATA	= ME_CALL_EVENT + 1,
	ME_PRIVATE_EVENT	= ME_TSP_DATA + 1
    }	MSP_EVENT;

typedef /* [public] */ struct __MIDL___MIDL_itf_msp_0000_0005
    {
    DWORD dwSize;
    MSP_EVENT Event;
    MSP_HANDLE hCall;
    /* [switch_is][switch_type] */ union 
        {
        /* [case()] */ struct 
            {
            MSP_ADDRESS_EVENT Type;
            ITTerminal __RPC_FAR *pTerminal;
            }	MSP_ADDRESS_EVENT_INFO;
        /* [case()] */ struct 
            {
            MSP_CALL_EVENT Type;
            MSP_CALL_EVENT_CAUSE Cause;
            ITStream __RPC_FAR *pStream;
            ITTerminal __RPC_FAR *pTerminal;
            HRESULT hrError;
            }	MSP_CALL_EVENT_INFO;
        /* [case()] */ struct 
            {
            DWORD dwBufferSize;
            BYTE pBuffer[ 1 ];
            }	MSP_TSP_DATA;
        /* [case()] */ struct 
            {
            IDispatch __RPC_FAR *pEvent;
            long lEventCode;
            }	MSP_PRIVATE_EVENT_INFO;
        }	;
    }	MSP_EVENT_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msp_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msp_0000_v0_0_s_ifspec;

#ifndef __ITMSPAddress_INTERFACE_DEFINED__
#define __ITMSPAddress_INTERFACE_DEFINED__

/* interface ITMSPAddress */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITMSPAddress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE3BD600-3868-11D2-A045-00C04FB6809F")
    ITMSPAddress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ MSP_HANDLE hEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Shutdown( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateMSPCall( 
            /* [in] */ MSP_HANDLE hCall,
            /* [in] */ DWORD dwReserved,
            /* [in] */ DWORD dwMediaType,
            /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStreamControl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShutdownMSPCall( 
            /* [in] */ IUnknown __RPC_FAR *pStreamControl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReceiveTSPData( 
            /* [in] */ IUnknown __RPC_FAR *pMSPCall,
            /* [size_is][in] */ BYTE __RPC_FAR *pBuffer,
            /* [in] */ DWORD dwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEvent( 
            /* [out][in] */ DWORD __RPC_FAR *pdwSize,
            /* [size_is][out][in] */ byte __RPC_FAR *pEventBuffer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITMSPAddressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITMSPAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITMSPAddress __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITMSPAddress __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            ITMSPAddress __RPC_FAR * This,
            /* [in] */ MSP_HANDLE hEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Shutdown )( 
            ITMSPAddress __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateMSPCall )( 
            ITMSPAddress __RPC_FAR * This,
            /* [in] */ MSP_HANDLE hCall,
            /* [in] */ DWORD dwReserved,
            /* [in] */ DWORD dwMediaType,
            /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStreamControl);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownMSPCall )( 
            ITMSPAddress __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pStreamControl);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveTSPData )( 
            ITMSPAddress __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pMSPCall,
            /* [size_is][in] */ BYTE __RPC_FAR *pBuffer,
            /* [in] */ DWORD dwSize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvent )( 
            ITMSPAddress __RPC_FAR * This,
            /* [out][in] */ DWORD __RPC_FAR *pdwSize,
            /* [size_is][out][in] */ byte __RPC_FAR *pEventBuffer);
        
        END_INTERFACE
    } ITMSPAddressVtbl;

    interface ITMSPAddress
    {
        CONST_VTBL struct ITMSPAddressVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITMSPAddress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITMSPAddress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITMSPAddress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITMSPAddress_Initialize(This,hEvent)	\
    (This)->lpVtbl -> Initialize(This,hEvent)

#define ITMSPAddress_Shutdown(This)	\
    (This)->lpVtbl -> Shutdown(This)

#define ITMSPAddress_CreateMSPCall(This,hCall,dwReserved,dwMediaType,pOuterUnknown,ppStreamControl)	\
    (This)->lpVtbl -> CreateMSPCall(This,hCall,dwReserved,dwMediaType,pOuterUnknown,ppStreamControl)

#define ITMSPAddress_ShutdownMSPCall(This,pStreamControl)	\
    (This)->lpVtbl -> ShutdownMSPCall(This,pStreamControl)

#define ITMSPAddress_ReceiveTSPData(This,pMSPCall,pBuffer,dwSize)	\
    (This)->lpVtbl -> ReceiveTSPData(This,pMSPCall,pBuffer,dwSize)

#define ITMSPAddress_GetEvent(This,pdwSize,pEventBuffer)	\
    (This)->lpVtbl -> GetEvent(This,pdwSize,pEventBuffer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITMSPAddress_Initialize_Proxy( 
    ITMSPAddress __RPC_FAR * This,
    /* [in] */ MSP_HANDLE hEvent);


void __RPC_STUB ITMSPAddress_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITMSPAddress_Shutdown_Proxy( 
    ITMSPAddress __RPC_FAR * This);


void __RPC_STUB ITMSPAddress_Shutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITMSPAddress_CreateMSPCall_Proxy( 
    ITMSPAddress __RPC_FAR * This,
    /* [in] */ MSP_HANDLE hCall,
    /* [in] */ DWORD dwReserved,
    /* [in] */ DWORD dwMediaType,
    /* [in] */ IUnknown __RPC_FAR *pOuterUnknown,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppStreamControl);


void __RPC_STUB ITMSPAddress_CreateMSPCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITMSPAddress_ShutdownMSPCall_Proxy( 
    ITMSPAddress __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pStreamControl);


void __RPC_STUB ITMSPAddress_ShutdownMSPCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITMSPAddress_ReceiveTSPData_Proxy( 
    ITMSPAddress __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pMSPCall,
    /* [size_is][in] */ BYTE __RPC_FAR *pBuffer,
    /* [in] */ DWORD dwSize);


void __RPC_STUB ITMSPAddress_ReceiveTSPData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITMSPAddress_GetEvent_Proxy( 
    ITMSPAddress __RPC_FAR * This,
    /* [out][in] */ DWORD __RPC_FAR *pdwSize,
    /* [size_is][out][in] */ byte __RPC_FAR *pEventBuffer);


void __RPC_STUB ITMSPAddress_GetEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITMSPAddress_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


