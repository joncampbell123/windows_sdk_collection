//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Trace.h
//
//  Contents:
//
//  Classes:    None.
//
//  Functions:  TraceInit
//              TraceSTDAPIEntry
//              TraceSTDAPIExit
//              TraceMethodEntry
//              TraceMethodExit
//
//  History:    01-Mar-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#ifdef _DEBUG
void TraceInit(void);
void TraceSTDAPIEnter(PSTR);
void TraceSTDAPIExit(PSTR, HRESULT);
void TraceMethodEnter(PSTR, PVOID);
void TraceMethodExit(PSTR, PVOID, HRESULT);
void TraceAddRef(PSTR, PVOID, LONG);
void TraceRelease(PSTR, PVOID, LONG);
void TraceNotify(PSTR);
void TraceWarning(PSTR);
void TraceWrapper(PSTR);
void TraceFatalError(PSTR);
void TraceOutput(PSTR);
#else
#define TraceInit()
#define TraceSTDAPIEnter(x)
#define TraceSTDAPIExit(x, y)
#define TraceMethodEnter(x, y)
#define TraceMethodExit(x, y, z)
#define TraceAddRef(x, y, z)        (z)
#define TraceRelease(x, y, z)       (z)
#define TraceNotify(x)
#define TraceWarning(x)
#define TraceWrapper(x)
#define TraceFatalError(x)
#define TraceOutput(x);
#endif
