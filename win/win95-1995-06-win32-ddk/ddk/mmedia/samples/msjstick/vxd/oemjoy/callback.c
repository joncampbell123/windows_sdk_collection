/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  CALLBACK.C
 *
 *  Notes:
 *
 *  File:       callback.c
 *  Content:	This file handles all callbacks from VJOYD.VXD
 */
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <debug.h>
#include "mm.h"
#include "vjoyd.h"

#undef CURSEG               
#define CURSEG()                   PCODE

#pragma VxD_LOCKED_DATA_SEG
#pragma VxD_PAGEABLE_CODE_SEG

#ifdef DEBUG
#define DPF(x)	Out_Debug_String( x )
void DPF_DW( DWORD dw )
{
    char	res[11];
    int		i;
    char	ch;
    res[0] = '0';
    res[1] = 'x';
    for( i=0;i<8;i++ ) {
	ch = (char) (dw & 0xf);
	if( ch > 9 ) {
	    ch = 'a' + ch -10;
	} else {
	    ch = '0' + ch;
	}
	res[9-i] = ch;
	dw >>= 4;
    }
    res[10] = 0;
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

#define MAX_JOYS_SUPPORTED	16
#define ADJUSTED_POS() (dwPos + (pd->id*10))

DWORD	dwButtons;
DWORD	dwPos;
DWORD	dwTimeOut = 3000;
DWORD	bPass1;

BOOL	bInUse[ MAX_JOYS_SUPPORTED ];

extern int __stdcall pollIt( DWORD id, DWORD do_y );	/* polling routine */

/*
 * PollRoutine - called by VJOYD to get device data
 */
int __stdcall PollRoutine( int type, LPJOYOEMPOLLDATA pd )
{
    int	rc = 0;

    /*
     * fail requests that are too big
     */
    if( pd->id >= MAX_JOYS_SUPPORTED ) {
	return rc;
    }
    if( !bInUse[pd->id] ) {
	DPF( "OEMJOY: joystick " );
	DPF_INT( pd->id );
	DPF( "not in use\r\n" );
    }
    DPF( "OEMJOY: PollRoutine, type=" ); DPF_INT( type ); DPF( "\r\n" );
    /*
     * note: x & y are inverted on purpose, so that we can tell
     * that this driver is getting called...
     */
    pd->dwPOV = POV_UNDEFINED;
    switch( type ) {
    case JOY_OEMPOLL_POLL1:
    	switch( pd->do_other ) {
	case JOY_AXIS_X:
	    rc = pollIt( 0, 0 );
	    break;
	case JOY_AXIS_Y:
	    rc = pollIt( 0, 1 );
	    break;
	case JOY_AXIS_Z:
	    rc = pollIt( 1, 1 );
	    break;
	case JOY_AXIS_R:
	    rc = pollIt( 1, 0 );
	    break;
	case JOY_AXIS_U:
	case JOY_AXIS_V:
	{
	    int	id;
	    if( pd->do_other == JOY_AXIS_U ) {
		id = 0;
	    } else {
		id = 1;
	    }
	    rc = pollIt( id, 0 );
	    if( rc ) {
		int	x;
		x = dwPos;
		rc = pollIt( id, 1 );
		if( rc ) {
		    dwPos += x;
		    dwPos /= 2;
		}
	    }
	    break;
	}
	pd->jp.dwX = ADJUSTED_POS();
	break;
	}
    case JOY_OEMPOLL_POLL2:
    	rc = pollIt( 0, 1 );
	if( rc ) {
	    pd->jp.dwX = ADJUSTED_POS();
	    rc = pollIt( 1, 0 );
	    if( rc ) {
		pd->jp.dwY = ADJUSTED_POS();
	    }
	}
	break;
    case JOY_OEMPOLL_POLL3:
    	rc = pollIt( 0, 1 );
	if( rc ) {
	    pd->jp.dwX = ADJUSTED_POS();
	    rc = pollIt( 0, 0 );
	    if( rc ) {
		pd->jp.dwY = ADJUSTED_POS();
		/*
		 * do_other indicates that we should get R instead of Z
		 */
		if( pd->do_other ) {
		    rc = pollIt( 1, 0 );
		    if( rc ) {
			pd->jp.dwR = ADJUSTED_POS();
		    }
		} else {
		    rc = pollIt( 1, 1 );
		    if( rc ) {
			pd->jp.dwZ = ADJUSTED_POS();
		    }
		}
	    }
	}
	break;
    case JOY_OEMPOLL_POLL4:
    case JOY_OEMPOLL_POLL5:
    case JOY_OEMPOLL_POLL6:
    	rc = pollIt( 0, 1 );
	if( rc ) {
	    pd->jp.dwX = ADJUSTED_POS();
	    rc = pollIt( 0, 0 );
	    if( rc ) {
		pd->jp.dwY = ADJUSTED_POS();
		rc = pollIt( 1, 1 );
		if( rc ) {
		    pd->jp.dwZ = ADJUSTED_POS();
		    rc = pollIt( 1, 0 );
		    if( rc ) {
			pd->jp.dwR = ADJUSTED_POS();
		    }
		}
	    }
	}
	/*
	 * do U & V support: U = (X+Y)/2, V = (Z+R)/2
	 */
	if( rc ) {
	    if( type == JOY_OEMPOLL_POLL5 ) {
		if( pd->do_other ) {
		    dwPos = (pd->jp.dwZ + pd->jp.dwR)/2;
		    pd->jp.dwV = ADJUSTED_POS();
		} else {
		    dwPos = (pd->jp.dwX + pd->jp.dwY)/2;
		    pd->jp.dwU = ADJUSTED_POS();
		}
	    } else if( type == JOY_OEMPOLL_POLL6 ) {
		dwPos = (pd->jp.dwX + pd->jp.dwY)/2;
		pd->jp.dwU = ADJUSTED_POS();
		dwPos = (pd->jp.dwZ + pd->jp.dwR)/2;
		pd->jp.dwV = ADJUSTED_POS();
	    }
	}
	break;
    case JOY_OEMPOLL_GETBUTTONS:
    	rc = pollIt( 0, 0 );
	break;
    }
    if( rc ) {
	pd->dwButtons = dwButtons;
    }
    return rc;

} /* PollRoutine */

/*
 * CfgRoutine - config manager message handler for PnP
 */
CONFIGRET CfgRoutine( CONFIGFUNC cfFuncName, SUBCONFIGFUNC scfSubFuncName,
                          DEVNODE dnToDevNode, ULONG dwRefData, ULONG ulFlags)
{
    DPF( "OEMJOY: CfgRoutine\r\n" );
    switch( cfFuncName ) {
    case CONFIG_FILTER:
	/*
	 * This command is sent to the devnode and all it's parents
	 * to give them a chance to delete logical configurations
	 * from the filtered configuration list.  A parent would
	 * remove a configuration from the filtered list if it cannot
	 * provide that configuration.
	 */
	return CR_SUCCESS;

    case CONFIG_START:

	switch( scfSubFuncName ) {
	case CONFIG_START_FIRST_START:
	case CONFIG_START_DYNAMIC_START:
	{
	    /*
	     * This command is sent to instruct the driver that
	     * it can start using the resources that have been
	     * allocated to it.	 This is the meat of this
	     * DriverConfigMan.
	     *
	     * We get our resource allocation by calling the
	     * configuration manager (CM) using the devnode that
	     * was passed to us.
	     *
	     * If all has gone well, we should not have been given
	     * a resource allocation that we can't use.	 Ideally
	     * this command should always succeed.  However, every
	     * now and then life throws you a curve ball, so we
	     * should still verify that we are happy with the
	     * resources we have been given.
	     *
	     * If we receive multiple _STARTs for multiple devodes,
	     * then there are multiple taget devices for us to drive.
	     * How we handle this is highly dependent upon the driver.
	     * This particular driver can only drive one target device.
	     */

	    CMCONFIG    ccb;        /* config buffer */
	    CONFIGRET   cr;         /* config man return code */

	    /*
	     * Get our resource allocation from CM.  if this fails,
	     * we have no choice but to fail the CONFIG_START.
	     * We'll return the same error that the CM_Get_Alloc_Conf
	     * returned.
	     */
	
	    cr = CM_Get_Alloc_Log_Conf( &ccb, dnToDevNode, 0 );
	    if( cr != CR_SUCCESS ) {
		return cr;
	    }

	    /*
	     * We need 1 io base of length 1, and it must be port 0x201
	     */
	    if( (ccb.wNumIOPorts < 1) || (ccb.wIOPortLength[0] < 1) ||
	    	(ccb.wIOPortBase[0] != 0x0201) ) {
		/*
		 * No way we can use this config!
		 */
		return CR_FAILURE;
	    }
	    
	    return CR_SUCCESS;
		
	}
	
	default:
	    return CR_DEFAULT;
	}
	    
    case CONFIG_TEST:
	/*
	 * This command is sent to query the driver whether it can
	 * change configuration, stop using the resources, or
	 * handle a remove.  The query is specified by the sub func.
	 */
	switch( scfSubFuncName ) {
	case CONFIG_TEST_CAN_STOP:
	    /*
	     * This command asks: Can we stop using our resources?
	     */
	    return CR_SUCCESS;

	case CONFIG_TEST_CAN_REMOVE:
	    /*
	     * This command asks: Is it OK for CM to remove the devnode?
	     * This implies that the associated device is also going to
	     * disappear (or has already disappeared).
	     */
	    return CR_SUCCESS;

	default:
	    return CR_DEFAULT;
	}

    case CONFIG_STOP:
	switch( scfSubFuncName ) {
	case CONFIG_STOP_DYNAMIC_STOP:
	    /*
	     * This command indicates that we should stop
	     * using the resources that were allocated to us.
	     *
	     * The device that this driver is driving can't really stop using
	     * it's resources. But the driver should do its best to stop using
	     * the device and any hardware resources.
	     */
	    return CR_SUCCESS;

	default:
	    return CR_DEFAULT;
	}

    case CONFIG_REMOVE:
	/*
	 * This command is sent to a devnode and all it's parents when
	 * when the devnode is being removed from the hardware tree.
	 * Deallocate any per devnode instance data and take whatever
	 * other measures are necessary to stop using that devnode and
	 * the associated hardware. The command is essentially saying
	 * that the hardware no longer exists in the system.
	 */
	return CR_SUCCESS;

    default:
	return CR_DEFAULT;
    }

} /* CfgRoutine */

/*
 * HWCapsRoutine - get caps of hardware
 */
int __stdcall HWCapsRoutine( int joyid, LPJOYOEMHWCAPS pjhwc )
{
    pjhwc->dwMaxButtons = 4;
    pjhwc->dwMaxAxes = 6;
    pjhwc->dwNumAxes = 6;
    return TRUE;

} /* HWCapsRoutine */

/*
 * JoyIdRoutine - keep track of what joystick IDs we handle
 */
int __stdcall JoyIdRoutine( int joyid, BOOL used )
{
    if( joyid >= MAX_JOYS_SUPPORTED ) {
	return FALSE;
    }
    bInUse[joyid] = used;
    return TRUE;

} /* JoyIdRoutine */
