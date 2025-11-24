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

#include "afxdlgs.h"    // standard AFX dialogs

#include "stddef.h"     // for offsetof macro
#include "window_.h"    // for hooks and other internals 
#include "dlgs.h"       // for standard control IDs for commdlg

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_AUX_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static char BASED_CODE szLBSELCH[] = LBSELCHSTRING;
static char BASED_CODE szSHAREVI[] = SHAREVISTRING;
static char BASED_CODE szFILEOK[] = FILEOKSTRING;
static char BASED_CODE szCOLOROK[] = COLOROKSTRING;
static char BASED_CODE szSETRGB[] = SETRGBSTRING;

static UINT nMsgLBSELCHANGE = ::RegisterWindowMessage(szLBSELCH);
static UINT nMsgSHAREVI = ::RegisterWindowMessage(szSHAREVI);
static UINT nMsgFILEOK = ::RegisterWindowMessage(szFILEOK);
static UINT nMsgCOLOROK = ::RegisterWindowMessage(szCOLOROK);
static UINT nMsgSETRGB = ::RegisterWindowMessage(szSETRGB);

typedef UINT (FAR PASCAL* COMMDLGPROC)(HWND, UINT, UINT, LONG);

UINT FAR PASCAL AFX_EXPORT
_AfxCommDlgProc(HWND hWnd, register UINT message, UINT wParam, LONG lParam)
{
	if (message == WM_SETFONT || message == WM_INITDIALOG)
		return (UINT)_AfxDlgProc(hWnd, message, wParam, lParam);

	if (message < 0xC000)
		// not a ::RegisterWindowMessage message
		return 0;

	// RegisterWindowMessage - does not copy to lastState buffer, so
	// CWnd::GetCurrentMessage and CWnd::Default will NOT work
	// while in these handlers

	// Get our Window
	// assume it is already wired up to a permanent one
	CDialog* pDlg = (CDialog*) CWnd::FromHandlePermanent(hWnd);
	ASSERT(pDlg != NULL);
	ASSERT(pDlg->m_hWnd == hWnd);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CDialog)));

	// Dispatch special commdlg messages through our virtual callbacks
	if (message == nMsgSHAREVI)
	{
		ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFileDialog)));
		return ((CFileDialog*)pDlg)->OnShareViolation((LPCSTR)lParam);
	}
	else if (message == nMsgFILEOK)
	{
		ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFileDialog)));
		return ((CFileDialog*)pDlg)->OnFileNameOK();
	}
	else if (message == nMsgLBSELCHANGE)
	{
		ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFileDialog)));
		((CFileDialog*)pDlg)->OnLBSelChangedNotify(wParam, LOWORD(lParam), 
				HIWORD(lParam));
		return 0;
	}
	else if (message == nMsgCOLOROK)
	{
		ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CColorDialog)));
		return ((CColorDialog*)pDlg)->OnColorOK();
	}
	else if (message == nMsgSETRGB)
	{
		// nothing to do here, since this is a SendMessage
		return 0;
	}

	TRACE("_AfxCommDlgProc: received unknown user message, returning 0\n");
	return 0;
}

////////////////////////////////////////////////////////////////////////////
// FileOpen/FileSaveAs common dialog helper

IMPLEMENT_DYNAMIC(CFileDialog, CModalDialog)

CFileDialog::CFileDialog(BOOL bOpenFileDialog,
		LPCSTR lpszDefExt /* = NULL */,
		LPCSTR lpszFileName /* = NULL */,
		DWORD dwFlags /* = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT */,
		LPCSTR lpszFilter /* = NULL */, 
		CWnd* pParentWnd /* = NULL */) : CModalDialog((UINT)0, pParentWnd)
{
	memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
	m_szFileName[0] = '\0';
	m_szFileTitle[0] = '\0';

	m_bOpenFileDialog = bOpenFileDialog;
	
	m_ofn.lStructSize = sizeof(m_ofn);

	if (m_pParentWnd != NULL)
		m_ofn.hwndOwner = m_pParentWnd->m_hWnd;
	else
		m_ofn.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	m_ofn.lpstrFile = (LPSTR)&m_szFileName;
	m_ofn.nMaxFile = sizeof(m_szFileName);
	m_ofn.lpstrDefExt = lpszDefExt; 
	m_ofn.lpstrFileTitle = (LPSTR)m_szFileTitle;
	m_ofn.nMaxFileTitle = sizeof(m_szFileTitle);

	m_ofn.Flags |= dwFlags | OFN_ENABLEHOOK;
	m_ofn.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

	// setup initial file name
	if (lpszFileName)
	{
		_fstrncpy(m_szFileName, lpszFileName, sizeof(m_szFileName));
		m_szFileName[sizeof(m_szFileName)-1] = '\0';
	}

	// Translate filter into commdlg format (lots of \0)
	if (lpszFilter)
	{
		int nLen = _fstrlen(lpszFilter);

		m_strFilter.GetBuffer(_fstrlen(lpszFilter)); // required because of '\0'
		while (*lpszFilter)
		{
			if (*lpszFilter == '|') // MFC delimits with ':' not '\0'
				m_strFilter += (char)'\0';
			else
				m_strFilter += *lpszFilter;
			lpszFilter++;
		}
		m_ofn.lpstrFilter = m_strFilter;
		m_strFilter.ReleaseBuffer(nLen);
	}
}

int
CFileDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
	ASSERT(m_ofn.lpfnHook != NULL); // can still be a user hook

	BOOL bResult;

	_AfxHookWindowCreate(this);

	if (m_bOpenFileDialog)
		bResult = ::GetOpenFileName(&m_ofn);
	else
		bResult = ::GetSaveFileName(&m_ofn);

	_AfxUnhookWindowCreate();   // just in case
	Detach();                   // just in case

	return bResult ? IDOK : IDCANCEL;
}

CString CFileDialog::GetFileName() const
{ 
	ASSERT_VALID(this);
	char szFile[32];

	if (m_ofn.nFileExtension == 0 || 
			m_ofn.lpstrFile[m_ofn.nFileExtension] == '\0')
		return m_ofn.lpstrFile + m_ofn.nFileOffset;
	else
	{
		int nFileLen = m_ofn.nFileExtension - m_ofn.nFileOffset - 1;
		_fstrncpy(szFile, m_ofn.lpstrFile + m_ofn.nFileOffset, nFileLen);
		szFile[nFileLen] = '\0';
		return szFile;
	}
}

UINT 
CFileDialog::OnShareViolation(LPCSTR)
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	return OFN_SHAREWARN; // default
}

BOOL 
CFileDialog::OnFileNameOK()
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	return FALSE;
}

void 
CFileDialog::OnLBSelChangedNotify(UINT, UINT, UINT)
{
	ASSERT_VALID(this);

	// Do not call Default() if you override
	// no default processing needed
}

void
CFileDialog::OnOK()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

void
CFileDialog::OnCancel()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

#ifdef _DEBUG
void 
CFileDialog::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CModalDialog::Dump(dc);

	if (m_bOpenFileDialog)
		dc << "\nFile open dialog";
	else
		dc << "\nFile save dialog";
	dc << "\nm_ofn.hwndOwner = " << (void NEAR*)m_ofn.hwndOwner;
	dc << "\nm_ofn.nFilterIndex = " << m_ofn.nFilterIndex;
	dc << "\nm_ofn.lpstrFile = " << m_ofn.lpstrFile;
	dc << "\nm_ofn.nMaxFile = " << m_ofn.nMaxFile;
	dc << "\nm_ofn.lpstrFileTitle = " << m_ofn.lpstrFileTitle;
	dc << "\nm_ofn.nMaxFileTitle = " << m_ofn.nMaxFileTitle;
	dc << "\nm_ofn.lpstrTitle = " << m_ofn.lpstrTitle;
	dc << "\nm_ofn.Flags = " << (LPVOID)m_ofn.Flags;
	dc << "\nm_ofn.lpstrDefExt = " << m_ofn.lpstrDefExt;
	dc << "\nm_ofn.nFileOffset = " << m_ofn.nFileOffset;
	dc << "\nm_ofn.nFileExtension = " << m_ofn.nFileExtension;

	dc << "\nm_ofn.lpstrFilter = ";
	LPCSTR lpstrItem = m_ofn.lpstrFilter;
	char* szBreak = "|";

	while (lpstrItem != NULL && *lpstrItem != '\0')
	{
		dc << lpstrItem << szBreak;
		lpstrItem += _fstrlen(lpstrItem) + 1;
	}
	if (lpstrItem != NULL)
		dc << szBreak;

	dc << "\nm_ofn.lpstrCustomFilter = ";
	lpstrItem = m_ofn.lpstrCustomFilter;
	while (lpstrItem != NULL && *lpstrItem != '\0')
	{
		dc << lpstrItem << szBreak;
		lpstrItem += _fstrlen(lpstrItem) + 1;
	}
	if (lpstrItem != NULL)
		dc << szBreak;
	
	if (m_ofn.lpfnHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Choose Color dialog

IMPLEMENT_DYNAMIC(CColorDialog, CModalDialog)

COLORREF CColorDialog::clrSavedCustom[16] = { RGB(255, 255, 255), 
	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255) };

CColorDialog::CColorDialog(COLORREF clrInit /* = 0 */,
		DWORD dwFlags /* = 0 */, 
		CWnd* pParentWnd /* = NULL */) : CModalDialog((UINT)0, pParentWnd)
{
	memset(&m_cc, 0, sizeof(m_cc));

	m_cc.lStructSize = sizeof(m_cc);
	if (m_pParentWnd != NULL)
		m_cc.hwndOwner = m_pParentWnd->m_hWnd;
	else
		m_cc.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	m_cc.lpCustColors = (COLORREF FAR*)&clrSavedCustom;
	m_cc.Flags = dwFlags | CC_ENABLEHOOK;
	m_cc.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

	if ((m_cc.rgbResult = clrInit) != 0)
		m_cc.Flags |= CC_RGBINIT;

	// There is a "bug" in the Windows 3.1 COMMDLG implementation
	// of ::ChooseColor that prevents the "new AFX look" from
	// functioning correctly.  If you wish to disable the default
	// AFX look, then ::DeleteObject(m_hBrushCtlBk) and set it
	// to NULL at this point.
}

int
CColorDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_cc.Flags & CC_ENABLEHOOK);
	ASSERT(m_cc.lpfnHook != NULL); // can still be a user hook

	BOOL bResult;

	_AfxHookWindowCreate(this);

	bResult = ::ChooseColor(&m_cc);

	_AfxUnhookWindowCreate();   // just in case
	Detach();                   // just in case

	return bResult ? IDOK : IDCANCEL;
}

BOOL 
CColorDialog::OnColorOK()
{
	ASSERT_VALID(this);
	// Do not call Default() if you override
	return FALSE;
}

void 
CColorDialog::SetCurrentColor(COLORREF clr)
{
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	SendMessage(nMsgSETRGB, 0, (DWORD)clr);
}

void
CColorDialog::OnOK()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);

	Default();
}

void
CColorDialog::OnCancel()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

#ifdef _DEBUG
void 
CColorDialog::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);
	CModalDialog::Dump(dc);
	
	dc << "\nm_cc.hwndOwner = " << (void NEAR*)m_cc.hwndOwner;
	dc << "\nm_cc.rgbResult = " << (LPVOID)m_cc.rgbResult;
	dc << "\nm_cc.Flags = " << (LPVOID)m_cc.Flags;
	dc << "\nm_cc.lpCustColors ";
	for (int iClr = 0; iClr < 16; iClr++)
		dc << "\n\t" << (LPVOID)m_cc.lpCustColors[iClr];
	if (m_cc.lpfnHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Choose Font dialog

IMPLEMENT_DYNAMIC(CFontDialog, CModalDialog)

CFontDialog::CFontDialog(LPLOGFONT lplfInitial  /* = NULL */,
		DWORD dwFlags /* = CF_EFFECTS | CF_SCREENFONTS */, 
		CDC* pdcPrinter /* = NULL */,
		CWnd* pParentWnd /* = NULL */) : CModalDialog((UINT)0, pParentWnd)
{
	memset(&m_cf, 0, sizeof(m_cf));
	memset(&m_lf, 0, sizeof(m_lf));
	memset(&m_szStyleName, 0, sizeof(m_szStyleName));

	m_cf.lStructSize = sizeof(m_cf);
	if (m_pParentWnd != NULL)
		m_cf.hwndOwner = m_pParentWnd->m_hWnd;
	else
		m_cf.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	m_cf.lpszStyle = (LPSTR)&m_szStyleName;
	m_cf.Flags = dwFlags | CF_USESTYLE | CF_ENABLEHOOK;
	m_cf.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;

	if (lplfInitial)
	{
		m_cf.lpLogFont = lplfInitial;
		m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
		_fmemcpy(&m_lf, m_cf.lpLogFont, sizeof(m_lf));
	}
	else
	{
		m_cf.lpLogFont = &m_lf;
	}

	if (pdcPrinter)
	{
		ASSERT(pdcPrinter->m_hDC != NULL);
		m_cf.hDC = pdcPrinter->m_hDC;
		m_cf.Flags |= CF_PRINTERFONTS;
	}
}

BOOL 
CFontDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_cf.Flags & CF_ENABLEHOOK);
	ASSERT(m_cf.lpfnHook != NULL); // can still be a user hook

	BOOL bResult;

	_AfxHookWindowCreate(this);

	bResult = ::ChooseFont(&m_cf);

	_AfxUnhookWindowCreate();   // just in case
	Detach();                   // just in case

	if (bResult)
		// copy logical font from user's initialization buffer (if needed)
		_fmemcpy(&m_lf, m_cf.lpLogFont, sizeof(m_lf));
		
	return bResult ? IDOK : IDCANCEL;
}


void
CFontDialog::OnOK()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

void
CFontDialog::OnCancel()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

#ifdef _DEBUG
void 
CFontDialog::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CModalDialog::Dump(dc);
	dc << "\nm_cf.hwndOwner = " << (void NEAR*)m_cf.hwndOwner;
	if (m_cf.hDC != NULL)
		dc << "\nm_cf.hDC = " << CDC::FromHandle(m_cf.hDC);
	dc << "\nm_cf.iPointSize = " << m_cf.iPointSize;
	dc << "\nm_cf.Flags = " << (LPVOID)m_cf.Flags;
	dc << "\nm_cf.lpszStyle = " << m_cf.lpszStyle;
	dc << "\nm_cf.nSizeMin = " << m_cf.nSizeMin;
	dc << "\nm_cf.nSizeMax = " << m_cf.nSizeMax;
	dc << "\nm_cf.nFontType = " << (void NEAR*)m_cf.nFontType;
	dc << "\nm_cf.rgbColors = " << (LPVOID)m_cf.rgbColors;
	if (m_cf.lpfnHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
}

#endif

/////////////////////////////////////////////////////////////////////////////
// Print/Print Setup dialog

IMPLEMENT_DYNAMIC(CPrintDialog, CModalDialog)

BEGIN_MESSAGE_MAP(CPrintDialog, CModalDialog)
	// Handle the print setup button when print is displayed
	ON_COMMAND(psh1, OnPrintSetup)
END_MESSAGE_MAP()

CPrintDialog::CPrintDialog(BOOL bPrintSetupOnly,
	DWORD dwFlags /* = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
		| PD_HIDEPRINTTOFILE | PD_NOSELECTION */,
	CWnd* pParentWnd /* = NULL */) 
		: m_pd(m_pdActual), CModalDialog((UINT)0, pParentWnd)
{
	memset(&m_pdActual, 0, sizeof(m_pdActual));

	m_pd.lStructSize = sizeof(m_pdActual);
	if (m_pParentWnd != NULL)
		m_pd.hwndOwner = m_pParentWnd->m_hWnd;
	else
		m_pd.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	m_pd.Flags = (dwFlags | PD_ENABLEPRINTHOOK | PD_ENABLESETUPHOOK);
	m_pd.lpfnPrintHook = (COMMDLGPROC)_AfxCommDlgProc;
	m_pd.lpfnSetupHook = (COMMDLGPROC)_AfxCommDlgProc;
		

	if (bPrintSetupOnly)
		m_pd.Flags |= PD_PRINTSETUP;
	else
		m_pd.Flags |= PD_RETURNDC;

	m_pd.Flags &= ~PD_RETURNIC; // do not support information context
}

// Helper ctor for AttachOnSetup
CPrintDialog::CPrintDialog(PRINTDLG FAR& pdInit)
		: m_pd(pdInit), CModalDialog((UINT)0, NULL)
{   
}

// Function to keep m_pd in sync after user invokes Setup from
// the print dialog (via the Setup button)
// If you decide to handle any messages/notifications and wish to
// handle them differently between Print/PrintSetup then override
// this function and create an object of a derived class
CPrintDialog* 
CPrintDialog::AttachOnSetup()
{
	ASSERT_VALID(this);

	CPrintDialog* pDlgSetup;

	pDlgSetup = new CPrintDialog(m_pd);
	pDlgSetup->m_hWnd = NULL;
	pDlgSetup->m_pParentWnd = m_pParentWnd;
	return pDlgSetup;
}

void
CPrintDialog::OnPrintSetup()
{
	ASSERT_VALID(this);

	CPrintDialog* pDlgSetup;

	VERIFY((pDlgSetup = this->AttachOnSetup()) != NULL);

	_AfxHookWindowCreate(pDlgSetup);
	Default();
	_AfxUnhookWindowCreate();

	delete pDlgSetup;
}

int
CPrintDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_pd.Flags & PD_ENABLEPRINTHOOK);
	ASSERT(m_pd.Flags & PD_ENABLESETUPHOOK);
	ASSERT(m_pd.lpfnPrintHook != NULL); // can still be a user hook
	ASSERT(m_pd.lpfnSetupHook != NULL); // can still be a user hook

	BOOL bResult;

	_AfxHookWindowCreate(this);

	bResult = ::PrintDlg(&m_pd);

	_AfxUnhookWindowCreate();   // just in case
	Detach();                   // just in case

	return bResult ? IDOK : IDCANCEL;
}

// Return an HDC.  We don't return a CDC* so the user can decide
// where to attach this HDC: either to a newly allocated object
// (use operator delete to clean up) or to an embedded/frame
// object (destructor will clean up when leaving scope)
HDC
CPrintDialog::GetPrinterDC() const
{
	ASSERT_VALID(this);
	ASSERT(m_pd.Flags & PD_RETURNDC);
	
	return m_pd.hDC;
}

int 
CPrintDialog::GetCopies() const
{   
	ASSERT_VALID(this);
	if (m_pd.Flags & PD_USEDEVMODECOPIES)
		return GetDevMode()->dmCopies;
	else
		return m_pd.nCopies; 
}

void
CPrintDialog::OnOK()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

void
CPrintDialog::OnCancel()
{
	// Common dialogs do not require ::EndDialog
	ASSERT_VALID(this);
	Default();
}

#ifdef _DEBUG
void 
CPrintDialog::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);
	CModalDialog::Dump(dc);

	dc << "\nm_pd.hwndOwner = " << (void NEAR*)m_pd.hwndOwner;
	if (m_pd.hDC != NULL)
		dc << "\nm_pd.hDC = " << CDC::FromHandle(m_pd.hDC);
	dc << "\nm_pd.Flags = " << (LPVOID)m_pd.Flags;
	dc << "\nm_pd.nFromPage = " << m_pd.nFromPage;
	dc << "\nm_pd.nToPage = " << m_pd.nToPage;
	dc << "\nm_pd.nMinPage = " << m_pd.nMinPage;
	dc << "\nm_pd.nMaxPage = " << m_pd.nMaxPage;
	dc << "\nm_pd.nCopies = " << m_pd.nCopies;
	if (m_pd.lpfnSetupHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nsetup hook function set to standard MFC hook function";
	else
		dc << "\nsetup hook function set to non-standard hook function";
	if (m_pd.lpfnPrintHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nprint hook function set to standard MFC hook function";
	else
		dc << "\nprint hook function set to non-standard hook function";

}
#endif

/////////////////////////////////////////////////////////////////////////////
// Find/FindReplace dialogs

IMPLEMENT_DYNAMIC(CFindReplaceDialog, CDialog)

CFindReplaceDialog::CFindReplaceDialog() 
{
	memset(&m_fr, 0, sizeof(m_fr));
	m_szFindWhat[0] = '\0';
	m_szReplaceWith[0] = '\0';

	m_fr.Flags = FR_ENABLEHOOK;
	m_fr.lpfnHook = (COMMDLGPROC)_AfxCommDlgProc;
	m_fr.lStructSize = sizeof(m_fr);
	m_fr.lpstrFindWhat = (LPSTR)m_szFindWhat;
}

void
CFindReplaceDialog::PostNcDestroy()
{
	ASSERT(m_hWnd == NULL);
	delete this;
}


BOOL
CFindReplaceDialog::Create(BOOL bFindDialogOnly,
		LPCSTR lpszFindWhat,
		LPCSTR lpszReplaceWith /* = NULL */,
		DWORD dwFlags /* = FR_DOWN */,
		CWnd* pParentWnd /* = NULL */)
{
	ASSERT_VALID(this);
	ASSERT(m_fr.Flags & FR_ENABLEHOOK);
	ASSERT(m_fr.lpfnHook != NULL);

	HWND hwndResult;

	m_fr.wFindWhatLen = sizeof(m_szFindWhat);
	m_fr.lpstrReplaceWith = (LPSTR)m_szReplaceWith;
	m_fr.wReplaceWithLen = sizeof(m_szReplaceWith);
	m_fr.Flags |= dwFlags;

	if (pParentWnd == NULL)
		m_fr.hwndOwner = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
	else
	{
		ASSERT_VALID(pParentWnd);
		m_fr.hwndOwner = pParentWnd->m_hWnd;
	}
	ASSERT(m_fr.hwndOwner != NULL); // must have a parent for modeless dialog
	

	if (lpszFindWhat)
		_fstrncpy(m_szFindWhat, lpszFindWhat, sizeof(m_szFindWhat));

	if (lpszReplaceWith)
		_fstrncpy(m_szReplaceWith, lpszReplaceWith, sizeof(m_szReplaceWith));


	_AfxHookWindowCreate(this);
	if (bFindDialogOnly)
		hwndResult = ::FindText(&m_fr);
	else
		hwndResult = ::ReplaceText(&m_fr);
	_AfxUnhookWindowCreate();   // just in case

	ASSERT(hwndResult == NULL || m_hWnd != NULL);

	return hwndResult == NULL ? FALSE : TRUE;
}

/* static */ CFindReplaceDialog* 
CFindReplaceDialog::GetNotifier(LONG lParam)
{
	ASSERT(lParam != NULL);
	CFindReplaceDialog* pDlg;

	pDlg = (CFindReplaceDialog*)(lParam - offsetof(CFindReplaceDialog, m_fr));
	ASSERT_VALID(pDlg);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFindReplaceDialog)));
	
	return pDlg;
}

#ifdef _DEBUG
void 
CFindReplaceDialog::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CDialog::Dump(dc);

	dc << "\nm_fr.hwndOwner = " << (void NEAR*)m_fr.hwndOwner;
	dc << "\nm_fr.Flags = " << (LPVOID)m_fr.Flags;
	dc << "\nm_fr.lpstrFindWhat = " << m_fr.lpstrFindWhat;
	dc << "\nm_fr.lpstrReplaceWith = " << m_fr.lpstrReplaceWith;
	if (m_fr.lpfnHook == (COMMDLGPROC)_AfxCommDlgProc)
		dc << "\nhook function set to standard MFC hook function";
	else
		dc << "\nhook function set to non-standard hook function";
}
#endif

/////////////////////////////////////////////////////////////////////////////
