/************************************************************

	PROGRAM:	HFORM.C

	PURPOSE:
	
		A program demonstrating the handwriting edit control
		(Hedit).

	COMMENTS:

		HFORM has several edit fields, typical of a generic
		form application (name, address, city, etc.).  The
		application registers itself as a pen aware application
		so that the edit controls will be replaced by hedit
		controls.

		This application will run under Win 3.0.  When running
		under Win 3.0, the edit control functionality will be
		present.  The boxed edit field and the picture field
		will not be present.

		The Delay Recognition menu, when checked, will cause
		ink to be captured in the inking field.  When the
		menu item is unchecked, the first tap in the field
		will cause recognition.  After that, the field will
		behave like a normal HEDIT control.

		HFORM has an accelerator gesture (circle-'D') which
		automatically brings up the sample dialog box.

************************************************************/

#define WIN31
#include <windows.h>
#include <penwin.h>
#include "hfres.h"
#include "hform.h"

#define Unused(x)		x		/* To prevent CS5.1 warning message */


/* Initialization of globals */

char *	szHformClass		= "HformSampleClass";
char *	szHformWnd			= "Hform Sample";
BOOL fDelayRecog = FALSE;  /* Set to true if just capture ink */ 
BOOL fAppwideHookSet;
BOOL fHookIsSet = FALSE;
char *	szHookErr			= "Could not set hook";
HANDLE hPenWin = NULL;
BOOL (FAR PASCAL *SetAppRecogHook)(UINT, UINT, HWND);
HANDLE hAccel;				   /* Menu Accelerator Table */ 
HANDLE hwndMain;			   /* Parent window to all fields */ 
#define  cFieldsMax 7
#define  cFormWidth 180	   /* In dialog units */ 
#define  cFormHeight 220   /* In dialog units */ 

static FIELD rgfield[cFieldsMax] =	
	{
	{"Name:",       6,   8,  44,   8, 128, 10, 0, ALC_DEFAULT, FIELDEDIT, NULL},
	{"Address:",    6,  24,  44,  24, 128, 10, 0, ALC_DEFAULT, FIELDEDIT, NULL},
	{"City:",       6,  40,  44,  40,  84, 10, 0, ALC_DEFAULT, FIELDEDIT, NULL},
	{"Zip:",      132,  40, 147,  40,  25, 10, 0, ALC_NUMERIC | ALC_GESTURE, FIELDEDIT, NULL},
	{"Notes:",      6,  56,  44,  54, 130, 40, WS_BORDER | ES_MULTILINE , ALC_DEFAULT, FIELDEDIT, NULL},
	{"Tel #:",      6, 102,  44,  98, BXD_CELLWIDTH*10+1, BXD_CELLHEIGHT, 0, ALC_NUMERIC | ALC_GESTURE, FIELDBEDIT, NULL},
	{"Directions:", 6, 120,  44, 118, 130, 70, WS_BORDER | ES_MULTILINE, ALC_DEFAULT, FIELDPIC, NULL}
	};


int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
						LPSTR lpszCommandLine, int cmdShow)
/***************************************************************

FUNCTION:	WinMain(hInstance, hPrevInstance, lpszCommandLine, cmdShow)

PURPOSE:
	
	Main Windows application function.

***************************************************************/
	{
	MSG	msg;
	VOID (FAR PASCAL *RegisterPenApp)(UINT, BOOL) = NULL;

	Unused(lpszCommandLine); 
	if (!hPrevInstance)
		{
		if (!FInitApp(hInstance))
			{
			return 1;
			}
		}

   /* If running on a Pen Windows system, register this app so all
		EDIT controls in dialogs are replaced by HEDIT controls.
		(Notice the CONTROL statement in the RC file is "EDIT",
		RegisterPenApp will automatically change that control to
		an HEDIT. */ 

	if ((hPenWin = GetSystemMetrics(SM_PENWINDOWS)) != NULL)
		{
	   /* We do this fancy GetProcAddress simply because we don't
			know if we're running Pen Windows. */ 
		if ( ((FARPROC)RegisterPenApp = GetProcAddress(hPenWin, "RegisterPenApp"))!= NULL)
			(*RegisterPenApp)(RPA_DEFAULT, TRUE);
		(FARPROC)SetAppRecogHook = GetProcAddress(hPenWin, "SetRecogHook");
		}

	if (FInitInstance(hInstance, hPrevInstance, cmdShow))
		{
		while (GetMessage((LPMSG)&msg,NULL,0,0) )
			{
			/* Check for menu accelerator message */ 
			if (!TranslateAccelerator(hwndMain, hAccel, &msg))
				{
				TranslateMessage((LPMSG)&msg);
				DispatchMessage((LPMSG)&msg);
				}
			}
		}
	else
		msg.wParam = 0;

   /* Unregister this app */ 
	if (RegisterPenApp != NULL)
			(*RegisterPenApp)(RPA_DEFAULT, FALSE);

	return msg.wParam;
	}



BOOL NEAR PASCAL FInitApp(HANDLE hInstance)
/***************************************************************

FUNCTION:	FInitApp(hInstance)

PURPOSE:
	
	Initialize application data and register window class.
	To be called only once.

	Returns FALSE if function could not register window class.
	TRUE if successful.

***************************************************************/
	{
	WNDCLASS	wc;
	HCURSOR	hcursor;
	
	hcursor = LoadCursor(NULL, IDC_ARROW);

	/* Register Main window class */

	wc.hCursor = hcursor;
	wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(iconHform));
	wc.lpszMenuName = MAKEINTRESOURCE(menuMain);
	wc.lpszClassName = (LPSTR)szHformClass;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW ;
	wc.lpfnWndProc = HformWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return RegisterClass((LPWNDCLASS) &wc);
	}


BOOL NEAR PASCAL FInitInstance(HANDLE hInstance, HANDLE hPrevInstance,
										int cmdShow)
/***************************************************************

FUNCTION:	FInitInstance(hInstance, hPrevInstance, cmdShow)

PURPOSE:
	
	Initialize all data structures for program instance and create
	the necessary windows.

	Returns TRUE if successsful, FALSE if failed.

***************************************************************/
	{
	int	i;
	LONG	lT				= GetDialogBaseUnits();
	int	cxDlgBase	= LOWORD(lT);
	int	cyDlgBase	= HIWORD(lT);

	Unused(hPrevInstance);

	/* Convert field coordinates to window coordinates */

	for (i = 0; i < cFieldsMax; i++)
		{
		rgfield[i].xStatic = (rgfield[i].xStatic * cxDlgBase)/4;
		rgfield[i].yStatic = (rgfield[i].yStatic * cyDlgBase)/8;
		rgfield[i].xEdit = (rgfield[i].xEdit * cxDlgBase)/4;
		rgfield[i].yEdit = ((rgfield[i].yEdit-(hPenWin ? 0 : 2)) * cyDlgBase)/8;
		rgfield[i].cxEdit = (rgfield[i].cxEdit * cxDlgBase)/4;
		rgfield[i].cyEdit = ((rgfield[i].cyEdit+(hPenWin ? 0 : 2)) * cyDlgBase)/8;
		}

	/* Create Main window */

	if (hwndMain = CreateWindow((LPSTR)szHformClass, 
		(LPSTR)szHformWnd,	  
		WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU,
		CW_USEDEFAULT, 0, (cFormWidth*cxDlgBase)/4, (cFormHeight*cyDlgBase)/8,
		(HWND)NULL,
		(HWND)NULL,
		(HANDLE)hInstance,
		(LPSTR)NULL
		))

		{

		hAccel = LoadAccelerators (hInstance, IDHFORM);

		if (hPenWin && SetAppRecogHook)
			{
		   /* Set a hook so our app window can get recognition results
			 	before the HEDIT control does.  We do this for an 
			 	accelerator gesture. */ 
			if(!(fHookIsSet = (*SetAppRecogHook)(HWR_APPWIDE, HKP_SETHOOK, hwndMain)))
				{
				MessageBox(NULL, szHookErr, szHformWnd, MB_ICONSTOP|MB_OK);
				DestroyWindow(hwndMain);
				hwndMain = NULL;
				}
			}

		ShowWindow(hwndMain, cmdShow);
		UpdateWindow(hwndMain);
		}

   /* If not running under Pen Win, disable delayed recognition option */ 
	if (hwndMain != NULL && !hPenWin)
		EnableMenuItem(GetMenu(hwndMain), miDelayRecog, MF_GRAYED|MF_BYCOMMAND);


	return hwndMain != NULL;
	}





LONG FAR PASCAL HformWndProc(HWND hwnd, UINT message,
			   WPARAM wParam, LPARAM lParam)
/***************************************************************

FUNCTION:	HformWndProc(hwnd, message, wParam, lParam)

PURPOSE:
	
	Windows procedure for application's parent window.

***************************************************************/
	{
	int		i;
	LONG	lRet	= 0L;

	static HWND	hwndFocusField = NULL;

	switch (message)
		{
		case WM_CREATE:
			/* Create fields */

			if (!FCreateForm(hwnd))
				{
				lRet = 1L;	/* Failed */
				break;	
				}

			/* Initialize focus to first edit control */

			hwndFocusField = rgfield[0].hwnd;
			SetFocus(hwndFocusField);
			break;

		case WM_COMMAND:
			{
			/* Edit control commands */

			if (HIWORD(lParam) == EN_SETFOCUS)
				{
				/* Field focus is being set */
	
				hwndFocusField = LOWORD(lParam);
				break;
				}

			/* Menu commands */

			switch (wParam)
				{
				case miExit:
					DestroyWindow(hwnd);
					break;

				case miClearAll:
					for (i = 0; i < cFieldsMax; i++)
						{
						if (rgfield[i].hwnd != NULL)
							{
						   /* Clear picture.  Forces edit back into picture mode */ 
							if (rgfield[i].wFieldType==FIELDPIC && fDelayRecog)
								SendMessage(rgfield[i].hwnd, WM_HEDITCTL, HE_SETINKMODE , (LONG)0L);
							else 
								SendMessage(rgfield[i].hwnd, WM_SETTEXT, 0, (LONG)(LPSTR)"");
							}
						}
					SetFocus(rgfield[0].hwnd);
					break;

				case miSampleDlg:
					SampleDialog(hwnd);
					break;

				case miDelayRecog:
					fDelayRecog = !fDelayRecog;
					ModifyMenu (GetMenu(hwndMain), miDelayRecog, MF_BYCOMMAND|MF_STRING, miDelayRecog, (LPSTR)(fDelayRecog ? szRecog : szDelay));
					{
					SetFocus(rgfield[0].hwnd);
					for (i = 0; i < cFieldsMax; i++)
						{
						if (rgfield[i].hwnd != NULL && rgfield[i].wFieldType==FIELDPIC)
							{
							if (fDelayRecog)
								{
							   /* Place control in delayed recognition mode */ 
								SendMessage(rgfield[i].hwnd, WM_HEDITCTL, HE_SETINKMODE , (LONG)0L);
								}
							else
								{
							   /* Send message to hedit to recognize data */ 
								SendMessage(rgfield[i].hwnd, WM_HEDITCTL, HE_STOPINKMODE , (LONG)HEP_RECOG);
								}
							}
						}
					}
					break;

				case miNextField:
				   /* Focus on the next field */ 
					ProcessFieldChange(hwndFocusField, (WORD) chNextField);
					break;

				case miPrecField:
				   /* Set Focus on the preceeding field */ 
					ProcessFieldChange(hwndFocusField, (WORD) chPrecField);
					break;

				default:
					break;
				}
			break;
			}

		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			HDC			hdc;

			hdc = BeginPaint(hwnd, &ps);
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkMode(hdc, TRANSPARENT);

			for (i = 0; i < cFieldsMax; i++)
				{
				PFIELD pfield = &rgfield[i];

				TextOut(hdc, pfield->xStatic, pfield->yStatic, pfield->szTitle,
					lstrlen(pfield->szTitle));
				}

			EndPaint(hwnd, &ps);
			break;
			}
		case WM_HOOKRCRESULT:	   /* getting recognition results */ 
			{
			LPRCRESULT lpr = (LPRCRESULT)lParam;
			SYV syv;
			WORD wResultsType = lpr->wResultsType;

			/* Does this result contain:
			 a gesture which has been translated by the gesture mapper
			 or an untranslated gesture */

			if(wResultsType & (RCRT_GESTURE|RCRT_GESTURETRANSLATED|RCRT_GESTURETOKEYS))
				{
				/* Get it from the results returned by the recognizer
				   lpr->lpsyv contains results which could have been
				   translated by the gesture mapper
				   Hence we get the original recognition results */

				syv = lpr->syg.lpsye->syv;

			   /* This is an example of an accelerator gesture.  The
				 	user writes a circle with a 'D' (or 'd') inside,
				 	and we look for this gesture.  

					However, if the user through the gesture mapper has mapped
					circle d to something else, then we don't need to do
					anything here. */

				if(syv == SyvAppGestureFromLoAnsi('d')
					|| syv == SyvAppGestureFromUpAnsi('D'))
					{
					PostMessage(hwnd, WM_COMMAND, miSampleDlg, 0L);

				   /* Let target window know result has already been acted upon */ 

					SetAlreadyProcessed(lpr);

					lRet = 1L;
					}
				}
			}
			break;

		case WM_SETFOCUS:
			SetFocus(hwndFocusField);
			break;

		case WM_DESTROY:
			{
			if (hPenWin)
				{
				if(SetAppRecogHook && fHookIsSet)
					(*SetAppRecogHook)(HWR_APPWIDE, HKP_UNHOOK, hwnd);
				}

			PostQuitMessage(0);
			break;
			}

		default:
			lRet = DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}

	return lRet;
	}


BOOL NEAR PASCAL FCreateForm(HWND hwndParent)
/***************************************************************

FUNCTION:	FCreateForm(hwndParent)

PURPOSE:
	
	This function creates the fields.

	Returns TRUE is successful, FALSE if field hedit window could
	not be created.

***************************************************************/
	{
	int		i;
	RC	rcin;
	LONG		lT		= GetDialogBaseUnits();
	int		cx		= LOWORD(lT)/2;
	int		cy		= HIWORD(lT)/2;
	RECTOFS	rectofs;

	for (i = 0; i < cFieldsMax; i++)
		{
		PFIELD pfield = &rgfield[i];
		DWORD dwStyle = WS_VISIBLE | WS_CHILD | (hPenWin ? 0L : WS_BORDER) | pfield->dwEditStyle;

		switch (pfield->wFieldType)
			{
		case FIELDPIC:
		case FIELDEDIT:
			/* Create Hedit window. */
			/* If running on a Pen Windows system, this app will have
				been registered, so all EDIT controls will be changed to
				HEDIT controls */

			pfield->hwnd = CreateWindow(
					(LPSTR)"edit",
					(LPSTR)NULL,
					dwStyle,
					pfield->xEdit,
					pfield->yEdit,
					pfield->cxEdit,
					pfield->cyEdit,
					hwndParent,	
					(HMENU)NULL,
					GetWindowWord(hwndParent, GWW_HINSTANCE),
					(LPSTR)NULL);

			break;

		case FIELDBEDIT:	 /* Comb field */ 
			pfield->hwnd = CreateWindow(
					(LPSTR)(hPenWin ? "bedit" : "edit"),
					(LPSTR)NULL,
					dwStyle,
					pfield->xEdit,
					pfield->yEdit,
					pfield->cxEdit,
					pfield->cyEdit,
					hwndParent,	
					(HMENU)NULL,
					GetWindowWord(hwndParent, GWW_HINSTANCE),
					(LPSTR)NULL);
			break;
		}
			
		if (!pfield->hwnd)
			{
			continue;
			}

		/* Set RC preferences for this edit control */

		if (hPenWin)
			{
			if (SendMessage(pfield->hwnd, WM_HEDITCTL, HE_GETRC, (LONG)((LPRC)&rcin)))
				{
				rcin.alc = pfield->alc;
				SendMessage(pfield->hwnd, WM_HEDITCTL, HE_SETRC, (LONG)((LPRC)&rcin));
				}

			/* Change default inflation rectangle offset so it is 
				half the base dialog unit for each respective axis. */

			rectofs.dLeft = -cx;
			rectofs.dTop = -cy;
			rectofs.dRight = cx;
			rectofs.dBottom = cy;
			SendMessage(pfield->hwnd, WM_HEDITCTL, HE_SETINFLATE, (LONG)((LPRECTOFS)&rectofs));

		   /* If no border, put under line in. */ 
			if ((pfield->dwEditStyle & ES_MULTILINE) == 0 && pfield->wFieldType==FIELDEDIT)
				SendMessage(pfield->hwnd, WM_HEDITCTL, HE_SETUNDERLINE, (LONG)(1));
			}

		}

	return TRUE;
	}


VOID NEAR PASCAL SampleDialog(HWND hwnd)
/***************************************************************

FUNCTION:	SampleDialog()

PURPOSE:
	
	Brings up a sample dialog containing an EDIT (not HEDIT) control.
	If we're running under Pen Windows system, then RegisterPenApp
	has been called, in which case the EDIT will act like an HEDIT.

***************************************************************/
	{
	FARPROC lpSampleDlg;
	HANDLE hinstance=GetWindowWord(hwnd, GWW_HINSTANCE);

	lpSampleDlg = MakeProcInstance((FARPROC) SampleDlgProc, hinstance);
	DialogBox(hinstance, "SampleH",	hwnd, lpSampleDlg);
	FreeProcInstance(lpSampleDlg);
	}


HANDLE FAR PASCAL SampleDlgProc(HWND hdlg, WORD message, WORD wParam, LONG lParam)
/***************************************************************

FUNCTION:	SampleDlgProc(hdlg, message, wParam, lParam)

PURPOSE:
	
	Dialog window procedure.

***************************************************************/
	{
	char szDlgString[cbSzDlgMax];

	Unused(lParam);

	switch (message)
		{
		case WM_COMMAND:
			{
			switch (wParam)
				{
            case IDOK:
					*szDlgString = 0;
					GetDlgItemText(hdlg, IDC_EDIT, szDlgString, cbSzDlgMax);
					if (*szDlgString != 0)
						{
	           		EndDialog(hdlg, 1);
               	return TRUE;
						}
					break;	/* Don't end dialog with empty box */

            case IDCANCEL:
               EndDialog(hdlg, 0);
					return TRUE;
				}
			}
			break;

      case WM_INITDIALOG:		
      	SetDlgItemText(hdlg, IDC_EDIT, "Sample Name");
			/* Set focus to edit field and select all text */
      	SetFocus(GetDlgItem(hdlg, IDC_EDIT));
			SendDlgItemMessage(hdlg, IDC_EDIT, EM_SETSEL, 0, MAKELONG(0,-1)); 
      	break;
		}
	return FALSE;
	}

	
BOOL NEAR PASCAL ProcessFieldChange (HWND hwndFocusField,  WORD wParam)	
/***************************************************************

FUNCTION:	ProcessFieldChange (HWND hwndFocusField,  WORD wParam)

PURPOSE:
	
	Set the focus on either the next or previous field based on
	the value of wParam. wParam is can be set to chNextField or
	chPrecField.  The hwndFocusField parameter is assigned the value
	of the newly focused field.

	Returns TRUE if successful, FALSE otherwise. 

***************************************************************/
	{
  	int i, inc, iCount;
	WORD wRet = FALSE;
	LONG lInkData;
	
	if ((wParam != chNextField) && (wParam != chPrecField))
		return FALSE;

	inc = wParam == chPrecField ? cFieldsMax-1 : 1;
	i = (IFromHwnd(hwndFocusField)+inc) %(cFieldsMax);


	/*
		To determine if an hedit is in cold recognition mode, send a GETINKHANDLE
		message to the control. If the loword of the return code is NULL, then the
		control is not in	cold recognition mode, and the focus can be set. If it
		is in cold mode, skip over it and check the next field.
		iCount is used to break the loop if all fields are in cold mode.
	*/

	iCount=0;
	while (iCount<cFieldsMax && !wRet)
		{
		if (!LOWORD(SendMessage(rgfield[i].hwnd, WM_HEDITCTL, HE_GETINKHANDLE,(LONG)(LPSTR)&lInkData)))
			{
			hwndFocusField = rgfield[i].hwnd;
			SetFocus(hwndFocusField);
			wRet = TRUE;
			}
		else
			i = (i+inc) %(cFieldsMax);		 /* Calculate the next field */ 
		iCount++;
		}

	return wRet;
	}



int NEAR PASCAL IFromHwnd(HWND hwnd)
/***************************************************************

FUNCTION:	IFromHwnd(HWND hwnd)

PURPOSE:
	
	Returns the index into the rgfield which corresponds to
	the entry containing parameter hwnd.

	Returns 0 if a match is not found.

***************************************************************/
	{
	register int i;
	LPFIELD lpfield;

	for (lpfield = rgfield, i = 0; i < cFieldsMax; i++, lpfield++)
		if (lpfield->hwnd == hwnd)
			return i;

	return 0;	 /* default on err */ 
	}
