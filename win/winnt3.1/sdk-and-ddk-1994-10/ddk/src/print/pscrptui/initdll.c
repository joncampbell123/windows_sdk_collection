/* ---File: initdll.c -----------------------------------------------------
 *
 *	Description:
 *		Dynamic Link Library initialization module.  These functions are
 *		invoked when the DLL is initially loaded by NT.
 *
 *		This document contains confidential/proprietary information.
 *		Copyright (c) 1991 Microsoft Corporation, All Rights Reserved.
 *
 * Revision History:
 *	 [00]	24-Jun-91	stevecat	created
 *
 * ---------------------------------------------------------------------- */

#include        <windows.h>

HANDLE    hModule;

/*************************** Function Header ******************************
 * DllInitialize ()
 *    DLL initialization procedure.  Save the module handle since it is needed
 *  by other library routines to get resources (strings, dialog boxes, etc.)
 *  from the DLL's resource data area.
 *
 * RETURNS:
 *   TRUE/FALSE.
 *
 * HISTORY:
 *     [01]     4-Oct-91    stevecat    new dll init logic
 *     [00]    24-Jun-91    stevecat    created
 *
 ***************************************************************************/

BOOL
DllInitialize( hmod, ulReason, pctx )
PVOID     hmod;
ULONG     ulReason;
PCONTEXT  pctx;
{
    BOOL   bRet;                /* Return code */

    UNREFERENCED_PARAMETER( pctx ); 

    bRet = TRUE;

    switch( ulReason )
    {
    case  DLL_PROCESS_ATTACH:
        hModule = hmod;
        break;

    default:
        break;
    }

    return bRet;
}
