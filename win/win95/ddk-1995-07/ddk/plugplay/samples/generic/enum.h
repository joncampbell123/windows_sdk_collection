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
 *  ENUM.H - NewEnumerator Support for SAMPLE
 *
 *  Notes:
 *
 *	This is the include file of ENUM.C
 */

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
NewEnumerator(DEVNODE DevNode);

