#include    "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


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
#ifdef _DEBUG
	1500 * 1024,
	1350 * 1024
#else
	750 * 1024,
	650 * 1024
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
	"ControlBars 1.0, Copyright \251 Microsoft Corp. 1994"
};

resource 'vers' (2)
{
	0x03,
	0x00,
	final,
	0x00,
	verUS,
	"3.0",
	"MFC for Macintosh 3.0"
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
	"ControlBars 1.0 Copyright \251 1994 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};
