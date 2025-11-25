/*===========================================================================*\
|
|  File:        cgdebug.h
|
|  Description: 
|       debugging macros 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#ifndef CGDEBUG_H
#define CGDEBUG_H

// NOTE: must define DEBUG to get any debug strings!
#ifdef DEBUG
#define DB_LOG(type, string) if (DB_TYPES & type) OutputDebugString(string);
#else
#define DB_LOG(type, string)
#endif

// DB_TYPES is a bitmap of desired debug strings
#define DB_INFO             0x01    // for non-error, non-time-critical information output
#define DB_TIMECRIT_INFO    0x02    // for non-error, time-critical (e.g. IRQ handler) info
#define DB_PROBLEM          0x04    // for non-time-critical problem report
#define DB_TIMECRIT_PROBLEM 0x08    // for time-critical (e.g. IRQ handler) problem report

// default to no debug output if DB_TYPES not defined
#ifndef DB_TYPES
#define DB_TYPES 0
#endif

#ifdef DEBUG
#define DB_BREAK_IF(x)  if (x) DebugBreak();
#define DB_BREAK()  DebugBreak();

#define DB_CHECK(x, string) if (x) DB_LOG(DB_PROBLEM, string);
#else
#define DB_BREAK_IF(x)
#define DB_BREAK()

#define DB_CHECK(x, string)
#endif


#endif // CGDEBUG_H
