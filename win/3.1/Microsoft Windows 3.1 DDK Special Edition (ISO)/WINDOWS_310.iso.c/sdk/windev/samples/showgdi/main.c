/****************************************************************************
 Main.c

 The Main module handles the ShowGDI main program instance and window.

 The ShowGDI program allows interactive rendering of GDI graphics with 
 an adjustable enlarged view of the results.

 Written by Dave Parker
 (c)Copyright 1991 Microsoft Corporation

****************************************************************************/

#include "windows.h"

#include "MENU.h"

#include "util.h"
#include "dialogs.h"
#include "view.h"
#include "dc.h"
#include "draw.h"
#include "dib.h"

#include "main.h"


/****************************************************************************
   Globals
****************************************************************************/

HANDLE   hInst;             /* current instance */
HWND     hwndMain;          /* main program window */
HANDLE   hAccTable;         /* handle to accelerator table */


/****************************************************************************
   Functions
****************************************************************************/

int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, 
                    int nCmdShow )
/* Main program entry point. */
{
   MSG msg;

   /* Initialize */
   if (!hPrevInstance) {
      if (!InitApplication( hInstance ))
         return FALSE;
   }
   if (!InitInstance( hInstance, nCmdShow ))
      return (FALSE);

   /* Event loop */
   while (GetMessage( &msg, NULL, NULL, NULL )) {
      if (!TranslateAccelerator( hwndMain, hAccTable, &msg )) {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }

   /* Cleanup and exit */
   Fini();
   return msg.wParam;
}


BOOL InitApplication( HANDLE hInstance )
/* Initialize the application. */
{
   WNDCLASS  wc;

   /* Register the main window class */
   wc.style = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = MainWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(hInstance, "ShowGDIIcon");
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = COLOR_WINDOW+1;
   wc.lpszMenuName =  "ShowGDIMenu";
   wc.lpszClassName = "ShowGDIClass";
   if (!RegisterClass(&wc))
       return FALSE;

   return TRUE;
}


BOOL InitInstance( HANDLE hInstance, int nCmdShow )
/* Initialize this instance of the application. */
{
   /* Record instance and accelerator table in globals */
   hInst = hInstance;
   hAccTable = LoadAccelerators(hInst, "ShowGDIAcc");

   /* Create the main window and record in global */
   hwndMain = CreateWindow(
       "ShowGDIClass",        /* See RegisterClass() call.          */
       "ShowGDI",             /* Text for window title bar.         */
       WS_OVERLAPPEDWINDOW,   /* Window style.                      */
       CW_USEDEFAULT,         /* Default horizontal position.       */
       CW_USEDEFAULT,         /* Default vertical position.         */
       CW_USEDEFAULT,         /* Default width.                     */
       CW_USEDEFAULT,         /* Default height.                    */
       NULL,                  /* Overlapped windows have no parent. */
       NULL,                  /* Use the window class menu.         */
       hInstance,             /* This instance owns this window.    */
       NULL                   /* Pointer not needed.                */
   );
   if (!hwndMain)
      return (FALSE);

   /* Initialize the view and the DC settings */
   if (!NewView())
      return FALSE;
   ReadDC( drawDC );

   /* Show the window and update it */
   ShowWindow( hwndMain, nCmdShow );
   UpdateWindow( hwndMain );

   return (TRUE);             
}


void Fini( void )
/* Cleanup and prepare to quit */
{
   if (curDIB != NULL)
      GlobalFree( curDIB );
}


long FAR PASCAL MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
/* Window handler for main application window */
{
   HDC         hdc;
   PAINTSTRUCT ps;

   switch (message) {
      case WM_COMMAND:
         switch( MenuGroupFromID( wParam ) ) {
            case VIEW_MENU_GROUP:  ViewCmd( wParam );  return 0L;
            case DC_MENU_GROUP:    DCCmd( wParam );    return 0L;
            case DRAW_MENU_GROUP:  DrawCmd( wParam );  return 0L;
            case HELP_MENU_GROUP:  HelpCmd( wParam );  return 0L;
            default:
               return (DefWindowProc( hWnd, message, wParam, lParam ));
         }
         break;

      case WM_INITMENU:
         CheckViewMenuItems( (HMENU) wParam );
         CheckDCMenuItems( (HMENU) wParam );
         break;

      case WM_PAINT:
         hdc = BeginPaint( hWnd, &ps );
         PaintView( hdc, ps.rcPaint );
         EndPaint( hWnd, &ps );
         break;

      case WM_DESTROY:
         PostQuitMessage( 0 );
         break;

      default:
         return (DefWindowProc( hWnd, message, wParam, lParam ));
   }
   return 0L;
}


void HelpCmd( int item )
/* Process the command 'item' from the Help menu. */
{
   switch (item) {
      case IDM_ABOUT:
         Dlg( AboutDlg );
         break;
   }
}
