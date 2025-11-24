//---------------------------------------------------------------------------
//
//  Module:   volume.c
//
//  Purpose:
//     Volume control interface (portions superseded by mixer)
//
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1991 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>

// stop compiler from griping about in-line

#pragma warning (disable:4704)

#define	Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include <ctype.h>
#include "mssndsys.h"
#include "driver.h"

#define SILENCE                 (192)

//--------------------------------------------------------------------------
//  
//  BYTE VolLinearToLog
//  
//  Description:
//      Converts a linear scale to logarithm (0xFFFF -> 0, 0x0001 -> 191)
//  
//  Parameters:
//      WORD wVolume
//         MMSystem.DLL's volume (0x0000 to 0xFFFF)
//  
//  Return (BYTE):
//      Value in decibels attenuation, each unit is 1.5 dB
//  
//  
//--------------------------------------------------------------------------

BYTE FAR PASCAL VolLinearToLog
(
    WORD            wVolume
)
{
   WORD    gain, shift ;
   WORD    temp ;
   WORD    lut[16] = {0,0,0,1,1,1,2,2,2,2,3,3,3,3,3,3} ;
   BYTE    out ;

   //
   // Catch boundary conditions...
   //

   if (wVolume == 0xFFFF)
      return ( 0 ) ;

   if (wVolume == 0x0000)
      return ( SILENCE ) ;

   // Get an estimate to within 6 dB of gain

   for (temp = wVolume, gain = 0, shift = 0;
         temp != 0;
         gain += 4, temp >>= 1, shift++);

   // Look at highest 3 bits in number into
   // look-up-table to find how many more dB

   if (shift > 5)
      temp = wVolume >> (shift - 5) ;
   else if (shift < 5)
      temp = wVolume << (5 - shift) ; 
   else
      temp = wVolume ;
   temp &= 0x000f ;

   gain += lut[ temp ] ;

   out = (BYTE) ((16 * 4) + 3 - gain) ;
   return (out < 128) ? out : (BYTE) 127 ;

} // VolLinearToLog()

//--------------------------------------------------------------------------
//  
//  WORD VolLogToLinear
//  
//  Description:
//      Convert logarithmic attenuation to linear
//         (0x00 -> 0xFFFF, 0x3F -> 0x0001)
//  
//  Parameters:
//      WORD wLog
//         in units of 1.5 dB
//  
//  Return (WORD):
//      Linear value
//  
//  
//--------------------------------------------------------------------------

WORD FAR PASCAL VolLogToLinear
(
    WORD            wLog
)
{
   WORD    div, rem ;
   WORD    lut[4] = { 0xffff, 0xe000, 0xc000, 0xa000 } ;

   div = wLog / 4 ;
   rem = wLog % 4 ;

   if (div >= 16)
      return 0 ;

   return lut[rem] >> div ;

} // VolLogToLinear

//---------------------------------------------------------------------------
//  End of File: volume.c
//---------------------------------------------------------------------------
