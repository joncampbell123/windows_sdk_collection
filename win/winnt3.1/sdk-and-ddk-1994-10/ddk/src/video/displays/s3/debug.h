/***************************************************************************\
* Module Name: debug.h
*
* Commonly used debugging macros.
*
* Copyright (c) 1992 Microsoft Corporation
\***************************************************************************/
#ifdef __CPLUSPLUS
extern "C"
VOID
DebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );
#else
extern
VOID
DebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );
#endif

// if we are in a debug environment, macros should

#if DBG

VOID DebugLog(ULONG, PCHAR, ...);

#define DISPDBG(arg) DebugPrint arg
#define LOGDBG(arg) DebugLog arg
#define RIP(x) { DebugPrint(0, x); DebugBreak();}
#define ASSERTS3(x, y) if (!(x)) RIP (y)

// if we are not in a debug environment, we want all of the debug
// information to be stripped out.

#else

#define DISPDBG(arg)
#define LOGDBG(arg)
#define RIP(x)
#define ASSERTS3(x, y)

#endif
