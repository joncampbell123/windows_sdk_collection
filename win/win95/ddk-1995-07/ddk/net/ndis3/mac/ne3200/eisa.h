/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    eisa.h

Abstract:

    EISA specific constants and structures.

    NOTE:  This is a temporary hack used by NE3200StartAdapters to
    allow it to search for all installed NE3200 adapters.  This file
    will be removed whenever we get the Configuration Manager.

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

Notes:

    optional-notes

--*/

#ifndef _EISACONFIGURATION_
#define _EISACONFIGURATION_

//
// These are used by NE3200StartAdapters during its search
// for NE3200 adapters.
//

#define EISA_MAXIMUM_NUMBER_OF_SLOTS    15
#define EISA_READ_FUNCTION_INFORMATION  0x0000D881
#define EISA_NE3200_IDENTIFICATION      0x0107CC3A
#define EISA_BIOS_ENTRY_POINT           0x000FF859


//
// This structure is returned by BIOS function D881.
//

typedef struct _EISA_CONFIGURATION {

    ULONG Identification;

    UINT DuplicateCfg:4;
    UINT SlotType:2;
    UINT IdReadable:1;
    UINT DuplicateIdPresent:1;

    UINT EisaEnableNotSupported:1;
    UINT EisaIochkerrSupported:1;
    UINT _reserved1:5;
    UINT ConfigurationNotComplete:1;

    UINT MinorRevisionLevel:8;
    UINT MajorRevisionLevel:8;

    UCHAR Selections[26];

    UCHAR TypeEntriesFollow:1;
    UCHAR MemoryEntriesFollow:1;
    UCHAR InterruptEntriesFollow:1;
    UCHAR DmaEntriesFollow:1;
    UCHAR PortRangeEntriesFollow:1;
    UCHAR PortInitializationEntriesFollow:1;
    UCHAR CfgExtensionFreeFormData:1;
    UCHAR FunctionDisabled:1;

    UCHAR TypeEntries[80];

    UCHAR MemoryEntries[63];

    UCHAR FirstInterruptLevel:4;
    UCHAR FirstInterruptReserved:1;
    UCHAR FirstInterruptLevelTriggered:1;
    UCHAR FirstInterruptShared:1;
    UCHAR FirstInterruptMoreEntries:1;

    UCHAR OtherInterrupts[13];

    UCHAR DmaEntries[8];

    UCHAR PortEntries[60];

    UCHAR PortInitializationEntires[60];

} EISA_CONFIGURATION, *PEISA_CONFIGURATION;

#endif // _EISACONFIGURATION_

