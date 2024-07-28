/*
 * WEP.C
 *
 * WEP code for the DLL.  This function is placed in a separate
 * segment so marking the segment as FIXED is not hard on the
 * system.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 */


#include <windows.h>


/*
 * WEP
 *
 * Purpose:
 *  Required DLL Exit function.  Does nothing.
 *
 * Parameters:
 *  bSystemExit     BOOL indicating if the system is being shut
 *                  down or the DLL has just been unloaded.
 *
 * Return Value:
 *  void
 *
 */

void FAR PASCAL WEP(int bSystemExit)
    {
    if (bSystemExit)
        {
        //Shutdown
        }
    else
        {
        //DLL use count is zero
        }

    return;
    }
