/****************************************************************************
 *
 *   initc.c
 * 
 *   LibMain and driver startup routines
 *
 *   Microsoft Video for Windows Sample Capture Driver
 *   Chips & Technologies 9001 based frame grabbers.
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <msvideo.h>
#include <msviddrv.h>
#define VCAP_MAIN       // define this and strings will get defined!!!
#include "ct.h"
#include "debug.h"

/*****************************************************************************

    strings

 ****************************************************************************/ 

extern char gszDriverName[];

//#define BCODE _based(_segname("_CODE"))

// non-localized strings

#ifdef DEBUG
    static char BCODE gszDebug[]    = "MMDebug";
#endif


static WORD FAR PASCAL GetWindowsVersionCorrectly()
{
    WORD w;

    w = LOWORD( GetVersion() );

    return (w << 8) | (w >> 8);
}

void FAR PASCAL HardErrorMsgBox( WORD wStringId )
{
    char    szPname[MAXPNAMELEN];
    char    szErrorBuffer[MAX_ERR_STRING]; // buffer for error messages

    //  Starting with Windows 3.1, it is ok to bring up a _hard system modal_
    //  message box during LibInit.  In Windows 3.0, this will not work!
    if ( GetWindowsVersionCorrectly() >= 0x30A )
    {
        LoadString(ghModule, IDS_VCAPPRODUCT, szPname, MAXPNAMELEN);
        LoadString(ghModule, wStringId, szErrorBuffer, sizeof(szErrorBuffer));
	MessageBox(NULL, szErrorBuffer, szPname,
#ifdef BIDI
		MB_RTL_READING |
#endif
    MB_OK|MB_SYSTEMMODAL|MB_ICONHAND);
    }
}


//
// Copy Hardware INI values to globals
// Returns 1
//
int NEAR PASCAL InitSetConfiguration(LPDEVICE_INIT lpDI)
{
    gwBaseReg           = lpDI->wIOBase;
    gbInt               = lpDI->bInterrupt;
    wPCVideoAddress     = lpDI->wIOBase;
    return 1;
}

//
// Checks that values in DEVICE_INIT are legal.  
// Ideally, this would also do a non-destructive test for hardware.
// Returns: 1 if OK, 0 if settings are bad.
//
int FAR PASCAL InitVerifyConfiguration(LPDEVICE_INIT lpDI)
{
    if (!ConfigCheckAllDeviceInitParms (lpDI))
        return 0;

    return 1;
}

//
// Main entry point to initialize the device.  Call once only.
// Returns 1 if success, 0 if failure
//
int FAR PASCAL HardwareInit (LPDEVICE_INIT lpDI)
{
    BOOL bOK = FALSE;
    
    if (CT_Init ()) {
        CT_SetFrameAddress ((int) (lpDI-> wSegment)); // wants 1-15
        gbInt = (BYTE) CT_SetIRQUsed ((int) lpDI-> bInterrupt); 
        bOK = TRUE;
    }
    return bOK;
}

//
// Deinitialize the device. Call once only.
// Returns 1 if success, 0 if failure
//
void FAR PASCAL HardwareFini ()
{
    CT_Fini ();
}

//
// Library initialization code.
// Parameters:
//      Our module handle.
//      The heap size from the .def file.
//      The command line.
// Returns:
//      Returns 1 if the initialization was successful and 0 otherwise.
//
int NEAR PASCAL LibMain(HANDLE hModule, WORD wHeapSize, LPSTR lpCmdLine)
{
    DEVICE_INIT devInit;

    D1("LibMain");

#ifdef DEBUG
    // get debug level - default is 0
    wDebugLevel = GetProfileInt(gszDebug, gszDriverName, 0);
#endif

    // save our module handle
    ghModule = hModule;

    /* Set Driver entry (usage) count to NULL */
    gwDriverUsage = 0;
    gwCaptureUsage = 0;
    gwDisplayUsage = 0;
    gwVideoInUsage = 0;
    gwVideoOutUsage = 0;

    // read system.ini and get the board configuration information.
    GetHardwareSettingsFromINI(&devInit);

    // verify address and IRQ
    if ( ! (InitVerifyConfiguration (&devInit) ) )
    {
        D1("hardware is not properly configured!");
//      HardErrorMsgBox( IDS_ERRBADPORT );
    }
    else
    {
        // set the configuration (IRQ and Frame memory address)
        InitSetConfiguration( &devInit );
    }
    // always succeed load! (if config error, we will not Enable...).
    return ( 1 );
}


//
// Returns DV_ERR_OK if frame memory is writable
//
DWORD FAR PASCAL InitCheckMem( void )
{
    int j;
    LPSTR lpc;

    if ((GetWinFlags() & WF_ENHANCED) == 0)
        return DV_ERR_PROTECT_ONLY;       // Only run in enhanced mode

      // Why doesn't this work?
//    if (IsBadHugeReadPtr (glpFrameBuffer, 1024L * 1023L))
//        return DV_ERR_MEM_CONFLICT;       // Check to make sure the pointer is valid

      // Not all the bits are returned when writing data, so
      // we must mask the returned data

      lpc = glpFrameBuffer;
      for (j = 0; j < 100; j++, lpc++) {
        *lpc = 0x0a;
        if ((*lpc & 0x0a) != 0x0a)
          return DV_ERR_MEM_CONFLICT;
      }

      return DV_ERR_OK;
}
