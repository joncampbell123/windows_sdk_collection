/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/***    Arena Header
 */

struct arena {
    UCHAR  arena_signature;     // 4D for valid item, 5A for last item
    USHORT arena_owner;         // owner of arena item
    USHORT arena_size;          // size in paragraphs of item
    UCHAR  arena_reserved[3];   // reserved
    UCHAR  arena_name[8];       // owner file name
};
typedef struct arena arena;

/*
 * CAUTION: The routines in ALLOC.ASM rely on the fact that arena_signature
 * and arena_owner_system are all equal to zero and are contained in DI.  Change
 * them and change ALLOC.ASM.
 *
 * I think I have all of these covered via .errnz - JGL
 */

#define arena_owner_system      0       // free block indication

#define arena_signature_normal  0x4D    // valid signature, not end of arena
#define arena_signature_end     0x5A    // valid signature, last block in arena


#define FIRST_FIT               0x00
#define BEST_FIT                0x01
#define LAST_FIT                0x02

#define LOW_FIRST               0x00
#define HIGH_FIRST              0x80
#define HIGH_ONLY               0x40

#define LINKSTATE               0x01

#define HF_MASK                 (~HIGH_FIRST)
#define HO_MASK                 (~HIGH_ONLY)

#define STRAT_MASK              (HF_MASK & HO_MASK) // used to mask of bits
                                                    // 6 & 7 of AllocMethod
