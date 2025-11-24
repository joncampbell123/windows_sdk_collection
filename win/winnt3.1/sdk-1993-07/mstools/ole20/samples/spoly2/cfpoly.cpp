/*** 
*cfpoly.cpp - The CPoly Class Factory.
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module implements the CPoly class factory.
*
*Implementation Notes:
*
*****************************************************************************/

#include <windows.h>
#include <ole2.h>
#include <dispatch.h>

#include "spoly.h"
#include "cpoint.h"
#include "cpoly.h"


CPolyCF::CPolyCF()
{
    m_refs = 0;
}


CPolyCF::~CPolyCF()
{
}


IClassFactory FAR*
CPolyCF::Create()
{
    CPolyCF FAR* pCF;

    if((pCF = new FAR CPolyCF()) == NULL)
      return NULL;
    pCF->AddRef();
    return pCF;
}


//---------------------------------------------------------------------
//                    IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CPolyCF::QueryInterface(REFIID iid, void FAR* FAR* ppv) 
{
    if(iid == IID_IUnknown || iid == IID_IClassFactory){
      *ppv = this;
      ++m_refs;
      return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CPolyCF::AddRef(void)
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CPolyCF::Release(void)
{
    if(--m_refs == 0){
      delete this;
      return 0;
    }
    return m_refs;
}


//---------------------------------------------------------------------
//                   IClassFactory Methods
//---------------------------------------------------------------------


STDMETHODIMP
CPolyCF::CreateInstance(
    IUnknown FAR* pUnkOuter,
    REFIID iid,
    void FAR* FAR* ppv)
{
    HRESULT hresult;
    CPoly FAR *ppoly;

    if((ppoly = CPoly::Create()) == NULL){
      *ppv = NULL;
      return ResultFromScode(E_OUTOFMEMORY);
    }
    hresult = ppoly->QueryInterface(iid, ppv);
    ppoly->Release();
    return hresult;
}


STDMETHODIMP
CPolyCF::LockServer(BOOL fLock)
{
    return NOERROR;
}
