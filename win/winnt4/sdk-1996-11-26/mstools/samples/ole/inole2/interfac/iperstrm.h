/*
 * IPERSTRM.H
 *
 * Definitions of a template IPersistStream interface
 * implementation.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IPERSTRM_H_
#define _IPERSTRM_H_

class CImpIPersistStream;
typedef class CImpIPersistStream *PIMPIPERSISTSTREAM;

class CImpIPersistStream : public IPersistStream
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIPersistStream(LPVOID, LPUNKNOWN);
        ~CImpIPersistStream(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetClassID(LPCLSID);

        STDMETHODIMP IsDirty(void);
        STDMETHODIMP Load(LPSTREAM);
        STDMETHODIMP Save(LPSTREAM, BOOL);
        STDMETHODIMP GetSizeMax(ULARGE_INTEGER *);
    };


#endif  //_IPERSTRM_H_
