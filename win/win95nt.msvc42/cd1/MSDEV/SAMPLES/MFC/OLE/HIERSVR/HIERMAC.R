#include        "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include        "ftab.r"
#include        "mrc\types.r"
#include        "mrc\balloons.r"
#include        "systypes.r"

/////////////////////////////////////////////////////////////////////////
// MFC resources

#include        "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// Code Fragment resources

#include        "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// hiersvr: Macintosh-specific resources

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
	isStationeryAware,
	useTextEditServices,
	reserved,
	reserved,
	reserved,
#ifdef _MPPC_
	3000 * 1024,
	3000 * 1024
#else   // 68K Mac
#ifdef _DEBUG
	3000 * 1024,
	3000 * 1024
#else
	2000 * 1024,
	2000 * 1024
#endif
#endif
};


resource 'BNDL' (128, purgeable)    // hiersvr bundle resource ID
{
	'HIER',                 // hiersvr signature
	0,                      // resource ID of signature resource:
							// should be 0
	{
		'ICN#',             // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, IDR_MAINFRAME,
			1, IDR_HIERSVRTYPE,
			2, 130
		},
		'FREF',             // local resource IDs for 'FREF's
		{
			0, 128,
			1, 129,
			2, 130
		}
	}
};

resource 'FREF' (128, purgeable)    // hiersvr application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)    // hiersvr document
{
	'HIER', 1,
	""
};

resource 'FREF' (130, purgeable)    // hiersvr document
{
	'sIER', 2,
	""
};

type 'HIER' as 'STR ';

resource 'HIER' (0, purgeable)
{
	"HierSvr Application"
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
	    "Miscellaneous part of the Microsoft HierSvr "
	    "Sample Application."
	},
	{
	    HMSkipItem {},  /* title bar */
	    HMSkipItem {},  /* reserved. always skip item here */
	    HMStringItem    /* close box */
	    {
		"Click here to close the Microsoft HierSvr "
		"Sample Application."
	    },
	    HMStringItem    /* zoom box */
			{
		"Click here to Zoom In or Zoom Out."
			},
	    HMSkipItem {},  /* active app's inactive window */
	    HMStringItem    /* inactive app's window */
	    {
		"This is not part of the Microsoft HierSvr "
		"Application. It may be part of the Apple "
		"Finder, or some other application."
	    },
	    HMSkipItem {}   /* outside modal dialog */
	}
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 HierSvr sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else   // 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 HierSvr sample application "
	"ported to the Macintosh using Microsoft VC++ Edition "
	"for the Apple Macintosh"
};
#endif

resource 'vers' (2)
{
	0x04,
	0x00,
	final,
	0x00,
	verUS,
	"4.0",
	"MFC for Macintosh 4.0"
};

#ifdef _MPPC_
resource 'cfrg' (0) {
  {
		kPowerPC,
		kFullLib,
		kNoVersionNum,
		kNoVersionNum,
		kDefaultStackSize,
		kNoAppSubFolder,
		kIsApp,
		kOnDiskFlat,
		kZeroOffset,
		kWholeFork,
		"hiersvr"
  }
};
#endif
