/*
 * EKOALA.H
 * Koala Object EXE Chapter 4
 *
 * Definitions, classes, and prototypes for an application that
 * provides Koala objects to any other object user.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _EKOALA_H_
#define _EKOALA_H_


//Get the object definitions
#include "koala.h"


//EKOALA.CPP
LRESULT APIENTRY KoalaWndProc(HWND, UINT, WPARAM, LPARAM);

class CAppVars
    {
    friend LRESULT APIENTRY KoalaWndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        HINSTANCE       m_hInst;            //WinMain parameters
        HINSTANCE       m_hInstPrev;
        LPSTR           m_pszCmdLine;
        UINT            m_nCmdShow;

        HWND            m_hWnd;             //Main window handle

        BOOL            m_fInitialized;     //Did CoInitialize work?
        LPCLASSFACTORY  m_pIClassFactory;   //Our class factory
        DWORD           m_dwRegCO;          //Registration key

    public:
        CAppVars(HINSTANCE, HINSTANCE, LPSTR, UINT);
        ~CAppVars(void);
        BOOL FInit(void);
    };

typedef CAppVars *PAPPVARS;

#define CBWNDEXTRA              sizeof(PAPPVARS)
#define KOALAWL_STRUCTURE       0


void PASCAL ObjectDestroyed(void);


//This class factory object creates Koala objects.

class CKoalaClassFactory : public IClassFactory
    {
    protected:
        ULONG           m_cRef;

    public:
        CKoalaClassFactory(void);
        ~CKoalaClassFactory(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP         CreateInstance(LPUNKNOWN, REFIID
                                , PPVOID);
        STDMETHODIMP         LockServer(BOOL);
    };

typedef CKoalaClassFactory *PCKoalaClassFactory;

#endif //_EKOALA_H_
