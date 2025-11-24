/*
 * PATRON.H
 * Patron Chapter 7
 *
 * Single include file that pulls in everything needed for other
 * source files in the application.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _PATRON_H_
#define _PATRON_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>
//CHAPTER7MOD
#include <ole2ui.h>
//End CHAPTER7MOD
#include <bookguid.h>

extern "C"
    {
    #include <commdlg.h>
   #ifndef WIN32
    #include <print.h>
   #endif
    }

#include <classlib.h>
#include "resource.h"

//Get editor window information
#include "pages.h"


/*
 * UINT value such that adding one produces zero.  Portable to Win32.
 * This is used to represent a non-existent zero-based UINT value
 */
#define NOVALUE                     ((UINT)-1)


//PATRON.CPP:  Frame object that creates a main window

class CPatronFrame : public CFrame
    {
    private:
        BOOL            m_fInitialized;     //OleInitialize worked

        //CHAPTER7MOD
        //For locking DATATRAN.DLL class factory
        LPCLASSFACTORY  m_pIClassDataTran;
        //End CHAPTER7MOD

    protected:
        //Overridable for creating a CPatronClient
        virtual PCClient    CreateCClient(void);

        virtual BOOL        FRegisterAllClasses(void);
        virtual UINT        CreateGizmos(void);
        virtual LRESULT     OnCommand(HWND, WPARAM, LPARAM);

    public:
        CPatronFrame(HINSTANCE, HINSTANCE, LPSTR, int);
        virtual ~CPatronFrame(void);

        //Overrides
        virtual BOOL        FInit(PFRAMEINIT);

        virtual void        UpdateMenus(HMENU, UINT);
        virtual void        UpdateGizmos(void);

    };


typedef CPatronFrame *PCPatronFrame;





//CLIENT.CPP

/*
 * The only reason we have a derived class here is to override
 * CreateCDocument so we can create our own type as well as
 * overriding NewDocument to perform one other piece of work once
 * the document's been created.
 */

class CPatronClient : public CClient
    {
    protected:
        //Overridable for creating a new CDocument
        virtual PCDocument CreateCDocument(void);

    public:
        CPatronClient(HINSTANCE);
        virtual ~CPatronClient(void);
    };


typedef CPatronClient *PCPatronClient;




//DOCUMENT.CPP

//Constant ID for the pages window that lives in a document window
#define ID_PAGES            723


class CPatronDoc : public CDocument
    {
    protected:
        LONG            m_lVer;         //Loaded data version
        PCPages         m_pPG;          //Pages window in us

        LPSTORAGE       m_pIStorage;    //Root storage for document

        //CHAPTER7MOD
        BOOL            m_fPrintSetup;
        //End CHAPTER7MOD

    protected:
        virtual BOOL    FMessageHook(HWND, UINT, WPARAM, LPARAM
            , LRESULT *);

        //CHAPTER7MOD
        BOOL            FQueryPasteFromData(LPDATAOBJECT
                            , LPFORMATETC, PTENANTTYPE);
        BOOL            FPasteFromData(LPDATAOBJECT, LPFORMATETC
                            , TENANTTYPE, PPATRONOBJECT, DWORD);
        //End CHAPTER7MOD

    public:
        CPatronDoc(HINSTANCE, PCFrame);
        virtual ~CPatronDoc(void);

        virtual BOOL    FInit(PDOCUMENTINIT);
        virtual void    Clear(void);

        //CHAPTER7MOD
        virtual BOOL    FDirtyGet(void);
        virtual void    Delete(void);
        virtual BOOL    FQueryPrinterSetup(void);
        virtual BOOL    FQueryObjectSelected(HMENU);
        //End CHAPTER7MOD

        virtual UINT    ULoad(BOOL, LPTSTR);
        virtual UINT    USave(UINT, LPTSTR);

        virtual BOOL    Print(HWND);
        virtual UINT    PrinterSetup(HWND, BOOL);

        //CHAPTER7MOD
        virtual BOOL    FClip(HWND, BOOL);
        virtual BOOL    FQueryPaste(void);
        virtual BOOL    FPaste(HWND);
        virtual BOOL    FPasteSpecial(HWND);
        //End CHAPTER7MOD

        virtual UINT    NewPage(void);
        virtual UINT    DeletePage(void);
        virtual UINT    NextPage(void);
        virtual UINT    PreviousPage(void);
        virtual UINT    FirstPage(void);
        virtual UINT    LastPage(void);
    };

typedef CPatronDoc *PCPatronDoc;

//Hook for Print Dialog to hide Setup... button
UINT CALLBACK PrintDlgHook(HWND, UINT, WPARAM, LPARAM);


#endif //_PATRON_H_
