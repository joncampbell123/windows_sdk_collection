/*++

Copyright (c) 1996  Microsoft Corporation

Module Name: 

    hidmini.h

Abstract

    Definitions that are common to all HID minidrivers.

Author:

    Forrest Foltz

Environment:

    Kernel mode only

Revision History:


--*/

#ifndef __HIDPORT_H__
#define __HIDPORT_H__

#include    <hidclass.h>

//
// HID_MINIDRIVER_REGISTRATION is a packet of information describing the
// HID minidriver to the class driver.  It must be filled in by the minidriver
// and passed to the class driver via HidRegisterMinidriver() from the
// minidriver's DriverEntry() routine.
//

typedef struct _HID_MINIDRIVER_REGISTRATION {

    //
    // Revision must be set to HID_REVISION by the minidriver
    //

    ULONG           Revision;

    //
    // DriverObject is a pointer to the minidriver's DriverObject that it
    // received as a DriverEntry() parameter.
    //

    PDRIVER_OBJECT  DriverObject;

    //
    // RegistryPath is a pointer to the minidriver's RegistryPath that it
    // received as a DriverEntry() parameter.
    //

    PUNICODE_STRING RegistryPath;

    //
    // DeviceExtensionSize is the size of the minidriver's per-device
    // extension.
    //

    ULONG           DeviceExtensionSize;

} HID_MINIDRIVER_REGISTRATION, *PHID_MINIDRIVER_REGISTRATION;

//
// HID_DEVICE_EXTENSION is the public part of the device extension of a HID
// functional device object.
// 

typedef struct _HID_DEVICE_EXTENSION {

    //
    // PhysicalDeviceObject... normally IRPs are not passed to this.
    // 

    PDEVICE_OBJECT  PhysicalDeviceObject;

    //
    // NextDeviceObject... IRPs are sent here by the minidriver.  Note that
    // NextDeviceObject and PhysicalDeviceObject are the same unless someone
    // has inserted a 'filter' device object, in which case they are not the
    // same.  Sending IRPs to NextDeviceObject will hit the filter device
    // objects on the way down.
    // 

    PDEVICE_OBJECT  NextDeviceObject;

    //
    // MiniDeviceExtension is the per-device extension area for use by
    // the minidriver.  It's size is determined by the DeviceExtensionSize
    // parameter passed in to HidAddDevice().
    //
    // So, given a Functional Device Object, a mininidriver finds this
    // structure by:
    //
    //    HidDeviceExtension = (PHID_DEVICE_EXTENSION)(Fdo->DeviceExtension);
    //
    // And of course it's per-device extension is found by:
    //                                                 
    //    MiniDeviceExtension = HidDeviceExtension->MiniDeviceExtension;
    //

    PVOID           MiniDeviceExtension;

} HID_DEVICE_EXTENSION, *PHID_DEVICE_EXTENSION;

//
// Function prototypes for the HID services exported by the hid class driver
// follow.
// 

NTSTATUS
HidRegisterMinidriver(
    IN PHID_MINIDRIVER_REGISTRATION  MinidriverRegistration
    );

//
// Internal IOCTLs for the class/mini driver interface.  
//

#define IOCTL_HID_GET_DEVICE_DESCRIPTOR     HID_CTL_CODE(0)
#define IOCTL_HID_GET_REPORT_DESCRIPTOR     HID_CTL_CODE(1)
#define IOCTL_HID_READ_REPORT               HID_CTL_CODE(2)
#define IOCTL_HID_WRITE_REPORT              HID_CTL_CODE(3)
#define IOCTL_HID_GET_STRING                HID_CTL_CODE(4)
#define IOCTL_HID_OPEN_COLLECTION           HID_CTL_CODE(5)
#define IOCTL_HID_CLOSE_COLLECTION          HID_CTL_CODE(6)

#endif  // __HIDPORT_H__
