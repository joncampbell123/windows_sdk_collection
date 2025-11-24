// multipad.h : Defines the class interfaces for the frame and child.
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

#ifndef __MULTIPAD_H__
#define __MULTIPAD_H__

/////////////////////////////////////////////////////////////////////////////

#define EXPORT _export

#include <afxwin.h>
#include <afxdlgs.h>
#include "bar.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////

CDC* GetPrinterDC();
class CMPChild;

class CMultiPad : public CWinApp
{
public:
	CMultiPad(const char* pAppName) : CWinApp(pAppName)
		{ }
	BOOL InitInstance();
	int ExitInstance();
};

class CMPFrame : public CMDIFrameWnd
{
protected:
	class CMPChild* m_pActiveChild;

public:
	CMPFrame() {};
	CMPFrame(const char* szTitle);
	
	BOOL QueryCloseAllChildren();
	void CloseAllChildren();
	void GetInitializationData();
	CMPChild* AlreadyOpen(char* szFile);
	void ReadFile(const char* szFile);

	static CMPFrame* GetMDIFrameWnd() 
		{ return (CMPFrame*)(AfxGetApp()->m_pMainWnd); }

	static CMPChild* GetActiveChild() 
		{ return ((CMPFrame*)(AfxGetApp()->m_pMainWnd))->m_pActiveChild; }
	static void SetActiveChild(CMPChild* pActiveChild) 
		{ GetMDIFrameWnd()->m_pActiveChild = pActiveChild; }

protected:
	void ReadFile();

	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnClose();
	afx_msg BOOL OnQueryEndSession();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	
	afx_msg void CmdFileNew();
	afx_msg void CmdFileOpen();
	afx_msg void CmdFileExit();
	afx_msg void CmdToggleMenu();
	afx_msg void CmdFileMRU();
	
	afx_msg void CmdMDITile();
	afx_msg void CmdMDICascade();
	afx_msg void CmdMDIIconArrange();
	afx_msg void CmdWinCloseAll();
	
	afx_msg void CmdHelpAbout();

	afx_msg void CmdFind();
	afx_msg void CmdFindPrev();
	afx_msg void CmdFindNext();
	
	DECLARE_MESSAGE_MAP()

	BOOL m_bShortMenu;
	CStatBar m_statBar;

	// Search helpers
	afx_msg LONG CmdFindHelper(UINT wParam, LONG lParam); // for m_nMsgFind
	static UINT m_nMsgFind;
	static CString m_strFind;
	static CFindReplaceDialog* m_pFindReplace;
};

class CMPChild : public CMDIChildWnd
{
public:
	BOOL m_bChanged;
	BOOL m_bUntitled;
	BOOL m_bWordWrap;
	CEdit m_edit;
	CFont m_font;
	
	CMPChild() : m_font(), m_edit() 
		{ } 
	CMPChild(char* szName);
	
	BOOL QueryCloseChild();
	void SaveFile();
	BOOL ChangeFile();
	void PrintFile();
	
	int LoadFile(char* szFile);
	
	void SetWrap(BOOL bWrap);
	
	// Find engine stuff
	enum { searchUp = -1, searchDown = +1};
	BOOL FindText(LPCSTR szSearch,
		int nDirection = searchDown, 
		BOOL bMatchCase = FALSE, 
		BOOL bWholeWord = FALSE); // does the actual search

protected:
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivate, CWnd* pDeactivate);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pWnd);
	afx_msg void OnEditChange();
	afx_msg void OnEditErrSpace();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	
	virtual void PostNcDestroy();

	// Command handlers
	afx_msg void CmdSetFont();
	afx_msg void CmdFileSave();
	afx_msg void CmdFileSaveAs();
	afx_msg void CmdUndo();
	afx_msg void CmdCut();
	afx_msg void CmdCopy();
	afx_msg void CmdPaste();
	afx_msg void CmdClear();
	afx_msg void CmdSelectAll();
	afx_msg void CmdWordWrap();

	DECLARE_MESSAGE_MAP()
};

class CPrinter
{
private:
	HWND hwndPDlg;      // Handle to the cancel print dialog
	HANDLE hInitData;   // handle to initialization data
	
	class CPrintCanDlg* pdlg;
	UINT fError;
	
public:
	BOOL fAbort;        // TRUE if the user has aborted the print job
	CPrintDialog printDlg;
	CDC* pdc;
	
	char szTitle [32];  // Global pointer to job title
	
	CPrinter() : printDlg(FALSE) {};
				
	BOOL        StartJob(char* szDocName);
	void        EndJob();
	
	friend BOOL FAR PASCAL EXPORT AbortProc(HDC, int);
};

short MPError(int bFlags, int id, ...);

/////////////////////////////////////////////////////////////////////////////

#endif // __MULTIPAD_H__

