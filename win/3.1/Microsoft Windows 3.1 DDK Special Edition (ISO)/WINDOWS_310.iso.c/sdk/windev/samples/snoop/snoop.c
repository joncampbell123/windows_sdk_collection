#define WIN31
#include <windows.h>
#include <penwin.h>
#include <penwoem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "snoop.h"


/************************* Module Data **************************************/

//Class name
static char szSnoopClassMain[] = "SnoopyClass";

//Window Caption
static char szAppName[] = "SnoopyWindow";

//Name of penwindows dll
static char szPenWin[] = "penwin.dll";

//Instance handle
static HANDLE hInstanceCur;

//Main window handle
static HANDLE hwndWindow;

static TEXTMETRIC tm;

static HANDLE hwndGraph, hwndResult;
static FARPROC lpfnGraph, lpfnResult, lpfnRoHeditProc;

HCURSOR hcurAltSelect;

//Ink data copy 
static HPENDATA hpendataCopy;

//Ink width
static int nInkWidth;

//Ink color
static DWORD rgbInk;

//TRUE if hook has been set
static BOOL fHookSet;

//TRUE if the current display is to be retained
static BOOL fHold = FALSE;

//Buffers used in translation
static char szSyeBuff[2048];
static char szSyvBuff[2048];

//Mapping table for Gesture Strings
static SYVID rgGestures[]=
	{
	SYV_EXTENDSELECT,	" ExtendSelect! ",
	SYV_UNDO,			" Undo! ",
	SYV_COPY,			" Copy! ",
	SYV_CUT,				" Cut! ",
	SYV_PASTE,			" Paste! ",
	SYV_CLEARWORD,		" Delete Words!",
	SYV_CORRECT,		" Correct! ",
	SYV_BACKSPACE,		" Backspace! ",
	SYV_SPACE,			" Space! ",
	SYV_RETURN,			" Return! ",
	SYV_TAB,				" Tab! ",
	SYV_CLEAR, 			" Delete!",
	};

// Prefix for displaying circled letter gestures
static char szCircleG[] = "Circle - ";

//Annotation strings
static char szGraph[] = "Graph";
static char szResult[] = "Result";

// Ink display offset
static POINT ptInk;

static POINT grTOrg = {0, 2};

static POINT grOrg;
static POINT grExt;

static POINT resOrg;
static POINT resExt;

static POINT resTOrg;

static int grXText, resXText;

static char szInitError[] = "Error during init process";

/****************************************************************************/

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
	LPSTR lpszCommandLine, int cmdShow)
/*******************************************************************

FUNCTION:	WinMain(hInstance, hPrevInstance, lpszCommandLine, cmdShow)

PURPOSE:		Windows Entry Point

*******************************************************************/
	{
	MSG msg;

	// register classes, init objects, set globals, prefs, etc:
	if (!FInitSnoop(hInstance, hPrevInstance))
		exit(-1);

	// Initialize the application instance
	if (FInitInstance(hInstance, hPrevInstance, cmdShow))
		{
		while (GetMessage((LPMSG)&msg, NULL, 0, 0) )
			{
			TranslateMessage((LPMSG)&msg);
			DispatchMessage((LPMSG)&msg);
			}
		}
	else
		msg.wParam = (WPARAM)-1;

	return msg.wParam;
	}

BOOL PASCAL FInitSnoop(HANDLE hInstance, HANDLE hPrevInstance)
/****************************************************************

FUNCTION:	FInitSnoop(hInstance, hPrevInstance)

PURPOSE:	   Init for all instances of the application

COMMENTS:	Register the main window class for the first
				instance of the program.

*****************************************************************/
	{
	WNDCLASS wndClass;

	hInstanceCur = hInstance;	// set global

	if (!hPrevInstance)
		{
		// Main Window
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hIcon = LoadIcon(hInstance, (LPSTR)"iconSnoop");
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = (LPSTR)szSnoopClassMain;
		wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW+1;
		wndClass.hInstance = hInstance;
		wndClass.style = CS_VREDRAW | CS_HREDRAW;
		wndClass.lpfnWndProc = SnoopWndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		if (!RegisterClass((LPWNDCLASS) &wndClass))
			return FALSE;
		}
    return TRUE;
	}

BOOL PASCAL FInitInstance(HANDLE hInstance, HANDLE hPrevInstance, int cmdShow)
/*******************************************************************************

FUNCTION:	FInitInstance(hInstance, hPrevInstance, cmdShow)

PURPOSE:		Initialize this instance of the application

COMMENTS:	
				Create main window. Set the hook so that all recognition
				results are delivered to this instance before being
				passed on to the recipient.

********************************************************************************/
	{
	HANDLE hPenWin;

	//Create the main window
   hwndWindow = CreateWindow((LPSTR)szSnoopClassMain,
                             (LPSTR)szAppName,
                             WS_OVERLAPPEDWINDOW,
                             0, 0, GetSystemMetrics(SM_CXFULLSCREEN),
                             7 * GetSystemMetrics(SM_CYCAPTION),
                             (HWND)NULL,
                             (HMENU)LoadMenu(hInstance, MAKEINTRESOURCE(menuSnoop)),
                             (HANDLE)hInstance,
                             (LPSTR)NULL);

   if (!hwndWindow)
       return FALSE;

	// Calculate the relative positions of all items on the screen
	CalcTopology(hwndWindow);

	// Create the EDIT control to show the Symbol Graph
	hwndGraph = CreateWindow("EDIT", NULL
										, WS_CHILDWINDOW|WS_BORDER|WS_VISIBLE|ES_AUTOHSCROLL|ES_READONLY,
										grOrg.x, grOrg.y,
										grExt.x, grExt.y,
										hwndWindow,
										idGraph,
										(HANDLE)hInstance,
										(LPSTR)NULL);

	// Create the EDIT control to show the recognition result
	hwndResult = CreateWindow("EDIT", NULL
										, WS_CHILDWINDOW|WS_BORDER|WS_VISIBLE|ES_AUTOHSCROLL|ES_READONLY
										,resOrg.x, resOrg.y
										,resExt.x, resExt.y
										,hwndWindow
										,idResult
										,(HANDLE)hInstance
										,NULL);


	//Set the results hook
   fHookSet = SetRecogHook(HWR_RESULTS, HKP_SETHOOK, hwndWindow);

	//If any of these steps failed bail out
	if(!hwndGraph || !hwndResult || !fHookSet)
		{
		MessageBox(hwndWindow, szInitError, NULL, MB_ICONEXCLAMATION|MB_OK);
		DestroyWindow(hwndWindow);
		return(FALSE);
		}

	// Subclass the controls to show alternate cursor

	lpfnRoHeditProc = MakeProcInstance((FARPROC)RoHeditProc, hInstanceCur);

	lpfnGraph = (FARPROC)SetWindowLong(hwndGraph, GWL_WNDPROC, (LPARAM)lpfnRoHeditProc);
	lpfnResult = (FARPROC)SetWindowLong(hwndResult, GWL_WNDPROC, (LPARAM)lpfnRoHeditProc);

	// Load the alternate cursor from penwin.dll
	hPenWin = LoadLibrary(szPenWin);
	hcurAltSelect = LoadCursor(hPenWin, IDC_ALTSELECT);
	FreeLibrary(hPenWin);

	ShowWindow(hwndWindow, cmdShow);
   UpdateWindow(hwndWindow);

   return(TRUE);
	}

long FAR PASCAL SnoopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
/***************************************************************************

FUNCTION:	SnoopWndProc(hwnd, message, wParam, lParam)

PURPOSE:		Windows message handling procedure

COMMENTS:
				Also handles WM_HOOKRCRESULT which the penwin.dll sends
				to every window that has set the hook.

***************************************************************************/
	{
	HDC hDC;
	long lRet = 1L;
	static long lCall=0;
	LPRCRESULT lpResT;
	PAINTSTRUCT ps;

	switch (message)
		{
		case WM_CREATE:
			EnableMenuItem(GetMenu(hwnd), miRelease, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			break;
		case WM_SIZE:
			{
			//Recalculate the width of both the edit controls
			grExt.x = resExt.x = LOWORD(lParam) - (max(grXText, resXText)+5) - 5;
			grExt.y = resExt.y = (int)(1.5 * tm.tmHeight);

			//Resize them
			if(hwndGraph)
				MoveWindow(hwndGraph, grOrg.x, grOrg.y, grExt.x, grExt.y, TRUE);
			if(hwndResult)
				MoveWindow(hwndResult, resOrg.x, resOrg.y, resExt.x, resExt.y, TRUE);
			}
			break;
		case WM_PAINT:
			if(hpendataCopy)
				{
				hDC = BeginPaint(hwnd, &ps);
				SetBkMode(hDC, TRANSPARENT);
				SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));

				//Draw annotation text
				TextOut(hDC, grTOrg.x, grTOrg.y, szGraph, _fstrlen(szGraph));
				TextOut(hDC, resTOrg.x, resTOrg.y, szResult, _fstrlen(szResult));

				//Show the ink exactly the user drew it
				RedisplayPenData(hDC, hpendataCopy, &ptInk, NULL, nInkWidth, rgbInk);

				EndPaint(hwnd, &ps);
				}
			break;

		case WM_HOOKRCRESULT:
         {
			PENDATAHEADER pendataheader;

			// Break if we are holding the results of earlier recognition
			if (fHold)
				break;

			//Get the pointer to the recognition results structure
			lpResT = (LPRCRESULT)lParam;

			//Convert the result into ANSI string
			SyvToStr(lpResT->lpsyv, lpResT->cSyv, szSyvBuff, sizeof(szSyvBuff)-1, FALSE);

			//Show it in the Result EDIT control
			SetWindowText(hwndResult, szSyvBuff);

			//Convert the symbol graph into ANSI string
			SyvToStr(lpResT->syg.lpsye, lpResT->syg.cSye, szSyeBuff, sizeof(szSyeBuff)-1, TRUE);

			//Show it in the Graph EDIT control
			SetWindowText(hwndGraph, szSyeBuff);

			//Delete any older pen data
			if (hpendataCopy)
				DestroyPenData(hpendataCopy);

			// Make a copy of the pendata
			hpendataCopy = DuplicatePenData(lpResT->hpendata, 0);

			//Save the ink color
			rgbInk = lpResT->lprc->rgbInk;

			//and the width
			nInkWidth = lpResT->lprc->nInkWidth;

			GetPenDataInfo(hpendataCopy, &pendataheader, NULL, 0);

			// Offset the pendata so all points are wrt. the top left
			//corner of it's bounding rect which is at 0, 0
			OffsetPenData(hpendataCopy, -pendataheader.rectBound.left
								,-pendataheader.rectBound.top);

			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			break;
         }

		case WM_CLOSE:
			//Remove the hook
			if(fHookSet)
				SetRecogHook(HWR_RESULTS, HKP_UNHOOK, hwnd);

			//Destroy the edit controls
			if(hwndGraph)
				DestroyWindow(hwndGraph);
			if(hwndResult)
				DestroyWindow(hwndResult);

			if(lpfnRoHeditProc)
				FreeProcInstance(lpfnRoHeditProc);

			if(hcurAltSelect)
				DestroyCursor(hcurAltSelect);

			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			if (FDispatchMenu(hwnd, wParam, lParam))
            return 1;
            break;

		case WM_DESTROY:
			break;

		default:
			break;
		}

	return DefWindowProc(hwnd, message, wParam, lParam);
	}

VOID PASCAL SyvToStr(LPVOID lpData, int cItem, LPSTR lpstr, int cMax, BOOL fSye)
/*******************************************************************************

FUNCTION:	SyvToStr(lpData, cItem, lpstr, cMax, fSye)

PURPOSE:		Converts from SYV to form to ANSI in order to display

COMMENTS:
				Converts meta symbols SYV_BEGINOR, SYV_ENDOR and SYV_OR
				to '{', '}' and '|' respectively.
				Also converts gestures to strings

*******************************************************************************/
	{
	int i, j, cLen;
	SYV syvT;
	char buff[33];

	for(i=j=0; i< cItem; ++i)
		{
		syvT = (fSye)?((LPSYE)lpData)[i].syv : ((LPSYV)lpData)[i];

		cLen = 1;
		switch(syvT)
			{
		case SYV_BEGINOR:
			lpstr[j] = '{';
			break;
		case SYV_ENDOR:
			lpstr[j] = '}';
			break;
		case SYV_OR:
			lpstr[j] = '|';
			break;
		case SYV_EMPTY:
			lpstr[j] = '_';
			break;
		case SYV_SPACENULL:
			cLen = 0;
			break;
		case SYV_SOFTNEWLINE:
			lpstr[j] = (char)ANSI_PARA_MARK;      // parragraph mark
			break;

		default:
			//If this is a gesture
			if(FIsGesture(syvT))
				{
				//Convert to string
				cLen = IGestureString(syvT, buff, sizeof(buff)-1);

				//And append it to the output buffer.
				if(cLen)
					{
					cLen = ((cLen + j) < cMax)?cLen:(cMax-j-1);
					if(cLen>0)
						_fstrncpy(&lpstr[j], buff, cLen);
					}	
				}
			//This could just be a SYV for an ANSI character
			else if(!SymbolToCharacter(&syvT, 1, &lpstr[j], NULL))
				lpstr[j] = '?';
			
			if (lpstr[j] == '\0')
				cLen = 0;
			else if (lpstr[j] == ANSI_NEW_LINE)  	// New Line
				lpstr[j] = (char)ANSI_PARA_MARK; // parragraph mark
			}
		j += cLen;
		}
		lpstr[j] = '\0';
	}


int PASCAL IGestureString(SYV syvGes, LPSTR lpstr, int cMac)
/*****************************************************************************

FUNCTION:	IGestureString(syvGes, lpstr, cMac)

PURPOSE:		Convert the SYV for a gesture to a corresponding string

COMMENTS:

		the rgGestures table above maps the SYVs to the corresponding strings

*****************************************************************************/
	{
	int i, cLen=0;
	BOOL	fFound = FALSE;

	// If this a Circled letter gesture
	if (FIsAppGesture(syvGes))
		{
	// Form a string with "circle -"  prefix

		_fstrncpy(lpstr, szCircleG, sizeof(szCircleG));
		if (syvGes >= SYV_CIRCLEUPA && syvGes <= SYV_CIRCLEUPZ)
			lpstr[sizeof(szCircleG) - 1] = (BYTE)LOWORD(syvGes - SYV_CIRCLEUPA + 'A');
		else 
			lpstr[sizeof(szCircleG) - 1] = (BYTE)LOWORD(syvGes - SYV_CIRCLELOA + 'a');
		lpstr[sizeof(szCircleG)] = '\0';
		cLen = sizeof(szCircleG);
		}
	else
		{
	//Must be a command gesture, look in the table
		for(i=0; i<(sizeof(rgGestures)/sizeof(SYVID)); ++i)
			{
			if(syvGes == rgGestures[i].syv)
				{
				cLen = _fstrlen(rgGestures[i].lpstr);
				cLen = min(cLen, cMac);
				_fstrncpy(lpstr, rgGestures[i].lpstr, cLen);
				fFound = TRUE;
				break;
				}
			}

		}
	return(cLen);
	}


BOOL PASCAL FDispatchMenu(HWND hwnd, WORD wParam, long lParam)
/*****************************************************************************

FUNCTION:	FDispatchMenu(hwnd, wParam, lParam)

PURPOSE:		Executes the menu commands

COMMENTS:

*****************************************************************************/
	{
	BOOL fOk = hwnd != NULL;

	if (fOk)
		{
		switch (wParam)
			{
			case miHold:
				fHold = TRUE;
				EnableMenuItem(GetMenu(hwnd), miHold, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); 
				EnableMenuItem(GetMenu(hwnd), miRelease, MF_BYCOMMAND | MF_ENABLED);
				DrawMenuBar(hwnd);
				break;

			case miRelease:
				fHold = FALSE;
				EnableMenuItem(GetMenu(hwnd), miRelease, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); 
				EnableMenuItem(GetMenu(hwnd), miHold, MF_BYCOMMAND | MF_ENABLED);
				DrawMenuBar(hwnd);
				break;

			default:
				fOk = FALSE;
				break;
			}
        }
	return fOk;
	}

VOID PASCAL CalcTopology(HWND hwndWindow)
/*****************************************************************************

FUNCTION:	VOID CalcTopology(hwndWindow)

PURPOSE:		To calculate the relative positions of various items
				on the screen

COMMENTS:

*****************************************************************************/
	{
	HDC hDC;

	hDC = GetDC(hwndWindow);

	//Get the textmetrics for the default font in the device context
	GetTextMetrics(hDC, &tm);

	grXText = LOWORD(GetTextExtent(hDC, szGraph, _fstrlen(szGraph)));
	resXText = LOWORD(GetTextExtent(hDC, szResult, _fstrlen(szResult)));

	grOrg.y = grTOrg.y;
	grOrg.x = grTOrg.x + max(grXText, resXText)+5;

	resOrg.x = grOrg.x;
	resOrg.y = (int)(grOrg.y + 1.5 * tm.tmHeight+5);

	resTOrg.x = grTOrg.x;
	resTOrg.y = resOrg.y;

	// Setup the negative offset for the pendata origin.
	// The x and y values are chosen so that the ink is displayed
	// just below the Hedit control showing the results

	ptInk.x = -grOrg.x;
	ptInk.y = (int)(-(resTOrg.y+1.5 * tm.tmHeight+4));

	ReleaseDC(hwndWindow, hDC);
	}

/***************************************************************************

FUNCTION:	RoHeditProc(hwnd, message, wParam, lParam)

PURPOSE:		Subclassed message handling procedure for the two edit controls.

COMMENTS:
				Handles only thw WM_SETCURSOR message. This is done in order
				not to show the pen cursor in a readonly edit control, to
				indicate that the control cannot be written into.
***************************************************************************/
long FAR PASCAL RoHeditProc(HWND hwnd, unsigned message, WORD wParam, long lParam)
	{
	switch(message)
		{
		case WM_SETCURSOR:
			SetCursor(hcurAltSelect);
			//Tell windows we took care of it
			return(0L);
		}
	// Pass on all the other messages to the original procedure(s)
	CallWindowProc((hwnd==hwndGraph)?lpfnGraph:lpfnResult, hwnd, message, wParam, lParam);
	}
