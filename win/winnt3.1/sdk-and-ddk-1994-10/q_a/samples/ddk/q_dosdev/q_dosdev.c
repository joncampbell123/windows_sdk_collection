/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    q_dosdev.c

Abstract:

    A user mode test app that lists all the DOS device names

Environment:

    User mode only

Revision History:

    05-26-93 : created

--*/



#include <stdio.h>
#include <windows.h>



void main()
{
    char   buf[1024], buf2[1024], *p, *q;

    QueryDosDevice (NULL, buf, 1024);

    p = buf;

    while (*p)
    {
        if (strlen(p) > 7)

          printf (p);

        else

          printf ("%s\t", p);

        QueryDosDevice (p, buf2, 1024);

        q = buf2;

        while (*q)
        {
            printf ("\t%s\n", q);
            q += strlen(q) + 1;
        }

        p += strlen(p) + 1;
    }

    return;
}
