/*
 * MALLOC.H
 * IMalloc Demonstration Chapter 4
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _MALLOC_H_
#define _MALLOC_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>
#include <book1632.h>
#include <dbgout.h>

//Menu Resource ID and Commands
#define IDR_MENU                        1


#define IDM_IMALLOCCOGETMALLOCTASKS     100
#define IDM_IMALLOCCOGETMALLOCSHARED    101
#define IDM_IMALLOCRELEASE              102
#define IDM_IMALLOCALLOC                103
#define IDM_IMALLOCFREE                 104
#define IDM_IMALLOCREALLOC              105
#define IDM_IMALLOCGETSIZE              106
#define IDM_IMALLOCDIDALLOC             107
#define IDM_IMALLOCHEAPMINIMIZE         108
#define IDM_IMALLOCEXIT                 109



//MALLOC.CPP
LRESULT APIENTRY MallocWndProc(HWND, UINT, WPARAM, LPARAM);

//How many allocations we'll perform
#define CALLOCS 10

//The types of allocators we can use
typedef enum
    {
    TASK_STANDARD, SHARED_STANDARD
    } ALLOCTYPE;



/*
 * Application-defined classes and types.
 */

class CAppVars
    {
    friend LRESULT APIENTRY MallocWndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        HINSTANCE       m_hInst;            //WinMain parameters
        HINSTANCE       m_hInstPrev;
        UINT            m_nCmdShow;

        HWND            m_hWnd;             //Main window handle
        LPMALLOC        m_pIMalloc;         //IMalloc interface
        BOOL            m_fInitialized;     //Did CoInitialize work?

        BOOL            m_fAllocated;       //We have allocations?
        ULONG           m_rgcb[CALLOCS];    //Sizes to allocate
        LPVOID          m_rgpv[CALLOCS];    //Allocated pointers

    public:
        CAppVars(HINSTANCE, HINSTANCE, UINT);
        ~CAppVars(void);
        BOOL FInit(void);

        void Message(LPTSTR);
        void SwitchAllocator(ALLOCTYPE);
        BOOL DoAllocations(BOOL);
        void FreeAllocations(BOOL);
    };


typedef CAppVars *PAPPVARS;


#define CBWNDEXTRA              sizeof(PAPPVARS)
#define MALLOCWL_STRUCTURE      0


//DMALLOC.CPP
class CDebugMalloc : public IMalloc
    {
    protected:
        ULONG           m_cRef;     //Interface reference count
        LPMALLOC        m_pIMalloc; //Standard IMalloc

    public:
        CDebugMalloc(void);
        ~CDebugMalloc(void);

        STDMETHODIMP QueryInterface(REFIID, void **);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(void *) Alloc(ULONG);
        STDMETHODIMP_(void *) Realloc(void *, ULONG);
        STDMETHODIMP_(void)   Free(void *);
        STDMETHODIMP_(ULONG)  GetSize(void *);
        STDMETHODIMP_(int)    DidAlloc(void *);
        STDMETHODIMP_(void)   HeapMinimize(void);
    };



#endif //_MALLOC_H_
