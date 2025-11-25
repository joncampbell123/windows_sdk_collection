/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

   protos.h

Abstract:

   Prototypes for internal functions of the High-Level portion (data
   formatter) of the QIC-117 device driver.

Revision History:


--*/

STATUS
q117Format(
   OUT LONG *NumberBad,
   IN UCHAR DoFormat,
   IN PQIC40_VENDOR_UNIQUE VendorUnique,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117ReqIO(
   IN PIO_REQUEST IoRequest,
   IN PSEGMENT_BUFFER BufferInfo,
   IN PQ117_CONTEXT Context
   );

STATUS
q117WaitIO(
   IN PIO_REQUEST IoRequest,
   IN PQ117_CONTEXT Context
   );

STATUS
q117DoIO(
   IN PIO_REQUEST IoRequest,
   IN PSEGMENT_BUFFER BufferInfo,
   IN PQ117_CONTEXT Context
   );

STATUS
q117AbortIo(
   IN PQ117_CONTEXT Context,
   IN PKEVENT DoneEvent,
   IN PIO_STATUS_BLOCK IoStatus
   );

STATUS
q117AbortIoDone(
   IN PQ117_CONTEXT Context,
   IN PKEVENT DoneEvent
   );

STATUS
q117CheckDrive(
   IN PQ117_CONTEXT Context
   );

STATUS
q117DoCmd(
   IN OUT PIO_REQUEST IoRequest,
   IN CHAR Command,
   IN PVOID Data,
   IN PQ117_CONTEXT Context
   );

STATUS
q117EndRest(
   IN PQ117_CONTEXT Context
   );

STATUS
q117MapBadBlock (
   IN PIO_REQUEST IoRequest,
   OUT PVOID *DataPointer,
   IN OUT USHORT *BytesLeft,
   IN OUT SEGMENT *CurrentSegment,
   IN OUT USHORT *Remainder,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117NewTrkRC(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117SelectVol(
   IN PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN PQ117_CONTEXT Context
   );

STATUS
q117UpdateHeader(
   IN PTAPE_HEADER Header,
   IN PQ117_CONTEXT Context
   );

STATUS
q117Update(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117DoUpdateBad(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117DoUpdateMarks(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117GetMarks(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117FillTapeBlocks(
   IN OUT DRIVER_COMMAND Command,
   IN SEGMENT CurrentSegment,
   IN SEGMENT EndSegment,
   IN OUT PVOID Buffer,
   IN SEGMENT FirstGood,
   IN SEGMENT SecondGood,
   IN PSEGMENT_BUFFER BufferInfo,
   IN PQ117_CONTEXT Context
   );
STATUS
q117IssIOReq(
   IN OUT PVOID Data,
   IN DRIVER_COMMAND Command,
   IN LONG Block,
   IN OUT PSEGMENT_BUFFER BufferInfo,
   IN OUT PQ117_CONTEXT Context
   );

BOOLEAN
q117QueueFull(
   IN PQ117_CONTEXT Context
   );

BOOLEAN
q117QueueEmpty(
   IN PQ117_CONTEXT Context
   );

PVOID
q117GetFreeBuffer(
   OUT PSEGMENT_BUFFER *BufferInfo,
   IN PQ117_CONTEXT Context
   );

PVOID
q117GetLastBuffer(
   IN PQ117_CONTEXT Context
   );

PIO_REQUEST
q117Dequeue(
   IN DEQUEUE_TYPE Type,
   IN OUT PQ117_CONTEXT Context
   );

VOID
q117ClearQueue(
   IN OUT PQ117_CONTEXT Context
   );

VOID
q117QueueSingle(
   IN OUT PQ117_CONTEXT Context
   );

VOID
q117QueueNormal(
   IN OUT PQ117_CONTEXT Context
   );

PIO_REQUEST
q117GetCurReq(
   IN PQ117_CONTEXT Context
   );

ULONG
q117GetQueueIndex(
   IN PQ117_CONTEXT Context
   );

VOID
q117SetQueueIndex(
   IN ULONG Index,
   OUT PQ117_CONTEXT Context
   );

STATUS
q117LoadTape (
   IN OUT PTAPE_HEADER*HeaderPointer,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117InitFiler (
   IN OUT PQ117_CONTEXT Context
   );

void
q117GetBadSectors (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117ReadHeaderSegment (
   OUT PTAPE_HEADER*HeaderPointer,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117WriteTape(
   IN OUT PVOID FromWhere,
   IN OUT ULONG HowMany,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117EndBack(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117ReadVolumeEntry(
   PVOLUME_TABLE_ENTRY VolumeEntry,
   PQ117_CONTEXT Context
   );

VOID
q117FakeDataSize(
   IN OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN PQ117_CONTEXT Context
   );

STATUS
q117AppVolTD(
   IN OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117SelectTD(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117Start (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117Stop (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117OpenForWrite (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117EndWriteOperation (
   IN OUT PQ117_CONTEXT Context
   );

NTSTATUS
q117OpenForRead (
    IN ULONG StartPosition,
    IN OUT PQ117_CONTEXT Context,
    IN PDEVICE_OBJECT DeviceObject
   );

STATUS
q117EndReadOperation (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117StartBack(
   IN OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN PQ117_CONTEXT Context
   );

STATUS
q117StartAppend(
   IN OUT ULONG BytesAlreadyThere,
   IN PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117StartComm(
   OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117SelVol (
   PVOLUME_TABLE_ENTRY TheVolumeTable,
   PQ117_CONTEXT Context
   );

STATUS
q117ReadTape (
   OUT PVOID ToWhere,
   IN OUT ULONG *HowMany,
   IN OUT PQ117_CONTEXT Context
   );

NTSTATUS
q117ConvertStatus(
   IN PDEVICE_OBJECT DeviceObject,
   IN STATUS status
   );

VOID
q117SetTpSt(
   PQ117_CONTEXT Context
   );

STATUS
q117GetEndBlock (
   OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
   OUT LONG *NumberVolumes,
   IN PQ117_CONTEXT Context
   );

STATUS
q117BuildHeader(
   OUT PQIC40_VENDOR_UNIQUE VendorUnique,
   IN SEGMENT *HeaderSect,
   IN OUT PTAPE_HEADER Header,
   IN PQ117_CONTEXT Context
   );

NTSTATUS
q117IoCtlGetMediaParameters (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlSetMediaParameters (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlGetDriveParameters (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlSetDriveParameters (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlWriteMarks (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlSetPosition (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117FindMark(
   ULONG Type,
   LONG Number,
   PQ117_CONTEXT Context,
   IN PDEVICE_OBJECT DeviceObject
   );

NTSTATUS
q117SeekToOffset(
   ULONG Offset,
   PQ117_CONTEXT Context,
   IN PDEVICE_OBJECT DeviceObject
   );

NTSTATUS
q117IoCtlErase (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlPrepare (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlGetStatus (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlGetPosition (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

STATUS
q117CheckNewTape (
   PQ117_CONTEXT             Context
   );

STATUS
q117NewTrkBk(
   PQ117_CONTEXT Context
   );

STATUS
q117GetTapeCapacity(
   struct S_O_DGetCap *ptr,
   PQ117_CONTEXT Context
   );

VOID
q117RdsInitReed (
   VOID
   );

UCHAR
q117RdsMultiplyTuples (
   IN UCHAR tup1,
   IN UCHAR tup2
   );

UCHAR
q117RdsDivideTuples (
   IN UCHAR tup1,
   IN UCHAR tup2
   );

UCHAR
q117RdsExpTuple (
   IN UCHAR tup1,
   IN UCHAR xpnt
   );

VOID
q117RdsMakeCRC (
   IN OUT UCHAR *Array,      // pointer to 32K data area (segment)
   IN UCHAR Count            // number of sectors (1K blocks)(1-32)
   );

BOOLEAN
q117RdsReadCheck (
   IN UCHAR *Array,         // pointer to 32K data area (segment)
   IN UCHAR Count           // number of sectors (1K blocks)(1-32)
   );

BOOLEAN
q117RdsCorrect(
   IN OUT UCHAR *Array,    // pointer to 32K data area (segment)
   IN UCHAR Count,         // number of good sectors in segment (4-32)
   IN UCHAR CRCErrors,     // number of crc errors
   IN UCHAR e1,
   IN UCHAR e2,
   IN UCHAR e3             // sectors where errors occurred
   );

VOID
q117RdsGetSyndromes (
   IN OUT UCHAR *Array,       // pointer to 32K data area (segment)
   IN UCHAR Count,            // number of good sectors in segment (4-32)
   IN UCHAR *ps1,
   IN UCHAR *ps2,
   IN UCHAR *ps3
   );

BOOLEAN
q117RdsCorrectFailure (
   IN OUT UCHAR *Array,     // pointer to 32K data area (segment)
   IN UCHAR Count,          // number of good sectors in segment (4-32)
   IN UCHAR s1,
   IN UCHAR s2,
   IN UCHAR s3
   );

BOOLEAN
q117RdsCorrectOneError (
   IN OUT UCHAR *Array,      // pointer to 32K data area (segment)
   IN UCHAR Count,           // number of good sectors in segment (4-32)
   IN UCHAR ErrorLocation,
   IN UCHAR s1,
   IN UCHAR s2,
   IN UCHAR s3
   );

BOOLEAN
q117RdsCorrectTwoErrors (
   IN OUT UCHAR *Array,       // pointer to 32K data area (segment)
   IN UCHAR Count,            // number of good sectors in segment (4-32)
   IN UCHAR ErrorLocation1,
   IN UCHAR ErrorLocation2,
   IN UCHAR s1,
   IN UCHAR s2,
   IN UCHAR s3
   );

BOOLEAN
q117RdsCorrectThreeErrors (
   IN OUT UCHAR *Array,       // pointer to 32K data area (segment)
   IN UCHAR Count,            // number of good sectors in segment (4-32)
   IN UCHAR ErrorLocation1,
   IN UCHAR ErrorLocation2,
   IN UCHAR ErrorLocation3,
   IN UCHAR s1,
   IN UCHAR s2,
   UCHAR s3
   );

BOOLEAN
q117RdsCorrectOneErrorAndOneFailure (
   IN OUT UCHAR *Array,        // pointer to 32K data area (segment)
   IN UCHAR Count,             // number of good sectors in segment (4-32)
   IN UCHAR ErrorLocation1,
   IN UCHAR s1,
   IN UCHAR s2,
   IN UCHAR s3
   );

void
q117SpacePadString(
   IN OUT CHAR *InputString,
   IN LONG StrSize
   );

STATUS
q117VerifyFormat(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117EraseQ(
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117EraseS(
   IN OUT PQ117_CONTEXT Context
   );

VOID
q117ClearVolume (
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117SkipBlock (
   IN OUT ULONG *HowMany,
   IN OUT PQ117_CONTEXT Context
   );

STATUS
q117ReconstructSegment(
   IN PIO_REQUEST IoReq,
   IN PQ117_CONTEXT Context
   );

STATUS
q117DoCorrect(
   IN PVOID DataBuffer,
   IN ULONG BadSectorMap,
   IN ULONG SectorsInError
   );

UCHAR
q117CountBits(
    IN PQ117_CONTEXT Context,
    IN SEGMENT Segment,
    ULONG Map
    );

ULONG q117ReadBadSectorList (
    IN PQ117_CONTEXT Context,
    IN SEGMENT Segment
    );

USHORT
q117GoodDataBytes(
   IN SEGMENT Segment,
   IN PQ117_CONTEXT Context
   );

NTSTATUS
q117AllocatePermanentMemory(
   PQ117_CONTEXT Context,
   PADAPTER_OBJECT AdapterObject,
   ULONG           NumberOfMapRegisters
   );

STATUS
q117GetTemporaryMemory(
   PQ117_CONTEXT Context
   );

VOID
q117FreeTemporaryMemory(
   PQ117_CONTEXT Context
   );

NTSTATUS
q117IoCtlReadAbs (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

NTSTATUS
q117IoCtlWriteAbs (
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp
   );

STATUS
q117UpdateBadMap(
    IN OUT PQ117_CONTEXT Context,
    IN SEGMENT Segment,
    IN ULONG BadSectors
    );

VOID
q117BadMapToBadList(
    IN SEGMENT Segment,
    IN ULONG BadSectors,
    IN BAD_LIST_PTR BadListPtr
    );

ULONG
q117BadListEntryToSector(
    IN UCHAR *ListEntry
    );
