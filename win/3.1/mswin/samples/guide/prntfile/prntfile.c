/****************************************************************************

    PROGRAM: PrntFile.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box
        SaveFile() - Save current file
        QuerySaveFile() - Called when some action might lose current contents
        SetNewBuffer() - Set new buffer for edit window

****************************************************************************/

#include "windows.h"
#include "drivinit.h"
#include "prntfile.h"
#include "commdlg.h"
#include "io.h"
#include "stdio.h"
#include "string.h"

HANDLE hInst;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hEditWnd;                                      /* handle to edit window */
HWND hwnd;                                       /* handle to main window */

/* Additional includes needed for the fstat() function */

#include <sys\types.h>
#include <sys\stat.h>

/* new variables for common dialogs */
OPENFILENAME ofn;
char szFilterSpec [128] =                       /* file type filters */
             "Text Files (*.TXT)\0*.TXT\0All Files (*.*)\0*.*\0";

char szFileName[MAXFILENAME];
char szFileTitle[MAXFILENAME];

char szBaseWindowTitle[] = "PrntOpen Sample Application";
char szWindowTitle[80];

PRINTDLG pd;                              /* Common print dialog structure */

char szTemp[128];
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

char Untitled[] =                         /* default window title      */
     "Edit File - (untitled)";

/* Printer variables  */

HDC hPr;                            /* handle for printer device context     */
int LineSpace;                      /* spacing between lines          */
int LinesPerPage;                   /* lines per page                 */
int CurrentLine;                    /* current line                   */
int LineLength;                     /* line length                    */
WORD wLines;                        /* number of lines to print       */
WORD wIndex;                        /* index into lines to print      */
char szLine[128];                   /* buffer to store lines before printing */
TEXTMETRIC TextMetric;              /* information about character size      */
BOOL bAbort;                        /* FALSE if user cancels printing      */
HWND hAbortDlgWnd;
FARPROC lpAbortDlg, lpAbortProc;

HDC GetPrinterDC(void);

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
    wc.hbrBackground = COLOR_WINDOW+1;
    wc.lpszMenuName =  "PrntFileMenu";
    wc.lpszClassName = "PrntFileWClass";

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

    hAccTable = LoadAccelerators(hInst, "PrntFileAcc");

    hwnd = CreateWindow(
        "PrntFileWClass",
        szBaseWindowTitle,
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
    
    /* fill in non-variant fields of OPENFILENAME struct. */
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner	  = hwnd;
    ofn.lpstrFilter	  = szFilterSpec;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter	  = 0;
    ofn.nFilterIndex	  = 1;
    ofn.lpstrFile         = szFileName;
    ofn.nMaxFile	  = MAXFILENAME;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = MAXFILENAME;
    ofn.lpstrTitle        = NULL;
    ofn.lpstrDefExt       = "TXT";
    ofn.Flags             = 0;

    /* fill in non-variant fields of PRINTDLG struct. */

    pd.lStructSize    = sizeof(PRINTDLG);
    pd.hwndOwner      = hwnd;
    pd.hDevMode       = NULL;
    pd.hDevNames      = NULL;
    pd.Flags          = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
    pd.nCopies        = 1;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

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

        Adds printing capability to the EDITFILE program.  Printing request
        is sent as an IDM_PRINT message.

        Before the printing operation begins, a modeless dialog box is
        created to allow the user to abort the printing operation.  This
        dialog box remains active until the print job is completed, or the
        user cancels the print operation.

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    FARPROC lpProcAbout;

    WORD IOStatus;                           /* result of file i/o      */
    int Status;
    int nPageSize;			    /* vert. resolution of printer device */
    DWORD FlagSave;


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
                    szFileName[0] = 0;

                    /* Update the edit buffer */

                    SetNewBuffer(hWnd, NULL, Untitled);
                    break;

                case IDM_OPEN:
                    if (!QuerySaveFile(hWnd))
                        return (NULL);

                    /* Use standard open dialog */

		    if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
			return NULL;

		    /* Open the file and get its handle */
                    hFile = OpenFile ((LPSTR)szFileName, (LPOFSTRUCT)&OfStruct,
						  OF_READ);
                    if (!hFile)
                        return (NULL);

                    /* Allocate edit buffer to the size of the file + 1 */
		    fstat(hFile, &FileStatus);

                    if (FileStatus.st_size > 65534) {
                        MessageBox(hWnd, "Can't load files larger than 65,534 bytes long",
                            NULL, MB_OK | MB_ICONEXCLAMATION);
                        close(hFile);
                        return (NULL);
                    }

                    hEditBuffer =
                        LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT,
			    (WORD)FileStatus.st_size+1);

                    if (!hEditBuffer) {
                        MessageBox(hWnd, "Not enough memory.",
                            NULL, MB_OK | MB_ICONEXCLAMATION);
                        close(hFile);
                        return (NULL);
                    }

                    hSaveCursor = SetCursor(hHourGlass);
                    pEditBuffer = LocalLock(hEditBuffer);

                    IOStatus = read(hFile, pEditBuffer, (WORD)FileStatus.st_size);
                    close(hFile);

                    /* # bytes read must equal file size */

                    if (IOStatus != (WORD)FileStatus.st_size) {

                        sprintf(szTemp, "Error reading %s.", szFileName);
                        SetCursor(hSaveCursor);      /* Remove the hourglass */
                        MessageBox(hWnd, szTemp, NULL, MB_OK | MB_ICONEXCLAMATION);
                    }

                    LocalUnlock(hEditBuffer);

                    /* Set up a new buffer and window title */

                    sprintf(szTemp, "%s - %s", szBaseWindowTitle, szFileTitle);
                    SetNewBuffer(hWnd, hEditBuffer, szTemp);
                    SetCursor(hSaveCursor);            /* restore the cursor */
                    break;

                case IDM_SAVE:

                    /* If there is no filename, use the saveas command to get
                     * one.  Otherwise, save the file using the current
                     * filename.
                     */
                    if (szFileName[0]){
			if (bChanges)
			    SaveFile(hWnd);
			break;
		    }
		    /* else fall thro' for Saveas processing */

                case IDM_SAVEAS:

                    /* Use standard save dialog */

		    if (!GetSaveFileName ((LPOPENFILENAME)&ofn))
			return FALSE;		/* User canceled */

                    /* Change window title to include file title */

                    lstrcpy(szWindowTitle, szBaseWindowTitle);
                    lstrcat(szWindowTitle, " - ");
                    lstrcat(szWindowTitle, szFileTitle);
                    SetWindowText(hWnd, szWindowTitle);

		    SaveFile(hWnd);
		    break;

                case IDM_PRINT:
                    hSaveCursor = SetCursor(hHourGlass);
                    hPr = GetPrinterDC();
                    if (!hPr) {
                        sprintf(szTemp, "Cannot print %s", szFileName);
                        MessageBox(hWnd, szTemp, NULL, MB_OK | MB_ICONHAND);
                        return (NULL);
                    }

                    lpAbortDlg =  MakeProcInstance(AbortDlg, hInst);
                    lpAbortProc = MakeProcInstance(AbortProc, hInst);

                    /* Define the abort function */

                    Escape(hPr, SETABORTPROC, NULL,
			(LPSTR)  lpAbortProc,
                        (LPSTR) NULL);

                    if (Escape(hPr, STARTDOC, 14, "PrntFile text",
                            (LPSTR) NULL) < 0) {
                        MessageBox(hWnd, "Unable to start print job",
                            NULL, MB_OK | MB_ICONHAND);
                        FreeProcInstance(lpAbortDlg);
                        FreeProcInstance(lpAbortProc);
                        DeleteDC(hPr);
                    }

                    bAbort = FALSE; /* Clears the abort flag  */

                    /* Create the Abort dialog box (modeless) */

                    hAbortDlgWnd = CreateDialog(hInst, "AbortDlg",
                        hWnd, lpAbortDlg);

                    if (!hAbortDlgWnd) {
                        SetCursor(hSaveCursor);      /* Remove the hourglass */
                        MessageBox(hWnd, "NULL Abort window handle",
                            NULL, MB_OK | MB_ICONHAND);
                        return (FALSE);
                    }
                    
                    /* Now show Abort dialog */

                    ShowWindow (hAbortDlgWnd, SW_NORMAL);

                    /* Disable the main window to avoid reentrancy problems */

                    EnableWindow(hWnd, FALSE);
                    SetCursor(hSaveCursor);      /* Remove the hourglass */

                    /* Since you may have more than one line, you need to
                     * compute the spacing between lines.  You can do that by
                     * retrieving the height of the characters you are printing
                     * and advancing their height plus the recommended external
                     * leading height.
                     */

                    GetTextMetrics(hPr, &TextMetric);
                    LineSpace = TextMetric.tmHeight +
                        TextMetric.tmExternalLeading;

                    /* Since you may have more lines than can fit on one
                     * page, you need to compute the number of lines you can
                     * print per page.  You can do that by retrieving the
		     * dimensions of the page and dividing the height
                     * by the line spacing.
                     */

		    nPageSize = GetDeviceCaps (hPr, VERTRES);
		    LinesPerPage = nPageSize / LineSpace - 1;


                    /* You can output only one line at a time, so you need a
                     * count of the number of lines to print.  You can retrieve
                     * the count sending the EM_GETLINECOUNT message to the edit
                     * control.
                     */

                    wLines = (WORD)SendMessage(hEditWnd, EM_GETLINECOUNT, 0, 0L);

                    /* Keep track of the current line on the current page */

                    CurrentLine = 1;

                    /* One way to output one line at a time is to retrieve
                     * one line at a time from the edit control and write it
                     * using the TextOut function.  For each line you need to
                     * advance one line space.  Also, you need to check for the
                     * end of the page and start a new page if necessary.
                     */

                    for (wIndex = Status = 0; wIndex < wLines; wIndex++) {
			szLine[0] = 127;	       /* Maximum buffer size */
                        szLine[1] = 0;
                        LineLength = (int)SendMessage(hEditWnd, EM_GETLINE,
                            wIndex, (DWORD)(LPSTR)szLine);
                        TextOut(hPr, 0, CurrentLine*LineSpace,
                            (LPSTR)szLine, LineLength);
                        if (++CurrentLine > LinesPerPage ) {
                            CurrentLine = 1;
                            Status = Escape(hPr, NEWFRAME, 0, 0L, 0L);
                            if (Status < 0 || bAbort)
                                break;
                        }
                    }

                    if (Status >= 0 && !bAbort) {
                        Escape(hPr, NEWFRAME, 0, 0L, 0L);
                        Escape(hPr, ENDDOC, 0, 0L, 0L);
                    }
                    EnableWindow(hWnd, TRUE);

                    /* Destroy the Abort dialog box */

                    DestroyWindow(hAbortDlgWnd);
                    FreeProcInstance(lpAbortDlg);
                    FreeProcInstance(lpAbortProc);
                    DeleteDC(hPr);
                    break;


                case IDM_PRINTSETUP:
                    FlagSave = pd.Flags;
                    pd.Flags |= PD_PRINTSETUP;    /* Set option */
                    PrintDlg((LPPRINTDLG)&pd);
                    pd.Flags = FlagSave;          /* Remove option */
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
                          "PrntFile Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDM_EXIT:
                    QuerySaveFile(hWnd);
                    DestroyWindow(hWnd);
                    break;

                case IDC_EDIT:
                    if (HIWORD (lParam) == EN_CHANGE)
                        bChanges = TRUE;
                    if (HIWORD (lParam) == EN_ERRSPACE) {
                        MessageBox (
                              GetFocus ()
                            , "Out of memory."
                            , "PrntFile Sample Application"
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

    if ((hFile = OpenFile(szFileName, &OfStruct,
        OF_CANCEL | OF_CREATE)) < 0) {

        /* If the file can't be saved */

        sprintf(szTemp, "Cannot write to %s.", szFileName);
        MessageBox(hWnd, szTemp, NULL, MB_OK | MB_ICONHAND);
        return (FALSE);
    }


    hEditBuffer = (HANDLE)SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    pEditBuffer = LocalLock(hEditBuffer);

    /* Set the cursor to an hourglass during the file transfer */

    hSaveCursor = SetCursor(hHourGlass);
    IOStatus = write(hFile, pEditBuffer, strlen(pEditBuffer));
    close(hFile);
    SetCursor(hSaveCursor);
    if (IOStatus != (int)strlen(pEditBuffer)) {
        sprintf(szTemp, "Error writing to %s.", szFileName);
        MessageBox(hWnd, szTemp,
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

    if (bChanges) {

        sprintf(szTemp, "Save current changes: %s", szFileName);
        Response = MessageBox(hWnd, szTemp,
            "EditFile",  MB_YESNOCANCEL | MB_ICONEXCLAMATION);
        if (Response == IDYES) {

            /* Make sure there is a filename to save to */
            while (!szFileName[0]) {

                /* Use common save dialog */
		if (!GetSaveFileName ((LPOPENFILENAME)&ofn))
		    return FALSE;	    /* User canceled */
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

    hOldBuffer = (HANDLE)SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    LocalFree(hOldBuffer);
    if (!hNewBuffer)                    /* Allocates a buffer if none exists */
        hNewBuffer = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 1);

    SendMessage(hEditWnd, EM_SETHANDLE, hNewBuffer, 0L); /* Updates the buffer
							    and displays new buffer */
    SetWindowText(hWnd, Title);
    SetFocus(hEditWnd);
    bChanges = FALSE;
}


/****************************************************************************

    FUNCTION: GetPrinterDC()

    PURPOSE:  Get hDc for current device on current output port according to
              info in WIN.INI.

    COMMENTS:

        Searches WIN.INI for information about what printer is connected, and
        if found, creates a DC for the printer.

        returns
            hDC > 0 if success
            hDC = 0 if failure

****************************************************************************/

HDC GetPrinterDC(void)
{

    HDC         hDC;
    LPDEVMODE   lpDevMode = NULL;
    LPDEVNAMES  lpDevNames;
    LPSTR       lpszDriverName;
    LPSTR       lpszDeviceName;
    LPSTR       lpszPortName;

    if (!PrintDlg((LPPRINTDLG)&pd))
        return(NULL);

    if (pd.hDC)
      {
        hDC = pd.hDC;
      }
    else
      {

        if (!pd.hDevNames)
            return(NULL);

        lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
        lpszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
        lpszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
        lpszPortName   = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
        GlobalUnlock(pd.hDevNames);

        if (pd.hDevMode)
            lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);

        hDC = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (LPSTR)lpDevMode);

        if (pd.hDevMode && lpDevMode)
            GlobalUnlock(pd.hDevMode);
      }

    if (pd.hDevNames)
	{
	GlobalFree(pd.hDevNames);
	pd.hDevNames=NULL;
	}
    if (pd.hDevMode)
       {
       GlobalFree(pd.hDevMode);
       pd.hDevMode=NULL;
       }
    return(hDC);
}

/****************************************************************************

    FUNCTION: AbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

int FAR PASCAL AbortProc(hPr, Code)
HDC hPr;                            /* for multiple printer display contexts */
int Code;                           /* printing status                */
{
    MSG msg;

    if (!hAbortDlgWnd)              /* If the abort dialog isn't up yet */
        return(TRUE);

    /* Process messages intended for the abort dialog box */

    while (!bAbort && PeekMessage(&msg, NULL, NULL, NULL, TRUE))
        if (!IsDialogMessage(hAbortDlgWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return (!bAbort);
}

/****************************************************************************

    FUNCTION: AbortDlg(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for printer abort dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

    COMMENTS

        This dialog box is created while the program is printing, and allows
        the user to cancel the printing process.

****************************************************************************/

int FAR PASCAL AbortDlg(hDlg, msg, wParam, lParam)
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
    switch(msg) {

        /* Watch for Cancel button, RETURN key, ESCAPE key, or SPACE BAR */

        case WM_COMMAND:
            return (bAbort = TRUE);

        case WM_INITDIALOG:

            /* Set the focus to the Cancel box of the dialog */

            SetFocus(GetDlgItem(hDlg, IDCANCEL));
            SetDlgItemText(hDlg, IDC_FILENAME, szFileName);
            return (TRUE);
        }
    return (FALSE);
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
            return (TRUE);
    }
    return (FALSE);
}

