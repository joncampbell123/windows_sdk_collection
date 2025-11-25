/*
 * COSMO.H
 * Cosmo Chapter 10
 *
 * Single include file that pulls in everything needed for other
 * source files in the Cosmo application.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _COSMO_H_
#define _COSMO_H_

#define INC_OLE2
#include <windows.h>
#include <memory.h>
#include <ole2.h>
#include <ole2ver.h>
//CHAPTER10MOD
#include <olestd.h>
//End CHAPTER10MOD
#include <bookguid.h>

extern "C"
    {
    #include <commdlg.h>
    }

#include <classlib.h>

#include "resource.h"


//Get the editor window information.
#include "polyline.h"



//COSMO.CPP:  Frame object that creates a main window

class CCosmoFrame : public CFrame
    {
    //CHAPTER10MOD
    friend class CFigureClassFactory;
    friend class CFigure;   //For UI purposes.
    //End CHAPTER10MOD

    private:
        HBITMAP         m_hBmpLines[5];     //Menu item bitmaps
        UINT            m_uIDCurLine;       //Current line selection
        BOOL            m_fInitialized;     //Did OleInitalize work?
        BOOL            m_fOleStdInit;      //Did OleStdInitialize work?
        LPCLASSFACTORY  m_pIClassDataTran;  //For locking

        //CHAPTER10MOD
        BOOL            m_fEmbedding;       //-Embedding found?
        DWORD           m_dwRegCO;          //Registration key
        LPCLASSFACTORY  m_pIClassFactory;
        //End CHAPTER10MOD

    protected:
        //Overridable for creating a CClient for this frame
        virtual PCClient  CreateCClient(void);

        virtual BOOL      FRegisterAllClasses(void);
        virtual BOOL      FPreShowInit(void);
        virtual UINT      CreateGizmos(void);

        //CHAPTER10MOD
        virtual void      ParseCommandLine(void);
        //End CHAPTER10MOD

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

        //CHAPTER10MOD
        virtual void      UpdateEmbeddingUI(BOOL, PCDocument
                              , LPCTSTR, LPCTSTR);
        //End CHAPTER10MOD
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

//Constant ID for the window polyline that lives in a document
#define ID_POLYLINE         10


class CCosmoDoc : public CDocument
    {
    friend class CPolylineAdviseSink;

    //These need access to FQueryPasteFromData, FPasteFromData
    friend class CDropTarget;
    friend class CDropSource;

    //CHAPTER10MOD
    friend class CFigureClassFactory;
    friend class CFigure;
    //End CHAPTER10MOD

    protected:
        UINT                    m_uPrevSize;    //Last WM_SIZE wParam
        LONG                    m_lVer;         //Loaded Polyline ver

        PCPolyline              m_pPL;          //Polyline window here
        PCPolylineAdviseSink    m_pPLAdv;       //Advises from Polyline

        class CDropTarget      *m_pDropTarget;  //Registered target
        BOOL                    m_fDragSource;  //Source==target?

        //CHAPTER10MOD
        UINT                    m_cfEmbedSource;
        UINT                    m_cfObjectDescriptor;

        class CFigure          *m_pFigure;      //The object in us.
        //End CHAPTER10MOD

    protected:
        virtual BOOL    FMessageHook(HWND, UINT, WPARAM, LPARAM
            , LRESULT *);

        void            DropSelectTargetWindow(void);

    public:
        CCosmoDoc(HINSTANCE, PCFrame);
        virtual ~CCosmoDoc(void);

        virtual BOOL     FInit(PDOCUMENTINIT);

        virtual void     Clear(void);

        //CHAPTER10MOD
        virtual BOOL     FDirtySet(BOOL);
        virtual BOOL     FDirtyGet(void);
        //End CHAPTER10MOD

        virtual UINT     ULoad(BOOL, LPTSTR);
        virtual UINT     USave(UINT, LPTSTR);

        virtual void     Undo(void);
        virtual BOOL     FClip(HWND, BOOL);
        virtual HGLOBAL  RenderFormat(UINT);
        //CHAPTER10MOD
        virtual BOOL     FRenderMedium(UINT, LPSTGMEDIUM);
        //End CHAPTER10MOD
        virtual BOOL     FQueryPaste(void);
        virtual BOOL     FPaste(HWND);

        //CHAPTER10MOD
        //These were protected.  Now for IOleObject, should be public.
        virtual BOOL     FQueryPasteFromData(LPDATAOBJECT);
        virtual BOOL     FPasteFromData(LPDATAOBJECT);
        LPDATAOBJECT     TransferObjectCreate(BOOL);
        //End CHAPTER10MOD

        virtual COLORREF ColorSet(UINT, COLORREF);
        virtual COLORREF ColorGet(UINT);

        virtual UINT     LineStyleSet(UINT);
        virtual UINT     LineStyleGet(void);
    };

typedef CCosmoDoc *PCCosmoDoc;


//These color indices wrap the polyline definitions
#define DOCCOLOR_BACKGROUND             POLYLINECOLOR_BACKGROUND
#define DOCCOLOR_LINE                   POLYLINECOLOR_LINE


//Drag-drop interfaces we need in the document
class CDropTarget : public IDropTarget
    {
    protected:
        ULONG               m_cRef;
        PCCosmoDoc          m_pDoc;

        LPDATAOBJECT        m_pIDataObject;     //From DragEnter

    public:
        CDropTarget(PCCosmoDoc);
        ~CDropTarget(void);

        //IDropTarget interface members
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
        STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
        STDMETHODIMP DragLeave(void);
        STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
    };


typedef CDropTarget *PCDropTarget;


class CDropSource : public IDropSource
    {
    protected:
        ULONG               m_cRef;
        PCCosmoDoc          m_pDoc;

    public:
        CDropSource(PCCosmoDoc);
        ~CDropSource(void);

        //IDropSource interface members
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP QueryContinueDrag(BOOL, DWORD);
        STDMETHODIMP GiveFeedback(DWORD);
    };


typedef CDropSource *PCDropSource;

//CHAPTER10MOD
//Include classes necessary to become an OLE 2.0 server.
#include "cosmole.h"
//End CHAPTER10MOD



#endif //_COSMO_H_
