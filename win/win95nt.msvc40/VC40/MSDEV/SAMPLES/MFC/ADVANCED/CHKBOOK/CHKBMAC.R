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
// Code Fragment resources

#include    "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// CheckBook resources

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
	0x03,
	0x00,
	final,
	0x00,
	verUS,
	"3.0",
	"CheckBook 3.0, Copyright \251 Microsoft Corp. 1994-1995"
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

resource 'kind' (128)
{
	'CHKB',
	0,          // verUS
	{
		'CHBK', "CheckBook document",
	}
};

resource 'open' (128)
{
	'CHKB',
	{
		'CHBK'
	}
};

resource 'BNDL' (128)
{
	'CHKB',
	0,
	{
		'FREF',
		{
			0, 128,
			1, 129,
			2, 130
		},
		'ICN#',
		{
			0, IDR_MAINFRAME,
			1, IDR_BOOKFRAME,
			2, IDR_CHECKFRAME
		}
	}
};

type 'CHKB' as 'STR ';
resource 'CHKB' (0)
{
	"CheckBook 3.0 Copyright \251 1994-1995 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};

resource 'FREF' (129)
{
	'CHBK',
	1,
	""
};

resource 'FREF' (130)
{
	'CHBK',
	2,
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
            "Miscellaneous part of the Microsoft CheckBook "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft CheckBook "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
			{
                "Click here to Zoom In or Zoom Out."
			},
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft CheckBook "
                "Application. It may be part of the Apple "
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 CheckBook sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 CheckBook sample application "
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

