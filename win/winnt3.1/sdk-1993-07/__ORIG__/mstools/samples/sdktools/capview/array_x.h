typedef struct {
    int         left;
    int         y;
    int         width;
} AAA;

////////////////////////////////////////////////////////////////////////////


class CAaaArray : public CObject
{

	DECLARE_DYNAMIC(CAaaArray)
public:

// Construction
	CAaaArray();

// Attributes
	int     GetSize() const
				{ return m_nSize; }
	int     GetUpperBound() const
				{ return m_nSize-1; }
	void    SetSize(int nNewSize, int nGrowBy = -1);

// Operations
	// Clean up
	void    FreeExtra();
	void    RemoveAll()
				{ SetSize(0); }

	// Accessing elements
	AAA    GetAt(int nIndex) const
				{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
					return m_pData[nIndex]; }
	void    SetAt(int nIndex, AAA newElement)
				{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
					m_pData[nIndex] = newElement; }
	AAA&   ElementAt(int nIndex)
				{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
					return m_pData[nIndex]; }

	// Potentially growing the array
	void    SetAtGrow(int nIndex, AAA newElement);
	int     Add(AAA newElement)
				{ int nIndex = m_nSize;
					SetAtGrow(nIndex, newElement);
					return nIndex; }

	// overloaded operator helpers
	AAA    operator[](int nIndex) const
				{ return GetAt(nIndex); }
	AAA&   operator[](int nIndex)
				{ return ElementAt(nIndex); }

	// Operations that move elements around
	void    InsertAt(int nIndex, AAA newElement, int nCount = 1);
	void    RemoveAt(int nIndex, int nCount = 1);
	void    InsertAt(int nStartIndex, CAaaArray* pNewArray);

// Implementation
protected:
	AAA*   m_pData;        // the actual array of data
	int     m_nSize;        // # of elements (upperBound - 1)
	int     m_nMaxSize;     // max allocated
	int     m_nGrowBy;      // grow amount

public:
	~CAaaArray();
#if 0
#ifdef _DEBUG
	void    Dump(CDumpContext&) const;
	void    AssertValid() const;
#endif
#endif // 0
};

