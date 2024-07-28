/****************************************************************************

    PROGRAM: EditFile.c

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
#include "editfile.h"
#include "stdio.h"
#include "io.h"
#include "string.h"
#include <commdlg.h>

HANDLE hInst;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hEditWnd;                                      /* handle to edit window */
/* Additional includes needed for the fstat() function */

#include <sys\types.h>
#include <sys\stat.h>

/* new variables for common dialogs */
static char szFilterSpec [128]
	      = "All Files(*.*)\0*.*\0";  /* filter string for dir. listings   */
char szCustFilterSpec[MAXCUSTFILTER];	  /* custom filter buffer	       */
char szOpenDlgTitle[] = "Open File";	  /* title of File open dialog	       */
char szSaveDlgTitle[] = "Save File";	  /* title of File saveas dialog       */
OPENFILENAME ofn;			  /* struct. passed to GetOpenFileName */

static char PathName[128] = "\0";
static char FileName[128] = "\0";
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

    /* fill in non-variant fields of OPENFILENAME struct. */
    ofn.lStructSize	  = sizeof(OPENFILENAME);
    ofn.hwndOwner	  = hwnd;
    ofn.lpstrFilter	  = szFilterSpec;
    ofn.lpstrCustomFilter = szCustFilterSpec;
    ofn.nMaxCustFilter	  = MAXCUSTFILTER;
    ofn.nFilterIndex	  = 1;
    ofn.lpstrFile	  = FileName;
    ofn.nMaxFile	  = MAXFILENAME;
    ofn.lpstrInitialDir   = PathName;
    ofn.Flags		  = 0L;
    ofn.lpfnHook	  = NULL;

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
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    FARPROC lpProcAbout;

    int IOStatus;			    /* result of file i/o      */

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

		    /* fill in title field of OPENFILENAME struct. and show
		     * dialog box
		     */
		    ofn.lpstrTitle	  = (LPSTR)szOpenDlgTitle;
		    if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
			return NULL;

		    /* Open the file and get its handle */
		    hFile = OpenFile (FileName, (LPOFSTRUCT)&OfStruct,
						  OF_READ);
                    if (!hFile)
                        return (NULL);

                    /* Allocate edit buffer to the size of the file + 1 */
		    fstat(hFile, &FileStatus);
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

		    IOStatus = read(hFile, pEditBuffer, (unsigned int)FileStatus.st_size);
                    close(hFile);

                    /* # bytes read must equal file size */

		    if (IOStatus != (int)FileStatus.st_size) {

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
		    if (FileName[0]){
			if (bChanges)
			    SaveFile(hWnd);
			break;
		    }
		    /* else fall thro' for Saveas processing */

                case IDM_SAVEAS:

		    /* fill in title field of OPENFILENAME struct. and show
		     * dialog box
		     */
		    ofn.lpstrTitle	  = (LPSTR)szSaveDlgTitle;
		    if (!GetSaveFileName ((LPOPENFILENAME)&ofn))
			return FALSE;		/* User canceled */

                    /* If successful, update the window title, save the file */
		    sprintf(str, "EditFile - %s", FileName);
		    SetWindowText(hWnd, str);
		    SaveFile(hWnd);
		    break;

                case IDM_PRINT:
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
		    switch (HIWORD (lParam)){
			case EN_ERRSPACE:
			    MessageBox (
				  GetFocus ()
				, "Out of memory."
				, "EditFile Sample Application"
				, MB_ICONHAND | MB_OK
			    );
			    break;

			case EN_CHANGE:
			    /* contents of edit control buffer have changed */
			    if (!bChanges)
				bChanges = TRUE;
			    break;

			default:
			    break;
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

    if ((hFile = OpenFile(FileName, &OfStruct,
        OF_PROMPT | OF_CANCEL | OF_CREATE)) < 0) {

        /* If the file can't be saved */

        sprintf(str, "Cannot write to %s.", FileName);
        MessageBox(hWnd, str, NULL, MB_OK | MB_ICONEXCLAMATION);
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

    if (bChanges) {

	ofn.lpstrTitle	      = (LPSTR)szSaveDlgTitle;
        sprintf(str, "Save current changes: %s", FileName);
        Response = MessageBox(hWnd, str,
            "EditFile",  MB_YESNOCANCEL | MB_ICONEXCLAMATION);
        if (Response == IDYES) {

            /* Make sure there is a filename to save to */
	    while (!FileName[0]) {

		/* fill in pertinent field of OPENFILENAME struct. and show
		 * dialog box
		 */
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
