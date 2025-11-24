

//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
    PULONG FloatingContext[20];
    PULONG FltF20;
    PULONG FltF21;
    PULONG FltF22;
    PULONG FltF23;
    PULONG FltF24;
    PULONG FltF25;
    PULONG FltF26;
    PULONG FltF27;
    PULONG FltF28;
    PULONG FltF29;
    PULONG FltF30;
    PULONG FltF31;
    PULONG IntegerContext[16];
    PULONG IntS0;
    PULONG IntS1;
    PULONG IntS2;
    PULONG IntS3;
    PULONG IntS4;
    PULONG IntS5;
    PULONG IntS6;
    PULONG IntS7;
    PULONG IntT8;
    PULONG IntT9;
    PULONG IntK0;
    PULONG IntK1;
    PULONG IntGp;
    PULONG IntSp;
    PULONG IntS8;
    PULONG IntRa;
} KNONVOLATILE_CONTEXT_POINTERS, *PKNONVOLATILE_CONTEXT_POINTERS;


//
// Exception frame
//
//  N.B. This frame must be an exact multiple of 8 bytes in length.
//

typedef struct _KEXCEPTION_FRAME {
    ULONG Argument[8];
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
    ULONG IntS0;
    ULONG IntS1;
    ULONG IntS2;
    ULONG IntS3;
    ULONG IntS4;
    ULONG IntS5;
    ULONG IntS6;
    ULONG IntS7;
    ULONG IntS8;
    ULONG SwapReturn;
    ULONG IntRa;
    ULONG Fill1;
} KEXCEPTION_FRAME, *PKEXCEPTION_FRAME;
