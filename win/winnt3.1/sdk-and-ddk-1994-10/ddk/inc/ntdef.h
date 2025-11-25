/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1993  Microsoft Corporation

Module Name:

    ntdef.h

Abstract:

    Type definitions for the basic types.

Author:

    Mark Lucovsky (markl)   02-Feb-1989

Revision History:

--*/

#ifndef _NTDEF_
#define _NTDEF_

#include <ctype.h>  // winnt

// begin_ntminiport

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef NOTHING
#define NOTHING
#endif

#ifndef CRITICAL
#define CRITICAL
#endif

#ifndef ANYSIZE_ARRAY
#define ANYSIZE_ARRAY 1       // winnt
#endif

#if defined(_MIPS_) || defined(_ALPHA_)     // winnt
#define UNALIGNED __unaligned               // winnt
#else                                       // winnt
#define UNALIGNED                           // winnt
#endif                                      // winnt

#ifndef CONST
#define CONST               const
#endif

//
// Void
//

typedef void *PVOID;    // winnt

// end_ntminiport

#if (_MSC_VER >= 800)                                                    // winnt
#define NTAPI __stdcall                                                  // winnt
#else                                                                    // winnt
#define _cdecl                                                           // winnt
#define NTAPI                                                            // winnt
#endif                                                                   // winnt

// begin_winnt begin_ntminiport

//
// Basics
//

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

//
// UNICODE (Wide Character) types
//

typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character

typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef CONST WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef CONST WCHAR *LPCWSTR, *PCWSTR;

//
// ANSI (Multi-byte Character) types
//
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;

typedef CONST CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;

//
// Neutral ANSI/UNICODE types and macros
//
#ifdef  UNICODE

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TUCHAR, *PTUCHAR;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPWSTR LPTCH, PTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR LPCTSTR;
typedef LPWSTR LP;
#define __TEXT(quote) L##quote

#else   /* UNICODE */

#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TUCHAR, *PTUCHAR;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */

typedef LPSTR LPTCH, PTCH;
typedef LPSTR PTSTR, LPTSTR;
typedef LPCSTR LPCTSTR;
#define __TEXT(quote) quote

#endif /* UNICODE */
#define TEXT(quote) __TEXT(quote)


// end_winnt

typedef double DOUBLE;

typedef struct _QUAD {              // QUAD is for those times we want
    double  DoNotUseThisField;      // an 8 byte aligned 8 byte long structure
} QUAD;                             // which is NOT really a floating point
                                    // number.  Use DOUBLE if you want an FP
                                    // number.

//
// Pointer to Basics
//

typedef SHORT *PSHORT;  // winnt
typedef LONG *PLONG;    // winnt
typedef QUAD *PQUAD;

//
// Unsigned Basics
//

// Tell windef.h that some types are already defined.
#define BASETYPES

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef QUAD UQUAD;

//
// Pointer to Unsigned Basics
//

typedef UCHAR *PUCHAR;
typedef USHORT *PUSHORT;
typedef ULONG *PULONG;
typedef UQUAD *PUQUAD;

//
// Signed characters
//

typedef signed char SCHAR;
typedef SCHAR *PSCHAR;

#ifndef NO_STRICT
#define STRICT 1
#endif

//
// Handle to an Object
//

// begin_winnt

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
#endif
typedef HANDLE *PHANDLE;

// end_winnt

//
// Low order two bits of a handle are ignored by the system and available
// for use by application code as tag bits.  The remaining bits are opaque
// and used to store a serial number and table index.
//

#define OBJ_HANDLE_TAGBITS  0x00000003L

//
// Cardinal Data Types [0 - 2**N-2)
//

typedef char CCHAR;          // winnt
typedef short CSHORT;
typedef ULONG CLONG;

typedef CCHAR *PCCHAR;
typedef CSHORT *PCSHORT;
typedef CLONG *PCLONG;

// end_ntminiport

//
// NLS basics (Locale and Language Ids)
//

typedef ULONG LCID;         // winnt
typedef PULONG PLCID;       // winnt
typedef USHORT LANGID;      // winnt

//
// NTSTATUS
//

typedef LONG NTSTATUS;
/*lint -e624 */  // Don't complain about different typedefs.   // winnt
typedef NTSTATUS *PNTSTATUS;
/*lint +e624 */  // Resume checking for different typedefs.    // winnt

//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

//
// Generic test for information on any status value.
//

#define NT_INFORMATION(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for warning on any status value.
//

#define NT_WARNING(Status) ((ULONG)(Status) >> 30 == 2)

//
// Generic test for error on any status value.
//

#define NT_ERROR(Status) ((ULONG)(Status) >> 30 == 3)

// begin_winnt
#define APPLICATION_ERROR_MASK       0x20000000
#define ERROR_SEVERITY_SUCCESS       0x00000000
#define ERROR_SEVERITY_INFORMATIONAL 0x40000000
#define ERROR_SEVERITY_WARNING       0x80000000
#define ERROR_SEVERITY_ERROR         0xC0000000
// end_winnt

//
// Large (64-bit) integer types and operations
//

#define TIME LARGE_INTEGER
#define _TIME _LARGE_INTEGER
#define PTIME PLARGE_INTEGER
#define LowTime LowPart
#define HighTime HighPart


// begin_winnt begin_ntminiport

#if defined(MIDL_PASS) || defined(_CFRONT_PASS_)
typedef double LONGLONG;
typedef double ULONGLONG;
#else
#if defined(_M_IX86)
typedef double LONGLONG;
typedef double ULONGLONG;
#elif defined(_M_MRX000)
typedef double LONGLONG;
typedef double ULONGLONG;
#elif defined(_ALPHA_)
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#else
typedef double LONGLONG;
typedef double ULONGLONG;
#endif
#endif

typedef LONGLONG *PLONGLONG;
typedef ULONGLONG *PULONGLONG;

#if defined(MIDL_PASS)
typedef struct _LARGE_INTEGER {
#else // MIDL_PASS
typedef union _LARGE_INTEGER {
#if defined(_CFRONT_PASS_)
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
#else
    struct {
        ULONG LowPart;
        LONG HighPart;
    };
#endif
#endif //MIDL_PASS
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;


#if defined(MIDL_PASS)
typedef struct _ULARGE_INTEGER {
#else // MIDL_PASS
typedef union _ULARGE_INTEGER {
#if defined(_CFRONT_PASS_)
    struct {
        ULONG LowPart;
        ULONG HighPart;
    } u;
#else
    struct {
        ULONG LowPart;
        ULONG HighPart;
    };
#endif
#endif //MIDL_PASS
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

// end_ntminiport

//
// Locally Unique Identifier
//

typedef LARGE_INTEGER LUID, *PLUID;

// end_winnt
// begin_ntminiport

//
// Physical address.
//

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS; // windbgkd

// end_ntminiport

#ifdef LARGE_INTEGER_INTRINICS

#define I64Neg( a ) (-(a))
#define I64Add( a, b ) ((a) + (b))
#define I64Sub( a, b ) ((a) - (b))
#define I64Mul( a, b ) ((a) * (b))
#define I64Div( a, b ) ((a) / (b))
#define I64Mod( a, b ) ((a) % (b))
#define I64Gtr( a, b ) ((a) > (b))
#define I64Geq( a, b ) ((a) >= (b))
#define I64Eql( a, b ) ((a) == (b))
#define I64Neq( a, b ) ((a) != (b))
#define I64Leq( a, b ) ((a) <= (b))
#define I64Ltr( a, b ) ((a) < (b))
#define I64Shr( a, b ) ((a) >> (b))
#define I64Shl( a, b ) ((a) << (b))
#define I64Sar( a, b ) ((LARGE_INTEGER)(a) >> (b))
#define I64Not( a ) (~(a))
#define I64And( a, b ) ((a) & (b))
#define I64Or( a, b ) ((a) | (b))
#define I64Xor( a, b ) ((a) ^ (b))

#define LiNeg(a)       (-(a))
#define LiAdd(a,b)     ((a) + (b))
#define LiSub(a,b)     ((a) - (b))
#define LiNMul(a,b)    ((a) * (b))
#define LiXMul(a,b)    ((a) * (b))
#define LiDiv(a,b)     ((a) / (b))
#define LiXDiv(a,b)    ((a) / (b))
#define LiMod(a,b)     ((a) % (b))
#define LiShr(a,b)     ((a) >> (b))
#define LiShl(a,b)     ((a) << (b))
#define LiGtr(a,b)     ((a) > (b))
#define LiGeq(a,b)     ((a) >= (b))
#define LiEql(a,b)     ((a) == (b))
#define LiNeq(a,b)     ((a) != (b))
#define LiLtr(a,b)     ((a) < (b))
#define LiLeq(a,b)     ((a) <= (b))
#define LiGtrZero(a)   ((a) > 0)
#define LiGeqZero(a)   ((a) >= 0)
#define LiEqlZero(a)   ((a) == 0)
#define LiNeqZero(a)   ((a) != 0)
#define LiLtrZero(a)   ((a) < (0)
#define LiLeqZero(a)   ((a) <= (0)
#define LiFromLong(a)  (a)
#define LiFromUlong(a) (a)

#define LiTemps        {;}
#define LiGtrT_(a,b)   ((a) > (b))
#define LiGtr_T(a,b)   ((a) > (b))
#define LiGtrTT(a,b)   ((a) > (b))
#define LiGeqT_(a,b)   ((a) >= (b))
#define LiGeq_T(a,b)   ((a) >= (b))
#define LiGeqTT(a,b)   ((a) >= (b))
#define LiEqlT_(a,b)   ((a) == (b))
#define LiEql_T(a,b)   ((a) == (b))
#define LiEqlTT(a,b)   ((a) == (b))
#define LiNeqT_(a,b)   ((a) != (b))
#define LiNeq_T(a,b)   ((a) != (b))
#define LiNeqTT(a,b)   ((a) != (b))
#define LiLtrT_(a,b)   ((a) < (b))
#define LiLtr_T(a,b)   ((a) < (b))
#define LiLtrTT(a,b)   ((a) < (b))
#define LiLeqT_(a,b)   ((a) <= (b))
#define LiLeq_T(a,b)   ((a) <= (b))
#define LiLeqTT(a,b)   ((a) <= (b))
#define LiGtrZeroT(a)  ((a) > 0)
#define LiGeqZeroT(a)  ((a) >= 0)
#define LiEqlZeroT(a)  ((a) == 0)
#define LiNeqZeroT(a)  ((a) != 0)
#define LiLtrZeroT(a)  ((a) < 0)
#define LiLeqZeroT(a)  ((a) <= 0)

#else

#define I64Neg( a ) (RtlLargeIntegerNegate((a)))
#define I64Add( a, b ) (RtlLargeIntegerAdd((a), (b)))
#define I64Sub( a, b ) (RtlLargeIntegerSubtract((a), (b)))
#define I64Mul( a, b ) (RtlLargeIntegerMultiply((a), (b)))
#define I64Div( a, b ) (RtlLargeIntegerDivide((a), (b), NULL))
#define I64Mod( a, b ) (RtlLargeIntegerModulo((a), (b)))
#define I64Gtr( a, b ) (RtlLargeIntegerGreaterThan((a), (b)))
#define I64Geq( a, b ) (RtlLargeIntegerGreaterThanOrEqualTo((a), (b)))
#define I64Eql( a, b ) (RtlLargeIntegerEqualTo((a), (b)))
#define I64Neq( a, b ) (!RtlLargeIntegerEqualTo((a), (b)))
#define I64Ltr( a, b ) (RtlLargeIntegerLessThan((a), (b)))
#define I64Leq( a, b ) (!RtlLargeIntegerGreaterThan((a), (b)))
#define I64Shr( a, b ) (RtlLargeIntegerShiftRight((a), (b)))
#define I64Shl( a, b ) (RtlLargeIntegerShiftLeft((a), (b)))
#define I64Sar( a, b ) (RtlLargeIntegerArithmeticShift((a), (b)))
#define I64Not( a ) (RtlLargeIntegerNegate((a)))
#define I64And( a, b ) (RtlLargeIntegerAnd((a), (b)))
#define I64Or( a, b ) (RtlLargeIntegerOr((a), (b)))
#define I64Xor( a, b ) (RtlLargeIntegerXor((a), (b)))

#define LiNeg(a)       (RtlLargeIntegerNegate((a)))                   // -a
#define LiAdd(a,b)     (RtlLargeIntegerAdd((a),(b)))                  // a + b
#define LiSub(a,b)     (RtlLargeIntegerSubtract((a),(b)))             // a - b
#define LiNMul(a,b)    (RtlEnlargedIntegerMultiply((a),(b)))          // a * b (Long * Long)
#define LiXMul(a,b)    (RtlExtendedIntegerMultiply((a),(b)))          // a * b (Large * Long)
#define LiDiv(a,b)     (RtlLargeIntegerDivide((a),(b),NULL))          // a / b (Large / Large)
#define LiXDiv(a,b)    (RtlExtendedLargeIntegerDivide((a),(b),NULL))  // a / b (Large / Long)
#define LiMod(a,b)     (RtlLargeIntegerModulo((a),(b)))               // a % b
#define LiShr(a,b)     (RtlLargeIntegerShiftRight((a),(CCHAR)(b)))    // a >> b
#define LiShl(a,b)     (RtlLargeIntegerShiftLeft((a),(CCHAR)(b)))     // a << b
#define LiGtr(a,b)     (RtlLargeIntegerGreaterThan((a),(b)))          // a > b
#define LiGeq(a,b)     (RtlLargeIntegerGreaterThanOrEqualTo((a),(b))) // a >= b
#define LiEql(a,b)     (RtlLargeIntegerEqualTo((a),(b)))              // a == b
#define LiNeq(a,b)     (RtlLargeIntegerNotEqualTo((a),(b)))           // a != b
#define LiLtr(a,b)     (RtlLargeIntegerLessThan((a),(b)))             // a < b
#define LiLeq(a,b)     (RtlLargeIntegerLessThanOrEqualTo((a),(b)))    // a <= b
#define LiGtrZero(a)   (RtlLargeIntegerGreaterThanZero((a)))          // a > 0
#define LiGeqZero(a)   (RtlLargeIntegerGreaterOrEqualToZero((a)))     // a >= 0
#define LiEqlZero(a)   (RtlLargeIntegerEqualToZero((a)))              // a == 0
#define LiNeqZero(a)   (RtlLargeIntegerNotEqualToZero((a)))           // a != 0
#define LiLtrZero(a)   (RtlLargeIntegerLessThanZero((a)))             // a < 0
#define LiLeqZero(a)   (RtlLargeIntegerLessOrEqualToZero((a)))        // a <= 0
#define LiFromLong(a)  (RtlConvertLongToLargeInteger((a)))
#define LiFromUlong(a) (RtlConvertUlongToLargeInteger((a)))

#define LiTemps        LARGE_INTEGER _LiT1,_LiT2
#define LiGtrT_(a,b)   ((_LiT1 = a,_LiT2),     LiGtr(_LiT1,(b)))
#define LiGtr_T(a,b)   ((_LiT1,_LiT2 = b),     LiGtr((a),_LiT2))
#define LiGtrTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiGtr(_LiT1,_LiT2))
#define LiGeqT_(a,b)   ((_LiT1 = a,_LiT2),     LiGeq(_LiT1,(b)))
#define LiGeq_T(a,b)   ((_LiT1,_LiT2 = b),     LiGeq((a),_LiT2))
#define LiGeqTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiGeq(_LiT1,_LiT2))
#define LiEqlT_(a,b)   ((_LiT1 = a,_LiT2),     LiEql(_LiT1,(b)))
#define LiEql_T(a,b)   ((_LiT1,_LiT2 = b),     LiEql((a),_LiT2))
#define LiEqlTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiEql(_LiT1,_LiT2))
#define LiNeqT_(a,b)   ((_LiT1 = a,_LiT2),     LiNeq(_LiT1,(b)))
#define LiNeq_T(a,b)   ((_LiT1,_LiT2 = b),     LiNeq((a),_LiT2))
#define LiNeqTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiNeq(_LiT1,_LiT2))
#define LiLtrT_(a,b)   ((_LiT1 = a,_LiT2),     LiLtr(_LiT1,(b)))
#define LiLtr_T(a,b)   ((_LiT1,_LiT2 = b),     LiLtr((a),_LiT2))
#define LiLtrTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiLtr(_LiT1,_LiT2))
#define LiLeqT_(a,b)   ((_LiT1 = a,_LiT2),     LiLeq(_LiT1,(b)))
#define LiLeq_T(a,b)   ((_LiT1,_LiT2 = b),     LiLeq((a),_LiT2))
#define LiLeqTT(a,b)   ((_LiT1 = a, _LiT2 = b),LiLeq(_LiT1,_LiT2))
#define LiGtrZeroT(a)  ((_LiT1 = a,_LiT2),     LiGtrZero(_LiT1))
#define LiGeqZeroT(a)  ((_LiT1 = a,_LiT2),     LiGeqZero(_LiT1))
#define LiEqlZeroT(a)  ((_LiT1 = a,_LiT2),     LiEqlZero(_LiT1))
#define LiNeqZeroT(a)  ((_LiT1 = a,_LiT2),     LiNeqZero(_LiT1))
#define LiLtrZeroT(a)  ((_LiT1 = a,_LiT2),     LiLtrZero(_LiT1))
#define LiLeqZeroT(a)  ((_LiT1 = a,_LiT2),     LiLeqZero(_LiT1))

#endif



//
// Event type
//

typedef enum _EVENT_TYPE {
    NotificationEvent,
    SynchronizationEvent
    } EVENT_TYPE;

//
// Wait type
//

typedef enum _WAIT_TYPE {
    WaitAll,
    WaitAny
    } WAIT_TYPE;

//
// Pointer to an Asciiz string
//

typedef CHAR *PSZ;
typedef CONST char *PCSZ;

//
// Counted String
//

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength), length_is(Length) ]
#endif // MIDL_PASS
    PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

//
// CONSTCounted String
//

typedef struct _CSTRING {
    USHORT Length;
    USHORT MaximumLength;
    CONST char *Buffer;
} CSTRING;
typedef CSTRING *PCSTRING;

typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;

//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
#define UNICODE_NULL ((WCHAR)0) // winnt

// begin_ntminiport

//
// Boolean
//

typedef UCHAR BOOLEAN;           // winnt
typedef BOOLEAN *PBOOLEAN;       // winnt

// end_ntminiport

// begin_winnt
//
//  Doubly linked list structure.  Can be used as either a list head, or
//  as link words.
//

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY;
typedef LIST_ENTRY *PLIST_ENTRY;

//
//  Singly linked list structure. Can be used as either a list head, or
//  as link words.
//

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY *Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;
// end_winnt


//
// Valid values for the Attributes field
//

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_VALID_ATTRIBUTES    0x000000F2L

//
// Object Attributes structure
//

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

//++
//
// VOID
// InitializeObjectAttributes(
//     OUT POBJECT_ATTRIBUTES p,
//     IN PUNICODE_STRING n,
//     IN ULONG a,
//     IN HANDLE r,
//     IN PSECURITY_DESCRIPTOR s
//     )
//
//--

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

// begin_ntminiport

//
// Constants
//

#define FALSE   0
#define TRUE    1

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif // NULL

// end_ntminiport

#define MINCHAR     0x80        // winnt
#define MAXCHAR     0x7f        // winnt
#define MINSHORT    0x8000      // winnt
#define MAXSHORT    0x7fff      // winnt
#define MINLONG     0x80000000  // winnt
#define MAXLONG     0x7fffffff  // winnt
#define MAXUCHAR    0xff        // winnt
#define MAXUSHORT   0xffff      // winnt
#define MAXULONG    0xffffffff  // winnt

//
// Useful Helper Macros
//

//
// Determine if an argument is present by testing the value of the pointer
// to the argument value.
//

#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)(ArgumentPointer) != (CHAR *)(NULL) )

// begin_winnt begin_ntminiport
//
// Calculate the byte offset of a field in a structure of type type.
//

#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))


//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (PCHAR)(address) - \
                                                  (PCHAR)(&((type *)0)->field)))
// end_winnt end_ntminiport

//
// Exception handler routine definition.
//

struct _CONTEXT;
struct _EXCEPTION_RECORD;

typedef
EXCEPTION_DISPOSITION
(*PEXCEPTION_ROUTINE) (
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT struct _CONTEXT *ContextRecord,
    IN OUT PVOID DispatcherContext
    );

// begin_ntminiport

//
// Interrupt Request Level (IRQL)
//

typedef UCHAR KIRQL;

typedef KIRQL *PKIRQL;

// end_ntminiport

//
// Product types
//

typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt
} NT_PRODUCT_TYPE, *PNT_PRODUCT_TYPE;




// begin_winnt

/*
 *  Language IDs.
 *
 *  The following two combinations of primary language ID and
 *  sublanguage ID have special semantics:
 *
 *    Primary Language ID   Sublanguage ID      Result
 *    -------------------   ---------------     ------------------------
 *    LANG_NEUTRAL          SUBLANG_NEUTRAL     Language neutral
 *    LANG_NEUTRAL          SUBLANG_DEFAULT     User default language
 *    LANG_NEUTRAL          SUBLANG_SYS_DEFAULT System default language
 */

/*
 *  Primary language IDs.
 */
#define LANG_NEUTRAL                     0x00

#define LANG_ALBANIAN                    0x1c
#define LANG_ARABIC                      0x01
#define LANG_BAHASA                      0x21
#define LANG_BULGARIAN                   0x02
#define LANG_CATALAN                     0x03
#define LANG_CHINESE                     0x04
#define LANG_CZECH                       0x05
#define LANG_DANISH                      0x06
#define LANG_DUTCH                       0x13
#define LANG_ENGLISH                     0x09
#define LANG_FINNISH                     0x0b
#define LANG_FRENCH                      0x0c
#define LANG_GERMAN                      0x07
#define LANG_GREEK                       0x08
#define LANG_HEBREW                      0x0d
#define LANG_HUNGARIAN                   0x0e
#define LANG_ICELANDIC                   0x0f
#define LANG_ITALIAN                     0x10
#define LANG_JAPANESE                    0x11
#define LANG_KOREAN                      0x12
#define LANG_NORWEGIAN                   0x14
#define LANG_POLISH                      0x15
#define LANG_PORTUGUESE                  0x16
#define LANG_RHAETO_ROMAN                0x17
#define LANG_ROMANIAN                    0x18
#define LANG_RUSSIAN                     0x19
#define LANG_SERBO_CROATIAN              0x1a
#define LANG_SLOVAK                      0x1b
#define LANG_SPANISH                     0x0a
#define LANG_SWEDISH                     0x1d
#define LANG_THAI                        0x1e
#define LANG_TURKISH                     0x1f
#define LANG_URDU                        0x20

/*
 *  Sublanguage IDs.
 *
 *  The name immediately following SUBLANG_ dictates which primary
 *  language ID that sublanguage ID can be combined with to form a
 *  valid language ID.
 */
#define SUBLANG_NEUTRAL                  0x00    /* language neutral */
#define SUBLANG_DEFAULT                  0x01    /* user default */
#define SUBLANG_SYS_DEFAULT              0x02    /* system default */

#define SUBLANG_CHINESE_SIMPLIFIED       0x02    /* Chinese (Simplified) */
#define SUBLANG_CHINESE_TRADITIONAL      0x01    /* Chinese (Traditional) */
#define SUBLANG_DUTCH                    0x01    /* Dutch */
#define SUBLANG_DUTCH_BELGIAN            0x02    /* Dutch (Belgian) */
#define SUBLANG_ENGLISH_US               0x01    /* English (USA) */
#define SUBLANG_ENGLISH_UK               0x02    /* English (UK) */
#define SUBLANG_ENGLISH_AUS              0x03    /* English (Australian) */
#define SUBLANG_ENGLISH_CAN              0x04    /* English (Canadian) */
#define SUBLANG_ENGLISH_NZ               0x05    /* English (New Zealand) */
#define SUBLANG_ENGLISH_EIRE             0x06    /* English (Irish) */
#define SUBLANG_FRENCH                   0x01    /* French */
#define SUBLANG_FRENCH_BELGIAN           0x02    /* French (Belgian) */
#define SUBLANG_FRENCH_CANADIAN          0x03    /* French (Canadian) */
#define SUBLANG_FRENCH_SWISS             0x04    /* French (Swiss) */
#define SUBLANG_GERMAN                   0x01    /* German */
#define SUBLANG_GERMAN_SWISS             0x02    /* German (Swiss) */
#define SUBLANG_GERMAN_AUSTRIAN          0x03    /* German (Austrian) */
#define SUBLANG_ITALIAN                  0x01    /* Italian */
#define SUBLANG_ITALIAN_SWISS            0x02    /* Italian (Swiss) */
#define SUBLANG_NORWEGIAN_BOKMAL         0x01    /* Norwegian (Bokmal) */
#define SUBLANG_NORWEGIAN_NYNORSK        0x02    /* Norwegian (Nynorsk) */
#define SUBLANG_PORTUGUESE               0x02    /* Portuguese */
#define SUBLANG_PORTUGUESE_BRAZILIAN     0x01    /* Portuguese (Brazilian) */
#define SUBLANG_SERBO_CROATIAN_CYRILLIC  0x02    /* Serbo-Croatian (Cyrillic) */
#define SUBLANG_SERBO_CROATIAN_LATIN     0x01    /* Croato-Serbian (Latin) */
#define SUBLANG_SPANISH                  0x01    /* Spanish (Castilian) */
#define SUBLANG_SPANISH_MEXICAN          0x02    /* Spanish (Mexican) */
#define SUBLANG_SPANISH_MODERN           0x03    /* Spanish (Modern) */

/*
 *  Sorting IDs.
 *
 */
#define SORT_DEFAULT                     0x0     /* sorting default */


/*
 *  A language ID is a 16 bit value which is the combination of a
 *  primary language ID and a secondary language ID.  The bits are
 *  allocated as follows:
 *
 *       +-----------------------+-------------------------+
 *       |     Sublanguage ID    |   Primary Language ID   |
 *       +-----------------------+-------------------------+
 *        15                   10 9                       0   bit
 *
 *
 *  Language ID creation/extraction macros:
 *
 *    MAKELANGID    - construct language id from a primary language id and
 *                    a sublanguage id.
 *    PRIMARYLANGID - extract primary language id from a language id.
 *    SUBLANGID     - extract sublanguage id from a language id.
 */
#define MAKELANGID(p, s)       ((((USHORT)(s)) << 10) | (USHORT)(p))
#define PRIMARYLANGID(lgid)    ((USHORT)(lgid) & 0x3ff)
#define SUBLANGID(lgid)        ((USHORT)(lgid) >> 10)


/*
 *  A locale ID is a 32 bit value which is the combination of a
 *  language ID, a sort ID, and a reserved area.  The bits are
 *  allocated as follows:
 *
 *       +-------------+---------+-------------------------+
 *       |   Reserved  | Sort ID |      Language ID        |
 *       +-------------+---------+-------------------------+
 *        31         20 19     16 15                      0   bit
 *
 *
 *  Locale ID creation/extraction macros:
 *
 *    MAKELCID       - construct locale id from a language id and a sort id.
 *    LANGIDFROMLCID - extract language id from a locale id.
 *    SORTIDFROMLCID - extract sort id from a locale id.
 */
#define NLS_VALID_LOCALE_MASK  0x000fffff

#define MAKELCID(lgid, srtid)  ((ULONG)((((ULONG)((USHORT)(srtid))) << 16) |  \
                                         ((ULONG)((USHORT)(lgid)))))
#define LANGIDFROMLCID(lcid)   ((USHORT)(lcid))
#define SORTIDFROMLCID(lcid)   ((USHORT)((((ULONG)(lcid)) & NLS_VALID_LOCALE_MASK) >> 16))


/*
 *  Default System and User IDs for language and locale.
 */
#define LANG_SYSTEM_DEFAULT    (MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT      (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))

#define LOCALE_SYSTEM_DEFAULT  (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT    (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))



// begin_ntminiport

//
// Macros used to eliminate compiler warning generated when formal
// parameters or local variables are not declared.
//
// Use DBG_UNREFERENCED_PARAMETER() when a parameter is not yet
// referenced but will be once the module is completely developed.
//
// Use DBG_UNREFERENCED_LOCAL_VARIABLE() when a local variable is not yet
// referenced but will be once the module is completely developed.
//
// Use UNREFERENCED_PARAMETER() if a parameter will never be referenced.
//
// DBG_UNREFERENCED_PARAMETER and DBG_UNREFERENCED_LOCAL_VARIABLE will
// eventually be made into a null macro to help determine whether there
// is unfinished work.
//

#if ! (defined(lint) || defined(_lint))
#define UNREFERENCED_PARAMETER(P)          (P)
#define DBG_UNREFERENCED_PARAMETER(P)      (P)
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)

#else // lint or _lint

// Note: lint -e530 says don't complain about uninitialized variables for
// this.  line +e530 turns that checking back on.  Error 527 has to do with
// unreachable code.

#define UNREFERENCED_PARAMETER(P)          \
    /*lint -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint +e527 +e530 */
#define DBG_UNREFERENCED_PARAMETER(P)      \
    /*lint -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint +e527 +e530 */
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) \
    /*lint -e527 -e530 */ \
    { \
        (V) = (V); \
    } \
    /*lint +e527 +e530 */

#endif // lint or _lint


// end_winnt end_ntminiport

#endif // _NTDEF_
