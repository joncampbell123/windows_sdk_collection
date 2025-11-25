#ifndef __DBLIST_H__
#define __DBLIST_H__

#include "debug.h"

#ifndef __cplusplus
#error DBList.h included from a C file!
#endif

template<class A>
class DblNode
{
friend class DbLinkedList<A>;

private:
    class DblNode<A>    *pNext;
    class DblNode<A>    *pPrev;

    A       value;
};

template <class T>
class DbLinkedList
{
private:

public:
    DbLinkedList();
    DbLinkedList( T emptyElt );
    ~DbLinkedList();

    inline void SetEmpty( T emptyElt ) { m_emptyT = emptyElt; }

    inline BOOL IsAtTail()  { ASSERT( NULL != m_pCurrent );
                    return (m_pCurrent->pNext == NULL); }

    void Clear( void );
    void RemoveCurrent();
    void Remove( T );
    BOOL InsertAfterCurrent( T val );
    BOOL InsertBeforeCurrent( T val );
    T GetCurrent();
    void Append( T );

    inline void SetAtHead()     { m_pCurrent = m_pHead; }
    inline void SetAtTail()     { m_pCurrent = m_pTail; }

    void AssertValid();

    inline int GetElementCount( void )  { return m_nElementCount; }

    DbLinkedList<T> &operator++()
        { if( m_pCurrent && m_pCurrent->pNext ) m_pCurrent = m_pCurrent->pNext; return *this; }
    DbLinkedList<T> &operator++( int )
        { if( m_pCurrent && m_pCurrent->pNext ) m_pCurrent = m_pCurrent->pNext; return *this; }
    DbLinkedList<T> &operator--()
        { if( m_pCurrent && m_pCurrent->pPrev ) m_pCurrent = m_pCurrent->pPrev; return *this; }
    DbLinkedList<T> &operator--( int )
        { if( m_pCurrent && m_pCurrent->pPrev ) m_pCurrent = m_pCurrent->pPrev; return *this; }


protected:
    DblNode<T>  *m_pHead;
    DblNode<T>  *m_pTail;
    DblNode<T>  *m_pCurrent;

    int m_nElementCount;
    T   m_emptyT;
};


// Because of the way templates work, we must include the code from the
// header file.
#include "DbList.cpp"


#endif

