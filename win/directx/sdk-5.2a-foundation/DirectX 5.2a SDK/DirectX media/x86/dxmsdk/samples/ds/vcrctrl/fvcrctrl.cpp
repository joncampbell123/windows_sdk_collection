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
//	filename: fvcrctrl.cpp
//
// Sample VCR Control Filter
//    This filter is CBaseFilter derived. 
//
// Responds to run/pause/stop.  An app would probably want to override the 
// default operation of this filter and "disconnect" the PLAY, STOP and 
// PAUSE mode commands from the filter's ::Run(), ::Stop() and ::Pause() 
// methods using the "link" property.
//
//

#include <windows.h>
#include <streams.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <initguid.h>
#include <olectl.h>
#include <winbase.h>

#include "vcruids.h"
#include "ctimecod.h"
#include "cdevcom.h"
#include "cvcrutil.h"
#include "fvcrctrl.h"
#include "vcrprop.h"
#include "trprop.h"

#define DbgFunc(a) DbgLog(( LOG_TRACE, \
                            2, \
                            TEXT("CVcrFilter::%s"), \
                            TEXT(a) \
                         ));

// IPersistStream macros
#define WRITEOUT(var)   hr = pStream->Write(&var, sizeof(var), NULL); \
                        if (FAILED(hr)) return hr;

#define READIN(var)     hr = pStream->Read(&var, sizeof(var), NULL); \
                        if (FAILED(hr)) return hr;

// some local storage
LONGLONG CVcrTCOut::m_llLastTime = 0L;

const int textbufsize = 12*sizeof(TCHAR);
static TIMECODE_SAMPLE tcsTimecode = {
	0L,{0,0,0L},0L,0L
};
// we will ask for timecode samples this often
const REFERENCE_TIME rtPeriodTime = 3333 * (UNITS / (100*MILLISECONDS));

// Physical Pin data
const TCHAR *strSVO = {"SVO-5800"};
const TCHAR *strPhysConn_Video_Composite = {"Composite"};
const TCHAR *strPhysConn_Video_SVideo = {"S-Video"};
const TCHAR *strPhysConn_Video_Black = {"Black"};
const TCHAR *strPhysConn_Audio_Mic = {"Microphone"};
const TCHAR *strPhysConn_Audio_Line = {"Line"};

// Setup data
const AMOVIESETUP_MEDIATYPE sudTcOpPinTypes[] =
{
    {&MEDIATYPE_Text,       // Major type
    &MEDIASUBTYPE_NULL},    // Minor type
	{&MEDIATYPE_Timecode,	// Major type
    &MEDIASUBTYPE_NULL}		// Minor type
};

const AMOVIESETUP_MEDIATYPE sudAudOpPinType[] =
{
    &MEDIATYPE_AnalogAudio,	// Major type
    &MEDIASUBTYPE_NULL		// Minor type
};

const AMOVIESETUP_MEDIATYPE sudVidIpPinType[] =
{
    &MEDIATYPE_AnalogVideo,				// Major type
    &MEDIASUBTYPE_AnalogVideo_NTSC_M,	// Minor type
	&MEDIATYPE_AnalogVideo,				// Major type
    &MEDIASUBTYPE_AnalogVideo_NTSC_M	// Minor type
};

const AMOVIESETUP_MEDIATYPE sudAudIpPinType[] =
{
    &MEDIATYPE_AnalogAudio,	// Major type
    &MEDIASUBTYPE_NULL,		// Minor type
	&MEDIATYPE_AnalogAudio,	// Major type
    &MEDIASUBTYPE_NULL		// Minor type
};

const AMOVIESETUP_PIN sudPins[] =
{
   {L"Timecode Out",        // Pin string name
    FALSE,                  // Is it rendered
    TRUE,                   // Is it an output
    FALSE,                  // Can we have none
    FALSE,                  // Can we have many
    &CLSID_NULL,            // Connects to filter
    NULL,                   // Connects to pin
    2,                      // Number of types
    sudTcOpPinTypes},       // Pin details
   {L"Analog Composite Video In",     // Pin string name
    FALSE,                  // Is it rendered
    FALSE,                  // Is it an output
    FALSE,                  // Can we have none
    FALSE,                  // Can we have many
    &CLSID_NULL,            // Connects to filter
    NULL,                   // Connects to pin
    1,                      // Number of types
    sudVidIpPinType} 		// Pin details
};

const AMOVIESETUP_FILTER sudVcrctrlax =
{
    &CLSID_VCRControlFilter, // Filter CLSID
    L"VCR Control Filter",   // String name
    MERIT_DO_NOT_USE,	    // Filter merit
    1,                      // Number of pins (that we know about)
    sudPins					// Pin details
};

CFactoryTemplate g_Templates[]= {
	{L"VCR Control Filter",
		&CLSID_VCRControlFilter, 
		CVcrFilter::CreateInstance, 
		NULL, 
		&sudVcrctrlax},
	{L"VCR Control General Property Page", 
		&CLSID_VCRControlPropertyPage, 
		CVcrProperties::CreateInstance, 
		NULL, 
		NULL},
	{L"VCR Control Transport Property Page", 
		&CLSID_VCRTransPropertyPage, 
		CExtTransProperties::CreateInstance, 
		NULL, 
		NULL}
	};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

//------------------------------------------
//
// exported entry points for registration and
// unregistration (in this case they only call
// through to default implmentations).
//
//------------------------------------------

//------------------------------------------
//
// DllRegisterServer
//
// Exported entry points for registration and unregistration
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer

//------------------------------------------
//
// DllUnregisterServer
//
//------------------------------------------
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer
//------------------------------------------


//------------------------------------------
//
// CVcrFilter::Constructor
//
//------------------------------------------
CVcrFilter::CVcrFilter(TCHAR *tszName, LPUNKNOWN punk, CLSID clsid,
						HRESULT *phr)
    : CBaseFilter(tszName, punk, &m_StateLock, clsid, phr),
	CAMExtDevice(phr, tszName),
	CAMExtTransport(phr, tszName),
	CAMTcr(phr, tszName),
	CPersistStream(punk, phr),
	m_iPins(0),
	m_paStreams(NULL)
{
    DbgFunc("CVcrFilter");
    
	CAutoLock cObjectLock(&m_StateLock);
	LPOLESTR pName = NULL;

	// we use a single CDevCom object to make the filter thread-safe
	CAMTcr::SetCommunicationObject(m_devcom);
	CAMExtTransport::SetCommunicationObject(m_devcom);

	// create the pins - reserve space for 6, but we might only make 5
    m_paStreams    = (CBasePin **) new COutStream*[6];
    if (m_paStreams == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }

    // our only output pin
	m_paStreams[0] = new CVcrTCOut(phr, this, L"Timecode Out");
    if (m_paStreams[0] == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }

	// Create the input pins
	m_paStreams[1] = new CVcrAudioLineInputPin(NAME("VCR Audio Line"),this, 
							phr, L"Audio Line In");
    if (m_paStreams[1] == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }

	m_paStreams[2] = new CVcrAudioMicInputPin(NAME("VCR Audio Line"),this,
							phr, L"Audio Mic In");
    if (m_paStreams[2] == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }

	m_paStreams[3] = new CVcrVideoCompInputPin(NAME("VCR Video Input"),this,
							phr, L"Composite Video In");
    if (m_paStreams[3] == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }
	
    m_paStreams[4] = new CVcrVideoSInputPin(NAME("VCR Video Input"),this,
						phr, L"S-Video In");
    if (m_paStreams[4] == NULL) {
        *phr = E_OUTOFMEMORY;
	return;
    }
	
	// if this is an SVO-5800, then it has an additional input pin
	get_ExternalDeviceID( &pName );
	if ( strcmp(strSVO, (const char *)(LPSTR)pName) == 0 ) {
		m_paStreams[5] = new CVcrVideoBlackInputPin(NAME("VCR Video Input"),this,
							phr, L"Video Black");
		if (m_paStreams[5] == NULL) {
	        *phr = E_OUTOFMEMORY;
			return;
		}
	}
	QzTaskMemFree(pName);		// must release the memory
	pName = NULL;
	
	put_Mode( ED_MODE_STOP );

	// configure tc reader
	SetTCRMode(ED_TCR_SOURCE, ED_TCR_LTC);
	CPersistStream::SetDirty(TRUE);		// always write out the state
}

//------------------------------------------
//
// CreateInstance
//
// Provide the way for COM to create a CVcrFilter object.  Also
// initialize a CVcrTCOut object so that we have a pin.
//------------------------------------------
CUnknown *
CVcrFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    CVcrFilter *pNewObject = new CVcrFilter(NAME("VCR Control Filter"),
		punk, CLSID_VCRControlFilter, phr);
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return (CSource *)pNewObject;
}

//------------------------------------------
//
// The Destructor
//
//------------------------------------------
CVcrFilter::~CVcrFilter( void )
{
	//  Free our pins and pin array 
    while (m_iPins != 0) {
		// deleting the pins causes them to be removed from the array...
		delete m_paStreams[m_iPins - 1];
    }
	
    ASSERT(m_paStreams == NULL);
}

//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our property pages and IAMExtDev/
//  IAMExtTransport/etc. interfaces
//------------------------------------------
STDMETHODIMP
CVcrFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    } else if (riid == IID_IAMExtDevice) {
        return GetInterface((IAMExtDevice *) this, ppv);
    } else if (riid == IID_IAMExtTransport) {
        return GetInterface((IAMExtTransport *) this, ppv);
	} else if (riid == IID_IAMTimecodeReader) {
        return GetInterface((IAMTimecodeReader *) this, ppv);
	} else if (riid == IID_IPersistStream) {
        return GetInterface((IPersistStream *) this, ppv);
    }else {
        return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
	}
} 

//------------------------------------------
//
// GetClassID
//
// Override CBaseMediaFilter method for interface IPersist
// Part of the persistent file support.  We must supply our class id
// which can be saved in a graph file and used on loading a graph with
// an external device in it to instantiate this filter via CoCreateInstance.
//
//------------------------------------------
STDMETHODIMP 
CVcrFilter::GetClassID(CLSID *pClsid)
{
    if (pClsid==NULL) {
        return E_POINTER;
    }
    *pClsid = CLSID_VCRControlFilter;
    return NOERROR;
}

//------------------------------------------
//
// SizeMax
//
// Override CPersistStream method.
// State the maximum number of bytes we would ever write in a file
// to save our properties.
//
//------------------------------------------
int
CVcrFilter::SizeMax()
{
    // When an int is expanded as characters it takes at most 12 characters
    // including a trailing delimiter.
    // Wide chars doubles this and we want 16 ints.
    //
    return 384;
} 

//------------------------------------------
//
// WriteToStream
//
// Override CPersistStream method.
// Write our properties to the stream.
//
//	We only write a few of the important properties
//	to demonstrate the process.
//
//------------------------------------------
HRESULT
CVcrFilter::WriteToStream(IStream *pStream)
{
	long temp;
    HRESULT hr;

	GetTransportVideoParameters(ED_TRANSVIDEO_SET_SOURCE, &temp);
    hr = WRITEOUT(temp);
    if (FAILED(hr)) return hr;

	GetTransportAudioParameters(ED_TRANSAUDIO_SET_SOURCE, &temp);
    hr = WRITEOUT(temp);
    if (FAILED(hr)) return hr;

	GetStatus(ED_LINK_MODE, &temp);
    hr = WRITEOUT(temp);
    if (FAILED(hr)) return hr;

	get_DevicePort(&temp);
    hr = WRITEOUT(temp);
    if (FAILED(hr)) return hr;

    return NOERROR;
}

//------------------------------------------
//
// ReadFromStream
//
// Override CPersistStream method.
// Read our properties from the stream.
//
//------------------------------------------
HRESULT
CVcrFilter::ReadFromStream(IStream *pStream)
{
	long temp;
    HRESULT hr;

	hr = READIN(temp);
	SetTransportVideoParameters(ED_TRANSVIDEO_SET_SOURCE, temp);
    if (FAILED(hr)) return hr;

	hr = READIN(temp);
	SetTransportAudioParameters(ED_TRANSAUDIO_SET_SOURCE, temp);
    if (FAILED(hr)) return hr;

	hr = READIN(temp);
	put_Mode(temp);	// this will set the link mode
    if (FAILED(hr)) return hr;

	hr = READIN(temp);
	put_DevicePort(temp);		// this will set the active communications
    if (FAILED(hr)) return hr;	// port

    return NOERROR;
}

//------------------------------------------
//
//  Add a new pin
//
//------------------------------------------
HRESULT
CVcrFilter::AddPin(CBasePin *pPin)
{
    CAutoLock lock(&m_StateLock);

    /*  Allocate space for this pin and the old ones */
    CBasePin **paStreams = new CBasePin *[m_iPins + 1];
    if (paStreams == NULL) {
        return E_OUTOFMEMORY;
    }
    if (m_paStreams != NULL) {
        CopyMemory((PVOID)paStreams, (PVOID)m_paStreams,
                   m_iPins * sizeof(m_paStreams[0]));
        paStreams[m_iPins] = pPin;
        delete [] m_paStreams;
    }
    m_paStreams = (CBasePin **)paStreams;
    m_paStreams[m_iPins] = pPin;
    m_iPins++;
	return S_OK;
}

//------------------------------------------
//
//  Remove a pin - pStream is NOT deleted
//
//------------------------------------------
HRESULT
CVcrFilter::RemovePin(CBasePin *pStream)
{
    int i;
    for (i = 0; i < m_iPins; i++) {
        if (m_paStreams[i] == pStream) {
            if (m_iPins == 1) {
                delete [] m_paStreams;
                m_paStreams = NULL;
            } else {
                /*  no need to reallocate */
				while (++i < m_iPins)
				m_paStreams[i - 1] = m_paStreams[i];
            }
            m_iPins--;
			return S_OK;
        }
    }
    return S_FALSE;
}

//------------------------------------------
//
// GetPinCount
//
// Returns the number of pins this filter has
//
//------------------------------------------
int
CVcrFilter::GetPinCount(void)
{
    CAutoLock lock(&m_StateLock);
    return m_iPins;
}

//------------------------------------------
//
// GetPin
//
// Return a non-addref'd pointer to pin n
// needed by CBaseFilter
//
//------------------------------------------
CBasePin *
CVcrFilter::GetPin(int n)
{
    CAutoLock lock(&m_StateLock);

    if ((m_iPins > 0) && (n < m_iPins)) {

        ASSERT(m_paStreams[n]);
		return m_paStreams[n];
    }
    return NULL;
}

//------------------------------------------
//
// IMediaControl Overrides
//
//------------------------------------------
HRESULT 
CVcrFilter::Run(REFERENCE_TIME tStart)
{
	long Mode;
	long LinkMode;
	
	DbgFunc("Run");
	GetStatus(ED_LINK_MODE, &LinkMode);
	if (LinkMode == OATRUE) {
		get_Mode( &Mode );
		if (Mode == ED_MODE_FREEZE)
			put_Mode(ED_MODE_THAW);
		else
			put_Mode(ED_MODE_PLAY);
	}

	// use the base Run method for the state transition
	CBaseFilter::Run(tStart);
	return NOERROR;
}

HRESULT 
CVcrFilter::Pause() {
	long LinkMode;

	DbgFunc("Pause");
	GetStatus(ED_LINK_MODE, &LinkMode);
	if (LinkMode == OATRUE)
		put_Mode(ED_MODE_FREEZE);

	// use the base Run method for the state transition
	CBaseFilter::Pause();
	return NOERROR;
}

HRESULT 
CVcrFilter::Stop()
{
	long LinkMode;

	DbgFunc("Stop");
	GetStatus(ED_LINK_MODE, &LinkMode);
	if (LinkMode == OATRUE)
		put_Mode(ED_MODE_STOP);

	// use the base Run method for the state transition
	CBaseFilter::Stop();
	return NOERROR;
}

// ==============Implementation of the IPropertypages Interface ===========
//------------------------------------------
//
// GetPages - return the CLSIDs for the property pages we support
//
//------------------------------------------
STDMETHODIMP 
CVcrFilter::GetPages(CAUUID * pPages)
{
	CAutoLock l(&m_StateLock);
	
	CheckPointer(pPages,E_POINTER);
    
	pPages->cElems = 2;
    pPages->pElems = (GUID *) QzTaskMemAlloc(2 * sizeof(GUID));
    if (pPages->pElems == NULL) {
        return E_OUTOFMEMORY;
    }

    pPages->pElems[0] = CLSID_VCRControlPropertyPage;
    pPages->pElems[1] = CLSID_VCRTransPropertyPage;
	
    return NOERROR;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * --- COutStream ----
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------
//
//------------------------------------------
//
// COutStream::Constructor
//
// increments the number of pins present on the filter
//
//------------------------------------------
COutStream::COutStream(
    TCHAR *pObjectName,
    HRESULT *phr,
    CVcrFilter *pParent,
    LPCWSTR pPinName)
    : CBaseOutputPin(pObjectName, pParent, pParent->pLock(), phr, pPinName),
    m_pFilter(pParent),
	m_pRefClock(NULL),
	m_hAdviseEvent(NULL)
{
     *phr = m_pFilter->AddPin(this);
}

//------------------------------------------
//
// COutStream::Destructor
//
// Decrements the number of pins on this filter
//------------------------------------------
COutStream::~COutStream(void)
{
	m_pFilter->RemovePin(this);
}
//------------------------------------------
//
// Active
//
// The pin is active - start up the worker thread
//
//------------------------------------------
HRESULT 
COutStream::Active(void)
{
    CAutoLock lock(m_pFilter->pLock());

    HRESULT hr;

    if (m_pFilter->IsActive()) {
	return S_FALSE;	// succeeded, but did not allocate resources 
					//  (they already exist...)
    }

    // do nothing if not connected - its ok not to connect to
    // all pins of an external device filter
    if (!IsConnected()) {
        return NOERROR;
    }

    hr = CBaseOutputPin::Active();
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT(!ThreadExists());

    // start the thread
    if (!Create()) {
        return E_FAIL;
    }

    // Tell thread to initialize. If OnThreadCreate Fails, so does this.
    hr = Init();
    if (FAILED(hr))
	return hr;

    return Pause();
}

//------------------------------------------
//
// Inactive
//
// Pin is inactive - shut down the worker thread
// Waits for the worker to exit before returning.
//
//------------------------------------------
HRESULT 
COutStream::Inactive(void)
{
    CAutoLock lock(m_pFilter->pLock());

    HRESULT hr;

    // do nothing if not connected - its ok not to connect to
    // all pins of an external device filter
    if (!IsConnected())
        return NOERROR;

    // !!! need to do this before trying to stop the thread, because
    // we may be stuck waiting for our own allocator!!!

    hr = CBaseOutputPin::Inactive();	// call this first to decommit 
										//  the allocator
    if (FAILED(hr))
		return hr;
    
    if (ThreadExists()) {
		hr = Stop();
		if (FAILED(hr)) {
			return hr;
		}
		hr = Exit();
		if (FAILED(hr)) {
			return hr;
		}
		Close();	// Wait for the thread to exit, then tidy up.
    }
    return NOERROR;
}

//------------------------------------------
//
// ThreadProc
//
// When this returns the thread exits
// Return codes > 0 indicate an error occured
//
//------------------------------------------
DWORD 
COutStream::ThreadProc(void)
{
    HRESULT hr;  // the return code from calls
    Command com;
	REFERENCE_TIME rtNow;
	DWORD    dwAdviseToken;

    do {
		com = GetRequest();
		if (com != CMD_INIT) {
			DbgLog((LOG_ERROR, 1, TEXT("Thread expected init command")));
			Reply(E_UNEXPECTED);
		}
    } while (com != CMD_INIT);

    DbgLog((LOG_TRACE, 1, TEXT("COutStream worker thread initializing")));

    hr = OnThreadCreate(); // perform set up tasks
    if (FAILED(hr)) {
        DbgLog((LOG_ERROR, 1,
			TEXT("COutStream::OnThreadCreate failed. Aborting thread.")));
        OnThreadDestroy();
		Reply(hr);	// send failed return code from OnThreadCreate
        return 1;
    }

    // Initialisation suceeded
    Reply(NOERROR);

    Command cmd;
    do {
		cmd = GetRequest();

		switch (cmd) {

		case CMD_EXIT:
			Reply(NOERROR);
			break;

		case CMD_RUN:
			DbgLog((LOG_ERROR, 1, 
				TEXT("CMD_RUN received before a CMD_PAUSE???")));
			// !!! fall through???
		case CMD_PAUSE:
			Reply(NOERROR);
			// we pick up the reference clock here
			if ( m_pFilter->GetSyncSource(&m_pRefClock) )
				return (DWORD) VFW_E_NO_CLOCK;
			ASSERT(m_pRefClock);
			// and set the tick to 33.33ms (an NTSC video frame).  We use a
			// "one-shot" event instead of a periodic notification because
			// we don't want events to stack up due to system latencies.
			m_hAdviseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			m_pRefClock->GetTime(&rtNow);
			m_pRefClock->AdviseTime(m_tStart, rtNow+rtPeriodTime,
				(unsigned long)m_hAdviseEvent, &dwAdviseToken);

			DoBufferProcessingLoop();
			break;

		case CMD_STOP:
			// release our reference clock
			m_pRefClock->Release();
			m_pRefClock = NULL;
			m_hAdviseEvent = NULL;
			Reply(NOERROR);
			break;

		default:
			DbgLog((LOG_ERROR, 1, TEXT("Unknown command %d received!"), cmd));
			Reply(E_NOTIMPL);
			break;
		}
    } while (cmd != CMD_EXIT);

    hr = OnThreadDestroy();	// tidy up.
    if (FAILED(hr)) {
        DbgLog((LOG_ERROR, 1,
			TEXT("COutStream::OnThreadDestroy failed. Exiting thread.")));
        return 1;
    }

    DbgLog((LOG_TRACE, 1, TEXT("COutStream worker thread exiting")));
    return 0;
}

//------------------------------------------
//
// DoBufferProcessingLoop
//
// Grabs a buffer and waits for a frame time to pass
//
//------------------------------------------
HRESULT 
COutStream::DoBufferProcessingLoop(void)
{
    Command com;
	REFERENCE_TIME rtNow = 0L;
	REFERENCE_TIME rtAdvise = 0L;
	DWORD    dwAdviseToken;

    OnThreadStartPlay();

    do {
	while (!CheckRequest(&com)) {

	    IMediaSample *pSample;

	    HRESULT hr = GetDeliveryBuffer(&pSample,NULL,NULL,FALSE);
	    if (FAILED(hr)) {
		continue;	// go round again. Perhaps the error will go away
			    // or the allocator is decommited & we will be asked to
			    // exit soon.
	    }

	    // Virtual function user will override.
	    hr = FillBuffer(pSample);
	    // !!! shouldn't we check for errors here???

	    if (hr == S_OK) {
			Deliver(pSample);
	    } else if (hr == S_FALSE) {
			pSample->Release();
			DeliverEndOfStream();
			return S_OK;
	    } else {
			DbgLog((LOG_ERROR, 1, TEXT("Error %08lX from FillBuffer!!!"), hr));
	    }

	    pSample->Release();
		// and block here for a frame -
		// but only do this if an event has been set
		if (m_hAdviseEvent) {
			WaitForSingleObject(m_hAdviseEvent, 1000L);
			// woke up - show the time
			m_pRefClock->GetTime(&rtNow);
			rtAdvise = rtNow + rtPeriodTime;
			// reset the clock
			m_pRefClock->AdviseTime(m_tStart, rtAdvise,
				(unsigned long)m_hAdviseEvent, &dwAdviseToken);
		}
	}
	
	if (com == CMD_RUN || com == CMD_PAUSE)
	    com = GetRequest(); // throw command away
	else if (com != CMD_STOP) {
	    DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
	}
    } while (com != CMD_STOP);

    return S_FALSE;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrTCOut
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrTCOut::Constructor
//
// Create a default stream
//
//------------------------------------------
CVcrTCOut::CVcrTCOut(HRESULT *phr, CVcrFilter *pParent, LPCWSTR pName)
    : COutStream(NAME("VCR Timecode output pin"),phr, pParent, pName),
    m_pFilter(pParent)
{
    CAutoLock l(m_pFilter->pLock());

	m_ctc = new CTimecode();
}

//------------------------------------------
//
// CVcrTCOut::Destructor
//
//------------------------------------------
CVcrTCOut::~CVcrTCOut(void)
{
    CAutoLock l(m_pFilter->pLock());

    delete m_ctc;
	
}

//------------------------------------------
//
// FillBuffer
//
// Stuffs the buffer with data
//
//------------------------------------------
HRESULT 
CVcrTCOut::FillBuffer(IMediaSample *pms)
{
    BYTE	*pData;
    long 	mode;
	DWORD	dwresult;

    TIMECODE_SAMPLE tcsTimecode ;

    pms->GetPointer(&pData);
    
	//what time is it?
	CRefTime rtStart  = m_rtSampleTime;		// the current time is 
											//  the sample's start
    m_rtSampleTime += (LONG)(1000L*1001L)/30000L;	// 29.97 fps for 
													//  now in 100 ns units
    pms->SetTime((REFERENCE_TIME*)&rtStart,
                 (REFERENCE_TIME*)&m_rtSampleTime);

	if ( *(m_mt.Type()) == MEDIATYPE_Text)
            pms->SetActualDataLength(textbufsize);
	else
            pms->SetActualDataLength(sizeof(TIMECODE_SAMPLE));


	// should we update the display (only when in PLAY)?
	m_pFilter->get_Mode(&mode);
	if ( mode == ED_MODE_PLAY ) {
		// get a new timecode
		if ( (dwresult = m_pFilter->GetTimecode(&tcsTimecode)) )
			tcsTimecode.timecode.dwFrames = -1;
		if ( m_mtype == 0) {
			// we need to output text - make the timecode displayable
			m_ctc->ConvertTimecodeToString(&tcsTimecode, (TCHAR *)pData);
		}
		else {
			// dummy out unused members
			tcsTimecode.qwTick = m_rtSampleTime;
			tcsTimecode.timecode.wFrameFract = 0;
			tcsTimecode.dwUser = 0L;
			tcsTimecode.dwFlags = 0x0000;	// make it non-drop for now
			memcpy((TCHAR *)pData, &tcsTimecode, sizeof(TIMECODE_SAMPLE));
		}
	}
    return NOERROR;
}

//------------------------------------------
//
// GetMediaType
//
//------------------------------------------
HRESULT 
CVcrTCOut::GetMediaType(int iPosition, CMediaType *pmt)
{
    CAutoLock l(m_pFilter->pLock());
	
    if (iPosition<0) {
        return E_INVALIDARG;
    }
	// we support 2 media types now
	if (iPosition == 0) {
		pmt->SetType(&MEDIATYPE_Text);
		pmt->SetSubtype(&GUID_NULL);
	}
	else if (iPosition == 1) {
		pmt->SetType(&MEDIATYPE_Timecode);
		pmt->SetSubtype(&MEDIASUBTYPE_NULL);
	}
	else if (iPosition>1) {
        return VFW_S_NO_MORE_ITEMS;
    }

    // no temporal compression in both cases
    pmt->SetTemporalCompression(FALSE);
        
    return NOERROR;
}

//------------------------------------------
//
// CheckMediaType
//
// Returns E_INVALIDARG if the mediatype is not 
//	acceptable, S_OK if it is
//
//------------------------------------------
HRESULT 
CVcrTCOut::CheckMediaType(const CMediaType *pMediaType)
{
    CAutoLock l(m_pFilter->pLock());

    if (*(pMediaType->Type()) != MEDIATYPE_Text &&
		*(pMediaType->Type()) != MEDIATYPE_Timecode)	//Text or timecode only
       return E_INVALIDARG;
    
    return S_OK;  // This format is acceptable.
}

//------------------------------------------
//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what size media sample
// we agreed. Then we can ask for buffers of the correct size to contain them.
//
//------------------------------------------
HRESULT 
CVcrTCOut::DecideBufferSize(IMemAllocator *pAlloc,
							ALLOCATOR_PROPERTIES *pRequest)
{
    CAutoLock l(m_pFilter->pLock());

	if ( *(m_mt.Type()) == MEDIATYPE_Text)
		pRequest->cbBuffer = textbufsize;	//the size we will request
	else
		pRequest->cbBuffer = sizeof(TIMECODE_SAMPLE);//the size we will request
	
	pRequest->cBuffers = 5;   	        //the number of buffers to use.
    ALLOCATOR_PROPERTIES  Actual;
    pAlloc->SetProperties(pRequest, &Actual);

    if ((Actual.cbBuffer < pRequest->cbBuffer) ) // this allocator is unsuitable
        return E_FAIL;
    
    return NOERROR;
}

//------------------------------------------
//
// OnThreadCreate
//
// as we go active reset the stream time to zero
//
//------------------------------------------
HRESULT 
CVcrTCOut::OnThreadCreate(void)
{
	// save a local copy of the currently selected media type
	// for this pin: 0 for text, 1 for timecode
	if (*(m_mt.Type()) == MEDIATYPE_Text)
			m_mtype = 0;
	else
			m_mtype = 1;

    m_rtSampleTime = 0;
	tcsTimecode.timecode.dwFrames = 0L;

    return NOERROR;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrInputPin 
// *
//
// Base class for hardware input pins
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrInputPin Constructor
//
//------------------------------------------
CVcrInputPin::CVcrInputPin(TCHAR *pObjectName, CVcrFilter *pFilter,
	HRESULT * phr, LPCWSTR pName)
	:
	CBaseInputPin(pObjectName, pFilter, pFilter->pLock(), phr, pName),
	CAMPhysicalPinInfo(phr, (TCHAR *)pName),
	m_pFilter(pFilter)
{
   DbgLog((LOG_TRACE,1,TEXT("CVcrInputPin constructor")));
    ASSERT(pFilter);
	*phr = m_pFilter->AddPin(this);
}

//------------------------------------------
//
//	CVcrInputPin Destructor
//
//------------------------------------------
CVcrInputPin::~CVcrInputPin(void) 
{
	DbgLog((LOG_TRACE,1,TEXT("*Destroying an input pin")));
	m_pFilter->RemovePin(this);
}

//------------------------------------------
//
// NonDelegatingQueryInterface
//
// Reveal our Interfaces
//------------------------------------------
STDMETHODIMP
CVcrInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CAutoLock cObjectLock(&m_StateLock);

    if (riid == IID_IAMPhysicalPinInfo)
        return GetInterface((IAMPhysicalPinInfo *) this, ppv);
    else
        return CBaseInputPin::NonDelegatingQueryInterface(riid, ppv);
} 

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrVideoCompInputPin
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrVideoCompInputPin Constructor
//
//------------------------------------------
CVcrVideoCompInputPin::CVcrVideoCompInputPin(TCHAR *pObjectName,
											 CVcrFilter *pFilter,
	HRESULT * phr, LPCWSTR pName)
	:
	CVcrInputPin(pObjectName, pFilter, phr, pName)
{

}

//------------------------------------------
//
//	CVcrVideoCompInputPin Destructor
//
//------------------------------------------
CVcrVideoCompInputPin::~CVcrVideoCompInputPin(void) 
{	
}

//------------------------------------------
//
// GetPhysicalType
//
// Returns physical connection info
//	
//------------------------------------------

HRESULT
CVcrVideoCompInputPin::GetPhysicalType( long *pType, LPOLESTR * ppszType)
{
    *pType = PhysConn_Video_Composite;
	CheckPointer(ppszType, E_POINTER);
	*ppszType = NULL;
	// query the device
	*ppszType = (LPOLESTR)
		QzTaskMemAlloc(sizeof(WCHAR) * \
			(1+lstrlenW((LPOLESTR)strPhysConn_Video_Composite)));
	if (*ppszType !=NULL)
		lstrcpyW(*ppszType, (LPOLESTR)strPhysConn_Video_Composite);

	return S_OK;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrVideoSInputPin
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrVideoSInputPin Constructor
//
//------------------------------------------
CVcrVideoSInputPin::CVcrVideoSInputPin(TCHAR *pObjectName, CVcrFilter *pFilter,
	HRESULT * phr, LPCWSTR pName)
	:
	CVcrInputPin(pObjectName, pFilter, phr, pName)
{

}

//------------------------------------------
//
//	CVcrVideoSInputPin Destructor
//
//------------------------------------------
CVcrVideoSInputPin::~CVcrVideoSInputPin(void) 
{	

}

//------------------------------------------
//
// GetPhysicalType
//
// Returns physical connection info
//
//------------------------------------------

HRESULT
CVcrVideoSInputPin::GetPhysicalType( long *pType, LPOLESTR * ppszType)
{
    *pType = PhysConn_Video_SVideo;
	CheckPointer(ppszType, E_POINTER);
	*ppszType = NULL;
	// query the device
	*ppszType = (LPOLESTR)
		QzTaskMemAlloc(sizeof(WCHAR) * \
			(1+lstrlenW((LPOLESTR)strPhysConn_Video_SVideo)));
	if (*ppszType !=NULL)
		lstrcpyW(*ppszType, (LPOLESTR)strPhysConn_Video_SVideo);

	return S_OK;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrVideoBlackInputPin
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrVideoBlackInputPin Constructor
//
//------------------------------------------
CVcrVideoBlackInputPin::CVcrVideoBlackInputPin(TCHAR *pObjectName,
											   CVcrFilter *pFilter,
												HRESULT * phr, LPCWSTR pName)
	:
	CVcrInputPin(pObjectName, pFilter, phr, pName)
{

}

//------------------------------------------
//
//	CVcrVideoBlackInputPin Destructor
//
//------------------------------------------
CVcrVideoBlackInputPin::~CVcrVideoBlackInputPin(void) 
{	

}

//------------------------------------------
//
// GetPhysicalType
//
// Returns physical connection info
//
//------------------------------------------

HRESULT
CVcrVideoBlackInputPin::GetPhysicalType( long *pType, LPOLESTR * ppszType)
{
	*pType = PhysConn_Video_Black;
	CheckPointer(ppszType, E_POINTER);
	*ppszType = NULL;
	// query the device
	*ppszType = (LPOLESTR)
		QzTaskMemAlloc(sizeof(WCHAR) * \
					(1+lstrlenW((LPOLESTR)strPhysConn_Video_Black)));
	if (*ppszType !=NULL)
		lstrcpyW(*ppszType, (LPOLESTR)strPhysConn_Video_Black);

    return S_OK;
}


//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrAudioLineInputPin
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrAudioLineInputPin Constructor
//
//------------------------------------------
CVcrAudioLineInputPin::CVcrAudioLineInputPin(TCHAR *pObjectName,
											 CVcrFilter *pFilter,
											HRESULT * phr, LPCWSTR pName)
	:
	CVcrInputPin(pObjectName, pFilter, phr, pName)
{

}

//------------------------------------------
//
//	CVcrAudioLineInputPin Destructor
//
//------------------------------------------
CVcrAudioLineInputPin::~CVcrAudioLineInputPin(void) 
{	

}

//------------------------------------------
//
// GetPhysicalType
//
// Returns physical connection info
//
//------------------------------------------

HRESULT
CVcrAudioLineInputPin::GetPhysicalType( long *pType, LPOLESTR * ppszType)
{
	*pType = PhysConn_Audio_Line;
	CheckPointer(ppszType, E_POINTER);
	*ppszType = NULL;
	// query the device
	*ppszType = (LPOLESTR)
		QzTaskMemAlloc(sizeof(WCHAR) * 
			(1+lstrlenW((LPOLESTR)strPhysConn_Audio_Line)));
	if (*ppszType !=NULL)
		lstrcpyW(*ppszType, (LPOLESTR)strPhysConn_Audio_Line);

	return S_OK;
}

//------------------------------------------
//------------------------------------------
//------------------------------------------
// *
// * CVcrAudioMicInputPin
// *
//------------------------------------------
//------------------------------------------
//------------------------------------------

//------------------------------------------
//
// CVcrAudioMicInputPin Constructor
//
//------------------------------------------
CVcrAudioMicInputPin::CVcrAudioMicInputPin(TCHAR *pObjectName,
										   CVcrFilter *pFilter,
											HRESULT * phr,
											LPCWSTR pName)
	:
	CVcrInputPin(pObjectName, pFilter, phr, pName)
{

}

//------------------------------------------
//
//	CVcrAudioMicInputPin Destructor
//
//------------------------------------------
CVcrAudioMicInputPin::~CVcrAudioMicInputPin(void) 
{
	
}

//------------------------------------------
//
// GetPhysicalType
//
// Returns physical connection info
//
//------------------------------------------

HRESULT
CVcrAudioMicInputPin::GetPhysicalType( long *pType, LPOLESTR * ppszType)
{
	*pType = PhysConn_Audio_Mic;
	CheckPointer(ppszType, E_POINTER);
	*ppszType = NULL;
	// query the device
	*ppszType = (LPOLESTR)
		QzTaskMemAlloc(sizeof(WCHAR) * \
						(1+lstrlenW((LPOLESTR)strPhysConn_Audio_Mic)));
	if (*ppszType !=NULL)
		lstrcpyW(*ppszType, (LPOLESTR)strPhysConn_Audio_Mic);

    return S_OK;
}

//eof fvcrctrl.cpp

