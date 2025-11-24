// paredit.cpp: C++ derived edit control for numbers/letters etc
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

#include "ctrltest.h"

#include "paredit.h"

/////////////////////////////////////////////////////////////////////////////
// ParsedEdit

CParsedEdit::CParsedEdit()
{
	m_wParseStyle = 0;
}

BEGIN_MESSAGE_MAP(CParsedEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_VSCROLL()     // for associated spin controls
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Creating from C++ code

BOOL CParsedEdit::Create(DWORD dwStyle, const RECT& rect,
		CWnd* pParentWnd, UINT nID)
{
	m_wParseStyle = LOWORD(dwStyle);
	// figure out edit control style
	DWORD dwEditStyle = MAKELONG(ES_LEFT, HIWORD(dwStyle));
	return CWnd::Create("EDIT", NULL, dwEditStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// Aliasing on top of an existing Edit control

BOOL CParsedEdit::SubclassEdit(UINT nID, CWnd* pParent, WORD wParseStyle)
{
	m_wParseStyle = wParseStyle;
	HWND hWndEdit = ::GetDlgItem(pParent->m_hWnd, nID);
	if (hWndEdit == NULL)
		return FALSE;
	return SubclassWindow(hWndEdit);
}

/////////////////////////////////////////////////////////////////////////////
// Input character filter

void CParsedEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	WORD type;

	if (nChar < 0x20)
		type = PES_ALL;                         // always allow control chars
	else if (nChar >= '0' && nChar <= '9')
		type = PES_NUMBERS;
	else if (nChar >= 'A' && nChar <= 'Z')      // hard coded to english
		type = PES_LETTERS;
	else if (nChar >= 'a' && nChar <= 'z')
		type = PES_LETTERS;
	else
		type = PES_OTHERCHARS;

	if (m_wParseStyle & type)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);  // permitted
	}
	else
	{
		// illegal character - inform parent
		OnBadInput();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Spin controls will send scroll messages

void CParsedEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nDelta = 0;
	if (nSBCode == SB_LINEDOWN)
		nDelta = -1;
	else if (nSBCode == SB_LINEUP)
		nDelta = +1;
	else
		return; // nothing special

	// set the focus to this edit item and select it all
	SetFocus();

	//Get the number in the control.
	BOOL bOk;
	int nOld = GetParent()->GetDlgItemInt(GetDlgCtrlID(), &bOk);
	if (bOk)
	{
		// The MuScroll control also supports range checking
		// for this example, we just prevent overflow
		int nNew = nOld + nDelta;
		if (nNew >= 0 && nNew <= 32767)
			GetParent()->SetDlgItemInt(GetDlgCtrlID(), nNew);
		else
			bOk = FALSE;
	}

	if (!bOk)
		OnBadInput();
	SetSel(0, -1);
}

/////////////////////////////////////////////////////////////////////////////
// default bad input handler, beep (unless parent notification
//    returns -1.  Most parent dialogs will return 0 or 1 for command
//    handlers (i.e. Beep is the default)

void CParsedEdit::OnBadInput()
{
	// In the Win32 API, the WM_COMMAND message is packed differently.
	// When using the Microsoft Foundation Classes this is hardly noticed,
	// because of the ON_COMMAND macro and message maps.  If you need
	// to send a WM_COMMAND to a window, the packing is specific to the
	// target: Win32 or Win16...

#ifndef _NTWIN
	if (GetParent()->SendMessage(WM_COMMAND,
		GetDlgCtrlID(), MAKELONG(m_hWnd, PEN_ILLEGALCHAR)) != -1)
#else
	if (GetParent()->SendMessage(WM_COMMAND,
		MAKELONG(GetDlgCtrlID(), PEN_ILLEGALCHAR), (LPARAM)m_hWnd) != -1)
#endif
	{
		MessageBeep(-1);
	}
}

/////////////////////////////////////////////////////////////////////////////
