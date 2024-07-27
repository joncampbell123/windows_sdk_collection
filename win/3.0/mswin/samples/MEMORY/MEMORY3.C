#include "windows.h"
#include "memory.h"

/****************************************************************************
    MODULE:  memory3.c

    FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - application menu (About dialog box)
	WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;

    switch (message) {
	case WM_COMMAND:
	    if (wParam == IDM_ABOUT) {
		lpProcAbout = MakeProcInstance(About, hInst);

		DialogBox(hInst,
		    "AboutBox",
		    hWnd,
		    lpProcAbout);

		FreeProcInstance(lpProcAbout);
		break;
	    }
	    else
		return (DefWindowProc(hWnd, message, wParam, lParam));

	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

	default:
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
