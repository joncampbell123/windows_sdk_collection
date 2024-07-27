/****************************************************************************

    PROGRAM: Prntfile.c

    PURPOSE: Loads, saves, and edits, and prints text files

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	FileprntInit() - initializes window data and registers window
	FileprntWndProc() - processes messages
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
#include "prntfile.h"

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

HANDLE hEditBuffer;
HANDLE hOldBuffer;
HANDLE hHourGlass;
HANDLE hSaveCursor;
int hFile;
int count;
PSTR pBuffer;
OFSTRUCT OfStruct;
struct stat FileStatus;
BOOL bChanges;
BOOL bSaveEnabled;
PSTR pEditBuffer;
RECT Rect;

char Untitled[] =
     "Edit File - (untitled)";

/* Printer variables  */

HDC hPr;			    /* handle for printer device context     */
int LineSpace;			    /* spacing between lines		     */
int LinesPerPage;		    /* lines per page			     */
int CurrentLine;		    /* current line			     */
int LineLength;			    /* line length			     */
DWORD dwLines;			    /* number of lines to print		     */
DWORD dwIndex;			    /* index into lines to print	     */
char pLine[128];		    /* buffer to store lines before printing */
TEXTMETRIC TextMetric;		    /* information about character size	     */
POINT PhysPageSize;		    /* information about the page	     */
BOOL bAbort;			    /* FALSE if user cancels printing	     */
HWND hAbortDlgWnd;
FARPROC lpAbortDlg, lpAbortProc;

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
	if (!PrntFileInit(hInstance))
	    return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("PrntFile",
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

    strcpy(DefSpec, "*.*");

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

    FUNCTION: PrntFileInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL PrntFileInit(hInst)
HANDLE hInst;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "PrntFileMenu";
    pWndClass->lpszClassName = (LPSTR) "PrntFile";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInst;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = PrntFileWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: FileprntWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_DESTROY    - destroy window
	WM_SIZE	      - window size has changed
	WM_QUERYENDSESSION - willing to end session?
	WM_ENDSESSION - end Windows session
	WM_CLOSE      - close the window
	WM_SIZE	      - window resized

    COMMENTS:

	Adds printing capability to the EDITFILE program.  Printing request
	is sent as an IDM_PRINT message.

	Before the printing operation begins, a modeless dialog box is
	created to allow the user to abort the printing operation.  This
	dialog box remains active until the print job is completed, or the
	user cancels the print operation.

****************************************************************************/

long FAR PASCAL PrntFileWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout, lpOpenDlg, lpSaveAsDlg;

    int Success;
    int IOStatus;

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "EditMenu");
	    break;

	case WM_COMMAND:
	    switch (wParam) {

		case IDM_NEW:
		    if (!QuerySaveFile(hWnd))
			return (NULL);
		    bChanges = FALSE;
		    FileName[0] = 0;
		    SetNewBuffer(hWnd, NULL, Untitled);
		    break;

		case IDM_OPEN:
		    if (!QuerySaveFile(hWnd))
			return (NULL);

		    lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);

		    hFile = DialogBox(hInst, "Open", hWnd, lpOpenDlg);
		    FreeProcInstance(lpOpenDlg);
		    if (!hFile)
			return (NULL);

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

		    if (IOStatus != FileStatus.st_size) {

			sprintf(str, "Error reading %s.", FileName);
			SetCursor(hSaveCursor);	    /* Removes the hourglass */
			MessageBox(hWnd, str, NULL,
			    MB_OK | MB_ICONQUESTION);
		    }

		    LocalUnlock(hEditBuffer);

		    sprintf(str, "PrntFile - %s", FileName);
		    SetNewBuffer(hWnd, hEditBuffer, str);
		    SetCursor(hSaveCursor);	    /* Restores the cursor   */
		    break;

		case IDM_SAVE:

		    if (!FileName[0])
			goto saveas;
		    if (bChanges)
			SaveFile(hWnd);
		    break;

		case IDM_SAVEAS:
saveas:
		    lpSaveAsDlg = MakeProcInstance(SaveAsDlg, hInst);

		    Success = DialogBox(hInst, "SaveAs", hWnd, lpSaveAsDlg);
		    FreeProcInstance(lpSaveAsDlg);

		    if (Success == IDOK) {
			sprintf(str, "PrntFile - %s", FileName);
			SetWindowText(hWnd, str);
			SaveFile(hWnd);
			break;
		    }
		    break;

		case IDM_PRINT:
		    hPr = GetPrinterDC();
		    if (!hPr) {
			sprintf(str, "Cannot print %s", FileName);
			MessageBox(hWnd, str, NULL, MB_OK | MB_ICONQUESTION);
			return (NULL);
		    }

		    lpAbortDlg =  MakeProcInstance(AbortDlg, hInst);
		    lpAbortProc = MakeProcInstance(AbortProc, hInst);

		    /* Define the abort function */

		    Escape(hPr, SETABORTPROC, NULL,
			(LPSTR) (long) lpAbortProc,
			(LPSTR) NULL);

		    if (Escape(hPr, STARTDOC, 4, "PrntFile text",
			    (LPSTR) NULL) < 0) {
			MessageBox(hWnd, "Unable to start print job",
			    NULL, MB_OK | MB_ICONQUESTION);
			FreeProcInstance(AbortDlg);
			FreeProcInstance(AbortProc);
			DeleteDC(hPr);
		    }

		    bAbort = FALSE; /* Clears the abort flag  */

		    /* Create the Abort dialog box (modeless) */

		    hAbortDlgWnd = CreateDialog(hInst, "AbortDlg",
			hWnd, lpAbortDlg);

		    /* Disable the main window to avoid reentrancy problems */

		    EnableWindow(hWnd, FALSE);

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
		     * print per page.	You can do that by retrieving the
		     * physical dimensions of the page and dividing the height
		     * by the line spacing.
		     */

		    Escape(hPr, GETPHYSPAGESIZE, NULL, (LPSTR) NULL,
			(LPSTR) &PhysPageSize);
		    LinesPerPage = PhysPageSize.y / LineSpace;


		    /* You can output only one line at a time, so you need a
		     * count of the number of lines to print.  You can retrieve
		     * the count sending the EM_GETLINECOUNT message to the edit
		     * control.
		     */

		    dwLines = SendMessage(hEditWnd, EM_GETLINECOUNT, 0, 0L);

		    /* Keep track of the current line on the current page */

		    CurrentLine = 1;

		    /* One way to output one line at a time is to retrieve
		     * one line at a time from the edit control and write it
		     * using the TextOut function.  For each line you need to
		     * advance one line space.	Also, you need to check for the
		     * end of the page and start a new page if necessary.
		     */

		    for (dwIndex = IOStatus = 0; dwIndex < dwLines; dwIndex++) {
			pLine[0] = 128;		      /* Maximum buffer size */
			pLine[1] = 0;
			LineLength = SendMessage(hEditWnd, EM_GETLINE,
			    (WORD)dwIndex, (LONG)((LPSTR)pLine));
			TextOut(hPr, 0, CurrentLine*LineSpace,
			    (LPSTR)pLine, LineLength);
			if (++CurrentLine > LinesPerPage ) {
			    Escape(hPr, NEWFRAME, 0, 0L, 0L);
			    CurrentLine = 1;
			    IOStatus = Escape(hPr, NEWFRAME, 0, 0L, 0L);
			    if (IOStatus<0 || bAbort)
				break;
			}
		    }

		    if (IOStatus >= 0 && !bAbort) {
			Escape(hPr, NEWFRAME, 0, 0L, 0L);
			Escape(hPr, ENDDOC, 0, 0L, 0L);
		    }
		    EnableWindow(hWnd, TRUE);

		    /* Destroy the Abort dialog box */

		    DestroyWindow(hAbortDlgWnd);
		    FreeProcInstance(AbortDlg);
		    FreeProcInstance(AbortProc);
		    DeleteDC(hPr);
		    break;

		case IDM_EXIT:
		    if (QuerySaveFile(hWnd))
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

	case WM_DESTROY:
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

	    if (!FileName[0])
		bSaveEnabled = FALSE;
	    else {
		bSaveEnabled = TRUE;
		DlgDirList(hDlg, DefPath, NULL, ID_PATH, 0x4010);
		SetDlgItemText(hDlg, ID_EDIT, FileName);
		SendDlgItemMessage(hDlg, ID_EDIT, EM_SETSEL, 0,
		    MAKELONG(0, 0x7fff));
	    }

	    EnableWindow(GetDlgItem(hDlg, IDOK), bSaveEnabled);
	    SetFocus(GetDlgItem(hDlg, ID_EDIT));
	    return (FALSE);		    /* FALSE since Focus was changed */

	case WM_COMMAND:
	    switch (wParam) {
		case ID_EDIT:
		    if (HIWORD(lParam) == EN_CHANGE && !bSaveEnabled)
		    EnableWindow(GetDlgItem(hDlg, IDOK),
			bSaveEnabled = TRUE);
		    return (TRUE);

		case IDOK:
		    GetDlgItemText(hDlg, ID_EDIT, TempName, 128);
		    if (CheckFileName(hDlg, FileName, TempName)) {
			SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec,
			    (LPSTR) FileName);
			if (str[0])
			    strcpy(DefPath, str);
			EndDialog(hDlg, IDOK);
		    }
		    return (TRUE);

		case IDCANCEL:
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
		    if (strchr(OpenName, '*') ||
			strchr(OpenName, '?')) {
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
		    if ((hFile = OpenFile(OpenName, (LPOFSTRUCT) &OfStruct,
			    OF_READ)) < 0) {
			sprintf(str, "Error %d opening %s.",
			    OfStruct.nErrCode, OpenName);
			MessageBox(hDlg, str, NULL, MB_OK | MB_ICONQUESTION);
		    }
		    else {
			fstat(hFile, &FileStatus);
			if (FileStatus.st_size > MAXFILESIZE) {
			    sprintf(str,
		  "Not enough memory to load %s.\n%s exceeds %u bytes.",
				OpenName, OpenName, MAXFILESIZE);
			    MessageBox(hDlg, str, NULL,
				MB_OK | MB_ICONQUESTION);
			    return (TRUE);
			}
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

	case WM_INITDIALOG:
	    UpdateListBox(hDlg);
	    SetDlgItemText(hDlg, ID_EDIT, DefSpec);
	    SendDlgItemMessage(hDlg,
		ID_EDIT,
		EM_SETSEL,
		NULL,
		MAKELONG(0, 0x7fff));
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
	    strcpy(Ext, Name);
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

****************************************************************************/

BOOL CheckFileName(hWnd, pDest, pSrc)
HWND hWnd;
PSTR pDest, pSrc;
{
    PSTR pTmp;

    if (!pSrc[0])
	return (FALSE);

    pTmp = pSrc;
    while (*pTmp) {
	switch (*pTmp++) {
	    case '*':
	    case '?':
		MessageBox(hWnd, "Wildcards not allowed.",
		    NULL, MB_OK | MB_ICONQUESTION);
		return (FALSE);
	}
    }

    AddExt(pSrc, DefExt);

    if (OpenFile(pSrc, (LPOFSTRUCT) &OfStruct, OF_EXIST) >= 0) {
	sprintf(str, "Replace existing %s?", pSrc);
	if (MessageBox(hWnd, str, "PrntFile",
		MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
	    return (FALSE);
    }
    strcpy(pDest, pSrc);
    return (TRUE);
}

/****************************************************************************

    FUNCTION: SaveFile(HWND)

    PURPOSE: Save current file

****************************************************************************/

BOOL SaveFile(hWnd)
HWND hWnd;
{
    BOOL bSuccess;
    int IOStatus;

    if ((hFile = OpenFile(FileName, &OfStruct,
	OF_PROMPT | OF_CANCEL | OF_CREATE)) < 0) {
	sprintf(str, "Cannot write to %s.", FileName);
	MessageBox(hWnd, str, NULL, MB_OK | MB_ICONQUESTION);
	return (FALSE);
    }


    hEditBuffer = SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    pEditBuffer = LocalLock(hEditBuffer);
    hSaveCursor = SetCursor(hHourGlass);
    IOStatus = write(hFile, pEditBuffer, strlen(pEditBuffer));
    close(hFile);
    SetCursor(hSaveCursor);
    if (IOStatus != strlen(pEditBuffer)) {
	sprintf(str, "Error writing to %s.", FileName);
	MessageBox(hWnd, str, NULL, MB_OK | MB_ICONQUESTION);
	bSuccess = FALSE;
    }
    else {
	bSuccess = TRUE;
	bChanges = FALSE;
    }

    LocalUnlock(hEditBuffer);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: QuerySaveFile(HWND);

    PURPOSE: Called when some action might lose current contents

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
		Response = DialogBox(hInst, "SaveAs", hWnd, lpSaveAsDlg);
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

****************************************************************************/

void SetNewBuffer(hWnd, hNewBuffer, Title)
HWND hWnd;
HANDLE hNewBuffer;
PSTR Title;
{
    HANDLE hOldBuffer;

    hOldBuffer = SendMessage(hEditWnd, EM_GETHANDLE, 0, 0L);
    LocalFree(hOldBuffer);
    if (!hNewBuffer)
	hNewBuffer = LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, 1);

    SendMessage(hEditWnd, EM_SETHANDLE, hNewBuffer, 0L);
    InvalidateRect(hEditWnd, NULL, TRUE);
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

HANDLE GetPrinterDC()
{
    char pPrintInfo[80];
    LPSTR lpTemp;
    LPSTR lpPrintType;
    LPSTR lpPrintDriver;
    LPSTR lpPrintPort;

    if (!GetProfileString("windows", "Device", (LPSTR)"", pPrintInfo, 80))
	return (NULL);
    lpTemp = lpPrintType = pPrintInfo;
    lpPrintDriver = lpPrintPort = 0;
    while (*lpTemp) {
	if (*lpTemp == ',') {
	    *lpTemp++ = 0;
	    while (*lpTemp == ' ')
		lpTemp = AnsiNext(lpTemp);
	    if (!lpPrintDriver)
		lpPrintDriver = lpTemp;
	    else {
		lpPrintPort = lpTemp;
		break;
	    }
	}
	else
	    lpTemp = AnsiNext(lpTemp);
    }

    return (CreateDC(lpPrintDriver, lpPrintType, lpPrintPort, (LPSTR) NULL));
}

/****************************************************************************

    FUNCTION: AbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

int FAR PASCAL AbortProc(hPr, Code)
HDC hPr;			    /* for multiple printer display contexts */
int Code;			    /* printing status			     */
{
    MSG msg;

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
	    SetDlgItemText(hDlg, ID_FILENAME, FileName);
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
	    if (wParam == IDOK) {
		EndDialog(hDlg, TRUE);
		return (TRUE);
	    }
	    return (TRUE);
    }
    return (FALSE);
}
