
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Sat Sep 25 00:00:00 1999
 */
/* Compiler settings for certview.idl:
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

#ifndef __certview_h__
#define __certview_h__

/* Forward Declarations */ 

#ifndef __IEnumCERTVIEWCOLUMN_FWD_DEFINED__
#define __IEnumCERTVIEWCOLUMN_FWD_DEFINED__
typedef interface IEnumCERTVIEWCOLUMN IEnumCERTVIEWCOLUMN;
#endif 	/* __IEnumCERTVIEWCOLUMN_FWD_DEFINED__ */


#ifndef __IEnumCERTVIEWATTRIBUTE_FWD_DEFINED__
#define __IEnumCERTVIEWATTRIBUTE_FWD_DEFINED__
typedef interface IEnumCERTVIEWATTRIBUTE IEnumCERTVIEWATTRIBUTE;
#endif 	/* __IEnumCERTVIEWATTRIBUTE_FWD_DEFINED__ */


#ifndef __IEnumCERTVIEWEXTENSION_FWD_DEFINED__
#define __IEnumCERTVIEWEXTENSION_FWD_DEFINED__
typedef interface IEnumCERTVIEWEXTENSION IEnumCERTVIEWEXTENSION;
#endif 	/* __IEnumCERTVIEWEXTENSION_FWD_DEFINED__ */


#ifndef __IEnumCERTVIEWROW_FWD_DEFINED__
#define __IEnumCERTVIEWROW_FWD_DEFINED__
typedef interface IEnumCERTVIEWROW IEnumCERTVIEWROW;
#endif 	/* __IEnumCERTVIEWROW_FWD_DEFINED__ */


#ifndef __ICertView_FWD_DEFINED__
#define __ICertView_FWD_DEFINED__
typedef interface ICertView ICertView;
#endif 	/* __ICertView_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_certview_0000 */
/* [local] */ 

#define	CV_OUT_BASE64HEADER	( 0 )

#define	CV_OUT_BASE64	( 0x1 )

#define	CV_OUT_BINARY	( 0x2 )

#define	CV_OUT_BASE64REQUESTHEADER	( 0x3 )

#define	CV_OUT_HEX	( 0x4 )

#define	CV_OUT_HEXASCII	( 0x5 )

#define	CV_OUT_BASE64X509CRLHEADER	( 0x9 )

#define	CV_OUT_HEXADDR	( 0xa )

#define	CV_OUT_HEXASCIIADDR	( 0xb )

#define	CV_OUT_ENCODEMASK	( 0xff )

#define	CVR_SEEK_NONE	( 0 )

#define	CVR_SEEK_EQ	( 0x1 )

#define	CVR_SEEK_LT	( 0x2 )

#define	CVR_SEEK_LE	( 0x4 )

#define	CVR_SEEK_GE	( 0x8 )

#define	CVR_SEEK_GT	( 0x10 )

#define	CVR_SEEK_MASK	( 0xff )

#define	CVR_SEEK_NODELTA	( 0x1000 )

#define	CVR_SORT_NONE	( 0 )

#define	CVR_SORT_ASCEND	( 0x1 )

#define	CVR_SORT_DESCEND	( 0x2 )

#define	CV_COLUMN_QUEUE_DEFAULT	( -1 )

#define	CV_COLUMN_LOG_DEFAULT	( -2 )

#define	CV_COLUMN_LOG_FAILED_DEFAULT	( -3 )



extern RPC_IF_HANDLE __MIDL_itf_certview_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_certview_0000_v0_0_s_ifspec;

#ifndef __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__
#define __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__

/* interface IEnumCERTVIEWCOLUMN */
/* [unique][helpstring][local][dual][uuid][object] */ 


EXTERN_C const IID IID_IEnumCERTVIEWCOLUMN;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9c735be2-57a5-11d1-9bdb-00c04fb683fa")
    IEnumCERTVIEWCOLUMN : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ LONG __RPC_FAR *pIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ LONG __RPC_FAR *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsIndexed( 
            /* [retval][out] */ LONG __RPC_FAR *pIndexed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxLength( 
            /* [retval][out] */ LONG __RPC_FAR *pMaxLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ LONG Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ LONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCERTVIEWCOLUMNVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetName )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDisplayName )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetType )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsIndexed )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndexed);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMaxLength )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pMaxLength);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetValue )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ LONG Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [in] */ LONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCERTVIEWCOLUMN __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumCERTVIEWCOLUMNVtbl;

    interface IEnumCERTVIEWCOLUMN
    {
        CONST_VTBL struct IEnumCERTVIEWCOLUMNVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCERTVIEWCOLUMN_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCERTVIEWCOLUMN_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCERTVIEWCOLUMN_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCERTVIEWCOLUMN_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumCERTVIEWCOLUMN_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumCERTVIEWCOLUMN_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumCERTVIEWCOLUMN_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumCERTVIEWCOLUMN_Next(This,pIndex)	\
    (This)->lpVtbl -> Next(This,pIndex)

#define IEnumCERTVIEWCOLUMN_GetName(This,pstrOut)	\
    (This)->lpVtbl -> GetName(This,pstrOut)

#define IEnumCERTVIEWCOLUMN_GetDisplayName(This,pstrOut)	\
    (This)->lpVtbl -> GetDisplayName(This,pstrOut)

#define IEnumCERTVIEWCOLUMN_GetType(This,pType)	\
    (This)->lpVtbl -> GetType(This,pType)

#define IEnumCERTVIEWCOLUMN_IsIndexed(This,pIndexed)	\
    (This)->lpVtbl -> IsIndexed(This,pIndexed)

#define IEnumCERTVIEWCOLUMN_GetMaxLength(This,pMaxLength)	\
    (This)->lpVtbl -> GetMaxLength(This,pMaxLength)

#define IEnumCERTVIEWCOLUMN_GetValue(This,Flags,pvarValue)	\
    (This)->lpVtbl -> GetValue(This,Flags,pvarValue)

#define IEnumCERTVIEWCOLUMN_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCERTVIEWCOLUMN_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCERTVIEWCOLUMN_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Next_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndex);


void __RPC_STUB IEnumCERTVIEWCOLUMN_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetName_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrOut);


void __RPC_STUB IEnumCERTVIEWCOLUMN_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetDisplayName_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrOut);


void __RPC_STUB IEnumCERTVIEWCOLUMN_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetType_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pType);


void __RPC_STUB IEnumCERTVIEWCOLUMN_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_IsIndexed_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndexed);


void __RPC_STUB IEnumCERTVIEWCOLUMN_IsIndexed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetMaxLength_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pMaxLength);


void __RPC_STUB IEnumCERTVIEWCOLUMN_GetMaxLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_GetValue_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [in] */ LONG Flags,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);


void __RPC_STUB IEnumCERTVIEWCOLUMN_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Skip_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [in] */ LONG celt);


void __RPC_STUB IEnumCERTVIEWCOLUMN_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Reset_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This);


void __RPC_STUB IEnumCERTVIEWCOLUMN_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWCOLUMN_Clone_Proxy( 
    IEnumCERTVIEWCOLUMN __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWCOLUMN_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCERTVIEWCOLUMN_INTERFACE_DEFINED__ */


#ifndef __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__
#define __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__

/* interface IEnumCERTVIEWATTRIBUTE */
/* [unique][helpstring][local][dual][uuid][object] */ 


EXTERN_C const IID IID_IEnumCERTVIEWATTRIBUTE;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e77db656-7653-11d1-9bde-00c04fb683fa")
    IEnumCERTVIEWATTRIBUTE : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ LONG __RPC_FAR *pIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ LONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCERTVIEWATTRIBUTEVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetName )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetValue )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [in] */ LONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumCERTVIEWATTRIBUTEVtbl;

    interface IEnumCERTVIEWATTRIBUTE
    {
        CONST_VTBL struct IEnumCERTVIEWATTRIBUTEVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCERTVIEWATTRIBUTE_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCERTVIEWATTRIBUTE_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCERTVIEWATTRIBUTE_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCERTVIEWATTRIBUTE_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumCERTVIEWATTRIBUTE_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumCERTVIEWATTRIBUTE_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumCERTVIEWATTRIBUTE_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumCERTVIEWATTRIBUTE_Next(This,pIndex)	\
    (This)->lpVtbl -> Next(This,pIndex)

#define IEnumCERTVIEWATTRIBUTE_GetName(This,pstrOut)	\
    (This)->lpVtbl -> GetName(This,pstrOut)

#define IEnumCERTVIEWATTRIBUTE_GetValue(This,pstrOut)	\
    (This)->lpVtbl -> GetValue(This,pstrOut)

#define IEnumCERTVIEWATTRIBUTE_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCERTVIEWATTRIBUTE_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCERTVIEWATTRIBUTE_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Next_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndex);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_GetName_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrOut);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_GetValue_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrOut);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Skip_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
    /* [in] */ LONG celt);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Reset_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWATTRIBUTE_Clone_Proxy( 
    IEnumCERTVIEWATTRIBUTE __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWATTRIBUTE_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCERTVIEWATTRIBUTE_INTERFACE_DEFINED__ */


#ifndef __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__
#define __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__

/* interface IEnumCERTVIEWEXTENSION */
/* [unique][helpstring][local][dual][uuid][object] */ 


EXTERN_C const IID IID_IEnumCERTVIEWEXTENSION;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e7dd1466-7653-11d1-9bde-00c04fb683fa")
    IEnumCERTVIEWEXTENSION : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ LONG __RPC_FAR *pIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [retval][out] */ LONG __RPC_FAR *pFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ LONG Type,
            /* [in] */ LONG Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ LONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCERTVIEWEXTENSIONVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetName )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrOut);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFlags )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pFlags);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetValue )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ LONG Type,
            /* [in] */ LONG Flags,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [in] */ LONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCERTVIEWEXTENSION __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumCERTVIEWEXTENSIONVtbl;

    interface IEnumCERTVIEWEXTENSION
    {
        CONST_VTBL struct IEnumCERTVIEWEXTENSIONVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCERTVIEWEXTENSION_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCERTVIEWEXTENSION_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCERTVIEWEXTENSION_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCERTVIEWEXTENSION_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumCERTVIEWEXTENSION_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumCERTVIEWEXTENSION_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumCERTVIEWEXTENSION_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumCERTVIEWEXTENSION_Next(This,pIndex)	\
    (This)->lpVtbl -> Next(This,pIndex)

#define IEnumCERTVIEWEXTENSION_GetName(This,pstrOut)	\
    (This)->lpVtbl -> GetName(This,pstrOut)

#define IEnumCERTVIEWEXTENSION_GetFlags(This,pFlags)	\
    (This)->lpVtbl -> GetFlags(This,pFlags)

#define IEnumCERTVIEWEXTENSION_GetValue(This,Type,Flags,pvarValue)	\
    (This)->lpVtbl -> GetValue(This,Type,Flags,pvarValue)

#define IEnumCERTVIEWEXTENSION_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCERTVIEWEXTENSION_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCERTVIEWEXTENSION_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Next_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndex);


void __RPC_STUB IEnumCERTVIEWEXTENSION_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetName_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrOut);


void __RPC_STUB IEnumCERTVIEWEXTENSION_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetFlags_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pFlags);


void __RPC_STUB IEnumCERTVIEWEXTENSION_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_GetValue_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [in] */ LONG Type,
    /* [in] */ LONG Flags,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarValue);


void __RPC_STUB IEnumCERTVIEWEXTENSION_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Skip_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [in] */ LONG celt);


void __RPC_STUB IEnumCERTVIEWEXTENSION_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Reset_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This);


void __RPC_STUB IEnumCERTVIEWEXTENSION_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWEXTENSION_Clone_Proxy( 
    IEnumCERTVIEWEXTENSION __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWEXTENSION_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCERTVIEWEXTENSION_INTERFACE_DEFINED__ */


#ifndef __IEnumCERTVIEWROW_INTERFACE_DEFINED__
#define __IEnumCERTVIEWROW_INTERFACE_DEFINED__

/* interface IEnumCERTVIEWROW */
/* [unique][helpstring][local][dual][uuid][object] */ 


EXTERN_C const IID IID_IEnumCERTVIEWROW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d1157f4c-5af2-11d1-9bdc-00c04fb683fa")
    IEnumCERTVIEWROW : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ LONG __RPC_FAR *pIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCertViewColumn( 
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCertViewAttribute( 
            /* [in] */ LONG Flags,
            /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCertViewExtension( 
            /* [in] */ LONG Flags,
            /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ LONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxIndex( 
            /* [retval][out] */ LONG __RPC_FAR *pIndex) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCERTVIEWROWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumCERTVIEWROW __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumCERTVIEWROW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCertViewColumn )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCertViewAttribute )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ LONG Flags,
            /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCertViewExtension )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ LONG Flags,
            /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [in] */ LONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumCERTVIEWROW __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMaxIndex )( 
            IEnumCERTVIEWROW __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pIndex);
        
        END_INTERFACE
    } IEnumCERTVIEWROWVtbl;

    interface IEnumCERTVIEWROW
    {
        CONST_VTBL struct IEnumCERTVIEWROWVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCERTVIEWROW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCERTVIEWROW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCERTVIEWROW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCERTVIEWROW_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumCERTVIEWROW_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumCERTVIEWROW_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumCERTVIEWROW_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumCERTVIEWROW_Next(This,pIndex)	\
    (This)->lpVtbl -> Next(This,pIndex)

#define IEnumCERTVIEWROW_EnumCertViewColumn(This,ppenum)	\
    (This)->lpVtbl -> EnumCertViewColumn(This,ppenum)

#define IEnumCERTVIEWROW_EnumCertViewAttribute(This,Flags,ppenum)	\
    (This)->lpVtbl -> EnumCertViewAttribute(This,Flags,ppenum)

#define IEnumCERTVIEWROW_EnumCertViewExtension(This,Flags,ppenum)	\
    (This)->lpVtbl -> EnumCertViewExtension(This,Flags,ppenum)

#define IEnumCERTVIEWROW_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCERTVIEWROW_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCERTVIEWROW_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#define IEnumCERTVIEWROW_GetMaxIndex(This,pIndex)	\
    (This)->lpVtbl -> GetMaxIndex(This,pIndex)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Next_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndex);


void __RPC_STUB IEnumCERTVIEWROW_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewColumn_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewAttribute_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [in] */ LONG Flags,
    /* [retval][out] */ IEnumCERTVIEWATTRIBUTE __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_EnumCertViewExtension_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [in] */ LONG Flags,
    /* [retval][out] */ IEnumCERTVIEWEXTENSION __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWROW_EnumCertViewExtension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Skip_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [in] */ LONG celt);


void __RPC_STUB IEnumCERTVIEWROW_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Reset_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This);


void __RPC_STUB IEnumCERTVIEWROW_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_Clone_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumCERTVIEWROW_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCERTVIEWROW_GetMaxIndex_Proxy( 
    IEnumCERTVIEWROW __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pIndex);


void __RPC_STUB IEnumCERTVIEWROW_GetMaxIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCERTVIEWROW_INTERFACE_DEFINED__ */


#ifndef __ICertView_INTERFACE_DEFINED__
#define __ICertView_INTERFACE_DEFINED__

/* interface ICertView */
/* [unique][helpstring][local][dual][uuid][object] */ 


EXTERN_C const IID IID_ICertView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c3fac344-1e84-11d1-9bd6-00c04fb683fa")
    ICertView : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OpenConnection( 
            /* [in] */ const BSTR strConfig) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCertViewColumn( 
            /* [in] */ LONG fResultColumn,
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColumnCount( 
            /* [in] */ LONG fResultColumn,
            /* [retval][out] */ LONG __RPC_FAR *pcColumn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetColumnIndex( 
            /* [in] */ LONG fResultColumn,
            /* [in] */ const BSTR strColumnName,
            /* [retval][out] */ LONG __RPC_FAR *pColumnIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResultColumnCount( 
            /* [in] */ LONG cResultColumn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResultColumn( 
            /* [in] */ LONG ColumnIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRestriction( 
            /* [in] */ LONG ColumnIndex,
            /* [in] */ LONG SeekOperator,
            /* [in] */ LONG SortOrder,
            /* [in] */ const VARIANT __RPC_FAR *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenView( 
            /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICertViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICertView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICertView __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICertView __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICertView __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICertView __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICertView __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICertView __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenConnection )( 
            ICertView __RPC_FAR * This,
            /* [in] */ const BSTR strConfig);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumCertViewColumn )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG fResultColumn,
            /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumnCount )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG fResultColumn,
            /* [retval][out] */ LONG __RPC_FAR *pcColumn);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumnIndex )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG fResultColumn,
            /* [in] */ const BSTR strColumnName,
            /* [retval][out] */ LONG __RPC_FAR *pColumnIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetResultColumnCount )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG cResultColumn);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetResultColumn )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG ColumnIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRestriction )( 
            ICertView __RPC_FAR * This,
            /* [in] */ LONG ColumnIndex,
            /* [in] */ LONG SeekOperator,
            /* [in] */ LONG SortOrder,
            /* [in] */ const VARIANT __RPC_FAR *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenView )( 
            ICertView __RPC_FAR * This,
            /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } ICertViewVtbl;

    interface ICertView
    {
        CONST_VTBL struct ICertViewVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICertView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICertView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICertView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICertView_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICertView_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICertView_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICertView_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICertView_OpenConnection(This,strConfig)	\
    (This)->lpVtbl -> OpenConnection(This,strConfig)

#define ICertView_EnumCertViewColumn(This,fResultColumn,ppenum)	\
    (This)->lpVtbl -> EnumCertViewColumn(This,fResultColumn,ppenum)

#define ICertView_GetColumnCount(This,fResultColumn,pcColumn)	\
    (This)->lpVtbl -> GetColumnCount(This,fResultColumn,pcColumn)

#define ICertView_GetColumnIndex(This,fResultColumn,strColumnName,pColumnIndex)	\
    (This)->lpVtbl -> GetColumnIndex(This,fResultColumn,strColumnName,pColumnIndex)

#define ICertView_SetResultColumnCount(This,cResultColumn)	\
    (This)->lpVtbl -> SetResultColumnCount(This,cResultColumn)

#define ICertView_SetResultColumn(This,ColumnIndex)	\
    (This)->lpVtbl -> SetResultColumn(This,ColumnIndex)

#define ICertView_SetRestriction(This,ColumnIndex,SeekOperator,SortOrder,pvarValue)	\
    (This)->lpVtbl -> SetRestriction(This,ColumnIndex,SeekOperator,SortOrder,pvarValue)

#define ICertView_OpenView(This,ppenum)	\
    (This)->lpVtbl -> OpenView(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICertView_OpenConnection_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ const BSTR strConfig);


void __RPC_STUB ICertView_OpenConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_EnumCertViewColumn_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG fResultColumn,
    /* [retval][out] */ IEnumCERTVIEWCOLUMN __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB ICertView_EnumCertViewColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_GetColumnCount_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG fResultColumn,
    /* [retval][out] */ LONG __RPC_FAR *pcColumn);


void __RPC_STUB ICertView_GetColumnCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_GetColumnIndex_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG fResultColumn,
    /* [in] */ const BSTR strColumnName,
    /* [retval][out] */ LONG __RPC_FAR *pColumnIndex);


void __RPC_STUB ICertView_GetColumnIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_SetResultColumnCount_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG cResultColumn);


void __RPC_STUB ICertView_SetResultColumnCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_SetResultColumn_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG ColumnIndex);


void __RPC_STUB ICertView_SetResultColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_SetRestriction_Proxy( 
    ICertView __RPC_FAR * This,
    /* [in] */ LONG ColumnIndex,
    /* [in] */ LONG SeekOperator,
    /* [in] */ LONG SortOrder,
    /* [in] */ const VARIANT __RPC_FAR *pvarValue);


void __RPC_STUB ICertView_SetRestriction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICertView_OpenView_Proxy( 
    ICertView __RPC_FAR * This,
    /* [retval][out] */ IEnumCERTVIEWROW __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB ICertView_OpenView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICertView_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


