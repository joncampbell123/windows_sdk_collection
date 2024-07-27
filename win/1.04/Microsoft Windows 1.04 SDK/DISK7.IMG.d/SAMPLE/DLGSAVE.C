/*
 * DlgSave.c   Save As dialog box functions for Sample file application.
 *----------------------------------------------------------------------*/

/*
 * NOTE: these routines control a SaveAs dialog box which must have
 * an edit field (ID_EDIT), a path field (ID_PATH), an ok button (IDOK),
 * and a cancel button (IDCANCEL).
 *
 * Also, the application using this module must export DlgFnSaveAs in
 * its .DEF file and must also include "declare.h" in its source file.
 *
 * Functions provided:
 *  DlgSaveAs	    - invokes the saveas dialog box.
 *  DlgFnSaveAs     - function passed to windows to control the saveas dialog box.
 *  DlgInitSaveAs   - initializes the edit and path fields in the box.
 *  DlgPFileInPath  - gets the filename out of a fully qualified filename.
 *  DlgAddExt	    - adds current extension to filenames.
 *  DlgMergeStrings - combines strings to use in message boxes.
 *
 *======================================================================*/

#include "windows.h"
#include "sample.h"
#include "declare.h"

/* message box strings loaded in sample.c from the stringtable */
extern char szIFN[], szFNF[], szREF[], szSCC[], szEOF[], szECF[];

char szDlgMsg[MAX_STR_LEN+MAX_FNAME_LEN];
char *pszExt;
char *pszAppName;
char *pszNewName;
char szTempName[MAX_FNAME_LEN];
OFSTRUCT *pOfstr;
OFSTRUCT ofstrtemp;
int *pFp;

/*=============================================================================
 DLGSAVEAS is used to initialize vars & invoke the specified Save As Dialog Box.
 Return value is either: NOSAVE, OLDSAVE, or NEWSAVE. (described in DlgFnSaveAs).
 Also, pFpIn, pOfstrIn, and pszNewName will be modified to hold info for the
 chosen file.  Inputs are commented below.
=============================================================================*/
int FAR DlgSaveAs(hInstance, hWndParent, idDlgBox, pOfstrIn, pFpIn,
		  pszNewNameIn, pszExtIn, pszAppNameIn)

HANDLE hInstance;	/* app module instance handle */
HWND   hWndParent;	/* window handle of parent window */
int    idDlgBox;	/* SaveAs dialog box id */
OFSTRUCT *pOfstrIn;	/* ptr to current file OFSTRUCT (->cBytes=0 if no cur. file)*/
int   *pFpIn;		/* ptr to file pointer */
char  *pszNewNameIn;	/* ptr to array which will get new file's name (no path) */
char  *pszExtIn;	/* ptr to current default extension */
char  *pszAppNameIn;	/* ptr to application name */
{
    FARPROC lpProc;
    int     fResult;

    ofstrtemp	= *pOfstrIn;
    pOfstr	= &ofstrtemp;
    pFp 	= pFpIn;
    lstrcpy((LPSTR)szTempName, (LPSTR)pszNewNameIn);
    pszNewName	= szTempName;
    pszExt	= pszExtIn;
    pszAppName	= pszAppNameIn;

    fResult = DialogBox(hInstance, MAKEINTRESOURCE(idDlgBox), hWndParent,
		      lpProc = MakeProcInstance(DlgFnSaveAs, hInstance));

    FreeProcInstance(lpProc);
    if (fResult != NOSAVE)
	{
	*pOfstrIn = ofstrtemp; /* copy new info into caller's ofstruct */
	lstrcpy((LPSTR)pszNewNameIn, (LPSTR)DlgPFileInPath(szTempName));
	}
    return (fResult);

} /* end dlgsaveas */


/*=============================================================================
 DLGFNSAVEAS controls the save dialog box.  It handles input, tests for legal
 filenames, and uses message boxes to report problems.	It returns to DlgSaveAs
 either: NOSAVE  - user hits cancel button.
	 OLDSAVE - user wants to save over existing file. (file left open)
	 NEWSAVE - user wants to save to a new file.	  (file left open)
=============================================================================*/
BOOL FAR PASCAL DlgFnSaveAs (hDlg, message, wParam, lParam)
HWND	 hDlg;
unsigned message;
WORD	 wParam;
LONG	 lParam;
{
    switch (message)
	{
	case WM_INITDIALOG:
	    /*** limit text to max filename length - size for ext ***/
	    SendDlgItemMessage(hDlg, ID_EDIT, EM_LIMITTEXT, MAX_FNAME_LEN-CBEXTMAX, 0L);
	    /*** initialize dialog box with current filename ***/
	    DlgInitSaveAs(hDlg, pOfstr);
	    break;

	case WM_COMMAND:
	    switch (wParam)
		{
		case IDOK:
		    if (IsWindowEnabled(GetDlgItem(hDlg, IDOK)))
			{
			GetDlgItemText(hDlg, ID_EDIT, (LPSTR)pszNewName,
					MAX_FNAME_LEN-CBEXTMAX);
			AnsiUpper((LPSTR)pszNewName);
			DlgAddExt(pszNewName);

			if (!DlgCheckFilename(pszNewName)) /* illegal filename */
			    {
			    DlgMergeStrings(szIFN, pszNewName, szDlgMsg);
			    MessageBox(hDlg, (LPSTR)szDlgMsg, (LPSTR)pszAppName,
				MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			    }
			else { /* see if file already exists; overwrite ok? */
			    *pFp = OpenFile((LPSTR)pszNewName, (LPOFSTRUCT)pOfstr, OF_EXIST);
			    if (*pFp >= 0) /* already exists */
				{
				DlgMergeStrings(szREF, pszNewName, szDlgMsg);
				if (MessageBox(hDlg, (LPSTR)szDlgMsg, (LPSTR)pszAppName,
				     MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION
				     | MB_APPLMODAL) == IDNO)
					return TRUE;   /* don't overwrite */
				*pFp = OpenFile((LPSTR)pszNewName, (LPOFSTRUCT)pOfstr, OF_WRITE);
				if (*pFp == -1)
				    {
				    DlgMergeStrings(szEOF, pszNewName, szDlgMsg);
				    MessageBox(hDlg, (LPSTR)szDlgMsg, (LPSTR)pszAppName,
					    MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
				    break;
				    }
				EndDialog(hDlg, OLDSAVE);
				}
			    *pFp = OpenFile((LPSTR)pszNewName, (LPOFSTRUCT)pOfstr, OF_CREATE);
			    if (*pFp == -1)
				{
				DlgMergeStrings(szECF, pszNewName, szDlgMsg);
				MessageBox(hDlg, (LPSTR)szDlgMsg, (LPSTR)pszAppName,
					MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
				break;
				}
			    EndDialog(hDlg, NEWSAVE);
			    }
			}
		    break;

		case IDCANCEL:
		    EndDialog(hDlg, NOSAVE);
		    break;

		case ID_EDIT:
		    /* enable save button if edit field is non null */
		    DlgCheckOkEnable(hDlg, ID_EDIT, HIWORD(lParam));
		    break;

		default:
		    return FALSE;
		} /* end switch wparam */
	default:
	    return FALSE;
	} /* end switch message */

	return TRUE;

} /* end dlgsaveasdlg */


/*=============================================================================
 DLGINITSAVEAS will initialize the save as dialog box.	It puts the current dir
 string in the ID_PATH field and initializes the edit box with the proposed
 filename to save.  The proposed filename is null if pof->cBytes==0, or if cur-
 rent dir==path in *pof, its the filename part of *pof, otherwise its fully qualified.
=============================================================================*/
VOID DlgInitSaveAs(hDlg, pof)
HWND	hDlg;	/* Handle to save as dialog box. */
OFSTRUCT *pof;	/* ptr to open file structure used by open file.  */
{

    char    rgch[150];
    LPSTR   lpchFileName;
    LPSTR   lpchCmp;
    LPSTR   lpchTest;
    int     cchCurDir;

    /* Initialize Path field with current directory */
    DlgDirList(hDlg, (LPSTR)0, 0, ID_PATH, 0);

    /* Initialize EditField */
    if (pof->cBytes != 0) {
	/* rgch gets current directory string, terminated with "\\\0" */
	cchCurDir = GetDlgItemText(hDlg, ID_PATH, (LPSTR)rgch, 128);
	if (rgch[cchCurDir-1] != '\\') {
	    rgch[cchCurDir] = '\\';
	}
	rgch[++cchCurDir] = 0;

	/* Now see if path in reopen buffer matches current directory. */
	for (lpchFileName = pof->szPathName,
	     lpchTest = DlgPFileInPath(pof->szPathName),
	     lpchCmp = rgch;
	     lpchFileName < lpchTest;
	     lpchFileName = AnsiNext(lpchFileName), lpchCmp = AnsiNext(lpchCmp)) {

	    if (*lpchFileName != *lpchCmp)
		break;
	}
	/* If paths don't match, reset pchFileName to point to fully qualified
	   path. (Otherwise, lpchFileName already points to filename. */
	if (lpchFileName != lpchTest || *lpchCmp)
	    lpchFileName = pof->szPathName;
	SetDlgItemText(hDlg, ID_EDIT, (LPSTR)lpchFileName);

    /* pof->cBytes == 0 => edit field empty, so disable ok button. */
    } else {
	EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
    }

} /* end dlginitsaveas */


/*=============================================================================
 DLGPFILEINPATH returns the filename of the given filename which may include path.
=============================================================================*/
char * FAR DlgPFileInPath(sz)
char *sz;
{
    char    *pch;

    /* Strip path/drive specification from name if there is one */
    pch = (char *)AnsiPrev((LPSTR)sz, (LPSTR)(sz + lstrlen((LPSTR)sz)));
    while (pch > sz) {
	pch = (char *)AnsiPrev((LPSTR)sz, (LPSTR)pch);
	if (*pch == '\\' || *pch == ':') {
	    pch = (char *)AnsiNext((LPSTR)pch);
	    break;
	}
    }
    return(pch);

} /* end dlgpfileinpath */


/*==========*/
DlgAddExt(sz)	/* if missing, add extension to filename */
/*==========*/
char	*sz;
{
    char *pch1;
    int  ch;

    pch1 = sz + lstrlen((LPSTR)sz);

    while ((ch = *pch1) != '.' && ch != '\\' && ch != ':' && pch1 > sz)
	pch1 = (char *)AnsiPrev((LPSTR)sz, (LPSTR)pch1);

    if (*pch1 != '.')
	lstrcat((LPSTR)sz, (LPSTR)(pszExt+2));

}  /* end dlgaddext */


/*=============================================================================
 DLGMERGESTRINGS scans string1 for merge spec (%%). If found, insert string2 at
 that point, and then append remainder of string1.  Result in string3.
==============================================================================*/
BOOL FAR DlgMergeStrings (szSrc, szMerge, szDst)
 char	*szSrc;
 char	*szMerge;
 char	*szDst;
 {
    char *pchSrc;
    char *pchDst;
    unsigned int wMerge;

    wMerge = '%';
    wMerge <<= 8;
    wMerge |= '%';

    pchSrc = szSrc;
    pchDst = szDst;

    /* Find merge spec if there is one. */
    while (*(unsigned *)pchSrc != wMerge) {
	*pchDst++ = *pchSrc;

	/* If we reach end of string before merge spec, just return. */
	if (!*pchSrc++)
	    return FALSE;

    }
    /* If merge spec found, insert sz2 there. (check for null merge string */
    if (szMerge) {
	while (*szMerge)
	    *pchDst++ = *szMerge++;

    }

    /* Jump over merge spec */
    pchSrc++,pchSrc++;


    /* Now append rest of Src String */
    while (*pchDst++ = *pchSrc++);
    return TRUE;

} /* end dlgmergestrings */
