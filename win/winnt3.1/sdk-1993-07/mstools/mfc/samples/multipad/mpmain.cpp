// mpmain.cpp : Defines the class behaviors for the frame and children.
//              Multipad is a standard MDI application where each child
//              window is similar to the "Notepad" application.  This
//              example illustrates document-maintenance techniques such
//              as typical file Open and Save processing, printing, and
//              managing MDI document children windows.
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
#include <direct.h>
#include <stdarg.h>

#ifndef _NTWIN
#pragma code_seg("_MPTEXT")
#endif

// a simple way to reduce size of C runtimes
// disable the use of getenv and argv/argc
extern "C" void _setargv() { }
extern "C" void _setenvp() { }

// MRU information
static CString mruFileNames [4];

/////////////////////////////////////////////////////////////////////////////
// The one global application object.
//
CMultiPad multiPad("MultiPad");


/////////////////////////////////////////////////////////////////////////////
// MPError:
// A useful printf()-style error routine, which formats and displays a
// message box.  Note that "idFmt" is a resource ID, which should be an
// fprintf()-style format string.
//
short MPError(int bFlags, int idFmt, ...)
{
	char sz[160];
	CString strFmt;
	
	strFmt.LoadString(idFmt);
	va_list argList;
	va_start(argList, idFmt);
	wvsprintf(sz, (LPCSTR)strFmt, argList);
	va_end(argList);
	
	return multiPad.m_pMainWnd->MessageBox(sz, AfxGetAppName(), bFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CMPFrame

CMPFrame::CMPFrame(const char* szTitle) : m_statBar()
{
	m_bShortMenu = FALSE;
	m_pActiveChild = NULL;
	Create(NULL, szTitle, WS_OVERLAPPEDWINDOW, rectDefault, NULL,
		MAKEINTRESOURCE(IDMULTIPAD));
}

// OnMenuSelect:
// As the user highlights different menu items, we are called.  We decide
// which string would be appropriate to put in our status bar.
//
// Note some OnMenuSelect messages go to the active child; we just ask the
// frame to handle these.
//
void CMPChild::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	GetParentFrame()->SendMessage(WM_MENUSELECT, nItemID,
		MAKELONG(nFlags, hSysMenu));
}
void CMPFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	char szBuf [128];
	
	UINT ids = 0;
	
	if (nFlags == 0xFFFF && hSysMenu == NULL)
	{
		ids = IDS_ALTPMT;
	}
	else if (nFlags & MF_POPUP)
	{
		ids = IDS_MENUPMT;
	}
	else if ((nFlags & MF_SEPARATOR) == 0)
	{
		if (nItemID >= AFX_IDM_FIRST_MDICHILD)
			ids = IDS_ACTTHISWIN;
		else if (nItemID >= IDM_FILE1 && nItemID <= IDM_FILE4)
			ids = IDS_OPENTHISFILE;
		else
			ids = nItemID;
	}
	
	if (ids != 0)
		LoadString(AfxGetResourceHandle(), ids, szBuf, sizeof (szBuf));
	else
		szBuf[0] = '\0';
	
	m_statBar.SetText(szBuf);
}

// OnCreate:
// When we're created, in addition to the usual CMDIFrameWnd::OnCreate work,
// we create a CStatBar status bar window (defined in bar.h and bar.cpp).
//
int CMPFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	char szBuf [128];

	// Create the status bar, with the usual "Press ALT to choose commands".
	//
	m_statBar.Create(this, CRect(0, 0, 0, 0));
	LoadString(AfxGetResourceHandle(), IDS_ALTPMT, szBuf, sizeof (szBuf));
	m_statBar.SetText(szBuf);

	return CMDIFrameWnd::OnCreate(lpCreateStruct);
}

// OnSize:
// The Windows 3.0 SDK documentation says that, in the case of an MDI Frame
// window, the ON_SIZE message MUST be passed on to the system after any
// processing.  This sizes the MDI Client window to the whole client area
// of the frame.
//
// Note that instead, you can simply use the MoveWindow function to move the
// MDI Client window yourself.  In fact, you must do this yourself if you
// don't want the MDI Client to take up the whole Frame client area.  If you
// use MoveWindow, do not pass the ON_SIZE message on for default processing.
//
// In our case, we leave some room at the bottom for a status bar.
//
void CMPFrame::OnSize(UINT size, int cx, int cy)
{
	if (size != SIZEICONIC)
	{
		int cxBorder = GetSystemMetrics(SM_CXBORDER);
		int cyBorder = GetSystemMetrics(SM_CYBORDER);
		CRect rcStatBar;
		int cyStatBar;
		
		m_statBar.GetWindowRect(rcStatBar);
		cyStatBar = rcStatBar.bottom - rcStatBar.top;
		HDWP hdwp = ::BeginDeferWindowPos(2);
		::DeferWindowPos(hdwp, m_statBar.m_hWnd, NULL, -cxBorder,
			cy - cyStatBar + cyBorder, cx + cxBorder * 2, cyStatBar,
			SWP_NOACTIVATE | SWP_NOZORDER);
		::DeferWindowPos(hdwp, m_hWndMDIClient, NULL, 0, 0,
			cx, cy - cyStatBar + cyBorder, SWP_NOZORDER | SWP_NOACTIVATE);
		::EndDeferWindowPos(hdwp);
	}
}

// OnInitMenu:
// The menu needs to be updated, so we observe our status and gray or check
// any appropriate menu items.
//

BOOL bUpdateMRU = TRUE;

void CMPFrame::OnInitMenu(CMenu* pMenu)
{
	extern char szSearch[];
	
	UINT nStatus;
	int i;

	if (IsIconic())
		return;

	if (bUpdateMRU)
	{
		BOOL bMaximized = HIWORD(::SendMessage(m_hWndMDIClient,
			WM_MDIGETACTIVE, 0, 0));
		CMenu * pFileMenu = pMenu->GetSubMenu(bMaximized ? 1 : 0);
		
		if (mruFileNames[0].GetLength() > 0)
		{
			char szCurDir [64];
			
			pFileMenu->DeleteMenu(IDM_FILEMRU, MF_BYCOMMAND);
			pFileMenu->DeleteMenu(IDM_FILE1, MF_BYCOMMAND);
			pFileMenu->DeleteMenu(IDM_FILE2, MF_BYCOMMAND);
			pFileMenu->DeleteMenu(IDM_FILE3, MF_BYCOMMAND);
			pFileMenu->DeleteMenu(IDM_FILE4, MF_BYCOMMAND);

			_getcwd(szCurDir, sizeof (szCurDir));
			int cchCurDir = strlen(szCurDir);
			if (szCurDir[cchCurDir - 1] != '\\')
			{
				szCurDir[cchCurDir++] = '\\';
				szCurDir[cchCurDir] = '\0';
			}
			
			for (i = 0; i < 4; i += 1)
			{
				char szBuf [128];
				char* pch;
				
				if (mruFileNames[i].GetLength() == 0)
					break;
				
				pch = szBuf;
				*pch++ = '&';
				*pch++ = '1' + i;
				*pch++ = ' ';
				strcpy(pch, (const char*)mruFileNames[i]);
				if (strncmp(szCurDir, (const char*)mruFileNames[i],
					cchCurDir) == 0)
				{
					strcpy(pch, pch + cchCurDir);
				}
									
				pFileMenu->InsertMenu(IDM_FILEEXIT, MF_STRING | MF_BYCOMMAND,
					IDM_FILE1 + i, szBuf);
			}
			
			pFileMenu->InsertMenu(IDM_FILEEXIT, MF_SEPARATOR, IDM_FILEMRU);
		}
		
		pFileMenu->Detach();
		bUpdateMRU = FALSE;
	}
	
	if (m_pActiveChild != NULL)
	{
		if (m_pActiveChild->m_edit.CanUndo())
			nStatus = MF_ENABLED;
		else
			nStatus = MF_GRAYED;
		
		pMenu->EnableMenuItem(IDM_EDITUNDO, nStatus);
		
		LONG lSel = m_pActiveChild->m_edit.GetSel();
		nStatus = (HIWORD(lSel) == LOWORD(lSel)) ? MF_GRAYED : MF_ENABLED;
		pMenu->EnableMenuItem(IDM_EDITCUT, nStatus);
		pMenu->EnableMenuItem(IDM_EDITCOPY, nStatus);
		pMenu->EnableMenuItem(IDM_EDITCLEAR, nStatus);
		
		nStatus = MF_GRAYED;
		if (OpenClipboard())
		{
			int nFmt = 0;
			
			while ((nFmt = EnumClipboardFormats(nFmt)) != 0)
			{
				if (nFmt == CF_TEXT)
				{
					nStatus = MF_ENABLED;
					break;
				}
			}
			
			CloseClipboard();
		}
		pMenu->EnableMenuItem(IDM_EDITPASTE, nStatus);
		
		if (m_pActiveChild->m_bWordWrap)
			nStatus = MF_CHECKED;
		else
			nStatus = MF_UNCHECKED;
		pMenu->CheckMenuItem(IDM_EDITWRAP, nStatus);

		if (m_pFindReplace != NULL)
			pMenu->EnableMenuItem(IDM_SEARCHFIND, MF_GRAYED);
		else
			pMenu->EnableMenuItem(IDM_SEARCHFIND, MF_ENABLED);

		if (m_strFind.GetLength() == 0)
			nStatus = MF_GRAYED;
		else
			nStatus = MF_ENABLED;
		pMenu->EnableMenuItem(IDM_SEARCHNEXT, nStatus);
		pMenu->EnableMenuItem(IDM_SEARCHPREV, nStatus);

		// Select All and Wrap toggle always enabled.
		//
		nStatus = MF_ENABLED;
		pMenu->EnableMenuItem(IDM_EDITSELECT, nStatus);
		pMenu->EnableMenuItem(IDM_EDITWRAP, nStatus);
		pMenu->EnableMenuItem(IDM_EDITSETFONT, nStatus);
	}
	else
	{
		nStatus = MF_GRAYED;
		
		for (i = IDM_EDITFIRST; i <= IDM_EDITLAST; i += 1)
			pMenu->EnableMenuItem(i, nStatus);
		
		pMenu->CheckMenuItem(IDM_EDITWRAP, MF_UNCHECKED);

		pMenu->EnableMenuItem(IDM_SEARCHFIND, nStatus);
		pMenu->EnableMenuItem(IDM_SEARCHNEXT, nStatus);
		pMenu->EnableMenuItem(IDM_SEARCHPREV, nStatus);

		pMenu->EnableMenuItem(IDM_FILEPRINT, nStatus);
	}

	// nStatus is last value in m_pActiveChild test
	pMenu->EnableMenuItem(IDM_FILEPRINT, nStatus);
	pMenu->EnableMenuItem(IDM_FILESAVE, nStatus);
	pMenu->EnableMenuItem(IDM_FILESAVEAS, nStatus);
	pMenu->EnableMenuItem(IDM_WINDOWTILE, nStatus);
	pMenu->EnableMenuItem(IDM_WINDOWCASCADE, nStatus);
	pMenu->EnableMenuItem(IDM_WINDOWICONS, nStatus);
	pMenu->EnableMenuItem(IDM_WINDOWCLOSEALL, nStatus);
	
}

void AddFileToMRU(const char* szFileName)
{
	int i;
	
	bUpdateMRU = TRUE;
	
	for (i = 0; i < 4; i += 1)
	{
		if (szFileName == mruFileNames[i])
		{
			if (i != 0)
			{
				CString temp = mruFileNames[0];
				mruFileNames[0] = mruFileNames[i];
				mruFileNames[i] = temp;
			}
			return;
		}
	}

	for (i = 3; i > 0; i -= 1)
		mruFileNames[i] = mruFileNames[i - 1];
	mruFileNames[0] = szFileName;
}


/////////////////////////////////////////////////////////////////////////////
// File menu commands

void CMPFrame::CmdFileNew()
{
	new CMPChild(NULL);
}

void CMPFrame::CmdFileOpen()
{
	ReadFile();
}

void CMPFrame::CmdWinCloseAll()
{
	if (QueryCloseAllChildren())
	{
		CloseAllChildren();
		::ShowWindow(m_hWndMDIClient, SW_SHOW);
	}
}

BOOL CMPFrame::QueryCloseAllChildren()
{
	CWnd* pWnd;
	
	for (pWnd = CWnd::FromHandle(::GetWindow(m_hWndMDIClient, GW_CHILD)); pWnd != NULL;
		pWnd = pWnd->GetNextWindow())
	{
		if (pWnd->GetWindow(GW_OWNER) != NULL)
			continue;
		
		if (pWnd->SendMessage(WM_QUERYENDSESSION))
			return FALSE;
	}
	
	return TRUE;
}

void CMPFrame::CloseAllChildren()
{
	CWnd* pWnd;
	
	// Hide the MDI client window to avoid multiple repaints.
	//
	::ShowWindow(m_hWndMDIClient, SW_HIDE);
	
	// As long as the MDI client has a child, destroy it.
	//
	while ((pWnd = CWnd::FromHandle(m_hWndMDIClient)->GetWindow(GW_CHILD)) != NULL)
	{
		// Skip the icon title windows.
		//
		while (pWnd != NULL && pWnd->GetWindow(GW_OWNER) != NULL)
			pWnd = pWnd->GetNextWindow();
		
		if (pWnd == NULL)
			break;
		
		pWnd->DestroyWindow();
	}
}

BOOL CMPFrame::OnQueryEndSession()
{
	return QueryCloseAllChildren();
}

void CMPFrame::OnClose()
{
	if (QueryCloseAllChildren())
	{
		if (m_pFindReplace != NULL)
		{
			// Do not delete, use DestroyWindow
			m_pFindReplace->SendMessage(WM_COMMAND, IDCANCEL, 0L);
		}
		this->DestroyWindow();
	}
}

void CMPFrame::CmdMDITile()
{
	MDITile();
}

void CMPFrame::CmdMDICascade()
{
	MDICascade();
}

void CMPFrame::CmdMDIIconArrange()
{
	MDIIconArrange();
}

void CMPFrame::CmdFileExit()
{
	OnClose();
}

// CmdToggleMenu:
// Switch between "Full" and "Short" menus.
//
void CMPFrame::CmdToggleMenu()
{
	CMenu menu;
	UINT id, i;
	
	if (m_bShortMenu)
	{
		id = IDMULTIPAD;
		i = WINDOWMENU;
		m_bShortMenu = FALSE;
	}
	else
	{
		id = IDMULTIPAD2;
		i = SHORTMENU;
		m_bShortMenu = TRUE;
	}
	
	menu.LoadMenu(id);
	MDISetMenu(&menu, menu.GetSubMenu(i))->DestroyMenu();
	menu.Detach(); // Keep it from being destroyed.
	DrawMenuBar();

	bUpdateMRU = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Handle profile stuff
static char BASED_CODE szIniFile[] = "multipad.ini";
void LoadMRU()
{
	char szBuf [128];
	
	GetPrivateProfileString(AfxGetAppName(), "File1", "", 
		szBuf, sizeof (szBuf), szIniFile);
	mruFileNames[0] = szBuf;

	GetPrivateProfileString(AfxGetAppName(), "File2", "", 
		szBuf, sizeof (szBuf), szIniFile);
	mruFileNames[1] = szBuf;

	GetPrivateProfileString(AfxGetAppName(), "File3", "", 
		szBuf, sizeof (szBuf), szIniFile);
	mruFileNames[2] = szBuf;

	GetPrivateProfileString(AfxGetAppName(), "File4", "", 
		szBuf, sizeof (szBuf), szIniFile);
	mruFileNames[3] = szBuf;

	bUpdateMRU = TRUE;
}

void SaveMRU()
{
	WritePrivateProfileString(AfxGetAppName(), "File1", 
		mruFileNames[0], szIniFile);
	WritePrivateProfileString(AfxGetAppName(), "File2", 
		mruFileNames[1], szIniFile);
	WritePrivateProfileString(AfxGetAppName(), "File3", 
		mruFileNames[2], szIniFile);
	WritePrivateProfileString(AfxGetAppName(), "File4", 
		mruFileNames[3], szIniFile);
}

void CMPFrame::CmdFileMRU()
{
	int mruIndex = (GetCurrentMessage()->wParam) - IDM_FILE1;
	ReadFile(mruFileNames[mruIndex]);
}

/////////////////////////////////////////////////////////////////////////////
// Help menu commands

void CMPFrame::CmdHelpAbout()
{
	CModalDialog aboutBox(IDD_ABOUT, this);
	aboutBox.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMPFrame message map
BEGIN_MESSAGE_MAP(CMPFrame, CMDIFrameWnd)
	ON_WM_INITMENU()
	ON_WM_MENUSELECT()
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_SIZE()
	ON_WM_CREATE()
		
	ON_COMMAND(IDM_FILENEW, CmdFileNew)
	ON_COMMAND(IDM_FILEOPEN, CmdFileOpen)
	ON_COMMAND(IDM_FILEMENU, CmdToggleMenu)
	ON_COMMAND(IDM_FILE1, CmdFileMRU)
	ON_COMMAND(IDM_FILE2, CmdFileMRU)
	ON_COMMAND(IDM_FILE3, CmdFileMRU)
	ON_COMMAND(IDM_FILE4, CmdFileMRU)
	ON_COMMAND(IDM_FILEEXIT, CmdFileExit)
	
	ON_COMMAND(IDM_WINDOWTILE, CmdMDITile)
	ON_COMMAND(IDM_WINDOWCASCADE, CmdMDICascade)
	ON_COMMAND(IDM_WINDOWICONS, CmdMDIIconArrange)
	ON_COMMAND(IDM_WINDOWCLOSEALL, CmdWinCloseAll)

	ON_COMMAND(IDM_HELPABOUT, CmdHelpAbout)

	// Find stuff
	ON_REGISTERED_MESSAGE(m_nMsgFind, CmdFindHelper) 
			/* helper for CFindReplace */

	ON_COMMAND(IDM_SEARCHFIND, CmdFind)
	ON_COMMAND(IDM_SEARCHNEXT, CmdFindNext)
	ON_COMMAND(IDM_SEARCHPREV, CmdFindPrev)

END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
// CMPChild routines
int CMPChild::OnCreate(LPCREATESTRUCT)
{
	CRect rect(0, 0, 0, 0);

	m_edit.Create(WS_BORDER | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL |
		WS_MAXIMIZE, rect, this, ID_EDIT);
	
	m_bChanged = FALSE;
	m_bWordWrap = FALSE;
	m_bUntitled = TRUE;
	m_edit.SetFocus();

	return 0;
}

void CMPChild::OnSize(UINT nFlags, int cx, int cy)
{
	CRect rc;
	
	GetClientRect(&rc);
	rc.InflateRect(GetSystemMetrics(SM_CXBORDER),
		GetSystemMetrics(SM_CYBORDER));
	m_edit.MoveWindow(rc);
	
	// This MUST be passed along for MDI to work properly.  It handles all
	// of the maximize, minimize, restore logic.
	//
	CMDIChildWnd::OnSize(nFlags, cx, cy);
}

// CMPChild:
// We keep tabs on who the active child is.
//
void CMPChild::OnMDIActivate(BOOL bActivate, CWnd* pWndActivate,
	CWnd* pWndDeactivate)
{
	if (bActivate)
		CMPFrame::SetActiveChild((CMPChild*)pWndActivate);
	else if (CMPFrame::GetActiveChild() == pWndDeactivate)
		CMPFrame::SetActiveChild(NULL);
}

// OnEditChange:
// The user's been editing the buffer.  Remember that it no longer matches
// the original disk file.
//
void CMPChild::OnEditChange()
{
	m_bChanged = TRUE;
}

// OnEditErrSpace:
// No more room in this buffer!  Windows limits the size of the CEdit's text
// to 32Kb.
//
void CMPChild::OnEditErrSpace()
{
	MessageBeep(0);
}

BOOL CMPChild::OnQueryEndSession()
{
	return !QueryCloseChild();
}

// OnSetFocus:
// Whenever a child gets the focus, it gives it to its child CEdit instead.
//
void CMPChild::OnSetFocus(CWnd* pWndOldFocus)
{
	m_edit.SetFocus();
	CMDIChildWnd::OnSetFocus(pWndOldFocus);
}

// SetWrap:
// Edit windows are difficult to wrap/unwrap the text within.  This code
// uses a temporary edit window scheme, transferring the old text into
// a new window to make it wrap properly.
//
void CMPChild::SetWrap(BOOL bWrap)
{
	LONG    dws;
	HANDLE  hText;
	HANDLE  hDummyText;

	// Change word wrap mode.
	//
	m_bWordWrap = bWrap;

	// Create the appropriate window style, adding a horizontal scroll
	// facility if wrapping is not present.
	//
	dws = WS_BORDER | WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | 
			ES_NOHIDESEL | ES_MULTILINE;
	if (!bWrap)
		dws |= WS_HSCROLL | ES_AUTOHSCROLL;

	// Get the data handle of the old control.
	//
	hText = m_edit.GetHandle();

	// Create a dummy data handle and make it the handle to
	// the old edit control (hText still references the text of
	// old control).
	//
	hDummyText = LocalAlloc(LHND, 0);
	m_edit.SetHandle(hDummyText);
	m_edit.DestroyWindow();

	// Create a new child window.
	//
	CRect rc(0, 0, 0, 0);
	m_edit.Create(dws, rc, this, ID_EDIT);

	// Cause the window to be properly sized.
	//
	SendMessage(WM_SIZE, 0, 0L);

	// Free the new window's old data handle and set it to
	// hText (text of old edit control).
	//
//  LocalFree(m_edit.GetHandle());
	m_edit.SetHandle(hText);
	m_edit.SetFont(&m_font);

	m_edit.ShowWindow(SW_SHOW);

	// Set focus to the new edit control.
	m_edit.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// File menu commands

void CMPChild::CmdFileSave()
{
	if (m_bUntitled && !ChangeFile())
		return;
	
	SaveFile();
}

void CMPChild::CmdFileSaveAs()
{
	if (ChangeFile())
		SaveFile();
}

void CMPChild::OnClose()
{
	if (QueryCloseChild())
		CMDIChildWnd::OnClose();
}

void CMPChild::PostNcDestroy()
{
	delete this;
}

BOOL CMPChild::QueryCloseChild()
{
	char sz[64];
	
	// Return OK if text has not changed.
	//
	if (!m_bChanged)
		return TRUE;

	GetWindowText(sz, sizeof (sz));
	
	// Ask user whether to save, not save, or cancel.
	//
	switch (MPError(MB_YESNOCANCEL | MB_ICONQUESTION, IDS_CLOSESAVE,
		(LPCSTR)sz))
	{
	case IDYES:
		// User wants file saved.
		//
		SaveFile();
		break;

	case IDNO:
		// User doesn't want file saved.  OK to close child.
		//
		break;

	default:
		// We couldn't do the message box, or not OK to close.
		//
		return FALSE;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Edit menu commands

void CMPChild::CmdSetFont()
{
	LOGFONT lfInitial;

	if (m_edit.GetFont() == NULL)
	{
		// using SystemFont
		CFont sysFont;
		VERIFY(sysFont.CreateStockObject(SYSTEM_FONT));
		VERIFY(sysFont.GetObject(sizeof(lfInitial), &lfInitial));
	}
	else
		m_edit.GetFont()->GetObject(sizeof(lfInitial), &lfInitial);

	CFontDialog fontDialog(&lfInitial, CF_SCREENFONTS);
	
	if (fontDialog.DoModal() == IDOK)
	{
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&fontDialog.m_lf);
		m_edit.SetFont(&m_font);
	}
}

void CMPChild::CmdWordWrap()
{
	SetWrap(!m_bWordWrap);
}

void CMPChild::CmdUndo()
{
	m_edit.Undo();
}

void CMPChild::CmdCut()
{
	m_edit.Cut();
}

void CMPChild::CmdCopy()
{
	m_edit.Copy();
}

void CMPChild::CmdPaste()
{
	m_edit.Paste();
}

void CMPChild::CmdClear()
{
	m_edit.ReplaceSel("");
}

void CMPChild::CmdSelectAll()
{
	m_edit.SetSel(MAKELONG(0, 0xE000));
}

/////////////////////////////////////////////////////////////////////////////
// CMPChild message map
BEGIN_MESSAGE_MAP(CMPChild, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_MDIACTIVATE()
	ON_WM_QUERYENDSESSION()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_MENUSELECT()
	
	ON_EN_CHANGE(ID_EDIT, OnEditChange)
	
	ON_COMMAND(IDM_FILESAVE, CmdFileSave)
	ON_COMMAND(IDM_FILESAVEAS, CmdFileSaveAs)
	ON_COMMAND(IDM_FILEPRINT, PrintFile)
	ON_COMMAND(IDM_EDITWRAP, CmdWordWrap)
	ON_COMMAND(IDM_EDITUNDO, CmdUndo)
	ON_COMMAND(IDM_EDITCUT, CmdCut)
	ON_COMMAND(IDM_EDITCOPY, CmdCopy)
	ON_COMMAND(IDM_EDITPASTE, CmdPaste)
	ON_COMMAND(IDM_EDITCLEAR, CmdClear)
	ON_COMMAND(IDM_EDITSETFONT, CmdSetFont)
	ON_COMMAND(IDM_EDITSELECT, CmdSelectAll)
END_MESSAGE_MAP()


int CMultiPad::ExitInstance()
{
	extern CPrinter* thePrinter;
	delete thePrinter;

	SaveMRU();
	return CWinApp::ExitInstance();
}
