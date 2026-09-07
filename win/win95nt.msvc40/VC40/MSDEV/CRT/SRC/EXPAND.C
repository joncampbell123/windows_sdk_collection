/***
*expand.c - Win32 expand heap routine
*
*       Copyright (c) 1991-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#ifdef WINHEAP


#include <cruntime.h>
#include <malloc.h>
#include <winheap.h>
#include <oscalls.h>
#include <dbgint.h>

/***
*void *_expand(void *pblck, size_t newsize) - expand/contract a block of memory
*       in the heap
*
*Purpose:
*       Resizes a block in the heap to newsize bytes. newsize may be either
*       greater (expansion) or less (contraction) than the original size of
*       the block. The block is NOT moved.
*
*       NOTES:
*
*       (1) In this implementation, if the block cannot be grown to the
*       desired size, the resulting block will NOT be grown to the max
*       possible size.  (That is, either it works or it doesn't.)
*
*       (2) Unlike other implementations, you can NOT pass a previously
*       freed block to this routine and expect it to work.
*
*Entry:
*       void *pblck - pointer to block in the heap previously allocated
*                 by a call to malloc(), realloc() or _expand().
*
*       size_t newsize  - requested size for the resized block
*
*Exit:
*       Success:  Pointer to the resized memory block (i.e., pblck)
*       Failure:  NULL
*
*Uses:
*
*Exceptions:
*       If pblck does not point to a valid allocation block in the heap,
*       _expand() will behave unpredictably and probably corrupt the heap.
*
*******************************************************************************/

void * __cdecl _expand_base(
        void * pblock,
        size_t newsize
        )
{
        /* newsize == 0 bumped up to 1 */
        newsize = (newsize ? newsize : 1);


        /* validate size */
        if ( newsize > _HEAP_MAXREQ )
            newsize = _HEAP_MAXREQ;

        return (HeapReAlloc( _crtheap,
                 HEAP_REALLOC_IN_PLACE_ONLY,
                 pblock,
                 newsize
                   ));
}


#endif  /* WINHEAP */
