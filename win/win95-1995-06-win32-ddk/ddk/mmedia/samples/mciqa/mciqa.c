/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 *
 *  MCIQA.C
 *
 *  Sample MCI Device Driver
 *
 *      Main Module - Standard Driver Interface and Message Procedures
 *
 ***************************************************************************/

/*  This sample MCI device driver consists of three source modules:
 *
 *  MCIQA.C  -   Non-MCI specific initialization and driver interface code
 *
 *  MCICMDS.C  - Handlers for the MCI specific messages.  Calls the device
 *               APIs contained in qasample.c
 *
 *  QASAMPLE.C - A self contained very simple device with mode and position
 *               which is used as a testbed for this sample MCI driver
 *
 *
 *  This sample driver is a simple MCI device driver which allows for a
 *  fixed number of shareable channels to be open.  The channel number is
 *  specified as a single integer parameter on the device line of the [mci]
 *  section of SYSTEM.INI.
 *
 *  'sample' is the name which should be used for this device in SYSTEM.INI
 *
 *  This device is of type MCI_DEVTYPE_OTHER.  Since it
 *  is just a sample device, the device name used in the SYSTEM.INI file
 *  is arbitrary.  For most user devices of type MCI_DEVTYPE_OTHER, a
 *  standard name should be used to identify the device name and should
 *  be specified in the installation notes for the device or used by
 *  installation software when writing to the [mci] section.
 */

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "mciqa.h"

HANDLE  hModuleInstance;        /* handle to this instance of the module */

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | LibMain | Library initialization code.
 *
 * @parm HANDLE | hModule | Our instance handle.
 *
 * @parm WORD | cbHeap | The heap size from the .def file.
 *
 * @parm LPSTR | lpszCmdLine | The command line.
 *
 * @rdesc Returns 1 if the initialization was successful and 0 otherwise.
 ***************************************************************************/
int NEAR PASCAL LibMain(HANDLE hModule, WORD cbHeap, LPSTR lpszCmdLine)
{
    hModuleInstance = hModule;

    /* call MCI specific initialization code */
    return mqInitialize();
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | mqDrvLoad | Respond to the DRV_LOAD message.  Perform any
 *     one-time initialization.
 *
 * @rdesc Returns 1L on success and 0L on failure.
 ***************************************************************************/
DWORD mqDrvLoad(void)
{
    return 1L;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | mqDrvFree | Respond to the DRV_FREE message.  Perform any
 *     device shutdown tasks.
 *
 * @rdesc Returns 1L on success and 0L on failure.
 ***************************************************************************/
DWORD mqDrvFree(void)
{
    return 1L;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | mqDrvOpen | Respond to the DRV_OPEN message.  Perform any
 *     initialization which is done once each time the driver is opened.
 *
 * @parm LPSTR | lpstrParams | NULL terminated command line string contains
 *     any characters following the filename in the SYSTEM.INI file.
 *
 * @parm LPMCI_OPEN_DRIVER_PARMS | lpOpen | Pointer to an
 *     MCI_OPEN_DRIVER_PARMS structure with information about this device.
 *
 * @rdesc Returns zero on failure or the driver ID that should be passed
 *      to identify this device on subsequent messages.
 *
 * @comm In this driver, the DRV_OPEN message is used to parse the
 *     parameters string and fill in the device type and custom command
 *     table.  The OPEN_DRIVER message is MCI specific and is used to
 *     register this device with the sample device.
 ***************************************************************************/
DWORD mqDrvOpen(LPSTR lpstrParams, LPMCI_OPEN_DRIVER_PARMS lpOpen)
{
    int nChannel = 0;

    dprintf("DRV_OPEN Params:'%ls'",
            lpOpen->lpstrParams ? lpOpen->lpstrParams : (LPSTR)"");

    /* determine what channel is being opened by looking at system.ini */
    /* parameters - default to channel 0. */
    if (lpstrParams != NULL) {

        /* remove leading blanks */
        while (*lpstrParams == ' ')
            ++lpstrParams;

        /* only allow one character for channel number */
        if (*lpstrParams != '\0') {

            /* ignore if that character is not a digit */
            if (*lpstrParams >= '0' || *lpstrParams <= '9') {
                nChannel = *lpstrParams - '0';

                /* fail if channel is out of range */
                if (nChannel >= MCIQA_MAX_CHANNELS)
                    return 0L;
            }
        }
    }

    /*  this device is not one of the pre-defined types - specifying
     *  MCI_DEVTYPE_OTHER causes MCI to look for a device type specific
     *  command table with the name of the device from SYSTEM.INI
     *  used to open the file
     */
    lpOpen->wType = MCI_DEVTYPE_OTHER;
    lpOpen->wCustomCommandTable = (WORD)-1;
    
    mciSetDriverData(lpOpen->wDeviceID, nChannel);

    /* this return value will be passed in as dwDriverID in future calls. */
    return (DWORD)lpOpen->wDeviceID;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | mqDrvClose | Respond to the DRV_CLOSE message.  Perform
 *     any cleanup necessary each time the device is closed.
 *
 * @rdesc Returns TRUE on success and FALSE on failure.
 ***************************************************************************/
DWORD mqDrvClose(void)
{
    return 1L;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api WORD | Config | User configuration.
 *
 * @parm WORD | wInfo |
 *
 * @parm LPDRVCONFIGINFO | lpConfig | Config data.
 *
 * @parm HANDLE | hModule | Instance handle of module.
 *
 * @rdesc Currently returns TRUE.
 ***************************************************************************/
WORD Config(WORD wData, LPDRVCONFIGINFO lpConfig, HANDLE hModule)
{
    /*  Driver configuration code goes here.  This is usually a dialog
     *  box which gets user configuration information and uses it to write
     *  parameters to a .INI file using the file and section names provided
     *  in lpConfig.
     *
     *  Since this driver has no configuration it normally would have
     *  returned 0L for the DRV_QUERYCONFIGURE message.
     */

    return TRUE;
}

/***************************************************************************
 * @doc INTERNAL
 *
 * @api LONG | DriverProc | Windows driver entry point.  All Windows driver
 *     control messages and all MCI messages pass through this entry point.
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
LONG FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HANDLE hDriver, WORD wMessage, LONG lParam1, LONG lParam2)
{

    switch (wMessage) {

        case DRV_LOAD:
            /* the DRV_LOAD message is received once, when the driver is */
            /* first loaded - any one-time initialization code goes here */
            return mqDrvLoad();

        case DRV_FREE:
            /* the DRV_FREE message is received once when the driver is */
            /* unloaded - any final shut down code goes here */
            return mqDrvFree();

        case DRV_OPEN:
            /* the DRV_OPEN message is received once for each MCI device open */
            if (lParam2)
                return mqDrvOpen((LPSTR)lParam1,
                                 (LPMCI_OPEN_DRIVER_PARMS)lParam2);
            else
                return 1L;
            
        case DRV_CLOSE:
            /* this message is received once for each MCI device close */
            return mqDrvClose();

        case DRV_QUERYCONFIGURE:
            /* the DRV_QUERYCONFIGURE message is used to determine if the */
            /* DRV_CONCIGURE message is supported - return 1 to indicate */
            /* configuration is supported */
            return 1L;

        case DRV_CONFIGURE:
            /* the DRV_CONFIGURE message instructs the device to perform */
            /* device configuration. */
            if (lParam2 && lParam1 &&
                (((LPDRVCONFIGINFO)lParam2)->dwDCISize == sizeof(DRVCONFIGINFO)))
                return (DWORD)Config(LOWORD(lParam1),
                                     (LPDRVCONFIGINFO)lParam2,
                                     hModuleInstance);
            break;

        default:
            /* all other messages are processed here */
            
            /* select messages in the MCI range */
            if (!HIWORD(dwDriverID) && wMessage >= DRV_MCI_FIRST &&
                wMessage <= DRV_MCI_LAST)
                return mqMCIProc((WORD)dwDriverID, wMessage, lParam1, lParam2);
            /* other messages get default processing */
            else
                return DefDriverProc(dwDriverID, hDriver, wMessage, lParam1, lParam2);
    }                  
    return 0L;
}
