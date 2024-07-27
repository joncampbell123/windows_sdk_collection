/*
 * DlgOpen.c   Open dialog box functions for Sample file application.
 *--------------------------------------------------------------------*/

/*
 * NOTE: these routines control an Open dialog box which must have
 * an edit field (ID_EDIT), a path field (ID_PATH), a listbox (ID_LISTBOX),
 * an ok button (IDOK), and a cancel button (IDCANCEL).
 *
 * Also, the application using this module must export DlgFnOpen in
 * its .DEF file and must also import "declare.h" in its source file.
 *
 * Functions provided:
 *  DlgOpen - invokes the Open file dialog box.
 *  DlgFnOpen - function passed to windows to control the open file dialog box.
 *  DlgCheckOkEnable - enables ok button iff there is text in box.
 *  DlgAddCorrectExtension - adds extensions to filenames or search specs.
 *  DlgCheckFilename - checks to see if filename is legal.
 *  DlgSearchSpec - searches string for '*' or '?'.
 *
 *==========================================================================*/


#include "windows.h"
#include "sample.h"
#include "declare.h"

#define ATTRDIRLIST 0x4010  /* include directories and drives in listbox */

/* message box strings loaded in sample.c from stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[];

char szMsg[MAX_STR_LEN+MAX_FNAME_LEN];
char *pszAppname;
char *szExtSave;
char *szFileNameSave;
char szFileNameTemp[MAX_FNAME_LEN];
int  *pfpSave;
int   cbRootNameMax;
OFSTRUCT *rgbOpenSave;
OFSTRUCT rgbOpenTemp;

/*=============================================================================
 DLGOPEN is used to initialize vars & invoke the specified Open dialog box.
 Return value is either: NOOPEN, NEWOPEN, or OLDOPEN.  (described in DlgFnOpen).
 Also, rgbOpenIn, pfpIn, and szFileNameIn are changed to hold info about the
 file which is chosen and opened.  Inputs are commented below.
=============================================================================*/
int far DlgOpen(hInstance, hwndParent, idDlgIn, rgbOpenIn, pfpIn,
		szFileNameIn, szExtIn, pszAppnameIn)

HANDLE	hInstance;	/* app module instance handle */
HWND	hwndParent;	/* window handle of parent window */
int	idDlgIn;	/* open dialog box id */
OFSTRUCT *rgbOpenIn;	/* ptr to current file's ofstruct */
int	*pfpIn; 	/* ptr to file pointer (current file's handle) */
char	*szFileNameIn;	/* ptr to array which will get new file's name (no path) */
char	*szExtIn;	/* ptr to current default extension */
char	*pszAppnameIn;	/* ptr to application name */
{
    BOOL    fResult;
    FARPROC lpProc;

    rgbOpenTemp = *rgbOpenIn;
    rgbOpenSave = &rgbOpenTemp;
    pfpSave = pfpIn;
    lstrcpy((LPSTR)szFileNameTemp, (LPSTR)szFileNameIn);
    szFileNameSave = szFileNameTemp;
    szExtSave = szExtIn;
    pszAppname = pszAppnameIn;
    cbRootNameMax = MAX_FNAME_LEN - CBEXTMAX - 1; /* leave room for ext. */

    fResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgIn), hwndParent,
			lpProc = MakeProcInstance(DlgFnOpen, hInstance));

    FreeProcInstance(lpProc);
    if (fResult != NOOPEN)
	{
	*rgbOpenIn = rgbOpenTemp; /* copy new info into caller's ofstruct */
	lstrcpy((LPSTR)szFileNameIn, (LPSTR)DlgPFileInPath(szFileNameSave));
	}
    return fResult;

} /* end dlgopen */


/*=============================================================================
 DLGFNOPEN controls the Open dialog box.  The dialog box handles input, allows
 user to change directories, checks for legal filenames, opens specified files,
 appends default extension if needed, and returns the file's name and ofstruct.
 This routine correctly parses filenames containing KANJI characters.
 It returns to DlgOpen one of the following:
    NOOPEN  - user hits cancel.
    NEWOPEN - created new file.     (file left open)
    OLDOPEN - opened existing file. (file left open)
=============================================================================*/
int FAR PASCAL DlgFnOpen(hwnd, msg, wParam, lParam)
HWND hwnd;
unsigned msg;
WORD wParam;
LONG lParam;
{
    int item;
    char rgch[256];
    int cchFile, cchDir;
    char *pchFile;
    BOOL    fWild;

    switch (msg) {
    case WM_INITDIALOG:
	/* Set edit field with default search spec */
	SetDlgItemText(hwnd, ID_EDIT, (LPSTR)(szExtSave+1));
	/* Don't let user type more than cbRootNameMax bytes in edit ctl. */
	SendDlgItemMessage(hwnd, ID_EDIT, EM_LIMITTEXT, cbRootNameMax, 0L);

	/* fill list box with filenames that match spec, and fill static
	   field with path name */
	if (!DlgDirList(hwnd, (LPSTR)(szExtSave+1), ID_LISTBOX, ID_PATH, ATTRDIRLIST))
	    EndDialog(hwnd, NOOPEN);
	break;

    case WM_COMMAND:
	switch (wParam) {
	case IDOK:
	    if (IsWindowEnabled(GetDlgItem(hwnd, IDOK))) {
		/* Get contents of edit field */
		/* Add search spec if it does not contain one. */
		GetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave, cbRootNameMax);
		lstrcpy((LPSTR)rgch, (LPSTR)szFileNameSave);

		/* Append appropriate extension to user's entry */
		DlgAddCorrectExtension(rgch, TRUE);
		/* Try to open directory.  If successful, fill listbox with
		   contents of new directory.  Otherwise, open datafile. */
		if (DlgSearchSpec(rgch)) {
		    if (DlgDirList(hwnd, (LPSTR)rgch, ID_LISTBOX, ID_PATH, ATTRDIRLIST)){
			lstrcpy((LPSTR)szFileNameSave, (LPSTR)rgch);
			SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
			break;
		    }
		}

		DlgAddCorrectExtension(szFileNameSave, FALSE);
		/* If no directory list and filename contained search spec,
		   honk and don't try to open. */
		if (DlgSearchSpec(szFileNameSave)) {
		    MessageBeep(0);
		    break;
		}
LoadIt:
		/* Make filename upper case and if it's a legal dos
		   name, try to open the file. */
		AnsiUpper((LPSTR)szFileNameSave);
		if (!DlgCheckFilename(szFileNameSave)) {
		    /* illegal filename */
		    DlgMergeStrings(szIFN, szFileNameSave, szMsg);
		    MessageBox(hwnd, (LPSTR)szMsg, (LPSTR)pszAppname,
				MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		    break;
		}
		/* see if file already exists */
		*pfpSave = OpenFile((LPSTR)szFileNameSave,(LPOFSTRUCT)rgbOpenSave,  OF_READ);
		if (*pfpSave != -1) {
		    EndDialog(hwnd, OLDOPEN);
		    break;
		}

		/** file doesn't exist; create new one? */
		DlgMergeStrings(szFNF, szFileNameSave, szMsg);
		if (MessageBox(hwnd, (LPSTR)szMsg, (LPSTR)pszAppname,
		    MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
		    {
		    *pfpSave = OpenFile((LPSTR)szFileNameSave,
					(LPOFSTRUCT)rgbOpenSave, OF_CREATE);
		    if (*pfpSave != -1)
			EndDialog(hwnd, NEWOPEN);
		    else
			{
			DlgMergeStrings(szECF, szFileNameSave, szMsg);
			MessageBox(hwnd, (LPSTR)szMsg, (LPSTR)pszAppname,
				   MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			break;
			}
		    }
		break;
	    }
	    break;

	case IDCANCEL:
	    /* User pressed cancel.  Just take down dialog box. */
	    EndDialog(hwnd, NOOPEN);
	    break;


	/* User single clicked or doubled clicked in listbox -
	   Single click means fill edit box with selection.
	   Double click means go ahead and open the selection. */
	case ID_LISTBOX:
	    switch (HIWORD(lParam)) {

	    /* Single click case */
	    case 1:
		GetDlgItemText(hwnd, ID_EDIT, (LPSTR)rgch, cbRootNameMax);

		/* Get selection, which may be either a prefix to a new search
		   path or a filename. DlgDirSelect parses selection, and
		   appends a backslash if selection is a prefix */
		if (DlgDirSelect(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX)) {
		    cchDir = lstrlen((LPSTR)szFileNameSave);
		    cchFile = lstrlen((LPSTR)rgch);
		    pchFile = rgch+cchFile;

		    /* Now see if there are any wild characters (* or ?) in
		       edit field.  If so, append to prefix. If edit field
		       contains no wild cards append default search spec
		       which is  "*.TXT" for notepad. */
		    fWild = (*pchFile == '*' || *pchFile == ':');
		    while (pchFile > rgch) {
			pchFile = (char *)AnsiPrev((LPSTR)(rgch), (LPSTR)pchFile);
			if (*pchFile == '*' || *pchFile == '?')
			    fWild = TRUE;
			if (*pchFile == '\\' || *pchFile == ':') {
			    pchFile = (char *)AnsiNext((LPSTR)pchFile);
			    break;
			}
		    }
		    if (fWild)
			lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)pchFile);
		    else
			lstrcpy((LPSTR)szFileNameSave + cchDir, (LPSTR)(szExtSave+1));
		}

		/* Set edit field to entire file/path name. */
		SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);

		break;

	    /* Double click case - first click has already been processed
	       as single click */
	    case 2:
		/* Basically the same as ok.  If new selection is directory,
		   open it and list it.  Otherwise, open file. */
		if (DlgDirList(hwnd, (LPSTR)szFileNameSave, ID_LISTBOX, ID_PATH, ATTRDIRLIST)) {
		    SetDlgItemText(hwnd, ID_EDIT, (LPSTR)szFileNameSave);
		    break;
		}
		goto LoadIt;	/* go load it up */
	    }
	    break;

	case ID_EDIT:
	    DlgCheckOkEnable(hwnd, ID_EDIT, HIWORD(lParam));
	    break;

	default:
	    return(FALSE);
	}
    default:
	return FALSE;
    }
    return(TRUE);

} /* end dlgfnopen */


/*=============================================================================
 DLGCHECKOKENABLE enables the ok button in dialog box if and only if the edit
 field contains text.  Edit field must have id of ID_EDIT.
=============================================================================*/
VOID FAR DlgCheckOkEnable(hwnd, idEdit, message)
HWND	hwnd;
int	idEdit;
unsigned message;
{
    if (message == EN_CHANGE) {
	EnableWindow(GetDlgItem(hwnd, IDOK),
	     (SendMessage(GetDlgItem(hwnd, idEdit), WM_GETTEXTLENGTH, 0, 0L)));
    }
}


/*=============================================================================
 DLGADDCORRECTEXTENSION adds appropriate extension to filename, partial filename,
 search spec, or partial search spec.
==============================================================================*/
DlgAddCorrectExtension(szEdit, fSearching)
char	*szEdit;
BOOL	fSearching;
{
    register char    *pchLast;
    register char    *pchT;
    int ichExt;
    BOOL    fDone = FALSE;
    int     cchEdit;

    pchT = pchLast = (char *)AnsiPrev((LPSTR)szEdit, (LPSTR)(szEdit + (cchEdit = lstrlen((LPSTR)szEdit))));

    if ((*pchLast == '.' && *(AnsiPrev((LPSTR)szEdit, (LPSTR)pchLast)) == '.') && cchEdit == 2)
	ichExt = 0;
    else if (*pchLast == '\\' || *pchLast == ':')
	ichExt = 1;
    else {
	ichExt = fSearching ? 0 : 2;
	for (; pchT > szEdit; pchT = (char *)AnsiPrev((LPSTR)szEdit, (LPSTR)pchT)) {
	    /* If we're not searching and we encounter a period, don't add
	       any extension.  If we are searching, period is assumed to be
	       part of directory name, so go ahead and add extension. However,
	       if we are searching and find a search spec, do not add any
	       extension. */
	    if (fSearching) {
		if (*pchT == '*' || *pchT == '?')
		    return;
	    } else if (*pchT == '.'){
		return;
	    }
	    /* Quit when we get to beginning of last node. */
	    if (*pchT == '\\')
		break;
	}
	/* Special case hack fix since AnsiPrev can not return value less than
	   szEdit. If first char is wild card, return without appending. */
	if (fSearching && (*pchT == '*' || *pchT == '?'))
	    return;
    }
    lstrcpy((LPSTR)(pchLast+1), (LPSTR)(szExtSave+ichExt));

} /* end dlgaddcorrectextension */


/*===========================*/
BOOL FAR DlgCheckFilename(pch)	 /* check for legal filename */
/*===========================*/
register char	*pch;
{
    OFSTRUCT	ofT;
    return (OpenFile((LPSTR)pch, (LPOFSTRUCT)&ofT, OF_PARSE) == 0);
}


/*=================*/
BOOL DlgSearchSpec(sz) /* return TRUE iff 0 terminated str contains  '*' or '?' */
/*=================*/
register char	 *sz;
{
    for (; *sz;sz++) {
	if (*sz == '*' || *sz == '?')
	    return TRUE;
    }
    return FALSE;
}
