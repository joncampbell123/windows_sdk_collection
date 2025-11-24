//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1993-1995 Microsoft Corporation, all rights reserved.
//
/* ***************************************************************************/
/* Dialer ---- Windows TAPI sample application created as an illustration of
   the usage of Windows TAPI

    Dialer does the following 3 things :

    (1) initiates/drops calls (2) handles simple TAPI request for other
    application initiating/dropping calls on their behalf (3) monitors
    incoming/outgoing calls and keeps a call log based on the user's request.

    tapifu.c : contains dialer's TAPI code. */

/* ***************************************************************************/
/* inculde files */

#include <windows.h>
#include <windowsx.h>
#include <tapi.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

#include "dialer.h"
#include "dialerrc.h"

/* ***************************************************************************/
/* constants definitions */

#define lcbLineDevCapsInitial               (sizeof(LINEDEVCAPS)+1000)
#define lcbAddrDevCapsInitial               (sizeof(LINEADDRESSCAPS)+1000)
#define lcbLineTransCapsInitial             (sizeof(LINETRANSLATECAPS)+1000)
#define lcbLineTransOutputInitial           (sizeof(LINETRANSLATEOUTPUT)+1000)

#define cchLineNameMac                      128
#define cchAddrNameMac                      128
#define cchLocationNameMac                  128
#define cchCardNameMac                      128

#define cstrrMax                            256

/* ***************************************************************************/
/* struct definitions and the corresponding global declarations */

typedef struct tagLINEINFO                  /* info we keep around for all
                                               available lines */
    {
    DWORD cAddr;                            /* number of available addresses
                                               on the line */
    BOOL fIsVoiceLine;                      /* is it a voice line or not? */
    DWORD dwAPIVersion;                     /* API version the line supports */
    HLINE hLine;                            /* handle to the line as returned
                                               by lineOpen */
    char  szLineName[cchLineNameMac];       /* the line's name */
    } LINEINFO,FAR *LPLINEINFO;

typedef enum tagMNS                         /* call MoNitoring State */
    {
    mnsIdle = 0,                            /* no call being monitored */
    mnsUnknown,                             /* don't know whether it is an
                                               incoming or outgoing call yet */
    mnsIncoming,                            /* call being monitored is
                                               incoming */
    mnsOutgoing                             /* call being monitored is
                                               outgoing */
    } MNS;

typedef struct tagDMR                       /* Dialer Monitor Record */
    {
    HCALL   hcall;                          /* handle to the call being
                                               monitored */
    MNS     mns;                            /* state of the call being
                                               monitored */
    BOOL    fConnected;                     /* has the call reached the
                                               connected state? */
    time_t  tBegin;                         /* beginning time of the call */
    time_t  tEnd;                           /* ending time of the call */

        HLINE   hLine;                                                  /* handle to the line on which the call exists */
    } DMR,FAR *LPDMR;


/* **************************************************/

typedef struct tagDTS                               /* Dialer Tapi State */
    {
    BOOL            fLineInited;                    /* whether lineInitialize
                                                       succeeded */
    BOOL            fSTapiMakeCallRegistered;       /* whether we are
                                                       registered for being
                                                       STAPI MakeCall
                                                       recipient */
    BOOL            fRefusing;                      /* This flag is set when
                                                       exiting to refuse new
                                                       requests */
    BOOL            fTapiAddrDLLPresent;            /* the address translation
                                                       module is present */
    BOOL            fReInitTapi;                    /* set to TRUE when
                                                       telephon.ini has
                                                       changed and we should
                                                       re init */
    BOOL            fCheckMakeCallRequest;          /* on idle, check TAPI
                                                       queue to see if there
                                                       is any STAPI MakeCall
                                                       reueste pending */
    BOOL            fAbortCall;

    HLINEAPP        hApp;                           /* instance handle TAPI
                                                       gives back to us
                                                       through lineInitialize
                                                       */
    DWORD           cLines;                         /* number of lines
                                                       available */
    LPLINEINFO      lprgLineInfo;                   /* info on all lines */

    DWORD           iLineCur;                       /* the line selected by
                                                       the user */
    DWORD           iAddrCur;                       /* the address selected by
                                                       the user */
    DWORD           dwCardCur;                      /* the calling card
                                                       selected by the user */

    LPDMR           lprgdmr;                        /* array of DMRs being
                                                       monitored */
    size_t          idmrMax;                        /* size of the *lprgdmr */
    size_t          idmrMac;                        /* number of calls
                                                       currently being
                                                       monitored */

    char szLocation[cchLocationNameMac];            /* the location name
                                                       selected by the user */
    char szCallingCard[cchCardNameMac];             /* the calling card name
                                                       selected by the user */
    char szAreaCode[4];                             /* area code of the
                                                       currentle selected
                                                       location */
    } DTS;


static DTS vdts = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,NULL,0,
            NULL,0,0,(DWORD)-1,NULL,0,0};

/* **************************************************/

typedef enum tagMCS                         /* These are states that we
                                               progress through on the way to
                                               establishing call */
    {
    mcsIdle,                                /* no call in progress */
    mcsMaking,                              /* just called lineMakeCall and
                                               waiting for its LINE_REPLY */
    mcsStopMaking,                          /* waiting for the LINE_REPLY for
                                               lineMakeCall so that the call
                                               can be dropped as requested by
                                               the user */
    mcsDropping,                            /* just called lineDrop and
                                               waiting for ite LINE_REPLY */
    mcsMade                                 /* lineMakeCall succeeded and the
                                               call is connected */
    } MCS;

typedef struct tagDCR                       /* Dialer Call Record */
    {
    MCS mcs;                                /* state of the call as tracked by
                                               the Dialer */
    DWORD lcs;                              /* Line Call State as trakced by
                                               TAPI about the call */
    DWORD requestID;                        /* as returned by async TAPI
                                               functions lineMakeCall/
                                               lineDropCall */
    DWORD iLine;                            /* the line ID on which the call
                                               is made */
    DWORD iAddr;                            /* the originating address ID of
                                               the call being made */
    HLINE hLine;                            /* handle to the line */
    HCALL hCall;                            /* handle to the call */

    char szDialString[TAPIMAXDESTADDRESSSIZE]; /* phone number being called */
    char dialStringDisplayable[TAPIMAXDESTADDRESSSIZE];
    char szDialStringOriginal[TAPIMAXDESTADDRESSSIZE];
    char szDialStringCanonical[TAPIMAXDESTADDRESSSIZE];
    char szCalledParty[TAPIMAXCALLEDPARTYSIZE]; /* name of the party being called */
    DWORD dwTranslateResults;

    BOOL fIsMakeCallRequest;
    BOOL fAlreadyConnected;

    LONG lErrCode;

    } DCR,FAR *LPDCR;


static DCR vdcr = {mcsIdle,LINECALLSTATE_IDLE};

/* ***************************************************************************/
/* function declarations */

LONG GetCachedLineInfo(HANDLE hInst,DWORD iLine);

/* calling/dropping core functions */

void TranslateAddress(LPDCR lpdcr,BOOL *pfCallAborted,BOOL fExpandOnly);
BOOL FLocalInitiateCall(LPCSTR lpszDialString,LPCSTR lpszCalledParty,
           LPDCR lpdcr);
BOOL FDropCall(LPDCR lpdcr);
VOID DoLineClose(LPDCR lpdcr);
LONG LDialerLineTranslateAddress(
                HLINEAPP hApp,
                DWORD dwDeviceId,
                DWORD dwApiVersion,
                LPCSTR lpszAddressIn,
                DWORD dwCard,
                DWORD dwTranslateOptions,
                LPLINETRANSLATEOUTPUT lpLineTranslateOutput);
LONG LDialerLineGetTranslateCaps(
                HLINEAPP hLineApp,
                DWORD dwApiVersion,
                LPLINETRANSLATECAPS lpLineTranslateCaps);

/* STAPI functions for calling on other apps behalf */

void DoMakeCallRequest(void);

/* call logging functions */

void LogUsingCall(HCALL,BOOL,time_t,time_t);

/* call monitoring functions */

LPDMR LpdmrAddMonitor(HCALL);
LPDMR LpdmrFindMonitor(HCALL);
VOID RemoveMonitor(LPDMR);

/* call state transition related functions */

VOID NewCallState(DWORD newState,DWORD callbackInstance,DWORD privilege,
           HCALL call,DWORD dwParam2);
void FAR PASCAL _export LineCallBackProc(DWORD hDevice,DWORD dwMessage,
           DWORD dwInstance,DWORD dwParam1,DWORD dwParam2,DWORD dwParam3);

/* ***************************************************************************/
/* TAPI init/clean functions */
/* ***************************************************************************/
/* %%Function:ErrStartTapi */
/* initializes TAPI by calling lineInitialize. enumerates all the available
   lines to set up vdts.lprgLineInfo. also opens up each available line for
   monitoring. sets up vdts.iLineCur and vdts.iAddrCur by checking the
   preferred line/address name stored in the ini file  against the available
   line/address names.

    returns errNone if success and the corresponding error code otherwise. */

DWORD ErrStartTapi(HANDLE hInst,LPCSTR szAppName)
{
    LONG lResult;
    DWORD iLine, iAddr, cAddr, iLineVoiceFirst = (DWORD)-1;
    LPLINEADDRESSCAPS lpAddrCaps = NULL;
    char szPreferedLine[cchLineNameMac];
    char szPreferedAddress[cchAddrNameMac];
    DWORD tc = GetTickCount();
    BOOL fFirstTime = TRUE;

    /* **************************************************/
    /* initialize tapi */
    while ((lResult = lineInitialize(&(vdts.hApp),hInst,
            (LINECALLBACK)MakeProcInstance((FARPROC)LineCallBackProc,hInst),
            szAppName,&(vdts.cLines))) == LINEERR_REINIT)
        {
        MSG msg;

        if (fFirstTime)
            {
            EnableWindow(HwndDialerMain(),FALSE);
            DisableDialerDesktop(TRUE);
            } /* if */

        if (PeekMessage(&msg,0,0,0,PM_REMOVE))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            } /* if */

        if (GetTickCount() - tc >= 5000 || fFirstTime)
            {
            if (DialerErrMessageBox(ikszWarningTapiReInit,MB_RETRYCANCEL) == IDCANCEL)
                break;

            tc = GetTickCount();
            fFirstTime = FALSE;
            } /* if */
        } /* while */
    if (!fFirstTime)
        {
        DisableDialerDesktop(FALSE);
        EnableWindow(HwndDialerMain(),TRUE);
        } /* if */

    if (lResult != errNone)
        return lResult;
    vdts.fLineInited = TRUE;

    /* **************************************************/
    /* initialize vdts.lprgLineInfo and open each available line for
       monitoring */

    /* get the name of the prefered line from ini file */
    CchGetDialerProfileString(hInst,ikszSecPreference,ikszFieldPreferedLine,
            ikszPreferedLineDefault,szPreferedLine,sizeof(szPreferedLine));

    vdts.iLineCur = (DWORD)-1;

    /* allocate buffer for storing LINEINFO for all the available lines */
        /* always allocate room for at least one line */

        if (vdts.cLines == 0)
                vdts.lprgLineInfo = (LPLINEINFO) _fmalloc(sizeof(LINEINFO));
        else
                vdts.lprgLineInfo = (LPLINEINFO) _fmalloc(sizeof(LINEINFO)*(int)vdts.cLines);

    if (vdts.lprgLineInfo == NULL)
        return LINEERR_NOMEM;

    if (vdts.cLines == 0)
                _fmemset(vdts.lprgLineInfo,0,sizeof(LINEINFO));
        else
                _fmemset(vdts.lprgLineInfo,0,sizeof(LINEINFO)*(int)vdts.cLines);


    /* init vdts.lprgLineInfo and open each line for monitoring */
    for (iLine = 0; iLine < vdts.cLines; ++iLine)
        {
        if (GetCachedLineInfo(hInst,iLine) != errNone)
            continue; // skip rest of processing this line if didn't open

        /* tell tapi that we are interested in CAPSCHANGE messages        */
        /* if this fails, we don't hear about lines that get reconfigured */
        /* which is a non-fatal problem so we ignore the rc               */
        lineSetStatusMessages (vdts.lprgLineInfo[iLine].hLine,
                               LINEDEVSTATE_CAPSCHANGE,
                               LINEADDRESSSTATE_CAPSCHANGE);

        if (iLineVoiceFirst == (DWORD)-1
            && vdts.lprgLineInfo[iLine].fIsVoiceLine)
            iLineVoiceFirst = iLine;

        /* check if it's the current line */
        if (lstrcmpi(vdts.lprgLineInfo[iLine].szLineName,szPreferedLine) == 0
            && vdts.lprgLineInfo[iLine].fIsVoiceLine)
            vdts.iLineCur = iLine;
        } /* for */

    if ((vdts.iLineCur == (DWORD)-1)
        && (iLineVoiceFirst != (DWORD)-1))
            vdts.iLineCur = iLineVoiceFirst;

    vdts.iAddrCur = 0;

        if (vdts.iLineCur != (DWORD)-1)
                {
            /* **************************************************/
            /* init vdts.iAddrCur */

            /* get the name of the prefered address from ini file */
            CchGetDialerProfileString(hInst,ikszSecPreference,ikszFieldPreferedAddress,
                    ikszPreferedAddressDefault,szPreferedAddress,
                    sizeof(szPreferedAddress));

            /* allocate buffer for the lineGetAddressCaps calls */
            if ((lpAddrCaps = (LPLINEADDRESSCAPS)_fmalloc(lcbAddrDevCapsInitial))
                    == NULL)
                {
                lResult = LINEERR_NOMEM;
                goto LDone;
                } /* if */
            lpAddrCaps->dwTotalSize = lcbAddrDevCapsInitial;

            /* enumerate all the available addresses to match szPreferedAddress with
               an address name */
            cAddr = vdts.lprgLineInfo[vdts.iLineCur].cAddr;
            for (iAddr = 0; iAddr < cAddr; ++iAddr)
                {
                char szAddrName[cchAddrNameMac];
                LPSTR lpszAddrName;

            /* get address capability info */
                if (lineGetAddressCaps(vdts.hApp,vdts.iLineCur,iAddr,
                        vdts.lprgLineInfo[vdts.iLineCur].dwAPIVersion,0,lpAddrCaps) != errNone)
                                continue; // skip this address if fails

                /* reallocate buffer if not big enough */
                while (lpAddrCaps->dwNeededSize > lpAddrCaps->dwTotalSize)
                    {
                    DWORD lcbNeeded;

                                lcbNeeded = lpAddrCaps->dwNeededSize;

                    _ffree(lpAddrCaps);
                    if ((lpAddrCaps = (LPLINEADDRESSCAPS)_fmalloc((size_t)lcbNeeded))
                            == NULL)
                        {
                        lResult = LINEERR_NOMEM;
                        goto LDone;
                        } /* if */
                    lpAddrCaps->dwTotalSize = lcbNeeded;

                    /* try it one more time */
                    lResult = lineGetAddressCaps(vdts.hApp,vdts.iLineCur,iAddr,
                            vdts.lprgLineInfo[vdts.iLineCur].dwAPIVersion,0,lpAddrCaps);
                    if (lResult != errNone)
                        break;
                    } /* while */

            if (lResult != errNone)
                                {
                                lResult = 0;
                                continue;               // skip this address on error
                                }
                /* get the address's name */
                if (lpAddrCaps->dwAddressSize > 0)
                    lpszAddrName = (LPSTR)(lpAddrCaps)+lpAddrCaps->dwAddressOffset;
                else
                    {/* use default name */
                    char szAddrFormat[32];

                    LoadString(hInst,ikszCallLogAddrName,szAddrFormat,
                            sizeof(szAddrFormat));
                    wsprintf(szAddrName,szAddrFormat,iAddr);
                    lpszAddrName = (LPSTR)szAddrName;
                    } /* else */

                if (lstrcmpi(lpszAddrName,szPreferedAddress) == 0)
                    {
                    vdts.iAddrCur = iAddr;
                    break;
                    } /* if */
                } /* for */
        } /* if a line exists */

    lResult = errNone;

LDone:
    /* free up memory allocated */
    if (lpAddrCaps)
        _ffree(lpAddrCaps);

    return lResult;

} /* ErrStartTapi */

/* ***************************************************************************/
/* %%Function:TapiDone */
/* frees up the memory allocated, closes all the lines we have opened for
   monitoring and calls lineShutDown to disconnect from TAPI. */

void TapiDone()
{
    /* never mind if lineInitialize failed in the first place */
    if (!vdts.fLineInited)
        return;

    /* unregister STAPI */
    FRegisterSimpleTapi(FALSE);

    /* closes all the open lines and free vdts.lprgLineInfo */
    if (vdts.lprgLineInfo)
        {
        DWORD iLine;

        for (iLine = 0; iLine < vdts.cLines; ++iLine)
            if (vdts.lprgLineInfo[iLine].hLine != 0)
                lineClose(vdts.lprgLineInfo[iLine].hLine);

        _ffree(vdts.lprgLineInfo);
        } /* if */

    /* disconnect from TAPI */
    lineShutdown(vdts.hApp);

        /* show that we're no longer inited */
        vdts.fLineInited = FALSE;

    /* free the rest of the memory allocated */
    if (vdts.lprgdmr)
        _ffree(vdts.lprgdmr);

} /* TapiDone */

/* ***************************************************************************/
/* %%Function:GetCachedLineInfo */
/* updates all the cached info about the specified line.
        returns the result. */

LONG GetCachedLineInfo(HANDLE hInst,DWORD iLine)

{
    LPLINEDEVCAPS lpDevCaps = NULL;
    LINEEXTENSIONID lineExtensionID;
    char szLineName[cchLineNameMac];
    LPSTR lpszLineName;
    LONG lResult = 0;

    /* negotiate API version for the line */

    lResult = lineNegotiateAPIVersion(vdts.hApp,iLine,TAPI_VERSION1_0,
                tapiVersionCur,&(vdts.lprgLineInfo[iLine].dwAPIVersion),
                        &lineExtensionID);
    if (lResult != errNone)
        goto LDone;

    /* if line not already open, open the line for monitoring */

        if (0 == vdts.lprgLineInfo[iLine].hLine)
                {
            lResult = lineOpen(vdts.hApp,iLine,&(vdts.lprgLineInfo[iLine].hLine),
                    vdts.lprgLineInfo[iLine].dwAPIVersion,
                0,(DWORD)(LPDMR)NULL,LINECALLPRIVILEGE_MONITOR,0,NULL);
            if (lResult != errNone)
                goto LDone;
                } /* if */

    /* allocate buffer for the lineGetDevCaps calls */

    if ((lpDevCaps = (LPLINEDEVCAPS) _fmalloc(lcbLineDevCapsInitial)) == NULL)
                {
        lResult = LINEERR_NOMEM;
                goto LDone;
                }
    lpDevCaps->dwTotalSize = lcbLineDevCapsInitial;

    /* get line capability info */

    lResult = lineGetDevCaps(vdts.hApp,iLine,
            vdts.lprgLineInfo[iLine].dwAPIVersion,0,lpDevCaps);
    if (lResult != errNone)
        goto LDone;

    /* reallocate buffer if not big enough */
    while (lpDevCaps->dwNeededSize > lpDevCaps->dwTotalSize)
        {
        DWORD lcbNeeded;

        lcbNeeded = lpDevCaps->dwNeededSize;
        _ffree(lpDevCaps);
        if ((lpDevCaps = (LPLINEDEVCAPS) _fmalloc((size_t)lcbNeeded)) == NULL)
                        {
                lResult = LINEERR_NOMEM;
                        goto LDone;
                        }
        lpDevCaps->dwTotalSize = lcbNeeded;

        /* try it one more time */
        lResult = lineGetDevCaps(vdts.hApp,iLine,
                vdts.lprgLineInfo[iLine].dwAPIVersion,0,lpDevCaps);
        if (lResult != errNone)
            goto LDone;
        } /* while */

    vdts.lprgLineInfo[iLine].cAddr = lpDevCaps->dwNumAddresses;
    vdts.lprgLineInfo[iLine].fIsVoiceLine = ((lpDevCaps->dwMediaModes
        & LINEMEDIAMODE_INTERACTIVEVOICE) != 0);

    if (lpDevCaps->dwLineNameSize > 0)
        lpszLineName = (LPSTR)(lpDevCaps)+lpDevCaps->dwLineNameOffset;
    else
        {/* use default name */
        char szLineFormat[32];

        LoadString(hInst,ikszCallLogLineName,szLineFormat,
                sizeof(szLineFormat));
        wsprintf(szLineName,szLineFormat,iLine);
        lpszLineName = (LPSTR)szLineName;
        } /* else */
    if (lstrlen(lpszLineName) > cchLineNameMac - 1)
        lpszLineName[cchLineNameMac - 1] = 0;
    _fstrncpyDialer(vdts.lprgLineInfo[iLine].szLineName,lpszLineName,
            sizeof(vdts.lprgLineInfo[iLine].szLineName),
            lpDevCaps->dwLineNameSize);

LDone:
        if (lResult != errNone)
                {
                if (vdts.lprgLineInfo[iLine].hLine)
                        lineClose(vdts.lprgLineInfo[iLine].hLine);

                // if any calls are being monitored on the line, stop monitoring
                        {
                    LPDMR lpdmr     = vdts.lprgdmr;
                    LPDMR lpdmrLast = lpdmr + vdts.idmrMax - 1;

                    for (; lpdmr <= lpdmrLast; ++lpdmr)
                        if (lpdmr->mns != mnsIdle
                                        && lpdmr->hLine == vdts.lprgLineInfo[iLine].hLine)
                                        RemoveMonitor(lpdmr);
                        }

                vdts.lprgLineInfo[iLine].hLine = (HLINE)0;
                vdts.lprgLineInfo[iLine].dwAPIVersion = 0; // mark as unavailable
            vdts.lprgLineInfo[iLine].cAddr = 0;
            vdts.lprgLineInfo[iLine].fIsVoiceLine = FALSE;
                vdts.lprgLineInfo[iLine].szLineName[0] = 0;
                } /* if lResult != errNone */

    if (lpDevCaps)
        _ffree(lpDevCaps);
    return lResult;

} /* GetCachedLineInfo */

/* ***************************************************************************/
/* %%Function:LpLineTransCapsGetLineTransCaps */
/* allocates a big enough buffer, calls lineGetTranslateCaps and returns the
   buffer. it's the caller's responsibility to free the buffer. */

LPLINETRANSLATECAPS LpLineTransCapsGetLineTransCaps(DWORD *perrCode);
LPLINETRANSLATECAPS LpLineTransCapsGetLineTransCaps(DWORD *perrCode)

{
    LPLINETRANSLATECAPS lpLineTransCaps;

    /* allocate buffer */
    if ((lpLineTransCaps = (LPLINETRANSLATECAPS)_fmalloc(lcbLineTransCapsInitial))
            == NULL)
        {
        *perrCode = LINEERR_NOMEM;
        return NULL;
        } /* if */
    lpLineTransCaps->dwTotalSize = lcbLineTransCapsInitial;

    /* try to get TranslateCaps */
    if ((*perrCode = LDialerLineGetTranslateCaps(vdts.hApp,
            tapiVersionCur,lpLineTransCaps)) != errNone)
        goto LDone;

    /* reallocate buffer if not big enough */
    while (lpLineTransCaps->dwNeededSize > lpLineTransCaps->dwTotalSize)
        {
        DWORD lcbNeeded = lpLineTransCaps->dwNeededSize;

        _ffree(lpLineTransCaps);
        if ((lpLineTransCaps = (LPLINETRANSLATECAPS)_fmalloc((size_t)lcbNeeded))
                == NULL)
            {
            *perrCode = LINEERR_NOMEM;
            return NULL;
            } /* if */
        lpLineTransCaps->dwTotalSize = lcbNeeded;

        /* try one more time */
        if ((*perrCode = LDialerLineGetTranslateCaps(vdts.hApp,
                tapiVersionCur,lpLineTransCaps)) != errNone)
            goto LDone;
        } /* while */

    return lpLineTransCaps;

LDone:
    if (lpLineTransCaps != NULL)
        _ffree(lpLineTransCaps);
    return NULL;

} /* LpLineTransCapsGetLineTransCaps */

/* ***************************************************************************/
/* %%Function:ErrInitCallingCardInfo */

DWORD ErrInitCallingCardInfo(HANDLE hInst)

{
    DWORD errCode;
    LPLINETRANSLATECAPS lpLineTransCaps = LpLineTransCapsGetLineTransCaps(&errCode);
    LPLINELOCATIONENTRY lplle;
    LPLINECARDENTRY lplce;
    DWORD iCard, cCards, curCard;
    DWORD iLoc, cLocs, curLoc;

    LoadString(hInst,ikszUnknown,vdts.szCallingCard,
            sizeof(vdts.szCallingCard));
    LoadString(hInst,ikszUnknown,vdts.szLocation,
        sizeof(vdts.szLocation));

        if (lpLineTransCaps == NULL)
        return errNone;

    cLocs = lpLineTransCaps->dwNumLocations;
    curLoc = lpLineTransCaps->dwCurrentLocationID;
    lplle = (LPLINELOCATIONENTRY)((LPSTR)(lpLineTransCaps) + lpLineTransCaps->dwLocationListOffset);
    for (iLoc = 0; iLoc < cLocs; ++iLoc)
        {
        if (lplle[iLoc].dwPermanentLocationID != curLoc)
            continue;
            _fstrncpyDialer(vdts.szLocation,
                    (LPSTR)(lpLineTransCaps) + lplle[iLoc].dwLocationNameOffset,
                    sizeof(vdts.szLocation),lplle[iLoc].dwLocationNameSize);
            _fstrncpyDialer(vdts.szAreaCode,
                    (LPSTR)(lpLineTransCaps) + lplle[iLoc].dwCityCodeOffset,
                    sizeof(vdts.szAreaCode),lplle[iLoc].dwCityCodeSize);
        break;
        } /* for */

        cCards = lpLineTransCaps->dwNumCards;
    curCard = lpLineTransCaps->dwCurrentPreferredCardID;
    lplce = (LPLINECARDENTRY)((LPSTR)(lpLineTransCaps) + lpLineTransCaps->dwCardListOffset);
    for (iCard = 0; iCard < cCards; ++iCard)
        {
        if (lplce[iCard].dwPermanentCardID != curCard)
            continue;
        _fstrncpyDialer(vdts.szCallingCard,
                (LPSTR)(lpLineTransCaps) + lplce[iCard].dwCardNameOffset,
                sizeof(vdts.szCallingCard),lplce[iCard].dwCardNameSize);
        break;
        } /* for */

    _ffree(lpLineTransCaps);
    vdts.fTapiAddrDLLPresent = TRUE;
    return errNone;

} /* ErrInitCallingCardInfo */

/* ***************************************************************************/
/* %%Function:FRegisterSimpleTapi */
/* register/unregister for simple tapi requests.
   sets vdts.fSTapiMakeCallRegistered accordingly. */

BOOL FRegisterSimpleTapi(BOOL fRegister)
{
    DWORD lResult;

    /* (un)register make call */
    if (fRegister != vdts.fSTapiMakeCallRegistered)
        {
        lResult = lineRegisterRequestRecipient(vdts.hApp,0,LINEREQUESTMODE_MAKECALL,
            fRegister);
        if (lResult != errNone)
                return FALSE;
        vdts.fSTapiMakeCallRegistered = fRegister;
        } /* if */

    return TRUE;

} /* FRegisterSimpleTapi */

/* ***************************************************************************/
/* Phone/Change Options... dialogs related functions */
/* ***************************************************************************/
/* %%Function:FInitLineLBox */
/* initialize the available line list box for the "Phone Options..." dialog.
   also selects the currently selected line. */

BOOL FAR FInitLineLBox(HINSTANCE hInst, HWND hdlgDialingOption)
{
    HWND  hwndLBoxLine = GetDlgItem(hdlgDialingOption,didDialingOptionLBoxLine);

    DWORD iLine, iItemCur = (DWORD)-1;
    char szPreferedLine[cchLineNameMac];

    /* get the name of the prefered line from ini file */
    CchGetDialerProfileString(hInst,ikszSecPreference,ikszFieldPreferedLine,
            ikszPreferedLineDefault,szPreferedLine,sizeof(szPreferedLine));

    /* enumerate each line device available */

    for (iLine = 0; iLine < vdts.cLines; ++iLine)
        {
        DWORD iItem;

        if ((GetCachedLineInfo(hInst,iLine) != errNone)
            || (!vdts.lprgLineInfo[iLine].fIsVoiceLine))
            continue;

        iItem = SendMessage(hwndLBoxLine,CB_ADDSTRING,0,
                (LPARAM)(LPCSTR)(vdts.lprgLineInfo[iLine].szLineName));
        if (iItem == LB_ERR || iItem == LB_ERRSPACE)
            return FALSE;
        /* the listbox is a sorted list box, so use ITEMDATA to store the
           device id */
        SendMessage(hwndLBoxLine,CB_SETITEMDATA,(WPARAM)iItem,(LPARAM)iLine);
        if (iLine == vdts.iLineCur)
            iItemCur = iItem;
                else if ((vdts.iLineCur == -1)
                                && (lstrcmpi(vdts.lprgLineInfo[iLine].szLineName,szPreferedLine) == 0))
                                iItemCur = iItem;
        else if (iItemCur != -1 && iItem <= iItemCur)
            ++iItemCur;
        } /* for */

        if (iItemCur = (DWORD)-1)
                iItemCur = 0;
        if (SendMessage(hwndLBoxLine,CB_GETCOUNT,0,0) != 0)
                {
                SendMessage(hwndLBoxLine,CB_SETCURSEL,(WPARAM)iItemCur,0);
                return TRUE;
                }

    return FALSE;

} /* FInitLineLBox */

/* ***************************************************************************/
/* %%Function:FInitAddressLBox */
/* initialize the available address list box for the "Phone Options..."
   dialog. also selects the first address if the current selection in the line
   list box is not the same line as the current line. Otherwise, it selects
   the current address. */

BOOL FAR FInitAddressLBox(HANDLE hInst,HWND hdlgDialingOption)
{
    WORD cAddr;
    WORD iAddr;
    WORD iLineCur;
    DWORD iItemCur = (DWORD)-1;
    LPLINEADDRESSCAPS lpAddrCaps;
    BOOL fSuccess = FALSE;
    HWND hwndLineLBox = GetDlgItem(hdlgDialingOption,didDialingOptionLBoxLine);
    HWND hwndAddrLBox = GetDlgItem(hdlgDialingOption,
            didDialingOptionLBoxAddress);

        if (SendMessage(hwndLineLBox,CB_GETCOUNT,0,0) == 0)
                return FALSE;

    iLineCur = (WORD)SendMessage(hwndLineLBox,CB_GETITEMDATA,
            (WORD)SendMessage(hwndLineLBox,CB_GETCURSEL,0,0),0);
    cAddr = (WORD)vdts.lprgLineInfo[iLineCur].cAddr;
    SendMessage(hwndAddrLBox,CB_RESETCONTENT,0,0);

    /* allocate buffer */
    if ((lpAddrCaps = (LPLINEADDRESSCAPS)_fmalloc(lcbAddrDevCapsInitial))
            == NULL)
        return FALSE;
    lpAddrCaps->dwTotalSize = lcbAddrDevCapsInitial;

    /* enumerate all the available addresses */
    iAddr = 0;
    while (iAddr < cAddr)
        {
        DWORD iItem;
        LONG lResult;
        char szAddrName[cchAddrNameMac];
        LPSTR lpszAddrName;

    /* get address capability info */
        if ((lResult = lineGetAddressCaps(vdts.hApp,iLineCur,iAddr,
                tapiVersionCur,0,lpAddrCaps)) != 0)
            goto LDone;

        /* reallocate buffer if not big enough */
        while (lpAddrCaps->dwNeededSize > lpAddrCaps->dwTotalSize)
            {
            DWORD lcbNeeded = lpAddrCaps->dwNeededSize;

            _ffree(lpAddrCaps);
            if ((lpAddrCaps = (LPLINEADDRESSCAPS)_fmalloc((size_t)lcbNeeded))
                    == NULL)
                goto LDone;
            lpAddrCaps->dwTotalSize = lcbNeeded;
            continue;
            } /* while */

        /* get the address's name */
        if (lpAddrCaps->dwAddressSize > 0)
            lpszAddrName = (LPSTR)(lpAddrCaps)+lpAddrCaps->dwAddressOffset;
        else
            {/* use default name */
            char szAddrFormat[32];

            LoadString(hInst,ikszCallLogAddrName,szAddrFormat,
                    sizeof(szAddrFormat));
            wsprintf(szAddrName,szAddrFormat,iAddr);
            lpszAddrName = (LPSTR)szAddrName;
            }

        /* insert the name into the list box */
        iItem = SendMessage(hwndAddrLBox,CB_ADDSTRING,0,
                (LPARAM)(LPCSTR)lpszAddrName);
        if (iItem == LB_ERR || iItem == LB_ERRSPACE)
            goto LDone;

        /* the listbox is a sorted list box, so use ITEMDATA to store the
           device id */
        SendMessage(hwndAddrLBox,CB_SETITEMDATA,(WPARAM)iItem,(LPARAM)iAddr);
        if (iLineCur == vdts.iLineCur)
            if (iAddr == vdts.iAddrCur)
                iItemCur = iItem;
            else if (iItemCur != -1 && iItem <= iItemCur)
                ++iItemCur;

        iAddr++;
        } /* while */

    SendMessage(hwndAddrLBox,CB_SETCURSEL,
            (iLineCur == vdts.iLineCur) ? ((WORD)iItemCur) : 0,0);
    fSuccess = TRUE;

LDone:
    if (lpAddrCaps != NULL)
        _ffree(lpAddrCaps);
    return fSuccess;

} /* FInitAddressLBox */

/* ***************************************************************************/
/* %%Function:UpdateDialingOptionSettings */
/* called after the user hits "OK" in the "Phone Options..." dialog. saves
   away the user's line and address selection. */

void UpdateDialingOptionSettings(HANDLE hInst,HWND hdlgDialingOption,LPSTR szAppName)

{
    HWND hwndCtrl;
    WORD iItemCur;
    char szBuffer[cchSzMax];

    /* update line */
    hwndCtrl = GetDlgItem(hdlgDialingOption,didDialingOptionLBoxLine);
    vdts.iLineCur = (WORD)SendMessage(hwndCtrl,CB_GETITEMDATA,
            iItemCur = (WORD)SendMessage(hwndCtrl,CB_GETCURSEL,0,0),0);
    GetDlgItemText(hdlgDialingOption,didDialingOptionLBoxLine,szBuffer,
            sizeof(szBuffer));
    SetDialerProfileString(hInst,ikszSecPreference,ikszFieldPreferedLine,
            szBuffer);

    /* update address */
    hwndCtrl = GetDlgItem(hdlgDialingOption,didDialingOptionLBoxAddress);
    vdts.iAddrCur = (WORD)SendMessage(hwndCtrl,CB_GETITEMDATA,
            (WORD)SendMessage(hwndCtrl,CB_GETCURSEL,0,0),0);
    GetDlgItemText(hdlgDialingOption,didDialingOptionLBoxAddress,szBuffer,
            sizeof(szBuffer));
    SetDialerProfileString(hInst,ikszSecPreference,ikszFieldPreferedAddress,
            szBuffer);

    /* update application priority */
    lstrcat(szAppName,".exe");
    lineSetAppPriority(szAppName,0,NULL,LINEREQUESTMODE_MAKECALL,NULL,
                (SendDlgItemMessage(hdlgDialingOption,didDialingOptionChkBoxSTAPIVoice,BM_GETCHECK,0,0L) != 0) ? 1 : 0);
    szAppName[lstrlen(szAppName) - 4] = 0;

} /* UpdateDialingOptionSettings */

/* ***************************************************************************/
/* %%Function:CallConfigDlg */
/* calls the lineTranslateDialog and returns the result. */

LONG CallConfigDlg(HWND hWndOwner,LPCSTR lpszAddressIn)
{
    long lResult;

    lResult = lineTranslateDialog(vdts.hApp,0,tapiVersionCur,hWndOwner,lpszAddressIn);
    if (lResult == LINEERR_INUSE)
        lResult = errNone;
    return lResult;

} /* CallConfigDlg */


/* ***************************************************************************/
/* call status dialog related functions */
/* ***************************************************************************/
/* %%Function:GetCurCallTranslatedNumber */
/* returns the translated number of the current call. */

void GetCurCallTranslatedNumber(char *szNumber,WORD cchSzNumber,
           DWORD *pdwTranslateResults,BOOL fReTranslate)

{
    BOOL fCallAborted;

    if (fReTranslate)
        {
        _fstrncpyDialer(vdcr.szDialString,vdcr.szDialStringOriginal,
                lstrlen(vdcr.szDialStringOriginal) + 1,
                lstrlen(vdcr.szDialStringOriginal) + 1);
        TranslateAddress(&vdcr,&fCallAborted,FALSE);
        } /* if */

    _fstrncpyDialer(szNumber,vdcr.szDialString,cchSzNumber,
            lstrlen(vdcr.szDialString));
    *pdwTranslateResults = vdcr.dwTranslateResults;

} /* GetCurCallTranslatedNumber */

/* ***************************************************************************/
/* %%Function:GetCurCallCanonicalNumber */
/* returns the canonical number of the current call. */

void GetCurCallCanonicalNumber(char *szNumber,WORD cchSzNumber)

{
    _fstrncpyDialer(szNumber,vdcr.szDialStringCanonical,cchSzNumber,
            lstrlen(vdcr.szDialStringCanonical));

} /* GetCurCallCanonicalNumber */

void GetCurCallName(char *szName,WORD cchSzName)

{
    _fstrncpyDialer(szName,vdcr.szCalledParty,cchSzName,
            lstrlen(vdcr.szCalledParty));

} /* GetCurCallName */

/* ***************************************************************************/
/* %%Function:ExpandNumberForDialHelper */
/* expands the number pass in so that it can be passed in to dial helper */

void ExpandNumberForDialHelper(char *szNumber,WORD cchSzNumber)

{
    BOOL fCallAborted;

    _fstrncpyDialer(vdcr.szDialString,szNumber,
            sizeof(vdcr.szDialString),lstrlen(szNumber));
        TranslateAddress(&vdcr,&fCallAborted,TRUE);
        if (vdcr.szDialStringCanonical[0] != 0)
        _fstrncpyDialer(szNumber,vdcr.szDialStringCanonical,cchSzNumber,
                lstrlen(vdcr.szDialStringCanonical));

} /* ExpandNumberForDialHelper */

/* ***************************************************************************/
/* ***************************************************************************/
/* %%Function:SetCurCallName */
/* sets vdcr.szCalledParty to the passed in string. */

void SetCurCallName(char *szName)

{
    _fstrncpyDialer(vdcr.szCalledParty,szName,sizeof(vdcr.szCalledParty),
            lstrlen(szName));

} /* SetCurCallName */

/* ***************************************************************************/
/* %%Function:SetCurCallDisplayableString */
/* sets vdcr.szDisplayableString. */

void SetCurCallDisplayableString(char *szDisplayableString)

{
    _fstrncpyDialer(vdcr.dialStringDisplayable,szDisplayableString,
            sizeof(vdcr.dialStringDisplayable),lstrlen(szDisplayableString));

} /* SetCurCallDisplayableString */

/* ***************************************************************************/
/* %%Function:FCurCallIsSTapiMakeCall */
/* returns vdcr.fIsMakeCallRequest. */

BOOL FCurCallIsSTapiMakeCall()

{
    return vdcr.fIsMakeCallRequest;
} /* */

/* ***************************************************************************/
/* making/dropping calls functions */
/* ***************************************************************************/
/* %%Function:FDialerCanonicalNumberTranslation */
/* this is Dialer's version of canonical number expansion for North America
   phone numbers. szDialString is assumed to be in a buffer of size
   TAPIMAXDESTADDRESSSIZE. */

BOOL FDialerCanonicalNumberTranslation(LPSTR szDialString)

{
    char szDigits[11] = {'0','1','2','3','4','5','6','7','8','9',0};
    char szCNum[TAPIMAXDESTADDRESSSIZE] = {'+','1',' ','('};
    LPSTR lpch, lpch2, lpch3, lpch4, lpch5;
    int cchDigit, ichCNumEnd = 4;

    if (!vdts.fTapiAddrDLLPresent || vdts.szAreaCode[0] == 0
        || lstrlen(szDialString) >= TAPIMAXDESTADDRESSSIZE - 9)
        return FALSE;

    /* skip all the leading white space characters */
    lpch = szDialString;
    while (*lpch == ' ')
        ++lpch;

    /* number is already canonical or bad */
    if (*lpch == '+')
        return TRUE;

    /* kill all the leading white space */
    if (lpch != szDialString)
        _fmemmove(szDialString,lpch,
                lstrlen(szDialString) - (lpch - szDialString) + 1);

    /* init cchDigit */
    for (cchDigit = 0, lpch = szDialString; *lpch != 0 && *lpch != '$'
        && *lpch != 'W' && *lpch != '?' && *lpch != '@'; ++lpch)
        if (*lpch >= '0' && *lpch <= '9')
            ++cchDigit;

    if (cchDigit < 7 || cchDigit > 11 || cchDigit == 9)
        return FALSE;

    /* find the pointer to the fist few digits */
    lpch = _fstrpbrk(szDialString,szDigits);
    lpch2 = _fstrpbrk(lpch + 1,szDigits);
    lpch3 = _fstrpbrk(lpch2 + 1,szDigits);
    lpch4 = _fstrpbrk(lpch3 + 1,szDigits);
    lpch5 = _fstrpbrk(lpch4 + 1,szDigits);

    /* don't expand in the following cases */
    if (cchDigit == 7 && (*lpch == '0' || *lpch == '1')
        || cchDigit == 8 && (*lpch != '0' && *lpch != '1' || *lpch2 == '0'
            || *lpch2 == '1')
        || cchDigit == 10 && (*lpch == '0' || *lpch == '1' || *lpch4 == '0'
            || *lpch4 == '1')
        || cchDigit == 11 && (*lpch != '0' && *lpch != '1' || *lpch2 == '0'
            || *lpch2 == '1' || *lpch5 == '0' || *lpch5 == '1'))
        return FALSE;

    /* add xxx to the end of szCNum */
    if (cchDigit < 10)
        {/* use the area code of the current location */
        lstrcpy(szCNum + ichCNumEnd,vdts.szAreaCode);
        ichCNumEnd += lstrlen(vdts.szAreaCode);
        if (cchDigit == 8)
            lpch = lpch2;
        } /* if */
    else if (cchDigit == 10)
        {
        szCNum[ichCNumEnd++] = *lpch;
        szCNum[ichCNumEnd++] = *lpch2;
        szCNum[ichCNumEnd++] = *lpch3;
        lpch = lpch4;
        } /* else if */
    else
        {
        szCNum[ichCNumEnd++] = *lpch2;
        szCNum[ichCNumEnd++] = *lpch3;
        szCNum[ichCNumEnd++] = *lpch4;
        lpch = lpch5;
        } /* else */

    szCNum[ichCNumEnd++] = ')';
    szCNum[ichCNumEnd++] = ' ';

    /* now put the reminder of szDialString into szCNum */
    _fmemmove(szCNum + ichCNumEnd,lpch,lstrlen(lpch) + 1);
    _fmemmove(szDialString,szCNum,lstrlen(szCNum)+1);

    return TRUE;

} /* FDialerCanonicalNumberTranslation */

/* ***************************************************************************/
/* %%Function:TranslateAddress */
/* calls LineTranslateAddress to get the dialString pointed to by lpdcr
   translated. stores the result in lpdcr->dialStringDisplayable while
   lpdcr->dialString is updated with the translated number. */

void TranslateAddress(LPDCR lpdcr,BOOL *pfCallCanceled,BOOL fExpandOnly)

{
    LONG lResult;
    DWORD errCode;
    BOOL fInUSA = FALSE;
    LPLINETRANSLATECAPS lpLineTransCaps;
    LPLINETRANSLATEOUTPUT lpLineTransOutput;
    char szDialStringOld[TAPIMAXDESTADDRESSSIZE];

    *pfCallCanceled = FALSE;

    _fstrncpyDialer(szDialStringOld,lpdcr->szDialString,
            lstrlen(lpdcr->szDialString) + 1,lstrlen(lpdcr->szDialString));
    _fstrncpyDialer(lpdcr->dialStringDisplayable,lpdcr->szDialString,
            lstrlen(lpdcr->szDialString) + 1,lstrlen(lpdcr->szDialString));
    lpdcr->szDialStringCanonical[0] = 0;

    lpLineTransCaps = LpLineTransCapsGetLineTransCaps(&errCode);

    if (lpLineTransCaps != NULL)
        {/* scan all locations associated with the line to figure out the
            country code of the current location */
        DWORD iLocation;
        DWORD cLocation = lpLineTransCaps->dwNumLocations;
        DWORD dwLocationCur = lpLineTransCaps->dwCurrentLocationID;
        LPLINELOCATIONENTRY lprgLineLocationEntry =
            (LPLINELOCATIONENTRY)((LPSTR)(lpLineTransCaps)
            + lpLineTransCaps->dwLocationListOffset);

        for (iLocation = 0; iLocation < cLocation; ++iLocation)
            if (lprgLineLocationEntry[iLocation].dwPermanentLocationID
                    == dwLocationCur)
                {
                fInUSA = (lprgLineLocationEntry[iLocation].dwCountryCode == 1);
                break;
                } /* if */
        } /* if */

    if (fInUSA)
        {
        _fstrncpyDialer(szDialStringOld,lpdcr->szDialString,
                lstrlen(lpdcr->szDialString) + 1,lstrlen(lpdcr->szDialString));

        if (FDialerCanonicalNumberTranslation(lpdcr->szDialString))
            _fstrncpyDialer(lpdcr->szDialStringCanonical,lpdcr->szDialString,
                    sizeof(lpdcr->szDialStringCanonical),
                    lstrlen(lpdcr->szDialString));
        } /* if */

    if (!vdts.fTapiAddrDLLPresent)
        return;

        if (fExpandOnly)
                return;

    /* allocate buffer */
    if ((lpLineTransOutput =
            (LPLINETRANSLATEOUTPUT)_fmalloc(lcbLineTransOutputInitial))
                == NULL)
        {
        lResult = LINEERR_NOMEM;
        goto LTranslateError;
        } /* if */
    lpLineTransOutput->dwTotalSize = lcbLineTransOutputInitial;

    /* try to get TranslateOutput */
    if ((lResult = LDialerLineTranslateAddress(vdts.hApp,lpdcr->iLine,
            vdts.lprgLineInfo[0].dwAPIVersion,lpdcr->szDialString,
            0,0,lpLineTransOutput)) != errNone)
        goto LTranslateError;

    /* reallocate buffer if not big enough */
    while (lpLineTransOutput->dwNeededSize > lpLineTransOutput->dwTotalSize)
        {
        DWORD lcbNeeded = lpLineTransOutput->dwNeededSize;

        _ffree(lpLineTransOutput);
        if ((lpLineTransOutput =
            (LPLINETRANSLATEOUTPUT)_fmalloc((size_t)lcbNeeded))
                == NULL)
            {
            lResult = LINEERR_NOMEM;
            goto LTranslateError;
            }
        lpLineTransOutput->dwTotalSize = lcbNeeded;

        /* try one more time */
        lResult = LDialerLineTranslateAddress(vdts.hApp,lpdcr->iLine,
                vdts.lprgLineInfo[0].dwAPIVersion,lpdcr->szDialString,0,0,lpLineTransOutput);
        if (lResult != errNone)
            {
LTranslateError:
            DialerErrMessageBox(IkszFromErrCode(lResult),MB_ICONEXCLAMATION | MB_OK);
            *pfCallCanceled = TRUE;
            goto LDone;
            } /* if */
        } /* while */

    _fstrncpyDialer(lpdcr->szDialString,
            (LPSTR)(lpLineTransOutput)
                + lpLineTransOutput->dwDialableStringOffset,
            sizeof(lpdcr->szDialString),
            lpLineTransOutput->dwDialableStringSize);
    _fstrncpyDialer(lpdcr->dialStringDisplayable,
            (LPSTR)(lpLineTransOutput)
                + lpLineTransOutput->dwDisplayableStringOffset,
            sizeof(lpdcr->dialStringDisplayable),
            lpLineTransOutput->dwDisplayableStringSize);
    lpdcr->dwTranslateResults = lpLineTransOutput->dwTranslateResults;

    /* if in USA, check if the user is trying to dial 911 and if so make sure
       that's what he/she wants */
    if (fInUSA)
        {
        char FAR *lpch;
        int ich = 0;
        BOOL fDialing911 = FALSE;

        for (lpch = lpdcr->szDialString; *lpch && ich < 3; ++lpch)
            {
            if (*lpch < '0' || *lpch > '9')
                continue;

            if (ich == 0 && *lpch != '9')
                break;
            else if (ich != 0 && *lpch != '1')
                break;

            ++ich;
            if (ich == 3)
                fDialing911 = TRUE;
            } /* while */
        if (fDialing911)
            {
            *pfCallCanceled = (DialerErrMessageBox(ikszWarning911,
                    MB_APPLMODAL | MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONSTOP) == IDCANCEL);
            } /* if */
        } /* if */

LDone:
    if (lpLineTransOutput != NULL)
        _ffree(lpLineTransOutput);

    if (lResult != errNone) /* restore the dialstring */
        _fstrncpyDialer(lpdcr->szDialString,szDialStringOld,
                lstrlen(szDialStringOld) + 1,lstrlen(szDialStringOld));

} /* TranslateAddress */

/* ***************************************************************************/
/* %%Function:DwDialerLineMakeCall */
/* Dialer's version of lineMakeCall. deals with all the control characters
   embedded inside lpdcr->szDialString. */

LONG DwDialerLineMakeCall(LPDCR lpdcr,LPLINECALLPARAMS lpParams,BOOL *pfCallMade)

{
    LONG lResult;
    BOOL fCallAborted;
    LPSTR lpchDialStart;
    char szCtrlCh[2] = {0,0};

    *pfCallMade = FALSE;
    lpchDialStart = lpdcr->szDialString;
    while (TRUE)
        {
        LONG dwLineErrCode;
        int ichCtrl;
        char chSave;

        /* try to make the call */
        if (!(*pfCallMade))
            lResult = lineMakeCall(lpdcr->hLine,&(lpdcr->hCall),lpchDialStart,0,
                    lpParams);
        else
            lResult = lineDial(lpdcr->hCall,lpchDialStart,0);
        if (lResult > 0) /* async */
            {/* we succeeded */
            lpdcr->requestID = lResult;
            return lResult;
            } /* if */

        /* if we fail not due to any control characters, just return */
        if (lResult != LINEERR_DIALBILLING && lResult != LINEERR_DIALDIALTONE
            && lResult != LINEERR_DIALPROMPT && lResult != LINEERR_DIALQUIET)
            return lResult;

        dwLineErrCode = lResult;

        /* find the offending control character and kill it */
        switch (lResult)
            {
            case LINEERR_DIALBILLING:
                {
                szCtrlCh[0] = '$';
                break;
                }
            case LINEERR_DIALDIALTONE:
                {
                szCtrlCh[0] = 'W';
                break;
                }
            case LINEERR_DIALQUIET:
                {
                szCtrlCh[0] = '@';
                break;
                }
            case LINEERR_DIALPROMPT:
                {
                szCtrlCh[0] = '?';
                break;
                }
            } /* switch */
        ichCtrl = _fstrcspn(lpchDialStart,szCtrlCh);
        lpchDialStart[ichCtrl] = ';';
        chSave = lpchDialStart[ichCtrl + 1];
        lpchDialStart[ichCtrl + 1] = 0;

        /* try to make the call again */
        if (!(*pfCallMade))
            lResult = lineMakeCall(lpdcr->hLine,&(lpdcr->hCall),lpchDialStart,0,
                    lpParams);
        else
            lResult = lineDial(lpdcr->hCall,lpchDialStart,0);
        if (lResult < 0)
            {/* we failed again but shouldn't */
            return lResult;
            } /* if */
        if (lResult > 0) /* async */
            lpdcr->requestID = lResult;

        fCallAborted = (DisplayDialingPromptDlg(dwLineErrCode) != 0);

        /* continue to the next iteration */
        lpchDialStart[ichCtrl] = szCtrlCh[0];
        lpchDialStart[ichCtrl + 1] = chSave;
        lpchDialStart += ichCtrl + 1;
        *pfCallMade = TRUE;

        /* put up the user prompt box */
        if (fCallAborted)
            {
            FDropCurrentCall();
            return 0;
            } /* if */
        } /* while */

} /* DwDialerLineMakeCall */

/* ***************************************************************************/
/* %%Function:FLocalInitiateCall */
/* upon entry, expects iLine and iAddr of lpdcr to
   be already set up. copies lpszDialString and lpszCalledParty into the
   corresponding fields in *lpdcr; opens the line for calling; builds the call
   parameters; and calls lineMakeCall to initiate the call. also puts up the
   call status dialog. */

BOOL FLocalInitiateCall(LPCSTR lpszDialString,LPCSTR lpszCalledParty,
            LPDCR lpdcr)
{
    LONG lResult;
    LPLINECALLPARAMS lpParams;
    WORD cchCalledParty;
        WORD cchDisplayable;
    BOOL fCallCanceled;
    BOOL fCallMade;

    /* copy dial string and called party name */
    _fstrncpyDialer(lpdcr->szDialString,lpszDialString,
            sizeof(lpdcr->szDialString),lstrlen(lpszDialString));
    _fstrncpyDialer(lpdcr->szDialStringOriginal,lpdcr->szDialString,
            sizeof(lpdcr->szDialStringOriginal),
            lstrlen(lpdcr->szDialString));
    _fstrncpyDialer(lpdcr->szCalledParty,lpszCalledParty,
            sizeof(lpdcr->szCalledParty),lstrlen(lpszCalledParty));
    lpdcr->szCalledParty[sizeof(lpdcr->szCalledParty)-1] = '\0';
    cchCalledParty = lstrlen(lpdcr->szCalledParty)+1;
        if (cchCalledParty == 1)
                cchCalledParty = 0;

    /* open the line specified in lpdcr->iLine for dialing */
    lResult = lineOpen(vdts.hApp,lpdcr->iLine,&(lpdcr->hLine),
            vdts.lprgLineInfo[lpdcr->iLine].dwAPIVersion,
        0,(DWORD)(lpdcr),LINECALLPRIVILEGE_NONE,0,NULL);
    if (lResult != errNone)
        {
        DialerErrMessageBox(IkszFromErrCode(lResult),MB_ICONEXCLAMATION | MB_OK);
        ActivateNextTask();
        return FALSE;
        } /* if */

    lpdcr->mcs = mcsMaking;
    lpdcr->lcs = LINECALLSTATE_UNKNOWN;

    TranslateAddress(lpdcr,&fCallCanceled,FALSE);

    if (fCallCanceled)
        {
        DoLineClose(lpdcr);
        return TRUE;
        } /* if */

        cchDisplayable = lstrlen(lpdcr->dialStringDisplayable) + 1;
        if (cchDisplayable == 1)
                cchDisplayable = 0;

    /* build call parameters */
    lpParams = (LPLINECALLPARAMS) _fmalloc(sizeof(LINECALLPARAMS) + cchCalledParty + cchDisplayable);
        if (lpParams)
                {
            _fmemset(lpParams,0,sizeof(LINECALLPARAMS) + cchCalledParty + cchDisplayable);

            lpParams->dwTotalSize = sizeof(LINECALLPARAMS) + cchCalledParty + cchDisplayable;
            lpParams->dwBearerMode = LINEBEARERMODE_VOICE;
            lpParams->dwMediaMode = LINEMEDIAMODE_INTERACTIVEVOICE;
            lpParams->dwCallParamFlags = LINECALLPARAMFLAGS_IDLE;
            lpParams->dwAddressMode = LINEADDRESSMODE_ADDRESSID;
            lpParams->dwAddressID = lpdcr->iAddr;
            if (cchCalledParty != 0)
                        {
                lpParams->dwCalledPartySize = cchCalledParty;
                        lpParams->dwCalledPartyOffset = sizeof(LINECALLPARAMS);
                _fstrncpy((LPSTR)lpParams + sizeof(LINECALLPARAMS), (LPSTR)lpdcr->szCalledParty, cchCalledParty);
                        }
            if (cchDisplayable != 0)
                        {
                lpParams->dwDisplayableAddressSize = cchDisplayable;
                        lpParams->dwDisplayableAddressOffset = sizeof(LINECALLPARAMS) + cchCalledParty;
                _fstrncpy((LPSTR)lpParams + sizeof(LINECALLPARAMS) + cchCalledParty, (LPSTR)lpdcr->dialStringDisplayable, cchDisplayable);
                        }
                }


    ShowCallStatusDlg(lpdcr->szCalledParty,lpdcr->szDialStringOriginal,
            vdts.szLocation,vdts.szCallingCard,lpdcr->dialStringDisplayable);

    /* make the call */
    lResult = DwDialerLineMakeCall(lpdcr,lpParams,&fCallMade);

        if (lpParams)  /* safe to dealloc this after synchronous return */
        _ffree(lpParams);

    if (lResult >= 0) /* async or aborted by user */
        return TRUE;

    HideCallStatusDlg();

    if (fCallMade)
        {/* lineDial failed */
        lpdcr->lErrCode = lResult;
        return FALSE;
        } /* if */

    /* lineMakeCall failed */
    if (lResult == LINEERR_CALLUNAVAIL)
        DisplayLineInUseDlg();
    else
        DialerErrMessageBox(IkszFromErrCode(lResult),MB_ICONEXCLAMATION | MB_OK);

    DoLineClose(lpdcr);
    lpdcr->mcs = mcsIdle;

    return FALSE;

} /* FLocalInitiateCall */

/* ***************************************************************************/
/* %%Function:FCallInProgress */
/* returns whether a call is in progress. */

BOOL FCallInProgress(void)

{
    return !(vdcr.mcs == mcsIdle || vdcr.mcs == mcsDropping);

} /* FCallInProgress */

/* ***************************************************************************/
/* %%Function:FInitiateCall */
/* init vdcr and calls FLocalInitiateCall to do the real work. */

BOOL FInitiateCall(LPCSTR lpszDialString,LPCSTR lpszCalledParty,BOOL fIsSTapiMakeCall)

{
    BOOL fSuccess;

    if (vdts.fRefusing)
        return TRUE;

        if (vdts.iLineCur == (DWORD)-1)
                {
                DialerErrMessageBox(ikszErrNoVoiceLine,MB_ICONEXCLAMATION | MB_OK);
                return TRUE;
                }
        else
                {
            vdcr.iLine = vdts.iLineCur;
            vdcr.iAddr = vdts.iAddrCur;
            vdcr.fIsMakeCallRequest = fIsSTapiMakeCall;
            vdcr.fAlreadyConnected = FALSE;
            vdcr.lErrCode = errNone;
            fSuccess = FLocalInitiateCall(lpszDialString,lpszCalledParty,&vdcr);
                }

} /* FInitiateCall */

/* ***************************************************************************/
/* %%Function:FDropCall */
/* calls lineDrop which returns asynchronously to drop the call specified by
   lpdcr. If we are still waiting for the preceding linMakeCall to complete,
   defer the drop by setting lpdcr->mcs to mcsStopMaking. */

BOOL FDropCall(LPDCR lpdcr)

{
    DWORD lResult;

    if (lpdcr->mcs != mcsMade)
        {
        if (lpdcr->mcs == mcsMaking)
            lpdcr->mcs = mcsStopMaking;
        return TRUE;
        } /* if */

    lResult = lineDrop(lpdcr->hCall,NULL,0);
    if (lResult > 0) /* assync */
        {
        lpdcr->mcs = mcsDropping;
        lpdcr->requestID = lResult;

        DisableDialerDesktop(TRUE);
        return TRUE;
        } /* if */

    lpdcr->mcs = mcsIdle;
    return FALSE;

} /* FDropCall */

/* ***************************************************************************/
/* %%Function:DoLineClose */
/* close the line as specified by lpdcr. */

VOID DoLineClose(LPDCR lpdcr)
{
    lpdcr->mcs = mcsIdle;
    lpdcr->lcs = LINECALLSTATE_IDLE;
    lineClose(lpdcr->hLine);

    if (lpdcr->fIsMakeCallRequest)
        ActivateNextTask();

} /* DoLineClose */

/* ***************************************************************************/
/* %%Function:FDropCurrentCall */
/* drops the current call as specified by vdcr. calls FDropCall to do the real
   work. */
BOOL FDropCurrentCall()
{
    return FDropCall(&vdcr);

} /* FDropCurrentCall */

/* ***************************************************************************/
/* %%Function:TerminateCalls */
/* drops the current call (if there is one) and waits until it goes to idle.
   gives up after 1 min in anycase. */

void TerminateCalls(HANDLE hInst)
{
    DWORD dwTick = GetTickCount();

    vdts.fRefusing = TRUE;
    if (vdcr.lcs == LINECALLSTATE_IDLE)
        return;

    FDropCurrentCall();

} /* TerminateCalls */

/* ***************************************************************************/
/* functions for calling/dropping on other apps behalf */
/* ***************************************************************************/
/* %%Function:lpCallDataFromSTAPIRequest */
/* calls lineGetRequest to retrieve the call data of the first queued request
   of the specified type. it is the caller's responsibility to free up the
   allocated memory. also updates the global flags vdts.fCheckMakeCallRequest */

LPSTR lpCallDataFromSTAPIRequest(DWORD dwRequestMode)

{
    DWORD lResult;
    LPSTR lpData;

    lpData = _fmalloc(sizeof(LINEREQMAKECALL));
    if (lpData == NULL)
        return NULL;

    lResult = lineGetRequest(vdts.hApp,dwRequestMode,lpData);
    if (lResult != errNone)
        {
        if (lResult == LINEERR_NOREQUEST)
            {/* turn down the flag */
            vdts.fCheckMakeCallRequest = FALSE;
            } /* if */

        _ffree(lpData);
        return NULL;
        } /* if */

    return lpData;

} /* lpCallDataFromSTAPIRequest */

/* ***************************************************************************/
/* %%Function:DoMakeCallRequest */
/* handles tapiRequestMakeCall. should only be called when vdts.mcs ==
   mcsIdle. */

void DoMakeCallRequest()
{
    LPLINEREQMAKECALL lpMakeCallData =
        (LPLINEREQMAKECALL)lpCallDataFromSTAPIRequest(LINEREQUESTMODE_MAKECALL);

    if (vdts.fRefusing || lpMakeCallData == NULL)
        return;

    FDialerInitiateCall(lpMakeCallData->szDestAddress,
            lpMakeCallData->szCalledParty,TRUE);
    _ffree(lpMakeCallData);

} /* DoMakeCallRequest */

/* ***************************************************************************/
/* %%Function:ProcessNextQueuedSTAPIRequest */
/* get the next queued simple tapi message and process it. */

void ProcessNextQueuedSTAPIRequest()

{
    if (vdcr.lcs != LINECALLSTATE_IDLE)
        return;

    if (vdts.fReInitTapi)
        {
        TapiDone();
        DialerReInitTapi();
        vdts.fReInitTapi = FALSE;
        } /* if */

    if (vdts.fCheckMakeCallRequest)
        {
        DoMakeCallRequest();
        PostMessage(HwndDialerMain(),WM_USER,0,0);
        }

} /* ProcessNextQueuedSTAPIRequest */

/* ***************************************************************************/
/* call logging functions */
/* ***************************************************************************/
/* %%Function:LpCallInfoFromHCall */
/* returns lpCallInfo for hcall as returned by lineGetCallInfo. returns NULL
   on failure. */

static LPLINECALLINFO LpCallInfoFromHCall(HCALL hcall)

{
    LPLINECALLINFO lpCallInfo = NULL;
    LONG lResult = 0;
    size_t lcb;

    /* allocate memory for lineGetCallInfo */
    lcb = sizeof(LINECALLINFO)+1000;
    lpCallInfo = (LPLINECALLINFO) _fmalloc(lcb);
    if (lpCallInfo == NULL)
        return NULL;

    /* try it for the first time */
    lpCallInfo->dwTotalSize = lcb;
    lResult = lineGetCallInfo(hcall,lpCallInfo);
    if (lResult < 0)
        goto LOOM;

    /* if buffer not big enough, try it again */
    while (lpCallInfo->dwNeededSize > lpCallInfo->dwTotalSize)
        {
        DWORD lcbNeeded = lpCallInfo->dwNeededSize;

        _ffree(lpCallInfo);
        lpCallInfo = (LPLINECALLINFO) _fmalloc(lcb);
        if (lpCallInfo == NULL)
            goto LOOM;
        lpCallInfo->dwTotalSize = lcbNeeded;

        /* try one more time */
        lResult = lineGetCallInfo(hcall,lpCallInfo);
        if (lResult < 0)
            goto LOOM;
        } /* while */

    return lpCallInfo;

LOOM:
    if (lpCallInfo != NULL)
        _ffree(lpCallInfo);
    return NULL;

} /* LpCallInfoFromHCall */

/* ***************************************************************************/
/* %%Function:LogUsingCall */
/* Uses lineGetCallInfo to extract logging info for call. We pass in whether
   the call is incoming/outgoing, begin and end time. Calls logCall() to put
   this info into log file and update log window if it is visible. */

void LogUsingCall(HCALL hcall,BOOL fIncoming,time_t tBegin,time_t tEnd)

{
    char szBlank[1] = {0};
    LPLINECALLINFO lpCallInfo = LpCallInfoFromHCall(hcall);
    LPSTR szName;
    LPSTR szDialString;

    if (lpCallInfo == NULL)
        return;

    /* get info out of lpCallInfo */
    if (fIncoming)
        {
        szName = (LPSTR) szBlank;
        szDialString = (LPSTR) szBlank;

        if (lpCallInfo->dwCallerIDFlags & LINECALLPARTYID_NAME)
            {
            if (lpCallInfo->dwCallerIDNameSize)
                szName = ((LPSTR)(lpCallInfo))+lpCallInfo->dwCallerIDNameOffset;

            } /* if */

        if (lpCallInfo->dwCallerIDFlags & LINECALLPARTYID_ADDRESS)
            {
            if (lpCallInfo->dwCallerIDSize)
                szDialString =
                    ((LPSTR)(lpCallInfo))+lpCallInfo->dwCallerIDOffset;
            } /* else if */
        } /* if */
    else
        {/* in the outgoing case, use info in vdcr */
        szDialString = vdcr.szDialStringOriginal;
        szName = vdcr.szCalledParty;
        } /* else */

    FLogCall(fIncoming,szName,szDialString,tBegin,tEnd);

    _ffree(lpCallInfo);

} /* LogUsingCall */

/* ***************************************************************************/
/* call monitoring functions */
/* ***************************************************************************/
/* %%Function:LpdmrAddMonitor */
/* add hcall to the list of calls we are currently monitoring(stored in
   vdts.lprgdmr). returns handle to this call's monitoring record if
   successful and NULL otherwise. */

LPDMR LpdmrAddMonitor(HCALL hcall)
{
    LPDMR lpdmr;
    LPLINECALLINFO lpCallInfo;

    if (vdts.idmrMac == vdts.idmrMax)
        {/* out of space, reallocate */
        LPDMR lprgdmr;
        size_t idmrMaxNew;

        idmrMaxNew = vdts.idmrMax + 32;
        lprgdmr = (LPDMR)_frealloc(vdts.lprgdmr,idmrMaxNew*sizeof(DMR));
        if (lprgdmr == NULL)
            return NULL;

        vdts.lprgdmr = lprgdmr;
        _fmemset(&((vdts.lprgdmr)[vdts.idmrMax]),mnsIdle,
                (idmrMaxNew-vdts.idmrMax)*sizeof(DMR));
        vdts.idmrMax = idmrMaxNew;
        } /* if */

    /* find the first empty slot */
    lpdmr = vdts.lprgdmr;
    while (lpdmr->mns != mnsIdle)
        lpdmr++;

    lpdmr->hcall = hcall;
    lpdmr->fConnected = FALSE;
    lpdmr->mns = mnsIncoming;
    if (lpCallInfo = LpCallInfoFromHCall(hcall))
        {
        if (lpCallInfo->dwOrigin == LINECALLORIGIN_OUTBOUND)
            lpdmr->mns = mnsOutgoing;

                lpdmr->hLine = lpCallInfo->hLine;

        _ffree(lpCallInfo);
        } /* if */
    vdts.idmrMac++;
    return lpdmr;

} /* LpdmrAddMonitor */

/* ***************************************************************************/
/* %%Function:LpdmrFindMonitor */
/* find the call specified by hcall and returns the corresponding lpdmr. NULL
   is returned if not found. */

LPDMR LpdmrFindMonitor(HCALL hcall)
{
    LPDMR lpdmr     = vdts.lprgdmr;
    LPDMR lpdmrLast = lpdmr + vdts.idmrMax - 1;

    for (; lpdmr <= lpdmrLast; ++lpdmr)
        {
        if (lpdmr->mns == mnsIdle)
            continue;

        if (lpdmr->hcall == hcall)
            return lpdmr;
        } /* for */

    return NULL;

} /* LpdmrFindMonitor */

/* ***************************************************************************/
/* %%Function:RemoveMonitor */
/* frees the DMR slot occupied by hcall. */

VOID RemoveMonitor(LPDMR lpdmr)
{
    lpdmr->mns = mnsIdle;
    vdts.idmrMac--;

} /* RemoveMonitor */

/* ***************************************************************************/
/* %%Function:NewCallState */
/* handles state transition for both calls we originate and calls we monitor.
   */

VOID NewCallState(DWORD newState,DWORD callbackInstance,DWORD privilege,
           HCALL hcall,DWORD dwParam2)
{
    LPDCR lpdcr;

    /* monitored call */
    if ((LPDMR)callbackInstance == NULL)
        {
        LPDMR lpdmr;

        /* get pointer to the monitoring record */
        if ((privilege & LINECALLPRIVILEGE_MONITOR) || (privilege
            & LINECALLPRIVILEGE_OWNER)) /* privilege != 0 iff this is new call
                                           to monitor */
            {
            lpdmr = LpdmrAddMonitor(hcall);
            if (lpdmr == NULL)
                lineDeallocateCall(hcall);
            else
                /* Default to mnsOutgoing until we see offering or accepted */
                lpdmr->mns = mnsOutgoing;
            } /* if */
        else if (privilege == 0)
            lpdmr = LpdmrFindMonitor(hcall);

        if (lpdmr == NULL) /* nothing more we can do */
            return;

        /* perfomr state transition */
        switch (newState)
            {
            case LINECALLSTATE_OFFERING:
            case LINECALLSTATE_ACCEPTED:
                {
                lpdmr->mns = mnsIncoming;
                break;
                }
            case LINECALLSTATE_DIALING:
            case LINECALLSTATE_DIALTONE:
            case LINECALLSTATE_PROCEEDING:
                {
                lpdmr->mns = mnsOutgoing;
                break;
                }
            case LINECALLSTATE_CONNECTED:
                {
                lpdmr->fConnected = TRUE;
                lpdmr->tBegin = time(NULL);
                break;
                }
            case LINECALLSTATE_IDLE:
                {
                if (lpdmr->fConnected)
                    {
                    lpdmr->tEnd = time(NULL);
                    LogUsingCall(lpdmr->hcall,lpdmr->mns == mnsIncoming,
                            lpdmr->tBegin,lpdmr->tEnd);
                    } /* if */
                RemoveMonitor(lpdmr);
                lineDeallocateCall(lpdmr->hcall);
                DoChangeOptionDlg();
                break;
                }
            } /* switch */

        return;
        } /* if */

    /* the call we originated */
    lpdcr = (LPDCR)callbackInstance;
    if (lpdcr->lcs == newState)
        return;

    lpdcr->lcs = newState;
    switch (newState)
        {
        case LINECALLSTATE_CONNECTED:
            {
            if (lpdcr->fAlreadyConnected)  /* raid #865 */
                break;

            UpdateCallStatusDlg(TRUE,lpdcr->szCalledParty,
                    lpdcr->szDialStringOriginal);
            lpdcr->fAlreadyConnected = TRUE;
            break;
            }
        case LINECALLSTATE_DISCONNECTED:
            {
            BOOL fCallOK = (dwParam2 == LINEDISCONNECTMODE_NORMAL
                || dwParam2 == LINEDISCONNECTMODE_UNKNOWN
                || dwParam2 == LINEDISCONNECTMODE_PICKUP
                || dwParam2 == LINEDISCONNECTMODE_FORWARDED
                || dwParam2 == LINEDISCONNECTMODE_UNAVAIL);

            if (fCallOK)
                UpdateCallStatusDlg(FALSE,lpdcr->szCalledParty,
                        lpdcr->szDialStringOriginal);

            FDropCall(lpdcr);

            if (!fCallOK)
                {/* need to do it after FDropCall! */
                HideCallStatusDlg();
                DisPlayDisconnectedErrorDlg(dwParam2);
                } /* if */
            break;
            }
        case LINECALLSTATE_BUSY:
            {
            NewCallState(LINECALLSTATE_DISCONNECTED,callbackInstance,privilege,
                    hcall,LINEDISCONNECTMODE_BUSY);
            break;
            }
        case LINECALLSTATE_SPECIALINFO:
            {
            NewCallState(LINECALLSTATE_DISCONNECTED,callbackInstance,privilege,
                    hcall,LINEDISCONNECTMODE_UNREACHABLE);
            break;
            }
        case LINECALLSTATE_IDLE:
            {
            HideCallStatusDlg();

            if (vdcr.lErrCode != errNone)
                if (vdcr.lErrCode == LINEERR_CALLUNAVAIL)
                    DisplayLineInUseDlg();
                else
                    DialerErrMessageBox(IkszFromErrCode(vdcr.lErrCode),MB_ICONEXCLAMATION | MB_OK);

            if (lpdcr->mcs == mcsDropping)
                DoLineClose(lpdcr);

            lineDeallocateCall(lpdcr->hCall);
            lpdcr->mcs = mcsIdle;
            DisableDialerDesktop(FALSE);
            break;
            }
        } /* switch */

} /* NewCallState */

/* ***************************************************************************/
/* %%Function:LineCallBackProc */
/* This is our tapi line call back function. */

void FAR PASCAL _export LineCallBackProc(DWORD hDevice,DWORD dwMessage,
           DWORD dwInstance,DWORD dwParam1,DWORD dwParam2,DWORD dwParam3)
{
    switch (dwMessage)
        {
        case LINE_LINEDEVSTATE:
            {
            if (dwParam1 & LINEDEVSTATE_REINIT)
                vdts.fReInitTapi = TRUE;

            if (dwParam1 & LINEDEVSTATE_TRANSLATECHANGE)
                ErrInitCallingCardInfo(HInstDialer());

            if (dwParam1 & LINEDEVSTATE_CAPSCHANGE)
                {
                DWORD iLine = vdts.cLines;
                HLINE hLine = (HLINE)hDevice;

                /* search for the handle of the line in vdts.lprgLineInfo */
                while (--iLine >= 0)
                    if (vdts.lprgLineInfo[iLine].hLine == hLine)
                        break;

                if (iLine < 0)
                    break;

                if (GetCachedLineInfo(HInstDialer(),iLine) == errNone
                                        && iLine == vdts.iLineCur
                    && vdts.lprgLineInfo[iLine].fIsVoiceLine)
                    break;

                /* now find the first voice line and set it to be the current line */
                for (iLine = 0; iLine < vdts.cLines; ++iLine)
                    if (vdts.lprgLineInfo[iLine].fIsVoiceLine)
                        {
                        vdts.iLineCur = iLine;
                                                vdts.iAddrCur = 0;  /* assume first address on change */
                        break;
                        } /* if */

                if (iLine == vdts.cLines)
                                        {
                                        vdts.iLineCur = (DWORD)-1;
                                        vdts.iAddrCur = 0;
                                        }
                } /*  CAPSCHANGE */

            break;
            }
        case LINE_ADDRESSSTATE:
            {
                        LPLINEADDRESSCAPS lpAddrCaps;
                char szAddrName[cchAddrNameMac];
                LPSTR lpszAddrName;
                        DWORD lcbNeeded = lcbAddrDevCapsInitial;

            if (vdts.iLineCur == (DWORD)-1
                                || (HLINE)hDevice != vdts.lprgLineInfo[vdts.iLineCur].hLine
                || dwParam1 != vdts.iAddrCur
                                || (dwParam2 & LINEADDRESSSTATE_CAPSCHANGE) == 0)
                break;

                        while (TRUE) /* loop until success or failure */
                                {
                        if ((lpAddrCaps = (LPLINEADDRESSCAPS)_fmalloc((size_t)lcbNeeded)) == NULL)
                                        break;
                        lpAddrCaps->dwTotalSize = lcbNeeded;
                    if ((lineGetAddressCaps(vdts.hApp,vdts.iLineCur,vdts.iAddrCur,
                                        vdts.lprgLineInfo[vdts.iLineCur].dwAPIVersion,0,lpAddrCaps)) != errNone)
                                        {
                                        _ffree(lpAddrCaps);
                                        lpAddrCaps = NULL;
                                        break;
                                        }
                                if (lpAddrCaps->dwNeededSize <= lpAddrCaps->dwTotalSize)
                                        break;
                                else
                                        {
                                        lcbNeeded = lpAddrCaps -> dwNeededSize;
                                        _ffree(lpAddrCaps);
                                        }
                    } /* while */

                        if (!lpAddrCaps)
                                break;

                /* get the address's name */
                if (lpAddrCaps->dwAddressSize > 0)
                    lpszAddrName = (LPSTR)(lpAddrCaps)+lpAddrCaps->dwAddressOffset;
                else
                    {/* use default name */
                    char szAddrFormat[32];

                    LoadString(HInstDialer(),ikszCallLogAddrName,szAddrFormat,sizeof(szAddrFormat));
                    wsprintf(szAddrName,szAddrFormat,vdts.iAddrCur);
                    lpszAddrName = (LPSTR)szAddrName;
                    } /* else */

            SetDialerProfileString(HInstDialer(),ikszSecPreference,ikszFieldPreferedAddress,lpszAddrName);
                        _ffree (lpAddrCaps);
                        break;
            }

        /* process state transition */
        case LINE_CALLSTATE:
            {
            NewCallState(dwParam1,dwInstance,dwParam3,(HCALL)hDevice,dwParam2);
            break;
            }
        case LINE_CREATE:
            {/* a new line is created */
            LPLINEINFO lpli;
                        DWORD iNewLine = dwParam1;  /* dwParam1 indicates new device ID */

                        if (iNewLine >= vdts.cLines)
                                {
                                DWORD iLine;
                                /* need to allocate space for new line */
                                lpli = (LPLINEINFO) _frealloc(vdts.lprgLineInfo,sizeof(LINEINFO)*(int)(iNewLine + 1));
                if (lpli == NULL)
                        break;
                                /* zero out newly-allocated array entries */
                vdts.lprgLineInfo = lpli;
                                for (iLine = vdts.cLines; iLine <= iNewLine; ++iLine)
                                        _fmemset(&(vdts.lprgLineInfo[iLine]),0,sizeof(LINEINFO));
                                vdts.cLines = iNewLine + 1;
                                }

            if (GetCachedLineInfo(HInstDialer(),iNewLine) == errNone)
                                {
                lineSetStatusMessages (vdts.lprgLineInfo[iNewLine].hLine,
                                       LINEDEVSTATE_CAPSCHANGE,
                                       LINEADDRESSSTATE_CAPSCHANGE);
                                if (vdts.iLineCur == (DWORD)-1
                                        && vdts.lprgLineInfo[iNewLine].fIsVoiceLine)
                                        {
                                        vdts.iLineCur = iNewLine;
                                        vdts.iAddrCur = 0;
                                        }
                                }
            break;
            }
        case LINE_CLOSE:
            {
                        // see if the closed line is the line on which we have an active call
                        // if it is, do what would have been done if that call went idle

                        if (vdcr.mcs != mcsIdle
                                && vdcr.hLine == (HLINE)hDevice)
                                {
                vdcr.mcs = mcsIdle;
                        vdcr.lcs = LINECALLSTATE_IDLE;
                HideCallStatusDlg();
                DialerErrMessageBox(IkszFromErrCode(errLineClose),MB_ICONEXCLAMATION | MB_OK);
                                vdcr.hLine = NULL;
                        if (vdcr.fIsMakeCallRequest)
                                ActivateNextTask();
                DisableDialerDesktop(FALSE);
                break;
                                }

                        // if not, see if it is a line for which we have a monitoring handle
                        // if it is, mark that line as closed

                    {
                    DWORD iLine = vdts.cLines;

                    while (--iLine >= 0)
                        if (vdts.lprgLineInfo[iLine].hLine == (HLINE)hDevice)
                                                {
                                                vdts.lprgLineInfo[iLine].hLine == NULL;
                            break;
                                                }
                    }

                        // if any calls are being monitored on that line, stop monitoring

                                {
                            LPDMR lpdmr     = vdts.lprgdmr;
                            LPDMR lpdmrLast = lpdmr + vdts.idmrMax - 1;

                            for (; lpdmr <= lpdmrLast; ++lpdmr)
                                if (lpdmr->mns != mnsIdle
                                                && lpdmr->hLine == (HLINE)hDevice)
                                                RemoveMonitor(lpdmr);
                                }

            break;
            }


        /* handle simple tapi request. */
        case LINE_REQUEST:
            {
            if (dwParam1 == LINEREQUESTMODE_MAKECALL)
                                {
                vdts.fCheckMakeCallRequest = TRUE;
                PostMessage(HwndDialerMain(),WM_USER,0,0);
                                }
            break;
            }
        /* handle the assync completion of TAPI functions
           lineMakeCall/lineDropCall */
        case LINE_REPLY:
            {
            if (vdcr.requestID != dwParam1)
                break;

            if (vdcr.mcs == mcsMaking || vdcr.mcs == mcsStopMaking)
                {/* reply message for lineMakeCall */
                if (dwParam2 != errNone)
                    {
                    DoLineClose(&vdcr);
                    HideCallStatusDlg();

                    if (dwParam2 == LINEERR_CALLUNAVAIL)
                        DisplayLineInUseDlg();
                    else
                        DialerErrMessageBox(IkszFromErrCode(dwParam2),MB_ICONEXCLAMATION | MB_OK);

                    vdcr.mcs = mcsIdle;
                    } /* if */
                else
                    {
                    MCS mcsOld = vdcr.mcs;

                    vdcr.mcs = mcsMade;
                    if (mcsOld == mcsStopMaking)
                        FDropCall(&vdcr);
                    } /* else */
                } /* if */
            break;
            }
        /* other messages that can be processed */
        case LINE_CALLINFO:
            break;
        case LINE_DEVSPECIFIC:
            break;
        case LINE_DEVSPECIFICFEATURE:
            break;
        case LINE_GATHERDIGITS:
            break;
        case LINE_GENERATE:
            break;
        case LINE_MONITORDIGITS:
            break;
        case LINE_MONITORMEDIA:
            break;
        case LINE_MONITORTONE:
            break;
        } /* switch */

} /* LineCallBackProc */


/*  ***************************************************************************/
/*  %%Function:LDialerLineTranslateAddress */
/*  Internal lineTranslateAddress proc. that handles the LINEERR_INIFILECORRUPT
    error by calling the dial helper and then trying lineTranslateAddress
    once more. */

LONG LDialerLineTranslateAddress(
                HLINEAPP hLineApp,
                DWORD dwDeviceId,
                DWORD dwApiVersion,
                LPCSTR lpszAddressIn,
                DWORD dwCard,
                DWORD dwTranslateOptions,
                LPLINETRANSLATEOUTPUT lpLineTranslateOutput)
{
    LONG lResult;
    BOOL fBreak=FALSE;

    while (TRUE)
        {
        if ((lResult = lineTranslateAddress(hLineApp, dwDeviceId, dwApiVersion,
                                        lpszAddressIn, dwCard, dwTranslateOptions,
                                        lpLineTranslateOutput)) != LINEERR_INIFILECORRUPT)
            break;

        if (fBreak)
            break;

        if ((lResult = CallConfigDlg(HwndFromVdws(), lpszAddressIn)) != errNone)
            break;

        fBreak = !fBreak;
        }

    return lResult;

}   /* LDialerLineTranslateAddress */


/*  ***************************************************************************/
/*  %%Function:LDialerLineGetTranslateCaps */
/*  Internal lineGetTranslateCaps proc. that handles the LINEERR_INIFILECORRUPT
    error by calling the dial helper and then trying lineGetTranslateCaps
    once more. */

LONG LDialerLineGetTranslateCaps(
                HLINEAPP hLineApp,
                DWORD dwApiVersion,
                LPLINETRANSLATECAPS lpLineTranslateCaps)
{
    LONG lResult;
    BOOL fBreak=FALSE;
    int  nLooper;

#if 0
    while (TRUE)  BAD IDEA (Contributed to bug 1142)
    should loop, say, 10 times...
#else
    for ( nLooper = 0; nLooper < 3; nLooper++)
#endif
        {
        if ((lResult = lineGetTranslateCaps(hLineApp, dwApiVersion,
                                        lpLineTranslateCaps)) != LINEERR_INIFILECORRUPT)
            break;

#ifdef DEBUG
        OutputDebugString((" lineGetTranslateCaps claims LINEERR_INIFILECORRUPT - DIALER\r\n"));
#endif

        if (fBreak)
            break;

        if ((lResult = CallConfigDlg(HwndFromVdws(), NULL)) != errNone)
            break;

        fBreak = !fBreak;
        }

    return lResult;

}   /* LDialerLineGetTranslateCaps */
