/*
 * DATAOBJ.H
 * Data Transfer Object Chapter 7
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _DATAOBJ_H_
#define _DATAOBJ_H_


#define INC_OLE2
#include <windows.h>
#include <ole2.h>
#include <bookguid.h>

//Type for an object-destroyed callback
typedef void (PASCAL *PFNDESTROYED)(void);


/*
 * Structure to describe a specific rendering that our user wants
 * us to maintain.
 */

typedef struct tagRENDERING
    {
    FORMATETC       fe;             //The format
    STGMEDIUM       stm;            //The actual data
    LPUNKNOWN       pUnkOrg;        //The real owner
    } RENDERING, *PRENDERING;



/*
 * The DataObject object is implemented in its own class with its
 * own IUnknown to support aggregation.  It contains one
 * CImpIDataObject object that we use to implement the externally
 * exposed interfaces.
 */

//DATAOBJ.CPP

class CImpIDataObject;
typedef class CImpIDataObject *PIMPIDATAOBJECT;

class CDataObject : public IUnknown
    {
    friend class CImpIDataObject;

    protected:
        ULONG               m_cRef;
        LPUNKNOWN           m_pUnkOuter;
        PFNDESTROYED        m_pfnDestroy;

        HWND                m_hList;        //Listbox of RENDERINGs

        //Contained interface implemetation
        PIMPIDATAOBJECT     m_pIDataObject;

    public:
        CDataObject(LPUNKNOWN, PFNDESTROYED);
        ~CDataObject(void);

        BOOL FInit(void);
        void Purge(void);

        //Non-delegating object IUnknown
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
    };

typedef CDataObject *PCDataObject;



/*
 * Interface implementations for the CDataObject object.
 */

class CImpIDataObject : public IDataObject
    {
    private:
        ULONG           m_cRef;
        PCDataObject    m_pObj;
        LPUNKNOWN       m_pUnkOuter;

    public:
        CImpIDataObject(PCDataObject, LPUNKNOWN);
        ~CImpIDataObject(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IDataObject members
        STDMETHODIMP GetData(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP GetDataHere(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP QueryGetData(LPFORMATETC);
        STDMETHODIMP GetCanonicalFormatEtc(LPFORMATETC, LPFORMATETC);
        STDMETHODIMP SetData(LPFORMATETC, LPSTGMEDIUM, BOOL);
        STDMETHODIMP EnumFormatEtc(DWORD, LPENUMFORMATETC *);
        STDMETHODIMP DAdvise(LPFORMATETC, DWORD
            ,  LPADVISESINK, DWORD *);
        STDMETHODIMP DUnadvise(DWORD);
        STDMETHODIMP EnumDAdvise(LPENUMSTATDATA *);
    };


class CEnumFormatEtc : public IEnumFORMATETC
    {
    private:
        ULONG           m_cRef;
        LPUNKNOWN       m_pUnkRef;
        LPFORMATETC     m_prgfe;
        ULONG           m_iCur;
        ULONG           m_cItems;

    public:
        CEnumFormatEtc(LPUNKNOWN);
        ~CEnumFormatEtc(void);

        BOOL FInit(HWND);

        //IUnknown members that delegate to m_pUnkOuter.
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

#endif //_DATAOBJ_H_
