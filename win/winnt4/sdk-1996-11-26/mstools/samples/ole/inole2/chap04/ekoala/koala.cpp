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
    m_pIExtConn=NULL;

    m_cStrong=0;
    return;
    }


CKoala::~CKoala(void)
    {
    //Free contained interfaces.
    if (NULL!=m_pIExtConn)
        delete m_pIExtConn;     //Interface does not free itself.

    if (NULL!=m_pIPersist)
        delete m_pIPersist;

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

    if (NULL==m_pIPersist)
        return FALSE;

    m_pIExtConn=new CImpIExternalConnection(this
        , pIUnknown);

    if (NULL==m_pIExtConn)
        return FALSE;

    return TRUE;
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
     * since OLE 2 doesn't support IPersist implementations by
     * themselves (assumed only to be a base class).  If a user
     * asked for an IPersistStorage and used it, they would crash--
     * but this is a demo, not a real object.
     */
    if (IID_IPersist==riid || IID_IPersistStorage==riid)
        *ppv=m_pIPersist;

    if (IID_IExternalConnection==riid)
        *ppv=m_pIExtConn;

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






/*
 * CImpIExternalConnection::CImpIExternalConnection
 * CImpIExternalConnection::~CImpIExternalConnection
 *
 * Parameters (Constructor):
 *  pObj            PCKoala of the object we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIExternalConnection::CImpIExternalConnection(PCKoala pObj
    , LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pObj=pObj;
    m_pUnkOuter=pUnkOuter;
    return;
    }

CImpIExternalConnection::~CImpIExternalConnection(void)
    {
    return;
    }



/*
 * CImpIExternalConnection::QueryInterface
 * CImpIExternalConnection::AddRef
 * CImpIExternalConnection::Release
 *
 * Purpose:
 *  Delegating IUnknown members for CImpIExternalConnection.
 */

STDMETHODIMP CImpIExternalConnection::QueryInterface(REFIID riid
    , PPVOID ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) CImpIExternalConnection::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIExternalConnection::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }




/*
 * CImpIExternalConnection::AddConnection
 *
 * Purpose:
 *  Informs the object that a strong connection has been made to it.
 *
 * Parameters:
 *  dwConn          DWORD identifying the type of connection, taken
 *                  from the EXTCONN enumeration.
 *  dwReserved      DWORD reserved.  This is used inside OLE and
 *                  should not be validated.
 *
 * Return Value:
 *  DWORD           The number of connection counts on the
 *                  object, used for debugging purposes only.
 */

STDMETHODIMP_(DWORD) CImpIExternalConnection::AddConnection
    (DWORD dwConn, DWORD dwReserved)
    {
    if (dwConn & EXTCONN_STRONG)
        m_pObj->m_cStrong++;

    return m_pObj->m_cStrong;
    }






/*
 * CImpIExternalConnection::ReleaseConnection
 *
 * Purpose:
 *  Informs an object that a connection has been taken away from
 *  it in which case the object may need to shut down.
 *
 * Parameters:
 *  dwConn              DWORD identifying the type of connection,
 *                      taken from the EXTCONN enumeration.
 *  dwReserved          DWORD reserved.  This is used inside OLE and
 *                      should not be validated.
 *  dwRerved            DWORD reserved
 *  fLastReleaseCloses  BOOL indicating if the last call to this
 *                      function should close the object.
 *
 * Return Value:
 *  DWORD           The number of remaining connection counts on
 *                  the object, used for debugging purposes only.
 */

STDMETHODIMP_(DWORD) CImpIExternalConnection::ReleaseConnection
    (DWORD dwConn, DWORD dwReserved, BOOL fLastReleaseCloses)
    {
    if (dwConn & EXTCONN_STRONG)
        m_pObj->m_cStrong--;

    return m_pObj->m_cStrong;
    }
