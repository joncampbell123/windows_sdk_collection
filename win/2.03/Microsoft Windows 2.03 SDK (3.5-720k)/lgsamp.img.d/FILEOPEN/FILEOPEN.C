/****************************************************************************

    PROGRAM: Fileopen.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	FileOpenInit() - initializes window data and registers window
	FileOpenWndProc() - processes messages
	OpenDlg() - let user select a file, and open it.
	UpdateListBox() - Update the list box of OpenDlg
	ChangeDefExt() - Change the default extension
	SeparateFile() - Separate filename and pathname
	AddExt() - Add default extension
	_lstrlen() - FAR version of strlen()
	_lstrncpy() - FAR version of strncpy()
	_lstrcpy() - FAR version of strcpy()
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "fileopen.h"

HANDLE hInst;
HANDLE hAccTable;
HWND hEditWnd;

char FileName[128];			       /* current filename	     */
char PathName[128];			       /* current pathname	     */
char OpenName[128];			       /* filename to open	     */
char DefPath[128];			       /* default path for list box  */
char DefSpec[13] = "*.*";		       /* default search spec	     */
char DefExt[] = ".txt";			       /* default extension	     */
char str[255];				       /* string for sprintf() calls */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

	DefSpec is set to "*.*" for use in the list box.  This will be
	changed whenever the user specifies a wild card in the OpenDlg Edit
	box.  One could also get the default extension from the WIN.INI
	file.

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{

    HWND hWnd;
    MSG msg;
    RECT Rect;

    if (!hPrevInstance)
	if (!FileOpenInit(hInstance))
	    return(NULL);

    hInst = hInstance;

    hWnd = CreateWindow("FileOpen",
	"FileOpen Sample Application",
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
	return(NULL);

    GetClientRect(hWnd, (LPRECT) &Rect);

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
	hWnd,
	ID_EDIT,
	hInst,
	NULL);

    if (!hEditWnd) {
	DestroyWindow(hWnd);
	return (NULL);
    }


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
	if (!TranslateAccelerator(hWnd, hAccTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg); 
	}
    }
    return(msg.wParam);
}

/****************************************************************************

    FUNCTION: EditfileInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL FileOpenInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "FileOpen";
    pWndClass->lpszClassName = (LPSTR) "FileOpen";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = FileOpenWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return(bSuccess);
}

/****************************************************************************

    FUNCTION: EditfileWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND - application menu selection
	WM_DESTROY - destroy window

    COMMENTS:

	WM_COMMAND processing:

	    IDM_OPEN - query to save current file if there is one and it
		       has been changed, open a new file.

	    IDM_ABOUT - display "About" box.

****************************************************************************/

long FAR PASCAL FileOpenWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpOpenDlg;

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "FileOpenMenu");
	    break;

	case WM_COMMAND:
	    switch (wParam) {
		case IDM_OPEN:

		    /* Call OpenDlg() to get the filename */

		    lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);
		    DialogBox(hInst, "Open", hWnd, lpOpenDlg);
		    FreeProcInstance(lpOpenDlg);
		    break;

		case IDM_NEW:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_PRINT:
		    MessageBox(hWnd, "Command not implemented",
			(LPSTR) NULL, MB_OK);
		    break;  

		case IDM_EXIT:
		    DestroyWindow(hWnd);
		    break;
    
		case IDM_ABOUT:
		    lpProcAbout = MakeProcInstance(About, hInst);
		    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		    FreeProcInstance(lpProcAbout);
		    break;

		/* edit menu commands */

		case IDM_UNDO:
		case IDM_CUT:
		case IDM_COPY:
		case IDM_PASTE:
		case IDM_CLEAR:
		    MessageBox(hWnd, "Command not implemented",
			(LPSTR) NULL, MB_OK);
		    break;  
	    }
	    break;

	case WM_DESTROY:
	    PostQuitMessage(NULL);
	    break;

	default:
	    return(DefWindowProc(hWnd, message, wParam, lParam));
    }
    return(NULL);
}

/****************************************************************************

    FUNCTION: OpenDlg(HWND, unsigned, WORD, LONG)

    PURPOSE: Let user select a file, and open it.

    COMMENTS:

	Get selection, which may be either a prefix to a new search path or
	a filename.  DlgDirSelect parses selection, and appends a backslash
	if selection is a prefix.

****************************************************************************/

HANDLE FAR PASCAL OpenDlg(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    WORD index;			   /* index to the filenames in the list box */
    PSTR pTptr;			   /* temporary pointer			     */
    HANDLE hFile;		   /* handle to the opened file		     */

    switch (message) {
	case WM_COMMAND:
	    switch (wParam) {

		case ID_LISTBOX:
		    switch (HIWORD(lParam)) {

			/* If the user clicks on a file in the list box, then
			 * check to see if if a directory or a file is selected.
			 * If it is a file, then update the ID_EDIT control
			 * which contains the current filename.	 If it's a
			 * directory, then update the default filespec, and
			 * update the list box.
			 */

			case LBN_SELCHANGE:
			    if (!DlgDirSelect(hDlg, str, ID_LISTBOX)) {

				/* update filename selection */

				SetDlgItemText(hDlg, ID_EDIT, str);
				SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL,
				    NULL, MAKELONG(0, 0x7fff));
			    }
			    else {

				/* update directory selection */

				strcat(str, DefSpec);
				DlgDirList(hDlg, str, ID_LISTBOX,
				    ID_PATH, 0x4010);
			    }
			    break;

			/* If the mouse is double clicked, then treat it as
			 * if the "OK" box was selected.  The first click will
			 * have updated the Edit control.
			 */

			case LBN_DBLCLK:
			    goto openfile;
		    }				     /* Ends ID_LISTBOX case */
		    return(TRUE);

		case IDOK:
openfile:
		    /* Get the filename from the edit control */

		    GetDlgItemText(hDlg, ID_EDIT, OpenName, 128);

		    /* Check for a wildcard; if found, set up a new search
		     * path.
		     */

		    if (strchr(OpenName, '*') || strchr(OpenName, '?')) {

			/* Separate filename from path.	 The path is stored
			 * in str, which is discarded if null, else it is used
			 * for a new search path.
			 */

			SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
			    (LPSTR) OpenName);
			if (str[0])
			    strcpy(DefPath, str);

			/* ChangeDefExt checks to see if the user requested a
			 * new extension (*.ext) and if so, it updates the
			 * DefExt string.
			 */

			ChangeDefExt(DefExt, DefSpec);
			UpdateListBox(hDlg);
			return(TRUE);
		    }

		    /* Ignore it if no filename is specified */

		    if (!OpenName[0]) {
			MessageBox(hDlg, "No filename specified.",
			    NULL, MB_OK | MB_ICONQUESTION);
			return(TRUE);
		    }

		    /* Append the default extension if needed */

		    AddExt(OpenName, DefExt);

		    /* The routine to open the file would go here, and the
		     * handle would be returned instead of NULL.
		     */

		    EndDialog(hDlg, NULL);
		    return(TRUE);

		case IDCANCEL:

		    /* Let the caller know the user canceled */

		    EndDialog(hDlg, NULL);
		    return(TRUE);
	    }
	    break;

	case WM_INITDIALOG:			   /* Request to initalize   */
	    UpdateListBox(hDlg);
	    SetDlgItemText(hDlg, ID_EDIT, DefSpec);
	    SendDlgItemMessage(hDlg,		   /* dialog handle	     */
		ID_EDIT,			   /* where to send message  */
		EM_SETSEL,			   /* select characters	     */
		NULL,				   /* additional information */
		MAKELONG(0, 0x7fff));		   /* Accept entire contents */

	    /* Set the focus to the Edit control so the user can type in a
	     * filename without having to click on the Edit control first.
	     */

	    SetFocus(GetDlgItem(hDlg, ID_EDIT));
	    return (FALSE);	  /* Indicates the focus is set to a control */
    }
    return FALSE;
}

/****************************************************************************

    FUNCTION: UpdateListBox(HWND);

    PURPOSE: Update the list box of OpenDlg

    COMMENTS:

	This function is called to update both the list box and the edit
	control within the OpenDlg box.	 It combines the default path and
	default filespec and uses them to update the list box.

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

    FUNCTION: ChangeDefExt(PSTR, PSTR);

    PURPOSE: Change the default extension

    COMMENTS:

	Check the Name to see if it contains a new extension, and if so,
	change the default extension passed as a paramter.  If there are
	any wildcards after the '.', then the default extension will
	remain unchanged.

****************************************************************************/

void ChangeDefExt(Ext, Name)
PSTR Ext, Name;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
	pTptr++;
    if (*pTptr)				     /* true if this is an extension */
	if (!strchr(pTptr, '*') && !strchr(pTptr, '?'))
	    strcpy(Ext, pTptr);		     /* Copies the extension	     */
}

/****************************************************************************

    FUNCTION: SeparateFile(HWND, LPSTR, LPSTR, LPSTR);(PSTR, PSTR)

    PURPOSE: Separate filename from pathname

    COMMENTS:

	This function takes a source filespec and splits it into a path and
	a filename, and copies these into the strings specified.  Because it
	uses the AnsiPrev call, it will work in any language.

****************************************************************************/

void SeparateFile(hDlg, lpDestPath, lpDestFileName, lpSrcFileName)
HWND hDlg;
LPSTR lpDestPath, lpDestFileName, lpSrcFileName;
{
    LPSTR lpTmp;

    /* Set lpTmp to the end of the filename, then backup until a path
     * delimiter is found, or the beginning of the string is reached.
     */

    lpTmp = lpSrcFileName + (long) _lstrlen(lpSrcFileName);

    while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
	lpTmp = AnsiPrev(lpSrcFileName, lpTmp);

    if (*lpTmp != ':' && *lpTmp != '\\') {

	/* No path delimiter was found, so the original filename is copied to
	 * the destination filename, and the destination path is zeroed.
	 */

	_lstrcpy(lpDestFileName, lpSrcFileName);
	lpDestPath[0] = 0;
	return;
    }

    /* A path delimiter was found.  Copy the text to the right of the
     * delimiter to the destination filename, and copy the text to the left of
     * the delimiter to the destination pathname.
     */

    _lstrcpy(lpDestFileName, lpTmp + 1L);
    _lstrncpy(lpDestPath, lpSrcFileName, (int) (lpTmp - lpSrcFileName) + 1);
    lpDestPath[(lpTmp - lpSrcFileName) + 1] = 0;
}

/****************************************************************************

    FUNCTION: AddExt(PSTR, PSTR);

    PURPOSE: Add default extension

    COMMENTS:

	Check the filename to see if a '.' was specified.  If not, then add
	the extension.

/***************************************************************************/

void AddExt(Name, Ext)
PSTR Name, Ext;
{
    PSTR pTptr;

    pTptr = Name;
    while (*pTptr && *pTptr != '.')
	pTptr++;
    if (*pTptr != '.')			 /* If no extension, add the default */
	strcat(Name, Ext);
}

/****************************************************************************

    FUNCTION: _lstrlen(LPSTR)

    PURPOSE:  uses a long far pointer to the string, returns the length

    COMMENTS:

	Because you are compiling with the short model, the normal string
	handling functions will not handle far pointers.  This function
	would be unneccesary if compiled using a medium or larger model.

****************************************************************************/

int _lstrlen(lpStr)
LPSTR lpStr;
{
    int i;
    for (i = 0; *lpStr++; i++);		    /* Gets length using far pointer */
    return(i);
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
	if (!(*lpDest++ = *lpSrc++))
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
	    return(TRUE);

	case WM_COMMAND:
	    if (wParam == IDOK) {
		EndDialog(hDlg, TRUE);
		return(TRUE);
	    }
	    return(TRUE);
    }
    return(FALSE);
}
