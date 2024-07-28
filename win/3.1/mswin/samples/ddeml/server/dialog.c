#include "ddemlsv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "huge.h"

LONG GetDlgItemLong(HWND hwnd, WORD id, BOOL *pfTranslated, BOOL fSigned);
VOID SetDlgItemLong(HWND hwnd, WORD id, LONG l, BOOL fSigned);

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : DoDialog()                                                 *
 *                                                                          *
 *  PURPOSE    : Generic dialog invocation routine.  Handles procInstance   *
 *               stuff, focus management and param passing.                 *
 *  RETURNS    : result of dialog procedure.                                *
 *                                                                          *
 ****************************************************************************/
int FAR DoDialog(
LPSTR lpTemplateName,
FARPROC lpDlgProc,
DWORD param,
BOOL fRememberFocus)
{
    WORD wRet;
    HWND hwndFocus;
    WORD cRunawayT;

    cRunawayT = cRunaway;
    cRunaway = 0;           // turn off runaway during dialogs.

    if (fRememberFocus)
        hwndFocus = GetFocus();
    lpDlgProc = MakeProcInstance(lpDlgProc, hInst);
    wRet = DialogBoxParam(hInst, lpTemplateName, hwndServer, lpDlgProc, param);
    FreeProcInstance(lpDlgProc);
    if (fRememberFocus)
        SetFocus(hwndFocus);

    cRunaway = cRunawayT;   // restore runaway state.
    return wRet;
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

    COMMENTS:

        No initialization is needed for this particular dialog box, but TRUE
        must be returned to Windows.

        Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:                /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:                      /* message: received a command */
            if (wParam == IDOK                /* "OK" box selected?          */
                || wParam == IDCANCEL) {      /* System menu close command? */
                EndDialog(hDlg, TRUE);        /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);                           /* Didn't process a message    */
}




BOOL FAR PASCAL RenderDelayDlgProc(
HWND          hwnd,
register WORD msg,
register WORD wParam,
LONG          lParam)
{
    switch (msg){
    case WM_INITDIALOG:
        SetWindowText(hwnd, "Data Render Delay");
        SetDlgItemInt(hwnd, IDEF_VALUE, RenderDelay, FALSE);
        SetDlgItemText(hwnd, IDTX_VALUE, "Delay in milliseconds:");
        return(1);
        break;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            RenderDelay = GetDlgItemInt(hwnd, IDEF_VALUE, NULL, FALSE);
            // fall through
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;

        default:
            return(FALSE);
        }
        break;
    }
    return(FALSE);
}




BOOL FAR PASCAL SetTopicDlgProc(
HWND          hwnd,
register WORD msg,
register WORD wParam,
LONG          lParam)
{
    char szT[MAX_APPNAME + MAX_TOPIC + 3];

    switch (msg){
    case WM_INITDIALOG:
        SetWindowText(hwnd, "Set Topic Dialog");
        SendDlgItemMessage(hwnd, IDEF_VALUE, EM_LIMITTEXT, MAX_TOPIC, 0);
        SetDlgItemText(hwnd, IDEF_VALUE, szTopic);
        SetDlgItemText(hwnd, IDTX_VALUE, "Topic:");
        return(1);
        break;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            DdeFreeStringHandle(idInst, topicList[1].hszTopic);
            GetDlgItemText(hwnd, IDEF_VALUE, szTopic, MAX_TOPIC);
            topicList[1].hszTopic = DdeCreateStringHandle(idInst, szTopic, NULL);
            strcpy(szT, szServer);
            strcat(szT, " | ");
            strcat(szT, szTopic);
            SetWindowText(hwndServer, szT);
            InvalidateRect(hwndServer, &rcTopics, TRUE);
            // fall through
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;

        default:
            return(FALSE);
        }
        break;
    }
    return(FALSE);
}


BOOL FAR PASCAL SetServerDlgProc(
HWND          hwnd,
register WORD msg,
register WORD wParam,
LONG          lParam)
{
    char szT[MAX_APPNAME + MAX_TOPIC + 3];

    switch (msg){
    case WM_INITDIALOG:
        SetWindowText(hwnd, "Set Server Name Dialog");
        SendDlgItemMessage(hwnd, IDEF_VALUE, EM_LIMITTEXT, MAX_APPNAME, 0);
        SetDlgItemText(hwnd, IDEF_VALUE, szServer);
        SetDlgItemText(hwnd, IDTX_VALUE, "Server:");
        return(1);
        break;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            GetDlgItemText(hwnd, IDEF_VALUE, szServer, MAX_APPNAME);
            DdeNameService(idInst, hszAppName, NULL, DNS_UNREGISTER);
            DdeFreeStringHandle(idInst, hszAppName);
            hszAppName = DdeCreateStringHandle(idInst, szServer, NULL);
            DdeNameService(idInst, hszAppName, NULL, DNS_REGISTER);
            strcpy(szT, szServer);
            strcat(szT, " | ");
            strcat(szT, szTopic);
            SetWindowText(hwndServer, szT);
            // fall through
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;

        default:
            return(FALSE);
        }
        break;
    }
    return(FALSE);
}




BOOL FAR PASCAL ContextDlgProc(
HWND          hwnd,
register WORD msg,
register WORD wParam,
LONG          lParam)
{
    BOOL fSuccess;

    switch (msg){
    case WM_INITDIALOG:
        SetDlgItemInt(hwnd, IDEF_FLAGS, CCFilter.wFlags, FALSE);
        SetDlgItemInt(hwnd, IDEF_COUNTRY, CCFilter.wCountryID, FALSE);
        SetDlgItemInt(hwnd, IDEF_CODEPAGE, CCFilter.iCodePage, TRUE);
        SetDlgItemLong(hwnd, IDEF_LANG, CCFilter.dwLangID, FALSE);
        SetDlgItemLong(hwnd, IDEF_SECURITY, CCFilter.dwSecurity, FALSE);
        return(1);
        break;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            CCFilter.wFlags = GetDlgItemInt(hwnd, IDEF_FLAGS, &fSuccess, FALSE);
            if (!fSuccess) return(0);
            CCFilter.wCountryID = GetDlgItemInt(hwnd, IDEF_COUNTRY, &fSuccess, FALSE);
            if (!fSuccess) return(0);
            CCFilter.iCodePage = GetDlgItemInt(hwnd, IDEF_CODEPAGE, &fSuccess, TRUE);
            if (!fSuccess) return(0);
            CCFilter.dwLangID = GetDlgItemLong(hwnd, IDEF_LANG, &fSuccess, FALSE);
            if (!fSuccess) return(0);
            CCFilter.dwSecurity = GetDlgItemLong(hwnd, IDEF_SECURITY, &fSuccess, FALSE);
            if (!fSuccess) return(0);
            // fall through
        case IDCANCEL:
            EndDialog(hwnd, 0);
            break;

        default:
            return(FALSE);
        }
        break;
    }
    return(FALSE);
}




LONG GetDlgItemLong(
HWND hwnd,
WORD id,
BOOL *pfTranslated,
BOOL fSigned)
{
    char szT[20];

    if (!GetDlgItemText(hwnd, id, szT, 20)) {
        if (pfTranslated != NULL) {
            *pfTranslated = FALSE;
        }
        return(0L);
    }
    if (pfTranslated != NULL) {
        *pfTranslated = TRUE;
    }
    return(atol(szT));
}


VOID SetDlgItemLong(
HWND hwnd,
WORD id,
LONG l,
BOOL fSigned)
{
    char szT[20];

    ltoa(l, szT, 10);
    SetDlgItemText(hwnd, id, szT);
}

