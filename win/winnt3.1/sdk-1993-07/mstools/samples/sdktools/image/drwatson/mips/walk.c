/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    walk.c

Abstract:

    This file provides support for stack walking.

Author:

    Wesley Witt (wesw) 1-May-1993

Environment:

    User Mode

--*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "drwatson.h"
#include "proto.h"


#ifdef MIPSEB
typedef union _MIPS_INSTRUCTION {
    ULONG Long;
    UCHAR Byte[4];

    struct {
        ULONG Opcode : 6;
        ULONG Target : 26;
    } j_format;

    struct {
        ULONG Opcode : 6;
        ULONG Rs : 5;
        ULONG Rt : 5;
        LONG Simmediate : 16;
    } i_format;

    struct {
        ULONG Opcode : 6;
        ULONG Rs : 5;
        ULONG Rt : 5;
        ULONG Uimmediate : 16;
    } u_format;

    struct {
        ULONG Opcode : 6;
        ULONG Rs : 5;
        ULONG Rt : 5;
        ULONG Rd : 5;
        ULONG Re : 5;
        ULONG Function : 6;
    } r_format;

    struct {
        ULONG Opcode : 6;
        ULONG Fill1 : 1;
        ULONG Format : 4;
        ULONG Rt : 5;
        ULONG Rd : 5;
        ULONG Re : 5;
        ULONG Function : 6;
    } f_format;

    struct {
        ULONG Opcode : 6;
        ULONG Fill1 : 1;
        ULONG Format : 4;
        ULONG Ft : 5;
        ULONG Fs : 5;
        ULONG Fd : 5;
        ULONG Function : 6;
    } c_format;

} MIPS_INSTRUCTION, *PMIPS_INSTRUCTION;

#else

typedef union _MIPS_INSTRUCTION {
    ULONG Long;
    UCHAR Byte[4];

    struct {
        ULONG Target : 26;
        ULONG Opcode : 6;
    } j_format;

    struct {
        LONG Simmediate : 16;
        ULONG Rt : 5;
        ULONG Rs : 5;
        ULONG Opcode : 6;
    } i_format;

    struct {
        ULONG Uimmediate : 16;
        ULONG Rt : 5;
        ULONG Rs : 5;
        ULONG Opcode : 6;
    } u_format;

    struct {
        ULONG Function : 6;
        ULONG Re : 5;
        ULONG Rd : 5;
        ULONG Rt : 5;
        ULONG Rs : 5;
        ULONG Opcode : 6;
    } r_format;

    struct {
        ULONG Function : 6;
        ULONG Re : 5;
        ULONG Rd : 5;
        ULONG Rt : 5;
        ULONG Format : 4;
        ULONG Fill1 : 1;
        ULONG Opcode : 6;
    } f_format;

    struct {
        ULONG Function : 6;
        ULONG Fd : 5;
        ULONG Fs : 5;
        ULONG Ft : 5;
        ULONG Format : 4;
        ULONG Fill1 : 1;
        ULONG Opcode : 6;
    } c_format;

} MIPS_INSTRUCTION, *PMIPS_INSTRUCTION;

#endif   // MIPSEB

/* Define MIPS nonvolatile register test macros. */
#define IS_FLOATING_SAVED(Register) ((SAVED_FLOATING_MASK >> Register) & 1L) /*
                                                                              */
#define IS_INTEGER_SAVED(Register) ((SAVED_INTEGER_MASK >> Register) & 1L)

//
  /* Define MIPS instruction opcode values. */
  //
#define ORI_OP 0xd              /* or unsigned immediate integer */
#define ADDIU_OP 0x9            /* add immediate unsigned integer register */
#define ADDU_OP 0x21            /* add unsigned integer register */
#define JUMP_RA 0x3e00008       /* jump indirect return address register */
#define LUI_OP 0xf              /* load upper immediate integer register */
#define SD_OP 0x2f              /* store double integer register */
#define SW_OP 0x2b              /* store word integer register */
#define SDC1_OP 0x3d            /* store double floating register */
#define SWC1_OP 0x39            /* store word floating register */
#define SPEC_OP 0x0             /* special opcode - use function field */
#define SUBU_OP 0x23            /* subtract unsigned integer register */

  /* Define stack register and zero register numbers. */

#define RA 0x1f                 /* integer register 31 */
#define SP 0x1d                 /* integer register 29 */
#define ZERO 0x0                /* integer register 0 */

  /* Define saved register masks. */

#define SAVED_FLOATING_MASK 0xfff00000 /* saved floating registers */
#define SAVED_INTEGER_MASK 0xf3ffff02 /* saved integer registers */



DWORD
VirtualUnwind (
    PDEBUGPACKET       dp,
    DWORD              ControlPc,
    PRUNTIME_FUNCTION  FunctionEntry,
    PCONTEXT           ContextRecord
    )

/*++

Routine Description:

    This function virtually unwinds the specfified function by executing its
    prologue code backwards.

    If the function is a leaf function, then the address where control left
    the previous frame is obtained from the context record. If the function
    is a nested function, but not an exception or interrupt frame, then the
    prologue code is executed backwards and the address where control left
    the previous frame is obtained from the updated context record.

    Otherwise, an exception or interrupt entry to the system is being unwound
    and a specially coded prologue restores the return address twice. Once
    from the fault instruction address and once from the saved return address
    register. The first restore is returned as the function value and the
    second restore is place in the updated context record.

    If a context pointers record is specified, then the address where each
    nonvolatile registers is restored from is recorded in the appropriate
    element of the context pointers record.

Arguments:

    ControlPc - Supplies the address where control left the specified
        function.

    FunctionEntry - Supplies the address of the function table entry for the
        specified function.

    ContextRecord - Supplies the address of a context record.

    ContextPointers - Supplies an optional pointer to a context pointers
        record.

Return Value:

    The address where control left the previous frame is returned as the
    function value.

--*/

{
    DWORD Address;
    DWORD DecrementOffset;
    DWORD DecrementRegister;
    LPDWORD FloatingRegister;
    MIPS_INSTRUCTION Instruction;
    LPDWORD IntegerRegister;
    DWORD NextPc;
    LONG Offset;
    DWORD Opcode;
    DWORD Rd;
    BOOL  Restored;
    DWORD Rs;
    DWORD Rt;
    DWORD instrProlog;
    DWORD addrPC;
    DWORD addrT;


    addrPC = ControlPc;

    if (!ReadProcessMemory( dp->hProcess,
                            (LPVOID)ControlPc,
                            (LPVOID)&instrProlog,
                            4,
                            NULL
                           )) {
        return 0;
    }

    if (instrProlog == JUMP_RA) {
        ControlPc += 4;
        addrPC += 4;
        if (!ReadProcessMemory( dp->hProcess,
                                (LPVOID)addrPC,
                                (LPVOID)&Instruction.Long,
                                4,
                                NULL
                               )) {
            return 0;
        }
        addrPC -= 4;
        ControlPc -= 4;
        Opcode = Instruction.i_format.Opcode;
        if (((Opcode != ADDIU_OP) &&
             ((Opcode != SPEC_OP) ||
              (Instruction.r_format.Function != ADDU_OP))) ||
            ((Opcode == ADDIU_OP) &&
             (Instruction.i_format.Rt != SP)) ||
            ((Opcode == SPEC_OP) &&
             (Instruction.r_format.Function == ADDU_OP) &&
             (Instruction.r_format.Rd != SP))) {
            return ContextRecord->IntRa;
        }
    }

    if (ControlPc > FunctionEntry->PrologEndAddress) {
        addrPC = FunctionEntry->PrologEndAddress;
        ControlPc = FunctionEntry->PrologEndAddress;
    }

    FloatingRegister = &ContextRecord->FltF0;
    IntegerRegister = &ContextRecord->IntZero;

    DecrementRegister = 0;
    NextPc = ContextRecord->IntRa;
    Restored = FALSE;
    while (ControlPc > FunctionEntry->BeginAddress) {
        ControlPc -= 4;
        addrPC -= 4;
        if (!ReadProcessMemory( dp->hProcess,
                                (LPVOID)addrPC,
                                (LPVOID)&Instruction.Long,
                                4,
                                NULL
                               )) {
            return 0;
        }
        Opcode = Instruction.i_format.Opcode;
        Offset = Instruction.i_format.Simmediate;
        Rd = Instruction.r_format.Rd;
        Rs = Instruction.i_format.Rs;
        Rt = Instruction.i_format.Rt;
        Address = Offset + IntegerRegister[Rs];
        addrT = Offset + IntegerRegister[Rs];
        if (Opcode == SW_OP) {

            if ((Rs == SP) && (IS_INTEGER_SAVED(Rt))) {
                if (!ReadProcessMemory( dp->hProcess,
                                        (LPVOID)addrT,
                                        (LPVOID)&IntegerRegister[Rt],
                                        4,
                                        NULL
                                       )) {
                    return 0;
                }
                if ((Rt == RA) && (Restored == FALSE)) {
                    NextPc = ContextRecord->IntRa;
                    Restored = TRUE;
                }

            }

        } else if (Opcode == SWC1_OP) {

            if ((Rs == SP) && (IS_FLOATING_SAVED(Rt))) {
                if (!ReadProcessMemory( dp->hProcess,
                                        (LPVOID)addrT,
                                        (LPVOID)&FloatingRegister[Rt],
                                        4,
                                        NULL
                                       )) {
                    return 0;
                }
            }

        } else if (Opcode == SDC1_OP) {

            if ((Rs == SP) && (IS_FLOATING_SAVED(Rt))) {
                if (!ReadProcessMemory( dp->hProcess,
                                        (LPVOID)addrT,
                                        (LPVOID)&(FloatingRegister[Rt]),
                                        4,
                                        NULL
                                       )) {
                    return 0;
                }
                Address += 4;
                addrT += 4;
                if (!ReadProcessMemory( dp->hProcess,
                                        (LPVOID)addrT,
                                        (LPVOID)&(FloatingRegister[Rt+1]),
                                        4,
                                        NULL
                                       )) {
                    return 0;
                }
                Address -= 4;
                addrT -= 4;
            }

        } else if (Opcode == ADDIU_OP) {

            if ((Rs == SP) && (Rt == SP)) {
                IntegerRegister[SP] -= Offset;

            } else if ((Rt == DecrementRegister) && (Rs == ZERO)) {
                IntegerRegister[SP] += Offset;
            }

        } else if (Opcode == ORI_OP) {

            if ((Rs == DecrementRegister) && (Rt == DecrementRegister)) {
                DecrementOffset = (Offset & 0xffff);

            } else if ((Rt == DecrementRegister) && (Rs == ZERO)) {
                IntegerRegister[SP] += (Offset & 0xffff);
            }

        } else if (Opcode == SPEC_OP) {

            Opcode = Instruction.r_format.Function;
            if (Opcode == ADDU_OP) {

                if (IS_INTEGER_SAVED(Rd)) {
                    if ((IS_INTEGER_SAVED(Rs)) && (Rt == ZERO)) {
                        IntegerRegister[Rs] = IntegerRegister[Rd];
                        if ((Rs == RA) && (Restored == FALSE)) {
                            NextPc = ContextRecord->IntRa;
                            Restored = TRUE;
                        }

                    } else if ((Rs == ZERO) && (IS_INTEGER_SAVED(Rt))) {
                        IntegerRegister[Rt] = IntegerRegister[Rd];
                        if ((Rt == RA) && (Restored == FALSE)) {
                            NextPc = ContextRecord->IntRa;
                            Restored = TRUE;
                        }
                    }
                }

            } else if (Opcode == SUBU_OP) {

                if ((Rd == SP) && (Rs == SP)) {
                    DecrementRegister = Rt;
                }
            }

        } else if (Opcode == LUI_OP) {

            if (Rt == DecrementRegister) {
                IntegerRegister[SP] += (DecrementOffset + (Offset << 16));
                DecrementRegister = 0;
            }
        }
    }
    return NextPc;
}

BOOL
StackWalkInit( PSTACKWALK pstk, PDEBUGPACKET dp )
{
    pstk->pc        = dp->tctx->context.Fir;
    pstk->frame     = dp->tctx->context.IntSp;
    pstk->params[0] = dp->tctx->context.IntA0;
    pstk->params[1] = dp->tctx->context.IntA1;
    pstk->params[2] = dp->tctx->context.IntA2;
    pstk->params[3] = dp->tctx->context.IntA3;

    return TRUE;
}

BOOL
StackWalkNext( PSTACKWALK pstk, PDEBUGPACKET dp )
{
    DWORD               fir;
    LONG                begin=0, end, test;
    CONTEXT             context;
    DWORD               dwRa;
    PMODULEINFO         mi;


    fir = pstk->pc;

    mi = GetModuleForPC( dp, fir );
    if (mi == NULL) {
        return FALSE;
    }

    /*
     * Do a binary search to determine which function contains this FIR
     */

    for(end=mi->dwEntries,test=(begin+end)/2; begin<=end; test=(begin+end)/2) {
        if (fir<mi->pExceptionData[test].BeginAddress) {
            end = test-1;
        }
        else
        if (fir>=mi->pExceptionData[test].EndAddress) {
            begin = test+1;
        }
        else {
            break;
        }
    }

    /*
     *  No function was found to include this FIR.  Therefore this
     *  is a leaf function
     */

    if (begin>end) {
        dp->tctx->stackBase = dp->tctx->context.IntSp;
        dp->tctx->stackRA   = dp->tctx->context.IntRa;

        pstk->frame = dp->tctx->context.IntSp;
        pstk->pc = dp->tctx->context.IntRa;

        if (!ReadProcessMemory( dp->hProcess,
                                (LPVOID)pstk->frame,
                                (LPVOID)pstk->params,
                                12,
                                NULL
                               )) {
            pstk->params[0] =
            pstk->params[1] =
            pstk->params[2] = 0;
        }

        return TRUE;
    }

    // Virtually unwind the stack to determine base and caller

    context = dp->tctx->context;
    context.IntSp = pstk->frame;

    dwRa = VirtualUnwind(dp, fir, &mi->pExceptionData[test], &context);

    if (((dp->tctx->stackBase == context.IntSp) && (dwRa == dp->tctx->stackRA)) ||
        (dwRa == 1)) {
        return FALSE;
    }

    dp->tctx->stackBase = context.IntSp;
    dp->tctx->stackRA   = dwRa;

    pstk->frame = context.IntSp;
    pstk->pc = dwRa;

    if (!ReadProcessMemory( dp->hProcess,
                            (LPVOID)pstk->frame,
                            (LPVOID)pstk->params,
                            12,
                            NULL
                           )) {
        pstk->params[0] =
        pstk->params[1] =
        pstk->params[2] = 0;
    }

    return TRUE;
}
