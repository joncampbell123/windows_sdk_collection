/*
 * IPERSTOR.CPP
 * Cosmo Chapter 14
 *
 * Implementation of the IPersistStorage interface that we expose on
 * the CFigure compound document object.  This ties into the
 * functionality of CPolyline.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "cosmo.h"


/*
 * CImpIPersistStorage:CImpIPersistStorage
 * CImpIPersistStorage::~CImpIPersistStorage
 *
 * Constructor Parameters:
 *  pObj            PCFigure associated with this object.
 *  pUnkOuter       LPUNKNOWN of the controlling unknown.
 */

CImpIPersistStorage::CImpIPersistStorage(PCFigure pObj
    , LPUNKNOWN pUnkOuter)
    {
    m_cRef=0;
    m_pObj=pObj;
    m_pUnkOuter=pUnkOuter;
    m_psState=PSSTATE_UNINIT;

    //CHAPTER14MOD
    m_fConvert=FALSE;
    m_clsID=CLSID_Cosmo2Figure;
    //End CHAPTER14MOD

    return;
    }


CImpIPersistStorage::~CImpIPersistStorage(void)
    {
    return;
    }




/*
 * CImpIPersistStorage::QueryInterface
 * CImpIPersistStorage::AddRef
 * CImpIPersistStorage::Release
 *
 * Purpose:
 *  Standard set of IUnknown members for this interface
 */

STDMETHODIMP CImpIPersistStorage::QueryInterface(REFIID riid
    , PPVOID ppv)
    {
    return m_pUnkOuter->QueryInterface(riid, ppv);
    }

STDMETHODIMP_(ULONG) CImpIPersistStorage::AddRef(void)
    {
    ++m_cRef;
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIPersistStorage::Release(void)
    {
    --m_cRef;
    return m_pUnkOuter->Release();
    }





/*
 * CImpIPersistStorage::GetClassID
 *
 * Purpose:
 *  Returns the CLSID of the object represented by this interface.
 *
 * Parameters:
 *  pClsID          LPCLSID in which to store our CLSID.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::GetClassID(LPCLSID pClsID)
    {
    if (PSSTATE_UNINIT==m_psState)
        return ResultFromScode(E_UNEXPECTED);

    //CHAPTER14MOD
    *pClsID=m_clsID;
    //End CHAPTER14MOD
    return NOERROR;
    }





/*
 * CImpIPersistStorage::IsDirty
 *
 * Purpose:
 *  Tells the caller if we have made changes to this object since
 *  it was loaded or initialized new.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Contains S_OK if we ARE dirty, S_FALSE if
 *                  NOT dirty.
 */

STDMETHODIMP CImpIPersistStorage::IsDirty(void)
    {
    if (PSSTATE_UNINIT==m_psState)
        return ResultFromScode(E_UNEXPECTED);

    //CFigure::FIsDirty returns the document's dirty flag.
    return ResultFromScode(m_pObj->FIsDirty() ? S_OK : S_FALSE);
    }







/*
 * CImpIPersistStorage::InitNew
 *
 * Purpose:
 *  Provides the object with the IStorage to hold on to while the
 *  object is running.  Here we initialize the structure of the
 *  storage and AddRef it for incremental access. This function will
 *  only be called once in the object's lifetime in lieu of Load.
 *
 * Parameters:
 *  pIStorage       LPSTORAGE for the object.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::InitNew(LPSTORAGE pIStorage)
    {
    HRESULT     hr;

    if (PSSTATE_UNINIT!=m_psState)
        return ResultFromScode(E_UNEXPECTED);

    if (NULL==pIStorage)
        return ResultFromScode(E_POINTER);

    /*
     * The rules of IPersistStorage mean we hold onto the IStorage
     * and pre-create anything we'd need in Save(...,TRUE) for
     * low-memory situations.  For us this means creating our
     * "CONTENTS" stream and holding onto that IStream as
     * well as the IStorage here (requiring an AddRef call).
     */

    hr=pIStorage->CreateStream(SZSTREAM, STGM_DIRECT
        | STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE
        , 0, 0, &m_pObj->m_pIStream);

    if (FAILED(hr))
        return hr;

    //We expect that the client has called WriteClassStg
#if defined(WIN32) && !defined(UNICODE)
    OLECHAR pwcsTemp[256];
    mbstowcs(pwcsTemp, (*m_pObj->m_pST)[IDS_USERTYPE], 256);
    WriteFmtUserTypeStg(pIStorage, m_pObj->m_cf, pwcsTemp);
#else
    WriteFmtUserTypeStg(pIStorage, m_pObj->m_cf
        , (*m_pObj->m_pST)[IDS_USERTYPE]);
#endif

    pIStorage->AddRef();
    m_pObj->m_pIStorage=pIStorage;

    m_psState=PSSTATE_SCRIBBLE;
    return NOERROR;
    }





/*
 * CImpIPersistStorage::Load
 *
 * Purpose:
 *  Instructs the object to load itself from a previously saved
 *  IStorage that was handled by Save in another object lifetime.
 *  This function will only be called once in the object's lifetime
 *  in lieu of InitNew. The object should hold on to pIStorage here
 *  for incremental access and low-memory saves in Save.
 *
 * Parameters:
 *  pIStorage       LPSTORAGE from which to load.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::Load(LPSTORAGE pIStorage)
    {
    HRESULT     hr;
    LONG        lRet;
    LPSTREAM    pIStream;

    if (PSSTATE_UNINIT!=m_psState)
        return ResultFromScode(E_UNEXPECTED);

    if (NULL==pIStorage)
        return ResultFromScode(E_POINTER);

    //CHAPTER14MOD
    //This tells us if we're coming from another class storage.
    m_fConvert=(NOERROR==GetConvertStg(pIStorage));

    //This is the type of storage we're really messing with in Treat As
    if (m_fConvert)
        ReadClassStg(pIStorage, &m_clsID);
    //End CHAPTER14MOD

    hr=pIStorage->OpenStream(SZSTREAM, 0, STGM_DIRECT
        | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pIStream);

    //CHAPTER14MOD
    //We might be looking for OLE 1 streams as well.
    if (FAILED(hr))
        {
        hr=pIStorage->OpenStream(SZOLE1STREAM, 0, STGM_DIRECT
            | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pIStream);

        if (FAILED(hr))
            return ResultFromScode(STG_E_READFAULT);

        m_pObj->m_pPL->m_fReadFromOLE10=TRUE;
        }
    //End CHAPTER14MOD

    if (FAILED(hr))
        return ResultFromScode(STG_E_READFAULT);

    lRet=m_pObj->m_pPL->ReadFromStream(pIStream);

    if (lRet < 0)
        return ResultFromScode(STG_E_READFAULT);


    /*
     * We don't call pIStream->Release here because we may need
     * it for a low-memory save in Save.  We also need to
     * hold onto a copy of pIStorage, meaning AddRef.
     */
    m_pObj->m_pIStream=pIStream;

    pIStorage->AddRef();
    m_pObj->m_pIStorage=pIStorage;

    m_psState=PSSTATE_SCRIBBLE;
    return NOERROR;
    }





/*
 * CImpIPersistStorage::Save
 *
 * Purpose:
 * Purpose:
 *  Saves the data for this object to an IStorage which may
 *  or may not be the same as the one previously passed to
 *  Load, indicated with fSameAsLoad.  After this call we may
 *  not write into the storage again until SaveCompleted is
 *  called, although we may still read.
 *
 * Parameters:
 *  pIStorage       LPSTORAGE in which to save our data.
 *  fSameAsLoad     BOOL indicating if this is the same pIStorage
 *                  that was passed to Load.  If TRUE, then the
 *                  object should write whatever it has *without
 *                  *using any extra memory* as this may be a low
 *                  memory save attempt.  That means that you must
 *                  not try to open or create streams.  If FALSE
 *                  you need to regenerate your whole storage
 *                  structure, being sure to also release any
 *                  pointers held from InitNew and Load.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::Save(LPSTORAGE pIStorage
    , BOOL fSameAsLoad)
    {
    LONG        lRet;
    HRESULT     hr;
    LPSTREAM    pIStream;
    //CHAPTER14MOD
    LONG        lVer=VERSIONCURRENT;
    //End CHAPTER14MOD

    //Have to come here from scribble state.
    if (PSSTATE_SCRIBBLE!=m_psState)
        return ResultFromScode(E_UNEXPECTED);

    //Must have an IStorage if we're not in SameAsLoad
    if (NULL==pIStorage && !fSameAsLoad)
        return ResultFromScode(E_POINTER);

    //CHAPTER14MOD
    /*
     * If this was read from an OLE 1.0 storage, but there is no
     * convert bit, then we have to save the 1.0 format to the
     * "\1Ole10Native" stream.  Otherwise if we were converting
     * from OLE 1.0, we should nuke the stream since it's no longer
     * useful.  To handle this, we call WriteToStorage with
     * VERSIONCURRENT in any convert case.  WriteToStorage will
     * remove the OLE 1.0 stream if it previously read from one, or
     * it will just save normally.
     *
     * The Polyine allows us to look at it's m_fReadFromOLE10 which
     * tells us to pass it 0x00010000 if we're not converting, that
     * is, we're doing Treat As on the OLE 1.0 object and so we
     * need to write the new data in the Ole10Native stream.
     */

    if (!m_fConvert && m_pObj->m_pPL->m_fReadFromOLE10)
        lVer=0x00010000;
    //End CHAPTER14MOD

    /*
     * If we're saving to a new storage, create a new stream.
     * If fSameAsLoad it TRUE, then we write to the
     * stream we already allocated.  We should NOT depends on
     * pIStorage with fSameAsLoad is TRUE.
     */

    //CHAPTER14MOD
    /*
     * If we're converting an OLE 1 storage to an OLE 2 storage,
     * then we have to create a new stream (conversion is not
     * guaranteed to succeed in low memory) and delete the old
     * one, so we ignore fSameAsLoad if we're converting.
     */

    if (fSameAsLoad
        && !(m_pObj->m_pPL->m_fReadFromOLE10 && m_fConvert))
    //End CHAPTER14MOD
        {
        LARGE_INTEGER   li;

        /*
         * Use pre-allocated streams to avoid failures due
         * to low-memory conditions.  Be sure to reset the
         * stream pointer if you used this stream before!!
         */
        pIStream=m_pObj->m_pIStream;
        LISet32(li, 0);
        pIStream->Seek(li, STREAM_SEEK_SET, NULL);

        //This matches the Release below.
        pIStream->AddRef();
        }
    else
        {
        hr=pIStorage->CreateStream(SZSTREAM, STGM_DIRECT
            | STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE
            , 0, 0, &pIStream);

        if (FAILED(hr))
            return hr;

#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsTemp[256];
        mbstowcs(pwcsTemp, (*m_pObj->m_pST)[IDS_USERTYPE], 256);
        WriteFmtUserTypeStg(pIStorage, m_pObj->m_cf, pwcsTemp);
#else
        WriteFmtUserTypeStg(pIStorage, m_pObj->m_cf
            , (*m_pObj->m_pST)[IDS_USERTYPE]);
#endif
        }

    //CHAPTER14MOD
    lRet=m_pObj->m_pPL->WriteToStream(pIStream, lVer);
    //End CHAPTER14MOD
    pIStream->Release();

    //CHAPTER14MOD
    /*
     * If we are overwriting an OLE 1 storage, delete the old
     * Ole10Native stream if writing our CONTENTS worked.
     */
    if (m_pObj->m_pPL->m_fReadFromOLE10 && m_fConvert && (lRet >= 0))
        pIStorage->DestroyElement(SZOLE1STREAM);

    //Clear the convert bit if it was set
    if (m_fConvert)
        {
        UINT        cf;

        /*
         * I really don't care to read this from resources...but we
         * need to make this call to fully convert a storage since
         * this may not be getting called from a document save.
         */
        cf=RegisterClipboardFormat((*m_pObj->m_pST)[IDS_FORMAT]);
#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsTemp[256];
        mbstowcs(pwcsTemp, (*m_pObj->m_pST)[IDS_USERTYPE], 256);
        WriteFmtUserTypeStg(pIStorage, cf, pwcsTemp);
#else
        WriteFmtUserTypeStg(pIStorage, cf
            , (*m_pObj->m_pST)[IDS_USERTYPE]);
#endif
        SetConvertStg(pIStorage, FALSE);
        m_fConvert=FALSE;
        }
    //End CHAPTER14MOD

    if (lRet >= 0)
        {
        m_psState=PSSTATE_ZOMBIE;
        return NOERROR;
        }

    return ResultFromScode(STG_E_WRITEFAULT);
    }






/*
 * CImpIPersistStorage::SaveCompleted
 *
 * Purpose:
 *  Notifies the object that the storage in pIStorage has been
 *  completely saved now.  This is called when the user of this
 *  object wants to save us in a completely new storage, and if
 *  we normally hang on to the storage we have to reinitialize
 *  ourselves here for this new one that is now complete.
 *
 * Parameters:
 *  pIStorage       LPSTORAGE of the new storage in which we live.
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::SaveCompleted(LPSTORAGE pIStorage)
    {
    HRESULT     hr;
    LPSTREAM    pIStream;

    //Must be called in no-scribble or hands-off state
    if (!(PSSTATE_ZOMBIE==m_psState || PSSTATE_HANDSOFF==m_psState))
        return ResultFromScode(E_UNEXPECTED);

    //If we're coming from Hands-Off, we'd better get a storage
    if (NULL==pIStorage && PSSTATE_HANDSOFF==m_psState)
        return ResultFromScode(E_UNEXPECTED);

    /*
     * If pIStorage is NULL, then we don't need to do anything
     * since we already have all the pointers we need for Save.
     * Otherwise we have to release any held pointers and
     * reinitialize them from pIStorage.
     */

    if (NULL!=pIStorage)
        {
        hr=pIStorage->OpenStream(SZSTREAM, 0, STGM_DIRECT
            | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0
            , &pIStream);

        if (FAILED(hr))
            return hr;

        if (NULL!=m_pObj->m_pIStream)
            m_pObj->m_pIStream->Release();

        m_pObj->m_pIStream=pIStream;

        if (NULL!=m_pObj->m_pIStorage)
            m_pObj->m_pIStorage->Release();

        m_pObj->m_pIStorage=pIStorage;
        m_pObj->m_pIStorage->AddRef();
        }

    m_psState=PSSTATE_SCRIBBLE;
    return NOERROR;
    }





/*
 * CImpIPersistStorage::HandsOffStorage
 *
 * Purpose:
 * Purpose:
 *  Instructs the object that another agent is interested in having
 *  total access to the storage we might be hanging on to from
 *  InitNew or SaveCompleted.  In this case we must release our hold
 *  and await another call to SaveCompleted before we have a hold
 *  again.  Therefore we cannot read or write after this call until
 *  SaveCompleted.
 *
 *  Situations where this might happen arise in compound document
 *  scenarios where this object might be in-place active but the
 *  application wants to rename and commit the root storage.
 *  Therefore we are asked to close our hold, let the container
 *  party on the storage, then call us again later to tell us the
 *  new storage we can hold.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or a general error value.
 */

STDMETHODIMP CImpIPersistStorage::HandsOffStorage(void)
    {
    /*
     * Must come from scribble or no-scribble.  A repeated call
     * to HandsOffStorage is an unexpected error (bug in client).
     */
    if (PSSTATE_UNINIT==m_psState || PSSTATE_HANDSOFF==m_psState)
        return ResultFromScode(E_UNEXPECTED);

    //Release held pointers
    if (NULL!=m_pObj->m_pIStream)
        {
        m_pObj->m_pIStream->Release();
        m_pObj->m_pIStream=NULL;
        }

    if (NULL!=m_pObj->m_pIStorage)
        {
        m_pObj->m_pIStorage->Release();
        m_pObj->m_pIStorage=NULL;
        }

    m_psState=PSSTATE_HANDSOFF;
    return NOERROR;
    }
