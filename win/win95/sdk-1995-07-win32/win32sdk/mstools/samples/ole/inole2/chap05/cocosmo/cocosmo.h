/*
 * COCOSMO.H
 * Component Cosmo Chapter 5
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
//CHAPTER5MOD
#include <ole2.h>
#include <ole2ver.h>
#include <bookguid.h>
#include <ipoly5.h>
//End CHAPTER5MOD

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

//Explicit CPolyline moved into Polyline DLL and hidden from us.
class CPolylineAdviseSink : public IPolylineAdviseSink5
    {
    private:
        LPVOID      m_pv;             //Customizable structure
        ULONG       m_cRef;

    public:
        CPolylineAdviseSink(LPVOID);
        ~CPolylineAdviseSink(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //Advise members.
        STDMETHODIMP_(void) OnPointChange(void);
        STDMETHODIMP_(void) OnSizeChange(void);
        STDMETHODIMP_(void) OnDataChange(void);
        STDMETHODIMP_(void) OnColorChange(void);
        STDMETHODIMP_(void) OnLineStyleChange(void);
    };

typedef CPolylineAdviseSink *PCPolylineAdviseSink;



//Constant ID for the window polyline that lives in a document
#define ID_POLYLINE         10


class CCosmoDoc : public CDocument
    {
    friend class CPolylineAdviseSink;

    protected:
        UINT                    m_uPrevSize;    //Last WM_SIZE wParam

        PPOLYLINE               m_pPL;          //Polyline window here
        PPOLYLINEADVISESINK     m_pPLAdv;       //Advises from Polyline

        //CHAPTER5MOD
        LPSTORAGE               m_pIStorage;        //Doc storage
        LPPERSISTSTORAGE        m_pIPersistStorage; //Polyline's
        //End CHAPTER5MOD

    protected:
        virtual BOOL    FMessageHook(HWND, UINT, WPARAM, LPARAM
            , LRESULT *);

    public:
        CCosmoDoc(HINSTANCE, PCFrame);
        virtual ~CCosmoDoc(void);

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
