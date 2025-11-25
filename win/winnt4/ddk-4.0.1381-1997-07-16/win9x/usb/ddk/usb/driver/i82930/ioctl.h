/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    ioctl.h

Abstract:

        

Environment:

    Kernel & user mode

Revision History:

    5-10-96 : created

--*/

#define I82930_IOCTL_INDEX  0x0000


#define IOCTL_I82930_GET_PIPE_INFO     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   I82930_IOCTL_INDEX,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_I82930_GET_CONFIG_DESCRIPTOR     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   I82930_IOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
                                                   
#define IOCTL_I82930_SET_PIPE_PARAMETER     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   I82930_IOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


#include <PSHPACK1.H>

#define BULK      0
#define INTERRUPT 1
#define CONTROL   2
#define ISO       3

typedef struct _I82930_PIPE_INFO {
    BOOLEAN In;
    UCHAR PipeType;
    UCHAR EndpointAddress;
    UCHAR Interval;
    ULONG MaximumPacketSize;
    ULONG MaximumTransferSize;
    UCHAR Name[32];
} I82930_PIPE_INFO, *PI82930_PIPE_INFO;


typedef struct _I82930_INTERFACE_INFO {
    ULONG PipeCount;
    I82930_PIPE_INFO Pipes[0];    
} I82930_INTERFACE_INFO, *PI82930_INTERFACE_INFO;
#include <POPPACK.H>

