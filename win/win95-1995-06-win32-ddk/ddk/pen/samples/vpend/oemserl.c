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

/* OEMSERL.c: 

   Misc OEM hardware code.<nl>

   Routines contained in module:<nl>
      <l CleanUpCom.CleanUpCom><nl>
      <l StartToSetUpCom.StartToSetUpCom><nl>
      <l ContinueToSetUpCom.ContinueToSetUpCom><nl>
      <l OutTabletString.OutTabletString><nl>
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

DWORD CleanUpCom()
{
   DWORD ddcom_base=pVMHwInfo->com_base;
   BYTE  bBaudDivisorShift=LOBYTE(HIWORD(pVMHwInfo->dwComPortSettings));
   BYTE  bLineControlReg=LOBYTE(LOWORD(pVMHwInfo->dwComPortSettings));

   DBG_TRACE("In CleanUpCom");

   // Using a serial port?
   if(!ddcom_base)
      return 0;

   DBG_TRACE("ddcom_base != 0");

   DBG_TRACE("Disabled interrupts on 8250");

   _asm   mov   edx,ddcom_base ; set dtr and rts
   _asm   add   dx,ACE_MCR
   _asm   mov   al,ACE_DTR+ACE_RTS
   IO_Delay()
   _asm   out   dx,al

   return TRUE;
}


DWORD StartToSetUpCom()
{
   DWORD ddcom_base=pVMHwInfo->com_base;
   BYTE  bBaudDivisorShift=LOBYTE(HIWORD(pVMHwInfo->dwComPortSettings));
   BYTE  bLineControlReg=LOBYTE(LOWORD(pVMHwInfo->dwComPortSettings));

   DBG_TRACE("In StartToSetUpCom");

   // Using a serial port?
   if(!ddcom_base)
      return 0;

   DBG_TRACE("ddcom_base != 0");

   _asm   mov   edx,ddcom_base   ; access divisor latches
   _asm   add   dx,ACE_LCR
   _asm   mov   al,ACE_DLAB
   IO_Delay();
   _asm   out   dx,al

   DBG_TRACE("latches accessed");

   // dwBaudDivisorShift is the number of times to shift the divisor "3" to
   // the left so the proper baud van be used.   For instance, if the tablet
   // runs a 38400, dwBaud would be 0.  If the tablet runs at 9600,
   // dwBaudDivisorShift should be 2.   So 3<<2=12==divisor for al below.

   _asm   mov cl,bBaudDivisorShift
   _asm   mov   edx,ddcom_base      ; set baud rate
   _asm   add   dx,ACE_DLM
   _asm   xor   al,al
   IO_Delay();
   _asm   out   dx,al
   _asm   mov   edx,ddcom_base
   _asm   add   dx,ACE_DLL
   _asm   mov   al,3
   _asm   shl al,cl   // generate divisor
   IO_Delay();
   _asm   out   dx,al

   DBG_TRACE("baud established");

   _asm   mov al,bLineControlReg
   _asm   mov   edx,ddcom_base
   _asm   add   dx,ACE_LCR
   IO_Delay();
   _asm   out   dx,al

   DBG_TRACE("Leaving StartToSetUpCom");

   return (DWORD)1;
}


DWORD ContinueToSetUpCom()
{
   DWORD ddcom_base=pVMHwInfo->com_base;

   DBG_TRACE("Entering ContinueToSetUpCom");

   if(!ddcom_base)
      return 0;

   DBG_TRACE("Valid com_base");

   _asm   mov   edx,ddcom_base ; disable interrupts at 8250
   _asm   add   dx,ACE_IER
   _asm   xor   al,al
   IO_Delay()
   _asm   out   dx,al

   DBG_TRACE("Disabled interrupts on 8250");

   _asm   mov   edx,ddcom_base ; set dtr and rts
   _asm   add   dx,ACE_MCR
   _asm   mov   al,ACE_DTR+ACE_RTS
   IO_Delay()
   _asm   out   dx,al

   DBG_TRACE("Set DTR and RTS");

   _asm   mov   edx,ddcom_base ; grab ghost character
   IO_Delay()
   _asm   in   al,dx

   DBG_TRACE("Grab ghost character");

   _asm   mov   edx,ddcom_base ; enable DR interrupts at the 8250
   _asm   add   dx,ACE_IER
   _asm   mov   al,ACE_ERBFI
   // added this line
   IO_Delay()
   _asm   out   dx,al

   DBG_TRACE("Enabled DR Interrupts at the 8250");

   _asm   mov   edx,ddcom_base ; raise ACE_OUT2 to allow interrupts
   _asm   add   dx,ACE_MCR
   IO_Delay()
   _asm   in   al,dx
   _asm   or   al,ACE_OUT2
   IO_Delay()
   _asm   out   dx,al

   DBG_TRACE("Raise Out2 to allow Interrupts and leave");

   return 1;
}


DWORD OutTabletString(char *pStr)
{
   DWORD ddcom_base=pVMHwInfo->com_base;

   if(!ddcom_base)
      return 0;

   _asm   mov esi,pStr      ; contains a pointer to the string to send
   _asm   mov   ebx,ddcom_base
   _asm   mov   cx,bx
   _asm   add   cx,ACE_LSR
   _asm   cld
OTS_loop:
   _asm   cmp   byte ptr [esi],0
   _asm   je   SHORT OTS_done

   _asm   mov   dx,cx
OTS_wait:
   IO_Delay()
   _asm   in   al,dx
   _asm   and   al,ACE_THRE      ; Is transmit holding reg empty?
   _asm   jz   OTS_wait          ; No, wait until it is.

   _asm   mov   dx,bx
   _asm   mov   al,byte ptr [esi]
   _asm   inc   esi
   IO_Delay()
   _asm   out   dx,al            ; send out character
   _asm   jmp   OTS_loop
OTS_done:

   return (DWORD)1;
}


//End-Of-File
