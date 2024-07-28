/*
 * DDEML CLOCK
 *
 * This application shows how to use DDEML to create a simple DDE server.
 *
 * This code uses the wrapper code to further simplify DDEML use for
 * simple applications.  See wrapper.h for details on its interface.
 */
 
#include <windows.h>
#include <ddeml.h>
#include "wrapper.h"
#include "ddemlclk.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOOL PASCAL PokeTime(HDDEDATA hData);
HDDEDATA PASCAL RequestTime(HDDEDATA hDataOut);
HDDEDATA PASCAL RequestHelp(HDDEDATA hDataOut);

DDEFORMATTBL ClockSystopicHelpFormats[] = {
    "TEXT", CF_TEXT, 0, NULL, RequestHelp
};

DDEITEMTBL ClockSystopicItems[] = {
    SZDDESYS_ITEM_HELP, 0, NULL, 1, 0, ClockSystopicHelpFormats
}; 

DDEFORMATTBL ClockTimeNowFormats[] = {
    { "TEXT",   CF_TEXT, 0, PokeTime, RequestTime },
    { "Dummy1", CF_TEXT, 0, PokeTime, RequestTime },
    { "Dummy2", CF_TEXT, 0, PokeTime, RequestTime },
};

DDEITEMTBL ClockTimeItems[] = {
    "Now", 0, NULL, 3, 0, ClockTimeNowFormats
};

DDETOPICTBL ClockTopics[] = {
    { "Time",         0, 1, 0, ClockTimeItems     },
    { SZDDESYS_TOPIC, 0, 1, 0, ClockSystopicItems },
};

DDESERVICETBL MyServiceInfo[] = {
    "Clock", 0, 2, 0, ClockTopics
};


DWORD idInst = 0;
/* Structure for holding time (in hours, minutes, and seconds) */
typedef struct
{
    int hour;
    int minute;
    int second;
} TIME;

extern void GetTime(TIME *); /* asm function to get current time */
extern void SetTime(TIME *); /* asm function to set current time */

TIME oTime;             /* the time currently displayed on the clock          */

HBRUSH hbrForegnd;      /* foreground brush -- system window text color       */
HBRUSH hbrBackgnd;      /* background brush -- system window backbround color */
HPEN   hpenForegnd;     /* foreground pen   -- system window text color       */
HPEN   hpenBackgnd;     /* background pen   -- system window background color */

HANDLE hInst;           /* instance of the CLOCK program being executed       */
BOOL   bFirst = TRUE;   /* TRUE if this is the 1st instance; FALSE otherwise  */

HANDLE    hCirTab;      /* Circle table for the circular clock face positions */
POINT FAR *lpCirTab;    /* Pointer to the circle table                        */

int   TimerID = 1;      /* number used for timer-id                           */
char  szBuffer[BUFLEN]; /* buffer for stringtable data                        */
RECT  ClockRect;        /* rectangle that EXACTLY bounds the clock face       */
long  ClockRadius;      /* clock face radius                                  */
POINT ClockCenter;      /* clock face center                                  */
BOOL  bIconic = FALSE;  /* TRUE if clock is currently iconic; FALSE otherwise */

int   HorzRes;          /* width of the display (in pixels)                   */
int   VertRes;          /* height of the display (in raster lines)            */
long  AspectH;          /* number of pixels per decimeter on the display      */
long  AspectV;          /* number of raster lines per decimeter on the display*/

/***************************************************************************
 *                                                                         *
 *  MACRO    : HourHandPos (TIME)                                          *
 *                                                                         *
 *  PURPOSE  : Computes the hour hand position based on both the hour and  *
 *             minute values in the given time record.                     *
 *                                                                         *
 ***************************************************************************/

#define HourHandPos(time)  (time.hour * 5) + (time.minute /12)


/***************************************************************************
 *                                                                         *
 *  MACRO    : VertEquiv (int)                                             *
 *                                                                         *
 *  PURPOSE  : Computes the raster line (vertical) equivalent to the given *
 *             pixel (horizontal) value.                                   *
 *                                                                         *
 ***************************************************************************/

#define VertEquiv(lengthH) ((lengthH * AspectV) / AspectH)


/***************************************************************************
 *                                                                         *
 *  MACRO    : HorzEquiv (int)                                             *
 *                                                                         *
 *  PURPOSE  : Computes the pixel (horizontal) equivalent to the given     *
 *             raster line (vertical) value.                               *
 *                                                                         *
 ***************************************************************************/

#define HorzEquiv(lengthV) ((lengthV * AspectH) / AspectV)


/***************************************************************************
 *                                                                         *
 *  FUNCTION : About (HWND, unsigned, WORD, LONG)                          *
 *                                                                         *
 *  PURPOSE  : Dialog function for the "About..." menu item dialog.        *
 *                                                                         *
 ***************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
    HWND     hDlg;
    unsigned message;
    WORD     wParam;
    LONG     lParam;
{
    switch (message)
    {
        case WM_COMMAND:
            EndDialog(hDlg, TRUE);
            /* fall through */

        case WM_INITDIALOG:
            return(TRUE);

        default:
            return(FALSE);
    }
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockWndProc (HWND, UINT, WPARAM, LPARAM)	       *
 *                                                                         *
 *  PURPOSE  : Window function for the application.                        *
 *                                                                         *
 ***************************************************************************/

long FAR PASCAL ClockWndProc(hWnd, message, wParam, lParam)
    HWND     hWnd;
    UINT     message;
    WPARAM   wParam;
    LPARAM     lParam;
{
    switch (message)
    {
        case WM_SYSCOMMAND:
            if (wParam == IDM_ABOUT)
            {
                /* Draw and handle messages for the "About..." Dialog */
                DialogBox(hInst,
                      MAKEINTRESOURCE(1),
                      hWnd,
                      MakeProcInstance((FARPROC) About, hInst));
            }
            else
            {
                /* Perform the default window processing */
                return(DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;

        case WM_SIZE:
            /* Resize clock based on window size and redraw */
            ClockSize(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
            UpdateWindow(hWnd);
            break;

        case WM_DESTROY:
            /* Destroy clock's timer and tools before exiting */
            KillTimer(hWnd, TimerID);
            DeleteTools();
            PostQuitMessage(0);
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;

                /* Paint clock displaying current time */
                InvalidateRect(hWnd, (LPRECT) NULL, TRUE);
                BeginPaint(hWnd, (LPPAINTSTRUCT) &ps);
                ClockPaint(hWnd, ps.hdc, PAINTALL);
                EndPaint(hWnd, (LPPAINTSTRUCT) &ps);
            }
            break;

        case WM_TIMECHANGE:
        case WM_TIMER:
            /* Update clock to display new time */
            ClockTimer(hWnd);
            break;

        case WM_SYSCOLORCHANGE:
            /* Change tools to coincide with system window colors */
            DeleteTools();
            CreateTools();
            break;

        case WM_ERASEBKGND:
            {
                RECT rc;

                /* Paint over the entire client area */
                GetClientRect(hWnd, (LPRECT) &rc);
                FillRect((HDC) wParam, (LPRECT) &rc, hbrBackgnd);
            }
            break;

        default:
            /* Perform the default window processing */
            return(DefWindowProc(hWnd, message, wParam, lParam));
    }
    return(0L);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : CreateTools ()                                              *
 *                                                                         *
 *  PURPOSE  : Creates brushes and pens to coincide with the current       *
 *             system colors.                                              *
 *                                                                         *
 ***************************************************************************/

VOID CreateTools()
{
    hbrForegnd  = CreateSolidBrush(GetSysColor(COLOR_WINDOWTEXT));
    hbrBackgnd  = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    hpenForegnd = CreatePen(0, 1, GetSysColor(COLOR_WINDOWTEXT));
    hpenBackgnd = CreatePen(0, 1, GetSysColor(COLOR_WINDOW));
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : DeleteTools ()                                              *
 *                                                                         *
 *  PURPOSE  : Destroys the brushes and pens created by CreateTools.       *
 *                                                                         *
 ***************************************************************************/

VOID DeleteTools()
{
    DeleteObject(hbrForegnd);
    DeleteObject(hbrBackgnd);
    DeleteObject(hpenForegnd);
    DeleteObject(hpenBackgnd);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockCreate ()                                              *
 *                                                                         *
 *  PURPOSE  : First, for drawing the clock, ClockCreate computes the      *
 *             aspect ratio and creates the necessary pens and brushes.    *
 *             Then, if this is the first instance of the app running,     *
 *             ClockCreate scales the circle table values according to the *
 *             aspect ratio. Finally, ClockCreate gets the initial time.   *
 *                                                                         *
 ***************************************************************************/

VOID ClockCreate()
{
    int  pos;      /* hand position index into the circle table */
    int  vertSize; /* height of the display in millimeters      */
    int  horzSize; /* width of the display in millimeters       */
    HDC  hDC;

    /* Get display size in (pixels X raster lines) */
    /* and in (millimeters X millimeters)          */
    hDC = GetDC(NULL);
    VertRes = GetDeviceCaps(hDC, VERTRES);
    HorzRes = GetDeviceCaps(hDC, HORZRES);
    vertSize= GetDeviceCaps(hDC, VERTSIZE);
    horzSize= GetDeviceCaps(hDC, HORZSIZE);
    ReleaseDC(NULL, hDC);

    /* Compute (raster lines / decimeter) and (pixels / decimeter) */
    AspectV = ((long) VertRes * MMPERDM) / (long) vertSize;
    AspectH = ((long) HorzRes * MMPERDM) / (long) horzSize;

    CreateTools();

    /* Scale cosines for aspect ratio if this is the first instance */
    if (bFirst)
    {
        lpCirTab = (POINT far *) GlobalLock(hCirTab);
        for (pos = 0; pos < HANDPOSITIONS; pos++)
        {
            lpCirTab[pos].y = (int)(VertEquiv(lpCirTab[pos].y));
        }
        GlobalUnlock(hCirTab);
        }

    GetTime(&oTime);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockSize (HWND, int, int, WORD)                            *
 *                                                                         *
 *  PURPOSE  : Resize the clock to the largest possible circle that will   *
 *             fit in the client area. If switching from not iconic to     *
 *             iconic, alter the timer to update every minute.  And if     *
 *             switching back to non iconic, restore the timer to update   *
 *             every second.                                               *
 *                                                                         *
 ***************************************************************************/
VOID ClockSize(
HWND hWnd,
int  newWidth,
int  newHeight,
WORD sizeType)
{
    /* Set ClockRect to bound the largest possible circle in the window */
    SetRect((LPRECT) &(ClockRect), 0, 0, newWidth, newHeight);
    CircleClock(newWidth, newHeight);

    if(sizeType == SIZEICONIC)
    {
        /* Update once every minute in the iconic state */
        KillTimer(hWnd, TimerID);
        SetTimer(hWnd, TimerID, (unsigned) ICON_TLEN, 0L);
        bIconic = TRUE;
    }
    else if (bIconic)
    {
        /* Update every second in the opened state (ignore tiling) */
        KillTimer(hWnd, TimerID);
        SetTimer(hWnd, TimerID, OPEN_TLEN, 0L);
        bIconic = FALSE;
    }
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockTimer (HWND)                                           *
 *                                                                         *
 *  PURPOSE  : Update the clock to reflect the most recent time.           *
 *                                                                         *
 ***************************************************************************/

VOID ClockTimer(
HWND hWnd)
{
    TIME nTime;
    HDC  hDC;

    GetTime(&nTime);

    /* It's possible to change any part of the system at any time through */
    /* the Control Panel. Check for any change in second, minute, or hour */
    if ((nTime.second != oTime.second) ||
        (nTime.minute != oTime.minute) ||
        (nTime.hour   != oTime.hour))
    {
        /* The time has changed -- update the clock */
        hDC = GetDC(hWnd);
        ClockPaint(hWnd, hDC, HANDPAINT);
        ReleaseDC(hWnd, hDC);
        DdePostAdvise(idInst, MyServiceInfo->topic[0].hszTopic,
                MyServiceInfo->topic[0].item[0].hszItem);
    }
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockPaint (HWND, HDC, int)                                 *
 *                                                                         *
 *  PURPOSE  : Paint the clock to display the most recent time.            *
 *                                                                         *
 ***************************************************************************/

VOID ClockPaint(
HWND hWnd,
HDC  hDC,
int  paintType)
{
    TIME nTime;

    SetBkMode(hDC, TRANSPARENT);

    lpCirTab = (POINT far *) GlobalLock(hCirTab);

    if (paintType == PAINTALL)
    {
        /* Paint entire clock -- face and hands */
        FillRect(hDC, (LPRECT) &ClockRect, hbrBackgnd);
        DrawFace(hDC);
        DrawFatHand(hDC, HourHandPos(oTime), hpenForegnd, HHAND);
        DrawFatHand(hDC, oTime.minute, hpenForegnd, MHAND);
        if (!bIconic)
        {
            /* Erase old second hand */
            DrawHand(hDC, oTime.second, hpenBackgnd, SECONDTIP, R2_NOT);
        }
        }
    else if (paintType == HANDPAINT)
    {
        GetTime(&nTime);

        if ((!bIconic) && (nTime.second != oTime.second))
        {
            /* Second has changed -- erase old second hand */
            DrawHand(hDC, oTime.second, hpenBackgnd, SECONDTIP, R2_NOT);
        }

        if ((nTime.minute != oTime.minute) || (nTime.hour != oTime.hour))
        {
            /* Hour and/or minute have changed -- update hands */
            if (bIconic)
            {
                /* Erase old minute and hour hands */
                DrawHand(hDC, oTime.minute,
                         hpenBackgnd, MINUTETIP, R2_COPYPEN);
                DrawHand(hDC, HourHandPos(oTime),
                         hpenBackgnd, HOURTIP, R2_COPYPEN);

                /* Draw new minute and hour hands */
                DrawHand(hDC, nTime.minute,
                         hpenForegnd, MINUTETIP, R2_COPYPEN);
                DrawHand(hDC, HourHandPos(nTime),
                         hpenForegnd, HOURTIP, R2_COPYPEN);
                    
            }
            else
            {
                /* Erase old minute and hour fat hands */
                DrawFatHand(hDC, oTime.minute,
                        hpenBackgnd, MHAND);
                DrawFatHand(hDC, HourHandPos(oTime),
                        hpenBackgnd, HHAND);

                /* Draw new minute and hour fat hands */
                DrawFatHand(hDC, nTime.minute,
                        hpenForegnd, MHAND);
                DrawFatHand(hDC, HourHandPos(nTime),
                            hpenForegnd, HHAND);
            }
        }

        if ((!bIconic) && (nTime.second != oTime.second))
        {
            /* second has changed -- draw new second hand */
            DrawHand(hDC, nTime.second,
                     hpenBackgnd, SECONDTIP, R2_NOT);
        }

        /* Store most recent time */
        oTime.minute = nTime.minute;
        oTime.hour   = nTime.hour;
        oTime.second = nTime.second;
    }
    GlobalUnlock(hCirTab);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : DrawFace (HDC)                                              *
 *                                                                         *
 *  PURPOSE  : Draws the clock face.                                       *
 *                                                                         *
 ***************************************************************************/

VOID DrawFace(
HDC hDC)
{
    int    pos;       /* hand position index into the circle table */
    int    dotHeight; /* height of the hour-marking dot            */
    int    dotWidth;  /* width of the hour-marking dot             */
    POINT  dotCenter; /* center point of the hour-marking dot      */
    RECT   rc;

    /* Compute hour-marking dot width, height, and center point */
    dotWidth = (int)((MAXDOTWIDTH * (long) (ClockRect.right - ClockRect.left)) / HorzRes);
    dotHeight = (int)(VertEquiv(dotWidth));

    if (dotHeight < MINDOTHEIGHT)
    {
        dotHeight = MINDOTHEIGHT;
    }

    if (dotWidth < MINDOTWIDTH)
    {
        dotWidth = MINDOTWIDTH;
    }

    dotCenter.x = dotWidth >> 1;
    dotCenter.y = dotHeight >> 1;

    /* Compute the clock center and radius */
    InflateRect((LPRECT) &ClockRect, -dotCenter.y, -dotCenter.x);
    ClockRadius = (long) ((ClockRect.right - ClockRect.left) >> 1);
    ClockCenter.x = (int)(ClockRect.left + ClockRadius);
    ClockCenter.y = ClockRect.top + ((ClockRect.bottom - ClockRect.top) >> 1);
    InflateRect((LPRECT) &ClockRect, dotCenter.y, dotCenter.x);

    /* Draw the large hour-marking dots and small minute-marking dots */
    for(pos = 0; pos < HANDPOSITIONS; pos++)
    {
        rc.top = (int)((lpCirTab[pos].y * ClockRadius) / CIRTABSCALE + ClockCenter.y);
        rc.left = (int)((lpCirTab[pos].x * ClockRadius) / CIRTABSCALE + ClockCenter.x);
        if (pos % 5)
        {
            if ((dotWidth > MINDOTWIDTH) && (dotHeight > MINDOTHEIGHT))
            {
                /* Draw small minute-marking dot */
                rc.right = rc.left + 1;
                rc.bottom = rc.top + 1;
                FillRect(hDC, (LPRECT) &rc, hbrForegnd);
            }
        }
        else
        {
            /* Draw large hour-marking dot */
            rc.right = rc.left + dotWidth;
            rc.bottom = rc.top + dotHeight;
            OffsetRect((LPRECT) &rc, -dotCenter.x, -dotCenter.y);
            FillRect(hDC, (LPRECT) &rc, hbrForegnd);
        }
    }
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : DrawHand (HDC, int, HPEN, int, int)                         *
 *                                                                         *
 *  PURPOSE  : Draws a thin hand with the specified pen in the specified   *
 *             hand position.                                              *
 *                                                                         *
 ***************************************************************************/

VOID DrawHand(
HDC  hDC,
int  pos,
HPEN hPen,
int  scale,
int  patMode)
{
    long radius;

    /* scale length of hand */
    radius = (ClockRadius * scale) / 100;

    /* set pattern mode for hand */
    SetROP2(hDC, patMode);

    /* select pen for hand */
    SelectObject(hDC, hPen);

    /* Draw thin hand */
    MoveTo(hDC, ClockCenter.x, ClockCenter.y);
    LineTo(hDC, ClockCenter.x + (int)((lpCirTab[pos].x * radius) / CIRTABSCALE),
            ClockCenter.y + (int)((lpCirTab[pos].y * radius) / CIRTABSCALE));
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : DrawFatHand (HDC, int, HPEN, BOOL)                          *
 *                                                                         *
 *  PURPOSE  : Draws a fat hand with the specified pen in the specified    *
 *             hand position.                                              *
 *                                                                         *
 ***************************************************************************/

VOID DrawFatHand(
HDC  hDC,
int  pos,
HPEN hPen,
BOOL hHand)
{
    POINT ptTip;  /* coordinates for the tip of the hand        */
    POINT ptTail; /* coordinates for the tail of the hand       */
    POINT ptSide; /* coordinates for the side of the hand       */
    int   index;  /* position index into the circle table       */
    long  scale;  /* ClockRadius percentage to scale drawing to */

    /* set pattern mode for hand */
    SetROP2(hDC, 13);

    /* select pen for hand */
    SelectObject(hDC, hPen);

    /* compute coordinates for the side of the hand */
    scale = (ClockRadius * (hHand ? HOURSIDE : MINUTESIDE)) / 100;
    index = (pos + SIDESHIFT) % HANDPOSITIONS;
    ptSide.y = (int)((lpCirTab[index].y * scale) / CIRTABSCALE);
    ptSide.x = (int)((lpCirTab[index].x * scale) / CIRTABSCALE);

    /* compute coordinates for the tip of the hand */
    scale = (ClockRadius * (hHand ? HOURTIP : MINUTETIP)) / 100;
    ptTip.y = (int)((lpCirTab[pos].y * scale) / CIRTABSCALE);
    ptTip.x = (int)((lpCirTab[pos].x * scale) / CIRTABSCALE);

    /* compute coordinates for the tail of the hand */
    scale = (ClockRadius * (hHand ? HOURTAIL : MINUTETAIL)) / 100;
    index = (pos + TAILSHIFT) % HANDPOSITIONS;
    ptTail.y = (int)((lpCirTab[index].y * scale) / CIRTABSCALE);
    ptTail.x = (int)((lpCirTab[index].x * scale) / CIRTABSCALE);

    /* Draw tip of hand */
    MoveTo(hDC, ClockCenter.x + ptSide.x, ClockCenter.y + ptSide.y);
    LineTo(hDC, ClockCenter.x +  ptTip.x, ClockCenter.y +  ptTip.y);
    MoveTo(hDC, ClockCenter.x - ptSide.x, ClockCenter.y - ptSide.y);
    LineTo(hDC, ClockCenter.x +  ptTip.x, ClockCenter.y +  ptTip.y);

    /* Draw tail of hand */
    MoveTo(hDC, ClockCenter.x + ptSide.x, ClockCenter.y + ptSide.y);
    LineTo(hDC, ClockCenter.x + ptTail.x, ClockCenter.y + ptTail.y);
    MoveTo(hDC, ClockCenter.x - ptSide.x, ClockCenter.y - ptSide.y);
    LineTo(hDC, ClockCenter.x + ptTail.x, ClockCenter.y + ptTail.y);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : CircleClock (int, int)                                      *
 *                                                                         *
 *  PURPOSE  : Resizes the clock rectangle to keep the face circular.      *
 *                                                                         *
 ***************************************************************************/

VOID CircleClock(
int maxWidth,
int maxHeight)
{
    int clockHeight; /* tallest height that will keep face circular */
    int clockWidth;  /* widest width that will keep face circular   */

    if (maxWidth > (int)HorzEquiv(maxHeight))
    {
        /* too wide -- decrease width to keep face circular */
        clockWidth = (int)HorzEquiv(maxHeight);
        ClockRect.left += (maxWidth - clockWidth) >> 1;
        ClockRect.right = ClockRect.left + clockWidth;
    }
    else
    {
        /* too tall -- decrease height to keep face circular */
        clockHeight = (int)VertEquiv(maxWidth);
        ClockRect.top += (maxHeight - clockHeight) >> 1;
        ClockRect.bottom = ClockRect.top + clockHeight;
    }
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : WinMain (HANDLE, HANDLE, LPSTR, int)                        *
 *                                                                         *
 *  PURPOSE  : Calls the initialization function, creates the main appli-  *
 *             cation window, and enters the message loop.                 *
 *                                                                         *
 ***************************************************************************/

int PASCAL WinMain(
HANDLE hInstance,
HANDLE hPrev,
LPSTR  lpszCmdLine,
int    cmdShow)
{
    HWND  hWnd;
    MSG   msg;
    HMENU hMenu;
    TIME  nTime;
    int   sysWidth;  /* width of left and right frames                  */
    int   sysHeight; /* height of caption bar and top and bottom frames */
    int   width;     /* width of entire clock window                    */
    int   height;    /* height of entire clock window                   */

    hInst = hInstance;

    LoadString(hInst, IDS_APPNAME, (LPSTR) szBuffer, BUFLEN);

    if (!hPrev)
    {
        /* First instance -- register window class */
        if (!ClockInit())
            return(FALSE);
        }
    else
    {
        /* Not first instance -- get circle table and reset bFirst flag */
        GetInstanceData(hPrev, (PSTR) &hCirTab, sizeof(HANDLE));
        bFirst = FALSE;
        }

    ClockCreate();
    /* compute window height and width */
    sysWidth  = GetSystemMetrics(SM_CXFRAME) * 2;
    sysHeight = GetSystemMetrics(SM_CYCAPTION) + (GetSystemMetrics(SM_CYFRAME) * 2);
    width = (HorzRes / 3) + sysWidth;
    height = (int)(VertEquiv(width) + sysHeight);

    hWnd = CreateWindow( (LPSTR) szBuffer, /* class name              */
                             (LPSTR) szBuffer, /* The window name         */
                             WS_TILEDWINDOW,   /* window style            */
                             CW_USEDEFAULT,    /* use default positioning */
                             0,                /* y not used              */
                             width,            /* window width            */
                 height,           /* window height           */
                             NULL,             /* NULL parent handle      */
                             NULL,             /* NULL menu/child handle  */
                             hInst,            /* program instance        */
                             NULL              /* NULL data structure ref.*/
               );

    GetTime(&nTime);
    GetTime(&oTime);
    while ((nTime.second == oTime.second) &&
               (nTime.minute == oTime.minute) &&
               (nTime.hour   == oTime.hour)     )
    {
        GetTime(&oTime);
    }

    if (!SetTimer(hWnd, TimerID, OPEN_TLEN, 0L))
    {
        LPSTR szTooMany;

        /* 16 public timers already in use -- post error and exit */
        szTooMany = (LPSTR)(unsigned long) LocalAlloc(LPTR, 40);
        LoadString(hInst, IDS_TOOMANY, szTooMany, 40);
        MessageBox((HWND) NULL, szTooMany, (LPSTR) szBuffer,
               MB_OK | MB_ICONHAND | MB_SYSTEMMODAL);
        DeleteTools();
        return(FALSE);
    }

    /* Add the "About..." menu item to the bottom of the system menu */
    LoadString(hInst, IDS_ABOUTMENU, (LPSTR) szBuffer, BUFLEN);
    hMenu = GetSystemMenu(hWnd, FALSE);
    ChangeMenu(hMenu, 0, (LPSTR) szBuffer, IDM_ABOUT, MF_APPEND | MF_STRING);

    ShowWindow(hWnd, cmdShow);

    InitializeDDE(NULL, &idInst, MyServiceInfo,
            CBF_FAIL_EXECUTES | CBF_SKIP_ALLNOTIFICATIONS, hInstance);

    /* Process messages until program termination */
    while (GetMessage((LPMSG) &msg, NULL, 0, 0))
    {
        TranslateMessage((LPMSG) &msg);
        DispatchMessage((LPMSG) &msg);
    }

    UninitializeDDE();
    
    return(msg.wParam);
}


/***************************************************************************
 *                                                                         *
 *  FUNCTION : ClockInit ()                                                *
 *                                                                         *
 *  PURPOSE  : Registers the applicatoin windwo class and initializes the  *
 *             circle values for the clock face.                           *
 *                                                                         *
 ***************************************************************************/

BOOL ClockInit()
{
    PWNDCLASS pClockClass;
    HANDLE    hRes;
    char      szData[5];

    pClockClass = (PWNDCLASS) LocalAlloc(LPTR, sizeof(WNDCLASS));

    pClockClass->lpszClassName = (LPSTR) szBuffer;
    pClockClass->hbrBackground = (HBRUSH) NULL;
    pClockClass->style         = CS_VREDRAW | CS_HREDRAW | CS_BYTEALIGNCLIENT;
    pClockClass->hInstance     = hInst;
    pClockClass->lpfnWndProc   = ClockWndProc;
    pClockClass->hCursor       = LoadCursor(NULL, IDC_ARROW);
    pClockClass->hIcon         = NULL;

    if (!RegisterClass((LPWNDCLASS) pClockClass))
    {
        /* Error registering class -- return */
        return(FALSE);
    }

    LocalFree((HANDLE) pClockClass);

    /* Load in pre-computed circle table cosine values from resource file */
    LoadString(hInst, IDS_DATA, (LPSTR) szData, 5);
    hRes = FindResource(hInst, (LPSTR) szBuffer, (LPSTR) szData);
    if (!hRes)
    {
        /* Could not find circle table resource data -- return */
        return(FALSE);
    }

    hCirTab = LoadResource(hInst, hRes);
    LockResource(hCirTab);

    return(TRUE);
}

                 
BOOL PASCAL PokeTime(
HDDEDATA hData)
{
    TIME nTime;
    char sz[40];
    
    DdeGetData(hData, (LPBYTE)sz, 40L, 0L);
    sscanf(sz, "%d:%d:%d", &nTime.hour, &nTime.minute, &nTime.second);
    SetTime(&nTime);
    return(TRUE);
}


          
HDDEDATA PASCAL RequestTime(
HDDEDATA hDataOut)
{
    char sz[40];
    
    itoa(oTime.hour, sz, 10);
    strcat(sz, ":");
    itoa(oTime.minute, &sz[strlen(sz)], 10);
    strcat(sz, ":");
    itoa(oTime.second, &sz[strlen(sz)], 10);
    return(DdeAddData(hDataOut, (LPBYTE)sz, strlen(sz) + 1, 0L));
}

HDDEDATA PASCAL RequestHelp(
HDDEDATA hDataOut)
{
    static char szHelp[] = "DDE Help for the Clock Service.\r\r\n\t"
        "Poke to 'Time!Now' to reset the clock.\r\n\t"
        "Request or advise on 'Time!Now' to get the time.";

    return(DdeAddData(hDataOut, szHelp, sizeof(szHelp), 0));
}
 
