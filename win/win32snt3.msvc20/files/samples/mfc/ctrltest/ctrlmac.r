/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


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
	0x01,
	final,
	0x00,
	verUS,
	"1.1",
	"ControlTest 1.1, Copyright \251 Microsoft Corp. 1994"
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
	"ControlTest 1.1 Copyright \251 1994 Microsoft Corp."
};

resource 'FREF' (2)
{
	'APPL',
	0,
	""
};
