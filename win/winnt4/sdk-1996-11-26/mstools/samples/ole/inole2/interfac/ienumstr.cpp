/*
 * IENUMSTR.CPP
 *
 * Standard implementation of a string enumerator with the
 * IEnumString interface that will generally not need
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


#include "ienumstr.h"


/*
 * CEnumString::CEnumString
 * CEnumString::~CEnumString
 *
 * Parameters (Constructor):
 *  pUnkRef         LPUNKNOWN to use for reference counting.
 *  cstr            ULONG number of LPTSTRs in pstr
 *  prgstr          LPTSTR to the array to enumerate.
 */

CEnumString::CEnumString(LPUNKNOWN pUnkRef, ULONG cstr
    , LPTSTR prgstr)
    {
    UINT        i;

    m_cRef=0;
    m_pUnkRef=pUnkRef;

    m_iCur=0;
    m_cstr=cFE;
    m_prgstr=new LPTSTR[(UINT)cFE];

    if (NULL!=m_prgstr)
        {
        for (i=0; i < cFE; i++)
            m_prgstr[i]=prgFE[i];
        }

    return;
    }


CEnumString::~CEnumString(void)
    {
    if (NULL!=m_prgstr)
        delete [] m_prgstr;

    return;
    }






/*
 * CEnumString::QueryInterface
 * CEnumString::AddRef
 * CEnumString::Release
 *
 * Purpose:
 *  IUnknown members for CEnumString object.
 */

STDMETHODIMP CEnumString::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    *ppv=NULL;

    if (IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IEnumString))
        *ppv=(LPVOID)this;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CEnumString::AddRef(void)
    {
    ++m_cRef;
    m_pUnkRef->AddRef();
    return m_cRef;
    }

STDMETHODIMP_(ULONG) CEnumString::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    m_pUnkRef->Release();

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }







/*
 * CEnumString::Next
 *
 * Purpose:
 *  Returns the next element in the enumeration.
 *
 * Parameters:
 *  cstr            ULONG number of LPTSTRs to return.
 *  pstr            LPTSTR in which to store the returned
 *                  structures.
 *  pulstr          ULONG * in which to return how many we
 *                  enumerated.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE otherwise,
 */

STDMETHODIMP CEnumString::Next(ULONG cstr, LPTSTR pstr
    , ULONG * pulstr)
    {
    ULONG               cReturn=0L;

    if (NULL==m_prgstr)
        return ResultFromScode(S_FALSE);

    if (NULL==pulstr)
        {
        if (1L!=cstr)
            return ResultFromScode(E_POINTER);
        }
    else
        *pulstr=0L;

    if (NULL==pstr || m_iCur >= m_cstr)
        return ResultFromScode(S_FALSE);

    while (m_iCur < m_cstr && cstr > 0)
        {
        *pstr++=m_prgstr[m_iCur++];
        cReturn++;
        cstr--;
        }

    if (NULL!=pulstr)
        *pulstr=cReturn;

    return NOERROR;
    }







/*
 * CEnumString::Skip
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

STDMETHODIMP CEnumString::Skip(ULONG cSkip)
    {
    if (((m_iCur+cSkip) >= m_cstr) || NULL==m_prgstr)
        return ResultFromScode(S_FALSE);

    m_iCur+=cSkip;
    return NOERROR;
    }






/*
 * CEnumString::Reset
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

STDMETHODIMP CEnumString::Reset(void)
    {
    m_iCur=0;
    return NOERROR;
    }






/*
 * CEnumString::Clone
 *
 * Purpose:
 *  Returns another IEnumString with the same state as ourselves.
 *
 * Parameters:
 *  ppEnum          LPENUMSTRING * in which to return the
 *                  new object.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CEnumString::Clone(LPENUMSTRING *ppEnum)
    {
    PCEnumString    pNew;

    *ppEnum=NULL;

    //Create the clone
    pNew=new CEnumString(m_pUnkRef, m_cstr, m_prgstr);

    if (NULL==pNew)
        return ResultFromScode(E_OUTOFMEMORY);

    pNew->AddRef();
    pNew->m_iCur=m_iCur;

    *ppEnum=pNew;
    return NOERROR;
    }
