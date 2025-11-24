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
#include "window_.h"

#ifndef _NTWIN
#include "penwin.h"     // MFC Apps are PenAware by default
#endif

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#include "trace_.h"
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Define global state in ordinary "C" globals

extern "C"
{
CWinApp* afxCurrentWinApp = NULL;
HANDLE afxCurrentInstanceHandle = NULL;
HANDLE afxCurrentResourceHandle = NULL;
const char* afxCurrentAppName = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// other globals (internal library use)

// Proc addresses for Win3.1 specifics
#ifndef _WINDLL
#ifndef _NTWIN
HOOKPROC (WINAPI* _afxSetWindowsHookExProc)(int, HOOKPROC, HINSTANCE, HTASK);
static void (FAR PASCAL *_afxRegisterPenAppProc)(UINT, BOOL);
#endif
#endif //!_WINDLL

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CWinApp, CObject)

#ifdef _DEBUG
void CWinApp::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(afxCurrentWinApp == this);
	ASSERT(afxCurrentInstanceHandle == m_hInstance);
}

void CWinApp::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
	dc << "\nm_hInstance = " << (UINT)m_hInstance;
	dc << "\nm_hPrevInstance = " << (UINT)m_hPrevInstance;
	dc << "\nm_lpCmdLine = " << m_lpCmdLine;
	dc << "\nm_nCmdShow = " << m_nCmdShow;
	dc << "\nm_pMainWnd = " << m_pMainWnd;
}
#endif

inline void CWinApp::SetCurrentHandles()
{
	ASSERT(this == afxCurrentWinApp);
	afxCurrentInstanceHandle = m_hInstance; // for instance tagging
	afxCurrentResourceHandle = m_hInstance; // for resource loading

	if (m_pszAppName == NULL)
	{
		// get name/path of executable
		char szName[256];
		::GetModuleFileName(m_hInstance, szName, sizeof(szName)-1);
		m_pszAppName = _strdup(szName);
	}
	afxCurrentAppName = m_pszAppName;
	ASSERT(afxCurrentAppName != NULL);
}

/////////////////////////////////////////////////////////////////////////////

CWinApp::CWinApp(const char* pszAppName)
{
	m_pszAppName = pszAppName;
	// in non-running state until WinMain
	m_hInstance = NULL;
	m_pMainWnd = NULL;

	ASSERT(afxCurrentWinApp == NULL);   // only one CWinApp object please
	afxCurrentWinApp = this;        // hook for WinMain

#ifdef _DEBUG
	m_nDisablePumpCount = 0;
#endif
}

#ifdef _DEBUG
void CWinApp::EnablePump(BOOL bEnable)
{
	if (bEnable)
		m_nDisablePumpCount--;
	else
		m_nDisablePumpCount++;
	ASSERT(m_nDisablePumpCount >= 0);
}
#endif // _DEBUG


BOOL CWinApp::PumpMessage()
{
#ifdef _DEBUG
	if (m_nDisablePumpCount != 0)
	{
		TRACE("Error: CWinApp::PumpMessage() called when not permitted\n");
		ASSERT(FALSE);
	}
#endif

	if (!::GetMessage(&m_msgCur, NULL, NULL, NULL))
	{
#ifdef _DEBUG
		if (afxTraceFlags & 2)
			TRACE("PumpMessage - Received WM_QUIT\n");
		m_nDisablePumpCount++; // application must die
			// NOTE: prevents calling message loop things in 'ExitInstance'
			// will never be decremented
#endif
		return FALSE;
	}

#ifdef _DEBUG
	if (afxTraceFlags & 2)
		AfxTraceMsg("PumpMessage", &m_msgCur);
#endif

	// process this message
	if (!PreTranslateMessage(&m_msgCur))
	{
		::TranslateMessage(&m_msgCur);
		::DispatchMessage(&m_msgCur);
	}
	return TRUE;
}

int CWinApp::Run()
{
	/* Acquire and dispatch messages until a WM_QUIT message is received. */

	while (1)
	{
		LONG lIdleCount = 0;
		// check to see if we can do idle work
		while (!::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
			OnIdle(lIdleCount++))
		{
			// more work to do
		}
		
		// either we have a message, or OnIdle returned false

		if (!PumpMessage())
			break;
	}

	return ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Stubs for standard initialization

BOOL CWinApp::InitApplication()
{
	return TRUE;
}

BOOL CWinApp::InitInstance()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Stubs for standard implementation

BOOL CWinApp::PreTranslateMessage(MSG* pMsg)
{
	register HWND hWnd;
	register CWnd* pWnd;

	// walk from the target window up to the desktop window seeing
	//  if any window wants to translate this message
	for (hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
	{
		if ((pWnd = CWnd::FromHandlePermanent(hWnd)) != NULL)
		{
			// target window is a C++ window
			if (pWnd->PreTranslateMessage(pMsg))
				return TRUE; // trapped by target window (eg: accelerators)

			if (pWnd == m_pMainWnd)
				return FALSE;       // got to our main window without interest
		}
	}

	// in case of modeless dialogs, last chance route through main window's
	//   accelerator table
	if (m_pMainWnd != NULL && m_pMainWnd->PreTranslateMessage(pMsg))
		return TRUE; // trapped by main window (eg: accelerators)
	
	return FALSE;       // no special processing
}


BOOL CWinApp::OnIdle(LONG /*lCount*/)
{
	CGdiObject::DeleteTempMap();
	CDC::DeleteTempMap();
	CMenu::DeleteTempMap();
	CWnd::DeleteTempMap();
	return FALSE;   // no more processing (sleep please)
}

int CWinApp::ExitInstance()
{
	return m_msgCur.wParam; // Returns the value from PostQuitMessage
}

/////////////////////////////////////////////////////////////////////////////
// Standard init called by WinMain

static BOOL NEAR RegisterWithIcon(register WNDCLASS* pWndCls,
	const char* szClassName, UINT nIDIcon)
{
	pWndCls->lpszClassName = szClassName;
	if ((pWndCls->hIcon = ::LoadIcon(pWndCls->hInstance,
	  MAKEINTRESOURCE(nIDIcon))) == NULL)
	{
		// use default icon
		pWndCls->hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
	}
	return RegisterClass(pWndCls);
}

/////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL AfxWinInit(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	register CWinApp* pApp = AfxGetApp();

	ASSERT(pApp != NULL);   // must have one

	// fill in the initial state for the application
	pApp->m_hInstance = hInstance;
	pApp->m_hPrevInstance = hPrevInstance;
	pApp->m_lpCmdLine = lpCmdLine;
	pApp->m_nCmdShow = nCmdShow;
	pApp->SetCurrentHandles();

	// Windows version specific initialization
#ifndef _WINDLL
	WORD wVersion = LOWORD(::GetVersion());
	if (LOBYTE(wVersion) > 3 || HIBYTE(wVersion) >= 10)
	{
#ifndef _NTWIN
		HINSTANCE hPenWin;
		if ((hPenWin = (HINSTANCE)GetSystemMetrics(SM_PENWINDOWS)) != NULL)
		{
			static char BASED_CODE szRegisterPenApp[] = "RegisterPenApp";
			_afxRegisterPenAppProc = (void (WINAPI*)(UINT, BOOL))
				::GetProcAddress(hPenWin, szRegisterPenApp);
			if (_afxRegisterPenAppProc != NULL)
				(*_afxRegisterPenAppProc)(RPA_DEFAULT, TRUE);
		}

		// Windows 3.1 or better - use SetWindowsHookEx (USER.291)
		// Under Windows/NT we call SetWindowsHookEx directly.
		static char BASED_CODE szUSER[] = "USER";
		_afxSetWindowsHookExProc =
			(HOOKPROC (WINAPI*)(int, HOOKPROC, HINSTANCE, HTASK))
			::GetProcAddress(::GetModuleHandle(szUSER), MAKEINTRESOURCE(291));
#endif //!_NTWIN
	}

#endif //!_WINDLL

	if (hPrevInstance == NULL)  // one instance initialization
	{
		// register basic WndClasses
		WNDCLASS wndcls;
		memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults

		// common initialization
		wndcls.lpfnWndProc  = AfxWndProc;
		wndcls.hInstance    = hInstance;
		wndcls.hCursor      = ::LoadCursor(NULL, IDC_ARROW);
		wndcls.style        = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;

		// Child windows - no brush, no icon
		wndcls.lpszClassName = _afxWnd;
		if (!::RegisterClass(&wndcls))
			return FALSE;

		// MDI Frame windows
		if (!RegisterWithIcon(&wndcls, _afxMDIFrameWnd, AFX_IDI_STD_MDIFRAME))
			return FALSE;

		// SDI Frame or MDI Child windows - normal colors
		wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		if (!RegisterWithIcon(&wndcls, _afxFrameWnd, AFX_IDI_STD_FRAME))
			return FALSE;
	}

	return TRUE;
}

extern "C"
void AfxWinTerm(void)
{
	// These static CWnd objects refer to HWNDs that don't exist
	// so let's not call ::DestroyWindow when CWnd::~CWnd() is invoked.
	((CWnd&)CWnd::wndTop).m_hWnd = NULL;
	((CWnd&)CWnd::wndBottom).m_hWnd = NULL;
	((CWnd&)CWnd::wndTopMost).m_hWnd = NULL;
	((CWnd&)CWnd::wndNoTopMost).m_hWnd = NULL;

#ifndef _WINDLL
#ifndef _NTWIN
	// if we registered ourself with PenWin, deregister now
	if (_afxRegisterPenAppProc != NULL)
		(*_afxRegisterPenAppProc)(RPA_DEFAULT, FALSE);
#endif //!_NTWIN
#endif //!_WINDLL
}

/////////////////////////////////////////////////////////////////////////////
// force WinMain or LibMain inclusion

#ifdef _WINDLL
#ifndef _NTWIN
extern "C" int PASCAL LibMain(HINSTANCE, WORD, WORD, LPSTR);
static FARPROC linkAddr = (FARPROC) &LibMain;
#else
extern "C" int WINAPI AfxLibMain(HINSTANCE, DWORD, LPVOID);
static FARPROC linkAddr = (FARPROC) &AfxLibMain;
#endif //!_NTWIN
#else
extern "C" int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
static FARPROC linkAddr = (FARPROC) &WinMain;
#endif //!_WINDLL

/////////////////////////////////////////////////////////////////////////////
