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

/*****************************************************************************
 *
 *  Title:      EXAMENUM.H - Example Enumerator header file
 *
 *  Version:    1.00
 *
 *****************************************************************************/

#define EXAMENUM_VERSION    0x100
#define EXAMENUM_NAME       "EXAMENUM"

/****************************************************************************
 *
 *              Function Prototypes
 *
 ***************************************************************************/
////////////////////////////////////////////////////////////////////////////// 
// ENUM.C
//
CONFIGRET CM_INTERNAL NewEnumerator(DEVNODE DevNode);
BOOL CM_INTERNAL EnumerateExampleBusType(DEVNODE dnDevNode);

