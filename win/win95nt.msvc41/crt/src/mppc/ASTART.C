/***
*astart.c -
*
*       Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#include <macos/types.h>

extern void __crt0();

//static unsigned long .mainCRTStartup[2] =
//                        { &mainCRTStartup, &_TocTb };
extern void *GetSP(void);

extern unsigned int dwSPStartup;

void mainCRTStartup(void)
{
         dwSPStartup = (unsigned int)GetSP();
          __crt0();
}


/* Directive to defaultly link interfac.lib
*
*/

#pragma data_seg(".drectve")

static char szDefaultLib[] = "-defaultlib:INTERFAC.LIB";

#pragma data_seg()
