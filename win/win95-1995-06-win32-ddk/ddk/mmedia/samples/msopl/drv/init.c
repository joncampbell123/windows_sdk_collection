//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------
//
//  Module:   init.c
//
//  Description:
//     MSOPL initialization routines
//
//
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>

#include <stdlib.h>

#include "msopl.h"
#include "resource.h"

#ifdef DEBUG
    WORD    wDebugLevel = 5 ;       // debug level
    //
    // these strings are NOT accessed at interrupt time, so they can
    // be based in discardable code (the init seg in this case).
    //
    static char BCODE STR_DRIVER[]     = "driver" ;
    static char BCODE STR_MMDEBUG[]    = "mmdebug" ;

    //
    // these strings are accessed at interrupt time, so they must be in
    // the fixed DS
    //
    char STR_PROLOGUE[] = "MSOPL: " ;
    char STR_CRLF[]     = "\r\n" ;
    char STR_SPACE[]    = " " ;
#endif

// Global driver status

WORD gwGlobalStatus ;

// Module handle

HMODULE ghModule ;

// Hardware instance list

PHARDWAREINSTANCE pHardwareList = NULL ;

//--------------------------------------------------------------------------
//  
//  VOID AddInstance
//  
//  Description:
//      Adds the instance to the device instance list.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         ptr -> hardware instance structure
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID AddInstance
(
    PHARDWAREINSTANCE   phwi
)
{
   phwi -> pNext = pHardwareList ;
   pHardwareList = phwi ;

} // AddInstance()

//--------------------------------------------------------------------------
//  
//  VOID RemoveInstance
//  
//  Description:
//      Removes the instance from device's list
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         ptr -> hardware instance structure
//
//  Return (VOID):
//      Nothing.
//
//--------------------------------------------------------------------------

VOID RemoveInstance
(
    PHARDWAREINSTANCE   phwi
)
{
   PHARDWAREINSTANCE *ppCur ;

   for (ppCur = &pHardwareList ;
        *ppCur != NULL;
        ppCur = &(*ppCur)->pNext)
   {
      if (*ppCur == phwi)
      {
         *ppCur = (*ppCur)-> pNext ;
         break ;
      }
   }

} // RemoveInstance()

//--------------------------------------------------------------------------
//  
//  LRESULT AddDevNode
//  
//  Description:
//      "Registers" a devnode with the driver.  If an instance of
//      this devnode already exists, the count is incremented.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL AddDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   DPF( 1, "AddDevNode" ) ;

   if (NULL != (phwi = DevNodeToHardwareInstance( dn )))
   {
      phwi -> cReference++ ;
      phwi -> cEnable++ ;
      return MMSYSERR_NOERROR ;
   }

   if (NULL == 
      (phwi = (PHARDWAREINSTANCE) LocalAlloc( LPTR, 
                                              sizeof( HARDWAREINSTANCE ) )))
      return MMSYSERR_NOMEM ;

   // Add instance to list.  We will fail _all_ MIDI requests until
   // we are unloaded the the VxD does not have the proper hardware
   // support.  When MMSYSTEM loads this driver, it will check the number
   // of devices supported, we will return 0 under the above circumstances
   // and will be unloaded.

   phwi -> dn = dn ;
   AddInstance( phwi ) ;

   phwi -> cReference = 1 ;

   return MMSYSERR_NOERROR ;

} // AddDevNode()


//--------------------------------------------------------------------------
//  
//  LRESULT EnableDevNode
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dn
//
//      UINT uVxDId
//         Supporting VxD ID
//  
//  Return (LRESULT):
//  
//
//--------------------------------------------------------------------------

LRESULT FAR PASCAL EnableDevNode
(
    DWORD           dn,
    UINT            uVxDId
)
{
   PHARDWAREINSTANCE  phwi ;
   PIPEOPENSTRUCT     pos ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (phwi -> cEnable)
   {
      phwi -> cEnable++ ;
      return MMSYSERR_NOERROR ;
   }

   if ((NULL == (phwi -> pVxDEntry = GetVxDEntry( uVxDId ))) ||
       !GetVxDInfo( phwi, dn ))
      return MMSYSERR_BADDEVICEID ;

   phwi -> fSoftwareVolumeEnabled = TRUE ;
   phwi -> dwSynthVolume = 0xFFFFFFFF ;
   phwi -> dwMasterVolume = 0xFFFFFFFF ;

   pos.cbSize = sizeof( pos ) ;
   pos.pClientProc = mixerPipeProc ;
   if (NULL == (phwi -> hpmxd = pipeOpen( phwi, "MSOPLMXD", &pos )))
   {
      DPF( 1, "pipeOpen() failed!" ) ;
   }

   phwi -> cEnable = 1 ;
   if (phwi -> wIOAddressSynth)
      phwi -> fEnabled = 1 ;

   return MMSYSERR_NOERROR ;

} // EnableDevNode()

//--------------------------------------------------------------------------
//  
//  LRESULT DisableDevNode
//  
//  Description:
//      Decrements the enabled count for the hardware instance.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL DisableDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   DPF( 1, "DisableDevNode" ) ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (--phwi -> cEnable)
      return MMSYSERR_NOERROR ;

   //
   //  at this point, it is safe to shut things down hard
   //

   if (phwi -> fEnabled)
   {
      AcquireFM( phwi ) ;

      // Turn off the chip...

      fmQuiet( phwi ) ;

      if (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED)
      {
         fmSend( phwi, AD_CONNECTION, 0x00 ) ;
         fmSend( phwi, AD_NEW, 0x00 ) ;
      }

      ReleaseFM( phwi ) ;
   }

   if (phwi -> hpmxd)
   {
      pipeClose( phwi, phwi -> hpmxd ) ;
      phwi -> hpmxd = NULL ;
   }

   phwi -> fEnabled = 0 ;

   return MMSYSERR_NOERROR ;

} // DisableDevNode()

//--------------------------------------------------------------------------
//  
//  LRESULT RemoveDevNode
//  
//  Description:
//      Removes a reference to the given devnode, when reference count
//      of zero is reached the hardware instance structure is removed.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL RemoveDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (--phwi -> cReference)
      return MMSYSERR_NOERROR ;

   if (phwi -> cEnable)
   {
      DPF( 1, "Trying to remove devnode but ref != enable" ) ;
      return MMSYSERR_ALLOCATED ;
   }

   RemoveInstance( phwi ) ;

   LocalFree( (LOCALHANDLE) phwi ) ;

   return MMSYSERR_NOERROR ;

} // RemoveDevNode()

//--------------------------------------------------------------------------
//  
//  PHARDWAREINSTANCE DevNodeToHardwareInstance
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dn
//  
//  Return (PHARDWAREINSTANCE):
//  
//--------------------------------------------------------------------------

PHARDWAREINSTANCE FAR PASCAL DevNodeToHardwareInstance
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  pCur ;

   if (!pHardwareList)
      return NULL ;

   for (pCur = pHardwareList; pCur != NULL; pCur = pCur -> pNext)
   {
      if (pCur -> dn == dn)
          return pCur ;
   }

   return NULL ;

} // DevNodeToHardwareInstance()

//------------------------------------------------------------------------
//  LRESULT FAR PASCAL DrvInit( VOID )
//
//  Description:
//     Verifies that the VxD loaded successfully.
//
//  Parameters:
//     Nothing.
//
//  Return Value (LRESULT):
//     MMSYSERR_NOERROR if no problem.
//
//------------------------------------------------------------------------

LRESULT FAR PASCAL DrvInit()
{
   HMMIO     hmmio ;
   MMCKINFO  mmckinfo, mm2 ;
   char      szPatchLib[ _MAX_PATH ] ;

   DPF( 1, "DrvInit" ) ;

   // Allocate the memory, and fill it up from the patch
   // library. The name of the library has been set previously
   // and written into szPatchLib

   glpPatch =
      (LPPATCHES)
         GlobalAllocPtr( GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT,
                         sizeof( PATCHES ) * NUMPATCHES ) ;
   if (!glpPatch)
   {
      DPF( 1, "DrvInit: could not allocate patch container memory!" ) ;
      return MMSYSERR_NOMEM ;
   }

   // Default to load patches from our resource, otherwise
   // load from file if user has specified.

   lstrcpy( szPatchLib, "Undefined" ) ;

#pragma message( REMIND( "want replaceable patch sets???" ) )

#if 0
   DrvGetProfileString( gszIniKeyPatchLib, "Undefined",
                        szPatchLib, sizeof( szPatchLib ), TRUE ) ;
#endif

   if (lstrcmpi( (LPSTR) szPatchLib, "Undefined" ) == 0)
   {
      HRSRC   hrsrcPatches ;
      HGLOBAL hPatches ;
      LPSTR   lpPatches ;

      hrsrcPatches =
         FindResource( ghModule, MAKEINTRESOURCE( DATA_FMPATCHES ),
                       RT_BINARY ) ;

      if (NULL != hrsrcPatches)
      {
         hPatches = LoadResource( ghModule, hrsrcPatches ) ;
         lpPatches = LockResource( hPatches ) ;
         _fmemcpy( glpPatch, lpPatches, sizeof( PATCHES ) * NUMPATCHES ) ;
         UnlockResource( hPatches ) ;
         FreeResource( hPatches ) ;
      }
      else
      {
         GlobalFreePtr( glpPatch ) ;
         glpPatch = NULL ;
         return MMSYSERR_NOMEM ;
      }

#pragma message( REMIND( "throw up message box when can't load patches?" ) )

#if 0
      else
      {
         char   szAlert[ 50 ] ;
         char   szErrorBuffer[ 255 ] ;

         LoadString( ghModule, SR_ALERT, szAlert, sizeof( szAlert ) ) ;
         LoadString( ghModule, SR_ALERT_NORESOURCE, szErrorBuffer,
                     sizeof( szErrorBuffer ) ) ;
         MessageBox( NULL, szErrorBuffer, szAlert, MB_OK|MB_ICONHAND ) ;
      }
#endif

   }
   else
   {
      hmmio = mmioOpen( szPatchLib, NULL, MMIO_READ ) ;
      if (hmmio)
      {
         mmioDescend( hmmio, &mmckinfo, NULL, 0 ) ; 
         if ((mmckinfo.ckid == FOURCC_RIFF) &&
            (mmckinfo.fccType == RIFF_PATCH))
         {
            mm2.ckid = RIFF_FM4;
            if (!mmioDescend( hmmio, &mm2, &mmckinfo, MMIO_FINDCHUNK ))
            {
               // We have found the synthesis chunk

               if (mm2.cksize > (sizeof( PATCHES ) * NUMPATCHES))
                  mm2.cksize = sizeof( PATCHES ) * NUMPATCHES ;
               mmioRead( hmmio, (LPSTR) glpPatch, mm2.cksize ) ;
            }
            else
            {
               DPF( 1, "Bad mmioDescend2" ) ;
            }
         }
         else
         {
            DPF( 1, "Bad mmioDescend1" ) ;
         }

         mmioClose( hmmio, 0 ) ;
      }
      else
      {
         GlobalFreePtr( glpPatch ) ;
         glpPatch = NULL ;
         return MMSYSERR_NOMEM ;
      }

#pragma message( REMIND( "throw up message box when can't load patches?" ) )

#if 0

      else
      {
         char   szAlert[ 50 ] ;
         char   szErrorBuffer[ 255 ] ;

         LoadString( ghModule, SR_ALERT, szAlert, sizeof( szAlert ) ) ;
         LoadString( ghModule, SR_ALERT_NOPATCH, szErrorBuffer,
                     sizeof( szErrorBuffer ) ) ;
         MessageBox( NULL, szErrorBuffer, szAlert, MB_OK|MB_ICONHAND ) ;
         D1 ( "\nBad mmioOpen" ) ;
      }
#endif

   }

   DPF( 1, "DrvInit() -- finished" ) ;
   return 0 ;

} // end of DrvInit()

//------------------------------------------------------------------------
//  VOID DrvEnd
//
//  Description:
//     Closes up the hardware and interrupts.
//
//  Parameters:
//     None.
//
//  Return Value:
//     None.
//
//------------------------------------------------------------------------

VOID FAR PASCAL DrvEnd()
{
   DPF( 1, "DrvEnd" ) ;

   if (glpPatch)
   {
      GlobalFreePtr( glpPatch ) ;
      glpPatch = NULL ;
   }

} // end of DrvEnd()


//--------------------------------------------------------------------------
//  
//  int LibMain
//  
//  Description:
//      Library initialization code
//  
//  Parameters:
//      HMODULE hModule
//         Module handle
//  
//      UINT uDataSeg
//         selector of data segment
//
//      UINT uHeapSize
//         Heap size as specified in .DEF
//  
//      LPSTR lpCmdLine
//         command line passed from kernel
//  
//  Return (int):
//      1 if successful
//  
//--------------------------------------------------------------------------

int FAR PASCAL LibMain
(
    HMODULE         hModule,
    UINT            uDataSeg,
    UINT            uHeapSize,
    LPSTR           lpCmdLine
)
{
#ifdef DEBUG
    // get debug level - default is 3

    wDebugLevel = GetProfileInt( STR_MMDEBUG, STR_DRIVER, 3 ) ;
#endif

    //
    //  save our module handle
    //

    ghModule = hModule;

    //
    //  succeed the load...
    //
    return (1) ;

} // LibMain()

//---------------------------------------------------------------------------
//  End of File: init.c
//---------------------------------------------------------------------------
