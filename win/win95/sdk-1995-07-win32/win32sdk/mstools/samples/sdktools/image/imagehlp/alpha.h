
typedef DWORDLONG  ULONGLONG;
typedef PDWORDLONG PULONGLONG;

//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {

    PULONGLONG FloatingContext[1];
    PULONGLONG FltF1;
    // Nonvolatile floating point registers start here.
    PULONGLONG FltF2;
    PULONGLONG FltF3;
    PULONGLONG FltF4;
    PULONGLONG FltF5;
    PULONGLONG FltF6;
    PULONGLONG FltF7;
    PULONGLONG FltF8;
    PULONGLONG FltF9;
    PULONGLONG FltF10;
    PULONGLONG FltF11;
    PULONGLONG FltF12;
    PULONGLONG FltF13;
    PULONGLONG FltF14;
    PULONGLONG FltF15;
    PULONGLONG FltF16;
    PULONGLONG FltF17;
    PULONGLONG FltF18;
    PULONGLONG FltF19;
    PULONGLONG FltF20;
    PULONGLONG FltF21;
    PULONGLONG FltF22;
    PULONGLONG FltF23;
    PULONGLONG FltF24;
    PULONGLONG FltF25;
    PULONGLONG FltF26;
    PULONGLONG FltF27;
    PULONGLONG FltF28;
    PULONGLONG FltF29;
    PULONGLONG FltF30;
    PULONGLONG FltF31;

    PULONGLONG IntegerContext[1];
    PULONGLONG IntT0;
    PULONGLONG IntT1;
    PULONGLONG IntT2;
    PULONGLONG IntT3;
    PULONGLONG IntT4;
    PULONGLONG IntT5;
    PULONGLONG IntT6;
    PULONGLONG IntT7;
    // Nonvolatile integer registers start here.
    PULONGLONG IntS0;
    PULONGLONG IntS1;
    PULONGLONG IntS2;
    PULONGLONG IntS3;
    PULONGLONG IntS4;
    PULONGLONG IntS5;
    PULONGLONG IntFp;
    PULONGLONG IntA0;
    PULONGLONG IntA1;
    PULONGLONG IntA2;
    PULONGLONG IntA3;
    PULONGLONG IntA4;
    PULONGLONG IntA5;
    PULONGLONG IntT8;
    PULONGLONG IntT9;
    PULONGLONG IntT10;
    PULONGLONG IntT11;
    PULONGLONG IntRa;
    PULONGLONG IntT12;
    PULONGLONG IntAt;
    PULONGLONG IntGp;
    PULONGLONG IntSp;
    PULONGLONG IntZero;

} KNONVOLATILE_CONTEXT_POINTERS, *PKNONVOLATILE_CONTEXT_POINTERS;


// Exception frame
//
//  This frame is established when handling an exception. It provides a place
//  to save all nonvolatile registers. The volatile registers will already
//  have been saved in a trap frame.
//
//  The layout of the record conforms to a standard call frame since it is
//  used as such. Thus it contains a place to save a return address and is
//  padded so that it is EXACTLY a multiple of 32 bytes in length.
//
//
//  N.B - the 32-byte alignment is more stringent than required by the
//  calling standard (which requires 16-byte alignment), the 32-byte alignment
//  is established for performance reasons in the interaction with the PAL.
//

typedef struct _KEXCEPTION_FRAME {

    ULONGLONG IntRa;    // return address register, ra

    ULONGLONG FltF2;    // nonvolatile floating registers, f2 - f9
    ULONGLONG FltF3;
    ULONGLONG FltF4;
    ULONGLONG FltF5;
    ULONGLONG FltF6;
    ULONGLONG FltF7;
    ULONGLONG FltF8;
    ULONGLONG FltF9;

    ULONGLONG IntS0;    //  nonvolatile integer registers, s0 - s5
    ULONGLONG IntS1;
    ULONGLONG IntS2;
    ULONGLONG IntS3;
    ULONGLONG IntS4;
    ULONGLONG IntS5;
    ULONGLONG IntFp;    // frame pointer register, fp/s6

    ULONGLONG SwapReturn;
    ULONG Psr;          // processor status
    ULONG Fill[5];      // padding for 32-byte stack frame alignment
                        // N.B. - Ulongs from the filler section are used
                        //        in ctxsw.s - do not delete

} KEXCEPTION_FRAME, *PKEXCEPTION_FRAME;


#include <alphaops.h>
