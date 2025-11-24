/*
 *  S M H O O F . C
 *
 *  Sample mail handling hook
 *  Out of office management
 *
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

#include "_pch.h"
#include <mapiutil.h>

#ifdef WIN32
#define szPlatform "32"
#else
#define szPlatform
#endif

enum { ipRID, ipREID, cpOofMax };
static const TCHAR rgchITable[] = "MAPIU" szPlatform ".DLL";
static const TCHAR rgchITableCreate[] = "CreateTable";
typedef SCODE (STDMETHODCALLTYPE FAR * LPITABLECREATETABLE) (
    LPCIID                  lpInterface,
    ALLOCATEBUFFER FAR *    lpAllocateBuffer,
    ALLOCATEMORE FAR *      lpAllocateMore,
    FREEBUFFER FAR *        lpFreeBuffer,
    LPVOID                  lpvReserved,
    ULONG                   ulTableType,
    ULONG                   ulPropTagIndexColumn,
    LPSPropTagArray         lpSPropTagArrayColumns,
    LPTABLEDATA FAR *       lppTableData);


VOID
ReleaseOof (LPOOF lpoof)
{
    if (lpoof->hlib)
    {
        UlRelease (lpoof->lptbl);
        UlRelease (lpoof->lptad);
        FreeLibrary (lpoof->hlib);
    }
    memset (lpoof, 0, sizeof(OOF));
    return;
}


HRESULT
HrLoadITable (LPOOF lpoof, LPITABLECREATETABLE FAR * lppfnCreateTable)
{
    UINT fuErr;
    HINSTANCE hlib = lpoof->hlib;

    if (!hlib)
    {
        fuErr = SetErrorMode(SEM_NOOPENFILEERRORBOX);
        hlib = LoadLibraryA (rgchITable);
        SetErrorMode (fuErr);

#ifdef  WIN32
        if(!hlib)
        {
            DebugTrace ("HrLoadITable() failed to load '%s'\n", rgchITable);
            DebugTrace ("GetLastError() returns %ld\n", GetLastError());
            DebugTraceSc (HrLoadITable(), MAPI_E_CALL_FAILED);
            return ResultFromScode (MAPI_E_CALL_FAILED);
        }
#else
        if(hlib <= HINSTANCE_ERROR)
        {
            DebugTrace ("HrLoadITable() failed to load '%s'\n", rgchITable);
            DebugTrace ("LoadLibrary() returns %ld\n", hlib);
            DebugTraceSc (HrLoadITable(), MAPI_E_CALL_FAILED);
            return ResultFromScode (MAPI_E_CALL_FAILED);
        }
#endif

        *lppfnCreateTable = (LPITABLECREATETABLE) GetProcAddress (hlib, rgchITableCreate);
        if (!*lppfnCreateTable)
        {
            DebugTrace ("HrLoadITable() failed to find create table API\n");
            DebugTraceSc (HrLoadITable(), MAPI_E_CALL_FAILED);
            return ResultFromScode (MAPI_E_CALL_FAILED);
        }
        
        lpoof->hlib = hlib;
    }
    return S_OK;
}


BOOL
FOofRecip (LPOOF lpoof, LPSPropValue lpvalEid)
{
    BOOL fOof = TRUE;
    LPMAPITABLE lptbl = lpoof->lptbl;
    SPropValue val;
    SRestriction res;

    if (lptbl)
    {
        val.ulPropTag = PR_ENTRYID;
        val.Value = lpvalEid->Value;

        res.rt = RES_PROPERTY;
        res.res.resProperty.relop = RELOP_EQ;
        res.res.resProperty.ulPropTag = PR_ENTRYID;
        res.res.resProperty.lpProp = &val;

        fOof = !!HR_FAILED (lptbl->lpVtbl->FindRow (lptbl, &res, BOOKMARK_BEGINNING, 0L));
    }
    return fOof;
}


HRESULT
HrRegOofRecip (LPSMH lpsmh, LPOOF lpoof, LPSPropValue lpvalEid)
{
    HRESULT hr;
    LPTABLEDATA lptad = lpoof->lptad;
    SizedSPropTagArray (cpOofMax, sptOofTbl) = {cpOofMax, { PR_ROWID, PR_ENTRYID }};
    SPropValue rgval[cpOofMax];
    SRow rw;

    if (!lptad)
    {
        LPITABLECREATETABLE lpfnCreateTable = NULL;
        
        //  This is the first OOF recip so we need to create the
        //  table of recips
        //
        hr = HrLoadITable (lpoof, &lpfnCreateTable);
        if (HR_FAILED (hr))
            goto ret;

        hr = ResultFromScode ((*lpfnCreateTable) ((LPIID)&IID_IMAPITableData,
                            lpsmh->lpfnAlloc,
                            lpsmh->lpfnAllocMore,
                            lpsmh->lpfnFree,
                            NULL,
                            TBLTYPE_DYNAMIC,
                            PR_ROWID,
                            (LPSPropTagArray)&sptOofTbl,
                            (LPTABLEDATA FAR *)&lptad));
        if (HR_FAILED (hr))
            goto ret;
        
        hr = lptad->lpVtbl->HrGetView (lptad, NULL, NULL, 0, &lpoof->lptbl);
        if (!HR_FAILED (hr))
        {
            UlRelease (lptad);
            goto ret;
        }
        lpoof->lptad = lptad;
    }
    
    rgval[ipRID].ulPropTag = PR_ROWID;
    rgval[ipRID].Value.l = lpoof->cRecips++;
    rgval[ipREID].ulPropTag = PR_ENTRYID;
    rgval[ipREID].Value = lpvalEid->Value;

    hr = lptad->lpVtbl->HrModifyRow (lptad, &rw);
    if (HR_FAILED (hr))
        goto ret;

ret:

    DebugTraceResult (HrRegOofRecip(), hr);
    return hr;
}
