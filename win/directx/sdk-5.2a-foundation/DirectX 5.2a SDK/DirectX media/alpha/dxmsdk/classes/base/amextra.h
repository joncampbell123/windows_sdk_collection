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

#ifndef __AMEXTRA__
#define __AMEXTRA__

// Simple rendered input pin
//
// NOTE if your filter queues stuff before rendering then it may not be
// appropriate to use this class
//
// In that case queue the end of stream condition until the last sample
// is actually rendered and flush the condition appropriately

class CRenderedInputPin : public CBaseInputPin
{
public:

    CRenderedInputPin(TCHAR *pObjectName,
                      CBaseFilter *pFilter,
                      CCritSec *pLock,
                      HRESULT *phr,
                      LPCWSTR pName);

    // Override methods to track end of stream state
    STDMETHODIMP EndOfStream();
    STDMETHODIMP EndFlush();

    HRESULT Active();
    HRESULT Run(REFERENCE_TIME tStart);

protected:

    // Member variables to track state
    BOOL m_bAtEndOfStream;      // Set by EndOfStream
    BOOL m_bCompleteNotified;   // Set when we notify for EC_COMPLETE

private:
    void DoCompleteHandling();
};

#endif // __AMEXTRA__

