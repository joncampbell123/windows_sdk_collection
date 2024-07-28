/*
    FRALC Consultores S.C.
    TtyHelp.h
    Por Cesar Emilio Soli's Brito
    Wed 06-Sep-1989 11:43:07
*/

// ===========================================================================
// Copyright (c) 1989-1990, Microsoft Corporation.
// ===========================================================================

//	Microsoft history
//	20 oct 89	peterbe		checked in

#define TTY_INDEX	    1000
#define TTY_GENERIC	    1001
#define TTY_ADDMODIFY	    1002
#define TTY_CHARACTERS	    1003
#define TTY_CONTROLCODES    1004

/*  Help engine section.  */

/* Commands to pass WinHelp() */
#define HELP_CONTEXT   0x0001	/* Display topic in ulTopic */
#define HELP_QUIT      0x0002	/* Terminate help */
#define HELP_INDEX     0x0003	/* Display index */
#define HELP_KEY       0x0101	/* Display topic for keyword in offabData */

// BOOL FAR PASCAL WinHelp(HWND hwndMain, LPSTR lpszHelp, WORD usCommand, DWORD ulData);
