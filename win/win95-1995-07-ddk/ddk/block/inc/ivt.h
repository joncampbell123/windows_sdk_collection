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

/****************************************************************************\
*
*  ivt - ios vector table - contained in ios's code segment and provides
*			various usefull pointers
*
\****************************************************************************/

typedef struct IVT { /* */

	ULONG	IVT_ios_mem_virtual;	/* virtual addr of ios memory pool	    */
	PVOID	IVT_ios_mem_phys;	/* physical addr of ios memory pool */
	ULONG	IVT_first_dvt;		/* 32-bit offset of first dvt	    */	

} _IVT;
