/*** 
*dspcalc2.h
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  UNDONE
*
*
*Implementation Notes:
*
*****************************************************************************/

#include "clsid.h" 

/* 
 * UNDONE: Shouldn't have to do this.
 */
typedef BOOL boolean;

/* 
 * The .h file included below is created via MkTypLib
 */
#include "calctype.h"

#pragma warning(disable:4355)

#define DIM(X) (sizeof(X)/sizeof(X[0]))


class FAR CCalc : public IUnknown {
public:
    static CCalc FAR* Create();

    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppv);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    // Introduced "calculator" interface
    //
    // This nested class implementes core arithmetic functionality
    // (such as it is) *and* is the interface that will be exposed via
    // IDispatch for external programmability.
    //
    class FAR CArith : _Calculator {
    public:

      /* IUnknown methods */
      STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj);
      STDMETHOD_(ULONG, AddRef)(THIS);
      STDMETHOD_(ULONG, Release)(THIS);

      /* _Calculator methods */
      STDMETHOD_(void, put_Accum)(long l);
      STDMETHOD_(long, get_Accum)(void);
      STDMETHOD_(void, put_Opnd)(long l);
      STDMETHOD_(long, get_Opnd)(void);
      STDMETHOD_(void, put_Op)(short op);
      STDMETHOD_(short, get_Op)(void);
      STDMETHOD_(boolean, Eval)(void);
      STDMETHOD_(void, Clear)(void);
      STDMETHOD_(void, Display)(void);
      STDMETHOD_(void, Quit)(void);
      STDMETHOD_(BOOL, Button)(BSTR button);

      // the following method is internal, and not exposed for programmability
      BOOL ButtonPush(int button);

      CArith(CCalc FAR* pcalc){
        m_pcalc = pcalc;
        Clear();
      }

      enum states { STATE_LOPND, STATE_OP, STATE_ROPND, STATE_EVAL };

    private:
      CCalc FAR* m_pcalc;

      short	m_op;
      long	m_opnd;
      long	m_accum;
      enum states m_state;
    };
    friend CArith;
    CArith m_arith;


    HWND m_hwnd;

    CCalc() : m_arith(this)
    {
      m_refs = 1;
      m_hwnd = 0;
      m_punkStdDisp = NULL;
    }

private:
    ULONG m_refs;
    IUnknown FAR* m_punkStdDisp;
};


// the CCalc Class Factory
//
class FAR CCalcCF : public IClassFactory {
public:
    static IClassFactory FAR* Create();

    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppv);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    STDMETHOD(CreateInstance)(
      IUnknown FAR* punkOuter, REFIID riid, void FAR* FAR* ppv);
    STDMETHOD(LockServer)(BOOL fLock);

    CCalcCF() { m_refs = 1; }

private:
    ULONG m_refs;
};
