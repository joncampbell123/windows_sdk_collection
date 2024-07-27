/****************************************************************************

    PROGRAM: FileOpen.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box
        OpenDlg() - let user select a file, and open it.
        UpdateListBox() - Update the list box of OpenDlg
        ChangeDefExt() - Change the default extension
        SeparateFile() - Separate filename and pathname
        AddExt() - Add default extension

****************************************************************************/

#include "windows.h"
#include "fileopen.h"

HANDLE hInst;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hEditWnd;                                      /* handle to edit window */
HWND hwnd;                                      /* handle to main window */

char FileName[128];
char PathName[128];
char OpenName[128];
char DefPath[128];
char DefSpec[13] = "*.*";
char DefExt[] = ".txt";
char str[255];

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
    MSG msg;

    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)) {

    /* Only translate message if it is not an accelerator message */

        if (!TranslateAccelerator(hwnd, hAccTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }
    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "FileOpenMenu";
    wc.lpszClassName = "FileOpenWClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;
    int             nCmdShow;
{
    RECT            Rect;

    hInst = hInstance;

    hAccTable = LoadAccelerators(hInst, "FileOpenAcc");

    hwnd = CreateWindow(
        "FileOpenWClass",
        "FileOpen Sample Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        return (FALSE);

    GetClientRect(hwnd, (LPRECT) &Rect);

    /* Create a child window */

    hEditWnd = CreateWindow("Edit",
        NULL,
        WS_CHILD | WS_VISIBLE |
        ES_MULTILINE |
        WS_VSCROLL | WS_HSCROLL |
        ES_AUTOHSCROLL | ES_AUTOVSCROLL,
        0,
        0,
        (Rect.right-Rect.left),
        (Rect.bottom-Rect.top),
        hwnd,
        IDC_EDIT,                          /* Child control i.d. */
        hInst,
        NULL);

    if (!hEditWnd) {
        DestroyWindow(hwnd);
        return (NULL);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_DESTROY    - destroy window

    COMMENTS:

        WM_COMMAND processing:

            IDM_OPEN - query to save current file if there is one and it
                       has been changed, open a new file.

            IDM_ABOUT - display "About" box.

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpOpenDlg, lpSaveAsDlg;

    int Success;                            /* return value from SaveAsDlg() */
    int IOStatus;                           /* result of file i/o      */
    int Return;

    switch (message) {
        case WM_COMMAND:
            switch (wParam) {
                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                case IDM_OPEN:
                    /* Call OpenDlg() to get the filename */

                    lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);
                    Return = DialogBox(hInst, "Open", hWnd, lpOpenDlg);
                    FreeProcInstance(lpOpenDlg);

                    /* Let user know that "open" functionality is not yet
                       implemented */
                    if (Return)
                       MessageBox (
                             GetFocus(),
                             "Command not implemented",
                             "FileOpen Sample Application",
                             MB_ICONASTERISK | MB_OK);
                    break;

                case IDM_NEW:
                case IDM_SAVE:
                case IDM_SAVEAS:
                case IDM_PRINT:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "FileOpen Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
    
                /* edit menu commands */

                case IDM_UNDO:
                case IDM_CUT:
                case IDM_COPY:
                case IDM_PASTE:
                case IDM_CLEAR:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "FileOpen Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDC_EDIT:
                    if (HIWORD (lParam) == EN_ERRSPACE) {
                        MessageBox (
                              GetFocus ()
                            , "Out of memory."
                            , "FileOpen Sample Application"
                            , MB_ICONHAND | MB_OK
                        );
                    }
                    break;

            } 
            break;

        case WM_SETFOCUS:
            SetFocus (hEditWnd);
            break;

        case WM_SIZE:
            MoveWindow(hEditWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/****************************************************************************

    FUNCTION: OpenDlg(HWND, unsigned, WORD, LONG)

    PURPOSE: Let user select a file, and return.  Open code not provided.

****************************************************************************/

HANDLE FAR PASCAL OpenDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    WORD index;
    PSTR pTptr;
    HANDLE hFile=1;     /* Temp value for return */

    switch (message) {
        case WM_COMMAND:
            switch (wParam) {

                case IDC_LISTBOX:
                    switch (HIWORD(lParam)) {

                        case LBN_SELCHANGE:
                            /* If item is a directory name, append "*.*" */
                            if (DlgDirSelect(hDlg, str, IDC_LISTBOX)) 
                                strcat(str, DefSpec);

                            SetDlgItemText(hDlg, IDC_EDIT, str);
                            SendDlgItemMessage(hDlg,
                                IDC_EDIT,
                                EM_SETSEL,
                                NULL,
                                MAKELONG(0, 0x7fff));
                            break;

                        case LBN_DBLCLK:
                            goto openfile;
                    }
                    return (TRUE);

                case IDOK:
openfile:
                    GetDlgItemText(hDlg, IDC_EDIT, OpenName, 128);
                    if (strchr(OpenName, '*') || strchr(OpenName, '?')) {
                        SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
                            (LPSTR) OpenName);
                        if (str[0])
                            strcpy(DefPath, str);
                        ChangeDefExt(DefExt, DefSpec);
                        UpdateListBox(hDlg);
                        return (TRUE);
                    }

                    if (!OpenName[0]) {
                        MessageBox(hDlg, "No filename specified.",
                            NULL, MB_OK | MB_ICONHAND);
                        return (TRUE);
                    }

                    AddExt(OpenName, DefExt);

                    /* The routine to open the file would go here, and the */
                    /* file handle would be returned instead of NULL.           */
                    EndDialog(hDlg, hFile);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, NULL);
                    return (FALSE);
            }
            break;

        case WM_INITDIALOG:                        /* message: initialize    */
            UpdateListBox(hDlg);
            SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
            SendDlgItemMessage(hDlg,               /* dialog handle      */
                IDC_EDIT,                          /* where to send message  */
                EM_SETSEL,                         /* select characters      */
                NULL,                              /* additional information */
                MAKELONG(0, 0x7fff));              /* entire contents      */
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE); /* Indicates the focus is set to a control */
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
    DlgDirList(hDlg, str, IDC_LISTBOX, IDC_PATH, 0x4010);

    /* To ensure that the listing is made for a subdir. of
     * current drive dir...
     */
    if (!strchr (DefPath, ':'))
	DlgDirList(hDlg, DefSpec, IDC_LISTBOX, IDC_PATH, 0x4010);

    /* Remove the '..' character from path if it exists, since this
     * will make DlgDirList move us up an additional level in the tree
     * when UpdateListBox() is called again.
     */
    if (strstr (DefPath, ".."))
	DefPath[0] = '\0';

    SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
}

/****************************************************************************

    FUNCTION: ChangeDefExt(PSTR, PSTR);

    PURPOSE: Change the default extension

****************************************************************************/

void ChangeDefExt(Ext, Name)
PSTR Ext, Name;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
        pTptr++;
    if (*pTptr)
        if (!strchr(pTptr, '*') && !strchr(pTptr, '?'))
            strcpy(Ext, pTptr);
}

/****************************************************************************

    FUNCTION: SeparateFile(HWND, LPSTR, LPSTR, LPSTR)

    PURPOSE: Separate filename and pathname

****************************************************************************/

void SeparateFile(hDlg, lpDestPath, lpDestFileName, lpSrcFileName)
HWND hDlg;
LPSTR lpDestPath, lpDestFileName, lpSrcFileName;
{
    LPSTR lpTmp;
    char  cTmp;

    lpTmp = lpSrcFileName + (long) lstrlen(lpSrcFileName);
    while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
        lpTmp = AnsiPrev(lpSrcFileName, lpTmp);
    if (*lpTmp != ':' && *lpTmp != '\\') {
        lstrcpy(lpDestFileName, lpSrcFileName);
        lpDestPath[0] = 0;
        return;
    }
    lstrcpy(lpDestFileName, lpTmp + 1);
    cTmp = *(lpTmp + 1);
    lstrcpy(lpDestPath, lpSrcFileName);
     *(lpTmp + 1) = cTmp;
    lpDestPath[(lpTmp - lpSrcFileName) + 1] = 0;
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
	    if (wParam == IDOK
                || wParam == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}

