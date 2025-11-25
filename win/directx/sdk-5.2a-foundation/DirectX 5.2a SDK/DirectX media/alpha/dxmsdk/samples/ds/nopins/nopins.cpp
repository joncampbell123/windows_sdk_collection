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

#include <streams.h>
#include <initguid.h>
#include "nopins.h"

// setup data

const AMOVIESETUP_FILTER sudNopins = { &CLSID_NoPins       // clsID
                                     , L"NoPins filter"    // strName
                                     , MERIT_DO_NOT_USE    // dwMerit
                                     , 0                   // nPins
                                     , NULL };             // lpPin

CFactoryTemplate g_Templates[]= { L"NoPins filter"
                                , &CLSID_NoPins
                                , CNoPins::CreateInstance
                                , NULL
                                , &sudNopins };
int g_cTemplates = 1;

/*
 * Null Filter - A filter that does nothing and has no pins !!!
 */

CNoPins::CNoPins(LPUNKNOWN pUnk, HRESULT *phr) :
    CBaseFilter(NAME("NoPins"), pUnk, &m_Lock, CLSID_NoPins),
    m_bComplete(FALSE)
{
}

/*
   CreateInstance
*/
CUnknown * WINAPI CNoPins::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    CNoPins *pNewObject = new CNoPins(punk, phr);
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

/*  Pin enumeration */
int CNoPins::GetPinCount()
{
    return 0;
}

CBasePin *CNoPins::GetPin(int i)
{
    UNREFERENCED_PARAMETER(i);
    return NULL;
}

STDMETHODIMP CNoPins::Run(REFERENCE_TIME tStart)
{
#if 0 // No need for this if we don't support IMediaPosition
    CAutoLock lck(&m_Lock);
    if (!m_bComplete) {
        NotifyEvent(EC_COMPLETE, 0, 0);
        m_bComplete = TRUE;
    }
#endif
    return CBaseFilter::Run(tStart);
}

STDMETHODIMP CNoPins::Stop()
{
    CAutoLock lck(&m_Lock);
    m_bComplete = FALSE;
    return CBaseFilter::Stop();
}

/******************************Public*Routine******************************\
* exported entry points for registration and
* unregistration (in this case they only call
* through to default implmentations).
*
*
*
* History:
*
\**************************************************************************/
STDAPI
DllRegisterServer()
{
  return AMovieDllRegisterServer2( TRUE );
}

STDAPI
DllUnregisterServer()
{
  return AMovieDllRegisterServer2( FALSE );
}

