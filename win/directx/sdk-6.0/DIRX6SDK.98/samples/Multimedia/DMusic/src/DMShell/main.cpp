/*****************************************************************************\
*  main.cpp - 
*
*  CMain is the primary object for dealing with window creation and message dispatching
*		It loads and communicates with DMHOOK.DLL
*		It contains a DMPlayer object it uses to control the music
*		It handles the menu selections
*
\*****************************************************************************/


#include "main.h"


// The single global variable
CMain theMain;


//  Entry point for the application.
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	return theMain.WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

int CMain::WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Perform instance initialization:
	if (!InitApplication(hInstance))
		return FALSE;

	// Perform application initialization:
	if (!InitInstance(hInstance))
		return FALSE;

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == m_uHookEvent)
		{
			if (m_pDMPlayer != NULL)
				m_pDMPlayer->PlayEvent(msg.wParam, msg.lParam);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (msg.wParam);
}

// Initialize window data and registers window class
BOOL CMain::InitApplication(HINSTANCE hInstance)
{
    // Bail if application is already running
    HWND hWnd = FindWindow (CLASSNAME, WINDOWNAME);
    if (hWnd)
        return FALSE;

    // Create a class structure for the target window
    WNDCLASSEX  wc;
	ZeroMemory(&wc, sizeof(wc));
    wc.cbSize		 = sizeof(wc);
    wc.lpfnWndProc   = ::WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASSNAME;

	return RegisterClassEx(&wc);
}

//  Create the target window and init the DLL and DMPlayer Object
BOOL CMain::InitInstance(HINSTANCE hInstance)
{
	m_hInst = hInstance;

	// Create the invisible target window
	m_hWnd = CreateWindow(CLASSNAME, WINDOWNAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);

	if (!m_hWnd)
		return FALSE;

	m_hhkGetMessage = NULL;
    m_hhkCallWndProc = NULL;
	m_hDLL = NULL;
	m_pDMPlayer = NULL;
	m_wActiveScheme = 0;

	// Create a registered event for the incoming event messages from DMHOOK.DLL
	m_uHookEvent = RegisterWindowMessage ( DME_REGISTEREDEVENT );

	// Build the icon in the tray
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.hIcon = LoadIcon(m_hInst, "MAIN");
	nid.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
	strcpy( nid.szTip, DMS_TIP);
	// Establish a message ID for communication from the tray icon to the target window
	m_uUIEvent = RegisterWindowMessage( DMS_UIEVENT );
	nid.uCallbackMessage = m_uUIEvent;
	Shell_NotifyIcon( NIM_ADD, &nid); 

	// Load DMHOOK.DLL
    if (!(m_hDLL = LoadLibrary("dmhook")))
    {
        MessageBox(m_hWnd, "Unable to load DMHOOK.DLL.", NULL, MB_OK | MB_ICONEXCLAMATION );
        return FALSE;
    }

	// Establish a message hook that flows through HookGetMsgProc in DMHOOK.DLL
    if (!(m_hhkGetMessage = SetWindowsHookEx(WH_GETMESSAGE,
        (HOOKPROC)GetProcAddress(m_hDLL, "HookGetMsgProc"), m_hDLL, 0)))
    {
        return FALSE;
    }

    // Establish a hook for Windows procedures through HookCallWndProc in DMHOOK.DLL
    if (!(m_hhkCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC,
        (HOOKPROC)GetProcAddress(m_hDLL, "HookCallWndProc"), m_hDLL, 0)))
    {
        UnhookWindowsHookEx(m_hhkCallWndProc);
        return FALSE;
    }

	// Initialize the DMPlayer
	HWND hWndDesktop = GetDesktopWindow();
	m_pDMPlayer = new CDMPlayer(hWndDesktop);

	return TRUE;
}

//  Processes messages for the main (target) window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return theMain.WndProc(hWnd, message, wParam, lParam);
}

LRESULT CMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == m_uUIEvent)
	{
		if (lParam == WM_LBUTTONDBLCLK || lParam == WM_LBUTTONDOWN)
		{
			CreatePopupMenu();
		}
		return TRUE;
	}

	WORD wId;
	switch (message)
	{
		case WM_COMMAND:
			wId = LOWORD(wParam); // Extract the menu ID
			if (DecodeMenuSelection(wId, hWnd))
				return TRUE;
			else
				return (DefWindowProc(hWnd, message, wParam, lParam));

		case WM_DESTROY:
			ExitApplication();
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return TRUE;
}

LRESULT CALLBACK AboutBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			ShowWindow(hWnd, SW_HIDE);

			// Determine the center of the screen to position the about box.
			RECT    rDlg, rDesktop;
			int     wDlg, hDlg, wDesktop, hDesktop;
			int     xNew, yNew;

			// Get the Height and Width of the dialog window
			GetWindowRect(hWnd, &rDlg);
			wDlg = rDlg.right - rDlg.left;
			hDlg = rDlg.bottom - rDlg.top;

			// Get the Height and Width of the desktop
			GetWindowRect( GetDesktopWindow(), &rDesktop);
			wDesktop = rDesktop.right - rDesktop.left;
			hDesktop = rDesktop.bottom - rDesktop.top;

			// Calculate the new upper left corner to center the box
			xNew = rDesktop.left + ((wDesktop - wDlg) /2);
			yNew = rDesktop.top  + ((hDesktop - hDlg) /2);

			// Reposition and display
			SetWindowPos(hWnd, HWND_TOPMOST, xNew, yNew, 0, 0, SWP_NOSIZE);
			ShowWindow(hWnd, SW_SHOW);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hWnd, TRUE);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

void CMain::ExitApplication()
{
	//Remove DMShell icon from the tray
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	Shell_NotifyIcon( NIM_DELETE, &nid); 

    if (m_pDMPlayer)
		delete m_pDMPlayer;
	
	// Unhook and unload the DLL
	if (m_hhkGetMessage)
        UnhookWindowsHookEx(m_hhkGetMessage);

    if (m_hhkCallWndProc)
        UnhookWindowsHookEx(m_hhkCallWndProc);

    if (m_hDLL)
		FreeLibrary(m_hDLL);

	PostQuitMessage(0);
}

// Dispatch the incoming command via the menu selection
// Return TRUE if acted upon selection, FALSE if not
BOOL CMain::DecodeMenuSelection(WORD wId, HWND hWnd)
{
	// Determine if this is a menu selection to switch schemes
	if (IDM_SCHEMEMENUBASE <= wId && wId < IDM_SCHEMEMENUBASE + NUM_SCHEMES)
	{
		WORD wScheme = wId - IDM_SCHEMEMENUBASE;
		m_wActiveScheme = wScheme;
		if (m_pDMPlayer)
		{
			m_pDMPlayer->SelectNewScheme(wScheme);
			m_pDMPlayer->Start();
		}
		return TRUE;
	}

	// Determine if this is a menu selection to switch ports
	if (IDM_PORTINDEXBASE <=wId && wId < IDM_PORTINDEXBASE + NUM_PORTS)
	{
		WORD wPortIndex = wId - IDM_PORTINDEXBASE;
		if (m_pDMPlayer)
			m_pDMPlayer->SelectOutputPort(wPortIndex);

		return TRUE;
	}

	// Check for remaining menu selections:
	switch (wId)
	{
		case IDM_ABOUT:
			DialogBox(m_hInst, "ABOUTBOX", hWnd, (DLGPROC)AboutBox);
			return TRUE;

		case IDM_EXIT:
			DestroyWindow (hWnd);
			return TRUE;

		case IDM_START:
			if (m_pDMPlayer)
				m_pDMPlayer->Start();
			return TRUE;

		case IDM_STOP:
			if (m_pDMPlayer)
				m_pDMPlayer->Stop();
			return TRUE;
	}
	return FALSE;
}

void CMain::CreatePopupMenu()
{
	HMENU hPrimaryMenu = LoadMenu(m_hInst, "MENUPOPUP");
	HMENU hMenu = GetSubMenu ( hPrimaryMenu, 0);
	if (hMenu == NULL)
		return;

	if (m_pDMPlayer)
	{
		if (m_pDMPlayer->IsPlaying() )
			EnableMenuItem(hMenu, IDM_START, MF_GRAYED | MF_BYCOMMAND);

		else
			EnableMenuItem(hMenu, IDM_STOP, MF_GRAYED | MF_BYCOMMAND);

		// Build the selection of scheme choices
		HMENU hSchemeMenu = GetSubMenu(hMenu, 0);
		if (hSchemeMenu)
		{
			RemoveMenu(hSchemeMenu, 0, MF_BYPOSITION);
			WCHAR wzSchName[MENU_FIELD_LENGTH];
			CHAR szSchName[MENU_FIELD_LENGTH];
			for (int x = 0 ; x < NUM_SCHEMES ; x++ )
			{
				if (m_pDMPlayer->GetSchemeName(x, (PWSTR)&wzSchName, MENU_FIELD_LENGTH ) )
				{
					WideToByte(wzSchName, szSchName, MENU_FIELD_LENGTH );
					AppendMenu(hSchemeMenu, MF_STRING, IDM_SCHEMEMENUBASE + x, szSchName );
				}
			}
			CheckMenuItem(hSchemeMenu, IDM_SCHEMEMENUBASE + m_wActiveScheme, MF_CHECKED);
		}

		// Build the selection of DirectMusic output ports
		HMENU hPortMenu = GetSubMenu(hMenu, 2);
		if (hPortMenu)
		{
			WCHAR wzPortName[MENU_FIELD_LENGTH];
			CHAR szPortName[MENU_FIELD_LENGTH];
			WORD wEnumPortReturn;
			WORD wActivePort = 0;
			RemoveMenu(hPortMenu, 0, MF_BYPOSITION);
			for (WORD x = 0 ; x < NUM_PORTS ; x++ )
			{
				wEnumPortReturn = m_pDMPlayer->EnumOutputPort(x, (PWSTR)&wzPortName, MENU_FIELD_LENGTH );
				if (wEnumPortReturn == VALID_PORT || wEnumPortReturn == SELECTED_PORT)
				{
					WideToByte(wzPortName, szPortName, MENU_FIELD_LENGTH );
					AppendMenu(hPortMenu, MF_STRING, IDM_PORTINDEXBASE + x, (PSTR)&szPortName );
				}
				if (wEnumPortReturn == SELECTED_PORT)
					wActivePort = x;
			}
			CheckMenuItem(hPortMenu, IDM_PORTINDEXBASE + wActivePort, MF_CHECKED);
		}
	}

	// Calculate where to popup the menu. Account for the fact that the taskbar
	//     may be mounted to any of the four screen edges.
	HWND hWnd = FindWindow( "Shell_TrayWnd", NULL);
	RECT rTaskbar, rDesktop;
	GetWindowRect(hWnd, &rTaskbar);
	hWnd = GetDesktopWindow();
	GetWindowRect(hWnd, &rDesktop);

	int x, y;
	x = rTaskbar.right - 2;  //Taskbar hangs off the edge of the screen by two pixels
	if (rTaskbar.top < 0)
	{
		//top left or right mounted
		if (rTaskbar.bottom > rDesktop.bottom)
		{
			//left or right mounted
			y = rDesktop.bottom;
			if (rTaskbar.left < 0)
				//left mounted
				x = rTaskbar.right;
			else
				//right mounted
				x = rTaskbar.left;
		}
		else
			//top mounted
			y = rTaskbar.bottom;
	}
	else
		//bottom mounted
		y = rTaskbar.top;
		
	// Display the popup menu
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, x, y, 0, m_hWnd, &rDesktop);
	DestroyMenu(hPrimaryMenu);
}


void CMain::WideToByte(PWSTR pwzInput, PSTR pszOutput, WORD wStrLength)
{
	WideCharToMultiByte( CP_ACP, 0, pwzInput, wStrLength, pszOutput, wStrLength, NULL, NULL);
}
