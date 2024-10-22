/* 
 * go.c		
 *
 * "Go" -- Sample Editor using the MedGo handler.
 *
 */

#include <windows.h>
#include <wincom.h>
#include <mediaman.h>
#include "medgo.h"
#include "go.h"


/* globals */
char		szAppName[] = "Go";	// the name of the application
HANDLE		hInstApp = NULL;	// application instance handle
HWND		hwndApp = NULL;		// application main window
MEDTYPE		medtypePhys = 0L;	// Physical type for Load/Save




/* GoWndProc()
 *
 * The window procedure for the main application window.
 */
LONG FAR PASCAL GoWndProc(HWND hwnd, unsigned msg, WORD wParam, LONG lParam)
{
	PWinInfo		pWinInfo;
	PAINTSTRUCT		ps;
	RECT			rc;
	POINT			pt;
	FPMedUserMsgInfo	fpUserInfo;

	switch (msg)
	{

	case WM_CREATE:

		/* allocate the window instance data structure */
		pWinInfo = (PWinInfo) LocalAlloc(LPTR, sizeof(GoWinInfo));
		if (pWinInfo == NULL)
			return -1L;

		/* save the pointer onto the window */
		SetWindowWord(hwnd, 0, (WORD) pWinInfo);

		/* fill in some default values, to be in the no-resource state
		 */
		pWinInfo->medid = NULL;
		pWinInfo->fFileOp = FALSE;

		/* allocate a resource user for this window -- <hwnd> is
		 * the meduser instance data
		 */
		pWinInfo->meduser = medRegisterUser( hwnd, 0L );

		break;

	case WM_ERASEBKGND:

		/* no need to erase background -- we are painting it all */
		return 0L;

	case WM_PAINT:

		pWinInfo = GetWinInfo(hwnd);
		BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		pt.x = rc.right;
		pt.y = rc.bottom;
		AppPaint(ps.hdc, pWinInfo->medid, &ps.rcPaint, pt);
		EndPaint(hwnd, &ps);
		return 0L;

	case WM_CLOSE:

		/* check that the user has saved his work first */
		if (!ResourceAskToSave(hwnd))
			return 0L;

		/* free the current resource and call DefWindowProc()
		 * to destroy the window
		 */
		ResourceRelease(hwnd, TRUE);
		break;

	case WM_DESTROY:

		/* destroy any allocated stuff: the resource user,
		 * the instance data structure
		 */
		pWinInfo = GetWinInfo(hwnd);
		medUnregisterUser(pWinInfo->meduser);
		LocalFree((HANDLE) pWinInfo);

		/* put a non-valid value into the window word to
		 * cause any further attempts to access the window instance
		 * structure to GP fault.
		 */
		SetWindowWord(hwnd, 0, 0xffff);

		/* tell my app to die */
		PostQuitMessage(0);
		break;

	case WM_COMMAND:

		return AppCommand(hwnd, msg, wParam, lParam);

	case WM_INITMENUPOPUP:

		return InitMenuPopup(hwnd, wParam, lParam);
	
	case WM_LBUTTONDOWN:

		pWinInfo = GetWinInfo(hwnd);
		AppClick(hwnd, pWinInfo->medid, MAKEPOINT(lParam), FALSE);
		return 0L;
	
	case WM_RBUTTONDOWN:

		pWinInfo = GetWinInfo(hwnd);
		AppClick(hwnd, pWinInfo->medid, MAKEPOINT(lParam), TRUE);
		return 0L;

	/*****************************************************************
	 * APPLICATION CUSTOM MESSAGES
	 */
	case WM_SETMEDID:
		/* somewhere, I just told myself to use a new resource ID
		 * from now on -- call ResourceSet() to get the job done.
		 */
		ResourceSet(hwnd, lParam);
		return 0L;

	/*****************************************************************
	 * APPLICATION CUSTOM MESSAGES
	 */
	case MM_MEDNOTIFY:
		fpUserInfo = (FPMedUserMsgInfo)lParam;
		return( GoUserProc( hwnd, fpUserInfo->medid, 
			(MEDMSG)wParam, fpUserInfo->medinfo,
			fpUserInfo->lParam, fpUserInfo->dwInst ) );
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}





/* WinMain()
 *
 * Application entry point.
 */
int PASCAL WinMain(HANDLE hInst, HANDLE hPrev, LPSTR szCmdLine, WORD sw)
{
	MSG     	msg;		// a message from GetMessage()
	WNDCLASS	cls;		// the Go application window class
	PWinInfo	pWinInfo;	// Go window info. block pointer
	HANDLE		hInstMedGo;	// handle to MedGo DLL

	/* save instance handle for dialog boxes */
	hInstApp = hInst;

	if (hPrev == NULL)
	{
		/* register application display class */
		cls.hCursor        = LoadCursor(NULL, IDC_ARROW);
		cls.hIcon          = LoadIcon(hInst, szAppName);
		cls.lpszMenuName   = szAppName;
		cls.lpszClassName  = szAppName;
		cls.hbrBackground  = GetStockObject(LTGRAY_BRUSH);
		cls.hInstance      = hInst;
		cls.style          = CS_VREDRAW | CS_HREDRAW;
		cls.lpfnWndProc    = GoWndProc;
		cls.cbWndExtra     = sizeof(PWinInfo);
		cls.cbClsExtra     = 0;

		if (!RegisterClass(&cls))
			return FALSE;
	}

	/* create the application main window */
	hwndApp = CreateWindow(szAppName,	// Class name
		    szAppName,			// Caption
		    WS_OVERLAPPEDWINDOW,	// Style bits
		    CW_USEDEFAULT, CW_USEDEFAULT, // Position
		    CW_USEDEFAULT, CW_USEDEFAULT, // Size
		    (HWND) NULL,		// Parent window
		    (HMENU) NULL,		// use class menu
		    (HANDLE) hInst,		// instance handle
		    (LPSTR) NULL		// no params to pass on
		   );
	ShowWindow(hwndApp,sw);

	if( !medClientInit() ) {
		return FALSE;
	}
	
	if ((hInstMedGo = medLoadHandlerDLL("MEDGO")) == NULL)
	{
		ErrorResBox(hwndApp, hInstApp, MB_OK | MB_ICONHAND,
			    IDS_APPTITLE, IDS_CANTLOADMEDGO);
		return FALSE;
	}


	/* fixup my title to reflect the current no-resource state */
	ResourceFixTitle(hwndApp, 0L);

	/* poll for messages */
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	medClientExit();
	return msg.wParam;
}





/* PhysTypeDlgProc()
 *
 * Dialog box procedure for get/set the physical type dialog box.
 */
BOOL FAR PASCAL PhysTypeDlgProc(HWND hwnd, unsigned msg,
		WORD wParam, LONG lParam)
{

	switch (msg)
	{
	case WM_COMMAND:
		switch (wParam) {
	        case IDOK:
			EndDialog( hwnd, TRUE );
			break;
		    
	        case IDCANCEL:
			EndDialog( hwnd, FALSE );
			break;

	        case IDB_GOHAND:
			medtypePhys = medtypeGO;
			CheckRadioButton( hwnd, IDB_GOHAND, IDB_GOTXHAND,
				IDB_GOHAND );
			break;
		    
	        case IDB_GOTXHAND:
			medtypePhys = medtypeGOTX;
			CheckRadioButton( hwnd, IDB_GOHAND, IDB_GOTXHAND,
				IDB_GOTXHAND );
			break;
		}
		break;
		
	case WM_INITDIALOG:
		medtypePhys = medtypeGO;
		CheckRadioButton( hwnd, IDB_GOHAND, IDB_GOTXHAND,
			IDB_GOHAND );
		break;

	default:
		return FALSE;
	}

	return TRUE;
}





