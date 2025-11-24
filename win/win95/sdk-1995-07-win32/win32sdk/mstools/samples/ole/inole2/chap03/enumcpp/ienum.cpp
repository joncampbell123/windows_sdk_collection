/*
 * IENUM.CPP
 * Enumerator in C++ Chapter 3
 *
 * Implements the CImpIEnumRECT class
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */

#include <windows.h>
#include "enumcpp.h"


/*
 * CreateRECTEnumerator
 *
 * Purpose:
 *  Given an array of rectangles, creates an enumerator interface
 *  on top of that array.
 *
 * Parameters:
 *  ppEnum          PENUMRECT * in which to return the
 *                  interface pointer on the created object.
 *
 * Return Value:
 *  BOOL            TRUE if the function is successful,
 *                  FALSE otherwise.
 */

BOOL CreateRECTEnumerator(PENUMRECT *ppEnum)
    {
    if (NULL==ppEnum)
        return FALSE;

    //Create the object storing a pointer to the interface
    *ppEnum=new CImpIEnumRECT();

    if (NULL==*ppEnum)
        return FALSE;

    //If creation worked, AddRef the interface
    if (NULL!=*ppEnum)
        (*ppEnum)->AddRef();

    return (NULL!=*ppEnum);
    }





/*
 * CImpIEnumRECT::CImpIEnumRECT
 * CImpIEnumRECT::~CImpIEnumRECT
 *
 * Constructor Parameters:
 *  None
 */

CImpIEnumRECT::CImpIEnumRECT(void)
    {
    UINT        i;

    //Initialize the array of rectangles
    for (i=0; i < CRECTS; i++)
        SetRect(&m_rgrc[i], i, i*2, i*3, i*4);

    //Ref counts always start as zero
    m_cRef=0;

    //Current pointer is the first element.
    m_iCur=0;

    return;
    }


CImpIEnumRECT::~CImpIEnumRECT(void)
    {
    return;
    }






/*
 * CImpIEnumRECT::AddRef
 *
 * Purpose:
 *  Increments the reference count on the object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  DWORD           New reference count.
 */

DWORD CImpIEnumRECT::AddRef(void)
    {
    return ++m_cRef;
    }






/*
 * CImpIEnumRECT::Release
 *
 * Purpose:
 *  Indicates that someone on whose behalf we once AddRef'd has
 *  finished with the object.  We decrement our reference count
 *  and if zero, we delete the object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  DWORD           Current reference count after decrement.  If
 *                  this returns zero then the interface is no
 *                  longer valid.
 */

DWORD CImpIEnumRECT::Release(void)
    {
    DWORD       cRefT;

    cRefT=--m_cRef;

    if (0==m_cRef)
        delete this;

    return cRefT;
    }







/*
 * CImpIEnumRECT::Next
 *
 * Purpose:
 *  Returns the next rectangle in the enumerator.
 *
 * Parameters:
 *  cRect           DWORD number of RECTs to return
 *  prc             LPRECT in which to store the returned RECT.
 *  pdwRects        LPDWORD in which to store the number of
 *                  structs returned.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise,
 *                  such as if we're at the end of the list.
 */

BOOL CImpIEnumRECT::Next(DWORD cRect, LPRECT prc, LPDWORD pdwRects)
    {
    DWORD           cRectReturn=0L;

    if (NULL==pdwRects)
        {
        if (1L!=cRect)
            return FALSE;
        }
    else
        *pdwRects=0L;

    if (NULL==prc || (m_iCur >= CRECTS))
        return FALSE;

    while (m_iCur < CRECTS && cRect > 0)
        {
        *prc++=m_rgrc[m_iCur++];
        cRectReturn++;
        cRect--;
        }

    if (NULL!=pdwRects)
        *pdwRects=cRectReturn;

    return TRUE;
    }





/*
 * CImpIEnumRECT::Skip
 *
 * Purpose:
 *  Skips the next n elements in the enumerator.
 *
 * Parameters:
 *  cSkip           DWORD number of elements to skip.
 *
 * Return Value:
 *  BOOL            TRUE if the elements could be skipped.  FALSE
 *                  if skipping would put us past the end of the
 *                  array.
 */

BOOL CImpIEnumRECT::Skip(DWORD cSkip)
    {
    if ((m_iCur+cSkip) >= CRECTS)
        return FALSE;

    m_iCur+=cSkip;
    return TRUE;
    }




/*
 * CImpIEnumRECT::Reset
 *
 * Purpose:
 *  Resets the current element in the enumerator to zero.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CImpIEnumRECT::Reset(void)
    {
    m_iCur=0;
    return;
    }
