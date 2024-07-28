/****************************************************************************
 *
 *   config.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include "ibmjoy.h"

/****************************************************************************

    local data

 ***************************************************************************/

static WORD wcNewAxes;  /* 2 or 3 */

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | Config | This puts up the configuration dialog box.
 *
 * @parm HWND | hWnd | Our Window handle.
 *
 * @parm HANDLE | hInstance | Our instance handle.
 *
 * @rdesc Returns whatever was returned from the dialog box procedure.
 ***************************************************************************/
int FAR PASCAL Config(HWND hWnd, HANDLE hInstance)
{
FARPROC fpDlg;
int iResult;

    fpDlg = MakeProcInstance(ConfigDlgProc, hInstance);
    iResult = DialogBox(hInstance, "Config", hWnd, fpDlg);
    FreeProcInstance(fpDlg);
    return iResult;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | GetAxes | Determines number of axes user has indicated in
 *     configuration dialog box.
 *
 * @parm HWND | hDlg | Handle to the configuration dialog box.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void GetAxes(HWND hDlg)
{
   if (IsDlgButtonChecked(hDlg, IDC_2AXES)) 
       wcNewAxes = 2;
   else if (IsDlgButtonChecked(hDlg, IDC_3AXES)) 
       wcNewAxes = 3;
   else
       wcNewAxes = DEF_AXES;   /* default is 2 */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | ConfigDlgProc | Dialog proc for the configuration dialog box.
 *
 * @parm HWND | hDlg | Handle to the configuration dialog box.
 *
 * @parm unsigned | msg | Message sent to the dialog box.
 *
 * @parm WORD | wParam | Message dependent parameter.
 *
 * @parm LONG | lParam | Message dependent parameter.
 *
 * @rdesc Returns DRVCNF_RESTART if the user has changed settings, which will
 *     cause the drivers applet which launched this to give the user a
 *     message about having to restart Windows for the changes to take
 *     effect.  If the user clicks on "Cancel" or if no settings have changed,
 *     DRVCNF_CANCEL is returned.
 ***************************************************************************/
int FAR PASCAL ConfigDlgProc(HWND hDlg, unsigned msg, WORD wParam, LONG lParam)
{
int  i;
char buf[6];               /* to put number of axes in gszIniFile */

    switch (msg) {

    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_2AXES, IDC_3AXES, gwcAxes - 2 + IDC_2AXES);
        break;

    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
            GetAxes(hDlg);    /* from radio buttons */
            wsprintf(buf, "%d", wcNewAxes);

            /* save number of axes to gszIniFile */
            WritePrivateProfileString(gszDriverName, gszAxes, buf, gszIniFile);

            /* get timeout (unplugged) loop value from gszIniFile */
            ibmjoyGetTimeoutValue();

            /* if user changed number of axes, give option to reboot */
            if (wcNewAxes != gwcAxes)
                EndDialog(hDlg, DRVCNF_RESTART);
            else 
                EndDialog(hDlg, DRVCNF_CANCEL);
            break;

        case IDCANCEL:
            /* user hit escape, so lose any changes */
            EndDialog(hDlg, DRVCNF_CANCEL);
            break;

        case IDC_2AXES:
        case IDC_3AXES:
            /* check clicked button and uncheck the other */
            CheckRadioButton(hDlg, IDC_2AXES, IDC_3AXES, wParam);
            break;

        default:
            break;
        }
        break;

    default:
        return FALSE;
    }

    return TRUE;
}
