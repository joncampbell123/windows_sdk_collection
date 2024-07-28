/****************************************************************************
 *
 *   mcipionr.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved
 *
 *   MCI Device Driver for the Pioneer 4200 Videodisc Player
 *
 *      MCI Module - MCI commands and interface to device
 *
 ***************************************************************************/

/* Known problems in this driver:
 *
 *  1) Play succeeds with no disc in the drive
 *  2) The command 'spin down' does not notify when completed
 *  3) The first 'status mode' after a 'spin down' followed by a
 *     'seek' to an invalid adress can return 'play' instead of
 *     'stopped'
 *  4) A 'spin down notify' command followed by a 'play' command
 *     will return MCI_NOTIFY_FAILURE instead of MCI_NOTIFY_ABORTED
 *  5) The first 'status time format' command sent after a new
 *     disk is inserted can result in a return value of '0' instead
 *     of a legal value like MCI_FORMAT_HMS
 */

#define USECOMM
#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "mcipionr.h"

/* Maximum number of supported comports */
#define PIONEER_MAX_COMPORTS 4

/* Time in milliseconds that the driver will normally wait for the device */
/* to return data */
#define PIONEER_READCHAR_TIMEOUT    1000 /* Minimum for 10Mhz 286 */

/* Time in milliseconds that the driver will wait for the device to return */
/* data when the device is busy (i.e. seeking) */
#define PIONEER_MAX_BUSY_TIME       30000

/* Maximum length of the videodisc's buffer */
#define VDISC_BUFFER_LENGTH         20

/* Macro to determine if the given videodisc frame is valid */
#define VALID_FRAME(n) ((DWORD)(n) <= (DWORD)99999)

/* Maximum number of frames on a CAV disc */
#define CAV_MAX_DISC_FRAMES         54000

/* Frame rate of a CAV disc */
#define CAV_FRAMES_PER_SECOND       30

/* Flags a to position as invalid */
#define NO_TO_POSITION              0xFFFFFFFF

/* Flags a time mode as invalid */
#define NO_TIME_MODE                0xFFFFFFFF

/* Flags a comport as invalid */
#define NO_COMPORT                  -1

/* Internal play directions */
/* Set at least two bits so they'll never equal MCI_PLAY_VD_REVERSE */
#define PION_PLAY_FORWARD       3
#define PION_PLAY_NO_DIRECTION  7

/* Polling period in milliseconds used for setting timer */
#define TIMER_POLLING_PERIOD    100

/* Macros to extract components of HMS time.  The standard macros in */
/* mmsystem.h could have been used instead */
#define HMS_HOUR(hms)   (((LPSTR)&hms)[0])
#define HMS_MINUTE(hms) (((LPSTR)&hms)[1])
#define HMS_SECOND(hms) (((LPSTR)&hms)[2])

/* Convert an HMS address to Seconds */
#define HMSTOSEC(hms) (HMS_HOUR(hms) * 3600 + HMS_MINUTE(hms) * 60 + \
                       HMS_SECOND(hms))

/* Difference in seconds between two HMS addresses */
#define HMS_DIFF(h2, h1) (HMSTOSEC(h2) - HMSTOSEC(h1))

/* Absolute value macro */
#define abs(a)  ((a) < 0 ? -(a) : (a))

#define AnsiUpperChar(c)        ((char)(WORD)(DWORD)AnsiUpper((LPSTR)(DWORD)(WORD)(char)(c)))

/* Module instance handle */
HINSTANCE hInstance;

/* ID of the timer used for polling */
static int wTimerID;

/* Number of channels which are using the timer */
static int nWaitingChannels;

/* Set to FALSE inside TimerProc to prevent re-entrant disasters when */
/* trying to yield */
static BOOL bYieldWhenReading = TRUE;

/* Forward declarations */
static DWORD PASCAL NEAR spinupdown(WORD wDeviceID, int nPort, DWORD dwFlag, BOOL bWait);
static DWORD PASCAL NEAR status(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_STATUS_PARMS lpStatus);

/* Data for each comm port - Port 0 is com1, port 1 is com2, etc... */
static struct {
    int    nCommID;        /* The ID returned by OpenComm */
    HWND   hCallback;      /* Handle to window function to call back */
    BOOL   bPlayerBusy;    /* TRUE if the player is seeking or transitioning */
                           /* between park and random access mode */
    BOOL   bDoorAction;    /* TRUE if door opening or closing */
    BOOL   bPlayTo;        /* TRUE if playing to a particular frame */
    DWORD  dwPlayTo;       /* The frame being played to */
    DWORD  dwToTimeMode;   /* Time mode of the dwPlayTo position */
    WORD   wAudioChannels; /* Bit 0 is ch1 status, bit 1 is ch2 */
    BOOL   bTimerSet;      /* TRUE when there is a timer for this channel */
    DWORD  dwTimeMode;     /*  One of MCI_FORMAT_MILLISECONDS, */
                           /*         MCI_FORMAT_HMS or */
                           /*         MCI_FORMAT_FRAMES or */
                           /*         MCI_VD_FORMAT_TRACK */
    BOOL   bCAVDisc;       /* True if a CAV disc is inserted */
    WORD   wDeviceID;      /* MCI device ID */
    int    nUseCount;
    BOOL   bShareable;
    BOOL   bResponding;
    DWORD  dwDirection;    /* The current direction.  One of: */
                           /* PION_PLAY_FORWARD */
                           /* PION_PLAY_NO_DIRECTION */
                           /* MCI_VD_PLAY_REVERSE */
    DWORD  dwBusyStart;    /* The time the timer loop started waiting */
} comport[PIONEER_MAX_COMPORTS];

#ifdef DEBUG
/* Amount of information (if any) to dump out the debug port */
static WORD wDebugLevel;
#endif

#define SZCODE char _based(_segname("_CODE"))

static SZCODE aszComm[] = "com";
static SZCODE aszCommOpenFormat[] = "%ls%1d";
static SZCODE aszCommSetupFormat[] = "%ls%1d:4800,n,8,1";
static SZCODE aszFrameFormat[] = "%lu";
static SZCODE aszTrackFormat[] = "%u";
static SZCODE aszHMSFormat[] = "%1u%2u%2u";
static SZCODE aszCLVLength[] = "10000";
static SZCODE aszNull[] = "";

static SZCODE aszQueryPlaying[] = "?P";
static SZCODE aszQueryMedia[] = "?D";
static SZCODE aszQueryTrack[] = "?C";
static SZCODE aszQueryFormat[] = "?F";
static SZCODE aszQueryTime[] = "?T";
static SZCODE aszFormat[] = "FR";
static SZCODE aszTime[] = "TM";
static SZCODE aszCheck[] = "CH";
static SZCODE aszClose[] = "0KL";
static SZCODE aszAudioOn[] = "3AD";
static SZCODE aszSpinUp[] = "SA";
static SZCODE aszStopMarker[] = "SM";
static SZCODE aszSeekTo[] = "SE";
static SZCODE aszSeekToFormat[] = "%uSE";
static SZCODE aszSeekStart[] = "0SE";
static SZCODE aszSeekEnd[] = "99999SE";
static SZCODE aszSeekSetSpeed[] = "255SP";
static SZCODE aszFastSetSpeed[] = "180SP";
static SZCODE aszSlowSetSpeed[] = "20SP";
static SZCODE aszMediumSetSpeed[] = "60SP";
static SZCODE aszSetSpeedFormat[] = "%uSP";
static SZCODE aszMediaReverse[] = "MR";
static SZCODE aszMediaForward[] = "MF";
static SZCODE aszPlay[] = "PL";
static SZCODE aszPause[] = "PA";
static SZCODE aszStop[] = "ST";
static SZCODE aszReject[] = "RJ";
static SZCODE aszStepReverse[] = "SR";
static SZCODE aszStepForward[] = "SF";
static SZCODE aszOpenDoor[] = "OP";
static SZCODE aszCommandOff[] = "2CM";
static SZCODE aszCommandOn[] = "3CM";
static SZCODE aszIndexOff[] = "0DS";
static SZCODE aszIndexOn[] = "1DS";
static SZCODE aszKeyLockOff[] = "0KL";
static SZCODE aszKeyLockOn[] = "1KL";

/****************************************************************************
 * @doc INTERNAL MCI
 *
 * @api WORD | getchars | Read "n" characters into "buf".  Wait up to
 *     PIONEER_READCHAR_TIMEOUT milliseconds.
 *
 * @parm int | nPort | Port number.
 *
 * @parm LPSTR | lpstrBuf | Buffer to fill.
 *
 * @parm int | n | Number of characters to read.
 *
 * @parm WORD | wWait | Number of milliseconds to wait before timing out
 *     If 0 then wait for PIONEER_READCHAR_TIMEOUT milliseconds.
 *
 * @rdesc Number of characters actually read.
 ***************************************************************************/
static WORD PASCAL NEAR getchars(WORD wDeviceID, int nPort, LPSTR lpstrBuf, int n, WORD wWait)
{
    int nRead, FirstTry = TRUE;
    DWORD dwTime0, dwTime;
    int nToRead = n;
    int nCommID = comport[nPort].nCommID;
#ifdef DEBUG
    LPSTR lpstrStart = lpstrBuf;
#endif

    if (wWait == 0)
        wWait = PIONEER_READCHAR_TIMEOUT;

    while (n > 0) {
/* Try to read the number of characters that are left to read */
        nRead = ReadComm(nCommID, lpstrBuf, n);

/* Some characters were read */
        if (nRead > 0) {
            n -= nRead;
            lpstrBuf += nRead;
        }
        else {
/* Either 0 characters read or an error occured */
            if (nRead < 0) {
                DOUT("mcipionr: Comm error");
                return MCIERR_HARDWARE;
            }
            if (nRead == 0) {
                COMSTAT comstat;
                if (GetCommError(nCommID, &comstat) != 0) {
                    DOUT("mcipionr: Comm error");
                    return MCIERR_HARDWARE;
                }
            }
        }
/* If not all characters were read */
        if (n > 0) {
/* If first failure then initialize time base */
            if (FirstTry) {
                dwTime0 = GetCurrentTime();
                FirstTry = FALSE;
            }
/* If subsequent failure then check for timeout */
            else {
                dwTime = GetCurrentTime();

/* Check timer rollover case */
                if (dwTime < dwTime0 &&
                    (dwTime + (0xFFFFFFFF - dwTime0)) > wWait) {
                    DOUT("mcipionr: getchars timeout!");
                    break;
                }

/* Normal case */
                if (dwTime - dwTime0 > wWait) {
                    DOUT("mcipionr: getchars timeout!");
                    break;
                }
                if (bYieldWhenReading)
                    mciDriverYield(wDeviceID);
            }
        }
    }
#ifdef DEBUG
    {
        char temp[50];
        LPSTR lpstrTemp = temp;
        nRead = nToRead - n;
        if (nRead > sizeof(temp) - 1)
            nRead = sizeof(temp) - 1;

        lpstrBuf = lpstrStart;
        while (nRead-- > 0)
            *lpstrTemp++ = *lpstrBuf++;
        *lpstrTemp = '\0';
        DOUT("MCIPIONR received:");
        DOUTX(temp);
    }
#endif
    return nToRead - n;
}

/****************************************************************************
 * @doc INTERNAL MCI
 *
 * @api int | GetCompletionCode | Read either a completion code ("R<CR>")
 *      or an error ("E0x<CR>").
 *
 * @parm WORD | wDeviceID | Device ID to use.
 *
 * @parm int | nPort | Port number.
 *
 * @parm WORD | wWait | Number of milliseconds to wait before timing out
 *     If 0 then wait for PIONEER_READCHAR_TIMEOUT milliseconds.
 *
 * @rdesc Returns zero if no error.
 ***************************************************************************/
static int PASCAL NEAR GetCompletionCode(WORD wDeviceID, int nPort, WORD wWait)
{
    char buf[2];
    
    DOUT("Get completion:");
    if (getchars(wDeviceID, nPort, buf, sizeof(buf), wWait) != sizeof(buf))
        return MCIERR_HARDWARE;
    if (buf[0] == 'E') {
/* Empty the buffer of the error condition */
        getchars(wDeviceID, nPort, buf, sizeof(buf), wWait);
        comport[nPort].bPlayerBusy = FALSE;
        return MCIERR_HARDWARE;
    }
    DOUT("True");
    return 0;
}    

/****************************************************************************
 * @doc INTERNAL MCI
 *
 * @api static int | putchars | Send "n" characters from "lpstrS" to the
 *     port specified by "nPort".
 *
 * @parm WORD | wDeviceID | Device ID to use.
 *
 * @parm int | nPort | The port number.
 *
 * @parm LPSTR | lpstrS | The string to send.
 * 
 * @parm int | bReturn | If TRUE then the function waits for a
 *     completion code (or an error) to be returned or until a timeout
 *     (PIONEER_GETCHAR_TIMEOUT msecs).  The timeout generates an error.
 *
 * @rdesc Returns zero if no error.
 ***************************************************************************/
static int PASCAL NEAR putchars(WORD wDeviceID, int nPort, LPSTR lpstrS, int bReturn)
{
    int nCommID = comport[nPort].nCommID;
    static char c = 0xd;
    int n = 0;

#ifdef DEBUG
    DOUT("MCIPIONR sent:");
    DOUTX(lpstrS);
#endif

    n = lstrlen(lpstrS);

/* Send string to player */
    if (WriteComm(nCommID, lpstrS, n) != n ||
        WriteComm(nCommID, &c, 1) != 1) {
        DOUT("mcipionr:  Hardware error on output");
        return MCIERR_HARDWARE;
    }

    if (bReturn)
/* Get completion code */
        return GetCompletionCode(wDeviceID, nPort, 0);
    else
        return 0;
}

/****************************************************************************
 * Get player status - Returns 1 if it is PLAY or MULTI-SPEED, -1 if there
 * is a hardware error, and 0 otherwise.
 ***************************************************************************/
static int PASCAL NEAR IsPlaying(WORD wDeviceID, int nPort)
{
    char buf[4];
    putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
    if (getchars(wDeviceID, nPort, buf, sizeof(buf), 0) != sizeof(buf))
        return -1;
    return buf[0] == 'P' && (buf[2] == '4' || buf[2] == '9');
}

/****************************************************************************
 * @doc INTERNAL MCI
 *
 * @api void | cancel_notify | Cancel any active notification for this port.
 *
 * @parm int | nPort | The port number to check.
 *
 * @parm WORD | wStatus | The notification status to return.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
static void PASCAL NEAR cancel_notify(int nPort, WORD wStatus)
{
    if (comport[nPort].bTimerSet) {
        comport[nPort].bTimerSet = FALSE;
        if (--nWaitingChannels <= 0)
            KillTimer(NULL, wTimerID);
        mciDriverNotify(comport[nPort].hCallback,
                            comport[nPort].wDeviceID, wStatus);
    }
}

/****************************************************************************
 * Check if the disc is spinning and return 0 if it is
 ***************************************************************************/
static DWORD PASCAL NEAR IsDiscSpinning(WORD wDeviceID, WORD nPort)
{
    char buf[4];

    putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
    if (getchars(wDeviceID, nPort, buf, sizeof(buf), 0) != sizeof(buf))
        return MCIERR_HARDWARE;
    if (buf[0] != 'P') {
        DOUT("mcipionr:  bad mode info from player");
        return MCIERR_HARDWARE;
    }
    if (buf[1] != '0')
        return MCIERR_PIONEER_NOT_SPINNING;
    if (buf[2] == '0' || buf[2] == '1')
        return MCIERR_PIONEER_NOT_SPINNING;
    else
        return 0;
}
    
/****************************************************************************
 * Get the media type
 ***************************************************************************/
static DWORD PASCAL NEAR get_media_type(WORD wDeviceID, WORD nPort)
{
    char buf[VDISC_BUFFER_LENGTH];
#if 0
    DWORD dwRet;
#endif

    putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
    if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
        return ((DWORD)MCIERR_HARDWARE) | 0x80000000;
    else if (buf[0] == '0')
        return ((DWORD)MCIERR_PIONEER_NOT_SPINNING) | 0x80000000;
#if 0
    if ((dwRet = IsDiscSpinning(wDeviceID, nPort)) != 0)
        return dwRet | 0x80000000;
    else
#endif
    if (buf[1] == '0') {
        comport[nPort].bCAVDisc = TRUE;
        return MCI_VD_MEDIA_CAV;
    }
    else if (buf[1] == '1') {
        comport[nPort].bCAVDisc = FALSE;
        return MCI_VD_MEDIA_CLV;
    } else
        return ((DWORD)MCIERR_PIONEER_NOT_SPINNING) | 0x80000000;
}

/****************************************************************************
 * Set the proper default time mode for the currently loaded media type
 ***************************************************************************/
static void PASCAL NEAR set_time_mode(WORD wDeviceID, WORD nPort)
{
    if (get_media_type(wDeviceID, nPort) == MCI_VD_MEDIA_CAV) {
        comport[nPort].dwTimeMode = MCI_FORMAT_FRAMES;
        putchars(wDeviceID, nPort, aszFormat, TRUE);
    }
    else {
        comport[nPort].dwTimeMode = MCI_FORMAT_HMS;
        putchars(wDeviceID, nPort, aszTime, TRUE);
    }
}

/****************************************************************************
 * See if the player has arrived at the proper 'to' position
 ***************************************************************************/
static BOOL PASCAL NEAR check_arrival(int nPort)
{
    MCI_STATUS_PARMS Status;
    BOOL bWasTracks = FALSE, bResult = TRUE;

    if (comport[nPort].dwPlayTo == NO_TO_POSITION)
        return TRUE;
    if (comport[nPort].dwTimeMode == MCI_VD_FORMAT_TRACK &&
        comport[nPort].dwToTimeMode != MCI_VD_FORMAT_TRACK) {
        bWasTracks = TRUE;
        set_time_mode(comport[nPort].wDeviceID, nPort);
    }
    Status.dwItem = MCI_STATUS_POSITION;
    if (LOWORD(status(comport[nPort].wDeviceID, nPort, MCI_STATUS_ITEM,
                       (LPMCI_STATUS_PARMS)&Status)) != 0) {
        bResult = FALSE;
        goto reset_mode;
    }

    switch (comport[nPort].dwTimeMode) {
        case MCI_FORMAT_HMS:
        {   int n;
            n = HMS_DIFF(Status.dwReturn, comport[nPort].dwPlayTo);
            if (abs(n) > 2)
                bResult = FALSE;
            break;
        }

        case MCI_FORMAT_FRAMES:
            if (abs(Status.dwReturn - comport[nPort].dwPlayTo) > 2)
                bResult = FALSE;
            break;

        case MCI_FORMAT_MILLISECONDS:
            if (abs(Status.dwReturn - comport[nPort].dwPlayTo) > 2000)
                bResult = FALSE;
            break;

        case MCI_VD_FORMAT_TRACK:
            if (abs(Status.dwReturn - comport[nPort].dwPlayTo) > 1) 
                bResult = FALSE;
            break;

        default:
            bResult = FALSE;
            break;
    }

reset_mode:;
    if (bWasTracks) {
        putchars(comport[nPort].wDeviceID, nPort, aszCheck, TRUE);
        comport[nPort].dwTimeMode = MCI_VD_FORMAT_TRACK;
    }

    return bResult;
}

/****************************************************************************
 *  The timer is active if any comm port controlled by this driver is
 *  waiting to reach a point on the disc (comport[nPort].bPlayTo == TRUE)
 *  or if it is seeking or spinning up or down
 *  (comport[nPort].bBusy == TRUE) AND if "notify <x>" was specified for
 *  the command
 ***************************************************************************/
void FAR PASCAL _loadds TimerProc(HWND hwnd, UINT uMessage, UINT uTimer, DWORD dwParam)
{
    int nCommID;
    char buf[4];
    int nPort;
    
    bYieldWhenReading = FALSE;
/* Loop through all channels */
    for (nPort = 0; nPort < PIONEER_MAX_COMPORTS; nPort++) {
        nCommID = comport[nPort].nCommID;

/* If this channel is not waiting, skip it */
        if (comport[nPort].bPlayerBusy) {
/* Try to get a character */
            if (ReadComm(nCommID, buf, 1) == 1) {
                int nRetCode;
/* At least one character available, get completion code */
                UngetCommChar(nCommID, buf[0]);
                nRetCode = GetCompletionCode(comport[nPort].wDeviceID,
                                           nPort, 0);
                comport[nPort].dwBusyStart = 0;
                comport[nPort].bPlayerBusy = FALSE;

/*  Unless the channel is waiting for a frame to be reached (play to x) */
/*  notify the application */
                if (!comport[nPort].bPlayTo)
                    cancel_notify(nPort, nRetCode == 0 ?
                                          MCI_NOTIFY_SUCCESSFUL :
                                          MCI_NOTIFY_FAILURE);
            }
            else {
/* No completion code so skip play */
                if (GetCommError(nCommID, NULL) != 0)
                    cancel_notify(nPort, MCI_NOTIFY_FAILURE);

                if (comport[nPort].dwBusyStart != 0) {
                    if (GetCurrentTime() >    
                        comport[nPort].dwBusyStart + PIONEER_MAX_BUSY_TIME)
                        cancel_notify(nPort, MCI_NOTIFY_FAILURE);
                } else
                    comport[nPort].dwBusyStart = GetCurrentTime();
                    
                continue;
            }
        }
        if (comport[nPort].bPlayTo) {

            int nPlay;

            if ((nPlay = IsPlaying(comport[nPort].wDeviceID, nPort)) == 0)
                cancel_notify(nPort, check_arrival(nPort) ?
                                      MCI_NOTIFY_SUCCESSFUL :
                                      MCI_NOTIFY_FAILURE);
            else if (nPlay == -1)
                cancel_notify(nPort, MCI_NOTIFY_FAILURE);
        }
    }
    bYieldWhenReading = TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | LibMain | Library initialization code.
 *
 * @parm HINSTANCE | hModule | Our instance handle.
 *
 * @parm WORD | cbHeap | The heap size from the .def file.
 *
 * @parm LPSTR | lpszCmdLine | The command line.
 *
 * @rdesc Returns 1 if the initialization was successful and 0 otherwise.
 ***************************************************************************/
int PASCAL NEAR LibMain(HINSTANCE hModule, WORD cbHeap, LPSTR lpszCmdLine)
{
    int nPort;

    hInstance = hModule;

/* Port 0 is com1, port 1 is com2, etc... */
    for (nPort = 0; nPort < PIONEER_MAX_COMPORTS; nPort++)
        comport[nPort].nCommID = NO_COMPORT;

#ifdef DEBUG
    wDebugLevel = GetProfileInt("mmdebug", "mcipionr", 0);
#endif

    return TRUE;
}

/****************************************************************************
 * Compare up to "n" characters in two strings. Comparison is case insensitive.
 * Returns 0 if they match, and non-zero otherwise.
 ***************************************************************************/
static WORD PASCAL NEAR vdisc_lstrncmp_nocase(LPSTR lpS1, LPSTR lpS2, int n)
{
    while (*lpS1 && *lpS2 && n--)
    {
        if (AnsiUpperChar(*lpS1) != AnsiUpperChar(*lpS2))
           break;
        lpS1++;
        lpS2++; 
    }
    return n;
}

/****************************************************************************
 * Send a successful notify if wErr is 0 and the MCI_NOTIFY flag is set
 * and supersede any active notification
 ***************************************************************************/
static void PASCAL NEAR notify(WORD wErr, DWORD dwFlags, WORD wDeviceID, LPMCI_GENERIC_PARMS lpParms, int nPort)
{
    if (wErr == 0 && dwFlags & MCI_NOTIFY) {
        cancel_notify(nPort, MCI_NOTIFY_SUPERSEDED);
            
        mciDriverNotify((HWND)(WORD)lpParms->dwCallback, wDeviceID,
                         MCI_NOTIFY_SUCCESSFUL);
    }
}

/****************************************************************************
 *  Process the MCI_NOTIFY and MCI_WAIT flags
 *
 *  If MCI_NOTIFY then start the timer going (if not started)
 *
 *  If MCI_WAIT then wait until completion if seeking or until the
 *  player is stopped if "play to <x>" is the command
 *
 *  Otherwise, if "play to <x>" was the command then just clear the
 *  bPlayTo flag
 ***************************************************************************/
static WORD PASCAL NEAR process_delay(WORD wDeviceID, int nPort, DWORD dwFlags, DWORD dwCb)
{
    int nCommID;

    nCommID = comport[nPort].nCommID;

    if (dwFlags & MCI_WAIT) {
        if (comport[nPort].bPlayerBusy) {
            comport[nPort].bPlayerBusy = FALSE;
            if (GetCompletionCode(wDeviceID, nPort, PIONEER_MAX_BUSY_TIME)
                != 0) {
                if (dwFlags & MCI_NOTIFY)
                    mciDriverNotify((HANDLE)dwCb, wDeviceID,
                                     MCI_NOTIFY_FAILURE);
                return MCIERR_HARDWARE;
            }
        }
/*        if (comport[nPort].bPlayTo) */
        {
            int nPlay;

            while ((nPlay = IsPlaying(wDeviceID, nPort)) == 1)
/* If the operation should be aborted */
                if (mciDriverYield(wDeviceID) != 0)
                    return process_delay(wDeviceID, nPort, dwFlags & ~MCI_WAIT, dwCb);
            comport[nPort].bPlayTo = FALSE;
            if (nPlay == -1) {
                if (dwFlags & MCI_NOTIFY)
                    mciDriverNotify((HANDLE)dwCb,
                                     wDeviceID, MCI_NOTIFY_FAILURE);
                return MCIERR_HARDWARE;
            }
        }
        if (dwFlags & MCI_NOTIFY)
            mciDriverNotify((HANDLE)dwCb, wDeviceID, MCI_NOTIFY_SUCCESSFUL);
    }
    else if (dwFlags & MCI_NOTIFY && (HANDLE)dwCb != NULL) {
        comport[nPort].hCallback = (HANDLE)dwCb;
        if (!comport[nPort].bTimerSet) {
            comport[nPort].bTimerSet = TRUE;
            comport[nPort].wDeviceID = wDeviceID;
            if (nWaitingChannels++ == 0)
                if ((wTimerID = SetTimer(NULL, 1, TIMER_POLLING_PERIOD,
                                          TimerProc)) == 0)
                    return MCIERR_PIONEER_NO_TIMERS;
        }
    } else
        comport[nPort].bPlayTo = FALSE;
    return 0;
}

/****************************************************************************
 * Concatenate lpIN onto lpOut but don't exceed wLen in length, including
 * terminating null.  Returns the total length not including the terminating
 * null or 0 on error or overflow.
 ***************************************************************************/
static WORD PASCAL NEAR catstring(LPSTR lpOut, LPSTR lpIn, int nLen)
{
    int nSize = 0;
    if (lpOut == NULL || lpIn == NULL)
        return 0;

/* search to end of lpOut */
    while (*lpOut != '\0') {
        ++lpOut;
        ++nSize;
    }

/* concatenate */
    while (nSize++ < nLen && *lpIn != '\0')
        *lpOut++ = *lpIn++;

    *lpOut = '\0';
    if (*lpIn != '\0')
        return 0;
    return nSize - 1;
}

/****************************************************************************
 * Convert the input string to a DWORD
 ***************************************************************************/
static DWORD PASCAL NEAR vdisc_atodw(LPSTR lpstrInput)
{
    DWORD dwRet = 0;

    while (*lpstrInput >= '0' && *lpstrInput <= '9')
        dwRet = dwRet * 10 + *lpstrInput++ - '0';

    return dwRet;
}

/****************************************************************************
 * Shut down the device and release the com port
 ***************************************************************************/
static void PASCAL NEAR vdisc_close(WORD wDeviceID, int nPort)
{
    int nCommID = comport[nPort].nCommID;

#ifdef DEBUG
    char buf[100];
    wsprintf(buf, "port=%d commid=%d", nPort, nCommID);
    DOUT(buf);
#endif
    DOUT("vdisc_close");
    if (nCommID != NO_COMPORT) {
/* Unlock front panel */
        DOUT("unlock");
/* Don't allow a yield because auto-close will be messed up */
        bYieldWhenReading = FALSE;
        putchars(wDeviceID, nPort, aszClose, TRUE);
        bYieldWhenReading = TRUE;
        DOUT("CloseComm");
        CloseComm(nCommID);
    }
}

/****************************************************************************
 * Switch to frame or time mode whichever is appropriate for the disk type
 ***************************************************************************/
static int PASCAL NEAR unset_chapter_mode(WORD wDeviceID, int nPort)
{
    char buf[6];

    putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
    if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
        return MCIERR_HARDWARE;
    if (buf[1] == '0')
        putchars(wDeviceID, nPort, aszFormat, TRUE);
    else
        putchars(wDeviceID, nPort, aszTime, TRUE);
    return 0;
}

/****************************************************************************
 * Read the comport number for the input in the form "com<x>"
 ***************************************************************************/
WORD PASCAL FAR pionGetComport(LPSTR lpstrBuf)
{
    if (lpstrBuf != NULL) {
        while (*lpstrBuf == ' ')
            ++lpstrBuf;
        if (!vdisc_lstrncmp_nocase(lpstrBuf, aszComm, sizeof(aszComm) -1))
            if (lpstrBuf[sizeof(aszComm)-1] >= '1' && lpstrBuf[sizeof(aszComm)-1] <=
                '0' + PIONEER_MAX_COMPORTS) {
                WORD    wPort;

                if ((wPort = lpstrBuf[sizeof(aszComm)-1] - '1') < PIONEER_MAX_COMPORTS)
                    return wPort;
            }
    }
    return 0;
}

/****************************************************************************
 * Initialize the player
 ***************************************************************************/
static WORD PASCAL NEAR init_player(WORD wDeviceID, int nPort)
{
    char buf[VDISC_BUFFER_LENGTH];
    BOOL bPlayerSpinning = FALSE;

/* Set the audio channels to a known state (both on) */
    putchars(wDeviceID, nPort, aszAudioOn, TRUE);
    comport[nPort].wAudioChannels = 3;

/* See if the disk is spinning */
    putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
    getchars(wDeviceID, nPort, buf, 4, 0);

    if (buf[0] == 'P' && buf[2] != '0' && buf[2] != '1') {
        bPlayerSpinning = TRUE;

/* What kind of disc is this (CAV or CLV)? */
        putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
        if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
            return (DWORD)MCIERR_HARDWARE;
        if (buf[1] != '1') {
            comport[nPort].dwTimeMode = MCI_FORMAT_FRAMES;
            comport[nPort].bCAVDisc = TRUE;
/* Set mode to frames */
            putchars(wDeviceID, nPort, aszFormat, TRUE);
        }
        else {
            comport[nPort].dwTimeMode = MCI_FORMAT_HMS;
            comport[nPort].bCAVDisc = FALSE;
/* Set mode to hms */
            putchars(wDeviceID, nPort, aszTime, TRUE);
        }
    }
    else {
        bPlayerSpinning = FALSE;
        comport[nPort].dwTimeMode = NO_TIME_MODE;
    }

    comport[nPort].bPlayerBusy = FALSE;
    comport[nPort].bDoorAction = FALSE;
    comport[nPort].bPlayTo = FALSE;
    comport[nPort].bTimerSet = FALSE;
    comport[nPort].dwBusyStart = 0;
    comport[nPort].dwDirection = PION_PLAY_NO_DIRECTION;

    if (!bPlayerSpinning && comport[nPort].bResponding
        && spinupdown(wDeviceID, nPort, MCI_VD_SPIN_UP, FALSE) != 0)

        return MCIERR_HARDWARE;
    else
        return 0;
}

/****************************************************************************
 * Process the MCI_OPEN_DRIVER message
 ***************************************************************************/
static DWORD PASCAL NEAR open(WORD wDeviceID, int nPort, DWORD dwFlags)
{
    DCB dcb;
    char strDescription [20];
    int nCommID;

    if (dwFlags & MCI_OPEN_ELEMENT)
        return MCIERR_NO_ELEMENT_ALLOWED;

/* See if a com port was specified in the SYSTEM.INI parameters */
    wsprintf(strDescription, aszCommOpenFormat, (LPSTR)aszComm, nPort + 1);

/* Try to open the com port */
    if ((nCommID = OpenComm(strDescription, 100, 100)) < 0)
        return (DWORD)MCIERR_HARDWARE;

/* Set up the com port, 4800 baud (switch S7 UP) is assumed */
    wsprintf(strDescription, aszCommSetupFormat, (LPSTR)aszComm, nPort + 1);
    BuildCommDCB(strDescription, &dcb);

    if (SetCommState(&dcb) != 0) {
        CloseComm(nCommID);
        return (DWORD)MCIERR_HARDWARE;
    }
                             
/* Set up the channel description */
    comport[nPort].nCommID = nCommID;
    if (dwFlags & MCI_OPEN_SHAREABLE)
        comport[nPort].bShareable = TRUE;
    else
        comport[nPort].bShareable = FALSE;

/* Lock the front panel */
    if (putchars(wDeviceID, nPort, aszKeyLockOn, TRUE) != 0) {
        comport[nPort].bResponding = FALSE;
        return 0;
    }
    else {
        comport[nPort].bResponding = TRUE;
        return init_player(wDeviceID, nPort);
    }

}

/****************************************************************************
 * Convert the given position dwPos in the current time format into the units
 * appropriate for the disk type puting the result in buf
 ***************************************************************************/
static WORD PASCAL NEAR encode_position(WORD wDeviceID, LPSTR buf, DWORD dwPos, int nPort)
{
    unsigned char h, m, s;

/* Allow frame 0 */
    if (dwPos == 0
        && comport[nPort].dwTimeMode != MCI_FORMAT_HMS
        && comport[nPort].dwTimeMode != MCI_VD_FORMAT_TRACK)
        dwPos = 1;

    if (comport[nPort].dwTimeMode == NO_TIME_MODE)
        set_time_mode(wDeviceID, nPort);

    switch (comport[nPort].dwTimeMode) {

        case MCI_FORMAT_FRAMES:
/* Ensure frame is at most five characters */
            if (!VALID_FRAME(dwPos))
                return (DWORD)MCIERR_OUTOFRANGE;
            wsprintf(buf, aszFrameFormat, dwPos);
            break;

        case MCI_FORMAT_HMS:
            h = ((LPSTR)&dwPos)[0];
            m = ((LPSTR)&dwPos)[1];
            s = ((LPSTR)&dwPos)[2];
            if (h > 9 || m > 59 || s > 59)
                return MCIERR_OUTOFRANGE;

            if (comport[nPort].bCAVDisc)
                wsprintf(buf, aszFrameFormat, (DWORD)(((h * 60) + m) * 60 + s) *
                                      CAV_FRAMES_PER_SECOND);
            else {
                wsprintf(buf, aszHMSFormat, h, m, s);
                if (m < 10)
                    buf[1] = '0';
                if (s < 10)
                    buf[3] = '0';
            }
            break;

        case MCI_FORMAT_MILLISECONDS:
            if (comport[nPort].bCAVDisc) {
                dwPos = (dwPos * 3) / 100; /* 30 frames/second */
                wsprintf(buf, aszFrameFormat, dwPos);
            }
            else {
                WORD wX;
                dwPos /= 1000; /* ignore fractions of a second */

                /* Number of minutes leftover from hours */
                wX = (WORD)(dwPos % 3600);
                h = (char)((dwPos - wX) / 3600);
                if (h > 9)
                    return MCIERR_OUTOFRANGE;

                dwPos = wX;

                s = (char)(dwPos % 60);
                m = (char)((dwPos - s) / 60);

                wsprintf(buf, aszHMSFormat, h, m, s);
/* Fill in leading zero's */
                if (m < 10)
                    buf[1] = '0';
                if (s < 10)
                    buf[3] = '0';
                }
                break;

        case MCI_VD_FORMAT_TRACK:
            if (dwPos > 99)
                return MCIERR_OUTOFRANGE;
            wsprintf(buf, aszTrackFormat, dwPos);
            break;
    }

    return 0L;
}

/****************************************************************************
 * Convert frames to the output representation for the current time mode
 ***************************************************************************/
static DWORD PASCAL NEAR convert_frames(WORD wDeviceID, int nPort, DWORD dwFrames, LPDWORD lpdwReturn)
{

    if (comport[nPort].dwTimeMode == NO_TIME_MODE)
        set_time_mode(wDeviceID, nPort);
    switch (comport[nPort].dwTimeMode) {

        case MCI_FORMAT_FRAMES:
            *lpdwReturn = dwFrames;
            break;

        case MCI_FORMAT_MILLISECONDS:
            *lpdwReturn = (dwFrames * 100) / 3; /* 30 frames per second */
            break;

        case MCI_FORMAT_HMS:
        {
            LPSTR lpOutput = (LPSTR)lpdwReturn;
            DWORD dwSeconds = dwFrames / CAV_FRAMES_PER_SECOND;
            DWORD dwRem;

            dwRem = dwSeconds % 3600;
            lpOutput[0] = (char)((dwSeconds - dwRem)/3600);

            dwSeconds = dwRem % 60;
            lpOutput[1] = (char)((dwRem - dwSeconds) / 60);

            lpOutput[2] = (char)dwSeconds;

            lpOutput[3] = 0;
            return MCI_COLONIZED3_RETURN;
        }
    }
        
    return 0;
}

/****************************************************************************
 * Convert hms to the output representation for the current time mode
 ***************************************************************************/
static DWORD PASCAL NEAR convert_hms(WORD wDeviceID, int nPort, LPSTR buf, LPDWORD lpdwReturn)
{
    if (comport[nPort].dwTimeMode == NO_TIME_MODE)
        set_time_mode(wDeviceID, nPort);

    if (comport[nPort].dwTimeMode == MCI_FORMAT_HMS) {
        WORD wTemp;

        ((LPSTR)lpdwReturn)[0] = (char)(buf[0] - '0');
        wTemp = (buf[1] - '0') * 10 + (buf[2] - '0');
        ((LPSTR)lpdwReturn)[1] = (char)wTemp;
        wTemp = (buf[3] - '0') * 10 + (buf[4] - '0');
        ((LPSTR)lpdwReturn)[2] = (char)wTemp;
        ((LPSTR)lpdwReturn)[3] = 0;
        return MCI_COLONIZED3_RETURN;
    }
    else if (comport[nPort].dwTimeMode == MCI_FORMAT_MILLISECONDS) {
        *lpdwReturn =
                    (buf[0] - '0') * 3600000L +
                    ((buf[1] - '0') * 10 + (buf[2] - '0')) * 60000L +
                    ((buf[3] - '0') * 10 + (buf[4] - '0')) * 1000L;
        return 0;
    } else
        return MCIERR_HARDWARE;
}

/****************************************************************************/

static  int PASCAL NEAR DeviceStatusMode(
        UINT    wDeviceID,
        int     nPort)
{
#define PIONEER_MODE_OPEN     '0'
#define PIONEER_MODE_PARK     '1'
#define PIONEER_MODE_PLAY     '4'
#define PIONEER_MODE_STILL    '5'
#define PIONEER_MODE_PAUSE    '6'
#define PIONEER_MODE_MULTI    '9'

    char buf[VDISC_BUFFER_LENGTH];
    WORD wPos;

    putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
    if (getchars(wDeviceID, nPort, buf, 4, 0) == 0)
        return MCI_MODE_NOT_READY;
/* This is done to remove the spurious return character generated */
/* if this command is sent after power is cut and restored to the player */
    if (buf[0] != 'P' && buf[1] == 'P') {
        wPos = 3;
        getchars(wDeviceID, nPort, buf, 1, 200);
    }
    else
        wPos = 2;

    switch (buf[wPos]) {

    case PIONEER_MODE_OPEN:
        return MCI_MODE_OPEN;

    case PIONEER_MODE_PARK:
        return MCI_VD_MODE_PARK;

    case PIONEER_MODE_PLAY:
        return MCI_MODE_PLAY;

    case PIONEER_MODE_STILL:
        return MCI_MODE_PAUSE;

    case PIONEER_MODE_PAUSE:
        return MCI_MODE_STOP;

    case PIONEER_MODE_MULTI:
        return MCI_MODE_PLAY;

    default:
        return MCI_MODE_NOT_READY;

    }
}

/****************************************************************************
 * Process the MCI_STATUS message
 ***************************************************************************/
static DWORD PASCAL NEAR status(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_STATUS_PARMS lpStatus)
{
    char buf[VDISC_BUFFER_LENGTH];
    int n;
    DWORD dwRet;

    if (!(dwFlags & MCI_STATUS_ITEM))
        return MCIERR_MISSING_PARAMETER;

    switch (lpStatus->dwItem) {

        case MCI_STATUS_MODE:
        {
            n = DeviceStatusMode(wDeviceID, nPort);
            if (n == MCI_MODE_NOT_READY)
                n = DeviceStatusMode(wDeviceID, nPort);

            lpStatus->dwReturn = MAKEMCIRESOURCE(n, n);
            return MCI_RESOURCE_RETURNED;
        }

        case MCI_STATUS_POSITION:
            putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
            getchars(wDeviceID, nPort, buf, 4, 0);
 
            if (buf[0] == 'P' && buf[1] == '0' && buf[2] == '0') 
                return (DWORD)MCIERR_HARDWARE;
                
            dwRet = spinupdown(wDeviceID, nPort, MCI_VD_SPIN_UP, TRUE);
            if (dwRet != 0)
                return dwRet;
            if (dwFlags & MCI_TRACK)
                return MCIERR_UNSUPPORTED_FUNCTION;
            if (dwFlags & MCI_STATUS_START) {
                if (comport[nPort].dwTimeMode == NO_TIME_MODE)
                    set_time_mode(wDeviceID, nPort);
                switch (comport[nPort].dwTimeMode) {
                    case MCI_VD_FORMAT_TRACK:
                        lpStatus->dwReturn = 0;
                        return 0;
                    case MCI_FORMAT_FRAMES:
                        lpStatus->dwReturn = 1;
                        return 0;
                    case MCI_FORMAT_HMS:
                        lpStatus->dwReturn = 0;
                        return MCI_COLONIZED3_RETURN;
                    case MCI_FORMAT_MILLISECONDS:
                        if (comport[nPort].bCAVDisc)
                            lpStatus->dwReturn = 1000 / CAV_FRAMES_PER_SECOND;
                        else
                            lpStatus->dwReturn = 0;
                        return 0;
                    default:
                        return MCIERR_HARDWARE;
                }
            }
            if (comport[nPort].dwTimeMode == MCI_VD_FORMAT_TRACK) {
                putchars(wDeviceID, nPort, aszQueryTrack, FALSE);
                n = getchars(wDeviceID, nPort, buf, 3, 0);
                buf[n-1] = '\0';
                if (buf[0] == 'E')
                    return MCIERR_HARDWARE;
                lpStatus->dwReturn = vdisc_atodw(buf);
                return 0;
            }

            if (comport[nPort].dwTimeMode == NO_TIME_MODE)
                set_time_mode(wDeviceID, nPort);
            else
                get_media_type(wDeviceID, nPort);

            if (comport[nPort].bCAVDisc) {
/* Try FRAMES */
                putchars(wDeviceID, nPort, aszQueryFormat, FALSE);
                n = getchars(wDeviceID, nPort, buf, 6, 0);
                buf[n - 1] = '\0';

/* If no error then convert from frames */
                if (buf[0] != 'E')
                    return convert_frames(wDeviceID, nPort, vdisc_atodw(buf),
                                        &lpStatus->dwReturn);
                else
                    return MCIERR_HARDWARE;
            }
            else {
/* Try TIME */
                putchars(wDeviceID, nPort, aszQueryTime, FALSE);
                n = getchars(wDeviceID, nPort, buf, 6, 0);
                buf[n - 1] = '\0';
                if (buf[0] == 'E') {
                    DOUT("mcipionr:  error returning HMS position");
                    return MCIERR_HARDWARE;
                }
                if (comport[nPort].dwTimeMode == MCI_FORMAT_FRAMES)
                    return MCIERR_HARDWARE;
                else
                    return convert_hms(wDeviceID, nPort, buf, &lpStatus->dwReturn);
            }

        case MCI_STATUS_MEDIA_PRESENT:
            putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
            if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
                return (DWORD)MCIERR_HARDWARE;
            if (buf[0] == '1')
                lpStatus->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
            else
                lpStatus->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
            return MCI_RESOURCE_RETURNED;

        case MCI_VD_STATUS_SPEED:
            return (DWORD)MCIERR_UNSUPPORTED_FUNCTION;

        case MCI_VD_STATUS_MEDIA_TYPE:
        {
            DWORD dwMedType;

            dwMedType = get_media_type(wDeviceID, nPort);
            if (dwMedType & 0x80000000)
                return dwMedType & 0x7FFFFFFF;

            n = LOWORD(dwMedType);
            lpStatus->dwReturn = MAKEMCIRESOURCE(n, n);

            if (dwMedType == MCI_VD_MEDIA_CAV)
                comport[nPort].bCAVDisc = TRUE;
            else {
                comport[nPort].bCAVDisc = FALSE;
/* If a CLV disc has time mode set to FRAMES, set the time mode to HMS */
/* because frames are illegal for CLV */
                if (comport[nPort].dwTimeMode == MCI_FORMAT_FRAMES)
                    comport[nPort].dwTimeMode = MCI_FORMAT_HMS;
            }
            return MCI_RESOURCE_RETURNED;
        }

        case MCI_VD_STATUS_SIDE:
        {
            DWORD dwRet = IsDiscSpinning(wDeviceID, nPort);

            if (dwRet != 0)
                return dwRet;

            putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
            if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
                return (DWORD)MCIERR_HARDWARE;
            if (buf[0] == '0')
                return MCIERR_PIONEER_NOT_SPINNING;
            if (buf[3] == '0')
                lpStatus->dwReturn = 1;
            else
                lpStatus->dwReturn = 2;
            return 0;
        }

        case MCI_VD_STATUS_DISC_SIZE:
        {
            DWORD dwRet = IsDiscSpinning(wDeviceID, nPort);

            if (dwRet != 0)
                return dwRet;

            putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
            if (getchars(wDeviceID, nPort, buf, 6, 0) != 6)
                return (DWORD)MCIERR_HARDWARE;
            if (buf[0] == '0')
                return MCIERR_PIONEER_NOT_SPINNING;
            if (buf[2] == '0')
                lpStatus->dwReturn = 12;
            else
                lpStatus->dwReturn = 8;
            return 0;
        }

        case MCI_STATUS_READY:
            putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
            if (getchars(wDeviceID, nPort, buf, 4, 0) != 4)
                lpStatus->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
            else
                lpStatus->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
            return MCI_RESOURCE_RETURNED;

        case MCI_STATUS_LENGTH:
        {
            DWORD dwType;
            if (dwFlags & MCI_TRACK)
                return MCIERR_UNSUPPORTED_FUNCTION;
            putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
            getchars(wDeviceID, nPort, buf, 4, 0);
 
            if (buf[0] == 'P' && buf[1] == '0' && buf[2] == '0') 
                return (DWORD)MCIERR_HARDWARE;

            dwRet = spinupdown(wDeviceID, nPort, MCI_VD_SPIN_UP, TRUE);
            if (dwRet != 0)
                return dwRet;

            if (comport[nPort].dwTimeMode == MCI_VD_FORMAT_TRACK)
                return MCIERR_BAD_TIME_FORMAT;

            if ((dwType = get_media_type(wDeviceID, nPort)) == MCI_VD_MEDIA_CAV)
                return convert_frames(wDeviceID, nPort, CAV_MAX_DISC_FRAMES,
                                    &lpStatus->dwReturn);
            else if (dwType == MCI_VD_MEDIA_CLV)
                return convert_hms(wDeviceID, nPort, aszCLVLength, &lpStatus->dwReturn);
            else
                return dwType & 0x7FFFFFFF;
        }

        case MCI_STATUS_TIME_FORMAT:
            putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
            getchars(wDeviceID, nPort, buf, 4, 0);
 
            if (buf[0] == 'P' && buf[1] == '0' && buf[2] == '0') 
                return (DWORD)MCIERR_HARDWARE;
                
            dwRet = spinupdown(wDeviceID, nPort, MCI_VD_SPIN_UP, TRUE);
            if (dwRet != 0)
                return dwRet;

            if (comport[nPort].dwTimeMode == NO_TIME_MODE)
                set_time_mode(nPort, wDeviceID);
            n = LOWORD(comport[nPort].dwTimeMode);
            if (n == MCI_VD_FORMAT_TRACK)
                lpStatus->dwReturn = MAKEMCIRESOURCE(MCI_VD_FORMAT_TRACK,
                                                     MCI_VD_FORMAT_TRACK_S);
            else
                lpStatus->dwReturn =
                    MAKEMCIRESOURCE(n, n + MCI_FORMAT_RETURN_BASE);
            return MCI_RESOURCE_RETURNED;

        case MCI_STATUS_CURRENT_TRACK:
            putchars(wDeviceID, nPort, aszQueryPlaying, FALSE);
            getchars(wDeviceID, nPort, buf, 4, 0);
 
            if (buf[0] == 'P' && buf[1] == '0' && buf[2] == '0') 
                return (DWORD)MCIERR_HARDWARE;
                
            dwRet = spinupdown(wDeviceID, nPort, MCI_VD_SPIN_UP, TRUE);
            if (dwRet != 0)
                return dwRet;

            putchars(wDeviceID, nPort, aszQueryTrack, FALSE);
            n = getchars(wDeviceID, nPort, buf, 3, 0);
            buf[n-1] = '\0';
            if (buf[0] == 'E') {
                /* Flush buffer */
                getchars(wDeviceID, nPort, buf, 2, 0);

                /* See if the problem is no chapter support */
                putchars(wDeviceID, nPort, aszQueryMedia, FALSE);
                if (getchars(wDeviceID, nPort, buf, 6, 0) != 6 ||
                    buf[4] == '1')
                    return MCIERR_HARDWARE;
                else
                    return MCIERR_PIONEER_NO_CHAPTERS;
            }
            lpStatus->dwReturn = vdisc_atodw(buf);
            return 0;
    }
            
    return (DWORD)MCIERR_UNSUPPORTED_FUNCTION;
} 

/****************************************************************************
 * Process the MCI_SEEK message
 ***************************************************************************/
static DWORD PASCAL NEAR seek(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_SEEK_PARMS lpSeek)
{
    int comlen;
    char buf[VDISC_BUFFER_LENGTH];
    WORD wErr;

    buf[0] = '\0';

/* Position for the wsprintf to start */
    comlen = 0;

    if (IsDiscSpinning(wDeviceID, nPort) != 0) {
        if (dwFlags & MCI_TO) {
/* Must spin up NOW if a to position needs to be converted */
            putchars(wDeviceID, nPort, aszSpinUp, FALSE);
            GetCompletionCode(wDeviceID, nPort, PIONEER_MAX_BUSY_TIME);
        } else
        {
            comlen = 2;
            catstring(buf, aszSpinUp, VDISC_BUFFER_LENGTH);
/* 2 characters possible here */
            comport[nPort].bPlayerBusy = TRUE;
        }
    }
    if (dwFlags & (MCI_TO | MCI_SEEK_TO_START | MCI_SEEK_TO_END)) {
        if (dwFlags & MCI_SEEK_TO_START) {
            if (dwFlags & MCI_SEEK_TO_END)
                return MCIERR_FLAGS_NOT_COMPATIBLE;
            catstring (buf, aszSeekStart, VDISC_BUFFER_LENGTH);
        }
        else if (dwFlags & MCI_SEEK_TO_END) {
            catstring (buf, aszSeekEnd, VDISC_BUFFER_LENGTH);
        }
        else if (dwFlags & MCI_TO) {
            if (comport[nPort].dwTimeMode == NO_TIME_MODE)
                set_time_mode(wDeviceID, nPort);

            if ((wErr = encode_position(wDeviceID, &buf[comlen], lpSeek->dwTo, nPort))
                != 0)
                return wErr;

            catstring(buf, aszSeekTo, VDISC_BUFFER_LENGTH);
        }
        putchars(wDeviceID, nPort, buf, FALSE);

        comport[nPort].bPlayerBusy = TRUE;
    }
    else {
        catstring(buf, aszSeekSetSpeed, VDISC_BUFFER_LENGTH);
        if (dwFlags & MCI_VD_SEEK_REVERSE) {
            if ((wErr = (WORD)get_media_type(wDeviceID, nPort)) ==
                MCI_VD_MEDIA_CAV)
                catstring(buf, aszMediaReverse, VDISC_BUFFER_LENGTH);
            else if (wErr == MCI_VD_MEDIA_CLV)
                return MCIERR_PIONEER_ILLEGAL_FOR_CLV;
            else return wErr;
        }
        else
            catstring(buf, aszMediaForward, VDISC_BUFFER_LENGTH);
        putchars(wDeviceID, nPort, buf, TRUE);
        if (dwFlags & MCI_NOTIFY) {
            comport[nPort].bPlayTo = TRUE;
            comport[nPort].dwPlayTo = NO_TO_POSITION;
        }
    }

    cancel_notify(nPort, MCI_NOTIFY_ABORTED);
    
    process_delay(wDeviceID, nPort, dwFlags, lpSeek->dwCallback);
    return 0;
}

/****************************************************************************
 * Process the MCI_PLAY message
 ***************************************************************************/
static DWORD PASCAL NEAR play(WORD wDeviceID, int nPort, DWORD dwFlags, MCI_VD_PLAY_PARMS FAR *lpPlay)
{
    
    LPSTR compart;
    int comlen;
    WORD wErr;
    BOOL bNormalSpeed = FALSE;
    char buf[VDISC_BUFFER_LENGTH];
    DWORD dwOldToPosition = comport[nPort].bPlayTo ? comport[nPort].dwPlayTo
                            : NO_TO_POSITION;
    DWORD dwOldDirection = comport[nPort].dwDirection;
    BOOL bPlayerSpinning;
    BOOL bGoingToBeBusy = FALSE;

    buf[0] = '\0';

/* Convert a 'play x to x' into 'seek to x' if the positions are equal or */
/* for milliseconds if they are within the same frame or second */
    if (dwFlags & MCI_FROM && dwFlags & MCI_TO &&
        (lpPlay->dwTo == lpPlay->dwFrom ||
        (comport[nPort].dwTimeMode == MCI_FORMAT_MILLISECONDS &&
         lpPlay->dwTo - lpPlay->dwFrom <
         (DWORD)(comport[nPort].bCAVDisc ? 40 : 1000))))
    {
        MCI_SEEK_PARMS Seek;
/* Preserve NOTIFY and WAIT and set TO flag */
        DWORD dwSeekFlags;
        
        dwSeekFlags = ((dwFlags & (MCI_NOTIFY || MCI_WAIT)) | MCI_TO);

        Seek.dwTo = lpPlay->dwFrom;
        Seek.dwCallback = lpPlay->dwCallback;
        return seek(wDeviceID, nPort, dwSeekFlags,
                     (LPMCI_SEEK_PARMS)&Seek);
    }

/* Build a command string to send to the player */

/* Position for the wsprintf to start */
    comlen = 0;

#define PLAY_SPEED_FLAGS (MCI_VD_PLAY_FAST | MCI_VD_PLAY_SLOW | \
                          MCI_VD_PLAY_SPEED | MCI_VD_PLAY_SCAN)

/* Determine speed */
    if (dwFlags & MCI_VD_PLAY_FAST) {
        if ((dwFlags & PLAY_SPEED_FLAGS) != MCI_VD_PLAY_FAST)
            return MCIERR_FLAGS_NOT_COMPATIBLE;
        /* PLAY FAST */
        compart = aszFastSetSpeed;
    }
    else if (dwFlags & MCI_VD_PLAY_SLOW) {
        if ((dwFlags & PLAY_SPEED_FLAGS) != MCI_VD_PLAY_SLOW)
            return MCIERR_FLAGS_NOT_COMPATIBLE;
        /* PLAY SLOW */
        compart = aszSlowSetSpeed;
    }
    else if (dwFlags & MCI_VD_PLAY_SPEED &&
             lpPlay->dwSpeed != CAV_FRAMES_PER_SECOND) {
        if ((dwFlags & PLAY_SPEED_FLAGS) != MCI_VD_PLAY_SPEED)
            return MCIERR_FLAGS_NOT_COMPATIBLE;

        if (lpPlay->dwSpeed > 127)
            return MCIERR_OUTOFRANGE;

        wsprintf(buf, aszSetSpeedFormat, (WORD)lpPlay->dwSpeed * 2);
        compart = buf;
    }
    else if (dwFlags & MCI_VD_PLAY_SCAN) {
        wsprintf(buf, aszSetSpeedFormat, 255);
        compart = buf;
    }
    else {
        /* PLAY NORMAL */
        compart = aszNull;
        if (dwFlags & MCI_VD_PLAY_REVERSE)
            compart = aszMediumSetSpeed;
        bNormalSpeed = TRUE;
    }
    if (compart[0] != '\0')
        putchars(wDeviceID, nPort, compart, TRUE);

    if (!(bPlayerSpinning = !IsDiscSpinning(wDeviceID, nPort))) {
        if ((dwFlags & (MCI_TO | MCI_FROM)) != 0 || !bNormalSpeed) {
/* Must spin up NOW if a position needs to be converted */
            putchars(wDeviceID, nPort, aszSpinUp, FALSE);
            GetCompletionCode(wDeviceID, nPort, PIONEER_MAX_BUSY_TIME);
        }
        else {
            catstring(buf, aszSpinUp, VDISC_BUFFER_LENGTH);
            comlen = 2;
/* 2 characters possible here */
            bGoingToBeBusy = TRUE;
        }
    }

    if (!bNormalSpeed &&
        get_media_type(wDeviceID, nPort) == MCI_VD_MEDIA_CLV)
        return MCIERR_PIONEER_ILLEGAL_FOR_CLV;

/* If FROM was specified */
    if (dwFlags & MCI_FROM) {
        if (comport[nPort].dwTimeMode == NO_TIME_MODE)
            set_time_mode(wDeviceID, nPort);

        if ((wErr = encode_position(wDeviceID, &buf[comlen], lpPlay->dwFrom, nPort))
            != 0)
            return wErr;
/* 5 characters possible here, total of 7 */
        catstring(buf, aszSeekTo, VDISC_BUFFER_LENGTH);
/* 2 characters possible here, total of 9 */
        bGoingToBeBusy = TRUE;
    }

/* If TO was specified */
    if (dwFlags & MCI_TO) {
        char tobuf[10];
        DWORD dwFrom;

        if (dwFlags & MCI_VD_PLAY_REVERSE)
            return MCIERR_FLAGS_NOT_COMPATIBLE;

        if (comport[nPort].dwTimeMode == NO_TIME_MODE)
            set_time_mode(wDeviceID, nPort);

        if ((wErr = encode_position(wDeviceID, tobuf, lpPlay->dwTo, nPort)) != 0)
            return wErr;

        catstring(buf, tobuf, VDISC_BUFFER_LENGTH);
/* 5 characters possible here, total of 14 */
        catstring(buf, aszStopMarker, VDISC_BUFFER_LENGTH);
/* 2 characters possible here, total of 16 */
        comport[nPort].bPlayTo = TRUE;
        comport[nPort].dwPlayTo = lpPlay->dwTo;
        comport[nPort].dwToTimeMode = comport[nPort].dwTimeMode;

/* If to is less than from then go in reverse */
        if (dwFlags & MCI_FROM)
            dwFrom = lpPlay->dwFrom;
        else {
            if (!bPlayerSpinning)
                dwFrom = 0;
            else {
                MCI_STATUS_PARMS Status;
                Status.dwItem = MCI_STATUS_POSITION;
                if ((wErr =
                    (WORD)status(wDeviceID, nPort, MCI_STATUS_ITEM,
                            (LPMCI_STATUS_PARMS)&Status)) != 0)
                    return (DWORD)wErr;
                dwFrom = Status.dwReturn;
            }
        }
/* Compare from and to positions */
        if (comport[nPort].dwTimeMode == MCI_FORMAT_HMS) {
/* Account for slop */
            DWORD dwTo = lpPlay->dwTo;
            if (HMS_HOUR(dwTo) < HMS_HOUR(dwFrom))
                dwFlags |= MCI_VD_PLAY_REVERSE;
            else if (HMS_HOUR(dwTo) == HMS_HOUR(dwFrom)) {
                if (HMS_MINUTE(dwTo) < HMS_MINUTE(dwFrom))
                    dwFlags |= MCI_VD_PLAY_REVERSE;
                else if (HMS_MINUTE(dwTo) == HMS_MINUTE(dwFrom)) {
                    int nDelta = HMS_SECOND(dwTo) - HMS_SECOND(dwFrom);
/* Position is plus or minus 1 second from HMS */
                    if (nDelta <= 1 && nDelta >= -1)
                        dwFrom = lpPlay->dwTo;
                    else if (nDelta < 0)
                        dwFlags |= MCI_VD_PLAY_REVERSE;
                }
            }
        }
        else if (comport[nPort].dwTimeMode == MCI_FORMAT_MILLISECONDS) {
/* Account for slop */
            long lDelta = lpPlay->dwTo - dwFrom;
            if (lDelta < 0) {
                lDelta = -lDelta;
                dwFlags |= MCI_VD_PLAY_REVERSE;
            }
            if (comport[nPort].bCAVDisc &&
                lDelta < 1000 / CAV_FRAMES_PER_SECOND)
                dwFrom = lpPlay->dwTo;
            else if (lDelta < 1000)
                dwFrom = lpPlay->dwTo;
        } else if (lpPlay->dwTo < dwFrom)
            dwFlags |= MCI_VD_PLAY_REVERSE;
        if (!comport[nPort].bCAVDisc && dwFlags & MCI_VD_PLAY_REVERSE)
            return MCIERR_PIONEER_ILLEGAL_FOR_CLV;
/* If from == to then do nothing */
        if (lpPlay->dwTo == dwFrom) {
            notify(0, dwFlags, wDeviceID,
                    (LPMCI_GENERIC_PARMS)lpPlay, nPort);
            return 0;
        }

    }
    else {
        comport[nPort].bPlayTo = TRUE;
        comport[nPort].dwPlayTo = NO_TO_POSITION;
    }

    /* Determine direction */
    if (dwFlags & MCI_VD_PLAY_REVERSE)
        /* PLAY REVERSE */
        compart = aszMediaReverse;
    else if (bNormalSpeed)
        /* PLAY FORWARD NORMAL SPEED */
/*  The PL command is used here instead of 60SPMF because only PL is */
/*  legal for CLV discs */
        compart = aszPlay;
    else
        /* PLAY FORWARD */
        compart = aszMediaForward;
    catstring(buf, compart, VDISC_BUFFER_LENGTH);
/* 2 characters possible here, total of 18, buffer size is 20 */

    if (putchars(wDeviceID, nPort, buf, !bGoingToBeBusy) != 0)
        return (DWORD)MCIERR_HARDWARE;

    if (bGoingToBeBusy)
        comport[nPort].bPlayerBusy = TRUE;

    if (dwFlags & MCI_VD_PLAY_REVERSE)
        comport[nPort].dwDirection = MCI_VD_PLAY_REVERSE;
    else
        comport[nPort].dwDirection = PION_PLAY_FORWARD;

/* If a from position is specified or a to position is specified with
 * a different position than the active notify or if a new direction is
 * specified then cancel notify;
 */
    if (dwFlags & MCI_FROM ||
        comport[nPort].dwPlayTo != dwOldToPosition ||
        (dwOldDirection != PION_PLAY_NO_DIRECTION &&
         comport[nPort].dwDirection != dwOldDirection))

        cancel_notify(nPort, MCI_NOTIFY_ABORTED);
    else if (dwFlags & MCI_NOTIFY)
        cancel_notify(nPort, MCI_NOTIFY_SUPERSEDED);

/*    if (comport[nPort].bPlayerBusy || comport[nPort].bPlayTo) */
    process_delay(wDeviceID, nPort, dwFlags, lpPlay->dwCallback);
    return 0;
}

/****************************************************************************
 * Process the MCI_STOP and MCI_PAUSE messages
 ***************************************************************************/
static DWORD PASCAL NEAR stop_pause(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_GENERIC_PARMS lpGeneric, WORD wCommand)
{
    DWORD dwErr;

/* Note, "ST" stands for 'still', "PA" stands for 'pause' but */
/* in MCI lingo "stop" uses the "PA" command and "pause" uses "ST" */
    if ((dwErr = IsDiscSpinning(wDeviceID, nPort)) == 0) {
        dwErr = (DWORD)putchars(wDeviceID, nPort,
                                  wCommand == MCI_STOP ? aszPause : aszStop,
                                  TRUE);
/* If error and CLV disc then try "stop" instead */
        if (dwErr != 0 && wCommand == MCI_PAUSE)
            if (get_media_type(wDeviceID, nPort) == MCI_VD_MEDIA_CLV)
                dwErr = (DWORD)putchars(wDeviceID, nPort, aszPause, TRUE);
    }

    if (dwErr == 0)
        cancel_notify(nPort, MCI_NOTIFY_ABORTED);

    notify(LOWORD(dwErr), dwFlags, wDeviceID, lpGeneric, nPort);

    return dwErr;
}

/****************************************************************************
 * Spin the player up or down depending on dwFlag
 ***************************************************************************/
static DWORD PASCAL NEAR spinupdown(WORD wDeviceID, int nPort, DWORD dwFlag, BOOL bWait)
{
    if (dwFlag & MCI_VD_SPIN_UP) {
        if (IsDiscSpinning(wDeviceID, nPort) != 0) {
            WORD wErr;
            if (!bWait) {
                comport[nPort].bPlayerBusy = TRUE;
                comport[nPort].bDoorAction = TRUE;
            }
            if (putchars(wDeviceID, nPort, aszSpinUp, FALSE) != 0)
                return (DWORD)MCIERR_HARDWARE;
            if (bWait && (wErr =
                 GetCompletionCode(wDeviceID, nPort, PIONEER_MAX_BUSY_TIME)) != 0)
                return wErr;
        }
    }
    else if (dwFlag & MCI_VD_SPIN_DOWN) {
        if (IsDiscSpinning(wDeviceID, nPort) == 0) {
            comport[nPort].bPlayerBusy = TRUE;
            comport[nPort].dwDirection = PION_PLAY_NO_DIRECTION;

            cancel_notify(nPort, MCI_NOTIFY_ABORTED);

            if (putchars(wDeviceID, nPort, aszReject, bWait) != 0)
                return (DWORD)MCIERR_HARDWARE;
        }
    } else
        return (DWORD)MCIERR_MISSING_PARAMETER;

    return 0;
}

/****************************************************************************
 * Process the MCI_SPIN message
 ***************************************************************************/
static DWORD PASCAL NEAR spin(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_GENERIC_PARMS lpGeneric)
{
    DWORD dwErr;

    dwErr = spinupdown(wDeviceID, nPort, dwFlags, FALSE);
    if (dwErr != 0)
        return dwErr;

    if (comport[nPort].bPlayerBusy)
        process_delay(wDeviceID, nPort, dwFlags, lpGeneric->dwCallback);
    else
        notify(0, dwFlags, wDeviceID, lpGeneric,
                nPort);

    return 0;
}

/****************************************************************************
 * Process the MCI_STEP message
 ***************************************************************************/
static DWORD PASCAL NEAR step(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_VD_STEP_PARMS lpStep)
{
    DWORD dwFrame, dwErr;
    WORD wErr;
    int n;
    char buf[VDISC_BUFFER_LENGTH];

    if (dwFlags & MCI_VD_STEP_FRAMES) {
        if (comport[nPort].dwTimeMode == NO_TIME_MODE)
            set_time_mode(wDeviceID, nPort);
/* Must get current position and go from there */
        if (putchars(wDeviceID, nPort, aszQueryFormat, FALSE) != 0)
            return (DWORD)MCIERR_HARDWARE;

        if ((n = getchars(wDeviceID, nPort, buf, 6, 0)) != 6
            || buf[0] == 'E')
            goto step_error;

        buf[n - 1] = '\0';
        dwFrame = vdisc_atodw(buf);

        if (dwFlags & MCI_VD_STEP_REVERSE)
            dwFrame -= lpStep->dwFrames;
        else
            dwFrame += lpStep->dwFrames;

        wsprintf(buf, aszSeekToFormat, (WORD)dwFrame);

        comport[nPort].bPlayerBusy = TRUE;

        if (putchars(wDeviceID, nPort, buf, FALSE) != 0)
            return (DWORD)MCIERR_HARDWARE;

        process_delay(wDeviceID, nPort, dwFlags, lpStep->dwCallback);
        return 0;
    }
    else {
        if (dwFlags & MCI_VD_STEP_REVERSE)
            wErr = putchars(wDeviceID, nPort, aszStepReverse, TRUE);
        else
            wErr = putchars(wDeviceID, nPort, aszStepForward, TRUE);
        if (wErr == 0)
            return 0;
        else
            goto step_error;
    }
step_error:;
    if ((dwErr = get_media_type(wDeviceID, nPort)) == MCI_VD_MEDIA_CLV)
        return MCIERR_PIONEER_ILLEGAL_FOR_CLV;
    else if (dwErr == MCI_VD_MEDIA_CAV)
        return (DWORD)MCIERR_HARDWARE;
    else
        return dwErr & 0x7FFFFFFF;
}

/****************************************************************************
 * Process the MCI_GETDEVCAPS message
 ***************************************************************************/
static DWORD PASCAL NEAR getdevcaps(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_GETDEVCAPS_PARMS lpCaps)
{
    BOOL bCLV = FALSE;

/* Info is for CAV unless CLV specified or current disc is CLV */
    if (dwFlags & MCI_VD_GETDEVCAPS_CLV)
        bCLV = TRUE;
    else if (!(dwFlags & MCI_VD_GETDEVCAPS_CAV) &&
             get_media_type(wDeviceID, nPort) == MCI_VD_MEDIA_CLV)
        bCLV = TRUE;

    if (!(MCI_GETDEVCAPS_ITEM))
        return MCIERR_MISSING_PARAMETER;

    switch (lpCaps->dwItem) {

        case MCI_GETDEVCAPS_CAN_RECORD:
        case MCI_GETDEVCAPS_CAN_SAVE:
        case MCI_GETDEVCAPS_USES_FILES:
        case MCI_GETDEVCAPS_COMPOUND_DEVICE:
            lpCaps->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
            return MCI_RESOURCE_RETURNED;

        case MCI_GETDEVCAPS_HAS_AUDIO:
        case MCI_GETDEVCAPS_HAS_VIDEO:
        case MCI_GETDEVCAPS_CAN_EJECT:
        case MCI_GETDEVCAPS_CAN_PLAY:
            lpCaps->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
            return MCI_RESOURCE_RETURNED;

        case MCI_VD_GETDEVCAPS_CAN_REVERSE:
            if (bCLV)
                lpCaps->dwReturn = MAKEMCIRESOURCE(FALSE, MCI_FALSE);
            else
                lpCaps->dwReturn = MAKEMCIRESOURCE(TRUE, MCI_TRUE);
            return MCI_RESOURCE_RETURNED;

        case MCI_GETDEVCAPS_DEVICE_TYPE:
            lpCaps->dwReturn = MAKEMCIRESOURCE(MCI_DEVTYPE_VIDEODISC,
                                               MCI_DEVTYPE_VIDEODISC);
            return MCI_RESOURCE_RETURNED;

        case MCI_VD_GETDEVCAPS_NORMAL_RATE:
            lpCaps->dwReturn = CAV_FRAMES_PER_SECOND;
            return 0;

        case MCI_VD_GETDEVCAPS_SLOW_RATE:
        case MCI_VD_GETDEVCAPS_CLV:
            if (bCLV)
                lpCaps->dwReturn = 0;
            else
                lpCaps->dwReturn = CAV_FRAMES_PER_SECOND / 3;
            return 0;

        case MCI_VD_GETDEVCAPS_FAST_RATE:
            if (bCLV)
                lpCaps->dwReturn = 0;
            else
                lpCaps->dwReturn = CAV_FRAMES_PER_SECOND * 3;
            return 0;
    }
    return (DWORD)MCIERR_MISSING_PARAMETER;
}

/****************************************************************************
 * Process the MCI_SET message
 ***************************************************************************/
static DWORD PASCAL NEAR set(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_SET_PARMS lpSet)
{
    char strCommand[4];
    WORD wMask;

    if (dwFlags & MCI_SET_AUDIO) {
        if (dwFlags & MCI_SET_VIDEO)
            return MCIERR_FLAGS_NOT_COMPATIBLE;

        strCommand[0] = '0';
        strCommand[1] = 'A';
        strCommand[2] = 'D';
        strCommand[3] = '\0';

        if (lpSet->dwAudio == MCI_SET_AUDIO_LEFT)
            wMask = 1;
        else if (lpSet->dwAudio == MCI_SET_AUDIO_RIGHT)
            wMask = 2;
        else if (lpSet->dwAudio == MCI_SET_AUDIO_ALL)
            wMask = 3;
        else
            return MCIERR_OUTOFRANGE;

        if (dwFlags & MCI_SET_ON) {
            if (dwFlags & MCI_SET_OFF)
                return MCIERR_FLAGS_NOT_COMPATIBLE;

            comport[nPort].wAudioChannels |= wMask;
        }
        else if (dwFlags & MCI_SET_OFF)
            comport[nPort].wAudioChannels &= ~wMask;
        else
            return MCIERR_MISSING_PARAMETER;

        strCommand[0] = (char)(comport[nPort].wAudioChannels + '0');
        putchars(wDeviceID, nPort, strCommand, TRUE);
    }
    else if (dwFlags & MCI_SET_TIME_FORMAT) {
        switch (lpSet->dwTimeFormat) {

            case MCI_FORMAT_MILLISECONDS:
                comport[nPort].dwTimeMode = MCI_FORMAT_MILLISECONDS;
                break;

            case MCI_FORMAT_HMS:
                comport[nPort].dwTimeMode = MCI_FORMAT_HMS;
                break;

            case MCI_FORMAT_FRAMES:
                if (get_media_type(wDeviceID, nPort) == MCI_VD_MEDIA_CLV)
                    return MCIERR_PIONEER_ILLEGAL_FOR_CLV;
                comport[nPort].dwTimeMode = MCI_FORMAT_FRAMES;
                break;

            case MCI_VD_FORMAT_TRACK:
                if (putchars(wDeviceID, nPort, aszCheck, TRUE) != 0)
                    return MCIERR_HARDWARE;
                comport[nPort].dwTimeMode = MCI_VD_FORMAT_TRACK;
                break;

            default:
                return MCIERR_BAD_TIME_FORMAT;
        }
        if (lpSet->dwTimeFormat != MCI_VD_FORMAT_TRACK) {
            WORD wErr;
            if ((wErr = unset_chapter_mode(wDeviceID, nPort)) != 0)
                return wErr;
        }
    }
    else if (dwFlags & MCI_SET_VIDEO) {
        strCommand[1] = 'V';
        strCommand[2] = 'D';
        strCommand[3] = '\0';
        if (dwFlags & MCI_SET_ON) {
            if (dwFlags & MCI_SET_OFF)
                return MCIERR_FLAGS_NOT_COMPATIBLE;

            strCommand[0] = '1';
        } else if (dwFlags & MCI_SET_OFF)
            strCommand[0] = '0';
        else
            return (DWORD)MCIERR_MISSING_PARAMETER;
        if (putchars(wDeviceID, nPort, strCommand, TRUE) != 0)
            return (DWORD)MCIERR_HARDWARE;
    }
    else if (dwFlags & MCI_SET_DOOR_OPEN) {
        if (putchars(wDeviceID, nPort, aszOpenDoor, FALSE) != 0)
            return MCIERR_HARDWARE;
        comport[nPort].bPlayerBusy = TRUE;
        comport[nPort].bDoorAction = TRUE;
        comport[nPort].dwTimeMode = NO_TIME_MODE;
        process_delay(wDeviceID, nPort, dwFlags, lpSet->dwCallback);
        return 0;
    }
    else if (dwFlags & MCI_SET_DOOR_CLOSED) {
/* Don't use spinupdown() because it won't work right for notification */
        if (IsDiscSpinning(wDeviceID, nPort) != 0) {
            comport[nPort].bPlayerBusy = TRUE;
            comport[nPort].bDoorAction = TRUE;
            if (putchars(wDeviceID, nPort, aszSpinUp, FALSE) != 0)
                return (DWORD)MCIERR_HARDWARE;
            process_delay(wDeviceID, nPort, dwFlags, lpSet->dwCallback);
            return 0;
        }

    } else
        return (DWORD)MCIERR_MISSING_PARAMETER;

    notify(0, dwFlags, wDeviceID, (LPMCI_GENERIC_PARMS)lpSet, nPort);
    return 0;
}

/****************************************************************************
 * Process the MCI_ESCAPE message
 ***************************************************************************/
static DWORD PASCAL NEAR command(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_VD_ESCAPE_PARMS lpCommand)
{
    WORD wErr, wReturn = 0;

    if (!(dwFlags & MCI_VD_ESCAPE_STRING) || lpCommand->lpstrCommand == NULL)
        return MCIERR_MISSING_PARAMETER;

/* Turn off return codes -- this command has no return code */
    if ((wErr = putchars(wDeviceID, nPort, aszCommandOff, FALSE)) != 0)
        return wErr;

/* Send application's string */
    if ((wErr = putchars(wDeviceID, nPort, (LPSTR)lpCommand->lpstrCommand, FALSE))
        != 0)
        wReturn = wErr;

/* Turn on return codes -- this command has a return code */
    if ((wErr = putchars(wDeviceID, nPort, aszCommandOn, TRUE)) != 0 &&
        wReturn == 0)

        wReturn = wErr;

    return wReturn;
}

/****************************************************************************
 * Process the MCI_VDISC_INDEX message
 ***************************************************************************/
static DWORD PASCAL NEAR index(WORD wDeviceID, int nPort, DWORD dwFlags)
{
    return putchars(wDeviceID, nPort,
                      dwFlags & VDISC_FLAG_ON ? aszIndexOn : aszIndexOff, TRUE);
}

/****************************************************************************
 * Process the MCI_VDISC_KEYLOCK message
 ***************************************************************************/
static DWORD PASCAL NEAR keylock(WORD wDeviceID, int nPort, DWORD dwFlags)
{
/* Ensure that one and only flag is set */
    if (dwFlags & VDISC_FLAG_ON && dwFlags & VDISC_FLAG_OFF)
        return MCIERR_FLAGS_NOT_COMPATIBLE;

    if (!(dwFlags & (VDISC_FLAG_ON | VDISC_FLAG_OFF)))
        return MCIERR_MISSING_PARAMETER;

    return putchars(wDeviceID, nPort,
                      dwFlags & VDISC_FLAG_ON ? aszKeyLockOn : aszKeyLockOff,
                      TRUE);
}

/****************************************************************************
 * Process the MCI_INFO message
 ***************************************************************************/
static DWORD PASCAL NEAR info(WORD wDeviceID, int nPort, DWORD dwFlags, LPMCI_INFO_PARMS lpInfo)
{
    DWORD dwErr;

    if (dwFlags & MCI_INFO_PRODUCT) {
        if (lpInfo->lpstrReturn == NULL ||
            !LOWORD(lpInfo->dwRetSize))
            dwErr = MCIERR_PARAM_OVERFLOW;
        else {
            WORD        wReturnBufferLength;

            wReturnBufferLength = LOWORD(lpInfo->dwRetSize);
            *(lpInfo->lpstrReturn + wReturnBufferLength - 1) = '\0';
            lpInfo->dwRetSize = LoadString(hInstance, IDS_PRODUCTNAME, lpInfo->lpstrReturn, wReturnBufferLength);
            if (*(lpInfo->lpstrReturn + wReturnBufferLength - 1) != '\0')
                dwErr = MCIERR_PARAM_OVERFLOW;
            else
                dwErr = 0;
        }
    } else
        dwErr = MCIERR_MISSING_PARAMETER;

    return dwErr;
}

/****************************************************************************
 * Process the MCI_CLOSE message
 ***************************************************************************/
static void PASCAL NEAR close(WORD wDeviceID, int nPort)
{
    DOUT("Closing...");   
    if (--comport[nPort].nUseCount == 0) {
        DOUT("comport...");   
        vdisc_close(wDeviceID, nPort);
        comport[nPort].nCommID = NO_COMPORT;
        if (comport[nPort].bTimerSet) {
            if (--nWaitingChannels == 0) {
                KillTimer(NULL, wTimerID);
                mciDriverNotify(comport[nPort].hCallback,
                                comport[nPort].wDeviceID,
                                MCI_NOTIFY_ABORTED);
            }

        }
    }
}

/****************************************************************************
 * Process all MCI specific message
 ***************************************************************************/
DWORD FAR PASCAL mciDriverEntry(WORD wDeviceID, UINT message, LPARAM lParam1, LPARAM lParam2)
{
    int nCommID, nPort;
    DWORD dwErr = 0;
    LPMCI_GENERIC_PARMS lpGeneric = (LPMCI_GENERIC_PARMS)lParam2;
#ifdef DEBUG
    char buf[100];
#endif

/* Catch these here to avoid a mandatory wait */
    switch (message) {
        case MCI_RECORD:
        case MCI_LOAD:
        case MCI_SAVE:
        case MCI_RESUME:
            return MCIERR_UNSUPPORTED_FUNCTION;
    }

    if (lpGeneric == NULL)
        return MCIERR_NULL_PARAMETER_BLOCK;

/* Find the channel number given the device ID */
    nPort = (WORD)mciGetDriverData(wDeviceID);
    nCommID = comport[nPort].nCommID;

#ifdef DEBUG
    wsprintf(buf, "port=%d commid=%d", nPort, nCommID);
    DOUT(buf);
#endif

/* If the device is busy then wait for completion before sending any commands */
    if (message != MCI_OPEN_DRIVER) {
        if (comport[nPort].bPlayerBusy) {
/* If the command is 'status mode' */
            if (message == MCI_STATUS && (DWORD)lParam1 & MCI_STATUS_ITEM &&
                ((LPMCI_STATUS_PARMS)lParam2)->dwItem == MCI_STATUS_MODE) {
/* Seek if the device is done seeking */
                if (GetCompletionCode(wDeviceID, nPort, 200) == 0) {
                    comport[nPort].bPlayerBusy = FALSE;
                    comport[nPort].bDoorAction = FALSE;
                }
                else {
/* If not then return MCI_MODE_SEEK */
                    WORD wMode;

                    wMode = (comport[nPort].bDoorAction? MCI_MODE_NOT_READY :
                        MCI_MODE_SEEK);
                    ((LPMCI_STATUS_PARMS)lParam2)->dwReturn =
                        MAKEMCIRESOURCE(wMode, wMode);
                    notify(LOWORD(dwErr), (DWORD)lParam1, wDeviceID,
                            (LPMCI_GENERIC_PARMS)lParam2, nPort);
                    return MCI_RESOURCE_RETURNED;
                }
            }
            else {
/* Wait up to 25 seconds for the ongoing command to complete */
                GetCompletionCode(wDeviceID, nPort, 25000);
                comport[nPort].bPlayerBusy = FALSE;
            }
        }
/* If the device has not yet responded try to see if it's alive */
        if (!comport[nPort].bResponding && message != MCI_CLOSE_DRIVER &&
            message != MCI_GETDEVCAPS)
            if (putchars(wDeviceID, nPort, aszKeyLockOn, TRUE) != 0)
                return MCIERR_HARDWARE;
            else {
                comport[nPort].bResponding = TRUE;
                init_player(wDeviceID, nPort);
            }
   } 

/* These commands will abort notification or are otherwise strange */
    switch (message) {

        case MCI_PLAY:
            return play(wDeviceID, nPort, (DWORD)lParam1, (MCI_VD_PLAY_PARMS FAR *)lParam2);

        case MCI_SEEK:
            return seek(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_SEEK_PARMS)lParam2);

        case MCI_SET:
            dwErr = set(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_SET_PARMS)lParam2);
            return dwErr;

        case MCI_SPIN:
            return spin(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_GENERIC_PARMS)lParam2);

        case MCI_CLOSE_DRIVER:
            close(wDeviceID, nPort);
            notify(0, (DWORD)lParam1, wDeviceID, (LPMCI_GENERIC_PARMS)lParam2,
                    nPort);
            return 0;

        case MCI_STOP:
/* Fall through */

        case MCI_PAUSE:
            dwErr = stop_pause(wDeviceID, nPort,
                                (DWORD)lParam1, lpGeneric, message);
            return dwErr;
    }



/* These commands will NOT abort notification */
    switch (message) {
         case MCI_OPEN_DRIVER:
/* If the port is in use then shareable must be specified now and with */
/* the previous open */
            if (comport[nPort].nUseCount != 0)
                if (!((DWORD)lParam1 & MCI_OPEN_SHAREABLE) ||
                    !comport[nPort].bShareable)
                    return MCIERR_MUST_USE_SHAREABLE;

            ++comport[nPort].nUseCount;

            if (comport[nPort].nCommID == NO_COMPORT)
                dwErr = open(wDeviceID, nPort, (DWORD)lParam1);
            break;

       case VDISC_INDEX:
            dwErr = index(wDeviceID, nPort, (DWORD)lParam1);
            break;

        case VDISC_KEYLOCK:
            dwErr = keylock(wDeviceID, nPort, (DWORD)lParam1);
            break;

        case MCI_ESCAPE:
            dwErr = command(wDeviceID, nPort, (DWORD)lParam1,
                             (LPMCI_VD_ESCAPE_PARMS)lParam2);
            break;

/* The MCI_STEP message should really be in the above list of message */
/* which can abort notification.  In this driver, MCI_STEP never */
/* aborts notification which is an error */
        case MCI_STEP:
            dwErr = step(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_VD_STEP_PARMS)lParam2);
            break;

        case MCI_GETDEVCAPS:
            dwErr = getdevcaps(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_GETDEVCAPS_PARMS)lParam2);
            break;

        case MCI_STATUS:
            dwErr = status(wDeviceID, nPort, (DWORD)lParam1, (LPMCI_STATUS_PARMS)lParam2);
            break;

        case MCI_INFO:
            dwErr = info(wDeviceID, nPort,  (DWORD)lParam1,
                          (LPMCI_INFO_PARMS)lParam2);
            break;

        default:
            dwErr = MCIERR_UNRECOGNIZED_COMMAND;
            break;

    } /* switch */

    notify(LOWORD(dwErr), (DWORD)lParam1, wDeviceID, (LPMCI_GENERIC_PARMS)lParam2,
            nPort);

    return dwErr;
}

/****************************************************************************
 * Library exit function
 ***************************************************************************/
BOOL PASCAL FAR _loadds WEP(BOOL fSystemExit)
{
    int n;

    for (n = 0; n < PIONEER_MAX_COMPORTS; n++)
        if (comport[n].nCommID != NO_COMPORT)
            CloseComm(comport[n].nCommID);
    return TRUE;
}
