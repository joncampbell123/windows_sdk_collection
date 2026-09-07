//      oclientMac.r: Macintosh-specific resources

#include "types.r"
#include "ftab.r"
#include "CodeFrag.r"
#include "resource.h"

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
	4000 * 1024,
	4000 * 1024
#else
	3000 * 1024,
	3000 * 1024
#endif //_DEBUG

#else //!_MPPC_

#ifdef _DEBUG
	4000 * 1024,
	4000 * 1024
#else
	4000 * 1024,
	4000 * 1024
#endif //DEBUG

#endif //_MPPC

};

resource 'BNDL' (IDR_MAINFRAME, purgeable)        // oclient bundle resource ID
{
	'OCLI',                 // oclient signature
	0,                                              // resource ID of signature resource:
							// should be 0
	{
		'ICN#',                         // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, IDR_MAINFRAME,
			1, IDR_OCLIENTTYPE
		},
		'FREF',                         // local resource IDs for 'FREF's
		{
			0, IDR_MAINFRAME,
			1, IDR_OCLIENTTYPE
		}
	}
};

resource 'FREF' (IDR_MAINFRAME, purgeable)        // oclient application
{
	'APPL', 0,
	""
};

resource 'FREF' (IDR_OCLIENTTYPE, purgeable)        // oclient document
{
	'OCLI', 1,
	""
};

type 'OCLI' as 'STR ';

resource 'OCLI' (0, purgeable)
{
	"oclient Application"
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
		"oclient"
	}
};

#endif //_MPPC_
