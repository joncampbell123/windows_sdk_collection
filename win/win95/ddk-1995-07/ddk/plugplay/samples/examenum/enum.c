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
 *  ENUM.C - Example enmumertor
 *
 *  Notes:
 *
 *  This file processes all the control messages sent by VMM.
 *
 */

#include "pch.h"

#pragma CM_PAGEABLE_DATA
#pragma CM_PAGEABLE_CODE

#pragma warning (disable:4100)  // Parameter unused

/****************************************************************************
 *
 *      ExampleEnumerator - Handler of the config manager enumeration calls
 *
 *      Exported.
 *
 *      ENTRY:  Standard enumerator handler.
 *          cfFuncName:     CONFIG_ENUMERATE
 *          sbfSubFuncName:
 *          dnToDevNode:    CONFIG_ENUMERATE:  DevNode being Enumerated
 *          ulFlags:        Not used
 *      EXIT:   Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_HANDLER
ExampleEnumerator
(
    CONFIGFUNC cfFuncName,
    SUBCONFIGFUNC sbfSubFuncName,
    DEVNODE dnToDevNode,
    DEVNODE dnAboutDevNode,
    ULONG ulFlags
)
{
    switch (cfFuncName) 
    {
        case CONFIG_ENUMERATE:
            EnumerateExampleBusType(dnToDevNode);
            return(CR_SUCCESS);
            break;    

        default:
            return(CR_DEFAULT);
    }
}

#pragma warning (default:4100)  // Parameter unused

#define EXAMPLEENUM_NAME        "EXAMENUM"
#define EXAMPLE_DEVNODE_NAME    "EXAMENUM\\ExampleDev"
#define EXAMPLE_DEVNODE_DESC    "Example Device on a Example Bus"
#define EXAMPLE_DEVNODE_CLASS   "ExampleClass"
/****************************************************************************

    EnumerateExampleBusType - Builds a new dev node for any device on the bus

    Description:

        Entry:  
            DevNode is the parent devnode
        Exit:   
            TRUE of successful (ie there is a device detected and a dev node is build)  
            FALSE if no device found or dev node if not built.

***************************************************************************/
BOOL CM_INTERNAL
EnumerateExampleBusType
(
    DEVNODE dnDevNode
)
{
    CONFIGRET   crRetVal;               // For CONFIGMG return values
    
    DEVNODE     dnNewDevNode;
    
    // Analyze the bus type here, and create any new devnodes for devices found on
    // the bus.  
    
    // Determine actual device name programatically, and include it here.  Do not add
    // an instance, and configmg will add it for you
        
    // Create the DevNode,.
    crRetVal = CM_Create_DevNode(&dnNewDevNode, EXAMPLE_DEVNODE_NAME, dnDevNode, 0L);
    if (crRetVal == CR_OUT_OF_MEMORY)
    {
        CM_WARN1(("FindMonitor: ConfigMG reports no mem to add DevNode"));
        return(FALSE);
    }

    // If Config Manager says its allready there, then just return
    if (crRetVal==CR_ALREADY_SUCH_DEVNODE)
    {
        CM_WARN1(("FindMonitor: ConfigMG reports DEVNODE allready exists"));
        return(TRUE);
    }
    
    if (crRetVal != CR_SUCCESS)
    {
        CM_WARN1(("[DISPENUM] FindMonitor: ConfigMG reports Error adding DevNode"));
        return(FALSE);
    }

    // If we get here, the devnode was created successfully.  Now we write
    // and usefull information about this devnode
    CM_Write_Registry_Value (dnNewDevNode, 
                             NULL, 
                             REGSTR_VAL_DEVDESC, 
                             REG_SZ, 
                             EXAMPLE_DEVNODE_DESC, 
                             sizeof(EXAMPLE_DEVNODE_DESC),
                             CM_REGISTRY_HARDWARE);
    
    CM_Write_Registry_Value (dnNewDevNode, 
                             NULL, 
                             REGSTR_VAL_CLASS, 
                             REG_SZ, 
                             EXAMPLE_DEVNODE_CLASS, 
                             sizeof(EXAMPLE_DEVNODE_CLASS), 
                             CM_REGISTRY_HARDWARE);
    return(TRUE);
}

/****************************************************************************
 *
 *  NewEnumerator - Register a new enumerator for the new devnode
 *
 *  Description:
 *      This function is call when we receive a PNP_NewDevnode system
 *      call.  This function registers an enumerator handler with the
 *      config manater.
 *
 *  Exported.
 *
 *  ENTRY:  
 *      DevNode is the new devnode that has just been created.
 *
 *  EXIT:   
 *      Standard config manager return value.
 *
 ***************************************************************************/
CONFIGRET CM_INTERNAL
NewEnumerator(DEVNODE DevNode)
{
    return (CM_Register_Enumerator(DevNode, ExampleEnumerator, 0));
}

