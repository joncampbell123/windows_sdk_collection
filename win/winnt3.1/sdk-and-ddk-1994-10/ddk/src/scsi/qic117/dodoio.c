/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    dodoio.c

Abstract:

    Forms a low-level command request and processes it syncronusly

Revision History:




--*/

//
// include files
//

#include <ntddk.h>
#include <ntddtape.h>
#include "common.h"
#include "q117.h"
#include "protos.h"


STATUS
q117DoCmd(
    IN OUT PIO_REQUEST IoRequest,
    IN CHAR Command,
    IN PVOID Data,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Makes the DoIO call.

Arguments:

    IoRequest -

    Command -

    Data -

    Context -

Return Value:



--*/

{
    STATUS ret;

    IoRequest->Command = Command;
    IoRequest->Data = Data;
    ret = q117DoIO(IoRequest, NULL, Context);
    if (!ret)
        ret = IoRequest->Status;
    return(ret);
}

