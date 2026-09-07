//	dynabindMac.r: Macintosh-specific resources

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

// Set different sizes depending on whether we're compiling for
//  PowerMacintosh or 68K, debug or retail.

#ifdef _MPPC_

#ifdef _DEBUG
	3000 * 1024,
	3000 * 1024
#else
	2500 * 1024,
	2500 * 1024
#endif //_DEBUG

#else //!_MPPC_

#ifdef _DEBUG
	2000 * 1024,
	2000 * 1024
#else
	2000 * 1024,
	2000 * 1024
#endif //DEBUG

#endif //_MPPC

};

resource 'BNDL' (128, purgeable)	// dynabind bundle resource ID
{
	'DYNA',			// dynabind signature
	0,						// resource ID of signature resource:
							// should be 0
 	{
		'ICN#',				// mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, 128,
			1, 129
		},
		'FREF',				// local resource IDs for 'FREF's
		{
			0, 128,
			1, 129
		}
	}
};

resource 'FREF' (128, purgeable)	// dynabind application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)	// dynabind document
{
	'DYNA', 1,
	""
};

type 'DYNA' as 'STR ';

resource 'DYNA' (0, purgeable)
{
	"dynabind Application"
};

#ifdef _MPPC_

resource 'cfrg' (0)
{
	{
		kPowerPC,
		kFullLib,
		kNoVersionNum,
		kNoVersionNum,
		kDefaultStackSize,
		kNoAppSubFolder,
		kIsApp,
		kOnDiskFlat,
		kZeroOffset,
		kWholeFork,
		"dynabind"
	}
};

#endif //_MPPC_
