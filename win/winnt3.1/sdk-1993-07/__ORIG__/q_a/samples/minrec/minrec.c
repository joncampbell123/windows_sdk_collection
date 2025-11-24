// ************************************************************************
//
//                      Microsoft Developer Support
//             Copyright (c) 1993 Microsoft Corporation
//
// ************************************************************************
// MODULE    : MinRec.C
// PURPOSE   : A Small Win32 Recorder-like Sample Application
// FUNCTIONS :
//   WinMain()         - initializes the main window, dispatches messages
//   MainDlgProc()     - processes messages for the main dialog box window
//   ErrorMessageBox() - displays an error message box when called
// COMMENTS  :
// ************************************************************************
#define   STRICT               // strict type checking enabled
#define   UNICODE              // make the application unicode complient
#include <Windows.H>           // required for all Windows applications

#include "MinRec.H"            // specific to this program
#include "RecHook.H"           // global journal hook functions

// internal defines
// ------------------------------------------------------------------------
typedef struct GLOBAL_STRUCT {
  HWND      hDlgMain;            // Main dialog box window handle
  HINSTANCE hInstance;           // current instance
  LPCTSTR   lpszApiFailedMsg;    // "A Windows API Failed" message
} GLOBAL, *PGLOBAL;

// internal data
// ------------------------------------------------------------------------
GLOBAL  Global;
LPCTSTR lpszSourceFileName = TEXT(__FILE__);

// internal function prototypes
// ------------------------------------------------------------------------
BOOL CALLBACK MainDlgProc ( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK AboutDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL          ErrorMessageBox ( LPCTSTR, LPCTSTR, LPCTSTR, INT );

// ************************************************************************
// FUNCTION : WinMain( HINSTANCE, HINSTANCE, LPSTR, INT )
// PURPOSE  : Calls initialization function, processes message loop
// COMMENTS :
// ************************************************************************
INT PASCAL
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
  INT nCmdShow )
{
  MSG     Msg;
  HACCEL  hAccel;
  LPCTSTR lpszClassName   = TEXT( "MinRecClass" );
  LPCTSTR lpszMenuName    = TEXT( "MinRecMenu"  );
  LPCTSTR lpszIconName    = TEXT( "MinRecIcon"  );
  LPCTSTR lpszAccelName   = TEXT( "MinRecAccel" );

  //-- check if Win32s, if so display notice and terminate
  if( GetVersion() & 0x80000000 ) {
    MessageBox( NULL,
      TEXT( "Sorry, MinRec requires Windows NT.\n" )
      TEXT( "This application will now terminate." ),
      TEXT( "Windows NT Required!"                 ),
      MB_OK );
    return( -1 );
  }

  Global.hInstance        = hInstance;
  Global.lpszApiFailedMsg = TEXT( "A Windows API Failed" );

  //-- register the window class
  {
    WNDCLASS WndClass;

    WndClass.style         = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc   = (WNDPROC) MainDlgProc;
    WndClass.cbClsExtra    = (int) NULL;
    WndClass.cbWndExtra    = DLGWINDOWEXTRA;
    WndClass.hInstance     = hInstance;
    WndClass.hIcon         = LoadIcon( Global.hInstance, lpszIconName );
    WndClass.hCursor       = LoadCursor( NULL, IDC_ARROW );
    WndClass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE+1);
    WndClass.lpszMenuName  = lpszMenuName;
    WndClass.lpszClassName = lpszClassName;

    if( !RegisterClass(&WndClass) ) {
      ErrorMessageBox( TEXT("RegisterClass()"),
        Global.lpszApiFailedMsg, lpszSourceFileName, __LINE__ );
      return( FALSE );
    }
  }

  //-- Create a main dialog window for this application instance
  Global.hDlgMain = CreateDialog( Global.hInstance, TEXT("MainDlgBox"),
                      NULL, NULL );

  //-- If window could not be created, return "failure"
  if( !Global.hDlgMain ) {
    ErrorMessageBox( TEXT("CreateDialog()"),
      Global.lpszApiFailedMsg, lpszSourceFileName, __LINE__ );
    return( FALSE );
  }

  //-- Load main menu accelerators
  if( !(hAccel = LoadAccelerators( Global.hInstance, lpszAccelName) ) ) {
    ErrorMessageBox( TEXT("LoadAccelerators()"),
      Global.lpszApiFailedMsg, lpszSourceFileName, __LINE__ );
    return( FALSE );
  }

  //-- Make the window visible; update its client area; and return "success"
  ShowWindow( Global.hDlgMain, SW_SHOWDEFAULT );  // Show the window

  //-- Acquire and dispatch messages until a WM_QUIT message is received.
  while( GetMessage( &Msg, NULL, 0, 0 ) ) {
    if( !TranslateAccelerator( Global.hDlgMain, hAccel, &Msg ) ) {
      if( !IsDialogMessage( Global.hDlgMain, &Msg ) ) {
        TranslateMessage( &Msg );     // Translates virtual key codes
        DispatchMessage( &Msg );      // Dispatches message to window
      }
    }
  }

  return( Msg.wParam );           // Returns the value from PostQuitMessage
  UNREFERENCED_PARAMETER( lpszCmdLine );  // avoid the warning
}


// ************************************************************************
// FUNCTION : MainDlgProc( HWND, UINT, WPARAM, LPARAM )
// PURPOSE  : Processes messages
// MESSAGES :
//   WM_COMMAND         - application menu
//     IDM_FILE_EXIT    - exit the application
//     IDM_HELP_ABOUT   - About Dialog Box
//     ...
//   WM_CREATE          - window initialization
//   WM_CLOSE           - handles cleanup
//   WM_DESTROY         - destroys window
// ************************************************************************
BOOL CALLBACK
MainDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  switch( uMsg ) {

    case WM_COMMAND:
      switch( LOWORD(wParam) ) {

        case IDM_FILE_EXIT:
          PostMessage( hWnd, WM_CLOSE, (WPARAM) 0, (LPARAM) 0 );
          return( TRUE );

        case IDM_MACRO_PLAYBACK:
          StartJournalPlayback();
          return( TRUE );

        case IDM_MACRO_RECORD:
          StartJournalRecord();
          return( TRUE );

        case IDM_MACRO_STOP:
          StopAllJournalling();
          return( TRUE );

        case IDM_HELP_ABOUT:
          if( DialogBox( Global.hInstance, TEXT( "AboutDlgBox" ),
                hWnd, AboutDlgProc ) == -1 )
            ErrorMessageBox( TEXT("DialogBox()"),
              Global.lpszApiFailedMsg, lpszSourceFileName, __LINE__ );
          return( TRUE );

      }
      break;

    case WM_SYSCOMMAND:
      switch( LOWORD(wParam) ) {

        // return a nonzero value to disable the screen saver
        case SC_SCREENSAVE:
          return( TRUE );

      }
      break;

    case WM_INITDIALOG:
      return( TRUE );

    case WM_CLOSE:
      DestroyWindow( Global.hDlgMain );
      Global.hDlgMain = NULL;
      PostQuitMessage( FALSE );
  }

  return( DefDlgProc( hWnd, uMsg, wParam, lParam )  );
}


// ************************************************************************
// FUNCTION : AboutDlgProc( HWND, UINT, WPARAM, LPARAM )
// PURPOSE  : Processes messages for "About" dialog box
// MESSAGES :
//   WM_INITDIALOG - initialize dialog box
//   WM_COMMAND    - Input received
//     IDOK        - OK button selected
//     IDCANCEL    - Cancel button selected
// COMMENTS:
//   No initialization is needed for this particular dialog box.
//   In this case, TRUE must be returned to Windows.
//   Wait for user to click on "Ok" button, then close the dialog box.
// ************************************************************************
BOOL CALLBACK
AboutDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  switch( uMsg ) {

    case WM_COMMAND:
      switch( LOWORD(wParam) ) {

        case IDOK:
          EndDialog( hDlg, TRUE );
          return( TRUE );

        case IDCANCEL:
          EndDialog( hDlg, FALSE );
          return( TRUE );
      }
      break;

    case WM_INITDIALOG:
      return( TRUE );

    case WM_CLOSE:
      return( TRUE );

  }

  return( FALSE );
  UNREFERENCED_PARAMETER( lParam );
}


// ************************************************************************
// FUNCTION : ErrorMessageBox( LPCTSTR, LPCTSTR, LPCTSTR, INT )
// PURPOSE  : Displays an error message box with various error information
//            and allows the user to terminate or continue the process.
//            For a Win32 Application, GetLastError and FormatMessage are
//            user to retrieve the last API error code and error message.
// COMMENTS :
// ************************************************************************
BOOL
ErrorMessageBox( LPCTSTR lpszText, LPCTSTR lpszTitle, LPCTSTR lpszFile,
  INT Line )
{
  #define ERROR_BUFFER_SIZE 512

  static TCHAR szMessageFormat[] =
    TEXT( "%s\n"                                           )
    TEXT( "\n"                                             )
    TEXT( "-- Error Information --\n"                      )
    TEXT( "File : %s\n"                                    )
    TEXT( "Line : %d\n"                                    )
    TEXT( "Error Number : %d\n"                            )
    TEXT( "Error Message : %s\n"                           )
    TEXT( "\n"                                             )
    TEXT( "Press OK to terminate this application.\n"      )
    TEXT( "Press Cancel to ignore the error and continue." );

  LPTSTR lpFormatMessageBuffer;
  DWORD  dwFormatMessage;
  DWORD  dwGetLastError;
  HLOCAL hMessageBoxBuffer;
  LPVOID lpMessageBoxBuffer;

  //-- get the system error
  dwGetLastError = GetLastError();

  //-- perform a simple check on the needed buffer size
  if( lstrlen(lpszText) > (ERROR_BUFFER_SIZE - lstrlen(szMessageFormat)) )
    return( FALSE );

  //-- allocate the message box buffer
  hMessageBoxBuffer  = LocalAlloc( LMEM_FIXED, ERROR_BUFFER_SIZE );
  lpMessageBoxBuffer = LocalLock( hMessageBoxBuffer );

  //-- get the system error message
  dwFormatMessage = FormatMessage(
                      FORMAT_MESSAGE_ALLOCATE_BUFFER
                      | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, dwGetLastError, LANG_NEUTRAL,
                      (LPTSTR) &lpFormatMessageBuffer, 0, NULL );
  if( !dwFormatMessage )
    lpFormatMessageBuffer = TEXT("FormatMessage() failed!");

  //-- format the error messge box string
  wsprintf( lpMessageBoxBuffer, szMessageFormat, lpszText, lpszFile,
    Line, dwGetLastError, lpFormatMessageBuffer );

  // -- display the error and allow the user to terminate or continue
  if( MessageBox( NULL, lpMessageBoxBuffer, lpszTitle,
        MB_APPLMODAL | MB_ICONSTOP | MB_OKCANCEL ) == IDOK )
    ExitProcess( 0 );

  //-- free all buffers
  if( dwFormatMessage )
    LocalFree( (HLOCAL) lpFormatMessageBuffer );
  LocalFree( (HLOCAL) hMessageBoxBuffer );

  return( TRUE );
}
