#include "hdrs.h"

ASSERTDATA

// TEMPORARY delete after uilib fix
#define IS_SIGNIFICANT_EVENT		mDownMask | mUpMask | keyDownMask | keyUpMask | autoKeyMask
#define IS_URGENT_EVENT				updateMask | activMask | osMask

void OleMessageFilterRegister(CApplication * pApp)
{
	HRESULT		hrErr;
	
	ASSERTCOND(pApp->m_pIMessageFilter == nil);
	
	OleStdMessageFilterInitInterfaces();
	
	pApp->m_pIMessageFilter = OleStdMsgFilter_Create(
						gAppName,
						OleMessageFilterMessagePending,
						nil,
						(long)pApp
				);
	ASSERTCOND(pApp->m_pIMessageFilter != nil);

	LPMESSAGEFILTER		pPreviousMessageFilter = NULL;
	
	hrErr = CoRegisterMessageFilter(pApp->m_pIMessageFilter, &pPreviousMessageFilter);
	ASSERTCOND(hrErr == NOERROR);
	FailScode("\pCoRegisterMessageFilter",GetScode(hrErr));
	ASSERTCOND(pPreviousMessageFilter == nil);
	
	if (pPreviousMessageFilter != nil)
		pPreviousMessageFilter->Release();
			
	OleStdRelease((LPUNKNOWN)pApp->m_pIMessageFilter);
}

void OleMessageFilterRevoke(CApplication * pApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;
	LPMESSAGEFILTER		pPreviousMessageFilter;

	hrErr = CoRegisterMessageFilter(nil, &pPreviousMessageFilter);
	ASSERTCOND(hrErr == NOERROR);
	ASSERTCOND(pPreviousMessageFilter == pApp->m_pIMessageFilter || pApp->m_pIMessageFilter == nil);
	
	if (pPreviousMessageFilter != nil)
	{
		cRef = OleStdVerifyRelease((LPUNKNOWN)pPreviousMessageFilter);
		ASSERTCOND(cRef == 0);
	}
	
	pApp->m_pIMessageFilter = nil;
}

#ifndef _MSC_VER
#pragma segment OleMessageFilterSeg
#endif
Boolean OleMessageFilterMessagePending(long userData)
{
	CApplication *	pApp;
	EventRecord			theEvent;
	
	pApp = (CApplication *)userData;
	ASSERTCOND(pApp != nil);
		
#if 1

	if (WaitNextEvent(IS_URGENT_EVENT | IS_SIGNIFICANT_EVENT, &theEvent, 10, NULL))
	{
		switch (theEvent.what)
		{
			case mouseDown:
			case mouseUp:
			case keyUp:
				break;

			case keyDown:
			case autoKey:
				SysBeep(0);
				break;

			default:
				gApp->HandleEvent(theEvent);
				break;
		}
	}

	return true;

#else

	if (EventAvail(IS_URGENT_EVENT, &theEvent))
	{
		if (WaitNextEvent(IS_URGENT_EVENT, &theEvent, 0, nil))
		{
			ASSERTCOND(pApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr != nil);
			(*pApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr)(pApp->m_pIApp, &theEvent);
			
			return true;
		}
	}
	
	return false;

#endif
}

