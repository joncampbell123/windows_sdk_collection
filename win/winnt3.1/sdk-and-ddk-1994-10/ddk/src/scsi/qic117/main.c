/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    main.c

Abstract:

    This is the tape class driver.

Revision History:




--*/

//
// Includes
//

#include <ntddk.h>
#include <ntddtape.h>   // tape device driver I/O control codes
#include <ntiologc.h>
#include "common.h"
#include "q117.h"
#include "cms.h"
#include "protos.h"
#include "hilevel.h"
#include "q117log.h"

//
// Protos for entry points
//

NTSTATUS
q117CreateNumericKey(
    IN HANDLE Root,
    IN ULONG Name,
    IN PWSTR Prefix,
    OUT PHANDLE NewKey
    );

NTSTATUS
q117CreateRegistryInfo(
    IN ULONG TapeNumber,
    IN PUNICODE_STRING RegistryPath,
    IN PQ117_CONTEXT Context
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,q117Initialize)
#endif


//
// Start of code
//


NTSTATUS
q117Initialize(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT q117iDeviceObject,
    IN PUNICODE_STRING RegistryPath,
    PADAPTER_OBJECT AdapterObject,
    ULONG           NumberOfMapRegisters
    )

/*++

Routine Description:

    This routine initializes the SCSI Tape class driver.

Arguments:

    DriverObject

Return Value:

    NT Status

--*/

{
    PDEVICE_OBJECT  q117DeviceObject;
    NTSTATUS        status;
    UNICODE_STRING  ntUnicodeString;
    UCHAR           ntNameBuffer[256];
    STRING          ntNameString;
    BOOLEAN         tapeDeviceFound = FALSE;
    PQ117_CONTEXT   context;
    ULONG           tapeNumber;
    STRING          dosString;
    UNICODE_STRING  dosUnicodeString;
    CCHAR           dosNameBuffer[64];

    //
    // Build the unicode name for the floppy tape.
    //
    tapeNumber = IoGetConfigurationInformation()->TapeCount;

    sprintf(ntNameBuffer,
            "\\Device\\Tape%d",
            tapeNumber);

    RtlInitString(&ntNameString, ntNameBuffer);

    status = RtlAnsiStringToUnicodeString(
        &ntUnicodeString,
        &ntNameString,
        TRUE );

    if (!NT_SUCCESS(status)) {
        return(status);
    }

    //
    // Create a device object for this floppy drive.
    //

    status = IoCreateDevice(
        DriverObject,
        sizeof( struct _Q117_CONTEXT ),
        &ntUnicodeString,
        FILE_DEVICE_UNKNOWN,
        FILE_REMOVABLE_MEDIA,
        FALSE,
        &q117DeviceObject );


    if (!NT_SUCCESS(status)) {
        return(status);
    }

    //
    // Create the DOS port driver name.
    //

    sprintf(dosNameBuffer,
            "\\DosDevices\\TAPE%d",
            tapeNumber);

    RtlInitString(&dosString, dosNameBuffer);

    status = RtlAnsiStringToUnicodeString(&dosUnicodeString,
                                          &dosString,
                                          TRUE);

    if (!NT_SUCCESS(status)) {

        dosUnicodeString.Buffer = NULL;

    } else {

        IoAssignArcName(&dosUnicodeString, &ntUnicodeString);

    }

    RtlFreeUnicodeString(&ntUnicodeString);

    if (dosUnicodeString.Buffer != NULL) {

        RtlFreeUnicodeString(&dosUnicodeString);

    }


    //
    // Increment system tape count.
    //
    ++IoGetConfigurationInformation()->TapeCount;

    //
    // Get device extension address.
    //

    context = q117DeviceObject->DeviceExtension;
    context->q117iDeviceObject = q117iDeviceObject;

    //
    // Allocate memory for the filer
    //

    status = q117AllocatePermanentMemory(
        context,
        AdapterObject,
        NumberOfMapRegisters
        );

    if (status) {
        IoDeleteDevice(q117DeviceObject);
        return status;
    }

    //
    // Indicate MDLs required.
    //

    q117DeviceObject->Flags = DO_DIRECT_IO;

    status = q117CreateRegistryInfo(tapeNumber, RegistryPath, context);

    q117RdsInitReed();

    return status;

} // end Q117Initialize()

NTSTATUS
q117CreateRegistryInfo(
    IN ULONG TapeNumber,
    IN PUNICODE_STRING RegistryPath,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    This function adds Tape\Unit x to the devicemap and puts the Id info
    and type info values in it.

Arguments:

    tapeNumber - unit number of the tape (current tape count)

    Context - current context of the driver (this is used to
                identify the device).


Return Value:

   Returns the status of the operation.

--*/

{
    HANDLE          lunKey;
    HANDLE          unitKey;
    UNICODE_STRING  ntUnicodeString;
    UCHAR           ntNameBuffer[80];
    STRING          ntNameString;
    UNICODE_STRING  name;
    OBJECT_ATTRIBUTES objectAttributes;
    ULONG           disposition;
    NTSTATUS        status;

    //
    // Create the Tape key in the device map.
    //

    RtlInitUnicodeString(
        &name,
        L"\\Registry\\Machine\\Hardware\\DeviceMap\\Tape"
        );

    //
    // Initialize the object for the key.
    //

    InitializeObjectAttributes( &objectAttributes,
                                &name,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                (PSECURITY_DESCRIPTOR) NULL );

    //
    // Create the key or open it.
    //

    status = ZwCreateKey(&lunKey,
                        KEY_READ | KEY_WRITE,
                        &objectAttributes,
                        0,
                        (PUNICODE_STRING) NULL,
                        REG_OPTION_VOLATILE,
                        &disposition );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = q117CreateNumericKey(lunKey, TapeNumber, L"Unit ", &unitKey);

    ZwClose(lunKey);

    if (!NT_SUCCESS(status)) {
        return status;
    }


    //
    // Add Identifier value.
    //

    RtlInitUnicodeString(&name, L"Identifier");

    status = ZwSetValueKey(
        unitKey,
        &name,
        0,
        REG_SZ,
        L"QIC-40/QIC-80 floppy tape drive",
        sizeof(L"QIC-40/QIC-80 floppy tape drive")
        );

    if ( NT_SUCCESS(status) ) {
        //
        // Add driver value.
        //

        RtlInitUnicodeString(&name, L"Driver");

        status = ZwSetValueKey(
            unitKey,
            &name,
            0,
            REG_SZ,
            RegistryPath->Buffer,
            RegistryPath->Length
            );
    }

    if ( NT_SUCCESS(status) ) {
        //
        // Add DeviceName value.
        //

        RtlInitUnicodeString(&name, L"DeviceName");

        sprintf(ntNameBuffer,
                "Tape%d",
                TapeNumber);

        RtlInitString(&ntNameString, ntNameBuffer);

        status = RtlAnsiStringToUnicodeString(&ntUnicodeString,
                                              &ntNameString,
                                              TRUE);

        status = ZwSetValueKey(
            unitKey,
            &name,
            0,
            REG_SZ,
            ntUnicodeString.Buffer,
            ntUnicodeString.Length
            );

        RtlFreeUnicodeString(&ntUnicodeString);
    }

    if ( NT_SUCCESS(status) ) {
        //
        // Add UniqueID
        //

        RtlInitUnicodeString(&name, L"UniqueId");

        status = ZwSetValueKey(
            unitKey,
            &name,
            0,
            REG_SZ,
            L"",
            0
            );
    }


    ZwClose(unitKey);

    return status;

} // end q117CreateRegistryInfo

NTSTATUS
q117CreateNumericKey(
    IN HANDLE Root,
    IN ULONG Name,
    IN PWSTR Prefix,
    OUT PHANDLE NewKey
    )

/*++

Routine Description:

    This function creates a registry key.  The name of the key is a string
    version of numeric value passed in.

Arguments:

    RootKey - Supplies a handle to the key where the new key should be inserted.

    Name - Supplies the numeric value to name the key.

    Prefix - Supplies a prefix name to add to name.

    NewKey - Returns the handle for the new key.

Return Value:

   Returns the status of the operation.

--*/

{

    UNICODE_STRING string;
    UNICODE_STRING stringNum;
    OBJECT_ATTRIBUTES objectAttributes;
    WCHAR bufferNum[16];
    WCHAR buffer[64];
    ULONG disposition;
    NTSTATUS status;

    //
    // Copy the Prefix into a string.
    //

    string.Length = 0;
    string.MaximumLength=64;
    string.Buffer = buffer;

    RtlInitUnicodeString(&stringNum, Prefix);

    RtlCopyUnicodeString(&string, &stringNum);

    //
    // Create a port number key entry.
    //

    stringNum.Length = 0;
    stringNum.MaximumLength = 16;
    stringNum.Buffer = bufferNum;

    status = RtlIntegerToUnicodeString(Name, 10, &stringNum);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Append the prefix and the numeric name.
    //

    RtlAppendUnicodeStringToString(&string, &stringNum);

    InitializeObjectAttributes( &objectAttributes,
                                &string,
                                OBJ_CASE_INSENSITIVE,
                                Root,
                                (PSECURITY_DESCRIPTOR) NULL );

    status = ZwCreateKey(NewKey,
                        KEY_READ | KEY_WRITE,
                        &objectAttributes,
                        0,
                        (PUNICODE_STRING) NULL,
                        REG_OPTION_VOLATILE,
                        &disposition );

    return(status);

} // end q117CreateNumericKey

NTSTATUS
q117Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This is the tape class driver IO handler routine.

Arguments:

    DeviceObject
    Irp - IO request

Return Value:

    NT Status

--*/

{

    PIO_STACK_LOCATION  currentIrpStack;
    KIRQL               currentIrql;
    STATUS              status;
    NTSTATUS            ntStatus;
    PVOID               usrBuf;
    PQ117_CONTEXT       context;
    ULONG               amount;
    BOOLEAN             endOfVolume = FALSE;


    context = DeviceObject->DeviceExtension;
    currentIrpStack = IoGetCurrentIrpStackLocation(Irp);

    status = NoErr;
    ntStatus = STATUS_SUCCESS;

    //
    // If this is the first read,  find the NT volume and read
    if (context->CurrentOperation.Type == NoOperation) {

        ntStatus = q117OpenForRead(
                    context->CurrentOperation.BytesRead,
                    context,
                    DeviceObject);

        if (ntStatus == STATUS_NO_DATA_DETECTED) {

            //
            // Flag no bytes read (end of media,  end of data,  etc.)
            //

            ntStatus = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;
            endOfVolume = TRUE;

        }

    }


    if (NT_SUCCESS(ntStatus)) {

        //
        // Return the results of the call to the port driver.
        //
        usrBuf = MmGetSystemAddressForMdl(Irp->MdlAddress);

        //
        // Check to see if user is asking for more data than is there
        //  (bytes on tape - current offset)
        //

        amount = currentIrpStack->Parameters.Read.Length;

        ntStatus = q117ConvertStatus(
            DeviceObject,
            q117ReadTape(usrBuf,&amount,context)
            );

        //
        // Set the amount read to the amount we copied out of the buffer
        //
        Irp->IoStatus.Information = amount;

        CheckedDump(QIC117SHOWTD,("%x=Read(%x) - Status: %x\n",amount,currentIrpStack->Parameters.Read.Length, status));

    }

    if (endOfVolume && NT_SUCCESS(ntStatus)) {

        status = STATUS_NO_DATA_DETECTED;

    }

    Irp->IoStatus.Status = ntStatus;

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);
    IoCompleteRequest(Irp, 0);
    KeLowerIrql(currentIrql);

    return ntStatus;


} // end Q117Read()


NTSTATUS
q117Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This is the tape class driver IO handler routine.

Arguments:

    DeviceObject
    Irp - IO request

Return Value:

    NT Status

--*/

{

    PIO_STACK_LOCATION  currentIrpStack;
    KIRQL               currentIrql;
    STATUS              status;
    NTSTATUS            ntStatus;
    PVOID               usrBuf;
    PQ117_CONTEXT       context;

    context = DeviceObject->DeviceExtension;
    currentIrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Make sure we are in write mode
    //
    status = q117OpenForWrite(context);

    if (!status) {

        //
        // Return the results of the call to the port driver.
        //
        usrBuf = MmGetSystemAddressForMdl(Irp->MdlAddress);

        status = q117WriteTape(usrBuf,currentIrpStack->Parameters.Write.Length,context);
        //
        // Set the amount written to the amount we copied out of the buffer
        //
        if (!status || status == EarlyWarning)
            Irp->IoStatus.Information = currentIrpStack->Parameters.Write.Length;
    }

    ntStatus = q117ConvertStatus(DeviceObject, status);

    Irp->IoStatus.Status = ntStatus;

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);
    IoCompleteRequest(Irp, 0);
    KeLowerIrql(currentIrql);

    return ntStatus;

} // end Q117Write()

NTSTATUS
q117ConvertStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN STATUS Status
    )
{
    NTSTATUS            ntStatus;
    PQ117_CONTEXT       context;
    BOOLEAN             suppressLog = FALSE;

    context = DeviceObject->DeviceExtension;

    switch(Status) {
    case NoVols:
    case InvalVol:
    case EndOfVol:
        // or maybe STATUS_END_OF_RECORDED_DATA?
        ntStatus = STATUS_NO_DATA_DETECTED;
        suppressLog = TRUE;
        break;

    case EarlyWarning:
        ntStatus = STATUS_END_OF_MEDIA;
        suppressLog = TRUE;
        break;

    case NoTape:
        ntStatus = STATUS_NO_MEDIA_IN_DEVICE;
        break;

    case NoErr:
        ntStatus = STATUS_SUCCESS;
        break;

#ifndef NO_MARKS
    case FileMark:
    case LongFileMark:
    case ShortFileMark:
        ntStatus = STATUS_FILEMARK_DETECTED;
        suppressLog = TRUE;
        break;

    case SetMark:
        ntStatus = STATUS_SETMARK_DETECTED;
        suppressLog = TRUE;
        break;
#endif

    case UnknownFormat:
    case Unformat:
    case BadFmt:
        ntStatus = STATUS_UNRECOGNIZED_MEDIA;
        break;

    case FMemErr:
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        break;

    case WProt:
        ntStatus = STATUS_MEDIA_WRITE_PROTECTED;
        break;

    case NewCart:
        ntStatus = STATUS_MEDIA_CHANGED;
        break;

    case UpdErr:
    case BadTape:
        ntStatus = STATUS_DATA_ERROR;
        break;

    case RdncUnsc:
        ntStatus = STATUS_CRC_ERROR;
        break;

    default:
        CheckedDump(QIC117DBGP,("Error %d reported\n",Status));
        ntStatus = (NTSTATUS)(STATUS_SEVERITY_WARNING << 30);
        ntStatus |= (FILE_DEVICE_TAPE << 16) & 0x3fff0000;
        ntStatus |= Status & 0x0000ffff;

    }

    if (Status != NoErr && !suppressLog) {
        NTSTATUS logStatus = q117MapStatus(Status);

        if (logStatus != QIC117_NOTAPE) {
            CheckedDump(QIC117SHOWTD,("Error %d logged\n",Status));
    
            q117LogError(
                DeviceObject,
                context->ErrorSequence++,
                context->MajorFunction,
                0,
                Status,
                ntStatus,
                logStatus
                );
        }

    } else {
#if DBG
        if (Status) {
            CheckedDump(QIC117SHOWTD,("Error %d reported to API\n",Status));
        }
#endif
    }

    return ntStatus;
}

NTSTATUS q117MapStatus(
    IN STATUS Status
    )
{
    NTSTATUS            mapped;

    switch(Status) {
        case UnusTape:
            mapped = QIC117_UNUSTAPE;
            break;
        case BadTape:
            mapped = QIC117_BADTAPE;
            break;
        case FMemErr:
            mapped = QIC117_FMEMERR;
            break;
        case TapeFull:
            mapped = QIC117_TAPEFULL;
            break;
        case VolFull:
            mapped = QIC117_VOLFULL;
            break;
        case RdncUnsc:
            mapped = QIC117_RDNCUNSC;
            break;
        case EndOfVol:
            mapped = QIC117_ENDOFVOL;
            break;
        case FCodeErr:
            mapped = QIC117_FCODEERR;
            break;
        case UpdErr:
            mapped = QIC117_UPDERR;
            break;
        case InvalVol:
            mapped = QIC117_INVALVOL;
            break;
        case NoVols:
            mapped = QIC117_NOVOLS;
            break;
        case Unformat:
            mapped = QIC117_UNFORMAT;
            break;
        case UnknownFormat:
            mapped = QIC117_UNKNOWNFORMAT;
            break;
        case BadBlk:
            mapped = QIC117_BADBLK;
            break;
        case EndTapeErr:
            mapped = QIC117_ENDTAPEERR;
            break;
        case DriveFlt:
            mapped = QIC117_DRIVEFLT;
            break;
        case WProt:
            mapped = QIC117_WPROT;
            break;
        case NoTape:
            mapped = QIC117_NOTAPE;
            break;
        case SeekErr:
            mapped = QIC117_SEEKERR;
            break;
        case NoDrive:
            mapped = QIC117_NODRIVE;
            break;
        case InvalCmd:
            mapped = QIC117_INVALCMD;
            break;
        case CodeErr:
            mapped = QIC117_CODEERR;
            break;
        case NECFlt:
            mapped = QIC117_NECFLT;
            break;
        case NoFDC:
            mapped = QIC117_NOFDC;
            break;
        case BadFmt:
            mapped = QIC117_BADFMT;
            break;
        case CmdFlt:
            mapped = QIC117_CMDFLT;
            break;
        case BadNEC:
            mapped = QIC117_BADNEC;
            break;
        case BadReq:
            mapped = QIC117_BADREQ;
            break;
        case TooFast:
            mapped = QIC117_TOOFAST;
            break;
        case NoData:
            mapped = QIC117_NODATA;
            break;
        case DAbort:
            mapped = QIC117_DABORT;
            break;
        case TapeFlt:
            mapped = QIC117_TAPEFLT;
            break;
        case UnspRate:
            mapped = QIC117_UNSPRATE;
            break;
        case Already:
            mapped = QIC117_ALREADY;
            break;
        case TooNoisy:
            mapped = QIC117_TOONOISY;
            break;
        case TimeOut:
            mapped = QIC117_TIMEOUT;
            break;
        case BadMark:
            mapped = QIC117_BADMARK;
            break;
        case NewCart:
            mapped = QIC117_NEWCART;
            break;
        case WrongFmt:
            mapped = QIC117_WRONGFMT;
            break;
        case FmtMisMatch:
            mapped = QIC117_FMTMISMATCH;
            break;
        case IncompTapeFmt:
            mapped = QIC117_INCOMPTAPEFMT;
            break;
        case SCSIFmtMsmtch:
            mapped = QIC117_SCSIFMTMSMTCH;
            break;
        case QIC40InEagle:
            mapped = QIC117_QIC40INEAGLE;
            break;
        case QIC80InEagle:
            mapped = QIC117_QIC80INEAGLE;
            break;
        case ControllerBusy:
            mapped = QIC117_CONTROLLERBUSY;
            break;
        case InQue:
            mapped = QIC117_INQUE;
            break;
        case SplitRequests:
            mapped = QIC117_SPLITREQUESTS;
            break;
        case EarlyWarning:
            mapped = QIC117_EARLYWARNING;
            break;
        default:
            mapped = QIC117_BOGUS;
    }
    return mapped;
}

NTSTATUS
q117DeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatcher for device control requests. It
    looks at the IOCTL code and calls the appropriate tape device
    routine.

Arguments:

    DeviceObject
    Irp - Request packet

Return Value:

--*/

{
    PIO_STACK_LOCATION  irpStack;
    KIRQL               currentIrql;
    NTSTATUS            status;
    PQ117_CONTEXT       context;

    context = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    status = 0;

    CheckedDump(QIC117SHOWPOLL,("Curmark: %x TotalMarks: %x\n", context->CurrentMark, context->MarkArray.TotalMarks));

    switch (irpStack->Parameters.DeviceIoControl.IoControlCode) {

    case IOCTL_TAPE_GET_DRIVE_PARAMS:

        status = q117IoCtlGetDriveParameters(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_GET_DRIVE_PARAMS done"));
        break;

    case IOCTL_TAPE_SET_DRIVE_PARAMS:

        status = q117IoCtlSetDriveParameters(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_SET_DRIVE_PARAMS done"));
        break;

    case IOCTL_TAPE_GET_MEDIA_PARAMS:

        status = q117IoCtlGetMediaParameters(DeviceObject, Irp);
        CheckedDump(QIC117SHOWPOLL,("IOCTL_TAPE_GET_MEDIA_PARAMS done"));
        break;

    case IOCTL_TAPE_SET_MEDIA_PARAMS:

        status = q117IoCtlSetMediaParameters(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_SET_MEDIA_PARAMS done\n"));
        break;

    case IOCTL_TAPE_CREATE_PARTITION:

        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_CREATE_PARTITION attempted\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;

    case IOCTL_TAPE_ERASE:

        status = q117IoCtlErase(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("Curmark: %x TotalMarks: %x\n", context->CurrentMark, context->MarkArray.TotalMarks));
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_ERASE done"));
        break;

    case IOCTL_TAPE_PREPARE:

        status = q117IoCtlPrepare(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_PREPARE done"));
        break;

    case IOCTL_TAPE_WRITE_MARKS:

        status = q117IoCtlWriteMarks(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("Curmark: %x TotalMarks: %x\n", context->CurrentMark, context->MarkArray.TotalMarks));
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_WRITE_MARKS done"));
        break;

    case IOCTL_TAPE_GET_POSITION:

        status = q117IoCtlGetPosition(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_GET_POSITION done"));
        break;

    case IOCTL_TAPE_SET_POSITION:

        status = q117IoCtlSetPosition(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("Curmark: %x TotalMarks: %x\n", context->CurrentMark, context->MarkArray.TotalMarks));
        CheckedDump(QIC117SHOWTD,("IOCTL_TAPE_SET_POSITION done"));
        break;

    case IOCTL_TAPE_GET_STATUS:

        status = q117IoCtlGetStatus (DeviceObject, Irp);
        CheckedDump(QIC117SHOWPOLL,("IOCTL_TAPE_GET_STATUS done"));
        break;

    case IOCTL_CMS_WRITE_ABS_BLOCK:

        status = q117IoCtlWriteAbs(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_CMS_WRITE_ABS_BLOCK done"));
        break;

    case IOCTL_CMS_READ_ABS_BLOCK:

        status = q117IoCtlReadAbs(DeviceObject, Irp);
        CheckedDump(QIC117SHOWTD,("IOCTL_CMS_READ_ABS_BLOCK done"));
        break;


    default:

        status = STATUS_INVALID_DEVICE_REQUEST;

    } // end switch()

    //
    // Complete the request.
    //

    CheckedDump(QIC117SHOWTD,(" -- Status:  %x\n",status));
    Irp->IoStatus.Status = status;

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);
    IoCompleteRequest(Irp, 2);
    KeLowerIrql(currentIrql);

    return status;

} // end Q117DeviceControl()


NTSTATUS
q117Create (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine handles CREATE/OPEN requests and does
    nothing more than return successful status.

Arguments:

    DeviceObject
    Irp

Return Value:

    NT Status

--*/

{
    KIRQL currentIrql;
    PQ117_CONTEXT     context;
    NTSTATUS ntStatus;

    context = DeviceObject->DeviceExtension;

    if (!context->DriverOpened) {

        ntStatus = q117ConvertStatus(DeviceObject, q117Start(context));

        //
        // If everything went OK,  then flag that the driver is now open
        //
        if (NT_SUCCESS(ntStatus)) {
            context->DriverOpened = TRUE;
        } else {
            q117Stop(context);
        }

    } else {

        ntStatus = STATUS_DEVICE_BUSY;

    }

    Irp->IoStatus.Status = ntStatus;

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);
    IoCompleteRequest(Irp, 0);
    KeLowerIrql(currentIrql);

    return ntStatus;

} // end Q117Create()

NTSTATUS
q117Close (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:


Arguments:

    DeviceObject
    Irp

Return Value:

    NT Status

--*/

{
    KIRQL currentIrql;
    PQ117_CONTEXT     context;
    NTSTATUS ntStatus;

    context = DeviceObject->DeviceExtension;

    ntStatus = q117ConvertStatus(DeviceObject, q117Stop(context));

    context->DriverOpened = FALSE;

    Irp->IoStatus.Status = ntStatus;

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);
    IoCompleteRequest(Irp, 0);
    KeLowerIrql(currentIrql);

    return ntStatus;

} // end Q117Close()

VOID
q117LogError(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SequenceNumber,
    IN UCHAR MajorFunctionCode,
    IN UCHAR RetryCount,
    IN ULONG UniqueErrorValue,
    IN NTSTATUS FinalStatus,
    IN NTSTATUS SpecificIOStatus
    )

/*++

Routine Description:

    This routine allocates an error log entry, copies the supplied data
    to it, and requests that it be written to the error log file.

Arguments:

    DeviceObject - a pointer to the device object associated with the
    device that had the error.

    SequenceNumber - A ulong value that is unique to an IRP over the
    life of the irp in this driver - 0 generally means an error not
    associated with an irp.

    MajorFunctionCode - If there is an error associated with the irp,
    this is the major function code of that irp.

    RetryCount - The number of times a particular operation has been
    retried.

    UniqueErrorValue - A unique long word that identifies the particular
    call to this function.

    FinalStatus - The final status given to the irp that was associated
    with this error.  If this log entry is being made during one of
    the retries this value will be STATUS_SUCCESS.

    SpecificIOStatus - The IO status for a particular error.

Return Value:

    None.

--*/

{
    PIO_ERROR_LOG_PACKET errorLogEntry;

    errorLogEntry = IoAllocateErrorLogEntry(
                        DeviceObject,
                        sizeof(IO_ERROR_LOG_PACKET)
                        );

    if ( errorLogEntry != NULL ) {
        errorLogEntry->ErrorCode = SpecificIOStatus;
        errorLogEntry->SequenceNumber = SequenceNumber;
        errorLogEntry->MajorFunctionCode = MajorFunctionCode;
        errorLogEntry->RetryCount = RetryCount;
        errorLogEntry->UniqueErrorValue = UniqueErrorValue;
        errorLogEntry->FinalStatus = FinalStatus;
        errorLogEntry->DumpDataSize = 0;
        IoWriteErrorLogEntry(errorLogEntry);

    }

}
#ifdef NOT_NOW

VOID
q117Cancel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is called from the I/O system when a request is cancelled.

    N.B.  The cancel spinlock is already held upon entry to this routine.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet to be cancelled.

Return Value:

    None.

--*/

{

    CheckedDump(QIC117INFO,("q117Cancel: enter\n"));

    if (Irp == DeviceObject->CurrentIrp) {

        //
        // The current request is being cancelled.  Set the CurrentIrp to
        // null and release the cancel spinlock before starting the next packet.
        //

        DeviceObject->CurrentIrp = NULL;
        IoReleaseCancelSpinLock(Irp->CancelIrql);
        IoStartNextPacket(DeviceObject, TRUE);

    } else {

        //
        // Cancel a request in the device queue.  Remove it from queue and
        // release the cancel spinlock.
        //

        if (TRUE != KeRemoveEntryDeviceQueue(
                        &DeviceObject->DeviceQueue,
                        &Irp->Tail.Overlay.DeviceQueueEntry
                        )) {
            CheckedDump(QIC117SHOWTD,(
                "q117Cancel: Irp 0x%x not in device queue?!?\n",
                Irp
                ));
        }
        IoReleaseCancelSpinLock(Irp->CancelIrql);
    }

    //
    // Complete the request with STATUS_CANCELLED.
    //

    Irp->IoStatus.Status = STATUS_CANCELLED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    CheckedDump(QIC117INFO,("q117Cancel: exit\n"));

    return;
}
#endif

NTSTATUS
q117Cleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch routine for cleanup requests.
    All queued q117 requests are completed with STATUS_CANCELLED,
    and the lower level driver's queue is cleared.

Arguments:

    DeviceObject - Pointer to class device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{
    KIRQL currentIrql;
    KIRQL cancelIrql;
    PKDEVICE_QUEUE_ENTRY packet;
    PIRP  currentIrp;

    CheckedDump(QIC117INFO,("q117Cleanup: enter\n"));

    /* clear everything from the low-level driver */
    q117ClearQueue(
        DeviceObject->DeviceExtension
        );
    //
    // Raise IRQL to DISPATCH_LEVEL.
    //

    KeRaiseIrql(DISPATCH_LEVEL, &currentIrql);

    //
    // Acquire the cancel spinlock.
    //

    IoAcquireCancelSpinLock(&cancelIrql);

    //
    // Complete all queued requests with STATUS_CANCELLED.
    // Start with the real CurrentIrp, and run down the list of requests in the
    // device queue.  Be sure to set the real CurrentIrp to NULL, so that
    // the interrupt service callback routine won't attempt to complete it.
    //

    currentIrp = DeviceObject->CurrentIrp;
    DeviceObject->CurrentIrp = NULL;

    while (currentIrp != NULL) {

        //
        // Remove the CurrentIrp from the cancellable state.
        //
        //

        IoSetCancelRoutine(currentIrp, NULL);

        //
        // Set Status to CANCELLED, release the cancel spinlock,
        // and complete the request.  Note that the IRQL is reset to
        // DISPATCH_LEVEL when we release the cancel spinlock.
        //

        currentIrp->IoStatus.Status = STATUS_CANCELLED;
        currentIrp->IoStatus.Information = 0;

        IoReleaseCancelSpinLock(cancelIrql);
        IoCompleteRequest(currentIrp, IO_NO_INCREMENT);

        //
        // Reacquire the cancel spinlock.
        //

        IoAcquireCancelSpinLock(&cancelIrql);

        //
        // Dequeue the next packet (IRP) from the device work queue.
        //

        packet = KeRemoveDeviceQueue(&DeviceObject->DeviceQueue);
        if (packet != NULL) {
            currentIrp =
                CONTAINING_RECORD(packet, IRP, Tail.Overlay.DeviceQueueEntry);
        } else {
            currentIrp = (PIRP) NULL;
        }
    } // end while

    //
    // Release the cancel spinlock and lower IRQL.
    //

    IoReleaseCancelSpinLock(cancelIrql);
    KeLowerIrql(currentIrql);

    //
    // Complete the cleanup request with STATUS_SUCCESS.
    //

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest (Irp, IO_NO_INCREMENT);

    CheckedDump(QIC117INFO,("q117Cleanup: exit\n"));

    return(STATUS_SUCCESS);

}

