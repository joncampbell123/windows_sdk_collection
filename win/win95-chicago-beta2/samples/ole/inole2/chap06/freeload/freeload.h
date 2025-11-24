/*
 * FREELOAD.H
 * Freeloader Chapter 6
 *
 * Class overrides for the FreeLoader application
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _FREELOAD_H_
#define _FREELOAD_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>
#include <bookguid.h>

extern "C"
    {
    #include <commdlg.h>
    }

#include <classlib.h>
#include "resource.h"


//FREELOAD.CPP:  Frame object that creates a main window

class CFreeloaderFrame : public CFrame
    {
    private:
        BOOL             m_fInitialized;     //OleInitialize work?

    protected:
        //Overridable for creating a CClient for this frame
        virtual PCClient CreateCClient(void);
        virtual LRESULT  OnCommand(HWND, WPARAM, LPARAM);

    public:
        CFreeloaderFrame(HINSTANCE, HINSTANCE, LPSTR, int);
        virtual ~CFreeloaderFrame(void);

        virtual BOOL     FInit(PFRAMEINIT);
        virtual void     UpdateMenus(HMENU, UINT);
    };


typedef CFreeloaderFrame *PCFreeloaderFrame;





//CLIENT.CPP

//Override to create CFreeloaderDoc
class CFreeloaderClient : public CClient
    {
    protected:
        //Overridable for creating a new CDocument
        virtual PCDocument  CreateCDocument(void);

    public:
        CFreeloaderClient(HINSTANCE);
        virtual ~CFreeloaderClient(void);
    };


typedef CFreeloaderClient *PCFreeloaderClient;




//DOCUMENT.CPP

//Override file and clipboard operations as well as painting.
class CFreeloaderDoc : public CDocument
    {
    protected:
        LPSTORAGE       m_pIStorage;    //Root storage for this doc
        LPUNKNOWN       m_pIUnknown;    //Static visual object.
        DWORD           m_dwConn;       //From IOleCache::Cache
        CLSID           m_clsID;        //Class of doc contents

    protected:
        //Centralized cleanup
        void ReleaseObject(void);

        //To catch WM_PAINT
        virtual BOOL    FMessageHook(HWND, UINT, WPARAM, LPARAM
            , LRESULT *);

    public:
        CFreeloaderDoc(HINSTANCE, PCFrame);
        virtual ~CFreeloaderDoc(void);

        virtual UINT     ULoad(BOOL, LPTSTR);
        virtual UINT     USave(UINT, LPTSTR);

        virtual BOOL     FClip(HWND, BOOL);
        virtual HGLOBAL  RenderFormat(UINT);
        virtual BOOL     FQueryPaste(void);
        virtual BOOL     FPaste(HWND);

        virtual BOOL     SizeToGraphic(BOOL);
    };

typedef CFreeloaderDoc *PCFreeloaderDoc;


//For mapping HIMETRIC to pixels.
#define HIMETRIC_PER_INCH       2540

#endif //_FREELOAD_H_
