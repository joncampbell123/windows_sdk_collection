/***
*timeset.c - contains defaults for timezone setting
*
*       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the timezone values for default timezone.
*       Also contains month and day name three letter abbreviations.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdlib.h>
#include <time.h>
#include <internal.h>


#ifndef DLL_FOR_WIN32S

long _timezone = 8*3600L;   /* Pacific Time */
int _daylight = 1;          /* Daylight Savings Time */
                            /* when appropriate */

/* note that NT Posix's TZNAME_MAX is 10 */

static char tzstd[11] = { "PST" };
static char tzdst[11] = { "PDT" };

char *_tzname[2] = { tzstd, tzdst };

#endif  /* DLL_FOR_WIN32S */


/*  Day names must be Three character abbreviations strung together */

const char __dnames[] = {
        "SunMonTueWedThuFriSat"
};

/*  Month names must be Three character abbreviations strung together */

const char __mnames[] = {
        "JanFebMarAprMayJunJulAugSepOctNovDec"
};
