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
 *  DEVLOAD.C - NewDevLoader Support for SAMPLE
 *
 *  Notes:
 *
 *	This file processes the DevLoader requests
 */

#include <basedef.h>
#include <vmm.h>
#include <debug.h>
#include <vmmreg.h>
#include <vxdwraps.h>
#include <configmg.h>
#include <devload.h>

#pragma CM_PAGEABLE_DATA

#pragma CM_PAGEABLE_CODE

/****************************************************************************
 *
 *	NewDevLoader - Loads the enumerator and driver for a new DevNode
 *
 *	Exported.
 *
 *	ENTRY:	DevNode is the new devnode that has just been created.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_INTERNAL
NewDevLoader(DEVNODE DevNode)
{
	CONFIGRET	result;

	result=CM_Load_DLVxDs(	DevNode,
				"\\chicago\\dos\\dos386\\vxd\\plugplay\\sample\\maxdebug\\sample.386",
				DLVXD_LOAD_ENUMERATOR,
				0);

	// If despite its best effort configuration manager cannot load the
	// enumerator, we just leave right away.

	if (result!=CR_SUCCESS)
		return(result);

	// Loading the enumerator was successfull. This is where some
	// devloader would talk directly to an enumerator if needed.

	result=CM_Load_DLVxDs(	DevNode,
				"\\chicago\\dos\\dos386\\vxd\\plugplay\\sample\\maxdebug\\sample.386",
				DLVXD_LOAD_DRIVER,
				0);

	// Our job as a DevLoader is finished, so we can leave this instance.

	return(result);

}
