/*
 * IOLECONT.H
 *
 * Definitions of a template IOleItemContainer interface
 * implementation.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IOLECONT_H_
#define _IOLECONT_H_

class CImpIOleItemContainer;
typedef class CImpIOleItemContainer *PIMPIOLEITEMCONTAINER;

class CImpIOleItemContainer : public IOleItemContainer
    {
    protected:
        ULONG               m_cRef;
        LPVOID              m_pObj;
        LPUNKNOWN           m_punkOuter;

    public:
        CImpIOleItemContainer(LPVOID, LPUNKNOWN);
        ~CImpIOleItemContainer(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP ParseDisplayName(LPBC, LPTSTR, ULONG *
                         , LPMONIKER *);
        STDMETHODIMP EnumObjects(DWORD, LPENUMUNKNOWN *);
        STDMETHODIMP LockContainer(BOOL);
        STDMETHODIMP GetObject(LPTSTR, DWORD, LPBINDCTX, REFIID
                         , LPVOID *);
        STDMETHODIMP GetObjectStorage(LPTSTR, LPBINDCTX, REFIID
                         , LPVOID *);
        STDMETHODIMP IsRunning(LPTSTR);
    };

#endif  //_IOLECONT_H_
