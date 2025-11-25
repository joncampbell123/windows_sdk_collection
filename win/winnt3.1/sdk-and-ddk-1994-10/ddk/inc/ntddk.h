/*++ BUILD Version: 0166    // Increment this if a change has global effects

Copyright (c) 1990-1992  Microsoft Corporation

Module Name:

    ntddk.h

Abstract:

    This module defines the NT types, constants, and functions that are
    exposed to device drivers.

Revision History:

--*/

#ifndef _NTDDK_
#define _NTDDK_

#define NT_INCLUDED

#include <excpt.h>
#include <ntdef.h>
#include <ntstatus.h>
#include <bugcodes.h>
#include <exlevels.h>
#include <ntiologc.h>

//
// Define types that are not exported.
//

typedef struct _KTHREAD *PKTHREAD;
typedef struct _ETHREAD *PETHREAD;
typedef struct _EPROCESS *PEPROCESS;
typedef struct _PEB *PPEB;
typedef struct _KINTERRUPT *PKINTERRUPT;
typedef struct _IO_TIMER *PIO_TIMER;
typedef struct _OBJECT_TYPE *POBJECT_TYPE;

#if defined(_ALPHA_)
PKTHREAD KeGetCurrentThread();
KIRQL KeGetCurrentIrql();
#endif // defined(_ALPHA_)

#if defined(MIPS)
#define KIPCR 0xfffff000
#define PCR ((volatile KPCR * const)KIPCR)
#define KeGetCurrentThread() PCR->CurrentThread
#define KeGetCurrentIrql() PCR->CurrentIrql
#endif // defined(MIPS)

#if defined(_M_IX86)
PKTHREAD KeGetCurrentThread();

KIRQL KeGetCurrentIrql();
#endif // defined(_M_IX86)

#define PsGetCurrentProcess() IoGetCurrentProcess()
#define PsGetCurrentThread() ((PETHREAD) (KeGetCurrentThread()))
extern PCCHAR KeNumberProcessors;

#ifndef DBG
#define DBG 0
#endif

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif

#if DEVL
//
// Global flag set by NtPartyByNumber(6) controls behaviour of
// NT.  See \nt\sdk\inc\ntexapi.h for flag definitions
//

extern ULONG NtGlobalFlag;

#define IF_NTOS_DEBUG( FlagName ) \
    if (NtGlobalFlag & (FLG_ ## FlagName))

#else
#define IF_NTOS_DEBUG( FlagName ) if (FALSE)
#endif

//
// Kernel definitions that need to be here for forward reference purposes
//

//
// Processor modes.
//

typedef CCHAR KPROCESSOR_MODE;

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

//
// APC function types
//

//
// Put in an empty definition for the KAPC so that the
// routines can reference it before it is declared.
//

struct _KAPC;

typedef
VOID
(*PKNORMAL_ROUTINE) (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

typedef
VOID
(*PKKERNEL_ROUTINE) (
    IN struct _KAPC *Apc,
    IN OUT PKNORMAL_ROUTINE *NormalRoutine,
    IN OUT PVOID *NormalContext,
    IN OUT PVOID *SystemArgument1,
    IN OUT PVOID *SystemArgument2
    );

typedef
VOID
(*PKRUNDOWN_ROUTINE) (
    IN struct _KAPC *Apc
    );

typedef
BOOLEAN
(*PKSYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );

typedef
BOOLEAN
(*PKTRANSFER_ROUTINE) (
    VOID
    );

//
//
// Asynchronous Procedure Call (APC) object
//

typedef struct _KAPC {
    CSHORT Type;
    CSHORT Size;
    struct _KPROCESS *Process;
    struct _KTHREAD *Thread;
    LIST_ENTRY ApcListEntry;
    PKKERNEL_ROUTINE KernelRoutine;
    PKRUNDOWN_ROUTINE RundownRoutine;
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    CCHAR ApcStateIndex;
    KPROCESSOR_MODE ApcMode;
    BOOLEAN Inserted;
} KAPC;

typedef KAPC *PKAPC;

//
// DPC routine
//

struct _KDPC;

typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Deferred Procedure Call (DPC) object
//

typedef struct _KDPC {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    BOOLEAN Inserted;
} KDPC;

typedef KDPC *PKDPC;

//
// Power Notify Routine
//

typedef
VOID
(*PKNOTIFY_ROUTINE) (
    IN PVOID NotifyContext
    );

//
// I/O system definitions.
//
// Define a Memory Descriptor List (MDL)
//
// An MDL describes pages in a virtual buffer in terms of physical pages.  The
// pages associated with the buffer are described in an array that is allocated
// just after the MDL header structure itself.  In a future compiler this will
// be placed at:
//
//      ULONG Pages[];
//
// Until this declaration is permitted, however, one simply calculates the
// base of the array by adding one to the base MDL pointer:
//
//      Pages = (PULONG) (Mdl + 1);
//
// Notice that while in the context of the subject thread, the base virtual
// address of a buffer mapped by an MDL may be referenced using the following:
//
//      Mdl->StartVa | Mdl->ByteOffset
//

typedef struct _MDL {
    struct _MDL *Next;
    CSHORT Size;
    CSHORT MdlFlags;
    struct _EPROCESS *Process;
    PVOID MappedSystemVa;
    PVOID StartVa;
    ULONG ByteCount;
    ULONG ByteOffset;
} MDL, *PMDL;

#define MDL_MAPPED_TO_SYSTEM_VA     0x0001
#define MDL_PAGES_LOCKED            0x0002
#define MDL_SOURCE_IS_NONPAGED_POOL 0x0004
#define MDL_ALLOCATED_FROM_ZONE     0x0008
#define MDL_PARTIAL                 0x0010
#define MDL_PARTIAL_HAS_BEEN_MAPPED 0x0020
#define MDL_IO_PAGE_READ            0x0040
#define MDL_WRITE_OPERATION         0x0080
#define MDL_PARENT_MAPPED_SYSTEM_VA 0x0100
#define MDL_LOCK_HELD               0x0200

#define MDL_MAPPING_FLAGS (MDL_MAPPED_TO_SYSTEM_VA |     \
                           MDL_PAGES_LOCKED |            \
                           MDL_SOURCE_IS_NONPAGED_POOL | \
                           MDL_PARTIAL_HAS_BEEN_MAPPED | \
                           MDL_PARENT_MAPPED_SYSTEM_VA | \
                           MDL_LOCK_HELD )


//
// switch to DBG when appropriate
//

#if DBG
#define PAGED_CODE() \
    if (KeGetCurrentIrql() > APC_LEVEL) { \
	KdPrint(( "EX: Pageable code called at IRQL %d\n", KeGetCurrentIrql() )); \
        ASSERT(FALSE); \
        }
#else
#define PAGED_CODE()
#endif

//
//  Define an access token from a programmer's viewpoint.  The structure is
//  completely opaque and the programer is only allowed to have pointers
//  to tokens.
//

typedef PVOID PACCESS_TOKEN;            // winnt

//
// Pointer to a SECURITY_DESCRIPTOR  opaque data type.
//

typedef PVOID PSECURITY_DESCRIPTOR;     // winnt

//
// Define a pointer to the Security ID data type (an opaque data type)
//

typedef PVOID PSID;     // winnt

typedef ULONG ACCESS_MASK;
typedef ACCESS_MASK *PACCESS_MASK;

// end_winnt
//
//  The following are masks for the predefined standard access types
//

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

//
// AccessSystemAcl access type
//

#define ACCESS_SYSTEM_SECURITY           (0x01000000L)

//
// MaximumAllowed access type
//

#define MAXIMUM_ALLOWED                  (0x02000000L)

//
//  These are the generic rights.
//

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)


//
//  Define the generic mapping array.  This is used to denote the
//  mapping of each generic access right to a specific access mask.
//

typedef struct _GENERIC_MAPPING {
    ACCESS_MASK GenericRead;
    ACCESS_MASK GenericWrite;
    ACCESS_MASK GenericExecute;
    ACCESS_MASK GenericAll;
} GENERIC_MAPPING;
typedef GENERIC_MAPPING *PGENERIC_MAPPING;



////////////////////////////////////////////////////////////////////////
//                                                                    //
//                        LUID_AND_ATTRIBUTES                         //
//                                                                    //
////////////////////////////////////////////////////////////////////////
//
//


#ifndef RC_INVOKED
#pragma pack(4)
#endif

typedef struct _LUID_AND_ATTRIBUTES {
    LUID Luid;
    ULONG Attributes;
    } LUID_AND_ATTRIBUTES, * PLUID_AND_ATTRIBUTES;
typedef LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef LUID_AND_ATTRIBUTES_ARRAY *PLUID_AND_ATTRIBUTES_ARRAY;

#ifndef RC_INVOKED
#pragma pack()
#endif

// This is the *current* ACL revision

#define ACL_REVISION     (2)

// This is the history of ACL revisions.  Add a new one whenever
// ACL_REVISION is updated

#define ACL_REVISION1   (1)
#define ACL_REVISION2   (2)

typedef struct _ACL {
    UCHAR AclRevision;
    UCHAR Sbz1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Sbz2;
} ACL;
typedef ACL *PACL;

//
// Current security descriptor revision value
//

#define SECURITY_DESCRIPTOR_REVISION     (1)
#define SECURITY_DESCRIPTOR_REVISION1    (1)

//
// Privilege attributes
//

#define SE_PRIVILEGE_ENABLED_BY_DEFAULT (0x00000001L)
#define SE_PRIVILEGE_ENABLED            (0x00000002L)
#define SE_PRIVILEGE_USED_FOR_ACCESS    (0x80000000L)

//
// Privilege Set Control flags
//

#define PRIVILEGE_SET_ALL_NECESSARY    (1)

//
//  Privilege Set - This is defined for a privilege set of one.
//                  If more than one privilege is needed, then this structure
//                  will need to be allocated with more space.
//
//  Note: don't change this structure without fixing the INITIAL_PRIVILEGE_SET
//  structure (defined in se.h)
//

typedef struct _PRIVILEGE_SET {
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
    } PRIVILEGE_SET, * PPRIVILEGE_SET;

//
// These must be converted to LUIDs before use.
//

#define SE_MIN_WELL_KNOWN_PRIVILEGE       (2L)
#define SE_CREATE_TOKEN_PRIVILEGE         (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE   (3L)
#define SE_LOCK_MEMORY_PRIVILEGE          (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE       (5L)
#define SE_UNSOLICITED_INPUT_PRIVILEGE    (6L)
#define SE_TCB_PRIVILEGE                  (7L)
#define SE_SECURITY_PRIVILEGE             (8L)
#define SE_TAKE_OWNERSHIP_PRIVILEGE       (9L)
#define SE_LOAD_DRIVER_PRIVILEGE          (10L)
#define SE_SYSTEM_PROFILE_PRIVILEGE       (11L)
#define SE_SYSTEMTIME_PRIVILEGE           (12L)
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE  (13L)
#define SE_INC_BASE_PRIORITY_PRIVILEGE    (14L)
#define SE_CREATE_PAGEFILE_PRIVILEGE      (15L)
#define SE_CREATE_PERMANENT_PRIVILEGE     (16L)
#define SE_BACKUP_PRIVILEGE               (17L)
#define SE_RESTORE_PRIVILEGE              (18L)
#define SE_SHUTDOWN_PRIVILEGE             (19L)
#define SE_DEBUG_PRIVILEGE                (20L)
#define SE_AUDIT_PRIVILEGE                (21L)
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE   (22L)
#define SE_CHANGE_NOTIFY_PRIVILEGE        (23L)
#define SE_REMOTE_SHUTDOWN_PRIVILEGE      (24L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE       (SE_REMOTE_SHUTDOWN_PRIVILEGE)

//
// Impersonation Level
//
// Impersonation level is represented by a pair of bits in Windows.
// If a new impersonation level is added or lowest value is changed from
// 0 to something else, fix the Windows CreateFile call.
//

typedef enum _SECURITY_IMPERSONATION_LEVEL {
    SecurityAnonymous,
    SecurityIdentification,
    SecurityImpersonation,
    SecurityDelegation
    } SECURITY_IMPERSONATION_LEVEL, * PSECURITY_IMPERSONATION_LEVEL;

#define SECURITY_MAX_IMPERSONATION_LEVEL SecurityDelegation

#define DEFAULT_IMPERSONATION_LEVEL SecurityImpersonation

// end_nthal
//
// Security Tracking Mode
//

#define SECURITY_DYNAMIC_TRACKING      (TRUE)
#define SECURITY_STATIC_TRACKING       (FALSE)

typedef BOOLEAN SECURITY_CONTEXT_TRACKING_MODE,
                    * PSECURITY_CONTEXT_TRACKING_MODE;



//
// Quality Of Service
//

typedef struct _SECURITY_QUALITY_OF_SERVICE {
    ULONG Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
    } SECURITY_QUALITY_OF_SERVICE, * PSECURITY_QUALITY_OF_SERVICE;


//
// Used to represent information related to a thread impersonation
//

typedef struct _SE_IMPERSONATION_STATE {
    PACCESS_TOKEN Token;
    BOOLEAN CopyOnOpen;
    BOOLEAN EffectiveOnly;
    SECURITY_IMPERSONATION_LEVEL Level;
} SE_IMPERSONATION_STATE, *PSE_IMPERSONATION_STATE;


typedef ULONG SECURITY_INFORMATION, *PSECURITY_INFORMATION;

#define OWNER_SECURITY_INFORMATION       (0X00000001L)
#define GROUP_SECURITY_INFORMATION       (0X00000002L)
#define DACL_SECURITY_INFORMATION        (0X00000004L)
#define SACL_SECURITY_INFORMATION        (0X00000008L)

#define LOW_PRIORITY 0              // Lowest thread priority level
#define LOW_REALTIME_PRIORITY 16    // Lowest realtime priority level
#define HIGH_PRIORITY 31            // Highest thread priority level
#define MAXIMUM_PRIORITY 32         // Number of thread priority levels
// begin_winnt
#define MAXIMUM_WAIT_OBJECTS 64     // Maximum number of wait objects

#define MAXIMUM_SUSPEND_COUNT MAXCHAR // Maximum times thread can be suspended
// end_winnt

//
// Thread affinity
//

typedef ULONG KAFFINITY;
typedef KAFFINITY *PKAFFINITY;

//
// Thread priority
//

typedef LONG KPRIORITY;

//
// Spin Lock
//

typedef ULONG KSPIN_LOCK;  // winnt

typedef KSPIN_LOCK *PKSPIN_LOCK;

//
// Interrupt routine (first level dispatch)
//

typedef
VOID
(*PKINTERRUPT_ROUTINE) (
    VOID
    );

// begin_winnt
//
// for move macros
//
#include <string.h>
// end_winnt
//
// If debugging support enabled, define an ASSERT macro that works.  Otherwise
// define the ASSERT macro to expand to an empty expression.
//

#if DBG
VOID
NTAPI
RtlAssert(
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
    );

#define ASSERT( exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, NULL )

#define ASSERTMSG( msg, exp ) \
    if (!(exp)) \
        RtlAssert( #exp, __FILE__, __LINE__, msg )

#else
#define ASSERT( exp )
#define ASSERTMSG( msg, exp )
#endif // DBG

//
//  Doubly-linked list manipulation routines.  Implemented as macros
//  but logically these are procedures.
//

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}

//
//  PLIST_ENTRY
//  RemoveTailList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

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

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }

//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)

//
// Subroutines for dealing with the Registry
//

typedef NTSTATUS (*PRTL_QUERY_REGISTRY_ROUTINE)(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

typedef struct _RTL_QUERY_REGISTRY_TABLE {
    PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
    ULONG Flags;
    PWSTR Name;
    PVOID EntryContext;
    ULONG DefaultType;
    PVOID DefaultData;
    ULONG DefaultLength;

} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;


//
// The following flags specify how the Name field of a RTL_QUERY_REGISTRY_TABLE
// entry is interpreted.  A NULL name indicates the end of the table.
//

#define RTL_QUERY_REGISTRY_SUBKEY   0x00000001  // Name is a subkey and remainder of
                                                // table or until next subkey are value
                                                // names for that subkey to look at.

#define RTL_QUERY_REGISTRY_TOPKEY   0x00000002  // Reset current key to original key for
                                                // this and all following table entries.

#define RTL_QUERY_REGISTRY_REQUIRED 0x00000004  // Fail if no match found for this table
                                                // entry.

#define RTL_QUERY_REGISTRY_NOVALUE  0x00000008  // Used to mark a table entry that has no
                                                // value name, just wants a call out, not
                                                // an enumeration of all values.

#define RTL_QUERY_REGISTRY_NOEXPAND 0x00000010  // Used to suppress the expansion of
                                                // REG_MULTI_SZ into multiple callouts or
                                                // to prevent the expansion of environment
                                                // variable values in REG_EXPAND_SZ

#define RTL_QUERY_REGISTRY_DIRECT   0x00000020  // QueryRoutine field ignored.  EntryContext
                                                // field points to location to store value.
                                                // For null terminated strings, EntryContext
                                                // points to UNICODE_STRING structure that
                                                // that describes maximum size of buffer.
                                                // If .Buffer field is NULL then a buffer is
                                                // allocated.
                                                //

#define RTL_QUERY_REGISTRY_DELETE   0x00000040  // Used to delete value keys after they
                                                // are queried.

NTSTATUS
NTAPI
RtlQueryRegistryValues(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PRTL_QUERY_REGISTRY_TABLE QueryTable,
    IN PVOID Context,
    IN PVOID Environment OPTIONAL
    );

NTSTATUS
NTAPI
RtlWriteRegistryValue(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength
    );

NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PWSTR ValueName
    );

NTSTATUS
NTAPI
RtlCreateRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

NTSTATUS
NTAPI
RtlCheckRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

//
// The following values for the RelativeTo parameter determine what the
// Path parameter to RtlQueryRegistryValues is relative to.
//

#define RTL_REGISTRY_ABSOLUTE     0   // Path is a full path
#define RTL_REGISTRY_SERVICES     1   // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL      2   // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT   3   // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP    4   // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER         5   // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM      6
#define RTL_REGISTRY_HANDLE       0x40000000    // Low order bits are registry handle
#define RTL_REGISTRY_OPTIONAL     0x80000000    // Indicates the key node is optional

NTSTATUS                                            
RtlIntegerToUnicodeString (                         
    ULONG Value,                                    
    ULONG Base,                                     
    PUNICODE_STRING String                          
    );                                              
//
//  String manipulation routines
//

VOID
NTAPI
RtlInitString(
    PSTRING DestinationString,
    PCSZ SourceString
    );

VOID
NTAPI
RtlInitAnsiString(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

VOID
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );


VOID
NTAPI
RtlCopyString(
    PSTRING DestinationString,
    PSTRING SourceString
    );


BOOLEAN
NTAPI
RtlEqualString(
    PSTRING String1,
    PSTRING String2,
    BOOLEAN CaseInSensitive
    );


VOID
NTAPI
RtlUpperString(
    PSTRING DestinationString,
    PSTRING SourceString
    );

//
// NLS String functions
//

NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


LONG
NTAPI
RtlCompareUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );


BOOLEAN
NTAPI
RtlEqualUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );


NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );


VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PUNICODE_STRING SourceString
    );

NTSTATUS
NTAPI
RtlAppendUnicodeStringToString (
    PUNICODE_STRING Destination,
    PUNICODE_STRING Source
    );

NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    PUNICODE_STRING Destination,
    PWSTR Source
    );


VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
    );

VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
    );


ULONG
NTAPI
RtlAnsiStringToUnicodeSize(
    PANSI_STRING AnsiString
    );

//
// Fast primitives to compare, move, and zero memory
//

// begin_winnt
#ifdef _M_IX86
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#else
#define RtlCopyMemory(Destination,Source,Length) RtlMoveMemory((Destination),(Source),(Length))
VOID
NTAPI
RtlMoveMemory (
   PVOID Destination,
   CONST VOID *Source,
   ULONG Length
   );

VOID
NTAPI
RtlFillMemory (
   PVOID Destination,
   ULONG Length,
   UCHAR Fill
   );

VOID
NTAPI
RtlZeroMemory (
   PVOID Destination,
   ULONG Length
   );
#endif
// end_winnt

ULONG
NTAPI
RtlCompareMemory (
    PVOID Source1,
    PVOID Source2,
    ULONG Length
    );


VOID                                        
DbgBreakPoint(                              
    VOID                                    
    );                                      
//
// Define kernel debugger print prototypes and macros.
//

#if DBG

#define KdPrint(_x_) DbgPrint _x_

#else

#define KdPrint(_x_)

#endif

#ifndef _DBGNT_
ULONG
DbgPrint(
    PCH Format,
    ...
    );
#endif // _DBGNT_
//
// Large integer arithmetic routines.
//

//
// Large integer add - 64-bits + 64-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlLargeIntegerAdd (
    LARGE_INTEGER Addend1,
    LARGE_INTEGER Addend2
    );

//
// Enlarged integer multiply - 32-bits * 32-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlEnlargedIntegerMultiply (
    LONG Multiplicand,
    LONG Multiplier
    );

//
// Unsigned enlarged integer multiply - 32-bits * 32-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlEnlargedUnsignedMultiply (
    ULONG Multiplicand,
    ULONG Multiplier
    );

//
// Enlarged integer divide - 64-bits / 32-bits > 32-bits
//

ULONG
NTAPI
RtlEnlargedUnsignedDivide (
    IN ULARGE_INTEGER Dividend,
    IN ULONG Divisor,
    IN PULONG Remainder
    );

//
// Extended large integer magic divide - 64-bits / 32-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlExtendedMagicDivide (
    LARGE_INTEGER Dividend,
    LARGE_INTEGER MagicDivisor,
    CCHAR ShiftCount
    );

//
// Large Integer divide - 64-bits / 32-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlExtendedLargeIntegerDivide (
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder
    );

//
// Large Integer divide - 64-bits / 32-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlLargeIntegerDivide (
    LARGE_INTEGER Dividend,
    LARGE_INTEGER Divisor,
    PLARGE_INTEGER Remainder
    );

//
// Extended integer multiply - 32-bits * 64-bits -> 64-bits
//

LARGE_INTEGER
NTAPI
RtlExtendedIntegerMultiply (
    LARGE_INTEGER Multiplicand,
    LONG Multiplier
    );

//
// Large integer negation - -(64-bits)
//

LARGE_INTEGER
NTAPI
RtlLargeIntegerNegate (
    LARGE_INTEGER Subtrahend
    );

//
// Large integer subtract - 64-bits - 64-bits -> 64-bits.
//

LARGE_INTEGER
NTAPI
RtlLargeIntegerSubtract (
    LARGE_INTEGER Minuend,
    LARGE_INTEGER Subtrahend
    );

//
// Large integer and - 64-bite & 64-bits -> 64-bits.
//

#define RtlLargeIntegerAnd(Result, Source, Mask)   \
        {                                           \
            Result.HighPart = Source.HighPart & Mask.HighPart; \
            Result.LowPart = Source.LowPart & Mask.LowPart; \
        }


//
// Large integer conversion routines.
//

//
// Convert signed integer to large integer.
//

LARGE_INTEGER
NTAPI
RtlConvertLongToLargeInteger (
    LONG SignedInteger
    );

//
// Convert unsigned integer to large integer.
//

LARGE_INTEGER
NTAPI
RtlConvertUlongToLargeInteger (
    ULONG UnsignedInteger
    );


//
// Large integer shift routines.
//

LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftLeft (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

LARGE_INTEGER
NTAPI
RtlLargeIntegerShiftRight (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

LARGE_INTEGER
NTAPI
RtlLargeIntegerArithmeticShift (
    LARGE_INTEGER LargeInteger,
    CCHAR ShiftCount
    );

//
// Large integer comparison routines.
//
// BOOLEAN
// RtlLargeIntegerGreaterThan (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterThanOrEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerNotEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThan (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThanOrEqualTo (
//     LARGE_INTEGER Operand1,
//     LARGE_INTEGER Operand2
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterThanZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerGreaterOrEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerNotEqualToZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerLessThanZero (
//     LARGE_INTEGER Operand
//     );
//
// BOOLEAN
// RtlLargeIntegerLessOrEqualToZero (
//     LARGE_INTEGER Operand
//     );
//

#define RtlLargeIntegerGreaterThan(X,Y) (                              \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart > (Y).LowPart)) || \
    ((X).HighPart > (Y).HighPart)                                      \
)

#define RtlLargeIntegerGreaterThanOrEqualTo(X,Y) (                      \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart >= (Y).LowPart)) || \
    ((X).HighPart > (Y).HighPart)                                       \
)

#define RtlLargeIntegerEqualTo(X,Y) (                              \
    !(((X).LowPart ^ (Y).LowPart) | ((X).HighPart ^ (Y).HighPart)) \
)

#define RtlLargeIntegerNotEqualTo(X,Y) (                          \
    (((X).LowPart ^ (Y).LowPart) | ((X).HighPart ^ (Y).HighPart)) \
)

#define RtlLargeIntegerLessThan(X,Y) (                                 \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart < (Y).LowPart)) || \
    ((X).HighPart < (Y).HighPart)                                      \
)

#define RtlLargeIntegerLessThanOrEqualTo(X,Y) (                         \
    (((X).HighPart == (Y).HighPart) && ((X).LowPart <= (Y).LowPart)) || \
    ((X).HighPart < (Y).HighPart)                                       \
)

#define RtlLargeIntegerGreaterThanZero(X) (       \
    (((X).HighPart == 0) && ((X).LowPart > 0)) || \
    ((X).HighPart > 0 )                           \
)

#define RtlLargeIntegerGreaterOrEqualToZero(X) ( \
    (X).HighPart >= 0                            \
)

#define RtlLargeIntegerEqualToZero(X) ( \
    !((X).LowPart | (X).HighPart)       \
)

#define RtlLargeIntegerNotEqualToZero(X) ( \
    ((X).LowPart | (X).HighPart)           \
)

#define RtlLargeIntegerLessThanZero(X) ( \
    ((X).HighPart < 0)                   \
)

#define RtlLargeIntegerLessOrEqualToZero(X) (           \
    ((X).HighPart < 0) || !((X).LowPart | (X).HighPart) \
)


//
//  Time conversion routines
//

typedef struct _TIME_FIELDS {
    CSHORT Year;        // range [1601...]
    CSHORT Month;       // range [1..12]
    CSHORT Day;         // range [1..31]
    CSHORT Hour;        // range [0..23]
    CSHORT Minute;      // range [0..59]
    CSHORT Second;      // range [0..59]
    CSHORT Milliseconds;// range [0..999]
    CSHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS;
typedef TIME_FIELDS *PTIME_FIELDS;


VOID
NTAPI
RtlTimeToTimeFields (
    PLARGE_INTEGER Time,
    PTIME_FIELDS TimeFields
    );

//
// The following macros store and retrieve USHORTS and ULONGS from potentially
// unaligned addresses, avoiding alignment faults.  they should probably be
// rewritten in assembler
//

#define SHORT_SIZE  (sizeof(USHORT))
#define SHORT_MASK  (SHORT_SIZE - 1)
#define LONG_SIZE       (sizeof(LONG))
#define LONG_MASK       (LONG_SIZE - 1)
#define LOWBYTE_MASK 0x00FF

#define FIRSTBYTE(VALUE)  (VALUE & LOWBYTE_MASK)
#define SECONDBYTE(VALUE) ((VALUE >> 8) & LOWBYTE_MASK)
#define THIRDBYTE(VALUE)  ((VALUE >> 16) & LOWBYTE_MASK)
#define FOURTHBYTE(VALUE) ((VALUE >> 24) & LOWBYTE_MASK)

//
// if MIPS Big Endian, order of bytes is reversed.
//

#define SHORT_LEAST_SIGNIFICANT_BIT  0
#define SHORT_MOST_SIGNIFICANT_BIT   1

#define LONG_LEAST_SIGNIFICANT_BIT       0
#define LONG_3RD_MOST_SIGNIFICANT_BIT    1
#define LONG_2ND_MOST_SIGNIFICANT_BIT    2
#define LONG_MOST_SIGNIFICANT_BIT        3

//++
//
// VOID
// RtlStoreUshort (
//     PUSHORT ADDRESS
//     USHORT VALUE
//     )
//
// Routine Description:
//
// This macro stores a USHORT value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store USHORT value
//     VALUE - USHORT to store
//
// Return Value:
//
//     none.
//
//--

#define RtlStoreUshort(ADDRESS,VALUE)                     \
         if ((ULONG)ADDRESS & SHORT_MASK) {               \
             ((PUCHAR) ADDRESS)[SHORT_LEAST_SIGNIFICANT_BIT] = (UCHAR)(FIRSTBYTE(VALUE));    \
             ((PUCHAR) ADDRESS)[SHORT_MOST_SIGNIFICANT_BIT ] = (UCHAR)(SECONDBYTE(VALUE));   \
         }                                                \
         else {                                           \
             *((PUSHORT) ADDRESS) = (USHORT) VALUE;       \
         }


//++
//
// VOID
// RtlStoreUlong (
//     PULONG ADDRESS
//     ULONG VALUE
//     )
//
// Routine Description:
//
// This macro stores a ULONG value in at a particular address, avoiding
// alignment faults.
//
// Arguments:
//
//     ADDRESS - where to store ULONG value
//     VALUE - ULONG to store
//
// Return Value:
//
//     none.
//
// Note:
//     Depending on the machine, we might want to call storeushort in the
//     unaligned case.
//
//--

#define RtlStoreUlong(ADDRESS,VALUE)                      \
         if ((ULONG)ADDRESS & LONG_MASK) {                \
             ((PUCHAR) ADDRESS)[LONG_LEAST_SIGNIFICANT_BIT      ] = (UCHAR)(FIRSTBYTE(VALUE));    \
             ((PUCHAR) ADDRESS)[LONG_3RD_MOST_SIGNIFICANT_BIT   ] = (UCHAR)(SECONDBYTE(VALUE));   \
             ((PUCHAR) ADDRESS)[LONG_2ND_MOST_SIGNIFICANT_BIT   ] = (UCHAR)(THIRDBYTE(VALUE));    \
             ((PUCHAR) ADDRESS)[LONG_MOST_SIGNIFICANT_BIT       ] = (UCHAR)(FOURTHBYTE(VALUE));   \
         }                                                \
         else {                                           \
             *((PULONG) ADDRESS) = (ULONG) VALUE;         \
         }

//++
//
// VOID
// RtlRetrieveUshort (
//     PUSHORT DESTINATION_ADDRESS
//     PUSHORT SOURCE_ADDRESS
//     )
//
// Routine Description:
//
// This macro retrieves a USHORT value from the SOURCE address, avoiding
// alignment faults.  The DESTINATION address is assumed to be aligned.
//
// Arguments:
//
//     DESTINATION_ADDRESS - where to store USHORT value
//     SOURCE_ADDRESS - where to retrieve USHORT value from
//
// Return Value:
//
//     none.
//
//--

#define RtlRetrieveUshort(DEST_ADDRESS,SRC_ADDRESS)                   \
         if ((ULONG)SRC_ADDRESS & SHORT_MASK) {                       \
             ((PUCHAR) DEST_ADDRESS)[0] = ((PUCHAR) SRC_ADDRESS)[0];  \
             ((PUCHAR) DEST_ADDRESS)[1] = ((PUCHAR) SRC_ADDRESS)[1];  \
         }                                                            \
         else {                                                       \
             *((PUSHORT) DEST_ADDRESS) = *((PUSHORT) SRC_ADDRESS);    \
         }                                                            \

//++
//
// VOID
// RtlRetrieveUlong (
//     PULONG DESTINATION_ADDRESS
//     PULONG SOURCE_ADDRESS
//     )
//
// Routine Description:
//
// This macro retrieves a ULONG value from the SOURCE address, avoiding
// alignment faults.  The DESTINATION address is assumed to be aligned.
//
// Arguments:
//
//     DESTINATION_ADDRESS - where to store ULONG value
//     SOURCE_ADDRESS - where to retrieve ULONG value from
//
// Return Value:
//
//     none.
//
// Note:
//     Depending on the machine, we might want to call retrieveushort in the
//     unaligned case.
//
//--

#define RtlRetrieveUlong(DEST_ADDRESS,SRC_ADDRESS)                    \
         if ((ULONG)SRC_ADDRESS & LONG_MASK) {                        \
             ((PUCHAR) DEST_ADDRESS)[0] = ((PUCHAR) SRC_ADDRESS)[0];  \
             ((PUCHAR) DEST_ADDRESS)[1] = ((PUCHAR) SRC_ADDRESS)[1];  \
             ((PUCHAR) DEST_ADDRESS)[2] = ((PUCHAR) SRC_ADDRESS)[2];  \
             ((PUCHAR) DEST_ADDRESS)[3] = ((PUCHAR) SRC_ADDRESS)[3];  \
         }                                                            \
         else {                                                       \
             *((PULONG) DEST_ADDRESS) = *((PULONG) SRC_ADDRESS);      \
         }
//
//  SecurityDescriptor RTL routine definitions
//

NTSTATUS
NTAPI
RtlCreateSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    ULONG Revision
    );

NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor (
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN DaclPresent,
    PACL Dacl,
    BOOLEAN DaclDefaulted
    );

//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//

#define DEVICE_TYPE ULONG

#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//


#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe


// begin_winnt

//
// Define access rights to files and directories
//

//
// The FILE_READ_DATA and FILE_WRITE_DATA constants are also defined in
// devioctl.h as FILE_READ_ACCESS and FILE_WRITE_ACCESS. The values for these
// constants *MUST* always be in sync.
// The values are redefined in devioctl.h because they must be available to
// both DOS and NT.
//

#define FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define FILE_LIST_DIRECTORY       ( 0x0001 )    // directory

#define FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define FILE_ADD_FILE             ( 0x0002 )    // directory

#define FILE_APPEND_DATA          ( 0x0004 )    // file
#define FILE_ADD_SUBDIRECTORY     ( 0x0004 )    // directory
#define FILE_CREATE_PIPE_INSTANCE ( 0x0004 )    // named pipe

#define FILE_READ_EA              ( 0x0008 )    // file & directory

#define FILE_WRITE_EA             ( 0x0010 )    // file & directory

#define FILE_EXECUTE              ( 0x0020 )    // file
#define FILE_TRAVERSE             ( 0x0020 )    // directory

#define FILE_DELETE_CHILD         ( 0x0040 )    // directory

#define FILE_READ_ATTRIBUTES      ( 0x0080 )    // all

#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // all

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)


#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)

// end_winnt


//
// Define share access rights to files and directories
//

#define FILE_SHARE_READ                 0x00000001  // winnt
#define FILE_SHARE_WRITE                0x00000002  // winnt
#define FILE_SHARE_DELETE               0x00000004
#define FILE_SHARE_VALID_FLAGS          0x00000007

//
// Define the file attributes values
//
// Note:  0x00000008 is reserved for use for the old DOS VOLID (volume ID)
//        and is therefore not considered valid in NT.
//
// Note:  0x00000010 is reserved for use for the old DOS SUBDIRECTORY flag
//        and is therefore not considered valid in NT.  This flag has
//        been disassociated with file attributes since the other flags are
//        protected with READ_ and WRITE_ATTRIBUTES access to the file.
//
// Note:  Note also that the order of these flags is set to allow both the
//        FAT and the Pinball File Systems to directly set the attributes
//        flags in attributes words without having to pick each flag out
//        individually.  The order of these flags should not be changed!
//

#define FILE_ATTRIBUTE_READONLY         0x00000001  // winnt
#define FILE_ATTRIBUTE_HIDDEN           0x00000002  // winnt
#define FILE_ATTRIBUTE_SYSTEM           0x00000004  // winnt
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010  // winnt
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020  // winnt
#define FILE_ATTRIBUTE_NORMAL           0x00000080  // winnt
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100  // winnt
#define FILE_ATTRIBUTE_ATOMIC_WRITE     0x00000200  // winnt
#define FILE_ATTRIBUTE_XACTION_WRITE    0x00000400  // winnt
#define FILE_ATTRIBUTE_VALID_FLAGS      0x000007b7

//
// Define the create disposition values
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Define the create/open option flags
//

#define FILE_DIRECTORY_FILE              0x00000001
#define FILE_WRITE_THROUGH               0x00000002
#define FILE_SEQUENTIAL_ONLY             0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING   0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT        0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT     0x00000020
#define FILE_NON_DIRECTORY_FILE          0x00000040
#define FILE_CREATE_TREE_CONNECTION      0x00000080
#define FILE_COMPLETE_IF_OPLOCKED        0x00000100
#define FILE_NO_EA_KNOWLEDGE             0x00000200
#define FILE_EIGHT_DOT_THREE_ONLY        0x00000400
#define FILE_RANDOM_ACCESS               0x00000800
#define FILE_DELETE_ON_CLOSE             0x00001000
#define FILE_OPEN_BY_FILE_ID             0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT      0x00004000
#define FILE_VALID_OPTION_FLAGS          0x00007FFF
#define FILE_VALID_PIPE_OPTION_FLAGS     0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS 0x00000032
#define FILE_VALID_SET_FLAGS             0x00001036

//
// Define the I/O status information return values for NtCreateFile/NtOpenFile
//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

//
// Define special ByteOffset parameters for read and write operations
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

//
// Define alignment requirement values
//

#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

//
// Define the maximum length of a filename string
//

#define MAXIMUM_FILENAME_LENGTH         256

//
// Define the various device characteristics flags
//

#define FILE_REMOVABLE_MEDIA            0x00000001
#define FILE_READ_ONLY_DEVICE           0x00000002
#define FILE_FLOPPY_DISKETTE            0x00000004
#define FILE_WRITE_ONCE_MEDIA           0x00000008
#define FILE_REMOTE_DEVICE              0x00000010
#define FILE_DEVICE_IS_MOUNTED          0x00000020

//
// Define the base asynchronous I/O argument types
//

typedef struct _IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

//
// Define an Asynchronous Procedure Call from I/O viewpoint
//

typedef
VOID
(*PIO_APC_ROUTINE) (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    );

//
// Define the file information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.
//

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

//
// Define the various structures which are returned on query operations
//

typedef struct _FILE_BASIC_INFORMATION {                    
    LARGE_INTEGER CreationTime;                             
    LARGE_INTEGER LastAccessTime;                           
    LARGE_INTEGER LastWriteTime;                            
    LARGE_INTEGER ChangeTime;                               
    ULONG FileAttributes;                                   
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         
                                                            
typedef struct _FILE_STANDARD_INFORMATION {                 
    LARGE_INTEGER AllocationSize;                           
    LARGE_INTEGER EndOfFile;                                
    ULONG NumberOfLinks;                                    
    BOOLEAN DeletePending;                                  
    BOOLEAN Directory;                                      
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;   
                                                            
typedef struct _FILE_POSITION_INFORMATION {                 
    LARGE_INTEGER CurrentByteOffset;                        
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;   
                                                            

typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

//
// Define the file system information class values
//
// WARNING:  The order of the following values are assumed by the I/O system.
//           Any changes made here should be reflected there as well.

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_FS_DEVICE_INFORMATION {                    
    DEVICE_TYPE DeviceType;                                     
    ULONG Characteristics;                                      
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;     
                                                                
//
// Define the I/O bus interface types.
//

typedef enum _INTERFACE_TYPE {
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

//
// Define the DMA transfer widths.
//

typedef enum _DMA_WIDTH {
    Width8Bits,
    Width16Bits,
    Width32Bits,
    MaximumDmaWidth
}DMA_WIDTH, *PDMA_WIDTH;

//
// Define DMA transfer speeds.
//

typedef enum _DMA_SPEED {
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    MaximumDmaSpeed
}DMA_SPEED, *PDMA_SPEED;

//
// Define I/O Driver error log packet structure.  This structure is filled in
// by the driver.
//

typedef struct _IO_ERROR_LOG_PACKET {
    UCHAR MajorFunctionCode;
    UCHAR RetryCount;
    USHORT DumpDataSize;
    USHORT NumberOfStrings;
    USHORT StringOffset;
    USHORT EventCategory;
    NTSTATUS ErrorCode;
    ULONG UniqueErrorValue;
    NTSTATUS FinalStatus;
    ULONG SequenceNumber;
    ULONG IoControlCode;
    LARGE_INTEGER DeviceOffset;
    ULONG DumpData[1];
}IO_ERROR_LOG_PACKET, *PIO_ERROR_LOG_PACKET;

//
// Define the I/O error log message.  This message is sent by the error log
// thread over the lpc port.
//

typedef struct _IO_ERROR_LOG_MESSAGE {
    USHORT Type;
    USHORT Size;
    USHORT DriverNameLength;
    LARGE_INTEGER TimeStamp;
    ULONG DriverNameOffset;
    IO_ERROR_LOG_PACKET EntryData;
}IO_ERROR_LOG_MESSAGE, *PIO_ERROR_LOG_MESSAGE;

//
// Define the maximum message size that will be sent over the LPC to the
// application reading the error log entries.
//

#define IO_ERROR_LOG_MESSAGE_LENGTH  PORT_MAXIMUM_MESSAGE_LENGTH

//
// Define the maximum packet size a driver can allocate.
//

#define ERROR_LOG_MAXIMUM_SIZE (IO_ERROR_LOG_MESSAGE_LENGTH + sizeof(IO_ERROR_LOG_PACKET) -    \
                        sizeof(IO_ERROR_LOG_MESSAGE) - (sizeof(WCHAR) * 40))

#define PORT_MAXIMUM_MESSAGE_LENGTH 256
//
// Registry Specific Access Rights.
//

#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

//
// Open/Create Options
//

#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved

#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved
                                                    // when system is rebooted

#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved
                                                    // when system is rebooted

#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a
                                                    // symbolic link

#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore
                                                    // special access rules
                                                    // privilege required

#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
                 REG_OPTION_BACKUP_RESTORE)

//
// Key creation/open disposition
//

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

//
// Key restore flags
//

#define REG_WHOLE_HIVE_VOLATILE     (0x00000001L)   // Restore whole hive volatile
#define REG_REFRESH_HIVE            (0x00000002L)   // Unwind changes to last flush

//
// Key query structures
//

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation
} KEY_SET_INFORMATION_CLASS;

//
// Value entry query structures
//

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation
} KEY_VALUE_INFORMATION_CLASS;


#define OBJ_NAME_PATH_SEPARATOR ((WCHAR)L'\\')

//
// Object Manager Object Type Specific Access Rights.
//

#define OBJECT_TYPE_CREATE (0x0001)

#define OBJECT_TYPE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

//
// Object Manager Directory Specific Access Rights.
//

#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

//
// Object Manager Symbolic Link Specific Access Rights.
//

#define SYMBOLIC_LINK_QUERY (0x0001)

#define SYMBOLIC_LINK_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

typedef struct _OBJECT_NAME_INFORMATION {               
    UNICODE_STRING Name;                                
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;   

//
// Section Information Structures.
//

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

//
// Section Access Rights.
//

// begin_winnt
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
// end_winnt

#define SEGMENT_ALL_ACCESS SECTION_ALL_ACCESS

#define PAGE_NOACCESS          0x01     // winnt
#define PAGE_READONLY          0x02     // winnt
#define PAGE_READWRITE         0x04     // winnt
#define PAGE_WRITECOPY         0x08     // winnt
#define PAGE_EXECUTE           0x10     // winnt
#define PAGE_EXECUTE_READ      0x20     // winnt
#define PAGE_EXECUTE_READWRITE 0x40     // winnt
#define PAGE_EXECUTE_WRITECOPY 0x80     // winnt
#define PAGE_GUARD            0x100     // winnt
#define PAGE_NOCACHE          0x200     // winnt

#define MEM_TOP_DOWN       0x100000     
#define MEM_LARGE_PAGES  0x20000000     
#define THREAD_TERMINATE               (0x0001)  // winnt
#define THREAD_SET_INFORMATION         (0x0020)  // winnt

#define THREAD_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3FF)

//
// ClientId
//

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

#define NtCurrentProcess() ( (HANDLE) -1 )

#if defined(_X86_)
//
// Indicate that the i386 compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1
#define NORMAL_DISPATCH_LENGTH 106                  
#define DISPATCH_LENGTH NORMAL_DISPATCH_LENGTH      
//
// Interrupt Request Level definitions
//

#define PASSIVE_LEVEL 0             // Passive release level
#define LOW_LEVEL 0                 // Lowest interrupt level
#define APC_LEVEL 1                 // APC interrupt level
#define DISPATCH_LEVEL 2            // Dispatcher level

#define PROFILE_LEVEL 27            // timer used for profiling.
#define CLOCK1_LEVEL 28             // Interval clock 1 level - Not used on x86
#define CLOCK2_LEVEL 28             // Interval clock 2 level
#define IPI_LEVEL 29                // Interprocessor interrupt level
#define POWER_LEVEL 30              // Power failure level
#define HIGH_LEVEL 31               // Highest interrupt level

//
// Get data cache fill size.
//

#define KeGetDcacheFillSize() 1L


#define KeFlushIoBuffers(Mdl, ReadOperation, DmaOperation)

//
// i386 Specific portions of mm component
//

//
// Define the page size for the Intel 386 as 4096 (0x1000).
//

#define PAGE_SIZE (ULONG)0x1000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

//
// The highest user address reserves 64K bytes for a guard
// page.  This allows the probing of address from kernel mode to
// only have to check the starting address for strucutures of 64k bytes
// or less.
//

#define MM_HIGHEST_USER_ADDRESS (PVOID)0x7FFEFFFF // temp should be 0xBFFEFFFF

#define MM_USER_PROBE_ADDRESS 0x7FFF0000    // starting address of guard page

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)0x00010000

//
// The lowest address for system space.
//

#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000

//
// Result type definition for i386.  (Machine specific enumerate type
// which is return type for portable exinterlockedincrement/decrement
// procedures.)  In general, you should use the enumerated type defined
// in ex.h instead of directly referencing these constants.
//

// Flags loaded into AH by LAHF instruction

#define EFLAG_SIGN      0x8000
#define EFLAG_ZERO      0x4000
#define EFLAG_SELECT    (EFLAG_SIGN | EFLAG_ZERO)

#define RESULT_NEGATIVE ((EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_ZERO     ((~EFLAG_SIGN & EFLAG_ZERO) & EFLAG_SELECT)
#define RESULT_POSITIVE ((~EFLAG_SIGN & ~EFLAG_ZERO) & EFLAG_SELECT)

//
// Convert various portable ExInterlock apis into their architecural
// equivalents.
//

#define ExInterlockedIncrementLong(Addend,Lock) \
        Exi386InterlockedIncrementLong(Addend)

#define ExInterlockedDecrementLong(Addend,Lock) \
        Exi386InterlockedDecrementLong(Addend)

#define ExInterlockedExchangeUlong(Target,Value,Lock) \
        Exi386InterlockedExchangeUlong(Target,Value)

//
// Prototypes for architecural specific versions of Exi386 api
//

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for value are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;

INTERLOCKED_RESULT
Exi386InterlockedIncrementLong (
    IN PLONG Addend
    );

INTERLOCKED_RESULT
Exi386InterlockedDecrementLong (
    IN PLONG Addend
    );

ULONG
Exi386InterlockedExchangeUlong (
    IN PULONG Target,
    IN ULONG Value
    );

VOID
KeRaiseIrql (
    IN KIRQL NewIrql,
    OUT PKIRQL OldIrql
    );

VOID
KeLowerIrql (
    IN KIRQL NewIrql
    );


#endif // defined(_X86_)


#if defined(_MIPS_)

//
// Indicate that the MIPS compiler supports the pragma textout construct.
//

#define ALLOC_PRAGMA 1

//
// MIPS specific interlocked operation result values.
//

#define RESULT_ZERO 0
#define RESULT_NEGATIVE -2
#define RESULT_POSITIVE -1

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for value are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;

//
// MIPS Interrupt Definitions.
//
// Define length on interupt object dispatch code in longwords.
//

#define DISPATCH_LENGTH 4               // Length of dispatch code in instructions

//
// Define Interrupt Request Levels.
//

#define PASSIVE_LEVEL 0                 // Passive release level
#define LOW_LEVEL 0                     // Lowest interrupt level
#define APC_LEVEL 1                     // APC interrupt level
#define DISPATCH_LEVEL 2                // Dispatcher level
#define IPI_LEVEL 7                     // Interprocessor interrupt level
#define POWER_LEVEL 7                   // Power failure level
#define FLOAT_LEVEL 8                   // Floating interrupt level
#define HIGH_LEVEL 8                    // Highest interrupt level

#if defined(R3000)

#define PROFILE_LEVEL 7                 // Profiling level

#endif

#if defined(R4000)

#define PROFILE_LEVEL 8                 // Profiling level

#endif

//
// Define profile intervals.
//

#if defined(R3000)

#define DEFAULT_PROFILE_INTERVAL (10 * 1000)  // 1 millisecond
#define MAXIMUM_PROFILE_INTERVAL (10 * 1000 * 1000) // 1 second
#define MINIMUM_PROFILE_INTERVAL (10 * 1000) // 1 millisecond

#endif

#if defined(R4000)

#define DEFAULT_PROFILE_COUNT 0x40000000 // ~= 20 seconds @50mhz
#define DEFAULT_PROFILE_INTERVAL (10 * 500) // 500 microseconds
#define MAXIMUM_PROFILE_INTERVAL (10 * 1000 * 1000) // 1 second
#define MINIMUM_PROFILE_INTERVAL (10 * 40) // 40 microseconds

#endif


//
// Get data cache fill size.
//

#define KeGetDcacheFillSize() PCR->DcacheFillSize

//
// Cache and write buffer flush functions.
//

VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );


VOID
KeLowerIrql (
    KIRQL NewIrql
    );

VOID
KeRaiseIrql (
    KIRQL NewIrql,
    PKIRQL OldIrql
    );

//
// I/O space read and write macros.
//

#define READ_REGISTER_UCHAR(x) \
    *(volatile UCHAR * const)(x)

#define READ_REGISTER_USHORT(x) \
    *(volatile USHORT * const)(x)

#define READ_REGISTER_ULONG(x) \
    *(volatile ULONG * const)(x)

#define READ_REGISTER_BUFFER_UCHAR(x, y, z) {                           \
    PUCHAR registerBuffer = x;                                          \
    PUCHAR readBuffer = y;                                              \
    ULONG readCount;                                                    \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile UCHAR * const)(registerBuffer);        \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_USHORT(x, y, z) {                          \
    PUSHORT registerBuffer = x;                                         \
    PUSHORT readBuffer = y;                                             \
    ULONG readCount;                                                    \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile USHORT * const)(registerBuffer);       \
    }                                                                   \
}

#define READ_REGISTER_BUFFER_ULONG(x, y, z) {                           \
    PULONG registerBuffer = x;                                          \
    PULONG readBuffer = y;                                              \
    ULONG readCount;                                                    \
    for (readCount = z; readCount--; readBuffer++, registerBuffer++) {  \
        *readBuffer = *(volatile ULONG * const)(registerBuffer);        \
    }                                                                   \
}

#define WRITE_REGISTER_UCHAR(x, y) {    \
    *(volatile UCHAR * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_USHORT(x, y) {   \
    *(volatile USHORT * const)(x) = y;  \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_ULONG(x, y) {    \
    *(volatile ULONG * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_REGISTER_BUFFER_UCHAR(x, y, z) {                            \
    PUCHAR registerBuffer = x;                                            \
    PUCHAR writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile UCHAR * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_USHORT(x, y, z) {                           \
    PUSHORT registerBuffer = x;                                           \
    PUSHORT writeBuffer = y;                                              \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile USHORT * const)(registerBuffer) = *writeBuffer;        \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}

#define WRITE_REGISTER_BUFFER_ULONG(x, y, z) {                            \
    PULONG registerBuffer = x;                                            \
    PULONG writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = z; writeCount--; writeBuffer++, registerBuffer++) { \
        *(volatile ULONG * const)(registerBuffer) = *writeBuffer;         \
    }                                                                     \
    KeFlushWriteBuffer();                                                 \
}


#define READ_PORT_UCHAR(x) \
    *(volatile UCHAR * const)(x)

#define READ_PORT_USHORT(x) \
    *(volatile USHORT * const)(x)

#define READ_PORT_ULONG(x) \
    *(volatile ULONG * const)(x)

#define READ_PORT_BUFFER_UCHAR(x, y, z) {                             \
    PUCHAR readBuffer = y;                                            \
    ULONG readCount;                                                  \
    for (readCount = 0; readCount < z; readCount++, readBuffer++) {   \
        *readBuffer = *(volatile UCHAR * const)(x);                   \
    }                                                                 \
}

#define READ_PORT_BUFFER_USHORT(x, y, z) {                            \
    PUSHORT readBuffer = y;                                            \
    ULONG readCount;                                                  \
    for (readCount = 0; readCount < z; readCount++, readBuffer++) {   \
        *readBuffer = *(volatile USHORT * const)(x);                  \
    }                                                                 \
}

#define READ_PORT_BUFFER_ULONG(x, y, z) {                             \
    PULONG readBuffer = y;                                            \
    ULONG readCount;                                                  \
    for (readCount = 0; readCount < z; readCount++, readBuffer++) {   \
        *readBuffer = *(volatile ULONG * const)(x);                   \
    }                                                                 \
}

#define WRITE_PORT_UCHAR(x, y) {        \
    *(volatile UCHAR * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_PORT_USHORT(x, y) {       \
    *(volatile USHORT * const)(x) = y;  \
    KeFlushWriteBuffer();               \
}

#define WRITE_PORT_ULONG(x, y) {        \
    *(volatile ULONG * const)(x) = y;   \
    KeFlushWriteBuffer();               \
}

#define WRITE_PORT_BUFFER_UCHAR(x, y, z) {                                \
    PUCHAR writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = 0; writeCount < z; writeCount++, writeBuffer++) {   \
        *(volatile UCHAR * const)(x) = *writeBuffer;                      \
        KeFlushWriteBuffer();                                             \
    }                                                                     \
}

#define WRITE_PORT_BUFFER_USHORT(x, y, z) {                               \
    PUSHORT writeBuffer = y;                                              \
    ULONG writeCount;                                                     \
    for (writeCount = 0; writeCount < z; writeCount++, writeBuffer++) {   \
        *(volatile USHORT * const)(x) = *writeBuffer;                     \
        KeFlushWriteBuffer();                                             \
    }                                                                     \
}

#define WRITE_PORT_BUFFER_ULONG(x, y, z) {                                \
    PULONG writeBuffer = y;                                               \
    ULONG writeCount;                                                     \
    for (writeCount = 0; writeCount < z; writeCount++, writeBuffer++) {   \
        *(volatile ULONG * const)(x) = *writeBuffer;                      \
        KeFlushWriteBuffer();                                             \
    }                                                                     \
}

//
// Define the page size for the MIPS r3000 and r4000 as 4096 (0x1000).
//

#define PAGE_SIZE (ULONG)0x1000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 12L

//
// The highest user address reserves 64K bytes for a guard page. This
// the probing of address from kernel mode to only have to check the
// starting address for structures of 64k bytes or less.
//

#define MM_HIGHEST_USER_ADDRESS (PVOID)0x7FFEFFFF // highest user address
#define MM_USER_PROBE_ADDRESS 0x7FFF0000    // starting address of guard page

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)0x00010000

//
// The lowest address for system space.
//

#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000
#define SYSTEM_BASE 0xc0800000          // start of system space (no typecast)

#endif // defined(_MIPS_)

#if defined(_ALPHA_)

//
// Include the alpha instruction definitions
//

#include "alphaops.h"

//
// Include reference machine definitions.
//

#include "alpharef.h"


//
// Define length of interrupt vector table.
//

#define MAXIMUM_VECTOR 256

//
// Define bus error routine type.
//

struct _EXCEPTION_RECORD;
struct _KEXCEPTION_FRAME;
struct _KTRAP_FRAME;

typedef
BOOLEAN
(*PKBUS_ERROR_ROUTINE) (
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN struct _KEXCEPTION_FRAME *ExceptionFrame,
    IN struct _KTRAP_FRAME *TrapFrame
    );


#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Major and minor version numbers of the PCR.
//

    ULONG MinorVersion;
    ULONG MajorVersion;

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific PAL/HAL code and will
// not change from version to version of NT.

//
// PALcode information.
//

    ULONGLONG PalBaseAddress;
    ULONG PalMajorVersion;
    ULONG PalMinorVersion;
    ULONG PalSequenceVersion;
    ULONG PalMajorSpecification;
    ULONG PalMinorSpecification;

//
// Firmware restart information.
//

    ULONGLONG FirmwareRestartAddress;
    PVOID RestartBlock;

//
// Reserved per-processor region for the PAL (3K bytes).
//

    ULONGLONG PalReserved[384];

//
// Panic Stack Address.
//

    ULONG PanicStack;

//
// Processor parameters.
//

    ULONG ProcessorType;
    ULONG ProcessorRevision;
    ULONG PhysicalAddressBits;
    ULONG MaximumAddressSpaceNumber;
    ULONG PageSize;
    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;

//
// System Parameters.
//

    ULONG FirmwareRevisionId;
    UCHAR SystemType[8];
    ULONG SystemVariant;
    ULONG SystemRevision;
    UCHAR SystemSerialNumber[16];
    ULONG CycleClockPeriod;
    ULONG SecondLevelCacheSize;
    ULONG SecondLevelCacheFillSize;
    ULONG ThirdLevelCacheSize;
    ULONG ThirdLevelCacheFillSize;
    ULONG FourthLevelCacheSize;
    ULONG FourthLevelCacheFillSize;

//
// Pointer to processor control block.
//

    struct _KPRCB *Prcb;

//
// Processor identification.
//

    CCHAR Number;
    KAFFINITY SetMember;

//
// Reserved per-processor region for the HAL (.5K bytes).
//

    ULONGLONG HalReserved[64];

//
// IRQL mapping tables.
//

    ULONG IrqlTable[8];

#define SFW_IMT_ENTRIES 4
#define HDW_IMT_ENTRIES 128

    struct _IRQLMASK {
        USHORT IrqlTableIndex;   // synchronization irql level
        USHORT IDTIndex;         // vector in IDT
    } IrqlMask[SFW_IMT_ENTRIES + HDW_IMT_ENTRIES];

//
// Interrupt Dispatch Table (IDT).
//

    PKINTERRUPT_ROUTINE InterruptRoutine[MAXIMUM_VECTOR];

//
// Reserved vectors mask, these vectors cannot be attached to via
// standard interrupt objects.
//

    ULONG ReservedVectors;

//
// Profiling data.
//

    PLIST_ENTRY ProfileListHead;
    ULONG ProfileInterval;
    ULONG ProfileCount;

//
// Pointer to machine check handler
//

    PKBUS_ERROR_ROUTINE MachineCheckError;

//
// DPC Stack.
//

    ULONG DpcStack;

//
// End of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.  Some of these values are
// reserved for chip-specific palcode.
} KPCR, *PKPCR; 
//
// length of dispatch code in interrupt template
//
#define DISPATCH_LENGTH 4

//
// Define IRQL levels across the architecture.
//

#define PASSIVE_LEVEL   0
#define LOW_LEVEL       0
#define APC_LEVEL       1
#define DISPATCH_LEVEL  2
#define HIGH_LEVEL      7


//
// Define Interlocked operation result values.
//

#define RESULT_ZERO 0
#define RESULT_NEGATIVE 1
#define RESULT_POSITIVE 2

//
// Interlocked result type is portable, but its values are machine specific.
// Constants for value are in i386.h, mips.h, etc.
//

typedef enum _INTERLOCKED_RESULT {
    ResultNegative = RESULT_NEGATIVE,
    ResultZero     = RESULT_ZERO,
    ResultPositive = RESULT_POSITIVE
} INTERLOCKED_RESULT;


// there is a lot of other stuff that could go in here
//   probe macros
//   others

//
// Define the page size for the Alpha ev4 and lca as 8k.
//

#define PAGE_SIZE (ULONG)0x2000

//
// Define the number of trailing zeroes in a page aligned virtual address.
// This is used as the shift count when shifting virtual addresses to
// virtual page numbers.
//

#define PAGE_SHIFT 13L


//
// The highest user address reserves 64K bytes for a guard page. This
// the probing of address from kernel mode to only have to check the
// starting address for structures of 64k bytes or less.
//

#define MM_HIGHEST_USER_ADDRESS (PVOID)0x7FFEFFFF // highest user address
#define MM_USER_PROBE_ADDRESS 0x7FFF0000    // starting address of guard page

//
// The lowest user address reserves the low 64k.
//

#define MM_LOWEST_USER_ADDRESS  (PVOID)0x00010000

//
// The lowest address for system space.
//

#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000
//
// Cache and write buffer flush functions.
//

VOID
KeFlushIoBuffers (
    IN PMDL Mdl,
    IN BOOLEAN ReadOperation,
    IN BOOLEAN DmaOperation
    );

VOID
KeRaiseIrql (
    IN KIRQL NewIrql,
    OUT PKIRQL OldIrql
    );

VOID
KeLowerIrql (
    IN KIRQL NewIrql
    );

#endif // _ALPHA_

#ifdef _X86_

//
// Disable these two pramas that evaluate to "sti" "cli" on x86 so that driver
// writers to not leave them inadvertantly in their code.
//

#if !defined(MIDL_PASS)
#if !defined(_CFRONT_PASS_)
#if !defined(RC_INVOKED)

#pragma warning(disable:4164)   // disable C4164 warning so that apps that
                                // build with /Od don't get weird errors !
#pragma function(_enable)
#pragma function(_disable)

#pragma warning(default:4164)   // reenable C4164 warning

#endif
#endif
#endif


#ifdef _X86_

// begin_winnt

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

//
// The following flags control the contents of the CONTEXT structure.
//

#define CONTEXT_i386    0x00010000    // this assumes that i386 and
#define CONTEXT_i486    0x00010000    // i486 have identical context records

#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // 387 state
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

typedef struct _FLOATING_SAVE_AREA {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   TagWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;
    ULONG   DataOffset;
    ULONG   DataSelector;
    UCHAR   RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    ULONG   SegGs;
    ULONG   SegFs;
    ULONG   SegEs;
    ULONG   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;              // MUST BE SANITIZED
    ULONG   EFlags;             // MUST BE SANITIZED
    ULONG   Esp;
    ULONG   SegSs;

} CONTEXT;



typedef CONTEXT *PCONTEXT;

// begin_ntminiport

#endif //_X86_

//
// I/O space read and write macros.
//
//  These have to be actual functions on the 386, because we need
//  to use assembler, but cannot return a value if we inline it.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space.
//  (Use x86 move instructions, with LOCK prefix to force correct behavior
//   w.r.t. caches and write buffers.)
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space.
//  (Use x86 in/out instructions.)
//

UCHAR
READ_REGISTER_UCHAR(
    PUCHAR  Register
    );

USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

ULONG
READ_REGISTER_ULONG(
    PULONG  Register
    );

VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


VOID
WRITE_REGISTER_UCHAR(
    PUCHAR  Register,
    UCHAR   Value
    );

VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

VOID
WRITE_REGISTER_ULONG(
    PULONG  Register,
    ULONG   Value
    );

VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

UCHAR
READ_PORT_UCHAR(
    PUCHAR  Port
    );

USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

#endif // _X86_

#if defined(_MIPS_)
//
// Define unsupported "keywords".
//

#define _cdecl

// begin_windbgkd

#if defined(_MIPS_)

#endif                          
//
// Define length of exception code dispatch vector.
//

#define XCODE_VECTOR_LENGTH 32

//
// Define length of interrupt vector table.
//

#define MAXIMUM_VECTOR 256

//
// Define bus error routine type.
//

struct _EXCEPTION_RECORD;
struct _KEXCEPTION_FRAME;
struct _KTRAP_FRAME;

typedef
VOID
(*PKBUS_ERROR_ROUTINE) (
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN struct _KEXCEPTION_FRAME *ExceptionFrame,
    IN struct _KTRAP_FRAME *TrapFrame,
    IN PVOID VirtualAddress,
    IN PHYSICAL_ADDRESS PhysicalAddress
    );

//
// Define Processor Control Region Structure.
//

#define PCR_MINOR_VERSION 1
#define PCR_MAJOR_VERSION 1

typedef struct _KPCR {

//
// Major and minor version numbers of the PCR.
//

    USHORT MinorVersion;
    USHORT MajorVersion;

//
// Start of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//
// Interrupt and error exception vectors.
//

    PKINTERRUPT_ROUTINE InterruptRoutine[MAXIMUM_VECTOR];
    PVOID XcodeDispatch[XCODE_VECTOR_LENGTH];

//
// First and second level cache parameters.
//

    ULONG FirstLevelDcacheSize;
    ULONG FirstLevelDcacheFillSize;
    ULONG FirstLevelIcacheSize;
    ULONG FirstLevelIcacheFillSize;
    ULONG SecondLevelDcacheSize;
    ULONG SecondLevelDcacheFillSize;
    ULONG SecondLevelIcacheSize;
    ULONG SecondLevelIcacheFillSize;

//
// Pointer to processor control block.
//

    struct _KPRCB *Prcb;

//
// Pointer to the thread environment block.
//

    PVOID Teb;

//
// Data cache alignment and fill size used for cache flushing and alignment.
// These fields are set to the larger of the first and second level data
// cache fill sizes.
//

    ULONG DcacheAlignment;
    ULONG DcacheFillSize;

//
// Instruction cache alignment and fill size used for cache flushing and
// alignment. These fields are set to the larger of the first and second
// level data cache fill sizes.
//

    ULONG IcacheAlignment;
    ULONG IcacheFillSize;

//
// Processor identification from PrId register.
//

    ULONG ProcessorId;

//
// Profiling data.
//

    ULONG ProfileInterval;
    ULONG ProfileCount;

//
// Stall execution count and scale factor.
//

    ULONG StallExecutionCount;
    ULONG StallScaleFactor;

//
// Spare cell.
//

    ULONG Spare;

//
// Pointers to bus error and parity error routines.
//

    PKBUS_ERROR_ROUTINE DataBusError;
    PKBUS_ERROR_ROUTINE InstructionBusError;

//
// Cache policy, right justified, as read from the processor configuration
// register at startup.
//

    ULONG CachePolicy;

//
// IRQL mapping tables.
//

    UCHAR IrqlMask[32];
    UCHAR IrqlTable[9];

//
// Current IRQL.
//

    UCHAR CurrentIrql;

//
// Processor affinity mask.
//

    KAFFINITY SetMember;

//
// Reserved interrupt vector mask.
//

    ULONG ReservedVectors;

//
// Current state parameters.
//

    struct _KTHREAD *CurrentThread;

//
// Cache policy, PTE field aligned, as read from the processor configuration
// register at startup.
//

    ULONG AlignedCachePolicy;

//
// Spare cell.
//

    ULONG Spare0;

//
// Space reserved for the system.
//

    ULONG   SystemReserved[16];

//
// Space reserved for the HAL
//

    ULONG   HalReserved[16];

//
// End of the architecturally defined section of the PCR. This section
// may be directly addressed by vendor/platform specific HAL code and will
// not change from version to version of NT.
//
} KPCR, *PKPCR;                     
//
// The following flags control the contents of the CONTEXT structure.
//

#define CONTEXT_R4000   0x00010000    // r4000 context

#define CONTEXT_CONTROL         (CONTEXT_R4000 | 0x00000001L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_R4000 | 0x00000002L)
#define CONTEXT_INTEGER         (CONTEXT_R4000 | 0x00000004L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

//
// Context Frame
//
//  N.B. This frame must be exactly a multiple of 16 bytes in length.
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to constuct a call frame for APC delivery,
//  3) it is used to construct a call frame for exception dispatching
//  in user mode, and 4) it is used in the user level thread creation
//  routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT {

    //
    // This section is always present and is used as an argument build
    // area.
    //

    ULONG Argument[4];

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONG FltF0;
    ULONG FltF1;
    ULONG FltF2;
    ULONG FltF3;
    ULONG FltF4;
    ULONG FltF5;
    ULONG FltF6;
    ULONG FltF7;
    ULONG FltF8;
    ULONG FltF9;
    ULONG FltF10;
    ULONG FltF11;
    ULONG FltF12;
    ULONG FltF13;
    ULONG FltF14;
    ULONG FltF15;
    ULONG FltF16;
    ULONG FltF17;
    ULONG FltF18;
    ULONG FltF19;
    ULONG FltF20;
    ULONG FltF21;
    ULONG FltF22;
    ULONG FltF23;
    ULONG FltF24;
    ULONG FltF25;
    ULONG FltF26;
    ULONG FltF27;
    ULONG FltF28;
    ULONG FltF29;
    ULONG FltF30;
    ULONG FltF31;

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_INTEGER.
    //
    // N.B. The registers gp, sp, and ra are defined in this section, but are
    //  considered part of the control context rather than part of the integer
    //  context.
    //
    // N.B. Register zero is not stored in the frame.
    //

    ULONG IntZero;
    ULONG IntAt;
    ULONG IntV0;
    ULONG IntV1;
    ULONG IntA0;
    ULONG IntA1;
    ULONG IntA2;
    ULONG IntA3;
    ULONG IntT0;
    ULONG IntT1;
    ULONG IntT2;
    ULONG IntT3;
    ULONG IntT4;
    ULONG IntT5;
    ULONG IntT6;
    ULONG IntT7;
    ULONG IntS0;
    ULONG IntS1;
    ULONG IntS2;
    ULONG IntS3;
    ULONG IntS4;
    ULONG IntS5;
    ULONG IntS6;
    ULONG IntS7;
    ULONG IntT8;
    ULONG IntT9;
    ULONG IntK0;
    ULONG IntK1;
    ULONG IntGp;
    ULONG IntSp;
    ULONG IntS8;
    ULONG IntRa;
    ULONG IntLo;
    ULONG IntHi;

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONG Fsr;

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_CONTROL.
    //
    // N.B. The registers gp, sp, and ra are defined in the integer section,
    //   but are considered part of the control context rather than part of
    //   the integer context.
    //

    ULONG Fir;
    ULONG Psr;

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;

    ULONG Fill[2];
} CONTEXT, *PCONTEXT;

#endif // defined(_MIPS_)
#ifdef _ALPHA_                  
//
// The following flags control the contents of the CONTEXT structure.
//

#define CONTEXT_PORTABLE_32BIT     0x00100000
#define CONTEXT_ALPHA              0x00020000

#define CONTEXT_CONTROL         (CONTEXT_ALPHA | 0x00000001L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_ALPHA | 0x00000002L)
#define CONTEXT_INTEGER         (CONTEXT_ALPHA | 0x00000004L)

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

#ifndef _PORTABLE_32BIT_CONTEXT

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to construct a call frame for APC delivery,
//  3) it is used to construct a call frame for exception dispatching
//  in user mode, 4) it is used in the user level thread creation
//  routines, and 5) it is used to to pass thread state to debuggers.
//
//  N.B. Because this record is used as a call frame, it must be EXACTLY
//  a multiple of 16 bytes in length.
//
//  There are two variations of the context structure. This is the real one.
//

typedef struct _CONTEXT {

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONGLONG FltF0;
    ULONGLONG FltF1;
    ULONGLONG FltF2;
    ULONGLONG FltF3;
    ULONGLONG FltF4;
    ULONGLONG FltF5;
    ULONGLONG FltF6;
    ULONGLONG FltF7;
    ULONGLONG FltF8;
    ULONGLONG FltF9;
    ULONGLONG FltF10;
    ULONGLONG FltF11;
    ULONGLONG FltF12;
    ULONGLONG FltF13;
    ULONGLONG FltF14;
    ULONGLONG FltF15;
    ULONGLONG FltF16;
    ULONGLONG FltF17;
    ULONGLONG FltF18;
    ULONGLONG FltF19;
    ULONGLONG FltF20;
    ULONGLONG FltF21;
    ULONGLONG FltF22;
    ULONGLONG FltF23;
    ULONGLONG FltF24;
    ULONGLONG FltF25;
    ULONGLONG FltF26;
    ULONGLONG FltF27;
    ULONGLONG FltF28;
    ULONGLONG FltF29;
    ULONGLONG FltF30;
    ULONGLONG FltF31;

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_INTEGER.
    //
    // N.B. The registers gp, sp, and ra are defined in this section, but are
    //  considered part of the control context rather than part of the integer
    //  context.
    //

    ULONGLONG IntV0;    //  $0: return value register, v0
    ULONGLONG IntT0;    //  $1: temporary registers, t0 - t7
    ULONGLONG IntT1;    //  $2:
    ULONGLONG IntT2;    //  $3:
    ULONGLONG IntT3;    //  $4:
    ULONGLONG IntT4;    //  $5:
    ULONGLONG IntT5;    //  $6:
    ULONGLONG IntT6;    //  $7:
    ULONGLONG IntT7;    //  $8:
    ULONGLONG IntS0;    //  $9: nonvolatile registers, s0 - s5
    ULONGLONG IntS1;    // $10:
    ULONGLONG IntS2;    // $11:
    ULONGLONG IntS3;    // $12:
    ULONGLONG IntS4;    // $13:
    ULONGLONG IntS5;    // $14:
    ULONGLONG IntFp;    // $15: frame pointer register, fp/s6
    ULONGLONG IntA0;    // $16: argument registers, a0 - a5
    ULONGLONG IntA1;    // $17:
    ULONGLONG IntA2;    // $18:
    ULONGLONG IntA3;    // $19:
    ULONGLONG IntA4;    // $20:
    ULONGLONG IntA5;    // $21:
    ULONGLONG IntT8;    // $22: temporary registers, t8 - t11
    ULONGLONG IntT9;    // $23:
    ULONGLONG IntT10;   // $24:
    ULONGLONG IntT11;   // $25:
    ULONGLONG IntRa;    // $26: return address register, ra
    ULONGLONG IntT12;   // $27: temporary register, t12
    ULONGLONG IntAt;    // $28: assembler temp register, at
    ULONGLONG IntGp;    // $29: global pointer register, gp
    ULONGLONG IntSp;    // $30: stack pointer register, sp
    ULONGLONG IntZero;  // $31: zero register, zero

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_FLOATING_POINT.
    //

    ULONGLONG Fpcr;     // floating point control register
    ULONGLONG SoftFpcr; // software extension to FPCR

    //
    // This section is specified/returned if the ContextFlags word contains
    // the flag CONTEXT_CONTROL.
    //
    // N.B. The registers gp, sp, and ra are defined in the integer section,
    //   but are considered part of the control context rather than part of
    //   the integer context.
    //

    ULONGLONG Fir;      // (fault instruction) continuation address
    ULONG Psr;          // processor status

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a thread's context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    ULONG ContextFlags;
    ULONG Fill[4];      // padding for 16-byte stack frame alignment

} CONTEXT, *PCONTEXT;

#else

//
// 32-bit Context Frame
//
//  This alternate version of the Alpha context structure parallels that
//  of MIPS and IX86 in style for the first 64 entries: 32-bit machines
//  can operate on the fields, and a value declared as a pointer to an
//  array of int's can be used to index into the fields.  This makes life
//  with windbg and ntsd vastly easier.
//
//  There are two parts: the first contains the lower 32-bits of each
//  element in the 64-bit definition above.  The second part contains
//  the upper 32-bits of each 64-bit element above.
//
//  The names in the first part are identical to the 64-bit names.
//  The second part names are prefixed with "High".
//
//  1st half: at 32 bits each, (containing the low parts of 64-bit values)
//      32 floats, 32 ints, fpcrs, fir, psr, contextflags
//  2nd half: at 32 bits each
//      32 floats, 32 ints, fpcrs, fir, fill
//
//  There is no external support for the 32-bit version of the context
//  structure.  It is only used internally by windbg and ntsd.
//
//  This structure must be the same size as the 64-bit version above.
//

typedef struct _CONTEXT {

    ULONG FltF0;
    ULONG FltF1;
    ULONG FltF2;
    ULONG FltF3;
    ULONG FltF4;
    ULONG FltF5;
    ULONG FltF6;
    ULONG FltF7;
    ULONG FltF8;
    ULONG FltF9;
    ULONG FltF10;
    ULONG FltF11;
    ULONG FltF12;
    ULONG FltF13;
    ULONG FltF14;
    ULONG FltF15;
    ULONG FltF16;
    ULONG FltF17;
    ULONG FltF18;
    ULONG FltF19;
    ULONG FltF20;
    ULONG FltF21;
    ULONG FltF22;
    ULONG FltF23;
    ULONG FltF24;
    ULONG FltF25;
    ULONG FltF26;
    ULONG FltF27;
    ULONG FltF28;
    ULONG FltF29;
    ULONG FltF30;
    ULONG FltF31;

    ULONG IntV0;        //  $0: return value register, v0
    ULONG IntT0;        //  $1: temporary registers, t0 - t7
    ULONG IntT1;        //  $2:
    ULONG IntT2;        //  $3:
    ULONG IntT3;        //  $4:
    ULONG IntT4;        //  $5:
    ULONG IntT5;        //  $6:
    ULONG IntT6;        //  $7:
    ULONG IntT7;        //  $8:
    ULONG IntS0;        //  $9: nonvolatile registers, s0 - s5
    ULONG IntS1;        // $10:
    ULONG IntS2;        // $11:
    ULONG IntS3;        // $12:
    ULONG IntS4;        // $13:
    ULONG IntS5;        // $14:
    ULONG IntFp;        // $15: frame pointer register, fp/s6
    ULONG IntA0;        // $16: argument registers, a0 - a5
    ULONG IntA1;        // $17:
    ULONG IntA2;        // $18:
    ULONG IntA3;        // $19:
    ULONG IntA4;        // $20:
    ULONG IntA5;        // $21:
    ULONG IntT8;        // $22: temporary registers, t8 - t11
    ULONG IntT9;        // $23:
    ULONG IntT10;       // $24:
    ULONG IntT11;       // $25:
    ULONG IntRa;        // $26: return address register, ra
    ULONG IntT12;       // $27: temporary register, t12
    ULONG IntAt;        // $28: assembler temp register, at
    ULONG IntGp;        // $29: global pointer register, gp
    ULONG IntSp;        // $30: stack pointer register, sp
    ULONG IntZero;      // $31: zero register, zero

    ULONG Fpcr;         // floating point control register
    ULONG SoftFpcr;     // software extension to FPCR

    ULONG Fir;          // (fault instruction) continuation address

    ULONG Psr;          // processor status
    ULONG ContextFlags;

    //
    // Beginning of the "second half".
    // The name "High" parallels the HighPart of a LargeInteger.
    //

    ULONG HighFltF0;
    ULONG HighFltF1;
    ULONG HighFltF2;
    ULONG HighFltF3;
    ULONG HighFltF4;
    ULONG HighFltF5;
    ULONG HighFltF6;
    ULONG HighFltF7;
    ULONG HighFltF8;
    ULONG HighFltF9;
    ULONG HighFltF10;
    ULONG HighFltF11;
    ULONG HighFltF12;
    ULONG HighFltF13;
    ULONG HighFltF14;
    ULONG HighFltF15;
    ULONG HighFltF16;
    ULONG HighFltF17;
    ULONG HighFltF18;
    ULONG HighFltF19;
    ULONG HighFltF20;
    ULONG HighFltF21;
    ULONG HighFltF22;
    ULONG HighFltF23;
    ULONG HighFltF24;
    ULONG HighFltF25;
    ULONG HighFltF26;
    ULONG HighFltF27;
    ULONG HighFltF28;
    ULONG HighFltF29;
    ULONG HighFltF30;
    ULONG HighFltF31;

    ULONG HighIntV0;        //  $0: return value register, v0
    ULONG HighIntT0;        //  $1: temporary registers, t0 - t7
    ULONG HighIntT1;        //  $2:
    ULONG HighIntT2;        //  $3:
    ULONG HighIntT3;        //  $4:
    ULONG HighIntT4;        //  $5:
    ULONG HighIntT5;        //  $6:
    ULONG HighIntT6;        //  $7:
    ULONG HighIntT7;        //  $8:
    ULONG HighIntS0;        //  $9: nonvolatile registers, s0 - s5
    ULONG HighIntS1;        // $10:
    ULONG HighIntS2;        // $11:
    ULONG HighIntS3;        // $12:
    ULONG HighIntS4;        // $13:
    ULONG HighIntS5;        // $14:
    ULONG HighIntFp;        // $15: frame pointer register, fp/s6
    ULONG HighIntA0;        // $16: argument registers, a0 - a5
    ULONG HighIntA1;        // $17:
    ULONG HighIntA2;        // $18:
    ULONG HighIntA3;        // $19:
    ULONG HighIntA4;        // $20:
    ULONG HighIntA5;        // $21:
    ULONG HighIntT8;        // $22: temporary registers, t8 - t11
    ULONG HighIntT9;        // $23:
    ULONG HighIntT10;       // $24:
    ULONG HighIntT11;       // $25:
    ULONG HighIntRa;        // $26: return address register, ra
    ULONG HighIntT12;       // $27: temporary register, t12
    ULONG HighIntAt;        // $28: assembler temp register, at
    ULONG HighIntGp;        // $29: global pointer register, gp
    ULONG HighIntSp;        // $30: stack pointer register, sp
    ULONG HighIntZero;      // $31: zero register, zero

    ULONG HighFpcr;         // floating point control register
    ULONG HighSoftFpcr;     // software extension to FPCR
    ULONG HighFir;          // processor status

    double DoNotUseThisField; // to force quadword structure alignment
    ULONG HighFill[2];      // padding for 16-byte stack frame alignment

} CONTEXT, *PCONTEXT;

//
// These should name the fields in the _PORTABLE_32BIT structure
// that overlay the Psr and ContextFlags in the normal structure.
//

#define _QUAD_PSR_OFFSET   HighSoftFpcr
#define _QUAD_FLAGS_OFFSET HighFir

#endif // _PORTABLE_32BIT_CONTEXT

//
// I/O space read and write macros.
//
//  These have to be actual functions on Alpha, because we need
//  to shift the VA and OR in the BYTE ENABLES.
//
//  These can become INLINEs if we require that ALL Alpha systems shift
//  the same number of bits and have the SAME byte enables.
//
//  The READ/WRITE_REGISTER_* calls manipulate I/O registers in MEMORY space?
//
//  The READ/WRITE_PORT_* calls manipulate I/O registers in PORT space?
//

UCHAR
READ_REGISTER_UCHAR(
    PUCHAR Register
    );

USHORT
READ_REGISTER_USHORT(
    PUSHORT Register
    );

ULONG
READ_REGISTER_ULONG(
    PULONG Register
    );

VOID
READ_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
READ_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
READ_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );


VOID
WRITE_REGISTER_UCHAR(
    PUCHAR Register,
    UCHAR   Value
    );

VOID
WRITE_REGISTER_USHORT(
    PUSHORT Register,
    USHORT  Value
    );

VOID
WRITE_REGISTER_ULONG(
    PULONG Register,
    ULONG   Value
    );

VOID
WRITE_REGISTER_BUFFER_UCHAR(
    PUCHAR  Register,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
WRITE_REGISTER_BUFFER_USHORT(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
WRITE_REGISTER_BUFFER_ULONG(
    PULONG  Register,
    PULONG  Buffer,
    ULONG   Count
    );

UCHAR
READ_PORT_UCHAR(
    PUCHAR Port
    );

USHORT
READ_PORT_USHORT(
    PUSHORT Port
    );

ULONG
READ_PORT_ULONG(
    PULONG  Port
    );

VOID
READ_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
READ_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
READ_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_UCHAR(
    PUCHAR  Port,
    UCHAR   Value
    );

VOID
WRITE_PORT_USHORT(
    PUSHORT Port,
    USHORT  Value
    );

VOID
WRITE_PORT_ULONG(
    PULONG  Port,
    ULONG   Value
    );

VOID
WRITE_PORT_BUFFER_UCHAR(
    PUCHAR  Port,
    PUCHAR  Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_BUFFER_USHORT(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG   Count
    );

VOID
WRITE_PORT_BUFFER_ULONG(
    PULONG  Port,
    PULONG  Buffer,
    ULONG   Count
    );

#endif // _ALPHA_               // ntddk nthal

// begin_winnt
//
// Predefined Value Types.
//

#define REG_NONE                    ( 0 )   // No value type
#define REG_SZ                      ( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3 )   // Free form binary
#define REG_DWORD                   ( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description

//
// Service Types (Bit Mask)
//
#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008

#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | \
                                        SERVICE_FILE_SYSTEM_DRIVER | \
                                        SERVICE_RECOGNIZER_DRIVER)

#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
                                        SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | \
                                        SERVICE_ADAPTER | \
                                        SERVICE_DRIVER)

//
// Start Type
//

#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004

//
// Error control type
//
#define SERVICE_ERROR_IGNORE           0x00000000
#define SERVICE_ERROR_NORMAL           0x00000001
#define SERVICE_ERROR_SEVERE           0x00000002
#define SERVICE_ERROR_CRITICAL         0x00000003

//
//
// Define the registry driver node enumerations
//

typedef enum _CM_SERVICE_NODE_TYPE {
    DriverType               = SERVICE_KERNEL_DRIVER,
    FileSystemType           = SERVICE_FILE_SYSTEM_DRIVER,
    Win32ServiceOwnProcess   = SERVICE_WIN32_OWN_PROCESS,
    Win32ServiceShareProcess = SERVICE_WIN32_SHARE_PROCESS,
    AdapterType              = SERVICE_ADAPTER,
    RecognizerType           = SERVICE_RECOGNIZER_DRIVER
} SERVICE_NODE_TYPE;

typedef enum _CM_SERVICE_LOAD_TYPE {
    BootLoad    = SERVICE_BOOT_START,
    SystemLoad  = SERVICE_SYSTEM_START,
    AutoLoad    = SERVICE_AUTO_START,
    DemandLoad  = SERVICE_DEMAND_START,
    DisableLoad = SERVICE_DISABLED
} SERVICE_LOAD_TYPE;

typedef enum _CM_ERROR_CONTROL_TYPE {
    IgnoreError   = SERVICE_ERROR_IGNORE,
    NormalError   = SERVICE_ERROR_NORMAL,
    SevereError   = SERVICE_ERROR_SEVERE,
    CriticalError = SERVICE_ERROR_CRITICAL
} SERVICE_ERROR_TYPE;

// end_winnt

//
// Resource List definitions
//

//
// Defines the Type in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_RESOURCE_TYPE {
    CmResourceTypeNull = 0,    // Reserved
    CmResourceTypePort,
    CmResourceTypeInterrupt,
    CmResourceTypeMemory,
    CmResourceTypeDma,
    CmResourceTypeDeviceSpecific
} CM_RESOURCE_TYPE;

//
// Defines the ShareDisposition in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;

//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0
#define CM_RESOURCE_INTERRUPT_LATCHED         1

//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_MEMORY_READ_WRITE 0
#define CM_RESOURCE_MEMORY_READ_ONLY  1
#define CM_RESOURCE_MEMORY_WRITE_ONLY 2

//
// Define the bit masks for Flags when type is CmResourceTypePort
//

#define CM_RESOURCE_PORT_MEMORY 0
#define CM_RESOURCE_PORT_IO 1

//
// This structure defines one type of resource used by a driver.
//
// There can only be *1* DeviceSpecificData block. It must be located at
// the end of all resource descriptors in a full descriptor block.
//

//
// BUGBUG Make sure alignment is made properly by compiler; otherwise move
// flags back to the top of the structure (common to all members of the
// union).
//

#pragma pack(4)
typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of port numbers, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        // IRQL and vector. Should be same values as were passed to
        // HalGetInterruptVector().
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;
#pragma pack()

//
// A Partial Resource List is what can be found in the ARC firmware
// or will be generated by ntdetect.com.
// The configuration manager will transform this structure into a Full
// resource descriptor when it is about to store it in the regsitry.
//
// Note: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

//
// A Full Resource Descriptor is what can be found in the registry.
// This is what will be returned to a driver when it queries the registry
// to get device information; it will be stored under a key in the hardware
// description tree.
//
// Note: The BusNumber and Type are redundant information, but we will keep
// it since it allows the driver _not_ to append it when it is creating
// a resource list which could possibly span multiple buses.
//
// Note2: There must a be a convention to the order of fields of same type,
// (defined on a device by device basis) so that the fields can make sense
// to a driver (i.e. when multiple memory ranges are necessary).
//

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR {
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    CM_PARTIAL_RESOURCE_LIST PartialResourceList;
} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR;

//
// The Resource list is what will be stored by the drivers into the
// resource map via the IO API.
//

typedef struct _CM_RESOURCE_LIST {
    ULONG Count;
    CM_FULL_RESOURCE_DESCRIPTOR List[1];
} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST;

//
// Define the structures used to interpret configuration data of
// \\Registry\machine\hardware\description tree.
// Basically, these structures are used to interpret component
// sepcific data.
//

//
// Define DEVICE_FLAGS
//

typedef struct _DEVICE_FLAGS {
    ULONG Failed : 1;
    ULONG ReadOnly : 1;
    ULONG Removable : 1;
    ULONG ConsoleIn : 1;
    ULONG ConsoleOut : 1;
    ULONG Input : 1;
    ULONG Output : 1;
} DEVICE_FLAGS, *PDEVICE_FLAGS;

//
// Define Component Information structure
//

typedef struct _CM_COMPONENT_INFORMATION {
    DEVICE_FLAGS Flags;
    ULONG Version;
    ULONG Key;
    ULONG AffinityMask;
} CM_COMPONENT_INFORMATION, *PCM_COMPONENT_INFORMATION;

//
// The following structures are used to interpret x86
// DeviceSpecificData of CM_PARTIAL_RESOURCE_DESCRIPTOR.
// (Most of the structures are defined by BIOS.  They are
// not aligned on word (or dword) boundary.
//

//
// Define the Rom Block structure
//

typedef struct _CM_ROM_BLOCK {
    ULONG Address;
    ULONG Size;
} CM_ROM_BLOCK, *PCM_ROM_BLOCK;

// begin_ntminiport

#pragma pack(1)

// end_ntminiport

//
// Define INT13 driver parameter block
//

typedef struct _CM_INT13_DRIVE_PARAMETER {
    USHORT DriveSelect;
    ULONG MaxCylinders;
    USHORT SectorsPerTrack;
    USHORT MaxHeads;
    USHORT NumberDrives;
} CM_INT13_DRIVE_PARAMETER, *PCM_INT13_DRIVE_PARAMETER;

// begin_ntminiport

//
// Define Mca POS data block for slot
//

typedef struct _CM_MCA_POS_DATA {
    USHORT AdapterId;
    UCHAR PosData1;
    UCHAR PosData2;
    UCHAR PosData3;
    UCHAR PosData4;
} CM_MCA_POS_DATA, *PCM_MCA_POS_DATA;

//
// Memory configuration of eisa data block structure
//

typedef struct _EISA_MEMORY_TYPE {
    UCHAR ReadWrite: 1;
    UCHAR Cached : 1;
    UCHAR Reserved0 :1;
    UCHAR Type:2;
    UCHAR Shared:1;
    UCHAR Reserved1 :1;
    UCHAR MoreEntries : 1;
} EISA_MEMORY_TYPE, *PEISA_MEMORY_TYPE;

typedef struct _EISA_MEMORY_CONFIGURATION {
    EISA_MEMORY_TYPE ConfigurationByte;
    UCHAR DataSize;
    USHORT AddressLowWord;
    UCHAR AddressHighByte;
    USHORT MemorySize;
} EISA_MEMORY_CONFIGURATION, *PEISA_MEMORY_CONFIGURATION;


//
// Interrupt configurationn of eisa data block structure
//

typedef struct _EISA_IRQ_DESCRIPTOR {
    UCHAR Interrupt : 4;
    UCHAR Reserved :1;
    UCHAR LevelTriggered :1;
    UCHAR Shared : 1;
    UCHAR MoreEntries : 1;
} EISA_IRQ_DESCRIPTOR, *PEISA_IRQ_DESCRIPTOR;

typedef struct _EISA_IRQ_CONFIGURATION {
    EISA_IRQ_DESCRIPTOR ConfigurationByte;
    UCHAR Reserved;
} EISA_IRQ_CONFIGURATION, *PEISA_IRQ_CONFIGURATION;


//
// DMA description of eisa data block structure
//

typedef struct _DMA_CONFIGURATION_BYTE0 {
    UCHAR Channel : 3;
    UCHAR Reserved : 3;
    UCHAR Shared :1;
    UCHAR MoreEntries :1;
} DMA_CONFIGURATION_BYTE0;

typedef struct _DMA_CONFIGURATION_BYTE1 {
    UCHAR Reserved0 : 2;
    UCHAR TransferSize : 2;
    UCHAR Timing : 2;
    UCHAR Reserved1 : 2;
} DMA_CONFIGURATION_BYTE1;

typedef struct _EISA_DMA_CONFIGURATION {
    DMA_CONFIGURATION_BYTE0 ConfigurationByte0;
    DMA_CONFIGURATION_BYTE1 ConfigurationByte1;
} EISA_DMA_CONFIGURATION, *PEISA_DMA_CONFIGURATION;


//
// Port description of eisa data block structure
//

typedef struct _EISA_PORT_DESCRIPTOR {
    UCHAR NumberPorts : 5;
    UCHAR Reserved :1;
    UCHAR Shared :1;
    UCHAR MoreEntries : 1;
} EISA_PORT_DESCRIPTOR, *PEISA_PORT_DESCRIPTOR;

typedef struct _EISA_PORT_CONFIGURATION {
    EISA_PORT_DESCRIPTOR Configuration;
    USHORT PortAddress;
} EISA_PORT_CONFIGURATION, *PEISA_PORT_CONFIGURATION;


//
// Eisa slot information definition
// N.B. This structure is different from the one defined
//      in ARC eisa addendum.
//

typedef struct _CM_EISA_SLOT_INFORMATION {
    UCHAR ReturnCode;
    UCHAR ReturnFlags;
    UCHAR MajorRevision;
    UCHAR MinorRevision;
    USHORT Checksum;
    UCHAR NumberFunctions;
    UCHAR FunctionInformation;
    ULONG CompressedId;
} CM_EISA_SLOT_INFORMATION, *PCM_EISA_SLOT_INFORMATION;


//
// Eisa function information definition
//

typedef struct _CM_EISA_FUNCTION_INFORMATION {
    ULONG CompressedId;
    UCHAR IdSlotFlags1;
    UCHAR IdSlotFlags2;
    UCHAR MinorRevision;
    UCHAR MajorRevision;
    UCHAR Selections[26];
    UCHAR FunctionFlags;
    UCHAR TypeString[80];
    EISA_MEMORY_CONFIGURATION EisaMemory[9];
    EISA_IRQ_CONFIGURATION EisaIrq[7];
    EISA_DMA_CONFIGURATION EisaDma[4];
    EISA_PORT_CONFIGURATION EisaPort[20];
    UCHAR InitializationData[60];
} CM_EISA_FUNCTION_INFORMATION, *PCM_EISA_FUNCTION_INFORMATION;

#pragma pack()

//
// Masks for EISA function information
//

#define EISA_FUNCTION_ENABLED                   0x80
#define EISA_FREE_FORM_DATA                     0x40
#define EISA_HAS_PORT_INIT_ENTRY                0x20
#define EISA_HAS_PORT_RANGE                     0x10
#define EISA_HAS_DMA_ENTRY                      0x08
#define EISA_HAS_IRQ_ENTRY                      0x04
#define EISA_HAS_MEMORY_ENTRY                   0x02
#define EISA_HAS_TYPE_ENTRY                     0x01
#define EISA_HAS_INFORMATION                    EISA_HAS_PORT_RANGE + \
                                                EISA_HAS_DMA_ENTRY + \
                                                EISA_HAS_IRQ_ENTRY + \
                                                EISA_HAS_MEMORY_ENTRY + \
                                                EISA_HAS_TYPE_ENTRY

//
// Masks for EISA memory configuration
//

#define EISA_MORE_ENTRIES                       0x80
#define EISA_SYSTEM_MEMORY                      0x00
#define EISA_MEMORY_TYPE_RAM                    0x01

//
// Returned error code for EISA bios call
//

#define EISA_INVALID_SLOT                       0x80
#define EISA_INVALID_FUNCTION                   0x81
#define EISA_INVALID_CONFIGURATION              0x82
#define EISA_EMPTY_SLOT                         0x83
#define EISA_INVALID_BIOS_CALL                  0x86

// end_ntminiport

//
// The following structures are used to interpret mips
// DeviceSpecificData of CM_PARTIAL_RESOURCE_DESCRIPTOR.
//

//
// Device data records for adapters.
//

//
// The device data record for the Emulex SCSI controller.
//

typedef struct _CM_SCSI_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    UCHAR HostIdentifier;
} CM_SCSI_DEVICE_DATA, *PCM_SCSI_DEVICE_DATA;

//
// Device data records for controllers.
//

//
// The device data record for the Video controller.
//

typedef struct _CM_VIDEO_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    ULONG VideoClock;
} CM_VIDEO_DEVICE_DATA, *PCM_VIDEO_DEVICE_DATA;

//
// The device data record for the SONIC network controller.
//

typedef struct _CM_SONIC_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT DataConfigurationRegister;
    UCHAR EthernetAddress[8];
} CM_SONIC_DEVICE_DATA, *PCM_SONIC_DEVICE_DATA;

//
// The device data record for the serial controller.
//

typedef struct _CM_SERIAL_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    ULONG BaudClock;
} CM_SERIAL_DEVICE_DATA, *PCM_SERIAL_DEVICE_DATA;

//
// Device data records for peripherals.
//

//
// The device data record for the Monitor peripheral.
//

typedef struct _CM_MONITOR_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    USHORT HorizontalScreenSize;
    USHORT VerticalScreenSize;
    USHORT HorizontalResolution;
    USHORT VerticalResolution;
    USHORT HorizontalDisplayTimeLow;
    USHORT HorizontalDisplayTime;
    USHORT HorizontalDisplayTimeHigh;
    USHORT HorizontalBackPorchLow;
    USHORT HorizontalBackPorch;
    USHORT HorizontalBackPorchHigh;
    USHORT HorizontalFrontPorchLow;
    USHORT HorizontalFrontPorch;
    USHORT HorizontalFrontPorchHigh;
    USHORT HorizontalSyncLow;
    USHORT HorizontalSync;
    USHORT HorizontalSyncHigh;
    USHORT VerticalBackPorchLow;
    USHORT VerticalBackPorch;
    USHORT VerticalBackPorchHigh;
    USHORT VerticalFrontPorchLow;
    USHORT VerticalFrontPorch;
    USHORT VerticalFrontPorchHigh;
    USHORT VerticalSyncLow;
    USHORT VerticalSync;
    USHORT VerticalSyncHigh;
} CM_MONITOR_DEVICE_DATA, *PCM_MONITOR_DEVICE_DATA;

//
// The device data record for the Floppy peripheral.
//

typedef struct _CM_FLOPPY_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    CHAR Size[8];
    ULONG MaxDensity;
    ULONG MountDensity;
    //
    // New data fields for version >= 2.0
    //
    UCHAR StepRateHeadUnloadTime;
    UCHAR HeadLoadTime;
    UCHAR MotorOffTime;
    UCHAR SectorLengthCode;
    UCHAR SectorPerTrack;
    UCHAR ReadWriteGapLength;
    UCHAR DataTransferLength;
    UCHAR FormatGapLength;
    UCHAR FormatFillCharacter;
    UCHAR HeadSettleTime;
    UCHAR MotorSettleTime;
    UCHAR MaximumTrackValue;
    UCHAR DataTransferRate;
} CM_FLOPPY_DEVICE_DATA, *PCM_FLOPPY_DEVICE_DATA;

//
// The device data record for the Keyboard peripheral.
// The KeyboardFlags is defined (by x86 BIOS INT 16h, function 02) as:
//      bit 7 : Insert on
//      bit 6 : Caps Lock on
//      bit 5 : Num Lock on
//      bit 4 : Scroll Lock on
//      bit 3 : Alt Key is down
//      bit 2 : Ctrl Key is down
//      bit 1 : Left shift key is down
//      bit 0 : Right shift key is down
//

typedef struct _CM_KEYBOARD_DEVICE_DATA {
    USHORT Version;
    USHORT Revision;
    UCHAR Type;
    UCHAR Subtype;
    USHORT KeyboardFlags;
} CM_KEYBOARD_DEVICE_DATA, *PCM_KEYBOARD_DEVICE_DATA;

//
// Declaration of the structure for disk geometries
//

typedef struct _CM_DISK_GEOMETRY_DEVICE_DATA {
    ULONG BytesPerSector;
    ULONG NumberOfCylinders;
    ULONG SectorsPerTrack;
    ULONG NumberOfHeads;
} CM_DISK_GEOMETRY_DEVICE_DATA, *PCM_DISK_GEOMETRY_DEVICE_DATA;

//
// Exception flag definitions.
//

// begin_winnt
#define EXCEPTION_NONCONTINUABLE 0x1    // Noncontinuable exception
// end_winnt

//
// Define maximum number of exception parameters.
//

// begin_winnt
#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

//
// Exception record definition.
//

typedef struct _EXCEPTION_RECORD {
    /*lint -e18 */  // Don't complain about different definitions
    NTSTATUS ExceptionCode;
    /*lint +e18 */  // Resume checking for different definitions
    ULONG ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
    } EXCEPTION_RECORD;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

//
// Typedef for pointer returned by exception_info()
//

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
// end_winnt

//
// Define configuration routine types.
//
// Configuration information.
//

typedef enum _CONFIGURATION_TYPE {
    ArcSystem,
    CentralProcessor,
    FloatingPointProcessor,
    PrimaryIcache,
    PrimaryDcache,
    SecondaryIcache,
    SecondaryDcache,
    SecondaryCache,
    EisaAdapter,
    TcAdapter,
    ScsiAdapter,
    DtiAdapter,
    MultiFunctionAdapter,
    DiskController,
    TapeController,
    CdromController,
    WormController,
    SerialController,
    NetworkController,
    DisplayController,
    ParallelController,
    PointerController,
    KeyboardController,
    AudioController,
    OtherController,
    DiskPeripheral,
    FloppyDiskPeripheral,
    TapePeripheral,
    ModemPeripheral,
    MonitorPeripheral,
    PrinterPeripheral,
    PointerPeripheral,
    KeyboardPeripheral,
    TerminalPeripheral,
    OtherPeripheral,
    LinePeripheral,
    NetworkPeripheral,
    SystemMemory,
    MaximumType
} CONFIGURATION_TYPE, *PCONFIGURATION_TYPE;


//
// Interrupt modes.
//

typedef enum _KINTERRUPT_MODE {
    LevelSensitive,
    Latched
    } KINTERRUPT_MODE;

//
// Wait reasons
//

typedef enum _KWAIT_REASON {
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPairHigh,
    WrEventPairLow,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    Spare1,
    Spare2,
    Spare3,
    Spare4,
    Spare5,
    Spare6,
    WrKernel,
    MaximumWaitReason
    } KWAIT_REASON;

//
// Common dispatcher object header
//

typedef struct _DISPATCHER_HEADER {
    CSHORT Type;
    CSHORT Size;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER;

//
// Wait block
//

typedef struct _KWAIT_BLOCK {
    LIST_ENTRY WaitListEntry;
    struct _KTHREAD *Thread;
    PVOID Object;
    struct _KWAIT_BLOCK *NextWaitBlock;
    CSHORT WaitKey;
    WAIT_TYPE WaitType;
} KWAIT_BLOCK, *PKWAIT_BLOCK;

//
// Thread start function
//

typedef
VOID
(*PKSTART_ROUTINE) (
    IN PVOID StartContext
    );

//
// Kernel object structure definitions
//

//
// Device Queue object and entry
//

typedef struct _KDEVICE_QUEUE {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY DeviceListHead;
    KSPIN_LOCK Lock;
    BOOLEAN Busy;
} KDEVICE_QUEUE, *PKDEVICE_QUEUE;

typedef struct _KDEVICE_QUEUE_ENTRY {
    LIST_ENTRY DeviceListEntry;
    ULONG SortKey;
    BOOLEAN Inserted;
} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY;

//
// Event object
//

typedef struct _KEVENT {
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT;

//
// Define the interrupt service function type and the empty struct
// type.
//
typedef
BOOLEAN
(*PKSERVICE_ROUTINE) (
    IN struct _KINTERRUPT *Interrupt,
    IN PVOID ServiceContext
    );
//
// Mutex object
//

typedef struct _KMUTEX {
    DISPATCHER_HEADER Header;
    LIST_ENTRY MutexListEntry;
    struct _KTHREAD *OwnerThread;
    ULONG Level;
} KMUTEX;

typedef KMUTEX *PKMUTEX;

//
// Power notify object
//

typedef struct _KPOWER_NOTIFY {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY NotifyListEntry;
    PKNOTIFY_ROUTINE NotifyRoutine;
    PVOID NotifyContext;
    BOOLEAN Inserted;
} KPOWER_NOTIFY;

typedef KPOWER_NOTIFY *PKPOWER_NOTIFY;

//
// Power status object
//

typedef struct _KPOWER_STATUS {
    CSHORT Type;
    CSHORT Size;
    LIST_ENTRY StatusListEntry;
    PBOOLEAN Status;
    BOOLEAN Inserted;
} KPOWER_STATUS;

typedef KPOWER_STATUS *PKPOWER_STATUS;

//
// Semaphore object
//

typedef struct _KSEMAPHORE {
    DISPATCHER_HEADER Header;
    LONG Limit;
} KSEMAPHORE;

typedef KSEMAPHORE *PKSEMAPHORE;

//
// Timer object
//

typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    BOOLEAN Inserted;
} KTIMER;

typedef KTIMER *PKTIMER;

//
// DPC object
//

VOID
KeInitializeDpc (
    IN PKDPC Dpc,
    IN PKDEFERRED_ROUTINE DeferredRoutine,
    IN PVOID DeferredContext
    );

BOOLEAN
KeInsertQueueDpc (
    IN PKDPC Dpc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

BOOLEAN
KeRemoveQueueDpc (
    IN PKDPC Dpc
    );

//
// Device queue object
//

VOID
KeInitializeDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

BOOLEAN
KeInsertDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

BOOLEAN
KeInsertByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry,
    IN ULONG SortKey
    );

PKDEVICE_QUEUE_ENTRY
KeRemoveDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue
    );

PKDEVICE_QUEUE_ENTRY
KeRemoveByKeyDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN ULONG SortKey
    );

BOOLEAN
KeRemoveEntryDeviceQueue (
    IN PKDEVICE_QUEUE DeviceQueue,
    IN PKDEVICE_QUEUE_ENTRY DeviceQueueEntry
    );

BOOLEAN                                             
KeSynchronizeExecution (                            
    IN PKINTERRUPT Interrupt,                       
    IN PKSYNCHRONIZE_ROUTINE SynchronizeRoutine,    
    IN PVOID SynchronizeContext                     
    );                                              
                                                    
//
// Kernel dispatcher object functions
//
// Event Object
//

VOID
KeInitializeEvent (
    IN PKEVENT Event,
    IN EVENT_TYPE Type,
    IN BOOLEAN State
    );

LONG
KeReadStateEvent (
    IN PKEVENT Event
    );

LONG
KeResetEvent (
    IN PKEVENT Event
    );

LONG
KeSetEvent (
    IN PKEVENT Event,
    IN KPRIORITY Increment,
    IN BOOLEAN Wait
    );

//
// Mutex object
//

VOID
KeInitializeMutex (
    IN PKMUTEX Mutex,
    IN ULONG Level
    );

LONG
KeReadStateMutex (
    IN PKMUTEX
    );

LONG
KeReleaseMutex (
    IN PKMUTEX Mutex,
    IN BOOLEAN Wait
    );

//
// Semaphore object
//

VOID
KeInitializeSemaphore (
    IN PKSEMAPHORE Semaphore,
    IN LONG Count,
    IN LONG Limit
    );

LONG
KeReadStateSemaphore (
    IN PKSEMAPHORE Semaphore
    );

LONG
KeReleaseSemaphore (
    IN PKSEMAPHORE Semaphore,
    IN KPRIORITY Increment,
    IN LONG Adjustment,
    IN BOOLEAN Wait
    );

NTSTATUS                                            
KeDelayExecutionThread (                            
    IN KPROCESSOR_MODE WaitMode,                    
    IN BOOLEAN Alertable,                           
    IN PLARGE_INTEGER Interval                      
    );                                              
                                                    
LONG                                                
KeSetBasePriorityThread (                           
    IN PKTHREAD Thread,                             
    IN LONG Increment                               
    );                                              
                                                    
KPRIORITY                                           
KeSetPriorityThread (                               
    IN PKTHREAD Thread,                             
    IN KPRIORITY Priority                           
    );                                              
                                                    
//
// Timer object
//

VOID
KeInitializeTimer (
    IN PKTIMER Timer
    );

BOOLEAN
KeCancelTimer (
    IN PKTIMER
    );

BOOLEAN
KeReadStateTimer (
    PKTIMER Timer
    );

BOOLEAN
KeSetTimer (
    IN PKTIMER Timer,
    IN LARGE_INTEGER DueTime,
    IN PKDPC Dpc OPTIONAL
    );


NTSTATUS
KeWaitForMultipleObjects (
    IN ULONG Count,
    IN PVOID Object[],
    IN WAIT_TYPE WaitType,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL,
    IN PKWAIT_BLOCK WaitBlockArray OPTIONAL
    );

NTSTATUS
KeWaitForSingleObject (
    IN PVOID Object,
    IN KWAIT_REASON WaitReason,
    IN KPROCESSOR_MODE WaitMode,
    IN BOOLEAN Alertable,
    IN PLARGE_INTEGER Timeout OPTIONAL
    );

//
// spin lock functions
//

VOID
KeInitializeSpinLock (
    IN PKSPIN_LOCK SpinLock
    );

VOID
KeAcquireSpinLock (
    IN PKSPIN_LOCK SpinLock,
    OUT PKIRQL OldIrql
    );

VOID
KeReleaseSpinLock (
    IN PKSPIN_LOCK SpinLock,
    IN KIRQL NewIrql
    );

VOID
KeAcquireSpinLockAtDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );

VOID
KeReleaseSpinLockFromDpcLevel (
    IN PKSPIN_LOCK SpinLock
    );


//
// Miscellaneous kernel functions
//

VOID
KeBugCheck (
    IN ULONG BugCheckCode
    );

VOID
KeBugCheckEx(
    IN ULONG BugCheckCode,
    IN ULONG BugCheckParameter1,
    IN ULONG BugCheckParameter2,
    IN ULONG BugCheckParameter3,
    IN ULONG BugCheckParameter4
    );

VOID
KeEnterKernelDebugger (
    VOID
    );


VOID
KeQuerySystemTime (
    OUT PLARGE_INTEGER CurrentTime
    );

VOID
KeQueryTickCount (
    OUT PLARGE_INTEGER CurrentCount
    );

ULONG
KeQueryTimeIncrement (
    VOID
    );

//
// Define external data.
//

extern BOOLEAN KdDebuggerNotPresent;
extern BOOLEAN KdDebuggerEnabled;

//
// Interlocked support routine definitions.
//

LARGE_INTEGER
ExInterlockedAddLargeInteger (
    IN PLARGE_INTEGER Addend,
    IN LARGE_INTEGER Increment,
    IN PKSPIN_LOCK Lock
    );

ULONG
ExInterlockedAddUlong (
    IN PULONG Addend,
    IN ULONG Increment,
    IN PKSPIN_LOCK Lock
    );

PLIST_ENTRY
ExInterlockedInsertHeadList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

PLIST_ENTRY
ExInterlockedInsertTailList (
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

PLIST_ENTRY
ExInterlockedRemoveHeadList (
    IN PLIST_ENTRY ListHead,
    IN PKSPIN_LOCK Lock
    );

PSINGLE_LIST_ENTRY
ExInterlockedPopEntryList (
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PKSPIN_LOCK Lock
    );

PSINGLE_LIST_ENTRY
ExInterlockedPushEntryList (
    IN PSINGLE_LIST_ENTRY ListHead,
    IN PSINGLE_LIST_ENTRY ListEntry,
    IN PKSPIN_LOCK Lock
    );

INTERLOCKED_RESULT
ExInterlockedIncrementLong (
    IN PLONG Addend,
    IN PKSPIN_LOCK Lock
    );

INTERLOCKED_RESULT
ExInterlockedDecrementLong (
    IN PLONG Addend,
    IN PKSPIN_LOCK Lock
    );

ULONG
ExInterlockedExchangeUlong (
    IN PULONG Target,
    IN ULONG Value,
    IN PKSPIN_LOCK Lock
    );

//
// Pool Allocation routines (in pool.c)
//

typedef enum _POOL_TYPE {
    NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed,
    DontUseThisType,
    NonPagedPoolCacheAligned,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS,
    MaxPoolType
    } POOL_TYPE;


PVOID
ExAllocatePool(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes
    );

PVOID
ExAllocatePoolWithQuota(
    IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes
    );

VOID
ExFreePool(
    IN PVOID P
    );

//
// Worker Thread
//

typedef enum _WORK_QUEUE_TYPE {
    CriticalWorkQueue,
    DelayedWorkQueue,
    MaximumWorkQueue
} WORK_QUEUE_TYPE;

typedef
VOID
(*PWORKER_THREAD_ROUTINE)(
    IN PVOID Parameter
    );

typedef struct _WORK_QUEUE_ITEM {
    LIST_ENTRY List;
    PWORKER_THREAD_ROUTINE WorkerRoutine;
    PVOID Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;


#define ExInitializeWorkItem(Item, Routine, Context) \
    (Item)->WorkerRoutine = (Routine);               \
    (Item)->Parameter = (Context);                   \
    (Item)->List.Flink = NULL;

VOID
ExQueueWorkItem(
    IN PWORK_QUEUE_ITEM WorkItem,
    IN WORK_QUEUE_TYPE QueueType
    );

//
// Zone Allocation
//

typedef struct _ZONE_SEGMENT_HEADER {
    SINGLE_LIST_ENTRY SegmentList;
    PVOID Reserved;
} ZONE_SEGMENT_HEADER, *PZONE_SEGMENT_HEADER;

typedef struct _ZONE_HEADER {
    SINGLE_LIST_ENTRY FreeList;
    SINGLE_LIST_ENTRY SegmentList;
    ULONG BlockSize;
    ULONG TotalSegmentSize;
} ZONE_HEADER, *PZONE_HEADER;


NTSTATUS
ExInitializeZone(
    IN PZONE_HEADER Zone,
    IN ULONG BlockSize,
    IN PVOID InitialSegment,
    IN ULONG InitialSegmentSize
    );

NTSTATUS
ExExtendZone(
    IN PZONE_HEADER Zone,
    IN PVOID Segment,
    IN ULONG SegmentSize
    );

//++
//
// PVOID
// ExAllocateFromZone(
//     IN PZONE_HEADER Zone
//     )
//
// Routine Description:
//
//     This routine removes an entry from the zone and returns a pointer to it.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage from which the
//         entry is to be allocated.
//
// Return Value:
//
//     The function value is a pointer to the storage allocated from the zone.
//
//--

#define ExAllocateFromZone(Zone) \
    (PVOID)((Zone)->FreeList.Next); \
    if ( (Zone)->FreeList.Next ) (Zone)->FreeList.Next = (Zone)->FreeList.Next->Next


//++
//
// PVOID
// ExFreeToZone(
//     IN PZONE_HEADER Zone,
//     IN PVOID Block
//     )
//
// Routine Description:
//
//     This routine places the specified block of storage back onto the free
//     list in the specified zone.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage to which the
//         entry is to be inserted.
//
//     Block - Pointer to the block of storage to be freed back to the zone.
//
// Return Value:
//
//     Pointer to previous block of storage that was at the head of the free
//         list.  NULL implies the zone went from no available free blocks to
//         at least one free block.
//
//--

#define ExFreeToZone(Zone,Block)                                    \
    ( ((PSINGLE_LIST_ENTRY)(Block))->Next = (Zone)->FreeList.Next,  \
      (Zone)->FreeList.Next = ((PSINGLE_LIST_ENTRY)(Block)),        \
      ((PSINGLE_LIST_ENTRY)(Block))->Next                           \
    )


//++
//
// BOOLEAN
// ExIsFullZone(
//     IN PZONE_HEADER Zone
//     )
//
// Routine Description:
//
//     This routine determines if the specified zone is full or not.  A zone
//     is considered full if the free list is empty.
//
// Arguments:
//
//     Zone - Pointer to the zone header to be tested.
//
// Return Value:
//
//     TRUE if the zone is full and FALSE otherwise.
//
//--

#define ExIsFullZone(Zone) \
    ( (Zone)->FreeList.Next == (PSINGLE_LIST_ENTRY)NULL )

//++
//
// PVOID
// ExInterlockedAllocateFromZone(
//     IN PZONE_HEADER Zone,
//     IN PKSPIN_LOCK Lock
//     )
//
// Routine Description:
//
//     This routine removes an entry from the zone and returns a pointer to it.
//     The removal is performed with the specified lock owned for the sequence
//     to make it MP-safe.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage from which the
//         entry is to be allocated.
//
//     Lock - Pointer to the spin lock which should be obtained before removing
//         the entry from the allocation list.  The lock is released before
//         returning to the caller.
//
// Return Value:
//
//     The function value is a pointer to the storage allocated from the zone.
//
//--

#define ExInterlockedAllocateFromZone(Zone,Lock) \
    (PVOID) ExInterlockedPopEntryList( &(Zone)->FreeList, Lock )

//++
//
// PVOID
// ExInterlockedFreeToZone(
//     IN PZONE_HEADER Zone,
//     IN PVOID Block,
//     IN PKSPIN_LOCK Lock
//     )
//
// Routine Description:
//
//     This routine places the specified block of storage back onto the free
//     list in the specified zone.  The insertion is performed with the lock
//     owned for the sequence to make it MP-safe.
//
// Arguments:
//
//     Zone - Pointer to the zone header controlling the storage to which the
//         entry is to be inserted.
//
//     Block - Pointer to the block of storage to be freed back to the zone.
//
//     Lock - Pointer to the spin lock which should be obtained before inserting
//         the entry onto the free list.  The lock is released before returning
//         to the caller.
//
// Return Value:
//
//     Pointer to previous block of storage that was at the head of the free
//         list.  NULL implies the zone went from no available free blocks to
//         at least one free block.
//
//--

#define ExInterlockedFreeToZone(Zone,Block,Lock) \
    ExInterlockedPushEntryList( &(Zone)->FreeList, ((PSINGLE_LIST_ENTRY) (Block)), Lock )

//
//  Shared resource function definitions (in resource.c).
//
//  Note that this structure is opaque, but we'll define it here
//  so drivers can allocate the proper amount of memory for a resource.
//

typedef ULONG ERESOURCE_THREAD;
typedef ERESOURCE_THREAD *PERESOURCE_THREAD;

typedef struct _ERESOURCE {

    //
    //  First 8 bytes are used to align the next part of the structure
    //  onto 16 bytes.  (typical case)
    //

    LIST_ENTRY          SystemResourcesList;

    //
    //  Next 128 bits of this structure are field which we know
    //  we will hit to obtain this resource either shared or exclusive
    //

    PERESOURCE_THREAD   OwnerThreads;
    PUCHAR              OwnerCounts;

    USHORT              TableSize;
    USHORT              ActiveCount;

    USHORT              Flag;
    USHORT              TableRover;                 // (0 - 128 bits)

    //
    //  Next 128 bits contain the initial counters and at least the
    //  first initial thread (which is also highly updated)
    //

    UCHAR               InitialOwnerCounts[4];
    ERESOURCE_THREAD    InitialOwnerThreads[4];

    ULONG               Spare1;

    //
    //  The rest is what ever was left.  The spinlock is in with
    //  a part of the stucture we normally don't touch in the
    //  hot paths (read or write)
    //

    ULONG               ContentionCount;

    USHORT              NumberOfExclusiveWaiters;
    USHORT              NumberOfSharedWaiters;

    KSEMAPHORE          SharedWaiters;
    KEVENT              ExclusiveWaiters;

    KSPIN_LOCK          SpinLock;

#ifdef _X86_
    ULONG CreatorBackTraceIndex;
    USHORT Depth;
    USHORT Reserved;
    PVOID OwnerBackTrace[ 4 ];
#else
    ULONG Spare[ 6 ];
#endif

} ERESOURCE;
typedef ERESOURCE *PERESOURCE;

//
//  Values for ERESOURCE.Flag
//
#define ResourceNeverExclusive          0x10
#define ResourceReleaseByOtherThread    0x20
#define ResourceOwnedExclusive          0x80

NTSTATUS
ExInitializeResource(
    IN PERESOURCE Resource
    );


BOOLEAN
ExAcquireResourceExclusive(
    IN PERESOURCE Resource,
    IN BOOLEAN Wait
    );

//
//  VOID
//  ExReleaseResource(
//      IN PERESOURCE Resource
//      );
//

#define ExReleaseResource(R) (ExReleaseResourceForThread(R, ExGetCurrentResourceThread()))

VOID
ExReleaseResourceForThread(
    IN PERESOURCE Resource,
    IN ERESOURCE_THREAD ResourceThreadId
    );


NTSTATUS
ExDeleteResource (
    IN PERESOURCE Resource
    );


//
//  ERESOURCE_THREAD
//  ExGetCurrentResourceThread(
//      );
//

#define ExGetCurrentResourceThread() ((ULONG)PsGetCurrentThread())

//
// Priority increment definitions.  The comment for each definition gives
// the names of the system services that use the definition when satisfying
// a wait.
//
//
// Priority increment when no I/O has been done.  This is used by device
// and file system drivers when completing an IRP (IoCompleteRequest).
//

#define IO_NO_INCREMENT                 0

//
// Priority increment for completing CD-ROM I/O.  This is used by CD-ROM device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_CD_ROM_INCREMENT             1

//
// Priority increment for completing disk I/O.  This is used by disk device
// and file system drivers when completing an IRP (IoCompleteRequest)
//

#define IO_DISK_INCREMENT               1

//
// Priority increment for completing keyboard I/O.  This is used by keyboard
// device drivers when completing an IRP (IoCompleteRequest)
//

#define IO_KEYBOARD_INCREMENT           6

//
// Priority increment for completing mailslot I/O.  This is used by the mail-
// slot file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_MAILSLOT_INCREMENT           2

//
// Priority increment for completing mouse I/O.  This is used by mouse device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_MOUSE_INCREMENT              6

//
// Priority increment for completing named pipe I/O.  This is used by the
// named pipe file system driver when completing an IRP (IoCompleteRequest).
//

#define IO_NAMED_PIPE_INCREMENT         2

//
// Priority increment for completing network I/O.  This is used by network
// device and network file system drivers when completing an IRP
// (IoCompleteRequest).
//

#define IO_NETWORK_INCREMENT            2

//
// Priority increment for completing parallel I/O.  This is used by parallel
// device drivers when completing an IRP (IoCompleteRequest)
//

#define IO_PARALLEL_INCREMENT           1

//
// Priority increment for completing serial I/O.  This is used by serial device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_SERIAL_INCREMENT             2

//
// Priority increment for completing sound I/O.  This is used by sound device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_SOUND_INCREMENT              8

//
// Priority increment for completing video I/O.  This is used by video device
// drivers when completing an IRP (IoCompleteRequest)
//

#define IO_VIDEO_INCREMENT              1

//
// Priority increment used when satisfying a wait on an executive semaphore
// (NtReleaseSemaphore)
//

#define SEMAPHORE_INCREMENT             1

//
// Define maximum disk transfer size to be used by MM and Cache Manager,
// so that packet-oriented disk drivers can optimize their packet allocation
// to this size.
//

#define MM_MAXIMUM_DISK_IO_SIZE          (0x10000)

//++
//
// ULONG
// ROUND_TO_PAGES (
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The ROUND_TO_PAGES macro takes a size in bytes and rounds it up to a
//     multiple of the page size.
//
//     NOTE: This macro fails for values 0xFFFFFFFF - (PAGE_SIZE - 1).
//
// Arguments:
//
//     Size - Size in bytes to round up to a page multiple.
//
// Return Value:
//
//     Returns the size rounded up to a multiple of the page size.
//
//--

#define ROUND_TO_PAGES(Size)  (((ULONG)(Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

//++
//
// ULONG
// BYTES_TO_PAGES (
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The BYTES_TO_PAGES macro takes the size in bytes and calculates the
//     number of pages required to contain the bytes.
//
// Arguments:
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages required to contain the specified size.
//
//--

#define BYTES_TO_PAGES(Size)  (((ULONG)(Size) >> PAGE_SHIFT) + \
                               (((ULONG)(Size) & (PAGE_SIZE - 1)) != 0))

//++
//
// ULONG
// BYTE_OFFSET (
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The BYTE_OFFSET macro takes a virtual address and returns the byte offset
//     of that address within the page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the byte offset portion of the virtual address.
//
//--

#define BYTE_OFFSET(Va) ((ULONG)(Va) & (PAGE_SIZE - 1))

//++
//
// PVOID
// PAGE_ALIGN (
//     IN PVOID Va
//     )
//
// Routine Description:
//
//     The PAGE_ALIGN macro takes a virtual address and returns a page-aligned
//     virtual address for that page.
//
// Arguments:
//
//     Va - Virtual address.
//
// Return Value:
//
//     Returns the page aligned virtual address.
//
//--

#define PAGE_ALIGN(Va) ((PVOID)((ULONG)(Va) & ~(PAGE_SIZE - 1)))

//++
//
// ULONG
// ADDRESS_AND_SIZE_TO_SPAN_PAGES (
//     IN PVOID Va,
//     IN ULONG Size
//     )
//
// Routine Description:
//
//     The ADDRESS_AND_SIZE_TO_SPAN_PAGES macro takes a virtual address and
//     size and returns the number of pages spanned by the size.
//
// Arguments:
//
//     Va - Virtual address.
//
//     Size - Size in bytes.
//
// Return Value:
//
//     Returns the number of pages spanned by the size.
//
//--

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   ((((ULONG)((ULONG)(Size) - 1L) >> PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(PAGE_SIZE-1)) + ((ULONG)Va & (PAGE_SIZE -1)))) >> PAGE_SHIFT)) + 1L)


//++
//
// PVOID
// MmGetMdlVirtualAddress (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlVirtualAddress returns the virual address of the buffer
//     described by the Mdl.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the virtual address of the buffer described by the Mdl
//
//--

#define MmGetMdlVirtualAddress(Mdl)  ((PVOID) ((PCHAR) (Mdl)->StartVa + (Mdl)->ByteOffset))

//++
//
// ULONG
// MmGetMdlByteCount (
//     IN PMDL Mdl
//     )
//
// Routine Description:
//
//     The MmGetMdlByteCount returns the length in bytes of the buffer
//     described by the Mdl.
//
// Arguments:
//
//     Mdl - Pointer to an MDL.
//
// Return Value:
//
//     Returns the byte count of the buffer described by the Mdl
//
//--

#define MmGetMdlByteCount(Mdl)  ((Mdl)->ByteCount)

typedef enum _MM_SYSTEM_SIZE {
    MmSmallSystem,
    MmMediumSystem,
    MmLargeSystem
} MM_SYSTEMSIZE;

MM_SYSTEMSIZE
MmQuerySystemSize(
    VOID
    );

typedef enum _LOCK_OPERATION {
    IoReadAccess,
    IoWriteAccess,
    IoModifyAccess
} LOCK_OPERATION;

//
// I/O support routines.
//

VOID
MmProbeAndLockPages (
    IN OUT PMDL MemoryDescriptorList,
    IN KPROCESSOR_MODE AccessMode,
    IN LOCK_OPERATION Operation
    );

VOID
MmUnlockPages (
    IN PMDL MemoryDescriptorList
    );

VOID
MmBuildMdlForNonPagedPool (
    IN OUT PMDL MemoryDescriptorList
    );

PVOID
MmMapLockedPages (
    IN PMDL MemoryDescriptorList,
    IN KPROCESSOR_MODE AccessMode
    );

VOID
MmUnmapLockedPages (
    IN PVOID BaseAddress,
    IN PMDL MemoryDescriptorList
    );


PVOID
MmMapIoSpace (
    IN PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG NumberOfBytes,
    IN BOOLEAN CacheEnable
    );

VOID
MmUnmapIoSpace (
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes
    );

PHYSICAL_ADDRESS
MmGetPhysicalAddress (
    IN PVOID BaseAddress
    );

PVOID
MmAllocateContiguousMemory (
    IN ULONG NumberOfBytes,
    IN PHYSICAL_ADDRESS HighestAcceptableAddress
    );

VOID
MmFreeContiguousMemory (
    IN PVOID BaseAddress
    );

PVOID
MmAllocateNonCachedMemory (
    IN ULONG NumberOfBytes
    );

VOID
MmFreeNonCachedMemory (
    IN PVOID BaseAddress,
    IN ULONG NumberOfBytes
    );

BOOLEAN
MmIsAddressValid (
    IN PVOID VirtualAddress
    );

BOOLEAN
MmIsNonPagedSystemAddressValid (
    IN PVOID VirtualAddress
    );


ULONG
MmSizeOfMdl(
    IN PVOID Base,
    IN ULONG Length
    );

PMDL
MmCreateMdl(
    IN PMDL MemoryDescriptorList OPTIONAL,
    IN PVOID Base,
    IN ULONG Length
    );

//++
//
// VOID
// MmInitializeMdl (
//     IN PMDL MemoryDescriptorList,
//     IN PVOID BaseVa,
//     IN ULONG Length
//     )
//
// Routine Description:
//
//     This routine initializes the header of a Memory Descriptor List (MDL).
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL to initialize.
//
//     BaseVa - Base virtual address mapped by the MDL.
//
//     Length - Length, in bytes, of the buffer mapped by the MDL.
//
// Return Value:
//
//     None.
//
//--

#define MmInitializeMdl(MemoryDescriptorList, BaseVa, Length) { \
    (MemoryDescriptorList)->Next = (PMDL) NULL; \
    (MemoryDescriptorList)->Size = (CSHORT)(sizeof(MDL) +  \
            (sizeof(ULONG) * ADDRESS_AND_SIZE_TO_SPAN_PAGES((BaseVa), (Length)))); \
    (MemoryDescriptorList)->MdlFlags = 0; \
    (MemoryDescriptorList)->StartVa = (PVOID) PAGE_ALIGN((BaseVa)); \
    (MemoryDescriptorList)->ByteOffset = BYTE_OFFSET((BaseVa)); \
    (MemoryDescriptorList)->ByteCount = (Length); \
    }

//++
//
// PVOID
// MmGetSystemAddressForMdl (
//     IN PMDL MDL
//     )
//
// Routine Description:
//
//     This routine returns the mapped address of an MDL, if the
//     Mdl is not already mapped or a system address, it is mapped.
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL to map.
//
// Return Value:
//
//     Returns the base address where the pages are mapped.  The base address
//     has the same offset as the virtual address in the MDL.
//
//--

//#define MmGetSystemAddressForMdl(MDL)
//     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA)) ?
//                             ((MDL)->MappedSystemVa) :
//                ((((MDL)->MdlFlags & (MDL_SOURCE_IS_NONPAGED_POOL)) ?
//                      ((PVOID)((ULONG)(MDL)->StartVa | (MDL)->ByteOffset)) :
//                            (MmMapLockedPages((MDL),KernelMode)))))

#define MmGetSystemAddressForMdl(MDL)                                  \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (MmMapLockedPages((MDL),KernelMode)))

//++
//
// VOID
// MmPrepareMdlForReuse (
//     IN PMDL MDL
//     )
//
// Routine Description:
//
//     This routine will take all of the steps necessary to allow an MDL to be
//     re-used.
//
// Arguments:
//
//     MemoryDescriptorList - Pointer to the MDL that will be re-used.
//
// Return Value:
//
//     None.
//
//--

#define MmPrepareMdlForReuse(MDL)                                       \
    if (((MDL)->MdlFlags & MDL_PARTIAL_HAS_BEEN_MAPPED) != 0) {         \
        ASSERT(((MDL)->MdlFlags & MDL_PARTIAL) != 0);                   \
        MmUnmapLockedPages( (MDL)->MappedSystemVa, (MDL) );             \
    } else if (((MDL)->MdlFlags & MDL_PARTIAL) == 0) {                  \
        ASSERT(((MDL)->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) == 0);       \
    }


//
//  Security operation codes
//

typedef enum _SECURITY_OPERATION_CODE {
    SetSecurityDescriptor,
    QuerySecurityDescriptor,
    DeleteSecurityDescriptor,
    AssignSecurityDescriptor
    } SECURITY_OPERATION_CODE, *PSECURITY_OPERATION_CODE;

//
//  Data structure used to capture subject security context
//  for access validations and auditing.
//
//  THE FIELDS OF THIS DATA STRUCTURE SHOULD BE CONSIDERED OPAQUE
//  BY ALL EXCEPT THE SECURITY ROUTINES.
//

typedef struct _SECURITY_SUBJECT_CONTEXT {
    PACCESS_TOKEN ClientToken;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    PACCESS_TOKEN PrimaryToken;
    PVOID ProcessAuditId;
    } SECURITY_SUBJECT_CONTEXT, *PSECURITY_SUBJECT_CONTEXT;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                  ACCESS_STATE and related structures                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
//  Initial Privilege Set - Room for three privileges, which should
//  be enough for most applications.  This structure exists so that
//  it can be imbedded in an ACCESS_STATE structure.  Use PRIVILEGE_SET
//  for all other references to Privilege sets.
//

#define INITIAL_PRIVILEGE_COUNT         3

typedef struct _INITIAL_PRIVILEGE_SET {
    ULONG PrivilegeCount;
    ULONG Control;
    LUID_AND_ATTRIBUTES Privilege[INITIAL_PRIVILEGE_COUNT];
    } INITIAL_PRIVILEGE_SET, * PINITIAL_PRIVILEGE_SET;



//
// Combine the information that describes the state
// of an access-in-progress into a single structure
//


typedef struct _ACCESS_STATE {
   LUID OperationID;
   BOOLEAN SecurityEvaluated;
   BOOLEAN AuditHandleCreation;
   BOOLEAN GenerateOnClose;
   BOOLEAN PrivilegesAllocated;
   ULONG Flags;
   ACCESS_MASK RemainingDesiredAccess;
   ACCESS_MASK PreviouslyGrantedAccess;
   ACCESS_MASK OriginalDesiredAccess;
   SECURITY_SUBJECT_CONTEXT SubjectSecurityContext;
   PSECURITY_DESCRIPTOR SecurityDescriptor;
   PPRIVILEGE_SET PrivilegesUsed;
   union {
      INITIAL_PRIVILEGE_SET InitialPrivilegeSet;
      PRIVILEGE_SET PrivilegeSet;
      } Privileges;
   } ACCESS_STATE, *PACCESS_STATE;


NTSTATUS
SeAssignSecurity (
    IN PSECURITY_DESCRIPTOR ParentDescriptor OPTIONAL,
    IN PSECURITY_DESCRIPTOR ExplicitDescriptor,
    OUT PSECURITY_DESCRIPTOR *NewDescriptor,
    IN BOOLEAN IsDirectoryObject,
    IN PSECURITY_SUBJECT_CONTEXT SubjectContext,
    IN PGENERIC_MAPPING GenericMapping,
    IN POOL_TYPE PoolType
    );

NTSTATUS
SeDeassignSecurity (
    IN OUT PSECURITY_DESCRIPTOR *SecurityDescriptor
    );


BOOLEAN
SeAccessCheck (
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSECURITY_SUBJECT_CONTEXT SubjectSecurityContext,
    IN BOOLEAN SubjectContextLocked,
    IN ACCESS_MASK DesiredAccess,
    IN ACCESS_MASK PreviouslyGrantedAccess,
    OUT PPRIVILEGE_SET *Privileges OPTIONAL,
    IN PGENERIC_MAPPING GenericMapping,
    IN KPROCESSOR_MODE AccessMode,
    OUT PACCESS_MASK GrantedAccess,
    OUT PNTSTATUS AccessStatus
    );

BOOLEAN                                                         
SeSinglePrivilegeCheck(                                         
    LUID PrivilegeValue,                                        
    KPROCESSOR_MODE PreviousMode                                
    );                                                          
//
// System Thread and Process Creation and Termination
//

NTSTATUS
PsCreateSystemThread(
    OUT PHANDLE ThreadHandle,
    IN ULONG DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle OPTIONAL,
    OUT PCLIENT_ID ClientId OPTIONAL,
    IN PKSTART_ROUTINE StartRoutine,
    IN PVOID StartContext
    );

NTSTATUS
PsTerminateSystemThread(
    IN NTSTATUS ExitStatus
    );

//
// Define I/O system data structure type codes.  Each major data structure in
// the I/O system has a type code  The type field in each structure is at the
// same offset.  The following values can be used to determine which type of
// data structure a pointer refers to.
//

#define IO_TYPE_ADAPTER                 0x00000001
#define IO_TYPE_CONTROLLER              0x00000002
#define IO_TYPE_DEVICE                  0x00000003
#define IO_TYPE_DRIVER                  0x00000004
#define IO_TYPE_FILE                    0x00000005
#define IO_TYPE_IRP                     0x00000006
#define IO_TYPE_MASTER_ADAPTER          0x00000007
#define IO_TYPE_OPEN_PACKET             0x00000008
#define IO_TYPE_TIMER                   0x00000009
#define IO_TYPE_VPB                     0x0000000a
#define IO_TYPE_ERROR_LOG               0x0000000b
#define IO_TYPE_ERROR_MESSAGE           0x0000000c

//
// Define the major function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//

#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_MAXIMUM_FUNCTION         0x15

//
// Make the Scsi major code the same as internal device control.
//

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

//
// Define the minor function codes for IRPs.  The lower 128 codes, from 0x00 to
// 0x7f are reserved to Microsoft.  The upper 128 codes, from 0x80 to 0xff, are
// reserved to customers of Microsoft.
//

//
// Directory control minor function codes
//

#define IRP_MN_QUERY_DIRECTORY          0x01
#define IRP_MN_NOTIFY_CHANGE_DIRECTORY  0x02

//
// File system control minor function codes.  Note that "user request" is
// assumed to be zero by both the I/O system and file systems.  Do not change
// this value.
//

#define IRP_MN_USER_FS_REQUEST          0x00
#define IRP_MN_MOUNT_VOLUME             0x01
#define IRP_MN_VERIFY_VOLUME            0x02
#define IRP_MN_LOAD_FILE_SYSTEM         0x03

//
// Lock control minor function codes
//

#define IRP_MN_LOCK                     0x01
#define IRP_MN_UNLOCK_SINGLE            0x02
#define IRP_MN_UNLOCK_ALL               0x03
#define IRP_MN_UNLOCK_ALL_BY_KEY        0x04

//
// Read and Write minor function codes for file systems supporting Lan Manager
// software.  All of these subfunction codes are invalid if the file has been
// opened with FO_NO_INTERMEDIATE_BUFFERING.  They are also invalid in combi-
// nation with synchronous calls (Irp Flag or file open option).
//
// Note that "normal" is assumed to be zero by both the I/O system and file
// systems.  Do not change this value.
//

#define IRP_MN_NORMAL                   0x00
#define IRP_MN_DPC                      0x01
#define IRP_MN_MDL                      0x02
#define IRP_MN_COMPLETE                 0x04

#define IRP_MN_MDL_DPC                  (IRP_MN_MDL | IRP_MN_DPC)
#define IRP_MN_COMPLETE_MDL             (IRP_MN_COMPLETE | IRP_MN_MDL)
#define IRP_MN_COMPLETE_MDL_DPC         (IRP_MN_COMPLETE_MDL | IRP_MN_DPC)

//
// Device Control Request minor function codes for SCSI support. Note that
// user requests are assumed to be zero.
//

#define IRP_MN_SCSI_CLASS               0x01

//
// Define option flags for IoCreateFile.  Note that these values must be
// exactly the same as the SL_... flags for a create function.  Note also
// that there are flags that may be passed to IoCreateFile that are not
// placed in the stack location for the create IRP.  These flags start in
// the next byte.
//

#define IO_FORCE_ACCESS_CHECK           0x0001
#define IO_OPEN_PAGING_FILE             0x0002
#define IO_OPEN_TARGET_DIRECTORY        0x0004

//
// Define callout routine type for use in IoQueryDeviceDescription().
//

typedef NTSTATUS (*PIO_QUERY_DEVICE_ROUTINE)(
    IN PVOID Context,
    IN PUNICODE_STRING PathName,
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PKEY_VALUE_FULL_INFORMATION *BusInformation,
    IN CONFIGURATION_TYPE ControllerType,
    IN ULONG ControllerNumber,
    IN PKEY_VALUE_FULL_INFORMATION *ControllerInformation,
    IN CONFIGURATION_TYPE PeripheralType,
    IN ULONG PeripheralNumber,
    IN PKEY_VALUE_FULL_INFORMATION *PeripheralInformation
    );

//
// Defines the order of the information in the array of
// PKEY_VALUE_FULL_INFORMATION.
//

typedef enum _IO_QUERY_DEVICE_DATA_FORMAT {
    IoQueryDeviceIdentifier = 0,
    IoQueryDeviceConfigurationData,
    IoQueryDeviceComponentInformation,
    IoQueryDeviceMaxData
} IO_QUERY_DEVICE_DATA_FORMAT, *PIO_QUERY_DEVICE_DATA_FORMAT;

//
// Define the objects that can be created by IoCreateFile.
//

typedef enum _CREATE_FILE_TYPE {
    CreateFileTypeNone,
    CreateFileTypeNamedPipe,
    CreateFileTypeMailslot
} CREATE_FILE_TYPE;

//
// Define the structures used by the I/O system
//

//
// Define empty typedefs for the _IRP, _DEVICE_OBJECT, and _DRIVER_OBJECT
// structures so they may be referenced by function types before they are
// actually defined.
//

struct _DEVICE_OBJECT;
struct _DRIVER_OBJECT;
struct _DRIVE_LAYOUT_INFORMATION;
struct _DISK_PARTITION;
struct _FILE_OBJECT;
struct _IRP;
struct _SCSI_REQUEST_BLOCK;

//
// Define the I/O version of a DPC routine.
//

typedef
VOID
(*PIO_DPC_ROUTINE) (
    IN PKDPC Dpc,
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp,
    IN PVOID Context
    );

//
// Define driver timer routine type.
//

typedef
VOID
(*PIO_TIMER_ROUTINE) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN PVOID Context
    );

//
// Define driver initialization routine type.
//

typedef
NTSTATUS
(*PDRIVER_INITIALIZE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

//
// Define driver reinitialization routine type.
//

typedef
VOID
(*PDRIVER_REINITIALIZE) (
    IN struct _DRIVER_OBJECT *DriverObject,
    IN PVOID Context,
    IN ULONG Count
    );

//
// Define driver cancel routine type.
//

typedef
VOID
(*PDRIVER_CANCEL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver dispatch routine type.
//

typedef
NTSTATUS
(*PDRIVER_DISPATCH) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver start I/O routine type.
//

typedef
VOID
(*PDRIVER_STARTIO) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp
    );

//
// Define driver unload routine type.
//

typedef
VOID
(*PDRIVER_UNLOAD) (
    IN struct _DRIVER_OBJECT *DriverObject
    );


//
// Define fast I/O procedure prototypes.
//
// Fast I/O read and write procedures.
//

typedef
BOOLEAN
(*PFAST_IO_CHECK_IF_POSSIBLE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN BOOLEAN CheckForReadOperation,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_READ) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    OUT PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_WRITE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN ULONG Length,
    IN BOOLEAN Wait,
    IN ULONG LockKey,
    IN PVOID Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
// Fast I/O query basic and standard information procedures.
//

typedef
BOOLEAN
(*PFAST_IO_QUERY_BASIC_INFO) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_BASIC_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_QUERY_STANDARD_INFO) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    OUT PFILE_STANDARD_INFORMATION Buffer,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
// Fast I/O lock and unlock procedures.
//

typedef
BOOLEAN
(*PFAST_IO_LOCK) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    BOOLEAN FailImmediately,
    BOOLEAN ExclusiveLock,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_SINGLE) (
    IN struct _FILE_OBJECT *FileObject,
    IN PLARGE_INTEGER FileOffset,
    IN PLARGE_INTEGER Length,
    PEPROCESS ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_ALL) (
    IN struct _FILE_OBJECT *FileObject,
    PEPROCESS ProcessId,
    OUT PIO_STATUS_BLOCK IoStatus
    );

typedef
BOOLEAN
(*PFAST_IO_UNLOCK_ALL_BY_KEY) (
    IN struct _FILE_OBJECT *FileObject,
    PVOID ProcessId,
    ULONG Key,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
// Fast I/O device control procedure.
//

typedef
BOOLEAN
(*PFAST_IO_DEVICE_CONTROL) (
    IN struct _FILE_OBJECT *FileObject,
    IN BOOLEAN Wait,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN ULONG IoControlCode,
    OUT PIO_STATUS_BLOCK IoStatus
    );

//
// Define the structure to describe the Fast I/O dispatch routines.
//

typedef struct _FAST_IO_DISPATCH {
    ULONG SizeOfFastIoDispatch;
    PFAST_IO_CHECK_IF_POSSIBLE FastIoCheckIfPossible;
    PFAST_IO_READ FastIoRead;
    PFAST_IO_WRITE FastIoWrite;
    PFAST_IO_QUERY_BASIC_INFO FastIoQueryBasicInfo;
    PFAST_IO_QUERY_STANDARD_INFO FastIoQueryStandardInfo;
    PFAST_IO_LOCK FastIoLock;
    PFAST_IO_UNLOCK_SINGLE FastIoUnlockSingle;
    PFAST_IO_UNLOCK_ALL FastIoUnlockAll;
    PFAST_IO_UNLOCK_ALL_BY_KEY FastIoUnlockAllByKey;
    PFAST_IO_DEVICE_CONTROL FastIoDeviceControl;
} FAST_IO_DISPATCH, *PFAST_IO_DISPATCH;

//
// Define the actions that a driver execution routine may request of the
// adapter/controller allocation routines upon return.
//

typedef enum _IO_ALLOCATION_ACTION {
    KeepObject = 1,
    DeallocateObject,
    DeallocateObjectKeepRegisters
} IO_ALLOCATION_ACTION, *PIO_ALLOCATION_ACTION;

//
// Define device driver adapter/controller execution routine.
//

typedef
IO_ALLOCATION_ACTION
(*PDRIVER_CONTROL) (
    IN struct _DEVICE_OBJECT *DeviceObject,
    IN struct _IRP *Irp,
    IN PVOID MapRegisterBase,
    IN PVOID Context
    );

//
// Define the I/O system's security context type for use by file system's
// when checking access to volumes, files, and directories.
//

typedef struct _IO_SECURITY_CONTEXT {
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_STATE AccessState;
    ACCESS_MASK DesiredAccess;
} IO_SECURITY_CONTEXT, *PIO_SECURITY_CONTEXT;

//
// Define Volume Parameter Block (VPB) flags.
//

#define VPB_MOUNTED                     0x00000001
#define VPB_LOCKED                      0x00000002

//
// Volume Parameter Block (VPB)
//

#define MAXIMUM_VOLUME_LABEL_LENGTH  (32 * sizeof(WCHAR)) // 32 characters

typedef struct _VPB {
    CSHORT Type;
    CSHORT Size;
    USHORT Flags;
    USHORT VolumeLabelLength; // in bytes
    struct _DEVICE_OBJECT *DeviceObject;
    struct _DEVICE_OBJECT *RealDevice;
    ULONG SerialNumber;
    ULONG ReferenceCount;
    WCHAR VolumeLabel[MAXIMUM_VOLUME_LABEL_LENGTH / sizeof(WCHAR)];
} VPB, *PVPB;

//
// Define object type specific fields of various objects used by the I/O system
//

typedef struct _ADAPTER_OBJECT *PADAPTER_OBJECT;

//
// Define Wait Context Block (WCB)
//

typedef struct _WAIT_CONTEXT_BLOCK {
    KDEVICE_QUEUE_ENTRY WaitQueueEntry;
    PDRIVER_CONTROL DeviceRoutine;
    PVOID DeviceContext;
    ULONG NumberOfMapRegisters;
    PVOID DeviceObject;
    PVOID CurrentIrp;
    PKDPC BufferChainingDpc;
} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK;

typedef struct _CONTROLLER_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PVOID ControllerExtension;
    KDEVICE_QUEUE DeviceWaitQueue;

    ULONG Spare1;
    LARGE_INTEGER Spare2;

} CONTROLLER_OBJECT, *PCONTROLLER_OBJECT;


//
// Define Device Object (DO) flags
//

#define DO_UNLOAD_PENDING               0x00000001
#define DO_VERIFY_VOLUME                0x00000002
#define DO_BUFFERED_IO                  0x00000004
#define DO_EXCLUSIVE                    0x00000008
#define DO_DIRECT_IO                    0x00000010
#define DO_MAP_IO_BUFFER                0x00000020
#define DO_DEVICE_HAS_NAME              0x00000040
#define DO_DEVICE_INITIALIZING          0x00000080
#define DO_SYSTEM_BOOT_PARTITION        0x00000100

//
// Device Object structure definition
//

typedef struct _DEVICE_OBJECT {
    CSHORT Type;
    USHORT Size;
    LONG ReferenceCount;
    struct _DRIVER_OBJECT *DriverObject;
    struct _DEVICE_OBJECT *NextDevice;
    struct _DEVICE_OBJECT *AttachedDevice;
    struct _IRP *CurrentIrp;
    PIO_TIMER Timer;
    ULONG Flags;                                // See above:  DO_...
    ULONG Characteristics;                      // See ntioapi:  FILE_...
    PVPB Vpb;
    PVOID DeviceExtension;
    DEVICE_TYPE DeviceType;
    CCHAR StackSize;
    union {
        LIST_ENTRY ListEntry;
        WAIT_CONTEXT_BLOCK Wcb;
    } Queue;
    ULONG AlignmentRequirement;
    KDEVICE_QUEUE DeviceQueue;
    KDPC Dpc;

    //
    //  The following field is for exclusive use by the filesystem to keep
    //  track of the number of Fsp threads currently using the device
    //

    ULONG ActiveThreadCount;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    KEVENT DeviceLock;

    ULONG Spare1;
    LARGE_INTEGER Spare2;

} DEVICE_OBJECT, *PDEVICE_OBJECT;

//
// Define Driver Object (DRVO) flags
//

#define DRVO_UNLOAD_INVOKED             0x00000001

typedef struct _DRIVER_OBJECT {
    CSHORT Type;
    CSHORT Size;

    //
    // The following links all of the devices created by a single driver
    // together on a list, and the Flags word provides an extensible flag
    // location for driver objects.
    //

    PDEVICE_OBJECT DeviceObject;
    ULONG Flags;

    //
    // The following section describes where the driver is loaded.  The count
    // field is used to count the number of times the driver has had its
    // registered reinitialization routine invoked.
    //

    PVOID DriverStart;
    ULONG DriverSize;
    PVOID DriverSection;
    ULONG Count;

    //
    // The driver name field is used by the error log thread
    // determine the name of the driver that an I/O request is/was bound.
    //

    UNICODE_STRING DriverName;

    //
    // The following section is for registry support.  Thise is a pointer
    // to the path to the hardware information in the registry
    //

    PUNICODE_STRING HardwareDatabase;

    //
    // The following section contains the optional pointer to an array of
    // alternate entry points to a driver for "fast I/O" support.  Fast I/O
    // is performed by invoking the driver routine directly with separate
    // parameters, rather than using the standard IRP call mechanism.  Note
    // that these functions may only be used for synchronous I/O, and when
    // the file is cached.
    //

    PFAST_IO_DISPATCH FastIoDispatch;

    //
    // The following section describes the entry points to this particular
    // driver.  Note that the major function dispatch table must be the last
    // field in the object so that it remains extensible.
    //

    PDRIVER_INITIALIZE DriverInit;
    PDRIVER_STARTIO DriverStartIo;
    PDRIVER_UNLOAD DriverUnload;
    PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
} DRIVER_OBJECT, *PDRIVER_OBJECT;

//
// The following structure is pointed to by the SectionObject pointer field
// of a file object, and is allocated by the various NT file systems.
//

typedef struct _SECTION_OBJECT_POINTERS {
    PVOID DataSectionObject;
    PVOID SharedCacheMap;
    PVOID ImageSectionObject;
} SECTION_OBJECT_POINTERS;
typedef SECTION_OBJECT_POINTERS *PSECTION_OBJECT_POINTERS;

//
// Define File Object (FO) flags
//

#define FO_FILE_OPEN                    0x00000001
#define FO_SYNCHRONOUS_IO               0x00000002
#define FO_ALERTABLE_IO                 0x00000004
#define FO_NO_INTERMEDIATE_BUFFERING    0x00000008
#define FO_WRITE_THROUGH                0x00000010
#define FO_SEQUENTIAL_ONLY              0x00000020
#define FO_CACHE_SUPPORTED              0x00000040
#define FO_NAMED_PIPE                   0x00000080
#define FO_STREAM_FILE                  0x00000100
#define FO_MAILSLOT                     0x00000200
#define FO_GENERATE_AUDIT_ON_CLOSE      0x00000400
#define FO_DIRECT_DEVICE_OPEN           0x00000800
#define FO_FILE_MODIFIED                0x00001000
#define FO_FILE_SIZE_CHANGED            0x00002000
#define FO_CLEANUP_COMPLETE             0x00004000
#define FO_TEMPORARY_FILE               0x00008000
#define FO_DELETE_ON_CLOSE              0x00010000
#define FO_OPENED_CASE_SENSITIVE        0x00020000
#define FO_HANDLE_CREATED               0x00040000
#define FO_FILE_FAST_IO_READ            0x00080000

typedef struct _FILE_OBJECT {
    CSHORT Type;
    CSHORT Size;
    PDEVICE_OBJECT DeviceObject;
    PVPB Vpb;
    PVOID FsContext;
    PVOID FsContext2;
    PSECTION_OBJECT_POINTERS SectionObjectPointer;
    PVOID PrivateCacheMap;
    NTSTATUS FinalStatus;
    struct _FILE_OBJECT *RelatedFileObject;
    BOOLEAN LockOperation;
    BOOLEAN DeletePending;
    BOOLEAN ReadAccess;
    BOOLEAN WriteAccess;
    BOOLEAN DeleteAccess;
    BOOLEAN SharedRead;
    BOOLEAN SharedWrite;
    BOOLEAN SharedDelete;
    ULONG Flags;
    UNICODE_STRING FileName;
    LARGE_INTEGER CurrentByteOffset;
    ULONG Waiters;
    ULONG Busy;
    BOOLEAN Spare1;
    KEVENT Lock;
    KEVENT Event;
} FILE_OBJECT, *PFILE_OBJECT;

//
// Define I/O Request Packet (IRP) flags
//

#define IRP_NOCACHE                     0x00000001
#define IRP_PAGING_IO                   0x00000002
#define IRP_MOUNT_COMPLETION            0x00000002
#define IRP_SYNCHRONOUS_API             0x00000004
#define IRP_ASSOCIATED_IRP              0x00000008
#define IRP_BUFFERED_IO                 0x00000010
#define IRP_DEALLOCATE_BUFFER           0x00000020
#define IRP_INPUT_OPERATION             0x00000040
#define IRP_SYNCHRONOUS_PAGING_IO       0x00000040
#define IRP_CREATE_OPERATION            0x00000080
#define IRP_READ_OPERATION              0x00000100
#define IRP_WRITE_OPERATION             0x00000200
#define IRP_CLOSE_OPERATION             0x00000400
#define IRP_DEFER_IO_COMPLETION         0x00000800
#define IRP_OB_QUERY_NAME               0x00001000

//
// I/O Request Packet (IRP) definition
//

typedef struct _IRP {
    CSHORT Type;
    USHORT Size;

    //
    // Define the common fields used to control the IRP.
    //

    //
    // Define a pointer to the Memory Descriptor List (MDL) for this I/O
    // request.  This field is only used if the I/O is "direct I/O".
    //

    PMDL MdlAddress;

    //
    // Flags word - used to remember various flags.
    //

    ULONG Flags;

    //
    // The following union is used for one of three purposes:
    //
    //    1. This IRP is an associated IRP.  The field is a pointer to a master
    //       IRP.
    //
    //    2. This is the master IRP.  The field is the count of the number of
    //       IRPs which must complete (associated IRPs) before the master can
    //       complete.
    //
    //    3. This operation is being buffered and the field is the address of
    //       the system space buffer.
    //

    union {
        struct _IRP *MasterIrp;
        LONG IrpCount;
        PVOID SystemBuffer;
    } AssociatedIrp;

    //
    // Thread list entry - allows queueing the IRP to the thread pending I/O
    // request packet list.
    //

    LIST_ENTRY ThreadListEntry;

    //
    // I/O status - final status of operation.
    //

    IO_STATUS_BLOCK IoStatus;

    //
    // Requestor mode - mode of the original requestor of this operation.
    //

    KPROCESSOR_MODE RequestorMode;

    //
    // Pending returned - TRUE if pending was initially returned as the
    // status for this packet.
    //

    BOOLEAN PendingReturned;

    //
    // Stack state information.
    //

    CCHAR StackCount;
    CCHAR CurrentLocation;

    //
    // Cancel - packet has been canceled.
    //

    BOOLEAN Cancel;

    //
    // Cancel Irql - Irql at which the cancel spinlock was acquired.
    //

    KIRQL CancelIrql;

    //
    // ApcEnvironment - Used to save the APC environment at the time that the
    // packet was initialized.
    //

    CCHAR ApcEnvironment;

    //
    // Zoned - packet was allocated from a zone.
    //

    BOOLEAN Zoned;

    //
    // User parameters.
    //

    PIO_STATUS_BLOCK UserIosb;
    PKEVENT UserEvent;
    union {
        struct {
            PIO_APC_ROUTINE UserApcRoutine;
            PVOID UserApcContext;
        } AsynchronousParameters;
        LARGE_INTEGER AllocationSize;
    } Overlay;

    //
    // CancelRoutine - Used to contain the address of a cancel routine supplied
    // by a device driver when the IRP is in a cancelable state.
    //

    PDRIVER_CANCEL CancelRoutine;

    //
    // Note that the UserBuffer parameter is outside of the stack so that I/O
    // completion can copy data back into the user's address space without
    // having to know exactly which service was being invoked.  The length
    // of the copy is stored in the second half of the I/O status block. If
    // the UserBuffer field is NULL, then no copy is performed.
    //

    PVOID UserBuffer;

    //
    // Kernel structures
    //
    // The following section contains kernel structures which the IRP needs
    // in order to place various work information in kernel controller system
    // queues.  Because the size and alignment cannot be controlled, they are
    // placed here at the end so they just hang off and do not affect the
    // alignment of other fields in the IRP.
    //

    union {

        struct {

            //
            // DeviceQueueEntry - The device queue entry field is used to queue
            // the IRP to the device driver device queue.
            //

            KDEVICE_QUEUE_ENTRY DeviceQueueEntry;

            //
            // Thread - pointer to caller's Thread Control Block.
            //

            PETHREAD Thread;

            //
            // Auxillary buffer - pointer to any auxillary buffer that is
            // required to pass information to a driver that is not contained
            // in a normal buffer.
            //

            PCHAR AuxiliaryBuffer;

            //
            // List entry - used to queue the packet to completion queue, among
            // others.
            //

            LIST_ENTRY ListEntry;

            //
            // Current stack location - contains a pointer to the current
            // IO_STACK_LOCATION structure in the IRP stack.  This field
            // should never be directly accessed by drivers.  They should
            // use the standard functions.
            //

            struct _IO_STACK_LOCATION *CurrentStackLocation;

            //
            // Original file object - pointer to the original file object
            // that was used to open the file.  This field is owned by the
            // I/O system and should not be used by any other drivers.
            //

            PFILE_OBJECT OriginalFileObject;

        } Overlay;

        //
        // APC - This APC control block is used for the special kernel APC as
        // well as for the caller's APC, if one was specified in the original
        // argument list.  If so, then the APC is reused for the normal APC for
        // whatever mode the caller was in and the "special" routine that is
        // invoked before the APC gets control simply deallocates the IRP.
        //

        KAPC Apc;

    } Tail;

} IRP, *PIRP;

//
// Define completion routine types for use in stack locations in an IRP
//

typedef
NTSTATUS
(*PIO_COMPLETION_ROUTINE) (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

//
// Define stack location control flags
//

#define SL_PENDING_RETURNED             0x01
#define SL_INVOKE_ON_CANCEL             0x20
#define SL_INVOKE_ON_SUCCESS            0x40
#define SL_INVOKE_ON_ERROR              0x80

//
// Define flags for various functions
//

//
// Create / Create Named Pipe
//
// The following flags must exactly match those in the IoCreateFile call's
// options.  The case sensitive flag is added in later, by the parse routine,
// and is not an actual option to open.  Rather, it is part of the object
// manager's attributes structure.
//

#define SL_FORCE_ACCESS_CHECK           0x01
#define SL_OPEN_PAGING_FILE             0x02
#define SL_OPEN_TARGET_DIRECTORY        0x04

#define SL_CASE_SENSITIVE               0x80

//
// Read / Write
//

#define SL_KEY_SPECIFIED                0x01
#define SL_OVERRIDE_VERIFY_VOLUME       0x02
#define SL_WRITE_THROUGH                0x04
#define SL_FT_SEQUENTIAL_WRITE          0x08

//
// Device I/O Control
//
//
// Same SL_OVERRIDE_VERIFY_VOLUME as for read/write above.
//

//
// Lock
//

#define SL_FAIL_IMMEDIATELY             0x01
#define SL_EXCLUSIVE_LOCK               0x02

//
// QueryDirectory / QueryEa
//

#define SL_RESTART_SCAN                 0x01
#define SL_RETURN_SINGLE_ENTRY          0x02
#define SL_INDEX_SPECIFIED              0x04

//
// NotifyDirectory
//

#define SL_WATCH_TREE                   0x01

//
// FileSystemControl
//
//    minor: mount/verify volume
//

#define SL_ALLOW_RAW_MOUNT              0x01

//
// Define I/O Request Packet (IRP) stack locations
//

#if !defined(_ALPHA_)
#pragma pack(4)
#endif
typedef struct _IO_STACK_LOCATION {
    UCHAR MajorFunction;
    UCHAR MinorFunction;
    UCHAR Flags;
    UCHAR Control;

    //
    // The following user parameters are based on the service that is being
    // invoked.  Drivers and file systems can determine which set to use based
    // on the above major and minor function codes.
    //

    union {

        //
        // System service parameters for:  NtCreateFile
        //

        struct {
            PIO_SECURITY_CONTEXT SecurityContext;
            ULONG Options;
            USHORT FileAttributes;
            USHORT ShareAccess;
            ULONG EaLength;
        } Create;


        //
        // System service parameters for:  NtReadFile
        //

        struct {
            ULONG Length;
            ULONG Key;
            LARGE_INTEGER ByteOffset;
        } Read;

        //
        // System service parameters for:  NtWriteFile
        //

        struct {
            ULONG Length;
            ULONG Key;
            LARGE_INTEGER ByteOffset;
        } Write;


        //
        // System service parameters for:  NtQueryInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS FileInformationClass;
        } QueryFile;

        //
        // System service parameters for:  NtSetInformationFile
        //

        struct {
            ULONG Length;
            FILE_INFORMATION_CLASS FileInformationClass;
            PFILE_OBJECT FileObject;
            BOOLEAN ReplaceIfExists;
            BOOLEAN AdvanceOnly;
        } SetFile;


        //
        // System service parameters for:  NtQueryVolumeInformationFile
        //

        struct {
            ULONG Length;
            FS_INFORMATION_CLASS FsInformationClass;
        } QueryVolume;


        //
        // System service parameters for:  NtFlushBuffersFile
        //
        // No extra user-supplied parameters.
        //


        //
        // System service parameters for:  NtDeviceIoControlFile
        //
        // Note that the user's output buffer is stored in the UserBuffer field
        // and the user's input buffer is stored in the SystemBuffer field.
        //

        struct {
            ULONG OutputBufferLength;
            ULONG InputBufferLength;
            ULONG IoControlCode;
            PVOID Type3InputBuffer;
        } DeviceIoControl;

        //
        // System service parameters for:  NtQuerySecurityObject
        //

        struct {
            SECURITY_INFORMATION SecurityInformation;
            ULONG Length;
        } QuerySecurity;

        //
        // System service parameters for:  NtSetSecurityObject
        //

        struct {
            SECURITY_INFORMATION SecurityInformation;
            PSECURITY_DESCRIPTOR SecurityDescriptor;
        } SetSecurity;

        //
        // Non-system service parameters.
        //
        // Parameters for MountVolume
        //

        struct {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } MountVolume;

        //
        // Parameters for VerifyVolume
        //

        struct {
            PVPB Vpb;
            PDEVICE_OBJECT DeviceObject;
        } VerifyVolume;

        //
        // Parameters for Scsi with internal device contorl.
        //

        struct {
            struct _SCSI_REQUEST_BLOCK *Srb;
        } Scsi;

        //
        // Parameters for Cleanup
        //
        // No extra parameters supplied
        //

        //
        // Others - driver-specific
        //

        struct {
            PVOID Argument1;
            PVOID Argument2;
            PVOID Argument3;
            PVOID Argument4;
        } Others;

    } Parameters;

    //
    // Save a pointer to this device driver's device object for this request
    // so it can be passed to the completion routine if needed.
    //

    PDEVICE_OBJECT DeviceObject;

    //
    // The following location contains a pointer to the file object for this
    //

    PFILE_OBJECT FileObject;

    //
    // The following routine is invoked depending on the flags in the above
    // flags field.
    //

    PIO_COMPLETION_ROUTINE CompletionRoutine;

    //
    // The following is used to store the address of the context parameter
    // that should be passed to the CompletionRoutine.
    //

    PVOID Context;

} IO_STACK_LOCATION, *PIO_STACK_LOCATION;
#if !defined(_ALPHA_)
#pragma pack()
#endif

//
// Define the share access structure used by file systems to determine
// whether or not another accessor may open the file.
//

typedef struct _SHARE_ACCESS {
    ULONG OpenCount;
    ULONG Readers;
    ULONG Writers;
    ULONG Deleters;
    ULONG SharedRead;
    ULONG SharedWrite;
    ULONG SharedDelete;
} SHARE_ACCESS, *PSHARE_ACCESS;

// begin_nthal

//
// The following structure is used by drivers that are initializing to
// determine the number of devices of a particular type that have already
// been initialized.  It is also used to track whether or not the AtDisk
// address range has already been claimed.  Finally, it is used by the
// NtQuerySystemInformation system service to return device type counts.
//

typedef struct _CONFIGURATION_INFORMATION {

    //
    // This field indicates the total number of disks in the system.  This
    // number should be used by the driver to determine the name of new
    // disks.  This field should be updated by the driver as it finds new
    // disks.
    //

    ULONG DiskCount;                // Count of hard disks thus far
    ULONG FloppyCount;              // Count of floppy disks thus far
    ULONG CdRomCount;               // Count of CD-ROM drives thus far
    ULONG TapeCount;                // Count of tape drives thus far
    ULONG ScsiPortCount;            // Count of SCSI port adapters thus far
    ULONG SerialCount;              // Count of serial devices thus far
    ULONG ParallelCount;            // Count of parallel devices thus far

    //
    // These next two fields indicate ownership of one of the two IO address
    // spaces that are used by WD1003-compatable disk controllers.
    //

    BOOLEAN AtDiskPrimaryAddressClaimed;    // 0x1F0 - 0x1FF
    BOOLEAN AtDiskSecondaryAddressClaimed;  // 0x170 - 0x17F

} CONFIGURATION_INFORMATION, *PCONFIGURATION_INFORMATION;

//
// Public I/O routine definitions
//

VOID
IoAcquireCancelSpinLock(
    OUT PKIRQL Irql
    );


NTSTATUS
IoAllocateAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG NumberOfMapRegisters,
    IN PDRIVER_CONTROL ExecutionRoutine,
    IN PVOID Context
    );

VOID
IoAllocateController(
    IN PCONTROLLER_OBJECT ControllerObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN PDRIVER_CONTROL ExecutionRoutine,
    IN PVOID Context
    );

PVOID
IoAllocateErrorLogEntry(
    IN PVOID IoObject,
    IN UCHAR EntrySize
    );

PIRP
IoAllocateIrp(
    IN CCHAR StackSize,
    IN BOOLEAN ChargeQuota
    );

PMDL
IoAllocateMdl(
    IN PVOID VirtualAddress,
    IN ULONG Length,
    IN BOOLEAN SecondaryBuffer,
    IN BOOLEAN ChargeQuota,
    IN OUT PIRP Irp OPTIONAL
    );

//++
//
// VOID
// IoAssignArcName(
//     IN PUNICODE_STRING ArcName,
//     IN PUNICODE_STRING DeviceName
//     )
//
// Routine Description:
//
//     This routine is invoked by drivers of bootable media to create a symbolic
//     link between the ARC name of their device and its NT name.  This allows
//     the system to determine which device in the system was actually booted
//     from since the ARC firmware only deals in ARC names, and NT only deals
//     in NT names.
//
// Arguments:
//
//     ArcName - Supplies the Unicode string representing the ARC name.
//
//     DeviceName - Supplies the name to which the ARCname refers.
//
// Return Value:
//
//     None.
//
//--

#define IoAssignArcName( ArcName, DeviceName ) (  \
    IoCreateSymbolicLink( (ArcName), (DeviceName) ) )

NTSTATUS
IoAttachDevice(
    IN PDEVICE_OBJECT SourceDevice,
    IN PUNICODE_STRING TargetDevice,
    OUT PDEVICE_OBJECT *AttachedDevice
    );

NTSTATUS
IoAttachDeviceByPointer(
    IN PDEVICE_OBJECT SourceDevice,
    IN PDEVICE_OBJECT TargetDevice
    );

PIRP
IoBuildAsynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PIO_STATUS_BLOCK IoStatusBlock OPTIONAL
    );

PIRP
IoBuildDeviceIoControlRequest(
    IN ULONG IoControlCode,
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID InputBuffer OPTIONAL,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer OPTIONAL,
    IN ULONG OutputBufferLength,
    IN BOOLEAN InternalDeviceIoControl,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

VOID
IoBuildPartialMdl(
    IN PMDL SourceMdl,
    IN OUT PMDL TargetMdl,
    IN PVOID VirtualAddress,
    IN ULONG Length
    );

PIRP
IoBuildSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL,
    IN PKEVENT Event,
    OUT PIO_STATUS_BLOCK IoStatusBlock
    );

NTSTATUS
IoCallDriver(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

BOOLEAN
IoCancelIrp(
    IN PIRP Irp
    );


NTSTATUS
IoCheckShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess,
    IN BOOLEAN Update
    );

VOID
IoCompleteRequest(
    IN PIRP Irp,
    IN CCHAR PriorityBoost
    );

NTSTATUS
IoConnectInterrupt(
    OUT PKINTERRUPT *InterruptObject,
    IN PKSERVICE_ROUTINE ServiceRoutine,
    IN PVOID ServiceContext,
    IN PKSPIN_LOCK SpinLock OPTIONAL,
    IN ULONG Vector,
    IN KIRQL Irql,
    IN KIRQL SynchronizeIrql,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector,
    IN KAFFINITY ProcessorEnableMask,
    IN BOOLEAN FloatingSave
    );

PCONTROLLER_OBJECT
IoCreateController(
    IN ULONG Size
    );

NTSTATUS
IoCreateDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN ULONG DeviceExtensionSize,
    IN PUNICODE_STRING DeviceName OPTIONAL,
    IN DEVICE_TYPE DeviceType,
    IN ULONG DeviceCharacteristics,
    IN BOOLEAN Exclusive,
    OUT PDEVICE_OBJECT *DeviceObject
    );


NTSTATUS
IoCreateSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    );

PKEVENT
IoCreateSynchronizationEvent(
    IN PUNICODE_STRING EventName,
    OUT PHANDLE EventHandle
    );

NTSTATUS
IoCreateUnprotectedSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName,
    IN PUNICODE_STRING DeviceName
    );

//++
//
// VOID
// IoDeassignArcName(
//     IN PUNICODE_STRING ArcName
//     )
//
// Routine Description:
//
//     This routine is invoked by drivers to deassign an ARC name that they
//     created to a device.  This is generally only called if the driver is
//     deleting the device object, which means that the driver is probably
//     unloading.
//
// Arguments:
//
//     ArcName - Supplies the ARC name to be removed.
//
// Return Value:
//
//     None.
//
//--

#define IoDeassignArcName( ArcName ) (  \
    IoDeleteSymbolicLink( (ArcName) ) )

VOID
IoDeleteController(
    IN PCONTROLLER_OBJECT ControllerObject
    );

VOID
IoDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IoDeleteSymbolicLink(
    IN PUNICODE_STRING SymbolicLinkName
    );

VOID
IoDetachDevice(
    IN OUT PDEVICE_OBJECT TargetDevice
    );

VOID
IoDisconnectInterrupt(
    IN PKINTERRUPT InterruptObject
    );

VOID
IoFreeController(
    IN PCONTROLLER_OBJECT ControllerObject
    );

VOID
IoFreeIrp(
    IN PIRP Irp
    );

VOID
IoFreeMdl(
    IN PMDL Mdl
    );

PCONFIGURATION_INFORMATION                  
IoGetConfigurationInformation( VOID );      

//++
//
// PIO_STACK_LOCATION
// IoGetCurrentIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the current stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the current stack location in the
//     packet.
//
//--

#define IoGetCurrentIrpStackLocation( Irp ) ( (Irp)->Tail.Overlay.CurrentStackLocation )

// end_nthal

PEPROCESS
IoGetCurrentProcess(
    VOID
    );

// begin_nthal

NTSTATUS
IoGetDeviceObjectPointer(
    IN PUNICODE_STRING ObjectName,
    IN ACCESS_MASK DesiredAccess,
    OUT PFILE_OBJECT *FileObject,
    OUT PDEVICE_OBJECT *DeviceObject
    );

//++
//
// PIO_STACK_LOCATION
// IoGetNextIrpStackLocation(
//     IN PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to return a pointer to the next stack location
//     in an I/O Request Packet (IRP).
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet.
//
// Return Value:
//
//     The function value is a pointer to the next stack location in the packet.
//
//--

#define IoGetNextIrpStackLocation( Irp ) (\
    (Irp)->Tail.Overlay.CurrentStackLocation - 1 )

PDEVICE_OBJECT
IoGetRelatedDeviceObject(
    IN PFILE_OBJECT FileObject
    );


//++
//
// VOID
// IoInitializeDpcRequest(
//     IN PDEVICE_OBJECT DeviceObject,
//     IN PIO_DPC_ROUTINE DpcRoutine
//     )
//
// Routine Description:
//
//     This routine is invoked to initialize the DPC in a device object for a
//     device driver during its initialization routine.  The DPC is used later
//     when the driver interrupt service routine requests that a DPC routine
//     be queued for later execution.
//
// Arguments:
//
//     DeviceObject - Pointer to the device object that the request is for.
//
//     DpcRoutine - Address of the driver's DPC routine to be executed when
//         the DPC is dequeued for processing.
//
// Return Value:
//
//     None.
//
//--

#define IoInitializeDpcRequest( DeviceObject, DpcRoutine ) (\
    KeInitializeDpc( &(DeviceObject)->Dpc,                  \
                     (PKDEFERRED_ROUTINE) (DpcRoutine),     \
                     (DeviceObject) ) )

VOID
IoInitializeIrp(
    IN OUT PIRP Irp,
    IN USHORT PacketSize,
    IN CCHAR StackSize
    );

NTSTATUS
IoInitializeTimer(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_TIMER_ROUTINE TimerRoutine,
    IN PVOID Context
    );


//++
//
// BOOLEAN
// IoIsErrorUserInduced(
//     IN NTSTATUS Status
//     )
//
// Routine Description:
//
//     This routine is invoked to determine if an error was as a
//     result of user actions.  Typically these error are related
//     to removable media and will result in a pop-up.
//
// Arguments:
//
//     Status - The status value to check.
//
// Return Value:
//
//     The function value is TRUE if the user induced the error,
//     otherwise FALSE is returned.
//
//--
#define IoIsErrorUserInduced( Status ) ((BOOLEAN)  \
    (((Status) == STATUS_DEVICE_NOT_READY) ||      \
     ((Status) == STATUS_IO_TIMEOUT) ||            \
     ((Status) == STATUS_MEDIA_WRITE_PROTECTED) || \
     ((Status) == STATUS_NO_MEDIA_IN_DEVICE) ||    \
     ((Status) == STATUS_VERIFY_REQUIRED) ||       \
     ((Status) == STATUS_UNRECOGNIZED_MEDIA) ||    \
     ((Status) == STATUS_WRONG_VOLUME)))


PIRP
IoMakeAssociatedIrp(
    IN PIRP Irp,
    IN CCHAR StackSize
    );

//++
//
// VOID
// IoMarkIrpPending(
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine marks the specified I/O Request Packet (IRP) to indicate
//     that an initial status of STATUS_PENDING was returned to the caller.
//     This is used so that I/O completion can determine whether or not to
//     fully complete the I/O operation requested by the packet.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet to be marked pending.
//
// Return Value:
//
//     None.
//
//--

#define IoMarkIrpPending( Irp ) ( \
    IoGetCurrentIrpStackLocation( (Irp) )->Control |= SL_PENDING_RETURNED )

NTSTATUS                                                
IoQueryDeviceDescription(                               
    IN PINTERFACE_TYPE BusType OPTIONAL,                
    IN PULONG BusNumber OPTIONAL,                       
    IN PCONFIGURATION_TYPE ControllerType OPTIONAL,     
    IN PULONG ControllerNumber OPTIONAL,                
    IN PCONFIGURATION_TYPE PeripheralType OPTIONAL,     
    IN PULONG PeripheralNumber OPTIONAL,                
    IN PIO_QUERY_DEVICE_ROUTINE CalloutRoutine,         
    IN PVOID Context                                    
    );                                                  
VOID
IoRaiseHardError(
    IN PIRP Irp,
    IN PVPB Vpb OPTIONAL,
    IN PDEVICE_OBJECT RealDeviceObject
    );

VOID
IoRaiseInformationalHardError(
    IN NTSTATUS ErrorStatus,
    IN PUNICODE_STRING String OPTIONAL,
    IN PKTHREAD Thread OPTIONAL
    );

VOID
IoRegisterDriverReinitialization(
    IN PDRIVER_OBJECT DriverObject,
    IN PDRIVER_REINITIALIZE DriverReinitializationRoutine,
    IN PVOID Context
    );

NTSTATUS
IoRegisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
IoReleaseCancelSpinLock(
    IN KIRQL Irql
    );


VOID
IoRemoveShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    );


NTSTATUS
IoReportResourceUsage(
    IN PUNICODE_STRING DriverClassName OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PCM_RESOURCE_LIST DriverList OPTIONAL,
    IN ULONG DriverListSize OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject,
    IN PCM_RESOURCE_LIST DeviceList OPTIONAL,
    IN ULONG DeviceListSize OPTIONAL,
    IN BOOLEAN OverrideConflict,
    OUT PBOOLEAN ConflictDetected
    );

//++
//
// VOID
// IoRequestDpc(
//     IN PDEVICE_OBJECT DeviceObject,
//     IN PIRP Irp,
//     IN PVOID Context
//     )
//
// Routine Description:
//
//     This routine is invoked by the device driver's interrupt service routine
//     to request that a DPC routine be queued for later execution at a lower
//     IRQL.
//
// Arguments:
//
//     DeviceObject - Device object for which the request is being processed.
//
//     Irp - Pointer to the current I/O Request Packet (IRP) for the specified
//         device.
//
//     Context - Provides a general context parameter to be passed to the
//         DPC routine.
//
// Return Value:
//
//     None.
//
//--

#define IoRequestDpc( DeviceObject, Irp, Context ) ( \
    KeInsertQueueDpc( &(DeviceObject)->Dpc, (Irp), (Context) ) )

//++
//
// VOID
// IoSetCancelRoutine(
//     IN PIRP Irp,
//     IN PDRIVER_CANCEL CancelRoutine
//     )
//
// Routine Description:
//
//     This routine is invoked to set the address of a cancel routine which
//     is to be invoked when an I/O packet has been canceled.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet itself.
//
//     CancelRoutine - Address of the cancel routine that is to be invoked
//         if the IRP is cancelled.
//
// Return Value:
//
//     None.
//
// Note:
//
//     The I/O cancel spin lock must be held when this routine is invoked.
//
//--

#define IoSetCancelRoutine( Irp, NewCancelRoutine ) (  \
    (Irp)->CancelRoutine = (NewCancelRoutine) )

//++
//
// VOID
// IoSetCompletionRoutine(
//     IN PIRP Irp,
//     IN PIO_COMPLETION_ROUTINE CompletionRoutine,
//     IN PVOID Context,
//     IN BOOLEAN InvokeOnSuccess,
//     IN BOOLEAN InvokeOnError,
//     IN BOOLEAN InvokeOnCancel
//     )
//
// Routine Description:
//
//     This routine is invoked to set the address of a completion routine which
//     is to be invoked when an I/O packet has been completed by a lower-level
//     driver.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet itself.
//
//     CompletionRoutine - Address of the completion routine that is to be
//         invoked once the next level driver completes the packet.
//
//     Context - Specifies a context parameter to be passed to the completion
//         routine.
//
//     InvokeOnSuccess - Specifies that the completion routine is invoked when the
//         operation is successfully completed.
//
//     InvokeOnError - Specifies that the completion routine is invoked when the
//         operation completes with an error status.
//
//     InvokeOnCancel - Specifies that the completion routine is invoked when the
//         operation is being canceled.
//
// Return Value:
//
//     None.
//
//--

#define IoSetCompletionRoutine( Irp, Routine, CompletionContext, Success, Error, Cancel ) { \
    PIO_STACK_LOCATION irpSp;                                               \
    ASSERT( (Success) | (Error) | (Cancel) ? (Routine) != NULL : TRUE );    \
    irpSp = IoGetNextIrpStackLocation( (Irp) );                             \
    irpSp->CompletionRoutine = (Routine);                                   \
    irpSp->Context = (CompletionContext);                                   \
    irpSp->Control = 0;                                                     \
    if ((Success)) { irpSp->Control = SL_INVOKE_ON_SUCCESS; }               \
    if ((Error)) { irpSp->Control |= SL_INVOKE_ON_ERROR; }                  \
    if ((Cancel)) { irpSp->Control |= SL_INVOKE_ON_CANCEL; } }

VOID
IoSetHardErrorOrVerifyDevice(
    IN PIRP Irp,
    IN PDEVICE_OBJECT DeviceObject
    );


//++
//
// VOID
// IoSetNextIrpStackLocation (
//     IN OUT PIRP Irp
//     )
//
// Routine Description:
//
//     This routine is invoked to set the current IRP stack location to
//     the next stack location, i.e. it "pushes" the stack.
//
// Arguments:
//
//     Irp - Pointer to the I/O Request Packet (IRP).
//
// Return Value:
//
//     None.
//
//--

#define IoSetNextIrpStackLocation( Irp ) {      \
    (Irp)->CurrentLocation--;                   \
    (Irp)->Tail.Overlay.CurrentStackLocation--; }

VOID
IoSetShareAccess(
    IN ACCESS_MASK DesiredAccess,
    IN ULONG DesiredShareAccess,
    IN OUT PFILE_OBJECT FileObject,
    OUT PSHARE_ACCESS ShareAccess
    );


//++
//
// USHORT
// IoSizeOfIrp(
//     IN CCHAR StackSize
//     )
//
// Routine Description:
//
//     Determines the size of an IRP given the number of stack locations
//     the IRP will have.
//
// Arguments:
//
//     StackSize - Number of stack locations for the IRP.
//
// Return Value:
//
//     Size in bytes of the IRP.
//
//--

#define IoSizeOfIrp( StackSize ) \
    ((USHORT) (sizeof( IRP ) + ((StackSize) * (sizeof( IO_STACK_LOCATION )))))

VOID
IoStartNextPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN Cancelable
    );

VOID
IoStartNextPacketByKey(
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN Cancelable,
    IN ULONG Key
    );

VOID
IoStartPacket(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PULONG Key OPTIONAL,
    IN PDRIVER_CANCEL CancelFunction OPTIONAL
    );

VOID
IoStartTimer(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
IoStopTimer(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
IoUnregisterShutdownNotification(
    IN PDEVICE_OBJECT DeviceObject
    );

VOID
IoUpdateShareAccess(
    IN PFILE_OBJECT FileObject,
    IN OUT PSHARE_ACCESS ShareAccess
    );

VOID                                            
IoWriteErrorLogEntry(                           
    IN PVOID ElEntry                            
    );                                          
//
// The following definitions are for HAL structures.
//

// begin_ntminiport
//
// Define types of bus information.
//

typedef enum _BUS_DATA_TYPE {
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

//
// Define the device description structure.
//

typedef struct _DEVICE_DESCRIPTION {
    ULONG Version;
    BOOLEAN Master;
    BOOLEAN ScatterGather;
    BOOLEAN DemandMode;
    BOOLEAN AutoInitialize;
    BOOLEAN Dma32BitAddresses;
    ULONG BusNumber;
    ULONG DmaChannel;
    INTERFACE_TYPE  InterfaceType;
    DMA_WIDTH DmaWidth;
    DMA_SPEED DmaSpeed;
    ULONG MaximumLength;
    ULONG DmaPort;
} DEVICE_DESCRIPTION, *PDEVICE_DESCRIPTION;

//
// Define the supported version numbers for the device description structure.
//

#define DEVICE_DESCRIPTION_VERSION 0

//
// The following function prototypes are for HAL routines with a prefix of Hal.
//
// General functions.
//

VOID
HalAcquireDisplayOwnership (
    VOID
    );

#if defined(_MIPS_) || defined(_ALPHA_)         
                                                
ULONG                                           
HalGetDmaAlignmentRequirement (                 
    VOID                                        
    );                                          
                                                
#endif                                          
                                                
#if defined(_M_IX86)                            
                                                
#define HalGetDmaAlignmentRequirement() 1L      
                                                
#endif                                          
                                                
VOID                                            
KeFlushWriteBuffer (                            
    VOID                                        
    );                                          
                                                
//
// I/O driver configuration functions.
//

ULONG
HalGetInterruptVector(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN ULONG BusInterruptLevel,
    IN ULONG BusInterruptVector,
    OUT PKIRQL Irql,
    OUT PKAFFINITY Affinity
    );

BOOLEAN
HalTranslateBusAddress(
    IN INTERFACE_TYPE  InterfaceType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS BusAddress,
    IN OUT PULONG AddressSpace,
    OUT PPHYSICAL_ADDRESS TranslatedAddress
    );

PVOID
HalAllocateCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    OUT PPHYSICAL_ADDRESS LogicalAddress,
    IN BOOLEAN CacheEnabled
    );

VOID
HalFreeCommonBuffer(
    IN PADAPTER_OBJECT AdapterObject,
    IN ULONG Length,
    IN PHYSICAL_ADDRESS LogicalAddress,
    IN PVOID VirtualAddress,
    IN BOOLEAN CacheEnabled
    );

ULONG
HalGetBusData(
    IN BUS_DATA_TYPE BusDataType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    IN PVOID Buffer,
    IN ULONG Length
    );

PADAPTER_OBJECT
HalGetAdapter(
    IN PDEVICE_DESCRIPTION DeviceDescription,
    IN OUT PULONG NumberOfMapRegisters
    );

//
// The following function prototypes are for HAL routines with a prefix of Io.
//
// DMA adapter object functions.
//

ULONG
HalReadDmaCounter(
    IN PADAPTER_OBJECT AdapterObject
    );

BOOLEAN
IoFlushAdapterBuffers(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

VOID
IoFreeAdapterChannel(
    IN PADAPTER_OBJECT AdapterObject
    );

VOID
IoFreeMapRegisters(
   IN PADAPTER_OBJECT AdapterObject,
   IN PVOID MapRegisterBase,
   IN ULONG NumberOfMapRegisters
   );

PHYSICAL_ADDRESS
IoMapTransfer(
    IN PADAPTER_OBJECT AdapterObject,
    IN PMDL Mdl,
    IN PVOID MapRegisterBase,
    IN PVOID CurrentVa,
    IN OUT PULONG Length,
    IN BOOLEAN WriteToDevice
    );

NTSTATUS
IoReadPartitionTable(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN BOOLEAN ReturnRecognizedPartitions,
    OUT struct _DRIVE_LAYOUT_INFORMATION **PartitionBuffer
    );

NTSTATUS
IoSetPartitionInformation(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN ULONG PartitionNumber,
    IN ULONG PartitionType
    );

NTSTATUS
IoWritePartitionTable(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SectorSize,
    IN ULONG SectorsPerTrack,
    IN ULONG NumberOfHeads,
    IN struct _DRIVE_LAYOUT_INFORMATION *PartitionBuffer
    );

//
// Performance counter function.
//

LARGE_INTEGER
KeQueryPerformanceCounter (
   IN PLARGE_INTEGER PerformanceFrequency OPTIONAL
   );

//
// Stall processor execution function.
//

VOID
KeStallExecutionProcessor (
    IN ULONG MicroSeconds
    );

//
// Determine if there is a complete device failure on an error.
//

BOOLEAN
FsRtlIsTotalDeviceFailure(
    IN NTSTATUS Status
    );

//
// Object Manager types
//

typedef struct _OBJECT_HANDLE_INFORMATION {
    ULONG HandleAttributes;
    ACCESS_MASK GrantedAccess;
} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION;

NTSTATUS                                                        
ObReferenceObjectByHandle(                                      
    IN HANDLE Handle,                                           
    IN ACCESS_MASK DesiredAccess,                               
    IN POBJECT_TYPE ObjectType OPTIONAL,                        
    IN KPROCESSOR_MODE AccessMode,                              
    OUT PVOID *Object,                                          
    OUT POBJECT_HANDLE_INFORMATION HandleInformation OPTIONAL   
    );                                                          
NTSTATUS                                                        
ObReferenceObjectByPointer(                                     
    IN PVOID Object,                                            
    IN ACCESS_MASK DesiredAccess,                               
    IN POBJECT_TYPE ObjectType,                                 
    IN KPROCESSOR_MODE AccessMode                               
    );                                                          
VOID                                                            
ObDereferenceObject(                                            
    IN PVOID Object                                             
    );                                                          

//
// Define exported ZwXxx routines to device drivers.
//

NTSTATUS
ZwClose(
    IN HANDLE Handle
    );

NTSTATUS
ZwCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSTATUS
ZwMakeTemporaryObject(
    IN HANDLE Handle
    );

NTSTATUS
ZwOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSTATUS
ZwMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN ULONG CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PULONG ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType,
    IN ULONG Protect
    );

NTSTATUS
ZwUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
    );

NTSTATUS
ZwCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSTATUS
ZwOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSTATUS
ZwEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS
ZwEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS
ZwFlushKey(
    IN HANDLE KeyHandle
    );

NTSTATUS
NTAPI
ZwQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS
ZwQueryValueKey(
    IN HANDLE KeyHandle,
    OUT PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSTATUS
ZwSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );


#endif // _NTDDK_
