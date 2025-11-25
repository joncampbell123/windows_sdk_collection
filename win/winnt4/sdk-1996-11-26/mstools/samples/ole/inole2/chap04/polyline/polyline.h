/*
 * POLYLINE.H
 * Polyline Component Object Chapter 4
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

//Pull in the interface and interface definitions
#include <ipoly4.h>


//Classname
#define SZCLASSPOLYLINE             TEXT("polyline")

#define HIMETRIC_PER_INCH           2540
#define CCHPATHMAX                  256

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


//POLYLINE.CPP
class CPolyline : public IUnknown
    {
    friend LRESULT APIENTRY PolylineWndProc(HWND, UINT, WPARAM
        , LPARAM);

    //Make any contained interfaces your friends
    friend class CImpIPolyline;

    protected:
        HWND            m_hWnd;
        HINSTANCE       m_hInst;

        ULONG           m_cRef;         //Object reference count
        LPUNKNOWN       m_pUnkOuter;    //Controlling Unknown
        PFNDESTROYED    m_pfnDestroy;   //Function called on closure
        BOOL            m_fDirty;       //Have we changed?
        POLYLINEDATA    m_pl;           //Our actual data

        //Contained interfaces
        PIMPIPOLYLINE   m_pIPolyline;

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

class CImpIPolyline : public IPolyline4
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

        //File-related members:
        STDMETHODIMP ReadFromFile(LPTSTR);
        STDMETHODIMP WriteToFile (LPTSTR);

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


#endif  //_POLYLINE_H_
