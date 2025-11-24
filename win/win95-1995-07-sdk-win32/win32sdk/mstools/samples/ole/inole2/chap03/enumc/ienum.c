/*
 * IENUM.C
 * Enumerator in C Chapter 3
 *
 * Implements the IMPIENUMRECT structure and functions (an object).
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
#include <malloc.h>
#include "enumc.h"


//We have to explicitly define fucntion table for IEnumRECT in C
static IEnumRECTVtbl  vtEnumRect;
static BOOL           fVtblInitialized=FALSE;


/*
 * CreateRECTEnumerator
 *
 * Purpose:
 *  Given an array of rectangles, creates an enumerator interface
 *  on top of that array.
 *
 * Parameters:
 *  ppEnum          PENUMRECT * in which to return the interface
 *                  pointer on the created object.
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
    *ppEnum=(PENUMRECT)IMPIEnumRect_Constructor();

    if (NULL==*ppEnum)
        return FALSE;

    //If creation worked, AddRef the interface
    if (NULL!=*ppEnum)
        (*ppEnum)->lpVtbl->AddRef(*ppEnum);

    return (NULL!=*ppEnum);
    }





/*
 * IMPIEnumRect_Constructor
 *
 * Purpose:
 *  Constructor for an IMPIEnumRect structure
 *
 * Parameters:
 *  None
 */

PIMPIENUMRECT IMPIEnumRect_Constructor(void)
    {
    PIMPIENUMRECT       pER;
    UINT                i;

    /*
     * First time through initialize function table.  Such a table
     * could be defined as a constant instead of doing explicit
     * initialization here.  However, this method shows exactly
     * which pointers are going where and does not depend on knowing
     * the ordering of the functions in the table, just the names.
     */
    if (!fVtblInitialized)
        {
        vtEnumRect.AddRef =IMPIEnumRect_AddRef;
        vtEnumRect.Release=IMPIEnumRect_Release;
        vtEnumRect.Next   =IMPIEnumRect_Next;
        vtEnumRect.Skip   =IMPIEnumRect_Skip;
        vtEnumRect.Reset  =IMPIEnumRect_Reset;

        fVtblInitialized=TRUE;
        }

    pER=(PIMPIENUMRECT)malloc(sizeof(IMPIENUMRECT));

    if (NULL==pER)
        return NULL;

    //Initialize function table pointer
    pER->lpVtbl=&vtEnumRect;

    //Initialize the array of rectangles
    for (i=0; i < CRECTS; i++)
        SetRect(&pER->m_rgrc[i], i, i*2, i*3, i*4);

    //Ref counts always start as zero
    pER->m_cRef=0;

    //Current pointer is the first element.
    pER->m_iCur=0;

    return pER;
    }





/*
 * IMPIEnumRect_Destructor
 *
 * Purpose:
 *  Destructor for IMPIENUMRECT structures.
 *
 * Parameters:
 *  pER            PIMPIENUMRECT to free
 */

void IMPIEnumRect_Destructor(PIMPIENUMRECT pER)
    {
    if (NULL==pER)
        return;

    free(pER);

    return;
    }






/*
 * IMPIEnumRect_AddRef
 *
 * Purpose:
 *  Increments the reference count on the object.
 *
 * Parameters:
 *  pEnum           PENUMRECT to affect
 *
 * Return Value:
 *  DWORD           New reference count.
 */

DWORD IMPIEnumRect_AddRef(PENUMRECT pEnum)
    {
    PIMPIENUMRECT       pER=(PIMPIENUMRECT)pEnum;

    if (NULL==pER)
        return 0L;

    return ++pER->m_cRef;
    }






/*
 * IMPIEnumRect_Release
 *
 * Purpose:
 *  Indicates that someone on whose behalf we once AddRef'd has
 *  finished with the object.  We decrement our reference count
 *  and if zero, we delete the object.
 *
 * Parameters:
 *  pEnum           PENUMRECT to affect
 *
 * Return Value:
 *  DWORD           Current reference count after decrement.  If
 *                  pER returns zero then the interface is no
 *                  longer valid.
 */

DWORD IMPIEnumRect_Release(PENUMRECT pEnum)
    {
    PIMPIENUMRECT       pER=(PIMPIENUMRECT)pEnum;
    DWORD               cRefT;

    if (NULL==pER)
        return 0L;

    cRefT=--pER->m_cRef;

    if (0==pER->m_cRef)
        IMPIEnumRect_Destructor(pER);

    return cRefT;
    }







/*
 * IMPIEnumRect_Next
 *
 * Purpose:
 *  Returns the next rectangle in the enumerator.
 *
 * Parameters:
 *  pEnum           PENUMRECT to affect
 *  cRect           DWORD number of RECTs to return
 *  prc             LPRECT in which to store the returned RECT.
 *  pdwRects        LPDWORD in which to store the number of
 *                  structs returned.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise,
 *                  such as if we're at the end of the list.
 */

BOOL IMPIEnumRect_Next(PENUMRECT pEnum, DWORD cRect, LPRECT prc
    , LPDWORD pdwRects)
    {
    PIMPIENUMRECT       pER=(PIMPIENUMRECT)pEnum;
    DWORD               cRectReturn=0L;

    if (NULL==pdwRects)
        {
        if (1L!=cRect)
            return FALSE;
        }
    else
        *pdwRects=0L;

    if (NULL==prc || (pER->m_iCur >= CRECTS))
        return FALSE;

    while (pER->m_iCur < CRECTS && cRect > 0)
        {
        *prc++=pER->m_rgrc[pER->m_iCur++];
        cRectReturn++;
        cRect--;
        }

    if (NULL!=pdwRects)
        *pdwRects=cRectReturn;

    return TRUE;
    }





/*
 * IMPIEnumRect_Skip
 *
 * Purpose:
 *  Skips the next n elements in the enumerator.
 *
 * Parameters:
 *  pEnum           PENUMRECT to affect
 *  cSkip           DWORD number of elements to skip.
 *
 * Return Value:
 *  BOOL            TRUE if the elements could be skipped.  FALSE
 *                  if skipping would put us past the end of the
 *                  array.
 */

BOOL IMPIEnumRect_Skip(PENUMRECT pEnum, DWORD cSkip)
    {
    PIMPIENUMRECT       pER=(PIMPIENUMRECT)pEnum;

    if (NULL==pER)
        return FALSE;

    if ((pER->m_iCur+cSkip) >= CRECTS)
        return FALSE;

    pER->m_iCur+=cSkip;
    return TRUE;
    }





/*
 * IMPIEnumRect_Reset
 *
 * Purpose:
 *  Resets the current element in the enumerator to zero.
 *
 * Parameters:
 *  pEnum            PENUMRECT to affect
 *
 * Return Value:
 *  None
 */

void IMPIEnumRect_Reset(PENUMRECT pEnum)
    {
    PIMPIENUMRECT       pER=(PIMPIENUMRECT)pEnum;

    if (NULL==pER)
        return;

    pER->m_iCur=0;
    return;
    }
