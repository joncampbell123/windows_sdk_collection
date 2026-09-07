#include    "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"
#include    "systypes.r"

/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// Code fragment resources

#include    "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// PropDlg resources

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
	"PropDlg 1.0, Copyright \251 Microsoft Corp. 1992-1995"
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
	'SHAP',
	0,          // verUS
	{
		'SHAP', "PropDlg Shape document",
	}
};

resource 'open' (128)
{
	'SHAP',
	{
		'SHAP'
	}
};

resource 'BNDL' (128)
{
	'PROP',
	0,
	{
		'FREF',
		{
			0, 128,
			1, 129
		},
		'ICN#',
		{
			0, IDR_MAINFRAME,
			1, IDR_SHAPESTYPE
		}
	}
};

type 'PROP' as 'STR ';
resource 'PROP' (0)
{
	"PropDlg 1.0 Copyright © 1994-1995 Microsoft Corp."
};

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};

resource 'FREF' (129)
{
	'SHAP',
	1,
	""
};

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
