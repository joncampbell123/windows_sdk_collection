/*
 * IENUMVAR.CPP
 *
 * Standard implementation of a VARIANT enumerator with the
 * IEnumVARIANT interface that will generally not need
 * modification.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "ienumfe.h"


/*
 * CEnumVariant::CEnumVariant
 * CEnumVariant::~CEnumVariant
 *
 * Parameters (Constructor):
 *  pUnkRef         LPUNKNOWN to use for reference counting.
 *  cVAR            ULONG number of VARIANTs in prgVAR
 *  prgVAR          LPVARIANT to the array to enumerate.
 */

CEnumVariant::CEnumVariant(LPUNKNOWN pUnkRef, ULONG cVAR
    , LPVARIANT prgVAR)
    {
    UINT        i;

    m_cRef=0;
    m_pUnkRef=pUnkRef;

    m_iCur=0;
    m_cVar=cVAR;
    m_prgVar=new VARIANT[(UINT)cVAR];

    if (NULL!=m_prgVar)
        {
        for (i=0; i < cVAR; i++)
            m_prgVar[i]=prgVAR[i];
        }

    return;
    }


CEnumVariant::~CEnumVariant(void)
    {
    if (NULL!=m_prgVar)
        delete [] m_prgVar;

    return;
    }






/*
 * CEnumVariant::QueryInterface
 * CEnumVariant::AddRef
 * CEnumVariant::Release
 *
 * Purpose:
 *  IUnknown members for CEnumVariant object.

STDMETHODIMP CEnumVariant::QueryInterface(REFIID riid
    , LPVOID *ppv)
    {
    *ppv=NULL;

    if (IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IEnumVARIANT))
        *ppv=(LPVOID)this;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CEnumVariant::AddRef(void)
    {
    ++m_cRef;
    m_pUnkRef->AddRef();
    return m_cRef;
    }

STDMETHODIMP_(ULONG) CEnumVariant::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    m_pUnkRef->Release();

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }







/*
 * CEnumVariant::Next
 *
 * Purpose:
 *  Returns the next element in the enumeration.
 *
 * Parameters:
 *  cVAR            ULONG number of VARIANTs to return.
 *  pVAR            LPVARIANT in which to store the returned
 *                  structures.
 *  pulVAR          ULONG * in which to return how many we
 *                  enumerated.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_FALSE otherwise,
 */

STDMETHODIMP CEnumVariant::Next(ULONG cVAR, LPVARIANT pVAR
    , ULONG * pulVAR)
    {
    ULONG               cReturn=0L;

    if (NULL==m_prgVar)
        return ResultFromScode(S_FALSE);

    if (NULL==pulVAR)
        {
        if (1L!=cVAR)
            return ResultFromScode(E_POINTER);
        }
    else
        *pulVAR=0L;

    if (NULL==pVAR|| m_iCur >= m_cVar)
        return ResultFromScode(S_FALSE);

    while (m_iCur < m_cVar && cVAR > 0)
        {
        *pVAR++=m_prgVar[m_iCur++];
        cReturn++;
        cVAR--;
        }

    if (NULL!=pulVAR)
        *pulVAR=cReturn;

    return NOERROR;
    }







/*
 * CEnumVariant::Skip
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

STDMETHODIMP CEnumVariant::Skip(ULONG cSkip)
    {
    if (((m_iCur+cSkip) >= m_cVar) || NULL==m_prgVar)
        return ResultFromScode(S_FALSE);

    m_iCur+=cSkip;
    return NOERROR;
    }






/*
 * CEnumVariant::Reset
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

STDMETHODIMP CEnumVariant::Reset(void)
    {
    m_iCur=0;
    return NOERROR;
    }






/*
 * CEnumVariant::Clone
 *
 * Purpose:
 *  Returns another IEnumVARIANT with the same state as ourselves.
 *
 * Parameters:
 *  ppEnum          LPENUMVARIANT * in which to return the
 *                  new object.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CEnumVariant::Clone(LPENUMVARIANT *ppEnum)
    {
    PCEnumVariant   pNew;

    *ppEnum=NULL;

    //Create the clone
    pNew=new CEnumVariant(m_pUnkRef, m_cVar, m_prgVar);

    if (NULL==pNew)
        return ResultFromScode(E_OUTOFMEMORY);

    pNew->AddRef();
    pNew->m_iCur=m_iCur;

    *ppEnum=pNew;
    return NOERROR;
    }
