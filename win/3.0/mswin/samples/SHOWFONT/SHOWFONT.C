/****************************************************************************

    PROGRAM: Showfont.c

    PURPOSE: Adds, deletes, creates and displays fonts

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        EditfileInit() - initializes window data and registers window
        EditfileWndProc() - processes messages
        About() - processes messages for "About" dialog box
        SelectFont() - select a font
        GetSizes() - get size of current font
        GetFonts() - get available fonts
        SetMyDC() - initializes DC
        Metric() - Metric dialog box
        Log() - Log dialog box
        AddDlg() - dialog box for adding a font
        RemoveDlg() - dialog box for removing a font
        CFontDlg()
        _lstrcpy() - long strcpy()
        _lstrncpy() - long strncpy()
        _lstrlen()  - long strlen()
        CheckFileName() - check for valid filename
        SeparateFile() - Separate filename and pathname
        UpdateListBox() - update file list box
        AddExt() - add default extension
        SetFaceName() - update title with current font's face name

****************************************************************************/

#include "windows.h"
#include "showfont.h"

HANDLE hInst;

HFONT hOFont, hFFont, hVFont, hSFont, hDFont, hMFont, hFont;
int hFile;
char line[4][64];
char FontNameList[32][128];                          /* list of added fonts  */
int nFontIndex = 0;                                  /* position in FontList */
int nLineSpace;

TEXTMETRIC TextMetric;
LOGFONT LogFont;
FARPROC lpCFontDlg;
POINT ptCurrent = {0, 0};
short nBkMode = OPAQUE;
DWORD rgbBkColor = RGB(255, 255, 255);
DWORD rgbTextColor = RGB(0, 0, 0);
DWORD rgbColor;
short nAlignLCR = TA_LEFT;
short nAlignTBB = TA_TOP; 
WORD wPaint = 0;
FARPROC lpColors;
char FontList[MAXFONT][32];
BYTE CharSet[MAXFONT];
BYTE PitchAndFamily[MAXFONT];
int FontIndex = 0;
int SizeList[MAXSIZE];
int SizeIndex = 0;
int CurrentFont = 0;
int CurrentSize = 0;
FARPROC lpSelectFont;
FARPROC lpEnumFunc;
WORD wPrevVAlign = IDM_ALIGNBASE;
WORD wPrevHAlign = IDM_ALIGNLEFT;
WORD wPrevFont = IDM_SYSTEM;
char AppName[] = "ShowFont Sample Application   Font: ";
char WindowTitle[80];
char str[255];
char DefPath[128];
char DefExt[] = ".fon";
char DefSpec[13];
char FontFileName[128];

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
        if (!ShowFontInit(hInstance))
            return (FALSE);

    hInst = hInstance;

    strcpy(WindowTitle, AppName);
    strcat(WindowTitle, "SYSTEM");                 /* default is SYSTEM font */

    hWnd = CreateWindow("ShowFont",
        WindowTitle,
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
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: ShowFontInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

int ShowFontInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "ShowFont";
    pWndClass->lpszClassName = (LPSTR) "ShowFont";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = ShowFontWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: SetMyDC(HDC)

    PURPOSE: Initializes the DC

****************************************************************************/

HDC SetMyDC(hDC) 
HDC hDC;
{
    SetBkMode(hDC, nBkMode);
    SetBkColor(hDC, rgbBkColor);
    SetTextColor(hDC, rgbTextColor);
    SetTextAlign(hDC, nAlignLCR | nAlignTBB);
}

/****************************************************************************

    FUNCTION: GetStringExtent(HDC, PSTR, HFONT)

    PURPOSE: get the string extent

****************************************************************************/

short GetStringExtent(hDC, pString, hFont)
HDC hDC;
PSTR pString;
HFONT hFont;
{
    HFONT hOldFont;
    DWORD dwExtent;

    if (hOldFont=SelectObject(hDC, hFont)) {
        dwExtent = GetTextExtent(hDC, pString, strlen(pString));
        SelectObject(hDC, hOldFont);
        return (LOWORD(dwExtent));
    }
    else
        return (0);
}

/****************************************************************************

    FUNCTION: GetStringExtent(HDC, PSTR, HFONT)

    PURPOSE: Sends string to application's window

****************************************************************************/

short StringOut(hDC, X, Y, pString, hFont)
HDC hDC;
short X;
short Y;
PSTR pString;
HFONT hFont;
{
    HFONT hOldFont;
    DWORD dwExtent;

    hOldFont = SelectObject(hDC, hFont);
    if (hOldFont != NULL) {
        dwExtent = GetTextExtent(hDC, pString, strlen(pString));
        TextOut(hDC, X, Y, pString, strlen(pString));
        SelectObject(hDC, hOldFont);
    }
    return (LOWORD(dwExtent));
}

/****************************************************************************

    FUNCTION: ShowString(HWND)

    PURPOSE: Show string in current font

****************************************************************************/

void ShowString(hWnd)
HWND hWnd;
{
    HFONT hItalicFont;
    HFONT hBoldFont;
    HFONT hUnderlineFont;
    HFONT hStrikeOutFont;
    HDC hDC;
    short X, tmpX;
    short Y;
    short nAlign;

    GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);
    LogFont.lfItalic = TRUE;
    hItalicFont = CreateFontIndirect(&LogFont);
    LogFont.lfItalic = FALSE;
    LogFont.lfUnderline = TRUE;
    hUnderlineFont = CreateFontIndirect(&LogFont);
    LogFont.lfUnderline = FALSE;
    LogFont.lfStrikeOut = TRUE;
    hStrikeOutFont = CreateFontIndirect(&LogFont);
    LogFont.lfStrikeOut = FALSE;
    LogFont.lfWeight = FW_BOLD;
    hBoldFont = CreateFontIndirect(&LogFont);

    hDC=GetDC(hWnd);
    SetMyDC(hDC);
    X=ptCurrent.x;
    Y=ptCurrent.y;
    nAlign =  nAlignLCR | nAlignTBB;                   /* GetTextAlign(hDC); */
    if ((nAlign & TA_CENTER) == TA_CENTER) {
        tmpX = X;
        nAlignLCR = TA_LEFT;
        SetTextAlign(hDC, nAlignLCR | nAlignTBB);
        X += StringOut(hDC, X, Y, ", and ", hFont);
        X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X += StringOut(hDC, X, Y, " in a single line.", hFont);
        X = tmpX;
        nAlignLCR = TA_RIGHT;
        SetTextAlign(hDC, nAlignLCR | nAlignTBB);
        X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "italic", hItalicFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "bold", hBoldFont);
        X -= StringOut(hDC, X, Y, "You can use ", hFont);
        nAlignLCR = TA_CENTER;
    }
    else if ((nAlign & TA_CENTER) == TA_RIGHT) {
        X -= StringOut(hDC, X, Y, " in a single line.", hFont);
        X -= StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X -= StringOut(hDC, X, Y, ", and ", hFont);
        X -= StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "italic", hItalicFont);
        X -= StringOut(hDC, X, Y, ", ", hFont);
        X -= StringOut(hDC, X, Y, "bold", hBoldFont);
        X -= StringOut(hDC, X, Y, "You can use ", hFont);
    }
    else  {
        X += StringOut(hDC, X, Y, "You can use ", hFont);
        X += StringOut(hDC, X, Y, "bold", hBoldFont);
        X += StringOut(hDC, X, Y, ", ", hFont);
        X += StringOut(hDC, X, Y, "italic", hItalicFont);
        X += StringOut(hDC, X, Y, ", ", hFont);
        X += StringOut(hDC, X, Y, "underline", hUnderlineFont);
        X += StringOut(hDC, X, Y, ", and ", hFont);
        X += StringOut(hDC, X, Y, "strikeout", hStrikeOutFont);
        X += StringOut(hDC, X, Y, " in a single line.", hFont);
    }
    ReleaseDC(hWnd, hDC);

    DeleteObject(hItalicFont);
    DeleteObject(hUnderlineFont);
    DeleteObject(hStrikeOutFont);
    DeleteObject(hBoldFont);
}

/****************************************************************************

    FUNCTION: ShowCharacterSet(HDC, HFONT)

    PURPOSE: display character set using current font

****************************************************************************/

void ShowCharacterSet(hDC, hFont)
HDC hDC;
HFONT hFont;
{
    HFONT hOldFont;
    TEXTMETRIC TextMetric;
    int LineSpace;
    short X;
    short Y;

    if (!(hOldFont = SelectObject(hDC, hFont)))
        return;
    GetTextMetrics(hDC, &TextMetric);
    nLineSpace = (TextMetric.tmHeight + TextMetric.tmExternalLeading)*2;
    X = ptCurrent.x;
    Y = ptCurrent.y;
    TextOut(hDC, X, Y, line[0], 64);
    TextOut(hDC, X, Y += nLineSpace, line[1], 64);
    TextOut(hDC, X, Y += nLineSpace, line[2], 64);
    TextOut(hDC, X, Y += nLineSpace, line[3], 64);
    SelectObject(hDC, hOldFont);
}

/****************************************************************************

    FUNCTION: ShowLogFont(HWND, HFONT)

    PURPOSE: Create dialog box to show information about logical font

****************************************************************************/

void ShowLogFont(hWnd, hFont)
HWND hWnd;
HFONT hFont;
{
    HFONT hOldFont;
    FARPROC lpProcLog;
    HDC hDC;
    TEXTMETRIC TextMetric;
    HANDLE hDlgBox;
    char buf[80];
    char DialogTitle[100];

    hDC = GetDC(hWnd);
    if (!(hOldFont = SelectObject(hDC, hSFont)))
        return;
    GetTextMetrics(hDC, &TextMetric);
    nLineSpace = TextMetric.tmHeight + TextMetric.tmExternalLeading;
    GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);

    lpProcLog = MakeProcInstance((FARPROC) Log, hInst);
    hDlgBox = CreateDialog(hInst, "LogBox", hWnd, lpProcLog);

    strcpy(DialogTitle, "Log Font: ");
    strcat(DialogTitle, LogFont.lfFaceName);
    SetWindowText(hDlgBox, (LPSTR) DialogTitle);

    SelectObject(hDC, hOldFont);
    ReleaseDC(hWnd, hDC);
}

/****************************************************************************

    FUNCTION: ShowMetricFont(HWND, HFONT)

    PURPOSE: Create dialog box to show information about metric font

****************************************************************************/

void ShowTextMetric(hWnd, hFont)
HWND hWnd;
HFONT hFont;
{
    FARPROC lpProcMetric;
    HFONT hOldFont;
    TEXTMETRIC LocalTextMetric;
    HDC hDC;
    HANDLE hDlgBox;
    char buf[80];
    char DialogTitle[100];

    hDC = GetDC(hWnd);
    if (!(hOldFont = SelectObject(hDC, hFont)))
        return;
    GetTextMetrics(hDC, &TextMetric);

    lpProcMetric = MakeProcInstance((FARPROC) Metric, hInst);
    hDlgBox = CreateDialog(hInst, "MetricBox", hWnd, lpProcMetric);

    strcpy(DialogTitle, "Metric Font: ");
    GetTextFace(hDC, 80, buf);
    strcat(DialogTitle, buf);
    SetWindowText(hDlgBox, (LPSTR) DialogTitle);

    SelectObject(hDC, hOldFont);
    ReleaseDC(hWnd, hDC);
}

/****************************************************************************

    FUNCTION: Colors(HWND, unsigned, WORD LONG)

    PURPOSE: Dialog box for changing background color of text

****************************************************************************/

BOOL FAR PASCAL Colors(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    int Red, Green, Blue;

    switch (message) {
        case WM_INITDIALOG:
            SetDlgItemInt(hDlg, ID_RED, GetRValue(rgbColor), FALSE);
            SetDlgItemInt(hDlg, ID_GREEN, GetGValue(rgbColor), FALSE);
            SetDlgItemInt(hDlg, ID_BLUE, GetBValue(rgbColor), FALSE);
            return (TRUE);
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    Red = GetDlgItemInt(hDlg, ID_RED, NULL, FALSE);
                    Green = GetDlgItemInt(hDlg, ID_GREEN, NULL, FALSE);
                    Blue = GetDlgItemInt(hDlg, ID_BLUE, NULL, FALSE);
                    rgbColor = RGB(Red, Green, Blue);
                    EndDialog(hDlg, 1);
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    break;
            }
            break;
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: EnumFunc(LPLOGFONT, LPTEXTMETRIC, short, LPSTR)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

int FAR PASCAL EnumFunc(lpLogFont, lpTextMetric, FontType, lpData)
LPLOGFONT lpLogFont;
LPTEXTMETRIC lpTextMetric;
short FontType;
LPSTR lpData;
{
    switch (LOWORD(lpData)) {
        case 0:
            if (FontIndex >= MAXFONT)
                return (0);
            _lstrcpy((LPSTR) FontList[FontIndex],
                (LPSTR) (lpLogFont->lfFaceName));
            CharSet[FontIndex] = lpLogFont->lfCharSet;
            PitchAndFamily[FontIndex] = lpLogFont->lfPitchAndFamily;
            return (++FontIndex);

        case 1:
            if (SizeIndex >= MAXSIZE)
                return (0);
            SizeList[SizeIndex] = lpLogFont->lfHeight;
            return (++SizeIndex);
    }
}

/****************************************************************************

    FUNCTION: GetFonts(HWND)

    PURPOSE: Get available fonts

****************************************************************************/

void GetFonts(hWnd)
HWND hWnd;
{

    HDC hDC;

    FontIndex = 0;
    SizeIndex = 0;
    hDC = GetDC(hWnd);
    lpEnumFunc = MakeProcInstance(EnumFunc, hInst);
    EnumFonts(hDC, (LPSTR) NULL, lpEnumFunc, (LPSTR) NULL);
    FreeProcInstance(lpEnumFunc);
    ReleaseDC(hWnd, hDC);
}

/****************************************************************************

    FUNCTION: GetSizes(hWnd, CurrentFont)

    PURPOSE: Get size of current font

****************************************************************************/

void GetSizes(hWnd, CurrentFont)
HWND hWnd;
int CurrentFont;
{
    HDC hDC;

    SizeIndex = 0;
    hDC = GetDC(hWnd);
    lpEnumFunc = MakeProcInstance(EnumFunc, hInst);
    EnumFonts(hDC, FontList[CurrentFont], lpEnumFunc, (LPSTR) 1L);
    FreeProcInstance(lpEnumFunc);
    ReleaseDC(hWnd, hDC);
}


/****************************************************************************

    FUNCTION: SelectFont(HWND, unsigned, WORD, LONG)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL FAR PASCAL SelectFont(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{

    int i;
    int index;
    char buf[LF_FACESIZE];

    switch (message) {
        case WM_INITDIALOG:
            for (i=0; i<FontIndex; i++) {        /* displays available fonts */
                SendDlgItemMessage(hDlg, ID_TYPEFACE, LB_ADDSTRING,
                    NULL, (LONG) (LPSTR) FontList[i]);
                SendDlgItemMessage(hDlg, ID_TYPEFACE, LB_SETCURSEL,
                    0, 0L);
            }
            GetSizes(hDlg, 0);
            for (i=0; i<SizeIndex; i++) {        /* displays font sizes      */
                sprintf(buf, "%d", SizeList[i]);
                SendDlgItemMessage(hDlg, ID_SIZE, LB_ADDSTRING,
                    0, (LONG) (LPSTR) buf);
                SendDlgItemMessage(hDlg, ID_SIZE, LB_SETCURSEL,
                    0, 0L);
            }
            return (TRUE);
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
okay:
                    index=SendDlgItemMessage(hDlg, ID_TYPEFACE,
                        LB_GETCURSEL, 0, 0L);
                    if (index == LB_ERR) {
                        MessageBox(hDlg, "No font selected",
                            "Select Font", MB_OK | MB_ICONEXCLAMATION);
                    break;
            }
            CurrentFont = index;
            index = SendDlgItemMessage(hDlg, ID_SIZE,
                LB_GETCURSEL, 0, 0L);
            if (index == LB_ERR) {
                MessageBox(hDlg, "No size selected",
                    "Select Font", MB_OK | MB_ICONEXCLAMATION);
                break;
            }
            CurrentSize = index;
            EndDialog(hDlg, 1);
            break;

        case IDCANCEL:
            EndDialog(hDlg, 0);
            break;

        case ID_TYPEFACE:
            switch (HIWORD(lParam)) {
                case LBN_SELCHANGE:
                    index = SendDlgItemMessage(hDlg, ID_TYPEFACE,
                        LB_GETCURSEL, 0, 0L);
                    if (index == LB_ERR)
                        break;
                    SendDlgItemMessage(hDlg, ID_SIZE, LB_RESETCONTENT, 0, 0L);
                    GetSizes(hDlg, index);
                    for (i = 0; i < SizeIndex; i++) {
                        sprintf(buf, "%d", SizeList[i]);
                        SendDlgItemMessage(hDlg, ID_SIZE,
                            LB_ADDSTRING, 0, (LONG) (LPSTR) buf);
                        SendDlgItemMessage(hDlg, ID_SIZE, LB_SETCURSEL, 0, 0L);
            }
            break;

                case LBN_DBLCLK:
                goto okay;
                break;
            }
            break;

        case ID_SIZE:
            if(HIWORD(lParam) == LBN_DBLCLK)
                goto okay;
            break;
        }
        break;
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: ShowFontWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE: Processes messages

****************************************************************************/

long FAR PASCAL ShowFontWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpAddDlg, lpRemoveDlg;
    HDC hDC;
    PAINTSTRUCT ps;
    HFONT hOldFont;
    int i;
    short Y;
    char buf[80];

    switch(message) {
        case WM_CREATE:
            GetFonts(hWnd);
            hMFont = CreateFont(
                10,                                      /* height           */
                10,                                      /* width            */
                0,                                       /* escapement       */
                0,                                       /* orientation      */
                FW_NORMAL,                               /* weight           */
                FALSE,                                   /* italic           */
                FALSE,                                   /* underline        */
                FALSE,                                   /* strikeout        */
                OEM_CHARSET,                             /* charset          */
                OUT_DEFAULT_PRECIS,                      /* out precision    */
                CLIP_DEFAULT_PRECIS,                     /* clip precision   */
                DEFAULT_QUALITY,                         /* quality          */
                FIXED_PITCH | FF_MODERN,                 /* pitch and family */
                "Courier");                              /* typeface         */
            hOFont = GetStockObject(OEM_FIXED_FONT);
            hFFont = GetStockObject(ANSI_FIXED_FONT);
            hVFont = GetStockObject(ANSI_VAR_FONT);
            hSFont = GetStockObject(SYSTEM_FONT);
            hDFont = GetStockObject(DEVICE_DEFAULT_FONT);
            hFont = hSFont;
            GetObject(hFont, sizeof(LOGFONT), (LPSTR) &LogFont);
            strcpy(WindowTitle, AppName);
            strcat(WindowTitle, "SYSTEM");
            SetWindowText(hWnd, (LPSTR) WindowTitle);

            for (i=0; i<64; i++) {
                line[0][i] = i;
                line[1][i] = i+64;
                line[2][i] = i+128;
                line[3][i] = i+192;
            }
            break;

        case WM_PAINT:
            hDC = BeginPaint(hWnd, &ps);
            SetMyDC(hDC);
            switch (wPaint) {
                case IDM_SHOWCHARSET:
                ShowCharacterSet(hDC, hFont);
                break;
            }
            EndPaint(hWnd, &ps);
            break;

        case WM_COMMAND:
            switch (wParam) {

                /* File menu */

                case IDM_ADDFONT:

                    /* Call AddDlg() to get the filename */

                    lpAddDlg = MakeProcInstance((FARPROC) AddDlg, hInst);
                    if (DialogBox(hInst, "Add", hWnd, lpAddDlg)) {

                        /* Check to see if it is a new font name */

                        for (i = 0; i < nFontIndex; i++) {
                            if (!strcmp(FontFileName, &FontNameList[i][0])) {
                                MessageBox(hWnd, "Font already exists",
                                    "Add Font", MB_OK | MB_ICONQUESTION);
                                FreeProcInstance(lpAddDlg);
                                return (0L);
                            }
                        }

                        /* Tell Windows to add the font resource */

                        AddFontResource((LPSTR) FontFileName);

                        /* Let all applications know there is a new font
                         * resource
                         */

                        SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL,
                            (LONG) NULL);

                        /* Copy the name selected to the list of fonts added */

                        strcpy(&FontNameList[nFontIndex++][0], FontFileName);
                    }

                    FreeProcInstance(lpAddDlg);
                    break;

                case IDM_DELFONT:
                    if (!nFontIndex) {
                        MessageBox(hWnd, "No fonts to delete",
                            "Remove Font", MB_OK | MB_ICONQUESTION);
                        break;
                    }

                    lpRemoveDlg = MakeProcInstance((FARPROC) RemoveDlg, hInst);
                    if (DialogBox(hInst, "Remove", hWnd, lpRemoveDlg)) {
                        for (i = 0; i < nFontIndex; i++) {
                            if (!strcmp(FontFileName, &FontNameList[i][0])) {
                                RemoveFontResource((LPSTR) FontFileName);
                                SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL,
                                    (LONG) NULL);
                                strcpy(&FontNameList[i][0],
                                    &FontNameList[--nFontIndex][0]);
                                break;
                            }
                        }
                    }
                    FreeProcInstance(lpRemoveDlg);
                    break;

                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;

                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance((FARPROC) About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                /* Show menu */

                case IDM_SHOWSTRING:
                    ShowString(hWnd);
                    break;

                case IDM_SHOWCHARSET:
                    InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
                    wPaint = wParam;
                    break;

                case IDM_SHOWLOGFONT:
                    ShowLogFont(hWnd, hFont);
                    break;

                case IDM_SHOWTEXTMETRICS:
                    ShowTextMetric(hWnd, hFont);
                    break;

                case IDM_CLEAR:
                    InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
                    wPaint = 0;
                    break;

                /* Font menu */

                case IDM_OEM:
                    hFont = hOFont;
                    SetFaceName(hWnd);                  /* sets window title */
                    CheckMenuItem(GetMenu(hWnd), wPrevFont, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevFont = wParam;
                    break;

                case IDM_ANSIFIXED:
                    hFont = hFFont;
                    SetFaceName(hWnd);
                    CheckMenuItem(GetMenu(hWnd), wPrevFont, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevFont = wParam;
                    break;

                case IDM_ANSIVAR:
                    hFont = hVFont;
                    SetFaceName(hWnd);
                    CheckMenuItem(GetMenu(hWnd), wPrevFont, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevFont = wParam;
                    break;

                case IDM_SYSTEM:
                    hFont = hSFont;
                    SetFaceName(hWnd);
                    CheckMenuItem(GetMenu(hWnd), wPrevFont, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevFont = wParam;
                    break;

                case IDM_DEVICEDEF:
                    hFont = hDFont;
                    SetFaceName(hWnd);
                    CheckMenuItem(GetMenu(hWnd), wPrevFont, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevFont = wParam;
                    break;

                case IDM_SELECTFONT:
                    lpSelectFont = MakeProcInstance(SelectFont, hInst);
                    if (DialogBox(hInst, "SelectFont", hWnd, lpSelectFont)) {
                        DeleteObject(hMFont);
                        hMFont = CreateFont(
                            SizeList[CurrentSize],
                            0,
                            0,
                            0,
                            FW_NORMAL,
                            FALSE,
                            FALSE,
                            FALSE,
                            CharSet[CurrentFont],
                            OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY,
                            PitchAndFamily[CurrentFont],
                            FontList[CurrentFont]);
                        hFont = hMFont;
                        SetFaceName(hWnd);
                    }
                    FreeProcInstance(lpSelectFont);
                    break;

                case IDM_CFONT:
                    lpCFontDlg = MakeProcInstance(CFontDlg, hInst);
                    GetObject(hMFont, sizeof(LOGFONT), (LPSTR) &CLogFont);
                    if (DialogBox(hInst, "CFont", hWnd, lpCFontDlg)) {
                        DeleteObject(hMFont);
                        hMFont = CreateFontIndirect(&CLogFont);
                        hFont = hMFont;
                        SetFaceName(hWnd);
                    }
                    FreeProcInstance(lpCFontDlg);
                    break;

                /* Options menu */

                case IDM_TEXTCOLOR:
                    lpColors = MakeProcInstance(Colors, hInst);
                    rgbColor = rgbTextColor;
                    if (DialogBox(hInst, "Colors", hWnd, lpColors))
                        rgbTextColor = rgbColor;
                    FreeProcInstance(lpColors);
                    break;

                case IDM_BACKGROUNDCOLOR:
                    lpColors = MakeProcInstance(Colors, hInst);
                    rgbColor = rgbBkColor;
                    if (DialogBox(hInst, "Colors", hWnd, lpColors))
                        rgbBkColor = rgbColor;
                    FreeProcInstance(lpColors);
                    break;

                case IDM_OPAQUE:
                    nBkMode = OPAQUE;
                    CheckMenuItem(GetMenu(hWnd), IDM_TRANSPARENT, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), IDM_OPAQUE, MF_CHECKED);
                    break;

                case IDM_TRANSPARENT:
                    nBkMode = TRANSPARENT;
                    CheckMenuItem(GetMenu(hWnd), IDM_OPAQUE,  MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), IDM_TRANSPARENT,  MF_CHECKED);
                    break;

                case IDM_ALIGNLEFT:
                    nAlignLCR = TA_LEFT;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNCENTER:
                    nAlignLCR = TA_CENTER;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNRIGHT:
                    nAlignLCR = TA_RIGHT;
                    CheckMenuItem(GetMenu(hWnd), wPrevHAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevHAlign = wParam;
                    break;

                case IDM_ALIGNTOP:
                    nAlignTBB = TA_TOP;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;

                case IDM_ALIGNBASE:
                    nAlignTBB = TA_BASELINE;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;

                case IDM_ALIGNBOTTOM:
                    nAlignTBB = TA_BOTTOM;
                    CheckMenuItem(GetMenu(hWnd), wPrevVAlign, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), wParam, MF_CHECKED);
                    wPrevVAlign = wParam;
                    break;
            }
            break;

        case WM_LBUTTONUP:
            ptCurrent.x = LOWORD(lParam);
            ptCurrent.y = HIWORD(lParam);
            ShowString(hWnd);
            break;

        case WM_FONTCHANGE:
            GetFonts(hWnd);
            break;

        case WM_DESTROY:

            /* Remove any fonts that were added */

            for (i = 0; i < nFontIndex; i++)
                RemoveFontResource((LPSTR) &FontNameList[i][0]);

            /* Notify any other applications know the fonts have been deleted */

            SendMessage((HWND) 0xFFFF, WM_FONTCHANGE, NULL, (LONG) NULL);
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (0L);
}

/****************************************************************************

    FUNCTION: Metric(HWND, unsigned, WORD, LONG)

    PURPOSE: Modeless dialog box to display metric font information

****************************************************************************/

BOOL FAR PASCAL Metric(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            SetDlgItemInt(hDlg, IDMB_HEIGHT, TextMetric.tmHeight, FALSE);
            SetDlgItemInt(hDlg, IDMB_ASCENT, TextMetric.tmAscent, FALSE);
            SetDlgItemInt(hDlg, IDMB_DESCENT, TextMetric.tmDescent, FALSE);
            SetDlgItemInt(hDlg, IDMB_INTERNALLEADING,
                TextMetric.tmInternalLeading, FALSE);
            SetDlgItemInt(hDlg, IDMB_EXTERNALLEADING,
                TextMetric.tmExternalLeading, FALSE);
            SetDlgItemInt(hDlg, IDMB_AVECHARWIDTH, TextMetric.tmAveCharWidth,
                FALSE);
            SetDlgItemInt(hDlg, IDMB_MAXCHARWIDTH, TextMetric.tmMaxCharWidth,
                FALSE);
            SetDlgItemInt(hDlg, IDMB_WEIGHT, TextMetric.tmWeight, FALSE);
            SetDlgItemInt(hDlg, IDMB_ITALIC, TextMetric.tmItalic, FALSE);
            SetDlgItemInt(hDlg, IDMB_UNDERLINED, TextMetric.tmUnderlined,
                FALSE);
            SetDlgItemInt(hDlg, IDMB_STRUCKOUT, TextMetric.tmStruckOut, FALSE);
            SetDlgItemInt(hDlg, IDMB_FIRSTCHAR, TextMetric.tmFirstChar, FALSE);
            SetDlgItemInt(hDlg, IDMB_LASTCHAR, TextMetric.tmLastChar, FALSE);
            SetDlgItemInt(hDlg, IDMB_DEFAULTCHAR, TextMetric.tmDefaultChar,
                FALSE);
            SetDlgItemInt(hDlg, IDMB_BREAKCHAR, TextMetric.tmBreakChar, FALSE);
            SetDlgItemInt(hDlg, IDMB_PITCHANDFAMILY,
                TextMetric.tmPitchAndFamily, FALSE);
            SetDlgItemInt(hDlg, IDMB_CHARSET, TextMetric.tmCharSet, FALSE);
            SetDlgItemInt(hDlg, IDMB_OVERHANG, TextMetric.tmOverhang, FALSE);
            SetDlgItemInt(hDlg, IDMB_DIGITIZEDASPECTX,
                TextMetric.tmDigitizedAspectX, FALSE);
            SetDlgItemInt(hDlg, IDMB_DIGITIZEDASPECTY,
		TextMetric.tmDigitizedAspectY, FALSE);
            return (TRUE);

        case WM_CLOSE:
            DestroyWindow(hDlg);
            break;
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: Log(HWND, unsigned, WORD, LONG)

    PURPOSE: Displays logical font information

****************************************************************************/

BOOL FAR PASCAL Log(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{

    switch (message) {
        case WM_INITDIALOG:
            SetDlgItemInt(hDlg, IDMI_HEIGHT, LogFont.lfHeight, FALSE);
            SetDlgItemInt(hDlg, IDMI_WIDTH, LogFont.lfWidth, FALSE);
            SetDlgItemInt(hDlg, IDMI_ESCAPEMENT, LogFont.lfEscapement, FALSE);
            SetDlgItemInt(hDlg, IDMI_ORIENTATION, LogFont.lfOrientation, FALSE);
            SetDlgItemInt(hDlg, IDMI_WEIGHT, LogFont.lfWeight, FALSE);
            SetDlgItemInt(hDlg, IDMI_ITALIC, LogFont.lfItalic, FALSE);
            SetDlgItemInt(hDlg, IDMI_UNDERLINED, LogFont.lfUnderline, FALSE);
            SetDlgItemInt(hDlg, IDMI_STRIKEOUT, LogFont.lfStrikeOut, FALSE);
            SetDlgItemInt(hDlg, IDMI_CHARSET, LogFont.lfCharSet, FALSE);
            SetDlgItemInt(hDlg, IDMI_OUTPRECISION, LogFont.lfOutPrecision,
                FALSE);
            SetDlgItemInt(hDlg, IDMI_CLIPPRECISION, LogFont.lfClipPrecision,
                FALSE);
            SetDlgItemInt(hDlg, IDMI_QUALITY, LogFont.lfQuality, FALSE);
            SetDlgItemInt(hDlg, IDMI_PITCHANDFAMILY,
                LogFont.lfPitchAndFamily, FALSE);
            return (TRUE);

        case WM_CLOSE:
            DestroyWindow(hDlg);
            break;

    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: AddDlg(HWND, unsigned, WORD, LONG)

    PURPOSE: Used to add a font

    COMMENTS:

        This dialog box displays all the availble font files on the currently
        selected directory, and lets the user select a font to add.

****************************************************************************/

BOOL FAR PASCAL AddDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_COMMAND:
            switch (wParam) {
                case ID_LISTBOX:

                    switch (HIWORD(lParam)) {
                        case LBN_SELCHANGE:
                            /* If item is a directory name, append "*.*" */
                            if (DlgDirSelect(hDlg, str, ID_LISTBOX)) 
                                strcat(str, DefSpec);

                            SetDlgItemText(hDlg, ID_EDIT, str);
                            SendDlgItemMessage(hDlg,
                                ID_EDIT,
                                EM_SETSEL,
                                NULL,
                                MAKELONG(0, 0x7fff));
                            break;

                        case LBN_DBLCLK:
                            goto CheckSelection;
                            break;

                    }                              /* end of ID_LISTBOX case */
                    return (TRUE);

                case IDOK:
CheckSelection:
                    /* Get the filename from the edit control */

                    GetDlgItemText(hDlg, ID_EDIT, str, 128);
                    GetDlgItemText(hDlg, ID_PATH, DefPath, 128);

                    /* Check for wildcard.  If found, use the string as a new
                     * search path.
                     */

                    if (strchr(str, '*') ||
                        strchr(str, '?')) {

                    /* Separate filename from path.  The path is stored in
                     * str which is discarded if null, else it is used for a new
                     * search path.
                     */

                        SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
                            (LPSTR) str);
                        if (str[0])
                            strcpy(DefPath, str);

                        UpdateListBox(hDlg);
                        return (TRUE);
                    }

                    /* Ignore it if no filename specified */

                    if (!str[0]) {
                        MessageBox(hDlg, "No filename specified.",
                            NULL, MB_OK | MB_ICONQUESTION);
                        return (TRUE);
                    }

                    /* Append the default extension if needed */

                    strcpy(FontFileName, DefPath);
                    strcat(FontFileName, str);
                    AddExt(FontFileName, DefExt);
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:

                    /* Let the caller know the user cancelled */

                    EndDialog(hDlg, FALSE);
                    return (TRUE);
            }
            break;

        case WM_INITDIALOG:
            SetWindowText(hDlg, (LPSTR) "Add Font Resource");
            strcpy(DefSpec, "*.fon");
            UpdateListBox(hDlg);
            SetDlgItemText(hDlg, ID_EDIT, DefSpec);
            SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL, NULL,
                MAKELONG(0, 0x7fff));
            SetFocus(GetDlgItem(hDlg, ID_EDIT));
            return (FALSE);
    }
    return FALSE;
}

/****************************************************************************

    FUNCTION: RemoveDlg(HANDLE)

    PURPOSE: Used to remove a font

    COMMENTS:

        This dialog box displays all fonts which have been added to the system,
        and lets the user select which font to delete.

****************************************************************************/

BOOL FAR PASCAL RemoveDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    WORD index;
    int i;

    switch (message) {
        case WM_COMMAND:

            switch (wParam) {
                case ID_LISTBOX:

                    switch (HIWORD(lParam)) {
                        case LBN_SELCHANGE:
                            index = SendDlgItemMessage(hDlg,
                                ID_LISTBOX,
                                LB_GETCURSEL,         /* get index command   */
                                NULL,
                                (LONG) NULL);
                            SendDlgItemMessage(hDlg,
                                ID_LISTBOX,
                                LB_GETTEXT,           /* copy string command */
                                index,
                                (LONG) (LPSTR) FontFileName);
                            SetDlgItemText(hDlg, ID_EDIT, FontFileName);
                            break;

                        case LBN_DBLCLK:
                            GetDlgItemText(hDlg, ID_EDIT, FontFileName, 128);
                            EndDialog(hDlg, TRUE);
                            return (TRUE);
                    }
                    return (TRUE);

                case IDOK:

                    /* Get the filename from the edit control */

                    GetDlgItemText(hDlg, ID_EDIT, FontFileName, 128);

                    /* Ignore it if no filename specified */

                    if (!FontFileName[0]) {
                        MessageBox(hDlg, "No filename specified.",
                            NULL, MB_OK | MB_ICONQUESTION);
                        return (TRUE);
                    }

                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return (TRUE);
            }
            break;

        case WM_INITDIALOG:
            SetWindowText(hDlg, (LPSTR) "Remove Font Resource");
            for (i = 0; i < nFontIndex; i++)
                SendDlgItemMessage(hDlg,
                    ID_LISTBOX,
                    LB_ADDSTRING,
                    NULL,
                    (LONG)(char far *) &FontNameList[i][0]);

            SetFocus(GetDlgItem(hDlg, ID_EDIT));
            return (FALSE);
    }
    return FALSE;
}

/****************************************************************************

    FUNCTION: UpdateListBox(HWND);

    PURPOSE: Update the list box of OpenDlg

****************************************************************************/

void UpdateListBox(hDlg)
HWND hDlg;
{
    strcpy(str, DefPath);
    strcat(str, DefSpec);
    DlgDirList(hDlg, str, ID_LISTBOX, ID_PATH, 0x4010);
    SetDlgItemText(hDlg, ID_EDIT, DefSpec);
}

/****************************************************************************

    FUNCTION: AddExt(PSTR, PSTR);

    PURPOSE: Add default extension

/***************************************************************************/

void AddExt(Name, Ext)
PSTR Name, Ext;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
        pTptr++;
    if (*pTptr != '.')
        strcat(Name, Ext);
}

/****************************************************************************

    FUNCTION: SeparateFile(HWND, LPSTR, LPSTR, LPSTR)

    PURPOSE: Separate filename and pathname

    COMMENTS:

        This function takes a source filespec and splits it into a path and a
        filename, and copies these into the strings specified.  Because it
        uses the AnsiPrev call, it will work in any language.

****************************************************************************/

void SeparateFile(hDlg, lpDestPath, lpDestFileName, lpSrcFileName)
HWND hDlg;
LPSTR lpDestPath, lpDestFileName, lpSrcFileName;
{
    LPSTR lpTmp;

    lpTmp = lpSrcFileName + (long) _lstrlen(lpSrcFileName);
    while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
        lpTmp = AnsiPrev(lpSrcFileName, lpTmp);
    if (*lpTmp != ':' && *lpTmp != '\\') {                  /* no path given */
        _lstrcpy(lpDestFileName, lpSrcFileName);
        lpDestPath[0] = 0;
        return;
    }
    _lstrcpy(lpDestFileName, lpTmp + 1L);
    _lstrncpy(lpDestPath, lpSrcFileName, (int) (lpTmp - lpSrcFileName) + 1);
    lpDestPath[(lpTmp - lpSrcFileName) + 1] = 0;
}

/****************************************************************************

    FUNCTION: _lstrlen(LPSTR)

    PURPOSE:  uses a long far pointer to the string, returns the length

****************************************************************************/

int _lstrlen(lpStr)
LPSTR lpStr;
{
    int i;
    for (i=0; *lpStr++; i++);
    return (i);
}

/****************************************************************************

    FUNCTION: _lstrncpy(LPSTR, LPSTR)

    PURPOSE:  FAR version of strncpy()

****************************************************************************/

void _lstrncpy(lpDest, lpSrc, n)
LPSTR lpDest, lpSrc;
int n;
{
    while (n--)
        if(!(*lpDest++ = *lpSrc++))
            return;
}

/****************************************************************************

    FUNCTION: _lstrcpy(LPSTR, LPSTR)

    PURPOSE:  FAR version of strcpy()

****************************************************************************/

void _lstrcpy(lpDest, lpSrc)
LPSTR lpDest, lpSrc;
{
    while(*lpDest++ = *lpSrc++);
}

/****************************************************************************

    FUNCTION: SetFaceName(HWND)

    PURPOSE: Retireves current font's face name, places it in WindowTitle

****************************************************************************/

SetFaceName(hWnd)
HWND hWnd;
{
    char buf[80];
    HDC hDC;

    hDC = GetDC(hWnd);
    SelectObject(hDC, hFont);
    strcpy(WindowTitle, AppName);
    GetTextFace(hDC, 80, buf);
    strcat(WindowTitle, buf);
    SetWindowText(hWnd, (LPSTR) WindowTitle);

    ReleaseDC(hWnd, hDC);
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
            return (TRUE);
    }
    return (FALSE);
}

