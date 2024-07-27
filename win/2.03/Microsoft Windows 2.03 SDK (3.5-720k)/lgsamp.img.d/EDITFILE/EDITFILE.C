/****************************************************************************

    PROGRAM: Editfile.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	EditfileInit() - initializes window data and registers window
	EditfileWndProc() - processes messages
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
	_lstrlen() - uses a long far pointer to the string, returns the length
	_lstrncpy() - FAR version of strncpy()
	_lstrcpy() - FAR version of strcpy()

****************************************************************************/

#include "windows.h"
#include "editfile.h"

/* Additional includes needed for the fstat() function */

#include <sys\types.h>
#include <sys\stat.h>

HANDLE hInst;
HANDLE hAccTable;
HWND hEditWnd;
char FileName[128];
char PathName[128];
char OpenName[128];
char DefPath[128];
char DefSpec[13] = "*.*";
char DefExt[] = ".txt";
char str[255];

HANDLE hEditBuffer;			  /* handle to editing buffer	     */
HANDLE hOldBuffer;			  /* old buffer handle		     */
HANDLE hHourGlass;			  /* handle to hourglass cursor	     */
HANDLE hSaveCursor;			  /* current cursor handle	     */
int hFile;				  /* file handle		     */
int count;				  /* number of chars read or written */
PSTR pBuffer;				  /* address of read/write buffer    */
OFSTRUCT OfStruct;			  /* information from OpenFile()     */
struct stat FileStatus;			  /* information from fstat()	     */
BOOL bChanges = FALSE;			  /* TRUE if the file is changed     */
BOOL bSaveEnabled = FALSE;		  /* TRUE if text in the edit buffer */
PSTR pEditBuffer;			  /* address of the edit buffer	     */
RECT Rect;				  /* dimension of the client window  */

char Untitled[] =			  /* default window title	     */
     "Edit File - (untitled)";

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
WORD nCmdShow;
{

    HWND hWnd;
    MSG msg;

    if (!hPrevInstance)
	if (!EditFileInit(hInstance))
	    return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("EditFile",
	Untitled,
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
	return (FALSE);
    }

    /* Get an hourglass cursor to use during file transfers */

    hHourGlass = LoadCursor(NULL, IDC_WAIT);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
	if (!TranslateAccelerator(hWnd, hAccTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg); 
	}
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: EditfileInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL EditFileInit(hInst)
HANDLE hInst;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "EditFileMenu";
    pWndClass->lpszClassName = (LPSTR) "EditFile";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInst;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = EditFileWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: EditfileWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_DESTROY    - destroy window
	WM_SIZE	      - window size has changed
	WM_QUERYENDSESSION - willing to end session?
	WM_ENDSESSION - end Windows session
	WM_CLOSE      - close the window
	WM_SIZE	      - window resized

    COMMENTS:

	WM_COMMAND processing:

	    IDM_NEW - query to save current file if there is one and it has
		      been changed, clear buffer and start new file.

	    IDM_OPEN - query to save current file if there is one and it
		       has been changed, open a new file.

	    IDM_SAVE - save current file, prompt for name if none exists.

	    IDM_SAVEAS - prompt for new filename to save to, save file.

	    ID_EDIT - change "bChanges" flag to indicate if edit buffer has
		      been modified.  Affects actions of IDM_NEW and
		      IDM_OPEN.	 Reset when file is saved.

	    IDM_EXIT - query to save current file if there is one and it
		       has been changed, then exit.

	    IDM_ABOUT - display "About" box.

	When more then one string needs to be sent to a message box,
	sprintf() is used to combine the strings into str[], and then str[]
	is passed to the MessageBox() function.	 A message box string cannot
	exceed 255 characters, but may contain \n to generate separate
	lines.

	After the size of the file is determined, only enough memory to store
	the file is allocated for the Edit buffer.  The edit control will
	automatically expand this memory as needed.  Once the file has been
	read into the edit buffer, unlock the memory.  Use whatever was
	obtained from the read() function, even if an error occured.  This
	allows partial salvage of a file with a bad sector.

****************************************************************************/

long FAR PASCAL EditFileWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpOpenDlg, lpSaveAsDlg;

    int Success;			    /* return value from SaveAsDlg() */
    int IOStatus;			    /* result of file i/o	     */

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "EditMenu");
	    break;

	case WM_COMMAND:
	    switch (wParam) {
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
			    FileStatus.st_size+1);

		    if (!hEditBuffer) {
			MessageBox(hWnd, "Not enough memory.",
			    NULL, MB_OK | MB_ICONQUESTION);
			return (NULL);
		    }
		    hSaveCursor = SetCursor(hHourGlass);
		    pEditBuffer = LocalLock(hEditBuffer);

		    IOStatus = read(hFile, pEditBuffer, FileStatus.st_size);
		    close(hFile);

		    /* # bytes read must equal file size */

		    if (IOStatus != FileStatus.st_size) {

			sprintf(str, "Error reading %s.", FileName);
			SetCursor(hSaveCursor);	     /* Remove the hourglass */
			MessageBox(hWnd, str, NULL, MB_OK | MB_ICONQUESTION);
		    }

		    LocalUnlock(hEditBuffer);

		    /* Set up a new buffer and window title */

		    sprintf(str, "EditFile - %s", FileName);
		    SetNewBuffer(hWnd, hEditBuffer, str);
		    SetCursor(hSaveCursor);	       /* restore the cursor */
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
		    break;				    /* User canceled */

		case IDM_EXIT:
		    QuerySaveFile(hWnd);
		    DestroyWindow(hWnd);
		    break;

		case IDM_ABOUT:
		    lpProcAbout = MakeProcInstance(About, hInst);
		    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		    FreeProcInstance(lpProcAbout);
		    break;

		case ID_EDIT:
		    if (HIWORD(lParam) == EN_CHANGE)
			bChanges = TRUE;
		    return (NULL);
	    }
	    break;

	case WM_SIZE:
	    MoveWindow(hEditWnd, 0, 0, LOWORD(lParam),
		HIWORD(lParam), FALSE);
	    break;

	case WM_QUERYENDSESSION:	     /* message: to end the session? */
	    return (QuerySaveFile(hWnd));

	case WM_CLOSE:			     /* message: close the window    */
	    if (QuerySaveFile(hWnd))
		DestroyWindow(hWnd);
	    break;

	case WM_DESTROY:		     /* message: destroy the window  */
	    PostQuitMessage(NULL);
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

		/* Process the path to fit within the ID_PATH field */

		DlgDirList(hDlg, DefPath, NULL, ID_PATH, 0x4010);

		/* Send the current filename to the edit control */

		SetDlgItemText(hDlg, ID_EDIT, FileName);

		/* Accept all characters in the edit control */

		SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL, 0,
		    MAKELONG(0, 0x7fff));
	    }

	    /* Enable or disable the save control depending on whether the
	     * filename exists.
	     */

	    EnableWindow(GetDlgItem(hDlg, IDOK), bSaveEnabled);

	    /* Set the focus to the edit control within the dialog box */

	    SetFocus(GetDlgItem(hDlg, ID_EDIT));
	    return (FALSE);		    /* FALSE since Focus was changed */

	case WM_COMMAND:
	    switch (wParam) {
		case ID_EDIT:

		    /* If there was previously no filename in the edit
		     * control, then the save control must be enabled as soon as
		     * a character is entered.
		     */

		    if (HIWORD(lParam) == EN_CHANGE && !bSaveEnabled)
		    EnableWindow(GetDlgItem(hDlg, IDOK), bSaveEnabled = TRUE);
		    return (TRUE);

		case IDOK:

		   /* Get the filename from the edit control */

		    GetDlgItemText(hDlg, ID_EDIT, TempName, 128);

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

		case ID_LISTBOX:
		    switch (HIWORD(lParam)) {

			case LBN_SELCHANGE:
			    if (!DlgDirSelect(hDlg, str, ID_LISTBOX)) {
				SetDlgItemText(hDlg, ID_EDIT, str);
				SendDlgItemMessage(hDlg,
				    ID_EDIT,
				    EM_SETSEL,
				    NULL,
				    MAKELONG(0, 0x7fff));
			    }
			    else {
				strcat(str, DefSpec);
				DlgDirList(hDlg, str, ID_LISTBOX,
				    ID_PATH, 0x4010);
			    }
			    break;

			case LBN_DBLCLK:
			    goto openfile;
		    }
		    return (TRUE);

		case IDOK:
openfile:
		    GetDlgItemText(hDlg, ID_EDIT, OpenName, 128);
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
			    NULL, MB_OK | MB_ICONQUESTION);
			return (TRUE);
		    }

		    AddExt(OpenName, DefExt);

		    /* Open the file */

		    if ((hFile = OpenFile(OpenName, (LPOFSTRUCT) &OfStruct,
			    OF_READ)) < 0) {
			sprintf(str, "Error %d opening %s.",
			    OfStruct.nErrCode, OpenName);
			MessageBox(hDlg, str, NULL,
			    MB_OK | MB_ICONQUESTION);
		    }
		    else {

			/* Make sure there's enough room for the file */

			fstat(hFile, &FileStatus);
			if (FileStatus.st_size > MAXFILESIZE) {
			    sprintf(str,
		    "Not enough memory to load %s.\n%s exceeds %ld bytes.",
				OpenName, OpenName, MAXFILESIZE);
			    MessageBox(hDlg, str, NULL,
				MB_OK | MB_ICONQUESTION);
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

	case WM_INITDIALOG:			   /* message: initialize    */
	    UpdateListBox(hDlg);
	    SetDlgItemText(hDlg, ID_EDIT, DefSpec);
	    SendDlgItemMessage(hDlg,		   /* dialog handle	     */
		ID_EDIT,			   /* where to send message  */
		EM_SETSEL,			   /* select characters	     */
		NULL,				   /* additional information */
		MAKELONG(0, 0x7fff));		   /* entire contents	     */
	    SetFocus(GetDlgItem(hDlg, ID_EDIT));
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
    DlgDirList(hDlg, str, ID_LISTBOX, ID_PATH, 0x4010);
    SetDlgItemText(hDlg, ID_EDIT, DefSpec);
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

    lpTmp = lpSrcFileName + (long) _lstrlen(lpSrcFileName);
    while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
	lpTmp = AnsiPrev(lpSrcFileName, lpTmp);
    if (*lpTmp != ':' && *lpTmp != '\\') {
	_lstrcpy(lpDestFileName, lpSrcFileName);
	lpDestPath[0] = 0;
	return;
    }
    _lstrcpy(lpDestFileName, lpTmp + 1L);
    _lstrncpy(lpDestPath, lpSrcFileName, (int) (lpTmp - lpSrcFileName) + 1);
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
	return (FALSE);		      /* Indicates no filename was specified */

    pTmp = pSrc;
    while (*pTmp) {			/* Searches the string for wildcards */
	switch (*pTmp++) {
	    case '*':
	    case '?':
		MessageBox(hWnd, "Wildcards not allowed.",
		    NULL, MB_OK | MB_ICONQUESTION);
		return (FALSE);
	}
    }

    AddExt(pSrc, DefExt);	     /* Adds the default extension if needed */

    if (OpenFile(pSrc, (LPOFSTRUCT) &OfStruct, OF_EXIST) >= 0) {
	sprintf(str, "Replace existing %s?", pSrc);
	if (MessageBox(hWnd, str, "EditFile",
		MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
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
	buffer.	 This allows Windows to move the buffer when not in immediate
	use.

****************************************************************************/

BOOL SaveFile(hWnd)
HWND hWnd;
{
    BOOL bSuccess;
    int IOStatus;				   /* result of a file write */

    if ((hFile = OpenFile(FileName, &OfStruct,
	OF_PROMPT | OF_CANCEL | OF_CREATE)) < 0) {

	/* If the file can't be saved */

	sprintf(str, "Cannot write to %s.", FileName);
	MessageBox(hWnd, str, NULL, MB_OK | MB_ICONQUESTION);
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
	    NULL, MB_OK | MB_ICONQUESTION);
	bSuccess = FALSE;
    }
    else {
	bSuccess = TRUE;		/* Indicates the file was saved	     */
	bChanges = FALSE;		/* Indicates changes have been saved */
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
	    "EditFile",	 MB_YESNOCANCEL | MB_ICONQUESTION);
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
	redraw the edit window.	 If hNewBuffer is NULL, then create an empty
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
    if (!hNewBuffer)			/* Allocates a buffer if none exists */
	hNewBuffer = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 1);

    SendMessage(hEditWnd, EM_SETHANDLE, hNewBuffer, 0L);
    InvalidateRect(hEditWnd, NULL, TRUE);	       /* Updates the buffer */
    UpdateWindow(hEditWnd);
    SetWindowText(hWnd, Title);
    SetFocus(hEditWnd);
    bChanges = FALSE;
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
