#include "stdafx.h"
#include "Data.h"
#include "Entity.h"
#include "Resultset.h"

// Since heap work in a multi-threaded and in particular multi-processor
// situation is very sensitive in terms of serialisation.
// The heap work that reflects the use of the C++ new/delete operators has
// been separated into this implementation file.

// Also a fixed allocator system has been implemented to provide a reduced
// heap access mechanism for frequently allocated objects.


HANDLE g_hGeneralHeap = HeapCreate(0, sizeof(CResultset) * 5, 0);

void* __cdecl operator new(size_t nSize)
{	
	return HeapAlloc(g_hGeneralHeap, 0, nSize);
}

void __cdecl operator delete(void* p)
{
	HeapFree(g_hGeneralHeap, 0, p);
}

// This is the MFC fixed allocator implementation.

CFixedAlloc::CFixedAlloc(UINT nAllocSize, UINT nBlockSize)
{
	m_nAllocSize = nAllocSize;
	m_nBlockSize = nBlockSize;
	m_pNodeFree = NULL;
	m_pBlocks = NULL;
	InitializeCriticalSection(&m_protect);
}

CFixedAlloc::~CFixedAlloc()
{
	FreeAll();
	DeleteCriticalSection(&m_protect);
}

void CFixedAlloc::FreeAll()
{
	EnterCriticalSection(&m_protect);
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
	m_pNodeFree = NULL;
	LeaveCriticalSection(&m_protect);
}


///////////////////////////////////////////////////////////////////////////


CPlex* PASCAL CPlex::Create(CPlex*& pHead, UINT nMax, UINT cbElement)
{
	CPlex* p = (CPlex*) new BYTE[sizeof(CPlex) + nMax * cbElement];
			// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void CPlex::FreeDataChain()     // free this one and links
{
	CPlex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		CPlex* pNext = p->pNext;
		delete[] bytes;
		p = pNext;
	}
}
