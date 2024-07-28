/****************************************************************************
 *
 *   drvproc.c
 * 
 *   Driver Entry Point 
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include "ct.h"
#include "config.h"
#include "debug.h"

/* This sample driver includes support for the Installable Compressor
   interface.  To create a combined capture and codec driver, define
   _CODEC_SUPPORT to be "1" in the following statement.
*/
#define _CODEC_SUPPORT  0

#if _CODEC_SUPPORT
#include <compddk.h> 
#include <compman.h> 
#include "icsample.h"   // This file may have been renamed, modified
LRESULT FAR PASCAL _loadds DriverProcCodec(DWORD dwDriverID, 
        HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);
#endif

extern BOOL fVideoOpen;

//
//  we use this driverID to determine when we where opened as a video
//  device or from the control panel, etc....
//
#define BOGUS_DRIVER_ID     1

/*
DriverProc  The entry point for an installable driver.
                
DWORD dwDriverId  For most messages, dwDriverId is the DWORD
      value that the driver returns in response to a DRV_OPEN message.
      Each time that the driver is opened, through the DrvOpen API,
      the driver receives a DRV_OPEN message and can return an
      arbitrary, non-zero value. The installable driver interface
      saves this value and returns a unique driver handle to the 
      application. Whenever the application sends a message to the
      driver using the driver handle, the interface routes the message
      to this entry point and passes the corresponding dwDriverId.
      This mechanism allows the driver to use the same or different
      identifiers for multiple opens but ensures that driver handles
      are unique at the application interface layer.
 
      The following messages are not related to a particular open
      instance of the driver. For these messages, the dwDriverId
      will always be zero.
 
          DRV_LOAD, DRV_FREE, DRV_ENABLE, DRV_DISABLE, DRV_OPEN
 
HDRVR  hDriver  This is the handle returned to the
      application by the driver interface.
           
UINT  uiMessage  The requested action to be performed. Message
      values below DRV_RESERVED are used for globally defined messages.
      Message values from DRV_RESERVED to DRV_USER are used for
      defined driver protocols. Messages above DRV_USER are used
      for driver specific messages.
 
LPARAM  lParam1  Data for this message.  Defined separately for
      each message
 
LPARAM  lParam2  Data for this message.  Defined separately for
      each message
 
Returns: Defined separately for each message. 
*/

LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{

#if _CODEC_SUPPORT
    if (dwDriverID && (dwDriverID != BOGUS_DRIVER_ID)) {
        if (((PINSTINFO) dwDriverID) -> fccType == ICTYPE_VIDEO)
            return DriverProcCodec (dwDriverID, hDriver, uiMessage, 
                lParam1, lParam2);
    }
#endif

    switch (uiMessage)
    {
        case DRV_LOAD:
            D1("DRV_LOAD");

            /*
               Sent to the driver when it is loaded. Always the first
               message received by a driver.

               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return 0L to fail the load.

               DefDriverProc will return NON-ZERO so we don't have to
               handle DRV_LOAD
            */

            return (LRESULT)1L;

        case DRV_FREE:
            D1("DRV_FREE");

            /*
               Sent to the driver when it is about to be discarded. This
               will always be the last message received by a driver before
               it is freed. 

               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return value is ignored.
            */

            return (LRESULT)1L;

        case DRV_OPEN:
            D1("DRV_OPEN");
             
            /*
               Sent to the driver when it is opened. 

               dwDriverID is 0L.
               
               lParam1 is a far pointer to a zero-terminated string
               containing the name used to open the driver.
               
               lParam2 is passed through from the drvOpen call. It is
               NULL if this open is from the Drivers Applet in control.exe
               It is LPVIDEO_OPEN_PARMS otherwise.
                
               Return 0L to fail the open.
 
               DefDriverProc will return ZERO so we do have to
               handle the DRV_OPEN message.
             */

            //
            //  if we were opened without a open structure then just
            //  return a phony (non zero) id so the OpenDriver() will
            //  work.
            //
            if (lParam2 == NULL)
                return BOGUS_DRIVER_ID;

#if _CODEC_SUPPORT
            if (((ICOPEN FAR *) lParam2) -> fccType == ICTYPE_VIDEO)
                return DriverProcCodec (dwDriverID, hDriver, uiMessage, 
                        lParam1, lParam2);
#endif
            //  Verify this open is for us, and not for an installable
            //  codec.
            if (((LPVIDEO_OPEN_PARMS) lParam2) -> fccType != OPEN_TYPE_VCAP)
                return 0L;

            return (DWORD)(WORD)
                VideoOpen ((LPVIDEO_OPEN_PARMS) lParam2);

        case DRV_CLOSE:
            D1("DRV_CLOSE");

            /*
               Sent to the driver when it is closed. Drivers are unloaded
               when the close count reaches zero. 

               dwDriverID is the driver identifier returned from the
               corresponding DRV_OPEN.
               
               lParam1 is passed through from the drvClose call.

               lParam2 is passed through from the drvClose call.
                
               Return 0L to fail the close.

               DefDriverProc will return ZERO so we do have to
               handle the DRV_CLOSE message.
            */

            if (dwDriverID == BOGUS_DRIVER_ID || dwDriverID == 0)
                return 1L;      // return success

            return ((VideoClose((PCHANNEL)dwDriverID) == DV_ERR_OK) ? 1L : 0);
           
        case DRV_ENABLE:
            D1("DRV_ENABLE");
 
            /*
               Sent to the driver when the driver is loaded or reloaded
               and whenever Windows is enabled. Drivers should only
               hook interrupts or expect ANY part of the driver to be in
               memory between enable and disable messages
 
               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return value is ignored.
            */
//            if( fVideoOpen )
//                IRQEnable();
            return (LRESULT)1L;

        case DRV_DISABLE:
            D1("DRV_DISABLE");

            /*
               Sent to the driver before the driver is freed.
               and whenever Windows is disabled
 
               dwDriverID is 0L. 
               lParam1 is 0L.
               lParam2 is 0L.
                
               Return value is ignored.
            */

//            if( fVideoOpen )
//                IRQDisable();
            return (LRESULT)1L;

        case DRV_QUERYCONFIGURE:
            D1("DRV_QUERYCONFIGURE");

            /*
                Sent to the driver so that applications can
                determine whether the driver supports custom
                configuration. The driver should return a
                non-zero value to indicate that configuration
                is supported.
                
                dwDriverID is the value returned from the DRV_OPEN
                call that must have succeeded before this message
                was sent.
 
                lParam1 is passed from the app and is undefined.
                lParam2 is passed from the app and is undefined.

                Return 0L to indicate configuration NOT supported.
            */

            return (LRESULT)1L;        // we do configuration

        case DRV_CONFIGURE:
            D1("DRV_CONFIGURE");

            /*
                Sent to the driver so that it can display a custom
                configuration dialog box.

                lParam1 is passed from the app. and should contain
                the parent window handle in the loword.
                lParam2 is passed from the app and is undefined.

                Return value is undefined.

                Drivers should create their own section in system.ini.
                The section name should be the driver name.
            */

            return (LRESULT)Config((HWND)lParam1, ghModule);

        case DRV_INSTALL:
            D1("DRV_INSTALL");
            /*
                Windows has installed the driver, and since we
                are loaded on demand, there is no need to 
                restart Windows.
            */
            return (LRESULT)DRV_OK;

        case DRV_REMOVE:
            D1("DRV_REMOVE");
            ConfigRemove();
            return (LRESULT)DRV_OK;

        case DVM_GETVIDEOAPIVER: /* lParam1 = LPDWORD */
             if (lParam1) {
                 *(LPDWORD) lParam1 = VIDEOAPIVERSION;
                 return DV_ERR_OK;
             }
             else
                return DV_ERR_PARAM1;

        default:
            if (dwDriverID == BOGUS_DRIVER_ID || dwDriverID == 0)
                return DefDriverProc(dwDriverID, hDriver, uiMessage,lParam1,lParam2);

            // FIX, check range?
            return VideoProcessMessage((PCHANNEL)dwDriverID, uiMessage, lParam1, lParam2);
    }
}

