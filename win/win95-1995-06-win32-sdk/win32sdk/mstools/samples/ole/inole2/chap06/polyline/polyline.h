/*
 * POLYLINE.H
 * Polyline Component Object Chapter 6
 *
 * Definitions and function prototypes
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
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
#include <classlib.h>
//CHAPTER6MOD
#include <ipoly6.h>
//End CHAPTER6MOD
#include "resource.h"


//Classname
#define SZCLASSPOLYLINE             TEXT("polyline")

//Stream Name that holds the data
#define SZSTREAM                    OLESTR("CONTENTS")


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

class CImpIPersistStorage;
typedef class CImpIPersistStorage *PIMPIPERSISTSTORAGE;

//CHAPTER6MOD
class CImpIDataObject;
typedef class CImpIDataObject *PIMPIDATAOBJECT;

#define CFORMATETCGET       3
#define CFORMATETCSET       1
//End CHAPTER6MOD


//POLYLINE.CPP
class CPolyline : public IUnknown
    {
    friend LRESULT APIENTRY PolylineWndProc(HWND, UINT, WPARAM
        , LPARAM);

    friend class CImpIPolyline;
    friend class CImpIPersistStorage;

    //CHAPTER6MOD
    friend class CImpIDataObject;
    //End CHAPTER6MOD

    protected:
        HWND            m_hWnd;
        HINSTANCE       m_hInst;

        ULONG           m_cRef;         //Object reference count
        LPUNKNOWN       m_pUnkOuter;    //Controlling Unknown
        PFNDESTROYED    m_pfnDestroy;   //Function called on closure
        BOOL            m_fDirty;       //Have we changed?
        POLYLINEDATA    m_pl;           //Our actual data

        PCStringTable   m_pST;          //Object strings
        UINT            m_cf;           //Object clipboard format
        CLSID           m_clsID;        //Current CLSID

        //We have to hold these for IPersistStorage::Save
        LPSTORAGE       m_pIStorage;
        LPSTREAM        m_pIStream;

        //Contained interfaces
        PIMPIPOLYLINE           m_pIPolyline;
        PIMPIPERSISTSTORAGE     m_pIPersistStorage;

        PPOLYLINEADVISESINK     m_pAdv;

        //CHAPTER6MOD
        PIMPIDATAOBJECT     m_pIDataObject;
        LPDATAADVISEHOLDER  m_pIDataAdviseHolder;

        //Arrays for IDataObject::EnumFormatEtc
        ULONG               m_cfeGet;
        FORMATETC           m_rgfeGet[CFORMATETCGET];

        ULONG               m_cfeSet;
        FORMATETC           m_rgfeSet[CFORMATETCSET];
        //End CHAPTER6MOD


    protected:
        void      PointScale(LPRECT, LPPOINTS, BOOL);
        void      Draw(HDC, BOOL, BOOL);
        void      RectConvertMappings(LPRECT, BOOL);

        //CHAPTER6MOD
        /*
         * These members pulled from IPolyline now serve as a
         * central store for this functionality to be used from
         * other interfaces like IPersistStorage and IDataObject.
         * Other interfaces later may also use them.
         */
        STDMETHODIMP DataSet(PPOLYLINEDATA, BOOL, BOOL);
        STDMETHODIMP DataGet(PPOLYLINEDATA);

        STDMETHODIMP RenderNative(HGLOBAL *);
        STDMETHODIMP RenderBitmap(HBITMAP *);
        STDMETHODIMP RenderMetafilePict(HGLOBAL *);
        //End CHAPTER6MOD


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

class CImpIPolyline : public IPolyline6
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

        //CHAPTER6MOD
        //Data* and Render* replaced with IDataObject
        //End CHAPTER6MOD

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


//CHAPTER6MOD
//IDATAOBJ.CPP
class CImpIDataObject : public IDataObject
    {
    private:
        ULONG               m_cRef;      //Interface reference count
        PCPolyline          m_pObj;      //Back pointer to object
        LPUNKNOWN           m_pUnkOuter; //Controlling unknown

    public:
        CImpIDataObject(PCPolyline, LPUNKNOWN);
        ~CImpIDataObject(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IDataObject members
        STDMETHODIMP GetData(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP GetDataHere(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP QueryGetData(LPFORMATETC);
        STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC,LPFORMATETC);
        STDMETHODIMP SetData(LPFORMATETC, LPSTGMEDIUM, BOOL);
        STDMETHODIMP EnumFormatEtc(DWORD, LPENUMFORMATETC *);
        STDMETHODIMP DAdvise(LPFORMATETC, DWORD, LPADVISESINK
            , DWORD *);
        STDMETHODIMP DUnadvise(DWORD);
        STDMETHODIMP EnumDAdvise(LPENUMSTATDATA *);
    };



//IENUMFE.CPP
class CEnumFormatEtc : public IEnumFORMATETC
    {
    private:
        ULONG           m_cRef;
        LPUNKNOWN       m_pUnkRef;
        ULONG           m_iCur;
        ULONG           m_cfe;
        LPFORMATETC     m_prgfe;

    public:
        CEnumFormatEtc(LPUNKNOWN, ULONG, LPFORMATETC);
        ~CEnumFormatEtc(void);

        //IUnknown members that delegate to m_pUnkRef.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IEnumFORMATETC members
        STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG *);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumFORMATETC **);
    };


typedef CEnumFormatEtc *PCEnumFormatEtc;

//End CHAPTER6MOD


#endif  //_POLYLINE_H_
