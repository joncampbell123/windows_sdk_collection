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
// System Clock implementation of IReferenceClock
// SYSCLOCK.H

#ifndef __SYSTEMCLOCK__
#define __SYSTEMCLOCK__

//
// Base clock.  Uses timeGetTime ONLY
// Uses most of the code in the base reference clock.
// Provides GetTime
//

class CSystemClock : public CBaseReferenceClock, public IPersist
{
public:
    // We must be able to create an instance of ourselves
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    CSystemClock(TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);

    DECLARE_IUNKNOWN

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void ** ppv);

    // Yield up our class id so that we can be persisted
    // Implement required Ipersist method
    STDMETHODIMP GetClassID(CLSID *pClsID);

}; //CSystemClock

#endif /* __SYSTEMCLOCK__ */
