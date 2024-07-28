//**************************************************************************
//
//  APPLICATION: 
//      DragDrop.exe                                               
//
//
//  Author:
//      Microsoft Product Support Services.
//
//
//  PURPOSE:                                                                
//      This sample uses the Windows 3.1 Drag and Drop interface.  It 
//      displays  the name of the files, in a list box, that are dragged 
//      from FileManager on this app.                                                            
//
//
//  FUNCTIONS:                                                              
//
//  WinMain()     - Creates the main window and its child list box.         
//                - Registers the main window to accept Drag/Drop messages. 
//                - Enters the message loop.                                
// 
//  MainWndProc() - Processes main window's messages.                       
//                - Also handles Drag/Drop messages.                        
// 
//  About()       - Processes the About dialog's messages.                  
// 
//  Info()        - Processes the Info dialog's messages.                   
//                - Displays info on the files dropped.                     
//
//
//  Copyright (c) 1992 Microsoft Corporation. All rights reserved.
//
//**************************************************************************

#include <windows.h>
#include "shellapi.h"   // Contains Drag/Drop APIs
#include "dragdrop.h"

HANDLE  ghInst;
HWND    ghListBox;
POINT   gpointDrop = {0,0};  // Point where the files were dropped
WORD    gwFilesDropped = 0;  // Total number of files dropped

//**************************************************************************
//
//  FUNCTION: WinMain()                                                     
//
//  PURPOSE:                                                                
//                                                                           
//  This function creates the main window and the list box, registers the   
//  main window to accept Drag/Drop messages, and goes in the message loop. 
//
//**************************************************************************

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpCmdLine,  int nCmdShow)
{
    MSG         msg;
    WNDCLASS    wc;
    HWND        hMainWnd;
    RECT        rectMain;

    if (!hPrevInstance)
    {
        wc.style = NULL;
        wc.lpfnWndProc = MainWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon (hInstance, "DragDrop");
        wc.hCursor = LoadCursor (NULL, IDC_ARROW);
        wc.hbrBackground = GetStockObject (WHITE_BRUSH);
        wc.lpszMenuName =  "DragDropMenu";
        wc.lpszClassName = "DragDropWClass";

        if (!RegisterClass (&wc))
            return (FALSE);
    }

    hMainWnd = CreateWindow ("DragDropWClass",
                            "Drag and Drop Client Application",
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
                            | WS_MINIMIZEBOX,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            MAIN_WIDTH,
                            MAIN_HEIGHT,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    if (!hMainWnd)
        return (FALSE);

    GetClientRect (hMainWnd, &rectMain);

    ghListBox = CreateWindow ("ListBox",
                        NULL,
                        WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT
                        | LBS_SORT | WS_VISIBLE,
                        0,
                        0,
                        rectMain.right - rectMain.left,
                        rectMain.bottom - rectMain.top,
                        hMainWnd,
                        1,
                        hInstance,
                        NULL);

    if (!ghListBox)
        return (FALSE);

    // Register the the main window for Drag/Drop messages.
    DragAcceptFiles (hMainWnd, TRUE);

    ShowWindow (hMainWnd, nCmdShow);
    UpdateWindow (hMainWnd);

    ghInst = hInstance;

    while (GetMessage (&msg,
                        NULL,
                        NULL,
                        NULL))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return (msg.wParam);
} // WinMain()


//**************************************************************************
//
//  FUNCTION: MainWndProc()                                                     
//
//  PURPOSE:                                                                
//                                                                           
//  This function handles messages belonging to the main window.
//  It also handles and processes Drag/Drop messages.
//
//**************************************************************************

long FAR PASCAL MainWndProc (HWND hWnd,   UINT message,
			     WPARAM wParam, LPARAM lParam)
{
    FARPROC lpDialogProc;
    HANDLE  hFilesInfo;
    WORD    wIndex;
    char    szFileName [FILE_NAME_LENGTH];

    switch (message)
    {
        case WM_DROPFILES:
            hFilesInfo = (HANDLE) wParam;

            // Retrieve the window coordinates of the mouse
            // pointer when the drop was made
            DragQueryPoint ((HANDLE) wParam, (LPPOINT) &gpointDrop);

            // Get the total number of files dropped
            gwFilesDropped = DragQueryFile (hFilesInfo,
					   (UINT)-1,
                                           NULL,
                                           0);

            // Retrieve each file name and add to the list box
            for (wIndex=0; wIndex < gwFilesDropped; wIndex++)
            {
                DragQueryFile (hFilesInfo,
                               wIndex,
                               (LPSTR) szFileName,
                               FILE_NAME_LENGTH);

                SendMessage (ghListBox,
                             LB_ADDSTRING,
                             0,
                             (LONG) (LPSTR) szFileName);
            } // for 

            DragFinish (hFilesInfo);
            break;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_ABOUT:
                    lpDialogProc = MakeProcInstance (About, ghInst);
                    DialogBox (ghInst,
                               "AboutBox",
                               hWnd,
                               lpDialogProc);
                    FreeProcInstance (lpDialogProc);
                    break;

                case IDM_INFO:
                    if (gwFilesDropped)
                    {
                        lpDialogProc = MakeProcInstance (Info, ghInst);
                        DialogBox (ghInst,
                                "InfoBox",
                                hWnd,
                                lpDialogProc);
                        FreeProcInstance (lpDialogProc);
                    }
                    else
                        MessageBox (hWnd, "Files have not been dropped yet.",
                                    "DragDrop", MB_OK | MB_ICONASTERISK);
                    break;

                default:
                    return (DefWindowProc (hWnd, message, wParam, lParam));
            }  // switch (wParam) 
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            return (DefWindowProc (hWnd, message, wParam, lParam));

    } // switch (message)

    return (0);

} // MainWndProc()


//**************************************************************************
//
//  FUNCTION: About()                                                     
//
//  PURPOSE:                                                                
//                                                                           
//  This function handles messages belonging to the "About" dialog box.
//  The only message that it looks for is WM_COMMAND, indicating that the
//  user has pressed the "OK" button.  When this happens, it takes down
//  the dialog box.
//  
//**************************************************************************

BOOL FAR PASCAL About (HWND hDlg,   unsigned message,
                       WORD wParam, LONG lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog (hDlg, TRUE);
                    return (TRUE);

                default:
                    break;
            } // switch (wParam)
    } // switch (message)

    return (FALSE);

} // About()


//**************************************************************************
//
//  FUNCTION: Info()                                                     
//
//  PURPOSE:                                                                
//                                                                           
//  This function handles messages belonging to the "Info" dialog box.
//  It displays information on the files dropped.  The only message that
//  it looks for is WM_COMMAND, indicating that the user has pressed the
//  "OK" button.  When this happens, it takes down the dialog box.
//  
//**************************************************************************

BOOL FAR PASCAL Info (HWND hDlg,   unsigned message,
                      WORD wParam, LONG lParam)
{
    char    szTemp [STRING_LEN];

    switch (message)
    {
        case WM_INITDIALOG:
            wsprintf (szTemp, "%d", gwFilesDropped);
            SetDlgItemText (hDlg, IDD_NUMFILES, (LPSTR) szTemp);
            
            wsprintf (szTemp, "%d", gpointDrop.x);
            SetDlgItemText (hDlg, IDD_XCORD, (LPSTR) szTemp);

            wsprintf (szTemp, "%d", gpointDrop.y);
            SetDlgItemText (hDlg, IDD_YCORD, (LPSTR) szTemp);

            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog (hDlg, TRUE);
                    return (TRUE);

                default:
                    break;
            } // switch (wParam)
    } // switch (message)

    return (FALSE);

} // Info()



