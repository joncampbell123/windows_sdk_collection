//  hello.r: Macintosh-specific resources

#include "resource.h"
#include "types.r"
#include "ftab.r"

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
	850 * 1024,
	750 * 1024
#endif
};

resource 'BNDL' (128, purgeable)    // viewex bundle resource ID
{
	'VIEW',                 // viewex signature
	0,                      // resource ID of signature resource:
							// should be 0
	{
		'ICN#',             // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, IDR_MAINFRAME,
			1, 129
		},
		'FREF',             // local resource IDs for 'FREF's
		{
			0, 128,
			1, 129
		}
	}
};

resource 'FREF' (128, purgeable)    // viewex application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)    // viewex document
{
	'VIEW', 1,
	""
};

type 'VIEW' as 'STR ';

resource 'VIEW' (0, purgeable)
{
	"viewex Application"
};
