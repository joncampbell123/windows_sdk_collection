#include "types.r"
#include "balloons.r"
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
    "Multipad"
  }
};
#endif

resource 'BNDL' (1)
{
	'CPPD',	        /* signature */
	0,              /* version id */
	{
		'ICN#',
		{
			0, IDI_ICON;
		};
		'FREF',
		{
			0, IDI_ICON;
		};
	};
};

type 'CPPD' as 'STR ';

resource 'CPPD' (0)
{
	"CPPDEMO Sample Application. Copyright Microsoft Corp. 1994-1995"
};

resource 'FREF' (IDI_ICON)
{
	'APPL', 0, "";
};


/* Balloon help resources */

resource 'hfdr' (-5696)
{
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
		HMSTRResItem {500}
	}
};

resource 'hovr' (1000)
{
    HelpMgrVersion, hmDefaultOptions, 0, 0,

        HMStringItem    /* missing items override */
        {
            "Miscellaneous part of the Microsoft Multipad "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft Multipad "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
            {
                "Click here to Zoom In or Zoom Out of the bitmap."
            },
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft Multipad"
                "Application. It may be part of the Apple"
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 ODBC CPPDEMO sample application "
	"ported to the Power Macintosh using Microsoft Visual C++ "
	"Edition for Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 ODBC CPPDEMO sample application "
	"ported to the Macintosh using Microsoft Visual C++ Edition "
	"for Macintosh"
};
#endif

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
