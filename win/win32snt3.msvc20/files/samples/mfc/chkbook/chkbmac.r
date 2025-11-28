#include    "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


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
#ifdef _DEBUG
	1500 * 1024,
	1350 * 1024
#else
	850 * 1024,
	750 * 1024
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
	"CheckBook 3.0, Copyright \251 Microsoft Corp. 1994"
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
	"CheckBook 3.0 Copyright \251 1994 Microsoft Corp."
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
