/*
 * OBJUSER.H
 * Koala Object User Chapter 4
 *
 * Definitions and structures.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _OBJUSER_H_
#define _OBJUSER_H_

#include <bookguid.h>


//Menu Resource ID and Commands
#define IDR_MENU                    1


#define IDM_OBJECTUSEDLL            100
#define IDM_OBJECTUSEEXE            101
#define IDM_OBJECTCREATECOGCO       102
#define IDM_OBJECTCREATECOCI        103
#define IDM_OBJECTRELEASE           104
#define IDM_OBJECTGETCLASSID        105
#define IDM_OBJECTEXIT              106



//OBJUSER.CPP
LRESULT APIENTRY ObjectUserWndProc(HWND, UINT, WPARAM, LPARAM);

class CAppVars
    {
    friend LRESULT APIENTRY ObjectUserWndProc(HWND, UINT, WPARAM
        , LPARAM);

    protected:
        HINSTANCE       m_hInst;            //WinMain parameters
        HINSTANCE       m_hInstPrev;
        UINT            m_nCmdShow;

        HWND            m_hWnd;             //Main window handle
        BOOL            m_fEXE;             //Menu selection

        LPPERSIST       m_pIPersist;        //IPersist interface
        BOOL            m_fInitialized;     //Did CoInitialize work?

    public:
        CAppVars(HINSTANCE, HINSTANCE, UINT);
        ~CAppVars(void);

        BOOL FInit(void);
        void Message(LPTSTR);
    };


typedef CAppVars *PAPPVARS;

#define CBWNDEXTRA              sizeof(PAPPVARS)
#define OBJUSERWL_STRUCTURE     0


#endif //_OBJUSER_H_
