/*
 * COCOSMO.H
 * Component Cosmo Chapter 6
 *
 * Single include file that pulls in everything needed for other
 * source files in the Cosmo application.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _COCOSMO_H_
#define _COCOSMO_H_

#define INC_OLE2
#include <windows.h>
#include <memory.h>
#include <ole2.h>
#include <ole2ver.h>
#include <bookguid.h>
//CHAPTER6MOD
#include <ipoly6.h>
//End CHAPTER6MOD

extern "C"
    {
    #include <commdlg.h>
    }

#include <classlib.h>
#include "resource.h"


//COCOSMO.CPP:  Frame object that creates a main window

class CCosmoFrame : public CFrame
    {
    private:
        HBITMAP         m_hBmpLines[5];     //Menu item bitmaps
        UINT            m_uIDCurLine;       //Current line selection
        BOOL            m_fInitialized;     //CoInitialize work?

    protected:
        //Overridable for creating a CClient for this frame
        virtual PCClient  CreateCClient(void);

        virtual BOOL      FRegisterAllClasses(void);
        virtual BOOL      FPreShowInit(void);
        virtual UINT      CreateGizmos(void);

        virtual LRESULT   OnCommand(HWND, WPARAM, LPARAM);
        virtual void      OnDocumentDataChange(PCDocument);
        virtual void      OnDocumentActivate(PCDocument);

        //New for this class
        virtual void      CreateLineMenu(void);

    public:
        CCosmoFrame(HINSTANCE, HINSTANCE, LPSTR, int);
        virtual ~CCosmoFrame(void);

        //Overrides
        virtual BOOL      FInit(PFRAMEINIT);
        virtual void      UpdateMenus(HMENU, UINT);
        virtual void      UpdateGizmos(void);

        //New for this class
        virtual void      CheckLineSelection(UINT);
    };


typedef CCosmoFrame *PCCosmoFrame;





//CLIENT.CPP

/*
 * The only reason we have a derived class here is to override
 * CreateCDocument so we can create our own type as well as
 * overriding NewDocument to perform one other piece of work once
 * the document's been created.
 */

class CCosmoClient : public CClient
    {
    protected:
        //Overridable for creating a new CDocument
        virtual PCDocument CreateCDocument(void);

    public:
        CCosmoClient(HINSTANCE);
        virtual ~CCosmoClient(void);

        virtual PCDocument NewDocument(BOOL, PCDocumentAdviseSink);
    };


typedef CCosmoClient *PCCosmoClient;




//DOCUMENT.CPP

//CHAPTER6MOD
class CPolylineAdviseSink : public IPolylineAdviseSink6
    {
    private:
        LPVOID              m_pv;        //Customizable structure
        ULONG               m_cRef;
        LPUNKNOWN           m_pUnkOuter; //Unknown for delegation

    public:
        CPolylineAdviseSink(LPVOID, LPUNKNOWN);
        ~CPolylineAdviseSink(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //Advise members.
        STDMETHODIMP_(void) OnPointChange(void);
        STDMETHODIMP_(void) OnSizeChange(void);
        STDMETHODIMP_(void) OnColorChange(void);
        STDMETHODIMP_(void) OnLineStyleChange(void);
    };

typedef CPolylineAdviseSink *PCPolylineAdviseSink;


/*
 * The generic advisory interface.  This object controls it's own
 * lifetime and the document becomes a user of the object with
 * the last reference count.
 */

class CImpIAdviseSink : public IAdviseSink
    {
    protected:
        ULONG               m_cRef;
        LPVOID              m_pObj;
        LPUNKNOWN           m_pUnkOuter;

    public:
        CImpIAdviseSink(LPVOID, LPUNKNOWN);
        ~CImpIAdviseSink(void);

        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP_(void)  OnViewChange(DWORD, LONG);
        STDMETHODIMP_(void)  OnRename(LPMONIKER);
        STDMETHODIMP_(void)  OnSave(void);
        STDMETHODIMP_(void)  OnClose(void);
    };

typedef CImpIAdviseSink *PIMPIADVISESINK;

//End CHAPTER6MOD



//Constant ID for the window polyline that lives in a document window
#define ID_POLYLINE         10


//CHAPTER6MOD
//CoCosmoDoc now has IUnknown.
class CCosmoDoc : public CDocument, public IUnknown
    {
    friend class CPolylineAdviseSink;
    friend class CImpIAdviseSink;

    protected:
        UINT                    m_uPrevSize;

        PPOLYLINE               m_pPL;
        LPSTORAGE               m_pIStorage;
        LPPERSISTSTORAGE        m_pIPersistStorage;

        //CHAPTER6MOD
        PPOLYLINEADVISESINK     m_pPLAdv;
        PIMPIADVISESINK         m_pIAdviseSink;

        DWORD                   m_dwConn;   //Advisory connection
        ULONG                   m_cRef;     //Document ref count
        //EndCHAPTER6MOD

    protected:
        virtual BOOL    FMessageHook(HWND, UINT, WPARAM, LPARAM
            , LRESULT *);

    public:
        CCosmoDoc(HINSTANCE, PCFrame);
        virtual ~CCosmoDoc(void);

        //CHAPTER6MOD
        //Need a controlling unknown for our interfaces
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
        //End CHAPTER6MOD

        virtual BOOL     FInit(PDOCUMENTINIT);
        virtual void     Clear(void);

        virtual UINT     ULoad(BOOL, LPTSTR);
        virtual UINT     USave(UINT, LPTSTR);

        virtual void     Undo(void);
        virtual BOOL     FClip(HWND, BOOL);
        virtual HGLOBAL  RenderFormat(UINT);
        virtual BOOL     FQueryPaste(void);
        virtual BOOL     FPaste(HWND);

        virtual COLORREF ColorSet(UINT, COLORREF);
        virtual COLORREF ColorGet(UINT);

        virtual UINT     LineStyleSet(UINT);
        virtual UINT     LineStyleGet(void);
    };

typedef CCosmoDoc *PCCosmoDoc;


//These color indices wrap the polyline definitions
#define DOCCOLOR_BACKGROUND             POLYLINECOLOR_BACKGROUND
#define DOCCOLOR_LINE                   POLYLINECOLOR_LINE


#endif //_COCOSMO_H_
