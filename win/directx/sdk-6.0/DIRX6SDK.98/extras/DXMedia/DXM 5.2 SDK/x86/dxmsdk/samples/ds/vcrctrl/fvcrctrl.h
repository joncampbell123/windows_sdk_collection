//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
// filename: fvcrctrl.h 
//
// VCR Control filter - CBaseFilter Derived
//
// This is a prototype "VCR" filter.  Depending on the detected VCR, 
// It has 5 or 6 pins - 2 or 3 Video inputs, 2 Audio inputs
// and one Timecode outout (a timecode input pin would be useless).
// It's a bit different than the standard source, transform,
// and rendering filters because it has both input and output
// pins with fairly unrelated behaviors.
//
// It implements the following External Device interfaces:
//
//	IAMExtDevice
//	IAMPhysicalPinInfo
//	IAMExtTransport
//	IAMTimecodeReader
//
// The filter also has a couple of property pages that put a simple
// user interface on many of the methods and properties of
// the supported interfaces.

class COutStream;	// The class that will handle the timecode output pin
//*************************************************/
// CVcrFilter
//
class CVcrFilter : public CBaseFilter, public CAMExtDevice,
 public CAMExtTransport, public CAMTcr, public CPersistStream,
 public ISpecifyPropertyPages 
{
	friend class COutStream;		// so they can lock
	friend class CVcrTCOut;
public:

    static CUnknown *CreateInstance(LPUNKNOWN punk, HRESULT *phr);

	// we replace the DECLARE_IUNKNOWN macro so we can resolve
	// ambiguous references due to multiple CUnknowns
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        return CBaseFilter::GetOwner()->QueryInterface(riid,ppv);      
    };                                                   
    STDMETHODIMP_(ULONG) AddRef() {                      
        return CBaseFilter::GetOwner()->AddRef();                     
    };                                                   
    STDMETHODIMP_(ULONG) Release() {                     
        return CBaseFilter::GetOwner()->Release();                    
    };
	HINSTANCE	LoadOLEAut32();

    //
    // --- CBaseFilter Overrides --
    //
	
    int GetPinCount();
    CBasePin * GetPin(int n);

	// CPersistStream overrides
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int SizeMax();
    STDMETHODIMP GetClassID(CLSID *pClsid);
	
	// Utilities
	HRESULT     AddPin(CBasePin *);
    HRESULT     RemovePin(CBasePin *);
	CCritSec*	pLock(void) { return &m_StateLock; }// provide our 
													//  critical section
	
	// override state changes to allow derived Device Control filter
    // to control its own start/stop
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();
    STDMETHODIMP Run(REFERENCE_TIME tStart);

	// Basic COM - used here to reveal our property interface.
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	// --- ISpecifyPropertyPages ---
	// return our property pages
    STDMETHODIMP GetPages(CAUUID * pPages);
    
private:

    CVcrFilter(TCHAR *tszName, LPUNKNOWN punk, CLSID clsid, HRESULT *phr);
	~CVcrFilter();
	
	int m_iPins;		// The number of pins on this filter. 
						//  Updated by COutStream and CVcrInputPin 
						//  constructors & destructors.
protected:

    CBasePin **m_paStreams;		// the pins on this filter.
	CCritSec m_StateLock;		// Lock this to serialize function accesses 
								//  to the filter state
};

//*************************************************/
// COutStream
//
// Use this class to manage a stream of data that comes
// from a derived pin.
// Uses a worker thread to put data on the pin.
//
class COutStream : public CAMThread, public CBaseOutputPin
{
public:

    COutStream(TCHAR *pObjectName,
                  HRESULT *phr,
                  CVcrFilter *pParent,
                  LPCWSTR pName);

    virtual ~COutStream(void);  // virtual destructor ensures derived class 
								//  destructors are called too.

	// thread commands
    virtual enum Command {CMD_INIT, CMD_PAUSE, CMD_RUN, CMD_STOP, CMD_EXIT};
    HRESULT Init(void) { return CallWorker(CMD_INIT); }
    HRESULT Exit(void) { return CallWorker(CMD_EXIT); }
    HRESULT Run(void) { return CallWorker(CMD_RUN); }
    HRESULT Pause(void) { return CallWorker(CMD_PAUSE); }
    HRESULT Stop(void) { return CallWorker(CMD_STOP); }

protected:

    CVcrFilter *m_pFilter;			// The parent of this stream
	HANDLE m_hAdviseEvent;			// for an advise event
	IReferenceClock *m_pRefClock;	// this graph's sync source
	
    // *
    // * Data Source
    // *
    // * The following three functions: FillBuffer, OnThreadCreate/Destroy, are
    // * called from within the ThreadProc. They are used in the creation of
    // * the media samples this pin will provide
    // *

    // Override this to provide the worker thread a means
    // of processing a buffer
    virtual HRESULT FillBuffer(IMediaSample *pSamp) PURE;

    // Called as the thread is created/destroyed - use to perform
    // jobs such as start/stop streaming mode
    // If OnThreadCreate returns an error the thread will exit.
    virtual HRESULT OnThreadCreate(void) {return NOERROR;};
    virtual HRESULT OnThreadDestroy(void) {return NOERROR;};
    virtual HRESULT OnThreadStartPlay(void) {return NOERROR;};

    // *
    // * Worker Thread
    // *

    HRESULT Active(void);    // Starts up the worker thread
    HRESULT Inactive(void);  // Exits the worker thread.

    Command GetRequest(void) { return (Command) CAMThread::GetRequest(); }
    BOOL    CheckRequest(Command *pCom) {
			return CAMThread::CheckRequest( (DWORD *) pCom); }

    // override these if you want to add thread commands 
	// (and the T/C stream will)
    virtual DWORD ThreadProc(void);  				// the thread function
    virtual HRESULT DoBufferProcessingLoop(void);   // the loop executed 
													//  whilst running


    // *
    // * MEDIA_TYPE support
    // *

    // Override this - we support multiple media types
    virtual HRESULT CheckMediaType( const CMediaType *pMediaType) PURE;
};

//*************************************************/
// CVcrTCOut
//
// This filter has an output pin which can output timecode samples
// as either a binary sample or as displayable text.
//
class CVcrTCOut : public COutStream
{
private:

	CVcrFilter *m_pFilter;			// the external device filter that owns us
    CCritSec    m_cSharedState;     // use this to lock access to 
									//  m_rtSampleTime and m_pFilter which are 
									//  shared with the worker thread.
    CRefTime    m_rtSampleTime;		// The time to be stamped on each sample
	CTimecode	*m_ctc;				// need this for data conversions
	static LONGLONG m_llLastTime;	// the last time timecode was updated
	int m_mtype;					// 0 for text, 1 for Aux Data
	
public:

	CVcrTCOut(
		HRESULT *phr,
		CVcrFilter *pParent,
		LPCWSTR pPinName);

	~CVcrTCOut();

	BOOL ReadyToStop(void) {return FALSE;}

    // stuff a text buffer with the current format
    HRESULT FillBuffer(IMediaSample *pms);

    // ask for buffers of the size appropriate to the agreed media type.
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
							ALLOCATOR_PROPERTIES *pRequest);

    // verify we can handle this format
    HRESULT CheckMediaType(const CMediaType *pMediaType);

	// we handle multiple media types
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

    // resets the stream time to zero.
    HRESULT OnThreadCreate(void);
};
//*************************************************/
//
//	Base Hardware Input Pin Class
//
// It might be desirable to add hardware output pins in a similar manner
// as the following input pins if, for example, an external device
// has switchable outputs.
// 
class CVcrInputPin : public CBaseInputPin, public CAMPhysicalPinInfo
{
public:
    CVcrInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrInputPin();

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** pv);

    // we aren't supposed to connect to anybody
    HRESULT CheckMediaType(const CMediaType*) {
	return E_FAIL;
    }
    
	// CAMPhysicalPinInfo's method - the derived input pins must implement
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType){
		return E_NOTIMPL;}

private:
    CVcrFilter * m_pFilter;	// parent
	CCritSec m_StateLock;	// Lock this to serialize function accesses
};

//*************************************************/
//
//	1st of 4 (or 5) input pins
//
class CVcrVideoCompInputPin : public CVcrInputPin
{
public:
    CVcrVideoCompInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrVideoCompInputPin();
      
	// CAMPhysicalPinInfo's method - we must return our physical pin info
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType);
};

//*************************************************/
//
//	2nd of 4 (or 5) input pins
//
class CVcrVideoSInputPin : public CVcrInputPin
{
public:
    CVcrVideoSInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrVideoSInputPin();
      
	// CAMPhysicalPinInfo's method - we must return our physical pin info
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType);
};

//*************************************************/
//
//	3rd of 4 (or 5) input pins
//
class CVcrAudioLineInputPin : public CVcrInputPin
{
public:
    CVcrAudioLineInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrAudioLineInputPin();
      
	// CAMPhysicalPinInfo's method - we must return our physical pin info
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType);
};

//*************************************************/
//
//	4th of 4 (or 5) input pins
//
class CVcrAudioMicInputPin : public CVcrInputPin
{
public:
    CVcrAudioMicInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrAudioMicInputPin();
      
	// CAMPhysicalPinInfo's method - we must return our physical pin info
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType);
};

//*************************************************/
//
//	Optional 5th input pin
//
class CVcrVideoBlackInputPin : public CVcrInputPin
{
public:
    CVcrVideoBlackInputPin(
        TCHAR *pObjectName,
        CVcrFilter *pFilter,
	    HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVcrVideoBlackInputPin();
      
	// CAMPhysicalPinInfo's method - we must return our physical pin info
	STDMETHODIMP GetPhysicalType(long *pType, LPOLESTR * ppszType);
};

// eof fvcrctrl.h
