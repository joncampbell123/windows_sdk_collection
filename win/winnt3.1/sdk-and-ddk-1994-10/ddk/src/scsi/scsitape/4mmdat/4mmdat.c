/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    4mmdat.c

Abstract:

    This module contains the device-specific routines for the Archive
    Python DDS, DDS-DC, and Turbo-Speed DDS-DC DAT tape drives, the
    HP 35470A DDS and 35480A DDS-DC DAT tape drives, the WangDAT
    Model 3200 DDS-DC DAT tape drive, and the Compaq DDS-DC DAT tape
    drive.

Author:

    Mike Glass
    Hunter Small (Maynard)
    Lori Brown (Maynard)
    Chris Hugh Sam (Maynard)

Environment:

    kernel mode only

Revision History:

--*/

#include "ntddk.h"
#include "tape.h"
#include "4mmdat.h"

static ULONG gb_PartitionCount;
static UCHAR gb_VendorId[8];
static UCHAR gb_ProductId[16];

//
//  Internal (module wide) defines that symbolize
//  the 4mm DAT drives supported by this module.
//
#define ARCHIVE_PYTHON   1 ;
#define DEC_TLZ06        2 ;
#define EXABYTE_4200     3 ;
#define EXABYTE_4200C    4 ;
#define HP_35470A        5 ;
#define HP_35480A        6 ;
#define HP_IBM35480A     7 ;
#define WANGDAT_1300     8 ;
#define WANGDAT_3100     9 ;
#define WANGDAT_3200    10 ;

//
//  Function prototype(s) for internal function(s)
//
static  ULONG  WhichIsIt(IN PINQUIRYDATA InquiryData);


NTSTATUS
TapeCreatePartition(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine creates initiator-defined partitions on the tape.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION      deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_CREATE_PARTITION tapePartition = Irp->AssociatedIrp.SystemBuffer;
    PMODE_MEDIUM_PART_PAGE buffer;
    SCSI_REQUEST_BLOCK     srb;
    PCDB                   cdb = (PCDB)srb.Cdb;
    NTSTATUS               status;

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SELECT.OperationCode = SCSIOP_MODE_SELECT;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = 8000;

    cdb->MODE_SELECT.ParameterListLength = sizeof(MODE_MEDIUM_PART_PAGE);
    cdb->MODE_SELECT.PFBit = 1;

    buffer = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_MEDIUM_PART_PAGE));

    if (!buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, sizeof(MODE_MEDIUM_PART_PAGE));

    buffer->ParameterListHeader.DeviceSpecificParameter = 0x10;

    buffer->MediumPartPage.PageCode = MODE_PAGE_MEDIUM_PARTITION;

    if (tapePartition->Count != 1 && tapePartition->Count != 2) {
        DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
        ExFreePool(buffer);
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    buffer->MediumPartPage.AdditionalPartitionDefined = tapePartition->Count - 1;

    switch (tapePartition->Method) {
        case TAPE_FIXED_PARTITIONS:
            if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
                DebugPrint((3,"TapeIoControl: Create Fixed Tape Partitions\n"));
                buffer->MediumPartPage.FDPBit = SETBITON;
                buffer->MediumPartPage.PageLength = 0x0A;
            } else {
                DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
                ExFreePool(buffer);
                return STATUS_INVALID_DEVICE_REQUEST;
            }
            break;

        case TAPE_SELECT_PARTITIONS:
            if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
                DebugPrint((3,"TapeIoControl: Create Select Tape Partitions\n"));
                buffer->MediumPartPage.SDPBit = SETBITON;
                buffer->MediumPartPage.PageLength = 0x0A;
            } else {
                DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
                ExFreePool(buffer);
                return STATUS_INVALID_DEVICE_REQUEST;
            }
            break;

        case TAPE_INITIATOR_PARTITIONS:
            DebugPrint((3,"TapeIoControl: Create Initiator Defined Tape Partitions\n"));
            buffer->MediumPartPage.IDPBit = SETBITON;

            if (buffer->MediumPartPage.AdditionalPartitionDefined) {
                if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
                    buffer->MediumPartPage.PageLength = 0x0A;

                    buffer->MediumPartPage.Partition1Size[0] =
                        ((tapePartition->Size >> 8) & 0xFF);
                    buffer->MediumPartPage.Partition1Size[1] =
                        (tapePartition->Size & 0xFF);

                    buffer->MediumPartPage.PSUMBit = 2;
                } else {
                    buffer->MediumPartPage.PageLength = 0x08;
                    cdb->MODE_SELECT.ParameterListLength -= 2;

                    buffer->MediumPartPage.Partition0Size[0] =
                        ((tapePartition->Size >> 8) & 0xFF);
                    buffer->MediumPartPage.Partition0Size[1] =
                        (tapePartition->Size & 0xFF);

                    buffer->MediumPartPage.PSUMBit = 2;
                }
            } else {
                if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
                    buffer->MediumPartPage.PageLength = 0x0A;
                } else {
                    buffer->MediumPartPage.PageLength = 0x06;
                    cdb->MODE_SELECT.ParameterListLength -= 4;
                }
            }

            break;

        default:
            DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
            ExFreePool(buffer);
            return STATUS_INVALID_DEVICE_REQUEST;
    }

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                buffer,
                                cdb->MODE_SELECT.ParameterListLength,
                                TRUE);

    ExFreePool(buffer);

    return status;

} // end TapeCreatePartition()


NTSTATUS
TapeErase(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine erases the current partition of the device by writing an
    end-of-recorded data marker beginning at the current position.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_ERASE         tapeErase = Irp->AssociatedIrp.SystemBuffer;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->ERASE.OperationCode = SCSIOP_ERASE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    if (tapeErase->Type != TAPE_ERASE_SHORT) {
        DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    DebugPrint((3,"TapeIoControl: Short Erase Tape\n"));

    //
    // Set immediate bit if indicated.
    //

    cdb->ERASE.Immediate = tapeErase->Immediate;

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                NULL,
                                0,
                                FALSE);

    return status;

} // end TapeErase()



VOID
TapeError(
    PDEVICE_OBJECT DeviceObject,
    PSCSI_REQUEST_BLOCK Srb,
    NTSTATUS *Status,
    BOOLEAN *Retry
    )

/*++

Routine Description:

    When a request completes with error, the routine InterpretSenseInfo is
    called to determine from the sense data whether the request should be
    retried and what NT status to set in the IRP. Then this routine is called
    for tape requests to handle tape-specific errors and update the nt status
    and retry boolean.

Arguments:

    DeviceObject - Supplies a pointer to the device object.

    Srb - Supplies a pointer to the failing Srb.

    Status - NT Status used to set the IRP's completion status.

    Retry - Indicates that this request should be retried.

Return Value:

    None.

--*/

{
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    PSENSE_DATA        senseBuffer = Srb->SenseInfoBuffer;
    NTSTATUS           status = *Status;
    BOOLEAN            retry = *Retry;
    UCHAR              adsenseq = senseBuffer->AdditionalSenseCodeQualifier;
    UCHAR              adsense = senseBuffer->AdditionalSenseCode;

    if (status == STATUS_DEVICE_NOT_READY) {
        if ((adsense == SCSI_ADSENSE_LUN_NOT_READY) &&
            (adsenseq == SCSI_SENSEQ_BECOMING_READY) ) {

            *Status = STATUS_NO_MEDIA;
            *Retry = FALSE;

        }
    }
    return;

} // end TapeError()


NTSTATUS
TapeGetDriveParameters(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine returns an indication if reporting setmarks is disabled or
    enabled, the default fixed-block size, the maximum block size, the
    minimum block size, the maximum number of partitions, and the device
    features flag.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION          deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_GET_DRIVE_PARAMETERS tapeGetDriveParams = Irp->AssociatedIrp.SystemBuffer;
    PMODE_DEVICE_CONFIG_PAGE   buffer;
    PREAD_BLOCK_LIMITS_DATA    blockLimits;
    SCSI_REQUEST_BLOCK         srb;
    PCDB                       cdb = (PCDB)srb.Cdb;
    NTSTATUS                   status;
    PMODE_DATA_COMPRESS_PAGE   dcbuff;

    DebugPrint((3,"TapeIoControl: Get Tape Drive Parameters\n"));

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SENSE.AllocationLength = sizeof(MODE_DEVICE_CONFIG_PAGE);
    cdb->MODE_SENSE.Dbd = SETBITON;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_DEVICE_CONFIG;

    buffer = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_DEVICE_CONFIG_PAGE));

    if (!buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, sizeof(MODE_DEVICE_CONFIG_PAGE));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                buffer,
                                sizeof( MODE_DEVICE_CONFIG_PAGE ),
                                FALSE);

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = sizeof(TAPE_GET_DRIVE_PARAMETERS);

        tapeGetDriveParams->ECC = 0;
        tapeGetDriveParams->DataPadding = 0;
        tapeGetDriveParams->Compression = 0;
        tapeGetDriveParams->ReportSetmarks = (buffer->DeviceConfigPage.RSmk ? 1 : 0 );
        tapeGetDriveParams->DefaultBlockSize = 0x200;
        tapeGetDriveParams->MaximumPartitionCount = 2;

        tapeGetDriveParams->FeaturesLow =
            TAPE_DRIVE_INITIATOR |
            TAPE_DRIVE_ERASE_SHORT |
            TAPE_DRIVE_ERASE_BOP_ONLY |
            TAPE_DRIVE_ERASE_IMMEDIATE |
            TAPE_DRIVE_TAPE_CAPACITY |
            TAPE_DRIVE_FIXED_BLOCK |
            TAPE_DRIVE_VARIABLE_BLOCK |
            TAPE_DRIVE_WRITE_PROTECT |
            TAPE_DRIVE_REPORT_SMKS |
            TAPE_DRIVE_GET_ABSOLUTE_BLK |
            TAPE_DRIVE_GET_LOGICAL_BLK;

        tapeGetDriveParams->FeaturesHigh =
            TAPE_DRIVE_LOAD_UNLOAD |
            TAPE_DRIVE_LOCK_UNLOCK |
            TAPE_DRIVE_REWIND_IMMEDIATE |
            TAPE_DRIVE_SET_BLOCK_SIZE |
            TAPE_DRIVE_LOAD_UNLD_IMMED |
            TAPE_DRIVE_SET_REPORT_SMKS |
            TAPE_DRIVE_ABSOLUTE_BLK |
            TAPE_DRIVE_ABS_BLK_IMMED |
            TAPE_DRIVE_LOGICAL_BLK |
            TAPE_DRIVE_END_OF_DATA |
            TAPE_DRIVE_RELATIVE_BLKS |
            TAPE_DRIVE_FILEMARKS |
            TAPE_DRIVE_SEQUENTIAL_FMKS |
            TAPE_DRIVE_SETMARKS |
            TAPE_DRIVE_REVERSE_POSITION |
            TAPE_DRIVE_WRITE_SETMARKS |
            TAPE_DRIVE_WRITE_FILEMARKS |
            TAPE_DRIVE_WRITE_MARK_IMMED;

        if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
            tapeGetDriveParams->FeaturesLow |=
                TAPE_DRIVE_FIXED |
                TAPE_DRIVE_SELECT;
            tapeGetDriveParams->FeaturesHigh |=
                TAPE_DRIVE_LOG_BLK_IMMED |
                TAPE_DRIVE_SEQUENTIAL_SMKS;
        } else if (RtlCompareMemory(gb_VendorId,"HP      ",8) == 8) {
            tapeGetDriveParams->FeaturesHigh |=
                TAPE_DRIVE_LOG_BLK_IMMED |
                TAPE_DRIVE_SEQUENTIAL_SMKS;
        } else {
            tapeGetDriveParams->FeaturesLow |=
                TAPE_DRIVE_TAPE_REMAINING;
        }

    }

    ExFreePool(buffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SENSE.AllocationLength = sizeof(MODE_DATA_COMPRESS_PAGE);
    cdb->MODE_SENSE.Dbd = SETBITON;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_DATA_COMPRESS;

    dcbuff = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_DATA_COMPRESS_PAGE));

    if (!dcbuff) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(dcbuff, sizeof(MODE_DATA_COMPRESS_PAGE));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                dcbuff,
                                sizeof( MODE_DATA_COMPRESS_PAGE ),
                                FALSE);

    if (NT_SUCCESS(status)) {
        if (dcbuff->DataCompressPage.DCC == 1) {
            tapeGetDriveParams->FeaturesLow |= TAPE_DRIVE_COMPRESSION;
            tapeGetDriveParams->FeaturesHigh |= TAPE_DRIVE_SET_COMPRESSION;
            tapeGetDriveParams->Compression = (dcbuff->DataCompressPage.DCE == 1 ? TRUE : FALSE);
        }
    }

    ExFreePool(dcbuff);

    if (!NT_SUCCESS(status) && (status != STATUS_INVALID_DEVICE_REQUEST)) {
        return status;
    }

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->CDB6GENERIC.OperationCode = SCSIOP_READ_BLOCK_LIMITS;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    blockLimits = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(READ_BLOCK_LIMITS_DATA));

    if (!blockLimits) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(blockLimits, sizeof(READ_BLOCK_LIMITS_DATA));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                blockLimits,
                                sizeof(READ_BLOCK_LIMITS_DATA),
                                FALSE);

    if (NT_SUCCESS(status)) {
        tapeGetDriveParams->MaximumBlockSize = blockLimits->BlockMaximumSize[2];
        tapeGetDriveParams->MaximumBlockSize += (blockLimits->BlockMaximumSize[1] << 8);
        tapeGetDriveParams->MaximumBlockSize += (blockLimits->BlockMaximumSize[0] << 16);

        tapeGetDriveParams->MinimumBlockSize = blockLimits->BlockMinimumSize[1];
        tapeGetDriveParams->MinimumBlockSize += (blockLimits->BlockMinimumSize[0] << 8);
    }

    ExFreePool(blockLimits);

    tapeGetDriveParams->FeaturesHigh &= ~TAPE_DRIVE_HIGH_FEATURES;

    return status;

} // end TapeGetDriveParameters()


NTSTATUS
TapeGetMediaParameters(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine returns the maximum tape capacity, the remaining tape
    capacity, the fixed-length logical block size, the number of partitions
    on the tape, and an indication if the tape is write protected.  A block
    size of 0 indicates variable-length block mode.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION            deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_GET_MEDIA_PARAMETERS   tapeGetMediaParams = Irp->AssociatedIrp.SystemBuffer;
    PMODE_TAPE_MEDIA_INFORMATION modeBuffer;
    PLOG_SENSE_PARAMETER_FORMAT  logBuffer;
    SCSI_REQUEST_BLOCK           srb;
    PCDB                         cdb = (PCDB)srb.Cdb;
    NTSTATUS                     status;
    LARGE_INTEGER                capacityBuffer[2];
    LARGE_INTEGER                remainingBuffer[2];
    ULONG                        sectorShift;

    DebugPrint((3,"TapeIoControl: Get Tape Media Parameters\n"));

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SENSE.AllocationLength = sizeof(MODE_TAPE_MEDIA_INFORMATION);
    cdb->MODE_SENSE.PageCode = MODE_PAGE_MEDIUM_PARTITION;

    modeBuffer = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_TAPE_MEDIA_INFORMATION));

    if (!modeBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(modeBuffer, sizeof(MODE_TAPE_MEDIA_INFORMATION));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                modeBuffer,
                                sizeof( MODE_TAPE_MEDIA_INFORMATION ),
                                FALSE);

    if (status == STATUS_DATA_OVERRUN) {
        status = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(status)) {
        Irp->IoStatus.Information = sizeof(TAPE_GET_MEDIA_PARAMETERS);

        tapeGetMediaParams->BlockSize = modeBuffer->ParameterListBlock.BlockLength[2];
        tapeGetMediaParams->BlockSize += (modeBuffer->ParameterListBlock.BlockLength[1] << 8);
        tapeGetMediaParams->BlockSize += (modeBuffer->ParameterListBlock.BlockLength[0] << 16);

        tapeGetMediaParams->PartitionCount =
            modeBuffer->MediumPartPage.AdditionalPartitionDefined + 1;

        tapeGetMediaParams->WriteProtected =
            ((modeBuffer->ParameterListHeader.DeviceSpecificParameter >> 7) &
            0x01);

        gb_PartitionCount =
            modeBuffer->MediumPartPage.AdditionalPartitionDefined + 1;

        if (RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8) {
            capacityBuffer[0].LowPart = modeBuffer->MediumPartPage.Partition0Size[1];
            capacityBuffer[0].LowPart += (modeBuffer->MediumPartPage.Partition0Size[0] << 8);

            capacityBuffer[0].HighPart = 0;

            if (tapeGetMediaParams->PartitionCount == 2) {
                capacityBuffer[1].LowPart = modeBuffer->MediumPartPage.Partition1Size[1];
                capacityBuffer[1].LowPart += (modeBuffer->MediumPartPage.Partition1Size[0] << 8);

                capacityBuffer[1].HighPart = 0;
            } else {
                capacityBuffer[1].LowPart = 0;
                capacityBuffer[1].HighPart = 0;
            }

            capacityBuffer[0] =
                RtlLargeIntegerAdd( capacityBuffer[0], capacityBuffer[1]);
            capacityBuffer[0] =
                RtlExtendedIntegerMultiply(capacityBuffer[0], 1024000);

            tapeGetMediaParams->Capacity = capacityBuffer[0];

            tapeGetMediaParams->Remaining.LowPart = 0;
            tapeGetMediaParams->Remaining.HighPart = 0;

            WHICH_BIT(tapeGetMediaParams->BlockSize, sectorShift);
            deviceExtension->DiskGeometry->BytesPerSector = tapeGetMediaParams->BlockSize;
            deviceExtension->SectorShift = sectorShift;


        }
    }

    ExFreePool(modeBuffer);

    if (!NT_SUCCESS(status)) {

        return status;
    }

    if (!(RtlCompareMemory(gb_VendorId,"WangDAT ",8) == 8)) {
        //
        // Zero CDB in SRB on stack.
        //

        RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

        srb.CdbLength = CDB10GENERIC_LENGTH;

        cdb->LOGSENSE.OperationCode = SCSIOP_LOG_SENSE;

        //
        // Set timeout value.
        //

        srb.TimeOutValue = deviceExtension->TimeOutValue;

        cdb->LOGSENSE.AllocationLength[0] = 0;
        cdb->LOGSENSE.AllocationLength[1] = 0x24;

        cdb->LOGSENSE.PageCode = LOGSENSEPAGE31;
        cdb->LOGSENSE.PCBit = 0x01;

        logBuffer = ExAllocatePool(NonPagedPoolCacheAligned,
            sizeof(LOG_SENSE_PARAMETER_FORMAT));

        if (!logBuffer) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlZeroMemory(logBuffer, sizeof(LOG_SENSE_PARAMETER_FORMAT));

        status = ScsiClassSendSrbSynchronous(DeviceObject,
                                    &srb,
                                    logBuffer,
                                    0x24,
                                    FALSE);

        if (status == STATUS_DATA_OVERRUN) {
            status = STATUS_SUCCESS;
        }

        if (NT_SUCCESS(status)) {
            capacityBuffer[0].LowPart = logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart0[3];
            capacityBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart0[2] << 8);
            capacityBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart0[1] << 16);
            capacityBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart0[0] << 24);

            capacityBuffer[0].HighPart = 0;

            remainingBuffer[0].LowPart = logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart0[3];
            remainingBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart0[2] << 8);
            remainingBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart0[1] << 16);
            remainingBuffer[0].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart0[0] << 24);

            remainingBuffer[0].HighPart = 0;

            if (tapeGetMediaParams->PartitionCount == 2) {
                capacityBuffer[1].LowPart = logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart1[3];
                capacityBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart1[2] << 8);
                capacityBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart1[1] << 16);
                capacityBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.MaximumCapacityPart1[0] << 24);

                capacityBuffer[1].HighPart = 0;

                remainingBuffer[1].LowPart = logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart1[3];
                remainingBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart1[2] << 8);
                remainingBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart1[1] << 16);
                remainingBuffer[1].LowPart += (logBuffer->LogSensePageInfo.LogSensePage.Page31.RemainingCapacityPart1[0] << 24);

                remainingBuffer[1].HighPart = 0;

            } else {
                capacityBuffer[1].LowPart = 0;
                capacityBuffer[1].HighPart = 0;
                remainingBuffer[1].LowPart = 0;
                remainingBuffer[1].HighPart = 0;
            }

            capacityBuffer[0] =
                RtlLargeIntegerAdd( capacityBuffer[0], capacityBuffer[1]);
            capacityBuffer[0] =
                RtlExtendedIntegerMultiply(capacityBuffer[0], 1024);

            tapeGetMediaParams->Capacity = capacityBuffer[0];

            remainingBuffer[0] =
                RtlLargeIntegerAdd( remainingBuffer[0], remainingBuffer[1]);
            remainingBuffer[0] =
                RtlExtendedIntegerMultiply(remainingBuffer[0], 1024);

            tapeGetMediaParams->Remaining = remainingBuffer[0];
        }

        ExFreePool(logBuffer);
    }

    return status;

} // end TapeGetMediaParameters()


NTSTATUS
TapeGetPosition(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine returns the current position of the tape.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_GET_POSITION  tapeGetPosition = Irp->AssociatedIrp.SystemBuffer;
    PTAPE_POSITION_DATA logicalBuffer;
    PUCHAR              absoluteBuffer;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    switch (tapeGetPosition->Type) {
        case TAPE_ABSOLUTE_POSITION:
            DebugPrint((3,"TapeIoControl: Get Absolute Position\n"));
            srb.CdbLength = CDB6GENERIC_LENGTH;
            cdb->CDB6GENERIC.OperationCode = SCSIOP_REQUEST_BLOCK_ADDR;
            absoluteBuffer = ExAllocatePool(NonPagedPoolCacheAligned, 3);

            if (!absoluteBuffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status = ScsiClassSendSrbSynchronous(DeviceObject,
                                        &srb,
                                        absoluteBuffer,
                                        3,
                                        FALSE);

            if (NT_SUCCESS(status)) {
                Irp->IoStatus.Information = sizeof(TAPE_GET_POSITION);

                tapeGetPosition->Partition = 0;
                tapeGetPosition->Offset.HighPart = 0;
                tapeGetPosition->Offset.LowPart = absoluteBuffer[2];
                tapeGetPosition->Offset.LowPart += (absoluteBuffer[1] << 8);
                tapeGetPosition->Offset.LowPart += (absoluteBuffer[0] << 16);
            }

            ExFreePool(absoluteBuffer);
            break;

        case TAPE_LOGICAL_POSITION:
            DebugPrint((3,"TapeIoControl: Get Logical Position\n"));
            srb.CdbLength = CDB10GENERIC_LENGTH;
            cdb->READ_POSITION.Operation = SCSIOP_READ_POSITION;
            logicalBuffer = ExAllocatePool(NonPagedPoolCacheAligned,
                sizeof(TAPE_POSITION_DATA));

            if (!logicalBuffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            status = ScsiClassSendSrbSynchronous(DeviceObject,
                                        &srb,
                                        logicalBuffer,
                                        sizeof( TAPE_POSITION_DATA ),
                                        FALSE);

            if (NT_SUCCESS(status)) {
                Irp->IoStatus.Information = sizeof(TAPE_GET_POSITION);

                if (logicalBuffer->BlockPositionUnsupported) {
                    DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
                    return STATUS_INVALID_DEVICE_REQUEST;
                }

                tapeGetPosition->Partition = logicalBuffer->PartitionNumber + 1;
                tapeGetPosition->Offset.HighPart = 0;
                REVERSE_BYTES((PFOUR_BYTE)&tapeGetPosition->Offset.LowPart,
                    (PFOUR_BYTE)logicalBuffer->FirstBlock);
            }

            ExFreePool(logicalBuffer);
            break;

        default:
            DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
            return STATUS_INVALID_DEVICE_REQUEST;
    }

    return status;

} // end TapeGetPosition()


NTSTATUS
TapeGetStatus(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine returns the status of the device.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    DebugPrint((3,"TapeIoControl: Get Tape Status\n"));

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->CDB6GENERIC.OperationCode = SCSIOP_TEST_UNIT_READY;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                NULL,
                                0,
                                FALSE);

    return status;

} // end TapeGetStatus()


NTSTATUS
TapePrepare(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine loads, unloads, locks, or unlocks the tape.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_PREPARE       tapePrepare = Irp->AssociatedIrp.SystemBuffer;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    switch (tapePrepare->Operation) {
        case TAPE_LOAD:
            DebugPrint((3,"TapeIoControl: Load Tape\n"));
            srb.CdbLength = CDB6GENERIC_LENGTH;
            cdb->CDB6GENERIC.OperationCode = SCSIOP_LOAD_UNLOAD;
            cdb->CDB6GENERIC.CommandUniqueBytes[2] = 0x01;
            break;

        case TAPE_UNLOAD:
            DebugPrint((3,"TapeIoControl: Unload Tape\n"));
            srb.CdbLength = CDB6GENERIC_LENGTH;
            cdb->CDB6GENERIC.OperationCode = SCSIOP_LOAD_UNLOAD;
            break;

        case TAPE_LOCK:
            DebugPrint((3,"TapeIoControl: Prevent Tape Removal\n"));
            srb.CdbLength = CDB6GENERIC_LENGTH;
            cdb->CDB6GENERIC.OperationCode = SCSIOP_MEDIUM_REMOVAL;
            cdb->CDB6GENERIC.CommandUniqueBytes[2] = 0x01;
            break;

        case TAPE_UNLOCK:
            DebugPrint((3,"TapeIoControl: Allow Tape Removal\n"));
            srb.CdbLength = CDB6GENERIC_LENGTH;
            cdb->CDB6GENERIC.OperationCode = SCSIOP_MEDIUM_REMOVAL;
            break;

        default:
            DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
            return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Set immediate bit if indicated.
    //

    cdb->CDB6GENERIC.Immediate = tapePrepare->Immediate;

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                NULL,
                                0,
                                FALSE);

    return status;

} // end TapePrepare()

NTSTATUS
TapeReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine builds SRBs and CDBs for read and write requests to 4MM DAT
    devices.

Arguments:

    DeviceObject
    Irp

Return Value:

    Returns STATUS_PENDING.

--*/

  {
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);
    PSCSI_REQUEST_BLOCK srb;
    PCDB cdb;
    KIRQL currentIrql;
    ULONG transferBlocks;
    LARGE_INTEGER startingOffset =
      currentIrpStack->Parameters.Read.ByteOffset;

    //
    // Allocate an Srb.
    //

    if (deviceExtension->SrbZone != NULL &&
        (srb = ExInterlockedAllocateFromZone(
            deviceExtension->SrbZone,
            deviceExtension->SrbZoneSpinLock)) != NULL) {

        srb->SrbFlags = SRB_FLAGS_ALLOCATED_FROM_ZONE;

    } else {

        //
        // Allocate Srb from non-paged pool.
        // This call must succeed.
        //

        srb = ExAllocatePool(NonPagedPoolMustSucceed, SCSI_REQUEST_BLOCK_SIZE);

        srb->SrbFlags = 0;

    }

    //
    // Write length to SRB.
    //

    srb->Length = SCSI_REQUEST_BLOCK_SIZE;

    //
    // Set up IRP Address.
    //

    srb->OriginalRequest = Irp;

    //
    // Set up target id and logical unit number.
    //

    srb->PathId = deviceExtension->PathId;
    srb->TargetId = deviceExtension->TargetId;
    srb->Lun = deviceExtension->Lun;


    srb->Function = SRB_FUNCTION_EXECUTE_SCSI;

    srb->DataBuffer = MmGetMdlVirtualAddress(Irp->MdlAddress);

    //
    // Save byte count of transfer in SRB Extension.
    //

    srb->DataTransferLength = currentIrpStack->Parameters.Read.Length;

    //
    // Indicate auto request sense by specifying buffer and size.
    //

    srb->SenseInfoBuffer = deviceExtension->SenseData;

    srb->SenseInfoBufferLength = SENSE_BUFFER_SIZE;

    //
    // Initialize the queue actions field.
    //

    srb->QueueAction = SRB_SIMPLE_TAG_REQUEST;

    //
    // Indicate auto request sense by specifying buffer and size.
    //

    srb->SenseInfoBuffer = deviceExtension->SenseData;

    srb->SenseInfoBufferLength = SENSE_BUFFER_SIZE;

    //
    // Set timeout value in seconds.
    //

    srb->TimeOutValue = deviceExtension->TimeOutValue;

    //
    // Zero statuses.
    //

    srb->SrbStatus = srb->ScsiStatus = 0;

    srb->NextSrb = 0;

    //
    // Indicate that 6-byte CDB's will be used.
    //

    srb->CdbLength = CDB6GENERIC_LENGTH;

    //
    // Fill in CDB fields.
    //

    cdb = (PCDB)srb->Cdb;

    //
    // Zero CDB in SRB.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    //
    // Since we are writing fixed block mode, normalize transfer count
    // to number of blocks.
    //

    transferBlocks =
        currentIrpStack->Parameters.Read.Length >> deviceExtension->SectorShift;

    //
    // Set up transfer length
    //

    cdb->CDB6READWRITETAPE.TransferLenMSB = (UCHAR)((transferBlocks >> 16) & 0xff);
    cdb->CDB6READWRITETAPE.TransferLen    = (UCHAR)((transferBlocks >> 8) & 0xff);
    cdb->CDB6READWRITETAPE.TransferLenLSB = (UCHAR)(transferBlocks & 0xff);

    //
    // Tell the python we are in fixed block mode
    //

    cdb->CDB6READWRITETAPE.VendorSpecific = 1;

    //
    // Set transfer direction flag and Cdb command.
    //

    if (currentIrpStack->MajorFunction == IRP_MJ_READ) {

         DebugPrint((3, "TapeRequest: Read Command\n"));

         srb->SrbFlags = SRB_FLAGS_DATA_IN;
         cdb->CDB6READWRITETAPE.OperationCode = SCSIOP_READ6;

    } else {

         DebugPrint((3, "TapeRequest: Write Command\n"));

         srb->SrbFlags = SRB_FLAGS_DATA_OUT;
         cdb->CDB6READWRITETAPE.OperationCode = SCSIOP_WRITE6;
    }

    //
    // Or in the default flags from the device object.
    //

    srb->SrbFlags |= deviceExtension->SrbFlags;

    //
    // Set up major SCSI function.
    //

    nextIrpStack->MajorFunction = IRP_MJ_SCSI;

    //
    // Save SRB address in next stack for port driver.
    //

    nextIrpStack->Parameters.Scsi.Srb = srb;

    //
    // Save retry count in current IRP stack.
    //

    currentIrpStack->Parameters.Others.Argument4 = (PVOID)MAXIMUM_RETRIES;

    //
    // Set up IoCompletion routine address.
    //

    IoSetCompletionRoutine(Irp,
                           ScsiClassIoComplete,
                           srb,
                           TRUE,
                           TRUE,
                           FALSE);

    return STATUS_PENDING;

} // end TapeReadWrite()

NTSTATUS
TapeSetDriveParameters(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine enables/disables reporting of setmarks.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION          deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_SET_DRIVE_PARAMETERS tapeSetDriveParams = Irp->AssociatedIrp.SystemBuffer;
    PMODE_DEVICE_CONFIG_PAGE   buffer;
    SCSI_REQUEST_BLOCK         srb;
    PCDB                       cdb = (PCDB)srb.Cdb;
    NTSTATUS                   status;
    PMODE_DATA_COMPRESS_PAGE   dcbuff;

    DebugPrint((3,"TapeIoControl: Set Tape Drive Parameters\n"));

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SENSE.AllocationLength = sizeof(MODE_DEVICE_CONFIG_PAGE);
    cdb->MODE_SENSE.Dbd = SETBITON;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_DEVICE_CONFIG;

    buffer = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_DEVICE_CONFIG_PAGE));

    if (!buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, sizeof(MODE_DEVICE_CONFIG_PAGE));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                buffer,
                                sizeof( MODE_DEVICE_CONFIG_PAGE ),
                                FALSE);

    if (!NT_SUCCESS(status)) {
        ExFreePool(buffer);
        return status;
    }

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SELECT.OperationCode = SCSIOP_MODE_SELECT;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SELECT.ParameterListLength = sizeof(MODE_DEVICE_CONFIG_PAGE);
    cdb->MODE_SELECT.PFBit = 1;

    buffer->ParameterListHeader.ModeDataLength = 0;
    buffer->ParameterListHeader.MediumType = 0;
    buffer->ParameterListHeader.DeviceSpecificParameter = 0x10;
    buffer->ParameterListHeader.BlockDescriptorLength = 0;

    buffer->DeviceConfigPage.PageCode = MODE_PAGE_DEVICE_CONFIG;
    buffer->DeviceConfigPage.PageLength = 0x0E;

    if (tapeSetDriveParams->ReportSetmarks) {
        buffer->DeviceConfigPage.RSmk = SETBITON;
    } else {
        buffer->DeviceConfigPage.RSmk = SETBITOFF;
    }

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                buffer,
                                sizeof( MODE_DEVICE_CONFIG_PAGE ),
                                TRUE);

    ExFreePool(buffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SENSE.OperationCode = SCSIOP_MODE_SENSE;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SENSE.AllocationLength = sizeof(MODE_DATA_COMPRESS_PAGE);
    cdb->MODE_SENSE.Dbd = SETBITON;
    cdb->MODE_SENSE.PageCode = MODE_PAGE_DATA_COMPRESS;

    dcbuff = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_DATA_COMPRESS_PAGE));

    if (!dcbuff) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(dcbuff, sizeof(MODE_DATA_COMPRESS_PAGE));

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                dcbuff,
                                sizeof( MODE_DATA_COMPRESS_PAGE ),
                                FALSE);

    if (!NT_SUCCESS(status) && (status != STATUS_INVALID_DEVICE_REQUEST)) {
        ExFreePool(dcbuff);
        return status;
    }

    if (NT_SUCCESS(status) && dcbuff->DataCompressPage.DCC == 1) {
        //
        // Zero CDB in SRB on stack.
        //

        RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

        srb.CdbLength = CDB6GENERIC_LENGTH;

        cdb->MODE_SELECT.OperationCode = SCSIOP_MODE_SELECT;

        //
        // Set timeout value.
        //

        srb.TimeOutValue = deviceExtension->TimeOutValue;

        cdb->MODE_SELECT.ParameterListLength = sizeof(MODE_DATA_COMPRESS_PAGE);
        cdb->MODE_SELECT.PFBit = 1;

        dcbuff->ParameterListHeader.ModeDataLength = 0;
        dcbuff->ParameterListHeader.MediumType = 0;
        dcbuff->ParameterListHeader.DeviceSpecificParameter = 0x10;
        dcbuff->ParameterListHeader.BlockDescriptorLength = 0;

        dcbuff->DataCompressPage.PageCode = MODE_PAGE_DATA_COMPRESS;
        dcbuff->DataCompressPage.PageLength = 0x0E;

        if (tapeSetDriveParams->Compression) {
            dcbuff->DataCompressPage.DCE = SETBITON;
        } else {
            dcbuff->DataCompressPage.DCE = SETBITOFF;
        }

        status = ScsiClassSendSrbSynchronous(DeviceObject,
                                    &srb,
                                    dcbuff,
                                    sizeof( MODE_DATA_COMPRESS_PAGE ),
                                    TRUE);
    } else if (status == STATUS_INVALID_DEVICE_REQUEST) {
        status = STATUS_SUCCESS;
    }

    ExFreePool(dcbuff);

    return status;

} // end TapeSetDriveParameters()


NTSTATUS
TapeSetMediaParameters(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine sets the fixed-length logical block size or variable-length
    block mode (if the block size is 0).

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION          deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_SET_MEDIA_PARAMETERS tapeSetMediaParams = Irp->AssociatedIrp.SystemBuffer;
    PMODE_PARM_READ_WRITE_DATA buffer;
    SCSI_REQUEST_BLOCK         srb;
    PCDB                       cdb = (PCDB)srb.Cdb;
    NTSTATUS                   status;

    DebugPrint((3,"TapeIoControl: Set Tape Media Parameters \n"));

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->MODE_SELECT.OperationCode = SCSIOP_MODE_SELECT;

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    cdb->MODE_SELECT.ParameterListLength = sizeof(MODE_PARM_READ_WRITE_DATA);
    cdb->MODE_SELECT.PFBit = 1;

    buffer = ExAllocatePool(NonPagedPoolCacheAligned,
        sizeof(MODE_PARM_READ_WRITE_DATA));

    if (!buffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(buffer, sizeof(MODE_PARM_READ_WRITE_DATA));

    buffer->ParameterListHeader.DeviceSpecificParameter = 0x10;
    buffer->ParameterListHeader.BlockDescriptorLength = MODE_BLOCK_DESC_LENGTH;

    buffer->ParameterListBlock.BlockLength[0] =
        ((tapeSetMediaParams->BlockSize >> 16) & 0xFF);
    buffer->ParameterListBlock.BlockLength[1] =
        ((tapeSetMediaParams->BlockSize >> 8) & 0xFF);
    buffer->ParameterListBlock.BlockLength[2] =
        (tapeSetMediaParams->BlockSize & 0xFF);

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                buffer,
                                sizeof( MODE_PARM_READ_WRITE_DATA ),
                                TRUE);

    ExFreePool(buffer);

    return status;

} // end TapeSetMediaParameters()


NTSTATUS
TapeSetPosition(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine sets the position of the tape.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_DATA          tapeData = (PTAPE_DATA)(deviceExtension + 1);
    PTAPE_SET_POSITION  tapeSetPosition = Irp->AssociatedIrp.SystemBuffer;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    if (tapeSetPosition->Immediate) {
        switch (tapeSetPosition->Method) {
            case TAPE_REWIND:
            case TAPE_ABSOLUTE_BLOCK:
            case TAPE_LOGICAL_BLOCK:
                break;

            case TAPE_SPACE_END_OF_DATA:
            case TAPE_SPACE_RELATIVE_BLOCKS:
            case TAPE_SPACE_FILEMARKS:
            case TAPE_SPACE_SEQUENTIAL_FMKS:
            case TAPE_SPACE_SETMARKS:
            case TAPE_SPACE_SEQUENTIAL_SMKS:
            default:
                return STATUS_NOT_IMPLEMENTED;
        }
    }

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    srb.CdbLength = CDB6GENERIC_LENGTH;

    //
    // Set immediate bit if indicated.
    //

    cdb->CDB6GENERIC.Immediate = tapeSetPosition->Immediate;

    switch (tapeSetPosition->Method) {
        case TAPE_REWIND:
            DebugPrint((3,"TapeIoControl: Rewind Tape\n"));
            cdb->CDB6GENERIC.OperationCode = SCSIOP_REWIND;

            break;

        case TAPE_ABSOLUTE_BLOCK:
            DebugPrint((3,"TapeIoControl: Position Tape to an Absolute Block\n"));

            srb.TimeOutValue = 360;

            cdb->CDB6GENERIC.OperationCode = SCSIOP_SEEK_BLOCK;

            cdb->CDB6GENERIC.CommandUniqueBytes[0] =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->CDB6GENERIC.CommandUniqueBytes[1] =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->CDB6GENERIC.CommandUniqueBytes[2] =
                (tapeSetPosition->Offset.LowPart & 0xFF);
            break;

        case TAPE_LOGICAL_BLOCK:
            DebugPrint((3,"TapeIoControl: Position Tape to a Logical Block\n"));

            srb.TimeOutValue = 360;

            cdb->LOCATE.OperationCode = SCSIOP_LOCATE;

            srb.CdbLength = CDB10GENERIC_LENGTH;

            if ((tapeSetPosition->Partition != (ULONG)0) &&
                (tapeSetPosition->Partition != (ULONG)tapeData->CurrentPartition) &&
                gb_PartitionCount > 1) {
                cdb->LOCATE.CPBit = SETBITON;
                cdb->LOCATE.Partition =
                    tapeSetPosition->Partition - 1;
            }
            cdb->LOCATE.LogicalBlockAddress[0] =
                ((tapeSetPosition->Offset.LowPart >> 24) & 0xFF);
            cdb->LOCATE.LogicalBlockAddress[1] =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->LOCATE.LogicalBlockAddress[2] =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->LOCATE.LogicalBlockAddress[3] =
                (tapeSetPosition->Offset.LowPart & 0xFF);

            break;

        case TAPE_SPACE_END_OF_DATA:
            DebugPrint((3,"TapeIoControl: Position Tape to End-of-Data\n"));
            cdb->SPACE_TAPE_MARKS.OperationCode = SCSIOP_SPACE;
            cdb->SPACE_TAPE_MARKS.Code = 3;
            break;

        case TAPE_SPACE_RELATIVE_BLOCKS:
            DebugPrint((3,"TapeIoControl: Position Tape by Spacing Blocks\n"));
            cdb->SPACE_TAPE_MARKS.OperationCode = SCSIOP_SPACE;
            cdb->SPACE_TAPE_MARKS.Code = 0;
            cdb->SPACE_TAPE_MARKS.NumMarksMSB =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarks =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarksLSB =
                (tapeSetPosition->Offset.LowPart & 0xFF);
            break;

        case TAPE_SPACE_FILEMARKS:
            DebugPrint((3,"TapeIoControl: Position Tape by Spacing Filemarks\n"));
            cdb->SPACE_TAPE_MARKS.OperationCode = SCSIOP_SPACE;
            cdb->SPACE_TAPE_MARKS.Code = 1;
            cdb->SPACE_TAPE_MARKS.NumMarksMSB =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarks =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarksLSB =
                (tapeSetPosition->Offset.LowPart & 0xFF);
            break;

        case TAPE_SPACE_SEQUENTIAL_FMKS:
            DebugPrint((3,"TapeIoControl: Position Tape by Spacing Sequential Filemarks\n"));
            cdb->SPACE_TAPE_MARKS.OperationCode = SCSIOP_SPACE;
            cdb->SPACE_TAPE_MARKS.Code = 2;
            cdb->SPACE_TAPE_MARKS.NumMarksMSB =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarks =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarksLSB =
                (tapeSetPosition->Offset.LowPart & 0xFF);
            break;

        case TAPE_SPACE_SETMARKS:
            DebugPrint((3,"TapeIoControl: Position Tape by Spacing Setmarks\n"));
            cdb->SPACE_TAPE_MARKS.OperationCode = SCSIOP_SPACE;
            cdb->SPACE_TAPE_MARKS.Code = 4;
            cdb->SPACE_TAPE_MARKS.NumMarksMSB =
                ((tapeSetPosition->Offset.LowPart >> 16) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarks =
                ((tapeSetPosition->Offset.LowPart >> 8) & 0xFF);
            cdb->SPACE_TAPE_MARKS.NumMarksLSB =
                (tapeSetPosition->Offset.LowPart & 0xFF);
            break;

        case TAPE_SPACE_SEQUENTIAL_SMKS:
        default:
            DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
            return STATUS_INVALID_DEVICE_REQUEST;
    }

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                NULL,
                                0,
                                FALSE);

    if (NT_SUCCESS(status)) {
        switch (tapeSetPosition->Method) {
            case TAPE_LOGICAL_BLOCK:
                tapeData->CurrentPartition = tapeSetPosition->Partition;
                break;
        }
    }

    return status;

} // end TapeSetPosition()


BOOLEAN
TapeVerifyInquiry(
    IN PSCSI_INQUIRY_DATA LunInfo
    )

/*++
Routine Description:

    This routine determines if the driver should claim this device.

Arguments:

    LunInfo

Return Value:

    TRUE - driver should claim this device.
    FALSE - driver should not claim this device.

--*/

{
    PINQUIRYDATA inquiryData;

    DebugPrint((3,"TapeIoControl: Verify Tape Inquiry Data\n"));

    inquiryData = (PVOID)LunInfo->InquiryData;

    //
    //  Determine, from the Product ID field in the
    //  inquiry data, whether or not to "claim" this drive.
    //

    if (WhichIsIt(inquiryData)) {

        RtlMoveMemory(gb_VendorId,inquiryData->VendorId,8);
        RtlMoveMemory(gb_ProductId,inquiryData->ProductId,16);
        return TRUE;

    }
    return FALSE;

} // end TapeVerifyInquiry()


NTSTATUS
TapeWriteMarks(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++
Routine Description:

    This routine writes tapemarks on the tape.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION   deviceExtension = DeviceObject->DeviceExtension;
    PTAPE_WRITE_MARKS   tapeWriteMarks = Irp->AssociatedIrp.SystemBuffer;
    SCSI_REQUEST_BLOCK  srb;
    PCDB                cdb = (PCDB)srb.Cdb;
    NTSTATUS            status;

    //
    // Zero CDB in SRB on stack.
    //

    RtlZeroMemory(cdb, MAXIMUM_CDB_SIZE);

    srb.CdbLength = CDB6GENERIC_LENGTH;

    cdb->WRITE_TAPE_MARKS.OperationCode = SCSIOP_WRITE_FILEMARKS;

    cdb->WRITE_TAPE_MARKS.TransferLength[0] =
        ((tapeWriteMarks->Count >> 16) & 0xFF);

    cdb->WRITE_TAPE_MARKS.TransferLength[1] =
        ((tapeWriteMarks->Count >> 8) & 0xFF);

    cdb->WRITE_TAPE_MARKS.TransferLength[2] =
        (tapeWriteMarks->Count & 0xFF);

    //
    // Set timeout value.
    //

    srb.TimeOutValue = deviceExtension->TimeOutValue;

    switch (tapeWriteMarks->Type) {
        case TAPE_SETMARKS:
            DebugPrint((3,"TapeIoControl: Write Setmarks to Tape\n"));
            cdb->WRITE_TAPE_MARKS.WriteSetMarks = SETBITON;
            break;

        case TAPE_FILEMARKS:
            DebugPrint((3,"TapeIoControl: Write Filemarks to Tape\n"));
            break;

        case TAPE_SHORT_FILEMARKS:
        case TAPE_LONG_FILEMARKS:
        default:
            DebugPrint((3,"TapeIoControl: Tape Operation Not Supported\n"));
            return STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Set immediate bit if indicated.
    //

    cdb->WRITE_TAPE_MARKS.Immediate = tapeWriteMarks->Immediate;


    srb.CdbLength = CDB6GENERIC_LENGTH;

    status = ScsiClassSendSrbSynchronous(DeviceObject,
                                &srb,
                                NULL,
                                0,
                                FALSE);

    return status;

} // end TapeWriteMarks()


static
ULONG
WhichIsIt(
    IN PINQUIRYDATA InquiryData
    )

/*++
Routine Description:

    This routine determines a drive's identity from the Product ID field
    in its inquiry data.

Arguments:

    InquiryData (from an Inquiry command)

Return Value:

    driveID

--*/

{
    if (RtlCompareMemory(InquiryData->VendorId,"ARCHIVE ",8) == 8) {

        if (RtlCompareMemory(InquiryData->ProductId,"Python",6) == 6) {
            return ARCHIVE_PYTHON;
        }

    }

    if (RtlCompareMemory(InquiryData->VendorId,"DEC     ",8) == 8) {

        if (RtlCompareMemory(InquiryData->ProductId,"TLZ06",5) == 5) {
            return DEC_TLZ06;
        }

    }

    if (RtlCompareMemory(InquiryData->VendorId,"EXABYTE ",8) == 8) {

        if (RtlCompareMemory(InquiryData->ProductId,"EXB-4200 ",9) == 9) {
            return EXABYTE_4200;
        }

        if (RtlCompareMemory(InquiryData->ProductId,"EXB-4200c",9) == 9) {
            return EXABYTE_4200C;
        }

    }

    if (RtlCompareMemory(InquiryData->VendorId,"HP      ",8) == 8) {

        if (RtlCompareMemory(InquiryData->ProductId,"HP35470A",8) == 8) {
            return HP_35470A;
        }

        if (RtlCompareMemory(InquiryData->ProductId,"HP35480A",8) == 8) {
            return HP_35480A;
        }

        if (RtlCompareMemory(InquiryData->ProductId,"IBM35480A",9) == 9) {
            return HP_IBM35480A;
        }

    }

    if (RtlCompareMemory(InquiryData->VendorId,"WangDAT ",8) == 8) {

        if (RtlCompareMemory(InquiryData->ProductId,"Model 1300",10) == 10) {
            return WANGDAT_1300;
        }

        if (RtlCompareMemory(InquiryData->ProductId,"Model 3100",10) == 10) {
            return WANGDAT_3100;
        }

        if (RtlCompareMemory(InquiryData->ProductId,"Model 3200",10) == 10) {
            return WANGDAT_3200;
        }

    }

    return 0;
}
