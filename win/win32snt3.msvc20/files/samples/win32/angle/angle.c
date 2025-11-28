
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/


#include <windows.h>
#include <math.h>
#include <string.h>
#include "angle.h"

HANDLE              hInst;
HWND                hwndMain, hwndDlg;


/**************************************************************************\
*
*  function:  WinMain()
*
*  input parameters:  c.f. generic sample
*
\**************************************************************************/
int PASCAL
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
    MSG                 msg;
    RECT                rect;

    UNREFERENCED_PARAMETER(lpCmdLine);

  // check if running on Windows NT, if not, display notice and terminate
    if( GetVersion() & 0x80000000 )
    {
       MessageBox( NULL,
          "This sample application can only be run on Windows NT.\n"
          "This application will now terminate.",
          "Angle",
          MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
       return( 1 );
    }

 /* Check for previous instance.  If none, then register class. */
    if (!hPrevInstance)
    {
        WNDCLASS            wc;

        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC) MainWndProc;

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, "AngleIcon");
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = GetStockObject(LTGRAY_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "angle";

        if (!RegisterClass(&wc))
            return (FALSE);
    }   /* class registered o.k. */
 /*  Create the main window.  Return false if CreateWindow() fails */
    hInst = hInstance;

    hwndMain = CreateWindow(
                            "angle",
                            "AngleArc",
                            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    if (!hwndMain)
        return (FALSE);
    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);


 /* create the top dialog as a child of the main window. */
    hwndDlg = CreateDialog(hInst, "AngleDlg", hwndMain, (DLGPROC) DlgProc);

 /*
  * Send main window a WM_SIZE message so that it will size the top dialog
  * correctly.  Also, force a repaint of the main window now that the dialog is
  * there and we can draw the arc. 
  */
    GetClientRect(hwndMain, &rect);
    SendMessage(hwndMain, WM_SIZE, 0, (rect.right - rect.left));
    ShowWindow(hwndDlg, SW_SHOW);
    InvalidateRect(hwndMain, NULL, FALSE);

 /* Loop getting messages and dispatching them. */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(hwndDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (msg.wParam);
}



/**************************************************************************\
*
*  function:  MainWndProc()
*
*  input parameters:  normal window procedure parameters.
*
*  global variables:
*   hwndDlg - dialog with entry fields containing arc parameters.
*
\**************************************************************************/
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    char                buffer[MAXCHARS];
    static HANDLE       hPenGrid, hPenArc;

    switch (message)
    {

        /**********************************************************************\
        *  WM_CREATE
        *
        *  Then create three pens for drawing with later.
        \**********************************************************************/
        case WM_CREATE:
            hPenGrid = CreatePen(PS_SOLID, 1, GRIDCOLOR);
            hPenArc = CreatePen(PS_SOLID, 2, (COLORREF) 0x01000005);
            break;


        /**********************************************************************\
        *  WM_DESTROY
        *
        * Complement of the WM_CREATE message.  Delete the pens that were
        *  created and then call postquitmessage.
        \**********************************************************************/
        case WM_DESTROY:
            DeleteObject(hPenGrid);
            DeleteObject(hPenArc);

            PostQuitMessage(0);
            break;


        /**********************************************************************\
        *  WM_SIZE
        *
        * Stretch the top dialog to fill the width of the main window.
        \**********************************************************************/
        case WM_SIZE:
            SetWindowPos(hwndDlg, NULL, 0, 0, LOWORD(lParam), DIALOGHEIGHT, 0);
			InvalidateRect(hwndMain, NULL, TRUE);
            break;



        /**********************************************************************\
        *  WM_PAINT
        *
        * First shift the viewport origin down so that 0,0 is the top left
        *  most visible point (out from underneath the top dialog).  Second,
        *  draw the grid with wider lines on the axes.  Finally, read the
        *  values out of the top dialog, do elementary validation, and then
        *  try to call AngleArc() with the values.  If a value fails validation,
        *  then write a small error message, and don't draw the arc.
        \**********************************************************************/
        case WM_PAINT:
            {
                HDC                 hdc;
                PAINTSTRUCT         ps;
                RECT                rect;
                int                 i;
                int                 x, y, radius;
                float               start, sweep;
                BOOL                success;

                hdc = BeginPaint(hwnd, &ps);

                SetViewportOrgEx(hdc, 0, DIALOGHEIGHT, NULL);

                GetClientRect(hwndMain, &rect);

                SelectObject(hdc, hPenGrid);
            /* Draw vertical lines.  */
                for (i = 0; i <= rect.right; i += TICKSPACE)
                {
                    MoveToEx(hdc, i, rect.top, NULL);
                    LineTo(hdc, i, rect.bottom);
                }
                MoveToEx(hdc, 1, 0, NULL);
                LineTo(hdc, 1, rect.bottom);

            /* Draw horizontal lines.  */
                for (i = 0; i <= rect.bottom; i += TICKSPACE)
                {
                    MoveToEx(hdc, rect.left, i, NULL);
                    LineTo(hdc, rect.right, i);
                }
                MoveToEx(hdc, 0, 1, NULL);
                LineTo(hdc, rect.right, 1);

            /* new color pen for the actual arc. */
                SelectObject(hdc, hPenArc);


            /*
             * Query the top dialog parameters, if a value is bad, report that
             * and break out of conditional. if all values are good, then set
             * the current point and call AngleArc(). 
             */
                if (IsWindow(hwndDlg))
                {
                    x = GetDlgItemInt(hwndDlg, DID_X, &success, TRUE);
                    if (!success)
                    {
                        TextOut(hdc, 10, rect.bottom - 2 * DIALOGHEIGHT, "Bad X", 5);
                        break;
                    }
                    y = GetDlgItemInt(hwndDlg, DID_Y, &success, TRUE);
                    if (!success)
                    {
                        TextOut(hdc, 30, rect.bottom - 2 * DIALOGHEIGHT, "Bad Y", 5);
                        break;
                    }
                    radius = GetDlgItemInt(hwndDlg, DID_RADIUS, &success, TRUE);
                    if (!success)
                    {
                        TextOut(hdc, 50, rect.bottom - 2 * DIALOGHEIGHT, "Bad Radius", 10);
                        break;
                    }

                /*
                 * Hard to validate these floating point numbers. Good chance
                 * that invalid values will just map to 0.0 
                 */
                    if (!GetDlgItemText(hwndDlg, DID_START, buffer, MAXCHARS))
                    {
                        TextOut(hdc, 70, rect.bottom - 2 * DIALOGHEIGHT, "Bad Start", 9);
                        break;
                    }
                    start = (float) atof(buffer);

                    if (!GetDlgItemText(hwndDlg, DID_SWEEP, buffer, MAXCHARS))
                    {
                        TextOut(hdc, 90, rect.bottom - 2 * DIALOGHEIGHT, "Bad Sweep", 9);
                        break;
                    }
                    sweep = (float) atof(buffer);

                    MoveToEx(hdc, x, y, NULL);

                /**********************************************************/
                /**********************************************************/
                    AngleArc(hdc, x, y, (DWORD) radius, start, sweep);
                /**********************************************************/
                /**********************************************************/
                }
                EndPaint(hwnd, &ps);

            } return FALSE;

    }   /* end switch */
    return (DefWindowProc(hwnd, message, wParam, lParam));
}




/**************************************************************************\
*
*  function:  DlgProc()
*
*  input parameters:  normal window procedure parameters.
*
*  global variables:
*   hwndmain - the main window.  also the parent of this dialog
*
\**************************************************************************/

BOOL CALLBACK
DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        /**********************************************************************\
        *  WM_INITDIALOG
        *
        * Fill the entry fields with sensible original values.
        \**********************************************************************/
        case WM_INITDIALOG:
            SetDlgItemText(hwnd, DID_X, "100");
            SetDlgItemText(hwnd, DID_Y, "100");
            SetDlgItemText(hwnd, DID_RADIUS, "50");
            SetDlgItemText(hwnd, DID_START, "0.0");
            SetDlgItemText(hwnd, DID_SWEEP, "270.0");
            return TRUE;



        /**********************************************************************\
        *  WM_COMMAND, DID_DRAW
        *
        * Invalidate the main window so that we force a repaint.
        \**********************************************************************/
        case WM_COMMAND:
            if (LOWORD(wParam) == DID_DRAW)
            {
                InvalidateRect(hwndMain, NULL, TRUE);
            }
            return FALSE;


    }   /* end switch */
    return 0;
}
