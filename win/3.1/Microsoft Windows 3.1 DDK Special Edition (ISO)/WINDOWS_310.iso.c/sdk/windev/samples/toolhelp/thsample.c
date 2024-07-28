/**************************************************************************
 *  THSAMPLE.C
 *
 *      TOOLHELP.DLL Sample program
 *
 **************************************************************************/

#include <stdio.h>
#undef NULL
#include <windows.h>
#include <string.h>
#include <toolhelp.h>
#include "thsample.h"

/* ----- Symbols ----- */
#define LIST_WIDTH      70
#define LIST_NONE       0
#define LIST_GLOBAL     1
#define LIST_MODULE     2
#define LIST_TASK       3
#define LIST_HEAP       4
#define LIST_TOOLHELP   5
#define LIST_MEMMAN     6
#define LIST_CLASS      7
#define BUTTON_WIDTH    (10 * xChar)
#define BUTTON_MARGIN   xChar
#define IDM_WRITE       100
#define IDM_WRITEBP     101
#ifndef SHORT
typedef short int SHORT;
#endif

/* ----- Memory window extra values ----- */
#define MEM_BLOCK       0
#define MEM_GLOBALENTRY 2
#define MEM_LPOURBLOCK  4
#define MEM_HOURBLOCK   8

/* ----- Macros ----- */
#define MAXLINES(dwSize,yClient) \
    max((SHORT)(dwSize / 16) - ((SHORT)yClient) / yChar, 0)

/* ----- Function prototypes ----- */

    LONG FAR PASCAL WndProc(
        HWND hWnd,
	UINT wMessage,
	WPARAM wParam,
	LPARAM lParam);

    LONG FAR PASCAL MemWndProc(
        HWND hWnd,
	UINT wMessage,
	WPARAM wParam,
	LPARAM lParam);

    void PASCAL DumpMem(
        HWND hwnd,
        HDC hDC,
        SHORT nScrollPos,
        SHORT nPos);

    LONG WalkGlobalHeap(
        HWND hwnd);

    LONG WalkFreeList(
        HWND hwnd);

    LONG WalkLRUList(
        HWND hwnd);

    LONG WalkLocalHeap(
        HWND hwnd,
        HANDLE hBlock);

    LONG WalkModuleList(
        HWND hwnd);

    LONG WalkTaskList(
        HWND hwnd);

    LONG WalkClassList(
        HWND hwnd);

    LONG DoStackTrace(
        HWND hwnd);

    LONG DoHeapInfo(
        HWND hwnd);

    LONG DoMemManInfo(
        HWND hwnd);

    LONG DoGlobalEntryModuleTest(
        HWND hwnd);

    LONG ReadMemoryTest(
        HWND hwnd,
        HANDLE hBlock);

    LONG TimerCountTest(
        HWND hwnd);

    BOOL FAR PASCAL MyNotifyHandler(
        WORD wID,
        DWORD dwData);

    WORD _cdecl MyCFaultHandler(
        WORD wES,
        WORD wDS,
        WORD wDI,
        WORD wSI,
        WORD wBP,
        WORD wSP,
        WORD wBX,
        WORD wDX,
        WORD wCX,
        WORD wOldAX,
        WORD wOldBP,
        WORD wRetIP,
        WORD wRetCS,
        WORD wRealAX,
        WORD wNumber,
        WORD wHandle,
        WORD wIP,
        WORD wCS,
        WORD wFlags);

    BOOL FAR PASCAL FaultDialogProc(
        HWND hDlg,
        WORD wMessage,
        WORD wParam,
        DWORD dwParam);

/* ----- Global variables ----- */
    short xScreen;
    short yScreen;
    short xChar;
    short yChar;
    HANDLE hInst;
    HWND hwndList;
    HWND hwndListLocal;
    HWND hwndListStatic;
    WORD wListStatus = LIST_NONE;
    char szText[80];
    HWND hwndMain;
    WORD wNotifyIn;
    WORD wNotifyOut;
    WORD wNotifyState;
    WORD wFilterState;
    WORD wsCS;
    WORD wsIP;
    WORD wsFault;
    WORD wsFaultTask;
    WORD wsProgramTask;
    char szAppName[] = "THTest";
    char szMemName[] = "THMemPopup";
    extern BYTE _AHINCR;

    char *szBlockTypes[] =
    {
        "Private", "DGroup", "Data", "Code", "Task", "Resource",
        "Module", "Free", "Internal", "Sentinel", "BMaster"
    };
    char bHasData[] =
    { 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 };
    char *szLocalFlags[] =
    { "", "Fixed", "Free", "", "Moveable" };
    char *szGDI[] =
    {
        "Unknown", "Pen", "Brush", "Font", "Palette", "Bitmap", "Region",
        "DC", "DisabledDC", "MetaDC", "Metafile", ""
    };
    char *szUser[] =
    {
        "Unknown", "Class", "Window", "String", "Menu", "Clip", "CBox",
        "Palette", "Ed", "Bwl", "OwnerDraw", "SPB", "CheckPoint", "DCE",
        "MWP", "PROP", "LBIV", "Misc", "Atoms", "LockInp", "HookList",
        "UserSeeUserDo", "HotKeyList", "PopupMenu", "HandleTab", ""
    };
    char *szNotify[] =
    {
        "Unknown", "LoadSeg", "FreeSeg", "StartDLL", "StartTask",
        "ExitTask", "DelModule", "RIP", "TaskIn", "TaskOut", "InChar",
        "OutStr", "LogErr", "LogPError"
    };


/*  WinMain
 */

int PASCAL WinMain(
    HANDLE hInstance,
    HANDLE hPrevInstance,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    FARPROC lpfnNotify;
    FARPROC lpfnFault;

    /* Register the interrupt handler */
    wsProgramTask = GetCurrentTask();
    (FARPROC)lpfnFault = MakeProcInstance((FARPROC)MyFaultHandler, hInstance);
    if (!InterruptRegister(NULL, lpfnFault))
    {
        OutputDebugString("THTest: Interrupt hook failed!!\r\n");
        return 1;
    }

    /* Register the notification handler */
    (FARPROC)lpfnNotify = MakeProcInstance(MyNotifyHandler, hInstance);
    if (!NotifyRegister(
	NULL, (LPFNNOTIFYCALLBACK)lpfnNotify, (WORD)NF_TASKSWITCH | NF_RIP))
    {
        OutputDebugString("THTest: Notification hook failed!!\r\n");
        return 1;
    }

    xScreen = GetSystemMetrics(SM_CXSCREEN);
    yScreen = GetSystemMetrics(SM_CYSCREEN);
    hInst = hInstance;

    if (!hPrevInstance)
    {
        wndclass.style = 0L;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wndclass.lpszMenuName = MAKEINTRESOURCE(ID_MENU);
        wndclass.lpszClassName = szAppName;

        if (!RegisterClass(&wndclass))
            return FALSE;

        wndclass.style = 0L;
        wndclass.lpfnWndProc = MemWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 10;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = szMemName;

        if (!RegisterClass(&wndclass))
            return FALSE;
    }

    hwnd = CreateWindow(
        szAppName,              /* Window class name */
        "TOOLHELP.DLL Test",    /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        0,                      /* Initial Y position */
        CW_USEDEFAULT,          /* Initial X size */
        0,                      /* Initial Y size */
        NULL,                   /* Parent window handle */
        NULL,                   /* Window menu handle */
        hInstance,              /* Program instance handle */
        NULL);                  /* Create parameters */

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    hwndMain = hwnd;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* Get rid of our Fault handler and our notification handler */
    InterruptUnRegister(NULL);
    FreeProcInstance(lpfnFault);
    NotifyUnRegister(NULL);
    FreeProcInstance(lpfnNotify);

    return msg.wParam;
}


/*  WndProc
 *      Main controlling window for sample program
 */

LONG FAR PASCAL WndProc(
    HWND hwnd,
    UINT wMessage,
    WPARAM wParam,
    LPARAM lParam)
{
    HDC hDC;
    TEXTMETRIC tm;
    HANDLE hTask;
    static HFONT hFont;

    switch (wMessage)
    {
    case WM_CREATE:

        /* Get static constants */
        hDC = GetDC(hwnd);
        GetTextMetrics(hDC, &tm);
        xChar = tm.tmAveCharWidth;
        yChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hDC);

        /* Create child controls */
        hwndListStatic = CreateWindow(
            "static",
            NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            xChar, 4 * yChar, 0, 0,
            hwnd, 1, hInst, NULL);
        hwndList = CreateWindow(
            "listbox",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_USETABSTOPS |
            LBS_NOTIFY,
            xChar, 5 * yChar + 2, 0, 0,
            hwnd, 2, hInst, NULL);
        hwndListLocal = CreateWindow(
            "listbox",
            NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_USETABSTOPS |
            LBS_NOTIFY,
            xChar, 0, 0, 0,
            hwnd, 3, hInst, NULL);

        /* Put a small non-proportional font in the box */
   		hFont = GetStockObject(SYSTEM_FIXED_FONT);
        SendMessage(hwndList, WM_SETFONT, hFont, NULL);
        SendMessage(hwndListLocal, WM_SETFONT, hFont, NULL);
        break;

    case WM_SIZE:
	if ((short)HIWORD(lParam) < 8 * yChar || (short)LOWORD(lParam) < 10 * xChar)
        {
            ShowWindow(hwndList, SW_HIDE);
            ShowWindow(hwndListLocal, SW_HIDE);
            ShowWindow(hwndListStatic, SW_HIDE);
        }
        else
        {
            short nHeight;
            short nWidth;

            nHeight = (HIWORD(lParam) - 2 * yChar) / 2 - yChar / 2;
            nWidth = LOWORD(lParam) - 2 * xChar;
            ShowWindow(hwndList, SW_SHOWNORMAL);
            ShowWindow(hwndListStatic, SW_SHOWNORMAL);
            ShowWindow(hwndListLocal, SW_SHOWNORMAL);
            MoveWindow(hwndList, xChar, yChar, nWidth, nHeight, TRUE);
            MoveWindow(hwndListLocal, xChar, yChar + nHeight + yChar,
                nWidth, nHeight, TRUE);
            MoveWindow(hwndListStatic, 2 * xChar, 0, nWidth - 2 * xChar,
                yChar, TRUE);
            UpdateWindow(hwndList);
            UpdateWindow(hwndListLocal);
        }
        return 0L;

    case WM_USER:
        /* Only handle the notifications we know about */
        if (wParam > 15)
            wParam = 0;

        /* Make a nice message to put in the box */
        wsprintf(szText, "%-12s\t| %08lX",
            (LPSTR)szNotify[wParam], lParam);
        SendMessage(hwndListLocal, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
        return 0L;

    case WM_COMMAND:
        switch (wParam)
        {
        case 2:     /* List box notification message */
            if (HIWORD(lParam) != LBN_DBLCLK)
                return 0L;

            if (wListStatus == LIST_GLOBAL)
            {
                HANDLE hBlock;
                GLOBALENTRY Global;

                /* Get the global handle from the list box */
                SendMessage(hwndList, LB_GETTEXT,
                    (WORD)SendMessage(hwndList, LB_GETCURSEL, 0, 0L),
                    (LONG)(LPSTR)szText);
                sscanf(szText, "%*2c%*lx %*2c%x", &hBlock);

                /* See if this block has a local heap, if not, call the
                 *  memory dump routine to test the memory functions.
                 */
                if (hBlock)
                {
                    Global.dwSize = sizeof (GLOBALENTRY);
                    GlobalEntryHandle(&Global, hBlock);
                }
                if (!hBlock)
                    return 0L;
                else if (Global.wHeapPresent)
                    return WalkLocalHeap(hwnd, hBlock);
                else
                    return ReadMemoryTest(hwnd, hBlock);
            }
            else if (wListStatus == LIST_TASK)
                return DoStackTrace(hwnd);
            else
                return 0L;

        case IDM_TEST_1:
            return WalkGlobalHeap(hwnd);

        case IDM_TEST_2:
            return WalkFreeList(hwnd);

        case IDM_TEST_3:
            return WalkLRUList(hwnd);

        case IDM_TEST_4:
            return WalkModuleList(hwnd);

        case IDM_TEST_5:
            return WalkTaskList(hwnd);

        case IDM_TEST_10:
            return WalkClassList(hwnd);

        case IDM_TEST_6:
            return DoHeapInfo(hwnd);

        case IDM_TEST_8:
            return DoGlobalEntryModuleTest(hwnd);

        case IDM_TEST_9:
            return DoMemManInfo(hwnd);

        case IDM_TEST_11:
            if (wListStatus != LIST_TASK)
                return 0L;

            /* Get the task handle from the list box */
            SendMessage(hwndList, LB_GETTEXT,
                (WORD)SendMessage(hwndList, LB_GETCURSEL, 0, 0L),
                (LONG)(LPSTR)szText);
            sscanf(szText, "%*6c%x", &hTask);

            /* Nuke the task if it is not the current one */
            if (hTask != wsProgramTask)
                TerminateApp(hTask, NO_UAE_BOX);

            /* Update the task list and get out */
            return WalkTaskList(hwnd);

        case IDM_TEST_12:
            return TimerCountTest(hwnd);

        case IDM_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
            break;

        case IDM_FAULT_1:
        case IDM_FAULT_2:
        case IDM_FAULT_3:
        case IDM_FAULT_4:
        case IDM_FAULT_5:
        case IDM_FAULT_6:
        case IDM_FAULT_7:
        case IDM_FAULT_8:
            Fault(wParam);
            return 0L;

        case IDM_NOTIFY_ENABLE:
            if (GetMenuState(GetMenu(hwnd), IDM_NOTIFY_ENABLE, 0) & MF_CHECKED)
            {
                wNotifyState = FALSE;
                CheckMenuItem(GetMenu(hwnd), IDM_NOTIFY_ENABLE, MF_UNCHECKED);
            }
            else
            {
                wNotifyState = TRUE;
                CheckMenuItem(GetMenu(hwnd), IDM_NOTIFY_ENABLE, MF_CHECKED);
            }
            return 0L;

        case IDM_FILTER_ENABLE:
            if (GetMenuState(GetMenu(hwnd), IDM_FILTER_ENABLE, 0) & MF_CHECKED)
            {
                wFilterState = FALSE;
                CheckMenuItem(GetMenu(hwnd), IDM_FILTER_ENABLE, MF_UNCHECKED);
            }
            else
            {
                wFilterState = TRUE;
                CheckMenuItem(GetMenu(hwnd), IDM_FILTER_ENABLE, MF_CHECKED);
            }
            return 0L;

        case IDM_NOTIFY_CLEAR:
            SendMessage(hwndListLocal, WM_SETREDRAW, FALSE, 0L);
            SendMessage(hwndListLocal, LB_RESETCONTENT, 0, 0L);
            SendMessage(hwndListLocal, WM_SETREDRAW, TRUE, 0L);
            InvalidateRect(hwndListLocal, NULL, TRUE);
            return 0L;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, wMessage, wParam, lParam);
    }

    return 0L;
}

/*  MemWndProc
 *      Window proc for memory browser.
 */

LONG FAR PASCAL MemWndProc(
    HWND hwnd,
    UINT wMessage,
    WPARAM wParam,
    LPARAM lParam)
{
    GLOBALENTRY *pGlobal;
    WORD w;
    LPSTR lpBlock;
    RECT rect;
    HMENU hMenu;
    DWORD dwReturn;

    switch (wMessage)
    {
    case WM_CREATE:
        /* Save the block pointer as window WORD 0 */
        SetWindowWord(hwnd, MEM_BLOCK,
            LOWORD((LONG)((LPCREATESTRUCT)lParam)->lpCreateParams));

        /* Get information about this block */
        pGlobal = (GLOBALENTRY *)LocalAlloc(LMEM_FIXED, sizeof (GLOBALENTRY));
        if (!pGlobal)
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
        }
        pGlobal->dwSize = sizeof (GLOBALENTRY);
        if (!GlobalEntryHandle(pGlobal, GetWindowWord(hwnd, MEM_BLOCK)))
        {
            MessageBox(hwnd, "Block Handle is invalid", "Browser", IDOK);
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
        }

        /* Save GLOBALENTRY pointer as window WORD 2 */
        SetWindowWord(hwnd, MEM_GLOBALENTRY, (WORD)pGlobal);

        /* Now read the memory into our global block */
        w = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, pGlobal->dwBlockSize);
        if (!w)
        {
            MessageBox(hwnd, "Not enough memory to copy block",
                "Browser", IDOK);
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
            return 0L;
        }
        lpBlock = GlobalLock(w);
        SetWindowWord(hwnd, MEM_HOURBLOCK, w);
        SetWindowLong(hwnd, MEM_LPOURBLOCK, (LONG)lpBlock);
        MemoryRead(GetWindowWord(hwnd, MEM_BLOCK), 0L, lpBlock,
            pGlobal->dwBlockSize);

        /* Put a new item on the system menu to rewrite the block */
        hMenu = GetSystemMenu(hwnd, 0);
        AppendMenu(hMenu, MF_STRING, IDM_WRITE, "&Write");
        AppendMenu(hMenu, MF_STRING, IDM_WRITEBP, "Write &BP");

        break;

    case WM_SIZE:
    {
        SHORT nScrollMax;

        /* Compute the scroll bar maximum and position */
        pGlobal = (GLOBALENTRY *)GetWindowWord(hwnd, MEM_GLOBALENTRY);
        nScrollMax = MAXLINES(pGlobal->dwBlockSize, HIWORD(lParam));
        SetScrollRange(hwnd, SB_VERT, 0, nScrollMax, FALSE);
        SetScrollPos(hwnd, SB_VERT, min(GetScrollPos(hwnd, SB_VERT),
            nScrollMax), FALSE);

        /* Force the whole thing to repaint */
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }

    case WM_VSCROLL:
    {
        SHORT nScrollInc;
        SHORT nScrollPos;
        SHORT nScrollMin;
        SHORT nScrollMax;

        /* Get the current position */
        nScrollPos = GetScrollPos(hwnd, SB_VERT);
        GetScrollRange(hwnd, SB_VERT, &nScrollMin, &nScrollMax);
        GetClientRect(hwnd, &rect);

        /* Decode the various forms of scrolling */
        switch (wParam)
        {
        case SB_TOP:
            nScrollInc = -nScrollPos;
            break;

        case SB_BOTTOM:
            nScrollInc = nScrollMax - nScrollPos;
            break;

        case SB_LINEUP:
            nScrollInc = -1;
            break;

        case SB_LINEDOWN:
            nScrollInc = 1;
            break;

        case SB_PAGEUP:
            nScrollInc = min(-1, -rect.bottom / yChar);
            break;

        case SB_PAGEDOWN:
            nScrollInc = max(1, rect.bottom / yChar);
            break;

        case SB_THUMBTRACK:
            nScrollInc = LOWORD(lParam) - nScrollPos;
            break;

        default:
            nScrollInc = 0;
            break;
        }
        
        /* Now do the scroll */
        if (nScrollInc = max(-nScrollPos,
            min(nScrollInc, nScrollMax - nScrollPos)))
        {
            ScrollWindow(hwnd, 0, -yChar * nScrollInc, NULL, NULL);
            SetScrollPos(hwnd, SB_VERT, nScrollPos + nScrollInc, TRUE);
            UpdateWindow(hwnd);
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        SHORT nScrollMin;
        SHORT nScrollMax;
        SHORT nScrollPos;
        SHORT nStart;
        SHORT nEnd;
        HFONT hFont;
        HFONT hOldFont;

        BeginPaint(hwnd, &ps);

        /* Compute the number of lines to paint */
        pGlobal = (GLOBALENTRY *)GetWindowWord(hwnd, MEM_GLOBALENTRY);
        nScrollPos = GetScrollPos(hwnd, SB_VERT);
        GetScrollRange(hwnd, SB_VERT, &nScrollMin, &nScrollMax);
        nStart = max(0, nScrollPos + ps.rcPaint.top / yChar - 1);
        nEnd = min((SHORT)(pGlobal->dwBlockSize / 16) + 1,
            nScrollPos + ps.rcPaint.bottom / yChar + 1);

        /* Get a font to use */
   		hFont = GetStockObject(SYSTEM_FIXED_FONT);
		hOldFont = SelectObject(ps.hdc, hFont);

        /* Loop through and draw all lines */
        for (; nStart < nEnd ; ++nStart)
            DumpMem(hwnd, ps.hdc, nScrollPos, nStart);

		/* Delete the font that is no longer needed */
		DeleteObject(SelectObject(ps.hdc, hOldFont));

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        /* Free memory associated with this window */
        LocalFree(GetWindowWord(hwnd, MEM_GLOBALENTRY));
        w = GetWindowWord(hwnd, MEM_HOURBLOCK);
        GlobalUnlock(w);
        GlobalFree(w);
        return 0L;

    case WM_COMMAND:
        switch (wParam)
        {
        case 1:     /* Close button */
			SendMessage(hwnd, WM_CLOSE, 0, 0L);
            return 0L;

        case 2:     /* Write button */
            return 0L;

        default:
            return DefWindowProc(hwnd, wMessage, wParam, lParam);
        }

    case WM_SYSCOMMAND:
        if (wParam == IDM_WRITE)
        {
            /* Write the block */
            pGlobal = (GLOBALENTRY *)GetWindowWord(hwnd, MEM_GLOBALENTRY);
            lpBlock = (LPSTR)GetWindowLong(hwnd, MEM_LPOURBLOCK);
            dwReturn = MemoryWrite(GetWindowWord(hwnd, MEM_BLOCK),
                0L, lpBlock, pGlobal->dwBlockSize);
            wsprintf(szText, "%lXh bytes written", dwReturn);
            MessageBox(hwnd, szText, "Memory Browser Write", MB_OK);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        else if (wParam == IDM_WRITEBP)
        {
            /* Write the breakpoint only if it's code */
            pGlobal = (GLOBALENTRY *)GetWindowWord(hwnd, MEM_GLOBALENTRY);
            if (pGlobal->wType != GT_CODE)
            {
                MessageBox(hwnd, "Breakpoints only go in code segments",
                    "Memory Browser Breakpoint", MB_OK);
                return 0L;
            }
            lpBlock = (LPSTR)GetWindowLong(hwnd, MEM_LPOURBLOCK);
	    *lpBlock =(WORD) 0xcc;
            dwReturn = MemoryWrite(GetWindowWord(hwnd, MEM_BLOCK),
                0L, lpBlock, 1);
            wsprintf(szText, "%lXh bytes written", dwReturn);
            MessageBox(hwnd, szText, "Memory Browser Write", MB_OK);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        else
            return DefWindowProc(hwnd, wMessage, wParam, lParam);

    default:
        return DefWindowProc(hwnd, wMessage, wParam, lParam);
    }

    return 0L;
}


/*  DumpMem
 *      Dumps memory to the memory window.  This routine is called once
 *      per memory line to dump in the middle of the paint message.
 */

void PASCAL DumpMem(
    HWND hwnd,
    HDC hDC,
    SHORT nScrollPos,
    SHORT nPos)
{
    LPSTR lpMem;
    DWORD dwOffset;
    WORD i;
    PSTR pstr;
    BYTE by;
    BYTE byCount;
    BYTE byBadCount;
    GLOBALENTRY *pGlobal;

    /* Get a pointer to the memory */
    dwOffset = ((DWORD)(WORD)nPos) << 4;
    lpMem = (LPSTR)MAKELONG(LOWORD(dwOffset),
        GetWindowWord(hwnd, MEM_LPOURBLOCK + 2) +
        HIWORD(dwOffset) * (WORD)&_AHINCR);

    /* How many real characters are there to draw? */
    pGlobal = (GLOBALENTRY *)GetWindowWord(hwnd, MEM_GLOBALENTRY);
    if (pGlobal->dwBlockSize < dwOffset + 16)
    {
        if (pGlobal->dwBlockSize < dwOffset)
        {
            byCount = 0;
            byBadCount = 16;
        }
        else
        {
	    byCount = (BYTE)(pGlobal->dwBlockSize - dwOffset);
	    byBadCount = (BYTE)(16 - byCount);
        }
    }
    else
    {
        byCount = 16;
        byBadCount = 0;
    }

    /* Put into a string so we can see it */
    pstr = szText;
    pstr += wsprintf(pstr, "%06lX:", dwOffset);
    for (i = 0 ; i < byCount ; ++i)
        pstr += wsprintf(pstr, "%02X ",
            (WORD)*(unsigned char FAR *)(lpMem + i));
    for (i = 0 ; i < byBadCount ; ++i)
        pstr += wsprintf(pstr, "?? ");
    for (i = 0 ; i < byCount ; ++i)
    {
        by = *(lpMem + i);
        pstr += wsprintf(pstr, "%c", by >= ' ' ? by : '.');
    }
    for (i = 0 ; i < byBadCount ; ++i)
        pstr += wsprintf(pstr, "?");

    /* Draw the text */
    TextOut(hDC, xChar, yChar * (nPos - nScrollPos), szText,
        strlen(szText));
}


/*  WalkGlobalHeap
 *      Walks the global heap in the list box.  Returns the WndProc return
 *      value.
 */

LONG WalkGlobalHeap(
    HWND hwnd)
{
    GLOBALINFO GlobalInf;
    GLOBALENTRY Global;
    MODULEENTRY Module;
    TASKENTRY Task;
    char *npText;
    char *npstr;
    int i;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_GLOBAL;

    /* Allocate a buffer to store this stuff in.  Pad this number
     *  because the LocalAlloc corrupts the walk.
     */
    GlobalInf.dwSize = sizeof (GLOBALINFO);
    GlobalInfo(&GlobalInf);
    npText = npstr = (char *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
        (GlobalInf.wcItems + 10) * (LIST_WIDTH + 1));
    if (!npText)
        return 0L;

    /* Loop through the global heap */
    Global.dwSize = sizeof (GLOBALENTRY);
    Module.dwSize = sizeof (MODULEENTRY);
    Task.dwSize = sizeof (TASKENTRY);
    if (GlobalFirst(&Global, GLOBAL_ALL))
    {
        char temp[30];

        i = 0;
        do
        {
            /* Get the module name */
            if (!Global.hOwner)
                lstrcpy(Module.szModule, "FREE");
            else if (!ModuleFindHandle(&Module, Global.hOwner))
            {
                if (TaskFindHandle(&Task, Global.hOwner))
                    lstrcpy(Module.szModule, Task.szModule);
                else
                    *Module.szModule = '\0';
            }

            /* Put some fun garbage in the text buffer */
            if (bHasData[Global.wType])
                sprintf(temp, "%s %d",
                    szBlockTypes[Global.wType], Global.wData);
            else
                lstrcpy(temp,szBlockTypes[Global.wType]);
            sprintf(npstr,
                "A=%08lX h=%04X S=%08lX O=%04X pglk=%d %c %-8s %s",
                Global.dwAddress, Global.hBlock, Global.dwBlockSize,
                Global.hOwner, Global.wcPageLock,
                Global.wHeapPresent ? 'Y' : 'N',
                Module.szModule,
                temp);

            /* Bump to the next spot */
            npstr += LIST_WIDTH + 1;
            ++i;
        }
        while (GlobalNext(&Global, GLOBAL_ALL));
    }

    /* Create number of items string */
    sprintf(szText, "GLOBAL_ALL:  Items = %u Really = %u",
        GlobalInf.wcItems, i);

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through all blocks, putting them in list box */
    for (i = 0, npstr = npText ; i < (int)GlobalInf.wcItems ;
        ++i, npstr += LIST_WIDTH + 1)
        if (*npstr)
            SendMessage(hwndList, LB_ADDSTRING, 0,
                (LONG)(LPSTR)npstr);
    LocalFree((HANDLE)npText);
        
    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, szText);
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}


/*  WalkFreeList
 *      Walks the free list, putting the result in a list box.
 */

LONG WalkFreeList(
    HWND hwnd)
{
    GLOBALINFO GlobalInf;
    GLOBALENTRY Global;
    MODULEENTRY Module;
    char *npText;
    char *npstr;
    int i;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_GLOBAL;

    /* Allocate a buffer to store this stuff in */
    GlobalInf.dwSize = sizeof (GLOBALINFO);
    GlobalInfo(&GlobalInf);
    npText = npstr = (char *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
        (GlobalInf.wcItemsFree + 10) * (LIST_WIDTH + 1));
    if (!npText)
        return 0L;

    /* Loop through the global heap */
    Global.dwSize = sizeof (GLOBALENTRY);
    if (GlobalFirst(&Global, GLOBAL_FREE))
    {
        i = 0;
        do
        {
            /* Get the module name */
            Module.dwSize = sizeof (MODULEENTRY);
            if (!Global.hOwner)
                lstrcpy(Module.szModule, "FREE");
            else if (!ModuleFindHandle(&Module, Global.hOwner))
                *Module.szModule = '\0';

            /* Put some fun garbage in the text buffer */
            sprintf(npstr, "A=%08lX h=%04X S=%08lX O=%04X f=%02X T=%2d %c %s",
                Global.dwAddress, Global.hBlock, Global.dwBlockSize,
                Global.hOwner, Global.wFlags, Global.wType,
                Global.wHeapPresent ? 'Y' : 'N', Module.szModule);

            /* Bump to the next spot */
            npstr += LIST_WIDTH + 1;
            ++i;
        }
        while (GlobalNext(&Global, GLOBAL_FREE));
    }

    /* Create number of items string */
    sprintf(szText, "GLOBAL_FREE:  Items = %u Really = %u",
        GlobalInf.wcItemsFree, i);

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through all blocks, putting them in list box */
    for (i = 0, npstr = npText ; i < (int)GlobalInf.wcItemsFree ;
        ++i, npstr += LIST_WIDTH + 1)
        if (*npstr)
            SendMessage(hwndList, LB_ADDSTRING, 0,
                (LONG)(LPSTR)npstr);
    LocalFree((HANDLE)npText);
        
    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, szText);
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}


/*  WalkLRUList
 *      Walks the LRU list, putting the result in a list box.
 */

LONG WalkLRUList(
    HWND hwnd)
{
    GLOBALINFO GlobalInf;
    GLOBALENTRY Global;
    MODULEENTRY Module;
    char *npText;
    char *npstr;
    int i;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_GLOBAL;

    /* Allocate a buffer to store this stuff in */
    GlobalInf.dwSize = sizeof (GLOBALINFO);
    GlobalInfo(&GlobalInf);
    npText = npstr = (char *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
        (GlobalInf.wcItemsLRU + 10) * (LIST_WIDTH + 1));
    if (!npText)
        return 0L;

    /* Loop through the global heap */
    Global.dwSize = sizeof (GLOBALENTRY);
    if (GlobalFirst(&Global, GLOBAL_LRU))
    {
        i = 0;
        do
        {
            /* Get the module name */
            Module.dwSize = sizeof (MODULEENTRY);
            if (!Global.hOwner)
                lstrcpy(Module.szModule, "FREE");
            else if (!ModuleFindHandle(&Module, Global.hOwner))
                *Module.szModule = '\0';

            /* Put some fun garbage in the text buffer */
            sprintf(npstr, "A=%08lX h=%04X S=%08lX O=%04X f=%02X T=%2d %c %s",
                Global.dwAddress, Global.hBlock, Global.dwBlockSize,
                Global.hOwner, Global.wFlags, Global.wType,
                Global.wHeapPresent ? 'Y' : 'N', Module.szModule);

            /* Bump to the next spot */
            npstr += LIST_WIDTH + 1;
            ++i;
        }
        while (GlobalNext(&Global, GLOBAL_LRU));
    }

    /* Create number of items string */
    sprintf(szText, "GLOBAL_LRU:  Items = %u Really = %u",
        GlobalInf.wcItemsLRU, i);

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through all blocks, putting them in list box */
    for (i = 0, npstr = npText ; i < (int)GlobalInf.wcItemsLRU ;
        ++i, npstr += LIST_WIDTH + 1)
        if (*npstr)
            SendMessage(hwndList, LB_ADDSTRING, 0,
                (LONG)(LPSTR)npstr);
    LocalFree((HANDLE)npText);
        
    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, szText);
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}


/*  WalkLocalHeap
 *      Walks the local heap into the second list box
 */

LONG WalkLocalHeap(
    HWND hwnd,
    HANDLE hBlock)
{
    LOCALENTRY Local;
    LOCALINFO LocalInf;
    char *npText;
    char *npstr;
    HCURSOR hCursor;
    WORD i;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    /* Allocate a buffer to do the local heapwalk */
    LocalInf.dwSize = sizeof (LOCALINFO);
    LocalInfo(&LocalInf, hBlock);
    npText = npstr = (char *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
        LocalInf.wcItems * (LIST_WIDTH + 1));
    if (!npText)
        return 0L;

    /* Loop through the local heap */
    Local.dwSize = sizeof (LOCALENTRY);
    if (LocalFirst(&Local, hBlock))
    {
        char temp[30];

        do
        {
            /* Get the type string */
            if (Local.wFlags & LF_FREE)
                strcpy(temp, "Free");
            else if (Local.wHeapType == GDI_HEAP &&
                Local.wType <= LT_GDI_MAX)
                strcpy(temp, szGDI[Local.wType]);
            else if (Local.wHeapType == USER_HEAP &&
                Local.wType <= LT_USER_MAX)
                strcpy(temp, szUser[Local.wType]);
            else
                strcpy(temp, "Unknown");

            /* Put some fun garbage in the text buffer */
            sprintf(npstr, "Ad=%04X h=%04X Sz=%04X %-8s L=%02X %s",
                Local.wAddress, Local.hHandle, Local.wSize,
                szLocalFlags[Local.wFlags & 7], Local.wcLock, temp);

            /* Bump to the next spot */
            npstr += LIST_WIDTH + 1;
        }
        while (LocalNext(&Local));
    }

    /* Clear list box */
    SendMessage(hwndListLocal, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndListLocal, LB_RESETCONTENT, 0, 0L);

    /* Loop through all blocks, putting them in list box */
    for (i = 0, npstr = npText ; i < LocalInf.wcItems ;
        ++i, npstr += LIST_WIDTH + 1)
        if (*npstr)
            SendMessage(hwndListLocal, LB_ADDSTRING, 0,
                (LONG)(LPSTR)npstr);
    LocalFree((HANDLE)npText);
        
    /* OK to redraw list box now */
    SendMessage(hwndListLocal, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndListLocal, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}



/*  WalkModuleList
 *      Walks the module list into the list box
 */

LONG WalkModuleList(
    HWND hwnd)
{
    MODULEENTRY Module;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_MODULE;

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through the module list */
    Module.dwSize = sizeof (MODULEENTRY);
    if (ModuleFirst(&Module))
        do
        {
            /* Put some fun garbage in the text buffer */
            sprintf(szText, "Mod=%s Path=%s", 
                Module.szModule, Module.szExePath);

            /* Put the string in the list box */
            SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
        }
        while (ModuleNext(&Module));

    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, "Module List");
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}



/*  WalkTaskList
 *      Walks the module list into the list box
 */

LONG WalkTaskList(
    HWND hwnd)
{
    TASKENTRY Task;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_TASK;

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through the task list */
    Task.dwSize = sizeof (TASKENTRY);
    if (TaskFirst(&Task))
        do
        {
            /* Put some fun garbage in the text buffer */
            sprintf(szText,
                "hTask=%04X Par=%04X hInst=%04X Mod=%04X szMod=%s",
                Task.hTask, Task.hTaskParent, Task.hInst, Task.hModule,
                Task.szModule);

            /* Put the string in the list box */
            SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
        }
        while (TaskNext(&Task));

    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, "Task List");
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}



/*  WalkClassList
 *      Walks the module list into the list box
 */

LONG WalkClassList(
    HWND hwnd)
{
    CLASSENTRY Class;
    HCURSOR hCursor;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    wListStatus = LIST_CLASS;

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Loop through the class list */
    Class.dwSize = sizeof (CLASSENTRY);
    if (ClassFirst(&Class))
        do
        {
            /* Put some fun garbage in the text buffer */
            sprintf(szText, "Name = %s  hInst = %04X",
                Class.szClassName, Class.hInst);

            /* Put the string in the list box */
            SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
        }
        while (ClassNext(&Class));

    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, "Class List");
    InvalidateRect(hwndListStatic, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}



/*  DoStackTrace
 *      Does a stack trace for the current module in the lower box
 */

LONG DoStackTrace(
    HWND hwnd)
{
    STACKTRACEENTRY StackTrace;
    MODULEENTRY ModuleEntry;
    HCURSOR hCursor;
    HANDLE hTask;
    WORD wCS;
    HANDLE hModule;

    /* Turn on the hourglass */
    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    /* Get the task handle from the list box */
    SendMessage(hwndList, LB_GETTEXT,
        (WORD)SendMessage(hwndList, LB_GETCURSEL, 0, 0L),
        (LONG)(LPSTR)szText);
    sscanf(szText, "%*6c%x", &hTask);

    /* Clear list box */
    SendMessage(hwndListLocal, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndListLocal, LB_RESETCONTENT, 0, 0L);

    /* Do the stack trace */
    StackTrace.dwSize = sizeof (STACKTRACEENTRY);
    wCS = 0;
    hModule = NULL;
    if (StackTraceFirst(&StackTrace, hTask))
        do
        {
            /* Get the module name */
            ModuleEntry.dwSize = sizeof (MODULEENTRY);
            if (!ModuleFindHandle(&ModuleEntry, StackTrace.hModule))
                ModuleEntry.szModule[0] = '\0';

            /* Put some fun garbage in the text buffer */
            sprintf(szText, "BP=%04X CS:IP=%04X:%04X (Mod=%s Seg=%u)", 
                StackTrace.wBP, StackTrace.wCS, StackTrace.wIP,
                ModuleEntry.szModule, StackTrace.wSegment);

            /* Set the last wCS in case we're using a near frame */
            if (StackTrace.wCS)
            {
                wCS = StackTrace.wCS;
                hModule = StackTrace.hModule;
            }

            /* Put the string in the list box */
            SendMessage(hwndListLocal, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
        }
        while (StackTraceNext(&StackTrace));

    /* OK to redraw list box now */
    SendMessage(hwndListLocal, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndListLocal, NULL, TRUE);

    /* Done with hourglass */
    ShowCursor(FALSE);
    SetCursor(hCursor);

    return 0L;
}


/*  DoHeapInfo
 *      Displays the data from a call to SystemHeapInfo()
 */

LONG DoHeapInfo(
    HWND hwnd)
{
    SYSHEAPINFO SysHeap;

    wListStatus = LIST_HEAP;

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Get the user information */
    SysHeap.dwSize = sizeof (SYSHEAPINFO);
    SystemHeapInfo(&SysHeap);

    /* Display the User information */
    wsprintf(szText, "hUserSegment = %04X", SysHeap.hUserSegment);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "hGDISegment = %04X", SysHeap.hGDISegment);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Free User Resources = %u%%", SysHeap.wUserFreePercent);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Free GDI Resources = %u%%", SysHeap.wGDIFreePercent);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);

    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, "System Heap Information");
    InvalidateRect(hwndListStatic, NULL, TRUE);

    return 0L;
}


/*  DoMemManInfo
 *      Displays the data from a call to UserInfo()
 */

LONG DoMemManInfo(
    HWND hwnd)
{
    MEMMANINFO MemMan;

    wListStatus = LIST_MEMMAN;

    /* Clear list box */
    SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
    SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);

    /* Get the Memory manager information */
    MemMan.dwSize = sizeof (MEMMANINFO);
    MemManInfo(&MemMan);

    /* Display the MemMan information */
    wsprintf(szText, "Largest Free Block = %08lXh", MemMan.dwLargestFreeBlock);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Max Pages Available = %08lXh", MemMan.dwMaxPagesAvailable);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Max Pages Lockable = %08lXh", MemMan.dwMaxPagesLockable);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Total Linear Space = %08lXh", MemMan.dwTotalLinearSpace);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Total Unlocked Pages = %08lXh", MemMan.dwTotalUnlockedPages);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Free Pages = %08lXh", MemMan.dwFreePages);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Total Pages = %08lXh", MemMan.dwTotalPages);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Free Linear Space = %08lXh", MemMan.dwFreeLinearSpace);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Swap File Pages = %08lXh", MemMan.dwSwapFilePages);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);
    wsprintf(szText, "Page Size = %04Xh", MemMan.wPageSize);
    SendMessage(hwndList, LB_ADDSTRING, 0, (LONG)(LPSTR)szText);

    /* OK to redraw list box now.  Also draw its title */
    SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
    InvalidateRect(hwndList, NULL, TRUE);
    SetWindowText(hwndListStatic, "Memory Manager Information");
    InvalidateRect(hwndListStatic, NULL, TRUE);

    return 0L;
}

/*  DoGlobalEntryModuleTest
 *      Tests the GlobalEntryModule API for correctness.
 */

LONG DoGlobalEntryModuleTest(
    HWND hwnd)
{
    GLOBALENTRY Global;
    HANDLE hModule;

    /* Get an interesting module handle */
    hModule = GetModuleHandle("USER");

    /* Find out about the segment */
    Global.dwSize = sizeof (GLOBALENTRY);
    if (!GlobalEntryModule(&Global, hModule, 14))
	{
	MessageBox(hwnd, "Error returned!", "GlobalEntryModule Test",
            MB_OK | MB_ICONEXCLAMATION);
	return(0);
	}
    else
    {
        wsprintf(szText, "USER Code Seg 14 has handle %04X", Global.hBlock);
        MessageBox(hwnd, szText, "GlobalEntryModule Test",
            MB_OK | MB_ICONINFORMATION);
	return(1);
    }

}


/*  ReadMemoryTest
 *      Opens a popup window and allows the user to scroll through the
 *      contents of the memory block.
 */

LONG ReadMemoryTest(
    HWND hwnd,
    HANDLE hBlock)
{
    HWND hwndPopup;
    GLOBALENTRY Global;

    /* Make a popup window to handle the data */
    Global.dwSize = sizeof (GLOBALENTRY);
    GlobalEntryHandle(&Global, hBlock);
    wsprintf(szText,"Handle = %4Xh  Length = %8lXh",
        hBlock, Global.dwBlockSize);
    hwndPopup = CreateWindow(
        szMemName,
        szText,
        WS_POPUP | WS_CAPTION |
        WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_VSCROLL,
        xScreen / 2 - 45 * xChar,
        yScreen / 8,
        90 * xChar,
        2 * yScreen / 3,
        hwnd,
        NULL,
        hInst,
        (LPSTR)MAKELONG(hBlock, 0));
 
    /* Display the window */
    ShowWindow(hwndPopup, SW_SHOWNORMAL);
    UpdateWindow(hwndPopup);

    return 0L;
}


/*  TimerCountTest
 *      Tests the timer count function.
 */

LONG TimerCountTest(
    HWND hwnd)
{
    TIMERINFO TimerInfo;

    /* Get the tick count */
    TimerInfo.dwSize = sizeof (TIMERINFO);
    if (!TimerCount(&TimerInfo))
    {
        MessageBox(hwnd, "Error calling TimerCount()", "Timer Count Test",
            MB_OK);
        return 0L;
    }

    /* Display it */
    wsprintf(szText, "Milliseconds since Windows started = %ld\r\n"
        "Milliseconds in this VM = %ld",
        TimerInfo.dwmsSinceStart, TimerInfo.dwmsThisVM);
    MessageBox(hwnd, szText, "Timer Count Test", MB_OK | MB_ICONINFORMATION);

    /* Return success */
    return 0L;
}


/*  MyNotifyHandler
 *      Notification message callback
 */

BOOL FAR PASCAL MyNotifyHandler(
    WORD wID,
    DWORD dwData)
{
    /* See if we should process this notification */
    if (!wNotifyState)
        return FALSE;

    /* Filter out task switch notifications if necessary */
    if (wFilterState && (wID == NFY_TASKIN || wID == NFY_TASKOUT))
        return FALSE;

    /* Put the information in a message */
    PostMessage(hwndMain, WM_USER, wID, dwData);

    /* Only return that we handled RIPs */
    if (wID == NFY_RIP)
        return TRUE;
    else
        return FALSE;
}


/*  MyCFaultHandler
 *      This routine is used to prove that C routines can be used to
 *      make fault handlers.  As can be seen here, the parameters are
 *      actually pointing into the stack frame.  Two important notes:
 *          1) This function MUST be declared as _cdecl so that the
 *              parameters are not popped off the stack!
 *          2) This function may change these values with the understanding
 *              that they are actually passed "by value" implying that
 *              any changes are for real.
 *      As defined in MyFaultHandler (TEST2.ASM), 0 nukes app, 1 restarts
 *      the instruction, 2 chains on.
 */

WORD _cdecl MyCFaultHandler(
    WORD wES,
    WORD wDS,
    WORD wDI,
    WORD wSI,
    WORD wBP,
    WORD wSP,
    WORD wBX,
    WORD wDX,
    WORD wCX,
    WORD wOldAX,
    WORD wOldBP,
    WORD wRetIP,
    WORD wRetCS,
    WORD wRealAX,
    WORD wNumber,
    WORD wHandle,
    WORD wIP,
    WORD wCS,
    WORD wFlags)
{
    FARPROC lpfnDlg;
    int nResult;
    static WORD wReentry;

    /* See if we're already here.  If so, tell routine to chain on */
    if (wReentry)
        return 2;
    wReentry = 1;

    /* If this was a CtlAltSysRq interrupt, just restart the instr. */
    if (wNumber == INT_CTLALTSYSRQ)
    {
        wsprintf(szText, "THTest:  CtlAltSysRq at %04X:%04X\r\n", wCS, wIP);
        OutputDebugString(szText);
        wReentry = 0;
        return 1;
    }

    /* Set the static variables */
    wsCS = wCS;
    wsIP = wIP;
    wsFault = wNumber;
    wsFaultTask = GetCurrentTask();

    /* Use the dialog box to determine what to do with the fault */
    lpfnDlg = MakeProcInstance((FARPROC)FaultDialogProc, hInst);
    nResult = DialogBox(hInst, MAKEINTRESOURCE(IDD_FAULT), hwndMain, lpfnDlg);
    FreeProcInstance(lpfnDlg);

    /* We're getting out now, so undo reentry flag */
    wReentry = 0;

    return (WORD)nResult;
}


/*  FaultDialogProc
 *      Handles the Fault dialog box
 *      It returns 0 to nuke the app, 1 to restart the instruction,
 *      2 to chain on
 */

BOOL FAR PASCAL FaultDialogProc(
    HWND hDlg,
    WORD wMessage,
    WORD wParam,
    DWORD dwParam)
{
    switch (wMessage)
    {
    case WM_INITDIALOG:
        wsprintf(szText, "%d", wsFault);
        SetDlgItemText(hDlg, IDC_FAULTNUM, szText);
        wsprintf(szText, "%04X:%04X", wsCS, wsIP);
        SetDlgItemText(hDlg, IDC_CSIP, szText);
        wsprintf(szText, "%04X", wsFaultTask);
        SetDlgItemText(hDlg, IDC_HFAULT, szText);
        wsprintf(szText, "%04X", wsProgramTask);
        SetDlgItemText(hDlg, IDC_HPROGRAM, szText);
        return TRUE;

    case WM_COMMAND:
        switch (wParam)
        {
        case IDC_KILL:
            EndDialog(hDlg, 0);
            return TRUE;

        case IDC_RESTART:
            EndDialog(hDlg, 1);
            return TRUE;

        case IDC_CHAIN:
            EndDialog(hDlg, 2);
            return TRUE;

        default:
            return FALSE;
        }

    default:
        return FALSE;
    }
}
