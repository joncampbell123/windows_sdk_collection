//---------------------------------------------------------------------------
//
//  Module: tty.c
//
//  Purpose:
//     The sample application demonstrates the usage of the COMM
//     API.  It implements the new COMM API of Windows 3.1.
//
//     NOTE:  no escape sequences are translated, only
//            the necessary control codes (LF, CR, BS, etc.)
//
//  Description of functions:
//     Descriptions are contained in the function headers.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//
//              5/ 8/91   BryanW      Wrote it.
//
//             10/18/91   BryanW      Sanitized code & comments.
//
//             10/22/91   BryanW      Minor bug fixes.
//
//              2/10/92   BryanW      Fixed a problem with ReadCommBlock()
//                                    and error detection.  ReadComm() is
//                                    now performed... if the result is < 0
//                                    the error status is read using
//                                    GetCommError() and displayed.  The
//                                    length is adjusted (*= -1).
//
//              2/13/92   BryanW      Updated to provide the option to
//                                    implement/handle CN_RECEIVE
//                                    notifications.
//
//              2/13/92   BryanW      Implemented forced DTR control. DTR
//                                    is asserted on connect and dropped
//                                    on disconnect.
//
//              2/21/92   BryanW      Removed overhead of local memory
//                                    functions - now uses LPTR for
//                                    allocation from heap.
//
//              2/26/92   BryanW      Fixed off by one problem in paint
//                                    calculations.
//
//---------------------------------------------------------------------------
//
//  Written by Microsoft Product Support Services, Windows Developer Support.
//  Copyright (c) 1991 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include "tty.h"

//---------------------------------------------------------------------------
//  int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
//                      LPSTR lpszCmdLine, int nCmdShow )
//
//  Description:
//     This is the main window loop!
//
//  Parameters:
//     As documented for all WinMain() functions.
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow )
{
   HWND  hTTYWnd ;
   MSG   msg ;

   if (!hPrevInstance)
      if (!InitApplication( hInstance ))
         return ( FALSE ) ;

   if (NULL == (hTTYWnd = InitInstance( hInstance, nCmdShow )))
      return ( FALSE ) ;

   while (GetMessage( &msg, NULL, 0, 0 ))
   {
      if (!TranslateAccelerator( hTTYWnd, ghAccel, &msg ))
      {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
      }
   }
   return ( (int) msg.wParam ) ;

} // end of WinMain()

//---------------------------------------------------------------------------
//  BOOL NEAR InitApplication( HANDLE hInstance )
//
//  Description:
//     First time initialization stuff.  This registers information
//     such as window classes.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to this instance of the application.
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//             10/22/91   BryanW      Fixed background color problem.
//
//---------------------------------------------------------------------------

BOOL NEAR InitApplication( HANDLE hInstance )
{
   WNDCLASS  wndclass ;

   // register tty window class

   wndclass.style =         NULL ;
   wndclass.lpfnWndProc =   TTYWndProc ;
   wndclass.cbClsExtra =    0 ;
   wndclass.cbWndExtra =    sizeof( WORD ) ;
   wndclass.hInstance =     hInstance ;
   wndclass.hIcon =         LoadIcon( hInstance, MAKEINTRESOURCE( TTYICON ) );
   wndclass.hCursor =       LoadCursor( NULL, IDC_ARROW ) ;
   wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
   wndclass.lpszMenuName =  MAKEINTRESOURCE( TTYMENU ) ;
   wndclass.lpszClassName = gszTTYClass ;

   return( RegisterClass( &wndclass ) ) ;

} // end of InitApplication()

//---------------------------------------------------------------------------
//  HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
//
//  Description:
//     Initializes instance specific information.
//
//  Parameters:
//     HANDLE hInstance
//        Handle to instance
//
//     int nCmdShow
//        How do we show the window?
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

HWND NEAR InitInstance( HANDLE hInstance, int nCmdShow )
{
   HWND  hTTYWnd ;

   // load accelerators
   ghAccel = LoadAccelerators( hInstance, MAKEINTRESOURCE( TTYACCEL ) ) ;

   // create the TTY window
   hTTYWnd = CreateWindow( gszTTYClass, gszAppName,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           NULL, NULL, hInstance, NULL ) ;

   if (NULL == hTTYWnd)
      return ( NULL ) ;

   ShowWindow( hTTYWnd, nCmdShow ) ;
   UpdateWindow( hTTYWnd ) ;

   return ( hTTYWnd ) ;

} // end of InitInstance()

//---------------------------------------------------------------------------
//  LRESULT FAR PASCAL TTYWndProc( HWND hWnd, UINT uMsg,
//                                 WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This is the TTY Window Proc.  This handles ALL messages
//     to the tty window.
//
//  Parameters:
//     As documented for Window procedures.
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

LRESULT FAR PASCAL TTYWndProc( HWND hWnd, UINT uMsg,
                               WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_CREATE:
         return ( CreateTTYInfo( hWnd ) ) ;

      case WM_COMMAND:
      {
         switch ((WORD) wParam)
         {
            case IDM_CONNECT:
               if (!OpenConnection( hWnd ))
                  MessageBox( hWnd, "Connection failed.", gszAppName,
                              MB_ICONEXCLAMATION ) ;
               break ;

            case IDM_DISCONNECT:
               CloseConnection( hWnd ) ;
               break ;

            case IDM_SETTINGS:
            {
               NPTTYINFO  npTTYInfo ;

               if (NULL == (npTTYInfo =
                               (NPTTYINFO) GetWindowWord( hWnd,
                                                          GWW_NPTTYINFO )))
                  return ( FALSE ) ;
               GoModalDialogBoxParam( GETHINST( hWnd ),
                                      MAKEINTRESOURCE( SETTINGSDLGBOX ), hWnd,
                                      SettingsDlgProc,
                                      (LPARAM) (LPSTR) npTTYInfo ) ;

               // if fConnected, set new COM parameters

               if (CONNECTED( npTTYInfo ))
               {
                  if (!SetupConnection( hWnd ))
                     MessageBox( hWnd, "Settings failed!", gszAppName,
                                 MB_ICONEXCLAMATION ) ;
               }
            }
            break ;

            case IDM_ABOUT:
               GoModalDialogBoxParam ( GETHINST( hWnd ),
                                       MAKEINTRESOURCE( ABOUTDLGBOX ),
                                       hWnd,
                                       AboutDlgProc, NULL ) ;
               break;

            case IDM_EXIT:
               PostMessage( hWnd, WM_CLOSE, NULL, 0L ) ;
               break ;
         }
      }
      break ;

      case WM_COMMNOTIFY:
         ProcessCOMMNotification( hWnd, (WORD) wParam, (LONG) lParam ) ;
         break ;

      case WM_PAINT:
         PaintTTY( hWnd ) ;
         break ;

      case WM_SIZE:
         SizeTTY( hWnd, HIWORD( lParam ), LOWORD( lParam ) ) ;
         break ;

      case WM_HSCROLL:
         ScrollTTYHorz( hWnd, (WORD) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_VSCROLL:
         ScrollTTYVert( hWnd, (WORD) wParam, LOWORD( lParam ) ) ;
         break ;

      case WM_CHAR:
         ProcessTTYCharacter( hWnd, LOBYTE( wParam ) ) ;
         break ;

      case WM_SETFOCUS:
         SetTTYFocus( hWnd ) ;
         break ;

      case WM_KILLFOCUS:
         KillTTYFocus( hWnd ) ;
         break ;

      case WM_DESTROY:
         DestroyTTYInfo( hWnd ) ;
         PostQuitMessage( 0 ) ;
         break ;

      case WM_CLOSE:
         if (IDOK != MessageBox( hWnd, "OK to close window?", "TTY Sample",
                                 MB_ICONQUESTION | MB_OKCANCEL ))
            break ;

         // fall through

      default:
         return( DefWindowProc( hWnd, uMsg, wParam, lParam ) ) ;
   }
   return 0L ;

} // end of TTYWndProc()

//---------------------------------------------------------------------------
//  LRESULT NEAR CreateTTYInfo( HWND hWnd )
//
//  Description:
//     Creates the tty information structure and sets
//     menu option availability.  Returns -1 if unsuccessful.
//
//  Parameters:
//     HWND  hWnd
//        Handle to main window.
//
//  History:   Date       Author      Comment
//             10/18/91   BryanW      Pulled from tty window proc.
//              1/13/92   BryanW      Fixed bug with invalid handle
//                                    caused by WM_SIZE sent by
//                                    ResetTTYScreen().
//
//---------------------------------------------------------------------------

LRESULT NEAR CreateTTYInfo( HWND hWnd )
{
   HMENU       hMenu ;
   NPTTYINFO   npTTYInfo ;

   if (NULL == (npTTYInfo =
                   (NPTTYINFO) LocalAlloc( LPTR, sizeof( TTYINFO ) )))
      return ( (LRESULT) -1 ) ;

   // initialize TTY info structure

   COMDEV( npTTYInfo )        = 0 ;
   CONNECTED( npTTYInfo )     = FALSE ;
   CURSORSTATE( npTTYInfo )   = CS_HIDE ;
   LOCALECHO( npTTYInfo )     = FALSE ;
   AUTOWRAP( npTTYInfo )      = TRUE ;
   PORT( npTTYInfo )          = 1 ;
   BAUDRATE( npTTYInfo )      = CBR_9600 ;
   BYTESIZE( npTTYInfo )      = 8 ;
   FLOWCTRL( npTTYInfo )      = FC_RTSCTS ;
   PARITY( npTTYInfo )        = NOPARITY ;
   STOPBITS( npTTYInfo )      = ONESTOPBIT ;
   XONXOFF( npTTYInfo )       = FALSE ;
   XSIZE( npTTYInfo )         = 0 ;
   YSIZE( npTTYInfo )         = 0 ;
   XSCROLL( npTTYInfo )       = 0 ;
   YSCROLL( npTTYInfo )       = 0 ;
   XOFFSET( npTTYInfo )       = 0 ;
   YOFFSET( npTTYInfo )       = 0 ;
   COLUMN( npTTYInfo )        = 0 ;
   ROW( npTTYInfo )           = 0 ;
   HTTYFONT( npTTYInfo )      = NULL ;
   FGCOLOR( npTTYInfo )       = RGB( 0, 0, 0 ) ;
   USECNRECEIVE( npTTYInfo )  = TRUE ;
   DISPLAYERRORS( npTTYInfo ) = TRUE ;

   // clear screen space

   _fmemset( SCREEN( npTTYInfo ), ' ', MAXROWS * MAXCOLS ) ;

   // setup default font information

   LFTTYFONT( npTTYInfo ).lfHeight =         12 ;
   LFTTYFONT( npTTYInfo ).lfWidth =          0 ;
   LFTTYFONT( npTTYInfo ).lfEscapement =     0 ;
   LFTTYFONT( npTTYInfo ).lfOrientation =    0 ;
   LFTTYFONT( npTTYInfo ).lfWeight =         0 ;
   LFTTYFONT( npTTYInfo ).lfItalic =         0 ;
   LFTTYFONT( npTTYInfo ).lfUnderline =      0 ;
   LFTTYFONT( npTTYInfo ).lfStrikeOut =      0 ;
   LFTTYFONT( npTTYInfo ).lfCharSet =        OEM_CHARSET ;
   LFTTYFONT( npTTYInfo ).lfOutPrecision =   OUT_DEFAULT_PRECIS ;
   LFTTYFONT( npTTYInfo ).lfClipPrecision =  CLIP_DEFAULT_PRECIS ;
   LFTTYFONT( npTTYInfo ).lfQuality =        DEFAULT_QUALITY ;
   LFTTYFONT( npTTYInfo ).lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
   LFTTYFONT( npTTYInfo ).lfFaceName[0] =    NULL ;

   // set TTYInfo handle before any further message processing.

   SetWindowWord( hWnd, GWW_NPTTYINFO, (WPARAM) npTTYInfo ) ;

   // reset the character information, etc.

   ResetTTYScreen( hWnd, npTTYInfo ) ;

   hMenu = GetMenu( hWnd ) ;
   EnableMenuItem( hMenu, IDM_DISCONNECT,
                   MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
   EnableMenuItem( hMenu, IDM_CONNECT, MF_ENABLED | MF_BYCOMMAND ) ;

   return ( (LRESULT) TRUE ) ;

} // end of CreateTTYInfo()

//---------------------------------------------------------------------------
//  BOOL NEAR DestroyTTYInfo( HWND hWnd )
//
//  Description:
//     Destroys block associated with TTY window handle.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR DestroyTTYInfo( HWND hWnd )
{
   NPTTYINFO npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   // force connection closed (if not already closed)

   if (CONNECTED( npTTYInfo ))
      CloseConnection( hWnd ) ;

   DeleteObject( HTTYFONT( npTTYInfo ) ) ;

   LocalFree( npTTYInfo ) ;
   return ( TRUE ) ;

} // end of DestroyTTYInfo()

//---------------------------------------------------------------------------
//  BOOL NEAR ResetTTYScreen( HWND hWnd, NPTTYINFO npTTYInfo )
//
//  Description:
//     Resets the TTY character information and causes the
//     screen to resize to update the scroll information.
//
//  Parameters:
//     NPTTYINFO  npTTYInfo
//        pointer to TTY info structure
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ResetTTYScreen( HWND hWnd, NPTTYINFO npTTYInfo )
{
   HDC         hDC ;
   TEXTMETRIC  tm ;
   RECT        rcWindow ;

   if (NULL == npTTYInfo)
      return ( FALSE ) ;

   if (NULL != HTTYFONT( npTTYInfo ))
      DeleteObject( HTTYFONT( npTTYInfo ) ) ;

   HTTYFONT( npTTYInfo ) = CreateFontIndirect( &LFTTYFONT( npTTYInfo ) ) ;

   hDC = GetDC( hWnd ) ;
   SelectObject( hDC, HTTYFONT( npTTYInfo ) ) ;
   GetTextMetrics( hDC, &tm ) ;
   ReleaseDC( hWnd, hDC ) ;

   XCHAR( npTTYInfo ) = tm.tmAveCharWidth  ;
   YCHAR( npTTYInfo ) = tm.tmHeight + tm.tmExternalLeading ;

   // a slimy hack to force the scroll position, region to
   // be recalculated based on the new character sizes

   GetWindowRect( hWnd, &rcWindow ) ;
   SendMessage( hWnd, WM_SIZE, SIZENORMAL,
                (LPARAM) MAKELONG( rcWindow.right - rcWindow.left,
                                   rcWindow.bottom - rcWindow.top ) ) ;

   return ( TRUE ) ;

} // end of ResetTTYScreen()

//---------------------------------------------------------------------------
//  BOOL NEAR PaintTTY( HWND hWnd )
//
//  Description:
//     Paints the rectangle determined by the paint struct of
//     the DC.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window (as always)
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//             10/22/91   BryanW      Problem with background color
//                                    and "off by one" fixed.
//
//              2/25/92   BryanW      Off-by-one not quite fixed...
//                                    also resolved min/max problem
//                                    for windows extended beyond
//                                    the "TTY display".
//
//---------------------------------------------------------------------------

BOOL NEAR PaintTTY( HWND hWnd )
{
   int          nRow, nCol, nEndRow, nEndCol, nCount, nHorzPos, nVertPos ;
   HDC          hDC ;
   HFONT        hOldFont ;
   NPTTYINFO    npTTYInfo ;
   PAINTSTRUCT  ps ;
   RECT         rect ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   hDC = BeginPaint( hWnd, &ps ) ;
   hOldFont = SelectObject( hDC, HTTYFONT( npTTYInfo ) ) ;
   SetTextColor( hDC, FGCOLOR( npTTYInfo ) ) ;
   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) ) ;
   rect = ps.rcPaint ;
   nRow =
      min( MAXROWS - 1,
           max( 0, (rect.top + YOFFSET( npTTYInfo )) / YCHAR( npTTYInfo ) ) ) ;
   nEndRow =
      min( MAXROWS - 1,
           ((rect.bottom + YOFFSET( npTTYInfo ) - 1) / YCHAR( npTTYInfo ) ) ) ;
   nCol =
      min( MAXCOLS - 1,
           max( 0, (rect.left + XOFFSET( npTTYInfo )) / XCHAR( npTTYInfo ) ) ) ;
   nEndCol =
      min( MAXCOLS - 1,
           ((rect.right + XOFFSET( npTTYInfo ) - 1) / XCHAR( npTTYInfo ) ) ) ;
   nCount = nEndCol - nCol + 1 ;
   for (; nRow <= nEndRow; nRow++)
   {
      nVertPos = (nRow * YCHAR( npTTYInfo )) - YOFFSET( npTTYInfo ) ;
      nHorzPos = (nCol * XCHAR( npTTYInfo )) - XOFFSET( npTTYInfo ) ;
      rect.top = nVertPos ;
      rect.bottom = nVertPos + YCHAR( npTTYInfo ) ;
      rect.left = nHorzPos ;
      rect.right = nHorzPos + XCHAR( npTTYInfo ) * nCount ;
      SetBkMode( hDC, OPAQUE ) ;
      ExtTextOut( hDC, nHorzPos, nVertPos, ETO_OPAQUE | ETO_CLIPPED, &rect,
                  (LPSTR)( SCREEN( npTTYInfo ) + nRow * MAXCOLS + nCol ),
                  nCount, NULL ) ;
   }
   SelectObject( hDC, hOldFont ) ;
   EndPaint( hWnd, &ps ) ;
   MoveTTYCursor( hWnd ) ;
   return ( TRUE ) ;

} // end of PaintTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR SizeTTY( HWND hWnd, WORD wVertSize, WORD wHorzSize )
//
//  Description:
//     Sizes TTY and sets up scrolling regions.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wVertSize
//        new vertical size
//
//     WORD wHorzSize
//        new horizontal size
//
//  History:   Date       Author      Comment
//              5/ 8/ 91  BryanW      Wrote it
//
//---------------------------------------------------------------------------

BOOL NEAR SizeTTY( HWND hWnd, WORD wVertSize, WORD wHorzSize )
{
   int        nScrollAmt ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   YSIZE( npTTYInfo ) = (int) wVertSize ;
   YSCROLL( npTTYInfo ) = max( 0, (MAXROWS * YCHAR( npTTYInfo )) -
                               YSIZE( npTTYInfo ) ) ;
   nScrollAmt = min( YSCROLL( npTTYInfo ), YOFFSET( npTTYInfo ) ) -
                     YOFFSET( npTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;

   YOFFSET( npTTYInfo ) = YOFFSET( npTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( npTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_VERT, 0, YSCROLL( npTTYInfo ), TRUE ) ;

   XSIZE( npTTYInfo ) = (int) wHorzSize ;
   XSCROLL( npTTYInfo ) = max( 0, (MAXCOLS * XCHAR( npTTYInfo )) -
                                XSIZE( npTTYInfo ) ) ;
   nScrollAmt = min( XSCROLL( npTTYInfo ), XOFFSET( npTTYInfo )) -
                     XOFFSET( npTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   XOFFSET( npTTYInfo ) = XOFFSET( npTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( npTTYInfo ), FALSE ) ;
   SetScrollRange( hWnd, SB_HORZ, 0, XSCROLL( npTTYInfo ), TRUE ) ;

   InvalidateRect( hWnd, NULL, TRUE ) ;

   return ( TRUE ) ;

} // end of SizeTTY()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
//
//  Description:
//     Scrolls TTY window vertically.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wScrollCmd
//        type of scrolling we're doing
//
//     WORD wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYVert( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int        nScrollAmt ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -YOFFSET( npTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = YSCROLL( npTTYInfo ) - YOFFSET( npTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -YSIZE( npTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = YSIZE( npTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -YCHAR( npTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = YCHAR( npTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - YOFFSET( npTTYInfo ) ;
         break ;

      default:
         return ( FALSE ) ;
   }
   if ((YOFFSET( npTTYInfo ) + nScrollAmt) > YSCROLL( npTTYInfo ))
      nScrollAmt = YSCROLL( npTTYInfo ) - YOFFSET( npTTYInfo ) ;
   if ((YOFFSET( npTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -YOFFSET( npTTYInfo ) ;
   ScrollWindow( hWnd, 0, -nScrollAmt, NULL, NULL ) ;
   YOFFSET( npTTYInfo ) = YOFFSET( npTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_VERT, YOFFSET( npTTYInfo ), TRUE ) ;

   return ( TRUE ) ;

} // end of ScrollTTYVert()

//---------------------------------------------------------------------------
//  BOOL NEAR ScrollTTYHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
//
//  Description:
//     Scrolls TTY window horizontally.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wScrollCmd
//        type of scrolling we're doing
//
//     WORD wScrollPos
//        scroll position
//
//  History:   Date       Author      Comment
//              5/ 8/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ScrollTTYHorz( HWND hWnd, WORD wScrollCmd, WORD wScrollPos )
{
   int        nScrollAmt ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   switch (wScrollCmd)
   {
      case SB_TOP:
         nScrollAmt = -XOFFSET( npTTYInfo ) ;
         break ;

      case SB_BOTTOM:
         nScrollAmt = XSCROLL( npTTYInfo ) - XOFFSET( npTTYInfo ) ;
         break ;

      case SB_PAGEUP:
         nScrollAmt = -XSIZE( npTTYInfo ) ;
         break ;

      case SB_PAGEDOWN:
         nScrollAmt = XSIZE( npTTYInfo ) ;
         break ;

      case SB_LINEUP:
         nScrollAmt = -XCHAR( npTTYInfo ) ;
         break ;

      case SB_LINEDOWN:
         nScrollAmt = XCHAR( npTTYInfo ) ;
         break ;

      case SB_THUMBPOSITION:
         nScrollAmt = wScrollPos - XOFFSET( npTTYInfo ) ;
         break ;

      default:
         return ( FALSE ) ;
   }
   if ((XOFFSET( npTTYInfo ) + nScrollAmt) > XSCROLL( npTTYInfo ))
      nScrollAmt = XSCROLL( npTTYInfo ) - XOFFSET( npTTYInfo ) ;
   if ((XOFFSET( npTTYInfo ) + nScrollAmt) < 0)
      nScrollAmt = -XOFFSET( npTTYInfo ) ;
   ScrollWindow( hWnd, -nScrollAmt, 0, NULL, NULL ) ;
   XOFFSET( npTTYInfo ) = XOFFSET( npTTYInfo ) + nScrollAmt ;
   SetScrollPos( hWnd, SB_HORZ, XOFFSET( npTTYInfo ), TRUE ) ;

   return ( TRUE ) ;

} // end of ScrollTTYHorz()

//---------------------------------------------------------------------------
//  BOOL NEAR SetTTYFocus( HWND hWnd )
//
//  Description:
//     Sets the focus to the TTY window also creates caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR SetTTYFocus( HWND hWnd )
{
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   if (CONNECTED( npTTYInfo ) && (CURSORSTATE( npTTYInfo ) != CS_SHOW))
   {
      CreateCaret( hWnd, NULL, XCHAR( npTTYInfo ), YCHAR( npTTYInfo ) ) ;
      ShowCaret( hWnd ) ;
      CURSORSTATE( npTTYInfo ) = CS_SHOW ;
   }
   MoveTTYCursor( hWnd ) ;
   return ( TRUE ) ;

} // end of SetTTYFocus()

//---------------------------------------------------------------------------
//  BOOL NEAR KillTTYFocus( HWND hWnd )
//
//  Description:
//     Kills TTY focus and destroys the caret.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR KillTTYFocus( HWND hWnd )
{
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   if (CONNECTED( npTTYInfo ) && (CURSORSTATE( npTTYInfo ) != CS_HIDE))
   {
      HideCaret( hWnd ) ;
      DestroyCaret() ;
      CURSORSTATE( npTTYInfo ) = CS_HIDE ;
   }
   return ( TRUE ) ;

} // end of KillTTYFocus()

//---------------------------------------------------------------------------
//  BOOL NEAR MoveTTYCursor( HWND hWnd )
//
//  Description:
//     Moves caret to current position.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR MoveTTYCursor( HWND hWnd )
{
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   if (CONNECTED( npTTYInfo ) && (CURSORSTATE( npTTYInfo ) & CS_SHOW))
      SetCaretPos( (COLUMN( npTTYInfo ) * XCHAR( npTTYInfo )) -
                   XOFFSET( npTTYInfo ),
                   (ROW( npTTYInfo ) * YCHAR( npTTYInfo )) -
                   YOFFSET( npTTYInfo ) ) ;
   
   return ( TRUE ) ;

} // end of MoveTTYCursor()

//---------------------------------------------------------------------------
//  BOOL NEAR ProcessCOMMNotification( HWND hWnd, WORD wParam, LONG lParam )
//
//  Description:
//     Processes the WM_COMMNOTIFY message from the COMM.DRV.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     WORD wParam
//        specifes the device (nCid)
//
//     LONG lParam
//        LOWORD contains event trigger
//        HIWORD is NULL
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//             10/18/91   BryanW      Updated to verify the event.
//
//---------------------------------------------------------------------------

BOOL NEAR ProcessCOMMNotification( HWND hWnd, WORD wParam, LONG lParam )
{
   char       szError[ 10 ] ;
   int        nError, nLength ;
   BYTE       abIn[ MAXBLOCK + 1] ;
   COMSTAT    ComStat ;
   NPTTYINFO  npTTYInfo ;
   MSG        msg ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   if (!USECNRECEIVE( npTTYInfo ))
   {
      // verify that it is a COMM event specified by our mask

      if (CN_EVENT & LOWORD( lParam ) != CN_EVENT)
         return ( FALSE ) ;

      // reset the event word so we are notified
      // when the next event occurs

      GetCommEventMask( COMDEV( npTTYInfo ), EV_RXCHAR ) ;

      // We loop here since it is highly likely that the buffer
      // can been filled while we are reading this block.  This
      // is especially true when operating at high baud rates
      // (e.g. >= 9600 baud).

      do
      {
         if (nLength = ReadCommBlock( hWnd, (LPSTR) abIn, MAXBLOCK ))
         {
            WriteTTYBlock( hWnd, (LPSTR) abIn, nLength ) ;

            // force a paint

            UpdateWindow( hWnd ) ;
         }
      }
      while (!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) ||
             (nLength > 0)) ;

   }
   else
   {
      // verify that it is a receive event

      if (CN_RECEIVE & LOWORD( lParam ) != CN_RECEIVE)
         return ( FALSE ) ;

      do
      {
         if (nLength = ReadCommBlock( hWnd, (LPSTR) abIn, MAXBLOCK ))
         {
            WriteTTYBlock( hWnd, (LPSTR) abIn, nLength ) ;

            // force a paint

            UpdateWindow( hWnd ) ;
         }
         if (nError = GetCommError( COMDEV( npTTYInfo ), &ComStat ))
         {
            if (DISPLAYERRORS( npTTYInfo ))
            {
               wsprintf( szError, "<CE-%d>", nError ) ;
               WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
            }
         }
      }
      while ((!PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE )) ||
            (ComStat.cbInQue >= MAXBLOCK)) ;
   }
   return ( TRUE ) ;

} // end of ProcessCOMMNotification()

//---------------------------------------------------------------------------
//  BOOL NEAR ProcessTTYCharacter( HWND hWnd, BYTE bOut )
//
//  Description:
//     This simply writes a character to the port and echos it
//     to the TTY screen if fLocalEcho is set.  Some minor
//     keyboard mapping could be performed here.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     BYTE bOut
//        byte from keyboard
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR ProcessTTYCharacter( HWND hWnd, BYTE bOut )
{
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   if (!CONNECTED( npTTYInfo ))
      return ( FALSE ) ;

   WriteCommByte( hWnd, bOut ) ;
   if (LOCALECHO( npTTYInfo ))
      WriteTTYBlock( hWnd, &bOut, 1 ) ;

   return ( TRUE ) ;

} // end of ProcessTTYCharacter()

//---------------------------------------------------------------------------
//  BOOL NEAR OpenConnection( HWND hWnd )
//
//  Description:
//     Opens communication port specified in the TTYINFO struct.
//     It also sets the CommState and notifies the window via
//     the fConnected flag in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR OpenConnection( HWND hWnd )
{
   char       szPort[ 10 ], szTemp[ 10 ] ;
   BOOL       fRetVal ;
   HCURSOR    hOldCursor, hWaitCursor ;
   HMENU      hMenu ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   // show the hourglass cursor
   hWaitCursor = LoadCursor( NULL, IDC_WAIT ) ;
   hOldCursor = SetCursor( hWaitCursor ) ;

   // load the COM prefix string and append port number

   LoadString( GETHINST( hWnd ), IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;
   wsprintf( szPort, "%s%d", (LPSTR) szTemp, PORT( npTTYInfo ) ) ;

   // open COMM device

   if ((COMDEV( npTTYInfo ) = OpenComm( szPort, RXQUEUE, TXQUEUE )) < 0)
      return ( FALSE ) ;

   fRetVal = SetupConnection( hWnd ) ;

   if (fRetVal)
   {
      CONNECTED( npTTYInfo ) = TRUE ;

      // set up notifications from COMM.DRV

      if (!USECNRECEIVE( npTTYInfo ))
      {
         // In this case we really are only using the notifications
         // for the received characters - it could be expanded to
         // cover the changes in CD or other status lines.

         SetCommEventMask( COMDEV( npTTYInfo ), EV_RXCHAR ) ;

         // Enable notifications for events only.
      
         // NB:  This method does not use the specific
         // in/out queue triggers.

         EnableCommNotification( COMDEV( npTTYInfo ), hWnd, -1, -1 ) ;
      }
      else
      {
         // Enable notification for CN_RECEIVE events.

         EnableCommNotification( COMDEV( npTTYInfo ), hWnd, MAXBLOCK, -1 ) ;
      }

      // assert DTR

      EscapeCommFunction( COMDEV( npTTYInfo ), SETDTR ) ;


      SetTTYFocus( hWnd ) ;

      hMenu = GetMenu( hWnd ) ;
      EnableMenuItem( hMenu, IDM_DISCONNECT,
                      MF_ENABLED | MF_BYCOMMAND ) ;
      EnableMenuItem( hMenu, IDM_CONNECT,
                      MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
   }
   else
   {
      CONNECTED( npTTYInfo ) = FALSE ;
      CloseComm( COMDEV( npTTYInfo ) ) ;
   }

   // restore cursor
   SetCursor( hOldCursor ) ;

   return ( fRetVal ) ;

} // end of OpenConnection()

//---------------------------------------------------------------------------
//  BOOL NEAR SetupConnection( HWND hWnd )
//
//  Description:
//     This routines sets up the DCB based on settings in the
//     TTY info structure and performs a SetCommState().
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR SetupConnection( HWND hWnd )
{
   BOOL       fRetVal ;
   BYTE       bSet ;
   DCB        dcb ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   GetCommState( COMDEV( npTTYInfo ), &dcb ) ;

   dcb.BaudRate = BAUDRATE( npTTYInfo ) ;
   dcb.ByteSize = BYTESIZE( npTTYInfo ) ;
   dcb.Parity = PARITY( npTTYInfo ) ;
   dcb.StopBits = STOPBITS( npTTYInfo ) ;

   // setup hardware flow control

   bSet = (BYTE) ((FLOWCTRL( npTTYInfo ) & FC_DTRDSR) != 0) ;
   dcb.fOutxDsrFlow = dcb.fDtrflow = bSet ;
   dcb.DsrTimeout = (bSet) ? 30 : 0 ;

   bSet = (BYTE) ((FLOWCTRL( npTTYInfo ) & FC_RTSCTS) != 0) ;
   dcb.fOutxCtsFlow = dcb.fRtsflow = bSet ;
   dcb.CtsTimeout = (bSet) ? 30 : 0 ;

   // setup software flow control

   bSet = (BYTE) ((FLOWCTRL( npTTYInfo ) & FC_XONXOFF) != 0) ;

   dcb.fInX = dcb.fOutX = bSet ;
   dcb.XonChar = ASCII_XON ;
   dcb.XoffChar = ASCII_XOFF ;
   dcb.XonLim = 100 ;
   dcb.XoffLim = 100 ;

   // other various settings

   dcb.fBinary = TRUE ;
   dcb.fParity = TRUE ;
   dcb.fRtsDisable = FALSE ;
   dcb.fDtrDisable = FALSE ;

   fRetVal = !(SetCommState( &dcb ) < 0) ;

   return ( fRetVal ) ;

} // end of SetupConnection()

//---------------------------------------------------------------------------
//  BOOL NEAR CloseConnection( HWND hWnd )
//
//  Description:
//     Closes the connection to the port.  Resets the connect flag
//     in the TTYINFO struct.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR CloseConnection( HWND hWnd )
{
   HMENU      hMenu ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   // Disable event notification.  Using a NULL hWnd tells
   // the COMM.DRV to disable future notifications.

   EnableCommNotification( COMDEV( npTTYInfo ), NULL, -1, -1 ) ;

   // kill the focus

   KillTTYFocus( hWnd ) ;

   // drop DTR

   EscapeCommFunction( COMDEV( npTTYInfo ), CLRDTR ) ;

   // close comm connection

   CloseComm( COMDEV( npTTYInfo ) ) ;
   CONNECTED( npTTYInfo ) = FALSE ;

   // change the selectable items in the menu

   hMenu = GetMenu( hWnd ) ;
   EnableMenuItem( hMenu, IDM_DISCONNECT,
                   MF_GRAYED | MF_DISABLED | MF_BYCOMMAND ) ;
   EnableMenuItem( hMenu, IDM_CONNECT,
                   MF_ENABLED | MF_BYCOMMAND ) ;

   return ( TRUE ) ;

} // end of CloseConnection()

//---------------------------------------------------------------------------
//  int NEAR ReadCommBlock( HWND hWnd, LPSTR lpszBlock, int nMaxLength )
//
//  Description:
//     Reads a block from the COM port and stuffs it into
//     the provided block.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpszBlock
//        block used for storage
//
//     int nMaxLength
//        max length of block to read
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

int NEAR ReadCommBlock( HWND hWnd, LPSTR lpszBlock, int nMaxLength )
{
   char       szError[ 10 ] ;
   int        nLength, nError ;
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   nLength = ReadComm( COMDEV( npTTYInfo ), lpszBlock, nMaxLength ) ;

   if (nLength < 0)
   {
      nLength *= -1 ;
      while (nError = GetCommError( COMDEV( npTTYInfo ), NULL ))
      {
         if (DISPLAYERRORS( npTTYInfo ))
         {
            wsprintf( szError, "<CE-%d>", nError ) ;
            WriteTTYBlock( hWnd, szError, lstrlen( szError ) ) ;
         }
      }
   }

   return ( nLength ) ;

} // end of ReadCommBlock()

//---------------------------------------------------------------------------
//  BOOL NEAR WriteCommByte( HWND hWnd, BYTE bByte )
//
//  Description:
//     Writes a byte to the COM port specified in the associated
//     TTY info structure.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     BYTE bByte
//        byte to write to port
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR WriteCommByte( HWND hWnd, BYTE bByte )
{
   NPTTYINFO  npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   WriteComm( COMDEV( npTTYInfo ), (LPSTR) &bByte, 1 ) ;

   return ( TRUE ) ;

} // end of WriteCommByte()

//---------------------------------------------------------------------------
//  BOOL NEAR WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
//
//  Description:
//     Writes block to TTY screen.  Nothing fancy - just
//     straight TTY.
//
//  Parameters:
//     HWND hWnd
//        handle to TTY window
//
//     LPSTR lpBlock
//        far pointer to block of data
//
//     int nLength
//        length of block
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//              5/20/91   BryanW      Modified... not character based,
//                                    block based now.  It was processing
//                                    per char.
//
//---------------------------------------------------------------------------

BOOL NEAR WriteTTYBlock( HWND hWnd, LPSTR lpBlock, int nLength )
{
   int        i ;
   NPTTYINFO  npTTYInfo ;
   RECT       rect ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GetWindowWord( hWnd, GWW_NPTTYINFO )))
      return ( FALSE ) ;

   for (i = 0 ; i < nLength; i++)
   {
      switch (lpBlock[ i ])
      {
         case ASCII_BEL:
            // Bell
            MessageBeep( 0 ) ;
            break ;
   
         case ASCII_BS:
            // Backspace
            if (COLUMN( npTTYInfo ) > 0)
               COLUMN( npTTYInfo ) -- ;
            MoveTTYCursor( hWnd ) ;
            break ;
   
         case ASCII_CR:
            // Carriage return
            COLUMN( npTTYInfo ) = 0 ;
            MoveTTYCursor( hWnd ) ;
            if (!NEWLINE( npTTYInfo ))
               break;
   
            // fall through
   
         case ASCII_LF:
            // Line feed
            if (ROW( npTTYInfo )++ == MAXROWS - 1)
            {
               _fmemmove( (LPSTR) (SCREEN( npTTYInfo )),
                          (LPSTR) (SCREEN( npTTYInfo ) + MAXCOLS),
                          (MAXROWS - 1) * MAXCOLS ) ;
               _fmemset( (LPSTR) (SCREEN( npTTYInfo ) + (MAXROWS - 1) * MAXCOLS),
                         ' ', MAXCOLS ) ;
               InvalidateRect( hWnd, NULL, FALSE ) ;
               ROW( npTTYInfo )-- ;
            }
            MoveTTYCursor( hWnd ) ;
            break ;

         default:
            *(SCREEN( npTTYInfo ) + ROW( npTTYInfo ) * MAXCOLS +
                COLUMN( npTTYInfo )) = lpBlock[ i ] ;
            rect.left = (COLUMN( npTTYInfo ) * XCHAR( npTTYInfo )) -
                        XOFFSET( npTTYInfo ) ;
            rect.right = rect.left + XCHAR( npTTYInfo ) ;
            rect.top = (ROW( npTTYInfo ) * YCHAR( npTTYInfo )) -
                       YOFFSET( npTTYInfo ) ;
            rect.bottom = rect.top + YCHAR( npTTYInfo ) ;
            InvalidateRect( hWnd, &rect, FALSE ) ;

            // Line wrap
            if (COLUMN( npTTYInfo ) < MAXCOLS - 1)
               COLUMN( npTTYInfo )++ ;
            else if (AUTOWRAP( npTTYInfo ))
               WriteTTYBlock( hWnd, "\r\n", 2 ) ;
            break;
      }
   }
   return ( TRUE ) ;

} // end of WriteTTYBlock()

//---------------------------------------------------------------------------
//  VOID NEAR GoModalDialogBoxParam( HINSTANCE hInstance,
//                                   LPCSTR lpszTemplate, HWND hWnd,
//                                   DLGPROC lpDlgProc, LPARAM lParam )
//
//  Description:
//     It is a simple utility function that simply performs the
//     MPI and invokes the dialog box with a DWORD paramter.
//
//  Parameters:
//     similar to that of DialogBoxParam() with the exception
//     that the lpDlgProc is not a procedure instance
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

VOID NEAR GoModalDialogBoxParam( HINSTANCE hInstance, LPCSTR lpszTemplate,
                                 HWND hWnd, DLGPROC lpDlgProc, LPARAM lParam )
{
   DLGPROC  lpProcInstance ;

   lpProcInstance = (DLGPROC) MakeProcInstance( (FARPROC) lpDlgProc,
                                                hInstance ) ;
   DialogBoxParam( hInstance, lpszTemplate, hWnd, lpProcInstance, lParam ) ;
   FreeProcInstance( (FARPROC) lpProcInstance ) ;

} // end of GoModalDialogBoxParam()

//---------------------------------------------------------------------------
//  BOOL FAR PASCAL AboutDlgProc( HWND hDlg, UINT uMsg,
//                                WPARAM wParam, LPARAM lParam )
//
//  Description:
//     Simulates the Windows System Dialog Box.
//
//  Parameters:
//     Same as standard dialog procedures.
//
//  History:   Date       Author      Comment
//             10/19/91   BryanW      Added this little extra.
//      
//---------------------------------------------------------------------------

BOOL FAR PASCAL AboutDlgProc( HWND hDlg, UINT uMsg,
                              WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         int          idModeString ;
         char         szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
         DWORD        dwFreeMemory, dwWinFlags ;
         WORD         wFreeResources, wRevision, wVersion ;

#ifdef ABOUTDLG_USEBITMAP
         // if we are using the bitmap, hide the icon

         ShowWindow( GetDlgItem( hDlg, IDD_ABOUTICON ), SW_HIDE ) ;
#endif
         // sets up the version number for Windows

         wVersion = LOWORD( GetVersion() ) ;
         switch (HIBYTE( wVersion ))
         {
            case 10:
               wRevision = 1 ;
               break ;

            default:
               wRevision = 0 ;
               break;
         }
         wVersion &= 0xFF ;

         GetDlgItemText( hDlg, IDD_TITLELINE, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, wVersion, wRevision ) ;
         SetDlgItemText( hDlg, IDD_TITLELINE, szBuffer ) ;

         // sets up version number for TTY

         GetDlgItemText( hDlg, IDD_VERSION, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, VER_MAJOR, VER_MINOR, VER_BUILD ) ;
         SetDlgItemText( hDlg, IDD_VERSION, (LPSTR) szBuffer ) ;

         // get by-line

         LoadString( GETHINST( hDlg ), IDS_BYLINE, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_BYLINE, szBuffer ) ;

         // set windows mode information

         dwWinFlags = GetWinFlags() ;
         if (dwWinFlags & WF_ENHANCED)
            idModeString = IDS_MODE_ENHANCED ;
         else if (dwWinFlags & WF_STANDARD)
            idModeString = IDS_MODE_STANDARD ;
         else if (dwWinFlags & WF_WLO)
            idModeString = IDS_MODE_WLO ;
         else
            idModeString = IDS_MODE_UNDEF ;

         LoadString( GETHINST( hDlg ), idModeString, szBuffer,
                     sizeof( szBuffer ) ) ;
         SetDlgItemText( hDlg, IDD_WINDOWSMODE, szBuffer ) ;

         // get free memory information

         dwFreeMemory = GetFreeSpace( 0 ) / 1024L ;
         GetDlgItemText( hDlg, IDD_FREEMEM, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, dwFreeMemory ) ;
         SetDlgItemText( hDlg, IDD_FREEMEM, (LPSTR) szBuffer ) ;

         // get free resources information

         wFreeResources = GetFreeSystemResources( 0 ) ;
         GetDlgItemText( hDlg, IDD_RESOURCES, szTemp, sizeof( szTemp ) ) ;
         wsprintf( szBuffer, szTemp, wFreeResources ) ;
         SetDlgItemText( hDlg, IDD_RESOURCES, (LPSTR) szBuffer ) ;
      }
      return ( TRUE ) ;

#ifdef ABOUTDLG_USEBITMAP
      // used to paint the bitmap

      case WM_PAINT:
      {
         HBITMAP      hBitMap ;
         HDC          hDC, hMemDC ;
         PAINTSTRUCT  ps ;

         // load bitmap and display it

         hDC = BeginPaint( hDlg, &ps ) ;
         if (NULL != (hMemDC = CreateCompatibleDC( hDC )))
         {
            hBitMap = LoadBitmap( GETHINST( hDlg ),
                                  MAKEINTRESOURCE( TTYBITMAP ) ) ;
            hBitMap = SelectObject( hMemDC, hBitMap ) ;
            BitBlt( hDC, 10, 10, 64, 64, hMemDC, 0, 0, SRCCOPY ) ;
            DeleteObject( SelectObject( hMemDC, hBitMap ) ) ;
            DeleteDC( hMemDC ) ;
         }
         EndPaint( hDlg, &ps ) ;
      }
      break ;
#endif

      case WM_COMMAND:
         if ((WORD) wParam == IDD_OK)
         {
            EndDialog( hDlg, TRUE ) ;
            return ( TRUE ) ;
         }
         break;
   }
   return ( FALSE ) ;

} // end of AboutDlgProc()

//---------------------------------------------------------------------------
//  VOID NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
//                          WORD NEAR *npTable, WORD wTableLen,
//                          WORD wCurrentSetting )
//
//  Description:
//     Fills the given combo box with strings from the resource
//     table starting at nIDString.  Associated items are
//     added from given table.  The combo box is notified of
//     the current setting.
//
//  Parameters:
//     HINSTANCE hInstance
//        handle to application instance
//
//     HWND hCtrlWnd
//        handle to combo box control
//
//     int nIDString
//        first resource string id
//
//     WORD NEAR *npTable
//        near point to table of associated values
//
//     WORD wTableLen
//        length of table
//
//     WORD wCurrentSetting
//        current setting (for combo box selection)
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Pulled from the init procedure.
//
//---------------------------------------------------------------------------

VOID NEAR FillComboBox( HINSTANCE hInstance, HWND hCtrlWnd, int nIDString,
                        WORD NEAR *npTable, WORD wTableLen,
                        WORD wCurrentSetting )
{
   char  szBuffer[ MAXLEN_TEMPSTR ] ;
   WORD  wCount, wPosition ;

   for (wCount = 0; wCount < wTableLen; wCount++)
   {
      // load the string from the string resources and
      // add it to the combo box

      LoadString( hInstance, nIDString + wCount, szBuffer, sizeof( szBuffer ) ) ;
      wPosition = LOWORD( SendMessage( hCtrlWnd, CB_ADDSTRING, NULL,
                                       (LPARAM) (LPSTR) szBuffer ) ) ;

      // use item data to store the actual table value

      SendMessage( hCtrlWnd, CB_SETITEMDATA, (WPARAM) wPosition,
                   (LPARAM) (LONG) *(npTable + wCount) ) ;

      // if this is our current setting, select it

      if (*(npTable + wCount) == wCurrentSetting)
         SendMessage( hCtrlWnd, CB_SETCURSEL, (WPARAM) wPosition, NULL ) ;
   }

} // end of FillComboBox()

//---------------------------------------------------------------------------
//  BOOL NEAR SettingsDlgInit( HWND hDlg )
//
//  Description:
//     Puts current settings into dialog box (via CheckRadioButton() etc.)
//
//  Parameters:
//     HWND hDlg
//        handle to dialog box
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//             10/20/91   BryanW      Dialog revision.
//             10/24/91   BryanW      Bug in EscapeCommFunction() call.
//
//---------------------------------------------------------------------------

BOOL NEAR SettingsDlgInit( HWND hDlg )
{
   char       szBuffer[ MAXLEN_TEMPSTR ], szTemp[ MAXLEN_TEMPSTR ] ;
   NPTTYINFO  npTTYInfo ;
   WORD       wCount, wMaxCOM, wPosition ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GET_PROP( hDlg, ATOM_TTYINFO )))
      return ( FALSE ) ;

   wMaxCOM = LOWORD( EscapeCommFunction( NULL, GETMAXCOM ) ) + 1 ;

   // load the COM prefix from resources

   LoadString( GETHINST( hDlg ), IDS_COMPREFIX, szTemp, sizeof( szTemp ) ) ;

   // fill port combo box and make initial selection

   for (wCount = 0; wCount < wMaxCOM; wCount++)
   {
      wsprintf( szBuffer, "%s%d", (LPSTR) szTemp, wCount + 1 ) ;
      SendDlgItemMessage( hDlg, IDD_PORTCB, CB_ADDSTRING, NULL,
                          (LPARAM) (LPSTR) szBuffer ) ;
   }
   SendDlgItemMessage( hDlg, IDD_PORTCB, CB_SETCURSEL,
                       (WPARAM) (PORT( npTTYInfo ) - 1), NULL ) ;

   // disable COM port combo box if connection has already been
   // established (e.g. OpenComm() already successful)

   EnableWindow( GetDlgItem( hDlg, IDD_PORTCB ), !CONNECTED( npTTYInfo ) ) ;

   // fill baud combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_BAUDCB ),
                 IDS_BAUD110, gawBaudTable,
                 sizeof( gawBaudTable ) / sizeof( WORD ),
                 BAUDRATE( npTTYInfo ) ) ;

   // fill data bits combo box and make initial selection

   for (wCount = 5; wCount < 9; wCount++)
   {
      wsprintf( szBuffer, "%d", wCount ) ;
      wPosition = LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                              CB_ADDSTRING, NULL,
                                              (LPARAM) (LPSTR) szBuffer ) ) ;

      // if current selection, tell the combo box

      if (wCount == BYTESIZE( npTTYInfo ))
         SendDlgItemMessage( hDlg, IDD_DATABITSCB, CB_SETCURSEL,
                             (WPARAM) wPosition, NULL ) ;
   }

   // fill parity combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_PARITYCB ),
                 IDS_PARITYNONE, gawParityTable,
                 sizeof( gawParityTable ) / sizeof( WORD ),
                 PARITY( npTTYInfo ) ) ;

   // fill stop bits combo box and make initial selection

   FillComboBox( GETHINST( hDlg ), GetDlgItem( hDlg, IDD_STOPBITSCB ),
                 IDS_ONESTOPBIT, gawStopBitsTable,
                 sizeof( gawStopBitsTable ) / sizeof ( WORD ),
                 STOPBITS( npTTYInfo ) ) ;

   // initalize the flow control settings

   CheckDlgButton( hDlg, IDD_DTRDSR,
                   (FLOWCTRL( npTTYInfo ) & FC_DTRDSR) > 0 ) ;
   CheckDlgButton( hDlg, IDD_RTSCTS,
                   (FLOWCTRL( npTTYInfo ) & FC_RTSCTS) > 0 ) ;
   CheckDlgButton( hDlg, IDD_XONXOFF,
                   (FLOWCTRL( npTTYInfo ) & FC_XONXOFF) > 0 ) ;

   // other TTY settings

   CheckDlgButton( hDlg, IDD_AUTOWRAP, AUTOWRAP( npTTYInfo ) ) ;
   CheckDlgButton( hDlg, IDD_NEWLINE, NEWLINE( npTTYInfo ) ) ;
   CheckDlgButton( hDlg, IDD_LOCALECHO, LOCALECHO( npTTYInfo ) ) ;

   // control options

   CheckDlgButton( hDlg, IDD_USECNRECEIVE, USECNRECEIVE( npTTYInfo ) ) ;

   // disable Use CN_RECEIVE option if connection has already been
   // established (e.g. OpenComm() already successful)

   EnableWindow( GetDlgItem( hDlg, IDD_USECNRECEIVE ),
                 !CONNECTED( npTTYInfo ) ) ;

   CheckDlgButton( hDlg, IDD_DISPLAYERRORS, DISPLAYERRORS( npTTYInfo ) ) ;

   return ( TRUE ) ;

} // end of SettingsDlgInit()

//---------------------------------------------------------------------------
//  BOOL NEAR SelectTTYFont( HWND hDlg )
//
//  Description:
//     Selects the current font for the TTY screen.
//     Uses the Common Dialog ChooseFont() API.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//  History:   Date       Author      Comment
//             10/20/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR SelectTTYFont( HWND hDlg )
{
   CHOOSEFONT  cfTTYFont ;
   NPTTYINFO   npTTYInfo ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GET_PROP( hDlg, ATOM_TTYINFO )))
      return ( FALSE ) ;

   cfTTYFont.lStructSize    = sizeof( CHOOSEFONT ) ;
   cfTTYFont.hwndOwner      = hDlg ;
   cfTTYFont.hDC            = NULL ;
   cfTTYFont.rgbColors      = FGCOLOR( npTTYInfo ) ;
   cfTTYFont.lpLogFont      = &LFTTYFONT( npTTYInfo ) ;
   cfTTYFont.Flags          = CF_SCREENFONTS | CF_FIXEDPITCHONLY |
                              CF_EFFECTS | CF_INITTOLOGFONTSTRUCT ;
   cfTTYFont.lCustData      = NULL ;
   cfTTYFont.lpfnHook       = NULL ;
   cfTTYFont.lpTemplateName = NULL ;
   cfTTYFont.hInstance      = GETHINST( hDlg ) ;

   if (ChooseFont( &cfTTYFont ))
   {
     FGCOLOR( npTTYInfo ) = cfTTYFont.rgbColors ;
     ResetTTYScreen( GetParent( hDlg ), npTTYInfo ) ;
   }

   return ( TRUE ) ;

} // end of SelectTTYFont()

//---------------------------------------------------------------------------
//  BOOL NEAR SettingsDlgTerm( HWND hDlg )
//
//  Description:
//     Puts dialog contents into TTY info structure.
//
//  Parameters:
//     HWND hDlg
//        handle to settings dialog
//
//  History:   Date       Author      Comment
//              5/11/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------

BOOL NEAR SettingsDlgTerm( HWND hDlg )
{
   NPTTYINFO  npTTYInfo ;
   WORD       wSelection ;

   if (NULL == (npTTYInfo = (NPTTYINFO) GET_PROP( hDlg, ATOM_TTYINFO )))
      return ( FALSE ) ;

   // get port selection

   PORT( npTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PORTCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 1 ) ;
   // get baud rate selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   BAUDRATE( npTTYInfo ) =
      LOWORD( SendDlgItemMessage( hDlg, IDD_BAUDCB, CB_GETITEMDATA,
                                  (WPARAM) wSelection, NULL ) ) ;

   // get data bits selection

   BYTESIZE( npTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_DATABITSCB,
                                          CB_GETCURSEL,
                                          NULL, NULL ) ) + 5 ) ;

   // get parity selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   PARITY( npTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_PARITYCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection,
                                           NULL ) ) ) ;

   // get stop bits selection

   wSelection =
      LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB, CB_GETCURSEL,
                                  NULL, NULL ) ) ;
   STOPBITS( npTTYInfo ) =
      LOBYTE( LOWORD( SendDlgItemMessage( hDlg, IDD_STOPBITSCB,
                                          CB_GETITEMDATA,
                                          (WPARAM) wSelection, NULL ) ) ) ;

   // get flow control settings

   FLOWCTRL( npTTYInfo ) = 0 ;
   if (IsDlgButtonChecked( hDlg, IDD_DTRDSR ))
      FLOWCTRL( npTTYInfo ) |= FC_DTRDSR ;
   if (IsDlgButtonChecked( hDlg, IDD_RTSCTS ))
      FLOWCTRL( npTTYInfo ) |= FC_RTSCTS ;
   if (IsDlgButtonChecked( hDlg, IDD_XONXOFF ))
      FLOWCTRL( npTTYInfo ) |= FC_XONXOFF ;

   // get other various settings

   AUTOWRAP( npTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_AUTOWRAP ) ;
   NEWLINE( npTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_NEWLINE ) ;
   LOCALECHO( npTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_LOCALECHO ) ;

   // control options

   USECNRECEIVE( npTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_USECNRECEIVE ) ;
   DISPLAYERRORS( npTTYInfo ) = IsDlgButtonChecked( hDlg, IDD_DISPLAYERRORS ) ;

   return ( TRUE ) ;

} // end of SettingsDlgTerm()

//---------------------------------------------------------------------------
//  BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, UINT uMsg,
//                                   WPARAM wParam, LPARAM lParam )
//
//  Description:
//     This handles all of the user preference settings for
//     the TTY.
//
//  Parameters:
//     same as all dialog procedures
//
//  History:   Date       Author      Comment
//              5/10/91   BryanW      Wrote it.
//             10/20/91   BryanW      Now uses window properties to
//                                    store TTYInfo handle.  Also added
//                                    font selection.
//
//---------------------------------------------------------------------------

BOOL FAR PASCAL SettingsDlgProc( HWND hDlg, UINT uMsg,
                                 WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
      case WM_INITDIALOG:
      {
         NPTTYINFO  npTTYInfo ;

         // get & save pointer to TTY info structure

         npTTYInfo = (NPTTYINFO) LOWORD( lParam ) ;
         SET_PROP( hDlg, ATOM_TTYINFO, (HANDLE) npTTYInfo ) ;

         return ( SettingsDlgInit( hDlg ) ) ;
      }

      case WM_COMMAND:
         switch ((WORD) wParam)
         {
            case IDD_FONT:
               return ( SelectTTYFont( hDlg ) ) ;

            case IDD_OK:
               // Copy stuff into structure
               SettingsDlgTerm( hDlg ) ;
               EndDialog( hDlg, TRUE ) ;
               return ( TRUE ) ;

            case IDD_CANCEL:
               // Just end
               EndDialog( hDlg, TRUE ) ;
               return ( TRUE ) ;
         }
         break;

      case WM_DESTROY:
         REMOVE_PROP( hDlg, ATOM_TTYINFO ) ;
         break ;
   }
   return ( FALSE ) ;

} // end of SettingsDlgProc()

//---------------------------------------------------------------------------
//  End of File: tty.c
//---------------------------------------------------------------------------

