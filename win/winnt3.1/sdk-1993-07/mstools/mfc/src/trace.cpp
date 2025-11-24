// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "afxwin.h"
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG       // entire file for debugging

#include "trace_.h"
#include <dde.h>

/////////////////////////////////////////////////////////////////////////////
// Build data tables by including data file three times

#define DO(WM_FOO)  static char BASED_CODE sz##WM_FOO[] = #WM_FOO;
#include "tracedat.h"
#undef DO

static UINT BASED_CODE allMessages[] =
{
#define DO(WM_FOO)  WM_FOO,
#include "tracedat.h"
#undef DO
	0   /* end of table */
};

static LPCSTR BASED_CODE allMessageNames[] =
{
#define DO(WM_FOO)  sz##WM_FOO,
#include "tracedat.h"
#undef DO
	NULL    /* end of table */
};

/////////////////////////////////////////////////////////////////////////////
// DDE special case

static void TraceDDE(LPCSTR lpPrefix, const MSG* pMsg)
{
	if (pMsg->message == WM_DDE_EXECUTE)
	{
#ifndef _NTWIN
		HANDLE hCommands = (HANDLE)HIWORD(pMsg->lParam);
#else
//REVIEW_NT: this is dumb, I wonder if it is correct.
		UINT nDummy;
		HANDLE hCommands;
		if (!UnpackDDElParam(WM_DDE_ADVISE, pMsg->lParam,
			(PUINT)&hCommands, &nDummy))
		{
			TRACE("Warning: Unable to unpack WM_DDE_EXECUTE lParam %08lX.\n",
				pMsg->lParam);
			return;
		}
#endif
		ASSERT(hCommands != NULL);

		LPCSTR lpCommands = (LPCSTR)::GlobalLock(hCommands);
		ASSERT(lpCommands != NULL);
		TRACE("%Fs: Execute '%Fs'\n", lpPrefix, lpCommands);
		::GlobalUnlock(hCommands);
	}
	else if (pMsg->message == WM_DDE_ADVISE)
	{
#ifndef _NTWIN
		ATOM aItem = HIWORD(pMsg->lParam);
		HANDLE hAdvise = (HANDLE)LOWORD(pMsg->lParam);
#else
		ATOM aItem;
		HANDLE hAdvise;
		if (!UnpackDDElParam(WM_DDE_ADVISE, pMsg->lParam,
			(PUINT)&hAdvise, (PUINT)&aItem))
		{
			TRACE("Warning: Unable to unpack WM_DDE_ADVISE lParam %08lX.\n",
				pMsg->lParam);
			return;
		}
#endif
		ASSERT(hAdvise != NULL);

		DDEADVISE FAR* lpAdvise = (DDEADVISE FAR *)::GlobalLock(hAdvise);
		ASSERT(lpAdvise != NULL);                   
		char szItem[80];
		szItem[0] = '\0';

		if (aItem != 0)
			::GlobalGetAtomName(aItem, szItem, sizeof(szItem));

		char szFormat[80];
		szFormat[0] = '\0';
		if ((0xC000 <= lpAdvise->cfFormat) && (lpAdvise->cfFormat <= 0xFFFF))
			::GetClipboardFormatName(lpAdvise->cfFormat,
				szFormat, sizeof(szFormat));

			// User defined clipboard formats have a range of 0xC000->0xFFFF
			// System clipboard formats have other ranges, but no printable
			// format names.


		TRACE("%Fs: Advise item='%s', Format='%s', Ack=%d, Defer Update= %d\n",
			lpPrefix, szItem, szFormat, lpAdvise->fAckReq, lpAdvise->fDeferUpd);
		::GlobalUnlock(hAdvise);
	}
}

/////////////////////////////////////////////////////////////////////////////

void AfxTraceMsg(LPCSTR lpPrefix, const MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE ||
		pMsg->message == WM_NCHITTEST ||
		pMsg->message == WM_SETCURSOR ||
#ifndef _NTWIN
		pMsg->message == WM_CTLCOLOR ||
#else
		pMsg->message == WM_CTLCOLORMSGBOX ||
		pMsg->message == WM_CTLCOLOREDIT ||
		pMsg->message == WM_CTLCOLORLISTBOX ||
		pMsg->message == WM_CTLCOLORBTN ||
		pMsg->message == WM_CTLCOLORDLG ||
		pMsg->message == WM_CTLCOLORSCROLLBAR ||
		pMsg->message == WM_CTLCOLORSTATIC ||
#endif
		pMsg->message == WM_ENTERIDLE)
	{
		// never report mouse moves (too frequent) or other messages also
		//   sent as part of mouse movement
		return;
	}

	LPCSTR lpMsgName = NULL;
	char szBuf[80];

	// find message name
	if (pMsg->message >= 0xC000)
	{
		// Window message registered with 'RegisterWindowMessage'
		//  (actually a USER atom)
		if (::GetClipboardFormatName(pMsg->message, szBuf, sizeof(szBuf)) != 0)
			lpMsgName = szBuf;
	}
	else if (pMsg->message >= WM_USER)
	{
		// User message
		sprintf(szBuf, "WM_USER+0x%04X", pMsg->message - WM_USER);
		lpMsgName = szBuf;
	}
	else
	{
		// a system windows message
		const UINT FAR* lpMessage;
		for (lpMessage = allMessages; *lpMessage != 0; lpMessage++)
		{
			if (*lpMessage == pMsg->message)
			{
				int iMsg = lpMessage - (const UINT FAR*)allMessages;
				lpMsgName = allMessageNames[iMsg];
				break;
			}
		}
	}

	if (lpMsgName != NULL)
	{
		TRACE("%Fs: hwnd=0x%04x, msg = %Fs (0x%04x, 0x%08x)\n", lpPrefix,
			pMsg->hwnd, lpMsgName, pMsg->wParam, pMsg->lParam);
	}
	else
	{
		TRACE("%Fs: hwnd=0x%04x, msg = 0x%04x (0x%04x, 0x%08x)\n", lpPrefix,
			pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}

	if (pMsg->message >= WM_DDE_FIRST && pMsg->message <= WM_DDE_LAST)
	{
		TraceDDE(lpPrefix, pMsg);
	}
}

/////////////////////////////////////////////////////////////////////////////

#endif // _DEBUG (entire file)
