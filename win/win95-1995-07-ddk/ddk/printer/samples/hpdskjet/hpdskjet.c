/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
//-----------------------------------------------------------------------------

#include "windows.h"

// the following 3 definitions MUST be compatible with the
// HPPCL font installer
#define CLASS_LASERJET      0
#define CLASS_DESKJET       1
#define CLASS_DESKJET_PLUS  2

char *rgchModuleName = "HPDSKJET";

// typedef for font installer
typedef int (FAR PASCAL *SFPROC)(HWND,LPSTR,LPSTR,BOOL,int,int);

//---------------------------*InstallExtFonts*---------------------------------
// Action: call the specific font installer to add/delete/modify soft fonts
//          and/or external cartridges.
//
// Parameters:
//      HWND    hWnd;           handle to the parent windows.
//      LPSTR   lpDeviceName;   long pointer to the printer name.
//      LPSTR   lpPortName;     long pointer to the associated port name.
//      BOOL    bSoftFonts;     flag if supporting soft fonts or not.
//
//  Return Value:
//      > 0   :  if the font information has changed;
//      == 0  :  if nothing has changed;
//      == -1 :  if intending to use the universal font installer
//               (not available now).
//-------------------------------------------------------------------------

int FAR PASCAL InstallExtFonts(hWnd, lpDeviceName, lpPortName, bSoftFonts)
HWND    hWnd;
LPSTR   lpDeviceName;
LPSTR   lpPortName;
BOOL    bSoftFonts;
{
    int     fsVers;
    HANDLE  hFIlib;
    int     class;
    SFPROC  lpFIns;

    if ((hFIlib = LoadLibrary((LPSTR)"FINSTALL.DLL")) < 32 ||
	!(lpFIns = (SFPROC)GetProcAddress(hFIlib,"InstallSoftFont")))
	{
	if (hFIlib >= 32)
	    FreeLibrary(hFIlib);
#ifdef DEBUG
	MessageBox(0,
	    "Can't load FINSTAL.DLL or can't get InstallSoftFont",
	    NULL, MB_OK);
#endif
	return TRUE;
	}

    // assume DeskJet 500 model has the same soft font format as DeskJet Plus

    if (lstrcmp(lpDeviceName, "HP DeskJet"))
	class = CLASS_DESKJET_PLUS;
    else if (lstrcmp(lpDeviceName, "HP DeskJet 540"))
        class = CLASS_LASERJET;
    else if (lstrcmp(lpDeviceName, "HP DeskJet 540 (Monochrome)"))
        class = CLASS_LASERJET;
    else
	class = CLASS_DESKJET;

    // FINSTALL.DLL was loaded properly. Now call InstallSoftFont().
    // We choose to ignore the returned "fvers". No use of it.
    fsVers = (*lpFIns)(hWnd, rgchModuleName, lpPortName,
		(GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0),
		1,        // dummy value for "fvers".
		bSoftFonts ? class : ((WORD)class | 0x0100)
		);
    FreeLibrary(hFIlib);
    return fsVers;
}
