/*
 * ENUMC.H
 * Enumerator in C Chapter 3
 *
 * Definitions, structures, and prototypes.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _ENUMC_H_
#define _ENUMC_H_


#include <ienum0.h>      //Found in shared include directory.
#include <book1632.h>


//Menu Resource ID and Commands
#define IDR_MENU                    1

#define IDM_ENUMCREATE              100
#define IDM_ENUMRELEASE             101
#define IDM_ENUMRUNTHROUGH          102
#define IDM_ENUMEVERYTHIRD          103
#define IDM_ENUMRESET               104
#define IDM_ENUMEXIT                105



//ENUMC.C
LRESULT APIENTRY EnumWndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct tagAPPVARS
    {
    HINSTANCE       m_hInst;            //WinMain parameters
    HINSTANCE       m_hInstPrev;
    UINT            m_nCmdShow;

    HWND            m_hWnd;             //Main window handle
    PENUMRECT       m_pIEnumRect;       //Enumerator interface
    } APPVARS, *PAPPVARS;


PAPPVARS  AppVarsConstructor(HINSTANCE, HINSTANCE, UINT);
void      AppVarsDestructor(PAPPVARS);
BOOL      AppVarsFInit(PAPPVARS);
void      AppVarsMsg(PAPPVARS, LPTSTR);


#define CBWNDEXTRA          sizeof(PAPPVARS)
#define ENUMWL_STRUCTURE    0


//Number of rect that objects with IEnumRECT support (for demo)
#define CRECTS      15


/*
 * In C we make a class by reusing the elements of IEnumRECT
 * thereby inheriting from it, albeit manually.
 */
typedef struct tagIMPIENUMRECT
    {
    IEnumRECTVtbl * lpVtbl;
    DWORD           m_cRef;         //Reference count
    DWORD           m_iCur;         //Current enum position
    RECT            m_rgrc[CRECTS]; //RECTS we enumerate
    } IMPIENUMRECT, *PIMPIENUMRECT, FAR * LPIMPIENUMRECT;


/*
 * In C we have to separately declare member functions with
 * globally unique names, so prefixing with the class name
 * should remove any conflicts.
 */

PIMPIENUMRECT   IMPIEnumRect_Constructor(void);
void            IMPIEnumRect_Destructor(PIMPIENUMRECT);

DWORD           IMPIEnumRect_AddRef(PENUMRECT);
DWORD           IMPIEnumRect_Release(PENUMRECT);
BOOL            IMPIEnumRect_Next(PENUMRECT, DWORD, LPRECT
                    , LPDWORD);
BOOL            IMPIEnumRect_Skip(PENUMRECT, DWORD);
void            IMPIEnumRect_Reset(PENUMRECT);


//Function that creates one of these objects
BOOL CreateRECTEnumerator(PENUMRECT *);


#endif //_ENUMC_H_
