///////////////////////////////////////////////////////////////////////////////
//
//  File Name 
//      CMDEXT.H
//
//  Description
//      Interface Methods are declared in this file for cmdext.cpp.
//
//  Author
//      Gary Peluso
//
//  Revision: 1.00
//
// Written for Microsoft Windows Developer Support
// Copyright (c) 1992-1995 Microsoft Corporation. All rights reserved.
//
#ifndef _CMDEXT_H_
#define _CMDEXT_H_

#include <WINDOWS.H> 
#include <COMMCTRL.H>
#include <MAPIX.H>
#include <MAPIUTIL.H>
#include <MAPIFORM.H>
#include <INITGUID.H>
#include <MAPIGUID.H>
#include <EXCHEXT.H>

#include "RESOURCE.H"

///////////////////////////////////////////////////////////////////////////////
//    prototypes
void GetFolderStats(LPMAPIFOLDER lpFolder, ULONG FAR * ulSubFolders, 
                    ULONG FAR * ulReadMsgs, ULONG FAR * ulUnReadMsgs);

extern "C"
{
    LPEXCHEXT CALLBACK ExchEntryPoint(void);
}

class MyExchExt : public IExchExt
{
 
 public:
    MyExchExt () { m_cRef = 0; m_context = 0;};
    STDMETHODIMP QueryInterface
                    (REFIID                     riid,
                     LPVOID *                   ppvObj);
    inline STDMETHODIMP_(ULONG) AddRef
                    () { ++m_cRef; return m_cRef; };
    inline STDMETHODIMP_(ULONG) Release
                    () { ULONG ulCount = --m_cRef;
                         if (!ulCount) { delete this; }
                         return ulCount;};
    STDMETHODIMP Install (LPEXCHEXTCALLBACK pmecb, 
                        ULONG mecontext, ULONG ulFlags);
    inline UINT WINAPI GetLastExchContext() { return m_context; };

 private:
    ULONG m_cRef;
    UINT  m_context;
};

class MyExchExtCommands : public IExchExtCommands
{
 public:
    MyExchExtCommands () { m_cRef = 0; };
    STDMETHODIMP QueryInterface
                    (REFIID                     riid,
                     LPVOID *                   ppvObj);
    inline STDMETHODIMP_(ULONG) AddRef
                    () { ++m_cRef; return m_cRef; };
    inline STDMETHODIMP_(ULONG) Release
                    () { ULONG ulCount = --m_cRef;
                         if (!ulCount) { delete this; }
                         return ulCount;};
    STDMETHODIMP InstallCommands(LPEXCHEXTCALLBACK pmecb, 
                                HWND hwnd, HMENU hmenu,
                                UINT FAR * cmdidBase, LPTBENTRY lptbeArray,
                                UINT ctbe, ULONG ulFlags);                                                                              
    STDMETHODIMP DoCommand(LPEXCHEXTCALLBACK pmecb, UINT mni);
    STDMETHODIMP_(VOID) InitMenu(LPEXCHEXTCALLBACK pmecb);
    STDMETHODIMP Help(LPEXCHEXTCALLBACK pmecb, UINT mni);
    STDMETHODIMP QueryHelpText(UINT mni, ULONG ulFlags, LPTSTR sz, UINT cch);
    STDMETHODIMP QueryButtonInfo(ULONG tbid, UINT itbb, LPTBBUTTON ptbb,
                                LPTSTR lpsz, UINT cch, ULONG ulFlags);
    STDMETHODIMP ResetToolbar(ULONG tbid, ULONG ulFlags);

    inline UINT GetCommandID
                () { return m_cmdidMain; };
  
 BOOL  m_bEnabled;     // true if our command is enabled

 private:
    ULONG m_cRef;

    UINT  m_cmdidMain;     // cmdid for main window menu extension command
    UINT  m_cmdidFind;     // cmdid for find window menu extension command

    UINT  m_itbb;          // toolbar
    UINT  m_itbm;

    HWND  m_hWndMain;      // Main Window Handle for the Viewer Context
    HWND  m_hWndFind;      // Find Window Handle

};


class MyExchExtUserEvents : public IExchExtUserEvents
{
 public:
    MyExchExtUserEvents() { m_cRef = 1; };
    STDMETHODIMP QueryInterface
                    (REFIID                     riid,
                     LPVOID *                   ppvObj);
    inline STDMETHODIMP_(ULONG) AddRef
                    () { ++m_cRef; return m_cRef; };
    inline STDMETHODIMP_(ULONG) Release
                    () { ULONG ulCount = --m_cRef;
                         if (!ulCount) { delete this; }
                         return ulCount;};

 STDMETHODIMP_(VOID) OnSelectionChange(LPEXCHEXTCALLBACK pmecb);
 STDMETHODIMP_(VOID) OnObjectChange(LPEXCHEXTCALLBACK pmecb);

 private:
    ULONG m_cRef;
    

};

#endif // _CMDEXT_H_
