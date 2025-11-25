/*++

Copyright (c) 1995      Microsoft Corporation

Module Name:

        HCDI.H

Abstract:

   structures common to the usbd and hcd device drivers.

Environment:

    Kernel & user mode

Revision History:

    09-29-95 : created

--*/

#ifndef   __HCDI_H__
#define   __HCDI_H__

typedef struct _USBD_EXTENSION {
    // ptr to true device extension or NULL if this
    // is the true extension
    PVOID TrueDeviceExtension;

    //
    // HCD device object we are connected to.
    //
    PDEVICE_OBJECT HcdDeviceObject;

    //
    // Used to serialize open/close endpoint and
    // device configuration
    //
    KSEMAPHORE UsbDeviceMutex;
    
    //
    // Bitmap of assigned USB addresses
    //
    ULONG AddressList[4];

    //
    // Remember the Root Hub PDO we created.
    //

    PDEVICE_OBJECT RootHubPDO;

    PDRIVER_OBJECT DriverObject;

    //
    // symbolic link created for HCD stack
    //
    
    UNICODE_STRING DeviceLinkUnicodeString;

    BOOLEAN DiagnosticMode;

    BOOLEAN SupportNonComplientDevices;

    UCHAR Pad[2];

    //
    // Store away the PDO
    //
    PDEVICE_OBJECT PhysicalDeviceObject;

} USBD_EXTENSION, *PUSBD_EXTENSION;

//
// This macro returns the true device object for the HCD give 
// either the true device_object or a PDO owned by the HCD/BUS 
// driver.
//

//
// HCD specific URB commands
//

#define URB_FUNCTION_HCD_OPEN_ENDPOINT                0x1000
#define URB_FUNCTION_HCD_CLOSE_ENDPOINT               0x1001
#define URB_FUNCTION_HCD_GET_ENDPOINT_STATE           0x1002
#define URB_FUNCTION_HCD_SET_ENDPOINT_STATE           0x1003
#define URB_FUNCTION_HCD_ABORT_ENDPOINT               0x1004

// this bit is set for all functions that must be handled by HCD
#define HCD_URB_FUNCTION                              0x1000  
// this bit is set in the function code by USBD to indicate that
// this is an internal call originating from USBD 
#define HCD_NO_USBD_CALL                              0x2000  

//
// values for HcdEndpointState
//

//
// set if the current state of the endpoint in the HCD is 'stalled'
//
#define HCD_ENDPOINT_HALTED_BIT            0
#define HCD_ENDPOINT_HALTED                (1<<HCD_ENDPOINT_HALTED_BIT)

//
// set if the HCD has any transfers queued for the endpoint
//
#define HCD_ENDPOINT_TRANSFERS_QUEUED_BIT   1
#define HCD_ENDPOINT_TRANSFERS_QUEUED       (1<<HCD_ENDPOINT_TRANSFERS_QUEUED_BIT)


//
// HCD specific URBs
//
    
struct _URB_HCD_OPEN_ENDPOINT {
    struct _URB_HEADER;
    USHORT DeviceAddress;
    BOOLEAN LowSpeed;
    BOOLEAN NeverHalt;
    PUSB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
    ULONG MaxTransferSize;
    PVOID HcdEndpoint;
};

struct _URB_HCD_CLOSE_ENDPOINT {
    struct _URB_HEADER;
    PVOID HcdEndpoint;
};

struct _URB_HCD_ENDPOINT_STATE {
    struct _URB_HEADER;
    PVOID HcdEndpoint;
    ULONG HcdEndpointState;
};

struct _URB_HCD_ABORT_ENDPOINT {
    struct _URB_HEADER;
    PVOID HcdEndpoint;
};


//
// Common transfer request definition, all transfer
// requests passed to the HCD will be mapped to this
// format.  The HCD will can use this structure to
// reference fields that are common to all transfers
// as well as fields specific to isochronous and
// control transfers.
//

typedef struct _COMMON_TRANSFER_EXTENSION {
    union {
        struct {
            ULONG StartFrame;
            ULONG NumberOfPackets;
            ULONG ErrorCount;
            USBD_ISO_PACKET_DESCRIPTOR IsoPacket[0];     
        } Isoch;
        UCHAR SetupPacket[8];    
    } u;
} COMMON_TRANSFER_EXTENSION, *PCOMMON_TRANSFER_EXTENSION;


struct _URB_HCD_COMMON_TRANSFER {
    struct _URB_HEADER;
    PVOID UsbdPipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;
    struct _HCD_URB *UrbLink;   // link to next urb request
                                // if this is a chain of requests
    struct _URB_HCD_AREA hca;       // fields for HCD use

    COMMON_TRANSFER_EXTENSION Extension; 
/*    
    //BUGBUG add fields for isoch and
    //control transfers
    UCHAR SetupPacket[8];

    ULONG StartFrame;
    // number of packets that make up this request
    ULONG NumberOfPackets;
    // number of packets that completed with errors
    ULONG ErrorCount;
    USBD_ISO_PACKET_DESCRIPTOR IsoPacket[0]; 
*/    
};

typedef struct _HCD_URB {
    union {
            struct _URB_HEADER                      UrbHeader;
            struct _URB_HCD_OPEN_ENDPOINT           HcdUrbOpenEndpoint;
            struct _URB_HCD_CLOSE_ENDPOINT          HcdUrbCloseEndpoint;
            struct _URB_GET_FRAME_LENGTH            UrbGetFrameLength;
            struct _URB_SET_FRAME_LENGTH            UrbSetFrameLength;
            struct _URB_GET_CURRENT_FRAME_NUMBER    UrbGetCurrentFrameNumber;
            struct _URB_HCD_ENDPOINT_STATE          HcdUrbEndpointState;
            struct _URB_HCD_ABORT_ENDPOINT          HcdUrbAbortEndpoint;
            //formats for USB transfer requests.
            struct _URB_HCD_COMMON_TRANSFER         HcdUrbCommonTransfer;
            //formats for specific transfer types
            //that have fields not contained in
            //CommonTransfer.
            //BUGBUG this will be merged with commontransfer
            struct _URB_ISOCH_TRANSFER              UrbIsochronousTransfer;

    };
} HCD_URB, *PHCD_URB;

#endif /* __HCDI_H__ */

