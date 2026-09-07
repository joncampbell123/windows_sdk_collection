//	oldbmac.r: Macintosh-specific resources

#include "resource.h"
#include "types.r"
#include "ftab.r"
#include "CodeFrag.r"

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
	useTextEditServices,
	reserved,
	reserved,
	reserved,
#ifdef _DEBUG
	1500 * 1024,
	1350 * 1024
#else
	1015 * 1024,
	965 * 1024
#endif
};

resource 'BNDL' (128, purgeable)	// oldbars bundle resource ID
{
	'OLDB',					// oldbars signature
	0,                      // resource ID of signature resource:
							// should be 0
	{
		'ICN#',             // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, IDR_MAINFRAME,
			1, IDR_OLDBARTYPE
		},
		'FREF',             // local resource IDs for 'FREF's
		{
			0, 128,
			1, 129
		}
	}
};

resource 'FREF' (128, purgeable)	// oldbars	application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)	// oldbars	document
{
	'OLDB', 1,
	""
};

type 'OLDB' as 'STR ';

resource 'OLDB' (0, purgeable)
{
	"OldBars 1.0, Copyright © 1994 Microsoft Corp."
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
