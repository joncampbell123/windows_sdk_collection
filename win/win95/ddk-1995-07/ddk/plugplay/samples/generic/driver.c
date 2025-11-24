/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*
 *  DRIVER.C - NewDriver Support for SAMPLE
 *
 *  Notes:
 *
 *	This file processes the Driver requests
 */

#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <driver.h>

#pragma CM_PAGEABLE_DATA

#pragma CM_PAGEABLE_CODE

IO_RESOURCE	Res1=
{
 {2, IOType_Range, 0, 0, 0, 0, 0},
 {
	{0xFFFF, 1, 0x103, 0x103, 0, 0xFF, 0xFF},
	{0xFFFF, 1, 0x203, 0x203, 0, 0xFF, 0xFF},
 }
};

IO_RESOURCE	Res2=
{
 {2, IOType_Range, 0, 0, 0, 0, 0},
 {
	{0x0300, 6, 0x200, 0x307, 0, 3, 3},
	{0x03F0, 4, 0x300, 0x313, 0, 3, 3},
 }
};

#pragma warning (disable:4100)	// Parameter unused

/****************************************************************************
 *
 *	ConfigHandler - Handler of the config manager config call
 *
 *	Exported.
 *
 *	ENTRY:	Standard config handler.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_HANDLER
ConfigHandler(CONFIGFUNC cfFuncName, SUBCONFIGFUNC scfSubFuncName, DEVNODE dnToDevNode, DWORD dwRefData, ULONG ulFlags)
{
	switch (cfFuncName) {

		case CONFIG_START:

			// You may or may not treat CONFIG_START_FIRST_START
			// different from CONFIG_START_DYNAMIC_START.

			// As far as CM goes, the first time it would
			// have called with CONFIG_START_DYNAMIC_START, 
			// it will call with CONFIG_START_FIRST_START
			// instead.

			return(CR_SUCCESS);

		case CONFIG_TEST:
			return(CR_SUCCESS);

		case CONFIG_STOP:
			return(CR_SUCCESS);

		case CONFIG_REMOVE:
			// deallocate per instance data.

			return(CR_SUCCESS);

		default:
			return(CR_DEFAULT);
	}
}

#pragma warning (default:4100)		// Parameter unused

/****************************************************************************
 *
 *	NewDriver - Register a new driver for the new devnode
 *
 *	Exported.
 *
 *	ENTRY:	DevNode is the new devnode that has just been created.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_INTERNAL
NewDriver(DEVNODE DevNode)
{
	LOG_CONF	lc;
	RES_DES		rd1, rd2;

	// Any failure should use LeaveInstance to unload the DLVxD if
	// it is the only instance. Only if it is success will we not
	// return with an "OR"ed CR_UNLOAD.

	// BUGBUG error checking

	CM_Add_Empty_Log_Conf(&lc, DevNode, 0, BASIC_LOG_CONF);

	CM_Add_Res_Des(&rd1, lc, ResType_IO, &Res1, SIZEOF_IORANGE(2), 0);

	CM_Add_Res_Des(&rd2, lc, ResType_IO, &Res2, SIZEOF_IORANGE(2), 0);

	CM_Register_Device_Driver(DevNode, ConfigHandler, 0, 0);

	// Allocate per instance data

	return(CR_SUCCESS);
}
