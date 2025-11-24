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

/* APIPROCS.c: 

   Contains a copy API message handling routines.<nl>

   Routines contained in module:<nl>
      <l cVpenD_API_Enable.cVpenD_API_Enable><nl>
      <l cVpenD_API_Disable.cVpenD_API_Disable><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <VMM.H>
#include <VPICD.H>
#include <VTD.H>
#ifdef DEBUG
#include <debug.h>
#endif

#include "PenDrv.h"
#include "protos.h"
#include "sysutil.h"

// external list
extern P_VpenD_Register ActiveDevice;
extern DWORD ddInstalled_Devices;
extern DWORD ddSafeToSendTimerTicks;

extern P_HARDWAREINFO pVMHwInfo;
extern LPDRV_PENINFO pVMPenInfo;

#ifdef WIN31COMPAT
#pragma VxD_LOCKED_CODE_SEG
#else
#pragma VxD_PAGEABLE_CODE_SEG
#endif

DWORD cVpenD_API_Enable(DWORD lParam1,DWORD lParam2)
{
   DWORD dwRet=DRV_FAILURE;

   // loading the first device
   if(ddInstalled_Devices==1)
   {
      DBG_TRACE("Physically Unmasking the IRQ before calling OEM code!!");
      VPICD_Physically_Unmask(pVMHwInfo->VpenD_IRQ_Handle);

      // turn on the tablet
      if(cOEM_API_Proc(VPEND_ENABLE,NULL,NULL))
      {
         // lParam1 - desired rate
         // lParam2 - Address of the global PENINFO structure
         pVMPenInfo->nSamplingRate=(int)cOEM_API_Proc(
                                 VPEND_SETSAMPLINGRATE,
                                 (DWORD)pVMPenInfo->nSamplingRate,
                                 (DWORD)pVMPenInfo);

         // Need to set sampling distance at smallest distance == 0.
         pVMPenInfo->nSamplingDist=(int)cOEM_API_Proc(
                                    VPEND_SETSAMPLINGDIST,
                                    (DWORD)0,
                                    (DWORD)pVMPenInfo);
//
// Time to turn on a Timer so out-of-range interrupts can be generated.
// Increment ddSafeToSendTimerTicks so if a timer tick is received the
// virtual pen driver will send the packet into the system.
//
         if(pVMHwInfo->dwHwFlags & HW_TIMERMODEL)
         {
            DBG_MISC(pVMHwInfo->ddDelayTime,"Starting timer tick callback");
            VTD_Begin_Min_Int_Period(pVMHwInfo->ddTimerTickRate);
            Set_Global_Time_Out(&cTimer_Callback_Entry_Point,
                           pVMHwInfo->ddDelayTime,0);
         }
         else
         {
            DBG_MISC(pVMHwInfo->ddTimerTickRate,"Starting timer tick callback");
            Set_Global_Time_Out(&cTimer_Callback_Entry_Point,
                           pVMHwInfo->ddTimerTickRate,0);
         }

         ddSafeToSendTimerTicks++;

         DBG_MISC(ddInstalled_Devices,"The number of installed devices");

         return ddInstalled_Devices;
      }
      else
      {
         DBG_WARNING(ddInstalled_Devices,"OEM failed to Enable device!!");
      }
   }
   else
   {
      DBG_WARNING(ddInstalled_Devices,"Device is already enabled, not calling OEM");
   }
   return dwRet;
}


DWORD cVpenD_API_Disable(DWORD lParam1,DWORD lParam2)
{
   DWORD dwRet=(DWORD)DRV_FAILURE;

   if(ddInstalled_Devices)
   {
      if(cOEM_API_Proc(VPEND_DISABLE,(DWORD)0,(DWORD)0))
      {
         int i;
         PDWORD pDword;

         if(pVMHwInfo->dwHwFlags & HW_TIMERMODEL)
         {
//
// Turn off out-of-range timer tick and decrement ddSafeToSendTimerTicks because
// it's no longer safe to send timer ticks!  If a timer callback is pending,
// avoid a call to the VM after the driver that will service it is gone.
//
            VTD_End_Min_Int_Period(pVMHwInfo->ddTimerTickRate);
         }

         ddSafeToSendTimerTicks=0;
//
// Need to remove the pointers this driver passed when it loaded.
//
         _asm pushf
         _asm cli
         _asm cld

#ifdef DEBUG
         DBG_MISC((sizeof(_VpenD_Register)/sizeof(DWORD)),
                      "Number of DWORDS to NULL out");
#endif
         // NULL out ActiveDevice structure
         pDword=(PDWORD)&ActiveDevice->Device_VM_Handle;
         for(i=0;i<(sizeof(_VpenD_Register)/sizeof(DWORD));i++)
            *pDword++=0;

         _asm sti
         _asm popf

         ddInstalled_Devices--;

         dwRet=(DWORD)DRV_SUCCESS;
      }
      else
         DBG_WARNING(0,"OEM failed to disable device!!");
   }
   else
      DBG_ERROR(ddInstalled_Devices,"No devices to disable!");

   return dwRet;
}

// End-Of-File
