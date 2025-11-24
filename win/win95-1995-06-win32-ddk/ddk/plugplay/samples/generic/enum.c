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
 *  ENUM.C - NewEnumerator Support for SAMPLE
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
#include <enum.h>

#pragma CM_PAGEABLE_DATA

#pragma CM_PAGEABLE_CODE

#pragma warning (disable:4100)	// Parameter unused

/****************************************************************************
 *
 *	EnumHandler - Handler of the config manager enumeration call
 *
 *	Exported.
 *
 *	ENTRY:	Standard enumerator handler.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_HANDLER
EnumHandler(CONFIGFUNC cfFuncName, SUBCONFIGFUNC sbfSubFuncName, DEVNODE dnToDevNode, DEVNODE dnAboutDevNode, ULONG ulFlags)
{
	LOG_CONF	lc;
	RES_DES		rd;
	CONFIGRET	status;

	switch (cfFuncName) {

		case CONFIG_FILTER:

			// Get the first filtered logical configuration.

			status=CM_Get_First_Log_Conf(	&lc,
							dnAboutDevNode,
							FILTERED_LOG_CONF);
			// For all logical configuration (we stopped when
			// the status is not CR_SUCCESS since it can only
			// become CR_NO_MORE_LOG_CONF).

			while (status==CR_SUCCESS) {

				// For all resource descriptors

				rd=(RES_DES)lc;
				while (CM_Get_Next_Res_Des(	&rd,
							   	rd,
								ResType_Mem,
								NULL, 0)==CR_SUCCESS) {

					// For all memory resource
					// (for instance) do some
					// checking (like ISA impose
					// <16Meg)

				}
				
				// Next logical configuration.

				status=CM_Get_Next_Log_Conf(	&lc,
								lc,
								0);

			}

			return(CR_SUCCESS);

		case CONFIG_START:
		case CONFIG_STOP:
		case CONFIG_TEST:
			return(CR_SUCCESS);

		case CONFIG_ENUMERATE:

			return(CR_SUCCESS);

		case CONFIG_REMOVE:
			return(CR_SUCCESS);

		default:
			return(CR_DEFAULT);
	}
}

#pragma warning (default:4100)	// Parameter unused

/****************************************************************************
 *
 *	NewEnumerator - Register a new enumerator for the new devnode
 *
 *	Exported.
 *
 *	ENTRY:	DevNode is the new devnode that has just been created.
 *
 *	EXIT:	Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_INTERNAL
NewEnumerator(DEVNODE DevNode)
{
	CONFIGRET	result;

	result=CM_Register_Enumerator(DevNode, EnumHandler, 0);

	return(result);
}
