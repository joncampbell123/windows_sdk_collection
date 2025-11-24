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

/* OEMAPI.c: 

   OEM message handling code.

   Routines contained in module:<nl>
      <l cOEM_API_Proc.cOEM_API_Proc><nl>
      <l OEMPause.OEMPause><nl>
      <l OEM_API.OEM_API><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>
#include <VTD.H>

#ifdef DEBUG
#include <DEBUG.h>
#endif

#include "PenDrv.h"
#include "protos.h"
#include "oemvpend.h"
#include "sysutil.h"

// start with extern list
#pragma VxD_LOCKED_DATA_SEG

extern LPDRV_PENINFO pVMPenInfo;
extern P_HARDWAREINFO pVMHwInfo;

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Hardware specific For Wacom Tablets vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

// These values should be read out of registry at bootup.
DWORD ddInductive = 0;
DWORD ddForce = 0;

PRATETABLE gpRateTable = 0;

// The report rate can be set by sending a string like:
// char szReportRateCC[]="\x1B%Wn\r"; where 1<n<125

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// MM series tablets --------------------------------------------------------

#ifdef DEBUG
typedef struct tag_MSGS
   {
   DWORD   dwMsg;
   char    *pStr;
   }
   MSGS, *PMSGS;

MSGS OEMmsgs[] =
   {
   {VPEND_PIINIT,          "OEM Entering VPEND_PIINIT"},
   {VPEND_HWINIT,          "OEM Entering VPEND_HWINIT"},
   {VPEND_COMMANDINIT,     "OEM Entering VPEND_COMMANDINIT"},
   {VPEND_LOAD,            "OEM Entering VPEND_LOAD"},
   {VPEND_FREE,            "OEM Entering VPEND_FREE"},
   {VPEND_ENABLE,          "OEM Entering VPEND_ENABLE"},
   {VPEND_DISABLE,         "OEM Entering VPEND_DISABLE"},
   {VPEND_SETSAMPLINGRATE, "OEM Entering VPEND_SETSAMPLINGRATE"},
   {VPEND_SETSAMPLINGDIST, "OEM Entering VPEND_SETSAMPLINGDIST"},
   {VPEND_SPEAKUP,         "OEM Entering VPEND_SPEAKUP"},
   {VPEND_SHUTUP,          "OEM Entering VPEND_SHUTUP"},
   {VPEND_GETPENINFO,      "OEM Entering VPEND_GETPENINFO"},
   {0L,NULL}
   };

char *gpOEMMsgStr = 0;
#endif // DEBUG

PCOMMANDS gpCommandStrs;

// prototype list
void OEMPause(PDWORD dwPauseTime);

#ifdef WIN31COMPAT
#pragma VxD_LOCKED_CODE_SEG
#else
#pragma VxD_PAGEABLE_CODE_SEG
#endif

//*****************************************************************************
//
//               E X P O R T E D   F U N C T I O N
//
//*****************************************************************************
DWORD cOEM_API_Proc(DWORD dwMsg, DWORD lParam1, DWORD lParam2)
{
   DWORD dwRet=0;

#ifdef DEBUG
   PMSGS pMessages=&OEMmsgs[0];

   dwMsg&=~(VpenD_lParam1_ptr | VpenD_lParam2_ptr | VPEND_OEM);

   while(pMessages->pStr)
   {
      if(pMessages->dwMsg==dwMsg)
      {
         gpOEMMsgStr=pMessages->pStr;
         break;
      }
      else
         pMessages++;
   }
   if(!gpOEMMsgStr)
      gpOEMMsgStr="OEM Entering with Unknown Message";
#endif

   DBG_TRACE(gpOEMMsgStr);

   // remove bogus information
   dwMsg=(dwMsg & (~(VpenD_lParam1_ptr | VpenD_lParam2_ptr)));

   if(dwMsg & VPEND_OEM)
      // call OEM_API routine and get OEM specific work done
      return (OEM_API(dwMsg, lParam1, lParam2));

   // Since dwMsg doesn't contain the Ptrs flags or the OEM flag, it can be
   // used as a switch variable at this point.
   switch(dwMsg)
   {
      case VPEND_SPEAKUP:
         {
            if(pVMHwInfo->dwHwFlags & HW_BIOS)
            {
               // act like bios device
               dwRet=BIOS_SpeakUp();
            }
            else  // act like serial device
            {
#ifdef DEBUG
               if(*(char *)(&gpCommandStrs[API_SPEAKUP].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SPEAKUP].CommandStr,
                           ":API_SPEAKUP command");
#endif
               dwRet=OutTabletString(&gpCommandStrs[API_SPEAKUP].CommandStr);
#ifdef DEBUG
               if( dwRet == 0 )
               {
                  DBG_ERROR(API_SPEAKUP,"VPEND_SPEAKUP Failed");
                  break;
               }
#endif
            }
         }
         break;

      case VPEND_SHUTUP:
         {
            if(pVMHwInfo->dwHwFlags & HW_BIOS)
            {
               // act like bios device
               dwRet=BIOS_ShutUp();
            }
            else  // act like serial device
            {
#ifdef DEBUG
               if(*(char *)(&gpCommandStrs[API_SHUTUP].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SHUTUP].CommandStr,
                           ":API_SHUTUP command");
#endif
               dwRet=OutTabletString(&gpCommandStrs[API_SHUTUP].CommandStr);
#ifdef DEBUG
               if( dwRet == 0 )
               {
                  DBG_ERROR(API_SHUTUP,"VPEND_SPEAKUP Failed");
                  break;
               }
#endif
            }
         }
         break;

      case VPEND_ENABLE:
         {
            if(pVMHwInfo->dwHwFlags & HW_BIOS)
            {
               // act like bios device
               dwRet=BIOS_Enable();
            }
            else  // act like serial device
            {
            DBG_TRACE("OEM Initializing baud");
            if(!StartToSetUpCom())
            {
               DBG_ERROR(0,"Failed to initialize baud!");
               dwRet=0;
               break;
            }

            if(!ContinueToSetUpCom())
            {
               DBG_ERROR(0,"Failed to Communications!");
               dwRet=0;
               break;
            }

            DBG_STRING((DWORD)&gpCommandStrs[API_RESET].CommandStr,
                        ":API_RESET command");
            if( OutTabletString(&gpCommandStrs[API_RESET].CommandStr))
            {
               int i;


               OEMPause((PDWORD)&(pVMHwInfo->ddPauseTime));

#ifdef DEBUG
               if(*(char *)(&gpCommandStrs[API_BITFORMAT].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_BITFORMAT].CommandStr,
                           ":API_BITFORMAT command");
               if(*(char *)(&gpCommandStrs[API_STREAM].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_STREAM].CommandStr,
                           ":API_STREAM command");
               if(*(char *)(&gpCommandStrs[API_EVENOOR].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_EVENOOR].CommandStr,
                           ":API_EVENOOR command");
               if(*(char *)(&gpCommandStrs[API_9600].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_9600].CommandStr,
                           ":API_9600 command");
               if(*(char *)(&gpCommandStrs[API_19200].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_19200].CommandStr,
                           ":API_19200 command");
               if(*(char *)(&gpCommandStrs[API_MEASUREMENT].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_MEASUREMENT].CommandStr,
                           ":API_MEASUREMENT command");
               if(*(char *)(&gpCommandStrs[API_FORMAT].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_FORMAT].CommandStr,
                           ":API_FORMAT command");
               if(*(char *)(&gpCommandStrs[API_RESOLUTION].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_RESOLUTION].CommandStr,
                           ":API_RESOLUTION command");
               if(*(char *)(&gpCommandStrs[API_PORTRAIT].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_PORTRAIT].CommandStr,
                           ":API_PORTRAIT command");
               if(*(char *)(&gpCommandStrs[API_REL_ABS].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_REL_ABS].CommandStr,
                           ":API_REL_ABS command");
#endif
               for(i=API_BITFORMAT;i<=API_REL_ABS;i++)
               {
                  if(OutTabletString(&gpCommandStrs[i].CommandStr)==0)
                  {
                     DBG_ERROR(0,"Failed between API_BITFORMAT and API_REL_ABS!");
                     dwRet=0;
                     break;
                  }
               }
               if(pVMPenInfo->fuOEM & PHW_PRESSURE)
               {
#ifdef DEBUG
                  if(*(char *)(&gpCommandStrs[API_PRESSUREON].CommandStr) != NULL)
                     DBG_STRING((DWORD)&gpCommandStrs[API_PRESSUREON].CommandStr,
                              ":API_PRESSUREON command");
                  if(*(char *)(&gpCommandStrs[API_PRESSURELEVEL].CommandStr) != NULL)
                     DBG_STRING((DWORD)&gpCommandStrs[API_PRESSURELEVEL].CommandStr,
                              ":API_PRESSURELEVEL command");
#endif
                  dwRet=OutTabletString(&gpCommandStrs[API_PRESSUREON].CommandStr);
                  dwRet=OutTabletString(&gpCommandStrs[API_PRESSURELEVEL].CommandStr);
               }
               if( (pVMPenInfo->fuOEM & PHW_ANGLEXY) ||
                  (pVMPenInfo->fuOEM & PHW_ANGLEZ) )
               {
#ifdef DEBUG
                  if(*(char *)(&gpCommandStrs[API_TILT].CommandStr) != NULL)
                     DBG_STRING((DWORD)&gpCommandStrs[API_TILT].CommandStr,
                              ":API_TILT command");
#endif
                  dwRet=OutTabletString(&gpCommandStrs[API_TILT].CommandStr);
               }
               if(pVMPenInfo->fuOEM & PHW_HEIGHT)
               {
#ifdef DEBUG
                  if(*(char *)(&gpCommandStrs[API_TILT].CommandStr) != NULL)
                     DBG_STRING((DWORD)&gpCommandStrs[API_EXTRADATA].CommandStr,
                              ":API_EXTRADATA command");
#endif
                  dwRet=OutTabletString(&gpCommandStrs[API_EXTRADATA].CommandStr);
               }

#ifdef DEBUG
               if(*(char *)(&gpCommandStrs[API_DATAFORMAT].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_DATAFORMAT].CommandStr,
                           ":API_DATAFORMAT command");
               if(*(char *)(&gpCommandStrs[API_TRANSMITON].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_TRANSMITON].CommandStr,
                           ":API_TRANSMITON command");
               if(*(char *)(&gpCommandStrs[API_SPACE1].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SPACE1].CommandStr,
                           ":API_SPACE1 command");
               if(*(char *)(&gpCommandStrs[API_SPACE2].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SPACE2].CommandStr,
                           ":API_SPACE2 command");
               if(*(char *)(&gpCommandStrs[API_SPACE3].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SPACE3].CommandStr,
                           ":API_SPACE3 command");
               if(*(char *)(&gpCommandStrs[API_SETUP_LAST].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_SETUP_LAST].CommandStr,
                           ":API_SETUP_LAST command");
#endif
               for(i=API_DATAFORMAT;i<=API_SETUP_LAST;i++)
               {
                  if(OutTabletString(&gpCommandStrs[i].CommandStr)==0)
                  {
                     DBG_ERROR(0,"Failed to Communications!");
                     dwRet=0;
                     break;
                  }
               }

#ifdef DEBUG
               if(*(char *)(&gpCommandStrs[API_START].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpCommandStrs[API_START].CommandStr,
                           ":API_START command");
#endif
               if(OutTabletString(&gpCommandStrs[API_START].CommandStr)==0)
               {
                  DBG_ERROR(0,"Failed to Communications!");
                  dwRet=0;
                  break;
               }
               dwRet=TRUE;

            }
#ifdef DEBUG
            else
            {
               DBG_ERROR(0,"API_RESET Failed!");
               dwRet=0;
               break;
            }
#endif
            }
         }
         break;

      // Turn off interrupts.
      case VPEND_DISABLE:
         {
            if(pVMHwInfo->dwHwFlags & HW_BIOS)
            {
               // act like bios device
               dwRet=BIOS_Disable();
            }
            else  // act like serial device
            {
#ifdef DEBUG
            if(*(char *)(&gpCommandStrs[API_SHUTDOWN].CommandStr) != NULL)
               DBG_STRING((DWORD)&gpCommandStrs[API_SHUTDOWN].CommandStr,
                        ":API_SHUTDOWN command");
#endif
            if( (dwRet=OutTabletString(
                      &gpCommandStrs[API_SHUTDOWN].CommandStr)) != 0 )
            {
               if(pVMPenInfo->fuOEM & PHW_PRESSURE)
               {
#ifdef DEBUG
                  if(*(char *)(&gpCommandStrs[API_PRESSUREOFF].CommandStr) != NULL)
                     DBG_STRING((DWORD)&gpCommandStrs[API_PRESSUREOFF].CommandStr,
                              ":API_PRESSUREOFF command");
#endif
                  dwRet=OutTabletString(&gpCommandStrs[API_PRESSUREOFF].CommandStr);
               }
            }
#ifdef DEBUG
            if(dwRet==0)
            {
               DBG_ERROR(0,"VPEND_DISABLE failed on Shut Down string!");
               break;
            }
#endif
            // bug on disable
            DBG_TRACE("OEM Cleaning up com port");
            if(!CleanUpCom())
            {
               DBG_ERROR(0,"Failed to Clean up!");
               dwRet=0;
               break;
            }
            }
         }
         break;

      // lParam1 - desired report rate
      // lParam2 - pointer to OEM_PENINFO structure
      // Returns new rate and it will be a positive number within the
      // bounds of realism:  say Min==20, Max==200 (or something).
      case VPEND_SETSAMPLINGRATE:
         {
         if(pVMHwInfo->dwHwFlags & HW_BIOS)
            {
            // act like bios device
            dwRet = BIOS_SetSamplingRate(lParam1, lParam2);
            }
         else  // act like serial device
            {
            BOOL          bRateSet=FALSE;
            PRATETABLE    pRT = &gpRateTable[0];   // start with minimum value.

            DBG_MISC(lParam1,": rate to look for...");

            // Walk through the rate table and send appropriate string.
            while(pRT->CommandStr && (pRT->dwRate > 0))
               {
               if (lParam1 <= pRT->dwRate)
                  {
                  bRateSet = TRUE;
#ifdef DEBUG
                  if(*(char *)(&pRT->CommandStr) != NULL)
                     DBG_STRING((DWORD)&pRT->CommandStr,":API_RATE command");
#endif
                  OutTabletString(&pRT->CommandStr);
                  dwRet = pRT->dwRate;
                  break;
                  }
               pRT++;
               }

            if (!bRateSet)
               {
               // set it to the maximum possible value
               pRT--;

#ifdef DEBUG
               if(*(char *)(&gpRateTable[0].CommandStr) != NULL)
                  DBG_STRING((DWORD)&gpRateTable[0].CommandStr,
                           ":API_RATE command");
#endif
               OutTabletString(&pRT->CommandStr);
               DBG_MISC(pRT->dwRate,"pRT->dwRate");
               dwRet=pRT->dwRate;
               }
            }
         DBG_MISC(dwRet,"New Report Rate");
         }
         break;

      // This OEM driver does not need to touch the PENINFO structure or
      // set the sampling distance.
      case VPEND_SETSAMPLINGDIST:
         DBG_MISC(lParam1,"== Distance, OEM returning success");
         dwRet=lParam1;
         break;

      case VPEND_LOAD:
      case VPEND_FREE:
      case VPEND_GETPENINFO:
         DBG_TRACE("OEM returning success");
         dwRet=1;
         break;

      default:
         break;
   }
   DBG_TRACE("OEM Leaving: cOEM_API_Proc");
   return dwRet;
}

//*****************************************************************************
//
//            M E S S A G E   S U P P O R T   F U N C T I O N S
//
//*****************************************************************************
DWORD OEM_API(DWORD dwMsg, DWORD lParam1, DWORD lParam2)
{
   return (DWORD)0;
}
   
// Use VxDWraps for VTD wrapper.

void OEMPause(PDWORD pdwPauseTime)
{
   DWORD dwTime=_VTD_Get_Real_Time();

#ifdef DEBUG
   DBG_MISC(*pdwPauseTime,"Pause time");
#endif

   while( (_VTD_Get_Real_Time()-dwTime) < *pdwPauseTime);
}

//End-Of-File
