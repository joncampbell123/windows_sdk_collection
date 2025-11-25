/*
 * IENUMUNK.CPP
 *
 * Standard implementation of am IUnknown enumerator with the
 * IEnumUnknown interface that will generally not need
 * modification.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "ienumunk.h"


/*
 * CEnumUnknown::CEnumUnknown
 * CEnumUnknown::~CEnumUnknown
 *
 * Parameters (Constructor):
 *  pUnkRef         LPUNKNOWN to use for reference counting.
 *  cUnk            ULONG number of LPUNKNOWNs in prgUnk
 *  prgUnk          LPUNKNOWN to the array to enumerate.
 */

CEnumUnknown::CEnumUnknown(LPUNKNOWN pUnkRef, ULONG cUnk
    , LPUNKNOWN prgUnk)
    {
    UINT        i;

    m_cRef=0;
    m_pUnkRef=pUnkRef;

    m_iCur=0;
    m_cUnk=cUnk;
    m_prgUnk=new LPUNKNOWN[(UINT)cUnk];

    if (NULL!=m_prgUnk)
        {
        for (i=0; i < cUnk; i++)
            m_prgUnk[i]=prgUnk[i];
        }

    return;
    }


CEnumUnknown::~CEnumUnknown(void)
    {
    if (NULL!=m_prgUnk)
        delete [] m_prgUnk;

    return;
    }






/*
 * CEnumUnknown::QueryInterface
 * CEnumUnknown::AddRef
 * CEnumUnknown::Release
 *
 * Purpose:
 *  IUnknown members for CEnumUnknown object.
 */

STDMETHODIMP CEnumUnknown::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    *ppv=NULL;

    /*
     * Enumerators are separate objects, not the data object, so
     * we only need to support out IUnknown and IEnumUnknown
     * interfaces here with no concern for aggregation.
     */
    if (IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IEnumUnknown))
        *ppv=(LPVOID)this;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CEnumUnknown::AddRef(void)
    {
    ++m_cRef;
    m_pUnkRef->AddRef();
    return m_cRef;
    }

STDMETHODIMP_(ULONG) CEnumUnknown::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    m_pUnkRef->Release();

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }







/*
 * CEnumUnknown::Next
 *
 * Purpose:
 *  Returns the next element in the enumeration.
 *
 * Parameters:
 *  cUnk            ULONG number of LPUNKNOWNs to return.
 *  pUnk            LPUNKNOWN in which to store the returned
 *                  structures.
 *  pulUnk          ULONG * in which to return how many we
 *                  enumerated.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE otherwise,
 */

STDMETHODIMP CEnumUnknown::Next(ULONG cUnk, LPUNKNOWN pUnk
    , ULONG * pulUnk)
    {
    ULONG               cReturn=0L;

    if (NULL==m_prgUnk)
        return ResultFromScode(S_FALSE);

    if (NULL==pulUnk)
        {
        if (1L!=cUnk)
            return ResultFromScode(E_POINTER);
        }
    else
        *pulUnk=0L;

    if (NULL==pUnk || m_iCur >= m_cUnk)
        return ResultFromScode(S_FALSE);

    while (m_iCur < m_cUnk && cUnk > 0)
        {
        *pUnk++=m_prgUnk[m_iCur++];
        cReturn++;
        cUnk--;
        }

    if (NULL!=pulUnk)
        *pulUnk=cReturn;

    return NOERROR;
    }







/*
 * CEnumUnknown::Skip
 *
 * Purpose:
 *  Skips the next n elements in the enumeration.
 *
 * Parameters:
 *  cSkip           ULONG number of elements to skip.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE if we could not
 *                  skip the requested number.
 */

STDMETHODIMP CEnumUnknown::Skip(ULONG cSkip)
    {
    if (((m_iCur+cSkip) >= m_cUnk) || NULL==m_prgUnk)
        return ResultFromScode(S_FALSE);

    m_iCur+=cSkip;
    return NOERROR;
    }






/*
 * CEnumUnknown::Reset
 *
 * Purpose:
 *  Resets the current element index in the enumeration to zero.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CEnumUnknown::Reset(void)
    {
    m_iCur=0;
    return NOERROR;
    }






/*
 * CEnumUnknown::Clone
 *
 * Purpose:
 *  Returns another IEnumUnknown with the same state as ourselves.
 *
 * Parameters:
 *  ppEnum          LPENUMUNKNOWN * in which to return the
 *                  new object.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CEnumUnknown::Clone(LPENUMUNKNOWN *ppEnum)
    {
    PCEnumUnknown   pNew;

    *ppEnum=NULL;

    //Create the clone
    pNew=new CEnumUnknown(m_pUnkRef, m_cUnk, m_prgUnk);

    if (NULL==pNew)
        return ResultFromScode(E_OUTOFMEMORY);

    pNew->AddRef();
    pNew->m_iCur=m_iCur;

    *ppEnum=pNew;
    return NOERROR;
    }
