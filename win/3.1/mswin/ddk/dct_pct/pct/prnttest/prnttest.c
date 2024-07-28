/*---------------------------------------------------------------------------*\
| WINDOWS PRINTER TEST APPLICATION                                            |
|   This is an application to test Windows Printer Drivers.  It is not meant  |
|   to test GDI and it's ability to interact with printer drivers.  It is     |
|   intended to touch on the functionality of the driver itself.              |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "PrntTest.h"

#define WINDOWSTYLE (WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_DLGFRAME)

typedef DWORD (FAR PASCAL *MYPROC) (void);

/*---------------------------------------------------------------------------*\
| REGISTER WINDOW CLASS                                                       |
|   This routine registers the main window class for the application.         |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HANDLE hInstance - Window Task Instance.                                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - Returns TRUE if the window was registered.                         |
\*---------------------------------------------------------------------------*/
BOOL RegisterPrntTestClass(HANDLE hInstance)
{
  WNDCLASS  wndClass;

  wndClass.style         = CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc   = PrntTestProc;
  wndClass.cbClsExtra    = 0;
  wndClass.cbWndExtra    = 0;
  wndClass.hInstance     = hInstance;
  wndClass.hIcon         = LoadIcon(hInstance,PRNTTESTICON);
  wndClass.hCursor       = LoadCursor(NULL,IDC_ARROW);
  wndClass.hbrBackground = GetStockObject(WHITE_BRUSH);
  wndClass.lpszMenuName  = PRNTTESTMENU;
  wndClass.lpszClassName = PRNTTESTCLASS;

  return    RegisterClass(&wndClass);
}



/*---------------------------------------------------------------------------*\
| APPLICATION MAIN ENTRY POINT                                                |
|   This routine Registers and Creates the main application window(s) for     |
|   use during the task's existence.  After which, the application polls the  |
|   the system queue for messages to be dispatched.                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   RegisterPrntTestClass() - (init.c)                                        |
|   CreatePrntTestWindow()  - (init.c)                                        |
|                                                                             |
| PARAMETERS                                                                  |
|   HANDLE hInstance     - Indicates the Task instance of this app.           |
|   HANDLE hPrevInstance - Indicates the previous Task instance of app.       |
|   LPSTR  lpszCmdLine   - Parameters passed to application.                  |
|   int    nCmdShow      - How to display application.                        |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HANDLE hInst    - Initially set to instance.                              |
|   BOOL   bAutoRun - Set from private profile.                               |
|   BOOL   hPrntDlg - Handle to interface control window.                     |
|                                                                             |
| RETURNS                                                                     |
|   int - Passes back the wParam of the message structure.                    |
\*---------------------------------------------------------------------------*/
int PASCAL WinMain(HANDLE hInstance,
                   HANDLE hPrevInstance,
                   LPSTR  lpszCmdLine,
                   int    nCmdShow)
{
  HWND    hWnd;                               /* Handle to Main Window    */
  MSG     msg;                                /* Window Message Structure */

  hInst = hInstance;

  /*-----------------------------------------*\
  | Register Classes, then create the Windows.|
  | If there's an error, the quit application.|
  \*-----------------------------------------*/
  if (!hPrevInstance && !RegisterPrntTestClass(hInst))
    return  NULL;

  if (!(hWnd = CreateWindow(PRNTTESTCLASS, PRNTTESTTITLE, WINDOWSTYLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, NULL, NULL, hInst, NULL)))
    return NULL;

  /*-----------------------------------------*\
  | Update the Window Client area before      |
  | entering the main loop.                   |
  \*-----------------------------------------*/
  ShowWindow(hWnd,nCmdShow);
  UpdateWindow(hWnd);

  /*-----------------------------------------*\
  | If autorun, then start executing tests.   |
  \*-----------------------------------------*/
  if (bAutoRun)
    PostMessage(hWnd,WM_COMMAND,IDM_TEST_RUN,0l);

  /*-----------------------------------------*\
  | MAIN MESSAGE PROCESSING LOOP.             |
  \*-----------------------------------------*/
  while (GetMessage(&msg,NULL,0,0))
  {
    if (!hPrntDlg || !IsDialogMessage(hPrntDlg, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return msg.wParam;
}


/*---------------------------------------------------------------------------*\
| MAIN WINDOW PROCEDURE                                                       |
|   This is the main Window-Function routine.  It handles the message         |
|   handling/Processing/Filtering for the application.                        |
|                                                                             |
| CALLED ROUTINES                                                             |
|   ProcessPrntTestCommands() - (command.c)                                   |
|   PaintPrntTestWindow()     - (paint.c)                                     |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hWnd     - The Window Handle.                                    |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   LONG - Returns a long integer to Windows.  If this routine can't          |
|          handle the message, then it passes it to Windows Default           |
|          Window Procedure (DefWindowProc).                                  |
\*---------------------------------------------------------------------------*/
LONG FAR PASCAL PrntTestProc(HWND     hWnd,
                             unsigned iMessage,
                             WORD     wParam,
                             LONG lParam)
{
  FARPROC lpProc;
  RECT    rRect;

  switch    (iMessage)
  {
    /*------------------------------------*\
    | Handle any variable which need to be |
    | initialized upon creation of the app.|
    \*------------------------------------*/
    case  WM_CREATE:
      lpProc   = MakeProcInstance(PrntTestDlg,hInst);
      hPrntDlg = CreateDialog(hInst,MAKEINTRESOURCE(INTERFACE),hWnd,lpProc);

      /*--------------------------------------------------------------*\
      | Retrieve the settings from the initialization file.  Set any   |
      | values not in the file to a default of ALL combinations.       |
      \*--------------------------------------------------------------*/
      wHeaderSet = (WORD)GetPrivateProfileInt("Startup", "HeaderTest",
                                              IDD_HEAD_ALL,
                                              strApplicationProfile);
      wHeaderSet &= FLAGMASK;

      wTestsSet = (WORD)GetPrivateProfileInt("Startup", "TestsTest",
                                             IDD_TEST_ALL,
                                             strApplicationProfile);
      wTestsSet &= FLAGMASK;

      bAutoRun = (BOOL)GetPrivateProfileInt("Startup", "AutoRun", FALSE,
                                            strApplicationProfile);

      wFontOptions = GetPrivateProfileInt("Startup","Font Options",
                                          ALL_FONTS,strApplicationProfile);

      bMetafile = (BOOL)GetPrivateProfileInt("Startup", "Use Metafiles",
                                              FALSE, strApplicationProfile);

      wStyleSelected = (WORD)GetPrivateProfileInt("Startup", "Print Style",
                                                  31, strApplicationProfile);

      if(0x0003 == LOWORD(GetVersion()))
        wStyleSelected=30;

      GetClientRect(hPrntDlg, &rRect);

      /*
          The use of WS_CAPTION is a hack- AdjustWindowRect does not handle
          the WS_OVERLAP style bit correctly.
      */

      AdjustWindowRect(&rRect, WINDOWSTYLE | WS_CAPTION, TRUE);
      SetWindowPos(hWnd, NULL, 0, 0, rRect.right - rRect.left,
                   rRect.bottom - rRect.top, SWP_NOMOVE | SWP_NOZORDER);

      SetFocus(hPrntDlg);
      break;


    /*------------------------------------*\
    | Handle the processing of commands in |
    | which user selects through the menu. |
    \*------------------------------------*/
    case WM_COMMAND:
      ProcessPrntTestCommands(hWnd,wParam);
      break;

    /*------------------------------------*\
    | Restore focus to dialog box whenever |
    | we're restored                       |
    \*------------------------------------*/
    case WM_ACTIVATE:
      SetFocus(hPrntDlg);
      break;

    /*------------------------------------*\
    | Time to die... The is sent from the  |
    | DestroyWindow() function.  Clean up  |
    | any windows/objects used in app.     |
    \*------------------------------------*/
    case WM_DESTROY:
      DestroyWindow(hPrntDlg);
      PostQuitMessage(0);
      break;

    /*------------------------------------*\
    | Let Windows handle the Message.      |
    \*------------------------------------*/
    default:
      return(DefWindowProc(hWnd,iMessage,wParam,lParam));
  }

  return(0L);
}
