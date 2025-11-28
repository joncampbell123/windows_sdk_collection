//  tracmac.r: Macintosh-specific resources

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

resource 'BNDL' (128, purgeable)    // tracer bundle resource ID
{
	'TRAC',                 // tracer signature
	0,                      // resource ID of signature resource:
							// should be 0
	{
		'ICN#',             // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, 31234,
			1, 129
		},
		'FREF',             // local resource IDs for 'FREF's
		{
			0, 128,
			1, 129
		}
	}
};

resource 'FREF' (128, purgeable)    // tracer application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)    // tracer document
{
	'TRAC', 1,
	""
};

type 'TRAC' as 'STR ';

resource 'TRAC' (0, purgeable)
{
	"tracer Application"
};
