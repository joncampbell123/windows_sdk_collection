/*
 * IENUMVAR.H
 *
 * Definitions of an IEnumVARIANT object.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IENUMVAR_H_
#define _IENUMVAR_H_


class CEnumVariant;
typedef class CEnumVariant *PCEnumVariant;

class CEnumVariant : public IEnumVARIANT
    {
    private:
        ULONG           m_cRef;     //Object reference count
        LPVARIANT       m_pUnkRef;  //IUnknown for ref counting
        ULONG           m_iCur;     //Current element
        ULONG           m_cunk;     //Number of variants in us
        LPVARIANT       m_prgunk;   //Source of variants

    public:
        CEnumVariant(LPUNKNOWN, ULONG, LPVARIANT);
        ~CEnumVariant(void);

        //IUnknown members that delegate to m_pUnkRef.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IEnumVARIANT members
        STDMETHODIMP Next(ULONG, LPVARIANT *, ULONG *);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumVARIANT **);
    };


#endif //_IENUMVAR_H_
