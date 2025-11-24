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
 *  SYSCTRL.C - System Control Message Support for EXAMENUM
 *
 *  Notes:
 *
 *  This file processes all the control messages sent by VMM.
 */

#include "pch.h"

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

/****************************************************************************
 *
 *  EXAMPENUM_Init - This procedure gets called the first time the DLVxD
 *                   is loaded
 *
 *  Exported.
 *
 *  ENTRY:  DevInf is a pointer to the VXDLDR device info packet.
 *
 *  EXIT:   VXD_SUCCESS or VXD_FAILURE.
 *
 ***************************************************************************/
CM_VXD_RESULT CM_SYSCTRL
EXAMENUM_Init(VOID)
{
    return(VXD_SUCCESS);
}

/****************************************************************************
 *
 *  EXAMENUM_Exit - This procedure gets called when the DLVxD is unloading
 *
 *  Exported.
 *
 *  ENTRY:  DevInf is a pointer to the VXDLDR device info packet.
 *
 *      DDB is a pointer to the DDB that was created for this device.
 *
 *  EXIT:   VXD_SUCCESS or VXD_FAILURE.
 *
 ***************************************************************************/
CM_VXD_RESULT CM_SYSCTRL
EXAMENUM_Exit(VOID)
{
    // This procedure must deallocate any global data allocated by 
    // EXAMENUM_Init.
    // EXAMENUM will be unloaded by Configuration Manager when
    // the ConfigHandler or the EnumHandler or the NewDevNode return
    // an error code that is "OR"ed with CR_UNLOAD.

    return(VXD_SUCCESS);
}

/****************************************************************************
 *
 *  EXAMENUM_NewDevNode - This procedure gets called when a new devnode
 *                        has been created
 *
 *  Exported.
 *
 *  ENTRY:  DevNode is the new devnode that has just been created.
 *
 *      LoadType is the type of functionality configuration manager
 *      (or a devloader) wants this DLVxD to handle. It can be
 *      devloader, driver or enumerator.
 *
 *  EXIT:   Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_SYSCTRL
EXAMENUM_NewDevNode(DEVNODE Devnode, LOAD_TYPE LoadType)
{
    // An enumerator's job is to do a CM_Register_Enumerator to
    // install a handler that will be called to enumerate children.

    if (LoadType == DLVXD_LOAD_ENUMERATOR)
        return(NewEnumerator(Devnode));
    else
        return(CR_DEFAULT);
}
