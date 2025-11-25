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

resource 'BNDL' (1)
{
	'MDIX',	        /* signature */
	0,              /* version id */
	{
		'ICN#',
		{
			0, IDI_APP;
		};
		'FREF',
		{
			0, 10000;
		};
	};
};

type 'MDIX' as 'STR ';

resource 'MDIX' (0)
{
	"MDI_ Sample Application. Copyright Microsoft Corp. 1994-1995"
};

resource 'FREF' (10000)
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
            "Miscellaneous part of the Microsoft MDI_ "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft MDI_ "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
            {
                "Click here to Zoom In or Zoom Out of the bitmap."
            },
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft MDI_"
                "Application. It may be part of the Apple"
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Windows32 MDI_ sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Windows32 MDI_ sample application "
	"ported to the Macintosh using Microsoft VC++ Edition "
	"for the Apple Macintosh"
};
#endif

resource 'SIZE' (-1)
{
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,				/* we can background; we don't currently, but our sleep value */
								/* guarantees we don't hog the Mac while we are in the background */
	multiFinderAware,			/* this says we do our own activate/deactivate; don't fake us out */
	backgroundAndForeground,	/* this is definitely not a background-only application! */
	dontGetFrontClicks,			/* change this is if you want "do first click" behavior like the Finder */
	ignoreChildDiedEvents,		/* essentially, I'm not a debugger (sub-launching) */
	is32BitCompatible,			/* this app should not be run in 32-bit address space */
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
