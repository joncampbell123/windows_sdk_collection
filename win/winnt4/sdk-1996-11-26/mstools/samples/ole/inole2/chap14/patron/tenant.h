/*
 * TENANT.H
 * Patron Chapter 14
 *
 * Definitions and function prototypes for the CTenant class
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _TENANT_H_
#define _TENANT_H_


class CImpIOleClientSite : public IOleClientSite
    {
    protected:
        ULONG               m_cRef;
        class CTenant      *m_pTen;
        LPUNKNOWN           m_pUnkOuter;

    public:
        CImpIOleClientSite(class CTenant *, LPUNKNOWN);
        ~CImpIOleClientSite(void);

        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP SaveObject(void);
        STDMETHODIMP GetMoniker(DWORD, DWORD, LPMONIKER *);
        STDMETHODIMP GetContainer(LPOLECONTAINER *);
        STDMETHODIMP ShowObject(void);
        STDMETHODIMP OnShowWindow(BOOL);
        STDMETHODIMP RequestNewObjectLayout(void);
    };

typedef CImpIOleClientSite *PIMPIOLECLIENTSITE;



class CImpIAdviseSink : public IAdviseSink2
    {
    protected:
        ULONG               m_cRef;
        class CTenant      *m_pTen;
        LPUNKNOWN           m_pUnkOuter;

    public:
        CImpIAdviseSink(class CTenant *, LPUNKNOWN);
        ~CImpIAdviseSink(void);

        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
        STDMETHODIMP_(void)  OnViewChange(DWORD, LONG);
        STDMETHODIMP_(void)  OnRename(LPMONIKER);
        STDMETHODIMP_(void)  OnSave(void);
        STDMETHODIMP_(void)  OnClose(void);
        STDMETHODIMP_(void)  OnLinkSrcChange(LPMONIKER);
    };


typedef CImpIAdviseSink *PIMPIADVISESINK;



/*
 * Tenant class describing an individual piece of data in a page.
 * It knows where it sits, what object is inside of it, and what
 * its idenitifer is such that it can find it's storage within a
 * page.
 */

//Patron Objects clipboard format
typedef struct tagPATRONOBJECT
    {
    POINTL      ptl;        //Location of object
    POINTL      ptlPick;    //Pick point from drag-drop operation
    SIZEL       szl;        //Extents of object (absolute)
    FORMATETC   fe;         //Actual object format
    } PATRONOBJECT, *PPATRONOBJECT;



//Values for hit-testing, drawing, and resize-tracking tenants
#define CXYHANDLE       5

//Tenant types (not persistent, but determined at load time)
typedef enum
    {
    TENANTTYPE_NULL=0,
    TENANTTYPE_STATIC,
    TENANTTYPE_EMBEDDEDOBJECT,
    TENANTTYPE_EMBEDDEDFILE,
    TENANTTYPE_EMBEDDEDOBJECTFROMDATA,
    TENANTTYPE_LINKEDOBJECT,
    TENANTTYPE_LINKEDFILE,
    TENANTTYPE_LINKEDOBJECTFROMDATA
    } TENANTTYPE, *PTENANTTYPE;


//State flags
#define TENANTSTATE_DEFAULT      0x00000000
#define TENANTSTATE_SELECTED     0x00000001
#define TENANTSTATE_OPEN         0x00000002
#define TENANTSTATE_SHOWTYPE     0x00000004


/*
 * Persistent information we need to save for each tenant, which is
 * done in the tenant list instead of with each tenant.  Since this
 * is a small structure it's best not to blow another stream for it
 * (overhead).
 */
typedef struct tagTENANTINFO
    {
    DWORD       dwID;
    RECTL       rcl;
    FORMATETC   fe;             //Excludes ptd
    short       fSetExtent;     //Call IOleObject::SetExtent on Run
    } TENANTINFO, *PTENANTINFO;


class CTenant : public IUnknown
    {
    friend CImpIOleClientSite;
    friend CImpIAdviseSink;

    private:
        HWND            m_hWnd;         //Pages window
        DWORD           m_dwID;         //Persistent DWORD ID
        DWORD           m_cOpens;       //Count calls to FOpen

        BOOL            m_fInitialized; //Something here?
        LPUNKNOWN       m_pObj;         //The object here
        LPSTORAGE       m_pIStorage;    //Sub-storage for tenant

        FORMATETC       m_fe;           //Used to create the object
        DWORD           m_dwState;      //State flags
        RECTL           m_rcl;          //Space of this object
        CLSID           m_clsID;        //Object class (for statics)
        BOOL            m_fSetExtent;   //Call SetExtent on next run

        class CPages   *m_pPG;          //Pages window

        TENANTTYPE      m_tType;            //Type identifier
        ULONG           m_cRef;             //We're an object now
        LPOLEOBJECT     m_pIOleObject;      //IOleObject on m_pObj
        LPVIEWOBJECT2   m_pIViewObject2;    //IViewObject2 on m_pObj
        ULONG           m_grfMisc;          //OLEMISC flags

        //Our interfaces
        PIMPIOLECLIENTSITE  m_pIOleClientSite;
        PIMPIADVISESINK     m_pIAdviseSink;

        LPMONIKER       m_pmk;          //Relative moniker
        LPMONIKER       m_pmkFile;      //Container document moniker

        //CHAPTER14MOD
        BOOL            m_fRepaintEnabled;  //No redundant paints
        //End CHAPTER14MOD

    /*
     * This flag is used exculsively by the implementation of
     * IOleUILinkContainer on the page we're in for the Links
     * Dialog. Since we never use it ourselves, public here
     * is no big deal.
     */
    public:
        BOOL            m_fLinkAvail;

    protected:
        BOOL    FObjectInitialize(LPUNKNOWN, LPFORMATETC, DWORD);
        HRESULT CreateStatic(LPDATAOBJECT, LPFORMATETC
            , LPUNKNOWN *);

    public:
        CTenant(DWORD, HWND, CPages *);
        ~CTenant(void);

        //Gotta have an IUnknown for delegation.
        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        DWORD       GetID(void);
        UINT        GetStorageName(LPTSTR);
        UINT        UCreate(TENANTTYPE, LPVOID, LPFORMATETC, LPPOINTL
                        , LPSIZEL, LPSTORAGE, PPATRONOBJECT, DWORD);
        BOOL        FLoad(LPSTORAGE, PTENANTINFO);
        void        GetInfo(PTENANTINFO);
        BOOL        FOpen(LPSTORAGE);
        void        Close(BOOL);
        BOOL        Update(void);
        void        Destroy(LPSTORAGE);

        void        Select(BOOL);
        void        ShowAsOpen(BOOL);
        void        ShowYourself(void);
        void        AddVerbMenu(HMENU, UINT);
        void        CopyEmbeddedObject(LPDATAOBJECT, LPFORMATETC
                        , LPPOINTL);
        void        CopyLinkedObject(LPDATAOBJECT, LPFORMATETC
                        , LPPOINTL);
        void        NotifyOfRename(LPTSTR, LPMONIKER, LPMONIKER);
        void        StorageGet(LPSTORAGE *);
        void        ShowObjectType(BOOL);
        BOOL        Activate(LONG);
        void        Draw(HDC, DVTARGETDEVICE *, HDC, int, int
                        , BOOL, BOOL);
        void        Repaint(void);
        void        Invalidate(void);

        TENANTTYPE  TypeGet(void);
        BOOL        FIsSelected(void);
        BOOL        FConvertToStatic(void);

        //CHAPTER14MOD
        void        ObjectClassFormatAndIcon(LPCLSID, LPWORD
                        , LPTSTR *, HGLOBAL *);
        BOOL        FSwitchOrUpdateAspect(HGLOBAL, BOOL);
        void        EnableRepaint(BOOL);
        //End CHAPTER14MOD

        void        ObjectGet(LPUNKNOWN *);
        void        FormatEtcGet(LPFORMATETC, BOOL);
        void        SizeGet(LPSIZEL, BOOL);
        void        SizeSet(LPSIZEL, BOOL, BOOL);
        void        RectGet(LPRECTL, BOOL);
        void        RectSet(LPRECTL, BOOL, BOOL);
    };


typedef CTenant *PCTenant;

//Return codes for UCreate
#define UCREATE_FAILED              0
#define UCREATE_GRAPHICONLY         1
#define UCREATE_PLACEDOBJECT        2



#endif //_TENANT_H_
