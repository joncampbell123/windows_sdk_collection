/* this ALWAYS GENERATED file contains the definitions for the interfaces 


File created by MIDL compiler version 3.00.15 
at Fri May 17 15:00:22 1996

Compiler settings for settime.idl:
   Os, W1, Zp8, env=Win32, ms_ext, c_ext
   error checks: none

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.
*/

//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __settime_h__
#define __settime_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IMySettime_FWD_DEFINED__
#define __IMySettime_FWD_DEFINED__
typedef interface IMySettime IMySettime;
#endif         /* __IMySettime_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IMySettime_INTERFACE_DEFINED__
#define __IMySettime_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IMySettime
 * at Fri May 17 15:00:22 1996
 * using MIDL 3.00.15
 ****************************************/
/* [unique][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IMySettime;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface IMySettime : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ SYSTEMTIME t) = 0;
        
    };
    
#else         /* C style interface */

    typedef struct IMySettimeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMySettime __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMySettime __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMySettime __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            IMySettime __RPC_FAR * This,
            /* [in] */ SYSTEMTIME t);
        
        END_INTERFACE
    } IMySettimeVtbl;

    interface IMySettime
    {
        CONST_VTBL struct IMySettimeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMySettime_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMySettime_AddRef(This)        \
    (This)->lpVtbl -> AddRef(This)

#define IMySettime_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IMySettime_Set(This,t)        \
    (This)->lpVtbl -> Set(This,t)

#endif /* COBJMACROS */


#endif         /* C style interface */



HRESULT STDMETHODCALLTYPE IMySettime_Set_Proxy( 
    IMySettime __RPC_FAR * This,
    /* [in] */ SYSTEMTIME t);


void __RPC_STUB IMySettime_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif         /* __IMySettime_INTERFACE_DEFINED__ */



#ifndef __SETTIMELib_LIBRARY_DEFINED__
#define __SETTIMELib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: SETTIMELib
 * at Fri May 17 15:00:22 1996
 * using MIDL 3.00.15
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_SETTIMELib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_MySettime;

class MySettime;
#endif
#endif /* __SETTIMELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
