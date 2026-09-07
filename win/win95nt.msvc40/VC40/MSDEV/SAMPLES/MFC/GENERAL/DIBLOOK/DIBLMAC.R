#include    "resource.h"

/////////////////////////////////////////////////////////////////////////
// WLM resources

#include    "ftab.r"
#include	"mrc\types.r"
#include	"mrc\balloons.r"
#include        "systypes.r"

/////////////////////////////////////////////////////////////////////////
// MFC resources

#include    "afxaete.r"


/////////////////////////////////////////////////////////////////////////
// Code Fragment resources

#include    "CodeFrag.r"


/////////////////////////////////////////////////////////////////////////
// DIBLook resources

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
	"DIBLook 1.0, Copyright \251 Microsoft Corp. 1994 - 1995"
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
	'DIBL',
	0,          // verUS
	{
		'DIB ', "DIBLook bitmap",
	}
};

resource 'open' (128)
{
	'DIBL',
	{
		'DIB '
	}
};

resource 'BNDL' (128)
{
	'DIBL',
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
			1, IDR_DIBTYPE,
			2, 130
		}
	}
};

type 'DIBL' as 'STR ';
resource 'DIBL' (0)
{
	"DIBLook 1.0 Copyright \251 1994 - 1995 Microsoft Corp."
};

/* Balloon help resources */

resource 'hfdr' (-5696)
{
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
		HMSTRResItem {500}
	}
};

resource 'hovr' (1000)
{
    HelpMgrVersion, hmDefaultOptions, 0, 0,

        HMStringItem    /* missing items override */
        {
            "Miscellaneous part of the Microsoft DIBLook "
            "Sample Application."
        },
        {
            HMSkipItem {},  /* title bar */
            HMSkipItem {},  /* reserved. always skip item here */
            HMStringItem    /* close box */
            {
                "Click here to close the Microsoft DIBLook "
                "Sample Application."
            },
            HMStringItem    /* zoom box */
			{
                "Click here to Zoom In or Zoom Out."
			},
            HMSkipItem {},  /* active app's inactive window */
            HMStringItem    /* inactive app's window */
            {
                "This is not part of the Microsoft DIBLook "
                "Application. It may be part of the Apple "
                "Finder, or some other application."
            },
            HMSkipItem {}   /* outside modal dialog */
        }
};

#ifdef _MPPC_
resource 'STR ' (500)
{
	"This is the Win32 DIBLook sample application "
	"ported to the Power Macintosh using Microsoft VC++ "
	"Edition for the Apple Power Macintosh"
};
#else	// 68K Mac
resource 'STR ' (500)
{
	"This is the Win32 DIBLook sample application "
	"ported to the Macintosh using Microsoft VC++ Edition "
	"for the Apple Macintosh"
};
#endif

resource 'FREF' (128)
{
	'APPL',
	0,
	""
};

resource 'FREF' (129)
{
	'DIB ',
	1,
	""
};

resource 'FREF' (130)
{
	'sIB ',
	2,
	""
};

resource 'ICN#' (130)
{
	{
		$"3FFF FFE0 2000 0020 2003 F038 201D 2E28"
		$"2026 D928 204B 6CA8 2095 0A68 20A5 0D68"
		$"217B 8D68 21BD 8D68 208C 0CE8 2008 1CE8"
		$"2008 2868 2380 2828 2440 5028 2440 9028"
		$"2441 2028 2382 4028 2004 8028 3FFC FFE8"
		$"2008 8128 28F0 4428 2300 3028 2400 0FE8"
		$"2800 0848 3000 0888 2000 0908 2000 0A08"
		$"2000 0C08 3FFF F808 0800 0008 0FFF FFF8",

		$"3FFF FFE0 3FFF FFE0 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 0FFF FFF8 0FFF FFF8"
	}
};

resource 'icl8' (130)
{
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
	$"F5F5 F5F5 F5F5 F5F5 F5F5 FF00 0000 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF"
	$"FFFF FFFF F5F5 F5F5 F5F5 FFFF FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5FF FFFF E3FF"
	$"E3E3 FFE3 FFFF FFF5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 FFE3 E3FF FFF5"
	$"FFFF 33FF FFE3 E3FF F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5FF E3E3 FFF5 FFFF"
	$"F5FF FFF5 FFFF E3E3 FFF5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 FFE3 E3FF F5FF E3FF"
	$"F5F5 F5F5 FFF5 FFE3 E3FF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 FFE3 FFF5 F5E3 E3FF"
	$"F5F5 F5F5 FFFF F5FF E3FF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5FF E3FF 33FC E3E3 FF33"
	$"FCF5 F5F5 FFFF F5FF E3FF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5FF E3F5 FC33 E3FF F5FC"
	$"33F5 F5F5 FFFF F5FF E3FF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 FFF5 F5F5 E3FF F5F5"
	$"F5F5 F5F5 FFFF F5F5 FFFF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 E3F5 F5F5"
	$"F5F5 F5FF FFFF F5F5 FFFF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 FFF5 F5F5"
	$"F5F5 FF33 FFF5 F5F5 F5FF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 FFFF FFF5 F5F5 F5F5 F5F5"
	$"F5F5 FFFC FFF5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5FF 0505 05FF F5F5 F5F5 F5F5"
	$"F5FF 33FF F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5FF 0505 05FF F5F5 F5F5 F5F5"
	$"FF33 FCFF F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5FF 0505 05FF F5F5 F5F5 F5FF"
	$"33FC FFF5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 FFFF FFF5 F5F5 F5F5 FF33"
	$"FCFF F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5FF 33FC"
	$"FFF5 F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FC33"
	$"FFFF FFFF FFFF FFFF FFFF FFF9 FF00 0000"
	$"0000 FFC0 C0C0 C0C0 C0C0 C0C0 FFFC 33FC"
	$"FFC0 C0C0 C0C0 C0FF C0C0 FFF9 FF00 0000"
	$"0000 FFC0 FFC0 C0C0 FFFF FFFF 3333 3333"
	$"33FF C0C0 C0FF C0C0 C0C0 FFF9 FF00 0000"
	$"0000 FFC0 C0C0 FFFF 3333 3333 3333 3333"
	$"3333 FFFF C0C0 C0C0 C0C0 FFF9 FF00 0000"
	$"0000 FFC0 C0FF 3333 3333 3333 3333 3333"
	$"3333 3333 FFFF FFFF FFFF FFF9 FF00 0000"
	$"0000 FFC0 FF33 3333 3333 3333 3333 3333"
	$"3333 3333 FF2B 2B2B F7FF F9F7 FF00 0000"
	$"0000 FFFF 3333 3333 3333 3333 3333 3333"
	$"3333 3333 FF2B 2BF7 FFF9 F72B FF00 0000"
	$"0000 FF33 3333 3333 3333 3333 3333 3333"
	$"3333 3333 FF2B F7FF F9F7 2BF6 FF00 0000"
	$"0000 FF33 3333 3333 3333 3333 3333 3333"
	$"3333 3333 FFF7 FFF9 F72B F6F6 FF00 0000"
	$"0000 FF33 3333 3333 3333 3333 3333 3333"
	$"3333 3333 FFFF F9F7 2BF6 F6F6 FF00 0000"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFF9 F72B F6F6 F6F6 FF00 0000"
	$"0000 0000 FFF9 F9F9 F9F9 F9F9 F9F9 F9F9"
	$"F9F9 F9F9 F9F7 2BF6 F6F6 F6F6 FF00 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF FFFF FFFF"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FF"
};

resource 'icl4' (130)
{
	$"00FF FFFF FFFF FFFF FFFF FFFF FFF0 0000"
	$"00F0 C0C0 C0C0 C0C0 C0C0 C0C0 C0F0 0000"
	$"00FC 0C0C 0C0C 0CFF FFFF 0C0C 0CFF F000"
	$"00F0 C0C0 C0CF FF8F 88F8 FFF0 C0FD F000"
	$"00FC 0C0C 0CF8 8FF0 FFAF F88F 0CFD F000"
	$"00F0 C0C0 CF88 FCFF CFF0 FF88 F0FD F000"
	$"00FC 0C0C F88F CF8F 0C0C F0F8 8FFD F000"
	$"00F0 C0C0 F8FC 088F C0C0 FFCF 8FFD F000"
	$"00FC 0C0F 8FAA 88FA AC0C FF0F 8FFD F000"
	$"00F0 C0CF 80AA 8FCA A0C0 FFCF 8FFD F000"
	$"00FC 0C0C FC0C 8F0C 0C0C FF0C FFFD F000"
	$"00F0 C0C0 C0C0 80C0 C0CF FFC0 FFFD F000"
	$"00FC 0C0C 0C0C FC0C 0CFA FC0C 0FFD F000"
	$"00F0 C0FF F0C0 C0C0 C0FA F0C0 C0FD F000"
	$"00FC 0F11 1F0C 0C0C 0FAF 0C0C 0CFD F000"
	$"00F0 CF11 1FC0 C0C0 FAAF C0C0 C0FD F000"
	$"00FC 0F11 1F0C 0C0F AAFC 0C0C 0CFD F000"
	$"00F0 C0FF F0C0 C0FA AFC0 C0C0 C0FD F000"
	$"00FC 0C0C 0C0C 0FAA FC0C 0C0C 0CFD F000"
	$"00FF FFFF FFFF FFAA FFFF FFFF FFFD F000"
	$"00F7 7777 7777 FABA F777 777F 77FD F000"
	$"00F7 F777 FFFF BBBB BF77 7F77 77FD F000"
	$"00F7 77FF BBBB BBBB BBFF 7777 77FD F000"
	$"00F7 7FBB BBBB BBBB BBBB FFFF FFFD F000"
	$"00F7 FBBB BBBB BBBB BBBB FCCC CFDD F000"
	$"00FF BBBB BBBB BBBB BBBB FCCC FDDC F000"
	$"00FB BBBB BBBB BBBB BBBB FCCF DDCC F000"
	$"00FB BBBB BBBB BBBB BBBB FCFD DCCC F000"
	$"00FB BBBB BBBB BBBB BBBB FFDD CCCC F000"
	$"00FF FFFF FFFF FFFF FFFF FDDC CCCC F000"
	$"0000 FDDD DDDD DDDD DDDD DDCC CCCC F000"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF F0"
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



