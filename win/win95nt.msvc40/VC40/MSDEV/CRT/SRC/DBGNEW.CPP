/***
*dbgnew.cpp - defines C++ new/delete routines, debug version
*
*       Copyright (c) 1995, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines C++ new/delete routines.
*
*******************************************************************************/

#ifdef _DEBUG

#include <cruntime.h>
#include <malloc.h>
#include <mtdll.h>
#include <dbgint.h>

/***
*void * operator new() - Get a block of memory from the debug heap
*
*Purpose:
*       Allocate of block of memory of at least size bytes from the heap and
*       return a pointer to it.
*
*       Allocates any type of supported memory block.
*
*Entry:
*       unsigned int    cb          - count of bytes requested
*       int             nBlockUse   - block type
*       char *          szFileName  - file name
*       int             nLine       - line number
*
*Exit:
*       Success:  Pointer to memory block
*       Failure:  NULL (or some error value)
*
*Exceptions:
*
*******************************************************************************/
void * operator new(
        unsigned int cb,
        int nBlockUse,
        const char * szFileName,
        int nLine
        )
{
        return _nh_malloc_dbg( cb, 1, nBlockUse, szFileName, nLine );
}

/***
*void operator delete() - delete a block in the debug heap
*
*Purpose:
*       Deletes any type of block.
*
*Entry:
*       void *pUserData - pointer to a (user portion) of memory block in the
*                         debug heap
*
*Return:
*       <void>
*
*******************************************************************************/
void operator delete(
        void *pUserData
        )
{
        _CrtMemBlockHeader * pHead;

        if (pUserData == NULL)
            return;

        _mlock(_HEAP_LOCK);  /* block other threads */

        /* get a pointer to memory block header */
        pHead = pHdr(pUserData);

         /* verify block type */
        _ASSERTE(_BLOCK_TYPE_IS_VALID(pHead->nBlockUse));

        _free_dbg( pUserData, pHead->nBlockUse );

        _munlock(_HEAP_LOCK);  /* release other threads */

        return;
}

#endif  /* _DEBUG */
