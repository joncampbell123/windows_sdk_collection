/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  MCICMDS.C
 *
 *  Sample MCI Device Driver
 *
 *      MCI Command Message Procedures
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "mciqa.h"
#include "qasample.h"

#ifdef DEBUG
int nDebugOutput;               /* non-zero for debug output enabled */

/****************************************************************************
 * Unformatted output to the debug port prefaced by the driver name and
 * followed by a line feed.
 ***************************************************************************/
void NEAR dout(PSTR szOutput)
{
    OutputDebugString(MCINAME);
    OutputDebugString(szOutput);
    OutputDebugString("\r\n");
}

/****************************************************************************
 * Formatted output to the debug port prefaced by the driver name and
 * followed by a line feed.
 *
 * This routine must have '#ifdef DEBUG' around it when called
 * to avoid allocating string space in the RETAIL version
 ***************************************************************************/
void NEAR dprintf(PSTR szFormat, ...)
{
char buf[128];
    
    if (nDebugOutput == 0)
        return;

    wvsprintf(buf, szFormat, (LPSTR)(&szFormat + 1));
    OutputDebugString(MCINAME);
    OutputDebugString(buf);
    OutputDebugString("\r\n");
}

#endif  /* #ifdef DEBUG */

/* Never a valid position for this device */
#define INVALID_POSITION    0xFFFFFFFF

/* Never a valid Windows timer ID */
#define INVALID_TIMER_ID    0

/* Returns TRUE if the given channel has a running timer */
#define ACTIVE_TIMER(nChannel)  (DriverData[nChannel].wTimerID != \
                                 INVALID_TIMER_ID)

/* Procedure instance address of timer procedure */
static FARPROC lpfnTimerProc;

/* Information kept for each channel */
typedef struct {
    int     nUseCount;          /* Incremented for each shared open */
    BOOL    fShareable;         /* TRUE if first open was shareable */
    DWORD   dwMode;             /* Not used */
    WORD    wNotifyDeviceID;    /* MCI device ID with a pending notification */
    WORD    wTimerID;           /* ID of any active timer or INVALID_TIMER_ID */
    DWORD   dwPosition;         /* Not used */
    DWORD   dwTimeFormat;       /* Not used */
    DWORD   dwToPosition;       /* 'to' position of last play command or */
                                /* INVALID_POSITION */
    HANDLE  hCallback;          /* Callback handle for pending notification */
} DriverDataRecord;

/*  This driver allows a fixed number of channels to be active.  The
 *  channel number is specified as a parameter on the device line in
 *  the [mci] section of SYSTEM.INI.
 */
DriverDataRecord DriverData[MCIQA_MAX_CHANNELS];

/****************************************************************************
 *
 * mqInitializeChannel: Reset all vital information for this channel
 *
 * Params:  nChannel    Channel to use
 *
 ***************************************************************************/
void mqInitializeChannel(int nChannel)
{
    /* Allows the channel to be opened */
    DriverData[nChannel].nUseCount = 0;

    /* Indicates no active timer (notification) */
    DriverData[nChannel].wTimerID = INVALID_TIMER_ID;

    /* Indicates no previous 'to' position */
    DriverData[nChannel].dwToPosition = INVALID_POSITION;
}

/****************************************************************************
 *
 * mqKillTimer: Kill and clear the timer for the indicated channel
 *
 * Params:  nChannel    Channel to use
 *
 ***************************************************************************/
void mqKillTimer(int nChannel)
{
    KillTimer (NULL, DriverData[nChannel].wTimerID);
    DriverData[nChannel].wTimerID = INVALID_TIMER_ID;
}

#ifdef DEBUG
/****************************************************************************
 * Print notification information to debug output
 ***************************************************************************/
void notify_debugout(WORD wDeviceID, WORD wNotifyStatus)
{
char *pszStatus;
    
    switch (wNotifyStatus) {

        case MCI_NOTIFY_SUCCESSFUL:
            pszStatus = "Successful";
            break;

        case MCI_NOTIFY_SUPERSEDED:
            pszStatus = "Superseded";
            break;

        case MCI_NOTIFY_ABORTED:
            pszStatus = "Aborted";
            break;

        case MCI_NOTIFY_FAILURE:
            pszStatus = "Failure";
            break;

    }
    dprintf ("Send Device ID %d Notify %s", wDeviceID, (LPSTR)pszStatus);
}
#endif

/****************************************************************************
 *
 * mqCancelNotify:   Kill and clear any timer for the indicate channel
 * and abort the pending notification
 *
 * Params:  nChannel        Channel to cancel
 *          wNotifyStatus   Notification status to return
 *
 ***************************************************************************/
void mqCancelNotify(int nChannel, WORD wNotifyStatus)
{
    if (ACTIVE_TIMER (nChannel)) {
        mqKillTimer (nChannel);
#ifdef DEBUG
        notify_debugout (DriverData[nChannel].wNotifyDeviceID, wNotifyStatus);
#endif
        mciDriverNotify (DriverData[nChannel].hCallback,
                         DriverData[nChannel].wNotifyDeviceID, wNotifyStatus);
    }
}

/****************************************************************************
 *
 * mqDriverNotify:  Function to post mm_mcinotify message if notification
 * has been requested. Used when command is actually synchronous
 *
 * Params:  wDeviceID       MCI device id.
 *          nChannel        Driver channel
 *          dwFlags         only notifiy flag applies.
 *          lpParms         std. MCI parameter block
 *          wNotifyStatus   notification status
 *
 ***************************************************************************/
void NEAR PASCAL mqDriverNotify(WORD wDeviceID, int nChannel, DWORD dwFlags, LPMCI_GENERIC_PARMS lpParms, WORD wNotifyStatus)
{
    /* If the notify flag was specified and if the callback handle is not 0 */
    if ((dwFlags & MCI_NOTIFY) && LOWORD(lpParms->dwCallback)) {
        mqCancelNotify (nChannel, MCI_NOTIFY_SUPERSEDED);
#ifdef DEBUG
        notify_debugout (wDeviceID, wNotifyStatus);
#endif
        mciDriverNotify (LOWORD(lpParms->dwCallback), wDeviceID,
                         wNotifyStatus);
    }
}

/****************************************************************************
 *
 * mqMCIOpenDriver:    Perform driver initialization
 *
 * Params:  nChannel    Device channel
 *          dwFlags     Open flags
 *          lpOpen      Open paramaters
 *
 * Returns:  MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIOpenDriver(int nChannel, DWORD dwFlags, LPMCI_OPEN_PARMS lpOpen)
{
#ifdef DEBUG
    /* Set debug output on if the 'mciqa' keyname in the [mmsystem] section */
    /* of SYSTEM.INI is set */
    nDebugOutput = GetPrivateProfileInt ("mmsystem", "mciqa", 0, "system.ini");
#endif

    dout ("MCI_OPEN_DRIVER message");

    if (dwFlags & MCI_OPEN_ELEMENT)
        return MCIERR_NO_ELEMENT_ALLOWED;

    if (DriverData[nChannel].nUseCount > 0) {
    /* The driver already open on this channel */

        /* If the driver was opened shareable before and this open specifies */
        /* shareable then increment the use count */
        if (DriverData[nChannel].fShareable &&
            dwFlags & MCI_OPEN_SHAREABLE)

            ++DriverData[nChannel].nUseCount;

        /* Otherwise the device cannot be opened */
        else
            return MCIERR_MUST_USE_SHAREABLE;

    }
    else {
    /* This channel is not already open */

        DriverData[nChannel].nUseCount = 1;
        DriverData[nChannel].fShareable = (dwFlags & MCI_OPEN_SHAREABLE) ? TRUE : FALSE;
        qsOpen (nChannel);
    }

    mciSetDriverData (lpOpen->wDeviceID, nChannel);

    return 0;
}

/****************************************************************************
 *
 * mqMCICloseDriver:   Perform driver termination
 *
 * Params:  nChannel    Channel to close
 *
 ***************************************************************************/
void NEAR PASCAL mqMCICloseDriver(int nChannel, WORD wDeviceID)
{
    dout("MCI_CLOSE_DRIVER message");
    mqCancelNotify (nChannel, MCI_NOTIFY_ABORTED);
    if (--DriverData[nChannel].nUseCount == 0) {
        qsClose (nChannel);

        if (wDeviceID == DriverData[nChannel].wNotifyDeviceID)
            mqKillTimer (nChannel);

        mqInitializeChannel (nChannel);
    }
}

/****************************************************************************
 *
 * mqSetDWTimer:    Sets a timer for the minimum of 0xFFFF and dwDelay
 *                  milliseconds
 *
 * Params:  nChannel    Channel to use
 *          dwDelay     Delay in milliseconds
 *
 * Returns: wTimerID if successful or 0 on failure
 *
 ***************************************************************************/
WORD mqSetDWTimer(int nChannel, DWORD dwDelay)
{
    return SetTimer (NULL, 0, dwDelay > 0xFFFF ? 0xFFFF : (WORD)dwDelay,
                     lpfnTimerProc);
}

/****************************************************************************
 * TimerProc:  Standard Windows timer procedure to handle notification
 * on play commands
 ***************************************************************************/
void FAR PASCAL TimerProc(HWND hWnd, unsigned iMessage, WORD wTimerID, DWORD dwSystemTime)
{
int   nChannel;
WORD  wStatus, wMode;
DWORD dwPosition;

    /* What channel does this timer belong to? */
    for (nChannel = 0; nChannel < MCIQA_MAX_CHANNELS; ++nChannel)

        /* The timer is for this channel */
        if (DriverData[nChannel].wTimerID == wTimerID) {

            /* Stop the timer */
            mqKillTimer (nChannel);

            /* Where are we? */
            dwPosition = qsCurrentPosition (nChannel);

            /* If the device is still playing or is paused */
            wMode = qsCurrentMode (nChannel);
            if (wMode == QS_PLAYING || wMode == QS_PAUSED)

                /* Reset the timer and try again (the delay was */
                /* probably > 0xFFFF) */
                DriverData[nChannel].wTimerID = mqSetDWTimer (nChannel,
                              DriverData[nChannel].dwToPosition - dwPosition);
            else {
            /* The device is not playing */

                /* Device arrived OK so indicate success */
                if (dwPosition == DriverData[nChannel].dwToPosition)
                    wStatus = MCI_NOTIFY_SUCCESSFUL;

                /* Device did not arrive at 'to' position so indicate failure */
                else
                    wStatus = MCI_NOTIFY_FAILURE;

#ifdef DEBUG
               /* Notify the application */
                notify_debugout (DriverData[nChannel].wNotifyDeviceID, wStatus);
#endif
                mciDriverNotify (DriverData[nChannel].hCallback,
                                DriverData[nChannel].wNotifyDeviceID, wStatus);
            }
        }
}

/****************************************************************************
 *
 * mqInitialize:    Do MCI device driver initializeation
 *
 * Return:  TRUE on success
 *
 ***************************************************************************/
BOOL mqInitialize ()
{
int n;

    /* Make callable of timer procedure */
    lpfnTimerProc = MakeProcInstance ((FARPROC)TimerProc, hModuleInstance);

    /* Initialize device data table */
    for (n = 0; n < MCIQA_MAX_CHANNELS; ++n)
        mqInitializeChannel (n);

    return TRUE;
}

/****************************************************************************
 *
 * mqNotifyAt:  Set up to notify at the given position
 *
 * Params:  nChannel    Channel number to use
 *          dwTo        Position
 *          hCallback   Callback window handle
 *
 ***************************************************************************/
void mqNotifyAt(int nChannel, WORD wDeviceID, DWORD dwTo, HANDLE hCallback)
{
DWORD dwCurrentPosition;

    dwCurrentPosition = qsCurrentPosition (nChannel);

    if (dwCurrentPosition > dwTo)
        return;

    DriverData[nChannel].hCallback = hCallback;
    DriverData[nChannel].wNotifyDeviceID = wDeviceID;

    if ((DriverData[nChannel].wTimerID =
            mqSetDWTimer (nChannel, dwTo - dwCurrentPosition))
         == INVALID_TIMER_ID)

        return;
}

/****************************************************************************
 *
 * mqWaitUntil:  Do not return until the given position is reached
 *
 * Params:  nChannel    Channel number to use
 *          dwTo        Position
 *
 * Returns: Notification status to use
 *
 ***************************************************************************/
WORD mqWaitUntil(int nChannel, WORD wDeviceID, DWORD dwTo)
{
/* We would like to have some sort of lower level operating system */
/* delay function here but instead must loop */

    /* Loop while the device is playing */
    while (qsCurrentMode (nChannel) == QS_PLAYING) {
        /*  mciDriverYield() calls the normal Windows Yield() procedure and
         *  checks for any application generated cancel condition such as a
         *  break key
         */
        if (mciDriverYield (wDeviceID)) {
            /* The application has generated a cancel condition so cancel */
            /* any notification on this play command */
            mqCancelNotify (nChannel, MCI_NOTIFY_ABORTED);
            return MCI_NOTIFY_ABORTED;
        }
    }

    if (qsCurrentPosition (nChannel) == dwTo)
        return MCI_NOTIFY_SUCCESSFUL;
    else
        return MCI_NOTIFY_FAILURE;
}

/****************************************************************************
 *
 * mqMCIPlay:  Setup to play wave file
 *
 * Params:  nChannel    Channel number to play
 *          dwFlags     play flags
 *          lpPlay      play parameters
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD mqMCIPlay(int nChannel, WORD wDeviceID, DWORD dwFlags, LPMCI_PLAY_PARMS lpPlay)
{
DWORD dwFrom, dwTo, dwLen;
WORD wNotifyStatus, wErr = MCIERR_NO_ERROR;

    dout("MCI_PLAY message");

    dwLen = qsMediaLength(nChannel);

    if (dwFlags & MCI_FROM) {
        dwFrom = lpPlay->dwFrom;
        if (dwFrom > dwLen)
            return MCIERR_OUTOFRANGE;
    }
    else
        dwFrom = qsCurrentPosition (nChannel);


    if (dwFlags & MCI_TO) {
        dwTo = lpPlay->dwTo;
        if (dwTo < dwFrom || dwTo > dwLen)
            return MCIERR_OUTOFRANGE;
    }
    else
        dwTo = dwLen;

    /* If there was a 'from' position or a new 'to' position then cancel */
    /* any pending notificiation */
    if (dwFlags & MCI_FROM ||
        (DriverData[nChannel].dwToPosition != INVALID_POSITION &&
         DriverData[nChannel].dwToPosition != dwTo))

        mqCancelNotify (nChannel, MCI_NOTIFY_ABORTED);

    DriverData[nChannel].dwToPosition = dwTo;

#ifdef DEBUG
    dprintf("   From:%08lX To:%08lX", dwFrom, dwTo);
#endif

    if (!qsPlay (nChannel, dwFrom, dwTo))
        return MCIERR_HARDWARE;

    if (dwFlags & MCI_WAIT) {
        /* Wait until device stops or application cancels */
        wNotifyStatus = mqWaitUntil (nChannel, wDeviceID, dwTo);

        /* Generate any notification */
        mqDriverNotify (wDeviceID, nChannel, dwFlags,
                        (LPMCI_GENERIC_PARMS)lpPlay, wNotifyStatus);
        if (wNotifyStatus == MCI_NOTIFY_FAILURE)
            wErr = MCIERR_HARDWARE;

    }
    else if (dwFlags & MCI_NOTIFY) {
        /* Set up for delayed notification */
        mqCancelNotify (nChannel, MCI_NOTIFY_SUPERSEDED);
        mqNotifyAt (nChannel, wDeviceID, dwTo, (HANDLE)lpPlay->dwCallback);
    }

    return wErr;
}

/****************************************************************************
 *
 * mqMCIStop:  Stop playing
 *
 * Params:  nChannel    Channel number to stop
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIStop(int nChannel)
{
    dout("MCI_STOP message");
    if (!qsStop (nChannel))
        return MCIERR_HARDWARE;

    mqCancelNotify (nChannel, MCI_NOTIFY_ABORTED);
    return MCIERR_NO_ERROR;
}

/****************************************************************************
 *
 * mqMCIPause:  Pause
 *
 * Params:  nChannel    Channel number to pause
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIPause(int nChannel)
{
    dout("MCI_PAUSE message");
    if (!qsPause (nChannel))
        return MCIERR_HARDWARE;
    else
        return MCIERR_NO_ERROR;
    return(MCIERR_NO_ERROR);
}

/****************************************************************************
 *
 * mqMCICue:  Cue
 *
 * Params:  nChannel    Channel number to cue
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCICue(int nChannel)
{
    dout("MCI_CUE message");
    return(MCIERR_NO_ERROR);
}

/****************************************************************************
 *
 * mqMCISeek:  Seek
 *
 * Params:  nChannel    Channel number to cue
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCISeek(int nChannel, DWORD dwFlags, LPMCI_SEEK_PARMS lpSeek)
{
    dout("MCI_SEEK message");

    if (dwFlags & MCI_TO) {
        /* seek to lpSeek->dwTo */
        if (lpSeek->dwTo > qsMediaLength (nChannel))
            return MCIERR_OUTOFRANGE;

        if (!qsSeek (nChannel, lpSeek->dwTo)) {
#ifdef DEBUG            
            dprintf(" To:%08lX", lpSeek->dwTo);
#endif

            return MCIERR_HARDWARE;
        }
    }
    else if (dwFlags & MCI_SEEK_TO_START) {
        if (!qsSeek (nChannel, 0)) {
#ifdef DEBUG
            dprintf(" To:%08lX", 0);
#endif

            return MCIERR_HARDWARE;
        }
    }
    else if (dwFlags & MCI_SEEK_TO_END) {
        if (!qsSeek (nChannel, qsMediaLength (nChannel))) {
#ifdef DEBUG
            dprintf(" To:%08lX", qsMediaLength(nChannel));
#endif

            return MCIERR_HARDWARE;
        }
    } else
            return MCIERR_MISSING_PARAMETER;

    mqCancelNotify (nChannel, MCI_NOTIFY_ABORTED);

    return MCIERR_NO_ERROR;
}

/****************************************************************************
 *
 * mqMCIStatus: Respond to a status command
 *
 * Params:  nChannel    Channel number for status
 *          dwFlags     flags
 *          lpStatus    status parameters
 *
 * Returns: MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIStatus (int nChannel, DWORD dwFlags, LPMCI_STATUS_PARMS lpStatus)
{
DWORD dwRes;
LPQS_INFO lpInfo;

    dout("MCI_STATUS message");

    if ((lpInfo = GAllocPtr (sizeof(QS_INFO))) == NULL)
        return MCIERR_OUT_OF_MEMORY;

    if (!qsInfo (nChannel, lpInfo)) {
        dwRes = MCIERR_DRIVER_INTERNAL;
        goto cleanup;
    }

    if (dwFlags & MCI_STATUS_ITEM)

        switch (lpStatus->dwItem) {

            case MCI_STATUS_POSITION:

                if (dwFlags & MCI_TRACK) {
                    if (lpStatus->dwTrack == 1) {
                        lpStatus->dwReturn = 0;
                        dwRes = MCIERR_NO_ERROR;
                    }
                    else
                        dwRes = MCIERR_OUTOFRANGE;
                }
                else if (dwFlags & MCI_STATUS_START) {
                    lpStatus->dwReturn = 0;
                    dwRes = MCIERR_NO_ERROR;
                }
                else {
                    dwRes = MCIERR_NO_ERROR; 
                    lpStatus->dwReturn =  lpInfo->dwPosition;
                }
                break;

            case MCI_STATUS_LENGTH:
                if ((dwFlags & MCI_TRACK) && (lpStatus->dwTrack != 1))
                    return MCIERR_OUTOFRANGE;

                lpStatus->dwReturn = lpInfo->dwLength;
                dwRes = MCIERR_NO_ERROR;
                break;

            case MCI_STATUS_CURRENT_TRACK:
                lpStatus->dwReturn = 1;
                dwRes = MCIERR_NO_ERROR;
                break;

            case MCI_STATUS_NUMBER_OF_TRACKS:
                lpStatus->dwReturn = 1;
                dwRes = MCIERR_NO_ERROR;
                break;

            case MCI_STATUS_READY:

/*  The MAKEMCIRESOURCE macro is used to return both a WORD value to
 *  be used by the application programing making this call and a
 *  string resource ID value to be used by the MCI core to look up
 *  a string value to be returned to the called of mciSendString.
 *  This second value has no meaning and is not available to callers
 *  of mciSendCommand
 */
                lpStatus->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);

/*  Indicates that the high word of the return value contains a string
 *  resource ID.  The resource is a standard string from MMSYSTEM.DLL.
 *  To indicate that the resource ID refers to a custom string contained in
 *  the driver module (MCIQA.DLL) bitwise or the MCI_RESOURCE_DRIVER
 *  flag in with MCI_RESOURCE_RETURNED.
 */

                dwRes = MCI_RESOURCE_RETURNED;
                break;

            case MCI_STATUS_MODE:

            { int n;

                switch (lpInfo->nMode) {

                    case QS_STOPPED:
                        n = MCI_MODE_STOP;
                        break;
                    case QS_PLAYING:
                        n = MCI_MODE_PLAY;
                        break;
                    case QS_PAUSED:
                        n = MCI_MODE_PAUSE;
                        break;
                    default:
                        n = MCI_MODE_NOT_READY;
                        break;
                }
                lpStatus->dwReturn = MAKEMCIRESOURCE (n, n);
                dwRes = MCI_RESOURCE_RETURNED;
                break;
            }

            case MCI_STATUS_TIME_FORMAT:
                lpStatus->dwReturn =
                    MAKEMCIRESOURCE(MCI_FORMAT_MILLISECONDS,
                                    MCI_FORMAT_MILLISECONDS_S);
                dwRes = MCI_RESOURCE_RETURNED;
                break;
                
            default:
                dwRes = MCIERR_MISSING_PARAMETER;
        }
    else
        dwRes = MCIERR_MISSING_PARAMETER;

cleanup:;
    GFreePtr (lpInfo);
    return (dwRes);
}

/****************************************************************************
 *
 * mqMCIInfo:  Respond to MCI_INFO message
 *
 * Parms:   dwFlags     flags
 *          lpInfo      info parameters
 *
 * Return:  MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIInfo(DWORD dwFlags, LPMCI_INFO_PARMS lpInfo)
{
DWORD dwRes = MCIERR_NO_ERROR;

    dout("MCI_INFO message");

    if (dwFlags & MCI_INFO_PRODUCT && lpInfo->lpstrReturn != NULL)
        lstrcpy (lpInfo->lpstrReturn, MCIQA_PRODUCT);
    else
        dwRes = MCIERR_MISSING_PARAMETER;

    return dwRes;
}

/****************************************************************************
 *
 * mqMCISet:   Respond to the MCI_SET message
 *
 * Parms:   nChannel    Channel number
 *          dwFlags     flags
 *          lpSet       set parameters
 *
 * Return:  MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCISet(WORD nChannel, DWORD dwFlags, LPMCI_WAVE_SET_PARMS lpSet)
{
DWORD dwRes;
    
    dout("MCI_SET message");

    if (dwFlags & MCI_SET_TIME_FORMAT) {
        DriverData[nChannel].dwTimeFormat = lpSet->dwTimeFormat;
        dwRes = MCIERR_NO_ERROR;
    }
    else if (dwFlags & MCI_SET_DOOR_OPEN ||
               dwFlags & MCI_SET_DOOR_CLOSED ||
               dwFlags & MCI_SET_VIDEO ||
               dwFlags & MCI_SET_AUDIO) {
        dwRes = MCIERR_UNSUPPORTED_FUNCTION;
    }
    else
        dwRes = MCIERR_MISSING_PARAMETER;

    return dwRes;
}

/****************************************************************************
 *
 * mqMCIGetDevCaps:  Respond to device capabilities command
 *
 * Parms:
 *          nChannel    Channel number for caps
 *          dwFlags     flags
 *          lpCaps      capability parameters
 *
 * Return:  MCI error code
 *
 ***************************************************************************/
DWORD NEAR PASCAL mqMCIGetDevCaps(int nChannel, DWORD dwFlags, LPMCI_GETDEVCAPS_PARMS lpCaps)
{
DWORD dwRes;

    dout("MCI_GETDEVCAPS message");

    if (!dwFlags & MCI_GETDEVCAPS_ITEM)
        return MCIERR_MISSING_PARAMETER;

    switch (lpCaps->dwItem) {

        case MCI_GETDEVCAPS_CAN_RECORD:
        case MCI_GETDEVCAPS_CAN_SAVE:
        case MCI_GETDEVCAPS_CAN_EJECT:
        case MCI_GETDEVCAPS_USES_FILES:
        case MCI_GETDEVCAPS_HAS_AUDIO:
        case MCI_GETDEVCAPS_HAS_VIDEO:
        case MCI_GETDEVCAPS_COMPOUND_DEVICE:
            lpCaps->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
            dwRes = MCI_RESOURCE_RETURNED;
            break;

        case MCI_GETDEVCAPS_CAN_PLAY:
            lpCaps->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
            dwRes = MCI_RESOURCE_RETURNED;
            break;

        case MCI_GETDEVCAPS_DEVICE_TYPE:
            lpCaps->dwReturn = MAKEMCIRESOURCE(MCI_DEVTYPE_OTHER,
                                               MCI_DEVTYPE_OTHER);
            dwRes = MCI_RESOURCE_RETURNED;
            break;

        default:
            dwRes = MCIERR_MISSING_PARAMETER;
            break;
    }

    return dwRes;
}

/****************************************************************************
 *
 * mqMCIProc:  All MCI specific messages go through this point
 *
 * Params:  wDeviceID   The MCI deviceID
 *          wMessage    The requested action to be performed
 *          dwParam1    Data.  Defined seperately for each message
 *          dwParam2    Data.  Defined seperately for each message
 *
 * Return:  Defined seperately for each message
 *
 ***************************************************************************/
DWORD FAR PASCAL mqMCIProc(WORD wDeviceID, WORD wMessage, DWORD dwParam1, DWORD dwParam2)
{
DWORD   dwRes = MCIERR_UNRECOGNIZED_COMMAND;
int     nChannel;
BOOL    fNotify = TRUE;

    nChannel = (int)mciGetDriverData(wDeviceID);

    switch (wMessage) {

        case MCI_OPEN_DRIVER:
            dwRes = mqMCIOpenDriver(nChannel, dwParam1,
                                 (LPMCI_OPEN_PARMS)dwParam2);
            break;

        case MCI_CLOSE_DRIVER:
            mqMCICloseDriver(nChannel, wDeviceID);
            dwRes = MCIERR_NO_ERROR;
            break;

        case MCI_PLAY:
            dwRes = mqMCIPlay(nChannel, wDeviceID, dwParam1,
                               (LPMCI_PLAY_PARMS)dwParam2);
            fNotify = FALSE;
            break;                
                      
        case MCI_STOP:
            dwRes = mqMCIStop(nChannel);
            break;

        case MCI_PAUSE:
            dwRes = mqMCIPause(nChannel);
            break;

        case MCI_CUE:
            dwRes = mqMCICue(nChannel);
            break;

        case MCI_SEEK:
            dwRes = mqMCISeek(nChannel, dwParam1, (LPMCI_SEEK_PARMS)dwParam2);
            break;

        case MCI_STATUS:
            dwRes = mqMCIStatus(nChannel, dwParam1,
                                (LPMCI_STATUS_PARMS)dwParam2);
            break;  

        case MCI_GETDEVCAPS:
            dwRes = mqMCIGetDevCaps(nChannel, dwParam1,
                                    (LPMCI_GETDEVCAPS_PARMS)dwParam2);
            break;

        case MCI_INFO:
            dwRes = mqMCIInfo(dwParam1, (LPMCI_INFO_PARMS)dwParam2);
            break;

        case MCI_SET:
            dwRes = mqMCISet(nChannel, dwParam1,
                             (LPMCI_WAVE_SET_PARMS)dwParam2);
            break;

        case MCI_RESUME:
        case MCI_RECORD:
        case MCI_LOAD:
        case MCI_SAVE:
            dwRes =  MCIERR_UNSUPPORTED_FUNCTION;
            break;
    }

    /*  If the message has not already handled notification and no
     *  error occured then process possible notification
     *
     *  Note, the LOWORD of the error is checked because the HIWORD may
     *  contain non-error condition flags
     */
    if (fNotify && LOWORD(dwRes) == 0)
        mqDriverNotify(wDeviceID, nChannel, 
                       dwParam1, (LPMCI_GENERIC_PARMS)dwParam2,
                       MCI_NOTIFY_SUCCESSFUL);

    return dwRes;
}
