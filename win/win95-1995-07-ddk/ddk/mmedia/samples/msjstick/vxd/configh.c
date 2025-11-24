/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 *  File: 	configh.c
 *  Content:	This file processes all the Config Manager messages.
 *
 */

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <debug.h>

#include "getpos.h"

#undef CURSEG               
#define CURSEG()                   PCODE

#ifdef  DEBUG
#define DPF(x)	Out_Debug_String(x)
extern void DPF_DW( DWORD );
extern void DPF_INT( DWORD );
#else
#define DPF(x)
#define DPF_DW(x)
#define DPF_INT(x)
#endif

#pragma VxD_PAGEABLE_CODE_SEG
#pragma VxD_PAGEABLE_DATA_SEG                        
/****************************************************************************
 *
 *	Config_Handler: This procedure handles all the mmdevldr
 *      configmg messages,
 *
 *	ENTRY:	Same entry parameters as a standard config handler
 *
 *	EXIT:	configmg return code.
 *
 ***************************************************************************/
CONFIGRET Config_Handler( CONFIGFUNC cfFuncName, SUBCONFIGFUNC scfSubFuncName,
                          DEVNODE dnToDevNode, ULONG dwRefData, ULONG ulFlags)
{
    int		i;
    LPVXDINFO	pvi;
    CONFIGRET	crc;
    BOOL	othervxd_found;


    /*
     * call config handlers for all loaded VxDs
     */
    othervxd_found = FALSE;
    for( i=0;i<iNumVxDs;i++ ) {
	pvi = VxDUsage[i].pvi;
	if( pvi->pExternalCfg != NULL ) {
	    othervxd_found = TRUE;
	    crc = pvi->pExternalCfg( cfFuncName, scfSubFuncName, dnToDevNode,
				    dwRefData, ulFlags );
	    /*
	     * note!!  OEMVxDs should NOT reject configs they don't understand
	     * Other OEMVxDs may need the info!   We return failures here,
	     * but they had better be life or death failures!
	     */
	    if( crc != CR_SUCCESS && crc != CR_DEFAULT ) {
		return crc;
	    }
	}
    }

    /*
     * if there is a VxD for each of the joysticks we can handle, then
     * we return; otherwise, check config info for the ones we can handle
     */
    if( pVxDInfo[JOYSTICKID1] != NULL && pVxDInfo[JOYSTICKID2] != NULL &&
    	pVxDInfo[JOYSTICKID3] != NULL && pVxDInfo[JOYSTICKID4] != NULL &&
	othervxd_found ) {
	return crc;
    }

    /*
     * handle this config message ourselves
     */
    switch( cfFuncName ) {
    case CONFIG_FILTER:
	/*
	 * This command is sent to the devnode and all it's parents
	 * to give them a chance to delete logical configurations
	 * from the filtered configuration list.  A parent would
	 * remove a configuration from the filtered list if it cannot
	 * provide that configuration.
	 */
	DPF("VJOYD: CONFIG_FILTER\r\n");
	return CR_SUCCESS;

    case CONFIG_START:
	DPF("VJOYD: CONFIG_START\r\n");

	switch( scfSubFuncName ) {
	case CONFIG_START_FIRST_START:
	    DPF("VJOYD: CONFIG_START_FIRST_START\r\n");
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
	     * we can use either 1 or 2 IO port ranges.   The range must be
	     * of length 1, and it must be within 0x201-0x20f
	     */
	    DPF( "VJOYD: ccb.wNumIOPorts = " ); DPF_INT( ccb.wNumIOPorts ); DPF( "\r\n" );
	    DPF( "VJOYD: ccb.wIOPortLength[0] = " ); DPF_INT( ccb.wIOPortLength[0] ); DPF( "\r\n" );
	    DPF( "VJOYD: ccb.wIOPortBase[0] = " ); DPF_DW( ccb.wIOPortBase[0] ); DPF( "\r\n" );
	    if( ccb.wNumIOPorts >= 1 ) {
//		Ignore port length (for PnP BIOS). it isn't that important -
//		we can use the base just fine.
//		if( (ccb.wIOPortLength[0] != 1) ||
		if( (ccb.wIOPortBase[0] < MIN_JOY_PORT ||
		     ccb.wIOPortBase[0] > MAX_JOY_PORT) ) {
		    if( othervxd_found ) {
			return crc;
		    } else {
			return CR_FAILURE;
		    }
		}
		/*
		 * only allow this if we don't have a joystick 1 or 2 yet.
		 */
		if( pVxDInfo[JOYSTICKID1] == NULL && pVxDInfo[JOYSTICKID2] == NULL ) {
		    dwJoyPortCount = ONE_DUALPORT_JOYCNT;
		    dwJoyPort = (DWORD) ccb.wIOPortBase[0];
		    dwJoyPortList[JOYSTICKID1] = dwJoyPort;
		    dwJoyPortList[JOYSTICKID2] = dwJoyPort;
		    dwMaxAxes[JOYSTICKID1] = MAX_JOYPORT_AXES;
		    dwMaxAxes[JOYSTICKID2] = MIN_JOYPORT_AXES;
		}

		/*
		 * for the second one, we don't fail if we get an invalid
		 * result so that if we are a child we still work
		 */
		if( ccb.wNumIOPorts == 2 ) {
		    DPF( "VJOYD: ccb.wIOPortLength[1] = " ); DPF_INT( ccb.wIOPortLength[1] ); DPF( "\r\n" );
		    DPF( "VJOYD: ccb.wIOPortBase[1] = " ); DPF_DW( ccb.wIOPortBase[1] ); DPF( "\r\n" );
		    if( (ccb.wIOPortLength[1] == 1) &&
			(ccb.wIOPortBase[1] >= MIN_JOY_PORT &&
			 ccb.wIOPortBase[1] <= MAX_JOY_PORT) ) {
			if( pVxDInfo[JOYSTICKID3] == NULL && pVxDInfo[JOYSTICKID4] == NULL ) {
			    DWORD	port;
			    dwJoyPortCount = TWO_DUALPORT_JOYCNT;
			    port = (DWORD) ccb.wIOPortBase[1];
			    dwJoyPortList[JOYSTICKID2] = port;
			    dwJoyPortList[JOYSTICKID3] = dwJoyPort;
			    dwJoyPortList[JOYSTICKID4] = port;
			    dwMaxAxes[JOYSTICKID2] = MAX_JOYPORT_AXES;
			    dwMaxAxes[JOYSTICKID3] = MIN_JOYPORT_AXES;
			    dwMaxAxes[JOYSTICKID4] = MIN_JOYPORT_AXES;
			}
		    }
		}
	    } else {
		if( othervxd_found ) {
		    return crc;
		} else {
		    return CR_FAILURE;
		}
	    }

	    #ifdef DEBUG
		/*
		 * test if we have a joysticks connected or not
		 */
		DPF( "VJOYD: trying PORT 1=" ); DPF_DW( dwJoyPort ); DPF( "\r\n" );
		if( !jsPoll1( 0,0 ) ) {
		    DPF( "VJOYD: joystick 1 not there\r\n" );
		}
		if( dwJoyPortCount == TWO_DUALPORT_JOYCNT ) {
		    dwJoyPort = dwJoyPortList[JOYSTICKID2];
		    DPF( "VJOYD: trying PORT 2=" ); DPF_DW( dwJoyPort ); DPF( "\r\n" );
		    if( !jsPoll1( 0,0 ) ) {
			DPF( "VJOYD: joystick 2 not there\r\n" );
		    }
		    dwJoyPort = dwJoyPortList[JOYSTICKID1];
		    DPF( "VJOYD: 2 joystick ports present\r\n" );
		}
	    #endif
	    
	    DPF("VJOYD: joystick detected\r\n");
	    return CR_SUCCESS;
		
	}
	
	default:
	    DPF("VJOYD: unhandled CONFIG_START scfSubFuncName:"); DPF_INT( scfSubFuncName ); DPF("\r\n");
	    return CR_DEFAULT;
	}
	    
    case CONFIG_TEST:
	/*
	 * This command is sent to query the driver whether it can
	 * change configuration, stop using the resources, or
	 * handle a remove.  The query is specified by the sub func.
	 */
	DPF("VJOYD: CONFIG_TEST\r\n");

	switch( scfSubFuncName ) {
	case CONFIG_TEST_CAN_STOP:
	    /*
	     * This command asks: Can we stop using our resources?
	     */
	    DPF("VJOYD: CONFIG_TEST_CAN_STOP\r\n");
	    return CR_SUCCESS;

	case CONFIG_TEST_CAN_REMOVE:
	    /*
	     * This command asks: Is it OK for CM to remove
	     * the devnode?  This implies that the associated
	     * device is also going to disappear (or has
	     * already disappeared).
	     */
	    DPF("VJOYD: CONFIG_TEST_CAN_REMOVE\r\n");
	    return CR_SUCCESS;

	default:
	    DPF("VJOYD: unhandled CONFIG_TEST scfSubFuncName:"); DPF_INT( scfSubFuncName ); DPF("\r\n");
	    return CR_DEFAULT;
	}

    case CONFIG_STOP:
	switch( scfSubFuncName ) {
	case CONFIG_STOP_DYNAMIC_STOP:
	    /*
	     * This command indicates that we should stop
	     * using the resources that were allocated to us.
	     *
	     * The device that this driver is driving can't
	     * really stop using it's resources. But the driver
	     * should do its best to stop using the device and
	     * any hardware resources.
	     */
	    DPF("VJOYD: CONFIG_STOP_DYNAMIC_STOP\r\n");
	    return CR_SUCCESS;

	default:
	    DPF("VJOYD: unhandled CONFIG_STOP scfSubFuncName:"); DPF_INT( scfSubFuncName ); DPF("\r\n");
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
	DPF("VJOYD: CONFIG_REMOVE\r\n");
	return CR_SUCCESS;

    default:
	DPF("VJOYD: unhandled cfFuncName:"); DPF_INT( cfFuncName ); DPF("\r\n");
	return CR_DEFAULT;
    }

} /* Config_Handler */
