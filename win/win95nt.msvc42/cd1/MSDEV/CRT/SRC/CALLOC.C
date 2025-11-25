/***
*calloc.c - allocate storage for an array from the heap
*
*       Copyright (c) 1989-1996, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the calloc() function.
*
*******************************************************************************/

#ifdef WINHEAP

#include <malloc.h>
#include <string.h>
#include <winheap.h>
#include <windows.h>
#include <internal.h>
#include <mtdll.h>
#include <dbgint.h>

/***
*void *calloc(size_t num, size_t size) - allocate storage for an array from
*       the heap
*
*Purpose:
*       Allocate a block of memory from heap big enough for an array of num
*       elements of size bytes each, initialize all bytes in the block to 0
*       and return a pointer to it.
*
*Entry:
*       size_t num  - number of elements in the array
*       size_t size - size of each element
*
*Exit:
*       Success:  void pointer to allocated block
*       Failure:  NULL
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

void * __cdecl _calloc_base (
    size_t num,
    size_t size
    )
{
        void * retp;

        size *= num;


        /* round up to the nearest paragraph */
        if ( size <= _HEAP_MAXREQ )
            if ( size > 0 )
                size = (size + _PARASIZE - 1) & ~(_PARASIZE - 1);
            else
                size = _PARASIZE;

        for (;;) {

            retp = NULL;

            if ( size <= _HEAP_MAXREQ ) {
                if ( size <= __sbh_threshold ) {
                    /*
                     * Allocate the block from the small-block heap and
                     * initialize it with zeros.
                     */
                    _mlock(_HEAP_LOCK);
                    retp = __sbh_alloc_block(size >> _PARASHIFT);
                    _munlock(_HEAP_LOCK);
                    if ( retp != NULL )
                        memset(retp, 0, size);
                }

                if ( retp == NULL )
                    retp = HeapAlloc( _crtheap,
                                      HEAP_ZERO_MEMORY,
                                      size );
            }


            if ( retp || _newmode == 0)
                return retp;

            /* call installed new handler */
            if (!_callnewh(size))
                return NULL;

            /* new handler was successful -- try to allocate again */
        }

}


#else  /* WINHEAP */


#include <cruntime.h>
#include <heap.h>
#include <malloc.h>
#include <mtdll.h>
#include <stddef.h>
#include <dbgint.h>

/***
*void *calloc(size_t num, size_t size) - allocate storage for an array from
*       the heap
*
*Purpose:
*       Allocate a block of memory from heap big enough for an array of num
*       elements of size bytes each, initialize all bytes in the block to 0
*       and return a pointer to it.
*
*Entry:
*       size_t num  - number of elements in the array
*       size_t size - size of each element
*
*Exit:
*       Success:  void pointer to allocated block block
*       Failure:  NULL
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

void * __cdecl _calloc_base (
        size_t num,
        size_t size
        )
{
        void *retp;
        REG1 size_t *startptr;
        REG2 size_t *lastptr;

        /* try to malloc the requested space
         */
        retp = _malloc_base(size *= num);

        /* if malloc() succeeded, initialize the allocated space to zeros.
         * note the assumptions that the size of the allocation block is an
         * integral number of sizeof(size_t) bytes and that (size_t)0 is
         * sizeof(size_t) bytes of 0.
         */
        if ( retp != NULL ) {
            startptr = (size_t *)retp;
            lastptr = startptr + ((size + sizeof(size_t) - 1) /
            sizeof(size_t));
            while ( startptr < lastptr )
                *(startptr++) = 0;
        }

        return retp;
}

#endif  /* WINHEAP */
