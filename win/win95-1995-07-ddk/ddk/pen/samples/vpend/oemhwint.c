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

/* OEMHWINT.c: 

   OEM hardware interrupt servicing code.<nl>

   Routines contained in module:<nl>
      <l cOEM_Hw_Int.cOEM_Hw_Int><nl>
      <l Wacom_Build_Pen_Packet.Wacom_Build_Pen_Packet><nl>
      <l Wacom_Build_Pen_Packet_UD.Wacom_Build_Pen_Packet_UD><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>
#ifdef DEBUG
#include <debug.h>
#endif

#include "PenDrv.h"
#include <oemvpend.h>
#include "protos.h"
#include "ins8250.h"

#pragma VxD_LOCKED_DATA_SEG

// imported from global data area
extern LPDRV_PENINFO pVMPenInfo;
extern P_HARDWAREINFO pVMHwInfo;

// imported from OEM data area
extern DWORD ddInductive;
extern DWORD ddForce;

#define SYNC_BIT 0x80

// Wacom-specific values
#define WACOM_IN_RANGE    0x40

#define WBYTE_STATUS      0
#define WBYTE_X_HIGH      0
#define WBYTE_X_MID       1
#define WBYTE_X_LOW       2
#define WBYTE_Y_HIGH      3
#define WBYTE_Y_MID       4
#define WBYTE_Y_LOW       5
#define WBYTE_BTNS        6
#define WBYTE_BTNS_UD     3
#define WBYTE_PRESSURE    6
#define WACOM_PACKET_SIZE 7


DWORD ibByteBuffer = 0;
BYTE rgbByteBuffer[WACOM_PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0};

#ifdef DEBUG_foo
// break macro
#define BREAK_X_TIME(uVariable,uIteration)    if(uVariable++>=uIteration)\
_asm int 3

UINT uOverRun=0;
UINT uExpectingSync=0;
UINT uIncomplete=0;
UINT uComBase=0;
UINT uDataReady=0;
UINT uSyncBit=0;
#else
#define BREAK_X_TIME(uVariable,uIteration)
#endif

#pragma VxD_LOCKED_CODE_SEG


//*****************************************************************************
//;      H A R D W A R E    I N T E R R U P T    R O U T I N E S
//*****************************************************************************
DWORD cOEM_Hw_Int(LPDRV_PENPACKET lppp)
{
   DWORD ddcom_base=pVMHwInfo->com_base;
   DWORD ddPenPacketSize=pVMHwInfo->dwHardwarePacketSize;
   BYTE bInputByte,bStatusByte;

#ifdef BIOS
   // Do we have a serial device? Bios device?
   if(pVMHwInfo->dwHwFlags & HW_BIOS)
   {
      return BIOS_ServiceInterrupt(lppp);
   }
#endif

   // This code requires a com base number!
   if(!ddcom_base)
   {
      return (FF_COM_BASE | FF_ERROR);
   }

   _asm   mov   edx,ddcom_base      ; see if there is a character available
   _asm   add   dx,ACE_LSR
   _asm   in   al,dx                ; get Line Status Register
   _asm   mov bStatusByte,al        ; save byte for testing

   if(bStatusByte & ACE_DR)
   {   // Data is ready to read, so get it.
      _asm   mov   ah,al            ; yes, get the character
      _asm   mov   edx,ddcom_base
      _asm   in   al,dx
      _asm   mov bInputByte,al      ; save the byte
   }
   else // data not ready
   {
      BREAK_X_TIME(uDataReady,20);
      return (FF_DATANOTREADY | FF_ERROR);
   }

   if(bStatusByte & ACE_OR)
   {
      BREAK_X_TIME(uOverRun,20);
      ibByteBuffer=0;
      return (FF_OVERRUN | FF_ERROR);
   }

   // get a sync byte?
   if(bInputByte & SYNC_BIT)
   {
      // yes, this one's a sync byte
      if( ibByteBuffer!=0 )
      {
         // Received a sync byte somewhere in the middle of the
         // pen packet.   This is bad, but recoverable!
         BREAK_X_TIME(uSyncBit,30);
         ibByteBuffer=0;
      }
   }
   else // This is not a sync byte.  Was one expected?
   {
      if(ibByteBuffer==0)
      {   // If expecting one, ignore this byte and leave.
         BREAK_X_TIME(uExpectingSync,30);
         return (FF_EXPECTING_SYNC | FF_ERROR);
      }
   }

   // Got what appears to be valid info.
   // Save the byte and increment number of bytes.
   rgbByteBuffer[ibByteBuffer++]=bInputByte;

   if(ibByteBuffer!=ddPenPacketSize)
   {
      BREAK_X_TIME(uIncomplete,20);
      return (FF_INCOMPLETE);
   }

   // Have gathered the appropriate number of bytes.  :)

   // Expect these routines to return FF_SUCCESS | FF_PENPACKET or
   // FF_SUCCESS | FF_OORPENPACKET.

   ibByteBuffer=0;               // initialize for next round
   switch(pVMHwInfo->ddHardwareType)
   {
#ifdef WACOM2
      case TYPE_WACOMHD648A:
      case TYPE_WACOMSD510C:
         return (Wacom_Build_Pen_Packet(lppp));
         break;
#endif

#ifdef WACOM4
      case TYPE_WACOMUD0608R:
         return (Wacom_Build_Pen_Packet_UD(lppp));
         break;
#endif

      default:
         break;
   }
}


#ifdef WACOM2
DWORD Wacom_Build_Pen_Packet(LPDRV_PENPACKET lppp)
{
   // Do we have a valid penpacket?
   if(rgbByteBuffer[WBYTE_STATUS] & WACOM_IN_RANGE)
   {
      WORD wTmp;
      _asm {
         ; mov edi,lppp   ; place the pointer in edi
Wacom_in_range:
         ; compute X coordinate
         mov   dh,rgbByteBuffer + WBYTE_X_MID   ;D6:D0 = X13:X7
         mov   dl,rgbByteBuffer + WBYTE_X_LOW   ;D6:D0 = X6:X0
         shl   dl,1
         shr   dx,1            ;D13:D0 = X13:X0
         mov   cl,rgbByteBuffer + WBYTE_X_HIGH   ;D2:D0 = X16:X14
         shl   cl,6
         or   dh,cl            ;DX = tablet X coord
         mov wTmp,dx
         }
      lppp->wTabletX=wTmp;

      _asm {
         ; compute Y coordinate
         mov   dh,rgbByteBuffer + WBYTE_Y_MID   ;D6:D0 = Y13:Y7
         mov   dl,rgbByteBuffer + WBYTE_Y_LOW   ;D6:D0 = Y6:Y0
         shl   dl,1
         shr   dx,1            ;D13:D0 = Y13:Y0
         mov   cl,rgbByteBuffer + WBYTE_Y_HIGH   ;D2:D0 = Y16:Y14
         shl   cl,6
         or   dh,cl            ;DX = tablet Y coord
         mov wTmp,dx
         }
      lppp->wTabletY=wTmp;

      if(!(pVMPenInfo->fuOEM & PHW_PRESSURE))
      {
         // Put the pen tip and button information into wPDK.
         lppp->wPDK=(rgbByteBuffer[WBYTE_BTNS] & 0x03);
      }
      else // do pressure info
      {
      _asm {
         mov   al,rgbByteBuffer + WBYTE_PRESSURE   ;D6:D0 = pressure
         and   al,01111111b            ;keep only D6:D0
         shl   al,1               ;D7:D1
         sar   al,1               ;D7:D0 sign extended
         cbw                  ;D15:D0 sign extended
         mov wTmp,ax
         }

         lppp->rgwOemData[0]=wTmp;

         if(ddInductive)
            lppp->rgwOemData[0]=~(lppp->rgwOemData[0]);

         // penwin wants range from 0 up...
         lppp->rgwOemData[0]+=35;

         if(lppp->rgwOemData[0] >= ddForce)
            lppp->wPDK=1;
         else
            lppp->wPDK=0;
      }

      if(lppp->wTabletY & 0xE000)
      {
         DBG_MISC(lppp->wTabletY,"Y out of range, bogus pen packet");
         return (DWORD)(FF_ERROR | FF_RECORD_ERROR);
      }
      if(lppp->wTabletX & 0xE000)
      {
         DBG_MISC(lppp->wTabletX,"X out of range, bogus pen packet");
         return (DWORD)(FF_ERROR | FF_RECORD_ERROR);
      }

      if(lppp->wPDK & ~(0x0007) )
      {
#ifdef DEBUG
         dbg_penpacket(lppp);
#endif
         return (DWORD)(FF_ERROR | FF_RECORD_ERROR);
      }

      return (DWORD)(FF_SUCCESS | FF_PENPACKET);
   }
   else
   {
      // packet is out of range
      lppp->wPDK=PDK_OUTOFRANGE;
      lppp->wTabletX=0;
      lppp->wTabletY=0;
      return (DWORD)(FF_SUCCESS | FF_OORPENPACKET);
   }
}
#endif //WACOM2


#ifdef WACOM4
DWORD Wacom_Build_Pen_Packet_UD(LPDRV_PENPACKET lppp)
   {
   // Is this a valid penpacket?
   if(rgbByteBuffer[WBYTE_STATUS] & WACOM_IN_RANGE)
      {
      WORD wTmp;
      _asm
         {
         ; mov edi,lppp   ; place the pointer in edi

Wacom_in_range_UD:
         ; compute X coordinate
         mov   dh,rgbByteBuffer + WBYTE_X_MID   ;D6:D0 = X13:X7
         mov   dl,rgbByteBuffer + WBYTE_X_LOW   ;D6:D0 = X6:X0
         shl   dl,1
         shr   dx,1            ;D13:D0 = X13:X0
         mov   cl,rgbByteBuffer + WBYTE_X_HIGH   ;D2:D0 = X16:X14
         shl   cl,6
         or   dh,cl            ;DX = tablet X coord
         mov wTmp,dx
         }
      lppp->wTabletX=wTmp;

      _asm
         {
         ; compute Y coordinate
         mov   dh,rgbByteBuffer + WBYTE_Y_MID   ;D6:D0 = Y13:Y7
         mov   dl,rgbByteBuffer + WBYTE_Y_LOW   ;D6:D0 = Y6:Y0
         shl   dl,1
         shr   dx,1            ;D13:D0 = Y13:Y0
         mov   cl,rgbByteBuffer + WBYTE_Y_HIGH   ;D2:D0 = Y16:Y14
         shl   cl,6
         or   dh,cl            ;DX = tablet Y coord
         mov wTmp,dx
         }
      lppp->wTabletY=wTmp;


      lppp->wPDK=((rgbByteBuffer[WBYTE_BTNS_UD]>>3) & 0x03);

      if(pVMPenInfo->fuOEM & PHW_PRESSURE)
         {  // user wants pressure information
         _asm
            {
            mov   al,rgbByteBuffer + WBYTE_PRESSURE   ;D6:D0 = pressure
            and   al,01111111b                        ;keep only D6:D0
            shl   al,1                                ;D7:D1
            sar   al,1                                ;D7:D0 sign extended
            cbw                                       ;D15:D0 sign extended
            mov wTmp,ax
            }

         lppp->rgwOemData[0]=wTmp;

         // penwin wants range from 0 up...
         lppp->rgwOemData[0]+=60;
         }
      return (DWORD)(FF_SUCCESS | FF_PENPACKET);
      }
   else
      {
      // packet is out-of-range
      lppp->wPDK=PDK_OUTOFRANGE;
      lppp->wTabletX=0;
      lppp->wTabletY=0;
      return (DWORD)(FF_SUCCESS | FF_OORPENPACKET);
      }
   }
#endif //WACOM4

//End-Of-File
