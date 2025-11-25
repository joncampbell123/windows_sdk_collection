/*
 * IMSGFILT.H
 *
 * Definitions of a template IMessageFilter interface
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


#ifndef _IMSGFILT_H_
#define _IMSGFILT_H_


class CImpIMessageFilter;
typedef class CImpIMessageFilter *PIMPIMESSAGEFILTER;

class CImpIMessageFilter : public IMessageFilter
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIMessageFilter(LPVOID, LPUNKNOWN);
        ~CImpIMessageFilter(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(DWORD) HandleInComingCall(DWORD, HTASK
            , DWORD, LPINTERFACEINFO);
        STDMETHODIMP_(DWORD) RetryRejectedCall(HTASK, DWORD, DWORD);
        STDMETHODIMP_(DWORD) MessagePending(HTASK, DWORD, DWORD);
    };


#endif //_IMSGFILT_H_
