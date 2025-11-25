/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    mtaccess.c

Abstract:

    interface functions to lower level driver.

Revision History:




--*/

//
// include files
//

#include <ntddk.h>
#include <ntddtape.h>
#include "common.h"
#include "q117.h"
#include "protos.h"

#define PAGES_TO_SECTORS(pages)  (((pages)*PAGE_SIZE)/BYTES_PER_SECTOR)

#define MIN(a,b) ((a)>(b)?(b):(a))

STATUS
q117ReqIO(
    IN PIO_REQUEST IoRequest,
    IN PSEGMENT_BUFFER BufferInfo,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Form an IRP to send to the lower level driver,  and send it.
    For MIPS,  allocate sub-requests and break the request into
    smaller chunks to allow for the 4K DMA limitation of the MIPS
    box.


Arguments:

    IoRequest - Original request

    BufferInfo - If a DMA is involved,  this contains information about the
                associated memory for DMA.

    Context - Context of the driver.

Return Value:

--*/

{
    PIO_STACK_LOCATION irpStack;
    PIRP irp;
#ifdef BUFFER_SPLIT
    BOOLEAN secondary = FALSE;
    PVOID address;
    PIO_REQUEST subRequest;
    IO_REQUEST requestCopy;
    PKEVENT pevent;
    ULONG mask;
    ULONG numberOfPagesInTransfer;
    BOOLEAN needToSplit;
    ULONG sectorsToTransfer;
    ULONG maxSplits;
#endif


    IoRequest->BufferInfo = BufferInfo;

#ifdef BUFFER_SPLIT

    needToSplit = FALSE;

    if (BufferInfo) {
        numberOfPagesInTransfer = ADDRESS_AND_SIZE_TO_SPAN_PAGES(
            IoRequest->Data,
            IoRequest->Number * BYTES_PER_SECTOR );

        if ( numberOfPagesInTransfer >
                Context->AdapterInfo->NumberOfMapRegisters ) {

            needToSplit = TRUE;
            maxSplits =
                (numberOfPagesInTransfer /
                Context->AdapterInfo->NumberOfMapRegisters) + 1;

        }
    }

#endif


#if DBG
    if (BufferInfo) {
        //
        // Check to make sure the buffer information is for the data pointer
        // we recieved.
        // If not,  then there is a real problem with the calling function.
        // We need to check this so we don't page fault the system when a bug
        // occurs.
        //
        if (BufferInfo->logical < IoRequest->Data ||
            ((PUCHAR)(IoRequest->Data) + (IoRequest->Number * BYTES_PER_SECTOR)) >
            ((PUCHAR)(BufferInfo->logical) + BYTES_PER_SEGMENT) ) {

            CheckedDump(QIC117DBGP,("Buffer pointer out of range\n"));

            return FCodeErr;

        }
    }
#endif


#ifdef BUFFER_SPLIT

//
// For MIPS we must split the request into 4K DMA requests because
// the MIPS will not transfer more than 4K at a time.
//

    if (BufferInfo && IoRequest->Command != DFmt && needToSplit) {

        address = IoRequest->Data;

        //
        // Split request into chunks
        //
        subRequest = ExAllocatePool(NonPagedPool,
                         sizeof(*IoRequest)*maxSplits);

        CheckedDump(QIC117INFO,("q117ReqIO: Allocating %d subreqs\n", maxSplits));

        IoRequest->Next = subRequest;
        IoRequest->Status = SplitRequests;

        requestCopy = *IoRequest;

#if DBG
        CheckedDump(QIC117SHOWTD,("*************************", requestCopy.Block ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Block     %x\n", requestCopy.Block ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Number    %x\n", requestCopy.Number ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Data      %x\n", requestCopy.Data ));
        CheckedDump(QIC117SHOWTD,("IoRequest->BadList   %x\n", requestCopy.BadList ));
        CheckedDump(QIC117SHOWTD,("IoRequest->RetryList %x\n", requestCopy.RetryList ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Command   %x\n\n", requestCopy.Command ));
#endif

        while (requestCopy.Number) {

            //
            // Make a copy of the current request
            //
            *subRequest = requestCopy;

            //
            // there aren't enough map registers to handle the whole
            // transfer so cap the transfer at the number of map registers
            // we have
            //

            sectorsToTransfer =
                PAGES_TO_SECTORS(Context->AdapterInfo->NumberOfMapRegisters);

            //
            // Perform the lesser of the two for this request
            //
            subRequest->Number = (UCHAR)
                MIN(sectorsToTransfer, (ULONG)subRequest->Number);

            CheckedDump(QIC117SHOWTD,("Split at %d sectors\n", subRequest->Number));

            //
            // Adjust the current request to be the remainder of request
            //
            (PCHAR)requestCopy.Data += subRequest->Number * BYTES_PER_SECTOR;
            requestCopy.Block += subRequest->Number;
            requestCopy.Number -= subRequest->Number;
            requestCopy.BadList >>= subRequest->Number;
            requestCopy.RetryList >>= subRequest->Number;

            mask = ~(0xffffffff << subRequest->Number);
            requestCopy.BadList &=  mask;
            requestCopy.RetryList &= mask;


            //
            // use IoRequest.DoneEvent on last request so that all requests
            //  up to the last one are done before event is set
            //
            pevent = requestCopy.Number ?
                &subRequest->DoneEvent : &IoRequest->DoneEvent;

            KeInitializeEvent(
                pevent,
                NotificationEvent,
                FALSE);


            irp = IoBuildDeviceIoControlRequest(
                    IOCTL_QIC117_DRIVE_REQUEST,
                    Context->q117iDeviceObject,
                    NULL,
                    0,
                    NULL,
                    0,
                    TRUE,
                    pevent,
                    &subRequest->IoStatus
                );


            if (irp == NULL) {

                CheckedDump(QIC117DBGP,("q117ReqIO: Can't allocate Irp\n"));

                //
                // If an Irp can't be allocated, then this call will
                // simply return. This will leave the queue frozen for
                // this device, which means it can no longer be accessed.
                //

                return FCodeErr;
            }



            //
            // Build mdl
            //
            irp->MdlAddress = IoAllocateMdl(
                    address,
                    subRequest->Number * BYTES_PER_SECTOR,
                    secondary,
                    FALSE,  // no charge of quota
                    NULL    // no irp
                );

            (PCHAR)address += subRequest->Number * BYTES_PER_SECTOR;
            secondary = TRUE;

            if (irp->MdlAddress == NULL) {

                CheckedDump(QIC117DBGP,("q117ReqIO: Can't allocate MDL\n"));

                //
                // If a MDL can't be allocated, then this call will
                // simply return. This will leave the queue frozen for
                // this device, which means it can no longer be accessed.
                //

                IoFreeIrp(irp);

                return FCodeErr;
            }

            MmBuildMdlForNonPagedPool(irp->MdlAddress);

            //
            // Get Q117I's stack location and store IoRequest for it's use
            //

            irpStack = IoGetNextIrpStackLocation(irp);
            irpStack->Parameters.DeviceIoControl.Type3InputBuffer = subRequest;

#if DBG
            CheckedDump(QIC117SHOWTD,("subRequest->Block     %x\n", subRequest->Block ));
            CheckedDump(QIC117SHOWTD,("subRequest->Number    %x\n", subRequest->Number ));
            CheckedDump(QIC117SHOWTD,("subRequest->Data      %x\n", subRequest->Data ));
            CheckedDump(QIC117SHOWTD,("subRequest->BadList   %x\n", subRequest->BadList ));
            CheckedDump(QIC117SHOWTD,("subRequest->RetryList %x\n", subRequest->RetryList ));
            CheckedDump(QIC117SHOWTD,("subRequest->Command   %x\n\n", subRequest->Command ));
#endif
            (VOID)IoCallDriver(Context->q117iDeviceObject, irp);

            CheckedDump(QIC117INFO,("q117ReqIO: Sending subreq\n"));

            //
            // point to the next sub-request to make
            //
            ++subRequest;
        }

    } else {

#endif // BUFFER_SPLIT

    IoRequest->Status = InQue;

    KeInitializeEvent(
        &IoRequest->DoneEvent,
        NotificationEvent,
        FALSE);

    irp = IoBuildDeviceIoControlRequest(
            IOCTL_QIC117_DRIVE_REQUEST,
            Context->q117iDeviceObject,
            NULL,
            0,
            NULL,
            0,
            TRUE,
            &IoRequest->DoneEvent,
            &IoRequest->IoStatus
        );


    if (irp == NULL) {

        CheckedDump(QIC117DBGP,("q117ReqIO: Can't allocate Irp\n"));

        //
        // If an Irp can't be allocated, then this call will
        // simply return. This will leave the queue frozen for
        // this device, which means it can no longer be accessed.
        //

        return FCodeErr;
    }



    //
    // If we have buffer information
    //
    if (BufferInfo) {

        irp->MdlAddress = IoAllocateMdl(
                IoRequest->Data,
                IoRequest->Number * BYTES_PER_SECTOR,
                FALSE,  // not a secondary buffer
                FALSE,  // no charge of quota
                NULL    // no irp
            );

        if (irp->MdlAddress == NULL) {

            CheckedDump(QIC117DBGP,("q117ReqIO: Can't allocate MDL\n"));

            //
            // If a MDL can't be allocated, then this call will
            // simply return. This will leave the queue frozen for
            // this device, which means it can no longer be accessed.
            //

            IoFreeIrp(irp);

            return FCodeErr;
        }
        MmBuildMdlForNonPagedPool(irp->MdlAddress);
    }



    //
    // Get Q117I's stack location and store IoRequest for it's use
    //

    irpStack = IoGetNextIrpStackLocation(irp);
    irpStack->Parameters.DeviceIoControl.Type3InputBuffer = IoRequest;

    IoCallDriver(Context->q117iDeviceObject, irp);

#ifdef BUFFER_SPLIT
    }
#endif // BUFFER_SPLIT

    return NoErr;

}

STATUS
q117WaitIO(
    IN PIO_REQUEST IoRequest,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Wait for an I/O request to complete.  If a MIPS machine,
    coalesce the sub-requests into the original request.

Arguments:

   IoRequest -

Return Value:

--*/

{
    CheckedDump(QIC117INFO,("WaitIO(%x,%x,%x) ... ", IoRequest->Command,IoRequest->Block,IoRequest->Status));

    KeWaitForSingleObject(
        &IoRequest->DoneEvent,
        Suspended,
        KernelMode,
        FALSE,
        NULL);

#ifdef BUFFER_SPLIT

    if (IoRequest->Status == SplitRequests) {
        PIO_REQUEST subRequest;
        ULONG blocksLeft,mask,slot;

        CheckedDump(QIC117INFO,("Got split request\n"));

        subRequest = IoRequest->Next;
        blocksLeft = IoRequest->Number;

        //
        // Zero accumulating fields
        //

        IoRequest->BadList = IoRequest->RetryList = 0;
        IoRequest->Status = NoErr;

        //
        // Loop through all sub-requests and build the resultant request
        //
        while (blocksLeft) {

#if DBG
        CheckedDump(QIC117SHOWTD,("subRequest(%x)\n",subRequest));
        CheckedDump(QIC117SHOWTD,("subRequest->Block     %x\n", subRequest->Block ));
        CheckedDump(QIC117SHOWTD,("subRequest->Number    %x\n", subRequest->Number ));
        CheckedDump(QIC117SHOWTD,("subRequest->Data      %x\n", subRequest->Data ));
        CheckedDump(QIC117SHOWTD,("subRequest->BadList   %x\n", subRequest->BadList ));
        CheckedDump(QIC117SHOWTD,("subRequest->RetryList %x\n", subRequest->RetryList ));
        CheckedDump(QIC117SHOWTD,("subRequest->Command   %x\n", subRequest->Command ));
        CheckedDump(QIC117SHOWTD,("subRequest->Status    %x\n", subRequest->Status ));
#endif

            //
            // Create mask and calculate bit shift (slot) for each
            // sub-request
            //
            mask = ~(0xffffffff << subRequest->Number);
            slot = subRequest->Block - IoRequest->Block;

            CheckedDump(QIC117SHOWTD,("mask=%08lx slot=%x\n",mask,slot));

            //
            // Coalesce the bad sector and retry bitfields
            //
            IoRequest->BadList |= (subRequest->BadList & mask) << slot;
            IoRequest->RetryList |= (subRequest->RetryList & mask) << slot;

            if (subRequest->Status == NewCart) {
                //
                // Saw new cart,  so set need loaded flag
                //
                Context->CurrentTape.State = NeedInfoLoaded;
                CheckedDump(QIC117SHOWTD,("New Cart Detected\n"));

            }

            //
            // Ignore BadBlk errors (but report them)
            //
            if (subRequest->Status == BadBlk) {

                IoRequest->Status = subRequest->Status;
                subRequest->Status = NoErr;

            }

            if (subRequest->Status != NoErr) {

                blocksLeft = 0;
                IoRequest->Status = subRequest->Status;

            } else {

                blocksLeft -= subRequest->Number;

            }

            //
            // point to the next sub-request to process
            //
            ++subRequest;
        }

        //
        // Free the sub-requests for this I/O request
        //
        ExFreePool(IoRequest->Next);

#if DBG
        CheckedDump(QIC117SHOWTD,("IoRequest->Status    %x\n", IoRequest->Status ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Block     %x\n", IoRequest->Block ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Number    %x\n", IoRequest->Number ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Data      %x\n", IoRequest->Data ));
        CheckedDump(QIC117SHOWTD,("IoRequest->BadList   %x\n", IoRequest->BadList ));
        CheckedDump(QIC117SHOWTD,("IoRequest->RetryList %x\n", IoRequest->RetryList ));
        CheckedDump(QIC117SHOWTD,("IoRequest->Command   %x\n", IoRequest->Command ));
#endif
    }

    if (IoRequest->Status == NewCart) {
        //
        // Saw new cart,  so set need loaded flag
        //
        Context->CurrentTape.State = NeedInfoLoaded;
        CheckedDump(QIC117SHOWTD,("New Cart Detected\n"));

    }

#endif // BUFFER_SPLIT

#if DBG
    if (IoRequest->Status == BadBlk) {
        CheckedDump(QIC117INFO,("bbm: %x ", IoRequest->BadList));
    }
#endif
    CheckedDump(QIC117INFO,("waitio status %x\n", IoRequest->Status));

    return NoErr;
}

STATUS
q117DoIO(
    IN PIO_REQUEST IoRequest,
    IN PSEGMENT_BUFFER BufferInfo,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:


Arguments:

    IoRequest -

    BufferInfo -

    Context -

Return Value:

--*/

{
    STATUS ret;

    CheckedDump(QIC117INFO,("DoIO called (%x)\n", IoRequest->Command));

    ret = q117ReqIO(IoRequest, BufferInfo, Context);
    if (!ret) {
        ret = q117WaitIO(IoRequest, Context);
    }
    return ret;
}

STATUS
q117AbortIo(
    IN PQ117_CONTEXT Context,
    IN PKEVENT DoneEvent,
    IN PIO_STATUS_BLOCK IoStatus
    )

/*++

Routine Description:


Arguments:

    Context -

    DoneEvent -

    IoStatus -

Return Value:

--*/

{
    PIRP irp;


    CheckedDump(QIC117INFO,("ClearIO Called\n"));

    KeInitializeEvent(
        DoneEvent,
        NotificationEvent,
        FALSE);

    irp = IoBuildDeviceIoControlRequest(
            IOCTL_QIC117_CLEAR_QUEUE,
            Context->q117iDeviceObject,
            NULL,
            0,
            NULL,
            0,
            TRUE,
            DoneEvent,
            IoStatus
        );


    if (irp == NULL) {

        CheckedDump(QIC117DBGP,("q117ClearIO: Can't allocate Irp\n"));

        //
        // If an Irp can't be allocated, then this call will
        // simply return. This will leave the queue frozen for
        // this device, which means it can no longer be accessed.
        //

        return FCodeErr;
    }


    (VOID)IoCallDriver(Context->q117iDeviceObject, irp);

    return NoErr;
}

STATUS
q117AbortIoDone(
    IN PQ117_CONTEXT Context,
    IN PKEVENT DoneEvent
    )

/*++

Routine Description:


Arguments:

    Context -

    DoneEvent -

Return Value:

--*/

{

    //
    // wait for the driver to complete request
    //

    KeWaitForSingleObject(
        DoneEvent,
        Suspended,

        KernelMode,
        FALSE,
        NULL);

    return NoErr;
}
