/**[f******************************************************************
* sfedit.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
typedef struct {
    HWND hEDwnd;
    HANDLE hSFlb;
    HANDLE hMd;
    int indSFfile;
    int response;
    int ind;
    int id;
    BOOL stop;
    BOOL showNext;
    BOOL normalEdit;
    BOOL global;
    WORD state;
    WORD family;
    char appName[64];
    char name[128];
    char desc[128];
    char dlname[16];
} EDREC;
  
typedef EDREC FAR *LPEDREC;
  
BOOL FAR PASCAL EditFonts(HWND, HANDLE, WORD, HANDLE, HANDLE, LPSTR, LPSTR, WORD FAR *);
BOOL FAR PASCAL editPFM(HWND, HANDLE, LPEDREC);
