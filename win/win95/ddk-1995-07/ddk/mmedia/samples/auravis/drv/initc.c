//////////////////////////////////////////////////////////////////////////
//	INITC.C																					//
//																								//
//	This module contains the LibMain and driver startup routines.			//
//																								//
//	For the AuraVision video capture driver AVCAPT.DRV.						//
//																								//
//////////////////////////////////////////////////////////////////////////
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

#define  VCAP_MAIN	// Define this and strings will get defined.

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

#define _INC_MMDEBUG_CODE_ TRUE // For assert and logging
#include "mmdebug.h"

//******************************************Added by GT 7/15/94

#if 0
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
    char STR_PROLOGUE[] = "AVCAPT.DRV: " ;
    char STR_CRLF[]     = "\r\n" ;
    char STR_SPACE[]    = " " ;
#endif
#endif

//*****************************************GT end


//////////////////////////////////////////////////////////////////////////
//	HardwareInit(lpDI)						//
//									//
//	Main entry point to initialize the device.  Call once only.	//
//	Returns 1 if success, 0 if failure.				//
//////////////////////////////////////////////////////////////////////////

int FAR PASCAL HardwareInit(LPDEVICE_INIT lpDI)
	{
	BOOL	bOK = FALSE;

        AuxDebugEx (2, DEBUGLINE "HardwareInit\r\n");

        HW_LoadConfiguration (NULL);

        // Because this driver uses another support DLL which accesses
        // the hardware directly, we need to inform it of the resources
        // to use by setting these into a separate config file.
        // Ideally, this step wouldn't be necessary...

        HW_SetPortAddress( (int) lpDI -> wIOBase ) ;
        gbInt = HW_SetIRQUsed(lpDI-> bInterrupt);
        HW_SetFrameAddress (lpDI -> wSegment, lpDI -> wSelector );
        HW_SaveConfiguration (NULL);

	if (HW_Init())
		{
                gbInt = HW_SetIRQUsed(lpDI-> bInterrupt);
                HW_SetFrameAddress (lpDI -> wSegment, lpDI -> wSelector );
                HW_SaveConfiguration (NULL);
		bOK = TRUE;
		}
        else
                {
                AuxDebugEx (2, DEBUGLINE "HardwareInit Failed!\r\n");
                }
	return bOK;
	}


//////////////////////////////////////////////////////////////////////////
//	HardwareFini()							//
//									//
//	Deinitialize the device. Call once only.			//
//	Returns 1 if success, 0 if failure.				//
//////////////////////////////////////////////////////////////////////////

void FAR PASCAL HardwareFini()
	{
        AuxDebugEx (2, DEBUGLINE "HardwareFini\r\n");
	HW_Fini();
	}


//////////////////////////////////////////////////////////////////////////
//	LibMain(hModule, uDataSeg, uHeapSize, lpCmdLine)
//	
//	Library initialization code for the driver DLL.
//
//	Parameters:
//      Our module handle.
//		  Data Segment
//      The heap size from the .def file.
//      The command line.
//	
//      Returns 1 if the initialization was successful and 0 otherwise.
//////////////////////////////////////////////////////////////////////////

int FAR PASCAL LibMain(HMODULE hModule, UINT uDataSeg,UINT uHeapSize,
			 LPCSTR lpCmdLine)
{

	// Save our module handle
	ghModule = hModule;

      #if defined DEBUG || defined DEBUG_RETAIL
        DebugSetOutputLevel (GetProfileInt ("Debug", "AVCapt", 0));
        AuxDebugEx (1, DEBUGLINE "LibMain: hModule=%x, DataSeg=%x, HeapSize=%x\r\n", 
                hModule, uDataSeg, uHeapSize);
      #endif

	/* Set Driver entry (usage) count to NULL */
	gwDriverUsage   = 0;
	gwCaptureUsage  = 0;
	gwDisplayUsage  = 0;
	gwVideoInUsage  = 0;
	gwVideoOutUsage = 0;

	// Get the board configuration information.
	
        gpVxDEntry = GetVxDEntry((UINT) AVVXP500_Device_ID ) ;
        AuxDebugEx (1, DEBUGLINE "VxDEntry Pointer is %X\r\n", gpVxDEntry);

        if (gpVxDEntry)
        {
            GetVxDInfo( gpVxDEntry, &devInit );  //devInit initialized here!
            AuxDebugEx (1, DEBUGLINE "AVCAPT LibMain: IO=%X, IRQ=%d, Segment=%X, Selector=%X, cAcquire=%X\r\n", 
                devInit.wIOBase, 
                (int) devInit.bInterrupt, 
                devInit.wSegment, 
                devInit.wSelector, 
                devInit.cAcquire);
	}
	// Always succeed load! (if config error, we will not Enable...).
	return ( 1 );
}

//////////////////////////////////////////////////////////////////////////
int FAR PASCAL WEP(int bSystemExit)
        {
        return 1;
        }
