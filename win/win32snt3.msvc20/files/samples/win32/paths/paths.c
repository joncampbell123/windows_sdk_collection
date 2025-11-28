
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/**************************************************************************\
*  paths.c -- sample program demonstrating paths.
\**************************************************************************/


#include <windows.h>
#include <string.h>
#include "paths.h"


/**************************************************************************\
*
*  function:  WinMain()
*
*  input parameters:  c.f. generic sample
*
\**************************************************************************/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hInst;
    HWND   hwndMain;
    MSG    msg;

    UNREFERENCED_PARAMETER( lpCmdLine );

     // check if running on Windows NT, if not, display notice and terminate
        if( GetVersion() & 0x80000000 )
        {
          MessageBoxA( NULL,
           "This sample application can only be run on Windows NT.\n"
             "This application will now terminate.",
             "Paths",
             MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
          return( 1 );
        }

    /* Check for previous instance.  If none, then register class. */
    if (!hPrevInstance) {
        WNDCLASS  wc;

        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = (WNDPROC)MainWndProc;

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, "pathsIcon");
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
        wc.lpszMenuName =  NULL;
        wc.lpszClassName = "paths";

        if (!RegisterClass(&wc)) return (FALSE);
    }  /* class registered o.k. */


    /* Create the main window.  Return false if CreateWindow() fails */
    hInst = hInstance;

    hwndMain = CreateWindow(
        "paths",
        "Paths",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwndMain) return (FALSE);
    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);


    /* Loop getting messages and dispatching them. */
    while (GetMessage(&msg,NULL, 0,0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return (msg.wParam);
}



/**************************************************************************\
*
*  function:  MainWndProc()
*
*  input parameters:  normal window procedure parameters.
*
*  This is a very simple window.  On create it creates pens to draw with.
*   They are later destroyed at destroy time.  The only interesting code
*   is in the paint message.  We draw 6 different paths into the client
*   area demonstrating various features of paths.
*
*  global variables: none.
\**************************************************************************/
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HANDLE hPenFill, hPenPath;


  switch (message) {


    /**********************************************************************\
    *  WM_CREATE
    *
    * Create three pens for drawing with later.
    \**********************************************************************/
    case WM_CREATE:
        hPenPath   = CreatePen (PS_SOLID, 2, (COLORREF) 0x01000005);
        hPenFill   = CreatePen (PS_SOLID, 1, (COLORREF) 0x01000002);
    break;


    /**********************************************************************\
    *  WM_DESTROY
    *
    * Complement of the WM_CREATE message.  Delete the pens that were
    *  created and then call postquitmessage.
    \**********************************************************************/
    case WM_DESTROY:
      DeleteObject (hPenFill);
      DeleteObject (hPenPath);

      PostQuitMessage(0);
    break;


    /**********************************************************************\
    *  WM_PAINT
    *
    * Query the client rectangle.  Divide it into six sub rectangles
    *  (only actually compute the top left sixth).  Now, set the origin
    *  for each one of these, write text describing the operation, and
    *  then stroke or fill or whatever the path.
    \**********************************************************************/
    case WM_PAINT: {
      HDC  hdc;
      PAINTSTRUCT ps;
      RECT rect;
      HRGN hrgn;
      int  i;

      hdc = BeginPaint(hwnd, &ps);
      SetBkMode (hdc, TRANSPARENT);

      GetClientRect (hwnd, &rect);
      rect.bottom /= 2;
      rect.right /= 3;
      SelectObject(hdc, hPenPath);

      /* 1. Just stroke the path */
      SetViewportOrgEx (hdc, 0, 0, NULL);
      TextOut (hdc, 2, 10, STROKEPATH, sizeof (STROKEPATH)-1);
      DrawPath(hdc, &rect);
      StrokePath (hdc);


      /* 2. Fill the path */
      SetViewportOrgEx (hdc, 0, rect.bottom, NULL);
      TextOut (hdc, 2, 10, FILLPATH, sizeof (FILLPATH)-1);
      DrawPath(hdc, &rect);
      FillPath (hdc);

      /* 3. Stroke and fill the path */
      SetViewportOrgEx (hdc,rect.right, 0, NULL);
      SetPolyFillMode (hdc, WINDING);
      TextOut (hdc, 2, 10, STROKEANDFILLWIND, sizeof (STROKEANDFILLWIND)-1);
      DrawPath(hdc, &rect);
      StrokeAndFillPath (hdc);

      /* 4. Stroke and fill it again, but with different mode. */
      SetViewportOrgEx (hdc,rect.right, rect.bottom, NULL);
      SetPolyFillMode (hdc, ALTERNATE);
      TextOut (hdc, 2, 10, STROKEANDFILLALT, sizeof (STROKEANDFILLALT)-1);
      DrawPath(hdc, &rect);
      StrokeAndFillPath (hdc);


      /* 5. Set the clipping region based on the path. */
      SetViewportOrgEx (hdc,rect.right*2, 0, NULL);
      SetPolyFillMode (hdc, WINDING);
      TextOut (hdc, 2, 10, CLIPPATHWIND, sizeof (CLIPPATHWIND)-1);
      SelectObject(hdc, hPenFill);
      DrawPath(hdc, &rect);
      hrgn = PathToRegion(hdc);
      SelectClipRgn (hdc, hrgn);
      DeleteObject (hrgn);
      MoveToEx (hdc, rect.right/2, rect.bottom/2, NULL);
      for (i = 0; i < rect.right; i += 5)
        AngleArc (hdc, rect.right/2, rect.bottom/2, i, (FLOAT)0.0, (FLOAT)360.0);

      /* 6. Set the clipping region based on the path, with different mode. */
      SetViewportOrgEx (hdc,rect.right*2, rect.bottom, NULL);
      SetPolyFillMode (hdc, ALTERNATE);
      /* clear the clip region so that TextOut() shows up. */
      ExtSelectClipRgn (hdc, NULL, RGN_COPY);
      TextOut (hdc, 2, 10, CLIPPATHALT, sizeof (CLIPPATHALT)-1);
      DrawPath(hdc, &rect);
      hrgn = PathToRegion(hdc);
      SelectClipRgn (hdc, hrgn);
      DeleteObject (hrgn);
      MoveToEx (hdc, rect.right/2, rect.bottom/2, NULL);
      for (i = 0; i < rect.right; i += 5)
        AngleArc (hdc, rect.right/2, rect.bottom/2, i, (FLOAT)0.0, (FLOAT)360.0);

      EndPaint (hwnd, &ps);

    } return 0;


  } /* end switch */
  return (DefWindowProc(hwnd, message, wParam, lParam));
}



/**************************************************************************\
*
*  function:  DrawPath()
*
* The path used here is a simple rectangle with an ellipse inside of it.
*  The InflateRect() calls have nothing to do with the path, but are just
*  used for convenience.
\**************************************************************************/
void DrawPath(HDC hdc, PRECT prect)
{
    RECT destRect;

    /* make a copy that we can modify with impunity. */
    CopyRect (&destRect, prect);

    BeginPath (hdc);

    InflateRect (&destRect, ((destRect.left- destRect.right)/5),
                        ((destRect.top- destRect.bottom)/5));

    Rectangle (hdc, destRect.left, destRect.top,
                    destRect.right, destRect.bottom);

    InflateRect (&destRect, ((destRect.left- destRect.right)/4),
                        ((destRect.top- destRect.bottom)/4));

    Ellipse (hdc, destRect.left, destRect.top,
                  destRect.right, destRect.bottom);

    EndPath (hdc);
}
