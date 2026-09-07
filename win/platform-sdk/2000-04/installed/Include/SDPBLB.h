
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Tue Oct 05 15:14:32 1999
 */
/* Compiler settings for sdpblb.idl:
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

#ifndef __sdpblb_h__
#define __sdpblb_h__

/* Forward Declarations */ 

#ifndef __ITConferenceBlob_FWD_DEFINED__
#define __ITConferenceBlob_FWD_DEFINED__
typedef interface ITConferenceBlob ITConferenceBlob;
#endif 	/* __ITConferenceBlob_FWD_DEFINED__ */


#ifndef __ITMedia_FWD_DEFINED__
#define __ITMedia_FWD_DEFINED__
typedef interface ITMedia ITMedia;
#endif 	/* __ITMedia_FWD_DEFINED__ */


#ifndef __IEnumMedia_FWD_DEFINED__
#define __IEnumMedia_FWD_DEFINED__
typedef interface IEnumMedia IEnumMedia;
#endif 	/* __IEnumMedia_FWD_DEFINED__ */


#ifndef __ITMediaCollection_FWD_DEFINED__
#define __ITMediaCollection_FWD_DEFINED__
typedef interface ITMediaCollection ITMediaCollection;
#endif 	/* __ITMediaCollection_FWD_DEFINED__ */


#ifndef __ITTime_FWD_DEFINED__
#define __ITTime_FWD_DEFINED__
typedef interface ITTime ITTime;
#endif 	/* __ITTime_FWD_DEFINED__ */


#ifndef __IEnumTime_FWD_DEFINED__
#define __IEnumTime_FWD_DEFINED__
typedef interface IEnumTime IEnumTime;
#endif 	/* __IEnumTime_FWD_DEFINED__ */


#ifndef __ITTimeCollection_FWD_DEFINED__
#define __ITTimeCollection_FWD_DEFINED__
typedef interface ITTimeCollection ITTimeCollection;
#endif 	/* __ITTimeCollection_FWD_DEFINED__ */


#ifndef __ITSdp_FWD_DEFINED__
#define __ITSdp_FWD_DEFINED__
typedef interface ITSdp ITSdp;
#endif 	/* __ITSdp_FWD_DEFINED__ */


#ifndef __ITConnection_FWD_DEFINED__
#define __ITConnection_FWD_DEFINED__
typedef interface ITConnection ITConnection;
#endif 	/* __ITConnection_FWD_DEFINED__ */


#ifndef __ITAttributeList_FWD_DEFINED__
#define __ITAttributeList_FWD_DEFINED__
typedef interface ITAttributeList ITAttributeList;
#endif 	/* __ITAttributeList_FWD_DEFINED__ */


#ifndef __ITMedia_FWD_DEFINED__
#define __ITMedia_FWD_DEFINED__
typedef interface ITMedia ITMedia;
#endif 	/* __ITMedia_FWD_DEFINED__ */


#ifndef __ITTime_FWD_DEFINED__
#define __ITTime_FWD_DEFINED__
typedef interface ITTime ITTime;
#endif 	/* __ITTime_FWD_DEFINED__ */


#ifndef __ITConnection_FWD_DEFINED__
#define __ITConnection_FWD_DEFINED__
typedef interface ITConnection ITConnection;
#endif 	/* __ITConnection_FWD_DEFINED__ */


#ifndef __ITAttributeList_FWD_DEFINED__
#define __ITAttributeList_FWD_DEFINED__
typedef interface ITAttributeList ITAttributeList;
#endif 	/* __ITAttributeList_FWD_DEFINED__ */


#ifndef __SdpConferenceBlob_FWD_DEFINED__
#define __SdpConferenceBlob_FWD_DEFINED__

#ifdef __cplusplus
typedef class SdpConferenceBlob SdpConferenceBlob;
#else
typedef struct SdpConferenceBlob SdpConferenceBlob;
#endif /* __cplusplus */

#endif 	/* __SdpConferenceBlob_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_sdpblb_0000 */
/* [local] */ 

/* Copyright (c) 1998-1999  Microsoft Corporation  */

typedef 
enum BLOB_CHARACTER_SET
    {	BCS_ASCII	= 1,
	BCS_UTF7	= 2,
	BCS_UTF8	= 3
    }	BLOB_CHARACTER_SET;



extern RPC_IF_HANDLE __MIDL_itf_sdpblb_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sdpblb_0000_v0_0_s_ifspec;

#ifndef __ITConferenceBlob_INTERFACE_DEFINED__
#define __ITConferenceBlob_INTERFACE_DEFINED__

/* interface ITConferenceBlob */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITConferenceBlob;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C259D7AA-C8AB-11D0-8D58-00C04FD91AC0")
    ITConferenceBlob : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR pName,
            /* [in] */ BLOB_CHARACTER_SET CharacterSet,
            /* [in] */ BSTR pBlob) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CharacterSet( 
            /* [retval][out] */ BLOB_CHARACTER_SET __RPC_FAR *pCharacterSet) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConferenceBlob( 
            /* [retval][out] */ BSTR __RPC_FAR *ppBlob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetConferenceBlob( 
            /* [in] */ BLOB_CHARACTER_SET CharacterSet,
            /* [in] */ BSTR pBlob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITConferenceBlobVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITConferenceBlob __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITConferenceBlob __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ BSTR pName,
            /* [in] */ BLOB_CHARACTER_SET CharacterSet,
            /* [in] */ BSTR pBlob);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CharacterSet )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [retval][out] */ BLOB_CHARACTER_SET __RPC_FAR *pCharacterSet);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConferenceBlob )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppBlob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetConferenceBlob )( 
            ITConferenceBlob __RPC_FAR * This,
            /* [in] */ BLOB_CHARACTER_SET CharacterSet,
            /* [in] */ BSTR pBlob);
        
        END_INTERFACE
    } ITConferenceBlobVtbl;

    interface ITConferenceBlob
    {
        CONST_VTBL struct ITConferenceBlobVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITConferenceBlob_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITConferenceBlob_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITConferenceBlob_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITConferenceBlob_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITConferenceBlob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITConferenceBlob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITConferenceBlob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITConferenceBlob_Init(This,pName,CharacterSet,pBlob)	\
    (This)->lpVtbl -> Init(This,pName,CharacterSet,pBlob)

#define ITConferenceBlob_get_CharacterSet(This,pCharacterSet)	\
    (This)->lpVtbl -> get_CharacterSet(This,pCharacterSet)

#define ITConferenceBlob_get_ConferenceBlob(This,ppBlob)	\
    (This)->lpVtbl -> get_ConferenceBlob(This,ppBlob)

#define ITConferenceBlob_SetConferenceBlob(This,CharacterSet,pBlob)	\
    (This)->lpVtbl -> SetConferenceBlob(This,CharacterSet,pBlob)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConferenceBlob_Init_Proxy( 
    ITConferenceBlob __RPC_FAR * This,
    /* [in] */ BSTR pName,
    /* [in] */ BLOB_CHARACTER_SET CharacterSet,
    /* [in] */ BSTR pBlob);


void __RPC_STUB ITConferenceBlob_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConferenceBlob_get_CharacterSet_Proxy( 
    ITConferenceBlob __RPC_FAR * This,
    /* [retval][out] */ BLOB_CHARACTER_SET __RPC_FAR *pCharacterSet);


void __RPC_STUB ITConferenceBlob_get_CharacterSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConferenceBlob_get_ConferenceBlob_Proxy( 
    ITConferenceBlob __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppBlob);


void __RPC_STUB ITConferenceBlob_get_ConferenceBlob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConferenceBlob_SetConferenceBlob_Proxy( 
    ITConferenceBlob __RPC_FAR * This,
    /* [in] */ BLOB_CHARACTER_SET CharacterSet,
    /* [in] */ BSTR pBlob);


void __RPC_STUB ITConferenceBlob_SetConferenceBlob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITConferenceBlob_INTERFACE_DEFINED__ */


#ifndef __ITMedia_INTERFACE_DEFINED__
#define __ITMedia_INTERFACE_DEFINED__

/* interface ITMedia */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITMedia;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0CC1F053-CAEB-11D0-8D58-00C04FD91AC0")
    ITMedia : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaName( 
            /* [retval][out] */ BSTR __RPC_FAR *ppMediaName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MediaName( 
            /* [in] */ BSTR pMediaName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartPort( 
            /* [retval][out] */ LONG __RPC_FAR *pStartPort) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NumPorts( 
            /* [retval][out] */ LONG __RPC_FAR *pNumPorts) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransportProtocol( 
            /* [retval][out] */ BSTR __RPC_FAR *ppProtocol) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TransportProtocol( 
            /* [in] */ BSTR pProtocol) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FormatCodes( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FormatCodes( 
            /* [in] */ VARIANT NewVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaTitle( 
            /* [retval][out] */ BSTR __RPC_FAR *ppMediaTitle) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MediaTitle( 
            /* [in] */ BSTR pMediaTitle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPortInfo( 
            /* [in] */ LONG StartPort,
            /* [in] */ LONG NumPorts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITMediaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITMedia __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITMedia __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITMedia __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaName )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppMediaName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MediaName )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ BSTR pMediaName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartPort )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pStartPort);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumPorts )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pNumPorts);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransportProtocol )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppProtocol);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TransportProtocol )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ BSTR pProtocol);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FormatCodes )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FormatCodes )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ VARIANT NewVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaTitle )( 
            ITMedia __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppMediaTitle);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MediaTitle )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ BSTR pMediaTitle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPortInfo )( 
            ITMedia __RPC_FAR * This,
            /* [in] */ LONG StartPort,
            /* [in] */ LONG NumPorts);
        
        END_INTERFACE
    } ITMediaVtbl;

    interface ITMedia
    {
        CONST_VTBL struct ITMediaVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITMedia_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITMedia_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITMedia_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITMedia_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITMedia_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITMedia_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITMedia_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITMedia_get_MediaName(This,ppMediaName)	\
    (This)->lpVtbl -> get_MediaName(This,ppMediaName)

#define ITMedia_put_MediaName(This,pMediaName)	\
    (This)->lpVtbl -> put_MediaName(This,pMediaName)

#define ITMedia_get_StartPort(This,pStartPort)	\
    (This)->lpVtbl -> get_StartPort(This,pStartPort)

#define ITMedia_get_NumPorts(This,pNumPorts)	\
    (This)->lpVtbl -> get_NumPorts(This,pNumPorts)

#define ITMedia_get_TransportProtocol(This,ppProtocol)	\
    (This)->lpVtbl -> get_TransportProtocol(This,ppProtocol)

#define ITMedia_put_TransportProtocol(This,pProtocol)	\
    (This)->lpVtbl -> put_TransportProtocol(This,pProtocol)

#define ITMedia_get_FormatCodes(This,pVal)	\
    (This)->lpVtbl -> get_FormatCodes(This,pVal)

#define ITMedia_put_FormatCodes(This,NewVal)	\
    (This)->lpVtbl -> put_FormatCodes(This,NewVal)

#define ITMedia_get_MediaTitle(This,ppMediaTitle)	\
    (This)->lpVtbl -> get_MediaTitle(This,ppMediaTitle)

#define ITMedia_put_MediaTitle(This,pMediaTitle)	\
    (This)->lpVtbl -> put_MediaTitle(This,pMediaTitle)

#define ITMedia_SetPortInfo(This,StartPort,NumPorts)	\
    (This)->lpVtbl -> SetPortInfo(This,StartPort,NumPorts)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_MediaName_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppMediaName);


void __RPC_STUB ITMedia_get_MediaName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITMedia_put_MediaName_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [in] */ BSTR pMediaName);


void __RPC_STUB ITMedia_put_MediaName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_StartPort_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pStartPort);


void __RPC_STUB ITMedia_get_StartPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_NumPorts_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pNumPorts);


void __RPC_STUB ITMedia_get_NumPorts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_TransportProtocol_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppProtocol);


void __RPC_STUB ITMedia_get_TransportProtocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITMedia_put_TransportProtocol_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [in] */ BSTR pProtocol);


void __RPC_STUB ITMedia_put_TransportProtocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_FormatCodes_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ITMedia_get_FormatCodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITMedia_put_FormatCodes_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [in] */ VARIANT NewVal);


void __RPC_STUB ITMedia_put_FormatCodes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMedia_get_MediaTitle_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppMediaTitle);


void __RPC_STUB ITMedia_get_MediaTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITMedia_put_MediaTitle_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [in] */ BSTR pMediaTitle);


void __RPC_STUB ITMedia_put_MediaTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITMedia_SetPortInfo_Proxy( 
    ITMedia __RPC_FAR * This,
    /* [in] */ LONG StartPort,
    /* [in] */ LONG NumPorts);


void __RPC_STUB ITMedia_SetPortInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITMedia_INTERFACE_DEFINED__ */


#ifndef __IEnumMedia_INTERFACE_DEFINED__
#define __IEnumMedia_INTERFACE_DEFINED__

/* interface IEnumMedia */
/* [hidden][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumMedia;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CA8397BE-2FA4-11D1-9774-00C04FD91AC0")
    IEnumMedia : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumMediaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumMedia __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumMedia __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumMedia __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumMedia __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumMedia __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumMedia __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumMedia __RPC_FAR * This,
            /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IEnumMediaVtbl;

    interface IEnumMedia
    {
        CONST_VTBL struct IEnumMediaVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumMedia_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumMedia_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumMedia_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumMedia_Next(This,celt,pVal,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pVal,pceltFetched)

#define IEnumMedia_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumMedia_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumMedia_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumMedia_Next_Proxy( 
    IEnumMedia __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumMedia_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumMedia_Reset_Proxy( 
    IEnumMedia __RPC_FAR * This);


void __RPC_STUB IEnumMedia_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumMedia_Skip_Proxy( 
    IEnumMedia __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumMedia_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumMedia_Clone_Proxy( 
    IEnumMedia __RPC_FAR * This,
    /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumMedia_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumMedia_INTERFACE_DEFINED__ */


#ifndef __ITMediaCollection_INTERFACE_DEFINED__
#define __ITMediaCollection_INTERFACE_DEFINED__

/* interface ITMediaCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITMediaCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6A8E16A2-0ABC-11D1-976D-00C04FD91AC0")
    ITMediaCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG Index,
            /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_EnumerationIf( 
            /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ LONG Index,
            /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *ppMedia) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ LONG Index) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITMediaCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITMediaCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITMediaCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITMediaCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ITMediaCollection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ITMediaCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnumerationIf )( 
            ITMediaCollection __RPC_FAR * This,
            /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *ppMedia);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ITMediaCollection __RPC_FAR * This,
            /* [in] */ LONG Index);
        
        END_INTERFACE
    } ITMediaCollectionVtbl;

    interface ITMediaCollection
    {
        CONST_VTBL struct ITMediaCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITMediaCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITMediaCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITMediaCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITMediaCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITMediaCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITMediaCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITMediaCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITMediaCollection_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define ITMediaCollection_get_Item(This,Index,pVal)	\
    (This)->lpVtbl -> get_Item(This,Index,pVal)

#define ITMediaCollection_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define ITMediaCollection_get_EnumerationIf(This,pVal)	\
    (This)->lpVtbl -> get_EnumerationIf(This,pVal)

#define ITMediaCollection_Create(This,Index,ppMedia)	\
    (This)->lpVtbl -> Create(This,Index,ppMedia)

#define ITMediaCollection_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_get_Count_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB ITMediaCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_get_Item_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITMediaCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_get__NewEnum_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITMediaCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_get_EnumerationIf_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [retval][out] */ IEnumMedia __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITMediaCollection_get_EnumerationIf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_Create_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ ITMedia __RPC_FAR *__RPC_FAR *ppMedia);


void __RPC_STUB ITMediaCollection_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITMediaCollection_Delete_Proxy( 
    ITMediaCollection __RPC_FAR * This,
    /* [in] */ LONG Index);


void __RPC_STUB ITMediaCollection_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITMediaCollection_INTERFACE_DEFINED__ */


#ifndef __ITTime_INTERFACE_DEFINED__
#define __ITTime_INTERFACE_DEFINED__

/* interface ITTime */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITTime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2652BB78-1516-11D1-9771-00C04FD91AC0")
    ITTime : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartTime( 
            /* [retval][out] */ DOUBLE __RPC_FAR *pTime) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartTime( 
            /* [in] */ DOUBLE Time) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopTime( 
            /* [retval][out] */ DOUBLE __RPC_FAR *pTime) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopTime( 
            /* [in] */ DOUBLE Time) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITTimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITTime __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITTime __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITTime __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITTime __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITTime __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITTime __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITTime __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartTime )( 
            ITTime __RPC_FAR * This,
            /* [retval][out] */ DOUBLE __RPC_FAR *pTime);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartTime )( 
            ITTime __RPC_FAR * This,
            /* [in] */ DOUBLE Time);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StopTime )( 
            ITTime __RPC_FAR * This,
            /* [retval][out] */ DOUBLE __RPC_FAR *pTime);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StopTime )( 
            ITTime __RPC_FAR * This,
            /* [in] */ DOUBLE Time);
        
        END_INTERFACE
    } ITTimeVtbl;

    interface ITTime
    {
        CONST_VTBL struct ITTimeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITTime_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITTime_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITTime_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITTime_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITTime_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITTime_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITTime_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITTime_get_StartTime(This,pTime)	\
    (This)->lpVtbl -> get_StartTime(This,pTime)

#define ITTime_put_StartTime(This,Time)	\
    (This)->lpVtbl -> put_StartTime(This,Time)

#define ITTime_get_StopTime(This,pTime)	\
    (This)->lpVtbl -> get_StopTime(This,pTime)

#define ITTime_put_StopTime(This,Time)	\
    (This)->lpVtbl -> put_StopTime(This,Time)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITTime_get_StartTime_Proxy( 
    ITTime __RPC_FAR * This,
    /* [retval][out] */ DOUBLE __RPC_FAR *pTime);


void __RPC_STUB ITTime_get_StartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITTime_put_StartTime_Proxy( 
    ITTime __RPC_FAR * This,
    /* [in] */ DOUBLE Time);


void __RPC_STUB ITTime_put_StartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITTime_get_StopTime_Proxy( 
    ITTime __RPC_FAR * This,
    /* [retval][out] */ DOUBLE __RPC_FAR *pTime);


void __RPC_STUB ITTime_get_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITTime_put_StopTime_Proxy( 
    ITTime __RPC_FAR * This,
    /* [in] */ DOUBLE Time);


void __RPC_STUB ITTime_put_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITTime_INTERFACE_DEFINED__ */


#ifndef __IEnumTime_INTERFACE_DEFINED__
#define __IEnumTime_INTERFACE_DEFINED__

/* interface IEnumTime */
/* [hidden][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumTime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9055322E-2FA8-11D1-9774-00C04FD91AC0")
    IEnumTime : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ ITTime __RPC_FAR *__RPC_FAR *pVal,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumTimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumTime __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumTime __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumTime __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumTime __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ ITTime __RPC_FAR *__RPC_FAR *pVal,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumTime __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumTime __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumTime __RPC_FAR * This,
            /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IEnumTimeVtbl;

    interface IEnumTime
    {
        CONST_VTBL struct IEnumTimeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumTime_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumTime_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumTime_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumTime_Next(This,celt,pVal,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pVal,pceltFetched)

#define IEnumTime_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumTime_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumTime_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumTime_Next_Proxy( 
    IEnumTime __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ ITTime __RPC_FAR *__RPC_FAR *pVal,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumTime_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumTime_Reset_Proxy( 
    IEnumTime __RPC_FAR * This);


void __RPC_STUB IEnumTime_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumTime_Skip_Proxy( 
    IEnumTime __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumTime_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumTime_Clone_Proxy( 
    IEnumTime __RPC_FAR * This,
    /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumTime_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumTime_INTERFACE_DEFINED__ */


#ifndef __ITTimeCollection_INTERFACE_DEFINED__
#define __ITTimeCollection_INTERFACE_DEFINED__

/* interface ITTimeCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITTimeCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0CC1F04F-CAEB-11D0-8D58-00C04FD91AC0")
    ITTimeCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG Index,
            /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_EnumerationIf( 
            /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ LONG Index,
            /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *ppTime) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ LONG Index) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITTimeCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITTimeCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITTimeCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITTimeCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ITTimeCollection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ITTimeCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnumerationIf )( 
            ITTimeCollection __RPC_FAR * This,
            /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *ppTime);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ITTimeCollection __RPC_FAR * This,
            /* [in] */ LONG Index);
        
        END_INTERFACE
    } ITTimeCollectionVtbl;

    interface ITTimeCollection
    {
        CONST_VTBL struct ITTimeCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITTimeCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITTimeCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITTimeCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITTimeCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITTimeCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITTimeCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITTimeCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITTimeCollection_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define ITTimeCollection_get_Item(This,Index,pVal)	\
    (This)->lpVtbl -> get_Item(This,Index,pVal)

#define ITTimeCollection_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define ITTimeCollection_get_EnumerationIf(This,pVal)	\
    (This)->lpVtbl -> get_EnumerationIf(This,pVal)

#define ITTimeCollection_Create(This,Index,ppTime)	\
    (This)->lpVtbl -> Create(This,Index,ppTime)

#define ITTimeCollection_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_get_Count_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB ITTimeCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_get_Item_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITTimeCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_get__NewEnum_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITTimeCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_get_EnumerationIf_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [retval][out] */ IEnumTime __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB ITTimeCollection_get_EnumerationIf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_Create_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ ITTime __RPC_FAR *__RPC_FAR *ppTime);


void __RPC_STUB ITTimeCollection_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITTimeCollection_Delete_Proxy( 
    ITTimeCollection __RPC_FAR * This,
    /* [in] */ LONG Index);


void __RPC_STUB ITTimeCollection_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITTimeCollection_INTERFACE_DEFINED__ */


#ifndef __ITSdp_INTERFACE_DEFINED__
#define __ITSdp_INTERFACE_DEFINED__

/* interface ITSdp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITSdp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9B2719D8-B696-11D0-A489-00C04FD91AC0")
    ITSdp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsValid( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsValid) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProtocolVersion( 
            /* [retval][out] */ unsigned char __RPC_FAR *pProtocolVersion) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SessionId( 
            /* [retval][out] */ DOUBLE __RPC_FAR *pSessionId) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SessionVersion( 
            /* [retval][out] */ DOUBLE __RPC_FAR *pSessionVersion) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SessionVersion( 
            /* [in] */ DOUBLE SessionVersion) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MachineAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *ppMachineAddress) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MachineAddress( 
            /* [in] */ BSTR pMachineAddress) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *ppName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR pName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *ppDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR pDescription) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Url( 
            /* [retval][out] */ BSTR __RPC_FAR *ppUrl) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Url( 
            /* [in] */ BSTR pUrl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEmailNames( 
            /* [out] */ VARIANT __RPC_FAR *pAddresses,
            /* [out] */ VARIANT __RPC_FAR *pNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEmailNames( 
            /* [in] */ VARIANT Addresses,
            /* [in] */ VARIANT Names) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPhoneNumbers( 
            /* [out] */ VARIANT __RPC_FAR *pNumbers,
            /* [out] */ VARIANT __RPC_FAR *pNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPhoneNumbers( 
            /* [in] */ VARIANT Numbers,
            /* [in] */ VARIANT Names) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Originator( 
            /* [retval][out] */ BSTR __RPC_FAR *ppOriginator) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Originator( 
            /* [in] */ BSTR pOriginator) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MediaCollection( 
            /* [retval][out] */ ITMediaCollection __RPC_FAR *__RPC_FAR *ppMediaCollection) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeCollection( 
            /* [retval][out] */ ITTimeCollection __RPC_FAR *__RPC_FAR *ppTimeCollection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITSdpVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITSdp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITSdp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITSdp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsValid )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsValid);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProtocolVersion )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ unsigned char __RPC_FAR *pProtocolVersion);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SessionId )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ DOUBLE __RPC_FAR *pSessionId);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SessionVersion )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ DOUBLE __RPC_FAR *pSessionVersion);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SessionVersion )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ DOUBLE SessionVersion);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MachineAddress )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppMachineAddress);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MachineAddress )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ BSTR pMachineAddress);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ BSTR pName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ BSTR pDescription);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Url )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppUrl);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Url )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ BSTR pUrl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEmailNames )( 
            ITSdp __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *pAddresses,
            /* [out] */ VARIANT __RPC_FAR *pNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEmailNames )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ VARIANT Addresses,
            /* [in] */ VARIANT Names);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPhoneNumbers )( 
            ITSdp __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *pNumbers,
            /* [out] */ VARIANT __RPC_FAR *pNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPhoneNumbers )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ VARIANT Numbers,
            /* [in] */ VARIANT Names);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Originator )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppOriginator);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Originator )( 
            ITSdp __RPC_FAR * This,
            /* [in] */ BSTR pOriginator);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaCollection )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ ITMediaCollection __RPC_FAR *__RPC_FAR *ppMediaCollection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeCollection )( 
            ITSdp __RPC_FAR * This,
            /* [retval][out] */ ITTimeCollection __RPC_FAR *__RPC_FAR *ppTimeCollection);
        
        END_INTERFACE
    } ITSdpVtbl;

    interface ITSdp
    {
        CONST_VTBL struct ITSdpVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITSdp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITSdp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITSdp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITSdp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITSdp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITSdp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITSdp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITSdp_get_IsValid(This,pfIsValid)	\
    (This)->lpVtbl -> get_IsValid(This,pfIsValid)

#define ITSdp_get_ProtocolVersion(This,pProtocolVersion)	\
    (This)->lpVtbl -> get_ProtocolVersion(This,pProtocolVersion)

#define ITSdp_get_SessionId(This,pSessionId)	\
    (This)->lpVtbl -> get_SessionId(This,pSessionId)

#define ITSdp_get_SessionVersion(This,pSessionVersion)	\
    (This)->lpVtbl -> get_SessionVersion(This,pSessionVersion)

#define ITSdp_put_SessionVersion(This,SessionVersion)	\
    (This)->lpVtbl -> put_SessionVersion(This,SessionVersion)

#define ITSdp_get_MachineAddress(This,ppMachineAddress)	\
    (This)->lpVtbl -> get_MachineAddress(This,ppMachineAddress)

#define ITSdp_put_MachineAddress(This,pMachineAddress)	\
    (This)->lpVtbl -> put_MachineAddress(This,pMachineAddress)

#define ITSdp_get_Name(This,ppName)	\
    (This)->lpVtbl -> get_Name(This,ppName)

#define ITSdp_put_Name(This,pName)	\
    (This)->lpVtbl -> put_Name(This,pName)

#define ITSdp_get_Description(This,ppDescription)	\
    (This)->lpVtbl -> get_Description(This,ppDescription)

#define ITSdp_put_Description(This,pDescription)	\
    (This)->lpVtbl -> put_Description(This,pDescription)

#define ITSdp_get_Url(This,ppUrl)	\
    (This)->lpVtbl -> get_Url(This,ppUrl)

#define ITSdp_put_Url(This,pUrl)	\
    (This)->lpVtbl -> put_Url(This,pUrl)

#define ITSdp_GetEmailNames(This,pAddresses,pNames)	\
    (This)->lpVtbl -> GetEmailNames(This,pAddresses,pNames)

#define ITSdp_SetEmailNames(This,Addresses,Names)	\
    (This)->lpVtbl -> SetEmailNames(This,Addresses,Names)

#define ITSdp_GetPhoneNumbers(This,pNumbers,pNames)	\
    (This)->lpVtbl -> GetPhoneNumbers(This,pNumbers,pNames)

#define ITSdp_SetPhoneNumbers(This,Numbers,Names)	\
    (This)->lpVtbl -> SetPhoneNumbers(This,Numbers,Names)

#define ITSdp_get_Originator(This,ppOriginator)	\
    (This)->lpVtbl -> get_Originator(This,ppOriginator)

#define ITSdp_put_Originator(This,pOriginator)	\
    (This)->lpVtbl -> put_Originator(This,pOriginator)

#define ITSdp_get_MediaCollection(This,ppMediaCollection)	\
    (This)->lpVtbl -> get_MediaCollection(This,ppMediaCollection)

#define ITSdp_get_TimeCollection(This,ppTimeCollection)	\
    (This)->lpVtbl -> get_TimeCollection(This,ppTimeCollection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_IsValid_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsValid);


void __RPC_STUB ITSdp_get_IsValid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_ProtocolVersion_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ unsigned char __RPC_FAR *pProtocolVersion);


void __RPC_STUB ITSdp_get_ProtocolVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_SessionId_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ DOUBLE __RPC_FAR *pSessionId);


void __RPC_STUB ITSdp_get_SessionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_SessionVersion_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ DOUBLE __RPC_FAR *pSessionVersion);


void __RPC_STUB ITSdp_get_SessionVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_SessionVersion_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ DOUBLE SessionVersion);


void __RPC_STUB ITSdp_put_SessionVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_MachineAddress_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppMachineAddress);


void __RPC_STUB ITSdp_get_MachineAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_MachineAddress_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ BSTR pMachineAddress);


void __RPC_STUB ITSdp_put_MachineAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_Name_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppName);


void __RPC_STUB ITSdp_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_Name_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ BSTR pName);


void __RPC_STUB ITSdp_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_Description_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppDescription);


void __RPC_STUB ITSdp_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_Description_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ BSTR pDescription);


void __RPC_STUB ITSdp_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_Url_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppUrl);


void __RPC_STUB ITSdp_get_Url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_Url_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ BSTR pUrl);


void __RPC_STUB ITSdp_put_Url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITSdp_GetEmailNames_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *pAddresses,
    /* [out] */ VARIANT __RPC_FAR *pNames);


void __RPC_STUB ITSdp_GetEmailNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITSdp_SetEmailNames_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ VARIANT Addresses,
    /* [in] */ VARIANT Names);


void __RPC_STUB ITSdp_SetEmailNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITSdp_GetPhoneNumbers_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *pNumbers,
    /* [out] */ VARIANT __RPC_FAR *pNames);


void __RPC_STUB ITSdp_GetPhoneNumbers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITSdp_SetPhoneNumbers_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ VARIANT Numbers,
    /* [in] */ VARIANT Names);


void __RPC_STUB ITSdp_SetPhoneNumbers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_Originator_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppOriginator);


void __RPC_STUB ITSdp_get_Originator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITSdp_put_Originator_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [in] */ BSTR pOriginator);


void __RPC_STUB ITSdp_put_Originator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_MediaCollection_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ ITMediaCollection __RPC_FAR *__RPC_FAR *ppMediaCollection);


void __RPC_STUB ITSdp_get_MediaCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITSdp_get_TimeCollection_Proxy( 
    ITSdp __RPC_FAR * This,
    /* [retval][out] */ ITTimeCollection __RPC_FAR *__RPC_FAR *ppTimeCollection);


void __RPC_STUB ITSdp_get_TimeCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITSdp_INTERFACE_DEFINED__ */


#ifndef __ITConnection_INTERFACE_DEFINED__
#define __ITConnection_INTERFACE_DEFINED__

/* interface ITConnection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8fa381d4-c8c2-11d0-8d58-00c04fd91ac0")
    ITConnection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NetworkType( 
            /* [retval][out] */ BSTR __RPC_FAR *ppNetworkType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NetworkType( 
            /* [in] */ BSTR pNetworkType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AddressType( 
            /* [retval][out] */ BSTR __RPC_FAR *ppAddressType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AddressType( 
            /* [in] */ BSTR pAddressType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *ppStartAddress) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NumAddresses( 
            /* [retval][out] */ LONG __RPC_FAR *pNumAddresses) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Ttl( 
            /* [retval][out] */ unsigned char __RPC_FAR *pTtl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BandwidthModifier( 
            /* [retval][out] */ BSTR __RPC_FAR *ppModifier) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Bandwidth( 
            /* [retval][out] */ DOUBLE __RPC_FAR *pBandwidth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetAddressInfo( 
            /* [in] */ BSTR pStartAddress,
            /* [in] */ LONG NumAddresses,
            /* [in] */ unsigned char Ttl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetBandwidthInfo( 
            /* [in] */ BSTR pModifier,
            /* [in] */ DOUBLE Bandwidth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetEncryptionKey( 
            /* [in] */ BSTR pKeyType,
            /* [in] */ BSTR __RPC_FAR *ppKeyData) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEncryptionKey( 
            /* [out] */ BSTR __RPC_FAR *ppKeyType,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pfValidKeyData,
            /* [out] */ BSTR __RPC_FAR *ppKeyData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITConnection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITConnection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NetworkType )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppNetworkType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NetworkType )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ BSTR pNetworkType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AddressType )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppAddressType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AddressType )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ BSTR pAddressType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartAddress )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppStartAddress);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumAddresses )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pNumAddresses);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Ttl )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ unsigned char __RPC_FAR *pTtl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BandwidthModifier )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppModifier);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Bandwidth )( 
            ITConnection __RPC_FAR * This,
            /* [retval][out] */ DOUBLE __RPC_FAR *pBandwidth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAddressInfo )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ BSTR pStartAddress,
            /* [in] */ LONG NumAddresses,
            /* [in] */ unsigned char Ttl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetBandwidthInfo )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ BSTR pModifier,
            /* [in] */ DOUBLE Bandwidth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEncryptionKey )( 
            ITConnection __RPC_FAR * This,
            /* [in] */ BSTR pKeyType,
            /* [in] */ BSTR __RPC_FAR *ppKeyData);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEncryptionKey )( 
            ITConnection __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *ppKeyType,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pfValidKeyData,
            /* [out] */ BSTR __RPC_FAR *ppKeyData);
        
        END_INTERFACE
    } ITConnectionVtbl;

    interface ITConnection
    {
        CONST_VTBL struct ITConnectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITConnection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITConnection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITConnection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITConnection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITConnection_get_NetworkType(This,ppNetworkType)	\
    (This)->lpVtbl -> get_NetworkType(This,ppNetworkType)

#define ITConnection_put_NetworkType(This,pNetworkType)	\
    (This)->lpVtbl -> put_NetworkType(This,pNetworkType)

#define ITConnection_get_AddressType(This,ppAddressType)	\
    (This)->lpVtbl -> get_AddressType(This,ppAddressType)

#define ITConnection_put_AddressType(This,pAddressType)	\
    (This)->lpVtbl -> put_AddressType(This,pAddressType)

#define ITConnection_get_StartAddress(This,ppStartAddress)	\
    (This)->lpVtbl -> get_StartAddress(This,ppStartAddress)

#define ITConnection_get_NumAddresses(This,pNumAddresses)	\
    (This)->lpVtbl -> get_NumAddresses(This,pNumAddresses)

#define ITConnection_get_Ttl(This,pTtl)	\
    (This)->lpVtbl -> get_Ttl(This,pTtl)

#define ITConnection_get_BandwidthModifier(This,ppModifier)	\
    (This)->lpVtbl -> get_BandwidthModifier(This,ppModifier)

#define ITConnection_get_Bandwidth(This,pBandwidth)	\
    (This)->lpVtbl -> get_Bandwidth(This,pBandwidth)

#define ITConnection_SetAddressInfo(This,pStartAddress,NumAddresses,Ttl)	\
    (This)->lpVtbl -> SetAddressInfo(This,pStartAddress,NumAddresses,Ttl)

#define ITConnection_SetBandwidthInfo(This,pModifier,Bandwidth)	\
    (This)->lpVtbl -> SetBandwidthInfo(This,pModifier,Bandwidth)

#define ITConnection_SetEncryptionKey(This,pKeyType,ppKeyData)	\
    (This)->lpVtbl -> SetEncryptionKey(This,pKeyType,ppKeyData)

#define ITConnection_GetEncryptionKey(This,ppKeyType,pfValidKeyData,ppKeyData)	\
    (This)->lpVtbl -> GetEncryptionKey(This,ppKeyType,pfValidKeyData,ppKeyData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_NetworkType_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppNetworkType);


void __RPC_STUB ITConnection_get_NetworkType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITConnection_put_NetworkType_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [in] */ BSTR pNetworkType);


void __RPC_STUB ITConnection_put_NetworkType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_AddressType_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppAddressType);


void __RPC_STUB ITConnection_get_AddressType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITConnection_put_AddressType_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [in] */ BSTR pAddressType);


void __RPC_STUB ITConnection_put_AddressType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_StartAddress_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppStartAddress);


void __RPC_STUB ITConnection_get_StartAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_NumAddresses_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pNumAddresses);


void __RPC_STUB ITConnection_get_NumAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_Ttl_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ unsigned char __RPC_FAR *pTtl);


void __RPC_STUB ITConnection_get_Ttl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_BandwidthModifier_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppModifier);


void __RPC_STUB ITConnection_get_BandwidthModifier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITConnection_get_Bandwidth_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [retval][out] */ DOUBLE __RPC_FAR *pBandwidth);


void __RPC_STUB ITConnection_get_Bandwidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConnection_SetAddressInfo_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [in] */ BSTR pStartAddress,
    /* [in] */ LONG NumAddresses,
    /* [in] */ unsigned char Ttl);


void __RPC_STUB ITConnection_SetAddressInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConnection_SetBandwidthInfo_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [in] */ BSTR pModifier,
    /* [in] */ DOUBLE Bandwidth);


void __RPC_STUB ITConnection_SetBandwidthInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConnection_SetEncryptionKey_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [in] */ BSTR pKeyType,
    /* [in] */ BSTR __RPC_FAR *ppKeyData);


void __RPC_STUB ITConnection_SetEncryptionKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITConnection_GetEncryptionKey_Proxy( 
    ITConnection __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *ppKeyType,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pfValidKeyData,
    /* [out] */ BSTR __RPC_FAR *ppKeyData);


void __RPC_STUB ITConnection_GetEncryptionKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITConnection_INTERFACE_DEFINED__ */


#ifndef __ITAttributeList_INTERFACE_DEFINED__
#define __ITAttributeList_INTERFACE_DEFINED__

/* interface ITAttributeList */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITAttributeList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5037fb82-cae9-11d0-8d58-00c04fd91ac0")
    ITAttributeList : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ LONG Index,
            /* [in] */ BSTR pAttribute) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ LONG Index) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AttributeList( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AttributeList( 
            /* [in] */ VARIANT newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITAttributeListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITAttributeList __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITAttributeList __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITAttributeList __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ITAttributeList __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ LONG Index,
            /* [in] */ BSTR pAttribute);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ LONG Index);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AttributeList )( 
            ITAttributeList __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AttributeList )( 
            ITAttributeList __RPC_FAR * This,
            /* [in] */ VARIANT newVal);
        
        END_INTERFACE
    } ITAttributeListVtbl;

    interface ITAttributeList
    {
        CONST_VTBL struct ITAttributeListVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITAttributeList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITAttributeList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITAttributeList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITAttributeList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITAttributeList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITAttributeList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITAttributeList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITAttributeList_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define ITAttributeList_get_Item(This,Index,pVal)	\
    (This)->lpVtbl -> get_Item(This,Index,pVal)

#define ITAttributeList_Add(This,Index,pAttribute)	\
    (This)->lpVtbl -> Add(This,Index,pAttribute)

#define ITAttributeList_Delete(This,Index)	\
    (This)->lpVtbl -> Delete(This,Index)

#define ITAttributeList_get_AttributeList(This,pVal)	\
    (This)->lpVtbl -> get_AttributeList(This,pVal)

#define ITAttributeList_put_AttributeList(This,newVal)	\
    (This)->lpVtbl -> put_AttributeList(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITAttributeList_get_Count_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pVal);


void __RPC_STUB ITAttributeList_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITAttributeList_get_Item_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ITAttributeList_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAttributeList_Add_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [in] */ LONG Index,
    /* [in] */ BSTR pAttribute);


void __RPC_STUB ITAttributeList_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITAttributeList_Delete_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [in] */ LONG Index);


void __RPC_STUB ITAttributeList_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ITAttributeList_get_AttributeList_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ITAttributeList_get_AttributeList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ITAttributeList_put_AttributeList_Proxy( 
    ITAttributeList __RPC_FAR * This,
    /* [in] */ VARIANT newVal);


void __RPC_STUB ITAttributeList_put_AttributeList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITAttributeList_INTERFACE_DEFINED__ */



#ifndef __SDPBLBLib_LIBRARY_DEFINED__
#define __SDPBLBLib_LIBRARY_DEFINED__

/* library SDPBLBLib */
/* [helpstring][version][uuid] */ 






EXTERN_C const IID LIBID_SDPBLBLib;

EXTERN_C const CLSID CLSID_SdpConferenceBlob;

#ifdef __cplusplus

class DECLSPEC_UUID("9B2719DD-B696-11D0-A489-00C04FD91AC0")
SdpConferenceBlob;
#endif
#endif /* __SDPBLBLib_LIBRARY_DEFINED__ */

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


