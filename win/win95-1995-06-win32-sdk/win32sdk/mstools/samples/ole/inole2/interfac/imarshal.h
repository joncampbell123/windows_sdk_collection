/*
 * IMARSHAL.H
 *
 * Definitions of a template IMarshal interface implementation.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IMARSHAL_H_
#define _IMARSHAL_H_

class CImpIMarshal;
typedef class CImpIMarshal *PIMPIMARSHAL;

class CImpIMarshal : public IMarshal
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIMarshal(LPVOID, LPUNKNOWN);
        ~CImpIMarshal(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetUnmarshalClass(REFIID, LPVOID, DWORD
            , LPVOID, DWORD, LPCLSID);
        STDMETHODIMP GetMarshalSizeMax(REFIID, LPVOID, DWORD
            , LPVOID, DWORD, LPDWORD);
        STDMETHODIMP MarshalInterface(LPSTREAM, REFIID, LPVOID
            , DWORD, LPVOID, DWORD);
        STDMETHODIMP UnmarshalInterface(LPSTREAM, REFIID, LPVOID *);
        STDMETHODIMP ReleaseMarshalData(LPSTREAM);
        STDMETHODIMP DisconnectObject(DWORD);
    };


#endif //_IMARSHAL_H_
