/*******************************************************************************
 *                                                                             *
 *  PROGRAM     : ShowDIB.c                                                    *
 *                                                                             *
 *  PURPOSE     : Application to illustrate the use of the GDI                 *
 *                DIB (Device Independent Bitmap) and Palette manager          *
 *                functions.                                                   *
 *                                                                             *
 *  FUNCTIONS   : WinMain ()             -  Creates the app. window and enters *
 *                                          the message loop.                  *
 *                                                                             *
 *                WndProc()              -  Processes app. window messages.    *
 *                                                                             *
 *                MenuCommand()          -  Processes menu commands.           *
 *                                                                             *
 *                FreeDIB()              -  Frees currently active objects.    *
 *                                                                             *
 *                InitDIB()              -  Reads DIB from a file and loads it.*
 *                                                                             *
 *******************************************************************************/

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include "showdib.h"
#include "commdlg.h"

DIBPARAMS      DIBParams;                  /* params for the SETSCALING escape */
char           achFileName[128] = "";
DWORD          dwOffset;
NPLOGPALETTE   pLogPal;
HPALETTE       hpalSave = NULL;
HANDLE         hInst ;
RECT           rcClip;
static	       HCURSOR hcurSave;

BOOL    fPalColors  = FALSE;          /* TRUE if the current DIB's color table   */
                                      /* contains palette indexes not rgb values */
WORD    nAnimating  = 0;              /* Palette animation count                 */
WORD    UpdateCount = 0;

BOOL    bMemoryDIB    = FALSE; /* Load Entire DIB into memory in CF_DIB format */
BOOL    bUpdateColors = TRUE;  /* Directly update screen colors                */
BOOL    bDIBToDevice  = FALSE; /* Use SetDIBitsToDevice() to BLT data.         */
BOOL    bNoUgly       = FALSE; /* Make window black on a WM_PALETTEISCHANGING  */
BOOL    bLegitDraw    = FALSE; /* We have a valid bitmap to draw               */

char	szBitmapExt[] = "Bitmaps\0*.BMP; *.DIB; *.RLE\0";     /* possible file extensions */
WORD    wTransparent  = TRANSPARENT;               /* Mode of DC               */
char    szAppName[]   = "ShowDIB" ;                /* App. name                */

HPALETTE hpalCurrent   = NULL;         /* Handle to current palette            */
HANDLE   hdibCurrent   = NULL;         /* Handle to current memory DIB         */
HBITMAP  hbmCurrent    = NULL;         /* Handle to current memory BITMAP      */
HANDLE   hbiCurrent    = NULL;         /* Handle to current bitmap info struct */
HWND     hWndApp;                      /* Handle to app. window                */

/* Styles of app. window */
DWORD          dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                         WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;

void PrintDIB (HWND hWnd, HDC hDC, int x, int y, int dx, int dy);
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : WinMain(HANDLE, HANDLE, LPSTR, int)                        *
 *                                                                          *
 *  PURPOSE    : Creates the app. window and enters the message loop.       *
 *                                                                          *
 ****************************************************************************/
int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)

HANDLE hInstance, hPrevInstance ;
LPSTR  lpszCmdLine ;
int    nCmdShow ;
{
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG         msg ;
     short       xScreen, yScreen ;
     char        ach[40];
     DWORD	 dwWinFlags;

     hInst = hInstance ;

     /* default to MEMORY DIB's if XWindows */
     dwWinFlags = GetWinFlags();
     bMemoryDIB = (BOOL)(dwWinFlags & WF_PMODE);

     /* Initialize clip rectangle */
     SetRectEmpty(&rcClip);

     if (!hPrevInstance) {
         wndclass.style         = CS_DBLCLKS;
         wndclass.lpfnWndProc   = WndProc ;
         wndclass.cbClsExtra    = 0 ;
         wndclass.cbWndExtra    = 0 ;
         wndclass.hInstance     = hInstance ;
         wndclass.hIcon         = LoadIcon(hInst, "SHOWICON");
         wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	 wndclass.hbrBackground = COLOR_WINDOW+1 ;
         wndclass.lpszMenuName  = szAppName ;
         wndclass.lpszClassName = szAppName ;

         if (!RegisterClass (&wndclass))
                 return FALSE ;
     }

     if (!GetProfileString("extensions", "bmp", "", ach, sizeof(ach)))
             WriteProfileString("extensions", "bmp", "showdib.exe ^.bmp");
     if (!GetProfileString("extensions", "dib", "", ach, sizeof(ach)))
             WriteProfileString("extensions", "dib", "showdib.exe ^.dib");

     /* Save the pointer to the command line */
     lstrcpy(achFileName, lpszCmdLine);

     xScreen = GetSystemMetrics (SM_CXSCREEN) ;
     yScreen = GetSystemMetrics (SM_CYSCREEN) ;

     /* Create the app. window */
     hWnd = CreateWindow( szAppName,
                          szAppName,
                          dwStyle,
                          CW_USEDEFAULT,
                          0,
                          xScreen / 2,
                          yScreen / 2,
                          NULL,
                          NULL,
                          hInstance,
                          NULL) ;

     ShowWindow (hWndApp = hWnd, nCmdShow) ;

     /* Enter message loop */
     while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
     }

     return msg.wParam ;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : WndProc (hWnd, iMessage, wParam, lParam)                   *
 *                                                                          *
 *  PURPOSE    : Processes window messages.                                 *
 *                                                                          *
 ****************************************************************************/
long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)

HWND     hWnd ;
UINT iMessage ;
WPARAM	 wParam ;
LPARAM	 lParam ;

{
    PAINTSTRUCT      ps;
    HDC              hDC;
    HANDLE           h;
    WORD             i;
    int              iMax;
    int              iMin;
    int              iPos;
    int              dn;
    RECT             rc,Rect;
    HPALETTE         hOldPal;

    switch (iMessage) {
        case WM_DESTROY:
                /* Clean up and quit */
                FreeDib();
                PostQuitMessage(0);
                break ;

        case WM_CREATE:
                /* Allocate space for our logical palette */
                pLogPal = (NPLOGPALETTE) LocalAlloc( LMEM_FIXED,
                                                     (sizeof(LOGPALETTE) +
                                                     (sizeof(PALETTEENTRY)*(MAXPALETTE))));

                /* If DIB initialization fails, quit */
                if (achFileName[0] && !InitDIB(hWnd))
                        PostQuitMessage (3) ;

                /* fall through */

        case WM_PALETTEISCHANGING:
                /* if SHOWDIB was not responsible for palette change and if
                 * ok to hide changes, paint app. window black.
                 */
                if (wParam != hWnd && bNoUgly) {
                    GetClientRect(hWnd, &Rect);

                    hDC = GetDC(hWnd);
                    FillRect( hDC, (LPRECT) &Rect, GetStockObject(BLACK_BRUSH));
                    ReleaseDC(hWnd, hDC);
                }
                break;

        case WM_ACTIVATE:
                if (!wParam)  /* app. is being de-activated */
                   break;
                /* If the app. is moving to the foreground, fall through and
                 * redraw full client area with the newly realized palette,
                 * if the palette has changed.
                 */

        case WM_QUERYNEWPALETTE:
                /* If palette realization causes a palette change,
                 * we need to do a full redraw.
                 */
                if (bLegitDraw) {
                    hDC = GetDC (hWnd);
                    hOldPal = SelectPalette (hDC, hpalCurrent, 0);

                    i = RealizePalette(hDC);

                    SelectPalette (hDC, hOldPal, 0);
                    ReleaseDC (hWnd, hDC);

                    if (i) {
                        InvalidateRect (hWnd, (LPRECT) (NULL), 1);
                        UpdateCount = 0;
                        return 1;
                    } else
                        return FALSE;
                }
                else
                    return FALSE;
                break;

        case WM_PALETTECHANGED:
                /* if SHOWDIB was not responsible for palette change and if
                 * palette realization causes a palette change, do a redraw.
                 */
                 if (wParam != hWnd){
                    if (bLegitDraw){
                        hDC = GetDC (hWnd);
                        hOldPal = SelectPalette (hDC, hpalCurrent, 0);

                        i = RealizePalette (hDC);

                        if (i){
                            if (bUpdateColors){
                                UpdateColors (hDC);
                                UpdateCount++;
                            }
                            else
                                InvalidateRect (hWnd, (LPRECT) (NULL), 1);
                        }

                        SelectPalette (hDC, hOldPal, 0);
                        ReleaseDC (hWnd, hDC);
                    }
                }
                break;

        case WM_RENDERALLFORMATS:
                /* Ensure that clipboard data can be rendered even tho'
                 * app. is being destroyed.
                 */
                SendMessage(hWnd,WM_RENDERFORMAT,CF_DIB,0L);
                SendMessage(hWnd,WM_RENDERFORMAT,CF_BITMAP,0L);
                SendMessage(hWnd,WM_RENDERFORMAT,CF_PALETTE,0L);
                break;

        case WM_RENDERFORMAT:
                /* Format data in manner specified and pass the data
                 * handle to clipboard.
                 */
                if (h = RenderFormat(wParam))
                    SetClipboardData(wParam,h);
                break;

        case WM_COMMAND:
                /* Process menu commands */
                return MenuCommand (hWnd, wParam);
                break;

        case WM_TIMER:
                /* Signal for palette animation */
                hDC = GetDC(hWnd);
                hOldPal = SelectPalette(hDC, hpalCurrent, 0);
                {
                    PALETTEENTRY peTemp;

                    /* Shift all palette entries left by one position and wrap
                     * around the first entry
                     */
                    peTemp = pLogPal->palPalEntry[0];
                    for (i = 0; i < (pLogPal->palNumEntries - 1); i++)
                         pLogPal->palPalEntry[i] = pLogPal->palPalEntry[i+1];
                    pLogPal->palPalEntry[i] = peTemp;
                }
                /* Replace entries in logical palette with new entries*/
                AnimatePalette(hpalCurrent, 0, pLogPal->palNumEntries, pLogPal->palPalEntry);

                SelectPalette(hDC, hOldPal, 0);
                ReleaseDC(hWnd, hDC);

                /* Decrement animation count and terminate animation
                 * if it reaches zero
                 */
                if (!(--nAnimating))
                    PostMessage(hWnd,WM_COMMAND,IDM_ANIMATE0,0L);
                break;

        case WM_PAINT:
                /* If we have updated more than once, the rest of our
                 * window is not in some level of degradation worse than
                 * our redraw...  we need to redraw the whole area
                 */
                if (UpdateCount > 1) {
                    BeginPaint(hWnd, &ps);
                    EndPaint(hWnd, &ps);
                    UpdateCount = 0;
                    InvalidateRect(hWnd, (LPRECT) (NULL), 1);
                    break;
                }

                hDC = BeginPaint(hWnd, &ps);
                AppPaint(hWnd,
                         hDC,
                         GetScrollPos(hWnd,SB_HORZ),
                         GetScrollPos(hWnd,SB_VERT) );
                EndPaint(hWnd, &ps);
                break ;

        case WM_SIZE:
            SetScrollRanges(hWnd);
            break;

        case WM_KEYDOWN:
            /* Translate keyboard messages to scroll commands */
            switch (wParam) {
                case VK_UP:    PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,   0L);
                               break;

                case VK_DOWN:  PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
                               break;

                case VK_PRIOR: PostMessage (hWnd, WM_VSCROLL, SB_PAGEUP,   0L);
                               break;

                case VK_NEXT:  PostMessage (hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
                               break;

                case VK_HOME:  PostMessage (hWnd, WM_HSCROLL, SB_PAGEUP,   0L);
                               break;

                case VK_END:   PostMessage (hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
                               break;

                case VK_LEFT:  PostMessage (hWnd, WM_HSCROLL, SB_LINEUP,   0L);
                               break;

                case VK_RIGHT: PostMessage (hWnd, WM_HSCROLL, SB_LINEDOWN, 0L);
                               break;
            }
            break;

        case WM_KEYUP:
            switch (wParam) {
               case VK_UP:
               case VK_DOWN:
               case VK_PRIOR:
               case VK_NEXT:
                  PostMessage (hWnd, WM_VSCROLL, SB_ENDSCROLL, 0L);
                  break;

               case VK_HOME:
               case VK_END:
               case VK_LEFT:
               case VK_RIGHT:
                  PostMessage (hWnd, WM_HSCROLL, SB_ENDSCROLL, 0L);
                  break;
            }
            break;

        case WM_VSCROLL:
            /* Calculate new vertical scroll position */
            GetScrollRange (hWnd, SB_VERT, &iMin, &iMax);
            iPos = GetScrollPos (hWnd, SB_VERT);
            GetClientRect (hWnd, &rc);

            switch (wParam) {
                case SB_LINEDOWN:      dn =  rc.bottom / 16 + 1;
                                       break;

                case SB_LINEUP:        dn = -rc.bottom / 16 + 1;
                                       break;

                case SB_PAGEDOWN:      dn =  rc.bottom / 2  + 1;
                                       break;

                case SB_PAGEUP:        dn = -rc.bottom / 2  + 1;
                                       break;

                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: dn = LOWORD(lParam)-iPos;
                                       break;

                default:               dn = 0;
            }
            /* Limit scrolling to current scroll range */
            if (dn = BOUND (iPos + dn, iMin, iMax) - iPos) {
                ScrollWindow (hWnd, 0, -dn, NULL, NULL);
                SetScrollPos (hWnd, SB_VERT, iPos + dn, TRUE);
            }
            break;

        case WM_HSCROLL:
            /* Calculate new horizontal scroll position */
            GetScrollRange (hWnd, SB_HORZ, &iMin, &iMax);
            iPos = GetScrollPos (hWnd, SB_HORZ);
            GetClientRect (hWnd, &rc);

            switch (wParam) {
                case SB_LINEDOWN:      dn =  rc.right / 16 + 1;
                                       break;

                case SB_LINEUP:        dn = -rc.right / 16 + 1;
                                       break;

                case SB_PAGEDOWN:      dn =  rc.right / 2  + 1;
                                       break;

                case SB_PAGEUP:        dn = -rc.right / 2  + 1;
                                       break;

                case SB_THUMBTRACK:
                case SB_THUMBPOSITION: dn = LOWORD (lParam) - iPos;
                                       break;

                default:               dn = 0;
            }
            /* Limit scrolling to current scroll range */
            if (dn = BOUND (iPos + dn, iMin, iMax) - iPos) {
                ScrollWindow (hWnd, -dn, 0, NULL, NULL);
                SetScrollPos (hWnd, SB_HORZ, iPos + dn, TRUE);
            }
            break;

        case WM_LBUTTONDOWN:
            /* Start rubberbanding a rect. and track it's dimensions.
             * set the clip rectangle to it's dimensions.
             */
            TrackMouse (hWnd, MAKEPOINT (lParam));
            break;

        case WM_LBUTTONDBLCLK:
            break;

        case WM_INITMENU:
            /* check/uncheck menu items depending on state  of related
             * flags
             */
            CheckMenuItem(wParam, IDM_UPDATECOL,
                (bUpdateColors ? MF_CHECKED : MF_UNCHECKED));
            CheckMenuItem(wParam, IDM_TRANSPARENT,
                (wTransparent == TRANSPARENT ? MF_CHECKED : MF_UNCHECKED));
            CheckMenuItem(wParam, IDM_DIBSCREEN,
                (bDIBToDevice ? MF_CHECKED : MF_UNCHECKED));
            CheckMenuItem(wParam, IDM_NOUGLY,
                (bNoUgly ? MF_CHECKED : MF_UNCHECKED));
            CheckMenuItem(wParam, IDM_MEMORYDIB,
                (bMemoryDIB ? MF_CHECKED : MF_UNCHECKED));
            EnableMenuItem(wParam, IDM_PASTEDIB,
                IsClipboardFormatAvailable(CF_DIB)?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(wParam, IDM_PASTEDDB,
                IsClipboardFormatAvailable(CF_BITMAP)?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(wParam, IDM_PASTEPAL,
                IsClipboardFormatAvailable(CF_PALETTE)?MF_ENABLED:MF_GRAYED);
            EnableMenuItem(wParam, IDM_PRINT,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_SAVE,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_COPY,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);

            EnableMenuItem(wParam, IDM_ANIMATE0,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_ANIMATE5,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_ANIMATE50,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_ANIMATE100,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_ANIMATE200,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_ANIMATE201,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(wParam, IDM_STEALCOL,
                bLegitDraw ? MF_ENABLED : MF_GRAYED);
            break;

        default:
            return DefWindowProc (hWnd, iMessage, wParam, lParam) ;

    }
    return 0L ;

}
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : MenuCommand ( HWND hWnd, WORD wParam)                      *
 *                                                                          *
 *  PURPOSE    : Processes menu commands.                                   *
 *                                                                          *
 *  RETURNS    : TRUE  - if command could be processed.                     *
 *               FALSE - otherwise                                          *
 *                                                                          *
 ****************************************************************************/
BOOL MenuCommand (hWnd, id)
HWND hWnd;
WORD id;

{
    BITMAPINFOHEADER bi;
    HDC              hDC;
    HANDLE           h;
    HBITMAP          hbm;
    HPALETTE         hpal;
    WORD             i;
    char             Name[40];
    BOOL             bSave;
    int              xSize, ySize, xRes, yRes, dx, dy;
    RECT             Rect;
    int              fh;
    WORD             fFileOptions;
    OPENFILENAME     ofn;

    switch (id) {
        case IDM_ABOUT:
                /* Show About .. box */
                fDialog (ABOUTBOX, hWnd,AppAbout);
                break;

        case IDM_COPY:
                if (!bLegitDraw)
                    return 0L;

                /* Clean clipboard of contents */
                if (OpenClipboard(hWnd)) {
                    EmptyClipboard ();
                    SetClipboardData (CF_DIB     ,NULL);
                    SetClipboardData (CF_BITMAP  ,NULL);
                    SetClipboardData (CF_PALETTE ,NULL);
                    CloseClipboard ();
                }
                break;

        case IDM_PASTEPAL:
                if (OpenClipboard (hWnd)) {
                    if (h = GetClipboardData (CF_PALETTE)) {
                        /* Delete current palette and get the CF_PALETTE data
                         * from the clipboard
                         */
                        if (hpalCurrent)
                            DeleteObject (hpalCurrent);

                        hpalCurrent = CopyPalette (h);

                        /*
                         * If we have a bitmap realized against the old palette
                         * delete the bitmap and rebuild it using the new palette.
                         */
                        if (hbmCurrent){
                            DeleteObject (hbmCurrent);
                            hbmCurrent = NULL;

                            if (hdibCurrent)
                                hbmCurrent = BitmapFromDib (hdibCurrent, hpalCurrent);
                        }
                    }
                    CloseClipboard();
                }
                break;

        case IDM_PASTEDIB:
                if (OpenClipboard (hWnd)) {
                    if (h = GetClipboardData (CF_DIB)) {
                        /* Delete current DIB and get CF_DIB and
                         * CF_PALETTE format data from the clipboard
                         */
                        hpal = GetClipboardData (CF_PALETTE);

                        FreeDib();
                        hdibCurrent = CopyHandle (h);
                        if (hdibCurrent) {
                            bLegitDraw = TRUE;
                            lstrcpy(achFileName,"<Clipboard>");
                            hbiCurrent = hdibCurrent;

                            /* If there is a CF_PALETTE object in the
                             * clipboard, this is the palette to assume
                             * the DIB should be realized against, otherwise
                             * create a palette for it.
                             */
                            if (hpal)
                                hpalCurrent = CopyPalette (hpal);
                            else
                                hpalCurrent = CreateDibPalette (hdibCurrent);

                            SizeWindow(hWnd);
                        }
                        else {
                            bLegitDraw = FALSE;
                            ErrMsg("No Memory Available!");
                        }
                    }
                    CloseClipboard();
                }
                break;

        case IDM_PASTEDDB:
                if (OpenClipboard (hWnd)) {
                    if (hbm = GetClipboardData(CF_BITMAP)) {
                        hpal = GetClipboardData(CF_PALETTE);
                        FreeDib();

                        /*
                         * If there is a CF_PALETTE object in the
                         * clipboard, this is the palette to assume
                         * the bitmap is realized against.
                         */
                        if (hpal)
                            hpalCurrent = CopyPalette(hpal);
                        else
                            hpalCurrent = GetStockObject(DEFAULT_PALETTE);

                        hdibCurrent = DibFromBitmap(hbm,BI_RGB,0,hpalCurrent);

                        if (hdibCurrent) {
                            bLegitDraw = TRUE;
                            lstrcpy(achFileName,"<Clipboard>");
                            hbiCurrent = hdibCurrent;

                            if (bMemoryDIB)
                                hbmCurrent = BitmapFromDib (hdibCurrent, hpalCurrent);

                            SizeWindow(hWnd);
                        }
                        else {
                            bLegitDraw = FALSE;
                            ErrMsg("No Memory Available!");
                        }
                    }
                    CloseClipboard ();
                }
                break;

        case IDM_PRINT:
                GetWindowText(hWnd, Name, sizeof(Name));

                DibInfo(hbiCurrent, &bi);

                if (!IsRectEmpty(&rcClip))
                {
                    bi.biWidth  = rcClip.right  - rcClip.left;
                    bi.biHeight = rcClip.bottom - rcClip.top;
                }

                /* Initialise printer stuff */
                if (!(hDC = GetPrinterDC()))
                        break;

                xSize = GetDeviceCaps(hDC, HORZRES);
                ySize = GetDeviceCaps(hDC, VERTRES);
                xRes  = GetDeviceCaps(hDC, LOGPIXELSX);
                yRes  = GetDeviceCaps(hDC, LOGPIXELSY);

                /* Use half inch margins on left and right
                 * and one inch on top. Maintain the same aspect ratio.
                 */

                dx = xSize - xRes;
                dy = (int)((long)dx * bi.biHeight/bi.biWidth);

                /* Fix bounding rectangle for the picture .. */
                Rect.top    = yRes;
                Rect.left   = xRes / 2;
                Rect.bottom = yRes + dy;
                Rect.right  = xRes / 2 + dx;

                /* ... and inform the driver */
                Escape(hDC, SET_BOUNDS, sizeof(RECT), (LPSTR)&Rect, NULL);

                bSave = TRUE;

                if (InitPrinting(hDC, hWnd, hInst, Name)) {

                        PrintDIB(hWnd, hDC, xRes/2, yRes, dx, dy);

                        /* Signal to the driver to begin translating the drawing
                         * commands to printer output...
                         */
                        Escape (hDC, NEWFRAME, NULL, NULL, NULL);

                        TermPrinting(hDC);
                }

                DeleteDC(hDC);
                break;

        case IDM_OPEN:
                /* Bring up File/Open ... dialog */
	       ofn.lStructSize = sizeof(OPENFILENAME);
	       ofn.hwndOwner = hWnd;
	       ofn.hInstance = hInst;
	       ofn.lpstrFilter = szBitmapExt;
	       ofn.lpstrCustomFilter = NULL;
	       ofn.nMaxCustFilter = 0L;
	       ofn.nFilterIndex = 1L;
	       ofn.lpstrFile = (LPSTR)achFileName;
	       ofn.nMaxFile = 128;
	       ofn.lpstrInitialDir = NULL;
	       ofn.lpstrTitle = NULL;
	       ofn.lpstrFileTitle = NULL;
	       ofn.lpstrDefExt = NULL;
	       ofn.Flags = 0;

	       fh=GetOpenFileName((LPOPENFILENAME)&ofn);

	      /*  Load up the DIB if the user did not press cancel */
                if (fh > 0) {
                   StartWait();
                   if (InitDIB (hWnd))
                       InvalidateRect (hWnd, NULL, FALSE);
                   else
                       bLegitDraw = FALSE;
                   EndWait();
                }
                break;

        case IDM_SAVE:
                DibInfo(hbiCurrent,&bi);
                fFileOptions = 0;

                /* Depending on compression type for current DIB,
                 * set the appropriate bit in the fFileOptions flag
                 */
                if (bi.biCompression == BI_RGB)
                    fFileOptions |= F_RGB;
                else if (bi.biCompression == BI_RLE4)
                    fFileOptions |= F_RLE4;
                else if (bi.biCompression == BI_RLE8)
                    fFileOptions |= F_RLE8;

                /* Depending on bits/pixel type for current DIB,
                 * set the appropriate bit in the fFileOptions flag
                 */
                switch (bi.biBitCount){
                    case  1:
                        fFileOptions |= F_1BPP;
                        break;

                    case  4:
                        fFileOptions |= F_4BPP;
                        break;

                    case  8:
                        fFileOptions |= F_8BPP;
                        break;

                    case 24:
                        fFileOptions |= F_24BPP;
                }

                /* Bring up File/Save... dialog and get info. about filename,
                 * compression, and bits/pix. of DIB to be written.
                 */

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.hInstance = hInst;
		ofn.lpstrFilter = szBitmapExt;
		ofn.lpstrCustomFilter = NULL;
		ofn.nMaxCustFilter = 0;
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = (LPSTR)achFileName;
		ofn.nMaxFile = 128;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle = NULL;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
		ofn.nFileOffset = 0;
		ofn.nFileExtension = 0;
		ofn.lpstrDefExt = (LPSTR)"";
		ofn.lCustData = 0L;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;

		fh=GetSaveFileName(&ofn);
		  /*   if (GetSaveFileName(&ofn))
		  {
		  OFSTRUCT of;
		  int cch;
		  char* pch;
		  HFILE hfile;
		  HLOCAL h;

		  hfile = OpenFile(szFilename, &of, OF_CREATE | OF_WRITE);

		  if (hfile != HFILE_ERROR)
		     {
		     h = Edit_GetHandle(hwndClient);
		     cch = Edit_GetTextLength(hwndClient);
		     pch = LocalLock(h);
		     if ((int)_lwrite(hfile, pch, cch) == cch)
		     fSuccess = TRUE;

		     LocalUnlock(h);
		     _lclose(hfile);
		     }
		  }*/




                /* Extract DIB specs. if the user did not press cancel */
                if (fh != 0){
                    if (fFileOptions & F_RGB)
                        bi.biCompression = BI_RGB;

                    if (fFileOptions & F_RLE4)
                        bi.biCompression = BI_RLE4;

                    if (fFileOptions & F_RLE8)
                        bi.biCompression = BI_RLE8;

                    if (fFileOptions & F_1BPP)
                        bi.biBitCount = 1;

                    if (fFileOptions & F_4BPP)
                        bi.biBitCount = 4;

                    if (fFileOptions & F_8BPP)
                        bi.biBitCount = 8;

                    if (fFileOptions & F_24BPP)
                        bi.biBitCount = 24;

                    /* Realize a DIB in the specified format and obtain a
                     * handle to it.
                     */
                    hdibCurrent = RealizeDibFormat(bi.biCompression,bi.biBitCount);
                    if (!hdibCurrent){
                        ErrMsg("Unable to save the specified file");
                        return 0L;
                    }

                    /* Write the DIB */
                    StartWait();
                    if (!WriteDIB(achFileName,hdibCurrent))
                        ErrMsg("Unable to save the specified file");
                    EndWait();
                }
                break;

        case IDM_EXIT:
                PostMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0L);
                break;

        case IDM_UPDATECOL:
                /* Toggle state of "update screen colors" flag. If it is
                 * off, clear the "hide changes" flag
                 */
                bUpdateColors = !bUpdateColors;
                if (bUpdateColors)
                    bNoUgly = 0;
                break;

        case IDM_DIBSCREEN:
                bDIBToDevice = !bDIBToDevice;
                InvalidateRect(hWnd, (LPRECT) (NULL), 1);
                break;

        case IDM_MEMORYDIB:
                bMemoryDIB = !bMemoryDIB;
                break;

        case IDM_NOUGLY:
                /* Toggle state of "hide changes" flag. If it is off, clear
                 * the "update screen colors" flag. This will tell SHOWDIB
                 * to paint itself black while the palette is changing.
                 */
                bNoUgly = !bNoUgly;
                if (bNoUgly)
                    bUpdateColors = 0;
                break;

        case IDM_TRANSPARENT:
                /* Toggle DC mode */
                wTransparent = wTransparent == TRANSPARENT ?
                    OPAQUE : TRANSPARENT;
                break;

        case IDM_ANIMATE0:
                if (!hpalSave)
                    break;

                /* Reset animation count and stop timer */
                KillTimer(hWnd, 1);
                nAnimating = 0;

                /* Restore palette which existed before animation started */
                DeleteObject(hpalCurrent);
                hpalCurrent = hpalSave;

                /* Rebuild bitmap based on newly realized information */
                hDC = GetDC (hWnd);
                SelectPalette (hDC, hpalCurrent, 0);
                RealizePalette (hDC);
                ReleaseDC (hWnd, hDC);

                if (hbmCurrent){
                    DeleteObject (hbmCurrent);
                    hbmCurrent = NULL;

                    if (hdibCurrent)
                       hbmCurrent = BitmapFromDib (hdibCurrent, hpalCurrent);
                }
                hpalSave = NULL;

                /* Force redraw with new palette for everyone */
                InvalidateRect(hWnd, NULL, TRUE);
                break;

        case IDM_STEALCOL:
        case IDM_ANIMATE5:
        case IDM_ANIMATE20:
        case IDM_ANIMATE50:
        case IDM_ANIMATE100:
        case IDM_ANIMATE200:
        case IDM_ANIMATE201:
                /* Set animation count i.e number of times animation is to
                 * take place.
                 */
                nAnimating = id;
                if (id == IDM_STEALCOL)
                        nAnimating = 0;

                /* Save current palette */
                hpalSave = CopyPalette(hpalCurrent);

                GetObject(hpalCurrent, sizeof(int), (LPSTR)&pLogPal->palNumEntries);
                GetPaletteEntries(hpalCurrent, 0, pLogPal->palNumEntries, pLogPal->palPalEntry);

                /* Reserve all entries in the palette otherwise AnimatePalette()
                 * will not modify them
                 */
                for (i = 0; i < pLogPal->palNumEntries; i++) {
                     pLogPal->palPalEntry[i].peFlags = (BYTE)PC_RESERVED;
                }

                SetPaletteEntries(hpalCurrent, 0, pLogPal->palNumEntries, pLogPal->palPalEntry);

                /* Rebuild bitmap based on newly realized information */
                if (hbmCurrent){
                    DeleteObject (hbmCurrent);
                    hbmCurrent = NULL;

                    if (hdibCurrent)
                       hbmCurrent = BitmapFromDib (hdibCurrent, hpalCurrent);
                }

                /* Force redraw with new palette for everyone */
                InvalidateRect(hWnd, NULL, TRUE);

                /* Initiate the timer so that palette can be animated in
                 * response to a WM_TIMER message
                 */
		if ((UINT)nAnimating && !SetTimer(hWnd, (UINT)1,(UINT) 250, (TIMERPROC) NULL))
                        nAnimating = 0;

        default:
                break;
    }

    return TRUE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : InitDIB(hWnd)                                              *
 *                                                                          *
 *  PURPOSE    : Reads a DIB from a file, obtains a handle to it's          *
 *               BITMAPINFO struct., sets up the palette and loads the DIB. *
 *                                                                          *
 *  RETURNS    : TRUE  - DIB loads ok                                       *
 *               FALSE - otherwise                                          *
 *                                                                          *
 ****************************************************************************/
int InitDIB(hWnd)
HWND hWnd;
{
    unsigned           fh;
    LPBITMAPINFOHEADER lpbi;
    WORD FAR *         pw;
    int                i;
    BITMAPINFOHEADER   bi;
    OFSTRUCT           of;

    FreeDib();

    /* Open the file and get a handle to it's BITMAPINFO */

    fh = OpenFile (achFileName, (LPOFSTRUCT)&of, OF_READ);
    if (fh == -1) {
        ErrMsg("Can't open file '%ls'", (LPSTR)achFileName);
        return FALSE;
    }
    hbiCurrent = ReadDibBitmapInfo(fh);

    dwOffset = _llseek(fh, 0L, SEEK_CUR);
    _lclose (fh);

    if (hbiCurrent == NULL) {
        ErrMsg("%ls is not a Legitimate DIB File!", (LPSTR)achFileName);
        return FALSE;
    }
    DibInfo(hbiCurrent,&bi);

    /* Set up the palette */
    hpalCurrent = CreateDibPalette(hbiCurrent);
    if (hpalCurrent == NULL) {
        ErrMsg("CreatePalette() Failed");
        return FALSE;
    }

    /*  Convert the DIB color table to palette relative indexes, so
     *  SetDIBits() and SetDIBitsToDevice() can avoid color matching.
     *  We can do this because the palette we realize is identical
     *  to the color table of the bitmap, ie the indexes match 1 to 1
     *
     *  Now that the DIB color table is palette indexes not RGB values
     *  we must use DIB_PAL_COLORS as the wUsage parameter to SetDIBits()
     */
    lpbi = (VOID FAR *)GlobalLock(hbiCurrent);
    if (lpbi->biBitCount != 24) {
        fPalColors = TRUE;

        pw = (WORD FAR *)((LPSTR)lpbi + lpbi->biSize);

        for (i=0; i<(int)lpbi->biClrUsed; i++)
            *pw++ = (WORD)i;
    }
    GlobalUnlock(hbiCurrent);
    bLegitDraw = TRUE;

    /*  If the input bitmap is not in RGB FORMAT the banding code will
     *  not work!  we need to load the DIB bits into memory.
     *  if memory DIB, load it all NOW!  This will avoid calling the
     *  banding code.
     */
    if (bMemoryDIB || bi.biCompression != BI_RGB)
        hdibCurrent = OpenDIB(achFileName);

    /*  If the RLE could not be loaded all at once, exit gracefully NOW,
     *  to avoid calling the banding code
     */
    if ((bi.biCompression != BI_RGB) && !hdibCurrent){
        ErrMsg ("Could not load RLE!");
        FreeDib();
        return FALSE;
    }

    if (hdibCurrent && !bDIBToDevice && bMemoryDIB){
        hbmCurrent = BitmapFromDib(hdibCurrent,hpalCurrent);
        if (!hbmCurrent){
            ErrMsg ("Could not create bitmap!");
            FreeDib();
            return FALSE;
        }
    }

    SizeWindow(hWnd);

    return TRUE;
}
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : FreeDib(void)                                              *
 *                                                                          *
 *  PURPOSE    : Frees all currently active bitmap, DIB and palette objects *
 *               and initializes their handles.                             *
 *                                                                          *
 ****************************************************************************/
void FreeDib(void)
{
    if (hpalCurrent)
        DeleteObject(hpalCurrent);

    if (hbmCurrent)
        DeleteObject(hbmCurrent);

    if (hdibCurrent)
        GlobalFree(hdibCurrent);

    if (hbiCurrent && hbiCurrent != hdibCurrent)
        GlobalFree(hbiCurrent);

    fPalColors  = FALSE;
    bLegitDraw  = FALSE;
    hpalCurrent = NULL;
    hdibCurrent = NULL;
    hbmCurrent  = NULL;
    hbiCurrent  = NULL;
    SetRectEmpty (&rcClip);
}
