/*
 * IIPOBJ.H
 *
 * Definitions of a template IOleInPlaceObject interface
 * implementation.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IIPOBJ_H_
#define _IIPOBJ_H_

class CImpIOleInPlaceObject;
typedef class CImpIOleInPlaceObject *PIMPIOLEINPLACEOBJECT;

class CImpIOleInPlaceObject : public IOleInPlaceObject
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIOleInPlaceObject(LPVOID, LPUNKNOWN);
        ~CImpIOleInPlaceObject(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetWindow(HWND *);
        STDMETHODIMP ContextSensitiveHelp(BOOL);
        STDMETHODIMP InPlaceDeactivate(void);
        STDMETHODIMP UIDeactivate(void);
        STDMETHODIMP SetObjectRects(LPCRECT, LPCRECT);
        STDMETHODIMP ReactivateAndUndo(void);
    };


#endif //_IIPOBJ_H_
