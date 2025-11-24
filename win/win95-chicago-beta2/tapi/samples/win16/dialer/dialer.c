//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1993-1995 Microsoft Corporation, all rights reserved.
//
/*****************************************************************************/
/* Dialer ---- Windows TAPI sample application created as an illustration of
   the usage of Windows TAPI

    Dialer does the following 3 things :

    (1) initiates/drops calls (2) handles simple TAPI request for other
    application initiating/dropping calls on their behalf (3) monitors
    incoming/outgoing calls and keeps a call log based on the user's request.

    dialer.c : contains dialer's main code module and all the related UI code.
    */

/* ***************************************************************************/
/* include files */

#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <errno.h>
#include <string.h>
#include <tapi.h>
#include <math.h>
#include <malloc.h>

#ifdef CHICAGO
#include "shellapi.h"
#endif

#include "dialer.h"
#include "dialhelp.h"

/* ***************************************************************************/
/* constants definitions */

#define cLogFieldMax            6       /* maximum number of log fields we
                                           allow */
#define iLogFieldType           0       /* index of the type of call field */
#define iLogFieldName           1       /* index of the name field */
#define iLogFieldNumber         2       /* index of the number field */
#define iLogFieldDate           3       /* index of the date field */
#define iLogFieldTime           4       /* index of the time field */
#define iLogFieldDuration       5       /* index of the duration field */
#define cchLogLineMax           256     /* maximum line length (in chars) =
                                           all fields plus tabs */
#define cchLogFileMax           32000   /* maximum number of characters we
                                           care to store */

#define szNameFormat            "%sn"
#define szNumFormat             "%sa"
#define szLocationFormat        "%sl"
#define szCallingCardFormat     "%sc"


/* ***************************************************************************/
/* struct definitions and the corresponding global declarations */

typedef struct tagDWS                   /* Dialer Window State */
    {
    HINSTANCE       hInst;              /* instance handle */
    HACCEL          haccel;             /* handle to our accelerator table */
    HWND            hwnd;               /* handle to our main window */
    HWND            hdlgCallStatus;     /* handle to the call status window */
    HWND            hdlgCallLog;        /* handle to the call logging window */
    HWND            hwndDummy    ;      /* handle to our dummy top level
                                           window */
    HWND            hdlgDialingPrompt;

    /* various resources we use */
    HFONT           hfntBtn;            /* the font that is used to display
                                           speed dial buttons and dial pad
                                           buttons */
    HFONT           hfntBtnText;        /* the font that is used to display
                                           the first line text in the dial pad
                                           buttons */
    HFONT           hfntBtnStar;        /* font used for * and # keys */
    HICON           hiconDialer;

    FARPROC         lpfnDefEditProc;
    FARPROC         lpfnSmartPasteEditProc;

    /* variousl flags we keep */
    BOOL        fCommandFromLogWindow;  /* tells me whether the
                                           Dial/LogOptions commands are issued
                                           from the log window's menu */
    BOOL        fLogIncomingCalls;      /* tells me whether incoming calls
                                           should be logged */
    BOOL        fLogOutgoingCalls;      /* tells me whether outgoing calls
                                           should be logged */
    BOOL        fCallLogVisible;        /* tells me whether the call log
                                           window is visible */
    BOOL        fCallLogDirty;          /* tells me whether the content of the
                                           call log window is out of date */
    BOOL        f24HourTime;            /* tells me whether we are on 24 hours
                                           time system */
    BOOL        fDisplayChangeOptionDlg;
    BOOL        fErrBoxOn;              /* an error dialog is displayed */
    BOOL        fCallActivateNextTask;  /* need to call ActivateNextTask */
    BOOL            fClassesRegistered;     /* need to unregister window classes */

    /* misc variables used for call logging */
    int             tabStops[cLogFieldMax];
    /* These are used for remembering what we've read into log window.  They
       are previous EOF and total data presented in log window.  We enforce
       the limit cchLogFileMax on curData so that we don't overflow control */
    LONG logLengthPrev;
    LONG curLogData;

    /* misc fields */
    WORD        cfDialer;               /* our private clipboard format. We
                                           use it as a flag to tell us whether
                                           the clipboard is from our log
                                           window */
    DWORD       dwSelPos;               /* text selection in the Number to
                                           Dial combo box */
    char        szAM[6];                /* call logging related strings */
    char        szPM[6];
    char        szDate[2];
    char        szTime[2];
    char        szDateFormat[20];
    char        szDialerName[32];       /* name of the application */

    } DWS;

DWS vdws = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
            NULL,NULL,NULL, FALSE,FALSE,FALSE,FALSE,
            TRUE,TRUE,FALSE,FALSE,FALSE,FALSE, {0},0,0,0, 0,0,0};

/* ***************************************************************************/
/* function declarations */

/* utility functions */

void CenterDlg(HWND hwndDlg);
int DoDialog(FARPROC pfnDlgProc,WORD iddlog,HINSTANCE hinst,HWND hwnd,
            LONG lref);
int IkszFromErrCode(DWORD errCode);
void DialerFatalExit(DWORD errCode);
void DisableDialerDesktop(BOOL fDisable);
void DialerReInitTapi(void);
int DialerErrMessageBox(int stringID,UINT wStyle);

/* main code module */

BOOL FDialerTranslateMessage(MSG *pmsg);
void HandleDialerWndCmds(WPARAM wParam,LPARAM lParam);
VOID DialPad(char chNum);

LRESULT FAR PASCAL _export      DialerWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,
            LPARAM lParam);
LRESULT FAR PASCAL _export      DummyWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,
            LPARAM lParam);

/* dialog procs for all the dialog boxes we bring up */

BOOL FAR PASCAL _export         ProgramSpeedDialDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         ProgramSpeedDialButtonDlgProc(HWND hDlg,
            UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         DialingOptionDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         CallLogOptionDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         AboutDialerDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         LineInUseDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         ChangeOptionDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL _export         DialingPromptDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);

LRESULT FAR PASCAL _export NameNumEditCtrlProc(HWND hwndCtrl,UINT uMsg,
            WPARAM wParam,LPARAM lParam);

/* call status dialog related function */

BOOL FCreateCallStatusDlg();
void ShowCallStatusDlg(LPCSTR szName,LPCSTR szNumber,LPCSTR szLocation,
            LPCSTR szCallingCard,LPCSTR szTranslatedSddress);
void UpdateCallStatusDlg(BOOL fCallIsActive,LPCSTR szName,LPCSTR szNumber);
void HideCallStatusDlg(void);

BOOL FAR PASCAL _export         CallStatusProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam);

/* Call logging related functions */

BOOL FAR PASCAL _export CallLogProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam);
BOOL FLogCall(BOOL,LPSTR,LPSTR,time_t,time_t);

void ShowCallLogDlg(BOOL fDoNotShow);
BOOL FReadCallLog(HWND,BOOL);
void SetCallLogTabs(HWND hwndList);
void LoadIntlFormats(void);
void GetSzNameNumFromSzLine(LPSTR szLine,LPSTR szName,LPSTR szNum);
BOOL FGetNameOfNumberFromCallLog(LPSTR szNumber,LPSTR szName);
void CopyLogToClipboard(HWND hwndList);
BOOL FRemoveLogEntries(HWND hwndList);
void PasteLogClipboardIntoEditCtrl(HWND hwndCtrl,BOOL fPasteName);

/* ***************************************************************************/
/* macro definitions */

/* Returns TRUE if preset (Quick Dial) is empty   */
#define FIsPresetEmpty(preset)  (!((preset)[0]))

/* Returns TRUE iff we have a dialable number in didDialerComboNumDial. We
    define this as just a non-empty string */
#define FNumberDialable()   (SendMessage(GetDlgItem(vdws.hwnd,didDialerComboNumDial),WM_GETTEXTLENGTH,0,0) > 0)

/* ***************************************************************************/
/* function definitions */
/* ***************************************************************************/
/* %%Function:_fstrncpyDialer  */
/* we make sure that the destination string is null-terminated. */

char far *_fstrncpyDialer(char far*lpszDest,const char far *lpszSrc,
            size_t cchDest,DWORD cchSrc)

{
    if (cchSrc > 0)
        {
        _fstrncpy(lpszDest,lpszSrc,cchDest);
        lpszDest[cchDest - 1] = 0;
        } /* if */
    else
        lpszDest[0] = 0;
    return lpszDest;

} /* _fstrncpyDialer */

/* ***************************************************************************/
/* %%Function:CenterDlg  */
/* this function centers the dialog hwndDlg with respect to vdws.hwnd. */

void CenterDlg(HWND hwndDlg)
{
    RECT rcDlg, rcDialerWnd, rcScreen;
    int dxd, dyd;

    GetWindowRect(hwndDlg,&rcDlg);
    OffsetRect(&rcDlg,-rcDlg.left,-rcDlg.top);

    GetWindowRect(GetDesktopWindow(),&rcScreen);
    if (vdws.fCommandFromLogWindow)
        GetWindowRect(vdws.hdlgCallLog,&rcDialerWnd);
    else if (!IsIconic(vdws.hwnd))
        GetWindowRect(vdws.hwnd,&rcDialerWnd);
    else
        /* center on screen */
        rcDialerWnd = rcScreen;

    dxd = max(0,(rcDialerWnd.right + rcDialerWnd.left - rcDlg.right)/2);
    dyd = max(0,(rcDialerWnd.bottom + rcDialerWnd.top - rcDlg.bottom)/2);

    if (rcDlg.right + dxd > rcScreen.right)
        dxd = rcScreen.right - rcDlg.right;
    if (rcDlg.bottom + dyd > rcScreen.bottom)
        dyd = rcScreen.bottom - rcDlg.bottom;

    OffsetRect(&rcDlg,dxd,dyd);
    MoveWindow(hwndDlg, rcDlg.left, rcDlg.top,
                rcDlg.right - rcDlg.left,
                    rcDlg.bottom - rcDlg.top,FALSE);

} /* CenterDlg */

/* ***************************************************************************/
/* %%Function:DoDialog */
/* General cover proc for running a modal dialog */

int DoDialog(FARPROC pfnDlgProc,WORD iddlog,HINSTANCE hinst,HWND hwnd,
            LONG lref)
{
    DLGPROC dlgproc;
    HWND hwndDisable = NULL;
    int res;

    if (hwnd == vdws.hdlgCallLog && IsWindowEnabled(vdws.hwnd))
        hwndDisable = vdws.hwnd;
    else if (vdws.fCallLogVisible && IsWindowEnabled(vdws.hdlgCallLog))
        hwndDisable = vdws.hdlgCallLog;

    if (hwndDisable)
        EnableWindow(hwndDisable,FALSE);

    dlgproc = (DLGPROC)MakeProcInstance((FARPROC)pfnDlgProc,hinst);
    res = DialogBoxParam(hinst, MAKEINTRESOURCE(iddlog),hwnd,dlgproc,lref);
    FreeProcInstance((FARPROC)dlgproc);

    if (hwndDisable)
        EnableWindow(hwndDisable,TRUE);

    if (vdws.fCallActivateNextTask)
        {
        vdws.fErrBoxOn = FALSE;
        ActivateNextTask();
        vdws.fCallActivateNextTask = FALSE;
        vdws.fErrBoxOn = TRUE;
        } /* if */

    return res;

} /* DoDialog */

/* ***************************************************************************/
/* %%Function:IkszFromErrCode */
/* translate errCode to its corresponding iksz string ID. */

int IkszFromErrCode(DWORD errCode)

{
    int ikszErr;

    switch (errCode)
        {
        case errNoVoiceLine:
            {
            ikszErr = ikszErrNoVoiceLine;
            break;
            }
        case errLineClose:
            {
            ikszErr = ikszErrLineClose;
            break;
            }
        case LINEERR_NODRIVER:
            {
            ikszErr = ikszErrLineInitNoDriver;
            break;
            }
        case LINEERR_INIFILECORRUPT:
            {
            ikszErr = ikszErrLineInitBadIniFile;
            break;
            }
        case LINEERR_NOMEM:
            {
            ikszErr = ikszErrOOM;
            break;
            }
        case LINEERR_INCOMPATIBLEAPIVERSION:
            {
            ikszErr = ikszErrLineInitWrongDrivers;
            break;
            }
        case LINEERR_OPERATIONFAILED:
            {
            ikszErr = ikszErrTAPI;
            break;
            }
        case LINEERR_INVALADDRESS:
            {
            ikszErr = ikszErrInvalAddress;
            break;
            }
        case LINEERR_ADDRESSBLOCKED:
            {
            ikszErr = ikszErrAddrBlocked;
            break;
            }
        case LINEERR_BILLINGREJECTED:
            {
            ikszErr = ikszErrBillingRejected;
            break;
            }
        case LINEERR_RESOURCEUNAVAIL:
        case LINEERR_ALLOCATED:
        case LINEERR_INUSE:
            {
            ikszErr = ikszErrResUnavail;
            break;
            }
        case LINEERR_NOMULTIPLEINSTANCE:
            {
            ikszErr = ikszErrNoMultipleInstance;
            break;
            }
        case LINEERR_INVALCALLSTATE:
            {
            ikszErr = ikszErrInvalCallState;
            break;
            }
        default:
            {
#ifdef DEBUG
                char msg[255];
                wsprintf (msg, "DIALER: unexpected error code 0x%08lX\n",
                         errCode);
                OutputDebugString (msg);
#endif
            ikszErr = ikszErrDefault;
            break;
            }
        } /* switch */
    return ikszErr;

} /* IkszFromErrCode */

/* ***************************************************************************/
/* %%Function:DialerCleanUp */
/* frees all the GDI objects we have allocated. */

void DialerCleanUp(void);
void DialerCleanUp()

{
    /* unregister and line close */
    TapiDone();

    /* kill the windows we created */
        if (vdws.hwndDummy)
        {
        DestroyWindow(vdws.hwndDummy);
        vdws.hwndDummy = NULL;
        } /* if */
    if (vdws.hwnd)
        {
        DestroyWindow(vdws.hwnd);
        vdws.hwnd = NULL;
        } /* if */
    if (vdws.hdlgCallLog)
        {
        DestroyWindow(vdws.hdlgCallLog);
        vdws.hdlgCallLog = NULL;
        } /* if */
        if (vdws.fClassesRegistered)
                {
        UnregisterClass(szDummyClass,vdws.hInst);
        UnregisterClass(szDialerClass,vdws.hInst);
                vdws.fClassesRegistered = FALSE;
                } /* if */

    if (vdws.hiconDialer)
        {
        DestroyIcon(vdws.hiconDialer);
        vdws.hiconDialer = NULL;
        } /* if */

    /* kill the fonts we created */
    if (vdws.hfntBtn)
        {
        DeleteObject(vdws.hfntBtn);
        vdws.hfntBtn = NULL;
        } /* if */
    if (vdws.hfntBtnText)
        {
        DeleteObject(vdws.hfntBtnText);
        vdws.hfntBtnText = NULL;
        } /* if */
    if (vdws.hfntBtnStar)
        {
        DeleteObject(vdws.hfntBtnStar);
        vdws.hfntBtnText = NULL;
        } /* if */
    if (vdws.lpfnSmartPasteEditProc)
        {
        FreeProcInstance(vdws.lpfnSmartPasteEditProc);
        vdws.lpfnSmartPasteEditProc = NULL;
        } /* if */

} /* DialerCleanUp */

/* ***************************************************************************/
/* %%Function:DialerFatalExit */
/* called when we encounter fatal error and we need to exit application. tells
    user what's wrong by putting up a message box and then calls DestroyWindow
    to end the world.  */

VOID FAR DialerFatalExit(DWORD errCode)
{
    DialerErrMessageBox(IkszFromErrCode(errCode),MB_APPLMODAL | MB_ICONEXCLAMATION);
    DialerCleanUp();

} /* DialerFatalExit */

/* ***************************************************************************/
/* %%Function:HwndDialerMain */
/* returns vdws.hwnd. */

HWND HwndDialerMain()

{
    return vdws.hwnd;

} /* HwndDialerMain */

/* ***************************************************************************/
/* %%Function:HInstDialer */
/* returns vdws.hInst. */

HWND HInstDialer()

{
    return vdws.hInst;

} /* HInstDialer */

/* ***************************************************************************/
/* %%Function:ActivateNextTask */
/* activate the task whose top-level window is just below Dialer's.
    this is to compensate for the fact that no widnow handle is being
    passed in with the STapi MakeCall request. */

void ActivateNextTask()

{
    HWND hwndTask = vdws.hwndDummy, hwndTaskNext, hwndPopup = NULL;

    if (vdws.fErrBoxOn)
        {
        vdws.fCallActivateNextTask = TRUE;
        return;
        } /* if */

    while ((hwndTaskNext = GetNextWindow(hwndTask,GW_HWNDNEXT)) != NULL)
        {
        hwndPopup = GetLastActivePopup(hwndTaskNext);
        if (IsWindowVisible(hwndTaskNext) || hwndPopup != hwndTaskNext && IsWindowVisible(hwndPopup))
            break;
        hwndTask = hwndTaskNext;
        } /* while */

    if (hwndPopup != NULL)
        BringWindowToTop(hwndPopup);

} /* ActivateNextTask */


/* ***************************************************************************/
/* %%Function:DisableDialerDesktop */
/* disables/enables Dialer's dialing functionality while waiting for the line
    state to returns to idle. */

void DisableDialerDesktop(BOOL fDisable)

{
    int did;

    EnableWindow(GetDlgItem(vdws.hwnd,didDialerBtnDial),!fDisable);
    for (did = didDialerBtnSpeedDialFirst; did <= didDialerBtnSpeedDialLast;
             ++did)
        EnableWindow(GetDlgItem(vdws.hwnd,did),!fDisable);

} /* DisableDialerDesktop */

/* ***************************************************************************/
/* %%Function:DialerReInitTapi */
/* called to reinit tapi after telephon.ini changes. */

void DialerReInitTapi(void)

{
    DWORD errCode;

    /* Initialize tapi, Negotiate TAPI versions for all line devices.Get
       lineDevCaps for all lines, and init address info. */
    if ((errCode = ErrStartTapi(vdws.hInst,vdws.szDialerName)) != errNone)
        {/* bail out */
        DialerFatalExit(errCode);
        return;
        } /* if */

    ErrInitCallingCardInfo(vdws.hInst);

    /* register ourselves to be simple TAPI recipient */
    if (!FRegisterSimpleTapi(TRUE))
        DialerErrMessageBox(ikszWarningRegisterSTapi,MB_ICONEXCLAMATION | MB_OK);

} /* DialerReInitTapi */

/* ***************************************************************************/
/* %%Function:DialerErrMessageBox */
/* Puts up a simple warning message when something non-tragic goes wrong.
    stringID - ID to pass to LoadString for text of warning message */

int DialerErrMessageBox(int stringID,UINT wStyle)
{
    char szText[cchSzMax];
    char szTitle[cchSzMax];
    int wResult;

    if (!LoadString(vdws.hInst,stringID,szText,sizeof(szText))
        || !LoadString(vdws.hInst,ikszWarningTitle,szTitle,sizeof(szTitle)))
        {
        MessageBeep(MB_ICONEXCLAMATION);
        return IDCANCEL;
        } /* if */

    vdws.fErrBoxOn = TRUE;
    if (vdws.fCallLogVisible)
        EnableWindow(vdws.hdlgCallLog,FALSE);
    wResult = MessageBox(vdws.hwnd,szText,szTitle,wStyle);
    if (vdws.fCallLogVisible)
        EnableWindow(vdws.hdlgCallLog,TRUE);
    vdws.fErrBoxOn = FALSE;

    return wResult;

} /* DialerErrMessageBox */

/* ***************************************************************************/
/* main code module */
/* ***************************************************************************/
/* %%Function:FDialerTranslateMessage */
/* translate VK_TAB and VK_RETURN keyboard messages passed to vdws.hwnd's
    children so that these key strokes have the same effects has controls in a
    dialog box. */

BOOL FDialerTranslateMessage(MSG *pmsg)

{
    HWND hwndCtrl;

    /* the following code deals with the RETURN key handling when the focus is
       on the list in the call log window */
    if (pmsg->message == WM_KEYUP && pmsg->wParam == VK_RETURN
        && pmsg->hwnd != NULL && vdws.fCallLogVisible
        && vdws.hdlgCallLog != NULL
        && GetFocus() == GetDlgItem(vdws.hdlgCallLog,didCallLogSTextLog))
        {
        SendMessage(vdws.hdlgCallLog,WM_COMMAND,didCallLogSTextLog,
                MAKELPARAM(GetDlgItem(vdws.hdlgCallLog,didCallLogSTextLog),
                CBN_DBLCLK));
        return TRUE;
        } /* if */

    /* the following code deals with the fact that we can't get the text
       selection of the combo box when it does not have focus. We need to know
       the text selection for the function DialPad */
    if (pmsg->hwnd != NULL && GetParent(pmsg->hwnd) == GetDlgItem(vdws.hwnd,
            didDialerComboNumDial) && GetFocus() == pmsg->hwnd)
        vdws.dwSelPos = SendMessage(GetDlgItem(vdws.hwnd,didDialerComboNumDial),
                CB_GETEDITSEL,0,0L);

    if (pmsg->message != WM_CHAR || pmsg->hwnd != NULL
        && GetParent(hwndCtrl = pmsg->hwnd) != vdws.hwnd
        && GetParent(pmsg->hwnd) != (hwndCtrl = GetDlgItem(vdws.hwnd,
            didDialerComboNumDial)))
        return FALSE;

    if (pmsg->wParam == VK_TAB)
        {
        HWND hwndNextCtrl;

        if (hwndNextCtrl = GetNextDlgTabItem(vdws.hwnd,hwndCtrl,
                GetKeyState(VK_SHIFT) < 0))
            {
            SetFocus(hwndNextCtrl);
            return TRUE;
            } /* if */
        } /* if */
    else if (pmsg->wParam == VK_RETURN)
        {
        int didCtrl;

        if ((didCtrl = GetDlgCtrlID(hwndCtrl)) == didDialerComboNumDial
            && IsWindowEnabled(GetDlgItem(vdws.hwnd,didDialerBtnDial)))
            SendMessage(vdws.hwnd,WM_COMMAND,didDialerBtnDial,
                    MAKELONG(GetDlgItem(vdws.hwnd,didDialerBtnDial),
                    BN_CLICKED));
        else
            SendMessage(vdws.hwnd,WM_COMMAND,didCtrl,MAKELONG(pmsg->hwnd,0));
        return TRUE;
        } /* else if */

    return FALSE;

} /* DialerTranslateMessage */

/* ***************************************************************************/
/* %%Function:DummyWndProc */
/* Window proc for vdws.hwndDummy. */

LRESULT FAR PASCAL _export DummyWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,
                            LPARAM lParam)

{
    return DefWindowProc(hwnd,uMsg,wParam,lParam);

} /* DummyWndProc */

/* ***************************************************************************/
/* %%Function:WinMain */
/* entry point to the Dialer program. */

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hprvinst, LPSTR lpszcmdline,
             int ncmdshow)
{
    WORD        rgBits[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    char        szClipbrdFormat[cchSzMax];
    MSG         msg;
    WNDCLASS    wndclass, wndclassDummy;
    RECT        rc;
    HDC         hdcScreen;
    DWORD       errCode;

    /* allow only one instance */
    if (hprvinst)
        {
        /* Find the first instance main window, and post a message to it to
           tell it to activate itself */
        HWND hwnd = FindWindow(szDialerClass,NULL);

        ShowWindow(hwnd,SW_SHOWNORMAL);
        PostMessage(hwnd,WM_ACTIVATEAPP,TRUE,0);
        return FALSE;
        } /* if */
    vdws.hInst = hinst;

    LoadString(hinst,ikszAppName,vdws.szDialerName,sizeof(vdws.szDialerName));

    /* we can still run the program withtout the being able to load the
       accelerator */
    vdws.haccel = LoadAccelerators(vdws.hInst,MAKEINTRESOURCE(aidDialer));

    vdws.hiconDialer = LoadIcon(hinst,MAKEINTRESOURCE(icoDialer));

    /* get international date/time formats */
    LoadIntlFormats();

    /* Dialer window class definition */
    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = DialerWndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = DLGWINDOWEXTRA;
    wndclass.hInstance      = hinst;
    wndclass.hIcon          = vdws.hiconDialer;
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = COLOR_3DFACE + 1;
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = szDialerClass;

    /* Dummy top level window class definition */
    wndclassDummy.style          = CS_HREDRAW | CS_VREDRAW;
    wndclassDummy.lpfnWndProc    = DummyWndProc;
    wndclassDummy.cbClsExtra     = 0;
    wndclassDummy.cbWndExtra     = 0;
    wndclassDummy.hInstance      = hinst;
    wndclassDummy.hIcon          = vdws.hiconDialer;
    wndclassDummy.hCursor        = NULL;
    wndclassDummy.hbrBackground  = NULL;
    wndclassDummy.lpszMenuName   = NULL;
    wndclassDummy.lpszClassName  = szDummyClass;

    vdws.lpfnSmartPasteEditProc = MakeProcInstance((FARPROC)NameNumEditCtrlProc,
            vdws.hInst);

    /* register window class; load accelerators; and create the Dialer Dialing
       window. return if OOM */
    if (RegisterClass(&wndclass) == 0
        || RegisterClass(&wndclassDummy) == 0
                || (vdws.hwndDummy = CreateWindow(szDummyClass,vdws.szDialerName,
                WS_POPUP,0,0,0,0,NULL,NULL,hinst,NULL)) == NULL
        || (vdws.hwnd = CreateDialog(hinst,MAKEINTRESOURCE(dlgDialer),
                vdws.hwndDummy,NULL)) == NULL
        || !FCreateCallStatusDlg())
        {
        char szT[256];

        LoadString(vdws.hInst,ikszErrOOM,szT,sizeof(szT));
        MessageBox(NULL,szT,vdws.szDialerName,MB_SYSTEMMODAL
            | MB_ICONEXCLAMATION);
        goto LOOM;
        } /* if */

        vdws.fClassesRegistered = TRUE;
    ShowWindow(vdws.hwndDummy,SW_SHOW);
    EnableWindow(vdws.hwndDummy,FALSE);

    SetCursor(LoadCursor(NULL,IDC_WAIT));

    /* set up line,address and last number(s) dialed pull downs */
    GetAllINIQuickDials(vdws.hInst,vdws.hwnd);
    GetLastDialedNumbers(vdws.hInst,vdws.hwnd);

    /* set the position of the window as it was before */
    GetWindowRect(vdws.hwnd,&rc);
    GetSetINIScreenPos(vdws.hInst,(POINT *)&rc,TRUE);
    SetWindowPos(vdws.hwnd,NULL,rc.left,rc.top,0,0,SWP_NOSIZE | SWP_NOACTIVATE
        | SWP_NOREDRAW |SWP_NOZORDER);

    /* set the font for the speed dial/dial pad buttons */
    hdcScreen = GetDC(GetDesktopWindow());
    vdws.hfntBtn = CreateFont((-10)*GetDeviceCaps(hdcScreen,LOGPIXELSY)/72,0,0,0,
            FW_BOLD,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
                VARIABLE_PITCH|FF_SWISS,(LPSTR)"Arial");
    vdws.hfntBtnText = CreateFont((-6)*GetDeviceCaps(hdcScreen,LOGPIXELSY)/72,0,
            0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
                VARIABLE_PITCH|FF_SWISS,NULL);
    vdws.hfntBtnStar = CreateFont((-18)*GetDeviceCaps(hdcScreen,LOGPIXELSY)/72,0,0,0,
            FW_BOLD,FALSE,FALSE,FALSE,
        SYMBOL_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
                VARIABLE_PITCH|FF_DONTCARE,(LPSTR)"Symbol");
    ReleaseDC(GetDesktopWindow(),hdcScreen);

    if (vdws.hfntBtn)
        {
        int did;

        for (did = didDialerBtnPadFirst; did <= didDialerBtnPadLast; ++did)
            {
            if ((did == didDialerBtnPadStar) || (did == didDialerBtnPadPound))
                SendMessage(GetDlgItem(vdws.hwnd,did),WM_SETFONT,
                        (WPARAM)vdws.hfntBtnStar,0L);
            else
                SendMessage(GetDlgItem(vdws.hwnd,did),WM_SETFONT,
                        (WPARAM)vdws.hfntBtn,0L);
            } /* for */
        } /* if */

    ShowWindow(vdws.hwnd,ncmdshow);
    UpdateWindow(vdws.hwnd);

    /* Limit text in Number field to TAPIMAXDESTADDRESSSIZE */
    SendDlgItemMessage(vdws.hwnd,didDialerComboNumDial,CB_LIMITTEXT,
            (WPARAM)TAPIMAXDESTADDRESSSIZE,0);

    /* Initialize tapi, Negotiate TAPI versions for all line devices.Get
       lineDevCaps for all lines, and init address info. */
    if ((errCode = ErrStartTapi(vdws.hInst,vdws.szDialerName)) != errNone)
        {/* bail out */
        DialerFatalExit(errCode);
        goto LOOM;
        } /* if */

    ErrInitCallingCardInfo(vdws.hInst);

    /* register ourselves to be simple TAPI recipient */
    if (!FRegisterSimpleTapi(TRUE))
        DialerErrMessageBox(ikszWarningRegisterSTapi,MB_ICONEXCLAMATION | MB_OK);

    /* register our clipboard format. go on if we fail */
    LoadString(vdws.hInst,ikszDialerClipbrdFormatName,szClipbrdFormat,
            sizeof(szClipbrdFormat));
    if (szClipbrdFormat[0])
        vdws.cfDialer = RegisterClipboardFormat(szClipbrdFormat);

    SetCursor(LoadCursor(NULL,IDC_ARROW));

    /* get the call logging options */
    vdws.fLogIncomingCalls = WGetDialerProfileInt(vdws.hInst,ikszSecCallLogging,
            ikszFieldCLIncoming,TRUE);
    vdws.fLogOutgoingCalls = WGetDialerProfileInt(vdws.hInst,ikszSecCallLogging,
            ikszFieldCLOutgoing,TRUE);

    /* show the call logging window according to the ini setting */
    if (WGetDialerProfileInt(vdws.hInst,ikszSecCallLogging,ikszFieldCLVisible,
            FALSE) != 0)
        {
        BOOL fCallLogNoShow = (ncmdshow == SW_SHOWMINIMIZED || ncmdshow == SW_SHOWMINNOACTIVE);

        ShowCallLogDlg(fCallLogNoShow);
        if (fCallLogNoShow)
            vdws.fCallLogVisible = TRUE;
        else
            BringWindowToTop(vdws.hwnd);
        } /* if */

    /* main message loop */
    while (GetMessage(&msg,NULL,0,0))
        {
        if (FDialerTranslateMessage(&msg)
            || vdws.fCallLogVisible && (GetActiveWindow() == vdws.hdlgCallLog
                && TranslateAccelerator(vdws.hdlgCallLog,vdws.haccel,&msg)
                || IsDialogMessage(vdws.hdlgCallLog,&msg))
            || vdws.hdlgCallStatus != NULL
                && IsWindowVisible(vdws.hdlgCallStatus)
                && IsDialogMessage(vdws.hdlgCallStatus,&msg)
            || TranslateAccelerator(vdws.hwnd,vdws.haccel,&msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        ProcessNextQueuedSTAPIRequest();
        } /* while */

LOOM:

    DialerCleanUp();
    return (int)msg.wParam;

} /* WinMain */

/* ***************************************************************************/
/* %%Function:DialPad */
/* called when the user presses the numeric key buttons. simulate typing the
    asciiCode into number. window proc will generate DTMF digit if we are
    already dialing (this triggers off EN_CHANGE notification from number. */

VOID DialPad(char chDigit)
{
    char szNumDial[64];
    WORD ichSelStart, ichSelEnd;
    HWND hwndNumDial = GetDlgItem(vdws.hwnd,didDialerComboNumDial);

    ichSelStart = LOWORD(vdws.dwSelPos);
    ichSelEnd = HIWORD(vdws.dwSelPos);

    GetWindowText(hwndNumDial,szNumDial,63);
    _fmemmove(szNumDial + ichSelStart + 1,szNumDial + ichSelEnd,
            strlen(szNumDial + ichSelEnd)+1);

    szNumDial[ichSelStart] = chDigit;
    SetWindowText(hwndNumDial,szNumDial);

    SetFocus(hwndNumDial);
    SendMessage(hwndNumDial,CB_SETEDITSEL,0,MAKELPARAM(ichSelStart+1,
            ichSelStart+1));
    vdws.dwSelPos = SendMessage(GetDlgItem(vdws.hwnd,didDialerComboNumDial),
            CB_GETEDITSEL,0,0L);
    EnableWindow(GetDlgItem(vdws.hwnd,didDialerBtnDial),TRUE);

} /* DialPad */

/* ***************************************************************************/
/* %%Function:replaceNumber */
/* replaces contents of number edit control with newNumber. */

VOID replaceNumber(char *newNumber)
{
    HWND numberHwnd = GetDlgItem(vdws.hwnd,didDialerComboNumDial);
    SetWindowText(GetDlgItem(vdws.hwnd,didDialerComboNumDial),newNumber);
    UpdateWindow(numberHwnd);

    EnableWindow(GetDlgItem(vdws.hwnd,didDialerBtnDial),TRUE);

} /* replaceNumber */


/* ***************************************************************************/
/* %%Function:getLast */
/* Returns selected redial number */

VOID getLast(char *redialNumber)
{
    DWORD lResult;

    HWND numberHwnd = GetDlgItem(vdws.hwnd,didDialerComboNumDial);
    lResult = SendMessage(numberHwnd,CB_GETLBTEXT,
            (WPARAM)SendMessage(numberHwnd,CB_GETCURSEL,0,0),
            (LPARAM)((LPSTR)redialNumber));
    if (lResult == CB_ERR)
        redialNumber[0] = 0;

} /* getLast */


/* ***************************************************************************/
/* %%Function:addToLast */
/* Adds number to redial list.  if list is full, moves list down 1 and
        replaces first item. */

VOID addToLast(LPCSTR newNumber)
{
#define cnumRedial      20          /* maximum number of redial number we care
                                       to remember */
    WORD clistItems;

    HWND numberHwnd = GetDlgItem(vdws.hwnd,didDialerComboNumDial);

    clistItems = (WORD)SendMessage(numberHwnd,CB_GETCOUNT, 0,0);
    if (clistItems != 0)
        {
        DWORD dwSearch = SendMessage(numberHwnd,CB_FINDSTRING,0,
                (LONG)(LPCSTR)newNumber);
        if (dwSearch != CB_ERR) /* remove duplicate */
            SendMessage(numberHwnd,CB_DELETESTRING,LOWORD(dwSearch),0);
        else if (clistItems == cnumRedial)
            SendMessage(numberHwnd,CB_DELETESTRING,cnumRedial-1,0);
        } /* else */

    SendMessage(numberHwnd,CB_INSERTSTRING,0,(LPARAM)((LPSTR)newNumber));
    SendMessage(numberHwnd,CB_SETCURSEL,0,0L);
    UpdateWindow(numberHwnd);

    EnableWindow(GetDlgItem(vdws.hwnd,didDialerBtnDial),TRUE);

} /* addToLast */

/* ***************************************************************************/
/* %%Function:FDialerInitiateCall */
/* adds szNumber to the combo box and calls FInitiateCall to do the real work.
        */

BOOL FDialerInitiateCall(LPCSTR szNumber,LPCSTR szName,BOOL fIsSTapiMakeCall)

{
    addToLast(szNumber);
    SetFocus(GetDlgItem(vdws.hwnd,didDialerBtnDial));
    return FInitiateCall(szNumber,szName,fIsSTapiMakeCall);

} /* FDialerInitiateCall */

/* ***************************************************************************/
/* %%Function:HandleDialerWndCmds */
/* called to process the WM_COMMAND message for vdws.hwnd. This message can be
    sent for either menu command or action to one of our dialog controls. */

void HandleDialerWndCmds(WPARAM wParam,LPARAM lParam)

{
    char szName[TAPIMAXCALLEDPARTYSIZE];
    char szNumber[TAPIMAXDESTADDRESSSIZE];

    switch (wParam)
        {
        /* *******************************************************/
        /* File menu */
        /* exit application */
        case midFileExit:
            {
            DialerCleanUp();
            break;
            }

        /* *******************************************************/
        /* Edit menu */
        /* Cut,Copy,Paste,Delete for Number Edit Control */
        case midEditCut:
        case midEditCopy:
        case midEditPaste:
        case midEditDelete:
            {
            if (wParam == midEditPaste && vdws.cfDialer != 0
                && IsClipboardFormatAvailable(vdws.cfDialer))
                {
                PasteLogClipboardIntoEditCtrl(GetDlgItem(vdws.hwnd,
                        didDialerComboNumDial),FALSE);
                SendMessage(vdws.hwnd,WM_COMMAND,didDialerComboNumDial,
                        MAKELPARAM(GetDlgItem(vdws.hwnd,didDialerComboNumDial),
                        CBN_EDITCHANGE));
                } /* if */
            else
                SendDlgItemMessage(vdws.hwnd,didDialerComboNumDial,
                        WM_CUT + (wParam-midEditCut),0,0);
            break;
            }
        /* Calls up panel to edit Quick Dial information. */
        case midEditSpeedDialButtons:
            {
            DoDialog(ProgramSpeedDialDlgProc,dlgProgSD,vdws.hInst,vdws.hwnd,1);
            SetFocus(GetDlgItem(vdws.hwnd, didDialerBtnDial));
            break;
            }

        /* *******************************************************/
        /* Options menu */
        /* Brings up the options dialog for dialing */
        case midOptionsDialing:
            {
            DoDialog(DialingOptionDlgProc,dlgDialingOption,vdws.hInst,vdws.hwnd,
                    0);
            break;
            }
        /* Brings up the options dialog for call logging */
        case midOptionsLog:
            {
            DoDialog(CallLogOptionDlgProc,dlgLogOption,vdws.hInst,vdws.hwnd,0);
            break;
            }
        /* Brings up the options dialog for locations or calling cards */
        case midSetupLocation:
            {
            if (FNumberDialable() && GetDlgItemText(vdws.hwnd,
                didDialerComboNumDial,(LPSTR)szNumber,sizeof(szNumber)))
            {
                ExpandNumberForDialHelper(szNumber,sizeof(szNumber));
                if (szNumber[0] != 0)
                    {
                        CallConfigDlg(vdws.hwnd,szNumber);
                        break;
                    }
            }
            CallConfigDlg(vdws.hwnd,NULL);
            break;
            }

        /* *******************************************************/
        /* Log menu */
        /* Show Call Log Menu item */
        case midOptionsViewLog:
            {
            if (!vdws.fCallLogVisible)
                ShowCallLogDlg(FALSE);
            else
                SendMessage(vdws.hdlgCallLog,WM_CLOSE,0,0);
            break;
            }

        /* *******************************************************/
        /* Help menu */
        /* Help */
        case midHelpContents:
            {
            WinHelp(vdws.hwnd,"dialer.hlp",HELP_FINDER,0);
            break;
            }

        case midHelpWhatThis:
            {
            PostMessage(vdws.hwnd, WM_SYSCOMMAND, SC_CONTEXTHELP, 0);
            break;
            }

        /* About Dialer Dialog */
        case midHelpAbout:
            {
            DoDialog(AboutDialerDlgProc,dlgAbout,vdws.hInst,vdws.hwnd,0);
            break;
            }
        /* does the accelerator action processing */
        case midAccelSelectNumToDial:
            {
            if (GetActiveWindow() == vdws.hwnd)
                SetFocus(GetDlgItem(vdws.hwnd,didDialerComboNumDial));
            break;
            }

        /* Phone number box. On change, if we are calling already, we generate
           DTMF for new digits entered. */
        case didDialerComboNumDial:
            {
            if (GetFocus() == GetDlgItem(vdws.hwnd,didDialerComboNumDial))
                vdws.dwSelPos = SendMessage(GetDlgItem(vdws.hwnd,
                        didDialerComboNumDial),CB_GETEDITSEL,0,0L);

            if (HIWORD(lParam) == CBN_SELENDOK && SendDlgItemMessage(vdws.hwnd,
                    didDialerComboNumDial,CB_GETCOUNT,0,0) > 0)
                {
                getLast(szNumber);
                if (szNumber[0] != 0)
                    replaceNumber(szNumber);
                else
                    {
                    GetDlgItemText(vdws.hwnd,didDialerComboNumDial,szNumber,
                            sizeof(szNumber));
                    addToLast(szNumber);
                    } /* else */
                } /* if */

            if (HIWORD(lParam) == CBN_EDITCHANGE
                || HIWORD(lParam) == CBN_SELENDOK)
                EnableWindow(GetDlgItem(vdws.hwnd,didDialerBtnDial),
                        GetWindowTextLength(GetDlgItem(vdws.hwnd,
                        didDialerComboNumDial)) > 0);

            break;
            }
        /* Dial/Hangup button. If currently dialing, button says hangup, line
           choice popup will also be disabled (along with most other
           controls). */
        case didDialerBtnDial:
            {
            if (FCallInProgress())
                {/* hangup */
                SetFocus(GetDlgItem(vdws.hwnd,didDialerComboNumDial));
                FDropCurrentCall();
                } /* if */
            else if (FNumberDialable() && GetDlgItemText(vdws.hwnd,
                    didDialerComboNumDial,(LPSTR)szNumber,sizeof(szNumber)))
                {
                char szName[cchSzMax], szNum[cchSzMax];

                /* scan the call log for a name that corresponds to szNumber */
                if (!FGetNameOfNumberFromCallLog(szNumber,szName))
                    {/* search all the speed dial buttons for szNumber */
                    UINT isd;

                    for (isd = 0;
                        isd <= didDialerBtnSpeedDialLast -
                                didDialerBtnSpeedDialFirst;
                        ++isd)
                        {
                        GetSetINIQuickDial(vdws.hInst,isd,szName,szNum,TRUE);
                        if (lstrcmp(szNum,szNumber) == 0)
                            break;
                        } /* for */

                    if (isd > didDialerBtnSpeedDialLast -
                            didDialerBtnSpeedDialFirst)
                        LoadString(vdws.hInst,ikszCallStatusNameUnknown,szName,32);
                    } /* if */

                FDialerInitiateCall(szNumber,szName,FALSE);
                } /* else if */
            break;
            }
        /* dial pad emulation */
        case didDialerBtnPad1:
        case didDialerBtnPad2:
        case didDialerBtnPad3:
        case didDialerBtnPad4:
        case didDialerBtnPad5:
        case didDialerBtnPad6:
        case didDialerBtnPad7:
        case didDialerBtnPad8:
        case didDialerBtnPad9:
        case didDialerBtnPad0:
            {
            DialPad((char)('0' + (1+wParam-didDialerBtnPad1)%10));
            break;
            }
        case didDialerBtnPadStar:
        case didDialerBtnPadPound:
            {
            DialPad((char)((wParam == didDialerBtnPadStar) ? '*' : '#'));
            break;
            }
        /* These are the preset buttons - dial the number hit if preset has
           setting. */
        case didDialerBtnSpeedDial1:
        case didDialerBtnSpeedDial2:
        case didDialerBtnSpeedDial3:
        case didDialerBtnSpeedDial4:
        case didDialerBtnSpeedDial5:
        case didDialerBtnSpeedDial6:
        case didDialerBtnSpeedDial7:
        case didDialerBtnSpeedDial8:
            {
            GetSetINIQuickDial(vdws.hInst,wParam-didDialerBtnSpeedDial1,szName,
                    szNumber,TRUE);
            if (!FIsPresetEmpty(szNumber))
                FDialerInitiateCall(szNumber,szName,FALSE);
            else
                DoDialog(ProgramSpeedDialButtonDlgProc,dlgProgSDB,vdws.hInst,
                        vdws.hwnd,MAKELPARAM(wParam,0));
            break;
            }
        } /* switch */

} /* HandleDialerWndCmds */

/* ***************************************************************************/
/* %%Function:DrawButton */
/* draws a 3D button within rcBtn on hdc. */

void DrawButton(HDC hdc,RECT rcBtn,BOOL fDown);
void DrawButton(HDC hdc,RECT rcBtn,BOOL fDown)
{
    HPEN        hpenPrev, hpenHilight, hpenShadow, hpenBlack;
    HBRUSH      hbrPrev, hbrFace;
    int         ropPrev;

    --rcBtn.right;
    --rcBtn.bottom;

        hpenBlack = GetStockObject(BLACK_PEN);
        hpenShadow = CreatePen(PS_SOLID,0,GetSysColor(COLOR_3DSHADOW));
        hpenHilight = CreatePen(PS_SOLID,0,GetSysColor(COLOR_3DHILIGHT));
        hbrFace = GetSysColorBrush(COLOR_3DFACE);

    hpenPrev = SelectObject(hdc,hpenBlack);
    ropPrev  = SetROP2(hdc,R2_COPYPEN);
    hbrPrev  = SelectObject(hdc,hbrFace);

    PatBlt(hdc,rcBtn.left+1,rcBtn.top+1,rcBtn.right-rcBtn.left-1,
            rcBtn.bottom-rcBtn.top-1,PATCOPY);

    if (fDown)
        {
        SelectObject(hdc,hpenBlack);
        MoveTo(hdc,rcBtn.left,rcBtn.bottom-1);
        LineTo(hdc,rcBtn.left,rcBtn.top);
        LineTo(hdc,rcBtn.right,rcBtn.top);

        SelectObject(hdc,hpenHilight);
        MoveTo(hdc,rcBtn.right,rcBtn.top);
        LineTo(hdc,rcBtn.right,rcBtn.bottom);
        LineTo(hdc,rcBtn.left-1,rcBtn.bottom);

        SelectObject(hdc,hpenShadow);
        MoveTo(hdc,rcBtn.left+1,rcBtn.bottom-2);
        LineTo(hdc,rcBtn.left+1,rcBtn.top+1);
        LineTo(hdc,rcBtn.right-1,rcBtn.top+1);
        } /* if */
    else
        { /* up state */
        /* Draw Edges */
        SelectObject(hdc,hpenHilight);
        MoveTo(hdc,rcBtn.left,rcBtn.bottom-1);
        LineTo(hdc,rcBtn.left,rcBtn.top);
        LineTo(hdc,rcBtn.right,rcBtn.top);

        SelectObject(hdc,hpenBlack);
        MoveTo(hdc,rcBtn.right,rcBtn.top);
        LineTo(hdc,rcBtn.right,rcBtn.bottom);
        LineTo(hdc,rcBtn.left-1,rcBtn.bottom);

        SelectObject(hdc,hpenShadow);
        MoveTo(hdc,rcBtn.left+1,rcBtn.bottom-1);
        LineTo(hdc,rcBtn.right-1,rcBtn.bottom-1);
        LineTo(hdc,rcBtn.right-1,rcBtn.top);

        } /* else */

    SetROP2(hdc,ropPrev);
    SelectObject(hdc,hbrPrev);
    SelectObject(hdc,hpenPrev);

        DeleteObject(hpenBlack);
        DeleteObject(hpenHilight);
        DeleteObject(hpenShadow);
//      DeleteObject(hbrFace);

} /* DrawButton */

/* ***************************************************************************/
/* %%Function:DialerGetTextWidth */
/* returns the width of the string specified by sz and cch. */

int DialerGetTextWidth(HDC hdc,LPCSTR sz,int cch);
int DialerGetTextWidth(HDC hdc,LPCSTR sz,int cch)

{
    return (LOWORD(GetTextExtent(hdc,sz,cch)));
} /* DialerGetTextWidth */

/* ***************************************************************************/
/* %%Function:DrawButtonText */
/* draws the text associated with didBtn. This whole mess is necessary since
    Windows standard buttons do not support multiple lines of text in them.

    expects the text associated with the button contains a single
    '\n'(chr(10)) character which breaks the text to be drawn into two lines.
    */

void DrawButtonText(HDC hdc,RECT rcBtn,UINT didBtn);
void DrawButtonText(HDC hdc,RECT rcBtn,UINT didBtn)

{
    BOOL fDown = ((WORD)SendDlgItemMessage(vdws.hwnd,didBtn,BM_GETSTATE,0,0))
        & 0x0004;
    BOOL fSingleLineText;
    int bkModePrev;
    char szText[cchSzMax];
    char szDelimiter[2] = {10,0};
    WORD ichDelimiter;
    TEXTMETRIC tm;
    RECT rcText;

    rcText = rcBtn;

    bkModePrev = SetBkMode(hdc,TRANSPARENT);

    GetDlgItemText(vdws.hwnd,didBtn,szText,sizeof(szText));
    ichDelimiter = _fstrcspn(szText,szDelimiter);
    fSingleLineText = (ichDelimiter == strlen(szText));

    /* draws the first line */
    if (ichDelimiter > 0)
        {
        HFONT hfntPrev = NULL;

        if (vdws.hfntBtnText && !fSingleLineText)
            hfntPrev = SelectObject(hdc,vdws.hfntBtnText);
        GetTextMetrics(hdc,&tm);
        rcText.bottom = ((rcBtn.bottom + rcBtn.top)/2) - 2;
        rcText.top = rcText.bottom - (tm.tmHeight - 1);
        if (fSingleLineText) OffsetRect(&rcText,0,(rcText.bottom - rcText.top)/2);
        if (fDown) OffsetRect(&rcText,1,1);
        DrawText(hdc,szText,ichDelimiter,&rcText,DT_SINGLELINE | DT_CENTER);

        if (hfntPrev)
            SelectObject(hdc,hfntPrev);
        } /* if */

    if(!fSingleLineText)
        {/* draws the second line */
        GetTextMetrics(hdc,&tm);
        if ((didBtn == didDialerBtnPadStar) || (didBtn == didDialerBtnPadPound))
            rcText.top = ((rcBtn.bottom + rcBtn.top)/2)-((tm.tmHeight)/2);
        else
            rcText.top = ((rcBtn.bottom + rcBtn.top)/2) - 2;
        rcText.bottom = rcText.top + tm.tmHeight;
        if (fDown) OffsetRect(&rcText,1,1);
        DrawText(hdc,szText+ichDelimiter+1,-1,&rcText,DT_SINGLELINE | DT_CENTER);
        } /* if */

    SetBkMode(hdc,bkModePrev);

} /* DrawButtonText */

/* ***************************************************************************/
/* %%Function:DialerWndProc */
/* Window proc for vdws.hwnd. */

LRESULT FAR PASCAL _export DialerWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,
                LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        didDialerSTextDial, IDH_DIALER_DIAL_NUMBER,
        didDialerComboNumDial, IDH_DIALER_DIAL_NUMBER,
        didDialerBtnDial, IDH_DIALER_DIAL_BUTTON,
        didDialerBtnSpeedDial1, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial2, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial3, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial4, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial5, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial6, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial7, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedDial8, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText1, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText2, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText3, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText4, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText5, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText6, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText7, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnSpeedText8, IDH_DIALER_DIAL_SPEED_CHOOSE,
        didDialerBtnPad1, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad2, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad3, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad4, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad5, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad6, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad7, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad8, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad9, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPadStar, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPad0, IDH_DIALER_DIAL_KEYPAD,
        didDialerBtnPadPound, IDH_DIALER_DIAL_KEYPAD,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        /* Set WS_EX_APPWINDOW to force onto task bar */
        case WM_CREATE:
            {
            SetWindowLong(hwnd,GWL_EXSTYLE,(GetWindowLong(hwnd,GWL_EXSTYLE)|WS_EX_APPWINDOW));
            break;
            }

        /* Handle menu commands and actions to dialog controls */
        case WM_COMMAND:
            {
            HandleDialerWndCmds(wParam,lParam);
            break;
            }
        /* Enable/Disable Menu items */
        case WM_INITMENUPOPUP:
            {
            HMENU mEdit;
            UINT wEnable;

            if (LOWORD(lParam) == 2) /* Options menu */
                {
                char szMenuText[cchSzMax];

                LoadString(vdws.hInst,
                    vdws.fCallLogVisible ?
                        ikszOptionsMenuHideLog : ikszOptionsMenuShowLog,
                    szMenuText,sizeof(szMenuText));
                ModifyMenu((HMENU)wParam,midOptionsViewLog,MF_STRING
                    | MF_BYCOMMAND,midOptionsViewLog,szMenuText);
                break;
                } /* else if */
            else if (LOWORD(lParam) != 1) /* not Edit menu */
                break;

            mEdit = (HMENU)wParam;
            EnableMenuItem(mEdit,midEditPaste,
                    IsClipboardFormatAvailable(CF_TEXT)? MF_ENABLED:MF_GRAYED);
            if (GetParent(GetFocus()) != GetDlgItem(vdws.hwnd,
                    didDialerComboNumDial))
                {
                wEnable = MF_GRAYED;
                EnableMenuItem(mEdit,midEditPaste,MF_GRAYED);
                } /* if */
            else
                {
                LONG lSelect = SendDlgItemMessage(vdws.hwnd,
                        didDialerComboNumDial,CB_GETEDITSEL,0,0);
                wEnable =
                    HIWORD(lSelect) != LOWORD(lSelect) ? MF_ENABLED:MF_GRAYED;
                } /* else */
            EnableMenuItem(mEdit,midEditCut,wEnable);
            EnableMenuItem(mEdit,midEditCopy,wEnable);
            EnableMenuItem(mEdit,midEditDelete,wEnable);

            break;
            }
        case WM_DRAWITEM:
            {
            LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

            DrawButton(lpdis->hDC,lpdis->rcItem,
                    ((WORD)SendDlgItemMessage(vdws.hwnd,lpdis->CtlID,
                    BM_GETSTATE,0,0)) & 0x0004);
            DrawButtonText(lpdis->hDC,lpdis->rcItem,lpdis->CtlID);
            break;
            }
        case WM_SIZE:
            {
            if (wParam == SIZE_MINIMIZED)
                {
                if (vdws.fCallLogVisible)
                    ShowWindow(vdws.hdlgCallLog,SW_HIDE);
                } /* if */
            else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
                {
                if (vdws.fCallLogVisible)
                    ShowWindow(vdws.hdlgCallLog,SW_SHOW);
                } /* else if */
            break;
            }
        /* Reflect changes in International date/time formating */
        case WM_WININICHANGE:
            {
            if (lParam == 0 || lstrcmp((LPSTR)lParam,"Intl"))
                LoadIntlFormats();
            break;
            }

                case WM_SYSCOLORCHANGE:
                        {
                        InvalidateRect(vdws.hwnd,NULL,TRUE);
                        UpdateWindow(vdws.hwnd);
                        }

        /* redraw Dialer window */
        /* Dialer being activated */
        case WM_ACTIVATE:
            {
            if (wParam != WA_INACTIVE && !HIWORD(lParam))
                SetFocus(GetDlgItem(hwnd,didDialerComboNumDial));
            break;
            }
        case WM_ACTIVATEAPP:
            {
            HWND hwndDlg;

            if (!wParam)
                break;

            hwndDlg = GetLastActivePopup(vdws.hwndDummy);
            BringWindowToTop(hwndDlg);
            if (hwndDlg == vdws.hwnd && !HIWORD(lParam))
                SetFocus(GetDlgItem(hwnd,didDialerComboNumDial));
            break;
            }
        case WM_LBUTTONDOWN:
        case WM_MOVE:
            {/* hide the number to dial drop-down list box */
            SendDlgItemMessage(vdws.hwnd,didDialerComboNumDial,CB_SHOWDROPDOWN,
                    (WPARAM)FALSE,0);
            break;
            }
        /* quitting application */
        case WM_DESTROY:
            {
            RECT    rc;

            /* terminate outstanding calls */
            TerminateCalls(vdws.hInst);

            /* save screen location */
            if (!IsIconic(hwnd))
                {
                GetWindowRect(hwnd,&rc);
                GetSetINIScreenPos(vdws.hInst,(POINT *)&rc,FALSE);
                } /* if */
            SaveLastDialedNumbers(vdws.hInst,vdws.hwnd);

            /* do our part to unload help */
            WinHelp(hwnd,"dialer.hlp", HELP_QUIT, 0);
            PostQuitMessage(wParam);

            vdws.hwnd = NULL;
            break;
            }
        default:
            return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } /* switch */

    return FALSE;

} /* DialerWndProc */

/* ***************************************************************************/
/* %%Function:DoubleUpAmpersandsInSz */
/* looks at each character in sz and doubles up each '&' we run into.
    cchSzBuffer contains the TOTAL available size of the buffer used to
    contain sz. */

void DoubleUpAmpersandsInSz(char *sz,int cchSzBuffer)

{
    int cchSz = strlen(sz) + 1;
    int cchFree = cchSzBuffer - cchSz;
    int cchAmpersand = 0;
    int ich = -1;

    while (++ich < cchSz)
        {
        if (sz[ich] != '&')
            continue;
        if (cchAmpersand >= cchFree)
            break;

        _fmemmove(sz + ich + 1,sz + ich,cchSz - ich);
        ++cchSz;
        ++ich;
        ++cchAmpersand;
        } /* while */

} /* DoubleUpAmpersandsInSz */

/* ***************************************************************************/
/* %%Function:SingleDownAmpersandsInSz */
/* looks at each character in sz and converts each TWO '&'s into a single '&'.
        */

void SingleDownAmpersandsInSz(char *sz)

{
    int cchSz = strlen(sz) + 1;
    int ich = -1;

    while (++ich < cchSz)
        {
        if (sz[ich] != '&' || sz[ich+1] != '&')
            continue;

        _fmemmove(sz + ich,sz + ich + 1,cchSz - ich - 1);
        --cchSz;
        } /* while */

} /* SingleDownAmpersandsInSz */

/* ***************************************************************************/
/* %%Function:NameNumEditCtrlProc */
/* sub-classed window-proc for edit controls that are used to contain name or
    phone number. This is necessary to implementing the smart paste when the
    clipboard contains the our call log entry. We trap SHIFT+INSERT/CTRL+V and
    do the right thing. */

LRESULT FAR PASCAL _export NameNumEditCtrlProc(HWND hwndCtrl,UINT uMsg,
            WPARAM wParam,LPARAM lParam)

{
    int did;

    if (vdws.cfDialer == 0 || !IsClipboardFormatAvailable(vdws.cfDialer)
        || !(uMsg == WM_KEYDOWN && wParam == VK_INSERT
            && GetKeyState(VK_SHIFT) < 0)
        && !(uMsg == WM_KEYUP && (wParam == 'V' || wParam == 'v')
            && GetKeyState(VK_CONTROL) < 0))
        return CallWindowProc(vdws.lpfnDefEditProc,hwndCtrl,uMsg,wParam,lParam);


    did = GetDlgCtrlID(hwndCtrl);
    PasteLogClipboardIntoEditCtrl(hwndCtrl,did == didProgSDEditName
        || did == didProgSDBEditName || did == didCallStatusEditBoxLogName);
    return TRUE;

} /* NameNumEditCtrlProc */

/* ***************************************************************************/
/* %%Function:ProgramSpeedDialDlgProc */
/* Dialog Proc for the "Program Speed Dial Buttons..." dialog. */

BOOL FAR PASCAL _export ProgramSpeedDialDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam)
{
    static WORD didBtnCurS;

// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        IDOK, IDH_DIALER_SPEED_SAVE,
        didProgSDBtnSpeedDial1, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial2, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial3, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial4, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial5, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial6, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial7, IDH_DIALER_SPEED_CHOOSE,
        didProgSDBtnSpeedDial8, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText1, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText2, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText3, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText4, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText5, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText6, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText7, IDH_DIALER_SPEED_CHOOSE,
                didProgSDBtnSpeedText8, IDH_DIALER_SPEED_CHOOSE,
        didProgSDSTextName, IDH_DIALER_SPEED_NAME,
        didProgSDEditName, IDH_DIALER_SPEED_NAME,
        didProgSDSTextNumber, IDH_DIALER_SPEED_NUMBER,
        didProgSDEditNumber, IDH_DIALER_SPEED_NUMBER,
        didProgSDSTextTop, IDH_DIALER_SPEED_CHOOSE,
        didProgSDSTextBottom, IDH_DIALER_SPEED_NAME,
        0, 0
    };


    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            char szName[TAPIMAXCALLEDPARTYSIZE];
            char szNumber[TAPIMAXDESTADDRESSSIZE];
            UINT did, didBtnEmptyFirst = (WORD)-1;

            CenterDlg(hDlg);

            /* get names from INI file */
            for (did = didProgSDBtnSpeedDialFirst;
                     did <= didProgSDBtnSpeedDialLast; ++did)
                {
                GetSetINIQuickDial(vdws.hInst,
                        (WORD)(did-didProgSDBtnSpeedDialFirst),szName,szNumber,
                        TRUE);
                /* remember first empty preset */
                if (didBtnEmptyFirst == ((WORD)-1) && FIsPresetEmpty(szName))
                    didBtnEmptyFirst = did;

                /* init the button with szName and set its font */
                DoubleUpAmpersandsInSz(szName,(int)TAPIMAXCALLEDPARTYSIZE);
                SetDlgItemText(hDlg,did,(LPCSTR)szName);
                SetDlgItemText(hDlg,
                    didProgSDSTextSpeedDialFirst
                        + did - didProgSDBtnSpeedDialFirst,
                    (LPCSTR)szNumber);
                } /* for */

            /* sub-class the two edit controls for smart paste */
            if (vdws.lpfnSmartPasteEditProc != NULL)
                {
                SetWindowLong(GetDlgItem(hDlg,didProgSDEditName),GWL_WNDPROC,
                        (LONG)vdws.lpfnSmartPasteEditProc);
                SetWindowLong(GetDlgItem(hDlg,didProgSDEditNumber),GWL_WNDPROC,
                        (LONG)vdws.lpfnSmartPasteEditProc);
                } /* if */

            /* Limit text in Name field to TAPIMAXCALLEDPARTYSIZE */
            SendDlgItemMessage(hDlg,didProgSDEditName,EM_LIMITTEXT,
                    (WPARAM)(TAPIMAXCALLEDPARTYSIZE - 1),0);
            /* Limit text in Number field to TAPIMAXDESTADDRESSSIZE */
            SendDlgItemMessage(hDlg,didProgSDEditNumber,EM_LIMITTEXT,
                    (WPARAM)(TAPIMAXDESTADDRESSSIZE - 1),0);

            /* select the first empty name  */
            didBtnEmptyFirst =
                (didBtnEmptyFirst == ((WORD)-1) ?
                    didProgSDBtnSpeedDialFirst : didBtnEmptyFirst);
            SendMessage(hDlg,WM_COMMAND,didBtnEmptyFirst,
                    MAKELPARAM(GetDlgItem(hDlg,didBtnEmptyFirst),BN_CLICKED));
            return FALSE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            char szName[TAPIMAXCALLEDPARTYSIZE];
            char szNumber[TAPIMAXDESTADDRESSSIZE];

            switch (wParam)
                {
                case IDOK:
                    {
                    UINT did;

                    for (did = didProgSDBtnSpeedDialFirst;
                             did <= didProgSDBtnSpeedDialLast; ++did)
                        {
                        GetDlgItemText(hDlg,did,szName,sizeof(szName));
                        SingleDownAmpersandsInSz(szName);
                        GetDlgItemText(hDlg,
                                didProgSDSTextSpeedDialFirst
                                    + did - didProgSDBtnSpeedDialFirst,
                                szNumber,
                                sizeof(szNumber));

                        /* save screen settings to INI file */
                        GetSetINIQuickDial(vdws.hInst,
                                did-didProgSDBtnSpeedDialFirst,szName,szNumber,
                                FALSE);

                        /* update the corresponding button in the main window
                           */
                        GetSetINIQuickDial(vdws.hInst,
                                did-didProgSDBtnSpeedDialFirst,szName,szNumber,
                                TRUE);
                        DoubleUpAmpersandsInSz(szName,
                                (int)TAPIMAXCALLEDPARTYSIZE);
                        SetDlgItemText(vdws.hwnd,
                                didDialerBtnSpeedDial1
                                    + did - didProgSDBtnSpeedDialFirst,
                                (LPCSTR)szName);
                        } /* for */

                    /* fall through is intentional */
                    }
                case IDCANCEL:
                    {
                    if (vdws.lpfnSmartPasteEditProc)
                        {
                        SetWindowLong(GetDlgItem(hDlg,didProgSDEditName),
                                GWL_WNDPROC,(LONG)vdws.lpfnDefEditProc);
                        SetWindowLong(GetDlgItem(hDlg,didProgSDEditNumber),
                                GWL_WNDPROC,(LONG)vdws.lpfnDefEditProc);
                        } /* if */
                    EndDialog(hDlg,wParam == IDOK);
                    return TRUE;
                    break;
                    }
                case didProgSDBtnSpeedDial1:
                case didProgSDBtnSpeedDial2:
                case didProgSDBtnSpeedDial3:
                case didProgSDBtnSpeedDial4:
                case didProgSDBtnSpeedDial5:
                case didProgSDBtnSpeedDial6:
                case didProgSDBtnSpeedDial7:
                case didProgSDBtnSpeedDial8:
                    {
                    /* save away the current selection */
                    didBtnCurS = wParam;

                    /* update the two edit boxes to reflect the new selection
                       */
                    GetDlgItemText(hDlg,didBtnCurS,szName,sizeof(szName));
                    SingleDownAmpersandsInSz(szName);
                    GetDlgItemText(hDlg,
                            didProgSDSTextSpeedDialFirst
                                + didBtnCurS - didProgSDBtnSpeedDialFirst,
                            szNumber,
                            sizeof(szNumber));
                    SetDlgItemText(hDlg,didProgSDEditName,(LPCSTR)szName);
                    SetDlgItemText(hDlg,didProgSDEditNumber,(LPCSTR)szNumber);
                    SetFocus(GetDlgItem(hDlg,didProgSDEditName));
                    SendDlgItemMessage(hDlg,didProgSDEditName,EM_SETSEL,0,
                            MAKELPARAM(0,-1));
                    break;
                    }
                case didProgSDEditName:
                    {
                    if (HIWORD(lParam) != EN_CHANGE)
                        break;

                    GetDlgItemText(hDlg,didProgSDEditName,szName,
                            sizeof(szName));
                    DoubleUpAmpersandsInSz(szName,(int)TAPIMAXCALLEDPARTYSIZE);
                    SetDlgItemText(hDlg,didBtnCurS,(LPCSTR)szName);
                    break;
                    }
                case didProgSDEditNumber:
                    {
                    if (HIWORD(lParam) != EN_CHANGE)
                        break;

                    GetDlgItemText(hDlg,didProgSDEditNumber,szNumber,
                            sizeof(szNumber));
                    SetDlgItemText(hDlg,
                            didProgSDSTextSpeedDialFirst
                                + didBtnCurS - didProgSDBtnSpeedDialFirst,
                                (LPCSTR)szNumber);
                    break;
                    }
                } /* switch wParam */
            break;
            }
        } /* switch */

    return FALSE;

} /* ProgramSpeedDialDlgProc */

/* ***************************************************************************/
/* %%Function:ProgramSpeedDialButtonDlgProc */
/* Dialog Proc for the dialog we put up when the user clicks on an empty speed
    dial button or when he/she right clicks on a non-empty speed dial button.
    */

BOOL FAR PASCAL _export ProgramSpeedDialButtonDlgProc(HWND hDlg,UINT uMsg,
            WPARAM wParam,LPARAM lParam)
{
    static WORD didDialerBtnSDClickedS = (WORD)-1;    /* the button the user
                                                         clicked to program
                                                         its speed dial
                                                         setting */

// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        IDOK, IDH_DIALER_SPEED_SAVE,
        didProgSDBBtnSaveDial, IDH_DIALER_SPEED_SAVE_DIAL,
        didProgSDBSTextName, IDH_DIALER_SPEED_NAME,
        didProgSDBEditName, IDH_DIALER_SPEED_NAME,
        didProgSDBSTextNumber, IDH_DIALER_SPEED_NUMBER,
        didProgSDBEditNumber, IDH_DIALER_SPEED_NUMBER,
        0, 0
    };


    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            char szName[TAPIMAXCALLEDPARTYSIZE];
            char szNumber[TAPIMAXDESTADDRESSSIZE];

            CenterDlg(hDlg);
            didDialerBtnSDClickedS = LOWORD(lParam);

            /* get names from INI file */
            GetSetINIQuickDial(vdws.hInst,
                    didDialerBtnSDClickedS-didDialerBtnSpeedDialFirst,szName,
                    szNumber,TRUE);

            /* init the edit boxes with szName and szNumber and set their font
               */
            SetDlgItemText(hDlg,didProgSDBEditName,(LPCSTR)szName);
            SetDlgItemText(hDlg,didProgSDBEditNumber,(LPCSTR)szNumber);

            /* sub-class the two edit controls for smart paste */
            if (vdws.lpfnSmartPasteEditProc != NULL)
                {
                SetWindowLong(GetDlgItem(hDlg,didProgSDBEditName),GWL_WNDPROC,
                        (LONG)vdws.lpfnSmartPasteEditProc);
                SetWindowLong(GetDlgItem(hDlg,didProgSDBEditNumber),GWL_WNDPROC,
                        (LONG)vdws.lpfnSmartPasteEditProc);
                } /* if */

            /* Limit text in Name field to TAPIMAXCALLEDPARTYSIZE */
            SendDlgItemMessage(hDlg,didProgSDBEditName,EM_LIMITTEXT,
                    (WPARAM)(TAPIMAXCALLEDPARTYSIZE - 1),0);
            /* Limit text in Number field to TAPIMAXDESTADDRESSSIZE */
            SendDlgItemMessage(hDlg,didProgSDBEditNumber,EM_LIMITTEXT,
                    (WPARAM)(TAPIMAXDESTADDRESSSIZE - 1),0);

            SetFocus(GetDlgItem(hDlg,didProgSDBEditName));
            SendDlgItemMessage(hDlg,didProgSDBEditName,EM_SETSEL,0,MAKELPARAM(0,
                    -1));
            return FALSE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            char szName[TAPIMAXCALLEDPARTYSIZE];
            char szNumber[TAPIMAXDESTADDRESSSIZE];

            switch (wParam)
                {
                case IDOK:
                case didProgSDBBtnSaveDial:
                    {
                    /* get the editted settings */
                    GetDlgItemText(hDlg,didProgSDBEditName,szName,
                            sizeof(szName));
                    GetDlgItemText(hDlg,didProgSDBEditNumber,szNumber,
                            sizeof(szNumber));

                    /* save screen settings to INI file */
                    GetSetINIQuickDial(vdws.hInst,
                            didDialerBtnSDClickedS-didDialerBtnSpeedDialFirst,
                            szName,szNumber,FALSE);

                    /* update the corresponding button in the main window */
                    GetSetINIQuickDial(vdws.hInst,
                            didDialerBtnSDClickedS-didDialerBtnSpeedDialFirst,
                            szName,szNumber,TRUE);
                    DoubleUpAmpersandsInSz(szName,(int)TAPIMAXCALLEDPARTYSIZE);
                    SetDlgItemText(vdws.hwnd,didDialerBtnSDClickedS,
                            (LPCSTR)szName);

                    /* execute the updated speed dial button action */
                    if (wParam == didProgSDBBtnSaveDial)
                        PostMessage(vdws.hwnd,WM_COMMAND,didDialerBtnSDClickedS,
                                MAKELPARAM(GetDlgItem(vdws.hwnd,
                                didDialerBtnSDClickedS),BN_CLICKED));

                    /* fall through is intentional */
                    }
                case IDCANCEL:
                    {
                    if (vdws.lpfnSmartPasteEditProc)
                        {
                        SetWindowLong(GetDlgItem(hDlg,didProgSDBEditName),
                                GWL_WNDPROC,(LONG)vdws.lpfnDefEditProc);
                        SetWindowLong(GetDlgItem(hDlg,didProgSDBEditNumber),
                                GWL_WNDPROC,(LONG)vdws.lpfnDefEditProc);
                        } /* if */
                    EndDialog(hDlg,wParam == IDOK);
                    return TRUE;
                    break;
                    }
                case didProgSDBEditName:
                case didProgSDBEditNumber:
                    {
                    if (HIWORD(lParam) != EN_CHANGE)
                        EnableWindow(GetDlgItem(hDlg,didProgSDBBtnSaveDial),
                                GetWindowTextLength(GetDlgItem(hDlg,
                                didProgSDBEditNumber)) > 0);
                    break;
                    }
                } /* switch wParam */
            break;
            }
        } /* switch */

    return FALSE;

} /* ProgramSpeedDialButtonDlgProc */

/* ***************************************************************************/
/* %%Function:DialingOptionDlgProc */
/* Dialog Proc for "Options..." menus command in "Phone" menu. */

BOOL FAR PASCAL _export DialingOptionDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
                    LPARAM lParam)
{

// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        didDialingOptionSTextLine, IDH_DIALER_OPTIONS_LINE,
        didDialingOptionLBoxLine, IDH_DIALER_OPTIONS_LINE,
        didDialingOptionSTextAddress, IDH_DIALER_OPTIONS_ADDRESS,
        didDialingOptionLBoxAddress, IDH_DIALER_OPTIONS_ADDRESS,
        didDialingOptionChkBoxSTAPIVoice, IDH_DIALER_OPTIONS_VOICE,
                didDialingOptionBtnProperties, IDH_DIALER_OPTIONS_PROPERTIES,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            BOOL fEnable;
            DWORD dwPriority;

            CenterDlg(hDlg);

            EnableWindow(GetDlgItem(hDlg,didDialingOptionLBoxLine),
                    fEnable = FInitLineLBox(vdws.hInst, hDlg));
            EnableWindow(GetDlgItem(hDlg,didDialingOptionLBoxAddress),
                    fEnable = fEnable && FInitAddressLBox(vdws.hInst,hDlg));
            EnableWindow(GetDlgItem(hDlg,IDOK),fEnable);
                        EnableWindow(GetDlgItem(hDlg,didDialingOptionBtnProperties),fEnable);

            lstrcat(vdws.szDialerName,".exe");
            lineGetAppPriority(vdws.szDialerName,0,NULL,LINEREQUESTMODE_MAKECALL,NULL,&dwPriority);
            CheckDlgButton(hDlg,didDialingOptionChkBoxSTAPIVoice,dwPriority == 1);
            EnableWindow(GetDlgItem(hDlg,didDialingOptionChkBoxSTAPIVoice),TRUE);

            vdws.szDialerName[lstrlen(vdws.szDialerName) - 4] = 0;

            return FALSE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            switch (wParam)
                {
                case didDialingOptionLBoxLine:
                    {
                    if (HIWORD(lParam) != CBN_SELENDOK)
                        break;

                    /* update the address list box */
                    FInitAddressLBox(vdws.hInst,hDlg);
                    break;
                    }
                case didDialingOptionBtnProperties:
                    {
                                        DWORD dwDeviceID;
                                        HWND hwndLineLBox = GetDlgItem(hDlg,didDialingOptionLBoxLine);

                                        dwDeviceID = (DWORD)SendMessage(hwndLineLBox,CB_GETITEMDATA,
                                                (WORD)SendMessage(hwndLineLBox,CB_GETCURSEL,0,0),0);
                                        lineConfigDialog(dwDeviceID,hDlg,NULL);
                                        break;
                    }
                case IDOK:
                    {
                    UpdateDialingOptionSettings(vdws.hInst,hDlg,vdws.szDialerName);
                    /* fall through is intentional */
                    }
                case IDCANCEL:
                    {
                    EndDialog(hDlg,wParam == IDOK);
                    return TRUE;
                    }
                } /* switch */
            }
        } /* switch */

    return FALSE;

} /* DialingOptionDlgProc */


/* ***************************************************************************/
/* %%Function:CallLogOptionDlgProc */
/* Dialog Proc for "Call Log Options..." menus command in "Log" menu. */

BOOL FAR PASCAL _export CallLogOptionDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        didLogOptionGrpBox, IDH_DIALER_LOG_IN,
        didLogOptionChkBoxIncoming, IDH_DIALER_LOG_IN,
        didLogOptionChkBoxOutgoing, IDH_DIALER_LOG_OUT,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            CenterDlg(hDlg);
            CheckDlgButton(hDlg,didLogOptionChkBoxIncoming,
                    vdws.fLogIncomingCalls);
            CheckDlgButton(hDlg,didLogOptionChkBoxOutgoing,
                    vdws.fLogOutgoingCalls);

            return FALSE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            if (wParam != IDOK && wParam != IDCANCEL)
                break;

            if (wParam == IDOK)
                {
                vdws.fLogIncomingCalls = (SendDlgItemMessage(hDlg,
                        didLogOptionChkBoxIncoming,BM_GETCHECK,0,0L) != 0);
                SetDialerProfileInt(vdws.hInst,ikszSecCallLogging,
                        ikszFieldCLIncoming,vdws.fLogIncomingCalls);

                vdws.fLogOutgoingCalls = (SendDlgItemMessage(hDlg,
                        didLogOptionChkBoxOutgoing,BM_GETCHECK,0,0L) != 0);
                SetDialerProfileInt(vdws.hInst,ikszSecCallLogging,
                        ikszFieldCLOutgoing,vdws.fLogOutgoingCalls);
                } /* if */

            EndDialog(hDlg,wParam == IDOK);
            return TRUE;
            }
        } /* switch */

    return FALSE;

} /* CallLogOptionDlgProc */


/* ***************************************************************************/
/* %%Function:AboutDialerDlgProc */
/* dialog proc for "About Dialer..." in "Help" menu. */

BOOL FAR PASCAL _export AboutDialerDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{

    switch (uMsg)
        {
        case WM_INITDIALOG:
            {
            WORD w;
            char sz[256];
            char szlabel[256];

            CenterDlg(hDlg);

            /* sets up the version number for Windows */
            w = LOWORD(GetVersion());
            GetDlgItemText(hDlg,didAboutSTextTitle,sz,sizeof(sz));
            wsprintf(szlabel,sz,w & 0xFF,HIBYTE(w) == 10 ? 1 : 0);
            SetDlgItemText(hDlg,didAboutSTextTitle,szlabel);

            /* sets up version number for Dialer */
            GetDlgItemText(hDlg,didAboutSTextVersion,sz,sizeof(sz));
            wsprintf(szlabel,sz,VER_MAJOR,VER_MINOR,VER_BUILD);

#ifndef DEBUG
    {
        // strip off build number for release copies

        int  i   = 0;
        char *ch = szlabel;

        for (; i < 2 && *ch; ch++)
        {
            if (*ch == '.') ++i;
            if (i == 2)      *ch = 0;
        }
    }
#endif
            SetDlgItemText(hDlg,didAboutSTextVersion,szlabel);

            /* set windows mode information */
            {
            DWORD dw = GetWinFlags();
            if (dw & WF_ENHANCED)
                w = ikszAboutModeEnhanced;
            else if (dw & WF_STANDARD)
                w = ikszAboutModeStandard;
            else if (dw & WF_WLO)
                w = ikszAboutModeWLO;
            else
                w = ikszAboutModeUndef;
            }
            LoadString(vdws.hInst,w,sz,sizeof(sz));
            SetDlgItemText(hDlg,didAboutSTextWinMode,sz);

            /* get free memory information */
            GetDlgItemText(hDlg,didAboutSTextFreeMem,sz,sizeof(sz));
            wsprintf(szlabel,sz,GetFreeSpace(0)>>10);
            SetDlgItemText(hDlg,didAboutSTextFreeMem,szlabel);

            /* get free resources information */
            w = GetFreeSystemResources(0);
            GetDlgItemText(hDlg,didAboutSTextResource,sz,sizeof(sz));
            wsprintf(szlabel,sz,w);
            SetDlgItemText(hDlg,didAboutSTextResource,szlabel);

            return TRUE;
            }
        case WM_COMMAND:
            {
            if (wParam != IDOK && wParam != IDCANCEL)
                break;

            EndDialog(hDlg,TRUE);
            return TRUE;
            }
        } /* switch */

    return FALSE;

} /* AboutDialerDlgProc */

/* ***************************************************************************/
/* %%Function:LineInUseDlgProc */
/* dialog proc for "Line In Use" dialog. */

BOOL FAR PASCAL _export LineInUseDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{
    static BOOL fIsLineInUseDlg = FALSE;

    switch (uMsg)
        {
        case WM_INITDIALOG:
            {
            int iksz;
            char sz[cchSzMax];

            CenterDlg(hDlg);
            if (fIsLineInUseDlg = (lParam == 0))
                return TRUE;

            switch (lParam)
                {
                case LINEDISCONNECTMODE_REJECT:
                    {
                    iksz = ikszDisconnectedReject;
                    break;
                    }
                case LINEDISCONNECTMODE_BUSY:
                    {
                    iksz = ikszDisconnectedBusy;
                    break;
                    }
                case LINEDISCONNECTMODE_NOANSWER:
                    {
                    iksz = ikszDisconnectedNoAnswer;
                    break;
                    }
                case LINEDISCONNECTMODE_CONGESTION:
                    {
                    iksz = ikszDisconnectedNetwork;
                    break;
                    }
                case LINEDISCONNECTMODE_INCOMPATIBLE:
                    {
                    iksz = ikszDisconnectedIncompatible;
                    break;
                    }
                case LINEDISCONNECTMODE_NODIALTONE:
                    {
                    iksz = ikszDisconnectedNoDialTone;
                    break;
                    }
                default:
                    {
                    iksz = ikszDisconnectedCantDo;
                    break;
                    }
                } /* switch */
            LoadString(vdws.hInst,iksz,sz,sizeof(sz));
            SetDlgItemText(hDlg,didDisconnectedErrorSTextErr,sz);
            return TRUE;
            }
        case WM_COMMAND:
            {
            if (wParam != IDOK && wParam != IDCANCEL)
                break;

            EndDialog(hDlg,TRUE);
            return TRUE;
            }
        } /* switch */

    return FALSE;

} /* LineInUseDlgProc */

/* ***************************************************************************/
/* %%Function:DisplayLineInUseDlg */
/* shows the modal dialog "Line In Use" to inform the user that the line is
    busy. called after lineMakeCall fails. */

void FAR DisplayLineInUseDlg()

{
    vdws.fErrBoxOn = TRUE;
    DoDialog(LineInUseDlgProc,dlgLineInUse,vdws.hInst,GetActiveWindow(),0);
    vdws.fErrBoxOn = FALSE;

} /* DisplayLineInUseDlg */


/* ***************************************************************************/
/* %%Function:DisPlayDisconnectedErrorDlg */
/* shows the modal dialog "Call Failed" to inform the user the reason the call
    is disconnected. */

void FAR DisPlayDisconnectedErrorDlg(DWORD errCode)

{
    vdws.fErrBoxOn = TRUE;
    DoDialog(LineInUseDlgProc,dlgDisconnectedError,vdws.hInst,vdws.hwnd,
            errCode);
    vdws.fErrBoxOn = FALSE;

} /* DisPlayDisconnectedErrorDlg */

/* ***************************************************************************/
/* %%Function:DialingPromptDlgProc */
/* dialog proc for "Dialing Prompt" dialog. */

BOOL FAR PASCAL _export DialingPromptDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        IDOK, IDH_DIALER_PAUSE_CONTINUE,
        IDCANCEL, IDH_DIALER_HANGUP,
        didDialingPromptBtnOption, IDH_DIALER_CHANGE_OPTIONS,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            int iksz;
            char szText[cchSzMax];

            CenterDlg(hDlg);

            switch (lParam)
                {
                case LINEERR_DIALBILLING:
                    {
                    iksz = ikszDialingPromptSTextBilling;
                    break;
                    }
                case LINEERR_DIALDIALTONE:
                    {
                    iksz = ikszDialingPromptSTextTone;
                    break;
                    }
                case LINEERR_DIALQUIET:
                    {
                    iksz = ikszDialingPromptSTextQuite;
                    break;
                    }
                default:
                    {
                    iksz = ikszDialingPromptSTextPrompt;
                    break;
                    }
                } /* switch */
            LoadString(vdws.hInst,iksz,szText,sizeof(szText));
            SetDlgItemText(hDlg,didDialingPromptSText,szText);
            vdws.hdlgDialingPrompt = hDlg;
            return TRUE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            vdws.hdlgDialingPrompt = NULL;
            if (wParam == didDialingPromptBtnOption || wParam == IDCANCEL)
                {
                int did = (wParam == didDialingPromptBtnOption) ?
                    didCallStatusBtnOption : IDCANCEL;

                PostMessage(vdws.hdlgCallStatus,WM_COMMAND,did,
                        MAKELPARAM(GetDlgItem(vdws.hdlgCallStatus,did),
                        BN_CLICKED));
                EndDialog(hDlg, TRUE);
                } /* if */
                else
                    EndDialog(hDlg, FALSE);
            break;
            }
        } /* switch */

    return FALSE;

} /* DialingPromptDlgProc */

/* ***************************************************************************/
/* %%Function:DisplayDialingPromptDlg */
/* shows the modal dialog "Dialing Prompt" to prompt the user to continue the
    dialing process when appropriate. dwLineErrCode is one of the following 4
    values : LINEERR_DIALBILLING, LINEERR_DIALDIALTONE, LINEERR_DIALQUIET, and
    LINEERR_DIALPROMPT. */

int FAR DisplayDialingPromptDlg(DWORD dwLineErrCode)

{
    return (DoDialog(DialingPromptDlgProc,dlgDialingPrompt,vdws.hInst,
            vdws.hdlgCallStatus,dwLineErrCode));

} /* DisplayDialingPromptDlg */


/* ***************************************************************************/
/* %%Function:ChangeOptionDlgProc */
/* dialog proc for "Change Options and Redial" dialog. */

BOOL FAR PASCAL _export ChangeOptionDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        IDOK, IDH_DIALER_CHANGE_REDIAL_BUTTON,
        didChangeOptionSTextNumber, IDH_DIALER_CHANGE_DIGITS,
        didChangeOptionEditBoxNumber, IDH_DIALER_CHANGE_DIGITS,
        didChangeOptionBtnDialHelper, IDH_DIALER_CHANGE_DIAL_HELPER,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_INITDIALOG:
            {
            char szNumber[cchSzMax];
            DWORD dwTranslateResults;

            CenterDlg(hDlg);

            GetCurCallTranslatedNumber(szNumber,sizeof(szNumber),
                    &dwTranslateResults,FALSE);

            SendDlgItemMessage(hDlg,didChangeOptionEditBoxNumber,EM_LIMITTEXT,
                    (WPARAM)(TAPIMAXDESTADDRESSSIZE - 1),0);
            SetDlgItemText(hDlg,didChangeOptionEditBoxNumber,szNumber);
            SetFocus(GetDlgItem(hDlg,didChangeOptionEditBoxNumber));
            SendDlgItemMessage(hDlg,didChangeOptionEditBoxNumber,EM_SETSEL,0,
                    MAKELPARAM(0,-1));
            return FALSE;
            }
        /* action processing */
        case WM_COMMAND:
            {
            char szNumber[cchSzMax];
            DWORD dwTranslateResults;

            switch (wParam)
                {
                case didChangeOptionBtnDialHelper:
                    {
                    GetCurCallCanonicalNumber(szNumber,sizeof(szNumber));
                    CallConfigDlg(hDlg,szNumber);
                    GetCurCallTranslatedNumber(szNumber,sizeof(szNumber),
                            &dwTranslateResults,TRUE);

                    SetDlgItemText(hDlg,didChangeOptionEditBoxNumber,szNumber);
                    SetFocus(GetDlgItem(hDlg,didChangeOptionEditBoxNumber));
                    SendDlgItemMessage(hDlg,didChangeOptionEditBoxNumber,
                            EM_SETSEL,0,MAKELPARAM(0,-1));
                    break;
                    }
                case IDOK:
                    {
                    char szName[cchSzMax];
                    char szDialString[cchSzMax];

                    GetDlgItemText(hDlg,didChangeOptionEditBoxNumber,szNumber,
                            sizeof(szNumber));
                    GetCurCallName(szName,sizeof(szName));

                    GetCurCallTranslatedNumber(szDialString,
                            sizeof(szDialString),&dwTranslateResults,FALSE);
                    if (strcmp(szNumber,szDialString) != 0)
                        SetCurCallDisplayableString(szNumber);

                    ShowWindow(hDlg,SW_HIDE);
                    EndDialog(hDlg,TRUE);
                    vdws.fDisplayChangeOptionDlg = FALSE;

                    FInitiateCall(szNumber,szName,FCurCallIsSTapiMakeCall());
                    return TRUE;
                    }
                case IDCANCEL:
                    {
                    EndDialog(hDlg,FALSE);
                    vdws.fDisplayChangeOptionDlg = FALSE;
                    return TRUE;
                    }
                } /* switch */
            }
        } /* switch */

    return FALSE;

} /* ChangeOptionDlgProc */


/* ***************************************************************************/
/* %%Function:CallStatusProc */
/* dialog proc for the call status dialog. */

BOOL FAR PASCAL _export CallStatusProc(HWND hDlg,UINT uMsg,WPARAM wParam,
            LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        IDCANCEL, IDH_DIALER_HANGUP,
        didCallStatusSTextLogName, IDH_DIALER_DIALING_LOGNAME,
        didCallStatusEditBoxLogName, IDH_DIALER_DIALING_LOGNAME,
        didCallStatusBtnOption, IDH_DIALER_CHANGE_OPTIONS,
        didCallStatusSTextCallingCard, IDH_DIALER_DIALING_STATUS,
        didCallStatusSTextNameNum, IDH_DIALER_DIALING_STATUS,
        didCallStatusSTextLocation, IDH_DIALER_DIALING_STATUS,
        didCallStatusSTextTranslatedNum, IDH_DIALER_DIALING_STATUS,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_DESTROY:
            {
            vdws.hdlgCallStatus = NULL;
            return TRUE;
            }
        case WM_CLOSE:
            {
            SendMessage(hDlg,WM_COMMAND,IDCANCEL,MAKELPARAM(GetDlgItem(hDlg,
                    IDCANCEL),BN_CLICKED));
            return TRUE;
            }
        case WM_COMMAND:
            {
            char szName[cchSzMax];

            if (wParam == didCallStatusEditBoxLogName)
                break;

                        // update the name entered

            GetDlgItemText(hDlg,didCallStatusEditBoxLogName,szName,sizeof(szName));
            SetCurCallName(szName);

            ShowWindow(hDlg,SW_HIDE);
            EnableWindow(vdws.hwnd,TRUE);
            if (vdws.fCallLogVisible)
                EnableWindow(vdws.hdlgCallLog,TRUE);
            BringWindowToTop(vdws.hwnd);

            if (FCallInProgress()) /* drop the call */
                SendMessage(vdws.hwnd,WM_COMMAND,didDialerBtnDial,
                        MAKELPARAM(GetDlgItem(vdws.hwnd,didDialerBtnDial),
                        BN_CLICKED));
            if (wParam == didCallStatusBtnOption)
                /* can't put up the dialog right now since lineDrop completes
                   assync and the LINECALLSTATE_IDLE processing calls
                   DisableDialerDesktop(FALSE) to enable all the controls in
                   our main window. This will cause Windows to get seriously
                   confused and make our Dialog modeless */
                vdws.fDisplayChangeOptionDlg = TRUE;
            return TRUE;
            }
        } /* switch */

    return FALSE;

} /* CallStatusProc */

/* ***************************************************************************/
/* %%Function:FCreateCallStatusDlg */
/* create the dialing dialog and initialize vhdlgDialing. */

BOOL FCreateCallStatusDlg()

{
    DLGPROC dlgproc;

    dlgproc = (DLGPROC)MakeProcInstance((FARPROC)CallStatusProc,vdws.hInst);
    vdws.hdlgCallStatus = CreateDialog(vdws.hInst,
            MAKEINTRESOURCE(dlgCallStatus),vdws.hwnd,dlgproc);
    if (vdws.hdlgCallStatus == NULL)
        {
        FreeProcInstance((FARPROC)dlgproc);
        return FALSE;
        } /* if */

    if (vdws.lpfnSmartPasteEditProc)
        {
        vdws.lpfnDefEditProc = (FARPROC)GetWindowLong(
                GetDlgItem(vdws.hdlgCallStatus,didCallStatusEditBoxLogName),
                GWL_WNDPROC);
        SetWindowLong(GetDlgItem(vdws.hdlgCallStatus,
                didCallStatusEditBoxLogName),GWL_WNDPROC,
                (LONG)vdws.lpfnSmartPasteEditProc);
        } /* if */

    /* Limit text in Name field to TAPIMAXCALLEDPARTYSIZE */
    SendDlgItemMessage(vdws.hdlgCallStatus,didCallStatusEditBoxLogName,
            EM_LIMITTEXT,(WPARAM)(TAPIMAXCALLEDPARTYSIZE - 1),0);

    return TRUE;

} /* FCreateCallStatusDlg */

/* ***************************************************************************/
/* %%Function:StrReplace */
/* upon entry, szOut contains szTok. upon exit, szOut's szTok portion is
        replaced with szCpntent. The maximum number of characters that can be
        used is cchContentMax. if szContent is longer than cchContentMax, it
        is trancated and "..." is added at the end. */

void StrReplace(LPSTR szTok,LPCSTR szContent,LPSTR szOut,
                int cchContentMax)

{
    LPSTR lpsz;
    char szBuffer[cchSzMax];
    int ich, cchContent;

    _fmemmove(szBuffer,szOut,lstrlen(szOut) + 1);

    /* figure out where the token starts in szOut */
    lpsz = _fstrstr(szOut,szTok);
    ich = lpsz - szOut;

    /* copy szContent to where szTok is in szOut */
    cchContent = lstrlen(szContent);
    _fmemmove(szOut + ich,szContent,min(cchContent,cchContentMax));
    if (cchContent > cchContentMax)
        {
        cchContent = cchContentMax;
        szOut[ich + cchContent - 1] = '.';
        szOut[ich + cchContent - 2] = '.';
        szOut[ich + cchContent - 3] = '.';
        } /* if */

    _fmemmove(szOut + ich + cchContent,szBuffer + ich + lstrlen(szTok),
            strlen(szBuffer) - ich - lstrlen(szTok) + 1);

} /* StrReplace */

/* ***************************************************************************/
/* %%Function:StrReplaceInSTextItem */
/* same as StrReplace except we don't want the string to be displayed in
        hwndCtrl is more than one line long. calls StrReplace to do the real
        work. */

void StrReplaceInSTextItem(LPSTR szTok,LPCSTR szContent,LPSTR szOut,
                HWND hwndCtrl)

{
    int cchAvail;
    TEXTMETRIC tm;
    HDC  hdcCtrl;
    RECT rcText;

    /* figure out the maximum number of chars we can display */
    hdcCtrl = GetDC(hwndCtrl);
    GetTextMetrics(hdcCtrl,&tm);
    ReleaseDC(hwndCtrl,hdcCtrl);

    GetClientRect(hwndCtrl,&rcText);
    cchAvail = min((rcText.right - rcText.left)/tm.tmAveCharWidth - 2,
            cchSzMax-1);

    cchAvail -= (lstrlen(szOut) - lstrlen(szTok));
    StrReplace(szTok,szContent,szOut,cchAvail);

} /* StrReplaceInSTextItem */

/* ***************************************************************************/
/* %%Function:SetUpDialingText */
/* sets up the text in didCallStatusSTextNameNum in the call status dialog. */

void SetUpDialingText(int ikszFormat,LPCSTR szName,LPCSTR szNum,
            LPSTR szText,int cchSzText)

{
    HWND hwndCtrl = GetDlgItem(vdws.hdlgCallStatus,didCallStatusSTextNameNum);
    int cchAvail, cchName, cchNum, cchText;
    TEXTMETRIC tm;
    HDC  hdcCtrl;
    RECT rcText;

    if (szName[0] == 0)
        LoadString(vdws.hInst,ikszCallStatusNameUnknown,(LPSTR)szName,32);

    /* figure out the maximum number of chars we can display */
    hdcCtrl = GetDC(hwndCtrl);
    GetTextMetrics(hdcCtrl,&tm);
    ReleaseDC(hwndCtrl,hdcCtrl);

    GetClientRect(hwndCtrl,&rcText);
    cchAvail = min((rcText.right - rcText.left)/tm.tmAveCharWidth - 2,
            cchSzText-1);

    /* load in the strings we use */
    LoadString(vdws.hInst,ikszFormat,szText,cchAvail);

    cchName = lstrlen(szName);
    cchNum = lstrlen(szNum);
    cchText = lstrlen(szText) - 6;      /* 6 is for %sn and %sa */

    /* if not enough space, use minimum number of chars for the name */
    if (cchText + cchName + cchNum > cchAvail)
        {
        cchName = max(5,
                cchAvail - (cchText + cchNum));
        cchNum = cchAvail - (cchText + cchName);
        } /* if */

    /* replace the szNameFormat portion in szText with the actual name szName
       */
    StrReplace(szNameFormat,szName,szText,cchName);

    /* replace the szNumFormat portion in szFormat with the actual address
       szNum */
    StrReplace(szNumFormat,szNum,szText,cchNum);

} /* SetUpDialingText */

/* ***************************************************************************/
/* %%Function:ShowCallStatusDlg */
/* create the dialing dialog and initialize vhdlgDialing. */

void ShowCallStatusDlg(LPCSTR szName,LPCSTR szNumber,LPCSTR szLocation,
            LPCSTR szCallingCard,LPCSTR szTranslatedNumber)

{
    char szText[cchSzMax];

    /* set up the dialog title */
    LoadString(vdws.hInst,ikszCallStatusDialing,szText,sizeof(szText));
    SetWindowText(vdws.hdlgCallStatus,szText);

    /* set up the name/number text */
    SetUpDialingText(ikszCallStatusNameFormatDialing,szName,szNumber,szText,
            sizeof(szText));
    SetDlgItemText(vdws.hdlgCallStatus,didCallStatusSTextNameNum,szText);

    /* set up location text */
    LoadString(vdws.hInst,ikszCallStatusLocationFormat,szText,sizeof(szText));
    StrReplaceInSTextItem(szLocationFormat,szLocation,szText,
            GetDlgItem(vdws.hdlgCallStatus,didCallStatusSTextLocation));
    SetDlgItemText(vdws.hdlgCallStatus,didCallStatusSTextLocation,szText);

    /* set up calling card text */
    LoadString(vdws.hInst,ikszCallStatusCallingCardFormat,szText,
            sizeof(szText));
    StrReplaceInSTextItem(szCallingCardFormat,szCallingCard,szText,
            GetDlgItem(vdws.hdlgCallStatus,didCallStatusSTextCallingCard));
    SetDlgItemText(vdws.hdlgCallStatus,didCallStatusSTextCallingCard,szText);

    /* set up translated number */
    LoadString(vdws.hInst,ikszCallStatusTranslatedNumberFormat,szText,
            sizeof(szText));
    StrReplaceInSTextItem(szNumFormat,szTranslatedNumber,szText,
            GetDlgItem(vdws.hdlgCallStatus,didCallStatusSTextTranslatedNum));
    SetDlgItemText(vdws.hdlgCallStatus,didCallStatusSTextTranslatedNum,szText);

    EnableWindow(GetDlgItem(vdws.hdlgCallStatus,didCallStatusBtnOption),TRUE);

    /* show the dialog */
    EnableWindow(vdws.hwnd,FALSE);
    if (vdws.fCallLogVisible)
        EnableWindow(vdws.hdlgCallLog,FALSE);
    CenterDlg(vdws.hdlgCallStatus);
    ShowWindow(vdws.hdlgCallStatus,SW_SHOW);

    /* set up the text in the edit box */
    SetFocus(GetDlgItem(vdws.hdlgCallStatus,didCallStatusEditBoxLogName));
    if (szName)
        {
        SetDlgItemText(vdws.hdlgCallStatus,didCallStatusEditBoxLogName,
                szName);
        SendDlgItemMessage(vdws.hdlgCallStatus,didCallStatusEditBoxLogName,
                EM_SETSEL,0,MAKELPARAM(0,-1));
        } /* if */

    UpdateWindow(vdws.hdlgCallStatus);

} /* ShowCallStatusDlg */

/* ***************************************************************************/
/* %%Function:UpdateCallStatusDlg */
/* update the call status dialog display to reflect the current call state.
        fCallIsActive == TRUE iff the call has just become active. Otherwise,
        the remote end has just disconnected. */

void UpdateCallStatusDlg(BOOL fCallIsActive,LPCSTR szName,LPCSTR szNumber)

{
    char szText[cchSzMax];

    if (!IsWindowVisible(vdws.hdlgCallStatus))
        return;

    /* set up the dialog title */
    LoadString(vdws.hInst,
            fCallIsActive
                ? ikszCallStatusConntected
                : ikszCallStatusDisconnected,
            szText,
            sizeof(szText));
    SetWindowText(vdws.hdlgCallStatus,szText);

    /* set up the name/number text */
    SetUpDialingText(fCallIsActive
        ? ikszCallStatusNameFormatConnected
        : ikszCallStatusNameFormatDisconnected,
        szName,szNumber,szText,sizeof(szText));
    SetDlgItemText(vdws.hdlgCallStatus,didCallStatusSTextNameNum,szText);

    /* disable the "Change Options" button */
    EnableWindow(GetDlgItem(vdws.hdlgCallStatus,didCallStatusBtnOption),!fCallIsActive);

    LoadString(vdws.hInst,
            fCallIsActive ? ikszCallStatusBtnHangup : ikszCallStatusBtnOK,
            szText,sizeof(szText));
    SetDlgItemText(vdws.hdlgCallStatus,IDOK,szText);

} /* UpdateCallStatusDlg */

/* ***************************************************************************/
/* %%Function:HideCallStatusDlg */
/* called when the call status dialog is visible. it hides it by issuing the
    "Cancel" button command. */

void HideCallStatusDlg()

{
    if (vdws.hdlgDialingPrompt)
        SendMessage(vdws.hdlgDialingPrompt,WM_COMMAND,IDCANCEL,0);
    SendMessage(vdws.hdlgCallStatus,WM_COMMAND,IDCANCEL,
            MAKELPARAM(GetDlgItem(vdws.hdlgCallStatus,IDCANCEL),BN_CLICKED));

} /* HideCallStatusDlg */

/* ***************************************************************************/
/* %%Function:HideCallStatusDlg */
/* now is the time to do the "Change Option" dialog. we need to wait till BOTH
    LINECALLSTATE_IDLEs are processed. */

void DoChangeOptionDlg()

{
    if (vdws.fDisplayChangeOptionDlg)
        DoDialog(ChangeOptionDlgProc,dlgChangeOption,vdws.hInst,vdws.hwnd,0);

} /* DoChangeOptionDlg */

/* ***************************************************************************/
/* call logging related functions */
/* ***************************************************************************/
/* %%Function:CchFile */
/* Returns the length of hFile. */

long CchFile(int hFile)
{
    long lCurrentPos = _lseek(hFile,0L,SEEK_CUR);
    long lFileLength = _lseek(hFile,0L,SEEK_END);

    _lseek(hFile,lCurrentPos,SEEK_SET);
    return lFileLength;

} /* CchFile */

/* ***************************************************************************/
/* %%Function:CchFreeLogBytes */
/* Frees log entries to make room for trailing cchLogFileMax bytes of file.
    Any log entry whose data doesn't come from within cchLogFileMax bytes of
    lastByte is tossed. We return total number of bytes tossed.  if fResetFile
    is true, then all entries are tossed. */

LONG CchFreeLogBytes(HWND hwndList,LONG eofByte,BOOL fResetFile)
{
    LONG cchFreed = 0;
    DWORD ifc;
    DWORD lineLen;
    DWORD bound = (eofByte < cchLogFileMax) ? 0 : (eofByte - cchLogFileMax);

    if (bound != 0 || fResetFile)
        {
        LONG item;
        WORD nItems;

        nItems = (WORD) SendMessage(hwndList,LB_GETCOUNT,0,0);
        for (item = (LONG)nItems-1; item >= 0; item--)
            {
            lineLen = SendMessage(hwndList,LB_GETTEXTLEN,(WPARAM)item,0);
            if (fResetFile)
                cchFreed += lineLen + 2;
            else
                {
                ifc = SendMessage(hwndList,LB_GETITEMDATA,(WPARAM)item,0);
                if (ifc < bound) {
                    SendMessage(hwndList,LB_DELETESTRING,(WPARAM)item,0);
                    cchFreed += lineLen+2;
                }
            } /* for */
        }  /* if */

        if (fResetFile)
            SendMessage(hwndList,LB_RESETCONTENT,0,0);
        }

    return cchFreed;

} /* CchFreeLogBytes */

/* ***************************************************************************/
/* %%Function:FPrependLogLine */
/* Prepends a line to the list. returns TRUE if successful. */

BOOL FPrependLogLine(HWND hwndList,LPSTR szLine,LONG ifc,
            WORD iItemInsert)
{
    DWORD item;

    item = SendMessage(hwndList,LB_INSERTSTRING,iItemInsert,(LPARAM)szLine);
    if (item == LB_ERR)
        return FALSE;

    SendMessage(hwndList,LB_SETITEMDATA,(WPARAM)item,(LPARAM)ifc);
    return TRUE;

} /* FPrependLogLine */

/* ***************************************************************************/
/* %%Function:FcOpenCallLog */
/* Opens the call log file, either for read/share or write/exclusive,
        depending on fWrite. creates the log file if it does not exists. */

int FcOpenCallLog(BOOL fWrite)
{
    char szBuffer[256];
    int cchBuffer;
    int fcLogFile;
    int retry = 2;
    int openMode;

    GetWindowsDirectory(szBuffer,144);
    cchBuffer = strlen(szBuffer);
    if (szBuffer[cchBuffer-1] != '\\')
        {
        szBuffer[cchBuffer++] = '\\';
        szBuffer[cchBuffer] = '\0';
        }  /* if */

    LoadString(vdws.hInst,ikszCallLogFile,&szBuffer[cchBuffer],
            sizeof(szBuffer) - cchBuffer);
    while (retry--)
        {
        openMode = fWrite ? (OF_READWRITE | OF_SHARE_EXCLUSIVE) : (OF_READ
            | OF_SHARE_DENY_WRITE);
        fcLogFile = _lopen((LPSTR)szBuffer,openMode);
        if (fcLogFile == HFILE_ERROR && retry)
            {
            if (_access(szBuffer,0) < 0 && errno == ENOENT)
                {
                fcLogFile = _lcreat((LPSTR)szBuffer,0 /* NORMAL */);
                if (fcLogFile != HFILE_ERROR)
                    fcLogFile = _lclose(fcLogFile);
                } /* if */
            if (fcLogFile == HFILE_ERROR)
                retry = 0;
            }
        else
            retry = 0;
        } /* while */

    return fcLogFile;

} /* FcOpenCallLog */

/* ***************************************************************************/
/* %%Function:FReadCallLog */
/* Reads the call log (starting on line boundary) into list window in CallLog
    Dialog. if fResetFile is TRUE, the local cache is ignored and the file is
    read in. if fResetFile is FALSE there are two cases: A.  We've added to
    log so read the new piece at the end of file (possibly need to toss oldest
    rows to make room B.  We've removed some rows and adjusted the cache.
    Might need to read in some additional older rows.

    Need to do following. If file is longer than MAX_SHOWLOG, a message is put
    at the top of the edit window followed by MAX_SHOWLOG-length of the
    message-2 bytes (2 added for \n\r). */

BOOL FReadCallLog(HWND hwndList,BOOL fResetFile)
{
    BOOL fres = FALSE;
    char szBuffer[cchLogLineMax];
    LPSTR readBuff = NULL;
    int fcLogFile = HFILE_ERROR;
    LONG fLength;
    LONG toRead;
    UINT nRead;
    LONG readAt;
    LPSTR pc,psrc,pbnd;
    BOOL fInvalid = FALSE;
    int cchBuffer;
    LONG filePosition;
    BOOL readOldRows = FALSE;
    WORD cEntryTotal;
    WORD at = 0;

    /* open the log file */
    fcLogFile = FcOpenCallLog(FALSE);
    if (fcLogFile == HFILE_ERROR)
        goto LDone;

    /* Read the new (unread) portion of the file, starting from the end. Stop
       when cchLogFileMax bytes are in list or when we've run out of new data.
       Toss oldest rows (those at end of list) if necessary to make room.  The
       intent here is to limit memory taken up by log and to avoid reading
       portions of the log that haven't changed. */
    if (fResetFile)
        vdws.logLengthPrev = 0;
    fLength = CchFile(fcLogFile);
    toRead = fLength - vdws.logLengthPrev;

    /* If we have current data in window up to current eof, check if we need
       to read some old rows that will now be vueable due to deletion of some
       rows via cut or delete operation. */
    if (toRead == 0)
        {
        cEntryTotal = (WORD) SendMessage(hwndList,LB_GETCOUNT,0,0);
        if (cEntryTotal == 0)
            filePosition = vdws.logLengthPrev;
        else
            filePosition = SendMessage(hwndList,LB_GETITEMDATA,
                    (WPARAM)(cEntryTotal-1),0);
        if (vdws.curLogData >= cchLogFileMax || filePosition == 0)
            goto exitSuccess;

        readOldRows = TRUE;
        toRead = cchLogFileMax - vdws.curLogData;
        if (toRead > filePosition)
            toRead = filePosition;
        readAt = filePosition - toRead;
        at = cEntryTotal;
        }  /* if */

    if (toRead > cchLogFileMax)
        toRead = cchLogFileMax;
    readBuff = (LPSTR)GlobalAllocPtr(GHND,toRead);
    if (readBuff == NULL)
        goto LDone;
    if (!readOldRows)
        readAt = fLength-toRead;
    if (_lseek(fcLogFile,readAt,SEEK_SET) < 0)
        goto LDone;
    nRead = _lread(fcLogFile,readBuff,(UINT)toRead);
    if (nRead != (UINT)toRead)
        goto LDone;

    fInvalid = TRUE;
    SendMessage(hwndList,WM_SETREDRAW,0,0);

    /* Free enough items to allow us to add vdws.curLogData bytes of new log */
    if (!readOldRows)
        vdws.curLogData -= CchFreeLogBytes(hwndList,nRead+readAt,fResetFile);
    pbnd = readBuff+toRead;
    pc = readBuff;

    /* Scan to a line start if we just read starting at random place in file */
    if (readAt != 0 && (readOldRows || readAt != vdws.logLengthPrev))
        {
        while (pc < pbnd && *pc != '\n' && *pc != '\r')
            pc++;
        while (pc < pbnd && (*pc == '\n' || *pc == '\r'))
            pc++;
        } /* for */

    while (pc < pbnd)
        {/* Find the next line and add it to list (truncate as required) */
        psrc = pc;
        while (pc < pbnd && *pc != '\n' && *pc != '\r')
            pc++;

        cchBuffer = pc-psrc;
        if (cchBuffer >= sizeof(szBuffer))
            cchBuffer = sizeof(szBuffer)-1;
        if (cchBuffer)
            {
            _fmemcpy(szBuffer,psrc,cchBuffer);
            szBuffer[cchBuffer] = '\0';
            filePosition = readAt+(LONG)(psrc-readBuff);
            if (!FPrependLogLine(hwndList,szBuffer,filePosition,at))
                goto LDone;
            vdws.curLogData += cchBuffer + 2;
            while (pc < pbnd && (*pc == '\n' || *pc == '\r'))
                pc++;
            } /* if */
        } /* while */

exitSuccess:
    if (!readOldRows)
        vdws.logLengthPrev = fLength;
    fres = TRUE;
    vdws.fCallLogDirty = FALSE;

LDone:
    if (fcLogFile != 0)
        _lclose(fcLogFile);
    if (readBuff)
        GlobalFreePtr(readBuff);
    if (fInvalid)
        {
        SendMessage(hwndList,WM_SETREDRAW,1,0);
        InvalidateRect(hwndList,NULL,TRUE);
        } /* if */
    return fres;

} /* FReadCallLog */

/* ***************************************************************************/
/* %%Function:GetSzNameNumFromSzLine */
/* szLine contains a log entry. it separates out the name and number portion
    from the log entry and put them into szName and szNum respectively. */

void GetSzNameNumFromSzLine(LPSTR szLine,LPSTR szName,LPSTR szNum)

{
    LPSTR lpchNameStart, lpchNumStart, lpchTimeStart;

    szName[0] = szNum[0] = 0;

    lpchNameStart = _fstrchr(szLine,'\t');
    if (lpchNameStart == NULL)
        return;
    ++lpchNameStart;

    lpchNumStart = _fstrchr(lpchNameStart,'\t');
    if (lpchNumStart == NULL)
        return;
    ++lpchNumStart;

    lpchTimeStart = _fstrchr(lpchNumStart,'\t');
    if (lpchTimeStart == NULL)
        return;
    ++lpchTimeStart;

    _fstrncpyDialer(szName,lpchNameStart,lpchNumStart - lpchNameStart,
            lstrlen(lpchNameStart));
    _fstrncpyDialer(szNum,lpchNumStart,lpchTimeStart - lpchNumStart,
            lstrlen(lpchNumStart));

} /* GetSzNameNumFromSzLine */

/* ***************************************************************************/
/* %%Function:FGetNameOfNumberFromCallLog */
/* given szNumber, searchs the call log to find the log entry that has the
    szNumber as it's number and returns in szName the name contained in the
    call log. empty string is returned if not found. */
BOOL FGetNameOfNumberFromCallLog(LPSTR szNumber,LPSTR szName)

{
    char szLine[cchSzMax],szNum[cchSzMax];
    int iLine, cLine;

    if (!vdws.hdlgCallLog || vdws.fCallLogDirty)
        ShowCallLogDlg(TRUE);

    cLine = LOWORD(SendDlgItemMessage(vdws.hdlgCallLog,didCallLogSTextLog,
            LB_GETCOUNT,0,0));
    for (iLine = 0; iLine < cLine; ++iLine)
        {
        SendDlgItemMessage(vdws.hdlgCallLog,didCallLogSTextLog,LB_GETTEXT,iLine,
                (LPARAM)((LPSTR)szLine));
        GetSzNameNumFromSzLine(szLine,szName,szNum);
        if (lstrcmp(szNum,szNumber) == 0)
            return TRUE;
        } /* for */

    szName[0] = 0;
    return FALSE;

} /* FGetNameOfNumberFromCallLog */

/* ***************************************************************************/
/* %%Function:ShowCallLogDlg */
/* Makes CallLog Non-Modal Dialog visible (creates it, iff necessary). insures
            that it is big enough to show whole log line. */

VOID ShowCallLogDlg(BOOL fDoNotShow)
{
    if (!vdws.hdlgCallLog)
        {
        DLGPROC dlgproc;

        dlgproc = (DLGPROC)MakeProcInstance((FARPROC)CallLogProc,vdws.hInst);
        vdws.hdlgCallLog = CreateDialog(vdws.hInst,MAKEINTRESOURCE(dlgCallLog),
                vdws.hwndDummy,dlgproc);
        if (!vdws.hdlgCallLog)
            {
            FreeProcInstance((FARPROC)dlgproc);
            /* should put up an error message */
            return;
            } /* if */

            SetCallLogTabs(vdws.hdlgCallLog);
        } /* if */

    if (!vdws.fCallLogVisible)
        {
        if (!FReadCallLog(GetDlgItem(vdws.hdlgCallLog,didCallLogSTextLog),
                TRUE))
            DialerErrMessageBox(ikszErrDefault,MB_ICONEXCLAMATION | MB_OK);
        if (!fDoNotShow)
            {
            ShowWindow(vdws.hdlgCallLog,SW_SHOW);
            vdws.fCallLogVisible = TRUE;
            } /* if */
        } /* if */

} /* ShowCallLogDlg */

void DrawItem(LPDRAWITEMSTRUCT pDI)
{
    COLORREF    crText, crBack;
    char        szBuf[cchLogLineMax];
    char            *str;
    int         i, left = 0;


    if((ODA_DRAWENTIRE | ODA_SELECT) & pDI->itemAction)
    {
        if(pDI->itemState & ODS_SELECTED)
        {
            // Select the appropriate text colors
            crText = SetTextColor(pDI->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
            crBack = SetBkColor(pDI->hDC, GetSysColor(COLOR_HIGHLIGHT));
        }

        // erase background
        ExtTextOut(pDI->hDC, 0, 0, ETO_OPAQUE, &pDI->rcItem, NULL, 0, NULL);

        // get the text
        SendMessage (pDI->hwndItem, LB_GETTEXT, pDI->itemID, (LPARAM) szBuf);


        str = strtok (szBuf, "\t"); // from

        for (i = 0; str && i < cLogFieldMax; i++, str = strtok (NULL, "\t"))
        {
            // trim off extra characters

            while ((int) (DialerGetTextWidth(pDI->hDC, str,
                     lstrlen(str))) > vdws.tabStops[i])
                strcpy (&str[strlen(str) - 4], "...");

            // draw the text.
            ExtTextOut(pDI->hDC, left, pDI->rcItem.top, ETO_CLIPPED,
                            &pDI->rcItem, str, lstrlen(str), NULL);

            left += vdws.tabStops[i];
        }

        // Restore original colors if we changed them above.
        if(pDI->itemState & ODS_SELECTED)
        {
            SetTextColor(pDI->hDC, crText);
            SetBkColor(pDI->hDC, crBack);
        }
    }

    if((ODA_FOCUS & pDI->itemAction) || (ODS_FOCUS & pDI->itemState))
        DrawFocusRect(pDI->hDC, &pDI->rcItem);
}

/* ***************************************************************************/
/* %%Function:CallLogProc */
/* Handles CallLog Non-Modal Dialog. Note we use fixed font so we can
        accurately compute tab stops from max chars per field. */

BOOL FAR PASCAL _export CallLogProc(HWND hDlg,UINT uMsg,WPARAM wParam,
                LPARAM lParam)
{
// table of controls and context-sensitive help IDs

    static DWORD aIds[] = {
        didCallLogSTextLog, IDH_DIALER_LOG,
        0, 0
    };

    switch (uMsg)
        {
        // Process clicks on controls after Context Help mode selected
        case WM_HELP:
            WinHelp (((LPHELPINFO) lParam)->hItemHandle, "dialer.hlp", HELP_WM_HELP,
                (DWORD)(LPSTR) aIds);
            break;

        // Process right-clicks on controls
        case WM_CONTEXTMENU:
            WinHelp ((HWND) wParam, "dialer.hlp", HELP_CONTEXTMENU, (DWORD)(LPVOID) aIds);
            break;

        case WM_DESTROY:
            {
            SetDialerProfileInt(vdws.hInst,ikszSecCallLogging,
                    ikszFieldCLVisible,vdws.fCallLogVisible);
            GetSetCallLogWinRect(vdws.hInst,vdws.hdlgCallLog,FALSE);
            vdws.hdlgCallLog = NULL;
            vdws.fCallLogVisible = FALSE;
            return TRUE;
            }

        case WM_INITDIALOG:
            return FALSE;

        case WM_CLOSE:
            {
            vdws.fCallLogVisible = FALSE;
            ShowWindow(vdws.hdlgCallLog,SW_HIDE);
            return TRUE;
            }

            case WM_DRAWITEM:
                {
                LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

                if (lpdis->CtlID == didCallLogSTextLog)
                    {
                            DrawItem (lpdis);
                            return TRUE;
                            }
                break;
                }

        case WM_INITMENUPOPUP:
            {
            HMENU mEdit;
            UINT wEnable;
            LONG nSelected;

            if (LOWORD(lParam) == 0)
                {/* Edit menu */
                mEdit = (HMENU)wParam;
                nSelected = SendDlgItemMessage(vdws.hdlgCallLog,
                        didCallLogSTextLog,LB_GETSELCOUNT,0,0);
                wEnable = ((nSelected != 0) ? MF_ENABLED:MF_GRAYED);
                EnableMenuItem(mEdit, midEditCut, wEnable);
                EnableMenuItem(mEdit, midEditCopy, wEnable);
                EnableMenuItem(mEdit, midEditDelete, wEnable);
                }
            else
                {/* Log menu */
                EnableMenuItem((HMENU)wParam,midLogDial,
                        (SendDlgItemMessage(vdws.hdlgCallLog,didCallLogSTextLog,
                        LB_GETSELCOUNT,0,0) == 1) ? MF_ENABLED:MF_GRAYED);
                }
            break;
            }

        case WM_SIZE:
            {
            MoveWindow(GetDlgItem(hDlg,didCallLogSTextLog),0,0,LOWORD(lParam),
                    HIWORD(lParam),TRUE);
                SetCallLogTabs(vdws.hdlgCallLog);
                break;
            }

        case WM_VKEYTOITEM: /* Delete key processing */
            {
            if (wParam == VK_CLEAR || wParam == VK_DELETE)
                {
                SendMessage(hDlg,WM_COMMAND,midEditDelete,0);
                return -2;
                } /* if */
            return -1;
            }
        case WM_COMMAND:
            {
            switch (wParam)
                {
                case midEditCut:
                    {
                    if (vdws.hdlgCallLog != 0)
                        {
                        CopyLogToClipboard(GetDlgItem(vdws.hdlgCallLog,
                                didCallLogSTextLog));
                        FRemoveLogEntries(GetDlgItem(vdws.hdlgCallLog,
                                didCallLogSTextLog));
                        }
                    break;
                    }
                case midEditCopy:
                    {
                    if (vdws.hdlgCallLog != 0)
                        CopyLogToClipboard(GetDlgItem(vdws.hdlgCallLog,
                                didCallLogSTextLog));
                    break;
                    }
                case midEditDelete:
                    {
                    if (vdws.hdlgCallLog != 0)
                        FRemoveLogEntries(GetDlgItem(vdws.hdlgCallLog,
                                didCallLogSTextLog));
                    break;
                    }
                case midLogDial:
                    {
                    char szLine[cchSzMax], szName[cchSzMax], szNum[cchSzMax];

                    SendDlgItemMessage(vdws.hdlgCallLog,didCallLogSTextLog,
                            LB_GETTEXT,
                        (WPARAM)SendDlgItemMessage(vdws.hdlgCallLog,
                                didCallLogSTextLog,LB_GETCURSEL,0,0),
                        (LPARAM)((LPSTR)szLine));
                    GetSzNameNumFromSzLine(szLine,szName,szNum);

                    vdws.fCommandFromLogWindow = TRUE;
                    FDialerInitiateCall(szNum,szName,FALSE);
                    vdws.fCommandFromLogWindow = FALSE;
                    break;
                    }
                /* Brings up the options dialog for call logging */
                case midOptionsLog:
                    {
                    vdws.fCommandFromLogWindow = TRUE;
                    DoDialog(CallLogOptionDlgProc,dlgLogOption,vdws.hInst,
                            vdws.hdlgCallLog,0);
                    vdws.fCommandFromLogWindow = FALSE;
                    break;
                    }
                case midHelpContents:
                    {
                    WinHelp(hDlg,"dialer.hlp",HELP_FINDER,0);
                    break;
                    }
                /* if we get a double click in the list box, dial the
                   associated number */
                case didCallLogSTextLog:
                    {
                    if (HIWORD(lParam) == CBN_DBLCLK
                        && SendDlgItemMessage(vdws.hdlgCallLog,
                            didCallLogSTextLog,LB_GETSELCOUNT,0,0) == 1)
                        SendMessage(hDlg,WM_COMMAND,midLogDial,0);
                    break;
                    }
                default:
                    return TRUE;
                }
            break;
            }
        } /* switch */


    return FALSE;

} /* CallLogProc */

/* ***************************************************************************/
/* %%Function:SzLogCat */
/* cat szDate onto szLog taking into account the size of szLog and the field
    position specified by iField. */

void SzLogCat(LPSTR szLog,LPCSTR szData,int iField)
{
    int cchLog = lstrlen(szLog);
    int cchLimit = cchLogLineMax - 4 - cchLog; /* leave room for \t, \r\n & \0
                                                  */

    if (iField)
        {
        lstrcpy((LPSTR)(&szLog[cchLog]),"\t");
        cchLog++;
        } /* if */

    _fstrncpyDialer((LPSTR)(&szLog[cchLog]),szData,cchLimit+1,lstrlen(szData));

} /* SzLogCat */

/* ***************************************************************************/
/* %%Function:LoadIntlFormats */
/* loads in international date/time format as stored in win.ini "Intl"
    section. */

void LoadIntlFormats()
{
    GetProfileString("Intl","s1159","am",vdws.szAM,sizeof(vdws.szAM));
    GetProfileString("Intl","s2359","pm",vdws.szPM,sizeof(vdws.szPM));
    GetProfileString("Intl","sDate","/",vdws.szDate,sizeof(vdws.szDate));
    GetProfileString("Intl","sTime",":",vdws.szTime,sizeof(vdws.szTime));
    GetProfileString("Intl","sShortDate","mm/dd/yy",vdws.szDateFormat,
            sizeof(vdws.szDateFormat));
    vdws.f24HourTime = GetProfileInt("Intl","iTime",0);

}  /* LoadIntlFormats */

/* ***************************************************************************/
/* %%Function:GetSzDateFromTm */
/* gets the date string corresponding to the date stored in the TM struct.
    assumes szDate is big enough. */

void GetSzDateFromTm(struct tm *tm,char *szDate)
{
    char *pch;
    char chSeg;
    char chSegLast = '\0';

    szDate[0] = '\0';
    for (pch = vdws.szDateFormat;*pch;pch++)
        {
        int data = 0;
        switch (*pch)
            {
            case 'm':
            case 'M':
                {
                chSeg = 'M';
                data = tm->tm_mon+1;
                break;
                }
            case 'd':
            case 'D':
                {
                chSeg = 'D';
                data = tm->tm_mday;
                break;
                }
            case 'Y':
            case 'y':
                {
                chSeg = 'Y';
                data = tm->tm_year;
                break;
                }
            } /* switch */

        if (data && chSegLast != chSeg)
            {
            if (chSegLast)
                lstrcat(szDate,vdws.szDate);
            wsprintf(&szDate[strlen(szDate)],"%02i",data);
            chSegLast = chSeg;
            } /* if */
        } /* for */

} /* GetSzDateFromTm */

/* ***************************************************************************/
/* %%Function:GetSzTimeFromTm */
/* gets the date string corresponding to the date stored in the TM struct.
        assumes szTime is big enough. */

void GetSzTimeFromTm(struct tm *tm,char *szTime)
{
    int hour;
    char *pch;

    hour = tm->tm_hour;
    pch = vdws.szPM;
    if (!vdws.f24HourTime)
        {
        if (hour <= 11)
            pch = vdws.szAM;
        else if (hour > 12)
            hour -= 12;
        } /* if */
    wsprintf(szTime,"%02i%s%02i%s",hour,(LPSTR)vdws.szTime,tm->tm_min,pch);

} /* GetSzTimeFromTm */

/* ***************************************************************************/
/* %%Function:FLogCall */
/* creates a log entry based on the input. return TRUE if success. */

BOOL FLogCall(BOOL in,LPSTR name,LPSTR number,time_t tBegin,time_t tEnd)
{
    char szLog[cchLogLineMax];
    char szBuffer[32], szMin[32];
    struct tm *tm;
    int fcLogFile;
    WORD cchLog;
    BOOL fSuccess = FALSE;

    /* a call that needn't be logged */
    if (in && !vdws.fLogIncomingCalls || !in && !vdws.fLogOutgoingCalls)
        return FALSE;

    if (name[0] == 0)
        {
        name[0] = ' ';
        name[1] = 0;
        } /* if */
    if (number[0] == 0)
        {
        number[0] = ' ';
        number[1] = 0;
        } /* if */

    /* load the header string */
    LoadString(vdws.hInst,in ? ikszCallLogFrom : ikszCallLogTo,szLog,
            sizeof(szLog));

    /* append name and number */
    SzLogCat(szLog,name,iLogFieldName);
    SzLogCat(szLog,number,iLogFieldNumber);

    /* append date string */
    tm = localtime(&tBegin);
    GetSzDateFromTm(tm,szBuffer);
    SzLogCat(szLog,szBuffer,iLogFieldDate);

    /* append time string. lop off seconds and include am/pm as required */
    GetSzTimeFromTm(tm,szBuffer);
    SzLogCat((LPSTR)szLog,(LPSTR)szBuffer,iLogFieldTime);

    /* append duration */
    LoadString(vdws.hInst,ikszCallLogMinute,szMin,sizeof(szMin));
    wsprintf(szBuffer,"%d %s",(int)ceil(difftime(tEnd,tBegin)/60.0),szMin);
    SzLogCat((LPSTR)szLog,(LPSTR)szBuffer,iLogFieldDuration);

    /* SzLogCat preserved 2 bytes at end for us! */
    lstrcat(szLog,"\r\n");

    /* append log entry to file */
    fcLogFile = FcOpenCallLog(TRUE);
    if (fcLogFile == HFILE_ERROR)
        goto LDone;
    if (_lseek(fcLogFile,0L,SEEK_END) < 0)
        goto LDone;
    cchLog = strlen(szLog);
    if (cchLog != _lwrite(fcLogFile,szLog,cchLog))
        goto LDone;

    fSuccess = TRUE;

LDone:
    if (fcLogFile != HFILE_ERROR)
        fSuccess = (_lclose(fcLogFile) == 0);

    /* read in all the log entry and update the log display */
    if (vdws.fCallLogVisible)
        FReadCallLog(GetDlgItem(vdws.hdlgCallLog,didCallLogSTextLog),FALSE);
    else
        vdws.fCallLogDirty = TRUE;

    return fSuccess;

} /* FLogCall */

/* ***************************************************************************/
/* %%Function:FRemoveLogEntries */
/* deletes the selected entries and reloads the display as necessary. this
    code depends on there being no additions to log that haven't been read by
    FReadCallLog, so that hwndList has the current tail of the log. */

BOOL FRemoveLogEntries(HWND hwndList)
{
    WORD cEntry, cEntryTotal;
    int iEntry;
    LONG ifc, ifcSmallest = 0x7fffffff;
    int FAR *lprgIEntrySel;
    DWORD cchSel = 0;
    WORD iItemSmallestIfc;
    LPSTR lpData = NULL, lpDataCur;
    LONG dataSize;
    BOOL fSuccess = FALSE;
    char xxxx[2];
    int fcLogFile;
    UINT cchEntry;
    BOOL fInvalid = FALSE;

    /* sets up cEntry and cEntryTotal */
    cEntry = (WORD)SendMessage(hwndList,LB_GETSELCOUNT,0,0);
    cEntryTotal = (WORD)SendMessage(hwndList,LB_GETCOUNT,0,0);
    if (cEntry == 0)
        return TRUE;

    /* figure out the amount of date we are removing and the lowest file
       offset */
    lprgIEntrySel = (int FAR *) _fmalloc(cEntry*sizeof(int));
    if (lprgIEntrySel == NULL)
        return FALSE;
    SendMessage(hwndList,LB_GETSELITEMS,(WPARAM)cEntry,(LPARAM)lprgIEntrySel);
    for (iEntry = 0; iEntry < (int)cEntry; iEntry++)
        {
        WORD iItem = lprgIEntrySel[iEntry];
        cchSel += SendMessage(hwndList,LB_GETTEXTLEN,iItem,0) + 2;
        ifc = SendMessage(hwndList,LB_GETITEMDATA,iItem,0);
        if (ifc < ifcSmallest)
            {
            ifcSmallest = ifc;
            iItemSmallestIfc = iItem;
            } /* if */
        } /* for */
    _ffree(lprgIEntrySel);

    /* open the log file */
    fcLogFile = FcOpenCallLog(TRUE);
    if (fcLogFile == HFILE_ERROR)
        return FALSE;
    if (_lseek(fcLogFile,ifcSmallest,SEEK_SET) < 0)
        goto LDone;

    /* Compute the size of the new piece of the log that starts at the first
       deleted line.  Add 2 bytes fudge to account for possible missing \r\n
       at eof */
    dataSize = vdws.logLengthPrev - ifcSmallest - cchSel + 2;
    vdws.logLengthPrev = ifcSmallest;
    fInvalid = TRUE;
    SendMessage(hwndList,WM_SETREDRAW,0,0);

    /* If this isn't just a truncate, put the new text in file.  Delete the
       selected rows and update the fileOffsets of other rows. */
    if (dataSize > 2)
        {
        lpData = GlobalAllocPtr(GHND,dataSize);
        if (lpData == NULL)
            goto LDone;
        lpDataCur = lpData;
        iEntry= iItemSmallestIfc + 1;
        while (--iEntry >= 0)
            {
            if (SendMessage(hwndList,LB_GETSEL,(WPARAM)iEntry,0) != 0)
                SendMessage(hwndList,LB_DELETESTRING,(WPARAM)iEntry,0);
            else
                {
                SendMessage(hwndList,LB_GETTEXT,(WPARAM)iEntry,
                        (LPARAM)lpDataCur);
                ifc = ifcSmallest + (lpDataCur-lpData);
                SendMessage(hwndList,LB_SETITEMDATA,(WPARAM)iEntry,ifc);
                lpDataCur = lpDataCur + lstrlen(lpDataCur);
                *lpDataCur++ = '\r';
                *lpDataCur++ = '\n';
                } /* else */
            } /* while */

        cchEntry = (UINT)(lpDataCur-lpData);
        vdws.logLengthPrev += cchEntry;
        if (cchEntry != 0 && cchEntry != _lwrite(fcLogFile,lpData,cchEntry))
            goto LDone;
        }
    else
        {
        /* Truncate only.  Delete selected rows */
        iEntry = iItemSmallestIfc + 1;
        while (--iEntry >= 0)
            {
            if (SendMessage(hwndList,LB_GETSEL,(WPARAM)iEntry,0) != 0)
                SendMessage(hwndList,LB_DELETESTRING,(WPARAM)iEntry,0);
            } /* while */
        } /* else */
    vdws.curLogData -= cchSel;

    /* Write of 0 truncates the file (ISN'T DOS SPECIAL?) */
    if (0 != _lwrite(fcLogFile,xxxx,0))
        goto LDone;

    fSuccess = TRUE;

LDone:
    if (lpData)
        GlobalFreePtr(lpData);
    if (fcLogFile != HFILE_ERROR)
        fSuccess = (_lclose(fcLogFile) != 0);

    FReadCallLog(hwndList,FALSE);
    if (fInvalid)
        {
        SendMessage(hwndList,WM_SETREDRAW,1,0);
        InvalidateRect(hwndList,NULL,TRUE);
        } /* if */

    return fSuccess;

} /* FRemoveLogEntries */

/* ***************************************************************************/
/* %%Function:SetCallLogTabs */
/* sets up the globals that contain the formatting info about a call log
    entry. */

void SetCallLogTabs (HWND hwnd)
{
    RECT    rcDlg;
    struct tm *tm;

    HWND    hwndList = GetDlgItem (hwnd, didCallLogSTextLog);
    HDC hdc     = GetDC (hwndList);
    time_t tBegin   = time(NULL);

    char    szbuf[cchLogLineMax];


    // from:
    LoadString (vdws.hInst, ikszCallLogFrom, szbuf, sizeof(szbuf));
    vdws.tabStops[0] = DialerGetTextWidth (hdc, szbuf, lstrlen(szbuf)) + 2;

    // number
    LoadString (vdws.hInst, ikszCallLogNumber, szbuf, sizeof(szbuf));
    vdws.tabStops[2] = DialerGetTextWidth (hdc, szbuf, lstrlen(szbuf)) + 6;

    // date
    tm = localtime (&tBegin);
    GetSzDateFromTm (tm, szbuf);
    vdws.tabStops[3] = DialerGetTextWidth (hdc, szbuf, lstrlen(szbuf)) + 6;

    // time
    GetSzTimeFromTm (tm, szbuf);
    vdws.tabStops[4] = DialerGetTextWidth (hdc, szbuf, lstrlen(szbuf)) + 4;

    // duration
    LoadString (vdws.hInst, ikszCallLogMinute, szbuf, sizeof(szbuf));
    lstrcat (szbuf, "60");
    vdws.tabStops[5] = DialerGetTextWidth (hdc, szbuf, lstrlen(szbuf));

    // name space is derived from amount left
    GetClientRect (hwndList, &rcDlg);

    vdws.tabStops[1] = rcDlg.right - vdws.tabStops[0] - vdws.tabStops[2] -
                        vdws.tabStops[3] - vdws.tabStops[4] - vdws.tabStops[5];

    if (vdws.tabStops[1] < 100)
        vdws.tabStops[1] = 100;


    InvalidateRect (hwndList, NULL, FALSE);
    ReleaseDC (hwndList, hdc);
}

/* ***************************************************************************/
/* %%Function:CopyLogToClipboard */
/* copies the selected call log entries into the clipboard as the CF_TEXT
    format. we also set clipboard of format vdws.cfDialer to notify ourselves
    that the clipboard is of our own format for the smart pasting of log
    entry. */

void CopyLogToClipboard(HWND hwndList)
{
    WORD cEntry;
    int FAR *lprgIEntrySel = NULL;
    DWORD cchTotal = 0;
    HGLOBAL ghData = 0;
    LPSTR lpData;
    WORD iEntry;

    /* get the number of selected entries */
    cEntry = (WORD) SendMessage(hwndList,LB_GETSELCOUNT,0,0);
    if (cEntry == 0)
        return;

    /* init lpgrCchItem */
    lprgIEntrySel = (int FAR *) _fmalloc(cEntry*sizeof(int));
    if (lprgIEntrySel == NULL)
        goto LDone;
    SendMessage(hwndList,LB_GETSELITEMS ,(WPARAM)cEntry, (LPARAM)lprgIEntrySel);

    for (iEntry = 0; iEntry < cEntry; iEntry++)
        cchTotal += SendMessage(hwndList,LB_GETTEXTLEN,lprgIEntrySel[iEntry],
                0) + 2;

    /* init ghData that will contain the dtat for the clipboard */
    ghData = GlobalAlloc(GHND,cchTotal+1);
    if (ghData == 0)
        goto LDone;
    lpData = GlobalLock(ghData);
    for (iEntry = 0; iEntry < cEntry; iEntry++)
        {
        SendMessage(hwndList,LB_GETTEXT,lprgIEntrySel[iEntry],(LPARAM)lpData);
        lpData = lpData + lstrlen(lpData);
        *lpData++ = '\r';
        *lpData++ = '\n';
        } /* for */
    GlobalUnlock(ghData);

    _ffree(lprgIEntrySel);
    lprgIEntrySel = NULL;

    /* give data to the clipboard */
    OpenClipboard(vdws.hwnd);
    EmptyClipboard();

    SetClipboardData(CF_TEXT,ghData);
    /* also put on the clipboard vdws.cfDialer so that we know our log entry
       is in the clipboard */
    SetClipboardData(vdws.cfDialer,NULL);

    CloseClipboard();
    ghData = NULL; /* Owned by ClipBoard now! */

LDone:
    if (lprgIEntrySel)
        _ffree(lprgIEntrySel);
    if (ghData)
        GlobalFree(ghData);

} /* CopyLogToClipboard */

/* ***************************************************************************/
/* %%Function:PasteLogClipboardIntoEditCtrl */
/* called when our call log entry is on the clipboard. It pastes either the
        name OR the number portion of the log entry into the specified
        edit/combo control based on fPasteName. */

void PasteLogClipboardIntoEditCtrl(HWND hwndCtrl,BOOL fPasteName)

{
    HGLOBAL ghData;
    LPSTR   lpData;
    char    szDelimiter[2] = {'\t',0}, chSave;
    int     ichDataStart, ichDataEnd, cchData;

    if (!IsClipboardFormatAvailable(CF_TEXT))
        return;

    OpenClipboard(vdws.hwnd);

    ghData = GetClipboardData(CF_TEXT);
    if (ghData == NULL)
        goto LOOM;

    lpData = GlobalLock(ghData);
    if (lpData == NULL)
        goto LOOM;

    cchData = lstrlen(lpData);
    ichDataStart = _fstrcspn(lpData,szDelimiter) + sizeof(szDelimiter) - 1;
    if (ichDataStart == cchData) /* the format is wrong */
        goto LUnlok;

    ichDataEnd = _fstrcspn(lpData + ichDataStart,szDelimiter) + ichDataStart;
    if (ichDataEnd == cchData) /* the format is wrong */
        goto LUnlok;

    if (!fPasteName)
        {
        ichDataStart = ichDataEnd + sizeof(szDelimiter) - 1;
        ichDataEnd = _fstrcspn(lpData + ichDataStart,
                szDelimiter) + ichDataStart;
        } /* if */

    chSave = lpData[ichDataEnd];
    lpData[ichDataEnd] = 0;
    /* this is a hack since SetWindowText wouldn't work for a combo box */
    if (GetDlgCtrlID(hwndCtrl) != didDialerComboNumDial)
        SetWindowText(hwndCtrl,lpData + ichDataStart);
    else
        {
        SendDlgItemMessage(GetParent(hwndCtrl),didDialerComboNumDial,
                CB_INSERTSTRING,0,(LPARAM)(lpData + ichDataStart));
        SendDlgItemMessage(GetParent(hwndCtrl),didDialerComboNumDial,
                CB_SETCURSEL,0,0);
        } /* else */
    lpData[ichDataEnd] = chSave;

LUnlok:
    GlobalUnlock(ghData);
LOOM:
    CloseClipboard();

} /* PasteLogClipboardIntoEditCtrl */

#ifdef DEBUG
void CDECL SPTrace(LPCSTR pszFormat, ...)
{
    static char szBuffer[512];
    static char fmtBuffer[1024];
    static char szModuleBuffer[_MAX_PATH];
    static char szTemp[_MAX_PATH];
    static char szFName[_MAX_FNAME];
    const char* pszLocalFormat;

    int nBuf, count, localCount;
    va_list args;

    pszLocalFormat = pszFormat;

    va_start (args, pszFormat);

    nBuf = wvsprintf (szBuffer, pszLocalFormat, args);

    // Convert formatting to readable format.
    for (count = 0, localCount = 0; count < nBuf; count++, localCount++)
    {
        if (szBuffer[count] == '\r')
        {
            fmtBuffer[localCount++] = '\\';
            fmtBuffer[localCount] = 'r';
        }
        else if (szBuffer[count] == '\n')
        {
            fmtBuffer[localCount++] = '\\';
            fmtBuffer[localCount] = 'n';
        }
        else
            fmtBuffer[localCount] = szBuffer[count];
    }

    fmtBuffer[localCount] = '\0';

    GetModuleFileName (vdws.hInst, szModuleBuffer, sizeof (szModuleBuffer));
    _splitpath (szModuleBuffer, szTemp, szTemp, szFName, szTemp);
    wsprintf (szBuffer, "%s: %s\n\r", (LPSTR) szFName, (LPSTR) fmtBuffer);

    OutputDebugString (szBuffer);
}
#endif

HWND HwndFromVdws()
{
    return vdws.hwnd;
}
