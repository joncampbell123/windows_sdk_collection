
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

////////////////////////////////////////////////////////
//
//  Client.c --
//
//      This program is a command line oriented
//      demonstration of the Simple service
//      sample.
//
//      Copyright 1993, Microsoft Corp.  All Rights Reserved
//
//  history:
//
//      who         when            what
//      ---         ----            ----
//      davidbro    2/3/93          creation
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VOID
main(int argc, char *argv[])
{
    char    inbuf[80];
    char    outbuf[80];
    DWORD   bytesRead;
    BOOL    ret;

    if (argc != 3) {
        printf("usage: client <pipename> <string>\n");
        exit(1);
    }

    strcpy(outbuf, argv[2]);

    ret = CallNamedPipe(argv[1], outbuf, sizeof(outbuf),
                                 inbuf, sizeof(inbuf),
                                 &bytesRead, NMPWAIT_WAIT_FOREVER);

    if (!ret) {
        printf("client: CallNamedPipe failed, GetLastError = %d\n",
                GetLastError());
        exit(1);
    }

    printf("client: received: %s\n", inbuf);
}
