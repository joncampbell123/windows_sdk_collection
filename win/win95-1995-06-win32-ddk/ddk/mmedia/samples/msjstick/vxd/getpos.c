/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1994 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  File:       getpos.c
 *  Content:	handles GetPos and GetPosEx requests
 */

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <debug.h>
#include "getpos.h"

#undef CURSEG               
#define CURSEG()                   PCODE

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG                        


/*
 * global data (pageable)
 */
DWORD	dwJoyPortList[MAX_JOYSTICKS] = {DEFAULT_JOY_PORT, DEFAULT_JOY_PORT};
DWORD	dwJoyPortCount = ONE_DUALPORT_JOYCNT;
DWORD	dwMaxAxes[MAX_JOYSTICKS] = {4, 2};

/*
 * local data
 */
static JOYDATA	jd;
static JOYPOS	deadZoneMin[MAX_JOYSTICKS];
static JOYPOS	deadZoneMax[MAX_JOYSTICKS];


static JOYPROCESSING	jproc[MAX_JOYSTICKS+1];

static BOOL	bHasDeadZone;
static DWORD	dwAxisCount[MAX_JOYSTICKS];
static DWORD	dwButtonNumber;

static DWORD	dwUpos;
static DWORD	dwVpos;
static DWORD	dwPOV = POV_UNDEFINED;

#ifdef  DEBUG
#define DPF(x)	Out_Debug_String( x )
void DPF_DW( DWORD dw )
{
    char	res[11];
    int		i;
    char	ch;

//    res[0] = '0';
//    res[1] = 'x';
    for( i=0;i<11;i++ ) {
	if( i == 0 ) {
	    res[i] = '0';
	} else if( i == 1 ) {
	    res[i] = 'x';
	} else if( i == 10 ) {
	    res[i] = 0;
	} else {
	    ch = (char) (dw & 0xf);
	    if( ch > 9 ) {
		ch = 'a' + ch -10;
	    } else {
		ch = '0' + ch;
	    }
	    res[11-i] = ch;
	    dw >>= 4;
	}
    }
//    res[10] = 0;
    DPF( res );
}

void DPF_INT( DWORD dw )
{
    char	res[32];
    int		dcnt;
    DWORD	tdw;
    char	ch;

    if( dw == 0 ) {
	res[0] = '0';
	res[1] = 0;
	DPF( res );
	return;
    }

    dcnt = 0;
    tdw = dw;
    while( tdw != 0 ) {
	tdw /= 10;
	dcnt++;
    }
    res[dcnt] = 0;
    while( dcnt != 0 ) {
	dcnt--;
	ch = (char) ((dw % 10) + '0');
	res[dcnt] = ch;
	dw /= 10;
    }
    DPF( res );
}
#else
#define DPF(x)
#define DPF_DW(x)
#define DPF_INT(x)
#endif

/* get the current button number */
#define BUTTON_NUMBER() ((dwButtonNumber == 0) ? \
	((dwButtons & JOY_BUTTON1) ? 1 : ((dwButtons & JOY_BUTTON2) ? 2: \
	((dwButtons & JOY_BUTTON3) ? 3 : ((dwButtons & JOY_BUTTON4) ? 4 : 0) \
	))) : dwButtonNumber)

/*
 * polling routines - either local or else stubs that call pExternalPoll
 */
int __stdcall jsPoll5( DWORD id, DWORD do_v ) { return 0; };
int __stdcall jsPoll6( DWORD id ) { return 0; };

/*
 * oemPoll1 - call OEM poll routine to poll any 1 axis
 */
static int __stdcall oemPoll1( DWORD id, DWORD axis )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    ojd.do_other = axis;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL1, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll1( id, axis );
    }

    return rc;

} /* oemPoll1 */

/*
 * oemPoll2 - call OEM poll routine to poll both X & Y axes of 1 joystick
 */
static int __stdcall oemPoll2( DWORD id )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL2, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwYpos = ojd.jp.dwY;
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll2( id );
    }
    return rc;

} /* oemPoll2 */

/*
 * oemPoll3 - call OEM poll routine to poll the X, Y and Z/R axes
 */
static int __stdcall oemPoll3( DWORD id, DWORD do_r )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    ojd.do_other = do_r;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL3, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwYpos = ojd.jp.dwY;
	if( !do_r ) {
	    dwZpos = ojd.jp.dwZ;
	} else {
	    dwRpos = ojd.jp.dwR;
	}
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll3( id, do_r );
    }
    return rc;

} /* oemPoll3 */

/*
 * oemPoll4 - call OEM poll routine to poll the X, Y, Z and R axes
 */
static int __stdcall oemPoll4( DWORD id )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL4, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwYpos = ojd.jp.dwY;
	dwZpos = ojd.jp.dwZ;
	dwRpos = ojd.jp.dwR;
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll4( id );
    }

    return rc;

} /* oemPoll4 */

/*
 * oemPoll5 - call OEM poll routine to poll 5 axes
 */
static int __stdcall oemPoll5( DWORD id, DWORD do_v )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    ojd.do_other = do_v;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL5, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwYpos = ojd.jp.dwY;
	dwZpos = ojd.jp.dwZ;
	dwRpos = ojd.jp.dwR;
	if( !do_v ) {
	    dwUpos = ojd.jp.dwU;
	} else {
	    dwVpos = ojd.jp.dwV;
	}
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll5( id, do_v );
    }

    return rc;

} /* oemPoll5 */

/*
 * oemPoll6 - call OEM poll routine to poll 6 axes
 */
static int __stdcall oemPoll6( DWORD id )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_POLL6, &ojd );
    if( rc == JOY_OEMPOLLRC_OK ) {
	dwXpos = ojd.jp.dwX;
	dwYpos = ojd.jp.dwY;
	dwZpos = ojd.jp.dwZ;
	dwRpos = ojd.jp.dwR;
	dwUpos = ojd.jp.dwU;
	dwVpos = ojd.jp.dwV;
	dwButtons = ojd.dwButtons;
	dwButtonNumber = ojd.dwButtonNumber;
	dwPOV = ojd.dwPOV;
    } else if( rc == JOY_OEMPOLLRC_YOUPOLL ) {
	rc = jsPoll6( id );
    }

    return rc;

} /* oemPoll6 */

/*
 * oemGetButtons - call OEM poll routine to poll the X, Y, Z and R axes
 */
static int __stdcall oemGetButtons( DWORD id )
{
    JOYOEMPOLLDATA	ojd;
    int			rc;

    ojd.id = id;
    rc = pVxDInfo[id]->pExternalPoll( JOY_OEMPOLL_GETBUTTONS, &ojd );
    dwButtons = ojd.dwButtons;

    return rc;

} /* oemGetButtons */

/*
 * InitJoyProcessing - initialize for using our polling routines
 */
void InitJoyProcessing( void )
{
    int	i;

    for( i=0;i<=MAX_JOYSTICKS;i++ ) {
	jproc[i].doPoll1 = jsPoll1;
	jproc[i].doPoll2 = jsPoll2;
	jproc[i].doPoll3 = jsPoll3;
	jproc[i].doPoll4 = jsPoll4;
	jproc[i].doPoll5 = jsPoll5;
	jproc[i].doPoll6 = jsPoll6;
	jproc[i].doGetButtons = jsGetButtons;
    }

} /* InitJoyProcessing */

/*
 * macros for setting various values
 */;
#define SET( jval ) \
    if( dwFlags & JOY_RETURN##jval ) { \
	if( dwFlags & JOY_RETURNRAWDATA ) { \
	    lpInfo->dw##jval##pos = dw##jval##pos; \
	} else { \
	    REAL_POS( lpInfo->dw##jval##pos, pijd, ##jval## ); \
	} \
    }

/*
 * SET_RZ - macro to set the R & Z values
 */
#define SET_RZ() \
{ \
    BOOL	do_r; \
    BOOL	do_z; \
 \
    /* \
     * if POV is not polling based, or if we've been asked to just \
     * return all axes, then return Z & R; else we use either the Z \
     * or the R value to compute the POV value later \
     */ \
    if( dwFlags & (JOY_CAL_READ4|JOY_CAL_READ5|JOY_CAL_READ6) ) { \
	do_r = TRUE; \
	do_z = TRUE; \
    } else { \
	if( pijd->pov_is_poll ) { \
	    if( !pijd->has_z ) { \
		do_r = TRUE; \
		do_z = FALSE; \
	    } else { \
		do_r = FALSE; \
		do_z = TRUE; \
	    } \
	} else { \
	    do_r = TRUE; \
	    do_z = TRUE; \
	} \
    } \
    if( do_r ) { \
	SET( R ); \
    } \
    if( do_z ) { \
	SET( Z ); \
    } \
 \
}

/*
 * validJoyID
 *
 * check if a given joystick ID is currently valid
 */
static __inline BOOL validJoyID( int joyid, BOOL read_always )
{
    if( joyid >= MAX_JOYSTICKS ) {
	return FALSE;
    }

    /*
     * if we are using our own polling, and we get asked for
     * the 2nd joystick on a dual port, then fail the request if
     * the 1rst joystick has more than 2 axes.
     */
    if( !jproc[joyid].bUsingMiniVxD ) {
	if( dwJoyPortCount == ONE_DUALPORT_JOYCNT ) {
	    if( joyid == JOYSTICKID2 ) {
		if( dwAxisCount[ JOYSTICKID1] > 2 ) {
		    return FALSE;
		}
	    } else if( joyid > JOYSTICKID2 ) {
		return FALSE;
	    }
	} else {
	    if( joyid > JOYSTICKID4 ) {
		return FALSE;
	    } else if( joyid > JOYSTICKID2 ) {
		if( dwAxisCount[ joyid-JOYSTICKID3 ] > 2 ) {
		    return FALSE;
		}
	    }
	}
    }

    /*
     * if read always is specified, then we succeed,
     * even if we think the joystick doesn't exist
     */
    if( !read_always ) {
	if( !jd.ijd[joyid].present ) {
	    return FALSE;
	}
	if( jd.ijd[joyid].hw.dwType == JOY_HW_NONE ) {
	    return FALSE;
	}
    }
    return TRUE;

} /* validJoyID */

/*
 * VJOYD_GetPosEx - get all joystick information, and allow customization
 *		    of how that information is obtained.
 */
int __stdcall VJOYD_GetPosEx( DWORD joyid, LPJOYINFOEX lpInfo )
{
    DWORD	pov;
    DWORD	val;
    DWORD	dwFlags;
    DWORD	axis_count;
    DWORD	max_axes;
    LPIJOYDATA	pijd;
    DWORD	denom;

    dwFlags = lpInfo->dwFlags;

    if( !validJoyID( joyid, dwFlags & JOY_CAL_READALWAYS ) ) {
	return JOYERR_PARMS;
    }

    dwButtonNumber = 0;
    axis_count = dwAxisCount[joyid];
    max_axes = dwMaxAxes[joyid];
    pijd = &jd.ijd[joyid];

    /*
     * zero result fields
     */
    lpInfo->dwButtons = 0;
    lpInfo->dwButtonNumber = 0;
    lpInfo->dwXpos = 0;
    lpInfo->dwYpos = 0;
    lpInfo->dwZpos = 0;
    lpInfo->dwRpos = 0;
    lpInfo->dwUpos = 0;
    lpInfo->dwVpos = 0;

    /*
     * set the correct port (for our own polling routine)
     */
    dwJoyPort = dwJoyPortList[joyid];

    /*
     * if we've been asked for just the buttons, then skip polling
     */
    if( (dwFlags & (JOY_RETURNBUTTONS|JOY_RETURNX|JOY_RETURNY|JOY_RETURNZ|
    	JOY_RETURNR|JOY_RETURNU|JOY_RETURNV|
	JOY_RETURNPOV|JOY_RETURNPOVCTS )) == JOY_RETURNBUTTONS ) {
	jproc[joyid].doGetButtons( joyid );
	lpInfo->dwButtons = dwButtons;
	lpInfo->dwButtonNumber = BUTTON_NUMBER();
	return JOYERR_NOERROR;
    }

    /*
     * if JOY_CAL_READ??ONLY is specified, then we just do that
     */
    if( dwFlags & (JOY_CAL_READRONLY|JOY_CAL_READZONLY|JOY_CAL_READUONLY|
    		JOY_CAL_READVONLY|JOY_CAL_READXYONLY|JOY_CAL_READXONLY|JOY_CAL_READYONLY) ) {
	/*
	 * we only return raw data from this, since we aren't calibrated in
	 * this scenario
	 */
	if( dwFlags & JOY_CAL_READXYONLY ) {
	    if( !jproc[joyid].doPoll1( joyid, JOY_AXIS_X ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwXpos = dwXpos;	
	    if( !jproc[joyid].doPoll1( joyid, JOY_AXIS_Y ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwYpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READXONLY ) {
	    if( !jproc[joyid].doPoll1( joyid, JOY_AXIS_X ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwXpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READYONLY ) {
	    if( !jproc[joyid].doPoll1( joyid, JOY_AXIS_Y ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwYpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READZONLY ) {
	    if( max_axes < 3 || !jproc[joyid].doPoll1( joyid, JOY_AXIS_Z ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwZpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READRONLY ) {
	    if( max_axes < 3 || !jproc[joyid].doPoll1( joyid, JOY_AXIS_R ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwRpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READUONLY ) {
	    if( max_axes < 5 || !jproc[joyid].doPoll1( joyid, JOY_AXIS_U ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwUpos = dwXpos;	
	} else if( dwFlags & JOY_CAL_READVONLY ) {
	    if( max_axes < 5 || !jproc[joyid].doPoll1( joyid, JOY_AXIS_V ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    lpInfo->dwVpos = dwXpos;	
	}
	return JOYERR_NOERROR;
    } else  {

	/*
	 * see if we need to read 5 or 6 axes
	 */
	if( (dwFlags & (JOY_CAL_READ5|JOY_CAL_READ6)) || (axis_count > 4 )) {
	    if( (dwFlags & (JOY_CAL_READ5)) || (axis_count == 5) ) {
		if( max_axes < 5 || !jproc[joyid].doPoll5( joyid, pijd->has_v ) ) {
		    return JOYERR_UNPLUGGED;
		}
		SET_RZ();
		if( pijd->has_v ) {
		    SET( V );
		} else {
		    SET( U );
		}
	    } else {
		if( max_axes < 6 || !jproc[joyid].doPoll6( joyid ) ) {
		    return JOYERR_UNPLUGGED;
		}
		SET_RZ();
		SET( U );
		SET( V );
	    }
	/*
	 * see if we need to read 4 axes
	 */
	} else if( (dwFlags & JOY_CAL_READ4) || (axis_count == 4) ) {
	    if( max_axes < 4 || !jproc[joyid].doPoll4( joyid ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    SET_RZ();
	/*
	 * see if we need to read 3 axes
	 */
	} else if( (dwFlags & JOY_CAL_READ3) || (axis_count == 3) ) {
	    if( max_axes < 3 || !jproc[joyid].doPoll3( joyid, pijd->has_r ) ) {
		return JOYERR_UNPLUGGED;
	    }
	    /*
	     * if POV isn't polling based, or if we've been asked to return
	     * the 3 axes, then return Z (or R); else we use the polled
	     * value to compute the POV value below
	     */
	    if( !pijd->pov_is_poll || (dwFlags & JOY_CAL_READ3) ) {
		if( pijd->has_r ) {
		    SET( R );
		} else {
		    SET( Z );
		}
	    }
	/*
	 * just read 2 axes
	 */
	} else {
	    if( !jproc[joyid].doPoll2( joyid ) ) {
		return JOYERR_UNPLUGGED;
	    }
	}
    }

    /*
     * Here is where we process the X & Y data
     *
     * if JOY_RETURNRAWDATA is specified, we don't massage the
     * polling results into the requested range
     *
     * if JOY_USEDEADZONE is specified, then we treat results that are
     * "near" the center as if they were at the center
     *
     * if JOY_RETURNCENTERED is specified, we make sure that the center
     * position of the joystick is actually the center of the range
     */
    if( dwFlags & JOY_RETURNRAWDATA ) {
	if( dwFlags & JOY_RETURNX ) {
	    lpInfo->dwXpos = dwXpos;
	}
	if( dwFlags & JOY_RETURNY ) {
	    lpInfo->dwYpos = dwYpos;
	}
    } else {
	if( (dwFlags & JOY_USEDEADZONE) && bHasDeadZone ) {
	    if( (dwXpos >= deadZoneMin[joyid].dwX && dwXpos <= deadZoneMax[joyid].dwX)
	     && (dwYpos >= deadZoneMin[joyid].dwY && dwYpos <= deadZoneMax[joyid].dwY) ) {
		dwXpos = pijd->hw.hwv.jrvHardware.jpCenter.dwX;
		dwYpos = pijd->hw.hwv.jrvHardware.jpCenter.dwY;
	    }
	}
	if( !(dwFlags & JOY_RETURNCENTERED) ) {
	    if( dwFlags & JOY_RETURNX ) {
		REAL_POS( lpInfo->dwXpos, pijd, X );
	    }
	    if( dwFlags & JOY_RETURNY ) {
		REAL_POS( lpInfo->dwYpos, pijd, Y );
	    }
	} else {
	    /*
	     * process Y data
	     */
	    if( dwFlags & JOY_RETURNY ) {
		int	cy;
		int	uy_range;
		int	y;
		int	ty;

		cy = pijd->hw.hwv.jrvHardware.jpCenter.dwY;
		uy_range = jd.cal_usr.dwY/2;

		/*
		 * y below center?
		 */
		if( (int) dwYpos < cy ) {
		    ty = pijd->hw.hwv.jrvHardware.jpMin.dwY;
		    y = (int) dwYpos-ty;
		    if( y < 0 ) {
			y = jd.usr.jrvRanges.jpMin.dwY;
		    } else {
			y *= uy_range;
			denom = (cy-ty);
			if( denom != 0 ) {
			    y /= denom;
			    y += jd.usr.jrvRanges.jpMin.dwY;
			} else {
			    y = -1;
			}
		    }
		/*
		 * no, y must be above center
		 */
		} else {
		    y = dwYpos-cy;
		    if( y < 0 ) {
			y = jd.usr.jrvRanges.jpMin.dwY+uy_range;
		    } else {
			y *= uy_range;
			denom = (pijd->hw.hwv.jrvHardware.jpMax.dwY-cy);
			if( denom != 0 ) {
			    y /= denom;
			    y += jd.usr.jrvRanges.jpMin.dwY+uy_range;
			} else {
			    y = -1;
			}
		    }
		}
		if( (DWORD) y > jd.usr.jrvRanges.jpMax.dwY ) {
		    lpInfo->dwYpos = jd.usr.jrvRanges.jpMax.dwY;
		} else {
		    lpInfo->dwYpos = (DWORD) y;
		}
	    }

	    /*
	     * process X data
	     */
	    if( dwFlags & JOY_RETURNX ) {
		int	cx;
		int	ux_range;
		int	x;
		int	tx;
    
		cx = pijd->hw.hwv.jrvHardware.jpCenter.dwX;
		ux_range = jd.cal_usr.dwX/2;

		/*
		 * x left of center?
		 */
		if( (int) dwXpos < cx ) {
		    tx = pijd->hw.hwv.jrvHardware.jpMin.dwX;
		    x = dwXpos-tx;
		    if( x < 0 ) {
			x = jd.usr.jrvRanges.jpMin.dwX;
		    } else {
			x *= ux_range;
			denom = (cx-tx);
			if( denom != 0 ) {
			    x /= denom;
			    x += jd.usr.jrvRanges.jpMin.dwX;
			} else {
			    x = -1;
			}
		    }
		/*
		 * no, must be right of center
		 */
		} else {
		    x = dwXpos-cx;
		    if( x < 0 ) {
			x = jd.usr.jrvRanges.jpMin.dwX+ux_range;
		    } else {
			x *= ux_range;
			denom = (pijd->hw.hwv.jrvHardware.jpMax.dwX-cx);
			if( denom != 0 ) {
			    x /= denom;
			    x += jd.usr.jrvRanges.jpMin.dwX+ux_range;
			} else {
			    x = -1;
			}
		    }
		}
		if( (DWORD) x > jd.usr.jrvRanges.jpMax.dwX ) {
		    lpInfo->dwXpos = jd.usr.jrvRanges.jpMax.dwX;
		} else {
		    lpInfo->dwXpos = (DWORD) x;
		}
	    }
	}
    }

    /*
     * return point of view data if requested
     */
    if( pijd->has_pov && (dwFlags & (JOY_RETURNPOV|JOY_RETURNPOVCTS) )) {
	if( dwPOV != POV_UNDEFINED ) {
	    pov = dwPOV;
	} else {
	    pov = JOY_POVCENTERED;
	    if( pijd->pov_is_poll ) {
		if( !pijd->has_z ) {
		    val = dwZpos;
		} else {
		    val = dwRpos;
		}
		/*
		 * figure out which direction this value indicates...
		 */
		if( (val > pijd->povcal[POV_MIN][JOY_POVVAL_FORWARD]) &&
		    (val < pijd->povcal[POV_MAX][JOY_POVVAL_FORWARD]) ) {
		    pov = JOY_POVFORWARD;
		} else if( (val > pijd->povcal[POV_MIN][JOY_POVVAL_BACKWARD]) &&
			   (val < pijd->povcal[POV_MAX][JOY_POVVAL_BACKWARD]) ) {
		    pov = JOY_POVBACKWARD;
		} else if( (val > pijd->povcal[POV_MIN][JOY_POVVAL_LEFT]) &&
			   (val < pijd->povcal[POV_MAX][JOY_POVVAL_LEFT]) ) {
		    pov = JOY_POVLEFT;
		} else if( (val > pijd->povcal[POV_MIN][JOY_POVVAL_RIGHT]) &&
			   (val < pijd->povcal[POV_MAX][JOY_POVVAL_RIGHT]) ) {
		    pov = JOY_POVRIGHT;
		}
	    } else {
		val = dwButtons;
		/*
		 * if no buttons are down (val == 0), then we don't want
		 * to pick an uninitialized wPOVValue by accident, so we set
		 * then button value to an invalid one
		 */
		if( val == 0 ) {
		    val = 0xffffffff;
		}
	
		/*
		 * see which POV value this button combo corrosponds to
		 */
		if( val == pijd->hw.hwv.dwPOVValues[JOY_POVVAL_FORWARD] ) {
		    pov = JOY_POVFORWARD;
		} else if( val == pijd->hw.hwv.dwPOVValues[JOY_POVVAL_BACKWARD] ) {
		    pov = JOY_POVBACKWARD;
		} else if( val == pijd->hw.hwv.dwPOVValues[JOY_POVVAL_LEFT] ) {
		    pov = JOY_POVLEFT;
		} else if( val == pijd->hw.hwv.dwPOVValues[JOY_POVVAL_RIGHT] ) {
		    pov = JOY_POVRIGHT;
		}
		if( pov != JOY_POVCENTERED ) {
		    dwButtons = 0;
		}
	    }
	}
	lpInfo->dwPOV = pov;
    }

    /*
     * return button settings if requested
     */
    if( dwFlags & JOY_RETURNBUTTONS ) {
	lpInfo->dwButtons = dwButtons;
	lpInfo->dwButtonNumber = BUTTON_NUMBER();
    }

    return JOYERR_NOERROR;

} /* VJOYD_GetPosEx */

/*
 * VJOYD_GetPos - get X,Y,Z joystick info - the original way of doing things
 */
int __stdcall VJOYD_GetPos( DWORD joyid, LPJOYINFO lpInfo )
{
    LPIJOYDATA	pijd;

    if( !validJoyID( joyid, FALSE ) ) {
	return JOYERR_PARMS;
    }
    pijd = &jd.ijd[joyid];

    /*
     * set the correct port
     */
    dwJoyPort = dwJoyPortList[ joyid ];

    /*
     * go get the results
     */
    if( pijd->has_z ) {
	if( dwMaxAxes[joyid] < 3 || !jproc[joyid].doPoll3( joyid, 0 ) ) {
	    return JOYERR_UNPLUGGED;
	}
	REAL_POS( lpInfo->wZpos, pijd, Z );
    } else {
	if( !jproc[joyid].doPoll2( joyid ) ) {
	    return JOYERR_UNPLUGGED;
	}
	lpInfo->wZpos = 0;
    }
    REAL_POS( lpInfo->wXpos, pijd, X );
    REAL_POS( lpInfo->wYpos, pijd, Y );
    lpInfo->wButtons = (WORD) dwButtons;

    return JOYERR_NOERROR;		  /* no error */

} /* VJOYD_GetPos */

/*
 * initPollingData - initialize data for using our built-in polling routines
 */
static void initPollingData( DWORD joyid )
{
    DWORD	hwflags;
    LPIJOYDATA	pijd;
    BOOL	doit;

    /*
     * set up for 1 or 2 port access
     */
    if( dwJoyPortCount == TWO_DUALPORT_JOYCNT ) {
	switch( joyid ) {
	case JOYSTICKID1:
	    dwMaxAxes[JOYSTICKID1] = MAX_JOYPORT_AXES;
	    break;
	case JOYSTICKID2:
	    dwMaxAxes[JOYSTICKID2] = MAX_JOYPORT_AXES;
	    break;
	case JOYSTICKID3:
	    dwMaxAxes[JOYSTICKID3] = MIN_JOYPORT_AXES;
	    break;
	case JOYSTICKID4:
	    dwMaxAxes[JOYSTICKID4] = MIN_JOYPORT_AXES;
	    break;
	default:
	    return;
	}
    } else {
	switch( joyid ) {
	case JOYSTICKID1:
	    dwMaxAxes[JOYSTICKID1] = MAX_JOYPORT_AXES;
	    break;
	case JOYSTICKID2:
	    dwMaxAxes[JOYSTICKID2] = MIN_JOYPORT_AXES;
	    break;
	default:
	    return;
	}
    }

    /*
     * initialize up what axis and poll bit information
     */
    dwAxisCount[joyid] = 2;
    cZbit[joyid] = 0;
    cRbit[joyid] = 0;
    pijd = &jd.ijd[joyid];

    hwflags = pijd->hw.hws.dwFlags;

    /*
     * initial settings for X & Y
     */
    cXbit[joyid] = JOY1_X_MASK;
    cYbit[joyid] = JOY1_Y_MASK;
    cXshift[joyid] = JOY1_X_SHIFT;
    cYshift[joyid] = JOY1_Y_SHIFT;
    cButtonShift[joyid] = JOY1_BUTTON_SHIFT;

    /*
     * See if this is a funky joystick where X and Y might be somewhere
     * else.
     *
     * Only do this if there is no secondary joystick associated with
     * the primary joystick on the port.
     */
    doit = FALSE;
    if( joyid == JOYSTICKID1 ) {
	if( dwJoyPortCount == TWO_DUALPORT_JOYCNT ) {
	    if( !jd.ijd[JOYSTICKID3].present ) {
		doit = TRUE;
	    }
	} else {
	    if( !jd.ijd[JOYSTICKID2].present ) {
		doit = TRUE;
	    }
	}
    } else if( joyid == JOYSTICKID2 ) {
	if( !jd.ijd[JOYSTICKID4].present ) {
	    doit = TRUE;
	}
    }
    if( doit ) {
	if( hwflags & JOY_HWS_XISJ2X ) {
	    cXbit[joyid] = JOY2_X_MASK;
	    cXshift[joyid] = JOY2_X_SHIFT;
	} else if( hwflags & JOY_HWS_XISJ2Y ) {
	    cXbit[joyid] = JOY2_Y_MASK;
	    cXshift[joyid] = JOY2_Y_SHIFT;
	} else if( hwflags & JOY_HWS_XISJ1Y ) {
	    cXbit[joyid] = JOY1_Y_MASK;
	    cXshift[joyid] = JOY1_Y_SHIFT;
	}
	if( hwflags & JOY_HWS_YISJ2X ) {
	    cYbit[joyid] = JOY2_X_MASK;
	    cYshift[joyid] = JOY2_X_SHIFT;
	} else if( hwflags & JOY_HWS_YISJ2Y ) {
	    cYbit[joyid] = JOY2_Y_MASK;
	    cYshift[joyid] = JOY2_Y_SHIFT;
	} else if( hwflags & JOY_HWS_YISJ1X ) {
	    cYbit[joyid] = JOY1_X_MASK;
	    cYshift[joyid] = JOY1_X_SHIFT;
	}
    }

    /*
     * POV info (if polled)
     */
    if( pijd->pov_is_poll ) {

	char	bit;
	char	shift;

	dwAxisCount[joyid]++;
	if( hwflags & JOY_HWS_POVISJ2X ) {
	    bit = JOY2_X_MASK;
	    shift = JOY2_X_SHIFT;
	} else if( hwflags & JOY_HWS_POVISJ1X ) {
	    bit = JOY1_X_MASK;
	    shift = JOY1_X_SHIFT;
	} else if( hwflags & JOY_HWS_POVISJ1Y ) {
	    bit = JOY1_Y_MASK;
	    shift = JOY1_Y_SHIFT;
	} else {
	    bit = JOY2_Y_MASK;
	    shift = JOY2_Y_SHIFT;
	}
	if( !pijd->has_z ) {
	    cZbit[joyid] = bit;
	    cZshift[joyid] = shift;
	} else {
	    cRbit[joyid] = bit;
	    cRshift[joyid] = shift;
	}
    }

    /*
     * Z info
     */
    if( pijd->has_z && cZbit[joyid] == 0 ) {
	dwAxisCount[joyid]++;
	if( hwflags & JOY_HWS_ZISJ2X ) {
	    cZbit[joyid] = JOY2_X_MASK;
	    cZshift[joyid] = JOY2_X_SHIFT;
	} else if( hwflags & JOY_HWS_ZISJ1X ) {
	    cZbit[joyid] = JOY1_X_MASK;
	    cZshift[joyid] = JOY1_X_SHIFT;
	} else if( hwflags & JOY_HWS_ZISJ1Y ) {
	    cZbit[joyid] = JOY1_Y_MASK;
	    cZshift[joyid] = JOY1_Y_SHIFT;
	} else {
	    cZbit[joyid] = JOY2_Y_MASK;
	    cZshift[joyid] = JOY2_Y_SHIFT;
	}
    }

    /*
     * R info (rudder or other 4th axis)
     */
    if( pijd->has_r && cRbit[joyid] == 0 ) {
	dwAxisCount[joyid]++;
	if( hwflags & JOY_HWS_RISJ2Y ) {
	    cRbit[joyid] = JOY2_Y_MASK;
	    cRshift[joyid] = JOY2_Y_SHIFT;
	} else if( hwflags & JOY_HWS_RISJ1X ) {
	    cRbit[joyid] = JOY1_X_MASK;
	    cRshift[joyid] = JOY1_X_SHIFT;
	} else if( hwflags & JOY_HWS_RISJ1Y ) {
	    cRbit[joyid] = JOY1_Y_MASK;
	    cRshift[joyid] = JOY1_Y_SHIFT;
	} else {
	    cRbit[joyid] = JOY2_X_MASK;
	    cRshift[joyid] = JOY2_X_SHIFT;
	}
    }

    if( pijd->has_u ) {
	dwAxisCount[joyid]++;
    }
    if( pijd->has_v ) {
	dwAxisCount[joyid]++;
    }

    /*
     * see if just a boring joystick implemented as the second half of a
     * 4 axis port
     */
    if( !((joyid == JOYSTICKID1) ||
	((joyid == JOYSTICKID2) && (dwJoyPortCount == TWO_DUALPORT_JOYCNT))) ) {
	cXbit[joyid] = JOY2_X_MASK;
	cYbit[joyid] = JOY2_Y_MASK;
	cXshift[joyid] = JOY2_X_SHIFT;
	cYshift[joyid] = JOY2_Y_SHIFT;
	cRbit[joyid] = 0;
	cZbit[joyid] = 0;
	cButtonShift[joyid] = JOY2_BUTTON_SHIFT;
    }

    /*
     * all bits to test
     */
    cAllBits[joyid] = cXbit[joyid] | cYbit[joyid] | cZbit[joyid] | cRbit[joyid];
    cXYBits[joyid] = cXbit[joyid] | cYbit[joyid];
    if( pijd->has_r ) {
	c3Bits[joyid] = cXYBits[joyid] | cRbit[joyid];
    } else {
	c3Bits[joyid] = cXYBits[joyid] | cZbit[joyid];
    }

} /* initPollingData */

/*
 * initExternalData - initialize data from the external VxD
 */
static void initExternalData( int joyid )
{
    JOYOEMHWCAPS	hwc;
    LPVXDINFO		pvi;

    pvi = pVxDInfo[joyid];
    if( pvi != NULL && pvi->pExternalHWCaps != NULL ) {
	if( pvi->pExternalHWCaps( joyid, &hwc ) ) {
	    dwAxisCount[joyid] = hwc.dwNumAxes;
	    dwMaxAxes[joyid] = hwc.dwMaxAxes;
	}
    }

} /* initExternalData */

/*
 * RegisterDeviceDriver - register an OEM joystick device driver
 */
void __stdcall RegisterDeviceDriver( JOYOEMPOLLRTN pollrtn,
				     CMCONFIGHANDLER cfgrtn,
				     JOYOEMHWCAPSRTN capsrtn,
				     JOYOEMJOYIDRTN joyidrtn )
{
    LPVXDINFO	pvi;

    dwPOV = POV_UNDEFINED;		/* default to POV not returned */

    /*
     * save the callback routines
     */
    pvi = pVxDInfo[dwOEMJoyId];
    if( pvi != NULL && pollrtn != NULL ) {
	DPF( "VJOYD: RegisterDeviceDriver, id=" ); DPF_INT( dwOEMJoyId );
	DPF( ",poll=" );
	DPF_DW( (DWORD) pollrtn );
	DPF( ",cfg=" );
	DPF_DW( (DWORD) cfgrtn );
	DPF( ",caps=" );
	DPF_DW( (DWORD) capsrtn );
	DPF( ",joyid=" );
	DPF_DW( (DWORD) joyidrtn );
	DPF( "\r\n" );
	pvi->pExternalPoll = pollrtn;
	pvi->pExternalCfg = cfgrtn;
	pvi->pExternalHWCaps = capsrtn;
	pvi->pExternalJoyId = joyidrtn;
    }

    if( pollrtn == NULL ) {
	/*
	 * external polling routine gone, use local ones
	 */
	jproc[dwOEMJoyId].doPoll1 = jsPoll1;
	jproc[dwOEMJoyId].doPoll2 = jsPoll2;
	jproc[dwOEMJoyId].doPoll3 = jsPoll3;
	jproc[dwOEMJoyId].doPoll4 = jsPoll4;
	jproc[dwOEMJoyId].doPoll5 = jsPoll5;
	jproc[dwOEMJoyId].doPoll6 = jsPoll6;
	jproc[dwOEMJoyId].doGetButtons = jsGetButtons;
	jproc[dwOEMJoyId].bUsingMiniVxD = FALSE;
	initPollingData( dwOEMJoyId );
    } else {
	jproc[dwOEMJoyId].doPoll1 = oemPoll1;
	jproc[dwOEMJoyId].doPoll2 = oemPoll2;
	jproc[dwOEMJoyId].doPoll3 = oemPoll3;
	jproc[dwOEMJoyId].doPoll4 = oemPoll4;
	jproc[dwOEMJoyId].doPoll5 = oemPoll5;
	jproc[dwOEMJoyId].doPoll6 = oemPoll6;
	jproc[dwOEMJoyId].doGetButtons = oemGetButtons;
	jproc[dwOEMJoyId].bUsingMiniVxD = TRUE;
	initExternalData( dwOEMJoyId );
    }

} /* RegisterDeviceDriver */

/*
 * VJOYD_GetHWCaps - get the hardware caps - maximum # of buttons and axes
 */
int __stdcall VJOYD_GetHWCaps( int joyid, LPJOYHWCAPS pjhwcaps )
{
    LPVXDINFO		pvi;

    if( !validJoyID( joyid, FALSE ) ) {
	return FALSE;
    }

    pvi = pVxDInfo[joyid];
    if( pvi != NULL && pvi->pExternalHWCaps != NULL ) {
	JOYOEMHWCAPS	hwc;
	if( pvi->pExternalHWCaps( joyid, &hwc ) ) {
	    pjhwcaps->dwMaxButtons = hwc.dwMaxButtons;
	    pjhwcaps->dwMaxAxes = hwc.dwMaxAxes;
	    pjhwcaps->dwNumAxes = hwc.dwNumAxes;
	    strcpy( pjhwcaps->szOEMVxD, pVxDInfo[joyid]->pszVxDName );
	} else {
	    return FALSE;
	}
    } else {
	pjhwcaps->szOEMVxD[0] = 0;
	if( joyid == JOYSTICKID1 ) {
	    pjhwcaps->dwMaxButtons = MAX_JOYPORT_BUTTONS;
	    pjhwcaps->dwMaxAxes = MAX_JOYPORT_AXES;
	    pjhwcaps->dwNumAxes = dwAxisCount[JOYSTICKID1];
	} else {
	    if( dwJoyPortCount == TWO_DUALPORT_JOYCNT ) {
		if( joyid == JOYSTICKID2 ) {
		    pjhwcaps->dwMaxButtons = MAX_JOYPORT_BUTTONS;
		    pjhwcaps->dwMaxAxes = MAX_JOYPORT_AXES;
		    pjhwcaps->dwNumAxes = dwAxisCount[JOYSTICKID2];
		} else if( joyid == JOYSTICKID3 ) {
		    pjhwcaps->dwMaxButtons = MIN_JOYPORT_BUTTONS;
		    pjhwcaps->dwMaxAxes = MIN_JOYPORT_AXES;
		    pjhwcaps->dwNumAxes = dwAxisCount[JOYSTICKID3];
		} else if( joyid == JOYSTICKID4 ) {
		    pjhwcaps->dwMaxButtons = MIN_JOYPORT_BUTTONS;
		    pjhwcaps->dwMaxAxes = MIN_JOYPORT_AXES;
		    pjhwcaps->dwNumAxes = dwAxisCount[JOYSTICKID4];
		} else {
		    return FALSE;
		}
	    } else {
		if( joyid == JOYSTICKID2 ) {
		    pjhwcaps->dwMaxButtons = MIN_JOYPORT_BUTTONS;
		    pjhwcaps->dwMaxAxes = MIN_JOYPORT_AXES;
		    pjhwcaps->dwNumAxes = dwAxisCount[JOYSTICKID2];
		} else {
		    return FALSE;
		}
	    }
	}
    }

    return TRUE;

} /* VJOYD_GetHWCaps */

/*
 * SetData - set up data for polling.   All information for how the
 * 		      devices are configured is passed here.
 */
void __stdcall SetData( LPJOYDATA pjd )
{
    DWORD	delta;
    DWORD	deadx;
    DWORD	deady;
    int		i;

    /*
     * since SetData is called when the registry is updated, we
     * should see if we need to load any alternate VxDs now
     */
    LoadOtherVxDs();

    /*
     * copy all the joystick data
     */
    jd = *pjd;

    /*
     * timeout for polling
     */
    dwTimeOut = jd.usr.dwTimeOut;

    /*
     * set up axis counts and polling bits (polling bits are specific
     * to our polling driver).
     */
    for( i=0;i<MAX_JOYSTICKS;i++ ) {
	if( !jproc[i].bUsingMiniVxD ) {
	    initPollingData( i );
	} else {
	    initExternalData( i );
	}
    }

    /*
     * compute the dead zone ranges if there is one
     */
    deadx = jd.usr.jpDeadZone.dwX;
    deady = jd.usr.jpDeadZone.dwY;
    bHasDeadZone = FALSE;
    if( deadx != 0 || deady != 0 ) {
	bHasDeadZone = TRUE;
	for( i=0;i<(int)MAX_JOYSTICKS;i++ ) {
	    delta = ((jd.ijd[i].hw.hwv.jrvHardware.jpMax.dwX-jd.ijd[i].hw.hwv.jrvHardware.jpMin.dwX)
		     * deadx)/100;
	    deadZoneMin[i].dwX = jd.ijd[i].hw.hwv.jrvHardware.jpCenter.dwX-delta;
	    deadZoneMax[i].dwX = jd.ijd[i].hw.hwv.jrvHardware.jpCenter.dwX+delta;
	    delta = ((jd.ijd[i].hw.hwv.jrvHardware.jpMax.dwY-jd.ijd[i].hw.hwv.jrvHardware.jpMin.dwY)
		     * deady)/100;
	    deadZoneMin[i].dwY = jd.ijd[i].hw.hwv.jrvHardware.jpCenter.dwY-delta;
	    deadZoneMax[i].dwY = jd.ijd[i].hw.hwv.jrvHardware.jpCenter.dwY+delta;
	}
    }

} /* SetData */
