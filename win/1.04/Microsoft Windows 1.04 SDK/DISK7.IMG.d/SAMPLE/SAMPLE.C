/*
 * Sample.c    sample file application
 *------------------------------------------------*/

/*
 * This file contains the routines to handle a file menu box with
 * commands NEW, OPEN, SAVE, SAVE AS, and PRINT.  An added option, MODIFY FILE,
 * was added strictly so that the user could set a flag that the current
 * file has changed and thus see the prompting to save current changes
 * when certain actions are taken which would wipe out the changes.
 *
 *======================================================================*/


#include    "windows.h"
#include    "sample.h"
#include    "declare.h"

HWND	hWndSample;
HANDLE	hInstSample;

HCURSOR  hArrowCursor;
HCURSOR  hWaitCursor;

OFSTRUCT vofstruct;	    /* current file's ofstruct */
int	 fp;		    /* file pointer */

BOOL	fUntitled = TRUE;   /* current file untitled? */
BOOL	fChanged = FALSE;   /* current file modified? */

char	szCurFile[MAX_FNAME_LEN];   /* current file's name (w/o path) */
char	szTitle[MAX_FNAME_LEN+15];
char	szMessage[MAX_STR_LEN+MAX_FNAME_LEN];

/*** string arrays for using stringtable ***/
char	szAppName[MAX_STR_LEN];
char	szUntitled[MAX_STR_LEN];
char	szIFN[MAX_STR_LEN];	    /* illegal filename */
char	szFNF[MAX_STR_LEN];	    /* file not found	*/
char	szREF[MAX_STR_LEN];	    /* replace existing file */
char	szSCC[MAX_STR_LEN];	    /* save current changes */
char	szEOF[MAX_STR_LEN];	    /* error opening file */
char	szECF[MAX_STR_LEN];	    /* error creating file */
char	szExt[MAX_STR_LEN];

FARPROC lpprocSampleAboutDlg;	/* ptr to About dialog box function */


/*==============================================================*/
LONG FAR PASCAL SampleWndProc( hWindow, message, wParam, lParam )
/*==============================================================*/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
LONG	   lParam;
{
    switch ( message )
    {
    case WM_CREATE:
	hWndSample = hWindow;
	break;

    case WM_DESTROY:
	PostQuitMessage( 0 );
	break;

    case WM_SYSCOMMAND:
	switch (wParam)
	{
	case IDM_ABOUT:
	    DialogBox( hInstSample, MAKEINTRESOURCE(IDD_ABOUT), hWindow,
		      lpprocSampleAboutDlg );
	    break;
	default:
	    return( DefWindowProc( hWindow, message, wParam, lParam ) );
	    break;
	}
	break;

    case WM_COMMAND:
	SampleCommand( hWindow, wParam );
	return (long)TRUE;
	break;

    case WM_CLOSE:
	if (SampleCheckSave(hWindow) != IDCANCEL)
	    DestroyWindow(hWindow);
	break;

    case WM_QUERYENDSESSION:
	if (SampleCheckSave(hWindow) == IDCANCEL)
	    return (0); /* don't end session */
	else
	    return (1); /* end session */

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }
    return(0L);
}


/*=============================*/
SampleCommand( hWindow, wParam )
/*=============================*/
HWND	hWindow;
WORD	wParam;
{
    int      nresult;
    char     szNewName[MAX_FNAME_LEN];

    switch (wParam) {
	case IDM_NEW:
	    if (SampleCheckSave(hWindow) == IDCANCEL)
		break;
	    fUntitled = TRUE;
	    fChanged = FALSE;
	    lstrcpy((LPSTR)szCurFile, (LPSTR)szUntitled);
	    SampleUpdateTitle(hWindow);
	    break;

	case IDM_OPEN:
	    if (SampleCheckSave(hWindow) == IDCANCEL)
		break;
	    nresult = DlgOpen(hInstSample, hWindow, IDD_OPEN,
		      (OFSTRUCT *)&vofstruct, &fp, szNewName, szExt, szAppName);
	    if (nresult != NOOPEN) /* filename given */
		{
		lstrcpy((LPSTR)szCurFile, (LPSTR)szNewName);
		fUntitled = FALSE;
		SampleUpdateTitle(hWindow);
		fChanged = FALSE;
		/*** actual code or procedure call to read file would go here ***/
		_lclose(fp);
		}
	    break;

	case IDM_SAVE:
	    if (!fUntitled)
		{
		/*** fp = OpenFile((LPSTR)szCurFile, (LPOFSTRUCT)vofstruct, OF_WRITE); ***/
		/*** procedure call to write file would go here ***/
		/*** _lclose(fp); ***/
		fChanged = FALSE;
		break;
		}
	    /* else fall through... */
	case IDM_SAVEAS:
	    if (fUntitled)
		vofstruct.cBytes = 0; /* no current file flag to dlginitsaveas */
	    nresult = DlgSaveAs(hInstSample, hWindow, IDD_SAVEAS,
		      (OFSTRUCT *)&vofstruct, &fp, szNewName, szExt, szAppName);
	    if (nresult != NOSAVE) /* filename given */
		{
		lstrcpy((LPSTR)szCurFile, (LPSTR)szNewName);
		fUntitled = FALSE;
		SampleUpdateTitle(hWindow);
		/*** procedure call to write file would go here ***/
		_lclose(fp);
		fChanged = FALSE;
		}
	    break;

	case IDM_PRINT:
	    PrintFile(hInstSample, hWindow, IDD_ABORT, vofstruct, szCurFile, szAppName);
	    break;

	case IDM_MODIFY:	/* option added just for this sample app. */
	    fChanged = TRUE;	/* fChanged would normally be set true by */
	    break;		/* routines which changed the file's contents.*/

	}

} /* end samplecommand */


/*========================================================*/
BOOL FAR PASCAL SampleAboutDlg (hDlg, message, wParam, lParam)
/*========================================================*/
HWND	 hDlg;
unsigned message;
WORD	 wParam;
LONG	 lParam;
{
    switch (message) {
	case WM_INITDIALOG:
	    break;
	case WM_COMMAND:
	    EndDialog(hDlg, TRUE);
	    break;
	default:
	    return FALSE;
	    break;
	}
    return(TRUE);
}


/*=========================================================*/
int PASCAL WinMain( hInstance, hPrev, lpszCmdLine, cmdShow )
/*=========================================================*/
HANDLE	hInstance, hPrev;
LPSTR	lpszCmdLine;
int	cmdShow;
{
    MSG     msg;
    HMENU   hMenu;

    if (!hPrev) {
	if (!SampleInit( hInstance ))
	    return 0;
	}
    else {
	GetInstanceData( hPrev, (PSTR)&hArrowCursor,  sizeof( hArrowCursor  ) );
	}

    LoadString(hInstance, IDS_APPNAME, szAppName, MAX_STR_LEN);
    LoadString(hInstance, IDS_UNTITLED, szUntitled, MAX_STR_LEN);
    LoadString(hInstance, IDS_IFN, szIFN, MAX_STR_LEN); /* illegal filename */
    LoadString(hInstance, IDS_FNF, szFNF, MAX_STR_LEN); /* file not found   */
    LoadString(hInstance, IDS_REF, szREF, MAX_STR_LEN); /* replace existing file */
    LoadString(hInstance, IDS_SCC, szSCC, MAX_STR_LEN); /* save current changes */
    LoadString(hInstance, IDS_EOF, szEOF, MAX_STR_LEN); /* error opening file */
    LoadString(hInstance, IDS_ECF, szECF, MAX_STR_LEN); /* error creating file */
    LoadString(hInstance, IDS_EXT, szExt, MAX_STR_LEN); /* default file ext. */

    CreateWindow(
	(LPSTR)"sample",		    /** lpClassName **/
	(LPSTR)NULL,			    /** lpWindowName **/
	WS_TILEDWINDOW, 		    /** dwStyle **/
	0, 0, 0, 100,			    /** position of window **/
	(HWND)NULL,			    /** hWndParent (null for tiled) **/
	(HMENU)NULL,			    /** hMenu (null = classmenu) **/
	(HANDLE)hInstSample=hInstance,	    /** hInstance **/
	(LPSTR)NULL			    /** lpParam **/
	);

    ShowWindow( hWndSample, cmdShow );

    lpprocSampleAboutDlg  = MakeProcInstance((FARPROC)SampleAboutDlg, hInstSample);

    hMenu = GetSystemMenu(hWndSample, FALSE);
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, "About...",IDM_ABOUT, MF_APPEND | MF_STRING);

    lstrcpy((LPSTR)szCurFile, (LPSTR)szUntitled);
    SampleUpdateTitle(hWndSample);

    while( GetMessage( (LPMSG)&msg, NULL, 0, 0 ))
	{
	TranslateMessage( (LPMSG)&msg );
	DispatchMessage( (LPMSG)&msg );
	}

    exit(0);
}

/*===========================*/
BOOL SampleInit( hInstance )
/*===========================*/
HANDLE hInstance;
{
    PWNDCLASS	pWndClass;

    hArrowCursor = LoadCursor( NULL, IDC_ARROW );
    hWaitCursor = LoadCursor( NULL, IDC_WAIT);

    pWndClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pWndClass->hIcon	     = LoadIcon( hInstance, MAKEINTRESOURCE(ID_ICON));
    pWndClass->lpszMenuName  = (LPSTR)"sample" ;
    pWndClass->lpszClassName = (LPSTR)"sample";
    pWndClass->hCursor	     = hArrowCursor;
    pWndClass->hbrBackground = COLOR_WINDOW+1;
    pWndClass->hInstance     = hInstance;
    pWndClass->style	     = CS_VREDRAW | CS_HREDRAW;
    pWndClass->lpfnWndProc   = SampleWndProc;

    if (!RegisterClass( (LPWNDCLASS)pWndClass ))
	return( FALSE );

    LocalFree( (HANDLE)pWndClass );
    return( TRUE );
}

/*=============================================================================
 SAMPLECHECKSAVE will put up a message box asking the user if they want to save
 current changes to the file.  If the user does, SampleSave will be called.
 Returns either: IDYES, IDNO, or IDCANCEL.
=============================================================================*/
SampleCheckSave(hWindow)
HWND hWindow;
{
    int nchoice, nresult;
    char szNewName[MAX_FNAME_LEN];

    if (fChanged) /* the file has changed */
	{
	DlgMergeStrings(szSCC, szCurFile, szMessage);
	nchoice = MessageBox(hWindow, (LPSTR)szMessage, (LPSTR)szAppName,
			     MB_YESNOCANCEL | MB_ICONQUESTION | MB_APPLMODAL);

	if (nchoice == IDYES)
	    if (!fUntitled)
		{
		/*** fp = OpenFile((LPSTR)szCurFile, (LPOFSTRUCT)vofstruct, OF_WRITE); ***/
		/*** procedure call to write file would go here ***/
		/*** _lclose(fp); ***/
		fChanged = FALSE;
		}
	    else
		{
		vofstruct.cBytes = 0; /* no current file flag to dlginitsaveas */
		nresult = DlgSaveAs(hInstSample, hWindow, IDD_SAVEAS,
			  (OFSTRUCT *)&vofstruct, &fp, szNewName, szExt, szAppName);

		if (nresult != NOSAVE) /* filename given */
		    {
		    lstrcpy((LPSTR)szCurFile, (LPSTR)szNewName);
		    fUntitled = FALSE;
		    SampleUpdateTitle(hWindow);
		    /*** procedure call to write file would go here ***/
		    _lclose(fp);
		    fChanged = FALSE;
		    }
		}
	return (nchoice);  /* yes, no, cancel */
	}
    else
	return (IDNO);	/* return no if file wasn't changed */

} /* end Samplechecksave */


/*=======================*/
SampleUpdateTitle( hWindow )
/*=======================*/
HWND	hWindow;
{
    lstrcpy((LPSTR)szTitle, (LPSTR)szAppName);
    lstrcat((LPSTR)szTitle, (LPSTR)" - ");
    lstrcat((LPSTR)szTitle, (LPSTR)szCurFile);
    SetWindowText(hWindow, (LPSTR)szTitle);

}
