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

/* MISCUTIL.c: 

   Misc. routines.

   Routines contained in module:<nl>
      <l cVpenD_Device_Exit.cVpenD_Device_Exit><nl>
*/

#include "defines.h"

#include <basedef.h>
#include <VMM.H>
#ifdef DEBUG
#include <debug.h>
#endif
#include "PenDrv.h"
#include "protos.h"

#pragma VxD_LOCKED_CODE_SEG

extern PVOID gpGlobalMemory;

void cVpenD_Device_Exit(void)
{
   _HeapFree(gpGlobalMemory,0);

   DBG_TRACE("Unloading VpenD");
}


// End-Of-File
