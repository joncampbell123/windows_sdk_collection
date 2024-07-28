/**[f******************************************************************
* qsort.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/********************************   qsort.c   ******************************/
/*
*  QSort:  Sorting utilities
*/
  
// history
// 27 apr 89    peterbe     Tabs at 8 spaces, other format cleanup.
  
#include "nocrap.h"
#include "windows.h"
#include "pfm.h"
#include "qsort.h"
#include "debug.h"
  
  
/*  Debugger tracing on pairkern sort function.
*/
#ifdef DEBUG
#define DBG_PKTRACE
#endif
  
#ifdef DBG_PKTRACE
#define DBGpksort(msg)  /*DBMSG(msg)*/
#else
#define DBGpksort(msg)  /*null*/
#endif
  
  
#define LOCAL static
  
#define swap_pk(pk1,pk2,pktmp) \
{   pktmp.kpPair=pk1.kpPair; pktmp.kpKernAmount=pk1.kpKernAmount; \
    pk1.kpPair=pk2.kpPair; pk1.kpKernAmount=pk2.kpKernAmount; \
pk2.kpPair=pktmp.kpPair; pk2.kpKernAmount=pktmp.kpKernAmount; }
  
LOCAL void qsort_pk(short, short, LPKERNPAIR);
  
/*  pksort
*
*  Sort the pair kern table.
*/
void FAR PASCAL pksort(lpPK, len)
LPKERNPAIR lpPK;
short len;
{
    qsort_pk(0, len, lpPK);
}
  
/*  qsort_pk
*
*  Low-level pairkern sort proc.  Standard q-sort algorithm compliments
*  of THINK Technologies and Knuth volume 3, p. 116.
*/
LOCAL void qsort_pk(first, last, lpPK)
short first;
short last;
LPKERNPAIR lpPK;
{
    static KERNPAIR temp_pk;
    register short i;
    register short j;
  
#ifdef DEBUG_FUNCT
    DB(("Entering qsort_pk\n"));
#endif
    #ifdef DBG_PKTRACE
    static short entry_count = 0;
  
    DBGpksort(
    ("\nqsort_pk(%d,%d,%lp): entry %d\n", first, last, lpPK, ++entry_count));
  
    if (entry_count == 1)
    {
        for (i = first; i < last; ++i)
        {
            DBMSG(("%c%c%d:  %c%d,%c%d=%d\n",
            ((i > 100) ? '\0' : ' '), ((i > 10) ? '\0' : ' '), i,
            (char)lpPK[i].kpPair.each[0], (WORD)lpPK[i].kpPair.each[0],
            (char)lpPK[i].kpPair.each[1], (WORD)lpPK[i].kpPair.each[1],
            (short)lpPK[i].kpKernAmount));
        }
    }
    #endif
  
    while (last - first > 1)
    {
        i = first;
        j = last;
  
        while (TRUE)
        {
            while (++i < last && lpPK[i].kpPair.both < lpPK[first].kpPair.both)
                ;
            while (--j > first && lpPK[j].kpPair.both > lpPK[first].kpPair.both)
                ;
  
            if (i >= j)
                break;
  
            DBGpksort(
            ("...inner while, swap items at slots i=%d, j=%d\n", i, j));
  
            swap_pk(lpPK[i], lpPK[j], temp_pk);
        }
  
        DBGpksort(("___end of while, swap items at slots i=%d, j=%d\n", i, j));
  
        swap_pk(lpPK[first], lpPK[j], temp_pk);
  
        if (j - first < last - (j + 1))
        {
            qsort_pk(first, j, lpPK);
            first = j + 1;
        }
        else
        {
            qsort_pk(j + 1, last, lpPK);
            last = j;
        }
  
        DBGpksort(
        ("***bottom of big while[%d], i=%d, j=%d, first=%d, last=%d, entry=%d\n",
        entry_count, i, j, first, last));
    }
  
#ifdef DEBUG_FUNCT
    DB(("Entering qsort_pk\n"));
#endif
    DBGpksort(("qsort_pk(): exit %d\n", entry_count--));
}
  
