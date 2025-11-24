//---------------------------------------------------------------------------
//
//  Module:   vxdiface.c
//
//  Description:
//     Interface routines to VxD(s)
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#include <mmddk.h>
#include <mmreg.h>
#include <stdlib.h>
#include "avcapt.h"
#include "avvxp500.h"
#include "debug.h"

#pragma optimize( "leg", off )
extern    WORD    wDebugLevel;       // debug level
//--------------------------------------------------------------------------
//  
//  LPVOID GetVxDEntry
//  
//  Description:
//  
//  
//  Parameters:
//      UINT uVxDId
//  
//  Return (LPVOID):
//  
//--------------------------------------------------------------------------

LPVOID NEAR PASCAL GetVxDEntry
(
    UINT            uVxDId
)
{
   LPVOID  pVxDEntry ;

   D1("GetVxDEntry");

   _asm
   {
      push  es
      push  di
      push  bx

      xor   di, di
      mov   es, di
      mov   ax, 0x1684
      mov   bx, uVxDId
      int   0x2F
      mov   word ptr pVxDEntry, di
      mov   word ptr pVxDEntry + 2, es

      pop   bx
      pop   di
      pop   es
   }

   return pVxDEntry ;

} // GetVxDEntry()

//--------------------------------------------------------------------------
//  
//  WORD GetVxDVersion
//  
//  Description:
//  
//  
//  Parameters:
//      LPVOID pVxDEntry
//  
//  Return (WORD):
//  
//--------------------------------------------------------------------------

WORD NEAR PASCAL GetVxDVersion
(
    LPVOID          pVxDEntry
)
{
   WORD  wRetVal = 0 ;

   D1("GetVxDVersion");
   if (!pVxDEntry)
      return wRetVal ;

   _asm
   {
      xor   dx, dx
      call  dword ptr pVxDEntry
      mov   wRetVal, ax
   }

   return wRetVal ;

} // GetVxDVersion()

//--------------------------------------------------------------------------
//  
//  BOOL GetVxDInfo
//  
//  Description:
//  
//  
//  Parameters:
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL NEAR PASCAL GetVxDInfo
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
)
{
   BOOL          fRetVal = FALSE ;
   AVVXP500INFO  avvi ;

   D1("GetVxDInfo");
   if (NULL == pVxDEntry) 
      return fRetVal ;

   avvi.dwSize = sizeof( avvi ) ;

   _asm
   {
      push  es
      push  di
      push  ss
      pop   es

      lea   bx, avvi

      xor   ax, ax
      mov   dx, AVVXP500_API_Get_Info
      call  dword ptr pVxDEntry

      jc    SHORT Failure
      mov   fRetVal, TRUE

   Failure:
      pop   di
      pop   es
   }

   if (fRetVal)
   {
      lpDI -> wIOBase    = avvi.wIOAddressVXP500 ;
      lpDI -> bInterrupt = avvi.bIRQ ;
      lpDI -> wSelector  = avvi.wSelector ;           
      lpDI -> wSegment = LOWORD( avvi.dwMemBase >> 12 ) ;
   }

   return fRetVal ;

} // GetVxDInfo()

//--------------------------------------------------------------------------
//  
//  BOOL AcquireVXP500
//  
//  Description:
//  
//  
//  Parameters:
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL AcquireVXP500
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
)
{
   BOOL    fRetVal = FALSE ;
   WORD    wIO = lpDI -> wIOBase ;

   D1("AcquireVXP500");

   if (NULL == pVxDEntry)
      return fRetVal ;

   if (lpDI -> cAcquire)
   {
      lpDI -> cAcquire++ ;
      return TRUE ;
   }

   _asm
   {
      mov   ax, wIO
      mov   dx, AVVXP500_API_Acquire
      call  dword ptr pVxDEntry
      jc    SHORT Failure
      mov   fRetVal, TRUE
   Failure:

   }

   if (fRetVal)
      lpDI -> cAcquire = 1 ;

   return fRetVal ;

} // AcquireVXP500()

//--------------------------------------------------------------------------
//  
//  BOOL ReleaseVXP500
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL FAR PASCAL ReleaseVXP500
(
    LPVOID              pVxDEntry,
    LPDEVICE_INIT       lpDI
)
{
   BOOL    fRetVal = FALSE ;
   WORD    wIO = lpDI -> wIOBase ;

   D1("ReleaseVXP500");

   if (NULL == pVxDEntry)
      return fRetVal ;

   if (lpDI -> cAcquire)
   {
      lpDI -> cAcquire-- ;

      if (!lpDI -> cAcquire)
      {
         _asm
         {
            mov   ax, wIO
            mov   dx, AVVXP500_API_Release
            call  dword ptr pVxDEntry
            jc    SHORT Failure
            mov   fRetVal, TRUE
         Failure:

         }
      }
   }
   return fRetVal ;

} // ReleaseVXP500()

#pragma optimize( "", on )

//---------------------------------------------------------------------------
//  End of File: vxdiface.c
//---------------------------------------------------------------------------
