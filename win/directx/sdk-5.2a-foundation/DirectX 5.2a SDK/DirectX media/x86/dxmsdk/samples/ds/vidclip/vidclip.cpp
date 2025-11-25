//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright  1994-1996  Microsoft Corporation.  All Rights Reserved.
//
// PROGRAM: VidClip.c - Stolen from Generic.C
//
// PURPOSE: Illustrates the 'minimum' functionality of a well-behaved Win32 application..
//
// PLATFORMS:  Windows 95, Windows NT 4.0 and up
//
// FUNCTIONS:
//    WinMain() - calls initialization function, processes message loop
//    InitApplication() - Initializes window data nd registers window
//    InitInstance() -saves instance handle and creates main window
//    WindProc() Processes messages
//    About() - Process menssages for "About" dialog box
//    CenterWindow() -  Centers one window over another
//
// SPECIAL INSTRUCTIONS: N/A
//
#include <vcproj.h>

#define APPNAME "VidClip"

#include <strmif.h>
#include <uuids.h>
#include <amstream.h>
#include <initguid.h>
#include <ddrawex.h>

#define _WIN32_WINNT 0x0400
#define _ATL_APARTMENT_THREADED


#include "atlbase.h"
CComModule _Module;
#include "atlcom.h"

#include <atlimpl.cpp>



// Global Variables:

HINSTANCE hInst;      // current instance
char szAppName[100];  // Name of the app
char szTitle[100];    // The title bar text
IDirectDraw *g_pDD = NULL;

TCHAR g_szStart[100];
TCHAR g_szEnd[100];
TCHAR g_szAll[100];

const DDPIXELFORMAT g_aPixelFormats[] =
{
///    {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_PALETTEINDEXED8, 0, 8, 0, 0, 0, 0},
    {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16, 0x00007C00, 0x000003E0, 0x0000001F, 0},
    {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0},
    {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0}
};
CDocument g_Document;

// Foward declarations of functions included in this code module:

BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LPTSTR   GetStringRes (int id);
HRESULT CreateWriterStream(LPOLESTR szOutputFileName,
                           LPOLESTR szVideoCodec,
                           DDSURFACEDESC& ddsdVideoFormat,
                           LPOLESTR szAudioCodec,
                           WAVEFORMATEX& wfexAudioFormat,
                           IMultiMediaStream **ppMMStream);
void DoMakeMovie(HINSTANCE, HWND, CDocument *);


//
//  FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
//
//  PURPOSE: Entry point for the application.
//
//  COMMENTS:
//
// This function initializes the application and processes the
// message loop.
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
   MSG msg;
   HACCEL hAccelTable;

   CoInitialize(NULL);
   // Initialize global strings
   lstrcpy (szAppName, APPNAME);
   LoadString (hInstance, IDS_APP_TITLE, szTitle, 100);
   LoadString (hInstance, IDS_START, g_szStart, sizeof(g_szStart));
   LoadString (hInstance, IDS_END,   g_szEnd, sizeof(g_szEnd));
   LoadString (hInstance, IDS_ALL,   g_szAll, sizeof(g_szAll));

   if (!hPrevInstance) {
      // Perform instance initialization:
      if (!InitApplication(hInstance)) {
         return (FALSE);
      }
   }

   // Perform application initialization:
   if (!InitInstance(hInstance, nCmdShow)) {
      return (FALSE);
   }

   hAccelTable = LoadAccelerators (hInstance, szAppName);

   // Main message loop:
   while (GetMessage(&msg, NULL, 0, 0)) {
      if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   CoUninitialize();
   return (msg.wParam);

   lpCmdLine; // This will prevent 'unused formal parameter' warnings
}


//
//  FUNCTION: InitApplication(HANDLE)
//
//  PURPOSE: Initializes window data and registers window class
//
//  COMMENTS:
//
//       In this function, we initialize a window class by filling out a data
//       structure of type WNDCLASS.
//
BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX  wcex;
    HWND        hwnd;

    // Win32 will always set hPrevInstance to NULL, so lets check
    // things a little closer. This is because we only want a single
    // version of this app to run at a time
    hwnd = FindWindow (szAppName, szTitle);
    if (hwnd) {
        // We found another version of ourself. Lets defer to it:
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        SetForegroundWindow (hwnd);

        // If this app actually had any functionality, we would
        // also want to communicate any action that our 'twin'
        // should now perform based on how the user tried to
        // execute us.
        return FALSE;
    } else {
        // Fill in window class structure with parameters that describe
        // the main window.

         // Added elements for Windows 95:
        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = (WNDPROC)WndProc;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = hInstance;
        wcex.hIcon          = LoadIcon (hInstance, szAppName);
        wcex.hIconSm        = LoadIcon(hInstance, "SMALL");
        wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);

        wcex.lpszMenuName  = szAppName;
        wcex.lpszClassName = szAppName;

        // Register the window class and return success/failure code.
        return RegisterClassEx(&wcex);
    }
}


BOOL AddListViewItem(HWND hLV, TCHAR * pszItem, void * pThingie)
{
    LV_ITEM lvi;
    lvi.mask	= LVIF_TEXT | LVIF_PARAM;
    lvi.iItem	= 0x7FFF;
    lvi.iSubItem	= 0;
    lvi.pszText	= pszItem;
    lvi.lParam	= (LPARAM)pThingie;
    lvi.cchTextMax  = 0;
    ListView_InsertItem(hLV, &lvi);
    return TRUE;
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szAppName, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

   if (!hWnd) {
      return (FALSE);
   }

   InitCommonControls();
   /*
   HWND hListView = CreateWindowEx(0, WC_LISTVIEW, "Fred", WS_CHILDWINDOW | WS_CLIPSIBLINGS | LVS_REPORT,
                        0, 0, 0, 0, hWnd, (HMENU)1, hInstance, NULL);

   InitListView(hListView);
   AddListViewItem(hListView, "This is a test", NULL);
   AddListViewItem(hListView, "And another test", NULL);
   AddListViewItem(hListView, "And yet another test", NULL);
   */
   g_Document.Initialize(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   MessageBox (GetFocus(), GetStringRes(IDS_LIMITEDFUNCTION), szAppName, MB_OK);

   //ShowWindow(hListView, nCmdShow);
   //UpdateWindow(hListView);

   // BUGBUG -- There is still a bug in ddstream with 8bpp palettes & ddreaw
   // Make sure to make this work with a NULL g_pDD!!!
   CComPtr <IDirectDrawFactory> pFactory;
   HRESULT hr = CoCreateInstance(CLSID_DirectDrawFactory, NULL, CLSCTX_INPROC_SERVER,
                                IID_IDirectDrawFactory, (void **)&pFactory);
   if (SUCCEEDED(hr)) {
        hr = pFactory->CreateDirectDraw(NULL, NULL, DDSCL_NORMAL,0, NULL, &g_pDD);
   }
   return (TRUE);
}



void UpdateMenus(HWND hWnd, CDocument * pDoc)
{
    BOOL bEnableSave = FALSE;
    BOOL bEnableEdit = FALSE;
    if (pDoc->m_ClipList.NumClips() > 0) {
        bEnableSave = (pDoc->m_TargetFileName != NULL);
        bEnableEdit = (pDoc->m_ClipList.CurSelClipIndex() >= 0);
    }
    HMENU hMenu = GetMenu(hWnd);
    UINT uEditSetting = bEnableEdit ? MF_ENABLED : MF_GRAYED;
    UINT uSaveSetting = bEnableSave ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(hMenu, IDM_VIDEO_EDITCLIP, uEditSetting);
    EnableMenuItem(hMenu, IDM_VIDEO_DELETECLIP, uEditSetting);
    EnableMenuItem(hMenu, IDM_VIDEO_MAKEMOVIE, uSaveSetting);
    EnableMenuItem(hMenu, IDM_SAVE, uSaveSetting);
    EnableMenuItem(hMenu, IDM_SAVEAS, uSaveSetting);
}



//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  MESSAGES:
//
// WM_COMMAND - process the application menu
// WM_PAINT - Paint the main window
// WM_DESTROY - post a quit message and return
//    WM_DISPLAYCHANGE - message sent to Plug & Play systems when the display changes
//    WM_RBUTTONDOWN - Right mouse click -- put up context menu here if appropriate
//    WM_NCRBUTTONUP - User has clicked the right button on the application's system menu
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int wmId, wmEvent;
   PAINTSTRUCT ps;
   HDC hdc;
   POINT pnt;
   HMENU hMenu;
   BOOL bGotHelp;
   CDocument *pDoc = &g_Document;

   switch (message) {

      case WM_COMMAND:
         wmId    = LOWORD(wParam); // Remember, these are...
         wmEvent = HIWORD(wParam); // ...different for Win32!

         //Parse the menu selections:
         switch (wmId) {

            case IDM_ABOUT:
               DialogBox(hInst, "AboutBox", hWnd, (DLGPROC)About);
               break;

            case IDM_SETTINGS:
                DoSettingsDialog(hInst, hWnd, pDoc);
                UpdateMenus(hWnd, pDoc);
                break;

            case IDM_EXIT:
               DestroyWindow (hWnd);
               break;

            case IDM_HELPTOPICS: // Only called in Windows 95
               bGotHelp = WinHelp (hWnd, APPNAME".HLP", HELP_FINDER,(DWORD)0);
               if (!bGotHelp)
               {
                  MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
                              szAppName, MB_OK|MB_ICONHAND);
               }
               break;

            case IDM_HELPCONTENTS: // Not called in Windows 95
               bGotHelp = WinHelp (hWnd, APPNAME".HLP", HELP_CONTENTS,(DWORD)0);
               if (!bGotHelp)
               {
                  MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
                              szAppName, MB_OK|MB_ICONHAND);
               }
               break;

            case IDM_HELPSEARCH: // Not called in Windows 95
               if (!WinHelp(hWnd, APPNAME".HLP", HELP_PARTIALKEY,
                           (DWORD)(LPSTR)""))
               {
                  MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
                              szAppName, MB_OK|MB_ICONHAND);
               }
               break;

            case IDM_HELPHELP: // Not called in Windows 95
               if(!WinHelp(hWnd, (LPSTR)NULL, HELP_HELPONHELP, 0))
               {
                  MessageBox (GetFocus(), GetStringRes(IDS_NO_HELP),
                              szAppName, MB_OK|MB_ICONHAND);
               }
               break;

            case IDM_VIDEO_ADDCLIP:
                pDoc->NewClip();
                break;

            case IDM_VIDEO_EDITCLIP:
                pDoc->EditClip();
                break;

            case IDM_VIDEO_DELETECLIP:
                pDoc->DeleteClip();
                break;

            case IDM_VIDEO_MAKEMOVIE:
                DoMakeMovie(hInst, hWnd, pDoc);
                break;

            // Here are all the other possible menu options,
            // all of these are currently disabled:
            case IDM_NEW:
                pDoc->ResetContents();
                break;

            case IDM_OPEN:
                return pDoc->OpenFile();

            case IDM_SAVE:
            case IDM_SAVEAS:
                return pDoc->SaveAsFile(wmId == IDM_SAVEAS);

            case IDM_UNDO:
            case IDM_CUT:
            case IDM_COPY:
            case IDM_PASTE:
            case IDM_LINK:
            case IDM_LINKS:

            default:
               return (DefWindowProc(hWnd, message, wParam, lParam));
         }
         break;

      case WM_NCRBUTTONUP: // RightClick on windows non-client area...
         if (SendMessage(hWnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU)
         {
            // The user has clicked the right button on the applications
            // 'System Menu'. Here is where you would alter the default
            // system menu to reflect your application. Notice how the
            // explorer deals with this. For this app, we aren't doing
            // anything
            return (DefWindowProc(hWnd, message, wParam, lParam));
         } else {
            // Nothing we are interested in, allow default handling...
            return (DefWindowProc(hWnd, message, wParam, lParam));
         }
            break;

        case WM_RBUTTONDOWN: // RightClick in windows client area...
            pnt.x = LOWORD(lParam);
            pnt.y = HIWORD(lParam);
            ClientToScreen(hWnd, (LPPOINT) &pnt);
      // This is where you would determine the appropriate 'context'
      // menu to bring up. Since this app has no real functionality,
      // we will just bring up the 'Help' menu:
            hMenu = GetSubMenu (GetMenu (hWnd), 2);
            if (hMenu) {
                TrackPopupMenu (hMenu, 0, pnt.x, pnt.y, 0, hWnd, NULL);
            } else {
            // Couldn't find the menu...
                MessageBeep(0);
            }
            break;



      case WM_PAINT:
         hdc = BeginPaint (hWnd, &ps);
         // Add any drawing code here...
         EndPaint (hWnd, &ps);
         break;

      case WM_DESTROY:
         // Tell WinHelp we don't need it any more...
               WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0);
         PostQuitMessage(0);
         break;

      case WM_SIZE:
          pDoc->m_ClipList.SetSize(LOWORD(lParam), HIWORD(lParam));
          break;

      case WM_NOTIFY: {
          LPNMHDR pnmhdr = (LPNMHDR)lParam;
          if (pnmhdr->hwndFrom == pDoc->m_ClipList.m_hLV) {
              if (pnmhdr->code == NM_DBLCLK) {
                pDoc->EditClip();
              }
              UpdateMenus(hWnd, pDoc);
          }
          }
          break;

      case WM_ACTIVATE:
          UpdateMenus(hWnd, pDoc);
          // Fall through...

      default:
         return (DefWindowProc(hWnd, message, wParam, lParam));
   }
   return (0);
}

//
//  FUNCTION: About(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for "About" dialog box
//       This version allows greater flexibility over the contents of the 'About' box,
//       by pulling out values from the 'Version' resource.
//
//  MESSAGES:
//
// WM_INITDIALOG - initialize dialog box
// WM_COMMAND    - Input received
//
//
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   static  HFONT hfontDlg;    // Font for dialog text
   static   HFONT hFinePrint; // Font for 'fine print' in dialog
   DWORD   dwVerInfoSize;     // Size of version information block
   LPSTR   lpVersion;         // String pointer to 'version' text
   DWORD   dwVerHnd=0;        // An 'ignored' parameter, always '0'
   UINT    uVersionLen;
   WORD    wRootLen;
   BOOL    bRetCode;
   int     i;
   char    szFullPath[256];
   char    szResult[256];
   char    szGetName[256];
   DWORD dwVersion;
   char  szVersion[40];
   DWORD dwResult;

   switch (message) {
        case WM_INITDIALOG:
         ShowWindow (hDlg, SW_HIDE);

         if (PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE)
         {
            hfontDlg = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0,
                                  VARIABLE_PITCH | FF_DONTCARE, "");
            hFinePrint = CreateFont(11, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0,
                                    VARIABLE_PITCH | FF_DONTCARE, "");
         }
         else
         {
            hfontDlg = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  VARIABLE_PITCH | FF_SWISS, "");
            hFinePrint = CreateFont(11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    VARIABLE_PITCH | FF_SWISS, "");
         }

         CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
         GetModuleFileName (hInst, szFullPath, sizeof(szFullPath));

         // Now lets dive in and pull out the version information:
         dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
         if (dwVerInfoSize) {
            LPSTR   lpstrVffInfo;
            HANDLE  hMem;
            hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
            lpstrVffInfo  = (char *)GlobalLock(hMem);
            GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
            // The below 'hex' value looks a little confusing, but
            // essentially what it is, is the hexidecimal representation
            // of a couple different values that represent the language
            // and character set that we are wanting string values for.
            // 040904E4 is a very common one, because it means:
            //   US English, Windows MultiLingual characterset
            // Or to pull it all apart:
            // 04------        = SUBLANG_ENGLISH_USA
            // --09----        = LANG_ENGLISH
            // --11----        = LANG_JAPANESE
            // ----04E4 = 1252 = Codepage for Windows:Multilingual

            lstrcpy(szGetName, GetStringRes(IDS_VER_INFO_LANG));

            wRootLen = lstrlen(szGetName); // Save this position

            // Set the title of the dialog:
            lstrcat (szGetName, "ProductName");
            bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
               (LPSTR)szGetName,
               (LPVOID *)&lpVersion,
               (UINT *)&uVersionLen);

            // Notice order of version and string...
            if (PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE)
            {
               lstrcpy(szResult, lpVersion);
               lstrcat(szResult, " ÇÃÉoÅ[ÉWÉáÉìèÓïÒ");
            }
            else
            {
               lstrcpy(szResult, "About ");
               lstrcat(szResult, lpVersion);
            }

            // -----------------------------------------------------

            SetWindowText (hDlg, szResult);

            // Walk through the dialog items that we want to replace:
            for (i = DLG_VERFIRST; i <= DLG_VERLAST; i++) {
               GetDlgItemText(hDlg, i, szResult, sizeof(szResult));
               szGetName[wRootLen] = (char)0;
               lstrcat (szGetName, szResult);
               uVersionLen   = 0;
               lpVersion     = NULL;
               bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
                  (LPSTR)szGetName,
                  (LPVOID *)&lpVersion,
                  (UINT *)&uVersionLen);

               if ( bRetCode && uVersionLen && lpVersion) {
               // Replace dialog item text with version info
                  lstrcpy(szResult, lpVersion);
                  SetDlgItemText(hDlg, i, szResult);
               }
               else
               {
                  dwResult = GetLastError();

                  wsprintf(szResult, GetStringRes(IDS_VERSION_ERROR), dwResult);
                  SetDlgItemText (hDlg, i, szResult);
               }
               SendMessage (GetDlgItem (hDlg, i), WM_SETFONT,
                  (UINT)((i==DLG_VERLAST)?hFinePrint:hfontDlg),
                  TRUE);
            } // for (i = DLG_VERFIRST; i <= DLG_VERLAST; i++)


            GlobalUnlock(hMem);
            GlobalFree(hMem);

         } else {
            // No version information available.
         } // if (dwVerInfoSize)

            SendMessage (GetDlgItem (hDlg, IDC_LABEL), WM_SETFONT,
            (WPARAM)hfontDlg,(LPARAM)TRUE);

         // We are  using GetVersion rather then GetVersionEx
         // because earlier versions of Windows NT and Win32s
         // didn't include GetVersionEx:
         dwVersion = GetVersion();

         if (dwVersion < 0x80000000) {
            // Windows NT
            wsprintf (szVersion, "Microsoft Windows NT %u.%u (Build: %u)",
               (DWORD)(LOBYTE(LOWORD(dwVersion))),
               (DWORD)(HIBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIWORD(dwVersion)) );
         } else if (LOBYTE(LOWORD(dwVersion))<4) {
            // Win32s
                wsprintf (szVersion, "Microsoft Win32s %u.%u (Build: %u)",
               (DWORD)(LOBYTE(LOWORD(dwVersion))),
               (DWORD)(HIBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIWORD(dwVersion) & ~0x8000) );
         } else {
            // Windows 95
                wsprintf (szVersion, "Microsoft Windows 95 %u.%u",
                    (DWORD)(LOBYTE(LOWORD(dwVersion))),
                    (DWORD)(HIBYTE(LOWORD(dwVersion))) );
         }

          SetWindowText (GetDlgItem(hDlg, IDC_OSVERSION), szVersion);
         ShowWindow (hDlg, SW_SHOW);
         return (TRUE);

      case WM_COMMAND:
         if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, TRUE);
            DeleteObject (hfontDlg);
            DeleteObject (hFinePrint);
            return (TRUE);
         }
         break;
   }

    return FALSE;
}

//
//   FUNCTION: CenterWindow(HWND, HWND)
//
//   PURPOSE: Centers one window over another.
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//       This functionwill center one window over another ensuring that
//    the placement of the window is within the 'working area', meaning
//    that it is both within the display limits of the screen, and not
//    obscured by the tray or other framing elements of the desktop.
BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
   RECT    rChild, rParent, rWorkArea;
   int     wChild, hChild, wParent, hParent;
   int     xNew, yNew;
   BOOL  bResult;

   // Get the Height and Width of the child window
   GetWindowRect (hwndChild, &rChild);
   wChild = rChild.right - rChild.left;
   hChild = rChild.bottom - rChild.top;

   // Get the Height and Width of the parent window
   GetWindowRect (hwndParent, &rParent);
   wParent = rParent.right - rParent.left;
   hParent = rParent.bottom - rParent.top;

   // Get the limits of the 'workarea'
   bResult = SystemParametersInfo(
      SPI_GETWORKAREA,  // system parameter to query or set
      sizeof(RECT),
      &rWorkArea,
      0);
   if (!bResult) {
      rWorkArea.left = rWorkArea.top = 0;
      rWorkArea.right = GetSystemMetrics(SM_CXSCREEN);
      rWorkArea.bottom = GetSystemMetrics(SM_CYSCREEN);
   }

   // Calculate new X position, then adjust for workarea
   xNew = rParent.left + ((wParent - wChild) /2);
   if (xNew < rWorkArea.left) {
      xNew = rWorkArea.left;
   } else if ((xNew+wChild) > rWorkArea.right) {
      xNew = rWorkArea.right - wChild;
   }

   // Calculate new Y position, then adjust for workarea
   yNew = rParent.top  + ((hParent - hChild) /2);
   if (yNew < rWorkArea.top) {
      yNew = rWorkArea.top;
   } else if ((yNew+hChild) > rWorkArea.bottom) {
      yNew = rWorkArea.bottom - hChild;
   }

   // Set it, and return
   return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


//---------------------------------------------------------------------------
//
// FUNCTION:    GetStringRes (int id INPUT ONLY)
//
// COMMENTS:    Load the resource string with the ID given, and return a
//              pointer to it.  Notice that the buffer is common memory so
//              the string must be used before this call is made a second time.
//
//---------------------------------------------------------------------------

LPTSTR   GetStringRes (int id)
{
  static TCHAR buffer[MAX_PATH];

  buffer[0]=0;
  LoadString (GetModuleHandle (NULL), id, buffer, MAX_PATH);
  return buffer;
}



////////////////////////////////////////////////////////////////////

typedef HRESULT (STDAPICALLTYPE * PFNSAMPLECALLBACK) (IStreamSample *pSource,
                                                      IStreamSample *pDest,
                                                      void * pvContext);

#define MAX_COPY_STREAMS 5

class CopyPair {
public:
    CComPtr<IStreamSample> pSource;
    CComPtr<IStreamSample> pDest;
    MSPID PurposeId;
    PFNSAMPLECALLBACK pCallback;
    void * pCallbackContext;
    HRESULT hrLastStatus;
    bool    bReading;
    bool    bGotFirstSample;
    STREAM_TIME stActualStart;
    STREAM_TIME stStartBias;
    STREAM_TIME stLastEndTime;
};



class CCopyEngine
{
public:
    CCopyEngine(IMultiMediaStream *pDestMMStream);
    ~CCopyEngine();

    HRESULT InitStream(REFMSPID PurposeId,
                       PFNSAMPLECALLBACK pCallback = NULL,
                       void * pContext = NULL,
                       bool bSharedFormat = false);
    HRESULT CopyStreamData(IMultiMediaStream *pSourceMMStream,
                           STREAM_TIME stStart, STREAM_TIME stEnd);

private:
    CComPtr<IMultiMediaStream>  m_pDestMMStream;
    CopyPair        m_aPair[MAX_COPY_STREAMS];
    HANDLE          m_aEvent[MAX_COPY_STREAMS];
    int             m_cNumPairs;
    bool            m_bStarted;
};

///////

CCopyEngine::CCopyEngine(IMultiMediaStream *pDestMMStream) :
    m_cNumPairs(0),
    m_pDestMMStream(pDestMMStream),
    m_bStarted(false)
    {};

HRESULT CCopyEngine::InitStream(REFMSPID PurposeId,
                                PFNSAMPLECALLBACK pCallback, void * pContext,
                                bool bSharedFormat)
{
    HRESULT hr = E_FAIL;    // Assume it won't work.
    if (m_cNumPairs < MAX_COPY_STREAMS) {
        CComPtr <IMediaStream> pDest;
        if (m_pDestMMStream->GetMediaStream(PurposeId, &pDest) == NOERROR) {
            CComPtr<IStreamSample> pDestSample;
            hr = pDest->AllocateSample(0, &pDestSample);
            if (SUCCEEDED(hr)) {
                m_aEvent[m_cNumPairs] = CreateEvent(NULL, TRUE, FALSE, NULL);
                if (!m_aEvent[m_cNumPairs]) {
                    hr = E_OUTOFMEMORY;
                } else {
                    m_aPair[m_cNumPairs].pDest = pDestSample;
                    m_aPair[m_cNumPairs].pCallback = pCallback;
                    m_aPair[m_cNumPairs].pCallbackContext = pContext;
                    m_aPair[m_cNumPairs].PurposeId = PurposeId;
                    m_aPair[m_cNumPairs].stStartBias = 0;
                    m_cNumPairs++;
                }
            }
        }
    }
    return hr;
}


HRESULT CCopyEngine::CopyStreamData(IMultiMediaStream *pSourceMMStream,
                                    STREAM_TIME stClipStart, STREAM_TIME stClipEnd)
{
    HRESULT hr;
    if (m_cNumPairs == 0) {
        return S_FALSE;
    }
    int i;
    for (i = 0; i < m_cNumPairs; i++) {
        m_aPair[i].bGotFirstSample = false;
        CComPtr <IMediaStream> pSource;
        if (pSourceMMStream->GetMediaStream(m_aPair[i].PurposeId, &pSource) == NOERROR) {
            m_aPair[i].pSource = NULL;
            hr = pSource->AllocateSample(0, &m_aPair[i].pSource);
        } else {
            hr = E_FAIL;
        }
        if (FAILED(hr)) {
            return hr;
        }
    }
    pSourceMMStream->SetState(STREAMSTATE_RUN);
    if (stClipStart) {
        pSourceMMStream->Seek(stClipStart);
    }
    for (i = 0; i < m_cNumPairs; i++) {
        m_aPair[i].hrLastStatus = NOERROR;
        m_aPair[i].bReading = true;
        m_aPair[i].pSource->Update(0, m_aEvent[i], NULL, 0);
    }
    if (!m_bStarted) {
        m_pDestMMStream->SetState(STREAMSTATE_RUN);
        m_bStarted = true;
    }
    int NumRunning = m_cNumPairs;
    while (NumRunning > 0) {
        DWORD dwWaitRet = WaitForMultipleObjects(m_cNumPairs, m_aEvent, FALSE, INFINITE);
        if (dwWaitRet >= WAIT_OBJECT_0 && dwWaitRet < WAIT_OBJECT_0 + m_cNumPairs) {
            int iCompleted = dwWaitRet - WAIT_OBJECT_0;
            CopyPair *pPair = &m_aPair[iCompleted];
            IStreamSample *pDone = pPair->bReading ? pPair->pSource : pPair->pDest;
            pPair->hrLastStatus = pDone->CompletionStatus(0, 0);
            if (pPair->hrLastStatus == NOERROR) {
                if (pPair->bReading) {
                    STREAM_TIME stStart, stStop;
                    pPair->pSource->GetSampleTimes(&stStart, &stStop, NULL);
                    if (stClipEnd > 0 && stStart > stClipEnd) {
                        if (pPair->bGotFirstSample) {
                            pPair->stStartBias += pPair->stLastEndTime - pPair->stActualStart;
                        }
                        NumRunning--;
                        ResetEvent(m_aEvent[iCompleted]);
                    } else {
                        if (pPair->pCallback) {
                            pPair->pCallback(pPair->pSource, pPair->pDest, pPair->pCallbackContext);
                        }
                        if (!pPair->bGotFirstSample) {
                            pPair->stActualStart = stStart;
                            pPair->bGotFirstSample = true;
                        }
                        pPair->stLastEndTime = stStop;
                        stStart += pPair->stStartBias - pPair->stActualStart;
                        stStop += pPair->stStartBias - pPair->stActualStart;
                        pPair->pDest->SetSampleTimes(&stStart, &stStop);
                        pPair->bReading = false;
                        pPair->pDest->Update(0, m_aEvent[iCompleted], NULL, 0);
                    }
                } else {
                    pPair->pSource->Update(0, m_aEvent[iCompleted], NULL, 0);
                    pPair->bReading = true;
                }
            } else {
                if (pPair->bGotFirstSample) {
                    pPair->stStartBias += pPair->stLastEndTime - pPair->stActualStart;
                }
                ResetEvent(m_aEvent[iCompleted]);
                NumRunning--;
            }
        }
    }
    pSourceMMStream->SetState(STREAMSTATE_STOP);
    for (i = 0; i < m_cNumPairs; i++) {
        m_aPair[i].pSource = NULL;  // Release the source sample
    }
    return NOERROR;
}


CCopyEngine::~CCopyEngine()
{
    int i;
    for (i = 0; i < m_cNumPairs; i++) {
        CloseHandle(m_aEvent[i]);
        if (m_bStarted && m_aPair[i].pDest) {
            CComPtr<IMediaStream> pMS;
            m_aPair[i].pDest->GetMediaStream(&pMS);
            pMS->SendEndOfStream(0);
        }
    }
    if (m_bStarted) {
        m_pDestMMStream->SetState(STREAMSTATE_STOP);
    }

}




///////////////////////////////////////////////////////////////////

void ErrorMessage(int StringId)
{
    MessageBox (GetFocus(), GetStringRes(StringId), szAppName, MB_OK|MB_ICONHAND);
}

#define CHECK_ERROR(x, idFailMsg) if (FAILED(hr = (x))) { if (idFailMsg) ErrorMessage(idFailMsg); goto Exit; }

#define RECTHEIGHT(r) ((r).bottom - (r).top)
#define RECTWIDTH(r)  ((r).right - (r).left)

HRESULT STDAPICALLTYPE VideoCallback(IStreamSample *pSource, IStreamSample *pDest,
                                     void * pvhWnd)
{
    CComQIPtr <IDirectDrawStreamSample, &IID_IDirectDrawStreamSample> pSrcSample(pSource);
    CComPtr <IDirectDrawSurface> pSrcSurface;
    RECT rectSrc;
    if (SUCCEEDED(pSrcSample->GetSurface(&pSrcSurface, &rectSrc))) {
        HDC hdcSurface;
        if (SUCCEEDED(pSrcSurface->GetDC(&hdcSurface))) {
            CComQIPtr <IDirectDrawStreamSample, &IID_IDirectDrawStreamSample> pDestSample(pDest);
            CComPtr <IDirectDrawSurface> pDestSurface;
            RECT rectDest;
            HRESULT hr = pDestSample->GetSurface(&pDestSurface, &rectDest);

            HDC hdcDest = GetDC((HWND)pvhWnd);
            BOOL fred = StretchBlt(hdcDest, 0, 0, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top,
                       hdcSurface, rectSrc.left, rectSrc.top, RECTWIDTH(rectSrc), RECTHEIGHT(rectSrc), SRCCOPY);
            ReleaseDC((HWND)pvhWnd, hdcDest);

            hr = pDestSurface->GetDC(&hdcDest);
            StretchBlt(hdcDest, 0, 0, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top,
                       hdcSurface, rectSrc.left, rectSrc.top, RECTWIDTH(rectSrc), RECTHEIGHT(rectSrc), SRCCOPY);

            TextOut(hdcDest, 20, 20, "Easy to do effects!", 19);

            pDestSurface->ReleaseDC(hdcDest);

        }

        //  Release this ALWAYS to bypass NT4.0 DDraw bug
        pSrcSurface->ReleaseDC(hdcSurface);
    }
    return NOERROR;
}



HRESULT OpenReadMMStream(LPOLESTR pszFileName, DDSURFACEDESC & ddsd, IMultiMediaStream **ppMMStream)
{
    *ppMMStream = NULL;
    CComPtr <IAMMultiMediaStream> pAMStream;
    CComPtr <IMediaStream> pVideoStream;
    CComQIPtr<IDirectDrawMediaStream, &IID_IDirectDrawMediaStream> pDDStream;
    HRESULT hr;

    CHECK_ERROR(CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
				 IID_IAMMultiMediaStream, (void **)&pAMStream), IDS_UNABLETOINITREAD);
    CHECK_ERROR(pAMStream->Initialize(STREAMTYPE_READ, 0, NULL), IDS_UNABLETOINITREAD);
    CHECK_ERROR(pAMStream->AddMediaStream(g_pDD, &MSPID_PrimaryVideo, 0, &pVideoStream), IDS_UNABLETOINITREAD);
    pDDStream = pVideoStream;

    CHECK_ERROR(pDDStream->SetFormat(&ddsd, NULL), IDS_UNABLETOINITREAD);
 //   CHECK_ERROR(pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, 0, NULL));

    CHECK_ERROR(pAMStream->OpenFile(pszFileName, AMMSF_NOCLOCK), IDS_UNABLETOOPENREAD);

    *ppMMStream = pAMStream;
    pAMStream->AddRef();

Exit:
    return hr;
}



void DoMakeMovie(HINSTANCE hInst, HWND hWndPreview, CDocument * pDocument)
{
    if (!pDocument->m_TargetFileName) {
        ErrorMessage(IDS_NOFILENAME);
        return;
    }
    if (pDocument->m_ClipList.NumClips() == 0) {
        ErrorMessage(IDS_NOCLIPS);
        return;
    }

    DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.dwHeight = pDocument->m_Height;
    ddsd.dwWidth  = pDocument->m_Width;
    memcpy(&ddsd.ddpfPixelFormat, &g_aPixelFormats[pDocument->m_PixelDepth], sizeof(ddsd.ddpfPixelFormat));

    WAVEFORMATEX wfex;

    ShowWindow(pDocument->m_ClipList.m_hLV, SW_HIDE);

    CComPtr <IMultiMediaStream> pWriterStream;
    if (SUCCEEDED(CreateWriterStream(pDocument->m_TargetFileName,   // If this fails, it will display an appropriate message
                       pDocument->m_VideoCodecDisplayName,
                       ddsd,
                       pDocument->m_AudioCodecDisplayName,
                       wfex,
                       &pWriterStream))) {
        CCopyEngine Engine(pWriterStream);
        Engine.InitStream(MSPID_PrimaryVideo, VideoCallback, hWndPreview, false);
        int NumClips = pDocument->m_ClipList.NumClips();
        for (int i = 0; i < NumClips; i++) {
            CClip *pClip = pDocument->m_ClipList.GetClip(i);
            CComPtr <IMultiMediaStream> pReadStream;
            ddsd.dwFlags = DDSD_PIXELFORMAT;    // Only set pixel format on source streams
            if (SUCCEEDED(OpenReadMMStream(pClip->m_FileName, ddsd, &pReadStream))) {
                Engine.CopyStreamData(pReadStream, pClip->m_stStart, pClip->m_stEnd);
            }
        }
    }

    ShowWindow(pDocument->m_ClipList.m_hLV, SW_SHOW);
}



HRESULT AddAndRenderCompressor(LPOLESTR pszCodec,
                               LPOLESTR pszFilterName,
                               ICaptureGraphBuilder *pBuilder,
                               IMediaStream *pStream,
                               IBaseFilter *pMuxFilter)
{
    HRESULT hr;
    CComPtr <IMoniker> pDeviceMoniker;
    CComPtr <IBindCtx> pBindCtx;
    CComPtr <IBaseFilter> pCodecFilter;
    CComPtr <IGraphBuilder> pFilterGraph;
    if (pszCodec) {
        unsigned long ccEaten;
        CHECK_ERROR(CreateBindCtx(0, &pBindCtx), IDS_NOCOMPRESSOR);
        CHECK_ERROR(MkParseDisplayName(pBindCtx, pszCodec, &ccEaten, &pDeviceMoniker), IDS_NOCOMPRESSOR);
        CHECK_ERROR(pDeviceMoniker->BindToObject(pBindCtx, NULL, IID_IBaseFilter, (void **)&pCodecFilter), IDS_NOCOMPRESSOR);
        CHECK_ERROR(pBuilder->GetFiltergraph(&pFilterGraph), IDS_INTERNALERROR);
        CHECK_ERROR(pFilterGraph->AddFilter(pCodecFilter, pszFilterName), IDS_INTERNALERROR);
        hr = pBuilder->RenderStream(NULL, pStream, pCodecFilter, pMuxFilter);
        if (FAILED(hr)) {
            ErrorMessage(IDS_CANTCOMPRESS);
        }
    } else {
        hr = pBuilder->RenderStream(NULL, pStream, NULL, pMuxFilter);
        if (FAILED(hr)) {
            ErrorMessage(IDS_CANTCONNECTTOMUX);
        }
    }
Exit:
    return hr;
}


HRESULT CreateWriterStream(LPOLESTR szOutputFileName,
                           LPOLESTR szVideoCodec,
                           DDSURFACEDESC& ddsdVideoFormat,
                           LPOLESTR szAudioCodec,
                           WAVEFORMATEX& wfexAudioFormat,
                           IMultiMediaStream **ppMMStream)
{
    *ppMMStream = NULL;
    CComPtr <IAMMultiMediaStream> pAMStream;
    CComPtr <IMediaStream> pVideoStream;
    CComPtr <IMediaStream> pAudioStream;
    CComPtr <ICaptureGraphBuilder> pBuilder;
    CComPtr <IGraphBuilder> pFilterGraph;
    CComPtr <IFileSinkFilter> pFileSinkWriter;
    CComPtr <IBaseFilter> pMuxFilter;
    CComQIPtr <IDirectDrawMediaStream, &IID_IDirectDrawMediaStream> pDDStream;
    CComQIPtr <IAudioMediaStream, &IID_IAudioMediaStream> pAudioSpecificStream;
    HRESULT hr;

    CHECK_ERROR(CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
				 IID_IAMMultiMediaStream, (void **)&pAMStream), IDS_UNABLETOINITWRITE);
    CHECK_ERROR(pAMStream->Initialize(STREAMTYPE_WRITE, 0, NULL), IDS_UNABLETOINITWRITE);

    CHECK_ERROR(pAMStream->AddMediaStream(g_pDD, &MSPID_PrimaryVideo, 0, &pVideoStream), IDS_UNABLETOINITWRITE);
  //  CHECK_ERROR(pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, 0, &pAudioStream));

    pDDStream = pVideoStream;
   // pAudioSpecificStream = pAudioStream;

    CHECK_ERROR(pDDStream->SetFormat(&ddsdVideoFormat, NULL), IDS_CANTSETFORMAT);
 /// BUGBUG!!!   CHECK_ERROR(pAudioSpecificStream->SetFormat(&wfexAudioFormat));

    CHECK_ERROR(CoCreateInstance(CLSID_CaptureGraphBuilder, NULL, CLSCTX_INPROC_SERVER,
                                 IID_ICaptureGraphBuilder, (void **)&pBuilder), IDS_UNABLETOINITWRITE);

    CHECK_ERROR(pAMStream->GetFilterGraph(&pFilterGraph), IDS_INTERNALERROR);
    CHECK_ERROR(pBuilder->SetFiltergraph(pFilterGraph), IDS_INTERNALERROR);

    CHECK_ERROR(pBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, szOutputFileName, &pMuxFilter, &pFileSinkWriter), IDS_UNABLETOSETOUTPUTNAME);

    CHECK_ERROR(AddAndRenderCompressor(szVideoCodec, L"Video compressor", pBuilder, pVideoStream, pMuxFilter), 0); // This function displays its own message
///    CHECK_ERROR(AddAndRenderCompressor(szAudioCodec, L"Audio compressor", pBuilder, pAudioStream, pMuxFilter));

    *ppMMStream = pAMStream;
    (*ppMMStream)->AddRef();

Exit:
    return hr;
}
