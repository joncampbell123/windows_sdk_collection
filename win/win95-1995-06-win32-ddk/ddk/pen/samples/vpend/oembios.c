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

/* OEMBIOS.c: 

   Misc OEM hardware code. Update this file to add/delete particular BIOS
   devices.

   Routines contained in module:<nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>

#ifdef DEBUG
#include <DEBUG.h>
#endif

#include "PenDrv.h"
#include "protos.h"
#include "ins8250.h"

extern P_HARDWAREINFO pVMHwInfo;

#pragma VxD_LOCKED_CODE_SEG

// BIOS entry points to talk to BIOS devices
DWORD BIOS_ServiceInterrupt(LPDRV_PENPACKET lppp)
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_ServiceInterrupt(lppp));
#endif
   default:
      break;
   }
}


DWORD BIOS_SpeakUp()
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_SpeakUp());
#endif
   default:
      break;
   }
}


DWORD BIOS_ShutUp()
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_ShutUp());
#endif
   default:
      break;
   }
}


DWORD BIOS_Enable()
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_Enable());
#endif
   default:
      break;
   }
}


DWORD BIOS_Disable()
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_Disable());
#endif
   default:
      break;
   }
}


DWORD BIOS_SetSamplingRate(DWORD lParam1,DWORD lParam2)
{
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef CPQ
   case TYPE_CPQCONCERTO:
      return(Concerto_SetSamplingRate(lParam1,lParam2));
#endif
   default:
      break;
   }
}


//End-Of-File
