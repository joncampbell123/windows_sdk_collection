/*
 -  P V A L L O C . C
 -
 *  Copyright (C) 1994 Microsoft Corporation
 *  Purpose:
 *      Implementation of a chained memory manager.
 *
 */

#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <pvalloc.h>

#ifdef _DEBUG
static CB       cbTotalAlloc    = 0;
static CB       ulTotalBlockNum = 0;
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
    PV      lpv         = pvNull;
    HANDLE  hMem;
    PPVINFO ppvinfo;
#ifdef _DEBUG
    char    szFileName[80];
    LPSTR   lpszTemp    = NULL;
    FILE    *pFile      = NULL;
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
        ulTotalBlockNum++;
        ppvinfo->ulBlockNum = ulTotalBlockNum;
        cbTotalAlloc += cbSize;
        
        // log to file
        lpszTemp = getenv("TEMP");

        if(lpszTemp)
            strcpy(szFileName, lpszTemp);
        else
            strcpy(szFileName, "c:\\temp");

        strcat(szFileName, "\\pvalloc.log");

        
        pFile = fopen(szFileName,"a");
        if (pFile == NULL)     
            goto NoFile;      
//           return NULL;

        fprintf(pFile, "Block: \t%lu\tPvAlloc: %ld Bytes\t\tTotal: %ld Bytes\n",
                 ulTotalBlockNum, cbSize, cbTotalAlloc);

        if (pFile)
            fclose(pFile);
        
        // log to comm port
        wsprintf(szBuff,"Block: \t%lu\tPvAlloc: %ld Bytes\t\tTotal: %ld Bytes\n",
                 ulTotalBlockNum, cbSize, cbTotalAlloc);
        OutputDebugString(szBuff);
                        
NoFile:                           

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
    PV          lpvStep = lpvParent;
    PV          lpv     = pvNull;
    PPVINFO     ppvinfoMore;
    HANDLE      hMem;
    PPVINFO     ppvinfo;

    /* Step to the last link */
    do
    {
        ppvinfoMore = (PPVINFO)(((PB)lpvStep) - sizeof(PVINFO));
        lpvStep = ppvinfoMore->lpvNext;
    }
    while(ppvinfoMore->lpvNext != pvNull);

    // beginning of section that was taken from PvAlloc

    if(cbSize < 4)
        cbSize = 4;
    else if(cbSize & 3)
        cbSize += 4 - (cbSize & 3);


    hMem = GlobalAlloc(GMEM_MOVEABLE, cbSize + sizeof(PVINFO));
    if(hMem)
    {
        ppvinfo = (PPVINFO)GlobalLock(hMem);
        ppvinfo->hMem       = hMem;
        ppvinfo->lpvNext    = pvNull;
        ppvinfo->lpvBuf     = ((PB)ppvinfo) + sizeof(PVINFO);
#ifdef _DEBUG
        ppvinfo->cbSize     = cbSize;
        ppvinfo->ulBlockNum = ppvinfoMore->ulBlockNum;
        cbTotalAlloc += cbSize;

#ifdef WIN32
        memset(ppvinfo->lpvBuf, 0xaa, (size_t)cbSize);
#else
        _fmemset(ppvinfo->lpvBuf, 0xaa, (size_t)cbSize);
#endif

#endif
        lpv = ppvinfo->lpvBuf;
    }
    else
        return lpv;
        
    // end of section taken from pvalloc

#ifdef WIN32
        memset(lpv, 0xbb, (size_t)cbSize);
#else
        _fmemset(lpv, 0xbb, (size_t)cbSize);
#endif  /* WIN32 */

    ppvinfoMore->lpvNext = lpv;

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
    CB      cbSize;
    CB      ulBlockNum;
    FILE    *pFile  = NULL;
    CB      cbFree  = 0;
    CB      cbTotalBeforeFree = cbTotalAlloc;
    char    szFileName[80];
    LPSTR   lpszTemp    = NULL;
    char    szBuff[128];
#endif

    if(!lpv)
        return 0;

    ppvinfo = (PPVINFO)(((PB)lpv) - sizeof(PVINFO));

    while(ppvinfo)
    {
        lpv = ppvinfo->lpvNext;

#ifdef _DEBUG
        cbSize      = ppvinfo->cbSize;
        cbFree      += ppvinfo->cbSize;
        ulBlockNum  = ppvinfo->ulBlockNum;

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
#endif

        if(lpv)
            ppvinfo = (PPVINFO)(((PB)lpv) - sizeof(PVINFO));
        else
            break;
    }


#ifdef _DEBUG
    
    if((cbTotalBeforeFree - cbTotalAlloc) != cbFree)
       goto err;
       
    // log to file
    lpszTemp = getenv("TEMP");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\temp");

    strcat(szFileName, "\\pvalloc.log");
        
    pFile = fopen(szFileName,"a");
       
    if (pFile == NULL)
       goto err;

    fprintf(pFile, "Block: \t%lu\t\t***PvFree***,  Freeing  %lu Bytes(Alloc and AllocMore)\tUnFreed: %ld Bytes\n",
                    ulBlockNum, cbFree, cbTotalAlloc);
    if (pFile)
        fclose(pFile);

     // log to comm port
    wsprintf(szBuff,"Block: \t%lu\t\t***PvFree***,  Freeing  %lu Bytes(Alloc and AllocMore)\tUnFreed: %ld Bytes\n",
                    ulBlockNum, cbFree, cbTotalAlloc);
    OutputDebugString(szBuff);

#endif  /* _DEBUG */

    return 0; // Success!

err:
#ifdef _DEBUG

    // find file to open
    lpszTemp = getenv("TEMP");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\temp");

    strcat(szFileName, "\\pvalloc.log");

        
    pFile = fopen(szFileName,"a");

    if (pFile == NULL)
       return 1;

    fprintf(pFile, "Block: %lu Failure freeing: %ld Bytes\tUnFreed: %ld Bytes\n",
             ulBlockNum, cbSize, cbTotalAlloc);
    if (pFile)
        fclose(pFile);

#endif  /* _DEBUG */

    return 1; // Failure!
}
