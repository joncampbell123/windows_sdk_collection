
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/


/* angle.h - header file for the AngleArc() demonstration program. */


/* function prototypes for the window procedures. */
long FAR                        PASCAL
MainWndProc(HWND, UINT, UINT, LONG);
long FAR                        PASCAL
DlgProc(HWND, UINT, UINT, LONG);


/* Top dialog item IDs */
#define DID_X       101
#define DID_Y       102
#define DID_RADIUS  103
#define DID_START   104
#define DID_SWEEP   105
#define DID_DRAW    200

#define MAXCHARS      32

/* Misc. defines for size, color, and appearance of drawing. */
#define GRIDCOLOR  (COLORREF) 0x01000006
#define TICKSPACE     20
#define DIALOGHEIGHT  60
