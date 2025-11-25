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
 * Null Filter - A filter that does nothing and has no pins !!!
 */

class CNoPins : public CBaseFilter
{
public:

    //  Make one of these
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    //  Constructor
    CNoPins(LPUNKNOWN pUnk, HRESULT *phr);

    //  Interface methods
    STDMETHODIMP Run(REFERENCE_TIME tStart);
    STDMETHODIMP Stop();

    //  CBaseFilter methods
    int CNoPins::GetPinCount();     // 0 pins
    CBasePin *CNoPins::GetPin(int iPin);

private:
    //  Locking
    CCritSec m_Lock;

    //  Don't send EC_COMPLETE twice
    BOOL     m_bComplete;
};

/*  Our class id */
// NoPins filter object
// a8fbb9c0-92e1-11cf-b4d1-00805f6cbbea
DEFINE_GUID(CLSID_NoPins,
0xa8fbb9c0, 0x92e1, 0x11cf, 0xb4, 0xd1, 0x00, 0x80, 0x5f, 0x6c, 0xbb, 0xea);


