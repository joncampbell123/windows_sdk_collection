//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------
//
//  Module:   drvproc.c
//
//  Description:
//     MSOPL driver procedure
//
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include "msopl.h"

//--------------------------------------------------------------------------
//  
//  DWORD DriverProc
//  
//  Description:
//      The entry point for an installable driver.
//  
//  Parameters:
//      DWORD dwDriverID
//
//      For most messages, dwDriverId is the DWORD
//      value that the driver returns in response to a DRV_OPEN message.
//      Each time that the driver is opened, through the DrvOpen API,
//      the driver receives a DRV_OPEN message and can return an
//      arbitrary, non-zero value. The installable driver interface
//      saves this value and returns a unique driver handle to the
//      application. Whenever the application sends a message to the
//      driver using the driver handle, the interface routes the message
//      to this entry point and passes the corresponding dwDriverId.
//      This mechanism allows the driver to use the same or different
//      identifiers for multiple opens but ensures that driver handles
//      are unique at the application interface layer.
//
//      The following messages are not related to a particular open
//      instance of the driver. For these messages, the dwDriverId
//      will always be zero.
//
//         DRV_LOAD, DRV_FREE, DRV_ENABLE, DRV_DISABLE, DRV_OPEN
//  
//      HANDLE hDriver
//
//      This is the handle returned to the application by the driver
//      interface.
//
//      WORD wMessage
//
//      The requested action to be performed. Message
//      values below DRV_RESERVED are used for globally defined messages.
//      Message values from DRV_RESERVED to DRV_USER are used for
//      defined driver protocols. Messages above DRV_USER are used
//      for driver specific messages.
//  
//      DWORD dwParam1
//
//      Data for this message.  Defined separately for each message
//
//      DWORD dwParam2
//
//      Data for this message.  Defined separately for each message
//  
//  Return (DWORD):
//      Defined separately for each message.
//
//--------------------------------------------------------------------------

LRESULT FAR PASCAL _loadds DriverProc
(
    DWORD           dwDriverID,
    HDRVR           hDriver,
    UINT            uMessage,
    LPARAM          lParam1,
    LPARAM          lParam2
)
{
   switch (uMessage)
   {
      case DRV_LOAD:

         //
         // Sent to the driver when it is loaded. Always the first
         // message received by a driver.
         //
         // dwDriverID is 0L.
         // lParam1 is 0L.
         // lParam2 is 0L.
         //
         // Return 0L to fail the load.
         //
         // DefDriverProc will return NON-ZERO so we don't have to
         // handle DRV_LOAD
         //

         DPF( 1, "DRV_LOAD" ) ;

         return 1L ;

      case DRV_FREE:

         // 
         // Sent to the driver when it is about to be discarded. This
         // will always be the last message received by a driver before
         // it is freed.
         // 
         // dwDriverID is 0L.
         // lParam1 is 0L.
         // lParam2 is 0L.
         // 
         // Return value is ignored.
         // 

         DPF( 1, "DRV_FREE" ) ;

         return 1L ;

      case DRV_OPEN:

         // 
         // Sent to the driver when it is opened.
         // 
         // dwDriverID is 0L.
         // 
         // lParam1 is a far pointer to a zero-terminated string
         // containing the name used to open the driver.
         // 
         // lParam2 is passed through from the drvOpen call.
         // 
         // Return 0L to fail the open.
         // 
         // DefDriverProc will return ZERO so we do have to
         // handle the DRV_OPEN message.
         // 

         DPF( 1, "DRV_OPEN" ) ;

         return 1L ;

      case DRV_CLOSE:

         // 
         // Sent to the driver when it is closed. Drivers are unloaded
         // when the close count reaches zero.
         // 
         // dwDriverID is the driver identifier returned from the
         // corresponding DRV_OPEN.
         // 
         // lParam1 is passed through from the drvOpen call.
         // 
         // lParam2 is passed through from the drvOpen call.
         // 
         // Return 0L to fail the close.
         // 
         // DefDriverProc will return ZERO so we do have to
         // handle the DRV_CLOSE message.
         // 

         DPF( 1, "DRV_CLOSE" ) ;

         return 1L ;

      case DRV_ENABLE:

         // 
         // Sent to the driver when the driver is loaded or reloaded
         // and whenever Windows is enabled. Drivers should only
         // hook interrupts or expect ANY part of the driver to be in
         // memory between enable and disable messages
         // 
         // dwDriverID is 0L.
         // lParam1 is 0L.
         // lParam2 is 0L.
         // 
         // Return value is ignored.
         // 

         DPF( 1, "DRV_ENABLE" ) ;

         if (DrvInit())
            return 0L ; // error

         return 1L ;

      case DRV_DISABLE:

         // 
         // Sent to the driver before the driver is freed.
         // and whenever Windows is disabled
         // 
         // dwDriverID is 0L.
         // lParam1 is 0L.
         // lParam2 is 0L.
         // 
         // Return value is ignored.
         // 

         DPF( 1, "DRV_DISABLE" ) ;

         DrvEnd() ;

         return 1L ;

      case DRV_QUERYCONFIGURE:

         //
         // Sent to the driver so that applications can
         // determine whether the driver supports custom
         // configuration. The driver should return a
         // non-zero value to indicate that configuration
         // is supported.
         // 
         // dwDriverID is the value returned from the DRV_OPEN
         // call that must have succeeded before this message
         // was sent.
         // 
         // lParam1 is passed from the app and is undefined.
         // lParam2 is passed from the app and is undefined.
         // 
         // Return 0L to indicate configuration NOT supported.
         // 

         DPF( 1, "DRV_QUERYCONFIGURE" ) ;

         return 0L ;

      case DRV_CONFIGURE:

         //
         // Sent to the driver so that it can display a custom
         // configuration dialog box.
         //
         // lParam1 is passed from the app. and should contain
         // the parent window handle in the loword.
         // lParam2 is passed from the app and is undefined.
         // 
         // Return value is REBOOT, OK, RESTART.
         // 

         DPF( 1, "DRV_CONFIGURE" ) ;

         return 0L ;

      case DRV_INSTALL:
         DPF( 1, "DRV_INSTALL" ) ;
         return DRV_OK ;

      case DRV_REMOVE:
         DPF( 1, "DRV_REMOVE" ) ;
         return 0 ;
               
      //
      // This notification is from the Windows power driver.  For more
      // information about APM and how it works under Windows, please
      // see:  Windows 3.1 Advanced Power Management (APM) Release Notes
      //

      case DRV_POWER:
         DPF( 1, "DRV_POWER" ) ;

         switch (lParam1)
         {
            case PWR_SUSPENDREQUEST:

               // If we decided we wanted to try to cause the machine to
               // not suspend, we would:
               // return PWR_FAIL;
               // However, this does NOT guarantee that the machine will not
               // be suspended (the APM BIOS may override the driver's wishes).

               break ;

            case PWR_SUSPENDRESUME:

               // This notification is just for information.  PWR_OK should
               // be returned regardless of what the driver does to resume
               // normal hardware operations.

               break ;

            case PWR_CRITICALRESUME:

               // This notification is just for information.  This message
               // indicates that the APM BIOS suspended the system without
               // prior notice.  PWR_OK should be returned regardless of what
               // the driver does to resume normal hardware operations.
               break ;
         }
         return PWR_OK ;
   }

   return DefDriverProc( dwDriverID, hDriver, uMessage, 
                         lParam1, lParam2 ) ;

} // DriverProc()

//---------------------------------------------------------------------------
//  End of File: drvproc.c
//---------------------------------------------------------------------------
