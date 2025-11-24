/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 *
 *  QASAMPLE.C
 *
 *  Sample MCI Device Driver
 *
 *      Internal representation of sample device
 *
 ***************************************************************************/

/*  This module contains the complete representation of a simple device
 *  which has mode and position and plays at one speed using the system
 *  clock to keep track of time.
 *
 *  the APIs listed in QASAMPLE.H are used to manipulate the device -
 *  they all have the prefix 'qs'.
 */

#include <windows.h>
#include "qasample.h"

#define QS_VALID_DEVICE(n)  ((n) >= 0 && (n) < QS_MAX_DEVICES)
#define QS_MAX_DEVICES      4

/* media length in milliseconds (60 seconds) */
#define QS_MEDIA_LENGTH     120000L

struct {
    BOOL    fInUse;
    WORD    nMode;
    DWORD   dwPosition;              /* in milliseconds */
    DWORD   dwTimeStartedPlaying;    /* in milliseconds */
    int     nTimerStop;
    DWORD   dwToPosition;            /* position to play to */
} DeviceData[QS_MAX_DEVICES];


/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsOpen | Open a sample device.
 *
 * @parm int | nDevice | Device number to open.
 *
 * @rdesc TRUE on success, FALSE on failure (device in use or invalid
 *     device number).
 ***************************************************************************/
BOOL qsOpen(int nDevice)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    if (DeviceData[nDevice].fInUse)
        return FALSE;

    DeviceData[nDevice].fInUse = TRUE;
    DeviceData[nDevice].dwPosition = 0L;
    DeviceData[nDevice].nMode = QS_STOPPED;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsClose | Close a sample device.
 *
 * @parm int | nDevice | Device number to close.
 *
 * @rdesc TRUE on success, FALSE on failure (invalid device number).
 ***************************************************************************/
BOOL qsClose(int nDevice)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    DeviceData[nDevice].fInUse = FALSE;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | qsCurrentPosition | Calculate current device position based on
 *     system time if playing or from the static position.
 *
 * @parm int | nDevice | Channel to get position of.
 *
 * @rdesc The current position.
 ***************************************************************************/
DWORD qsCurrentPosition(int nDevice)
{
    /* if the device is currently playing */
    /* update the current position based on the current system time */
    if (DeviceData[nDevice].nMode == QS_PLAYING) {
        DWORD dwSystemTime = GetCurrentTime ();
        DeviceData[nDevice].dwPosition = 
                dwSystemTime -
                DeviceData[nDevice].dwTimeStartedPlaying +
                DeviceData[nDevice].dwPosition;
        
        DeviceData[nDevice].dwTimeStartedPlaying = dwSystemTime;

        /* if the device has passed the to position */
        if (DeviceData[nDevice].dwPosition >=
            DeviceData[nDevice].dwToPosition) {
            /* stop the device at the to position */
            DeviceData[nDevice].nMode = QS_STOPPED;
            DeviceData[nDevice].dwPosition = DeviceData[nDevice].dwToPosition;
        }
    }

    return DeviceData[nDevice].dwPosition;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsStop | Stop the sample device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @rdesc Returns TRUE on success, and FALSE if device number is invalid.
 ***************************************************************************/
BOOL qsStop(int nDevice)
{
    if (DeviceData[nDevice].nMode == QS_PLAYING)
        DeviceData[nDevice].dwPosition = qsCurrentPosition (nDevice);

    DeviceData[nDevice].nMode = QS_STOPPED;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsPause | Pause the sample device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @rdesc Returns TRUE on success, and FALSE if device number is invalid.
 ***************************************************************************/
BOOL qsPause(int nDevice)
{
    if (DeviceData[nDevice].nMode == QS_PLAYING)
        DeviceData[nDevice].dwPosition = qsCurrentPosition (nDevice);

    DeviceData[nDevice].nMode = QS_PAUSED;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsPlay | Play the sample device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @parm DWORD | dwFrom | From position.
 *
 * @parm DWORD | dwTo | To position.
 *
 * @rdesc Returns TRUE on success, and FALSE if device number is invalid.
 ***************************************************************************/
BOOL qsPlay(int nDevice, DWORD dwFrom, DWORD dwTo)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    if (!DeviceData[nDevice].fInUse)
        return FALSE;

    if (dwFrom > dwTo)
        return FALSE;

    DeviceData[nDevice].dwPosition = dwFrom;

    if (dwFrom == dwTo)
        return TRUE;

    DeviceData[nDevice].nMode = QS_PLAYING;

    DeviceData[nDevice].dwToPosition = dwTo;

    DeviceData[nDevice].dwTimeStartedPlaying = GetCurrentTime();

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsSeek | Seek on the sample device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @parm DWORD | dwTo | To position.
 *
 * @rdesc Returns TRUE on success, and FALSE if device number is invalid.
 ***************************************************************************/
BOOL qsSeek(int nDevice, DWORD dwTo)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    if (!DeviceData[nDevice].fInUse)
        return FALSE;

    DeviceData[nDevice].dwPosition = dwTo;

    DeviceData[nDevice].nMode = QS_STOPPED;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api BOOL | qsInfo | Dump all information about the device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @parm LPQS_INFO | lpInfo | Structure to hold data.
 *
 * @rdesc Returns TRUE on success, and FALSE if device number is invalid.
 ***************************************************************************/
BOOL qsInfo(int nDevice, LPQS_INFO lpInfo)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    if (lpInfo == NULL)
        return FALSE;

    /* if the device is currently playing */
    /* calculate the current position based on the current time */
    if (DeviceData[nDevice].nMode == QS_PLAYING)
        lpInfo->dwPosition = qsCurrentPosition (nDevice);

    /* else device is not playing */
    /* so current position is static */
    else                    
        lpInfo->dwPosition = DeviceData[nDevice].dwPosition;

    lpInfo->nMode = DeviceData[nDevice].nMode;
    lpInfo->dwLength = QS_MEDIA_LENGTH;

    return TRUE;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | qsCurrentMode | Return the current mode of the device.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @rdesc Returns mode.
 ***************************************************************************/
int qsCurrentMode(int nDevice)
{
    if (!QS_VALID_DEVICE(nDevice))
        return FALSE;

    /* if the device is currently playing */
    /* recalculate the mode to see if the device has stopped */
    if (DeviceData[nDevice].nMode == QS_PLAYING)
        qsCurrentPosition (nDevice);

    return DeviceData[nDevice].nMode;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | qsMediaLength | Return the media length.
 *
 * @parm int | nDevice | Device number to use.
 *
 * @rdesc Returns media length, or 0 on error.
 ***************************************************************************/
DWORD qsMediaLength(int nDevice)
{
    if (!QS_VALID_DEVICE(nDevice))
        return 0;

    return QS_MEDIA_LENGTH;
}
