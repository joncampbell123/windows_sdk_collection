//---------------------------------------------------------------------------
//
//  Module:   proppage.c
//
//  Description:
//      Property page handling code.  Please note that the
//      property sheet that is displayed is the dynamic
//      based on whether the Sound Blaster device is
//      software emulated or it is available in hardware.
//
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1995 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>

// stop compiler from griping about in-line

#pragma warning (disable:4704)

#define  Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include "config.h"
#define NOSTR
#include "mssndsys.h"
#include "driver.h"
#include "mixer.h"

#include <verinfo.h>
#include <wsshlpid.h>

const static DWORD aHelpIds[] = {  // Context Help IDs
    IDC_DRVPROP_EMULATION_GRP,      IDH_COMM_GROUPBOX,
    IDC_DRVPROP_SINGLEMODEDMA,      IDH_WINDOWS_SNDSYS_DMA,       
    IDC_DRVPROP_ACCEPTCLOSERATES,   IDH_WINDOWS_SNDSYS_SAMPLE,    
    IDC_DRVPROP_ADVANCED_GRP,       IDH_COMM_GROUPBOX,
    IDC_DRVPROP_ENABLESB,           IDH_WINDOWS_SNDSYS_SNDBLASTER,
    0, 0
};

static const char cszHelpFile[] = "MMDRV.HLP";

static char BCODE gszRegKeyConfig[]           = REGSTR_SUBKEY_CONFIG;
static char BCODE gszRegValSingleModeDMA[]    = REGSTR_VAL_SINGLEMODEDMA ;
static char BCODE gszRegValAcceptCloseRates[] = REGSTR_VAL_ACCEPTCLOSERATES ;
static char BCODE gszRegValSoundBlasterEmulation[] = REGSTR_VAL_SBEMULATION ;
static char BCODE gszRegValDo11kHzMCE[]       = REGSTR_VAL_DO11KHZMCE ;

extern char gszConfig[] ;

typedef RETERR (WINAPI *FNDICHANGESTATE)( LPDEVICE_INFO lpdi, DWORD dwStateChange, DWORD dwFlags, LPARAM lParam ) ;

//--------------------------------------------------------------------------
//  
//  VOID DrvLoadUserConfig
//  
//  Description:
//      Reads the user configurable settings from the registry
//  
//  Parameters:
//
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID NEAR DrvLoadUserConfig
(
    DEVNODE         dnDevNode,
    LPDRVDATA       pDrvData
)
{

   BYTE       b ;
   DWORD      cbLen ;
   CONFIGRET  cr ;

   //
   // Accept close sampling rates
   //

   if (NULL != pDrvData -> lpdi)
      dnDevNode = pDrvData -> lpdi -> dnDevnode ;

   cbLen = sizeof( b ) ;
   cr = CM_Read_Registry_Value( dnDevNode,
                                gszRegKeyConfig,
                                gszRegValAcceptCloseRates,
                                REG_BINARY,
                                &b,
                                &cbLen,
                                CM_REGISTRY_SOFTWARE ) ;
   if (CR_SUCCESS != cr)
      b = DEFAULT_ACCEPTCLOSERATES ;

   pDrvData -> fAcceptCloseRates = (b != 0) ;
   
   //
   // Single Mode DMA
   //

   cbLen = sizeof( b ) ;
   cr = CM_Read_Registry_Value( dnDevNode,
                                gszRegKeyConfig,
                                gszRegValSingleModeDMA,
                                REG_BINARY,
                                &b,
                                &cbLen,
                                CM_REGISTRY_SOFTWARE ) ;
   if (CR_SUCCESS != cr)
      b = DEFAULT_SINGLEMODEDMA ;

   pDrvData -> fSingleModeDMA = (b != 0) ;

   //
   // Sound Blaster emulation
   //

   if (pDrvData -> pszTemplate == IDD_PROPDLG_SB)
   {
      cbLen = sizeof( b ) ;
      cr = CM_Read_Registry_Value( dnDevNode,
                                 gszRegKeyConfig,
                                 gszRegValSoundBlasterEmulation,
                                 REG_BINARY,
                                 &b,
                                 &cbLen,
                                 CM_REGISTRY_SOFTWARE ) ;
      if (CR_SUCCESS != cr)
         b = DEFAULT_SBEMULATION ;

      pDrvData -> fSBEmulation = (b != 0) ;
   }

   //
   // Do extra MCE for 11KHz sample rates
   //

   cbLen = sizeof( b ) ;
   cr = CM_Read_Registry_Value( dnDevNode,
                                gszRegKeyConfig,
                                gszRegValDo11kHzMCE,
                                REG_BINARY,
                                &b,
                                &cbLen,
                                CM_REGISTRY_SOFTWARE ) ;
   if (CR_SUCCESS != cr)
       b = DEFAULT_DO11KHZMCE ;

   pDrvData -> fDo11kHzMCE = (b != 0) ;
   
   return ;

} // DrvLoadUserConfig()

//--------------------------------------------------------------------------
//  
//  VOID DrvSaveUserConfig
//  
//  Description:
//     Writes the user configurable settings to the registry
//  
//  Parameters:
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID NEAR DrvSaveUserConfig
(
    PDRVDATA        pDrvData
)
{
    BYTE b ;
    
    //
    //  Save fAcceptCloseRates
    //

    b = pDrvData -> fAcceptCloseRates ;
    CM_Write_Registry_Value( pDrvData -> lpdi -> dnDevnode,
                             gszRegKeyConfig,
                             gszRegValAcceptCloseRates,
                             REG_BINARY,
                             &b,
                             sizeof( b ),
                             CM_REGISTRY_SOFTWARE ) ;

    //
    //  Save fSingleModeDMA
    //

    b = pDrvData -> fSingleModeDMA ;

    CM_Write_Registry_Value( pDrvData -> lpdi -> dnDevnode,
                             gszRegKeyConfig,
                             gszRegValSingleModeDMA,
                             REG_BINARY,
                             &b,
                             sizeof( b ),
                             CM_REGISTRY_SOFTWARE ) ;

    //
    //  Save fSBEmulation
    //

    if (pDrvData -> pszTemplate == IDD_PROPDLG_SB)
    {
      b = pDrvData -> fSBEmulation ;

      CM_Write_Registry_Value( pDrvData -> lpdi -> dnDevnode,
                              gszRegKeyConfig,
                              gszRegValSoundBlasterEmulation,
                              REG_BINARY,
                              &b,
                              sizeof( b ),
                              CM_REGISTRY_SOFTWARE ) ;
    }

    //
    //  Save 11kHz MCE stuff
    //

    b = pDrvData -> fDo11kHzMCE ;

    CM_Write_Registry_Value( pDrvData -> lpdi -> dnDevnode,
                             gszRegKeyConfig,
                             gszRegValDo11kHzMCE,
                             REG_BINARY,
                             &b,
                             sizeof( b ),
                             CM_REGISTRY_SOFTWARE ) ;

    return ;

} // DrvSaveUserConfig()

//--------------------------------------------------------------------------
//  
//  VOID InitDrvPropertiesDlg
//  
//  Description:
//  
//  
//  Parameters:
//      HWND hDlg
//  
//      PDRVDATA pDrvData
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID NEAR InitDrvPropertiesDlg
(
    HWND            hDlg,
    PDRVDATA        pDrvData
)
{
   DrvLoadUserConfig( pDrvData -> lpdi -> dnDevnode, pDrvData ) ;
   
   SendDlgItemMessage( hDlg, IDC_DRVPROP_ACCEPTCLOSERATES,
                       BM_SETCHECK, pDrvData -> fAcceptCloseRates, 0L ) ;
   SendDlgItemMessage( hDlg, IDC_DRVPROP_SINGLEMODEDMA,
                       BM_SETCHECK, pDrvData -> fSingleModeDMA, 0L ) ;

   if (pDrvData -> pszTemplate == IDD_PROPDLG_SB)
      SendDlgItemMessage( hDlg, IDC_DRVPROP_ENABLESB,
                          BM_SETCHECK, pDrvData -> fSBEmulation, 0L ) ;

} // InitDrvPropertiesDlg()

//--------------------------------------------------------------------------
//  
//  VOID ApplyDrvPropertiesDlg
//  
//  Description:
//  
//  
//  Parameters:
//      HWND hDlg
//  
//      PDRVDATA pDrvData
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID NEAR ApplyDrvPropertiesDlg
(
    HWND            hDlg,
    PDRVDATA        pDrvData
)
{
   BOOL  fChanged, fSetting ;

   fChanged = FALSE ;


   fSetting =
      (BOOL) SendDlgItemMessage( hDlg, IDC_DRVPROP_ACCEPTCLOSERATES,
                                 BM_GETCHECK, 0, 0L );
   if (pDrvData -> fAcceptCloseRates != fSetting)
   {
      fChanged = TRUE ;
      pDrvData -> fAcceptCloseRates = fSetting ;
   }

   fSetting =
      (BOOL) SendDlgItemMessage( hDlg, IDC_DRVPROP_SINGLEMODEDMA,
                                 BM_GETCHECK, 0, 0L ) ;

   if (pDrvData -> fSingleModeDMA != fSetting)
   {
      fChanged = TRUE ;
      pDrvData -> fSingleModeDMA = fSetting ;
   }

   if (pDrvData -> pszTemplate == IDD_PROPDLG_SB)
   {
      fSetting =
         (BOOL) SendDlgItemMessage( hDlg, IDC_DRVPROP_ENABLESB,
                                    BM_GETCHECK, 0, 0L ) ;

      if (pDrvData -> fSBEmulation != fSetting)
      {
         fChanged = TRUE ;
         pDrvData -> fSBEmulation = fSetting ;
      }
   }

   if (fChanged)
   {
      HINSTANCE        hMod ;
      FNDICHANGESTATE  pDiChangeState ;
      UINT             uLastErrorMode ;

      //
      // restart the device with the new configuration settings
      // 

      DrvSaveUserConfig( pDrvData ) ;

      uLastErrorMode = SetErrorMode( SEM_NOOPENFILEERRORBOX ) ;
      if (HINSTANCE_ERROR < (hMod = LoadLibrary( "SETUPX.DLL" )))
      {
         pDiChangeState = 
            (FNDICHANGESTATE) GetProcAddress( hMod, "DiChangeState" ) ;
         if (pDiChangeState)
            pDiChangeState( pDrvData -> lpdi, DICS_PROPCHANGE, 
                            DICS_FLAG_GLOBAL, 0 ) ;
         FreeLibrary( hMod ) ;
      }
      SetErrorMode( uLastErrorMode) ;
   }

} // ApplyDrvPropertiesDlg()

//--------------------------------------------------------------------------
//  
//  BOOL DrvPropertiesDlgProc
//  
//  Description:
//  
//  
//  Parameters:
//      HWND hDlg
//  
//      UINT uMsg
//  
//      WPARAM wParam
//  
//      LPARAM lParam
//  
//  Return (BOOL):
//  
//--------------------------------------------------------------------------

BOOL CALLBACK _loadds DrvPropertiesDlgProc
(
    HWND            hDlg,
    UINT            uMsg,
    WPARAM          wParam,
    LPARAM          lParam
)
{
   PDRVDATA  pDrvData ;

   pDrvData = (PDRVDATA) GetWindowLong( hDlg, DWL_USER ) ;

   switch(uMsg)
   {
      case WM_INITDIALOG:
         pDrvData = (PDRVDATA) (((LPPROPSHEETPAGE) lParam) -> lParam) ;
         SetWindowLong( hDlg, DWL_USER, MAKELONG( pDrvData, 0 ) ) ;
         InitDrvPropertiesDlg( hDlg, pDrvData ) ;
         break;
         
      case WM_DESTROY:
         LocalFree( (HLOCAL) pDrvData ) ;
         break ;

      case WM_COMMAND:
      {
         switch (wParam)
         {
            case IDC_DRVPROP_SETDEFAULTS:
               SendDlgItemMessage( hDlg, IDC_DRVPROP_ACCEPTCLOSERATES,
                                   BM_SETCHECK,
                                   DEFAULT_ACCEPTCLOSERATES, 0L ) ;
               SendDlgItemMessage( hDlg, IDC_DRVPROP_SINGLEMODEDMA,
                                   BM_SETCHECK,
                                   DEFAULT_SINGLEMODEDMA, 0L ) ;
               SendDlgItemMessage( hDlg, IDC_DRVPROP_ENABLESB,
                                   BM_SETCHECK,
                                   DEFAULT_SBEMULATION, 0L ) ;
               break ;
         }
         break ;
      }
      
      case WM_NOTIFY:
         switch (((NMHDR FAR *)lParam)->code)
         {
            case PSN_APPLY:
               ApplyDrvPropertiesDlg( hDlg, pDrvData ) ;
               break ;
         }
         break ;

      case WM_CONTEXTMENU:
         WinHelp( (HWND) wParam, (LPSTR) cszHelpFile, HELP_CONTEXTMENU,
                  (DWORD) (LPSTR) aHelpIds ) ;
         return TRUE ;

      case WM_HELP:
      {
         LPHELPINFO lphi = (LPVOID) lParam;
         WinHelp( lphi -> hItemHandle, (LPSTR)cszHelpFile, HELP_WM_HELP,
                  (DWORD) (LPSTR) aHelpIds ) ;
         return TRUE ;
      }
      
      default:
         return FALSE ;
   }
   return TRUE ;

} // DrvPropertiesDlgProc()

//--------------------------------------------------------------------------
//  
//  BOOL DrvEnumPropPages
//  
//  Description:
//  
//  
//  Parameters:
//      LPDEVICE_INFO lpdi
//  
//      LPFNADDPROPSHEETPAGE pfn
//  
//      LPARAM lParam
//  
//  Return (BOOL):
//
//  
//--------------------------------------------------------------------------

BOOL WINAPI _loadds DrvEnumPropPages
(
    LPDEVICE_INFO           lpdi,
    LPFNADDPROPSHEETPAGE    pfn,
    LPARAM                  lParam
)
{
   PDRVDATA           pDrvData ;
   PROPSHEETPAGE      psp ;
   HPROPSHEETPAGE     hpage ;
   PHARDWAREINSTANCE  phwi ;

   if (NULL == (phwi = DevNodeToHardwareInstance( lpdi -> dnDevnode )))
      return FALSE ;

   //  Allocate a per-instance dialog data structure.

   if (NULL != 
         (pDrvData = (PDRVDATA) LocalAlloc( LPTR, sizeof( DRVDATA ) )))
   {

      psp.dwSize = sizeof( psp ) ;
      psp.dwFlags = PSP_DEFAULT ;
      psp.hInstance = ghModule ;

      //
      // Only use SB emulation settings when hardware 
      // is not present (because we virtualize a Sound Blaster
      // in this case).
      //

      if (phwi -> wFlags & SSI_FLAG_HWSB)
         psp.pszTemplate = IDD_PROPDLG_NOSB ;
      else
         psp.pszTemplate = IDD_PROPDLG_SB ;
      psp.pfnDlgProc = DrvPropertiesDlgProc;
      psp.lParam = MAKELPARAM( pDrvData, 0 ) ;

      pDrvData -> lpdi = lpdi ;
      pDrvData -> pszTemplate = psp.pszTemplate ;

      hpage = CreatePropertySheetPage( &psp ) ;
      if (hpage)
      {
         if (!pfn(hpage, lParam))
            DestroyPropertySheetPage(hpage) ;
      }
   }
   return( TRUE ) ;

} // DrvEnumPropPages()

//---------------------------------------------------------------------------
//  End of File: proppage.c
//---------------------------------------------------------------------------

