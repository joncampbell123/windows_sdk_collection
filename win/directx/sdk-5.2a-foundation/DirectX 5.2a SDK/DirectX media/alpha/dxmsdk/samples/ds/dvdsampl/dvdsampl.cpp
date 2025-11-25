/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1997  Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/

//
// DVDSampl.cpp: DvdGraphBuilder test/sample app
//

#include <streams.h>
#include <windows.h>
#include <IL21Dec.h>
#include <dvdevcod.h>

#include "DVDSampl.h"

#define APPNAME  TEXT("DVDSampl")


// -------------------------------------------------------------------------
//                .... Windows App Requirement starts ....
// -------------------------------------------------------------------------

//
// Forward declaration of functions
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK SelectLang(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK SelectViewLevel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK MenuProc(HWND hWnd, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK KeyProc(HWND hWnd, WPARAM wParam, LPARAM lParam) ;

CSampleDVDPlay  Player ;  // global player object

int APIENTRY WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR     lpCmdLine,
		     int       nCmdShow)
{
    MSG             msg ;
    HACCEL          hAccelTable ;

    DbgInitialise(hInstance);
    Player.SetAppValues(hInstance, APPNAME, IDS_APP_TITLE) ;

    if (! Player.InitApplication() ) 
    {
	DbgTerminate();
	return (FALSE) ;
    } 

    // Perform application initialization:
    if (! Player.InitInstance(nCmdShow) ) 
    {
	DbgTerminate();
	return (FALSE) ;
    }

    hAccelTable = LoadAccelerators(hInstance, Player.GetAppName()) ;

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
	if (! TranslateAccelerator(msg.hwnd, hAccelTable, &msg) )
	{
	    TranslateMessage(&msg) ;
	    DispatchMessage(&msg) ;
	}
    }

    DbgTerminate();
    return (msg.wParam) ;
}


// A helper function to convert render flags to corresponding menu ids
int FlagToMenuID(DWORD dwFlag)
{
    dwFlag = (dwFlag & (~AM_DVD_NOVPE)) ;  // strip off VPE flag
    switch (dwFlag)
    {
	case AM_DVD_HWDEC_PREFER:
	    return (IDM_FLAGBASE + 1) ;

	case AM_DVD_HWDEC_ONLY:
	    return (IDM_FLAGBASE + 2) ;

	case AM_DVD_SWDEC_PREFER:
	    return (IDM_FLAGBASE + 3) ;

	case AM_DVD_SWDEC_ONLY:
	    return (IDM_FLAGBASE + 4) ;

	default:  // huh!!!
	    return 0 ;
    }
}


LRESULT CALLBACK MenuProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu = GetMenu(hWnd) ;
    int   wmId    = LOWORD(wParam);
    int   wmEvent = HIWORD(wParam);

    //Parse the menu selections:
    switch (wmId) {

    case IDM_SELECT:
        Player.FileSelect() ;  // if volume has been changed, playback stopped.
        break;

    case IDM_ABOUT:
        DialogBox(Player.GetInstance(), TEXT("AboutBox"), Player.GetWindow(),
                  (DLGPROC) About);
        break;

    case IDM_EXIT:
        DestroyWindow(Player.GetWindow());
        break;

    case IDM_HWMAX:
        if (0 == (Player.GetRenderFlag() & AM_DVD_HWDEC_PREFER))
        {
            CheckMenuItem(hMenu, FlagToMenuID(Player.GetRenderFlag()),
                          MF_UNCHECKED) ;
            Player.SetRenderFlag(AM_DVD_HWDEC_PREFER) ;
            CheckMenuItem(hMenu, IDM_HWMAX, MF_CHECKED) ;
        }
        break;

    case IDM_HWONLY:
        if (0 == (Player.GetRenderFlag() & AM_DVD_HWDEC_ONLY))
        {
            CheckMenuItem(hMenu, FlagToMenuID(Player.GetRenderFlag()),
                          MF_UNCHECKED) ;
            Player.SetRenderFlag(AM_DVD_HWDEC_ONLY) ;
            CheckMenuItem(hMenu, IDM_HWONLY, MF_CHECKED) ;
        }
        break;

    case IDM_SWMAX:
        if (0 == (Player.GetRenderFlag() & AM_DVD_SWDEC_PREFER))
        {
            CheckMenuItem(hMenu, FlagToMenuID(Player.GetRenderFlag()),
                          MF_UNCHECKED) ;
            Player.SetRenderFlag(AM_DVD_SWDEC_PREFER) ;
            CheckMenuItem(hMenu, IDM_SWMAX, MF_CHECKED) ;
        }
        break;

    case IDM_SWONLY:
        if (0 == (Player.GetRenderFlag() & AM_DVD_SWDEC_ONLY))
        {
            CheckMenuItem(hMenu, FlagToMenuID(Player.GetRenderFlag()),
                          MF_UNCHECKED) ;
            Player.SetRenderFlag(AM_DVD_SWDEC_ONLY) ;
            CheckMenuItem(hMenu, IDM_SWONLY, MF_CHECKED) ;
        }
        break;

    case IDM_NOVPE:
        Player.SetRenderFlag(AM_DVD_NOVPE) ;
        CheckMenuItem(hMenu, IDM_NOVPE, 
             (Player.GetRenderFlag() & AM_DVD_NOVPE) ? MF_CHECKED : MF_UNCHECKED) ;
        break;

    case IDM_BUILDGRAPH:
        Player.BuildGraph() ;
        break ;

    case IDM_PLAY:
        Player.Play() ;
        break;

    case IDM_STOP:
        Player.Stop() ;
        break;

    case IDM_PAUSE:
        Player.Pause() ;
        break;

    case IDM_SLOWPLAY:
        Player.SlowPlay() ;
        break;

    case IDM_FASTFWD:
        Player.FastForward() ;
        break;

    case IDM_VERYFASTFWD:
        Player.VeryFastForward() ;
        break;

    case IDM_FASTRWND:
        Player.FastRewind() ;
        break;

    case IDM_VERYFASTRWND:
        Player.VeryFastRewind() ;
        break;

    case IDM_MENU:
        Player.ShowMenu() ;
        break;

    case IDM_USER_MENUSTATUS:
        //
        // This is a custom user message that informs the app to update the
        // menu according to the state.
        //
        ModifyMenu(hMenu, IDM_MENU, MF_STRING, IDM_MENU,
                   lParam ? TEXT("Resume") : TEXT("Menu")) ;
        break ;

    case IDM_LANG:
        if (! Player.IsLangKnown() )
        {
            MessageBox(Player.GetWindow(), 
                    TEXT("Can't specify audio/subpicture language until playback starts."), 
                    TEXT("Error"), MB_OK | MB_ICONINFORMATION) ;
            break ;
        }
        DialogBox(Player.GetInstance(), MAKEINTRESOURCE(IDD_LANGUAGE), Player.GetWindow(),
                  (DLGPROC) SelectLang);
        break;

    case IDM_VIEWLEVEL:
        if (Playing == Player.GetState())
        {
            MessageBox(Player.GetWindow(), 
                    TEXT("Can't change parental control level during playback. Please stop first."), 
                    TEXT("Error"), MB_OK | MB_ICONINFORMATION) ;
            break ;
        }
        DialogBox(Player.GetInstance(), MAKEINTRESOURCE(IDD_VIEWLEVEL), Player.GetWindow(),
                (DLGPROC) SelectViewLevel);
        break;

    case IDM_CC:
        if (Player.ClosedCaption())  // CC turned on
            CheckMenuItem(hMenu, IDM_CC, MF_CHECKED) ;
        else  // CC turned off
            CheckMenuItem(hMenu, IDM_CC, MF_UNCHECKED) ;
        break ;
		
    case IDM_FULLSCRN:
        if (Player.ShowFullScreen())  // if really in fullscreen mode
            CheckMenuItem(hMenu, IDM_FULLSCRN, MF_CHECKED) ;
        else  // full screen turned off
            CheckMenuItem(hMenu, IDM_FULLSCRN, MF_UNCHECKED) ;
        break;

    default:
        break ;
    }

    return 0 ;
}


LRESULT CALLBACK KeyProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
	case VK_ESCAPE:
	    {
	    MessageBeep(MB_OK) ;
	    HMENU hMenu = GetMenu(hWnd) ;
	    Player.StopFullScreen() ;
	    CheckMenuItem(hMenu, IDM_FULLSCRN, MF_UNCHECKED) ;  // no fullscrn anymore
	    }
	    break ;

	case VK_UP:
	    Player.CursorMove(Cursor_Up) ;
	    break ;

	case VK_DOWN:
	    Player.CursorMove(Cursor_Down) ;
	    break ;

	case VK_LEFT:
	    Player.CursorMove(Cursor_Left) ;
	    break ;

	case VK_RIGHT:
	    Player.CursorMove(Cursor_Right) ;
	    break ;

	case VK_RETURN:
	    Player.CursorSelect() ;
	    break ;

	default:
	    break ;
    }

    return 0 ;
}


//
// One function to update the menu state based on the player object state.
// Used by WM_INITMENUPOPUP message handling.
//
void UpdateMenuState(void)
{
    HMENU  hMenu = GetMenu(Player.GetWindow()) ;

    switch (Player.GetState())
    {
    case Unknown:
        EnableMenuItem(hMenu, IDM_BUILDGRAPH, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_PAUSE, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_PLAY, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_STOP, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_SLOWPLAY, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_FASTFWD, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_VERYFASTFWD, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_FASTRWND, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_VERYFASTRWND, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_MENU, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_LANG, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_VIEWLEVEL, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_CC, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_FULLSCRN, MF_GRAYED) ;
        break ;

    case Stopped:
        EnableMenuItem(hMenu, IDM_BUILDGRAPH, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_STOP, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_PAUSE, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_PLAY, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_SLOWPLAY, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTRWND, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTRWND, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_MENU, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_FULLSCRN, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_LANG, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_VIEWLEVEL, MF_ENABLED) ;
	    // CC option has been turned on/off above based on Line21Dec presence
        break ;

    case Paused:
        EnableMenuItem(hMenu, IDM_BUILDGRAPH, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_PAUSE, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_STOP, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_PLAY, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_SLOWPLAY, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTRWND, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTRWND, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_MENU, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_FULLSCRN, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_LANG, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_VIEWLEVEL, MF_ENABLED) ;
        break ;

    case Playing:
        EnableMenuItem(hMenu, IDM_BUILDGRAPH, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_PLAY, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_PAUSE, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_STOP, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_SLOWPLAY, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTFWD, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_FASTRWND, MF_ENABLED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTRWND, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_MENU, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_FULLSCRN, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_LANG, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_VIEWLEVEL, MF_ENABLED) ;
        break ;

    case Scanning:
        EnableMenuItem(hMenu, IDM_BUILDGRAPH, MF_GRAYED) ;
	    EnableMenuItem(hMenu, IDM_SLOWPLAY, MF_GRAYED) ;
	    EnableMenuItem(hMenu, IDM_FASTFWD, MF_GRAYED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTFWD, MF_GRAYED) ;
	    EnableMenuItem(hMenu, IDM_FASTRWND, MF_GRAYED) ;
	    EnableMenuItem(hMenu, IDM_VERYFASTRWND, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_MENU, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_FULLSCRN, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_LANG, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_VIEWLEVEL, MF_GRAYED) ;
        EnableMenuItem(hMenu, IDM_STOP, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_PAUSE, MF_ENABLED) ;
        EnableMenuItem(hMenu, IDM_PLAY, MF_ENABLED) ;
        break ;

    default:  // Huh!!!
        break ;
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC  hDC ;
    static PAINTSTRUCT ps ;

    switch (message) {

    case WM_INITMENUPOPUP:
        UpdateMenuState() ;
        break ;

	case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps) ;
        ASSERT(hDC) ;
        Player.DrawStatus(hDC) ;
        EndPaint(hWnd, &ps) ;
        break ;

	case WM_DVDPLAY_EVENT:
        Player.OnDVDPlayEvent(wParam, lParam) ;
        break ;

	case WM_KEYUP:
        KeyProc(hWnd, wParam, lParam) ;
        break ;

	case WM_COMMAND:
        MenuProc(hWnd, wParam, lParam) ;
        break;

	case WM_DESTROY:
        PostQuitMessage(0);
        break;

	default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

    return 0 ;
}


LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
	case WM_INITDIALOG:
        return TRUE;

	case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, TRUE);
            return TRUE;
	    }
        break;

	default:
        break ;
    }

    return FALSE;
}


LRESULT CALLBACK SelectLang(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
	case WM_INITDIALOG:
    {
        Player.MakeAudioStreamList(hDlg, IDD_LANG_AUDIOLANG) ;
        Player.MakeSPStreamList(hDlg, IDD_LANG_SPLANG) ;
        BOOL bSPOn = IsDlgButtonChecked(hDlg, IDD_LANG_SHOWSUBPIC) ;
        EnableWindow(GetDlgItem(hDlg, IDD_LANG_SPLANGTEXT), bSPOn) ;
        EnableWindow(GetDlgItem(hDlg, IDD_LANG_SPLANG), bSPOn) ;
    }
    return TRUE;

	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDD_LANG_SHOWSUBPIC:
        {
            BOOL bSPOn = IsDlgButtonChecked(hDlg, IDD_LANG_SHOWSUBPIC) ;
            EnableWindow(GetDlgItem(hDlg, IDD_LANG_SPLANGTEXT), bSPOn) ;
            EnableWindow(GetDlgItem(hDlg, IDD_LANG_SPLANG), bSPOn) ;
        }
        break ;

        case IDOK:
        {
            LONG   lStream ;
            BOOL   bRes ;

            // Set the SP choice value & option specified by the user
            BOOL bSPOn = IsDlgButtonChecked(hDlg, IDD_LANG_SHOWSUBPIC) ;
            if (bSPOn)
            {
                lStream = SendDlgItemMessage(hDlg, IDD_LANG_SPLANG, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0) ;
                if (CB_ERR == lStream)
                    DbgLog((LOG_ERROR, 1, 
                        TEXT("WARNING: Couldn't get selected SP stream id (Error %d)"), lStream)) ;
                else
                {
                    bRes = Player.SetSPState(lStream, bSPOn) ; // TRUE
                    ASSERT(bRes) ;
                }
            }
            else
            {
                bRes = Player.SetSPState(SPSTREAM_NOCHANGE, bSPOn) ; // FALSE
                ASSERT(bRes) ;  // a benign assert !!!
            }

            // Set the audio choice value specified by the user
            lStream = SendDlgItemMessage(hDlg, IDD_LANG_AUDIOLANG, CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0) ;
            if (CB_ERR == lStream)
                DbgLog((LOG_ERROR, 1, TEXT("WARNING: Couldn't get selected audio stream id (Error %d)"), lStream)) ;
            else
            {
                bRes = Player.SetAudioState(lStream) ;
                ASSERT(bRes) ;  // a benign assert !!!
            }
        }

        // Now fall through to just end the dialog

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;

    default:
        break ;
    }

    return FALSE;
}


LRESULT CALLBACK SelectViewLevel(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        Player.MakeParentalLevelList(hDlg, IDD_VIEW_LEVELLIST) ;
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            LONG  lLevel ;
            lLevel = SendDlgItemMessage(hDlg, IDD_VIEW_LEVELLIST, LB_GETCURSEL, (WPARAM) 0, (LPARAM) 0) ;
            if (CB_ERR == lLevel)
                DbgLog((LOG_ERROR, 1, TEXT("WARNING: Couldn't get selected parental control level (Error %d)"), lLevel)) ;
            else
                Player.SetParentalLevel(lLevel) ;
        }

        // Now fall through to just end the dialog

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;

    default:
        break ;
    }

    return FALSE;
}



// -------------------------------------------------------------------------
//                .... Windows App Requirement ends ....
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
//           ....  CSampleDVDPlay class implementation  ....
// -------------------------------------------------------------------------

CSampleDVDPlay::CSampleDVDPlay(void)
{
    CoInitialize(NULL) ;

    // The app stuff
    m_dwRenderFlag = 0 ;
    m_bMenuOn = FALSE ;
    m_bCCOn = FALSE ;
    m_bFullScrnOn = FALSE ;
    m_ulSPStream = 0 ;
    m_bSPOn = TRUE ;
    m_ulAudioStream = 0 ;
    m_bStillOn = FALSE ;
    m_bLangKnown = FALSE ;
    m_eState = Unknown ;
    m_ulParentCtrlLevel = 3 ;  // 3 is the index to level 6 which is for "R"

    m_pDvdGB = NULL ;
    m_pGraph = NULL ;
    m_pDvdC = NULL ;
    m_pDvdI = NULL ;
    m_pVW = NULL ;
    m_pMC = NULL ;
    m_pME = NULL ;

    ZeroMemory(m_achFileName, sizeof(m_achFileName)) ;
    ZeroMemory(m_achStillText, sizeof(m_achStillText)) ;
    ZeroMemory(m_achTitleText, sizeof(m_achTitleText)) ;
    ZeroMemory(m_achChapterText, sizeof(m_achChapterText)) ;
    ZeroMemory(m_achTimeText, sizeof(m_achTimeText)) ;
}


CSampleDVDPlay::~CSampleDVDPlay(void)
{
    // Release the DS interfaces we have got
    ReleaseInterfaces() ;

    if (m_pDvdGB)
        m_pDvdGB->Release() ;

    CoUninitialize() ;
    DbgLog((LOG_TRACE, 0, TEXT("CSampleDVDPlay d-tor exiting..."))) ;
}


void CSampleDVDPlay::ReleaseInterfaces(void)
{
    if (m_pDvdC) {
        m_pDvdC->Release() ;
        m_pDvdC = NULL ;
    }
    if (m_pDvdI) {
        m_pDvdI->Release() ;
        m_pDvdI = NULL ;
    }
    if (m_pVW) {
        m_pVW->Release() ;
        m_pVW = NULL ;
    }
    if (m_pMC) {
        m_pMC->Release() ;
        m_pMC = NULL ;
    }
    if (m_pME) {
        // clear any already set notification arrangement
        m_pME->SetNotifyWindow(NULL, WM_DVDPLAY_EVENT, (ULONG)(LPVOID)m_pME) ;
        m_pME->Release() ;
        m_pME = NULL ;
    }
    if (m_pGraph) {
        m_pGraph->Release() ;
        m_pGraph = NULL ;
    }
}


void CSampleDVDPlay::SetAppValues(HINSTANCE hInst, LPTSTR szAppName, 
				  int iAppTitleResId)
{
    // The Windows stuff
    m_hInstance = hInst ;
    lstrcpy(m_szAppName, APPNAME) ;
    LoadString(m_hInstance, IDS_APP_TITLE, m_szTitle, 100) ;
}


BOOL CSampleDVDPlay::InitApplication(void)
{
    WNDCLASSEX  wc ;

    // Win32 will always set hPrevInstance to NULL, so lets check
    // things a little closer. This is because we only want a single
    // version of this app to run at a time
    m_hWnd = FindWindow (m_szAppName, m_szTitle) ;
    if (m_hWnd) {
        // We found another version of ourself. Lets defer to it:
        if (IsIconic(m_hWnd)) {
            ShowWindow(m_hWnd, SW_RESTORE);
        }
        SetForegroundWindow(m_hWnd);

        // If this app actually had any functionality, we would
        // also want to communicate any action that our 'twin'
        // should now perform based on how the user tried to
        // execute us.
        return FALSE;
    }

    // Register the app main window class
    wc.cbSize        = sizeof(wc) ;
    wc.style         = CS_HREDRAW | CS_VREDRAW ;
    wc.lpfnWndProc   = (WNDPROC) WndProc ;
    wc.cbClsExtra    = 0 ;
    wc.cbWndExtra    = 0 ;
    wc.hInstance     = m_hInstance ;
    wc.hIcon         = LoadIcon(m_hInstance, m_szAppName) ;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1) ;
    wc.lpszMenuName  = m_szAppName ;
    wc.lpszClassName = m_szAppName ;
    wc.hIconSm       = NULL ;
    if (0 == RegisterClassEx(&wc))
    {
        DbgLog((LOG_ERROR, 0, 
	        TEXT("ERROR: RegisterClassEx() for app class failed (Error %ld)"), 
	        GetLastError())) ;
        return FALSE ;
    }

    return TRUE ;
}

BOOL CSampleDVDPlay::InitInstance(int nCmdShow)
{
    m_hWnd = CreateWindowEx(0, m_szAppName, m_szTitle, WS_OVERLAPPEDWINDOW,
                        200, 400, 400, 200,
                        NULL, NULL, m_hInstance, NULL);
    if (!m_hWnd) {
        return FALSE ;
    }

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd) ;

    //
    // The DirectShow stuff:
    //     Now instantiate the DVD Graph Builder object and start working
    //
    HRESULT hr = CoCreateInstance(CLSID_DvdGraphBuilder, NULL, CLSCTX_INPROC, 
			IID_IDvdGraphBuilder, (LPVOID *)&m_pDvdGB) ;
    if (FAILED(hr) || NULL == m_pDvdGB)
    {
        MessageBox(m_hWnd, 
                TEXT("DirectShow DVD software not installed properly.\nPress OK to end the app."), 
                TEXT("Error"), MB_OK | MB_ICONSTOP) ;
        return FALSE ;
    }

    //
    // By default we use HW decoding as preferred mode. Set menu option.
    // Also we don't turn on CC by default.
    m_dwRenderFlag = AM_DVD_HWDEC_PREFER ;
    CheckMenuItem(GetMenu(m_hWnd), IDM_HWMAX, MF_CHECKED) ;
    CheckMenuItem(GetMenu(m_hWnd), IDM_CC, MF_UNCHECKED) ;
    CheckMenuItem(GetMenu(m_hWnd), IDM_FULLSCRN, MF_UNCHECKED) ;

    return TRUE;
}


void CSampleDVDPlay::BuildGraph(void)
{
    // First release any existing interface pointer(s)
    ReleaseInterfaces() ;

    // Check if a DVD-Video volume name has been specified; if so, use that
    WCHAR    achwFileName[MAX_PATH] ;
    LPCWSTR  lpszwFileName = NULL ;  // by default
    if (lstrlen(m_achFileName) > 0)  // if something was specified before
    {
#ifdef UNICODE
        lstrcpy(achwFileName, m_achFileName) ;
#else
        MultiByteToWideChar(CP_ACP, 0, m_achFileName, -1, achwFileName, MAX_PATH) ;
#endif // UNICODE

        lpszwFileName = achwFileName ;
    }

    // Build the graph
    AM_DVD_RENDERSTATUS   Status ;
    HRESULT hr = m_pDvdGB->RenderDvdVideoVolume(lpszwFileName,
                                    m_dwRenderFlag, &Status) ;
    if (FAILED(hr))
    {
        AMGetErrorText(hr, m_achBuffer, sizeof(m_achBuffer)) ;
        MessageBox(m_hWnd, m_achBuffer, m_szAppName, MB_OK) ;
        return ;
    }
    if (S_FALSE == hr)  // if partial success
    {
        TCHAR    achStatusText[1000] ;
        if (0 == GetStatusText(&Status, achStatusText, sizeof(achStatusText)))
        {
            lstrcpy(achStatusText, TEXT("An unknown error has occurred")) ;
        }
        lstrcat(achStatusText, TEXT("\n\nDo you still want to continue?")) ;
        if (IDNO == MessageBox(m_hWnd, achStatusText, TEXT("Warning"), MB_YESNO))
        {
            return ;
        }
    }

    m_eState = Stopped ;  // graph has been built, however bad it may be

    // Now get all the interfaces to playback the DVD-Video volume
    hr = m_pDvdGB->GetFiltergraph(&m_pGraph) ;
    ASSERT(SUCCEEDED(hr) && m_pGraph) ;

    hr = m_pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMC) ;
    ASSERT(SUCCEEDED(hr) && m_pMC) ;

    hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_pME) ;
    ASSERT(SUCCEEDED(hr) && m_pME) ;

    //
    // Also set up the event notification so that the main window gets
    // informed about all that we care about during playback.
    //
    hr = m_pME->SetNotifyWindow((OAHWND) m_hWnd, WM_DVDPLAY_EVENT, 
				(ULONG)(LPVOID)m_pME) ;
    ASSERT(SUCCEEDED(hr)) ;

    hr = m_pDvdGB->GetDvdInterface(IID_IDvdControl, (LPVOID *)&m_pDvdC) ;
    ASSERT(SUCCEEDED(hr) && m_pDvdC) ;

    hr = m_pDvdGB->GetDvdInterface(IID_IDvdInfo, (LPVOID *)&m_pDvdI) ;
    ASSERT(SUCCEEDED(hr) && m_pDvdI) ;

    IAMLine21Decoder  *pL21Dec ;
    hr = m_pDvdGB->GetDvdInterface(IID_IAMLine21Decoder, (LPVOID *)&pL21Dec) ;
    if (pL21Dec)
    {
	    EnableMenuItem(GetMenu(m_hWnd), IDM_CC, MF_ENABLED) ;
	    pL21Dec->SetServiceState(m_bCCOn ? AM_L21_CCSTATE_On : AM_L21_CCSTATE_Off) ;
	    pL21Dec->Release() ;
    }
    else
    {
        EnableMenuItem(GetMenu(m_hWnd), IDM_CC, MF_GRAYED) ;
        DbgLog((LOG_TRACE, 2, TEXT("Line21 Decoder interface not found. Can't CC"))) ;
    }

    // Now change the title of the playback window
    hr = m_pDvdGB->GetDvdInterface(IID_IVideoWindow, (LPVOID *)&m_pVW) ;
    if (SUCCEEDED(hr) && m_pVW)
    {
        TCHAR   achTitle[40] ;
#ifdef UNICODE
        LoadString(m_hInstance, IDS_WINDOW_TITLE, achwTitle, sizeof(achwTitle)) ;
#else
        WCHAR   achwTitle[40] ;
        LoadString(m_hInstance, IDS_WINDOW_TITLE, achTitle, sizeof(achTitle)) ;
        MultiByteToWideChar(CP_ACP, 0, achTitle, -1, achwTitle, sizeof(achwTitle)) ;
#endif // UNICODE
        m_pVW->put_Caption((BSTR)achwTitle) ;
    }
    else
        DbgLog((LOG_ERROR, 1, TEXT("IVideoWindow interface not found. No window title change."))) ;

    return ;
}


BOOL CSampleDVDPlay::Play(void)
{
    HRESULT  hr ;

    //
    // If somehow the still image is On, ask Nav to get just out of that mode
    //
    if (m_eState != Playing  &&  m_bStillOn  &&  m_pDvdC)
    {
        if (SUCCEEDED(hr = m_pDvdC->StillOff()))
       	{
            m_bStillOn = FALSE ;
            m_eState = Playing ;
            return TRUE ;    // we are done!!!
        }
        else
            DbgLog((LOG_TRACE, 3, TEXT("IDvdControl::StillOff() failed (Error 0x%lx)"), hr)) ;
    }

    //
    // If we are scanning now, just set the scanning speed to normal, i.e, play
    //
    PLAYER_STATE  State = GetState() ;
    if (Scanning == State)
    {
        hr = m_pDvdC->ForwardScan(1.0) ;
        ASSERT(SUCCEEDED(hr)) ;
    }
    //
    // Parental level needs to be set every time we run, because the Nav flushes its
    // settings everytime the graph is "stopped". So we better apply it on transition
    // to "play".
    // Use the actual value of viewing level rather than list box index
    //
    else if (Stopped == State)  // if stopped, set parental level again
    {
        hr = m_pDvdC->ParentalLevelSelect(m_ViewLevels.GetValue(m_ulParentCtrlLevel)) ;
        ASSERT(SUCCEEDED(hr)) ;
    }

    //
    // We need to do some work to play...
    //
    if (NULL == m_pMC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }
    hr = m_pMC->Run() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Run() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    // Some state changes now
    m_eState = Playing ;
    m_bStillOn = FALSE ;  // should be, right?

    return TRUE ;  // success
}

BOOL CSampleDVDPlay::Stop(void)
{
    if (NULL == m_pMC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }
    HRESULT hr = m_pMC->Stop() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Stop() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Stopped ;

    return TRUE ;  // success
}

BOOL CSampleDVDPlay::Pause(void)
{
    if (NULL == m_pMC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }
    HRESULT hr = m_pMC->Pause() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Pause() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Paused ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::FastForward(void)
{
    if (NULL == m_pMC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    HRESULT  hr ;
    hr = m_pMC->Run() ;  // just to make sure that we are running
    ASSERT(SUCCEEDED(hr)) ;

    if (NULL == m_pDvdC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }
    hr = m_pDvdC->ForwardScan(2.0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdControl::ForwardScan(2.0) failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Scanning ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::VeryFastForward(void)
{
    if (NULL == m_pDvdC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    HRESULT  hr ;
    hr = m_pMC->Run() ;  // just to make sure that we are running
    ASSERT(SUCCEEDED(hr)) ;

    hr = m_pDvdC->ForwardScan(8.0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdControl::ForwardScan(8.0) failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Scanning ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::FastRewind(void)
{
    if (NULL == m_pDvdC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    HRESULT  hr ;
    hr = m_pMC->Run() ;  // just to make sure that we are running
    ASSERT(SUCCEEDED(hr)) ;

    hr = m_pDvdC->BackwardScan(2.0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdControl::BackwardScan(2.0) failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Scanning ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::VeryFastRewind(void)
{
    if (NULL == m_pDvdC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    HRESULT  hr ;
    hr = m_pMC->Run() ;  // just to make sure that we are running
    ASSERT(SUCCEEDED(hr)) ;

    hr = m_pDvdC->BackwardScan(8.0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdControl::BackwardScan(8.0) failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Scanning ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::SlowPlay(void)
{
    if (NULL == m_pDvdC)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    HRESULT  hr ;
    hr = m_pMC->Run() ;  // just to make sure that we are running
    ASSERT(SUCCEEDED(hr)) ;

    hr = m_pDvdC->ForwardScan(0.5) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdControl::ForwardScan(0.5) failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }

    m_eState = Scanning ;

    return TRUE ;  // success
}


BOOL CSampleDVDPlay::SetSPState(LONG lSPStream, BOOL bState)
{
    HRESULT hr = m_pDvdC->SubpictureStreamChange(lSPStream, bState) ;
    if (SUCCEEDED(hr))
    {
        m_bSPOn = bState ;
        m_ulSPStream = lSPStream ;  // store new SP stream id
        return TRUE ;  // success
    }

    DbgLog((LOG_ERROR, 0, 
            TEXT("WARNING: IDvdControl::SubpictureStreamChange(%ld, %s) failed (Error 0x%lx)"), 
            lSPStream, bState ? TEXT("TRUE") : TEXT("FALSE"), hr)) ;
    return FALSE ;  // failure
}


//
// On Windows 95 (and OSR2), the locale information is not retrieved properly.  To get
// the audio and subpicture language names properly, we have to do a bit of work around.
// This should be fixed in Windows 98 making the extra work unnecessary.
//
int CSampleDVDPlay::MakeSPStreamList(HWND hDlg, int iListID)
{
    if (NULL == m_pDvdI)
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdInfo interface not available"))) ;
        return FALSE ;
    }

    ULONG  ulStream ;
    LCID   lcid ;
    TCHAR  achLang[MAX_PATH] ;
    int    iRes ;
    ULONG  ulNumLang ;
    BOOL   bSPOn ;

    // First clear the list box of all SP stream names
    SendDlgItemMessage(hDlg, iListID, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0) ;

    // First find out how many SP streams are there and what's the current lang.
    // This is our chance to find out if someone changed the SP lang through 
    // DVD menu so that we can synch up our SP stream value now.
    HRESULT hr = m_pDvdI->GetCurrentSubpicture(&ulNumLang, &m_ulSPStream, &bSPOn) ;
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, TEXT("Not ready to find language information"), TEXT("Warning"), MB_OK) ;
        return 0 ;
    }

    // Now add all other available SP streams
    for (ulStream = 0 ; ulStream < ulNumLang ; ulStream++)
    {
        if (SUCCEEDED(m_pDvdI->GetSubpictureLanguage(ulStream, &lcid))  &&
            0 != lcid)
        {
            iRes = GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, achLang, MAX_PATH) ;
            if (0 == iRes)
            {
                if (! GetSPLanguage(ulStream, bSPOn, achLang, MAX_PATH) )  // Pre-Win98
                    lstrcpy(achLang, TEXT("Unknown")) ;
            }
        }
        else  // SP language enum failed
        {
            if (! GetSPLanguage(ulStream, bSPOn, achLang, MAX_PATH) )  // Pre-Win98
                lstrcpy(achLang, TEXT("Unknown")) ;
        }

        // Add to the listbox now
        iRes = SendDlgItemMessage(hDlg, iListID, CB_ADDSTRING, (WPARAM) 0, (LPARAM)(LPVOID) achLang) ;
        if (CB_ERR == iRes || CB_ERRSPACE == iRes)
        {
            DbgLog((LOG_ERROR, 1, TEXT("Error (%d) adding SP language '%s' to list"), iRes, achLang)) ;
        }
    }

    //
    // GetSPLanguage() method might have changed the currently set SP stream and/or state.
    // Let's set it back to what it was before.
    //
    hr = m_pDvdC->SubpictureStreamChange(m_ulSPStream, m_bSPOn) ;
    ASSERT(SUCCEEDED(hr)) ;  // a benign assert !!!

    if (ulStream > 0)
    {
        iRes = SendDlgItemMessage(hDlg, IDD_LANG_SPLANG, CB_SETCURSEL, 
                        (WPARAM) m_ulSPStream, (LPARAM) 0) ;
        if (CB_ERR == iRes)
            DbgLog((LOG_ERROR, 1, 
                TEXT("WARNING: Couldn't set %ld as selected SP stream id (Error %d)"),
                m_ulSPStream, iRes)) ;
    }
    CheckDlgButton(hDlg, IDD_LANG_SHOWSUBPIC, m_bSPOn) ;

    return ulStream ;
}


void CSampleDVDPlay::GetSPLangCode(DVD_SubpictureATR *pSPATR, LPTSTR lpszCode)
{
    lpszCode[0] = (((LANGINFO *)pSPATR)->wLang & 0xFF) | 0x20 ;        // get byte & lower case
    lpszCode[1] = ((((LANGINFO *)pSPATR)->wLang >> 8) & 0xFF) | 0x20 ; // get byte & lower case
    lpszCode[2] = 0 ;
}


BOOL CSampleDVDPlay::GetSPLanguage(ULONG ulStream, BOOL bSPOn, LPTSTR lpszLang, int iMaxLang)
{
    HRESULT hr = m_pDvdC->SubpictureStreamChange(ulStream, bSPOn) ;
    if (FAILED(hr))
        return FALSE ;  // stream disabled?

    DVD_SubpictureATR SPATR ;
    hr = m_pDvdI->GetCurrentSubpictureAttributes(&SPATR) ;
    ASSERT(SUCCEEDED(hr)) ;
    TCHAR  achCode[3] ;
    GetSPLangCode(&SPATR, achCode) ;
    BOOL bRes = m_Langs.GetLangString(achCode, lpszLang) ;
    ASSERT((bRes && 0 != lstrcmp(TEXT("Unknown"), lpszLang)) || 
           (0 == lstrcmp(TEXT("Unknown"), lpszLang))) ;
    return TRUE ;  // success
}


BOOL CSampleDVDPlay::SetAudioState(LONG lAudioStream)
{
    HRESULT hr = m_pDvdC->AudioStreamChange(lAudioStream) ;
    if (SUCCEEDED(hr))
    {
        m_ulAudioStream = lAudioStream ;  // store new audio stream id
        return TRUE ;  // success
    }

    DbgLog((LOG_ERROR, 0, 
        TEXT("WARNING: IDvdControl::AudioStreamChange(%ld) failed (Error 0x%lx)"), 
        lAudioStream, hr)) ;
    return FALSE ;  // failure
}


int CSampleDVDPlay::MakeAudioStreamList(HWND hDlg, int iListID)
{
    if (NULL == m_pDvdI)
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdInfo interface not available"))) ;
        return FALSE ;
    }

    ULONG  ulStream ;
    LCID   lcid ;
    TCHAR  achLang[MAX_PATH] ;
    int    iRes ;
    ULONG  ulNumLang ;

    // First clear the list box of all SP stream names
    SendDlgItemMessage(hDlg, iListID, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0) ;

    // First find out how many audio langs are there and what's the current lang.
    // This is our chance to find out if someone changed the audio lang through 
    // DVD menu so that we can synch up our audio lang value now.
    HRESULT hr = m_pDvdI->GetCurrentAudio(&ulNumLang, &m_ulAudioStream) ;
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, TEXT("Not ready to find language information"), TEXT("Warning"), MB_OK) ;
        return 0 ;
    }

    // Now add all available audio streams
    for (ulStream = 0 ; ulStream < ulNumLang ; ulStream++)
    {
        if (SUCCEEDED(m_pDvdI->GetAudioLanguage(ulStream, &lcid))  &&
            0 != lcid)
        {
            iRes = GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, achLang, MAX_PATH) ;
            if (0 == iRes)
            {
                if (! GetAudioLanguage(ulStream, achLang, MAX_PATH) ) // Pre-Win98
                    lstrcpy(achLang, TEXT("Unknown")) ;
            }
        }
        else
        {
            if (! GetAudioLanguage(ulStream, achLang, MAX_PATH) ) // Pre-Win98
                lstrcpy(achLang, TEXT("Unknown")) ;
        }

        // Add to the listbox now
        iRes = SendDlgItemMessage(hDlg, iListID, CB_ADDSTRING, (WPARAM) 0, (LPARAM)(LPVOID) achLang) ;
        if (CB_ERR == iRes || CB_ERRSPACE == iRes)
        {
            DbgLog((LOG_ERROR, 1, TEXT("Error (%d) adding audio language '%s' to list"), iRes, achLang)) ;
        }
    }

    //
    // GetAudioLanguage() method might have changed the currently set audio stream.
    // Let's set it back to what it was before.
    //
    hr = m_pDvdC->AudioStreamChange(m_ulAudioStream) ;
    ASSERT(SUCCEEDED(hr)) ;  // a benign assert !!!

    if (ulStream > 0)
    {
        iRes = SendDlgItemMessage(hDlg, iListID, CB_SETCURSEL, 
                    (WPARAM) m_ulAudioStream, (LPARAM) 0) ;
        if (CB_ERR == iRes)
            DbgLog((LOG_ERROR, 1, 
                TEXT("WARNING: Couldn't set %ld as selected audio stream id (Error %d)"),
                m_ulAudioStream, iRes)) ;
    }

    return ulStream ;
}


void CSampleDVDPlay::GetAudioLangCode(DVD_AudioATR *pAudATR, LPTSTR lpszCode)
{
    lpszCode[0] = (((LANGINFO *)pAudATR)->wLang & 0xFF) | 0x20 ;        // get byte & lower case
    lpszCode[1] = ((((LANGINFO *)pAudATR)->wLang >> 8) & 0xFF) | 0x20 ; // get byte & lower case
    lpszCode[2] = 0 ;
}


BOOL CSampleDVDPlay::GetAudioLanguage(ULONG ulStream, LPTSTR lpszLang, int iMaxLang)
{
    HRESULT hr = m_pDvdC->AudioStreamChange(ulStream) ;
    if (FAILED(hr))
        return FALSE ;  // stream disabled

    DVD_AudioATR AudATR ;
    hr = m_pDvdI->GetCurrentAudioAttributes(&AudATR) ;
    ASSERT(SUCCEEDED(hr)) ;
    TCHAR  achCode[3] ;
    GetAudioLangCode(&AudATR, achCode) ;
    BOOL bRes = m_Langs.GetLangString(achCode, lpszLang) ;
    ASSERT((bRes && 0 != lstrcmp(TEXT("Unknown"), lpszLang)) || 
           (0 == lstrcmp(TEXT("Unknown"), lpszLang))) ;
    return TRUE ;  // success
}


int CSampleDVDPlay::MakeParentalLevelList(HWND hDlg, int iListID)
{
    if (NULL == m_pDvdI)
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IDvdInfo interface not available"))) ;
        return FALSE ;
    }

    int      iLevels = m_ViewLevels.GetCount() ;
    int      iRes ;

    // First clear the list box of all SP stream names
    SendDlgItemMessage(hDlg, iListID, LB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0) ;

    // Now add all other available SP streams
    for (int i = 0 ; i < iLevels ; i++)
    {
        // Add to the listbox now
        iRes = SendDlgItemMessage(hDlg, iListID, LB_ADDSTRING, (WPARAM) 0, 
                                  (LPARAM)(LPVOID) m_ViewLevels.GetName(i)) ;
        if (LB_ERR == iRes || LB_ERRSPACE == iRes)
        {
            DbgLog((LOG_ERROR, 1, 
                TEXT("Error (%d) adding parental level '%s'(%d) to list"), 
                iRes, m_ViewLevels.GetName(i), i)) ;
        }
    }

    if (iLevels > 0)
    {
        iRes = SendDlgItemMessage(hDlg, iListID, LB_SETCURSEL, 
                        (WPARAM) m_ulParentCtrlLevel, (LPARAM) 0) ;
	    if (LB_ERR == iRes)
	    {
            DbgLog((LOG_ERROR, 1, 
                TEXT("WARNING: Couldn't set %ld as selected parent level (Error %d)"),
                m_ulParentCtrlLevel, iRes)) ;
	    }
    }
    return iLevels ;
}


ULONG CSampleDVDPlay::SetParentalLevel(LONG lLevel)
{
	m_ulParentCtrlLevel = lLevel ;
    return m_ulParentCtrlLevel ;
}


BOOL CSampleDVDPlay::IsLangKnown(void)
{ 
    if (NULL == m_pDvdI)       // most probably graph hasn't been built yet
        return FALSE ;

    // Otherwise see if we can find out our current SP stream id; that indicates it
    ULONG   ulNumLang ;
    ULONG   ulSPLang ;
    BOOL    bSPOn ;
    HRESULT hr = m_pDvdI->GetCurrentSubpicture(&ulNumLang, &ulSPLang, &bSPOn) ; 
    m_bLangKnown = SUCCEEDED(hr) ;

    return m_bLangKnown ;
}


BOOL CSampleDVDPlay::ShowMenu(void)
{
    if (NULL == m_pDvdC || NULL == m_pDvdI)
    {
        MessageBox(m_hWnd, TEXT("DVD-Video playback graph hasn't been built yet"), TEXT("Error"), MB_OK) ;
        return FALSE ;
    }

    DVD_DOMAIN   Domain ;
    HRESULT hr = m_pDvdI->GetCurrentDomain(&Domain) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_TRACE, 0, TEXT("IDvdInfo::GetCurentDomain() failed (0x%lx)"), hr)) ;
        return FALSE ;
    }

    //
    // Decide what the menu state flag should be
    //
    switch (Domain)
    {
        case DVD_DOMAIN_FirstPlay:          // lEvent = 1
        case DVD_DOMAIN_Stop:               // lEvent = 5
            break ;

        case DVD_DOMAIN_VideoManagerMenu:   // lEvent = 2
        case DVD_DOMAIN_VideoTitleSetMenu:  // lEvent = 3
            m_bMenuOn = TRUE ;
            break ;

        case DVD_DOMAIN_Title:              // lEvent = 4
            m_bMenuOn = FALSE ;
            break ;
    }

    if (m_bMenuOn)  // if ON now, turn it OFF
    {
        m_pDvdC->Resume() ;
        m_bMenuOn = FALSE ;
        // Inform the app to update the menu option to show "Menu" again
        PostMessage(m_hWnd, WM_COMMAND, IDM_USER_MENUSTATUS, (LPARAM) 0) ;
        return m_bMenuOn ;
    }

    // Turn root menu ON
    hr = m_pDvdC->MenuCall(DVD_MENU_Root) ;
    if (SUCCEEDED(hr))
    {
        m_bMenuOn = TRUE ;
        // Inform the app to update the menu option to show "Resume" now
        PostMessage(m_hWnd, WM_COMMAND, IDM_USER_MENUSTATUS, (LPARAM) 1) ;
        return m_bMenuOn ;
    }

    return m_bMenuOn ;
}


BOOL CSampleDVDPlay::ClosedCaption(void)
{
    IAMLine21Decoder  *pL21Dec ;
    m_pDvdGB->GetDvdInterface(IID_IAMLine21Decoder, (LPVOID *)&pL21Dec) ;
    if (NULL == pL21Dec)  // we should have not come here at all!!
        MessageBox(m_hWnd, TEXT("Line21 Decoder not found.  Can't show Closed Caption"), TEXT("Error"), MB_OK) ;
    else
    {
        m_bCCOn = !m_bCCOn ;
        pL21Dec->SetServiceState(m_bCCOn ? 
                        AM_L21_CCSTATE_On : AM_L21_CCSTATE_Off) ;
        pL21Dec->Release() ;
    }

    return m_bCCOn ;
}


BOOL CSampleDVDPlay::ShowFullScreen(void)
{
    if (NULL == m_pVW)
        MessageBox(m_hWnd, TEXT("Video interface not found.  Can't show in full screen mode."), TEXT("Error"), MB_OK) ;
    else
    {
        if (!m_bFullScrnOn)    
            StartFullScreen() ;
        else
            StopFullScreen() ;
    }

    return m_bFullScrnOn ;
}


void CSampleDVDPlay::SetRenderFlag(DWORD dwFlag)
{
    if (AM_DVD_NOVPE == dwFlag)  // VPE related flag
        m_dwRenderFlag ^= dwFlag ;
    else                         // HW/SW Dec related flag
        m_dwRenderFlag = (dwFlag | (AM_DVD_NOVPE & m_dwRenderFlag)) ;
}


DWORD CSampleDVDPlay::GetStatusText(AM_DVD_RENDERSTATUS *pStatus, 
				    LPTSTR lpszStatusText,
				    DWORD dwMaxText)
{
    TCHAR    achBuffer[1000] ;

    if (IsBadWritePtr(lpszStatusText, sizeof(*lpszStatusText) * dwMaxText))
    {
        DbgLog((LOG_ERROR, 0, TEXT("GetStatusText(): bad text buffer param"))) ;
        return 0 ;
    }

    int    iChars ;
    LPTSTR lpszBuff = achBuffer ;
    ZeroMemory(achBuffer, sizeof(TCHAR) * 1000) ;
    if (pStatus->iNumStreamsFailed > 0)
    {
        iChars = wsprintf(lpszBuff, 
                        TEXT("* %d out of %d DVD-Video streams failed to render properly\n"), 
                        pStatus->iNumStreamsFailed, pStatus->iNumStreams) ;
        lpszBuff += iChars ;

        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_VIDEO)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - video stream\n")) ;
            lpszBuff += iChars ;
        }
        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_AUDIO)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - audio stream\n")) ;
            lpszBuff += iChars ;
        }
        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_SUBPIC)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - subpicture stream\n")) ;
            lpszBuff += iChars ;
        }
    }

    if (FAILED(pStatus->hrVPEStatus))
    {
        lstrcat(lpszBuff, "* ") ;
        lpszBuff += lstrlen("* ") ;
        iChars = AMGetErrorText(pStatus->hrVPEStatus, lpszBuff, 200) ;
        lpszBuff += iChars ;
        lstrcat(lpszBuff, "\n") ;
        lpszBuff += lstrlen("\n") ;
    }

    if (pStatus->bDvdVolInvalid)
    {
        iChars = wsprintf(lpszBuff, TEXT("* Specified DVD-Video volume was invalid\n")) ;
        lpszBuff += iChars ;
    }
    else if (pStatus->bDvdVolUnknown)
    {
        iChars = wsprintf(lpszBuff, TEXT("* No valid DVD-Video volume could be located\n")) ;
        lpszBuff += iChars ;
    }

    if (pStatus->bNoLine21In)
    {
        iChars = wsprintf(lpszBuff, TEXT("* The video decoder doesn't produce closed caption data\n")) ;
        lpszBuff += iChars ;
    }
    if (pStatus->bNoLine21Out)
    {
        iChars = wsprintf(lpszBuff, TEXT("* Decoded closed caption data not rendered properly\n")) ;
        lpszBuff += iChars ;
    }
    
    DWORD dwLength = (lpszBuff - achBuffer) * sizeof(*lpszBuff) ;
    dwLength = min(dwLength, dwMaxText) ;
    lstrcpyn(lpszStatusText, achBuffer, dwLength) ;

    return dwLength ;
}


BOOL CSampleDVDPlay::FileSelect(void)
{
    OPENFILENAME  ofn ;
    TCHAR         achFileName[MAX_PATH] ;

    // Init the filename buffer with either a filename or *.ifo
    if (lstrlen(m_achFileName) > 0)
	lstrcpy(achFileName, m_achFileName) ;
    else
	lstrcpy(achFileName, TEXT("*.ifo")) ;

    ZeroMemory(&ofn, sizeof(OPENFILENAME)) ;
    ofn.lStructSize = sizeof(OPENFILENAME) ;
    ofn.hwndOwner = m_hWnd ;
    ofn.lpstrFilter = TEXT("IFO Files\0*.ifo\0All Files\0*.*\0") ;
    ofn.nFilterIndex = 1 ;
    ofn.lpstrFile = achFileName ;
    ofn.nMaxFile = sizeof(achFileName) ;
    ofn.lpstrFileTitle = NULL ;
    ofn.lpstrTitle = TEXT("Select DVD-Video Volume") ;
    ofn.nMaxFileTitle = 0 ;
    ofn.lpstrInitialDir = NULL ;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY ;

    if (GetOpenFileName(&ofn)) // user specified a file
    {
        if (m_pDvdC)  // Graph has already been built; set it now
        {
            Stop() ;  // first ask player to stop the playback.

            // Convert TCHAR filename to WCHAR for COM
            WCHAR   achwFileName[MAX_PATH] ;
#ifdef UNICODE
            lstrcpy(achwFileName, achFileName) ;
#else
            MultiByteToWideChar(CP_ACP, 0, achFileName, -1, achwFileName, MAX_PATH) ;
#endif // UNICODE            
            HRESULT hr = m_pDvdC->SetRoot(achwFileName) ;
            if (SUCCEEDED(hr))  // if the new file name is valid DVD-video volume
            {
                lstrcpy(m_achFileName, achFileName) ;  // store it
            }
            else
                DbgLog((LOG_ERROR, 2, TEXT("WARNING: SetRoot(%s) failed (Error 0x%lx)"), achFileName, hr)) ;
        }
        else   // graph not yet built; store name and use in graph building
        {
            lstrcpy(m_achFileName, achFileName) ;
        }

        return TRUE ;  // DVD-Video volume changed
    }
    
    // Either failed or user hit Esc.
    DWORD dw = CommDlgExtendedError() ;
    DbgLog((LOG_TRACE, 3, TEXT("GetOpenFileName() cancelled/failed with error %lu"), dw)) ;
    return FALSE ; // DVD-Video volume not changed
}


LPTSTR CSampleDVDPlay::GetStringRes(int id)
{
    LoadString(GetModuleHandle(NULL), id, m_achBuffer, 100) ;
    return m_achBuffer ;
}


HRESULT CSampleDVDPlay::OnDVDPlayEvent(WPARAM wParam, LPARAM lParam)
{
    ASSERT(lParam == (LONG)(LPVOID) m_pME) ;

    LONG     lEvent ;
    LONG     lParam1, lParam2 ;

    //
    //  Because the message mode for IMediaEvent may not be set before
    //  we get the first event it's important to read all the events
    //  pending when we get a window message to say there are events pending.
    //  GetEvent() returns E_ABORT when no more event is left.
    //
    while (SUCCEEDED(m_pME->GetEvent(&lEvent, &lParam1, &lParam2, 0))) // no wait
    {
        switch (lEvent)
        {
            //
            // First the DVD related events
            //
            case EC_DVD_STILL_ON:
		        if (1 == lParam1 && 0xFFFFFFFF == lParam2)
                {
                    wsprintf(m_achStillText, TEXT("Still On")) ;
                    InvalidateRect(m_hWnd, NULL, TRUE) ;
                }
                break ;

            case EC_DVD_STILL_OFF:
                wsprintf(m_achStillText, TEXT("Still Off")) ;
                InvalidateRect(m_hWnd, NULL, TRUE) ;
                break ;

            case EC_DVD_DOMAIN_CHANGE:
                switch (lParam1)
                {
                    case DVD_DOMAIN_FirstPlay:  // = 1
                    case DVD_DOMAIN_Stop:       // = 5
                    break ;

                    case DVD_DOMAIN_VideoManagerMenu:  // = 2
                    case DVD_DOMAIN_VideoTitleSetMenu: // = 3
                        // Inform the app to update the menu option to show "Resume" now
                        PostMessage(m_hWnd, WM_COMMAND, IDM_USER_MENUSTATUS, (LPARAM) 1) ;
                        m_bMenuOn = TRUE ;  // now menu is "On"
                        break ;

                    case DVD_DOMAIN_Title:      // = 4
                        // Inform the app to update the menu option to show "Menu" again
                        PostMessage(m_hWnd, WM_COMMAND, IDM_USER_MENUSTATUS, (LPARAM) 0) ;
                        m_bMenuOn = FALSE ; // now menu is "Off"
                        break ;

                    default: // hmmmm...
                        break ;
                }
                break ;

            case EC_DVD_BUTTON_CHANGE:
                DbgLog((LOG_TRACE, 5, TEXT("DVD Event: Button Changed to %d out of %d"),
                        lParam2, lParam1));
                break;
    
            case EC_DVD_TITLE_CHANGE:
                wsprintf(m_achTitleText, TEXT("Title %ld"), lParam1) ;
                InvalidateRect(m_hWnd, NULL, TRUE) ;
                break ;

            case EC_DVD_CHAPTER_START:
                wsprintf(m_achChapterText, TEXT("Chapter %ld"), lParam1) ;
                InvalidateRect(m_hWnd, NULL, TRUE) ;
                break ;

            case EC_DVD_CURRENT_TIME:
            {
                DVD_TIMECODE *pTime = (DVD_TIMECODE *) &lParam1 ;
                wsprintf(m_achTimeText, TEXT("Current Time is  %d%d:%d%d:%d%d"),
                        pTime->Hours10, pTime->Hours1,
                        pTime->Minutes10, pTime->Minutes1,
                        pTime->Seconds10, pTime->Seconds1) ;
                InvalidateRect(m_hWnd, NULL, TRUE) ;
            }
            break ;

            //
            // Then the general DirectShow related events
            //
            case EC_COMPLETE:
                DbgLog((LOG_TRACE, 5, TEXT("DVD Event: Playback complete"))) ;
                m_pMC->Stop() ;  // DShow doesn't stop on end; we should do that
                // fall through now...

            case EC_USERABORT:
            case EC_ERRORABORT:
            case EC_FULLSCREEN_LOST:
                DbgLog((LOG_TRACE, 5, TEXT("DVD Event: 0x%lx"), lEvent)) ;
                StopFullScreen() ;  // we must get out of fullscreen mode now
                break ;

            default:
                DbgLog((LOG_TRACE, 5, TEXT("Unknown DVD Event: 0x%lx"), lEvent)) ;
                break ;
        }

        //
        // Remember to free the event params
        //
        m_pME->FreeEventParams(lEvent, lParam1, lParam2) ;

    }  // end of while (GetEvent()) loop

    return 0 ;
}


void CSampleDVDPlay::DrawStatus(HDC hDC)
{
    TextOut(hDC,  10, 10, m_achStillText,   lstrlen(m_achStillText)) ;
    TextOut(hDC,  10, 25, m_achTitleText,   lstrlen(m_achTitleText)) ;
    TextOut(hDC, 110, 25, m_achChapterText, lstrlen(m_achChapterText)) ;
    TextOut(hDC,  10, 40, m_achTimeText,    lstrlen(m_achTimeText)) ;
}


void CSampleDVDPlay::CursorMove(CURSOR_DIR Dir)
{
    if (! m_bMenuOn )  // if menu is not ON, we can't select!!
        return ;

    switch (Dir)
    {
        case Cursor_Up:
            m_pDvdC->UpperButtonSelect() ;
            break ;

        case Cursor_Down:
            m_pDvdC->LowerButtonSelect() ;
            break ;

        case Cursor_Left:
            m_pDvdC->LeftButtonSelect() ;
            break ;

        case Cursor_Right:
            m_pDvdC->RightButtonSelect() ;
            break ;

        default:  // huh!!!
            break ;
    }
}


void CSampleDVDPlay::CursorSelect(void)
{
    if (m_bMenuOn)
        m_pDvdC->ButtonActivate() ;
}


BOOL CSampleDVDPlay::StartFullScreen()
{
    DbgLog((LOG_TRACE, 5, TEXT("CSampleDVDPlay::StartFullScreen()"))) ;

    if (NULL == m_pVW)
    {
        DbgLog((LOG_ERROR, 0, 
                TEXT("CSampleDVDPlay::StartFullScreen() -- no IVideoWindow pointer yet"))) ;
        return m_bFullScrnOn ;
    }

    HRESULT   hr ;
    hr = m_pVW->put_MessageDrain((OAHWND) m_hWnd) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, 
                TEXT("CSampleDVDPlay::StartFullScreen() -- IVideoWindow::put_MessageDrain() failed (0x%lx)"),
                hr)) ;
        return m_bFullScrnOn ;
    }
    hr = m_pVW->put_FullScreenMode(OATRUE) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("CSampleDVDPlay::StartFullScreen() -- IVideoWindow::put_FullScreenMode() failed (0x%lx)"),
            hr)) ;
        return m_bFullScrnOn ;
    }

    m_bFullScrnOn = TRUE ;  // we are really in full screen mode

    return m_bFullScrnOn ;
}


BOOL CSampleDVDPlay::StopFullScreen()
{
    DbgLog((LOG_TRACE, 5, TEXT("CSampleDVDPlay::StopFullScreen()"))) ;

    HRESULT   hr ;

    if (NULL == m_pVW)
    {
        DbgLog((LOG_ERROR, 0, 
                TEXT("CSampleDVDPlay::StopFullScreen() -- no IVideoWindow pointer yet"))) ;
        return m_bFullScrnOn ;
    }

    hr = m_pVW->put_FullScreenMode(OAFALSE) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, 
                TEXT("CSampleDVDPlay::StopFullScreen() -- IVideoWindow::put_FullScreenMode() failed (0x%lx)"),
                hr)) ;
        return m_bFullScrnOn ;
    }
    hr = m_pVW->put_MessageDrain((OAHWND) NULL) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, 
                TEXT("CSampleDVDPlay::StopFullScreen() -- IVideoWindow::put_MessageDrain() failed (0x%lx)"),
                hr)) ;
        return m_bFullScrnOn ;
    }

    m_bFullScrnOn = FALSE ;  // we are no more in full screen mode

    return m_bFullScrnOn ;
}


CViewLevels::CViewLevels()
{
    m_iCount = 6 ;
    m_alpszNames[0] = TEXT(" G ") ;       m_aiValues[0] = 1 ;
    m_alpszNames[1] = TEXT(" PG ") ;      m_aiValues[1] = 3 ;
    m_alpszNames[2] = TEXT(" PG-13 ") ;   m_aiValues[2] = 4 ;
    m_alpszNames[3] = TEXT(" R ") ;       m_aiValues[3] = 6 ;
    m_alpszNames[4] = TEXT(" NC-17 ") ;   m_aiValues[4] = 7 ;
    m_alpszNames[5] = TEXT(" Adult ") ;   m_aiValues[5] = 8 ;
}


//
// Only 10 languages have been used here as a sample. The list can be extended to
// include any language listed in ISO 639.
//
CDVDLanguages::CDVDLanguages()
{
    m_alpszCodes[0] = TEXT("de") ;    m_alpszLangNames[0] = TEXT("German") ;
    m_alpszCodes[1] = TEXT("en") ;    m_alpszLangNames[1] = TEXT("English") ;
    m_alpszCodes[2] = TEXT("es") ;    m_alpszLangNames[2] = TEXT("Spanish") ;
    m_alpszCodes[3] = TEXT("fr") ;    m_alpszLangNames[3] = TEXT("French") ;
    m_alpszCodes[4] = TEXT("ja") ;    m_alpszLangNames[4] = TEXT("Japanese") ;
    m_alpszCodes[5] = TEXT("ko") ;    m_alpszLangNames[5] = TEXT("Korean") ;
    m_alpszCodes[6] = TEXT("ni") ;    m_alpszLangNames[6] = TEXT("Dutch") ;
    m_alpszCodes[7] = TEXT("pt") ;    m_alpszLangNames[7] = TEXT("Portuguese") ;
    m_alpszCodes[8] = TEXT("sv") ;    m_alpszLangNames[8] = TEXT("Swedish") ;
    m_alpszCodes[9] = TEXT("zh") ;    m_alpszLangNames[9] = TEXT("Chinese") ;
}

BOOL CDVDLanguages::GetLangString(LPTSTR lpszCode, LPTSTR lpszLang)
{
    for (int i = 0 ; i < 10 ; i++)
    {
        if (0 == lstrcmp(lpszCode, m_alpszCodes[i]))  // match!!
        {
            lstrcpy(lpszLang, m_alpszLangNames[i]) ;
            return TRUE ;  // got a match
        }
    }
    lstrcpy(lpszLang, TEXT("Unknown")) ;
    return FALSE ;  // didn't get a match
}
