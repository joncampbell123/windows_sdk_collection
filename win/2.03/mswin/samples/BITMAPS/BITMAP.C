/****************************************************************************

    PROGRAM: Bitmap.c

    PURPOSE: Demonstrates how to use bitmap

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        BitmapInit() - initializes window data and registers window
        BitmapWndProc() - processes messages
        About() - processes messages for "About" dialog box
        MakeColorBitmap(HWND) - creates a color bitmap

    COMMENTS:

        This application is linked with select.exe which is a library
        module.  For the source code, look in the \select\ directory, and
        in Appendix C of the Windows Programmer's Learning Guide.

****************************************************************************/

#include "windows.h"
#include "bitmap.h"
#include "select.h"                          /* used to link with select.exe */

HANDLE hInst;

/* Patterns used for the background */

short White[] =  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
short Black[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
short Zigzag[] = { 0xFF, 0xF7, 0xEB, 0xDD, 0xBE, 0x7F, 0xFF, 0xFF };
short CrossHatch[] = { 0xEF, 0xEF, 0xEF, 0xEF, 0x00, 0xEF, 0xEF, 0xEF };

/* handles used for the various bitmaps */

HBITMAP hPattern1;
HBITMAP hPattern2;
HBITMAP hPattern3;
HBITMAP hPattern4;
HBITMAP hBitmap1;
HBITMAP hBitmap2;
HBITMAP hBitmap3;
HBITMAP hMenuBitmap1;
HBITMAP hMenuBitmap2;
HBITMAP hMenuBitmap3;
HBITMAP hBitmap;
HBITMAP hOldBitmap;

HBRUSH hBrush;                         /* brush handle                       */
int fStretchMode;                      /* type of stretch mode to use        */

HDC hDC;                               /* handle to device context           */
HDC hMemoryDC;                         /* handle to memory device context    */
BITMAP Bitmap;                         /* bitmap structure                   */

BOOL bTrack = FALSE;                   /* TRUE if user is selecting a region */
RECT Rect;

/* The following variables keep track of which menu item is checked */

WORD wPrevBitmap = IDM_BITMAP1;
WORD wPrevPattern = IDM_PATTERN1;
WORD wPrevMode = IDM_WHITEONBLACK;
WORD wPrevItem;

int Shape = SL_BLOCK;            /* shape to use for the selection rectangle */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{

    HWND hWnd;
    MSG msg;

    if (!hPrevInstance)
        if (!BitmapInit(hInstance))
            return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("Bitmap",
        "Bitmap Sample Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
        return (NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: BitmapInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL BitmapInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "Bitmap";
    pWndClass->lpszClassName = (LPSTR) "Bitmap";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = CS_VREDRAW | CS_HREDRAW;
    pWndClass->lpfnWndProc = BitmapWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: BitmapWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_SYSCOMMAND  - system menu (About dialog box)
        WM_CREATE      - create window and objects
        WM_LBUTTONDOWN - begin selection
        WM_MOUSEMOVE   - keep track of mouse movement during selection
        WM_LBUTTONUP   - end selection, draw bitmap
        WM_RBUTTONUP   - draw bitmap without resizing
        WM_ERASEBKGND  - erase background and redraw
        WM_DESTROY     - destroy window

    COMMENTS:

        User may select a "normal" size bitmap by pressing the right mouse
        button, or set the size of the bitmap to display by using the left
        button to define a region.  The routines to display the selection
        region are defined in the library module select.exe.

****************************************************************************/

long FAR PASCAL BitmapWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    HBRUSH hOldBrush;
    HBITMAP hOurBitmap;

    switch (message) {
        case WM_CREATE:                            /* message: create window */

            hPattern1 = CreateBitmap(8, 8, 1, 1, (LPSTR) White);
            hPattern2 = CreateBitmap(8, 8, 1, 1, (LPSTR) Black);
            hPattern3 = CreateBitmap(8, 8, 1, 1, (LPSTR) Zigzag);
            hPattern4 = CreateBitmap(8, 8, 1, 1, (LPSTR) CrossHatch);

            hBitmap1 = LoadBitmap(hInst, "dog");
            hBitmap2 = LoadBitmap(hInst, "cat");
            hBitmap3 = MakeColorBitmap(hWnd);

            hMenuBitmap1 = LoadBitmap(hInst, "dog");
            hMenuBitmap2 = LoadBitmap(hInst, "cat");
            hMenuBitmap3 = MakeColorBitmap(hWnd);

            hMenu = CreateMenu();
            ChangeMenu(hMenu, NULL, "&White", IDM_PATTERN1,
                MF_APPEND | MF_STRING | MF_CHECKED);
            ChangeMenu(hMenu, NULL, "&Black", IDM_PATTERN2,
                MF_APPEND | MF_STRING);
            ChangeMenu(hMenu, NULL, (LPSTR) (LONG) hPattern3, IDM_PATTERN3,
                MF_APPEND | MF_BITMAP);
            ChangeMenu(hMenu, NULL, (LPSTR) (LONG) hPattern4, IDM_PATTERN4,
                MF_APPEND | MF_BITMAP);

            ChangeMenu(GetMenu(hWnd), 1, "&Pattern", hMenu,
                MF_CHANGE | MF_POPUP | MF_BYPOSITION);

            hMenu = CreateMenu();

            /* Use bitmaps for menu items */

            ChangeMenu(hMenu, NULL, (LPSTR) (LONG) hMenuBitmap1, IDM_BITMAP1,
                MF_APPEND | MF_BITMAP | MF_CHECKED);
            ChangeMenu(hMenu, NULL, (LPSTR) (LONG) hMenuBitmap2, IDM_BITMAP2,
                MF_APPEND | MF_BITMAP);
            ChangeMenu(hMenu, NULL, (LPSTR) (LONG) hMenuBitmap3, IDM_BITMAP3,
                MF_APPEND | MF_BITMAP);

            ChangeMenu(GetMenu(hWnd), 0, "&Bitmap", hMenu,
                MF_CHANGE | MF_POPUP | MF_BYPOSITION);

            hBrush = CreatePatternBrush(hPattern1);
            fStretchMode = IDM_BLACKONWHITE;

            /* Select the first bitmap */

            hDC = GetDC(hWnd);
            hMemoryDC = CreateCompatibleDC(hDC);
            ReleaseDC(hWnd, hDC);
            hOldBitmap = SelectObject(hMemoryDC, hBitmap1);
            GetObject(hBitmap1, 16, (LPSTR) &Bitmap);

            hMenu = GetSystemMenu(hWnd, FALSE);
            ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
            ChangeMenu(hMenu, NULL, "A&bout Bitmap...", ID_ABOUT,
                MF_APPEND | MF_STRING);
            break;

        case WM_LBUTTONDOWN:           /* message: left mouse button pressed */

            /* Start selection of region */

            bTrack = TRUE;
            SetRectEmpty(&Rect);
            StartSelection(hWnd, MAKEPOINT(lParam), &Rect,
                (wParam & MK_SHIFT) ? (SL_EXTEND | Shape) : Shape);
            break;

        case WM_MOUSEMOVE:                        /* message: mouse movement */

            /* Update the selection region */

            if (bTrack)
                UpdateSelection(hWnd, MAKEPOINT(lParam), &Rect, Shape);
            break;

        case WM_LBUTTONUP:            /* message: left mouse button released */

            /* End the selection */

            bTrack = FALSE;
            EndSelection(MAKEPOINT(lParam), &Rect);
            ClearSelection(hWnd, &Rect, Shape);

            hDC = GetDC(hWnd);
            SetStretchBltMode(hDC, fStretchMode);
            StretchBlt(hDC, Rect.left, Rect.top,
                Rect.right - Rect.left, Rect.bottom - Rect.top,
                hMemoryDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, SRCCOPY);
            ReleaseDC(hWnd, hDC);
            break;

        case WM_RBUTTONUP:           /* message: right mouse button released */

            /* Display a normal sized bitmap */

            hDC = GetDC(hWnd);
            BitBlt(hDC, LOWORD(lParam), HIWORD(lParam),
                Bitmap.bmWidth, Bitmap.bmHeight, hMemoryDC, 0, 0, SRCCOPY);
            ReleaseDC(hWnd, hDC);
            break;

        case WM_ERASEBKGND:                    /* messsage: erase background */

            /* Repaint the background */

            UnrealizeObject(hBrush);
            hOldBrush = SelectObject(wParam, hBrush);
            GetClientRect(hWnd, &Rect);
            PatBlt(wParam, Rect.left, Rect.top, 
                Rect.right-Rect.left, Rect.bottom-Rect.top, PATCOPY);
            SelectObject(wParam, hOldBrush);
            return TRUE;

        case WM_COMMAND:                         /* message: Windows command */
            switch (wParam) {

                /* Bitmap menu: select the bitmap to use for drawing */

                case IDM_BITMAP1:
                    wPrevItem = wPrevBitmap;
                    wPrevBitmap = wParam;
                    GetObject(hBitmap1, 16, (LPSTR) &Bitmap);
                    SelectObject(hMemoryDC, hBitmap1);
                    break;

                case IDM_BITMAP2:
                    wPrevItem = wPrevBitmap;
                    wPrevBitmap = wParam;
                    GetObject(hBitmap2, 16, (LPSTR) &Bitmap);
                    SelectObject(hMemoryDC, hBitmap2);
                    break;

                case IDM_BITMAP3:
                    wPrevItem = wPrevBitmap;
                    wPrevBitmap = wParam;
                    GetObject(hBitmap3, 16, (LPSTR) &Bitmap);
                    hOurBitmap = SelectObject(hMemoryDC, hBitmap3);
                    break;
 
                /* Pattern menu: select the background brush to use */

                case IDM_PATTERN1:
                    wPrevItem = wPrevPattern;
                    wPrevPattern = wParam;
                    DeleteObject(hBrush);
                    hBrush = CreatePatternBrush(hPattern1);
                    InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;

                case IDM_PATTERN2:
                    wPrevItem = wPrevPattern;
                    wPrevPattern = wParam;
                    DeleteObject(hBrush);
                    hBrush = CreatePatternBrush(hPattern2);
                    InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;

                case IDM_PATTERN3:
                    wPrevItem = wPrevPattern;
                    wPrevPattern = wParam;
                    DeleteObject(hBrush);
                    hBrush = CreatePatternBrush(hPattern3);
                    InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;

                case IDM_PATTERN4:
                    wPrevItem = wPrevPattern;
                    wPrevPattern = wParam;
                    DeleteObject(hBrush);
                    hBrush = CreatePatternBrush(hPattern4);
                    InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
                    UpdateWindow(hWnd);
                    break;

                /* Mode menu: select the stretch mode to use */

                case IDM_BLACKONWHITE:
                    wPrevItem = wPrevMode;
                    wPrevMode = wParam;
                    fStretchMode = BLACKONWHITE;
                    break;

                case IDM_WHITEONBLACK:
                    wPrevItem = wPrevMode;
                    wPrevMode = wParam;
                    fStretchMode = WHITEONBLACK;
                    break;

                case IDM_COLORONCOLOR:
                    wPrevItem = wPrevMode;
                    wPrevMode = wParam;
                    fStretchMode = COLORONCOLOR;
                    break;
            }

            /* Uncheck the old item, check the new item */

            CheckMenuItem(GetMenu(hWnd), wPrevItem, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
            break;

        case WM_SYSCOMMAND:                          /* message: system menu */
            if (wParam == ID_ABOUT) {
                lpProcAbout = MakeProcInstance(About, hInst);
                DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                FreeProcInstance(lpProcAbout);
                break;
            }
            else
                return (DefWindowProc(hWnd, message, wParam, lParam));

        case WM_DESTROY:                          /* message: destroy window */
            SelectObject(hMemoryDC, hOldBitmap);
            DeleteDC(hMemoryDC);
            DeleteObject(hBrush);
            DeleteObject(hPattern1);
            DeleteObject(hPattern2);
            DeleteObject(hPattern3);
            DeleteObject(hPattern4);
            DeleteObject(hBitmap1);
            DeleteObject(hBitmap2);
            DeleteObject(hBitmap3);
            DeleteObject(hMenuBitmap1);
            DeleteObject(hMenuBitmap2);
            DeleteObject(hMenuBitmap3);

            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: MakeColorBitmap(HWND)

    PURPOSE: Creates a color bitmap

    COMMENTS:

        This creates a plaid color bitmap by using overlappying colors.

****************************************************************************/

HBITMAP MakeColorBitmap(hWnd)
HWND hWnd;
{
    HDC hDC;
    HDC hMemoryDC;
    HBITMAP hBitmap;
    HBITMAP hOldBitmap;
    HBRUSH hRedBrush;
    HBRUSH hGreenBrush;
    HBRUSH hBlueBrush;
    HBRUSH hOldBrush;

    hDC = GetDC(hWnd);
    hMemoryDC = CreateCompatibleDC(hDC);
    hBitmap = CreateCompatibleBitmap(hDC, 64, 32);
    hOldBitmap = SelectObject(hMemoryDC, hBitmap);
    hRedBrush = CreateSolidBrush(RGB(255,0,0));
    hGreenBrush = CreateSolidBrush(RGB(0,255,0));
    hBlueBrush = CreateSolidBrush(RGB(0,0,255));

    PatBlt(hMemoryDC, 0, 0, 64, 32, BLACKNESS);
    hOldBrush = SelectObject(hMemoryDC, hRedBrush);
    PatBlt(hMemoryDC, 0, 0, 24, 11, PATORDEST);
    PatBlt(hMemoryDC, 40, 10, 24, 12, PATORDEST);
    PatBlt(hMemoryDC, 20, 21, 24, 11, PATORDEST);
    SelectObject(hMemoryDC, hGreenBrush);
    PatBlt(hMemoryDC, 20, 0, 24, 11, PATORDEST);
    PatBlt(hMemoryDC, 0, 10, 24, 12, PATORDEST);
    PatBlt(hMemoryDC, 40, 21, 24, 11, PATORDEST);
    SelectObject(hMemoryDC, hBlueBrush);
    PatBlt(hMemoryDC, 40, 0, 24, 11, PATORDEST);
    PatBlt(hMemoryDC, 20, 10, 24, 12, PATORDEST);
    PatBlt(hMemoryDC, 0, 21, 24, 11, PATORDEST);

    SelectObject(hMemoryDC, hOldBrush);
    DeleteObject(hRedBrush);
    DeleteObject(hGreenBrush);
    DeleteObject(hBlueBrush);
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(hWnd, hDC);
    return (hBitmap);
}
