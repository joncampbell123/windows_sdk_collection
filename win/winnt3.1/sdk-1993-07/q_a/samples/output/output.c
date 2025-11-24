
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/****************************************************************************

    PROGRAM: Output.c

    PURPOSE: Output template for Windows applications

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "string.h"
#include "output.h"

HANDLE hInst;

HPEN hDashPen;                                         /* "---" pen handle   */
HPEN hDotPen;                                          /* "..." pen handle   */
HBRUSH hOldBrush;                                      /* old brush handle   */
HBRUSH hRedBrush;                                      /* red brush handle   */
HBRUSH hGreenBrush;                                    /* green brush handle */
HBRUSH hBlueBrush;                                     /* blue brush handle  */


/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int APIENTRY WinMain(
    HANDLE hInstance,
    HANDLE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
    )
{
    MSG msg;

    UNREFERENCED_PARAMETER( lpCmdLine );

    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/
BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wc;

    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "OutputMenu";
    wc.lpszClassName = "OutputWClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(
    HANDLE          hInstance,
    INT             nCmdShow)
{
    HWND            hWnd;

    hInst = hInstance;

    hWnd = CreateWindow(
        "OutputWClass",
        "Output Sample Application",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_CREATE     - create window and objects
        WM_PAINT      - update window, draw objects
        WM_DESTROY    - destroy window

    COMMENTS:

        Handles to the objects you will use are obtained when the WM_CREATE
        message is received, and deleted when the WM_DESTROY message is
        received.  The actual drawing is done whenever a WM_PAINT message is
        received.

****************************************************************************/

LONG APIENTRY MainWndProc(
    HWND hWnd,
    UINT message,
    UINT wParam,
    LONG lParam)
{
    FARPROC lpProcAbout;

    HDC hDC;                          /* display-context variable  */
    PAINTSTRUCT ps;                   /* paint structure           */
    RECT rcTextBox;                   /* rectangle around the text */
    HPEN hOldPen;                     /* old pen handle            */



    switch (message) {
        case WM_COMMAND:
            if (LOWORD(wParam) == IDM_ABOUT) {
                lpProcAbout = MakeProcInstance((FARPROC)About, hInst);

                DialogBox(hInst,
                    "AboutBox",
                    hWnd,
                    lpProcAbout);

                FreeProcInstance(lpProcAbout);
                break;
            }
            else
                return (DefWindowProc(hWnd, message, wParam, lParam));

        case WM_CREATE:

            /* Create the brush objects */

            hRedBrush =   CreateSolidBrush(RGB(255,   0,   0));
            hGreenBrush = CreateSolidBrush(RGB(  0, 255,   0));
            hBlueBrush =  CreateSolidBrush(RGB(  0,   0, 255));

            /* Create the "---" pen */

            hDashPen = CreatePen(PS_DASH,                /* style */
                1,                                       /* width */
                RGB(0, 0, 0));                           /* color */

            /* Create the "..." pen */

            hDotPen = CreatePen(2,                       /* style */
                1,                                       /* width */
                RGB(0, 0, 0));                           /* color */
            break;

        case WM_PAINT:
            {
                TEXTMETRIC textmetric;
                INT nDrawX;
                INT nDrawY;
                CHAR szText[300];

                /* Set up a display context to begin painting */

                hDC = BeginPaint (hWnd, &ps);

                /* Get the size characteristics of the current font.  */
                /* This information will be used for determining the  */
                /* vertical spacing of text on the screen.            */

                GetTextMetrics (hDC, &textmetric);

                /* Initialize drawing position to 1/4 inch from the top  */
                /* and from the left of the top, left corner of the      */
                /* client area of the main windows.                      */

                nDrawX = GetDeviceCaps(hDC, LOGPIXELSX) / 4;   /* 1/4 inch */
                nDrawY = GetDeviceCaps(hDC, LOGPIXELSY) / 4;   /* 1/4 inch */

                /* Send characters to the screen.  After displaying each   */
                /* line of text, advance the vertical position for the     */
                /* next line of text.  The pixel distance between the top  */ 
                /* of each line of text is equal to the standard height of */
                /* the font characters (tmHeight), plus the standard       */
                /* amount of spacing (tmExternalLeading) between adjacent  */
                /* lines.                                                  */

                strcpy (szText, "These characters are being painted using ");
                TextOut (hDC, nDrawX, nDrawY, szText, strlen (szText));
                nDrawY += textmetric.tmExternalLeading + textmetric.tmHeight;

                strcpy (szText, "the TextOut() function, which is fast and ");
                TextOut (hDC, nDrawX, nDrawY, szText, strlen (szText));
                nDrawY += textmetric.tmExternalLeading + textmetric.tmHeight;

                strcpy (szText, "allows programmer control of placement and ");
                TextOut (hDC, nDrawX, nDrawY, szText, strlen (szText));
                nDrawY += textmetric.tmExternalLeading + textmetric.tmHeight;

                strcpy (szText, "formatting details.  However, TextOut() ");
                TextOut (hDC, nDrawX, nDrawY, szText, strlen (szText));
                nDrawY += textmetric.tmExternalLeading + textmetric.tmHeight;

                strcpy (szText, "does not provide any automatic formatting.");
                TextOut (hDC, nDrawX, nDrawY, szText, strlen (szText));
                nDrawY += textmetric.tmExternalLeading + textmetric.tmHeight;

                /* Put text in a 5-inch by 1-inch rectangle and display it. */
                /* First define the size of the rectangle around the text   */

                nDrawY += GetDeviceCaps(hDC, LOGPIXELSY) / 4;  /* 1/4 inch */
                SetRect (
                      &rcTextBox
                    , nDrawX
                    , nDrawY
                    , nDrawX + (5 * GetDeviceCaps(hDC, LOGPIXELSX)) /* 5" */
                    , nDrawY + (1 * GetDeviceCaps(hDC, LOGPIXELSY)) /* 1" */
                );

                /* Draw the text within the bounds of the above rectangle */

                strcpy (szText, "This text is being displayed with a single "
                            "call to DrawText().  DrawText() isn't as fast "
                            "as TextOut(), and it is somewhat more "
                            "constrained, but it provides numerous optional "
                            "formatting features, such as the centering and "
                            "line breaking used in this example.");
                DrawText (
                      hDC
                    , szText
                    , strlen (szText)
                    , &rcTextBox
                    , DT_CENTER | DT_EXTERNALLEADING | DT_NOCLIP
                                                | DT_NOPREFIX | DT_WORDBREAK
                );
            
                /*  Paint the next object immediately below the bottom of   */
                /*  the above rectangle in which the text was drawn.        */

                nDrawY = rcTextBox.bottom;

                /* The (x,y) pixel coordinates of the objects about to be   */
                /* drawn are below, and to the right of, the current        */
                /* coordinate (nDrawX,nDrawY).                              */

                /* Draw a red rectangle.. */

                hOldBrush = SelectObject(hDC, hRedBrush);
                Rectangle (
                      hDC
                    , nDrawX
                    , nDrawY
                    , nDrawX + 50
                    , nDrawY + 30
                );

                /* Draw a green ellipse */

                SelectObject(hDC, hGreenBrush);
                Ellipse (
                      hDC
                    , nDrawX + 150
                    , nDrawY
                    , nDrawX + 150 + 50
                    , nDrawY + 30
                );

                /* Draw a blue pie shape */

                SelectObject(hDC, hBlueBrush);
                Pie (
                      hDC
                    , nDrawX + 300
                    , nDrawY
                    , nDrawX + 300 + 50
                    , nDrawY + 50
                    , nDrawX + 300 + 50
                    , nDrawY
                    , nDrawX + 300 + 50
                    , nDrawY + 50
                );

                nDrawY += 50;

                /* Restore the old brush */

                SelectObject(hDC, hOldBrush);

                /* Select a "---" pen, save the old value */

                nDrawY += GetDeviceCaps(hDC, LOGPIXELSY) / 4;  /* 1/4 inch */
                hOldPen = SelectObject(hDC, hDashPen);

                /* Move to a specified point */

                MoveToEx(hDC, nDrawX, nDrawY, NULL );

                /* Draw a line */

                LineTo(hDC, nDrawX + 350, nDrawY);

                /* Select a "..." pen */

                SelectObject(hDC, hDotPen);

                /* Draw an arc connecting the line */

                Arc (
                      hDC
                    , nDrawX
                    , nDrawY - 20
                    , nDrawX + 350
                    , nDrawY + 20
                    , nDrawX
                    , nDrawY
                    , nDrawX + 350
                    , nDrawY
                );

                /* Restore the old pen */

                SelectObject(hDC, hOldPen);

                /* Tell Windows you are done painting */

                EndPaint (hWnd,  &ps);
            }
            break;

        case WM_DESTROY:
            DeleteObject(hRedBrush);
            DeleteObject(hGreenBrush);
            DeleteObject(hBlueBrush);
            DeleteObject(hDashPen);
            DeleteObject(hDotPen);
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0);
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL APIENTRY About(
    HWND hDlg,
    UINT message,
    UINT wParam,
    LONG lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK
                || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
        UNREFERENCED_PARAMETER(lParam);
}
