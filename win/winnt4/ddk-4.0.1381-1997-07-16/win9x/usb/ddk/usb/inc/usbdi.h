/*++

Copyright (c) 1995      Microsoft Corporation

Module Name:

        USBDI.H

Abstract:

   structures common to the USBD and USB device drivers.

Environment:

    Kernel & user mode

Revision History:

    09-29-95 : created

--*/

#ifndef   __USBDI_H__
#define   __USBDI_H__

#ifndef _NTDDK_
#ifndef _WDMDDK_
typedef PVOID PIRP;
typedef PVOID PMDL;
#endif
#endif

#define USBDI_VERSION    0x100

#include "usbioctl.h"
//
// USB defined structures and constants
// (see chapter 9 of USB specification)
//

#define USB_DEFAULT_DEVICE_ADDRESS     0
#define USB_DEFAULT_ENDPOINT_ADDRESS   0

//
// max packet size (bytes) for default endpoint
// until SET_ADDRESS command is received.
//

#define USB_DEFAULT_MAX_PACKET         64

//
// USB descriptor types, bDescriptorType field
// in USB descriptor structure
//

#define USB_DEVICE_DESCRIPTOR_TYPE                0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE         0x02
#define USB_STRING_DESCRIPTOR_TYPE                0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE             0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE              0x05

#define USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((USHORT)((USHORT)d<<8 | i))

//
// Values for bmAttributes field of an
// endpoint descriptor
//

#define USB_ENDPOINT_TYPE_MASK                    0x03

#define USB_ENDPOINT_TYPE_CONTROL                 0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01
#define USB_ENDPOINT_TYPE_BULK                    0x02
#define USB_ENDPOINT_TYPE_INTERRUPT               0x03

//
// Endpoint direction bit, stored in address
//

#define USB_ENDPOINT_DIRECTION_MASK               0x80

// test direction bit in the bEndpointAddress field of
// an endpoint descriptor.
#define USB_ENDPOINT_DIRECTION_OUT(addr)          (!((addr) & USB_ENDPOINT_DIRECTION_MASK))
#define USB_ENDPOINT_DIRECTION_IN(addr)           ((addr) & USB_ENDPOINT_DIRECTION_MASK)

//
// USB defined request codes
// see chapter 9 of the USB 1.0 specifcation for
// more information.
//

// These are the correct values based on the USB 1.0
// specification

#define USB_REQUEST_GET_STATUS                    0x00
#define USB_REQUEST_CLEAR_FEATURE                 0x01

#define USB_REQUEST_SET_FEATURE                   0x03

#define USB_REQUEST_SET_ADDRESS                   0x05
#define USB_REQUEST_GET_DESCRIPTOR                0x06
#define USB_REQUEST_SET_DESCRIPTOR                0x07
#define USB_REQUEST_GET_CONFIGURATION             0x08
#define USB_REQUEST_SET_CONFIGURATION             0x09
#define USB_REQUEST_GET_INTERFACE                 0x0A
#define USB_REQUEST_SET_INTERFACE                 0x0B
#define USB_REQUEST_SYNC_FRAME                    0x0C


#include <PSHPACK1.H>

#define USB_DEVICE_CLASS_RESERVED           0
#define USB_DEVICE_CLASS_AUDIO              1
#define USB_DEVICE_CLASS_COMMUNICATIONS     2
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    3
#define USB_DEVICE_CLASS_MONITOR            4
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 5
#define USB_DEVICE_CLASS_POWER              6
#define USB_DEVICE_CLASS_PRINTER            7
#define USB_DEVICE_CLASS_STORAGE            8
#define USB_DEVICE_CLASS_HUB                9
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    FFH

typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bEndpointAddress;
    UCHAR bmAttributes;
    USHORT wMaxPacketSize;
    UCHAR bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

//
// values for bmAttributes Field in
// USB_CONFIGURATION_DESCRIPTOR
//

#define BUS_POWERED                           0x80
#define SELF_POWERED                          0x40
#define REMOTE_WAKEUP                         0x20

typedef struct _USB_CONFIGURATION_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bInterfaceNumber;
    UCHAR bAlternateSetting;
    UCHAR bNumEndpoints;
    UCHAR bInterfaceClass;
    UCHAR bInterfaceSubClass;
    UCHAR bInterfaceProtocol;
    UCHAR iInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

typedef struct _USB_STRING_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    WCHAR bString[1];
} USB_STRING_DESCRIPTOR, *PUSB_STRING_DESCRIPTOR;

typedef struct _USB_COMMON_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
} USB_COMMON_DESCRIPTOR, *PUSB_COMMON_DESCRIPTOR;

#include <POPPACK.H>


//
// USBD interface structures and constants
//


#define URB_FROM_IRP(Irp) ((IoGetCurrentIrpStackLocation(Irp))->Parameters.Others.Argument1)

//
//  URB request codes
//
                                                    
#define URB_FUNCTION_SELECT_CONFIGURATION            0x0000
#define URB_FUNCTION_SELECT_INTERFACE                0x0001
#define URB_FUNCTION_ABORT_PIPE                      0x0002
#define URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL       0x0003
#define URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL    0x0004
#define URB_FUNCTION_GET_FRAME_LENGTH                0x0005
#define URB_FUNCTION_SET_FRAME_LENGTH                0x0006
#define URB_FUNCTION_GET_CURRENT_FRAME_NUMBER        0x0007
#define URB_FUNCTION_CONTROL_TRANSFER                0x0008
#define URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER      0x0009
#define URB_FUNCTION_ISOCH_TRANSFER                  0x000A
#define URB_FUNCTION_RESET_PIPE                      0x001E

//
// These functions correspond
// to the standard commands on the default pipe
//
// direction is implied
//

#define URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE     0x000B
#define URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT   0x0024
                                                           
#define URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE       0x000C
#define URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT     0x0025

#define URB_FUNCTION_SET_FEATURE_TO_DEVICE          0x000D
#define URB_FUNCTION_SET_FEATURE_TO_INTERFACE       0x000E
#define URB_FUNCTION_SET_FEATURE_TO_ENDPOINT        0x000F
#define URB_FUNCTION_SET_FEATURE_TO_OTHER           0x0023

#define URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE        0x0010
#define URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE     0x0011
#define URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT      0x0012
#define URB_FUNCTION_CLEAR_FEATURE_TO_OTHER         0x0022

#define URB_FUNCTION_GET_STATUS_FROM_DEVICE         0x0013
#define URB_FUNCTION_GET_STATUS_FROM_INTERFACE      0x0014
#define URB_FUNCTION_GET_STATUS_FROM_ENDPOINT       0x0015
#define URB_FUNCTION_GET_STATUS_FROM_OTHER          0x0021

// direction is specified in TransferFlags

#define URB_FUNCTION_SYNC_FRAME                     0x0016

//
// These are for sending vendor and class commands
// on the default pipe
//
// direction is specified in TransferFlags
//

#define URB_FUNCTION_VENDOR_DEVICE                   0x0017
#define URB_FUNCTION_VENDOR_INTERFACE                0x0018
#define URB_FUNCTION_VENDOR_ENDPOINT                 0x0019
#define URB_FUNCTION_VENDOR_OTHER                    0x0020

#define URB_FUNCTION_CLASS_DEVICE                    0x001A
#define URB_FUNCTION_CLASS_INTERFACE                 0x001B
#define URB_FUNCTION_CLASS_ENDPOINT                  0x001C
#define URB_FUNCTION_CLASS_OTHER                     0x001F

//
// Reserved function codes
//                                                                                                        
#define URB_FUNCTION_RESERVED                        0x001D

#define URB_FUNCTION_GET_CONFIGURATION               0x0026
#define URB_FUNCTION_GET_INTERFACE                   0x0027
                    
#define URB_FUNCTION_LAST                            0x0027

//
// Values for URB TransferFlags Field
//
#define USBD_TRANSFER_DIRECTION_BIT           0
#define USBD_TRANSFER_DIRECTION_IN            (1<<USBD_TRANSFER_DIRECTION_BIT)

#define USBD_SHORT_TRANSFER_OK_BIT            1
#define USBD_SHORT_TRANSFER_OK                (1<<USBD_SHORT_TRANSFER_OK_BIT)

#define USBD_START_ISO_TRANSFER_ASAP_BIT      2
#define USBD_START_ISO_TRANSFER_ASAP          (1<<USBD_START_ISO_TRANSFER_ASAP_BIT)


#define USBD_ISO_START_FRAME_RANGE            1024

typedef LONG USBD_STATUS;

//
// USBD status codes
//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+---------------------------+-------------------------------+
//  | S |               Status Code                                 |
//  +---+---------------------------+-------------------------------+
//
//  where
//
//      S - is the state code
//
//          00 - completed with success
//          01 - request is pending
//          10 - completed with error, endpoint not stalled
//          11 - completed with error, endpoint stalled
//
//
//      Code - is the status code
//

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define USBD_SUCCESS(Status) ((USBD_STATUS)(Status) >= 0)

//
// Generic test for pending status value.
//

#define USBD_PENDING(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for error on any status value.
//

#define USBD_ERROR(Status) ((USBD_STATUS)(Status) < 0)

//
// Generic test for stall on any status value.
//

#define USBD_HALTED(Status) ((ULONG)(Status) >> 30 == 3)

//
// Macro to check the status code only
//

#define USBD_STATUS(Status) ((ULONG)(Status) & 0x0FFFFFFFL)


#define USBD_STATUS_SUCCESS                  ((USBD_STATUS)0x00000000L)
#define USBD_STATUS_PENDING                  ((USBD_STATUS)0x40000000L)
#define USBD_STATUS_HALTED                   ((USBD_STATUS)0xC0000000L)
#define USBD_STATUS_ERROR                    ((USBD_STATUS)0x80000000L)

//
// HC status codes
// Note: these status codes have both the error and the stall bit set.
//
#define USBD_STATUS_CRC                      ((USBD_STATUS)0xC0000001L)
#define USBD_STATUS_BTSTUFF                  ((USBD_STATUS)0xC0000002L)
#define USBD_STATUS_DATA_TOGGLE_MISMATCH     ((USBD_STATUS)0xC0000003L)
#define USBD_STATUS_STALL_PID                ((USBD_STATUS)0xC0000004L)
#define USBD_STATUS_DEV_NOT_RESPONDING       ((USBD_STATUS)0xC0000005L)
#define USBD_STATUS_PID_CHECK_FAILURE        ((USBD_STATUS)0xC0000006L)
#define USBD_STATUS_UNEXPECTED_PID           ((USBD_STATUS)0xC0000007L)
#define USBD_STATUS_DATA_OVERRUN             ((USBD_STATUS)0xC0000008L)
#define USBD_STATUS_DATA_UNDERRUN            ((USBD_STATUS)0xC0000009L)
#define USBD_STATUS_RESERVED1                ((USBD_STATUS)0xC000000AL)
#define USBD_STATUS_RESERVED2                ((USBD_STATUS)0xC000000BL)
#define USBD_STATUS_BUFFER_OVERRUN           ((USBD_STATUS)0xC000000CL)
#define USBD_STATUS_BUFFER_UNDERRUN          ((USBD_STATUS)0xC000000DL)
#define USBD_STATUS_NOT_ACCESSED             ((USBD_STATUS)0xC000000FL)
#define USBD_STATUS_FIFO                     ((USBD_STATUS)0xC0000010L)

//
// returned by HCD if a transfer is submitted to an endpoint that is 
// stalled
//
#define USBD_STATUS_ENDPOINT_HALTED         ((USBD_STATUS)0xC0000030L)

//
// Software status codes
// Note: the following status codes have only the error bit set
//
#define USBD_STATUS_NO_MEMORY                ((USBD_STATUS)0x80000100L)
#define USBD_STATUS_INVALID_URB_FUNCTION     ((USBD_STATUS)0x80000200L)
#define USBD_STATUS_INVALID_PARAMETER        ((USBD_STATUS)0x80000300L)

//
// returned if client driver attempts to close an endpoint/interface
// or configuration with outstanding transfers.
//
#define USBD_STATUS_ERROR_BUSY               ((USBD_STATUS)0x80000400L)
//
// returned by USBD if it cannot complete a URB request, typically this 
// will be returned in the URB status field when the Irp is completed
// with a more specific NT error code in the irp.status field.
//
#define USBD_STATUS_REQUEST_FAILED           ((USBD_STATUS)0x80000500L)

#define USBD_STATUS_INVALID_PIPE_HANDLE      ((USBD_STATUS)0x80000600L)

// returned when there is not enough bandwidth avialable
// to open a requested endpoint
#define USBD_STATUS_NO_BANDWIDTH             ((USBD_STATUS)0x80000700L)
//
// generic HC error
// 
#define USBD_STATUS_INTERNAL_HC_ERROR        ((USBD_STATUS)0x80000800L)
//
// returned when a short packet terminates the transfer
// ie USBD_SHORT_TRANSFER_OK bit not set
// 
#define USBD_STATUS_ERROR_SHORT_TRANSFER     ((USBD_STATUS)0x80000900L)
// 
// returned if the requested start frame is not within
// USBD_ISO_START_FRAME_RANGE of the current USB frame, 
// note that the stall bit is set
// 
#define USBD_STATUS_BAD_START_FRAME          ((USBD_STATUS)0xC0000A00L)

//
// returned by HCD if all packets in an iso transfer complete with an error 
//
#define USBD_STATUS_ISOCH_REQUEST_FAILED     ((USBD_STATUS)0xC0000B00L)

//
// set when a transfers is completed due to an AbortPipe request from
// the client driver
//
// Note: no error or stall bit is set for these status codes
//
#define USBD_STATUS_CANCELED                 ((USBD_STATUS)0x00010000L)

#define USBD_STATUS_CANCELING                ((USBD_STATUS)0x00020000L)

typedef PVOID USBD_PIPE_HANDLE;
typedef PVOID USBD_CONFIGURATION_HANDLE;
typedef PVOID USBD_INTERFACE_HANDLE;

//
// Value used to indicate the default max transfer size
//

#define USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE  PAGE_SIZE


// 
// structure returned from USBD_GetVersion function
//

typedef struct _USBD_VERSION_INFORMATION {
    ULONG USBDI_Version;          //BCD usb interface version number
    ULONG Supported_USB_Version;  //BCD USB spec version number
} USBD_VERSION_INFORMATION, *PUSBD_VERSION_INFORMATION;

typedef enum _USBD_PIPE_TYPE {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
} USBD_PIPE_TYPE;

#define USBD_PIPE_DIRECTION_IN(pipeInformation) ((pipeInformation)->EndpointAddress & \
                                                  USB_ENDPOINT_DIRECTION_MASK) 

typedef struct _USBD_DEVICE_INFORMATION {
    ULONG OffsetNext;
    PVOID UsbdDeviceHandle;
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;
} USBD_DEVICE_INFORMATION, *PUSBD_DEVICE_INFORMATION;

//
//      URB request structures
//

//
// USBD pipe information structure, this structure
// is returned for each pipe opened thru an
// SELECT_CONFIGURATION or SELECT_INTERFACE request.
//

typedef struct _USBD_PIPE_INFORMATION {
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    USHORT MaximumPacketSize;  // Maximum packet size for this pipe
    UCHAR EndpointAddress;     // 8 bit USB endpoint address (includes direction)
                               // taken from endpoint descriptor
    UCHAR Interval;            // Polling interval in ms if interrupt pipe 
    
    USBD_PIPE_TYPE PipeType;   // PipeType identifies type of transfer valid for this pipe
    USBD_PIPE_HANDLE PipeHandle;
    
    //
    // INPUT
    // These fields are filled in by the client driver
    //
    ULONG MaximumTransferSize; // Maximum size for a single request
                               // in bytes.
    ULONG PipeFlags;                                   
} USBD_PIPE_INFORMATION, *PUSBD_PIPE_INFORMATION;

//
// USBD interface information structure, this structure
// is returned for each interface opened thru an
// SELECT_CONFIGURATION or SELECT_INTERFACE request.
//

typedef struct _USBD_INTERFACE_INFORMATION {
    USHORT Length;       // Length of this structure, including
                         // all pipe information structures that
                         // follow.
    //
    // INPUT
    //
    // Interface number and Alternate setting this
    // structure is associated with
    //
    UCHAR InterfaceNumber;
    UCHAR AlternateSetting;
    
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    UCHAR Class;
    UCHAR SubClass;
    UCHAR Protocol;
    UCHAR Reserved;
    
    USBD_INTERFACE_HANDLE InterfaceHandle;
    ULONG NumberOfPipes; 

    //
    // INPUT/OUPUT
    // see PIPE_INFORMATION
    USBD_PIPE_INFORMATION Pipes[0];
} USBD_INTERFACE_INFORMATION, *PUSBD_INTERFACE_INFORMATION;

//
// work space in transfer request provided
// for HCDs
//

struct _URB_HCD_AREA {
    PVOID HcdEndpoint;
    PIRP HcdIrp;
    LIST_ENTRY HcdListEntry;
    LIST_ENTRY HcdListEntry2;
    PVOID HcdCurrentIoFlushPointer;
    PVOID HcdExtension;
};

struct _URB_HEADER {
    //
    // Fields filled in by client driver
    //
    USHORT Length;
    USHORT Function;
    USBD_STATUS Status;
    //
    // Fields used only by USBD
    //
    PVOID UsbdDeviceHandle; // device handle assigned to this device
                            // by USBD
    ULONG UsbdFlags;        // flags field reserved for USBD use.
};

struct _URB_SELECT_INTERFACE {
    struct  _URB_HEADER;
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;

    // client must input AlternateSetting & Interface Number
    // class driver returns interface and handle
    // for new alternate setting
    USBD_INTERFACE_INFORMATION Interface;
};

struct _URB_SELECT_CONFIGURATION {
    struct  _URB_HEADER;
    // NULL indicates to set the device
    // to the 'unconfigured' state
    // ie set to configuration 0
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;
    USBD_INTERFACE_INFORMATION Interface;
};

//
// This structure used for ABORT_PIPE & RESET_PIPE
//

struct _URB_PIPE_REQUEST {
    struct _URB_HEADER;
    USBD_PIPE_HANDLE PipeHandle;
};

//
// This structure used for
// TAKE_FRAME_LENGTH_CONTROL &
//        RELEASE_FRAME_LENGTH_CONTROL
//

struct _URB_FRAME_LENGTH_CONTROL {
    struct _URB_HEADER;
};

struct _URB_GET_FRAME_LENGTH {
    struct _URB_HEADER;
    ULONG FrameLength;
    ULONG FrameNumber;
};

struct _URB_SET_FRAME_LENGTH {
    struct _URB_HEADER;
    LONG FrameLengthDelta;
};

struct _URB_GET_CURRENT_FRAME_NUMBER {
    struct _URB_HEADER;
    ULONG FrameNumber;
};

//
// Structures for specific control transfers
// on the default pipe.
//

// GET_DESCRIPTOR
// SET_DESCRIPTOR

struct _URB_CONTROL_DESCRIPTOR_REQUEST {
    struct _URB_HEADER;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;               // fields for HCD use
    USHORT Reserved1;
    UCHAR Index;
    UCHAR DescriptorType;
    USHORT LanguageId;
    USHORT Reserved2;
};

// GET_STATUS

struct _URB_CONTROL_GET_STATUS_REQUEST {
    struct _URB_HEADER;
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[4];
    USHORT Index;                       // zero, interface or endpoint
    USHORT Reserved2;
};

// SET_FEATURE
// CLEAR_FEATURE

struct _URB_CONTROL_FEATURE_REQUEST {
    struct _URB_HEADER;
    UCHAR Reserved[20];
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    USHORT Reserved0;
    USHORT FeatureSelector;
    USHORT Index;                       // zero, interface or endpoint
    USHORT Reserved1;
};

// VENDOR & CLASS

struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST {
    struct _URB_HEADER;
    PVOID Reserved;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR RequestTypeReservedBits;
    UCHAR Request;
    USHORT Value;
    USHORT Index;
    USHORT Reserved1;
};

// SYNC_FRAME

//
// BUGBUG we need a separate api for this one
//
struct _URB_CONTROL_SYNC_FRAME_REQUEST {
    struct _URB_HEADER;
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[8];
//BUGBUG fix this
    USHORT Endpoint;                    // zero, interface or endpoint
    USHORT Reserved2;
};


struct _URB_CONTROL_GET_INTERFACE_REQUEST {
    struct _URB_HEADER;
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[4];    
    USHORT Interface;
    USHORT Reserved2;
};


struct _URB_CONTROL_GET_CONFIGURATION_REQUEST {
    struct _URB_HEADER;
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[8];    
};


//
// request format for a control transfer on
// the non-default pipe.
//

struct _URB_CONTROL_TRANSFER {
    struct _URB_HEADER ;
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR SetupPacket[8];
};


struct _URB_BULK_OR_INTERRUPT_TRANSFER {
    struct _URB_HEADER ;
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;                // note: the direction bit will be set by USBD
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
};


//
// ISO Transfer request
//
// TransferBufferMDL must point to a single virtually 
// contiguous buffer.
//
// StartFrame - the frame to send/receive the first packet of 
// the request. 
//
// NumberOfPackets - number of packets to send in this request
//
//
// IsoPacket Array
//
//      Input:  Offset - offset of the packet from the beginig
//                 of the client buffer.
//      Output: Length -  is set to the actual length of the packet
//                (For IN transfers). 
//      Status: error that occurred during transmission or 
//              reception of the packet.
//      

typedef struct _USBD_ISO_PACKET_DESCRIPTOR {
    ULONG Offset;       // INPUT Offset of the packet from the begining of the
                        // buffer.

    ULONG Length;       // OUTPUT length of data received (for in).
                        // OUTPUT 0 for OUT.
    USBD_STATUS Status; // status code for this packet.     
} USBD_ISO_PACKET_DESCRIPTOR, *PUSBD_PACKET_DESCRIPTOR;

struct _URB_ISOCH_TRANSFER {
    //
    // This block is the same as CommonTransfer
    //
    struct _URB_HEADER; 
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    ULONG ReservedMBZ;                   
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use

    //
    // this block contains transfer fields
    // specific to isochronous transfers
    //

    // 32 bit frame number to begin this transfer on, must be within 1000
    // frames of the current USB frame or an error is returned.

    // START_ISO_TRANSFER_ASAP flag in transferFlags:
    // If this flag is set and no transfers have been submitted
    // for the pipe then the transfer will begin on the next frame
    // and StartFrame will be updated with the frame number the transfer
    // was started on.
    // If this flag is set and the pipe has active transfers then 
    // the transfer will be queued to begin on the frame after the 
    // last transfer queued is completed.
    //
    ULONG StartFrame;
    // number of packets that make up this request
    ULONG NumberOfPackets;
    // number of packets that completed with errors
    ULONG ErrorCount;
    USBD_ISO_PACKET_DESCRIPTOR IsoPacket[0]; 
};


typedef struct _URB {
    union {
            struct _URB_HEADER                           UrbHeader;
            struct _URB_SELECT_INTERFACE                 UrbSelectInterface;
            struct _URB_SELECT_CONFIGURATION             UrbSelectConfiguration;
            struct _URB_PIPE_REQUEST                     UrbPipeRequest;
            struct _URB_FRAME_LENGTH_CONTROL             UrbFrameLengthControl;
            struct _URB_GET_FRAME_LENGTH                 UrbGetFrameLength;
            struct _URB_SET_FRAME_LENGTH                 UrbSetFrameLength;
            struct _URB_GET_CURRENT_FRAME_NUMBER         UrbGetCurrentFrameNumber;
            struct _URB_CONTROL_TRANSFER                 UrbControlTransfer;
            struct _URB_BULK_OR_INTERRUPT_TRANSFER       UrbBulkOrInterruptTransfer;
            struct _URB_ISOCH_TRANSFER                   UrbIsochronousTransfer;

            // for standard control transfers on the default pipe
            struct _URB_CONTROL_DESCRIPTOR_REQUEST       UrbControlDescriptorRequest;
            struct _URB_CONTROL_GET_STATUS_REQUEST       UrbControlGetStatusRequest;
            struct _URB_CONTROL_FEATURE_REQUEST          UrbControlFeatureRequest;
            struct _URB_CONTROL_SYNC_FRAME_REQUEST       UrbControlSyncFrameRequest;
            struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST  UrbControlVendorClassRequest;
            struct _URB_CONTROL_GET_INTERFACE_REQUEST    UrbControlGetInterfaceRequest;
            struct _URB_CONTROL_GET_CONFIGURATION_REQUEST UrbControlGetConfigurationRequest;
    };
} URB, *PURB;


#endif /*  __USBDI_H__ */

