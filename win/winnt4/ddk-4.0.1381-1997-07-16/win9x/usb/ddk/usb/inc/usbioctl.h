/*++

Copyright (c) 1995      Microsoft Corporation

Module Name:

        USBIOCTL.H

Abstract:

   structures common to the USBD and USB device drivers.

Environment:

    Kernel & user mode

Revision History:

    09-29-95 : created

--*/

#ifndef   __USBIOCTL_H__
#define   __USBIOCTL_H__

// BUGBUG we need a device type added to DEVIOCTL.H
#define FILE_DEVICE_USB         FILE_DEVICE_UNKNOWN

//
// USB IOCTLS
//

#define USB_IOCTL_INTERNAL_INDEX       0x0000
#define USB_IOCTL_INDEX                0x00ff

//
// USB Internal IOCtls
//

//
// This IOCTL is used by client drivers to submit URB (USB Request Blocks)
//

#define IOCTL_INTERNAL_USB_SUBMIT_URB  CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INTERNAL_INDEX,  \
                                                METHOD_NEITHER,  \
                                                FILE_ANY_ACCESS)

#define IOCTL_INTERNAL_USB_RESET_PORT  CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INTERNAL_INDEX+1, \
                                                METHOD_NEITHER,  \
                                                FILE_ANY_ACCESS)

//
// This IOCTL is used to retrieve a list of all devices on the bus.
//
 
#define IOCTL_INTERNAL_USB_ENUMERATE   CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INTERNAL_INDEX+2, \
                                                METHOD_NEITHER,  \
                                                FILE_ANY_ACCESS)

//
// USB Public IOCtls
//

//
// this ioctl is for adding debug hooks to HCDs
//

#define IOCTL_USB_HCD_FEATURE          CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INDEX,  \
                                                METHOD_BUFFERED,  \
                                                FILE_ANY_ACCESS)

//
// These ioctls are used for USB ddiagnostic and test applications
// 

#define IOCTL_USB_DIAGNOSTIC_MODE_ON   CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INDEX+1,  \
                                                METHOD_BUFFERED,  \
                                                FILE_ANY_ACCESS)                                                

#define IOCTL_USB_DIAGNOSTIC_MODE_OFF  CTL_CODE(FILE_DEVICE_USB,  \
                                                USB_IOCTL_INDEX+2,  \
                                                METHOD_BUFFERED,  \
                                                FILE_ANY_ACCESS)



#endif // __USBIOCTL_H__                                               
