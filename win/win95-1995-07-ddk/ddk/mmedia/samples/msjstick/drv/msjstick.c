/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1993 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       msjstick.c
 *  Content:	Generic game port joystick device driver - entry point,
 *   		initialization and message module
 *
 ***************************************************************************/

#include <windows.h>
#include <memory.h>
#include <malloc.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include <regstr.h>
#include "joycom.h"
#include "msjstick.h"

#ifdef DEBUG
void FAR cdecl dprintf(LPSTR szFormat, ...)
{
    char str[256];

#if 1
    wsprintf( str, "$$$ MSJSTICK: " );
    wvsprintf( str+lstrlen( str ), szFormat, (LPVOID)(&szFormat+1) );
    lstrcat( str, " $$$\r\n" );
    OutputDebugString( str );
#else
    wvsprintf( str, szFormat, (LPVOID)(&szFormat+1) );
    MessageBox( NULL, str, "MSJSTICK.DRV", MB_OK | MB_SYSTEMMODAL );
#endif
}
#define DPF	dprintf 
#else
#define DPF 1 ? (void)0 : (void)
#endif

/*********************************************************************

    Private data

 ********************************************************************/

static HINSTANCE 	hInstance;	/* driver instance */
static char             szRegKey[MAXPNAMELEN];
static char		szCfgRegKey[] = REGSTR_PATH_JOYCONFIG "\\";
static JOYDATA		jd;
static BOOL		bHasVJoyD;
static UINT		uMsgId;
static DWORD		dwMaxJoysticks = MAX_JOYSTICKS;

/****************************************************************************
 * @doc INTERNAL
 *
 * @api int | LibMain | Library initialization code.
 *
 * @parm HANDLE | hInstance | Our instance handle.
 *
 * @parm WORD | wDataSeg | Our data segment.
 *
 * @parm WORD | wHeapSize | The heap size from the .def file.
 *
 * @parm LPSTR | lpCmdLine | The command line.
 *
 * @rdesc Returns 1 if the initialization was successful and 0 otherwise.
 ***************************************************************************/
int NEAR PASCAL LibMain(HINSTANCE hInst, WORD wHeapSize, LPCSTR lpCmdLine)
{
    hInstance = hInst;
    bHasVJoyD = FALSE;
    if( !jsInitVJoyD() ) {
	return 0;
    }
    bHasVJoyD = TRUE;
    uMsgId = RegisterWindowMessage( JOY_CONFIGCHANGED_MSGSTRING	);
    return 1;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | jsGetDevCaps | Get the device capabilities.
 *
 * @parm BYTE FAR * | lpJoyCaps | Far pointer to a JOYCAPS structure
 *     to receive the information.
 *
 * @parm WORD | wSize | The size of the JOYCAPS structure.
 *
 * @rdesc The return value is always zero.
 ***************************************************************************/
static LRESULT NEAR PASCAL jsGetDevCaps( UINT joyid, JOYCAPS FAR* const lpCaps, UINT uSize)
{
    JOYCAPS	jc;
    LPIJOYDATA	pijd;
    JOYHWCAPS	jhwc;
    BOOL	axes_ok;

    /*
     * set non-device specific stuff
     */
    lstrcpy( jc.szRegKey, szRegKey );
    LoadString( hInstance, IDS_DESCRIPTION, jc.szPname, sizeof(jc.szPname) );
    jc.wMid = MM_MICROSOFT;	/* manufacturer id */
    jc.wPid = MM_PC_JOYSTICK;	/* product id */
    jc.wPeriodMin = MIN_PERIOD;	/* period low */
    jc.wPeriodMax = MAX_PERIOD;	/* period high */

    /*
     * uSize with hi-bit on: get basic info only...
     */
    if( uSize & 0x8000 ) {
	uSize &= 0x7fff;
	_fmemcpy(lpCaps, (LPJOYCAPS)&jc, min(uSize, sizeof(JOYCAPS)));
	return JOYERR_NOERROR;
    }

    /*
     * get hardware caps (max # of buttons & axes)
     */
    if( joyid >= dwMaxJoysticks || !jsGetHWCaps( joyid, &jhwc ) ) {
	return JOYERR_PARMS;
    }

    jc.wCaps = 0;		/* caps bits */

    jc.wMaxButtons = (WORD) jhwc.dwMaxButtons;
    jc.wMaxAxes = (WORD) jhwc.dwMaxAxes;
    jc.wNumAxes = (WORD) jhwc.dwNumAxes;

    pijd = &jd.ijd[joyid];

    /*
     * set x position range
     */
    jc.wXmin = (WORD) jd.usr.jrvRanges.jpMin.dwX;
    jc.wXmax = (WORD) jd.usr.jrvRanges.jpMax.dwX;

    /*
     * set y position range
     */
    jc.wYmin = (WORD) jd.usr.jrvRanges.jpMin.dwY;
    jc.wYmax = (WORD) jd.usr.jrvRanges.jpMax.dwY;

    /*
     * see if we can allow z/r
     */
    axes_ok = FALSE;
    if( pijd->has_z && pijd->has_r ) {
	if( jhwc.dwMaxAxes >= 4 ) {
	    axes_ok = TRUE;
	}
    } else if( pijd->has_z || pijd->has_r ) {
	if( jhwc.dwMaxAxes >= 3 ) {
	    axes_ok = TRUE;
	}
    }

    /*
     * set z position range
     */
    if( pijd->has_z && axes_ok ) {
	jc.wZmin = (WORD) jd.usr.jrvRanges.jpMin.dwZ;
	jc.wZmax = (WORD) jd.usr.jrvRanges.jpMax.dwZ;
	jc.wCaps |= JOYCAPS_HASZ;
    } else {
	jc.wZmin = 0;	
	jc.wZmax = 0;
    }

    /*
     * number of buttons
     */
    jc.wNumButtons = (WORD) pijd->hw.hws.dwNumButtons;
    if( jc.wNumButtons > jhwc.dwMaxButtons ) {
    	jc.wNumButtons = (WORD) jhwc.dwMaxButtons;
    }

    /*
     * set r position range
     */
    if( pijd->has_r && axes_ok ) {
	jc.wRmin = (WORD) jd.usr.jrvRanges.jpMin.dwR;
	jc.wRmax = (WORD) jd.usr.jrvRanges.jpMax.dwR;
	jc.wCaps |= JOYCAPS_HASR;
    } else {
	jc.wRmin = 0;
	jc.wRmax = 0;
    }

    /*
     * set POV capabilites
     */
    axes_ok = TRUE;
    if( pijd->pov_is_poll ) {
	/*
	 * must have at least 3 axes to have a POV hat that is polled
	 */
	if( jhwc.dwMaxAxes < 3 ) {
	    axes_ok = FALSE;
	}
    }
    if( pijd->has_pov && axes_ok ) {
	jc.wCaps |= (JOYCAPS_HASPOV | JOYCAPS_POV4DIR);
    }

    /*
     * see if we can allow u/v
     */
    axes_ok = FALSE;
    if( pijd->has_u && pijd->has_v ) {
	if( jhwc.dwMaxAxes >= 6 ) {
	    axes_ok = TRUE;
	}
    } else if( pijd->has_u || pijd->has_v ) {
	if( jhwc.dwMaxAxes >= 5 ) {
	    axes_ok = TRUE;
	}
    }

    /*
     * set U position range
     */
    if( pijd->has_u && axes_ok ) {
	jc.wUmin = (WORD) jd.usr.jrvRanges.jpMin.dwU;
	jc.wUmax = (WORD) jd.usr.jrvRanges.jpMax.dwU;
	jc.wCaps |= JOYCAPS_HASU;
    } else {
	jc.wUmin = 0;	
	jc.wUmax = 0;
    }

    /*
     * set V position range
     */
    if( pijd->has_v && axes_ok ) {
	jc.wVmin = (WORD) jd.usr.jrvRanges.jpMin.dwV;
	jc.wVmax = (WORD) jd.usr.jrvRanges.jpMax.dwV;
	jc.wCaps |= JOYCAPS_HASV;
    } else {
	jc.wVmin = 0;	
	jc.wVmax = 0;
    }

    lstrcpy( jc.szOEMVxD, jhwc.szOEMVxD );

    _fmemcpy(lpCaps, (LPJOYCAPS)&jc, min(uSize, sizeof(JOYCAPS)));

    return JOYERR_NOERROR;	/* no error */

} /* jsGetDevCaps */

/*
 * setJoyCal - set the JoyCal calibration array from the usr and hw settings
 */
static void setJoyCal( void )
{
    DWORD	hw_range;
    int		id;

    /*
     * set user calibration factors
     */
    #define SETCALUSR( a ) \
	jd.cal_usr.dw##a = jd.usr.jrvRanges.jpMax.dw##a - jd.usr.jrvRanges.jpMin.dw##a + 1;

    SETCALUSR( X );
    SETCALUSR( Y );
    SETCALUSR( Z );
    SETCALUSR( R );
    SETCALUSR( U );
    SETCALUSR( V );

    /*
     * set hardware calibration factors
     */
    #define SETCALHW( a ) \
	    hw_range = jd.ijd[id].hw.hwv.jrvHardware.jpMax.dw##a - jd.ijd[id].hw.hwv.jrvHardware.jpMin.dw##a + 1; \
	    if( hw_range == 0 ) { \
		hw_range = 1; \
	    } \
	    jd.ijd[id].cal_hw.dw##a = hw_range;

    for( id=0;id<(int)dwMaxJoysticks;id++ ) {
	SETCALHW( X );
	SETCALHW( Y );
	SETCALHW( Z );
	SETCALHW( R );
	SETCALHW( U );
	SETCALHW( V );
    }

} /* setJoyCal */

/*
 * doPOVCal - compute calibration for POV for a direction
 */
static void doPOVCal( LPIJOYDATA pijd, WORD dir, LPWORD order )
{
    DWORD	val;
    int		i;

    for( i=0;i<JOY_POV_NUMDIRS;i++ ) {
	if( order[i] == dir ) {
	    break;
	}
    }

    if( i == 0 ) {
	val = 1;
    } else {
	val = (pijd->hw.hwv.dwPOVValues[dir] - pijd->hw.hwv.dwPOVValues[order[i-1]])/2;
	val = pijd->hw.hwv.dwPOVValues[dir] - val;
    }
    pijd->povcal[POV_MIN][dir] = (WORD) val;

    if( i == JOY_POV_NUMDIRS-1 ) {
	val = pijd->hw.hwv.dwPOVValues[dir]/10l;
	val += pijd->hw.hwv.dwPOVValues[dir];
    } else {
	val = (pijd->hw.hwv.dwPOVValues[order[i+1]] - pijd->hw.hwv.dwPOVValues[dir])/2;
	val = pijd->hw.hwv.dwPOVValues[dir] + val;
    }
    pijd->povcal[POV_MAX][dir] = (WORD) val;

} /* doPOVCal */

/*
 * jsReadRegistry - read new config info from the registry
 */
static void NEAR PASCAL jsReadRegistry( void )
{
    HKEY		hkey;
    HKEY		hsubkey;
    JOYREGHWCONFIG	config;
    JOYREGUSERVALUES	user;
    DWORD		dwType;
    DWORD		cb;
    DWORD		hwflags;
    WORD		order[JOY_POV_NUMDIRS];
    WORD		tmp[JOY_POV_NUMDIRS];
    WORD		val;
    int			i,j;
    LPSTR		cfgkey;
    char		cfgbuff[512];
    int			len;
    LPIJOYDATA		pijd;
    int			joyid;

    /*
     * reset joystick data
     */
    _fmemset( &jd, 0, sizeof( jd ) );

    /*
     * get user values: return ranges, deadzone, timeout
     */
    jd.usr.jrvRanges.jpMin.dwX = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMin.dwY = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMin.dwZ = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMin.dwR = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMin.dwU = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMin.dwV = DEFAULT_RANGE_MIN;
    jd.usr.jrvRanges.jpMax.dwX = DEFAULT_RANGE_MAX;
    jd.usr.jrvRanges.jpMax.dwY = DEFAULT_RANGE_MAX;
    jd.usr.jrvRanges.jpMax.dwZ = DEFAULT_RANGE_MAX;
    jd.usr.jrvRanges.jpMax.dwR = DEFAULT_RANGE_MAX;
    jd.usr.jrvRanges.jpMax.dwU = DEFAULT_RANGE_MAX;
    jd.usr.jrvRanges.jpMax.dwV = DEFAULT_RANGE_MAX;
    jd.usr.jpDeadZone.dwX = DEFAULT_DEADZONE;
    jd.usr.jpDeadZone.dwY = DEFAULT_DEADZONE;
    jd.usr.dwTimeOut = DEFAULT_TIMEOUT;

    /*
     * get configuration key
     */
    len = sizeof( szCfgRegKey ) + lstrlen( (LPSTR) szRegKey );
    if( len > sizeof( cfgbuff )-1 ) {
	cfgkey = NULL;
    } else {
	cfgkey = cfgbuff;
    }
    if( cfgkey != NULL )  {
	lstrcpy( cfgkey, szCfgRegKey );
	lstrcpy( &cfgkey[sizeof(szCfgRegKey)-1], (LPSTR) szRegKey );
    }

    /*
     * get user info
     */
    hkey = NULL;
    if( cfgkey != NULL && !RegOpenKey( HKEY_LOCAL_MACHINE, cfgkey, &hkey ) ) {
	cb = sizeof( user );
	if( !RegQueryValueEx( hkey, REGSTR_VAL_JOYUSERVALUES, NULL, &dwType,
			(const LPBYTE)&user, &cb) ) {
	    if( dwType == REG_BINARY && cb == sizeof( user ) ) {
		jd.usr = user;
	    }
	}
    }

    /*
     * get hardware settings for each joystick
     */
    jd.ijd[JOYSTICKID1].hw.dwUsageSettings = JOY_US_PRESENT;
    if( hkey != NULL && !RegOpenKey( hkey, REGSTR_KEY_JOYCURR, &hsubkey ) ) {
	for( i=0;i<(int)dwMaxJoysticks;i++ ) {
	    pijd = &jd.ijd[i];
	    cb = sizeof( config );
	    wsprintf( cfgbuff, REGSTR_VAL_JOYNCONFIG, i+1 );
	    if( !RegQueryValueEx( hsubkey, cfgbuff, NULL, &dwType,
			    (const LPBYTE)&config, &cb) ) {
		if( dwType == REG_BINARY && cb == sizeof( config ) ) {
		    jd.ijd[i].hw = config;
		}
	    }
	}
	RegCloseKey( hsubkey );
    }
    if( hkey != NULL ) {
	RegCloseKey( hkey );
    }

    /*
     * set some default hardware max values if there aren't any
     */
    #define SETDEFHW( a ) \
	    if( jd.ijd[i].hw.hwv.jrvHardware.jpMax.dw##a == 0 ) { \
		jd.ijd[i].hw.hwv.jrvHardware.jpMin.dw##a = 0; \
		jd.ijd[i].hw.hwv.jrvHardware.jpMax.dw##a = DEFAULT_HW; \
		jd.ijd[i].hw.hwv.jrvHardware.jpCenter.dw##a = DEFAULT_HW/2; \
	    }
     for( i=0; i<(int)dwMaxJoysticks; i++ ) {
	 SETDEFHW( X );
	 SETDEFHW( Y );
	 SETDEFHW( Z );
	 SETDEFHW( R );
	 SETDEFHW( U );
	 SETDEFHW( V );
     }

    /*
     * set calibration array based on user values and hardware
     */
    setJoyCal();

    /*
     * process Z, R, and POV data for both joystickis
     */
    for( joyid=0; joyid<(int)dwMaxJoysticks; joyid++ ) {
	pijd = &jd.ijd[joyid];
	/*
	 * initial state
	 */
	pijd->has_z = FALSE;
	pijd->has_r = FALSE;
	pijd->has_u = FALSE;
	pijd->has_v = FALSE;
	pijd->has_pov = FALSE;
	pijd->pov_is_poll = FALSE;
	pijd->present = FALSE;

	/*
	 * see if hw is present
	 */
	if( pijd->hw.dwUsageSettings & JOY_US_PRESENT ) {
	    pijd->present = TRUE;
	}

	/*
	 * check hardware caps
	 */
	hwflags = pijd->hw.hws.dwFlags;
	if( hwflags & JOY_HWS_HASZ ) {
	    pijd->has_z = TRUE;
	}
	if( hwflags & JOY_HWS_HASU ) {
	    pijd->has_u = TRUE;
	}
	if( hwflags & JOY_HWS_HASV ) {
	    pijd->has_v = TRUE;
	}
	if( hwflags & JOY_HWS_HASR ) {
	    pijd->has_r = TRUE;
	} else {
	    if( pijd->hw.dwUsageSettings & JOY_US_HASRUDDER ) {
		pijd->has_r = TRUE;
		/* default to J2 X */
		pijd->hw.hws.dwFlags &=
			~(JOY_HWS_RISJ1X|JOY_HWS_RISJ1Y|JOY_HWS_RISJ2Y);
	    }
	}
	if( hwflags & JOY_HWS_HASPOV ) {
	    pijd->has_pov = TRUE;
	    if( hwflags & JOY_HWS_POVISPOLL ) {
		pijd->pov_is_poll = TRUE;
	    }
	    /*
	     * calibrate POV for polling based ones
	     */
	    if( pijd->pov_is_poll ) {
		for( i=0;i<JOY_POV_NUMDIRS;i++ ) {
		    tmp[i] = (WORD) pijd->hw.hwv.dwPOVValues[i];
		    order[i] = i;
		}
		/*
		 * sort (did you ever think you'd see a bubble sort again?)
		 */
		for( i=0;i<JOY_POV_NUMDIRS;i++ ) {
		    for( j=i;j<JOY_POV_NUMDIRS;j++ ) {
			if( tmp[i] > tmp[j] ) {
			    val = tmp[i];
			    tmp[i] = tmp[j];
			    tmp[j] = val;
			    val = order[i];
			    order[i] = order[j];
			    order[j] = val;
			}
		    }
		}
		for( i=0;i<JOY_POV_NUMDIRS;i++ ) {
		    doPOVCal( pijd, i, order );
		}
	    }
	}
    }

    jsSetVJoyDData( (LPJOYDATA) &jd );

} /* jsReadRegistry */

/***************************************************************************
 * @doc INTERNAL
 *
 * @api LONG | DriverProc | The entry point for an installable driver.
 *		  
 * @parm DWORD | dwDriverId | For most messages, <p dwDriverId> is the DWORD
 *     value that the driver returns in response to a <m DRV_OPEN> message.
 *     Each time that the driver is opened, through the <f DrvOpen> API,
 *     the driver receives a <m DRV_OPEN> message and can return an
 *     arbitrary, non-zero value. The installable driver interface
 *     saves this value and returns a unique driver handle to the 
 *     application. Whenever the application sends a message to the
 *     driver using the driver handle, the interface routes the message
 *     to this entry point and passes the corresponding <p dwDriverId>.
 *     This mechanism allows the driver to use the same or different
 *     identifiers for multiple opens but ensures that driver handles
 *     are unique at the application interface layer.
 *
 *     The following messages are not related to a particular open
 *     instance of the driver. For these messages, the dwDriverId
 *     will always be zero.
 *
 *	   DRV_LOAD, DRV_FREE, DRV_ENABLE, DRV_DISABLE, DRV_OPEN
 *
 * @parm HANDLE	 | hDriver | This is the handle returned to the
 *     application by the driver interface.
 *	    
 * @parm WORD | wMessage | The requested action to be performed. Message
 *     values below <m DRV_RESERVED> are used for globally defined messages.
 *     Message values from <m DRV_RESERVED> to <m DRV_USER> are used for
 *     defined driver protocols. Messages above <m DRV_USER> are used
 *     for driver specific messages.
 *
 * @parm LONG | lParam1 | Data for this message.  Defined separately for
 *     each message
 *
 * @parm LONG | lParam2 | Data for this message.  Defined separately for
 *     each message
 *
 * @rdesc Defined separately for each message. 
 ***************************************************************************/
LRESULT _loadds FAR PASCAL DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT wMessage, LPARAM lParam1, LPARAM lParam2)
{
    switch (wMessage) {
    case JDD_GETNUMDEVS:
    {
        lstrcpy( szRegKey, (LPCSTR)lParam2 );
	/* now that we have a registry key name, go read the registry */
	jsReadRegistry();
	return (LRESULT) dwMaxJoysticks;
    }

    case JDD_GETDEVCAPS:
	return jsGetDevCaps( (UINT) dwDriverID-1, (LPJOYCAPS)lParam1, (UINT)lParam2);

    case JDD_GETPOS:
    	{
	    /*
	     * fake joyinfo struct with DWORDS, since that's what VJOYD expects
	     */
	    struct {
		DWORD wXpos;
		DWORD wYpos;
		DWORD wZpos;
		DWORD wButtons;
	    } ji32;
	    LPJOYINFO	pji;
	    LRESULT	rc;

	    rc = jsGetPos( (UINT) dwDriverID-1, (LPJOYINFO) &ji32 );
	    pji = (LPJOYINFO)lParam1;
	    if( pji != NULL ) {
		pji->wXpos = (WORD) ji32.wXpos;
		pji->wYpos = (WORD) ji32.wYpos;
		pji->wZpos = (WORD) ji32.wZpos;
		pji->wButtons = (WORD) ji32.wButtons;
	    }
	    return rc;
	}

    case JDD_GETPOSEX:
	/* make device ID zero-based and get position */
	return jsGetPosEx((UINT)dwDriverID-1, (LPJOYINFOEX)lParam1);
	break;

    case JDD_CONFIGCHANGED:
	/* configuration has been updated, go read it */
	jsReadRegistry();
	PostMessage( HWND_BROADCAST, uMsgId, 0, 0L );
	return MMSYSERR_NOERROR;

    case JDD_SETCALIBRATION:
	/* primitive artifical organism - obsolete in the new order */
	return MMSYSERR_NOERROR;

    case DRV_OPEN:
	/* lParam2 comes from mmsystem. one open per joystick device */
	if( (DWORD)lParam2 < dwMaxJoysticks ) {
	    return lParam2 + 1;
	} else {
	    return 0L;
	}

    case DRV_LOAD:
    case DRV_FREE:
    case DRV_CLOSE:
    case DRV_DISABLE:
	return 1L;

    case DRV_ENABLE:
	return bHasVJoyD;
	return 1L;

    case DRV_QUERYCONFIGURE:
    case DRV_CONFIGURE:
	/* we don't support this, configuration done by separate app */
	return 0L;

    case DRV_INSTALL:
    case DRV_REMOVE:
	return 1L;

    default:
	return DefDriverProc(dwDriverID, hDriver, wMessage, lParam1, lParam2);
    }

} /* DriverProc */
