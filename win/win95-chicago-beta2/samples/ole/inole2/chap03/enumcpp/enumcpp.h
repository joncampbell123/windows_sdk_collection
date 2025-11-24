/*
 * ENUMCPP.H
 * Enumerator in C++ Chapter 3
 *
 * Definitions, classes, and prototypes
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _ENUMCPP_H_
#define _ENUMCPP_H_


#include <ienum0.h>      //Found in shared INC directory.
#include <book1632.h>


//Menu Resource ID and Commands
#define IDR_MENU                    1

#define IDM_ENUMCREATE              100
#define IDM_ENUMRELEASE             101
#define IDM_ENUMRUNTHROUGH          102
#define IDM_ENUMEVERYTHIRD          103
#define IDM_ENUMRESET               104
#define IDM_ENUMEXIT                105


//ENUMCPP.CPP
LRESULT APIENTRY EnumWndProc(HWND, UINT, WPARAM, LPARAM);

class CAppVars
    {
    friend LRESULT APIENTRY EnumWndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        HINSTANCE       m_hInst;            //WinMain parameters
        HINSTANCE       m_hInstPrev;
        UINT            m_nCmdShow;

        HWND            m_hWnd;             //Main window handle
        PENUMRECT       m_pIEnumRect;       //Enumerator interface

    public:
        CAppVars(HINSTANCE, HINSTANCE, UINT);
        ~CAppVars(void);

        BOOL        FInit(void);
        void inline Message(LPTSTR);
    };


typedef CAppVars *PAPPVARS;

#define CBWNDEXTRA          sizeof(PAPPVARS)
#define ENUMWL_STRUCTURE    0


//IENUM.CPP

//Number of rects that objects with IEnumRECT support (for demo)
#define CRECTS      15

/*
 * A class definition, which OLE doesn't provide, then inherits from
 * whatever interfaces it supports.  Multiple inheritance works fine
 * in this scenario as well as single inheritance shown here.
 */
class CImpIEnumRECT : public IEnumRECT
    {
    private:
        DWORD           m_cRef;         //Reference count
        DWORD           m_iCur;         //Current enum position
        RECT            m_rgrc[CRECTS]; //RECTS we enumerate

    public:
        CImpIEnumRECT(void);
        ~CImpIEnumRECT(void);

        virtual DWORD AddRef(void);
        virtual DWORD Release(void);
        virtual BOOL  Next(DWORD, LPRECT, LPDWORD);
        virtual BOOL  Skip(DWORD);
        virtual void  Reset(void);
    };


typedef CImpIEnumRECT *PIMPIENUMRECT;


//Function that creates one of these objects
BOOL CreateRECTEnumerator(PENUMRECT *);


#endif //_ENUMCPP_H_
