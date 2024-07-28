/****************************************************************************
 *
 *   ibmjoy.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 *   IBM generic game port joystick device driver - entry point,
 *   initialization and message module
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "ibmjoy.h"

/*********************************************************************

    Public data

 ********************************************************************/

/* joystick calibration values used by Poll function in file
 * "poll.asm" to give info to GetPos
 */
JOYCALIBRATE JoyCal[2] = { { 0, 100, 0, 100, 0, 100 },
                           { 0, 100, 0, 100, 0, 100 } };

WORD   gwXpos;                          /* joystick device X position */
WORD   gwYpos;                          /* joystick device Y position */
WORD   gwZpos;                          /* joystick device Z position */
DWORD  gdwTimeout;                      /* timeout value */
WORD   gwButtons;                       /* joystick device button state */
WORD   gwcAxes;                         /* how many axes we have */
WORD   gwDebugLevel = 0;                /* debug level */
char   gszDriverName[] = "ibmjoy.drv";  /* driver name */
char   gszIniFile[] = "system.ini";     /* where the gszDriverName section is */
char   gszAxes[] = "Axes";              /* keyword used for number of axes */
char   gszTimeout[] = "Timeout";        /* keyword used for timeout */
char   gszDevCapsDescription[] = "Microsoft PC-joystick driver";
HANDLE ghInstance = NULL;               /* our instance handle */

/*********************************************************************

    Local data

 ********************************************************************/

static WORD iZaxis;

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | LibMain | Library initialization code.
 *
 * @parm HANDLE | hInstance | Our instance handle.
 *
 * @parm WORD | wDataSeg | Our data segment.
 *
 * @parm WORD | wHeapSize | The heap size from the .def file.
 *
 * @parm LPSTR | lpCmdLine | The command line.
 *
 * @rdesc Returns 1 if the initialization was successful and 0 otherwise.
 ***************************************************************************/
int PASCAL LibMain(HANDLE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpCmdLine)
{
    /* get debug level - default is 0 */
    gwDebugLevel = GetProfileInt("mmdebug", "ibmjoy", 0);

    D1("LibMain");

    ghInstance = hInstance;

    return 1;                            /* exit ok */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | GetDevCaps | Get the device capabilities.
 *
 * @parm BYTE FAR * | lpJoyCaps | Far pointer to a JOYCAPS structure
 *     to receive the information.
 *
 * @parm WORD | wSize | The size of the JOYCAPS structure.
 *
 * @rdesc The return value is always zero.
 ***************************************************************************/
DWORD GetDevCaps(BYTE FAR * lpCaps, WORD wSize)
{
JOYCAPS jc;                     /* local copy */
LPBYTE  cp;
WORD    w;

    jc.wMid = MM_MICROSOFT;     /* manufacturer id */
    jc.wPid = MM_PC_JOYSTICK;   /* product id */

    jc.wXmin = 0;               /* set x position range */
    jc.wXmax = 65535;

    jc.wYmin = 0;               /* set y position range */
    jc.wYmax = 65535;

    jc.wZmin = 0;               /* set z position range */
    jc.wZmax = (gwcAxes == 2) ? 0 : 65535;

    jc.wPeriodMin = 10;         /* set period range */
    jc.wPeriodMax = 1000;

    jc.wNumButtons = 2;         /* number of buttons */

    lstrcpy(jc.szPname, gszDevCapsDescription);

    w = min(wSize, sizeof(JOYCAPS));
    cp = (BYTE FAR *)&jc;
    while(w--) *lpCaps++ = *cp++;

    return MMSYSERR_NOERROR;    /* no error */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | GetPos | Get the position of a joystick.
 *
 * @parm WORD | id | The device being queried.
 *
 * @parm LPJOYINFO | lpInfo | Far pointer to a structure to fill
 *      with the position info.
 *
 * @rdesc The return value is zero if the position was obtained. Otherwise
 *      it is an error code.
 ***************************************************************************/
DWORD GetPos(WORD id, LPJOYINFO lpInfo)
{
    if (id > 1)
        return JOYERR_PARMS;              /* only 2 ports supported (0 and 1) */

    lpInfo->wZpos = 0;

    if (gwcAxes == 3) {
        if (id == 1)                      /* if 3D, only id 0 is valid */
            return JOYERR_UNPLUGGED;
        else {
            /* Poll will return 0 on error */
            if (!Poll(1, iZaxis))         /* get z position */
                return JOYERR_UNPLUGGED;
            lpInfo->wZpos = gwYpos;
            if (!Poll(0, 0))              /* get x and y positions */
                return JOYERR_UNPLUGGED;
        }
    }
    else if (!Poll(id, 0))                /* if 2D, either id is valid */
        return JOYERR_UNPLUGGED;

    lpInfo->wButtons = gwButtons;
    lpInfo->wXpos = gwXpos;
    lpInfo->wYpos = gwYpos;

    return MMSYSERR_NOERROR;              /* no error */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | ibmjoyGetTimeoutValue | Gets the timeout value from the
 *     gszIniFile and stores it in a global variable.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void FAR PASCAL ibmjoyGetTimeoutValue(void)
{
WORD    w;

    w = GetPrivateProfileInt(gszDriverName, gszTimeout, DEF_TIMEOUT,gszIniFile);
    if (!w)
        w = DEF_TIMEOUT;

    gdwTimeout = (DWORD)w << 4;
}

/***************************************************************************
 * @doc INTERNAL
 *
 * @api LONG | DriverProc | The entry point for an installable driver.
 *                
 * @parm DWORD | dwDriverId | For most messages, <p dwDriverId> is the DWORD
 *     value that the driver returns in response to a <m DRV_OPEN> message.
 *     Each time that the driver is opened, through the <f DrvOpen> API,
 *     the driver receives a <m DRV_OPEN> message and can return an
 *     arbitrary, non-zero value. The installable driver interface
 *     saves this value and returns a unique driver handle to the 
 *     application. Whenever the application sends a message to the
 *     driver using the driver handle, the interface routes the message
 *     to this entry point and passes the corresponding <p dwDriverId>.
 *     This mechanism allows the driver to use the same or different
 *     identifiers for multiple opens but ensures that driver handles
 *     are unique at the application interface layer.
 *
 *     The following messages are not related to a particular open
 *     instance of the driver. For these messages, the dwDriverId
 *     will always be zero.
 *
 *         DRV_LOAD, DRV_FREE, DRV_ENABLE, DRV_DISABLE, DRV_OPEN
 *
 * @parm HANDLE  | hDriver | This is the handle returned to the
 *     application by the driver interface.
 *          
 * @parm WORD | wMessage | The requested action to be performed. Message
 *     values below <m DRV_RESERVED> are used for globally defined messages.
 *     Message values from <m DRV_RESERVED> to <m DRV_USER> are used for
 *     defined driver protocols. Messages above <m DRV_USER> are used
 *     for driver specific messages.
 *
 * @parm LONG | lParam1 | Data for this message.  Defined separately for
 *     each message
 *
 * @parm LONG | lParam2 | Data for this message.  Defined separately for
 *     each message
 *
 * @rdesc Defined separately for each message. 
 ***************************************************************************/
LONG FAR PASCAL DriverProc(DWORD dwDriverID, HANDLE hDriver, WORD wMessage, LONG lParam1, LONG lParam2)
{
char buf[6];                /* to put number of axes in gszIniFile */
static WORD wDevMax;        /* maximum number of joysticks */

    switch (wMessage) {

        case JDD_GETNUMDEVS:
            return MAKELONG(wDevMax, 0);

        case JDD_GETDEVCAPS:
            return GetDevCaps((BYTE FAR *)lParam1, (WORD)lParam2);

        case JDD_GETPOS:
            /* make device ID zero-based and get position */
            return GetPos(LOWORD(dwDriverID)-1, (LPJOYINFO)lParam1);

        case JDD_SETCALIBRATION:
            #define lpNewCal  ((LPJOYCALIBRATE)(lParam1))
            #define lpOldCal  ((LPJOYCALIBRATE)(lParam2))

            *lpOldCal = JoyCal[ LOWORD(dwDriverID)-1 ];

            JoyCal[ LOWORD(dwDriverID)-1 ] = *lpNewCal;

            return MMSYSERR_NOERROR;

            #undef lpOldCal
            #undef lpNewCal

        case DRV_OPEN:

           /* lParam2 comes from mmsystem and is either 0 or 1 for the 
            * first and second joystick respectively.  wDevMax is set at
            * DRV_ENABLE time and is 1 if we have a 3D joystick and 2
            * otherwise (regardless of whether we actually have 1 or 2
            * 2D joysticks attached).
            */
            if (lParam2 < wDevMax)
                return (lParam2 + 1);
            else
                return 0L;

        case DRV_LOAD:
        case DRV_FREE:
        case DRV_CLOSE:
        case DRV_DISABLE:
            return 1L;

        case DRV_ENABLE:
            /* Get time-out value from INI file */
            ibmjoyGetTimeoutValue();
            gwcAxes = GetPrivateProfileInt(gszDriverName, gszAxes, DEF_AXES, gszIniFile);
            if (gwcAxes == 3) {
                if (Poll(1, 1)) {
                    D1("Configured for 1, 3 axis joystick on x");
                    iZaxis = 1;
                }
                else {
                    D1("Configured for 1, 3 axis joystick on y");
                    iZaxis = -1;
                }
                wDevMax = 1;
            }
            else {
                D1("Configured for 2, 2 axis joysticks");
                gwcAxes = 2;
                wDevMax = 2;
            }
            return 1L;

        case DRV_QUERYCONFIGURE:
            return 1L;   /* indicates that configuration is supported */

        case DRV_CONFIGURE:
            return Config(LOWORD(lParam1), ghInstance);

        case DRV_INSTALL:
            wsprintf(buf, "%d", DEF_AXES);
            WritePrivateProfileString(gszDriverName, gszAxes, buf, gszIniFile);
            return 1L;

        case DRV_REMOVE:
            WritePrivateProfileString(gszDriverName, NULL, NULL, gszIniFile);
            return 1L;

        default:
            return DefDriverProc(dwDriverID, hDriver, wMessage, lParam1, lParam2);
        }
}
