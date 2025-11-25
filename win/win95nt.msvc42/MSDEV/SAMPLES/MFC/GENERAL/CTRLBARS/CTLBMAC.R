#include	"mrc\types.r"
#include	"mrc\balloons.r"
#include        "resource.h"
#include        "systypes.r"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// Code fragment resource

#include    "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// CtrlBars resources

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

resource 'vers' (1)
{
	0x01,
	0x00,
	final,
	0x00,
	verUS,
	"1.0",
	"ControlBars 1.0, Copyright \251 Microsoft Corp. 1994-1995"
};

resource 'BNDL' (128)
{
	'CTLB',
	0,
	{
		'FREF',
		{
			0, 128
		},
		'ICN#',
		{
			0, IDR_MAINFRAME
		}
	}
};

type 'CTLB' as 'STR ';
resource 'CTLB' (0)
{
	"ControlBars 1.0 Copyright \251 1994-1995 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
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
            "Miscellaneous part of the Microsoft ControlBars "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft ControlBars "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
			{
                "Click here to Zoom In or Zoom Out."
			},
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft ControlBars "
                "Application. It may be part of the Apple "
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 ControlBars sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 ControlBars sample application "
	"ported to the Macintosh using Microsoft VC++ Edition "
	"for the Apple Macintosh"
};
#endif

#ifdef _MPPC_
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

