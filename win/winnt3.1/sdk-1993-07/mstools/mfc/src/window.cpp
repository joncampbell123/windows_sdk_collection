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
#include "trace_.h"
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CHandleMap implementation

CObject*
CHandleMap::FromHandle(HANDLE h)
{
	if (h == NULL)
		return NULL;

	void* p;
	if (LookupPermanent(h, p))
		return (CObject*)p;     // return permanent one
	else if (LookupTemporary(h, p))
		return (CObject*)p;     // return last temporary one

	// This handle wasn't created by us, so we must create a temporary
	// C++ object to wrap it.  We don't want the user to see this memory
	// allocation, so we turn tracing off.
#ifdef _DEBUG
	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
#endif
	CObject* pTemp = NewTempObject(h);
#ifdef _NTWIN
	temporaryMap.SetAt((LPVOID)h, pTemp);
#else
	temporaryMap.SetAt((WORD)h, pTemp);
#endif
#ifdef _DEBUG
	AfxEnableMemoryTracking(bEnable);
#endif
	return pTemp;
}

void
CHandleMap::DeleteTemp()
{
	POSITION pos = temporaryMap.GetStartPosition();
	while (pos != NULL)
	{
#ifdef _NTWIN
		LPVOID h; // not used 
#else
		WORD h; // not used 
#endif
		void* p;
		temporaryMap.GetNextAssoc(pos, h, p);
		DeleteTempObject((CObject*)p);      // free it up
	}

	temporaryMap.RemoveAll();       // free up dictionary links etc
}

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CButton, CWnd)
IMPLEMENT_DYNAMIC(CListBox, CWnd)
IMPLEMENT_DYNAMIC(CComboBox, CWnd)

IMPLEMENT_DYNAMIC(CWnd, CObject)


char NEAR _afxWnd[] = "AfxWnd";
char NEAR _afxMDIFrameWnd[] = "AfxMDIFrameWnd";
char NEAR _afxFrameWnd[] = "AfxFrameWnd";


/////////////////////////////////////////////////////////////////////////////
// Special bootstrap globals

static CWnd* pWndInit = NULL;
static CDialog* pDlgInit = NULL;

/////////////////////////////////////////////////////////////////////////////
// Official way to send message to a CWnd

struct CLastState
{
	MSG msg;
};
static CLastState NEAR lastState;
	// global for last state of call to 'WindowProc'

LONG _AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT message,
	UINT wParam, LONG lParam)
{
	LONG lResult;
	CLastState oldState = lastState;    // save for nesting

	lastState.msg.hwnd = hWnd;
	lastState.msg.message = message;
	lastState.msg.wParam = wParam;
	lastState.msg.lParam = lParam;

#ifdef _DEBUG
	if (afxTraceFlags & 4)
		AfxTraceMsg("WndProc", &lastState.msg);
#endif

	// Catch exceptions thrown outside the scope of a callback
	// in debug builds and warn the user.
	TRY
	{
		lResult = pWnd->WindowProc(message, wParam, lParam);
	}
	CATCH (CException, e)
	{
		if (message == WM_CREATE)
			lResult = -1;
		else
			lResult = 0;
		TRACE("Warning: Uncaught exception in WindowProc (returning %ld)\n",
			lResult);
		ASSERT(FALSE);
	}
	END_CATCH

	lastState = oldState;
	return lResult;
}

const MSG* CWnd::GetCurrentMessage()
{
	// fill in time and position when asked for
	lastState.msg.time = ::GetMessageTime();
	*((DWORD*)&lastState.msg.pt) = ::GetMessagePos();
	return &lastState.msg;
}

LONG CWnd::Default()
	// call DefWindowProc with the last message
{
	return DefWindowProc(lastState.msg.message,
			lastState.msg.wParam, lastState.msg.lParam);
}

/////////////////////////////////////////////////////////////////////////////
// Map from HWND to CWnd*

class NEAR CWndHandleMap : public CHandleMap
{
public:
	CObject* NewTempObject(HANDLE h)
				{
					// don't add in permanent
					CWnd* p = new CWnd();
					p->m_hWnd = (HWND)h;      // set after constructed
					return p;
				}
	void DeleteTempObject(CObject* ob)
				{
					ASSERT(ob->IsKindOf(RUNTIME_CLASS(CWnd)));
					((CWnd*)ob)->m_hWnd = NULL; // clear before destructed
					delete ob;
				}
	CWndHandleMap()
		{ }
};
static CWndHandleMap NEAR hWndMap;

CWnd*
CWnd::FromHandle(HWND hWnd)
{
	return (CWnd*)hWndMap.FromHandle(hWnd);
}

CWnd*
CWnd::FromHandlePermanent(HWND hWnd)
{
	// only look in the permanent map - does no allocations
	void* p;
	return (hWndMap.LookupPermanent(hWnd, p)) ? (CWnd*)p : NULL;
}

void
CWnd::DeleteTempMap()
{
	hWndMap.DeleteTemp();
}

BOOL
CWnd::Attach(HWND hWnd)
{
	ASSERT(m_hWnd == NULL);     // only attach once, detach on destroy
	if (hWnd == NULL)
		return FALSE;
	hWndMap.SetPermanent(m_hWnd = hWnd, this);
	return TRUE;
}

HWND
CWnd::Detach()
{
	HWND hWnd;
	if ((hWnd = m_hWnd) != NULL)
		hWndMap.RemovePermanent(m_hWnd);
	m_hWnd = NULL;
	return hWnd;
}


/////////////////////////////////////////////////////////////////////////////
// One main WndProc for all CWnd's and derived classes

LONG FAR PASCAL AFX_EXPORT
AfxWndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)
{
	register CWnd* pWnd;

	pWnd = CWnd::FromHandlePermanent(hWnd);
	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd == hWnd);

	LONG lResult = _AfxCallWndProc(pWnd, hWnd, message, wParam, lParam);

	return lResult;
}

// Special case for remaining dialog cases
// Most messages will go through the window proc (AfxWndProc) of the
//   subclassed dialog.  Some messages like WM_SETFONT and WM_INITDIALOG
//   are sent directly to the dialog proc only.  These messages cannot be
//   passed on to DefWindowProc() or infinite recursion will result!
// In responding to these messages, you shouldn't call the Default handler
LONG FAR PASCAL AFX_EXPORT
_AfxDlgProc(HWND hWnd, register UINT message, UINT wParam, LONG lParam)
{
	register CDialog* pDlg;

	// test for special case (Win 3.0 will call dialog proc instead
	//  of SendMessage for these two messages).
	if (message != WM_SETFONT && message != WM_INITDIALOG)
		return 0L;      // normal handler

	// assume it is already wired up to a permanent one
	pDlg = (CDialog*) CWnd::FromHandlePermanent(hWnd);
	ASSERT(pDlg != NULL);
	ASSERT(pDlg->m_hWnd == hWnd);

	// prepare for callback, make it look like message map call
	LONG lResult = 0;
	CLastState oldState = lastState;    // save for nesting

	lastState.msg.hwnd = hWnd;
	lastState.msg.message = message;
	lastState.msg.wParam = wParam;
	lastState.msg.lParam = lParam;

	TRY
	{
		if (message == WM_SETFONT)
			pDlg->OnSetFont(CFont::FromHandle((HFONT)wParam));
		else // WM_INITDIALOG
			lResult = pDlg->OnInitDialog();
	}
	CATCH (CException, e)
	{
		// fall through
		TRACE("Warning: something went wrong in dialog init\n");
		pDlg->EndDialog(IDCANCEL);  // something went wrong
		ASSERT(FALSE);
	}
	END_CATCH

	lastState = oldState;
	return lResult;
}

/////////////////////////////////////////////////////////////////////////////
// Window creation hook
#ifdef STRICT
static HHOOK pfnOldSendMsgHook = NULL;
#else
static HOOKPROC pfnOldSendMsgHook = NULL;
#endif

#pragma optimize("q", off)    // disable pcode opt (Win 3.0 compatibility)

void FAR PASCAL AFX_EXPORT
_AfxSendMsgHook(int code, UINT wParam, LONG lParam)
{
	struct HOOKINFO     // Hook info struct passed by send message hook
	{
		LONG lParam;
		UINT wParam;
		UINT msg;
		HWND hWnd;
	};
	HOOKINFO FAR* hookInfo;

	if (code < 0)
	{
		::DefHookProc(code, wParam, lParam, &pfnOldSendMsgHook);
		return;
	}

	ASSERT(pWndInit != NULL);
	hookInfo = (HOOKINFO FAR*)lParam;
	HWND hWnd = hookInfo->hWnd;

	// ignore non-creation messages
	if (hookInfo->msg != WM_GETMINMAXINFO &&
	  hookInfo->msg != WM_NCCREATE)
	{
		// not being constructed
		return;
	}

	// Connect the HWND to pWndInit...
	pWndInit->Attach(hWnd);

	// Subclass the window by replacing its window proc addr...
	WNDPROC oldWndProc = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC,
		(DWORD)AfxWndProc);
	if (oldWndProc != (WNDPROC)AfxWndProc)
	{
		*(pWndInit->GetSuperWndProcAddr()) = oldWndProc; // save if not default
	}

	// Unhook the send message hook since we don't need it any more
	::UnhookWindowsHook(WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook);
	pWndInit = NULL;
}

#pragma optimize("", on)    // return to default optimizations

void _AfxHookWindowCreate(register CWnd* pWnd)
{
#ifndef _WINDLL
	if (_afxSetWindowsHookExProc == NULL)
	{
		pfnOldSendMsgHook = ::SetWindowsHook(WH_CALLWNDPROC,
			(HOOKPROC)_AfxSendMsgHook);
	}
	else
	{
#ifdef STRICT
#ifdef _NTWIN
		pfnOldSendMsgHook = (HHOOK)(*_afxSetWindowsHookExProc)(
			WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook, NULL,
			::GetCurrentThreadId());
#else
		pfnOldSendMsgHook = (HHOOK)(*_afxSetWindowsHookExProc)(
			WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook, AfxGetInstanceHandle(),
			GetCurrentTask());
#endif
#else
#ifdef _NTWIN
		pfnOldSendMsgHook = (HOOKPROC)(*_afxSetWindowsHookExProc)(
			WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook, NULL,
			::GetCurrentThreadId());
#else
		pfnOldSendMsgHook = (HOOKPROC)(*_afxSetWindowsHookExProc)(
			WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook, AfxGetInstanceHandle(),
			GetCurrentTask());
#endif
#endif
	}
#else // _WINDLL
#ifndef _NTWIN
	pfnOldSendMsgHook = ::SetWindowsHook(WH_CALLWNDPROC,
		(HOOKPROC)_AfxSendMsgHook);
#else
	pfnOldSendMsgHook = (HOOKPROC)(*_afxSetWindowsHookExProc)(
		WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook, NULL,
		::GetCurrentThreadId());
#endif
#endif // _WINDLL

	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd == NULL);   // only do once

	ASSERT(pWndInit == NULL);       // hook not already in progress
	pWndInit = pWnd;
}

BOOL _AfxUnhookWindowCreate()
	// return TRUE if already unhooked
{
	if (pWndInit == NULL)
		return TRUE;        // already unhooked => window create success
	::UnhookWindowsHook(WH_CALLWNDPROC, (HOOKPROC)_AfxSendMsgHook);
	pWndInit = NULL;
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWnd creation

BOOL CWnd::CreateEx(DWORD dwExStyle, LPCSTR lpClassName,
		LPCSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, HMENU nIDorHMenu)
{
	_AfxHookWindowCreate(this);
	HWND hWnd = ::CreateWindowEx(dwExStyle, lpClassName,
			lpWindowName, dwStyle, x, y, nWidth, nHeight,
			hWndParent, nIDorHMenu, AfxGetInstanceHandle(), NULL);

	_AfxUnhookWindowCreate();
	if (hWnd == NULL)
		return NULL;
	ASSERT(hWnd == m_hWnd); // should have been set in send msg hook
	return TRUE;
}

// for child windows
BOOL CWnd::Create(LPCSTR lpClassName,
	LPCSTR lpWindowName, DWORD dwStyle,
	const RECT& rect,
	const CWnd* pParentWnd, UINT nID)
{
	ASSERT(pParentWnd != NULL);

	if (lpClassName == NULL)
		lpClassName = _afxWnd;

	return CreateEx(0, lpClassName, lpWindowName,
		dwStyle | WS_CHILD,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID);
}

CWnd::~CWnd()
{
	DestroyWindow();
}

#ifdef _NTWIN
struct _AFXCTLCOLOR {
	HWND hWnd;
	HDC hDC;
	UINT nCtlType;
};

LRESULT CWnd::OnNTCtlColor(WPARAM wParam, LPARAM lParam)
{
	struct _AFXCTLCOLOR ctl;

	ctl.hDC = (HDC)wParam;
	ctl.hWnd = (HWND)lParam;
	ctl.nCtlType = GetCurrentMessage()->message - WM_CTLCOLORMSGBOX;

	ASSERT(ctl.nCtlType >= CTLCOLOR_MSGBOX);
	ASSERT(ctl.nCtlType <= CTLCOLOR_STATIC);

	// NOTE: We call the virtual WindowProc for this window directly,
	//  instead of calling _AfxCallWindowProc, so that Default()
	//  will still work (it will call the Default window proc with
	//	the original NT WM_CTLCOLOR message).

	return WindowProc(WM_CTLCOLOR, 0, (LPARAM)&ctl);
}
#endif

void CWnd::OnDestroy()
{
#ifndef _WINDLL
	// Automatically quit when the main window is destroyed.
	if (AfxGetApp()->m_pMainWnd == this)
		::PostQuitMessage(0);
#endif // _WINDLL
	Default();
}

void CWnd::OnNcDestroy()
{
	// WM_NCDESTROY is the absolute LAST message sent.
	if (AfxGetApp()->m_pMainWnd == this)
		AfxGetApp()->m_pMainWnd = NULL;

	Default();
	Detach();
	ASSERT(m_hWnd == NULL);

	// call special post-cleanup routine
	PostNcDestroy();
}

void CWnd::PostNcDestroy()
{
	// default to nothing
}

#ifdef _DEBUG
void CWnd::AssertValid() const
{
	ASSERT(m_hWnd == NULL ||
		(m_hWnd == (HWND)1 && this == &CWnd::wndBottom) ||
		::IsWindow(m_hWnd));
	void* p;
	ASSERT(m_hWnd == NULL ||
		(m_hWnd == (HWND)1 && this == &CWnd::wndBottom) ||
		hWndMap.LookupPermanent(m_hWnd, p) ||
		hWndMap.LookupTemporary(m_hWnd, p));
}

void CWnd::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "with window information:\n";
	dc << "m_hWnd = " << (void NEAR*)m_hWnd;

	if ((UINT)m_hWnd > 1)
	{
		char szBuf [64];

		GetWindowText(szBuf, sizeof (szBuf));
		dc << "\ncaption = \"" << szBuf << "\"";

		::GetClassName(m_hWnd, szBuf, sizeof (szBuf));
		dc << "\nclass name = \"" << szBuf << "\"";

		CRect rect;
		GetWindowRect(&rect);
		dc << "\nrect = " << rect;

		dc << "\nparent CWnd* = " << (void*)GetParent();

		dc << "\nstyle = " << (void FAR*)::GetWindowLong(m_hWnd, GWL_STYLE);
		if (::GetWindowLong(m_hWnd, GWL_STYLE) & WS_CHILD)
#ifndef _NTWIN
			dc << "\nid = " << ::GetWindowWord(m_hWnd, GWW_ID);
#else
			dc << "\nid = " << ::GetWindowLong(m_hWnd, GWL_ID);
#endif
	}
}
#endif

BOOL
CWnd::DestroyWindow()
{
	if (m_hWnd == NULL)
		return FALSE;

	void* p;
	BOOL bInPermanentMap = hWndMap.LookupPermanent(m_hWnd, p);
	BOOL bRet = ::DestroyWindow(m_hWnd);
	// Note that 'this' may have been deleted at this point.
	if (bInPermanentMap)
	{
		// Should have been detached by OnNcDestroy
		ASSERT(!hWndMap.LookupPermanent(m_hWnd, p));
	}
	else
	{
		// Detach after DestroyWindow called just in case
		Detach();
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// Default CWnd implementation

LONG CWnd::DefWindowProc(UINT nMsg, UINT wParam, LONG lParam)
{
	WNDPROC pfnWndProc;

	if ((pfnWndProc = *GetSuperWndProcAddr()) == NULL)
		return ::DefWindowProc(m_hWnd, nMsg, wParam, lParam);
	else
#ifdef STRICT
		return ::CallWindowProc(pfnWndProc, m_hWnd, nMsg, wParam, lParam);
#else
		return ::CallWindowProc((FARPROC)pfnWndProc, m_hWnd, nMsg, wParam, lParam);
#endif
}

WNDPROC* CWnd::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper = NULL;
	ASSERT(pfnSuper == NULL);       // should never be changed !!!
					// if this is non-NULL, then a derived class of CWnd
					//  forgot to override 'superWndProc' as well as 'className'
	return &pfnSuper;
}

BOOL CWnd::PreTranslateMessage(MSG*)
{
	// no default processing
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWnd will delegate owner draw messages to self drawing controls

// Drawing: for all 4 control types
void CWnd::OnDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	UINT nType;
	if ((nType = lpDrawItemStruct->CtlType) == ODT_MENU)
	{
		CMenu* pMenu = CMenu::FromHandle((HMENU)lpDrawItemStruct->hwndItem);
		if (pMenu != NULL)
		{
			pMenu->DrawItem(lpDrawItemStruct);
			return;
		}
	}
	else
	{
		CWnd* pChild = CWnd::FromHandlePermanent(lpDrawItemStruct->hwndItem);
		if (pChild != NULL)
		{
			if (nType == ODT_BUTTON &&
				pChild->IsKindOf(RUNTIME_CLASS(CButton)))
			{
				((CButton*)pChild)->DrawItem(lpDrawItemStruct);
				return;
			}
			else if (nType == ODT_LISTBOX &&
				pChild->IsKindOf(RUNTIME_CLASS(CListBox)))
			{
				((CListBox*)pChild)->DrawItem(lpDrawItemStruct);
				return;
			}
			else if (nType == ODT_COMBOBOX &&
				pChild->IsKindOf(RUNTIME_CLASS(CComboBox)))
			{
				((CComboBox*)pChild)->DrawItem(lpDrawItemStruct);
				return;
			}
		}
	}
	// not handled - do default
	Default();
}

// Drawing: for all 4 control types
int CWnd::OnCompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	CWnd* pChild = CWnd::FromHandlePermanent(lpCompareItemStruct->hwndItem);
	if (pChild != NULL)
	{
		UINT nType = lpCompareItemStruct->CtlType;
		if (nType == ODT_LISTBOX &&
			pChild->IsKindOf(RUNTIME_CLASS(CListBox)))
		{
			return ((CListBox*)pChild)->CompareItem(lpCompareItemStruct);
		}
		else if (nType == ODT_COMBOBOX &&
			pChild->IsKindOf(RUNTIME_CLASS(CComboBox)))
		{
			return ((CComboBox*)pChild)->CompareItem(lpCompareItemStruct);
		}
	}
	// not handled - do default
	return (int)Default();
}

void CWnd::OnDeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	CWnd* pChild = CWnd::FromHandlePermanent(lpDeleteItemStruct->hwndItem);
	if (pChild != NULL)
	{
		UINT nType = lpDeleteItemStruct->CtlType;
		if (nType == ODT_LISTBOX &&
			pChild->IsKindOf(RUNTIME_CLASS(CListBox)))
		{
			((CListBox*)pChild)->DeleteItem(lpDeleteItemStruct);
			return;
		}
		else if (nType == ODT_COMBOBOX &&
			pChild->IsKindOf(RUNTIME_CLASS(CComboBox)))
		{
			((CComboBox*)pChild)->DeleteItem(lpDeleteItemStruct);
			return;
		}
	}
	// not handled - do default
	Default();
}


static CMenu* FindPopupMenuFromID(CMenu* pMenu, UINT nID)
{
	// walk through all items, looking for ID match
	UINT nItems = pMenu->GetMenuItemCount();
	for (int iItem = 0; iItem < (int)nItems; iItem++)
	{
		CMenu* pPopup = pMenu->GetSubMenu(iItem);
		if (pPopup != NULL)
		{
			// recurse to child popup
			pPopup = FindPopupMenuFromID(pPopup, nID);
			// try recursing
			if (pPopup != NULL)
				return pPopup;
		}
		else if (pMenu->GetMenuItemID(iItem) == nID)
		{
			// it is a normal item inside our popup
			return pMenu;
		}
	}
	// not found
	return NULL;
}

// Measure item implementation relies on unique control/menu IDs
void CWnd::OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	UINT nType;
	if ((nType = lpMeasureItemStruct->CtlType) == ODT_MENU)
	{
		ASSERT(lpMeasureItemStruct->CtlID == 0);
		CMenu* pMenu = FindPopupMenuFromID(GetMenu(),
			lpMeasureItemStruct->itemID);
		if (pMenu != NULL)
		{
			pMenu->MeasureItem(lpMeasureItemStruct);
			return;
		}
		else
		{
			TRACE("Warning: unknown WM_MEASUREITEM request for"
				" menu item 0x%04X\n", lpMeasureItemStruct->itemID);
		}
	}
	else
	{
		HWND hWndChild = ::GetDlgItem(m_hWnd, lpMeasureItemStruct->CtlID);
		CWnd* pChild;
		if (hWndChild != NULL &&
			(pChild = CWnd::FromHandlePermanent(hWndChild)) != NULL)
		{
			if (nType == ODT_LISTBOX &&
				pChild->IsKindOf(RUNTIME_CLASS(CListBox)))
			{
				((CListBox*)pChild)->MeasureItem(lpMeasureItemStruct);
				return;
			}
			else if (nType == ODT_COMBOBOX &&
				pChild->IsKindOf(RUNTIME_CLASS(CComboBox)))
			{
				((CComboBox*)pChild)->MeasureItem(lpMeasureItemStruct);
				return;
			}
		}
	}
	// not handled - do default
	Default();
}

/////////////////////////////////////////////////////////////////////////////
// CFrameWnd

IMPLEMENT_DYNAMIC(CFrameWnd, CWnd)

CFrameWnd::CFrameWnd()
{
	ASSERT(m_hWnd == NULL);
	m_hAccelTable = NULL;
}

CFrameWnd::~CFrameWnd()
{
	if (m_hAccelTable != NULL)
		::FreeResource(m_hAccelTable);
}

void CFrameWnd::PostNcDestroy()
{
	// default for frame windows is to allocate them on the heap
	//  the default post-cleanup is to 'delete this'.
	// never explicitly call 'delete' on a CFrameWnd, use DestroyWindow instead
	delete this;
}

#ifdef _DEBUG
void CFrameWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CFrameWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
	dc << "\nm_hAccelTable = " << (UINT) m_hAccelTable;
}
#endif

BOOL CFrameWnd::LoadAccelTable(LPCSTR lpAccelTableName)
{
	ASSERT(m_hAccelTable == NULL);  // only do once
	ASSERT(lpAccelTableName != NULL);

	m_hAccelTable = ::LoadAccelerators(AfxGetResourceHandle(),
		lpAccelTableName);
	return (m_hAccelTable != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Creation and window tree access

BOOL CFrameWnd::Create(LPCSTR lpClassName,
	LPCSTR lpWindowName,
	DWORD dwStyle, const RECT& rect,
	const CWnd* pParentWnd,
	LPCSTR lpMenuName)
{
	HMENU   hMenu = NULL;

	if (lpClassName == NULL)
		lpClassName = _afxFrameWnd;

	if (lpMenuName != NULL)
	{
		// load in a menu that will get destroyed when window gets destroyed
		hMenu = ::LoadMenu(AfxGetResourceHandle(), lpMenuName);
		if (hMenu == NULL)
		{
			TRACE("Warning: failed to load menu for CFrameWnd\n");
			return FALSE;
		}
	}

	if (!CreateEx(0L, lpClassName, lpWindowName, dwStyle,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), hMenu))
	{
		TRACE("Warning: failed to create CFrameWnd\n");
		return FALSE;
	}
	return TRUE;
}

CFrameWnd* CFrameWnd::GetChildFrame()
{
	return this;
}

CFrameWnd* CFrameWnd::GetParentFrame()
{
	return this;
}

BOOL CFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	return (m_hAccelTable != NULL &&
	  ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg));
}


/////////////////////////////////////////////////////////////////////////////
// Additional helpers for WNDCLASS init

const char* AfxRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon)
{
	// Returns a temporary string name for the class
	//  Save in a CString if you want to use it for a long time
	WNDCLASS wndcls;
	static char szName[64];     // 1 global string

	// generate a synthetic name for this class
	if (hCursor == NULL && hbrBackground == NULL && hIcon == NULL)
		wsprintf(szName, "Afx:%x", nClassStyle);
	else
		wsprintf(szName, "Afx:%x:%x:%x:%x", nClassStyle,
			(UINT) hCursor, (UINT) hbrBackground, (UINT) hIcon);

	// see if the class already exists
	if (::GetClassInfo(AfxGetInstanceHandle(), szName, &wndcls))
	{
		// already registered, assert everything is good
		ASSERT(wndcls.style == nClassStyle);
		
		// NOTE: We have to trust that the hIcon, hbrBackground, and the
		//  hCursor are semantically the same, because sometimes Windows does 
		//  some internal translation or copying of those handles before 
		//  storing them in the internal WNDCLASS retrieved by GetClassInfo.
		return szName;
	}

	// otherwise we need to register a new class
	wndcls.style = nClassStyle;
	wndcls.lpfnWndProc  = AfxWndProc;
	wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = hIcon;
	wndcls.hCursor = hCursor;
	wndcls.hbrBackground = hbrBackground;
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = szName;
	if (!::RegisterClass(&wndcls))
		AfxThrowResourceException();
	return szName;
}



/////////////////////////////////////////////////////////////////////////////
// Dialogs have 2-phase construction

IMPLEMENT_DYNAMIC(CDialog, CWnd)

CDialog::CDialog()
{
	ASSERT(m_hWnd == NULL);

	m_hBrushCtlBk = NULL;
	VERIFY(SetCtlBkColor(::GetSysColor(COLOR_BTNFACE)));
}

CDialog::~CDialog()
{
	if (m_hBrushCtlBk != NULL)
		::DeleteObject(m_hBrushCtlBk);
	m_hBrushCtlBk = NULL;
}

#ifdef _DEBUG
void
CDialog::AssertValid() const
{
	CWnd::AssertValid();
	ASSERT(m_hWnd != (HWND)1);
}
#endif

// Modeless
BOOL
CDialog::Create(LPCSTR lpTemplateName, CWnd* pParentWnd)
{
	if (pParentWnd == NULL)
		pParentWnd = AfxGetApp()->m_pMainWnd;

	_AfxHookWindowCreate(this);
	HWND hWnd = ::CreateDialog(AfxGetResourceHandle(),
		lpTemplateName, pParentWnd->GetSafeHwnd(),
		(DLGPROC)_AfxDlgProc);
	_AfxUnhookWindowCreate();

	return (m_hWnd = hWnd) != NULL;
}

BOOL
CDialog::CreateIndirect(const void FAR* lpDialogTemplate,
		CWnd* pParentWnd)
{
	if (pParentWnd == NULL)
		pParentWnd = AfxGetApp()->m_pMainWnd;

	_AfxHookWindowCreate(this);
#ifndef _NTWIN
	HWND hWnd = ::CreateDialogIndirect(AfxGetResourceHandle(),
		lpDialogTemplate, pParentWnd->GetSafeHwnd(),
		(DLGPROC)_AfxDlgProc);
#else
	HWND hWnd = ::CreateDialogIndirect(AfxGetResourceHandle(),
		(LPCDLGTEMPLATE)lpDialogTemplate, pParentWnd->GetSafeHwnd(),
		(DLGPROC)_AfxDlgProc);
#endif
	_AfxUnhookWindowCreate();

	return (m_hWnd = hWnd) != NULL;
}

void
CDialog::OnSetFont(CFont*)
{
	// ignore it
}

BOOL
CDialog::OnInitDialog()
{
	return TRUE;    // set focus to first one
}


BOOL 
CDialog::SetCtlBkColor(COLORREF clrCtlBk)
{ 
	if (m_hBrushCtlBk != NULL)
		::DeleteObject(m_hBrushCtlBk);
	m_hBrushCtlBk = NULL;

	if (clrCtlBk == 0xFFFFFFFF)
	{
		// -1 means do not handle any WM_CTLCOLOR messages
		ASSERT(m_hBrushCtlBk == NULL);
		return TRUE;
	}

	m_hBrushCtlBk = ::CreateSolidBrush(clrCtlBk);
	return m_hBrushCtlBk != NULL ? TRUE : FALSE;
}

HBRUSH 
CDialog::OnCtlColor(CDC* pDC, CWnd* /* pWnd */, UINT nCtlColor)
{
	if (m_hBrushCtlBk == NULL || 
			nCtlColor == CTLCOLOR_LISTBOX ||
			nCtlColor == CTLCOLOR_EDIT || 
			nCtlColor == CTLCOLOR_MSGBOX)
		return (HBRUSH)Default();

	// Use new look AFX colors
	// Set the background color for controls
	LOGBRUSH logbrush;
	if (::GetObject(m_hBrushCtlBk, sizeof(LOGBRUSH), (LPSTR)&logbrush) != 0) 
	{
		pDC->SetBkColor(logbrush.lbColor);
	}
	else
	{
		TRACE("Warning: couldn't set background color for CTLCOLOR\n");
	}
	return m_hBrushCtlBk;
}

BEGIN_MESSAGE_MAP(CDialog, CWnd)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dialog Proc support

BOOL
CDialog::PreTranslateMessage(MSG* pMsg)
{
	// for modeless processing (or modal)
	ASSERT(m_hWnd != NULL);

	// filter both messages to dialog and from children
	return ::IsDialogMessage(m_hWnd, pMsg);
}

WNDPROC*
CDialog::GetSuperWndProcAddr()
{
	static WNDPROC pfnSuper;
	return &pfnSuper;
}

/////////////////////////////////////////////////////////////////////////////
// CModalDialog

IMPLEMENT_DYNAMIC(CModalDialog, CDialog)

BEGIN_MESSAGE_MAP(CModalDialog, CDialog)
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

// Constructors just save parameters
CModalDialog::CModalDialog(LPCSTR lpTemplateName, CWnd* pParentWnd)
{
	m_lpDialogTemplate = lpTemplateName;
	m_hDialogTemplate = NULL;
	m_pParentWnd = pParentWnd;
}

CModalDialog::CModalDialog(UINT nIDTemplate, CWnd* pParentWnd)
{
	m_lpDialogTemplate = MAKEINTRESOURCE(nIDTemplate);
	m_hDialogTemplate = NULL;
	m_pParentWnd = pParentWnd;
}

BOOL
CModalDialog::CreateIndirect(HANDLE hDialogTemplate)
{
	// must be called on an empty constructed CModalDialog
	ASSERT(m_lpDialogTemplate == NULL);
	ASSERT(m_hDialogTemplate == NULL);

	m_hDialogTemplate = hDialogTemplate;
	return TRUE;        // always ok (DoModal actually brings up dialog)
}

#ifdef _DEBUG
void
CModalDialog::AssertValid() const
{
	CDialog::AssertValid();
}

void
CModalDialog::Dump(CDumpContext& dc) const
{
	CDialog::Dump(dc);
	dc << "\nm_lpDialogTemplate = " << m_lpDialogTemplate << "\n";
	dc << "m_hDialogTemplate = " << m_hDialogTemplate << "\n";
	dc << "m_pParentWnd = " << (void *)m_pParentWnd;
}
#endif

int
CModalDialog::DoModal()
{
	HWND    hWndParent;
	int     nResult;

	// can be constructed with a resource template or CreateIndirect
	ASSERT(m_lpDialogTemplate != NULL || m_hDialogTemplate != NULL);

	// find parent HWND
	if (m_pParentWnd != NULL)
		hWndParent = m_pParentWnd->m_hWnd;
	else
		hWndParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();

	_AfxHookWindowCreate(this);
	if (m_lpDialogTemplate != NULL)
	{
		nResult = ::DialogBox(AfxGetResourceHandle(), m_lpDialogTemplate,
			hWndParent, (DLGPROC)_AfxDlgProc);
	}
	else
	{
#ifndef _NTWIN
		nResult = ::DialogBoxIndirect(AfxGetResourceHandle(), m_hDialogTemplate,
			hWndParent, (DLGPROC)_AfxDlgProc);
#else
		nResult = ::DialogBoxIndirect(AfxGetResourceHandle(),
			(LPCDLGTEMPLATE)m_hDialogTemplate,
			hWndParent, (DLGPROC)_AfxDlgProc);
#endif
	}

	_AfxUnhookWindowCreate();   // just in case
	Detach();               // just in case
	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// Standard CModalDialog implementation

void
CModalDialog::OnOK()
{
	EndDialog(IDOK);
}

void
CModalDialog::OnCancel()
{
	EndDialog(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
// CRect for creating windows with the default position/size
const CRect NEAR CFrameWnd::rectDefault(CW_USEDEFAULT, CW_USEDEFAULT,
	0 /* 2*CW_USEDEFAULT */, 0 /* 2*CW_USEDEFAULT */);

// CWnds for setting z-order with SetWindowPos's pWndInsertAfter parameter
const CWnd NEAR CWnd::wndTop((HWND)0);
const CWnd NEAR CWnd::wndBottom((HWND)1);
const CWnd NEAR CWnd::wndTopMost((HWND)-1);
const CWnd NEAR CWnd::wndNoTopMost((HWND)-2);

/////////////////////////////////////////////////////////////////////////////
// Message table implementation

CMessageMap CWnd::messageMap =
{
	NULL,           // end of chain of message maps
	(CMessageEntry FAR*) &CWnd::_messageEntries
};

CMessageMap* CWnd::GetMessageMap() const
{
	return &CWnd::messageMap;
}


CMessageEntry BASED_CODE CWnd::_messageEntries[] =
{
	ON_WM_COMPAREITEM()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_DELETEITEM()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
#ifdef _NTWIN
	ON_MESSAGE(WM_CTLCOLORMSGBOX, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLOREDIT, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORBTN, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORDLG, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORSCROLLBAR, OnNTCtlColor)
	ON_MESSAGE(WM_CTLCOLORSTATIC, OnNTCtlColor)
#endif

	{ 0, 0, AfxSig_end, (AFX_PMSG)0 }
};

union MessageMapFunctions
{
	AFX_PMSG pfn;   // generic member function pointer

	// specific type safe variants
	BOOL    (CWnd::*pfn_bD)(CDC *);
	BOOL    (CWnd::*pfn_bb)(BOOL);
	BOOL    (CWnd::*pfn_bWww)(CWnd*, UINT, UINT);
	HBRUSH  (CWnd::*pfn_hDWw)(CDC *, CWnd*, UINT);
	int     (CWnd::*pfn_iwWw)(UINT, CWnd*, UINT);
	int     (CWnd::*pfn_iWww)(CWnd*, UINT, UINT);
	int     (CWnd::*pfn_is)(LPSTR);
	LONG    (CWnd::*pfn_lwl)(UINT, LONG);
	LONG    (CWnd::*pfn_lwwM)(UINT, UINT, CMenu *);
	void    (CWnd::*pfn_vv)(void);

	void    (CWnd::*pfn_vw)(UINT);
	void    (CWnd::*pfn_vww)(UINT, UINT);
	void    (CWnd::*pfn_vvii)(int, int);
	void    (CWnd::*pfn_vwww)(UINT, UINT, UINT);
	void    (CWnd::*pfn_vwii)(UINT, int, int);
	void    (CWnd::*pfn_vwl)(UINT, LONG);
	void    (CWnd::*pfn_vbWW)(BOOL, CWnd*, CWnd*);
	void    (CWnd::*pfn_vD)(CDC *);
	void    (CWnd::*pfn_vM)(CMenu *);
	void    (CWnd::*pfn_vMwb)(CMenu *, UINT, BOOL);

	void    (CWnd::*pfn_vW)(CWnd*);
	void    (CWnd::*pfn_vWww)(CWnd*, UINT, UINT);
	void    (CWnd::*pfn_vWh)(CWnd*, HANDLE);
	void    (CWnd::*pfn_vwW)(UINT, CWnd*);
	void    (CWnd::*pfn_vwWb)(UINT, CWnd*, BOOL);
	void    (CWnd::*pfn_vwwW)(UINT, UINT, CWnd*);
	void    (CWnd::*pfn_vs)(LPSTR);
	UINT    (CWnd::*pfn_wp)(CPoint);
	UINT    (CWnd::*pfn_wv)(void);
	BOOL    (CWnd::*pfn_bh)(HANDLE);
	void    (CWnd::*pfn_vPOS)(WINDOWPOS FAR*);
	void    (CWnd::*pfn_vCALC)(NCCALCSIZE_PARAMS FAR*);
#ifdef _NTWIN
	void	(CWnd::*pfn_vwp)(UINT, CPoint);
	void	(CWnd::*pfn_vwwh)(UINT, UINT, HANDLE);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Routines for fast search of message maps


// Hand tuned routine

#pragma optimize("qgel", off) // assembler cannot be globally optimized

#ifdef _NTWIN
// C versions of search routines
static inline CMessageEntry FAR*
FindMessageEntry(CMessageEntry FAR* lpEntry, UINT nMsg, UINT nID)
{
	while (lpEntry->nSig != AfxSig_end)
	{
		if (lpEntry->nMessage == nMsg && lpEntry->nID == nID)
			return lpEntry;
		lpEntry++;
	}
	return NULL;	// not found
}

#else
static CMessageEntry FAR* NEAR
FindMessageEntry(CMessageEntry FAR* lpEntry, UINT nMsg, UINT nID)
{
	_asm
	{
				LES     BX,lpEntry
				MOV     AX,nMsg
				MOV     DX,nID
		__loop:
				MOV     CX,WORD PTR ES:[BX+4]   ; nSig (0 => end)
				JCXZ    __failed
				CMP     AX,WORD PTR ES:[BX]     ; nMessage
				JE      __found_1
		__next:
				ADD     BX,SIZE CMessageEntry
				JMP     __loop
		__found_1:
				CMP     DX,WORD PTR ES:[BX+2]   ; nID
				JNE     __next
		// found a match
				MOV     WORD PTR lpEntry,BX
				MOV     WORD PTR lpEntry+2,ES
				JMP     __end
		__failed:
				XOR     AX,AX
				MOV     WORD PTR lpEntry,AX
				MOV     WORD PTR lpEntry+2,AX
		__end:
	}
	return lpEntry;
}
#endif //_NTWIN

#pragma optimize("", on)    // return to default optimizations


/////////////////////////////////////////////////////////////////////////////

#ifndef iHashMax
// iHashMax must be a power of two
	#ifdef _NEARDATA
		#define iHashMax 64
	#else
		#define iHashMax 256
	#endif
#endif

struct MsgCache
{
	UINT nMsg;
	CMessageEntry FAR* lpEntry;
	CMessageMap* pMessageMap;
};

MsgCache _afxMsgCache[iHashMax];

LONG
CWnd::WindowProc(UINT nMsg, UINT wParam, LONG lParam)
{
	register CMessageMap* pMessageMap;
	CMessageEntry FAR* lpEntry;

	if (nMsg == WM_COMMAND) // special case for commands
	{
		if (OnCommand(wParam, lParam))
			return 1L; // command handled
		else 
			return (LONG)DefWindowProc(nMsg, wParam, lParam); // call default handler
	}

	pMessageMap = GetMessageMap();
	UINT iHash = (_AFX_FP_OFF(pMessageMap) ^ nMsg) & (iHashMax-1);
	MsgCache& msgCache = _afxMsgCache[iHash];

	if (nMsg == msgCache.nMsg && pMessageMap == msgCache.pMessageMap)
	{
		// Cache hit
		lpEntry = msgCache.lpEntry;
		if (lpEntry == NULL)
			return (LONG)DefWindowProc(nMsg, wParam, lParam);
		else if (nMsg < 0xC000)
			goto LDispatch;
		else
			goto LDispatchRegistered;
	}
	else
	{
		// not in cache, look for it
		msgCache.nMsg = nMsg;
		msgCache.pMessageMap = pMessageMap;

		for (/* pMessageMap already init'ed */; pMessageMap != NULL;
			pMessageMap = pMessageMap->pBaseMessageMap)
		{
			// This may loop forever if the message maps are not properly
			// chained together.  Make sure each window class's message map
			// points to the base window class's message map.

			if (nMsg < 0xC000)
			{
				// constant window message
				if ((lpEntry = FindMessageEntry(pMessageMap->lpEntries,
					nMsg, 0)) != NULL)
				{
					msgCache.lpEntry = lpEntry;
					goto LDispatch;
				}
			}
			else
			{
				// registered windows message
				lpEntry = pMessageMap->lpEntries;

				while ((lpEntry = FindMessageEntry(lpEntry, 0xC000, 0)) != NULL)
				{
					UINT NEAR* pnID = (UINT NEAR*)(lpEntry->nSig);
					ASSERT(*pnID >= 0xC000);
						// must be successfully registered
					if (*pnID == nMsg)
					{
						msgCache.lpEntry = lpEntry;
						goto LDispatchRegistered;
					}
					lpEntry++;      // keep looking past this one
				}
			}
		}

		msgCache.lpEntry = NULL;
		return DefWindowProc(nMsg, wParam, lParam);
	}
	ASSERT(FALSE);      // not reached


LDispatch:
	ASSERT(nMsg < 0xC000);
	union MessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	switch (lpEntry->nSig)
	{
	default:
		ASSERT(FALSE);
		return 0;

	case AfxSig_bD:
		return (this->*mmf.pfn_bD)(CDC::FromHandle((HDC)wParam));

	case AfxSig_bb:
		return (this->*mmf.pfn_bb)((BOOL)wParam);

	case AfxSig_bWww:
		return (this->*mmf.pfn_bWww)(CWnd::FromHandle((HWND)wParam),
			(short)LOWORD(lParam), HIWORD(lParam));

#ifdef _NTWIN
	case AfxSig_hDWw:
		{
			ASSERT(nMsg == WM_CTLCOLOR);
			struct _AFXCTLCOLOR* pCtl = (struct _AFXCTLCOLOR*)lParam;
			return (LONG)((this->*mmf.pfn_hDWw)( CDC::FromHandle(pCtl->hDC),
				CWnd::FromHandle(pCtl->hWnd), pCtl->nCtlType));
		}
#else
	case AfxSig_hDWw:
		return (LONG)(UINT)(this->*mmf.pfn_hDWw)(CDC::FromHandle((HDC)wParam),
			CWnd::FromHandle((HWND)LOWORD(lParam)), HIWORD(lParam));
#endif

#ifndef _NTWIN
	case AfxSig_iwWw:
		return (this->*mmf.pfn_iwWw)(wParam, CWnd::FromHandle((HWND)LOWORD(lParam)),
			HIWORD(lParam));
#else
	case AfxSig_iwWw:
		return (this->*mmf.pfn_iwWw)(LOWORD(wParam), 
			CWnd::FromHandle((HWND)lParam),
			HIWORD(wParam));
#endif

	case AfxSig_iWww:
		return (this->*mmf.pfn_iWww)(CWnd::FromHandle((HWND)wParam),
			(short)LOWORD(lParam), HIWORD(lParam));

	case AfxSig_is:
		return (this->*mmf.pfn_is)((LPSTR)lParam);

	case AfxSig_lwl:
		return (this->*mmf.pfn_lwl)(wParam, lParam);

#ifndef _NTWIN
	case AfxSig_lwwM:
		return (this->*mmf.pfn_lwwM)(wParam, LOWORD(lParam),
			CMenu::FromHandle((HMENU)HIWORD(lParam)));
#else
	case AfxSig_lwwM:
		return (this->*mmf.pfn_lwwM)((UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
			(CMenu*)CMenu::FromHandle((HMENU)lParam));
#endif

	case AfxSig_vv:
		(this->*mmf.pfn_vv)();
		return 0;


	case AfxSig_vw: // AfxSig_vb:
		(this->*mmf.pfn_vw)(wParam);
		return 0;

	case AfxSig_vww:
#ifndef _NTWIN
		(this->*mmf.pfn_vww)(wParam, LOWORD(lParam));
#else
		(this->*mmf.pfn_vww)(wParam, lParam);
#endif
		return 0;

	case AfxSig_vvii:
		(this->*mmf.pfn_vvii)(LOWORD(lParam), HIWORD(lParam));
		return 0;

#ifdef _NTWIN
	case AfxSig_vwwh:
		(this->*mmf.pfn_vwwh)(LOWORD(wParam), HIWORD(wParam), (HANDLE)lParam);
		return 0;
#endif

	case AfxSig_vwww:
		(this->*mmf.pfn_vwww)(wParam, LOWORD(lParam), HIWORD(lParam));
		return 0;

	case AfxSig_vwii:
		(this->*mmf.pfn_vwii)(wParam, LOWORD(lParam), HIWORD(lParam));
		return 0;

	case AfxSig_vwl:
		(this->*mmf.pfn_vwl)(wParam, lParam);
		return 0;

#ifndef _NTWIN
	case AfxSig_vbWW:
		(this->*mmf.pfn_vbWW)((BOOL)wParam,
			CWnd::FromHandle((HWND)LOWORD(lParam)),
			CWnd::FromHandle((HWND)HIWORD(lParam)));
		return 0;
#else
	case AfxSig_vbWW:
		(this->*mmf.pfn_vbWW)(m_hWnd == (HWND)lParam,
			CWnd::FromHandle((HWND)lParam),
			CWnd::FromHandle((HWND)wParam));
		return 0;
#endif

	case AfxSig_vD:
		(this->*mmf.pfn_vD)(CDC::FromHandle((HDC)wParam));
		return 0;

	case AfxSig_vM:
		(this->*mmf.pfn_vM)(CMenu::FromHandle((HMENU)wParam));
		return 0;

	case AfxSig_vMwb:
		(this->*mmf.pfn_vMwb)(CMenu::FromHandle((HMENU)wParam),
			LOWORD(lParam), (BOOL)HIWORD(lParam));
		return 0;


	case AfxSig_vW:
		(this->*mmf.pfn_vW)(CWnd::FromHandle((HWND)wParam));
		return 0;

	case AfxSig_vWww:
		(this->*mmf.pfn_vWww)(CWnd::FromHandle((HWND)wParam), LOWORD(lParam),
			HIWORD(lParam));
		return 0;

#ifndef _NTWIN
	case AfxSig_vWh:
		(this->*mmf.pfn_vWh)(CWnd::FromHandle((HWND)wParam),
				(HANDLE)LOWORD(lParam));
		return 0;
#else
	case AfxSig_vWh:
		(this->*mmf.pfn_vWh)(CWnd::FromHandle((HWND)wParam),
				(HANDLE)lParam);
		return 0;
#endif

#ifndef _NTWIN
	case AfxSig_vwW:
		(this->*mmf.pfn_vwW)(wParam, CWnd::FromHandle((HWND)LOWORD(lParam)));
		return 0;
#else
	case AfxSig_vwW:
		(this->*mmf.pfn_vwW)(wParam, CWnd::FromHandle((HWND)lParam));
		return 0;
#endif

#ifndef _NTWIN
	case AfxSig_vwWb:
		(this->*mmf.pfn_vwWb)(wParam, CWnd::FromHandle((HWND)LOWORD(lParam)),
			(BOOL)HIWORD(lParam));
		return 0;
#else
	case AfxSig_vwWb:
		(this->*mmf.pfn_vwWb)((UINT)(LOWORD(wParam)), 
			CWnd::FromHandle((HWND)lParam),
			(BOOL)(!(!(HIWORD(wParam)))));
		return 0;
#endif //_NTWIN

#ifndef _NTWIN
	case AfxSig_vwwW:
		(this->*mmf.pfn_vwwW)(wParam, LOWORD(lParam),
			CWnd::FromHandle((HWND)HIWORD(lParam)));
		return 0;
#else
	case AfxSig_vwwW:
		(this->*mmf.pfn_vwwW)((short)LOWORD(wParam), (short)HIWORD(wParam),
			CWnd::FromHandle((HWND)lParam));
		return 0;
#endif

	case AfxSig_vs:
		(this->*mmf.pfn_vs)((LPSTR)lParam);
		return 0;

#ifndef _NTWIN
	case AfxSig_wp:
		return (this->*mmf.pfn_wp)(*(CPoint*)&lParam);
#else
	case AfxSig_wp:
		{
			CPoint point((DWORD)lParam);
			return (this->*mmf.pfn_wp)(point);
		}

	case AfxSig_vwp:
		{
			CPoint point((DWORD)lParam);
			(this->*mmf.pfn_vwp)(wParam, point);
			return 0;
		}
#endif

	case AfxSig_wv: // AfxSig_bv, AfxSig_wv
		return (this->*mmf.pfn_wv)();

	case AfxSig_bh:
		return (this->*mmf.pfn_bh)((HANDLE)wParam);

	case AfxSig_vCALC:
		(this->*mmf.pfn_vCALC)((NCCALCSIZE_PARAMS FAR*)lParam);
		return 0;

	case AfxSig_vPOS:
		(this->*mmf.pfn_vPOS)((WINDOWPOS FAR*)lParam);
		return 0;
	}
	ASSERT(FALSE);      // not reached

LDispatchRegistered:    // for registered windows messages
	ASSERT(nMsg >= 0xC000);
	mmf.pfn = lpEntry->pfn;
	return (this->*mmf.pfn_lwl)(wParam, lParam);
}

BOOL CWnd::OnCommand(UINT wParam, LONG lParam)
{
#ifdef _NTWIN
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	UINT nNotifyCode = HIWORD(wParam);	// control specific
#else	
	UINT nID = wParam;
	HWND hWndCtrl = (HWND)LOWORD(lParam);
	UINT nNotifyCode = HIWORD(lParam);  // control specific
#endif
	if (nID == 0)
		return FALSE;       // 0 control IDs are not allowed !

	// default routing for command messages (through closure table)
	if (hWndCtrl == NULL)
		nNotifyCode = 0;        // accelerators are not special

	// check in message map table for matching control ID
	register CMessageMap* pMessageMap;
	register CMessageEntry FAR* lpEntry;

	for (pMessageMap = GetMessageMap(); pMessageMap != NULL;
	  pMessageMap = pMessageMap->pBaseMessageMap)
	{
		if ((lpEntry = FindMessageEntry(pMessageMap->lpEntries,
		  nNotifyCode, nID)) != NULL)
		{
#ifdef _DEBUG
			// diagnostic trace reporting of command notifications
			if (afxTraceFlags & 8)  // if command reporting
			{
				if (nNotifyCode == 0)
				{
					TRACE("SENDING command %d to %s window\n", nID,
						GetRuntimeClass()->m_pszClassName);
				}
				else if (afxTraceFlags & 4) // if verbose windows messages
				{
					TRACE("SENDING control notification %d from control id %d "
						"to %s window\n", nNotifyCode, nID,
						GetRuntimeClass()->m_pszClassName);
				}
			}
#endif
			// dispatch it
			(this->*lpEntry->pfn)();
			return TRUE;    // handled
		}
	}

#ifdef _DEBUG
	if (afxTraceFlags & 8)
	{
		if (nNotifyCode == 0)
		{
			TRACE("IGNORING command %d sent to %s window\n", nID,
					GetRuntimeClass()->m_pszClassName);
		}
		else if (afxTraceFlags & 4) // if verbose windows messages
		{
			TRACE("IGNORING control notification %d from control id %d "
				"to %s window\n", nNotifyCode, nID,
				GetRuntimeClass()->m_pszClassName);
		}
	}
#endif

	return FALSE;       // not handled
}



/////////////////////////////////////////////////////////////////////////////
