/***************************************************************************
 *                                                                         *
 *  MODULE    : PpFile.c                                                   *
 *                                                                         *
 *  PURPOSE   : Contains the code for File I/O for penpad.               *
 *                                                                         *
 *  FUNCTIONS : AlreadyOpen   - Determines if a file is already open.      *
 *                                                                         *
 *              AddFile       - Creates a new MDI window and, if specified,*
 *				loads a file into it.                      *
 *                                                                         *
 *              LoadFile      - Loads a file into a MDI window.            *
 *                                                                         *
 *              ReadFile      - Calls File/Open dialog and appropriately   *
 *                              responds to the user's input.              *
 *                                                                         *
 *              SaveFile      - Saves the contents of a MDI window's edit  *
 *                              control to a file.                         *
 *                                                                         *
 *              SetSaveFrom   - Formats the "Save 'file' to" string.       *
 *                                                                         *
 *              SaveAsDlgProc - Dialog function for the File/SaveAs dialog.*
 *                                                                         *
 *              ChangeFile    - Calls File/SaveAs dialog.                  *
 *                                                                         *
 ***************************************************************************/
#include "penpad.h"

VOID FAR PASCAL GetFileName(PSTR);
BOOL FAR PASCAL SaveAsDlgProc( HWND,WORD,WORD,LONG);
int FAR PASCAL DialogBoxParam(HANDLE,LPCSTR,HWND,FARPROC,LONG);

OFSTRUCT	of;
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : AlreadyOpen(szFile)                                        *
 *                                                                          *
 *  PURPOSE    : Checks to see if the file described by the string pointed  *
 *               to by 'szFile' is already open.                            *
 *                                                                          *
 *  RETURNS    : a handle to the described file's window if that file is    *
 *               already open;  NULL otherwise.                             *
 *                                                                          *
 ****************************************************************************/

HWND AlreadyOpen(char *szFile)
{
    int     iDiff;
    HWND    hwndCheck;
    char    szChild[64];
    LPSTR   lpChild, lpFile;
    int     wFileTemp;

    /* Open the file with the OF_PARSE flag to obtain the fully qualified
     * pathname in the OFSTRUCT structure.
     */
    wFileTemp = OpenFile ((LPSTR)szFile, (LPOFSTRUCT)&of, OF_PARSE);
    if (! wFileTemp)
	return(NULL);
    _lclose (wFileTemp);

    /* Check each MDI child window in Penpad */
    for (   hwndCheck = GetWindow(hwndMDIClient, GW_CHILD);
	    hwndCheck;
	    hwndCheck = GetWindow(hwndCheck, GW_HWNDNEXT)   ) {
	/* Initialization  for comparison */
	lpChild = szChild;
	lpFile = AnsiUpper((LPSTR) of.szPathName);
	iDiff = 0;

	/* Skip icon title windows */
	if (GetWindow(hwndCheck, GW_OWNER))
	    continue;

	/* Get current child window's name */
	GetWindowText(hwndCheck, lpChild, 64);

	/* Compare window name with given name */
	while ((*lpChild) && (*lpFile) && (!iDiff)){
	    if (*lpChild++ != *lpFile++)
		iDiff = 1;
	}

	/* If the two names matched, the file is already   */
	/* open -- return handle to matching child window. */
	if (!iDiff)
	    return(hwndCheck);
    }
    /* No match found -- file is not open -- return NULL handle */
    return(NULL);
}

/****************************************************************************
 *									    *
 *  FUNCTION   : AddFile (lpName)					    *
 *									    *
 *  PURPOSE    : Creates a new MDI window. If the lpName parameter is not   *
 *		 NULL, it loads a file into the window. 		    *
 *									    *
 *  RETURNS    : HWND  - A handle to the new window.			    *
 *									    *
 ****************************************************************************/

HWND FAR PASCAL AddFile(pName)
char * pName;
{
    HWND hwnd;

    char	    sz[160];
    MDICREATESTRUCT mcs;

    if (!pName) {
	/* The pName parameter is NULL -- load the "Untitled" string from */
	/* STRINGTABLE and set the title field of the MDI CreateStruct.    */
	LoadString (hInst, IDS_UNTITLED, sz, sizeof(sz));
	mcs.szTitle = (LPSTR)sz;
    }
    else
	/* Title the window with the fully qualified pathname obtained by
	 * calling OpenFile() with the OF_PARSE flag (in function
	 * AlreadyOpen(), which is called before AddFile().
	 */
	mcs.szTitle = of.szPathName;

    mcs.szClass = szChild;
    mcs.hOwner	= hInst;

    /* Use the default size for the window */
    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;

    /* Set the style DWORD of the window to default */
    mcs.style = styleDefault;

    /* tell the MDI Client to create the child */
    hwnd = (WORD)SendMessage (hwndMDIClient,
			      WM_MDICREATE,
			      0,
			      (LONG)(LPMDICREATESTRUCT)&mcs);

    /* Did we get a file? Read it into the window */
    if (pName){
	if (!LoadFile(hwnd, pName)){
	    /* File couldn't be loaded -- close window */
	    SendMessage(hwndMDIClient, WM_MDIDESTROY, (WORD) hwnd, 0L);
	}
    }

    return hwnd;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : LoadFile (lpName)					    *
 *									    *
 *  PURPOSE    : Given the handle to a MDI window and a filename, reads the *
 *		 file into the window's edit control child.                 *
 *									    *
 *  RETURNS    : TRUE  - If file is sucessfully loaded. 		    *
 *		 FALSE - Otherwise.					    *
 *									    *
 ****************************************************************************/

int FAR PASCAL LoadFile (hwnd, pName)
HWND hwnd;
char * pName;
{
    WORD   wLength;
    HANDLE hT;
    LPSTR  lpB;
    HWND   hwndEdit;
    int    fh;

    hwndEdit = GetWindowWord (hwnd, GWW_HWNDEDIT);

    /* The file has a title, so reset the UNTITLED flag. */
    SetWindowWord(hwnd, GWW_UNTITLED, FALSE);

    fh = _lopen (pName, 0);

    /* Make sure file has been opened correctly */
    if ( fh < 0 )
	goto error;

    /* Find the length of the file */
    wLength = (WORD)_llseek (fh, 0L, 2);
    _llseek (fh, 0L, 0);

    /* Attempt to reallocate the edit control's buffer to the file size */
    hT = (HANDLE)SendMessage (hwndEdit, EM_GETHANDLE, 0, 0L);
    if (LocalReAlloc(hT, wLength+1, LHND) == NULL) {
	/* Couldn't reallocate to new size -- error */
	_lclose (fh);
	goto error;
    }

    /* read the file into the buffer */
    if (wLength != _lread (fh, (lpB = (LPSTR)LocalLock (hT)), wLength))
	PPError (hwnd, MB_OK|MB_ICONHAND, IDS_CANTREAD, (LPSTR)pName);

    /* Zero terminate the edit buffer */
    lpB[wLength] = 0;
    LocalUnlock (hT);

    SendMessage (hwndEdit, EM_SETHANDLE, hT, 0L);
    _lclose (fh);

    return TRUE;

error:
    /* Report the error and quit */
    PPError(hwnd, MB_OK | MB_ICONHAND, IDS_CANTOPEN, (LPSTR)pName);
    return FALSE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ReadFile(hwnd)                                             *
 *                                                                          *
 *  PURPOSE    : Called in response to a File/Open menu selection. It asks  *
 *               the user for a file name and responds appropriately.       *
 *                                                                          *
 ****************************************************************************/

VOID FAR PASCAL ReadFile(HWND hwnd)
{
    char    szFile[128];
    HWND    hwndFile;

	 hwnd;					/* Needed to prevent compiler warning message */

    GetFileName (szFile);

    /* If the result is not the empty string -- take appropriate action */
    if (*szFile) {
	/* Is file already open?? */
	if (hwndFile = AlreadyOpen(szFile)) {
	    /* Yes -- bring the file's window to the top */
	    BringWindowToTop(hwndFile);
	}
	else {
	    /* No -- make a new window and load file into it */
	    AddFile(szFile);
	}
    }
}

/****************************************************************************
 *									    *
 *  FUNCTION   : SaveFile (hwnd)					    *
 *									    *
 *  PURPOSE    : Saves contents of current edit control to disk.	    *
 *									    *
 ****************************************************************************/

VOID FAR PASCAL SaveFile( hwnd )

HWND hwnd;
{
    HANDLE   hT;
    LPSTR    lpT;
    char     szFile[128];
    WORD     cch;
    int      fh;
    OFSTRUCT of;
    HWND     hwndEdit;

    hwndEdit = GetWindowWord ( hwnd, GWW_HWNDEDIT);
    GetWindowText (hwnd, szFile, sizeof(szFile));

    /* If there is no extension (control is 'Untitled') add .TXT as extension */
    for (cch = FALSE, lpT = szFile; *lpT; lpT++)
	switch (*lpT){
	    case '.':
		 cch = TRUE;
		 break;

	    case '\\':
	    case ':' :
		 cch = FALSE;
		 break;
	}
    if (!cch)
	LoadString (hInst, IDS_ADDEXT, lpT, lpT - (LPSTR)szFile);

    fh = OpenFile (szFile, &of, OF_WRITE | OF_CREATE);

    /* If file could not be opened, quit */
    if (fh < 0){
	PPError (hwnd, MB_OK | MB_ICONHAND, IDS_CANTCREATE, (LPSTR)szFile);
	return;
    }

    /* Find out the length of the text in the edit control */
    cch = GetWindowTextLength (hwndEdit);

    /* Obtain a handle to the text buffer */
    hT	= (HANDLE)SendMessage (hwndEdit, EM_GETHANDLE, 0, 0L);
    lpT = (LPSTR)LocalLock (hT);

    /* Write out the contents of the buffer to the file. */
    if (cch != _lwrite (fh, lpT, cch))
	PPError (hwnd, MB_OK | MB_ICONHAND, IDS_CANTWRITE, (LPSTR)szFile);

    /* Clean up */
    LocalUnlock (hT);
    SendMessage (hwndEdit, EM_SETHANDLE, hT, 0L);

    _lclose (fh);

    return;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : SetSaveFrom () 					    *
 *									    *
 *  PURPOSE    : Formats the "Save 'file' to .." string.		    *
 *									    *
 ****************************************************************************/

VOID NEAR PASCAL SetSaveFrom ( hwnd , psz)
HWND hwnd;
PSTR psz;
{
    char szFmt[32];
    char szText[160];

    /* The text string in the .RC file contains the format string... */
    GetDlgItemText( hwnd, IDD_SAVEFROM, szFmt, sizeof(szFmt));

    /* NOTE: this (LPSTR) cast MUST be here... wsprintf() is a cdecl
     * (C calling conventions) function with varying args... there is
     * no way for the compiler to know that all strings must be LPSTR's
     * and do the conversion, so we have to be careful about wsprintf()'s.
     */
    wsprintf ( szText, szFmt, (LPSTR)psz);

    /* set the text in the static control */
    SetDlgItemText (hwnd, IDD_SAVEFROM, szText);
}

/****************************************************************************
 *									    *
 *  FUNCTION   : SaveAsDlgProc(hwnd, message, wParam, lParam)		    *
 *									    *
 *  PURPOSE    : Dialog function File/SaveAs. It waits for a filename, and  *
 *		 then calls SaveFile or cancels the operation.		    *
 *									    *
 ****************************************************************************/

BOOL FAR PASCAL SaveAsDlgProc( hwnd, message, wParam, lParam)
HWND hwnd;
WORD message;
WORD wParam;
LONG lParam;
{
    char   sz[64];
    char   *pch;
    BOOL   fExt;
    HWND   hwndSave;

    switch (message){

	case WM_INITDIALOG:

	    /* Identify the window whose contents we're saving */
	    hwndSave = LOWORD (lParam);

	    /* Set it's name in the property list */
	    SetProp (hwnd, PROP_FILENAME, hwndSave);

	    GetWindowText (hwndSave, sz, sizeof(sz));

	    /* Set the save from string... */
	    SetSaveFrom (hwnd,sz);

	    /* Generate a filename complete with extension */
	    AnsiUpper (sz);
	    for (fExt = FALSE, pch = sz; *pch; pch++)
		if (*pch == '.')
		    fExt = TRUE;
		else if (*pch == '\\')
		    fExt = FALSE;
	    if (!fExt)
		LoadString (hInst, IDS_ADDEXT, (LPSTR)pch, pch - sz);

	    /* Display the filename in the edit control */
	    SetDlgItemText (hwnd, IDD_SAVETO, sz);

	    /* Select the entire range of text */
	    SendDlgItemMessage (hwnd, IDD_SAVETO, EM_SETSEL, 0, MAKELONG (0,100));

	    DlgDirList (hwnd, "*.*", IDD_DIRS, IDD_PATH, ATTR_DIRS);

	    /* enable OK butto iff edit control is nonempty */
	    if (!*sz)
		EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);
	    break;

	case WM_COMMAND:
	    switch (wParam){
		case IDCANCEL:
		    /* Abort operation */
		    EndDialog(hwnd,1);
		    break;

		case IDOK:
		   /*  Just change the title of the MDI child. The calling
		    *  function of ChangeFile(), which uses the title text
		    *  for the filename, will do the actual save.
		    */
		    hwndSave = GetProp (hwnd, PROP_FILENAME);
		    GetDlgItemText (hwnd, IDD_SAVETO, sz, sizeof(sz));
		    AnsiUpper ((LPSTR)sz);
		    SetWindowText (hwndSave, sz);
		    EndDialog (hwnd, 0);
		    break;

		case IDD_SAVETO:
		   /* If the edit control changes, check to see if its empty.
		    * enable OK if it contains something
		    */
		    if (HIWORD (lParam) != EN_CHANGE)
			return FALSE;
		    EnableWindow (GetDlgItem (hwnd, IDOK),
			(WORD) SendDlgItemMessage (hwnd,
						   IDD_SAVETO,
						   WM_GETTEXTLENGTH,
						   0,
						   0L));
		    break;

		case IDD_DIRS:
		    if (HIWORD(lParam)==LBN_DBLCLK){
			char szT[64];

			DlgDirSelect ( hwnd, szT, IDD_DIRS);
			lstrcat ( szT, "*.*");
			DlgDirList (hwnd, szT, IDD_DIRS, IDD_PATH, ATTR_DIRS);
			break;
		    }
		    return FALSE;

		default:
		    return FALSE;
	    }

	default:
	    return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : ChangeFile (hwnd)					    *
 *									    *
 *  PURPOSE    : Invokes the File/SaveAs dialog.			    *
 *									    *
 *  RETURNS    : TRUE  - if user selected OK or NO.			    *
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/

BOOL FAR PASCAL ChangeFile (hwnd)
HWND hwnd;
{
    FARPROC lpfn;
    int     i;

    lpfn = MakeProcInstance (SaveAsDlgProc, hInst);
    i = DialogBoxParam (hInst, IDD_SAVEAS, hwnd, lpfn, MAKELONG (hwnd, 0));
    FreeProcInstance (lpfn);
    if (!i)
	SetWindowWord (hwnd, GWW_UNTITLED, 0);
    return !i;
}
