/*
 * IADVSINK.CPP
 * Data Object User Chapter 6
 *
 * Implementation of the IAdviseSink interface.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "datauser.h"


/*
 * CImpIAdviseSink::CImpIAdviseSink
 * CImpIAdviseSink::~CImpIAdviseSink
 *
 * Parameters (Constructor):
 *  pAV             PAPPVARS to the application
 *
 */

CImpIAdviseSink::CImpIAdviseSink(PAPPVARS pAV)
    {
    m_cRef=0;
    m_pAV=pAV;
    return;
    }

CImpIAdviseSink::~CImpIAdviseSink(void)
    {
    return;
    }




/*
 * CImpIAdviseSink::QueryInterface
 * CImpIAdviseSink::AddRef
 * CImpIAdviseSink::Release
 *
 * Purpose:
 *  IUnknown members for CImpIAdviseSink object.
 */

STDMETHODIMP CImpIAdviseSink::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    //Any interface on this object is the object pointer.
    if (IID_IUnknown==riid || IID_IAdviseSink==riid)
        *ppv=this;

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CImpIAdviseSink::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CImpIAdviseSink::Release(void)
    {
    ULONG   cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }




/*
 * CImpIAdviseSink::OnDataChange
 *
 * Purpose:
 *  Notifes the advise sink that data changed in a data object.
 *  On this message you may request a new data rendering and update
 *  your displays as necessary.  Any data sent to this function is
 *  owned by the caller, not by this advise sink.
 *
 *  All Advise Sink methods are asynchronous and therefore we
 *  should attempt no synchronous calls from within them to an EXE
 *  object.  If we do, we'll get RPC_E_CALLREJECTED as shown below.
 *
 * Parameters:
 *  pFEIn           LPFORMATETC describing format that changed
 *  pSTM            LPSTGMEDIUM providing the medium in which the
 *                  data is provided.
 *
 * Return Value:
 *  None
 */


STDMETHODIMP_(void) CImpIAdviseSink::OnDataChange(LPFORMATETC pFE
    , LPSTGMEDIUM pSTM)
    {
    BOOL        fUsable=TRUE;
    UINT        cf;
    STGMEDIUM   stm;

    /*
     * We first check that the changed data is, in fact, a format
     * we're interested in, either CF_TEXT, CF_BITMAP, or
     * CF_METAFILEPICT, then only in the aspects we want.  We check
     * if pSTM->tymed is TYMED_NULL or something else.  If NULL, we
     * just exit so the data object can time ADVF_NODATA trans-
     * actions.  Otherwise we verify that the data is useful and
     * repaint. If there is data in pSTM we are responsible for it.
     */

    //Ignore the m_fGetData flag for EXE objects (we can't GetData)
    if (!m_pAV->m_fGetData && !m_pAV->m_fEXE)
        return;

    //See if we're interested
    cf=pFE->cfFormat;

    if ((CF_TEXT!=cf && CF_BITMAP!=cf && CF_METAFILEPICT!=cf)
        || !(DVASPECT_CONTENT & pFE->dwAspect))
        return;

    //Check media types
    switch (cf)
        {
        case CF_TEXT:
            fUsable=(BOOL)(TYMED_HGLOBAL & pFE->tymed);
            break;

        case CF_BITMAP:
            fUsable=(BOOL)(TYMED_GDI & pFE->tymed);
            break;

        case CF_METAFILEPICT:
            fUsable=(BOOL)(TYMED_MFPICT & pFE->tymed);
            break;

        default:
            break;
        }

    if (!fUsable)
        return;

    if (NULL==m_pAV->m_pIDataObject)
        return;

    /*
     * When dealing with EXE objects, invalidate ourselves
     * after setting TYMED_NULL in our STGMEDIUM that causes
     * CAppVars::Paint to request new data.  We cannot call
     * GetData in here because this is an async call when we're
     * dealing with an EXE.
     */
    if (m_pAV->m_fEXE)
        {
        ReleaseStgMedium(&(m_pAV->m_stm));
        m_pAV->m_cf=cf;
        m_pAV->m_stm.tymed=TYMED_NULL;

        InvalidateRect(m_pAV->m_hWnd, NULL, TRUE);
        return;
        }

    if (FAILED(m_pAV->m_pIDataObject->GetData(pFE, &stm)))
        return;

    //Get rid of old data and update.
    ReleaseStgMedium(&(m_pAV->m_stm));

    m_pAV->m_cf=cf;
    m_pAV->m_stm=stm;

    InvalidateRect(m_pAV->m_hWnd, NULL, TRUE);

    if (m_pAV->m_fRepaint)
        UpdateWindow(m_pAV->m_hWnd);

    return;
    }






/*
 * CImpIAdviseSink::OnViewChange
 *
 * Purpose:
 *  Notifes the advise sink that presentation data changed in the
 *  data object to which we're connected providing the right time
 *  to update displays using such presentations.
 *
 * Parameters:
 *  dwAspect        DWORD indicating which aspect has changed.
 *  lindex          LONG indicating the piece that changed.
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnViewChange(DWORD dwAspect
    , LONG lindex)
    {
    return;
    }





/*
 * CImpIAdviseSink::OnRename
 *
 * Purpose:
 *  Informs the advise sink that a linked object has been renamed.
 *  Generally only the OLE default handler cares about this.
 *
 * Parameters:
 *  pmk             LPMONIKER providing the new name of the object
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnRename(LPMONIKER pmk)
    {
    return;
    }






/*
 * CImpIAdviseSink::OnSave
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has been saved
 *  persistently.  The primary purpose of this is for containers
 *  that want to make optimizations for objects that are not in a
 *  saved state, so on this you have to disable such optimizations.
 *  Generally only the OLE default handler cares about this.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnSave(void)
    {
    return;
    }





/*
 * CImpIAdviseSink::OnClose
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has closed and is
 *  no longer bound in any way.  Generally only of interest to the
 *  OLE default handler.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnClose(void)
    {
    return;
    }
