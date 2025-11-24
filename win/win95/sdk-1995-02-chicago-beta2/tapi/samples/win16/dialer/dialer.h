//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1993-1995 Microsoft Corporation, all rights reserved.
//
/****************************************************************************/
/*
    Dialer ---- Windows TAPI sample application created as an illustration of
                the usage of Windows TAPI

    Dialer does the following 3 things :

    (1) initiates/drops calls
    (2) handles simple TAPI request for other application initiating/dropping
        calls on their behalf
    (3) monitors incoming/outgoing calls and keeps a call log based on the
        user's request.

    dialer.h : contains dialer's extern function declaration as defined in
               dialer.c, inipref.c, and tapifu.c.
*/

/****************************************************************************/
/* global switches */

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>
void CDECL SPTrace(LPCSTR pszFormat, ...);
#define DebugMsg(_x_)  SPTrace _x_
#else
#define DebugMsg(_x_)
#endif

#define WIN31        /* this is a Windows 3.1 application */

#define STRICT       /* be bold! */

#define rmj     1
#define rmm     0
#define rup     150

#define VER_MAJOR               rmj
#define VER_MINOR               rmm
#define VER_BUILD               rup

#define SC_CONTEXTHELP          0xf180

/****************************************************************************/
/* include files */

#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "dialerrc.h"

/****************************************************************************/
/* constants definitions */

#define errNone                 0
#define cchSzMax                256

#define errNoVoiceLine          1
#define errLineClose            2

#define TAPI_VERSION1_0         0x00010003
#define TAPI_VERSION1_4         0x00010004
#define tapiVersionCur          TAPI_VERSION1_4

#define szDialerClass           "DialerClass"
#define szDummyClass            "DialerDummyClass"

/****************************************************************************/
/* declarations for context-sensitive help support in 16-bit app            */
/****************************************************************************/

#ifndef WM_CONTEXTMENU
#define WM_CONTEXTMENU          0x007B
typedef  struct  tagHELPINFO
{
    DWORD   cbSize;
    int     iContextType;
    int     iCtrlId;
    HANDLE  hItemHandle;
    DWORD   dwContextId;
    POINT   MousePos;
}
HELPINFO, FAR* LPHELPINFO;

#endif

/****************************************************************************/
/* functions defined in dialer.c */
/****************************************************************************/

char far *_fstrncpyDialer(char far*lpszDest,const char far *lpszSrc,
                size_t cchDest,DWORD cchSrc);

HWND HwndDialerMain(void);
HANDLE HInstDialer(void);
void ActivateNextTask(void);

void DisableDialerDesktop(BOOL fDisable);
void DialerReInitTapi(void);

BOOL FDialerInitiateCall(LPCSTR szNumber,LPCSTR szName,BOOL fIsSTapiMakeCall);
void FAR DisplayLineInUseDlg();
void FAR DisPlayDisconnectedErrorDlg(DWORD errCode);
int  FAR DisplayDialingPromptDlg(DWORD dwLineErrCode);

void ShowCallStatusDlg(LPCSTR szName,LPCSTR szNumber,LPCSTR szLocation,
                LPCSTR szCallingCard,LPCSTR szTranslatedSddress);
void UpdateCallStatusDlg(BOOL fCallIsActive,LPCSTR szName,LPCSTR szNumber);
void HideCallStatusDlg(void);
void DoChangeOptionDlg(void);

BOOL FLogCall(BOOL,LPSTR,LPSTR,time_t,time_t);
int IkszFromErrCode(DWORD errCode);
void FAR DialerFatalExit(DWORD errCode);
int DialerErrMessageBox(int stringID,UINT wStyle);

void DoubleUpAmpersandsInSz(char *sz,int cchSzBuffer);

/****************************************************************************/
/* functions defined in iniprefs.c */
/****************************************************************************/

int CchGetDialerProfileString(HINSTANCE hInst,int ikszSecName,
            int ikszFieldName,int ikszDefault,LPSTR lpszBuffer,int cchBuffer);
int WGetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
            int wDefault);

void SetDialerProfileString(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
            LPSTR lpszVal);
void SetDialerProfileInt(HINSTANCE hInst,int ikszSecName,int ikszFieldName,
            int wVal);

void FAR GetSetINIScreenPos(HINSTANCE hInst,POINT *pptTopLeft,BOOL fGet);
void FAR GetSetCallLogWinRect(HINSTANCE hInst,HWND hwndCL,BOOL fGet);

void FAR GetSetINIQuickDial(HINSTANCE hInst,UINT ips,char *szName,char *szNum,
            BOOL fGet);
void FAR GetAllINIQuickDials(HINSTANCE hInst,HWND hwndDialer);

void FAR GetLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer);
void FAR SaveLastDialedNumbers(HINSTANCE hInst,HWND hwndDialer);
HWND HwndFromVdws(void);

/****************************************************************************/
/* functions defined in tapifu.c */
/****************************************************************************/

/* TAPI init/clean functions */

DWORD ErrStartTapi(HINSTANCE hInst,LPCSTR szAppName);
void  TapiDone(void);

DWORD ErrInitCallingCardInfo(HINSTANCE hInst);
BOOL  FRegisterSimpleTapi(BOOL fRegister);

/* Phone/Change Options... dialogs related functions */

BOOL FAR FInitLineLBox(HINSTANCE hInst, HWND hdlgDialingOption);
BOOL FAR FInitAddressLBox(HINSTANCE hInst,HWND hdlgDialingOption);

void UpdateDialingOptionSettings(HINSTANCE hInst,HWND hdlgDialingOption,
            LPSTR szAppName);

LONG CallConfigDlg(HWND hWndOwner,LPCSTR lpszAddressIn);

/* calling related functions */

BOOL FCallInProgress(void);
void AbortTapiCallInProgress(void);
BOOL FInitiateCall(LPCSTR lpszDialString,LPCSTR lpszCalledParty,BOOL fIsSTapiMakeCall);
void FakeConnectedState(void);
BOOL FDropCurrentCall(void);
void TerminateCalls(HINSTANCE hInst);

void ProcessNextQueuedSTAPIRequest(void);

void GetCurCallTranslatedNumber(char *szNumber,WORD cchSzNumber,
            DWORD *pdwTranslateResults,BOOL fReTranslate);
void GetCurCallCanonicalNumber(char *szNumber,WORD cchSzNumber);
void GetCurCallName(char *szName,WORD cchSzName);
void SetCurCallName(char *szName);
void SetCurCallDisplayableString(char *szDisplayableString);
BOOL FCurCallIsSTapiMakeCall();
void ExpandNumberForDialHelper(char *szNumber,WORD cchSzNumber);
