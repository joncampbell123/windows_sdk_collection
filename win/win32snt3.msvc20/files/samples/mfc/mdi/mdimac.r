/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// MDI resources

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
	dontUseTextEditServices,
	reserved,
	reserved,
	reserved,
#ifdef _DEBUG
	1450 * 1024,
	1300 * 1024
#else
	750 * 1024,
	650 * 1024
#endif
};

resource 'vers' (1)
{
	0x02,
	0x00,
	final,
	0x00,
	verUS,
	"2.0",
	"MDI 2.0, Copyright \251 Microsoft Corp. 1992-1994"
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

resource 'BNDL' (128, purgeable)    // mdiapp bundle resource ID
{
	'MDI ',
	0,
	{
		'FREF',
		{
			0, 128
		},
		'ICN#',
		{
			0, 31233
		}
	}
};

type 'MDI ' as 'STR ';
resource 'MDI ' (0)
{
	"MDI Application"
};

resource 'FREF' (128, purgeable)    // mdi application
{
	'APPL', 0,
	""
};
