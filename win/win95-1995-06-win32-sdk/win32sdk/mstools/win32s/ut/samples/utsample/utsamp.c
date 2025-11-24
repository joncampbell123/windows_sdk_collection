/********************************************************************\
*  UTSamp.c -- Sample program demonstrating Universal Thunks under   *
*              Win32s, including calling APIs unsupported by         *
*              Win32s.                                               *
*                                                                    *
*  Lee Hart                                                          *
*  Microsoft Developer Support                                       *
*  Copyright (c) 1993-1994 Microsoft Corporation                     *
*                                                                    *
*  Comments:                                                         *
*                                                                    *
*  Shows how to use Universal Thunks to call APIs that are not       *
*  available directly through Win32s.                                *
*                                                                    *
*  Modifications made by Julie Solon and Lee Hart after initial      *
*  sample release to demonstrate the callback from the 16-bit side.  *
*                                                                    *
*  Functions:                                                        *
*                                                                    *
*  WinMain()         - Initializes Application                       *
*  MainWndProc()     - Processes Application Messages                *
*  AboutDlgProc()    - Processes "About" Dialog Box Messages         *
*                                                                    *
*                                                                    *
\********************************************************************/


/*********************  Header Files  *********************/

#include <windows.h>
#include "utsamp.h"

/**********************  Prototypes  **********************/

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK AboutDlgProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );

/*******************  Global Variables  *******************/

HANDLE ghInstance;

/********************************************************************\
*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
*                                                                    *
*   Purpose: Initializes Application, sets up Universal Thunk if     *
*            running under Win32s (as determined by the high bit of  *
*            GetVersion().                                           *
*                                                                    *
\********************************************************************/


int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine,
                    int nCmdShow )
{
   WNDCLASS wc;
   MSG msg;
   HWND hWnd;

   if( !hPrevInstance )
   {
      wc.lpszClassName = "UTSampClass";
      wc.lpfnWndProc = MainWndProc;
      wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
      wc.hInstance = hInstance;
      wc.hIcon = LoadIcon( hInstance, "UTSampIcon" );
      wc.hCursor = LoadCursor( NULL, IDC_ARROW );
      wc.hbrBackground = (HBRUSH)( COLOR_WINDOW+1 );
      wc.lpszMenuName = "UTSampMenu";
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;

      RegisterClass( &wc );
   }

   ghInstance = hInstance;

   hWnd = CreateWindow( "UTSampClass",
                        "Universal Thunks Sample",
                        WS_OVERLAPPEDWINDOW,
                        0,
                        0,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

   ShowWindow( hWnd, nCmdShow );

   while( GetMessage( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }

   return msg.wParam;
} // WinMain()


/********************************************************************\
* Function: LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM) *
*                                                                    *
*  Purpose: Processes Application Messages                           *
*                                                                    *
* Comments: The following messages are processed                     *
*                                                                    *
*           WM_COMMAND                                               *
*           WM_DESTROY                                               *
*                                                                    *
*                                                                    *
\********************************************************************/


LRESULT CALLBACK MainWndProc( HWND hWnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam )
{
   switch( msg )
   {

/********************************************************************\
*     WM_COMMAND: Handle menu selections for Exit, GetFreeSpace,     *
*        WNetGetUser, and Help About                                 *
\********************************************************************/

      case WM_COMMAND:
         switch( wParam )
         {
            case IDM_EXIT:
               SendMessage( hWnd, WM_CLOSE, 0, 0 );
               break;

            case IDM_GETUSER:
            {
               CHAR buf1[255], buf2[255];
	       UINT bufsize = 255;

               MyWNetGetUser( NULL, buf1, &bufsize );
               wsprintf( buf2, "User is %s", buf1 );
               MessageBox( hWnd, buf2, "UTSamp", MB_OK );
               break;
            }

            case IDM_GETMEM:
            {
               CHAR buf[255];

               wsprintf( buf, "Free Ram = 0x%x", MyGetFreeSpace() );
               MessageBox( hWnd, buf, "UTSamp", MB_OK );
               break;
            }

            case IDM_CALLBACK1:
               GenerateCallback( 1 );
               break;

            case IDM_CALLBACK2:
               GenerateCallback( 2 );
               break;

            case IDM_ABOUT:
               DialogBox( ghInstance,
                          "AboutDlg",
                          hWnd,
                          (DLGPROC) AboutDlgProc );
               break;
         }
         break;

/**************************************************************\
*     WM_DESTROY: PostQuitMessage() is called                  *
\**************************************************************/

      case WM_DESTROY:
         PostQuitMessage( 0 );
         break;

/**************************************************************\
*     Let the default window proc handle all other messages    *
\**************************************************************/

      default:
         return( DefWindowProc( hWnd, msg, wParam, lParam ) );
   }

   return 0;
} // MainWndProc()

/********************************************************************\
* Function: LRESULT CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM)*
*                                                                    *
*  Purpose: Processes "About" Dialog Box Messages                    *
*                                                                    *
* Comments: The Dialog Box is displayed when the user selects        *
*           Help.About.  The following messages are processed:       *
*                                                                    *
*           WM_INITDIALOG                                            *
*           WM_COMMAND                                               *
*                                                                    *
\********************************************************************/


LRESULT CALLBACK AboutDlgProc( HWND hDlg,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam )
{
   switch( uMsg )
   {
      case WM_INITDIALOG:
         return TRUE;

      case WM_COMMAND:
         switch( wParam )
         {
            case IDOK:
               EndDialog( hDlg, TRUE );
               return TRUE;
         }
         break;
   }

   return FALSE;
} //AboutDlgProc()
