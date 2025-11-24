/*** 
*CFPoint.cpp - The CPoint Class Factory.
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module implements the CPoint class factory.
*
*Implementation Notes:
*
*****************************************************************************/

#include <windows.h>
#include <ole2.h>
#include <dispatch.h>

#include "spoly.h"
#include "cpoint.h"


CPointCF::CPointCF()
{
    m_refs = 0;
}


CPointCF::~CPointCF()
{
}


IClassFactory FAR*
CPointCF::Create()
{
    CPointCF FAR* pCF;

    if((pCF = new FAR CPointCF()) == NULL)
      return NULL;
    pCF->AddRef();
    return pCF;
}


//---------------------------------------------------------------------
//                     IUnknown Methods
//---------------------------------------------------------------------


STDMETHODIMP
CPointCF::QueryInterface(REFIID riid, void FAR* FAR* ppv) 
{
    if(riid == IID_IUnknown || riid == IID_IClassFactory){
      *ppv = this;
      ++m_refs;
      return NOERROR;
    }
    *ppv = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG)
CPointCF::AddRef(void)
{
    return ++m_refs;
}

STDMETHODIMP_(ULONG)
CPointCF::Release(void)
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
CPointCF::CreateInstance(
    IUnknown FAR* pUnkOuter,
    REFIID riid,
    void FAR* FAR* ppv)
{
    HRESULT hresult;
    CPoint FAR *ppoint;

    if((ppoint = CPoint::Create()) == NULL){
      *ppv = NULL;
      return ResultFromScode(E_OUTOFMEMORY);
    }
    hresult = ppoint->QueryInterface(riid, ppv);
    ppoint->Release();
    return hresult;
}


STDMETHODIMP
CPointCF::LockServer(BOOL fLock)
{
    return NOERROR;
}
