/*
 * LNKASSIS.H
 * Links Assistant Chapter 12
 *
 * Classes that implement the LinksAssistant object
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _LNKASSIS_H_
#define _LNKASSIS_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <oledlg.h>
#include <bookguid.h>       //CLSIDs


typedef void (PASCAL *PFNDESTROYED)(void);


class CLinks : public IOleUILinkContainer
    {
    protected:
        ULONG           m_cRef;
        PFNDESTROYED    m_pfnDestroy;

    protected:
        //Internal functions for implementing IOleUILinkContainer
        BOOL FValidateLinkSource(LPOLESTR, ULONG *, LPMONIKER *
            , LPCLSID);
        BOOL FCreateNewSourceMoniker(LPOLESTR, ULONG, LPMONIKER *);


    public:
        CLinks(PFNDESTROYED);
        ~CLinks(void);

        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        /*
         * The dwLink parameter to all of these functions is assumed
         * to be an IOleLink pointer for the object in question
         * (GetNextLink is not implelemented in us).  We can
         * QueryInterface IOleLink for anything else we need.
         */

        STDMETHODIMP_(DWORD) GetNextLink(DWORD);
        STDMETHODIMP         SetLinkUpdateOptions(DWORD, DWORD);
        STDMETHODIMP         GetLinkUpdateOptions(DWORD, LPDWORD);
        STDMETHODIMP         SetLinkSource(DWORD, LPTSTR, ULONG
                                 , ULONG *, BOOL);
        STDMETHODIMP         GetLinkSource(DWORD, LPTSTR *, ULONG *
                                 , LPTSTR *, LPTSTR *, BOOL *, BOOL *);
        STDMETHODIMP         OpenLinkSource(DWORD);
        STDMETHODIMP         UpdateLink(DWORD, BOOL, BOOL);
        STDMETHODIMP         CancelLink(DWORD);
    };

typedef CLinks *PCLinks;


#endif //_LNKASSIS_H_
