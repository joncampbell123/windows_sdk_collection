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
//  SYSCLOCK.CPP
//    Implements a system clock based on IReferenceClock

#include <streams.h>
#include <limits.h>


#ifdef FILTER_DLL

/* List of class IDs and creator functions for the class factory. This
   provides the link between the OLE entry point in the DLL and an object
   being created. The class factory will call the static CreateInstance
   function when it is asked to create a CLSID_SystemClock object */

CFactoryTemplate g_Templates[1] = {
    {&CLSID_SystemClock, CSystemClock::CreateInstance}
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);
#endif

/* This goes in the factory template table to create new instances */
CUnknown * WINAPI CSystemClock::CreateInstance(LPUNKNOWN pUnk,HRESULT *phr)
{
    return new CSystemClock(NAME("System reference clock"),pUnk, phr);
}


CSystemClock::CSystemClock(TCHAR *pName,LPUNKNOWN pUnk,HRESULT *phr) :
    CBaseReferenceClock(pName, pUnk, phr)
{
}

STDMETHODIMP CSystemClock::NonDelegatingQueryInterface(
    REFIID riid,
    void ** ppv)
{
    if (riid == IID_IPersist)
    {
        return GetInterface(static_cast<IPersist *>(this), ppv);
    }
    else
    {
        return CBaseReferenceClock::NonDelegatingQueryInterface(riid, ppv);
    }
}

/* Return the clock's clsid */
STDMETHODIMP
CSystemClock::GetClassID(CLSID *pClsID)
{
    CheckPointer(pClsID,E_POINTER);
    ValidateReadWritePtr(pClsID,sizeof(CLSID));
    *pClsID = CLSID_SystemClock;
    return NOERROR;
}
