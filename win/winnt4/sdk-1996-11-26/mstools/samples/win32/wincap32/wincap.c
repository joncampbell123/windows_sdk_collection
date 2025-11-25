//***********************************************************************
//
// Wincap.c
//
// Windows Screen Capture Utility
// Version 3.20
//
// Description:
// ------------
//
// Captures portions of the screen, specific windows, or the entire screen
// and saves it to a file or prints it.  Uses DIBAPI functions to do most
// of the capture/printing/saving work.  See the file DIBAPI.TXT for a
// description of the DIB api functions.
//
// Changes from first version:
//
// - Updated to use 3.1 Common Dialogs and 3.1 hook functions
// - New user interface displays window which was captured in client
//      area of Wincap, complete with scroll bars
// - Hot new hotkeys allow more versatile capturing of windows (e.g. you
//      can now capture windows with menu items pulled down)
// - New APIs to draw bitmaps and DIBs on the screen
// - Nifty startup bitmap
// - All DIB API functions are now in a DLL, which can be used
//      by any application
//
// Changes from version 3.1 to 3.2:
// - Window selection changed due to how SetCapture works under Win32
// - The window to be selected for capture is highlighted as the cursor is moved
// - Option to copy DIB, DDB, and Palette to the clipboard
//
// Written by Microsoft Product Support Services, Developer Support.
// Copyright (c) 1991-1996 Microsoft Corporation. All rights reserved.
//***********************************************************************

#define STRICT
#include <windows.h>
#include <string.h>
#include "commdlg.h"
#include "wincap.h"
#include "resource.h"
#include "dialogs.h"
#include "dibapi.h"

char szAppName[20];     // Name of application - used in dialog boxes

// Global variables

HINSTANCE   ghInst;             // Handle to instance
HWND        ghWndMain;          // Handle to main window

HOOKPROC    lpfnKeyHook;        // Used in keyboard hook
HOOKPROC    lpfnOldHook;        // Used for keyboard hook
HWND        hModelessDlg;       // Handle to modeless "Saving to file..."
                                // dialog box

BOOL        bStartup=TRUE;      // Startup flag for WM_PAINT/logo
BOOL        bViewFull=FALSE;    // Full view flag
HBITMAP     ghbmLogo;           // Handle to logo bitmap
HBITMAP     ghBitmap=NULL;      // Handle to captured bitmap
HPALETTE    ghPal=NULL;         // Handle to our bitmap's palette
char        gszWindowText[100]; // Text which tells what we captured

BOOL        gbLButtonDown=FALSE;
BOOL        gbSave = FALSE;
HWND        ghWndCapture=NULL;
BOOL        gbNowCapturing = FALSE;
BOOL        gbCaptRect = FALSE;
HWND        hSelectDlg;         // help dialog for window selection
HWND        hRectangleDlg;      // help dialog for rectangle selection
UINT        guiFileOKMsg;       // for common dialog FILEOKSTRING

#define WM_DOCAPTURE WM_USER+101 // Used for screen capture messages

// Macro to swap two values

#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))
#define SCROLL_RATIO    4

BOOL CenterWindow (HWND hwndChild, HWND hwndParent);

//************************************************************************
//
// WinMain()
//
// Entry point of the Application.
//
//************************************************************************

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
        int nCmdShow)
{
    MSG msg;
    WNDCLASS wndclass;
    HWND hWnd;
	CHAR lpBuffer[256];

    strcpy(szAppName, "WinCap");     // Name of our App
    hModelessDlg = NULL;             // Set handle to modeless dialog to NULL
                                     // because we haven't created it yet

    hSelectDlg = hRectangleDlg = NULL;

	LoadString(ghInst, IDS_MAINWINDOWTITLE, lpBuffer, sizeof(lpBuffer));
    if (!FindWindow(szAppName, lpBuffer))
    {
        wndclass.style = 0;
        wndclass.lpfnWndProc = (WNDPROC)WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, "WINCAP");
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

        // Use black for background for better contrast

        wndclass.hbrBackground = GetStockObject(GRAY_BRUSH);

        wndclass.lpszMenuName = (LPSTR)"MAINMENU";
        wndclass.lpszClassName = (LPSTR)szAppName;

        if (!RegisterClass(&wndclass))
            return FALSE;

        ghInst = hInstance;  // Set Global variable

        // Create a main window for this application instance.

        hWnd = CreateWindow(szAppName, lpBuffer, WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 450, 345, NULL, NULL, hInstance,            // This instance owns this window
                NULL);

        ghWndMain = hWnd;      // Set global variable

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        // Set up the Keyboard hook for our hotkeys

        InstallHook(hWnd, TRUE);  // Function resides in DIBAPI32.DLL

        // Create our full-screen view class

        wndclass.style = 0;
        wndclass.lpfnWndProc = (WNDPROC)FullViewWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = GetStockObject(GRAY_BRUSH);
        wndclass.lpszMenuName = (LPSTR)NULL;
        wndclass.lpszClassName = (LPSTR)"ViewClass";

        if (!RegisterClass(&wndclass))
            return FALSE;

    }

    // Let's make this a single-instance app -- we can get into hotkey
    // conflicts (e.g. windows won't know which instance of WINCAP to 
    // send the message to).  
    
    else
    {
        LoadString(ghInst, IDS_WINCAPRUNNING, lpBuffer, sizeof(lpBuffer));
		MessageBeep(0);
        MessageBox(NULL, lpBuffer, szAppName, MB_OK | MB_ICONHAND);
        return FALSE;
    }

    // Polling messages from event queue -- we have a modeless dialog
    // box, so we have to take care of the messages here also

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (hModelessDlg == NULL || !IsDialogMessage(hModelessDlg, &msg) ||
                hSelectDlg || !IsDialogMessage(hSelectDlg, &msg) ||
                hRectangleDlg || !IsDialogMessage(hRectangleDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}



//**********************************************************************
//
// WndProc()
//
// This is our main window procedure.  It receives all the messages destined
// for our application's main window.
//
// When we capture a window, we capture it into a Device-Dependent bitmap,
// and at the same time, we get a copy of the current system palette.  This
// makes displaying the bitmap on the screen very fast.  And when we want
// to print or save the captured window, we need to use this palette to
// convert the DDB to a DIB.
//
//*********************************************************************

long WINAPI WndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    
    // The gbNowCapturing variable is set to TRUE if we are in the middle of
    // printing.  This takes care of the case when the user presses the hotkey
    // during capturing
    
    static BOOL     bCapturedYet = FALSE;  // TRUE if window contains captured screen
    HWND            hViewWnd;              // Handle to our view window
    static WORD     wCaptureCommand;
    static HCURSOR  hOldCursor;
    static HWND     hBrowseWnd;
    CHAR            lpBuffer[128];
    CHAR            lpBuffer2[128];


    switch (wMessage)
    {

        case WM_CREATE:
            if (!hSelectDlg)
                hSelectDlg = CreateDialog(ghInst, "Select", hWnd,
                        SelectDlgProc);

            if (!hRectangleDlg)
                hRectangleDlg = CreateDialog(ghInst, "Rectangle", hWnd,
                        RectangleDlgProc);


            ghbmLogo = LoadBitmap(ghInst, "STARTBMP");
            guiFileOKMsg = RegisterWindowMessage( (LPCSTR)FILEOKSTRING );
            break;

        // Gray out menu items if we haven't captured anything yet.

        case WM_INITMENU:
            EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_BYCOMMAND |
                    (bCapturedYet ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
            EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_BYCOMMAND |
                    (bCapturedYet ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
            EnableMenuItem(GetMenu(hWnd), IDM_EDITCOPY, MF_BYCOMMAND |
                    (bCapturedYet ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
            return 0;


        // The WM_PALETTECHANGED message informs all windows that the window
        // with input focus has realized its logical palette, thereby changing 
        // the system palette. This message allows a window without input focus
        // that uses a color palette to realize its logical palettes and update
        // its client area.
        //
        // This message is sent to all windows, including the one that changed
        // the system palette and caused this message to be sent. The wParam of
        // this message contains the handle of the window that caused the system
        // palette to change. To avoid an infinite loop, care must be taken to
        // check that the wParam of this message does not match the window's
        // handle.

        case WM_PALETTECHANGED:
        {
            HDC         hDC;      // Handle to device context
            HPALETTE    hOldPal;  // Handle to previous logical palette

            // Before processing this message, make sure we
            // are indeed using a palette

            if (ghPal)
            {
                // If this application did not change the palette, select
                // and realize this application's palette

                if (wParam != (WPARAM)hWnd)
                {
                    // Need the window's DC for SelectPalette/RealizePalette

                    hDC = GetDC(hWnd);

                    // Select and realize our palette

                    hOldPal = SelectPalette(hDC, ghPal, FALSE);
                    RealizePalette(hDC);

                    // WHen updating the colors for an inactive window,
                    // UpdateColors can be called because it is faster than
                    // redrawing the client area (even though the results are
                    // not as good)

                    UpdateColors(hDC);

                    // Clean up

                    if (hOldPal)
                       SelectPalette(hDC, hOldPal, FALSE);

                    ReleaseDC(hWnd, hDC);
                }
            }
            break;
        }


        // The WM_QUERYNEWPALETTE message informs a window that it is about to
        // receive input focus. In response, the window receiving focus should
        // realize its palette as a foreground palette and update its client
        // area. If the window realizes its palette, it should return TRUE;
        // otherwise, it should return FALSE.

        case WM_QUERYNEWPALETTE:
        {
            HDC         hDC;      // Handle to device context
            HPALETTE    hOldPal;  // Handle to previous logical palette

            // Before processing this message, make sure we
            // are indeed using a palette

            if (ghPal)
            {
                 // Need the window's DC for SelectPalette/RealizePalette

                 hDC = GetDC(hWnd);

                 // Select and realize our palette

                 hOldPal = SelectPalette(hDC, ghPal, FALSE);
                 RealizePalette(hDC);

                 // Redraw the entire client area

                 InvalidateRect(hWnd, NULL, TRUE);
                 UpdateWindow(hWnd);

                 // Clean up

                 if (hOldPal)
                    SelectPalette(hDC, hOldPal, FALSE);

                 ReleaseDC(hWnd, hDC);

                 // Message processed, return TRUE

                 return TRUE;
             }

            // Message not processed, return FALSE

            return FALSE;
        }



        case WM_MOVE:
        {
            if (gbNowCapturing)
            {
                if (gbCaptRect)
                    CenterWindow(hRectangleDlg, hWnd);
                else
                    CenterWindow(hSelectDlg, hWnd);
            }

            return 0;
        }

        case WM_SIZE:
        {
            static BOOL     bSizing=FALSE;

            if (gbNowCapturing)
            {
                if (gbCaptRect)
                    CenterWindow(hRectangleDlg, hWnd);
                else
                    CenterWindow(hSelectDlg, hWnd);
            }

            // Check if we are already sizing

            if (bSizing)
                return 0l;

            bSizing = TRUE;
            DoSize(hWnd);
            bSizing = FALSE;
            break;
        }

        case WM_HSCROLL:
        case WM_VSCROLL:
            DoScroll(hWnd, wMessage, (int)HIWORD(wParam), (int)LOWORD(wParam));
            break;

        case WM_PAINT:
            DoPaint(hWnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_ABOUT:
                    DialogBox(ghInst, (LPSTR)"About", hWnd, AboutDlgProc);
                    break;

                case IDM_SAVE:
                    SaveMe();
                    break;

                case IDM_PRINT:
                    PrintMe();
                    break;

                case IDM_VIEWFULL:
                    if (!bViewFull && (ghBitmap || bStartup))
                    {
                        HDC     hDC;
                        int ScreenX, ScreenY;

                        hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

                        ScreenX = GetDeviceCaps(hDC, HORZRES);

                        ScreenY = GetDeviceCaps(hDC, VERTRES);
                        DeleteDC(hDC);

                        hViewWnd = CreateWindow((LPSTR)"ViewClass", (LPSTR)NULL,
                                WS_POPUP | WS_VISIBLE, 0, 0, ScreenX, ScreenY,
                                hWnd, NULL, ghInst, NULL);

                        ShowWindow(hViewWnd, SW_SHOW);
                        UpdateWindow(hViewWnd);
                    }
                    else {   
                        LoadString(ghInst, IDS_NOIMAGE, lpBuffer, sizeof(lpBuffer));
                        LoadString(ghInst, IDS_VWFULLSCRN, lpBuffer2, sizeof(lpBuffer2));
                        MessageBox(hWnd, lpBuffer, lpBuffer2, MB_OK);
                    }
                    break;
            
                case IDM_EDITCOPY: // Copy DIB, bitmap, & palette to clipboard        
                    if (OpenClipboard(hWnd))
                    {
                        HDIB        hDib;
                        HBITMAP     hBitmap;
                        HPALETTE    hPal;

                        EmptyClipboard();

                        if (ghBitmap)
                        {
                            // Once these are added to the clipboard, the
                            // clipboard owns the data.

                            if (hDib = BitmapToDIB(ghBitmap, ghPal))
                                SetClipboardData(CF_DIB, hDib);

                            // Make a copy of the bitmap so the clipboard can
                            // own it.

                            if (hBitmap = DIBToBitmap(hDib, ghPal))
                                SetClipboardData(CF_BITMAP, hBitmap);

                            // Make a copy of the palette so the clipboard can
                            // own it.

                            if (hPal = GetStockObject(DEFAULT_PALETTE))
                                SetClipboardData(CF_PALETTE, hPal);
                        }

                        CloseClipboard();
                    }
                    break;

                case IDM_VIEWCLEAR:
                {
                    WORD        wRet;

                    if (gbSave)
                    {
                        LoadString(ghInst, IDS_SAVEBMP, lpBuffer, sizeof(lpBuffer));
                        wRet = MessageBox(hWnd, lpBuffer,
                                szAppName, MB_ICONEXCLAMATION | MB_YESNOCANCEL);

                        if (wRet == IDYES)
                            SaveMe();
                        else if( wRet == IDCANCEL)
                            break;
                    }


                    gbSave = FALSE;

                    // If we are just displaying logo, don't
                    // display it anymore

                    if (bStartup)
                        bStartup = FALSE;

                    // Delete captured bitmap if we have one

                    if (ghBitmap)
                    {
                        DeleteObject(ghBitmap);
                        ghBitmap = NULL;
                    }

                    // Delete our captured bitmap's palette if we have one

                    if (ghPal)
                    {
                        DeleteObject(ghPal);
                        ghPal = NULL;
                    }

                    // Now update display to reflect fact that we
                    // nuked the captured bitmap or don't want to
                    // look at the cool logo

                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);

                    bCapturedYet = FALSE;  // Gray out "File.Save" menu item
                    break;
                }
                

                case IDM_CAPTRECT:
                    gbCaptRect = TRUE;

                case IDM_CAPTWINDOW:  // make sure lParam == IDM_CURSORSELECT
                case IDM_CAPTCLIENT:  // make sure lParam == IDM_CURSORSELECT
                    lParam = IDM_CURSORSELECT;

                case IDM_ACTIVEWINDOW:
                case IDM_DESKTOP:

         
                // User selected one of the window capture items

                // Check to see that we aren't already in the middle of 
                // capturing.  This could happen if the user presses our hotkey
                // in the middle of one of our dialog boxes.

                if (gbNowCapturing)
                {
                    LoadString(ghInst, IDS_CAPTEDALREADY, lpBuffer, sizeof(lpBuffer));
                    MessageBox(NULL, lpBuffer,
                            szAppName, MB_OK | MB_ICONEXCLAMATION);
                }
                else
                {
                    // User wants to capture screen.  One problem we may
                    // run into here is that we might have a popup menu pulled
                    // down in our own application when we get to this point
                    // (because of the hotkey feature of this app).
                    //
                    // Normally, we'd just enter a message loop after calling
                    // ShowWindow(SW_HIDE) to take care of any menu messages
                    // which may have been posted as we hide our application's
                    // window, and *then* call our screen capture routine.
                    // But unfortunately, we can't do that here and be 100% safe.
                    //
                    // If we *have* been sent here on a hotkey, and a menu of
                    // our own app is currently down, then we are currently running
                    // inside of a PeekMessage() loop in the Windows Menu
                    // manager. We should *not* enter another PeekMessage loop,
                    // but should return from this message case right away.
                    // The Windows Menu manager code relies on checking messages
                    // in it's queue via it's own PeekMessage loop, and if we
                    // entered one, it would confuse the menu manager.
                    //
                    // So what we do instead is just post ourselves a private
                    // message, and when we get this message (see below), then
                    // do the screen capture.
                    //

                    // Commence screen capture!

                    gbNowCapturing = TRUE;

                    if (wParam == IDM_DESKTOP)
                        ShowWindow(hWnd, SW_HIDE);

                    // Allow this message case to return right away.  We'll
                    // capture screen down below.

                    if (lParam != IDM_CURSORSELECT)
                        PostMessage(hWnd, WM_DOCAPTURE, wParam, 0L);
                    else if (gbCaptRect)
                    {
                        CenterWindow(hRectangleDlg, hWnd);
                        ShowWindow(hRectangleDlg, SW_SHOW);
                    }
                    else
                    {
                        CenterWindow(hSelectDlg, hWnd);
                        ShowWindow(hSelectDlg, SW_SHOW);
                    }

                    wCaptureCommand = (WORD)wParam;
                }
                break;

                case IDM_EXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    break;

                default:
                    return DefWindowProc(hWnd, wMessage, wParam, lParam);
                    break;
            } //switch (LOWORD(wParam))

            break; // case WM_COMMAND:


        // Message case for doing screen capture.  This message is posted
        // to us from the IDM_CAPT* code above.  wParam should be equal to
        // the ID of the message which we got sent here for (it is used in
        // the call to DoCapture()).

        case WM_DOCAPTURE:
        {
            // We're going to capture a new screen, get rid of
            // previous captured image

            if (ghBitmap)
            {
                DeleteObject(ghBitmap);
                ghBitmap = NULL;
            }

            if (ghPal)
            {
                DeleteObject(ghPal);
                ghPal = NULL;
            }

            SetForegroundWindow(ghWndCapture);

            // Save captured screen as bitmap

            DoCapture(hWnd, (WORD)wParam);

            SetForegroundWindow(hWnd);

            // Un-hide Window

            ShowWindow(hWnd, SW_SHOW);

            if (IsIconic(hWnd))
                PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE,  0L);

            if (!gbCaptRect)
                gbNowCapturing = FALSE;

            bCapturedYet = TRUE; // Enable "File.Save" menu item
            gbSave = TRUE;
            break;
        }

        case WM_LBUTTONDOWN:
        {

            if (!gbNowCapturing)
                break;
            
            ShowWindow(hWnd, SW_HIDE);  // Hide main app's window
            if (gbCaptRect)
                ShowWindow(hRectangleDlg, SW_HIDE);
            else
                ShowWindow(hSelectDlg, SW_HIDE);
            if (gbCaptRect)
            {
                hOldCursor = SetCursor(LoadCursor(NULL, IDC_CROSS));
                PostMessage(hWnd, WM_DOCAPTURE, wCaptureCommand, 0L);
            }
            else
                hOldCursor = SetCursor(LoadCursor(ghInst, "SELECT"));
            gbLButtonDown = TRUE;
            SetCapture(ghWndMain);
            break;
        }


        case WM_LBUTTONUP:
        {
            POINTS      pts;
            POINT       pt;

            if (!(gbNowCapturing && gbLButtonDown))
                break;
            
            ReleaseCapture();
            gbLButtonDown = FALSE;
            SetCursor(hOldCursor);

            if (gbCaptRect)
            {
                gbCaptRect = FALSE;
                gbNowCapturing = FALSE;
                break;
            }

            FrameWindow(hBrowseWnd);
            hBrowseWnd = NULL;

            pts = MAKEPOINTS(lParam);
            pt.x = pts.x;
            pt.y = pts.y;

            // Convert to screen coordinates
            
            ClientToScreen(ghWndMain, &pt);
            
            // Get Window that we clicked on
            
            ghWndCapture = WindowFromPoint(pt);

            // If it's not a valid window, just return NULL
            
            if (!ghWndCapture)
            {
               SetCursor(hOldCursor);
               gbLButtonDown = gbNowCapturing = FALSE;
               break;
            }

            PostMessage(hWnd, WM_DOCAPTURE, wCaptureCommand, 0L);
            break;

        }

        case WM_MOUSEMOVE:
        {
            POINT       pt;
            HWND        hWndCurrent;

            if (!(gbNowCapturing && gbLButtonDown))
                break;

            if (gbCaptRect)
                break;

            // get the current mouse position

            GetCursorPos(&pt);
            
            // Get Window that we clicked on
             
            hWndCurrent = WindowFromPoint(pt);

            // if the cursor is still moving in the current window, no need
            // to highlight it again

            if (hWndCurrent != hBrowseWnd)
            {
                FrameWindow(hBrowseWnd);                
                FrameWindow(hWndCurrent);
                hBrowseWnd = hWndCurrent;
                if (hWndCurrent)
                    UpdateWindow(hWndCurrent);
            }

            break;
        }

        case WM_CLOSE:
        {
            WORD        wRet;

            if (gbSave)
            {
                LoadString(ghInst, IDS_SAVEBMP, lpBuffer, sizeof(lpBuffer));
                wRet = MessageBox(hWnd, lpBuffer,
                        szAppName, MB_ICONEXCLAMATION | MB_YESNOCANCEL);

                if (wRet == IDYES)
                    SaveMe();
                else if( wRet == IDCANCEL)
                    break;
            }

            if (IsWindow(hSelectDlg))
                DestroyWindow(hSelectDlg);
            if (IsWindow(hRectangleDlg))
                DestroyWindow(hRectangleDlg);

            return DefWindowProc(hWnd, wMessage, wParam, lParam);
        }

        case WM_DESTROY:  // Clean up
            InstallHook(hWnd, FALSE);  // Remove keyboard hook
            DeleteObject(ghbmLogo);

            if (ghBitmap)
                DeleteObject(ghBitmap);
            if (ghPal)
                DeleteObject(ghPal);


            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, wMessage, wParam, lParam);
    }
    return 0L;
}


//**********************************************************************
//
// FullViewWndProc()
//
// This is our full-screen popup window procedure. It is used to display an
// image using the entire screen. Clicking the left mouse button restores
// to the main app window.
//
//
//*********************************************************************


long APIENTRY FullViewWndProc(HWND hWnd, UINT wMessage, WPARAM wParam,
        LPARAM  lParam)
{
    switch (wMessage)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT     ps;
            HDC             hMemDC;
            BITMAP          bm;
            HBITMAP         hOldBm;
            RECT            rect, rectClient;
            int             x, y;

            BeginPaint(hWnd, &ps);

            // Check to see if we are displaying a bitmap

            if (!ghBitmap)
            {

                // No bitmap yet, are we in start mode?

                if (bStartup)
                {
                    GetClientRect(hWnd, &rectClient);

                    hMemDC = CreateCompatibleDC(ps.hdc);

                    // Select our logo bitmap

                    hOldBm = SelectObject(hMemDC, ghbmLogo);
                    GetObject(ghbmLogo, sizeof(BITMAP), (VOID *)&bm);

                    x = (rectClient.right - bm.bmWidth) / 2;
                    y = (rectClient.bottom - bm.bmHeight) / 2;

                    // Now bitblt our logo to client area
                    BitBlt(ps.hdc, x, y, bm.bmWidth, bm.bmHeight,
                          hMemDC, 0, 0, SRCCOPY);

                    // Clean up
                    SelectObject(hMemDC,hOldBm);
                    DeleteDC(hMemDC);
                }
            }
            else
            {
                // Get info for captured bitmap

                GetObject(ghBitmap, sizeof(BITMAP), (LPSTR)&bm);

                // Fill in src/dest rectangle with width and height
                // of captured bitmap
                
                rect.left = 0;
                rect.top = 0;
                rect.right = bm.bmWidth;
                rect.bottom = bm.bmHeight;

                // Paint the captured bitmap in the client area

                PaintBitmap(ps.hdc, &rect, ghBitmap, &rect, ghPal);
            }

            EndPaint(hWnd, &ps);
            break;
         }

        case WM_KEYDOWN:
        case WM_LBUTTONDOWN:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, wMessage, wParam, lParam);
    }
}


//**********************************************************************
//
// SaveMe()
//
// This procedure calls up the common dialog "File.Save" box, then 
// saves the current hBitmap as a DIB in the specified file, in the
// file format specified by the user in the dialog.
//
//*********************************************************************

void SaveMe()
{
    char    szFileBuf[255];     // Buffer to hold returned file name
    DWORD   dwFlags;            // used to pass in / get out file type
    HDC     hDC;                // HDC for getting info
    int     iBits;              // Bits per pixel of the display adapter
    DWORD   dCompression;       // Compression that the user specifies
    WORD    wBitCount;          // Bits per pixel that the user specifies
    WORD    wCompression;       // Compression
    HDIB    hDIB;               // A handle to our dib
    BOOL    bResult;            // Result of dialog box - TRUE if OK was pressed
    CHAR    lpBuffer[128];      // Buffer to hold string message retrieved from resources
    CHAR    lpBuffer2[128];     // Buffer to hold string message retrieved from resources

    // Set up default compression to display in dialog box

    wCompression = IDD_RGB;

    // Depending on bits/pixel type for current display,
    // set the appropriate bit in the fFileOptions flag
    //
    // NOTE that we don't just assign wBitCount to iBits.  The reason
    // for this is some displays aren't 1,4,8 or 24 bits.  Some are
    // 15 bits, which isn't valid for a DIB, so in this case, we would
    // set the bits to 24. 

    hDC = CreateDC("DISPLAY",NULL,NULL,NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);

    if (iBits <= 1)
        wBitCount = 1;

    else if (iBits <= 4)
        wBitCount = 4;

    else if (iBits <= 8)
        wBitCount = 8;

    else if (iBits <= 24)
        wBitCount = 24;

    // Our dwFlags parameter to GetFileName is made up of the
    // bits per pixel in the HIWORD (1, 2, 4, or 24), and the compression
    // type in the LOWORD (IDD_RGB, IDD_RLE4, or IDD_RLE8).

    dwFlags = MAKELONG(wCompression, wBitCount);


    // Bring up File/Save... dialog and get info. about filename,
    // compression, and bits/pix. of DIB to be written.

    bResult = GetFileName(ghWndMain, (LPSTR)szFileBuf, &dwFlags);

    // Extract DIB specs and save to file (if the user did not 
    // press cancel)

    if (bResult)
    {
        switch(LOWORD(dwFlags))
        {
            case IDD_RLE4:
                dCompression = BI_RLE4;
                break;

            case IDD_RLE8:
                dCompression = BI_RLE8;
                break;

            case IDD_RGB:
            default:
                dCompression = BI_RGB;
                break;
        }

        // First, call up a modeless dialog box which tells that we are
        // saving this to a file...

        if (!hModelessDlg)
        {
            hModelessDlg = CreateDialogParam(ghInst, (LPSTR)"Saving",
                    ghWndMain,SavingDlgProc, (DWORD)(LPSTR)szFileBuf);
        }

        // Now, write out the DIB in the proper format.  The following
        // API ChangeBitmapFormat() will convert the specified bitmap
        // to a DIB with the specified Bit Count, Compression and
        // Palette.  Remember that the HIWORD of dwFlags specifies the
        // bits per pixel.

        hDIB = ChangeBitmapFormat(ghBitmap, HIWORD(dwFlags), 
                dCompression, ghPal);

        if (hDIB)
        {
            if (SaveDIB(hDIB, szFileBuf))
            {
                LoadString(ghInst, IDS_CANTSAVE, lpBuffer, sizeof(lpBuffer));
                LoadString(ghInst, IDS_SAVEERROR, lpBuffer2, sizeof(lpBuffer2));
                MessageBox(NULL, lpBuffer, lpBuffer2, MB_ICONEXCLAMATION);
                gbSave = TRUE;
            }
            else
                gbSave = FALSE;

            DestroyDIB(hDIB);
        }

        DestroyWindow(hModelessDlg);
        hModelessDlg = NULL;
    }

}

//*********************************************************************
//
// PrintMe()
//
// This procedure calls up the "File.Print" dialog, then prints the
// current hBitmap as a DIB on the default printer.
//
//*********************************************************************

void PrintMe()
{
    static OPTIONSTRUCT opts;
    int                 iReturn;
    HDIB                hDIB;
    WORD                wReturn;

  
    // Display "Print Options" Box
   

    iReturn = DialogBoxParam(ghInst, (LPSTR)"Print", ghWndMain, PrintDlgProc,
            (LONG)(LPSTR)&opts);

    if (iReturn)
    {
        // User pressed "OK" -- do the printing

        hDIB = BitmapToDIB(ghBitmap, ghPal);

        if (hDIB)
        {

            // Print the dib using PrintDIB() API

            if (opts.iOption == IDC_STRETCHTOPAGE)
                wReturn = PrintDIB(hDIB, PW_STRETCHTOPAGE, 0, 0,
                        (LPSTR)gszWindowText);

            else if (opts.iOption == IDC_SCALE)
                wReturn = PrintDIB(hDIB, (WORD)PW_SCALE, (WORD)opts.iXScale,
                        (WORD)opts.iYScale, (LPSTR)gszWindowText);
            else
                wReturn = PrintDIB(hDIB, PW_BESTFIT, 0, 0,
                        (LPSTR)gszWindowText);

            if (wReturn)
                DIBError(wReturn);

            DestroyDIB(hDIB);
        }
    }
}



//*********************************************************************
//
// DoCapture()
//
// This procedure gets called when the user wants to capture the
// screen.  The wCommand parameter tells us which capture operation
// we want to perform.
//
//*********************************************************************


void DoCapture(HWND hWnd, WORD wCommand)
{
    HBITMAP     hBitmap;        // Handle to our temporary bitmap
    HPALETTE    hPal;           // Handle to our palette
    CHAR        lpBuffer[128];  // Buffer for string retrieved from resources


    switch (wCommand)
    {
        // Copy Entire screen to DIB

        case IDM_DESKTOP:
        {
            RECT rScreen;       // Rect containing entire screen coordinates
            HDC hDC;            // DC to screen
            MSG msg;            // Message for the PeekMessage()
            CHAR lpBuffer[128]; // Buffer for string retrieved from resources

            hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
            rScreen.left = rScreen.top = 0;
            rScreen.right = GetDeviceCaps(hDC, HORZRES);
            rScreen.bottom = GetDeviceCaps(hDC, VERTRES);

            // Delete our DC

            DeleteDC(hDC);

            LoadString(ghInst, IDS_ENTIRESCRN, lpBuffer, sizeof(lpBuffer));
            strcpy(gszWindowText, lpBuffer);

            // Wait until everybody repaints themselves

            while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }


            hBitmap = CopyScreenToBitmap(&rScreen);

            if (hBitmap)
                hPal = GetSystemPalette();
            else
                hPal = NULL;
        }
        break;


        // copy user-selected portion of screen to DIB

        case IDM_CAPTRECT:
        {
            RECT rRubberBand;       // Region to capture (screen coordinates)

            // Allow user to "rubberband" a section of the screen for
            // us to capture

            RubberBandScreen(&rRubberBand);
            LoadString(ghInst, IDS_USERSELECT, lpBuffer, sizeof(lpBuffer));
            strcpy(gszWindowText, lpBuffer);

            hBitmap = CopyScreenToBitmap(&rRubberBand);

            if (hBitmap)
                hPal = GetSystemPalette();
            else
                hPal = NULL;
        }
        break;



        case IDM_CAPTWINDOW:
        case IDM_CAPTCLIENT:
        case IDM_ACTIVEWINDOW:
        {
            HWND    hWndSelect;       // The current active window
            HWND    hWndDesktop;      // Window to desktop
            MSG     msg;              // For our peekmessage loop

            // Just capture the current active window, whatever it is

            if (wCommand == IDM_ACTIVEWINDOW)
                hWndSelect = GetForegroundWindow();

            // Allow the user to click on a single window to capture

            else    
                hWndSelect = ghWndCapture;

            // If they try to capture the desktop window, then just
            // capture the entire screen.

            hWndDesktop = GetDesktopWindow();

            if (hWndSelect == hWndDesktop)
            {
                RECT    rScreen;    // contains entire screen coordinates
                HDC     hDC;        // DC to screen
                MSG     msg;        // Message for the PeekMessage()

                hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
                rScreen.left = rScreen.top = 0;
                rScreen.right = GetDeviceCaps(hDC, HORZRES);
                rScreen.bottom = GetDeviceCaps(hDC, VERTRES);

                /* Delete our DC */
                DeleteDC(hDC);

                LoadString(ghInst, IDS_ENTIRESCRN, lpBuffer, sizeof(lpBuffer));
                strcpy(gszWindowText, lpBuffer);

                // Wait until everybody repaints themselves

                while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                ghBitmap = CopyScreenToBitmap(&rScreen);

                if (ghBitmap)
                    ghPal = GetSystemPalette();
                else
                    ghPal = NULL;

                ghWndCapture = NULL;
                return;
            }

            // Check to see that the hWnd is not NULL

            if (!hWndSelect)
            {
                LoadString(ghInst, IDS_CANTCAPWNDW, lpBuffer, sizeof(lpBuffer));
                MessageBox(NULL, lpBuffer, szAppName,
                        MB_ICONEXCLAMATION | MB_OK);

                hBitmap = NULL;
                hPal = NULL;
                break;
            }

            // Make sure it's not a hidden window.  Hmm, capturing a hidden
            // window would certainly be a cool trick, wouldn't it?

            if (!IsWindowVisible(hWndSelect))
            {
                LoadString(ghInst, IDS_WNDWNOTVIS, lpBuffer, sizeof(lpBuffer));
                MessageBox(NULL, lpBuffer,
                        szAppName, MB_ICONEXCLAMATION | MB_OK);

                ghWndCapture = NULL;
                hBitmap = NULL;
                hPal = NULL;
                break;
            }

            // Move window which was selected to top of Z-order for
            // the capture, and make it redraw itself

            SetWindowPos(hWndSelect, NULL, 0, 0, 0, 0,
                    SWP_DRAWFRAME | SWP_NOSIZE | SWP_NOMOVE);

            UpdateWindow(hWndSelect);

            // Wait until everybody repaints themselves

            while (PeekMessage(&msg,NULL,0,0,PM_REMOVE) != 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            // Get the caption

            GetWindowText(hWndSelect, gszWindowText, 100);

            // Capture the screen -- if we selected CLIENT only capture, then
            // select that open when calling our API.  Otherwise, get the
            // entire window.

            hBitmap = CopyWindowToBitmap(hWndSelect,
                    (WORD)((wCommand == IDM_CAPTCLIENT) ? (PW_CLIENT) :
                    (PW_WINDOW)));

            if (hBitmap)
                hPal = GetSystemPalette();
            else
                hPal = NULL;

            break;
        }

        //Oops, something went wrong

        default:
            LoadString(ghInst, IDS_INTERROR, lpBuffer, sizeof(lpBuffer));
            MessageBox(NULL, lpBuffer, szAppName,
                    MB_ICONHAND | MB_OK);
            break;
    }

    if (hBitmap)
    {
        ghBitmap = hBitmap;
        hBitmap = NULL;
    }
    if (hPal)
    {
        ghPal = hPal;
        hPal = NULL;
    }

    // Now, paint our bitmap in the client area

    if (bStartup)
        bStartup = FALSE;

    InvalidateRect(hWnd, NULL, FALSE);
    UpdateWindow(hWnd);
    ghWndCapture = NULL;
}



//**********************************************************************
//
// RubberBandScreen()
//
// This function allows the user to rubber-band a portion of the screen.
// When the left button is released, the rect that the user selected
// (in screen coordinates) is returned in lpRect.
//
//*********************************************************************

void RubberBandScreen(LPRECT lpRect)
{
    POINT   pt;                 // Temporary POINT
    MSG     msg;                // Used in our PeekMessage() loop
    POINT   ptOrigin;           // Point where the user pressed left mouse button down
    RECT    rcClip;             // Current selection
    HDC     hScreenDC;          // DC to the screen (so we can draw on it)
    BOOL    bCapturing = FALSE; // TRUE if we are rubber-banding
    POINTS  pts;
    BOOL    bLButtonUp = FALSE;

    hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);

    // Eat mouse messages until a WM_LBUTTONUP is encountered. Meanwhile
    // continue to draw a rubberbanding rectangle and display it's dimensions

    for (;;)
    {
        WaitMessage();
        if (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
        {
            // If the message is a WM_LBUTTONDOWN, begin drawing the
            // rubber-band box.
          
            if (msg.message == WM_RBUTTONDOWN)
            {
                // User pressed left button, initialize selection
                // Set origin to current mouse position (in window coords)

                pts = MAKEPOINTS(msg.lParam);

                ptOrigin.x = pts.x;
                ptOrigin.y = pts.y;

                // Convert to screen coordinates
             
                ClientToScreen(ghWndMain, &ptOrigin);

            
                // rcClip is the current rectangle that the user
                // has selected.  Since user just pressed left button down,
                // initialize this rect very small
             
                rcClip.left = rcClip.right = ptOrigin.x;
                rcClip.top = rcClip.bottom = ptOrigin.y;
                NormalizeRect(&rcClip);     // Make sure it is a normal rect
                DrawSelect(hScreenDC, TRUE, &rcClip); // Draw the rubber-band box
                bCapturing = TRUE;
            }

            else if (msg.message == WM_LBUTTONUP)
            {
                if (!bCapturing)
                    break;
                else
                {
                    bLButtonUp = TRUE;
                    PostMessage(ghWndMain, WM_RBUTTONUP, msg.wParam, msg.lParam);
                }
            }

            // Any messages that make it into the next statement are mouse
            // messages, and we are capturing, so let's update the rubber-band
            // box
          
            if (bCapturing)
            {
                DrawSelect(hScreenDC, FALSE, &rcClip);  // erase old rubber-band
                rcClip.left = ptOrigin.x;   // Update rect with new mouse info
                rcClip.top = ptOrigin.y;

                pts = MAKEPOINTS(msg.lParam);

                pt.x = pts.x;
                pt.y = pts.y;

                // Convert to screen coordinates
                
                ClientToScreen(ghWndMain, &pt);
                rcClip.right = pt.x;
                rcClip.bottom = pt.y;
                NormalizeRect(&rcClip);
                DrawSelect(hScreenDC, TRUE, &rcClip); // new rubber-band
            }

            // If the message is WM_LBUTTONUP, then we stop the selection
            // process.

            if (msg.message == WM_RBUTTONUP)
            {
                DrawSelect(hScreenDC, FALSE, &rcClip);    // erase rubber-band
                if (bLButtonUp)
                    PostMessage(ghWndMain, WM_LBUTTONUP, msg.wParam, msg.lParam);
                break;
            }
        }
        else
            continue;
    }
    DeleteDC(hScreenDC);

    // Assign rect user selected to lpRect parameter
    
    if (!IsRectEmpty(&rcClip))
        CopyRect(lpRect, &rcClip);

    PostMessage(ghWndMain, WM_LBUTTONUP, msg.wParam, msg.lParam);
}


//***************************************************************************
//
// DrawSelect
//
// Draws the selected clip rectangle with its dimensions on the DC
//
//***************************************************************************


void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip)
{
    char sz[80];
    DWORD dw;
    int x, y, len, dx, dy;
    HDC hdcBits;
    HBITMAP hbm;
    RECT rcClip;
    SIZE sExtent;

    rcClip = *lprClip;
    if (!IsRectEmpty(&rcClip))
    {

        // If a rectangular clip region has been selected, draw it

        PatBlt(hdc, rcClip.left, rcClip.top, rcClip.right - rcClip.left, 1,
                DSTINVERT);
        PatBlt(hdc, rcClip.left, rcClip.bottom, 1, -(rcClip.bottom-rcClip.top),
                DSTINVERT);
        PatBlt(hdc, rcClip.right - 1, rcClip.top, 1, rcClip.bottom - rcClip.top,
                DSTINVERT);
        PatBlt(hdc, rcClip.right, rcClip.bottom - 1, -(rcClip.right -
                rcClip.left), 1, DSTINVERT);

        // Format the dimensions string ...

        wsprintf(sz, "%dx%d", rcClip.right - rcClip.left, rcClip.bottom -
               rcClip.top);
        len = lstrlen(sz);

        // ... and center it in the rectangle

        dw = GetTextExtentPoint(hdc, sz, len, &sExtent);
        dx = sExtent.cx;
        dy = sExtent.cy;
        x = (rcClip.right + rcClip.left - dx) / 2;
        y = (rcClip.bottom + rcClip.top - dy) / 2;
        hdcBits = CreateCompatibleDC(hdc);
        SetTextColor(hdcBits, 0xFFFFFFL);
        SetBkColor(hdcBits, 0x000000L);

        // Output the text to the DC

        if (hbm = CreateBitmap(dx, dy, 1, 1, NULL))
        {
            hbm = SelectObject(hdcBits, hbm);
            ExtTextOut(hdcBits, 0, 0, 0, NULL, sz, len, NULL);
            BitBlt(hdc, x, y, dx, dy, hdcBits, 0, 0, SRCINVERT);
            hbm = SelectObject(hdcBits, hbm);
            DeleteObject(hbm);
        }
        DeleteDC(hdcBits);
    }
}


//*************************************************************************
//
//  FUNCTION   : NormalizeRect(RECT *prc)
//
//  PURPOSE    : If the rectangle coordinates are reversed, swaps them.
//               This is used to make sure that the first coordinate of
//               our rect is the upper left, and the second is lower right.
//
//*************************************************************************

void WINAPI NormalizeRect(LPRECT prc)
{
    if (prc->right  < prc->left) SWAP(prc->right,  prc->left);
    if (prc->bottom < prc->top)  SWAP(prc->bottom, prc->top);
}


//***************************************************************************
//
// Function: DoSize
//
// Purpose:  Called by WndProc() on WM_SIZE
//
//           When the window is sized -- set up the scroll bars.
//
//           The window will be repainted if the new size, combined
//           with the current scroll bar positions would create blank
//           space at the left or bottom of the window.
//
//***************************************************************************

void DoSize(HWND hWnd)
{
    BITMAP      bm;                     // Bitmap info structure
    int         cxBitmap=0, cyBitmap=0; // Bitmap width and height
    int         cxScroll, cyScroll;     // Scroll positions
    RECT        rect;                   // Client rectangle

    // repaint if displaying bitmap

    if (ghBitmap)
    {
        // Get info about bitmap

        GetObject(ghBitmap, sizeof(BITMAP), (LPSTR)&bm);

        // Get the width and height of the bitmap

        cxBitmap = bm.bmWidth;
        cyBitmap = bm.bmHeight;

        // Find out the dimensions of the window, and the current thumb
        // positions

        GetClientRect(hWnd, &rect);

        cxScroll = GetScrollPos (hWnd, SB_HORZ);
        cyScroll = GetScrollPos (hWnd, SB_VERT);

        // If current thumb positions would cause blank space
        // at right or bottom of client area, repaint

        if (cxScroll + rect.right > cxBitmap ||
                cyScroll + rect.bottom > cyBitmap)
            InvalidateRect(hWnd, NULL, FALSE);

        // Make sure scroll bars are updated

        SetupScrollBars(hWnd, (WORD)cxBitmap, (WORD)cyBitmap);
    }
    else if (bStartup)
        InvalidateRect(hWnd, NULL, TRUE);
}


//***************************************************************************
//
// Function: ReallyGetClientRect
//
// Purpose:  Gets the rectangular area of the client rect including
//           the area underneath visible scroll bars.  Stolen from
//           ShowDIB.
//
//***************************************************************************

void ReallyGetClientRect(HWND hWnd, LPRECT lpRect)
{
    DWORD   dwWinStyle;

    dwWinStyle = GetWindowLong (hWnd, GWL_STYLE);

    GetClientRect (hWnd, lpRect);

    if (dwWinStyle & WS_HSCROLL)
        lpRect->bottom += (GetSystemMetrics (SM_CYHSCROLL) - 1);

    if (dwWinStyle & WS_VSCROLL)
        lpRect->right  += (GetSystemMetrics (SM_CXVSCROLL) - 1);
}


//***************************************************************************
//
// Function: SetupScrollBars
//
// Purpose:  Sets up scroll bars.
//
//***************************************************************************

void SetupScrollBars(HWND hWnd, WORD cxBitmap, WORD cyBitmap)
{
    RECT        rect;                       // Client Rectangle
    BOOL        bNeedScrollBars=FALSE;      // Need Scroll bars?
    unsigned    cxWindow, cyWindow;         // Width and height of client area
    int         cxRange=0, cyRange=0;       // Range needed for horz and vert

    // Do some initialization

    ReallyGetClientRect(hWnd, &rect);

    cxWindow = rect.right - rect.left;
    cyWindow = rect.bottom - rect.top;

    // Now determine if we need the scroll bars

    if ((cxWindow < (unsigned)cxBitmap) || (cyWindow < (unsigned)cyBitmap))
        bNeedScrollBars = TRUE;


    // Setup the scroll bar ranges.  We want to be able to
    // scroll the window so that all the bitmap can appear
    // within the client area.  Take into account that
    // if the opposite scroll bar is activated, it eats
    // up some client area.

    if (bNeedScrollBars)
    {
        cyRange = (unsigned)cyBitmap - cyWindow - 1 +
                GetSystemMetrics (SM_CYHSCROLL);
        cxRange = (unsigned)cxBitmap - cxWindow - 1 +
                GetSystemMetrics (SM_CXVSCROLL);
    }

    // Set the ranges we've calculated (0 to 0 means invisible scrollbar)

    SetScrollRange(hWnd, SB_VERT, 0, cyRange, TRUE);
    SetScrollRange(hWnd, SB_HORZ, 0, cxRange, TRUE);
}


//**********************************************************************
//
// Function:   DoScroll()
//
// Purpose:    Called by ChildWndProc() on WM_HSCROLL and WM_VSCROLL.
//             Window needs to be scrolled (user has clicked on one
//             of the scroll bars.
//
//             Does scrolling in both horiziontal and vertical directions.
//             Note that the variables are all named as if we were
//             doing a horizontal scroll.  However, if we're doing a
//             vertical scroll, they are initialized to the appropriate
//             values for a vertical scroll.
//
//             If we scroll by one (i.e. user clicks on one of the
//             scrolling arrows), we scroll the window by 1/SCROLL_RATIO
//             of the client area.  In other words, if SCROLL_RATION==4,
//             then we move the client area over a 1/4 of the width/height
//             of the screen.
//
//             If the user is paging up/down we move a full client area's
//             worth.
//
//             If the user moves the thumb to an absolute position, we
//             just move there.
//
//             ScrollWindow/re-painting do the actual work of scrolling.
//
//**********************************************************************

void DoScroll(HWND hWnd, int message, int wPos, int wScrollType)
{
    int  xBar;          // Where scrollbar is now.
    int  nMin;          // Minumum scroll bar value.
    int  nMax;          // Maximum scroll bar value.
    int  dx;            // How much to move.
    int  nOneUnit;      // # of pixels for LINEUP/LINEDOWN
    int  cxClient;      // Width of client area.
    int  nHorzOrVert;   // Doing the horizontal or vertical?
    RECT rect;          // Client area.

    GetClientRect (hWnd, &rect);

    if (message == WM_HSCROLL)
    {
        nHorzOrVert = SB_HORZ;
        cxClient    = rect.right - rect.left;
    }
    else
    {
        nHorzOrVert = SB_VERT;
        cxClient    = rect.bottom - rect.top;
    }

    // On a SB_LINEUP/SB_LINEDOWN we will move the DIB by
    //  1/SCROLL_RATIO of the client area (i.e. if SCROLL_RATIO
    //  is 4, it will scroll the DIB a quarter of the client
    //  area's height or width.

    nOneUnit = cxClient / SCROLL_RATIO;
    if (!nOneUnit)
        nOneUnit = 1;

    xBar = GetScrollPos (hWnd, nHorzOrVert);
    GetScrollRange (hWnd, nHorzOrVert, &nMin, &nMax);

    switch (wScrollType)
    {
        case SB_LINEDOWN:             // One line right.
            dx = nOneUnit;
            break;

        case SB_LINEUP:               // One line left.
            dx = -nOneUnit;
            break;

        case SB_PAGEDOWN:             // One page right.
            dx = cxClient;
            break;

        case SB_PAGEUP:               // One page left.
            dx = -cxClient;
            break;

        case SB_THUMBPOSITION:        // Absolute position.
            dx = wPos - xBar;
            break;

        default:                      // No change.
            dx = 0;
            break;
    }

    if (dx)
    {
        xBar += dx;

        if (xBar < nMin)
        {
            dx  -= xBar - nMin;
            xBar = nMin;
        }

        if (xBar > nMax)
        {
            dx  -= xBar - nMax;
            xBar = nMax;
        }

        if (dx)
        {
            SetScrollPos (hWnd, nHorzOrVert, xBar, TRUE);

            if (nHorzOrVert == SB_HORZ)
                ScrollWindow (hWnd, -dx, 0, NULL, NULL);
            else
                ScrollWindow (hWnd, 0, -dx, NULL, NULL);

            UpdateWindow (hWnd);
        }
    }
}

//****************************************************************************
//
// Function: DoPaint()
//
// Purpose:  Called by WndProc. Does painting for client area.
//
//
//***************************************************************************

void DoPaint(HWND hWnd)
{
    HDC             hDC, hMemDC;         // Handle to DC, memory DC
    PAINTSTRUCT     ps;                  // Painting structure
    BITMAP          bm;                  // BITMAP structure
    HBITMAP         hOldBm;              // Handle to previous bitmap
    RECT            rectClient, rectDDB; // Client and bitmap rectangles
    int             xScroll, yScroll;    // Scroll positions
    int             x, y;                // Logo origin


    // Begin painting

    hDC = BeginPaint(hWnd, &ps);

    // Check to see if we are displaying a bitmap
    
    if (!ghBitmap)
    {
        // No bitmap yet, are we in start mode?

        if (bStartup)
        {
            GetClientRect(hWnd, &rectClient);

            hMemDC = CreateCompatibleDC(ps.hdc);

            // Select our logo bitmap

            hOldBm = SelectObject(hMemDC, ghbmLogo);

            GetObject(ghbmLogo, sizeof(BITMAP), (VOID *)&bm);

            x = (rectClient.right - bm.bmWidth) / 2;
            y = (rectClient.bottom - bm.bmHeight) / 2;

            // Now bitblt our logo to client area

            BitBlt(ps.hdc, x, y, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0,
                    SRCCOPY);

            // Clean up
            SelectObject(hMemDC,hOldBm);
            DeleteDC(hMemDC);
        }
        else
        {
            // Turn off scroll bars in case they were on

                SetScrollRange (hWnd, SB_VERT, 0, 0, TRUE);
            SetScrollRange (hWnd, SB_HORZ, 0, 0, TRUE);
        }
    }
    else // We are displaying a bitmap
    {
        // Get bitmap info

        GetObject(ghBitmap, sizeof(BITMAP), (LPSTR)&bm);

        // Get scroll bar positions

        xScroll  = GetScrollPos  (hWnd, SB_HORZ);
        yScroll  = GetScrollPos  (hWnd, SB_VERT);

        // Set up the scroll bars appropriately.

        SetupScrollBars(hWnd, (WORD)bm.bmWidth, (WORD)bm.bmHeight);

        // Set up the necessary rectangles -- i.e. the rectangle
        //  we're rendering into, and the rectangle in the bitmap

        GetClientRect (hWnd, &rectClient);

        rectDDB.left   = xScroll;
        rectDDB.top    = yScroll;
        rectDDB.right  = xScroll + rectClient.right - rectClient.left;
        rectDDB.bottom = yScroll + rectClient.bottom - rectClient.top;

        if (rectDDB.right > bm.bmWidth)
        {
            int dx;

            dx = bm.bmWidth - rectDDB.right;

            rectDDB.right     += dx;
            rectClient.right  += dx;
        }

        if (rectDDB.bottom > bm.bmHeight)
        {
            int dy;

            dy = bm.bmHeight - rectDDB.bottom;

            rectDDB.bottom    += dy;
            rectClient.bottom += dy;
        }

        // Go do the actual painting.

        PaintBitmap(hDC, &rectClient, ghBitmap, &rectDDB, ghPal);
    }

    EndPaint(hWnd, &ps);
}


//****************************************************************************
//
// Function: FrameWindow()
//
// Purpose:  Highlight the window frame
//
//
//***************************************************************************

void FrameWindow(HWND hWnd)
{
    HDC     hdc;
    RECT    rc;

#define DINV    3
    
    if (!IsWindow(hWnd))
        return;

    hdc = GetWindowDC(hWnd);
    GetWindowRect(hWnd, &rc);
    OffsetRect(&rc, -rc.left, -rc.top);

    if (!IsRectEmpty(&rc))
    {
        PatBlt(hdc, rc.left, rc.top, rc.right-rc.left, DINV, DSTINVERT);
        PatBlt(hdc, rc.left, rc.bottom-DINV, DINV, -(rc.bottom-rc.top-2*DINV),
                DSTINVERT);
        PatBlt(hdc, rc.right-DINV, rc.top+DINV, DINV, rc.bottom-rc.top-2*DINV,
                DSTINVERT);
        PatBlt(hdc, rc.right, rc.bottom-DINV, -(rc.right-rc.left), DINV,
                DSTINVERT);
    }

    ReleaseDC(hWnd, hdc);
}

//****************************************************************************
//
// FUNCTION: CenterWindow (HWND, HWND)
//
// PURPOSE:  Center one window over another
//
// COMMENTS:
//
//      Dialog boxes take on the screen position that they were designed at,
//      which is not always appropriate. Centering the dialog over a particular
//      window usually results in a better position.
//
//***************************************************************************

BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
    RECT    rChild, rParent;
    int     wChild, hChild, wParent, hParent;
    int     wScreen, hScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect (hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect (hwndParent, &rParent);
    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;

    // Get the display limits
    hdc = GetDC (hwndChild);
    wScreen = GetDeviceCaps (hdc, HORZRES);
    hScreen = GetDeviceCaps (hdc, VERTRES);
    ReleaseDC (hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rParent.left + ((wParent - wChild) /2);
    if (xNew < 0)
        xNew = 0;
    else if ((xNew+wChild) > wScreen)
        xNew = wScreen - wChild;

    // Calculate new Y position, then adjust for screen
    yNew = rParent.top  + ((hParent - hChild) /2);
    if (yNew < 0)
        yNew = 0;
    else if ((yNew+hChild) > hScreen)
        yNew = hScreen - hChild;

    // Set it, and return
    return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER);
}

