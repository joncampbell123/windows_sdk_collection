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

#include <streams.h>        // ActiveMovie base class definitions
#include <mmsystem.h>       // Needed for definition of timeGetTime
#include <limits.h>         // Standard data type limit definitions
#include <measure.h>        // Used for time critical log functions

#include "amextra.h"

#pragma warning(disable:4355)

//  Implements CRenderedInputPin class

CRenderedInputPin::CRenderedInputPin(TCHAR *pObjectName,
                                     CBaseFilter *pFilter,
                                     CCritSec *pLock,
                                     HRESULT *phr,
                                     LPCWSTR pName) :
    CBaseInputPin(pObjectName, pFilter, pLock, phr, pName),
    m_bAtEndOfStream(FALSE),
    m_bCompleteNotified(FALSE)
{
}


// Flush end of stream condition - caller should do any
// necessary stream level locking before calling this

STDMETHODIMP CRenderedInputPin::EndOfStream()
{
    HRESULT hr = CheckStreaming();

    //  Do EC_COMPLETE handling for rendered pins
    if (S_OK == hr  && !m_bAtEndOfStream) {
        m_bAtEndOfStream = TRUE;
        FILTER_STATE fs;
        EXECUTE_ASSERT(SUCCEEDED(m_pFilter->GetState(0, &fs)));
        if (fs == State_Running) {
            DoCompleteHandling();
        }
    }
    return hr;
}


// Called to complete the flush

STDMETHODIMP CRenderedInputPin::EndFlush()
{
    CAutoLock lck(m_pLock);

    // Clean up renderer state
    m_bAtEndOfStream = FALSE;
    m_bCompleteNotified = FALSE;

    return CBaseInputPin::EndFlush();
}


// Notify of Run() from filter

HRESULT CRenderedInputPin::Run(REFERENCE_TIME tStart)
{
    UNREFERENCED_PARAMETER(tStart);
    m_bCompleteNotified = FALSE;
    if (m_bAtEndOfStream) {
        DoCompleteHandling();
    }
    return S_OK;
}


//  Clear status on going into paused state

HRESULT CRenderedInputPin::Active()
{
    m_bAtEndOfStream = FALSE;
    m_bCompleteNotified = FALSE;
    return CBaseInputPin::Active();
}


//  Do stuff to deliver end of stream

void CRenderedInputPin::DoCompleteHandling()
{
    ASSERT(m_bAtEndOfStream);
    if (!m_bCompleteNotified) {
        m_bCompleteNotified = TRUE;
        m_pFilter->NotifyEvent(EC_COMPLETE, 0, 0);
    }
}

