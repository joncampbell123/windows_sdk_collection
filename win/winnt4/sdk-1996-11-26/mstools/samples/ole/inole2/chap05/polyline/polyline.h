/*
 * POLYLINE.H
 * Polyline Component Object Chapter 5
 *
 * Definitions and function prototypes
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _POLYLINE_H_
#define _POLYLINE_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <bookguid.h>
//CHAPTER5MOD
#include <classlib.h>
#include <ipoly5.h>
#include "resource.h"
//End CHAPTER5MOD


//Classname
#define SZCLASSPOLYLINE             TEXT("polyline")

//CHAPTER5MOD
//Stream Name that holds the data
#define SZSTREAM                    OLESTR("CONTENTS")
//End CHAPTER5MOD

#define HIMETRIC_PER_INCH           2540

//Window extra bytes and offsets
#define CBPOLYLINEWNDEXTRA          (sizeof(LONG))
#define PLWL_STRUCTURE              0


//DLLPOLY.CPP
int PASCAL LibMain(HINSTANCE, WORD, WORD, LPSTR);

//This class factory object creates Polyline objects.

class CPolylineClassFactory : public IClassFactory
    {
    protected:
        ULONG           m_cRef;

    public:
        CPolylineClassFactory(void);
        ~CPolylineClassFactory(void);

        //IUnknown members
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP         CreateInstance(LPUNKNOWN, REFIID
                                 , PPVOID);
        STDMETHODIMP         LockServer(BOOL);
    };

typedef CPolylineClassFactory *PCPolylineClassFactory;



//POLYWIN.CPP
LRESULT APIENTRY PolylineWndProc(HWND, UINT, WPARAM, LPARAM);

//Type for an object-destroyed callback
typedef void (PASCAL *PFNDESTROYED)(void);

//Forward class references
class CImpIPolyline;
typedef class CImpIPolyline *PIMPIPOLYLINE;

//CHAPTER5MOD
class CImpIPersistStorage;
typedef class CImpIPersistStorage *PIMPIPERSISTSTORAGE;
//End CHAPTER5MOD


//POLYLINE.CPP
class CPolyline : public IUnknown
    {
    friend LRESULT APIENTRY PolylineWndProc(HWND, UINT, WPARAM
        , LPARAM);

    friend class CImpIPolyline;
    //CHAPTER5MOD
    friend class CImpIPersistStorage;
    //End CHAPTER5MOD

    protected:
        HWND            m_hWnd;
        HINSTANCE       m_hInst;

        ULONG           m_cRef;         //Object reference count
        LPUNKNOWN       m_pUnkOuter;    //Controlling Unknown
        PFNDESTROYED    m_pfnDestroy;   //Function called on closure
        BOOL            m_fDirty;       //Have we changed?
        POLYLINEDATA    m_pl;           //Our actual data

        //CHAPTER5MOD
        PCStringTable   m_pST;          //Object strings
        UINT            m_cf;           //Object clipboard format
        CLSID           m_clsID;        //Current CLSID

        //We have to hold these for IPersistStorage::Save
        LPSTORAGE       m_pIStorage;
        LPSTREAM        m_pIStream;
        //End CHAPTER5MOD

        //Contained interfaces
        PIMPIPOLYLINE           m_pIPolyline;
        //CHAPTER5MOD
        PIMPIPERSISTSTORAGE     m_pIPersistStorage;
        //End CHAPTER5MOD

        PPOLYLINEADVISESINK     m_pAdv;

    protected:
        void      PointScale(LPRECT, LPPOINTS, BOOL);
        void      Draw(HDC, BOOL, BOOL);
        void      RectConvertMappings(LPRECT, BOOL);

    public:
        CPolyline(LPUNKNOWN, PFNDESTROYED, HINSTANCE);
        ~CPolyline(void);

        BOOL      FInit(void);

        //Non-delegating object IUnknown
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
    };

typedef CPolyline *PCPolyline;




//Interface implementation contained in the Polyline.

class CImpIPolyline : public IPolyline5
    {
    protected:
        ULONG               m_cRef;      //Interface reference count
        PCPolyline          m_pObj;      //Back pointer to object
        LPUNKNOWN           m_pUnkOuter; //Controlling unknown

    public:
        CImpIPolyline(PCPolyline, LPUNKNOWN);
        ~CImpIPolyline(void);

        //IUnknown members.
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //CHAPTER5MOD
        //ReadFromFile and WriteToFile replaced with IPersistStorage.
        //End CHAPTER5MOD

        //Data transfer members:
        STDMETHODIMP DataSet(PPOLYLINEDATA, BOOL, BOOL);
        STDMETHODIMP DataGet(PPOLYLINEDATA);
        STDMETHODIMP DataSetMem(HGLOBAL, BOOL, BOOL, BOOL);
        STDMETHODIMP DataGetMem(HGLOBAL *);

        STDMETHODIMP RenderBitmap(HBITMAP *);
        STDMETHODIMP RenderMetafile(HMETAFILE *);
        STDMETHODIMP RenderMetafilePict(HGLOBAL *);

        //Manipulation members:
        STDMETHODIMP Init(HWND, LPRECT, DWORD, UINT);
        STDMETHODIMP New(void);
        STDMETHODIMP Undo(void);
        STDMETHODIMP Window(HWND *);

        STDMETHODIMP SetAdvise(PPOLYLINEADVISESINK);
        STDMETHODIMP GetAdvise(PPOLYLINEADVISESINK *);

        STDMETHODIMP RectGet(LPRECT);
        STDMETHODIMP SizeGet(LPRECT);
        STDMETHODIMP RectSet(LPRECT, BOOL);
        STDMETHODIMP SizeSet(LPRECT, BOOL);

        STDMETHODIMP ColorSet(UINT, COLORREF, COLORREF *);
        STDMETHODIMP ColorGet(UINT, COLORREF *);

        STDMETHODIMP LineStyleSet(UINT, UINT *);
        STDMETHODIMP LineStyleGet(UINT *);
    };


//CHAPTER5MOD

//Interface implementation contained in the Polyline.

class CImpIPersistStorage : public IPersistStorage
    {
    protected:
        ULONG               m_cRef;      //Interface reference count
        PCPolyline          m_pObj;      //Back pointer to object
        LPUNKNOWN           m_pUnkOuter; //Controlling unknown
        PSSTATE             m_psState;   //Storage state

    public:
        CImpIPersistStorage(PCPolyline, LPUNKNOWN);
        ~CImpIPersistStorage(void);

        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetClassID(LPCLSID);

        STDMETHODIMP IsDirty(void);
        STDMETHODIMP InitNew(LPSTORAGE);
        STDMETHODIMP Load(LPSTORAGE);
        STDMETHODIMP Save(LPSTORAGE, BOOL);
        STDMETHODIMP SaveCompleted(LPSTORAGE);
        STDMETHODIMP HandsOffStorage(void);
    };

//End CHAPTER5MOD

#endif  //_POLYLINE_H_
