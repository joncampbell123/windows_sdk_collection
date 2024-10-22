/*----------------------------------------------------------------------------*\
|   edit.h - routines for dealing with multi-line edit controls                |
|                                                                              |
|                                                                              |
|   History:                                                                   |
|       01/01/88 toddla     Created                                            |
|       11/04/90 w-dougb    Commented & formatted the code to look pretty      |
|                                                                              | 
\*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*\
|                                                                              |
|   f u n c t i o n   p r o t o t y p e s                                      |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL EditOpenFile(HWND hwndE, LPSTR szFile);
BOOL EditSaveFile(HWND hwndE, LPSTR szFile);
DWORD EditGetLineCount(HWND hwndE);
BOOL EditGetLine(HWND hwndE, int iLine, LPSTR pch, int cch);
int  EditGetCurLine(HWND hwndE);
void EditSetCurLine(HWND hwndE, int iLine);
void EditSelectLine(HWND hwndE, int iLine);
