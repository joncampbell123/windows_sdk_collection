/*++

Copyright (c) 1993 - Colorado Memory Systems,  Inc.
All Rights Reserved

Module Name:

    ioctl.c

Abstract:

    Tape IOCTL support for NT Backup aplication.

Revision History:




--*/

//
// Includes
//

#include <ntddk.h>
#include <ntddtape.h>   // tape device driver I/O control codes
#include "common.h"
#include "q117.h"
#include "protos.h"
#include "cms.h"


NTSTATUS
q117IoCtlGetPosition (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT       context;
    PTAPE_GET_POSITION  currentPosition;
    PLARGE_INTEGER      offset;

    context = DeviceObject->DeviceExtension;
    currentPosition = Irp->AssociatedIrp.SystemBuffer;

    Irp->IoStatus.Information = sizeof(TAPE_GET_POSITION);

    //
    // signal no partition support
    //
    currentPosition->Partition = 0;

    //
    // Fill in the CurrentOperation.Position based on the mode
    //
    offset = &currentPosition->Offset;

    offset->HighPart = (LONG)0;


    switch (context->CurrentOperation.Type) {

        case BackupInProgress:
            offset->LowPart = context->CurrentOperation.BytesOnTape;
            break;

        case RestoreInProgress:
            offset->LowPart = context->CurrentOperation.BytesRead;
            break;

        case NoOperation:
            offset->LowPart = context->CurrentOperation.BytesRead;
            break;

    }
    offset->LowPart /= BLOCK_SIZE;

    CheckedDump(QIC117SHOWTD,("%d=GetPosition()",currentPosition->Offset.LowPart));

    return STATUS_SUCCESS;
}

NTSTATUS
q117IoCtlGetMediaParameters (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT   context;
    NTSTATUS        ntStatus;

    context = DeviceObject->DeviceExtension;

    //
    // Make sure there is a tape in the drive and that the tape information
    // has been loaded
    //
    if (context->CurrentOperation.Type == NoOperation) {

        ntStatus = q117ConvertStatus(DeviceObject, q117CheckNewTape(context));

    } else {

        ntStatus = STATUS_SUCCESS;

    }


    if ( NT_SUCCESS( ntStatus ) ) {
        //
        // Copy already formed (by q117CheckNewTape) information into callers buffer
        //
        Irp->IoStatus.Information = sizeof(TAPE_GET_MEDIA_PARAMETERS);

        *(PTAPE_GET_MEDIA_PARAMETERS)Irp->AssociatedIrp.SystemBuffer =
            *context->CurrentTape.MediaInfo;

    }

    return ntStatus;
}

NTSTATUS
q117IoCtlSetMediaParameters (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT                context;
    PTAPE_SET_MEDIA_PARAMETERS   setMedia;

    context = DeviceObject->DeviceExtension;

    setMedia = (PTAPE_SET_MEDIA_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;

    CheckedDump((QIC117SHOWTD | QIC117WARN),("SetDriveParameters not implemented yet\n"));
    CheckedDump(QIC117SHOWTD,("BlockSize: %x",setMedia->BlockSize));

    if (setMedia->BlockSize != BLOCK_SIZE)
        return STATUS_INVALID_PARAMETER;
    else
        return STATUS_SUCCESS;
}

NTSTATUS
q117IoCtlGetDriveParameters (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT             context;
    PTAPE_GET_DRIVE_PARAMETERS driveInfo;

    context = DeviceObject->DeviceExtension;

    //
    // Copy already formed (by q117CheckNewTape) information into callers buffer
    //
    //
    driveInfo = (PTAPE_GET_DRIVE_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;
    Irp->IoStatus.Information = sizeof(TAPE_GET_DRIVE_PARAMETERS);

    driveInfo->ECC = TRUE;
    driveInfo->Compression = FALSE;
    driveInfo->DataPadding = FALSE;
    driveInfo->ReportSetmarks = TRUE;
    driveInfo->DefaultBlockSize = BLOCK_SIZE;
    driveInfo->MaximumBlockSize = BLOCK_SIZE;
    driveInfo->MinimumBlockSize = BLOCK_SIZE;
    driveInfo->MaximumPartitionCount = 0;
    driveInfo->FeaturesLow =
        TAPE_DRIVE_ERASE_SHORT |
        TAPE_DRIVE_ERASE_BOP_ONLY |
        TAPE_DRIVE_TAPE_CAPACITY |
        TAPE_DRIVE_TAPE_REMAINING |
        TAPE_DRIVE_FIXED_BLOCK |
        TAPE_DRIVE_WRITE_PROTECT |
        TAPE_DRIVE_ECC |
        TAPE_DRIVE_COMPRESSION |
        TAPE_DRIVE_REPORT_SMKS |
        TAPE_DRIVE_GET_ABSOLUTE_BLK |
        TAPE_DRIVE_GET_LOGICAL_BLK;
    driveInfo->FeaturesHigh =
        TAPE_DRIVE_LOAD_UNLOAD |
        TAPE_DRIVE_TENSION |
        TAPE_DRIVE_LOCK_UNLOCK |
        TAPE_DRIVE_ABSOLUTE_BLK |
        TAPE_DRIVE_LOGICAL_BLK |
        TAPE_DRIVE_END_OF_DATA |
        TAPE_DRIVE_RELATIVE_BLKS |
        TAPE_DRIVE_FILEMARKS |
        TAPE_DRIVE_SEQUENTIAL_FMKS |
        TAPE_DRIVE_SETMARKS |
        TAPE_DRIVE_SEQUENTIAL_SMKS |
        TAPE_DRIVE_REVERSE_POSITION |
        TAPE_DRIVE_WRITE_SETMARKS |
        TAPE_DRIVE_WRITE_FILEMARKS |
        TAPE_DRIVE_FORMAT;

    driveInfo->FeaturesHigh &= ~TAPE_DRIVE_HIGH_FEATURES;

    return STATUS_SUCCESS;
}

NTSTATUS
q117IoCtlSetDriveParameters (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT             context;
    PTAPE_SET_DRIVE_PARAMETERS driveInfo;
    NTSTATUS            ntStatus;

    context = DeviceObject->DeviceExtension;

    //
    // Copy already formed (by q117CheckNewTape) information into callers buffer
    //
    //
    driveInfo = (PTAPE_SET_DRIVE_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;

    CheckedDump((QIC117SHOWTD | QIC117WARN),("SetDriveParameters not implemented yet\n"));
    CheckedDump(QIC117SHOWTD,("ECC: %x",driveInfo->ECC));
    CheckedDump(QIC117SHOWTD,("Compression: %x",driveInfo->Compression));
    CheckedDump(QIC117SHOWTD,("DataPadding: %x",driveInfo->DataPadding));
    CheckedDump(QIC117SHOWTD,("ReportSetmarks: %x",driveInfo->ReportSetmarks));
    ntStatus = STATUS_SUCCESS;
    if (!driveInfo->ECC ||
        driveInfo->Compression ||
        driveInfo->DataPadding ||
        !driveInfo->ReportSetmarks) {

        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    }

    return ntStatus;
}

NTSTATUS
q117IoCtlWriteMarks (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    Handle user request to write tape mark

Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
#ifndef NO_MARKS

    PQ117_CONTEXT       context;
    PTAPE_WRITE_MARKS   tapeMarks = Irp->AssociatedIrp.SystemBuffer;
    ULONG               numMarks;
    NTSTATUS            ntStatus;
    ULONG               type;

    context = DeviceObject->DeviceExtension;

    //
    // Make sure we are in write mode
    //
    ntStatus = q117ConvertStatus(DeviceObject, q117OpenForWrite(context));

    numMarks = tapeMarks->Count;
    type = tapeMarks->Type;

    //
    // Don't allow long/short filemarks
    //
    switch(type) {
        case TAPE_LONG_FILEMARKS:
        case TAPE_SHORT_FILEMARKS:
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
    }

    //
    // Put as many marks as the user asked for,  in the mark array
    //
    while (numMarks && NT_SUCCESS( ntStatus )) {

        context->MarkArray.MarkEntry[
            context->MarkArray.TotalMarks].Type = tapeMarks->Type;

        context->MarkArray.MarkEntry[
            context->MarkArray.TotalMarks].Offset =
            context->CurrentOperation.BytesOnTape;

        --numMarks;

        ++context->MarkArray.TotalMarks;
        ++context->CurrentMark;

        //
        // Always make the (last mark) huge so we don't have to check
        // for the end of the table in the rest of the code.
        //
        context->MarkArray.MarkEntry[
            context->MarkArray.TotalMarks].Offset = 0xffffffff;

        //
        // For each mark, write a "fake" block on the tape.  This
        // is due to the ntBackup program assuming that a filemark
        // takes a block
        //

        ntStatus = q117ConvertStatus(
            DeviceObject,
            q117WriteTape(NULL,BLOCK_SIZE,context)
            );
    }

    return ntStatus;

#else

    return STATUS_INVALID_DEVICE_REQUEST;

#endif
}

NTSTATUS
q117IoCtlSetPosition (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT             context;
    PTAPE_SET_POSITION tapePosition = Irp->AssociatedIrp.SystemBuffer;
    STATUS status;
    NTSTATUS ntStatus;
    IO_REQUEST ioreq;
    ULONG offset;
#ifndef NO_MARKS
    int x = 0;
#endif

    context = DeviceObject->DeviceExtension;

    status = NoErr;
    ntStatus = STATUS_SUCCESS;

    if (context->CurrentOperation.Type != NoOperation) {
        switch(context->CurrentOperation.Type) {

            case BackupInProgress:
                status = q117EndWriteOperation(context);
                break;

            case RestoreInProgress:
                if (
                    tapePosition->Method == TAPE_REWIND
                    ) {
                    status = q117EndReadOperation(context);
                }

                break;
        }
    }

    context->CurrentOperation.Position = tapePosition->Method;
    switch(tapePosition->Method) {

        case TAPE_REWIND:

            CheckedDump(QIC117INFO,("Rewind()\n"));
            status = q117DoCmd(&ioreq, DEject, NULL, context);

//            context->TapeStatus.Status |= TAPE_STATUS_BEGINNING_OF_MEDIA;
//            context->TapeStatus.Status &= ~TAPE_STATUS_END_OF_MEDIA;

            context->CurrentOperation.BytesRead = 0;

#ifndef NO_MARKS
            context->CurrentMark = 0;
#endif
            break;

        case TAPE_LOGICAL_BLOCK:
        case TAPE_ABSOLUTE_BLOCK:

            CheckedDump(QIC117SHOWTD,(
                "%s SeekBlock(%d)\n",
                tapePosition->Method==TAPE_LOGICAL_BLOCK?"Logical":"Absolute",
                tapePosition->Offset.LowPart
                ));

            offset = (tapePosition->Offset.LowPart)*BLOCK_SIZE;

            ntStatus = q117SeekToOffset(offset, context, DeviceObject);

            break;

        case TAPE_SPACE_END_OF_DATA:
            //
            // This will be taken care of when backup starts
            //  by using the context->CurrentOperation.Position
            //
            // It is assumed that this function will only be called prior
            //  to a backup operation only.

            CheckedDump(QIC117SHOWTD,("SeekEOD()\n"));
            context->CurrentOperation.BytesRead =
                context->ActiveVolume.DataSize;

#ifndef NO_MARKS
            context->CurrentMark = context->MarkArray.TotalMarks;
#endif

//            context->TapeStatus.Status |= TAPE_STATUS_END_OF_MEDIA;
//            context->TapeStatus.Status &= ~TAPE_STATUS_BEGINNING_OF_MEDIA;
            break;


        case TAPE_SPACE_RELATIVE_BLOCKS:


            CheckedDump(QIC117SHOWTD,("SeekRelBlock(%d)\n",tapePosition->Offset.LowPart));
            //
            // Convert relative offset into absolute
            //
            offset = (LONG)context->CurrentOperation.BytesRead +
                    ((LONG)tapePosition->Offset.LowPart*BLOCK_SIZE);

            //
            // Perform absolute seek.
            //
            ntStatus = q117SeekToOffset(offset,context, DeviceObject);

            break;

#ifndef NO_MARKS
        case TAPE_SPACE_SETMARKS:
            ++x;
        case TAPE_SPACE_FILEMARKS:
            ++x;
        case TAPE_SPACE_SEQUENTIAL_FMKS:
            ++x;
        case TAPE_SPACE_SEQUENTIAL_SMKS:
            if (1) {
                static char *type[4] = {"SequentialSet","SequentialFile","File","Set"};

                CheckedDump(QIC117SHOWTD,("Seek%sMark(%d)\n",type[x],tapePosition->Offset.LowPart));
            }
            ntStatus = q117FindMark(tapePosition->Method,
                         tapePosition->Offset.LowPart, context, DeviceObject);
            break;
#else
        case TAPE_SPACE_SETMARKS:
        case TAPE_SPACE_FILEMARKS:
        case TAPE_SPACE_SEQUENTIAL_FMKS:
        case TAPE_SPACE_SEQUENTIAL_SMKS:
#endif
        default:
            CheckedDump(QIC117DBGP,("TAPE: Position: Invalid Position Code (%x)\n",
                tapePosition->Method));

            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            break;

    } // end switch(tapePosition->Method)

    if (status)
        ntStatus = q117ConvertStatus(DeviceObject, status);

    return ntStatus;

}

#ifndef NO_MARKS
NTSTATUS
q117FindMark(
    ULONG Type,
    LONG Number,
    PQ117_CONTEXT Context,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:



Arguments:


Return Value:

    NT Status

--*/

{
    NTSTATUS ntStatus;
    BOOLEAN forwardSeek;

    ntStatus = STATUS_SUCCESS;

    //
    // Convert the SetPosition commands into WriteMark types
    //
    switch(Type) {

        case TAPE_SPACE_SEQUENTIAL_FMKS:

            //
            // If sequential mark,  then always start from the first mark.
            //
            Context->CurrentMark = 0;

        case TAPE_SPACE_FILEMARKS:

            Type = TAPE_FILEMARKS;
            break;

        case TAPE_SPACE_SEQUENTIAL_SMKS:

            //
            // If sequential mark,  then always start from the first mark.
            //
            Context->CurrentMark = 0;

        case TAPE_SPACE_SETMARKS:

            Type = TAPE_SETMARKS;
            break;

    }

    if (Number > 0) {
        forwardSeek = TRUE;
    } else {
        forwardSeek = FALSE;
    }


    //
    // Now seek the appropriate amount
    //
    while (NT_SUCCESS( ntStatus ) && Number != 0) {

        if (forwardSeek) {

            if (Context->CurrentMark >= Context->MarkArray.TotalMarks) {

                ntStatus = STATUS_END_OF_MEDIA;

            } else {

                if (Context->MarkArray.MarkEntry[Context->CurrentMark].Type ==
                    Type) {

                    //
                    // If we found one,  decrement the count
                    //

                    --Number;


                    //
                    // Don't increment the current mark on the last one we
                    // find.  This is because current mark points to
                    // the mark we are going to hit next.
                    //

                    if (Number) {

                        ++Context->CurrentMark;
                    }

                } else {

                    ++Context->CurrentMark;

                }
            }

        } else {

            if (Context->CurrentMark == 0) {

                ntStatus = STATUS_END_OF_MEDIA;

            } else {

                --Context->CurrentMark;

                if (Context->MarkArray.MarkEntry[Context->CurrentMark].Type ==
                    Type) {

                    ++Number;

                }

            }

        }

    }

    if (NT_SUCCESS( ntStatus )) {

        if (Context->CurrentMark >= Context->MarkArray.TotalMarks) {

            ntStatus = STATUS_END_OF_MEDIA;

        } else {

            //
            // Seek to proper location.  Note:  forward seek
            // seeks to block after file mark (successive read will return
            // block after filemark.
            // A backward seek will position at the filemark.  The next read
            // will return filemark found,  and successive reads will read
            // data after the mark)
            //
            ntStatus = q117SeekToOffset(
                Context->MarkArray.MarkEntry[Context->CurrentMark].Offset+
                    (forwardSeek?BLOCK_SIZE:0),
                Context,
                DeviceObject
                );

        }

    }

    return ntStatus;
}
#endif

NTSTATUS
q117SeekToOffset(
    ULONG Offset,
    PQ117_CONTEXT Context,
    IN PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    Seek to specified offset on the tape (absolute offset from 0) in bytes

Arguments:

    Offset - Bytes from begining of the volume to seek.

Return Value:

    NT Status

--*/

{
    NTSTATUS ntStatus;


    CheckedDump(QIC117SHOWTD,("Absolute seek: %x\n",Offset));

    ntStatus = STATUS_SUCCESS;

    //
    // If not in read mode,  switch into read mode
    //
    if (Context->CurrentOperation.Type == NoOperation) {

        ntStatus = q117OpenForRead(0, Context, DeviceObject);

        //
        // if there is no data on the tape
        //
        if (ntStatus == STATUS_NO_DATA_DETECTED) {
            return ntStatus;
        }

        Context->CurrentOperation.Type = RestoreInProgress;
    }


    if (Offset < Context->CurrentOperation.BytesRead) {

        //
        // Backward seek,  so stop current operation,
        //  rewind to begining of volume,  and drop
        //  through to a forward seek.
        //

        ntStatus = q117ConvertStatus(
            DeviceObject,
            q117EndReadOperation(Context)
            );

        if (NT_SUCCESS(ntStatus)) {

            ntStatus = q117OpenForRead(0, Context, DeviceObject);

            //
            // if there is no data on the tape
            //
            if (ntStatus == STATUS_NO_DATA_DETECTED) {
                return ntStatus;
            }

            Context->CurrentOperation.Type = RestoreInProgress;
        }
    }


    if (NT_SUCCESS(ntStatus)) {
        //
        // Forward seek only (if we were doing a backward seek,  the operation
        // has been re-started this point and BytesRead == 0)
        //
        Offset -= Context->CurrentOperation.BytesRead;

        //
        // Skip to the appropriate place
        //
        ntStatus = q117ConvertStatus(
                        DeviceObject,
                        q117SkipBlock(&Offset, Context)
                        );

    }

    return ntStatus;
}

NTSTATUS
q117IoCtlErase (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT             context;
    NTSTATUS ntStatus;
    STATUS status;

    context = DeviceObject->DeviceExtension;

    //
    // Complete any operation in progress
    //
    switch(context->CurrentOperation.Type) {

        case BackupInProgress:
            status = q117EndWriteOperation(context);
            break;

        case RestoreInProgress:
            status = q117EndReadOperation(context);
            break;
    }

    //
    // Make sure there is a tape in the drive and that the tape information
    // has been loaded
    //
    ntStatus = q117ConvertStatus(
        DeviceObject,
        q117CheckNewTape(context));

    if ( NT_SUCCESS( ntStatus ) ) {
        //
        // Don't allow an erase if write protected
        //
        if (context->CurrentTape.MediaInfo->WriteProtected) {
            return STATUS_MEDIA_WRITE_PROTECTED;
        }

        //
        // Erase the tape
        //
        status = q117EraseQ(context);

        ntStatus = q117ConvertStatus(DeviceObject, status);
    }

    return ntStatus;
}

NTSTATUS
q117IoCtlPrepare (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT       context;
    NTSTATUS            ntStatus;
    STATUS              status;
    PTAPE_PREPARE       tapePrepare;
    IO_REQUEST          ioreq;
    QIC40_VENDOR_UNIQUE vendorUnique;
    LONG                numberBad;

    context = DeviceObject->DeviceExtension;

    status = NoErr;

    //
    // Complete any operation in progress
    //
    switch(context->CurrentOperation.Type) {

        case BackupInProgress:
            status = q117EndWriteOperation(context);
            break;

        case RestoreInProgress:
            status = q117EndReadOperation(context);
            break;
    }

    //
    // All prepare except LOCK and UNLOCK operations rewind the media.
    //
    tapePrepare = Irp->AssociatedIrp.SystemBuffer;

    if ((tapePrepare->Operation != TAPE_LOCK) &&
        (tapePrepare->Operation != TAPE_UNLOCK)) {
        context->CurrentOperation.BytesRead = 0;
        context->CurrentOperation.Position = 0;
#ifndef NO_MARKS
        context->CurrentMark = 0;
#endif
    }


    if (status) {
        return q117ConvertStatus(DeviceObject, status);
    }

    ntStatus = STATUS_SUCCESS;

    switch (tapePrepare->Operation) {

        case TAPE_LOAD:
            CheckedDump(QIC117SHOWTD,("TAPE_LOAD  ... "));
            ntStatus = q117ConvertStatus(DeviceObject, q117CheckNewTape(context));
            break;

        case TAPE_UNLOAD:

            //
            // Just rewind the tape
            //
            CheckedDump(QIC117SHOWTD,("TAPE_UNLOAD  ... "));
            ntStatus = q117ConvertStatus (
                    DeviceObject,
                    q117DoCmd(&ioreq, DEject, NULL, context) );

            break;

        case TAPE_TENSION:

            CheckedDump(QIC117SHOWTD,("TAPE_TENSION  ... "));
            ntStatus = q117ConvertStatus (
                    DeviceObject,
                    q117DoCmd(&ioreq, DReten, NULL, context) );

            break;

        case TAPE_UNLOCK:
            CheckedDump(QIC117SHOWTD,("UN"));
        case TAPE_LOCK:
            CheckedDump(QIC117SHOWTD,("LOCK TAPE ..."));

            //
            // These commands mean nothing for a QIC-40 drive.  However,  the current
            // operation will be flushed allowing user to remove cartridge.
            //
            break;

        case TAPE_FORMAT:
            CheckedDump(QIC117SHOWTD,("TAPE_FORMAT  ... "));

            ntStatus = q117ConvertStatus (
                        DeviceObject,
                        q117Format(
                            &numberBad,
                            TRUE,
                            &vendorUnique,
                            context ) );
            break;

        default:
            CheckedDump(QIC117SHOWTD,("INVALID  ... "));

            ntStatus = STATUS_INVALID_DEVICE_REQUEST;

    } // end switch (tapePrepare->Operation)

    return ntStatus;
}

NTSTATUS
q117IoCtlGetStatus (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT             context;
//    PTAPE_STATUS tapeStatus;
    STATUS status;

    context = DeviceObject->DeviceExtension;


    //
    // Complete any operation in progress
    //
    switch(context->CurrentOperation.Type) {

        case BackupInProgress:
            status = NoErr;
            break;

        case RestoreInProgress:
            status = NoErr;
            break;

        case NoOperation:
            status = q117CheckNewTape(context);
            break;

        default:
            status = FCodeErr;

    }

    //tapeStatus = Irp->UserBuffer;

    //
    // Is this supported in the tape API ?????
    //
//    *tapeStatus = context->TapeStatus;

    //
    // Reset media changed flag.
    //
//    context->TapeStatus.Status &= ~TAPE_STATUS_MEDIA_CHANGED;

    return q117ConvertStatus(DeviceObject, status);
}

NTSTATUS
q117IoCtlReadAbs (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT   context;
    PSEGMENT_BUFFER bufferInfo;
    PVOID           scrbuf;
    PIO_REQUEST     ioreq;
    PCMS_RW_ABS     readWrite;
    STATUS          status;
    ULONG           len;
    PIO_STACK_LOCATION  irpStack;

    context = DeviceObject->DeviceExtension;
    readWrite = Irp->AssociatedIrp.SystemBuffer;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    scrbuf = q117GetFreeBuffer(&bufferInfo,context);

    status=q117IssIOReq(
                scrbuf,
                DRead,
                readWrite->Block,
                bufferInfo,
                context
                );

    if (!status) {

        //
        // Wait for data to be written
        //
        ioreq=q117Dequeue(WaitForItem,context);

        status = ioreq->Status;

    }

    readWrite->Status = status;

    len = BYTES_PER_SECTOR*readWrite->Count;

    if (len > irpStack->Parameters.DeviceIoControl.OutputBufferLength) {

        len = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    }

    RtlMoveMemory(
        readWrite+1,
        scrbuf,
        len
        );

    Irp->IoStatus.Information = sizeof(CMS_RW_ABS)+len;

    return q117ConvertStatus(DeviceObject, status);


}

NTSTATUS
q117IoCtlWriteAbs (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:



Arguments:

    DeviceObject


Return Value:

    NT Status

--*/

{
    PQ117_CONTEXT   context;
    PSEGMENT_BUFFER bufferInfo;
    PVOID           scrbuf;
    PIO_REQUEST     ioreq;
    PCMS_RW_ABS     readWrite;
    STATUS          status;

    context = DeviceObject->DeviceExtension;
    readWrite = Irp->AssociatedIrp.SystemBuffer;

    scrbuf = q117GetFreeBuffer(&bufferInfo,context);

    RtlMoveMemory(
        scrbuf,
        readWrite+1,
        BYTES_PER_SECTOR*readWrite->Count
        );

    status = q117IssIOReq(
        scrbuf,
        DWrite,
        readWrite->Block,
        bufferInfo,
        context);

    if (!status) {

        //
        // Wait for data to be written
        //
        ioreq=q117Dequeue(WaitForItem,context);

        status = ioreq->Status;

    }

    readWrite->Status = status;

    return q117ConvertStatus(DeviceObject, status);

}

STATUS
q117CheckNewTape (
    PQ117_CONTEXT             Context
    )
/*++

Routine Description:

    This routine checks for new tape and reads header if necessary

Arguments:

    Context - Current context information

Return Value:

    NT Status

--*/

{
    STATUS              stat;
    IO_REQUEST          ioreq;
    PTAPE_HEADER        header;
    VOLUME_TABLE_ENTRY  tempVolume;
    UCHAR               tapeType;
    BOOLEAN             found;
    BOOLEAN             notNt;
    USHORT              volumesRead;
    DRIVE_CAPACITY      driveCapacity;
    SEGMENT             curseg;


    //
    // Check to see if there is a tape in the drive
    //
    stat = q117DoCmd(&ioreq, DGetCart, &tapeType, Context);

    //
    // If we found a tape and need to read the header and volume tables,
    //   do it now.
    //
    if (stat == NewCart) {
//        Context->TapeStatus.Status |= TAPE_STATUS_MEDIA_CHANGED;
    }

    if (stat == NewCart ||
            (stat == NoErr && Context->CurrentTape.State == NeedInfoLoaded) ) {

        CheckedDump(QIC117SHOWTD,("New Cart Detected\n"));

        //
        // Check to see if there is a tape in the drive
        //

        stat = q117DoCmd(&ioreq, DClearNewCart, NULL, Context);

        if (stat) {
            return stat;
        }

        //
        // Saw new cart,  so set need loaded flag
        //
        Context->CurrentTape.State = NeedInfoLoaded;

        // clear no media flag and end of media flag
//        Context->TapeStatus.Status &=
//            ~(TAPE_STATUS_NO_MEDIA|TAPE_STATUS_END_OF_MEDIA);

        // Set bom and device ready.
//        Context->TapeStatus.Status |=
//            TAPE_STATUS_DEVICE_READY|TAPE_STATUS_BEGINNING_OF_MEDIA;

        //
        // Check to see if tape is write protected
        //
        if (stat = q117DoCmd(&ioreq, DSndWPro, NULL, Context)) {

            if (stat != WProt) {
                return stat;
            }

        }

        Context->CurrentTape.MediaInfo->WriteProtected = (stat == WProt);
        if (stat == WProt) {
//            Context->TapeStatus.Status |= TAPE_STATUS_WRITE_PROTECTED;
        }

        //
        // Check to see if drive is formatted
        //
        stat = q117DoCmd(&ioreq, DGetCap, &driveCapacity, Context);

        if (stat) {
            return stat;
        }

        //
        // If tape not referenced.
        //
        if (driveCapacity.referenced == FALSE) {
            return BadFmt;
        }

        //
        // Check to see if tape is correct format
        //
        stat = q117DoCmd(&ioreq, DChkFmt, NULL, Context);

        if (stat) {
            if (stat == WrongFmt) {
                Context->CurrentTape.MediaInfo->WriteProtected =
                    TRUE;
                stat = NoErr;
            } else {
                return stat;
            }
        }

        //
        // Now read the tape header (and bad sector map)
        //
        if (stat = q117LoadTape(&header,Context)) {
            return stat;
        }
        CheckedDump(QIC117SHOWTD,("LoadTape successful\n"));

        //
        // If this capacity not supported by this drive,
        // (i.e..  Pegasus cart in a QIC-40 drive)
        // return invalid format
        //
        if (header->FormatCode != driveCapacity.TapeFormatCode) {
            return BadFmt;
        }

        //
        // Copy over bad sector map, etc.
        //
        RtlMoveMemory(
            Context->CurrentTape.TapeHeader,
            header,
            sizeof(*Context->CurrentTape.TapeHeader) );

        //
        // Now scan volume list and get last volume on tape as well as the
        // NT volume (if one exists
        //

        if (stat = q117SelectTD(Context))
            return stat;

        volumesRead = 0;

        found = FALSE;

        do {
            /* get a volume directory from the tape (if error then)*/
            stat = q117ReadVolumeEntry(&tempVolume,Context);

            if (stat && (stat != EndOfVol))
                return stat;

            if (!stat) {
                volumesRead++;

                //
                // For now,  let the system find ANY volume type and select
                // it.  This will allow NT Backup software to read the
                // volume and detect that it is a non-nt format.
                // To do this,  the if statement is removed,  allowing
                // the first volume found,  be the one that we select.
                //
                if (!(tempVolume.VendorSpecific &&
                    tempVolume.Vendor.cms_QIC40.OpSysType == OP_WINDOWS_NT)) {

                    Context->CurrentOperation.BytesOnTape =
                        Context->CurrentOperation.BytesRead = 0;

                    notNt = TRUE;

#ifndef NO_MARKS
                    Context->MarkArray.TotalMarks = 0;
                    Context->CurrentMark = Context->MarkArray.TotalMarks;
                    Context->MarkArray.MarkEntry[Context->CurrentMark].Offset =
                        0xffffffff;
#endif

                    /* force the data size to be the entire backup */
                    tempVolume.DataSize = 0;
                    curseg = tempVolume.StartSegment;
                    while (curseg <= tempVolume.EndingSegment) {

                        tempVolume.DataSize +=
                            q117GoodDataBytes(
                                curseg,
                                Context);
                        ++curseg;


                    }

                } else {

                    notNt = FALSE;

                }
                found = TRUE;
                Context->ActiveVolume = tempVolume;
            }

        } while (!stat && volumesRead < Context->CurrentTape.MaximumVolumes);

        if (stat = q117EndRest(Context))
            return(stat);

        //
        // If we did not find a volume,  then signal others that
        //  the ActiveVolume information is invalid.
        //
        if (!found)
            Context->ActiveVolumeNumber = 0;

        //
        // Zero out bytes saved (incase user trys to read)
        // Also set CurrentOperation.BytesRead to zero (start of tape)
        //
        Context->CurrentOperation.BytesOnTape =
            Context->CurrentOperation.BytesRead = 0;

        //
        // Flag that we have done everything
        //
        Context->CurrentTape.State = TapeInfoLoaded;

#ifndef NO_MARKS

        //
        // Read the mark list from the active volume
        //
        if (found && !notNt) {

            stat = q117GetMarks(Context);

        }

#endif

        //
        // If no more pressing error,  return NewCart
        // so application can be aware of the new insertion.
        //
        if (stat == NoErr) {
            stat = NewCart;
        }

    } else {

        if (stat == NoTape) {
//            Context->TapeStatus.Status |= TAPE_STATUS_NO_MEDIA;
            Context->ActiveVolumeNumber = 0;
            Context->CurrentOperation.BytesOnTape =
                Context->CurrentOperation.BytesRead = 0;

#ifndef NO_MARKS
            Context->MarkArray.TotalMarks = 0;
            Context->CurrentMark = Context->MarkArray.TotalMarks;
            Context->MarkArray.MarkEntry[Context->CurrentMark].Offset =
                0xffffffff;
#endif

        }

    }

    return stat;
}

