//---------------------------------------------------------------------------
//
//  Module:   cnfgmgr.c
//
//  Description:
//     Config manager notification handler
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//---------------------------------------------------------------------------
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#define  WANTVXDWRAPS

#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <vmmreg.h>
#include <regstr.h>
#include <mmdevldr.h>
#include <pccard.h>
#include <debug.h>

#include "cnfgmgr.h"

#undef CURSEG               
#define CURSEG()                   PCODE

extern BOOL Validate_AutoSel_IRQ( WORD wBaseAutoSel, WORD wIRQ ) ;
extern BOOL Is_AutoSelect_Valid( WORD wBaseAutoSel ) ;

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG

// from detection module

#define REGSTR_PATH_CONFIG "\\Config"
#define STR_BLASTER "BLASTER"
#define STR_MIDI    "MIDI"

typedef struct tagPCMCIA_BusInfo
{
   WORD  wLogicalSocket ;
   WORD  wReserved ;

} PCMCIA_BusInfo ;

//--------------------------------------------------------------------------
//  
//  VOID MSSNDSYS_Log_Error
//  
//  Description:
//  
//  
//  Parameters:
//      PCHAR pStr
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID MSSNDSYS_Log_Error
(
    PCHAR           pStr
)
{
    if (0 == Open_Boot_Log())
    {
#ifdef MSSNDSYS
       Write_Boot_Log( "MSSNDSYS: ", 0 ) ;
#endif
#ifdef AZTECH
       Write_Boot_Log( "AZT16: ", 0 ) ;
#endif
       Write_Boot_Log( pStr, 0 ) ;
       Close_Boot_Log() ;
    }

} // MSSNDSYS_Log_Error()

//--------------------------------------------------------------------------
//  
//  VOID MSSNDSYS_Log_IRQ_Error
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dwFound
//  
//      DWORD dwExpecting
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID MSSNDSYS_Log_IRQ_Error
(
    DWORD           dwFound,
    DWORD           dwExpecting
)
{
   char  szBuffer[ 128 ] ;

   _Sprintf( szBuffer, "IRQ not responding (%x:%x)\n", 
             dwFound, dwExpecting ) ;
   MSSNDSYS_Log_Error( szBuffer ) ;

} // MSSNDSYS_Log_IRQ_Error()

//--------------------------------------------------------------------------
//  
//  VOID MSSNDSYS_Log_DMA_Error
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dwFound
//  
//      DWORD dwExpecting
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID MSSNDSYS_Log_DMA_Error
(
    DWORD           dwChannel, 
    DWORD           dwCount
)
{
   char  szBuffer[ 128 ] ;

   _Sprintf( szBuffer, "DMA not responding (%x:%x)\n", 
             dwChannel, dwCount ) ;
   MSSNDSYS_Log_Error( szBuffer ) ;

} // MSSNDSYS_Log_DMA_Error()

//--------------------------------------------------------------------------
//
//  CONFIGRET MSSNDSYS_MigrateRegistry
//
//  Description:
//      Migrates "selections" keys from hardware to software portion
//      of devnode.
//
//  Parameters:
//      DEVNODE dn
//
//  Return (CONFIGRET):
//
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_MigrateRegistry
(
    DEVNODE         dn
)
{
   char       szHWRegKey[ 256 ],
              szSWRegKey[ 256 ] ;
   BYTE       bValue ;
   CONFIGRET  cr = CR_SUCCESS ;
   ULONG      cbLen ;
   VMMHKEY    hkSW, hkSWConfig, hkHW ;

   if (cr = CM_Get_DevNode_Key( dn, NULL, szHWRegKey,
                                sizeof( szHWRegKey ),
                                CM_REGISTRY_HARDWARE ))
      return cr ;

   // If we can't open it, then we don't have anything to migrate

   strcat( szHWRegKey, REGSTR_PATH_CONFIG ) ;
   if (_RegOpenKey( HKEY_LOCAL_MACHINE, szHWRegKey, &hkHW ))
      return CR_SUCCESS ;

   // Ok, have to migrate something...

   if (cr = CM_Get_DevNode_Key( dn, NULL, szSWRegKey,
                                sizeof( szSWRegKey ),
                                CM_REGISTRY_SOFTWARE ))
   {
      _RegCloseKey( hkHW ) ;
      return cr ;
   }

   if (_RegOpenKey( HKEY_LOCAL_MACHINE, szSWRegKey, &hkSW ))
   {
      cr = CR_REGISTRY_ERROR ;
      goto Error_CleanUp ;
   }

   // Single Mode DMA...

   cbLen = sizeof( BYTE ) ;
   if (_RegQueryValueEx( hkHW, REGSTR_VAL_SINGLEMODEDMA, NULL, NULL,
                         &bValue, &cbLen ))
   {
      cr = CR_REGISTRY_ERROR ;
      goto Error_CleanUp ;
   }

   if (cbLen)
   {
      if (_RegOpenKey( hkSW, REGSTR_SUBKEY_CONFIG, &hkSWConfig ))
         _RegCreateKey( hkSW, REGSTR_SUBKEY_CONFIG, &hkSWConfig ) ;

      if (hkSWConfig)
      {
         cbLen = sizeof( BYTE ) ;
         _RegSetValueEx( hkSWConfig, REGSTR_VAL_SINGLEMODEDMA, NULL, REG_BINARY,
                         &bValue, cbLen ) ;
         _RegCloseKey( hkSWConfig ) ;
         hkSWConfig = NULL ;
      }
      else
      {
         cr = CR_REGISTRY_ERROR ;
         goto Error_CleanUp ;
      }
   }

   // delete the subtree in the hardware registry

   _RegCloseKey( hkHW ) ;
   hkHW = NULL ;
   _RegDeleteKey( HKEY_LOCAL_MACHINE, szHWRegKey ) ;

Error_CleanUp:

   if (hkHW)
      _RegCloseKey( hkHW ) ;

   if (hkSW)
      _RegCloseKey( hkSW ) ;

   if (hkSWConfig)
      _RegCloseKey( hkSWConfig ) ;

   return cr ;

} // MSSNDSYS_MigrateRegistry()

//--------------------------------------------------------------------------
//  
//  VOID MSSNDSYS_Get_Registry_Info
//  
//  Description:
//  
//  
//  Parameters:
//      PDEVNODE pdn
//
//      PDWORD pfdwOptions
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID MSSNDSYS_Get_Registry_Info
(
    DEVNODE         dn,
    PDWORD          pfdwOptions
)
{
   BOOL  fSetting ;
   ULONG ulLength ;
   BYTE   b;


   //
   // Single Mode DMA switch...
   //
   ulLength = sizeof(b);
   if (CR_SUCCESS ==
          CM_Read_Registry_Value( dn, 
                                  REGSTR_SUBKEY_CONFIG,
                                  REGSTR_VAL_SINGLEMODEDMA,
                                  REG_BINARY,
                                  &b,
                                  &ulLength,
                                  CM_REGISTRY_SOFTWARE ))
       fSetting = (0 != b);
   else
       fSetting = DEFAULT_SINGLEMODEDMA ;

   if (fSetting)
      *pfdwOptions |= SSI_FLAG_SINGLEMODEDMA ;

} // MSSNDSYS_Get_Registry_Info()

//--------------------------------------------------------------------------
//  
//  CONFIGRET MSSNDSYS_Get_ISA_Config
//  
//  Description:
//  
//  
//  Parameters:
//      DEVNODE dn
//  
//      PWORD pwBaseAutoSel
//  
//      PWORD pwBaseAGA
//  
//      PWORD pwBaseCODEC
//  
//      PWORD pwBaseOPL3
//
//      PWORD pwBaseSB
//  
//      PWORD pwIRQ
//  
//      PWORD pwPlaybackDMA
//  
//      PWORD pwCaptureDMA
//  
//  Return (CONFIGRET):
//      CR_SUCCESS or error return
//  
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_Get_ISA_Config
(
    DEVNODE         dn,
    PWORD           pwBaseAutoSel,
    PWORD           pwBaseAGA,
    PWORD           pwBaseCODEC,
    PWORD           pwBaseOPL3,
    PWORD           pwBaseSB,
    PWORD           pwIRQ,
#ifdef AZTECH
    PWORD           pwSBIRQ,
#endif
    PWORD           pwPlaybackDMA,
    PWORD           pwCaptureDMA
#ifdef AZTECH
    ,PWORD           pwSBDMA
#endif
)
{
   CONFIGRET  cr ;
   CMCONFIG   ccb ;

   //
   // Get our resource allocation from CM.  if this fails, we have no 
   // choice but to fail the CONFIG_START.  We'll return the same
   // error that the CM_Get_Alloc_Config returned.
   //

   if (CR_SUCCESS != 
         (cr = CM_Get_Alloc_Log_Conf( &ccb, dn, 0 )))
      return cr ;

   //
   // Extract the information of interest from the config buffer
   // that we got from CM_Get_Alloc_Config.
   //
#ifdef AZTECH
   if ( ccb.wNumIOPorts != 3 )
   {
      DPF( ("<AZTECH>: can't use this config (I/O Port != 3 )\r\n") ) ;
      return CR_FAILURE ;
   }

   if ((ccb.wIOPortLength[ 0 ] == 16) &&
       (ccb.wIOPortLength[ 1 ] == 8) &&
       (ccb.wIOPortLength[ 2 ] == 4))
   {
        *pwBaseSB = ccb.wIOPortBase[ 0 ] ;
        *pwBaseAutoSel = ccb.wIOPortBase[ 1 ] ;
        *pwBaseCODEC = *pwBaseAutoSel + 4 ;
        *pwBaseOPL3 = ccb.wIOPortBase[ 2 ] ;
   }
   else
   {
        DPF( ("<AZTECH>: can't use this config (I/O Wrong)\r\n") ) ;
        return CR_FAILURE ;
   }

#endif

#ifdef MSSNDSYS
   switch (ccb.wNumIOPorts)
   {
      case 1:
      {
         // CODEC _OR_ (CODEC + AutoSel only)...

         // If nPorts == 4, then there is no auto-select,
         // otherwise nPorts better be 8.
         
         switch (ccb.wIOPortLength[ 0 ])
         {
            case 4:
               *pwBaseCODEC = ccb.wIOPortBase[ 0 ] ;
               break ;

            case 8:
               *pwBaseAutoSel = ccb.wIOPortBase[ 0 ] ;
               *pwBaseCODEC = *pwBaseAutoSel + 4 ;
               break ;

            default:
               DBG_ERROR( ( "invalid wIOPortLength[ 0 ]" ) ) ;
               return CR_FAILURE ;
         }
      }
      break ;


      case 2:
      {
         // ((CODEC _OR_ CODEC with AutoSel) + OPL3) _OR_
         // (CODEC + AGA).

         switch (ccb.wIOPortLength[ 0 ])
         {
            case 8:
               // Has to be (CODEC with AutoSel) + OPL3,
               // this is 100% compatible the retail
               // version of the Windows Sound System.

               *pwBaseAutoSel = ccb.wIOPortBase[ 0 ] ;
               *pwBaseCODEC = *pwBaseAutoSel + 4 ;
               *pwBaseOPL3 = ccb.wIOPortBase[ 1 ] ;
               break ;

            case 4:
               // CODEC + (AGA _OR_ OPL3)...

               *pwBaseCODEC = ccb.wIOPortBase[ 0 ] ;
               *pwBaseAGA = ccb.wIOPortBase[ 1 ] ;

               if (!Is_AGA_Valid( *pwBaseCODEC, *pwBaseAGA, NULL ))
               {
                  *pwBaseAGA = (WORD) -1 ;
                  *pwBaseOPL3 = ccb.wIOPortBase[ 1 ] ;
               }
               break ;

            default:
               DBG_ERROR( ( "unknown I/O configuration" ) ) ;
               return CR_FAILURE ;
         }
      }
      break ;

      case 3:
      {
         //
         // CODEC, OPL3 + Sound Blaster register plane
         // (a.k.a. CS4232)
         //

         if ((ccb.wIOPortLength[ 0 ] == 4) &&
            (ccb.wIOPortLength[ 1 ] == 4) &&
            (ccb.wIOPortLength[ 2 ] == 16))
         {
            *pwBaseCODEC = ccb.wIOPortBase[ 0 ] ;
            *pwBaseOPL3 = ccb.wIOPortBase[ 1 ] ;
            *pwBaseSB = ccb.wIOPortBase[ 2 ] ;
         }
         else
         {
            DBG_ERROR( ( "can't use this config (3 I/O)" ) ) ;
            return CR_FAILURE ;
         }
      }
      break ;

      default:
         DBG_ERROR( ( "can't use this configuration" ) ) ;
         return CR_FAILURE ;
   }
#endif

#ifdef AZTECH
   if ((ccb.wNumIRQs < 2) || (ccb.wNumDMAs < 2))
#endif
#ifdef MSSNDSYS
   if ((ccb.wNumIRQs < 1) || (ccb.wNumDMAs < 1))
#endif
   {
      //
      // No way we can use this config!
      //

      return CR_FAILURE ;
   }

#ifdef AZTECH
   *pwSBIRQ = ccb.bIRQRegisters[ 0 ] ;
   *pwIRQ = ccb.bIRQRegisters[ 1 ] ;
   *pwSBDMA = ccb.bDMALst[ 0 ] ;
   *pwCaptureDMA = *pwPlaybackDMA = ccb.bDMALst[ 1 ] ;
#endif
#ifdef MSSNDSYS
   *pwIRQ = ccb.bIRQRegisters[ 0 ] ;

   if (ccb.wNumDMAs > 1)
   {
      *pwPlaybackDMA = ccb.bDMALst[ 0 ] ;
      *pwCaptureDMA = ccb.bDMALst[ 1 ] ;
   }
   else
      *pwCaptureDMA = *pwPlaybackDMA = ccb.bDMALst[ 0 ] ;
#endif

   if (-1 == *pwBaseCODEC)             
      return CR_FAILURE ;

   return CR_SUCCESS ;

} // MSSNDSYS_Get_ISA_Config()

#ifdef MSSNDSYS

//--------------------------------------------------------------------------
//  
//  CONFIGRET MSSNDSYS_Get_PCCard_Info
//  
//  Description:
//  
//  
//  Parameters:
//      DEVNODE dn
//  
//      PWORD pwSRAMOffset
//  
//      PWORD pwId
//  
//  Return (CONFIGRET):
//  
//  
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_Get_PCCard_Info
(
    DEVNODE         dn,
    PWORD           pwSRAMOffset,
    PWORD           pwId
)
{
   CMBUSTYPE            cmbt ;
   DWORD                rc, cbInfo ;
   GET_CONFIG_INFO_PKT  cip ;
   PCMCIA_BusInfo       pbi ;

   cbInfo = sizeof( PCMCIA_BusInfo ) ;
   CM_Get_Bus_Info( dn, &cmbt, &cbInfo, &pbi, 0 ) ;

   cip.Socket = pbi.wLogicalSocket ;

   DBG_TRACE( ("dn: %lx, logical socket: %d", dn, cip.Socket) ) ;

   if (rc = PCCARD_Get_Configuration_Info( &cip ))
   {
      DBG_ERROR( ("PCCARD returned an error: %d", rc) ) ;
      return CR_FAILURE ;
   }

   switch (cip.ManufInfo & 0xF)
   {
      case CIP_PRODUCTID_WAVJAMMER:

         switch (cip.ManufInfo & 0xF000)
         {
            case CIP_WAVJAMMER_REV_E:
               //
               // .WAVJammer Rev. E
               //

               DBG_TRACE( ("cip reports .WAVJammer Rev. E") ) ;

               //
               // SRAM offset is CODEC + 4
               //

               if (pwId)
                  *pwId = PCMCIA_ID_WAVJAMMER_REV_E ;

               if (pwSRAMOffset)
                  *pwSRAMOffset = 4 ;

               break ;

            case CIP_WAVJAMMER_REV_F:

               //
               // .WAVJammer Rev. F
               //

               DBG_TRACE( ("cip reports .WAVJammer Rev. F") ) ;

               //
               // SRAM offset is CODEC - 4
               //

               if (pwId)
                  *pwId = PCMCIA_ID_WAVJAMMER_REV_F ;

               if (pwSRAMOffset)
                  *pwSRAMOffset = (WORD) -4 ;
               break ;

            default:

               DBG_TRACE( ("cip reports unknown .WAVJammer revision") ) ;

               //
               // assume SRAM offset is CODEC - 4
               //

               if (pwId)
                  *pwId = PCMCIA_ID_WAVJAMMER_REV_OTHER ;

               if (pwSRAMOffset)
                  *pwSRAMOffset = (WORD) -4 ;

         }
         break ;

      case CIP_PRODUCTID_SOUNDSCSI:

         //
         // Sound/SCSI II
         //

         DBG_TRACE( ("cip reports Sound/SCSI II") ) ;

         //
         // SRAM offset is CODEC - 4
         //

         if (pwId)
            *pwId = PCMCIA_ID_SOUNDSCSI ;

         if (pwSRAMOffset)
            *pwSRAMOffset = (WORD) -4 ;

         break ;

      default:
         break ;
   }

   return CR_SUCCESS ;

} // MSSNDSYS_Get_PCCard_Info()

//--------------------------------------------------------------------------
//  
//  DWORD MSSNDSYS_Get_SCSI_Base
//  
//  Description:
//      Returns the base address of the SCSI controller on
//      New Media Sound/SCSI II combo by parsing the parent's
//      resources.
//  
//  Parameters:
//      DEVNODE dn
//  
//  Return (DWORD):
//      base address of SCSI adapter
//  
//  
//--------------------------------------------------------------------------

DWORD MSSNDSYS_Get_SCSI_Base
(
    DEVNODE         dn
)
{
   CMCONFIG  ccb ;
   DEVNODE   dnParent ;

   if ((CR_SUCCESS != CM_Get_Parent( &dnParent, dn, 0 )) ||
       (CR_SUCCESS != CM_Get_Alloc_Log_Conf( &ccb, dnParent, 0 )) ||
       (ccb.wNumIOPorts != 2))
      return (DWORD) -1 ;

   return ccb.wIOPortBase[ 1 ] ;

} // MSSNDSYS_Get_SCSI_Base()

//--------------------------------------------------------------------------
//  
//  CONFIGRET MSSNDSYS_Get_PCMCIA_Config
//  
//  Description:
//  
//  
//  Parameters:
//      DEVNODE dn
//  
//      PWORD pwBaseCODEC
//  
//      PWORD pwBaseOPL3
//  
//      PWORD pwIRQ
//  
//  Return (CONFIGRET):
//  
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_Get_PCMCIA_Config
(
    DEVNODE         dn,
    PWORD           pwBaseCODEC,
    PWORD           pwBaseOPL3,
    PWORD           pwIRQ
)
{
   CONFIGRET            cr ;
   CMCONFIG             ccb ;
   WORD                 wId ;

   //
   // Get our resource allocation from CM.  if this fails, we have no 
   // choice but to fail the CONFIG_START.  We'll return the same
   // error that the CM_Get_Alloc_Config returned.
   //

   if (CR_SUCCESS != (cr = CM_Get_Alloc_Log_Conf( &ccb, dn, 0 )))
      return cr ;

   if (CR_SUCCESS != (cr = MSSNDSYS_Get_PCCard_Info( dn, NULL, &wId )))
      return cr ;


   //
   // Extract the information of interest from the config buffer
   // that we got from CM_Get_Alloc_Config.
   //

   switch (ccb.wNumIOPorts)
   {
      case 1:
      {
         switch (ccb.wIOPortLength[ 0 ])
         {
            case 16:
               *pwBaseCODEC = ccb.wIOPortBase[ 0 ] + 4 ;
               if (wId == PCMCIA_ID_SOUNDSCSI)
                  *pwBaseOPL3 = ccb.wIOPortBase[ 0 ] + 8 ;
               break ;

            default:
               DBG_ERROR( ( "unknown I/O configuration"  ) ) ;
               return CR_FAILURE ;
         }
      }
      break ;

      case 2:
      {
         switch (ccb.wIOPortLength[ 0 ])
         {
            case 4:
               // OPL3 + extended CODEC interface

               *pwBaseCODEC = ccb.wIOPortBase[ 1 ] + 4 ;
               *pwBaseOPL3 = ccb.wIOPortBase[ 0 ] ;
               break ;

            default:
               DBG_ERROR( ( "unknown I/O configuration" ) ) ;
               return CR_FAILURE ;
         }
      }
      break ;

      default:
         DBG_ERROR( ( "can't use this configuration" ) ) ;
         return CR_FAILURE ;
   }

   if (ccb.wNumIRQs < 1)
   {
      //
      // No way we can use this config!
      //

      DBG_ERROR( ( "No IRQ assigned." ) ) ;
      return CR_FAILURE ;
   }

   *pwIRQ = ccb.bIRQRegisters[ 0 ] ;

   if (-1 == *pwBaseCODEC)
      return CR_FAILURE ;

   return CR_SUCCESS ;

} // MSSNDSYS_Get_PCMCIA_Config()

#endif

//--------------------------------------------------------------------------
//  
//  CONFIGRET MSSNDSYS_Config_Handler
//  
//  Description:
//  
//  
//  Parameters:
//      CONFIGFUNC cf
//  
//      SUBCONFIGFUNC scf
//  
//      DEVNODE dn
//  
//      DWORD dwRefData
//  
//      ULONG ulFlags
//  
//  Return (CONFIGRET):
//  
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_Config_Handler
(
    CONFIGFUNC      cf,
    SUBCONFIGFUNC   scf,
    DEVNODE         dn,
    DWORD           dwRefData,
    ULONG           ulFlags
)
{
   //
   // Switch the function (aka command)
   //

   DBG_TRACE( ( "Config_Handler: cf: %lx, scf: %lx, dn: %lx", cf, scf, dn ) ) ;

   switch (cf)
   {

      case CONFIG_FILTER:
      {
         //
         // This command is sent to the devnode and all it's parents
         // to give them a chance to delete logical configurations
         // from the filtered configuration list.  A parent would
         // remove a configuration from the filtered list if it cannot
         // provide that configuration.
         //

         int           i ;
         BOOL          fAutoSel = FALSE ;
         CONFIGRET     cr ;
         CMBUSTYPE     cmbt ;
         DWORD         cbInfo ;
         IRQ_RESOURCE  *pIRQRes ;
         LOG_CONF      lc ;
         RES_DES       rd ;
         RESOURCEID    rid ;
         ULONG         cIORes ;
         WORD          wBaseAutoSel = (WORD) -1 ;

         cIORes = 0 ;
         cbInfo = 0 ; 
         if (CR_SUCCESS != 
            CM_Get_Bus_Info( dn, &cmbt, &cbInfo, NULL, 0 ))
            cmbt = BusType_None ;

         if ((cmbt != BusType_ISA) && (cmbt != BusType_None))
            return CR_SUCCESS ;

         if (CR_SUCCESS != 
               (cr = CM_Get_First_Log_Conf( &lc, dn, FILTERED_LOG_CONF )))
         {
            DBG_ERROR( ( "failed to get FILTERED_LOG_CONF!" ) ) ;
            return cr ;
         }

         while (CR_SUCCESS == cr)
         {
            if (CR_SUCCESS != 
                  CM_Get_Next_Res_Des( &rd, lc, ResType_All, &rid, 0 ))
            {
               DBG_ERROR( ( "filter log conf has no resources!" ) ) ;
               return CR_FAILURE ;
            }

            pIRQRes = NULL ;

            while (CR_SUCCESS == cr)
            {
               switch (rid)
               {
                  case ResType_IRQ:
                     pIRQRes = (IRQ_RESOURCE *) rd ;
                     break ;

                  case ResType_IO:
                     cIORes++ ;

                     if (((PIO_RES) rd) -> IO_Data.IOR_nPorts == 8)
                     {
                        wBaseAutoSel = 
                           ((PIO_RES) rd) -> IO_Data.IOR_Min ;
                        fAutoSel = TRUE ;
                     }
                     break ;

                  default:
                     break ;
               }

               cr = CM_Get_Next_Res_Des( &rd, rd, ResType_All, &rid, 0 ) ;
            }

            if (CR_NO_MORE_RES_DES != cr)
            {
               DBG_ERROR( ( "error processing filter log conf resources" ) ) ;
               return cr ;
            }

            if (!fAutoSel || 
                !pIRQRes ||
                !Is_AutoSelect_Valid( wBaseAutoSel ))
            {
               DBG_ERROR( ( "did not find IRQ des. or valid AutoSelect range" ) ) ;
               return CR_SUCCESS ;
            }

            //
            // First, validate the AutoSelect range, then modify the
            // IRQ resource descriptor with information obtained by
            // poking at the bus...
            //

            DBG_TRACE( ( "have IRQ descriptor and valid auto-select range." ) ) ;

            for (i = 0; i < 16; i++)
            {
               if (pIRQRes -> IRQ_Header.IRQD_Req_Mask & (1 << i))
               {
                  if (!Validate_AutoSel_IRQ( wBaseAutoSel, (WORD) i ))
                     pIRQRes -> IRQ_Header.IRQD_Req_Mask &= (WORD) ~(1 << i) ;
               }
            }

            cr = CM_Get_Next_Log_Conf( &lc, lc, 0 ) ;
         }
         
         return CR_SUCCESS ;

      }
    
      case CONFIG_START:
      {
         //
         // This command is sent to instruct the driver that
         // it can start using the resources that have been
         // allocated to it.   This is the meat of this
         // DriverConfigMan.
         //
         // We get our resource allocation by calling the
         // configuration manager (CM) using the devnode that
         // was passed to us.
         //
         // If all has gone well, we should not have been given
         // a resource allocation that we can't use.   Ideally
         // this command should always succeed.  However, every
         // now and then life throws you a curve ball, so we
         // should still verify that we are happy with the
         // resources we have been given.
         //
         // If we receive multiple _STARTs for multiple devodes,
         // then there are multiple taget devices for us to drive.
         // How we handle this is highly dependent upon the driver.
         // This particular driver can only drive one target device.
         //

         char        szRegKey[ 256 ] ;
         CMBUSTYPE   cmbt ;
         CONFIGRET   cr ;
         WORD        wBaseAGA, wBaseCODEC, wBaseAutoSel, wBaseSB,
                     wBaseOPL3 ;
         WORD        wIRQ ;
#ifdef AZTECH
         WORD        wSBIRQ;
#endif
         WORD        wCaptureDMA, wPlaybackDMA ;
#ifdef AZTECH
         WORD        wSBDMA ;
#endif
         DWORD       cbInfo, fdwOptions = 0, 
                     dwDMABufferSize = DEFAULT_DMABUFFERSIZE ;

         DBG_TRACE( ( "CONFIG_START" ) ) ;

         if (cr = CM_Get_DevNode_Key( dn, NULL, szRegKey,
                                      sizeof( szRegKey ), 
                                      CM_REGISTRY_HARDWARE ))
            return cr ;

         // If the bus type is unavailable (shouldn't be), default
         // to ISA legacy device.

         cbInfo = 0 ; 
         if (CR_SUCCESS != 
            CM_Get_Bus_Info( dn, &cmbt, &cbInfo, NULL, 0 ))
            cmbt = BusType_None ;

         wBaseAutoSel = (WORD) -1 ;
         wBaseAGA = (WORD) -1 ;
         wBaseCODEC = (WORD) -1 ;
         wBaseOPL3 = (WORD) - 1 ;
         wBaseSB = (WORD) -1 ;
                           
         switch (cmbt)
         {
#ifdef MSSNDSYS
            case BusType_PCMCIA:
               fdwOptions |= SSI_FLAG_BUSTYPE_PCMCIA ;
               if (CR_SUCCESS !=
                      (cr = MSSNDSYS_Get_PCMCIA_Config( dn, &wBaseCODEC,
                                                        &wBaseOPL3, &wIRQ )))
                  return cr ;
               break ;
#endif

            case BusType_BIOS:
            case BusType_ISAPNP:
               fdwOptions |=
                  (cmbt == BusType_ISAPNP) ? SSI_FLAG_BUSTYPE_ISAPNP :
                                             SSI_FLAG_BUSTYPE_PNPBIOS ;

               // ...fall-through...
               

            default:
               if (CR_SUCCESS != 
                      (cr = MSSNDSYS_Get_ISA_Config( dn, &wBaseAutoSel,
                                                     &wBaseAGA, &wBaseCODEC,
                                                     &wBaseOPL3, &wBaseSB,
                                                     &wIRQ,
                                                #ifdef AZTECH
                                                      &wSBIRQ, &wPlaybackDMA,
                                                     &wCaptureDMA, &wSBDMA )))
                                                #endif
                                                #ifdef MSSNDSYS
                                                      &wPlaybackDMA, &wCaptureDMA )))
                                                #endif
                  return cr ;
               break ;
         }

         if (CR_SUCCESS != (cr = MSSNDSYS_MigrateRegistry( dn )))
            return cr ;

         //
         // Read device settings from registry
         //

         MSSNDSYS_Get_Registry_Info( dn, &fdwOptions ) ;
#ifdef AZTECH
         //
         // Write Aztech config register these setting
         //
         if (CR_SUCCESS !=
                (cr = AZT_Init_Config( wBaseSB, wBaseAutoSel,
                                       wSBIRQ, wSBDMA )))
            return cr ;
#endif

         //
         // OK, now do whatever else is necessary to put this
         // configuration into effect.
         //

         cr = MSSNDSYS_Set_Config( dn, wBaseCODEC, wBaseAutoSel,
                                   wBaseAGA, wBaseOPL3, wBaseSB,
                                   wIRQ, wPlaybackDMA, wCaptureDMA, 
                                   dwDMABufferSize, fdwOptions ) ;

         if (CR_SUCCESS == cr)
         {
#ifdef MSSNDSYS
            if ((WORD) -1 == wBaseSB)
            {
               //
               // Set up child DevNode for Sound Blaster emulation
               //

               cr = CM_Register_Enumerator( dn, 
                                            MSSNDSYS_Enumerator,
                                            CM_REGISTER_ENUMERATOR_HARDWARE ) ;
            }
            else
            {
               char   szSetting[ 32 ] ;
               int    iBlasterIRQ ;
       
               // We'd rather have I2 than I9 in BLASTER environment string

               iBlasterIRQ = ( (9 == wIRQ) ? 2 : wIRQ ) ;
               _Sprintf( szSetting, "A%03X I%d D%d", wBaseSB, iBlasterIRQ, 
                         wPlaybackDMA ) ;
               MMDEVLDR_SetEnvironmentString( STR_BLASTER, szSetting ) ;
            }

            //
            // On WSS hardware, the entire MIDI section for this device 
            // is non-existant (OPL3 assumption when wBaseOPL3 == -1), 
            // so well let MMDEVLDR know not to try to load the ring 3
            // driver.
            //
            // It is also possible to set the device presence for
            // particular drivers such as: midi\<xxx> where <xxx>
            // is the instance identifier for the devnode.  For
            // most implementations, the driver instance identifier
            // is the driver name.   For example, in a case where
            // the MPU401 and the MSOPL midi drivers could exist
            // but the resources for the MPU401 were not assigned
            // to the devnode, the STR_MIDI_MSOPL would be defined
            // as "midi\\msopl.drv".
            //

            MMDEVLDR_SetDevicePresence( dn, STR_MIDI, 
                                        ((WORD) -1 != wBaseOPL3) ) ;
#endif

#ifdef AZTECH
            char   szSetting[ 32 ] ;
            int    iBlasterIRQ ;
       
            // We'd rather have I2 than I9 in BLASTER environment string

            iBlasterIRQ = ( (9 == wSBIRQ) ? 2 : wSBIRQ ) ;
            _Sprintf( szSetting, "A%03X I%d D%d", wBaseSB, iBlasterIRQ, wSBDMA ) ;
            MMDEVLDR_SetEnvironmentString( STR_BLASTER, szSetting ) ;
#endif
         }

         return cr ;
      }

      case CONFIG_TEST:
       
         //
         // This command is sent to query the driver whether it can
         // change configuration, stop using the resources, or
         // handle a remove.  The query is specified by the sub func.
         //

         switch (scf)
         {
            case CONFIG_TEST_CAN_STOP:

               //
               // This command asks: Can we stop
               // using our resources?  If we're making noise,
               // we'd rather not stop.
               //

               return (MSSNDSYS_IsOwned( dn ) ? CR_FAILURE : CR_SUCCESS) ;

            case CONFIG_TEST_CAN_REMOVE:

               //
               // This command asks: Is it OK for CM to remove
               // the devnode?  This implies that the associated
               // device is also going to disappear (or has
               // already disappeared).
               //
          
               return (MSSNDSYS_IsOwned( dn ) ? CR_FAILURE : CR_SUCCESS) ;

            default:
               return CR_DEFAULT ;
         }

      case CONFIG_APM:
      {
         switch (scf & 0x7FFFFFFFL)
         {
            case CONFIG_APM_TEST_STANDBY_SUCCEEDED:
            case CONFIG_APM_TEST_SUSPEND_SUCCEEDED:
               MSSNDSYS_Suspend( dn ) ;
               break ;

            case CONFIG_APM_RESUME_SUSPEND:
            case CONFIG_APM_RESUME_STANDBY:
            case CONFIG_APM_RESUME_CRITICAL:
               MSSNDSYS_Resume( dn ) ;
               break ;
         }
         return CR_DEFAULT ;
      }

      case CONFIG_REMOVE:
        
         //
         // This command is sent to a devnode and all it's parents when
         // when the devnode is being removed from the hardware tree.
         // Deallocate any per devnode instance data and take whatever
         // other measures are necessary to stop using that devnode and
         // the associated hardware.   The command is essentially saying
         // that the hardware no longer exists in the system.
         //

         // ... fall-through ...

      case CONFIG_STOP:

         // ... fall-through ...

      case CONFIG_SHUTDOWN:
      {
         DWORD  dwFlags ;

         //
         // This command indicates that we should stop
         // using the resources that were allocated to us.
         //

         if (MSSNDSYS_Get_DevNode_Flags( dn, &dwFlags ))
         {
            //
            // remove the BLASTER= line for HW supported emulation...
            // 

            if (dwFlags & SSI_FLAG_HWSB)
               MMDEVLDR_RemoveEnvironmentString( STR_BLASTER ) ;

            MSSNDSYS_Remove_Config( dn ) ;
            return CR_SUCCESS ;
         }
         else
            return CR_DEFAULT ;
      }

      default:
         return CR_DEFAULT ;
   }

} // MSSNDSYS_Config_Handler()

#ifdef MSSNDSYS

//--------------------------------------------------------------------------
//  
//  CONFIGRET MSSNDSYS_Enumerator
//  
//  Description:
//  
//  
//  Parameters:
//      CONFIGFUNC cf
//  
//      SUBCONFIGFUNC scf
//  
//      DEVNODE dnBus
//  
//      DEVNODE dnAbout
//  
//      ULONG ulFlags
//  
//  Return (CONFIGRET):
//--------------------------------------------------------------------------

CONFIGRET MSSNDSYS_Enumerator
(
    CONFIGFUNC      cf,
    SUBCONFIGFUNC   scf,
    DEVNODE         dnBus,
    DEVNODE         dnAbout,
    ULONG           ulFlags
)
{

   switch (cf)
   {
      case CONFIG_ENUMERATE:
      {
         BYTE        bWantSB ;
         UINT        cbLen ;
         CONFIGRET   cr ;
         DEVNODE     dnVirtualSB ;
         DWORD       dwFlags ;

         DBG_TRACE( ( "CONFIG_ENUMERATE" ) ) ;

         //
         // If we a child, then don't re-enumerate...
         //

         if (CR_SUCCESS == CM_Get_Child( &dnVirtualSB, dnBus, 0 ))
            return CR_DEFAULT ;

         //
         // Check registry switch...
         //

         // Is This code missing from the Aztech? or did
         // they remove it? J.R.
         cbLen = sizeof( bWantSB ) ;
         cr = CM_Read_Registry_Value( dnBus,
                                      REGSTR_SUBKEY_CONFIG,
                                      REGSTR_VAL_SBEMULATION,
                                      REG_BINARY,
                                      &bWantSB,
                                      &cbLen,
                                      CM_REGISTRY_SOFTWARE ) ;

         if (CR_SUCCESS != cr)
            bWantSB = DEFAULT_SBEMULATION ;

         if (bWantSB && MSSNDSYS_WantSB( dnBus, &dwFlags ))
         {
            CONFIGRET   cr ;
            DMA_DES     dma ;
            IO_RES      io ;
            LOG_CONF    lc ;
            RES_DES     rd ; 
            
            if (CR_SUCCESS !=
                   (cr = CM_Create_DevNode( &dnVirtualSB, 
                                            DEVIDSTR_WSSSB, dnBus, 0 )))
            {
               DBG_ERROR( ( "failed to create DevNode" ) ) ;
               return cr ;
            }

            if (CR_SUCCESS != 
                   (cr = CM_Add_Empty_Log_Conf( &lc, dnVirtualSB, 
                                                LCPRI_NORMAL, 
                                                BASIC_LOG_CONF )))
            {
               DBG_ERROR( ( "failed to add empty LogConf to DevNode" ) ) ;
               return cr ;
            }

            //
            // Add I/O resources to our new log conf
            //

            io.IO_Header.IOD_Count = 1 ;
            io.IO_Header.IOD_Type = IOType_Range ;
            io.IO_Header.IOD_Alloc_Base = 0 ;
            io.IO_Header.IOD_Alloc_End = 0 ;
            io.IO_Header.IOD_Alloc_Alias = 0 ;
            io.IO_Header.IOD_Alloc_Decode = 0 ;
            io.IO_Header.IOD_DesFlags = 0 ;
            io.IO_Data.IOR_Align = 0xFFF0 ;
            io.IO_Data.IOR_nPorts = 16 ;
            io.IO_Data.IOR_Min = 0x220 ;
            io.IO_Data.IOR_Max = 0x24F ;
            io.IO_Data.IOR_RangeFlags = 0x00 ;
            io.IO_Data.IOR_Alias = 0x00 ;
            io.IO_Data.IOR_Decode = 0x00 ;

            if (CR_SUCCESS != 
                   (cr = CM_Add_Res_Des( &rd, lc,
                                         ResType_IO,
                                         &io,
                                         sizeof( IO_RES ),
                                         0 )))
            {
               DBG_ERROR( ( "failed to add I/O resource to LogConf" ) ) ;
               return cr ;
            }

            if (dwFlags & SBVIRT_FLAG_BUSTYPE_PCMCIA)
            {
               if (dwFlags & SBVIRT_FLAG_VIRT_DMA)
               {
                  dma.DD_Flags = fDD_BYTE ;
                  dma.DD_Alloc_Chan = 0 ;
                  dma.DD_Req_Mask = 0x02 ;   // DMA channel 1 required
                  dma.DD_Reserved = 0 ;

                  if (CR_SUCCESS !=
                        (cr = CM_Add_Res_Des( &rd, lc, ResType_DMA,
                                              &dma, sizeof( DMA_DES ), 0 )))
                  {
                     DBG_ERROR( ( "PCMCIA: unable to add DMA resource to LogConf" ) ) ;
                     return cr ;
                  }
               }
            }

            return CR_SUCCESS ;
         }
         return CR_DEFAULT ;
      }

      default:
         return CR_DEFAULT ;
   }


} // MSSNDSYS_Enumerator()

//--------------------------------------------------------------------------
//  
//  CONFIGRET SBVIRT_Config_Handler
//  
//  Description:
//  
//  
//  Parameters:
//      CONFIGFUNC cf
//  
//      SUBCONFIGFUNC scf
//  
//      DEVNODE dn
//  
//      DWORD dwRefData
//  
//      ULONG ulFlags
//  
//  Return (CONFIGRET):
//--------------------------------------------------------------------------

CONFIGRET SBVIRT_Config_Handler
(
    CONFIGFUNC      cf,
    SUBCONFIGFUNC   scf,
    DEVNODE         dn,
    DWORD           dwRefData,
    ULONG           ulFlags
)
{

   //
   // Switch the function (aka command)
   //

   DBG_TRACE( ( "SBVirt Config_Handler: cf: %lx, scf: %lx, dn: %lx", cf, scf, dn ) ) ;

   switch (cf)
   {
      case CONFIG_FILTER:
         //
         // This command is sent to the devnode and all it's parents
         // to give them a chance to delete logical configurations
         // from the filtered configuration list.  A parent would
         // remove a configuration from the filtered list if it cannot
         // provide that configuration.
         //

         return CR_SUCCESS ;
    
      case CONFIG_START:
      {
         //
         // This command is sent to instruct the driver that
         // it can start using the resources that have been
         // allocated to it.   This is the meat of this
         // DriverConfigMan.
         //
         // We get our resource allocation by calling the
         // configuration manager (CM) using the devnode that
         // was passed to us.
         //
         // If all has gone well, we should not have been given
         // a resource allocation that we can't use.   Ideally
         // this command should always succeed.  However, every
         // now and then life throws you a curve ball, so we
         // should still verify that we are happy with the
         // resources we have been given.
         //
         // If we receive multiple _STARTs for multiple devodes,
         // then there are multiple taget devices for us to drive.
         // How we handle this is highly dependent upon the driver.
         // This particular driver can only drive one target device.
         //

         CONFIGRET   cr ;
         CMCONFIG    ccb ;
         WORD        wBaseSB ;
         WORD        wDMA, wIRQ ;

         //
         // Get our resource allocation from CM.  if
         // this fails, we have no choice but to fail
         // the CONFIG_START.  We'll return the same
         // error that the CM_Get_Alloc_Config returned.
         //
         //

         if (CR_SUCCESS != 
               (cr = CM_Get_Alloc_Log_Conf( &ccb, dn, 0 )))
            return cr ;

         //
         // Extract the information of interest from the config buffer
         // that we got from CM_Get_Alloc_Config.
         //

         wBaseSB = (WORD) -1 ;
         wDMA = (WORD) -1 ;
         wIRQ = (WORD) -1 ;   // filled in by SBVirt_Set_Config()
                  
         if (!ccb.wNumIOPorts)
         {
            DBG_ERROR( ( "SBVIRT: I/O resource failure (no I/O resources!)" ) ) ;
            return CR_FAILURE ;
         }
         wBaseSB = ccb.wIOPortBase[ 0 ] ;

         if (ccb.wNumDMAs)
         {
            wDMA = ccb.bDMALst[ 0 ] ;
         }

         //
         // OK, now do whatever else is necessary to put this
         // configuration into effect.
         //

         cr = SBVirt_Set_Config( dn, wBaseSB, &wDMA, &wIRQ ) ;

         if (CR_SUCCESS == cr)
         {
            char   szSetting[ 32 ] ;
            int    iBlasterIRQ ;

            // We'd rather have I2 than I9 in BLASTER environment string

            iBlasterIRQ = ( (9 == wIRQ) ? 2 : wIRQ ) ;
            _Sprintf( szSetting, "A%03X I%d D%d", wBaseSB, iBlasterIRQ, wDMA ) ;
            MMDEVLDR_SetEnvironmentString( STR_BLASTER, szSetting ) ;
         }

         return cr ;
      }

      case CONFIG_TEST:
       
         //
         // This command is sent to query the driver whether it can
         // change configuration, stop using the resources, or
         // handle a remove.  The query is specified by the sub func.
         //

         switch (scf)
         {
            case CONFIG_TEST_CAN_STOP:

               //
               // This command asks: Can we stop
               // using our resources?  If we're making noise,
               // we'd rather not stop.
               //

               return (SBVirt_IsOwned( dn ) ? CR_FAILURE : CR_SUCCESS) ;

            case CONFIG_TEST_CAN_REMOVE:

               //
               // This command asks: Is it OK for CM to remove
               // the devnode?  This implies that the associated
               // device is also going to disappear (or has
               // already disappeared).
               //
          
               return (SBVirt_IsOwned( dn ) ? CR_FAILURE : CR_SUCCESS) ;

            default:
               return CR_DEFAULT ;
         }

      case CONFIG_REMOVE:

         //
         // This command is sent to a devnode and all it's parents when
         // when the devnode is being removed from the hardware tree.
         // Deallocate any per devnode instance data and take whatever
         // other measures are necessary to stop using that devnode and
         // the associated hardware.   The command is essentially saying
         // that the hardware no longer exists in the system.
         //

         // ... fall-through ...

      case CONFIG_STOP:
         //
         // This command indicates that we should stop
         // using the resources that were allocated to us.
         //

         SBVirt_Remove_Config( dn ) ;
         MMDEVLDR_RemoveEnvironmentString( STR_BLASTER ) ;
         return CR_SUCCESS ;

      default:
         return CR_DEFAULT ;
   }

} // SBVIRT_Config_Handler()

#endif

//---------------------------------------------------------------------------
//  End of File: cnfgmgr.c
//---------------------------------------------------------------------------
