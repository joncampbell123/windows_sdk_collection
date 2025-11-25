/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    ntstapi.h

Abstract:

    This module declares the STREAMS APIs that are provided for use
    primarily by the NT tcp/ip socket library.

Author:

    Eric Chin (ericc)           July 26, 1991

Revision History:

    mikemas   01-02-92    Deleted poll definition. Mips complained because
                          of def in winsock.h


--*/

#ifndef _NTSTAPI_
#define _NTSTAPI_


//
// s_close() is not provided.  Use the open and close primitives that are
// appropriate to your subsystem.
//

int
WINAPI
getmsg(
    IN HANDLE fd,
    IN OUT struct strbuf *ctrlptr OPTIONAL,
    IN OUT struct strbuf *dataptr OPTIONAL,
    IN OUT int *flagsp
    );

int
WINAPI
putmsg(
    IN HANDLE fd,
    IN struct strbuf *ctrlptr OPTIONAL,
    IN struct strbuf *dataptr OPTIONAL,
    IN int flags
    );

int
WINAPI
s_ioctl(
    IN HANDLE fd,
    IN int cmd,
    IN OUT void *arg OPTIONAL
    );

HANDLE
WINAPI
s_open(
    IN char *path,
    IN int oflag,
    IN int ignored
    );

#endif /* _NTSTAPI_ */
