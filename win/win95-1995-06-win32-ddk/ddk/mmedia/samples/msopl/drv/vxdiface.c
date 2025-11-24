//---------------------------------------------------------------------------
//
//  Module:   vxdiface.c
//
//  Description:
//     Interface to VxD(s)
//
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>

#include <stdlib.h>

#include "msopl.h"

#pragma optimize( "leg", off )

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
//      PHARDWAREINSTANCE phwi
//  
//      DWORD dn
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL NEAR PASCAL GetVxDInfo
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dn
)
{
   BOOL       fRetVal = FALSE ;
   MSOPLINFO  msoi ;
   LPVOID     pVxDEntry ;

   if (NULL == (pVxDEntry = phwi -> pVxDEntry))
      return fRetVal ;

   msoi.dwSize = sizeof( msoi ) ;

   _asm
   {
      push  es
      push  di
      push  ss
      pop   es

      _emit 0x66
      push  cx

      lea   bx, msoi

      ;
      ; This silly hack is because mov ecx, dn doesn't take...
      ;

      mov   ax, word ptr dn + 2
      push  ax
      mov   ax, word ptr dn
      push  ax

      _emit 0x66
      pop   cx

      mov   ax, MSOPL_API_GetInfoF_DevNode
      mov   dx, MSOPL_API_Get_Info
      call  dword ptr pVxDEntry

      _emit 0x66
      pop   cx

      jc    SHORT Failure
      mov   fRetVal, TRUE

   Failure:
      pop   di
      pop   es
   }

   if (fRetVal)
   {
      phwi -> wHardwareOptions = msoi.wHardwareOptions ;
      phwi -> wIOAddressSynth = msoi.wIOAddressSynth ;
      phwi -> dn = msoi.dn ;
   }

   return fRetVal ;

} // GetVxDInfo()


//--------------------------------------------------------------------------
//  
//  BOOL AcquireFM
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

BOOL NEAR PASCAL AcquireFM
(
    PHARDWAREINSTANCE   phwi
)
{
   BOOL    fRetVal = FALSE ;
   WORD    wIO = phwi -> wIOAddressSynth ;
   LPVOID  pVxDEntry ;

   if (NULL == (pVxDEntry = phwi -> pVxDEntry))
      return fRetVal ;

   if (phwi -> cAcquire)
   {
      phwi -> cAcquire++ ;
      return TRUE ;
   }

   _asm
   {
      mov   ax, wIO
      mov   dx, MSOPL_API_Acquire
      call  dword ptr pVxDEntry
      jc    SHORT Failure
      mov   fRetVal, TRUE
   Failure:

   }

   if (fRetVal)
      phwi -> cAcquire = 1 ;

   return fRetVal ;

} // AcquireFM()

//--------------------------------------------------------------------------
//  
//  BOOL ReleaseFM
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

BOOL NEAR PASCAL ReleaseFM
(
    PHARDWAREINSTANCE   phwi
)
{
   BOOL    fRetVal = FALSE ;
   WORD    wIO = phwi -> wIOAddressSynth ;
   LPVOID  pVxDEntry ;

   if (NULL == (pVxDEntry = phwi -> pVxDEntry))
      return fRetVal ;

   if (phwi -> cAcquire)
   {
      phwi -> cAcquire-- ;

      if (!phwi -> cAcquire)
      {
         _asm
         {
            mov   ax, wIO
            mov   dx, MSOPL_API_Release
            call  dword ptr pVxDEntry
            jc    SHORT Failure
            mov   fRetVal, TRUE
         Failure:

         }
      }
   }
   return fRetVal ;

} // ReleaseFM()

//--------------------------------------------------------------------------
//  
//  HPIPE pipeOpen
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      LPSTR psz
//  
//      PPIPEOPENSTRUCT pos
//  
//  Return (HPIPE):
//  
//--------------------------------------------------------------------------

HPIPE pipeOpen
(
    PHARDWAREINSTANCE   phwi,
    LPSTR               psz,
    PPIPEOPENSTRUCT     pos
)
{
   FNPIPEOPEN  fnpipeOpen ;

   if (NULL == (fnpipeOpen = phwi -> pVxDEntry))
      return NULL ;

   _asm mov dx, PIPE_API_Open
   return fnpipeOpen( phwi -> dn, psz, pos ) ;

} // pipeOpen()

//--------------------------------------------------------------------------
//  
//  VOID pipeClose
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      HPIPE hp
//  
//  Return (HPIPE):
//  
//--------------------------------------------------------------------------

VOID pipeClose
(
    PHARDWAREINSTANCE   phwi,
    HPIPE               hp
)
{
   FNPIPECLOSE  fnpipeClose ;

   if (NULL == (fnpipeClose = phwi -> pVxDEntry))
      return ;

   _asm mov dx, PIPE_API_Close
   fnpipeClose( phwi -> dn, hp ) ;

} // pipeClose()

#pragma optimize( "", on )

//---------------------------------------------------------------------------
//  End of File: vxdiface.c
//---------------------------------------------------------------------------
