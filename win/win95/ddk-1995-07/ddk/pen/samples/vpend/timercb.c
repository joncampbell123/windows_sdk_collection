/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* TIMERCB.c: 

   This file contains the code that calls the pen driver and the timer
   callback entry point.<nl>

   Routines contained in module:<nl>
      <l cCall_Installable_Driver.cCall_Installable_Driver><nl>
      <l cTimer_Callback_Entry_Point.cTimer_Callback_Entry_Point><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>
#include <vtd.h>
#ifdef DEBUG
#include <Debug.h>
#endif

#include "PenDrv.h"
#include "protos.h"
#include "sysutil.h"

#pragma VxD_LOCKED_DATA_SEG

// external list
extern P_VpenD_Register ActiveDevice;

#ifdef DEBUG
extern P_VpenD_Register NullDevice;
extern PCOMMANDS gpCommandStrs;
extern PRATETABLE gpRateTable;
#endif

DWORD ddContinuous;
extern DWORD ddSafeToSendTimerTicks;
extern DWORD ddCurrently_in_an_int;

extern DWORD ddProfile_diff;
extern DWORD ddProfile_start;

extern LPDRV_PENINFO pVMPenInfo;
extern P_HARDWAREINFO pVMHwInfo;

extern DWORD ddCurrentTime;

#ifdef DEBUG
extern DWORD ddSequentialNum;
extern DWORD ddDebugErrors;
#endif

extern DWORD ddOEM_Data;
extern DWORD dwInPlayBackMode;

extern void VpenD_Call_Installable_Driver(void);

// If driver is executing, this is set.
DWORD gdwEntered = 0;
WORD gwPDKLast = 0;
DWORD gddTimeLastCall = 0;

#pragma VxD_LOCKED_CODE_SEG


DWORD cCall_Installable_Driver(DWORD dwVMHandle,
            DWORD dwClientRegStruc,
            DWORD dwInfo)
{
   DWORD ibNextTmp;
   DWORD ibThisTmp;
   DWORD ibThis_offsetTmp;
   DWORD ibNext_offsetTmp;
   DWORD dwRet;
   DWORD dwNum;
   DWORD ddProfile_stop;

#ifdef DEBUG_FOO
   Dbg_MiscPtrs(pVMHwInfo,pVMPenInfo,gpRateTable,
       gpCommandStrs,ActiveDevice,NullDevice);
#endif

   // prevent re-entrancy
   gdwEntered++;
   if(gdwEntered > 1 )
   {
      DBG_TRACE("Re-Entered more then once: cCall_Installable_Driver");
      gdwEntered--;
      return 1;
   }

   switch(dwInfo)
   {
   case DT_OEM:
      {
      // Record the number of bytes in OEM buffer and mark the
      // data type.  Recommend that OEM NULL terminates their
      // data.
      //ActiveDevice.piNumberOfPenPackets=dwNum;

      *(PDWORD)(ActiveDevice->pDataType)=dwInfo;
      // set ddOEM_Data back to zero
      ddOEM_Data=0;
      }
      break;

   // Note: Add OOR notification.
   // Note: Add power management notification.

   case DT_PENPACKET:
   case DT_OORPENPACKET:
   {
#ifdef DEBUG
      int i;
#endif
      // Turn off interrupts and take a count of the number of pen packets!
      _asm pushfd
      _asm cli

      ibNextTmp=ActiveDevice->ibNext;
      ibThisTmp=ActiveDevice->ibThis;

      ibThis_offsetTmp=ActiveDevice->ibThis_offset;

      ActiveDevice->ibThis=ibNextTmp;

      ibNext_offsetTmp=ActiveDevice->ibNext_offset;
      ActiveDevice->ibNext_offset=ibNext_offsetTmp;

      _asm popfd

      if(ibNextTmp==ibThisTmp)
      {
          // DBG_MISC(ibThisTmp,"ibThisTmp==ibNextTmp");
          // no pen packets to send...
          ddCurrently_in_an_int=0;
          gdwEntered--;
          return (DWORD)0;
      }
      else
      {
         // There must be something waiting to be serviced.
         // Calculate the number of pen packets waiting.
         if(ibThisTmp > ibNextTmp)
         {
         dwNum=(32-ibThisTmp)+ibNextTmp;
         ddContinuous=0;   // nope
         }
         else
         {
         dwNum=ibNextTmp-ibThisTmp;
         ddContinuous=1;   // yep
         }

/*
      NOTE: Removed the time stamp functionality for 3.x.
      // Do we need to stamp the pen packets?

      if( (ddVMMVersion < WIN_VER_40) && (dwNum!=0) )
      {   // Yes.  Make up some times.

         DWORD dwi;
         LPDRV_PENPACKET lpTmp;
         DWORD ddDelta;
         DWORD dwIndex=ibThisTmp;

         ddCurrentTime=_VTD_Get_Real_Time();
         if(ddCurrentTime == 0)
         {
            DBG_TRACE("TimerCB.c: ddCurrentTime==0");
         }

         // ddDelta is the amount of time that should be added
         // to the last time call.
         ddDelta=(ddCurrentTime-gddTimeLastCall)/dwNum;

         for(dwi=0;dwi<dwNum;dwi++)
         {
            lpTmp=&ActiveDevice.pPenPacketBuffer[dwIndex];

            // place time in pen packet
            lpTmp->ddTimeStamp=gddTimeLastCall+(ddDelta*(dwi+1));

            // advance index into buffer
            dwIndex++;
            if(dwIndex==32)  // at the end, wrap
               dwIndex=0;
         }
         gddTimeLastCall=ddCurrentTime;
      }
*/

      // record information for driver in VM
      *(PDWORD)(ActiveDevice->pDataType)=dwInfo;
      *(PDWORD)(ActiveDevice->piNumberOfPenPackets)=dwNum;
      *(PDWORD)(ActiveDevice->piThis_Offset)=ibThis_offsetTmp;
      *(PDWORD)(ActiveDevice->piThis)=ibThisTmp;

      {
      DWORD dwi;
      LPDRV_PENPACKET lpTmp;
      DWORD dwIndex=ibThisTmp;

      for(dwi=0;dwi<dwNum;dwi++)
         {
            lpTmp=&ActiveDevice->pPenPacketBuffer[dwIndex];

            if(lpTmp->wTabletX & 0xC000)
              {
#ifdef DEBUG
               DBG_TRACE("X is too large");
               dbg_penpacket(lpTmp);
#endif // DEBUG

               ddCurrently_in_an_int=0;
               gdwEntered--;
               return 0;
              }
              if(lpTmp->wTabletY & 0xC000)
              {
#ifdef DEBUG
               DBG_TRACE("Y is too large");
               dbg_penpacket(lpTmp);
#endif // DEBUG
               ddCurrently_in_an_int=0;
               gdwEntered--;
               return 0;
              }

              if(lpTmp->wPDK & ~(0x4007) )
              {
#ifdef DEBUG
               DBG_MISC(dwi,"Index");
               DBG_MISC(dwNum,"Total number");
               if(ddContinuous)
                  DBG_MISC(ddContinuous,"Continuous");
               else
                  DBG_MISC(ddContinuous,"Not Continuous");

               dbg_penpacket(lpTmp);
#endif
               ddCurrently_in_an_int=0;
               gdwEntered--;
               return 0;
            }
              else
            {
               if(gwPDKLast != lpTmp->wPDK)
               {
                  gwPDKLast=lpTmp->wPDK;
#ifdef DEBUG
                  dbg_penpacket(lpTmp);
#endif
               }
            }
            dwIndex++;
            if(dwIndex==32)  // at the end, rap
               dwIndex=0;
         }
      }
      }
   }
   break;

   default:
      DBG_ERROR(dwInfo,"Unknown type in callback function!");
      ddCurrently_in_an_int=0;
      gdwEntered--;
      return 0;
      break;
   }

   // If code has executed to this point, need to send the info...
#ifdef DEBUG
   if(pVMHwInfo->dwHwFlags & HW_PROFILE)
   {
      ddProfile_start=_VTD_Get_Real_Time();
   }
#endif

   CallPenDriverInVM();

#ifdef DEBUG
   if(pVMHwInfo->dwHwFlags & HW_PROFILE)
   {
      ddProfile_diff=(_VTD_Get_Real_Time()-ddProfile_start);
   }
#endif

   ddCurrently_in_an_int=0;
   gdwEntered--;
   return 1;
}


void cTimer_Callback_Entry_Point()
{
   if( !dwInPlayBackMode)
   {   // If in playback mode, do not send any packets!

   if(!ddCurrently_in_an_int)
   {
      DWORD ibNextTmp;
      DWORD ibThisTmp;
      DWORD dwType;

      if(ddSafeToSendTimerTicks)
      {
#ifdef DEBUG_FOO
      Dbg_MiscPtrs(pVMHwInfo,pVMPenInfo,gpRateTable,
         gpCommandStrs,ActiveDevice,NullDevice);
#endif
      if(pVMHwInfo->dwHwFlags & HW_PUSHMODEL)
      {
         DWORD dwMilliTime;

         // NOTE:  ddCurrentTime is a global variable that gets set
         // each time a pen packet is gathered when running on Windows 95.
         // The variable gets set every time a pen packet is sent
         // when running on Windows 3.11 or less.  Use
         // ddCurrentTime to see when the last pen packet was sent into
         // the system.   If some delay time has passed,
         // make up a pen packet and send it in!

         // delay=(_VTD_Get_Real_Time()-ddCurrentTime);

         if( (dwMilliTime=_VTD_Get_Real_Time()) == 0)
            DBG_TRACE("TimerCB.c: TimeStamp==0");

         if(pVMHwInfo->ddDelayTime < (dwMilliTime - ddCurrentTime) )
            dwType=DT_TIMERTICK;
         else
            dwType=(DWORD)0;
      }
      if(pVMHwInfo->dwHwFlags & HW_TIMERMODEL)
      {
         if(ddOEM_Data)
         dwType=DT_OEM;
         else
         {
            _asm pushfd
            _asm cli

            // Are there pen packets to be serviced?
            ibNextTmp=ActiveDevice->ibNext;
            ibThisTmp=ActiveDevice->ibThis;
            _asm popfd

            if(ibThisTmp!=ibNextTmp)
               dwType=DT_PENPACKET;
            else
               dwType=DT_TIMERTICK;
         }
      }

      if(dwType==DT_TIMERTICK)
      {
          // create OOR pen packet
          LPDRV_PENPACKET pPP;

#ifdef DEBUG
          //DBG_TRACE("TimerCB.c Making Up Pen Packet!");
#endif
          _asm pushfd
          _asm cli

          pPP=(LPDRV_PENPACKET)&ActiveDevice->pPenPacketBuffer[ActiveDevice->ibNext];
          pPP->wTabletX=100;
          pPP->wTabletY=100;
          pPP->wPDK=PDK_OUTOFRANGE;
          pPP->ddTimeStamp=_VTD_Get_Real_Time();
          if(pPP->ddTimeStamp == 0)
          {
             DBG_TRACE("TimerCB.c: TimeStamp==0");
          }

#ifdef DEBUG
          // Now look into debug information.
          if(pVMHwInfo->dwHwFlags & HW_SEQUENTIAL)
          {
         // want sequential information
         ddSequentialNum++;
         pPP->ddSequential=ddSequentialNum;
          }
          if(pVMHwInfo->dwHwFlags & HW_PROFILE)
          {
         // want profile information
         pPP->ddProfile=ddProfile_diff;
          }
          if(pVMHwInfo->dwHwFlags & HW_DEBUG)
          {
         // want debug information
         pPP->ddDebug=ddDebugErrors;
         ddDebugErrors=0;
          }
#endif
          // Increment ibNext and wrap if needed.
          if(ActiveDevice->ibNext>=31)
          {
         ActiveDevice->ibNext=0;
         ActiveDevice->ibNext_offset=0;
          }
          else // just advance one unit
          {
         ActiveDevice->ibNext++;
         ActiveDevice->ibNext_offset+=sizeof(DRV_PENPACKET);
          }

          _asm popfd
          dwType=DT_OORPENPACKET;
      }
      if(dwType)
      {
          // This will get set to zero at end of service routine.
          ddCurrently_in_an_int=1;
          _Call_Priority_VM_Event(HIGH_PRI_DEVICE_BOOST,
                  PEF_WAIT_FOR_STI,
                  dwType,
                  (DWORD)&VpenD_Call_Installable_Driver);
      }
       }
   }
   } // end dwInPlayBackMode
   // schedule another timer tick
    if(pVMHwInfo->dwHwFlags & HW_TIMERMODEL)
    {
   Set_Global_Time_Out(&cTimer_Callback_Entry_Point,
               pVMHwInfo->ddDelayTime,0);
    }
    else
    {
   Set_Global_Time_Out(&cTimer_Callback_Entry_Point,
               pVMHwInfo->ddTimerTickRate,0);
    }
}

//End-Of-File
