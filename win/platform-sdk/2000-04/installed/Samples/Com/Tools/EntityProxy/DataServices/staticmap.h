// intended for small number strings
class CStaticMap
{
public:
	int m_nSize;
	mutable CString* m_aKey;

// Construction/destruction
	CStaticMap() : m_nSize(0)
	{
		m_aKey = NULL;
	}

	void ClearArray()
	{
		if (m_aKey != NULL)
		{
			delete [] m_aKey;
			m_aKey = NULL;
		}
	}

	~CStaticMap()
	{
		RemoveAll();
		ClearArray();
	}

	void Initialize(long Size)
	{
		ClearArray();

		m_aKey = new CString[Size + 1];
	}

// Operations
	int GetSize() const
	{
		return m_nSize;
	}
	BOOL Add(LPCWSTR key, long len, long val)
	{
		if (val >= m_nSize)
		{
			m_nSize = val+1;
		}
		SetAtIndex(val, key, len);
		return TRUE;
	}
	BOOL AddNoLen(LPCWSTR key, long val)
	{
		if (val >= m_nSize)
		{
			m_nSize = val+1;
		}
		SetAtIndexNoLen(val, key);
		return TRUE;
	}
	void RemoveAll()
	{
		m_nSize = 0;
	}
	long Lookup(LPCWSTR key) const
	{
		int nIndex = FindKey(key);
		return nIndex;
	}
	BOOL SetKey(long val, WCHAR* key) const
	{
		m_aKey[val] = key;
		return TRUE;
	}

// Implementation

	void SetAtIndexNoLen(int nIndex, LPCWSTR key)
	{
		ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_aKey[nIndex] = key;
	}

	void SetAtIndex(int nIndex, LPCWSTR key, long len)
	{
		ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
		
		m_aKey[nIndex].AssignCopy(len, key);
	}
	int FindKey(LPCWSTR key) const
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(m_aKey[i] == key)
				return i;
		}
		return -1;  // not found
	}
};
