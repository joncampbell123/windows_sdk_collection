/*
 * IENUMSTR.H
 *
 * Definitions of an IEnumString object.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IENUMSTR_H_
#define _IENUMSTR_H_


class CEnumString;
typedef class CEnumString *PCEnumString;

class CEnumString : public IEnumString
    {
    private:
        ULONG           m_cRef;     //Object reference count
        LPUNKNOWN       m_pUnkRef;  //IUnknown for ref counting
        ULONG           m_iCur;     //Current element
        ULONG           m_cstr;     //Number of strings in us
        LPTSTR          m_prgstr;   //Source of strings

    public:
        CEnumString(LPUNKNOWN, ULONG, LPTSTR);
        ~CEnumString(void);

        //IUnknown members that delegate to m_pUnkRef.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IEnumString members
        STDMETHODIMP Next(ULONG, LPTSTR *, ULONG *);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumString **);
    };


#endif //_IENUMSTR_H_
