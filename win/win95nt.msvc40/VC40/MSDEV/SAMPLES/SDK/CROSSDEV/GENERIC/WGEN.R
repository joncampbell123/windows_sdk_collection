
//-----------------------------------------------------------------------------
// This is a part of the Microsoft Source Code Samples. 
// Copyright (C) 1993-1995 Microsoft Corporation.
// All rights reserved. 
//  
// This source code is only intended as a supplement to 
// Microsoft Development Tools and/or WinHelp documentation.
// See these sources for detailed information regarding the 
// Microsoft samples programs.
//-----------------------------------------------------------------------------


#include "mrc\types.r"
#include "mrc\balloons.r"
#include "ftab.r"
#include "resource.h"
#ifdef _MPPC_
#include "CodeFrag.r"

resource 'cfrg' (0) {
  {
    kPowerPC,
    kFullLib,
    kNoVersionNum,kNoVersionNum,
    0, 0,
    kIsApp,kOnDiskFlat,kZeroOffset,kWholeFork,
    ""
  }
};
#endif

/* our WLM generic application bundle - just the application's icon and
   the version string */

resource 'BNDL' (128)
{
	'WGEN',	/* signature */
	0,		/* version id */
	{
		'ICN#',
		{
			0, IDR_GENERIC;
		},
		'FREF',
		{
			0, IDR_GENERIC;
		};
	};
};

type 'WGEN' as 'STR ';
resource 'WGEN' (0)
{
	"Generic (WLM Sample App) Copyright 1995 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL', 0, "";
};


/* Balloon help resources */

resource 'hfdr' (-5696)
{
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
		HMSTRResItem { 512 }
	}
};

#ifdef _MPPC_
resource 'STR ' (512)
{
	"This is the Windows32 Generic sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (512)
{
	"This is the Windows32 Generic sample application "
	"ported to the Macintosh using Microsoft VC++ Edition "
	"for the Apple Macintosh"
};
#endif

/*	SIZE resource */

resource 'SIZE' (-1)
{
	reserved,
	acceptSuspendResumeEvents,
	reserved,
	canBackground,
	doesActivateOnFGSwitch,
	backgroundAndForeground,
	dontGetFrontClicks,
	ignoreAppDiedEvents,
	is32BitCompatible,
	isHighLevelEventAware,
	localAndRemoteHLEvents,
	notStationeryAware,
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
#ifdef _MPPC_
	2500 * 1024,
	2500 * 1024
#else	// 68K Mac
#ifdef _DEBUG
	3000 * 1024,
	3000 * 1024
#else
	2000 * 1024,
	2000 * 1024
#endif
#endif
};
