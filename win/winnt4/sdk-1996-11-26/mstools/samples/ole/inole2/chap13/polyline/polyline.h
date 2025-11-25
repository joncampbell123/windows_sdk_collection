/*
 * POLYLINE.H
 * Polyline Component Object Chapter 13
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
#include <classlib.h>
#include <ipoly6.h>
#include "resource.h"
#include <oledlg.h>

#define CF_EMBEDSOURCE              TEXT("Embed Source")
//Classname
#define SZCLASSPOLYLINE             TEXT("polyline")

//Stream Name that holds the data
#define SZSTREAM                    OLESTR("CONTENTS")

#define SZPOLYFRAMETITLE            TEXT("Polyline Figure in %s")

//Magic number to add to aspects returned from IViewObject::Freeze
#define FREEZE_KEY_OFFSET           0x0723

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


#ifdef WIN32
#define PROP_POINTER    TEXT("Pointer")
#else
#define PROP_SELECTOR   "Selector"
#define PROP_OFFSET     "Offset"
#endif

BOOL APIENTRY PolyDlgProc(HWND, UINT, WPARAM, LPARAM);

//Type for an object-destroyed callback
typedef void (PASCAL *PFNDESTROYED)(void);

//Forward class references
class CImpIPolyline;
typedef class CImpIPolyline *PIMPIPOLYLINE;

class CImpIPersistStorage;
typedef class CImpIPersistStorage *PIMPIPERSISTSTORAGE;

class CImpIDataObject;
typedef class CImpIDataObject *PIMPIDATAOBJECT;

class CImpIOleObject;
typedef class CImpIOleObject *PIMPIOLEOBJECT;

class CImpIViewObject;
typedef class CImpIViewObject *PIMPIVIEWOBJECT;

class CImpIRunnableObject;
typedef class CImpIRunnableObject *PIMPIRUNNABLEOBJECT;

//CHAPTER13MOD
class CImpIExternalConnection;
typedef class CImpIExternalConnection *PIMPIEXTERNALCONNECTION;
//End CHAPTER13MOD


//POLYLINE.CPP
class CPolyline : public IUnknown
    {
    friend LRESULT APIENTRY PolylineWndProc(HWND, UINT, WPARAM
        , LPARAM);
    friend BOOL APIENTRY PolyDlgProc(HWND, UINT, WPARAM, LPARAM);

    friend class CImpIPolyline;
    friend class CImpIPersistStorage;
    friend class CImpIDataObject;
    friend class CImpIOleObject;
    friend class CImpIViewObject;
    friend class CImpIRunnableObject;

    //CHAPTER13MOD
    friend class CImpIExternalConnection;
    //End CHAPTER13MOD

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

        PIMPIDATAOBJECT     m_pIDataObject;
        LPDATAADVISEHOLDER  m_pIDataAdviseHolder;

        //These are default handler interfaces we use
        LPUNKNOWN           m_pDefIUnknown;
        LPVIEWOBJECT2       m_pDefIViewObject;
        LPPERSISTSTORAGE    m_pDefIPersistStorage;
        LPDATAOBJECT        m_pDefIDataObject;

        //Implemented and used interfaces
        PIMPIOLEOBJECT      m_pIOleObject;          //Implemented
        LPOLEADVISEHOLDER   m_pIOleAdviseHolder;    //Used

        LPOLECLIENTSITE     m_pIOleClientSite;      //Used

        PIMPIVIEWOBJECT     m_pIViewObject;         //Implemented
        LPADVISESINK        m_pIAdviseSink;         //Used
        DWORD               m_dwFrozenAspects;      //Freeze
        DWORD               m_dwAdviseAspects;      //SetAdvise
        DWORD               m_dwAdviseFlags;        //SetAdvise

        POLYLINEDATA        m_plContent;            //For freezing
        POLYLINEDATA        m_plThumbnail;          //For freezing

        PIMPIRUNNABLEOBJECT m_pIRunnableObject;     //Implemented
        HWND                m_hDlg;                 //Editing window

        //CHAPTER13MOD
        PIMPIEXTERNALCONNECTION m_pIExternalConnection; //Implemented
        BOOL                    m_fLockContainer;
        DWORD                   m_dwRegROT;
        //End CHAPTER13MOD

    protected:
        void      PointScale(LPRECT, LPPOINTS, BOOL);
        void      Draw(HDC, BOOL, BOOL, LPRECT, PPOLYLINEDATA);
        HINSTANCE Instance(void);
        LPTSTR    String(UINT);
        void      SendAdvise(UINT);
        void      RectConvertMappings(LPRECT, BOOL);

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


//Codes for CPolyline::SendAdvise
//......Code.....................Method called in CPolyline::SendAdvise
#define OBJECTCODE_SAVED       0 //IOleAdviseHolder::SendOnSave
#define OBJECTCODE_CLOSED      1 //IOleAdviseHolder::SendOnClose
#define OBJECTCODE_RENAMED     2 //IOleAdviseHolder::SendOnRename
#define OBJECTCODE_SAVEOBJECT  3 //IOleClientSite::SaveObject
#define OBJECTCODE_DATACHANGED 4 //IDataAdviseHolder::SendOnDataChange
#define OBJECTCODE_SHOWWINDOW  5 //IOleClientSite::OnShowWindow(TRUE)
#define OBJECTCODE_HIDEWINDOW  6 //IOleClientSite::OnShowWindow(FALSE)
#define OBJECTCODE_SHOWOBJECT  7 //IOleClientSite::ShowObject



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



class CImpIOleObject : public IOleObject
    {
    private:
        ULONG           m_cRef;
        PCPolyline      m_pObj;
        LPUNKNOWN       m_pUnkOuter;

    public:
        CImpIOleObject(PCPolyline, LPUNKNOWN);
        ~CImpIOleObject(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IOleObject members
        STDMETHODIMP SetClientSite(LPOLECLIENTSITE);
        STDMETHODIMP GetClientSite(LPOLECLIENTSITE *);
        STDMETHODIMP SetHostNames(LPCOLESTR, LPCOLESTR);
        STDMETHODIMP Close(DWORD);
        STDMETHODIMP SetMoniker(DWORD, LPMONIKER);
        STDMETHODIMP GetMoniker(DWORD, DWORD, LPMONIKER *);
        STDMETHODIMP InitFromData(LPDATAOBJECT, BOOL, DWORD);
        STDMETHODIMP GetClipboardData(DWORD, LPDATAOBJECT *);
        STDMETHODIMP DoVerb(LONG, LPMSG, LPOLECLIENTSITE, LONG
                         , HWND, LPCRECT);
        STDMETHODIMP EnumVerbs(LPENUMOLEVERB *);
        STDMETHODIMP Update(void);
        STDMETHODIMP IsUpToDate(void);
        STDMETHODIMP GetUserClassID(CLSID *);
        STDMETHODIMP GetUserType(DWORD, LPOLESTR *);
        STDMETHODIMP SetExtent(DWORD, LPSIZEL);
        STDMETHODIMP GetExtent(DWORD, LPSIZEL);
        STDMETHODIMP Advise(LPADVISESINK, DWORD *);
        STDMETHODIMP Unadvise(DWORD);
        STDMETHODIMP EnumAdvise(LPENUMSTATDATA *);
        STDMETHODIMP GetMiscStatus(DWORD, DWORD *);
        STDMETHODIMP SetColorScheme(LPLOGPALETTE);
    };


//IVIEWOBJ.CPP
class CImpIViewObject : public IViewObject2
    {
    private:
        ULONG           m_cRef;
        PCPolyline      m_pObj;
        LPUNKNOWN       m_pUnkOuter;

    public:
        CImpIViewObject(PCPolyline, LPUNKNOWN);
        ~CImpIViewObject(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IViewObject members
        STDMETHODIMP Draw(DWORD, LONG, LPVOID
            , DVTARGETDEVICE *, HDC, HDC, LPCRECTL
            , LPCRECTL, BOOL (CALLBACK *)(DWORD), DWORD);
        STDMETHODIMP GetColorSet(DWORD, LONG, LPVOID
            , DVTARGETDEVICE *, HDC, LPLOGPALETTE *);
        STDMETHODIMP Freeze(DWORD, LONG, LPVOID, LPDWORD);
        STDMETHODIMP Unfreeze(DWORD);
        STDMETHODIMP SetAdvise(DWORD, DWORD, LPADVISESINK);
        STDMETHODIMP GetAdvise(LPDWORD, LPDWORD, LPADVISESINK *);
        STDMETHODIMP GetExtent(DWORD, LONG, DVTARGETDEVICE *
            , LPSIZEL);
    };


class CImpIRunnableObject : public IRunnableObject
    {
    protected:
        ULONG           m_cRef;
        PCPolyline      m_pObj;
        LPUNKNOWN       m_pUnkOuter;
        DWORD           m_dwRegROT;

    public:
        CImpIRunnableObject(PCPolyline, LPUNKNOWN);
        ~CImpIRunnableObject(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetRunningClass(LPCLSID);
        STDMETHODIMP Run(LPBINDCTX);
        STDMETHODIMP_(BOOL) IsRunning(void);
        STDMETHODIMP LockRunning(BOOL, BOOL);
        STDMETHODIMP SetContainedObject(BOOL);
    };


//CHAPTER13MOD
class CImpIExternalConnection : public IExternalConnection
    {
    protected:
        ULONG           m_cRef;
        PCPolyline      m_pObj;
        LPUNKNOWN       m_pUnkOuter;
        DWORD           m_cLockStrong;

    public:
        CImpIExternalConnection(PCPolyline, LPUNKNOWN);
        ~CImpIExternalConnection(void);

        STDMETHODIMP QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(DWORD) AddConnection(DWORD, DWORD);
        STDMETHODIMP_(DWORD) ReleaseConnection(DWORD, DWORD, BOOL);
    };
//End CHAPTER13MOD


#endif  //_POLYLINE_H_
