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
 *  DRIVER.H - NewDriver Support for SAMPLE
 *
 *  Notes:
 *
 *	This is the include file of DRIVER.C
 */

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
NewDriver(DEVNODE DevNode);

