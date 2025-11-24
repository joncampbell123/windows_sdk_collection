/*
 *  S M H C F G . C
 *
 *  Sample mail handling hook configuration
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

#include "_pch.h"

/*
 *  Configuration Properties
 *  
 *  sptConfigProps
 *  
 *      This is the set of properties that we expect to find in the
 *      provider's profile section.
 *  
 *  sptRule
 *  
 *      This is the set of properties that are expected to be found in
 *      each rule's profile section.
 */
const SizedSPropTagArray (cpMax, sptConfigProps) =
{
    cpMax,
    {
        PR_SMH_FLAGS,
        PR_SMH_RULES,
        PR_SMH_EXCLUSIONS
    }
};
const SizedSPropTagArray (cpRLMax, sptRule) =
{
    cpRLMax,
    {
        PR_DISPLAY_NAME,
        PR_RULE_TYPE,
        PR_RULE_DATA,
        PR_RULE_FLAGS,
        PR_RULE_TARGET_ENTRYID,
        PR_RULE_TARGET_PATH,
        PR_RULE_STORE_ENTRYID,
        PR_RULE_STORE_DISPLAY_NAME,
        PR_RULE_SOUND_FILENAME
    }
};


/*
 *  Configuration event name
 */
const TCHAR lpszConfigEvt[] = "SMH_CONFIGURATION_EVENT";


/*
 *  HrOpenSingleProvider()
 *  
 *  Purpose:
 *  
 *      Opens the profile section of this service provider.  This is done
 *      by opening the message service table and querying all rows.
 *      Since the SMH service contains only one provider, there is only
 *      one row in the table.  This contains our profile section.  Use
 *      the PR_PROFILE_UID to open the section.
 *  
 *  Arguments:
 *  
 *      lpadmin         the IProfileAdmin object
 *      lppprof [OUT]   on return contains the profile section obj
 *  
 *  Returns:
 *  
 *      (HRESULT)
 *      lppprof [OUT]   contains the profile section object iff the call
 *                      is successful.
 */
HRESULT
HrOpenSingleProvider (LPPROVIDERADMIN lpadmin, LPPROFSECT FAR * lppprof)
{
    HRESULT hr;
    LPMAPITABLE lptbl = NULL;
    LPSRowSet lprws = NULL;
    SPropTagArray sptProvider = {1, {PR_PROVIDER_UID}};
    SRestriction res = {0};

    res.rt = RES_EXIST;
    res.res.resExist.ulPropTag = PR_PROVIDER_DLL_NAME;

    hr = lpadmin->lpVtbl->GetProviderTable (lpadmin, 0, &lptbl);
    if (!HR_FAILED (hr))
    {
        hr = lptbl->lpVtbl->SetColumns (lptbl, &sptProvider, 0L);
        if (!HR_FAILED (hr))
        {
            hr = lptbl->lpVtbl->Restrict (lptbl, &res, 0);
            if (!HR_FAILED (hr))
            {
                hr = lptbl->lpVtbl->QueryRows (lptbl, 10, 0, &lprws);
                if (!HR_FAILED (hr) && lprws->cRows)
                {
                    Assert (lprws->cRows == 1);
                    Assert (lprws->aRow[0].cValues == 1);
                    Assert (lprws->aRow[0].lpProps);
                    Assert (lprws->aRow[0].lpProps[0].ulPropTag == PR_PROVIDER_UID);
                    hr = lpadmin->lpVtbl->OpenProfileSection (lpadmin,
                        (LPMAPIUID)lprws->aRow[0].lpProps[0].Value.bin.lpb,
                        NULL,
                        MAPI_MODIFY,
                        lppprof);
                    MAPIFreeBuffer (lprws->aRow[0].lpProps);
                }
                MAPIFreeBuffer (lprws);
            }
        }
        UlRelease (lptbl);
    }
    DebugTraceResult (HrOpenSingleProvider, hr);
    return hr;
}


/*
 *  HrMergeValues()
 *  
 *  Purpose:
 *  
 *      Merges two property value arrays into one.  By making a copy of
 *      the first and then adding/replacing props from the second.
 *  
 *      IMPORTANT: if there are any property values that are separately
 *      allocated with either MAPIAllocateBuffer() or MAPIAllocateMore(),
 *      then these property values must remain valid as long as the
 *      merged property array is expected to be valid.
 *  
 *  Returns:
 *  
 *      (HRESULT)
 */
HRESULT
HrMergeValues (ULONG cval1,
    LPSPropValue lpval1,
    ULONG cval2,
    LPSPropValue lpval2,
    LPALLOCATEBUFFER lpfnAlloc,
    ULONG FAR * lpcvalMerged,
    LPSPropValue FAR * lppvalMerged)
{
    SCODE sc;
    LPSPropValue lpval = NULL;
    UINT cb;
    UINT ip;

    cb = (UINT)(cval1 + cval2) * sizeof(SPropValue);
    sc = (*lpfnAlloc)(cb, &lpval);
    if (!FAILED (sc))
    {
        /* Slurp the original data across */

        memcpy (lpval, lpval1, (UINT)cval1 * sizeof(SPropValue));

        /* Move the new stuff over */

        while (cval2--)
        {
            for (ip = 0; ip < (UINT)cval1; ip++)
            {
                /* See if we match properties */

                if (PROP_ID (lpval[ip].ulPropTag) == PROP_ID (lpval2[cval2].ulPropTag))
                {
                    /* We matched, but are we a real property */

                    if (PROP_TYPE (lpval2[cval2].ulPropTag) != PT_ERROR)
                        lpval[ip] = lpval2[cval2];
                    break;
                }
            }
            if (ip == cval1)
                lpval[cval1++] = lpval2[cval2];
        }
        *lpcvalMerged = cval1;
        *lppvalMerged = lpval;
    }
    DebugTraceSc (HrMergeValues(), sc);
    return ResultFromScode (sc);
}


/*
 *  UlChkReqProps()
 *  
 *  Purpose:
 *  
 *      Checks a property value array and sets bits in the return value
 *      for properties that are present and non-NULL
 *  
 *  Returns:
 *  
 *      (ULONG) where the Nth bit is set iff the property value
 *      structure at index N is valid and non-NULL
 */
ULONG
UlChkReqProps (ULONG cval, LPSPropValue lpval)
{
    ULONG ul;

    Assert (cval <= 32);
    for (ul = 0; cval--; )
    {
        if ((lpval[cval].ulPropTag != PR_NULL) &&
            (PROP_TYPE(lpval[cval].ulPropTag) != PT_ERROR))
        {
            /* Mark the property as being there */

            ul |= (1 << cval);
        }
    }
    return ul;
}

/*
 *  HrGetConfigEvent()
 *  
 *  Purpose:
 *  
 *      Gets the configuration event handle.  The handle is used to
 *      signal logged in handlers that their configuration has been
 *      modified and, at the next reasonable chance, should be reloaded.
 *  
 *      Called at SMH init time only.
 *  
 *  Arguments:
 *  
 *      lphevt  [OUT]   contains the handle iff the call succeeds
 *  
 *  Returns:
 *  
 *      (HRESULT)
 */
#ifdef  WIN32
HRESULT
HrGetConfigEvent (HANDLE FAR * lphevt)
{
    HANDLE hevt = NULL;
        
    if (!(hevt = CreateEvent (NULL, TRUE, FALSE, lpszConfigEvt)) &&
        !(hevt = OpenEvent (EVENT_MODIFY_STATE, FALSE, lpszConfigEvt)))
        ResultFromScode (MAPI_E_NOT_ENOUGH_RESOURCES);

    *lphevt = hevt;
    return hrSuccess;
}
#endif  /* WIN32 */


/*
 *  SignalConfigChanged()
 *  
 *  Purpose:
 *  
 *      Sets the configuration event such that logged in hooks can update
 *      their configuration on the fly.
 *  
 *      Called from within the service entry when configuration changes
 *      are commited.
 */
#ifdef  WIN32
VOID
SignalConfigChanged (VOID)
{
    HANDLE hevt = NULL;
    
    if (hevt = OpenEvent (EVENT_MODIFY_STATE, FALSE, lpszConfigEvt))
    {
        SetEvent (hevt);
        CloseHandle (hevt);
    }
    return;
}
#endif  /* WIN32 */


/*
 *  FConfigChanged()
 *  
 *  Purpose:
 *  
 *      Tests the configuration event such that logged in hooks can update
 *      their configuration on the fly if the configuration has changed.
 *  
 *      Called from within the SMH object at regualr intervals
 *  
 *  Returns:
 *  
 *      TRUE iff the config changed
 *  
 */
#ifdef  WIN32
BOOL
FConfigChanged (HANDLE hevt)
{
    ULONG dw;

    dw = WaitForSingleObject (hevt, 0);
    Assert (dw != WAIT_ABANDONED);
    return (dw == WAIT_OBJECT_0);
}
#endif  /* WIN32 */


/*
 *  SMH_ServiceEntry()
 *  
 *  Purpose:
 *  
 *      The ServiceEntry() function is the MAPI entry point to configure
 *      a service for use in a profile.  The call can then bring up UI to
 *      ensure configuration of the SMH provider.
 *  
 *  Parameters:
 *  
 *      hinst           DLL instance
 *      lpmalloc        OLE style allocator (used by PropSheet())
 *      lpsup           MAPI profile support object
 *      ulUIParam       hwnd that is to be used as UI parent
 *      ulFlags         configuration flags
 *      ulContext       configuration action
 *      cval            count of caller supplied properties
 *      lpval           caller supplied properties to be configured
 *      lpadmin         IProviderAdmin object
 *      lppmerr [OUT]   extended error information
 *  
 *  Operation:
 *  
 *      The ServiceEntry() uses the IProviderAdmin object to open its
 *      profile section and retrieve the current set of properties.  The
 *      caller supplied properties are then merged into the set of
 *      current properties.
 *  
 *      If either this set of properties is not sufficient for
 *      configuration or the caller specifically asked for configuration
 *      UI, then ServiceEntry() will make calls to bring up its config
 *      UI.
 *  
 *      ServiceEntry() recognizes several configuration flags.  If
 *      SERVICE_UI_ALWAYS and/or SERVICE_UI_ALLOWED are set, UI is
 *      allowed and we be brought up if appropriate.  Is
 *      MSG_SERVICE_UI_READ_ONLY is set, then the UI should not 
 *      allow the configuration to be modified.
 *  
 *      The configuration contexts MSG_SERVICE_DELETE, MSG_SERVICE_INSTALL, 
 *      and MSG_SERVICE_UNINSTALL are ignored and no action is taken.
 *      MSG_SERVICE_CONFIGURE and MSG_SERVICE_CREATE allow the caller to
 *      create or update the configuration properties in this providers
 *      profile section. 
 *  
 *      SMH will not return extended information in the MAPIERROR in case
 *      of error
 */
HRESULT STDAPICALLTYPE
SMH_ServiceEntry(
    HINSTANCE hinst,
    LPMALLOC lpmalloc,
    LPMAPISUP lpsup,
    ULONG ulUIParam,
    ULONG ulFlags,
    ULONG ulContext,
    ULONG cval,
    LPSPropValue lpval,
    LPPROVIDERADMIN lpadmin,
    LPMAPIERROR FAR * lppmerr)
{
    HRESULT hr = hrSuccess;
    BOOL fUI = FALSE;
    BOOL fSave = FALSE;
    LPALLOCATEBUFFER lpfnAlloc = NULL;
    LPALLOCATEMORE lpfnAllocMore = NULL;
    LPFREEBUFFER lpfnFree = NULL;
    LPPROFSECT lpprof = NULL;
    LPPROFSECT lpprofSvc = NULL;
    LPSPropValue lpvalCur = NULL;
    LPSPropValue lpvalNew = NULL;
    LPSMHDLG lpsmhdlg = NULL;
    ULONG cvalCur;
    ULONG cvalNew;
    ULONG ulMyFlags;

    if ((ulContext == MSG_SERVICE_INSTALL) ||
        (ulContext == MSG_SERVICE_UNINSTALL))
        goto ret;

    if ((ulContext != MSG_SERVICE_CONFIGURE) &&
        (ulContext != MSG_SERVICE_CREATE) &&
        (ulContext != MSG_SERVICE_DELETE))
    {
        hr = ResultFromScode (MAPI_E_NO_SUPPORT);
        goto ret;
    }

    if (ulFlags & MAPI_UNICODE)
    {
        /* Unicode is not supported by SMH */

        hr = ResultFromScode (MAPI_E_BAD_CHARWIDTH);
        goto ret;
    }

    /* Find out our UI options */

    fUI = !!(ulFlags & SERVICE_UI_ALWAYS);
    ulMyFlags = (ulFlags & MSG_SERVICE_UI_READ_ONLY)
        ? UI_READONLY
        : 0;

    /* Get memory routines */

    hr = lpsup->lpVtbl->GetMemAllocRoutines (lpsup,
                            &lpfnAlloc,
                            &lpfnAllocMore,
                            &lpfnFree);
    if (HR_FAILED (hr))
        goto ret;

    /* Open the profile section */

    hr = HrOpenSingleProvider (lpadmin, &lpprof);
    if (HR_FAILED (hr))
        goto ret;

    /* Get the values already in the profile */

    hr = lpprof->lpVtbl->GetProps (lpprof,
                            (LPSPropTagArray)&sptConfigProps,
                            0,
                            &cvalCur,
                            &lpvalCur);
    if (HR_FAILED (hr))
        goto ret;

    if (ulContext != MSG_SERVICE_DELETE)
    {
        /* Merge what was in the profile with what was passed in */

        hr = HrMergeValues (cvalCur,
                            lpvalCur,
                            cval,
                            lpval,
                            lpfnAlloc,
                            &cvalNew,
                            &lpvalNew);
        if (HR_FAILED (hr))
            goto ret;

        /* If we dont have all the props we need, then
         *  we will have to ask for them
         */
        fUI |= (REQ_PROPS != (UlChkReqProps (cvalNew, lpvalNew) & REQ_PROPS));
        if ((REQ_PROPS != (UlChkReqProps (cvalNew, lpvalNew) & REQ_PROPS)) &&
            (!(ulFlags & (SERVICE_UI_ALLOWED | SERVICE_UI_ALWAYS)) ||
            (ulFlags & MSG_SERVICE_UI_READ_ONLY)))
        {
            /* We need UI but can't have it. */

            hr = ResultFromScode (MAPI_E_UNCONFIGURED);
            goto ret;
        }

        if (fUI)
        {
            /* Do the config dialog */

            if (!FAILED ((*lpfnAlloc) (sizeof(SMHDLG), &lpsmhdlg)))
            {
                memset (lpsmhdlg, 0, sizeof(SMHDLG));
                lpsmhdlg->hinst = hinst;
                lpsmhdlg->hwnd = (HWND)ulUIParam;
                lpsmhdlg->lpfnAlloc = lpfnAlloc;
                lpsmhdlg->lpfnAllocMore = lpfnAllocMore;
                lpsmhdlg->lpfnFree = lpfnFree;
                lpsmhdlg->lpmalloc = lpmalloc;
                lpsmhdlg->lpspt = (LPSPropTagArray)&sptConfigProps;
                lpsmhdlg->lpvalSMH = lpvalNew;
                lpsmhdlg->lpsec = lpprof;
                lpsmhdlg->lpsup = lpsup;
                lpsmhdlg->lpadmin = lpadmin;
                lpsmhdlg->ulFlags = ulMyFlags;
                hr = HrDisplayPropSheets (hinst, (HWND)ulUIParam, lpsmhdlg);
                fSave = lpsmhdlg->fDirty;
                (*lpfnFree) (lpsmhdlg);
            }
            else
            {
                hr = ResultFromScode (MAPI_E_NOT_ENOUGH_MEMORY);
                goto ret;
            }
        }

        /* Set what we got and save the changes */

        if (fSave)
        {
            HRESULT hrT;
            
            hrT = lpprof->lpVtbl->SaveChanges (lpprof, 0);
            if (HR_FAILED (hrT))
            {
                hr = hrT;
                goto ret;
            }
            SignalConfigChanged ();
        }

        /* Open the service profile section and set PR_SERVICE_EXTRA_UIDS */

        if (!HR_FAILED (lpadmin->lpVtbl->OpenProfileSection (lpadmin,
                                            NULL,
                                            NULL,
                                            MAPI_MODIFY,
                                            &lpprofSvc)))
        {
            lpvalNew[ipRules].ulPropTag = PR_SERVICE_EXTRA_UIDS;
            if (!HR_FAILED (HrSetOneProp ((LPMAPIPROP)lpprofSvc, &lpvalNew[ipRules])))
                (void)lpprofSvc->lpVtbl->SaveChanges (lpprofSvc, 0);

            UlRelease (lpprofSvc);
        }
    }
    
ret:

    if (lpfnFree)
    {
        (*lpfnFree) (lpvalCur);
        (*lpfnFree) (lpvalNew);
    }

    UlRelease (lpprof);
    DebugTraceResult (SMH_ServiceEntry, hr);
    return hr;
};
