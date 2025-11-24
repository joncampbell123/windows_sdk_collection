typedef double DOUBLE;

//
// Exception frame  --  NON-VOLATILE state
//
// This structure's layout matches that of the registers as saved
// in a call/return stack frame, where the called program has saved
// all the non-volatile registers.
//
// N.B. This frame must be a multiple of 8 bytes in length, as the
//      Stack Frame header, Trap Frame and the Exception
//      Frame together must make up a valid call stack frame.
//

typedef struct _KEXCEPTION_FRAME {

    ULONG  Fill1;                       // padding

    ULONG  Gpr13;
    ULONG  Gpr14;
    ULONG  Gpr15;
    ULONG  Gpr16;
    ULONG  Gpr17;
    ULONG  Gpr18;
    ULONG  Gpr19;
    ULONG  Gpr20;
    ULONG  Gpr21;
    ULONG  Gpr22;
    ULONG  Gpr23;
    ULONG  Gpr24;
    ULONG  Gpr25;
    ULONG  Gpr26;
    ULONG  Gpr27;
    ULONG  Gpr28;
    ULONG  Gpr29;
    ULONG  Gpr30;
    ULONG  Gpr31;

    DOUBLE Fpr14;               // 8-byte boundary required here
    DOUBLE Fpr15;
    DOUBLE Fpr16;
    DOUBLE Fpr17;
    DOUBLE Fpr18;
    DOUBLE Fpr19;
    DOUBLE Fpr20;
    DOUBLE Fpr21;
    DOUBLE Fpr22;
    DOUBLE Fpr23;
    DOUBLE Fpr24;
    DOUBLE Fpr25;
    DOUBLE Fpr26;
    DOUBLE Fpr27;
    DOUBLE Fpr28;
    DOUBLE Fpr29;
    DOUBLE Fpr30;
    DOUBLE Fpr31;

} KEXCEPTION_FRAME, *PKEXCEPTION_FRAME;


//
// Special version of exception frame for use by SwapContext and
// KiInitializeContextThread.
//

typedef struct _KSWAP_FRAME {
    KEXCEPTION_FRAME ExceptionFrame;
    ULONG ConditionRegister;
    ULONG SwapReturn;
} KSWAP_FRAME, *PKSWAP_FRAME;


//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
    DOUBLE *FloatingContext[32];
    PULONG FpscrContext;
    PULONG IntegerContext[32];
    PULONG CrContext;
    PULONG XerContext;
    PULONG MsrContext;
    PULONG IarContext;
    PULONG LrContext;
    PULONG CtrContext;
} KNONVOLATILE_CONTEXT_POINTERS, *PKNONVOLATILE_CONTEXT_POINTERS;


#define ARGUMENT_PRESENT(ArgumentPointer)    (\
    (CHAR *)(ArgumentPointer) != (CHAR *)(NULL) )
