/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"
#include	"mrc\types.r"
#include	"mrc\balloons.r"
#include        "systypes.r"

/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// Code Fragment resources

#include    "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// CtrlTest resources

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
	0x01,
	final,
	0x00,
	verUS,
	"1.1",
	"ControlTest 1.1, Copyright \251 Microsoft Corp. 1994-1995"
};

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

resource 'BNDL' (128)
{
	'CTRL',
	0,
	{
		'FREF',
		{
			0, 2
		},
		'ICN#',
		{
			0, 31234
		}
	}
};

type 'CTRL' as 'STR ';
resource 'CTRL' (0)
{
	"ControlTest 1.1 Copyright \251 1994-1995 Microsoft Corp."
};

resource 'FREF' (2)
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
            "Miscellaneous part of the Microsoft ControlTest "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft ControlTest "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
			{
                "Click here to Zoom In or Zoom Out."
			},
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft ControlTest "
                "Application. It may be part of the Apple "
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 ControlTest sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 ControlTest sample application "
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
