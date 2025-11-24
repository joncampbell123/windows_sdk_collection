/*
 *  S M H P S . C
 *
 *  Sample mail handling hook configuration property sheets
 *  Copyright 1992-95 Microsoft Corporation.  All Rights Reserved.
 */

#include "_pch.h"

#ifndef WIN16
#include <commctrl.h>
#endif

extern LPTSTR lpszConfigEvt;
extern SPropTagArray sptRule;
extern SPropTagArray sptConfigProps;


/*
 *  sptDelEid
 *
 *  This is the set of properties that need to be deleted from a rule
 *  profile section any time the rule is edited.  Otherwise, changes in
 *  target folders may not be retained across edits.
 */
const static SizedSPropTagArray (2, sptDelEid) =
{
    2,
    {
        PR_RULE_TARGET_ENTRYID,
        PR_RULE_STORE_ENTRYID
    }
};


/*
 *  SizeODButtons()
 *
 *  Purpose:
 *
 *      Set the control size for the two owner-draw buttons in the filter
 *      page of the configuration property sheets.
 *
 *  Arguments:
 *
 *      hinst       the DLL instance
 *      hdlg        the dialog in which the buttons will be drawn
 */
VOID
SizeODButtons (HINSTANCE hInst, HWND hdlg)
{
    BITMAP  bitmap;
    HBITMAP hbmp;

    if (!(hbmp = LoadBitmap (hInst, MAKEINTRESOURCE(ID_UpArrow))))
        return;
    GetObject (hbmp, sizeof(BITMAP), &bitmap);
    SetWindowPos (GetDlgItem (hdlg, ID_FilterUp), NULL, 0, 0, bitmap.bmWidth,
        bitmap.bmHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
    SetWindowPos (GetDlgItem (hdlg, ID_FilterDown), NULL, 0, 0, bitmap.bmWidth,
        bitmap.bmHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

    DeleteBitmap (hbmp);
}


/*
 *  DrawODButton()
 *
 *  Purpose:
 *
 *      Draws the button control for either of the two owner-draw buttons
 *      in the filter page of the configuration property sheets.
 *
 *  Arguments:
 *
 *      hinst       the DLL instance
 *      pdi         the DRAWITEMSTRUCT info for drawing the button
 */
VOID
DrawODButton (HINSTANCE hInst, DRAWITEMSTRUCT FAR * lpdi)
{
    HDC hDC;
    HBITMAP hbmpOld;
    HBITMAP hbmpArw;
    WORD wBtnRes;
    BITMAP bitmap;

    Assert (lpdi->CtlType == ODT_BUTTON);
    if (!(hDC = CreateCompatibleDC (lpdi->hDC)))
        return;

    /* Get the bitmap */

    if (lpdi->itemState & ODS_SELECTED)
        wBtnRes = (lpdi->CtlID == ID_FilterUp) ? ID_UpArrowInv : ID_DownArrowInv;
    else if (lpdi->itemState & ODS_DISABLED)
        wBtnRes = (lpdi->CtlID == ID_FilterUp) ? ID_UpArrowDis : ID_DownArrowDis;
    else
        wBtnRes = (lpdi->CtlID == ID_FilterUp) ? ID_UpArrow : ID_DownArrow;

    /* blit the bitmap */

    if (!(hbmpArw = CreateMappedBitmap (hInst, wBtnRes, FALSE, NULL, 0)))
        goto err;
    hbmpOld = SelectObject (hDC, hbmpArw);
    BitBlt (lpdi->hDC, 0, 0, lpdi->rcItem.right - lpdi->rcItem.left,
        lpdi->rcItem.bottom - lpdi->rcItem.top, hDC, 0, 0, SRCCOPY);

    /* Draw a focus rectangle if the button has the focus */

    if(lpdi->itemState & ODS_FOCUS)
    {
        GetObject (hbmpArw, sizeof(BITMAP), &bitmap);
        lpdi->rcItem.right = bitmap.bmWidth;
        lpdi->rcItem.bottom = bitmap.bmHeight;
        InflateRect (&lpdi->rcItem, -3, -3);
        if (lpdi->itemState & ODS_SELECTED)
            OffsetRect (&lpdi->rcItem, 1, 1);
        DrawFocusRect (lpdi->hDC, &lpdi->rcItem);
    }

    SelectObject (hDC, hbmpOld);
    DeleteBitmap (hbmpArw);
err:
    DeleteDC (hDC);
}


/*
 *  HrDeleteOneProp()
 *
 *  Purpose:
 *
 *      Small wrapper that allows the deletion of a single property.
 *
 *  Arguments:
 *
 *      lpmp        the IMAPIPROP object
 *      ulPropTag   the property to delete
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrDeleteOneProp (LPMAPIPROP lpmp, ULONG ulPropTag)
{
    SizedSPropTagArray (1,spt) = {1,{ulPropTag}};
    return lpmp->lpVtbl->DeleteProps (lpmp, (LPSPropTagArray)&spt, NULL);
}


/*
 *  FilterDesc_INITDIALOG()
 *
 *  Purpose:
 *
 *      Handles the WM_INITDIALOG message for the filter description
 *      dialog.
 *
 *      The values for the current filter are passed in via the
 *      short-term property value pointer in the SMHDLG struct.  The
 *      values are then scanned and processed to fill out the state of
 *      the dialog.
 */
BOOL
FilterDesc_INITDIALOG (HWND hdlg, HWND hwndFocus, LPARAM lParam)
{
    BOOL fDisableOk = FALSE;
    LPSPropValue lpval;

    MakeDlg3D (hdlg);
    Edit_LimitText (GetDlgItem (hdlg, ID_Name), cchRuleMax);
    SetWindowLong (hdlg, DWL_USER, lParam);
    if (lpval = ((LPSMHDLG)lParam)->lpval)
    {
        Assert (!IsBadReadPtr (lpval, sizeof(SPropValue) * cpRLMax));
        if (lpval[ipDisp].ulPropTag == PR_DISPLAY_NAME)
            Edit_SetText (GetDlgItem (hdlg, ID_Name), lpval[ipDisp].Value.LPSZ);

        if (lpval[ipType].ulPropTag == PR_RULE_TYPE)
        {
            switch (lpval[ipType].Value.l)
            {
              case RL_RECIP:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_AnyRecip);
                break;
              case RL_TO:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_ToRecip);
                break;
              case RL_CC:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_CcRecip);
                break;
              case RL_BCC:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_BccRecip);
                break;
              case RL_SUBJECT:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_Subject);
                break;
              case RL_FROM:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_Sender);
                break;
              case RL_ATTACH:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_HasAttach);
                break;
              case RL_BODY:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_Body);
                break;
              case RL_CLASS:
                CheckRadioButton (hdlg, ID_Subject, ID_HasAttach, ID_MsgClass);
                break;
              default:
                fDisableOk = TRUE;
                break;
            }
        }
        else
            fDisableOk = TRUE;

        if (lpval[ipData].ulPropTag == PR_RULE_DATA)
            Edit_SetText (GetDlgItem (hdlg, ID_Value), lpval[ipData].Value.bin.lpb);
        else
            fDisableOk = TRUE;

        /* An empty or missing value here implies uses the default store */

        if (lpval[ipStore].ulPropTag == PR_RULE_STORE_DISPLAY_NAME)
            Edit_SetText (GetDlgItem (hdlg, ID_Store), lpval[ipStore].Value.LPSZ);

        if (lpval[ipPath].ulPropTag == PR_RULE_TARGET_PATH)
            Edit_SetText (GetDlgItem (hdlg, ID_Folder), lpval[ipPath].Value.LPSZ);
        else
            fDisableOk = TRUE;

        if (lpval[ipSound].ulPropTag == PR_RULE_SOUND_FILENAME)
            Edit_SetText (GetDlgItem (hdlg, ID_Sound), lpval[ipSound].Value.LPSZ);
        
        if (lpval[ipRLFlags].ulPropTag == PR_RULE_FLAGS)
        {
            CheckDlgButton (hdlg, ID_NotMatch,
                    !!(lpval[ipRLFlags].Value.l & RULE_NOT));
            CheckDlgButton (hdlg, ID_TerminalTarg,
                    !!(lpval[ipRLFlags].Value.l & RULE_TERMINAL));
            CheckDlgButton (hdlg, ID_ArchTarg,
                    !!(lpval[ipRLFlags].Value.l & RULE_ARCHIVED));
            CheckDlgButton (hdlg, ID_ArchTargYr,
                    !!(lpval[ipRLFlags].Value.l & RULE_ARCHIVED_BY_YEAR));
            EnableWindow (GetDlgItem (hdlg, ID_ArchTargYr),
                    !!(lpval[ipRLFlags].Value.l & RULE_ARCHIVED));

            if (lpval[ipRLFlags].Value.l & RULE_DELETE)
            {
                CheckRadioButton (hdlg, ID_FilterMsg, ID_DeleteMsg, ID_DeleteMsg);
                EnableWindow (GetDlgItem (hdlg, ID_FolderTxt), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_Folder), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_StoreTxt), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_Store), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_SoundTxt), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_Sound), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_TerminalTarg), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_ArchTarg), FALSE);
                EnableWindow (GetDlgItem (hdlg, ID_ArchTargYr), FALSE);
            }
            else
                CheckRadioButton (hdlg, ID_FilterMsg, ID_DeleteMsg, ID_FilterMsg);
        }
        else
        {
            Button_Enable (GetDlgItem (hdlg, ID_ArchTargYr), FALSE);
            CheckRadioButton (hdlg, ID_FilterMsg, ID_DeleteMsg, ID_FilterMsg);
        }

        Button_Enable (GetDlgItem (hdlg, IDOK), !fDisableOk);
    }
    return TRUE;
}


/*
 *  FilterDesc_COMMAND()
 *
 *  Purpose:
 *
 *      Handles the WM_COMMAND message for the filter description dialog
 *
 *      On IDOK, the values from the dialog are processed into the rule
 *      properties and placed in the property value array hanging off of
 *      the SMHDLG structure.
 */
BOOL FilterDesc_COMMAND (HWND hdlg, UINT id, HWND hwndCtl, UINT codeNotify)
{
    SCODE sc;
    BOOL fCheck;
    HWND hctrl;
    LPSMHDLG lpsmhdlg;
    LPSPropValue lpval;
    LPTSTR lpsz = NULL;
    UINT cb;

    lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
    lpval = lpsmhdlg->lpval;

    switch (id)
    {
      case IDOK:

        /* Get the rule name */

        hctrl = GetDlgItem (hdlg, ID_Name);
        cb = Edit_GetTextLength (hctrl) + 1;
        sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &(lpval[ipDisp].Value.lpszA));
        if (FAILED (sc))
            goto err;
        memset (lpval[ipDisp].Value.LPSZ, 0, cb);
        lpval[ipDisp].ulPropTag = PR_DISPLAY_NAME;
        Edit_GetText (hctrl, lpval[ipDisp].Value.LPSZ, cb);

        /* Rule type */

        lpval[ipType].ulPropTag = PR_RULE_TYPE;

        /* Rule data */

        hctrl = GetDlgItem (hdlg, ID_Value);
        cb = Edit_GetTextLength (hctrl) + 1;
        sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &lpval[ipData].Value.bin.lpb);
        if (FAILED (sc))
            goto err;
        memset (lpval[ipData].Value.bin.lpb, 0, cb);
        lpval[ipData].ulPropTag = PR_RULE_DATA;
        Edit_GetText (hctrl, lpval[ipData].Value.bin.lpb, cb);
        lpval[ipData].Value.bin.cb = (ULONG)cb;

        /* Target store EID */

        lpval[ipSEID].ulPropTag = PR_NULL;

        /* Target store */

        hctrl = GetDlgItem (hdlg, ID_Store);
        cb = Edit_GetTextLength (hctrl) + 1;
        sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &(lpval[ipStore].Value.lpszA));
        if (FAILED (sc))
            goto err;
        memset (lpval[ipStore].Value.LPSZ, 0, cb);
        lpval[ipStore].ulPropTag = PR_RULE_STORE_DISPLAY_NAME;
        Edit_GetText (hctrl, lpval[ipStore].Value.LPSZ, cb);

        /* Target folder EID */

        lpval[ipEID].ulPropTag = PR_NULL;

        /* Target folder */

        hctrl = GetDlgItem (hdlg, ID_Folder);
        cb = Edit_GetTextLength (hctrl) + 1;
        sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &(lpval[ipPath].Value.lpszA));
        if (FAILED (sc))
            goto err;
        memset (lpval[ipPath].Value.LPSZ, 0, cb);
        lpval[ipPath].ulPropTag = PR_RULE_TARGET_PATH;
        Edit_GetText (hctrl, lpval[ipPath].Value.LPSZ, cb);

        /* Sound */

        hctrl = GetDlgItem (hdlg, ID_Sound);
        cb = Edit_GetTextLength (hctrl) + 1;
        sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &(lpval[ipSound].Value.lpszA));
        if (FAILED (sc))
            goto err;
        memset (lpval[ipSound].Value.LPSZ, 0, cb);
        lpval[ipSound].ulPropTag = PR_RULE_SOUND_FILENAME;
        Edit_GetText (hctrl, lpval[ipSound].Value.LPSZ, cb);

        /* Flags */

        lpval[ipRLFlags].ulPropTag = PR_RULE_FLAGS;
        lpval[ipRLFlags].Value.l = 0;
        if (IsDlgButtonChecked (hdlg, ID_ArchTarg))
        {
            lpval[ipRLFlags].Value.l |= RULE_ARCHIVED;

            if (IsDlgButtonChecked (hdlg, ID_ArchTargYr))
                lpval[ipRLFlags].Value.l |= RULE_ARCHIVED_BY_YEAR;
        }
        if (IsDlgButtonChecked (hdlg, ID_NotMatch))
            lpval[ipRLFlags].Value.l |= RULE_NOT;
        if (IsDlgButtonChecked (hdlg, ID_TerminalTarg))
            lpval[ipRLFlags].Value.l |= RULE_TERMINAL;
        if (IsDlgButtonChecked (hdlg, ID_DeleteMsg))
            lpval[ipRLFlags].Value.l |= RULE_DELETE;

err:
        lpsmhdlg->sc = sc;
        break;

      case IDCANCEL:
        break;

      case ID_AnyRecip:
        lpval[ipType].Value.l = RL_RECIP;
        return TRUE;

      case ID_ToRecip:
        lpval[ipType].Value.l = RL_TO;
        return TRUE;

      case ID_CcRecip:
        lpval[ipType].Value.l = RL_CC;
        return TRUE;

      case ID_BccRecip:
        lpval[ipType].Value.l = RL_BCC;
        return TRUE;

      case ID_Subject:
        lpval[ipType].Value.l = RL_SUBJECT;
        return TRUE;

      case ID_Sender:
        lpval[ipType].Value.l = RL_FROM;
        return TRUE;

      case ID_HasAttach:
        lpval[ipType].Value.l = RL_ATTACH;
        return TRUE;

      case ID_Body:
        lpval[ipType].Value.l = RL_BODY;
        return TRUE;

      case ID_MsgClass:
        lpval[ipType].Value.l = RL_CLASS;
        return TRUE;

      case ID_FilterMsg:
      case ID_DeleteMsg:
        fCheck = IsDlgButtonChecked (hdlg, ID_FilterMsg);
        EnableWindow (GetDlgItem (hdlg, ID_FolderTxt), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_Folder), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_StoreTxt), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_Store), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_SoundTxt), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_Sound), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_TerminalTarg), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_ArchTarg), fCheck);
        EnableWindow (GetDlgItem (hdlg, ID_ArchTargYr), fCheck && IsDlgButtonChecked (hdlg, ID_ArchTarg));
        return TRUE;

      case ID_ArchTarg:
        EnableWindow (GetDlgItem (hdlg, ID_ArchTargYr), IsDlgButtonChecked (hdlg, ID_ArchTarg));
        return TRUE;

      default:
        return FALSE;
    }

    EndDialog (hdlg, id);
    return TRUE;
}


/*
 *  FilterDescProc()
 *
 *  Purpose:
 *
 *      Dispatches window messages to the proper function for processing
 */
BOOL CALLBACK
FilterDescProc (HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
      case WM_INITDIALOG:

        FHandleWm (FilterDesc, hdlg, INITDIALOG, wParam, lParam);
        return TRUE;

      case WM_COMMAND:

        FHandleWm (FilterDesc, hdlg, COMMAND, wParam, lParam);
        Button_Enable (GetDlgItem(hdlg, IDOK),
                Edit_GetTextLength (GetDlgItem (hdlg, ID_Name)) &&
                Edit_GetTextLength (GetDlgItem (hdlg, ID_Value)) &&
                ((Edit_GetTextLength (GetDlgItem (hdlg, ID_Folder)) &&
                  Edit_GetTextLength (GetDlgItem (hdlg, ID_Store))) ||
                 IsDlgButtonChecked (hdlg, ID_DeleteMsg)) &&
                ((LPSMHDLG)GetWindowLong (hdlg, DWL_USER))->lpval[ipType].Value.l);
        break;

      case WM_DRAWITEM:

        break;

      case WM_DESTROY:

        break;
    }
    return FALSE;
}


/*
 *  ScEditFilterDesc()
 *
 *  Purpose:
 *
 *      Brings up the filter description dialog for the rule properties
 *      found in the profile section muid passed in.
 *
 *  Arguments:
 *
 *      lpsmhdlg        the SMH dialog structure
 *      hdlg            the parent dialog for the filter description
 *      lpmuid          the profile secion UID
 *      lppval  [OUT]   contains a pointer to the filter properties
 *                      iff the call is succeeds
 *
 *  Returns:
 *
 *      (HRESULT)
 *      lppval  [OUT]   this can be used to determine the name of the
 *                      edited rule
 */
SCODE
ScEditFilterDesc (LPSMHDLG lpsmhdlg,
    HWND hdlg,
    UINT ulFlags,
    LPMAPIUID lpmuid,
    LPSPropValue FAR * lppval)
{
    HRESULT hr;
    LPPROFSECT lpsec = NULL;
    LPSPropValue lpval = NULL;
    ULONG cval;
    UINT id;

    if (ulFlags == NEW_RULE)
    {
        hr = lpsmhdlg->lpsup->lpVtbl->NewUID (lpsmhdlg->lpsup, lpmuid);
        if (HR_FAILED (hr))
            goto ret;
    }

    hr = lpsmhdlg->lpadmin->lpVtbl->OpenProfileSection (lpsmhdlg->lpadmin,
                                        lpmuid,
                                        NULL,
                                        MAPI_MODIFY,
                                        &lpsec);
    if (HR_FAILED (hr))
        goto ret;

    hr = lpsec->lpVtbl->GetProps (lpsec,
                            (LPSPropTagArray)&sptRule,
                            0,
                            &cval,
                            &lpval);
    if (HR_FAILED (hr))
        goto ret;

    lpsmhdlg->lpval = lpval;
    id = DialogBoxParam (lpsmhdlg->hinst,
                    MAKEINTRESOURCE (SMH_FilterDesc),
                    GetParent(hdlg),
                    FilterDescProc,
                    (LPARAM)lpsmhdlg);
    switch (id)
    {
      case (UINT)-1:

        hr = ResultFromScode (MAPI_E_CALL_FAILED);
        break;

      case IDOK:

        /* Make changes into profile section */

        hr = lpsec->lpVtbl->SetProps (lpsec, cpRLMax, lpval, NULL);
        if (!HR_FAILED (hr))
        {
            /* Delete store/folder entryid in case the path changed */

            lpval[ipEID].ulPropTag = PR_NULL;
            lpval[ipSEID].ulPropTag = PR_NULL;
            (void)lpsec->lpVtbl->DeleteProps (lpsec, (LPSPropTagArray)&sptDelEid, NULL);
            hr = lpsec->lpVtbl->SaveChanges (lpsec, 0);
            if (!HR_FAILED (hr))
                *lppval = lpval;
        }
        break;

      case IDCANCEL:

        hr = ResultFromScode (MAPI_E_USER_CANCEL);
        break;
    }

ret:

    UlRelease (lpsec);
    MAPIFreeBuffer (lpsmhdlg->lpval);
    lpsmhdlg->lpval = NULL;

    DebugTraceResult (ScEditFilterDesc(), hr);
    return GetScode (hr);
}


/*
 *  EnableFilterPageCtrls()
 *
 *  Purpose:
 *
 *      Enables the set of buttons on the filter page based on the
 *      current selection in the filter list dialog
 */
VOID
EnableFilterPageCtrls (HWND hdlg)
{
    BOOL fDelete;
    BOOL fDown;
    BOOL fEdit;
    BOOL fUp;
    HWND hctrl = GetDlgItem (hdlg, ID_FilterOrderLB);
    UINT isel = ListBox_GetCurSel (hctrl);

    /* Enable Ctrls */

    fEdit = fDelete = fUp = fDown = ((isel == LB_ERR) ? FALSE : TRUE);
    if (isel == 0)
    {
        fUp = FALSE;
    }
    if (isel == (UINT)(ListBox_GetCount (hctrl) - 1))
    {
        fDown = FALSE;
    }

    Button_Enable (GetDlgItem (hdlg, ID_FilterUp), fUp);
    Button_Enable (GetDlgItem (hdlg, ID_FilterDown), fDown);
    Button_Enable (GetDlgItem (hdlg, ID_EditFilter), fEdit);
    Button_Enable (GetDlgItem (hdlg, ID_RmvFilter), fDelete);
}


/*
 *  FilterPage_INITDIALOG()
 *
 *  Purpose:
 *
 *      Handles the WM_INITDIALOG message for the filter description
 *      dialog.
 *
 *      The current list of filters are processed and the name is
 *      retrieved for use in the dialog.
 */
BOOL
FilterPage_INITDIALOG (HWND hdlg, HWND hwndFocus, LPARAM lParam)
{
    SCODE sc;
    HRESULT hr;
    HWND hctrl;
    LPMAPIUID lpmuid;
    LPPROFSECT lpsec;
    LPSMHDLG lpsmhdlg = (LPSMHDLG)(((PROPSHEETPAGE *)lParam)->lParam);
    LPSPropValue lpval;
    LPSPropValue lpvalSMH;
    UINT irl = 0;
    UINT crl;

    MakeDlg3D (hdlg);
    SizeODButtons (lpsmhdlg->hinst, hdlg);
    SetWindowLong (hdlg, DWL_USER, ((PROPSHEETPAGE *)lParam)->lParam);

    /* Load the filter LBX and create the mapping */

    if (lpvalSMH = lpsmhdlg->lpvalSMH)
    {
        if (lpvalSMH[ipRules].ulPropTag == PR_SMH_RULES)
        {
            hctrl = GetDlgItem (hdlg, ID_FilterOrderLB);
            lpmuid = (LPMAPIUID)lpvalSMH[ipRules].Value.bin.lpb;
            crl = (UINT)(lpvalSMH[ipRules].Value.bin.cb / sizeof(MAPIUID));
            sc = (*lpsmhdlg->lpfnAlloc) (crl * sizeof(MAPIUID), &lpsmhdlg->lpmuid);
            if (!FAILED (sc))
            {
                lpsmhdlg->cmuidMax = crl;
                for (; crl--; lpmuid++)
                {
                    hr = lpsmhdlg->lpadmin->lpVtbl->OpenProfileSection (lpsmhdlg->lpadmin,
                                                        lpmuid,
                                                        NULL,
                                                        MAPI_MODIFY,
                                                        &lpsec);
                    if (!HR_FAILED (hr))
                    {
                        hr = HrGetOneProp ((LPMAPIPROP)lpsec, PR_DISPLAY_NAME, &lpval);
                        if (!HR_FAILED (hr))
                        {
                            if ((ListBox_AddString (hctrl, lpval->Value.LPSZ) != LB_ERRSPACE)  &&
                                (ListBox_SetItemData (hctrl, irl, irl) != LB_ERR))
                                memcpy (&lpsmhdlg->lpmuid[irl++], lpmuid, sizeof(MAPIUID));
                        }
                    }

                    (*lpsmhdlg->lpfnFree) (lpval);
                    lpval = NULL;

                    UlRelease (lpsec);
                    lpsec = NULL;
                }

                /* Set the selection to the first filter in the list */

                ListBox_SetCurSel (hctrl, 0);
            }
            else
            {
                lpsmhdlg->sc = sc;
                return FALSE;
            }
        }
    }
    lpsmhdlg->cmuid = irl;
    EnableFilterPageCtrls (hdlg);
    return TRUE;
}


/*
 *  FilterPage_NOTIFY()
 *
 *  Purpose:
 *
 *      Handles the WM_NOTIFY message for the filter description dialog.
 *      On PSN_APPLY, the filter order is computed and set in PR_SMH_RULES.
 */
BOOL
FilterPage_NOTIFY (HWND hdlg, UINT id, NMHDR FAR * lpnmhdr)
{
    switch (lpnmhdr->code)
    {
      case PSN_KILLACTIVE:
      case PSN_RESET:
      case PSN_SETACTIVE:
      default:

        break;

      case PSN_APPLY:

        {
            SCODE sc = S_OK;
            BOOL fWarn = FALSE;
            HWND hctrl = GetDlgItem (hdlg, ID_FilterOrderLB);
            LPMAPIUID lpmuid;
            LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
            LPSPropValue lpval = lpsmhdlg->lpvalSMH;
            UINT cb = lpsmhdlg->cmuid * sizeof(MAPIUID);
            UINT crl;
            UINT imuid;
            UINT irl;

            if ((lpval[ipRules].ulPropTag != PR_SMH_RULES) ||
                (lpval[ipRules].Value.bin.cb < cb))
            {
                /* There in no room at the inn */

                sc = (*lpsmhdlg->lpfnAllocMore) (cb, lpval, &(lpval[ipRules].Value.bin.lpb));
            }
            if (!FAILED (sc))
            {
                crl = ListBox_GetCount (hctrl);
                lpmuid = (LPMAPIUID)lpval[ipRules].Value.bin.lpb;
                for (cb = 0, irl = 0; irl < crl; irl++, cb += sizeof(MAPIUID))
                {
                    imuid = (UINT)ListBox_GetItemData (hctrl, irl);
                    memcpy (lpmuid++, &lpsmhdlg->lpmuid[imuid], sizeof(MAPIUID));
                }
            }
            lpval[ipRules].Value.bin.cb = cb;
            lpval[ipRules].ulPropTag = PR_SMH_RULES;
            sc = GetScode (lpsmhdlg->lpsec->lpVtbl->SetProps (lpsmhdlg->lpsec, cpMax, lpval, NULL));
            lpsmhdlg->sc = (FAILED (sc) ? sc : (fWarn ? MAPI_W_ERRORS_RETURNED : S_OK));
            lpsmhdlg->fDirty = (!FAILED (sc));
        }
        return TRUE;

#ifdef PSN_HASHELP
      case PSN_HASHELP:
#endif
      case PSN_HELP:

        return TRUE;
    }

    return FALSE;
}


/*
 *  FilterPage_COMMAND()
 *
 *  Purpose:
 *
 *      Handles the WM_COMMAND message for the filter description dialog.
 */
BOOL
FilterPage_COMMAND (HWND hdlg, UINT id, HWND hwndCtl, UINT codeNotify)
{
    SCODE sc;
    HWND hctrl = GetDlgItem (hdlg, ID_FilterOrderLB);
    LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
    LPSPropValue lpval = NULL;
    LPMAPIUID lpmuidT = NULL;
    UINT cb;
    UINT irl = 0;
    UINT imuid;
    ULONG dwData;

    irl = ListBox_GetCurSel (hctrl);
    switch (id)
    {
      case ID_NewFilter:

        if (lpsmhdlg->cmuid == lpsmhdlg->cmuidMax)
        {
            /* We need to grow the critter */

            cb = lpsmhdlg->cmuidMax * sizeof(MAPIUID);
            sc = (*lpsmhdlg->lpfnAlloc) (cb + (GROW_SIZE * sizeof(MAPIUID)), &lpmuidT);
            if (FAILED (sc))
            {
                lpsmhdlg->sc = sc;
                break;
            }
            memset (&lpmuidT[lpsmhdlg->cmuid], 0, GROW_SIZE * sizeof(MAPIUID));

            if (cb && lpsmhdlg->lpmuid)
                memcpy (lpmuidT, lpsmhdlg->lpmuid, cb);

            (*lpsmhdlg->lpfnFree) (lpsmhdlg->lpmuid);
            lpsmhdlg->lpmuid = lpmuidT;
            lpsmhdlg->cmuidMax += GROW_SIZE;
        }
        imuid = lpsmhdlg->cmuid;
        memset (&lpsmhdlg->lpmuid[imuid], 0, sizeof(MAPIUID));
        sc = ScEditFilterDesc (lpsmhdlg, hdlg, NEW_RULE, &lpsmhdlg->lpmuid[imuid], &lpval);
        if (!FAILED (sc))
        {
            if ((irl = ListBox_AddString (hctrl, lpval[ipDisp].Value.LPSZ)) != LB_ERR)
            {
                ListBox_SetItemData (hctrl, irl, imuid);
                ListBox_SetCurSel (hctrl, irl);
                PropSheet_Changed (GetParent (hdlg), hdlg);
                lpsmhdlg->cmuid++;
            }
            else
                sc = MAPI_E_CALL_FAILED;
        }
        lpsmhdlg->sc = sc;
        (*lpsmhdlg->lpfnFree) (lpval);
        break;

      case ID_EditFilter:

        imuid = (UINT)ListBox_GetItemData (hctrl, irl);
        sc = ScEditFilterDesc (lpsmhdlg, hdlg, EDIT_RULE, &lpsmhdlg->lpmuid[imuid], &lpval);
        if (!FAILED (sc))
        {
            dwData = ListBox_GetItemData (hctrl, irl);
            if ((ListBox_DeleteString (hctrl, irl) == LB_ERR) ||
                (ListBox_InsertString (hctrl, irl, lpval[ipDisp].Value.LPSZ) == LB_ERRSPACE) ||
                (ListBox_SetItemData (hctrl, irl, imuid) == LB_ERR))
                sc = MAPI_E_NOT_ENOUGH_MEMORY;

            ListBox_SetCurSel (hctrl, irl);
            PropSheet_Changed (GetParent (hdlg), hdlg);
        }
        lpsmhdlg->sc = sc;
        (*lpsmhdlg->lpfnFree) (lpval);
        break;

      case ID_RmvFilter:

        if ((irl != LB_ERR) &&
            (ListBox_DeleteString (hctrl, irl) == LB_ERR))
            lpsmhdlg->sc = MAPI_E_CALL_FAILED;

        ListBox_SetCurSel (hctrl, irl);
        break;

      case ID_FilterDown:

        if ((irl == LB_ERR) ||
            (irl == (UINT)(ListBox_GetCount (hctrl) - 1)))
                break;

        dwData = ListBox_GetItemData (hctrl, irl + 1);
        if ((ListBox_GetText (hctrl, irl + 1, lpsmhdlg->rgchT) == LB_ERR) ||
            (dwData == LB_ERR))
        {
            lpsmhdlg->sc = MAPI_E_CALL_FAILED;
            break;
        }

        if ((ListBox_DeleteString (hctrl, irl + 1) == LB_ERR) ||
            (ListBox_InsertString (hctrl, irl, lpsmhdlg->rgchT) == LB_ERRSPACE))
        {
            ListBox_ResetContent (hctrl);
            lpsmhdlg->sc = MAPI_E_CALL_FAILED;
            break;
        }
        else
            ListBox_SetItemData (hctrl, irl, dwData);

        ListBox_SetCurSel (hctrl, irl + 1);
        PropSheet_Changed (GetParent (hdlg), hdlg);
        break;

      case ID_FilterUp:

        if ((irl == LB_ERR) || (irl == 0))
            break;

        dwData = ListBox_GetItemData (hctrl, irl - 1);
        if ((ListBox_GetText (hctrl, irl - 1, lpsmhdlg->rgchT) == LB_ERR) ||
            (dwData == LB_ERR))
        {
            lpsmhdlg->sc = MAPI_E_CALL_FAILED;
            break;
        }

        if ((ListBox_DeleteString (hctrl, irl - 1) == LB_ERR) ||
            (ListBox_InsertString (hctrl, irl, lpsmhdlg->rgchT) == LB_ERRSPACE))
        {
            ListBox_ResetContent (hctrl);
            lpsmhdlg->sc = MAPI_E_CALL_FAILED;
            break;
        }
        else
            ListBox_SetItemData (hctrl, irl, dwData);

        ListBox_SetCurSel (hctrl, irl - 1);
        PropSheet_Changed (GetParent (hdlg), hdlg);
        break;

      default:
        break;
    }

    EnableFilterPageCtrls (hdlg);
    return TRUE;
}


/*
 *  FilterPageProc()
 *
 *  Purpose:
 *
 *      Dispatches window messages to the proper function for processing
 */
BOOL CALLBACK
FilterPageProc (HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
      case WM_INITDIALOG:

        FHandleWm (FilterPage, hdlg, INITDIALOG, wParam, lParam);
        return TRUE;

      case WM_COMMAND:

        FHandleWm (FilterPage, hdlg, COMMAND, wParam, lParam);
        return TRUE;

      case WM_DRAWITEM:

        DrawODButton (((LPSMHDLG)GetWindowLong (hdlg, DWL_USER))->hinst,
            (DRAWITEMSTRUCT FAR *)lParam);
        break;

      case WM_DESTROY:

        break;

      case WM_NOTIFY:

        return FHandleWm (FilterPage, hdlg, NOTIFY, wParam, lParam);
    }

    return FALSE;
}


/*
 *  GeneralPage_INITDAILOG()
 *
 *  Purpose:
 *
 *      Handles the WM_INITDIALOG message for the filter description dialog
 */
BOOL
GeneralPage_INITDIALOG (HWND hdlg, HWND hwndFocus, LPARAM lParam)
{
    LPSMHDLG lpsmhdlg = (LPSMHDLG)(((PROPSHEETPAGE *)lParam)->lParam);
    LPSPropValue lpval = lpsmhdlg->lpvalSMH;
    ULONG ulFlags;

    MakeDlg3D (hdlg);
    SetWindowLong (hdlg, DWL_USER, ((PROPSHEETPAGE *)lParam)->lParam);

    ulFlags = (lpval[ipSMHFlags].ulPropTag == PR_SMH_FLAGS)
                ? lpval[ipSMHFlags].Value.l
                : 0;

    /* Enable/check the checkboxes based on SMH flags */

    CheckDlgButton (hdlg, ID_SentMail, !!(ulFlags & SMH_FILTER_SENTMAIL));
    CheckDlgButton (hdlg, ID_SentMailYr, !!(ulFlags & SMH_FILTER_SENTMAIL_YR));
    CheckDlgButton (hdlg, ID_Deleted, !!(ulFlags & SMH_FILTER_DELETED));
    CheckDlgButton (hdlg, ID_DeletedYr, !!(ulFlags & SMH_FILTER_DELETED_YR));
    CheckDlgButton (hdlg, ID_Inbound, !!(ulFlags & SMH_FILTER_INBOUND));
    CheckDlgButton (hdlg, ID_Unread, !!(ulFlags & SMH_UNREAD_VIEWER));
    EnableWindow (GetDlgItem (hdlg, ID_DeletedYr), !!(ulFlags & SMH_FILTER_DELETED));
    EnableWindow (GetDlgItem (hdlg, ID_SentMailYr), !!(ulFlags & SMH_FILTER_SENTMAIL));
    lpval[ipSMHFlags].ulPropTag = PR_SMH_FLAGS;
    lpval[ipSMHFlags].Value.l = ulFlags;
    return TRUE;
}

/*
 *  GeneralPage_NOTIFY()
 *
 *  Purpose:
 *
 *      Handles the WM_NOTIFY message for the filter description dialog.
 *      On PSN_APPLY, SMHFlags is recomputed from the checkbox states.
 */
BOOL
GeneralPage_NOTIFY (HWND hdlg, UINT id, NMHDR FAR * lpnmhdr)
{
    switch (lpnmhdr->code)
    {
      case PSN_KILLACTIVE:
      case PSN_RESET:
      case PSN_SETACTIVE:
      default:

        break;

      case PSN_APPLY:
        {
            LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
            LPSPropValue lpval = lpsmhdlg->lpvalSMH;

            if (IsDlgButtonChecked (hdlg, ID_SentMail))
            {
                lpval[ipSMHFlags].Value.l |= SMH_FILTER_SENTMAIL;
                if (IsDlgButtonChecked (hdlg, ID_SentMailYr))
                    lpval[ipSMHFlags].Value.l |= SMH_FILTER_SENTMAIL_YR;
                else
                    lpval[ipSMHFlags].Value.l &= ~SMH_FILTER_SENTMAIL_YR;
            }
            else
                lpval[ipSMHFlags].Value.l &= ~(SMH_FILTER_SENTMAIL | SMH_FILTER_SENTMAIL_YR);

            if (IsDlgButtonChecked (hdlg, ID_Deleted))
            {
                lpval[ipSMHFlags].Value.l |= SMH_FILTER_DELETED;
                if (IsDlgButtonChecked (hdlg, ID_DeletedYr))
                    lpval[ipSMHFlags].Value.l |= SMH_FILTER_DELETED_YR;
                else
                    lpval[ipSMHFlags].Value.l &= ~SMH_FILTER_DELETED_YR;
            }
            else
                lpval[ipSMHFlags].Value.l &= ~(SMH_FILTER_DELETED | SMH_FILTER_DELETED_YR);

            if (IsDlgButtonChecked (hdlg, ID_Inbound))
                lpval[ipSMHFlags].Value.l |= SMH_FILTER_INBOUND;
            else
                lpval[ipSMHFlags].Value.l &= ~SMH_FILTER_INBOUND;

            if (IsDlgButtonChecked (hdlg, ID_Unread))
                lpval[ipSMHFlags].Value.l |= SMH_UNREAD_VIEWER;
            else
                lpval[ipSMHFlags].Value.l &= ~SMH_UNREAD_VIEWER;

            lpsmhdlg->sc = GetScode (lpsmhdlg->lpsec->lpVtbl->SetProps (lpsmhdlg->lpsec,
                cpMax, lpval, NULL));

            lpsmhdlg->fDirty = (!FAILED (lpsmhdlg->sc));
        }
        return TRUE;

#ifdef PSN_HASHELP
      case PSN_HASHELP:
#endif
      case PSN_HELP:

        return TRUE;
    }

    return FALSE;
}


/*
 *  GeneralPage_COMMAND()
 *
 *  Purpose:
 *
 *      Handles the WM_COMMAND message for the filter description dialog
 *
 *      IMPORTANT: This function relies on the dialog control IDs as
 *      defined in _SMH.RH.  The yearly archive checkboxes must have an
 *      ID that is one greater than the companion checkbox.
 */
BOOL
GeneralPage_COMMAND (HWND hdlg, UINT id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
      case ID_SentMail:
      case ID_Deleted:

        EnableWindow (GetDlgItem (hdlg, id + 1),
                    !!IsDlgButtonChecked (hdlg, ID_SentMail));
        break;

      case ID_SentMailYr:
      case ID_DeletedYr:
      case ID_Inbound:
      case ID_Unread:

        break;

      default:

        return TRUE;
    }
    PropSheet_Changed (GetParent (hdlg), hdlg);
    return TRUE;
}


/*
 *  GeneralPageProc()
 *
 *  Purpose:
 *
 *      Dispatches window messages to the proper function for processing
 */
BOOL CALLBACK
GeneralPageProc (HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
      case WM_INITDIALOG:

        FHandleWm (GeneralPage, hdlg, INITDIALOG, wParam, lParam);
        return TRUE;

      case WM_COMMAND:

        FHandleWm (GeneralPage, hdlg, COMMAND, wParam, lParam);
        break;

      case WM_DRAWITEM:

        break;

      case WM_DESTROY:

        break;

      case WM_NOTIFY:

        return FHandleWm (GeneralPage, hdlg, NOTIFY, wParam, lParam);
    }

    return FALSE;
}


/*
 *  ExclusionEdit_INITDIALOG()
 *
 *  Purpose:
 *
 *      Handles the WM_INITDIALOG message for the filter description dialog
 */
BOOL
ExclusionEdit_INITDIALOG (HWND hdlg, HWND hwndFocus, LPARAM lParam)
{
    MakeDlg3D (hdlg);
    SetWindowLong (hdlg, DWL_USER, lParam);

    /* Enable/limit the exclusion edit */

    Edit_LimitText (GetDlgItem (hdlg, ID_ExclusionClass), cchRuleMax);
    Button_Enable (GetDlgItem (hdlg, IDOK), FALSE);
    return TRUE;
}


/*
 *  ExclusionEdit_COMMAND()
 *
 *  Purpose:
 *
 *      Handles the WM_COMMAND message for the filter description dialog
 */
BOOL ExclusionEdit_COMMAND (HWND hdlg, UINT id, HWND hwndCtl, UINT codeNotify)
{
    LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
    HWND hctrl = GetDlgItem (hdlg, ID_ExclusionClass);

    switch (id)
    {
      case IDOK:

        Edit_GetText (hctrl, lpsmhdlg->rgchT, cchRuleMax);
        break;

      case IDCANCEL:

        break;

      default:

        return FALSE;
    }

    EndDialog (hdlg, id);
    return TRUE;
}


/*
 *  ExclusionEditProc()
 *
 *  Purpose:
 *
 *      Dispatches window messages to the proper function for processing
 */
BOOL CALLBACK
ExclusionEditProc (HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hctrl = GetDlgItem (hdlg, ID_ExclusionClass);

    switch (msg)
    {
      case WM_INITDIALOG:

        FHandleWm (ExclusionEdit, hdlg, INITDIALOG, wParam, lParam);
        return TRUE;

      case WM_COMMAND:

        FHandleWm (ExclusionEdit, hdlg, COMMAND, wParam, lParam);
        break;

      case WM_DRAWITEM:

        break;

      case WM_DESTROY:

        break;
    }
    Button_Enable (GetDlgItem (hdlg, IDOK), !!Edit_GetTextLength (hctrl));
    return FALSE;
}


/*
 *  ExclusionPage_INITDIALOG()
 *
 *  Purpose:
 *
 *      Handles the WM_INITDIALOG message for the filter description dialog
 */
BOOL
ExclusionPage_INITDIALOG (HWND hdlg, HWND hwndFocus, LPARAM lParam)
{
    LPSMHDLG lpsmhdlg = (LPSMHDLG)(((PROPSHEETPAGE *)lParam)->lParam);
    HWND hctrl = GetDlgItem (hdlg, ID_ExclusionLB);
    UINT isz;

    MakeDlg3D (hdlg);
    SetWindowLong (hdlg, DWL_USER, ((PROPSHEETPAGE *)lParam)->lParam);

    /* Populate the exclusion listbox */

    if (lpsmhdlg->lpvalSMH[ipExc].ulPropTag == PR_SMH_EXCLUSIONS)
        for (isz = 0; isz < lpsmhdlg->lpvalSMH[ipExc].Value.MVSZ.cValues; isz++)
            ListBox_AddString (hctrl, lpsmhdlg->lpvalSMH[ipExc].Value.MVSZ.LPPSZ[isz]);

    return TRUE;
}


/*
 *  ExclusionPage_NOTIFY()
 *
 *  Purpose:
 *
 *      Handles the WM_NOTIFY message for the filter description dialog
 */
BOOL
ExclusionPage_NOTIFY (HWND hdlg, UINT id, NMHDR FAR * lpnmhdr)
{
    SCODE sc;
    LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
    HWND hctrl = GetDlgItem (hdlg, ID_ExclusionLB);
    LPTSTR FAR * lppsz = NULL;
    UINT cex;
    UINT iex;

    switch (lpnmhdr->code)
    {
      case PSN_KILLACTIVE:
      case PSN_RESET:
      case PSN_SETACTIVE:
      default:

        break;

      case PSN_APPLY:

        /* Assemble a new PR_SMH_EXCLUSIONS property value */

        cex = ListBox_GetCount (hctrl);
        if (cex)
        {
            sc = (*lpsmhdlg->lpfnAllocMore) (cex * sizeof(LPTSTR),
                                    lpsmhdlg->lpvalSMH,
                                    (LPVOID FAR *)&lppsz);
            if (!FAILED (sc))
            {
                lpsmhdlg->lpvalSMH[ipExc].ulPropTag = PR_SMH_EXCLUSIONS;
                lpsmhdlg->lpvalSMH[ipExc].Value.MVSZ.LPPSZ = lppsz;
                for (iex = 0; iex < cex; iex++, lppsz++)
                {
                    sc = (*lpsmhdlg->lpfnAllocMore) (ListBox_GetTextLen (hctrl, iex) + 1,
                                    lpsmhdlg->lpvalSMH,
                                    lppsz);
                    if (FAILED (sc))
                        break;

                    ListBox_GetText (hctrl, iex, *lppsz);
                }
                lpsmhdlg->lpvalSMH[ipExc].Value.MVSZ.cValues = iex;

                /* Set the new value */

                sc = GetScode (lpsmhdlg->lpsec->lpVtbl->SetProps (lpsmhdlg->lpsec,
                        cpMax, lpsmhdlg->lpvalSMH, NULL));

                lpsmhdlg->fDirty = (!FAILED (sc));
            }
            lpsmhdlg->sc = sc;
        }
        else
        {
            lpsmhdlg->lpvalSMH[ipExc].ulPropTag = PR_NULL;
            HrDeleteOneProp ((LPMAPIPROP)lpsmhdlg->lpsec, PR_SMH_EXCLUSIONS);
        }
        return TRUE;

#ifdef PSN_HASHELP
      case PSN_HASHELP:
#endif
      case PSN_HELP:

        return TRUE;
    }

    return FALSE;
}


/*
 *  ExclusionPage_COMMAND()
 *
 *  Purpose:
 *
 *      Handles the WM_COMMAND message for the filter description dialog
 */
BOOL
ExclusionPage_COMMAND (HWND hdlg, UINT id, HWND hwndCtl, UINT codeNotify)
{
    LPSMHDLG lpsmhdlg = (LPSMHDLG)GetWindowLong (hdlg, DWL_USER);
    HWND hctrl = GetDlgItem (hdlg, ID_ExclusionLB);
    UINT iex;

    switch (id)
    {
      case ID_NewExclusion:

        id = DialogBoxParam (lpsmhdlg->hinst,
                    MAKEINTRESOURCE (SMH_ExclusionEdit),
                    GetParent(hdlg),
                    ExclusionEditProc,
                    (LPARAM)lpsmhdlg);
        if (id == IDOK)
            ListBox_SetCurSel (hctrl, ListBox_AddString (hctrl, lpsmhdlg->rgchT));

        PropSheet_Changed (GetParent (hdlg), hdlg);
        break;

      case ID_RmvExclusion:

        iex = ListBox_GetCurSel (hctrl);
        ListBox_DeleteString (hctrl, iex);
        ListBox_SetCurSel (hctrl, iex);
        PropSheet_Changed (GetParent (hdlg), hdlg);
        break;

      default:

        break;
    }
    return TRUE;
}


/*
 *  ExclusionPageProc()
 *
 *  Purpose:
 *
 *      Dispatches window messages to the proper function for processing
 */
BOOL CALLBACK
ExclusionPageProc (HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
      case WM_INITDIALOG:

        FHandleWm (ExclusionPage, hdlg, INITDIALOG, wParam, lParam);
        return TRUE;

      case WM_COMMAND:

        FHandleWm (ExclusionPage, hdlg, COMMAND, wParam, lParam);
        return TRUE;

      case WM_DESTROY:

        break;

      case WM_NOTIFY:

        return FHandleWm (ExclusionPage, hdlg, NOTIFY, wParam, lParam);
    }

    return FALSE;
}


/*
 *  HrDisplayPropSheets()
 *
 *  Purpose:
 *
 *      Brings up the SMH property sheets.
 *
 *  Returns:
 *
 *      (HRESULT)
 */
HRESULT
HrDisplayPropSheets (HINSTANCE hinst,
    HWND hwnd,
    LPSMHDLG lpsmhdlg)
{
    UINT ipg;
    HRESULT hr = hrSuccess;
    CHAR rgch[60] = {0};
    PROPSHEETPAGE psp[] =
    {
        {
            sizeof(PROPSHEETPAGE),
            PSP_USETITLE,
            hinst,
            MAKEINTRESOURCE(SMH_GeneralPage),
            NULL,
            MAKEINTRESOURCE(SMH_GeneralTab),
            GeneralPageProc,
            0,
            NULL,
            NULL
        },
        {
            sizeof(PROPSHEETPAGE),
            PSP_USETITLE,
            hinst,
            MAKEINTRESOURCE(SMH_FilterPage),
            NULL,
            MAKEINTRESOURCE(SMH_FilterTab),
            FilterPageProc,
            0,
            NULL,
            NULL
        },
        {
            sizeof(PROPSHEETPAGE),
            PSP_USETITLE,
            hinst,
            MAKEINTRESOURCE(SMH_ExclusionPage),
            NULL,
            MAKEINTRESOURCE(SMH_ExclusionTab),
            ExclusionPageProc,
            0,
            NULL,
            NULL
        },
    };
    PROPSHEETHEADER psh =
    {
        sizeof(PROPSHEETHEADER),
        PSH_PROPSHEETPAGE | PSH_PROPTITLE,
        hwnd,
        hinst,
        NULL,
        NULL,
        sizeof(psp) / sizeof(PROPSHEETPAGE),
        0,
        (LPCPROPSHEETPAGE)&psp
    };

    /* set property sheet data */

    for (ipg = 0; ipg < psh.nPages; ipg++)
        psp[ipg].lParam = (LPARAM)lpsmhdlg;

    if (LoadString (hinst, SMH_ProviderName, rgch, sizeof(rgch)))
    {
        RegDlg3D (hinst);
        psh.pszCaption = rgch;
        switch (PropertySheet (&psh))
        {
          case -1:

            hr = ResultFromScode (MAPI_E_CALL_FAILED);
            break;

          case 0:

            hr = ResultFromScode (MAPI_E_USER_CANCEL);
            break;
        }
        UnregDlg3D (hinst);
    }
    else
        hr = ResultFromScode (MAPI_E_CALL_FAILED);

    (*lpsmhdlg->lpfnFree) (lpsmhdlg->lpmuid);
    DebugTraceResult (HrDisplayPropSheets(), hr);
    return hr;
}
