/*
 * SMASHER.H
 *
 * Definitions and function prototypes for the file manager extesion
 * SMASHER.DLL.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved.
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 *
 */


//Resource identifiers
#define IDR_BITMAP          1
#define IDR_MENU            1

//Menu constants
#define IDS_SMASH           1
#define IDS_SMASHHELP       100
#define IDM_SMASH           1


//Cheap strings
#define SZSMASHER               TEXT("Compound File Smasher")
#define SZERRNOTACOMPOUNDFILE   TEXT("Smasher cannot defragment %s.\n\rNot a compound file.")
#define SZERRTEMPFILE           TEXT("Could not create an intermediate file.\n\rCheck disk space and your TEMP environment variable.")
#define SZERROPENFAILED         TEXT("Could not access %s for defragmentation.\n\rFile could be locked.")
#define SZERRTEMPFILECOPY       TEXT("Could not write to intermediate file.\n\rCould be out of disk space or memory.")
#define SZERRTEMPHASFILE        TEXT("Failure to overwrite file.  Defragemented version can be found in\r\n%s.")

#define CCHPATHMAX              256


//SMASHER.CPP
BOOL SmashSelectedFiles(HWND);

#ifndef WIN32
extern "C"
    {
    HMENU WINAPI FMExtensionProc(HWND hWnd, UINT iMsg, LONG lParam);
    }
#endif
