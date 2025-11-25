//      catalogMac.r: Macintosh-specific resources

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

resource 'BNDL' (IDR_MAINFRAME, purgeable)        // catalog bundle resource ID
{
	'CATA',                 // catalog signature
	0,                                              // resource ID of signature resource:
							// should be 0
	{
		'ICN#',                         // mapping local IDs in 'FREF's to 'ICN#' IDs
		{
			0, IDR_MAINFRAME
		},
		'FREF',                         // local resource IDs for 'FREF's
		{
			0, 128,
			1, 129
		}
	}
};

resource 'FREF' (128, purgeable)        // catalog application
{
	'APPL', 0,
	""
};

resource 'FREF' (129, purgeable)        // catalog document
{
	'CATA', 1,
	""
};

type 'CATA' as 'STR ';

resource 'CATA' (0, purgeable)
{
	"catalog Application"
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
		"catalog"
	}
};

#endif //_MPPC_
