//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

//
//  Video capture stream source filter
//

// - Uses the CSource/CSourceStream classes to create a video capture filter.
// - Uses the AVICap capXXX macros to capture from the first driver it finds.

// CLSID_VidCap
// {fd501040-8ebe-11ce-8183-00aa00577da1}
DEFINE_GUID(CLSID_VidCap,
0xfd501040, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00, 0xaa, 0x00, 0x57, 0x7d, 0xa1);


// align a dw to an arbitrary size
#define ALIGNUP(dw,align) ((long)(((long)(dw)+(align)-1) / (align)) * (align))

// forward declarations
class CVidOverlay;
class CVidPreview;

// The buffer sizes required for driver name and version strings
const int giDriverNameStrLen = 80;
const int giDriverVerStrLen  = 40;

class CVidStream;       // manages the output stream & pin
class CVideoBufferList; // A place to store filled buffers before we can send
                        // them downstream


//
// CVidCap
//
// The VidCap filter object. Provides the IBaseFilter/IMediaFilter interfaces.
// IPersistPropertyBag must be supported to find out what device to use
// (this filter works with many video cards)
// CPersistStream must be supported so a .GRF file with this filter in it
// can be re-created with the right device
// IAMVfwCaptureDialogs is supported to bring up the VfW capture driver
// dialog boxes.  A filter that does not work with the VfW capture drivers
// will NOT support this interface
class CVidCap : public CSource, public IPersistPropertyBag,
  		public IAMVfwCaptureDialogs, public CPersistStream
{

public:

    // Construct a VidCap filter
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

    CCritSec m_cStateLock;      // Lock this when a function accesses
                                // the filter state.
                                // Generally _all_ functions, since access to
                                // this filter will be by multiple threads.

    DECLARE_IUNKNOWN

    // reveals our interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    int GetPinCount();
    CBasePin * GetPin(int ix);

    // overide the Run & Pause methods so that I can notify the pin
    // of the state it should move to - also needed for CBaseStreamControl
    STDMETHODIMP Run(REFERENCE_TIME tStart);
    STDMETHODIMP Pause(void);
    STDMETHODIMP Stop(void);

    // live source - override GetState to return VFW_S_CANT_CUE when pausing
    // since we won't be sending any data when paused
    STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE *State);

    // IAMVfwCaptureDialogs stuff
    STDMETHODIMP HasDialog(int iDialog);
    STDMETHODIMP ShowDialog(int iDialog, HWND hwnd);
    STDMETHODIMP SendDriverMessage(int iDialog, int uMsg, long dw1, long dw2);

    // for IAMStreamControl
    STDMETHODIMP SetSyncSource(IReferenceClock *pClock);
    STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);

    // IPersistPropertyBag methods
    STDMETHODIMP InitNew();
    STDMETHODIMP Load(LPPROPERTYBAG pPropBag, LPERRORLOG pErrorLog);
    STDMETHODIMP Save(LPPROPERTYBAG pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties);

    // CPersistStream
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int SizeMax();
    STDMETHODIMP GetClassID(CLSID *pClsid);

    STDMETHODIMP FindPin(
        LPCWSTR Id,
        IPin ** ppPin
    );
private:

    // During construction we create the single CVidStream object that provides
    // the capture output pin.
    CVidCap(TCHAR *, LPUNKNOWN, HRESULT *);
    ~CVidCap();
    void CreatePins(HRESULT *phr);

    CVidOverlay *CreateOverlayPin(HRESULT *phr);
    CVidPreview *CreatePreviewPin(HRESULT *phr);

    CVidStream  *m_pCapturePin;	// our capture pin
    CVidOverlay *m_pOverlayPin;	// our h/w overlay preview pin
    CVidPreview *m_pPreviewPin;	// our non h/w overlay preview pin

    int m_iVideoId;		// which device to use

    friend class CVidStream;
    friend class CVideoBufferList;
    friend class CVidOverlay;
    friend class CVidPreview;
    friend class CVidOverlayNotify;
};


//
// CVidStream
//
// Manages the output pin and the video capture device.
// Video capture pins are supposed to support all of these interfaces
class CVidStream : public CSourceStream, public CBaseStreamControl,
		   public IAMVideoCompression, public IAMStreamConfig,
		   public IAMDroppedFrames, public IAMBufferNegotiation,
		   public IKsPropertySet
{

public:

    CVidStream( TCHAR           *pObjectName
              , HRESULT         *phr
              , CVidCap         *pParentFilter
              , unsigned int    uiDriverIndex
              , LPCWSTR         pPinName
              );

    ~CVidStream();

    //
    //  --- CSourceStream implementation ---
    //
public:

    HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    // helper reconnect function.  We can reconnect everything, or just
    // the preview pin because the capture pin changed
    void Reconnect(BOOL fCapturePinToo);

    // IKsPropertySet stuff - to tell the world we are a "capture" type pin
    STDMETHODIMP Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData);
    STDMETHODIMP Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData,
		DWORD *pcbReturned);
    STDMETHODIMP QuerySupported(REFGUID guidPropSet, DWORD dwPropID,
		DWORD *pTypeSupport);

    // IAMStreamConfig stuff
    STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt);
    STDMETHODIMP GetFormat(AM_MEDIA_TYPE **ppmt);
    STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize);
    STDMETHODIMP GetStreamCaps(int i, AM_MEDIA_TYPE **ppmt, LPBYTE pSCC);

    /* IAMVideoCompression methods */
    STDMETHODIMP put_KeyFrameRate(long KeyFrameRate) {return E_NOTIMPL;};
    STDMETHODIMP get_KeyFrameRate(long *pKeyFrameRate) {return E_NOTIMPL;};
    STDMETHODIMP put_WindowSize(DWORDLONG WindowSize) {return E_NOTIMPL;};
    STDMETHODIMP get_WindowSize(DWORDLONG *pWindowSize) {return E_NOTIMPL;};
    STDMETHODIMP put_PFramesPerKeyFrame(long PFramesPerKeyFrame)
			{return E_NOTIMPL;};
    STDMETHODIMP get_PFramesPerKeyFrame(long *pPFramesPerKeyFrame)
			{return E_NOTIMPL;};
    STDMETHODIMP put_Quality(double Quality) {return E_NOTIMPL;};
    STDMETHODIMP get_Quality(double *pQuality) {return E_NOTIMPL;};
    STDMETHODIMP OverrideKeyFrame(long FrameNumber) {return E_NOTIMPL;};
    STDMETHODIMP OverrideFrameSize(long FrameNumber, long Size)
			{return E_NOTIMPL;};
    STDMETHODIMP GetInfo(LPWSTR pstrVersion,
			int *pcbVersion,
			LPWSTR pstrDescription,
			int *pcbDescription,
			long *pDefaultKeyFrameRate,
			long *pDefaultPFramesPerKey,
			double *pDefaultQuality,
			long *pCapabilities);

    /* IAMBufferNegotiation methods */
    STDMETHODIMP SuggestAllocatorProperties(const ALLOCATOR_PROPERTIES *pprop);
    STDMETHODIMP GetAllocatorProperties(ALLOCATOR_PROPERTIES *pprop);


    /* IAMDroppedFrames methods */
    STDMETHODIMP GetNumDropped(long *plDropped);
    STDMETHODIMP GetNumNotDropped(long *plNotDropped);
    STDMETHODIMP GetDroppedInfo(long lSize, long *plArray,
			long *plNumCopied);
    STDMETHODIMP GetAverageFrameSize(long *plAverageSize);

    //
    // --- Worker Thread fn's ---
    //
    HRESULT OnThreadCreate(void);
    HRESULT OnThreadDestroy(void);

    HRESULT DoBufferProcessingLoop(void);
    HRESULT FillBuffer(IMediaSample *pSamp);

    HRESULT Inactive(void);

    // Override to handle quality messages
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);


    STDMETHODIMP QueryId(
        LPWSTR * Id
    );
    
public:

    DECLARE_IUNKNOWN;

    // reveals our interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

private:        // State shared between worker & client
    CCritSec            m_cSharedState;     // Lock this to access this state,
                                            // shared with the worker thread

    unsigned int        m_uiDriverIndex;    // the device to open when active
    HWND                m_hwCapCapturing;   // The AVI capture window,
                                            // used whilst active
    CVideoBufferList    *m_plFilled;        // The buffers AVICap has filled,
                                            // but that haven't been sent yet.
                                            // NB This has its own critical
                                            // section, so m_cSharedState does
                                            // not need to be locked for access

    DWORD               m_dwMicroSecPerFrame;   // The current number of
                                            // microseconds between each frame
    ALLOCATOR_PROPERTIES m_propSuggested;   // IAMBufferNegotiation
    ALLOCATOR_PROPERTIES m_propActual;      // what the allocator is using
    BOOL		m_fSetFormatCalled; // restricted to using this?

private:        // thread state. No peeking/writing by anyone else!

    enum ThreadState { Stopped,         // will exit soon, or just started
                       Paused,          // generate a poster frame if entered
                                        // from stop
                       Running          // streaming data downstream
                     };
    ThreadState m_ThreadState;


private:        // Capture Support

    HRESULT GetMediaType(CMediaType *pmt);
    HRESULT CheckMediaType(const CMediaType *pmt);
    HRESULT SetMediaType(const CMediaType *pmt);
    HRESULT BreakConnect();

    long    GetSampleSize(LPBITMAPINFOHEADER pbmi);   // Calculate the sample
                                                      // size needed.

    HWND CreateCaptureWindow(long lBufferCount);// Create the AVICap window
                                            // invisibly. Balance with
                                            // calls to DestroyCaptureWindow.
                                            // This is the function that grabs
                                            // resources. The filter holds no
                                            // resources (the capture driver)
                                            // until it is called
    BOOL DestroyCaptureWindow(HWND hwnd);   // Destroy an AVICap window,
                                            // release resources

    // put the buffer we are given onto the filled list for delivery downstream
    static LRESULT CALLBACK VideoCallback(HWND hwnd, LPVIDEOHDR lpVHdr);


private:

    WCHAR m_szName[giDriverNameStrLen];         // The driver's name
    WCHAR m_szVersion[giDriverVerStrLen];       // The driver's version

    BOOL m_SupportsVideoSourceDialog;           // The dialogs this driver supports
    BOOL m_SupportsVideoDisplayDialog;          //
    BOOL m_SupportsVideoFormatDialog;           //
    BOOL m_HasOverlay;           		// device does h/w overlay
#if 0
    BOOL m_UsesPalettes;			// does the driver use a palette?
    BOOL m_SuppliesPalettes;			// can the driver give us a palette?
#endif

    // for IAMDroppedFrames
    unsigned int                m_uiFramesCaptured;
    LONGLONG                    m_llTotalFrameSize;
    unsigned int                m_uiFramesSkipped;
    unsigned int                m_uiFramesDelivered;

    friend class CVidCap;
    friend class CVideoBufferList;
    friend class CVidOverlay;
    friend class CVidPreview;
    friend class CVidOverlayNotify;
};


//
// CVideoBufferList
//
// This list is a place to store a buffer that AVIcap gives to us, before we can
// pass them on to the next filter downstream.
// Constructs a list of free buffers on construction.
class CVideoBufferList {

public:

    CVideoBufferList( int iBufferSize
                    , DWORD dwMicroSecPerFrame
                    , CVidCap *pFilter
                    , int iBuffers
                    );
    ~CVideoBufferList();

    HRESULT Add(LPVIDEOHDR lpVHdr);
    HRESULT RemoveHeadIntoSample(IMediaSample *pSample);

    HANDLE GetWaitHandle() {
        return (HANDLE)m_evList;
    }

    // *
    // * CBuffer
    // *

    // This is a class to store the data given to us by AVICap.
    // This must copy the data it is given, as we don't know what AVICap
    // does to its buffers.
    class CBuffer {

    public:

        CBuffer(int iBufferSize);       // prepares an iBufferSize'd memory buffer
        ~CBuffer();                     // frees the data buffer

        BYTE            *GetPointer() const {return m_pData;}
        void            CopyBuffer(LPVIDEOHDR lpVHdr, CRefTime& rt, LONGLONG llTime);
        int             GetSize() const {return m_iCaptureDataLength;}
        // The time for this sample to be presented
        CRefTime        GetCaptureTime() const {return m_rt;}
	// The frame number
        LONGLONG        GetCaptureFrame() const {return m_llFrame;}

    private:

        BYTE            *m_pData;       // The data stored in this buffer
        int             m_iDataLength;  // m_pData's length
	int             m_iCaptureDataLength;  // length returned from device

        CRefTime        m_rt;           // The stream time for this sample.
        LONGLONG        m_llFrame;      // The frame number
	BOOL		m_fSyncPoint;	// for setting sample flags
	BOOL		m_fDiscontinuity;// for setting sample flags

	friend class CVideoBufferList;
    };

private:

    CCritSec                    m_ListCrit;     // Serialize lists
    CAMEvent                      m_evList;       // New element on list
    CGenericList<CBuffer>       m_lFilled;
    CGenericList<CBuffer>       m_lFree;

    CVidCap                     *m_pFilter;

    BOOL                        m_FirstBuffer;

    DWORD                       m_dwMicroSecPerFrame;
    CRefTime                    m_rtStartTime;
    BOOL			m_fLastSampleDiscarded;

    // the Time and MediaTime of the last frame delivered
    REFERENCE_TIME		m_rtLastStartTime;
    LONGLONG			m_llLastFrame;

    // Did somebody do Run->Pause->Run to the graph?
    BOOL	m_fReRun;

    // frame offset to add to the MediaTime - explained in the code
    LONGLONG			m_llFrameOffset;

    int m_iPreviewCount;	// send a preview frame every 30th frame

    friend class CVidStream;
};


// CVidOverlayNotify
// where the video renderer informs us of window moves/clips so we can fix
// the overlay
//
class CVidOverlayNotify : public CUnknown, public IOverlayNotify
{
    public:
        /* Constructor and destructor */
        CVidOverlayNotify(TCHAR              *pName,
                       CVidCap	  *pFilter,
                       LPUNKNOWN           pUnk,
                       HRESULT            *phr);
        ~CVidOverlayNotify();

        /* Unknown methods */

        DECLARE_IUNKNOWN

        STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
        STDMETHODIMP_(ULONG) NonDelegatingRelease();
        STDMETHODIMP_(ULONG) NonDelegatingAddRef();

        /* IOverlayNotify methods */

        STDMETHODIMP OnColorKeyChange(
            const COLORKEY *pColorKey);         // Defines new colour key

        STDMETHODIMP OnClipChange(
            const RECT *pSourceRect,            // Area of video to play
            const RECT *pDestinationRect,       // Area of video to play
            const RGNDATA *pRegionData);        // Header describing clipping

        STDMETHODIMP OnPaletteChange(
            DWORD dwColors,                     // Number of colours present
            const PALETTEENTRY *pPalette);      // Array of palette colours

        STDMETHODIMP OnPositionChange(
            const RECT *pSourceRect,            // Area of video to play with
            const RECT *pDestinationRect);      // Area video goes

    private:
        CVidCap *m_pFilter;
        RECT     m_rcClient;		// how big the preview window is

} ;


// CVidOverlay
// If the capture card supports hardware overlay, this kind of pin will
// be made for a preview pin and connect to the renderer using IOverlay
// Hardware overlay preview is free... there's no point ever turning it
// off, so we don't support CBaseStreamControl (IAMStreamControl)
//
class CVidOverlay : public CBaseOutputPin, public IKsPropertySet
{
public:
    CVidOverlay(
        TCHAR *pObjectName,
        CVidCap *pCapture,
        HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVidOverlay();

    DECLARE_IUNKNOWN

    // reveals our interfaces
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    // IKsPropertySet stuff - to tell the world we are a "preview" type pin
    STDMETHODIMP Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData);
    STDMETHODIMP Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData,
		DWORD *pcbReturned);
    STDMETHODIMP QuerySupported(REFGUID guidPropSet, DWORD dwPropID,
		DWORD *pTypeSupport);

    HRESULT GetMediaType(int iPosition, CMediaType* pt);

    // check if the pin can support this specific proposed type&format
    HRESULT CheckMediaType(const CMediaType*);

    // override this to not do anything with allocators
    HRESULT DecideAllocator(IMemInputPin *pPin,
                           IMemAllocator **ppAlloc);

    // override these to use IOverlay, not IMemInputPin
    STDMETHODIMP Connect(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt);
    HRESULT BreakConnect();
    HRESULT CheckConnect(IPin *pPin);

    HRESULT Active();		// Stop-->Pause
    HRESULT Inactive();		// Pause-->Stop
    HRESULT ActiveRun(REFERENCE_TIME tStart);	// Pause-->Run
    HRESULT ActivePause();	// Run-->Pause

    // say how big our buffers should be and how many we want
    HRESULT DecideBufferSize(IMemAllocator * pAllocator,
                            ALLOCATOR_PROPERTIES *pProperties)
    {
	return NOERROR;
    };

private:
    HWND CreateCaptureWindow(HWND hwnd);
    BOOL DestroyCaptureWindow(HWND hwnd);

    CVidCap     * m_pCap;     // parent
    IOverlay    * m_pOverlay; // Overlay window on output pin
    CVidOverlayNotify m_OverlayNotify; // Notify object
    BOOL         m_bAdvise;   // Advise id
    BOOL		m_fRunning;  // am I running?
    HWND		m_hwndCap;   // the capture window to use

    friend class CVidCap;
    friend class CVidOverlayNotify;
};


// CVidPreview
// for devices that do NOT do hardware overlay, we will be nice and provide
// a preview ourselves, even though the device doesn't support it.  We'll do
// it by noticing when we have free time, and copying some frames and sending
// them out the preview pin.  We'll make sure we never hurt capture
// performance by doing so.
//
class CVidPreview : public CBaseOutputPin, public CBaseStreamControl,
		    public IKsPropertySet
{
public:
    CVidPreview(
        TCHAR *pObjectName,
        CVidCap *pCapture,
        HRESULT * phr,
        LPCWSTR pName);

    virtual ~CVidPreview();

    DECLARE_IUNKNOWN

    // override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    // IKsPropertySet stuff - to tell the world we are a "preview" type pin
    STDMETHODIMP Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData);
    STDMETHODIMP Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData,
		DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData,
		DWORD *pcbReturned);
    STDMETHODIMP QuerySupported(REFGUID guidPropSet, DWORD dwPropID,
		DWORD *pTypeSupport);

    HRESULT GetMediaType(int iPosition, CMediaType* pt);

    // check if the pin can support this specific proposed type&format
    HRESULT CheckMediaType(const CMediaType*);

    HRESULT ActiveRun(REFERENCE_TIME tStart);	// Pause-->Run
    HRESULT ActivePause();	// Run-->Pause
    HRESULT Active();		// Stop-->Pause
    HRESULT Inactive();		// Pause-->Stop

    STDMETHODIMP Notify(IBaseFilter *pFilter, Quality q);

    // say how big our buffers should be and how many we want
    HRESULT DecideBufferSize(IMemAllocator * pAllocator,
                            ALLOCATOR_PROPERTIES *pProperties);


private:
    static DWORD WINAPI ThreadProcInit(void *pv);
    DWORD ThreadProc();
    HRESULT CapturePinActive(BOOL fActive);
    HRESULT ReceivePreviewFrame(LPVOID lpFrame, int iSize);
    HWND CreateCaptureWindow();
    BOOL DestroyCaptureWindow(HWND hwnd);

    CVidCap     *m_pCap;  // parent
    REFERENCE_TIME m_rtRun;
    HANDLE	m_hThread;
    DWORD	m_tid;
    HANDLE	m_hEventRun;
    HANDLE	m_hEventActiveChanged;
    CAMEvent    m_EventAdvise;
    DWORD	m_dwAdvise;
    BOOL	m_fCapturing;	// is the streaming pin active?
    BOOL	m_fLastSampleDiscarded;	// for IAMStreamControl
    HWND	m_hwndCap;

// !!! these have to be public so the video callback can get at them
public:
    LPVOID	m_lpFrame;
    BOOL	m_fRunning; // am I streaming?
    int		m_iFrameSize;
    BOOL	m_fFrameValid;
    HANDLE	m_hEventFrameValid;

    // capGrabFrame callback
    static LRESULT CALLBACK VideoCallback(HWND hwnd, LPVIDEOHDR lpVHdr);

    friend class CVidCap;
    friend class CVidStream;
    friend class CVideoBufferList;
};
