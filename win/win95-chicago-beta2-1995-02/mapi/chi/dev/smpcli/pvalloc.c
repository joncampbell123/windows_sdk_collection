/*
 -  P V A L L O C . C
 -
 *  Purpose:
 *      Implementation of a chained memory manager.
 *
 *  Copyright 1993-1995 Microsoft Corporation. All Rights Reserved.
 */

#include <string.h>
#include <windows.h>
#include "pvalloc.h"

#ifdef _DEBUG
static CB cbTotalAlloc = 0;
#endif

/*
 -  PvAlloc
 -
 *  Purpose:
 *      Allocates a chunk of memory on the global heap.
 *
 *  Parameters:
 *      cbSize          - Count of bytes requested.
 *
 *  Returns:
 *      lpv             - Pointer to the allocated memory
 *
 */

PV PvAlloc(CB cbSize)
{
    PV      lpv = pvNull;
    HANDLE  hMem;
    PPVINFO ppvinfo;
#ifdef _DEBUG
    char    szBuff[128];
#endif

    /* Make sure allocations are in multiples of 4 */

    if(cbSize < 4)
        cbSize = 4;
    else if(cbSize & 3)
        cbSize += 4 - (cbSize & 3);

    /* Allocate the block */

    hMem = GlobalAlloc(GMEM_MOVEABLE, cbSize + sizeof(PVINFO));
    if(hMem)
    {
        ppvinfo = (PPVINFO)GlobalLock(hMem);
        ppvinfo->hMem    = hMem;
        ppvinfo->lpvNext = pvNull;
        ppvinfo->lpvBuf  = ((PB)ppvinfo) + sizeof(PVINFO);
#ifdef _DEBUG
        ppvinfo->cbSize  = cbSize;
        cbTotalAlloc += cbSize;
        wsprintf(szBuff, "Allocating: %ld Bytes\tTotal: %ld Bytes\r\n",
                 cbSize, cbTotalAlloc);
        OutputDebugString(szBuff);

#ifdef WIN32
        memset(ppvinfo->lpvBuf, 0xaa, (size_t)cbSize);
#else
        _fmemset(ppvinfo->lpvBuf, 0xaa, (size_t)cbSize);
#endif  /* WIN32 */

#endif  /* _DEBUG */
        lpv = ppvinfo->lpvBuf;
    }

    return lpv;
}



/*
 -  PvAllocMore
 -
 *  Purpose:
 *      Allocates a chunk of memory and chains it to a parent block.
 *
 *  Parameters:
 *      cbSize          - Count of additional bytes to allocate
 *      lpvParent       - Pointer to parent in memory chain
 *
 *  Returns:
 *      lpv             - Pointer to the allocated memory
 *
 */

PV PvAllocMore(CB cbSize, PV lpvParent)
{
    PV       lpvStep = lpvParent;
    PV       lpv     = pvNull;
    PPVINFO  ppvinfo;

    /* Step to the last link */
    do
    {
        ppvinfo = (PPVINFO)(((PB)lpvStep) - sizeof(PVINFO));
        lpvStep = ppvinfo->lpvNext;
    }
    while(ppvinfo->lpvNext != pvNull);

    lpv = PvAlloc(cbSize);

#ifdef WIN32
        memset(lpv, 0xbb, (size_t)cbSize);
#else
        _fmemset(lpv, 0xbb, (size_t)cbSize);
#endif  /* WIN32 */

    ppvinfo->lpvNext = lpv;

    return lpv;
}


/*
 -  PvFree
 -
 *  Purpose:
 *      This function frees memory allocated by PvAlloc or PvAllocMore.
 *      After the call, the pointer memory will be invalid and should
 *      not be referenced again.
 *      When memory is allocated by PvAlloc and PvAllocMore, which can
 *      contain several levels of pointers, all the application needs to
 *      do to free the entire structure is call this routine with the
 *      base pointer returned by the PvAlloc call.
 *
 *  Parameters:
 *      lpv             - Pointer to memory to be freed.
 *
 *  Returns:
 *      Void
 *
 */

BOOL PvFree(PV lpv)
{
    PPVINFO ppvinfo;
#ifdef _DEBUG
    char    szBuff[128];
    CB      cbSize;
#endif

    if(!lpv)
        return 0;

    ppvinfo = (PPVINFO)(((PB)lpv) - sizeof(PVINFO));

    while(ppvinfo)
    {
        lpv = ppvinfo->lpvNext;

#ifdef _DEBUG
        cbSize = ppvinfo->cbSize;

#ifdef WIN32
        memset(ppvinfo->lpvBuf, 0xcc, (size_t)ppvinfo->cbSize);
#else
        _fmemset(ppvinfo->lpvBuf, 0xcc, (size_t)ppvinfo->cbSize);
#endif  /* WIN32 */

#endif  /* _DEBUG */

        if(GlobalUnlock(ppvinfo->hMem))
            goto err;  // Our lock count is non-zero

        if(GlobalFree(ppvinfo->hMem))
            goto err;  // Failure

#ifdef _DEBUG
        cbTotalAlloc -= cbSize;
        wsprintf(szBuff, "Freeing: %ld Bytes\tUnFreed: %ld Bytes\r\n",
                 cbSize, cbTotalAlloc);
        OutputDebugString(szBuff);
#endif  /* _DEBUG */

        if(lpv)
            ppvinfo = (PPVINFO)(((PB)lpv) - sizeof(PVINFO));
        else
            break;
    }

    return 0; // Success!

err:
#ifdef _DEBUG
    wsprintf(szBuff, "Failure freeing: %ld Bytes\tUnFreed: %ld Bytes\r\n",
             cbSize, cbTotalAlloc);
    OutputDebugString(szBuff);
#endif  /* _DEBUG */

    return 1; // Failure!
}
