/****************************************************************************
 *
 *   midiout.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

/*****************************************************************************
 * @doc INTERNAL
 *
 * @api void | modGetDevCaps | Get the capabilities of the port.
 *
 * @parm LPBYTE | lpCaps | Far pointer to a MIDIOUTCAPS structure.
 *
 * @parm WORD | wSize | Size of the MIDIOUTCAPS structure.
 *
 * @rdesc There is no return value.
 ****************************************************************************/
void FAR PASCAL modGetDevCaps(LPBYTE lpCaps, WORD wSize)
{
MIDIOUTCAPS mc;

    mc.wMid = MM_MICROSOFT;
    mc.wPid = MM_SNDBLST_MIDIOUT;
    mc.vDriverVersion = DRIVER_VERSION;
    mc.wTechnology = MOD_MIDIPORT;
    mc.wVoices = 0;                   /* not used for ports */
    mc.wNotes = 0;                    /* not used for ports */
    mc.wChannelMask = 0xFFFF;         /* all channels */
    mc.dwSupport = 0L;
    LoadString(ghModule, IDS_SNDBLSTMIDIOUT, mc.szPname, MAXPNAMELEN);

    MemCopy(lpCaps, &mc, min(wSize,sizeof(mc)));
}
