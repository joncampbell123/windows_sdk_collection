/*++

Copyright (c) 1992 Microsoft Corporation

Module Name:

    rpcndr.h

Abstract:

    Float and double conversion routines.

Author:

    DonnaLi (01-01-91)

Environment:

    This code should execute in all environments supported by RPC
    (DOS, Win 3.X, and Win/NT as well as OS2).

Revision History:

   DONNALI  08-29-91     Start recording history
   donnali  09-11-91     change conversion macros
   donnali  09-18-91     check in files for moving
   STEVEZ   10-15-91     Merge with NT tree
   donnali  10-28-91     add prototype
   donnali  11-19-91     bugfix for strings
   MIKEMON  12-17-91     DCE runtime API conversion
   donnali  03-24-92     change rpc public header f
   STEVEZ   04-04-92     add nsi include
   mikemon  04-18-92     security support and misc
   DovhH    04-24-24     Changed signature of <int>_from_ndr
                         (to unsigned <int>)
                         Added <base_type>_array_from_ndr routines

--*/

#ifndef __RPCNDR_H__
#define __RPCNDR_H__

// Set the packing level for RPC structures for Dos and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack(2)
#endif

#include "rpcnsip.h"

#ifdef __cplusplus
extern "C" {
#endif

/*

     Network Computing Architecture (NCA) definition:

     Network Data Representation: (NDR) Label format:
     An unsigned long (32 bits) with the following layout:

     3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
     1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +---------------+---------------+---------------+-------+-------+
    |   Reserved    |   Reserved    |Floating point | Int   | Char  |
    |               |               |Representation | Rep.  | Rep.  |
    +---------------+---------------+---------------+-------+-------+

     Where

         Reserved:

             Must be zero (0) for NCA 1.5 and NCA 2.0.

         Floating point Representation is:

             0 - IEEE
             1 - VAX
             2 - Cray
             3 - IBM

         Int Rep. is Integer Representation:

             0 - Big Endian
             1 - Little Endian

         Char Rep. is Character Representation:

             0 - ASCII
             1 - EBCDIC

     The Microsoft Local Data Representation (for all platforms which are
     of interest currently is edefined below:

*/

#define NDR_CHAR_REP_MASK               (unsigned long)0X0000000FL
#define NDR_INT_REP_MASK                (unsigned long)0X000000F0L
#define NDR_FLOAT_REP_MASK              (unsigned long)0X0000FF00L

#define NDR_LITTLE_ENDIAN               (unsigned long)0X00000010L
#define NDR_BIG_ENDIAN                  (unsigned long)0X00000000L

#define NDR_IEEE_FLOAT                  (unsigned long)0X00000000L
#define NDR_VAX_FLOAT                   (unsigned long)0X00000100L

#define NDR_ASCII_CHAR                  (unsigned long)0X00000000L
#define NDR_EBCDIC_CHAR                 (unsigned long)0X00000001L

#define NDR_LOCAL_DATA_REPRESENTATION   (unsigned long)0X00000010L

/*
 *  Other MIDL base types / predefined types:
 */

typedef unsigned char byte;
typedef unsigned char boolean;

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifdef __RPC_DOS__
#define __RPC_CALLEE       __far __pascal
#endif

#ifdef __RPC_WIN16__
#define __RPC_CALLEE       __far __pascal __export
#endif

#ifdef __RPC_WIN32__
#if   (_MSC_VER >= 800)
#define __RPC_CALLEE       __stdcall
#else
#define __RPC_CALLEE
#endif
#endif

#ifdef __RPC_OS2_1X__
#define __RPC_CALLEE_STUB  __far __pascal __loadds
#endif

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free     MIDL_user_free
#define __MIDL_USER_DEFINED
#endif


/*
 * Context related definitions:
 *
 * Client and Server Contexts.  Just enough of the structure is exposed
 * to allow access to some public fields
 *
 */

typedef void __RPC_FAR * NDR_CCONTEXT;
typedef struct {void * pad[2]; void * userContext; } * NDR_SCONTEXT;

#define NDRSContextValue(hContext) (&(hContext)->userContext)

#define cbNDRContext 20         /* size of context on WIRE */

typedef void (__RPC_USER __RPC_FAR * NDR_RUNDOWN)(void __RPC_FAR * context);

RPC_BINDING_HANDLE RPC_ENTRY
NDRCContextBinding (
    IN NDR_CCONTEXT CContext
    );

void RPC_ENTRY
NDRCContextMarshall (
        IN  NDR_CCONTEXT CContext,
        OUT void __RPC_FAR *pBuff
        );

void RPC_ENTRY
NDRCContextUnmarshall (
        OUT NDR_CCONTEXT __RPC_FAR *pCContext,
        IN  RPC_BINDING_HANDLE hBinding,
        IN  void __RPC_FAR *pBuff,
        IN  unsigned long DataRepresentation
        );

void RPC_ENTRY
NDRSContextMarshall (
        IN  NDR_SCONTEXT CContext,
        OUT void __RPC_FAR *pBuff,
        IN  NDR_RUNDOWN userRunDownIn
        );

NDR_SCONTEXT RPC_ENTRY
NDRSContextUnmarshall (
    IN  void __RPC_FAR *pBuff,
    IN  unsigned long DataRepresentation
    );

void RPC_ENTRY
NDRcopy (
    IN void __RPC_FAR *pTarget,
    IN void __RPC_FAR *pSource,
    IN unsigned int size
    );


/*
 *
 *  Base type conversion macros:
 *
 */

#define byte_from_ndr(source, target) \
    { \
    *target = *(*(char __RPC_FAR * __RPC_FAR *)&source->Buffer)++; \
    }

#define byte_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
		(((char __RPC_FAR *)Target)+LowerIndex), \
		Source->Buffer, \
		(unsigned int)(UpperIndex-LowerIndex)); \
    *(unsigned long __RPC_FAR *)&Source->Buffer += (UpperIndex-LowerIndex); \
    }

#define boolean_from_ndr(source, target) \
    { \
    *target = *(*(char __RPC_FAR * __RPC_FAR *)&source->Buffer)++; \
    }

#define boolean_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
		(((char __RPC_FAR *)Target)+LowerIndex), \
		Source->Buffer, \
		(unsigned int)(UpperIndex-LowerIndex)); \
    *(unsigned long __RPC_FAR *)&Source->Buffer += (UpperIndex-LowerIndex); \
    }

#define small_from_ndr(source, target) \
    { \
    *target = *(*(char __RPC_FAR * __RPC_FAR *)&source->Buffer)++; \
    }

#define small_from_ndr_temp(source, target, format) \
    { \
    *target = *(*(char __RPC_FAR * __RPC_FAR *)source)++; \
    }

#define small_array_from_ndr(Source, LowerIndex, UpperIndex, Target) \
    { \
    NDRcopy ( \
		(((char __RPC_FAR *)Target)+LowerIndex), \
		Source->Buffer, \
		(unsigned int)(UpperIndex-LowerIndex)); \
    *(unsigned long __RPC_FAR *)&Source->Buffer += (UpperIndex-LowerIndex); \
    }

/*
 *
 *  Character and string support routines:
 *
 */

size_t RPC_ENTRY
MIDL_wchar_strlen (
    IN wchar_t __RPC_FAR *   s
    );

void RPC_ENTRY
MIDL_wchar_strcpy (
    OUT void __RPC_FAR *     t,
    IN wchar_t __RPC_FAR *   s
    );

/*
 *
 *  Base type conversion routines:
 *
 */

void RPC_ENTRY
char_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT unsigned char __RPC_FAR *                 Target
    );

void RPC_ENTRY
char_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned char __RPC_FAR *                 Target
    );

void RPC_ENTRY
short_from_ndr (
    IN OUT PRPC_MESSAGE                           source,
    OUT unsigned short __RPC_FAR *                target
    );

void RPC_ENTRY
short_array_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned short __RPC_FAR *                Target
    );

void RPC_ENTRY
short_from_ndr_temp (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR *  source,
    OUT unsigned short __RPC_FAR *                target,
    IN unsigned long                              format
    );

void RPC_ENTRY
long_from_ndr (
    IN OUT PRPC_MESSAGE                           source,
    OUT unsigned long __RPC_FAR *                 target
    );

void RPC_ENTRY
long_array_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT unsigned long __RPC_FAR *                 Target
    );

void RPC_ENTRY
long_from_ndr_temp (
    IN OUT unsigned char __RPC_FAR * __RPC_FAR *  source,
    OUT unsigned long __RPC_FAR *                 target,
    IN unsigned long                              format
    );

void RPC_ENTRY
enum_from_ndr(
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT unsigned int __RPC_FAR *                  Target
    );

void RPC_ENTRY
float_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT void __RPC_FAR *                          Target
    );

void RPC_ENTRY
float_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT void __RPC_FAR *                          Target
    );

void RPC_ENTRY
double_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    OUT void __RPC_FAR *                          Target
    );

void RPC_ENTRY
double_array_from_ndr (
    IN OUT PRPC_MESSAGE                           SourceMessage,
    IN unsigned long                              LowerIndex,
    IN unsigned long                              UpperIndex,
    OUT void __RPC_FAR *                          Target
    );

/*
 *
 *  Other data conversion / unmarshalling routines:
 *
 */

void RPC_ENTRY
data_from_ndr (
    PRPC_MESSAGE                                  source,
    void __RPC_FAR *                              target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void RPC_ENTRY
data_into_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void RPC_ENTRY
tree_into_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void RPC_ENTRY
data_size_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void RPC_ENTRY
tree_size_ndr (
    void __RPC_FAR *                              source,
    PRPC_MESSAGE                                  target,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void RPC_ENTRY
tree_peek_ndr (
    PRPC_MESSAGE                                  source,
    unsigned char __RPC_FAR * __RPC_FAR *         buffer,
    char __RPC_FAR *                              format,
    unsigned char                                 MscPak
    );

void __RPC_FAR * RPC_ENTRY
midl_allocate (
    size_t      size
    );

#ifdef __RPC_DOS__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#ifdef __RPC_WIN16__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif

#ifdef __RPC_WIN32__
#define MIDL_ascii_strlen(string) \
    strlen(string)
#define MIDL_ascii_strcpy(target,source) \
    strcpy(target,source)
#define MIDL_memset(s,c,n) \
    memset(s,c,n)
#endif

#ifdef __RPC_OS2_1X__
#define MIDL_ascii_strlen(string) \
    _fstrlen(string)
#define MIDL_ascii_strcpy(target,source) \
    _fstrcpy(target,source)
#define MIDL_memset(s,c,n) \
    _fmemset(s,c,n)
#endif


void RPC_ENTRY
RpcSsDestroyClientContext (
    IN void __RPC_FAR * __RPC_FAR * ContextHandle
    );

#ifdef __cplusplus
}
#endif

// Reset the packing level for DOS and Windows.

#if defined(__RPC_DOS__) || defined(__RPC_WIN16__)
#pragma pack()
#endif

#endif /* __RPCNDR_H__ */
