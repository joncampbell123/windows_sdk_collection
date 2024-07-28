/*
 * <register.h>
 *
 * Created by Microsoft Corporation.
 * (c) Copyright Microsoft Corp. 1990 - 1992  All Rights Reserved
 */

//--- PROTOTYPES ---

//- Far

BOOL FAR    RegCopyClassName(HWND hwndList, LPSTR lpstrClassName);
void FAR    RegGetClassId(LPSTR lpstrName, LPSTR lpstrClass);
BOOL FAR    RegGetClassNames(HWND hwndList);
void FAR    RegInit(HANDLE hInst);
int  FAR    RegMakeFilterSpec(LPSTR lpstrClass, LPSTR lpstrExt, LPSTR lpstrFilterSpec);
void FAR    RegTerm(void);
