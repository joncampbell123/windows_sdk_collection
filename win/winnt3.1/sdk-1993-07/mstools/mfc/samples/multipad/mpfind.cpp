// mpfind.cpp : Defines the class behaviors for the text searches.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "multipad.h"
#include "ctype.h" // for _tolower

#ifndef _NTWIN
#pragma code_seg("_MPFIND")
#endif

// HIWORD and LOWORD as defined are not able to be used as lvalues, so
// "HIWORD(dwVar) = 0xFFFF" are normally impossible.  These macros allow it.
//
#undef HIWORD
#undef LOWORD
#define HIWORD(l) (((WORD*)&(l))[1])
#define LOWORD(l) (((WORD*)&(l))[0])

///////////////////////////////////////////////////////////////////////////
// Search dialog stuff
UINT CMPFrame::m_nMsgFind = ::RegisterWindowMessage(FINDMSGSTRING);
CFindReplaceDialog* CMPFrame::m_pFindReplace = NULL;
CString CMPFrame::m_strFind;
static BOOL bMatchCase = FALSE;

LONG
CMPFrame::CmdFindHelper(UINT wParam, LONG lParam)
{
	CFindReplaceDialog* pDlgFR = CFindReplaceDialog::GetNotifier(lParam);
	ASSERT(pDlgFR == m_pFindReplace);

	if (pDlgFR->IsTerminating())
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		m_pFindReplace = NULL;  // DO NOT DELETE, DONE AUTOMATICALLY

		GetMenu()->EnableMenuItem(IDM_SEARCHFIND, MF_ENABLED);
		if (m_strFind.GetLength() > 0)
		{
			GetMenu()->EnableMenuItem(IDM_SEARCHNEXT, MF_ENABLED);
			GetMenu()->EnableMenuItem(IDM_SEARCHPREV, MF_ENABLED);
		}
	}
	else
	{
		ASSERT(m_pActiveChild != NULL);

		// look for string
		m_strFind = m_pFindReplace->GetFindString();
		if (m_strFind.GetLength() == 0)
			return 0;

		if (m_pActiveChild->FindText(m_strFind,
			m_pFindReplace->SearchDown() ? 
					CMPChild::searchDown : CMPChild::searchUp,
			bMatchCase = m_pFindReplace->MatchCase()) == FALSE)
		{
			MPError(MB_OK|MB_ICONEXCLAMATION, IDS_CANTFIND, (LPCSTR)m_strFind);
		}
		else
		{
			GetMenu()->EnableMenuItem(IDM_SEARCHNEXT, MF_ENABLED);
			GetMenu()->EnableMenuItem(IDM_SEARCHPREV, MF_ENABLED);
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	return 0;
}

// CmdFind:
// Invoke the search dialog.  Returns when the dialog is closed by the user.
//
void CMPFrame::CmdFind()
{
	ASSERT(m_pFindReplace == NULL);

	m_pFindReplace = new CFindReplaceDialog;
	if (m_pFindReplace->Create(TRUE, m_strFind, NULL,
			FR_HIDEWHOLEWORD | FR_DOWN) == FALSE)
	{
		delete m_pFindReplace;
		m_pFindReplace = NULL;
		return;
	}
	GetMenu()->EnableMenuItem(IDM_SEARCHFIND, MF_GRAYED);
}

// CmdFindPrev:
//
void CMPFrame::CmdFindPrev(void)
{
	ASSERT(m_strFind.GetLength() != 0);
	ASSERT(m_pActiveChild != NULL);

	if (m_pActiveChild->FindText(m_strFind, CMPChild::searchUp,
			bMatchCase) == FALSE)
		MPError(MB_OK | MB_ICONEXCLAMATION, IDS_CANTFIND, (LPCSTR)m_strFind);
	else
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

// CmdFindNext:
//
void CMPFrame::CmdFindNext(void)
{
	ASSERT(m_strFind.GetLength() != 0);
	ASSERT(m_pActiveChild != NULL);

	if (m_pActiveChild->FindText(m_strFind, CMPChild::searchDown,
			bMatchCase) == FALSE)
		MPError(MB_OK | MB_ICONEXCLAMATION, IDS_CANTFIND, (LPCSTR)m_strFind);
	else
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

/////////////////////////////////////////////////////////////////////////////
// Search Engine code
// This code is the workhorse code to search through the text buffer looking
// for a particular substring.

// RealSlowCompare:
// This is the brute-force method, which leaves a lot to be desired in
// performance.  However, this works fine for the typical text-file-sized
// buffer, which is currently limited by Windows to 32Kb anyway.
//
BOOL NEAR PASCAL RealSlowCompare(PSTR pSubject, LPSTR pTarget, 
		BOOL fCase = FALSE)
{
	if (fCase)
	{
		while (*pTarget)
		{
			if (*pTarget++ != *pSubject++)
				return FALSE;
		}
	}
	else
	{
		// If case-insensitive, convert both subject and target
		// to lowercase before comparing.
		//
		while (*pTarget)
		{
			if (::AnsiLower((LPSTR)(DWORD)*pTarget++) != 
				::AnsiLower((LPSTR)(DWORD)*pSubject++))
				return FALSE;
		}
	}
	return TRUE;
}

// FindText:
// Takes the szSearch buffer and tries to find it in the text buffer.
// The nDirection may be 1 for forward searches or -1 for backward searches.
//
BOOL CMPChild::FindText(LPCSTR lpszSearch, 
		int nDirection /* = searchDown */, 
		BOOL bMatchCase /* = FALSE */,
		BOOL bWholeWord /* = FALSE */)
{
	PSTR pText;
	HANDLE hText;
	LONG lSel;
	UINT cch;
	int i;
	
	if (*lpszSearch == '\0')
		return TRUE;
	
	// Find the current selection range.
	//
	lSel = m_edit.GetSel();
	
	// Get the handle to the text buffer and lock it.
	//
	hText = m_edit.GetHandle();
	pText = (PSTR)LocalLock(hText);
	
	// Get the length of the text.
	//
	cch = m_edit.GetWindowTextLength();
	
	// Start with the next char in selected range.
	//
	pText += LOWORD(lSel) + nDirection;
	
	// Compute how many characters are before/after the current selection.
	//
	if (nDirection < 0)
		i = LOWORD(lSel);
	else
		i = cch - LOWORD(lSel) + 1 - lstrlen(lpszSearch);
	
	// While there are uncompared substrings.
	//
	while (i > 0)
	{
		LOWORD(lSel) += nDirection;
	
		// Does this substring match?
		//
		if (RealSlowCompare(pText, (LPSTR)lpszSearch, bMatchCase))
		{
			// Unlock the buffer.
			//
			LocalUnlock(hText);
			
			// Select the located string.
			//
			HIWORD(lSel) = LOWORD(lSel) + lstrlen(lpszSearch);
			m_edit.SetSel(lSel);
			return TRUE;
		}
		i--;
		
		// Increment/decrement start position.
		//
		pText += nDirection;
	}
	
	// Not found... unlock buffer.
	//
	LocalUnlock(hText);
	
	return FALSE;
}
