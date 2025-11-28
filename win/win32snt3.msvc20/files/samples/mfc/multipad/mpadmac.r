#include    "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// MultiPad resources

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
	0x01,
	0x00,
	final,
	0x00,
	verUS,
	"1.0",
	"MultiPad 1.0, Copyright \251 Microsoft Corp. 1994"
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
	'MPAD',
	0,          // verUS
	{
		'TEXT', "MultiPad text document",
	}
};

resource 'open' (128)
{
	'MPAD',
	{
		'TEXT', 'ttro'
	}
};

resource 'BNDL' (128)
{
	'MPAD',
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
			1, IDR_TEXTTYPE,
			2, 130
		}
	}
};

type 'MPAD' as 'STR ';
resource 'MPAD' (0)
{
	"MultiPad 1.0 Copyright © 1994 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};

resource 'FREF' (129)
{
	'TEXT',
	1,
	""
};

resource 'FREF' (130) {
	'sEXT',
	2,
	""
};
