#ifdef _WIN64 

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:30:42 1999
 */
/* Compiler settings for control.odl:
    Oicf (OptLev=i2), W0, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
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


#ifndef __control_h__
#define __control_h__

/* Forward Declarations */ 

#ifndef __IAMCollection_FWD_DEFINED__
#define __IAMCollection_FWD_DEFINED__
typedef interface IAMCollection IAMCollection;
#endif 	/* __IAMCollection_FWD_DEFINED__ */


#ifndef __IMediaControl_FWD_DEFINED__
#define __IMediaControl_FWD_DEFINED__
typedef interface IMediaControl IMediaControl;
#endif 	/* __IMediaControl_FWD_DEFINED__ */


#ifndef __IMediaEvent_FWD_DEFINED__
#define __IMediaEvent_FWD_DEFINED__
typedef interface IMediaEvent IMediaEvent;
#endif 	/* __IMediaEvent_FWD_DEFINED__ */


#ifndef __IMediaEventEx_FWD_DEFINED__
#define __IMediaEventEx_FWD_DEFINED__
typedef interface IMediaEventEx IMediaEventEx;
#endif 	/* __IMediaEventEx_FWD_DEFINED__ */


#ifndef __IMediaPosition_FWD_DEFINED__
#define __IMediaPosition_FWD_DEFINED__
typedef interface IMediaPosition IMediaPosition;
#endif 	/* __IMediaPosition_FWD_DEFINED__ */


#ifndef __IBasicAudio_FWD_DEFINED__
#define __IBasicAudio_FWD_DEFINED__
typedef interface IBasicAudio IBasicAudio;
#endif 	/* __IBasicAudio_FWD_DEFINED__ */


#ifndef __IVideoWindow_FWD_DEFINED__
#define __IVideoWindow_FWD_DEFINED__
typedef interface IVideoWindow IVideoWindow;
#endif 	/* __IVideoWindow_FWD_DEFINED__ */


#ifndef __IBasicVideo_FWD_DEFINED__
#define __IBasicVideo_FWD_DEFINED__
typedef interface IBasicVideo IBasicVideo;
#endif 	/* __IBasicVideo_FWD_DEFINED__ */


#ifndef __IBasicVideo2_FWD_DEFINED__
#define __IBasicVideo2_FWD_DEFINED__
typedef interface IBasicVideo2 IBasicVideo2;
#endif 	/* __IBasicVideo2_FWD_DEFINED__ */


#ifndef __IDeferredCommand_FWD_DEFINED__
#define __IDeferredCommand_FWD_DEFINED__
typedef interface IDeferredCommand IDeferredCommand;
#endif 	/* __IDeferredCommand_FWD_DEFINED__ */


#ifndef __IQueueCommand_FWD_DEFINED__
#define __IQueueCommand_FWD_DEFINED__
typedef interface IQueueCommand IQueueCommand;
#endif 	/* __IQueueCommand_FWD_DEFINED__ */


#ifndef __FilgraphManager_FWD_DEFINED__
#define __FilgraphManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class FilgraphManager FilgraphManager;
#else
typedef struct FilgraphManager FilgraphManager;
#endif /* __cplusplus */

#endif 	/* __FilgraphManager_FWD_DEFINED__ */


#ifndef __IFilterInfo_FWD_DEFINED__
#define __IFilterInfo_FWD_DEFINED__
typedef interface IFilterInfo IFilterInfo;
#endif 	/* __IFilterInfo_FWD_DEFINED__ */


#ifndef __IRegFilterInfo_FWD_DEFINED__
#define __IRegFilterInfo_FWD_DEFINED__
typedef interface IRegFilterInfo IRegFilterInfo;
#endif 	/* __IRegFilterInfo_FWD_DEFINED__ */


#ifndef __IMediaTypeInfo_FWD_DEFINED__
#define __IMediaTypeInfo_FWD_DEFINED__
typedef interface IMediaTypeInfo IMediaTypeInfo;
#endif 	/* __IMediaTypeInfo_FWD_DEFINED__ */


#ifndef __IPinInfo_FWD_DEFINED__
#define __IPinInfo_FWD_DEFINED__
typedef interface IPinInfo IPinInfo;
#endif 	/* __IPinInfo_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __QuartzTypeLib_LIBRARY_DEFINED__
#define __QuartzTypeLib_LIBRARY_DEFINED__

/* library QuartzTypeLib */
/* [version][lcid][helpstring][uuid] */ 

typedef double REFTIME;

typedef LONG_PTR OAEVENT;

typedef LONG_PTR OAHWND;

typedef long OAFilterState;


DEFINE_GUID(LIBID_QuartzTypeLib,0x56a868b0,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#ifndef __IAMCollection_INTERFACE_DEFINED__
#define __IAMCollection_INTERFACE_DEFINED__

/* interface IAMCollection */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMCollection,0x56a868b9,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b9-0ad4-11ce-b03a-0020af0ba770")
    IAMCollection : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG __RPC_FAR *plCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long lItem,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IAMCollection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ long lItem,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IAMCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IAMCollectionVtbl;

    interface IAMCollection
    {
        CONST_VTBL struct IAMCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMCollection_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define IAMCollection_Item(This,lItem,ppUnk)	\
    (This)->lpVtbl -> Item(This,lItem,ppUnk)

#define IAMCollection_get__NewEnum(This,ppUnk)	\
    (This)->lpVtbl -> get__NewEnum(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IAMCollection_get_Count_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plCount);


void __RPC_STUB IAMCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAMCollection_Item_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [in] */ long lItem,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IAMCollection_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IAMCollection_get__NewEnum_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IAMCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMCollection_INTERFACE_DEFINED__ */


#ifndef __IMediaControl_INTERFACE_DEFINED__
#define __IMediaControl_INTERFACE_DEFINED__

/* interface IMediaControl */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaControl,0x56a868b1,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b1-0ad4-11ce-b03a-0020af0ba770")
    IMediaControl : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState __RPC_FAR *pfs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderFile( 
            /* [in] */ BSTR strFilename) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( 
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterCollection( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RegFilterCollection( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopWhenReady( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetState )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState __RPC_FAR *pfs);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RenderFile )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ BSTR strFilename);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddSourceFilter )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterCollection )( 
            IMediaControl __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RegFilterCollection )( 
            IMediaControl __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopWhenReady )( 
            IMediaControl __RPC_FAR * This);
        
        END_INTERFACE
    } IMediaControlVtbl;

    interface IMediaControl
    {
        CONST_VTBL struct IMediaControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaControl_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define IMediaControl_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IMediaControl_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IMediaControl_GetState(This,msTimeout,pfs)	\
    (This)->lpVtbl -> GetState(This,msTimeout,pfs)

#define IMediaControl_RenderFile(This,strFilename)	\
    (This)->lpVtbl -> RenderFile(This,strFilename)

#define IMediaControl_AddSourceFilter(This,strFilename,ppUnk)	\
    (This)->lpVtbl -> AddSourceFilter(This,strFilename,ppUnk)

#define IMediaControl_get_FilterCollection(This,ppUnk)	\
    (This)->lpVtbl -> get_FilterCollection(This,ppUnk)

#define IMediaControl_get_RegFilterCollection(This,ppUnk)	\
    (This)->lpVtbl -> get_RegFilterCollection(This,ppUnk)

#define IMediaControl_StopWhenReady(This)	\
    (This)->lpVtbl -> StopWhenReady(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaControl_Run_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_Pause_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_Stop_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_GetState_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ LONG msTimeout,
    /* [out] */ OAFilterState __RPC_FAR *pfs);


void __RPC_STUB IMediaControl_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_RenderFile_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ BSTR strFilename);


void __RPC_STUB IMediaControl_RenderFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_AddSourceFilter_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ BSTR strFilename,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_AddSourceFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaControl_get_FilterCollection_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_get_FilterCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaControl_get_RegFilterCollection_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_get_RegFilterCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_StopWhenReady_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_StopWhenReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaControl_INTERFACE_DEFINED__ */


#ifndef __IMediaEvent_INTERFACE_DEFINED__
#define __IMediaEvent_INTERFACE_DEFINED__

/* interface IMediaEvent */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaEvent,0x56a868b6,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b6-0ad4-11ce-b03a-0020af0ba770")
    IMediaEvent : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEventHandle( 
            /* [out] */ OAEVENT __RPC_FAR *hEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEvent( 
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ LONG_PTR __RPC_FAR *lParam1,
            /* [out] */ LONG_PTR __RPC_FAR *lParam2,
            /* [in] */ long msTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WaitForCompletion( 
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelDefaultHandling( 
            /* [in] */ long lEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RestoreDefaultHandling( 
            /* [in] */ long lEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeEventParams( 
            /* [in] */ long lEvCode,
            /* [in] */ LONG_PTR lParam1,
            /* [in] */ LONG_PTR lParam2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventHandle )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ OAEVENT __RPC_FAR *hEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvent )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ LONG_PTR __RPC_FAR *lParam1,
            /* [out] */ LONG_PTR __RPC_FAR *lParam2,
            /* [in] */ long msTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WaitForCompletion )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelDefaultHandling )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaultHandling )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeEventParams )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode,
            /* [in] */ LONG_PTR lParam1,
            /* [in] */ LONG_PTR lParam2);
        
        END_INTERFACE
    } IMediaEventVtbl;

    interface IMediaEvent
    {
        CONST_VTBL struct IMediaEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaEvent_GetEventHandle(This,hEvent)	\
    (This)->lpVtbl -> GetEventHandle(This,hEvent)

#define IMediaEvent_GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)	\
    (This)->lpVtbl -> GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)

#define IMediaEvent_WaitForCompletion(This,msTimeout,pEvCode)	\
    (This)->lpVtbl -> WaitForCompletion(This,msTimeout,pEvCode)

#define IMediaEvent_CancelDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> CancelDefaultHandling(This,lEvCode)

#define IMediaEvent_RestoreDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> RestoreDefaultHandling(This,lEvCode)

#define IMediaEvent_FreeEventParams(This,lEvCode,lParam1,lParam2)	\
    (This)->lpVtbl -> FreeEventParams(This,lEvCode,lParam1,lParam2)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaEvent_GetEventHandle_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [out] */ OAEVENT __RPC_FAR *hEvent);


void __RPC_STUB IMediaEvent_GetEventHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_GetEvent_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lEventCode,
    /* [out] */ LONG_PTR __RPC_FAR *lParam1,
    /* [out] */ LONG_PTR __RPC_FAR *lParam2,
    /* [in] */ long msTimeout);


void __RPC_STUB IMediaEvent_GetEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_WaitForCompletion_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long msTimeout,
    /* [out] */ long __RPC_FAR *pEvCode);


void __RPC_STUB IMediaEvent_WaitForCompletion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_CancelDefaultHandling_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode);


void __RPC_STUB IMediaEvent_CancelDefaultHandling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_RestoreDefaultHandling_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode);


void __RPC_STUB IMediaEvent_RestoreDefaultHandling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_FreeEventParams_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode,
    /* [in] */ LONG_PTR lParam1,
    /* [in] */ LONG_PTR lParam2);


void __RPC_STUB IMediaEvent_FreeEventParams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaEvent_INTERFACE_DEFINED__ */


#ifndef __IMediaEventEx_INTERFACE_DEFINED__
#define __IMediaEventEx_INTERFACE_DEFINED__

/* interface IMediaEventEx */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaEventEx,0x56a868c0,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868c0-0ad4-11ce-b03a-0020af0ba770")
    IMediaEventEx : public IMediaEvent
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetNotifyWindow( 
            /* [in] */ OAHWND hwnd,
            /* [in] */ long lMsg,
            /* [in] */ LONG_PTR lInstanceData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNotifyFlags( 
            /* [in] */ long lNoNotifyFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNotifyFlags( 
            /* [out] */ long __RPC_FAR *lplNoNotifyFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaEventExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaEventEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaEventEx __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventHandle )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ OAEVENT __RPC_FAR *hEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvent )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ LONG_PTR __RPC_FAR *lParam1,
            /* [out] */ LONG_PTR __RPC_FAR *lParam2,
            /* [in] */ long msTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WaitForCompletion )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelDefaultHandling )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaultHandling )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeEventParams )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode,
            /* [in] */ LONG_PTR lParam1,
            /* [in] */ LONG_PTR lParam2);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNotifyWindow )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ OAHWND hwnd,
            /* [in] */ long lMsg,
            /* [in] */ LONG_PTR lInstanceData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNotifyFlags )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lNoNotifyFlags);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNotifyFlags )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lplNoNotifyFlags);
        
        END_INTERFACE
    } IMediaEventExVtbl;

    interface IMediaEventEx
    {
        CONST_VTBL struct IMediaEventExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaEventEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaEventEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaEventEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaEventEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaEventEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaEventEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaEventEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaEventEx_GetEventHandle(This,hEvent)	\
    (This)->lpVtbl -> GetEventHandle(This,hEvent)

#define IMediaEventEx_GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)	\
    (This)->lpVtbl -> GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)

#define IMediaEventEx_WaitForCompletion(This,msTimeout,pEvCode)	\
    (This)->lpVtbl -> WaitForCompletion(This,msTimeout,pEvCode)

#define IMediaEventEx_CancelDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> CancelDefaultHandling(This,lEvCode)

#define IMediaEventEx_RestoreDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> RestoreDefaultHandling(This,lEvCode)

#define IMediaEventEx_FreeEventParams(This,lEvCode,lParam1,lParam2)	\
    (This)->lpVtbl -> FreeEventParams(This,lEvCode,lParam1,lParam2)


#define IMediaEventEx_SetNotifyWindow(This,hwnd,lMsg,lInstanceData)	\
    (This)->lpVtbl -> SetNotifyWindow(This,hwnd,lMsg,lInstanceData)

#define IMediaEventEx_SetNotifyFlags(This,lNoNotifyFlags)	\
    (This)->lpVtbl -> SetNotifyFlags(This,lNoNotifyFlags)

#define IMediaEventEx_GetNotifyFlags(This,lplNoNotifyFlags)	\
    (This)->lpVtbl -> GetNotifyFlags(This,lplNoNotifyFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaEventEx_SetNotifyWindow_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [in] */ OAHWND hwnd,
    /* [in] */ long lMsg,
    /* [in] */ LONG_PTR lInstanceData);


void __RPC_STUB IMediaEventEx_SetNotifyWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEventEx_SetNotifyFlags_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [in] */ long lNoNotifyFlags);


void __RPC_STUB IMediaEventEx_SetNotifyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEventEx_GetNotifyFlags_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lplNoNotifyFlags);


void __RPC_STUB IMediaEventEx_GetNotifyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaEventEx_INTERFACE_DEFINED__ */


#ifndef __IMediaPosition_INTERFACE_DEFINED__
#define __IMediaPosition_INTERFACE_DEFINED__

/* interface IMediaPosition */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaPosition,0x56a868b2,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b2-0ad4-11ce-b03a-0020af0ba770")
    IMediaPosition : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ REFTIME __RPC_FAR *plength) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StopTime( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_StopTime( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PrerollTime( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PrerollTime( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ double dRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ double __RPC_FAR *pdRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanSeekForward( 
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanSeekBackward( 
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaPositionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaPosition __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaPosition __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaPosition __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *plength);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StopTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StopTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrerollTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrerollTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ double dRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pdRate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanSeekForward )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanSeekBackward )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward);
        
        END_INTERFACE
    } IMediaPositionVtbl;

    interface IMediaPosition
    {
        CONST_VTBL struct IMediaPositionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaPosition_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaPosition_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaPosition_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaPosition_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaPosition_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaPosition_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaPosition_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaPosition_get_Duration(This,plength)	\
    (This)->lpVtbl -> get_Duration(This,plength)

#define IMediaPosition_put_CurrentPosition(This,llTime)	\
    (This)->lpVtbl -> put_CurrentPosition(This,llTime)

#define IMediaPosition_get_CurrentPosition(This,pllTime)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pllTime)

#define IMediaPosition_get_StopTime(This,pllTime)	\
    (This)->lpVtbl -> get_StopTime(This,pllTime)

#define IMediaPosition_put_StopTime(This,llTime)	\
    (This)->lpVtbl -> put_StopTime(This,llTime)

#define IMediaPosition_get_PrerollTime(This,pllTime)	\
    (This)->lpVtbl -> get_PrerollTime(This,pllTime)

#define IMediaPosition_put_PrerollTime(This,llTime)	\
    (This)->lpVtbl -> put_PrerollTime(This,llTime)

#define IMediaPosition_put_Rate(This,dRate)	\
    (This)->lpVtbl -> put_Rate(This,dRate)

#define IMediaPosition_get_Rate(This,pdRate)	\
    (This)->lpVtbl -> get_Rate(This,pdRate)

#define IMediaPosition_CanSeekForward(This,pCanSeekForward)	\
    (This)->lpVtbl -> CanSeekForward(This,pCanSeekForward)

#define IMediaPosition_CanSeekBackward(This,pCanSeekBackward)	\
    (This)->lpVtbl -> CanSeekBackward(This,pCanSeekBackward)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_Duration_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *plength);


void __RPC_STUB IMediaPosition_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_CurrentPosition_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_CurrentPosition_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_StopTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_StopTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_PrerollTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_PrerollTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_PrerollTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_PrerollTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_Rate_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ double dRate);


void __RPC_STUB IMediaPosition_put_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_Rate_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pdRate);


void __RPC_STUB IMediaPosition_get_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPosition_CanSeekForward_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward);


void __RPC_STUB IMediaPosition_CanSeekForward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPosition_CanSeekBackward_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward);


void __RPC_STUB IMediaPosition_CanSeekBackward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaPosition_INTERFACE_DEFINED__ */


#ifndef __IBasicAudio_INTERFACE_DEFINED__
#define __IBasicAudio_INTERFACE_DEFINED__

/* interface IBasicAudio */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicAudio,0x56a868b3,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b3-0ad4-11ce-b03a-0020af0ba770")
    IBasicAudio : public IDispatch
    {
    public:
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ long lVolume) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ long __RPC_FAR *plVolume) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Balance( 
            /* [in] */ long lBalance) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Balance( 
            /* [retval][out] */ long __RPC_FAR *plBalance) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicAudioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicAudio __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicAudio __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicAudio __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ long lVolume);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Volume )( 
            IBasicAudio __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVolume);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Balance )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ long lBalance);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Balance )( 
            IBasicAudio __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBalance);
        
        END_INTERFACE
    } IBasicAudioVtbl;

    interface IBasicAudio
    {
        CONST_VTBL struct IBasicAudioVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicAudio_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicAudio_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicAudio_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicAudio_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicAudio_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicAudio_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicAudio_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicAudio_put_Volume(This,lVolume)	\
    (This)->lpVtbl -> put_Volume(This,lVolume)

#define IBasicAudio_get_Volume(This,plVolume)	\
    (This)->lpVtbl -> get_Volume(This,plVolume)

#define IBasicAudio_put_Balance(This,lBalance)	\
    (This)->lpVtbl -> put_Balance(This,lBalance)

#define IBasicAudio_get_Balance(This,plBalance)	\
    (This)->lpVtbl -> get_Balance(This,plBalance)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicAudio_put_Volume_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [in] */ long lVolume);


void __RPC_STUB IBasicAudio_put_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicAudio_get_Volume_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVolume);


void __RPC_STUB IBasicAudio_get_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicAudio_put_Balance_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [in] */ long lBalance);


void __RPC_STUB IBasicAudio_put_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicAudio_get_Balance_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBalance);


void __RPC_STUB IBasicAudio_get_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicAudio_INTERFACE_DEFINED__ */


#ifndef __IVideoWindow_INTERFACE_DEFINED__
#define __IVideoWindow_INTERFACE_DEFINED__

/* interface IVideoWindow */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IVideoWindow,0x56a868b4,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b4-0ad4-11ce-b03a-0020af0ba770")
    IVideoWindow : public IDispatch
    {
    public:
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Caption( 
            /* [in] */ BSTR strCaption) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR __RPC_FAR *strCaption) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowStyle( 
            /* [in] */ long WindowStyle) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowStyle( 
            /* [retval][out] */ long __RPC_FAR *WindowStyle) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowStyleEx( 
            /* [in] */ long WindowStyleEx) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowStyleEx( 
            /* [retval][out] */ long __RPC_FAR *WindowStyleEx) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_AutoShow( 
            /* [in] */ long AutoShow) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AutoShow( 
            /* [retval][out] */ long __RPC_FAR *AutoShow) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ long WindowState) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ long __RPC_FAR *WindowState) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BackgroundPalette( 
            /* [in] */ long BackgroundPalette) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BackgroundPalette( 
            /* [retval][out] */ long __RPC_FAR *pBackgroundPalette) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ long Visible) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ long __RPC_FAR *pVisible) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Left( 
            /* [in] */ long Left) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Left( 
            /* [retval][out] */ long __RPC_FAR *pLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ long Width) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long __RPC_FAR *pWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Top( 
            /* [in] */ long Top) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Top( 
            /* [retval][out] */ long __RPC_FAR *pTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ long Height) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Owner( 
            /* [in] */ OAHWND Owner) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Owner( 
            /* [retval][out] */ OAHWND __RPC_FAR *Owner) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MessageDrain( 
            /* [in] */ OAHWND Drain) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MessageDrain( 
            /* [retval][out] */ OAHWND __RPC_FAR *Drain) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ long __RPC_FAR *Color) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ long Color) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FullScreenMode( 
            /* [retval][out] */ long __RPC_FAR *FullScreenMode) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FullScreenMode( 
            /* [in] */ long FullScreenMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWindowForeground( 
            /* [in] */ long Focus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyOwnerMessage( 
            /* [in] */ OAHWND hwnd,
            /* [in] */ long uMsg,
            /* [in] */ LONG_PTR wParam,
            /* [in] */ LONG_PTR lParam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWindowPosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWindowPosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMinIdealImageSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxIdealImageSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRestorePosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideCursor( 
            /* [in] */ long HideCursor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsCursorHidden( 
            /* [out] */ long __RPC_FAR *CursorHidden) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVideoWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IVideoWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IVideoWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Caption )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ BSTR strCaption);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Caption )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strCaption);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowStyle )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowStyle);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowStyle )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowStyle);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowStyleEx )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowStyleEx);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowStyleEx )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowStyleEx);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoShow )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long AutoShow);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoShow )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *AutoShow);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowState )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowState);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowState )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowState);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BackgroundPalette )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long BackgroundPalette);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BackgroundPalette )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBackgroundPalette);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Visible);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVisible);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Left);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Width);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Top);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Height);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Owner )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND Owner);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Owner )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ OAHWND __RPC_FAR *Owner);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MessageDrain )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND Drain);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MessageDrain )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ OAHWND __RPC_FAR *Drain);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderColor )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Color);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderColor )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Color);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenMode )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *FullScreenMode);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenMode )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long FullScreenMode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetWindowForeground )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Focus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NotifyOwnerMessage )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND hwnd,
            /* [in] */ long uMsg,
            /* [in] */ LONG_PTR wParam,
            /* [in] */ LONG_PTR lParam);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetWindowPosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWindowPosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMinIdealImageSize )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMaxIdealImageSize )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRestorePosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideCursor )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long HideCursor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCursorHidden )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *CursorHidden);
        
        END_INTERFACE
    } IVideoWindowVtbl;

    interface IVideoWindow
    {
        CONST_VTBL struct IVideoWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVideoWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVideoWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVideoWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVideoWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVideoWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVideoWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVideoWindow_put_Caption(This,strCaption)	\
    (This)->lpVtbl -> put_Caption(This,strCaption)

#define IVideoWindow_get_Caption(This,strCaption)	\
    (This)->lpVtbl -> get_Caption(This,strCaption)

#define IVideoWindow_put_WindowStyle(This,WindowStyle)	\
    (This)->lpVtbl -> put_WindowStyle(This,WindowStyle)

#define IVideoWindow_get_WindowStyle(This,WindowStyle)	\
    (This)->lpVtbl -> get_WindowStyle(This,WindowStyle)

#define IVideoWindow_put_WindowStyleEx(This,WindowStyleEx)	\
    (This)->lpVtbl -> put_WindowStyleEx(This,WindowStyleEx)

#define IVideoWindow_get_WindowStyleEx(This,WindowStyleEx)	\
    (This)->lpVtbl -> get_WindowStyleEx(This,WindowStyleEx)

#define IVideoWindow_put_AutoShow(This,AutoShow)	\
    (This)->lpVtbl -> put_AutoShow(This,AutoShow)

#define IVideoWindow_get_AutoShow(This,AutoShow)	\
    (This)->lpVtbl -> get_AutoShow(This,AutoShow)

#define IVideoWindow_put_WindowState(This,WindowState)	\
    (This)->lpVtbl -> put_WindowState(This,WindowState)

#define IVideoWindow_get_WindowState(This,WindowState)	\
    (This)->lpVtbl -> get_WindowState(This,WindowState)

#define IVideoWindow_put_BackgroundPalette(This,BackgroundPalette)	\
    (This)->lpVtbl -> put_BackgroundPalette(This,BackgroundPalette)

#define IVideoWindow_get_BackgroundPalette(This,pBackgroundPalette)	\
    (This)->lpVtbl -> get_BackgroundPalette(This,pBackgroundPalette)

#define IVideoWindow_put_Visible(This,Visible)	\
    (This)->lpVtbl -> put_Visible(This,Visible)

#define IVideoWindow_get_Visible(This,pVisible)	\
    (This)->lpVtbl -> get_Visible(This,pVisible)

#define IVideoWindow_put_Left(This,Left)	\
    (This)->lpVtbl -> put_Left(This,Left)

#define IVideoWindow_get_Left(This,pLeft)	\
    (This)->lpVtbl -> get_Left(This,pLeft)

#define IVideoWindow_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IVideoWindow_get_Width(This,pWidth)	\
    (This)->lpVtbl -> get_Width(This,pWidth)

#define IVideoWindow_put_Top(This,Top)	\
    (This)->lpVtbl -> put_Top(This,Top)

#define IVideoWindow_get_Top(This,pTop)	\
    (This)->lpVtbl -> get_Top(This,pTop)

#define IVideoWindow_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IVideoWindow_get_Height(This,pHeight)	\
    (This)->lpVtbl -> get_Height(This,pHeight)

#define IVideoWindow_put_Owner(This,Owner)	\
    (This)->lpVtbl -> put_Owner(This,Owner)

#define IVideoWindow_get_Owner(This,Owner)	\
    (This)->lpVtbl -> get_Owner(This,Owner)

#define IVideoWindow_put_MessageDrain(This,Drain)	\
    (This)->lpVtbl -> put_MessageDrain(This,Drain)

#define IVideoWindow_get_MessageDrain(This,Drain)	\
    (This)->lpVtbl -> get_MessageDrain(This,Drain)

#define IVideoWindow_get_BorderColor(This,Color)	\
    (This)->lpVtbl -> get_BorderColor(This,Color)

#define IVideoWindow_put_BorderColor(This,Color)	\
    (This)->lpVtbl -> put_BorderColor(This,Color)

#define IVideoWindow_get_FullScreenMode(This,FullScreenMode)	\
    (This)->lpVtbl -> get_FullScreenMode(This,FullScreenMode)

#define IVideoWindow_put_FullScreenMode(This,FullScreenMode)	\
    (This)->lpVtbl -> put_FullScreenMode(This,FullScreenMode)

#define IVideoWindow_SetWindowForeground(This,Focus)	\
    (This)->lpVtbl -> SetWindowForeground(This,Focus)

#define IVideoWindow_NotifyOwnerMessage(This,hwnd,uMsg,wParam,lParam)	\
    (This)->lpVtbl -> NotifyOwnerMessage(This,hwnd,uMsg,wParam,lParam)

#define IVideoWindow_SetWindowPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetWindowPosition(This,Left,Top,Width,Height)

#define IVideoWindow_GetWindowPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetWindowPosition(This,pLeft,pTop,pWidth,pHeight)

#define IVideoWindow_GetMinIdealImageSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetMinIdealImageSize(This,pWidth,pHeight)

#define IVideoWindow_GetMaxIdealImageSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetMaxIdealImageSize(This,pWidth,pHeight)

#define IVideoWindow_GetRestorePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetRestorePosition(This,pLeft,pTop,pWidth,pHeight)

#define IVideoWindow_HideCursor(This,HideCursor)	\
    (This)->lpVtbl -> HideCursor(This,HideCursor)

#define IVideoWindow_IsCursorHidden(This,CursorHidden)	\
    (This)->lpVtbl -> IsCursorHidden(This,CursorHidden)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Caption_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ BSTR strCaption);


void __RPC_STUB IVideoWindow_put_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Caption_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strCaption);


void __RPC_STUB IVideoWindow_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowStyle_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowStyle);


void __RPC_STUB IVideoWindow_put_WindowStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowStyle_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowStyle);


void __RPC_STUB IVideoWindow_get_WindowStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowStyleEx_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowStyleEx);


void __RPC_STUB IVideoWindow_put_WindowStyleEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowStyleEx_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowStyleEx);


void __RPC_STUB IVideoWindow_get_WindowStyleEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_AutoShow_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long AutoShow);


void __RPC_STUB IVideoWindow_put_AutoShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_AutoShow_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *AutoShow);


void __RPC_STUB IVideoWindow_get_AutoShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowState_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowState);


void __RPC_STUB IVideoWindow_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowState_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowState);


void __RPC_STUB IVideoWindow_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_BackgroundPalette_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long BackgroundPalette);


void __RPC_STUB IVideoWindow_put_BackgroundPalette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_BackgroundPalette_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBackgroundPalette);


void __RPC_STUB IVideoWindow_get_BackgroundPalette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Visible_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Visible);


void __RPC_STUB IVideoWindow_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Visible_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVisible);


void __RPC_STUB IVideoWindow_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Left_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Left);


void __RPC_STUB IVideoWindow_put_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Left_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLeft);


void __RPC_STUB IVideoWindow_get_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Width_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Width);


void __RPC_STUB IVideoWindow_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Width_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pWidth);


void __RPC_STUB IVideoWindow_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Top_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Top);


void __RPC_STUB IVideoWindow_put_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Top_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pTop);


void __RPC_STUB IVideoWindow_get_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Height_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Height);


void __RPC_STUB IVideoWindow_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Height_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Owner_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND Owner);


void __RPC_STUB IVideoWindow_put_Owner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Owner_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ OAHWND __RPC_FAR *Owner);


void __RPC_STUB IVideoWindow_get_Owner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_MessageDrain_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND Drain);


void __RPC_STUB IVideoWindow_put_MessageDrain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_MessageDrain_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ OAHWND __RPC_FAR *Drain);


void __RPC_STUB IVideoWindow_get_MessageDrain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_BorderColor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Color);


void __RPC_STUB IVideoWindow_get_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_BorderColor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Color);


void __RPC_STUB IVideoWindow_put_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_FullScreenMode_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *FullScreenMode);


void __RPC_STUB IVideoWindow_get_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_FullScreenMode_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long FullScreenMode);


void __RPC_STUB IVideoWindow_put_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_SetWindowForeground_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Focus);


void __RPC_STUB IVideoWindow_SetWindowForeground_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_NotifyOwnerMessage_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND hwnd,
    /* [in] */ long uMsg,
    /* [in] */ LONG_PTR wParam,
    /* [in] */ LONG_PTR lParam);


void __RPC_STUB IVideoWindow_NotifyOwnerMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_SetWindowPosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IVideoWindow_SetWindowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetWindowPosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetWindowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetMinIdealImageSize_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetMinIdealImageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetMaxIdealImageSize_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetMaxIdealImageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetRestorePosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetRestorePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_HideCursor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long HideCursor);


void __RPC_STUB IVideoWindow_HideCursor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_IsCursorHidden_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *CursorHidden);


void __RPC_STUB IVideoWindow_IsCursorHidden_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVideoWindow_INTERFACE_DEFINED__ */


#ifndef __IBasicVideo_INTERFACE_DEFINED__
#define __IBasicVideo_INTERFACE_DEFINED__

/* interface IBasicVideo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicVideo,0x56a868b5,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b5-0ad4-11ce-b03a-0020af0ba770")
    IBasicVideo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AvgTimePerFrame( 
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BitRate( 
            /* [retval][out] */ long __RPC_FAR *pBitRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BitErrorRate( 
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VideoWidth( 
            /* [retval][out] */ long __RPC_FAR *pVideoWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VideoHeight( 
            /* [retval][out] */ long __RPC_FAR *pVideoHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceLeft( 
            /* [in] */ long SourceLeft) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceLeft( 
            /* [retval][out] */ long __RPC_FAR *pSourceLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceWidth( 
            /* [in] */ long SourceWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceWidth( 
            /* [retval][out] */ long __RPC_FAR *pSourceWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceTop( 
            /* [in] */ long SourceTop) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceTop( 
            /* [retval][out] */ long __RPC_FAR *pSourceTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceHeight( 
            /* [in] */ long SourceHeight) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceHeight( 
            /* [retval][out] */ long __RPC_FAR *pSourceHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationLeft( 
            /* [in] */ long DestinationLeft) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationLeft( 
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationWidth( 
            /* [in] */ long DestinationWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationWidth( 
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationTop( 
            /* [in] */ long DestinationTop) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationTop( 
            /* [retval][out] */ long __RPC_FAR *pDestinationTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationHeight( 
            /* [in] */ long DestinationHeight) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationHeight( 
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSourcePosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourcePosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultSourcePosition( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDestinationPosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDestinationPosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultDestinationPosition( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoPaletteEntries( 
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentImage( 
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUsingDefaultSource( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUsingDefaultDestination( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicVideoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicVideo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AvgTimePerFrame )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitErrorRate )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSourcePosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSourcePosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultSourcePosition )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDestinationPosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDestinationPosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultDestinationPosition )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoSize )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoPaletteEntries )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentImage )( 
            IBasicVideo __RPC_FAR * This,
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultSource )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultDestination )( 
            IBasicVideo __RPC_FAR * This);
        
        END_INTERFACE
    } IBasicVideoVtbl;

    interface IBasicVideo
    {
        CONST_VTBL struct IBasicVideoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicVideo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicVideo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicVideo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicVideo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicVideo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicVideo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicVideo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicVideo_get_AvgTimePerFrame(This,pAvgTimePerFrame)	\
    (This)->lpVtbl -> get_AvgTimePerFrame(This,pAvgTimePerFrame)

#define IBasicVideo_get_BitRate(This,pBitRate)	\
    (This)->lpVtbl -> get_BitRate(This,pBitRate)

#define IBasicVideo_get_BitErrorRate(This,pBitErrorRate)	\
    (This)->lpVtbl -> get_BitErrorRate(This,pBitErrorRate)

#define IBasicVideo_get_VideoWidth(This,pVideoWidth)	\
    (This)->lpVtbl -> get_VideoWidth(This,pVideoWidth)

#define IBasicVideo_get_VideoHeight(This,pVideoHeight)	\
    (This)->lpVtbl -> get_VideoHeight(This,pVideoHeight)

#define IBasicVideo_put_SourceLeft(This,SourceLeft)	\
    (This)->lpVtbl -> put_SourceLeft(This,SourceLeft)

#define IBasicVideo_get_SourceLeft(This,pSourceLeft)	\
    (This)->lpVtbl -> get_SourceLeft(This,pSourceLeft)

#define IBasicVideo_put_SourceWidth(This,SourceWidth)	\
    (This)->lpVtbl -> put_SourceWidth(This,SourceWidth)

#define IBasicVideo_get_SourceWidth(This,pSourceWidth)	\
    (This)->lpVtbl -> get_SourceWidth(This,pSourceWidth)

#define IBasicVideo_put_SourceTop(This,SourceTop)	\
    (This)->lpVtbl -> put_SourceTop(This,SourceTop)

#define IBasicVideo_get_SourceTop(This,pSourceTop)	\
    (This)->lpVtbl -> get_SourceTop(This,pSourceTop)

#define IBasicVideo_put_SourceHeight(This,SourceHeight)	\
    (This)->lpVtbl -> put_SourceHeight(This,SourceHeight)

#define IBasicVideo_get_SourceHeight(This,pSourceHeight)	\
    (This)->lpVtbl -> get_SourceHeight(This,pSourceHeight)

#define IBasicVideo_put_DestinationLeft(This,DestinationLeft)	\
    (This)->lpVtbl -> put_DestinationLeft(This,DestinationLeft)

#define IBasicVideo_get_DestinationLeft(This,pDestinationLeft)	\
    (This)->lpVtbl -> get_DestinationLeft(This,pDestinationLeft)

#define IBasicVideo_put_DestinationWidth(This,DestinationWidth)	\
    (This)->lpVtbl -> put_DestinationWidth(This,DestinationWidth)

#define IBasicVideo_get_DestinationWidth(This,pDestinationWidth)	\
    (This)->lpVtbl -> get_DestinationWidth(This,pDestinationWidth)

#define IBasicVideo_put_DestinationTop(This,DestinationTop)	\
    (This)->lpVtbl -> put_DestinationTop(This,DestinationTop)

#define IBasicVideo_get_DestinationTop(This,pDestinationTop)	\
    (This)->lpVtbl -> get_DestinationTop(This,pDestinationTop)

#define IBasicVideo_put_DestinationHeight(This,DestinationHeight)	\
    (This)->lpVtbl -> put_DestinationHeight(This,DestinationHeight)

#define IBasicVideo_get_DestinationHeight(This,pDestinationHeight)	\
    (This)->lpVtbl -> get_DestinationHeight(This,pDestinationHeight)

#define IBasicVideo_SetSourcePosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetSourcePosition(This,Left,Top,Width,Height)

#define IBasicVideo_GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo_SetDefaultSourcePosition(This)	\
    (This)->lpVtbl -> SetDefaultSourcePosition(This)

#define IBasicVideo_SetDestinationPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetDestinationPosition(This,Left,Top,Width,Height)

#define IBasicVideo_GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo_SetDefaultDestinationPosition(This)	\
    (This)->lpVtbl -> SetDefaultDestinationPosition(This)

#define IBasicVideo_GetVideoSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetVideoSize(This,pWidth,pHeight)

#define IBasicVideo_GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)	\
    (This)->lpVtbl -> GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)

#define IBasicVideo_GetCurrentImage(This,pBufferSize,pDIBImage)	\
    (This)->lpVtbl -> GetCurrentImage(This,pBufferSize,pDIBImage)

#define IBasicVideo_IsUsingDefaultSource(This)	\
    (This)->lpVtbl -> IsUsingDefaultSource(This)

#define IBasicVideo_IsUsingDefaultDestination(This)	\
    (This)->lpVtbl -> IsUsingDefaultDestination(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_AvgTimePerFrame_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);


void __RPC_STUB IBasicVideo_get_AvgTimePerFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_BitRate_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBitRate);


void __RPC_STUB IBasicVideo_get_BitRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_BitErrorRate_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBitErrorRate);


void __RPC_STUB IBasicVideo_get_BitErrorRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_VideoWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVideoWidth);


void __RPC_STUB IBasicVideo_get_VideoWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_VideoHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVideoHeight);


void __RPC_STUB IBasicVideo_get_VideoHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceLeft);


void __RPC_STUB IBasicVideo_put_SourceLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceLeft);


void __RPC_STUB IBasicVideo_get_SourceLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceWidth);


void __RPC_STUB IBasicVideo_put_SourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceWidth);


void __RPC_STUB IBasicVideo_get_SourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceTop);


void __RPC_STUB IBasicVideo_put_SourceTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceTop);


void __RPC_STUB IBasicVideo_get_SourceTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceHeight);


void __RPC_STUB IBasicVideo_put_SourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceHeight);


void __RPC_STUB IBasicVideo_get_SourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationLeft);


void __RPC_STUB IBasicVideo_put_DestinationLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationLeft);


void __RPC_STUB IBasicVideo_get_DestinationLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationWidth);


void __RPC_STUB IBasicVideo_put_DestinationWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationWidth);


void __RPC_STUB IBasicVideo_get_DestinationWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationTop);


void __RPC_STUB IBasicVideo_put_DestinationTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationTop);


void __RPC_STUB IBasicVideo_get_DestinationTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationHeight);


void __RPC_STUB IBasicVideo_put_DestinationHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationHeight);


void __RPC_STUB IBasicVideo_get_DestinationHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IBasicVideo_SetSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDefaultSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_SetDefaultSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IBasicVideo_SetDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDefaultDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_SetDefaultDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetVideoSize_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetVideoSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetVideoPaletteEntries_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long StartIndex,
    /* [in] */ long Entries,
    /* [out] */ long __RPC_FAR *pRetrieved,
    /* [out] */ long __RPC_FAR *pPalette);


void __RPC_STUB IBasicVideo_GetVideoPaletteEntries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetCurrentImage_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out][in] */ long __RPC_FAR *pBufferSize,
    /* [out] */ long __RPC_FAR *pDIBImage);


void __RPC_STUB IBasicVideo_GetCurrentImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_IsUsingDefaultSource_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_IsUsingDefaultSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_IsUsingDefaultDestination_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_IsUsingDefaultDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicVideo_INTERFACE_DEFINED__ */


#ifndef __IBasicVideo2_INTERFACE_DEFINED__
#define __IBasicVideo2_INTERFACE_DEFINED__

/* interface IBasicVideo2 */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicVideo2,0x329bb360,0xf6ea,0x11d1,0x90,0x38,0x00,0xa0,0xc9,0x69,0x72,0x98);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("329bb360-f6ea-11d1-9038-00a0c9697298")
    IBasicVideo2 : public IBasicVideo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPreferredAspectRatio( 
            /* [out] */ long __RPC_FAR *plAspectX,
            /* [out] */ long __RPC_FAR *plAspectY) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicVideo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicVideo2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AvgTimePerFrame )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitErrorRate )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoSize )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoPaletteEntries )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentImage )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultSource )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultDestination )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPreferredAspectRatio )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *plAspectX,
            /* [out] */ long __RPC_FAR *plAspectY);
        
        END_INTERFACE
    } IBasicVideo2Vtbl;

    interface IBasicVideo2
    {
        CONST_VTBL struct IBasicVideo2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicVideo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicVideo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicVideo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicVideo2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicVideo2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicVideo2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicVideo2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicVideo2_get_AvgTimePerFrame(This,pAvgTimePerFrame)	\
    (This)->lpVtbl -> get_AvgTimePerFrame(This,pAvgTimePerFrame)

#define IBasicVideo2_get_BitRate(This,pBitRate)	\
    (This)->lpVtbl -> get_BitRate(This,pBitRate)

#define IBasicVideo2_get_BitErrorRate(This,pBitErrorRate)	\
    (This)->lpVtbl -> get_BitErrorRate(This,pBitErrorRate)

#define IBasicVideo2_get_VideoWidth(This,pVideoWidth)	\
    (This)->lpVtbl -> get_VideoWidth(This,pVideoWidth)

#define IBasicVideo2_get_VideoHeight(This,pVideoHeight)	\
    (This)->lpVtbl -> get_VideoHeight(This,pVideoHeight)

#define IBasicVideo2_put_SourceLeft(This,SourceLeft)	\
    (This)->lpVtbl -> put_SourceLeft(This,SourceLeft)

#define IBasicVideo2_get_SourceLeft(This,pSourceLeft)	\
    (This)->lpVtbl -> get_SourceLeft(This,pSourceLeft)

#define IBasicVideo2_put_SourceWidth(This,SourceWidth)	\
    (This)->lpVtbl -> put_SourceWidth(This,SourceWidth)

#define IBasicVideo2_get_SourceWidth(This,pSourceWidth)	\
    (This)->lpVtbl -> get_SourceWidth(This,pSourceWidth)

#define IBasicVideo2_put_SourceTop(This,SourceTop)	\
    (This)->lpVtbl -> put_SourceTop(This,SourceTop)

#define IBasicVideo2_get_SourceTop(This,pSourceTop)	\
    (This)->lpVtbl -> get_SourceTop(This,pSourceTop)

#define IBasicVideo2_put_SourceHeight(This,SourceHeight)	\
    (This)->lpVtbl -> put_SourceHeight(This,SourceHeight)

#define IBasicVideo2_get_SourceHeight(This,pSourceHeight)	\
    (This)->lpVtbl -> get_SourceHeight(This,pSourceHeight)

#define IBasicVideo2_put_DestinationLeft(This,DestinationLeft)	\
    (This)->lpVtbl -> put_DestinationLeft(This,DestinationLeft)

#define IBasicVideo2_get_DestinationLeft(This,pDestinationLeft)	\
    (This)->lpVtbl -> get_DestinationLeft(This,pDestinationLeft)

#define IBasicVideo2_put_DestinationWidth(This,DestinationWidth)	\
    (This)->lpVtbl -> put_DestinationWidth(This,DestinationWidth)

#define IBasicVideo2_get_DestinationWidth(This,pDestinationWidth)	\
    (This)->lpVtbl -> get_DestinationWidth(This,pDestinationWidth)

#define IBasicVideo2_put_DestinationTop(This,DestinationTop)	\
    (This)->lpVtbl -> put_DestinationTop(This,DestinationTop)

#define IBasicVideo2_get_DestinationTop(This,pDestinationTop)	\
    (This)->lpVtbl -> get_DestinationTop(This,pDestinationTop)

#define IBasicVideo2_put_DestinationHeight(This,DestinationHeight)	\
    (This)->lpVtbl -> put_DestinationHeight(This,DestinationHeight)

#define IBasicVideo2_get_DestinationHeight(This,pDestinationHeight)	\
    (This)->lpVtbl -> get_DestinationHeight(This,pDestinationHeight)

#define IBasicVideo2_SetSourcePosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetSourcePosition(This,Left,Top,Width,Height)

#define IBasicVideo2_GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo2_SetDefaultSourcePosition(This)	\
    (This)->lpVtbl -> SetDefaultSourcePosition(This)

#define IBasicVideo2_SetDestinationPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetDestinationPosition(This,Left,Top,Width,Height)

#define IBasicVideo2_GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo2_SetDefaultDestinationPosition(This)	\
    (This)->lpVtbl -> SetDefaultDestinationPosition(This)

#define IBasicVideo2_GetVideoSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetVideoSize(This,pWidth,pHeight)

#define IBasicVideo2_GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)	\
    (This)->lpVtbl -> GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)

#define IBasicVideo2_GetCurrentImage(This,pBufferSize,pDIBImage)	\
    (This)->lpVtbl -> GetCurrentImage(This,pBufferSize,pDIBImage)

#define IBasicVideo2_IsUsingDefaultSource(This)	\
    (This)->lpVtbl -> IsUsingDefaultSource(This)

#define IBasicVideo2_IsUsingDefaultDestination(This)	\
    (This)->lpVtbl -> IsUsingDefaultDestination(This)


#define IBasicVideo2_GetPreferredAspectRatio(This,plAspectX,plAspectY)	\
    (This)->lpVtbl -> GetPreferredAspectRatio(This,plAspectX,plAspectY)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBasicVideo2_GetPreferredAspectRatio_Proxy( 
    IBasicVideo2 __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *plAspectX,
    /* [out] */ long __RPC_FAR *plAspectY);


void __RPC_STUB IBasicVideo2_GetPreferredAspectRatio_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicVideo2_INTERFACE_DEFINED__ */


#ifndef __IDeferredCommand_INTERFACE_DEFINED__
#define __IDeferredCommand_INTERFACE_DEFINED__

/* interface IDeferredCommand */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IDeferredCommand,0x56a868b8,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b8-0ad4-11ce-b03a-0020af0ba770")
    IDeferredCommand : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Confidence( 
            /* [out] */ LONG __RPC_FAR *pConfidence) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Postpone( 
            /* [in] */ REFTIME newtime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHResult( 
            /* [out] */ HRESULT __RPC_FAR *phrResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeferredCommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDeferredCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDeferredCommand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDeferredCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            IDeferredCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Confidence )( 
            IDeferredCommand __RPC_FAR * This,
            /* [out] */ LONG __RPC_FAR *pConfidence);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Postpone )( 
            IDeferredCommand __RPC_FAR * This,
            /* [in] */ REFTIME newtime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHResult )( 
            IDeferredCommand __RPC_FAR * This,
            /* [out] */ HRESULT __RPC_FAR *phrResult);
        
        END_INTERFACE
    } IDeferredCommandVtbl;

    interface IDeferredCommand
    {
        CONST_VTBL struct IDeferredCommandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeferredCommand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeferredCommand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeferredCommand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeferredCommand_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define IDeferredCommand_Confidence(This,pConfidence)	\
    (This)->lpVtbl -> Confidence(This,pConfidence)

#define IDeferredCommand_Postpone(This,newtime)	\
    (This)->lpVtbl -> Postpone(This,newtime)

#define IDeferredCommand_GetHResult(This,phrResult)	\
    (This)->lpVtbl -> GetHResult(This,phrResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDeferredCommand_Cancel_Proxy( 
    IDeferredCommand __RPC_FAR * This);


void __RPC_STUB IDeferredCommand_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_Confidence_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [out] */ LONG __RPC_FAR *pConfidence);


void __RPC_STUB IDeferredCommand_Confidence_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_Postpone_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [in] */ REFTIME newtime);


void __RPC_STUB IDeferredCommand_Postpone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_GetHResult_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [out] */ HRESULT __RPC_FAR *phrResult);


void __RPC_STUB IDeferredCommand_GetHResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeferredCommand_INTERFACE_DEFINED__ */


#ifndef __IQueueCommand_INTERFACE_DEFINED__
#define __IQueueCommand_INTERFACE_DEFINED__

/* interface IQueueCommand */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IQueueCommand,0x56a868b7,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b7-0ad4-11ce-b03a-0020af0ba770")
    IQueueCommand : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InvokeAtStreamTime( 
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InvokeAtPresentationTime( 
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueueCommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQueueCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQueueCommand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQueueCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvokeAtStreamTime )( 
            IQueueCommand __RPC_FAR * This,
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvokeAtPresentationTime )( 
            IQueueCommand __RPC_FAR * This,
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IQueueCommandVtbl;

    interface IQueueCommand
    {
        CONST_VTBL struct IQueueCommandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueueCommand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueueCommand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueueCommand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueueCommand_InvokeAtStreamTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)	\
    (This)->lpVtbl -> InvokeAtStreamTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)

#define IQueueCommand_InvokeAtPresentationTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)	\
    (This)->lpVtbl -> InvokeAtPresentationTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQueueCommand_InvokeAtStreamTime_Proxy( 
    IQueueCommand __RPC_FAR * This,
    /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
    /* [in] */ REFTIME time,
    /* [in] */ GUID __RPC_FAR *iid,
    /* [in] */ long dispidMethod,
    /* [in] */ short wFlags,
    /* [in] */ long cArgs,
    /* [in] */ VARIANT __RPC_FAR *pDispParams,
    /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
    /* [out] */ short __RPC_FAR *puArgErr);


void __RPC_STUB IQueueCommand_InvokeAtStreamTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IQueueCommand_InvokeAtPresentationTime_Proxy( 
    IQueueCommand __RPC_FAR * This,
    /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
    /* [in] */ REFTIME time,
    /* [in] */ GUID __RPC_FAR *iid,
    /* [in] */ long dispidMethod,
    /* [in] */ short wFlags,
    /* [in] */ long cArgs,
    /* [in] */ VARIANT __RPC_FAR *pDispParams,
    /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
    /* [out] */ short __RPC_FAR *puArgErr);


void __RPC_STUB IQueueCommand_InvokeAtPresentationTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQueueCommand_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FilgraphManager,0xe436ebb3,0x524f,0x11ce,0x9f,0x53,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#ifdef __cplusplus

class DECLSPEC_UUID("e436ebb3-524f-11ce-9f53-0020af0ba770")
FilgraphManager;
#endif

#ifndef __IFilterInfo_INTERFACE_DEFINED__
#define __IFilterInfo_INTERFACE_DEFINED__

/* interface IFilterInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IFilterInfo,0x56a868ba,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868ba-0ad4-11ce-b03a-0020af0ba770")
    IFilterInfo : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FindPin( 
            /* [in] */ BSTR strPinID,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VendorInfo( 
            /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filter( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Pins( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsFileSource( 
            /* [retval][out] */ LONG __RPC_FAR *pbIsSource) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filename( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrFilename) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Filename( 
            /* [in] */ BSTR strFilename) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFilterInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IFilterInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IFilterInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IFilterInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FindPin )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ BSTR strPinID,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorInfo )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pins )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsFileSource )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pbIsSource);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filename )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrFilename);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filename )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ BSTR strFilename);
        
        END_INTERFACE
    } IFilterInfoVtbl;

    interface IFilterInfo
    {
        CONST_VTBL struct IFilterInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFilterInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFilterInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFilterInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFilterInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFilterInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFilterInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFilterInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFilterInfo_FindPin(This,strPinID,ppUnk)	\
    (This)->lpVtbl -> FindPin(This,strPinID,ppUnk)

#define IFilterInfo_get_Name(This,strName)	\
    (This)->lpVtbl -> get_Name(This,strName)

#define IFilterInfo_get_VendorInfo(This,strVendorInfo)	\
    (This)->lpVtbl -> get_VendorInfo(This,strVendorInfo)

#define IFilterInfo_get_Filter(This,ppUnk)	\
    (This)->lpVtbl -> get_Filter(This,ppUnk)

#define IFilterInfo_get_Pins(This,ppUnk)	\
    (This)->lpVtbl -> get_Pins(This,ppUnk)

#define IFilterInfo_get_IsFileSource(This,pbIsSource)	\
    (This)->lpVtbl -> get_IsFileSource(This,pbIsSource)

#define IFilterInfo_get_Filename(This,pstrFilename)	\
    (This)->lpVtbl -> get_Filename(This,pstrFilename)

#define IFilterInfo_put_Filename(This,strFilename)	\
    (This)->lpVtbl -> put_Filename(This,strFilename)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFilterInfo_FindPin_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [in] */ BSTR strPinID,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_FindPin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Name_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strName);


void __RPC_STUB IFilterInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_VendorInfo_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo);


void __RPC_STUB IFilterInfo_get_VendorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Filter_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_get_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Pins_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_get_Pins_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_IsFileSource_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pbIsSource);


void __RPC_STUB IFilterInfo_get_IsFileSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Filename_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrFilename);


void __RPC_STUB IFilterInfo_get_Filename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IFilterInfo_put_Filename_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [in] */ BSTR strFilename);


void __RPC_STUB IFilterInfo_put_Filename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFilterInfo_INTERFACE_DEFINED__ */


#ifndef __IRegFilterInfo_INTERFACE_DEFINED__
#define __IRegFilterInfo_INTERFACE_DEFINED__

/* interface IRegFilterInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IRegFilterInfo,0x56a868bb,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bb-0ad4-11ce-b03a-0020af0ba770")
    IRegFilterInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Filter( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRegFilterInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRegFilterInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRegFilterInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Filter )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IRegFilterInfoVtbl;

    interface IRegFilterInfo
    {
        CONST_VTBL struct IRegFilterInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegFilterInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRegFilterInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRegFilterInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRegFilterInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRegFilterInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRegFilterInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRegFilterInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRegFilterInfo_get_Name(This,strName)	\
    (This)->lpVtbl -> get_Name(This,strName)

#define IRegFilterInfo_Filter(This,ppUnk)	\
    (This)->lpVtbl -> Filter(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IRegFilterInfo_get_Name_Proxy( 
    IRegFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strName);


void __RPC_STUB IRegFilterInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRegFilterInfo_Filter_Proxy( 
    IRegFilterInfo __RPC_FAR * This,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IRegFilterInfo_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRegFilterInfo_INTERFACE_DEFINED__ */


#ifndef __IMediaTypeInfo_INTERFACE_DEFINED__
#define __IMediaTypeInfo_INTERFACE_DEFINED__

/* interface IMediaTypeInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaTypeInfo,0x56a868bc,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bc-0ad4-11ce-b03a-0020af0ba770")
    IMediaTypeInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Subtype( 
            /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaTypeInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaTypeInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaTypeInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Subtype )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strType);
        
        END_INTERFACE
    } IMediaTypeInfoVtbl;

    interface IMediaTypeInfo
    {
        CONST_VTBL struct IMediaTypeInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaTypeInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaTypeInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaTypeInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaTypeInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaTypeInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaTypeInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaTypeInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaTypeInfo_get_Type(This,strType)	\
    (This)->lpVtbl -> get_Type(This,strType)

#define IMediaTypeInfo_get_Subtype(This,strType)	\
    (This)->lpVtbl -> get_Subtype(This,strType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaTypeInfo_get_Type_Proxy( 
    IMediaTypeInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strType);


void __RPC_STUB IMediaTypeInfo_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaTypeInfo_get_Subtype_Proxy( 
    IMediaTypeInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strType);


void __RPC_STUB IMediaTypeInfo_get_Subtype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaTypeInfo_INTERFACE_DEFINED__ */


#ifndef __IPinInfo_INTERFACE_DEFINED__
#define __IPinInfo_INTERFACE_DEFINED__

/* interface IPinInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IPinInfo,0x56a868bd,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bd-0ad4-11ce-b03a-0020af0ba770")
    IPinInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Pin( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ConnectedTo( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionMediaType( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterInfo( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Direction( 
            /* [retval][out] */ LONG __RPC_FAR *ppDirection) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PinID( 
            /* [retval][out] */ BSTR __RPC_FAR *strPinID) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MediaTypes( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectDirect( 
            /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectWithType( 
            /* [in] */ IUnknown __RPC_FAR *pPin,
            /* [in] */ IDispatch __RPC_FAR *pMediaType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Render( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPinInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPinInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPinInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPinInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pin )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectedTo )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionMediaType )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterInfo )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Direction )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *ppDirection);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PinID )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strPinID);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaTypes )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectDirect )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectWithType )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin,
            /* [in] */ IDispatch __RPC_FAR *pMediaType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IPinInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Render )( 
            IPinInfo __RPC_FAR * This);
        
        END_INTERFACE
    } IPinInfoVtbl;

    interface IPinInfo
    {
        CONST_VTBL struct IPinInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPinInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPinInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPinInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPinInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPinInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPinInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPinInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPinInfo_get_Pin(This,ppUnk)	\
    (This)->lpVtbl -> get_Pin(This,ppUnk)

#define IPinInfo_get_ConnectedTo(This,ppUnk)	\
    (This)->lpVtbl -> get_ConnectedTo(This,ppUnk)

#define IPinInfo_get_ConnectionMediaType(This,ppUnk)	\
    (This)->lpVtbl -> get_ConnectionMediaType(This,ppUnk)

#define IPinInfo_get_FilterInfo(This,ppUnk)	\
    (This)->lpVtbl -> get_FilterInfo(This,ppUnk)

#define IPinInfo_get_Name(This,ppUnk)	\
    (This)->lpVtbl -> get_Name(This,ppUnk)

#define IPinInfo_get_Direction(This,ppDirection)	\
    (This)->lpVtbl -> get_Direction(This,ppDirection)

#define IPinInfo_get_PinID(This,strPinID)	\
    (This)->lpVtbl -> get_PinID(This,strPinID)

#define IPinInfo_get_MediaTypes(This,ppUnk)	\
    (This)->lpVtbl -> get_MediaTypes(This,ppUnk)

#define IPinInfo_Connect(This,pPin)	\
    (This)->lpVtbl -> Connect(This,pPin)

#define IPinInfo_ConnectDirect(This,pPin)	\
    (This)->lpVtbl -> ConnectDirect(This,pPin)

#define IPinInfo_ConnectWithType(This,pPin,pMediaType)	\
    (This)->lpVtbl -> ConnectWithType(This,pPin,pMediaType)

#define IPinInfo_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IPinInfo_Render(This)	\
    (This)->lpVtbl -> Render(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Pin_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_Pin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_ConnectedTo_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_ConnectedTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_ConnectionMediaType_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_ConnectionMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_FilterInfo_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_FilterInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Name_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Direction_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *ppDirection);


void __RPC_STUB IPinInfo_get_Direction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_PinID_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strPinID);


void __RPC_STUB IPinInfo_get_PinID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_MediaTypes_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_MediaTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Connect_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin);


void __RPC_STUB IPinInfo_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_ConnectDirect_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin);


void __RPC_STUB IPinInfo_ConnectDirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_ConnectWithType_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin,
    /* [in] */ IDispatch __RPC_FAR *pMediaType);


void __RPC_STUB IPinInfo_ConnectWithType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Disconnect_Proxy( 
    IPinInfo __RPC_FAR * This);


void __RPC_STUB IPinInfo_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Render_Proxy( 
    IPinInfo __RPC_FAR * This);


void __RPC_STUB IPinInfo_Render_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPinInfo_INTERFACE_DEFINED__ */

#endif /* __QuartzTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


#else // _WIN64 

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:30:45 1999
 */
/* Compiler settings for control.odl:
    Oicf (OptLev=i2), W0, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
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


#ifndef __control_h__
#define __control_h__

/* Forward Declarations */ 

#ifndef __IAMCollection_FWD_DEFINED__
#define __IAMCollection_FWD_DEFINED__
typedef interface IAMCollection IAMCollection;
#endif 	/* __IAMCollection_FWD_DEFINED__ */


#ifndef __IMediaControl_FWD_DEFINED__
#define __IMediaControl_FWD_DEFINED__
typedef interface IMediaControl IMediaControl;
#endif 	/* __IMediaControl_FWD_DEFINED__ */


#ifndef __IMediaEvent_FWD_DEFINED__
#define __IMediaEvent_FWD_DEFINED__
typedef interface IMediaEvent IMediaEvent;
#endif 	/* __IMediaEvent_FWD_DEFINED__ */


#ifndef __IMediaEventEx_FWD_DEFINED__
#define __IMediaEventEx_FWD_DEFINED__
typedef interface IMediaEventEx IMediaEventEx;
#endif 	/* __IMediaEventEx_FWD_DEFINED__ */


#ifndef __IMediaPosition_FWD_DEFINED__
#define __IMediaPosition_FWD_DEFINED__
typedef interface IMediaPosition IMediaPosition;
#endif 	/* __IMediaPosition_FWD_DEFINED__ */


#ifndef __IBasicAudio_FWD_DEFINED__
#define __IBasicAudio_FWD_DEFINED__
typedef interface IBasicAudio IBasicAudio;
#endif 	/* __IBasicAudio_FWD_DEFINED__ */


#ifndef __IVideoWindow_FWD_DEFINED__
#define __IVideoWindow_FWD_DEFINED__
typedef interface IVideoWindow IVideoWindow;
#endif 	/* __IVideoWindow_FWD_DEFINED__ */


#ifndef __IBasicVideo_FWD_DEFINED__
#define __IBasicVideo_FWD_DEFINED__
typedef interface IBasicVideo IBasicVideo;
#endif 	/* __IBasicVideo_FWD_DEFINED__ */


#ifndef __IBasicVideo2_FWD_DEFINED__
#define __IBasicVideo2_FWD_DEFINED__
typedef interface IBasicVideo2 IBasicVideo2;
#endif 	/* __IBasicVideo2_FWD_DEFINED__ */


#ifndef __IDeferredCommand_FWD_DEFINED__
#define __IDeferredCommand_FWD_DEFINED__
typedef interface IDeferredCommand IDeferredCommand;
#endif 	/* __IDeferredCommand_FWD_DEFINED__ */


#ifndef __IQueueCommand_FWD_DEFINED__
#define __IQueueCommand_FWD_DEFINED__
typedef interface IQueueCommand IQueueCommand;
#endif 	/* __IQueueCommand_FWD_DEFINED__ */


#ifndef __FilgraphManager_FWD_DEFINED__
#define __FilgraphManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class FilgraphManager FilgraphManager;
#else
typedef struct FilgraphManager FilgraphManager;
#endif /* __cplusplus */

#endif 	/* __FilgraphManager_FWD_DEFINED__ */


#ifndef __IFilterInfo_FWD_DEFINED__
#define __IFilterInfo_FWD_DEFINED__
typedef interface IFilterInfo IFilterInfo;
#endif 	/* __IFilterInfo_FWD_DEFINED__ */


#ifndef __IRegFilterInfo_FWD_DEFINED__
#define __IRegFilterInfo_FWD_DEFINED__
typedef interface IRegFilterInfo IRegFilterInfo;
#endif 	/* __IRegFilterInfo_FWD_DEFINED__ */


#ifndef __IMediaTypeInfo_FWD_DEFINED__
#define __IMediaTypeInfo_FWD_DEFINED__
typedef interface IMediaTypeInfo IMediaTypeInfo;
#endif 	/* __IMediaTypeInfo_FWD_DEFINED__ */


#ifndef __IPinInfo_FWD_DEFINED__
#define __IPinInfo_FWD_DEFINED__
typedef interface IPinInfo IPinInfo;
#endif 	/* __IPinInfo_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __QuartzTypeLib_LIBRARY_DEFINED__
#define __QuartzTypeLib_LIBRARY_DEFINED__

/* library QuartzTypeLib */
/* [version][lcid][helpstring][uuid] */ 

typedef double REFTIME;

typedef long OAEVENT;

typedef long OAHWND;

typedef long OAFilterState;


DEFINE_GUID(LIBID_QuartzTypeLib,0x56a868b0,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#ifndef __IAMCollection_INTERFACE_DEFINED__
#define __IAMCollection_INTERFACE_DEFINED__

/* interface IAMCollection */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IAMCollection,0x56a868b9,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b9-0ad4-11ce-b03a-0020af0ba770")
    IAMCollection : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ LONG __RPC_FAR *plCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long lItem,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAMCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAMCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAMCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAMCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IAMCollection __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IAMCollection __RPC_FAR * This,
            /* [in] */ long lItem,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IAMCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IAMCollectionVtbl;

    interface IAMCollection
    {
        CONST_VTBL struct IAMCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAMCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAMCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAMCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAMCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAMCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAMCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAMCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAMCollection_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define IAMCollection_Item(This,lItem,ppUnk)	\
    (This)->lpVtbl -> Item(This,lItem,ppUnk)

#define IAMCollection_get__NewEnum(This,ppUnk)	\
    (This)->lpVtbl -> get__NewEnum(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IAMCollection_get_Count_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plCount);


void __RPC_STUB IAMCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAMCollection_Item_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [in] */ long lItem,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IAMCollection_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IAMCollection_get__NewEnum_Proxy( 
    IAMCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IAMCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAMCollection_INTERFACE_DEFINED__ */


#ifndef __IMediaControl_INTERFACE_DEFINED__
#define __IMediaControl_INTERFACE_DEFINED__

/* interface IMediaControl */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaControl,0x56a868b1,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b1-0ad4-11ce-b03a-0020af0ba770")
    IMediaControl : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState __RPC_FAR *pfs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderFile( 
            /* [in] */ BSTR strFilename) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddSourceFilter( 
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterCollection( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RegFilterCollection( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopWhenReady( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Run )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IMediaControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetState )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ LONG msTimeout,
            /* [out] */ OAFilterState __RPC_FAR *pfs);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RenderFile )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ BSTR strFilename);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddSourceFilter )( 
            IMediaControl __RPC_FAR * This,
            /* [in] */ BSTR strFilename,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterCollection )( 
            IMediaControl __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RegFilterCollection )( 
            IMediaControl __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopWhenReady )( 
            IMediaControl __RPC_FAR * This);
        
        END_INTERFACE
    } IMediaControlVtbl;

    interface IMediaControl
    {
        CONST_VTBL struct IMediaControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaControl_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define IMediaControl_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IMediaControl_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IMediaControl_GetState(This,msTimeout,pfs)	\
    (This)->lpVtbl -> GetState(This,msTimeout,pfs)

#define IMediaControl_RenderFile(This,strFilename)	\
    (This)->lpVtbl -> RenderFile(This,strFilename)

#define IMediaControl_AddSourceFilter(This,strFilename,ppUnk)	\
    (This)->lpVtbl -> AddSourceFilter(This,strFilename,ppUnk)

#define IMediaControl_get_FilterCollection(This,ppUnk)	\
    (This)->lpVtbl -> get_FilterCollection(This,ppUnk)

#define IMediaControl_get_RegFilterCollection(This,ppUnk)	\
    (This)->lpVtbl -> get_RegFilterCollection(This,ppUnk)

#define IMediaControl_StopWhenReady(This)	\
    (This)->lpVtbl -> StopWhenReady(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaControl_Run_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_Pause_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_Stop_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_GetState_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ LONG msTimeout,
    /* [out] */ OAFilterState __RPC_FAR *pfs);


void __RPC_STUB IMediaControl_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_RenderFile_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ BSTR strFilename);


void __RPC_STUB IMediaControl_RenderFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_AddSourceFilter_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [in] */ BSTR strFilename,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_AddSourceFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaControl_get_FilterCollection_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_get_FilterCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaControl_get_RegFilterCollection_Proxy( 
    IMediaControl __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IMediaControl_get_RegFilterCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaControl_StopWhenReady_Proxy( 
    IMediaControl __RPC_FAR * This);


void __RPC_STUB IMediaControl_StopWhenReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaControl_INTERFACE_DEFINED__ */


#ifndef __IMediaEvent_INTERFACE_DEFINED__
#define __IMediaEvent_INTERFACE_DEFINED__

/* interface IMediaEvent */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaEvent,0x56a868b6,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b6-0ad4-11ce-b03a-0020af0ba770")
    IMediaEvent : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEventHandle( 
            /* [out] */ OAEVENT __RPC_FAR *hEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEvent( 
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ long __RPC_FAR *lParam1,
            /* [out] */ long __RPC_FAR *lParam2,
            /* [in] */ long msTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WaitForCompletion( 
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelDefaultHandling( 
            /* [in] */ long lEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RestoreDefaultHandling( 
            /* [in] */ long lEvCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeEventParams( 
            /* [in] */ long lEvCode,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventHandle )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ OAEVENT __RPC_FAR *hEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvent )( 
            IMediaEvent __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ long __RPC_FAR *lParam1,
            /* [out] */ long __RPC_FAR *lParam2,
            /* [in] */ long msTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WaitForCompletion )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelDefaultHandling )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaultHandling )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeEventParams )( 
            IMediaEvent __RPC_FAR * This,
            /* [in] */ long lEvCode,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2);
        
        END_INTERFACE
    } IMediaEventVtbl;

    interface IMediaEvent
    {
        CONST_VTBL struct IMediaEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaEvent_GetEventHandle(This,hEvent)	\
    (This)->lpVtbl -> GetEventHandle(This,hEvent)

#define IMediaEvent_GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)	\
    (This)->lpVtbl -> GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)

#define IMediaEvent_WaitForCompletion(This,msTimeout,pEvCode)	\
    (This)->lpVtbl -> WaitForCompletion(This,msTimeout,pEvCode)

#define IMediaEvent_CancelDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> CancelDefaultHandling(This,lEvCode)

#define IMediaEvent_RestoreDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> RestoreDefaultHandling(This,lEvCode)

#define IMediaEvent_FreeEventParams(This,lEvCode,lParam1,lParam2)	\
    (This)->lpVtbl -> FreeEventParams(This,lEvCode,lParam1,lParam2)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaEvent_GetEventHandle_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [out] */ OAEVENT __RPC_FAR *hEvent);


void __RPC_STUB IMediaEvent_GetEventHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_GetEvent_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lEventCode,
    /* [out] */ long __RPC_FAR *lParam1,
    /* [out] */ long __RPC_FAR *lParam2,
    /* [in] */ long msTimeout);


void __RPC_STUB IMediaEvent_GetEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_WaitForCompletion_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long msTimeout,
    /* [out] */ long __RPC_FAR *pEvCode);


void __RPC_STUB IMediaEvent_WaitForCompletion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_CancelDefaultHandling_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode);


void __RPC_STUB IMediaEvent_CancelDefaultHandling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_RestoreDefaultHandling_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode);


void __RPC_STUB IMediaEvent_RestoreDefaultHandling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEvent_FreeEventParams_Proxy( 
    IMediaEvent __RPC_FAR * This,
    /* [in] */ long lEvCode,
    /* [in] */ long lParam1,
    /* [in] */ long lParam2);


void __RPC_STUB IMediaEvent_FreeEventParams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaEvent_INTERFACE_DEFINED__ */


#ifndef __IMediaEventEx_INTERFACE_DEFINED__
#define __IMediaEventEx_INTERFACE_DEFINED__

/* interface IMediaEventEx */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaEventEx,0x56a868c0,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868c0-0ad4-11ce-b03a-0020af0ba770")
    IMediaEventEx : public IMediaEvent
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetNotifyWindow( 
            /* [in] */ OAHWND hwnd,
            /* [in] */ long lMsg,
            /* [in] */ long lInstanceData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNotifyFlags( 
            /* [in] */ long lNoNotifyFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNotifyFlags( 
            /* [out] */ long __RPC_FAR *lplNoNotifyFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaEventExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaEventEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaEventEx __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventHandle )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ OAEVENT __RPC_FAR *hEvent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEvent )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lEventCode,
            /* [out] */ long __RPC_FAR *lParam1,
            /* [out] */ long __RPC_FAR *lParam2,
            /* [in] */ long msTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WaitForCompletion )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long msTimeout,
            /* [out] */ long __RPC_FAR *pEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelDefaultHandling )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaultHandling )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeEventParams )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lEvCode,
            /* [in] */ long lParam1,
            /* [in] */ long lParam2);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNotifyWindow )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ OAHWND hwnd,
            /* [in] */ long lMsg,
            /* [in] */ long lInstanceData);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNotifyFlags )( 
            IMediaEventEx __RPC_FAR * This,
            /* [in] */ long lNoNotifyFlags);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNotifyFlags )( 
            IMediaEventEx __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lplNoNotifyFlags);
        
        END_INTERFACE
    } IMediaEventExVtbl;

    interface IMediaEventEx
    {
        CONST_VTBL struct IMediaEventExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaEventEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaEventEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaEventEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaEventEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaEventEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaEventEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaEventEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaEventEx_GetEventHandle(This,hEvent)	\
    (This)->lpVtbl -> GetEventHandle(This,hEvent)

#define IMediaEventEx_GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)	\
    (This)->lpVtbl -> GetEvent(This,lEventCode,lParam1,lParam2,msTimeout)

#define IMediaEventEx_WaitForCompletion(This,msTimeout,pEvCode)	\
    (This)->lpVtbl -> WaitForCompletion(This,msTimeout,pEvCode)

#define IMediaEventEx_CancelDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> CancelDefaultHandling(This,lEvCode)

#define IMediaEventEx_RestoreDefaultHandling(This,lEvCode)	\
    (This)->lpVtbl -> RestoreDefaultHandling(This,lEvCode)

#define IMediaEventEx_FreeEventParams(This,lEvCode,lParam1,lParam2)	\
    (This)->lpVtbl -> FreeEventParams(This,lEvCode,lParam1,lParam2)


#define IMediaEventEx_SetNotifyWindow(This,hwnd,lMsg,lInstanceData)	\
    (This)->lpVtbl -> SetNotifyWindow(This,hwnd,lMsg,lInstanceData)

#define IMediaEventEx_SetNotifyFlags(This,lNoNotifyFlags)	\
    (This)->lpVtbl -> SetNotifyFlags(This,lNoNotifyFlags)

#define IMediaEventEx_GetNotifyFlags(This,lplNoNotifyFlags)	\
    (This)->lpVtbl -> GetNotifyFlags(This,lplNoNotifyFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaEventEx_SetNotifyWindow_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [in] */ OAHWND hwnd,
    /* [in] */ long lMsg,
    /* [in] */ long lInstanceData);


void __RPC_STUB IMediaEventEx_SetNotifyWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEventEx_SetNotifyFlags_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [in] */ long lNoNotifyFlags);


void __RPC_STUB IMediaEventEx_SetNotifyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaEventEx_GetNotifyFlags_Proxy( 
    IMediaEventEx __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lplNoNotifyFlags);


void __RPC_STUB IMediaEventEx_GetNotifyFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaEventEx_INTERFACE_DEFINED__ */


#ifndef __IMediaPosition_INTERFACE_DEFINED__
#define __IMediaPosition_INTERFACE_DEFINED__

/* interface IMediaPosition */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaPosition,0x56a868b2,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b2-0ad4-11ce-b03a-0020af0ba770")
    IMediaPosition : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Duration( 
            /* [retval][out] */ REFTIME __RPC_FAR *plength) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CurrentPosition( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CurrentPosition( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StopTime( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_StopTime( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PrerollTime( 
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PrerollTime( 
            /* [in] */ REFTIME llTime) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Rate( 
            /* [in] */ double dRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Rate( 
            /* [retval][out] */ double __RPC_FAR *pdRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanSeekForward( 
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanSeekBackward( 
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaPositionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaPosition __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaPosition __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaPosition __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Duration )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *plength);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentPosition )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentPosition )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StopTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StopTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrerollTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pllTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrerollTime )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ REFTIME llTime);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Rate )( 
            IMediaPosition __RPC_FAR * This,
            /* [in] */ double dRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rate )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pdRate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanSeekForward )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanSeekBackward )( 
            IMediaPosition __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward);
        
        END_INTERFACE
    } IMediaPositionVtbl;

    interface IMediaPosition
    {
        CONST_VTBL struct IMediaPositionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaPosition_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaPosition_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaPosition_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaPosition_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaPosition_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaPosition_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaPosition_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaPosition_get_Duration(This,plength)	\
    (This)->lpVtbl -> get_Duration(This,plength)

#define IMediaPosition_put_CurrentPosition(This,llTime)	\
    (This)->lpVtbl -> put_CurrentPosition(This,llTime)

#define IMediaPosition_get_CurrentPosition(This,pllTime)	\
    (This)->lpVtbl -> get_CurrentPosition(This,pllTime)

#define IMediaPosition_get_StopTime(This,pllTime)	\
    (This)->lpVtbl -> get_StopTime(This,pllTime)

#define IMediaPosition_put_StopTime(This,llTime)	\
    (This)->lpVtbl -> put_StopTime(This,llTime)

#define IMediaPosition_get_PrerollTime(This,pllTime)	\
    (This)->lpVtbl -> get_PrerollTime(This,pllTime)

#define IMediaPosition_put_PrerollTime(This,llTime)	\
    (This)->lpVtbl -> put_PrerollTime(This,llTime)

#define IMediaPosition_put_Rate(This,dRate)	\
    (This)->lpVtbl -> put_Rate(This,dRate)

#define IMediaPosition_get_Rate(This,pdRate)	\
    (This)->lpVtbl -> get_Rate(This,pdRate)

#define IMediaPosition_CanSeekForward(This,pCanSeekForward)	\
    (This)->lpVtbl -> CanSeekForward(This,pCanSeekForward)

#define IMediaPosition_CanSeekBackward(This,pCanSeekBackward)	\
    (This)->lpVtbl -> CanSeekBackward(This,pCanSeekBackward)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_Duration_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *plength);


void __RPC_STUB IMediaPosition_get_Duration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_CurrentPosition_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_CurrentPosition_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_CurrentPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_StopTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_StopTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_StopTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_PrerollTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pllTime);


void __RPC_STUB IMediaPosition_get_PrerollTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_PrerollTime_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ REFTIME llTime);


void __RPC_STUB IMediaPosition_put_PrerollTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IMediaPosition_put_Rate_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [in] */ double dRate);


void __RPC_STUB IMediaPosition_put_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaPosition_get_Rate_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pdRate);


void __RPC_STUB IMediaPosition_get_Rate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPosition_CanSeekForward_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pCanSeekForward);


void __RPC_STUB IMediaPosition_CanSeekForward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaPosition_CanSeekBackward_Proxy( 
    IMediaPosition __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pCanSeekBackward);


void __RPC_STUB IMediaPosition_CanSeekBackward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaPosition_INTERFACE_DEFINED__ */


#ifndef __IBasicAudio_INTERFACE_DEFINED__
#define __IBasicAudio_INTERFACE_DEFINED__

/* interface IBasicAudio */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicAudio,0x56a868b3,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b3-0ad4-11ce-b03a-0020af0ba770")
    IBasicAudio : public IDispatch
    {
    public:
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Volume( 
            /* [in] */ long lVolume) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Volume( 
            /* [retval][out] */ long __RPC_FAR *plVolume) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Balance( 
            /* [in] */ long lBalance) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Balance( 
            /* [retval][out] */ long __RPC_FAR *plBalance) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicAudioVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicAudio __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicAudio __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicAudio __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Volume )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ long lVolume);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Volume )( 
            IBasicAudio __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVolume);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Balance )( 
            IBasicAudio __RPC_FAR * This,
            /* [in] */ long lBalance);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Balance )( 
            IBasicAudio __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBalance);
        
        END_INTERFACE
    } IBasicAudioVtbl;

    interface IBasicAudio
    {
        CONST_VTBL struct IBasicAudioVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicAudio_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicAudio_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicAudio_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicAudio_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicAudio_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicAudio_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicAudio_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicAudio_put_Volume(This,lVolume)	\
    (This)->lpVtbl -> put_Volume(This,lVolume)

#define IBasicAudio_get_Volume(This,plVolume)	\
    (This)->lpVtbl -> get_Volume(This,plVolume)

#define IBasicAudio_put_Balance(This,lBalance)	\
    (This)->lpVtbl -> put_Balance(This,lBalance)

#define IBasicAudio_get_Balance(This,plBalance)	\
    (This)->lpVtbl -> get_Balance(This,plBalance)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicAudio_put_Volume_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [in] */ long lVolume);


void __RPC_STUB IBasicAudio_put_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicAudio_get_Volume_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVolume);


void __RPC_STUB IBasicAudio_get_Volume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicAudio_put_Balance_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [in] */ long lBalance);


void __RPC_STUB IBasicAudio_put_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicAudio_get_Balance_Proxy( 
    IBasicAudio __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBalance);


void __RPC_STUB IBasicAudio_get_Balance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicAudio_INTERFACE_DEFINED__ */


#ifndef __IVideoWindow_INTERFACE_DEFINED__
#define __IVideoWindow_INTERFACE_DEFINED__

/* interface IVideoWindow */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IVideoWindow,0x56a868b4,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b4-0ad4-11ce-b03a-0020af0ba770")
    IVideoWindow : public IDispatch
    {
    public:
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Caption( 
            /* [in] */ BSTR strCaption) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR __RPC_FAR *strCaption) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowStyle( 
            /* [in] */ long WindowStyle) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowStyle( 
            /* [retval][out] */ long __RPC_FAR *WindowStyle) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowStyleEx( 
            /* [in] */ long WindowStyleEx) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowStyleEx( 
            /* [retval][out] */ long __RPC_FAR *WindowStyleEx) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_AutoShow( 
            /* [in] */ long AutoShow) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AutoShow( 
            /* [retval][out] */ long __RPC_FAR *AutoShow) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ long WindowState) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ long __RPC_FAR *WindowState) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BackgroundPalette( 
            /* [in] */ long BackgroundPalette) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BackgroundPalette( 
            /* [retval][out] */ long __RPC_FAR *pBackgroundPalette) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ long Visible) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ long __RPC_FAR *pVisible) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Left( 
            /* [in] */ long Left) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Left( 
            /* [retval][out] */ long __RPC_FAR *pLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ long Width) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long __RPC_FAR *pWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Top( 
            /* [in] */ long Top) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Top( 
            /* [retval][out] */ long __RPC_FAR *pTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ long Height) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Owner( 
            /* [in] */ OAHWND Owner) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Owner( 
            /* [retval][out] */ OAHWND __RPC_FAR *Owner) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_MessageDrain( 
            /* [in] */ OAHWND Drain) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MessageDrain( 
            /* [retval][out] */ OAHWND __RPC_FAR *Drain) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BorderColor( 
            /* [retval][out] */ long __RPC_FAR *Color) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BorderColor( 
            /* [in] */ long Color) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FullScreenMode( 
            /* [retval][out] */ long __RPC_FAR *FullScreenMode) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FullScreenMode( 
            /* [in] */ long FullScreenMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWindowForeground( 
            /* [in] */ long Focus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyOwnerMessage( 
            /* [in] */ OAHWND hwnd,
            /* [in] */ long uMsg,
            /* [in] */ long wParam,
            /* [in] */ long lParam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWindowPosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWindowPosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMinIdealImageSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxIdealImageSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRestorePosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideCursor( 
            /* [in] */ long HideCursor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsCursorHidden( 
            /* [out] */ long __RPC_FAR *CursorHidden) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVideoWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IVideoWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IVideoWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Caption )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ BSTR strCaption);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Caption )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strCaption);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowStyle )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowStyle);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowStyle )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowStyle);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowStyleEx )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowStyleEx);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowStyleEx )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowStyleEx);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoShow )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long AutoShow);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoShow )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *AutoShow);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowState )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long WindowState);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowState )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *WindowState);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BackgroundPalette )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long BackgroundPalette);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BackgroundPalette )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBackgroundPalette);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Visible )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Visible);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Visible )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVisible);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Left);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Width);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Top);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Height);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Owner )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND Owner);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Owner )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ OAHWND __RPC_FAR *Owner);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MessageDrain )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND Drain);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MessageDrain )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ OAHWND __RPC_FAR *Drain);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderColor )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Color);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderColor )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Color);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullScreenMode )( 
            IVideoWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *FullScreenMode);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FullScreenMode )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long FullScreenMode);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetWindowForeground )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Focus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NotifyOwnerMessage )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ OAHWND hwnd,
            /* [in] */ long uMsg,
            /* [in] */ long wParam,
            /* [in] */ long lParam);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetWindowPosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWindowPosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMinIdealImageSize )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMaxIdealImageSize )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRestorePosition )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideCursor )( 
            IVideoWindow __RPC_FAR * This,
            /* [in] */ long HideCursor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCursorHidden )( 
            IVideoWindow __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *CursorHidden);
        
        END_INTERFACE
    } IVideoWindowVtbl;

    interface IVideoWindow
    {
        CONST_VTBL struct IVideoWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVideoWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVideoWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVideoWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVideoWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVideoWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVideoWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVideoWindow_put_Caption(This,strCaption)	\
    (This)->lpVtbl -> put_Caption(This,strCaption)

#define IVideoWindow_get_Caption(This,strCaption)	\
    (This)->lpVtbl -> get_Caption(This,strCaption)

#define IVideoWindow_put_WindowStyle(This,WindowStyle)	\
    (This)->lpVtbl -> put_WindowStyle(This,WindowStyle)

#define IVideoWindow_get_WindowStyle(This,WindowStyle)	\
    (This)->lpVtbl -> get_WindowStyle(This,WindowStyle)

#define IVideoWindow_put_WindowStyleEx(This,WindowStyleEx)	\
    (This)->lpVtbl -> put_WindowStyleEx(This,WindowStyleEx)

#define IVideoWindow_get_WindowStyleEx(This,WindowStyleEx)	\
    (This)->lpVtbl -> get_WindowStyleEx(This,WindowStyleEx)

#define IVideoWindow_put_AutoShow(This,AutoShow)	\
    (This)->lpVtbl -> put_AutoShow(This,AutoShow)

#define IVideoWindow_get_AutoShow(This,AutoShow)	\
    (This)->lpVtbl -> get_AutoShow(This,AutoShow)

#define IVideoWindow_put_WindowState(This,WindowState)	\
    (This)->lpVtbl -> put_WindowState(This,WindowState)

#define IVideoWindow_get_WindowState(This,WindowState)	\
    (This)->lpVtbl -> get_WindowState(This,WindowState)

#define IVideoWindow_put_BackgroundPalette(This,BackgroundPalette)	\
    (This)->lpVtbl -> put_BackgroundPalette(This,BackgroundPalette)

#define IVideoWindow_get_BackgroundPalette(This,pBackgroundPalette)	\
    (This)->lpVtbl -> get_BackgroundPalette(This,pBackgroundPalette)

#define IVideoWindow_put_Visible(This,Visible)	\
    (This)->lpVtbl -> put_Visible(This,Visible)

#define IVideoWindow_get_Visible(This,pVisible)	\
    (This)->lpVtbl -> get_Visible(This,pVisible)

#define IVideoWindow_put_Left(This,Left)	\
    (This)->lpVtbl -> put_Left(This,Left)

#define IVideoWindow_get_Left(This,pLeft)	\
    (This)->lpVtbl -> get_Left(This,pLeft)

#define IVideoWindow_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IVideoWindow_get_Width(This,pWidth)	\
    (This)->lpVtbl -> get_Width(This,pWidth)

#define IVideoWindow_put_Top(This,Top)	\
    (This)->lpVtbl -> put_Top(This,Top)

#define IVideoWindow_get_Top(This,pTop)	\
    (This)->lpVtbl -> get_Top(This,pTop)

#define IVideoWindow_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IVideoWindow_get_Height(This,pHeight)	\
    (This)->lpVtbl -> get_Height(This,pHeight)

#define IVideoWindow_put_Owner(This,Owner)	\
    (This)->lpVtbl -> put_Owner(This,Owner)

#define IVideoWindow_get_Owner(This,Owner)	\
    (This)->lpVtbl -> get_Owner(This,Owner)

#define IVideoWindow_put_MessageDrain(This,Drain)	\
    (This)->lpVtbl -> put_MessageDrain(This,Drain)

#define IVideoWindow_get_MessageDrain(This,Drain)	\
    (This)->lpVtbl -> get_MessageDrain(This,Drain)

#define IVideoWindow_get_BorderColor(This,Color)	\
    (This)->lpVtbl -> get_BorderColor(This,Color)

#define IVideoWindow_put_BorderColor(This,Color)	\
    (This)->lpVtbl -> put_BorderColor(This,Color)

#define IVideoWindow_get_FullScreenMode(This,FullScreenMode)	\
    (This)->lpVtbl -> get_FullScreenMode(This,FullScreenMode)

#define IVideoWindow_put_FullScreenMode(This,FullScreenMode)	\
    (This)->lpVtbl -> put_FullScreenMode(This,FullScreenMode)

#define IVideoWindow_SetWindowForeground(This,Focus)	\
    (This)->lpVtbl -> SetWindowForeground(This,Focus)

#define IVideoWindow_NotifyOwnerMessage(This,hwnd,uMsg,wParam,lParam)	\
    (This)->lpVtbl -> NotifyOwnerMessage(This,hwnd,uMsg,wParam,lParam)

#define IVideoWindow_SetWindowPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetWindowPosition(This,Left,Top,Width,Height)

#define IVideoWindow_GetWindowPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetWindowPosition(This,pLeft,pTop,pWidth,pHeight)

#define IVideoWindow_GetMinIdealImageSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetMinIdealImageSize(This,pWidth,pHeight)

#define IVideoWindow_GetMaxIdealImageSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetMaxIdealImageSize(This,pWidth,pHeight)

#define IVideoWindow_GetRestorePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetRestorePosition(This,pLeft,pTop,pWidth,pHeight)

#define IVideoWindow_HideCursor(This,HideCursor)	\
    (This)->lpVtbl -> HideCursor(This,HideCursor)

#define IVideoWindow_IsCursorHidden(This,CursorHidden)	\
    (This)->lpVtbl -> IsCursorHidden(This,CursorHidden)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Caption_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ BSTR strCaption);


void __RPC_STUB IVideoWindow_put_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Caption_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strCaption);


void __RPC_STUB IVideoWindow_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowStyle_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowStyle);


void __RPC_STUB IVideoWindow_put_WindowStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowStyle_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowStyle);


void __RPC_STUB IVideoWindow_get_WindowStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowStyleEx_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowStyleEx);


void __RPC_STUB IVideoWindow_put_WindowStyleEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowStyleEx_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowStyleEx);


void __RPC_STUB IVideoWindow_get_WindowStyleEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_AutoShow_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long AutoShow);


void __RPC_STUB IVideoWindow_put_AutoShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_AutoShow_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *AutoShow);


void __RPC_STUB IVideoWindow_get_AutoShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_WindowState_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long WindowState);


void __RPC_STUB IVideoWindow_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_WindowState_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *WindowState);


void __RPC_STUB IVideoWindow_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_BackgroundPalette_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long BackgroundPalette);


void __RPC_STUB IVideoWindow_put_BackgroundPalette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_BackgroundPalette_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBackgroundPalette);


void __RPC_STUB IVideoWindow_get_BackgroundPalette_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Visible_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Visible);


void __RPC_STUB IVideoWindow_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Visible_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVisible);


void __RPC_STUB IVideoWindow_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Left_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Left);


void __RPC_STUB IVideoWindow_put_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Left_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLeft);


void __RPC_STUB IVideoWindow_get_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Width_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Width);


void __RPC_STUB IVideoWindow_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Width_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pWidth);


void __RPC_STUB IVideoWindow_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Top_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Top);


void __RPC_STUB IVideoWindow_put_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Top_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pTop);


void __RPC_STUB IVideoWindow_get_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Height_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Height);


void __RPC_STUB IVideoWindow_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Height_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_Owner_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND Owner);


void __RPC_STUB IVideoWindow_put_Owner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_Owner_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ OAHWND __RPC_FAR *Owner);


void __RPC_STUB IVideoWindow_get_Owner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_MessageDrain_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND Drain);


void __RPC_STUB IVideoWindow_put_MessageDrain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_MessageDrain_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ OAHWND __RPC_FAR *Drain);


void __RPC_STUB IVideoWindow_get_MessageDrain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_BorderColor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Color);


void __RPC_STUB IVideoWindow_get_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_BorderColor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Color);


void __RPC_STUB IVideoWindow_put_BorderColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVideoWindow_get_FullScreenMode_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *FullScreenMode);


void __RPC_STUB IVideoWindow_get_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVideoWindow_put_FullScreenMode_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long FullScreenMode);


void __RPC_STUB IVideoWindow_put_FullScreenMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_SetWindowForeground_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Focus);


void __RPC_STUB IVideoWindow_SetWindowForeground_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_NotifyOwnerMessage_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ OAHWND hwnd,
    /* [in] */ long uMsg,
    /* [in] */ long wParam,
    /* [in] */ long lParam);


void __RPC_STUB IVideoWindow_NotifyOwnerMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_SetWindowPosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IVideoWindow_SetWindowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetWindowPosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetWindowPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetMinIdealImageSize_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetMinIdealImageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetMaxIdealImageSize_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetMaxIdealImageSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_GetRestorePosition_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IVideoWindow_GetRestorePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_HideCursor_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [in] */ long HideCursor);


void __RPC_STUB IVideoWindow_HideCursor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVideoWindow_IsCursorHidden_Proxy( 
    IVideoWindow __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *CursorHidden);


void __RPC_STUB IVideoWindow_IsCursorHidden_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVideoWindow_INTERFACE_DEFINED__ */


#ifndef __IBasicVideo_INTERFACE_DEFINED__
#define __IBasicVideo_INTERFACE_DEFINED__

/* interface IBasicVideo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicVideo,0x56a868b5,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b5-0ad4-11ce-b03a-0020af0ba770")
    IBasicVideo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AvgTimePerFrame( 
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BitRate( 
            /* [retval][out] */ long __RPC_FAR *pBitRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BitErrorRate( 
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VideoWidth( 
            /* [retval][out] */ long __RPC_FAR *pVideoWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VideoHeight( 
            /* [retval][out] */ long __RPC_FAR *pVideoHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceLeft( 
            /* [in] */ long SourceLeft) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceLeft( 
            /* [retval][out] */ long __RPC_FAR *pSourceLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceWidth( 
            /* [in] */ long SourceWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceWidth( 
            /* [retval][out] */ long __RPC_FAR *pSourceWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceTop( 
            /* [in] */ long SourceTop) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceTop( 
            /* [retval][out] */ long __RPC_FAR *pSourceTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SourceHeight( 
            /* [in] */ long SourceHeight) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SourceHeight( 
            /* [retval][out] */ long __RPC_FAR *pSourceHeight) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationLeft( 
            /* [in] */ long DestinationLeft) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationLeft( 
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationWidth( 
            /* [in] */ long DestinationWidth) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationWidth( 
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationTop( 
            /* [in] */ long DestinationTop) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationTop( 
            /* [retval][out] */ long __RPC_FAR *pDestinationTop) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DestinationHeight( 
            /* [in] */ long DestinationHeight) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DestinationHeight( 
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSourcePosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourcePosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultSourcePosition( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDestinationPosition( 
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDestinationPosition( 
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultDestinationPosition( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoSize( 
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVideoPaletteEntries( 
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentImage( 
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUsingDefaultSource( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUsingDefaultDestination( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicVideoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicVideo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AvgTimePerFrame )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitErrorRate )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long SourceHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationLeft )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationWidth )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationTop )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long DestinationHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationHeight )( 
            IBasicVideo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSourcePosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSourcePosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultSourcePosition )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDestinationPosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDestinationPosition )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultDestinationPosition )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoSize )( 
            IBasicVideo __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoPaletteEntries )( 
            IBasicVideo __RPC_FAR * This,
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentImage )( 
            IBasicVideo __RPC_FAR * This,
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultSource )( 
            IBasicVideo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultDestination )( 
            IBasicVideo __RPC_FAR * This);
        
        END_INTERFACE
    } IBasicVideoVtbl;

    interface IBasicVideo
    {
        CONST_VTBL struct IBasicVideoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicVideo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicVideo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicVideo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicVideo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicVideo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicVideo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicVideo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicVideo_get_AvgTimePerFrame(This,pAvgTimePerFrame)	\
    (This)->lpVtbl -> get_AvgTimePerFrame(This,pAvgTimePerFrame)

#define IBasicVideo_get_BitRate(This,pBitRate)	\
    (This)->lpVtbl -> get_BitRate(This,pBitRate)

#define IBasicVideo_get_BitErrorRate(This,pBitErrorRate)	\
    (This)->lpVtbl -> get_BitErrorRate(This,pBitErrorRate)

#define IBasicVideo_get_VideoWidth(This,pVideoWidth)	\
    (This)->lpVtbl -> get_VideoWidth(This,pVideoWidth)

#define IBasicVideo_get_VideoHeight(This,pVideoHeight)	\
    (This)->lpVtbl -> get_VideoHeight(This,pVideoHeight)

#define IBasicVideo_put_SourceLeft(This,SourceLeft)	\
    (This)->lpVtbl -> put_SourceLeft(This,SourceLeft)

#define IBasicVideo_get_SourceLeft(This,pSourceLeft)	\
    (This)->lpVtbl -> get_SourceLeft(This,pSourceLeft)

#define IBasicVideo_put_SourceWidth(This,SourceWidth)	\
    (This)->lpVtbl -> put_SourceWidth(This,SourceWidth)

#define IBasicVideo_get_SourceWidth(This,pSourceWidth)	\
    (This)->lpVtbl -> get_SourceWidth(This,pSourceWidth)

#define IBasicVideo_put_SourceTop(This,SourceTop)	\
    (This)->lpVtbl -> put_SourceTop(This,SourceTop)

#define IBasicVideo_get_SourceTop(This,pSourceTop)	\
    (This)->lpVtbl -> get_SourceTop(This,pSourceTop)

#define IBasicVideo_put_SourceHeight(This,SourceHeight)	\
    (This)->lpVtbl -> put_SourceHeight(This,SourceHeight)

#define IBasicVideo_get_SourceHeight(This,pSourceHeight)	\
    (This)->lpVtbl -> get_SourceHeight(This,pSourceHeight)

#define IBasicVideo_put_DestinationLeft(This,DestinationLeft)	\
    (This)->lpVtbl -> put_DestinationLeft(This,DestinationLeft)

#define IBasicVideo_get_DestinationLeft(This,pDestinationLeft)	\
    (This)->lpVtbl -> get_DestinationLeft(This,pDestinationLeft)

#define IBasicVideo_put_DestinationWidth(This,DestinationWidth)	\
    (This)->lpVtbl -> put_DestinationWidth(This,DestinationWidth)

#define IBasicVideo_get_DestinationWidth(This,pDestinationWidth)	\
    (This)->lpVtbl -> get_DestinationWidth(This,pDestinationWidth)

#define IBasicVideo_put_DestinationTop(This,DestinationTop)	\
    (This)->lpVtbl -> put_DestinationTop(This,DestinationTop)

#define IBasicVideo_get_DestinationTop(This,pDestinationTop)	\
    (This)->lpVtbl -> get_DestinationTop(This,pDestinationTop)

#define IBasicVideo_put_DestinationHeight(This,DestinationHeight)	\
    (This)->lpVtbl -> put_DestinationHeight(This,DestinationHeight)

#define IBasicVideo_get_DestinationHeight(This,pDestinationHeight)	\
    (This)->lpVtbl -> get_DestinationHeight(This,pDestinationHeight)

#define IBasicVideo_SetSourcePosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetSourcePosition(This,Left,Top,Width,Height)

#define IBasicVideo_GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo_SetDefaultSourcePosition(This)	\
    (This)->lpVtbl -> SetDefaultSourcePosition(This)

#define IBasicVideo_SetDestinationPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetDestinationPosition(This,Left,Top,Width,Height)

#define IBasicVideo_GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo_SetDefaultDestinationPosition(This)	\
    (This)->lpVtbl -> SetDefaultDestinationPosition(This)

#define IBasicVideo_GetVideoSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetVideoSize(This,pWidth,pHeight)

#define IBasicVideo_GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)	\
    (This)->lpVtbl -> GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)

#define IBasicVideo_GetCurrentImage(This,pBufferSize,pDIBImage)	\
    (This)->lpVtbl -> GetCurrentImage(This,pBufferSize,pDIBImage)

#define IBasicVideo_IsUsingDefaultSource(This)	\
    (This)->lpVtbl -> IsUsingDefaultSource(This)

#define IBasicVideo_IsUsingDefaultDestination(This)	\
    (This)->lpVtbl -> IsUsingDefaultDestination(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_AvgTimePerFrame_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);


void __RPC_STUB IBasicVideo_get_AvgTimePerFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_BitRate_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBitRate);


void __RPC_STUB IBasicVideo_get_BitRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_BitErrorRate_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pBitErrorRate);


void __RPC_STUB IBasicVideo_get_BitErrorRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_VideoWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVideoWidth);


void __RPC_STUB IBasicVideo_get_VideoWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_VideoHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVideoHeight);


void __RPC_STUB IBasicVideo_get_VideoHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceLeft);


void __RPC_STUB IBasicVideo_put_SourceLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceLeft);


void __RPC_STUB IBasicVideo_get_SourceLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceWidth);


void __RPC_STUB IBasicVideo_put_SourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceWidth);


void __RPC_STUB IBasicVideo_get_SourceWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceTop);


void __RPC_STUB IBasicVideo_put_SourceTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceTop);


void __RPC_STUB IBasicVideo_get_SourceTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_SourceHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long SourceHeight);


void __RPC_STUB IBasicVideo_put_SourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_SourceHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pSourceHeight);


void __RPC_STUB IBasicVideo_get_SourceHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationLeft);


void __RPC_STUB IBasicVideo_put_DestinationLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationLeft_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationLeft);


void __RPC_STUB IBasicVideo_get_DestinationLeft_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationWidth);


void __RPC_STUB IBasicVideo_put_DestinationWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationWidth_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationWidth);


void __RPC_STUB IBasicVideo_get_DestinationWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationTop);


void __RPC_STUB IBasicVideo_put_DestinationTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationTop_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationTop);


void __RPC_STUB IBasicVideo_get_DestinationTop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IBasicVideo_put_DestinationHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long DestinationHeight);


void __RPC_STUB IBasicVideo_put_DestinationHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IBasicVideo_get_DestinationHeight_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pDestinationHeight);


void __RPC_STUB IBasicVideo_get_DestinationHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IBasicVideo_SetSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDefaultSourcePosition_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_SetDefaultSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long Left,
    /* [in] */ long Top,
    /* [in] */ long Width,
    /* [in] */ long Height);


void __RPC_STUB IBasicVideo_SetDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pLeft,
    /* [out] */ long __RPC_FAR *pTop,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_SetDefaultDestinationPosition_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_SetDefaultDestinationPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetVideoSize_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *pWidth,
    /* [out] */ long __RPC_FAR *pHeight);


void __RPC_STUB IBasicVideo_GetVideoSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetVideoPaletteEntries_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [in] */ long StartIndex,
    /* [in] */ long Entries,
    /* [out] */ long __RPC_FAR *pRetrieved,
    /* [out] */ long __RPC_FAR *pPalette);


void __RPC_STUB IBasicVideo_GetVideoPaletteEntries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_GetCurrentImage_Proxy( 
    IBasicVideo __RPC_FAR * This,
    /* [out][in] */ long __RPC_FAR *pBufferSize,
    /* [out] */ long __RPC_FAR *pDIBImage);


void __RPC_STUB IBasicVideo_GetCurrentImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_IsUsingDefaultSource_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_IsUsingDefaultSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBasicVideo_IsUsingDefaultDestination_Proxy( 
    IBasicVideo __RPC_FAR * This);


void __RPC_STUB IBasicVideo_IsUsingDefaultDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicVideo_INTERFACE_DEFINED__ */


#ifndef __IBasicVideo2_INTERFACE_DEFINED__
#define __IBasicVideo2_INTERFACE_DEFINED__

/* interface IBasicVideo2 */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IBasicVideo2,0x329bb360,0xf6ea,0x11d1,0x90,0x38,0x00,0xa0,0xc9,0x69,0x72,0x98);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("329bb360-f6ea-11d1-9038-00a0c9697298")
    IBasicVideo2 : public IBasicVideo
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPreferredAspectRatio( 
            /* [out] */ long __RPC_FAR *plAspectX,
            /* [out] */ long __RPC_FAR *plAspectY) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBasicVideo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBasicVideo2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AvgTimePerFrame )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ REFTIME __RPC_FAR *pAvgTimePerFrame);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitRate )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BitErrorRate )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pBitErrorRate);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VideoHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVideoHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long SourceHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pSourceHeight);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationLeft);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationLeft )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationLeft);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationWidth);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationWidth )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationWidth);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationTop);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationTop )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationTop);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long DestinationHeight);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationHeight )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pDestinationHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultSourcePosition )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long Left,
            /* [in] */ long Top,
            /* [in] */ long Width,
            /* [in] */ long Height);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pLeft,
            /* [out] */ long __RPC_FAR *pTop,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultDestinationPosition )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoSize )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *pWidth,
            /* [out] */ long __RPC_FAR *pHeight);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVideoPaletteEntries )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [in] */ long StartIndex,
            /* [in] */ long Entries,
            /* [out] */ long __RPC_FAR *pRetrieved,
            /* [out] */ long __RPC_FAR *pPalette);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentImage )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out][in] */ long __RPC_FAR *pBufferSize,
            /* [out] */ long __RPC_FAR *pDIBImage);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultSource )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUsingDefaultDestination )( 
            IBasicVideo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPreferredAspectRatio )( 
            IBasicVideo2 __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *plAspectX,
            /* [out] */ long __RPC_FAR *plAspectY);
        
        END_INTERFACE
    } IBasicVideo2Vtbl;

    interface IBasicVideo2
    {
        CONST_VTBL struct IBasicVideo2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBasicVideo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBasicVideo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBasicVideo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBasicVideo2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBasicVideo2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBasicVideo2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBasicVideo2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IBasicVideo2_get_AvgTimePerFrame(This,pAvgTimePerFrame)	\
    (This)->lpVtbl -> get_AvgTimePerFrame(This,pAvgTimePerFrame)

#define IBasicVideo2_get_BitRate(This,pBitRate)	\
    (This)->lpVtbl -> get_BitRate(This,pBitRate)

#define IBasicVideo2_get_BitErrorRate(This,pBitErrorRate)	\
    (This)->lpVtbl -> get_BitErrorRate(This,pBitErrorRate)

#define IBasicVideo2_get_VideoWidth(This,pVideoWidth)	\
    (This)->lpVtbl -> get_VideoWidth(This,pVideoWidth)

#define IBasicVideo2_get_VideoHeight(This,pVideoHeight)	\
    (This)->lpVtbl -> get_VideoHeight(This,pVideoHeight)

#define IBasicVideo2_put_SourceLeft(This,SourceLeft)	\
    (This)->lpVtbl -> put_SourceLeft(This,SourceLeft)

#define IBasicVideo2_get_SourceLeft(This,pSourceLeft)	\
    (This)->lpVtbl -> get_SourceLeft(This,pSourceLeft)

#define IBasicVideo2_put_SourceWidth(This,SourceWidth)	\
    (This)->lpVtbl -> put_SourceWidth(This,SourceWidth)

#define IBasicVideo2_get_SourceWidth(This,pSourceWidth)	\
    (This)->lpVtbl -> get_SourceWidth(This,pSourceWidth)

#define IBasicVideo2_put_SourceTop(This,SourceTop)	\
    (This)->lpVtbl -> put_SourceTop(This,SourceTop)

#define IBasicVideo2_get_SourceTop(This,pSourceTop)	\
    (This)->lpVtbl -> get_SourceTop(This,pSourceTop)

#define IBasicVideo2_put_SourceHeight(This,SourceHeight)	\
    (This)->lpVtbl -> put_SourceHeight(This,SourceHeight)

#define IBasicVideo2_get_SourceHeight(This,pSourceHeight)	\
    (This)->lpVtbl -> get_SourceHeight(This,pSourceHeight)

#define IBasicVideo2_put_DestinationLeft(This,DestinationLeft)	\
    (This)->lpVtbl -> put_DestinationLeft(This,DestinationLeft)

#define IBasicVideo2_get_DestinationLeft(This,pDestinationLeft)	\
    (This)->lpVtbl -> get_DestinationLeft(This,pDestinationLeft)

#define IBasicVideo2_put_DestinationWidth(This,DestinationWidth)	\
    (This)->lpVtbl -> put_DestinationWidth(This,DestinationWidth)

#define IBasicVideo2_get_DestinationWidth(This,pDestinationWidth)	\
    (This)->lpVtbl -> get_DestinationWidth(This,pDestinationWidth)

#define IBasicVideo2_put_DestinationTop(This,DestinationTop)	\
    (This)->lpVtbl -> put_DestinationTop(This,DestinationTop)

#define IBasicVideo2_get_DestinationTop(This,pDestinationTop)	\
    (This)->lpVtbl -> get_DestinationTop(This,pDestinationTop)

#define IBasicVideo2_put_DestinationHeight(This,DestinationHeight)	\
    (This)->lpVtbl -> put_DestinationHeight(This,DestinationHeight)

#define IBasicVideo2_get_DestinationHeight(This,pDestinationHeight)	\
    (This)->lpVtbl -> get_DestinationHeight(This,pDestinationHeight)

#define IBasicVideo2_SetSourcePosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetSourcePosition(This,Left,Top,Width,Height)

#define IBasicVideo2_GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetSourcePosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo2_SetDefaultSourcePosition(This)	\
    (This)->lpVtbl -> SetDefaultSourcePosition(This)

#define IBasicVideo2_SetDestinationPosition(This,Left,Top,Width,Height)	\
    (This)->lpVtbl -> SetDestinationPosition(This,Left,Top,Width,Height)

#define IBasicVideo2_GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)	\
    (This)->lpVtbl -> GetDestinationPosition(This,pLeft,pTop,pWidth,pHeight)

#define IBasicVideo2_SetDefaultDestinationPosition(This)	\
    (This)->lpVtbl -> SetDefaultDestinationPosition(This)

#define IBasicVideo2_GetVideoSize(This,pWidth,pHeight)	\
    (This)->lpVtbl -> GetVideoSize(This,pWidth,pHeight)

#define IBasicVideo2_GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)	\
    (This)->lpVtbl -> GetVideoPaletteEntries(This,StartIndex,Entries,pRetrieved,pPalette)

#define IBasicVideo2_GetCurrentImage(This,pBufferSize,pDIBImage)	\
    (This)->lpVtbl -> GetCurrentImage(This,pBufferSize,pDIBImage)

#define IBasicVideo2_IsUsingDefaultSource(This)	\
    (This)->lpVtbl -> IsUsingDefaultSource(This)

#define IBasicVideo2_IsUsingDefaultDestination(This)	\
    (This)->lpVtbl -> IsUsingDefaultDestination(This)


#define IBasicVideo2_GetPreferredAspectRatio(This,plAspectX,plAspectY)	\
    (This)->lpVtbl -> GetPreferredAspectRatio(This,plAspectX,plAspectY)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBasicVideo2_GetPreferredAspectRatio_Proxy( 
    IBasicVideo2 __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *plAspectX,
    /* [out] */ long __RPC_FAR *plAspectY);


void __RPC_STUB IBasicVideo2_GetPreferredAspectRatio_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBasicVideo2_INTERFACE_DEFINED__ */


#ifndef __IDeferredCommand_INTERFACE_DEFINED__
#define __IDeferredCommand_INTERFACE_DEFINED__

/* interface IDeferredCommand */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IDeferredCommand,0x56a868b8,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b8-0ad4-11ce-b03a-0020af0ba770")
    IDeferredCommand : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Confidence( 
            /* [out] */ LONG __RPC_FAR *pConfidence) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Postpone( 
            /* [in] */ REFTIME newtime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHResult( 
            /* [out] */ HRESULT __RPC_FAR *phrResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeferredCommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDeferredCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDeferredCommand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDeferredCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cancel )( 
            IDeferredCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Confidence )( 
            IDeferredCommand __RPC_FAR * This,
            /* [out] */ LONG __RPC_FAR *pConfidence);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Postpone )( 
            IDeferredCommand __RPC_FAR * This,
            /* [in] */ REFTIME newtime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHResult )( 
            IDeferredCommand __RPC_FAR * This,
            /* [out] */ HRESULT __RPC_FAR *phrResult);
        
        END_INTERFACE
    } IDeferredCommandVtbl;

    interface IDeferredCommand
    {
        CONST_VTBL struct IDeferredCommandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeferredCommand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeferredCommand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeferredCommand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeferredCommand_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define IDeferredCommand_Confidence(This,pConfidence)	\
    (This)->lpVtbl -> Confidence(This,pConfidence)

#define IDeferredCommand_Postpone(This,newtime)	\
    (This)->lpVtbl -> Postpone(This,newtime)

#define IDeferredCommand_GetHResult(This,phrResult)	\
    (This)->lpVtbl -> GetHResult(This,phrResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDeferredCommand_Cancel_Proxy( 
    IDeferredCommand __RPC_FAR * This);


void __RPC_STUB IDeferredCommand_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_Confidence_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [out] */ LONG __RPC_FAR *pConfidence);


void __RPC_STUB IDeferredCommand_Confidence_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_Postpone_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [in] */ REFTIME newtime);


void __RPC_STUB IDeferredCommand_Postpone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeferredCommand_GetHResult_Proxy( 
    IDeferredCommand __RPC_FAR * This,
    /* [out] */ HRESULT __RPC_FAR *phrResult);


void __RPC_STUB IDeferredCommand_GetHResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeferredCommand_INTERFACE_DEFINED__ */


#ifndef __IQueueCommand_INTERFACE_DEFINED__
#define __IQueueCommand_INTERFACE_DEFINED__

/* interface IQueueCommand */
/* [object][helpstring][uuid] */ 


DEFINE_GUID(IID_IQueueCommand,0x56a868b7,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868b7-0ad4-11ce-b03a-0020af0ba770")
    IQueueCommand : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InvokeAtStreamTime( 
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InvokeAtPresentationTime( 
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueueCommandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQueueCommand __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQueueCommand __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQueueCommand __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvokeAtStreamTime )( 
            IQueueCommand __RPC_FAR * This,
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvokeAtPresentationTime )( 
            IQueueCommand __RPC_FAR * This,
            /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
            /* [in] */ REFTIME time,
            /* [in] */ GUID __RPC_FAR *iid,
            /* [in] */ long dispidMethod,
            /* [in] */ short wFlags,
            /* [in] */ long cArgs,
            /* [in] */ VARIANT __RPC_FAR *pDispParams,
            /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
            /* [out] */ short __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IQueueCommandVtbl;

    interface IQueueCommand
    {
        CONST_VTBL struct IQueueCommandVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueueCommand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueueCommand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueueCommand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueueCommand_InvokeAtStreamTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)	\
    (This)->lpVtbl -> InvokeAtStreamTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)

#define IQueueCommand_InvokeAtPresentationTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)	\
    (This)->lpVtbl -> InvokeAtPresentationTime(This,pCmd,time,iid,dispidMethod,wFlags,cArgs,pDispParams,pvarResult,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQueueCommand_InvokeAtStreamTime_Proxy( 
    IQueueCommand __RPC_FAR * This,
    /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
    /* [in] */ REFTIME time,
    /* [in] */ GUID __RPC_FAR *iid,
    /* [in] */ long dispidMethod,
    /* [in] */ short wFlags,
    /* [in] */ long cArgs,
    /* [in] */ VARIANT __RPC_FAR *pDispParams,
    /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
    /* [out] */ short __RPC_FAR *puArgErr);


void __RPC_STUB IQueueCommand_InvokeAtStreamTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IQueueCommand_InvokeAtPresentationTime_Proxy( 
    IQueueCommand __RPC_FAR * This,
    /* [out] */ IDeferredCommand __RPC_FAR *__RPC_FAR *pCmd,
    /* [in] */ REFTIME time,
    /* [in] */ GUID __RPC_FAR *iid,
    /* [in] */ long dispidMethod,
    /* [in] */ short wFlags,
    /* [in] */ long cArgs,
    /* [in] */ VARIANT __RPC_FAR *pDispParams,
    /* [out][in] */ VARIANT __RPC_FAR *pvarResult,
    /* [out] */ short __RPC_FAR *puArgErr);


void __RPC_STUB IQueueCommand_InvokeAtPresentationTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQueueCommand_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_FilgraphManager,0xe436ebb3,0x524f,0x11ce,0x9f,0x53,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#ifdef __cplusplus

class DECLSPEC_UUID("e436ebb3-524f-11ce-9f53-0020af0ba770")
FilgraphManager;
#endif

#ifndef __IFilterInfo_INTERFACE_DEFINED__
#define __IFilterInfo_INTERFACE_DEFINED__

/* interface IFilterInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IFilterInfo,0x56a868ba,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868ba-0ad4-11ce-b03a-0020af0ba770")
    IFilterInfo : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FindPin( 
            /* [in] */ BSTR strPinID,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VendorInfo( 
            /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filter( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Pins( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsFileSource( 
            /* [retval][out] */ LONG __RPC_FAR *pbIsSource) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filename( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrFilename) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Filename( 
            /* [in] */ BSTR strFilename) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFilterInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IFilterInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IFilterInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IFilterInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FindPin )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ BSTR strPinID,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorInfo )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filter )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pins )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsFileSource )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *pbIsSource);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Filename )( 
            IFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrFilename);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Filename )( 
            IFilterInfo __RPC_FAR * This,
            /* [in] */ BSTR strFilename);
        
        END_INTERFACE
    } IFilterInfoVtbl;

    interface IFilterInfo
    {
        CONST_VTBL struct IFilterInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFilterInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFilterInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFilterInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFilterInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFilterInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFilterInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFilterInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFilterInfo_FindPin(This,strPinID,ppUnk)	\
    (This)->lpVtbl -> FindPin(This,strPinID,ppUnk)

#define IFilterInfo_get_Name(This,strName)	\
    (This)->lpVtbl -> get_Name(This,strName)

#define IFilterInfo_get_VendorInfo(This,strVendorInfo)	\
    (This)->lpVtbl -> get_VendorInfo(This,strVendorInfo)

#define IFilterInfo_get_Filter(This,ppUnk)	\
    (This)->lpVtbl -> get_Filter(This,ppUnk)

#define IFilterInfo_get_Pins(This,ppUnk)	\
    (This)->lpVtbl -> get_Pins(This,ppUnk)

#define IFilterInfo_get_IsFileSource(This,pbIsSource)	\
    (This)->lpVtbl -> get_IsFileSource(This,pbIsSource)

#define IFilterInfo_get_Filename(This,pstrFilename)	\
    (This)->lpVtbl -> get_Filename(This,pstrFilename)

#define IFilterInfo_put_Filename(This,strFilename)	\
    (This)->lpVtbl -> put_Filename(This,strFilename)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFilterInfo_FindPin_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [in] */ BSTR strPinID,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_FindPin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Name_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strName);


void __RPC_STUB IFilterInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_VendorInfo_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strVendorInfo);


void __RPC_STUB IFilterInfo_get_VendorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Filter_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_get_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Pins_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IFilterInfo_get_Pins_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_IsFileSource_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *pbIsSource);


void __RPC_STUB IFilterInfo_get_IsFileSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IFilterInfo_get_Filename_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrFilename);


void __RPC_STUB IFilterInfo_get_Filename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IFilterInfo_put_Filename_Proxy( 
    IFilterInfo __RPC_FAR * This,
    /* [in] */ BSTR strFilename);


void __RPC_STUB IFilterInfo_put_Filename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFilterInfo_INTERFACE_DEFINED__ */


#ifndef __IRegFilterInfo_INTERFACE_DEFINED__
#define __IRegFilterInfo_INTERFACE_DEFINED__

/* interface IRegFilterInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IRegFilterInfo,0x56a868bb,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bb-0ad4-11ce-b03a-0020af0ba770")
    IRegFilterInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *strName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Filter( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRegFilterInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRegFilterInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRegFilterInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Filter )( 
            IRegFilterInfo __RPC_FAR * This,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IRegFilterInfoVtbl;

    interface IRegFilterInfo
    {
        CONST_VTBL struct IRegFilterInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegFilterInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRegFilterInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRegFilterInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRegFilterInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRegFilterInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRegFilterInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRegFilterInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRegFilterInfo_get_Name(This,strName)	\
    (This)->lpVtbl -> get_Name(This,strName)

#define IRegFilterInfo_Filter(This,ppUnk)	\
    (This)->lpVtbl -> Filter(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IRegFilterInfo_get_Name_Proxy( 
    IRegFilterInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strName);


void __RPC_STUB IRegFilterInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRegFilterInfo_Filter_Proxy( 
    IRegFilterInfo __RPC_FAR * This,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IRegFilterInfo_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRegFilterInfo_INTERFACE_DEFINED__ */


#ifndef __IMediaTypeInfo_INTERFACE_DEFINED__
#define __IMediaTypeInfo_INTERFACE_DEFINED__

/* interface IMediaTypeInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IMediaTypeInfo,0x56a868bc,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bc-0ad4-11ce-b03a-0020af0ba770")
    IMediaTypeInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Subtype( 
            /* [retval][out] */ BSTR __RPC_FAR *strType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaTypeInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaTypeInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaTypeInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strType);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Subtype )( 
            IMediaTypeInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strType);
        
        END_INTERFACE
    } IMediaTypeInfoVtbl;

    interface IMediaTypeInfo
    {
        CONST_VTBL struct IMediaTypeInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaTypeInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaTypeInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaTypeInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaTypeInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaTypeInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaTypeInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaTypeInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMediaTypeInfo_get_Type(This,strType)	\
    (This)->lpVtbl -> get_Type(This,strType)

#define IMediaTypeInfo_get_Subtype(This,strType)	\
    (This)->lpVtbl -> get_Subtype(This,strType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaTypeInfo_get_Type_Proxy( 
    IMediaTypeInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strType);


void __RPC_STUB IMediaTypeInfo_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IMediaTypeInfo_get_Subtype_Proxy( 
    IMediaTypeInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strType);


void __RPC_STUB IMediaTypeInfo_get_Subtype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaTypeInfo_INTERFACE_DEFINED__ */


#ifndef __IPinInfo_INTERFACE_DEFINED__
#define __IPinInfo_INTERFACE_DEFINED__

/* interface IPinInfo */
/* [object][dual][oleautomation][helpstring][uuid] */ 


DEFINE_GUID(IID_IPinInfo,0x56a868bd,0x0ad4,0x11ce,0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56a868bd-0ad4-11ce-b03a-0020af0ba770")
    IPinInfo : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Pin( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ConnectedTo( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionMediaType( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FilterInfo( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *ppUnk) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Direction( 
            /* [retval][out] */ LONG __RPC_FAR *ppDirection) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PinID( 
            /* [retval][out] */ BSTR __RPC_FAR *strPinID) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MediaTypes( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectDirect( 
            /* [in] */ IUnknown __RPC_FAR *pPin) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConnectWithType( 
            /* [in] */ IUnknown __RPC_FAR *pPin,
            /* [in] */ IDispatch __RPC_FAR *pMediaType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Render( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPinInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPinInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPinInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPinInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pin )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectedTo )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionMediaType )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FilterInfo )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *ppUnk);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Direction )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *ppDirection);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PinID )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *strPinID);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MediaTypes )( 
            IPinInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectDirect )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectWithType )( 
            IPinInfo __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pPin,
            /* [in] */ IDispatch __RPC_FAR *pMediaType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IPinInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Render )( 
            IPinInfo __RPC_FAR * This);
        
        END_INTERFACE
    } IPinInfoVtbl;

    interface IPinInfo
    {
        CONST_VTBL struct IPinInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPinInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPinInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPinInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPinInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPinInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPinInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPinInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPinInfo_get_Pin(This,ppUnk)	\
    (This)->lpVtbl -> get_Pin(This,ppUnk)

#define IPinInfo_get_ConnectedTo(This,ppUnk)	\
    (This)->lpVtbl -> get_ConnectedTo(This,ppUnk)

#define IPinInfo_get_ConnectionMediaType(This,ppUnk)	\
    (This)->lpVtbl -> get_ConnectionMediaType(This,ppUnk)

#define IPinInfo_get_FilterInfo(This,ppUnk)	\
    (This)->lpVtbl -> get_FilterInfo(This,ppUnk)

#define IPinInfo_get_Name(This,ppUnk)	\
    (This)->lpVtbl -> get_Name(This,ppUnk)

#define IPinInfo_get_Direction(This,ppDirection)	\
    (This)->lpVtbl -> get_Direction(This,ppDirection)

#define IPinInfo_get_PinID(This,strPinID)	\
    (This)->lpVtbl -> get_PinID(This,strPinID)

#define IPinInfo_get_MediaTypes(This,ppUnk)	\
    (This)->lpVtbl -> get_MediaTypes(This,ppUnk)

#define IPinInfo_Connect(This,pPin)	\
    (This)->lpVtbl -> Connect(This,pPin)

#define IPinInfo_ConnectDirect(This,pPin)	\
    (This)->lpVtbl -> ConnectDirect(This,pPin)

#define IPinInfo_ConnectWithType(This,pPin,pMediaType)	\
    (This)->lpVtbl -> ConnectWithType(This,pPin,pMediaType)

#define IPinInfo_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IPinInfo_Render(This)	\
    (This)->lpVtbl -> Render(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Pin_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_Pin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_ConnectedTo_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_ConnectedTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_ConnectionMediaType_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_ConnectionMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_FilterInfo_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_FilterInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Name_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_Direction_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *ppDirection);


void __RPC_STUB IPinInfo_get_Direction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_PinID_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *strPinID);


void __RPC_STUB IPinInfo_get_PinID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IPinInfo_get_MediaTypes_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPinInfo_get_MediaTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Connect_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin);


void __RPC_STUB IPinInfo_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_ConnectDirect_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin);


void __RPC_STUB IPinInfo_ConnectDirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_ConnectWithType_Proxy( 
    IPinInfo __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pPin,
    /* [in] */ IDispatch __RPC_FAR *pMediaType);


void __RPC_STUB IPinInfo_ConnectWithType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Disconnect_Proxy( 
    IPinInfo __RPC_FAR * This);


void __RPC_STUB IPinInfo_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPinInfo_Render_Proxy( 
    IPinInfo __RPC_FAR * This);


void __RPC_STUB IPinInfo_Render_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPinInfo_INTERFACE_DEFINED__ */

#endif /* __QuartzTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


#endif // _WIN64 
