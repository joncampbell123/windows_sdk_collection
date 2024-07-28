/*
 * WEP.C
 *
 * Standard routines for a DLL.
 *
 */


#include <windows.h>


/*
 * LibMain
 *
 * Purpose:
 *  Called by libentry.obj. Other than that this does very very little
 *  so I won't bother to write much more in this header.
 *
 */

int FAR PASCAL LibMain(HANDLE hModule, WORD wDataSeg,
                       WORD cbHeapSize, LPSTR lpszCmdLine)
    {
    return 1;
    }


/*
 * WEP
 *
 * Purpose:
 *  Performs cleanup tasks when the DLL is unloaded.  WEP() is
 *  called automatically by Windows when the DLL is unloaded (no
 *  remaining tasks still have the DLL loaded).  It is strongly
 *  recommended that a DLL have a WEP() function, even if it does
 *  nothing but returns success (1), as in this example.
 *
 * Parameters:
 *  bSystemExit int     Flag indicating if the system is shutting
 *                      down or not.
 *
 * Return Value:
 *  1
 *
 */
int FAR PASCAL WEP (int bSystemExit);
#pragma alloc_text(FIXEDSEG, WEP)

int FAR PASCAL WEP (int bSystemExit)
    {
    return(1);
    }
