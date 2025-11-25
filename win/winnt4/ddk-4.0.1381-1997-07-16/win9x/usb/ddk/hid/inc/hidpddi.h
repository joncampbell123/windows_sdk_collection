/*++

Copyright (c) 1996    Microsoft Corporation

Module Name:

    HIDDLL.H

Abstract:

    This module contains the PUBLIC definitions for the
    code that implements the driver side of the parsing library.

Environment:

    Kernel & user mode

Revision History:

    Aug-96 : created by Kenneth Ray

--*/

#ifndef _HIDPDDI_H
#define _HIDPDDI_H

#include "hidpi.h"

typedef struct _HIDP_COLLECTION_DESC
{
   UCHAR       CollectionNumber;
   UCHAR       UsagePage;
   UCHAR       Usage;
   UCHAR       Reserved1; // Must be zero

   USHORT      InputLength;
   USHORT      OutputLength;
   USHORT      FeatureLength;
   USHORT      PreparsedDataLength, Reserved2;

   PHIDP_PREPARSED_DATA             PreparsedData;
} HIDP_COLLECTION_DESC, *PHIDP_COLLECTION_DESC;

typedef struct _HIDP_GETCOLDESC_DBG
{
   ULONG    BreakOffset;
   ULONG    ErrorCode;
   ULONG    Args[3];
} HIDP_GETCOLDESC_DBG, *PHIDP_GETCOLDESC_DBG;

NTSTATUS
HidP_GetCollectionDescription_DBG (
   IN  PHIDP_REPORT_DESCRIPTOR   ReportDesc,
   IN  ULONG                     DescLength,
   IN  POOL_TYPE                 PoolType,
   OUT PHIDP_COLLECTION_DESC *   CollectionDesc,
   OUT PULONG                    CollectionDescLength,
   OUT PHIDP_GETCOLDESC_DBG       Dbg
   );

NTSTATUS
HidP_GetCollectionDescription (
   IN  PHIDP_REPORT_DESCRIPTOR   ReportDesc,
   IN  ULONG                     DescLength,
   IN  POOL_TYPE                 PoolType,
   OUT PHIDP_COLLECTION_DESC *   CollectionDesc,
   OUT PULONG                    CollectionDescLength
   );
/*++
Routine Description:
   Given a RAW report descriptor return a linked list of collection descriptors
   describing the given device.  This data is allocated from NON_PAGED pool.

Arguments:
   ReportDesc            the raw report descriptor.
   DescLength            the length of the report descriptor.
   PoolType              pool type sent to ExAllocatePoolWithTag.
   CollectionDesc        Pointer an array of collection descriptors.
   CollectionDescLength  the length of said array.

Return Value:
·  STATUS_SUCCESS
·  STATUS_COULD_NOT_INTERPRET an error detected in the report descriptor.
·  STATUS_BUFFER_TOO_SMALL if while parsing an item, with additional bytes
                        of information, the library finds itself at the end of
                        the descriptor.
·  STATUS_INVALID_PARAMETER an invalid item is discovered in the report.
·  STATUS_INVALID_PARAMETER_MIX if local and global modifiers improperly
                        match up with a main item.
--*/


#define HIDP_GETCOLDESC_RESOURCES            0x01
// Insufficient resources to allocate needed memory.
#define HIDP_GETCOLDESC_BUFFER               0x02
#define HIDP_GETCOLDESC_LINK_RESOURCES       0x03
#define HIDP_GETCOLDESC_UNEXP_END_COL        0x04
// An extra end collection token was found.
#define HIDP_GETCOLDESC_PREPARSE_RESOURCES   0x05
// Insufficient resources to allocate memory for preparsing.
#define HIDP_GETCOLDESC_ONE_BYTE             0x06
#define HIDP_GETCOLDESC_TWO_BYTE             0x07
#define HIDP_GETCOLDESC_FOUR_BYTE            0x08
// One two and four more byte were expected but not found.
#define HIDP_GETCOLDESC_BYTE_ALLIGN          0x09
// The top level collection was not Byte alligned.
// Args[0~2] are bit lengths of Input Output and Report to this point
#define HIDP_GETCOLDESC_PUSH_RESOURCES       0x10
// Insufficient resources required to push more on the usage stack
#define HIDP_GETCOLDESC_ARRAY_NOT_8_BITS     0x11
#define HIDP_GETCOLDESC_ITEM_UNKNOWN         0x12
// The item in Args[0] is unknown
#define HIDP_GETCOLDESC_REPORT_ID            0x13
// Report ID outside of top level collection. Args[0] is report ID in question

#endif
