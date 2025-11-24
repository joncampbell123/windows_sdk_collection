/*
 -  mapiguid.c
 -
 *  Copyright (C) 1994 Microsoft Corporation
 *  Purpose:
 */

// This builds mapiguid.obj, which can be linked into a DLL
// or EXE to provide the MAPI GUIDs. It contains all GUIDs 
// defined by MAPI.

#define USES_IID_IUnknown     
#define USES_IID_IMAPISession     
#define USES_IID_IMAPITable       
#define USES_IID_IMAPIAdviseSink          
#define USES_IID_IMAPIControl     
#define USES_IID_IProfAdmin       
#define USES_IID_IMsgServiceAdmin 
#define USES_IID_IMAPIProgress
#define USES_IID_IMAPIProp        
#define USES_IID_IProfSect        
#define USES_IID_IMAPIStatus      
#define USES_IID_IMsgStore        
#define USES_IID_IMessage         
#define USES_IID_IAttachment      
#define USES_IID_IAddrBook        
#define USES_IID_IMailUser        
#define USES_IID_IMAPIContainer   
#define USES_IID_IMAPIFolder
#define USES_IID_IABContainer     
#define USES_IID_IDistList        
#define USES_IID_IMAPISup         
#define USES_IID_IMSProvider      
#define USES_IID_IABProvider      
#define USES_IID_IXPProvider      
#define USES_IID_IMSLogon         
#define USES_IID_IABLogon         
#define USES_IID_IXPLogon      
#define USES_IID_IMAPITableData   
#define USES_IID_IMAPISpoolerInit 
#define USES_IID_IMAPISpoolerSession
#define USES_IID_ITNEF       
#define USES_IID_IMAPIPropData
#define USES_IID_IMAPISpoolerService

#define USES_PS_MAPI
#define USES_PS_PUBLIC_STRINGS
#define USES_IID_IPersistMessage
#define USES_IID_IMAPIViewAdviseSink
#define USES_IID_IStreamDocfile
#define USES_IID_IMAPIFormProp
#define USES_IID_IMAPIFormContainer
#define USES_IID_IMAPIFormAdviseSink
#define USES_IID_IStreamTnef
#define USES_IID_IMAPIMessageSite
#define USES_IID_IProviderAdmin
#define USES_IID_ISpoolerHook
#define USES_IID_IMAPIViewContext
#define USES_IID_IMAPIFormMgr
#define USES_IID_IMAPIForm
#define USES_IID_IMAPIFormRegistry


#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif

#define INITGUID

#ifdef WIN32    /* Must include WINDOWS.H on Win32 */
#ifndef _WINDOWS_
#define INC_OLE2 /* Get the OLE2 stuff */
#define INC_RPC  /* harmless on Daytona; Chicago needs it */
#define _INC_OLE /* Chicago will include OLE1 without this */
#include <windows.h>
#pragma warning(disable:4001)   /* single line comments */
#endif
#include <objerror.h>
#include <objbase.h>
#include <initguid.h>
#endif

#ifdef WIN16
#include <windows.h>
#include <compobj.h>
#include <coguid.h>
#endif

#include <mapiguid.h>



