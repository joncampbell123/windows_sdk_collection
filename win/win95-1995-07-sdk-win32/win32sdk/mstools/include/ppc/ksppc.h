#include "kxppc.h"

//
// Context Frame Offset and Flag Definitions
//

#define CONTEXT_CONTROL         0x00000001L
#define CONTEXT_FLOATING_POINT  0x00000002L
#define CONTEXT_INTEGER         0x00000004L
#define CONTEXT_DEBUG_REGISTERS 0x00000008L

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_FLOATING_POINT | CONTEXT_INTEGER)

#define CxFpr0 0x0
#define CxFpr1 0x8
#define CxFpr2 0x10
#define CxFpr3 0x18
#define CxFpr4 0x20
#define CxFpr5 0x28
#define CxFpr6 0x30
#define CxFpr7 0x38
#define CxFpr8 0x40
#define CxFpr9 0x48
#define CxFpr10 0x50
#define CxFpr11 0x58
#define CxFpr12 0x60
#define CxFpr13 0x68
#define CxFpr14 0x70
#define CxFpr15 0x78
#define CxFpr16 0x80
#define CxFpr17 0x88
#define CxFpr18 0x90
#define CxFpr19 0x98
#define CxFpr20 0xa0
#define CxFpr21 0xa8
#define CxFpr22 0xb0
#define CxFpr23 0xb8
#define CxFpr24 0xc0
#define CxFpr25 0xc8
#define CxFpr26 0xd0
#define CxFpr27 0xd8
#define CxFpr28 0xe0
#define CxFpr29 0xe8
#define CxFpr30 0xf0
#define CxFpr31 0xf8
#define CxFpscr 0x100
#define CxGpr0 0x108
#define CxGpr1 0x10c
#define CxGpr2 0x110
#define CxGpr3 0x114
#define CxGpr4 0x118
#define CxGpr5 0x11c
#define CxGpr6 0x120
#define CxGpr7 0x124
#define CxGpr8 0x128
#define CxGpr9 0x12c
#define CxGpr10 0x130
#define CxGpr11 0x134
#define CxGpr12 0x138
#define CxGpr13 0x13c
#define CxGpr14 0x140
#define CxGpr15 0x144
#define CxGpr16 0x148
#define CxGpr17 0x14c
#define CxGpr18 0x150
#define CxGpr19 0x154
#define CxGpr20 0x158
#define CxGpr21 0x15c
#define CxGpr22 0x160
#define CxGpr23 0x164
#define CxGpr24 0x168
#define CxGpr25 0x16c
#define CxGpr26 0x170
#define CxGpr27 0x174
#define CxGpr28 0x178
#define CxGpr29 0x17c
#define CxGpr30 0x180
#define CxGpr31 0x184
#define CxCr 0x188
#define CxXer 0x18c
#define CxMsr 0x190
#define CxIar 0x194
#define CxLr 0x198
#define CxCtr 0x19c
#define CxContextFlags 0x1a0
#define CxDr0 0x1b0
#define CxDr1 0x1b4
#define CxDr2 0x1b8
#define CxDr3 0x1bc
#define CxDr4 0x1c0
#define CxDr5 0x1c4
#define CxDr6 0x1c8
#define CxDr7 0x1cc
#define ContextFrameLength 0x1d0


//
// Breakpoint Definitions
//

#define USER_BREAKPOINT 0x0
#define KERNEL_BREAKPOINT 0x1
#define BREAKIN_BREAKPOINT 0x2
#define BRANCH_TAKEN_BREAKPOINT 0x3
#define BRANCH_NOT_TAKEN_BREAKPOINT 0x4
#define SINGLE_STEP_BREAKPOINT 0x5
#define DIVIDE_OVERFLOW_BREAKPOINT 0x6
#define DIVIDE_BY_ZERO_BREAKPOINT 0x7
#define RANGE_CHECK_BREAKPOINT 0x8
#define STACK_OVERFLOW_BREAKPOINT 0x9
#define MULTIPLY_OVERFLOW_BREAKPOINT 0xa
#define DEBUG_PRINT_BREAKPOINT 0x14
#define DEBUG_PROMPT_BREAKPOINT 0x15
#define DEBUG_STOP_BREAKPOINT 0x16
#define DEBUG_LOAD_SYMBOLS_BREAKPOINT 0x17
#define DEBUG_UNLOAD_SYMBOLS_BREAKPOINT 0x18
