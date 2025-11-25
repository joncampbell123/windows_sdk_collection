#include	"CodeFrag.r"
#include	"resource.h"
#include        "systypes.r"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include	"ftab.r"


/////////////////////////////////////////////////////////////////////////
// MFC resources

#include	"afxaete.r"


/////////////////////////////////////////////////////////////////////////
// DLLHusk resources

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

resource 'cfrg' (0)
{
	{
		ARCHITECTURE,
		kFullLib,
		kNoVersionNum,
		kNoVersionNum,
		kDefaultStackSize,
		kNoAppSubFolder,
		kIsApp,
		kOnDiskFlat,
		kZeroOffset,
		kWholeFork,
		"DLLHusk"
	}
};

resource 'vers' (1)
{
	0x03,
	0x00,
	final,
	0x00,
	verUS,
	"3.0",
	"DLLHusk 3.0, Copyright \251 Microsoft Corp. 1994-1995"
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

#ifdef LATER
resource 'kind' (128)
{
	'CHKB',
	0,			// verUS
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
	'DLLH',
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
#endif
