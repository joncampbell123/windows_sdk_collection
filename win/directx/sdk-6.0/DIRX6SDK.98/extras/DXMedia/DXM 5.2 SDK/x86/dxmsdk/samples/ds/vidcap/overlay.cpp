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

    Methods for CVidOverlay, CVidOverlayNotify

    This is a preview pin, which will use the hardware overlay capabilities
    of the driver to make it draw into the window of the video renderer that
    this pin is connected to.  We connect to the renderer through IOverlay,
    meaning that the renderer tells us where the window is at all times, and
    it's up to us to draw in it.

    When we get RUN, if the capture pin is connected, it will have made
    a capture window already, and we must use that capture window, since
    there can be only one (it takes the hardware).  If the capture pin
    isn't connected, we create the capture window.  In any case, we make it
    a child of the video renderer's window and always make it fill up the
    video renderer's window's rectangle, and then turn overlay on, and
    VOILA!  we have preview.  When we stop streaming, we destroy the
    capture window (if we made it) so the capture pin get make one again
    if it needs to.

    Reading this code is very useful for figuring out how to make a pin
    that connects to the renderer through IOverlay.

*/

#include <streams.h>
#include <mmsystem.h>
#include <vfw.h>
#include "vidcap.h"

// CVidOverlay constructor
//
CVidOverlay::CVidOverlay(TCHAR *pObjectName, CVidCap *pCapture,
        HRESULT * phr, LPCWSTR pName)
   :
   CBaseOutputPin(pObjectName, pCapture, pCapture->pStateLock(), phr, pName),
   m_OverlayNotify(NAME("Overlay notification interface"), pCapture, NULL, phr),
   m_pCap(pCapture),
   m_fRunning(FALSE),
   m_hwndCap(NULL)
{
   DbgLog((LOG_TRACE,1,TEXT("CVidOverlay constructor")));
   ASSERT(pCapture);
}


CVidOverlay::~CVidOverlay()
{
    DbgLog((LOG_TRACE,1,TEXT("*Destroying the Overlay pin")));
};


STDMETHODIMP CVidOverlay::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    DbgLog((LOG_TRACE,99,TEXT("CVidOverlay::QueryInterface")));
    if (ppv)
	*ppv = NULL;

    /* Do we have this interface */

    if (riid == IID_IKsPropertySet) {
        return GetInterface((LPUNKNOWN) (IKsPropertySet *) this, ppv);
    } else {
        return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
    }
}


// Overridden to connect with IOverlay instead of IMemInputPin
// Say if we're prepared to connect to a given input pin from
// this output pin
//
STDMETHODIMP CVidOverlay::Connect(IPin *pReceivePin,
                                        const AM_MEDIA_TYPE *pmt)
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlay::Connect")));

    /*  Call the base class to make sure the directions match! */
    HRESULT hr = CBaseOutputPin::Connect(pReceivePin,pmt);
    if (FAILED(hr)) {
        return hr;
    }
    /*  We're happy if we can get an IOverlay interface */

    hr = pReceivePin->QueryInterface(IID_IOverlay,
                                     (void **)&m_pOverlay);

    // we were promised this would work
    ASSERT(SUCCEEDED(hr));

    /*  Because we're not going to get called again - except to
        propose a media type - we set up a callback here.

        There's only one overlay pin so we don't need any context.
    */

    hr = m_pOverlay->Advise(&m_OverlayNotify,
                            ADVISE_CLIPPING | ADVISE_POSITION);

    /*
        We don't need to hold on to the IOverlay pointer
        because BreakConnect will be called before the receiving
        pin goes away.
    */


    if (FAILED(hr)) {
	// !!! Shouldn't happen, but this isn't quite right
        Disconnect();
	pReceivePin->Disconnect();
        return hr;
    } else {
        m_bAdvise = TRUE;
    }

    return hr;
}


// Overridden to connect with IOverlay instead of IMemInputPin
//
HRESULT CVidOverlay::BreakConnect()
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlay::BreakConnect")));

    // we must get rid of our overlay notify advise now
    if (m_pOverlay != NULL) {
        if (m_bAdvise) {
            m_pOverlay->Unadvise();
            m_bAdvise = FALSE;
        }
        m_pOverlay->Release();
        m_pOverlay = NULL;
    }

    return CBaseOutputPin::BreakConnect();
}


// Overridden to connect with IOverlay instead of IMemInputPin
// Override this because we don't want any allocator!
//
HRESULT CVidOverlay::DecideAllocator(IMemInputPin * pPin,
                        IMemAllocator ** pAlloc) {
    /*  We just don't want one so everything's OK as it is */
    return S_OK;
}


// Overridden to connect with IOverlay instead of IMemInputPin
HRESULT CVidOverlay::GetMediaType(int iPosition, CMediaType *pmt)
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlay::GetMediaType #%d"), iPosition));

    if (pmt == NULL) {
        DbgLog((LOG_TRACE,3,TEXT("NULL format, no can do")));
	return E_INVALIDARG;
    }
	
    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    if (iPosition > 0) {
        return VFW_S_NO_MORE_ITEMS;
    }

    // We provide a media type of OVERLAY with an 8 bit format (the
    // renderer won't accept it if we don't set up an 8 bit format)

    BYTE aFormat[sizeof(VIDEOINFOHEADER) + SIZE_PALETTE];
    VIDEOINFOHEADER *pFormat = (VIDEOINFOHEADER *)aFormat;
    ZeroMemory(pFormat, sizeof(VIDEOINFOHEADER) + SIZE_PALETTE);

    // make the overlay window the same size as what's being captured
    if (m_pCap->m_pCapturePin->m_mt.IsValid()) {
        pFormat->bmiHeader.biWidth =
			HEADER(m_pCap->m_pCapturePin->m_mt.Format())->biWidth;
        pFormat->bmiHeader.biHeight =
			HEADER(m_pCap->m_pCapturePin->m_mt.Format())->biHeight;
    } else {
        pFormat->bmiHeader.biWidth = 320;
        pFormat->bmiHeader.biHeight = 240;
    }

    pFormat->bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
    pFormat->bmiHeader.biPlanes = 1;
    pFormat->bmiHeader.biBitCount = 8;

    pmt->SetFormat((PBYTE)pFormat, sizeof(VIDEOINFOHEADER) + SIZE_PALETTE);
    pmt->SetFormatType(&FORMAT_VideoInfo);

    if (pmt->pbFormat == NULL) {
        return E_OUTOFMEMORY;
    }

    pmt->majortype = MEDIATYPE_Video;
    pmt->subtype   = MEDIASUBTYPE_Overlay;
    pmt->bFixedSizeSamples    = FALSE;
    pmt->bTemporalCompression = FALSE;	
    pmt->lSampleSize          = 0;

    return NOERROR;
}


// Overridden to connect with IOverlay instead of IMemInputPin
// We accept overlay connections only
//
HRESULT CVidOverlay::CheckMediaType(const CMediaType *pMediaType)
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlay::CheckMediaType")));
    if (pMediaType->subtype == MEDIASUBTYPE_Overlay)
        return NOERROR;
    else
	return E_FAIL;
}


// Overridden to connect with IOverlay instead of IMemInputPin
//
HRESULT CVidOverlay::CheckConnect(IPin *pPin)
{
    // we don't connect to anyone who doesn't support IOverlay.
    // after all, we're an overlay pin
    HRESULT hr = pPin->QueryInterface(IID_IOverlay, (void **)&m_pOverlay);

    if (FAILED(hr)) {
        return E_NOINTERFACE;
    } else {
	m_pOverlay->Release();
	m_pOverlay = NULL;
    }

    return CBasePin::CheckConnect(pPin);
}


// Overridden to connect with IOverlay instead of IMemInputPin
//
HRESULT CVidOverlay::Active()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidOverlay Stop->Pause")));
    // don't let the base class Active() get called for non-IMemInput pins
    return NOERROR;
}


// Overridden to connect with IOverlay instead of IMemInputPin
//
HRESULT CVidOverlay::Inactive()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidOverlay Pause->Stop")));

    // we made our own capture window since the capture pin doesn't have one
    if (m_pCap->m_pCapturePin->m_hwCapCapturing == NULL && m_hwndCap)
	DestroyCaptureWindow(m_hwndCap);

    // don't let the base class Inactive() get called for non-IMemInput pins
    return NOERROR;
}


// we want to know when we start running... that's when we are supposed
// to start previewing... NOT when we start streaming like normal filters
// (when paused)
//
HRESULT CVidOverlay::ActiveRun(REFERENCE_TIME tStart)
{
    DbgLog((LOG_TRACE,2,TEXT("CVidOverlay Pause->Run")));

    ASSERT(m_pCap->m_pOverlayPin->IsConnected());

    m_fRunning = TRUE;

    // something's wrong...
    if (m_pOverlay == NULL)
	return NOERROR;

    m_hwndCap = m_pCap->m_pCapturePin->m_hwCapCapturing;

    HWND hwndRender;
    m_pOverlay->GetWindowHandle(&hwndRender);
    if (hwndRender == NULL)
	return NOERROR;

    // it appears the capture pin is not connected up.  We'll need to make
    // our own capture window.  We'll destroy it when we stop, so we never
    // get confused if somebody connects the capture pin up after that.
    if (m_hwndCap == NULL) {
        DbgLog((LOG_TRACE,2,TEXT("OVERLAY creating its own capwin")));
        // this will make the cap win a child of the renderer
	m_hwndCap = CreateCaptureWindow(hwndRender);
	if (m_hwndCap == NULL)
	   return NOERROR;
    } else {
	// !!! THIS HANGS IN SEND MESSAGE in SetWindowLong!
        // make the capture window a child of the renderer so it moves with
        // the renderer window and overlay will appear in the renderer window
        LONG lStyle = GetWindowLong(m_hwndCap, GWL_STYLE);
        lStyle |= WS_CHILD;
        SetWindowLong(m_hwndCap, GWL_STYLE, lStyle);
        SetParent(m_hwndCap, hwndRender);
    }

    // now turn overlay ON
    DbgLog((LOG_TRACE,2,TEXT("Turning OVERLAY ON")));
    capOverlay(m_hwndCap, TRUE);

    return NOERROR;
}


// we need to stop overlay when we transition from RUN to PAUSE... not
// from PAUSE to STOP like normal filters -- that's the rule for capture
// filters
//
HRESULT CVidOverlay::ActivePause()
{
    DbgLog((LOG_TRACE,2,TEXT("CVidOverlay Run->Pause")));

    m_fRunning = FALSE;

    // turn overlay off and stop being a child of the renderer
    if (m_hwndCap) {
        DbgLog((LOG_TRACE,2,TEXT("Turning OVERLAY off")));
        capOverlay(m_hwndCap, FALSE);
        LONG lStyle = GetWindowLong(m_hwndCap, GWL_STYLE);
        lStyle &= ~WS_CHILD;
        SetWindowLong(m_hwndCap, GWL_STYLE, lStyle);
        SetParent(m_hwndCap, NULL);
    }

    return NOERROR;
}


// make a capture window for our overlay pin, and make it a child of the
// video renderer window passed in
//
HWND CVidOverlay::CreateCaptureWindow(HWND hwndP)
{
    BOOL bErr;

    HWND hwndCapture;   // The window to return

    // make the capture window a child of the renderer window passed in
    hwndCapture = capCreateCaptureWindow(NULL, WS_CHILD | WS_VISIBLE,
                                         0, 0, 150, 150,
                                         hwndP, 261 /* ID */);

    if (!hwndCapture) {
        DbgLog((LOG_ERROR|LOG_TRACE, 1, TEXT("CAP Window could not be created") ));
        return NULL;
    }
    //DbgLog((LOG_TRACE,2,TEXT("OVERLAY created capture window")));

    // connect to the hardware device we're supposed to use
    bErr = capDriverConnect(hwndCapture,
				m_pCap->m_pCapturePin->m_uiDriverIndex);
    if (!bErr) {
        DestroyWindow(hwndCapture);
        DbgLog((LOG_ERROR|LOG_TRACE, 1, TEXT("Driver failed to connect") ) );
        return NULL;
    }

    return hwndCapture;
}


// Disconnect the driver before destroying the window.
//
BOOL CVidOverlay::DestroyCaptureWindow(HWND hwnd)
{

    ASSERT(hwnd != NULL);

    // !!! why is this failing?
    BOOL bDriverDisconnected = capDriverDisconnect(hwnd);
    //DbgLog((LOG_TRACE,2,TEXT("Driver disconnect: %x"), bDriverDisconnected));

    BOOL bWindowGone = DestroyWindow(hwnd);
    //DbgLog((LOG_TRACE,2,TEXT("Window destroy: %x"), bWindowGone));

    return (bDriverDisconnected && bWindowGone);
}


//
// PIN CATEGORIES - let the world know that we are a PREVIEW pin
//

HRESULT CVidOverlay::Set(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData)
{
    return E_NOTIMPL;
}

// To get a property, the caller allocates a buffer which the called
// function fills in.  To determine necessary buffer size, call Get with
// pPropData=NULL and cbPropData=0.
HRESULT CVidOverlay::Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
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
HRESULT CVidOverlay::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
    if (guidPropSet != AMPROPSETID_Pin)
	return E_PROP_SET_UNSUPPORTED;

    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
	return E_PROP_ID_UNSUPPORTED;

    if (pTypeSupport)
	*pTypeSupport = KSPROPERTY_SUPPORT_GET;
    return S_OK;
}




//=========================================================================//
//***			I N T E R M I S S I O N				***//
//=========================================================================//




/*
        IOverlayNotify - how the renderer tells us where the window is
			 and when it needs repainting
*/

CVidOverlayNotify::CVidOverlayNotify(TCHAR              * pName,
                               CVidCap 	  * pFilter,
                               LPUNKNOWN            pUnk,
                               HRESULT            * phr) :
    CUnknown(pName, pUnk)
{
    DbgLog((LOG_TRACE,1,TEXT("*Instantiating CVidOverlayNotify")));
    m_pFilter = pFilter;
    m_rcClient.top = -1;	// something unusual
}


CVidOverlayNotify::~CVidOverlayNotify()
{
    DbgLog((LOG_TRACE,1,TEXT("*Destroying CVidOverlayNotify")));
}


STDMETHODIMP CVidOverlayNotify::NonDelegatingQueryInterface(REFIID riid,
                                                         void ** ppv)
{
    DbgLog((LOG_TRACE,99,TEXT("CVidOverlayNotify::QueryInterface")));
    if (ppv)
	*ppv = NULL;

    /* Do we have this interface */

    if (riid == IID_IOverlayNotify) {
        return GetInterface((LPUNKNOWN) (IOverlayNotify *) this, ppv);
    } else {
        return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }
}


STDMETHODIMP_(ULONG) CVidOverlayNotify::NonDelegatingRelease()
{
    return m_pFilter->Release();
}


STDMETHODIMP_(ULONG) CVidOverlayNotify::NonDelegatingAddRef()
{
    return m_pFilter->AddRef();
}


STDMETHODIMP CVidOverlayNotify::OnColorKeyChange(
    const COLORKEY *pColorKey)          // Defines new colour key
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlayNotify::OnColorKeyChange")));

// We expect the hardware to handle colour key stuff, so I'm really
// hoping that the renderer will never draw the colour key itself.

    return NOERROR;
}


// The calls to OnClipChange happen in sync with the window. So it's called
// with an empty clip list before the window moves to freeze the video, and
// then when the window has stabilised it is called again with the new clip
// list. The OnPositionChange callback is for overlay cards that don't want
// the expense of synchronous clipping updates and just want to know when
// the source or destination video positions change. They will NOT be called
// in sync with the window but at some point after the window has changed
// (basicly in time with WM_SIZE etc messages received). This is therefore
// suitable for overlay cards that don't inlay their data to the framebuffer

STDMETHODIMP CVidOverlayNotify::OnClipChange(
    const RECT    * pSourceRect,         // Area of source video to use
    const RECT    * pDestinationRect,    // screen co-ords of window
    const RGNDATA * pRegionData)         // Header describing clipping
{

    // we get a LOT of these messages.  Try to save time by ignore many
    // of them

    if (!m_pFilter->m_pOverlayPin)
	return NOERROR;

    if (!m_pFilter->m_pOverlayPin->IsConnected())
	return NOERROR;

    if (IsRectEmpty(pSourceRect) && IsRectEmpty(pDestinationRect))
	return NOERROR;

    HWND hwnd = NULL;
    RECT rcC;
    if (m_pFilter->m_pOverlayPin->m_pOverlay)
        m_pFilter->m_pOverlayPin->m_pOverlay->GetWindowHandle(&hwnd);

    if (hwnd == NULL || !IsWindowVisible(hwnd))
	return NOERROR;

    // It's up to us to keep garbage out of the window by painting it if
    // we're not running, and the hardware has nothing to draw
    if (!m_pFilter->m_pOverlayPin->m_fRunning) {
        HDC  hdc;
        if (hwnd)
	    hdc = GetDC(hwnd);
        if (hdc == NULL)
	    return NOERROR;
	GetClientRect(hwnd, &rcC);
	HBRUSH hbrOld = (HBRUSH)SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	PatBlt(hdc, 0, 0, rcC.right, rcC.bottom, PATCOPY);
	SelectObject(hdc, hbrOld);
        ReleaseDC(hwnd, hdc);
	return NOERROR;
    }

    DbgLog((LOG_TRACE,3,TEXT("OnClip/PositionChange (%d,%d) (%d,%d)"),
        		pSourceRect->right - pSourceRect->left,
        		pSourceRect->bottom - pSourceRect->top,
        		pDestinationRect->right - pDestinationRect->left,
        		pDestinationRect->bottom - pDestinationRect->top));

    // make the capture window fill up the renderer window
    if (m_pFilter->m_pOverlayPin->m_hwndCap) {
        GetClientRect(hwnd, &rcC);
        if (rcC.left != m_rcClient.left || rcC.top != m_rcClient.top ||
        			rcC.right != m_rcClient.right ||
				rcC.bottom != m_rcClient.bottom) {
	    SetWindowPos(m_pFilter->m_pOverlayPin->m_hwndCap, NULL,
			rcC.left, rcC.top, rcC.right, rcC.bottom, SWP_NOZORDER);
            DbgLog((LOG_TRACE,3,TEXT("Moving CAPWIN to (%d,%d,%d,%d)"),
			rcC.left, rcC.top, rcC.right, rcC.bottom));
	    m_rcClient = rcC;
	}
    }

    return NOERROR;
}


STDMETHODIMP CVidOverlayNotify::OnPaletteChange(
    DWORD dwColors,                     // Number of colours present
    const PALETTEENTRY *pPalette)       // Array of palette colours
{
    DbgLog((LOG_TRACE,3,TEXT("CVidOverlayNotify::OnPaletteChange")));

    return NOERROR;
}


STDMETHODIMP CVidOverlayNotify::OnPositionChange(
    const RECT *pSourceRect,            // Area of video to play with
    const RECT *pDestinationRect)       // Area video goes
{

    return OnClipChange(pSourceRect, pDestinationRect, NULL);
}
