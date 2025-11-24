/*** 
*dispcalc.h
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

#ifndef CLASS
# ifdef __TURBOC__
#  define CLASS class huge
# else
#  define CLASS class FAR
# endif
#endif

#pragma warning(disable:4355)

#define DIM(X) (sizeof(X)/sizeof(X[0]))


enum operators {
    OP_NONE = 0,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIV
};


CLASS CCalc : public IUnknown {
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
    CLASS CArith {
    public:

      STDMETHOD_(void, put_Accum)(long l);
      STDMETHOD_(long, get_Accum)(void);
      STDMETHOD_(void, put_Opnd)(long l);
      STDMETHOD_(long, get_Opnd)(void);
      STDMETHOD_(void, put_Op)(short op);
      STDMETHOD_(short, get_Op)(void);
      STDMETHOD_(BOOL, Eval)(void);
      STDMETHOD_(void, Clear)(void);
      STDMETHOD_(void, Display)(void);
      STDMETHOD_(void, Quit)(void);
      STDMETHOD_(BOOL, Button)(BSTR button);

      // the following method is internal, and not exposed for programmability
      BOOL ButtonPush(int button);

      CArith(CCalc FAR* pcalc){
	m_pcalc	= pcalc;
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


// the following enum defines method indices used by the
// default IDispatch implementation - DispInvoke().
//
// Note: these must match the order of the preceeding declarations
//
enum IMETH_CARITH {
    IMETH_PUTACCUM = 0,
    IMETH_GETACCUM,
    IMETH_PUTOPERAND,
    IMETH_GETOPERAND,
    IMETH_PUTOPERATOR,
    IMETH_GETOPERATOR,
    IMETH_EVAL,
    IMETH_CLEAR,
    IMETH_DISPLAY,
    IMETH_QUIT,
    IMETH_BUTTON,

    // Define the "property" indices. these are defined to be
    // the first index in a set/get property method pair. These
    // definitions are used to build the METHODDATA that drives
    // our implementation of IDispatch. see cdisp.cpp.
    //
    IMETH_ACCUM    = IMETH_PUTACCUM,
    IMETH_OPERAND  = IMETH_PUTOPERAND,
    IMETH_OPERATOR = IMETH_PUTOPERATOR
};

// the following enum defines the IDs used by IDispatch
//
// Note: these values do *not* depend on order of declaration,
// but are sensitive to the kind of the method - ie, if a get/set
// method pair implements a property, then they need to share
// an ID.
//
// Note: by assigning "accum" the ID 'DISPID_VALUE', we are
// choosing to expose it as the default "value" property.
//
enum IDMEMBER_CARITH {
    IDMEMBER_ACCUM = DISPID_VALUE,	// the default property
    IDMEMBER_OPERAND,
    IDMEMBER_OPERATOR,
    IDMEMBER_EVAL,
    IDMEMBER_CLEAR,
    IDMEMBER_DISPLAY,
    IDMEMBER_QUIT,
    IDMEMBER_BUTTON
};


// the CCalc Class Factory
//
CLASS CCalcCF : public IClassFactory {
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
