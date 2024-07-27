/* Driver for the HP7470A plotter. */

#include    "plotters.h"
#include    "hp7470A.h"


LPSTR	    far PASCAL lstrcpy( LPSTR, LPSTR );
MODEBLOCK   hp7470AModeBlock;
BOOL	    hp7470ADevModeBusy = FALSE;


BOOL FAR PASCAL HP7470ADialogFn(HWND, unsigned, WORD, DWORD);


extern	DWORD	hp747xAPenColors[];

INTEGER FAR PASCAL
hp7470ADeviceMode(hWnd, hInstance, lpDeviceType, lpOutputFile)
HANDLE		    hWnd;
HANDLE		    hInstance;
LPSTR		    lpDeviceType;
LPSTR		    lpOutputFile;
{

    /* make sure we don't do two devmodes for this driver at once */
    if(hp7470ADevModeBusy)
	return(FALSE);
    else
	hp7470ADevModeBusy = TRUE;

    /* default */
    if(!GetEnvironment(lpOutputFile, (LPSTR)&hp7470AModeBlock, sizeof(hp7470AModeBlock))
	 || lstrcmp((LPSTR)lpDeviceType, (LPSTR)&hp7470AModeBlock)){

#ifdef SPOOLING
	hp7470AModeBlock.Spooling     = TRUE;
#endif
	hp7470AModeBlock.PenSpeed     = FASTSPEED;
	hp7470AModeBlock.Orientation  = LANDSCAPE;
	hp7470AModeBlock.PaperType    = APAPER;
	hp7470AModeBlock.PenColors[0] = hp747xAPenColors[hp7470AModeBlock.PenIndexes[0] = LREDPEN - LBLACKPEN];
	hp7470AModeBlock.PenColors[1] = hp747xAPenColors[hp7470AModeBlock.PenIndexes[1] = RBLACKPEN - RBLACKPEN];

    }

    if(DialogBox(hInstance, (LPSTR)"HP7470AdtMODE", hWnd, (FARPROC)HP7470ADialogFn) == IDCANCEL){

	hp7470ADevModeBusy = FALSE;
	return(FALSE);
    }

    lstrcpy((LPSTR)&hp7470AModeBlock, (LPSTR)"HP 7470A");

    SetEnvironment(lpOutputFile, (LPSTR)&hp7470AModeBlock, sizeof(hp7470AModeBlock));

    SendMessage((HWND)(-1), WM_DEVMODECHANGE, 0, (DWORD)(LPSTR)lpDeviceType);
    hp7470ADevModeBusy = FALSE;

    return(TRUE);
}



/* A menu item has been selected, or a control is notifying
** its parent.	wParam is the menu item value (for menus),
** or control ID (for controls).  For controls, the low word
** of lParam has the window handle of the control, and the hi
** word has the notification code.  For menus, lParam contains
** 0L. */

BOOL FAR PASCAL HP7470ADialogFn(hDB, message, wParam, lParam)
HWND		    hDB;
unsigned	    message;
WORD		    wParam;
DWORD		    lParam;
{

    switch(message){

	case WM_INITDIALOG:
	    hp7470aSetChecks(hDB, 1);
	    hp7470aSetChecks(hDB, 2);
	    hp7470aSetChecks(hDB, 3);

	    CheckDlgButton(hDB, hp7470AModeBlock.PenSpeed, TRUE);
	    CheckDlgButton(hDB, hp7470AModeBlock.Orientation, TRUE);

#ifdef SPOOLING
/* ---------------------------------------------------------------------------
	    CheckDlgButton(hDB,(hp7470AModeBlock.Spooling ? SETSPOOLING : RESETSPOOLING), TRUE);
--------------------------------------------------------------------------- */
#endif

	    SetFocus(GetDlgItem(hDB, hp7470AModeBlock.PenIndexes[0] + LBLACKPEN));
	    return(FALSE);
	    break;

	case WM_COMMAND:
	    switch (wParam){

#ifdef SPOOLING
/* ---------------------------------------------------------------------------
		case SETSPOOLING:
		case RESETSPOOLING:
		    CheckDlgButton(hDB, (hp7470AModeBlock.Spooling ? SETSPOOLING : RESETSPOOLING), FALSE);

		    CheckDlgButton(hDB, wParam, TRUE);

		    hp7470AModeBlock.Spooling = (wParam == SETSPOOLING);

		    break;
--------------------------------------------------------------------------- */
#endif

		case SLOWSPEED:
		case FASTSPEED:
		    CheckDlgButton(hDB, hp7470AModeBlock.PenSpeed, FALSE);

		    CheckDlgButton(hDB, hp7470AModeBlock.PenSpeed = wParam, TRUE);

		    break;

		case LANDSCAPE:
		case PORTRAIT:
		    CheckDlgButton(hDB, hp7470AModeBlock.Orientation, FALSE);

		    CheckDlgButton(hDB, hp7470AModeBlock.Orientation = wParam, TRUE);

		    break;

		case APAPER:
		case A4PAPER:
		    hp7470AModeBlock.PaperType = wParam;
		    hp7470aSetChecks(hDB, 1);
		    break;

		case LBLACKPEN:
		case LREDPEN:
		case LGREENPEN:
		case LBLUEPEN:
		case LYELLOWPEN:
		case LVIOLETPEN:
		case LTURQUOISEPEN:
		case LORANGEPEN:
		case LBROWNPEN:

		case RBLACKPEN:
		case RREDPEN:
		case RGREENPEN:
		case RBLUEPEN:
		case RYELLOWPEN:
		case RVIOLETPEN:
		case RTURQUOISEPEN:
		case RORANGEPEN:
		case RBROWNPEN:
		    hp7470AModeBlock.PenIndexes[wParam/10 - 2] = wParam - (wParam/10)*10;

		    hp7470AModeBlock.PenColors[wParam/10 - 2] = hp747xAPenColors[wParam - (wParam/10)*10];

		    hp7470aSetChecks(hDB, wParam/10);
		    break;

		case IDOK:
		    EndDialog(hDB, IDOK);
		    break;

		case IDCANCEL:
		    EndDialog(hDB, IDCANCEL);
		    break;

		default:
		    return(FALSE);
		    break;
	    }
	    return(TRUE);
	    break;
    }

    return(FALSE);
}



hp7470aSetChecks(hDB, Section)
HWND	    hDB;
INTEGER     Section;
{
    INTEGER	Index;
    INTEGER	Base;

    switch(Section){

	case 1:
	    CheckDlgButton(hDB, APAPER, FALSE);
	    CheckDlgButton(hDB, A4PAPER, FALSE);

	    CheckDlgButton(hDB, hp7470AModeBlock.PaperType, TRUE);

	    break;

	case 2:
	case 3:
	    Base = Section*10;
	    for(Index = Base; Index <= Base + LBROWNPEN - LBLACKPEN; Index++){
		if(Index == (hp7470AModeBlock.PenIndexes[Section-2] + Base))
		    CheckDlgButton(hDB, Index, TRUE);
		else
		    CheckDlgButton(hDB, Index, FALSE);
	    }
	    break;
    }
}
