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

/* APIPROC.c: 

   This file handles the message processing for the virtual pen driver.<nl>

   Routines contained in module:<nl>
      <l cVpenD_API_Proc.cVpenD_API_Proc><nl>
*/

#include "defines.h"
#include <basedef.h>
#include <vxdwraps.h>
#ifdef DEBUG
#include <debug.h>
#endif
#include <VMM.H>
#include <vmmreg.h>
#include <vpicd.h>
#include "pendrv.h"
#include "protos.h"

//apiproc.c
extern char pExpectNullP2[];
extern char pExpectNullP1[];
extern DWORD ddSequentialNum;
extern DWORD ddPreviousTime;
extern DWORD ddProfile_start;
extern DWORD ddProfile_diff;
extern DWORD ddInstalled_Devices;
extern DWORD ddSafeToSendTimerTicks;
extern DWORD ddCurrentTime;

extern PDWORD   pdwPlayDonePtr;
extern DWORD   dwInPlayBackMode;
extern LPDRV_PENPACKET pPenPacketArray20;
extern PPENPACKET pPenPacketArray10;
extern DWORD   dwPenPacketArraySize;
extern DWORD   dwPlayFlags;

extern DWORD ddCurrently_in_an_int;

extern P_HARDWAREINFO pVMHwInfo;
extern LPDRV_PENINFO pVMPenInfo;

extern PCOMMANDS gpCommandStrs;
extern PRATETABLE gpRateTable;

extern void VpenD_Hw_Int(void);
extern void VpenD_EOI(void);

// Note this data type changed.
P_VpenD_Register ActiveDevice = 0;
P_VpenD_Register NullDevice = 0;

#pragma VxD_LOCKED_DATA_SEG

DWORD ddCurrently_in_an_int = 0;
DWORD ddPDKLast=PDK_OUTOFRANGE;
DWORD   dwPlaying = FALSE;

// used in asm files
DWORD ddPM_API_ebp = 0;
DWORD ddMiscFlags = 0;


//*****************************************************************************
//               S E R V I C E S
//*****************************************************************************
// NOTE: removed WIN31COMPAT, apiproc.c is pageable.
//#ifdef WIN31COMPAT
//#pragma VxD_LOCKED_CODE_SEG
//#else
#pragma VxD_PAGEABLE_CODE_SEG
//#endif

DWORD cVpenD_API_Proc(DWORD dwMsg, DWORD lParam1, DWORD lParam2)
   {
   // Assume error...
   DWORD dwRet=DRV_FAILURE;

   DBG_RECORDMESSAGE(dwMsg);
   DBG_TRACE("Entering");

   // Look for successful and error load conditions.
   if(ddMiscFlags!=1)
      {
      DBG_ERROR(ddMiscFlags,"There was a critical error during VpenD load!");
      return (DWORD)ddMiscFlags;
      }

   if(dwMsg&VPEND_OEM)
      return cOEM_API_Proc(dwMsg,lParam1,lParam2);

   // remove bogus flags
   dwMsg&=~(VpenD_lParam1_ptr | VpenD_lParam2_ptr);

   switch(dwMsg)
      {
      // lParam1 - pointer to a VpenD_Init structure
      //
      case VPEND_LOAD:
         {
         PDWORD pDword;
         int i;

         DBG_PWARN1(lParam1);
         DBG_PWARN2(lParam2);
#ifdef DEBUG
         Dbg_MiscPtrs(pVMHwInfo,pVMPenInfo,gpRateTable,
                 gpCommandStrs,ActiveDevice,NullDevice);
#endif

         // All structure elements from ibThis through wPDKLast are NULL.
         pDword=(PDWORD)&ActiveDevice->ibThis;

         for(i=0;i<7;i++)
            *pDword++=0;

         if( cOEM_API_Proc((DWORD)VPEND_GETPENINFO,
                       (DWORD)pVMPenInfo,
                       (DWORD)0) )
            {
            // need to return PENINFO structure
            LPDRV_PENINFO ppiSource=pVMPenInfo;
            LPDRV_PENINFO ppiDest=ActiveDevice->pPenInfo;

            DBG_TRACE("Filling PenInfo structure in register structure");
            *ppiDest=*ppiSource;

            // from now on use pen info structure in VM...
            pVMPenInfo=ppiDest;

            DBG_MISC_HEX((DWORD)pVMPenInfo,"Starting Address of pVMPenInfo");
#ifdef DEBUG
            Dbg_PenInfo(pVMPenInfo); // PENINFO structure
#endif
            ActiveDevice->Device_VM_Handle=Get_Cur_VM_Handle();
            dwRet=DRV_SUCCESS; //ActiveDevice->Device_VM_Handle;

            ddCurrently_in_an_int=0;

            // Increment the number of drivers to service.
            ddInstalled_Devices++;

#ifdef DEBUG
            Dbg_ActiveDevice(ActiveDevice);
#endif
            DBG_MISC_HEX((DWORD)pVMPenInfo,"Leaving Load pVMPenInfo");
            }
         }
         break;

      case VPEND_FREE:
         {
         PDWORD pDword;
         DWORD dwi;

         DBG_PWARN1(lParam1);
         DBG_PWARN2(lParam2);

#ifdef DEBUG
         Dbg_MiscPtrs(pVMHwInfo,pVMPenInfo,gpRateTable,
                 gpCommandStrs,ActiveDevice,NullDevice);
#endif

         if(ddInstalled_Devices == 0 )
            {
            // All the structure elements from ibThis through wPDKLast are NULL.
            pDword=(PDWORD)&ActiveDevice->Device_VM_Handle;
            for(dwi=0;dwi<16;dwi++)
               *pDword++=0;
            }

         DBG_MISC(ddInstalled_Devices,"Number of remaining devices");
         }
         break;

      case VPEND_ENABLE:
         {
         DBG_PWARN1(lParam1);
         DBG_PWARN2(lParam2);

         DBG_MISC_HEX((DWORD)pVMPenInfo,"Entering Enable pVMPenInfo");

         dwRet=cVpenD_API_Enable(lParam1,lParam2);

         DBG_MISC_HEX((DWORD)pVMPenInfo,"Leaving Enable pVMPenInfo");
#ifdef DEBUG
         DBG_RECORDMESSAGE((DWORD)-1);
#endif
         }
         break;

      case VPEND_DISABLE:
         {
         DBG_PWARN1(lParam1);
         DBG_PWARN2(lParam2);

         dwRet=cVpenD_API_Disable(lParam1,lParam2);
         }
         break;

      case VPEND_SETSAMPLINGRATE:
         {
         DBG_PWARN2(lParam2);

         // Update peninfo and return the new sampling rate.
         pVMPenInfo->nSamplingRate=cOEM_API_Proc(dwMsg,lParam1,NULL);
         dwRet=pVMPenInfo->nSamplingRate;
         }
         break;

      //LOWORD(lParam1)==desired sampling dist
      case VPEND_SETSAMPLINGDIST:
         {
         DWORD dwOldDist=pVMPenInfo->nSamplingDist;
         DBG_PWARN2(lParam2);

         pVMPenInfo->nSamplingDist=(DWORD)LOWORD(lParam1);
         dwRet=dwOldDist;
         }
         break;

      // Need to put tablet in speak up mode and set flags so that the pen
      // gets disabled until the user performs a left click.
      case VPEND_PENPLAYSTART:
          {
         if (dwInPlayBackMode)
            {
            dwRet = DRV_FAILURE;
            }
         else
              {
            //
            // The pen driver will send one or the other of these
            // values to set.
            //
            // PLAY_VERSION_10_DATA = 0
            // PLAY_VERSION_20_DATA = 1
            //
            dwPlayFlags = lParam2;

            // record stop timer location
            pdwPlayDonePtr=(PDWORD)lParam1;

            // put tablet in speak up mode at the current rate
            dwRet = cOEM_API_Proc(VPEND_SPEAKUP,0L,0L);

            if (dwRet == 0)
               {
               dwRet = DRV_FAILURE;
               }
            else
               {
               dwRet = DRV_SUCCESS;
               dwInPlayBackMode=TRUE;
               }
              }
          }
          break;

      // Start sending pen packets - replace current stream with pen packet
      // in the given array.
       case VPEND_PENPLAYBACK:
          {
          _asm cli

           if (!dwInPlayBackMode || dwPlaying)
              {
            dwRet = DRV_FAILURE;
              }
           else
              {
            pPenPacketArray20=(LPDRV_PENPACKET)lParam1;
            pPenPacketArray10=(PPENPACKET)lParam1;
            dwPenPacketArraySize=(DWORD)lParam2;

            // start playing them back
            dwPlaying = TRUE;
            dwRet = DRV_SUCCESS;
              }

           _asm sti
          }
          break;

      // turn off play mode
       case VPEND_PENPLAYSTOP:
          {
           //
           // If the virtual driver is not in play back mode, there is nothing to do.
           // If it is in play back mode, synchronously take the virtual driver
           // out of that mode since an interrupt could come in at
           // any time and would fault if it doesn't see these variables
           // all updated synchronously.
           //
           _asm cli

           if (dwInPlayBackMode)
              {
            pPenPacketArray10=0;
            pPenPacketArray20=0;
            dwInPlayBackMode = dwPlaying = FALSE;
            dwRet = DRV_SUCCESS;
              }

           _asm sti
          }
          break;

      // lParam1 is a pointer to a OEM_CALBSTRUCT
      case VPEND_GETCALIBRATION:
         {
         PDRV_CALBSTRUCT pcbSource=(PDRV_CALBSTRUCT)&pVMHwInfo->calibrate;
         PDRV_CALBSTRUCT pcbDest=(PDRV_CALBSTRUCT)lParam1;

         DBG_PWARN2(lParam2);

         // fill pointer with values...
         _asm pushf
         _asm cld
         *pcbDest=*pcbSource;
         _asm popf
         dwRet=DRV_SUCCESS;
         }
         break;

      // lParam1 is a pointer to a OEM_CALBSTRUCT
      case VPEND_SETCALIBRATION:
         {
         PDRV_CALBSTRUCT pDest=(PDRV_CALBSTRUCT)&pVMHwInfo->calibrate;
         PDRV_CALBSTRUCT pcbTmp=(PDRV_CALBSTRUCT)lParam1;

         // fill internal structure...
         _asm pushf
         _asm cld
         *pDest=*pcbTmp;
         _asm popf

         // Record the new coordinates in the peninfo structure and set 
         // the calibration flag.
         _asm cli
         pVMHwInfo->dwHwFlags|=HW_CALIBRATE;
         pVMPenInfo->wDistinctWidth=pVMHwInfo->calibrate.dwDistinctWidth;
         pVMPenInfo->wDistinctHeight=pVMHwInfo->calibrate.dwDistinctHeight;
         _asm sti
         dwRet=DRV_SUCCESS;
         }
         break;

      // lParam1 will point to a _PENINFO structure
      case VPEND_GETPENINFO:
         {
         LPDRV_PENINFO pPI=(LPDRV_PENINFO)lParam1;
         LPDRV_PENINFO ppiSource=pVMPenInfo;

         DBG_PWARN2(lParam2);

         DBG_TRACE("Copying pVMPenInfo into destination pointer");

         // update the PENINFO structure
         _asm pushf
         _asm cld
         *pPI=*ppiSource;
         _asm popf
         dwRet=DRV_SUCCESS;
         }
         break;
      default:
         // ?Invalid Msg?
         DBG_TRACE("--Invalid Message--");
         break;
   }

   DBG_RETURN(dwRet,"Returning");
   DBG_TRACE("Leaving");

   return dwRet;
}

// End-Of-File
