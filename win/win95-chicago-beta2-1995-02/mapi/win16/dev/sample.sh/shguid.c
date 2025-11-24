/*
 -  S H G U I D . C
 -
 *  Purpose:
 *      This file is used to build shguid.obj, which contains the binary
 *      representation of the ISpoolerHook MAPI guid.
 *
 *  Copyright 1992-94 Microsoft Corporation.  All Rights Reserved.
 *
 */

#define USES_IID_ISpoolerHook
#define USES_IID_IMAPISession

#ifdef WIN32                    /* Must include WINDOWS.H on Win32 */
#ifndef _WINDOWS_
#define INC_OLE2                /* Get the OLE2 stuff */
#define INC_RPC                 /* harmless on Daytona; Chicago needs it */
#define _INC_OLE                /* Chicago will include OLE1 without this */
#include <windows.h>
#include <ole2.h>
#endif
#endif

#ifdef WIN16
#include <compobj.h>
#endif

#define INITGUID
#include <initguid.h>

#include "mapiguid.h"
