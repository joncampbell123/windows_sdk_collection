/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  DefDlgProc() -                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

LRESULT API IDefDlgProc(HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
{
    HWND hwndT1;
    HWND hwndT2;
    BOOL result;

    ((PDLG)hwnd)->resultWP = 0L;
    result = FALSE;
    //
    // Call the dialog proc if it exists
    //
    if (((PDLG)hwnd)->lpfnDlg == NULL ||
            !(result = CallDlgProc(hwnd, message, wParam, lParam)))
        {

        // Make sure window still exists.
        if (!IsWindow(hwnd))
        {
            DebugErr(DBF_ERROR, "Dialog window destroyed in dialog callback");
            goto ReturnIt;
        }

        switch (message)
        {
        case WM_ERASEBKGND:
            FillWindow(hwnd, hwnd, (HDC)wParam, (HBRUSH)CTLCOLOR_DLG);
            return((LRESULT)(LONG)TRUE);

        case WM_SHOWWINDOW:
            // If hiding the window, save the focus. If showing the window
            // by means of a SW_* command and the fEnd bit is set, do not
            // pass to DWP so it won't get shown.
            //
            if (!wParam)
                SaveDlgFocus(hwnd);
            else if (LOWORD(lParam) && pdlg->fEnd)
                break;
            return(DefWindowProc(hwnd, message, wParam, lParam));

        case WM_SYSCOMMAND:
            //
            // If we're minimizing and a dialog control has the focus,
            // save the hwnd for that control
            //
            if ((int) wParam == SC_MINIMIZE)
                SaveDlgFocus(hwnd);
            return(DefWindowProc(hwnd, message, wParam, lParam));

        case WM_ACTIVATE:
            if (fDialog = (wParam != 0))
                RestoreDlgFocus(hwnd);
            else
                SaveDlgFocus(hwnd);
            break;

        case WM_SETFOCUS:
            if (!pdlg->fEnd && !RestoreDlgFocus(hwnd))
                DlgSetFocus(GetFirstTab(hwnd));
            break;

        case WM_CLOSE:
            // Make sure cancel button is not disabled before sending the
            // IDCANCEL.  Note that we need to do this as a message instead
            // of directly calling the dlg proc so that any dialog box
            // filters get this.
            //
            hwndT1 = GetDlgItem(hwnd, IDCANCEL);
            if (hwndT1 && TestWF(hwndT1, WFDISABLED))
                MessageBeep(0);
            else
                PostMessage(hwnd, WM_COMMAND, (WPARAM)IDCANCEL, MAKELPARAM(hwndT1, BN_CLICKED));
            break;

        case WM_NCDESTROY:
            fDialog = FALSE;      /* clear this flag */

            // Make sure we are going to terminate the mode loop, in case
            // DestroyWindow was called instead of EndDialog.  We'll RIP
            // in DialogBox2.
            //
            ((PDLG)hwnd)->fEnd = TRUE;

            if (!(hwnd->style & DS_LOCALEDIT))
            {
                if (((PDLG)hwnd)->hData)
                {

                    GlobalUnlock(((PDLG)hwnd)->hData);

                    ReleaseEditDS(((PDLG)hwnd)->hData);
                    ((PDLG)hwnd)->hData = NULL;
                }
            }

            // Delete the user defined font if any
            if (((PDLG)hwnd)->hUserFont)
            {
                DeleteObject((HANDLE)((PDLG)hwnd)->hUserFont);
                ((PDLG)hwnd)->hUserFont = NULL;
            }

            // Always let DefWindowProc do its thing to ensure that
            // everything associated with the window is freed up.
            //
            DefWindowProc(hwnd, message, wParam, lParam);
            break;

        case DM_SETDEFID:
            if (!(((PDLG)hwnd)->fEnd))
            {
                // Make sure that the new default button has the highlight.
                // We need to ignore this if we are ending the dialog box
                // because hwnd->result is no longer a default window id but
                // rather the return value of the dialog box.
                //
                // Catch the case of setting the defid to null or setting
                // the defid to something else when it was initially null.
                //
                CheckDefPushButton(hwnd,
                     (((PDLG)hwnd)->result ? GetDlgItem(hwnd, ((PDLG)hwnd)->result) : NULL),
                     (wParam               ? GetDlgItem(hwnd, (int) wParam        ) : NULL) );
                ((PDLG)hwnd)->result = (int)wParam;
            }
            return((LRESULT)(DWORD)TRUE);

        case DM_GETDEFID:
            if (!((PDLG)hwnd)->fEnd && ((PDLG)hwnd)->result)
                return(MAKELRESULT(((PDLG)hwnd)->result, DC_HASDEFID));
            else
                return(0L);

        case WM_NEXTDLGCTL:
            // This message is so TAB-like operations can be properly handled
            // (simple SetFocus won't do the default button stuff)
            //
            hwndT2 = hwndFocus;
            if (LOWORD(lParam))
            {
                if (hwndT2 == NULL)
                    hwndT2 = hwnd;

                // wParam contains the hwnd of the ctl to set focus to
                hwndT1 = (HWND)wParam;
            }
            else
            {
                if (hwndT2 == NULL)
                {
                    // Set focus to the first tab item.
                    hwndT1 = GetFirstTab(hwnd);
                    hwndT2 = hwnd;
                }
                else
                {
                    // If window with focus not a dlg ctl, ignore message.
                    if (!IsChild(hwnd, hwndT2))
                        return((LRESULT)(LONG)TRUE);

                    // wParam = TRUE for previous, FALSE for next
                    hwndT1 = GetNextDlgTabItem(hwnd, hwndT2, (BOOL)wParam);
                }
            }
            DlgSetFocus(hwndT1);
            CheckDefPushButton(hwnd, hwndT2, hwndT1);
            return((LRESULT)(LONG)TRUE);

        case WM_ENTERMENULOOP:
        case WM_LBUTTONDOWN:
        case WM_NCLBUTTONDOWN:
            //
            // PLEASE NOTE: The following code is a VERY UGLY compatibility
            // hack.  NEVER write code that looks at the window proc address
            // in order to determine the window type.  The following code will
            // not work with subclassed combo or edit controls.
            //
            if (hwndT1 = hwndFocus)
            {
                if (hwndT1->pcls->lpfnWndProc == ComboBoxCtlWndProc)
                {
                    // If user clicks anywhere in dialog box and a combo box (or
                    // the editcontrol of a combo box) has the focus, then hide
                    // it's listbox.
                    //
                    SendMessage(hwndT1, CB_SHOWDROPDOWN, FALSE, 0L);
                }
                else
                {
                    if (hwndT1->pcls->lpfnWndProc == EditWndProc &&
                        hwndT1->hwndParent->pcls->lpfnWndProc==ComboBoxCtlWndProc)
                    {
                        SendMessage(hwndT1->hwndParent,CB_SHOWDROPDOWN, FALSE, 0L);
                    }
                }
            }
            return(DefWindowProc(hwnd, message, wParam, lParam));

        case WM_GETFONT:
            return (LRESULT)(DWORD)(WORD)((PDLG)hwnd)->hUserFont;

        // We don't want to pass the following messages to DefWindowProc:
        // instead, return the value returned by the dialog proc (FALSE)
        //
        case WM_VKEYTOITEM:
        case WM_COMPAREITEM:
        case WM_CHARTOITEM:
        case WM_INITDIALOG:
            break;

        default:
            return(DefWindowProc(hwnd, message, wParam, lParam));
        }
    }

ReturnIt:
    // These messages are special cased in an unusual way: the return value
    // of the dialog function is not BOOL fProcessed, but instead it's the
    // return value of these messages.
    //
    if (message == WM_CTLCOLOR ||
        message == WM_COMPAREITEM ||
        message == WM_VKEYTOITEM ||
        message == WM_CHARTOITEM ||
        message == WM_QUERYDRAGICON ||
        message == WM_INITDIALOG)
    {
        return((LRESULT)(DWORD)result);
    }

    return(((PDLG)hwnd)->resultWP);
}

BOOL SaveDlgFocus(HWND hwnd)
{
    if (hwndFocus && IsChild(hwnd, hwndFocus) && !((PDLG)hwnd)->hwndFocusSave)
    {
        ((PDLG)hwnd)->hwndFocusSave = hwndFocus;
        RemoveDefaultButton(hwnd, hwndFocus);
        return(TRUE);
    }
    return(FALSE);
}

BOOL RestoreDlgFocus(HWND hwnd)
{
    BOOL fRestored = FALSE;

    if (((PDLG)hwnd)->hwndFocusSave && !TestWF(hwnd, WFMINIMIZED))
    {
        if (IsWindow(((PDLG)hwnd)->hwndFocusSave))
        {
            CheckDefPushButton(hwnd, hwndFocus, ((PDLG)hwnd)->hwndFocusSave);
            SetFocus(((PDLG)hwnd)->hwndFocusSave);
            fRestored = TRUE;
        }

        ((PDLG)hwnd)->hwndFocusSave = NULL;
    }
    return(fRestored);
}
