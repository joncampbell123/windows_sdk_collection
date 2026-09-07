#ifndef __HEAP_H_
#define __HEAP_H_

// fixalloc.h - declarations for fixed block allocator

struct CPlex     // warning variable length structure
{
	CPlex* pNext;
	DWORD dwReserved[1];    // align on 8 byte boundary
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& head, UINT nMax, UINT cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

/////////////////////////////////////////////////////////////////////////////
// CFixedAlloc

class CFixedAlloc
{
// Constructors
public:
	CFixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);

// Attributes
	UINT GetAllocSize() { return m_nAllocSize; }

// Operations
public:
	inline void* Alloc()// return a chunk of memory of nAllocSize
	{
		EnterCriticalSection(&m_protect);
		if (m_pNodeFree == NULL)
		{
			CPlex* pNewBlock;
			// add another block
			pNewBlock = CPlex::Create(m_pBlocks, m_nBlockSize, m_nAllocSize);

			// chain them into free list
			CNode* pNode = (CNode*)pNewBlock->data();
			// free in reverse order to make it easier to debug
			(BYTE*&)pNode += (m_nAllocSize * m_nBlockSize) - m_nAllocSize;
			for (int i = m_nBlockSize-1; i >= 0; i--, (BYTE*&)pNode -= m_nAllocSize)
			{
				pNode->pNext = m_pNodeFree;
				m_pNodeFree = pNode;
			}
		}

		// remove the first available node from the free list
		void* pNode = m_pNodeFree;
		m_pNodeFree = m_pNodeFree->pNext;

		LeaveCriticalSection(&m_protect);
		return pNode;
	}

	inline void Free(void* p) // free chunk of memory returned from Alloc
	{
		if (p != NULL)
		{
			EnterCriticalSection(&m_protect);

			// simply return the node to the free list
			CNode* pNode = (CNode*)p;
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
			LeaveCriticalSection(&m_protect);
		}
	}
	void FreeAll(); // free everything allocated from this allocator

// Implementation
public:
	~CFixedAlloc();

protected:
	struct CNode
	{
		CNode* pNext;   // only valid when in free list
	};

	UINT m_nAllocSize;  // size of each block from Alloc
	UINT m_nBlockSize;  // number of blocks to get at a time
	CPlex* m_pBlocks;   // linked list of blocks (is nBlocks*nAllocSize)
	CNode* m_pNodeFree; // first free node (NULL if no free nodes)
	CRITICAL_SECTION m_protect;
};

// DECLARE_FIXED_ALLOC -- used in class definition
#define DECLARE_FIXED_ALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		return s_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		return s_alloc.Alloc(); \
	} \
protected: \
	static CFixedAlloc s_alloc \


// IMPLEMENT_FIXED_ALLOC -- used in class implementation file
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) \
CFixedAlloc class_name::s_alloc(sizeof(class_name), block_size) \


void* __cdecl operator new(size_t nSize);
void __cdecl operator delete(void* p);

#endif
