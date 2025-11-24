

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    ULONG Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

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

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD *Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

typedef EXCEPTION_REGISTRATION_RECORD *PEXCEPTION_REGISTRATION_RECORD;

// begin_nthal
//
// Trap frame
//
//  NOTE - We deal only with 32bit registers, so the assembler equivalents
//         are always the extended forms.
//
//  NOTE - Unless you want to run like slow molasses everywhere in the
//         the system, this structure must be of DWORD length, DWORD
//         aligned, and its elements must all be DWORD aligned.
//
//  NOTE WELL   -
//
//      The i386 does not build stack frames in a consistent format, the
//      frames vary depending on whether or not a privledge transition
//      was involved.
//
//      In order to make NtContinue work for both user mode and kernel
//      mode callers, we must force a cononical stack.
//
//      If we're called from kernel mode, this structure is 8 bytes longer
//      than the actual frame!
//
//  WARNING:
//
//      KTRAP_FRAME_LENGTH needs to be 16byte integral (at present.)
//

typedef struct _KTRAP_FRAME {


//
//  Following 4 values are only used and defined for DBG systems,
//  but are always allocated to make switching from DBG to non-DBG
//  and back quicker.  They are not DEVL because they have a non-0
//  performance impact.
//

    ULONG   DbgEbp;         // Copy of User EBP set up so KB will work.
    ULONG   DbgEip;         // EIP of caller to system call, again, for KB.
    ULONG   DbgArgMark;     // Marker to show no args here.
    ULONG   DbgArgPointer;  // Pointer to the actual args

//
//  Temporary values used when frames are edited.
//
//
//  NOTE:   Any code that want's ESP must materialize it, since it
//          is not stored in the frame for kernel mode callers.
//
//          And code that sets ESP in a KERNEL mode frame, must put
//          the new value in TempEsp, make sure that TempSegCs holds
//          the real SegCs value, and put a special marker value into SegCs.
//

    ULONG   TempSegCs;
    ULONG   TempEsp;

//
//  Debug registers.
//

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

//
//  Segment registers
//

    ULONG   SegGs;
    ULONG   SegEs;
    ULONG   SegDs;

//
//  Volatile registers
//

    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

//
//  Nesting state, not part of context record
//

    ULONG   PreviousPreviousMode;

    PEXCEPTION_REGISTRATION_RECORD ExceptionList;
                                            // Trash if caller was user mode.
                                            // Saved exception list if caller
                                            // was kernel mode or we're in
                                            // an interrupt.

//
//  FS is TIB/PCR pointer, is here to make save sequence easy
//

    ULONG   SegFs;

//
//  Non-volatile registers
//

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Ebp;

//
//  Control registers
//

    ULONG   ErrCode;
    ULONG   Eip;
    ULONG   SegCs;
    ULONG   EFlags;

    ULONG   HardwareEsp;    // WARNING - segSS:esp are only here for stacks
    ULONG   HardwareSegSs;  // that involve a ring transition.

    ULONG   V86Es;          // these will be present for all transitions from
    ULONG   V86Ds;          // V86 mode
    ULONG   V86Fs;
    ULONG   V86Gs;
} KTRAP_FRAME;


typedef KTRAP_FRAME *PKTRAP_FRAME;
typedef KTRAP_FRAME *PKEXCEPTION_FRAME;

#define KTRAP_FRAME_LENGTH  (sizeof(KTRAP_FRAME))
#define KTRAP_FRAME_ALIGN   (sizeof(ULONG))
#define KTRAP_FRAME_ROUND   (KTRAP_FRAME_ALIGN-1)

#define EFLAGS_V86_MASK     0x00020000L
#define GRAN_PAGE           1
#define PAGE_SHIFT          12L
#define MODE_MASK           1
#define KGDT_TSS            40
#define KGDT_LDT            72
#define FRAME_EDITED        0xfff8

typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

typedef int BOOL;

typedef struct _DESCRIPTOR {
    USHORT  Pad;
    USHORT  Limit;
    ULONG   Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

typedef struct _KSPECIAL_REGISTERS {
    ULONG Cr0;
    ULONG Cr2;
    ULONG Cr3;
    ULONG Cr4;
    ULONG KernelDr0;
    ULONG KernelDr1;
    ULONG KernelDr2;
    ULONG KernelDr3;
    ULONG KernelDr6;
    ULONG KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    USHORT Tr;
    USHORT Ldtr;
    ULONG Reserved[6];
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

//
// Processor State frame: Before a processor freezes itself, it
// dumps the processor state to the processor state frame for
// debugger to examine.
//

typedef struct _KPROCESSOR_STATE {
    struct _CONTEXT ContextFrame;
    struct _KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;
