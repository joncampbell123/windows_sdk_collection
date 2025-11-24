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

/* CONCERTO.c: 

   Misc OEM hardware code.<nl>

   Routines contained in module:<nl>
*/

#include "defines.h"

#include <basedef.h>
#include <vmm.h>

#ifdef DEBUG
#include <DEBUG.h>
#endif

#include "PenDrv.h"
#include "oemvpend.h"
#include "protos.h"
#include "ins8250.h"

#pragma VxD_LOCKED_DATA_SEG

extern P_HARDWAREINFO pVMHwInfo;

void CPQSetupTablet(void);
void CPQRestoreTablet(void);

CHAR bMiscOptSav = 0;

#pragma VxD_LOCKED_CODE_SEG

#ifdef CPQ

/*
-------------------------------------------------------------------------------

 Function: CPQ_Build_Pen_Packet
 Params:   lppp - pointer destination for pen packet
 Return:   long - FF_ condition from above
 Comment:  Gets called when there is a complete pen packet that needs to
           be interpreted into a pen packet.

-------------------------------------------------------------------------------
*/
/************************************************************************
*
* Format of CPQ packet:
*
*   Byte  MSB 7   6     5     4     3     2     1   LSB 0
*        -------------------------------------------------
*    0   | PR  | CMD | RES | RES | RES | RES | SW1 | SW0 | 
*        -------------------------------------------------
*    1   | X15 | X14 | X13 | X12 | X11 | X10 | X9  | X8  |
*        -------------------------------------------------
*    2   | X7  | X6  | X5  | X4  | X3  | X2  | X1  | X0  |
*        -------------------------------------------------
*    3   | Y15 | Y14 | Y13 | Y12 | Y11 | Y10 | Y9  | Y8  |
*        -------------------------------------------------
*    4   | Y7  | Y6  | Y5  | Y4  | Y3  | Y2  | Y1  | Y0  |
*        -------------------------------------------------
*
*   SW0 = Tip status.
*   SW1 = Barrel status.
*   CMD = Set if this is response to a command.
*   PR  = Proximity: 1 = in proximity, 0 = out of proximity.
************************************************************************/

DWORD Concerto_ServiceInterrupt(LPDRV_PENPACKET lppp)
{
   BYTE bStatus,bRealStatus;
   WORD wX,wY;

   _asm {
// debug concerto
//int 3

//
// Check for spurious interrupts.
//
   mov   dx,INDEXADDRREG
   mov   al,PACKSIZEREGADDR
   out   dx,al

   mov   dx,DATAREG
   in   al,dx
   mov   bStatus,al
//
// add statistics...
//
   }   // end _asm

   if(!(bStatus & FIFO_EMPTY))
   {
      _asm {
;
; Set up to read data from the FIFO.
;
      mov   dx,INDEXADDRREG
      mov   al,FIFOADDR
      out   dx,al

      mov   dx,DATAREG
   
;
; Read the bytes from the FIFO. 
; 
      in   al,dx
      mov   bRealStatus,al

;
; X coordinate
;
      in   al,dx
      mov   ah,al
      in    al,dx
      mov   wX,ax

;
; Y coordinate
;
      in   al,dx
      mov   ah,al
      in    al,dx
      mov   wY,ax

      }   // end _asm

      if(bRealStatus &  CPQPEN_IN_RANGE)
      {   // in range pen packet

// debug concerto
//_asm int 3
         lppp->wPDK = 0;      // clear OOR...
         if(bRealStatus & CPQ_TIP_DOWN)
            lppp->wPDK|=PDK_DOWN;
         if(bRealStatus & CPQ_BARREL_DOWN)
            lppp->wPDK|=PDK_BARREL1;

         lppp->wTabletX=wX;
         lppp->wTabletY=wY;
         return (DWORD)(FF_SUCCESS | FF_PENPACKET);
      }
      else // Pen is not near or down.
      {
         lppp->wTabletX=0;
         lppp->wTabletY=0;
         lppp->wPDK=PDK_OUTOFRANGE;

         return (DWORD)(FF_SUCCESS | FF_OORPENPACKET);
      }
   }

//
// this was really a spurious interrupt - need to take some action besides
// this, but for now....
//
   else
   {
      lppp->wTabletX=0;
      lppp->wTabletY=0;
      lppp->wPDK=PDK_OUTOFRANGE;

      return (DWORD)(FF_SUCCESS | FF_OORPENPACKET);
   }
}


DWORD Concerto_Enable()
{
   CPQSetupTablet();
   return 1;
}


DWORD Concerto_Disable()
{
   CPQRestoreTablet();
   return 1;
}


DWORD Concerto_SetSamplingRate(DWORD lParam1,DWORD lParam2)
{
   return lParam1;
}


//
// PenWin 1.0 assembler functions  - convert to 'c'...
//

//******************************************************************************
//
//   CPQSendCmdByte
//
//   DESCRIPTION:
//      This routine sends a command byte to the 8051 digitizer
//      processor.  The SPRATLY ASIC is used to communicate between
//      the 8051 and the system processor.  This ASIC uses an
//      index register to select the port which will be read
//      from the data register.  In order to write to the 8051,
//      the "secondary system processor to 8051 mailbox" is 
//      used.  In order to determine that the 8051 is ready to
//      receive a command, the "mailbox empty/full" register must
//      be read.  Bit 3 is clear when it is ok to write to the
//      secondary mailbox register.
//      If not able to write to the register after several tries,
//      display an error message and return to the caller with
//      carry set.
//      
//
//   ENTRY:
//      ah = cmd byte to write
//
//   EXIT:
//      carry clear = success
//      carry set   = unable to send command (mailbox unavailable)
//
//   USES:
//      al, bx, cx, dx
//
//   NOTE:
//
//==============================================================================

int   SendCmdByte(CHAR bCmd)
{

// debug concerto
//_asm int 3

_asm   mov   cx,MBOX_WAIT_COUNT

ReadMBoxStat:
_asm   pushf
_asm   cli

//
// Select the mailbox empty/full register.
//
_asm   mov   al,MBOXSTATADDR
_asm   mov   dx,INDEXADDRREG
_asm   out   dx,al

//
// Now, read the data register until the secondary system processor
// to 8051 mailbox full bit is reset.
//
_asm   mov   dx,DATAREG
_asm   in   al,dx
_asm   test   al,SECMBOXFULL
_asm   jz   short MboxAvail

//
// The MBOX register was busy, so restore the interrupt flag and
// decrement the try counter and then try again.
//
_asm   popf

//   Delay84h USEC_COUNT      ; delay 100 usec
_asm   push   cx

_asm   mov   cx,USEC_COUNT
SCBDelay:
_asm   in al,0x84
_asm   loop   SCBDelay

_asm   pop   cx
_asm   loop   ReadMBoxStat

//
// Have timed out - return false!!!
//
//_asm int 3
   return 0;

MboxAvail:
//
// The mailbox register is available, so send the command.
//
//_asm   pushf
//_asm   cli
_asm   mov   al,SECMBOXADDR
_asm   mov   dx,INDEXADDRREG
_asm   out   dx,al           ; select secondary mbox register
_asm   mov   al,bCmd         ; get command to send
_asm   mov   dx,DATAREG
_asm   out   dx,al

_asm   popf                  ; restore int flag state

   return 1;
}


//---------------------------Public-Routine-----------------------------;
// ResetTablet - Sends a reset command to the tablet and waits for
//      end of reset.
//
//
// Input:
//   none
// Output:
//   none
//
// Needs:
//   Error checking!
//-----------------------------------------------------------------------;
void ResetTablet(void)
{
CHAR   bData;


// debug concerto
//_asm int 3

   SendCmdByte(RESET_CMD);

//
// need error checking!!!
//

//
// Determine the start of reset by reading the packet size register
// until the "in reset" bit is set.
//
   do
   {
      CPQ_READ_INDEX_PORT(PACKSIZEREGADDR, bData)
   } while (!(bData & 0x20));

//
// need time out check
//

//
// Determine the end of reset by reading the packet size register
// until the "in reset" bit is clear.
//
   do
   {
      CPQ_READ_INDEX_PORT(PACKSIZEREGADDR, bData)
   } while (bData & 0x20);

//
// need time out check
//
}


//---------------------------Public-Routine-----------------------------;
// setup_tablet - put digitizer in correct state:
//   Stream mode, send points continuously regardless of whether
//   in proximity.
//-----------------------------------------------------------------------;

void CPQSetupTablet(void)
{
   DWORD ddIRQ;
//
// The following table maps IRQs to the IRQ mask bits in the
// miscellaneous options register.
//
CHAR   IRQTable[] = {0x00, 0x40, 0x80, 0xc0, 0xc0, 0xc0, 0xc0};
/****************************
00000000b   ; IRQ 9
01000000b   ; IRQ 10
10000000b   ; IRQ 11
11000000b   ; IRQ 12 - error, default to IRQ 15
11000000b   ; IRQ 13 - error, default to IRQ 15
11000000b   ; IRQ 14 - error, default to IRQ 15
11000000b   ; IRQ 15
*****************************/

//
// Save the current tablet settings so they can be restored on
// exit.
//
//   Format of Miscellaneous options register:
//       ------------------------------------------------------------
//       | IRQ1  | IRQ0 | FIFO | RES | DISK | SLOTON | IRQ12 | BEEP | 
//       ------------------------------------------------------------
//
//   BEEP    0 = beeps enabled
//           1 = beeps disabled
//   IRQ12   0 = Mouse ints enabled on IRQ12
//           1 = Mouse ints disabled
//   SLOTON  0 = option slot off
//           1 = option slot on
//   DISK    0 = disk change clear enable
//           1 = indicate disk change
//   RES     Reserved
//   FIFO    0 = FIFO interrupt map disabled
//           1 = FIFO interrupt map enabled
//
//   IRQ1   IRQ0   Digitizer interrupt
//    0    0      IRQ9
//    0    1      IRQ10
//    1    0      IRQ11
//    1    1      IRQ15
//


// debug concerto
//_asm int 3


_asm   mov   dx,MISCOPTIONREG
_asm   in   al,dx
_asm   mov   bMiscOptSav,al          ; save for disable routine
_asm   and   bMiscOptSav,MISC_MASK   ; mask off bits that are not ours

//
// Get the bit mask for the IRQ. 
// The following algorithm is a fairly easy way to get the IRQ bits, but
// is VERY specific to this hardware...  BE CAREFUL - if the IRQs ever change,
// this code will have to be modified.
//
   ddIRQ=pVMHwInfo->ddIRQ_Number;
_asm  mov     ebx,ddIRQ
//_asm   mov   ebx,word ptr HwInfo.ddIRQ_Number
_asm   sub   ebx,9                 ; zero base irq
_asm   mov   bl,IRQTable[ebx]      ; get IRQ bits into bl
_asm   or   bl,FIFO_ENABLE         ; set FIFO enable bit
_asm   and   al,not MISC_MASK      ; set rest of misc opt bits
_asm   or   al,bl                  ; to original values
_asm   out   dx,al                 ; and set it up.

//
// need to get the mode and protect against a full FIFO
// before the getmode cmd is sent in the final driver...
//

//
// Reset the tablet.
//
   ResetTablet();

//
// Need to add code to setup popup orientation.
//

//
// Put the tablet in Streaming mode.
//
   SendCmdByte(MODE_CMD);

   SendCmdByte(STREAM_MOD);

}

//---------------------------Public-Routine------------------------------;
// CPQRestoreTablet - called on VPEND_DISABLE message.
//-----------------------------------------------------------------------;
void CPQRestoreTablet(void)
{

// debug concerto
//_asm int 3

//
// Restore Popup orientation to default.
//

//
// Restore the MISC_OPT_REGIS to its original value.
// See SetupTablet for details on format.
//

_asm   mov   dx,MISCOPTIONREG
_asm   in   al,dx
_asm   and   al,not MISC_MASK   ; Clear digitizer bits
_asm   or   al,bMiscOptSav      ; or in old values
_asm   out   dx,al         ; and restore them.

//
// Reset the tablet.
//
   ResetTablet();

//
// Make sure the tablet is in a clean state at the end of tablet initialization.
//

}


//*****************************************************************************
DWORD Concerto_SpeakUp()
{
// debug concerto
//_asm int 3

// need error checking
// handle increment mode...

   SendCmdByte(MODE_CMD);
   SendCmdByte(STREAM_MOD);
   return 1;
}
DWORD Concerto_ShutUp()
{
// debug concerto
//_asm int 3

// need error checking
// handle increment mode...

   SendCmdByte(MODE_CMD);
   SendCmdByte(STREAM_MOD | PROX_MOD);
   return 1;
}

#endif   //CPQ

//End-Of-File
