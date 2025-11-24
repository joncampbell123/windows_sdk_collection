/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    imagehlp.h

Abstract:

    This is a private header file for imagehlp.

Revision History:

--*/

#ifndef _IMAGEHLP_PRV_
#define _IMAGEHLP_PRV_

#ifdef __cplusplus
extern "C" {
#endif

#define _IMAGEHLP_SOURCE_

#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <malloc.h>
#include <cvexefmt.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "pdb.h"


// Define some list prototypes

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

BOOL
CalculateImagePtrs(
    PLOADED_IMAGE LoadedImage
    );

#ifdef __cplusplus
}
#endif

#endif
