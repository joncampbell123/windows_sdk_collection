/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    cms.h

Abstract:


Revision History:          




--*/
#define IOCTL_CMS_WRITE_ABS_BLOCK CTL_CODE(FILE_DEVICE_TAPE, 0x5500, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

typedef struct _CMS_RW_ABS {
    ULONG Block;    // Block number to start operation on
    ULONG Count;    // Number of blocks to read/write
    ULONG Status;   // CMS Status of operation
    ULONG BadMap;   // in: bad sectors,  out: sector failures
} CMS_RW_ABS, *PCMS_RW_ABS;

#define IOCTL_CMS_READ_ABS_BLOCK CTL_CODE(FILE_DEVICE_TAPE, 0x5501, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
