/*
 * IPERFILE.H
 *
 * Definitions of a template IPersistFile interface implementation.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IPERFILE_H_
#define _IPERFILE_H_

class CImpIPersistFile;
typedef class CImpIPersistFile *PIMPIPERSISTFILE;

class CImpIPersistFile : public IPersistFile
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIPersistFile(LPVOID, LPUNKNOWN);
        ~CImpIPersistFile(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetClassID(LPCLSID);

        STDMETHODIMP IsDirty(void);
        STDMETHODIMP Load(LPCSTR, DWORD);
        STDMETHODIMP Save(LPCSTR, BOOL);
        STDMETHODIMP SaveCompleted(LPCSTR);
        STDMETHODIMP GetCurFile(LPTSTR *);
    };


#endif  //_IPERFILE_H_
