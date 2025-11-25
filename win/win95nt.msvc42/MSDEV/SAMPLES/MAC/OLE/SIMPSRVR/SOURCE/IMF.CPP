#include "hdrs.h"

ASSERTDATA

void OleMessageFilterRegister(CApplication * pApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;
	LPMESSAGEFILTER		pPreviousMessageFilter;

	ASSERTCOND(pApp->m_pIMessageFilter == NULL);

	OleStdMessageFilterInitInterfaces();

	pApp->m_pIMessageFilter = OleStdMsgFilter_Create(
						gAppName,
						OleMessageFilterMessagePending,
						NULL,
						(long)pApp);
	ASSERTCOND(pApp->m_pIMessageFilter != NULL);

	hrErr = CoRegisterMessageFilter(pApp->m_pIMessageFilter, &pPreviousMessageFilter);
	ASSERTCOND(hrErr == NOERROR);
	FailScode("\pCoRegisterMessageFilter",GetScode(hrErr));
	ASSERTCOND(pPreviousMessageFilter == NULL);

	if (pPreviousMessageFilter != NULL)
	{
		cRef = pPreviousMessageFilter->Release();
		ASSERTCOND(cRef == 0);
	}

	OleStdRelease((LPUNKNOWN)pApp->m_pIMessageFilter);
}

void OleMessageFilterRevoke(CApplication * pApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;
	LPMESSAGEFILTER		pPreviousMessageFilter;

	hrErr = CoRegisterMessageFilter(NULL, &pPreviousMessageFilter);
	ASSERTCOND(hrErr == NOERROR);
	ASSERTCOND(pPreviousMessageFilter == pApp->m_pIMessageFilter || pApp->m_pIMessageFilter == NULL);

	if (pPreviousMessageFilter != NULL)
	{
		cRef = OleStdVerifyRelease((LPUNKNOWN)pPreviousMessageFilter);
		ASSERTCOND(cRef == 0);
	}

	pApp->m_pIMessageFilter = NULL;
}

#ifndef _MSC_VER
#pragma segment OleMessageFilterSeg
#endif
Boolean OleMessageFilterMessagePending(long userData)
{
	CApplication *	pApp;
	EventRecord			theEvent;

	pApp = (CApplication *)userData;
	ASSERTCOND(pApp != NULL);

	if (WaitNextEvent(UserInputMask | UrgentEvtMask, &theEvent, 10, NULL))
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
}

