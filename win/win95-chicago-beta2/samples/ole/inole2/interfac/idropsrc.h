/*
 * IDROPSRC.H
 *
 * Definitions and function prototypes for a template
 * DropSrouce object.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IDROPSRC_H_
#define _IDROPSRC_H_

class CDropSource;
typedef class CDropSource *PCDropSource;

class CDropSource : public IDropSource
    {
    protected:
        ULONG           m_cRef;     //Interface reference count
        LPVOID          m_pBack;    //Back pointer

    public:
        CDropSource(LPVOID);
        ~CDropSource(void);

        //IDropSource interface members
        STDMETHODIMP QueryInterface(REFIID, LPVOID );
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP QueryContinueDrag(BOOL, DWORD);
        STDMETHODIMP GiveFeedback(DWORD);
    };


#endif //_IDROPSRC_H_
