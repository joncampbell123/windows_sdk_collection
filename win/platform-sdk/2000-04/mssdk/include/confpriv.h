
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Tue Oct 05 15:14:51 1999
 */
/* Compiler settings for confpriv.idl:
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

#ifndef __confpriv_h__
#define __confpriv_h__

/* Forward Declarations */ 

#ifndef __ITParticipant_FWD_DEFINED__
#define __ITParticipant_FWD_DEFINED__
typedef interface ITParticipant ITParticipant;
#endif 	/* __ITParticipant_FWD_DEFINED__ */


#ifndef __ITLocalParticipant_FWD_DEFINED__
#define __ITLocalParticipant_FWD_DEFINED__
typedef interface ITLocalParticipant ITLocalParticipant;
#endif 	/* __ITLocalParticipant_FWD_DEFINED__ */


#ifndef __IEnumParticipant_FWD_DEFINED__
#define __IEnumParticipant_FWD_DEFINED__
typedef interface IEnumParticipant IEnumParticipant;
#endif 	/* __IEnumParticipant_FWD_DEFINED__ */


#ifndef __ITParticipantControl_FWD_DEFINED__
#define __ITParticipantControl_FWD_DEFINED__
typedef interface ITParticipantControl ITParticipantControl;
#endif 	/* __ITParticipantControl_FWD_DEFINED__ */


#ifndef __ITParticipantSubStreamControl_FWD_DEFINED__
#define __ITParticipantSubStreamControl_FWD_DEFINED__
typedef interface ITParticipantSubStreamControl ITParticipantSubStreamControl;
#endif 	/* __ITParticipantSubStreamControl_FWD_DEFINED__ */


#ifndef __ITParticipantEvent_FWD_DEFINED__
#define __ITParticipantEvent_FWD_DEFINED__
typedef interface ITParticipantEvent ITParticipantEvent;
#endif 	/* __ITParticipantEvent_FWD_DEFINED__ */


/* header files for imported files */
#include "tapi3if.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_confpriv_0000 */
/* [local] */ 

/* Copyright (c) 1998-1999  Microsoft Corporation  */
#define	MAX_PARTICIPANT_TYPED_INFO_LENGTH	( 256 )

typedef 
enum PARTICIPANT_TYPED_INFO
    {	PTI_CANONICALNAME	= 0,
	PTI_NAME	= PTI_CANONICALNAME + 1,
	PTI_EMAILADDRESS	= PTI_NAME + 1,
	PTI_PHONENUMBER	= PTI_EMAILADDRESS + 1,
	PTI_LOCATION	= PTI_PHONENUMBER + 1,
	PTI_TOOL	= PTI_LOCATION + 1,
	PTI_NOTES	= PTI_TOOL + 1,
	PTI_PRIVATE	= PTI_NOTES + 1
    }	PARTICIPANT_TYPED_INFO;

typedef 
enum PARTICIPANT_EVENT
    {	PE_NEW_PARTICIPANT	= 0,
	PE_INFO_CHANGE	= PE_NEW_PARTICIPANT + 1,
	PE_PARTICIPANT_LEAVE	= PE_INFO_CHANGE + 1,
	PE_NEW_SUBSTREAM	= PE_PARTICIPANT_LEAVE + 1,
	PE_SUBSTREAM_REMOVED	= PE_NEW_SUBSTREAM + 1,
	PE_SUBSTREAM_MAPPED	= PE_SUBSTREAM_REMOVED + 1,
	PE_SUBSTREAM_UNMAPPED	= PE_SUBSTREAM_MAPPED + 1,
	PE_PARTICIPANT_TIMEOUT	= PE_SUBSTREAM_UNMAPPED + 1,
	PE_PARTICIPANT_RECOVERED	= PE_PARTICIPANT_TIMEOUT + 1,
	PE_PARTICIPANT_ACTIVE	= PE_PARTICIPANT_RECOVERED + 1,
	PE_PARTICIPANT_INACTIVE	= PE_PARTICIPANT_ACTIVE + 1,
	PE_LOCAL_TALKING	= PE_PARTICIPANT_INACTIVE + 1,
	PE_LOCAL_SILENT	= PE_LOCAL_TALKING + 1
    }	PARTICIPANT_EVENT;



extern RPC_IF_HANDLE __MIDL_itf_confpriv_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_confpriv_0000_v0_0_s_ifspec;

#ifndef __ITParticipant_INTERFACE_DEFINED__
#define __ITParticipant_INTERFACE_DEFINED__

/* interface ITParticipant */
/* [object][dual][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITParticipant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5899b820-5a34-11d2-95a0-00a0244d2298")
    ITParticipant : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParticipantTypedInfo( 
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [retval][out] */ BSTR __RPC_FAR *ppInfo) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaTypes( 
            /* [retval][out] */ long __RPC_FAR *plMediaType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Status( 
            /* [in] */ ITStream __RPC_FAR *pITStream,
            /* [in] */ VARIANT_BOOL fEnable) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [in] */ ITStream __RPC_FAR *pITStream,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Streams( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVariant) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE EnumerateStreams( 
            /* [retval][out] */ IEnumStream __RPC_FAR *__RPC_FAR *ppEnumStream) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITParticipantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITParticipant __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITParticipant __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITParticipant __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParticipantTypedInfo )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [retval][out] */ BSTR __RPC_FAR *ppInfo);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaTypes )( 
            ITParticipant __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMediaType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Status )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ ITStream __RPC_FAR *pITStream,
            /* [in] */ VARIANT_BOOL fEnable);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Status )( 
            ITParticipant __RPC_FAR * This,
            /* [in] */ ITStream __RPC_FAR *pITStream,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Streams )( 
            ITParticipant __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVariant);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumerateStreams )( 
            ITParticipant __RPC_FAR * This,
            /* [retval][out] */ IEnumStream __RPC_FAR *__RPC_FAR *ppEnumStream);
        
        END_INTERFACE
    } ITParticipantVtbl;

    interface ITParticipant
    {
        CONST_VTBL struct ITParticipantVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITParticipant_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITParticipant_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITParticipant_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITParticipant_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITParticipant_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITParticipant_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITParticipant_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITParticipant_get_ParticipantTypedInfo(This,InfoType,ppInfo)	\
    (This)->lpVtbl -> get_ParticipantTypedInfo(This,InfoType,ppInfo)

#define ITParticipant_get_MediaTypes(This,plMediaType)	\
    (This)->lpVtbl -> get_MediaTypes(This,plMediaType)

#define ITParticipant_put_Status(This,pITStream,fEnable)	\
    (This)->lpVtbl -> put_Status(This,pITStream,fEnable)

#define ITParticipant_get_Status(This,pITStream,pStatus)	\
    (This)->lpVtbl -> get_Status(This,pITStream,pStatus)

#define ITParticipant_get_Streams(This,pVariant)	\
    (This)->lpVtbl -> get_Streams(This,pVariant)

#define ITParticipant_EnumerateStreams(This,ppEnumStream)	\
    (This)->lpVtbl -> EnumerateStreams(This,ppEnumStream)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipant_get_ParticipantTypedInfo_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
    /* [retval][out] */ BSTR __RPC_FAR *ppInfo);


void __RPC_STUB ITParticipant_get_ParticipantTypedInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipant_get_MediaTypes_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMediaType);


void __RPC_STUB ITParticipant_get_MediaTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITParticipant_put_Status_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [in] */ ITStream __RPC_FAR *pITStream,
    /* [in] */ VARIANT_BOOL fEnable);


void __RPC_STUB ITParticipant_put_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipant_get_Status_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [in] */ ITStream __RPC_FAR *pITStream,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pStatus);


void __RPC_STUB ITParticipant_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipant_get_Streams_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVariant);


void __RPC_STUB ITParticipant_get_Streams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id] */ HRESULT STDMETHODCALLTYPE ITParticipant_EnumerateStreams_Proxy( 
    ITParticipant __RPC_FAR * This,
    /* [retval][out] */ IEnumStream __RPC_FAR *__RPC_FAR *ppEnumStream);


void __RPC_STUB ITParticipant_EnumerateStreams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITParticipant_INTERFACE_DEFINED__ */


#ifndef __ITLocalParticipant_INTERFACE_DEFINED__
#define __ITLocalParticipant_INTERFACE_DEFINED__

/* interface ITLocalParticipant */
/* [object][dual][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITLocalParticipant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39cbf055-f77a-11d2-a824-00c04f8ef6e3")
    ITLocalParticipant : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LocalParticipantTypedInfo( 
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [retval][out] */ BSTR __RPC_FAR *ppInfo) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LocalParticipantTypedInfo( 
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [in] */ BSTR ppInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITLocalParticipantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITLocalParticipant __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITLocalParticipant __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocalParticipantTypedInfo )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [retval][out] */ BSTR __RPC_FAR *ppInfo);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LocalParticipantTypedInfo )( 
            ITLocalParticipant __RPC_FAR * This,
            /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
            /* [in] */ BSTR ppInfo);
        
        END_INTERFACE
    } ITLocalParticipantVtbl;

    interface ITLocalParticipant
    {
        CONST_VTBL struct ITLocalParticipantVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITLocalParticipant_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITLocalParticipant_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITLocalParticipant_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITLocalParticipant_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITLocalParticipant_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITLocalParticipant_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITLocalParticipant_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITLocalParticipant_get_LocalParticipantTypedInfo(This,InfoType,ppInfo)	\
    (This)->lpVtbl -> get_LocalParticipantTypedInfo(This,InfoType,ppInfo)

#define ITLocalParticipant_put_LocalParticipantTypedInfo(This,InfoType,ppInfo)	\
    (This)->lpVtbl -> put_LocalParticipantTypedInfo(This,InfoType,ppInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITLocalParticipant_get_LocalParticipantTypedInfo_Proxy( 
    ITLocalParticipant __RPC_FAR * This,
    /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
    /* [retval][out] */ BSTR __RPC_FAR *ppInfo);


void __RPC_STUB ITLocalParticipant_get_LocalParticipantTypedInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITLocalParticipant_put_LocalParticipantTypedInfo_Proxy( 
    ITLocalParticipant __RPC_FAR * This,
    /* [in] */ PARTICIPANT_TYPED_INFO InfoType,
    /* [in] */ BSTR ppInfo);


void __RPC_STUB ITLocalParticipant_put_LocalParticipantTypedInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITLocalParticipant_INTERFACE_DEFINED__ */


#ifndef __IEnumParticipant_INTERFACE_DEFINED__
#define __IEnumParticipant_INTERFACE_DEFINED__

/* interface IEnumParticipant */
/* [object][unique][hidden][helpstring][uuid] */ 


EXTERN_C const IID IID_IEnumParticipant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0a91b56c-5a35-11d2-95a0-00a0244d2298")
    IEnumParticipant : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppElements,
            /* [full][out][in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumParticipantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumParticipant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumParticipant __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumParticipant __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumParticipant __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppElements,
            /* [full][out][in] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumParticipant __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumParticipant __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumParticipant __RPC_FAR * This,
            /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IEnumParticipantVtbl;

    interface IEnumParticipant
    {
        CONST_VTBL struct IEnumParticipantVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumParticipant_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumParticipant_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumParticipant_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumParticipant_Next(This,celt,ppElements,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched)

#define IEnumParticipant_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumParticipant_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumParticipant_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumParticipant_Next_Proxy( 
    IEnumParticipant __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppElements,
    /* [full][out][in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumParticipant_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumParticipant_Reset_Proxy( 
    IEnumParticipant __RPC_FAR * This);


void __RPC_STUB IEnumParticipant_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumParticipant_Skip_Proxy( 
    IEnumParticipant __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumParticipant_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumParticipant_Clone_Proxy( 
    IEnumParticipant __RPC_FAR * This,
    /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumParticipant_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumParticipant_INTERFACE_DEFINED__ */


#ifndef __ITParticipantControl_INTERFACE_DEFINED__
#define __ITParticipantControl_INTERFACE_DEFINED__

/* interface ITParticipantControl */
/* [object][dual][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITParticipantControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d2ee6684-5a34-11d2-95a0-00a0244d2298")
    ITParticipantControl : public IDispatch
    {
    public:
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE EnumerateParticipants( 
            /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnumParticipants) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Participants( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVariant) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITParticipantControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITParticipantControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITParticipantControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITParticipantControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITParticipantControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITParticipantControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITParticipantControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITParticipantControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumerateParticipants )( 
            ITParticipantControl __RPC_FAR * This,
            /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnumParticipants);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Participants )( 
            ITParticipantControl __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVariant);
        
        END_INTERFACE
    } ITParticipantControlVtbl;

    interface ITParticipantControl
    {
        CONST_VTBL struct ITParticipantControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITParticipantControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITParticipantControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITParticipantControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITParticipantControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITParticipantControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITParticipantControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITParticipantControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITParticipantControl_EnumerateParticipants(This,ppEnumParticipants)	\
    (This)->lpVtbl -> EnumerateParticipants(This,ppEnumParticipants)

#define ITParticipantControl_get_Participants(This,pVariant)	\
    (This)->lpVtbl -> get_Participants(This,pVariant)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [hidden][id] */ HRESULT STDMETHODCALLTYPE ITParticipantControl_EnumerateParticipants_Proxy( 
    ITParticipantControl __RPC_FAR * This,
    /* [retval][out] */ IEnumParticipant __RPC_FAR *__RPC_FAR *ppEnumParticipants);


void __RPC_STUB ITParticipantControl_EnumerateParticipants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantControl_get_Participants_Proxy( 
    ITParticipantControl __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVariant);


void __RPC_STUB ITParticipantControl_get_Participants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITParticipantControl_INTERFACE_DEFINED__ */


#ifndef __ITParticipantSubStreamControl_INTERFACE_DEFINED__
#define __ITParticipantSubStreamControl_INTERFACE_DEFINED__

/* interface ITParticipantSubStreamControl */
/* [object][dual][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITParticipantSubStreamControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2c679108-5a35-11d2-95a0-00a0244d2298")
    ITParticipantSubStreamControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubStreamFromParticipant( 
            /* [in] */ ITParticipant __RPC_FAR *pParticipant,
            /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppITSubStream) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParticipantFromSubStream( 
            /* [in] */ ITSubStream __RPC_FAR *pITSubStream,
            /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SwitchTerminalToSubStream( 
            /* [in] */ ITTerminal __RPC_FAR *pITTerminal,
            /* [in] */ ITSubStream __RPC_FAR *pITSubStream) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITParticipantSubStreamControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITParticipantSubStreamControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITParticipantSubStreamControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubStreamFromParticipant )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ ITParticipant __RPC_FAR *pParticipant,
            /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppITSubStream);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParticipantFromSubStream )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ ITSubStream __RPC_FAR *pITSubStream,
            /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SwitchTerminalToSubStream )( 
            ITParticipantSubStreamControl __RPC_FAR * This,
            /* [in] */ ITTerminal __RPC_FAR *pITTerminal,
            /* [in] */ ITSubStream __RPC_FAR *pITSubStream);
        
        END_INTERFACE
    } ITParticipantSubStreamControlVtbl;

    interface ITParticipantSubStreamControl
    {
        CONST_VTBL struct ITParticipantSubStreamControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITParticipantSubStreamControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITParticipantSubStreamControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITParticipantSubStreamControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITParticipantSubStreamControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITParticipantSubStreamControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITParticipantSubStreamControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITParticipantSubStreamControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITParticipantSubStreamControl_get_SubStreamFromParticipant(This,pParticipant,ppITSubStream)	\
    (This)->lpVtbl -> get_SubStreamFromParticipant(This,pParticipant,ppITSubStream)

#define ITParticipantSubStreamControl_get_ParticipantFromSubStream(This,pITSubStream,ppParticipant)	\
    (This)->lpVtbl -> get_ParticipantFromSubStream(This,pITSubStream,ppParticipant)

#define ITParticipantSubStreamControl_SwitchTerminalToSubStream(This,pITTerminal,pITSubStream)	\
    (This)->lpVtbl -> SwitchTerminalToSubStream(This,pITTerminal,pITSubStream)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantSubStreamControl_get_SubStreamFromParticipant_Proxy( 
    ITParticipantSubStreamControl __RPC_FAR * This,
    /* [in] */ ITParticipant __RPC_FAR *pParticipant,
    /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppITSubStream);


void __RPC_STUB ITParticipantSubStreamControl_get_SubStreamFromParticipant_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantSubStreamControl_get_ParticipantFromSubStream_Proxy( 
    ITParticipantSubStreamControl __RPC_FAR * This,
    /* [in] */ ITSubStream __RPC_FAR *pITSubStream,
    /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant);


void __RPC_STUB ITParticipantSubStreamControl_get_ParticipantFromSubStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITParticipantSubStreamControl_SwitchTerminalToSubStream_Proxy( 
    ITParticipantSubStreamControl __RPC_FAR * This,
    /* [in] */ ITTerminal __RPC_FAR *pITTerminal,
    /* [in] */ ITSubStream __RPC_FAR *pITSubStream);


void __RPC_STUB ITParticipantSubStreamControl_SwitchTerminalToSubStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITParticipantSubStreamControl_INTERFACE_DEFINED__ */


#ifndef __ITParticipantEvent_INTERFACE_DEFINED__
#define __ITParticipantEvent_INTERFACE_DEFINED__

/* interface ITParticipantEvent */
/* [object][dual][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_ITParticipantEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8bb35070-2dad-11d3-a580-00c04f8ef6e3")
    ITParticipantEvent : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Event( 
            /* [retval][out] */ PARTICIPANT_EVENT __RPC_FAR *pParticipantEvent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Participant( 
            /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubStream( 
            /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppSubStream) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITParticipantEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITParticipantEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITParticipantEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Event )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [retval][out] */ PARTICIPANT_EVENT __RPC_FAR *pParticipantEvent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Participant )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubStream )( 
            ITParticipantEvent __RPC_FAR * This,
            /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppSubStream);
        
        END_INTERFACE
    } ITParticipantEventVtbl;

    interface ITParticipantEvent
    {
        CONST_VTBL struct ITParticipantEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITParticipantEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITParticipantEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITParticipantEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITParticipantEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITParticipantEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITParticipantEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITParticipantEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITParticipantEvent_get_Event(This,pParticipantEvent)	\
    (This)->lpVtbl -> get_Event(This,pParticipantEvent)

#define ITParticipantEvent_get_Participant(This,ppParticipant)	\
    (This)->lpVtbl -> get_Participant(This,ppParticipant)

#define ITParticipantEvent_get_SubStream(This,ppSubStream)	\
    (This)->lpVtbl -> get_SubStream(This,ppSubStream)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantEvent_get_Event_Proxy( 
    ITParticipantEvent __RPC_FAR * This,
    /* [retval][out] */ PARTICIPANT_EVENT __RPC_FAR *pParticipantEvent);


void __RPC_STUB ITParticipantEvent_get_Event_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantEvent_get_Participant_Proxy( 
    ITParticipantEvent __RPC_FAR * This,
    /* [retval][out] */ ITParticipant __RPC_FAR *__RPC_FAR *ppParticipant);


void __RPC_STUB ITParticipantEvent_get_Participant_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITParticipantEvent_get_SubStream_Proxy( 
    ITParticipantEvent __RPC_FAR * This,
    /* [retval][out] */ ITSubStream __RPC_FAR *__RPC_FAR *ppSubStream);


void __RPC_STUB ITParticipantEvent_get_SubStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITParticipantEvent_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


