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
 *  PCI.H - 	PCI include file for structure definitions of the private
 *		info the PCI enumerator uses.
 *
 */

//
// Note: For these two device info functions, PCI uses the ulRefData as being
// the offset. The buffer is where the info is read/written to.
//
#define	PCI_ENUM_FUNC_GET_DEVICE_INFO	0
#define	PCI_ENUM_FUNC_SET_DEVICE_INFO	1
#define	NUM_PCI_ENUM_FUNC		2

