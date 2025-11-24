//---------------------------------------------------------------------------
//
//  Module:   midiout.c
//
//  Description:
//     MSMPU401 Non-fixed routines
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

#include "msmpu401.h"

//--------------------------------------------------------------------------
//  
//  void midSendPartBuffer
//  
//  Description:
//      This function is called from midStop().  It looks at the buffer
//      at the head of the queue and if it contains any data marks it
//      as done and sends it back to the client.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL midSendPartBuffer
(
    PMIDIINCLIENT      pmic
)
{
    LPMIDIHDR lpH;

    if ( pmic -> lpmhQueue && pmic -> dwCurData )
    {
        lpH = pmic -> lpmhQueue;
        pmic -> lpmhQueue = pmic -> lpmhQueue->lpNext;

        lpH->dwFlags |= MHDR_DONE;
        lpH->dwFlags &= ~MHDR_INQUEUE;
	lpH->dwBytesRecorded = pmic->dwCurData;
        pmic -> dwCurData = 0L;
	
        midiCallback( (PPORTALLOC) pmic, 
                      MIM_LONGERROR, (DWORD)lpH, pmic -> dwMsgTime ) ;
    }
} // midSendPartBuffer()

//--------------------------------------------------------------------------
//  
//  void modGetDevCaps
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      MDEVICECAPSEX FAR* lpCaps
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void FAR PASCAL modGetDevCaps
(
    PHARDWAREINSTANCE       phwi,
    MDEVICECAPSEX FAR*      lpCaps
)
{
    MIDIOUTCAPS mc;

    mc.wMid = MM_MICROSOFT;
    mc.wPid = MM_MPU401_MIDIOUT;
    mc.vDriverVersion = DRIVER_VERSION;
    mc.wTechnology = MOD_MIDIPORT;
    mc.wVoices = 0;                     // not used for ports
    mc.wNotes = 0;                      // not used for ports
    mc.wChannelMask = 0xFFFF;           // all channels

    mc.dwSupport = 0L;
   #ifdef MIDI_STREAMS
    mc.dwSupport |= MIDICAPS_STREAM;
   #endif

    LoadString(ghModule, IDS_MPU401MIDIOUT, mc.szPname, MAXPNAMELEN);
    _fmemcpy(lpCaps->pCaps, &mc, min(lpCaps->cbSize, sizeof(MIDIOUTCAPS)));

} // modGetDevCaps()

//---------------------------------------------------------------------------
//  End of File: midiout.c
//---------------------------------------------------------------------------
