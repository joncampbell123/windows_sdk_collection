//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include <streams.h>
#include <pullpin.h>
#include <alloc.h>
#include <splitter.h>

/*  Ignore warnings about this pointers being used in member initialization
    lists
*/
#pragma warning(disable:4355)

/*  Splitter output pin methds */

/*  -- Constructor -- */
CSplitterOutputPin::CSplitterOutputPin(
        CBaseSplitterFilter *pFilter,
        HRESULT *phr,
        LPCWSTR pName) :
    CBaseOutputPin(
        NAME("CSplitterOutputPin"),
        pFilter,
        &pFilter->m_csFilter,
        phr,
        pName),
    m_Notify(this),
    m_bDiscontinuity(FALSE),
    m_pOutputQueue(NULL),
    m_lTypes(NAME("CSplitterFilter::m_lTypes"))
{
}

CSplitterOutputPin::~CSplitterOutputPin()
{
    while (m_lTypes.GetCount() != 0) {
        delete m_lTypes.RemoveHead();
    }
}

/* Override revert to normal ref counting
   These pins cannot be finally Release()'d while the input pin is
   connected */

STDMETHODIMP_(ULONG)
CSplitterOutputPin::NonDelegatingAddRef()
{
    return CUnknown::NonDelegatingAddRef();
}


/* Override to do normal ref counting */

STDMETHODIMP_(ULONG)
CSplitterOutputPin::NonDelegatingRelease()
{
    return CUnknown::NonDelegatingRelease();
}

// override this to set the buffer size and count. Return an error
// if the size/count is not to your liking.
HRESULT CSplitterOutputPin::DecideBufferSize(
    IMemAllocator * pAlloc,
    ALLOCATOR_PROPERTIES * pProp
)
{
    pProp->cBuffers = 100;
    pProp->cbBuffer = 65536;            /* Don't care about size */
    pProp->cbAlign = 1;
    pProp->cbPrefix = 0;
    ALLOCATOR_PROPERTIES propActual;
    return pAlloc->SetProperties(pProp, &propActual);
}

//
//  Override DecideAllocator because we insist on our own allocator since
//  it's 0 cost in terms of bytes
//
HRESULT CSplitterOutputPin::DecideAllocator(
    IMemInputPin *pPin,
    IMemAllocator **ppAlloc
)
{
    HRESULT hr = InitAllocator(ppAlloc);
    if (SUCCEEDED(hr)) {
        ALLOCATOR_PROPERTIES propRequest;
        ZeroMemory(&propRequest, sizeof(propRequest));
        hr = DecideBufferSize(*ppAlloc, &propRequest);
        if (SUCCEEDED(hr)) {
            // tell downstream pins that modification
            // in-place is not permitted
            hr = pPin->NotifyAllocator(*ppAlloc, TRUE);
            if (SUCCEEDED(hr)) {
                return NOERROR;
            }
        }
    }

    /* Likewise we may not have an interface to release */

    if (*ppAlloc) {
        (*ppAlloc)->Release();
        *ppAlloc = NULL;
    }
    return hr;
}

// override this to control the connection
// We use the subsample allocator derived from the input pin's allocator
HRESULT CSplitterOutputPin::InitAllocator(IMemAllocator **ppAlloc)
{
    ASSERT(m_pAllocator == NULL);
    HRESULT hr = NOERROR;

    *ppAlloc = new CSubAllocator(NULL, &hr, Filter()->Allocator());
    if (*ppAlloc == NULL) {
        return E_OUTOFMEMORY;
    }

    if (FAILED(hr)) {
        delete *ppAlloc;
        *ppAlloc = NULL;
        return hr;
    }
    /* Get a reference counted IID_IMemAllocator interface */
    (*ppAlloc)->AddRef();
    return NOERROR;
}

//  Check if we accept a media type - only accept 1 for now
HRESULT CSplitterOutputPin::CheckMediaType(const CMediaType *pmt)
{
    POSITION pos = m_lTypes.GetHeadPosition();
    while (pos) {
        CMediaType *pmtList = m_lTypes.GetNext(pos);
        if (*pmtList == *pmt) {
            return S_OK;
        }
    }
    return S_FALSE;
}

// returns the preferred formats for a pin
HRESULT CSplitterOutputPin::GetMediaType(
    int iPosition,
    CMediaType *pMediaType
)
{
    POSITION pos = m_lTypes.GetHeadPosition();
    while (pos) {
        CMediaType *pmtList = m_lTypes.GetNext(pos);
        if (iPosition-- == 0) {
            *pMediaType = *pmtList;
            return S_OK;
        }
    }
    return VFW_S_NO_MORE_ITEMS;
}

/*  Add a media type that's supported */
HRESULT CSplitterOutputPin::AddMediaType(
    CMediaType const *pmt
)
{
    CMediaType *pmtNew = new CMediaType(*pmt);
    if (pmtNew) {
        if (m_lTypes.AddTail(pmtNew)) {
            return S_OK;
        }
    }
    delete pmtNew;
    return E_OUTOFMEMORY;
}

/*  Output pin - Deliver a sample */
HRESULT CSplitterOutputPin::SendSample(
    const BYTE * pbData,
    LONG lData,
    REFERENCE_TIME rtStart,
    BOOL bSync
)
{
    /*  Get a sample from the allocator */
    if (!IsConnected()) {
        return S_FALSE;
    }
    CMediaSample *pSample = Allocator()->GetSample((PBYTE)pbData, lData);
    ASSERT(pSample != NULL);
    if (bSync) {
        REFERENCE_TIME rtStop = rtStart + 1;
        pSample->SetTime(&rtStart, &rtStop);

        /*  Allow for some decoders that only take not of sync points */
        pSample->SetSyncPoint(TRUE);
    }

    /*  First send newsegment etc if discontinuity */
    if (m_bDiscontinuity) {
        m_bDiscontinuity = FALSE;
        pSample->SetDiscontinuity(TRUE);

        /*  HACK - fix this for seeking */
        m_pOutputQueue->NewSegment(0, 0x7F00000000000000, 1.0);
    }

    /*  Output queue will release the sample */
    return m_pOutputQueue->Receive(pSample);
}


/*  Base splitter class methods */


/* -- Constructor -- */

CBaseSplitterFilter::CBaseSplitterFilter(
    TCHAR *pName,
    LPUNKNOWN pUnk,
    REFCLSID rclsid,
    HRESULT *phr) :

    CBaseFilter(pName, pUnk, &m_csFilter, rclsid, phr),
    m_OutputPins(NAME("CBaseSplitterFilter::m_OutputPins")),
    m_Notify(this),
    m_pInput(NULL),
    m_pParser(NULL)
{
}

/* -- Destructor -- */

CBaseSplitterFilter::~CBaseSplitterFilter()
{
    ASSERT(m_State == State_Stopped);

    /* -- Destroy all pins */
    DestroyInputPin();
    DestroyOutputPins();
}

//
//  Override Pause() so we can prevent the input pin from starting
//  the puller before we're ready (ie have exited stopped state)
//
//  Starting the puller in Active() caused a hole where the first
//  samples could be rejected becase we seemed to be in 'stopped'
//  state
//
STDMETHODIMP
CBaseSplitterFilter::Pause()
{
    CAutoLock lockFilter(&m_csFilter);
    if (m_State == State_Stopped) {
        // and do the normal inactive processing
        POSITION pos = m_OutputPins.GetHeadPosition();
        while (pos) {
            CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
            if (pPin->IsConnected()) {
                pPin->Active();
            }
        }

        CAutoLock lockStreaming(&m_csStream);

        //  Activate our input pin only if we're connected
        if (m_pInput->IsConnected()) {
            m_pInput->Active();
        }
        m_State = State_Paused;

        //  Initialize our state
        ResetAllocatorAndParser();
    } else {
        m_State = State_Paused;
    }
    return S_OK;
}

/*  Stop the filter

    We override this for efficiency and so that we can manage the
    locking correctly
*/
STDMETHODIMP CBaseSplitterFilter::Stop()
{
    // must get this one first - it serializes state changes
    CAutoLock lockFilter(&m_csFilter);
    if (m_State == State_Stopped) {
        return NOERROR;
    }

    // decommit the input pin or we can deadlock
    if (m_pInput != NULL) {
        m_pInput->Inactive();
    }

    // now hold the Receive critsec to prevent further Receive and EOS calls,
    CAutoLock lockStreaming(&m_csStream);

    // and do the normal inactive processing
    POSITION pos = m_OutputPins.GetHeadPosition();
    while (pos) {
        CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
        if (pPin->IsConnected()) {
            pPin->Inactive();
        }
    }
    ResetAllocatorAndParser();
    m_State = State_Stopped;
    return S_OK;
}

/*  -- Return number of pins */
int CBaseSplitterFilter::GetPinCount()
{
    CAutoLock lck(&m_csPins);
    return (m_pInput != NULL ? 1 : 0) +
           m_OutputPins.GetCount();
}

/*  -- Return a given pin -- */
CBasePin *CBaseSplitterFilter::GetPin(int iPin)
{
    CAutoLock lck(&m_csPins);
    if (iPin == 0 && m_pInput != NULL) {
        return m_pInput;
    }
    if (m_pInput != NULL) {
        iPin--;
    }
    if (iPin < 0 || iPin >= m_OutputPins.GetCount()) {
        return NULL;
    }
    POSITION pos = m_OutputPins.GetHeadPosition();
    while (iPin-- > 0) {
        m_OutputPins.GetNext(pos);
    }
    return m_OutputPins.GetNext(pos);
}

/*  -- Add an output pin -- */
BOOL CBaseSplitterFilter::AddOutputPin(CSplitterOutputPin *pOutputPin)
{
    CAutoLock lckFilter(&m_csFilter);
    CAutoLock lckPins(&m_csPins);
    ASSERT(m_State == State_Stopped);
    IncrementPinVersion();
    pOutputPin->AddRef();
    if (m_OutputPins.AddTail(pOutputPin) == NULL) {
        delete pOutputPin;
        return FALSE;
    }
    return TRUE;
}

/*  -- Destroy the input pin -- */
void CBaseSplitterFilter::DestroyInputPin()
{
    delete m_pInput;
    m_pInput = NULL;
}

/*  -- Delete the output pins -- */
void CBaseSplitterFilter::DestroyOutputPins()
{
    CAutoLock lckFilter(&m_csFilter);
    CAutoLock lckPins(&m_csPins);
    ASSERT(m_State == State_Stopped);
    for (;;) {
        CSplitterOutputPin *pPin = m_OutputPins.RemoveHead();
        if (pPin == NULL) {
            break;
        }
        IncrementPinVersion();

        //  Disconnect if necessary
        IPin *pPeer = pPin->GetConnected();
        if (pPeer != NULL) {
            pPeer->Disconnect();
            pPin->Disconnect();
        }
        pPin->Release();
    }
}

/*  Notify to create a new stream
    Called back through CParseNotify
*/
HRESULT CBaseSplitterFilter::CreateStream(
    LPCWSTR         pszName,
    CStreamNotify **ppStreamNotify
)
{
    HRESULT hr = S_OK;
    CSplitterOutputPin *pPin =
        new CSplitterOutputPin(this, &hr, pszName);
    if (FAILED(hr)) {
        delete pPin;
        return hr;
    }
    if (pPin == NULL) {
        return E_OUTOFMEMORY;
    }

    if (!AddOutputPin(pPin)) {
        delete pPin;
        return E_OUTOFMEMORY;
    }
    *ppStreamNotify = pPin->GetNotify();
    return S_OK;
}

HRESULT CBaseSplitterFilter::CompleteConnect(IPin *pReceivePin)
{
    m_pParser = CreateParser(&m_Notify, InputPin()->MediaType());
    if (m_pParser == NULL) {
        return E_OUTOFMEMORY;
    }
    /*  Create a reader to initialize the parser */
    CParseReaderFromAsync rdr(InputPin()->Reader());

    HRESULT hr = m_pParser->Init(&rdr);
    if (SUCCEEDED(hr)) {
        ALLOCATOR_PROPERTIES Props, Actual;

        /*  Our allocator doesn't accumulate alignment correctly
            so use the alignment currently set
        */
        Allocator()->GetProperties(&Props);
        m_pParser->GetSizeAndCount(&Props.cbBuffer, &Props.cBuffers);
        ((IMemAllocator *)Allocator())->SetProperties(&Props, &Actual);
    }
    return hr;
}

/*  Process some data */
HRESULT CBaseSplitterFilter::Receive(IMediaSample *pSample)
{
    /*  Notify our allocator about how much data we've got,
        Call the parser to eat the data,
        eat the data that was parsed
    */

    Allocator()->AddBuffer((CMediaSample *)pSample);

    /*  Now eat the data */
    LONG lValid;
    PBYTE pbValid = Allocator()->GetValid(&lValid);
    LONG lProcessed;
    HRESULT hr = m_pParser->Process(pbValid, lValid, &lProcessed);
    if (S_OK != hr) {
        /*  If something goes wrong do error processing */
        return hr;
    }

    Allocator()->Advance(lProcessed);

    /*  Flush the output queues */
    POSITION pos = m_OutputPins.GetHeadPosition();
    while (pos) {
        CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
        pPin->SendAnyway();
    }
    return S_OK;
}

/*  Send EndOfStream */
void CBaseSplitterFilter::EndOfStream()
{
    ResetAllocatorAndParser();
    POSITION pos = m_OutputPins.GetHeadPosition();
    while (pos) {
        CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
        if (pPin->IsConnected()) {
            pPin->DeliverEndOfStream();
        }
    }
}

/*  Send BeginFlush() */
HRESULT CBaseSplitterFilter::BeginFlush()
{
    POSITION pos = m_OutputPins.GetHeadPosition();

    while (pos) {
        CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
        if (pPin->IsConnected()) {
            pPin->DeliverBeginFlush();
        }
    }
    return S_OK;
}

/*  Send EndFlush() */
HRESULT CBaseSplitterFilter::EndFlush()
{
    ResetAllocatorAndParser();
    POSITION pos = m_OutputPins.GetHeadPosition();
    while (pos) {
        CSplitterOutputPin *pPin = m_OutputPins.GetNext(pos);
        if (pPin->IsConnected()) {
            pPin->DeliverEndFlush();
        }
    }
    return S_OK;
}

/*  Break connection on the input pin */
void CBaseSplitterFilter::BreakConnect()
{
    delete m_pParser;
    m_pParser = NULL;
    DestroyOutputPins();
}

/*  -- Input pin methods -- */

/*  Constructor */
CSplitterInputPin::CSplitterInputPin(
    CBaseSplitterFilter *pFilter,
    HRESULT *phr
) : CBaseInputPin(NAME("CSplitterInputPin"),
             pFilter,
             &pFilter->m_csFilter,
             phr,
             L"Input"),
    m_puller(this)
{
}

/*  Connection stuff */
HRESULT CSplitterInputPin::CheckConnect(IPin *pPin)
{
    HRESULT hr = CBaseInputPin::CheckConnect(pPin);
    if (FAILED(hr)) {
        return hr;
    }

    /*  Create our allocator */
    ASSERT(m_pAllocator == NULL);

    m_pAllocator = new CSequentialAllocator(
                           NULL, // No owner - allocators are separate objects
                           &hr);
    if (m_pAllocator == NULL) {
        return E_OUTOFMEMORY;
    }
    if (FAILED(hr)) {
        return hr;
    }

    /*  The base classes expect the allocator to be AddRef'd */
    m_pAllocator->AddRef();

    // BUGBUG CPullPin might change the allocator and not tell us!
    hr = m_puller.Connect(pPin, m_pAllocator, TRUE);
    if (FAILED(hr)) {
        return hr;
    }
    return hr;
}

/*  Break connection - filter already locked by caller */
HRESULT CSplitterInputPin::BreakConnect()
{
    Filter()->BreakConnect();
    m_puller.Disconnect();
    if (m_pAllocator) {
        m_pAllocator->Release();
        m_pAllocator = NULL;
    }
    return CBaseInputPin::BreakConnect();
}

/*  End of stream */
STDMETHODIMP CSplitterInputPin::EndOfStream()
{
    CAutoLock lck(&Filter()->m_csStream);
    HRESULT hr = CheckStreaming();
    if (FAILED(hr)) {
        return hr;
    }

    /*  Forward to the output pins */
    Filter()->EndOfStream();
    return S_OK;
}

/*  Forward BeginFlush() and EndFlush() to filter */
STDMETHODIMP CSplitterInputPin::BeginFlush()
{
    CAutoLock lck(m_pLock);
    CBaseInputPin::BeginFlush();
    Filter()->BeginFlush();
    return S_OK;
}
STDMETHODIMP CSplitterInputPin::EndFlush()
{
    CAutoLock lck(&Filter()->m_csStream);
    CBaseInputPin::EndFlush();
    Filter()->EndFlush();
    return S_OK;
}

/*  Receive - forward to filter */
STDMETHODIMP CSplitterInputPin::Receive(IMediaSample *pSample)
{
    CAutoLock lck(&Filter()->m_csStream);
    HRESULT hr = CheckStreaming();
    if (FAILED(hr)) {
        return hr;
    }
    hr = Filter()->Receive(pSample);
    if (S_OK != hr) {
        NotifyError(hr);
    }
    return hr;
}

HRESULT CSplitterInputPin::Active()
{
    /*  If we're not seekable then just seek to start for now */
    REFERENCE_TIME rtDuration;
    m_puller.Duration(&rtDuration);
    m_puller.Seek(0, rtDuration);
    HRESULT hr = m_puller.Active();
    if (FAILED(hr)) {
        return hr;
    }
    return CBaseInputPin::Active();
}

HRESULT CSplitterInputPin::Inactive()
{
    m_puller.Inactive();
    return CBaseInputPin::Inactive();
}


/*  forward connection stuff to filter */
HRESULT CSplitterInputPin::CheckMediaType(const CMediaType *pmt)
{
    return Filter()->CheckInputType(pmt);
}
HRESULT CSplitterInputPin::CompleteConnect(IPin *pReceivePin)
{
    return Filter()->CompleteConnect(pReceivePin);
}

