/****************************************************************************
 *
 *   pioncnfg.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "mcipionr.h"
#include "pioncnfg.h"

#define MAX_INI_LENGTH   128               /* maximum .ini file line length */

#define SZCODE char _based(_segname("_CODE"))

static WORD nPort;                         /* which com port we're using */
static SZCODE szIniFile[] = "system.ini";  /* configuration information file */
static SZCODE szNull[] = "";
static SZCODE szCommIniFormat[] = "com%1d";

/****************************************************************************
 * @doc INTERNAL
 *
 * @api LPSTR | GetTail |  Find the start of the second word in the input.
 *
 * @parm LPSTR | pch | Input string.
 *
 * @rdesc Points to the first non-space character after the first word in
 *     the input.
 ***************************************************************************/
static LPSTR PASCAL NEAR GetTail(LPSTR  pch)
{
    while (*pch && *pch != ' ') 
        pch++;

    while (*pch == ' ')
        pch++ ;

    return pch;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | GetCmdParam | Returns the currently selected comport in
 *     system.ini.
 *
 * @parm LPDRVCONFIGINFO | lpdci | Config information from the DRV_CONFIGURE
 *     message.
 *
 * @rdesc Returns comport number.
 ***************************************************************************/
static WORD PASCAL NEAR GetCmdParam(LPDRVCONFIGINFO lpdci)
{
char sz[MAX_INI_LENGTH];

    if (GetPrivateProfileString(
         lpdci->lpszDCISectionName, lpdci->lpszDCIAliasName, szNull, sz,
         MAX_INI_LENGTH, szIniFile))
        return pionGetComport(GetTail(sz));
    else
        return 0;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | PutCmdParam | Sets the current comport in system.ini.
 *
 * @parm LPDRVCONFIGINFO | lpdci | Config information from the DRV_CONFIGURE
 *     message.
 *
 * @parm WORD | nPort | Comport to set.
 *
 * @rdesc Returns comport number.
 ***************************************************************************/
static void PASCAL NEAR PutCmdParam(LPDRVCONFIGINFO lpdci, WORD nPort)
{
char  sz[MAX_INI_LENGTH];
LPSTR lpch;
    
    if (GetPrivateProfileString(
        lpdci->lpszDCISectionName,
        lpdci->lpszDCIAliasName,
        szNull,  sz, MAX_INI_LENGTH-5, szIniFile)) {

        sz[MAX_INI_LENGTH-1] = 0;
        lpch = GetTail(sz);

        if (lpch > (LPSTR)sz && *(lpch - 1) != ' ')
            *lpch++ = ' ';

        wsprintf(lpch, szCommIniFormat, nPort + 1);
        WritePrivateProfileString(
            lpdci->lpszDCISectionName,
            lpdci->lpszDCIAliasName,
            sz, szIniFile);
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @func BOOL | ConfigDlgProc | Dialog proc for the configuration dialog box.
 *
 * @parm HWND | hDlg | Handle to the configuration dialog box.
 *
 * @parm UINT | msg | Message sent to the dialog box.
 *
 * @parm WPARAM | wParam | Message dependent parameter.
 *
 * @parm LPARAM | lParam | Message dependent parameter.
 *
 * @rdesc Returns DRV_OK if the user clicks on "OK" and DRV_CANCEL if the
 *     user clicks on "Cancel".
 ***************************************************************************/
BOOL FAR PASCAL _loadds ConfigDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
static LPDRVCONFIGINFO lpdci;

    switch (msg) {

    case WM_INITDIALOG:
        lpdci = (LPDRVCONFIGINFO)lParam;
        nPort = GetCmdParam(lpdci);
        CheckRadioButton(hDlg, P_COM1, P_COM4, P_COM1 + nPort);
        break;

    case WM_COMMAND:
        switch ((WORD)wParam) {

            case IDOK:
                PutCmdParam(lpdci, nPort);
                EndDialog(hDlg, DRVCNF_OK);
                break;

            case IDCANCEL:
                EndDialog(hDlg, DRVCNF_CANCEL);
                break;

            case P_COM1:
            case P_COM2:
            case P_COM3:
            case P_COM4:
                nPort = (WORD)wParam - P_COM1;
                break;

            default:
                break;
            }
        break;

    default:
        return FALSE;
        break;
    }

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @func int | pionConfig | This puts up the configuration dialog box.
 *
 * @parm HWND | hwndParent | Parent window.
 *
 * @parm LPDRVCONFIGINFO | lpInfo | Config information from the DRV_CONFIGURE
 *     message.
 *
 * @rdesc Returns whatever was returned from the dialog box procedure.
 ***************************************************************************/
int PASCAL FAR pionConfig(HWND hwndParent, LPDRVCONFIGINFO lpInfo)
{
    return DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PIONCNFG), hwndParent, ConfigDlgProc,
                             (DWORD)lpInfo);
}
