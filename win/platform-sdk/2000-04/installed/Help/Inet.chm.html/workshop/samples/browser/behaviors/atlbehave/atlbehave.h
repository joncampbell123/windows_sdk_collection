/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 24 16:47:46 1998
 */
/* Compiler settings for AtlBehave.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __AtlBehave_h__
#define __AtlBehave_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IFactory_FWD_DEFINED__
#define __IFactory_FWD_DEFINED__
typedef interface IFactory IFactory;
#endif 	/* __IFactory_FWD_DEFINED__ */


#ifndef __IBehavior_FWD_DEFINED__
#define __IBehavior_FWD_DEFINED__
typedef interface IBehavior IBehavior;
#endif 	/* __IBehavior_FWD_DEFINED__ */


#ifndef __IEventSink_FWD_DEFINED__
#define __IEventSink_FWD_DEFINED__
typedef interface IEventSink IEventSink;
#endif 	/* __IEventSink_FWD_DEFINED__ */


#ifndef __Factory_FWD_DEFINED__
#define __Factory_FWD_DEFINED__

#ifdef __cplusplus
typedef class Factory Factory;
#else
typedef struct Factory Factory;
#endif /* __cplusplus */

#endif 	/* __Factory_FWD_DEFINED__ */


#ifndef __Behavior_FWD_DEFINED__
#define __Behavior_FWD_DEFINED__

#ifdef __cplusplus
typedef class Behavior Behavior;
#else
typedef struct Behavior Behavior;
#endif /* __cplusplus */

#endif 	/* __Behavior_FWD_DEFINED__ */


#ifndef __EventSink_FWD_DEFINED__
#define __EventSink_FWD_DEFINED__

#ifdef __cplusplus
typedef class EventSink EventSink;
#else
typedef struct EventSink EventSink;
#endif /* __cplusplus */

#endif 	/* __EventSink_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IFactory_INTERFACE_DEFINED__
#define __IFactory_INTERFACE_DEFINED__

/* interface IFactory */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("140D550C-2290-11D2-AF61-00A0C9C9E6C5")
    IFactory : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IFactory __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IFactory __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IFactory __RPC_FAR * This);
        
        END_INTERFACE
    } IFactoryVtbl;

    interface IFactory
    {
        CONST_VTBL struct IFactoryVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFactory_INTERFACE_DEFINED__ */


#ifndef __IBehavior_INTERFACE_DEFINED__
#define __IBehavior_INTERFACE_DEFINED__

/* interface IBehavior */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IBehavior;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("140D550E-2290-11D2-AF61-00A0C9C9E6C5")
    IBehavior : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IBehaviorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBehavior __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBehavior __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBehavior __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IBehavior __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IBehavior __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IBehavior __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IBehavior __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IBehaviorVtbl;

    interface IBehavior
    {
        CONST_VTBL struct IBehaviorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBehavior_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBehavior_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBehavior_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBehavior_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IBehavior_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IBehavior_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IBehavior_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBehavior_INTERFACE_DEFINED__ */


#ifndef __IEventSink_INTERFACE_DEFINED__
#define __IEventSink_INTERFACE_DEFINED__

/* interface IEventSink */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5B3517FB-306F-11D2-AF62-00A0C9C9E6C5")
    IEventSink : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IEventSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventSink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventSink __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventSink __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventSink __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventSink __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventSink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventSink __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IEventSinkVtbl;

    interface IEventSink
    {
        CONST_VTBL struct IEventSinkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventSink_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventSink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventSink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventSink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEventSink_INTERFACE_DEFINED__ */



#ifndef __ATLBEHAVELib_LIBRARY_DEFINED__
#define __ATLBEHAVELib_LIBRARY_DEFINED__

/* library ATLBEHAVELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATLBEHAVELib;

EXTERN_C const CLSID CLSID_Factory;

#ifdef __cplusplus

class DECLSPEC_UUID("140D550D-2290-11D2-AF61-00A0C9C9E6C5")
Factory;
#endif

EXTERN_C const CLSID CLSID_Behavior;

#ifdef __cplusplus

class DECLSPEC_UUID("140D550F-2290-11D2-AF61-00A0C9C9E6C5")
Behavior;
#endif

EXTERN_C const CLSID CLSID_EventSink;

#ifdef __cplusplus

class DECLSPEC_UUID("5B3517FC-306F-11D2-AF62-00A0C9C9E6C5")
EventSink;
#endif
#endif /* __ATLBEHAVELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
