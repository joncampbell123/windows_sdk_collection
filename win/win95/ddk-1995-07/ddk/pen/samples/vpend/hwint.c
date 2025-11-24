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

/* HWINT.c: 

   This file contains the hardware interrupt time processing code.<nl>

   Routines contained in module:<nl>
      <l cVpenD_Hw_Int.cVpenD_Hw_Int><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>
#include <vpicd.h>
#include <vtd.h>
#ifdef DEBUG
#include <Debug.h>
#endif
#include "PenDrv.h"
#include "protos.h"
#include "sysutil.h"

#pragma VxD_LOCKED_DATA_SEG

DWORD ddInstalled_Devices = 0;

extern P_VpenD_Register ActiveDevice;
extern P_VpenD_Register NullDevice;
P_HARDWAREINFO pVMHwInfo = 0;
LPDRV_PENINFO pVMPenInfo = 0;

#ifdef DEBUG
DWORD ddProfile_diff = 0;
DWORD ddProfile_start = 0;
DWORD ddSequentialNum = 0;
DWORD ddTest=0;
BOOL bDisplayed=FALSE;
#endif

DWORD ddCurrentTime = 0;
DWORD ddSafeToSendTimerTicks = 0;

extern void VpenD_Call_Installable_Driver(void);

extern DWORD ddCurrently_in_an_int;
PDWORD  pdwPlayDonePtr = 0;
DWORD   dwInPlayBackMode=FALSE;
LPDRV_PENPACKET pPenPacketArray20 = 0;
PPENPACKET pPenPacketArray10 = 0;
DWORD   dwPenPacketArraySize = 0;
DWORD   dwPlayFlags = 0;

// This variable is used if the caller is checking sampling distance.
DRV_PENPACKET LastPP = {0, 0, 0,
         {0, 0, 0, 0, 0, 0},
         0,
#ifdef DEBUG
         0, 0, 0
#endif
         };

DWORD ddOEM_Data = 0;
DWORD ddDebugErrors = 0;

//;******************************************************************************
//;      H A R D W A R E    I N T E R R U P T    R O U T I N E S
//;******************************************************************************

#pragma VxD_LOCKED_CODE_SEG

DWORD cVpenD_Hw_Int(void)
{
   LPDRV_PENPACKET   pOEM_PP;
   DWORD      dwRet=0;

   if(ActiveDevice->pPenPacketBuffer==NULL)
   {
      DBG_ERROR(ActiveDevice->pPenPacketBuffer,"No Pen Packet Buffer Yet!");
      // There is no Pen Driver to service interrupts yet.
      // Send EOI and leave.
      VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
      return 1;
   }

   pOEM_PP=(LPDRV_PENPACKET)&ActiveDevice->pPenPacketBuffer[ActiveDevice->ibNext];

// Call OEM to service interrupt.
//
//   FF_SUCCESS -   will be set if there is information that needs to be sent to
//                  the interrupt handlers.
//      FF_PENPACKET - pen packet to service
//      FF_OORPENPACKET - out-of-range pen packet
//      FF_OEMSPECIFIC - OEM-specific information to service
//
//   FF_INCOMPLETE - The OEM has not completed gathering the information.
//
//   FF_ERROR - if an error occured
//      FF_UNKNOWN - not used
//      FF_COM_BASE - Com_Base problem
//      FF_OVERRUN - overrun
//      FF_RECORD_ERROR - some record error
//      FF_EXPECTING_SYNC - sync out of order

   dwRet=cOEM_Hw_Int(pOEM_PP);

   if(dwRet & FF_INCOMPLETE)
   {
      // Haven't completely gathered data yet and no errors.
      // Send EOI and leave.
      VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
      return 1;
   }

   if(dwRet & FF_ERROR)
   {
      // ddDebugErrors contains three different counts at this time:
      // LOBYTE(LOWORD(ddDebugErrors))==FF_OVERRUN
      // HIBYTE(LOWORD(ddDebugErrors))==FF_RECORD_ERROR
      // LOBYTE(HIWORD(ddDebugErrors))==FF_EXPECTING_SYNC
      if(dwRet & FF_OVERRUN)
      {
         _asm mov eax,ddDebugErrors
         _asm inc al
         _asm mov ddDebugErrors,eax
         VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
         return 1;
      }
      if(dwRet & FF_RECORD_ERROR)
      {
         _asm mov eax,ddDebugErrors
         _asm inc ah
         _asm mov ddDebugErrors,eax
         VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
         return 1;
      }
      if(dwRet & FF_EXPECTING_SYNC)
      {
         _asm mov eax,ddDebugErrors
         _asm mov cx,ax
         _asm shr eax,16
         _asm inc al
         _asm shl eax,16
         _asm mov ax,cx
         _asm mov ddDebugErrors,eax
         VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
         return 1;
      }
   }

   // Received either OEM data, an in-range pen
   // packet, or an out-of-range pen packet.
   if(dwRet & FF_SUCCESS )
   {
      if(dwRet & FF_OEMSPECIFIC)
      {
         // done servicing this interrupt
         VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);

         // Need to make sure that the information gets through!
         // If not currently servicing an interrupt, set up a
         // callback and service the OEM data.  Else wait for the next
         // timer tick.
         ddOEM_Data=1;

         if(!ddCurrently_in_an_int)
         {
            ddCurrently_in_an_int=1;
            _Call_Priority_VM_Event(HIGH_PRI_DEVICE_BOOST, //TIME_CRITICAL_BOOST,
                              PEF_WAIT_FOR_STI,
                              DT_OEM,
                              (DWORD)&VpenD_Call_Installable_Driver);
         }
         // else wait for a timer tick...
         return 1;
      }

      if(dwInPlayBackMode)
      {
      // Check and see if user terminated playback by touching the pen
      // on the tablet.

         if(pOEM_PP->wPDK==PDK_DOWN)
         {
             dwInPlayBackMode=FALSE;
             pPenPacketArray10=(PPENPACKET)NULL;
             pPenPacketArray20=(LPDRV_PENPACKET)NULL;
             *(PDWORD)pdwPlayDonePtr=_VTD_Get_Real_Time();
         }
         else  // currently in playback mode
         {
             if(dwPenPacketArraySize)
             {
                 int i;

                 switch(dwPlayFlags)
                 {
                 case PLAY_VERSION_20_DATA:
                     {
                     pOEM_PP->wTabletX=pPenPacketArray20->wTabletX;
                     pOEM_PP->wTabletY=pPenPacketArray20->wTabletY;
                     pOEM_PP->wPDK=pPenPacketArray20->wPDK;

                     // copy OEM data
                     for(i=0;i<MAXOEMDATAWORDS;i++)
                             pOEM_PP->rgwOemData[i]=pPenPacketArray20->rgwOemData[i];

                     pOEM_PP->ddTimeStamp=pPenPacketArray20->ddTimeStamp;
#ifdef DEBUG
                     pOEM_PP->ddSequential=pPenPacketArray20->ddSequential;
                     pOEM_PP->ddProfile=pPenPacketArray20->ddProfile;
                     pOEM_PP->ddDebug=pPenPacketArray20->ddDebug;
#endif
                     pPenPacketArray20++;
                     }
                     break;

                 default: // PLAY_VERSION_10_DATA
                     {
                     pOEM_PP->wTabletX=pPenPacketArray10->wTabletX;
                     pOEM_PP->wTabletY=pPenPacketArray10->wTabletY;
                     pOEM_PP->wPDK=pPenPacketArray10->wPDK;

                     // copy OEM data
                     for(i=0;i<MAXOEMDATAWORDS;i++)
                             pOEM_PP->rgwOemData[i]=pPenPacketArray10->rgwOemData[i];
                     // no time stamp here...
                     pPenPacketArray10++;
                     }
                     break;
                 }
                 dwPenPacketArraySize--;

                 // if done, tell the calling client
                 if(dwPenPacketArraySize==0)
                 {
                     *(PDWORD)pdwPlayDonePtr=_VTD_Get_Real_Time();
                 }
             }
             else
             {
             // Received an interrupt during playback mode and there
             // was no replacement packet.  Just return.

             VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
             return 1;
             }
         }
      }

      // If in playback mode, no need to perform any time stamping
      // calibration, orientation, or debug info.   Just send the packet
      // through.  So skip all this...
      if(!dwInPlayBackMode)
      {
         // Has the sampling distance been set?  If so check to see if
         // point should be sent or not.
         if(pVMPenInfo->nSamplingDist)
         {
            int i;
            DWORD dwDeltaDistance=pVMPenInfo->nSamplingDist;

            // If the state of the pen has changed, send it through.
            // Need to ask if pen has moved enough from the last packet to
            // generate a new event.  Need to check dwDeltaDistance.  If
            // either X or Y has changed by dwDeltaDistance or more, generate
            // a new event.

            // First check PDK flags, did they change?   If so, generate event.
            if( LastPP.wPDK != pOEM_PP->wPDK)
               goto HW_Gen_New_Event;

            // Next check to see if the OEM data has changed. If so,
            // generate new event!
            for (i=0;i<MAXOEMDATAWORDS;i++)
            {
               if( LastPP.rgwOemData[i] != pOEM_PP->rgwOemData[i++] )
                  goto HW_Gen_New_Event;
            }

            // now for X
            _asm {
               ; movzx eax,pOEM_PP->wTabletX
               mov edi,pOEM_PP
               movzx eax,WORD PTR [edi]
               movzx ecx,LastPP.wTabletX
               cmp eax,ecx
               jge HW_Skip_X
               xchg eax,ecx
HW_Skip_X:      sub eax,ecx  ; this generates absolute value (positive)
               cmp eax,dwDeltaDistance
               jg HW_Gen_New_Event
               }

            // now for Y
            _asm {
               ; movzx eax,pOEM_PP->wTabletX
               mov edi,pOEM_PP
               movzx eax,WORD PTR [edi+2]
               movzx ecx,LastPP.wTabletY
               cmp eax,ecx
               jge HW_Skip_Y
               xchg eax,ecx
HW_Skip_Y:      sub eax,ecx  ; this generates absolute value (positive)
               cmp eax,dwDeltaDistance
               jg HW_Gen_New_Event
               }

            // If code has executed to this point, pen hasn't traveled far enough
            // to generate an event!
            VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);
            return 1;
         }
HW_Gen_New_Event:
         // Save the current pen packet for next time through.
         _asm pushf
         _asm cld
         LastPP=*pOEM_PP;
         _asm popf
         if( (ddCurrentTime=_VTD_Get_Real_Time()) == 0 )
         {
            DBG_TRACE("HWInt.c: TimeStamp==0");
#ifdef DEBUG
            // This breakpoint should never be hit!!
            // If so, virtual driver memory is being overwritten.
            _asm int 3
#endif //DEBUG
         }
         pOEM_PP->ddTimeStamp=ddCurrentTime;

#ifdef DEBUG
         //dbg_penpacket(pOEM_PP);
#endif

         if(dwRet & FF_PENPACKET)
         {
            long x,y;

            // orientation...
            // is tablet in normal orientation?
            if(pVMHwInfo->ddOrientation!=0)   // need to do something
            {
               if(pVMHwInfo->ddOrientation==1)
               {
                  WORD wTmp;

                  wTmp=pOEM_PP->wTabletX;  // save X

                  // the Y value gets put in X
                  pOEM_PP->wTabletX=pOEM_PP->wTabletY;

                  // negate X
                  pOEM_PP->wTabletY=pVMPenInfo->wDistinctHeight-wTmp;
               }
               else
               {
                  if(pVMHwInfo->ddOrientation==2)
                  {
                     // flip Y
                     pOEM_PP->wTabletY=pVMPenInfo->wDistinctHeight-pOEM_PP->wTabletY;
                
                     // flip X
                     pOEM_PP->wTabletX=pVMPenInfo->wDistinctWidth-pOEM_PP->wTabletX;
                  }
                  else // HwInfo.ddOrientation==3
                  {
                     WORD wTmp;

                     wTmp=pOEM_PP->wTabletY;  // save Y
                     
                     // the X value gets put in Y
                     pOEM_PP->wTabletY=pOEM_PP->wTabletX;

                     // negate X
                     pOEM_PP->wTabletX=pVMPenInfo->wDistinctWidth-wTmp;
                  }
               }
            }

            // Deal with offsetting for buffer zone or normalize the point
            // within range.
            x=(long)(short signed)pOEM_PP->wTabletX;
            y=(long)(short signed)pOEM_PP->wTabletY;

            if(pVMHwInfo->dwHwFlags & HW_CALIBRATE)
            {
               x+=(long)pVMHwInfo->calibrate.dwOffsetX;
               y+=(long)pVMHwInfo->calibrate.dwOffsetY;
            }

            if(pVMHwInfo->dwHwFlags & HW_BUFFERZONE)
            {
               x+=(long)pVMHwInfo->ddBufferZoneX;
               y+=(long)pVMHwInfo->ddBufferZoneY;
            }

            if(x < 0)    x=0;
            if(x >= (long)pVMPenInfo->wDistinctWidth)
               x=(long)(pVMPenInfo->wDistinctWidth-1);
            pOEM_PP->wTabletX=(WORD)x;

            if(y < 0)    y=0;
            if(y >= pVMPenInfo->wDistinctHeight)
               y=(pVMPenInfo->wDistinctHeight-1);
            pOEM_PP->wTabletY=(WORD)y;

            // convert to 1000ths
#ifdef DEBUG
            if( (pVMPenInfo->wDistinctHeight==0)   ||
               (pVMPenInfo->wDistinctWidth==0) )
               DBG_ERROR(0L,"pVMPenInfo->wDistinctWidth/Height==0!!");

            if( ( (int)pOEM_PP->wTabletX < 0 ) ||
               ( (int)pOEM_PP->wTabletX >= pVMPenInfo->wDistinctWidth ) )
            {
               DBG_ERROR(pOEM_PP->wTabletX,"Out of range X coordinate!");
               pOEM_PP->wTabletX=0;
            }

            if( ( (int)pOEM_PP->wTabletY < 0 ) ||
               ( (int)pOEM_PP->wTabletY >= pVMPenInfo->wDistinctHeight ) )
            {
               DBG_ERROR(pOEM_PP->wTabletY,"Out of range Y coordinate!");
               pOEM_PP->wTabletY=0;
            }

#endif
#ifdef DEBUG_FOOBAR
            DBG_MISC_HEX((DWORD)pVMPenInfo,"pVMPenInfo");
            DBG_TRACE("Before mult and div");
            dbg_penpacket(pOEM_PP);
            DBG_MISC(pVMPenInfo->wDistinctWidth,"wDistinctWidth");
            DBG_MISC(pVMPenInfo->wDistinctHeight,"wDistinctHeight");
            DBG_MISC(pVMPenInfo->cxRawWidth,"cxRawWidth");
            DBG_MISC(pVMPenInfo->cyRawHeight,"cyRawHeight");

#endif

            if( (pVMPenInfo->wDistinctWidth!=0)   &&
               (pVMPenInfo->wDistinctHeight!=0) )
            {
               pOEM_PP->wTabletX=((DWORD)pOEM_PP->wTabletX * pVMPenInfo->cxRawWidth)/
                                      pVMPenInfo->wDistinctWidth;
               pOEM_PP->wTabletY=((DWORD)pOEM_PP->wTabletY * pVMPenInfo->cyRawHeight)/
                                      pVMPenInfo->wDistinctHeight;
            }
#ifdef DEBUG_FOOBAR
            DBG_TRACE("After 1000ths");
            dbg_penpacket(pOEM_PP);
#endif
         }

#ifdef DEBUG
         // Now look into debug information.
         if(pVMHwInfo->dwHwFlags & HW_SEQUENTIAL)
         {
            // want sequential information
            ddSequentialNum++;
            pOEM_PP->ddSequential=ddSequentialNum;
         }
         if(pVMHwInfo->dwHwFlags & HW_PROFILE)
         {
            // want profile information
            pOEM_PP->ddProfile=ddProfile_diff;
         }
         if(pVMHwInfo->dwHwFlags & HW_DEBUG)
         {
            // want debug information
            pOEM_PP->ddDebug=ddDebugErrors;
            ddDebugErrors=0;
         }
#endif
      }

#ifdef DEBUG_FOOBAR
      ddTest++;
      if(pVMPenInfo)
      {
         DBG_MISC_HEX((DWORD)pOEM_PP,"pOEM_PP");
         dbg_penpacket(pOEM_PP);
      }

      if( (pVMPenInfo == NULL) && (!bDisplayed) )
      {
         bDisplayed=TRUE;
         DBG_MISC_HEX((DWORD)pOEM_PP,"pOEM_PP");
         dbg_penpacket(pOEM_PP);
      }
#endif

// Now set up for another pen packet.
//
// This code has been changed so that ibNext is not an address but an index.
// Every time a pen packet is recorded, all that is done is to place it in the buffer.
// Only ibNext needs to be used (not ibThis).
//
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

      // done servicing this interrupt
      VPICD_Phys_EOI((HIRQ)pVMHwInfo->VpenD_IRQ_Handle);

      if(pVMHwInfo->dwHwFlags & HW_PUSHMODEL)
      {
//
// This is the code that gets the pen packet serviced if using the push model.
// What the push model means is that every time a complete pen packet is received,
// it is forced into the system.   This is different from the timer model where
// a packet is reported only on a timer tick.
//
#ifdef DEBUG
         //dbg_penpacket(pOEM_PP);
#endif
         if(!ddCurrently_in_an_int)
         {
            ddCurrently_in_an_int=1;
            _Call_Priority_VM_Event(HIGH_PRI_DEVICE_BOOST, //TIME_CRITICAL_BOOST,
                              PEF_WAIT_FOR_STI,
                              (dwRet & FF_PENPACKET ? DT_PENPACKET : DT_OORPENPACKET),
                              (DWORD)&VpenD_Call_Installable_Driver);
         }
      }
      return 1;
   }
   DBG_ERROR(dwRet,"Bad OEM return value!");
   return 1;
}

//End-Of-File
