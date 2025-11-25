/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    I82930.h

Abstract:



Environment:

    Kernel & user mode

Revision History:

    5-10-96 : created

--*/

#ifdef DRIVER

#define I82930_NAME_MAX  64

//
// we support up to 10 pipe handles
//
#define I82930_MAX_PIPES 10


typedef struct _I82930_PIPE {
    BOOLEAN Opened;
    UCHAR Pad[3];
    PUSBD_PIPE_INFORMATION PipeInfo;
    WCHAR Name[I82930_NAME_MAX];
} I82930_PIPE, *PI82930_PIPE;

#define MAX_INTERFACE 2

//
// A structure representing the instance information associated with
// this particular device.
//

typedef struct _DEVICE_EXTENSION {

    // Device object we call when submitting Urbs
    PDEVICE_OBJECT StackDeviceObject;

    PDEVICE_OBJECT PhysicalDeviceObject;

    // configuration handle for the configuration the
    // device is currently in
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;

    // ptr to the USB device descriptor
    // for this device
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor;

    // we support one interface
    // this is a copy of the info structure
    // returned from select_configuration or
    // select_interface
    PUSBD_INTERFACE_INFORMATION Interface;

    // Name buffer for our named Functional device object link
    WCHAR DeviceLinkNameBuffer[I82930_NAME_MAX];

    BOOLEAN NeedCleanup;

    UCHAR Pad[3];

    I82930_PIPE PipeList[I82930_MAX_PIPES];

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


#if DBG

#define I82930_KdPrint(_x_) DbgPrint("I82930.SYS: "); \
                             DbgPrint _x_ ;

#ifdef NTKERN
#define TRAP() _asm {int 3}
#else
#define TRAP() DbgBreakPoint()
#endif

#else

#define I82930_KdPrint(_x_)

#define TRAP()

#endif

NTSTATUS
I82930_Dispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    );

VOID
I82930_Unload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
I82930_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    );

NTSTATUS
I82930_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
I82930_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT *DeviceObject,
    LONG Instance
    );

NTSTATUS
I82930_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
I82930_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
I82930_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
I82930_ProcessIOCTL(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
I82930_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    IN PUSBD_INTERFACE_INFORMATION Interface
    );

PUSB_CONFIGURATION_DESCRIPTOR
I82930_GetConfigDescriptor(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_BuildPipeList(
    IN  PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
I82930_Cleanup(
    PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
I82930_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PI82930_PIPE Pipe
    );

#endif

