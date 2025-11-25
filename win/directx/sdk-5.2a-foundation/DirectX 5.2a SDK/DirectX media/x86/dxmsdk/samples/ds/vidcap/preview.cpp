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

/*

    Methods for CVidPreview - the preview pin that doesn't use overlay

	The basic premise here is that the driver only provides one
	stream... we are FAKING this preview stream by only accepting
	the same media type as our capture pin, and every once in a while
	when the capture pin has some extra time it will send us a copy
	of a frame to use as a preview frame.  If we had hardware that
	can do a h/w overlay, we would have used the overlay type of
	preview pin.  If we had hardware that can capture two streams
	at once, we wouldn't need to fake a preview pin by duplicating
	data going out the capture pin, but alas, we are not that lucky.

 	This is tricky to do.  If the capture pin is streaming and using
 	a capture window to capture with, we can't make our own capture
	window or make any calls to the capture pin's capture window (there
	can be only one around at a time).  So in this case, the capture pin
	will send us a frame every once in a while for us to copy and deliver.
	If the capture pin is NOT active, and does not have the capture
	device open, then we need to create our own capture window and use
	it to capture frames and deliver them.  We will be able to switch
	back and forth.

	This pin supports CBaseStreamControl (and thus IAMStreamControl)
	for turning preview on and off while the graph is running
*/

#include <streams.h>
#include <mmsystem.h>
#include <vfw.h>
#include "vidcap.h"

// CVidPreview constructor
//
CVidPreview::CVidPreview(TCHAR *pObjectName, CVidCap *pCapture,
        HRESULT * phr, LPCWSTR pName)
   :
   CBaseOutputPin(pObjectName, pCapture, pCapture->pStateLock(), phr, pName),
   m_pCap(pCapture),
   m_fRunning(FALSE),
   m_hThread(NULL),
   m_tid(0),
   m_hEventRun(NULL),
   m_dwAdvise(0),
   m_fCapturing(FALSE),
   m_hEventActiveChanged(NULL),
   m_hEventFrameValid(NULL),
   m_lpFrame(NULL),
   m_iFrameSize(0),
   m_fLastSampleDiscarded(FALSE),
   m_fFrameValid(FALSE),
   m_hwndCap(NULL)
{
   DbgLog((LOG_TRACE,1,TEXT("CVidPreview constructor")));
   ASSERT(pCapture);
}


CVidPreview::~CVidPreview()
{
    DbgLog((LOG_TRACE,1,TEXT("*Destroying the Preview pin")));
    // we left one of these hanging around
    if (m_hwndCap)
	DestroyCaptureWindow(m_hwndCap);
};


STDMETHODIMP CVidPreview::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    if (riid == IID_IAMStreamControl) {
	return GetInterface((LPUNKNOWN)(IAMStreamControl *)this, ppv);
    } else if (riid == IID_IKsPropertySet) {
	return GetInterface((LPUNKNOWN)(IKsPropertySet *)this, ppv);
    }

   return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
}


// we can only allow being connected with the same media type as the
// capture pin.  It may be giving us the frames to deliver.
//
HRESULT CVidPreview::GetMediaType(int iPosition, CMediaType *pmt)
{
    DbgLog((LOG_TRACE,3,TEXT("CVidPreview::GetMediaType #%d"), iPosition));

    if (iPosition < 0)
	return E_INVALIDARG;
    if (iPosition > 0)
	return VFW_S_NO_MORE_ITEMS;

    // we preview the same format as we capture
    return m_pCap->m_pCapturePin->GetMediaType(pmt);
}


// We only accept what the capture pin would accept.
//
HRESULT CVidPreview::CheckMediaType(const CMediaType *pMediaType)
{
    DbgLog((LOG_TRACE,3,TEXT("CVidPreview::CheckMediaType")));

    // Only accept what our capture pin is providing.  I will not switch
    // our capture pin over to a new format just because somebody changes
    // the preview pin.
    return m_pCap->m_pCapturePin->CheckMediaType(pMediaType);
}


// we are being RUN.  Time to start previewing
//
HRESULT CVidPreview::ActiveRun(REFERENCE_TIME tStart)
{
    DbgLog((LOG_TRACE,2,TEXT("CVidPreview Pause->Run")));

    ASSERT(IsConnected());

    m_fRunning = TRUE;
    m_rtRun = tStart;

    // tell our thread to start previewing
    SetEvent(m_hEventRun);

    return NOERROR;
}


// we have stopped running.  Our thread will stop previewing
//
HRESULT CVidPreview::ActivePause()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidPreview Run->Pause")));

    m_fRunning = FALSE;

    return NOERROR;
}


// we are starting to stream.  Time to create a thread to do preview
//
HRESULT CVidPreview::Active()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidPreview Stop->Pause")));

    ASSERT(IsConnected());

    // This event tells the thread when we are RUN or STOPPED
    m_hEventRun = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hEventRun) {
        DbgLog((LOG_ERROR,1,TEXT("Can't create Run event")));
        return E_OUTOFMEMORY;
    }

    // This event is set by the thread when it notices the capture pin
    // going from inactive->active or vv.  We delay returning from
    // CapturePinActive() until the thread notices, because the capture
    // pin will actually go active after CapturePinActive returns and
    // our thread has to have closed the capture window by then.
    m_hEventActiveChanged = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hEventActiveChanged) {
        DbgLog((LOG_ERROR,1,TEXT("Can't create ActiveChanged event")));
        return E_OUTOFMEMORY;
    }

    // This event tells the thread that a frame is ready to deliver.  It
    // might have come from the capture pin, or we might have captured it
    // ourself.
    m_hEventFrameValid = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_hEventFrameValid) {
        DbgLog((LOG_ERROR,1,TEXT("Can't create FrameValid event")));
        return E_OUTOFMEMORY;
    }

    m_EventAdvise.Reset();
    m_fFrameValid = FALSE;

    m_hThread = CreateThread(NULL, 0, CVidPreview::ThreadProcInit, this,
				0, &m_tid);
    if (!m_hThread) {
        DbgLog((LOG_ERROR,1,TEXT("Can't create Preview thread")));
       return E_OUTOFMEMORY;
    }

    // now that we're streaming, we need a capture window to use, unless
    // of course, our capture pin is active too, in which case we'll use
    // its capture window since there can be only one.
    if (!m_fCapturing && !m_hwndCap) {
	m_hwndCap = CreateCaptureWindow();
	ASSERT(m_hwndCap);
    }
	
    return CBaseOutputPin::Active();
}


// our pin has stopped streaming.  Time to kill the thread.
//
HRESULT CVidPreview::Inactive()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidPreview Pause->Stop")));

    ASSERT(IsConnected());

    // tell our thread to give up and die
    SetEvent(m_hEventRun);
    SetEvent(m_hEventFrameValid);
    SetEvent(m_hEventActiveChanged);

    // If our thread is stuck in GetDeliveryBuffer, only this will release it
    HRESULT hr = CBaseOutputPin::Inactive();

    // We're waiting for an advise that will now never come
    if (m_pCap->m_pClock && m_dwAdvise) {
	m_pCap->m_pClock->Unadvise(m_dwAdvise);
	m_EventAdvise.Set();
    }

    CloseHandle(m_hEventRun);
    CloseHandle(m_hEventActiveChanged);
    CloseHandle(m_hEventFrameValid);
    m_hEventRun = NULL;
    m_hEventActiveChanged = NULL;
    m_hEventFrameValid = NULL;

    // when our main thread shuts down the capture thread, the capture
    // thread will destroy the capture window it made, and this will cause
    // USER to send messages to our main thread, and if we're blocked
    // waiting for the capture thread to go away, we will deadlock
    // preventing user from sending us the messages, and thus our thread
    // will never go away.  We need to dispatch messages while waiting
    // !!! It's more efficient to use MsgWaitForMultipleObjects, I know.
    while (WaitForSingleObject(m_hThread, 50) == WAIT_TIMEOUT) {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
	 }
    }
    CloseHandle(m_hThread);
    m_tid = 0;
    m_hThread = NULL;
    
    // now that we've stopped streaming, the capture pin may need to open
    // the device to connect, etc, so we better let it go
    if (m_hwndCap)
	DestroyCaptureWindow(m_hwndCap);
    m_hwndCap = NULL;

    return hr;
}


// we want 1 buffer big enough to hold a frame of the type that the capture
// pin is capturing.  We do some fancy math to align the buffers if the
// pin we are connected to requires it.
//
HRESULT CVidPreview::DecideBufferSize(IMemAllocator * pAllocator, ALLOCATOR_PROPERTIES *pProperties)
{
   DbgLog((LOG_TRACE,2,TEXT("CVidPreview DecideBufferSize")));

   ASSERT(pAllocator);
   ASSERT(pProperties);

   // !!! more preview buffers?
   if (pProperties->cBuffers < 1)
       pProperties->cBuffers = 1;

   if (pProperties->cbAlign == 0)
	pProperties->cbAlign = 1;

   // This is how big we need each buffer to be
   CMediaType cmt;
   HRESULT hr = GetMediaType(0, &cmt);
   if (hr != S_OK)
	return hr;
   pProperties->cbBuffer = max(pProperties->cbBuffer,
				(long)(HEADER(cmt.Format())->biSizeImage));

   // Make the prefix + buffer size meet the alignment restriction
   pProperties->cbBuffer = (long)ALIGNUP(pProperties->cbBuffer +
				pProperties->cbPrefix, pProperties->cbAlign) -
				pProperties->cbPrefix;

   ASSERT(pProperties->cbBuffer);

   DbgLog((LOG_TRACE,2,TEXT("Preview: %d buffers, prefix %d size %d align %d"),
			pProperties->cBuffers, pProperties->cbPrefix,
			pProperties->cbBuffer,
			pProperties->cbAlign));

   ALLOCATOR_PROPERTIES Actual;
   return pAllocator->SetProperties(pProperties,&Actual);

   // !!! Are we sure we'll be happy with this?

}


// I know my preview sucks.  Stop telling me.  :-)
HRESULT CVidPreview::Notify(IBaseFilter *pFilter, Quality q)
{
    return NOERROR;
}


// The capture pin is going active ==> We must shut down our capture window
// The capture pin is inactive ==> We need our own capture window now
//
HRESULT CVidPreview::CapturePinActive(BOOL fActive)
{
    DbgLog((LOG_TRACE,2,TEXT("Capture pin says Active=%d"), fActive));

    if (fActive == m_fCapturing)
	return S_OK;
    m_fCapturing = fActive;

    // the capture pin will want to use the h/w.  Close our capture window
    if (fActive && m_hwndCap) {
	DestroyCaptureWindow(m_hwndCap);
	m_hwndCap = NULL;
    }

    // don't make a capture window if we're stopped and don't need it.  We'll
    // make one when we're activated
    if (!fActive && !m_hwndCap && m_hThread) {
	m_hwndCap = CreateCaptureWindow();
        ASSERT(m_hwndCap);
    }
	
    // stop thread from waiting for us to send a valid frame - no more to come
    // it will hang waiting forever if we don't do this
    SetEvent(m_hEventFrameValid);

    // wait until our worker thread notices the difference
    if (m_fRunning)
        WaitForSingleObject(m_hEventActiveChanged, INFINITE);
    ResetEvent(m_hEventActiveChanged);

    return S_OK;
}


// The capture pin is sending us a frame to preview
//
HRESULT CVidPreview::ReceivePreviewFrame(LPVOID lpFrame, int iSize)
{
    // I'm not the least bit interested in previewing right now, or
    // we haven't used the last one yet, or we don't have a place to put it
    if (!m_fRunning || m_fFrameValid || m_lpFrame == NULL) {
        //DbgLog((LOG_TRACE,4,TEXT("Not interested")));
	return S_OK;
    }

    DbgLog((LOG_TRACE,4,TEXT("Capture pin is giving us a preview frame")));

    // !!! can't avoid mem copy without using our own allocator
    // !!! we do this copy memory even if preview pin is OFF (IAMStreamControl)
    // because we can't risk blocking this call by calling CheckStreamState
    CopyMemory(m_lpFrame, lpFrame, iSize);
    m_iFrameSize = iSize;
    m_fFrameValid = TRUE;
    SetEvent(m_hEventFrameValid);
    return S_OK;
}


// make a capture window for our preview pin
//
HWND CVidPreview::CreateCaptureWindow()
{
    BOOL bErr;

    DbgLog((LOG_TRACE,2,TEXT("PREVIEW creating a capture window")));

    HWND hwndCapture;   // The window to return

    hwndCapture = capCreateCaptureWindow(NULL, 0,
                                         0, 0, 150, 150,
                                         NULL, 0 /* ID */);

    if (!hwndCapture) {
        DbgLog((LOG_ERROR|LOG_TRACE, 1, TEXT("CAP Window could not be created") ));
        return NULL;
    }

    bErr = capDriverConnect(hwndCapture,
				m_pCap->m_pCapturePin->m_uiDriverIndex);
    if (!bErr) {
        DestroyWindow(hwndCapture);
        DbgLog((LOG_ERROR|LOG_TRACE, 1, TEXT("Driver failed to connect") ) );
        return NULL;
    }

    // we will be grabbing single frames
    capSetCallbackOnFrame(hwndCapture, &VideoCallback);

    SetWindowLong(hwndCapture, GWL_USERDATA, (LONG) this);

    return hwndCapture;
}


// Disconnect the driver before destroying the window.
//
BOOL CVidPreview::DestroyCaptureWindow(HWND hwnd)
{

    ASSERT(hwnd != NULL);

    DbgLog((LOG_TRACE,2,TEXT("PREVIEW destroying the capture window")));

    // !!! why is this failing?
    BOOL bDriverDisconnected = capDriverDisconnect(hwnd);
    //DbgLog((LOG_TRACE,2,TEXT("Driver disconnect: %x"), bDriverDisconnected));

    BOOL bWindowGone = DestroyWindow(hwnd);
    //DbgLog((LOG_TRACE,2,TEXT("Window destroy: %x"), bWindowGone));

    return (bDriverDisconnected && bWindowGone);
}


//
// VideoCallback
//
// The AVICap Video callback. Keep a copy of the buffer we are given
// May be called after the worker thread, or even the pin has gone away,
// depending on AVICap's internal timing. Therefore be very careful with the
// pointers we use.
//
LRESULT CALLBACK CVidPreview::VideoCallback(HWND hwnd, LPVIDEOHDR lpVHdr)
{

    CVidPreview *pThis = (CVidPreview *)GetWindowLong(hwnd, GWL_USERDATA);
    ASSERT(pThis);

    // I'm not the least bit interested in previewing right now, or
    // we haven't used the last one yet, or we don't have a place to put it
    if (!pThis->m_fRunning || pThis->m_fFrameValid ||
						pThis->m_lpFrame == NULL) {
        //DbgLog((LOG_TRACE,4,TEXT("Not interested")));
	return S_OK;
    }

    DbgLog((LOG_TRACE,4,TEXT("capGrabFrame callback got a preview frame")));

    // !!! can't avoid mem copy without using our own allocator
    // !!! we do this copy memory even if preview pin is OFF (IAMStreamControl)
    // because we can't risk blocking this call by calling CheckStreamState
    CopyMemory(pThis->m_lpFrame, lpVHdr->lpData, lpVHdr->dwBufferLength);
    pThis->m_iFrameSize = lpVHdr->dwBufferLength;
    pThis->m_fFrameValid = TRUE;
    SetEvent(pThis->m_hEventFrameValid);
    return S_OK;
}


DWORD WINAPI CVidPreview::ThreadProcInit(void *pv)
{
    CVidPreview *pThis = (CVidPreview *)pv;
    return pThis->ThreadProc();
}


DWORD CVidPreview::ThreadProc()
{
    IMediaSample *pSample;
    CRefTime rtStart, rtEnd;
    HRESULT hr;
    BOOL fCaptureActive = m_fCapturing;
    int iWait;
    HANDLE hWait[2] = {m_hEventFrameValid, m_hEventRun};

    DbgLog((LOG_TRACE,2,TEXT("CVidPreview ThreadProc")));

    // Send preview frames as long as we're running.  Die when not streaming
    while (1) {

	// only preview while running
	if (!m_fRunning) {
       	    DbgLog((LOG_TRACE,3,TEXT("Preview thread waiting for RUN")));
	    WaitForSingleObject(m_hEventRun, INFINITE);
       	    DbgLog((LOG_TRACE,3,TEXT("Preview thread got RUN")));
 	}
	ResetEvent(m_hEventRun);

	// if we stopped instead of ran
	if (!m_fRunning)
	    break;

	while (m_fRunning) {
	    hr = GetDeliveryBuffer(&pSample, NULL, NULL, 0);
	    if (FAILED(hr))
		break;
	    hr = pSample->GetPointer((LPBYTE *)&m_lpFrame);
	    if (FAILED(hr))
		break;

	    // try to notice if the capture pin starts/stops streaming
	    if (m_fCapturing != fCaptureActive) {
       	        DbgLog((LOG_TRACE,3,TEXT("Preview thread noticed Active=%d"),
				 			m_fCapturing));
		SetEvent(m_hEventActiveChanged);
		fCaptureActive = m_fCapturing;
	    }

	    // the capture pin will send us a frame
	    if (fCaptureActive) {
       	        DbgLog((LOG_TRACE,4,TEXT("PREVIEW using capture picture")));

    		// m_hEventFrameValid, m_hEventRun
		iWait = WaitForMultipleObjects(2, hWait, FALSE, INFINITE);

		// time for our thread to die - don't reset the event because
		// we may need it to fire when we break out of this loop
		if (iWait != WAIT_OBJECT_0) {
       	            DbgLog((LOG_TRACE,2,TEXT("Wait for streaming pic abort1")));
		    m_lpFrame = NULL;	// please don't write here anymore
	    	    pSample->Release();
		    continue;
		}

		// the streaming pin stopped being active... switch again
		if (!m_fFrameValid) {
       	            DbgLog((LOG_TRACE,2,TEXT("Wait for streaming pic abort2")));
		    ResetEvent(m_hEventFrameValid);
		    m_lpFrame = NULL;	// please don't write here anymore
	    	    pSample->Release();
		    continue;
		}

		ResetEvent(m_hEventFrameValid);
	        pSample->SetActualDataLength(m_iFrameSize);

	    // we have our own capture window and are getting our own frames
	    } else {
       	        DbgLog((LOG_TRACE,4,TEXT("PREVIEW using capGrabFrame")));

		// get a frame, and give it to our callback
		capGrabFrame(m_hwndCap);

    		// m_hEventFrameValid, m_hEventRun
		// wait for our callback to get the frame
		iWait = WaitForMultipleObjects(2, hWait, FALSE, INFINITE);

		// time for our thread to die - don't reset the event because
		// we may need it to fire when we break out of this loop
		if (iWait != WAIT_OBJECT_0) {
       	            DbgLog((LOG_TRACE,2,TEXT("Wait for capGrabFrame abort1")));
		    m_lpFrame = NULL;	// please don't write here anymore
	    	    pSample->Release();
		    continue;
		}

		// the streaming pin stopped being active... switch again
		if (!m_fFrameValid) {
       	            DbgLog((LOG_TRACE,2,TEXT("Wait for capGrabFrame abort2")));
		    ResetEvent(m_hEventFrameValid);
		    m_lpFrame = NULL;	// please don't write here anymore
	    	    pSample->Release();
		    continue;
		}

		ResetEvent(m_hEventFrameValid);
	        pSample->SetActualDataLength(m_iFrameSize);
	    }

	    // time stamp the sample
	    if (m_pCap->m_pClock) {
	        m_pCap->m_pClock->GetTime((REFERENCE_TIME *)&rtStart);
      	        rtStart = rtStart - m_pCap->m_tStart;
		if (m_mt.IsValid() &&
			((VIDEOINFOHEADER *)m_mt.Format())->AvgTimePerFrame)
      	            rtEnd = rtStart +
			((VIDEOINFOHEADER *)m_mt.Format())->AvgTimePerFrame;
		else
		    rtEnd = rtStart + 666666;	// assume 15fps

		// !!! NO TIME STAMPS for preview unless we know the latency
		// of the graph... we could drop every frame needlessly since
		// they'll all arrive at the renderer late if we're software
		// decoding these frames.
		//
		// We only send another preview frame when this one is done,
		// so we won't get a backup if decoding is slow.
		// 
		// Actually, adding a latency time would still be broken
		// if the latency was > 1 frame length, because the renderer
		// would hold on to the sample until past the time for the
		// next frame, and we wouldn't send out the next preview frame
		// as soon as we should, and our preview frame rate would suffer
		//
		// But besides all that, we really need time stamps for
		// the stream control stuff to work, so we'll have to live
		// with preview frame rates being inferior if we have an
		// oustanding stream control request.

		AM_STREAM_INFO am;
		GetInfo(&am);
		if (am.dwFlags & AM_STREAM_INFO_START_DEFINED ||
				am.dwFlags & AM_STREAM_INFO_STOP_DEFINED) {
                    //DbgLog((LOG_TRACE,0,TEXT("TIME STAMPING ANYWAY")));
                    pSample->SetTime((REFERENCE_TIME *)&rtStart,
						(REFERENCE_TIME *)&rtEnd);
		}
      	    }

	    // This is important so ReceivePreviewFrame doesn't screw up
	    m_lpFrame = NULL;	// please don't write here anymore
	    m_fFrameValid = FALSE;// next time m_lpFrame is set, Ok to write

	    int iStreamState = CheckStreamState(pSample);
            pSample->SetDiscontinuity(FALSE);
            if (iStreamState == STREAM_FLOWING) {
                DbgLog((LOG_TRACE,4,TEXT("*PREV Sending frame at %d"), (int)rtStart));
	        if (m_fLastSampleDiscarded)
                    pSample->SetDiscontinuity(TRUE);
            } else {
                DbgLog((LOG_TRACE,4,TEXT("*PREVIEW Discarding frame at %d"),
							(int)rtStart));
	        m_fLastSampleDiscarded = TRUE;
            }
	    if (iStreamState == STREAM_FLOWING) {
	        pSample->SetSyncPoint(TRUE);	// !!! not necessarily
	        pSample->SetPreroll(FALSE);
       	        DbgLog((LOG_TRACE,4,TEXT("*Delivering a preview frame")));
	        Deliver(pSample);
	    }
	    pSample->Release();

	    // if previewing ourself, wait for time till next frame
	    // !!! capture pin may wait on this to become active
	    if (!fCaptureActive && m_pCap->m_pClock) {
                hr = m_pCap->m_pClock->AdviseTime(m_rtRun, rtEnd,
            			(HEVENT)(HANDLE) m_EventAdvise, &m_dwAdvise);
            	if (SUCCEEDED(hr)) {
	            m_EventAdvise.Wait();
            	}
		m_dwAdvise = 0;
            }
	}
	// just in case
	SetEvent(m_hEventActiveChanged);
    }

    DbgLog((LOG_TRACE,2,TEXT("CVidPreview ThreadProc is dead")));
    return 0;
}


//
// PIN CATEGORIES - let the world know that we are a PREVIEW pin
//

HRESULT CVidPreview::Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData)
{
    return E_NOTIMPL;
}

// To get a property, the caller allocates a buffer which the called
// function fills in.  To determine necessary buffer size, call Get with
// pPropData=NULL and cbPropData=0.
HRESULT CVidPreview::Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
{
    if (guidPropSet != AMPROPSETID_Pin)
	return E_PROP_SET_UNSUPPORTED;

    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
	return E_PROP_ID_UNSUPPORTED;

    if (pPropData == NULL && pcbReturned == NULL)
	return E_POINTER;

    if (pcbReturned)
	*pcbReturned = sizeof(GUID);

    if (pPropData == NULL)
	return S_OK;

    if (cbPropData < sizeof(GUID))
	return E_UNEXPECTED;

    *(GUID *)pPropData = PIN_CATEGORY_PREVIEW;
    return S_OK;
}


// QuerySupported must either return E_NOTIMPL or correctly indicate
// if getting or setting the property set and property is supported.
// S_OK indicates the property set and property ID combination is
HRESULT CVidPreview::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
    if (guidPropSet != AMPROPSETID_Pin)
	return E_PROP_SET_UNSUPPORTED;

    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
	return E_PROP_ID_UNSUPPORTED;

    if (pTypeSupport)
	*pTypeSupport = KSPROPERTY_SUPPORT_GET;
    return S_OK;
}
