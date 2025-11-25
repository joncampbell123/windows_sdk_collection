/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    walk.c

Abstract:

    This file provides support for stack walking.

Author:

    Miche Baker-Harvey (v-michbh) 1-May-1993  (ported from ntsd)

Environment:

    User Mode

--*/

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "drwatson.h"
#include "proto.h"

#include <alphaops.h>
#include "disasm.h"

DWORD VirtualUnwind (PDEBUGPACKET, DWORD, PRUNTIME_FUNCTION, PCONTEXT);

ALPHA_INSTRUCTION disinstr;

#define DPRINT(a,b)

BOOL
StackWalkInit( PSTACKWALK pstk,
                  PDEBUGPACKET dp)
{
    pstk->pc        = dp->tctx->context.Fir;
    pstk->frame     = dp->tctx->context.IntSp;
    pstk->params[0] = dp->tctx->context.IntA0;
    pstk->params[1] = dp->tctx->context.IntA1;
    pstk->params[2] = dp->tctx->context.IntA2;
    pstk->params[3] = dp->tctx->context.IntA3;

    return TRUE;
}                                      /* ProcessStackWalkInitCmd() */

BOOL
StackWalkNext( PSTACKWALK pstk,
                         PDEBUGPACKET dp)
{
    DWORD               fir;
    LONG                begin=0, end, test;
    PRUNTIME_FUNCTION   rf;
    CONTEXT             context;
    DWORD               dwRa;
    DWORD               cb;
    PMODULEINFO         mi;

    fir = pstk->pc;

    mi = GetModuleForPC( dp, fir );
    if (mi == NULL) {
         return FALSE;
    }

    //
    // Do a binary search to determine which function contains this FIR
    //


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

    //
    // No function was found to include this FIR
    // Therefore this is a leaf function
    //

    if (begin>end) {
        dp->tctx->stackBase = dp->tctx->context.IntSp;
        dp->tctx->stackRA   = dp->tctx->context.IntRa;

        pstk->frame = dp->tctx->context.IntSp;
        pstk->pc    = dp->tctx->context.IntRa;
//
// MBH - bugbug
// This is hopeless: the arguments aren't normally homed on ALPHA
//
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


    //
    // Virtually unwind the stack to determine base and caller
    //

    context = dp->tctx->context;
    context.IntSp = pstk->frame;

    dwRa = VirtualUnwind(dp, fir, &mi->pExceptionData[test], &context);

    //
    // The Ra value coming out of mainCRTStartup is set by some RTL
    // routines to be "1"; return out of mainCRTStartup is actually
    // done through Jump/Unwind, so this serves to cause an error if
    // someone actually does a return.  That's why we check here for
    // dwRa == 1 - this happens when in the frame for CRTStartup.
    //
    // We test for (1-4) because on ALPHA, the value returned by
    // VirtualUnwind is the value to be passed to the next call to
    // VirtualUnwind, which is NOT the same as the Ra - it's sometimes
    // decremented by four - this gives the faulting instruction -
    // in particular, we want the fault instruction so we can get the
    // correct scope in the case of an exception.
    //

    if (((dp->tctx->stackBase == context.IntSp) && (dwRa == dp->tctx->stackRA)) ||
        (dwRa == (1) ) ||
        (dwRa == (1-4) ) ||
        (dwRa == 0) ) {
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

//
// MBH - beginning of new VirtualUnwind code



#ifdef DBG

ULONG RtlDebugFlags = 0;
#define RTL_DBG_VIRTUAL_UNWIND 1
#define RTL_DBG_VIRTUAL_UNWIND_DETAIL 2

//
// Define an array of symbolic names for the integer registers.
//

PCHAR RtlpIntegerRegisterNames[32] = {
    "v0",  "t0",  "t1",  "t2",  "t3",  "t4",  "t5",  "t6",      // 0 - 7
    "t7",  "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "fp",      // 8 - 15
    "a0",  "a1",  "a2",  "a3",  "a4",  "a5",  "t8",  "t9",      // 16 - 23
    "t10", "t11", "ra",  "t12", "at",  "gp",  "sp",  "zero",    // 24 - 31
};

//
// This function disassembles the instruction at the given address. It is
// only used for debugging and recognizes only those few instructions that
// are relevant during reverse execution of the prologue by virtual unwind.
//

VOID
_RtlpDebugDisassemble (
    IN ULONG ControlPc,
    IN PCONTEXT ContextRecord
    )
{
    UCHAR Comments[50];
    PULONG FloatingRegister;
//    PUQUAD FloatingRegister;
    ULONG Function;
    ULONG Hint;
    ULONG Literal8;
    ALPHA_INSTRUCTION Instruction;
//    PUQUAD IntegerRegister;
    PULONG IntegerRegister;
    LONG Offset16;
    UCHAR Operands[20];
    ULONG Opcode;
    PCHAR OpName;
    ULONG Ra;
    ULONG Rb;
    ULONG Rc;
    PCHAR RaName;
    PCHAR RbName;
    PCHAR RcName;

    if (RtlDebugFlags & RTL_DBG_VIRTUAL_UNWIND_DETAIL) {
        Instruction.Long = *((PULONG)ControlPc);
        Hint = Instruction.Jump.Hint;
        Literal8 = Instruction.OpLit.Literal;
        Offset16 = Instruction.Memory.MemDisp;
        Opcode = Instruction.Memory.Opcode;
        Ra = Instruction.OpReg.Ra;
        RaName = RtlpIntegerRegisterNames[Ra];
        Rb = Instruction.OpReg.Rb;
        RbName = RtlpIntegerRegisterNames[Rb];
        Rc = Instruction.OpReg.Rc;
        RcName = RtlpIntegerRegisterNames[Rc];

        IntegerRegister = &ContextRecord->IntV0;
        FloatingRegister = &ContextRecord->FltF0;

        OpName = NULL;
        switch (Opcode) {
        case JMP_OP :
            if (Instruction.Jump.Function == RET_FUNC) {
                OpName = "ret";
                sprintf(Operands, "%s, (%s), %04lx", RaName, RbName, Hint);
                sprintf(Comments, "%s = %Lx", RbName, IntegerRegister[Rb]);
            }
            break;

        case LDAH_OP :
        case LDA_OP :
        case STQ_OP :
            if (Opcode == LDA_OP) {
                OpName = "lda";

            } else if (Opcode == LDAH_OP) {
                OpName = "ldah";

            } else if (Opcode == STQ_OP) {
                OpName = "stq";
            }
            sprintf(Operands, "%s, $%d(%s)", RaName, Offset16, RbName);
            sprintf(Comments, "%s = %Lx", RaName, IntegerRegister[Ra]);
            break;

        case ARITH_OP :
        case BIT_OP :
            Function = Instruction.OpReg.Function;
            if ((Opcode == ARITH_OP) && (Function == ADDQ_FUNC)) {
                    OpName = "addq";

            } else if ((Opcode == ARITH_OP) && (Function == SUBQ_FUNC)) {
                    OpName = "subq";

            } else if ((Opcode == BIT_OP) && (Function == BIS_FUNC)) {
                    OpName = "bis";

            } else {
                break;
            }
            if (Instruction.OpReg.RbvType == RBV_REGISTER_FORMAT) {
                sprintf(Operands, "%s, %s, %s", RaName, RbName, RcName);

            } else {
                sprintf(Operands, "%s, $%d, %s", RaName, Literal8, RcName);
            }
            sprintf(Comments, "%s = %Lx", RcName, IntegerRegister[Rc]);
            break;

        case FPOP_OP :
            if (Instruction.FpOp.Function == CPYS_FUNC) {
                OpName = "cpys";
                sprintf(Operands, "f%d, f%d, f%d", Ra, Rb, Rc);
                sprintf(Comments, "f%d = %Lx", Rc, FloatingRegister[Rc]);
            }
            break;

        case STT_OP :
            OpName = "stt";
            sprintf(Operands, "f%d, $%d(%s)", Ra, Offset16, RbName);
            sprintf(Comments, "f%d = %Lx", Ra, FloatingRegister[Ra]);
            break;
        }
        if (OpName == NULL) {
            OpName = "???";
            sprintf(Operands, "...");
            sprintf(Comments, "Unknown to virtual unwind.");
        }
        DbgPrint("    %08lx: %08lx  %-5s %-16s // %s\n",
                 ControlPc, Instruction.Long, OpName, Operands, Comments);
    }
    return;
}

#define _RtlpFoundTrapFrame(NextPc) \
    if (RtlDebugFlags & RTL_DBG_VIRTUAL_UNWIND) { \
        DbgPrint("    *** Looks like a trap frame (fake prologue), Fir = %lx\n", \
                 NextPc); \
    }


#else

#define _RtlpDebugDisassemble(ControlPc, ContextRecord)
#define _RtlpFoundTrapFrame(NextPc)

#endif


/*++

Routine Description:

    Read longword at addr into value.

Arguments:

    addr  - address at which to read
    value - where to put the result


--*/

BOOLEAN
LocalDoMemoryRead(PDEBUGPACKET dp, ULONG addr, PULONG pvalue)
{
    ULONG cb;

    if (!ReadProcessMemory(dp->hProcess,
                           (LPVOID)addr,
                           (LPVOID)pvalue,
                           sizeof(*pvalue),
                           &cb)) {
         DPRINT(1, ("LocalDoMemoryRead: Can't read at %08x\n", addr));
         return(FALSE);
    }
    if (cb != sizeof(*pvalue)) {
         DPRINT(1, ("LocalDoMemoryRead: Count wrong at %08x\n", addr));
         return(FALSE);
    }
    return(TRUE);
}

//
// MBH - this value is redefined in windbg common code to be IntSp.
//
#define SP_REG 30


DWORD
VirtualUnwind (
    PDEBUGPACKET dp,
    DWORD ControlPc,
    PRUNTIME_FUNCTION FunctionEntry,
    PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This function virtually unwinds the specified function by executing its
    prologue code backwards. Given the current context and the instructions
    that preserve registers in the prologue, it is possible to recreate the
    nonvolatile context at the point the function was called.

    If the function is a leaf function, then the address where control left
    the previous frame is obtained from the context record. If the function
    is a nested function, but not an exception or interrupt frame, then the
    prologue code is executed backwards and the address where control left
    the previous frame is obtained from the updated context record.

    Otherwise, an exception or interrupt entry to the system is being unwound
    and a specially coded prologue restores the return address twice. Once
    from the fault instruction address and once from the saved return address
    register. The first restore is returned as the function value and the
    second restore is placed in the updated context record.

    During the unwind, the virtual and real frame pointers for the function
    are calculated and returned in the given frame pointers structure.

    If a context pointers record is specified, then the address where each
    register is restored from is recorded in the appropriate element of the
    context pointers record.

Arguments:

    ControlPc - Supplies the address where control left the specified
        function.

    FunctionEntry - Supplies the address of the function table entry for the
        specified function.

    ContextRecord - Supplies the address of a context record.


Return Value:

    The address where control left the previous frame is returned as the
    function value.

Implementation Notes:

    N.B. "where control left" is not the "return address" of the call in the
    previous frame. For normal frames, NextPc points to the last instruction
    that completed in the previous frame (the JSR/BSR). The difference between
    NextPc and NextPc + 4 (return address) is important for correct behavior
    in boundary cases of exception addresses and scope tables.

    For exception and interrupt frames, NextPc is obtained from the trap frame
    contination address (Fir). For faults and synchronous traps, NextPc is both
    the last instruction to execute in the previous frame and the next
    instruction to execute if the function were to return. For asynchronous
    traps, NextPc is the continuation address. It is the responsibility of the
    compiler to insert TRAPB instructions to insure asynchronous traps do not
    occur outside the scope from the instruction(s) that caused them.

    N.B. in this and other files where RtlVirtualUnwind is used, the variable
    named NextPc is perhaps more accurately, LastPc - the last PC value in
    the previous frame, or CallPc - the address of the call instruction, or
    ControlPc - the address where control left the previous frame. Instead
    think of NextPc as the next PC to use in another call to virtual unwind.

    The Alpha version of virtual unwind is similar in design, but slightly
    more complex than the Mips version. This is because Alpha compilers
    are given more flexibility to optimize generated code and instruction
    sequences, including within procedure prologues. And also because of
    compiler design issues, the function must manage both virtual and real
    frame pointers.

Version Information:  This version was taken from exdspatch.c@v37 (Feb 1993)

--*/

{

    ULONG Address;
    ULONG DecrementOffset;
    ULONG DecrementRegister;
    ALPHA_INSTRUCTION FollowingInstruction;
//    PUQUAD FloatingRegister;
    PULONG FloatingRegister;
    ULONG FrameSize;
    ULONG Function;
    ALPHA_INSTRUCTION Instruction;
//    PUQUAD IntegerRegister;
    PULONG IntegerRegister;
    ULONG Literal8;
    ULONG NextPc;
    LONG Offset16;
    ULONG Opcode;
    ULONG Ra;
    ULONG Rb;
    ULONG Rc;
    BOOLEAN RestoredRa;
    BOOLEAN RestoredSp;

#ifdef DBG
    if (RtlDebugFlags & RTL_DBG_VIRTUAL_UNWIND) {
        DbgPrint("\nRtlVirtualUnwind(ControlPc = %lx, FunctionEntry = %lx,) sp = %lx\n",
                 ControlPc, FunctionEntry, ContextRecord->IntSp);
    }
#endif
#ifdef DBG
    if ((FunctionEntry == NULL) || (ControlPc & 0x3) ||
        (FunctionEntry->BeginAddress >= FunctionEntry->EndAddress) ||
        (FunctionEntry->PrologEndAddress < FunctionEntry->BeginAddress) ||
        (FunctionEntry->PrologEndAddress >= FunctionEntry->EndAddress)) {
        DbgPrint("\n****** Warning - invalid PC or function table entry (virtual unwind).\n");
        return ControlPc;
    }
#endif
    //
    // Set the base address of the integer and floating register arrays within
    // the context record. Each set of 32 registers is known to be contiguous.
    //

// MBH - assuming that quad values are together in context.
// Do we really need 64 bit values?

    IntegerRegister = &ContextRecord->IntV0;
    FloatingRegister = &ContextRecord->FltF0;

    //
    // Handle the epilogue case where the next instruction is a return.
    //
    // Exception handlers cannot be called if the ControlPc is within the
    // epilogue because exception handlers expect to operate with a current
    // stack frame. The value of SP is not current within the epilogue.
    //

//MBH
    if (! LocalDoMemoryRead(dp, ControlPc, &Instruction.Long))  {
        return(0);
    }

//    Instruction.Long = *((PULONG)ControlPc);
//MBH
    if (IS_RETURN_0001_INSTRUCTION(Instruction.Long)) {
        Rb = Instruction.Jump.Rb;
        NextPc = (ULONG)IntegerRegister[Rb] - 4;

        //
        // The instruction at the point where control left the specified
        // function is a return, so any saved registers have already been
        // restored, and the stack pointer has already been adjusted. The
        // stack does not need to be unwound in this case and the saved
        // return address register is returned as the function value.
        //
        // In fact, reverse execution of the prologue is not possible in
        // this case: the stack pointer has already been incremented and
        // so, for this frame, neither a valid stack pointer nor frame
        // pointer exists from which to begin reverse execution of the
        // prologue. In addition, the integrity of any data on the stack
        // below the stack pointer is never guaranteed (due to interrupts
        // and exceptions).
        //
        // The epilogue instruction sequence is:
        //
        // ==>  ret   zero, (Ra), 1     // return
        // or
        //
        //      mov   ra, Rx            // save return address
        //      ...
        // ==>  ret   zero, (Rx), 1     // return
        //

//        EstablisherFrame->Real = 0;
//        EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;
//        *InFunction = FALSE;
        _RtlpDebugDisassemble(ControlPc, ContextRecord);
//        _RtlpVirtualUnwindExit(NextPc, ContextRecord);
        return NextPc;
    }

    //
    // Handle the epilogue case where the next two instructions are a stack
    // frame deallocation and a return.
    //

// MBH
    if (!LocalDoMemoryRead(dp,
                           ControlPc+4,
                           &FollowingInstruction.Long))
          return (0);

//    FollowingInstruction.Long = *((PULONG)(ControlPc + 4));
// MBH
    if (IS_RETURN_0001_INSTRUCTION(FollowingInstruction.Long)) {
        Rb = FollowingInstruction.Jump.Rb;
        NextPc = (ULONG)IntegerRegister[Rb] - 4;

        //
        // The second instruction following the point where control
        // left the specified function is a return. If the instruction
        // before the return is a stack increment instruction, then all
        // saved registers have already been restored except for SP.
        // The value of the stack pointer register cannot be recovered
        // through reverse execution of the prologue because in order
        // to begin reverse execution either the stack pointer or the
        // frame pointer (if any) must still be valid.
        //
        // Instead, the effect that the stack increment instruction
        // would have had on the context is manually applied to the
        // current context. This is forward execution of the epilogue
        // rather than reverse execution of the prologue.
        //
        // In an epilogue, as in a prologue, the stack pointer is always
        // adjusted with a single instruction: either an immediate-value
        // (lda) or a register-value (addq) add instruction.
        //

        Function = Instruction.OpReg.Function;
        Offset16 = Instruction.Memory.MemDisp;
        Opcode = Instruction.OpReg.Opcode;
        Ra = Instruction.OpReg.Ra;
        Rb = Instruction.OpReg.Rb;
        Rc = Instruction.OpReg.Rc;

        if ((Opcode == LDA_OP) && (Ra == SP_REG)) {

            //
            // Load Address instruction.
            //
            // Since the destination (Ra) register is SP, an immediate-
            // value stack deallocation operation is being performed. The
            // displacement value should be added to SP. The displacement
            // value is assumed to be positive. The amount of stack
            // deallocation possible using this instruction ranges from
            // 16 to 32752 (32768 - 16) bytes. The base register (Rb) is
            // usually SP, but may be another register.
            //
            // The epilogue instruction sequence is:
            //
            // ==>  lda   sp, +N(sp)        // deallocate stack frame
            //      ret   zero, (ra)        // return
            // or
            //
            // ==>  lda   sp, +N(Rx)        // restore SP and deallocate frame
            //      ret   zero, (ra)        // return
            //

            ContextRecord->IntSp = Offset16 + IntegerRegister[Rb];
//            EstablisherFrame->Real = 0;
//            EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;
//            *InFunction = FALSE;
            _RtlpDebugDisassemble(ControlPc, ContextRecord);
            _RtlpDebugDisassemble(ControlPc + 4, ContextRecord);
//            _RtlpVirtualUnwindExit(NextPc, ContextRecord);
            return NextPc;

        } else if ((Opcode == ARITH_OP) && (Function == ADDQ_FUNC) &&
                   (Rc == SP_REG) &&
                   (Instruction.OpReg.RbvType == RBV_REGISTER_FORMAT)) {

            //
            // Add Quadword instruction.
            //
            // Since both source operands are registers, and the
            // destination register is SP, a register-value stack
            // deallocation is being performed. The value of the two
            // source registers should be added and this is the new
            // value of SP. One of the source registers is usually SP,
            // but may be another register.
            //
            // The epilogue instruction sequence is:
            //
            //      ldiq  Rx, N             // set [large] frame size
            //      ...
            // ==>  addq  sp, Rx, sp        // deallocate stack frame
            //      ret   zero, (ra)        // return
            // or
            //
            // ==>  addq  Rx, Ry, sp        // restore SP and deallocate frame
            //      ret   zero, (ra)        // return
            //

            ContextRecord->IntSp = IntegerRegister[Ra] + IntegerRegister[Rb];
//            EstablisherFrame->Real = 0;
//            EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;
//            *InFunction = FALSE;
            _RtlpDebugDisassemble(ControlPc, ContextRecord);
            _RtlpDebugDisassemble(ControlPc + 4, ContextRecord);
//            _RtlpVirtualUnwindExit(NextPc, ContextRecord);
            return NextPc;
        }
    }

    //
    // By default set the frame pointers to the current value of SP.
    //
    // When a procedure is called, the value of SP before the stack
    // allocation instruction is the virtual frame pointer. When reverse
    // executing instructions in the prologue, the value of SP before the
    // stack allocation instruction is encountered is the real frame
    // pointer. This is the current value of SP unless the procedure uses
    // a frame pointer (e.g., FP_REG).
    //

//    EstablisherFrame->Real = (ULONG)ContextRecord->IntSp;
//    EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;

    //
    // If the address where control left the specified function is beyond
    // the end of the prologue, then the control PC is considered to be
    // within the function and the control address is set to the end of
    // the prologue. Otherwise, the control PC is not considered to be
    // within the function (i.e., the prologue).
    //
    // N.B. PrologEndAddress is equal to BeginAddress for a leaf function.
    //
    // The low-order two bits of PrologEndAddress are reserved for the IEEE
    // exception mode and so must be masked out.
    //

    if ((ControlPc < FunctionEntry->BeginAddress) ||
        (ControlPc >= FunctionEntry->PrologEndAddress)) {
//        *InFunction = TRUE;
        ControlPc = (FunctionEntry->PrologEndAddress & (~0x3));

    } else {
//        *InFunction = FALSE;
    }

    //
    // Scan backward through the prologue to reload callee saved registers
    // that were stored or copied and to increment the stack pointer if it
    // was decremented.
    //

    DecrementRegister = ZERO_REG;
    NextPc = (ULONG)ContextRecord->IntRa - 4;
    RestoredRa = FALSE;
    RestoredSp = FALSE;
    while (ControlPc > FunctionEntry->BeginAddress) {

        //
        // Get instruction value, decode fields, case on opcode value, and
        // reverse register store and stack decrement operations.
        // N.B. The location of Opcode, Ra, Rb, and Rc is the same across
        // all opcode formats. The same is not true for Function.
        //

        ControlPc -= 4;
//MBH
        if (!LocalDoMemoryRead(dp, ControlPc, &Instruction.Long)) {
             return(0);
        }
//        Instruction.Long = *((PULONG)ControlPc);
//MBH
        Function = Instruction.OpReg.Function;
        Literal8 = Instruction.OpLit.Literal;
        Offset16 = Instruction.Memory.MemDisp;
        Opcode = Instruction.OpReg.Opcode;
        Ra = Instruction.OpReg.Ra;
        Rb = Instruction.OpReg.Rb;
        Rc = Instruction.OpReg.Rc;

        //
        // Compare against each instruction type that will affect the context
        // and that is allowed in a prologue. Any other instructions found
        // in the prologue will be ignored since they are assumed to have no
        // effect on the context.
        //

        switch (Opcode) {

        case STQ_OP :

            //
            // Store Quad instruction.
            //
            // If the base register is SP, then reload the source register
            // value from the value stored on the stack.
            //
            // The prologue instruction sequence is:
            //
            // ==>  stq   Rx, N(sp)         // save integer register Rx
            //

            if ((Rb == SP_REG) && (Ra != ZERO_REG)) {

                //
                // Reload the register by retrieving the value previously
                // stored on the stack.
                //

                Address = Offset16 + ContextRecord->IntSp;
// MBH
                if (!LocalDoMemoryRead(dp,
                                       Address,
                                       &IntegerRegister[Ra])) {
                    return(0);
                }
//                IntegerRegister[Ra] = *((PULONG)Address);
// MBH

                //
                // If the destination register is RA and this is the first
                // time that RA is being restored, then set the address of
                // where control left the previous frame. Otherwise, if this
                // is the second time RA is being restored, then the first
                // one was an interrupt or exception address and the return
                // PC should not have been biased by 4.
                //

                if (Ra == RA_REG) {
                    if (RestoredRa == FALSE) {
                        NextPc = (ULONG)ContextRecord->IntRa - 4;
                        RestoredRa = TRUE;

                    } else {
                        NextPc += 4;
                        _RtlpFoundTrapFrame(NextPc);
                    }

                //
                // Otherwise, if the destination register is SP and this is
                // the first time that SP is being restored, then set the
                // establisher frame pointers.
                //

                } else if ((Ra == SP_REG) && (RestoredSp == FALSE)) {
                    RestoredSp = TRUE;
                }

                //
                // If a context pointer record is specified, then record
                // the address where the destination register contents
                // are stored.
                //

                _RtlpDebugDisassemble(ControlPc, ContextRecord);
            }
            break;

        case LDAH_OP :
            Offset16 <<= 16;

        case LDA_OP :

            //
            // Load Address High, Load Address instruction.
            //
            // There are several cases where the lda and/or ldah instructions
            // are used: one to decrement the stack pointer directly, and the
            // others to load immediate values into another register and that
            // register is then used to decrement the stack pointer.
            //
            // In the examples below, as a single instructions or as a pair,
            // a lda may be substituted for a ldah and visa-versa.
            //

            if (Ra == SP_REG) {
                if (Rb == SP_REG) {

                    //
                    // If both the destination (Ra) and base (Rb) registers
                    // are SP, then a standard stack allocation was performed
                    // and the negated displacement value is the stack frame
                    // size. The amount of stack allocation possible using
                    // the lda instruction ranges from 16 to 32768 bytes and
                    // the amount of stack allocation possible using the ldah
                    // instruction ranges from 65536 to 2GB in multiples of
                    // 65536 bytes. It is rare for the ldah instruction to be
                    // used in this manner.
                    //
                    // The prologue instruction sequence is:
                    //
                    // ==>  lda   sp, -N(sp)    // allocate stack frame
                    //

                    FrameSize = -Offset16;
                    goto StackAllocation;

                } else {

                    //
                    // The destination register is SP and the base register
                    // is not SP, so this instruction must be the second
                    // half of an instruction pair to allocate a large size
                    // (>32768 bytes) stack frame. Save the displacement value
                    // as the partial decrement value and postpone adjusting
                    // the value of SP until the first instruction of the pair
                    // is encountered.
                    //
                    // The prologue instruction sequence is:
                    //
                    //      ldah  Rx, -N(sp)    // prepare new SP (upper)
                    // ==>  lda   sp, sN(Rx)    // allocate stack frame
                    //

                    DecrementRegister = Rb;
                    DecrementOffset = Offset16;
                    _RtlpDebugDisassemble(ControlPc, ContextRecord);
                }

            } else if (Ra == DecrementRegister) {
                if (Rb == DecrementRegister) {

                    //
                    // Both the destination and base registers are the
                    // decrement register, so this instruction exists as the
                    // second half of a two instruction pair to load a
                    // 31-bit immediate value into the decrement register.
                    // Save the displacement value as the partial decrement
                    // value.
                    //
                    // The prologue instruction sequence is:
                    //
                    //      ldah  Rx, +N(zero)      // set frame size (upper)
                    // ==>  lda   Rx, sN(Rx)        // set frame size (+lower)
                    //      ...
                    //      subq  sp, Rx, sp        // allocate stack frame
                    //

                    DecrementOffset += Offset16;
                    _RtlpDebugDisassemble(ControlPc, ContextRecord);

                } else if (Rb == ZERO_REG) {

                    //
                    // The destination register is the decrement register and
                    // the base register is zero, so this instruction exists
                    // to load an immediate value into the decrement register.
                    // The stack frame size is the new displacement value added
                    // to the previous displacement value, if any.
                    //
                    // The prologue instruction sequence is:
                    //
                    // ==>  lda   Rx, +N(zero)      // set frame size
                    //      ...
                    //      subq  sp, Rx, sp        // allocate stack frame
                    // or
                    //
                    // ==>  ldah  Rx, +N(zero)      // set frame size (upper)
                    //      lda   Rx, sN(Rx)        // set frame size (+lower)
                    //      ...
                    //      subq  sp, Rx, sp        // allocate stack frame
                    //

                    FrameSize = (Offset16 + DecrementOffset);
                    goto StackAllocation;

                } else if (Rb == SP_REG) {

                    //
                    // The destination (Ra) register is SP and the base (Rb)
                    // register is the decrement register, so a two
                    // instruction, large size (>32768 bytes) stack frame
                    // allocation was performed. Add the new displacement
                    // value to the previous displacement value. The negated
                    // displacement value is the stack frame size.
                    //
                    // The prologue instruction sequence is:
                    //
                    // ==>  ldah  Rx, -N(sp)    // prepare new SP (upper)
                    //      lda   sp, sN(Rx)    // allocate stack frame
                    //

                    FrameSize = -(Offset16 + (LONG)DecrementOffset);
                    goto StackAllocation;
                }
            }
            break;

        case ARITH_OP :

            if ((Function == ADDQ_FUNC) &&
                (Instruction.OpReg.RbvType != RBV_REGISTER_FORMAT)) {

                //
                // Add Quadword (immediate) instruction.
                //
                // If the first source register is zero, and the second
                // operand is a literal, and the destination register is
                // the decrement register, then the instruction exists
                // to load an unsigned immediate value less than 256 into
                // the decrement register. The immediate value is the stack
                // frame size.
                //
                // The prologue instruction sequence is:
                //
                // ==>  addq  zero, N, Rx       // set frame size
                //      ...
                //      subq  sp, Rx, sp        // allocate stack frame
                //

                if ((Ra == ZERO_REG) && (Rc == DecrementRegister)) {
                    FrameSize = Literal8;
                    goto StackAllocation;
                }

            } else if ((Function == SUBQ_FUNC) &&
                       (Instruction.OpReg.RbvType == RBV_REGISTER_FORMAT)) {

                //
                // Subtract Quadword (register) instruction.
                //
                // If both source operands are registers and the first
                // source (minuend) register and the destination
                // (difference) register are both SP, then a register value
                // stack allocation was performed and the second source
                // (subtrahend) register value will be added to SP when its
                // value is known. Until that time save the register number of
                // this decrement register.
                //
                // The prologue instruction sequence is:
                //
                //      ldiq  Rx, N             // set frame size
                //      ...
                // ==>  subq  sp, Rx, sp        // allocate stack frame
                //

                if ((Ra == SP_REG) && (Rc == SP_REG)) {
                    DecrementRegister = Rb;
                    DecrementOffset = 0;
                    _RtlpDebugDisassemble(ControlPc, ContextRecord);
                }
            }
            break;

        case BIT_OP :

            //
            // If the second operand is a register the bit set instruction
            // may be a register move instruction, otherwise if the second
            // operand is a literal, the bit set instruction may be a load
            // immediate value instruction.
            //

            if ((Function == BIS_FUNC) && (Rc != ZERO_REG)) {
                if (Instruction.OpReg.RbvType == RBV_REGISTER_FORMAT) {

                    //
                    // Bit Set (register move) instruction.
                    //
                    // If both source registers are the same register, or
                    // one of the source registers is zero, then this is a
                    // register move operation. Restore the value of the
                    // source register by copying the current destination
                    // register value back to the source register.
                    //
                    // The prologue instruction sequence is:
                    //
                    // ==>  bis   Rx, Rx, Ry        // copy register Rx
                    // or
                    //
                    // ==>  bis   Rx, zero, Ry      // copy register Rx
                    // or
                    //
                    // ==>  bis   zero, Rx, Ry      // copy register Rx
                    //

                    if (Ra == ZERO_REG) {

                        //
                        // Map the third case above to the first case.
                        //

                        Ra = Rb;

                    } else if (Rb == ZERO_REG) {

                        //
                        // Map the second case above to the first case.
                        //

                        Rb = Ra;
                    }

                    if ((Ra == Rb) && (Ra != ZERO_REG)) {
                        IntegerRegister[Ra] = IntegerRegister[Rc];


                        //
                        // If the destination register is RA and this is the
                        // first time that RA is being restored, then set the
                        // address of where control left the previous frame.
                        // Otherwise, if this is the second time RA is being
                        // restored, then the first one was an interrupt or
                        // exception address and the return PC should not
                        // have been biased by 4.
                        //

                        if (Ra == RA_REG) {
                            if (RestoredRa == FALSE) {
                                NextPc = (ULONG)ContextRecord->IntRa - 4;
                                RestoredRa = TRUE;

                            } else {
                                NextPc += 4;
                                _RtlpFoundTrapFrame(NextPc);
                            }
                        }

                        //
                        // If the source register is SP and this is the first
                        // time SP is set, then this is a frame pointer set
                        // instruction. Reset the frame pointers to this new
                        // value of SP.
                        //

                        if ((Ra == SP_REG) && (RestoredSp == FALSE)) {
//                            EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;
//                            EstablisherFrame->Real = (ULONG)ContextRecord->IntSp;
                            RestoredSp = TRUE;
                        }
                        _RtlpDebugDisassemble(ControlPc, ContextRecord);
                    }

                } else {

                    //
                    // Bit Set (load immediate) instruction.
                    //
                    // If the first source register is zero, and the second
                    // operand is a literal, and the destination register is
                    // the decrement register, then this instruction exists
                    // to load an unsigned immediate value less than 256 into
                    // the decrement register. The decrement register value is
                    // the stack frame size.
                    //
                    // The prologue instruction sequence is:
                    //
                    // ==>  bis   zero, N, Rx       // set frame size
                    //      ...
                    //      subq  sp, Rx, sp        // allocate stack frame
                    //

                    if ((Ra == ZERO_REG) && (Rc == DecrementRegister)) {
                        FrameSize = Literal8;
StackAllocation:
                        //
                        // Add the frame size to SP to reverse the stack frame
                        // allocation, leave the real frame pointer as is, set
                        // the virtual frame pointer with the updated SP value,
                        // and clear the decrement register.
                        //

                        ContextRecord->IntSp += FrameSize;
//                        EstablisherFrame->Virtual = (ULONG)ContextRecord->IntSp;
                        DecrementRegister = ZERO_REG;
                        _RtlpDebugDisassemble(ControlPc, ContextRecord);
                    }
                }
            }
            break;

        case STT_OP :

            //
            // Store T-Floating (quadword integer) instruction.
            //
            // If the base register is SP, then reload the source register
            // value from the value stored on the stack.
            //
            // The prologue instruction sequence is:
            //
            // ==>  stt   Fx, N(sp)         // save floating register Fx
            //

            if ((Rb == SP_REG) && (Ra != FZERO_REG)) {

                //
                // Reload the register by retrieving the value previously
                // stored on the stack.
                //

                Address = Offset16 + ContextRecord->IntSp;
// MBH
                if (!LocalDoMemoryRead(dp,
                                       Address,
                                       &FloatingRegister[Ra])) {
                    return(0);
                }
//                FloatingRegister[Ra] = *((PUQUAD)Address);
// MBH

                //
                // If a context pointer record is specified, then record
                // the address where the destination register contents are
                // stored.
                //

                _RtlpDebugDisassemble(ControlPc, ContextRecord);
            }
            break;

        case FPOP_OP :

            //
            // N.B. The floating operate function field is not the same as
            // the integer operate nor the jump function fields.
            //

            if (Instruction.FpOp.Function == CPYS_FUNC) {

                //
                // Copy Sign (floating-point move) instruction.
                //
                // If both source registers are the same register, then this is
                // a floating-point register move operation. Restore the value
                // of the source register by copying the current destination
                // register value to the source register.
                //
                // The prologue instruction sequence is:
                //
                // ==>  cpys  Fx, Fx, Fy        // copy floating register Fx
                //

                if ((Ra == Rb) && (Ra != FZERO_REG)) {
                    FloatingRegister[Ra] = FloatingRegister[Rc];
                    _RtlpDebugDisassemble(ControlPc, ContextRecord);
                }
            }

        default :
            break;
        }
    }

//    _RtlpVirtualUnwindExit(NextPc, ContextRecord);
    return NextPc;
}





