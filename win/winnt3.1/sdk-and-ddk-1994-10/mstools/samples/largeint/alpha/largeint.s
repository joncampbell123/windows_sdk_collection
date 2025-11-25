//      TITLE("Large Integer Arithmetic")
//++
//
// Copyright (c) 1990  Microsoft Corporation
// Copyright (c) 1993  Digital Equipment Corporation
//
// Module Name:
//
//    largeint.s
//
// Abstract:
//
//    This module implements routines for performing extended integer
//    arithmetic.
//
// Author:
//
//    David N. Cutler (davec) 18-Apr-1990
//
// Environment:
//
//    Any mode.
//
// Revision History:
//
//    Thomas Van Baak (tvb) 9-May-1992
//
//        Adapted for Alpha AXP.
//
//--

#include "ksalpha.h"

//
// Alpha AXP Implementation Notes:
//
//    The LargeInteger functions defined below implement a set of portable
//    64-bit integer arithmetic operations for x86, Mips, and Alpha systems
//    using the LARGE_INTEGER data type. Code using LARGE_INTEGER variables
//    and calling these functions will be portable across all NT platforms.
//    This is the recommended approach to 64-bit arithmetic on NT.
//
//    However, if performance is more important than portability, then for
//    Alpha systems, the native 64-bit integer data types may be used instead
//    of the LARGE_INTEGER type and the LargeInteger functions. The Alpha C
//    compilers support a __int64 data type (renamed LONGLONG in the system
//    header files). All C integer arithmetic operators may be used with
//    these quadword types. This eliminates the need for, and the overhead
//    of, any of the portable LargeInteger functions.
//
//    In general, a LARGE_INTEGER cannot simply be converted to a LONGLONG
//    because of explicit references in application code to the 32-bit LowPart
//    and HighPart members of the LARGE_INTEGER structure.
//
//    The performance difference between using the portable LARGE_INTEGER
//    types with LargeInteger functions and the Alpha LONGLONG types and
//    operations is often not significant enough to warrant modifying otherwise
//    portable code. In addition, future compiler optimization and inlining may
//    actually result in identical performance between portable LARGE_INTEGER
//    code and Alpha specific LONGLONG code. Therefore it is recommended to
//    keep NT source code portable.
//
//    The Alpha source for the large integer functions below differs from the
//    Mips version since for Alpha a 64-bit argument or return value is passed
//    through a 64-bit integer register. In addition, most operations are
//    implemented with a single instruction. The division routines below,
//    however, like Mips, use a simple shift/subtract algorithm which is
//    considerably slower than the algorithm used by Alpha runtime division
//    routines.
//

        SBTTL("Convert Long to Large Integer")
//++
//
// LARGE_INTEGER
// ConvertLongToLargeInteger (
//    IN LONG SignedInteger
//    )
//
// Routine Description:
//
//    This function converts a signed integer to a signed large integer
//    and returns the result.
//
// Arguments:
//
//    SignedInteger (a0) - Supplies the value to convert.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(ConvertLongToLargeInteger)

        addl    a0, 0, v0               // ensure canonical (signed long) form
        ret     zero, (ra)              // return

        .end    ConvertLongToLargeInteger

        SBTTL("Convert Ulong to Large Integer")
//++
//
// LARGE_INTEGER
// ConvertUlongToLargeInteger (
//    IN ULONG UnsignedInteger
//    )
//
// Routine Description:
//
//    This function converts an unsigned integer to a signed large
//    integer and returns the result.
//
// Arguments:
//
//    UnsignedInteger (a0) - Supplies the value to convert.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(ConvertUlongToLargeInteger)

        zap     a0, 0xf0, v0            // convert canonical ULONG to quadword
        ret     zero, (ra)              // return

        .end    ConvertUlongToLargeInteger

        SBTTL("Enlarged Signed Integer Multiply")
//++
//
// LARGE_INTEGER
// EnlargedIntegerMultiply (
//    IN LONG Multiplicand,
//    IN LONG Multiplier
//    )
//
// Routine Description:
//
//    This function multiplies a signed integer by a signed integer and
//    returns a signed large integer result.
//
//    N.B. An overflow is not possible.
//
// Arguments:
//
//    Multiplicand (a0) - Supplies the multiplicand value.
//
//    Multiplier (a1) - Supplies the multiplier value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(EnlargedIntegerMultiply)

        addl    a0, 0, a0               // ensure canonical (signed long) form
        addl    a1, 0, a1               // ensure canonical (signed long) form
        mulq    a0, a1, v0              // multiply signed both quadwords
        ret     zero, (ra)              // return

        .end    EnlargedIntegerMultiply

        SBTTL("Enlarged Unsigned Divide")
//++
//
// ULONG
// EnlargedUnsignedDivide (
//    IN ULARGE_INTEGER Dividend,
//    IN ULONG Divisor,
//    IN OUT PULONG Remainder OPTIONAL
//    )
//
// Routine Description:
//
//    This function divides an unsigned large integer by an unsigned long
//    and returns the resultant quotient and optionally the remainder.
//
//    N.B. An overflow or divide by zero exception is possible.
//
// Arguments:
//
//    Dividend (a0) - Supplies the unsigned 64-bit dividend value.
//
//    Divisor (a1) - Supplies the unsigned 32-bit divisor value.
//
//    Remainder (a2) - Supplies an optional pointer to a variable that
//      receives the unsigned 32-bit remainder.
//
// Return Value:
//
//    The unsigned long integer quotient is returned as the function value.
//
//--

        LEAF_ENTRY(EnlargedUnsignedDivide)

//
// Check for division by zero.
//

        zap     a1, 0xf0, a1            // convert ULONG divisor to quadword
        beq     a1, 30f                 // trap if divisor is zero

//
// Check for overflow. If the divisor is less than the upper half of the
// dividend the quotient would be wider than 32 bits.
//

        srl     a0, 32, t0              // get upper longword of dividend
        cmpule  a1, t0, t1              // is divisor <= upper dividend?
        bne     t1, 40f                 // if ne[true], then overflow trap

//
// Perform the shift/subtract loop 8 times and 4 bits per loop.
//
// t0 - Temp used for 0/1 results of compares.
// t1 - High 64-bits of 128-bit (t1, a0) dividend.
// t2 - Loop counter.
//

        ldiq    t2, 32/4                // set iteration count

        srl     a0, 32, t1              // get top 32 bits of carry-out
        sll     a0, 32, a0              // preshift first 32 bits left

10:     cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        subq    t2, 1, t2               // any more iterations?
        bne     t2, 10b                 //

//
// Finished with remainder value in t1 and quotient value in a0.
//

        addl    a0, 0, v0               // set longword quotient return value
        beq     a2, 20f                 // skip optional remainder store
        stl     t1, 0(a2)               // store longword remainder

20:     ret     zero, (ra)              // return

//
// Generate an exception for divide by zero and return a zero quotient if the
// caller continues execution.
//

30:     ldil    a0, GENTRAP_INTEGER_DIVIDE_BY_ZERO

        GENERATE_TRAP

        ldil    v0, 0                   // return zero quotient
        ret     zero, (ra)              // return

//
// Generate an exception for overflow.
//

40:     ldiq    a0, 0x8000000000000000  //
        subqv   zero, a0, v0            // negate in order to overflow
        trapb                           // wait for trap to occur
        ret     zero, (ra)              // return

        .end    EnlargedUnsignedDivide

        SBTTL("Enlarged Unsigned Integer Multiply")
//++
//
// LARGE_INTEGER
// EnlargedUnsignedMultiply (
//    IN ULONG Multiplicand,
//    IN ULONG Multiplier
//    )
//
// Routine Description:
//
//    This function multiplies an unsigned integer by an unsigned integer
//    and returns a signed large integer result.
//
// Arguments:
//
//    Multiplicand (a0) - Supplies the multiplicand value.
//
//    Multiplier (a1) - Supplies the multiplier value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(EnlargedUnsignedMultiply)

        zap     a0, 0xf0, a0            // convert canonical ULONG to quadword
        zap     a1, 0xf0, a1            // convert canonical ULONG to quadword
        mulq    a0, a1, v0              // multiply signed both quadwords
        ret     zero, (ra)              // return

        .end    EnlargedUnsignedMultiply

        SBTTL("Extended Integer Multiply")
//++
//
// LARGE_INTEGER
// ExtendedIntegerMultiply (
//    IN LARGE_INTEGER Multiplicand,
//    IN LONG Multiplier
//    )
//
// Routine Description:
//
//    This function multiplies a signed large integer by a signed integer and
//    returns the signed large integer result.
//
//    N.B. An overflow is possible, but no exception is generated.
//
// Arguments:
//
//    Multiplicand (a0) - Supplies the multiplicand value.
//
//    Multiplier (a1) - Supplies the multiplier value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(ExtendedIntegerMultiply)

        addl    a1, 0, a1               // ensure canonical (signed long) form
        mulq    a0, a1, v0              // multiply signed both quadwords
        ret     zero, (ra)              // return

        .end    ExtendedIntegerMultiply

        SBTTL("Extended Large Integer Divide")
//++
//
// LARGE_INTEGER
// ExtendedLargeIntegerDivide (
//    IN LARGE_INTEGER Dividend,
//    IN ULONG Divisor,
//    IN OUT PULONG Remainder OPTIONAL
//    )
//
// Routine Description:
//
//    This function divides an unsigned large integer by an unsigned long
//    and returns the quadword quotient and optionally the long remainder.
//
//    N.B. A divide by zero exception is possible.
//
// Arguments:
//
//    Dividend (a0) - Supplies the dividend value.
//
//    Divisor (a1) - Supplies the divisor value.
//
//    Remainder (a2) - Supplies an optional pointer to a variable that
//      receives the remainder.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(ExtendedLargeIntegerDivide)

//
// Check for division by zero.
//

        zap     a1, 0xf0, a1            // convert canonical ULONG to quadword
        beq     a1, 30f                 // trap if divisor is zero

//
// Perform the shift/subtract loop 16 times and 4 bits per loop.
//
// t0 - Temp used for 0/1 results of compares.
// t1 - High 64-bits of 128-bit (t1, a0) dividend.
// t2 - Loop counter.
//

        ldiq    t2, 64/4                // set iteration count

        ldiq    t1, 0                   // zero-extend dividend to 128 bits

10:     cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        subq    t2, 1, t2               // any more iterations?
        bne     t2, 10b                 //

//
// Finished with remainder value in t1 and quotient value in a0.
//

        mov     a0, v0                  // set quadword quotient return value
        beq     a2, 20f                 // skip optional remainder store
        stl     t1, 0(a2)               // store longword remainder

20:     ret     zero, (ra)              // return

//
// Generate an exception for divide by zero.
//

30:     ldil    a0, GENTRAP_INTEGER_DIVIDE_BY_ZERO

        GENERATE_TRAP

        br      zero, 30b               // in case they continue

        .end    ExtendedLargeIntegerDivide

        SBTTL("Extended Magic Divide")
//++
//
// LARGE_INTEGER
// ExtendedMagicDivide (
//    IN LARGE_INTEGER Dividend,
//    IN LARGE_INTEGER MagicDivisor,
//    IN CCHAR ShiftCount
//    )
//
// Routine Description:
//
//    This function divides a signed large integer by an unsigned large integer
//    and returns the signed large integer result. The division is performed
//    using reciprocal multiplication of a signed large integer value by an
//    unsigned large integer fraction which represents the most significant
//    64-bits of the reciprocal divisor rounded up in its least significant bit
//    and normalized with respect to bit 63. A shift count is also provided
//    which is used to truncate the fractional bits from the result value.
//
// Arguments:
//
//    Dividend (a0) - Supplies the dividend value.
//
//    MagicDivisor (a1) - Supplies the magic divisor value which
//      is a 64-bit multiplicative reciprocal.
//
//    Shiftcount (a2) - Supplies the right shift adjustment value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(ExtendedMagicDivide)

//
// Make the dividend positive for the reciprocal multiplication to work.
//

        negq    a0, t0                  // negate dividend
        cmovgt  a0, a0, t0              // get absolute value in t0

//
// Multiply both quadword arguments together and take only the upper 64 bits of
// the resulting 128 bit product. This can be done using the umulh instruction.
//
// Division of a dividend by a constant divisor through reciprocal
// multiplication works because a/b is equivalent to (a*x)/(b*x) for any
// value of x. Now if b is a constant, some "magic" integer x can be chosen,
// based on the value of b, so that (b*x) is very close to a large power of
// two (e.g., 2^64). Then a/b = (a*x)/(b*x) = (a*x)/(2^64) = (a*x)>>64. This
// effectively turns the problem of division by a constant into multiplication
// by a constant which is a much faster operation.
//

        umulh   t0, a1, t1              // multiply high both quadword arguments
        sra     t1, a2, t1              // shift result right by requested amount

//
// Make the result negative if the dividend was negative.
//

        negq    t1, t0                  // negate result
        cmovgt  a0, t1, t0              // restore sign of dividend

        mov     t0, v0                  // set quadword result return value
        ret     zero, (ra)              // return

        .end    ExtendedMagicDivide

        SBTTL("Large Integer Add")
//++
//
// LARGE_INTEGER
// LargeIntegerAdd (
//    IN LARGE_INTEGER Addend1,
//    IN LARGE_INTEGER Addend2
//    )
//
// Routine Description:
//
//    This function adds a signed large integer to a signed large integer and
//    returns the signed large integer result.
//
//    N.B. An overflow is possible, but no exception is generated.
//
// Arguments:
//
//    Addend1 (a0) - Supplies the first addend value.
//
//    Addend2 (a1) - Supplies the second addend value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerAdd)

        addq    a0, a1, v0              // add both quadword arguments
        ret     zero, (ra)              // return

        .end    LargeIntegerAdd

        SBTTL("Large Integer Arithmetic Shift Right")
//++
//
// LARGE_INTEGER
// LargeIntegerArithmeticShift (
//    IN LARGE_INTEGER LargeInteger,
//    IN CCHAR ShiftCount
//    )
//
// Routine Description:
//
//    This function shifts a signed large integer right by an unsigned integer
//    modulo 64 and returns the shifted signed large integer result.
//
// Arguments:
//
//    LargeInteger (a0) - Supplies the large integer to be shifted.
//
//    ShiftCount (a1) - Supplies the right shift count.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerArithmeticShift)

        sra     a0, a1, v0              // shift the quadword right/arithmetic
        ret     zero, (ra)              // return

        .end    LargeIntegerArithmeticShift

        SBTTL("Large Integer Divide")
//++
//
// LARGE_INTEGER
// LargeIntegerDivide (
//    IN LARGE_INTEGER Dividend,
//    IN LARGE_INTEGER Divisor,
//    IN OUT PLARGE_INTEGER Remainder OPTIONAL
//    )
//
// Routine Description:
//
//    This function divides an unsigned large integer by an unsigned large
//    integer and returns the quadword quotient and optionally the quadword
//    remainder.
//
//    N.B. A divide by zero exception is possible.
//
// Arguments:
//
//    Dividend (a0) - Supplies the dividend value.
//
//    Divisor (a1) - Supplies the divisor value.
//
//    Remainder (a2) - Supplies an optional pointer to a variable that
//      receives the remainder.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerDivide)

//
// Check for division by zero.
//

        beq     a1, 30f                 // trap if divisor is zero

//
// Perform the shift/subtract loop 16 times and 4 bits per loop.
//
// t0 - Temp used for 0/1 results of compares.
// t1 - High 64-bits of 128-bit (t1, a0) dividend.
// t2 - Loop counter.
//

        ldiq    t2, 64/4                // set iteration count

        ldiq    t1, 0                   // zero-extend dividend to 128 bits

10:     cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        cmplt   a0, 0, t0               // predict low-dividend shift carry-out
        addq    a0, a0, a0              // shift low-dividend left
        addq    t1, t1, t1              // shift high-dividend left
        bis     t1, t0, t1              // merge in carry-out of low-dividend

        cmpule  a1, t1, t0              // if dividend >= divisor,
        addq    a0, t0, a0              //   then set quotient bit
        subq    t1, a1, t0              // subtract divisor from dividend,
        cmovlbs a0, t0, t1              //   if dividend >= divisor

        subq    t2, 1, t2               // any more iterations?
        bne     t2, 10b                 //

//
// Finished with remainder value in t1 and quotient value in a0.
//

        mov     a0, v0                  // set quadword quotient return value
        beq     a2, 20f                 // skip optional remainder store
        stq     t1, 0(a2)               // store quadword remainder

20:     ret     zero, (ra)              // return

//
// Generate an exception for divide by zero.
//

30:     ldil    a0, GENTRAP_INTEGER_DIVIDE_BY_ZERO

        GENERATE_TRAP

        br      zero, 30b               // in case they continue

        .end    LargeIntegerDivide

        SBTTL("Large Integer Negate")
//++
//
// LARGE_INTEGER
// LargeIntegerNegate (
//    IN LARGE_INTEGER Subtrahend
//    )
//
// Routine Description:
//
//    This function negates a signed large integer and returns the signed
//    large integer result.
//
//    N.B. An overflow is possible, but no exception is generated.
//
// Arguments:
//
//    Subtrahend (a0) - Supplies the subtrahend value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerNegate)

        subq    zero, a0, v0            // negate the quadword argument
        ret     zero, (ra)              // return

        .end    LargeIntegerNegate

        SBTTL("Large Integer Shift Left")
//++
//
// LARGE_INTEGER
// LargeIntegerShiftLeft (
//    IN LARGE_INTEGER LargeInteger,
//    IN CCHAR ShiftCount
//    )
//
// Routine Description:
//
//    This function shifts a signed large integer left by an unsigned integer
//    modulo 64 and returns the shifted signed large integer result.
//
// Arguments:
//
//    LargeInteger (a0) - Supplies the large integer to be shifted.
//
//    ShiftCount (a1) - Supplies the left shift count.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerShiftLeft)

        sll     a0, a1, v0              // shift the quadword argument left
        ret     zero, (ra)              // return

        .end    LargeIntegerShiftLeft

        SBTTL("Large Integer Logical Shift Right")
//++
//
// LARGE_INTEGER
// LargeIntegerShiftRight (
//    IN LARGE_INTEGER LargeInteger,
//    IN CCHAR ShiftCount
//    )
//
// Routine Description:
//
//    This function shifts an unsigned large integer right by an unsigned
//    integer modulo 64 and returns the shifted unsigned large integer result.
//
// Arguments:
//
//    LargeInteger (a0) - Supplies the large integer to be shifted.
//
//    ShiftCount (a1) - Supplies the right shift count.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerShiftRight)

        srl     a0, a1, v0              // shift the quadword right/logical
        ret     zero, (ra)              // return

        .end    LargeIntegerShiftRight

        SBTTL("Large Integer Subtract")
//++
//
// LARGE_INTEGER
// LargeIntegerSubtract (
//    IN LARGE_INTEGER Minuend,
//    IN LARGE_INTEGER Subtrahend
//    )
//
// Routine Description:
//
//    This function subtracts a signed large integer from a signed large
//    integer and returns the signed large integer result.
//
//    N.B. An overflow is possible, but no exception is generated.
//
// Arguments:
//
//    Minuend (a0) - Supplies the minuend value.
//
//    Subtrahend (a1) - Supplies the subtrahend value.
//
// Return Value:
//
//    The large integer result is returned as the function value in v0.
//
//--

        LEAF_ENTRY(LargeIntegerSubtract)

        subq    a0, a1, v0              // subtract the quadword arguments
        ret     zero, (ra)              // return

        .end    LargeIntegerSubtract
