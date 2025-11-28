
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/* process.h - header file for the process sample. */


/* function prototypes.  Window procedures first. */
LRESULT CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);

VOID doCreate (HWND);
VOID doTerminate(HWND);


/* the control ID's from the dialog box. */
#define DID_CREATE      0x0065
#define DID_TERMINATE   0x0066
#define DID_LISTBOX     0x0067
#define DID_HEADER      0x0068


/* length of character buffers.  use longest possible file name */
#define MAXCHARS MAX_PATH
