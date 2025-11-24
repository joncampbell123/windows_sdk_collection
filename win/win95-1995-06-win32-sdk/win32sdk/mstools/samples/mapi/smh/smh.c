/*
 *  S M H . C
 *
 *  Sample mail handling hook
 *
 *  Purpose:
 *
 *      The sample mail handling (SMH) hook illustrates the use of the
 *      MAPI Spooler Hook Provider Interface (ISpoolerHook) and those
 *      parts of the general MAPI API that are available to a spooler
 *      hook implementation.
 *
 *      Specifically, SMH illusttrates the operation of both an inbound
 *      message hook as well as outbound.  SMH also has examples of the
 *      configuration of a spooler hook provider via calls to SMH's
 *      ServiceEntry() and/or through calls from the Profile Wizard.
 *
 *  Features:
 *
 *    Sent Mail:
 *
 *      SMH allows the archiving of outbound messages (sent mail), into a
 *      well defined set of subfolders in the default stores sent mail
 *      folder.  The archive folders are comprised of monthly archive
 *      folders.  The monthly folders can be, optionally, created under a
 *      year based folder in the sent mail folder.  Thus in a typical
 *      message store, a fully archived sent mail folder might have a
 *      hierarchy that looks similar to the following:
 *
 *          Sent Mail
 *              |
 *              |-- 1994
 *              |    |
 *              |    |-- 10 October
 *              |    |-- 11 November
 *              |    |-- 12 December
 *              |
 *              |-- 1995
 *                   |
 *                   |-- 01 January
 *
 *      This allows for a mail user to organize their outgoing mail in
 *      a managible fashion.
 *
 *    Deleted Mail:
 *
 *      SMH allows the archiving of deleted mail in the same fashion as
 *      sent mail can be archived.  This feature helps people who choose
 *      keep their 'deleted' mail accessible.  It should be noted here
 *      that this feature does not make use of the ISpoolerHook
 *      interface, but is an example of what all can be done with spooler
 *      hook providers.
 *
 *    Incoming Mail:
 *
 *      SMH can also 'filter' incoming messages and route the message
 *      directly to folders, other than the default stores inbox, based
 *      on message content.  The user can define any number of filters
 *      that can help organize and manage the email.
 *
 *    Unread Search Folders:
 *
 *      Because SMH can filter unread messages deep into a message store
 *      folder hierarchy, SMH can also create a search folder in the root
 *      of the IPM_SUBTREE that searches the entire subtree for unread
 *      messages.
 *
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

#include "_pch.h"

/*
 *  FIsLeapYear()
 *
 *  Used to calculate leap years when determining month ranges for
 *  archive folders.
 */
#define FIsLeapYear(_yr) ((!((_yr) % 400) || ((_yr) % 100) && !((_yr) % 4)) ? TRUE : FALSE)

/*
 *  sptMsgDates
 *
 *  The list of properties that are used for archiving.  If
 *  PR_MESSAGE_DELIVERY_TIME is not available, then SMH will use
 *  PR_CLIENT_SUBMIT_TIME.
 */
const static SizedSPropTagArray(2, sptMsgDates) =
{
    2,
    {
        PR_MESSAGE_DELIVERY_TIME,
        PR_CLIENT_SUBMIT_TIME
    }
};

/*
 *  sptFldrDates
 *
 *  Theses properties describe the range of dates that an archive folder
 *  with the returned EntryID will support.  This is used in the call to
 *  find a supporting sub-folder.
 */
const static SizedSPropTagArray (3, sptFldrDates) =
{
    3,
    {
        PR_START_DATE,
        PR_END_DATE,
        PR_ENTRYID
    }
};

/*
 *  rgtstrMonth
 *  rgtstrMonthFull
 *  rgwDaysPerMonth
 *
 *  These arrays are used in the calculation and creation of supporting
 *  archive folders.
 */
const static TCHAR * rgtstrMonth[] =
{
    TEXT ("Jan"),
    TEXT ("Feb"),
    TEXT ("Mar"),
    TEXT ("Apr"),
    TEXT ("May"),
    TEXT ("Jun"),
    TEXT ("Jul"),
    TEXT ("Aug"),
    TEXT ("Sep"),
    TEXT ("Oct"),
    TEXT ("Nov"),
    TEXT ("Dec")
};
const static TCHAR * rgtstrMonthFull[] =
{
    TEXT ("January"),
    TEXT ("February"),
    TEXT ("March"),
    TEXT ("April"),
    TEXT ("May"),
    TEXT ("June"),
    TEXT ("July"),
    TEXT ("August"),
    TEXT ("September"),
    TEXT ("October"),
    TEXT ("November"),
    TEXT ("December")
};
const static WORD rgwDaysPerMonth[] =
{
    31, //  JAN
    28, //  FEB
    31, //  MAR
    30, //  APR
    31, //  MAY
    30, //  JUN
    31, //  JUL
    31, //  AUG
    30, //  SEP
    31, //  OCT
    30, //  NOV
    31  //  DEC
};

/*
 *  sptMessageProps
 *
 *  These are the properties that are required to check filters against
 *  messages.
 */
const static SizedSPropTagArray (cpMsgPrps, sptMsgPrps) =
{
    cpMsgPrps,
    {
        PR_MESSAGE_FLAGS,
        PR_SUBJECT,
        PR_SENT_REPRESENTING_NAME,
        PR_SENT_REPRESENTING_EMAIL_ADDRESS
    }
};

extern LPTSTR lpszConfigEvt;
extern SPropTagArray sptRule;
extern SPropTagArray sptConfigProps;

/*
 *  vtblSMH
 *
 *  This is the SMH object's vtable.  The table and its functions are
 *  defined by MAPI's ISpoolerHook interface.
 */
static const SMH_Vtbl vtblSMH =
{
    SMH_QueryInterface,
    SMH_AddRef,
    SMH_Release,
    SMH_InboundMsgHook,
    SMH_OutboundMsgHook
};


/*
 *  HrCacheFolder()
 *
 *  Purpose:
 *
 *      Caches the passed in entryid along with a matching folder
 *
 *  Arguments:
 *
 *      lpsmh       pointer to the sentmail handler
 *      cbeid       count of bytes for the entryid to check
 *      lpeid       data for the entryid to check
 *      lpcbeid     points to the cached entryid size
 *      lppeid      points to the cached entryid data
 *      lppfldr     points to the cached mapi folder object
 *      lpfUpdated  points to cache update flag
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpcbeid     [OUT] size of newly cached entryid
 *      lppeid      [OUT] data of newly cached entryid
 *      lppfldr     [OUT] folder corresponding to cached entryid
 *      lpfUpdated  [OUT] TRUE iff the out folder is not the
 *                          previously cached folder
 */
HRESULT
HrCacheFolder (LPSMH lpsmh,
    ULONG cbeid,
    LPENTRYID lpeid,
    ULONG FAR * lpcbeid,
    LPENTRYID FAR * lppeid,
    LPMAPIFOLDER FAR * lppfldr,
    BOOL far * lpfUpdated)
{
    HRESULT hr;
    ULONG ulType;
    ULONG ulMatch;
    LPMAPIPROP lpmp = NULL;

    /* Init the update flag */

    *lpfUpdated = FALSE;

    /*  Is the topmost sent mail folder the same folder
     *  as the last filtered message?
     */
    hr = lpsmh->lpsess->lpVtbl->CompareEntryIDs (lpsmh->lpsess,
                                        cbeid,
                                        lpeid,
                                        *lpcbeid,
                                        *lppeid,
                                        0,
                                        &ulMatch);
    if (HR_FAILED (hr) || !ulMatch)
    {
        /* Different folder, guess we better toss the cached one */

        (*lpsmh->lpfnFree) (*lppeid);
        *lppeid = NULL;
        *lpcbeid = 0;

        /* Cache the SentMail */

        hr = ResultFromScode ((*lpsmh->lpfnAlloc) (cbeid, lppeid));
        if (HR_FAILED (hr))
            goto ret;
        memcpy (*lppeid, lpeid, (UINT)cbeid);
        *lpcbeid = cbeid;
        *lpfUpdated = TRUE;
    }
    else if (*lppfldr)
        return hrSuccess;

    /* Open the new folder */

    hr = lpsmh->lpsess->lpVtbl->OpenEntry (lpsmh->lpsess,
                                        cbeid,
                                        lpeid,
                                        NULL,
                                        MAPI_BEST_ACCESS,
                                        &ulType,
                                        (LPUNKNOWN FAR *)&lpmp);
    if (HR_FAILED (hr))
        goto ret;

    if (ulType != MAPI_FOLDER)
    {
        hr = ResultFromScode (MAPI_E_UNEXPECTED_TYPE);
        goto ret;
    }

ret:

    if (HR_FAILED (hr))
    {
        UlRelease (lpmp);
        (*lpsmh->lpfnFree) (*lppeid);
        *lppeid = NULL;
        *lpcbeid = 0;
        lpmp = NULL;
    }

    UlRelease (*lppfldr);
    *lppfldr = (LPMAPIFOLDER)lpmp;

    DebugTraceResult (HrCacheFolder(), hr);
    return hr;
}


/*
 *  HrCreateHashedFolder()
 *
 *  Purpose:
 *
 *      Create/Caches the a folder that satisfies the hash value
 *
 *  Arguments:
 *
 *      lpsmh       pointer to the sentmail handler
 *      lpdft       pointer the the hash interval
 *      lpfldrPar   parnet folder
 *      lpszName    name for folder
 *      lpszComment comment for folder
 *      lpcbeid     points to the cached entryid size
 *      lppeid      points to the cached entryid data
 *      lppfldr     points to the cached mapi folder object
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpcbeid     [OUT] size of newly cached entryid
 *      lppeid      [OUT] data of newly cached entryid
 *      lppfldr     [OUT] folder corresponding to cached entryid
 */
HRESULT
HrCreateHashedFolder (LPSMH lpsmh,
    LPDFT lpdft,
    LPMAPIFOLDER lpfldrPar,
    LPTSTR lpszName,
    LPTSTR lpszComment,
    ULONG FAR * lpcbeid,
    LPENTRYID FAR * lppeid,
    LPMAPIFOLDER FAR * lppfldr)
{
    HRESULT hr;
    LPMAPIFOLDER lpfldr = NULL;
    LPSPropValue lpval = NULL;
    SPropValue rgval[2] = {0};
    ULONG cval;

    /* Toss the current cache info */

    (*lpsmh->lpfnFree) (*lppeid);
    *lppeid = NULL;
    *lpcbeid = 0;

    /* Create the new folder */

    hr = lpfldrPar->lpVtbl->CreateFolder (lpfldrPar,
                            FOLDER_GENERIC,
                            lpszName,
                            lpszComment,
                            NULL,
                            OPEN_IF_EXISTS,
                            &lpfldr);
    if (HR_FAILED (hr))
        goto ret;

    /* Set the hashing interval properties */

    rgval[0].ulPropTag = PR_START_DATE;
    rgval[0].Value.ft = lpdft->ftStart;
    rgval[1].ulPropTag = PR_END_DATE;
    rgval[1].Value.ft = lpdft->ftEnd;
    hr = lpfldr->lpVtbl->SetProps (lpfldr, 2, rgval, NULL);
    if (HR_FAILED (hr))
        goto ret;

    /* Cache the folder info */

    hr = lpfldr->lpVtbl->GetProps (lpfldr,
                            (LPSPropTagArray)&sptFldrDates,
                            FALSE,
                            &cval,
                            &lpval);
    if (HR_FAILED (hr))
        goto ret;

    /* Make sure we have all the info we need */

    if ((lpval[0].ulPropTag != PR_START_DATE) ||
        (lpval[1].ulPropTag != PR_END_DATE) ||
        (lpval[2].ulPropTag != PR_ENTRYID))
    {
        hr = ResultFromScode (MAPI_E_BAD_VALUE);
        goto ret;
    }

    /* Cache the entryid */

    hr = ResultFromScode ((*lpsmh->lpfnAlloc) (lpval[2].Value.bin.cb, lppeid));
    if (HR_FAILED (hr))
        goto ret;
    memcpy (*lppeid, lpval[2].Value.bin.lpb, (UINT)lpval[2].Value.bin.cb);
    *lpcbeid = lpval[2].Value.bin.cb;

ret:

    (*lpsmh->lpfnFree) (lpval);
    if (HR_FAILED (hr))
    {
        UlRelease (lpfldr);
        lpfldr = NULL;
    }
    UlRelease (*lppfldr);
    *lppfldr = lpfldr;

    DebugTraceResult (HrCreateHashedFolder(), hr);
    return hr;
}


/*
 *  HrCacheHashedFolder()
 *
 *  Purpose:
 *
 *      Caches the folder that matches the hash value (file time)
 *
 *  Arguments:
 *
 *      lpsmh       pointer to the sentmail handler
 *      ft          hash filetime
 *      lpdft       pointer the the hash interval
 *      lpcbeid     points to the cached entryid size
 *      lppeid      points to the cached entryid data
 *      lppfldr     points to the cached mapi folder object
 *      lpfUpdated  points to cache update flag
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpcbeid     [OUT] size of newly cached entryid
 *      lppeid      [OUT] data of newly cached entryid
 *      lppfldr     [OUT] folder corresponding to cached entryid
 *      lpfUpdated  [OUT] TRUE iff the out folder is not the
 *                          previously cached folder
 */
HRESULT
HrCacheHashedFolder (LPSMH lpsmh,
    FILETIME ft,
    LPDFT lpdft,
    LPMAPIFOLDER lpfldr,
    ULONG FAR * lpcbeid,
    LPENTRYID FAR * lppeid,
    LPMAPIFOLDER FAR * lppfldr,
    BOOL far * lpfUpdated)
{
    HRESULT hr;
    LPMAPIPROP lpmp = NULL;
    LPMAPITABLE lptbl = NULL;
    LPSRow lprw = NULL;
    LPSRowSet lprws = NULL;
    ULONG ulType;
    UINT i;

    /*  Check to see if the new hash fits the
     *  the current hashed folder, if the hash
     *  value does not work, find one that does
     */
    if (!*lpfUpdated &&
        (CompareFileTime (&lpdft->ftStart, &ft) != 1) &&
        (CompareFileTime (&lpdft->ftEnd, &ft) != -1))
    {
        /* The hash works, but do we have a folder? */

        if (*lppfldr)
            return hrSuccess;

        hr = lpsmh->lpsess->lpVtbl->OpenEntry (lpsmh->lpsess,
                                        *lpcbeid,
                                        *lppeid,
                                        NULL,
                                        MAPI_BEST_ACCESS,
                                        &ulType,
                                        (LPUNKNOWN FAR *)&lpmp);
        if (!HR_FAILED (hr) && (ulType != MAPI_FOLDER))
        {
            hr = ResultFromScode (MAPI_E_UNEXPECTED_TYPE);
            UlRelease (lpmp);
            lpmp = NULL;
        }
        goto ret;
    }

    /* Toss the cached info */

    (*lpsmh->lpfnFree) (*lppeid);
    *lppeid = NULL;
    *lpcbeid = 0;

    /* Get the hierachy and set it up to find the target folder */

    hr = lpfldr->lpVtbl->GetHierarchyTable (lpfldr, 0, &lptbl);
    if (HR_FAILED (hr))
        goto ret;

    hr = lptbl->lpVtbl->SetColumns (lptbl, (LPSPropTagArray)&sptFldrDates, 0);
    if (HR_FAILED (hr))
        goto ret;

    hr = lptbl->lpVtbl->QueryRows (lptbl, 12, 0L, &lprws);
    if (HR_FAILED (hr))
        goto ret;

    while (lprws->cRows)
    {
        for (i = 0; i < lprws->cRows; i++)
        {
            lprw = &lprws->aRow[i];

            if (!lpmp &&
                (lprw->lpProps[0].ulPropTag == PR_START_DATE) &&
                (lprw->lpProps[1].ulPropTag == PR_END_DATE) &&
                (CompareFileTime (&lprw->lpProps[0].Value.ft, &ft) != 1) &&
                (CompareFileTime (&lprw->lpProps[1].Value.ft, &ft) != -1))
            {
                /* Hey, this looks like the folder we want */

                hr = HrCacheFolder (lpsmh,
                        lprw->lpProps[2].Value.bin.cb,
                        (LPENTRYID)lprw->lpProps[2].Value.bin.lpb,
                        lpcbeid,
                        lppeid,
                        (LPMAPIFOLDER FAR *)&lpmp,
                        lpfUpdated);
                if (!HR_FAILED (hr))
                {
                    lpdft->ftStart = lprw->lpProps[0].Value.ft;
                    lpdft->ftEnd = lprw->lpProps[1].Value.ft;
                }
            }
        }

        /* Clean up the row properies */

        for (i = 0; i < lprws->cRows; i++)
            (*lpsmh->lpfnFree) (lprws->aRow[i].lpProps);

        /* We either found the folder or we had an error */

        if (lpmp || HR_FAILED (hr))
            break;

        /* Clean up the row set */

        (*lpsmh->lpfnFree) (lprws);
        hr = lptbl->lpVtbl->QueryRows (lptbl, 12, 0L, &lprws);
        if (HR_FAILED (hr))
            goto ret;
    }

    /* Clean up the final row set */

    (*lpsmh->lpfnFree) (lprws);

ret:

    UlRelease (lptbl);
    UlRelease (*lppfldr);
    *lppfldr = (LPMAPIFOLDER)lpmp;

    DebugTraceResult (HrCacheHashedFolder(), hr);
    return hr ? hr : (lpmp ? hrSuccess : ResultFromScode (MAPI_E_NOT_FOUND));
}


/*
 *  HrArchiveMessage()
 *
 *  Purpose:
 *
 *      The purpose of this function is to "hash" a single message by
 *      processing based on date.  The most obvious bucket size is
 *      monthly but there is no reason not to make this an option the
 *      user could confiigure.
 *
 *  Arguments:
 *
 *      lpsmh           this filter hook obj
 *      lpmsg           the message to be filtered
 *      lpfldrDef       the owning folder of the message
 *      lpmdbDef        the owning store of the message
 *      lpbkit          the cached bucket structure
 *      fCatByYear      uses yearly subfolders iff TRUE
 *      lpcbeid         cb for entryid of default target for message
 *      lppbeid         pb for entryid of default target for message
 *
 *  Operation:
 *
 *      Opens the suggested folder (if needed) and checks for the
 *      existence of the appropriate "bucket" folder.  If it does exist,
 *      then the  folder is created and cached.  The entryid is grabbed
 *      and passed back in to the spooler.
 *
 *      IMPORTANT: the entryid passed in will be swapped out by this
 *      call.  Therefore the *lppeid buffer must be allocated with the
 *      MAPIAllocateBuffer provider to the filter.
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpcbeid [out]   the size of the returned EntryID
 *      lppbeid [out]   the data of the returned EntryID
 *
 */
HRESULT
HrArchiveMessage (LPSMH lpsmh,
    LPMESSAGE lpmsg,
    LPMAPIFOLDER lpfldrDef,
    LPMDB lpmdbDef,
    LPBKIT lpbkit,
    BOOL fCatByYear,
    ULONG FAR * lpcbeid,
    LPBYTE FAR * lppeid)
{
    HRESULT hr = hrSuccess;
    BOOL fUpdated = FALSE;
    FILETIME ft;
    LPMAPIFOLDER lpfldr;
    LPSPropValue lpval = NULL;
    SYSTEMTIME st;
    TCHAR rgchName[64] = {0};
    ULONG cval;

    /* Quick and dirty parameter check */

    if (IsBadWritePtr (lpsmh, sizeof(SMH)) ||
        IsBadWritePtr (lpcbeid, sizeof(ULONG)) ||
        IsBadWritePtr (lppeid, sizeof(LPBYTE)) ||
        IsBadWritePtr (*lppeid, (UINT)(*lpcbeid)))
        return ResultFromScode (MAPI_E_INVALID_PARAMETER);

    /* Get the date used by the hash */

    hr = lpmsg->lpVtbl->GetProps (lpmsg,
                            (LPSPropTagArray)&sptMsgDates,
                            FALSE,
                            &cval,
                            &lpval);
    if (HR_FAILED (hr))
        goto ret;

    /* Make sure what we end up with is usable */

    if (lpval[0].ulPropTag == PR_MESSAGE_DELIVERY_TIME)
    {
        DebugTrace ("SMH: filtering on PR_MESSAGE_DELIVERY_TIME\n");
        ft = lpval[0].Value.ft;
    }
    else if (lpval[1].ulPropTag == PR_CLIENT_SUBMIT_TIME)
    {
        DebugTrace ("SMH: filtering on PR_CLIENT_SUBMIT_TIME\n");
        ft = lpval[0].Value.ft;
    }
    else
    {
        DebugTrace ("SMH: cannot filter on provided time props\n");
        hr = ResultFromScode (MAPI_E_BAD_VALUE);
        goto ret;
    }
    if (!FileTimeToSystemTime (&ft, &st))
    {
        hr = ResultFromScode (MAPI_E_BAD_VALUE);
        goto ret;
    }

    /* Cache the parent folder */

    hr = HrCacheFolder (lpsmh,
                    *lpcbeid,
                    (LPENTRYID)*lppeid,
                    &lpbkit->cbeidParent,
                    &lpbkit->lpeidParent,
                    &lpbkit->lpfldrParent,
                    &fUpdated);
    if (HR_FAILED (hr))
        goto ret;

    if (fCatByYear)
    {
        /* Cache the year folder */

        hr = HrCacheHashedFolder (lpsmh,
                    ft,
                    &lpbkit->dftYr,
                    lpbkit->lpfldrParent,
                    &lpbkit->cbeidYr,
                    &lpbkit->lpeidYr,
                    &lpbkit->lpfldrYr,
                    &fUpdated);
        if (HR_FAILED (hr) && (GetScode (hr) == MAPI_E_NOT_FOUND))
        {
            wsprintf (rgchName, "%04hu", st.wYear);
            st.wMonth = 1;
            st.wDayOfWeek = 0;
            st.wDay = 1;
            st.wHour = 0;
            st.wMinute = 0;
            st.wSecond = 0;
            st.wMilliseconds = 0;
            if (!SystemTimeToFileTime (&st, &lpbkit->dftYr.ftStart))
            {
                hr = ResultFromScode (MAPI_E_BAD_VALUE);
                goto ret;
            }
            st.wDay = rgwDaysPerMonth[st.wMonth - 1];
            st.wMonth = 12;
            st.wDayOfWeek = 0;
            st.wDay = 31;
            st.wHour = 23;
            st.wMinute = 59;
            st.wSecond = 59;
            st.wMilliseconds = 999;
            if (!SystemTimeToFileTime (&st, &lpbkit->dftYr.ftEnd))
            {
                hr = ResultFromScode (MAPI_E_BAD_VALUE);
                goto ret;
            }
            hr = HrCreateHashedFolder (lpsmh,
                            &lpbkit->dftYr,
                            lpbkit->lpfldrParent,
                            rgchName,
                            NULL,
                            &lpbkit->cbeidYr,
                            &lpbkit->lpeidYr,
                            &lpbkit->lpfldrYr);
            if (HR_FAILED (hr))
                goto ret;
        }
        else if (HR_FAILED (hr))
            goto ret;

        lpfldr = lpbkit->lpfldrYr;
    }
    else
        lpfldr = lpbkit->lpfldrParent;

    /* Cache the hashed target folder */

    hr = HrCacheHashedFolder (lpsmh,
                    ft,
                    &lpbkit->dft,
                    lpfldr,
                    &lpbkit->cbeid,
                    &lpbkit->lpeid,
                    &lpbkit->lpfldr,
                    &fUpdated);
    if (HR_FAILED (hr) && (GetScode (hr) == MAPI_E_NOT_FOUND))
    {
        if (!FileTimeToSystemTime (&ft, &st))
        {
            hr = ResultFromScode (MAPI_E_BAD_VALUE);
            goto ret;
        }
        if (fCatByYear)
        {
            wsprintf (rgchName,
                "%02hu %s",
                st.wMonth,
                rgtstrMonthFull[st.wMonth - 1]);
        }
        else
        {
            wsprintf (rgchName,
                TEXT("'%02hu/%02hu %s"),
                st.wYear % 100,
                st.wMonth,
                rgtstrMonthFull[st.wMonth - 1]);
        }
        st.wDayOfWeek = (st.wDay - st.wDayOfWeek - 1) % 7;
        st.wDay = 1;
        st.wHour = 0;
        st.wMinute = 0;
        st.wSecond = 0;
        st.wMilliseconds = 0;
        if (!SystemTimeToFileTime (&st, &lpbkit->dft.ftStart))
        {
            hr = ResultFromScode (MAPI_E_BAD_VALUE);
            goto ret;
        }
        st.wDay = rgwDaysPerMonth[st.wMonth - 1];
        if ((st.wMonth == 1) && FIsLeapYear (st.wYear))
            st.wDay += 1;
        st.wDayOfWeek = (st.wDayOfWeek + st.wDay - 1) % 7;
        st.wHour = 23;
        st.wMinute = 59;
        st.wSecond = 59;
        st.wMilliseconds = 999;
        if (!SystemTimeToFileTime (&st, &lpbkit->dft.ftEnd))
        {
            hr = ResultFromScode (MAPI_E_BAD_VALUE);
            goto ret;
        }
        hr = HrCreateHashedFolder (lpsmh,
                            &lpbkit->dft,
                            lpfldr,
                            rgchName,
                            NULL,
                            &lpbkit->cbeid,
                            &lpbkit->lpeid,
                            &lpbkit->lpfldr);
    }

ret:

    if (!HR_FAILED (hr))
    {
        LPBYTE lpeid;

        /* OK, If we get this far we are moving the message */

        hr = ResultFromScode ((*lpsmh->lpfnAlloc) (lpbkit->cbeid, &lpeid));
        if (HR_FAILED (hr))
            goto ret;
        memcpy (lpeid, lpbkit->lpeid, (UINT)lpbkit->cbeid);
        (*lpsmh->lpfnFree) (*lppeid);
        *lpcbeid = lpbkit->cbeid;
        *lppeid = lpeid;
    }
    (*lpsmh->lpfnFree) (lpval);
    lpval = NULL;

    DebugTraceResult (HrArchiveMessage(), hr);
    return hr;
}


/*
 *  HrFilterDeleted()
 *
 *  Purpose:
 *
 *      Filters all the current message from the 'Wastebasket'/'Deleted Items'
 *      folder based on the archiving model used in sent mail processing.
 *
 *  Arguments:
 *
 *      lpwb            wastbucket struct for the current store
 *      lpbin           sbinary holding the entryid of the message
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrFilterDeleted (LPWB lpwb, LPSBinary lpbin)
{
    HRESULT hr = ResultFromScode (MAPI_E_NOT_ENOUGH_MEMORY);
    LPBYTE lpeid = NULL;
    LPMAPIFOLDER lpfldr = lpwb->lpfldr;
    LPMDB lpmdb = lpwb->lpmdb;
    LPMESSAGE lpmsg = NULL;
    LPSMH lpsmh = lpwb->lpsmh;
    SBinaryArray sba = {1, lpbin};
    ULONG cb = lpwb->lpvalEid->Value.bin.cb;
    ULONG ulFlags = 0;
    ULONG ulType;

    if (!FAILED ((*lpsmh->lpfnAlloc) (cb, &lpeid)))
    {
        hr = lpmdb->lpVtbl->OpenEntry (lpmdb,
                                lpbin->cb,
                                (LPENTRYID)lpbin->lpb,
                                NULL,
                                0,
                                &ulType,
                                (LPUNKNOWN FAR *)&lpmsg);
        if (!HR_FAILED (hr))
        {
            memcpy (lpeid, lpwb->lpvalEid->Value.bin.lpb, (UINT)cb);
            hr = HrArchiveMessage (lpsmh,
                                lpmsg,
                                lpfldr,
                                lpmdb,
                                &lpwb->bkit,
                                lpsmh->fCatWb,
                                &cb,
                                &lpeid);
            if (!HR_FAILED (hr))
            {
                hr = lpfldr->lpVtbl->CopyMessages (lpfldr,
                                &sba,
                                NULL,
                                lpwb->bkit.lpfldr,
                                0,
                                NULL,
                                MAPI_MOVE);
            }
        }
    }
    (*lpsmh->lpfnFree) (lpeid);
    DebugTraceResult (HrFilterDeleted(), hr);
    return hr;
}


/*
 *  WBNotify()
 *
 *  Purpose:
 *
 *      Notification callback on the WB folders of message stores.  When
 *      rows are added to the WB contents table, we enum the table and
 *      filter each message added.
 *
 *  Arguments:
 *
 *      lpv         void pointer to current WB struct
 *      cntf        count of notifications
 *      lpntf       notifications
 *
 *  Returns:
 *
 *      (SCODE)
 */
STDAPI_(SCODE)
WBNotify (LPVOID lpv, ULONG cntf, LPNOTIFICATION lpntf)
{
    HRESULT hr;
    LPMAPITABLE lptbl;
    LPSMH lpsmh;
    LPSRowSet lprws = NULL;
    LPWB lpwb;
    UINT irw;

    /* Quick and dirty check on the context */

    if (IsBadReadPtr (lpv, sizeof(WB)) ||
        IsBadReadPtr (((LPWB)lpv)->lpsmh, sizeof(SMH)))
        return S_OK;

    lpwb = (LPWB)lpv;
    lptbl = lpwb->lptbl;
    lpsmh = lpwb->lpsmh;

    /* Just incase we were turned off */

    if (lpsmh->fCatWb)
    {
        while (cntf--)
        {
            Assert (lpntf->ulEventType == fnevTableModified);
            if (lpntf->info.tab.ulTableEvent == TABLE_ROW_ADDED)
            {
                lptbl->lpVtbl->SeekRow (lptbl, BOOKMARK_BEGINNING, 0, NULL);

                while (TRUE)
                {
                    hr = lptbl->lpVtbl->QueryRows (lptbl, 5, 0, &lprws);
                    if (HR_FAILED (hr))
                        break;

                    for (irw = 0; irw < lprws->cRows; irw++)
                    {
                        HrFilterDeleted (lpwb, &lprws->aRow[irw].lpProps[0].Value.bin);
                        (*lpsmh->lpfnFree) (lprws->aRow[irw].lpProps);
                    }

                    if (!lprws->cRows)
                        break;

                    (*lpsmh->lpfnFree) (lprws);
                    lprws = NULL;
                }
                break;
            }
            lpntf++;
        }
        (*lpsmh->lpfnFree) (lprws);
    }
    return S_OK;
}


/*
 *  HrInitDeletedMailFilter()
 *
 *  Purpose:
 *
 *      Inits the deleted mail filters by opening the store, finding the
 *      WB folder, opening the contents table of the WB, and registering
 *      for TABLE_CHANGED notifications.
 *
 *  Arguments:
 *
 *      lpsmg           the sample mail handler object
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrInitDeletedMailFilter (LPSMH lpsmh)
{
    HRESULT hr;
    LPMAPIADVISESINK lpadvz = NULL;
    LPMAPIFOLDER lpfldr = NULL;
    LPMAPITABLE lptbl = NULL;
    LPMDB lpmdb = NULL;
    LPSPropValue lpval = NULL;
    LPWB lpwb = NULL;
    SizedSPropTagArray (1, spt) = {1, { PR_ENTRYID }};
    ULONG ulType;
    UINT cerr = 0;
    UINT i;

    for (i = 0; i < lpsmh->lpstotbl->cSto; i++)
    {
        hr = ResultFromScode ((*lpsmh->lpfnAlloc) (sizeof(WB), &lpwb));
        if (HR_FAILED (hr))
            goto nxt;
        memset (lpwb, 0, sizeof(WB));

        hr = HrOpenStoEntry (lpsmh->lpsess, &lpsmh->lpstotbl->aSto[i], &lpmdb);
        if (HR_FAILED (hr))
            goto nxt;

        hr = HrGetOneProp ((LPMAPIPROP)lpmdb, PR_IPM_WASTEBASKET_ENTRYID, &lpval);
        if (HR_FAILED (hr))
            goto nxt;

        hr = lpmdb->lpVtbl->OpenEntry (lpmdb,
                                lpval->Value.bin.cb,
                                (LPENTRYID)lpval->Value.bin.lpb,
                                NULL,
                                MAPI_MODIFY,
                                &ulType,
                                (LPUNKNOWN FAR *)&lpfldr);
        if (HR_FAILED (hr))
            goto nxt;

        hr = lpfldr->lpVtbl->GetContentsTable (lpfldr, 0, &lptbl);
        if (HR_FAILED (hr))
            goto nxt;

        hr = lptbl->lpVtbl->SetColumns (lptbl, (LPSPropTagArray)&spt, 0);
        if (HR_FAILED (hr))
            goto nxt;

        hr = HrAllocAdviseSink ((LPNOTIFCALLBACK)&WBNotify, lpwb, &lpadvz);
        if (HR_FAILED (hr))
            goto nxt;

        hr = lptbl->lpVtbl->Advise (lptbl, fnevTableModified, lpadvz, &lpwb->ulAdvz);
        if (HR_FAILED (hr))
            goto nxt;

        UlAddRef (lptbl);
        UlAddRef (lpfldr);
        lpwb->lpmdb = lpmdb;
        lpwb->lptbl = lptbl;
        lpwb->lpfldr = lpfldr;
        lpwb->lpvalEid = lpval;
        lpwb->lpsmh = lpsmh;
        lpval = NULL;

        /* Hook it in */

        lpwb->wbNext = lpsmh->lstWb;
        lpsmh->lstWb = lpwb;
        lpwb = NULL;
nxt:

        if (HR_FAILED (hr))
            cerr++;

        (*lpsmh->lpfnFree) (lpval);
        lpval = NULL;

        (*lpsmh->lpfnFree) (lpwb);
        lpwb = NULL;

        UlRelease (lpadvz);
        lpadvz = NULL;

        UlRelease (lpfldr);
        lpfldr = NULL;

        UlRelease (lptbl);
        lptbl = NULL;
    }

    hr = ResultFromScode (cerr ? MAPI_W_ERRORS_RETURNED : S_OK);
    DebugTraceResult (HrInitDeletedMailFilter(), hr);
    return hr;
}


/*
 *  HrInitUnreadSearch()
 *
 *  Purpose:
 *
 *      Inits/creates an 'Unread Messages' search folder in the root of
 *      the given store's IPM_SUBTREE hierarchy.
 *
 *  Arguments:
 *
 *      lpsmh           the sample mail handler object
 *      lpmdb           the store getting the search folder
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrInitUnreadSearch (LPSMH lpsmh)
{
    HRESULT hr;
    ENTRYLIST el = {0};
    LPMAPIFOLDER lpfldr = NULL;
    LPMAPIFOLDER lpfldrUM = NULL;
    LPMDB lpmdb = NULL;
    LPSPropValue lpval = NULL;
    SRestriction res = {0};
    ULONG ulType = 0;
    UINT cerr = 0;
    UINT i;

    for (i = 0; i < lpsmh->lpstotbl->cSto; i++)
    {
        hr = HrOpenStoEntry (lpsmh->lpsess, &lpsmh->lpstotbl->aSto[i], &lpmdb);
        if (!HR_FAILED (hr))
        {
            hr = HrGetOneProp ((LPMAPIPROP)lpmdb, PR_IPM_SUBTREE_ENTRYID, &lpval);
            if (!HR_FAILED (hr))
            {
                hr = lpmdb->lpVtbl->OpenEntry (lpmdb,
                                        lpval->Value.bin.cb,
                                        (LPENTRYID)lpval->Value.bin.lpb,
                                        NULL,
                                        MAPI_MODIFY,
                                        &ulType,
                                        (LPUNKNOWN FAR *)&lpfldr);
                if (!HR_FAILED (hr))
                {
                    hr = lpfldr->lpVtbl->CreateFolder (lpfldr,
                                        FOLDER_SEARCH,
                                        "Unread Messages",
                                        "Simple Mail Handler: unread message search",
                                        NULL,
                                        MAPI_MODIFY | OPEN_IF_EXISTS,
                                        &lpfldrUM);
                    if (!HR_FAILED (hr))
                    {
                        el.cValues = 1;
                        el.lpbin = &lpval->Value.bin;
                        res.rt = RES_BITMASK;
                        res.res.resBitMask.relBMR = BMR_EQZ;
                        res.res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
                        res.res.resBitMask.ulMask = MSGFLAG_READ;
                        hr = lpfldrUM->lpVtbl->SetSearchCriteria (lpfldrUM,
                                        &res,
                                        &el,
                                        RECURSIVE_SEARCH | BACKGROUND_SEARCH | RESTART_SEARCH);
                        UlRelease (lpfldrUM);
                        lpfldrUM = NULL;
                    }
                    UlRelease (lpfldr);
                    lpfldr = NULL;
                }
                (*lpsmh->lpfnFree) (lpval);
                lpval = NULL;
            }
        }
        if (HR_FAILED (hr))
        {
            DebugTrace ("SMH: WARNING: failed to init unread search (store:%d)\n", i);
            cerr++;
        }
    }

    hr = ResultFromScode (cerr ? MAPI_W_ERRORS_RETURNED : S_OK);
    DebugTraceResult (HrInitUnreadSearch(), hr);
    return hr;
}


/*
 *  LpszFindChar()
 *
 *  Purpose:
 *
 *      Finds the given character in the passed in string.  This is an
 *      exact matching model so strings should be normalized to either
 *      upper or lower case if case insensitvity is required.
 *
 *  Arguments:
 *
 *      lpszSrc         source string
 *      ch              character to find
 *
 *  Returns:
 *
 *      NULL iff the char was not found, otherwise, the return is a
 *      pointer to the character in the source string.
 */
LPTSTR
LpszFindChar (LPTSTR lpszSrc, TCHAR ch)
{
    LPTSTR lpsz = lpszSrc;

    if (lpszSrc)
    {
        while (*lpsz && (*lpsz != ch))
            lpsz++;
    }
    else
        return NULL;

    return (*lpsz ? lpsz : NULL);
}


/*
 *  FLpszContainsLpsz()
 *
 *  Purpose:
 *
 *      Finds the given sub-string in the passed in string.  This is a
 *      case insensitive matching model that normalizes the strings to
 *      upper case.  So, if preservation is required, pass in copies of
 *      the two strings
 *
 *  Arguments:
 *
 *      lpszSrc         source string
 *      lpsz            string to find
 *
 *  Returns:
 *
 *      FALSE iff the string was not found, TRUE otherwise.
 */
BOOL
FLpszContainsLpsz (LPTSTR lpszSrc, LPTSTR lpsz)
{
    UINT cch = lstrlen(lpsz);
    TCHAR ch;

    CharUpper (lpsz);
    CharUpper (lpszSrc);
    while (lpszSrc = LpszFindChar (lpszSrc, *lpsz))
    {
        if ((UINT)lstrlen(lpszSrc) < cch)
            break;

        ch = *(lpszSrc + cch);
        *(lpszSrc + cch) = 0;

        if (!lstrcmp (lpszSrc, lpsz))
            return TRUE;

        *(lpszSrc + cch) = ch;
        lpszSrc++;
    }
    return FALSE;
}


/*
 *  HrCheckExclusions()
 *
 *  Purpose:
 *
 *      Checks the message class against the list of message classes
 *      excluded from filtering.
 *
 *  Arguments:
 *
 *      lpsmh           the smh parent object
 *      lpmsg           the message to check for exclusion
 *
 *  Returns:
 *
 *      (HRESULT)       If the message is to be excluded, then hrSuccess
 *                      is returned, otherwize MAPI_E_NOT_ME indicates it
 *                      is OK to filter the message
 */
HRESULT
HrCheckExclusions (LPSMH lpsmh, LPMESSAGE lpmsg)
{
    SCODE sc;
    UINT isz = 0;
    LPSPropValue lpvalT = NULL;

    if (lpsmh->valEx.ulPropTag == PR_SMH_EXCLUSIONS)
    {
        if (!HR_FAILED (HrGetOneProp ((LPMAPIPROP)lpmsg, PR_MESSAGE_CLASS, &lpvalT)))
        {
            /* See if this is in the list of exclusions */

            for (isz = 0; isz < lpsmh->valEx.Value.MVSZ.cValues; isz++)
                if (!lstrcmpi (lpvalT->Value.LPSZ, lpsmh->valEx.Value.MVSZ.LPPSZ[isz]))
                    break;

            (*lpsmh->lpfnFree) (lpvalT);
        }
        sc = ((isz == lpsmh->valEx.Value.MVSZ.cValues)
                    ? MAPI_E_NOT_ME
                    : S_OK);
    }
    else
        sc = MAPI_E_NOT_ME;

    DebugTraceSc (HrCheckExclusions(), sc);
    return ResultFromScode (sc);
}


/*
 *  HrCheckRule()
 *  
 *  Purpose:
 *  
 *      Examines a message to see if the current rule applies to the
 *      message. 
 *  
 *      IMPORTANT: If a rule is of type RL_SUBJECT, RL_FROM, or
 *      RL_ATTACH, then the set of properties required to check for
 *      matches with the message are retained and passed back to the
 *      caller such that subsequent calls to HrCheckRule() will not have
 *      to get those props a second time.  THEREFORE the caller is
 *      responsible for cleaning up any returned property values.
 *  
 *  Arguments:
 *  
 *      lprl            pointer to the rule
 *      lpmsg           pointer to the message to examine
 *      lppval  [OUT]   buffer containing the target entryid value struct
 *  
 *  Returns:
 *  
 *      (HRESULT)       If the rule does apply, hrSuccess is returned and
 *                      the lppval parameter will point to a lpval struct
 *                      containing the entryid of the target folder.
 *  
 *                      If the rule does not apply, the return value is
 *                      an HRESULT with MAPI_E_NOT_ME as the SCODE.
 *  
 *                      Otherwise normal error codes apply.
 */
HRESULT
HrCheckRule (LPSMH lpsmh, LPRULE lprl, LPMESSAGE lpmsg, LPSPropValue FAR * lppval)
{
    HRESULT hr;
    LPMAPITABLE lptbl = NULL;
    LPSPropValue lpval = *lppval;
    LPSPropValue lpvalT = NULL;
    ULONG cval;

    if (!lpval)
    {
        if ((lprl->rlTyp == RL_FROM) ||
            (lprl->rlTyp == RL_ATTACH) ||
            (lprl->rlTyp == RL_SUBJECT))
        {
            hr = lpmsg->lpVtbl->GetProps (lpmsg,
                                    (LPSPropTagArray)&sptMsgPrps,
                                    0,
                                    &cval,
                                    &lpval);
            if (HR_FAILED (hr))
                goto ret;
        }
    }

    /* Init for failure */

    hr = ResultFromScode (MAPI_E_NOT_ME);

    if ((lprl->rlTyp == RL_TO) ||
        (lprl->rlTyp == RL_CC) ||
        (lprl->rlTyp == RL_BCC) ||
        (lprl->rlTyp == RL_RECIP))
    {
        hr = lpmsg->lpVtbl->GetRecipientTable (lpmsg, 0, &lptbl);
        if (HR_FAILED (hr))
            goto ret;

        hr = lptbl->lpVtbl->FindRow (lptbl, lprl->lpres, BOOKMARK_BEGINNING, 0);
        UlRelease (lptbl);
        if (HR_FAILED (hr) && (GetScode (hr) == MAPI_E_NOT_FOUND))
            hr = ResultFromScode (MAPI_E_NOT_ME);
    }
    else if (lprl->rlTyp == RL_SUBJECT)
    {
        if (lpval[ipSubj].ulPropTag == PR_SUBJECT)
        {
            if (FLpszContainsLpsz (lpval[ipSubj].Value.LPSZ, lprl->lpszData))
                hr = hrSuccess;
        }
    }
    else if (lprl->rlTyp == RL_FROM)
    {
        if (lpval[ipSentRep].ulPropTag == PR_SENT_REPRESENTING_NAME)
        {
            if (FLpszContainsLpsz (lpval[ipSentRep].Value.LPSZ, lprl->lpszData))
                hr = hrSuccess;
        }
        if (HR_FAILED (hr))
        {
            if (lpval[ipSentRepEA].ulPropTag == PR_SENT_REPRESENTING_EMAIL_ADDRESS)
            {
                if (FLpszContainsLpsz (lpval[ipSentRepEA].Value.LPSZ, lprl->lpszData))
                    hr = hrSuccess;
            }
        }
    }
    else if (lprl->rlTyp == RL_ATTACH)
    {
        if (lpval[ipMsgFlgs].ulPropTag == PR_MESSAGE_FLAGS)
        {
            if (lpval[ipMsgFlgs].Value.l & MSGFLAG_HASATTACH)
                hr = hrSuccess;
        }
    }
    else if (lprl->rlTyp == RL_BODY)
    {
        if (!HR_FAILED (HrGetOneProp ((LPMAPIPROP)lpmsg, PR_BODY, &lpvalT)))
        {
            if (FLpszContainsLpsz (lpvalT->Value.LPSZ, lprl->lpszData))
                hr = hrSuccess;

            (*lpsmh->lpfnFree) (lpvalT);
        }
    }
    else if (lprl->rlTyp == RL_CLASS)
    {
        if (!HR_FAILED (HrGetOneProp ((LPMAPIPROP)lpmsg, PR_MESSAGE_CLASS, &lpvalT)))
        {
            if (FLpszContainsLpsz (lpvalT->Value.LPSZ, lprl->lpszData))
                hr = hrSuccess;

            (*lpsmh->lpfnFree) (lpvalT);
        }
    }

    if (HR_FAILED (hr))
    {
        Assert (GetScode (hr) == MAPI_E_NOT_ME);
        if (lprl->ulFlags & RULE_NOT)
            hr = hrSuccess;
    }

ret:

    *lppval = lpval;

    DebugTraceResult (HrCheckRule(), hr);
    return hr;
}


/*
 *  HrFolderFromPath()
 *
 *  Purpose:
 *
 *      Takes a IPM root-based path string and returns a folder
 *      corresponding to the path given.  The '\' character is the path
 *      separator.  And non-existing folders are created as a psrt of the
 *      process.
 *
 *  Arguments:
 *
 *      lpsmh               pointer to smh parent object
 *      lpmdb               store in which the path is to exist
 *      lpszPath            the root-based path to use
 *      lppfldr     [OUT]   buffer to place target folder
 *      lppvalEid   [OUT]   buffer for target entryid value struct pointer
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrFolderFromPath (LPSMH lpsmh,
    LPMDB lpmdb,
    LPTSTR lpszPath,
    LPMAPIFOLDER FAR * lppfldr,
    LPSPropValue FAR * lppvalEid)
{
    HRESULT hr;
    LPMAPIFOLDER lpfldr = NULL;
    LPMAPIFOLDER lpfldrT = NULL;
    LPSPropValue lpval = NULL;
    LPTSTR lpch;
    TCHAR rgch[MAX_PATH];
    ULONG ulType;

    if (!LoadString (lpsmh->hinst, SMH_FolderComment, rgch, sizeof(rgch)))
        rgch[0] = 0;

    hr = HrGetOneProp ((LPMAPIPROP)lpmdb, PR_IPM_SUBTREE_ENTRYID, &lpval);
    if (!HR_FAILED (hr))
    {
        hr = lpmdb->lpVtbl->OpenEntry (lpmdb,
                            lpval->Value.bin.cb,
                            (LPENTRYID)lpval->Value.bin.lpb,
                            NULL,
                            MAPI_MODIFY,
                            &ulType,
                            (LPUNKNOWN FAR *)&lpfldr);

        (*lpsmh->lpfnFree) (lpval);
        lpval = NULL;

        if (!HR_FAILED (hr))
        {
            do
            {
                if (lpch = LpszFindChar (lpszPath, '\\'))
                    *lpch = 0;

                Assert (lstrlen (lpszPath));
                hr = lpfldr->lpVtbl->CreateFolder (lpfldr,
                            FOLDER_GENERIC,
                            lpszPath,
                            rgch,
                            NULL,
                            MAPI_MODIFY | OPEN_IF_EXISTS,
                            &lpfldrT);
                if (HR_FAILED (hr))
                {
#ifdef  DEBUG
                    LPMAPIERROR lperr = NULL;
                    lpfldr->lpVtbl->GetLastError (lpfldr, hr, 0, &lperr);
                    DebugTrace ("SMH: WARNING: unable to open/create folder: '%s' in %s\n",
                        lperr->lpszError, lperr->lpszComponent);
                    (*lpsmh->lpfnFree) (lperr);
#endif
                    break;
                }

                UlRelease (lpfldr);
                lpfldr = lpfldrT;
                lpfldrT = NULL;

                lpszPath = (lpch ? ++lpch : NULL);

            } while (lpszPath);
        }
        if (!HR_FAILED (hr))
        {
            hr = HrGetOneProp ((LPMAPIPROP)lpfldr, PR_ENTRYID, &lpval);
            if (!HR_FAILED (hr))
            {
                *lppfldr = lpfldr;
                *lppvalEid = lpval;
                lpfldr = NULL;
                lpval = NULL;
            }
        }
    }

    (*lpsmh->lpfnFree) (lpval);
    UlRelease (lpfldr);

    DebugTraceResult (HrFolderFromPath(), hr);
    return hr;
}


/*
 *  HrBuildRule()
 *
 *  Purpose:
 *
 *      Takes a profile section and builds a rule structure that
 *      corresponds to the properties in the profile section.
 *
 *  Arguments:
 *
 *      lpsmh           pointer to smh parent object
 *      lpmuid          profile section UID
 *      lpprl   [OUT]   buffer for the newly created rule pointer
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrBuildRule (LPSMH lpsmh, LPMAPIUID lpmuid, LPRULE FAR * lpprl)
{
    SCODE sc;
    HRESULT hr;
    LPMAPISESSION lpsess = lpsmh->lpsess;
    LPPROFSECT lpprof = NULL;
    LPRULE lprl = NULL;
    LPSPropValue lpval = NULL;
    LPSPropValue lpvalEid = NULL;
    LPSPropValue lpvalT;
    LPSRestriction lpres = NULL;
    ULONG cval;
    ULONG ulType;
    UINT cb;

    sc = (*lpsmh->lpfnAlloc) (sizeof(RULE), &lprl);
    if (FAILED (sc))
    {
        hr = ResultFromScode (sc);
        goto ret;
    }
    memset (lprl, 0, sizeof(RULE));
    memcpy (&lprl->muid, lpmuid, sizeof(MAPIUID));
    hr = lpsess->lpVtbl->OpenProfileSection (lpsess,
                            lpmuid,
                            NULL,
                            MAPI_MODIFY,
                            &lpprof);
    if (HR_FAILED (hr))
        goto ret;

    hr = lpprof->lpVtbl->GetProps (lpprof,
                            (LPSPropTagArray)&sptRule,
                            0,
                            &cval,
                            &lpval);
    if (HR_FAILED (hr))
        goto ret;

    if ((lpval[ipType].ulPropTag != PR_RULE_TYPE) ||
        (lpval[ipData].ulPropTag != PR_RULE_DATA) ||
        (lpval[ipRLFlags].ulPropTag != PR_RULE_FLAGS) ||
        (!(lpval[ipRLFlags].Value.l & RULE_DELETE) &&
          (lpval[ipType].ulPropTag != PR_RULE_TYPE) ||
          (lpval[ipPath].ulPropTag != PR_RULE_TARGET_PATH) ||
          (lpval[ipStore].ulPropTag != PR_RULE_STORE_DISPLAY_NAME)))
    {
        /* Something very important is missing */

        hr = ResultFromScode (MAPI_E_UNCONFIGURED);
        goto ret;
    }
    lprl->rlTyp = (UINT)lpval[ipType].Value.l;
    lprl->ulFlags = lpval[ipRLFlags].Value.l;
    memcpy (lprl->lpszData, lpval[ipData].Value.bin.lpb, (UINT)lpval[ipData].Value.bin.cb);

    if (lpval[ipSound].ulPropTag == PR_RULE_SOUND_FILENAME)
        lstrcpy (lprl->lpszSound, lpval[ipSound].Value.lpszA);

    if (!(lpval[ipRLFlags].Value.l & RULE_DELETE))
    {
        hr = HrOpenMdbFromName (lpsmh, lpval[ipStore].Value.LPSZ, &lprl->lpmdb);
        if (HR_FAILED (hr))
            goto ret;

        if ((lpval[ipEID].ulPropTag != PR_RULE_TARGET_ENTRYID) ||
            (HR_FAILED (lpsess->lpVtbl->OpenEntry (lpsess,
                            lpval[ipEID].Value.bin.cb,
                            (LPENTRYID)lpval[ipEID].Value.bin.lpb,
                            NULL,
                            MAPI_MODIFY,
                            &ulType,
                            (LPUNKNOWN FAR *)&lprl->lpfldr))))
        {
            hr = HrFolderFromPath (lpsmh,
                            lprl->lpmdb,
                            lpval[ipPath].Value.LPSZ,
                            &lprl->lpfldr,
                            &lpvalEid);
            if (HR_FAILED (hr))
                goto ret;

            lpvalEid->ulPropTag = PR_RULE_TARGET_ENTRYID;
            HrSetOneProp ((LPMAPIPROP)lpprof, lpvalEid);
            lprl->lpvalEid = lpvalEid;
        }
        else
        {
            hr = HrGetOneProp ((LPMAPIPROP)lprl->lpfldr, PR_ENTRYID, &lprl->lpvalEid);
            if (HR_FAILED (hr))
                goto ret;
        }
    }

    if ((lpval[ipType].Value.l == RL_TO) ||
        (lpval[ipType].Value.l == RL_CC) ||
        (lpval[ipType].Value.l == RL_BCC) ||
        (lpval[ipType].Value.l == RL_RECIP))
    {
        cb = (sizeof(SRestriction) * cresMax) + (sizeof(SPropValue) * cvMax);
        sc = (*lpsmh->lpfnAllocMore) (cb, lprl, &lpres);
        if (FAILED (sc))
        {
            hr = ResultFromScode (sc);
            goto ret;
        }
        lpvalT = (LPSPropValue)&lpres[cresMax];

        lpres[iresAnd].rt = RES_AND;
        lpres[iresAnd].res.resAnd.cRes = 2;
        lpres[iresAnd].res.resAnd.lpRes = &lpres[iresRecip];

        lpvalT[ivRecip].ulPropTag = PR_RECIPIENT_TYPE;
        lpvalT[ivRecip].Value.l = ((lpval[ipType].Value.l == RL_TO)
                                    ? MAPI_TO
                                    : ((lpval[ipType].Value.l == RL_CC)
                                        ? MAPI_CC
                                        : ((lpval[ipType].Value.l == RL_BCC)
                                            ? MAPI_BCC
                                            : 0)));
        lpres[iresRecip].rt = RES_PROPERTY;
        lpres[iresRecip].res.resProperty.relop = RELOP_EQ;
        lpres[iresRecip].res.resContent.ulPropTag = PR_RECIPIENT_TYPE;
        lpres[iresRecip].res.resContent.lpProp = &lpvalT[ivRecip];

        lpres[iresOr].rt = RES_OR;
        lpres[iresOr].res.resOr.cRes = 2;
        lpres[iresOr].res.resOr.lpRes = &lpres[iresEmail];

        lpvalT[ivEmail].ulPropTag = PR_EMAIL_ADDRESS;
        lpvalT[ivEmail].Value.LPSZ = (LPTSTR)lprl->lpszData;
        lpres[iresEmail].rt = RES_CONTENT;
        lpres[iresEmail].res.resContent.ulFuzzyLevel = FL_SUBSTRING | FL_IGNORECASE;
        lpres[iresEmail].res.resContent.ulPropTag = PR_EMAIL_ADDRESS;
        lpres[iresEmail].res.resContent.lpProp = &lpvalT[ivEmail];

        lpvalT[ivDispNm].ulPropTag = PR_DISPLAY_NAME;
        lpvalT[ivDispNm].Value.LPSZ = (LPTSTR)lprl->lpszData;
        lpres[iresDispNm].rt = RES_CONTENT;
        lpres[iresDispNm].res.resContent.ulFuzzyLevel = FL_SUBSTRING | FL_IGNORECASE;
        lpres[iresDispNm].res.resContent.ulPropTag = PR_DISPLAY_NAME;
        lpres[iresDispNm].res.resContent.lpProp = &lpvalT[ivDispNm];

        if ((lpval[ipType].Value.l == RL_TO) ||
            (lpval[ipType].Value.l == RL_CC) ||
            (lpval[ipType].Value.l == RL_BCC))
            lprl->lpres = &lpres[iresAnd];
        else
            lprl->lpres = &lpres[iresOr];
    }

ret:

    (*lpsmh->lpfnFree) (lpval);

    UlRelease (lpprof);
    if (HR_FAILED (hr))
    {
        (*lpsmh->lpfnFree) (lprl->lpvalEid);
        (*lpsmh->lpfnFree) (lprl);
        lprl = NULL;
    }
    *lpprl = lprl;

    DebugTraceResult (HrBuildRule(), hr);
    return hr;
}


/*
 *  ReleaseBkit()
 *
 *  Purpose:
 *
 *      Cleans up all resources held by a bucket structure and wipes the
 *      structure clean.
 *
 *  Arguments:
 *
 *      lpsmh           pointer to the smh object (uses allocation fn's)
 *      lpbkit          pointer to the bucket needing cleaning
 *
 */
VOID
ReleaseBkit (LPSMH lpsmh, LPBKIT lpbkit)
{
    UlRelease (lpbkit->lpfldr);
    UlRelease (lpbkit->lpfldrYr);
    UlRelease (lpbkit->lpfldrParent);
    (*lpsmh->lpfnFree) (lpbkit->lpeid);
    (*lpsmh->lpfnFree) (lpbkit->lpeidYr);
    (*lpsmh->lpfnFree) (lpbkit->lpeidParent);
    memset (lpbkit, 0, sizeof(BKIT));
    return;
}


/*
 *  CleanupSMH()
 *
 *  Purpose:
 *
 *      Cleans up all resources and objects held by the SMH object.
 *
 *  Arguments:
 *
 *      lpsmh           the SMH parent object
 */
VOID
CleanupSMH (LPSMH lpsmh)
{
    LPRULE lprl;
    LPRULE lprlT;
    LPWB lpwb;
    LPWB lpwbT;

    ReleaseBkit (lpsmh, &lpsmh->bkitSm);
    memset (&lpsmh->bkitSm, 0, sizeof(BKIT));

    for (lpwb = lpsmh->lstWb; lpwb; lpwb = lpwbT)
    {
        lpwbT = lpwb->wbNext;
        lpwb->lptbl->lpVtbl->Unadvise (lpwb->lptbl, lpwb->ulAdvz);
        ReleaseBkit (lpsmh, &lpwb->bkit);
        UlRelease (lpwb->lptbl);
        UlRelease (lpwb->lpfldr);
        (*lpsmh->lpfnFree) (lpwb->lpvalEid);
        (*lpsmh->lpfnFree) (lpwb);
    }
    lpsmh->lstWb = NULL;
    for (lprl = lpsmh->lstRl; lprl; lprl = lprlT)
    {
        lprlT = lprl->rlNext;
        ReleaseBkit (lpsmh, &lprl->bkit);
        UlRelease (lprl->lpfldr);
        (*lpsmh->lpfnFree) (lprl->lpvalEid);
        (*lpsmh->lpfnFree) (lprl);

    }
    lpsmh->lstRl = NULL;
    ReleaseStoresTable (lpsmh);
}


/*
 *  HrInitSMH()
 *
 *  Purpose:
 *
 *      Initializes the SMH stucture, loads all filters, loads all
 *      exclusions, creates any unread search folders, and inits all
 *      wastebasket objects -- all based on the profile section.
 *
 *  Arguments:
 *
 *      lpsmh           the SMH parent object
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrInitSMH (LPSMH lpsmh)
{
    HRESULT hr;
    LPMAPISESSION lpsess = lpsmh->lpsess;
    LPMAPIUID lpmuid = &lpsmh->muid;
    LPPROFSECT lpprof = NULL;
    LPSPropValue lpval = NULL;
    ULONG cval;

    /* Get options from the profile */

    hr = lpsess->lpVtbl->OpenProfileSection (lpsess,
                            lpmuid,
                            NULL,
                            MAPI_MODIFY,
                            &lpprof);
    if (HR_FAILED (hr))
        goto ret;

    hr = lpprof->lpVtbl->GetProps (lpprof,
                            (LPSPropTagArray)&sptConfigProps,
                            0,
                            &cval,
                            &lpval);
    UlRelease (lpprof);
    if (HR_FAILED (hr))
        goto ret;

    /* Check to see if we are configured */

    if (REQ_PROPS != (UlChkReqProps (cval, lpval) & REQ_PROPS))
    {
        hr = ResultFromScode (MAPI_E_UNCONFIGURED);
        goto ret;
    }

    /* Grab the exclusions list */

    hr = ResultFromScode (PropCopyMore (&lpsmh->valEx,
                            &lpval[ipExc],
                            lpsmh->lpfnAllocMore,
                            lpsmh));
    if (HR_FAILED (hr))
        goto ret;

    /* Init the stores table */

    hr = HrInitStoresTable (lpsmh, lpsess);
    if (HR_FAILED (hr))
        goto ret;

    /* Store the values */

    lpsmh->fCatSm = !!(lpval[ipSMHFlags].Value.l & SMH_FILTER_SENTMAIL);
    lpsmh->fCatSmByYr = !!(lpval[ipSMHFlags].Value.l & SMH_FILTER_SENTMAIL_YR);
    if (lpval[ipSMHFlags].Value.l & SMH_FILTER_DELETED)
    {
        HrInitDeletedMailFilter (lpsmh);
        lpsmh->fCatWb = !!(lpval[ipSMHFlags].Value.l & SMH_FILTER_DELETED_YR);
    }
    if (!!(lpval[ipSMHFlags].Value.l & SMH_UNREAD_VIEWER))
        HrInitUnreadSearch (lpsmh);

    if ((lpval[ipSMHFlags].Value.l & SMH_FILTER_INBOUND) &&
        (lpval[ipRules].ulPropTag == PR_SMH_RULES) &&
        (lpval[ipRules].Value.bin.cb != 0))
    {
        UINT crl = (UINT)(lpval[ipRules].Value.bin.cb / sizeof(MAPIUID));
        LPMAPIUID lpmuid = ((LPMAPIUID)lpval[ipRules].Value.bin.lpb) + crl;
        LPRULE lprl;

        while (crl--)
        {
            --lpmuid;
            hr = HrBuildRule (lpsmh, lpmuid, &lprl);
            if (!HR_FAILED (hr))
            {
                lprl->rlNext = lpsmh->lstRl;
                lpsmh->lstRl = lprl;
            }
        }
        hr = hrSuccess;
    }

ret:

    (*lpsmh->lpfnFree) (lpval);
    DebugTraceResult (HrInitSMH(), hr);
    return hr;
}


/*
 *  HrReConfigSMH()
 *
 *  Purpose:
 *
 *      Cleans up the current SMH configuration and reloads it from
 *      scratch.
 *
 *  Arguments:
 *
 *      lpsmh           the SMH parent object
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrReConfigSMH (LPSMH lpsmh)
{
    HRESULT hr;

    CleanupSMH (lpsmh);
    hr = HrInitSMH (lpsmh);
    if (!HR_FAILED (hr))
        ResetConfigEvent (lpsmh->hevtConfig);

    DebugTraceResult (HrReconfigSMH(), hr);
    return hr;
}


/*
 *  SMH Object Methods
 *
 *  SMH_QueryInterface      (See OLE IUnknown object methods)
 *  SMH_AddRef              (See OLE IUnknown object methods)
 *  SMH_Release             (See OLE IUnknown object methods)
 *  SMH_InboundMsgHook      Filters inbound messages
 *  SMH_OutboundMsgHook     Filters sent mail messages
 *
 */
STDMETHODIMP
SMH_QueryInterface (LPSMH lpsmh, REFIID lpiid, LPVOID FAR * lppv)
{
    if (IsBadWritePtr (lpsmh, sizeof(SMH)) ||
        IsBadReadPtr (lpiid, sizeof(IID)) ||
        IsBadWritePtr (lppv, sizeof(LPVOID)))
        return ResultFromScode (MAPI_E_INVALID_PARAMETER);

    if (!memcmp (lpiid, &IID_ISpoolerHook, sizeof(IID)) ||
        !memcmp (lpiid, &IID_IUnknown, sizeof(IID)))
    {
        *lppv = (LPVOID)lpsmh;
        lpsmh->lcInit++;
        return hrSuccess;
    }
    DebugTraceSc (SMH_QueryInterface(), MAPI_E_INTERFACE_NOT_SUPPORTED);
    return ResultFromScode (MAPI_E_INTERFACE_NOT_SUPPORTED);
}

STDMETHODIMP_ (ULONG)
SMH_AddRef (LPSMH lpsmh)
{
    if (IsBadWritePtr (lpsmh, sizeof(SMH)))
        return 0;

    return ++lpsmh->lcInit;
}

STDMETHODIMP_ (ULONG)
SMH_Release (LPSMH lpsmh)
{
    if (IsBadWritePtr (lpsmh, sizeof(SMH)))
        return 0;

    if (--lpsmh->lcInit)
        return lpsmh->lcInit;

    CloseConfigEvent (lpsmh->hevtConfig);
    CleanupSMH (lpsmh);
    UlRelease (lpsmh->lpsess);
    (*lpsmh->lpfnFree) (lpsmh);
    return 0;
}

/*
 *  SMH_InboundMsgHook()
 *
 *  Purpose:
 *
 *      The purpose of this filter is to match inbound messages to
 *      individual rules from the profile and re-route the messages based
 *      on the results of the comparisons.
 *
 *  Arguments:
 *
 *      lpsmh           this filter hook obj
 *      lpmsg           the message to be filtered
 *      lpfldrDef       owning folder of message
 *      lpmdbDef        owning store of message
 *      lpulFlags       flags returned by filter
 *      lpcbeid         cb for entryid of default target for message
 *      lppbeid         pb for entryid of default target for message
 *
 *  Operation:
 *
 *      Opens the suggested folder (if needed) and checks for the
 *      existence of the appropriate "bucket" folder.  If it does exist,
 *      then the  folder is created and cached.  The entryid is grabbed
 *      and passed back in to the spooler.
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpulFlags   [out]   set HOOK_CANCEL if this is the last hook
 *      lpcbeid     [out]   the size of the returned EntryID
 *      lppbeid     [out]   the data of the returned EntryID
 *
 */
STDMETHODIMP
SMH_InboundMsgHook (LPSMH lpsmh,
    LPMESSAGE lpmsg,
    LPMAPIFOLDER lpfldrDef,
    LPMDB lpmdbDef,
    ULONG FAR * lpulFlags,
    ULONG FAR * lpcbeid,
    LPBYTE FAR * lppeid)
{
    HRESULT hr = hrSuccess;
    LPRULE lprl;
    LPBYTE lpeid;
    LPSPropValue lpval = NULL;

    /* Quick and dirty parameter check */

    if (IsBadWritePtr (lpsmh, sizeof(SMH)) ||
        IsBadWritePtr (lpcbeid, sizeof(ULONG)) ||
        IsBadWritePtr (lppeid, sizeof(LPBYTE)) ||
        IsBadWritePtr (*lppeid, (UINT)(*lpcbeid)))
        return ResultFromScode (MAPI_E_INVALID_PARAMETER);

    if (FConfigChanged (lpsmh->hevtConfig) &&
        HR_FAILED (hr = HrReConfigSMH (lpsmh)))
    {
        DebugTraceResult (SMH_InboundMsgHook(), hr);
        return hrSuccess;
    }

    if (lprl = lpsmh->lstRl)    /* Yup '=' */
    {
        hr = HrCheckExclusions (lpsmh, lpmsg);
        if (HR_FAILED (hr))
        {
            /* We have not been excluded */

            for (; lprl; lprl = lprl->rlNext)
            {
                hr = HrCheckRule (lpsmh, lprl, lpmsg, &lpval);
                if (!HR_FAILED (hr))
                {
                    /* We have a match. What do we do, filter or delete? */
                    
                    if (!(lprl->ulFlags & RULE_DELETE))
                    {
                        /* Filter the critter */

                        hr = ResultFromScode ((*lpsmh->lpfnAlloc) (lprl->lpvalEid->Value.bin.cb, &lpeid));
                        if (!HR_FAILED (hr))
                        {
                            memcpy (lpeid, lprl->lpvalEid->Value.bin.lpb,
                                (UINT)lprl->lpvalEid->Value.bin.cb);

                            (*lpsmh->lpfnFree) (*lppeid);
                            *lpcbeid = lprl->lpvalEid->Value.bin.cb;
                            *lppeid = lpeid;

                            if (lprl->ulFlags & RULE_ARCHIVED)
                            {
                                hr = HrArchiveMessage (lpsmh,
                                            lpmsg,
                                            lpfldrDef,
                                            lpmdbDef,
                                            &lprl->bkit,
                                            !!(lprl->ulFlags & RULE_ARCHIVED_BY_YEAR),
                                            lpcbeid,
                                            lppeid);
                                
                                if (lprl->ulFlags & RULE_TERMINAL)
                                    *lpulFlags = HOOK_CANCEL;
                            }
                        }

                        if (lstrlen (lprl->lpszSound))
                            sndPlaySound (lprl->lpszSound, SND_ASYNC);
                    }
                    else
                        *lpulFlags = HOOK_DELETE | HOOK_CANCEL;

                    break;
                }
                else if (GetScode (hr) != MAPI_E_NOT_ME)
                {
                    /*  We have a failure that is not really
                     *  expected, we need to bail.  Also, this
                     *  should cancel any further hooking
                     */
                    *lpulFlags = HOOK_CANCEL;
                    break;
                }
                else
                    hr = hrSuccess;
            }
        }
    }

    (*lpsmh->lpfnFree) (lpval);
    DebugTraceResult (SMH_InboundMsgHook(), hr);
    return hrSuccess;
}


/*
 *  SMH_OutboundMsgHook()
 *
 *  Purpose:
 *
 *      The purpose of this filter is to "hash" a users sent mail
 *      processing based on date.  The most obvious bucket size is
 *      monthly but there is no reason not to make this an option the
 *      user could confiigure.
 *
 *  Arguments:
 *
 *      lpsmh           this filter hook obj
 *      lpmsg           the message to be filtered
 *      lpfldrDef       owning folder of message
 *      lpmdbDef        owning store of message
 *      lpulFlags       flags returned by filter
 *      lpcbeid         cb for entryid of default target for message
 *      lppbeid         pb for entryid of default target for message
 *
 *  Operation:
 *
 *      Opens the suggested folder (if needed) and checks for the
 *      existence of the appropriate "bucket" folder.  If it does exist,
 *      then the  folder is created and cached.  The entryid is grabbed
 *      and passed back in to the spooler.
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpulFlags   [out]   set HOOK_CANCEL if this is the last hook
 *      lpcbeid     [out]   the size of the returned EntryID
 *      lppbeid     [out]   the data of the returned EntryID
 *
 */
STDMETHODIMP
SMH_OutboundMsgHook (LPSMH lpsmh,
    LPMESSAGE lpmsg,
    LPMAPIFOLDER lpfldrDef,
    LPMDB lpmdbDef,
    ULONG FAR * lpulFlags,
    ULONG FAR * lpcbeid,
    LPBYTE FAR * lppeid)
{
    HRESULT hr = hrSuccess;

    /* Quick and dirty parameter check */

    if (IsBadWritePtr (lpsmh, sizeof(SMH)) ||
        IsBadWritePtr (lpcbeid, sizeof(ULONG)) ||
        IsBadWritePtr (lppeid, sizeof(LPBYTE)) ||
        IsBadWritePtr (*lppeid, (UINT)(*lpcbeid)))
        return ResultFromScode (MAPI_E_INVALID_PARAMETER);

    if (FConfigChanged (lpsmh->hevtConfig) &&
        HR_FAILED (hr = HrReConfigSMH (lpsmh)))
    {
        DebugTraceResult (SMH_InboundMsgHook(), hr);
        return hrSuccess;
    }

    if (lpsmh->fCatSm)
        hr = HrArchiveMessage (lpsmh,
                lpmsg,
                lpfldrDef,
                lpmdbDef,
                &lpsmh->bkitSm,
                lpsmh->fCatSmByYr,
                lpcbeid,
                lppeid);

    DebugTraceResult (SMH_OutboundMsgHook(), hr);
    return hrSuccess;
}


/*
 *  SMH_Init()
 *
 *  Purpose:
 *
 *      Spooler's entry into the sample mail handler.  This function is
 *      equivilent to a provider logon in that it returns an object to
 *      the spooler that will be used to make any additional calls into
 *      the handler.
 *
 *  Arguments:
 *
 *      lpsess          the session this handler relates to
 *      hinst           hinst of the SMH dll
 *      lpfnAlloc       pointer to MAPIAllocateBuffer()
 *      lpfnAllocMore   pointer to MAPIAllocateMore()
 *      lpfnFree        pointer to MAPIFreeBuffer()
 *      lpmuid          pointer to profile section muid
 *      ulFlags         flags
 *      lppHook         buffer to hold handler object
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lpphook     [OUT]   holds the returned handler object iff successful
 */
STDINITMETHODIMP
SMH_Init (LPMAPISESSION lpsess,
    HINSTANCE hinst,
    LPALLOCATEBUFFER lpfnAlloc,
    LPALLOCATEMORE lpfnAllocMore,
    LPFREEBUFFER lpfnFree,
    LPMAPIUID lpmuid,
    ULONG ulFlags,
    LPSPOOLERHOOK FAR * lppHook)
{
    SCODE sc;
    HRESULT hr;
    LPSMH lpsmh = NULL;

    sc = (*lpfnAlloc) (sizeof(SMH), &lpsmh);
    if (FAILED (sc))
        return ResultFromScode (sc);
    memset (lpsmh, 0, sizeof(SMH));

    hr = lpsess->lpVtbl->QueryInterface (lpsess,
                            &IID_IMAPISession,
                            &lpsmh->lpsess);
    if (HR_FAILED (hr))
    {
        (*lpfnFree) (lpsmh);
        lpsmh = NULL;
        goto ret;
    }

    hr = HrGetConfigEvent (&lpsmh->hevtConfig);
    if (HR_FAILED (hr))
    {
        (*lpfnFree) (lpsmh);
        lpsmh = NULL;
        goto ret;
    }

    /* Fill in all fields of the object */

    lpsmh->lpVtbl = (SMH_Vtbl FAR *)&vtblSMH;
    lpsmh->lcInit = 1;
    lpsmh->hinst = hinst;
    lpsmh->lpsess = lpsess;
    lpsmh->lpfnAlloc = lpfnAlloc;
    lpsmh->lpfnAllocMore = lpfnAllocMore;
    lpsmh->lpfnFree = lpfnFree;
    memcpy (&lpsmh->muid, lpmuid, sizeof(MAPIUID));

    hr = HrInitSMH (lpsmh);

ret:

    if (HR_FAILED (hr))
    {
        UlRelease (lpsmh);
        lpsmh = NULL;
    }
    *lppHook = (LPSPOOLERHOOK)lpsmh;
    DebugTraceResult (SMH_Init(), hr);
    return hr;
}
