/*
 * KOALA.CPP
 * Koala Object DLL/EXE Chapter 4
 *
 * Implementation of the CKoala and CImpIPersist objects that
 * works in either an EXE or DLL.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "koala.h"


/*
 * CKoala::CKoala
 * CKoala::~CKoala
 *
 * Parameters (Constructor):
 *  pUnkOuter       LPUNKNOWN of a controlling unknown.
 *  pfnDestroy      PFNDESTROYED to call when an object
 *                  is destroyed.
 */

CKoala::CKoala(LPUNKNOWN pUnkOuter, PFNDESTROYED pfnDestroy)
    {
    m_cRef=0;
    m_pUnkOuter=pUnkOuter;
    m_pfnDestroy=pfnDestroy;

    //NULL any contained interfaces initially.
    m_pIPersist=NULL;

    return;
    }


CKoala::~CKoala(void)
    {
    //Free contained interfaces.
    if (NULL!=m_pIPersist)
        delete m_pIPersist;     //Interface does not free itself.

    return;
    }



/*
 * CKoala::FInit
 *
 * Purpose:
 *  Performs any intiailization of a CKoala that's prone to failure
 *  that we also use internally before exposing the object outside.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the function is successful,
 *                  FALSE otherwise.
 */

BOOL CKoala::FInit(void)
    {
    LPUNKNOWN       pIUnknown=this;

    if (NULL!=m_pUnkOuter)
        pIUnknown=m_pUnkOuter;

    //Allocate contained interfaces.
    m_pIPersist=new CImpIPersist(this, pIUnknown);

    return (NULL!=m_pIPersist);
    }






/*
 * CKoala::QueryInterface
 * CKoala::AddRef
 * CKoala::Release
 *
 * Purpose:
 *  IUnknown members for CKoala object.
 */

STDMETHODIMP CKoala::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    /*
     * The only calls for IUnknown are either in a nonaggregated
     * case or when created in an aggregation, so in either case
     * always return our IUnknown for IID_IUnknown.
     */
    if (IID_IUnknown==riid)
        *ppv=this;

    /*
     * For IPersist we return our contained interface.  For EXEs we
     * have to return our interface for IPersistStorage as well
     * since OLE doesn't support IPersist implementations by
     * themselves (assumed only to be a base class).  If a user
     * asked for an IPersistStorage and used it, they would crash--
     * but this is a demo, not a real object.
     */
    if (IID_IPersist==riid || IID_IPersistStorage==riid)
        *ppv=m_pIPersist;

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CKoala::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CKoala::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        {
        /*
         * Tell the housing that an object is going away so it can
         * shut down if appropriate.
         */
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();

        delete this;
        }

    return cRefT;
    }









/*
 * CImpIPersist::CImpIPersist
 * CImpIPersist::~CImpIPersist
 *
 * Parameters (Constructor):
 *  pObj            PCKoala of the object we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIPersist::CImpIPersist(PCKoala pObj, LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pObj=pObj;
    m_pUnkOuter=pUnkOuter;
    return;
    }

CImpIPersist::~CImpIPersist(void)
    {
    return;
    }






/*
 * CImpIPersist::QueryInterface
 * CImpIPersist::AddRef
 * CImpIPersist::Release
 *
 * Purpose:
 *  IUnknown members for CImpIPersist object.
 */

STDMETHODIMP CImpIPersist::QueryInterface(REFIID riid, PPVOID ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) CImpIPersist::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIPersist::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }




/*
 * CImpIPersist::GetClassID
 *
 * Purpose:
 *  Returns the Class ID of this object.
 *
 * Parameters:
 *  pClsID          LPCLSID in which to store our class ID.
 *
 * Return Value:
 *  HRESULT         NOERROR always.
 */

STDMETHODIMP CImpIPersist::GetClassID(LPCLSID pClsID)
    {
    *pClsID=CLSID_Koala;
    return NOERROR;
    }
