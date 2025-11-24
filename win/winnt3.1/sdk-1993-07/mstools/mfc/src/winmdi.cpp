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

#include "winhand_.h"
#include "window_.h"

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWnd

IMPLEMENT_DYNAMIC(CMDIFrameWnd, CFrameWnd)

CMDIFrameWnd::CMDIFrameWnd()
{
	m_hWndMDIClient = NULL;
}

#ifdef _DEBUG
void CMDIFrameWnd::AssertValid() const
{
	CFrameWnd::AssertValid();
	ASSERT(m_hWndMDIClient == NULL || ::IsWindow(m_hWndMDIClient));
}
#endif

BEGIN_MESSAGE_MAP(CMDIFrameWnd, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int 
CMDIFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMenu* pMenu = GetMenu();
	// This is attempting to guess which sub-menu is the Window menu.
	// The Windows user interface guidelines say that the right-most
	// menu on the menu bar should be Help and Window should be one
	// to the left of that.
	int iMenu = pMenu->GetMenuItemCount() - 2;
	
	// If this assertion fails, your menu bar does not follow the guidelines
	// so you will have to override this function and call CreateClient
	// appropriately.
	ASSERT(iMenu >= 0);
	
	return CreateClient(lpCreateStruct, pMenu->GetSubMenu(iMenu)) ? 0 : -1;
}


BOOL 
CMDIFrameWnd::OnCommand(UINT wParam, LONG lParam)
{
	CWnd* pActiveChild = GetChildFrame();
	
	if (pActiveChild != this && _AfxCallWndProc(pActiveChild,
	  pActiveChild->m_hWnd, WM_COMMAND, wParam, lParam) != 0)
	{
		// handled by child
		return TRUE;
	}

	if (CFrameWnd::OnCommand(wParam, lParam))
	{
		// handled by us
		return TRUE;
	}

	if (LOWORD(lParam) == 0 && (wParam & 0xf000) == 0xf000)
	{
		// menu or accelerator within range of MDI children
		// default frame proc will handle it
		DefWindowProc(WM_COMMAND, wParam, lParam);
		return TRUE;
	}

	return FALSE;   // not handled
}


BOOL
CMDIFrameWnd::CreateClient(LPCREATESTRUCT /* lpCreateStruct */, 
	CMenu* pWindowMenu)
{
	ASSERT(m_hWnd != NULL);
	
	CLIENTCREATESTRUCT ccs;
	
	ccs.hWindowMenu = pWindowMenu->m_hMenu;
	ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;
	
	if ((m_hWndMDIClient = ::CreateWindowEx(0, "mdiclient", NULL, 
		WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, 0, 0, 0, 0, m_hWnd, NULL, 
		AfxGetInstanceHandle(), (LPSTR)(LPCLIENTCREATESTRUCT)&ccs)) == NULL)
	{
		TRACE("Warning: CMDIFrameWnd::CreateClient: failed to create MDICLIENT\n");
		return FALSE;
	}

	return TRUE;
}


CFrameWnd* 
CMDIFrameWnd::GetChildFrame()
{
	CFrameWnd* pActiveWnd = MDIGetActive();
	
	if (pActiveWnd != NULL)
		return pActiveWnd;
	
	return this;
}


LONG 
CMDIFrameWnd::DefWindowProc(UINT nMsg, UINT wParam, LONG lParam)
{
	return ::DefFrameProc(m_hWnd, m_hWndMDIClient, nMsg, wParam, lParam);
}


BOOL 
CMDIFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	CMDIChildWnd * pChildWnd = MDIGetActive();
	
	// current active child gets first crack at it
	if (pChildWnd != NULL && pChildWnd->PreTranslateMessage(pMsg))
		return TRUE;
	
	// translate accelerators for frame and any children
	if (m_hAccelTable != NULL &&
		::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
	{
		return TRUE;
	}
	
	// special processing for MDI accelerators last
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		// the MDICLIENT window may translate it
		if (::TranslateMDISysAccel(m_hWndMDIClient, pMsg))
			return TRUE;
	}

	return FALSE;
}

BOOL 
CMDIFrameWnd::Create(LPCSTR lpClassName,
	LPCSTR lpWindowName, DWORD dwStyle, const RECT& rect, 
	const CWnd* pParentWnd, LPCSTR lpMenuName)
{
	ASSERT(lpMenuName != NULL);

	if (lpClassName == NULL)
		lpClassName = _afxMDIFrameWnd;

	return CFrameWnd::Create(lpClassName,
		lpWindowName, dwStyle, rect, pParentWnd, lpMenuName);
}

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWnd

IMPLEMENT_DYNAMIC(CMDIChildWnd, CFrameWnd)

#ifdef _DEBUG
void 
CMDIChildWnd::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void 
CMDIChildWnd::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
	dc << "\nm_pMDIFrameWnd = " << (void *)m_pMDIFrameWnd;
}
#endif

LONG
CMDIChildWnd::DefWindowProc(UINT nMsg, UINT wParam, LONG lParam)
{
	return ::DefMDIChildProc(m_hWnd, nMsg, wParam, lParam);
}


BOOL
CMDIChildWnd::DestroyWindow()
{
	if (m_hWnd == NULL)
		return FALSE;
	MDIDestroy();
	return TRUE;
}

BOOL 
CMDIChildWnd::PreTranslateMessage(MSG* pMsg)
{
	// we can't call 'CFrameWnd::PreTranslate' since it will translate
	//  accelerators in the context of the MDI Child - but since MDI Child
	//  windows don't have menus this doesn't work properly.  MDI Child
	//  accelerators must be translated in context of their MDI Frame.

	return (m_hAccelTable != NULL &&
	  ::TranslateAccelerator(m_pMDIFrameWnd->m_hWnd, m_hAccelTable, pMsg));
}


BOOL 
CMDIChildWnd::Create(LPCSTR lpClassName,
	LPCSTR lpWindowName, DWORD dwStyle,
	const RECT& rect,
	CMDIFrameWnd* pParentWnd)
{
	MDICREATESTRUCT mcs;
	
	if (lpClassName == NULL)
		lpClassName = _afxFrameWnd;
	mcs.szClass = lpClassName;
	mcs.szTitle = lpWindowName;
	mcs.hOwner = AfxGetInstanceHandle();
	mcs.x = rect.left;
	mcs.y = rect.top;
	mcs.cx = rect.right - rect.left;
	mcs.cy = rect.bottom - rect.top;
	mcs.style = dwStyle;
	mcs.lParam = 0;
	
	if (pParentWnd == NULL)
	{
		CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
		ASSERT(pMainWnd != NULL);
		ASSERT(pMainWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
		pParentWnd = (CMDIFrameWnd*)pMainWnd;
	}
	
	m_pMDIFrameWnd = pParentWnd;
	
	// Restore the currently active MDI child if it is maximized since
	// Windows will do this anyway when this one is created and if we
	// wait until then several more messages go through our hook...
	BOOL bMaximized;
	CMDIChildWnd* pActiveMDIChild = pParentWnd->MDIGetActive(&bMaximized);
	if (bMaximized)
		pParentWnd->MDIRestore(pActiveMDIChild);

	_AfxHookWindowCreate(this);
	BOOL bReturn = (BOOL)::SendMessage(pParentWnd->m_hWndMDIClient, 
		WM_MDICREATE, 0, (LONG)(LPSTR)&mcs);
	_AfxUnhookWindowCreate();
	return bReturn;
}

CFrameWnd* 
CMDIChildWnd::GetParentFrame()
{
	return m_pMDIFrameWnd;
}

