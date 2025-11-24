/****************************************************************************
 *
 *   captevw.cpp: implementation of the CCaptestView class
 * 
 *   Microsoft Video for Windows Capture Class Sample Program
 *     
 *
 *   Warning:   Some playback hardware does not support an active capture 
 *              window simultaneous with an active playback window.
 *              For more information, see the comments in the 
 *              CCaptestView::OnCapPlayback method below.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include "stdafx.h"
#include <windowsx.h>
#include "captest.h"
#include "captedoc.h"
#include "captevw.h"
#include "mainfrm.h" 
#include "capparms.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////
// CCaptestView
///////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCaptestView, CView)

BEGIN_MESSAGE_MAP(CCaptestView, CView)
	//{{AFX_MSG_MAP(CCaptestView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_CAP_OVERLAY, OnCapOverlay)
	ON_COMMAND(ID_CAP_PREVIEW, OnCapPreview)
	ON_COMMAND(ID_CAP_DLG_FORMAT, OnCapDlgFormat)
	ON_COMMAND(ID_CAP_DLG_DISPLAY, OnCapDlgDisplay)
	ON_COMMAND(ID_CAP_DLG_SOURCE, OnCapDlgSource)
	ON_COMMAND(ID_CAP_AUTOPAL5, OnCapAutopal5)
	ON_COMMAND(ID_CAP_SEQUENCE, OnCapSequence)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_CAP_PREVIEW, OnUpdateCapPreview)
	ON_UPDATE_COMMAND_UI(ID_CAP_OVERLAY, OnUpdateCapOverlay)
	ON_COMMAND(ID_CAP_SCALE, OnCapScale)
	ON_UPDATE_COMMAND_UI(ID_CAP_SCALE, OnUpdateCapScale)
	ON_UPDATE_COMMAND_UI(ID_CAP_DLG_DISPLAY, OnUpdateCapDlgDisplay)
	ON_UPDATE_COMMAND_UI(ID_CAP_DLG_FORMAT, OnUpdateCapDlgFormat)
	ON_UPDATE_COMMAND_UI(ID_CAP_DLG_SOURCE, OnUpdateCapDlgSource)
	ON_COMMAND(ID_CAP_SETUP, OnCapSetup)
	ON_COMMAND(ID_CAP_PLAYBACK, OnCapPlayback)
	ON_UPDATE_COMMAND_UI(ID_CAP_PLAYBACK, OnUpdateCapPlayback)
	ON_COMMAND(ID_CAP_DRV0, OnCapDrv0)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV0, OnUpdateCapDrv)
	ON_COMMAND(ID_CAP_DRV1, OnCapDrv1)
	ON_COMMAND(ID_CAP_DRV2, OnCapDrv2)
	ON_COMMAND(ID_CAP_DRV3, OnCapDrv3)
	ON_COMMAND(ID_CAP_DRV4, OnCapDrv4)
	ON_COMMAND(ID_CAP_DRV5, OnCapDrv5)
	ON_COMMAND(ID_CAP_DRV6, OnCapDrv6)
	ON_COMMAND(ID_CAP_DRV7, OnCapDrv7)
	ON_COMMAND(ID_CAP_DRV8, OnCapDrv8)
	ON_COMMAND(ID_CAP_DRV9, OnCapDrv9)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_UPDATE_COMMAND_UI(ID_CAP_AUTOPAL5, OnUpdateCapAutopal5)
	ON_UPDATE_COMMAND_UI(ID_CAP_SEQUENCE, OnUpdateCapSequence)
	ON_UPDATE_COMMAND_UI(ID_CAP_SETUP, OnUpdateCapSetup)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV1, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV2, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV3, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV4, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV5, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV6, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV7, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV8, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_CAP_DRV9, OnUpdateCapDrv)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste) 
	ON_WM_QUERYENDSESSION ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
							
static int nMenuFixed;
							
///////////////////////////////////////////////
// CCaptestView construction/destruction
///////////////////////////////////////////////
CCaptestView::CCaptestView()
{
	hwndPlayback = NULL;
	fDialogIsUp = FALSE;
}

CCaptestView::~CCaptestView()
{

}

///////////////////////////////////////////////
// CCaptestView drawing
///////////////////////////////////////////////
void CCaptestView::OnDraw(
CDC* pDC)
{
	CCaptestDoc* pDoc = GetDocument();

	// TODO: add draw code here
}



/////////////////////////////////////////////////////////////////////////////
// CCaptestView diagnostics

#ifdef _DEBUG
void CCaptestView::AssertValid() const
{
	CView::AssertValid();
}

void CCaptestView::Dump(
CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCaptestDoc* CCaptestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCaptestDoc)));
	return (CCaptestDoc*) m_pDocument;
}

#endif //_DEBUG


// MCIWnd won't paint properly if the parent window does not have
// the WS_CLIPCHILDREN style set.  Since this is not enabled by
// default in MFC, we must manually set the flag...

BOOL CCaptestView::PreCreateWindow(
CREATESTRUCT &cs)
{
	cs.style |= WS_CLIPCHILDREN; 
	
	// Do default processing.
	if(CView::PreCreateWindow(cs)==0) 
		return FALSE;
	
	cs.style |= WS_CLIPCHILDREN; 
	
	return TRUE;
}

////////////////////////////////////////////////////
// Callback functions for Status and Error Messages
////////////////////////////////////////////////////

LRESULT FAR PASCAL EXPORT StatusMessageCallback(
HWND hwnd, 
int nID, 
LPCSTR lpszStatusText)
{
	if (lpszStatusText == NULL) {
		char buf[80];
		if (nID == 0)
			lstrcpy (buf, "");
		else
		{
			CString cs;
			//wsprintf (buf, "StatusValue = %d", nID);
			cs.LoadString(IDS_STATVALEQPCNTD);
			wsprintf (buf, (const char*)cs, nID);
		}
		((CMainFrame *) (AfxGetApp()->m_pMainWnd))
			->StatusCallback (nID, buf);
	}
	else {
		// If the status message is anything except "Capture End", display it
		if (nID != IDS_CAP_END)
			((CMainFrame *) (AfxGetApp()->m_pMainWnd))
				->StatusCallback (nID, lpszStatusText);
	}
	return 0L;
}
 
LRESULT FAR PASCAL EXPORT ErrorMessageCallback(
HWND hwnd, 
int nID, 
LPCSTR lpszErrorText)
{
	if (nID == 0)
		return 0L;
		
	AfxMessageBox (lpszErrorText, MB_OK, 0);
	return 0L;
}


// Append a list of current capture drivers to our menu
void CCaptestView::GetDriverList()
{ 
	int j;
	char szName[80];
	char szVer[80];
	CMenu* menu;
	CMenu* submenu;
		
	menu = AfxGetApp()->m_pMainWnd->GetMenu();
	
	submenu = menu-> GetSubMenu ( 3);
	for (j =0; j < 10; j++) {
		// Initially, all 10 selections are in the menu
		// to enable the auto status bar messages
		submenu->DeleteMenu(ID_CAP_DRV0 + j, MF_BYCOMMAND);
		if (capGetDriverDescription ((UINT) j,
				szName, sizeof (szName),
				szVer, sizeof (szVer))) {
			submenu->AppendMenu(MF_ENABLED, ID_CAP_DRV0 + j, szName) ;
		}
	}

}

 
///////////////////////////////////////////////
// Create a capture window
///////////////////////////////////////////////
int CCaptestView::OnCreate(
LPCREATESTRUCT lpCreateStruct)
{
	static int nID = 0;
	int nIndex;

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	RECT rc;
	GetClientRect (&rc);
	
	// Create a new capture window
	hwndC = capCreateCaptureWindow ("", 
			WS_CHILD | WS_VISIBLE, 
			0, 0,
			160, 120,
			m_hWnd,
			nID++);
	// Connect to the next available driver
	for (nIndex = 0; nIndex < 10; nIndex++)
		if (capDriverConnect (hwndC, nIndex))
			break;
			
	nDriverIndex = ((nIndex == 10) ? -1 : nIndex);
	
	// Find out what the driver can do
	if (!capDriverGetCaps (hwndC, &CapDriverCaps, sizeof (CapDriverCaps))) {
		_fmemset (&CapDriverCaps, 0, sizeof (CapDriverCaps));
		_fmemset (&CapStatus, 0, sizeof (CapStatus));
	}
		
	// Retrieve the default capture settings
	capCaptureGetSetup (hwndC, &CaptureParms, sizeof (CaptureParms));
	ResizeNow ();
	
	// Append the list of drivers to the menu
	if (!nMenuFixed++)
		GetDriverList();
				
	capPreviewRate (hwndC, 66);	// Set the preview rate to 66 milliseconds
	if (CapDriverCaps.fHasOverlay)	// Enable overlay if the card supports it
		capOverlay (hwndC, TRUE);
	else
		capPreview (hwndC, TRUE);

	// Set error and status callbacks
	capSetCallbackOnStatus (hwndC,
		(LPVOID) MakeProcInstance ((FARPROC) StatusMessageCallback, 
		AfxGetInstanceHandle() )); 
				
	capSetCallbackOnError (hwndC, 
		(LPVOID) MakeProcInstance ((FARPROC) ErrorMessageCallback, 
		AfxGetInstanceHandle() ));
				

	return 0;
}



///////////////////////////////////////////////
// Preview and Overlay selection
///////////////////////////////////////////////
void CCaptestView::OnCapOverlay()
{
	capGetStatus (hwndC, &CapStatus, sizeof (CAPSTATUS));
	CapStatus.fOverlayWindow = !CapStatus.fOverlayWindow; 
	capOverlay (hwndC, CapStatus.fOverlayWindow);
}

void CCaptestView::OnCapPreview()
{
	capGetStatus (hwndC, &CapStatus, sizeof (CAPSTATUS));
	CapStatus.fLiveWindow = !CapStatus.fLiveWindow; 
	capPreview (hwndC, CapStatus.fLiveWindow);
}

void CCaptestView::OnUpdateCapPreview(
CCmdUI* pCmdUI)
{
	capGetStatus (hwndC, &CapStatus, sizeof (CapStatus));
	pCmdUI->SetCheck (CapStatus.fLiveWindow);
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
}

void CCaptestView::OnUpdateCapOverlay(
CCmdUI* pCmdUI)
{
	capGetStatus (hwndC, &CapStatus, sizeof (CapStatus));
	pCmdUI->SetCheck (CapStatus.fOverlayWindow);
	pCmdUI->Enable (!hwndPlayback && CapDriverCaps.fHasOverlay 
			&& CapDriverCaps.fCaptureInitialized);
}

// Raise the preview rate if we get focus
void CCaptestView::OnSetFocus(
CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	
	::SendMessage (hwndC, WM_QUERYNEWPALETTE, 0, 0);
	capPreviewRate (hwndC, 66);
}

// Lower preview rate if we lose focus
void CCaptestView::OnKillFocus(
CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	
	capPreviewRate (hwndC, 500);
}

///////////////////////////////////////////////
// Preview scaling
/////////////////////////////////////////////// 
void CCaptestView::OnCapScale()
{
	RECT rc;

	GetClientRect (&rc);
	CapStatus.fScale = !CapStatus.fScale;
	capPreviewScale (hwndC, CapStatus.fScale); // Scale preview to the window
	if (CapStatus.fScale)
		::SetWindowPos (hwndC, NULL, 0, 0, rc.right, rc.bottom, 
			SWP_NOZORDER);
	else
		::SetWindowPos (hwndC, NULL, 0, 0, 
			CapStatus.uiImageWidth, CapStatus.uiImageHeight, 
			SWP_NOZORDER);
	GetParent()-> InvalidateRect (NULL, FALSE);
}

void CCaptestView::OnUpdateCapScale(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
	pCmdUI->SetCheck (CapStatus.fScale);
}


///////////////////////////////////////////////
// Driver supplied dialogs
///////////////////////////////////////////////
void CCaptestView::OnCapDlgFormat()
{    
	fDialogIsUp = TRUE;
	capDlgVideoFormat (hwndC);	// Sets the format of captured video
	ResizeNow ();
	fDialogIsUp = FALSE;
}

void CCaptestView::OnCapDlgDisplay()
{
	fDialogIsUp = TRUE; 
	capDlgVideoDisplay (hwndC);	// Controls appearance of video output
	fDialogIsUp = FALSE; 
}

void CCaptestView::OnCapDlgSource()
{
	fDialogIsUp = TRUE;
	capDlgVideoSource (hwndC);	// Selects input channel, NTSC-PAL, etc.
	fDialogIsUp = FALSE;
}

void CCaptestView::OnUpdateCapDlgDisplay(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback && 
			CapDriverCaps.fHasDlgVideoDisplay);
}

void CCaptestView::OnUpdateCapDlgFormat(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback && 
			CapDriverCaps.fHasDlgVideoFormat);
}

void CCaptestView::OnUpdateCapDlgSource(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback && 
			CapDriverCaps.fHasDlgVideoSource);
}

///////////////////////////////////////////////
// Palette creation
///////////////////////////////////////////////

void CCaptestView::OnCapAutopal5()
{
	capPaletteAuto (hwndC, 5, 256);	// Sample 5 frames of 256 colors
}

void CCaptestView::OnUpdateCapAutopal5(
CCmdUI* pCmdUI)
{
	// Gray the capture palette menu item if capture device 
	// doesn't support palettes
	
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback && 
			CapDriverCaps.fDriverSuppliesPalettes);
}


///////////////////////////////////////////////
// Capture Setup Dialog
/////////////////////////////////////////////// 
void CCaptestView::OnCapSetup()
{
	CCapParms CapParmsDlg;
	
	CapParmsDlg.m_FrameRate		= (float)1000000. 
		/ (float) CaptureParms.dwRequestMicroSecPerFrame;
	CapParmsDlg.m_EnableAudio	= CaptureParms.fCaptureAudio;
	CapParmsDlg.m_DosBuffers	= CaptureParms.fUsingDOSMemory;
	CapParmsDlg.m_VideoBuffers	= (int) CaptureParms.wNumVideoRequested;
	// fDisableWriteCache was removed from CAPTUREPARMS in avicap.h
	CapParmsDlg.m_DisableSmartDrv	= 0;// CaptureParms.fDisableWriteCache;
	CapParmsDlg.hwndCap		= hwndC;
	
	fDialogIsUp = TRUE;
	if (CapParmsDlg.DoModal() == IDOK) {
		CaptureParms.dwRequestMicroSecPerFrame = (DWORD) (1000000. / CapParmsDlg.m_FrameRate);
		CaptureParms.fCaptureAudio = CapParmsDlg.m_EnableAudio;
		CaptureParms.fUsingDOSMemory = CapParmsDlg.m_DosBuffers;
		CaptureParms.wNumVideoRequested = (UINT) CapParmsDlg.m_VideoBuffers;
		// fDisableWriteCache was removed from CAPTUREPARMS in avicap.h
 		// CaptureParms.fDisableWriteCache = CapParmsDlg.m_DisableSmartDrv;
	}
	fDialogIsUp = FALSE;
}


void CCaptestView::OnUpdateCapSetup(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
}


///////////////////////////////////////////////
// Capture!!!
///////////////////////////////////////////////
void CCaptestView::OnCapSequence()
{
	CAPINFOCHUNK cinfo;
	
	// Let's add a Copyright chunk to the capture file!
	cinfo.fccInfoID = mmioFOURCC ('I','C','O','P');
	CString cs;
	cs.LoadString(IDS_CHUNKCOPYRIGHT);
	cinfo.cbData = cs.GetLength() + 1;	// Add one for the NULL!
	cinfo.lpData = (char*)(const char*)cs;	// cast off the const
	capFileSetInfoChunk (hwndC, &cinfo);
	
	// Inform the capture window of the capture settings
	capCaptureSetSetup (hwndC, &CaptureParms, sizeof (CaptureParms));
	
	// And finally, the point of it all...
	capCaptureSequence (hwndC);
}

void CCaptestView::OnUpdateCapSequence(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
}

///////////////////////////////////////////////
// File Menu functions
///////////////////////////////////////////////
void CCaptestView::OnFileOpen()
{
	char szPath [_MAX_PATH];

	capFileGetCaptureFile (hwndC, szPath, sizeof (szPath),);
	CString csFileDefExt, csFileFilter;
	csFileDefExt.LoadString(IDS_FILEDEFEXT);
	csFileFilter.LoadString(IDS_FILEFILTER);
	CFileDialog dlgFile (TRUE /*bOpenFileDialog*/, 
				csFileDefExt /*lpszDefExt*/, 
				szPath /*lpszFileName*/, 
				OFN_HIDEREADONLY /* flags */, 
				csFileFilter, 
				NULL /* hwndParent */);

		
	dlgFile.m_ofn.lpstrFile = (LPSTR)szPath; // just fill in my own buffer

	if (dlgFile.DoModal() != IDOK)
		return;
		
	capFileSetCaptureFile (hwndC, (LPSTR)szPath);
}

/////////////////////////////////////////////
// Copy only the portion of the capture file 
// containing "real" data to another file
/////////////////////////////////////////////
void CCaptestView::OnFileSaveAs()
{
	char szPath [_MAX_PATH];
	
	CString csFileDefExt, csFileFilter;
	csFileDefExt.LoadString(IDS_FILEDEFEXT);
	csFileFilter.LoadString(IDS_FILEFILTER);
	lstrcpy (szPath, "*." + csFileDefExt);
		
	CFileDialog dlgFile (FALSE /*bOpenFileDialog*/, 
				csFileDefExt /*lpszDefExt*/, 
				szPath /*lpszFileName*/, 
				OFN_HIDEREADONLY /* flags */, 
				csFileFilter, 
				NULL /* hwndParent */);

		
	dlgFile.m_ofn.lpstrFile = (LPSTR)szPath; // just fill in my own buffer

	if (dlgFile.DoModal() != IDOK)
		return;
		
	capFileSaveAs (hwndC, (LPSTR)szPath);
}
 
///////////////////////////////////////////////
// Edit Menu functions
///////////////////////////////////////////////
void CCaptestView::OnEditPaste()
{
	capPalettePaste (hwndC);
}

void CCaptestView::OnUpdateEditPaste(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
}

void CCaptestView::OnEditCopy()
{
	capEditCopy (hwndC);
}

void CCaptestView::OnUpdateEditCopy(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (CapDriverCaps.fCaptureInitialized && !hwndPlayback);
}

///////////////////////////////////////////////
// Show and hide a playback window using MCIWnd
/////////////////////////////////////////////// 

void CCaptestView::ResizeNow(void)
{
	RECT rc;

	if (hwndPlayback) {
		// Resize to the dimensions of the playback window
		::GetWindowRect (hwndPlayback, &rc);
		OffsetRect (&rc, -rc.left, -rc.top);
	}
	else {
		// Resize our window to the size of the captured video
		capGetStatus (hwndC, &CapStatus, sizeof (CAPSTATUS)); 
		SetRect (&rc, 0, 0, CapStatus.uiImageWidth, CapStatus.uiImageHeight);

	}
	GetParent()-> CalcWindowRect (&rc);
	GetParent()-> SetWindowPos (NULL, 0, 0, rc.right - rc.left,
				rc.bottom - rc.top, 
				SWP_NOZORDER | SWP_NOMOVE);
				
	GetParent()-> InvalidateRect (NULL, FALSE);
}

void CCaptestView::OnSize(
UINT nType, 
int cx, 
int cy)
{
	CView::OnSize(nType, cx, cy);
	
	capGetStatus (hwndC, &CapStatus, sizeof (CAPSTATUS));

	if (hwndPlayback) {
		::SetWindowPos (hwndPlayback, NULL, 0, 0, cx, cy, SWP_NOZORDER);
	} 
	else {
		if (CapStatus.fScale)
			::SetWindowPos (hwndC, NULL, 0, 0, cx, cy, SWP_NOZORDER);
		else
			::SetWindowPos (hwndC, NULL, 0, 0, 
				CapStatus.uiImageWidth, CapStatus.uiImageHeight, SWP_NOZORDER);
			
		// InvalidateRect (NULL, TRUE);
	}
}


void CCaptestView::OnCapPlayback()
{
	// Note: Some boards which support playback in hardware do not support 
	// simultaneous use of the capture driver and the codec.
	// For these devices, you will need to disconnect the capture driver
	// from the window before starting playback, and then reconnect the
	// capture driver when playback completes.
	// The required lines are shown below in comments...
	
	if (hwndPlayback) {
		::ShowWindow (hwndC, SW_SHOW); 
		MCIWndDestroy(hwndPlayback);
		hwndPlayback = NULL;
		// capDriverConnect (hwndC, nIndex); /* Required if hardware playback */
	}
	else {
		char szPath[_MAX_PATH];
		
		::ShowWindow (hwndC, SW_HIDE); 
		capFileGetCaptureFile (hwndC, (LPSTR)szPath, sizeof (szPath));
		// capDriverDisconnect (hwndC); /* Required if hardware playback */
		hwndPlayback = MCIWndCreate (m_hWnd, 
			AfxGetInstanceHandle(), /* hInstance */
			WS_BORDER | WS_CHILD | WS_VISIBLE , /* extended style flags */
			szPath /* szFileName */);
	}
	ResizeNow();
}

void CCaptestView::OnUpdateCapPlayback(
CCmdUI* pCmdUI)
{
		pCmdUI->SetCheck ((BOOL)hwndPlayback);
	
}


/////////////////////////////////////////////////
// Drivers typically crash if they are displaying
// a dialog at end session.  For this reason,
// don't allow end session until the user closes
// all dialogs.
//////////////////////////////////////////////////
BOOL CCaptestView::OnQueryEndSession()
{
	if (fDialogIsUp) {
		MessageBeep (MB_ICONHAND);
		AfxMessageBox (IDS_CLOSEDIALOGS, MB_OK, 0);
		BringWindowToTop();
		return FALSE;
	}
	else
		return TRUE; 
}


///////////////////////////////////////////////
// Connect to a driver (0-9)
///////////////////////////////////////////////

void CCaptestView::OnUpdateCapDrv(
CCmdUI* pCmdUI)
{
	pCmdUI->Enable (!hwndPlayback);
	pCmdUI->SetCheck ((BOOL)((int)pCmdUI->m_nID == (nDriverIndex + ID_CAP_DRV0)));
}


void CCaptestView::ConnectToDriver(
int nIndex)
{
	if (capDriverConnect (hwndC, nIndex)) {
		nDriverIndex = nIndex;
		capDriverGetCaps (hwndC, &CapDriverCaps, sizeof (CapDriverCaps)); 
		ResizeNow ();
	}
	else {
		nDriverIndex = -1;
		_fmemset (&CapDriverCaps, 0, sizeof (CapDriverCaps));
		_fmemset (&CapStatus, 0, sizeof (CapStatus));
	}
}

void CCaptestView::OnCapDrv0()
{
	ConnectToDriver(0);
}

void CCaptestView::OnCapDrv1()
{
	ConnectToDriver(1);
}

void CCaptestView::OnCapDrv2()
{
	ConnectToDriver(2);
}

void CCaptestView::OnCapDrv3()
{
	ConnectToDriver(3);
}

void CCaptestView::OnCapDrv4()
{
	ConnectToDriver(4);
}

void CCaptestView::OnCapDrv5()
{
	ConnectToDriver(5);
}

void CCaptestView::OnCapDrv6()
{
	ConnectToDriver(6);
}

void CCaptestView::OnCapDrv7()
{
	ConnectToDriver(7);
}

void CCaptestView::OnCapDrv8()
{
	ConnectToDriver(8);
}

void CCaptestView::OnCapDrv9()
{
	ConnectToDriver(9);
}
