/*
 *  M S P G L E . C
 *
 *  Code for implementing the GetLastError method of all store
 *  objects.
 *
 *  Copyright 1992-1995 Microsoft Corporation.  All Rights Reserved.
 */

#include "msp.h"
#include "msprc.h"

/* Manifest constants */

#define STRING_MAX      128

/* Global variables */

/* The following array maps a string identifier (IDS) to status code */
/* (SCODE).  The order of SCODEs in the array has an external        */
/* dependency:  the order of elements in the array is dictated by    */
/* the IDS definitions in rc.h.  This implicit association must be   */
/* maintained for the strings associated with string identifiers to  */
/* make sense.  Thus, if either this structure or the rc.h defines   */
/* changes, the other must change to match it.                       */

SCODE mpIdsScode[] =
{
    SUCCESS_SUCCESS,
    MAPI_E_NOT_ENOUGH_MEMORY,
    MAPI_E_NO_ACCESS,
    MAPI_E_INVALID_PARAMETER,
    MAPI_E_INTERFACE_NOT_SUPPORTED,
    MAPI_E_INVALID_ENTRYID,
    MAPI_E_CALL_FAILED,
    MAPI_W_ERRORS_RETURNED,
    MAPI_E_NO_SUPPORT,
    MAPI_E_NOT_IN_QUEUE,
    MAPI_E_UNABLE_TO_ABORT,
    MAPI_E_NOT_FOUND,
    MAPI_E_LOGON_FAILED,
    MAPI_E_CORRUPT_STORE,
    MAPI_E_BAD_VALUE,
    MAPI_E_INVALID_OBJECT,
    MAPI_E_NOT_ENOUGH_DISK,
    MAPI_E_DISK_ERROR,
    E_NOINTERFACE,
    E_INVALIDARG,
    MAPI_E_UNKNOWN_FLAGS,
    MAPI_E_HAS_MESSAGES,
    MAPI_E_HAS_FOLDERS,
    MAPI_E_NOT_ENOUGH_RESOURCES
};

/*
 *  Exported functions
 */

/*
 *  MapScodeSz
 *
 *  Purpose:
 *      Look up an SCODE in a mapping of IDS <-> SCODE to find its
 *      associated informational string and return it (with memory
 *      allocated by this function) to the caller.
 *
 *  Arguments:
 *      scArg       The SCODE to look up.
 *      pims        Pointer to the message store object (where we
 *                  obtain the memory allocation functions).
 *      lppszError  Location in which to place an address to a
 *                  newly allocated buffer containing the
 *                  informational string associated with scArg.
 *
 *  Returns:
 *      HRESULT
 *
 *  Side effects:
 *      None.
 *
 *  Errors:
 *      MAPI_E_NO_MEMORY    Could not allocate space for
 *                                  the return string.
 */
HRESULT MapScodeSz(SCODE scArg, PIMS pims, LPTSTR * lppszError)
{
    HRESULT hr = hrSuccess;
    SCODE sc = SUCCESS_SUCCESS;
    UINT ui = 0;
    UINT uiMax = 0;
    int iLS = 0;
    LPTSTR szErr = NULL;
    TCHAR rgch[STRING_MAX];

    AssertSz(lppszError, "Bad lppszError\n");

    /* Linear search in mpIdsScode for scArg.  When found, index is IDS. */

    uiMax = sizeof mpIdsScode / sizeof mpIdsScode[0];
    for (ui = 0; ui < uiMax; ui++)
    {
        if (mpIdsScode[ui] == scArg)
            break;
    }

    if (ui == uiMax)
    {
        hr = ResultFromScode(MAPI_E_INVALID_PARAMETER);
        goto exit;
    }

    /* Get the string from the resource.  Note:  the assumption that   */
    /* rgch is large enough to hold the largest string that LoadString */
    /* could return can be checked by looking at the resource strings  */
    /* file, \mapi\src\resrc\usa\strings\smpl_ms.s.                    */

    Assert(pims->pmsp);

    iLS = LoadString(pims->pmsp->hInst, ui, rgch, STRING_MAX);
    AssertSz(iLS, "Unknown string identifier!\n");
    AssertSz(iLS < STRING_MAX, "String resource truncated!\n");

    /* Allocate memory for return variable and set it */

    sc = LMAlloc(&pims->lmr, Cbtszsize(rgch), &szErr);
    if (sc != S_OK)
    {
        hr = ResultFromScode(sc);
        goto exit;
    }

    lstrcpy(szErr, rgch);
    *lppszError = szErr;

exit:
    DebugTraceResult(MapScodeSz, hr);
    return hr;
}
