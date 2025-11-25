/*
 *    MSGFILTR.C
 *
 *    This file contains a standard implementation of IMessageFilter
 *    interface.
 *    This file is part of the OLE 2.0 User Interface support library.
 *
 *    (c) Copyright Microsoft Corp. 1990 - 1994 All Rights Reserved
 *
 */


#if defined(__MWERKS__) && !defined(__powerc)
#pragma pointers_in_D0
#endif

#if defined(USEHEADER)
#include "OleHeaders.h"
#endif
#ifndef _MSC_VER
#include <Resources.h>
#else
#include <Resource.h>
#endif
#include <stdio.h>

#include <ole2.h>

#include "ole2ui.h"
#include "uidebug.h"


OLEDBGDATA


static IMessageFilterVtbl g_OleStdMessageFilterVtbl;


typedef struct tagOLESTDMESSAGEFILTER {
    IMessageFilterVtbl* 		m_lpVtbl;
    unsigned int            	m_cRef;
    unsigned long           	m_dwInComingCallStatus; // Status to return from
                                                    	// HandleIncomingCall
    HANDLEINCOMINGCALLBACKPROC 	m_lpfnHandleInComingCallback;
                                                    // Callback function
                                                    // to selectively handle
                                                    // interface method calls
    Boolean                    	m_fEnableBusyDialog;    // enable RetryRejected
                                                    	//  Call dialog
    Boolean                    	m_fEnableNotRespondingDialog; 	// enable
                                                    			// MessagePending dialog
    MSGPENDINGPROC          	m_lpfnMessagePendingCallback; 	// MessagePending
                                                    			// Callback function
    LPFNOLEUIHOOK           	m_lpfnBusyDialogHookCallback; 	// Busy dialog hook
    char*                   	m_lpszAppName;          		// Name of application
                                                    			// installing filter
    Boolean                    	m_bUnblocking;

	NMRec						m_nmRec;				// notification record
	Boolean						m_fNotifyInstalled;		// whether the record is installed

    long						m_dwUserData;

 } OLESTDMESSAGEFILTER, * LPOLESTDMESSAGEFILTER;

/* interface IMessageFilter implementation */
STDMETHODIMP OleStdMsgFilter_QueryInterface(
        LPMESSAGEFILTER lpThis, REFIID riid, void** ppvObj);
STDMETHODIMP_(unsigned long) OleStdMsgFilter_AddRef(LPMESSAGEFILTER lpThis);
STDMETHODIMP_(unsigned long) OleStdMsgFilter_Release(LPMESSAGEFILTER lpThis);
STDMETHODIMP_(unsigned long) OleStdMsgFilter_HandleInComingCall (
        LPMESSAGEFILTER     		lpThis,
        unsigned long               dwCallType,
        ProcessSerialNumberPtr      htaskCaller,
        unsigned long               dwTickCount,
        LPINTERFACEINFO             lpInterfaceInfo
);
STDMETHODIMP_(unsigned long) OleStdMsgFilter_RetryRejectedCall (
        LPMESSAGEFILTER     		lpThis,
        ProcessSerialNumberPtr      htaskCallee,
        unsigned long               dwTickCount,
        unsigned long               dwRejectType
);
STDMETHODIMP_(unsigned long) OleStdMsgFilter_MessagePending (
        LPMESSAGEFILTER     		lpThis,
        ProcessSerialNumberPtr      htaskCallee,
        unsigned long               dwTickCount,
        unsigned long               dwPendingType
);

static void OleStdMsgFilter_NotifyUser(LPOLESTDMESSAGEFILTER lpStdMsgFilter, Boolean fEnable);

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdMessageFilterInitInterfaces(void)
{
	IMessageFilterVtbl*		p;

	p = &g_OleStdMessageFilterVtbl;

	p->QueryInterface		= OleStdMsgFilter_QueryInterface;
	p->AddRef				= OleStdMsgFilter_AddRef;
	p->Release				= OleStdMsgFilter_Release;
	p->HandleInComingCall	= OleStdMsgFilter_HandleInComingCall;
	p->RetryRejectedCall	= OleStdMsgFilter_RetryRejectedCall;
	p->MessagePending		= OleStdMsgFilter_MessagePending;
}
	

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(LPMESSAGEFILTER) OleStdMsgFilter_Create(
        char*           szAppName,
        MSGPENDINGPROC  lpfnCallback,
        LPFNOLEUIHOOK   lpfnOleUIHook,         // Busy dialog hook callback
        long			userData
)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter;
    LPMALLOC lpMalloc;

    if (CoGetMalloc(MEMCTX_TASK, (LPMALLOC *)&lpMalloc) != NOERROR)
        return NULL;

    lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpMalloc->lpVtbl->Alloc(
            lpMalloc, (sizeof(OLESTDMESSAGEFILTER)));
    lpMalloc->lpVtbl->Release(lpMalloc);
    if (! lpStdMsgFilter) return NULL;

    lpStdMsgFilter->m_lpVtbl = &g_OleStdMessageFilterVtbl;
    lpStdMsgFilter->m_cRef = 1;
    lpStdMsgFilter->m_dwInComingCallStatus = SERVERCALL_ISHANDLED;
    lpStdMsgFilter->m_lpfnHandleInComingCallback = NULL;
    lpStdMsgFilter->m_fEnableBusyDialog = TRUE;
    lpStdMsgFilter->m_fEnableNotRespondingDialog = TRUE;
    lpStdMsgFilter->m_lpszAppName = szAppName;
    lpStdMsgFilter->m_lpfnMessagePendingCallback = lpfnCallback;
    lpStdMsgFilter->m_lpfnBusyDialogHookCallback = lpfnOleUIHook;
    lpStdMsgFilter->m_fNotifyInstalled = FALSE;
    lpStdMsgFilter->m_bUnblocking = FALSE;
    lpStdMsgFilter->m_dwUserData = userData;

    return (LPMESSAGEFILTER)lpStdMsgFilter;
}


/* OleStdMsgFilter_SetInComingStatus
** ---------------------------------
**    This is a private function that allows the caller to control what
**    value is returned from the IMessageFilter::HandleInComing method.
**
**    if a HandleInComingCallbackProc is installed by a call to
**    OleStdMsgFilter_SetHandleInComingCallbackProc, then this
**    overrides the dwIncomingCallStatus established by a call to
**    OleStdMsgFilter_SetInComingStatus.  Using
**    OleStdMsgFilter_SetInComingStatus allows the app to reject or
**    accept ALL in coming calls. Using a HandleInComingCallbackProc
**    allows the app to selectively handle or reject particular method
**    calls.
*/

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(void) OleStdMsgFilter_SetInComingCallStatus(
        LPMESSAGEFILTER lpThis, unsigned long dwInComingCallStatus)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;

    if (lpStdMsgFilter)
        lpStdMsgFilter->m_dwInComingCallStatus = dwInComingCallStatus;
    else
        OleDbgAssert(
            "OleStdMsgFilter_SetIncomingCallStatus: Invalid IMessageFilter*");

}


/* OleStdMsgFilter_SetHandleInComingCallbackProc
** ---------------------------------------------
**    This is a private function that allows the caller to install (or
**    de-install) a special callback function to selectively
**    handle/reject specific incoming method calls on particular
**    interfaces.
**
**    if a HandleInComingCallbackProc is installed by a call to
**    OleStdMsgFilter_SetHandleInComingCallbackProc, then this
**    overrides the dwIncomingCallStatus established by a call to
**    OleStdMsgFilter_SetInComingStatus.  Using
**    OleStdMsgFilter_SetInComingStatus allows the app to reject or
**    accept ALL in coming calls. Using a HandleInComingCallbackProc
**    allows the app to selectively handle or reject particular method
**    calls.
**
**    to de-install the HandleInComingCallbackProc, call
**          OleStdMsgFilter_SetHandleInComingCallbackProc(NULL);
**
**    Returns previous callback proc in effect.
*/

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(HANDLEINCOMINGCALLBACKPROC)
    OleStdMsgFilter_SetHandleInComingCallbackProc(
        LPMESSAGEFILTER             lpThis,
        HANDLEINCOMINGCALLBACKPROC  lpfnHandleInComingCallback)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    HANDLEINCOMINGCALLBACKPROC    lpfnPrevCallback =
            lpStdMsgFilter->m_lpfnHandleInComingCallback;

    if (lpStdMsgFilter)
        lpStdMsgFilter->m_lpfnHandleInComingCallback = lpfnHandleInComingCallback;
    else
        OleDbgAssert("OleStdMsgFilter_SetIncomingCallStatus: Invalid IMessageFilter*");

    return lpfnPrevCallback;
}


/* OleStdMsgFilter_GetInComingStatus
** ---------------------------------
**    This is a private function that returns the current
**    incoming call status.  Can be used to disable/enable options
**    in the calling application.
**
** Returns: one of
**
**    SERVERCALL_ISHANDLED
**    SERVERCALL_REJECTED
**    SERVERCALL_RETRYLATER
**    or -1 for ERROR
**
*/

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(unsigned long) OleStdMsgFilter_GetInComingCallStatus(
        LPMESSAGEFILTER lpThis)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    unsigned long dwReturn;

    if (lpStdMsgFilter)
        dwReturn = lpStdMsgFilter->m_dwInComingCallStatus;
    else {
        OleDbgAssert("OleStdMsgFilter_GetIncomingCallStatus: Invalid IMessageFilter*");
        dwReturn = (unsigned long)-1;
	}

    return dwReturn;
}


/* OleStdMsgFilter_EnableBusyDialog
** --------------------------------
**    This function allows the caller to control whether
**    the busy dialog is enabled. this is the dialog put up when
**    IMessageFilter::RetryRejectedCall is called because the server
**    responded SERVERCALL_RETRYLATER or SERVERCALL_REJECTED.
**
**    if the busy dialog is NOT enabled, then the rejected call is
**    immediately canceled WITHOUT prompting the user. in this situation
**    OleStdMsgFilter_RetryRejectedCall always retuns
**    OLESTDCANCELRETRY canceling the outgoing LRPC call.
**    If the busy dialog is enabled, then the user is given the choice
**    of whether to retry, switch to, or cancel.
**
**    Returns previous dialog enable state
*/

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(Boolean) OleStdMsgFilter_EnableBusyDialog(LPMESSAGEFILTER lpThis, Boolean fEnable)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    Boolean fPrevEnable = lpStdMsgFilter->m_fEnableBusyDialog;

    if (lpStdMsgFilter)
        lpStdMsgFilter->m_fEnableBusyDialog = fEnable;
    else
        OleDbgAssert("OleStdMsgFilter_EnableBusyDialog: Invalid IMessageFilter*");

    return fPrevEnable;
}


/* OleStdMsgFilter_EnableNotRespondingDialog
** -----------------------------------------
**    This function allows the caller to control whether
**    the app "NotResponding" (Blocked) dialog is enabled. this is the
**    dialog put up when IMessageFilter::MessagePending is called.
**    If the NotResponding dialog is enabled, then the user is given
**    the choice of whether to retry or switch to, but NOT to cancel.
**
**    Returns previous dialog enable state
*/

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDAPI_(Boolean) OleStdMsgFilter_EnableNotRespondingDialog(LPMESSAGEFILTER lpThis, Boolean fEnable)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    Boolean fPrevEnable = lpStdMsgFilter->m_fEnableNotRespondingDialog;

    if (lpStdMsgFilter)
        lpStdMsgFilter->m_fEnableNotRespondingDialog = fEnable;
    else
        OleDbgAssert("OleStdMsgFilter_EnableNotRespondingDialog: Invalid IMessageFilter*");

    return fPrevEnable;
}


#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP OleStdMsgFilter_QueryInterface(LPMESSAGEFILTER lpThis, REFIID riid, void** ppvObj)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    SCODE scode;

	/*
     * Two interfaces supported: IUnknown, IMessageFilter
     */
    if (IsEqualIID(riid, &IID_IMessageFilter) || IsEqualIID(riid, &IID_IUnknown)) {
        lpStdMsgFilter->m_cRef++;   // A pointer to this object is returned
        *ppvObj = lpThis;
        scode = S_OK;
    }
    else {                 // unsupported interface
        *ppvObj = NULL;
        scode = E_NOINTERFACE;
    }

    return ResultFromScode(scode);
}


#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long) OleStdMsgFilter_AddRef(LPMESSAGEFILTER lpThis)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    return ++lpStdMsgFilter->m_cRef;
}

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long) OleStdMsgFilter_Release(LPMESSAGEFILTER lpThis)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    LPMALLOC lpMalloc;

    if (--lpStdMsgFilter->m_cRef != 0) // Still used by others
        return lpStdMsgFilter->m_cRef;

    // Free storage
    if (CoGetMalloc(MEMCTX_TASK, (LPMALLOC*)&lpMalloc) != NOERROR)
        return (unsigned long)0;

    lpMalloc->lpVtbl->Free(lpMalloc, lpStdMsgFilter);
    lpMalloc->lpVtbl->Release(lpMalloc);
    return (unsigned long)0;
}


#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long) OleStdMsgFilter_HandleInComingCall (
        LPMESSAGEFILTER     		lpThis,
        unsigned long               dwCallType,
        ProcessSerialNumberPtr      htaskCaller,
        unsigned long               dwTickCount,
        LPINTERFACEINFO             lpInterfaceInfo
)
{
    LPOLESTDMESSAGEFILTER lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;

    /* if a HandleInComingCallbackProc is in effect, then this
    **    overrides dwIncomingCallStatus established by a call to
    **    OleStdMsgFilter_SetInComingStatus.  we will call this
    **    callback to allow the app to selectively handle or reject
    **    incoming method calls. the LPINTERFACEINFO parameter
    **    describes which method is being called.
    */
    if (lpStdMsgFilter->m_lpfnHandleInComingCallback){
        return lpStdMsgFilter->m_lpfnHandleInComingCallback(
						                dwCallType,
						                htaskCaller,
						                dwTickCount,
						                (LPINTERFACEINFO)lpInterfaceInfo);
    }

    switch (dwCallType) {
        case CALLTYPE_TOPLEVEL:
            /* OLE2NOTE: we currently have NO pending outgoing call and
            **    there is a new toplevel incoming call.
            **    this call may be rejected.
            */
            return lpStdMsgFilter->m_dwInComingCallStatus;

        case CALLTYPE_TOPLEVEL_CALLPENDING:
            /* OLE2NOTE: we currently HAVE a pending outgoing call and
            **    there is a new toplevel incoming call.
            **    this call may be rejected.
            */
            return lpStdMsgFilter->m_dwInComingCallStatus;

        case CALLTYPE_NESTED:
            /* OLE2NOTE: we currently HAVE a pending outgoing call and
            **    there callback on behalf of the previous outgoing
            **    call. this type of call should ALWAYS be handled.
            */
            return SERVERCALL_ISHANDLED;

        case CALLTYPE_ASYNC:
            /* OLE2NOTE: we currently have NO pending outgoing call and
            **    there is a new asynchronous incoming call.
            **    this call can NEVER be rejected. OLE actually ignores
            **    the return code in this case and always allows the
            **    call through.
            */
            return SERVERCALL_ISHANDLED;    // value returned does not matter

        case CALLTYPE_ASYNC_CALLPENDING:
            /* OLE2NOTE: we currently HAVE a pending outgoing call and
            **    there is a new asyncronis incoming call.
            **    this call can NEVER be rejected. OLE ignore the
            **    return code in this case.
            */
            return SERVERCALL_ISHANDLED;    // value returned does not

        default:
            OleDbgAssert("OleStdMsgFilter_HandleInComingCall: Invalid CALLTYPE");
            return lpStdMsgFilter->m_dwInComingCallStatus;
    }
}


#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long) OleStdMsgFilter_RetryRejectedCall (
        LPMESSAGEFILTER     		lpThis,
        ProcessSerialNumberPtr      pPSNCallee,
        unsigned long               dwTickCount,
        unsigned long               dwRejectType
)
{
    LPOLESTDMESSAGEFILTER	lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
    unsigned long			dwRet = 0;
    unsigned int			uRet;

    /* OLE2NOTE: we do not want to put up the Busy dialog immediately
    **    when an app says RETRYLATER. we should continue retrying
    **    for a while in case the app can un-busy itself in a
    **    reasonable amount of time.
    */
    if (dwRejectType == SERVERCALL_RETRYLATER && dwTickCount <= (unsigned long)OLESTDRETRYDELAY)
    {
        dwRet = 0;                  // Retry immediately
    }

    /* OLE2NOTE: we should only put up the application busy dialog when
    **    the callee has responded SERVERCALL_RETRYLATER. if the
    **    dwRejectType is SERVERCALL_REJECTED then there is something
    **    seriously wrong with the callee (perhaps a severe low memory
    **    situation). we don't want to even try to "Switch To" this app
    **    or even try to "Retry".
    */
    else if (dwRejectType == SERVERCALL_RETRYLATER && lpStdMsgFilter->m_fEnableBusyDialog)
    {
		ProcessSerialNumber	psnCurrent;
		ProcessSerialNumber	psnFront;
		Boolean				fSame;
        OLEUIBUSY 			bz;

		GetCurrentProcess(&psnCurrent);
		
		GetFrontProcess(&psnFront);
		SameProcess(&psnCurrent, &psnFront, &fSame);
		
		/* Note: we don't want to pop up the modal dialog if we are not the front process, so
		 *		 we will just keep retrying
		 */
		if (fSame)
		{
			OleStdMsgFilter_NotifyUser(lpStdMsgFilter, false);

		        /* Set up structure for calling OLEUIBUSY dialog */
	        OleStdMemSet(&bz, 0, sizeof(OLEUIBUSY));
	        bz.cbStruct = sizeof(OLEUIBUSY);
	        bz.dwFlags = 0L;
	        bz.ptPosition.v = 0;
	        bz.ptPosition.h = 0;
	        bz.lpszCaption = lpStdMsgFilter->m_lpszAppName;
	        bz.lpfnHook = lpStdMsgFilter->m_lpfnBusyDialogHookCallback;
	        bz.lCustData = lpStdMsgFilter->m_dwUserData;
	        bz.pPSN = pPSNCallee;
	
	        uRet = OleUIBusy(&bz);
	
	        switch (uRet)
	        {
	        	case OLEUI_BZ_SWITCHTOSELECTED:
	            case OLEUI_BZ_RETRYSELECTED:
	                dwRet = OLESTDRETRYDELAY * 1000 / 60;   // Retry immediately
	                break;
	
	            case OLEUI_CANCEL:
	                dwRet = OLESTDCANCELRETRY;  // Cancel pending outgoing call
	                break;
	
	            case OLEUI_BZERR_HTASKINVALID:
	                // PSN was invalid, return OLESTDRETRYDELAY anyway
	                dwRet = OLESTDRETRYDELAY * 1000 / 60;   // Retry after <retry delay> msec
	                break;
	        }
	    }
	    else
	    {
			OleStdMsgFilter_NotifyUser(lpStdMsgFilter, true);
	      	dwRet = 0;                  // Retry immediately while we are in the background
	    }
    }
    else
    {
        dwRet = OLESTDCANCELRETRY;  // Cancel pending outgoing call
    }

    return dwRet;
}


#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
STDMETHODIMP_(unsigned long) OleStdMsgFilter_MessagePending (
        LPMESSAGEFILTER     		lpThis,
        ProcessSerialNumberPtr      pPSNCallee,
        unsigned long               dwTickCount,
        unsigned long               dwPendingType)
{
#ifndef _MSC_VER
#pragma unused(dwPendingType)
#endif

    LPOLESTDMESSAGEFILTER   lpStdMsgFilter = (LPOLESTDMESSAGEFILTER)lpThis;
	static unsigned long	dwTickLimit = (unsigned long)OLESTDRETRYDELAY,
							dwTickStart;
	static short			fSameCount = 0;
    EventRecord             msg;
    unsigned int            uRet;
	
    /* OLE2NOTE: If our tick count for this call exceeds our standard retry
    **      delay, then we need to put up the dialog.  We will only
    **      consider putting up the dialog if the user has issued a
    **      "significant" event (ie. mouse click or keyboard event). a
    **      simple mouse move should NOT trigger this dialog.
    **      Since our call to
    **      OleUIBusy below enters a DialogBox() message loop, there's a
    **      possibility that another call will be initiated during the dialog,
    **      and this procedure will be re-entered.  Just so we don't put up
    **      two dialogs at a time, we use the m_bUnblocking varable
    **      to keep track of this situation.
    */

	if (dwTickCount + (unsigned long)OLESTDRETRYDELAY < dwTickLimit)
		dwTickLimit = (unsigned long)OLESTDRETRYDELAY;

    if (dwTickCount > dwTickLimit && !lpStdMsgFilter->m_bUnblocking && EventAvail(UserInputMask, &msg))
    {
        if (lpStdMsgFilter->m_fEnableNotRespondingDialog && WaitNextEvent(UserInputMask, &msg, 10, NULL))
        {
           OLEUIBUSY bz;
			ProcessSerialNumber	psnCurrent;
			ProcessSerialNumber	psnFront;
			Boolean				fSame;
	
			GetCurrentProcess(&psnCurrent);
			
			GetFrontProcess(&psnFront);
			SameProcess(&psnCurrent, &psnFront, &fSame);
			
			if (fSame)
				fSameCount++;

			if (fSameCount >= 2)
			{
				// OleStdMsgFilter_NotifyUser(lpStdMsgFilter, false);

				dwTickStart = TickCount();
				lpStdMsgFilter->m_bUnblocking = TRUE;
				
				/* OLE2NOTE: We need to eat all mouse and keyboard input messages.
				**      These messages were ones where the user tried playing around
				**      with our app as we were waiting for our OLE server to
				**      respond.  We want to eat them so they don't affect our app.
				*/
				
				// Eat all mouse & keyboard messages in our queue
				while (GetNextEvent(UserInputMask, &msg))
					;
						
				/* Set up structure for calling OLEUIBUSY dialog,
				** using the "not responding" variety
				*/
				OleStdMemSet(&bz, 0, sizeof(OLEUIBUSY));
				bz.cbStruct = sizeof(OLEUIBUSY);
				bz.dwFlags = BZ_NOTRESPONDINGDIALOG;
				bz.ptPosition.v = 0;
				bz.ptPosition.h = 0;
				bz.lpszCaption = lpStdMsgFilter->m_lpszAppName;
				bz.lpfnHook = lpStdMsgFilter->m_lpfnBusyDialogHookCallback;
				bz.lCustData = lpStdMsgFilter->m_dwUserData;
				bz.pPSN = pPSNCallee;
				
				uRet = OleUIBusy(&bz);
				
				if (uRet == OLEUI_BZ_SWITCHTOSELECTED || uRet == OLEUI_BZ_RETRYSELECTED)
					dwTickLimit = dwTickCount + (unsigned long)OLESTDRETRYDELAY + (TickCount() - dwTickStart);

				lpStdMsgFilter->m_bUnblocking = FALSE;
				fSameCount = 0;
				
				return PENDINGMSG_WAITNOPROCESS;
        	}
        	else
        	{
				// OleStdMsgFilter_NotifyUser(lpStdMsgFilter, true);
        	}
    	}
	}

    /* If we're already unblocking, we're being re-entered.  Don't
    ** process message
    */

    if (lpStdMsgFilter->m_bUnblocking)
    {
		WaitNextEvent(UserInputMask | UrgentEvtMask, &msg, 10, NULL);		// called on behalf of the app
        return PENDINGMSG_WAITNOPROCESS;	
    }

    /* OLE2NOTE: If we have a callback function set up, call it with the
    ** current message.  If not, tell OLE LPRC mechanism to automatically
    ** handle all messages.
    */
    if (lpStdMsgFilter->m_lpfnMessagePendingCallback)
    {
		// the result from this call is ignored since Mac OLE provides no
		// default handling of messages.
		lpStdMsgFilter->m_lpfnMessagePendingCallback(lpStdMsgFilter->m_dwUserData);
    }
    else
		WaitNextEvent(UserInputMask | UrgentEvtMask, &msg, 10, NULL);		// called on behalf of the app

    return PENDINGMSG_WAITNOPROCESS;
}

#ifndef _MSC_VER
#pragma segment MsgFiltrSeg
#pragma code_seg("MsgFiltrSeg", "SWAPPABLE")
#endif
static void OleStdMsgFilter_NotifyUser(LPOLESTDMESSAGEFILTER lpStdMsgFilter, Boolean fEnable)
{
	ProcessSerialNumber	psnCurrent;
    ProcessInfoRec		pir;
    PicHandle			hMetaPict = nil;
    PicHandle			hIcon = nil;
    Handle				hString;
    char				*pszMessage = nil;
    Str255				processName;
    FSSpec				fspProcess;

#ifdef UIDLL
   short             hostResNum = 0;
#endif

	if (fEnable && !lpStdMsgFilter->m_fNotifyInstalled) {
		GetCurrentProcess(&psnCurrent);
		OleStdMemSet(&pir, 0, sizeof(pir));		// the standard memset in the library doesn't work
		pir.processInfoLength = sizeof(pir);
		pir.processName = processName;
		pir.processAppSpec = &fspProcess;
		GetProcessInformation(&psnCurrent, &pir);

#if NOTIFYICON

		hMetaPict = OleGetIconOfFSp(&fspProcess, TRUE);
			
		if (hMetaPict) {
			hIcon = OleUIPictExtractIcon(hMetaPict);
			OleUIPictIconFree(hMetaPict);
		}

		OleStdMemSet(&lpStdMsgFilter->m_nmRec, 0, sizeof(NMRec));
		lpStdMsgFilter->m_nmRec.qType = nmType;
		lpStdMsgFilter->m_nmRec.nmMark = 1;
		lpStdMsgFilter->m_nmRec.nmIcon = (Handle)hIcon;
		lpStdMsgFilter->m_nmRec.nmSound = (Handle)-1;
		lpStdMsgFilter->m_nmRec.nmStr = NULL;
		lpStdMsgFilter->m_nmRec.nmResp = 0;
		lpStdMsgFilter->m_nmRec.nmRefCon = 0;
		
		if (NMInstall(&lpStdMsgFilter->m_nmRec) == noErr)
			lpStdMsgFilter->m_fNotifyInstalled = TRUE;
		else {
			if (lpStdMsgFilter->m_nmRec.nmIcon) {
				OleUIPictIconFree((PicHandle)lpStdMsgFilter->m_nmRec.nmIcon);
				lpStdMsgFilter->m_nmRec.nmIcon = nil;
			}
		}
#else

		pszMessage = NewPtr(255);
#ifdef UIDLL
      hostResNum = SetUpOLEUIResFile();
#endif

		hString = (Handle)GetResource('CSTR', IDS_APPBLOCKED);

#ifdef UIDLL
      ClearOLEUIResFile(hostResNum);
#endif

		p2cstr(processName);
		sprintf(pszMessage, *hString, (char*)processName, (char*)processName);
		c2pstr(pszMessage);
		
		lpStdMsgFilter->m_nmRec.qType = nmType;
		lpStdMsgFilter->m_nmRec.nmMark = 0;
		lpStdMsgFilter->m_nmRec.nmIcon = nil;
		lpStdMsgFilter->m_nmRec.nmSound = (Handle)-1;
		lpStdMsgFilter->m_nmRec.nmStr = (StringPtr)pszMessage;
#ifdef __powerc
		lpStdMsgFilter->m_nmRec.nmResp = (NMUPP)-1;
#else
		lpStdMsgFilter->m_nmRec.nmResp = (NMProcPtr)-1;
#endif
		lpStdMsgFilter->m_nmRec.nmRefCon = 0;
		
		if (NMInstall(&lpStdMsgFilter->m_nmRec) == noErr)
			lpStdMsgFilter->m_fNotifyInstalled = TRUE;

#endif
	}
	else if (!fEnable && lpStdMsgFilter->m_fNotifyInstalled) {
	
#if NOTIFYICON

		if (NMRemove(&lpStdMsgFilter->m_nmRec) == noErr) {
			lpStdMsgFilter->m_fNotifyInstalled = FALSE;
			if (lpStdMsgFilter->m_nmRec.nmIcon) {
				OleUIPictIconFree((PicHandle)lpStdMsgFilter->m_nmRec.nmIcon);
				lpStdMsgFilter->m_nmRec.nmIcon = nil;
			}
		}
#else
			lpStdMsgFilter->m_fNotifyInstalled = FALSE;
			if (lpStdMsgFilter->m_nmRec.nmStr) {
				DisposPtr((Ptr)lpStdMsgFilter->m_nmRec.nmStr);
				lpStdMsgFilter->m_nmRec.nmStr = nil;
			}
			
#endif
	}
}
