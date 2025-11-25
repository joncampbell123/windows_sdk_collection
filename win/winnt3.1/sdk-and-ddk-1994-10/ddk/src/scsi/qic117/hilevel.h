/*++

Copyright (c) 1993  

Module Name:

    hilevel.h

Abstract:

    Prototypes for the entry points to the High-Level portion (data
    formatter) of the QIC-117 device driver.

Revision History:


--*/

NTSTATUS
q117Initialize(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT q117iDeviceObject,
    IN PUNICODE_STRING RegistryPath,
    PADAPTER_OBJECT AdapterObject,
    ULONG           NumberOfMapRegisters
    );


NTSTATUS
q117Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
q117Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
q117DeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
q117Create (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
q117Close (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
q117Cleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//
// Make compiler happy (this should be in the NT DDK but I could not
//  find it)
//
int
sprintf(
    char *s,
    const char *format,
    ...
    );

