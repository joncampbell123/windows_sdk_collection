//---------------------------------------------------------------------------
//
//  Module:   init.c
//
//  Description:
//     MSMPU401 initialization routines
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
#include <mmddk.h>
#include <mmreg.h>

#include <stdlib.h>

#include "msmpu401.h"

#ifdef DEBUG
    WORD    wDebugLevel = 3 ;       // debug level
    //
    // these strings are NOT accessed at interrupt time, so they can
    // be based in discardable code (the init seg in this case).
    //
    static char BCODE STR_DRIVER[]     = "msmpu401" ;
    static char BCODE STR_MMDEBUG[]    = "debug" ;

    //
    // these strings are accessed at interrupt time, so they must be in
    // the fixed DS
    //
    char STR_PROLOGUE[] = "MSMPU401\\" ;
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
      return MMSYSERR_NOERROR ;
   }

   if (NULL == 
      (phwi = (PHARDWAREINSTANCE) LocalAlloc( LPTR, 
                                              sizeof( HARDWAREINSTANCE ) )))
      return MMSYSERR_NOMEM ;

   phwi -> dn = dn ;
   phwi -> cReference = 1 ;
   AddInstance( phwi ) ;

   return MMSYSERR_NOERROR ;

} // AddDevNode()

//--------------------------------------------------------------------------
//  
//  LRESULT EnableDevNode
//  
//  Description:
//      Enables a devnode.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//
//      UINT uVxDId
//         Supporting VxD ID
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
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

   DPF( 1, "EnableDevNode" ) ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (phwi -> cEnable++)
      return MMSYSERR_NOERROR ;

   phwi -> uVxDId = uVxDId ;
   if ((NULL == (phwi -> pVxDEntry = GetVxDEntry( phwi -> uVxDId ))) ||
       !GetVxDInfo( phwi, dn ))
      return MMSYSERR_INVALHANDLE ;

#pragma message( REMIND( "Need to read MIDI-in persistence from registry" ) )

   phwi -> wMidiInPersistence = DEF_MIDIINPERSISTENCE ;

   if (phwi -> wHardwareOptions & MSMPU401_HWOPTIONSF_IRQSHARED)
   {
      pos.cbSize = sizeof( pos ) ;
      pos.pClientProc = isrPipeProc ;
      if (NULL == (phwi -> hpisr = pipeOpen( phwi, "MSMPUISR", &pos )))
      {
         DPF( 1, "pipeOpen() failed!" ) ;
      }
   }
   else if (phwi -> bIRQ != (BYTE) -1)
   {
      if (!Create_ISR( phwi ))
      {
         DPF( 1, "Failed to create ISR!!!\r\n" ) ;

         return MMSYSERR_NOTENABLED ;
      }

      DPF( 1, "hooking interrupt" ) ;

      // Command structure needed to EOI the PIC

      phwi -> wEOICommands = (phwi -> bIRQ > 7) ?
                                (0x6200 | (0x60 | (phwi -> bIRQ & 0x07)) ) : 
                                ((WORD) (phwi -> bIRQ | 0x60) << 8) ;

      phwi -> bOrigIntMask =
         (BYTE) SetInterruptMask( phwi -> bIRQ, 1 ) ;
      phwi -> lpOldISR =
         SetInterruptVector( phwi -> bIRQ,
                             (LPVOID) MAKELONG( 0, phwi -> uISRCodeSel ) ) ;
      SetInterruptMask( phwi -> bIRQ, 0 ) ;
   }

   if (phwi -> wIOAddressMPU401)
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

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (--phwi -> cEnable)
      return MMSYSERR_NOERROR ;

   if (phwi -> wHardwareOptions & MSMPU401_HWOPTIONSF_IRQSHARED)
   {
      if (phwi -> hpisr)
      {
         pipeClose( phwi, phwi -> hpisr ) ;
         phwi  -> hpisr = NULL ;
      }

   }
   else if (phwi -> bIRQ != (BYTE) -1)
   {
      // Disable the interrupt

      SetInterruptMask( phwi -> bIRQ, 1 ) ;
      SetInterruptVector( phwi -> bIRQ, (LPVOID) phwi -> lpOldISR ) ;
      SetInterruptMask( phwi -> bIRQ, phwi -> bOrigIntMask ) ;\

      //
      // Free the ISR stub...
      //

      GlobalFree( phwi -> uISRDataSel ) ;
      FreeSelector( phwi -> uISRCodeSel ) ;
      phwi -> uISRDataSel = NULL ;
      phwi -> uISRCodeSel = NULL ;
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

   DPF( 1, "RemoveDevNode" ) ;

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
   DPF( 1, "DrvInit" ) ;

   return 0 ;

} // end of DrvInit()

//--------------------------------------------------------------------------
//  
//  LRESULT isrPipeProc
//  
//  Description:
//  
//  
//  Parameters:
//      HPIPE hp
//  
//      DWORD dwMsg
//  
//      DWORD dwParam1
//  
//      DWORD dwParam2
//  
//  Return (LRESULT):
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL _loadds isrPipeProc
(
    HPIPE           hp,
    DWORD           dwMsg,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PHARDWAREINSTANCE  phwi ;

   //
   // dwParam1 is PnP DevNode
   //

   if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
      return PIPE_ERR_INVALIDPARAM ;

   switch( dwMsg )
   {
      case PIPE_MSG_OPEN:
         break ;

      case PIPE_MSG_INIT:
         phwi -> fnisrPipe = (FNPIPECALLBACK) dwParam2 ;
         break ;

      case PIPE_MSG_CLOSE:
         phwi -> hpisr = NULL ;
         phwi -> fnisrPipe = NULL ;
         break ;
   }

   return PIPE_ERR_NOERROR ;

} // isrPipeProc()

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
//
//------------------------------------------------------------------------

VOID FAR PASCAL DrvEnd()
{
   DPF( 1, "DrvEnd" ) ;

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
