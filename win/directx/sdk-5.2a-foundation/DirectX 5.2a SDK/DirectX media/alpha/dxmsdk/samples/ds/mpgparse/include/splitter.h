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
/*  Parser class - this class defines the object that actually splits
    out the data
*/

/*  CBaseParser class

    This is the object that determines the nature of the data
    and actually splits the stream
*/

/*  Simple stream reader class */
class CParseReader
{
public:
    /*  Set the position */
    virtual HRESULT Length(LONGLONG *pLength) = 0;
    virtual HRESULT SetPointer(LONGLONG) = 0;
    virtual HRESULT Read(PBYTE pbData, DWORD cbData) = 0;
};

/*  Parsing reader from CAsyncReader */
class CParseReaderFromAsync : public CParseReader
{
public:
    CParseReaderFromAsync(IAsyncReader *pRdr) :
        m_pReader(pRdr), m_llPos(0) {};
    HRESULT Length(LONGLONG *pLength)
    {
        LONGLONG llAvailable;
        return m_pReader->Length(pLength, &llAvailable);
    }
    HRESULT SetPointer(LONGLONG llPos)
    {
        m_llPos = 0;
        return S_OK;
    }
    HRESULT Read(PBYTE pbData, DWORD cbData)
    {
        HRESULT hr = m_pReader->SyncRead(m_llPos, (LONG)cbData, pbData);
        if (S_OK == hr) {
            m_llPos += cbData;
        }
        return hr;
    }

private:
    IAsyncReader *m_pReader;
    LONGLONG      m_llPos;
};
class CParserNotify;
class CBaseParser
{
public:
    /*  Instantiate a parser

        pNotify - to call back the calling object to create streams etc
    */
    CBaseParser(CParserNotify *pNotify,
                HRESULT *phr) : m_pNotify(pNotify) {};

    /*  Initialize a parser

        pmt     - type of stream if known - can be NULL
        pRdr    - way to read the source medium - can be NULL
    */
    virtual HRESULT Init(
        CParseReader *pRdr
    ) = 0;


    /*  Get the size and count of buffers preferred based on the
        actual content
    */
    virtual void GetSizeAndCount(LONG *plSize, LONG *plCount) = 0;

    /*  Call this to reinitialize for a new stream */
    virtual void StreamReset() = 0;

    /*  Call this to pass new stream data :

        pbData        - pointer to data
        lData         - length of data
        plProcessed   - Amount of data consumed
    */
    virtual HRESULT Process(
        const BYTE *pbData,
        LONG lData,
        LONG *plProcessed
    ) = 0;
protected:
    CParserNotify * const m_pNotify;
};

/*  Parser calls back to create streams and spit out
    buffers
*/
class CStreamNotify;
class CParserNotify
{
public:

    /*  Create an output stream with type *pmt, notifications
        to this stream passed to the **pStreamNotify object
    */
    virtual HRESULT CreateStream(
        LPCWSTR pszName,
        CStreamNotify **pStreamNotify) = 0;

};


class CStreamNotify
{
public:
    virtual HRESULT SendSample(
        const BYTE *pbData,
        LONG lData,
        REFERENCE_TIME rtStart,
        BOOL bSync
    ) = 0;

    /*  Add a media type that's supported */
    virtual HRESULT AddMediaType(CMediaType const *pmt) = 0;

    /*  Return the current type */
    virtual void CurrentMediaType(AM_MEDIA_TYPE *pmt) = 0;
};

/*  Splitter filter base classes */


/*  Design :

    A splitter filter will have 1 input pin and multiple output pins

    CBaseSplitterFilter defines a base filter with 1 input pin and
    a list of output pins.

    This base class provides for pin enumeration, correct distribution
    of EndOfStream and handling of errors.

    The object structure is :


                            IsA

    CBaseSplitterFilter  <--------  CBaseFilter


    Allocators:

        This class relies on use CSequentialAllocator

        The input pin is designed around CPullPin which hooks up
        to IAsyncReader on the upstream output pin
*/

class CBaseSplitterFilter;
class CBaseParser;

/*  Input pin stuff

    The input pin deletes all the output pins when it's disconnected

    On connection the output pins are created based on the media type
    and possibly on the file content

    The base class handles things like flushing and end of stream
*/

/*  Special output pin type to handle lifetime */
class CSplitterOutputPin : public CBaseOutputPin
{

public:

    //  Constructor
    CSplitterOutputPin(
        CBaseSplitterFilter *pFilter,
        HRESULT *phr,
        LPCWSTR pName);

    ~CSplitterOutputPin();

    //  CUnknown methods
    STDMETHODIMP_(ULONG) NonDelegatingAddRef();
    STDMETHODIMP_(ULONG) NonDelegatingRelease();

    //  CBaseOutputPin methods - we just override these to do
    //  our own hack allocator

    // override this to set the buffer size and count. Return an error
    // if the size/count is not to your liking
    virtual HRESULT DecideBufferSize(
                        IMemAllocator * pAlloc,
                        ALLOCATOR_PROPERTIES * pProp);

    // negotiate the allocator and its buffer size/count
    // calls DecideBufferSize to call SetCountAndSize
    virtual HRESULT DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);

    // override this to control the connection
    virtual HRESULT InitAllocator(IMemAllocator **ppAlloc);

    // Check the media type proposed
    HRESULT CheckMediaType(const CMediaType *);

    // returns the preferred formats for a pin
    HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);

    // Send sample generated by parser
    HRESULT SendSample(
        const BYTE *pbData,
        LONG lData,
        REFERENCE_TIME rtStart,
        BOOL bSync
    );

    /*  Add a media type */
    HRESULT AddMediaType(
        CMediaType const *pmt
    );


    HRESULT DeliverEndOfStream()
    {
        m_pOutputQueue->EOS();
        return S_OK;
    }

    //  Delete our output queue on inactive
    HRESULT Inactive()
    {
        HRESULT hr = CBaseOutputPin::Inactive();
        if (FAILED(hr)) {
            return hr;
        }

        delete m_pOutputQueue;
        m_pOutputQueue = NULL;
        return S_OK;
    }

    /*  Wrapper to call output queue to flush samples */
    void SendAnyway()
    {
        if (NULL != m_pOutputQueue) {
            m_pOutputQueue->SendAnyway();
        }
    }

    //  Override Active and EndFlush to set the discontinuity flag
    HRESULT Active()
    {
        m_bDiscontinuity = TRUE;
        /*  If we're not connected we don't participate so it's OK */
        if (!IsConnected()) {
            return S_OK;
        }

        HRESULT hr = CBaseOutputPin::Active();
        if (FAILED(hr)) {
            return hr;
        }

        /*  Create our batch list */
        ASSERT(m_pOutputQueue == NULL);

        hr = S_OK;
        m_pOutputQueue = new COutputQueue(GetConnected(), // input pin
                                          &hr,            // return code
                                          FALSE,          // Auto detect
                                          TRUE,           // ignored
                                          50,             // batch size
                                          TRUE,           // exact batch
                                          50);            // queue size
        if (m_pOutputQueue == NULL) {
            return E_OUTOFMEMORY;
        }
        if (FAILED(hr)) {
            delete m_pOutputQueue;
            m_pOutputQueue = NULL;
        }
        return hr;
    }
    HRESULT DeliverBeginFlush()
    {
        /*  We're already locked via the input pin */
        m_pOutputQueue->BeginFlush();
        return S_OK;
    }
    HRESULT DeliverEndFlush()
    {
        /*  We're already locked via the input pin */
        m_bDiscontinuity = TRUE;
        m_pOutputQueue->EndFlush();
        return S_OK;
    }

    //  Get our notify object
    CStreamNotify *GetNotify()
    {
        return &m_Notify;
    }

protected:
    //  Get a pointer to our allocator
    CSubAllocator *Allocator()
    {
        return (CSubAllocator *)m_pAllocator;
    }

    //  Get a properly cast pointer to our filter
    CBaseSplitterFilter *Filter()
    {
        return (CBaseSplitterFilter *)m_pFilter;
    }


protected:
    //  Stream notify stuff
    class CImplStreamNotify : public CStreamNotify
    {
    public:
        CImplStreamNotify(CSplitterOutputPin *pPin) : m_pPin(pPin) {}
        HRESULT SendSample(
            const BYTE *pbData,
            LONG lData,
            REFERENCE_TIME rtStart,
            BOOL bSync
        )
        {
            return m_pPin->SendSample(pbData, lData, rtStart, bSync);
        }

        HRESULT AddMediaType(CMediaType const *pmt)
        {
            return m_pPin->AddMediaType(pmt);
        }
        void CurrentMediaType(AM_MEDIA_TYPE *pmt)
        {
            m_pPin->ConnectionMediaType(pmt);
        }

    private:
        CSplitterOutputPin * const m_pPin;
    };

    CImplStreamNotify  m_Notify;

    //  Remember when to send NewSegment and discontinuity */
    BOOL               m_bDiscontinuity;

    //  Output queue
    COutputQueue      *m_pOutputQueue;

    //  List of media types we support
    CGenericList<CMediaType> m_lTypes;
};


/*  Base CSplitterInputPin on CPullPin
*/
class CSplitterInputPin : public CBaseInputPin
{
public:
    CSplitterInputPin(
        CBaseSplitterFilter *pFilter,
        HRESULT *phr);

    /*  NonDelegating IUnknown methods - we don't support IMemInputPin */
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv)
    {
        if (riid == IID_IMemInputPin) {
            return E_NOINTERFACE;
        }
        return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv);
    }

    /*  IPin methods */
    STDMETHODIMP EndOfStream();
    STDMETHODIMP BeginFlush();
    STDMETHODIMP EndFlush();
    STDMETHODIMP Receive(IMediaSample *pSample);

    /*  IMemInputPin methods */

    /*  Where we're told which allocator we are using */
    STDMETHODIMP NotifyAllocator(IMemAllocator *pAllocator)
    {
        if (pAllocator != (IMemAllocator *)m_pAllocator) {
            return E_FAIL;
        } else {
            return S_OK;
        }
    }

    /*  Say if we're blocking */
    STDMETHODIMP ReceiveCanBlock()
    {
        return S_FALSE;
    }


    /*  CBasePin methods */
    HRESULT BreakConnect();  //  Override to release puller
    HRESULT CheckConnect(IPin *pPin);  //  Override to connect to puller
    HRESULT Active();
    HRESULT Inactive();
    HRESULT CheckMediaType(const CMediaType *pmt);
    HRESULT CompleteConnect(IPin *pReceivePin);

    /*  Report filter from reader */
    void NotifyError(HRESULT hr)
    {
        if (FAILED(hr)) {
            m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
        }
        EndOfStream();
    };

    /*  Convenient way to get the allocator */
    CSequentialAllocator *Allocator()
    {
        return (CSequentialAllocator *)m_pAllocator;
    }

    /*  Point to our media type */
    CMediaType *MediaType()
    {
        return &m_mt;
    }

    /*  Return our async reader */
    IAsyncReader *Reader()
    {
        IAsyncReader *pReader = m_puller.GetReader();
        pReader->Release();
        return pReader;
    }

private:

    //  Get a properly case pointer to our filter
    CBaseSplitterFilter *Filter()
    {
        return (CBaseSplitterFilter *)m_pFilter;
    }

    // class to pull data from IAsyncReader if we detect that interface
    // on the output pin
    class CImplPullPin : public CPullPin
    {
        // forward everything to containing pin
        CSplitterInputPin * const m_pPin;

    public:
        CImplPullPin(CSplitterInputPin* pPin)
          : m_pPin(pPin)
        {
        };

        // forward this to the pin's IMemInputPin::Receive
        HRESULT Receive(IMediaSample* pSample) {
            return m_pPin->Receive(pSample);
        };

        // override this to handle end-of-stream
        HRESULT EndOfStream(void) {
            return m_pPin->EndOfStream();
        };

        // these errors have already been reported to the filtergraph
        // by the upstream filter so ignore them
        void OnError(HRESULT hr) {
            // ignore VFW_E_WRONG_STATE since this happens normally
            // during stopping and seeking
            if (hr != VFW_E_WRONG_STATE) {
                m_pPin->NotifyError(hr);
            }
        };

        // flush the pin and all downstream
        HRESULT BeginFlush() {
            return m_pPin->BeginFlush();
        };

        // Tell the next guy we've finished flushing
        HRESULT EndFlush() {
            return m_pPin->EndFlush();
        };

    };
    CImplPullPin m_puller;
};


class CBaseSplitterFilter : public CBaseFilter
{
friend class CSplitterOutputPin;
friend class CSplitterInputPin;

public:
    //  Constructor and destructor
    CBaseSplitterFilter(
       TCHAR *pName,
       LPUNKNOWN pUnk,
       REFCLSID rclsid,
       HRESULT *phr);
    ~CBaseSplitterFilter();

    //  IMediaFilter methods - override these to manage locking
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();

    /*  Stream control stuff */
    virtual HRESULT BeginFlush();
    virtual HRESULT EndFlush();
    virtual void    EndOfStream();
    virtual HRESULT Receive(IMediaSample *pSample);
    virtual HRESULT CheckInputType(const CMediaType *pmt) = 0;
    virtual HRESULT CompleteConnect(IPin *pReceivePin);

    //  Create the parser
    virtual CBaseParser *CreateParser(
        CParserNotify *pNotify,
        CMediaType *pType) = 0;

    //  CBaseFilter methods
    int GetPinCount();     // 0 pins
    CBasePin *GetPin(int iPin);

    /*  Add a new output pin */
    BOOL AddOutputPin(CSplitterOutputPin *pOutputPin);

    /*  Destroy the output pins */
    void DestroyOutputPins();

    /*  Destroy the input pin */
    void DestroyInputPin();

    /*  Notify to create a new stream
        Called back through CParseNotify
    */
    HRESULT CreateStream(
        LPCWSTR         pszName,
        CStreamNotify **ppStreamNotify
    );

    /*  Called when BreakConnect called on the input pin */
    virtual void BreakConnect();

protected:


    /*  Utility to get the allocator */
    CSequentialAllocator *Allocator()
    {
        return InputPin()->Allocator();
    }

    /*  Reset the states of the allocator and parser
        ready to receive a new stream
    */
    void ResetAllocatorAndParser()
    {
        Allocator()->Flush();
        m_pParser->StreamReset();
    }

    CSplitterInputPin *InputPin()
    {
        return (CSplitterInputPin *)m_pInput;
    }

protected:

    //  Our input pin - created by derived classes
    CBaseInputPin           *m_pInput;

    //  Our output pin list - Derived classes append to this
    CGenericList<CSplitterOutputPin> m_OutputPins;

    //  Filter locking
    CCritSec                 m_csFilter;

    //  Streaming lock
    CCritSec                 m_csStream;

    //  Pin database lock
    CCritSec                 m_csPins;

    //  Parser - created when we're connected
    CBaseParser             *m_pParser;

    //  Parser notify - use member variable to avoid mulitple inheritance
    class CImplParserNotify : public CParserNotify
    {
    public:
        CImplParserNotify(CBaseSplitterFilter *pFilter) : m_pFilter(pFilter) {};
        HRESULT CreateStream(
            LPCWSTR         pszName,
            CStreamNotify **ppStreamNotify
        )
        {
            return m_pFilter->CreateStream(pszName, ppStreamNotify);
        }
    private:
        CBaseSplitterFilter * const m_pFilter;

    };
    CImplParserNotify        m_Notify;
};
