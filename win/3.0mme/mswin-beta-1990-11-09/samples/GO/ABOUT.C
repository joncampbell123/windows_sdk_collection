/* 
 * about.c
 *
 * Application "about" box.
 */

#include <windows.h>



/* AboutDlgProc()
 *
 * Dialog box procedure for the "about" dialog box.
 */
BOOL FAR PASCAL AboutDlgProc(HWND hwnd,unsigned msg,WORD wParam,LONG lParam)
{

	switch (msg)
	{
	case WM_COMMAND:
		/* The only valid command is hitting the "OK" button */
		EndDialog(hwnd, 0);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}



/* DoAbout()
 *
 * Pop up the "about" dialog box.
 */
WORD DoAbout(HWND hwnd, HANDLE hInst)
{
	FARPROC		fpfn;

	fpfn = MakeProcInstance(AboutDlgProc, hInst);
	DialogBox(hInst, "AboutBox", hwnd, fpfn);
	FreeProcInstance(fpfn);

	return TRUE;
}


