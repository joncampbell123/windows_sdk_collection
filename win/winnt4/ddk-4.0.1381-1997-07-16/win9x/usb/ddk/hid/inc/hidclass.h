/*++

Copyright (c) 1996  Microsoft Corporation

Module Name: 

    hidclass.h

Abstract

    Definitions that are common to clients of the HID class driver.

Author:

    Forrest Foltz

Environment:

    Kernel mode only

Revision History:


--*/

#ifndef __HIDCLASS_H__
#define __HIDCLASS_H__

#include    <basetyps.h>

#ifndef FAR

//
// BUGBUG (forrestf) it doesn't seem like i should have to do this
//

#define FAR
#endif

//
// Current version of this header file.
//

#define HID_REVISION    0x00000000

//
// Define the HID class guid
// 

DEFINE_GUID( GUID_CLASS_INPUT, 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, \
             0x11, 0x11, 0x00, 0x00, 0x30);

#define GUID_CLASS_INPUT_STR "4D1E55B2-F16F-11CF-88CB-001111000030"

//
// Macro for defining HID ioctls
// 

#define HID_CTL_CODE(id)    \
    CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_NEITHER, FILE_ANY_ACCESS)

//
// IOCTLs supported by the upper edge of the HID class driver
//

#define IOCTL_HID_GET_DRIVER_CONFIG             HID_CTL_CODE(100)
#define IOCTL_HID_SET_DRIVER_CONFIG             HID_CTL_CODE(101)
#define IOCTL_HID_GET_COLLECTION_INFORMATION    HID_CTL_CODE(102)
#define IOCTL_HID_GET_COLLECTION_DESCRIPTOR     HID_CTL_CODE(103)
#define IOCTL_HID_FLUSH_QUEUE                   HID_CTL_CODE(104)

//
// Structure passed by IOCTL_HID_GET_COLLECTION_INFORMATION
//

typedef struct _HID_COLLECTION_INFORMATION {

    //
    // DescriptorSize is the size of the input buffer required to accept
    // the collection descriptor returned by
    // IOCTL_HID_GET_COLLECTION_DESCRIPTOR.
    //

    ULONG   DescriptorSize;

    //
    // Polled is TRUE if this collection is a polled collection.
    //

    BOOLEAN Polled;

    //
    // Reserved1 must be set to zero.
    //

    UCHAR   Reserved1[ 3 ];     

    //
    // Additional fields, if any, will be added at the end of this structure.
    // 

} HID_COLLECTION_INFORMATION, PHID_COLLECTION_INFORMATION;

//
// Structure passed by IOCTL_HID_GET_DRIVER_CONFIG and
// IOCTL_HID_SET_DRIVER_CONFIG
//

typedef struct _HID_DRIVER_CONFIG {

    //
    // Size must be set to the size of this structure.
    // 

    ULONG   Size;

    //
    // Size of the input report queue (in reports).  This value can be set.
    //

    ULONG   RingBufferSize;

} HID_DRIVER_CONFIG, *PHID_DRIVER_CONFIG;

#endif  // __HIDCLASS_H__


