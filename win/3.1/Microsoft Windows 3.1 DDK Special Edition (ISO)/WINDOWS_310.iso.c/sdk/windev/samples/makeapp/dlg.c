#include "makeapp.h"

static BOOL fDefDlgEx = FALSE;

int SampleDlg_Do(HWND hwndOwner)
{
    int result;
    DLGPROC lpfndp;

    lpfndp = (DLGPROC)MakeProcInstance((FARPROC)SampleDlg_OldDlgProc, g_app.hinst);

    result = DialogBoxParam(g_app.hinst,
            MAKEINTRESOURCE(IDR_SAMPLEDLG),
            hwndOwner, lpfndp, 0L);

    FreeProcInstance((FARPROC)lpfndp);

    return result;
}

BOOL CALLBACK _export SampleDlg_OldDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CheckDefDlgRecursion(&fDefDlgEx);

    return SetDlgMsgResult(hwndDlg, msg, SampleDlg_DlgProc(hwndDlg, msg, wParam, lParam));
}

LRESULT SampleDlg_DefProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefDlgProcEx(hwndDlg, msg, wParam, lParam, &fDefDlgEx);
}

LRESULT SampleDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_MSGFILTER)
        return HANDLE_WM_MSGFILTER(hwndDlg, wParam, lParam, SampleDlg_OnMsgFilter);

    switch (msg)
    {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, SampleDlg_OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, SampleDlg_OnCommand);
    default:
        return SampleDlg_DefProc(hwndDlg, msg, wParam, lParam);
    }
}

BOOL SampleDlg_OnMsgFilter(HWND hwndDlg, MSG FAR* lpmsg, int context)
{
    HWND hwndMsg = lpmsg->hwnd;

    // If we're in DialogBox(), or this is a modeless dialog and
    // we're in our main loop, check for F1 or see if this is a
    // dialog message.  Otherwise, don't do any filtering.
    //
    switch (context)
    {
    case MSGF_DIALOGBOX:
    case MSGF_MAINLOOP:
        break;

    default:
        return FALSE;
    }

    // If the message is destined for the dialog or one of its
    // children, then check for possible F1 key press.
    //
    // (NOTE: F1 and other keys could be handled with a call to
    //  TranslateAccelerator as well)
    //
    if (hwndMsg == hwndDlg || IsChild(hwndDlg, hwndMsg))
    {
        // If F1 key was pressed, then post WM_COMMAND CMD_HELPABOUT
        // to the dialog box.
        //
        if (lpmsg->message == WM_KEYDOWN && lpmsg->wParam == VK_F1)
            FORWARD_WM_COMMAND(hwndDlg, CMD_HELPABOUT, NULL, 0, PostMessage);
        else
            return IsDialogMessage(hwndDlg, lpmsg);
    }
    else
    {
        return IsDialogMessage(hwndDlg, lpmsg);
    }
}

BOOL SampleDlg_OnInitDialog(HWND hwndDlg, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndEdit1 = GetDlgItem(hwndDlg, CTL_EDIT1);
    HWND hwndOK    = GetDlgItem(hwndDlg, CTL_OK);

    if (Edit_GetTextLength(hwndEdit1) == 0)
        Button_Enable(hwndOK, FALSE);
    else
        Button_Enable(hwndOK, TRUE);

    return TRUE;
}

VOID SampleDlg_OnCommand(HWND hwndDlg, UINT id, HWND hwndCtl, UINT code)
{
    HWND hwndEdit1  = GetDlgItem(hwndDlg, CTL_EDIT1);
    HWND hwndOK     = GetDlgItem(hwndDlg, CTL_OK);
    HWND hwndCancel = GetDlgItem(hwndDlg, CTL_CANCEL);

    if (hwndCtl == NULL)
    {
        switch (id)
        {
        case CMD_HELPABOUT:
            AboutDlg_Do(hwndDlg);
            break;
        default:
            MessageBeep(0);
            break;
        }
    }
    else
    {
        if (id == CTL_OK && code == BN_CLICKED)
        {
            EndDialog(hwndDlg, TRUE);
        }
        else if (id == CTL_CANCEL && code == BN_CLICKED)
        {
            EndDialog(hwndDlg, FALSE);
        }
        else if (id == CTL_EDIT1 && code == EN_CHANGE)
        {
            if (Edit_GetTextLength(hwndEdit1) == 0)
                Button_Enable(hwndOK, FALSE);
            else
                Button_Enable(hwndOK, TRUE);
        }
        else if (id >= CTL_RED && id <= CTL_GREEN && code == BN_CLICKED)
        {
            CheckRadioButton(hwndDlg, CTL_RED, CTL_GREEN, id);
        }
        else if (id >= CTL_PURPLE && id <= CTL_SIENNA && code == BN_CLICKED)
        {
            HWND hwndCheck = GetDlgItem(hwndDlg, id);

            Button_SetCheck(hwndCheck, !Button_GetCheck(hwndCheck));
        }
    }
}
