/*++

Copyright (c) 1993  Digital Equipment Corporation

Module Name:

    alpharef.h

Abstract:

    This module defines the reference hardware definitions for Alpha AXP 
    platforms.  Any platform that adheres to these interfaces will be 
    capable of running all of the common drivers.

Author:

    Joe Notarangelo 15-Feb-1993

Revision History:

    John DeRosa [DEC]	2-July-1993

    Added firmware vendor call definitions that are generic to all Alpha
    platforms.

--*/

#ifndef _ALPHAREF_
#define _ALPHAREF_


//
// Define interesting device addresses.
//

#define KEYBOARD_PHYSICAL_BASE 0x60

//
// Define DMA device channels.
//

#define SCSI_CHANNEL 0x0                // SCSI DMA channel number
#define FLOPPY_CHANNEL 0x2              // Floppy DMA channel
#define SOUND_CHANNEL_A 0x2             // Sound DMA channel A
#define SOUND_CHANNEL_B 0x3             // Sound DMA channel B


//
// Define default thread and process quantum values.
//


#define PROCESS_QUANTUM 2           // Process quantum in thread quantums
#define THREAD_QUANTUM 2            // Thread quantum in ticks

//
// Define the default quantum decrement values.
//

#define CLOCK_QUANTUM_DECREMENT 1   // Decrement value at clock interrupt
#define MUTEX_QUANTUM_DECREMENT 1   // Decrement value at mutex release
#define WAIT_QUANTUM_DECREMENT 1    // Decrement value at wait operation

//
// Define the interrupt request levels.
//

#define FLOPPY_LEVEL	  6		// The floppy 
#define CLOCK_LEVEL       5             // Interval clock level
#define PROFILE_LEVEL     3             // Profiling level
#define EISA_DEVICE_LEVEL 3             // EISA bus interrupt level
#define ISA_DEVICE_LEVEL  3             // ISA bus interrupt level
#define DEVICE_LEVEL      3             // Generic device interrupt level
#define DEVICE_LOW_LEVEL  3             // I/O device interrupt level low
#define DEVICE_HIGH_LEVEL 4             // I/O device interrupt level high
#define IPI_LEVEL         6             // Inter-processor interrupt level
#define POWER_LEVEL       7             // Powerfail level
#define EISA_NMI_LEVEL    POWER_LEVEL   // Eisa NMI failures
#define CLOCK2_LEVEL CLOCK_LEVEL        //

//
// Define EISA device interrupt vectors.
//

#define EISA_VECTORS 48

//
// Define the EISA interrupt request levels.  Levels 1,8 and 13 are not
// defined.  Level 0 is also the timer.  Level 2 is not assignable because
// it receives the vector from the second PIC bank.
//

#define EISA_IRQL0_VECTOR (0 + EISA_VECTORS) // Eisa interrupt request level 0

#define EISA_IRQL3_VECTOR (3 + EISA_VECTORS)
#define EISA_IRQL4_VECTOR (4 + EISA_VECTORS)
#define EISA_IRQL5_VECTOR (5 + EISA_VECTORS)
#define EISA_IRQL6_VECTOR (6 + EISA_VECTORS)
#define EISA_IRQL7_VECTOR (7 + EISA_VECTORS)
#define EISA_IRQL9_VECTOR (9 + EISA_VECTORS)
#define EISA_IRQL10_VECTOR (10 + EISA_VECTOR)
#define EISA_IRQL11_VECTOR (11 + EISA_VECTORS)
#define EISA_IRQL12_VECTOR (12 + EISA_VECTORS)
#define EISA_IRQL14_VECTOR (14 + EISA_VECTORS)
#define EISA_IRQL15_VECTOR (15 + EISA_VECTORS)

#define MAXIMUM_EISA_VECTOR (16 + EISA_VECTORS) // maximum EISA vector

//
// The parallel port is at IRQL1 by default.
//

#define PARALLEL_VECTOR (1 + EISA_VECTORS) // Parallel device interrupt vector

//
// Define ISA device interrupt vectors.
//

#define ISA_VECTORS 48

#define KEYBOARD_VECTOR 1
#define MOUSE_VECTOR 12

//
// Define the EISA interrupt request levels.  Levels 1,8 and 13 are not
// defined.  Level 0 is also the timer.  Level 2 is not assignable because
// it receives the vector from the second PIC bank.
//

#define ISA_IRQL0_VECTOR (0 + ISA_VECTORS)

#define ISA_IRQL3_VECTOR (3 + ISA_VECTORS)
#define ISA_IRQL4_VECTOR (4 + ISA_VECTORS)
#define ISA_IRQL5_VECTOR (5 + ISA_VECTORS)
#define ISA_IRQL6_VECTOR (6 + ISA_VECTORS)
#define ISA_IRQL7_VECTOR (7 + ISA_VECTORS)
#define ISA_IRQL9_VECTOR (9 + ISA_VECTORS)
#define ISA_IRQL10_VECTOR (10 + ISA_VECTORS)
#define ISA_IRQL11_VECTOR (11 + ISA_VECTORS)
#define ISA_IRQL12_VECTOR (12 + ISA_VECTORS)
#define ISA_IRQL14_VECTOR (14 + ISA_VECTORS)
#define ISA_IRQL15_VECTOR (15 + ISA_VECTORS)

#define MAXIMUM_ISA_VECTOR (16 + ISA_VECTORS) // maximum ISA vector

//
// Define I/O device interrupt level.
//


//
// Define device interrupt vectors.
//

#define DEVICE_VECTORS 0               // starting builtin device vector

#define SERIAL_VECTOR (4 + DEVICE_VECTORS)    // Serial device 1 interrupt vector
#define CLOCK_VECTOR (5 + DEVICE_VECTORS)     // Clock interrupt vector
#define PC0_VECTOR   (6 + DEVICE_VECTORS)     // Performance counter 0
#define EISA_NMI_VECTOR (7 + DEVICE_VECTORS)  // NMI vector
#define PC1_VECTOR   (8 + DEVICE_VECTORS)     // Performance counter 1
#define PIC_VECTOR   (10 + DEVICE_VECTORS)    // Programmable Interrupt Ctrler
#define ERROR_VECTOR (12 + DEVICE_VECTORS)    // Error interrupt vector
#define HALT_VECTOR  (14 + DEVICE_VECTORS)    // Halt Button interrupt vector
#define UNUSED_VECTOR (16 + DEVICE_VECTORS)   // Highest possible builtin vector

#define MAXIMUM_BUILTIN_VECTOR UNUSED_VECTOR // maximum builtin vector

//
// Define profile intervals.
//

#define DEFAULT_PROFILE_COUNT 0x40000000 // ~= 20 seconds @50mhz
#define DEFAULT_PROFILE_INTERVAL (10 * 500) // 500 microseconds
#define MAXIMUM_PROFILE_INTERVAL (10 * 1000 * 1000) // 1 second
#define MINIMUM_PROFILE_INTERVAL (10 * 40) // 40 microseconds

//
// Define the QVA selector bits which indicate an address is a 
// "QVA" - quasi-virtual address.
//

#define QVA_ENABLE 0xA0000000     


//
// Alpha-generic firmware vendor calls.
//

#ifndef _LANGUAGE_ASSEMBLY
//
// Structure used to return system and processor information.
//

typedef struct _EXTENDED_SYSTEM_INFORMATION {
    ULONG   ProcessorId;
    ULONG   ProcessorRevision;
    ULONG   ProcessorPageSize;
    ULONG   NumberOfPhysicalAddressBits;
    ULONG   MaximumAddressSpaceNumber;
    ULONG   ProcessorCycleCounterPeriod;
    ULONG   SystemRevision;
    UCHAR   SystemSerialNumber[16];
    UCHAR   FirmwareVersion[16];
} EXTENDED_SYSTEM_INFORMATION, *PEXTENDED_SYSTEM_INFORMATION;

//
// Define the firmware vendor specific entry point numbers that are
// common to all Alpha AXP platforms.  fw\alpha\fwp.h has private
// definitions that are machine-specific.
//

typedef enum _VENDOR_GENERIC_ENTRY {
    AllocatePoolRoutine,
    StallExecutionRoutine,
    PrintRoutine,
    ReturnExtendedSystemInformationRoutine,
    VendorGenericMaximumRoutine
    } VENDOR_GENERIC_ENTRY;

//
// Define vendor specific routine types.
//

typedef
PVOID
(*PVEN_ALLOCATE_POOL_ROUTINE) (
    IN ULONG NumberOfBytes
    );

typedef
VOID
(*PVEN_STALL_EXECUTION_ROUTINE) (
    IN ULONG Microseconds
    );

typedef
ULONG
(*PVEN_PRINT_ROUTINE) (
    IN PCHAR Format,
    ...
    );

typedef
VOID
(*PVEN_RETURN_EXTENDED_SYSTEM_INFORMATION_ROUTINE) (
    OUT PEXTENDED_SYSTEM_INFORMATION SystemInfo
    );

//
// Define vendor specific prototypes.
//

PVOID
FwAllocatePool (
    IN ULONG NumberOfBytes
    );

VOID
FwStallExecution (
    IN ULONG Microseconds
    );

ULONG
FwPrint (
    IN PCHAR Format,
    ...
    );

VOID
FwReturnExtendedSystemInformation (
    OUT PEXTENDED_SYSTEM_INFORMATION SystemInfo
    );
    

//
// Define vendor specific functions.
//

#define VenAllocatePool(NumberOfBytes) \
    ((PVEN_ALLOCATE_MEMORY_ROUTINE)(SYSTEM_BLOCK->VendorVector[AllocatePoolRoutine])) \
        ((NumberOfBytes))

#define VenStallExecution(Microseconds) \
    ((PVEN_STALL_EXECUTION_ROUTINE)(SYSTEM_BLOCK->VendorVector[StallExecutionRoutine])) \
        ((Microseconds))

#define VenPrint(x) \
    ((PVEN_PRINT_ROUTINE)(SYSTEM_BLOCK->VendorVector[PrintRoutine])) \
        ((x))

#define VenPrint1(x,y) \
    ((PVEN_PRINT_ROUTINE)(SYSTEM_BLOCK->VendorVector[PrintRoutine])) \
        ((x), (y))

#define VenPrint2(x,y,z) \
    ((PVEN_PRINT_ROUTINE)(SYSTEM_BLOCK->VendorVector[PrintRoutine])) \
        ((x), (y), (z))

#define VenReturnExtendedSystemInformation(x) \
    ((PVEN_RETURN_EXTENDED_SYSTEM_INFORMATION_ROUTINE)(SYSTEM_BLOCK->VendorVector[ReturnExtendedSystemInformationRoutine]))(x)

#endif // _LANGUAGE_ASSEMBLY not defined
#endif // _ALPHAREF_












