/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 3.00.15 */
/* at Fri May 17 15:00:22 1996
 */
/* Compiler settings for settime.idl:
    Os, W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )

#include "rpcproxy.h"
#include "settime.h"

#define TYPE_FORMAT_STRING_SIZE   15                                
#define PROC_FORMAT_STRING_SIZE   7                                 

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDLTypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDLProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMySettime, ver. 0.0,
   GUID={0x043DACA1,0xA8ED,0x11CF,{0x91,0xE3,0x00,0xA0,0xC9,0x03,0x97,0x6F}} */


extern const MIDL_STUB_DESC Object_StubDesc;


#pragma code_seg(".orpc")

HRESULT STDMETHODCALLTYPE IMySettime_Set_Proxy( 
    IMySettime __RPC_FAR * This,
    /* [in] */ SYSTEMTIME t)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        RpcTryFinally
            {
            NdrProxyInitialize(
                      ( void __RPC_FAR *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      3);
            
            
            
            
            _StubMsg.BufferLength = 0U;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char __RPC_FAR *)&t,
                                       (PFORMAT_STRING) &__MIDLTypeFormatString.Format[0] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char __RPC_FAR *)&t,
                                     (PFORMAT_STRING) &__MIDLTypeFormatString.Format[0] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDLProcFormatString.Format[0] );
            
            _RetVal = *(( HRESULT __RPC_FAR * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB IMySettime_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void __RPC_FAR *_p_t;
    SYSTEMTIME t;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    _p_t = &t;
    MIDL_memset(
               _p_t,
               0,
               sizeof( SYSTEMTIME  ));
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDLProcFormatString.Format[0] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char __RPC_FAR * __RPC_FAR *)&_p_t,
                                   (PFORMAT_STRING) &__MIDLTypeFormatString.Format[0],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((IMySettime *) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Set((IMySettime *) ((CStdStubBuffer *)This)->pvServerObject,t);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 4U;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        *(( HRESULT __RPC_FAR * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)((long)_StubMsg.Buffer - (long)_pRpcMessage->Buffer);
    
}


static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDLTypeFormatString.Format,
    0, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x300000f, /* MIDL Version 3.0.15 */
    0,
    0,
    0,  /* Reserved1 */
    0,  /* Reserved2 */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

const CINTERFACE_PROXY_VTABLE(4) _IMySettimeProxyVtbl = 
{
    &IID_IMySettime,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    IMySettime_Set_Proxy
};


static const PRPC_STUB_FUNCTION IMySettime_table[] =
{
    IMySettime_Set_Stub
};

const CInterfaceStubVtbl _IMySettimeStubVtbl =
{
    &IID_IMySettime,
    0,
    4,
    &IMySettime_table[-3],
    CStdStubBuffer_METHODS
};

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDLProcFormatString =
    {
        0,
        {
                        
                        0x4d,                /* FC_IN_PARAM */
#ifndef _ALPHA_
                        0x4,                /* x86, MIPS & PPC Stack size = 4 */
#else
                        0x4,                /* Alpha Stack size = 4 */
#endif
/*  2 */        NdrFcShort( 0x0 ),        /* Type Offset=0 */
/*  4 */        0x53,                /* FC_RETURN_PARAM_BASETYPE */
                        0x8,                /* FC_LONG */

                        0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDLTypeFormatString =
    {
        0,
        {
                        
                        0x15,                /* FC_STRUCT */
                        0x1,                /* 1 */
/*  2 */        NdrFcShort( 0x10 ),        /* 16 */
/*  4 */        0x6,                /* FC_SHORT */
                        0x6,                /* FC_SHORT */
/*  6 */        0x6,                /* FC_SHORT */
                        0x6,                /* FC_SHORT */
/*  8 */        0x6,                /* FC_SHORT */
                        0x6,                /* FC_SHORT */
/* 10 */        0x6,                /* FC_SHORT */
                        0x6,                /* FC_SHORT */
/* 12 */        0x5c,                /* FC_PAD */
                        0x5b,                /* FC_END */

                        0x0
        }
    };

const CInterfaceProxyVtbl * _settime_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IMySettimeProxyVtbl,
    0
};

const CInterfaceStubVtbl * _settime_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IMySettimeStubVtbl,
    0
};

PCInterfaceName const _settime_InterfaceNamesList[] = 
{
    "IMySettime",
    0
};


#define _settime_CHECK_IID(n)        IID_GENERIC_CHECK_IID( _settime, pIID, n)

int __stdcall _settime_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_settime_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo settime_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _settime_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _settime_StubVtblList,
    (const PCInterfaceName * ) & _settime_InterfaceNamesList,
    0, // no delegation
    & _settime_IID_Lookup, 
    1,
    1
};
