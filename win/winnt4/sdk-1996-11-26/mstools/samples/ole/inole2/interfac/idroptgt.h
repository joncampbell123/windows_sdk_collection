/*
 * IDROPTGT.H
 *
 * Definitions and function prototypes for a template
 * DropTarget object.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IDROPTGT_H_
#define _IDROPTGT_H_

class CDropTarget;
typedef class CDropTarget *PCDropTarget;

class CDropTarget : public IDropTarget
    {
    protected:
        ULONG           m_cRef;      //Interface reference count.
        LPVOID          m_pBack;     //Back pointer

    public:
        CDropTarget(LPVOID);
        ~CDropTarget(void);

        //IDropTarget interface members
        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL
            , LPDWORD);
        STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
        STDMETHODIMP DragLeave(void);
        STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
    };


#endif //_IDROPTGT_H_
