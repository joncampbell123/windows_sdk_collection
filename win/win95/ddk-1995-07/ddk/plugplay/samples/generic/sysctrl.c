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
 *  SYSCTRL.C - System Control Message Support for SAMPLE
 *
 *  Notes:
 *
 *	This file processes all the control messages sent by VMM.
 */

#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <driver.h>
#include <enum.h>
#include <devload.h>

#pragma CM_PAGEABLE_DATA

#pragma CM_PAGEABLE_CODE

/****************************************************************************
 *
 *	SAMPLE_Init - This procedure gets called the first time the DLVxD
 *		      is loaded
 *
 *	Exported.
 *
 *	ENTRY:	pDDB which is NULL is dynamically loaded.
 *
 *	EXIT:	VXD_SUCCESS or VXD_FAILURE.
 *
 ***************************************************************************/
CM_VXD_RESULT CM_SYSCTRL
SAMPLE_Init(PVMMDDB pDDB)
{
	// This procedure normally does allocate/initialize global data (like a
	// global table of COM ports or PNP ids).

	CM_Register_DevLoader(pDDB, 0);

	return(VXD_SUCCESS);
}

/****************************************************************************
 *
 *	SAMPLE_Exit - This procedure gets called when the DLVxD is unloading
 *
 *	Exported.
 *
 *	ENTRY:	None.
 *
 *	EXIT:	VXD_SUCCESS or VXD_FAILURE.
 *
 ***************************************************************************/
CM_VXD_RESULT CM_SYSCTRL
SAMPLE_Exit(VOID)
{
	// This procedure must deallocate any global data allocated by 
	// SAMPLE_Init (like a global table of COM ports or ISA RTR ids).
	// SAMPLE will be unloaded by Configuration Manager when
	// the ConfigHandler or the EnumHandler or the NewDevNode return
	// an error code that is "OR"ed with CR_UNLOAD.

	return(VXD_SUCCESS);
}

/****************************************************************************
 *
 *	SAMPLE_NewDevNode - This procedure gets called when a new devnode
 *			    has been created
 *
 *	Exported.
 *
 *	ENTRY:	DevNode is the new devnode that has just been created.
 *
 *		LoadType is the type of functionality configuration manager
 *		(or a devloader) wants this DLVxD to handle. It can be
 *		devloader, driver or enumerator.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_SYSCTRL
SAMPLE_NewDevNode(DEVNODE Devnode, LOAD_TYPE LoadType)
{
	// A DLVxD can be any combination of being a devloader, a driver or
	// an enumerator.
	//
	// A DevLoader's job is to load the enumerator and driver for a
	// devnode (using CM_Load_DLVxD) and usually maintain some info
	// on the enumerator/driver it loaded.
	//
	// An enumerator's job is to do a CM_Register_Enumerator to
	// install an handler that will be called to enumerate children.
	//
	// A driver's job is to make the devnode work, which means installing
	// a configuration handler with CM_Register_Device_Driver.
	//
	// This sample code does all three functions.

	switch (LoadType) {

		case DLVXD_LOAD_ENUMERATOR:
			return(NewEnumerator(Devnode));

		case DLVXD_LOAD_DRIVER:
			return(NewDriver(Devnode));

		case DLVXD_LOAD_DEVLOADER:
			return(NewDevLoader(Devnode));

		default:
			return(CR_DEFAULT);
	}
}
