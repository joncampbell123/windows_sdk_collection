/****************************************************************************
 *
 *   drvproc.c
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <compddk.h>
#include "TxtOut.h"

HMODULE ghModule;

/***************************************************************************
 *
 * DriverProc The entry point for an installable driver.
 *                
 ***************************************************************************/

LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    INSTINFO *pi = (INSTINFO *)(WORD)dwDriverID;

    switch (uiMessage)
    {
        case DRV_LOAD:
	    return (LRESULT) Load();

	case DRV_FREE:
	    Free();
	    return (LRESULT)1L;

	case DRV_OPEN:
	    // if being opened with no open struct, then return a non-zero
	    // value without actually opening
	    if (lParam2 == 0L)
                return 0xFFFF0000;

	    return (LRESULT)(DWORD)(WORD)Open((ICOPEN FAR *) lParam2);

	case DRV_CLOSE:
	    if (pi)
		Close(pi);

	    return (LRESULT)1L;

	/*********************************************************************

	    state messages

	*********************************************************************/

        case DRV_QUERYCONFIGURE:    // configuration from drivers applet
            return (LRESULT)0L;

        case DRV_CONFIGURE:
            return DRV_OK;

        case ICM_CONFIGURE:
            //
            //  return ICERR_OK if you will do a configure box, error otherwise
            //
            if (lParam1 == -1)
		return QueryConfigure(pi) ? ICERR_OK : ICERR_UNSUPPORTED;
	    else
		return Configure(pi, (HWND)lParam1);

        case ICM_ABOUT:
            //
            //  return ICERR_OK if you will do a about box, error otherwise
            //
            if (lParam1 == -1)
		return QueryAbout(pi) ? ICERR_OK : ICERR_UNSUPPORTED;
	    else
		return About(pi, (HWND)lParam1);

	case ICM_GETSTATE:
	    return GetState(pi, (LPVOID)lParam1, (DWORD)lParam2);

	case ICM_SETSTATE:
	    return SetState(pi, (LPVOID)lParam1, (DWORD)lParam2);

	case ICM_GETINFO:
	    return GetInfo(pi, (ICINFO FAR *)lParam1, (DWORD)lParam2);
	    
	/*********************************************************************

	    draw messages

	*********************************************************************/

	case ICM_DRAW_QUERY:
            return DrawQuery(pi, (LPVOID FAR *)lParam1);

	case ICM_DRAW_BEGIN:
            return DrawBegin(pi,(ICDRAWBEGIN FAR *)lParam1, (DWORD)lParam2);

	case ICM_DRAW:
            return Draw(pi,(ICDRAW FAR *)lParam1, (DWORD)lParam2);

	case ICM_DRAW_END:
	    return DrawEnd(pi);
	    
	/*********************************************************************

	    standard driver messages

	*********************************************************************/

	case DRV_DISABLE:
	case DRV_ENABLE:
	    return (LRESULT)1L;

	case DRV_INSTALL:
	case DRV_REMOVE:
	    return (LRESULT)DRV_OK;
    }

    if (uiMessage < DRV_USER)
        return DefDriverProc(dwDriverID, hDriver, uiMessage,lParam1,lParam2);
    else
	return ICERR_UNSUPPORTED;
}

/****************************************************************************
 *
 * LibMain  Library initialization code.
 *
 ***************************************************************************/
int NEAR PASCAL LibMain(HMODULE hModule, WORD wHeapSize, LPSTR lpCmdLine)
{
    ghModule = hModule;

    return 1;
}
