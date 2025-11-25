#include <windows.h>
#include "debug.h"

template<class T>
DbLinkedList<T>::DbLinkedList()
{
    m_pTail = m_pHead = m_pCurrent = NULL;
}


template<class T>
DbLinkedList<T>::DbLinkedList( T emptyElt )
{
    m_pTail = m_pHead = m_pCurrent = NULL;
    m_emptyT = emptyElt;
}


template<class T>
DbLinkedList<T>::~DbLinkedList()
{
    Clear();
}


template<class T>
void DbLinkedList<T>::Clear( void )
{
    DblNode<T>  *pTemp;

    while( m_nElementCount )
    {
    pTemp = m_pHead->pNext;
    delete m_pHead;
    m_pHead = pTemp;
    m_nElementCount--;
    }

    m_pHead = m_pTail = m_pCurrent = NULL;
}


template<class T>
void DbLinkedList<T>::Remove( T val )
{
    DblNode<T>  *pTemp = m_pHead;

    while( pTemp )
    {
    if( pTemp->value == val )
    {
        if( pTemp->pNext )
            pTemp->pNext->pPrev = pTemp->pPrev;
        if( pTemp->pPrev )
            pTemp->pPrev->pNext = pTemp->pNext;
        if( m_pHead == pTemp )
            m_pHead = pTemp->pNext;
        if( m_pTail == pTemp )
            m_pTail = pTemp->pPrev;

        delete pTemp;

        m_nElementCount--;
        pTemp = NULL;
        return;
    }

    pTemp = pTemp->pNext;
    }
}

template<class T>
void DbLinkedList<T>::RemoveCurrent()
{
    if( m_pCurrent )
    {
    ASSERT( m_nElementCount > 0 );
    
    if( m_pCurrent->pNext )
        m_pCurrent->pNext->pPrev = m_pCurrent->pPrev;
    if( m_pCurrent->pPrev )
        m_pCurrent->pPrev->pNext = m_pCurrent->pNext;
    if( m_pHead == m_pCurrent )
        m_pHead = m_pCurrent->pNext;
    if( m_pTail == m_pCurrent )
        m_pTail = m_pCurrent->pPrev;

    delete m_pCurrent;

    m_nElementCount--;
    m_pCurrent = NULL;
    }
}

template<class T>
T DbLinkedList<T>::GetCurrent()
{
    if( m_pCurrent )
    {
    ASSERT( m_nElementCount > 0 );
    
    return m_pCurrent->value;
    }
    return m_emptyT;
}

// TRUE == success, FALSE == failure
template<class T>
BOOL DbLinkedList<T>::InsertAfterCurrent( T val )
{
    DbNode<T> *pTemp = new DbNode<T>;

    if( NULL == pTemp )
    return FALSE;

    if( NULL == m_pCurrent )
    {
    AssertValid();
    if( NULL != m_pHead )
       {
        pTemp->value = val;
        pTemp->pPrev = m_pHead;
        pTemp->pNext = m_pHead->pNext;
        if( m_pHead->pNext )
            m_pHead->pNext->pPrev = pTemp;
        if( m_pTail == m_pHead )
            m_pTail = pTemp;
       }
    } else {
    pTemp->value = val;
    pTemp->pPrev = m_pCurrent;
    pTemp->pNext = m_pCurrent->pNext;
    if( m_pCurrent->pNext )
        m_pCurrent->pNext->pPrev = pTemp;
    // If we were pointing at the tail, we are no longer pointing
    // at it because we put a new element after the current one.
    if( m_pTail == m_pCurrent )
        m_pTail = pTemp;
    }

    return TRUE;
}


// TRUE == success, FALSE == failure
template<class T>
BOOL DbLinkedList<T>::InsertBeforeCurrent( T val )
{
    DbNode<T> *pTemp = new DbNode<T>;

    if( NULL == pTemp )
    return FALSE;

    if( NULL == m_pCurrent )
    {
    AssertValid();
    if( NULL != m_pHead )
       {
        pTemp->value = val;
        pTemp->pPrev = m_pHead;
        pTemp->pNext = m_pHead->pNext;
        if( m_pHead->pNext )
            m_pHead->pNext->pPrev = pTemp;
        if( m_pTail == m_pHead )
            m_pTail = pTemp;
       }
    } else {
    pTemp->value = val;
    pTemp->pPrev = m_pCurrent;
    pTemp->pNext = m_pCurrent->pNext;
    if( m_pCurrent->pNext )
        m_pCurrent->pNext->pPrev = pTemp;
    // If we were pointing at the tail, we are no longer pointing
    // at it because we put a new element after the current one.
    if( m_pTail == m_pCurrent )
        m_pTail = pTemp;
    }

    return TRUE;
}


template<class T>
void DbLinkedList<T>::Append( T val )
{
    DblNode<T> *pNode = new DblNode<T>;

    if( NULL == pNode )
    return;

    AssertValid();
    
    pNode->value = val;
    pNode->pNext = NULL;
    pNode->pPrev = m_pTail;

    if( m_pTail )
    m_pTail->pNext = pNode;
    if( !m_pHead )
    m_pHead = pNode;
    m_pTail = pNode;
    m_nElementCount++;

    // If there were no elements in the list, the m_pCurrent will be NULL
    if( NULL == m_pCurrent )
    {
    ASSERT( m_nElementCount == 1 );
    m_pCurrent = m_pHead;
    }
}


// Do some assertions that determine whether or not this object is in a valid
// state.  If it's not, one of these will fire.
template<class T>
void DbLinkedList<T>::AssertValid( void )
{
    if( m_nElementCount )
    {
    // If there are elements, then both these pointers should point
    // at a legitimate DbNode object, or the list is corrupted
    ASSERT( NULL != m_pHead );
    ASSERT( NULL != m_pTail );
    ASSERT( NULL != m_pCurrent );
    }
    else
    {
    ASSERT( NULL == m_pHead );
    ASSERT( NULL == m_pTail );
    ASSERT( NULL == m_pCurrent );
    }
}


