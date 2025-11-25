/*** 
*clsid.c
*
*  This is a part of the Microsoft Source Code Samples.
*
*  Copyright (C) 1992-1995 Microsoft Corporation. All rights reserved.
*
*  This source code is only intended as a supplement to Microsoft Development
*  Tools and/or WinHelp documentation.  See these sources for detailed
*  information regarding the Microsoft samples programs.
*
*Purpose:
*  This file allocates and initializes the CLSIDs.
*
*****************************************************************************/



// initguid.h requires this.
//

#ifdef _MAC
# include <Types.h>
# include <Processes.h>
# include <AppleEvents.h>
#else
# include <windows.h>
#endif

#ifndef WIN32
#include <compobj.h>
#endif //!WIN32

// this redefines the DEFINE_GUID() macro to do allocation.
//
#include <initguid.h>

// due to the previous header, including this causes the DEFINE_GUID
// definitions in the following header(s) to actually allocate data.
//
#include "clsid.h"
