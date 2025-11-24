/*** 
*dispcalc.cpp
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module implements the basic user interface and arithmetic
*  functionality of the IDispatch calculator. 
*
*  The implementation of IDispatch it via aggregation with an
*  instance of the "standard" IDispatch implementation, which is
*  initialized with a DispTypeInfo constructed from an INTERFACEDATA
*  description.
*
*Implementation Notes:
*
*****************************************************************************/

#include <windows.h>
#include <ole2.h>
#if !defined(WIN32)
#include <olenls.h>
#endif
#include <dispatch.h>

#include "resource.h"
#include "dispcalc.h"


/***
*CCalc *CCalc::Create(void)
*Purpose:
*  Create an instance of the IDispatch calculator, build a
*  TypeInfo that describes the exposed functionality and
*  aggregate with an instance of CStdDispatch that has been
*  initialized with this TypeInfo.
*
*Entry:
*  None
*
*Exit:
*  return value = CCalc*, NULL if the creation failed.
*
***********************************************************************/
CCalc FAR*
CCalc::Create()
{
    HRESULT hresult;
    CCalc FAR* pcalc;
    ITypeInfo FAR* ptinfo;
    IUnknown FAR* punkStdDisp;
extern INTERFACEDATA NEAR g_idataCCalc;


    if((pcalc = new FAR CCalc()) == NULL)
      return NULL;

    // Build a TypeInfo for the functionality on this object that
    // is being exposing for external programmability.
    //
    hresult = CreateDispTypeInfo(
      &g_idataCCalc, LOCALE_SYSTEM_DEFAULT, &ptinfo);
    if(hresult != NOERROR)
      goto LError0;

    // Create and aggregate with an instance of the default
    // implementation of IDispatch that is initialized with our
    // TypeInfo.
    //
    hresult = CreateStdDispatch(
      pcalc,				// controlling unknown
      &pcalc->m_arith,			// vtable* to dispatch on
      ptinfo,
      &punkStdDisp);

    ptinfo->Release();

    if(hresult != NOERROR)
      goto LError0;

    pcalc->m_punkStdDisp = punkStdDisp;

    return pcalc;

LError0:;
    pcalc->Release();

    return NULL;
}


//---------------------------------------------------------------------
//                        IUnknown methods
//---------------------------------------------------------------------


STDMETHODIMP
CCalc::QueryInterface(REFIID riid, void FAR* FAR* ppv)
{
    if(riid == IID_IUnknown){
      *ppv = this;
    }else
    if(riid == IID_IDispatch){
      return m_punkStdDisp->QueryInterface(riid, ppv);
    }else
      return ResultFromScode(E_NOINTERFACE);

    AddRef();
    return NOERROR;
}


STDMETHODIMP_(ULONG)
CCalc::AddRef()
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CCalc::Release()
{
    if(--m_refs == 0){
      if(m_punkStdDisp != NULL)
	m_punkStdDisp->Release();
      PostQuitMessage(0);
      delete this;
      return 0;
    }
    return m_refs;
}


//---------------------------------------------------------------------
//                       Arithmetic features
//---------------------------------------------------------------------


STDMETHODIMP_(void)
CCalc::CArith::Clear()
{
    m_opnd = 0;
    m_accum = 0;
    m_op = OP_NONE;
    m_state = STATE_LOPND;
}

STDMETHODIMP_(void)
CCalc::CArith::put_Accum(long l)
{
    m_accum = l;
}


STDMETHODIMP_(long)
CCalc::CArith::get_Accum()
{
    return m_accum;
}


STDMETHODIMP_(void)
CCalc::CArith::put_Opnd(long l)
{
    m_opnd = l;
}


STDMETHODIMP_(long)
CCalc::CArith::get_Opnd()
{
    return m_opnd;
}


STDMETHODIMP_(void)
CCalc::CArith::put_Op(short op)
{
    m_op = op;
}


STDMETHODIMP_(short)
CCalc::CArith::get_Op()
{
    return m_op;
}


STDMETHODIMP_(BOOL)
CCalc::CArith::Eval()
{
    if(m_op == OP_NONE)
      return FALSE;

    switch(m_op){
    case OP_PLUS:
      m_accum += m_opnd;
      break;
    case OP_MINUS:
      m_accum -= m_opnd;
      break;
    case OP_MULT:
      m_accum *= m_opnd;
      break;
    case OP_DIV:
      m_accum = (m_opnd == 0) ? 0 : (m_accum / m_opnd);
      break;
    default:
      // ASSERT(UNREACHED);
      return FALSE;
      
    }

    m_state = STATE_EVAL;

    return TRUE;
}


//---------------------------------------------------------------------
//                       User Interface features
//---------------------------------------------------------------------


/***
*void CCalc::CArith::Display()
*Purpose:
*  Display the contents of the register currently being edited.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
STDMETHODIMP_(void)
CCalc::CArith::Display()
{
    VARIANT var;

    V_VT(&var) = VT_I4;
    V_I4(&var) = (m_state == STATE_ROPND) ? m_opnd : m_accum;
    VariantChangeType(&var, &var, 0, VT_BSTR);
    SetDlgItemText(m_pcalc->m_hwnd, IDC_DISPLAY, V_BSTR(&var));
    VariantClear(&var);
}


STDMETHODIMP_(BOOL)
CCalc::CArith::Button(BSTR bstrButton)
{
    int i, button;

static struct {
    char ch;
    int idc;
} NEAR rgIdcOfCh[] = {
      { '+', IDC_PLUS   }
    , { '-', IDC_MINUS  }
    , { '*', IDC_MULT   }
    , { '/', IDC_DIV    }
    , { 'C', IDC_CLEAR  }
    , { 'c', IDC_CLEAR  }
    , { '=', IDC_EQUALS }
    , { '0', IDC_ZERO   }
    , { '1', IDC_ONE    }
    , { '2', IDC_TWO    }
    , { '3', IDC_THREE  }
    , { '4', IDC_FOUR   }
    , { '5', IDC_FIVE   }
    , { '6', IDC_SIX    }
    , { '7', IDC_SEVEN  }
    , { '8', IDC_EIGHT  }
    , { '9', IDC_NINE   }
    , { -1 , -1         }
};

    // if the string is more that 1 character long, then we know its wrong.
    if(SysStringLen(bstrButton) > 1)
      return FALSE;

    // translate button string into control ID
    for(i = 0;; ++i){
      if(rgIdcOfCh[i].ch == -1)
	return FALSE;
      if(rgIdcOfCh[i].ch == bstrButton[0]){
	button = rgIdcOfCh[i].idc;
	break;
      }
    }

    return ButtonPush(button);
}


// the following method is internal, and not exposed for programmability
BOOL
CCalc::CArith::ButtonPush(int button)
{
    if(button >= IDC_ZERO && button <= IDC_NINE){

      long lVal = button - IDC_ZERO;

      switch(m_state){
      case STATE_EVAL:
	m_accum = lVal;
	m_state = STATE_LOPND;
	break;
      case STATE_OP:
	m_opnd = lVal;
	m_state = STATE_ROPND;
	break;
      case STATE_LOPND:
	m_accum = (m_accum * 10) + lVal;
	break;
      case STATE_ROPND:
	m_opnd  = (m_opnd * 10) + lVal;
	break;
      }

    }else if(button >= IDC_PLUS && button <= IDC_DIV){

      if(m_state == STATE_LOPND){
	m_opnd  = m_accum;
	m_state = STATE_OP;
	m_op    = button - IDC_PLUS + OP_PLUS;
      }

    }else if(button == IDC_EQUALS){

      if(m_state > STATE_LOPND)
        Eval();

    }else if (button == IDC_CLEAR){

      Clear();

    }

    SendMessage(m_pcalc->m_hwnd, BM_SETSTATE, 1, 0L);
    SendMessage(m_pcalc->m_hwnd, BM_SETSTATE, 0, 0L);

    Display();

    return TRUE;
}

/***
*void CCalc::CArith::Quit()
*Purpose:
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
STDMETHODIMP_(void)
CCalc::CArith::Quit()
{
    PostQuitMessage(0);
}


//---------------------------------------------------------------------
//                      The CCalc Class Factory
//---------------------------------------------------------------------


IClassFactory FAR*
CCalcCF::Create()
{
    return new FAR CCalcCF();
}


STDMETHODIMP
CCalcCF::QueryInterface(REFIID riid, void FAR* FAR* ppv)
{
    if(riid == IID_IUnknown || riid == IID_IClassFactory){
      AddRef();
      *ppv = this;
      return NOERROR;
    }
    return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG)
CCalcCF::AddRef()
{
    return ++m_refs;
}


STDMETHODIMP_(ULONG)
CCalcCF::Release()
{
    if(--m_refs == 0){
      delete this;
      return 0;
    }
    return m_refs;
}


STDMETHODIMP
CCalcCF::CreateInstance(
    IUnknown FAR* punkOuter,
    REFIID riid,
    void FAR* FAR* ppv)
{
extern CCalc FAR* g_pcalc;

    return g_pcalc->QueryInterface(riid, ppv);
}


STDMETHODIMP
CCalcCF::LockServer(BOOL fLock)
{
    return NOERROR;
}
