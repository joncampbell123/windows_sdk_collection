/*
 * IMSGFILT.CPP
 *
 * Template IMessageFilter interface implementation.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "imsgfilt.h"


/*
 * CImpIMessageFilter::CImpIMessageFilter
 * CImpIMessageFilter::~CImpIMessageFilter
 *
 * Parameters (Constructor):
 *  pObj            LPVOID of the object we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIMessageFilter::CImpIMessageFilter(LPVOID pObj
    , LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pObj=pObj;
    m_pUnkOuter=pUnkOuter;
    return;
    }

CImpIMessageFilter::~CImpIMessageFilter(void)
    {
    return;
    }




/*
 * CImpIMessageFilter::QueryInterface
 * CImpIMessageFilter::AddRef
 * CImpIMessageFilter::Release
 *
 * Purpose:
 *  Delegating IUnknown members for CImpIMessageFilter.
 */

STDMETHODIMP CImpIMessageFilter::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) CImpIMessageFilter::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIMessageFilter::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }






/*
 * CImpIMessageFilter::HandleIncomingCall
 *
 * Purpose:
 *  Requests that the container call OleSave for the object that
 *  lives here.  Typically this happens on server shutdown.
 *
 * Parameters:
 *  dwCallType      DWORD indicating the type of call received, from
 *                  the CALLTYPE enumeration
 *  hTaskCaller     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the outgoing call
 *                  was made if dwCallType is not CALLTYPE_TOPLEVEL.
 *                  Ignored for other call types.
 *  pInterfaceInfo  LPINTERFACEINFO providing information about the
 *                  call.  Can be NULL.
 *
 * Return Value:
 *  DWORD           One of SERVERCALL_ISHANDLED (if the call might
 *                  be handled), SERVERCALL_REJECTED (call cannot
 *                  be handled), or SERVERCALL_RETRYLATER (try
 *                  again sometime).
 */

STDMETHOD_(DWORD) CImpIMessageFilter::HandleInComingCall
    (DWORD dwCallType, HTASK htaskCaller, DWORD dwTickCount
    , LPINTERFACEINFO pInterfaceInfo)
    {
    return SERVERCALL_ISHANDLED;
    }





/*
 * CImpIMessageFilter::RetryRejectedCall
 *
 * Purpose:
 *
 * Parameters:
 *  hTaskCallee     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the call was made
 *  dwRejectType    DWORD either SERVERCALL_REJECTED or
 *                  SERVERCALL_RETRYLATER as returned by
 *                  HandleIncomingCall.
 *
 * Return Value:
 *  DWORD           (DWORD)-1 to cancel the call, any number between
 *                  0 and 100 to try the call again immediately, or
 *                  a value over 100 (but not (DWORD)-1) to instruct
 *                  COM to wait that many milliseconds before trying
 *                  again.
 */

STDMETHOD_(DWORD) CImpIMessageFilter::RetryRejectedCall
    (HTASK htaskCallee, DWORD dwTickCount, DWORD dwRejectType)
    {
    return 0;
    }



/*
 * CImpIMessageFilter::MessagePending
 *
 * Purpose:
 *
 * Parameters:
 *  hTaskCallee     HTASK of the caller
 *  dwTickCount     DWORD elapsed tick count since the call was made
 *  dwPendingType   DWORD with the type of call made from the
 *                  PENDINGTYPE enumeration.
 *
 * Return Value:
 *  DWORD           One of PENDINGMSG_CANCELCALL (cancels the call
 *                  under extreme conditions), PENDINGMSG_WAITNO-
 *                  PROCESS (continue waiting), or PENDINGMSG_WAIT-
 *                  DEFPROCESS (invoke default handling).
 */

STDMETHOD_(DWORD) CImpIMessageFilter::MessagePending
    (HTASK htaskCallee, DWORD dwTickCount, DWORD dwPendingType)
    {
    return PENDINGMSG_WAITDEFPROCESS;
    }
