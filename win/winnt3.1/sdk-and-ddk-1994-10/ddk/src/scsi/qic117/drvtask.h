/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

   drvtask.h

Abstract:

Revision History:


--*/

/***********************************************************************/
/* Task manager defines */
/***********************************************************************/

/* used in mt_driver_waiting */
#define IDLE 0

/* used in IRQ_struct.irq_flags */
#define FIRST                           0x80            /* IRQ format (See IBM PS/2 interrupt sharing documentation) */

/***********************************************************************/
/* Task manager globals */
/***********************************************************************/

/* hardware info (retrieved from config) */

/* Timing values for mt_Sleep */


typedef enum _QIC_TIME {
    t0010us     =     10l,
    t0012us     =     12l,
    mt_wt2ticks =     31l,
    mt_wt090ms  =     90l,
    mt_wt200ms  =    200l,
    mt_wt260ms  =    260l,
    mt_wt500ms  =    500l,
    mt_wt001s   =   1000l,
    mt_wt003s   =   3000l,
    mt_wt004s   =   4000l,
    mt_wt005s   =   5000l,
    mt_wt007s   =   7000l,
    mt_wt010s   =  10000l,
    mt_wt016s   =  16000l,
    mt_wt035s   =  35000l,
    mt_wt045s   =  45000l,
    mt_wt050s   =  50000l,
    mt_wt060s   =  60000l,
    mt_wt065s   =  65000l,
    mt_wt085s   =  85000l,
    mt_wt090s   =  90000l,
    mt_wt100s   = 100000l,
    mt_wt105s   = 105000l,
    mt_wt130s   = 130000l,
    mt_wt150s   = 150000l,
    mt_wt180s   = 180000l,
    mt_wt200s   = 200000l,
    mt_wt228s   = 228000l,
    mt_wt250s   = 250000l,
    mt_wt260s   = 260000l,
    mt_wt300s   = 300000l,
    mt_wt350s   = 350000l,
    mt_wt455s   = 455000l,
    mt_wt460s   = 460000l,
    mt_wt475s   = 475000l,
    mt_wt700s   = 700000l,
    mt_wt910s   = 910000l,
    mt_wttrks   =   5000l
} QIC_TIME, *PQIC_TIME;
