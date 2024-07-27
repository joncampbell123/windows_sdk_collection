/****************************************************************************

    PROGRAM: EditFile.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box
        SaveAsDlg() - save file under different name
        OpenDlg() - let user select a file, and open it.
        UpdateListBox() - Update the list box of OpenDlg
        ChangeDefExt() - Change the default extension
        SeparateFile() - Separate filename and pathname
        AddExt() - Add default extension
        CheckFileName() - Check for wildcards, add extension if needed
        SaveFile() - Save current file
        QuerySaveFile() - Called when some action might lose current contents
        SetNewBuffer() - Set new buffer for edit window

****************************************************************************/

#include "windows.h"
#include "editfile.h"

HANDLE hInst;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hEditWnd;                                      /* handle to edit window */
/* Additional includes needed for the fstat() function */

#include <sys\types.h>
#include <sys\stat.h>

char FileName[128];
char PathName[128];
char OpenName[128];
char DefPath[128];
char DefSpec[13] = "*.*";
char DefExt[] = ".txt";
char str[255];

HANDLE hEditBuffer;                       /* handle to editing buffer      */
HANDLE hOldBuffer;                        /* old buffer handle        */
HANDLE hHourGlass;                        /* handle to hourglass cursor      */
HANDLE hSaveCursor;                       /* current cursor handle      */
int hFile;                                /* file handle              */
int count;                                /* number of chars read or written */
PSTR pBuffer;                             /* address of read/write buffer    */
OFSTRUCT OfStruct;                        /* information from OpenFile()     */
struct stat FileStatus;                   /* information from fstat()      */
BOOL bChanges = FALSE;                    /* TRUE if the file is changed     */
BOOL bSaveEnabled = FALSE;                /* TRUE if text in the edit buffer */
PSTR pEditBuffer;                         /* address of the edit buffer      */
RECT Rect;                                /* dimension of the client window  */
HWND hwnd;                                /* handle to main window           */


char Untitled[] =                         /* default window title      */
     "Edit File - (untitled)";

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
    wc.lpszMenuName =  "EditFileMenu";
    wc.lpszClassName = "EditFileWClass";

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

    hAccTable = LoadAccelerators(hInst, "EditFileAcc");

    hwnd = CreateWindow(
        "EditFileWClass",
        "EditFile Sample Application",
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

    /* Get an hourglass cursor to use during file transfers */

    hHourGlass = LoadCursor(NULL, IDC_WAIT);
    
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
        WM_SIZE       - window size has changed
        WM_QUERYENDSESSION - willing to end session?
        WM_ENDSESSION - end Windows session
        WM_CLOSE      - close the window
        WM_SIZE       - window resized

    COMMENTS:

        WM_COMMAND processing:

            IDM_NEW - query to save current file if there is one and it has
                      been changed, clear buffer and start new file.

            IDM_OPEN - query to save current file if there is one and it
                       has been changed, open a new file.

            IDM_SAVE - save current file, prompt for name if none exists.

            IDM_SAVEAS - prompt for new filename to save to, save file.

            IDC_EDIT - change "bChanges" flag to indicate if edit buffer has
                      been modified.  Affects actions of IDM_NEW and
                      IDM_OPEN.  Reset when file is saved.

            IDM_EXIT - query to save current file if there is one and it
                       has been changed, then exit.

            IDM_ABOUT - display "About" box.

        When more then one string needs to be sent to a message box,
        sprintf() is used to combine the strings into str[], and then str[]
        is passed to the MessageBox() function.  A message box string cannot
        exceed 255 characters, but may contain \n to generate separate
        lines.

        After the size of the file is determined, only enough memory to store
        the file is allocated for the Edit buffer.  The edit control will
        automatically expand this memory as needed.  Once the file has been
        read into the edit buffer, unlock the memory.  Use whatever was
        obtained from the read() function, even if an error occured.  This
        allows partial salvage of a file with a bad sector.

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


    switch (message) {
        case WM_COMMAND:
            switch (wParam) {
                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                case IDM_NEW:

                    /* If current file has been modified, query user about
                     * saving it.
                     */

                    if (!QuerySaveFile(hWnd))
                        return (NULL);

                    /* bChanges is set to FALSE to indicate there have been
                     * no changes since the last file save.
                     */

                    bChanges = FALSE;
                    FileName[0] = 0;

                    /* Update the edit buffer */

                    SetNewBuffer(hWnd, NULL, Untitled);
                    break;

                case IDM_OPEN:
                    if (!QuerySaveFile(hWnd))
                        return (NULL);

                    lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);

                    /* Open the file and get its handle */

                    hFile = DialogBox(hInst, "Open", hWnd, lpOpenDlg);
                    FreeProcInstance(lpOpenDlg);
                    if (!hFile)
                        return (NULL);

                    /* Allocate edit buffer to the size of the file + 1 */

                    hEditBuffer =
                        LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT,
			    (WORD)(FileStatus.st_size+1));

                    if (!hEditBuffer) {
                        MessageBox(hWnd, "Not enough memory.",
                            NULL, MB_OK | MB_ICONHAND);
                        return (NULL);
                    }
                    hSaveCursor = SetCursor(hHourGlass);
                    pEditBuffer = LocalLock(hEditBuffer);

                    IOStatus = read(hFile, pEditBuffer, FileStatus.st_size);
                    close(hFile);

                    /* # bytes read must equal file size */

                    if (IOStatus != FileStatus.st_size) {

                        sprintf(str, "Error reading %s.", FileName);
                        SetCursor(hSaveCursor);      /* Remove the hourglass */
                        MessageBox(hWnd, str, NULL, MB_OK | MB_ICONEXCLAMATION);
                    }

                    LocalUnlock(hEditBuffer);

                    /* Set up a new buffer and window title */

                    sprintf(str, "EditFile - %s", FileName);
                    SetNewBuffer(hWnd, hEditBuffer, str);
                    SetCursor(hSaveCursor);            /* restore the cursor */
                    break;

                case IDM_SAVE:

                    /* If there is no filename, use the saveas command to get
                     * one.  Otherwise, save the file using the current
                     * filename.
                     */

                    if (!FileName[0])
                        goto saveas;
                    if (bChanges)
                        SaveFile(hWnd);
                    break;

                case IDM_SAVEAS:
saveas:
                    lpSaveAsDlg = MakeProcInstance(SaveAsDlg, hInst);

                    /* Call the SaveAsDlg() function to get the new filename */

                    Success = DialogBox(hInst, "SaveAs", hWnd, lpSaveAsDlg);
                    FreeProcInstance(lpSaveAsDlg);

                    /* If successful, update the window title, save the file */

                    if (Success == IDOK) {
                        sprintf(str, "EditFile - %s", FileName);
                        SetWindowText(hWnd, str);
                        SaveFile(hWnd);
                    }
                    break;                                  /* User canceled */
                case IDM_PRINT:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "EditFile Sample Application",
                          MB_ICONASTERISK | MB_OK);
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
                          "EditFile Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDM_EXIT:
                    QuerySaveFile(hWnd);
                    DestroyWindow(hWnd);
                    break;

                case IDC_EDIT:
                    if (HIWORD (lParam) == EN_ERRSPACE) {
                        MessageBox (
                              GetFocus ()
                            , "Out of memory."
                            , "EditFile Sample Application"
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

        case WM_QUERYENDSESSION:             /* message: to end the session? */
            return (QuerySaveFile(hWnd));

        case WM_CLOSE:                       /* message: close the window    */
            if (QuerySaveFile(hWnd))
                DestroyWindow(hWnd);
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

    FUNCTION: SaveAsDlg(HWND, unsigned, WORD, LONG)

    PURPOSE: Allows user to change name to save file to

    COMMENTS:

        This will initialize the window class if it is the first time this
        application is run.  It then creates the window, and processes the
        message loop until a PostQuitMessage is received.  It exits the
        application by returning the value passed by the PostQuitMessage.

****************************************************************************/

int FAR PASCAL SaveAsDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    char TempName[128];

    switch (message) {
        case WM_INITDIALOG:

            /* If no filename is entered, don't allow the user to save to it */

            if (!FileName[0])
                bSaveEnabled = FALSE;
            else {
                bSaveEnabled = TRUE;

                /* Process the path to fit within the IDC_PATH field */

                DlgDirList(hDlg, DefPath, NULL, IDC_PATH, 0x4010);

                /* Send the current filename to the edit control */

                SetDlgItemText(hDlg, IDC_EDIT, FileName);

                /* Accept all characters in the edit control */

                SendDlgItemMessage(hDlg, IDC_EDIT, EM_SETSEL, 0,
                    MAKELONG(0, 0x7fff));
            }

            /* Enable or disable the save control depending on whether the
             * filename exists.
             */

            EnableWindow(GetDlgItem(hDlg, IDOK), bSaveEnabled);

            /* Set the focus to the edit control within the dialog box */

            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE);                 /* FALSE since Focus was changed */

        case WM_COMMAND:
            switch (wParam) {
                case IDC_EDIT:

                    /* If there was previously no filename in the edit
                     * control, then the save control must be enabled as soon as
                     * a character is entered.
                     */

                    if (HIWORD(lParam) == EN_CHANGE && !bSaveEnabled)
                    EnableWindow(GetDlgItem(hDlg, IDOK), bSaveEnabled = TRUE);
                    return (TRUE);

                case IDOK:

                   /* Get the filename from the edit control */

                    GetDlgItemText(hDlg, IDC_EDIT, TempName, 128);

                    /* If there are no wildcards, then separate the name into
                     * path and name.  If a path was specified, replace the
                     * default path with the new path.
                     */

                    if (CheckFileName(hDlg, FileName, TempName)) {
                        SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
                            (LPSTR) FileName);
                        if (str[0])
                            strcpy(DefPath, str);

                        /* Tell the caller a filename was selected */

                        EndDialog(hDlg, IDOK);
                    }
                    return (TRUE);

                case IDCANCEL:

                    /* Tell the caller the user canceled the SaveAs function */

                    EndDialog(hDlg, IDCANCEL);
                    return (TRUE);
            }
            break;

    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: OpenDlg(HWND, unsigned, WORD, LONG)

    PURPOSE: Let user select a file, and open it.

****************************************************************************/

HANDLE FAR PASCAL OpenDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    WORD index;
    PSTR pTptr;
    HANDLE hFile;

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

                    /* Open the file */

                    if ((hFile = OpenFile(OpenName, (LPOFSTRUCT) &OfStruct,
                            OF_READ)) == -1) {
                        sprintf(str, "Error %d opening %s.",
                            OfStruct.nErrCode, OpenName);
                        MessageBox(hDlg, str, NULL,
                            MB_OK | MB_ICONHAND);
                    }
                    else {

                        /* Make sure there's enough room for the file */

                        fstat(hFile, &FileStatus);
                        if (FileStatus.st_size > MAXFILESIZE) {
                            sprintf(str,
                    "Not enough memory to load %s.\n%s exceeds %ld bytes.",
                                OpenName, OpenName, MAXFILESIZE);
                            MessageBox(hDlg, str, NULL,
                                MB_OK | MB_ICONHAND);
                            return (TRUE);
                        }

                        /* File is opened and there is enough room so return
                         * the handle to the caller.
                         */

                        strcpy(FileName, OpenName);
                        EndDialog(hDlg, hFile);
                        return (TRUE);
                    }
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, NULL);
                    return (TRUE);
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

    FUNCTION: CheckFileName(HWND, PSTR, PSTR)

    PURPOSE: Check for wildcards, add extension if needed

    COMMENTS:

        Make sure you have a filename and that it does not contain any
        wildcards.  If needed, add the default extension.  This function is
        called whenever your application wants to save a file.

****************************************************************************/

BOOL CheckFileName(hWnd, pDest, pSrc)
HWND hWnd;
PSTR pDest, pSrc;
{
    PSTR pTmp;

    if (!pSrc[0])
        return (FALSE);               /* Indicates no filename was specified */

    pTmp = pSrc;
    while (*pTmp) {                     /* Searches the string for wildcards */
        switch (*pTmp++) {
            case '*':
            case '?':
                MessageBox(hWnd, "Wildcards not allowed.",
                    NULL, MB_OK | MB_ICONEXCLAMATION);
                return (FALSE);
        }
    }

    AddExt(pSrc, DefExt);            /* Adds the default extension if needed */

    if (OpenFile(pSrc, (LPOFSTRUCT) &OfStruct, OF_EXIST) >= 0) {
        sprintf(str, "Replace existing %s?", pSrc);
        if (MessageBox(hWnd, str, "EditFile",
                MB_OKCANCEL | MB_ICONHAND) == IDCANCEL)
            return (FALSE);
    }
    strcpy(pDest, pSrc);
    return (TRUE);
}

/****************************************************************************

    FUNCTION: SaveFile(HWND)

    PURPOSE: Save current file

    COMMENTS:

        This saves the current contents of the Edit buffer, and changes
        bChanges to indicate that the buffer has not been changed since the
        last save.

        Before the edit buffer is sent, you must get its handle and lock it
        to get its address.  Once the file is written, you must unlock the
        buffer.  This allows Windows to move the buffer when not in immediate
        use.

****************************************************************************/

BOOL SaveFile(hWnd)
HWND hWnd;
{
    BOOL bSuccess;
    int IOStatus;                                  /* result of a file write */

    if ((hFile = OpenFile(FileName, &OfStruct,
        OF_PROMPT | OF_CANCEL | OF_CREATE)) < 0) {

        /* If the file can't be saved */

        sprintf(str, "Cannot write to %s.", FileName);
        MessageBox(hWnd, str, NULL, MB_OK | MB_ICONEXCLAMATION);
        return (FALSE);
    }


    hEditBuffer = SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    pEditBuffer = LocalLock(hEditBuffer);

    /* Set the cursor to an hourglass during the file transfer */

    hSaveCursor = SetCursor(hHourGlass);
    IOStatus = write(hFile, pEditBuffer, strlen(pEditBuffer));
    close(hFile);
    SetCursor(hSaveCursor);
    if (IOStatus != strlen(pEditBuffer)) {
        sprintf(str, "Error writing to %s.", FileName);
        MessageBox(hWnd, str,
            NULL, MB_OK | MB_ICONHAND);
        bSuccess = FALSE;
    }
    else {
        bSuccess = TRUE;                /* Indicates the file was saved      */
        bChanges = FALSE;               /* Indicates changes have been saved */
    }

    LocalUnlock(hEditBuffer);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: QuerySaveFile(HWND);

    PURPOSE: Called when some action might lose current contents

    COMMENTS:

        This function is called whenever we are about to take an action that
        would lose the current contents of the edit buffer.

****************************************************************************/

BOOL QuerySaveFile(hWnd)
HWND hWnd;
{
    int Response;
    FARPROC lpSaveAsDlg;

    if (bChanges) {
        sprintf(str, "Save current changes: %s", FileName);
        Response = MessageBox(hWnd, str,
            "EditFile",  MB_YESNOCANCEL | MB_ICONEXCLAMATION);
        if (Response == IDYES) {
check_name:

            /* Make sure there is a filename to save to */

            if (!FileName[0]) {
                lpSaveAsDlg = MakeProcInstance(SaveAsDlg, hInst);
                Response = DialogBox(hInst, "SaveAs",
                    hWnd, lpSaveAsDlg);
                FreeProcInstance(lpSaveAsDlg);
                if (Response == IDOK)
                    goto check_name;
                else
                    return (FALSE);
            }
            SaveFile(hWnd);
        }
        else if (Response == IDCANCEL)
            return (FALSE);
    }
    else
        return (TRUE);
}

/****************************************************************************

    FUNCTION: SetNewBuffer(HWND, HANDLE, PSTR)

    PURPOSE: Set new buffer for edit window

    COMMENTS:

        Point the edit window to the new buffer, update the window title, and
        redraw the edit window.  If hNewBuffer is NULL, then create an empty
        1K buffer, and return its handle.

****************************************************************************/

void SetNewBuffer(hWnd, hNewBuffer, Title)
HWND hWnd;
HANDLE hNewBuffer;
PSTR Title;
{
    HANDLE hOldBuffer;

    hOldBuffer = SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    LocalFree(hOldBuffer);
    if (!hNewBuffer)                    /* Allocates a buffer if none exists */
        hNewBuffer = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 1);

    /* Updates the buffer and displays new buffer */
    SendMessage(hEditWnd, EM_SETHANDLE, hNewBuffer, 0L);

    SetWindowText(hWnd, Title);
    SetFocus(hEditWnd);
    bChanges = FALSE;
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
