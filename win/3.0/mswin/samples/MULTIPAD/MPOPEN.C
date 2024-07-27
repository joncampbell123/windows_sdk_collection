/***************************************************************************
 *									   *
 *  MODULE	: MpOpen.c						   *
 *									   *
 *  PURPOSE	: Contains the file open dialog function and it's helper   *
 *		  functions.						   *
 *									   *
 *  FUNCTIONS	: IsWild ()	      - Ascertains that the input string   *
 *					contains a DOS wildcard character. *
 *									   *
 *		  SelectFile()	      - If filename supplied contains a    *
 *					wildcard, this function fills the  *
 *					listboxes in File/Open dialog, else*
 *					the dialog is closed.		   *
 *									   *
 *		  FileOpenDlgProc()   - Dialog funcion for the File/Open   *
 *					dialog. 			   *
 *									   *
 *		  GetFileName ()      - Gets a file name from the user.    *
 *									   *
 ***************************************************************************/
#include "multipad.h"

char szPropertyName [] = "FILENAME";/* Name of the File name property list item */

int FAR PASCAL DialogBoxParam ( HANDLE, LPSTR, HWND, FARPROC, LONG);


/****************************************************************************
 *									    *
 *  FUNCTION   : IsWild ( psz ) 					    *
 *									    *
 *  PURPOSE    : Checks if the string (referenced by a NEAR pointer)	    *
 *		 contains a DOS wildcard character ("*" or "?").	    *
 *									    *
 *  RETURNS    : TRUE  - iff the string contains a wildcard character.	    *
 *		 FALSE - otherwise.				     .	    *
 *									    *
 ****************************************************************************/
BOOL NEAR PASCAL IsWild( psz )
register PSTR psz;
{
    for(;;)
	switch (*psz++){
	    case '*':
	    case '?':
		/* Found wildcard */
		return TRUE;

	    case 0:
		/* Reached end of string */
		return FALSE;

	    default:
		continue;
	}
}

/****************************************************************************
 *									    *
 *  FUNCTION   : FileExists(pch)                                            *
 *									    *
 *  PURPOSE    : Checks to see if a file exists with the path/filename	    *
 *               described by the string pointed to by 'pch'.               *
 *									    *
 *  RETURNS    : TRUE  - if the described file does exist.                  *
 *               FALSE - otherwise.                                         *
 *									    *
 ****************************************************************************/

BOOL FileExists(PSTR pch)
{
	int fh;

	if ((fh = _lopen((LPSTR) pch, 0)) < 0)
	     return(FALSE);

	_lclose(fh);
	return(TRUE);
}

/****************************************************************************
 *									    *
 *  FUNCTION   : SelectFile ( hwnd )					    *
 *									    *
 *  PURPOSE    : Reads the string in the edit control of the File/Open	    *
 *		 dialog. If it contains a wildcard, then it attempts to     *
 *		 fill the listboxes in the File/Open dialog. Othewise it    *
 *		 ends the dialog. Modifies the FILENAME item in the property*
 *		 list of the window.					    *
 *									    *
 ****************************************************************************/

VOID NEAR PASCAL SelectFile( hwnd )

register HWND hwnd;
{
    register PSTR pch;
    PSTR	  pch2;
    int 	  cch;

    char	  szBuf[256];

    /* Get handle (near address) to filename data in window's property list */
    pch = (PSTR)GetProp (hwnd, PROP_FILENAME);

    /* Get the text from the dialog's edit control into this address */
    GetDlgItemText (hwnd, IDD_FILENAME, pch, 64);

    if ( IsWild (pch)){
	/* Select the directory and make a listing of the directories */
	DlgDirList(hwnd, (LPSTR)pch, IDD_DIRS, IDD_PATH, ATTR_DIRS);

	/* Obtain the filename-only part of the path in the edit control */
	for (pch2 = pch; *pch; pch++)
	    if (*pch == '\\' || *pch == ':')
		pch2 = pch + 1;

	/* List the files in this directory based on the wildcard. */
	DlgDirList(hwnd, (LPSTR)pch2, IDD_FILES, IDD_PATH, ATTR_FILES);

	/* Set the dialog's edit control to the filename part of path
	 * string.
	 */
	SetDlgItemText (hwnd, IDD_FILENAME, pch2);
    }
    else
    {
	/* The filename in the property list is not a wildcard */
	if (FileExists (pch)){

	    RemoveProp (hwnd, PROP_FILENAME);
	    EndDialog (hwnd, 0);
	}
	else{
	    MPError ( hwnd, MB_OK | MB_SYSTEMMODAL, IDS_CANTOPEN, (LPSTR) pch);
	    SetActiveWindow (hwnd);
	}
    }
}
/****************************************************************************
 *									    *
 *  FUNCTION   : FileOpenDlgProc()					    *
 *									    *
 *  PURPOSE    : Dialog function for the File/Open dialog. Takes care of    *
 *		 calling the appropriate functions for extracting the	    *
 *		 filename and wildcard, filling the listboxes and changing  *
 *		 the FILENAME item in the property list for the window.     *
 *									    *
 ****************************************************************************/

BOOL FAR PASCAL FileOpenDlgProc ( hwnd, message, wParam, lParam)
register HWND hwnd;
WORD	      message;
register WORD wParam;
LONG	      lParam;
{
    PSTR pch;

    switch (message){

	case WM_INITDIALOG:
	    /* Set the default file extension on edit window, and try to
	     * get a listing of the files and directories.
	     */
	    SetDlgItemText ( hwnd, IDD_FILENAME, DEFFILESEARCH);
	    SetProp (hwnd, PROP_FILENAME, LOWORD(lParam));
	    SendDlgItemMessage (hwnd, IDD_FILENAME, EM_LIMITTEXT, 64, 0L);
	    SelectFile (hwnd);
	    break;

	case WM_COMMAND:
	    switch (wParam){
		case IDOK:
		    SelectFile(hwnd);
		    break;

		case IDCANCEL:
		    /* Set the filename in the prop. list to NULL and quit */
		    pch  = (PSTR) GetProp (hwnd, PROP_FILENAME);
		    *pch = 0;
		    EndDialog (hwnd, 0);
		    break;

		case IDD_FILENAME:
		    /* Enable the OK button if the edit control has text. */
		    EnableWindow ( GetDlgItem (hwnd, IDOK),
				   GetWindowTextLength ((HWND)LOWORD (lParam)));
		    break;

		case IDD_FILES:

		    /* The files listbox. If file selection has changed, fill
		     * the new filename into the property list buffer and set
		     * text in edit control.
		     */
		    if (HIWORD(lParam) == LBN_SELCHANGE){
			pch = (PSTR) GetProp (hwnd, PROP_FILENAME);
			DlgDirSelect (hwnd, (LPSTR)pch, IDD_FILES);
			SetDlgItemText (hwnd, IDD_FILENAME, (LPSTR)pch);
		    }
		    else if (HIWORD(lParam) == LBN_DBLCLK)
			/* if the item was double-clicked, try to open it */
			SelectFile(hwnd);
		    break;

		case IDD_DIRS:

		    /* The directories listbox. Append current filename in edit
		     * control (stripped of the path prefix) to the name from
		     * the property list and set the new string in the edit
		     * control.
		     */
		    if (HIWORD(lParam) == LBN_SELCHANGE){

			PSTR pch2, pchT, pchS;

			pch = (PSTR) GetProp (hwnd, PROP_FILENAME);

			/* Get the new drive/dir */
			DlgDirSelect (hwnd, pch, IDD_DIRS);
			pch2 = pch + lstrlen(pch);

			/* Fetch current contents of dialog's edit control and append
			 * it to name from property list... */
			GetDlgItemText(hwnd,IDD_FILENAME,(LPSTR)pch2,64);
			if (*pch2 == 0){
			    SetDlgItemText(hwnd, IDD_FILENAME, DEFFILESEARCH);
			    GetDlgItemText(hwnd,IDD_FILENAME,(LPSTR)pch2,64);
			}
			else {
			    pchS = pch;
			    for (pchT = pch = pch2; *pch; pch++) {
				if (*pch == '\\' || *pch == ':')
				    pchT = pch2;
				else
				    *pchT++ = *pch;
			    }
			    *pchT = 0;
			    pch = pchS;
			}

			/* Set the edit control with new string */
			SetDlgItemText (hwnd, IDD_FILENAME, (LPSTR)pch);
		    }
		    else if (HIWORD(lParam) == LBN_DBLCLK)
			SelectFile (hwnd);
		    break;

		default:
		    return FALSE;
	    }
	    break;

	default:
	    return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : GetFilename ( pstr )					    *
 *									    *
 *  PURPOSE    : Gets a filename from the user by calling the File/Open     *
 *		 dialog.						    *
 *									    *
 ****************************************************************************/
VOID NEAR PASCAL GetFileName(PSTR pstr)
{
    FARPROC lpfn;

    lpfn = MakeProcInstance (FileOpenDlgProc, hInst);
    DialogBoxParam (hInst, IDD_FILEOPEN, hwndFrame, lpfn, MAKELONG(pstr,0));
    FreeProcInstance (lpfn);
}
