/****************************************************************************
    MODULE:  memory2.c

    FUNCTION: MemoryWndProc(HANDLE)

    PURPOSE: Processes messages

****************************************************************************/

#include "windows.h"
#include "memory.h"

long FAR PASCAL MemoryWndProc(hWnd, message, wParam, lParam)
HWND hWnd;	    
unsigned message;   
WORD wParam;	    
LONG lParam;	    
{
    FARPROC lpProcAbout;  
    HMENU hMenu;

    switch (message) {
	case WM_CREATE:
	    hMenu = GetSystemMenu(hWnd, FALSE);
	    ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
	    ChangeMenu(hMenu, NULL, "A&bout Memory...",
		ID_ABOUT, MF_APPEND | MF_STRING);
	    break;

	case WM_SYSCOMMAND:
	    if (wParam == ID_ABOUT) {
		lpProcAbout = MakeProcInstance(About, hInst);
		DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		FreeProcInstance(lpProcAbout);
	    }
	    else
		return (DefWindowProc(hWnd, message, wParam, lParam));

	    break;

	case WM_DESTROY:
	    PostQuitMessage(NULL);
	    break;

	default:
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}
