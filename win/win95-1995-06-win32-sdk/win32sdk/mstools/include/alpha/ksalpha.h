#include "kxalpha.h"

//
// Context Frame Offset and Flag Definitions
//

#define CONTEXT_FULL 0x20007
#define CONTEXT_CONTROL 0x20001
#define CONTEXT_FLOATING_POINT 0x20002
#define CONTEXT_INTEGER 0x20004

#define CxFltF0 0x0
#define CxFltF1 0x8
#define CxFltF2 0x10
#define CxFltF3 0x18
#define CxFltF4 0x20
#define CxFltF5 0x28
#define CxFltF6 0x30
#define CxFltF7 0x38
#define CxFltF8 0x40
#define CxFltF9 0x48
#define CxFltF10 0x50
#define CxFltF11 0x58
#define CxFltF12 0x60
#define CxFltF13 0x68
#define CxFltF14 0x70
#define CxFltF15 0x78
#define CxFltF16 0x80
#define CxFltF17 0x88
#define CxFltF18 0x90
#define CxFltF19 0x98
#define CxFltF20 0xa0
#define CxFltF21 0xa8
#define CxFltF22 0xb0
#define CxFltF23 0xb8
#define CxFltF24 0xc0
#define CxFltF25 0xc8
#define CxFltF26 0xd0
#define CxFltF27 0xd8
#define CxFltF28 0xe0
#define CxFltF29 0xe8
#define CxFltF30 0xf0
#define CxFltF31 0xf8
#define CxIntV0 0x100
#define CxIntT0 0x108
#define CxIntT1 0x110
#define CxIntT2 0x118
#define CxIntT3 0x120
#define CxIntT4 0x128
#define CxIntT5 0x130
#define CxIntT6 0x138
#define CxIntT7 0x140
#define CxIntS0 0x148
#define CxIntS1 0x150
#define CxIntS2 0x158
#define CxIntS3 0x160
#define CxIntS4 0x168
#define CxIntS5 0x170
#define CxIntFp 0x178
#define CxIntA0 0x180
#define CxIntA1 0x188
#define CxIntA2 0x190
#define CxIntA3 0x198
#define CxIntA4 0x1a0
#define CxIntA5 0x1a8
#define CxIntT8 0x1b0
#define CxIntT9 0x1b8
#define CxIntT10 0x1c0
#define CxIntT11 0x1c8
#define CxIntRa 0x1d0
#define CxIntT12 0x1d8
#define CxIntAt 0x1e0
#define CxIntGp 0x1e8
#define CxIntSp 0x1f0
#define CxIntZero 0x1f8
#define CxFpcr 0x200
#define CxSoftFpcr 0x208
#define CxFir 0x210
#define CxPsr 0x218
#define CxContextFlags 0x21c
#define ContextFrameLength 0x230

//
// Breakpoint Definitions
//

#define USER_BREAKPOINT 0x0
#define KERNEL_BREAKPOINT 0x1
#define BREAKIN_BREAKPOINT 0x19
#define DEBUG_PRINT_BREAKPOINT 0x14
#define DEBUG_PROMPT_BREAKPOINT 0x15
#define DEBUG_STOP_BREAKPOINT 0x16
#define DEBUG_LOAD_SYMBOLS_BREAKPOINT 0x17
#define DEBUG_UNLOAD_SYMBOLS_BREAKPOINT 0x18

//
// Trap Code Definitions
//

#define GENTRAP_INTEGER_OVERFLOW 0xffffffff
#define GENTRAP_INTEGER_DIVIDE_BY_ZERO 0xfffffffe
#define GENTRAP_FLOATING_OVERFLOW 0xfffffffd
#define GENTRAP_FLOATING_DIVIDE_BY_ZERO 0xfffffffc
#define GENTRAP_FLOATING_UNDERFLOW 0xfffffffb
#define GENTRAP_FLOATING_INVALID_OPERAND 0xfffffffa
#define GENTRAP_FLOATING_INEXACT_RESULT 0xfffffff9

//
// Call PAL mnemonics
//

#define bpt 0x80
#define callsys 0x83
#define imb 0x86
#define gentrap 0xaa
#define rdteb 0xab
#define kbpt 0xac
#define callkd 0xad

