//***************************************************************************
//
//  Library:
//      XTENSION.DLL
//
//
//  Author:
//      Microsoft Product Support Services.
//
//
//  Purpose:
//      XTENSION is a File Manager extension DLL.  An extension DLL adds
//      a menu to File Manager, contains entry point that processes menu
//      commands and notification messages sent by File Manager, and
//      queries data and information about the File Manager windows.  The 
//      purpose of an extension DLL is to add administration support 
//      features to File Manager, for example, file and disk utilities.
//      Up to five extension DLLs may be instaled at any one time.
//
//      XTENSION adds a menu called "Extension" to File manager and 
//      processes all the messages that are sent by File Manager to an 
//      extension DLL.  In order to retrieve any information, it sends 
//      messages to File Manager.  It also creates a topmost status window 
//      using the DLL's instance handle.
//
//
//  Usage:
//      File Manager installs the extensions that have entries in the 
//      [AddOns] section of the WINFILE.INI initialization file.  An entry 
//      consists of a tag and a value.  To load XTENSION.DLL as a File 
//      Manager extension, add the following to WINFILE.INI (assuming the 
//      DLL resides in c:\win\system):
//
//      [AddOns]
//      SDK Demo Extension=c:\win\system\xtension.dll
//
//
//  Menu Options:
//      Following menu items belong to the "Extension" menu that is added
//      to File Manager:
//
//      Status Window               - Shows/Hides status window
//      Selected File(s) Size...    - Displays disk space taken by the files
//      Selected Drive Info...      - Displays selected drive information
//      Focused Item Info           - Displays the name of the focused item
//      Reload Extension            - Reloads this extension
//      Refresh Window              - Refreshes File Manager's active window
//      Refresh All Windows         - Refreshes all the File Manager's windows
//      About Extension...          - Displays About dialog
//
//
//  More Info:
//      Query on-line help on: FMExtensionProc, File Manager Extensions
//
//
//  Copyright (c) 1992 Microsoft Corporation. All rights reserved.
//
//***************************************************************************

#include <windows.h>
#include "wfext.h"
#include "xtension.h"

char    gszDllWndClass[] = "ExtenStatusWClass"; // Class name for status window
HWND    ghwndStatus = 0;                        // Status window
HWND    ghwndInfo;                              // Child window of status window
HANDLE  ghDllInst;                              // DLL's instance handle
HMENU   ghMenu;                                 // Extension's menu handle
WORD    gwMenuDelta;                            // Delta for extension's menu items
BOOL    gbStatusWinVisible = FALSE;             // Flag for status window
                                                   // FALSE=Hidden,  TRUE=Visible

//***************************************************************************
//
//  LibMain()
//
//  Purpose:
//
//      LibMain is called by LibEntry.  LibEntry is called by Windows
//      when the DLL is loaded.  The LibEntry routine is provided
//      in the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
//      source LIBENTRY.ASM is also provided.)
//
//      LibEntry initializes the DLL's heap if a HEAPSIZE value is
//      specified in the DLL's DEF file.  After this, LibEntry calls
//      LibMain.  The LibMain function below satisfies that call.
//
//      The LibMain function should perform additional initialization
//      tasks required by the DLL.  In this example, no initialization
//      tasks are required; only the DLL's instance handle is saved.  
//      LibMain should return a value of TRUE if the initialization is 
//      successful.
//
//
//  Parameters:
//
//      IN:
//          hLibInst    - DLLs instance handle
//          wDataSeg    - Data segment
//          cbHeapSize  - Size of the DLL's heap
//          lpszCmdLine - Command line
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      TRUE if the initialization is successful; FALSE otherwise.
//
//***************************************************************************

int FAR PASCAL LibMain (HANDLE hLibInst,  WORD wDataSeg,
                       WORD cbHeapSize, LPSTR lpszCmdLine)
{
    ghDllInst = hLibInst;
    return (1);
} // LibMain()

//***************************************************************************
//
//  WEP()
//
//  Purpose:
//
//      Performs cleanup tasks when the .DLL is unloaded.  The WEP() is
//      called automatically by Windows when the DLL is unloaded.
//      
//      Make sure that the WEP() is @1 RESIDENTNAME in the EXPORTS
//      section of the .DEF file.  This ensures that the WEP() can
//      be called as quickly as possible.  Incidently, this is why
//      the WEP() is called the WEP() instead of WindowsExitProcedure().
//      It takes up the minimum amount of space and is quickly located.
//
//
//  Parameters:
//
//      IN:
//          bSystemExit - Type of exit
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      TRUE.
//
//***************************************************************************

int FAR PASCAL WEP (int bSystemExit)
{
    return (1);
} // WEP()


//***************************************************************************
//
//  FMExtensionProc()
//
//  Purpose:
//
//      This is an application-defined callback function.  It processes menu 
//      commands and messages sent to XTENSION.DLL.
//
//
//  Parameters:
//
//      IN:
//          hwndExtension   - Identifies the File Manager window
//          wMessage        - Message sent to extension DLL
//          lParam          - Message information
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      When the wMessage is FMEVENT_INITMENU, handle to extension's menu
//      should be returned; otherwise a NULL value.
//
//***************************************************************************

HMENU FAR PASCAL FMExtensionProc (HWND hwndExtension, WORD wMessage,
                                  LONG lParam)
{
    LPFMS_LOAD  lpload;
    FARPROC     lpDialogProc;
    WORD        wFocusedItem;

    switch (wMessage)
    {

    // ****************** File Manager Events

        case FMEVENT_INITMENU:
            DisplayStatus (hwndExtension, wMessage);

            break;

        case FMEVENT_LOAD:

            // Create status window

            if (!ghwndStatus)
            {
                if (!CreateStatusWindow (hwndExtension))
                {
                    MessageBox (hwndExtension,
                                "Extension not loaded.  Status window creation error.",
                                "File Manager Extension",
                                MB_OK | MB_ICONASTERISK);

                    // Unload
                    break;
                }
            }

    		lpload	= (LPFMS_LOAD) lParam;

		    // Assign the menu handle from the DLL's resource

            ghMenu = LoadMenu (ghDllInst, "ExtensionMenu");

            lpload->hMenu = ghMenu;

		    // This is the delta we are being assigned.

		    gwMenuDelta = lpload->wMenuDelta;

            // Size of the load structure

		    lpload->dwSize = sizeof (FMS_LOAD);
		
		    // Assign the popup menu name for this extension

		    lstrcpy (lpload->szMenuName, "&Extension");

            MessageBox (hwndExtension, "File Manager Extension will be loaded.",
                        "File Manager Extension", MB_OK);

            // Return that handle

		    return (ghMenu);


        case FMEVENT_SELCHANGE:
            DisplayStatus (hwndExtension, wMessage);

            break;

        case FMEVENT_UNLOAD:
            DisplayStatus (hwndExtension, wMessage);
            MessageBox (hwndExtension, "File Manager Extension will be unloaded.",
                        "File Manager Extension", MB_OK);

            // Since the status window was created using DLL's 
            // instance handle, we will have to destroy it on our own.

            DestroyWindow (ghwndStatus);

            break;

        case FMEVENT_USER_REFRESH:
            DisplayStatus (hwndExtension, wMessage);

            break;


    // ****************** Extension menu commands

        case IDM_STATUSWIN:
            if (GetMenuState (ghMenu, gwMenuDelta + wMessage,
                              MF_BYCOMMAND) & MF_CHECKED)
            {
                gbStatusWinVisible = FALSE;

                // Hide the status window

                ShowWindow (ghwndStatus, SW_HIDE);

                // Remove the checkmark 

                CheckMenuItem (ghMenu, gwMenuDelta+IDM_STATUSWIN,
                               MF_UNCHECKED | MF_BYCOMMAND);

            }
            else
            {
                gbStatusWinVisible = TRUE;

                // Show the status window

                ShowWindow (ghwndStatus, SW_SHOW);

                // Add the checkmark 

                CheckMenuItem (ghMenu, gwMenuDelta+IDM_STATUSWIN,
                               MF_CHECKED | MF_BYCOMMAND);
            }

            break;

        case IDM_GETDRIVEINFO:
            lpDialogProc = (FARPROC) DriveInfoDlgProc;

            DialogBoxParam (ghDllInst,
                            "DriveInfo",
                            hwndExtension,
                            lpDialogProc,
                            (LONG) hwndExtension);

            break;

        case IDM_GETFILESELLFN:
            lpDialogProc = (FARPROC) SelFileInfoDlgProc;

            DialogBoxParam (ghDllInst,
                            "FileInfo",
                            hwndExtension,
                            lpDialogProc,
                            (LONG) hwndExtension);

            break;

        case IDM_GETFOCUS:
            wFocusedItem = (WORD) SendMessage (hwndExtension, FM_GETFOCUS, 0, 0L);

            switch (wFocusedItem)
            {
                case FMFOCUS_DIR:
                    MessageBox (hwndExtension, "Focus is on the DIRECTORY window.",
                                "Focus Information", MB_OK);
                    break;

                case FMFOCUS_TREE:
                    MessageBox (hwndExtension, "Focus is on the TREE window.",
                                "Focus Information", MB_OK);
                    break;

                case FMFOCUS_DRIVES:
                    MessageBox (hwndExtension, "Focus is on the DRIVE bar.",
                                "Focus Information", MB_OK);
                    break;

                case FMFOCUS_SEARCH:
                    MessageBox (hwndExtension, "Focus is on the SEARCH RESULTS window.",
                                "Focus Information", MB_OK);
            }

            break;

        case IDM_REFRESHWINDOW:
        case IDM_REFRESHALLWINDOWS:
            // Refresh one or all the windows

            SendMessage (hwndExtension, FM_REFRESH_WINDOWS,
                         wMessage == IDM_REFRESHALLWINDOWS, 0L);

            break;

        case IDM_RELOADEXTENSIONS:
    		PostMessage(hwndExtension, FM_RELOAD_EXTENSIONS, 0, 0L);

            break;

        case IDM_ABOUTEXT:
            lpDialogProc = (FARPROC) AboutDlgProc;

            DialogBox (ghDllInst,
                       "AboutExtension",
                       hwndExtension,
                       lpDialogProc);

            break;
    }

    return (NULL);

} // FMExtentensionProc()


//***************************************************************************
//
//  CreateStatusWindow()
//
//  Purpose:
//
//      Creates a status window to display different File Manager events.
//
//
//  Parameters:
//
//      IN:
//          void
//
//      OUT:
//          void
//
//  Return Value:
//
//      TRUE if status window is created; otherwise FALSE.
//
//***************************************************************************

BOOL CreateStatusWindow (HWND hwndExtension)
{
    WNDCLASS    wc;

    wc.style            = NULL;
    wc.lpfnWndProc      = StatusWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = ghDllInst;
    wc.hIcon            = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor          = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground    = COLOR_WINDOW+1;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = (LPSTR) gszDllWndClass;

    if (!RegisterClass (&wc))
        return (FALSE);

    ghwndStatus = CreateWindowEx (WS_EX_TOPMOST | WS_EX_DLGMODALFRAME,
                                  gszDllWndClass,
                                  "File Manager Extension",
                                  WS_POPUP | WS_CAPTION,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  STATUS_WIDTH,
                                  STATUS_HEIGHT,
                                  hwndExtension,
                                  NULL,
                                  ghDllInst,
                                  NULL);

    ghwndInfo = CreateWindow ("STATIC",
                              NULL,
                              WS_CHILD | WS_VISIBLE,
                              INFO_LINE_X,
                              INFO_LINE_Y,
                              INFO_LINE_WIDTH,
                              INFO_LINE_HEIGHT,
                              ghwndStatus,
                              1,
                              ghDllInst,
                              NULL);


    return ((BOOL) ghwndStatus | ghwndInfo);
} // CreateStatusWindow()


//***************************************************************************
//
//  StatusWndProc()
//
//  Purpose:
//
//      Window procedure of the DLL's status window.
//
//
//  Parameters:
//
//      IN:
//          hWnd        - Identifies the status window
//          uMessage    - Message for this window
//          wParam      - Message information
//          lParam      - Additional message information
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      Appropriate value for the window message.
//
//***************************************************************************

long FAR PASCAL StatusWndProc (HWND hWnd, UINT uMessage,
			       WPARAM wParam, LPARAM lParam)
{
    switch (uMessage)
    {
        case WM_TIMER:
            // This timer is used to erase info from the 
            // status window at the elapsed time

            if (wParam == ID_STATUSTIMER)
            {
                KillTimer (hWnd, (UINT) wParam);
                SetWindowText (ghwndInfo, (LPSTR) '\0');
            }
            break;

        default:
            return (DefWindowProc (hWnd, uMessage, wParam, lParam));
    }
    return (0L);
} // StatusWndProc()


//***************************************************************************
//
//  DisplayStatus()
//
//  Purpose:
//
//      Displays a File Manager event in the status window.  A timer is used
//      to cler the status window after the elapsed time.  The timer messages
//      go to the status window's procedure.
//
//
//  Parameters:
//
//      IN:
//          hwndExtension   - Identifies the File Manager window
//          wEvent          - File Manager's event to be displayed
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      void
//
//***************************************************************************

void DisplayStatus (HWND hwndExtension, WORD wEvent)
{
    WORD    wFileCount;
    char    szInfo [INFO_STR_LEN];

    if (gbStatusWinVisible)
    {
        switch (wEvent)
        {
            case FMEVENT_INITMENU:
                SetWindowText (ghwndInfo, (LPSTR) "Extension menu selected...");
                break;
 
            case FMEVENT_SELCHANGE:
                wFileCount = (WORD) SendMessage (hwndExtension, FM_GETSELCOUNTLFN, 0, 0L);
                wsprintf (szInfo, "File selection changed: %d item(s) selected...", wFileCount);
                SetWindowText (ghwndInfo, (LPSTR) szInfo);
                break;

            case FMEVENT_UNLOAD:
                SetWindowText (ghwndInfo, (LPSTR) "Unloading extension...");
                break;

            case FMEVENT_USER_REFRESH:
                SetWindowText (ghwndInfo, (LPSTR) "Refreshing window(s)...");
                break;
        }

        // Timer to erase the info after the elapsed time

        SetTimer (ghwndStatus, ID_STATUSTIMER, TIMER_DURATION, NULL);
    }
} // DisplayStatus()


//***************************************************************************
//
//  AboutDlgProc()
//
//  Purpose:
//
//      Procedure to handle About dialog messages.  This dialog displays
//      copyright and help information.
//      
//
//  Parameters:
//
//      IN:
//          hDlg        - Dialog window handle
//          uMessage    - Dialog message
//          wParam      - Message information
//          lParam      - Additional message information
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      Appropriate value for the dialog message.
//
//***************************************************************************

BOOL FAR PASCAL AboutDlgProc (HWND hDlg,   unsigned uMessage,
                              WORD wParam, LONG lParam)
{
    switch (uMessage)
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
} // AboutDlgProc


//***************************************************************************
//
//  DriveInfoDlgProc()
//
//  Purpose:
//
//      Procedure to handle Drive Info dialog messages.  This dialog displays
//      information on the selected drive.  It queries this information by
//      sending a FM_GETDRIVEINFO message to File Manager.
//      
//
//  Parameters:
//
//      IN:
//          hDlg        - Dialog window handle
//          uMessage    - Dialog message
//          wParam      - Message information
//          lParam      - Additional message information
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      Appropriate value for the dialog message.
//
//***************************************************************************

BOOL FAR PASCAL DriveInfoDlgProc (HWND hDlg,   unsigned uMessage,
                                  WORD wParam, LONG lParam)
{
    static  FMS_GETDRIVEINFO    fmsDriveInfo;
            char                szTempString [SMALL_STR_LEN];

    switch (uMessage)
    {
        case WM_INITDIALOG:
            SendMessage ((HWND) lParam, FM_GETDRIVEINFO, 0,
                         (LONG) (LPFMS_GETDRIVEINFO) &fmsDriveInfo);

            // Convert OEM characters to Windows characters

            OemToAnsi (fmsDriveInfo.szPath, fmsDriveInfo.szPath);
            OemToAnsi (fmsDriveInfo.szVolume, fmsDriveInfo.szVolume);

            if (fmsDriveInfo.szShare [0])
                OemToAnsi (fmsDriveInfo.szShare, fmsDriveInfo.szShare);
            else
                lstrcpy (fmsDriveInfo.szShare, "< Not a Share >");

            if (fmsDriveInfo.szVolume [0])
                SetDlgItemText (hDlg, IDD_VOLUME, (LPSTR) fmsDriveInfo.szVolume);
            else
                SetDlgItemText (hDlg, IDD_VOLUME, "< No volume label >");

            SetDlgItemText (hDlg, IDD_PATH, (LPSTR) fmsDriveInfo.szPath);
            SetDlgItemText (hDlg, IDD_SHARE, (LPSTR) fmsDriveInfo.szShare);


            // When a -1 is returned for either dwTotalSpace or dwFreeSpace,
            // the extension will have compute that number on its own.

            if (fmsDriveInfo.dwTotalSpace == -1)
                SetDlgItemText (hDlg, IDD_TOTALSPACE, "< Info. not available >");
            else
            {
                wsprintf ((LPSTR) szTempString, "%ld", fmsDriveInfo.dwTotalSpace);
                SetDlgItemText (hDlg, IDD_TOTALSPACE, (LPSTR) szTempString);
            }

            if (fmsDriveInfo.dwFreeSpace == -1)
                SetDlgItemText (hDlg, IDD_FREESPACE, "< Info. not available >");
            else
            {
                wsprintf ((LPSTR) szTempString, "%ld", fmsDriveInfo.dwFreeSpace);
                SetDlgItemText (hDlg, IDD_FREESPACE, (LPSTR) szTempString);
            }

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

} // DriveInfoDlgProc()


//***************************************************************************
//
//  SelFIleInfoDlgProc()
//
//  Purpose:
//
//      Procedure to handle File Info dialog messages.  This dialog displays
//      information on the selected files.  It queries this information by
//      sending a FM_GETSELCOUNTLFN message to File Manager.
//      
//
//  Parameters:
//
//      IN:
//          hDlg        - Dialog window handle
//          uMessage    - Dialog message
//          wParam      - Message information
//          lParam      - Additional message information
//
//      OUT:
//          N/A
//
//  Return Value:
//
//      Appropriate value for the dialog message.
//
//***************************************************************************

BOOL FAR PASCAL SelFileInfoDlgProc (HWND hDlg,   unsigned uMessage,
                                    WORD wParam, LONG lParam)
{
    static  FMS_GETFILESEL  fmsFileInfo;
            WORD            wSelFileCount;
            WORD            wIndex;
            char            szTempString [SMALL_STR_LEN];
            DWORD           dwTotalSize = 0;

    switch (uMessage)
    {
        case WM_INITDIALOG:

            wSelFileCount = (WORD) SendMessage ((HWND) lParam,
                                                FM_GETSELCOUNTLFN, 0, 0L);

            wsprintf ((LPSTR) szTempString, "%d",
                      wSelFileCount);

            SetDlgItemText (hDlg, IDD_SELFILECOUNT, (LPSTR) szTempString);
    
            for (wIndex = 1; wIndex <= wSelFileCount; wIndex++)
            {
                SendMessage ((HWND) lParam, FM_GETFILESELLFN, wIndex,
                            (LONG) (LPFMS_GETFILESEL) &fmsFileInfo);

                dwTotalSize += fmsFileInfo.dwSize;

            }

            wsprintf ((LPSTR) szTempString, "%ld bytes",
                      dwTotalSize);

            SetDlgItemText (hDlg, IDD_SELFILESIZE, (LPSTR) szTempString);

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

} // SelFileInfoDlgProc()


