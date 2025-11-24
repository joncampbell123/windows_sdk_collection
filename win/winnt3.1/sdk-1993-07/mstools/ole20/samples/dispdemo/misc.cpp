/*** 
*misc.cpp
*
*  Copyright (C) 1992-93, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*
*Implementation Notes:
*
*****************************************************************************/

#include "dispdemo.h"

#include <stdio.h>
#include <stdarg.h>


int g_fTrace = 0;

extern "C" void
ChkAssert(int fCond, char *file, int line)
{
    char buf[128];

    if(fCond)
      return;

    sprintf(buf, "Assertion failed: %s(%d)\n", file, line);

#ifdef _MAC
    DebugStr(c2pstr(buf));
#else
    OutputDebugString(buf);
    DebugBreak();
#endif
}

STDAPI
InitOle(void)
{
    HRESULT hresult;

    if((hresult = OleInitialize(NULL)) != NOERROR)
      return hresult;

    return NOERROR;
}

STDAPI
UninitOle()
{
    OleUninitialize();

    return NOERROR;
}

