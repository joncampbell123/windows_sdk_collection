/*
 * edit.h
 */

/* Function prototypes.
 */
BOOL PASCAL FAR EditOpenFile(HWND, LPSTR);
BOOL PASCAL FAR EditSaveFile(HWND, LPSTR);
DWORD PASCAL FAR EditGetLineCount(HWND);
BOOL PASCAL FAR EditGetLine(HWND, int, LPSTR, int);
int PASCAL FAR EditGetCurLine(HWND);
void PASCAL FAR EditSetCurLine(HWND, int);
void PASCAL FAR EditSelectLine(HWND, int);
