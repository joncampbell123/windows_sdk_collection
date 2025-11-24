/******************************Module*Header*******************************\
* Module Name: julia.h
*
* Header file for Julia.c
*
* Created: 24-Oct-1991 18:15:12
* Author: Petrus Wong
*
* Copyright (c) 1990 Microsoft Corporation
*
* Contains the #define values for the menu items' id and handy globals
*
* Dependencies:
*       none
*
\**************************************************************************/

#include <string.h>
#include <winspool.h>
#include <drivinit.h>
#include "jtypes.h"
#include "bndscan.h"
#include "dibmp.h"
#include "printer.h"

//
// A large number.  Used in all the fix-point versions of the fractal
// generation algorithms.
//
#define FIRST_PIXEL     429496796

//
// Threshold value (pixels) for blting drawing from shadow bitmap to
// screen.  Used in fractal drawing algorithm.  Minimize device access.
//
#define BATCH           25000

//
// menu item IDs
//
#define MM_ABOUT        8000
#define MM_JULIA	8001
#define MM_MANDEL  	8002
#define MM_RLEVIEWER    8006
#define MM_SAVE 	8003
#define MM_SAVE_MONO    8004
#define MM_LOAD 	8005

#define MM_CREATE_JULIA_THREAD	7001
#define MM_SET_XFORM_ATTR	7002
#define MM_CREATE_MANDEL_THREAD	7003

#define MM_FLOAT        7009
#define MM_FIX          7010

#define MM_TP_IDLE              7030
#define MM_TP_LOW               7031
#define MM_TP_BELOW_NORMAL      7032
#define MM_TP_NORMAL            7033
#define MM_TP_ABOVE_NORMAL      7034
#define MM_TP_HIGH              7035
#define MM_TP_TIME_CRITICAL     7036
#define MM_ITERATION_100        7011
#define MM_ITERATION_500        7012
#define MM_ITERATION_1000       7013
#define MM_ITERATION_5000       7014
#define MM_ITERATION_DOUBLE     7015
#define MM_STEP_ONE             7016
#define MM_STEP_TWO             7017
#define MM_STEP_THREE           7018
#define MM_STRETCHBLT           7019
#define MM_BITBLT               7020
#define MM_BLACKONWHITE         7021
#define MM_COLORONCOLOR         7022
#define MM_WHITEONBLACK         7023
#define MM_HALFTONE             7024
#define MM_OPT_4                7025
#define MM_CLIP                 7026
#define MM_RM_CLIP              7027
#define MM_ERASE                7028
#define MM_SETDIB2DEVICE        7029

// defined in bndscan.h
//
// #define MM_SELCLIPRGN           7050

// defined in printer.h
//
// #define MM_PORTRAIT             7040
// #define MM_LANDSCAPE            7041
// #define MM_PRINTER              9000


#define MM_RLELOAD_DEMO     9800
#define MM_RLELOAD          9801
#define MM_RLESAVE          9802
#define MM_CLEAR            9803
#define MM_RLEPLAY          9804
#define MM_RLEPLAYCONT      9805

#define IDM_CASCADE	30
#define IDM_TILE	31
#define IDM_ARRANGE	32
#define IDM_CLOSEALL	33

//
// Resource IDs
//
#define ACCEL_ID        100
#define APPICON         1001
#define VIEWICON        1003
#define PAINTCURSOR     1002

//
// Handy globals
//
HPEN   hpnRed;
HPEN   hpnBlack;
HPEN   hpnGreen;
INT    giPen = 0;

HANDLE ghModule;
HWND   ghwndMain = NULL;
HWND   ghwndClient = NULL;
HANDLE ghAccel;

HMENU  hMenu, hChildMenu, hViewMenu;
HMENU  hViewSubOne, hSubMenuOne, hSubMenuThree;
HMENU  hPrinterMenu;

CHAR   gszFile[20];
CHAR   gszMapName[20];
char   gtext[256];

BOOL   gFloat = TRUE;
LONG   gStep = 2;
LONG   gIteration = 500;
BOOL   gbStretch = TRUE;
INT    giStretchMode = COLORONCOLOR;
INT    giDmOrient = DMORIENT_PORTRAIT;
INT    giNPrinters = 0;

HPALETTE        ghPal, ghPalOld;

double xFrom, xTo, yFrom, yTo, c1, c2;
LONG   lxFrom, lxTo, lyFrom, lyTo, lc1, lc2;

extern PPRINTER_INFO_1     gpPrinters;
extern PSZ                *gpszPrinterNames;
extern PSZ                *gpszDeviceNames;

extern BOOL bCycle(HWND);
extern BOOL bCleanupPrinter(VOID);
extern INT  iCreatePenFrPal(HDC, PVOID *, INT, HPALETTE *);
extern BOOL bBoundaryScanFix(PINFO);
extern BOOL bChangeDIBColor(HDC, PINFO, INT);
extern BOOL bInitPrinter(HWND);
extern BOOL bCleanupPrinter(VOID);


extern BOOL SaveBitmapFile(HDC, HBITMAP, PSTR);
extern BOOL LoadBitmapFile(HDC, PINFO, PSTR);

extern HPALETTE CopyPalette(HPALETTE);

extern void cdecl ErrorOut(char errstring[30]);
extern int FAR PASCAL ShellAbout(HWND, LPCSTR, LPCSTR, HICON);
