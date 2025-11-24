/*
 * PAGES.H
 * Patron Chapter 7
 *
 * Definitions and function prototypes for the Pages window control
 * as well as the CPage and CTenant classes.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _PAGES_H_
#define _PAGES_H_

//We need this for UICursorLoad and new-style cursors.
#include <bttncur.h>


//Versioning.
#define VERSIONMAJOR                2
#define VERSIONMINOR                0
#define VERSIONCURRENT              0x00020000

//Classname
#define SZCLASSPAGES                TEXT("pages")

#define HIMETRIC_PER_INCH           2540
#define LOMETRIC_PER_INCH           254
#define LOMETRIC_BORDER             60          //Border around page


//Window extra bytes and offsets
#define CBPAGESWNDEXTRA             (sizeof(LONG))
#define PAGEWL_STRUCTURE            0



//CHAPTER7MOD

/*
 * Tenant class describing an individual piece of data in a page.
 * It knows where it sits, what object is inside of it, and what
 * its identifer is such that it can find it's storage within a
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

//Tenant creation types (not persistent)
typedef enum
    {
    TENANTTYPE_NULL=0,
    TENANTTYPE_STATIC,
    } TENANTTYPE, *PTENANTTYPE;


//State flags
#define TENANTSTATE_DEFAULT      0x00000000
#define TENANTSTATE_SELECTED     0x00000001


/*
 * Persistent information we need to save for each tenant, which is
 * done in the tenant list instead of with each tenant.  Since this
 * is a small structure it's best not to blow another stream for it
 * (overhead).  (fSetExtent used in compound documents later on.)
 */
typedef struct tagTENANTINFO
    {
    DWORD       dwID;
    RECTL       rcl;
    FORMATETC   fe;             //Excludes ptd
    short       fSetExtent;     //Call IOleObject::SetExtent on Run
    } TENANTINFO, *PTENANTINFO;


class CTenant
    {
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

    protected:
        HRESULT CreateStatic(LPDATAOBJECT, LPFORMATETC
            , LPUNKNOWN *);

    public:
        CTenant(DWORD, HWND, CPages *);
        ~CTenant(void);

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
        BOOL        Activate(LONG);
        void        Draw(HDC, DVTARGETDEVICE *, HDC, int, int
                        , BOOL, BOOL);
        void        Repaint(void);
        void        Invalidate(void);

        void        ObjectGet(LPUNKNOWN *);
        void        FormatEtcGet(LPFORMATETC, BOOL);
        void        SizeGet(LPSIZEL, BOOL);
        void        SizeSet(LPSIZEL, BOOL);
        void        RectGet(LPRECTL, BOOL);
        void        RectSet(LPRECTL, BOOL);
    };


typedef CTenant *PCTenant;

//Return codes for UCreate
#define UCREATE_FAILED              0
#define UCREATE_GRAPHICONLY         1
#define UCREATE_PLACEDOBJECT        2



typedef struct tagTENANTLIST
    {
    DWORD       cTenants;
    DWORD       dwIDNext;
    } TENANTLIST, *PTENANTLIST;

#define SZSTREAMTENANTLIST        OLESTR("Tenant List")

//Delay timer used in mouse debouncing
#define IDTIMER_DEBOUNCE          120
//End CHAPTER7MOD



/*
 * Page class describing an individual page and what things it
 * contains, managing an IStorage for us.
 *
 * A DWORD is used to identify this page as the name of the storage
 * is the string form of this ID.  If we added a page every second,
 * it would take 136 years to overrun this counter, so we can
 * get away with saving it persistently.  I hope this software is
 * obsolete by then.
 */

class CPage
    {
    private:
        DWORD       m_dwID;             //Persistent identifier
        LPSTORAGE   m_pIStorage;        //Substorage for this page
        //CHAPTER7MOD
        HWND        m_hWnd;             //Pages window
        DWORD       m_cOpens;           //Calls to FOpen

        class CPages *m_pPG;            //Pages window

        DWORD       m_dwIDNext;
        DWORD       m_cTenants;
        HWND        m_hWndTenantList;   //Listbox; our tenant list

        UINT        m_iTenantCur;
        PCTenant    m_pTenantCur;

        UINT        m_uHTCode;          //Last hit-test/mouse move
        UINT        m_uSizingFlags;     //Restrictions on sizing
        BOOL        m_fTracking;        //Tracking resize?
        RECTL       m_rclOrg;           //Original before tracking
        RECTL       m_rcl;              //Tracking rectangle
        RECTL       m_rclBounds;        //Boundaries f/size tracking
        HDC         m_hDC;              //Tracking hDC

        BOOL        m_fSizePending;     //Waiting for debounce?
        int         m_cxyDist;          //Debounce distance
        UINT        m_cDelay;           //Debounce delay
        POINTS      m_ptDown;           //Point of click to debounce
        DWORD       m_fTimer;           //Timer active?

    protected:
        BOOL         FTenantGet(UINT, PCTenant *, BOOL);
        BOOL         FTenantAdd(UINT, DWORD, PCTenant *);
        LPDATAOBJECT TransferObjectCreate(LPPOINTL);

        //PAGEMOUS.CPP
        UINT         TenantFromPoint(UINT, UINT, PCTenant *);
        //End CHAPTER7MOD

    public:
        //CHAPTER7MOD
        CPage(DWORD, HWND, class CPages *);
        //End CHAPTER7MOD
        ~CPage(void);

        DWORD       GetID(void);
        BOOL        FOpen(LPSTORAGE);
        void        Close(BOOL);
        BOOL        Update(void);
        void        Destroy(LPSTORAGE);
        UINT        GetStorageName(LPTSTR);

        //CHAPTER7MOD
        void        Draw(HDC, int, int, BOOL, BOOL);

        BOOL        TenantCreate(TENANTTYPE, LPVOID, LPFORMATETC
                        , PPATRONOBJECT, DWORD);
        BOOL        TenantDestroy(void);
        BOOL        TenantClip(BOOL);
        BOOL        FQueryObjectSelected(HMENU);

        //PAGEMOUSE.CPP
        BOOL        OnLeftDown(UINT, UINT, UINT);
        BOOL        OnLeftDoubleClick(UINT, UINT, UINT);
        BOOL        OnLeftUp(UINT, UINT, UINT);
        void        OnMouseMove(UINT, int, int);
        void        OnTimer(UINT);
        void        StartSizeTracking(void);
        void        OnNCHitTest(UINT, UINT);
        BOOL        OnSetCursor(UINT);
        //End CHAPTER7MOD
    };

typedef CPage *PCPage;



/*
 * Structures to save with the document describing the device
 * configuration and pages that we have.  This is followed by
 * a list of DWORD IDs for the individual pages.
 */

typedef struct tagDEVICECONFIG
    {
    DWORD       cb;                         //Size of structure
    TCHAR       szDriver[CCHDEVICENAME];
    TCHAR       szDevice[CCHDEVICENAME];
    TCHAR       szPort[CCHDEVICENAME];
    DWORD       cbDevMode;                  //Size of actual DEVMODE
    DEVMODE     dm;                         //Variable
    } DEVICECONFIG, *PDEVICECONFIG;

//Offset to cbDevMode
#define CBSEEKOFFSETCBDEVMODE  (sizeof(DWORD)   \
                               +(3*CCHDEVICENAME*sizeof(TCHAR)))


//CHAPTER7MOD
//Combined OLE and Patron device structures.
typedef struct tagCOMBINEDEVICE
    {
    DVTARGETDEVICE  td;
    DEVICECONFIG    dc;
    } COMBINEBDEVICE, *PCOMBINEDEVICE;
//End CHAPTER7MOD


typedef struct tagPAGELIST
    {
    DWORD       cPages;
    DWORD       iPageCur;
    DWORD       dwIDNext;
    } PAGELIST, *PPAGELIST;


//PAGEWIN.CPP
LRESULT APIENTRY PagesWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL    APIENTRY AbortProc(HDC, int);
BOOL    APIENTRY PrintDlgProc(HWND, UINT, WPARAM, LPARAM);
void             RectConvertMappings(LPRECT, HDC, BOOL);


class CPages : public CWindow
    {
    friend LRESULT APIENTRY PagesWndProc(HWND, UINT, WPARAM, LPARAM);
    friend BOOL    APIENTRY PrintDlgProc(HWND, UINT, WPARAM, LPARAM);

    friend class CPage;
    friend class CTenant;

    protected:
        //CHAPTER7MOD
        PCPage      m_pPageCur;             //Current page
        //End CHAPTER7MOD
        UINT        m_iPageCur;             //Current page
        UINT        m_cPages;               //Number of pages

        HWND        m_hWndPageList;         //Listbox with page list
        HFONT       m_hFont;                //Page font
        BOOL        m_fSystemFont;          //m_hFont system object?

        UINT        m_cx;                   //Page size in LOMETRIC
        UINT        m_cy;

        UINT        m_xMarginLeft;          //Unusable margins,
        UINT        m_xMarginRight;         //in LOMETRIC
        UINT        m_yMarginTop;
        UINT        m_yMarginBottom;

        UINT        m_xPos;                 //Viewport scroll pos,
        UINT        m_yPos;                 //both in *PIXELS*

        DWORD       m_dwIDNext;             //Next ID for a page.
        LPSTORAGE   m_pIStorage;            //Root storage

        //CHAPTER7MOD
        UINT        m_cf;                   //Clipboard format
        BOOL        m_fDirty;
        //End CHAPTER7MOD

    protected:
        void      Draw(HDC, BOOL, BOOL);
        void      UpdateScrollRanges(void);
        BOOL      ConfigureForDevice(void);
        BOOL      FPageGet(UINT, PCPage *, BOOL);
        BOOL      FPageAdd(UINT, DWORD, BOOL);

        //CHAPTER7MOD
        void      CalcBoundingRect(LPRECT, BOOL);
        //End CHAPTER7MOD

    public:
        //CHAPTER7MOD
        CPages(HINSTANCE, UINT);
        //End CHAPTER7MOD
        ~CPages(void);

        BOOL      FInit(HWND, LPRECT, DWORD, UINT, LPVOID);

        BOOL      FIStorageSet(LPSTORAGE, BOOL, BOOL);
        BOOL      FIStorageUpdate(BOOL);

        BOOL      Print(HDC, LPTSTR, DWORD, UINT, UINT, UINT);

        void      RectGet(LPRECT);
        void      RectSet(LPRECT, BOOL);
        void      SizeGet(LPRECT);
        void      SizeSet(LPRECT, BOOL);

        UINT      PageInsert(UINT);
        UINT      PageDelete(UINT);
        UINT      CurPageGet(void);
        UINT      CurPageSet(UINT);
        UINT      NumPagesGet(void);

        BOOL      DevModeSet(HGLOBAL, HGLOBAL);
        HGLOBAL   DevModeGet(void);

        //CHAPTER7MOD
        BOOL      FIsDirty(void);
        BOOL      DevReadConfig(PCOMBINEDEVICE *, HDC *);
        BOOL      TenantCreate(TENANTTYPE, LPVOID, LPFORMATETC
                      , PPATRONOBJECT, DWORD);
        BOOL      TenantDestroy(void);
        BOOL      TenantClip(BOOL);
        BOOL      FQueryObjectSelected(HMENU);
        //End CHAPTER7MOD
    };

typedef CPages *PCPages;


//Fixed names of streams in the Pages IStorage
#define SZSTREAMPAGELIST        OLESTR("Page List")
#define SZSTREAMDEVICECONFIG    OLESTR("Device Configuration")


#endif  //_PAGES_H_
