// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 


#include "afx.h"
#pragma hdrstop

#include <limits.h>
#define SIZE_T_MAX  UINT_MAX

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG       // entire file for debugging

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// forward

static void* AllocMemoryDebug(size_t nSize, BOOL bIsObject, 
	const char FAR* pFileName, int nLine);
static void FreeMemoryDebug(void* pbData, BOOL bIsObject);

/////////////////////////////////////////////////////////////////////////////
// test allocation routines

extern "C" int afxMemDF = allocMemDF;

void* operator new(size_t nSize)
{
	// memory corrupt before global new
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	void* p = AllocMemoryDebug(nSize, FALSE, NULL, 0);

	if (p == NULL)
	{
		TRACE("::operator new(%u) failed - throwing exception\n", nSize);
		AfxThrowMemoryException();
	}
	
	return p;
}

void* operator new(size_t nSize, const char FAR* pFileName, int nLine)
{
	// memory corrupt before global new
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	void* p = AllocMemoryDebug(nSize, FALSE, pFileName, nLine);
	
	if (p == NULL)
	{
		TRACE("::operator new(%u) failed - throwing exception\n", nSize);
		AfxThrowMemoryException();
	}

	return p;
}

void operator delete(void* pbData)
{
	// memory corrupt before global delete
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	FreeMemoryDebug(pbData, FALSE);
}

void* CObject::operator new(size_t nSize)
{
	// memory corrupt before global new
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	void* p = AllocMemoryDebug(nSize, TRUE, NULL, 0);
	
	if (p == NULL)
	{
		TRACE("CObject::operator new(%u) failed - throwing exception\n", nSize);
		AfxThrowMemoryException();
	}

	return p;
}

void* 
CObject::operator new(size_t nSize, const char FAR* pFileName, int nLine)
{
	// memory corrupt before 'CObject::new'
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	void* p = AllocMemoryDebug(nSize, TRUE, pFileName, nLine);
	
	if (p == NULL)
	{
		TRACE("CObject::operator new(%u) failed - throwing exception\n", nSize);
		AfxThrowMemoryException();
	}

	return p;
}

void CObject::operator delete(void* pbData)
{
	// memory corrupt before 'CObject::delete'
	if (afxMemDF & checkAlwaysMemDF)
		ASSERT(AfxCheckMemory()); 

	FreeMemoryDebug(pbData, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// allocation failure hook, tracking turn on

static BOOL bTrackingOn = TRUE;
static BOOL PASCAL DefaultAllocHook(size_t, BOOL, LONG)
	{ return TRUE; }

static AFX_ALLOC_HOOK pfnAllocHook = DefaultAllocHook;

AFX_ALLOC_HOOK AfxSetAllocHook(AFX_ALLOC_HOOK pfnNewHook)
{
	AFX_ALLOC_HOOK pfnOldHook = pfnAllocHook;
	pfnAllocHook = pfnNewHook;
	return pfnOldHook;
}

BOOL PASCAL AfxEnableMemoryTracking(BOOL bNewTrackingOn)
{
	BOOL bOldTrackingOn = bTrackingOn;
	bTrackingOn = bNewTrackingOn;
	return bOldTrackingOn;
}

/////////////////////////////////////////////////////////////////////////////
// stop on a specific memory request

static LONG lStopRequest = 0;
static AFX_ALLOC_HOOK pfnOldStopHook = NULL;

#pragma optimize("q", off)
extern "C" void PASCAL AfxStop()
{
	// set a breakpoint on this routine from debugger
	TRACE("AfxStop() stopping under the debugger\n");
#ifdef _NTWIN
	DebugBreak();
#else
	_asm { int 3 };
#endif
	TRACE("AfxStop() continues\n");
}
#pragma optimize("", on)

static BOOL PASCAL AfxTestAllocStop(size_t nSize, BOOL bIsObject, LONG lRequest)
{
	if (lRequest == lStopRequest)
	{
		TRACE("Allocating block # %ld\n", lRequest);
		AfxStop();
	}

	// otherwise just pass on to other hook
	return (*pfnOldStopHook)(nSize, bIsObject, lRequest);
}

void 
PASCAL AfxSetAllocStop(LONG lRequestNumber)
{
	if (pfnOldStopHook == NULL)
		pfnOldStopHook = AfxSetAllocHook(AfxTestAllocStop);

	lStopRequest = lRequestNumber;
}

/////////////////////////////////////////////////////////////////////////////
// AFX Memory Management diagnostics - malloc-like
//

// we keep statistics on what memory is/was used
static LONG lTotalAlloc;// total bytes of memory allocated 
static LONG lCurAlloc;  // current bytes of memory allocated 
static LONG lMaxAlloc;  // maximum bytes of memory allocated at any one time 

// we keep a request count to use in replaying memory consumption
static LONG lRequestLast = 0;
#define lNotTracked 0       // if not tracked 

// for diagnostic purpose, blocks are allocated with extra information and
//  stored in a doubly-linked list.  This makes all blocks registered with
//  how big they are, when they were allocated and what they are used for.

static struct CBlockHeader* pFirstBlock = NULL; // add in reverse order

//  A no-mans-land area is allocated before and after the actual data:
//      ---------
//          start of CBlockHeader pFirstBlocker (linkage and statistical info)
//          no man's land before actual data
//          app pointer-> actual data
//          no man's land after actual data
//      ---------

#define nNoMansLandSize     4       // # of bytes 

// The following values are non-zero, constant, odd, large, and atypical
//    Non-zero values help find bugs assuming zero filled data.
//    Constant values are good so that memory filling is deterministic
//        (to help make bugs reproducable).  Of course it is bad if
//        the contant filling of weird values masks a bug.
//    Mathematically odd numbers are good for finding bugs assuming a cleared
//        lower bit.
//    Large numbers (byte values at least) are less typical, and are good
//        at finding bad addresses.
//    Atypical values (i.e. not too often) are good since they typically
//        cause early detection in code.
//    For the case of no-man's land and free blocks, if you store to any
//         of these locations, the memory integrity checker will detect it.

#define bNoMansLandFill     0xFD    // fill no-man's land with this 
#define bDeadLandFill       0xDD    // fill free objects with this 
#define bCleanLandFill      0xCD    // fill new objects with this 

// three uses for registered blocks
static char* blockUseName[CMemoryState::nBlockUseMax] =
	{ "Free", "Object", "Non-Object" };

struct CBlockHeader 
{
	struct CBlockHeader* pBlockHeaderNext;
	struct CBlockHeader* pBlockHeaderPrev;
	const char FAR*     pFileName;
	int                 nLine;
	size_t              nDataSize;
	enum CMemoryState::blockUsage use;
	LONG                lRequest;
	BYTE                gap[nNoMansLandSize];
	// followed by:
	//  BYTE            data[nDataSize];
	//  BYTE            anotherGap[nNoMansLandSize];
	BYTE* pbData()
		{ return (BYTE*) (this + 1); }
};

static void* 
AllocMemoryDebug(size_t nSize, BOOL bIsObject, const char FAR* pFileName, int nLine)
// Allocate a memory block of the specific nSize with extra diagnostic
//      support (padding on either nSize of block + linkage)
// Mark it either as object (stores a non-primitive object) or just bits
{
	ASSERT(nSize > 0);

	LONG    lRequest;
	lRequest = bTrackingOn ? ++lRequestLast : lNotTracked;

	// forced failure
	if (!(*pfnAllocHook)(nSize, bIsObject, lRequest))
		return NULL;

	if (!(afxMemDF & allocMemDF))
		return malloc(nSize);

	// Diagnostic memory allocation from this point on
	if (nSize > (size_t)SIZE_T_MAX - nNoMansLandSize - sizeof(CBlockHeader))
	{
		TRACE("Error: memory allocation: tried to allocate %u bytes\n", nSize);
		TRACE("  object too large or negative size\n");
		AfxThrowMemoryException();
	}

	// keep track of total amount of memory allocated
	lTotalAlloc += nSize;
	lCurAlloc += nSize;

	if (lCurAlloc > lMaxAlloc)
		lMaxAlloc = lCurAlloc;
			
	register struct CBlockHeader* p = (struct CBlockHeader*)
	   malloc(sizeof(CBlockHeader) + nSize + nNoMansLandSize);

	if (p == NULL)
		return NULL;

	if (pFirstBlock)
		pFirstBlock->pBlockHeaderPrev = p;

	p->pBlockHeaderNext = pFirstBlock;
	p->pBlockHeaderPrev = NULL;
	p->pFileName = pFileName;
	p->nLine = nLine;
	p->nDataSize = nSize;
	p->use = bIsObject ? CMemoryState::objectBlock : CMemoryState::bitBlock;
	p->lRequest = lRequest;

	// fill in gap before and after real block 
	memset(p->gap, bNoMansLandFill, nNoMansLandSize);
	memset(p->pbData() + nSize, bNoMansLandFill, nNoMansLandSize);

	// fill data with silly value (but non-zero) 
	memset(p->pbData(), bCleanLandFill, nSize);

	// link blocks together
	pFirstBlock = p;
	return (void*)p->pbData();
}


// debugging free
static void
FreeMemoryDebug(void* pbData, BOOL bIsObject)
{
	if (pbData == NULL)
		return;

	if (!(afxMemDF & allocMemDF))
	{
		free(pbData);
		return;
	}

	register struct CBlockHeader* p = ((struct CBlockHeader*) pbData)-1;

	// make sure we are freeing what we think we are:
	ASSERT(p->use == (bIsObject ? CMemoryState::objectBlock 
		: CMemoryState::bitBlock));
		// error if freeing incorrect memory type

	// keep track of total amount of memory allocated
	lCurAlloc -= p->nDataSize;
	
	p->use = CMemoryState::freeBlock;
	// keep memory around as dead space
	memset(p->pbData(), bDeadLandFill, p->nDataSize);

	// optionally reclaim memory
	if (!(afxMemDF & delayFreeMemDF))
	{
		// remove from the linked list
		if (p->pBlockHeaderNext)
			p->pBlockHeaderNext->pBlockHeaderPrev = p->pBlockHeaderPrev;
		
		if (p->pBlockHeaderPrev)
		{
			p->pBlockHeaderPrev->pBlockHeaderNext = p->pBlockHeaderNext;
		}
		else
		{
			ASSERT(pFirstBlock == p);
			pFirstBlock = p->pBlockHeaderNext;
		}

		free(p);
	}
}

static BOOL
CheckBytes(register BYTE* pb, register WORD bCheck, register size_t nSize)
{
	BOOL bOkay = TRUE;
	while (nSize--)
	{
		if (*pb++ != bCheck)
		{
			TRACE("memory check error at $%08lX = $%02X, should be $%02X\n",
				(BYTE FAR*) (pb-1),*(pb-1), bCheck);
			bOkay = FALSE;
		}
	}
	return bOkay;
}


BOOL PASCAL
AfxCheckMemory()
  // check all of memory (look for memory tromps)
{
	if (!(afxMemDF & allocMemDF))
		return TRUE;        // can't do any checking

	BOOL    allOkay = TRUE;

	// check all allocated blocks
	register struct CBlockHeader* p;
	for (p = pFirstBlock; p != NULL; p = p->pBlockHeaderNext)
	{
		BOOL    okay = TRUE;        // this block okay ?
		char*   blockUse;

		if (p->use >= 0 && p->use < CMemoryState::nBlockUseMax)
			blockUse = blockUseName[p->use];
		else
			blockUse = "Damage";

		// first check no-mans-land gaps
		if (!CheckBytes(p->gap, bNoMansLandFill, nNoMansLandSize))
		{
			TRACE("DAMAGE: before %s block at $%08lX\n", blockUse,
				(BYTE FAR*) p->pbData());
			okay = FALSE;
		}

		if (!CheckBytes(p->pbData() + p->nDataSize, bNoMansLandFill,
		  nNoMansLandSize))
		{
			TRACE("DAMAGE: after %s block at $%08lX\n", blockUse,
				(BYTE FAR*) p->pbData());
			okay = FALSE;
		}

		// free blocks should remain undisturbed
		if (p->use == CMemoryState::freeBlock &&
		  !CheckBytes(p->pbData(), bDeadLandFill, p->nDataSize))
		{
			TRACE("DAMAGE: on top of Free block at $%08lX\n",
				(BYTE FAR*) p->pbData());
			okay = FALSE;
		}

		if (!okay)
		{
			// report some more statistics about the broken object

			if (p->pFileName != NULL)
				TRACE("%s allocated at file %Fs(%d)\n", blockUse, 
					p->pFileName, p->nLine);

			TRACE("%s located at $%08lX is %u bytes long\n", blockUse,
				(BYTE FAR*) p->pbData(), p->nDataSize);

			allOkay = FALSE;
		}
	}
	return allOkay;
}


// -- true if block of exact size, allocated on the heap
// -- set *plRequestNumber to request number (or 0)
BOOL 
PASCAL AfxIsMemoryBlock(const void* pData, UINT nSize,
		LONG* plRequestNumber)
{

	if (!(afxMemDF & allocMemDF))
	{
		// no tracking memory allocator
		if (plRequestNumber != NULL)
			*plRequestNumber = 0;
		return AfxIsValidAddress(pData, nSize); // the best we can do
	}

	// otherwise we can check to make sure this was allocated with tracking
	register struct CBlockHeader* p = ((struct CBlockHeader*) pData) - 1;

	if (AfxIsValidAddress(p, sizeof(CBlockHeader)) &&
		(p->use == CMemoryState::objectBlock ||
			p->use == CMemoryState::bitBlock) &&
		AfxIsValidAddress(pData, nSize) &&
		p->nDataSize == nSize)
	{
		if (plRequestNumber != NULL)
			*plRequestNumber = p->lRequest;
		return TRUE;
	}

	return FALSE;
}

// fills 'this' with the difference, returns TRUE if significant
BOOL CMemoryState::Difference(const CMemoryState& oldState,
		const CMemoryState& newState)
{
	BOOL bSignificantDifference = FALSE;
	for (int use = 0; use < CMemoryState::nBlockUseMax; use++)
	{
		m_lSizes[use] = newState.m_lSizes[use] - oldState.m_lSizes[use];
		m_lCounts[use] = newState.m_lCounts[use] - oldState.m_lCounts[use];

		if ((m_lSizes[use] != 0 || m_lCounts[use] != 0) &&
		  use != CMemoryState::freeBlock)
			bSignificantDifference = TRUE;
	}
	m_lHighWaterCount = newState.m_lHighWaterCount - oldState.m_lHighWaterCount;
	m_lTotalCount = newState.m_lTotalCount - oldState.m_lTotalCount;

	return bSignificantDifference;
}


void CMemoryState::DumpStatistics() const
{
	for (int use = 0; use < CMemoryState::nBlockUseMax; use++)
	{
		TRACE("%ld bytes in %ld %s Blocks\n", m_lSizes[use],
			m_lCounts[use], blockUseName[use]);
	}

	TRACE("Largest number used: %ld bytes\n", m_lHighWaterCount);
	TRACE("Total allocations: %ld bytes\n", m_lTotalCount);
}

// -- fill with current memory state
void CMemoryState::Checkpoint()
{
	if (!(afxMemDF & allocMemDF))
		return;     // can't do anything

	m_pBlockHeader = pFirstBlock;
	for (int use = 0; use < CMemoryState::nBlockUseMax; use++)
		m_lCounts[use] = m_lSizes[use] = 0;

	register struct CBlockHeader* p;
	for (p = pFirstBlock; p != NULL; p = p->pBlockHeaderNext)
	{
		if (p->lRequest == lNotTracked)
		{
			// ignore it for statistics
		}
		else if (p->use >= 0 && p->use < CMemoryState::nBlockUseMax)
		{
			m_lCounts[p->use]++;
			m_lSizes[p->use] += p->nDataSize;
		}
		else
		{
			TRACE("Bad memory block found at $%08lX\n", (BYTE FAR*) p);
		}
	}

	m_lHighWaterCount = lMaxAlloc;
	m_lTotalCount = lTotalAlloc;
}

// Dump objects created after this memory state was checkpointed
// Will dump all objects if this memory state wasn't checkpointed
// Dump all objects, report about non-objects also
// List request number in {}, {0} => not tracked
void CMemoryState::DumpAllObjectsSince() const
{
	if (!(afxMemDF & allocMemDF))
	{
		TRACE("Debugging allocator turned off, can't dump objects\n");
		return;
	}

	register struct CBlockHeader* pBlockStop;

	TRACE("Dumping objects ->\n");
	pBlockStop = m_pBlockHeader;

	register struct CBlockHeader* p;
	for (p = pFirstBlock; p != NULL && p != pBlockStop;
		p = p->pBlockHeaderNext)
	{
		char sz[255];

		if (p->lRequest == lNotTracked)
		{
			// ignore it for dumping
		}
		else if (p->use == CMemoryState::objectBlock)
		{
			CObject* pObject = (CObject*) p->pbData();

			TRACE("{%ld} ", p->lRequest);
			if (p->pFileName != NULL)
			{
				sprintf(sz, "%Fs(%d) : ", p->pFileName, p->nLine);
				afxDump << (const char FAR*)sz;
			}


			pObject->Dump(afxDump);
			afxDump << "\n";
		}
		else if (p->use == CMemoryState::bitBlock)
		{
			TRACE("{%ld} ", p->lRequest);
			if (p->pFileName != NULL)
			{
				sprintf(sz, "%Fs(%d) : ", p->pFileName, p->nLine);
				afxDump << (const char FAR*)sz;
			}

			sprintf(sz, "non-object block at $%08lX, %u bytes long\n",
				(BYTE FAR*) p->pbData(), p->nDataSize);
			afxDump << (const char FAR*)sz;
		}
	}
	TRACE("Object dump complete.\n");
}

/////////////////////////////////////////////////////////////////////////////
// Enumerate all objects allocated in the diagnostic memory heap

void 
PASCAL AfxDoForAllObjects(void (*pfn)(CObject*, void*), void* pContext)
{
	if (!(afxMemDF & allocMemDF))
		return;         // sorry not enabled

	register struct CBlockHeader* p;
	for (p = pFirstBlock; p != NULL; p = p->pBlockHeaderNext)
	{
		if (p->lRequest == lNotTracked)
		{
			// ignore it for iteration
		}
		else if (p->use == CMemoryState::objectBlock)
		{
			CObject* pObject = (CObject*) p->pbData();
			(*pfn)(pObject, pContext);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////


#endif //_DEBUG (entire file)
