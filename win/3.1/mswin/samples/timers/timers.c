#if 0

ABSTRACT START

TITLE
        Windows Timer Sample Application
EXE
        timers.exe
AUTHOR
        Bob Gunderson - Microsoft Corp.
CONTENT
        This sample application demonstrates how to create and terminate
        application timers and how to change the rate of an existing timer.

        On menu commands, it will create a timer that can either dispatch a
        WM_TIMER message to a window procedure or call a callback function.
        The timer can be set to run at one of three speeds.

        The sample also demonstrates calling the Toolhelp.dll timer services.
TAG
        1.3.2
        2.1
        2.2
        3.1.1.21
        3.1.15

ABSTRACT END

#endif


#include "windows.h"
#include "timers.h"
#include "toolhelp.h"

//---------------------------------------------------------------------------
// Global Variables...
//---------------------------------------------------------------------------

HANDLE  hInstance;              // Global instance handle for application
int     TimerType = cmdStop;    // Current timer type
int     TimerSpeed = cmdSlow;   // Current timer speed
int     hwndKill, idKill;       // Values to use on KillTimer
int     Millisec = 5000;        // Current timer speed in milliseconds
FARPROC fpTimerCallback;        // MakeProcInstance of callback
HWND    hwndMain;               // Main hwnd.  Needed in callback
int     counter = 0;            // Counter to display when timer goes off
char    szText[80];             // Temp storage to build strings
TIMERINFO  ti;                  // Structure to pass to Toolhelp.dll

//---------------------------------------------------------------------------
// Function declarations
//---------------------------------------------------------------------------

LONG FAR PASCAL TimerTestWndProc(HWND, UINT, WPARAM, LPARAM);
void StartTimer(HWND, int, int);
WORD FAR PASCAL TimerCallbackProc(HWND, WORD, int, DWORD);

//---------------------------------------------------------------------------
// WinMain
//---------------------------------------------------------------------------

int PASCAL WinMain(HANDLE hInst, HANDLE hInstPrev, LPSTR lpstrCmdLine, int cmdShow)
{
	MSG msgMain;
	WNDCLASS wc;

	// Set the global instance variable
	hInstance = hInst;

	// Register the window class if this is the first instance.
	if (hInstPrev == NULL)
	{
		wc.lpszMenuName     = "TimerMenu";
		wc.lpszClassName    = "TimerTestApp";
		wc.hInstance        = hInst;
		wc.hIcon            = LoadIcon(hInst, "TimerIcon");
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = COLOR_WINDOW + 1;
		wc.style            = 0;
		wc.lpfnWndProc      = TimerTestWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = 0;

		if (!RegisterClass(&wc))
			return(0);
	}

	// Create the main window
	if ((hwndMain = CreateWindow("TimerTestApp",
								 "Sample Timer Application",
								 WS_OVERLAPPEDWINDOW,
								 CW_USEDEFAULT, 0,
								 CW_USEDEFAULT, CW_USEDEFAULT,
								 NULL, NULL, hInst, NULL)) == NULL)
		return(0);

        // Need to call MakeProcInstance because multiple instances of this
        // application can exist.
	fpTimerCallback = MakeProcInstance(TimerCallbackProc, hInst);

        // Show the window and make sure it is updated.
	ShowWindow(hwndMain, cmdShow);
	UpdateWindow(hwndMain);

	// Main message "pump"
	while (GetMessage((LPMSG) &msgMain, NULL, 0, 0))
	{
	   TranslateMessage((LPMSG) &msgMain);
	   DispatchMessage((LPMSG) &msgMain);
	}

        // Even though Windows destroys timers created by this app, destroying
        // the things the application creates is good practice.
	if (TimerType != cmdStop)
		KillTimer(hwndKill, idKill);

	return(0);
}



//---------------------------------------------------------------------------
// TimerTestWndProc
//
// Window procedure for the sample applications window.
//
//---------------------------------------------------------------------------

LONG FAR PASCAL TimerTestWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HMENU       hMenu;
	HDC         hdc;

	switch(msg) {

		//
		// Handle menu selections
		//
		case WM_COMMAND:
			switch (wParam) {

				case cmdStop:
				case cmdToHwnd:
				case cmdCallProc:
					//
					// Change the checkmark to the proper item
					//
					hMenu = GetMenu(hwnd);
					CheckMenuItem(hMenu, TimerType, MF_UNCHECKED | MF_BYCOMMAND);
					CheckMenuItem(hMenu, wParam, MF_CHECKED | MF_BYCOMMAND);
					//
					// Kill any existing timer.  Also clear the text displayed
					// by a previous timer going off.
					//
					KillTimer(hwndKill, idKill);
					idKill = 0;
					counter = 0;
					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);
					//
					// Set the new timer type and start it up.
					//
					TimerType = wParam;
					if (wParam != cmdStop)
						StartTimer(hwnd, TimerType, TimerSpeed);
					break;

				//
				// Set the timer speed
				//
				case cmdFast:
					Millisec = 250;
					goto Common;
				case cmdMedium:
					Millisec = 1000;
					goto Common;
				case cmdSlow:
					Millisec = 5000;
Common:
					//
					// Change the checkmark to the proper item
					//
					hMenu = GetMenu(hwnd);
					CheckMenuItem(hMenu, TimerSpeed, MF_UNCHECKED | MF_BYCOMMAND);
					CheckMenuItem(hMenu, wParam, MF_CHECKED | MF_BYCOMMAND);
					//
					// Reset the timer.  No need to kill the existing one and
                                        // start a new one.  Windows resets an existing timer
					// with a new speed if the hWnd and ID are identical to
					// an existing timer.
					//
					TimerSpeed = wParam;
					if (TimerType != cmdStop)
						StartTimer(hwnd, TimerType, TimerSpeed);
					break;

			}
			break;

		case WM_TIMER:
			//
			// Timer set to dispatch a WM_TIMER message has gone off.
			// Display a little message and beep the speaker.
			//
			MessageBeep(0);
			hdc = GetDC(hwnd);
			wsprintf((LPSTR)&szText, "WM_TIMER sent to hwnd - %d    ", counter++);
			TextOut(hdc, 10, 10,(LPSTR)&szText, lstrlen((LPSTR)&szText));
			//
			// Now call Toolhelp to get the system timer numbers
			//
			ti.dwSize = sizeof(TIMERINFO);
			if (!TimerCount((TIMERINFO FAR *)&ti))
			{
				ReleaseDC(hwnd, hdc);
				break;
			}
			wsprintf((LPSTR)&szText, "System: %lu, this VM: %lu    ", ti.dwmsSinceStart, ti.dwmsThisVM);
			TextOut(hdc, 10, 40, (LPSTR)&szText, lstrlen((LPSTR)&szText));
			ReleaseDC(hwnd, hdc);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return(DefWindowProc(hwnd, msg, wParam, lParam));
	}

	return(0);
}

//---------------------------------------------------------------------------
// StartTimer
//
// Starts a timer to either dispatch a WM_TIMER message or call a callback.
// Care is taken to save the proper hWnd and timer ID for use when killing
// the timer.
//---------------------------------------------------------------------------

void StartTimer(HWND hwnd, int Type, int Speed)
{
	switch (Type) {

		//
		// Create a timer to dispatch a WM_TIMER message.
		//
		case cmdToHwnd:
			if (SetTimer(hwnd, 1, Millisec, NULL) == NULL)
			{
				MessageBox(hwnd, "Couldn't create timer.", "Notice!", MB_OK);
				return;
			}
			hwndKill = hwnd;
			idKill = 1;
			break;

		//
                // Create a timer to call a callback function.  If we are
                // changing only the timer rate, be sure that the ID parameter is
		// the idKill value returned from the initial SetTimer call.
		//
		case cmdCallProc:
			if ((idKill = SetTimer(NULL, (idKill ? idKill : 1), Millisec, fpTimerCallback)) == NULL)
			{
				MessageBox(hwnd, "Couldn't create timer.", "Notice!", MB_OK);
				return;
			}
			hwndKill = NULL;
			break;

	}
}

//---------------------------------------------------------------------------
// TimerCallbackProc
//
// Callback function that is called when a timer setup to call the callback
// goes off.  Displays a short message and beeps the speaker.
//---------------------------------------------------------------------------

WORD FAR PASCAL TimerCallbackProc(HWND hwnd, WORD msg, int idEvent, DWORD dwtime)

{
	HDC hdc;

	MessageBeep(0);
	hdc = GetDC(hwndMain);
	wsprintf((LPSTR)&szText, "Callback function called - %d    ", counter++);
	TextOut(hdc, 10, 10, (LPSTR)&szText, lstrlen((LPSTR)&szText));
	//
	// Now call Toolhelp to get the system timer numbers
	//
	ti.dwSize = sizeof(TIMERINFO);
	if (!TimerCount((TIMERINFO FAR *)&ti))
	{
		ReleaseDC(hwnd, hdc);
		return(0);
	}
	wsprintf((LPSTR)&szText, "System: %lu, this VM: %lu    ", ti.dwmsSinceStart, ti.dwmsThisVM);
	TextOut(hdc, 10, 40, (LPSTR)&szText, lstrlen((LPSTR)&szText));
	ReleaseDC(hwndMain, hdc);
	return(0);
}
