#include "dbwin.h"

// Max no. of lines allowed in edit control buffer
// (this will hopefully keep us under the 64k buffer maximum)
//
#define CLINESMAX   500

// Globals

HINSTANCE   hinstDBWin = NULL;

char szDBWinClass[] = "DBWin";

HWND        hwndDBWin = NULL;
HWND        hwndClient = NULL;
HFONT       hfontClient = NULL;
HACCEL      haccelDBWin = NULL;

BOOL        fTopmost = FALSE;       // Stay on top or not

BOOL DBWinInit(HINSTANCE hinst, HINSTANCE hinstPrev, int showCmd, LPSTR szCmdLine)
{
    WNDCLASS cls;
    LOGFONT  lf;

    if (hinstPrev)
    {
        // Only one instance allowed.
        //
        // Find the first instance main window, and post a message
        // to it to tell it to activate itself.
        //
        HWND hwnd = FindWindow(szDBWinClass, NULL);

        if (hwnd)
        {
            PostMessage(hwnd, WM_ACTIVATEFIRST, 0, 0L);
            return FALSE;
        }
    }

    hinstDBWin = hinst;

    cls.hCursor        = LoadCursor(NULL, IDC_ARROW);
    cls.hIcon          = LoadIcon(hinstDBWin, MAKEINTRESOURCE(IDR_MAINICON));
    cls.lpszMenuName   = MAKEINTRESOURCE(IDR_MAINMENU);
    cls.lpszClassName  = szDBWinClass;
    cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    cls.hInstance      = hinstDBWin;
    cls.style          = CS_BYTEALIGNCLIENT;
    cls.lpfnWndProc    = DBWinWndProc;
    cls.cbWndExtra     = 0;
    cls.cbClsExtra     = 0;

    if (!RegisterClass(&cls))
        return FALSE;

    haccelDBWin = LoadAccelerators(hinstDBWin, MAKEINTRESOURCE(IDR_MAINACCEL));
    if (!haccelDBWin)
        return FALSE;

    if (!OutputInit())
        return FALSE;

    // Create the main window

    hwndDBWin = CreateWindow(szDBWinClass,          // Class name
                            "Debug Messages",       // Caption
                            WS_OVERLAPPEDWINDOW,    // Style bits
                            0, 0, 100, 100,
                            (HWND)NULL,             // Parent window (no parent)
                            (HMENU)NULL,            // use class menu
                            (HINSTANCE)hinstDBWin,  // handle to window instance
                            (LPSTR)NULL             // no params to pass on
                           );
    if (!hwndDBWin)
        return FALSE;

    SetBufferNotify(hwndDBWin);

    hwndClient = CreateWindow("EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL |
            ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
            0, 0, 0, 0,
            hwndDBWin, (HMENU)0, hinstDBWin, NULL);

    if (!hwndClient)
        return FALSE;

    // Use the small icon title font
    //
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, FALSE);
    hfontClient = CreateFontIndirect(&lf);
    if (hfontClient)
        SendMessage(hwndClient, WM_SETFONT, (WPARAM)(UINT)hfontClient, 1);

    ShowWindow(hwndClient, SW_SHOWNORMAL);

    ReadDBWinState(hwndDBWin);

    return TRUE;
}

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int showCmd)
{
    MSG msg;

    if (!DBWinInit(hinst, hinstPrev, showCmd, lpszCmdLine))
        return FALSE;

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator(hwndDBWin, haccelDBWin, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DBWinTerminate(FALSE);
    return msg.wParam;
}

void DBWinTerminate(BOOL fEndSession)
{
    // Write out the application state
    //
    WriteDBWinState(hwndDBWin);

    // Make sure our hook is unregistered...

    SetOutputMode(OMD_NONE);

    if (!fEndSession)
    {
        if (hwndDBWin)
        {
            DestroyWindow(hwndDBWin);
            hwndDBWin = NULL;
        }

        if (hfontClient)
        {
            DeleteObject(hfontClient);
            hfontClient = NULL;
        }
    }
}

LRESULT CALLBACK _export DBWinWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        DoCommand(hwnd, msg, wParam, lParam);
        return 0L;

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_ENDSESSION:
        DBWinTerminate(TRUE);
        break;

    case WM_ACTIVATE:
        if (wParam)
            SetFocus(hwndClient);
        break;

    case WM_SIZE:
        /* Make sure the edit control always occupies the entire
         * client area.
         */
        if (hwndClient)
        {
            RECT rc;

            GetClientRect(hwnd, &rc);
            //
            // Outset border
            //
            InflateRect(&rc, 1, 1);
            MoveWindow(hwndClient, rc.top, rc.left, rc.right - rc.left, rc.bottom - rc.top, TRUE);
        }
        break;

    case WM_INITMENU:
        DoInitMenu(hwnd);
        break;

    case WM_BUFFERNOTEMPTY:
        OnBufferNotEmpty();
        return 0L;

    case WM_ACTIVATEFIRST:
        // The user tried to run a second instance of this app.
        // The second instance posted this message to us and exited.
        //
        // If we're iconic, restore ourself, otherwise just make
        // ourself the active window.
        //
        if (IsIconic(hwnd))
            ShowWindow(hwnd, SW_RESTORE);
        else
            SetActiveWindow(hwnd);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0L;
}

void DoCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case CMD_HELPABOUT:
        DoAbout(hwnd);
        break;

    case CMD_FILEEXIT:
        PostQuitMessage(0);
        break;

    case CMD_EDITCOPY:
        PostMessage(hwndClient, WM_COPY, 0, 0L);
        break;

    case CMD_EDITCLEARBUFFER:
        Edit_SetSel(hwndClient, 0, 32767);
        Edit_ReplaceSel(hwndClient, "");
        break;

    case CMD_EDITSELECTALL:
        Edit_SetSel(hwndClient, 0, 32767);
        break;

    case CMD_FILESAVEBUFFER:
        SaveBuffer(hwnd);
        break;

    case CMD_OPTIONSOUTWINDOW:
    case CMD_OPTIONSOUTCOM1:
    case CMD_OPTIONSOUTMONO:
    case CMD_OPTIONSOUTNONE:
        SetOutputMode(ModeFromCmd(wParam));
        break;

    case CMD_OPTIONSTOPMOST:
        SetTopmost(hwnd, !fTopmost);
        break;

    case CMD_OPTIONSSETTINGS:
        if (IsDebugSystem(hwnd))
            DoDebugOptions(hwnd);
        break;

    case CMD_FILESAVESETTINGS:
        if (IsDebugSystem(hwnd))
            DoSaveOptions(hwnd);
        break;

    case CMD_OPTIONSALLOCBREAK:
        if (IsDebugSystem(hwnd))
            DoAllocBrk(hwnd);
        break;
    }
}

BOOL IsDebugSystem(HWND hwnd)
{
    if (!GetSystemMetrics(SM_DEBUG))
    {
        MessageBox(hwnd,
                "This command requires the debugging Windows system binaries",
                NULL, MB_OK | MB_ICONSTOP);

        return FALSE;
    }
    return TRUE;
}

void OnBufferNotEmpty(void)
{
    OUTBUFINFO obi;
    int cLines;

    if (!GetOutputBufferInfo(&obi) || obi.cch == 0)
        return;

    // First see if we will exceed our line maximum
    //
    cLines = Edit_GetLineCount(hwndClient) + obi.cLines;
    if (cLines > CLINESMAX)
    {
        Edit_SetSel(hwndClient, 0, Edit_LineIndex(hwndClient, cLines - CLINESMAX));
        Edit_ReplaceSel(hwndClient, "");
    }

    // Append to the end of the buffer.
    //
    Edit_SetSel(hwndClient, 32767, 32767);
    Edit_ReplaceSel(hwndClient, obi.lpch);
    Edit_SetSel(hwndClient, 32767, 32767);

    // Clear the buffer
    //
    ResetBuffer();
}

UINT CmdFromMode(UINT mode)
{
    static UINT mpModeCmd[] =
    {
        CMD_OPTIONSOUTNONE,     // OMD_NONE
        CMD_OPTIONSOUTWINDOW,   // OMD_BUFFER
        CMD_OPTIONSOUTCOM1,     // OMD_COM1
        CMD_OPTIONSOUTCOM2,     // OMD_COM2
        CMD_OPTIONSOUTMONO      // OMD_MONO
    };

    return mpModeCmd[mode - OMD_NONE];
}

UINT ModeFromCmd(UINT cmd)
{
    static UINT mpCmdMode[] =
    {
        OMD_BUFFER,     // CMD_OPTIONSOUTWINDOW
        OMD_COM1,       // CMD_OPTIONSOUTCOM1
        OMD_COM2,       // CMD_OPTIONSOUTCOM2
        OMD_MONO,       // CMD_OPTIONSOUTMONO
        OMD_NONE        // CMD_OPTIONSOUTNONE
    };

    return mpCmdMode[cmd - CMD_OPTIONSOUTWINDOW];
}

void SetTopmost(HWND hwnd, BOOL fTopmostT)
{
    fTopmost = fTopmostT;

    SetWindowPos(hwnd, (fTopmostT ? HWND_TOPMOST : HWND_NOTOPMOST),
            0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
}

void DoInitMenu(HWND hwnd)
{
    UINT mode;
    UINT flagsOutput;
    HMENU hmenu = GetMenu(hwnd);
    UINT modeCur = GetOutputMode();

    flagsOutput = GetOutputFlags();

    for (mode = OMD_NONE; mode <= OMD_MONO; mode++)
    {
        CheckMenuItem(hmenu, CmdFromMode(mode),
            ((modeCur == mode) ? MF_CHECKED : MF_UNCHECKED));
    }

    EnableMenuItem(hmenu, CMD_OPTIONSOUTMONO,
            ((flagsOutput & DBOF_HASMONO) ? MF_ENABLED : MF_GRAYED));

    EnableMenuItem(hmenu, CMD_OPTIONSOUTCOM1,
            ((flagsOutput & DBOF_HASCOM1) ? MF_ENABLED : MF_GRAYED));

    EnableMenuItem(hmenu, CMD_OPTIONSOUTCOM2,
            ((flagsOutput & DBOF_HASCOM2) ? MF_ENABLED : MF_GRAYED));

    EnableMenuItem(hmenu, CMD_FILESAVEBUFFER,
            ((modeCur == OMD_BUFFER) ? MF_ENABLED : MF_GRAYED));

    CheckMenuItem(hmenu, CMD_OPTIONSTOPMOST,
            (fTopmost ? MF_CHECKED : MF_UNCHECKED));
}

int IntFromString(LPSTR FAR* lplpsz)
{
    LPSTR lpsz = *lplpsz;
    int i = 0;
    char ch;
    BOOL fNeg;

    while (*lpsz == ' ')
        lpsz++;

    fNeg = FALSE;
    while (ch = *lpsz++)
    {
        if (ch == '-')
        {
            fNeg = !fNeg;
            continue;
        }

        if (ch < '0' || ch > '9')
            break;
        i = (i * 10) + (ch - '0');
    }
    *lplpsz = lpsz;

    return (fNeg ? -i : i);
}

void ReadDBWinState(HWND hwndMain)
{
    WINDOWPLACEMENT wpl;
    char ach[128];
    LPSTR lpsz;
    int cch;

    lpsz = ach;
    cch = GetProfileString("DBWin", "State", "", ach, sizeof(ach));

    if (cch == 0 || IntFromString(&lpsz) != 12)
    {
        int cxScreen, cyScreen;
        int x, y, cx, cy;

        // defaultly position window along right edge of screen
        //
        cxScreen = GetSystemMetrics(SM_CXSCREEN);
        cyScreen = GetSystemMetrics(SM_CYSCREEN);

        cx = cxScreen / 2;
        x = cxScreen - cx;
        cy = cyScreen - GetSystemMetrics(SM_CYICONSPACING);
        y = 0;

        wpl.length = sizeof(wpl);
        GetWindowPlacement(hwndMain, &wpl);
        wpl.rcNormalPosition.left = x;
        wpl.rcNormalPosition.top = y;
        wpl.rcNormalPosition.right = x + cx;
        wpl.rcNormalPosition.bottom = y + cy;

        SetOutputMode(OMD_BUFFER);

        SetTopmost(hwndMain, FALSE);

        SetWindowPlacement(hwndMain, &wpl);
        ShowWindow(hwndMain, SW_SHOW);
    }
    else
    {
        wpl.length = sizeof(wpl);
        wpl.flags = (UINT)IntFromString(&lpsz);
        wpl.showCmd = (UINT)IntFromString(&lpsz);
        wpl.ptMinPosition.x = IntFromString(&lpsz);
        wpl.ptMinPosition.y = IntFromString(&lpsz);
        wpl.ptMaxPosition.x = IntFromString(&lpsz);
        wpl.ptMaxPosition.y = IntFromString(&lpsz);
        wpl.rcNormalPosition.left = IntFromString(&lpsz);
        wpl.rcNormalPosition.top = IntFromString(&lpsz);
        wpl.rcNormalPosition.right = IntFromString(&lpsz);
        wpl.rcNormalPosition.bottom = IntFromString(&lpsz);

        SetOutputMode((UINT)IntFromString(&lpsz));

        SetTopmost(hwndMain, (BOOL)IntFromString(&lpsz));

        SetWindowPlacement(hwndMain, &wpl);
    }
}

void WriteDBWinState(HWND hwndMain)
{
    WINDOWPLACEMENT wpl;

    char ach[128];

    wpl.length = sizeof(wpl);
    GetWindowPlacement(hwndMain, &wpl);

    wsprintf(ach, "%d %d %d %d %d %d %d %d %d %d %d %d %d",
        12,
        wpl.flags,
        wpl.showCmd,
        wpl.ptMinPosition.x,
        wpl.ptMinPosition.y,
        wpl.ptMaxPosition.x,
        wpl.ptMaxPosition.y,
        wpl.rcNormalPosition.left,
        wpl.rcNormalPosition.top,
        wpl.rcNormalPosition.right,
        wpl.rcNormalPosition.bottom,
        GetOutputMode(),
        fTopmost);

    WriteProfileString("DBWin", "State", ach);
}

BOOL SaveBuffer(HWND hwnd)
{
    char szFilename[256];
    OPENFILENAME ofn;           /* passed to the File Open/save APIs */
    BOOL fSuccess = FALSE;

    szFilename[0] = 0;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = hinstDBWin;
    ofn.lpstrFilter = (LPSTR)"All\0*.*\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = (LPSTR)szFilename;
    ofn.nMaxFile = sizeof(szFilename);
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

    if (GetSaveFileName(&ofn))
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
        return fSuccess;
    }
}

void DoAbout(HWND hwnd)
{
    DLGPROC lpfndp;

    lpfndp = (DLGPROC)MakeProcInstance((FARPROC)AboutDlgProc, hinstDBWin);

    if (!lpfndp)
        return;

    DialogBoxParam(hinstDBWin, MAKEINTRESOURCE(IDR_ABOUTDLG), hwnd, lpfndp, 0L);

    FreeProcInstance((FARPROC)lpfndp);
}

BOOL CALLBACK _export AboutDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (wParam == CTL_OK || wParam == CTL_CANCEL)
            EndDialog(hwndDlg, TRUE);
        return TRUE;
        break;

    case WM_INITDIALOG:
        return TRUE;
    }
    return FALSE;
}
