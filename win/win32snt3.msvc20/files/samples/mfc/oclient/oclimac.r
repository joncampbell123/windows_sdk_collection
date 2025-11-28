/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"
#include    "afxres.r"


/////////////////////////////////////////////////////////////////////////
// Contain resources

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
	2100 * 1024,
	1650 * 1024
#else
	1050 * 1024,
	850 * 1024
#endif
};

resource 'vers' (1)
{
	0x01,
	0x00,
	alpha,
	0x01,
	verUS,
	"1.0a1",
	"OClient 1.0a1, Copyright \251 Microsoft Corp. 1992, 1993"
};

resource 'vers' (2)
{
	0x03,
	0x00,
	development,
	0x01,
	verUS,
	"3.0d1",
	"MFC for Macintosh 3.0d1"
};

resource 'kind' (128)
{
	'OCLI',
	0,          // verUS
	{
		'ODOC', "OClient document",
	}
};

resource 'open' (128)
{
	'OCLI',
	{
		'ODOC'
	}
};

resource 'BNDL' (128)
{
	'OCLI',
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
			0, 128,
			1, 129,
			2, 130
		}
	}
};

type 'OCLI' as 'STR ';
resource 'OCLI' (0)
{
	"OClient 1.0 Copyright \251 1992,1993 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};

resource 'FREF' (129)
{
	'ODOC',
	1,
	""
};

resource 'FREF' (130)
{
	'sDOC',
	2,
	""
};
