#include <generic.h>
#include <resource.h>

DEVMODE devmode;

void showme() {}

BOOL FAR PASCAL DialogFn(HWND, unsigned, WORD, LONG);
int         far PASCAL lstrcmp( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcpy( LPSTR, LPSTR );
void FAR PASCAL GetDeviceMode(LPSTR, DEVMODE far *);
void FAR PASCAL SaveDeviceMode(LPSTR, DEVMODE far *);
FAR PASCAL isUSA();
int  FAR PASCAL DialogBox(HANDLE, LPSTR, HWND, FARPROC);
void FAR PASCAL EndDialog(HWND, int);
long FAR PASCAL SendMessage(HWND, unsigned, WORD, LONG);
HWND FAR PASCAL SetFocus(HWND);
void   FAR PASCAL SetWindowText(HWND, LPSTR);
int    FAR PASCAL GetWindowText(HWND, LPSTR, int);
HWND FAR PASCAL GetDlgItem(HWND, int);
void FAR PASCAL CheckDlgButton(HWND, int, WORD);
void FAR PASCAL CheckRadioButton(HWND, int, int, int);

FAR PASCAL DeviceMode(hWnd, hInst, lpDeviceType, lpOutputFile)
HANDLE hWnd, hInst;
LPSTR lpDeviceType;
LPSTR lpOutputFile;
{
        static BOOL DevModeBusy = FALSE;

        /* make sure we don't do two devmodes for this driver at once */
        if (DevModeBusy)
            return(FALSE);
        else
            DevModeBusy = TRUE;

	/* try to get devmode from environment */
        if ((short) !GetEnvironment(lpOutputFile, (LPSTR) &devmode, sizeof(DEVMODE))
                || lstrcmp((LPSTR)lpDeviceType, (LPSTR)devmode.DeviceName))

		GetDeviceMode((LPSTR)lpDeviceType, (DEVMODE FAR *)&devmode);

	showme();
	
#if defined(EPSON) || defined(IBMGRX)
     if (((lstrcmp((LPSTR)"Epson FX-80", (LPSTR)devmode.DeviceName)) == 0) ||
       (lstrcmp((LPSTR)"Okidata 92/192 (IBM)",(LPSTR)devmode.DeviceName) == 0))
	{
            if (DialogBox(hInst,	/* use the non-wide carriage dialog */
                    (LPSTR) "dtLETTER",
                    hWnd,
                    (FARPROC) DialogFn) == IDCANCEL)
                {
                DevModeBusy = FALSE;
                return FALSE;
                }
	    devmode.use_wide_carriage = 0;			
/* preset this variable; just in case somebody set up a profile string 
   in win.ini -- I don't write the string if it is a non-wide carriage,
   but I don't skip the GetProfileString either (MitchL - 10/28/87) 
 */        
	}
	else
#endif
	{
            if (DialogBox(hInst,
                    (LPSTR) "dtMODE",
                    hWnd,
                    (FARPROC) DialogFn) == IDCANCEL)
                {
                DevModeBusy = FALSE;
                return FALSE;
                }	
	}
	
        SetEnvironment(lpOutputFile, (LPSTR) &devmode, sizeof(DEVMODE));
        SendMessage(-1, WM_DEVMODECHANGE, 0,(LONG)(LPSTR) lpDeviceType);

	SaveDeviceMode((LPSTR)lpDeviceType, (DEVMODE FAR *)&devmode);

        DevModeBusy = FALSE;
        return TRUE;
}

/* A menu item has been selected, or a control is notifying
** its parent.  wParam is the menu item value (for menus),
** or control ID (for controls).  For controls, the low word
** of lParam has the window handle of the control, and the hi
** word has the notification code.  For menus, lParam contains
** 0L. */

BOOL FAR PASCAL DialogFn(hDB, message, wParam, lParam)
HWND hDB;
unsigned message;
WORD wParam;
LONG lParam;
{
    char    WindowText[WINDOW_TEXT_LENGTH];
    char    FAR * lpStr;
    short   i;

    switch(message)
    {
    case WM_INITDIALOG:
        i = 0;
        lpStr = devmode.DeviceName;
        while ((WindowText[i] = *lpStr++) != '\0')
            i++;
        GetWindowText(hDB, (LPSTR)&WindowText[i], WINDOW_TEXT_LENGTH - i);

        SetWindowText(hDB, (LPSTR)WindowText);
        CheckDlgButton(hDB, devmode.orient, TRUE);
	
#if DEVMODE_WIDEPAPER	/* set check mark  to represent initial state */   
        CheckDlgButton(hDB, PAPERWIDTH, devmode.use_wide_carriage);
  #if 0	/* Decided to allow all 6 paper choices - MitchL 10/22/87 */
/* set paper to default to letter */
        if (devmode.use_wide_carriage)
            CheckDlgButton(hDB, (devmode.paper=LETTER), TRUE);		
        else
  #endif
#endif
            CheckDlgButton(hDB, devmode.paper, TRUE);

#if DEVMODE_NO_PRINT_QUALITY
#else
        CheckDlgButton(hDB, devmode.res, TRUE);
#endif
#if COLOR
	CheckDlgButton(hDB, devmode.color, TRUE);
#endif
        SetFocus(GetDlgItem(hDB, devmode.paper));
        break;

    case WM_COMMAND:
        switch (wParam)
        {
        case PORTRAIT:
        case LANDSCAPE:
            CheckRadioButton(hDB, PORTRAIT, LANDSCAPE, devmode.orient = wParam);
            break;

#if DEVMODE_WIDEPAPER
	case PAPERWIDTH:
	    CheckDlgButton(hDB, PAPERWIDTH, (devmode.use_wide_carriage = 
					     !devmode.use_wide_carriage));
	    break;
#endif

        case LETTER:
        case DINA4:
        case FANFOLD:
            CheckRadioButton(hDB, LETTER, FANFOLD, devmode.paper =wParam);
            break;

#if DEVMODE_NO_PRINT_QUALITY
#else
        case HIGH:
        case LOW:
            CheckRadioButton(hDB, HIGH, LOW, devmode.res = wParam);
            break;
#endif

#if COLOR
	case BLACK_COLOR:
	case ALL_COLOR:
	    CheckRadioButton(hDB, ALL_COLOR, BLACK_COLOR, devmode.color = wParam);
	    break;
#endif

        case IDOK:
        case IDCANCEL:
            EndDialog(hDB, wParam);
            return TRUE;

        default:
            return FALSE;
        }
        break;
    }
    return FALSE;
}
