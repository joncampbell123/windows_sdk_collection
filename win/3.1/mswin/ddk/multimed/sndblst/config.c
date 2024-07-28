/****************************************************************************
 *
 *   config.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include "sndblst.h"

static char _based(_segname("_CODE")) aszHexFormat[] = "%X";
static char _based(_segname("_CODE")) aszIntFormat[] = "%d";

/*****************************************************************************

    internal function prototypes

 ****************************************************************************/ 

static int NEAR PASCAL PortToId(WORD wPort)
{
    switch(wPort) {
        case 0x210:  return IDC_210;
        case 0x220:  return IDC_220;
        case 0x230:  return IDC_230;
        case 0x240:  return IDC_240;
        case 0x250:  return IDC_250;
        case 0x260:  return IDC_260;
        default:     return -1;
    }                              
}

static WORD NEAR PASCAL IdToPort(int id)
{
    switch(id) {
        case IDC_210:  return 0x210;
        case IDC_220:  return 0x220;
        case IDC_230:  return 0x230;
        case IDC_240:  return 0x240;
        case IDC_250:  return 0x250;
        case IDC_260:  return 0x260;
        default:       return (WORD)-1;
    }
}

static int NEAR PASCAL IntToId(BYTE bInt)
{
    switch(bInt) {
        case 2:  return IDC_2;
        case 9:  return IDC_2;
        case 3:  return IDC_3;
        case 5:  return IDC_5;
        case 7:  return IDC_7;
        default: return -1;
    }
}

static BYTE NEAR PASCAL IdToInt(int id)
{
    switch(id) {
        case IDC_2:  return 9;
        case IDC_3:  return 3;
        case IDC_5:  return 5;
        case IDC_7:  return 7;
        default:     return (BYTE)-1;
    }
}

/***************************************************************************/

static void NEAR PASCAL ConfigErrorMsgBox(HWND hDlg, WORD wStringId)
{
char szErrorBuffer[MAX_ERR_STRING];    /* buffer for error messages */

    LoadString(ghModule, wStringId, szErrorBuffer, sizeof(szErrorBuffer));
    MessageBox(hDlg, szErrorBuffer, STR_PRODUCTNAME, MB_OK|MB_ICONEXCLAMATION);
}

/***************************************************************************/

void FAR PASCAL ConfigRemove(void)
{
    WritePrivateProfileString(STR_DRIVERNAME, NULL, NULL, STR_INIFILE);
}

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
    return DialogBox(hInstance, MAKEINTATOM(DLG_CONFIG), hWnd, (FARPROC)ConfigDlgProc);
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | GetPortAndInt | Determines which port and interrupt settings
 *     the user has chosen in the configuration dialog box.
 *
 * @parm HWND | hDlg | Handle to the configuration dialog box.
 *
 * @rdesc HIWORD = new interrupt, LOWORD = new port
 ***************************************************************************/
static DWORD NEAR PASCAL GetPortAndInt(HWND hDlg)
{
WORD wNewPort   = -1;       /* new port chosen by user in config box */
BYTE bNewInt    = -1;       /* new interrupt chosen */
int  id;

    for (id = IDC_FIRSTPORT; id <= IDC_LASTPORT; id++)
        if (IsDlgButtonChecked(hDlg, id)) {
            wNewPort = IdToPort(id);
            break;
        }

    for (id = IDC_FIRSTINT; id <= IDC_LASTINT; id++)
        if (IsDlgButtonChecked(hDlg, id)) {
            bNewInt = IdToInt(id);
            break;
        }

    return MAKELONG(wNewPort, bNewInt);
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | ConfigDlgProc | Dialog proc for the configuration dialog box.
 *
 * @parm HWND | hDlg | Handle to the configuration dialog box.
 *
 * @parm WORD | msg | Message sent to the dialog box.
 *
 * @parm WORD | wParam | Message dependent parameter.
 *
 * @parm LONG | lParam | Message dependent parameter.
 *
 * @rdesc Returns DRV_RESTART if the user has changed settings, which will
 *     cause the drivers applet which launched this to give the user a
 *     message about having to restart Windows for the changes to take
 *     effect.  If the user clicks on "Cancel" or if no settings have changed,
 *     DRV_CANCEL is returned.
 ***************************************************************************/
int FAR PASCAL _loadds ConfigDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam)
{
char    buf[20];            /* buffer to write profile string into */
DWORD   dw;                 /* return value from GetPortAndInt */
WORD    wNewPort;           /* new port chosen by user in config box */
BYTE    bNewInt;            /* new interrupt chosen */
BYTE    bNewDMAChannel;     /* new DMA channel chosen */
int     id;

    switch (msg) {
        case WM_INITDIALOG:
            bNewInt = ConfigGetIRQ();
            wNewPort = ConfigGetPortBase();

            if ((id = PortToId(wNewPort)) != -1)
                CheckRadioButton(hDlg, IDC_FIRSTPORT, IDC_LASTPORT, id);

            if ((id = IntToId(bNewInt)) != -1)
                CheckRadioButton(hDlg, IDC_FIRSTINT, IDC_LASTINT, id);
            break;

        case WM_COMMAND:
            switch (wParam) {
                case IDOK:
                    dw = GetPortAndInt(hDlg);
                    wNewPort = LOWORD(dw);
                    bNewInt  = (BYTE)HIWORD(dw);

                    /*  we don't have a config option for DMA channel */
                    /*  so use the current setting */
                    bNewDMAChannel = (BYTE)(HIWORD(InitGetConfiguration())>>8);

                    /*  verify settings - if this fails, DO NOT WRITE INI */
                    /*  SETTINGS! */
                    if (LOWORD(dw = InitVerifyConfiguration(wNewPort, bNewInt, bNewDMAChannel)))
                    {
                        ConfigErrorMsgBox(hDlg, LOWORD(dw));
                        break;
                    }

                    /*  do NOT allow driver to install on < 2.00 DSP!! */
                    if (HIWORD(dw) < DSP_VERSION_REQD) {
                        /*  display error and DON'T write INI settings - */
                        /*  this way the driver will never enable */
                        ConfigErrorMsgBox(hDlg, IDS_ERRBADVERSION);
                        break;
                    }

                    /* high bit set if Thunder Board - warn them */
                    /* but continue installing */
                    if (HIWORD(dw) & 0x8000)
                        ConfigErrorMsgBox(hDlg, IDS_WARNTHUNDER);

                    /*  if installing on a Pro card, warn them */
                    /*  but continue installing */
                    else if (HIWORD(dw) >= DSP_VERSION_PRO)
                        ConfigErrorMsgBox(hDlg, IDS_WARNPROCARD);

                    /* settings are valid, so write them out */
                    wsprintf(buf, aszHexFormat, wNewPort);
                    WritePrivateProfileString(STR_DRIVERNAME, STR_PORT, buf, STR_INIFILE);
                    wsprintf(buf, aszIntFormat, bNewInt);
                    WritePrivateProfileString(STR_DRIVERNAME, STR_INT, buf, STR_INIFILE);

                    dw = InitGetConfiguration();
                    if ((wNewPort != LOWORD(dw))||(bNewInt != (BYTE)HIWORD(dw)))
                        EndDialog(hDlg, DRVCNF_RESTART);
                    else
                        EndDialog(hDlg, DRVCNF_CANCEL);
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, DRVCNF_CANCEL);
                    break;

                case IDC_210:
                case IDC_220:
                case IDC_230:
                case IDC_240:
                case IDC_250:
                case IDC_260:
                    CheckRadioButton(hDlg, IDC_FIRSTPORT, IDC_LASTPORT, wParam);
                    break;

                case IDC_2:
                case IDC_3:
                case IDC_5:
                case IDC_7:
                    CheckRadioButton(hDlg, IDC_FIRSTINT, IDC_LASTINT, wParam);
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
