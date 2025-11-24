/*
 * FIGURE.CPP
 * Cosmo Chapter 13
 *
 * Implementation of the CFigure object for Cosmo.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "cosmo.h"


/*
 * CFigure::CFigure
 * CFigure::~CFigure
 *
 * Parameters (Constructor):
 *  pfnDestroy      PFNDESTROYED to call when object is destroyed.
 *  pDoc            PCCosmoDoc we're associated with.
 */

CFigure::CFigure(PFNDESTROYED pfnDestroy, PCCosmoDoc pDoc)
    {
    m_cRef=0;
    m_pfnDestroy=pfnDestroy;

    m_pFR=NULL;     //We get this later through FrameSet.
    m_pDoc=pDoc;
    m_pPL=pDoc->m_pPL;

    m_fEmbedded=FALSE;

    //NULL any contained interfaces initially.
    m_pIPersistStorage=NULL;
    m_pIStorage=NULL;
    m_pIStream=NULL;
    m_pIDataObject=NULL;
    m_pIDataAdviseHolder=NULL;
    m_pIOleObject=NULL;
    m_pIOleAdviseHolder=NULL;
    m_pIOleClientSite=NULL;

    m_cf=pDoc->m_cf;

    //These are for IDataObject::QueryGetData
    m_cfeGet=CFORMATETCGET;

    SETDefFormatEtc(m_rgfeGet[0], pDoc->m_cf, TYMED_HGLOBAL);
    SETDefFormatEtc(m_rgfeGet[1], pDoc->m_cfEmbedSource
        , TYMED_ISTORAGE);
    SETDefFormatEtc(m_rgfeGet[2], pDoc->m_cfObjectDescriptor
        , TYMED_HGLOBAL);
    SETDefFormatEtc(m_rgfeGet[3], CF_METAFILEPICT, TYMED_MFPICT);
    SETDefFormatEtc(m_rgfeGet[4], CF_BITMAP, TYMED_GDI);

    m_pST=NULL;

    //CHAPTER13MOD
    m_pIPersistFile=NULL;

    //We live in the document's lifetime, so no need to AddRef here.
    m_pMoniker=m_pDoc->m_pMoniker;
    m_dwRegROT=0L;
    //End CHAPTER13MOD

    return;
    }


CFigure::~CFigure(void)
    {
    if (NULL!=m_pIOleClientSite)
        {
        m_pIOleClientSite->Release();
        m_pIOleClientSite=NULL;
        }

    if (NULL!=m_pIDataAdviseHolder)
        {
        m_pIDataAdviseHolder->Release();
        m_pIDataAdviseHolder=NULL;
        }

    if (NULL!=m_pIOleAdviseHolder)
        {
        m_pIOleAdviseHolder->Release();
        m_pIOleAdviseHolder=NULL;
        }

    //CHAPTER13MOD
    //Make sure no one thinks we're still running
    if (0L!=m_dwRegROT)
        OleStdRevokeAsRunning(&m_dwRegROT);

    //Free contained interfaces.
    if (NULL!=m_pIPersistFile)
        delete m_pIPersistFile;
    //End CHAPTER13MOD

    if (NULL!=m_pIOleObject)
        delete m_pIOleObject;

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;

    if (NULL!=m_pIStorage)
        m_pIStorage->Release();

    if (NULL!=m_pIStream)
        m_pIStream->Release();

    if (NULL!=m_pIPersistStorage)
        delete m_pIPersistStorage;

    //Free strings.
    if (NULL!=m_pST)
        delete m_pST;

    return;
    }





/*
 * CFigure::QueryInterface
 * CFigure::AddRef
 * CFigure::Release
 *
 * Purpose:
 *  IUnknown members for CFigure object.
 */

STDMETHODIMP CFigure::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid)
        *ppv=this;

    if (IID_IPersist==riid || IID_IPersistStorage==riid)
        *ppv=m_pIPersistStorage;

    if (IID_IDataObject==riid)
        *ppv=m_pIDataObject;

    if (IID_IOleObject==riid)
        *ppv=m_pIOleObject;

    //CHAPTER13MOD
    if (IID_IPersistFile==riid)
        *ppv=m_pIPersistFile;
    //End CHAPTER13MOD

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CFigure::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CFigure::Release(void)
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
        }

    return cRefT;
    }






/*
 * CFigure::FInit
 *
 * Purpose:
 *  Performs any initialization of a CFigure that's prone to failure
 *  that we also use internally before exposing the object outside.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the function is successful,
 *                  FALSE otherwise.
 */

BOOL CFigure::FInit(void)
    {
    m_pST=new CStringTable(m_pDoc->m_hInst);

    if (NULL==m_pST)
        return FALSE;

    if (!m_pST->FInit(IDS_FIGUREMIN, IDS_FIGUREMAX))
        return FALSE;

    //Allocate contained interfaces.
    m_pIPersistStorage=new CImpIPersistStorage(this, this);

    if (NULL==m_pIPersistStorage)
        return FALSE;

    m_pIDataObject=new CImpIDataObject(this, this);

    if (NULL==m_pIDataObject)
        return FALSE;

    m_pIOleObject=new CImpIOleObject(this, this);

    if (NULL==m_pIOleObject)
        return FALSE;

    //CHAPTER13MOD
    m_pIPersistFile=new CImpIPersistFile(this, this);

    if (NULL==m_pIPersistFile)
        return FALSE;
    //End CHAPTER13MOD

    return TRUE;
    }



/*
 * CFigure::FrameSet
 *
 * Purpose:
 *  Provides the compound document object with access to the frame
 *  of this application for UI purposes.
 *
 * Parameters:
 *  pFR             PCCosmoFrame of the frame window.
 *
 * Return Value:
 *  None
 */

void CFigure::FrameSet(PCCosmoFrame pFR)
    {
    m_pFR=pFR;
    return;
    }




/*
 * CFigure::FIsDirty
 *
 * Purpose:
 *  Checks if the document is dirty.  This can be called from
 *  IPersistStorage::IsDirty which doesn't have access to CCosmoDoc.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if dirty, FALSE if clean.
 */

BOOL CFigure::FIsDirty(void)
    {
    return m_pDoc->m_fDirty;
    }




/*
 * CFigure::FIsEmbedded
 *
 * Purpose:
 *  Answers if the object is embedded or not.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the object is embedded, FALSE otherwise.
 */

BOOL CFigure::FIsEmbedded(void)
    {
    return m_fEmbedded;
    }




/*
 * CFigure::SendAdvise
 *
 * Purpose:
 *  Calls the appropriate IOleClientSite or IAdviseSink member
 *  function for various events such as closure, saving, etc.
 *
 * Parameters:
 *  uCode           UINT OBJECTCODE_* identifying the notification.
 *
 * Return Value:
 *  None
 */

void CFigure::SendAdvise(UINT uCode)
    {
    switch (uCode)
        {
        case OBJECTCODE_SAVED:
            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnSave();

			break;

        case OBJECTCODE_CLOSED:
            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnClose();

			break;

        case OBJECTCODE_RENAMED:
            //CHAPTER13MOD
            //Update the moniker copy we have.
            m_pMoniker=m_pDoc->m_pMoniker;

            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnRename(m_pMoniker);
            //End CHAPTER13MOD
            break;

        case OBJECTCODE_SAVEOBJECT:
            if (FIsDirty() && NULL!=m_pIOleClientSite)
                m_pIOleClientSite->SaveObject();

            break;

        case OBJECTCODE_DATACHANGED:
            //No flags are necessary here.
            if (NULL!=m_pIDataAdviseHolder)
                {
                m_pIDataAdviseHolder->SendOnDataChange
                    (m_pIDataObject, 0, 0);
                }
            break;

        case OBJECTCODE_SHOWWINDOW:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->OnShowWindow(TRUE);

            break;

        case OBJECTCODE_HIDEWINDOW:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->OnShowWindow(FALSE);

            break;

        case OBJECTCODE_SHOWOBJECT:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->ShowObject();

            break;
        }

    return;
    }
